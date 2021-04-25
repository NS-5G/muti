/*
 * FooRequestSender.h
 *
 *  Created on: Feb 6, 2021
 *      Author: Zhen Xiong
 */

#ifndef CLIENT_SENDER_SAMPLEREQUESTSENDER_H_
#define CLIENT_SENDER_SAMPLEREQUESTSENDER_H_

#include <client/RequestSender.h>

extern Response* FooResponseDecoderGet(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* FooResponseDecoderPut(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Response* FooResponseDecoderList(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);

extern ResponseDecoder FooResponseDecoder[];

bool FooRequestEncoderGet(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool FooRequestEncoderPut(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool FooRequestEncoderList(Connection *conn_p, Request *resp, char **buffer, size_t *buff_len, bool *free_resp);

extern RequestEncoder FooRequestEncoder[];

#endif /* CLIENT_SENDER_SAMPLEREQUESTSENDER_H_ */
