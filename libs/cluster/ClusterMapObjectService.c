/*
 * ClusterMap.c
 *
 *  Created on: Mar 30, 2021
 *      Author: Zhen Xiong
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/FileUtil.h>
#include "ClusterMap.h"
#include "ClusterMapPrivate.h"

#define OBJECT_SERVICE_MAP_BIN_PATH "./object_service_map%u.bin"

typedef struct ClusterMapObjectServicePrivate {
	ClusterMapPrivate		super;
	ClusterMapObjectServiceParam	param;
} ClusterMapObjectServicePrivate;

static void destroy(ClusterMap* obj) {
	ClusterMapObjectServicePrivate *priv = obj->p;
	
	free(priv);
}

static ClusterMapMethod method = {
	.getObjectServiceMap = clusterMapGetObjectServiceMap,
        .destroy = destroy,
};

bool clusterMapFetchOSMapFromMon(ClusterMapObjectServicePrivate *priv_p, char *os_map_path) {
        // TODO
        return true;
}

bool initClusterMapObjectService(ClusterMap* obj, ClusterMapParam* param) {
	ClusterMapObjectServicePrivate *priv_p = malloc(sizeof(*priv_p));
	ClusterMapObjectServiceParam *mparam = (ClusterMapObjectServiceParam*)param;
	char os_map_path[1024];
        char *buffer;
        ssize_t buf_len;
        bool rc;

	obj->p = priv_p;
	obj->m = &method;
	memcpy(&priv_p->param, mparam, sizeof(*mparam));
	sprintf(os_map_path, OBJECT_SERVICE_MAP_BIN_PATH, mparam->object_service_id);

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
                // TODO Fetch os version from mon, compare and update os map.
        }

	return rc;
}


