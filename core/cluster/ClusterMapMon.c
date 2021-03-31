/*
 * ClusterMap.c
 *
 *  Created on: Mar 30, 2021
 *      Author: Zhen Xiong
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <cluster/ClusterMap.h>
#include <util/FileUtil.h>
#include <Log.h>
#include "ClusterMapPrivate.h"

#define MON_OBJECT_SERVICE_MAP_BIN_PATH "./mon_object_service_map.bin"

typedef struct ClusterMapMonPrivate {
	ClusterMapPrivate	super;
	ClusterMapMonParam	param;
} ClusterMapMonPrivate;

/**
 * init cluster map file format:
 * {
 *  bset_length:1024,
 *  bset_replica:3,
 *  object_service_count:9,
 *  object_services:[
 *      {id:0, host:"127.0.0.1",port:10100},
 *      {id:1, host:"127.0.0.1",port:10101},
 *      ...
 *  ]
 * }
 */
static bool cmmParseClusterMap(ObjectServiceMap *os_map, char *buffer, ssize_t buf_len) {

        return true;
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


