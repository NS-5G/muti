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

typedef struct ClusterMapMonPrivate {
	ClusterMapPrivate	super;
	ClusterMapMonParam	param;
} ClusterMapMonPrivate;

static void destroy(ClusterMap* obj) {
	ClusterMapMonPrivate *priv = obj->p;
	
	free(priv);
}

static ClusterMapMethod method = {
	.getObjectServiceMap = ClusterMapGetObjectServiceMap,
        .destroy = destroy,
};

bool initClusterMapMon(ClusterMap* obj, ClusterMapParam* param) {
	ClusterMapMonPrivate *priv = malloc(sizeof(*priv));
	ClusterMapMonParam *mparam = (ClusterMapMonParam*)param;

	obj->p = priv;
	obj->m = &method;
	memcpy(&priv->param, mparam, sizeof(*mparam));
	
	return true;
}


