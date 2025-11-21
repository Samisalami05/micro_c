#include <compiler/ast.h>
#include <compiler/lexer.h>
#include <compiler/parser.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>

static int parse_primary_expr(ast_tree* ast, token** tokens, int* pos);
static int parse_parenthesis(ast_tree* ast, token** tokens, int* pos);
static int parse_expression(ast_tree* ast, token** tokens, int* pos);
static int parse_add_expr(ast_tree* ast, token** tokens, int* pos);
static int parse_mul_expr(ast_tree* ast, token** tokens, int* pos);
static int parse_block(ast_tree* ast, token** tokens, int* pos);

static int parse_primary_expr(ast_tree* ast, token** tokens, int* pos) {
	token* token = tokens[*pos];
	ast_nodetype type;

	switch (token->type) {
		case TOKEN_NUMBER:
			type = NODE_NUMBER;
			break;
		case TOKEN_IDENTIFIER:
			type = NODE_IDENTIFIER;
			break;
		case TOKEN_PARENTHESIS_OPEN: return parse_parenthesis(ast, tokens, pos);
		default:
			LOG_ERR("missing primary expression at token index %d\n", *pos);
			return -1;
	}

	ast_node node = {
		.type = type,
		.token_index = (*pos)++,
		.data.node_array.count = 0,
	};

	return init_node(ast, node);
}

static int parse_parenthesis(ast_tree* ast, token** tokens, int* pos) {
	// expected that token->type == TOKEN_PARENTHESIS_OPEN
	(*pos)++;
	int index = parse_expression(ast, tokens, pos);

	if (tokens[*pos]->type != TOKEN_PARENTHESIS_CLOSE) {
		LOG_ERR("ERROR: No closing parenthesis on expression\n");
		return -1;
	}

	(*pos)++;

	return index;
}

static int parse_mul_expr(ast_tree* ast, token** tokens, int* pos) {
	int l = parse_primary_expr(ast, tokens, pos); // call func with lower order of op
	if (l < 0) return -1;

	while (tokens[*pos]->type == TOKEN_STAR || tokens[*pos]->type == TOKEN_SLASH) {
		ast_node node = {
			.type = NODE_BINARY_OP,
			.token_index = (*pos)++,
		};

		int op = init_node(ast, node);
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
		ast_node node = {
			.type = NODE_BINARY_OP,
			.token_index = (*pos)++,
		};

		int op = init_node(ast, node);
		int r = parse_mul_expr(ast, tokens, pos);
		if (r < 0) return r;

		ast->nodes[op].data.two_nodes.node_l = l;
		ast->nodes[op].data.two_nodes.node_r = r;

		l = op;
	}
	return l;
}

static int parse_assignment_expr(ast_tree* ast, token** tokens, int* pos) {
	int l = parse_add_expr(ast, tokens, pos); // call func with lower order of op
	if (l < 0) return l;

	while (tokens[*pos]->type == TOKEN_EQUALS) {
		ast_node node = {
			.type = NODE_ASSIGNMENT_OP,
			.token_index = (*pos)++,
		};

		int op = init_node(ast, node);
		int r = parse_add_expr(ast, tokens, pos);
		if (r < 0) return r;

		ast->nodes[op].data.two_nodes.node_l = l;
		ast->nodes[op].data.two_nodes.node_r = r;

		l = op;
	}
	return l;
}

static int parse_expression(ast_tree* ast, token** tokens, int* pos) {
	return parse_assignment_expr(ast, tokens, pos);
}

static int parse_statement(ast_tree* ast, token** tokens, int* pos) {
	ast_node node = {
		.token_index = *pos,
		.type = NODE_STATEMENT,
		.data.node_array.count = 2,
		.data.node_array.ast_indices = malloc(sizeof(int) * 2),
	};

	node.data.node_array.ast_indices[0] = parse_primary_expr(ast, tokens, pos);
	if (node.data.node_array.ast_indices[0] < 0) return -1;
	
	node.data.node_array.ast_indices[1] = parse_expression(ast, tokens, pos);
	if (node.data.node_array.ast_indices[1] < 0) return -1;
	int statement = init_node(ast, node);
	return statement;
}

static int parse_block(ast_tree* ast, token** tokens, int* pos) {
	if (tokens[*pos]->type == TOKEN_BRACE_OPEN) {
		ast_node block_node = {
			.type = NODE_BLOCK,
			.token_index = (*pos)++,
			.data.node_array.count = 1, // Why is this the max number of elements ????? (maybe something to do with it being a union)
			.data.node_array.ast_indices = NULL,
		};
		
		int i = 0;
		while (tokens[*pos]->type != TOKEN_EOF && tokens[*pos]->type != TOKEN_BRACE_CLOSE) {
			block_node.data.node_array.ast_indices = realloc(block_node.data.node_array.ast_indices, sizeof(int) * (i + 1));
			block_node.data.node_array.ast_indices[i] = parse_block(ast, tokens, pos);
			if (block_node.data.node_array.ast_indices[i] < 0) return -1;
			i++;
		}

		block_node.data.node_array.count = i;

		if (tokens[*pos]->type != TOKEN_BRACE_CLOSE) {
			LOG_ERR("No closing bracket found for block\n");
			return -1;
		}

		*pos += 1; // Skip close brace
	
		int block = init_node(ast, block_node);
		return block;
	}
	else {
		return parse_statement(ast, tokens, pos);
	}
}


static int parse_parameter_list(ast_tree* ast, token** tokens, int* pos) {
	if (tokens[*pos]->type != TOKEN_PARENTHESIS_OPEN) {
		LOG_ERR("Expected parameter list after function name\n");
		return -1;
	}
	*pos += 1;
	while (tokens[*pos]->type != TOKEN_PARENTHESIS_CLOSE) {
		int datatype = parse_primary_expr(ast, tokens, pos);
		int name = parse_primary_expr(ast, tokens, pos);

		if (tokens[*pos]->type != TOKEN_COMMA) {
			break;
		}
		*pos += 1;
	}
	if (tokens[*pos]->type != TOKEN_PARENTHESIS_CLOSE) {
		LOG_ERR("Expected closing parenthesis after parameter list\n");
		return -1;
	}

	return 5;
}

static int parse_function(ast_tree* ast, token** tokens, int* pos) {
	int front = parse_primary_expr(ast, tokens, pos);
	if (ast->nodes[front].type != NODE_IDENTIFIER) {
		LOG_ERR("Expected identifier in front of function name\n");
		return -1;
	}
	int name = parse_primary_expr(ast, tokens, pos);
	if (ast->nodes[name].type != NODE_IDENTIFIER) {
		LOG_ERR("Expected function name after datatype\n");
		return -1;
	}
	int parameters = parse_parameter_list(ast, tokens, pos);
	return front;
}

void parse(ast_tree* ast, token** tokens) {
	int i = 0;
	int index = parse_function(ast, tokens, &i);
	
	if (index < 0) {
		LOG_ERR("Error detected, exiting\n");
		exit(EXIT_FAILURE);
	}
	LOG_MSG("parse success\n");

	ast->nodes[ast->root_index].data.node_array.count = 1;
	ast->nodes[ast->root_index].data.node_array.ast_indices = malloc(1*sizeof(int));
	ast->nodes[ast->root_index].data.node_array.ast_indices[0] = index;
}
