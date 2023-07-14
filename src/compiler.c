#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "lexer.h"
#include "chunk.h"
#include "token.h"
#include "type.h"

static void compile_statement(Compiler *compiler);
static void compile_block(Compiler *compiler);
static void compile_let(Compiler *compiler);
static void compile_assignment(Compiler *compiler);
static Type compile_type(Compiler *compiler);
static void compile_if(Compiler *compiler);
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
static void compile_name(Compiler *compiler);
static void compile_negation(Compiler *compiler);

static Token match(Compiler *compiler, TokenType type);

static int resolve_variable(Compiler *compiler, Token *name);
static void enter_block(Compiler *compiler);
static void exit_block(Compiler *compiler);
static void declare_variable(Compiler *compiler, Token name, Type type);
static void mark_initializied(Compiler *compiler);

static void push_type(Compiler *compiler, Type type);
static Type pop_type(Compiler *compiler); 
static void match_type(Compiler *compiler, Type expected);
static void type_error(Type expected, Type found);

void init_compiler(Compiler *compiler, Lexer *lexer, Chunk *chunk) {
	compiler->lexer = lexer;
	compiler->chunk = chunk;

	memset(compiler->variables, 0, 256 * sizeof(Variable));
	compiler->variable_count = 0;
	compiler->depth = 0;

        compiler->type_stackSize = 0;
}

void compile(Compiler *compiler) {
	compile_block(compiler);
	match(compiler, TT_END);
	write_into_chunk(compiler->chunk, OP_EXIT);
}

static void compile_statement(Compiler *compiler) {
	Token token = peek_next_token(compiler->lexer);
	switch (token.type) {
		case TT_LCURLY:
			compile_block(compiler);
			break;
		case TT_LET:
			compile_let(compiler);
			break;
		case TT_PRINT:
			compile_print(compiler);
			break;
                case TT_IF:
                        compile_if(compiler);
                        break;
		default:
                        compile_assignment(compiler);
	}
}

static void compile_block(Compiler *compiler) {
	match(compiler, TT_LCURLY);
	enter_block(compiler);
	for (;;) {
		Token token = peek_next_token(compiler->lexer);
		if (token.type == TT_RCURLY || token.type == TT_END) {
			break;
		}
		compile_statement(compiler);
	}
	exit_block(compiler);
	match(compiler, TT_RCURLY);
}

static void compile_let(Compiler *compiler) {
	match(compiler, TT_LET);
	Token name = match(compiler, TT_NAME);

	for (int i = compiler->variable_count - 1; i >= 0; --i) {
		Variable *variable = &compiler->variables[i];
		if (compiler->depth != -1 && variable->depth < compiler->depth) {
			break;
		}

		if (token_equal(&name, &variable->name)) {
			fprintf(stderr, "Variable already declared\n");
                        exit(EXIT_FAILURE);
		}
	}

        match(compiler, TT_COLON);
        Type type = compile_type(compiler);

	declare_variable(compiler, name, type);

	match(compiler, TT_EQUAL);
	compile_expression(compiler);
	match(compiler, TT_SEMICOLON);

        Type expressionType = pop_type(compiler);
        if (expressionType != type) {
            type_error(type, expressionType);
        }

	mark_initializied(compiler);
}

static void compile_assignment(Compiler *compiler) {
        Token name = match(compiler, TT_NAME);
        match(compiler, TT_EQUAL);
        compile_expression(compiler);
	match(compiler, TT_SEMICOLON);

        int i = resolve_variable(compiler, &name);
        match_type(compiler, pop_type(compiler));
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

static void compile_if(Compiler *compiler) {
    match(compiler, TT_IF);
    compile_expression(compiler);
    match_type(compiler, TY_BOOLEAN);
    write_into_chunk(compiler->chunk, OP_JUMP_IF_FALSE);
    int dest_index = compiler->chunk->length;
    write_into_chunk(compiler->chunk, -1);
    compile_block(compiler);
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
		type_error(TY_INTEGER, first_type);
        }
        Type second_type = pop_type(compiler);
        if (second_type != TY_INTEGER) {
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
                        compile_name(compiler);
			break;
		}
		default: {
			fprintf(stderr, "Syntax Error: Unexpected token ");
			print_token(stderr, &token);
			fprintf(stderr, " while parsing expression\n");
                        exit(EXIT_FAILURE);
		}
	}
}

static void compile_name(Compiler *compiler) {
        Token token = get_next_token(compiler->lexer);
        write_into_chunk(compiler->chunk, OP_LOAD);

        int i = resolve_variable(compiler, &token);
        write_into_chunk(compiler->chunk, i);
}
        

static void compile_negation(Compiler *compiler) {
	get_next_token(compiler->lexer);
	compile_unary(compiler);

        match_type(compiler, TY_INTEGER);
        push_type(compiler, TY_INTEGER);

	write_into_chunk(compiler->chunk, OP_NEGATE);
}

static Token match(Compiler *compiler, TokenType type) {
	Token token = peek_next_token(compiler->lexer);
	if (token.type != type) {
		fprintf(stderr, "Syntax Error: Expected ");
		print_token_type(stderr, &type);
		fprintf(stderr, " but found ");
		print_token(stderr, &token);
		fprintf(stderr, "\n");
                exit(EXIT_FAILURE);
	}
	return get_next_token(compiler->lexer);
}

static int resolve_variable(Compiler *compiler, Token *name) {
	for (int i = compiler->variable_count - 1; i >= 0; --i) {
		Variable *variable = &compiler->variables[i];
		if (token_equal(&variable->name, name)) {
			if (variable->depth != -1) {
                            push_type(compiler, variable->type);
                            return i;
			}
		}
	}

	fprintf(stderr, "Undeclared variable!\n");
        exit(EXIT_FAILURE);
}

static void enter_block(Compiler *compiler) {
	compiler->depth++;
}

static void exit_block(Compiler *compiler) {
	compiler->depth--;

	while (compiler->variable_count > 0 &&
			compiler->variables[compiler->variable_count - 1].depth > compiler->depth) {
		write_into_chunk(compiler->chunk, OP_POP);
		compiler->variable_count--;
	}
}

static void declare_variable(Compiler *compiler, Token name, Type type) {
	Variable* variable = &compiler->variables[compiler->variable_count++];
	variable->name = name;
        variable->type = type;
	variable->depth = -1;
}

static void mark_initializied(Compiler *compiler) {
	compiler->variables[compiler->variable_count - 1].depth = compiler->depth;
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
