#ifndef CGO_MAIN_H
#define CGO_MAIN_H

#include <stdlib.h>

typedef struct CdyneException {
    long code;
    char *message;
} CdyneException;

typedef struct CdyneChild {
    char *key;
    char *value;
} CdyneChild;

typedef struct CdyneParent {
    char *exampleString;
    int exampleInt;
    double exampleDouble;
    int exampleBool;
    CdyneChild* children;
} CdyneParent;

#endif