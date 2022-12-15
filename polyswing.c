#include "polyswing.h"
#include "constants.h"
#include "bitreader.h"


void update_ATA(Mat* ATA, long new_x);
void update_ATY(Mat* ATY, long new_x, double new_y);
Mat* update_all(Mat* ATA, Mat* ATY, long new_x, double new_y, Poly_swing *model);
terms ingest(long x, double y, int printmat, Mat* ATA, Mat* ATY, Poly_swing *model);
void reset_struct(Poly_swing *data);

int fit_values_polyswing(Poly_swing *data, long timestamp, double value, int is_error_absolute){
    double maximum_deviation = 0;
    if (is_error_absolute)  // check if using relative or absolute error bounds
    {
        maximum_deviation = data->error_bound;
    }
    else
    {
        maximum_deviation = fabs(value * (data->error_bound / 100.1));
    }

    data->delta_time = timestamp - data->first_timestamp;
    if (data->length == 0) {
        // Line 1 - 2 of Algorithm 1 in the Swing and Slide paper.
        data->first_timestamp = timestamp;
        data->last_timestamp = timestamp;
        data->first_value = value;
        data->length += 1;
        data->ATA = newmat(3,3, 0);
        data->ATY = newmat(3,1, 0);
        return 1;
    }
    else if (data->length == 1) {
        // Line 3 of Algorithm 1 in the Swing and Slide paper.
        data->second_value = value;
        data->second_timestamp = timestamp;
        data->length += 1;
        return 1;
    } else if (data->length == 2){
        data->last_timestamp = timestamp;
        ingest(0, data->first_value,0, data->ATA, data->ATY, data);
        ingest(data->second_timestamp - data->first_timestamp, data->second_value,0, data->ATA, data->ATY, data);

        Mat* upper_ATA = copyvalue(data->ATA);
        Mat* upper_ATY = copyvalue(data->ATY);
        data->upper = ingest(data->delta_time, value+maximum_deviation, 0, upper_ATA, upper_ATY, data);
        freemat(upper_ATA);
        freemat(upper_ATY);

        Mat* lower_ATA = copyvalue(data->ATA);
        Mat* lower_ATY = copyvalue(data->ATY);
        data->lower = ingest(data->delta_time, value-maximum_deviation, 0, lower_ATA, lower_ATY, data);
        freemat(lower_ATA);
        freemat(lower_ATY);

        data->current = ingest(data->delta_time, value,0, data->ATA, data->ATY, data);
        data->length += 1;
        return 1;
    }
    else {
        // Line 6 of Algorithm 1 in the Swing and Slide paper.
        double upper_bound_approximate_value = data->upper.pow2 * data->delta_time * data->delta_time + data->upper.pow1 * data->delta_time + data->upper.pow0;
        double lower_bound_approximate_value = data->lower.pow2 * data->delta_time * data->delta_time + data->lower.pow1 * data->delta_time + data->lower.pow0;

        // Prevent swapping
        if(upper_bound_approximate_value<=lower_bound_approximate_value){
            return 0;
        }


        if (upper_bound_approximate_value + maximum_deviation < value
            || lower_bound_approximate_value - maximum_deviation > value)
        {
            return 0;
        } else {
            data->last_timestamp = timestamp;


            // Line 17 of Algorithm 1 in the Swing and Slide paper.
            if (upper_bound_approximate_value - maximum_deviation > value) {
                Mat* upper_ATA = copyvalue(data->ATA);
                Mat* upper_ATY = copyvalue(data->ATY);
                data->upper = ingest(data->delta_time, value+maximum_deviation, 0, upper_ATA, upper_ATY, data);
                freemat(upper_ATA);
                freemat(upper_ATY);
            }
            if (lower_bound_approximate_value + maximum_deviation < value) {
                Mat* lower_ATA = copyvalue(data->ATA);
                Mat* lower_ATY = copyvalue(data->ATY);
                data->lower = ingest(data->delta_time, value-maximum_deviation, 0, lower_ATA, lower_ATY, data);
                freemat(lower_ATA);
                freemat(lower_ATY);
            }


            if (data->terminate_segment){
                return 0;
            }
            struct terms temp = ingest(data->delta_time, value,1, data->ATA, data->ATY, data);
            if (data->terminate_segment){
                return 0;
            }
            else {
                data->current = temp;
            }
            data->length += 1;

            return 1;
            }
    }
}


