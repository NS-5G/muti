/*
 * ClusterMap.c
 *
 *  Created on: Mar 30, 2021
 *      Author: Zhen Xiong
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <assert.h>

#include <util/FileUtil.h>
#include <client/Client.h>
#include <mon/client/MonSenders.h>
#include <mon/share/Cluster.h>
#include <Log.h>
#include "ClusterMap.h"
#include "ClusterMapPrivate.h"

#define OBJECT_SERVICE_MAP_BIN_PATH "./object_service_map%u.bin"

typedef struct ClusterMapObjectServicePrivate {
	ClusterMapPrivate		super;
	ClusterMapObjectServiceParam	param;
	Client                          monClient;
	void                            *hk_worker;
} ClusterMapObjectServicePrivate;

static void destroy(ClusterMap* obj) {
	ClusterMapObjectServicePrivate *priv_p = obj->p;
	// TODO
	free(priv_p);
}

static ClusterMapMethod method = {
	.getObjectServiceMap = clusterMapGetObjectServiceMap,
	.putObjectServiceMap = clusterMapPutObjectServiceMap,
        .destroy = destroy,
};

typedef struct ClusterMapCallbackArgument {
        bool                            rc;
        ClusterMapObjectServicePrivate  *priv_p;
        sem_t                           sem;
} ClusterMapCallbackArgument;

static void clusterMapFetchOSMapCallback(Client *client, Response *resp, void *p) {
        ClusterMapCallbackArgument *cbarg_p = p;
        ClusterMapObjectServicePrivate *priv_p;

        if (resp->error_id) {
                ELOG("clusterMapFetchOSMapCallback: Send request failed, error id:%d", resp->error_id);
                cbarg_p->rc = false;
        } else {
                ClusterGetLatestObjectServiceMapResponse *cglosm_resp = (ClusterGetLatestObjectServiceMapResponse*)resp;
                priv_p = cbarg_p->priv_p;
                assert(priv_p->super.os_map == NULL);
                priv_p->super.os_map = cglosm_resp->os_map;
                cbarg_p->rc = true;
        }
        sem_post(&cbarg_p->sem);
}

static bool clusterMapFetchOSMapFromMon(ClusterMapObjectServicePrivate *priv_p, char *os_map_path) {
        Client *mon_client = &priv_p->monClient;
        ClusterGetLatestObjectServiceMapRequest *req = malloc(sizeof(*req));
        bool free_req;
        ClusterMapCallbackArgument cbarg;

        cbarg.rc = false;
        cbarg.priv_p = priv_p;
        sem_init(&cbarg.sem, 0, 0);
        req->super.resource_id = ResourceIdCluster;
        req->super.request_id = ClusterRequestId_GetLatestObjectServiceMap;
        bool rc = mon_client->m->sendRequest(mon_client, &req->super, clusterMapFetchOSMapCallback, &cbarg, &free_req);
        if (rc == false) {
                free(req);
                return rc;
        }
        if (free_req) free(req);
        sem_wait(&cbarg.sem);
        rc = cbarg.rc;
        if (rc == true) {
                ssize_t buf_len;
                buf_len = clusterMapDumpObjectServiceMapLength(priv_p->super.os_map);
                void *buffer;
                buffer = malloc(buf_len);
                rc = clusterMapDumpObjectServiceMap(priv_p->super.os_map,  buffer, buf_len);
                if (rc == false) {
                        ELOG("Error dump object service map");
                        assert(0);
                }
                rc = fileUtilWriteAFile(os_map_path, buffer, buf_len);
                if (rc == false) {
                        ELOG("Error dump object service map, path:%s", os_map_path);
                        assert(0);
                }
                free(buffer);
        }
        return rc;
}

static bool clusterMapInitMonClient(ClusterMapObjectServicePrivate *priv_p) {
        ClientParam param;
        strcpy(param.host, priv_p->param.mon_host);
        param.port = priv_p->param.mon_port;
        param.read_buffer_size = 1 << 22;
        param.read_tp = priv_p->param.read_tp;
        param.write_tp = priv_p->param.write_tp;
        param.worker_tp = priv_p->param.work_tp;
        param.request_sender = MonSenders;
        param.context = NULL;

        bool rc = initClient(&priv_p->monClient, &param);
        return rc;
}


static void clusterMapOSKeepAliveCallback(Client *client, Response *resp, void *p) {
        // TODO
}

static void clusterMapKeepAliveHouseKeepingWorker(void *p) {
        ClusterMap* this = p;
        ClusterMapObjectServicePrivate *priv_p = this->p;
        Client *mon_client = &priv_p->monClient;
        ClusterKeepAliveObjectServiceRequest *req = malloc(sizeof(*req));
        ObjectService *os;
        bool free_req;
        ClusterMapCallbackArgument cbarg;

        cbarg.rc = false;
        cbarg.priv_p = priv_p;
        sem_init(&cbarg.sem, 0, 0);
        req->super.resource_id = ResourceIdCluster;
        req->super.request_id = ClusterRequestId_KeepAliveObjectService;
        req->os_id = priv_p->param.os_id;
        req->version = priv_p->super.os_map->version;
        os = clusterMapGetObjectService(priv_p->super.os_map, priv_p->param.os_id);
        req->status = os->status;  // TODO need modify
        bool rc = mon_client->m->sendRequest(mon_client, &req->super, clusterMapOSKeepAliveCallback, &cbarg, &free_req);
        if (rc == false) {
                os->status = ObjectServiceStatus_Offline;
                free(req);
                return;
        }
        if (free_req) free(req);
        sem_wait(&cbarg.sem);
        rc = cbarg.rc;
        // TODO
}

bool initClusterMapObjectService(ClusterMap* this, ClusterMapParam* param) {
	ClusterMapObjectServicePrivate *priv_p = malloc(sizeof(*priv_p));
	ClusterMapObjectServiceParam *mparam = (ClusterMapObjectServiceParam*)param;
	char os_map_path[1024];
        char *buffer;
        ssize_t buf_len;
        bool rc;

	this->p = priv_p;
	this->m = &method;
	memcpy(&priv_p->param, mparam, sizeof(*mparam));
	sprintf(os_map_path, OBJECT_SERVICE_MAP_BIN_PATH, mparam->os_id);

	rc = clusterMapInitMonClient(priv_p);
	if (rc == false) return rc;

        rc = fileUtilReadAFile(os_map_path, &buffer, &buf_len);
        if (rc == false) {
                rc = clusterMapFetchOSMapFromMon(priv_p, os_map_path);
        } else {
                priv_p->super.os_map = calloc(sizeof(*priv_p->super.os_map), 1);
                listHeadInit(&priv_p->super.os_map->object_service_list);
                rc = clusterMapParseObjectServiceMap(priv_p->super.os_map, buffer, buf_len, NULL);
                if (rc == false) {
                        free(buffer);
                        clusterMapFreeOSMap(priv_p->super.os_map);
                        free(priv_p->super.os_map);
                        priv_p->super.os_map = NULL;
                        rc = clusterMapFetchOSMapFromMon(priv_p, os_map_path);
                        return rc;
                }
                free(buffer);
        }
        priv_p->super.os_map->reference = 1;
        ObjectService *os = clusterMapGetObjectService(priv_p->super.os_map, priv_p->param.os_id);
        os->status = ObjectServiceStatus_ReadyToJoin;
        // Start keep alive thread
        HouseKeeping *hk_p = priv_p->param.hk;
        priv_p->hk_worker = hk_p->m->addWorker(hk_p, clusterMapKeepAliveHouseKeepingWorker, this, 5);
	return rc;
}


