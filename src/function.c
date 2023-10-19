#include "function.h"
#include "type.h"
#include <stdlib.h>
#include <string.h>

void print_function(FILE *file, Function *function) {
	print_token(file, &function->name);
	fprintf(file, ": ");
	for (int i = 0; i < function->parameter_count; i++) {
		print_type(file, function->parameter_types[i]);
		fprintf(file, ", ");
	}
	fprintf(file, " returns ");
	print_type(file, function->return_type);
	fprintf(file, " @ %d\n", function->index);
}

void add_parameter_type(Function *function, Type type) {
	if (function->parameter_count == function->parameter_capacity) {
		function->parameter_capacity *= 2;
		int new_size = function->parameter_capacity * sizeof(Type);
		function->parameter_types =
		    realloc(function->parameter_types, new_size);
	}
	function->parameter_types[function->parameter_count++] = type;
}

void init_function_list(FunctionList *flist) {
	flist->functions = malloc(4 * sizeof(Function));
	flist->count = 0;
	flist->capacity = 4;
}

void free_function_list(FunctionList *flist) {
	free(flist->functions);
}

Function *add_function(FunctionList *flist) {
	if (flist->count == flist->capacity) {
		flist->capacity *= 2;
		int new_size = flist->capacity * sizeof(Function);
		flist->functions = realloc(flist->functions, new_size);
	}
	Function *function = &flist->functions[flist->count++];
	function->parameter_types = malloc(4 * sizeof(Type));
	function->parameter_count = 0;
	function->parameter_capacity = 4;
	return function;
}

Function *find_function(FunctionList *flist, Token *name) {
	for (int i = 0; i < flist->count; i++) {
		Function *f = &flist->functions[i];
		if (token_equal(&f->name, name)) {
			return f;
		}
	}
	return NULL;
}

Function *find_main_function(FunctionList *flist) {
	char *main_string = "main";
	for (int i = 0; i < flist->count; i++) {
		Function *f = &flist->functions[i];
		Token name = f->name;
		if (name.length == strlen(main_string) &&
		    strncmp(name.start, main_string, name.length) == 0) {
			return f;
		}
	}
	return NULL;
}

void print_function_list(FILE *file, FunctionList *flist) {
	for (int i = 0; i < flist->count; i++) {
		print_function(file, &flist->functions[i]);
	}
}