Mat* update_all(Mat* ATA, Mat* ATY, long new_x, double new_y, Poly_swing *model){
    update_ATA(ATA, new_x);
    update_ATY(ATY, new_x, new_y);

    Mat* _ATA, *_ATY, *res;

    // Get upper left element in ATA. This number describes the number of points we have ingested
    int elems = get(ATA, 1,1);

    // Return first point in ATY if we only have ingested one point (i.e. the y-value of the point)
    if(elems == 1){
        res = newmat(1,1,get(ATY,1,1));
    }
        // If two points are ingested, operate on the upper left 2x2 submatrix (of ATA) and first two elements of ATY.
        // This makes a linear regression
    else if(elems == 2){
        _ATA = submat(ATA, 1,2,1,2);
        Mat* sub = submat(ATY, 1,1,1,2);
        _ATY = transpose(sub);
        Mat* ATA_inv = inverse(_ATA);
        //showmat(_ATA);
        //showmat(ATA_inv);
        //exit(23);
        res = multiply(ATA_inv, _ATY);
        freemat(_ATA);
        freemat(_ATY);
        freemat(sub);
        freemat(ATA_inv);
    }
        // If three or more points are ingested, operate on the entire matrix to do a polynomial regression
    else if(elems >= 3){
        _ATA = ATA;
        _ATY = ATY;

        if(det(_ATA) != 0){
            Mat* ATA_inv = inverse(_ATA);
            res = multiply(ATA_inv, _ATY);
            //showmat(_ATA);
            //showmat(ATA_inv);
            freemat(ATA_inv);
        } else {
            printf("wrong\n");
            model->terminate_segment = 1;
            res = newmat(3,3,0);
        }
    }
    return res;
}

void update_ATY(Mat* ATY, long new_x, double new_y){
    float current = get(ATY, 1,1);
    set(ATY, 1, 1, current + pow(new_x, 0)*new_y);
    current = get(ATY, 1,2);
    set(ATY, 1, 2, current + pow(new_x, 1)*new_y);
    current = get(ATY, 1,3);
    set(ATY, 1, 3, current + pow(new_x, 2)*new_y);
}

void update_ATA(Mat* ATA, long new_x){
    //Øverste række
    float current = get(ATA, 1,1);
    set(ATA, 1, 1, current + pow(new_x, 0));
    current = get(ATA, 1,2);
    set(ATA, 1, 2, current + pow(new_x, 1));
    current = get(ATA, 1,3);
    set(ATA, 1, 3, current + pow(new_x, 2));
    //Mellemste række
    current = get(ATA, 2,1);
    set(ATA, 2, 1, current + pow(new_x, 1));
    current = get(ATA, 2,2);
    set(ATA, 2, 2, current + pow(new_x, 2));
    current = get(ATA, 2,3);
    set(ATA, 2, 3, current + pow(new_x, 3));
    //Nederste række
    current = get(ATA, 3,1);
    set(ATA, 3, 1, current + pow(new_x, 2));
    current = get(ATA, 3,2);
    set(ATA, 3, 2, current + pow(new_x, 3));
    current = get(ATA, 3,3);
    set(ATA, 3, 3, current + pow(new_x, 4));
}

terms ingest(long x, double y, int printmat, Mat* ATA, Mat* ATY, Poly_swing *model){
    Mat* res = update_all(ATA, ATY, x, y, model);

    terms result = {0,0,0};
    int pow = get(ATA, 1,1);

    if (pow > 2){
        result.pow2 = get(res, 3, 1);
        result.pow1 = get(res, 2, 1);
        result.pow0 = get(res, 1, 1);
    }
    freemat(res);
    return result;
}


void reset_struct(Poly_swing *data){
    data->length = 0;
}

float get_bytes_per_value_polyswing(Poly_swing* data){
    return (float) (3 * VALUE_SIZE_IN_BYTES) / (float) data->length;
}

Poly_swing get_polyswing(double error_bound){
    Poly_swing model;
    model.error_bound = error_bound;
    model.length = 0;
    model.delta_time = 0;
    model.terminate_segment = 0;
    return model;
}

void delete_polyswing(Poly_swing* poly_swing){
    freemat(poly_swing->ATA);
    freemat(poly_swing->ATY);
}

float* grid_polyswing(float c, float b, uint8_t* values, long* timestamps, int timestamp_count){
    BitReader bitReader = tryNewBitreader(values, 4);
    float a = intToFloat(read_bits(&bitReader, 32));
    float* result;
    result = calloc(timestamp_count, sizeof(*result));
    if(!result){
        printf("CALLOC ERROR (grid_polyswing: result)\n");
    }
    long deltatime;
    // printf("COEFFICIENTS: P2: %lf, P1: %lf, P0: %lf, delta: %ld\n", a, b, c, timestamps[0]);
    printf("f(x)=%lf(x-%ld)²+%lf(x-%ld)+%lf\n\n", a, timestamps[0], b, timestamps[0], c);
    for(int i = 0; i < timestamp_count; i++){
        deltatime = timestamps[i] - timestamps[0];
        result[i] = b * deltatime + a * (deltatime * deltatime) + c;
    }
    return result;
}