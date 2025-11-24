#include <compiler/ast.h>
#include <compiler/lexer.h>
#include <compiler/parser.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>

typedef struct parser {
	ast_tree* ast;
	token** tokens;
	int t_pos;
} parser;

static token* current_token(parser* parser);
static ast_node* get_node(parser* parser, int index);
static void parser_error(char* filename, token token, char* msg);

static int parse_primary_expr(parser* parser);
static int parse_parenthesis(parser* parser);
static int parse_expression(parser* parser);
static int parse_add_expr(parser* parser);
static int parse_mul_expr(parser* parser);
static int parse_block(parser* parser);
static int parse_parameter(parser* parser);
static int parse_parameter_list(parser* parser);
static int parse_function(parser* parser);

static token* current_token(parser* parser) {
	return parser->tokens[parser->t_pos];
}

static ast_node* get_node(parser* parser, int index) {
	return &parser->ast->nodes[index];
}

static void parser_error(char* filename, token token, char* msg) {
	printf("%s:%d: error: %s\n", filename, token.length, msg);
	char* line = "malloc(sizeof(int) * 3);";
	printf("%s\n", line);
	int column = 10;

	for (int i = 0; i < column; i++) {
		printf(" ");
	}
	printf("~\n");
}

static int parse_primary_expr(parser* parser) {
	token* token = current_token(parser);
	ast_nodetype type;

	switch (token->type) {
		case TOKEN_NUMBER:
			type = NODE_NUMBER;
			break;
		case TOKEN_IDENTIFIER:
			type = NODE_IDENTIFIER;
			break;
		case TOKEN_PARENTHESIS_OPEN: return parse_parenthesis(parser);
		default:
			LOG_ERROR(ERROR_PARSER, "missing primary expression at token index %d\n", parser->t_pos);
			return -1;
	}

	ast_node node = {
		.type = type,
		.token_index = parser->t_pos++,
		.data.node_array.count = 0,
	};

	return init_node(parser->ast, node);
}

static int parse_parenthesis(parser* parser) {
	// expected that token->type == TOKEN_PARENTHESIS_OPEN
	parser->t_pos++;
	int index = parse_expression(parser);

	if (current_token(parser)->type != TOKEN_PARENTHESIS_CLOSE) {
		LOG_ERROR(ERROR_PARSER, "ERROR: No closing parenthesis on expression\n");
		return -1;
	}

	parser->t_pos++;

	return index;
}

static int parse_mul_expr(parser* parser) {
	int l = parse_primary_expr(parser); // call func with lower order of op
	if (l < 0) return -1;

	while (current_token(parser)->type == TOKEN_STAR || current_token(parser)->type == TOKEN_SLASH) {
		ast_node node = {
			.type = NODE_BINARY_OP,
			.token_index = parser->t_pos++,
		};

		int op = init_node(parser->ast, node);
		int r = parse_primary_expr(parser);
		if (r < 0) return -1;

		parser->ast->nodes[op].data.two_nodes.node_l = l;
		parser->ast->nodes[op].data.two_nodes.node_r = r;

		l = op;
	}
	return l;
}

static int parse_add_expr(parser* parser) {
	int l = parse_mul_expr(parser); // call func with lower order of op
	if (l < 0) return l;

	while (current_token(parser)->type == TOKEN_PLUS || current_token(parser)->type == TOKEN_MINUS) {
		ast_node node = {
			.type = NODE_BINARY_OP,
			.token_index = parser->t_pos++,
		};

		int op = init_node(parser->ast, node);
		int r = parse_mul_expr(parser);
		if (r < 0) return r;

		parser->ast->nodes[op].data.two_nodes.node_l = l;
		parser->ast->nodes[op].data.two_nodes.node_r = r;

		l = op;
	}
	return l;
}

static int parse_assignment_expr(parser* parser) {
	int l = parse_add_expr(parser); // call func with lower order of op
	if (l < 0) return l;

	while (current_token(parser)->type == TOKEN_EQUALS) {
		ast_node node = {
			.type = NODE_ASSIGNMENT_OP,
			.token_index = parser->t_pos++,
		};

		int op = init_node(parser->ast, node);
		int r = parse_add_expr(parser);
		if (r < 0) return r;

		parser->ast->nodes[op].data.two_nodes.node_l = l;
		parser->ast->nodes[op].data.two_nodes.node_r = r;

		l = op;
	}
	return l;
}

static int parse_expression(parser* parser) {
	return parse_assignment_expr(parser);
}

static int parse_statement(parser* parser) {
	ast_node node = {
		.token_index = parser->t_pos,
		.type = NODE_STATEMENT,
		.data.node_array.count = 2,
		.data.node_array.ast_indices = malloc(sizeof(int) * 2),
	};

	node.data.node_array.ast_indices[0] = parse_primary_expr(parser);
	if (node.data.node_array.ast_indices[0] < 0) return -1;
	
	node.data.node_array.ast_indices[1] = parse_expression(parser);
	if (node.data.node_array.ast_indices[1] < 0) return -1;
	int statement = init_node(parser->ast, node);
	return statement;
}

