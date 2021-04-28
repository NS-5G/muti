/*
 * Resource.h
 *
 *  Created on: Jan 30, 2021
 *      Author: root
 */

#ifndef RES_RESOURCE_H_
#define RES_RESOURCE_H_

#include <stdint.h>
#include <stdio.h>

typedef struct Response {
        int32_t         error_id;
        uint32_t        sequence;
} Response;

typedef struct Request {
        uint16_t        resource_id;
        uint16_t        request_id;
        uint32_t        sequence;
} Request;

typedef struct Readbuffer {
        char                    *buffer;
        off_t                   processed_buffer_start;
        off_t                   read_buffer_start;
        volatile uint32_t       reference;
} Readbuffer;

#endif /* RES_RESOURCE_H_ */
