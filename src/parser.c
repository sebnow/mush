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
#include "parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "testing_util.h"
#include "command.h"
#include "queue.h"
#include "mush_error.h"

enum {
	/*! No redirection */
	kRedirectionTypeNone = 0,
	/*! Output redirected */
	kRedirectionTypeOut,
	/*! Input redirected */
	kRedirectionTypeIn
};

enum {
	/*! State in which it is determined what the next element is */
	kMachineStateDefault = 0,
	/*! State in which data is prepared for parsing of the next command */
	kMachineStateInitial,
	/*! State in which data is prepared for parsing of a path element */
	kMachineStateEnteringPath,
	/*! State in which a path element is parsed */
	kMachineStateParsingPath,
	/*! State in which the data for parsing a path element is freed or reset */
	kMachineStateLeavingPath,
	/*! State in which data is prepared for parsing of a token */
	kMachineStateEnteringToken,
	/*! State in which a token is parsed */
	kMachineStateParsingToken,
	/*! State in which the data for parsing a token is freed or reset */
	kMachineStateLeavingToken,
	/*! State in which data is prepared for parsing of a redirection element */
	kMachineStateEnteringRedirection,
	/*! State in which a redirection element is parsed */
	kMachineStateParsingRedirection,
	/*! State in which the data for parsing a redirection element is freed or reset */
	kMachineStateLeavingRedirection,
	/*! State in which the command terminator is parsed */
	kMachineStateParsingCommandTerminator,
	/*! Terminal state of the FSM, where all data is freed */
	kMachineStateTerminal
};

/*!
 \brief Set the path of a command from another string
 
 \a n characters of \a path are copied and the string is terminated.
 
 \param command command to set the path value to
 \param path string to copy the path from
 \param n length of the string to copy
 */
static void _setCommandPath(command_t *command, const char *path, size_t n) {
	char *str;
	size_t size = (n + 1) * sizeof(*str);
	str = malloc(size);
	if(str == NULL) {
		return;
	}
	memset(str, 0, size);
	str = strncpy(str, path, n);
	*(str + n + 1) = '\0';
	commandSetPath(command, str);
	free(str);
}

static void _setConnectionMaskBasedOnCharacter(command_t *command, char chr) {
	assert(command != NULL);
	switch(chr) {
		case '|':
			commandSetConnectionMask(command, kCommandConnectionPipe);
			break;
		case '&':
			commandSetConnectionMask(command, kCommandConnectionBackground);
			break;
		case ';':
			commandSetConnectionMask(command, kCommandConnectionSequential);
			break;
		default:
			commandSetConnectionMask(command, kCommandConnectionNone);
	}
}

static int _isTerminator(char ch) {
	return (ch == '|' || ch == '&' || ch == ';');
}

static void _setRedirectionBasedOnType(command_t *command, int redirectionType, char *ptr, size_t n) {
	char *str;
	size_t size = (n + 1) * sizeof(*str);
	str = malloc(size);
	if(str == NULL) {
		return;
	}
	memset(str, 0, size);
	str = strncpy(str, ptr, n);
	*(str + size) = '\0';
	if(redirectionType == kRedirectionTypeOut) {
		commandSetRedirectToPath(command, str);
	} else if(redirectionType == kRedirectionTypeIn) {
		commandSetRedirectFromPath(command, str);
	}
	free(str);
}


static void _addTokensToCommand(queue_t *tokens, command_t *command)
{
	char *token;
	char **argv = NULL;
	size_t count;

	assert(command != NULL);

	count = queueCount(tokens) + 1;
	if(count == 0) {
		return;
	}
	argv = malloc((count + 1) * sizeof(*argv));
	if(argv == NULL) {
		return;
	}
	command->argc = count;
	command->argv = argv;
	/* The first element should point to the program filename */
	*argv = command->path;
	argv++;
	while(queueRemove(tokens, (void *)&token)) {
		*argv = token;
		argv++;
	}
	/* execvp requires a NULL terminated array */
	*argv = NULL;
}

static void _addTokenToQueue(queue_t *queue, char *token, size_t n) {
	char *str;
	size_t size = (n + 1) * sizeof(*str);
	str = malloc(size);
	if(str == NULL) {
		return;
	}
	memset(str, 0, size);
	str = strncpy(str, token, n);
	*(str + size) = '\0';
#if UNIT_TESTING
	queueInsert(queue, str, NULL);
#else
	queueInsert(queue, str, free);
#endif
}

