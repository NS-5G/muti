/*
 * Client.c
 *
 *  Created on: Feb 6, 2021
 *      Author: Zhen Xiong
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#include <client/Client.h>
#include <client/RequestSender.h>
#include <Log.h>
#include <util/LinkedList.h>

#define CLIENT_WAITING_HASH_LEN (128)

typedef struct ClientPrivate {
        ClientParam             param;
        Connection              *conn;
        void                    *context;
        Socket                  socket;
        sem_t                   sem;
        volatile uint32_t       sequence;
        pthread_mutex_t         waiting_hash_locks[CLIENT_WAITING_HASH_LEN];
        ListHead                waiting_hash_slots[CLIENT_WAITING_HASH_LEN];
} ClientPrivate;

typedef struct CConnectionContext {
        volatile uint64_t       read_counter;
        volatile uint64_t       read_done_counter;
        volatile uint64_t       write_counter;
        volatile uint64_t       write_done_counter;
} CConnectionContext;

typedef struct ClientSendArg {
        union {
                ListElement     element;
                Job             job;
        };
        volatile int            ref;

        Readbuffer              *rbuf;
        Response                *response;
        bool                    free_resp;
        uint32_t                sequence;

        char                    *wbuf;
        size_t                  wbuf_len;
        ResponseDecoder         decoder;

        Connection              *conn;
        ClientSendCallback      callback;
        void                    *arg;
} ClientSendArg;

static void clientOnClose(Connection *conn) {
        Socket *socket = conn->m->getSocket(conn);
        CConnectionContext *ccxt = conn->m->getContext(conn);
        Client* this = socket->m->getContext(socket);
        ClientPrivate *priv_p = this->p;
        ListHead *slot;
        pthread_mutex_t *lock;
        ClientSendArg *send_arg, *arg1;
        Response err_resp;
        int i;

        priv_p->conn = NULL;

        while (ccxt->read_counter != ccxt->read_done_counter) {
                WLOG("Waiting read response, request counter:%lu, response counter:%lu.", ccxt->read_counter, ccxt->read_done_counter);
                usleep(1000);
        }
        while (ccxt->write_counter != ccxt->write_done_counter) {
                WLOG("Waiting write response, request counter:%lu, response counter:%lu.", ccxt->write_counter, ccxt->write_done_counter);
                usleep(1000);
        }
        free(ccxt);

        err_resp.error_id = -4;
        err_resp.sequence = (uint32_t)-1;
        for (i = 0; i < CLIENT_WAITING_HASH_LEN; i++) {
                slot = &priv_p->waiting_hash_slots[i];
                lock = &priv_p->waiting_hash_locks[i];
                pthread_mutex_lock(lock);
                listForEachEntrySafe(send_arg, arg1, slot, element) {
                        listDel(&send_arg->element);
                        send_arg->callback(this, &err_resp, send_arg->arg, NULL, NULL, NULL);
                        if (send_arg->wbuf) {
                                free(send_arg->wbuf);
                                send_arg->wbuf = NULL;
                        }
                        free(send_arg);
                }
                pthread_mutex_unlock(lock);
        }
        DLOG("clientOnClose");
}

static Readbuffer* clientCreateReadBuffer(ClientPrivate *priv_p) {
        Readbuffer *rbuf = malloc(sizeof(*rbuf));

        rbuf->buffer = malloc(priv_p->param.read_buffer_size);
        rbuf->read_buffer_start = 0;
        rbuf->processed_buffer_start = 0;
        rbuf->reference = 1;
        return rbuf;
}

static inline void clientFreeReadBuffer(Readbuffer *rbuf, Connection* conn_p) {
        CConnectionContext *ccxt;
        uint64_t left = __sync_sub_and_fetch(&rbuf->reference, 1);
        if (left == 0) {
                free(rbuf->buffer);
                free(rbuf);
                ccxt = conn_p->m->getContext(conn_p);
                __sync_add_and_fetch(&ccxt->read_done_counter, 1);
        }
}


static void clientReadCallback(Connection* conn_p, bool rc, void* buffer, size_t sz, void *cbarg);

static inline void clientRead(Connection* conn_p, ClientPrivate *priv_p, Readbuffer *old_rbuf, char *buf, size_t buf_len) {
        CConnectionContext *ccxt = conn_p->m->getContext(conn_p);
        Readbuffer *rbuf;

        if (old_rbuf->processed_buffer_start + buf_len < priv_p->param.read_buffer_size) {
                rbuf = old_rbuf;
                rbuf->read_buffer_start = old_rbuf->processed_buffer_start + buf_len;
        } else {
                rbuf = clientCreateReadBuffer(priv_p);
                memcpy(rbuf->buffer, buf, buf_len);
                rbuf->read_buffer_start = buf_len;
                clientFreeReadBuffer(old_rbuf, conn_p);
                __sync_add_and_fetch(&ccxt->read_counter, 1);
        }

        bool rrc = conn_p->m->read(conn_p, rbuf->buffer + rbuf->read_buffer_start,
                        priv_p->param.read_buffer_size - rbuf->read_buffer_start,
                        clientReadCallback, rbuf);
        if (rrc == false) {
                clientFreeReadBuffer(rbuf, conn_p);
//                clientFreeReadBuffer(rbuf, conn_p);
        }
}

static void clientFreeResp(void *arg) {
        ClientSendArg *send_arg = arg;
        Connection *conn_p = send_arg->conn;
        Readbuffer *rbuf = send_arg->rbuf;

        clientFreeReadBuffer(rbuf, conn_p);
        if (send_arg->free_resp) {
                free(send_arg->response);
        }
        int left = __sync_sub_and_fetch(&send_arg->ref, 1);
        if (left == 0) {
                free(send_arg);
        }
}

static void clientDoJob(Job *job) {
        ClientSendArg *send_arg = containerOf(job, ClientSendArg, job);
        Connection *conn_p = send_arg->conn;
        Socket* skt = conn_p->m->getSocket(conn_p);
        Client* this = skt->m->getContext(skt);
        bool free_resp = true;

        send_arg->callback(this, send_arg->response, send_arg->arg, &free_resp, clientFreeResp, send_arg);
        if (free_resp) {
                Readbuffer *rbuf = send_arg->rbuf;
                clientFreeReadBuffer(rbuf, conn_p);

                if (send_arg->free_resp) {
                        free(send_arg->response);
                }
                int left = __sync_sub_and_fetch(&send_arg->ref, 1);
                if (left == 0) {
                        free(send_arg);
                }
        }
}

static void clientReadCallback(Connection* conn_p, bool rc, void* buffer, size_t sz, void *cbarg) {
        Socket* skt = conn_p->m->getSocket(conn_p);
        Client* this = skt->m->getContext(skt);
        ClientPrivate *priv_p = this->p;
        ThreadPool *work_tp = priv_p->param.worker_tp;
        Readbuffer *rbuf = cbarg;
        char *buf;
        size_t buf_len;

        if (rc == false) {
                clientFreeReadBuffer(rbuf, conn_p);
                return;
        }

        buf = rbuf->buffer + rbuf->processed_buffer_start;
        buf_len = rbuf->read_buffer_start - rbuf->processed_buffer_start + sz;
        __sync_add_and_fetch(&rbuf->reference, 1);

        while(true) {
                if (buf_len < sizeof(Response)) {
                        clientRead(conn_p, priv_p, rbuf, buf, buf_len);
                        break;
                }
                ClientSendArg *send_arg, *send_arg1;
                Response *resp = (Response*)buf;
                uint32_t hash = resp->sequence & (CLIENT_WAITING_HASH_LEN - 1);
                pthread_mutex_t *lock = &priv_p->waiting_hash_locks[hash];
                ListHead *slot = &priv_p->waiting_hash_slots[hash];
                bool got_it = false;
                pthread_mutex_lock(lock);
                listForEachEntrySafe(send_arg, send_arg1, slot, element) {
                        if (send_arg->sequence == resp->sequence) {
                                got_it = true;
                                listDel(&send_arg->element);
                                break;
                        }
                }
                pthread_mutex_unlock(lock);
                if (got_it == false) {
                        rc = false;
                        goto out;
                }

                size_t consume_len;
                bool free_resp;
                if (resp->error_id) {
                        resp = ErrorResponseDecoder(buf, buf_len, &consume_len, &free_resp);
                } else {
                        resp = send_arg->decoder(conn_p, buf, buf_len, &consume_len, &free_resp);
                }
                if (resp) {
                        buf += consume_len;
                        buf_len -= consume_len;
                        rbuf->processed_buffer_start += consume_len;
                        __sync_add_and_fetch(&rbuf->reference, 1);
                        send_arg->rbuf = rbuf;
                        send_arg->response = resp;
                        send_arg->free_resp = free_resp;
                        send_arg->job.doJob = clientDoJob;
                        send_arg->conn = conn_p;
                        work_tp->m->insertTail(work_tp, &send_arg->job);
                } else {
                        pthread_mutex_lock(lock);
                        listAdd(&send_arg->element, slot);
                        pthread_mutex_unlock(lock);
                        clientRead(conn_p, priv_p, rbuf, buf, buf_len);
                        break;
                }
        }
out:
        clientFreeReadBuffer(rbuf, conn_p);
        if (rc == false) {
                clientFreeReadBuffer(rbuf, conn_p);
                conn_p->m->close(conn_p);
        }
}

static void clientOnConnect(Connection *conn) {
        Socket *socket = conn->m->getSocket(conn);
        Client* this = socket->m->getContext(socket);
        CConnectionContext *ctx = calloc(1, sizeof(CConnectionContext));
        ClientPrivate *priv_p = this->p;
        priv_p->conn = conn;
        conn->m->setContext(conn, ctx);

        sem_post(&priv_p->sem);

        Readbuffer *rbuf = clientCreateReadBuffer(priv_p);
        ctx->read_counter ++;
        conn->m->read(conn, rbuf->buffer,
                        priv_p->param.read_buffer_size,
                        clientReadCallback, rbuf);

}

static void clientWriteCallback(Connection *conn, bool rc, void *cbarg) {
        Socket *socket = conn->m->getSocket(conn);
        Client *this = socket->m->getContext(socket);
        CConnectionContext *ccxt = conn->m->getContext(conn);
        pthread_mutex_t *lock;
        ClientPrivate *priv_p = this->p;
        ClientSendArg *send_arg = cbarg;

        if (rc == false) {
                ELOG("Write failed!");

                uint32_t hash = send_arg->sequence & (CLIENT_WAITING_HASH_LEN - 1);
                lock = &priv_p->waiting_hash_locks[hash];
                pthread_mutex_lock(lock);
                listDel(&send_arg->element);
                pthread_mutex_unlock(lock);

                Response resp;
                resp.error_id = -3;
                send_arg->callback(this, &resp, send_arg->arg, NULL, NULL, NULL);
                free(send_arg->wbuf);
                free(send_arg);
        } else {
//                DLOG("Client write success\n");
                free(send_arg->wbuf);
                send_arg->wbuf = NULL;
                int left = __sync_sub_and_fetch(&send_arg->ref, 1);
                if (left == 0) {
                        free(send_arg);
                }
        }
        __sync_add_and_fetch(&ccxt->write_done_counter, 1);
}

static bool clientSendRequest(Client* this, Request* req, ClientSendCallback callback, void *arg, bool *free_req) {
        ClientPrivate *priv_p = this->p;
        Connection *conn_p = priv_p->conn;
        if (conn_p == NULL) {
                *free_req = true;
                ELOG("Connection not create.");
                return false;
        }

        RequestSender *sender = &priv_p->param.request_sender[req->resource_id];
        RequestEncoder encoder = sender->request_encoders[req->request_id];
        ResponseDecoder decoder = sender->response_decoders[req->request_id];
        ClientSendArg *send_arg = malloc(sizeof(*send_arg));

        send_arg->callback = callback;
        send_arg->ref = 2;
        send_arg->arg = arg;
        req->sequence = __sync_add_and_fetch(&priv_p->sequence, 1);
        send_arg->sequence = req->sequence;
        send_arg->decoder = decoder;

        bool rc = encoder(conn_p, req, &send_arg->wbuf, &send_arg->wbuf_len, free_req);
        if (rc == false) {
                *free_req = true;
                free(send_arg);
                ELOG("Encoding request error!");
                return rc;
        }

        uint32_t hash = send_arg->sequence & (CLIENT_WAITING_HASH_LEN - 1);
        pthread_mutex_t *lock = &priv_p->waiting_hash_locks[hash];
        ListHead *slot = &priv_p->waiting_hash_slots[hash];
        pthread_mutex_lock(lock);
        listAddTail(&send_arg->element, slot);
        pthread_mutex_unlock(lock);

        CConnectionContext *ccxt = conn_p->m->getContext(conn_p);
        __sync_add_and_fetch(&ccxt->write_counter, 1);

        rc = conn_p->m->write(conn_p, send_arg->wbuf, send_arg->wbuf_len, clientWriteCallback, send_arg);
        if (rc == false) {
                ELOG("Send request buffer failed.");
                __sync_add_and_fetch(&ccxt->write_done_counter, 1);
                pthread_mutex_lock(lock);
                listDel(&send_arg->element);
                pthread_mutex_unlock(lock);

                if (*free_req) {
                        free(send_arg->wbuf);
                }
                *free_req = true;
                free(send_arg);
        }
        return rc;
}

typedef struct ClientSendReqSyncCbarg {
	sem_t   sem;
	int32_t error_id;
} ClientSendReqSyncCbarg;

static void clientSendRequestSyncCallbck(Client *this, Response *resp, void *p, bool *free_resp, ClientFreeResp freeResp, void *resp_ctx) {
	ClientSendReqSyncCbarg *cbargp = p;
	cbargp->error_id = resp->error_id;
	DLOG("Response error id:%d", resp->error_id);
	sem_post(&cbargp->sem);
}

static bool clientSendRequestSync(Client* this, Request* req, bool *free_req) {
	ClientSendReqSyncCbarg cbarg;

	sem_init(&cbarg.sem, 0, 0);
	bool rc = clientSendRequest(this, req, clientSendRequestSyncCallbck, &cbarg, free_req);
	if (rc == true) sem_wait(&cbarg.sem);
	else return rc;
	rc = (cbarg.error_id == 0);
	if (rc == false) {
		ELOG("Request %d got error, error id:%d", req->resource_id, cbarg.error_id);
	}
	return rc;
}

static void* clientGetContext(Client *this) {
        ClientPrivate *priv_p = this->p;
        return priv_p->context;
}

static void destroy(Client* this) {
        ClientPrivate *priv_p = this->p;
        int i;

        for (i = 0; i < CLIENT_WAITING_HASH_LEN; i++) {
                pthread_mutex_t *lock;
                lock = &priv_p->waiting_hash_locks[i];
                pthread_mutex_destroy(lock);
        }

        priv_p->socket.m->destroy(&priv_p->socket);
        free(priv_p);
}

static ClientMethod method = {
        .sendRequest = clientSendRequest,
	.sendRequestSync = clientSendRequestSync,
	.getContext = clientGetContext,
        .destroy = destroy,
};

bool initClient(Client* this, ClientParam* param) {
        ClientPrivate *priv_p = malloc(sizeof(*priv_p));
        ListHead *wlist;
        pthread_mutex_t *lock;
        int i, rc;

        this->p = priv_p;
        this->m = &method;
        memcpy(&priv_p->param, param, sizeof(*param));
        priv_p->conn = NULL;
        sem_init(&priv_p->sem, 0, 0);
        priv_p->sequence = 1022;
        for (i = 0; i < CLIENT_WAITING_HASH_LEN; i++) {
                wlist = &priv_p->waiting_hash_slots[i];
                lock = &priv_p->waiting_hash_locks[i];
                pthread_mutex_init(lock, NULL);
                listHeadInit(wlist);
        }
        priv_p->context = param->context;

        SocketLinuxParam sparam;
        strcpy(sparam.super.host, param->host);
        sparam.super.onClose = clientOnClose;
        sparam.super.onConnect = clientOnConnect;
        sparam.super.port = param->port;
        sparam.super.type = SOCKET_TYPE_CLIENT;
        sparam.read_tp = param->read_tp;
        sparam.write_tp = param->write_tp;
        sparam.super.context = this;
        rc = initSocketLinux(&priv_p->socket, &sparam);
        if (rc == false) {
                ELOG("initSocketLinux error.");
                free(priv_p);
                goto out;
        }
        sem_wait(&priv_p->sem);
        sem_destroy(&priv_p->sem);
out:
        return rc;
}


