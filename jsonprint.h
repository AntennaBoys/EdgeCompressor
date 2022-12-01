#ifndef JSONPRINT
#define JSONPRINT
#include <stdio.h>
#include "swing.h"
#include "gorilla.h"
#include "PMCMean.h"
#include "mod.h"
#include "timestamps.h"


FILE* openFile(char* fileName);
void closeFile(FILE* file);
void writeModelToFile(FILE* file, Timestamps timestamps, Selected_model model, int *first, int start_time, int end_time, double error, int column_id);
void write_text_to_file(FILE* file, int* first, int column_id, int count, char* string, long start_time, long end_time, Timestamps timestamps);
#endif