#include "function.h"
#include "type.h"
#include <stdlib.h>
#include <string.h>

void print_function(FILE *file, Function *function) {
	print_token(file, &function->name);
	fprintf(file, ": ");
	print_type(file, function->return_type);
	fprintf(file, " @ %d\n", function->index);
}

void add_parameter_type(Function *function, Type type) {
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
	return &flist->functions[flist->count++];
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
