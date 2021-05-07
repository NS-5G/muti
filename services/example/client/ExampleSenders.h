/*
 * RequestSenders.h
 *
 *  Created on: Feb 6, 2021
 *      Author: root
 */

#ifndef ExampleSENDERS_H_
#define ExampleSENDERS_H_
#include <example/client/foo/FooSender.h>
#include <example/share/ExampleResources.h>

static RequestSender ExampleSenders[] = {
                {ResourceIdFoo, FooRequestEncoder, FooResponseDecoder},
};

#endif /* ExampleSENDERS_H_ */
