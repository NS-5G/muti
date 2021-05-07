/*
 * ServerFooContext.h
 *
 *  Created on: Feb 3, 2021
 *      Author: root
 */

#ifndef SERVER_SERVEROSCONTEXT_H_
#define SERVER_SERVEROSCONTEXT_H_
#include <stdbool.h>

typedef struct ServerContextOs ServerContextOs;
typedef struct ServerContextOsMethod {
        void    (*destroy)(ServerContextOs*);
} ServerContextOsMethod;

struct ServerContextOs {
        void                    *p;
        ServerContextOsMethod   *m;
};

typedef struct ServerContextOsParam {

} ServerContextOsParam;

bool initServerContextOs(ServerContextOs*, ServerContextOsParam*);

#endif /* SERVER_SERVEROSCONTEXT_H_ */
