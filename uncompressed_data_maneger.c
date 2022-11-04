#include "uncompressed_data_maneger.h"
#include "jsonprint.h"
#include "compression.h"

#define BUFFER_SIZE 1024
#define ERROR_BOUND 0.002

void resize(UncompressedData* data);

UncompressedData createUncompressedDataManeger(char* filePath){
    UncompressedData data;
    data.maxSize = 1;
    data.currentSize = 0;
    data.output = openFile(filePath);
    data.timestamps = malloc(data.maxSize * sizeof(*data.timestamps));
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
    data->timestamps[data->currentSize-1] = timestamp;
    data->values[data->currentSize-1] = value;
    if(data->currentSize >= BUFFER_SIZE){
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
