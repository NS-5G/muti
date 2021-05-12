/*
 * ServerFooContext.h
 *
 *  Created on: Feb 3, 2021
 *      Author: root
 */

#ifndef SERVER_SERVEROSCONTEXT_H_
#define SERVER_SERVEROSCONTEXT_H_
#include <stdbool.h>
#include <semaphore.h>
#include <cluster/ClusterMap.h>

typedef struct ServerContextOs ServerContextOs;
typedef struct ServerContextOsMethod {
        void    (*destroy)(ServerContextOs*);
} ServerContextOsMethod;

struct ServerContextOs {
        void                    *p;
        ServerContextOsMethod   *m;
        ClusterMap              clusterMap;
        sem_t                   stop_sem;
};

typedef struct ServerContextOsParam {
        char            mon_host[NETWORK_HOST_LEN + 1];
        int             mon_port;
        uint32_t        object_service_id;
} ServerContextOsParam;

bool initServerContextOs(ServerContextOs*, ServerContextOsParam*);

#endif /* SERVER_SERVEROSCONTEXT_H_ */
