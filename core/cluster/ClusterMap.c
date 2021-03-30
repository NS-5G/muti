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

ObjectServiceMap* ClusterMapGetObjectServiceMap(ClusterMap* obj) {
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


