/*
 * Foo.h
 *
 *  Created on: Jan 8, 2021
 *      Author: rick
 */

#ifndef RESSAMPLE_H_
#define RESSAMPLE_H_
#include <server/RequestHandler.h>
#include <stddef.h>

extern void FooActionGet(SRequest *);
extern void FooActionPut(SRequest *);
extern void FooActionList(SRequest *);

extern Action FooActions[];

extern Request* FooRequestDecoderGet(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* FooRequestDecoderPut(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);
extern Request* FooRequestDecoderList(Connection *conn_p, char *buffer, size_t buff_len, size_t *consume_len, bool *free_req);

extern RequestDecoder FooRequestDecoder[];

bool FooResponseEncoderGet(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool FooResponseEncoderPut(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);
bool FooResponseEncoderList(Connection *conn_p, Response *resp, char **buffer, size_t *buff_len, bool *free_resp);

extern ResponseEncoder FooResponseEncoder[];

#endif /* RESSAMPLE_H_ */
