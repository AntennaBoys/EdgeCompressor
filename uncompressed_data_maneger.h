#ifndef UNCOMPRESSEDMANEGER
#define UNCOMPRESSEDMANEGER
#include "jsonprint.h"
#include <stdio.h>

struct UncompressedData{
    size_t maxSize;
    size_t currentSize;
    FILE* output;
    long* timestamps;
    float* values;
} typedef UncompressedData;
UncompressedData createUncompressedDataManeger(char* filePath);
void resizeUncompressedData(UncompressedData* data);
void insertData(UncompressedData* data, long timestamp, float value, int* first);
void deleteUncompressedDataManeger(UncompressedData* data);
void forceCompressData(UncompressedData* data, int first);

#endif