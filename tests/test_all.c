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

/*! \defgroup unit_tests Unit Tests */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>

#include "test_queue.h"
#include "test_command.h"
#include "test_parser.h"
#include "test_builtin.h"

int main(int argc, char* argv[]) {
	const UnitTest tests[] = {
		unit_test(testQueueNew),
		unit_test(testQueueInsert),
		unit_test(testQueueRemove),
		unit_test(testQueueCount),
		unit_test(testCommandNew),
		unit_test(testCommandSetPath),
		unit_test(testCommandSetRedirectToPath),
		unit_test(testCommandSetRedirectFromPath),
		unit_test(testCommandSetConnectionMask),
		unit_test(testCommandIsBuiltin),
		unit_test(testParseSingleCommand),
		unit_test(testParseMultipleCommands),
		unit_test(testParseTerminators),
		unit_test(testParseRedirection),
		unit_test(testPrompt),
		unit_test(testCd),
	};
	return run_tests(tests);
}
