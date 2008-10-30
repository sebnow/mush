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

#ifndef COMMAND_H
#define COMMAND_H

/*!
 \addtogroup command
 \{
 */

enum {
	/*! \brief No connector between commands (default) */
	kCommandConnectionNone = 0,
	/*! \brief The command is to be piped to another command */
	kCommandConnectionPipe = 1,
	/*! \brief The command is to be executed in the background */
	kCommandConnectionBackground = 2,
	/*! \brief The command is to be executed sequentially in the foreground */
	kCommandConnectionSequential = 4,
};

/*! \brief Represents a command and all relevant information for execution */
typedef struct __command_t {
	/*! \brief path of the command */
	char *path;
	/*! \brief amount of command line arguments for the command */
	int argc;
	/*! \brief arguments provided for the command */
	char **argv;
	/*! \brief path to redirect output, or \a NULL */
	char *redirectToPath;
	/*! \brief path from which input is redirected, or \a NULL */
	char *redirectFromPath;
	/*! \brief whether to pipe the output to the next command */
	int connectionMask;
} command_t;

/*!
 \brief Initialize a new \c command_t structure

 As with malloc() and free(), each call to commandNew() should be paired with a
 call to commandFree().

 \return A pointer to an initialized \c command_t structure, or \c NULL on error
 */
command_t *commandNew();

/*!
 \brief Set the  \link command_t::path path \endlink of a \c command_t structure

 The \a path string is copied and assigned to the \link command_t::path path
 \endlink of \a command. The copied string will be freed when commandFree() is
 called.

 \param command a pointer to the \c command_t structure to manipulate
 \param path the path to be set
 */
void commandSetPath(command_t *command, char *path);

/*!
 \brief Assign an array of command line arguments to a \c command_t structure

 The \a argv array is only assigned to the \c command_t structure. The memory
 for this array is not managed by the \c command_t related functions. This array
 should be freed before commandFree() is called. If the \link command_t::path
 path \endlink was previously assigned, its memory will first be freed before
 assigning
 the new path.

 \param command a pointer to the \c command_t structure to manipulate
 \param argc amount of command line arguments (elements in \a argv)
 \param argv array of command line arguments
 */
void commandSetArgs(command_t *command, int argc, char **argv);

/*!
 \brief Set the \link command_t::redirectToPath redirectToPath \endlink of a
 \c command_t structure

 The \a redirectToPath string is copied and assigned to the
 \link command_t::redirectToPath redirectToPath \endlink of \a command. The
 copied string will be freed when commandFree() is called.

 \param command a pointer to the \c command_t structure to manipulate
 \param redirectToPath the path to be set
 */
void commandSetRedirectToPath(command_t *command, char *redirectToPath);

/*!
 \brief Set the \link command_t::redirectFromPath redirectFromPath \endlink of a
 \c command_t structure

 The \a redirectFromPath string is copied and assigned to the
 \link command_t::redirectFromPath redirectFromPath \endlink of \a command. The
 copied string will be freed when commandFree() is called.

 \param command a pointer to the \c command_t structure to manipulate
 \param redirectFromPath the path to be set
 */
void commandSetRedirectFromPath(command_t *command, char *redirectFromPath);

/*!
 \brief Assign the \link command_t::connectionMask connectionMask \endlink of a
 \c command_t structure
 \param command a pointer to the \c command_t structure to manipulate
 \param connectionMask the connection mask to be assigned
 */
void commandSetConnectionMask(command_t *command, int connectionMask);

/*!
 \brief Free memory allocated by commandNew()
 \param command a pointer to the \c command_t structure to be freed
 */
void commandFree(command_t *command);

/*!
 \brief Indicate whether the command is builtin or external
 \param command a pointer to the \c command_t structure
 \return \c 1 if the command is builtin, \c 0 otherwise
 */
int commandIsBuiltIn(command_t *command);

/*!
 \}
 */

#endif /* COMMAND_H */
