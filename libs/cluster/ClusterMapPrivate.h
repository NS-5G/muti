/*
 * ClusterMapPrivate.h
 *
 *  Created on: Mar 30, 2021
 *      Author: Zhen Xiong
 */

#ifndef CLUSTER_CLUSTERMAPPRIVATE_H_
#define CLUSTER_CLUSTERMAPPRIVATE_H_

#include "ClusterMap.h"

typedef struct ClusterMapPrivate {
	ObjectServiceMap	*os_map;
} ClusterMapPrivate;

extern bool initClusterMapMon(ClusterMap* obj, ClusterMapParam* param);
extern bool initClusterMapClient(ClusterMap* obj, ClusterMapParam* param);
extern bool initClusterMapObjectService(ClusterMap* obj, ClusterMapParam* param);

extern ObjectServiceMap* clusterMapGetObjectServiceMap(ClusterMap* obj);
extern void clusterMapPutObjectServiceMap(ClusterMap* obj, ObjectServiceMap *os_map);
extern void clusterMapDestroy(ClusterMap* obj);

extern void clusterMapInitOSMap(ObjectServiceMap *os_map);

#endif /* CLUSTER_CLUSTERMAPPRIVATE_H_ */
