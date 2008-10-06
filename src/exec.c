/*
 * Copyright (c) 2008 Sebastian Nowicki <sebnow@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "exec.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command.h"
#include "util.h"
#include "builtin.h"

static void _executeBuiltinCommand(command_t *command)
{
	commandBuiltinFunction builtinFunc;
	/* TODO: Ideally this should be in a hash table so it is easier to look up
	 the command and execute it. It would also allow them to be plugin-based
	 */
	/* Determine builtin command and execute it */
	if(strcmp(command->path, "prompt") == 0) {
		builtinFunc = cmd_prompt;
	} else if(strcmp(command->path, "exit") == 0) {
		builtinFunc = cmd_exit;
	}
	builtinFunc(command->argc, command->argv);
	builtinFunc = NULL;
}

void executeCommandsInQueue(queue_t *commandQueue)
{
	command_t *currentCommand = NULL;
	pid_t pid;
	int status;

	/* Check if we have something to execute */
	if(commandQueue == NULL) {
		return;
	}

	while(queueRemove(commandQueue, (void *)&currentCommand)) {
		ASSERT(currentCommand != NULL, "NULL command to be executed\n");
		if(commandIsBuiltIn(currentCommand)) {
			_executeBuiltinCommand(currentCommand);
		} else {
			pid = fork();
			/* Child */
			if(pid == 0) {
				/* Redirect stdout to file if file is specified */
				if(currentCommand->redirectToPath != NULL) {
					freopen(currentCommand->redirectToPath, "w", stdout);
				}
				/* Redirect stdin to file if file is specified */
				if(currentCommand->redirectFromPath != NULL) {
					freopen(currentCommand->redirectFromPath, "r", stdin);
				}
				status = execvp(currentCommand->path, currentCommand->argv);
				if(status != 0) {
					fprintf(stderr, "mush: could not execute: %s\n", currentCommand->path);
					exit(1);
				}
			} else {
				if(currentCommand->connectionMask != kCommandConnectionBackground) {
					waitpid(pid, &status, 0);
				}
			}
			commandFree(currentCommand);
		}
	}
}
