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
#include <cluster/ClusterMap.h>

typedef enum {
	ClusterRequestId_GetObjectServiceMapLatestVersion = 0,
        ClusterRequestId_GetLatestObjectServiceMap,
        ClusterRequestId_AddObjectService,
	ClusterRequestId_RemoveObjectService,
	ClusterRequestId_KeepAliveObjectService,
	ClusterRequestId_KeepAliveClient,
	ClusterRequestId_GetObjectServiceMapChangeLog,
	ClusterRequestId_Status,
	ClusterRequestId_Stop,
} ClusterRequestId;

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

typedef struct ClusterStopRequest {
        Request         super;
} ClusterStopRequest;

typedef struct ClusterStopResponse {
        Response        super;
} ClusterStopResponse;

#endif /* SHARE_CLUSTER_H_ */
