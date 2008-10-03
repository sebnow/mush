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
#include "path.h"
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "util.h"

#define PATH_SEPARATOR ':'

static const char *defaultPath = "/bin:/sbin:/usr/bin:/usr/sbin";

static char *filepathForCommandInDirectory(char *command, char *directoryPath)
{
	DIR *directory;
	struct dirent *entry;
	char *filepath = NULL;

	directory = opendir(directoryPath);
	if(entry == NULL) {
		return NULL;
	}
	
	do {
		entry = readdir(directory);
		if(entry != NULL) {
			if(strcmp(entry->d_name, command) == 0) {
				filepath = malloc((strlen(directoryPath) + strlen(command) + 2) * sizeof(*filepath));
				filepath = strcat(filepath, directoryPath);
				filepath = strcat(filepath, "/");
				filepath = strcat(filepath, command);
			}
		}
	} while(entry != NULL && filepath == NULL);

	closedir(directory);
	return filepath;
}

char *filepathForCommand(char *command)
{
	char *envPath = getenv("PATH");
	char *path;
	char *pathPtr = NULL;
	char separator[2] = {PATH_SEPARATOR, '\0'};
	char *filepath = NULL;
	
	ASSERT(command != NULL, "command can not be NULL");
	
	if(strchr(command, '/') != NULL) {
		return command;
	}
	
	if(envPath == NULL) {
		path = strdup(defaultPath);
	} else {
		path = strdup(envPath);
	}
	
	/* Tokenize PATH and search for command in each PATH element */
	pathPtr = strtok(path, separator);
	if(pathPtr != NULL) {
		filepath = filepathForCommandInDirectory(command, pathPtr);
		if(filepath == NULL) {
			do {
				pathPtr = strtok(NULL, separator);
				filepath = filepathForCommandInDirectory(command, pathPtr);
			} while(pathPtr != NULL && filepath == NULL);
		}
	}
	free(path);
	return filepath;
}
