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
#include <Log.h>

Response* ClusterResponseDecoderGetObjectServiceMapLatestVersion(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        ClusterGetObjectServiceMapLatestVersionResponse *resp1 = (ClusterGetObjectServiceMapLatestVersionResponse*)buffer;
        if (sizeof(*resp1) > buff_len) return NULL;
        *consume_len = sizeof(*resp1);
        *free_req = false;
        return &resp1->super;
}

Response* ClusterResponseDecoderGetLatestObjectServiceMap(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
	// TODO
	return NULL;
//        ClusterGetLatestObjectServiceMapResponse *resp1 = (ClusterGetLatestObjectServiceMapResponse*)buffer;
//        if (sizeof(*resp1) > buff_len) return NULL;
//        *consume_len = sizeof(*resp1);
//        *free_req = false;
//        return &resp1->super;
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

Response* ClusterResponseDecoderStop(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req) {
        ClusterStopResponse *resp1 = (ClusterStopResponse*)buffer;
        if (sizeof(*resp1) > buff_len) return NULL;
        *consume_len = sizeof(*resp1);
        *free_req = false;
        DLOG("ClusterResponseDecoderStop done!");
        return &resp1->super;
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
		ClusterResponseDecoderStop,
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

bool ClusterRequestEncoderStop(Request *req, char **buffer, size_t *buff_len, bool *free_req) {
        ClusterStopRequest *req1 = (ClusterStopRequest*)req;
        *buffer = (char*) req1;
        *buff_len = sizeof(*req1);
        *free_req = false;
        DLOG("ClusterRequestEncoderStop done!");
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
		ClusterRequestEncoderStop,
};

