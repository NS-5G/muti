/*
 * ClusterHandler.c
 *
 *  Created on: Jan 12, 2021
 *      Author: Zhen Xiong
 */
#include <mon/server/cluster/ClusterHandler.h>
#include <string.h>
#include <stdbool.h>

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
};

void ClusterActionGetObjectServiceMapLatestVersion(SRequest *req) {

}

void ClusterActionGetLatestObjectServiceMap(SRequest *req) {

}

void ClusterActionAddObjectService(SRequest *req) {

}

void ClusterActionRemoveObjectService(SRequest *req) {

}

void ClusterActionKeepAliveObjectService(SRequest *req) {

}

void ClusterActionKeepAliveClient(SRequest *req) {

}

void ClusterActionGetObjectServiceMapChangeLog(SRequest *req) {

}

void ClusterActionStatus(SRequest *req) {

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

ResponseEncoder ClusterResponseEncoder[] = {
        ClusterResponseEncoderGetObjectServiceMapLatestVersion,
        ClusterResponseEncoderGetLatestObjectServiceMap,
        ClusterResponseEncoderAddObjectService,
        ClusterResponseEncoderRemoveObjectService,
        ClusterResponseEncoderKeepAliveObjectService,
        ClusterResponseEncoderKeepAliveClient,
        ClusterResponseEncoderGetObjectServiceMapChangeLog,
        ClusterResponseEncoderStatus,
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



