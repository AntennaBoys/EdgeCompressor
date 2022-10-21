#ifndef JSONPRINT
#define JSONPRINT
#include <stdio.h>

FILE* openFile(char* fileName);
void closeFile(FILE* file);
void writeSwingToFile(FILE *file, struct swing model, int index, int first);
void writeGorillaToFile(FILE* file, struct Gorilla model, int index, int first);
void writePMCMeanToFile(FILE* file, struct PMCMean model, int index, int first);
void writeModelToFile(FILE* file, struct SelectedModel model, int first, int startTime, int endTime);
#endif