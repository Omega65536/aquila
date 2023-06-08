#include "interpreter.h"
#include "chunk.h"
#include <stdio.h>
#include <stdlib.h>

static void push(Interpreter *interpreter, int value);
static int pop(Interpreter *interpreter);

void initInterpreter(Interpreter *interpreter, Chunk *chunk) {
        interpreter->chunk = chunk;
        interpreter->stack = malloc(4 * sizeof(int));
        interpreter->length = 0;
        interpreter->capacity = 4;
}

void freeInterpreter(Interpreter *interpreter) {
        free(interpreter->stack);
}

int interpret(Interpreter *interpreter) {
        for (int i = 0; i < interpreter->chunk->length; ++i) {
		OpCode opCode = interpreter->chunk->code[i];
                switch (opCode) {
                        case OP_NOOP:
                                break;
                        case OP_CONST: {
                                ++i;
                                int value = interpreter->chunk->code[i];
                                push(interpreter, value);
                                break;
                        }
			case OP_STORE: {
				// TODO: implement me
				++i;
				pop(interpreter);
				break;
			}
			case OP_LOAD: {
				// TODO: implement me
				++i;
				push(interpreter, 1337);
				break;
			}
			case OP_PRINT: {
				++i;
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
                for (int j = 0; j < interpreter->length; ++j) {
                        printf("%d\n", interpreter->stack[j]);
                }
#endif
        }
        return pop(interpreter);
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
