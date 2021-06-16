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
#include <os/OSSyncing.h>
#include <util/HouseKeeping.h>
#include <util/ThreadPool.h>

typedef struct ServerContextOs ServerContextOs;
typedef struct ServerContextOsMethod {
        void    (*destroy)(ServerContextOs*);
} ServerContextOsMethod;

struct ServerContextOs {
        void                    *p;
        ServerContextOsMethod   *m;
        ClusterMap              cluster_map;
        OSSyncing               ossyncing;
        HouseKeeping            house_keeping;
        ThreadPool              read_tp;
        ThreadPool              write_tp;
        ThreadPool              work_tp;
        sem_t                   stop_sem;
        char                    os_host[NETWORK_HOST_LEN + 1];
        int                     os_port;
};

typedef struct ServerContextOsParam {
        char            mon_host[NETWORK_HOST_LEN + 1];
        int             mon_port;
        uint32_t        object_service_id;
} ServerContextOsParam;

bool initServerContextOs(ServerContextOs*, ServerContextOsParam*);

#endif /* SERVER_SERVEROSCONTEXT_H_ */
