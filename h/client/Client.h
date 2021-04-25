/*
 * Client.h
 *
 *  Created on: Feb 6, 2021
 *      Author: root
 */

#ifndef CLIENT_CLIENT_H_
#define CLIENT_CLIENT_H_
#include <stdbool.h>
#include <res/Resource.h>
#include <network/Socket.h>
#include <client/RequestSender.h>

typedef struct Client Client;

typedef void (*ClientSendCallback)(Client *, Response *, void *);

typedef struct ClientMethod {
        bool    (*sendRequest)(Client*, Request*, ClientSendCallback, void *, bool *free_req);
        bool    (*sendRequestSync)(Client*, Request*);
        void*   (*getContext)(Client *);
        void    (*destroy)(Client*);
} ClientMethod;

struct Client {
        void            *p;
        ClientMethod    *m;
};

typedef struct ClientParam {
        RequestSender   *request_sender;
        ThreadPool      *worker_tp;
        ThreadPool      *read_tp;
        ThreadPool      *write_tp;
        char            host[NETWORK_HOST_LEN + 1];
        int             port;
        size_t          read_buffer_size;
        void            *context;
} ClientParam;

bool initClient(Client*, ClientParam*);

#endif /* CLIENT_CLIENT_H_ */
