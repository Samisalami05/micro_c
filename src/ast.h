#ifndef AST_H
#define AST_H

typedef enum value_type {
	VAL_INT,
	VAL_FLOAT,
	VAL_CHAR,
	VAL_STRING,
} value_type;

typedef union value_data {
	int	  int_value;
	float float_value;
	char  char_value;
	char* string_value;
} value_data;

typedef enum ast_nodetype {
	NODE_INDETIFIER,
	NODE_FUNCTION,
} ast_nodetype;

typedef struct ast_node {
	ast_nodetype type;
	char* name;

	value_type value_type;
	value_data value;

	// functions
	int arg_count;
	ast_node** args;

	ast_node* left;
	ast_node* right;
} ast_node;

#endif
