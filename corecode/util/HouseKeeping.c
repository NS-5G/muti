/*
 * HouseKeeping.c
 *
 *  Created on: May 19, 2021
 *      Author: rick
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>

#include <util/HouseKeeping.h>
#include <util/LinkedList.h>

typedef struct HouseKeepingPrivate {
	HouseKeepingParam	param;
	ListHead		worker_list;
	pthread_mutex_t		worker_list_lock;
	pthread_t 		worker_thread_id;
	bool			worker_stop;
} HouseKeepingPrivate;

typedef enum HouseKeepingWorkerStatus {
	HouseKeepingWorkerStatus_Waiting,
	HouseKeepingWorkerStatus_Busy
} HouseKeepingWorkerStatus;

typedef struct HouseKeepingWorker {
	Job				job;
	ListElement			element;
	HouseKeepingWorkerStatus	status;
	HouseKeepingWorkerFun		fun;
	void				*argument;
	int				interval;
	int				counter;
} HouseKeepingWorker;

static void houseKeepingWorkerDo(Job *job_p) {
	HouseKeepingWorker *hk_worker = containerOf(job_p, HouseKeepingWorker, job);

	hk_worker->fun(hk_worker->argument);
	assert(hk_worker->status == HouseKeepingWorkerStatus_Busy
		&& hk_worker->counter == 0);
	hk_worker->counter = hk_worker->interval;
	hk_worker->status = HouseKeepingWorkerStatus_Waiting;
}

static void houseKeepingAddWorker(HouseKeeping *this, HouseKeepingWorkerFun fun, void *argument, int interval) {
	HouseKeepingPrivate *priv_p = this->p;
	HouseKeepingWorker *hk_worker = malloc(sizeof(*hk_worker));

	hk_worker->fun = fun;
	hk_worker->argument = argument;
	hk_worker->interval = interval;
	hk_worker->status = HouseKeepingWorkerStatus_Waiting;
	hk_worker->counter = interval;
	hk_worker->job.doJob = houseKeepingWorkerDo;

	pthread_mutex_lock(&priv_p->worker_list_lock);
	listAdd(&hk_worker->element, &priv_p->worker_list);
	pthread_mutex_unlock(&priv_p->worker_list_lock);
}

static void destroy(HouseKeeping* this) {
	HouseKeepingPrivate *priv_p = this->p;
	HouseKeepingWorker *hk_worker, *hk_worker1;

	priv_p->worker_stop = true;
	pthread_join(priv_p->worker_thread_id, NULL);
	listForEachEntrySafe(hk_worker, hk_worker1, &priv_p->worker_list, element) {
		listDel(&hk_worker->element);
		free(hk_worker);
	}
	free(priv_p);
}

static HouseKeepingMethod method = {
	.addWorker = houseKeepingAddWorker,
        .destroy = destroy,
};

static void* houseKeepingWorkerThread(void *p) {
	HouseKeeping* this = p;
	HouseKeepingPrivate *priv_p = this->p;
	HouseKeepingWorker *hk_worker;
	ThreadPool *tp = priv_p->param.working_tp;
	int left;
	bool rc;

	while(!priv_p->worker_stop) {
		pthread_mutex_lock(&priv_p->worker_list_lock);
		listForEachEntry(hk_worker, &priv_p->worker_list, element) {
			if (hk_worker->status == HouseKeepingWorkerStatus_Busy) continue;
			left = --hk_worker->counter;
			if (left == 0) {
				hk_worker->status = HouseKeepingWorkerStatus_Busy;
				rc = tp->m->insertTail(tp, &hk_worker->job);
				assert(rc == true);
			}
		}
		pthread_mutex_unlock(&priv_p->worker_list_lock);
		sleep(1);
	}
	return NULL;
}

bool initHouseKeeping(HouseKeeping* this, HouseKeepingParam* param) {
	HouseKeepingPrivate *priv_p = malloc(sizeof(*priv_p));

	this->p = priv_p;
	this->m = &method;
	memcpy(&priv_p->param, param, sizeof(*param));
	listHeadInit(&priv_p->worker_list);
	pthread_mutex_init(&priv_p->worker_list_lock, NULL);
	priv_p->worker_stop = false;

	int ret = pthread_create(&priv_p->worker_thread_id, NULL, houseKeepingWorkerThread, (void *)this);
	if (ret != 0) return false;
	return true;
}


