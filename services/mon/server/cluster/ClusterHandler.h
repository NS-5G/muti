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
extern void ClusterActionStop(SRequest *);

extern Action ClusterActions[];

extern Request* ClusterRequestDecoderGetObjectServiceMapLatestVersion(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderGetLatestObjectServiceMap(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderAddObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderRemoveObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderKeepAliveObjectService(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderKeepAliveClient(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderGetObjectServiceMapChangeLog(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderStatus(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ClusterRequestDecoderStop(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);

extern RequestDecoder ClusterRequestDecoder[];

bool ClusterResponseEncoderGetObjectServiceMapLatestVersion(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderGetLatestObjectServiceMap(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderAddObjectService(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderRemoveObjectService(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderKeepAliveObjectService(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderKeepAliveClient(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderGetObjectServiceMapChangeLog(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderStatus(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ClusterResponseEncoderStop(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);

extern ResponseEncoder ClusterResponseEncoder[];

#endif /* CLUSTERHANDLER_H_ */
