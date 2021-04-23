/*
 * ClusterHandler.c
 *
 *  Created on: Jan 12, 2021
 *      Author: Zhen Xiong
 */
#include <mon/server/cluster/ClusterHandler.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include <mon/share/Cluster.h>
#include <server/Server.h>
#include <network/Connection.h>
#include <network/Socket.h>
#include <Log.h>
#include <mon/server/ServerContextMon.h>

Action ClusterActions[] = {
        ClusterActionGetObjectServiceMapLatestVersion,
        ClusterActionGetLatestObjectServiceMap,
        ClusterActionAddObjectService,
        ClusterActionRemoveObjectService,
        ClusterActionKeepAliveObjectService,
        ClusterActionKeepAliveClient,
        ClusterActionGetObjectServiceMapChangeLog,
        ClusterActionStatus,
	ClusterActionStop,
};

void ClusterActionGetObjectServiceMapLatestVersion(SRequest *req) {
        ClusterGetObjectServiceMapLatestVersionRequest *request = (ClusterGetObjectServiceMapLatestVersionRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextMon *sctx = server->m->getContext(server);
        ObjectServiceMap *os_map = sctx->clusterMap.m->getObjectServiceMap(&sctx->clusterMap);

        ClusterGetObjectServiceMapLatestVersionResponse *resp = malloc(sizeof(*resp));
        resp->version = os_map->version;
        sctx->clusterMap.m->putObjectServiceMap(&sctx->clusterMap, os_map);
        resp->super.sequence = request->super.sequence;
        resp->super.error_id = 0;
        req->response = &resp->super;
        req->action_callback(req);
}

static void ClusterActionGetLatestObjectServiceMapFreeResponse(SRequest *req) {
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextMon *sctx = server->m->getContext(server);
        ClusterGetLatestObjectServiceMapResponse *resp = (ClusterGetLatestObjectServiceMapResponse *)req->response;

        sctx->clusterMap.m->putObjectServiceMap(&sctx->clusterMap, resp->os_map);
        free(resp);
}

void ClusterActionGetLatestObjectServiceMap(SRequest *req) {
        ClusterGetLatestObjectServiceMapRequest *request = (ClusterGetLatestObjectServiceMapRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextMon *sctx = server->m->getContext(server);
        ObjectServiceMap *os_map = sctx->clusterMap.m->getObjectServiceMap(&sctx->clusterMap);

        ClusterGetLatestObjectServiceMapResponse *resp = malloc(sizeof(*resp));
        resp->os_map = os_map;
        resp->super.sequence = request->super.sequence;
        resp->super.error_id = 0;
        req->response = &resp->super;
        req->free_response = ClusterActionGetLatestObjectServiceMapFreeResponse;
        req->action_callback(req);
}

typedef struct StatMap {
        ObjectServiceStatus     input;
        ObjectServiceStatus     output;
} StatMap;

static StatMap NoneStatMap[] = {
                {ObjectServiceStatus_Online, ObjectServiceStatus_Error},
                {ObjectServiceStatus_Syncing, ObjectServiceStatus_Error},
                {ObjectServiceStatus_Offline, ObjectServiceStatus_Error},
                {ObjectServiceStatus_ReadyToJoin, ObjectServiceStatus_Error},
                {ObjectServiceStatus_Error, ObjectServiceStatus_Error},
};

static StatMap OnlineStatMap[] = {
                {ObjectServiceStatus_Online, ObjectServiceStatus_Online},
                {ObjectServiceStatus_Syncing, ObjectServiceStatus_Syncing},
                {ObjectServiceStatus_Offline, ObjectServiceStatus_Offline},
                {ObjectServiceStatus_ReadyToJoin, ObjectServiceStatus_Syncing},
                {ObjectServiceStatus_Error, ObjectServiceStatus_Error},
};

static StatMap SyncingStatMap[] = {
                {ObjectServiceStatus_Online, ObjectServiceStatus_Online},
                {ObjectServiceStatus_Syncing, ObjectServiceStatus_Syncing},
                {ObjectServiceStatus_Offline, ObjectServiceStatus_Offline},
                {ObjectServiceStatus_ReadyToJoin, ObjectServiceStatus_Error},
                {ObjectServiceStatus_Error, ObjectServiceStatus_Error},
};

static StatMap OfflineStatMap[] = {
                {ObjectServiceStatus_Online, ObjectServiceStatus_Online},
                {ObjectServiceStatus_Syncing, ObjectServiceStatus_Syncing},
                {ObjectServiceStatus_Offline, ObjectServiceStatus_Offline},
                {ObjectServiceStatus_ReadyToJoin, ObjectServiceStatus_Syncing},
                {ObjectServiceStatus_Error, ObjectServiceStatus_Error},
};

static StatMap ReadyToJoinStatMap[] = {
                {ObjectServiceStatus_Online, ObjectServiceStatus_Error},
                {ObjectServiceStatus_Syncing, ObjectServiceStatus_Syncing},
                {ObjectServiceStatus_Offline, ObjectServiceStatus_Error},
                {ObjectServiceStatus_ReadyToJoin, ObjectServiceStatus_ReadyToJoin},
                {ObjectServiceStatus_Error, ObjectServiceStatus_Error},
};

static StatMap ErrorStatMap[] = {
                {ObjectServiceStatus_Online, ObjectServiceStatus_Error},
                {ObjectServiceStatus_Syncing, ObjectServiceStatus_Error},
                {ObjectServiceStatus_Offline, ObjectServiceStatus_Error},
                {ObjectServiceStatus_ReadyToJoin, ObjectServiceStatus_Error},
                {ObjectServiceStatus_Error, ObjectServiceStatus_Error},
};

static StatMap *MonStatMap[] = {
                OnlineStatMap,
                SyncingStatMap,
                OfflineStatMap,
                ReadyToJoinStatMap,
                ErrorStatMap
};

/*
 *      Object Service Stat             Mon Stat                Mon response
 *
 *      Online                          None                    Error
 *      Syncing                         None                    Error
 *      Offline                         None                    Error
 *      Ready to join                   None                    Error
 *      Error                           None                    Error
 *
 *      Online                          Online                  Online
 *      Syncing                         Online                  Syncing
 *      Offline                         Online                  Offline
 *      Ready to join                   Online                  Syncing
 *      Error                           Online                  Error
 *
 *      Online                          Syncing                 Online
 *      Syncing                         Syncing                 Syncing
 *      Offline                         Syncing                 Offline
 *      Ready to join                   Syncing                 Error
 *      Error                           Syncing                 Error
 *
 *      Online                          Offline                 Online
 *      Syncing                         Offline                 Syncing
 *      Offline                         Offline                 Offline
 *      Ready to join                   Offline                 Syncing
 *      Error                           Offline                 Error
 *
 *      Online                          Ready to join           Error
 *      Syncing                         Ready to join           Syncing
 *      Offline                         Ready to join           Error
 *      Ready to join                   Ready to join           Ready to join
 *      Error                           Ready to join           Error
 *
 *      Online                          Error                   Error
 *      Syncing                         Error                   Error
 *      Offline                         Error                   Error
 *      Ready to join                   Error                   Error
 *      Error                           Error                   Error
 */
void ClusterActionKeepAliveObjectService(SRequest *req) {
        ClusterKeepAliveObjectServiceRequest *request = (ClusterKeepAliveObjectServiceRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextMon *sctx = server->m->getContext(server);
        ObjectServiceMap *os_map = sctx->clusterMap.m->getObjectServiceMap(&sctx->clusterMap);
        ClusterKeepAliveObjectServiceResponse *resp = malloc(sizeof(*resp));

        ObjectService *os = os_map->os_map.m->get(&os_map->os_map, &request->os_id);
        if (os == NULL) {
                resp->status = NoneStatMap[request->status].output;
        } else {
                resp->status = MonStatMap[os->status][request->status].output;
                os->status = resp->status;
        }
        resp->version = os_map->version;
        sctx->clusterMap.m->putObjectServiceMap(&sctx->clusterMap, os_map);

        resp->super.sequence = request->super.sequence;
        resp->super.error_id = 0;
        req->response = &resp->super;
        req->action_callback(req);
}

void ClusterActionKeepAliveClient(SRequest *req) {
        ClusterKeepAliveClientRequest *request = (ClusterKeepAliveClientRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextMon *sctx = server->m->getContext(server);
        ObjectServiceMap *os_map = sctx->clusterMap.m->getObjectServiceMap(&sctx->clusterMap);
        ClusterKeepAliveClientResponse *resp = malloc(sizeof(*resp));

        resp->version = os_map->version;
        sctx->clusterMap.m->putObjectServiceMap(&sctx->clusterMap, os_map);

        resp->super.sequence = request->super.sequence;
        resp->super.error_id = 0;
        req->response = &resp->super;
        req->action_callback(req);
}

static void ClusterActionStatusFreeResponse(SRequest *req) {
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextMon *sctx = server->m->getContext(server);
        ClusterStatusResponse *resp = (ClusterStatusResponse *)req->response;

        sctx->clusterMap.m->putObjectServiceMap(&sctx->clusterMap, resp->os_map);
        free(resp);
}

void ClusterActionStatus(SRequest *req) {
        ClusterStatusRequest *request = (ClusterStatusRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextMon *sctx = server->m->getContext(server);
        ObjectServiceMap *os_map = sctx->clusterMap.m->getObjectServiceMap(&sctx->clusterMap);
        ClusterStatusResponse *resp = malloc(sizeof(*resp));

        resp->os_map = os_map;
        resp->total_objects = 0; // TODO
        resp->free = 0; // TODO
        resp->used = 0; // TODO
        resp->super.sequence = request->super.sequence;
        resp->super.error_id = 0;
        req->response = &resp->super;
        req->free_response = ClusterActionStatusFreeResponse;
        req->action_callback(req);
}

void ClusterActionAddObjectService(SRequest *req) {
        // TODO
}

void ClusterActionRemoveObjectService(SRequest *req) {
        // TODO
}

void ClusterActionGetObjectServiceMapChangeLog(SRequest *req) {
        // TODO
}

static void* clusterActionStopThread(void *p) {
        ServerContextMon *sctx = p;
        sleep(2);
        sem_post(&sctx->stop_sem);
        return NULL;
}

void ClusterActionStop(SRequest *req) {
        ClusterStopRequest *request = (ClusterStopRequest*) req->request;
        Socket* socket = req->connection->m->getSocket(req->connection);
        Server* server = socket->m->getContext(socket);
        ServerContextMon *sctx = server->m->getContext(server);

        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&tid, &attr, clusterActionStopThread, sctx);

        ClusterStopResponse *resp = malloc(sizeof(*resp));
        resp->super.sequence = request->super.sequence;
        resp->super.error_id = 0;
        req->response = &resp->super;
        req->action_callback(req);
}

RequestDecoder ClusterRequestDecoder[] = {
        ClusterRequestDecoderGetObjectServiceMapLatestVersion,
        ClusterRequestDecoderGetLatestObjectServiceMap,
        ClusterRequestDecoderAddObjectService,
        ClusterRequestDecoderRemoveObjectService,
        ClusterRequestDecoderKeepAliveObjectService,
        ClusterRequestDecoderKeepAliveClient,
        ClusterRequestDecoderGetObjectServiceMapChangeLog,
        ClusterRequestDecoderStatus,
	ClusterRequestDecoderStop,
};

Request* ClusterRequestDecoderGetObjectServiceMapLatestVersion(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        ClusterGetObjectServiceMapLatestVersionRequest *req = (ClusterGetObjectServiceMapLatestVersionRequest*)buffer;
        if (buff_len < sizeof(*req)) return NULL;
        *consume_len = sizeof(*req);
        *free_req = false;
        return &req->super;
}
Request* ClusterRequestDecoderGetLatestObjectServiceMap(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        ClusterGetLatestObjectServiceMapRequest *req = (ClusterGetLatestObjectServiceMapRequest*)buffer;
        if (buff_len < sizeof(*req)) return NULL;
        *consume_len = sizeof(*req);
        *free_req = false;
        return &req->super;
}

Request* ClusterRequestDecoderAddObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        // TODO
        return NULL;
}

Request* ClusterRequestDecoderRemoveObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        // TODO
        return NULL;
}

Request* ClusterRequestDecoderKeepAliveObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        ClusterKeepAliveObjectServiceRequest *req = (ClusterKeepAliveObjectServiceRequest*)buffer;
        if (buff_len < sizeof(*req)) return NULL;
        *consume_len = sizeof(*req);
        *free_req = false;
        return &req->super;
}

Request* ClusterRequestDecoderKeepAliveClient(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        ClusterKeepAliveClientRequest *req = (ClusterKeepAliveClientRequest*)buffer;
        if (buff_len < sizeof(*req)) return NULL;
        *consume_len = sizeof(*req);
        *free_req = false;
        return &req->super;
}

Request* ClusterRequestDecoderGetObjectServiceMapChangeLog(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        // TODO
        return NULL;
}

Request* ClusterRequestDecoderStatus(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        ClusterStatusRequest *req = (ClusterStatusRequest*)buffer;
        if (buff_len < sizeof(*req)) return NULL;
        *consume_len = sizeof(*req);
        *free_req = false;
        return &req->super;
}

Request* ClusterRequestDecoderStop(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        ClusterStopRequest *req = (ClusterStopRequest*)buffer;
        if (buff_len < sizeof(*req)) return NULL;
        *consume_len = sizeof(*req);
        *free_req = false;
        return &req->super;
}

ResponseEncoder ClusterResponseEncoder[] = {
        ClusterResponseEncoderGetObjectServiceMapLatestVersion,
        ClusterResponseEncoderGetLatestObjectServiceMap,
        ClusterResponseEncoderAddObjectService,
        ClusterResponseEncoderRemoveObjectService,
        ClusterResponseEncoderKeepAliveObjectService,
        ClusterResponseEncoderKeepAliveClient,
        ClusterResponseEncoderGetObjectServiceMapChangeLog,
        ClusterResponseEncoderStatus,
	ClusterResponseEncoderStop,
};

bool ClusterResponseEncoderGetObjectServiceMapLatestVersion(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        ClusterGetObjectServiceMapLatestVersionResponse *resp1 = (ClusterGetObjectServiceMapLatestVersionResponse*)resp;
        *buffer = (char *)resp1;
        *buff_len = sizeof(*resp1);
        *free_resp = false;
        return true;
}

bool ClusterResponseEncoderGetLatestObjectServiceMap(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        ClusterGetLatestObjectServiceMapResponse *resp1 = (ClusterGetLatestObjectServiceMapResponse*)resp;
        Socket* socket = conn_p->m->getSocket(conn_p);
        Server* server = socket->m->getContext(socket);
        ServerContextMon *sctx = server->m->getContext(server);
        ClusterMap *cmap = &sctx->clusterMap;
        char *os_map_buffer, *buf;
        ssize_t header_len, os_map_buffer_len;

        header_len = sizeof(int8_t) + sizeof(uint32_t);
        os_map_buffer_len = cmap->m->dumpObjectServiceMapLength(resp1->os_map);
        buf = malloc(header_len + os_map_buffer_len);
        *buffer = buf;
        *buff_len = header_len + os_map_buffer_len;
        os_map_buffer = buf + header_len;

        bool rc = cmap->m->dumpObjectServiceMap(resp1->os_map, os_map_buffer, os_map_buffer_len);
        if (rc == false) {
                free(buf);
                return rc;
        }
        *(int8_t*)buf = resp->error_id; buf += 1;
        *(uint32_t*)buf = resp->sequence;

        *free_resp = true;
        return true;
}

bool ClusterResponseEncoderAddObjectService(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        // TODO
        return true;
}

bool ClusterResponseEncoderRemoveObjectService(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        // TODO
        return true;
}

bool ClusterResponseEncoderKeepAliveObjectService(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        ClusterKeepAliveObjectServiceResponse *resp1 = (ClusterKeepAliveObjectServiceResponse*)resp;
        *buffer = (char *)resp1;
        *buff_len = sizeof(*resp1);
        *free_resp = false;
        return true;
}

bool ClusterResponseEncoderKeepAliveClient(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        ClusterKeepAliveClientResponse *resp1 = (ClusterKeepAliveClientResponse*)resp;
        *buffer = (char *)resp1;
        *buff_len = sizeof(*resp1);
        *free_resp = false;
        return true;
}

bool ClusterResponseEncoderGetObjectServiceMapChangeLog(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        // TODO
        return true;
}

bool ClusterResponseEncoderStatus(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        ClusterStatusResponse *resp1 = (ClusterStatusResponse*)resp;
        Socket* socket = conn_p->m->getSocket(conn_p);
        Server* server = socket->m->getContext(socket);
        ServerContextMon *sctx = server->m->getContext(server);
        ClusterMap *cmap = &sctx->clusterMap;
        char *os_map_buffer, *buf;
        ssize_t header_len, os_map_buffer_len;

        header_len = sizeof(int8_t) + sizeof(uint32_t) + sizeof(uint64_t) * 3;
        os_map_buffer_len = cmap->m->dumpObjectServiceMapLength(resp1->os_map);
        buf = malloc(header_len + os_map_buffer_len);
        *buffer = buf;
        *buff_len = header_len + os_map_buffer_len;
        os_map_buffer = buf + header_len;

        bool rc = cmap->m->dumpObjectServiceMap(resp1->os_map, os_map_buffer, os_map_buffer_len);
        if (rc == false) {
                free(buf);
                return rc;
        }
        *(int8_t*)buf = resp->error_id; buf += 1;
        *(uint32_t*)buf = resp->sequence; buf += 4;
        *(uint64_t*)buf = resp1->total_objects; buf += 8;
        *(uint64_t*)buf = resp1->used; buf += 8;
        *(uint64_t*)buf = resp1->free; buf += 8;
        *free_resp = true;
        return true;
}

bool ClusterResponseEncoderStop(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        ClusterStopResponse *resp1 = (ClusterStopResponse*)resp;
        *buffer = (char *)resp1;
        *buff_len = sizeof(*resp1);
        *free_resp = false;
        return true;
}


