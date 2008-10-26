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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "exec.h"
#include "prompt.h"
#include "testing_util.h"

/*!
 \brief Main program loop
 */
static void run();

/*!
 \brief Retrieve input from the user
 \return input string
 */
static const char *getInput();

/*! \brief Initial size of the input buffer */
#define INITIAL_INPUT_BUFFER_SIZE 24

int main()
{
	char *prompt_argv[2] = {"prompt", "% "};
	cmd_prompt(2, prompt_argv);
	run();
	return 0;
}

void run()
{
	char *input = NULL;
	char *prompt = NULL;
	queue_t *commandQueue = NULL;
	do {
		prompt = getPrompt();
		printf("%s", prompt);
		/* Free input buffer from previous loop run */
		if(input != NULL) {
			free(input);
		}
		input = (char *)getInput();
		commandQueue = commandQueueFromInput(input);
		executeCommandsInQueue(commandQueue);
		queueFree(commandQueue);
	} while(1);
}

const char *getInput()
{
	char *buffer = malloc(INITIAL_INPUT_BUFFER_SIZE * sizeof(char));
	int input;
	size_t inputLength = 0;
	size_t bufferSize = INITIAL_INPUT_BUFFER_SIZE;
	if(buffer == NULL) {
		return buffer;
	}
	input = fgetc(stdin);
	/* Get input until we get EOF or a new line */
	while(input != EOF && (char)input != '\n') {
		/* Increase the buffer size if necessary */
		if(inputLength >= bufferSize) {
			bufferSize = bufferSize * 1.5;
			buffer = realloc(buffer, bufferSize * sizeof(*buffer));
		}
		buffer[inputLength] = (char)input;
		inputLength++;
		input = fgetc(stdin);
	}
	buffer[inputLength] = '\0';
	return buffer;
}
