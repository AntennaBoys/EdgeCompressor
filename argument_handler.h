#include <stdio.h>
#include "getopt.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>



typedef struct Cols {
    int col;
    float error;
    int isAbsolute;
    int currentSize;
} Cols;

typedef struct Arguments {
    Cols latCol, longCol;
    Cols* cols;
    int timestampCol;
    int numberOfCols;
    int containsPosition;
} Arguments;

void removeQuotesFromArgs(char* str);
Arguments handleArguments(int argc, char *argv[]);