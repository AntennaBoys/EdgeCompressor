#include <stdio.h>
#include "getopt.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "paths.h"


typedef struct Cols {
    int col;
    float error;
    int isAbsolute;
} Cols;

typedef struct Arguments {
    Cols latCol, longCol;
    Cols* cols;
    int* text_cols;
    int timestampCol;
    int numberOfCols;
    int number_of_text_cols;
    int containsPosition;
    char* output;
} Arguments;

void removeQuotesFromArgs(char* str);
Arguments handleArguments(int argc, char *argv[]);