#include "lexer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ALLOCATION_SIZE 20

void skip_whitespace(char* buffer, int* i) {
	while (buffer[*i] == ' ' || buffer[*i] == '\n' || buffer[*i] == '\t') {
		*i += 1;
	}
}

token* create_token(TokenType type, const char* start, int length) {
	token* t = malloc(sizeof(token));
	t->type = type;
	t->start = start;
	t->length = length;

	return t;
}

token* create_token_err(const char* message) {
	return create_token(TOKEN_ERROR, message, strlen(message));
}

char is_digit(char c) {
	return c >= '0' && c <= '9';
}

char is_alpha(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

token* number(char* buffer, int* i) {
	int start = *i - 1;
	
	while (is_digit(buffer[*i])) {
		*i += 1;
	}

	int length = *i - start;
	printf("TOKEN_NUMBER: length = %d, start = %c\n", length, buffer[start]);
	return create_token(TOKEN_NUMBER, &buffer[start], length);
}

token* identifier(char* buffer, int* i) {
	int start = *i - 1;
	
	while (is_alpha(buffer[*i]) || buffer[*i] == '_') {
		*i += 1;
	}

	int length = *i - start;
	printf("TOKEN_IDENTIFIER: length = %d, start = %c\n", length, buffer[start]);
	return create_token(TOKEN_IDENTIFIER, &buffer[start], length);
}

token* string(char* buffer, int* i) {
	int start = *i;

	while (buffer[*i] != '"' && buffer[*i] != '\n' && buffer[*i] != '\0') {
		*i += 1;
	}

	if (buffer[*i] == '\n' || buffer[*i] == '\0') {
		return create_token_err("Unterminated string");
	}
	
	int length = *i - start;

	*i += 1;
	printf("TOKEN_STRING: length = %d, start = %c\n", length, buffer[start]);
	return create_token(TOKEN_STRING, &buffer[start], length);
}

void skip_comment(char* buffer, int* i) {
	while (buffer[*i] == '/' && buffer[*i + 1] == '/') {
		char c;
		while ((c = buffer[*i]) != '\n' && c != '\0') {
			*i += 1;
		}
		if (buffer[*i] == '\n') {
			*i += 1;
		}
		skip_whitespace(buffer, i);
	}

	while (buffer[*i] == '/' && buffer[*i + 1] == '*') {
		*i += 1;
		while (buffer[*i] != '\0' && !(buffer[*i] == '*' && buffer[*i + 1] == '/')) {
			*i += 1;
		}
		*i += 2;
		skip_whitespace(buffer, i);
	}
}

token* scan_token(char* buffer, int* i) {
	skip_whitespace(buffer, i);
	skip_comment(buffer, i);

	char c = buffer[*i];
	if (c == '\0') return create_token(TOKEN_EOF, &buffer[*i], 1);

	*i += 1;

	if (is_digit(c)) return number(buffer, i);
	if (is_alpha(c) || c == '_') return identifier(buffer, i);

	switch (c) {
		case '(': return create_token(TOKEN_PARENTHESIS_OPEN, &buffer[*i - 1], 1);
		case ')': return create_token(TOKEN_PARENTHESIS_CLOSE, &buffer[*i - 1], 1);
		case '{': return create_token(TOKEN_BRACE_OPEN, &buffer[*i - 1], 1);
		case '}': return create_token(TOKEN_BRACE_CLOSE, &buffer[*i - 1], 1);
		case '[': return create_token(TOKEN_BRACKET_OPEN, &buffer[*i - 1], 1);
		case ']': return create_token(TOKEN_BRACKET_CLOSE, &buffer[*i - 1], 1);

		case ';': return create_token(TOKEN_SEMICOLON, &buffer[*i - 1], 1);
		case ',': return create_token(TOKEN_COMMA, &buffer[*i - 1], 1);
		
		case '=': return create_token(TOKEN_EQUALS, &buffer[*i - 1], 1);
		case '+': return create_token(TOKEN_PLUS, &buffer[*i - 1], 1);
		case '-': return create_token(TOKEN_MINUS, &buffer[*i - 1], 1);
		case '*': return create_token(TOKEN_STAR, &buffer[*i - 1], 1);
		case '/': return create_token(TOKEN_SLASH, &buffer[*i - 1], 1);
		case '%': return create_token(TOKEN_PROCENT, &buffer[*i - 1], 1);
		
		case '"': return string(buffer, i);
	}

	printf("Unexpected character: %c\n", c);
	return create_token_err("Unexpected character found");
}

token** lex(char* buffer, int* tokenCount) {
	*tokenCount = 0;
	token** tokens = malloc(sizeof(token*) * ALLOCATION_SIZE);

	int allocated = ALLOCATION_SIZE;

	int i = 0;
	while (buffer[i] != '\0') {
		if (allocated <= *tokenCount) {
			tokens = realloc(tokens, (*tokenCount + ALLOCATION_SIZE) * sizeof(token*));
			allocated += ALLOCATION_SIZE;
		}

		token* t = scan_token(buffer, &i);
		tokens[*tokenCount] = t;
		*tokenCount += 1;

		if (t->type == TOKEN_ERROR) {
			fprintf(stderr, "LEXER ERROR: %s\n", t->start);
		}
	}
	return tokens;
}
