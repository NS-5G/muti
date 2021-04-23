/*
 * Resource.h
 *
 *  Created on: Jan 8, 2021
 *      Author: Zhen Xiong
 */

#ifndef SERVER_HANDLER_REQUESTHANDLER_H_
#define SERVER_HANDLER_REQUESTHANDLER_H_

#include <stdint.h>
#include <stdbool.h>
#include <util/ThreadPool.h>
#include <network/Connection.h>
#include <res/Resource.h>

typedef struct RequestHandler RequestHandler;
typedef struct SRequest SRequest;
typedef void (*ActionCallback)(SRequest *);
typedef void (*FreeResponse)(SRequest *);

struct SRequest {
        Job             job;
        Request         *request;
        bool            free_req;
        Connection      *connection;
        RequestHandler  *req_handler;
        Response        *response;
        Readbuffer      *read_buffer;
        ActionCallback  action_callback;
        FreeResponse    free_response;
};

typedef void (*Action)(SRequest *);
typedef Request* (*RequestDecoder)(Connection *conn, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
typedef bool (*ResponseEncoder)(Connection *conn, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);

struct RequestHandler {
        int             id;
        Action          *actions;
        RequestDecoder  *request_decoders;
        ResponseEncoder *response_encoders;
};

extern bool ErrorResponseEncoder(Response *resp, char **buffer, size_t *buff_len, bool *free_resp);

#endif /* SERVER_HANDLER_REQUESTHANDLER_H_ */
