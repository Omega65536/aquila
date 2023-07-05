#ifndef TYPE_H
#define TYPE_H

#include <stdio.h>

typedef enum Type {
	TY_INTEGER,
	TY_BOOLEAN,
} Type;

void printType(FILE *file, Type type);

#endif
