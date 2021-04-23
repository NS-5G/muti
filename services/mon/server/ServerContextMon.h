/*
 * ServerContextMon.h
 *
 *  Created on: Feb 3, 2021
 *      Author: root
 */

#ifndef MON_SERVERMONCONTEXT_H_
#define MON_SERVERMONCONTEXT_H_
#include <stdbool.h>
#include <semaphore.h>
#include <cluster/ClusterMap.h>

typedef struct ServerContextMon ServerContextMon;
typedef struct ServerContextMonMethod {
        void    (*destroy)(ServerContextMon*);
} ServerContextMonMethod;

struct ServerContextMon {
        void                    *p;
        ServerContextMonMethod  *m;
        ClusterMap              clusterMap;
        sem_t                   stop_sem;
};

typedef struct ServerContextMonParam {
	char		cmap_init_path[1024];
} ServerContextMonParam;

bool initServerMonContext(ServerContextMon*, ServerContextMonParam*);

#endif /* MON_SERVERMONCONTEXT_H_ */
