//
// Created by danie on 30-11-2022.
//

#include "text_compressor.h"
#include <string.h>
#include "timestamps.h"
Text_compressor get_text_compressor(int id){
    Text_compressor compressor;
    compressor.count = 0;
    compressor.max_timestamps = 64;
    compressor.timestamps = calloc(compressor.max_timestamps, sizeof(*compressor.timestamps));
    compressor.id = id;
    return compressor;

}


int fit_string(Text_compressor* data, char* string, long timestamp){
    if(data->count == 0){
        data->string = string;
        data->timestamps[data->count] = timestamp;
        data->count++;
        return 1;
    }else if(strcmp(data->string, string) == 0){ //strcmp returns 0 if they are equal
        data->timestamps[data->count] = timestamp;
        data->count++;
        if(data->count == data->max_timestamps){
            data->max_timestamps += 64;
            data->timestamps = realloc(data->timestamps, data->max_timestamps * sizeof(*data->timestamps));
        }
        return 1;
    }
    return 0;
}

void reset_text_compressor(Text_compressor* data, char* string, long timestamp){
    data->count = 1;
    data->max_timestamps = 64;
    free(data->timestamps);
    data->timestamps = calloc(data->max_timestamps, sizeof(*data->timestamps));
    data->timestamps[0] = timestamp;
    data->string = string;
}

void print_compressed_text(Text_compressor* data, FILE* file, int* first){
    write_text_to_file(file, first,data->id, data->count, data->string, data->timestamps[0], data->timestamps[data->count-1],
                       compress_residual_timestamps(data->timestamps, data->count));
}