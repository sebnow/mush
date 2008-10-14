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
#include "test_command.h"
#include "command.h"

void testCommandNew(void **state)
{
	command_t *command = commandNew();
	assert_true(command != NULL);
	assert_true(command->path == NULL);
	assert_true(command->argc == 0);
	assert_true(command->argv == NULL);
	assert_true(command->redirectToPath == NULL);
	assert_true(command->redirectFromPath == NULL);
	assert_true(command->connectionMask == kCommandConnectionNone);
	commandFree(command);
}

void testCommandSetPath(void **state)
{
	command_t *command = commandNew();
	char *path = "/bin/ls";
	commandSetPath(command, path);
	assert_string_equal(command->path, path);
	assert_false(command->path == path); /* should be deep copied */
	commandSetPath(command, NULL);
	assert_true(command->path == NULL);
	commandFree(command);
}

void testCommandSetArgs(void **state)
{
	command_t *command = commandNew();
	char *argv[] = {"a", "b", "cdef"};
	int argc = 3;
	assert_true(command->argv == argv);
	assert_int_equal(command->argc, argc);
	commandFree(command);
}

void testCommandSetRedirectToPath(void **state)
{
	command_t *command = commandNew();
	char *path = "/dev/urandom";
	commandSetRedirectToPath(command, path);
	assert_string_equal(command->redirectToPath, path);
	assert_true(command->redirectToPath != path);
	commandSetRedirectToPath(command, NULL);
	assert_true(command->redirectToPath == NULL);
	commandFree(command);
}

void testCommandSetRedirectFromPath(void **state)
{
	command_t *command = commandNew();
	char *path = "/dev/urandom";

	commandSetRedirectFromPath(command, path);
	assert_string_equal(command->redirectFromPath, path);
	assert_true(command->redirectFromPath != path);
	commandSetRedirectFromPath(command, NULL);
	assert_true(command->redirectFromPath == NULL);
	commandFree(command);
}

void testCommandSetConnectionMask(void **state)
{
	command_t *command = commandNew();
	commandSetConnectionMask(command, kCommandConnectionBackground);
	assert_true(command->connectionMask == kCommandConnectionBackground);
	commandSetConnectionMask(command, kCommandConnectionPipe);
	assert_true(command->connectionMask == kCommandConnectionPipe);
	commandSetConnectionMask(command, kCommandConnectionNone);
	assert_true(command->connectionMask == kCommandConnectionNone);
	commandFree(command);
}

void testCommandIsBuiltin(void **state)
{
	command_t *command = commandNew();
	commandSetPath(command, "ls");
	assert_false(commandIsBuiltIn(command));
	commandSetPath(command, "prompt");
	assert_true(commandIsBuiltIn(command));
	commandSetPath(command, "exit");
	assert_true(commandIsBuiltIn(command));
	commandSetPath(command, "cd");
	assert_true(commandIsBuiltIn(command));
	commandSetPath(command, "pwd");
	assert_true(commandIsBuiltIn(command));
	commandSetPath(command, "echo");
	assert_false(commandIsBuiltIn(command));
	commandFree(command);
}
