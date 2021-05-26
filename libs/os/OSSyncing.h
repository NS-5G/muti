/*
 * OSSyncing.h
 *
 *  Created on: May 26, 2021
 *      Author: root
 */

#ifndef OS_OSSYNCING_H_
#define OS_OSSYNCING_H_
#include <stdbool.h>
#include <cluster/ClusterMap.h>

typedef void (*OSSyncingCallback)(void *);

typedef struct OSSyncing OSSyncing;
typedef struct OSSyncingMethod {
        void    (*syncMasterNode)(OSSyncing*, ObjectServiceMap*, ObjectService *, OSSyncingCallback, void *);
        void    (*destroy)(OSSyncing*);
} OSSyncingMethod;

struct OSSyncing {
        void                    *p;
        OSSyncingMethod         *m;
};

typedef struct OSSyncingParam {

} OSSyncingParam;

bool initOSSyncing(OSSyncing*, OSSyncingParam*);

#endif /* OS_OSSYNCING_H_ */
