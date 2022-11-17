#ifndef SWING
#define SWING

//
// Created by power on 05-10-2022.
//

#include <stddef.h>
#include <stdint.h>
struct Swing {
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
    /// Slope for the linear function specifying the upper bound for the current
    /// model.
    double upper_bound_slope;
    /// Intercept for the linear function specifying the upper bound for the
    /// current model.
    double upper_bound_intercept;
    /// Slope for the linear function specifying the lower bound for the current
    /// model.
    double lower_bound_slope;
    /// Intercept for the linear function specifying the lower bound for the
    /// current model.
    double lower_bound_intercept;
    /// The number of data points the current model has been fitted to.
    int length;
} typedef Swing;
int fitValueSwing(Swing *data, long timeStamp, double value);
double getModelFirst(Swing current_swing);
double getModelLast(Swing current_swing);
float get_bytes_per_value_swing(Swing* data);
void get_model_swing(Swing* data, float arr[]);
size_t get_length_swing(Swing* data);

Swing getSwing(double error_bound);
void resetSwing(Swing *data);
float* gridSwing(float min, float max, uint8_t values, long* timestamps,int timestamp_count);

#endif