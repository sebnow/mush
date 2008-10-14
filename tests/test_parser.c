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

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include "test_parser.h"
#include "queue.h"
#include "parser.h"
#include "command.h"

void testParseSingleCommand(void **state)
{
	queue_t *commands;
	char input[8] = "ls -l /";
	command_t *command;
	commands = commandQueueFromInput(input);
	assert_int_equal(queueCount(commands), 1);
	queueRemove(commands, (void *)&command);
	assert_string_equal(command->path, "ls");
	assert_int_equal(command->argc, 3);
	assert_string_equal(command->argv[0], "ls");
	assert_string_equal(command->argv[1], "-l");
	assert_string_equal(command->argv[2], "/");
	commandFree(command);
	queueFree(commands);
}

void testParseMultipleCommands(void **state)
{
	queue_t *commands;
	char *input = "ls -l; echo a b c; ps; uname -a";
	command_t *command;

	commands = commandQueueFromInput(input);
	assert_int_equal(queueCount(commands), 4);

	queueRemove(commands, (void *)&command);
	assert_string_equal(command->path, "ls");
	assert_int_equal(command->argc, 2);
	assert_string_equal(command->argv[0], "ls");
	assert_string_equal(command->argv[1], "-l");
	commandFree(command);

	queueRemove(commands, (void *)&command);
	assert_string_equal(command->path, "echo");
	assert_int_equal(command->argc, 4);
	assert_string_equal(command->argv[0], "echo");
	assert_string_equal(command->argv[1], "a");
	assert_string_equal(command->argv[2], "b");
	assert_string_equal(command->argv[3], "c");
	commandFree(command);

	queueRemove(commands, (void *)&command);
	assert_string_equal(command->path, "ps");
	assert_int_equal(command->argc, 1);
	assert_string_equal(command->argv[0], "ps");
	commandFree(command);

	queueRemove(commands, (void *)&command);
	assert_string_equal(command->path, "uname");
	assert_int_equal(command->argc, 2);
	assert_string_equal(command->argv[0], "uname");
	assert_string_equal(command->argv[1], "-a");
	commandFree(command);
	queueFree(commands);
}

void testParseTerminators(void **state)
{
	char *input = "a; b;c ;d| e|f |g& k&l &m";
	queue_t *commands;
	command_t *command;

	commands = commandQueueFromInput(input);

	queueRemove(commands, (void *)&command);
	assert_true(command->connectionMask == kCommandConnectionSequential);
	commandFree(command);
	queueRemove(commands, (void *)&command);
	assert_true(command->connectionMask == kCommandConnectionSequential);
	commandFree(command);
	queueRemove(commands, (void *)&command);
	assert_true(command->connectionMask == kCommandConnectionSequential);
	commandFree(command);
	queueRemove(commands, (void *)&command);
	assert_true(command->connectionMask == kCommandConnectionPipe);
	commandFree(command);
	queueRemove(commands, (void *)&command);
	assert_true(command->connectionMask == kCommandConnectionPipe);
	commandFree(command);
	queueRemove(commands, (void *)&command);
	assert_true(command->connectionMask == kCommandConnectionPipe);
	commandFree(command);
	queueRemove(commands, (void *)&command);
	assert_true(command->connectionMask == kCommandConnectionBackground);
	commandFree(command);
	queueRemove(commands, (void *)&command);
	assert_true(command->connectionMask == kCommandConnectionBackground);
	commandFree(command);
	queueRemove(commands, (void *)&command);
	assert_true(command->connectionMask == kCommandConnectionBackground);
	commandFree(command);
	queueFree(commands);
}

void testParseRedirection(void **state)
{
	char *input;
	queue_t *commands;
	command_t *command;

	input = "echo a > output";
	commands = commandQueueFromInput(input);
	queueRemove(commands, (void *)&command);
	assert_string_equal(command->redirectToPath, "output");
	assert_true(command->redirectFromPath == NULL);
	commandFree(command);

	input = "echo a < input";
	commands = commandQueueFromInput(input);
	queueRemove(commands, (void *)&command);
	assert_true(command->redirectToPath == NULL);
	assert_string_equal(command->redirectFromPath, "input");
	commandFree(command);
	queueFree(commands);

	input = "echo a < input > output";
	commands = commandQueueFromInput(input);
	queueRemove(commands, (void *)&command);
	assert_string_equal(command->redirectToPath, "output");
	assert_string_equal(command->redirectFromPath, "input");
	commandFree(command);
	queueFree(commands);

	input = "echo a < ";
	commands = commandQueueFromInput(input);
	assert_true(commands == NULL); /* parse error */
}

/* TODO: Test quotes */
