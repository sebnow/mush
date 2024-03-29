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
#include "prompt.h"
#include <stdlib.h>
#include <string.h>
#include "testing_util.h"

static char *g_prompt = NULL;

void cmd_prompt(int argc, char **argv)
{
	size_t newSize;
	char *promptPtr;
	char *str;
	int argi;

	if(g_prompt != NULL) {
		free(g_prompt);
		g_prompt = NULL;
	}

	/* If we don't have any arguments, just set an empty string */
	if(argc == 1) {
		g_prompt = malloc(1);
		if(promptPtr == NULL) {
			return;
		}
		*g_prompt = '\0';
		return;
	}

	newSize = 0;
	/* Determine the size of the prompt */
	for(argi = 1; argi < argc; argi++) {
		newSize = strlen(argv[argi]);
		newSize++; /* space */
	}
	/* Previous extra size for "space" can be used for the terminator */

	g_prompt = malloc(newSize * sizeof(*g_prompt));
	promptPtr = g_prompt;
	if(promptPtr == NULL) {
		return;
	}
	memset(g_prompt, 0, newSize * sizeof(*g_prompt));

	/* Copy the prompt */
	for(argi = 1; argi < argc; argi++) {
		str = argv[argi];
		strcat(g_prompt, str);
		if(argi < (argc - 1)) {
			promptPtr = g_prompt + strlen(g_prompt);
			*promptPtr = ' ';
			promptPtr++;
			*promptPtr = '\0';
		}
	}
}

char *getPrompt()
{
	return g_prompt;
}
