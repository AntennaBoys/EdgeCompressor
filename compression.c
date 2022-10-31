#include "compression.h"
#include "jsonprint.h"
#include <stdio.h>

#define GORILLAMAX 50
CompressedSegmentBuilder newCompressedSegmentBuilder(int startIndex, int* uncompressedTimestamps, float* uncompressedValues, int endIndex, float errorBound){
    CompressedSegmentBuilder builder;
    builder.pmc_mean_could_fit_all = 1;
    builder.swing_could_fit_all = 1;
    builder.start_index = startIndex;
    builder.pmceman = getPMCMean(errorBound);
    builder.swing = getSwing(errorBound);
    builder.gorilla = getGorilla();
    builder.uncompressed_timestamps = calloc(endIndex, *builder.uncompressed_timestamps);
    if(!builder.uncompressed_timestamps){
        printf("CALLOC ERROR(newCompressedSegmentBuilder->uncompressedTimestamps)\n");
    }
    builder.uncompressed_values = calloc(endIndex, *builder.uncompressed_values);
    if(!builder.uncompressed_values){
        printf("CALLOC ERROR(newCompressedSegmentBuilder->uncompressed_values)\n");
    }
    
    int currentIndex = startIndex;
    while (canFitMore(builder) && currentIndex < endIndex)
    {
        int timestamp = uncompressedTimestamps[currentIndex];
        float value = uncompressedValues[currentIndex];
        tryToUpdateModels(&builder, timestamp, value);
        currentIndex++;
    }
    return builder;
}

int finishBatch(CompressedSegmentBuilder builder, FILE* file, int first){
    struct SelectedModel model;
    selectModel(&model, &builder.start_index, &builder.pmceman, &builder.swing, &builder.gorilla, builder.uncompressed_values);

    int startTime = builder.uncompressed_timestamps[builder.start_index];
    int endTime = builder.uncompressed_timestamps[model.end_index];

    writeModelToFile(file, model ,first, startTime, endTime);
    return model.end_index;
}

int canFitMore(CompressedSegmentBuilder builder){
    return builder.pmc_mean_could_fit_all 
        || builder.swing_could_fit_all 
        || builder.gorilla.length < GORILLAMAX;
}

tryToUpdateModels(CompressedSegmentBuilder* builder, int timestamp, float value){
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