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

/*!
 \addtogroup command
 \{
 */

enum {
	kCommandConnectionNone = 0,
	kCommandConnectionPipe = 1,
	kCommandConnectionBackground = 2,
	kCommandConnectionSequential = 4,
};

typedef struct __command_t {
	/*! \brief path of the command */
	char *path;
	/*! \brief amount of command line arguments for the command */
	int argc;
	char **argv;
	/*! \brief path to redirect output, or \a NULL */
	char *redirectToPath;
	/*! \brief path from which input is redirected, or \a NULL */
	char *redirectFromPath;
	/*! \brief whether to pipe the output to the next command */
	int connectionMask;
} command_t;

command_t *commandNew();
void commandSetPath(command_t *command, char *path);
void commandSetArgs(command_t *command, int argc, char **argv);
void commandSetRedirectToPath(command_t *command, char *redirectToPath);
void commandSetRedirectFromPath(command_t *command, char *redirectFromPath);
void commandSetConnectionMask(command_t *command, int connectionMask);
void commandFree(command_t *command);
int commandIsBuiltIn(command_t *command);

/*!
 \}
 */