/*
 * ServerRequestHandlers.h
 *
 *  Created on: Jan 8, 2021
 *      Author: rick
 */

#ifndef OsHandlers_H_
#define OsHandlers_H_

#include <os/server/object/ObjectHandler.h>
#include "../share/OsResources.h"

static RequestHandler OsHandlers[] = {
                {ResourceIdObject, ObjectActions, ObjectRequestDecoder, ObjectResponseEncoder},
};


#endif /* OsHandlers_H_ */
