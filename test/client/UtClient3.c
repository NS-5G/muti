/*
 * UtClient2.c
 *
 *  Created on: Feb 18, 2021
 *      Author: Zhen Xiong
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#include <Log.h>
#include <client/Client.h>
#include <util/ThreadPool.h>

#include <example/client/ExampleSenders.h>
#include <example/share/Foo.h>

static void UtClientFooPutSendCallback(Client *client, Response *resp, void *arg) {
        if (resp->error_id) {
//                ELOG("Send request failed.");
        }
        volatile int *done_number = arg;
        __sync_add_and_fetch(done_number, 1);
}

typedef struct UtClientDoTestParam {
	ThreadPool work_tp, write_tp, read_tp;
	volatile int done_thread;
	Client	client;
} UtClientDoTestParam;

static volatile int the_test_id = 0;

static void* UtClientDoTest(void *p) {
	UtClientDoTestParam *tp = p;
        int rc, id;
        Client *client = &tp->client;

        id = __sync_fetch_and_add(&the_test_id, 1);
        int round;
        volatile int done_number = 0;
        int round_max = 100000;
        int round_start = id * round_max;
        int round_end = id * round_max + round_max;
        for (round = round_start; round < round_end; round++) {
                FooPutRequest *sput = calloc(1, sizeof(*sput) + 1024);
                Foo *s1 = &sput->foo;
                strcpy(s1->bucket, "buck1");
                s1->id = round;
                s1->path = s1->data;
                sprintf(s1->path, "/var/log/message%dxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", round);
                s1->path_length = strlen(s1->path);
                s1->size = 1 << 30;

                Request *req = &sput->super;
                req->request_id = FooRequestIdPut;
                req->resource_id = ResourceIdFoo;

                bool free_req;

                rc = client->m->sendRequest(client, &sput->super, UtClientFooPutSendCallback, (void*)&done_number, &free_req);
                if (free_req) {
                        free(sput);
                }
                if (rc) {
//                        DLOG("Put success round:%d!!", round);
                } else {
                        __sync_add_and_fetch(&done_number, 1);
//                        DLOG("Send request failed!!");
                }
        }
        while (done_number != round_max) {
                usleep(1000000);
                DLOG("DONE:%d round_max:%d", done_number, round_max);
        }

        __sync_add_and_fetch(&tp->done_thread, 1);
        DLOG("Done thread:%d", tp->done_thread);
        return NULL;
}

int UtClient3(int argv, char **argvs) {
	UtClientDoTestParam tp;
	ThreadPoolParam param_tp;
	int thread_number = 10;
	pthread_attr_t attr;
	pthread_t thread_id;

	tp.done_thread = 0;

	param_tp.do_batch = NULL;
	param_tp.thread_number = 12;

	bool rc = initThreadPool(&tp.work_tp, &param_tp);
	assert(rc == true);

	param_tp.thread_number = 6;
	rc = initThreadPool(&tp.write_tp, &param_tp);
	assert(rc == true);

	rc = initThreadPool(&tp.read_tp, &param_tp);
	assert(rc == true);

        ClientParam param;

        strcpy(param.host, "127.0.0.1");
        param.port = 10809;
        param.read_buffer_size = 1 << 12;
        param.read_tp = &tp.read_tp;
        param.write_tp = &tp.write_tp;
        param.worker_tp = &tp.work_tp;
        param.request_sender = ExampleSenders;

        rc = initClient(&tp.client, &param);
        assert(rc == true);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	int i;
	for (i = 0; i < thread_number; i++) {
		pthread_create(&thread_id, &attr, UtClientDoTest, &tp);
	}


	while(thread_number != tp.done_thread) {
		usleep(1000);
	}
	tp.client.m->destroy(&tp.client);

	tp.read_tp.m->destroy(&tp.read_tp);
	tp.write_tp.m->destroy(&tp.write_tp);
	tp.work_tp.m->destroy(&tp.work_tp);

	return 0;
}