static int parse_block(parser* parser) {
	if (current_token(parser)->type == TOKEN_BRACE_OPEN) {
		ast_node block_node = {
			.type = NODE_BLOCK,
			.token_index = parser->t_pos++,
			.data.node_array.count = 1, // Why is this the max number of elements ????? (maybe something to do with it being a union)
			.data.node_array.ast_indices = NULL,
		};
		
		int i = 0;
		while (current_token(parser)->type != TOKEN_EOF && current_token(parser)->type != TOKEN_BRACE_CLOSE) {
			block_node.data.node_array.ast_indices = realloc(block_node.data.node_array.ast_indices, sizeof(int) * (i + 1));
			block_node.data.node_array.ast_indices[i] = parse_block(parser);
			if (block_node.data.node_array.ast_indices[i] < 0) return -1;
			i++;
		}

		block_node.data.node_array.count = i;

		if (current_token(parser)->type != TOKEN_BRACE_CLOSE) {
			LOG_ERROR(ERROR_PARSER, "No closing bracket found for block\n");
			return -1;
		}

		parser->t_pos++; // Skip close brace
	
		int block = init_node(parser->ast, block_node);
		return block;
	}
	else {
		return parse_statement(parser);
	}
}

static int parse_parameter(parser* parser) {
	int datatype = parse_primary_expr(parser);
	if (parser->ast->nodes[datatype].type != NODE_IDENTIFIER) {
		LOG_ERROR(ERROR_PARSER, "Expected identifier as datatype in variable declaration");
		return -1;
	}
	int name = parse_primary_expr(parser);
	if (parser->ast->nodes[name].type != NODE_IDENTIFIER) {
		LOG_ERROR(ERROR_PARSER, "Expected identifier as variable name\n");
		return -1;
	}

	ast_node node = {
		.type = NODE_PARAMETER,
		.token_index = parser->t_pos,
	};

	node.data.two_nodes.node_l = datatype;
	node.data.two_nodes.node_r = name;

	return init_node(parser->ast, node);
}

static int parse_parameter_list(parser* parser) {
	if (current_token(parser)->type != TOKEN_PARENTHESIS_OPEN) {
		LOG_ERROR(ERROR_PARSER, "Expected parameter list after function name\n");
		return -1;
	}
	parser->t_pos++;

	ast_node node = {
		.type = NODE_PARAMETERS,
		.token_index = parser->t_pos,
		.data.node_array.count = 0,
	};

	while (current_token(parser)->type != TOKEN_PARENTHESIS_CLOSE) {
		int var = parse_parameter(parser);
		if (var == -1) return -1;

		node_append(&node, var);

		if (current_token(parser)->type != TOKEN_COMMA) {
			break;
		}
		parser->t_pos++;
	}
	if (current_token(parser)->type != TOKEN_PARENTHESIS_CLOSE) {
		if (current_token(parser)->type != TOKEN_EOF && parser->tokens[parser->t_pos + 1]->type == TOKEN_IDENTIFIER)
			LOG_ERROR(ERROR_PARSER, "Expected comma between function parameters\n");
		else
			LOG_ERROR(ERROR_PARSER, "Expected closing parenthesis after parameter list\n");
		return -1;
	}
	parser->t_pos++;

	return init_node(parser->ast, node);
}

static int parse_function(parser* parser) {
	ast_node node = {
		.type = NODE_FUNC,
		.token_index = parser->t_pos,
		.data.node_array.count = 0,
	};

	while (current_token(parser)->type != TOKEN_PARENTHESIS_OPEN && current_token(parser)->type != TOKEN_EOF) {
		int front = parse_primary_expr(parser);
		if (front == -1) return -1;

		if (get_node(parser, front)->type != NODE_IDENTIFIER) {
			LOG_ERR("Expected identifier in function declaration\n");
			return -1;
		}

		node_append(&node, front);
	}
	int parameters = parse_parameter_list(parser);
	if (parameters == -1) return -1;

	node_append(&node, parameters);

	switch (current_token(parser)->type) {
		case TOKEN_BRACE_OPEN:
			int block = parse_block(parser);
			if (block == -1) return -1;

			node_append(&node, block);
			break;
		case TOKEN_SEMICOLON:
			node.type = NODE_FUNC_DECL;
			parser->t_pos++;
			break;
		default:
			LOG_ERR("Expected ';' or '{' after function declaration\n");
			return -1;
	}

	return init_node(parser->ast, node);
}

void parse(ast_tree* ast, token** tokens) {
	parser parser = {
		.ast = ast,
		.tokens = tokens,
		.t_pos = 0,
	};

	while (current_token(&parser)->type != TOKEN_EOF) {
		int index = -1;
		switch(current_token(&parser)->type) {
			case TOKEN_IDENTIFIER:
				index = parse_function(&parser);
				break;
			default:
				LOG_ERROR(ERROR_PARSER, "Unknown token in file statement\n");
		}

		if (index < 0) {
			LOG_ERROR(ERROR_PARSER, "Error detected, exiting\n");
			exit(EXIT_FAILURE);
		}

		node_append(&ast->nodes[ast->root_index], index);
	}

	parser_error("main.c", *parser.tokens[1], "Invalid allocation of memery");

	LOG_MSG("parse success\n");
}
