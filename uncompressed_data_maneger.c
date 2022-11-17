#include "uncompressed_data_maneger.h"
#include "compression.h"
#define ERROR_BOUND 0.01

void resize(Uncompressed_data* data);

Uncompressed_data create_uncompressed_data_maneger(char* file_path){
    Uncompressed_data data;
    data.max_size = 1;
    data.current_size = 0;
    data.output = openFile(file_path);
    data.timestamps = malloc(data.max_size * sizeof(*data.timestamps));
    data.new_builder = 1;
    if(!data.timestamps){
        printf("CALLOC ERROR(create_uncompressed_data_maneger->data.timestamps)\n");
    }
    data.values = malloc(data.max_size * sizeof(*data.values));
    if(!data.values){
        printf("CALLOC ERROR(create_uncompressed_data_maneger->data.values)\n");
    }
    return data;
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

void insert_data(Uncompressed_data* data, long timestamp, float value, int* first){
    data->current_size++;
    resize_uncompressed_data(data);
    if(!data->new_builder){
        data->segment_builder.uncompressed_timestamps = data->timestamps;
        data->segment_builder.uncompressed_values = data->values;
    }
    data->timestamps[data->current_size-1] = timestamp;
    data->values[data->current_size-1] = value;
    if(data->new_builder){
        data->segment_builder = newCompressedSegmentBuilder(0, data->timestamps, data->values, data->current_size, ERROR_BOUND);
        data->new_builder = 0;
    }else{
        tryToUpdateModels(&data->segment_builder, timestamp, value);
    }
    if(!canFitMore(data->segment_builder)){
        tryCompress(data, ERROR_BOUND, first);
    }
}

void force_compress_data(Uncompressed_data* data, int first){
    forceCompress(data, ERROR_BOUND, first);
}

void delete_uncompressed_data_maneger(Uncompressed_data* data){
    free(data->timestamps);
    free(data->values);
    closeFile(data->output);
}
