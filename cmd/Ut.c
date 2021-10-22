/*
 * Ut.c
 *
 *  Created on: Mar 2, 2021
 *      Author: Zhen Xiong
 */
#include "../cmd/Ut.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>


int UtThreadPool(int argv, char **argvs);
int UtMapHashLinked(int argv, char **argvs);
int UtSocketServer(int argv, char **argvs);
int UtSocketClient(int argv, char **argvs);
int UtSocketServer2(int argv, char **argvs);
int UtSocketClient2(int argv, char **argvs);
int UtClient(int argv, char **argvs);
int UtServer(int argv, char **argvs);
int UtClient2(int argv, char **argvs);
int UtClient3(int argv, char **argvs);

TestCase TestCases[] = {
        {"UtThreadPool", UtThreadPool},
        {"UtMapHashLinked", UtMapHashLinked},
        {"UtSocketServer", UtSocketServer},
        {"UtSocketClient", UtSocketClient},
        {"UtSocketServer2", UtSocketServer2},
        {"UtSocketClient2", UtSocketClient2},
        {"UtServer", UtServer},
        {"UtClient", UtClient},
	{"UtClient2", UtClient2},
	{"UtClient3", UtClient3},
        {(char *)NULL, (TestCaseFn)NULL}
};

TestCase *findTestCase(char *cnm) {
        int i = 0;
        while (TestCases[i].name != NULL) {
                if (strcasecmp(TestCases[i].name, cnm) == 0) {
                        return &TestCases[i];
                }
                i ++;
        }

        return NULL;
}

static inline suseconds_t
ut_time_diff(struct timeval start, struct timeval end)
{
	suseconds_t diff;
	if (end.tv_sec > start.tv_sec) {
		diff = (end.tv_sec - start.tv_sec - 1) * 1000000;
		diff += end.tv_usec + (1000000 - start.tv_usec);
	} else {
		//assert(end.tv_sec == start.tv_sec);
		//assert(end.tv_usec >= start.tv_usec);
		diff = end.tv_usec - start.tv_usec;
	}
	return diff;
}

int UtDo(int argv, char **argvs) {
	int ret, i;
	struct timeval start, end;

	if (argv > 2) {
		if (strcasecmp(argvs[2], "all") == 0 ) {
			int i = 0;
			while (TestCases[i].name != NULL) {

				printf("\nStart test case %s ...\n", TestCases[i].name);
				gettimeofday(&start, NULL);
				ret = (*TestCases[i].fn)(0, NULL);
				gettimeofday(&end, NULL);
				printf("\nEnd test case %s, Result: %d, Cost time: %ld\n", TestCases[i].name, ret, ut_time_diff(start, end));
				i ++;
			}
		} else {
			TestCase *tcase = findTestCase(argvs[2]);
			if (tcase != NULL) {
				gettimeofday(&start, NULL);
				ret = (*tcase->fn)(argv, argvs);
				gettimeofday(&end, NULL);
				printf("\nEnd test case %s, Result: %d, Cost time: %ld\n", tcase->name, ret, ut_time_diff(start, end));
			} else {
				goto wrong_test_nm;
			}
		}
	} else {
wrong_test_nm:
		i=0;
		printf("Available test names:\n");
                while (TestCases[i].name != NULL) {
                        printf("   %s   \n", TestCases[i].name);
                        i++;
                }
	}
	return ret;
}
