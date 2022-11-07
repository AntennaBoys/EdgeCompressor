#include "compression.h"
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
    builder.start_index = startIndex;
    builder.pmceman = getPMCMean(errorBound);
    builder.swing = getSwing(errorBound);
    builder.gorilla = getGorilla();
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
    selectModel(&model, builder.start_index, &builder.pmceman, &builder.swing, &builder.gorilla, builder.uncompressed_values);

    int startTime = builder.uncompressed_timestamps[builder.start_index];
    int endTime = builder.uncompressed_timestamps[model.end_index];

    float* reconstructedValues = getReconstructedValues(model, builder.uncompressed_timestamps);
    double error = getRMSE(builder.uncompressed_values, reconstructedValues, model.end_index+1);
    
    
    writeModelToFile(file, model ,first, startTime, endTime, error);
    deleteGorilla(&builder.gorilla);
    deleteSelectedModel(&model);
    return model.end_index + 1;
}

int canFitMore(CompressedSegmentBuilder builder){
    return builder.pmc_mean_could_fit_all 
        || builder.swing_could_fit_all 
        || builder.gorilla.length < GORILLAMAX;
}

void tryToUpdateModels(CompressedSegmentBuilder* builder, long timestamp, float value){
    if(builder->pmc_mean_could_fit_all){
        builder->pmc_mean_could_fit_all = fitValuePMC(&builder->pmceman, value);
    }
    if(builder->swing_could_fit_all){
        builder->swing_could_fit_all = fitValueSwing(&builder->swing, timestamp, value);
    }
    if(builder->gorilla.length < GORILLAMAX){
        fitValueGorilla(&builder->gorilla, value);
    }
}

void tryCompress(UncompressedData* data, double errorBound, int* first){
    size_t currentIndex = 0;
    while(currentIndex < data->currentSize){
        CompressedSegmentBuilder builder = newCompressedSegmentBuilder(currentIndex, data->timestamps, data->values, data->currentSize, errorBound);
        if(!canFitMore(builder)){
            currentIndex = finishBatch(builder, data->output, *first);
            *first = 0;
            for (int i = 0; i+currentIndex < data->currentSize; i++){
                data->values[i] = data->values[i+currentIndex];
                data->timestamps[i] = data->timestamps[i+currentIndex];
            }
            data->currentSize = data->currentSize - currentIndex;
            resizeUncompressedData(data);
            currentIndex = 0;
        }else{
            currentIndex = data->currentSize;
        }
    }
}

void forceCompress(UncompressedData* data, double errorBound, int first){
    size_t currentIndex = 0;
    while(currentIndex < data->currentSize){
        CompressedSegmentBuilder builder = newCompressedSegmentBuilder(currentIndex, data->timestamps, data->values, data->currentSize, errorBound);
        currentIndex = finishBatch(builder, data->output, first);
        for (int i = 0; i+currentIndex < data->currentSize; i++){
            data->values[i] = data->values[i+currentIndex];
            data->timestamps[i] = data->timestamps[i+currentIndex];
        }
        data->currentSize = data->currentSize - currentIndex;
        resizeUncompressedData(data);
        currentIndex = 0;
    }
}

float* getReconstructedValues(struct SelectedModel model, long* timestamps){
    switch (model.model_type_id)
    {
    case PMC_MEAN_ID:
        return gridPMCMean(model, model.end_index+1);
    case SWING_ID:
        return gridSwing(model, timestamps, model.end_index+1);
    case GORILLA_ID:
        return gridGorilla(model.values, model.values_capacity, model.end_index+1);
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