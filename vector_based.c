#include "vector_based.h"
#include <stdio.h>

#define ERROR 2.5

Vector_based get_vector_based(){
    Vector_based vb;
    vb.prev = (Position){ .latitude = 0, .longitude = 0};
    vb.current = (Position){ .latitude = 0, .longitude = 0};
    vb.length = 0;
    vb.current_delta = 0;
    vb.prev_delta = 0;
    return vb;   
}

void reset_vector_based(Vector_based* vb){
    vb->prev = (Position){ .latitude = 0, .longitude = 0};
    vb->current = (Position){ .latitude = 0, .longitude = 0};
    vb->length = 0;
    vb->current_delta = 0;
    vb->prev_delta = 0;
}

int fit_values_vector_based(Vector_based *data, long time_stamp, double latitude, double longitude){
    if(data->length == 0){
        data->start = (Position){.latitude = latitude, .longitude = longitude};
        data->prev = (Position){ .latitude = latitude, .longitude = longitude};
        data->start_time = time_stamp;
        data->length++;
        return 1;
    }
    else if (data->length == 1) {
        data->prev_delta = data->current_delta;
        data->current_delta = time_stamp - data->start_time;


        data->current = (Position){ .latitude = latitude, .longitude = longitude};
        data->end_time = time_stamp;
        data->length++;        
        
        // Build vector
        data->vec.x = data->current.longitude - data->prev.longitude;
        data->vec.y = data->current.latitude - data->prev.latitude;
        return 1;
    } 
    else {
        data->prev_delta = data->current_delta;
        data->current_delta = time_stamp - data->end_time;
        data->end_time = time_stamp;


        // Make prediction
        Position prediction; 

        // Scale vector down based on previous delta
        data->vec.y = data->vec.y / (double)(data->prev_delta);
        data->vec.x = data->vec.x / (double)(data->prev_delta);

        // Scale vector up based on current delta
        data->vec.y *= (double)(data->current_delta);
        data->vec.x *= (double)(data->current_delta);

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

            // Set point as the first point in the next segment
            data->prev = (Position){ .latitude = latitude, .longitude = longitude};
            data->start_time = time_stamp;
            data->length = 1;

            return 0;
        } 

        //printf("Vector --- x: %f, y: %f\n", data->vec.x, data->vec.y);
        // printf("%lf, %lf\n", prediction.latitude, prediction.longitude);

        return 1;
        
    }


}
