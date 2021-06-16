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
#include <assert.h>
#include <cluster/ClusterMap.h>

static void destroy(ServerContextOs* this) {
        ClusterMap *cmap = &this->cluster_map;
        HouseKeeping *hk = &this->house_keeping;
        OSSyncing *ossync = &this->ossyncing;
        ThreadPool *read_tp = &this->read_tp;
        ThreadPool *write_tp = &this->write_tp;
        ThreadPool *work_tp = &this->work_tp;

        cmap->m->destroy(cmap);
        hk->m->destroy(hk);
        ossync->m->destroy(ossync);
        work_tp->m->destroy(work_tp);
        read_tp->m->destroy(read_tp);
        write_tp->m->destroy(write_tp);
        sem_destroy(&this->stop_sem);
}

static ServerContextOsMethod method = {
        .destroy = destroy,
};

bool initServerContextOs(ServerContextOs* this, ServerContextOsParam* param) {
        bool rc = true;
        ThreadPoolParam param_tp;

        this->p = NULL;
        this->m = &method;
        
        sem_init(&this->stop_sem, 0, 0);

        param_tp.do_batch = NULL;
        param_tp.thread_number = 6;
        rc = initThreadPool(&this->work_tp, &param_tp);
        assert(rc == true);
        param_tp.thread_number = 1;
        rc = initThreadPool(&this->write_tp, &param_tp);
        assert(rc == true);
        rc = initThreadPool(&this->read_tp, &param_tp);
        assert(rc == true);

        OSSyncingParam os_syncing_param;
        rc = initOSSyncing(&this->ossyncing, &os_syncing_param);
        assert(rc == true);

        HouseKeepingParam house_keeping_param;
        house_keeping_param.work_tp = &this->work_tp;
        rc = initHouseKeeping(&this->house_keeping, &house_keeping_param);
        assert(rc == true);

        ClusterMapObjectServiceParam cmap_os_param;
        cmap_os_param.super.type = ClusterMapType_ObjectService;
        strcpy(cmap_os_param.mon_host, param->mon_host);
        cmap_os_param.mon_port = param->mon_port;
        cmap_os_param.os_id = param->object_service_id;
        cmap_os_param.hk = &this->house_keeping;
        cmap_os_param.ossync = &this->ossyncing;
        cmap_os_param.read_tp = &this->read_tp;
        cmap_os_param.work_tp = &this->work_tp;
        cmap_os_param.write_tp = &this->write_tp;

        rc = initClusterMap(&this->cluster_map, &cmap_os_param.super);
        if (rc == false) {
                return rc;
        }
        ObjectServiceMap *os_map = this->cluster_map.m->getObjectServiceMap(&this->cluster_map);
        ObjectService *os = os_map->os_map.m->get(&os_map->os_map, &param->object_service_id);
        if (os == NULL) {
                this->cluster_map.m->putObjectServiceMap(&this->cluster_map, os_map);
                return false;
        }
        strcpy(this->os_host, os->host);
        this->os_port = os->port;
        this->cluster_map.m->putObjectServiceMap(&this->cluster_map, os_map);
        return rc;
}


