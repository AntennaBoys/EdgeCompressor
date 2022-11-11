#include "uncompressed_data_maneger.h"
#include "compression.h"
#define ERROR_BOUND 0.0005

void resize(UncompressedData* data);

UncompressedData createUncompressedDataManeger(char* filePath){
    UncompressedData data;
    data.maxSize = 1;
    data.currentSize = 0;
    data.output = openFile(filePath);
    data.timestamps = malloc(data.maxSize * sizeof(*data.timestamps));
    data.newBuilder = 1;
    if(!data.timestamps){
        printf("CALLOC ERROR(createUncompressedDataManeger->data.timestamps)\n");
    }
    data.values = malloc(data.maxSize * sizeof(*data.values));
    if(!data.values){
        printf("CALLOC ERROR(createUncompressedDataManeger->data.values)\n");
    }
    return data;
}

void resize(UncompressedData* data){
    data->timestamps = realloc(data->timestamps, data->maxSize * sizeof(*data->timestamps));
    if(!data->timestamps){
        printf("REALLOC ERROR(resize->data.timestamps)\n");
    }
    data->values = realloc(data->values, data->maxSize * sizeof(*data->values));
    if(!data->values){
        printf("REALLOC ERROR(resize->data.values)\n");
    }
}

void resizeUncompressedData(UncompressedData* data){
    if(data->currentSize < data->maxSize/2){
        data->maxSize = data->maxSize/2;
        resize(data);
    }
    if(data->currentSize >= data->maxSize){
        data->maxSize = data->maxSize *2;
        resize(data);
    }
}

void insertData(UncompressedData* data, long timestamp, float value, int* first){
    data->currentSize++;
    resizeUncompressedData(data);
    if(!data->newBuilder){
        data->segmentBuilder.uncompressed_timestamps = data->timestamps;
        data->segmentBuilder.uncompressed_values = data->values;
    }
    data->timestamps[data->currentSize-1] = timestamp;
    data->values[data->currentSize-1] = value;
    if(data->newBuilder){
        data->segmentBuilder = newCompressedSegmentBuilder(0, data->timestamps, data->values, data->currentSize, ERROR_BOUND);
        data->newBuilder = 0;
    }else{
        tryToUpdateModels(&data->segmentBuilder, timestamp, value);
    }
    if(!canFitMore(data->segmentBuilder)){
        tryCompress(data, ERROR_BOUND, first);
    }
}

void forceCompressData(UncompressedData* data, int first){
    forceCompress(data, ERROR_BOUND, first);
}

void deleteUncompressedDataManeger(UncompressedData* data){
    free(data->timestamps);
    free(data->values);
    closeFile(data->output);
}
