/*
 * Service.c
 *
 *  Created on: Mar 2, 2021
 *      Author: Zhen Xiong
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdbool.h>

#include "Main.h"

#include <mon/server/ServerContextMon.h>
#include <server/Server.h>
#include <mon/server/MonHandlers.h>
#include <mon/client/MonSenders.h>
#include <util/ThreadPool.h>
#include <client/Client.h>
#include <Log.h>
#include <mon/share/Cluster.h>
#include <mon/share/MonResources.h>

#define DEFAULT_MON_INIT_PATH "./default_mon_init_path"

int ServiceMonStart(int argv, char **argvs) {
	Server server;
	ServerParam param;
	ThreadPool work_tp;
	ThreadPoolParam param_tp;
	ServerContextMon scxt;
	ServerContextMonParam param_cxt;

	param_tp.do_batch = NULL;
	param_tp.thread_number = 6;

	bool rc = initThreadPool(&work_tp, &param_tp);
	assert(rc == true);

	if (argv > 4) {
		strcpy(param_cxt.cmap_init_path, argvs[4]);
	} else {
		strcpy(param_cxt.cmap_init_path, DEFAULT_MON_INIT_PATH);
	}

	rc = initServerMonContext(&scxt, &param_cxt);
	assert(rc == true);

	param.port = 10809;
	param.read_buffer_size = 1 << 20;
	param.request_handler = MonHandlers;
	param.request_handler_length = sizeof(MonHandlers) / sizeof(RequestHandler);
	param.socket_io_thread_number = 1;
	param.worker_tp = &work_tp;
	param.max_read_buffer_counter = 16;
	param.context = &scxt;

	rc = initServer(&server, &param);
	assert(rc == true);
	sem_wait(&scxt.stop_sem);
	DLOG("Exiting...");
	server.m->destroy(&server);
	scxt.m->destroy(&scxt);
	work_tp.m->destroy(&work_tp);
	return 0;
}

int ServiceMonStop(int argv, char **argvs) {

        Client client;
        ClientParam param;
        ThreadPool work_tp, write_tp, read_tp;
        ThreadPoolParam param_tp;

        if (argv > 4) {
        	strcpy(param.host, argvs[4]);
        } else {
        	strcpy(param.host, "127.0.0.1");
        }

        if (argv > 5) {
        	int ret = sscanf(argvs[5], "%d", &param.port);
        	if (ret != 1) param.port = 10809;
        } else {
        	param.port = 10809;
        }

        param_tp.do_batch = NULL;
        param_tp.thread_number = 0;

        bool rc = initThreadPool(&work_tp, &param_tp);
        assert(rc == true);

        param_tp.thread_number = 1;
        rc = initThreadPool(&write_tp, &param_tp);
        assert(rc == true);

        rc = initThreadPool(&read_tp, &param_tp);
        assert(rc == true);

        param.read_buffer_size = 1 << 12;
        param.read_tp = &read_tp;
        param.write_tp = &write_tp;
        param.worker_tp = &work_tp;
        param.request_sender = MonSenders;

        rc = initClient(&client, &param);
        if (rc == false) {
        	ELOG("Error send stop request.");
        	goto out;
        }

        ClusterStopRequest *sreq = calloc(1, sizeof(*sreq) + 1024);
        Request *req = &sreq->super;
        req->request_id = ClusterRequestId_Stop;
        req->resource_id = ResourceIdCluster;

        rc = client.m->sendRequestSync(&client, &sreq->super);
        free(sreq);
        if (rc == false) {
        	ELOG("Error send stop request.");
        }
        DLOG("Exiting...");
        client.m->destroy(&client);
out:
        read_tp.m->destroy(&read_tp);
        write_tp.m->destroy(&write_tp);
        work_tp.m->destroy(&work_tp);
        return 0;
}

int ServiceMonDo(int argv, char **argvs) {
	if (argv > 3) {
		 if (strcasecmp(argvs[3], "start") == 0) {
			 return ServiceMonStart(argv, argvs);
		 } else if(strcasecmp(argvs[3], "stop") == 0) {
			 return ServiceMonStop(argv, argvs);
		 } else {
			 goto wrong_cmd;
		 }
	} else {
wrong_cmd:
		printf("Usage:./muti service mon <command>\n\n");
		printf("    start    Start mon service.");
		printf("    stop     Stop mon service.");
	}
	return 0;
}

int ServiceOsDo(int argv, char **argvs) {
	return 0;
}

int ServiceMdsDo(int argv, char **argvs) {
	return 0;
}

static Command ServiceCmds[] = {
	{"mon", ServiceMonDo, "Start mon service."},
	{"os", ServiceOsDo, "Start object service."},
	{"mds", ServiceMdsDo, "Start meta data service."},
	{(char *)NULL, (CommandFn)NULL, (char*)NULL}
};

int ServiceDo(int argv, char **argvs) {
        int ret = 0;
        int i = 0, j, maxlen;

        if (argv > 2) {
        	Command *tcase = findCommand(ServiceCmds, argvs[2]);
		if (tcase != NULL) {
			ret = (*tcase->cmd)(argv, argvs);
		} else {
			printf("Unknown command:%s\n", argvs[2]);
			goto wrong_cmd;
		}
        } else {
wrong_cmd:
                printf("Usage:./muti service <command>\n\n");
                printf("These are common muti service commands used in various situations:\n\n");

                maxlen = 0;
                i = 0;
                while (ServiceCmds[i].name != NULL) {
                	if (strlen(ServiceCmds[i].name) > maxlen) maxlen = strlen(ServiceCmds[i].name);
                	i++;
                }
                i = 0;
                while (ServiceCmds[i].name != NULL) {
                        printf("   %s   ", ServiceCmds[i].name);
                        for(j = strlen(ServiceCmds[i].name); j < maxlen; j++) printf(" ");
                        printf("%s\n", ServiceCmds[i].desc);
                        i++;
                }
        }
        return ret;
}
