#include <compiler/ast.h>
#include <compiler/lexer.h>

#include <stdio.h>
#include <stdlib.h>

#define ALLOC_COUNT 10

int parse_add_expr(ast_tree* ast, token** tokens, int* pos);
int parse_mul_expr(ast_tree* ast, token** tokens, int* pos);

static char node_has_two(ast_node node) {
	return node.type == NODE_BINARY_OP;
}

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

int init_node(ast_tree* ast, int pos, ast_nodetype type) {
	validate_size(ast, 1);
	ast->nodes[ast->count].type = type;
	ast->nodes[ast->count].token_index = pos;
	ast->nodes[ast->count].data.node_array.count = 0;
	ast->count++;
	
	return ast->count-1;
}

int parse_primary_expr(ast_tree* ast, token** tokens, int* pos) {
	printf("primary_expr: %d\n", *pos);
	token* token = tokens[*pos];

	switch (token->type) { // TODO: add indentifier and parenthesis parsing
		case TOKEN_NUMBER: {
			   return init_node(ast, (*pos)++, NODE_NUMBER);
			}
		default:
			fprintf(stderr, "ERROR: token %c is not a primary expression\n", token->start[0]);
			return -1;
	
	}

	return -1;
}

int parse_mul_expr(ast_tree* ast, token** tokens, int* pos) {
	int l = parse_primary_expr(ast, tokens, pos); // call func with lower order of op
	if (l < 0) return -1;

	while (tokens[*pos]->type == TOKEN_STAR || tokens[*pos]->type == TOKEN_SLASH) {
		int op = init_node(ast, (*pos)++, NODE_BINARY_OP);
		int r = parse_primary_expr(ast, tokens, pos);
		if (r < 0) return -1;

		ast->nodes[op].data.two_nodes.node_l = l;
		ast->nodes[op].data.two_nodes.node_r = r;

		l = op;
	}
	return l;
}

int parse_add_expr(ast_tree* ast, token** tokens, int* pos) {
	int l = parse_mul_expr(ast, tokens, pos); // call func with lower order of op
	if (l < 0) return l;

	while (tokens[*pos]->type == TOKEN_PLUS || tokens[*pos]->type == TOKEN_MINUS) {
		int op = init_node(ast, (*pos)++, NODE_BINARY_OP);
		int r = parse_mul_expr(ast, tokens, pos);
		if (r < 0) return r;

		ast->nodes[op].data.two_nodes.node_l = l;
		ast->nodes[op].data.two_nodes.node_r = r;

		return op;
	}
	return l;
}

int parse_expression(ast_tree* ast, token** tokens, int* pos) {
	return parse_add_expr(ast, tokens, pos);
}

int parse_scope(ast_tree* ast, token** tokens, int* pos) {
	return 0;
}

void parse(ast_tree* ast, token** tokens) {
	int i = 0;

	int index = parse_expression(ast, tokens, &i);
	if (index < 0) {
		fprintf(stderr, "BIG ERROR MODE\n");
		exit(EXIT_FAILURE);
	}
	printf("parse success\n");

	ast->nodes[ast->root_index].data.node_array.count = 1;
	ast->nodes[ast->root_index].data.node_array.ast_indices[0] = index;
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

static void print_tab(int count) {
	for (int i = 0; i < count; i++) {
		printf("  ");
	}
}

static void ast_node_print(ast_tree* ast, int index, token** tokens, int tab) {
	ast_node node = ast->nodes[index];
	if (node.type == NODE_ROOT) {
		printf("ROOT\n");
	}
	else {
		token* token = tokens[node.token_index];
		
		print_tab(tab);
		for (int i = 0; i < token->length; i++) {
			printf("%c", token->start[i]);
		}
		printf("\n");
	}

	if (node_has_two(node)) {
		int a = node.data.two_nodes.node_l;
		int b = node.data.two_nodes.node_r;

		ast_node_print(ast, a, tokens, tab + 1);
		ast_node_print(ast, b, tokens, tab + 1);
	}
	else {
		for (int i = 0; i < node.data.node_array.count; i++) {
			int ind = node.data.node_array.ast_indices[i];
			ast_node_print(ast, ind, tokens, tab + 1);
		}
	}
}

void ast_print(ast_tree* ast, token** tokens) {
	ast_node_print(ast, ast->root_index, tokens, 0);
}
