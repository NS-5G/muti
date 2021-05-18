/*
 * HouseKeeping.h
 *
 *  Created on: May 18, 2021
 *      Author: root
 */

#ifndef UTIL_HOUSEKEEPING_H_
#define UTIL_HOUSEKEEPING_H_
#include <stdbool.h>
#include <util/ThreadPool.h>

typedef void (*HouseKeepingWorkerFun)(void*);

typedef struct HouseKeeping HouseKeeping;
typedef struct HouseKeepingMethod {
        void    (*addWorker)(HouseKeeping *, HouseKeepingWorkerFun, void*, int interval);
        void    (*destroy)(HouseKeeping*);
} HouseKeepingMethod;

struct HouseKeeping {
        void                    *p;
        HouseKeepingMethod      *m;
};

typedef struct HouseKeepingParam {
        ThreadPool      *working_tp;
} HouseKeepingParam;

bool initHouseKeeping(HouseKeeping*, HouseKeepingParam*);

#endif /* UTIL_HOUSEKEEPING_H_ */
