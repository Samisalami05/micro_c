#include "ast.h"
#include "lexer.h"

#include <stdlib.h>

#define ALLOC_COUNT 10

void init_root(ast_node* root) {
	root->type = NODE_ROOT;
	root->token_index = -1;
	root->data.node_array.count = 0;
}

void validate_size(ast_tree* ast, int num) {
	while (ast->count + num >= ast->allocated) {
		ast->nodes = realloc(ast->nodes, sizeof(ast_node) * (ast->allocated + ALLOC_COUNT));
		ast->allocated += ALLOC_COUNT;
	}
}

char is_operator(token* token) {
	TokenType type = token->type;
	return type == TOKEN_EQUALS || type == TOKEN_PLUS ||
		type == TOKEN_MINUS || type == TOKEN_STAR ||
		type == TOKEN_SLASH || type == TOKEN_PROCENT;
}

void parse_term(ast_tree* ast, token** tokens, int* pos) {
	validate_size(ast, 1);
	ast->nodes[*pos].type = NODE_INDENTIFIER;
	ast->nodes[*pos].token_index = *pos;
	ast->nodes[*pos].data.node_array.count = 0;
	
	*pos += 1;
}

void parse_binary_op(ast_tree* ast, token** tokens, int* pos) {
	token* token = tokens[*pos];

	if (is_operator(tokens[*pos + 1])) {
		// DO stuff
		printf("Need to parse operator: %c\n", tokens[*pos + 1]->start[0]);
	}
	else {
		parse_term(ast, tokens, pos);
	}
}

void parse(ast_tree* ast, token** tokens) {
	int i = 0;

	parse_binary_op(ast, tokens, &i);
}

ast_tree* ast_create(token** tokens, int token_count) {
	ast_tree* ast = malloc(sizeof(ast_tree));
	ast->count = 1;
	ast->allocated = 1;
	ast->root_index = 0;
	ast->nodes = malloc(sizeof(ast_node));
	init_root(&ast->nodes[0]);
	parse(ast, tokens);

	return ast;
}
