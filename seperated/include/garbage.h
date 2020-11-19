#ifndef GARBAGE_H
#define GARBAGE_H

#include "stack.h"
#include "parser.h"
#include "lexer.h"
#include "cstring.h"

/*start of typedef*/
typedef enum {GrNode, GrCString, GrStack, GrTokensList, GrSplitsList,
                GrParsingReport, GrDefault} GrType;
typedef struct garbage Garbage;
typedef struct garbage_entry GarbageEntry;
/*end of typedef*/


/*start of struct defs*/
typedef struct garbage {
    Stack* stack;
} Garbage;

typedef struct garbage_entry {
    void *object;
    GrType type;
} GarbageEntry;
/*end of struct defs*/

/*start of func protos*/
Garbage *new_garbage();
void* w(const Garbage *gr, void* ptr, GrType type); // watch object for collection
unsigned int collect(const Garbage *gr);
void dispose(Garbage *gr);
void collect_and_dispose(Garbage *gr);
/*end of func protos*/

#endif