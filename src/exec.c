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
#include "mush_error.h"

static void _executeBuiltinCommand(command_t *command)
{
	commandBuiltinFunction builtinFunc = NULL;
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
	assert(builtinFunc != NULL);
	builtinFunc(command->argc, command->argv);
	builtinFunc = NULL;
}

static glob_t *_globCommand(command_t *command)
{
	glob_t *globBuf;
	int index;
	globBuf = malloc(sizeof(*globBuf));
	if(command->argc > 0) {
		globBuf->gl_offs = 0;
		glob(command->argv[0], GLOB_NOCHECK, NULL, globBuf);
		for(index = 1; index < command->argc; index++) {
			glob(command->argv[index], GLOB_APPEND|GLOB_NOCHECK, NULL, globBuf);
		}
		command->path = globBuf->gl_pathv[0];
		command->argv = globBuf->gl_pathv;
		command->argc = globBuf->gl_pathc;
	}
	return globBuf;
}

int executeCommandsInQueue(queue_t *commandQueue)
{
	command_t *currentCommand = NULL;
	command_t *previousCommand = NULL;
	pid_t pid;
	int pipeDescriptors[2];
	int execStatus = 0;
	int pipeStatus;
	int waitStatus;
	glob_t *globBuf;
	int wasGlobUsed = 0;
	char *errorDescription = NULL;

	/* Check if we have something to execute */
	if(commandQueue == NULL) {
		return kMushNoError;
	}

	while(queueRemove(commandQueue, (void *)&currentCommand)) {
		assert(currentCommand != NULL);
		/* Create the pipe before forking */
		if(currentCommand->connectionMask == kCommandConnectionPipe) {
			pipeStatus = pipe(pipeDescriptors);
			if(pipeStatus != 0) {
				setMushError(kMushGenericError);
				setMushErrorDescription("unable to create pipe");
				free(errorDescription);
				errorDescription = NULL;
				return mushError();
			}
		}
		globBuf = _globCommand(currentCommand);
		wasGlobUsed = globBuf != NULL;
		/* Execute builtin command, pwd is a special case as it has output */
		if(commandIsBuiltIn(currentCommand)
		&& strncmp(currentCommand->path, "pwd", 3) != 0) {
			_executeBuiltinCommand(currentCommand);
			continue;
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
			if(strncmp(currentCommand->path, "pwd", 3) == 0) {
				_executeBuiltinCommand(currentCommand);
				exit(EXIT_SUCCESS);
			} else if(wasGlobUsed) {
				execStatus = execvp(currentCommand->path, &globBuf->gl_pathv[0]);
			} else {
				execStatus = execvp(currentCommand->path, currentCommand->argv);
			}
			if(execStatus != 0) {
				fprintf(stderr, "could not execute: %s\n", currentCommand->path);
				exit(kMushExecutionError);
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
		currentCommand = NULL;
	}
	/* FIXME: Dieing children can interrupt the loop, causing a NULL pointer to be
	   freed. This is a temporary solution. */
	if(previousCommand != NULL) {
		commandFree(previousCommand);
		previousCommand = NULL;
	}
	if(wasGlobUsed) {
		globfree(globBuf);
	}

	return 0;
}
