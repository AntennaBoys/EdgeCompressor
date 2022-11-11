#ifndef COMPRESSION
#define COMPRESSION
#include <stddef.h>
#include <stdint.h>
#include "PMCMean.h"
#include "swing.h"
#include "gorilla.h"
#include "mod.h"
#include "jsonprint.h"
#include "uncompressed_data_maneger.h"

struct CompressedSegmentBuilder{
    size_t start_index;
    struct PMCMean pmceman;
    int pmc_mean_could_fit_all;
    struct swing swing;
    int swing_could_fit_all;
    struct Gorilla gorilla;
    long* uncompressed_timestamps;
    float* uncompressed_values;
} typedef CompressedSegmentBuilder;

CompressedSegmentBuilder newCompressedSegmentBuilder(size_t startIndex, long* uncompressedTimestamps, float* uncompressedValues, size_t endIndex, double errorBound);
void tryCompress(UncompressedData* data, double errorBound, int* first);
void forceCompress(UncompressedData* data, double errorBound, int first);

#endif