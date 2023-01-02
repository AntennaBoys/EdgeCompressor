#include "vector_based.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "distance_calculator.h"

#define ERROR 50

Vector_based get_vector_based(){
    Vector_based vb;
    vb.prev = (Position){ .latitude = 0, .longitude = 0};
    vb.current = (Position){ .latitude = 0, .longitude = 0};
    vb.length = 0;
    vb.current_delta = 0;
    vb.model_length = 0;
    vb.max_timestamps = 256;
    vb.timestamps = calloc(vb.max_timestamps, sizeof(*vb.timestamps));
    vb.current_timestamp_index = 0;
    return vb;   
}

void reset_vector_based(Vector_based* vb){
    vb->prev = (Position){ .latitude = 0, .longitude = 0};
    vb->current = (Position){ .latitude = 0, .longitude = 0};
    vb->length = 0;
    vb->current_delta = 0;
    vb->model_length = 0;
    free_vectorbased(vb);
    vb->max_timestamps = 256;
    vb->timestamps = calloc(vb->max_timestamps, sizeof(*vb->timestamps));
    vb->current_timestamp_index = 0;
}

int fit_values_vector_based(Vector_based *data, long time_stamp, double latitude, double longitude, float error){
    if(data->length == 0){
        data->start = (Position){.latitude = latitude, .longitude = longitude};
        data->prev = (Position){ .latitude = latitude, .longitude = longitude};
        data->start_time = time_stamp;
        data->length++;
        data->model_length++;
        data->timestamps[data->current_timestamp_index] = time_stamp;
        data->error_sum = 0;
        return 1;
    }
    else if (data->length == 1) {
        data->current_delta = time_stamp - data->start_time;

        data->current = (Position){ .latitude = latitude, .longitude = longitude};
        data->end_time = time_stamp;
        data->length++;        
        
        // Build vector
        data->vec.x = data->current.longitude - data->prev.longitude;
        data->vec.y = data->current.latitude - data->prev.latitude;

        // Scale down vector to only reflect the change for a single time step
        data->vec.y = data->vec.y / (double)(data->current_delta);
        data->vec.x = data->vec.x / (double)(data->current_delta);

        data->timestamps[data->current_timestamp_index] = time_stamp;
        data->model_length++;
        return 1;
    } 
    else {
        data->current_delta = time_stamp - data->end_time;


        // Make prediction
        // Scale up vector to reflect the change for the current delta
        Position prediction; 
        prediction.latitude = data->current.latitude + (data->vec.y * (double)(data->current_delta));
        prediction.longitude = data->current.longitude + (data->vec.x * (double)(data->current_delta));

        // Update current
        data->current = (Position){ .latitude = latitude, .longitude = longitude};

        // calls Distance calculator, to get distance between two points on a sphere, takes two pairs of lat,long
        // handled as a sphere, which means we get some measure of error the longer a distance is.
        double m_distance = haversine_distance(prediction.latitude, prediction.longitude,data->current.latitude, data->current.longitude);

        // Calculate distance between predicted and current
        double distance = sqrt( (prediction.longitude - data->current.longitude) * (prediction.longitude - data->current.longitude) 
                                + (prediction.latitude - data->current.latitude) * (prediction.latitude - data->current.latitude) );

        //printf("Distance: %f\n", distance);
        data->current = prediction;

        data->length++;


        if(m_distance > error){
            return 0;
        } 

        //printf("Vector --- x: %f, y: %f\n", data->vec.x, data->vec.y);
        // printf("%lf, %lf\n", prediction.latitude, prediction.longitude);
        data->model_length++;
        data->end_time = time_stamp;
        data->timestamps[data->current_timestamp_index] = time_stamp;
        data->error_sum += (float)m_distance;
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
