#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum TokenType {
	TOKEN_ERROR,
	TOKEN_NUMBER,
	TOKEN_IDENTIFIER,
	TOKEN_STRING,
	TOKEN_EOF,

	TOKEN_PARENTHESIS_OPEN,
	TOKEN_PARENTHESIS_CLOSE,
	TOKEN_BRACE_OPEN,
	TOKEN_BRACE_CLOSE,
	TOKEN_SEMICOLON,
} TokenType;

typedef struct token {
    TokenType type;
    const char* start;
	int length;
} token;


token** lex(char* buffer, int* tokenCount);

#endif
