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

static int osInt32KeyCompare(void *key, void *key1) {
        return *(uint32_t *)key - *(uint32_t *)key1;
}

static uint64_t osInt32Hash(void *key) {
        return (uint64_t)*(uint32_t *)key;
}

void clusterMapInitOSMap(ObjectServiceMap *os_map) {
        MapHashLinkedParam mparam;
        mparam.super.compareMethod = osInt32KeyCompare;
        mparam.hashMethod = osInt32Hash;
        mparam.keyOffsetInValue = (long)(&((struct ObjectService *)0)->id);
        mparam.slot_size = os_map->object_service_length;
        initMapHashLinked(&os_map->os_map, &mparam);
}

bool clusterMapBinDump(ObjectServiceMap *os_map,  char **buffer, ssize_t *buf_len) {
        char *buf;
        ssize_t len = 0, total_len;
        uint32_t i, j;

        total_len = 14 + os_map->bset_length * (4 + os_map->bset_replica_size * 4) +
                        os_map->object_service_length * (12 + NETWORK_HOST_LEN + 1);
        for (i = 0; i < os_map->object_service_length; i++) {
                ObjectService *os = &os_map->object_services[i];
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
                ObjectService *os = &os_map->object_services[i];

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
        char *buf = buffer;
        ssize_t len = 0;
        uint32_t i, j;

        os_map->bset = NULL;
        os_map->object_services = NULL;
        os_map->status = ObjectServiceMapStatus_Updating;

        len += 4; if (len > buf_len) { goto error_out;}
        os_map->version = *(uint32_t*)buf;
        buf += 4;

        len += 4; if (len > buf_len) { goto error_out;}
        os_map->bset_length = *(uint32_t*)buf;
        buf += 4;

        len += 2; if (len > buf_len) { goto error_out;}
        os_map->bset_replica_size = *(uint16_t*)buf;
        buf += 2;

        len += 4; if (len > buf_len) { goto error_out;}
        os_map->object_service_length = *(uint32_t*)buf;
        buf += 4;

        os_map->bset = calloc(sizeof(BSet), os_map->bset_length);
        for (i = 0; i < os_map->bset_length; i++) {
                BSet *bset = &os_map->bset[i];

                len += 4; if (len > buf_len) { goto error_out;}
                bset->id = *(uint32_t*)buf;
                buf += 4;

                bset->object_service_ids = malloc(sizeof(uint32_t) * os_map->bset_replica_size);

                for(j = 0; j < os_map->bset_replica_size; j++) {
                        len += 4; if (len > buf_len) { goto error_out;}
                        bset->object_service_ids[j] = *(uint32_t*)buf;
                        buf += 4;
                }
        }

        clusterMapInitOSMap(os_map);
        os_map->object_services = calloc(sizeof(ObjectService), os_map->object_service_length);

        for (i = 0; i < os_map->object_service_length; i++) {
                ObjectService *os = &os_map->object_services[i];

                len += 4; if (len > buf_len) { goto error_out;}
                os->id = *(uint32_t*)buf;
                buf += 4;

                len += NETWORK_HOST_LEN + 1; if (len > buf_len) { goto error_out;}
                memcpy(os->host, buf, NETWORK_HOST_LEN + 1);
                buf += NETWORK_HOST_LEN + 1;

                len += 4; if (len > buf_len) { goto error_out;}
                os->port = *(uint32_t*)buf;
                buf += 4;

                len += 4; if (len > buf_len) { goto error_out;}
                os->bset_length = *(uint32_t*)buf;
                buf += 4;

                os->bset_ids = malloc(sizeof(uint32_t) * os->bset_length);
                for (j = 0; j < os->bset_length; j++) {
                        len += 4; if (len > buf_len) { goto error_out;}
                        os->bset_ids[j] = *(uint32_t*)buf;
                        buf += 4;
                }
                os->status = ObjectServiceStatus_Offline;

                void *v = os_map->os_map.m->put(&os_map->os_map, &os->id, os);
                assert(v == NULL);
        }

        return true;
error_out:
        if (os_map->bset != NULL) {
                for (i = 0; i < os_map->bset_length; i++) {
                        BSet *bset = &os_map->bset[i];
                        if (bset->object_service_ids != NULL) free(bset->object_service_ids);
                }
                free(os_map->bset);
        }
        if (os_map->object_services != NULL) {
                for (i = 0; i < os_map->object_service_length; i++) {
                        ObjectService *os = &os_map->object_services[i];
                        if (os->bset_ids != NULL) free(os->bset_ids);
                }
                free(os_map->object_services);
        }
        return false;
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


