/*
 * Object.h
 *
 *  Created on: Jan 8, 2021
 *      Author: rick
 */

#ifndef OBJECTHANDLER_H_
#define OBJECTHANDLER_H_
#include <server/RequestHandler.h>
#include <stddef.h>

extern void ObjectActionGet(SRequest *);
extern void ObjectActionPut(SRequest *);
extern void ObjectActionList(SRequest *);
extern void ObjectActionDelete(SRequest *);
extern void ObjectActionStop(SRequest *);

extern Action ObjectActions[];

extern Request* ObjectRequestDecoderGet(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ObjectRequestDecoderPut(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ObjectRequestDecoderList(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ObjectRequestDecoderDelete(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* ObjectRequestDecoderStop(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);

extern RequestDecoder ObjectRequestDecoder[];

extern bool ObjectResponseEncoderGet(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
extern bool ObjectResponseEncoderPut(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
extern bool ObjectResponseEncoderList(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
extern bool ObjectResponseEncoderDelete(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
extern bool ObjectResponseEncoderStop(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);

extern ResponseEncoder ObjectResponseEncoder[];

#endif /* OBJECTHANDLER_H_ */
