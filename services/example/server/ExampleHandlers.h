/*
 * ServerRequestHandlers.h
 *
 *  Created on: Jan 8, 2021
 *      Author: rick
 */

#ifndef ExampleHANDLERS_H_
#define ExampleHANDLERS_H_

#include <example/server/foo/FooHandler.h>
#include "../share/ExampleResources.h"

static RequestHandler ExampleHandlers[] = {
                {ResourceIdFoo, FooActions, FooRequestDecoder, FooResponseEncoder},
};


#endif /* ExampleHANDLERS_H_ */
