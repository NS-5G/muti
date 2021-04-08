/*
 * Main.h
 *
 *  Created on: Apr 8, 2021
 *      Author: Zhen Xiong
 */

#ifndef MAIN_H_
#define MAIN_H_

typedef int(*CommandFn)(int argv, char **argvs);

typedef struct Command {
	char		*name;
	CommandFn	cmd;
	char		*desc;
} Command;

extern Command *findCommand(Command *cmds, char *cnm);

#endif /* MAIN_H_ */
