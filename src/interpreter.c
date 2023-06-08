#include "interpreter.h"
#include "chunk.h"
#include <stdio.h>
#include <stdlib.h>

//#define DEBUG

static uint32_t next(Interpreter *interpreter);
static void push(Interpreter *interpreter, int value);
static int pop(Interpreter *interpreter);

void initInterpreter(Interpreter *interpreter, Chunk *chunk) {
        interpreter->chunk = chunk;
	interpreter->index = 0;
        interpreter->stack = malloc(4 * sizeof(int));
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
                        case OP_CONST: {
                                int value = next(interpreter);
                                push(interpreter, value);
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
			case OP_PRINT: {
				int value = pop(interpreter);
				printf("%d\n", value);
				break;
			}
                        case OP_ADD: {
                                int a = pop(interpreter);
                                int b = pop(interpreter);
                                int result = b + a;
                                push(interpreter, result);
                                break;
                        }
                        case OP_SUB: {
                                int a = pop(interpreter);
                                int b = pop(interpreter);
                                int result = b - a;
                                push(interpreter, result);
                                break;
                        }
                        case OP_MUL: {
                                int a = pop(interpreter);
                                int b = pop(interpreter);
                                int result = b * a;
                                push(interpreter, result);
                                break;
                        }
                        case OP_DIV: {
                                int a = pop(interpreter);
                                int b = pop(interpreter);
                                int result = b / a;
                                push(interpreter, result);
                                break;
                        }
                        case OP_NEGATE: {
                                int a = pop(interpreter);
                                int result = -a;
                                push(interpreter, result);
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

static void push(Interpreter *interpreter, int value) {
        if (interpreter->length == interpreter->capacity) {
                interpreter->capacity *= 2;
                int newSize = interpreter->capacity * sizeof(int);
                interpreter->stack = realloc(interpreter->stack, newSize);
        }
        interpreter->stack[interpreter->length] = value;
        interpreter->length++;
}

static int pop(Interpreter *interpreter) {
        return interpreter->stack[--interpreter->length];
}
