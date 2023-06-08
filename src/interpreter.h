#ifndef INTERPETER_H
#define INTERPRETE_H

#include "chunk.h"

typedef struct Interpreter {
        Chunk *chunk;
	int index;
        int *stack;
        int length;
        int capacity;
} Interpreter;

void initInterpreter(Interpreter *interpreter, Chunk *chunk);
void freeInterpreter(Interpreter *interpreter);

int interpret(Interpreter *interpreter);

#endif
