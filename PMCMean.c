#include "PMCMean.h"
#include "constants.h"
#include <math.h>
#include <stdio.h>


int fitValuePMC(Pmc_mean *data, float value);
int isValueWithinErrorBound(Pmc_mean*, float, float);
int equalOrNAN_pmc(float, float);
int isNan_pmc(float);


int fitValuePMC(Pmc_mean* data, float value){
    float nextMinValue = data->minValue < value ? data->minValue : value;
    float nextMaxValue = data->maxValue > value ? data->maxValue : value;
    float nextSumOfValues = data->sumOfValues + value;
    size_t nextLength = data->length+1;
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

int isValueWithinErrorBound(Pmc_mean* data, float realValue, float approxValue){
    if(equalOrNAN_pmc(realValue, approxValue)){
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


int equalOrNAN_pmc(float v1, float v2){
    return v1==v2 || (isNan_pmc(v1) && isNan_pmc(v2));
}

int isNan_pmc(float val){
    return val != val; //Wacky code but should work for now. Val is NAN if val != val returns 1
}

float get_model_pmcmean(Pmc_mean* data){
    return (float) (data->sumOfValues / (double) data->length);
}

size_t get_length_pmcmean (Pmc_mean* data){
    return data->length;
}

Pmc_mean getPMCMean(double errorBound){
  Pmc_mean data;
  data.error = errorBound;
  data.minValue = NAN;
  data.maxValue = NAN;
  data.sumOfValues = 0;
  data.length = 0;
  return data;
}

void resetPMCMean(Pmc_mean *pmc){
  pmc->minValue = NAN;
  pmc->maxValue = NAN;
  pmc->sumOfValues = 0;
  pmc->length = 0;
}

float* gridPMCMean(float value, int timestampCount){
    float* result;
    result = malloc(timestampCount * sizeof(*result));
    if(!result){
        printf("CALLOC ERROR (gridPMCMean: result)\n");
    }
    for(int i = 0; i < timestampCount; i++){
        result[i] = value;
    }
    return result;
}