queue_t *commandQueueFromInput(char *inputLine) {
	queue_t *commandQueue = queueNew();
	queue_t *tokens = queueNew();
	command_t *command = NULL;
	char *inputPtr = inputLine;
	char *errorDescription = NULL;
	char lastTerminator = 0;
	
	int currentState = kMachineStateInitial;

	char *dataStart = inputLine;
	char *dataEnd = NULL;
	int isFinishedParsing = 0;
	int redirectionType = kRedirectionTypeNone;

	int isInSingleQuote = 0;
	int isInDoubleQuote = 0;
	int isInQuote = 0;
	
	if(inputLine == NULL) {
		return NULL;
	}
	
	/* Traverse the input string until it is parsed */
	while(isFinishedParsing == 0) {
		if(*inputPtr == '\'' && !isInDoubleQuote) {
			isInSingleQuote = !isInSingleQuote;
		}
		if(*inputPtr == '"' && !isInSingleQuote) {
			isInDoubleQuote = !isInDoubleQuote;
		}
		isInQuote = isInDoubleQuote || isInSingleQuote;
		switch(currentState) {
			case kMachineStateInitial:
				/* Set everything up to be ready for parsing the next command */
				assert(command == NULL);
				/* Ignore whitespace */
				if(isspace(*inputPtr)) {
					continue;
				}
				if(*inputPtr == '\0') {
					if(lastTerminator == '|') {
						asprintf(&errorDescription, "parse error near '%c'", lastTerminator);
						setMushError(kMushParseError);
						setMushErrorDescription(errorDescription);
						free(errorDescription);
						errorDescription = NULL;
						queueFree(commandQueue);
						queueFree(tokens);
						return NULL;
					}
					currentState = kMachineStateTerminal;
				} else {
					command = commandNew();
					if(command == NULL) {
						return NULL;
					}
					currentState = kMachineStateEnteringPath;
				}
				lastTerminator = 0;
				break;
			case kMachineStateEnteringPath:
				/* Set up everything to parse the path */
				if(isspace(*inputPtr)) {
					inputPtr++;
				} else {
					dataStart = inputPtr;
					currentState = kMachineStateParsingPath;
				}
				break;
			case kMachineStateParsingPath:
				if((*inputPtr == ' ' && !isInQuote) || *inputPtr == '\0' || (_isTerminator(*inputPtr) && !isInQuote)) {
					currentState = kMachineStateLeavingPath;
				} else {
					inputPtr++;
				}
				break;
			case kMachineStateLeavingPath:
				/* Clean up after parsing the path */
				dataEnd = inputPtr;
				_setCommandPath(command, dataStart, dataEnd - dataStart);
				dataStart = NULL;
				dataEnd = NULL;
				currentState = kMachineStateDefault;
				break;
			case kMachineStateParsingCommandTerminator:
				if(command == NULL) {
					if(_isTerminator(*(inputPtr - 1))) {
						asprintf(&errorDescription, "parse error near '%c%c'", *(inputPtr - 1), *inputPtr);
						setMushError(kMushParseError);
						setMushErrorDescription(errorDescription);
						free(errorDescription);
						errorDescription = NULL;
						queueFree(commandQueue);
						queueFree(tokens);
						return NULL;
					}
					assert(0);
				}
				lastTerminator = *inputPtr;
				_setConnectionMaskBasedOnCharacter(command, *inputPtr);
				_addTokensToCommand(tokens, command);
				queueInsert(commandQueue, command, (queueNodeFreeFunction)commandFree);
				command = NULL;
				currentState = kMachineStateInitial;
				inputPtr++;
				break;
			case kMachineStateEnteringToken:
				assert(tokens != NULL);
				if(isspace(*inputPtr)) {
					inputPtr++;
				} else {
					dataStart = inputPtr;
					currentState = kMachineStateParsingToken;
				}
				break;
			case kMachineStateParsingToken:
				if((*inputPtr == ' ' && !isInQuote) || *inputPtr == '\0' || (_isTerminator(*inputPtr) && !isInQuote)) {
					currentState = kMachineStateLeavingToken;
				} else {
					inputPtr++;
				}
				break;
			case kMachineStateLeavingToken:
				dataEnd = inputPtr;
				_addTokenToQueue(tokens, dataStart, dataEnd - dataStart);
				dataStart = NULL;
				dataEnd = NULL;
				currentState = kMachineStateDefault;
				break;
			case kMachineStateEnteringRedirection:
				if(redirectionType == kRedirectionTypeNone) {
					if(*inputPtr == '<') {
						redirectionType = kRedirectionTypeIn;
						currentState = kMachineStateParsingRedirection;
					}	else if(*inputPtr == '>') {
						redirectionType = kRedirectionTypeOut;
						currentState = kMachineStateParsingRedirection;
					}
				}
				inputPtr++;
				break;
			case kMachineStateParsingRedirection:
				if(dataStart == NULL) {
					if(isspace(*inputPtr)) {
						inputPtr++;
					}	else if(*inputPtr == '\0' || _isTerminator(*inputPtr)) {
						asprintf(&errorDescription, "parse error near '%c%c'", *(inputPtr - 1), *inputPtr);
						setMushError(kMushParseError);
						setMushErrorDescription(errorDescription);
						free(errorDescription);
						errorDescription = NULL;
						queueFree(commandQueue);
						queueFree(tokens);
						return NULL;
					} else {
						dataStart = inputPtr;
						inputPtr++;
					}
				} else if(isspace(*inputPtr) || _isTerminator(*inputPtr) || *inputPtr == '\0' || *inputPtr == '<' || *inputPtr == '>') {
					currentState = kMachineStateLeavingRedirection;
				} else {
					inputPtr++;
				}
				break;
			case kMachineStateLeavingRedirection:
				dataEnd = inputPtr;
				_setRedirectionBasedOnType(command, redirectionType, dataStart, dataEnd - dataStart);
				dataStart = NULL;
				dataEnd = NULL;
				redirectionType = kRedirectionTypeNone;
				currentState = kMachineStateDefault;
				break;
			case kMachineStateTerminal:
				isFinishedParsing = 1;
				if(command != NULL) {
					_addTokensToCommand(tokens, command);
					queueInsert(commandQueue, command, (queueNodeFreeFunction)commandFree);
					command = NULL;
				}
				break;
			default:
			case kMachineStateDefault:
				/* Determine what the next element is */
				if(_isTerminator(*inputPtr)) {
					currentState = kMachineStateParsingCommandTerminator;
				} else if(*inputPtr == '>' || *inputPtr == '<') {
					currentState = kMachineStateEnteringRedirection;
				} else if(*inputPtr == '\0') {
					currentState = kMachineStateTerminal;
				} else if(isspace(*inputPtr)) {
					inputPtr++;
				} else if(command->path == NULL) {
					currentState = kMachineStateEnteringPath;
				} else {
					currentState = kMachineStateEnteringToken;
				}
				break;
		}
	}

	return commandQueue;
}
