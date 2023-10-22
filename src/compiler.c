#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "compiler.h"
#include "lexer.h"
#include "token.h"
#include "type.h"
#include "variable.h"

static void compile_statement(Compiler *compiler, Type type);
static void compile_return(Compiler *compiler, Type type);
static void compile_function(Compiler *compiler);
static void compile_block(Compiler *compiler, Type type);
static void compile_let(Compiler *compiler);
static void compile_assignment(Compiler *compiler);
static Type compile_type(Compiler *compiler);
static void compile_if(Compiler *compiler, Type type);
static void compile_while(Compiler *compiler, Type type);
static void compile_print(Compiler *compiler);

static void compile_expression(Compiler *compiler);
static void compile_comparison(Compiler *compiler);
static void compile_addition_and_subtraction(Compiler *compiler);
static void compile_addition(Compiler *compiler);
static void compile_subtraction(Compiler *compiler);
static void compile_multiplication_and_division(Compiler *compiler);
static void compile_multiplication(Compiler *compiler);
static void compile_division(Compiler *compiler);
static void compile_unary(Compiler *compiler);
static void compile_name_or_call(Compiler *comppiler);
static void compile_name(Compiler *compiler, Token token);
static void compile_call(Compiler *compiler, Token token);
static void compile_negation(Compiler *compiler);

static void error(Compiler *compiler);
static Token match(Compiler *compiler, TokenType type);

static void push_type(Compiler *compiler, Type type);
static Type pop_type(Compiler *compiler);
static void match_type(Compiler *compiler, Type expected);
static void type_error(Type expected, Type found);

void init_compiler(Compiler *compiler, Lexer *lexer, Chunk *chunk) {
	compiler->lexer = lexer;
	compiler->chunk = chunk;
	init_variable_stack(&compiler->variable_stack);
	init_function_list(&compiler->flist);
	compiler->type_stackSize = 0;
}

void free_compiler(Compiler *compiler) {
	free_variable_stack(&compiler->variable_stack);
}

void compile(Compiler *compiler) {
	write_into_chunk(compiler->chunk, OP_CALL);
	write_into_chunk(compiler->chunk, -1);
	write_into_chunk(compiler->chunk, 0);
	write_into_chunk(compiler->chunk, OP_EXIT);

	for (;;) {
		Token token = peek_next_token(compiler->lexer);
		if (token.type == TT_END) {
			break;
		}
		compile_function(compiler);
	}
	match(compiler, TT_END);

	Function *main = find_main_function(&compiler->flist);
	if (main == NULL) {
		fprintf(stderr, "No main function\n");
		exit(EXIT_FAILURE);
	}
	compiler->chunk->code[1] = main->index;

	// print_function_list(stdout, &compiler->flist);
}

static void compile_function(Compiler *compiler) {
	match(compiler, TT_FUNC);
	Function *f = add_function(&compiler->flist);

	Token name = match(compiler, TT_NAME);
	f->name = name;

	match(compiler, TT_LPAREN);
	Token token = peek_next_token(compiler->lexer);
	if (token.type != TT_RPAREN) {
		Token name = match(compiler, TT_NAME);
		match(compiler, TT_COLON);
		Type type = compile_type(compiler);

		declare_variable(&compiler->variable_stack, name, type);
		mark_initializied(&compiler->variable_stack);
		add_parameter_type(f, type);

		for (;;) {
			Token token = peek_next_token(compiler->lexer);
			if (token.type == TT_RPAREN) {
				break;
			}

			match(compiler, TT_COMMA);
			Token name = match(compiler, TT_NAME);
			match(compiler, TT_COLON);
			Type type = compile_type(compiler);

			declare_variable(&compiler->variable_stack, name, type);
			mark_initializied(&compiler->variable_stack);
			add_parameter_type(f, type);
		}
	}
	match(compiler, TT_RPAREN);

	match(compiler, TT_COLON);
	Type type = compile_type(compiler);
	f->return_type = type;

	f->index = compiler->chunk->length;
	compile_block(compiler, type);

	compiler->variable_stack.variable_count = 0;
}

