#include "type.h"
#include <stdio.h>

void print_type(FILE *file, Type type) {
    switch (type) {
        case TY_INTEGER:
            fprintf(file, "Integer");
            break;
        case TY_BOOLEAN:
            fprintf(file, "Boolean");
            break;
    }
}
