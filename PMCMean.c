#include "PMCMean.h"
#include "constants.h"
#include <math.h>
#include <stdio.h>


int fit_value_pmc(Pmc_mean *data, float value);
int is_value_within_error_bound(Pmc_mean*, float, float);
int equal_or_nan_pmc(float, float);
int is_nan_pmc(float);


int fit_value_pmc(Pmc_mean* data, float value){
    float nextMinValue = data->minValue < value ? data->minValue : value;
    float nextMaxValue = data->maxValue > value ? data->maxValue : value;
    float nextSumOfValues = data->sumOfValues + value;
    size_t nextLength = data->length+1;
    float average = (nextSumOfValues / nextLength);

    if(is_value_within_error_bound(data, nextMinValue, average) && is_value_within_error_bound(data, nextMaxValue, average)){
        data->minValue = nextMinValue;
        data->maxValue = nextMaxValue;
        data->sumOfValues = nextSumOfValues;
        data->length = nextLength;
        return 1;
    } else {
        return 0;
    }
}

int is_value_within_error_bound(Pmc_mean* data, float realValue, float approxValue){
    if(equal_or_nan_pmc(realValue, approxValue)){
        return 1;
    } else {
        float difference = realValue - approxValue;
        float result = fabsf(difference / realValue);
        return (result * 100) <= data->error;
    }
}

float get_bytes_per_value_pmc(Pmc_mean* data){
    return (float) VALUE_SIZE_IN_BYTES / (float) data -> length;
}


int equal_or_nan_pmc(float v1, float v2){
    return v1==v2 || (is_nan_pmc(v1) && is_nan_pmc(v2));
}

int is_nan_pmc(float val){
    return val != val; //Wacky code but should work for now. Val is NAN if val != val returns 1
}

float get_model_pmcmean(Pmc_mean* data){
    return (float) (data->sumOfValues / (double) data->length);
}

size_t get_length_pmcmean (Pmc_mean* data){
    return data->length;
}

Pmc_mean get_pmc_mean(double error_bound){
  Pmc_mean data;
  data.error = error_bound;
  data.minValue = NAN;
  data.maxValue = NAN;
  data.sumOfValues = 0;
  data.length = 0;
  return data;
}

void reset_pmc_mean(Pmc_mean *pmc){
  pmc->minValue = NAN;
  pmc->maxValue = NAN;
  pmc->sumOfValues = 0;
  pmc->length = 0;
}

float* grid_pmc_mean(float value, int timestamp_count){
    float* result;
    result = malloc(timestamp_count * sizeof(*result));
    if(!result){
        printf("CALLOC ERROR (grid_pmc_mean: result)\n");
    }
    for(int i = 0; i < timestamp_count; i++){
        result[i] = value;
    }
    return result;
}