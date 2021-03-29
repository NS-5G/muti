/*
 * MonSenders.h
 *
 *  Created on: Feb 6, 2021
 *      Author: root
 */

#ifndef CLIENT_SENDER_MONSENDERS_H_
#define CLIENT_SENDER_MONSENDERS_H_
#include <mon/client/cluster/ClusterSender.h>
#include <mon/share/MonResources.h>

static RequestSender MonSenders[] = {
                {ResourceIdCluster, ClusterRequestEncoder, ClusterResponseDecoder},
};

#endif /* CLIENT_SENDER_REQUESTSENDERS_H_ */
