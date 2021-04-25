/*
 * ClusterMap.c
 *
 *  Created on: Mar 30, 2021
 *      Author: Zhen Xiong
 */

#include "ClusterMap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>

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
        size_t ssize = 0, i = 1;
        while (ssize < os_map->object_service_length) {
                ssize = 1 << i;
                i++;
        }
        mparam.slot_size = ssize;
        initMapHashLinked(&os_map->os_map, &mparam);
}

ssize_t clusterMapDumpObjectServiceMapLength(ObjectServiceMap *os_map) {
        ssize_t total_len;

        total_len = 14 + os_map->bset_length * (4 + os_map->bset_replica_size * 4) +
                        os_map->object_service_length * (13 + NETWORK_HOST_LEN + 1);
        ObjectService *os;
        listForEachEntry(os, &os_map->object_service_list, element) {
                total_len += os->bset_length * 4;
        }
        return total_len;
}

bool clusterMapDumpObjectServiceMap(ObjectServiceMap *os_map,  char *buf, ssize_t total_len) {
        ssize_t len = 0;
        uint32_t i, j;
        ObjectService *os;

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

        listForEachEntry(os, &os_map->object_service_list, element) {

                *(uint32_t*)buf = os->id;
                buf += 4; len += 4;

                memcpy(buf, os->host, NETWORK_HOST_LEN + 1);
                buf += NETWORK_HOST_LEN + 1; len += NETWORK_HOST_LEN + 1;

                *(uint32_t*)buf = os->port;
                buf += 4; len += 4;

                *(int8_t*)buf = os->status;
                buf += 1; len += 1;

                *(uint32_t*)buf = os->bset_length;
                buf += 4; len += 4;

                for (j = 0; j < os->bset_length; j++) {
                        *(uint32_t*)buf = os->bset_ids[j];
                        buf += 4; len += 4;
                }
        }

        assert(len == total_len);
        return true;
}

void clusterMapFreeOSMap(ObjectServiceMap *os_map) {
        uint32_t i;
        if (os_map->bset != NULL) {
                for (i = 0; i < os_map->bset_length; i++) {
                        BSet *bset = &os_map->bset[i];
                        if (bset->object_service_ids != NULL) free(bset->object_service_ids);
                }
                free(os_map->bset);
        }
        if (!listEmpty(&os_map->object_service_list)) {
                while (!listEmpty(&os_map->object_service_list)) {
                        ObjectService *os = listFirstEntry(&os_map->object_service_list, ObjectService, element);
                        listDel(&os->element);
                        if (os->bset_ids != NULL) free(os->bset_ids);
                        free(os);
                }
                os_map->os_map.m->clear(&os_map->os_map, NULL);
                os_map->os_map.m->destroy(&os_map->os_map);
        }
}

bool clusterMapParseObjectServiceMap(ObjectServiceMap *os_map,  char *buffer, ssize_t buf_len, ssize_t *consume_len) {
        char *buf = buffer;
        ssize_t len = 0;
        uint32_t i, j;

        os_map->bset = NULL;
        listHeadInit(&os_map->object_service_list);
        os_map->status = ObjectServiceMapStatus_Normal;
        clusterMapInitOSMap(os_map);

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

        for (i = 0; i < os_map->object_service_length; i++) {
                ObjectService *os = malloc(sizeof(*os));

                len += 4; if (len > buf_len) { goto error_out;}
                os->id = *(uint32_t*)buf;
                buf += 4;

                len += NETWORK_HOST_LEN + 1; if (len > buf_len) { goto error_out;}
                memcpy(os->host, buf, NETWORK_HOST_LEN + 1);
                buf += NETWORK_HOST_LEN + 1;

                len += 4; if (len > buf_len) { goto error_out;}
                os->port = *(uint32_t*)buf;
                buf += 4;

                len += 1; if (len > buf_len) { goto error_out;}
                os->status = *(int8_t*)buf;
                buf += 1;

                len += 4; if (len > buf_len) { goto error_out;}
                os->bset_length = *(uint32_t*)buf;
                buf += 4;

                os->bset_ids = malloc(sizeof(uint32_t) * os->bset_length);
                for (j = 0; j < os->bset_length; j++) {
                        len += 4; if (len > buf_len) { goto error_out;}
                        os->bset_ids[j] = *(uint32_t*)buf;
                        buf += 4;
                }

                void *v = os_map->os_map.m->put(&os_map->os_map, &os->id, os);
                assert(v == NULL);
                listAddTail(&os->element, &os_map->object_service_list);
        }
        if (consume_len) *consume_len = len;
        return true;
error_out:
        clusterMapFreeOSMap(os_map);
        return false;
}

ObjectServiceMap* clusterMapGetObjectServiceMap(ClusterMap* obj) {
	ClusterMapPrivate *priv_p = obj->p;
	__sync_add_and_fetch(&priv_p->os_map->reference, 1);
	return priv_p->os_map;
}

void clusterMapPutObjectServiceMap(ClusterMap* obj, ObjectServiceMap* os_map) {
        ClusterMapPrivate *priv_p = obj->p;
        int left = __sync_sub_and_fetch(&os_map->reference, 1);
        if (left == 0) {
                clusterMapFreeOSMap(os_map);
                if (os_map == priv_p->os_map) {
                        // TODO Need lock check here
                        priv_p->os_map = NULL;
                }
                free(os_map);
        }
}

void clusterMapDestroy(ClusterMap* obj) {
        ClusterMapPrivate *priv_p = obj->p;
        clusterMapPutObjectServiceMap(obj,priv_p->os_map);
        while (priv_p->os_map != NULL) usleep(100000);
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
	if (rc == true) {
	        ClusterMapPrivate *priv_p = obj->p;
	        priv_p->os_map->reference = 1;
	}
	return rc;
}


