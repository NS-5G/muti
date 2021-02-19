/*
 * Sample.c
 *
 *  Created on: Jan 12, 2021
 *      Author: root
 */
#include <string.h>

#include <server/handler/SampleRequestHandler.h>
#include <stdbool.h>
#include <res/Sample.h>
#include <server/dao/SampleDao.h>
#include <server/Server.h>
#include <server/ServerContext.h>
#include <network/Connection.h>
#include <network/Socket.h>
#include <Log.h>

Action SampleActions[] = {
        SampleActionGet,
        SampleActionPut,
        SampleActionList,
};

void SampleActionGet(SRequest *req) {
        SampleGetRequest *request = (SampleGetRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContext *sctx = server->m->getContext(server);
        SampleDao *sdao = &sctx->sampleDao;

        Sample* sample = sdao->m->getSample(sdao, request->id);
        Response *resp;
        if (sample == NULL) {
                resp = calloc(1, sizeof(*resp));
                resp->sequence = request->super.sequence;
                resp->error_id = 1;
        } else {
                SampleGetResponse *resp1 = calloc(1, sizeof(*resp1) + sample->path_length);
                resp = &resp1->super;
                resp->sequence = request->super.sequence;
                resp->error_id = 0;
                memcpy(&resp1->sample, sample, sizeof(*sample) + sample->path_length);
        }
        req->response = resp;
        req->action_callback(req);
}

void SampleActionPut(SRequest *req) {
        SamplePutRequest *request = (SamplePutRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContext *sctx = server->m->getContext(server);
        SampleDao *sdao = &sctx->sampleDao;
        Sample* sample = &request->sample;
        Response *resp;

        resp = calloc(1, sizeof(*resp));
        resp->sequence = request->super.sequence;
        resp->error_id = (int8_t)sdao->m->putSample(sdao, sample);

        req->response = resp;
        req->action_callback(req);
}

void SampleActionList(SRequest *req) {
        SampleListRequest *request = (SampleListRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContext *sctx = server->m->getContext(server);
        SampleDao *sdao = &sctx->sampleDao;
        SampleListResponse *resp = malloc(sizeof(*resp));

        resp->length = sdao->m->listSample(sdao, &resp->sample_list, request->page, request->page_size);

        resp->super.sequence = request->super.sequence;
        resp->super.error_id = 0;

        req->response = &resp->super;
        req->action_callback(req);
}

RequestDecoder SampleRequestDecoder[] = {
        SampleRequestDecoderGet,
        SampleRequestDecoderPut,
        SampleRequestDecoderList,
};

Request* SampleRequestDecoderGet(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        size_t req_len;
        SampleGetRequest *req = (SampleGetRequest*)buffer;
        req_len = sizeof(SampleGetRequest);
        if (buff_len < req_len) return NULL;
        *consume_len = req_len;
        *free_req = false;
        return &req->super;
}

Request* SampleRequestDecoderPut(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        size_t req_len;
        SamplePutRequest *req = (SamplePutRequest*)buffer;
        req_len = sizeof(SamplePutRequest) + req->sample.path_length;
        req->sample.path = req->sample.data;
        if (buff_len < req_len) return NULL;
        *consume_len = req_len;
        *free_req = false;
        return &req->super;
}

Request* SampleRequestDecoderList(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        size_t req_len;
        SampleListRequest *req = (SampleListRequest*)buffer;
        req_len = sizeof(SampleListRequest);
        if (buff_len < req_len) return NULL;
        *consume_len = req_len;
        *free_req = false;
        return &req->super;
}

ResponseEncoder SampleResponseEncoder[] = {
        SampleResponseEncoderGet,
        SampleResponseEncoderPut,
        SampleResponseEncoderList,
};

bool SampleResponseEncoderGet(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        *buffer = (char*)resp;
        SampleGetResponse *get_resp = (SampleGetResponse*)resp;
        *buff_len = sizeof(SampleGetResponse) + get_resp->sample.path_length;
        *free_resp = false;
        return true;
}

bool SampleResponseEncoderPut(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        *buffer = (char*)resp;
        *buff_len = sizeof(SamplePutResponse);
        DLOG("buffer_len:%lu", *buff_len);
        *free_resp = false;
        return true;
}

bool SampleResponseEncoderList(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        SampleListResponse *list_resp = (SampleListResponse*)resp;
        size_t buf_len = 0;
        char *buf;

        buf = malloc(sizeof(Response) + sizeof(list_resp->length) + list_resp->length * (sizeof(Sample) + 1024));
        *buffer = buf;

        *(int8_t*)buf = resp->error_id;
        buf += 1;
        buf_len += 1;

        *(uint32_t*)buf = resp->sequence;
        buf += 4;
        buf_len += 4;

        *(uint32_t*)buf = list_resp->length;
        buf += sizeof(uint32_t);
        buf_len += sizeof(uint32_t);

        int i;
        ListHead *head = list_resp->sample_list;
        for (i = 0; i < list_resp->length; i++) {
                Sample *s = listFirstEntry(head, Sample, element);
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

