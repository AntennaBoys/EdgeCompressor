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

void writeSwingToFile(FILE *file, Swing model, int index, int first){
    float first_value = getModelFirst(model);
    float last_value = getModelLast(model);

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

void writeGorillaToFile(FILE* file, Gorilla model, int index, int first){
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

void writePMCMeanToFile(FILE* file, Pmc_mean model, int index, int first){
    if (!first){
        fprintf(file,"  ,{\n");
    }else{
        fprintf(file,"{\n");
    }
    fprintf(file,"   \"Mid\":1,\n");
    fprintf(file,"   \"end_index\":%d,\n", index);
    fprintf(file,"   \"min_value\":NaN,\n");
    fprintf(file,"   \"max_value\":NaN,\n");
    fprintf(file,"   \"values\":[%f],\n", model.sum_of_values / model.length);
    fprintf(file,"   \"start_time\":NaN,\n");
    fprintf(file,"   \"end_time\":NaN\n");
    fprintf(file,"  }\n");
}

void writeModelToFile(FILE* file, Selected_model model, int first, int start_time, int end_time, double error){
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
    fprintf(file,"   \"end_time\":%d\n", end_time);
    fprintf(file,"  }\n");
}