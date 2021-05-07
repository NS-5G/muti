/*
 * ObjectRequestSender.c
 *
 *  Created on: Jan 12, 2021
 *      Author: Zhen Xiong
 */
#include <os/client/object/ObjectSender.h>
#include <string.h>

#include <stdbool.h>
#include <os/share/Object.h>

RequestEncoder ObjectRequestEncoder[] = {
                ObjectRequestEncoderGet,
                ObjectRequestEncoderPut,
                ObjectRequestEncoderList,
};

bool ObjectRequestEncoderGet(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
	ObjectGetRequest *req1 = (ObjectGetRequest*)req;
	*buffer = (char*) req1;
	*buff_len = sizeof(*req1);
	*free_req = false;
        return true;
}

bool ObjectRequestEncoderPut(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
	ObjectPutRequest *req1 = (ObjectPutRequest*)req;
	*buffer = (char*) req1;
	*buff_len = sizeof(*req1) + req1->object.path_length;
	*free_req = false;
        return true;
}

bool ObjectRequestEncoderList(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
	ObjectListRequest *req1 = (ObjectListRequest*)req;
	*buffer = (char*) req1;
	*buff_len = sizeof(*req1);
	*free_req = false;
        return true;
}

ResponseDecoder ObjectResponseDecoder[] = {
                ObjectResponseDecoderGet,
                ObjectResponseDecoderPut,
                ObjectResponseDecoderList,
};

Response* ObjectResponseDecoderGet(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        Response *resp1 = (Response*)buffer;
        if (sizeof(*resp1) > buff_len) return NULL;
        if (resp1->error_id) {
                *consume_len = sizeof(Response);
                *free_resp = false;
                return resp1;
        }

	ObjectGetResponse *resp = (ObjectGetResponse*)buffer;
	size_t len = sizeof(*resp);
	if (buff_len < len) return NULL;
	len += resp->object.path_length;
	if (buff_len < len) return NULL;
	*consume_len = len;
	*free_resp = false;
	return &resp->super;
}

Response* ObjectResponseDecoderPut(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        SENDER_RESPONSE_DECODER(ObjectPutResponse);
}

Response* ObjectResponseDecoderList(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        Response *resp1 = (Response*)buffer;
        if (sizeof(*resp1) > buff_len) return NULL;
        if (resp1->error_id) {
                *consume_len = sizeof(Response);
                *free_resp = false;
                return resp1;
        }

	ObjectListResponse *resp = malloc(sizeof(*resp));
	listHeadInit(&resp->object_head);

	size_t len = 0;
	len += 4; if (buff_len < len) goto err_out;
	resp->super.error_id = *(int32_t*)buffer;
	buffer += 4;

	len += 4; if (buff_len < len) goto err_out;
	resp->super.sequence = *(uint32_t*)buffer;
	buffer += 4;

	len += 4; if (buff_len < len) goto err_out;
	resp->length = *(uint32_t*)buffer;
	buffer += 4;

	int i;
	for (i = 0; i < resp->length; i++) {
		len += sizeof(Object);
		if (buff_len < len) goto err_out;
		Object *s = (Object*)buffer;
		len += s->path_length;
		if (buff_len < len) goto err_out;
		buffer += sizeof(Object) + s->path_length;
		listAddTail(&s->element, &resp->object_head);
	}
	*consume_len = len;
	*free_resp = true;
        return &resp->super;
err_out:
	free(resp);
	return NULL;
}

