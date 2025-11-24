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

	// int pos
	// int len
	// int line_num
} token;

typedef struct lex_out {
	char* filename;
	char* buffer;
	token* tokens;
	// int *columns;
	// int *line;
} lex_out;

typedef struct lexer {
	char* buffer;
	int line_num;
	int pos;
} lexer;

// TODO: implement this
int token_line(lex_out lex, int token, int* o_len);

token** lex(char* buffer, int* tokenCount);

void tokens_print(token** tokens, int token_count);

void token_print(token* token);
#endif
