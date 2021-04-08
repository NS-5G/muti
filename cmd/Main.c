
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "Service.h"
#include "Stat.h"
#include "Storage.h"
#include "Ut.h"
#include "Main.h"

Command MutiCmds[] = {
	{"ut", UtDo, "Do internal unit tests, only for develop person."},
	{"service", ServiceDo, "Start all kinds of services(OS, MDS, MON ...)."},
	{"storage", StorageDo, "Storage related operations(put, get, delete ...)."},
	{"stat", StatDo, "Statistic related operations(Cluster, nodes ...)."},
	{(char *)NULL, (CommandFn)NULL, (char*)NULL}
};

Command *findCommand(Command *cmds, char *cnm) {
        int i = 0;
        while (cmds[i].name != NULL) {
                if (strcasecmp(cmds[i].name, cnm) == 0) {
                        return &cmds[i];
                }
                i ++;
        }
        return NULL;
}

int main(int argv, char **argvs) {
        int ret = 0;
        int i = 0, j, maxlen;

        if (argv > 1) {
        	Command *tcase = findCommand(MutiCmds, argvs[1]);
		if (tcase != NULL) {
			ret = (*tcase->cmd)(argv, argvs);
		} else {
			printf("Unknown command:%s\n", argvs[1]);
			goto wrong_cmd;
		}
        } else {
wrong_cmd:
                printf("Usage:./muti <command>\n\n");
                printf("These are common muti commands used in various situations:\n\n");

                maxlen = 0;
                i = 0;
                while (MutiCmds[i].name != NULL) {
                	if (strlen(MutiCmds[i].name) > maxlen) maxlen = strlen(MutiCmds[i].name);
                	i++;
                }
                i = 0;
                while (MutiCmds[i].name != NULL) {
                        printf("   %s   ", MutiCmds[i].name);
                        for(j = strlen(MutiCmds[i].name); j < maxlen; j++) printf(" ");
                        printf("%s\n", MutiCmds[i].desc);
                        i++;
                }
        }
        return ret;
}
