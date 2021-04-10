/*
 * ClusterMap.c
 *
 *  Created on: Mar 30, 2021
 *      Author: Zhen Xiong
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cluster/ClusterMap.h>

#include "ClusterMapPrivate.h"

typedef struct ClusterMapClientPrivate {
	ClusterMapPrivate	super;
	ClusterMapClientParam	param;
} ClusterMapClientPrivate;

static void destroy(ClusterMap* obj) {
	ClusterMapClientPrivate *priv = obj->p;
	
	free(priv);
}

static ClusterMapMethod method = {
	.getObjectServiceMap = clusterMapGetObjectServiceMap,
        .destroy = destroy,
};

bool initClusterMapClient(ClusterMap* obj, ClusterMapParam* param) {
	ClusterMapClientPrivate *priv = malloc(sizeof(*priv));
	ClusterMapClientParam *mparam = (ClusterMapClientParam*)param;

	obj->p = priv;
	obj->m = &method;
	memcpy(&priv->param, mparam, sizeof(*mparam));
	
	return true;
}


