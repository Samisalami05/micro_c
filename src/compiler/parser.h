#ifndef PARSER_H
#define PARSER_H

#include <compiler/ast.h>
#include <compiler/lexer.h>

void parse(ast_tree* ast, token** tokens);

#endif
