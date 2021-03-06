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
#include <util/Map.h>
#include <util/LinkedList.h>
#include <util/HouseKeeping.h>

typedef enum ObjectServiceStatus {
	ObjectServiceStatus_Online = 0,
	ObjectServiceStatus_Syncing,
	ObjectServiceStatus_Offline,
	ObjectServiceStatus_ReadyToJoin,
	ObjectServiceStatus_Error,
} ObjectServiceStatus;

extern char *OSStatusMap[];

typedef struct BSet BSet;

typedef struct ObjectService {
        ListElement             element;
        uint32_t                id;
        ObjectServiceStatus     status;
        char                    host[NETWORK_HOST_LEN + 1];
        int32_t         	port;
        uint32_t                bset_length;
        uint32_t                *bset_ids;
        void			*user_define;
} ObjectService;

struct BSet {
	uint32_t                id;
	uint32_t                *object_service_ids;
};

typedef enum ObjectServiceMapStatus {
	ObjectServiceMapStatus_Normal = 1,
	ObjectServiceMapStatus_Updating,
} ObjectServiceMapStatus;

typedef struct ObjectServiceMap {
	uint32_t		version;
	ObjectServiceMapStatus	status;
	uint32_t		object_service_length;
	ListHead                object_service_list;
	Map			os_map;
	uint32_t		bset_length;
	BSet			*bset;
	uint16_t		bset_replica_size;
	volatile int            reference;
} ObjectServiceMap;

typedef enum {
	ObjectServiceMapChangeOperation_AddObjectService = 1,
	ObjectServiceMapChangeOperation_RemoveObjectService = 2,
} ObjectServiceMapChangeOperation;

typedef struct ObjectServiceMapChangeLog {
	uint32_t			from_version;
	uint32_t			to_version;
	ObjectServiceMapChangeOperation	operation;
	uint32_t			from_os_id;
	uint32_t			to_os_id;
	uint32_t			moved_bset_length;
	uint32_t			moved_bset_ids[];
} ObjectServiceMapChangeLog;

typedef enum ClusterMapType {
	ClusterMapType_Mon,
	ClusterMapType_Client,
	ClusterMapType_ObjectService,
} ClusterMapType;

typedef struct ClusterMap ClusterMap;
typedef struct ClusterMapMethod {
	int			(*removeObjectService)(ClusterMap*, uint32_t);
	int			(*addObjectService)(ClusterMap*, ObjectService*);
	ObjectServiceMap* 	(*getObjectServiceMap)(ClusterMap*);
	void                    (*putObjectServiceMap)(ClusterMap*, ObjectServiceMap *);
        void    		(*destroy)(ClusterMap*);
} ClusterMapMethod;

struct ClusterMap {
        void                    *p;
        ClusterMapMethod        *m;
};

typedef struct ClusterMapParam {
	ClusterMapType	type;
} ClusterMapParam;

typedef struct ClusterMapMonParam {
	ClusterMapParam	super;
	char		init_path[1024];
} ClusterMapMonParam;

typedef struct ClusterMapClientParam {
	ClusterMapParam	super;
	char		mon_host[NETWORK_HOST_LEN + 1];
	int		mon_port;
} ClusterMapClientParam;

struct OSSyncing;
typedef struct ClusterMapObjectServiceParam {
	ClusterMapParam	super;
	char		mon_host[NETWORK_HOST_LEN + 1];
	int		mon_port;
	uint32_t	os_id;
	ThreadPool      *work_tp;
	ThreadPool      *write_tp;
	ThreadPool      *read_tp;
	HouseKeeping    *hk;
	struct OSSyncing *ossync;
} ClusterMapObjectServiceParam;

bool initClusterMap(ClusterMap*, ClusterMapParam*);

extern ssize_t clusterMapDumpObjectServiceMapLength(ObjectServiceMap *);
extern bool clusterMapDumpObjectServiceMap(ObjectServiceMap *,  char *, ssize_t);
extern bool clusterMapParseObjectServiceMap(ObjectServiceMap *,  char *, ssize_t, ssize_t*);
extern void clusterMapFreeOSMap(ObjectServiceMap *os_map);
extern ObjectService* clusterMapGetObjectService(ObjectServiceMap *, uint32_t);

#endif /* CLUSTER_CLUSTERMAP_H_ */
