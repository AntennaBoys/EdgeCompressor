#ifndef POLYSWING
#define POLYSWING
#include <stdio.h>
#include <math.h>
#include "MatLib.h"
#include <time.h>
#include <string.h>
#include <stdint.h>
//
// Created by power on 05-10-2022.
//

typedef struct terms {
    double pow2;
    double pow1;
    double pow0;
} terms;

struct Poly_swing {
    
    terms upper;
    terms lower;
    terms current;
    
    /// Maximum relative error for the value of each data point.
    double error_bound;
    /// Time at which the first value represented by the current model was
    /// collected.
    long first_timestamp;
    /// Time at which the last value represented by the current model was
    /// collected.
    long last_timestamp;
    /// First value in the segment the current model is fitted to.
    double first_value; // f64 instead of Value to remove casts in fit_value()

    long delta_time;
    
    //poly
    long second_timestamp;
    double second_value;

    Mat* ATA;
    Mat* ATY;

    /// The number of data points the current model has been fitted to.
    int length;

    int terminate_segment;    
} typedef Poly_swing;


int fit_values_polyswing(Poly_swing *data, long timestamp, double value, int is_error_absolute);
Poly_swing get_polyswing(double error_bound);
float get_bytes_per_value_polyswing(Poly_swing* data);
void delete_polyswing(Poly_swing* poly_swing);
float* grid_polyswing(float pow0, float pow1, uint8_t* values, long* timestamps, int timestamp_count);
#endif