static void compile_statement(Compiler *compiler, Type type) {
	Token token = peek_next_token(compiler->lexer);
	switch (token.type) {
		case TT_LCURLY:
			compile_block(compiler, type);
			break;
		case TT_RETURN:
			compile_return(compiler, type);
			break;
		case TT_LET:
			compile_let(compiler);
			break;
		case TT_PRINT:
			compile_print(compiler);
			break;
		case TT_IF:
			compile_if(compiler, type);
			break;
		case TT_WHILE:
			compile_while(compiler, type);
			break;
		default:
			compile_assignment(compiler);
	}
}

static void compile_block(Compiler *compiler, Type type) {
	match(compiler, TT_LCURLY);
	enter_block(&compiler->variable_stack);
	for (;;) {
		Token token = peek_next_token(compiler->lexer);
		if (token.type == TT_RCURLY || token.type == TT_END) {
			break;
		}
		compile_statement(compiler, type);
	}
	int pops = exit_block(&compiler->variable_stack);
	for (int i = 0; i < pops; i++) {
		write_into_chunk(compiler->chunk, OP_POP);
	}
	match(compiler, TT_RCURLY);
}

static void compile_return(Compiler *compiler, Type type) {
	match(compiler, TT_RETURN);
	compile_expression(compiler);
	match(compiler, TT_SEMICOLON);

	Type ret_type = pop_type(compiler);
	if (ret_type != type) {
		error(compiler);
		type_error(type, ret_type);
	}

	write_into_chunk(compiler->chunk, OP_RETURN);
	write_into_chunk(compiler->chunk,
			 compiler->variable_stack.variable_count);
}

static void compile_let(Compiler *compiler) {
	match(compiler, TT_LET);
	Token name = match(compiler, TT_NAME);

	match(compiler, TT_COLON);
	Type type = compile_type(compiler);

	declare_variable(&compiler->variable_stack, name, type);

	match(compiler, TT_EQUAL);
	compile_expression(compiler);
	match(compiler, TT_SEMICOLON);

	Type expression_type = pop_type(compiler);
	if (expression_type != type) {
		error(compiler);
		type_error(type, expression_type);
	}

	mark_initializied(&compiler->variable_stack);
}

static void compile_assignment(Compiler *compiler) {
	Token name = match(compiler, TT_NAME);
	match(compiler, TT_EQUAL);
	compile_expression(compiler);
	match(compiler, TT_SEMICOLON);

	int i = resolve_variable(&compiler->variable_stack, &name);
	Variable *variable = &compiler->variable_stack.variables[i];
	match_type(compiler, variable->type);
	write_into_chunk(compiler->chunk, OP_STORE);
	write_into_chunk(compiler->chunk, i);
}

static Type compile_type(Compiler *compiler) {
	Token token = get_next_token(compiler->lexer);
	switch (token.type) {
		case TT_INTEGER:
			return TY_INTEGER;
		case TT_BOOLEAN:
			return TY_BOOLEAN;
		default:
			error(compiler);
			fprintf(stderr, "Unknown type\n");
			exit(EXIT_FAILURE);
	}
}

static void compile_print(Compiler *compiler) {
	match(compiler, TT_PRINT);
	match(compiler, TT_LPAREN);
	compile_expression(compiler);
	match(compiler, TT_RPAREN);
	match(compiler, TT_SEMICOLON);

	Type type = pop_type(compiler);
	switch (type) {
		case TY_INTEGER:
			write_into_chunk(compiler->chunk, OP_PRINT_INTEGER);
			break;
		case TY_BOOLEAN:
			write_into_chunk(compiler->chunk, OP_PRINT_BOOLEAN);
			break;
	}
}

static void compile_if(Compiler *compiler, Type type) {
	match(compiler, TT_IF);
	compile_expression(compiler);
	match_type(compiler, TY_BOOLEAN);
	write_into_chunk(compiler->chunk, OP_JUMP_IF_FALSE);
	int dest_index = compiler->chunk->length;
	write_into_chunk(compiler->chunk, -1);
	compile_block(compiler, type);
	compiler->chunk->code[dest_index] = compiler->chunk->length;
}

static void compile_while(Compiler *compiler, Type type) {
	match(compiler, TT_WHILE);
	int entry_index = compiler->chunk->length;
	compile_expression(compiler);
	match_type(compiler, TY_BOOLEAN);
	write_into_chunk(compiler->chunk, OP_JUMP_IF_FALSE);
	int dest_index = compiler->chunk->length;
	write_into_chunk(compiler->chunk, -1);
	compile_block(compiler, type);
	write_into_chunk(compiler->chunk, OP_JUMP);
	write_into_chunk(compiler->chunk, entry_index);
	compiler->chunk->code[dest_index] = compiler->chunk->length;
}

