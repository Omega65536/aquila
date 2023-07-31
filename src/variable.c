#include "variable.h"
#include <stdlib.h>

void init_variable_stack(VariableStack *vs) {
	vs->variables = malloc(256 * sizeof(Variable));
	vs->variable_count = 0;
	vs->depth = 0;
}

void free_variable_stack(VariableStack *vs) {
	free(vs->variables);
}

int resolve_variable(VariableStack *vs, Token *name) {
	for (int i = vs->variable_count - 1; i >= 0; --i) {
		Variable *variable = &vs->variables[i];
		if (token_equal(&variable->name, name)) {
			if (variable->depth != -1) {
				return i;
			}
		}
	}

	fprintf(stderr, "Undeclared variable!\n");
	exit(EXIT_FAILURE);
}

void enter_block(VariableStack *vs) {
	vs->depth++;
}

int exit_block(VariableStack *vs) {
	vs->depth--;

	int pops = 0;
	while (vs->variable_count > 0 &&
	       vs->variables[vs->variable_count - 1].depth > vs->depth) {
		pops++;
		vs->variable_count--;
	}
	return pops;
}

void declare_variable(VariableStack *vs, Token name, Type type) {
	for (int i = vs->variable_count - 1; i >= 0; --i) {
		Variable *variable = &vs->variables[i];
		if (vs->depth != -1 && variable->depth < vs->depth) {
			break;
		}

		if (token_equal(&name, &variable->name)) {
			fprintf(stderr, "Variable already declared\n");
			exit(EXIT_FAILURE);
		}
	}

	Variable *variable = &vs->variables[vs->variable_count++];
	variable->name = name;
	variable->type = type;
	variable->depth = -1;
}

void mark_initializied(VariableStack *vs) {
	vs->variables[vs->variable_count - 1].depth = vs->depth;
}
