/*
 * ServerContextOs.c
 *
 *  Created on: Feb 3, 2021
 *      Author: root
 */

#include "ServerContextOs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cluster/ClusterMap.h>

static void destroy(ServerContextOs* this) {
        ClusterMap *cmap = &this->clusterMap;
        cmap->m->destroy(cmap);
        sem_destroy(&this->stop_sem);
}

static ServerContextOsMethod method = {
        .destroy = destroy,
};

bool initServerContextOs(ServerContextOs* this, ServerContextOsParam* param) {
        bool rc = true;

        this->p = NULL;
        this->m = &method;
        
        ClusterMapObjectServiceParam cmap_os_param;
        cmap_os_param.super.type = ClusterMapType_ObjectService;
        strcpy(cmap_os_param.mon_host, param->mon_host);
        cmap_os_param.mon_port = param->mon_port;
        cmap_os_param.object_service_id = param->object_service_id;

        rc = initClusterMap(&this->clusterMap, &cmap_os_param.super);
        sem_init(&this->stop_sem, 0, 0);

        return rc;
}


