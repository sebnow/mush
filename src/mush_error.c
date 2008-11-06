#include "mush_error.h"
#include <stdlib.h>
#include <string.h>

static int _mushErrorNumber = kMushNoError;
static char *_mushErrorDescription = NULL;

MushErrorCode mushError()
{
	return _mushErrorNumber;
}

void setMushError(MushErrorCode error_code)
{
	_mushErrorNumber = error_code;
}

const char *mushErrorDescription()
{
	return _mushErrorDescription;
}

void setMushErrorDescription(char *description)
{
	if(_mushErrorDescription != NULL) {
		free(_mushErrorDescription);
	}
	_mushErrorDescription = strdup(description);
}