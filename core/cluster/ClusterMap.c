/*
 * ClusterMap.c
 *
 *  Created on: Mar 30, 2021
 *      Author: Zhen Xiong
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <cluster/ClusterMap.h>
#include "ClusterMapPrivate.h"

bool clusterMapBinDump(ObjectServiceMap *os_map,  char **buffer, ssize_t *buf_len) {
        char *buf;
        ssize_t len = 0, total_len;
        uint32_t i, j;

        total_len = 14 + os_map->bset_length * (4 + os_map->bset_replica_size * 4) +
                        os_map->object_service_length * (12 + NETWORK_HOST_LEN + 1);
        for (i = 0; i < os_map->object_service_length; i++) {
                ObjectService *os = os_map->object_services[i];
                total_len += os->bset_length * 4;
        }
        buf = malloc(total_len);

        *(uint32_t*)buf = os_map->version;
        buf += 4; len += 4;

        *(uint32_t*)buf = os_map->bset_length;
        buf += 4; len += 4;

        *(uint16_t*)buf = os_map->bset_replica_size;
        buf += 2; len += 2;

        *(uint32_t*)buf = os_map->object_service_length;
        buf += 4; len += 4;

        for (i = 0; i < os_map->bset_length; i++) {
                BSet *bset = &os_map->bset[i];

                *(uint32_t*)buf = bset->id;
                buf += 4; len += 4;

                for(j = 0; j < os_map->bset_replica_size; j++) {
                        *(uint32_t*)buf = bset->object_service_ids[j];
                        buf += 4; len += 4;
                }
        }

        for (i = 0; i < os_map->object_service_length; i++) {
                ObjectService *os = os_map->object_services[i];

                *(uint32_t*)buf = os->id;
                buf += 4; len += 4;

                memcpy(buf, os->host, NETWORK_HOST_LEN + 1);
                buf += NETWORK_HOST_LEN + 1; len += NETWORK_HOST_LEN + 1;

                *(uint32_t*)buf = os->port;
                buf += 4; len += 4;

                *(uint32_t*)buf = os->bset_length;
                buf += 4; len += 4;

                for (j = 0; j < os->bset_length; j++) {
                        *(uint32_t*)buf = os->bset_ids[j];
                        buf += 4; len += 4;
                }
        }

        assert(len == total_len);
        *buffer = buf;
        *buf_len = len;
        return true;
}

bool clusterMapBinParse(ObjectServiceMap *os_map,  char *buffer, ssize_t buf_len) {
        // TODO
        return true;
}

ObjectServiceMap* clusterMapGetObjectServiceMap(ClusterMap* obj) {
	ClusterMapPrivate *priv_p = obj->p;

	return &priv_p->os_map;
}

bool initClusterMap(ClusterMap* obj, ClusterMapParam* param) {
	int rc = false;

	switch(param->type) {
	case ClusterMapType_Mon:
		rc = initClusterMapMon(obj, param);
		break;
	case ClusterMapType_Client:
		rc = initClusterMapClient(obj, param);
		break;
	case ClusterMapType_ObjectService:
		rc = initClusterMapObjectService(obj, param);
		break;
	default:
		assert(0);
	}
	
	return rc;
}


