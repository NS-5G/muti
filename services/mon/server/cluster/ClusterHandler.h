/*
 * ClusterHandler.h
 *
 *  Created on: Jan 8, 2021
 *      Author: rick
 */

#ifndef CLUSTERHANDLER_H_
#define CLUSTERHANDLER_H_
#include <server/RequestHandler.h>
#include <stddef.h>

extern void ClusterActionGetObjectServiceMapLatestVersion(SRequest *);
extern void ClusterActionGetLatestObjectServiceMap(SRequest *);
extern void ClusterActionAddObjectService(SRequest *);
extern void ClusterActionRemoveObjectService(SRequest *);
extern void ClusterActionKeepAliveObjectService(SRequest *);
extern void ClusterActionKeepAliveClient(SRequest *);
extern void ClusterActionGetObjectServiceMapChangeLog(SRequest *);
extern void ClusterActionStatus(SRequest *);

extern Action ClusterActions[];

extern Request* ClusterRequestDecoderGetObjectServiceMapLatestVersion(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderGetLatestObjectServiceMap(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderAddObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderRemoveObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderKeepAliveObjectService(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderKeepAliveClient(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderGetObjectServiceMapChangeLog(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderStatus(char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);

extern RequestDecoder ClusterRequestDecoder[];

bool ClusterResponseEncoderGetObjectServiceMapLatestVersion(Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderGetLatestObjectServiceMap(Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderAddObjectService(Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderRemoveObjectService(Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderKeepAliveObjectService(Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderKeepAliveClient(Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderGetObjectServiceMapChangeLog(Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderStatus(Response *resp, char **buffer, size_t *buff_len, bool *free_resp);

extern ResponseEncoder ClusterResponseEncoder[];

#endif /* CLUSTERHANDLER_H_ */
