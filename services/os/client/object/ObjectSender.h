/*
 * ObjectRequestSender.h
 *
 *  Created on: Feb 6, 2021
 *      Author: Zhen Xiong
 */

#ifndef OBJECTSENDER_H_
#define OBJECTSENDER_H_

#include <client/RequestSender.h>

Response* ObjectResponseDecoderGet(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
Response* ObjectResponseDecoderPut(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
Response* ObjectResponseDecoderList(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
Response* ObjectResponseDecoderDelete(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
Response* ObjectResponseDecoderStop(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);


extern ResponseDecoder ObjectResponseDecoder[];

bool ObjectRequestEncoderGet(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ObjectRequestEncoderPut(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ObjectRequestEncoderList(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ObjectRequestEncoderDelete(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool ObjectRequestEncoderStop(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);

extern RequestEncoder ObjectRequestEncoder[];

#endif /* OBJECTSENDER_H_ */
