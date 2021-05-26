/*
 * OSSyncing.c
 *
 *  Created on: May 26, 2021
 *      Author: root
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "OSSyncing.h"

typedef struct OSSyncingPrivate {
        OSSyncingParam       param;
} OSSyncingPrivate;

static void ossyncingSyncMasterNode(OSSyncing* ossync, ObjectServiceMap* os_map, ObjectService *os, OSSyncingCallback callback, void *arg) {
        // TODO
        callback(arg);
}

static void destroy(OSSyncing* this) {
        OSSyncingPrivate *priv_p = this->p;
        
        free(priv_p);
}

static OSSyncingMethod method = {
        .syncMasterNode = ossyncingSyncMasterNode,
        .destroy = destroy,
};

bool initOSSyncing(OSSyncing* this, OSSyncingParam* param) {
        OSSyncingPrivate *priv_p = malloc(sizeof(*priv_p));

        this->p = priv_p;
        this->m = &method;
        memcpy(&priv_p->param, param, sizeof(*param));

        
        return true;
}


