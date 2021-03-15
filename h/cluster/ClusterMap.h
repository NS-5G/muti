/*
 * ClusterMap.h
 *
 *  Created on: Mar 7, 2021
 *      Author: Zhen Xiong
 */

#ifndef CLUSTER_CLUSTERMAP_H_
#define CLUSTER_CLUSTERMAP_H_
#include <stdbool.h>
#include <network/Socket.h>

typedef enum OSDNodeStatus {
	OSDNodeStatus_Online,
	OSDNodeStatus_SyncOut,
	OSDNodeStatus_SyncIn,
	OSDNodeStatus_Offline
} OSDNodeStatus;

typedef struct OSDNode {
        uint32_t        id;
        OSDNodeStatus	status;
        char            host[NETWORK_HOST_LEN + 1];
        int             port;
        void		*user_define;
} OSDNode;

typedef struct BSet {
	uint32_t	id;
	OSDNode		*replicas[];
} Bset;

typedef enum ClusterMapType {
	ClusterMapType_MON,
	ClusterMapType_Client
} ClusterMapType;

typedef struct ClusterMap ClusterMap;
typedef struct ClusterMapMethod {
	uint16_t	(*getBSetReplicaLength)(ClusterMap*);
        void    	(*destroy)(ClusterMap*);
} ClusterMapMethod;

struct ClusterMap {
        void                    *p;
        ClusterMapMethod        *m;
};

typedef struct ClusterMapParam {
	ClusterMapType	type;
	uint16_t	bset_replica_length;
} ClusterMapParam;

typedef struct ClusterMapMonParam {
	ClusterMapParam	super;
	char		*path;
} ClusterMapMonParam;

typedef struct ClusterMapClientParam {
	ClusterMapParam	super;
	char		*mon_host;
	int		mon_port;
} ClusterMapClientParam;

bool initClusterMap(ClusterMap*, ClusterMapParam*);

#endif /* CLUSTER_CLUSTERMAP_H_ */
