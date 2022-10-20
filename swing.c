#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include "swing.h"

const uint8_t VALUE_SIZE_IN_BYTES = (uint8_t) sizeof(float);
const uint8_t VALUE_SIZE_IN_BITS = (uint8_t) 8 * VALUE_SIZE_IN_BYTES;


struct slopeAndIntercept {
    double slope;
    double intercept;
};

struct slopeAndIntercept compute_slope_and_intercept(
        long Timestamp,
        double first_value,
        long final_timestamp,
        double final_value);
int isNan(double val);
int equalOrNAN(double v1, double v2);
int fitValuesSwing(struct swing *data, long timeStamp, double value);



int fitValuesSwing(struct swing* data, long timeStamp, double value){
    double maximum_deviation = fabs(value * (data->error_bound / 100.1));

    if (data->length == 0) {
        // Line 1 - 2 of Algorithm 1 in the Swing and Slide paper.
        data->first_timestamp = timeStamp;
        data->last_timestamp = timeStamp;
        data->first_value = value;
        data->length += 1;
        return 1;
    } else if (isNan(data->first_value) || isNan(value)) {
        // Extend Swing to handle both types of infinity and NAN.
        if (equalOrNAN(data->first_value, value)) {
            data->last_timestamp = timeStamp;
            data->upper_bound_slope = value;
            data->upper_bound_intercept = value;
            data->lower_bound_slope = value;
            data->lower_bound_intercept = value;
            data->length += 1;
            return 1;
        } else {
            return 0;
        }
    } else if (data->length == 1) {
        // Line 3 of Algorithm 1 in the Swing and Slide paper.
        data->last_timestamp = timeStamp;
        struct slopeAndIntercept slopes = compute_slope_and_intercept(
                data->first_timestamp,
                data->first_value,
                timeStamp,
                value + maximum_deviation
        );

        data->upper_bound_slope = slopes.slope;
        data->upper_bound_intercept = slopes.intercept;

        slopes = compute_slope_and_intercept(
                data->first_timestamp,
                data->first_value,
                timeStamp,
                value - maximum_deviation
        );

        data->lower_bound_slope = slopes.slope;
        data->lower_bound_intercept = slopes.intercept;
        data->length += 1;
        return 1;
    } else {
        // Line 6 of Algorithm 1 in the Swing and Slide paper.
        double upper_bound_approximate_value = data->upper_bound_slope * timeStamp + data->upper_bound_intercept;
        double lower_bound_approximate_value = data->lower_bound_slope * timeStamp + data->lower_bound_intercept;

        if (upper_bound_approximate_value + maximum_deviation < value
           || lower_bound_approximate_value - maximum_deviation > value)
        {
            return 0;
        } else {
            data->last_timestamp = timeStamp;

            // Line 17 of Algorithm 1 in the Swing and Slide paper.
            if (upper_bound_approximate_value - maximum_deviation > value) {
                struct slopeAndIntercept slopes =
                        compute_slope_and_intercept(
                                data->first_timestamp,
                                data->first_value,
                                timeStamp,
                                value + maximum_deviation
                        );
                data->upper_bound_slope = slopes.slope;
                data->upper_bound_intercept = slopes.intercept;
            }

            // Line 15 of Algorithm 1 in the Swing and Slide paper.
            if (lower_bound_approximate_value + maximum_deviation < value) {
                struct slopeAndIntercept slopes =
                        compute_slope_and_intercept(
                                data->first_timestamp,
                                data->first_value,
                                timeStamp,
                                value - maximum_deviation
                        );
                data->lower_bound_slope = slopes.slope;
                data->lower_bound_intercept = slopes.intercept;
            }
            data->length += 1;
            return 1;
        }
    }
}

struct slopeAndIntercept compute_slope_and_intercept(
        long first_timestamp,
        double first_value,
        long final_timestamp,
        double final_value) {
    // An if expression is used as it seems that no values can be assigned to
    // first_value and final_value so slope * timestamp + intercept = INFINITY
    // or slope * timestamp + intercept = NEG_INFINITY.
    if (!isNan(first_value) && !isNan(final_value)){
        double slope = (final_value - first_value) / (final_timestamp - first_timestamp);
        double intercept = first_value - slope * first_timestamp;
        struct slopeAndIntercept sample;
        sample.slope = slope;
        sample.intercept = intercept;
        return sample;
    } else {
        struct slopeAndIntercept sample;
        sample.slope = first_value;
        sample.intercept = final_value;
        return sample;
    }
}

double getModelFirst(struct swing current_swing){
    return current_swing.upper_bound_slope * current_swing.first_timestamp + current_swing.upper_bound_intercept;
}

double getModelLast(struct swing current_swing){
    return current_swing.upper_bound_slope * current_swing.last_timestamp + current_swing.upper_bound_intercept;
}

int isNan(double val){
    return isnan(val) || isinf(val);
    //return val != val; //Wacky code but should work for now. Val is NAN if val != val returns 1
}

int equalOrNAN(double v1, double v2){
    return v1==v2 || (isNan(v1) && isNan(v2));
}

float get_bytes_per_value_swing(struct swing* data){
    return (float) (2 * VALUE_SIZE_IN_BYTES) / (float) data->length;
}
void get_model_swing(struct swing* data, float *arr){
  double first_value = data->upper_bound_slope * (double) data->first_timestamp + data->upper_bound_intercept;

  double last_value = data->upper_bound_slope * (double) data->last_timestamp + data->upper_bound_intercept;

  arr[0] = (float) first_value;
  arr[1] = (float) last_value;
}

size_t get_length_swing(struct swing* data){
  return data->length;
}

//
// Created by power on 23-09-2022.
//

