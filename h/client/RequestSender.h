/*
 * RequestSender.h
 *
 *  Created on: Feb 6, 2021
 *      Author: Zhen Xiong
 */

#ifndef CLIENT_SENDER_REQUESTSENDER_H_
#define CLIENT_SENDER_REQUESTSENDER_H_

#include <stdint.h>
#include <stdbool.h>

#include <res/Resource.h>
#include <network/Connection.h>

typedef struct RequestSender RequestSender;

typedef Response* (*ResponseDecoder)(Connection *, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp);
typedef bool (*RequestEncoder)(Connection *, Request *req, char **buffer, size_t *buff_len, bool *free_req);

struct RequestSender {
        int             id;
        RequestEncoder  *request_encoders;
        ResponseDecoder *response_decoders;
};

extern Response* ErrorResponseDecoder(char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp);

#define SENDER_RESPONSE_DECODER(type)  Response *resp = (Response*)buffer; \
        if (sizeof(*resp) > buff_len) return NULL; \
        if (resp->error_id) { \
                *consume_len = sizeof(Response); \
                *free_resp = false; \
                return resp; \
        } \
        type *resp1 = (type*)buffer; \
        if (sizeof(*resp1) > buff_len) return NULL; \
        *consume_len = sizeof(*resp1); \
        *free_resp = false; \
        return &resp1->super;

#endif /* CLIENT_SENDER_REQUESTSENDER_H_ */
