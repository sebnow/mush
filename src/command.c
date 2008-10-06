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
#include "command.h"
#include <string.h>
#include <stdlib.h>
#include "util.h"

command_t *commandNew()
{
	command_t *command = NULL;
	command = malloc(sizeof(*command));
	if(command == NULL) {
		return NULL;
	}
	command->path = NULL;
	command->argc = 0;
	command->argv = NULL;
	command->redirectToPath = NULL;
	command->redirectFromPath = NULL;
	command->connectionMask = kCommandConnectionNone;

	return command;
}

void commandSetPath(command_t *command, char *path)
{
	ASSERT(command != NULL, "command can not be null");
	command->path = strdup(path);
}

void commandSetArgs(command_t *command, int argc, char **argv)
{
	ASSERT(command != NULL, "command can not be null");
	command->argc = argc;
	command->argv = argv;
}

void commandSetRedirectToPath(command_t *command, char *redirectToPath)
{
	ASSERT(command != NULL, "command can not be null");
	if(redirectToPath == NULL) {
		command->redirectToPath = NULL;
	}
	command->redirectToPath = strdup(redirectToPath);
}

void commandSetRedirectFromPath(command_t *command, char *redirectFromPath)
{
	ASSERT(command != NULL, "command can not be null");
	if(redirectFromPath == NULL) {
		command->redirectFromPath = NULL;
	}
	command->redirectFromPath = strdup(redirectFromPath);
}

void commandSetConnectionMask(command_t *command, int connectionMask)
{
	ASSERT(command != NULL, "command can not be null");
	command->connectionMask = connectionMask;
}

void commandFree(command_t *command)
{
	char **argv = command->argv;
	ASSERT(command != NULL, "command can not be null");
	if(argv != NULL) {
		do{
			FREE(*argv);
			argv++;
		} while(*argv != NULL);
		FREE(command->argv);
		argv = NULL;
	}
	/* path is an alias for argv[0], which has already been freed */
	command->path = NULL;
	if(command->redirectToPath != NULL) {
		FREE(command->redirectToPath);
	}
	if(command->redirectFromPath != NULL) {
		FREE(command->redirectFromPath);
	}
	free(command);
}

int commandIsBuiltIn(command_t *command)
{
	return strcmp(command->path, "prompt") == 0
	    || strcmp(command->path, "exit") == 0
	    || strcmp(command->path, "pwd") == 0
	    || strcmp(command->path, "cd") == 0;
}
