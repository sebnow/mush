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

#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include <unistd.h>
#include "test_builtin.h"
#include "command.h"
#include "builtin.h"

void testPrompt(void **state)
{
	char *argv1[3] = {"prompt", "testing", NULL};
	char *argv2[2] = {"prompt", NULL};
	char *argv3[6] = {"prompt", "this",  "is", "a", "test", NULL};
	cmd_prompt(2, argv1);
	assert_string_equal(getPrompt(), "testing");
	cmd_prompt(1, argv2);
	assert_string_equal(getPrompt(), "");
	cmd_prompt(5, argv3);
	assert_string_equal(getPrompt(), "this is a test");
}

void testCd(void **state)
{
	char *argv[3] = {"cd", "/", NULL};
	char *cwd;
	cmd_cd(2, argv);
	cwd = malloc(32 * sizeof(*cwd));
	cwd = getcwd(cwd, 32);
	assert_string_equal(cwd, "/");
	free(cwd);
}
