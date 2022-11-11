#ifndef UNCOMPRESSEDMANEGER
#define UNCOMPRESSEDMANEGER
#include "jsonprint.h"
#include <stdio.h>
#include "compressed_segment_builder.h"
struct Uncompressed_data{
    size_t max_size;
    size_t current_size;
    FILE* output;
    long* timestamps;
    float* values;
    int new_builder;
    Compressed_segment_builder segment_builder;
} typedef Uncompressed_data;
Uncompressed_data create_uncompressed_data_maneger(char* file_path);
void resize_uncompressed_data(Uncompressed_data* data);
void insert_data(Uncompressed_data* data, long timestamp, float value, int* first);
void delete_uncompressed_data_maneger(Uncompressed_data* data);
void force_compress_data(Uncompressed_data* data, int first);

#endif