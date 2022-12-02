#include "jsonprint.h"
#include "paths.h"


FILE* openFile(char* fileName){
    char* full_path = calloc((strlen(outPutCsvFile) + strlen(fileName)+1), sizeof(char));
    strcpy(full_path, outPutCsvFile);
    strcat(full_path, fileName);

    FILE* file = fopen(full_path, "w+");
    //fprintf(file,"{\"models\":[\n");
    free(full_path);
    return file;
}

void closeFile(FILE* file){
    //fprintf(file,"]}\n");
    fclose(file);
}

void writeModelToFile(FILE* file, Timestamps timestamps, Selected_model model, int* first, int start_time, int end_time, double error, int column_id){

    fprintf(file,"%d,", model.model_type_id);                   //Model id
    fprintf(file,"%d,", column_id);                             //Column id
    fprintf(file,"%d,", (int)model.end_index);                  //End index
    fprintf(file,"%f,", model.min_value);                       //Min value
    fprintf(file,"%f,", model.max_value);                       //Max value
    fprintf(file,"%lf,", error);                                //Model error
    for (int i = 0; i < model.values_capacity; i++){
        fprintf(file, "%02x", model.values[i]);                 //Values (use of this array differs from model to model)
    }
    fprintf(file,",");
    fprintf(file,"%d,", start_time);                            //First timestamp
    fprintf(file,"%d,", end_time);                              //Last timestamp
    for (int i = 0; i < timestamps.compressed_timestamp_count; i++) {
        fprintf(file, "%02x", timestamps.compressed_time[i]);   //Compressed timestamps
    }
    fprintf(file,"\n");
}

void write_text_to_file(FILE* file, int* first, int column_id, int count, char* string, long start_time, long end_time, Timestamps timestamps){
    fprintf(file,"5,");                                         //Model id
    fprintf(file,"%d,", column_id);                             //Column id
    fprintf(file,"%d,", count-1);                               //End index
    fprintf(file,"%d,", count);                                 //Min value (for text this is the running length of the string)
    fprintf(file,",");                                          //Max value (not used for text)
    fprintf(file,"0,");                                         //Error (lossless compression, so always 0)
    fprintf(file,"%x,", string);                                //Values (the string)
    fprintf(file,"%d,", start_time);                            //First timestamp
    fprintf(file,"%d,", end_time);                              //Last timestamp
    for (int i = 0; i < timestamps.compressed_timestamp_count; i++){
        fprintf(file, "%02x", timestamps.compressed_time[i]);   //Compressed timestamps
    }
    fprintf(file,"\n");
}

/*
//backup
void writeModelToFile(FILE* file, Timestamps timestamps, Selected_model model, int* first, int start_time, int end_time, double error, int column_id){
    if (!*first){
        fprintf(file,"  ,{\n");
    }else{
        fprintf(file,"{\n");
        *first = 0;
    }
    fprintf(file,"   \"Mid\":%d,\n", model.model_type_id);
    fprintf(file,"   \"Cid\":%d,\n", column_id);
    fprintf(file,"   \"end_index\":%d,\n", (int)model.end_index);
    fprintf(file,"   \"min_value\":%f,\n", model.min_value);
    fprintf(file,"   \"max_value\":%f,\n", model.max_value);
    fprintf(file,"   \"error\":%lf,\n", error);
    fprintf(file,"   \"values\":\"");
    int firstInArray = 1;
    for (int i = 0; i < model.values_capacity; i++){
        if(!firstInArray){
            fprintf(file, ",%d", model.values[i]);
        }else{
            fprintf(file, "%d", model.values[i]);
            firstInArray = 0;
        }
    }
    fprintf(file,"\",\n");
    fprintf(file,"   \"start_time\":%d,\n", start_time);
    fprintf(file,"   \"end_time\":%d,\n", end_time);
    fprintf(file,"   \"compressed_timestamps\":\"");
    firstInArray = 1;
    for (int i = 0; i < timestamps.compressed_timestamp_count; i++){
        if(!firstInArray){
            fprintf(file, ",%d", timestamps.compressed_time[i]);
        }else{
            fprintf(file, "%d", timestamps.compressed_time[i]);
            firstInArray = 0;
        }
    }
    fprintf(file,"\"\n");
    fprintf(file,"  }\n");
}

void write_text_to_file(FILE* file, int* first, int column_id, int count, char* string, long start_time, long end_time, Timestamps timestamps){
    if (!*first){
        fprintf(file,"  ,{\n");
    }else{
        fprintf(file,"{\n");
        *first = 0;
    }
    fprintf(file,"   \"Mid\":%d,\n", 5);
    fprintf(file,"   \"Cid\":%d,\n", column_id);
    fprintf(file,"   \"text\":%s,\n", string);
    fprintf(file,"   \"count\":%d,\n", count);
    fprintf(file,"   \"start_time\":%d,\n", start_time);
    fprintf(file,"   \"end_time\":%d,\n", end_time);
    fprintf(file,"   \"compressed_timestamps\":\"");
    int firstInArray = 1;
    for (int i = 0; i < timestamps.compressed_timestamp_count; i++){
        if(!firstInArray){
            fprintf(file, ",%d", timestamps.compressed_time[i]);
        }else{
            fprintf(file, "%d", timestamps.compressed_time[i]);
            firstInArray = 0;
        }
    }
    fprintf(file,"\"\n");
    fprintf(file,"  }\n");
}
*/