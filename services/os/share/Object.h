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
	ObjectRequestIdDelete = 3,
} ObjectRequestId;

typedef struct Object {
        ListElement     element;
        size_t          path_length;
        char            *path;
        size_t          size;
        char            data[];
} Object;

typedef struct ObjectGetRequest {
        Request         super;
        size_t          path_length;
        char            *path;
        char		data[];
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
        size_t          prefix_path_length;
        char            *prefix_path;
        int32_t         page;
        int32_t         page_size;
        char		data[];
} ObjectListRequest;

typedef struct ObjectListResponse {
        Response        super;
        uint32_t        length;
        union {
        	ListHead	object_head;	// For decoder
        	ListHead        *object_list;	// For encoder
        };
} ObjectListResponse;

typedef struct ObjectDeleteRequest {
        Request         super;
        size_t          path_length;
        char            *path;
        char		data[];
} ObjectDeleteRequest;

typedef struct ObjectDeleteResponse {
        Response        super;
} ObjectDeleteResponse;

#endif /* RES_OBJECT_H_ */
