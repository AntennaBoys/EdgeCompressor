#ifndef COMPRESSION
#define COMPRESSION
#include <stddef.h>
#include <stdint.h>
#include "PMCMean.h"
#include "swing.h"
#include "gorilla.h"
#include "mod.h"

struct CompressedSegmentBuilder{
    int start_index;
    struct PMCMean pmceman;
    int pmc_mean_could_fit_all;
    struct swing swing;
    int swing_could_fit_all;
    struct Gorilla gorilla;
    int* uncompressed_timestamps;
    float* uncompressed_values;
} typedef CompressedSegmentBuilder;

CompressedSegmentBuilder newCompressedSegmentBuilder(int startIndex, int* uncompressedTimestamps, float* uncompressedValues, int uncompressedValuesLength, float errorBound);
#endif