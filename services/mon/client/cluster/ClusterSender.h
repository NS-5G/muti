/*
 * ClusterRequestSender.h
 *
 *  Created on: Feb 6, 2021
 *      Author: Zhen Xiong
 */

#ifndef CLIENT_SENDER_CLUSTERQUESTSENDER_H_
#define CLIENT_SENDER_CLUSTERQUESTSENDER_H_

#include <client/RequestSender.h>

extern Response* ClusterResponseDecoderGetObjectServiceMapLatestVersion(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderGetLatestObjectServiceMap(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderAddObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderRemoveObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderKeepAliveObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderKeepAliveClient(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderGetObjectServiceMapChangeLog(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderStatus(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* ClusterResponseDecoderStop(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);

extern ResponseDecoder ClusterResponseDecoder[];

bool ClusterRequestEncoderGetObjectServiceMapLatestVersion(Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderGetLatestObjectServiceMap(Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderAddObjectService(Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderRemoveObjectService(Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderKeepAliveObjectService(Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderKeepAliveClient(Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderGetObjectServiceMapChangeLog(Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderStatus(Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterRequestEncoderStop(Request *resp, char **buffer, size_t *buff_len, bool *free_resp);

extern RequestEncoder ClusterRequestEncoder[];

#endif /* CLIENT_SENDER_CLUSTERQUESTSENDER_H_ */