static void compile_expression(Compiler *compiler) {
	compile_comparison(compiler);
}

static void compile_comparison(Compiler *compiler) {
	compile_addition_and_subtraction(compiler);
	OpCode comparison;
	Token token = peek_next_token(compiler->lexer);
	switch (token.type) {
		case TT_DOUBLE_EQUAL:
			comparison = OP_EQUAL;
			break;
		case TT_NOT_EQUAL:
			comparison = OP_NOT_EQUAL;
			break;
		case TT_LESS:
			comparison = OP_LESS;
			break;
		case TT_LESS_EQUAL:
			comparison = OP_LESS_EQUAL;
			break;
		case TT_GREATER:
			comparison = OP_GREATER;
			break;
		case TT_GREATER_EQUAL:
			comparison = OP_GREATER_EQUAL;
			break;
		default:
			return;
	}
	get_next_token(compiler->lexer);
	compile_addition_and_subtraction(compiler);

	Type first_type = pop_type(compiler);
	if (first_type != TY_INTEGER) {
		error(compiler);
		type_error(TY_INTEGER, first_type);
	}
	Type second_type = pop_type(compiler);
	if (second_type != TY_INTEGER) {
		error(compiler);
		type_error(TY_INTEGER, first_type);
	}
	push_type(compiler, TY_BOOLEAN);

	write_into_chunk(compiler->chunk, comparison);
}

static void compile_addition_and_subtraction(Compiler *compiler) {
	compile_multiplication_and_division(compiler);
	for (;;) {
		Token token = peek_next_token(compiler->lexer);
		switch (token.type) {
			case TT_PLUS:
				compile_addition(compiler);
				break;
			case TT_MINUS:
				compile_subtraction(compiler);
				break;
			default:
				return;
		}
	}
}

static void compile_addition(Compiler *compiler) {
	get_next_token(compiler->lexer);
	compile_multiplication_and_division(compiler);

	match_type(compiler, TY_INTEGER);
	match_type(compiler, TY_INTEGER);
	push_type(compiler, TY_INTEGER);

	write_into_chunk(compiler->chunk, OP_ADD);
}

static void compile_subtraction(Compiler *compiler) {
	get_next_token(compiler->lexer);
	compile_multiplication_and_division(compiler);

	match_type(compiler, TY_INTEGER);
	match_type(compiler, TY_INTEGER);
	push_type(compiler, TY_INTEGER);

	write_into_chunk(compiler->chunk, OP_SUB);
}

static void compile_multiplication_and_division(Compiler *compiler) {
	compile_unary(compiler);
	for (;;) {
		Token token = peek_next_token(compiler->lexer);
		switch (token.type) {
			case TT_STAR:
				compile_multiplication(compiler);
				break;
			case TT_SLASH:
				compile_division(compiler);
				break;
			default:
				return;
		}
	}
}

static void compile_multiplication(Compiler *compiler) {
	get_next_token(compiler->lexer);
	compile_unary(compiler);

	match_type(compiler, TY_INTEGER);
	match_type(compiler, TY_INTEGER);
	push_type(compiler, TY_INTEGER);

	write_into_chunk(compiler->chunk, OP_MUL);
}

static void compile_division(Compiler *compiler) {
	get_next_token(compiler->lexer);
	compile_unary(compiler);

	match_type(compiler, TY_INTEGER);
	match_type(compiler, TY_INTEGER);
	push_type(compiler, TY_INTEGER);

	write_into_chunk(compiler->chunk, OP_DIV);
}

static void compile_unary(Compiler *compiler) {
	Chunk *chunk = compiler->chunk;
	Token token = peek_next_token(compiler->lexer);
	switch (token.type) {
		case TT_NUMBER: {
			get_next_token(compiler->lexer);
			write_into_chunk(chunk, OP_PUSH_INTEGER);
			int n = atoi(token.start);
			write_into_chunk(chunk, n);
			push_type(compiler, TY_INTEGER);
			break;
		}
		case TT_TRUE: {
			get_next_token(compiler->lexer);
			write_into_chunk(chunk, OP_PUSH_TRUE);
			push_type(compiler, TY_BOOLEAN);
			break;
		}
		case TT_FALSE: {
			get_next_token(compiler->lexer);
			write_into_chunk(chunk, OP_PUSH_FALSE);
			push_type(compiler, TY_BOOLEAN);
			break;
		}
		case TT_LPAREN: {
			get_next_token(compiler->lexer);
			compile_expression(compiler);
			match(compiler, TT_RPAREN);
			break;
		}
		case TT_MINUS: {
			compile_negation(compiler);
			break;
		}
		case TT_NAME: {
			compile_name_or_call(compiler);
			break;
		}
		default: {
			error(compiler);
			fprintf(stderr, "Syntax Error: Unexpected token ");
			print_token(stderr, &token);
			fprintf(stderr, " while parsing expression\n");
			exit(EXIT_FAILURE);
		}
	}
}

