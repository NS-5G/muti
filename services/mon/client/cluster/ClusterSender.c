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
#include <client/Client.h>
#include <cluster/ClusterMap.h>

Response* ClusterResponseDecoderGetObjectServiceMapLatestVersion(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        SENDER_RESPONSE_DECODER(ClusterGetObjectServiceMapLatestVersionResponse);
}

Response* ClusterResponseDecoderGetLatestObjectServiceMap(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        ClusterGetLatestObjectServiceMapResponse *resp1 = NULL;
        ObjectServiceMap *os_map = NULL;
        Response *resp = (Response*)buffer;
        if (sizeof(*resp) > buff_len) return NULL;
        if (resp->error_id) {
                *consume_len = sizeof(Response);
                *free_resp = false;
                return resp;
        }

        resp1 = malloc(sizeof(*resp));
        ssize_t len = 0, len1;

        len += 4; if (buff_len < len) goto err_out;
        resp1->super.error_id = *(int32_t*)buffer;
        buffer += 4;

        len += 4; if (buff_len < len) goto err_out;
        resp1->super.sequence = *(uint32_t*)buffer;
        buffer += 4;

        os_map = calloc(1, sizeof(*os_map));
        bool rc = clusterMapParseObjectServiceMap(os_map,  buffer, (ssize_t)buff_len - len, &len1);
        if (rc == false) goto err_out;
        resp1->os_map = os_map;
        *consume_len = (size_t) len + len1;
        *free_resp = true;
	return &resp1->super;
err_out:
        if (resp1) free(resp1);
        if (os_map) free(os_map);
        return NULL;
}

Response* ClusterResponseDecoderAddObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        // TODO
        return NULL;
}

Response* ClusterResponseDecoderRemoveObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        // TODO
        return NULL;
}

Response* ClusterResponseDecoderKeepAliveObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        SENDER_RESPONSE_DECODER(ClusterKeepAliveObjectServiceResponse);
}

Response* ClusterResponseDecoderKeepAliveClient(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        SENDER_RESPONSE_DECODER(ClusterKeepAliveClientResponse);
}

Response* ClusterResponseDecoderGetObjectServiceMapChangeLog(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        // TODO
        return NULL;
}

Response* ClusterResponseDecoderStatus(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        ClusterStatusResponse *resp1 = NULL;
        ObjectServiceMap *os_map = NULL;
        Response *resp = (Response*)buffer;
        if (sizeof(*resp) > buff_len) return NULL;
        if (resp->error_id) {
                *consume_len = sizeof(Response);
                *free_resp = false;
                return resp;
        }

        resp1 = malloc(sizeof(*resp1));
        ssize_t len = 0, len1;

        len += 4; if (buff_len < len) goto err_out;
        resp1->super.error_id = *(int32_t*)buffer;
        buffer += 4;

        len += 4; if (buff_len < len) goto err_out;
        resp1->super.sequence = *(uint32_t*)buffer;
        buffer += 4;

        len += 8; if (buff_len < len) goto err_out;
        resp1->total_objects = *(uint32_t*)buffer;
        buffer += 8;

        len += 8; if (buff_len < len) goto err_out;
        resp1->used = *(uint32_t*)buffer;
        buffer += 8;

        len += 8; if (buff_len < len) goto err_out;
        resp1->free = *(uint32_t*)buffer;
        buffer += 8;

        os_map = calloc(1, sizeof(ObjectServiceMap));
        bool rc = clusterMapParseObjectServiceMap(os_map,  buffer, (ssize_t)buff_len - len, &len1);
        if (rc == false) goto err_out;
        resp1->os_map = os_map;
        *consume_len = (size_t) len + len1;
        *free_resp = true;
        return &resp1->super;
err_out:
        if (resp1) free(resp1);
        if (os_map) free(os_map);
        return NULL;
}

Response* ClusterResponseDecoderStop(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_resp) {
        SENDER_RESPONSE_DECODER(ClusterStopResponse);
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

bool ClusterRequestEncoderGetObjectServiceMapLatestVersion(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
	SENDER_REQUEST_ENCODER(ClusterGetObjectServiceMapLatestVersionRequest);
}

bool ClusterRequestEncoderGetLatestObjectServiceMap(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
	SENDER_REQUEST_ENCODER(ClusterGetLatestObjectServiceMapRequest);
}

bool ClusterRequestEncoderAddObjectService(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
        // TODO
	return true;
}

bool ClusterRequestEncoderRemoveObjectService(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
        // TODO
	return true;
}

bool ClusterRequestEncoderKeepAliveObjectService(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
	SENDER_REQUEST_ENCODER(ClusterKeepAliveObjectServiceRequest);
}

bool ClusterRequestEncoderKeepAliveClient(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
	SENDER_REQUEST_ENCODER(ClusterKeepAliveClientRequest);
}

bool ClusterRequestEncoderGetObjectServiceMapChangeLog(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
        // TODO
	return true;
}

bool ClusterRequestEncoderStatus(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
	SENDER_REQUEST_ENCODER(ClusterStatusRequest);
}

bool ClusterRequestEncoderStop(Connection *conn_p, Request *req, char **buffer, size_t *buff_len, bool *free_req) {
        SENDER_REQUEST_ENCODER(ClusterStopRequest);
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

