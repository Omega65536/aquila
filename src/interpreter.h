#ifndef INTERPETER_H
#define INTERPRETE_H

#include "chunk.h"
#include <stdbool.h>

typedef union Object {
	int integer;
	bool boolean;
} Object;

typedef struct Interpreter {
	Chunk *chunk;
	int index;
	Object *stack;
	int length;
	int capacity;
} Interpreter;

void init_interpreter(Interpreter *interpreter, Chunk *chunk);
void free_interpreter(Interpreter *interpreter);

int interpret(Interpreter *interpreter);

#endif
