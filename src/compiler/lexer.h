#ifndef LEXER_H
#define LEXER_H

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
	TOKEN_BRACKET_OPEN,
	TOKEN_BRACKET_CLOSE,

	TOKEN_SEMICOLON,
	TOKEN_COMMA,
	
	TOKEN_EQUALS,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_STAR,
	TOKEN_SLASH,
	TOKEN_PROCENT,
} TokenType;

typedef struct token {
    TokenType type;
    const char* start;
	int length;
} token;


token** lex(char* buffer, int* tokenCount);

void tokens_print(token** tokens, int token_count);

void token_print(token* token);
#endif
