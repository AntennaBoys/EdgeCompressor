#ifndef COMPRESSEDSEGMENTBUILDER
#define COMPRESSEDSEGMENTBUILDER
#include "PMCMean.h"
#include "swing.h"
#include "gorilla.h"
#include "polyswing.h"
struct Compressed_segment_builder{
    Poly_swing polyswing; //152
    Swing swing; //72
    Gorilla gorilla; //40
    Pmc_mean pmc_mean; //32

    long* uncompressed_timestamps;
    float* uncompressed_values;
    int pmc_mean_could_fit_all;
    int swing_could_fit_all;
    int polyswing_could_fit_all;
    size_t start_index;

} typedef Compressed_segment_builder;

#endif