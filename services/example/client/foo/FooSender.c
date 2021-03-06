/*
 * FooRequestSender.c
 *
 *  Created on: Jan 12, 2021
 *      Author: Zhen Xiong
 */
#include <example/client/foo/FooSender.h>
#include <string.h>

#include <stdbool.h>
#include <example/share/Foo.h>

RequestEncoder FooRequestEncoder[] = {
                FooRequestEncoderGet,
                FooRequestEncoderPut,
                FooRequestEncoderList,
};

bool FooRequestEncoderGet(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
	FooGetRequest *req1 = (FooGetRequest*)req;
	*buffer = (char*) req1;
	*buff_len = sizeof(*req1);
	*free_req = false;
        return true;
}

bool FooRequestEncoderPut(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
	FooPutRequest *req1 = (FooPutRequest*)req;
	*buffer = (char*) req1;
	*buff_len = sizeof(*req1) + req1->foo.path_length;
	*free_req = false;
        return true;
}

bool FooRequestEncoderList(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
	FooListRequest *req1 = (FooListRequest*)req;
	*buffer = (char*) req1;
	*buff_len = sizeof(*req1);
	*free_req = false;
        return true;
}

ResponseDecoder FooResponseDecoder[] = {
                FooResponseDecoderGet,
                FooResponseDecoderPut,
                FooResponseDecoderList,
};

Response* FooResponseDecoderGet(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        Response *resp1 = (Response*)buffer;
        if (sizeof(*resp1) > buff_len) return NULL;
        if (resp1->error_id) {
                *consume_len = sizeof(Response);
                *free_resp = false;
                return resp1;
        }

	FooGetResponse *resp = (FooGetResponse*)buffer;
	size_t len = sizeof(*resp);
	if (buff_len < len) return NULL;
	len += resp->foo.path_length;
	if (buff_len < len) return NULL;
	*consume_len = len;
	*free_resp = false;
	return &resp->super;
}

Response* FooResponseDecoderPut(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        SENDER_RESPONSE_DECODER(FooPutResponse);
}

Response* FooResponseDecoderList(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        Response *resp1 = (Response*)buffer;
        if (sizeof(*resp1) > buff_len) return NULL;
        if (resp1->error_id) {
                *consume_len = sizeof(Response);
                *free_resp = false;
                return resp1;
        }

	FooListResponse *resp = malloc(sizeof(*resp));
	listHeadInit(&resp->foo_head);

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
		len += sizeof(Foo);
		if (buff_len < len) goto err_out;
		Foo *s = (Foo*)buffer;
		len += s->path_length;
		if (buff_len < len) goto err_out;
		buffer += sizeof(Foo) + s->path_length;
		listAddTail(&s->element, &resp->foo_head);
	}
	*consume_len = len;
	*free_resp = true;
        return &resp->super;
err_out:
	free(resp);
	return NULL;
}

