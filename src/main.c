#include "compiler/parser.h"
#include <stdio.h>
#include <stdlib.h>

#include <compiler/ast.h>
#include <compiler/lexer.h>

char* readFile(FILE* fp) {
	fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

	if (size < 0) { // error or empty
        fclose(fp);
        return NULL;
    }

	char *buffer = malloc(size + 1);
	size_t bytes_read = fread(buffer, 1, size, fp);
    fclose(fp);

    buffer[bytes_read] = '\0';

	return buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
		fprintf(stderr, "Usage: %s [FILE]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	FILE* fp = fopen(argv[1], "rb");
	
	if (fp == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	char* buffer = readFile(fp);
	
	int token_count;
	token** tokens = lex(buffer, &token_count);

	for (int i = 0; i < token_count; i++) {
		token* t = tokens[i];

		// Print token
		printf("TOKEN: length = %d, start = ", t->length);
		for (int j = 0; j < t->length; j++) {
			printf("%c", t->start[j]);
		}
		printf("\n");
	}

	ast_tree* ast = ast_create();
	parse(ast, tokens);
	ast_print(ast, tokens);
	return 0;
}
