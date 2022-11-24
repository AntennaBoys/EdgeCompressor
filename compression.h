#ifndef COMPRESSION
#define COMPRESSION
#include <stddef.h>
#include <stdint.h>
#include "PMCMean.h"
#include "swing.h"
#include "gorilla.h"
#include "mod.h"
#include "polyswing.h"
#include "uncompressed_data_maneger.h"
#include "compressed_segment_builder.h"


Compressed_segment_builder new_compressed_segment_builder(size_t startIndex, long* uncompressedTimestamps, float* uncompressedValues, size_t endIndex, double error_bound, int is_error_absolute);
void try_compress(Uncompressed_data* data, double error_bound, int* first);
void forceCompress(Uncompressed_data* data, double error_bound, int first);
void try_to_update_models(Compressed_segment_builder* builder, long timestamp, float value);
int can_fit_more(Compressed_segment_builder builder);

#endif