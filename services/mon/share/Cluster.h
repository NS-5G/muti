/*
 * Cluster.h
 *
 *  Created on: Jan 30, 2021
 *      Author: Zhen Xiong
 */

#ifndef SHARE_CLUSTER_H_
#define SHARE_CLUSTER_H_

#include <res/Resource.h>
#include <util/Map.h>
#include <network/Socket.h>

typedef enum {
	ClusterRequestId_GetObjectServiceMapLatestVersion = 0,
        ClusterRequestId_GetLatestObjectServiceMap,
        ClusterRequestId_AddObjectService,
	ClusterRequestId_RemoveObjectService,
	ClusterRequestId_KeepAliveObjectService,
	ClusterRequestId_KeepAliveClient,
	ClusterRequestId_GetObjectServiceMapChangeLog,
	ClusterRequestId_Status,
} ClusterRequestId;

typedef enum ObjectServiceStatus {
	ObjectServiceStatus_Online = 1,
	ObjectServiceStatus_Starting,
	ObjectServiceStatus_Syncing,
	ObjectServiceStatus_Offline,
	ObjectServiceStatus_Deleting,
} ObjectServiceStatus;

typedef struct BSet BSet;

typedef struct ObjectService {
        uint32_t        	id;
        ObjectServiceStatus	status;
        char            	host[NETWORK_HOST_LEN + 1];
        int32_t         	port;
        uint32_t		bset_length;
        uint32_t		*bset_ids;
} ObjectService;

struct BSet {
	ObjectService		**object_services;
};

typedef enum ObjectServiceMapStatus {
	ObjectServiceMapStatus_Normal = 1,
	ObjectServiceMapStatus_Starting,
	ObjectServiceMapStatus_Updating,
} ObjectServiceMapStatus;

typedef struct ObjectServiceMap {
	uint32_t		version;
	ObjectServiceMapStatus	status;
	uint32_t		object_service_length;
	ObjectService		**object_services;
	Map			nodes_map;
	uint32_t		bset_length;
	BSet			**bset;
	uint16_t		replica_length;
} ObjectServiceMap;

typedef enum {
	ObjectServiceMapChangeOperation_AddObjectService = 1,
	ObjectServiceMapChangeOperation_RemoveObjectService = 2,
} ObjectServiceMapChangeOperation;

typedef struct ObjectServiceMapChangeLog {
	uint32_t			from_version;
	uint32_t			to_version;
	ObjectServiceMapChangeOperation	operation;
	uint32_t			from_os_node_id;
	uint32_t			to_os_node_id;
	uint32_t			moved_bset_length;
	uint32_t			moved_bset_ids[];
} ObjectServiceMapChangeLog;

typedef struct ClusterGetObjectServiceMapLatestVersionRequest {
        Request         super;
} ClusterGetObjectServiceMapLatestVersionRequest;

typedef struct ClusterGetObjectServiceMapLatestVersionResponse {
        Response        super;
        uint32_t        version;
} ClusterGetObjectServiceMapLatestVersionResponse;

typedef struct ClusterGetLatestObjectServiceMapRequest {
        Request         super;
} ClusterGetLatestObjectServiceMapRequest;

typedef struct ClusterGetLatestObjectServiceMapResponse {
        Response        	super;
        ObjectServiceMap	os_map;
} ClusterGetLatestObjectServiceMapResponse;

typedef struct ClusterAddObjectServiceRequest {
        Request         super;
        uint32_t	id;
        char            host[NETWORK_HOST_LEN + 1];
        int             port;
} ClusterAddObjectServiceRequest;

typedef struct ClusterAddObjectServiceResponse {
        Response        super;
} ClusterAddObjectServiceResponse;

typedef struct ClusterRemoveObjectServiceRequest {
        Request         super;
        uint32_t	os_id;
} ClusterRemoveObjectServiceRequest;

typedef struct ClusterRemoveObjectServiceResponse {
        Response        super;
} ClusterRemoveObjectServiceResponse;

typedef struct ClusterKeepAliveObjectServiceRequest {
        Request         	super;
        uint32_t		os_id;
        ObjectServiceStatus	status;
} ClusterKeepAliveObjectServiceRequest;

typedef struct ClusterKeepAliveObjectServiceResponse {
        Response        	super;
        uint32_t        	version;
        ObjectServiceStatus	status;
} ClusterKeepAliveObjectServiceResponse;

typedef struct ClusterKeepAliveClientRequest {
        Request         super;
} ClusterKeepAliveClientRequest;

typedef struct ClusterKeepAliveClientResponse {
        Response        super;
        uint32_t        version;
} ClusterKeepAliveClientResponse;

typedef struct ClusterGetObjectServiceMapChangeLogRequest {
        Request         super;
        uint32_t        version;
} ClusterGetObjectServiceMapChangeLogRequest;

typedef struct ClusterGetObjectServiceMapChangeLogResponse {
        Response        		super;
        ObjectServiceMapChangeLog	chang_log;
} ClusterGetObjectServiceMapChangeLogResponse;

typedef struct ClusterStatusRequest {
        Request         super;
} ClusterStatusRequest;

typedef struct ClusterStatusResponse {
        Response        	super;
        ObjectServiceMap	os_map;
        uint64_t		total_objects;
        uint64_t		used;
        uint64_t		free;
} ClusterStatusResponse;

#endif /* SHARE_CLUSTER_H_ */
