#include "jsonprint.h"
#include <stdio.h>


FILE* openFile(char* fileName){
    FILE* file = fopen(fileName, "w+");
    fprintf(file,"{\"models\":[\n");
    return file;
}

void closeFile(FILE* file){
    fprintf(file,"]}\n");
    fclose(file);
}

void writeSwingToFile(FILE *file, struct swing model, int index, int first){
    double first_value = getModelFirst(model);
    double last_value = getModelLast(model);

    if (!first){
        fprintf(file,"  ,{\n");
    }else{
        fprintf(file,"{\n");
    }
    fprintf(file,"   \"Mid\":2,\n");
    fprintf(file,"   \"end_index\":%d,\n", index);
    fprintf(file,"   \"min_value\":%lf,\n", first_value < last_value ? first_value : last_value);
    fprintf(file,"   \"max_value\":%lf,\n", first_value >= last_value ? first_value : last_value);
    fprintf(file,"   \"values\":[%d],\n", (first_value < last_value));
    fprintf(file,"   \"start_time\":%ld,\n", model.first_timestamp);
    fprintf(file,"   \"end_time\":%ld\n", model.last_timestamp);
    fprintf(file,"  }\n");
}

void writeGorillaToFile(FILE* file, struct Gorilla model, int index, int first){
    if (!first){
        fprintf(file,"  ,{\n");
    }else{
        fprintf(file,"{\n");
    }
    fprintf(file,"   \"Mid\":3,\n");
    fprintf(file,"   \"end_index\":%d,\n", index);
    fprintf(file,"   \"min_value\":NaN,\n");
    fprintf(file,"   \"max_value\":NaN,\n");
    fprintf(file,"   \"values\":[");
    for (int i = 0; i < model.compressed_values.bytes_counter; i++){
        if (i != model.compressed_values.bytes_counter-1){
            fprintf(file, "%d, ", model.compressed_values.bytes[i]);
        }else{
            fprintf(file, "%d", model.compressed_values.bytes[i]);
        }
    }
    fprintf(file,"],\n");
    fprintf(file,"   \"start_time\":NaN,\n");
    fprintf(file,"   \"end_time\":NaN\n");
    fprintf(file,"  }\n");
    
}

void writePMCMeanToFile(FILE* file, struct PMCMean model, int index, int first){
    if (!first){
        fprintf(file,"  ,{\n");
    }else{
        fprintf(file,"{\n");
    }
    fprintf(file,"   \"Mid\":1,\n");
    fprintf(file,"   \"end_index\":%d,\n", index);
    fprintf(file,"   \"min_value\":NaN,\n");
    fprintf(file,"   \"max_value\":NaN,\n");
    fprintf(file,"   \"values\":[%d]", model.sumOfValues / model.length);
    fprintf(file,"   \"start_time\":NaN,\n");
    fprintf(file,"   \"end_time\":NaN\n");
    fprintf(file,"  }\n");
}