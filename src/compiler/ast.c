#include <compiler/ast.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALLOC_COUNT 10

// Ast
static char node_has_two(ast_node node);
static void validate_size(ast_tree* ast, int num);

static char node_has_two(ast_node node) {
	return node.type == NODE_BINARY_OP;
}

static void init_root(ast_node* root) {
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

int init_node(ast_tree* ast, int pos, ast_nodetype type) {
	validate_size(ast, 1);
	ast->nodes[ast->count].type = type;
	ast->nodes[ast->count].token_index = pos;
	ast->nodes[ast->count].data.node_array.count = 0;
	ast->count++;
	
	return ast->count-1;
}

ast_tree* ast_create() {
	ast_tree* ast = malloc(sizeof(ast_tree));
	ast->count = 1;
	ast->allocated = 1;
	ast->root_index = 0;
	ast->nodes = malloc(sizeof(ast_node));
	init_root(&ast->nodes[0]);

	return ast;
}

static void ast_node_print_rec(
    ast_tree* ast,
    int index,
    token** tokens,
    const char* prefix,
	char is_last
) {
    ast_node node = ast->nodes[index];

    // Print prefix and connector
    if (node.type == NODE_ROOT) {
        printf("ROOT\n");
    } else {
        printf("%s", prefix);

        if (is_last)
            printf("└── ");
        else
            printf("├── ");

        // Print token text
        token* t = tokens[node.token_index];
        fwrite(t->start, 1, t->length, stdout);
        printf("\n");
    }

    // Determine children
    int children[64];
    int child_count = 0;

    if (node_has_two(node)) {
        children[0] = node.data.two_nodes.node_l;
        children[1] = node.data.two_nodes.node_r;
        child_count = 2;
    } else {
        for (int i = 0; i < node.data.node_array.count; i++) {
            children[child_count++] = node.data.node_array.ast_indices[i];
        }
    }

    // Build next prefix
    char next_prefix[256];
    strcpy(next_prefix, prefix);

    if (node.type != NODE_ROOT) {
        if (is_last)
            strcat(next_prefix, "    "); // no vertical line
        else
            strcat(next_prefix, "│   "); // vertical continuation
    }

    // Recurse into children
    for (int i = 0; i < child_count; i++) {
        ast_node_print_rec(
            ast,
            children[i],
            tokens,
            next_prefix,
            i == child_count - 1
        );
    }
}

void ast_print(ast_tree* ast, token** tokens) {
    ast_node_print_rec(ast, ast->root_index, tokens, "", 1);
}
