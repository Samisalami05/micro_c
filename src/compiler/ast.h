#ifndef AST_H
#define AST_H

#include <compiler/lexer.h>

typedef enum ast_nodetype {
	NODE_ROOT,
	NODE_IDENTIFIER,
	NODE_NUMBER,
	NODE_STRING_LITERAL,
	NODE_EXPRESSION,
	NODE_STATEMENT,

	NODE_FUNC_DECL,
	NODE_BLOCK, // Internal block of the function
	
	NODE_RETURN,

	NODE_BINARY_OP,
	NODE_ASSIGNMENT_OP,
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

int init_node(ast_tree* ast, int t_pos, ast_nodetype type);

ast_tree* ast_create();

void ast_print(ast_tree* ast, token** tokens);

#endif
