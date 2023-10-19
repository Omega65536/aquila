#ifndef FUNCTION_H
#define FUNCTION_H

#include "token.h"
#include "type.h"
#include <stdio.h>

typedef struct Function {
	Token name;
	Type return_type;
	Type *parameter_types;
	int parameter_count;
	int parameter_capacity;
	int index;
} Function;

void print_function(FILE *file, Function *function);
void add_parameter_type(Function *function, Type type);

typedef struct FunctionList {
	Function *functions;
	int count;
	int capacity;
} FunctionList;

void init_function_list(FunctionList *flist);
void free_function_list(FunctionList *flist);
Function *add_function(FunctionList *flist);
Function *find_function(FunctionList *flist, Token *name);
Function *find_main_function(FunctionList *flist);
void print_function_list(FILE *file, FunctionList *flist);

#endif
