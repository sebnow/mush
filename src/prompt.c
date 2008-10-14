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
	newSize = 0;
	/* Determine the size of the prompt */
	for(argi = 1; argi < argc; argi++) {
		str = argv[argi];
		do{
			/* Add extra space for '%' characters to enable escaping */
			if(*str == '%') {
				newSize++;
			}
			newSize++;
		} while(*(++str) != '\0');
		newSize++; /* space */
	}
	newSize++; /* terminator */

	g_prompt = malloc(newSize * sizeof(*g_prompt));
	promptPtr = g_prompt;
	if(promptPtr == NULL) {
		return;
	}

	/* Copy the prompt */
	for(argi = 1; argi < argc; argi++) {
		str = argv[argi];
		do {
			/* Escpape '%' characters for printf */
			if(*str == '%') {
				*promptPtr = '%';
				*promptPtr++;
			}
			*promptPtr = *str;
			promptPtr++;
			str++;
		} while(*str != '\0');
		/* Only add a space between "arguments", not after the last one */
		if(argi != argc - 1) {
			*promptPtr = ' ';
		}
		promptPtr++;
	}
	*promptPtr = '\0';
}

char *getPrompt()
{
	return g_prompt;
}
