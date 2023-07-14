#include "interpreter.h"
#include "chunk.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//#define DEBUG

static uint32_t next(Interpreter *interpreter);
static void push(Interpreter *interpreter, Object value);
static Object pop(Interpreter *interpreter);
static Object make_integer(int value);
static Object make_boolean(bool value);

void init_interpreter(Interpreter *interpreter, Chunk *chunk) {
        interpreter->chunk = chunk;
	interpreter->index = 0;
        interpreter->stack = malloc(4 * sizeof(Object));
        interpreter->length = 0;
        interpreter->capacity = 4;
}

void free_interpreter(Interpreter *interpreter) {
        free(interpreter->stack);
}

int interpret(Interpreter *interpreter) {
	for (;;) {
		OpCode op_code = next(interpreter);
		//printf("%d: %d\n", i, op_code);
                switch (op_code) {
                        case OP_NOOP:
                                break;
			case OP_EXIT:
				return 0;
                        case OP_PUSH_INTEGER: {
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
				int index = next(interpreter);
				push(interpreter, interpreter->stack[index]);
				break;
			}
                        case OP_STORE: {
                                 int index = next(interpreter);
                                 Object object = pop(interpreter);
                                 interpreter->stack[index] = object;
                                 break;
                        }
			case OP_PRINT_INTEGER: {
				int value = pop(interpreter).integer;
				printf("%d\n", value);
				break;
			}
			case OP_PRINT_BOOLEAN: {
				bool value = pop(interpreter).boolean;
                                if (value ) {
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
                                bool cond = pop(interpreter).boolean;
                                int dest = next(interpreter);
                                if (!cond) {
                                    interpreter->index = dest;
                                }
                                break;
                        }
                        default:
                                fprintf(stderr, "Invalid opcode: %d\n", op_code);
                                exit(EXIT_FAILURE);
                                break;
                }
#ifdef DEBUG
		printf("STACK: ");
                for (int j = 0; j < interpreter->length; ++j) {
                        printf("%d, ", interpreter->stack[j]);
                }
		printf("\n");
#endif
        }
        //return pop(interpreter);
	return 0;
}

static uint32_t next(Interpreter *interpreter) {
        return interpreter->chunk->code[interpreter->index++];
}

static void push(Interpreter *interpreter, Object value) {
        if (interpreter->length == interpreter->capacity) {
                interpreter->capacity *= 2;
                int new_size = interpreter->capacity * sizeof(Object);
                interpreter->stack = realloc(interpreter->stack, new_size);
        }
        interpreter->stack[interpreter->length] = value;
        interpreter->length++;
}

static Object pop(Interpreter *interpreter) {
        return interpreter->stack[--interpreter->length];
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
