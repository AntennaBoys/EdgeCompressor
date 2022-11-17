#include "polyswing.h"
#include "constants.h"
#include "bitreader.h"


void update_ATA(Mat* ATA, long new_x);
void update_ATY(Mat* ATY, long new_x, double new_y);
Mat* update_all(Mat* ATA, Mat* ATY, long new_x, double new_y, Poly_swing *model);
terms ingest(long x, double y, int printmat, Mat* ATA, Mat* ATY, Poly_swing *model);
void reset_struct(Poly_swing *data);

int fit_values_polyswing(Poly_swing *data, long timeStamp, double value){
    double maximum_deviation = fabs(value * (data->error_bound / 100.1));

    data->deltaTime = timeStamp - data->first_timestamp;
    if (data->length == 0) {
        // Line 1 - 2 of Algorithm 1 in the Swing and Slide paper.
        data->first_timestamp = timeStamp;
        data->last_timestamp = timeStamp;
        data->first_value = value;
        data->length += 1;
        data->ATA = newmat(3,3, 0);
        data->ATY = newmat(3,1, 0);
        return 1;
    }
    else if (data->length == 1) {
        // Line 3 of Algorithm 1 in the Swing and Slide paper.
        data->second_value = value;
        data->second_timestamp = timeStamp;
        data->length += 1;
        return 1;
    } else if (data->length == 2){
        data->last_timestamp = timeStamp;
        ingest(0, data->first_value,0, data->ATA, data->ATY, data);
        ingest(data->second_timestamp - data->first_timestamp, data->second_value,0, data->ATA, data->ATY, data);

        Mat* upperATA = copyvalue(data->ATA);
        Mat* upperATY = copyvalue(data->ATY);
        data->upper = ingest(data->deltaTime, value+maximum_deviation, 0, upperATA, upperATY, data);
        freemat(upperATA);
        freemat(upperATY);

        Mat* lowerATA = copyvalue(data->ATA);
        Mat* lowerATY = copyvalue(data->ATY);
        data->lower = ingest(data->deltaTime, value-maximum_deviation, 0, lowerATA, lowerATY, data);
        freemat(lowerATA);
        freemat(lowerATY);

        data->current = ingest(data->deltaTime, value,0, data->ATA, data->ATY, data);
        data->length += 1;
        return 1;
    }
    else {
        // Line 6 of Algorithm 1 in the Swing and Slide paper.
        double upper_bound_approximate_value = data->upper.pow2 * data->deltaTime * data->deltaTime + data->upper.pow1 * data->deltaTime + data->upper.pow0;
        double lower_bound_approximate_value = data->lower.pow2 * data->deltaTime * data->deltaTime + data->lower.pow1 * data->deltaTime + data->lower.pow0;

        if (upper_bound_approximate_value + maximum_deviation < value
            || lower_bound_approximate_value - maximum_deviation > value)
        {
            return 0;
        } else {
            data->last_timestamp = timeStamp;

            // Line 17 of Algorithm 1 in the Swing and Slide paper.
            if (upper_bound_approximate_value - maximum_deviation > value) {
                Mat* upperATA = copyvalue(data->ATA);
                Mat* upperATY = copyvalue(data->ATY);
                data->upper = ingest(data->deltaTime, value+maximum_deviation, 0, upperATA, upperATY, data);
                freemat(upperATA);
                freemat(upperATY);
            }
            if (lower_bound_approximate_value + maximum_deviation < value) {
                Mat* lowerATA = copyvalue(data->ATA);
                Mat* lowerATY = copyvalue(data->ATY);
                data->lower = ingest(data->deltaTime, value-maximum_deviation, 0, lowerATA, lowerATY, data);
                freemat(lowerATA);
                freemat(lowerATY);
            }
            if (data->terminateSegment){
                return 0;
            }
            struct terms temp = ingest(data->deltaTime, value,1, data->ATA, data->ATY, data);
            if (data->terminateSegment){
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
            model->terminateSegment = 1;
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

Poly_swing getPolySwing(double errorBound){
    Poly_swing model;
    model.error_bound = errorBound;
    model.length = 0;
    model.deltaTime = 0;
    model.terminateSegment = 0;
    return model;
}

void deletePolySwing(Poly_swing* poly_swing){
    freemat(poly_swing->ATA);
    freemat(poly_swing->ATY);
}

float* gridPolySwing(float c, float b, uint8_t* values, long* timestamps, int timestampCount){
    BitReader bitReader = tryNewBitreader(values, 4);
    float a = intToFloat(read_bits(&bitReader, 32));
    float* result;
    result = malloc(timestampCount * sizeof(*result));
    if(!result){
        printf("CALLOC ERROR (gridPolySwing: result)\n");
    }
    long deltatime;
    for(int i = 0; i < timestampCount; i++){
        deltatime = timestamps[i] - timestamps[0];
        result[i] = b * deltatime + a * (deltatime * deltatime) + c;
    }
    return result;
}