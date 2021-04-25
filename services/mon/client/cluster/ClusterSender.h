/*
 * ClusterRequestSender.h
 *
 *  Created on: Feb 6, 2021
 *      Author: Zhen Xiong
 */

#ifndef CLIENT_SENDER_CLUSTERQUESTSENDER_H_
#define CLIENT_SENDER_CLUSTERQUESTSENDER_H_

#include <client/RequestSender.h>

extern Response* ClusterResponseDecoderGetObjectServiceMapLatestVersion(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderGetLatestObjectServiceMap(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderAddObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderRemoveObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderKeepAliveObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderKeepAliveClient(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderGetObjectServiceMapChangeLog(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderStatus(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderStop(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);

extern ResponseDecoder ClusterResponseDecoder[];

bool ClusterRequestEncoderGetObjectServiceMapLatestVersion(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderGetLatestObjectServiceMap(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderAddObjectService(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderRemoveObjectService(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderKeepAliveObjectService(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderKeepAliveClient(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderGetObjectServiceMapChangeLog(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderStatus(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderStop(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);

extern RequestEncoder ClusterRequestEncoder[];

#endif /* CLIENT_SENDER_CLUSTERQUESTSENDER_H_ */
