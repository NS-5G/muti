/*
 * RequestSenders.h
 *
 *  Created on: Feb 6, 2021
 *      Author: root
 */

#ifndef OSSENDERS_H_
#define OSSENDERS_H_
#include <os/client/object/ObjectSender.h>
#include <os/share/OsResources.h>

static RequestSender OsSenders[] = {
                {ResourceIdObject, ObjectRequestEncoder, ObjectResponseDecoder},
};

#endif /* OSSENDERS_H_ */
