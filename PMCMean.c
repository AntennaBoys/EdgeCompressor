#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "PMCMean.h"


int fitValue(struct PMCMean*, float);
int isValueWithinErrorBound(struct PMCMean*, float, float);
int equalOrNAN(float, float);
int isNan(float);

int mains() {
    struct PMCMean data;
    data.error = 11;
    data.minValue = NAN;
    data.maxValue = NAN;
    data.sumOfValues = 0;
    data.length = 0;
    printf("%f\n\n", data.error);
    float testValues[4] = {1.2, 1.4, 1.3, 1.4};
    int lengthOfArray = sizeof(testValues)/sizeof(float);

    printf("Lengthofarray: %d\n\n", lengthOfArray);
    for(int i = 0; i < lengthOfArray; i++){
        printf("Fits: ");
        printf("%d\n\n",fitValue(&data, testValues[i]));
    }

    return 0;
}

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

int equalOrNAN(float v1, float v2){
    return v1==v2 || (isNan(v1) && isNan(v2));
}

int isNan(float val){
    return val != val; //Wacky code but should work for now. Val is NAN if val != val returns 1
}