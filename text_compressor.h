//
// Created by danie on 30-11-2022.
//

#ifndef COMPRESSEXEC_TEXT_COMPRESSOR_H
#define COMPRESSEXEC_TEXT_COMPRESSOR_H

#include <stdlib.h>
#include "jsonprint.h"

typedef struct Text_compressor{
    char* string;
    long* timestamps;
    unsigned int count;
    int max_timestamps;
    int id;

} Text_compressor;

Text_compressor get_text_compressor();
int fit_string(Text_compressor* data, char* string, long timestamp);
void reset_text_compressor(Text_compressor* data, char* string, long timestamp);
void print_compressed_text(Text_compressor* data, FILE* file, int* first);


#endif //COMPRESSEXEC_TEXT_COMPRESSOR_H
