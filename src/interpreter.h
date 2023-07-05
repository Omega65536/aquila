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

void initInterpreter(Interpreter *interpreter, Chunk *chunk);
void freeInterpreter(Interpreter *interpreter);

int interpret(Interpreter *interpreter);

#endif
