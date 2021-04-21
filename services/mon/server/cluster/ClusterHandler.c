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

void ClusterActionAddObjectService(SRequest *req) {
        // TODO
}

void ClusterActionRemoveObjectService(SRequest *req) {
        // TODO
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

}

void ClusterActionGetObjectServiceMapChangeLog(SRequest *req) {

}

void ClusterActionStatus(SRequest *req) {

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

Request* ClusterRequestDecoderGetObjectServiceMapLatestVersion(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}
Request* ClusterRequestDecoderGetLatestObjectServiceMap(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderAddObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderRemoveObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderKeepAliveObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderKeepAliveClient(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderGetObjectServiceMapChangeLog(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderStatus(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Request* ClusterRequestDecoderStop(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        ClusterStopRequest *req = (ClusterStopRequest*)buffer;
        if (buff_len < sizeof(req)) return NULL;
        *consume_len = sizeof(req);
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

bool ClusterResponseEncoderGetObjectServiceMapLatestVersion(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderGetLatestObjectServiceMap(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderAddObjectService(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderRemoveObjectService(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderKeepAliveObjectService(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderKeepAliveClient(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderGetObjectServiceMapChangeLog(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderStatus(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterResponseEncoderStop(Response *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        ClusterStopResponse *resp1 = (ClusterStopResponse*)resp;
        *buffer = (char *)resp1;
        *buff_len = sizeof(resp1);
        *free_resp = false;
        return true;
}


