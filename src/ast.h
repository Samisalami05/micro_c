#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef enum ast_nodetype {
	NODE_ROOT,
	NODE_INDENTIFIER,
	NODE_CONSTANT,
	NODE_STRING_LITERAL,
	NODE_EXPRESSION,

	NODE_FUNC_DECL,
	NODE_BLOCK, // Internal block of the function
	
	NODE_RETURN,
	BINARY_OP,
} ast_nodetype;

typedef struct ast_node {
	ast_nodetype type;
	int token_index;

	union {
		struct {
			int *ast_indices;
			int count;
		} node_array;
		struct {
			int node_l;
			int node_r;
		} two_nodes;
	} data;
} ast_node;

typedef struct {
	ast_node *nodes;
	int count;
	int allocated;
	int root_index;
} ast_tree;

ast_tree* ast_create(token** tokens, int token_count);

#endif
