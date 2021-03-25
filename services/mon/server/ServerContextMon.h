/*
 * ServerContextMon.h
 *
 *  Created on: Feb 3, 2021
 *      Author: root
 */

#ifndef MON_SERVERMONCONTEXT_H_
#define MON_SERVERMONCONTEXT_H_
#include <stdbool.h>

typedef struct ServerContextMon ServerContextMon;
typedef struct ServerContextMonMethod {
        void    (*destroy)(ServerContextMon*);
} ServerContextMonMethod;

struct ServerContextMon {
        void                            *p;
        ServerContextMonMethod      *m;
};

typedef struct ServerContextMonParam {

} ServerContextMonParam;

bool initServerMonContext(ServerContextMon*, ServerContextMonParam*);

#endif /* MON_SERVERMONCONTEXT_H_ */
