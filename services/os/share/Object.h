/*
 * Object.h
 *
 *  Created on: Jan 30, 2021
 *      Author: root
 */

#ifndef RES_OBJECT_H_
#define RES_OBJECT_H_

#include <res/Resource.h>
#include <util/LinkedList.h>

typedef enum {
        ObjectRequestIdGet = 0,
        ObjectRequestIdPut = 1,
        ObjectRequestIdList = 2,
} ObjectRequestId;

typedef struct Object {
        ListElement     element;
        int             id;
        char            bucket[64];
        size_t          path_length;
        char            *path;
        size_t          size;
        char            data[];
} Object;

typedef struct ObjectGetRequest {
        Request         super;
        int             id;
} ObjectGetRequest;

typedef struct ObjectGetResponse {
        Response        super;
        Object          object;
} ObjectGetResponse;

typedef struct ObjectPutRequest {
        Request         super;
        Object          object;
} ObjectPutRequest;

typedef struct ObjectPutResponse {
        Response        super;
} ObjectPutResponse;

typedef struct ObjectListRequest {
        Request         super;
        int32_t         page;
        int32_t         page_size;
} ObjectListRequest;

typedef struct ObjectListResponse {
        Response        super;
        uint32_t        length;
        union {
        	ListHead	object_head;	// For decoder
        	ListHead        *object_list;	// For encoder
        };
} ObjectListResponse;

#endif /* RES_OBJECT_H_ */
