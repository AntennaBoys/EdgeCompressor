#ifndef COMPRESSEDSEGMENTBUILDER
#define COMPRESSEDSEGMENTBUILDER
#include "PMCMean.h"
#include "swing.h"
#include "gorilla.h"
#include "polyswing.h"
struct Compressed_segment_builder{
    size_t start_index;
    Pmc_mean pmc_mean;
    int pmc_mean_could_fit_all;
    Swing swing;
    int swing_could_fit_all;
    Gorilla gorilla;
    int polyswing_could_fit_all;
    Poly_swing polyswing;
    long* uncompressed_timestamps;
    float* uncompressed_values;
} typedef Compressed_segment_builder;

#endif