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

#define FREE(p) do { free(p); p = NULL; } while(0)
#define STRDUP(r, s, action) do {\
	if(s != NULL) {\
		r = strdup(s);\
		if(r == NULL) {\
			action;\
		}\
	} else {\
		r = NULL;\
	}\
} while(0)

#ifdef NDEBUG
#define	ASSERTF(cond, str, ...) ((void)0)
#define	ASSERT(cond, str) ((void)0)
#else
#define ASSERTF(cond, str, ...) do {\
	if(!(cond)) {\
		printf("*** Assertion failure in %s, %s:%d\n", __func__, __FILE__, __LINE__);\
		printf(str, __VA_ARGS__);\
		abort();\
	}\
} while(0)
#define ASSERT(cond, str) ASSERTF(cond, str, NULL)
#endif

