#include "vector_based.h"
#include <stdio.h>

#define ERROR 2.5

Vector_based getVector_based(){
    Vector_based vb;
    vb.prev = (Position){ .latitude = 0, .longitude = 0};
    vb.current = (Position){ .latitude = 0, .longitude = 0};
    vb.length = 0;
    vb.currentDelta = 0;
    vb.prevDelta = 0;
    return vb;   
}


int fitValuesVectorBased(Vector_based *data, long timeStamp, double latitude, double longitude){
    FILE *test1;
    test1 = fopen("/home/teis/git/EdgeCompressor/vector_plot.csv", "a");
    if(data->length == 0){

        fprintf(test1,"%lf, %lf\n", latitude, longitude);
        fclose(test1);

        data->prev = (Position){ .latitude = latitude, .longitude = longitude};
        data->startTime = timeStamp;
        data->length++;
        return 1;
    }
    else if (data->length == 1) {
        data->prevDelta = data->currentDelta;
        data->currentDelta = timeStamp - data->startTime;


        // printf("%lf, %lf\n", latitude, longitude);
        fprintf(test1,"%lf, %lf\n", latitude, longitude);
        fclose(test1);

        data->current = (Position){ .latitude = latitude, .longitude = longitude};
        data->endTime = timeStamp;
        data->length++;        
        
        // Build vector
        data->vec.x = data->current.longitude - data->prev.longitude;
        data->vec.y = data->current.latitude - data->prev.latitude;
        return 1;
    } 
    else {
        data->prevDelta = data->currentDelta;
        data->currentDelta = timeStamp - data->endTime;
        data->endTime = timeStamp;


        // Make prediction
        Position prediction; 

        // Scale vector down based on previous delta
        data->vec.y = data->vec.y / (double)(data->prevDelta);
        data->vec.x = data->vec.x / (double)(data->prevDelta);

        // Scale vector up based on current delta
        data->vec.y *= (double)(data->currentDelta);
        data->vec.x *= (double)(data->currentDelta);

        prediction.latitude = data->current.latitude + data->vec.y;
        prediction.longitude = data->current.longitude + data->vec.x;


        // Update current
        data->current = (Position){ .latitude = latitude, .longitude = longitude};

        // Calculate distance between predicted and current
        double distance = sqrt( (prediction.longitude - data->current.longitude) * (prediction.longitude - data->current.longitude) 
                                + (prediction.latitude - data->current.latitude) * (prediction.latitude - data->current.latitude) );


        //printf("Distance: %f\n", distance);
        data->current = prediction;

        data->length++;

        if(distance > ERROR){
            // printf("%lf, %lf\n", latitude, longitude);
            fprintf(test1,"%lf, %lf\n", latitude, longitude);
            fclose(test1);

            // Set point as the first point in the next segment
            data->prev = (Position){ .latitude = latitude, .longitude = longitude};
            data->startTime = timeStamp;
            data->length = 1;

            return 0;
        } 

        //printf("Vector --- x: %f, y: %f\n", data->vec.x, data->vec.y);
        // printf("%lf, %lf\n", prediction.latitude, prediction.longitude);

        return 1;
        
    }


}
