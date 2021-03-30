/*
 * ClusterMapPrivate.h
 *
 *  Created on: Mar 30, 2021
 *      Author: Zhen Xiong
 */

#ifndef CLUSTER_CLUSTERMAPPRIVATE_H_
#define CLUSTER_CLUSTERMAPPRIVATE_H_

#include <cluster/ClusterMap.h>

typedef struct ClusterMapPrivate {
	ObjectServiceMap	os_map;
} ClusterMapPrivate;

extern bool initClusterMapMon(ClusterMap* obj, ClusterMapParam* param);
extern bool initClusterMapClient(ClusterMap* obj, ClusterMapParam* param);
extern bool initClusterMapObjectService(ClusterMap* obj, ClusterMapParam* param);

extern ObjectServiceMap* ClusterMapGetObjectServiceMap(ClusterMap* obj);

#endif /* CLUSTER_CLUSTERMAPPRIVATE_H_ */
