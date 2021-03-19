/*
 * Cluster.h
 *
 *  Created on: Jan 30, 2021
 *      Author: Zhen Xiong
 */

#ifndef RES_CLUSTER_H_
#define RES_CLUSTER_H_

#include <res/Resource.h>
#include <util/Map.h>
#include <network/Socket.h>

typedef enum {
	ClusterRequestId_GetOSDMapLatestVersion = 0,
        ClusterRequestId_GetLatestOSDMap,
        ClusterRequestId_AddOSDNode,
	ClusterRequestId_RemoveOSDNode,
	ClusterRequestId_KeepAliveOSD,
	ClusterRequestId_KeepAliveClient,
	ClusterRequestId_GetOSDMapChangeLog,
	ClusterRequestId_Status,
} ClusterRequestId;

typedef enum OSDNodeStatus {
	OSDNodeStatus_Online = 1,
	OSDNodeStatus_Starting,
	OSDNodeStatus_Syncing,
	OSDNodeStatus_Offline,
	OSDNodeStatus_Deleting,
} OSDNodeStatus;

typedef struct BSet BSet;

typedef struct OSDNode {
        uint32_t        id;
        OSDNodeStatus	status;
        char            host[NETWORK_HOST_LEN + 1];
        int32_t         port;
        uint32_t	bset_length;
        uint32_t	*bset_ids;
} OSDNode;

struct BSet {
	OSDNode		**osd_nodes;
};

typedef enum OSDMapStatus {
	OSDMapStatus_Normal = 1,
	OSDMapStatus_Starting,
	OSDMapStatus_Changing,
} OSDMapStatus;

typedef struct OSDMap {
	uint32_t	version;
	OSDMapStatus	status;
	uint32_t	node_length;
	OSDNode		**nodes;
	Map		nodes_map;
	uint32_t	bset_length;
	BSet		**bset;
	uint16_t	replica_length;
} OSDMap;

typedef enum {
	OSDMapChangeOperation_AddOSDNode = 1,
	OSDMapChangeOperation_RemoveOSDNode = 2,
} OSDMapChangeOperation;

typedef struct OSDMapChangeLog {
	uint32_t		from_version;
	uint32_t		to_version;
	OSDMapChangeOperation	operation;
	uint32_t		from_osd_node_id;
	uint32_t		to_osd_node_id;
	uint32_t		moved_bset_length;
	uint32_t		moved_bset_ids[];
} OSDMapChangeLog;

typedef struct ClusterGetOSDMapLatestVersionRequest {
        Request         super;
} ClusterGetOSDMapLatestVersionRequest;

typedef struct ClusterGetOSDMapLatestVersionResponse {
        Response        super;
        uint32_t        version;
} ClusterGetOSDMapLatestVersionResponse;

typedef struct ClusterGetLatestOSDMapRequest {
        Request         super;
} ClusterGetLatestOSDMapRequest;

typedef struct ClusterGetLatestOSDMapResponse {
        Response        super;
        OSDMap		osd_map;
} ClusterGetLatestOSDMapResponse;

typedef struct ClusterAddOSDNodeRequest {
        Request         super;
        uint32_t	id;
        char            host[NETWORK_HOST_LEN + 1];
        int             port;
} ClusterAddOSDNodeRequest;

typedef struct ClusterAddOSDNodeResponse {
        Response        super;
} ClusterAddOSDNodeResponse;

typedef struct ClusterRemoveOSDNodeRequest {
        Request         super;
        uint32_t	osd_node_id;
} ClusterRemoveOSDNodeRequest;

typedef struct ClusterRemoveOSDNodeResponse {
        Response        super;
} ClusterRemoveOSDNodeResponse;

typedef struct ClusterKeepAliveOSDRequest {
        Request         super;
        uint32_t	osd_node_id;
        OSDNodeStatus	status;
} ClusterKeepAliveOSDRequest;

typedef struct ClusterKeepAliveOSDResponse {
        Response        super;
        uint32_t        version;
        OSDNodeStatus	status;
} ClusterKeepAliveOSDResponse;

typedef struct ClusterKeepAliveClientRequest {
        Request         super;
} ClusterKeepAliveClientRequest;

typedef struct ClusterKeepAliveClientResponse {
        Response        super;
        uint32_t        version;
} ClusterKeepAliveClientResponse;

typedef struct ClusterGetOSDMapChangeLogRequest {
        Request         super;
        uint32_t        version;
} ClusterGetOSDMapChangeLogRequest;

typedef struct ClusterGetOSDMapChangeLogResponse {
        Response        super;
        OSDMapChangeLog	chang_log;
} ClusterGetOSDMapChangeLogResponse;

typedef struct ClusterStatusRequest {
        Request         super;
} ClusterStatusRequest;

typedef struct ClusterStatusResponse {
        Response        super;
        OSDMap		osd_map;
        uint64_t	total_objects;
        uint64_t	used;
        uint64_t	free;
} ClusterStatusResponse;

#endif /* RES_CLUSTER_H_ */
