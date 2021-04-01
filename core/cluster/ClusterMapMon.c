/*
 * ClusterMap.c
 *
 *  Created on: Mar 30, 2021
 *      Author: Zhen Xiong
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>

#include <cluster/ClusterMap.h>
#include <util/FileUtil.h>
#include <Log.h>
#include "ClusterMapPrivate.h"
#include <util/cJSON.h>
#include <util/Map.h>

#define MON_OBJECT_SERVICE_MAP_BIN_PATH "./mon_object_service_map.bin"

typedef struct ClusterMapMonPrivate {
	ClusterMapPrivate	super;
	ClusterMapMonParam	param;
} ClusterMapMonPrivate;

static int osInt32KeyCompare(void *key, void *key1) {
        return *(uint32_t *)key - *(uint32_t *)key1;
}

static uint64_t osInt32Hash(void *key) {
        return (uint64_t)*(uint32_t *)key;
}

static void cmmFillBSets(ObjectServiceMap *os_map) {

}

/**
 * init cluster map file format:
 * {
 *  bset_length:1024,
 *  bset_replica_size:3,
 *  object_services:[
 *      {id:0, host:"127.0.0.1",port:10100},
 *      {id:1, host:"127.0.0.1",port:10101},
 *      ...
 *  ]
 * }
 */
static bool cmmParseClusterMap(ObjectServiceMap *os_map, char *buffer, ssize_t buf_len) {
        bool rc = true;
        uint32_t i, j;

        buffer[buf_len] = '\0';

        cJSON *root = cJSON_Parse(buffer);
        if (root == NULL) {
                ELOG("Parse json string failed.");
                rc = false;
                return rc;
        }

        cJSON *js_bset_length = cJSON_GetObjectItem(root, "bset_length");
        if (js_bset_length == NULL) { rc = false;  goto out;}
        int n = sscanf(js_bset_length->valuestring, "%" SCNu32, &os_map->bset_length);
        if (n != 1) { rc = false; goto out; }

        cJSON *js_bset_replica_size = cJSON_GetObjectItem(root, "bset_replica_size");
        if (js_bset_replica_size == NULL) { rc = false;  goto out;}
        n = sscanf(js_bset_replica_size->valuestring, "%" SCNu16, &os_map->bset_replica_size);
        if (n != 1) { rc = false; goto out; }

        cJSON *js_object_services = cJSON_GetObjectItem(root, "object_services");
        os_map->object_service_length = cJSON_GetArraySize(js_object_services);
        os_map->object_services = malloc(sizeof(void*) * os_map->object_service_length);
        os_map->status = ObjectServiceMapStatus_Updating;

        MapHashLinkedParam mparam;
        mparam.super.compareMethod = osInt32KeyCompare;
        mparam.hashMethod = osInt32Hash;
        mparam.keyOffsetInValue = (long)(&((struct ObjectService *)0)->id);
        mparam.slot_size = os_map->object_service_length;
        initMapHashLinked(&os_map->os_map, &mparam);

        for (i = 0; i < os_map->object_service_length; i++) {
                cJSON *js_os = cJSON_GetArrayItem(js_object_services, i);
                ObjectService *os = malloc(sizeof(*os));
                os_map->object_services[i] = os;

                cJSON *js_id = cJSON_GetObjectItem(js_os, "id");
                cJSON *js_host = cJSON_GetObjectItem(js_os, "host");
                cJSON *js_port = cJSON_GetObjectItem(js_os, "port");

                if (js_id == NULL || js_host == NULL || js_port == NULL) {
                        rc = false; goto out1;
                }

                n = sscanf(js_id->valuestring, "%" SCNu32, &os->id);
                if (n != 1) { rc = false; goto out1; }

                strcpy(os->host, js_host->valuestring);

                n = sscanf(js_port->valuestring, "%" SCNu32, &os->port);
                if (n != 1) { rc = false; goto out1; }

                void *v = os_map->os_map.m->put(&os_map->os_map, &os->id, os);
                if (v != NULL){ ELOG("Duplicate object service id:%u", os->id); rc = false; goto out1; }

                os->status = ObjectServiceStatus_Offline;
                os->bset_ids = NULL;
                os->bset_length = 0;
        }

        cmmFillBSets(os_map);

out:
        cJSON_Delete(root);
        return rc;
out1:
        for (j = 0; j<=i; j++) {
                free(os_map->object_services[j]);
                free(os_map->object_services);
        }
        os_map->os_map.m->destroy(&os_map->os_map);
        cJSON_Delete(root);
        return rc;

}

static void destroy(ClusterMap* obj) {
	ClusterMapMonPrivate *priv = obj->p;
	
	free(priv);
}

static ClusterMapMethod method = {
	.getObjectServiceMap = clusterMapGetObjectServiceMap,
        .destroy = destroy,
};

bool initClusterMapMon(ClusterMap* obj, ClusterMapParam* param) {
	ClusterMapMonPrivate *priv = malloc(sizeof(*priv));
	ClusterMapMonParam *mparam = (ClusterMapMonParam*)param;
	bool rc;
	char *buffer;
	ssize_t buf_len;

	obj->p = priv;
	obj->m = &method;
	memcpy(&priv->param, mparam, sizeof(*mparam));
	
	rc = fileUtilReadAFile(MON_OBJECT_SERVICE_MAP_BIN_PATH, &buffer, &buf_len);
	if (rc == false) {
	        rc = fileUtilReadAFile(mparam->init_path, &buffer, &buf_len);
	        if (rc == false) return rc;
	        rc = cmmParseClusterMap(&priv->super.os_map, buffer, buf_len);
	        if (rc == false) {
	                free(buffer);
	                return rc;
	        }
	        free(buffer);
	        rc = clusterMapBinDump(&priv->super.os_map,  &buffer, &buf_len);
	        if (rc == false) {
	                ELOG("Error dump object service map");
	                assert(0);
	        }
	        rc = fileUtilWriteAFile(MON_OBJECT_SERVICE_MAP_BIN_PATH, buffer, buf_len);
	        if (rc == false) {
                        ELOG("Error dump object service map, path:%s", MON_OBJECT_SERVICE_MAP_BIN_PATH);
                        assert(0);
                }
	        free(buffer);
	} else {
	        rc = clusterMapBinParse(&priv->super.os_map, buffer, buf_len);
	        if (rc == false) {
	                free(buffer);
	                return rc;
	        }
	        free(buffer);
	}
	return true;
}


