#include "compression.h"
#include "jsonprint.h"
#include <math.h>
#include <stdio.h>

#define GORILLAMAX 50

void deleteCompressedSegementBuilder(CompressedSegmentBuilder* builder);
int canFitMore(CompressedSegmentBuilder builder);
void tryToUpdateModels(CompressedSegmentBuilder* builder, long timestamp, float value);
float* getReconstructedValues(struct SelectedModel model, long* timestamps);
double getRMSE(float* baseValues, float* reconstructedValues, int valuesCount);

CompressedSegmentBuilder newCompressedSegmentBuilder(size_t startIndex, long* uncompressedTimestamps, float* uncompressedValues, size_t endIndex, double errorBound){
    CompressedSegmentBuilder builder;
    builder.pmc_mean_could_fit_all = 1;
    builder.swing_could_fit_all = 1;
    builder.polyswing_could_fit_all = 1;
    builder.start_index = startIndex;
    builder.pmcmean = getPMCMean(errorBound);
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

int finishBatch(CompressedSegmentBuilder builder, FILE* file, int first){
    struct SelectedModel model = getSelectedModel();
    selectModel(&model, builder.start_index, &builder.pmcmean, &builder.swing, &builder.gorilla, &builder.polyswing, builder.uncompressed_values);

    int startTime = builder.uncompressed_timestamps[builder.start_index];
    int endTime = builder.uncompressed_timestamps[model.end_index];

    float* reconstructedValues = getReconstructedValues(model, builder.uncompressed_timestamps);
    double error = getRMSE(builder.uncompressed_values, reconstructedValues, model.end_index+1);
    
    
    writeModelToFile(file, model ,first, startTime, endTime, error);
    deleteGorilla(&builder.gorilla);
    deletePolySwing(&builder.polyswing);
    deleteSelectedModel(&model);
    return model.end_index + 1;
}

int canFitMore(CompressedSegmentBuilder builder){
    return builder.pmc_mean_could_fit_all 
        || builder.swing_could_fit_all 
        || builder.gorilla.length < GORILLAMAX
        || builder.polyswing_could_fit_all;
}

void tryToUpdateModels(CompressedSegmentBuilder* builder, long timestamp, float value){
    if(builder->pmc_mean_could_fit_all){
        builder->pmc_mean_could_fit_all = fitValuePMC(&builder->pmcmean, value);
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

void tryCompress(UncompressedData* data, double errorBound, int* first){
    size_t currentIndex = 0;
    currentIndex = finishBatch(data->segmentBuilder, data->output, *first);
    *first = 0;
    for (int i = 0; i+currentIndex < data->currentSize; i++){
        data->values[i] = data->values[i+currentIndex];
        data->timestamps[i] = data->timestamps[i+currentIndex];
    }
    data->currentSize = data->currentSize - currentIndex;
    resizeUncompressedData(data);
    data->newBuilder = 1;
    currentIndex = 0;
}

void forceCompress(UncompressedData* data, double errorBound, int first){
    int isFirst = first;
    CompressedSegmentBuilder builder = data->segmentBuilder;
    size_t currentIndex = 0;
    while(currentIndex < data->currentSize){
        currentIndex = finishBatch(builder, data->output, isFirst);
        isFirst = 0;
        for (int i = 0; i+currentIndex < data->currentSize; i++){
            data->values[i] = data->values[i+currentIndex];
            data->timestamps[i] = data->timestamps[i+currentIndex];
        }
        data->currentSize = data->currentSize - currentIndex;
        resizeUncompressedData(data);
        currentIndex = 0;
        if(currentIndex != data->currentSize){
            builder = newCompressedSegmentBuilder(currentIndex, data->timestamps, data->values, data->currentSize, errorBound);
        }
    }
    //finishBatch(data->segmentBuilder, data->output, first);
}

float* getReconstructedValues(struct SelectedModel model, long* timestamps){
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