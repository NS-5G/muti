/*
 * ServerContextMon.c
 *
 *  Created on: Feb 3, 2021
 *      Author: root
 */

#include <mon/server/ServerContextMon.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void destroy(ServerContextMon* this) {
}

static ServerContextMonMethod method = {
        .destroy = destroy,
};

bool initServerMonContext(ServerContextMon* this, ServerContextMonParam* param) {
        bool rc = true;

        this->p = NULL;
        this->m = &method;

        return rc;
}


