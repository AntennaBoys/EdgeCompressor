#include "jsonprint.h"


FILE* openFile(char* fileName){
    FILE* file = fopen(fileName, "w+");
    fprintf(file,"{\"models\":[\n");
    return file;
}

void closeFile(FILE* file){
    fprintf(file,"]}\n");
    fclose(file);
}

void writeModelToFile(FILE* file, Timestamps timestamps, Selected_model model, int first, int start_time, int end_time, double error){
    if (!first){
        fprintf(file,"  ,{\n");
    }else{
        fprintf(file,"{\n");
    }
    fprintf(file,"   \"Mid\":%d,\n", model.model_type_id);
    fprintf(file,"   \"end_index\":%d,\n", (int)model.end_index);
    fprintf(file,"   \"min_value\":%f,\n", model.min_value);
    fprintf(file,"   \"max_value\":%f,\n", model.max_value);
    fprintf(file,"   \"error\":%lf,\n", error);
    fprintf(file,"   \"values\":[");
    int firstInArray = 1;
    for (int i = 0; i < model.values_capacity; i++){
        if(!firstInArray){
            fprintf(file, ",%d", model.values[i]);
        }else{
            fprintf(file, "%d", model.values[i]);
            firstInArray = 0;
        }
    }
    fprintf(file,"],\n");
    fprintf(file,"   \"start_time\":%d,\n", start_time);
    fprintf(file,"   \"end_time\":%d,\n", end_time);
    fprintf(file,"   \"compressed_timestamps\":[");
    firstInArray = 1;
    for (int i = 0; i < timestamps.compressed_timestamp_count; i++){
        if(!firstInArray){
            fprintf(file, ",%d", timestamps.compressed_time[i]);
        }else{
            fprintf(file, "%d", timestamps.compressed_time[i]);
            firstInArray = 0;
        }
    }
    fprintf(file,"]\n");
    fprintf(file,"  }\n");
}
