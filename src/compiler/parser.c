#include "ast.h"
#include "lexer.h"
#include <compiler/parser.h>
#include <stdio.h>
#include <stdlib.h>

static int parse_primary_expr(ast_tree* ast, token** tokens, int* pos);
static int parse_parenthesis(ast_tree* ast, token** tokens, int* pos);
static int parse_expression(ast_tree* ast, token** tokens, int* pos);
static int parse_add_expr(ast_tree* ast, token** tokens, int* pos);
static int parse_mul_expr(ast_tree* ast, token** tokens, int* pos);
static int parse_block(ast_tree* ast, token** tokens, int* pos);

static int parse_primary_expr(ast_tree* ast, token** tokens, int* pos) {
	printf("primary_expr: %d\n", *pos);
	token* token = tokens[*pos];

	switch (token->type) {
		case TOKEN_NUMBER:
			return init_node(ast, (*pos)++, NODE_NUMBER);
		case TOKEN_IDENTIFIER:
			return init_node(ast, (*pos)++, NODE_IDENTIFIER);
		case TOKEN_PARENTHESIS_OPEN:
			return parse_parenthesis(ast, tokens, pos);
		default:
			fprintf(stderr, "ERROR: token %c is not a primary expression\n", token->start[0]);
			return -1;
	
	}

	return -1;
}

static int parse_parenthesis(ast_tree* ast, token** tokens, int* pos) {
	token* token = tokens[*pos];
	if (token->type == TOKEN_PARENTHESIS_OPEN) {
		(*pos)++;
		int index = parse_expression(ast, tokens, pos);

		if (tokens[*pos]->type != TOKEN_PARENTHESIS_CLOSE) {
			fprintf(stderr, "ERROR: No closing parenthesis on expression\n");
			return -1;
		}

		(*pos)++;

		return index;
	}
	else {
		return parse_primary_expr(ast, tokens, pos);
	}
}

static int parse_mul_expr(ast_tree* ast, token** tokens, int* pos) {
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

static int parse_add_expr(ast_tree* ast, token** tokens, int* pos) {
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

static int parse_expression(ast_tree* ast, token** tokens, int* pos) {
	return parse_add_expr(ast, tokens, pos);
}

static int parse_block(ast_tree* ast, token** tokens, int* pos) {
	return 0;
}

void parse(ast_tree* ast, token** tokens) {
	int i = 0;

	int index = parse_expression(ast, tokens, &i);
	if (index < 0) {
		fprintf(stderr, "Error detected, exiting\n");
		exit(EXIT_FAILURE);
	}
	printf("parse success\n");

	ast->nodes[ast->root_index].data.node_array.count = 1;
	ast->nodes[ast->root_index].data.node_array.ast_indices[0] = index;
}
