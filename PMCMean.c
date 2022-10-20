#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <PMCMean.h>

const uint8 VALUE_SIZE_IN_BYTES = (uint8) sizeof(float);
const uint8 VALUE_SIZE_IN_BITS = (uint8) 8 * VALUE_SIZE_IN_BYTES;

int fitValue(struct PMCMean* data, float value){
    float nextMinValue = data->minValue < value ? data->minValue : value;
    float nextMaxValue = data->maxValue > value ? data->maxValue : value;
    float nextSumOfValues = data->sumOfValues + value;
    float nextLength = data->length+1;
    float average = (nextSumOfValues / nextLength);

    if(isValueWithinErrorBound(data, nextMinValue, average) && isValueWithinErrorBound(data, nextMaxValue, average)){
        data->minValue = nextMinValue;
        data->maxValue = nextMaxValue;
        data->sumOfValues = nextSumOfValues;
        data->length = nextLength;
        return 1;
    } else {
        return 0;
    }
}

int isValueWithinErrorBound(struct PMCMean* data, float realValue, float approxValue){
    if(equalOrNAN(realValue, approxValue)){
        return 1;
    } else {
        float difference = realValue - approxValue;
        float result = fabsf(difference / realValue);
        return (result * 100) <= data->error;
    }
}

float get_bytes_per_value_pmc(struct PMCMean* data){
    return (float) VALUE_SIZE_IN_BYTES / (float) data -> length;
}

int equalOrNAN(float v1, float v2){
    return v1==v2 || (isNan(v1) && isNan(v2));
}

int isNan(float val){
    return val != val; //Wacky code but should work for now. Val is NAN if val != val returns 1
}

float get_model_pmcmean(struct PMCMean* data){
    return (float) (data->sumOfValues / (double) data->length);
}

size_t get_length_pmcmean (struct PMCMean* data){
    return data->length;
}