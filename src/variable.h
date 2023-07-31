#ifndef VARIABLE_H
#define VARIABLE_H

#include "type.h"
#include "token.h"

typedef struct Variable {
	Token name;
        Type type;
	int depth;
} Variable;

typedef struct VariableStack {
	Variable *variables;
	int variable_count;
	int depth;
} VariableStack;

void init_variable_stack(VariableStack *vs);
void free_variable_stack(VariableStack *vs);

void enter_block(VariableStack *vs);
int exit_block(VariableStack *vs);
void declare_variable(VariableStack *vs, Token name, Type type);
int resolve_variable(VariableStack *vs, Token *name);
void mark_initializied(VariableStack *vs);

#endif
