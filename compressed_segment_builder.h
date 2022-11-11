#ifndef COMPRESSEDSEGMENTBUILDER
#define COMPRESSEDSEGMENTBUILDER
#include "PMCMean.h"
#include "swing.h"
#include "gorilla.h"
#include "polyswing.h"
struct CompressedSegmentBuilder{
    size_t start_index;
    struct PMCMean pmcmean;
    int pmc_mean_could_fit_all;
    struct swing swing;
    int swing_could_fit_all;
    struct Gorilla gorilla;
    int polyswing_could_fit_all;
    struct polySwing polyswing;
    long* uncompressed_timestamps;
    float* uncompressed_values;
} typedef CompressedSegmentBuilder;

#endif