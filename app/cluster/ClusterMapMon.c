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
#include <util/cJSON.h>
#include <util/Map.h>
#include "ClusterMapPrivate.h"

#define MON_OBJECT_SERVICE_MAP_BIN_PATH "./mon_object_service_map.bin"

typedef struct ClusterMapMonPrivate {
	ClusterMapPrivate	super;
	ClusterMapMonParam	param;
} ClusterMapMonPrivate;

static ObjectService *cmmGetOSbyIndex(ObjectServiceMap *os_map, uint32_t k) {
        uint32_t i = 0, got_it = 0;
        ObjectService *os;
        listForEachEntry(os, &os_map->object_service_list, element) {
                if (i == k) {
                        got_it = 1;
                        break;
                }
                i++;
        }
        assert(got_it == 1);
        return os;
}

static void cmmFillBSets(ObjectServiceMap *os_map) {
        uint32_t total_bset_num = os_map->bset_length * os_map->bset_replica_size;
        uint32_t avg_bset_num = total_bset_num / os_map->object_service_length;
        avg_bset_num = avg_bset_num * os_map->object_service_length == total_bset_num ? avg_bset_num : (avg_bset_num + 1);
        uint32_t i, j, k, l;
        ObjectService *os;
        BSet *bset;

        os_map->bset = malloc(os_map->bset_length * sizeof(BSet));
        for (i = 0; i < os_map->bset_length; i++) {
                bset = &os_map->bset[i];
                bset->id = i;
                bset->object_service_ids = malloc(sizeof(uint32_t) * os_map->bset_replica_size);
        }

        listForEachEntry(os, &os_map->object_service_list, element) {
                os->bset_ids = malloc(sizeof(uint32_t) * avg_bset_num);
                os->bset_length = 0;
                os->user_define = NULL;
        }

        for (j = 0; j < os_map->bset_replica_size; j++) {
                for (i = 0; i < os_map->bset_length; i++) {
                        l = (j + i) % os_map->bset_length;
                        k = l % os_map->object_service_length;
                        os = cmmGetOSbyIndex(os_map, k);
                        os->bset_ids[os->bset_length++] = l;
                        assert(os->bset_length <= avg_bset_num);
                        bset = &os_map->bset[l];
                        bset->object_service_ids[j] = os->id;
                }
        }
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
        uint32_t i;

        buffer[buf_len] = '\0';

        cJSON *root = cJSON_Parse(buffer);
        if (root == NULL) {
                ELOG("Parse json string failed:\n%s", buffer);
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
        os_map->status = ObjectServiceMapStatus_Updating;
        os_map->version = 0;

        clusterMapInitOSMap(os_map);

        for (i = 0; i < os_map->object_service_length; i++) {
                cJSON *js_os = cJSON_GetArrayItem(js_object_services, i);
                ObjectService *os = malloc(sizeof(*os));

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
                listAddTail(&os->element, &os_map->object_service_list);
        }

        cmmFillBSets(os_map);

out:
        cJSON_Delete(root);
        return rc;
out1:
        while (!listEmpty(&os_map->object_service_list)) {
                ObjectService *os = listFirstEntry(&os_map->object_service_list, ObjectService, element);
                listDel(&os->element);
                if (os->bset_ids != NULL) free(os->bset_ids);
                free(os);
        }
        os_map->os_map.m->destroy(&os_map->os_map);
        cJSON_Delete(root);
        return rc;

}

static void destroy(ClusterMap* obj) {
        ClusterMapMonPrivate *priv_p = obj->p;
        clusterMapDestroy(obj);
	free(priv_p);
}

static ClusterMapMethod method = {
	.getObjectServiceMap = clusterMapGetObjectServiceMap,
	.putObjectServiceMap = clusterMapPutObjectServiceMap,
	.parseObjectServiceMap = clusterMapParseObjectServiceMap,
	.dumpObjectServiceMap = clusterMapDumpObjectServiceMap,
	.dumpObjectServiceMapLength = clusterMapDumpObjectServiceMapLength,
        .destroy = destroy,
};

bool initClusterMapMon(ClusterMap* obj, ClusterMapParam* param) {
	ClusterMapMonPrivate *priv_p = malloc(sizeof(*priv_p));
	ClusterMapMonParam *mparam = (ClusterMapMonParam*)param;
	bool rc;
	char *buffer;
	ssize_t buf_len;

	obj->p = priv_p;
	obj->m = &method;
	memcpy(&priv_p->param, mparam, sizeof(*mparam));
	priv_p->super.os_map = calloc(sizeof(*priv_p->super.os_map), 1);
	listHeadInit(&priv_p->super.os_map->object_service_list);
	
	rc = fileUtilReadAFile(MON_OBJECT_SERVICE_MAP_BIN_PATH, &buffer, &buf_len);
	if (rc == false) {
		ELOG("Read file %s failed, try read init file %s.",
			MON_OBJECT_SERVICE_MAP_BIN_PATH, mparam->init_path);
	        rc = fileUtilReadAFile(mparam->init_path, &buffer, &buf_len);
	        if (rc == false) {
	        	ELOG("Read file %s failed.", mparam->init_path);
	        	goto error_out;
	        }
	        rc = cmmParseClusterMap(priv_p->super.os_map, buffer, buf_len);
	        if (rc == false) {
	        	ELOG("cmmParseClusterMap parse file %s failed.", mparam->init_path);
	                free(buffer);
	                goto error_out;
	        }
	        free(buffer);
	        buf_len = clusterMapDumpObjectServiceMapLength(priv_p->super.os_map);
	        buffer = malloc(buf_len);
	        rc = clusterMapDumpObjectServiceMap(priv_p->super.os_map,  buffer, buf_len);
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
	        rc = clusterMapParseObjectServiceMap(priv_p->super.os_map, buffer, buf_len);
	        if (rc == false) {
	                free(buffer);
	                goto error_out;
	        }
	        free(buffer);
	}

	return true;
error_out:
        clusterMapFreeOSMap(priv_p->super.os_map);
        free(priv_p->super.os_map);
        free(priv_p);
        return rc;
}


