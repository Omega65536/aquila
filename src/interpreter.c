#include "interpreter.h"
#include "chunk.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//#define DEBUG

static uint32_t next(Interpreter *interpreter);
static void push(Interpreter *interpreter, Object value);
static Object pop(Interpreter *interpreter);
static Object make_integer(int value);
static Object make_boolean(bool value);

void init_interpreter(Interpreter *interpreter, Chunk *chunk) {
	interpreter->chunk = chunk;
	interpreter->index = 0;
	interpreter->offset = 0;
	interpreter->stack = malloc(256 * sizeof(Object));
	interpreter->stack_length = 0;
	interpreter->frames = malloc(256 * sizeof(Frame));
	interpreter->frame_count = 0;
}

void free_interpreter(Interpreter *interpreter) {
	free(interpreter->stack);
}

int interpret(Interpreter *interpreter) {
	for (;;) {
		OpCode op_code = next(interpreter);
		// printf("INDEX: %d, OFFSET: %d\n", interpreter->index-1,
		// interpreter->offset);
		switch (op_code) {
			case OP_NOOP:
				break;
			case OP_EXIT:
				return 0;
			case OP_PUSH_INTEGER: {
#ifdef DEBUG
				printf("PUSH_INTEGER\n");
#endif
				int value = next(interpreter);
				push(interpreter, make_integer(value));
				break;
			}
			case OP_PUSH_TRUE: {
				push(interpreter, make_boolean(true));
				break;
			}
			case OP_PUSH_FALSE: {
				push(interpreter, make_boolean(false));
				break;
			}
			case OP_POP: {
				pop(interpreter);
				break;
			}
			case OP_LOAD: {
#ifdef DEBUG
				printf("LOAD\n");
#endif
				int index = next(interpreter);
				push(interpreter,
				     interpreter
					 ->stack[interpreter->offset + index]);
				break;
			}
			case OP_STORE: {
#ifdef DEBUG
				printf("STORE\n");
#endif
				int index = next(interpreter);
				Object object = pop(interpreter);
				interpreter
				    ->stack[interpreter->offset + index] =
				    object;
				break;
			}
			case OP_PRINT_INTEGER: {
				int value = pop(interpreter).integer;
				printf("%d\n", value);
				break;
			}
			case OP_PRINT_BOOLEAN: {
				bool value = pop(interpreter).boolean;
				if (value) {
					printf("true\n");
				} else {
					printf("false\n");
				}
				break;
			}
			case OP_ADD: {
#ifdef DEBUG
				printf("ADD\n");
#endif
				int a = pop(interpreter).integer;
				int b = pop(interpreter).integer;
				int result = b + a;
				push(interpreter, make_integer(result));
				break;
			}
			case OP_SUB: {
				int a = pop(interpreter).integer;
				int b = pop(interpreter).integer;
				int result = b - a;
				push(interpreter, make_integer(result));
				break;
			}
			case OP_MUL: {
				int a = pop(interpreter).integer;
				int b = pop(interpreter).integer;
				int result = b * a;
				push(interpreter, make_integer(result));
				break;
			}
			case OP_DIV: {
				int a = pop(interpreter).integer;
				int b = pop(interpreter).integer;
				int result = b / a;
				push(interpreter, make_integer(result));
				break;
			}
			case OP_NEGATE: {
				int a = pop(interpreter).integer;
				int result = -a;
				push(interpreter, make_integer(result));
				break;
			}
			case OP_EQUAL: {
				int a = pop(interpreter).integer;
				int b = pop(interpreter).integer;
				bool result = b == a;
				push(interpreter, make_boolean(result));
				break;
			}
			case OP_NOT_EQUAL: {
				int a = pop(interpreter).integer;
				int b = pop(interpreter).integer;
				bool result = b != a;
				push(interpreter, make_boolean(result));
				break;
			}
			case OP_LESS: {
				int a = pop(interpreter).integer;
				int b = pop(interpreter).integer;
				bool result = b < a;
				push(interpreter, make_boolean(result));
				break;
			}
			case OP_LESS_EQUAL: {
				int a = pop(interpreter).integer;
				int b = pop(interpreter).integer;
				bool result = b <= a;
				push(interpreter, make_boolean(result));
				break;
			}
			case OP_GREATER: {
				int a = pop(interpreter).integer;
				int b = pop(interpreter).integer;
				bool result = b > a;
				push(interpreter, make_boolean(result));
				break;
			}
			case OP_GREATER_EQUAL: {
				int a = pop(interpreter).integer;
				int b = pop(interpreter).integer;
				bool result = b >= a;
				push(interpreter, make_boolean(result));
				break;
			}
			case OP_JUMP_IF_FALSE: {
				bool cond = pop(interpreter).boolean;
				int dest = next(interpreter);
				if (!cond) {
					interpreter->index = dest;
				}
				break;
			}
			case OP_JUMP: {
				int dest = next(interpreter);
				interpreter->index = dest;
				break;
			}
			case OP_CALL: {
#ifdef DEBUG
				printf("CALL\n");
#endif
				Frame *frame =
				    &interpreter
					 ->frames[interpreter->frame_count++];
				frame->offset = interpreter->offset;
				interpreter->offset = interpreter->stack_length;

				int dest = next(interpreter);
				int parameter_count = next(interpreter);
				interpreter->offset -= parameter_count;
				frame->return_address = interpreter->index;
				interpreter->index = dest;
				break;
			}
			case OP_RETURN: {
#ifdef DEBUG
				printf("RETURN\n");
#endif
				Object return_value = pop(interpreter);
				int pops = next(interpreter);
				for (int i = 0; i < pops; i++) {
					pop(interpreter);
				}
				push(interpreter, return_value);
				Frame *frame =
				    &interpreter
					 ->frames[--interpreter->frame_count];
				interpreter->index = frame->return_address;
				interpreter->offset = frame->offset;
				break;
			}
			default:
				fprintf(stderr, "Invalid opcode: %d\n",
					op_code);
				exit(EXIT_FAILURE);
				break;
		}
#ifdef DEBUG
		for (int i = 0; i < interpreter->frame_count; i++) {
			printf("    ");
		}
		printf("\\- Stack: ");
		for (int j = 0; j < interpreter->stack_length; ++j) {
			printf("%d, ", interpreter->stack[j].integer);
		}
		printf("\n");
#endif
	}
	// return pop(interpreter);
	return 0;
}

static uint32_t next(Interpreter *interpreter) {
	return interpreter->chunk->code[interpreter->index++];
}

static void push(Interpreter *interpreter, Object value) {
	interpreter->stack[interpreter->stack_length++] = value;
}

static Object pop(Interpreter *interpreter) {
	return interpreter->stack[--interpreter->stack_length];
}

static Object make_integer(int value) {
	Object object;
	object.integer = value;
	return object;
}

static Object make_boolean(bool value) {
	Object object;
	object.boolean = value;
	return object;
}
