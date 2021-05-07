/*
 * ServerContextOs.c
 *
 *  Created on: Feb 3, 2021
 *      Author: root
 */

#include "ServerContextOs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void destroy(ServerContextOs* this) {
}

static ServerContextOsMethod method = {
        .destroy = destroy,
};

bool initServerContextOs(ServerContextOs* this, ServerContextOsParam* param) {
        bool rc = true;

        this->p = NULL;
        this->m = &method;
        
        return rc;
}


