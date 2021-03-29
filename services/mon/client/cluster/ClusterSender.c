/*
 * ClusterRequestSender.c
 *
 *  Created on: Jan 12, 2021
 *      Author: Zhen Xiong
 */
#include <mon/client/cluster/ClusterSender.h>
#include <string.h>

#include <stdbool.h>
#include <mon/share/Cluster.h>

Response* ClusterResponseDecoderGetObjectServiceMapLatestVersion(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Response* ClusterResponseDecoderGetLatestObjectServiceMap(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Response* ClusterResponseDecoderAddObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Response* ClusterResponseDecoderRemoveObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Response* ClusterResponseDecoderKeepAliveObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Response* ClusterResponseDecoderKeepAliveClient(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Response* ClusterResponseDecoderGetObjectServiceMapChangeLog(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}

Response* ClusterResponseDecoderStatus(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        return NULL;
}


ResponseDecoder ClusterResponseDecoder[] = {
                ClusterResponseDecoderGetObjectServiceMapLatestVersion,
                ClusterResponseDecoderGetLatestObjectServiceMap,
                ClusterResponseDecoderAddObjectService,
                ClusterResponseDecoderRemoveObjectService,
                ClusterResponseDecoderKeepAliveObjectService,
                ClusterResponseDecoderKeepAliveClient,
                ClusterResponseDecoderGetObjectServiceMapChangeLog,
                ClusterResponseDecoderStatus,
};

bool ClusterRequestEncoderGetObjectServiceMapLatestVersion(Request *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterRequestEncoderGetLatestObjectServiceMap(Request *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterRequestEncoderAddObjectService(Request *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterRequestEncoderRemoveObjectService(Request *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterRequestEncoderKeepAliveObjectService(Request *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterRequestEncoderKeepAliveClient(Request *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterRequestEncoderGetObjectServiceMapChangeLog(Request *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

bool ClusterRequestEncoderStatus(Request *resp, char **buffer, size_t *buff_len, bool *free_resp) {
        return true;
}

RequestEncoder ClusterRequestEncoder[] = {
                ClusterRequestEncoderGetObjectServiceMapLatestVersion,
                ClusterRequestEncoderGetLatestObjectServiceMap,
                ClusterRequestEncoderAddObjectService,
                ClusterRequestEncoderRemoveObjectService,
                ClusterRequestEncoderKeepAliveObjectService,
                ClusterRequestEncoderKeepAliveClient,
                ClusterRequestEncoderGetObjectServiceMapChangeLog,
                ClusterRequestEncoderStatus,
};

