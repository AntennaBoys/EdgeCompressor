#include "error_bound_calculator.h"

float calculate_error_bound(running_mean mean,float lastValue, int segmentLengt){
    mean.value = (mean.value * mean.lenght + lastValue * segmentLengt) / (mean.lenght + segmentLengt);
    mean.lenght++;
    if(mean.lenght < 10000){

    }else{
        mean.lenght = 500;
    }
}