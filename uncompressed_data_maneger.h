#ifndef UNCOMPRESSEDMANEGER
#define UNCOMPRESSEDMANEGER
#include "jsonprint.h"
#include <stdio.h>
#include "vector_based.h"
#include "compressed_segment_builder.h"
struct Uncompressed_data{
    size_t max_size;
    size_t current_size;
    FILE* output;
    long* timestamps;
    float* values;
    int reset_internal_model;
    Compressed_segment_builder segment_builder;
    Vector_based vector_model;
    int* first;
    int id;
    int is_absolute_error;
} typedef Uncompressed_data;
Uncompressed_data create_uncompressed_data_maneger(FILE * output, int id, int* first);
void resize_uncompressed_data(Uncompressed_data* data);

void insert_data(Uncompressed_data* data, long timestamp, float value, int* first, float error, int is_error_absolute);
void insert_vector_based_data(FILE* output, Vector_based *model, long timestamp, float lat, float lon, int *first, float error);

void delete_uncompressed_data_maneger(Uncompressed_data* data);
void force_compress_data(Uncompressed_data* data, int* first, float error);
void print_vector_based(FILE* output, Vector_based *model, int *first);


#endif