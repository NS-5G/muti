/*
 * Object.c
 *
 *  Created on: Jan 12, 2021
 *      Author: root
 */
#include <os/server/object/ObjectHandler.h>
#include <string.h>

#include <stdbool.h>
#include <os/share/Object.h>
#include <server/Server.h>
#include "../ServerContextOs.h"
#include <network/Connection.h>
#include <network/Socket.h>
#include <Log.h>

Action ObjectActions[] = {
        ObjectActionGet,
        ObjectActionPut,
        ObjectActionList,
};

void ObjectActionGet(SRequest *req) {
        ObjectGetRequest *request = (ObjectGetRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextOs *sctx = server->m->getContext(server);
        (void)sctx;
        Object* object = NULL;
        Response *resp;
        if (object == NULL) {
                resp = calloc(1, sizeof(*resp));
                resp->sequence = request->super.sequence;
                resp->error_id = -1;
        } else {
                ObjectGetResponse *resp1 = calloc(1, sizeof(*resp1) + object->path_length);
                resp = &resp1->super;
                resp->sequence = request->super.sequence;
                resp->error_id = 0;
                memcpy(&resp1->object, object, sizeof(*object) + object->path_length);
        }
        req->response = resp;
        req->action_callback(req);
}

void ObjectActionPut(SRequest *req) {
        ObjectPutRequest *request = (ObjectPutRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextOs *sctx = server->m->getContext(server);
        Response *resp;
        (void)sctx;
        resp = calloc(1, sizeof(*resp));
        resp->sequence = request->super.sequence;
        resp->error_id = -1;

        req->response = resp;
        req->action_callback(req);
}

void ObjectActionList(SRequest *req) {
        ObjectListRequest *request = (ObjectListRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextOs *sctx = server->m->getContext(server);
        ObjectListResponse *resp = malloc(sizeof(*resp));
        (void)sctx;

        resp->length = 0;

        resp->super.sequence = request->super.sequence;
        resp->super.error_id = 0;

        req->response = &resp->super;
        req->action_callback(req);
}

RequestDecoder ObjectRequestDecoder[] = {
        ObjectRequestDecoderGet,
        ObjectRequestDecoderPut,
        ObjectRequestDecoderList,
};

Request* ObjectRequestDecoderGet(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        size_t req_len;
        ObjectGetRequest *req = (ObjectGetRequest*)buffer;
        req_len = sizeof(ObjectGetRequest);
        if (buff_len < req_len) return NULL;
        *consume_len = req_len;
        *free_req = false;
        return &req->super;
}

Request* ObjectRequestDecoderPut(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        size_t req_len;
        ObjectPutRequest *req = (ObjectPutRequest*)buffer;
        req_len = sizeof(ObjectPutRequest);
        if (buff_len < req_len) return NULL;
        req_len += req->object.path_length;
        if (buff_len < req_len) return NULL;
        req->object.path = req->object.data;
        *consume_len = req_len;
        *free_req = false;
        return &req->super;
}

Request* ObjectRequestDecoderList(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        size_t req_len;
        ObjectListRequest *req = (ObjectListRequest*)buffer;
        req_len = sizeof(ObjectListRequest);
        if (buff_len < req_len) return NULL;
        *consume_len = req_len;
        *free_req = false;
        return &req->super;
}

ResponseEncoder ObjectResponseEncoder[] = {
        ObjectResponseEncoderGet,
        ObjectResponseEncoderPut,
        ObjectResponseEncoderList,
};

bool ObjectResponseEncoderGet(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        *buffer = (char*)resp;
        ObjectGetResponse *get_resp = (ObjectGetResponse*)resp;
        *buff_len = sizeof(ObjectGetResponse) + get_resp->object.path_length;
        *free_resp = false;
        return true;
}

bool ObjectResponseEncoderPut(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        *buffer = (char*)resp;
        *buff_len = sizeof(ObjectPutResponse);
        *free_resp = false;
        return true;
}

bool ObjectResponseEncoderList(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        ObjectListResponse *list_resp = (ObjectListResponse*)resp;
        size_t buf_len = 0;
        char *buf;

        buf = malloc(sizeof(Response) + sizeof(list_resp->length) + list_resp->length * (sizeof(Object) + 1024));
        *buffer = buf;

        *(int32_t*)buf = resp->error_id;
        buf += 4;
        buf_len += 4;

        *(uint32_t*)buf = resp->sequence;
        buf += 4;
        buf_len += 4;

        *(uint32_t*)buf = list_resp->length;
        buf += sizeof(uint32_t);
        buf_len += sizeof(uint32_t);

        int i;
        ListHead *head = list_resp->object_list;
        for (i = 0; i < list_resp->length; i++) {
                Object *s = listFirstEntry(head, Object, element);
                size_t blen = sizeof(*s) + s->path_length;
                memcpy(buf, s, blen);
                buf += blen;
                buf_len += blen;
                head = &s->element;
        }

        *buff_len = buf_len;
        *free_resp = true;
        return true;
}

