/*
 * ClusterHandler.c
 *
 *  Created on: Jan 12, 2021
 *      Author: Zhen Xiong
 */
#include <mon/server/cluster/ClusterHandler.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include <mon/share/Cluster.h>
#include <server/Server.h>
#include <network/Connection.h>
#include <network/Socket.h>
#include <Log.h>
#include <mon/server/ServerContextMon.h>

Action ClusterActions[] = {
        ClusterActionGetObjectServiceMapLatestVersion,
        ClusterActionGetLatestObjectServiceMap,
        ClusterActionAddObjectService,
        ClusterActionRemoveObjectService,
        ClusterActionKeepAliveObjectService,
        ClusterActionKeepAliveClient,
        ClusterActionGetObjectServiceMapChangeLog,
        ClusterActionStatus,
	ClusterActionStop,
};

void ClusterActionGetObjectServiceMapLatestVersion(SRequest *req) {

}

void ClusterActionGetLatestObjectServiceMap(SRequest *req) {

}

void ClusterActionAddObjectService(SRequest *req) {

}

void ClusterActionRemoveObjectService(SRequest *req) {

}

void ClusterActionKeepAliveObjectService(SRequest *req) {

}

void ClusterActionKeepAliveClient(SRequest *req) {

}

void ClusterActionGetObjectServiceMapChangeLog(SRequest *req) {

}

void ClusterActionStatus(SRequest *req) {

}

static void* clusterActionStopThread(void *p) {
        ServerContextMon *sctx = p;
        sleep(2);
        sem_post(&sctx->stop_sem);
        return NULL;
}

void ClusterActionStop(SRequest *req) {
        ClusterStopRequest *request = (ClusterStopRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextMon *sctx = server->m->getContext(server);

        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&tid, &attr, clusterActionStopThread, sctx);

        ClusterStopResponse *resp = malloc(sizeof(*resp));
        resp->super.sequence = request->super.sequence;
        resp->super.error_id = 0;
        req->response = &resp->super;
        req->action_callback(req);
}

RequestDecoder ClusterRequestDecoder[] = {
        ClusterRequestDecoderGetObjectServiceMapLatestVersion,
        ClusterRequestDecoderGetLatestObjectServiceMap,
        ClusterRequestDecoderAddObjectService,
        ClusterRequestDecoderRemoveObjectService,
        ClusterRequestDecoderKeepAliveObjectService,
        ClusterRequestDecoderKeepAliveClient,
        ClusterRequestDecoderGetObjectServiceMapChangeLog,
        ClusterRequestDecoderStatus,
	ClusterRequestDecoderStop,
};

Request* ClusterRequestDecoderGetObjectServiceMapLatestVersion(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}
Request* ClusterRequestDecoderGetLatestObjectServiceMap(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderAddObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderRemoveObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderKeepAliveObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderKeepAliveClient(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderGetObjectServiceMapChangeLog(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderStatus(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderStop(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        ClusterStopRequest *req = (ClusterStopRequest*)buffer;
        if (buff_len < sizeof(req)) return NULL;
        *consume_len = sizeof(req);
        *free_req = false;
        return &req->super;
}

ResponseEncoder ClusterResponseEncoder[] = {
        ClusterResponseEncoderGetObjectServiceMapLatestVersion,
        ClusterResponseEncoderGetLatestObjectServiceMap,
        ClusterResponseEncoderAddObjectService,
        ClusterResponseEncoderRemoveObjectService,
        ClusterResponseEncoderKeepAliveObjectService,
        ClusterResponseEncoderKeepAliveClient,
        ClusterResponseEncoderGetObjectServiceMapChangeLog,
        ClusterResponseEncoderStatus,
	ClusterResponseEncoderStop,
};

bool ClusterResponseEncoderGetObjectServiceMapLatestVersion(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderGetLatestObjectServiceMap(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderAddObjectService(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderRemoveObjectService(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderKeepAliveObjectService(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderKeepAliveClient(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderGetObjectServiceMapChangeLog(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderStatus(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderStop(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        ClusterStopResponse *resp1 = (ClusterStopResponse*)resp;
        *buffer = (char *)resp1;
        *buff_len = sizeof(resp1);
        *free_resp = false;
        return true;
}


