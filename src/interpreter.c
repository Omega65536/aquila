#include "interpreter.h"
#include "chunk.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//#define DEBUG
//#define DEBUG_STACK

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
#ifdef DEBUG
		print_op_code(interpreter->chunk, interpreter->index);
#endif
		OpCode op_code = next(interpreter);
		// printf("INDEX: %d, OFFSET: %d\n", interpreter->index-1,
		// interpreter->offset);
		switch (op_code) {
			case OP_NOOP:
				break;
			case OP_EXIT:
				return 0;
			case OP_PUSH: {
				int value = next(interpreter);
				push(interpreter, make_integer(value));
				break;
			}
			case OP_POP: {
				pop(interpreter);
				break;
			}
			case OP_LOAD: {
				int index = next(interpreter);
				int var_index = interpreter->offset + index;
				Object var_value =
				    interpreter->stack[var_index];
				push(interpreter, var_value);
				break;
			}
			case OP_STORE: {
				int index = next(interpreter);
				int var_index = interpreter->offset + index;
				Object *var_location =
				    &interpreter->stack[var_index];
				Object object = pop(interpreter);
				*var_location = object;
				break;
			}
			case OP_PRINT_UNIT: {
				pop(interpreter);
				printf("unit\n");
				break;
			}
			case OP_PRINT_INTEGER: {
				int value = pop(interpreter).integer;
				printf("%d\n", value);
				break;
			}
			case OP_PRINT_BOOLEAN: {
				int value = pop(interpreter).integer;
				if (value == AQ_TRUE) {
					printf("true\n");
				} else {
					printf("false\n");
				}
				break;
			}
			case OP_ADD: {
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
				int cond = pop(interpreter).integer;
				int dest = next(interpreter);
				if (cond == AQ_FALSE) {
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
				int frame_index = interpreter->frame_count++;
				Frame *frame =
				    &interpreter->frames[frame_index];
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
				Object return_value = pop(interpreter);
				int pops = next(interpreter);
				for (int i = 0; i < pops; i++) {
					pop(interpreter);
				}
				push(interpreter, return_value);

				int frame_index = --interpreter->frame_count;
				Frame *frame =
				    &interpreter->frames[frame_index];
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
#ifdef DEBUG_STACK
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
	object.integer = value ? AQ_TRUE : AQ_FALSE;
	return object;
}
