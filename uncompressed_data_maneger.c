#include "uncompressed_data_maneger.h"
#include "compression.h"
#include "jsonprint.h"
#include "timestamps.h"
#include "distance_calculator.h"
#define ERROR_BOUND 0.001

void resize(Uncompressed_data* data);
float get_vector_based_error(Vector_based *model);

Uncompressed_data create_uncompressed_data_maneger(FILE *output, int id, int* first){
    Uncompressed_data data;
    data.max_size = 1;
    data.current_size = 0;
    data.output = output;
    data.timestamps = calloc(data.max_size, sizeof(*data.timestamps));
    data.reset_internal_model = 1;
    data.first = first;
    data.id = id;
    if(!data.timestamps){
        printf("CALLOC ERROR(create_uncompressed_data_maneger->data.timestamps)\n");
    }
    data.values = calloc(data.max_size, sizeof(*data.values));
    if(!data.values){
        printf("CALLOC ERROR(create_uncompressed_data_maneger->data.values)\n");
    }
    return data;
}

void insert_vector_based_data(FILE* output, Vector_based *model, long timestamp, float lat, float lon, int *first, float error){
    if(!fit_values_vector_based(model, timestamp, lat, lon, error)){
        print_vector_based(output, model, first);
        reset_vector_based(model);
        fit_values_vector_based(model, timestamp, lat, lon, error);
    }
}

float get_vector_based_error(Vector_based *model){
    float sum_of_errors = 0;
    for(int i = 0; i < model->model_length; i++){
        long delta = model->timestamps[i] - model->timestamps[0];
        double lat = model->start.latitude + (model->vec.y * (double)delta);
        double lon = model->start.longitude + (model->vec.x * (double)delta);
        sum_of_errors += haversine_distance(model->lats[i], model->longs[i], lat, lon);
    }
    return sum_of_errors / model->model_length;
}

void print_vector_based(FILE* output, Vector_based *model, int *first){
    Selected_model selected_model = get_selected_model();
    select_vector_based(&selected_model, model);
    Timestamps timestamps = compress_residual_timestamps(model->timestamps, model->current_timestamp_index);
    writeModelToFile(output, timestamps, selected_model, first, model->start_time, model->end_time,
                     get_vector_based_error(model), 0);
    delete_selected_model(&selected_model);
    free_timestamps(&timestamps);
}



void resize(Uncompressed_data* data){
    data->timestamps = realloc(data->timestamps, data->max_size * sizeof(*data->timestamps));
    if(!data->timestamps){
        printf("REALLOC ERROR(resize->data.timestamps)\n");
    }
    data->values = realloc(data->values, data->max_size * sizeof(*data->values));
    if(!data->values){
        printf("REALLOC ERROR(resize->data.values)\n");
    }
}

void resize_uncompressed_data(Uncompressed_data* data){
    if(data->current_size < data->max_size/2){
        data->max_size = data->max_size/2;
        resize(data);
    }
    if(data->current_size >= data->max_size){
        data->max_size = data->max_size *2;
        resize(data);
    }
}


void insert_data(Uncompressed_data* data, long timestamp, float value, int* first, float error, int is_error_absolute){

    data->current_size++;
    resize_uncompressed_data(data);
    if(!data->reset_internal_model){
        data->segment_builder.uncompressed_timestamps = data->timestamps;
        data->segment_builder.uncompressed_values = data->values;
    }
    data->timestamps[data->current_size-1] = timestamp;
    data->values[data->current_size-1] = value;

    if(data->reset_internal_model){
        data->segment_builder = new_compressed_segment_builder(0, data->timestamps, data->values, data->current_size, error, is_error_absolute);

        data->reset_internal_model = 0;
    }else{
        try_to_update_models(&data->segment_builder, timestamp, value, is_error_absolute);
    }
    if(!can_fit_more(data->segment_builder)){
        try_compress(data, error, first);
    }
}

void force_compress_data(Uncompressed_data* data, int *first, float error){
    if(data->reset_internal_model){
        data->segment_builder.uncompressed_timestamps = data->timestamps;
        data->segment_builder.uncompressed_values = data->values;
        data->segment_builder = new_compressed_segment_builder(0, data->timestamps, data->values, data->current_size, error, data->is_absolute_error);
        data->reset_internal_model = 0;
    }
    forceCompress(data, error, first);
}
