/*
 * ClusterMap.h
 *
 *  Created on: Mar 7, 2021
 *      Author: Zhen Xiong
 */

#ifndef CLUSTER_CLUSTERMAP_H_
#define CLUSTER_CLUSTERMAP_H_
#include <stdbool.h>
#define MAXIPSIZE        63
#define MAXIPSIZE_STR    (MAXIPSIZE + 1)

typedef struct Node {
        uint32_t        id;

        char            ip[MAXIPSIZE_STR];
        int             port;
} Node;

typedef struct ClusterMap ClusterMap;
typedef struct ClusterMapMethod {
        void    (*destroy)(ClusterMap*);
} ClusterMapMethod;

struct ClusterMap {
        void                    *p;
        ClusterMapMethod        *m;
};

typedef struct ClusterMapParam {

} ClusterMapParam;

bool initClusterMap(ClusterMap*, ClusterMapParam*);

#endif /* CLUSTER_CLUSTERMAP_H_ */
