#include <stdio.h>
#include <math.h>
#include <swing.h>

const uint8 VALUE_SIZE_IN_BYTES = (uint8) sizeof(float);
const uint8 VALUE_SIZE_IN_BITS = (uint8) 8 * VALUE_SIZE_IN_BYTES;


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
int fitValues(struct swing *data, long timeStamp, double value);


int mains(){
    struct swing data;
    data.error_bound = 5;
    data.first_timestamp = 0;
    data.last_timestamp = 0;
    data.first_value = NAN;
    data.upper_bound_slope = NAN;
    data.upper_bound_intercept = NAN;
    data.lower_bound_slope = NAN;
    data.lower_bound_intercept = NAN;
    data.length = 0;

    double values[10] = {1, 2, 3, 400, 5, 6, 7, 8, 9, 100};
    long ts[10] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    int lengthOfArray = sizeof(values)/sizeof(double);
    for(int i = 0; i<lengthOfArray; i++){
        printf("%d",fitValues(&data, ts[i], values[i]));
    }
}

int fitValues(struct swing* data, long timeStamp, double value){
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
//
// Created by power on 23-09-2022.
//

