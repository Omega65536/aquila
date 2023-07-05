#include "interpreter.h"
#include "chunk.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//#define DEBUG

static uint32_t next(Interpreter *interpreter);
static void push(Interpreter *interpreter, Object value);
static Object pop(Interpreter *interpreter);
static Object makeInteger(int value);
static Object makeBoolean(bool value);

void initInterpreter(Interpreter *interpreter, Chunk *chunk) {
        interpreter->chunk = chunk;
	interpreter->index = 0;
        interpreter->stack = malloc(4 * sizeof(Object));
        interpreter->length = 0;
        interpreter->capacity = 4;
}

void freeInterpreter(Interpreter *interpreter) {
        free(interpreter->stack);
}

int interpret(Interpreter *interpreter) {
	for (;;) {
		OpCode opCode = next(interpreter);
		//printf("%d: %d\n", i, opCode);
                switch (opCode) {
                        case OP_NOOP:
                                break;
			case OP_EXIT:
				return 0;
                        case OP_PUSH_INTEGER: {
                                int value = next(interpreter);
                                push(interpreter, makeInteger(value));
                                break;
                        }
                        case OP_PUSH_TRUE: {
                                push(interpreter, makeBoolean(true));
                                break;
                        }
                        case OP_PUSH_FALSE: {
                                push(interpreter, makeBoolean(false));
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
			case OP_PRINT_INTEGER: {
				int value = pop(interpreter).integer;
				printf("%d\n", value);
				break;
			}
			case OP_PRINT_BOOLEAN: {
				bool value = pop(interpreter).boolean;
                                if (value == 0) {
                                        printf("false\n");
                                } else {
                                        printf("true\n");
                                }
				break;
			}
                        case OP_ADD: {
                                int a = pop(interpreter).integer;
                                int b = pop(interpreter).integer;
                                int result = b + a;
                                push(interpreter, makeInteger(result));
                                break;
                        }
                        case OP_SUB: {
                                int a = pop(interpreter).integer;
                                int b = pop(interpreter).integer;
                                int result = b - a;
                                push(interpreter, makeInteger(result));
                                break;
                        }
                        case OP_MUL: {
                                int a = pop(interpreter).integer;
                                int b = pop(interpreter).integer;
                                int result = b * a;
                                push(interpreter, makeInteger(result));
                                break;
                        }
                        case OP_DIV: {
                                int a = pop(interpreter).integer;
                                int b = pop(interpreter).integer;
                                int result = b / a;
                                push(interpreter, makeInteger(result));
                                break;
                        }
                        case OP_NEGATE: {
                                int a = pop(interpreter).integer;
                                int result = -a;
                                push(interpreter, makeInteger(result));
                                break;
                        }
                        default:
                                fprintf(stderr, "Invalid opcode: %d\n", opCode);
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
                int newSize = interpreter->capacity * sizeof(Object);
                interpreter->stack = realloc(interpreter->stack, newSize);
        }
        interpreter->stack[interpreter->length] = value;
        interpreter->length++;
}

static Object pop(Interpreter *interpreter) {
        return interpreter->stack[--interpreter->length];
}

static Object makeInteger(int value) {
    Object object;
    object.integer = value;
    return object;
}

static Object makeBoolean(bool value) {
    Object object;
    object.boolean = value;
    return object;
}
