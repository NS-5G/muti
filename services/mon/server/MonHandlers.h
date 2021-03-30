/*
 * ServerRequestHandlers.h
 *
 *  Created on: Jan 8, 2021
 *      Author: rick
 */

#ifndef ServerRequestHandlers_H_
#define ServerRequestHandlers_H_

#include <mon/server/cluster/ClusterHandler.h>
#include "../share/MonResources.h"

static RequestHandler MonHandlers[] = {
                {ResourceIdCluster, ClusterActions, ClusterRequestDecoder, ClusterResponseEncoder},
};


#endif /* ServerRequestHandlers_H_ */
