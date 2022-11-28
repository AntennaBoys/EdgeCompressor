#include "compression.h"
#include "jsonprint.h"
#include <math.h>
#include <stdio.h>

#define GORILLAMAX 50

void deleteCompressedSegementBuilder(Compressed_segment_builder* builder);
int can_fit_more(Compressed_segment_builder builder);
void try_to_update_models(Compressed_segment_builder* builder, long timestamp, float value, int is_error_absolute);
float* getReconstructedValues(Selected_model model, long* timestamps);
double getRMSE(float* baseValues, float* reconstructedValues, int values_count);

Compressed_segment_builder new_compressed_segment_builder(size_t startIndex, long* uncompressedTimestamps, float* uncompressedValues, size_t endIndex, double error_bound, int is_absolute_error){
    Compressed_segment_builder builder;
    builder.pmc_mean_could_fit_all = 1;
    builder.swing_could_fit_all = 1;
    builder.polyswing_could_fit_all = 1;
    builder.start_index = startIndex;
    builder.pmc_mean = get_pmc_mean(error_bound);
    builder.swing = getSwing(error_bound);
    builder.gorilla = get_gorilla();
    builder.polyswing = get_polyswing(error_bound);
    builder.uncompressed_timestamps = uncompressedTimestamps;
    builder.uncompressed_values = uncompressedValues;
    
    
    size_t currentIndex = startIndex;
    while (can_fit_more(builder) && currentIndex < endIndex)
    {
        long timestamp = uncompressedTimestamps[currentIndex];
        float value = uncompressedValues[currentIndex];
        try_to_update_models(&builder, timestamp, value, is_absolute_error);
        currentIndex++;
    }
    return builder;
}

int finishBatch(Compressed_segment_builder builder, FILE* file, int id ,int* first){
    Selected_model model = get_selected_model();
    select_model(&model, builder.start_index, &builder.pmc_mean, &builder.swing, &builder.gorilla, &builder.polyswing, builder.uncompressed_values);

    int start_time = builder.uncompressed_timestamps[builder.start_index];
    int end_time = builder.uncompressed_timestamps[model.end_index];

    float* reconstructedValues = getReconstructedValues(model, builder.uncompressed_timestamps);
    double error = getRMSE(builder.uncompressed_values, reconstructedValues, model.end_index+1);
    
    long* temp_times;
    temp_times = calloc((model.end_index+1), sizeof(*temp_times));
    for(int i = 0; i <= model.end_index; i++){
        temp_times[i] = builder.uncompressed_timestamps[i];
    }
    Timestamps time = compress_residual_timestamps(temp_times, model.end_index+1);
    writeModelToFile(file, time, model, first, start_time, end_time, error, id);
    free_timestamps(&time);
    delete_gorilla(&builder.gorilla);
    delete_polyswing(&builder.polyswing);
    delete_selected_model(&model);
    free(temp_times);
    free(reconstructedValues);
    return model.end_index + 1;
}

int can_fit_more(Compressed_segment_builder builder){
    return builder.pmc_mean_could_fit_all 
        || builder.swing_could_fit_all 
        || builder.gorilla.length < GORILLAMAX
        || builder.polyswing_could_fit_all;
}

void try_to_update_models(Compressed_segment_builder* builder, long timestamp, float value, int is_error_absolute){
    if(builder->pmc_mean_could_fit_all){
        builder->pmc_mean_could_fit_all = fit_value_pmc(&builder->pmc_mean, value, is_error_absolute);
    }
    if(builder->swing_could_fit_all){
        builder->swing_could_fit_all = fitValueSwing(&builder->swing, timestamp, value, is_error_absolute);
    }
    if(builder->gorilla.length < GORILLAMAX){
        fitValueGorilla(&builder->gorilla, value);
    }
    if(builder->polyswing_could_fit_all){
        builder->polyswing_could_fit_all = fit_values_polyswing(&builder->polyswing, timestamp, value, is_error_absolute);// && builder->polyswing.length < 4;
    }
}

void try_compress(Uncompressed_data* data, double error_bound, int* first){
    size_t currentIndex = 0;
    currentIndex = finishBatch(data->segment_builder, data->output, data->id, first);
    for (int i = 0; i+currentIndex < data->current_size; i++){
        data->values[i] = data->values[i+currentIndex];
        data->timestamps[i] = data->timestamps[i+currentIndex];
    }
    data->current_size = data->current_size - currentIndex;
    resize_uncompressed_data(data);
    data->reset_internal_model = 1;
    currentIndex = 0;
}

void forceCompress(Uncompressed_data* data, double error_bound, int *first){
    Compressed_segment_builder builder = data->segment_builder;
    size_t currentIndex = 0;
    while(currentIndex < data->current_size){
        currentIndex = finishBatch(builder, data->output, data->id, first);
        for (int i = 0; i+currentIndex < data->current_size; i++){
            data->values[i] = data->values[i+currentIndex];
            data->timestamps[i] = data->timestamps[i+currentIndex];
        }
        data->current_size = data->current_size - currentIndex;
        resize_uncompressed_data(data);
        currentIndex = 0;
        if(currentIndex != data->current_size){
            builder = new_compressed_segment_builder(currentIndex, data->timestamps, data->values, data->current_size, error_bound, data->is_absolute_error);
        }
    }
    //finishBatch(data->segment_builder, data->output, first);
}

float* getReconstructedValues(Selected_model model, long* timestamps){
    switch (model.model_type_id)
    {
    case PMC_MEAN_ID:
        return grid_pmc_mean(model.min_value, model.end_index+1);
    case SWING_ID:
        return gridSwing(model.min_value, model.max_value, model.values[0], timestamps, model.end_index+1);
    case GORILLA_ID:
        return grid_gorilla(model.values, model.values_capacity, model.end_index+1);
    case POLYSWING_ID:
        return grid_polyswing(model.min_value, model.max_value, model.values, timestamps, model.end_index+1);
    default:
        printf("Invalid ID in getReconstructedValues");
        break;
    }
    
}

double getRMSE(float* baseValues, float* reconstructedValues, int values_count){
    double error = 0;
    float baseValue = 0;
    float reconstructedValue = 0;
    for(int i = 0; i < values_count; i++){
        baseValue = baseValues[i];
        reconstructedValue = reconstructedValues[i];
        error += (baseValue - reconstructedValue) * (baseValue - reconstructedValue);
    }
    return sqrt(error/values_count);
}