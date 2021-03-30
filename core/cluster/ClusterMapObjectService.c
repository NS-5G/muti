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

typedef struct ClusterMapObjectServicePrivate {
	ClusterMapPrivate		super;
	ClusterMapObjectServiceParam	param;
} ClusterMapObjectServicePrivate;

static void destroy(ClusterMap* obj) {
	ClusterMapObjectServicePrivate *priv = obj->p;
	
	free(priv);
}

static ClusterMapMethod method = {
	.getObjectServiceMap = ClusterMapGetObjectServiceMap,
        .destroy = destroy,
};

bool initClusterMapObjectService(ClusterMap* obj, ClusterMapParam* param) {
	ClusterMapObjectServicePrivate *priv = malloc(sizeof(*priv));
	ClusterMapObjectServiceParam *mparam = (ClusterMapObjectServiceParam*)param;

	obj->p = priv;
	obj->m = &method;
	memcpy(&priv->param, mparam, sizeof(*mparam));
	
	return true;
}


