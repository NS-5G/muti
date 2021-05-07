/*
 * ServerRequestHandlers.h
 *
 *  Created on: Jan 8, 2021
 *      Author: rick
 */

#ifndef MonHandlers_H_
#define MonHandlers_H_

#include <mon/server/cluster/ClusterHandler.h>
#include "../share/MonResources.h"

static RequestHandler MonHandlers[] = {
                {ResourceIdCluster, ClusterActions, ClusterRequestDecoder, ClusterResponseEncoder},
};


#endif /* MonHandlers_H_ */
