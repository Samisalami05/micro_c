#ifndef ERROR_H
#define ERROR_H

typedef enum error {
	ERROR_SUCCESS,
	ERROR_FAILURE,
	ERROR_OOM,
	ERROR_LEXER,
	ERROR_PARSER,
} error;

char* error_str(error e);

#endif
