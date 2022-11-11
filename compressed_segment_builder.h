#ifndef COMPRESSEDSEGMENTBUILDER
#define COMPRESSEDSEGMENTBUILDER
#include "PMCMean.h"
#include "swing.h"
#include "gorilla.h"
#include "polyswing.h"
struct CompressedSegmentBuilder{
    size_t start_index;
    Pmc_mean pmcmean;
    int pmc_mean_could_fit_all;
    Swing swing;
    int swing_could_fit_all;
    Gorilla gorilla;
    int polyswing_could_fit_all;
    Poly_swing polyswing;
    long* uncompressed_timestamps;
    float* uncompressed_values;
} typedef CompressedSegmentBuilder;

#endif