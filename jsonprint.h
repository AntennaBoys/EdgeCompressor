#ifndef JSONPRINT
#define JSONPRINT
#include <stdio.h>
#include "swing.h"
#include "gorilla.h"
#include "PMCMean.h"
#include "mod.h"

FILE* openFile(char* fileName);
void closeFile(FILE* file);
void writeSwingToFile(FILE *file, Swing model, int index, int first);
void writeGorillaToFile(FILE* file, Gorilla model, int index, int first);
void writePMCMeanToFile(FILE* file, Pmc_mean model, int index, int first);
void writeModelToFile(FILE* file, struct SelectedModel model, int first, int startTime, int endTime, double error);

#endif