#include "vector_based.h"
#include <stdio.h>
#include <stdlib.h>

#define ERROR 0.02

Vector_based get_vector_based(){
    Vector_based vb;
    vb.prev = (Position){ .latitude = 0, .longitude = 0};
    vb.current = (Position){ .latitude = 0, .longitude = 0};
    vb.length = 0;
    vb.current_delta = 0;
    vb.prev_delta = 0;
    vb.model_length = 0;
    vb.max_timestamps = 256;
    vb.timestamps = malloc(vb.max_timestamps * sizeof(*vb.timestamps));
    vb.current_timestamp_index = 0;
    return vb;   
}

void reset_vector_based(Vector_based* vb){
    vb->prev = (Position){ .latitude = 0, .longitude = 0};
    vb->current = (Position){ .latitude = 0, .longitude = 0};
    vb->length = 0;
    vb->current_delta = 0;
    vb->prev_delta = 0;
    vb->model_length = 0;
    free_vectorbased(vb);
    vb->max_timestamps = 256;
    vb->timestamps = malloc(vb->max_timestamps * sizeof(*vb->timestamps));
    vb->current_timestamp_index = 0;
}

int fit_values_vector_based(Vector_based *data, long time_stamp, double latitude, double longitude){
    if(data->length == 0){
        data->start = (Position){.latitude = latitude, .longitude = longitude};
        data->prev = (Position){ .latitude = latitude, .longitude = longitude};
        data->start_time = time_stamp;
        data->length++;
        data->model_length++;
        data->timestamps[data->current_timestamp_index++] = time_stamp;
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
        data->timestamps[data->current_timestamp_index++] = time_stamp;
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
            return 0;
        } 

        //printf("Vector --- x: %f, y: %f\n", data->vec.x, data->vec.y);
        // printf("%lf, %lf\n", prediction.latitude, prediction.longitude);
        data->model_length++;
        data->timestamps[data->current_timestamp_index++] = time_stamp;
        if(data->current_timestamp_index + 1 >= data->max_timestamps) {
            data->max_timestamps = data->max_timestamps * 2;
            data->timestamps = realloc(data->timestamps, data->max_timestamps * sizeof(*data->timestamps));
        }
        return 1;
        
    }
}
void free_vectorbased(Vector_based* vb){
    free(vb->timestamps);
}
