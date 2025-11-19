#include "ast.h"

#include <stdlib.h>

ast_node* create_root() {
	ast_node* root = malloc(sizeof(ast_node));
	root->type = NODE_ROOT;
	root->token_index = -1;
	root->data.node_array.count = 0;
	return root;
}

ast_node* parse_binary_op(ast_tree* ast, token** tokens, int* pos) {
	token* token = tokens[*pos];
	
	return NULL;
}


ast_tree* ast_create(token** tokens, int token_count) {
	ast_tree* ast = malloc(sizeof(ast_tree));
	ast->count = 1;
	ast->allocated = 1;
	ast->root_index = 0;
	ast->nodes = malloc(sizeof(ast_node));

	return ast;
}
