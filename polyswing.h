#ifndef POLYSWING
#define POLYSWING
#include "MatLib.h"
#include "constants.h"
#include "bitreader.h"
#include <stdio.h>
#include "math.h"
#include <time.h>
#include <string.h>
//
// Created by power on 05-10-2022.
//

typedef struct terms {
    double pow2;
    double pow1;
    double pow0;
} terms;

struct polySwing {
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

    long deltaTime;
    
    terms upper;
    terms lower;
    terms current;


    /// The number of data points the current model has been fitted to.
    int length;
    //poly
    long second_timestamp;
    double second_value;
    Mat* ATA;
    Mat* ATY;
    int terminateSegment;
};


int fitValuesPolySwing(struct polySwing *data, long timeStamp, double value);
double getModelFirst(struct swing current_swing);
double getModelLast(struct swing current_swing);
struct polySwing getPolySwing(double errorBound);
float get_bytes_per_value_polyswing(struct polySwing* data);
void deletePolySwing(struct polySwing* polySwing);
float* gridPolySwing(float pow0, float pow1, uint8_t* values, long* timestamps, int timestampCount);
#endif