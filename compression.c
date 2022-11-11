#include "compression.h"
#include "jsonprint.h"
#include <math.h>
#include <stdio.h>

#define GORILLAMAX 50

void deleteCompressedSegementBuilder(Compressed_segment_builder* builder);
int canFitMore(Compressed_segment_builder builder);
void tryToUpdateModels(Compressed_segment_builder* builder, long timestamp, float value);
float* getReconstructedValues(Selected_model model, long* timestamps);
double getRMSE(float* baseValues, float* reconstructedValues, int valuesCount);

Compressed_segment_builder newCompressedSegmentBuilder(size_t startIndex, long* uncompressedTimestamps, float* uncompressedValues, size_t endIndex, double errorBound){
    Compressed_segment_builder builder;
    builder.pmc_mean_could_fit_all = 1;
    builder.swing_could_fit_all = 1;
    builder.polyswing_could_fit_all = 1;
    builder.start_index = startIndex;
    builder.pmc_mean = getPMCMean(errorBound);
    builder.swing = getSwing(errorBound);
    builder.gorilla = getGorilla();
    builder.polyswing = getPolySwing(errorBound);
    builder.uncompressed_timestamps = uncompressedTimestamps;
    builder.uncompressed_values = uncompressedValues;
    
    size_t currentIndex = startIndex;
    while (canFitMore(builder) && currentIndex < endIndex)
    {
        long timestamp = uncompressedTimestamps[currentIndex];
        float value = uncompressedValues[currentIndex];
        tryToUpdateModels(&builder, timestamp, value);
        currentIndex++;
    }
    return builder;
}

int finishBatch(Compressed_segment_builder builder, FILE* file, int first){
    Selected_model model = get_selected_model();
    select_model(&model, builder.start_index, &builder.pmc_mean, &builder.swing, &builder.gorilla, &builder.polyswing, builder.uncompressed_values);

    int startTime = builder.uncompressed_timestamps[builder.start_index];
    int endTime = builder.uncompressed_timestamps[model.end_index];

    float* reconstructedValues = getReconstructedValues(model, builder.uncompressed_timestamps);
    double error = getRMSE(builder.uncompressed_values, reconstructedValues, model.end_index+1);
    
    
    writeModelToFile(file, model ,first, startTime, endTime, error);
    deleteGorilla(&builder.gorilla);
    deletePolySwing(&builder.polyswing);
    delete_selected_model(&model);
    return model.end_index + 1;
}

int canFitMore(Compressed_segment_builder builder){
    return builder.pmc_mean_could_fit_all 
        || builder.swing_could_fit_all 
        || builder.gorilla.length < GORILLAMAX
        || builder.polyswing_could_fit_all;
}

void tryToUpdateModels(Compressed_segment_builder* builder, long timestamp, float value){
    if(builder->pmc_mean_could_fit_all){
        builder->pmc_mean_could_fit_all = fitValuePMC(&builder->pmc_mean, value);
    }
    if(builder->swing_could_fit_all){
        builder->swing_could_fit_all = fitValueSwing(&builder->swing, timestamp, value);
    }
    if(builder->gorilla.length < GORILLAMAX){
        fitValueGorilla(&builder->gorilla, value);
    }
    if(builder->polyswing_could_fit_all){
        builder->polyswing_could_fit_all = fitValuesPolySwing(&builder->polyswing, timestamp, value);// && builder->polyswing.length < 4;
    }
}

void tryCompress(Uncompressed_data* data, double errorBound, int* first){
    size_t currentIndex = 0;
    currentIndex = finishBatch(data->segment_builder, data->output, *first);
    *first = 0;
    for (int i = 0; i+currentIndex < data->current_size; i++){
        data->values[i] = data->values[i+currentIndex];
        data->timestamps[i] = data->timestamps[i+currentIndex];
    }
    data->current_size = data->current_size - currentIndex;
    resize_uncompressed_data(data);
    data->new_builder = 1;
    currentIndex = 0;
}

void forceCompress(Uncompressed_data* data, double errorBound, int first){
    int isFirst = first;
    Compressed_segment_builder builder = data->segment_builder;
    size_t currentIndex = 0;
    while(currentIndex < data->current_size){
        currentIndex = finishBatch(builder, data->output, isFirst);
        isFirst = 0;
        for (int i = 0; i+currentIndex < data->current_size; i++){
            data->values[i] = data->values[i+currentIndex];
            data->timestamps[i] = data->timestamps[i+currentIndex];
        }
        data->current_size = data->current_size - currentIndex;
        resize_uncompressed_data(data);
        currentIndex = 0;
        if(currentIndex != data->current_size){
            builder = newCompressedSegmentBuilder(currentIndex, data->timestamps, data->values, data->current_size, errorBound);
        }
    }
    //finishBatch(data->segment_builder, data->output, first);
}

float* getReconstructedValues(Selected_model model, long* timestamps){
    switch (model.model_type_id)
    {
    case PMC_MEAN_ID:
        return gridPMCMean(model.min_value, model.end_index+1);
    case SWING_ID:
        return gridSwing(model.min_value, model.max_value, model.values[0], timestamps, model.end_index+1);
    case GORILLA_ID:
        return gridGorilla(model.values, model.values_capacity, model.end_index+1);
    case POLYSWING_ID:
        return gridPolySwing(model.min_value, model.max_value, model.values, timestamps, model.end_index+1);
    default:
        printf("Invalid ID in getReconstructedValues");
        break;
    }
    
}

double getRMSE(float* baseValues, float* reconstructedValues, int valuesCount){
    double error = 0;
    float baseValue = 0;
    float reconstructedValue = 0;
    for(int i = 0; i < valuesCount; i++){
        baseValue = baseValues[i];
        reconstructedValue = reconstructedValues[i];
        error += (baseValue - reconstructedValue) * (baseValue - reconstructedValue);
    }
    return sqrt(error/valuesCount);
}