#ifndef MUSH_ERROR_H
#define MUSH_ERROR_H

typedef enum {
	kMushNoError = 0,
	kMushGenericError,
	kMushParseError,
	kMushExecutionError
} MushErrorCode;

MushErrorCode mushError();
void setMushError(MushErrorCode error_code);
const char *mushErrorDescription();
void setMushErrorDescription(char *description);

#endif /* MUSH_ERROR_H */