static void compile_name_or_call(Compiler *compiler) {
	Token token = get_next_token(compiler->lexer);
	Token next = peek_next_token(compiler->lexer);
	if (next.type == TT_LPAREN) {
		compile_call(compiler, token);
	} else {
		compile_name(compiler, token);
	}
}

static void compile_name(Compiler *compiler, Token token) {
	write_into_chunk(compiler->chunk, OP_LOAD);

	int i = resolve_variable(&compiler->variable_stack, &token);
	Variable *v = &compiler->variable_stack.variables[i];
	push_type(compiler, v->type);
	write_into_chunk(compiler->chunk, i);
}

static void compile_call(Compiler *compiler, Token token) {
	Function *f = find_function(&compiler->flist, &token);
	if (f == NULL) {
		error(compiler);
		fprintf(stderr, "Name Error: Unknown Function\n");
		exit(EXIT_FAILURE);
	}

	match(compiler, TT_LPAREN);
	int num_args = 0;
	if (token.type != TT_RPAREN) {
		compile_expression(compiler);
		match_type(compiler, f->parameter_types[num_args++]);

		for (;;) {
			Token token = peek_next_token(compiler->lexer);
			if (token.type == TT_RPAREN) {
				break;
			}
			if (num_args >= f->parameter_count) {
				error(compiler);
				fprintf(stderr,
					"Function Error: Expected %d arguments "
					"but received %d\n",
					f->parameter_count, num_args + 1);
				exit(EXIT_FAILURE);
			}
			match(compiler, TT_COMMA);
			compile_expression(compiler);
			match_type(compiler, f->parameter_types[num_args++]);
		}
	}
	match(compiler, TT_RPAREN);

	if (num_args != f->parameter_count) {
		error(compiler);
		fprintf(
		    stderr,
		    "Function Error: Expected %d arguments but received %d\n",
		    f->parameter_count, num_args);
		exit(EXIT_FAILURE);
	}

	push_type(compiler, f->return_type);

	write_into_chunk(compiler->chunk, OP_CALL);
	write_into_chunk(compiler->chunk, f->index);
	write_into_chunk(compiler->chunk, f->parameter_count);
}

static void compile_negation(Compiler *compiler) {
	get_next_token(compiler->lexer);
	compile_unary(compiler);

	match_type(compiler, TY_INTEGER);
	push_type(compiler, TY_INTEGER);

	write_into_chunk(compiler->chunk, OP_NEGATE);
}

static void error(Compiler *compiler) {
	fprintf(stderr, "Line %d: ", compiler->lexer->line_number);
}

static Token match(Compiler *compiler, TokenType type) {
	Token token = peek_next_token(compiler->lexer);
	if (token.type != type) {
		error(compiler);
		fprintf(stderr, "Syntax Error: Expected ");
		print_token_type(stderr, &type);
		fprintf(stderr, " but found ");
		print_token(stderr, &token);
		fprintf(stderr, "\n");
		exit(EXIT_FAILURE);
	}
	return get_next_token(compiler->lexer);
}

static void push_type(Compiler *compiler, Type type) {
	compiler->type_stack[compiler->type_stackSize++] = type;
}

static Type pop_type(Compiler *compiler) {
	return compiler->type_stack[--compiler->type_stackSize];
}

static void match_type(Compiler *compiler, Type expected) {
	Type found = pop_type(compiler);
	if (found != expected) {
		error(compiler);
		type_error(expected, found);
	}
}

static void type_error(Type expected, Type found) {
	fprintf(stderr, "TypeError: Expected ");
	print_type(stderr, expected);
	fprintf(stderr, " but found ");
	print_type(stderr, found);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}
