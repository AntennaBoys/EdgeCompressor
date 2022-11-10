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


CompressedSegmentBuilder newCompressedSegmentBuilder(size_t startIndex, long* uncompressedTimestamps, float* uncompressedValues, size_t endIndex, double errorBound);
void tryCompress(UncompressedData* data, double errorBound, int* first);
void forceCompress(UncompressedData* data, double errorBound, int first);
void tryToUpdateModels(CompressedSegmentBuilder* builder, long timestamp, float value);
int canFitMore(CompressedSegmentBuilder builder);

#endif