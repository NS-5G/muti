/*
 * ServerContextMon.c
 *
 *  Created on: Feb 3, 2021
 *      Author: root
 */

#include <mon/server/ServerContextMon.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cluster/ClusterMap.h>

static void destroy(ServerContextMon* this) {
	ClusterMap *cmap = &this->clusterMap;
	cmap->m->destroy(cmap);
	sem_destroy(&this->stop_sem);
}

static ServerContextMonMethod method = {
        .destroy = destroy,
};

bool initServerMonContext(ServerContextMon* this, ServerContextMonParam* param) {
        bool rc = true;

        this->p = NULL;
        this->m = &method;

        ClusterMapMonParam cmap_mon_param;
        cmap_mon_param.super.type = ClusterMapType_Mon;
        strcpy(cmap_mon_param.init_path, param->cmap_init_path);

        rc = initClusterMap(&this->clusterMap, &cmap_mon_param.super);
        sem_init(&this->stop_sem, 0, 0);
        return rc;
}


