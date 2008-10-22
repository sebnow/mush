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
#include <glob.h>
#include <assert.h>
#include "command.h"
#include "builtin.h"
#include "testing_util.h"

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
	} else if(strcmp(command->path, "pwd") == 0) {
		builtinFunc = cmd_pwd;
	} else if(strcmp(command->path, "cd") == 0) {
		builtinFunc = cmd_cd;
	}
	builtinFunc(command->argc, command->argv);
	builtinFunc = NULL;
}

void executeCommandsInQueue(queue_t *commandQueue)
{
	command_t *currentCommand = NULL;
	command_t *previousCommand = NULL;
	pid_t pid;
	int pipeDescriptors[2];
	int execStatus;
	int pipeStatus;
	int waitStatus;
	glob_t globBuf;
	int index;
	int wasGlobUsed = 0;

	/* Check if we have something to execute */
	if(commandQueue == NULL) {
		return;
	}

	while(queueRemove(commandQueue, (void *)&currentCommand)) {
		assert(currentCommand != NULL);
		if(commandIsBuiltIn(currentCommand)) {
			_executeBuiltinCommand(currentCommand);
		} else {
			/* Create the pipe before forking */
			if(currentCommand->connectionMask == kCommandConnectionPipe) {
				pipeStatus = pipe(pipeDescriptors);
				if(pipeStatus != 0) {
					fprintf(stderr, "mush: unable to create pipe\n");
				}
			}
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
				/* Send stdout to the write end of the pipe */
				if(currentCommand->connectionMask == kCommandConnectionPipe) {
					dup2(pipeDescriptors[1], fileno(stdout));
					close(pipeDescriptors[0]);
				}
				/* Redirect the read end into stdin */
				if(previousCommand != NULL && previousCommand->connectionMask == kCommandConnectionPipe) {
					dup2(pipeDescriptors[0], fileno(stdin));
					close(pipeDescriptors[1]);
				}
				/* Expand globs */
				if(currentCommand->argc > 1) {
					globBuf.gl_offs = 1;
					glob(currentCommand->argv[1], GLOB_DOOFFS|GLOB_NOCHECK, NULL, &globBuf);
					for(index = 2; index < currentCommand->argc; index++) {
						glob(currentCommand->argv[index], GLOB_DOOFFS|GLOB_APPEND|GLOB_NOCHECK, NULL, &globBuf);
					}
					globBuf.gl_pathv[0] = currentCommand->path;
					wasGlobUsed = 1;
					execStatus = execvp(currentCommand->path, &globBuf.gl_pathv[0]);
				} else {
					wasGlobUsed = 0;
					execStatus = execvp(currentCommand->path, currentCommand->argv);
				}
				if(execStatus != 0) {
					fprintf(stderr, "mush: could not execute: %s\n", currentCommand->path);
					exit(1);
				}
			} else {
				if(currentCommand->connectionMask != kCommandConnectionBackground) {
					waitpid(pid, &waitStatus, 0);
				}
			}
			if(previousCommand != NULL) {
				commandFree(previousCommand);
			}
			previousCommand = currentCommand;
		}
		free(currentCommand);
		currentCommand = NULL;
		if(wasGlobUsed) {
			globfree(&globBuf);
		}
	}
}
