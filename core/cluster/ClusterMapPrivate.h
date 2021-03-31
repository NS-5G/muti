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

extern ObjectServiceMap* clusterMapGetObjectServiceMap(ClusterMap* obj);
extern bool clusterMapBinDump(ObjectServiceMap *os_map,  char **buffer, ssize_t *buf_len);
extern bool clusterMapBinParse(ObjectServiceMap *os_map,  char *buffer, ssize_t buf_len);

#endif /* CLUSTER_CLUSTERMAPPRIVATE_H_ */
