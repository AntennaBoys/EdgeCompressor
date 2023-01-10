#include "jsonprint.h"
#include "paths.h"
#include "global_error.h"


FILE* openFile(char* fileName){
    char* full_path = calloc((strlen(outPutCsvFile) + strlen(fileName)+1), sizeof(char));
    strcpy(full_path, outPutCsvFile);
    strcat(full_path, fileName);

    FILE* file = fopen(full_path, "w+");
    free(full_path);
    return file;
}

void closeFile(FILE* file){
    fclose(file);
}

int float_to_int(float val){
    return *(int *)&val;
}

void dump(FILE* file, const void *data, size_t len) {
    const unsigned char *x = data;
    for (size_t k = len-1;k < len; k--){
        fprintf(file, "%02x", x[k]);
    }
    fprintf(file,",");
}

void writeModelToFile(FILE* file, Timestamps timestamps, Selected_model model, int* first, int start_time, int end_time, float error, int column_id){
    if(model.model_type_id != 4){
        total_value_count_with_lossless+=model.end_index+1;
    }
    fprintf(file,"%d,", model.model_type_id);                   //Model id
    fprintf(file,"%d,", column_id);                             //Column id
    fprintf(file,"%d,", (int)model.end_index);                  //End index
    dump(file, &model.min_value, sizeof(float));                //Min value
    if(model.model_type_id == 0){//pmc mean
        fprintf(file,",");
    }else{
        dump(file, &model.max_value, sizeof(float));            //Max value
    }

    dump(file, &error, sizeof(float));                         //Model error
    for (int i = 0; i < model.values_capacity; i++){
        fprintf(file, "%02x", model.values[i]);                 //Values (use of this array differs from model to model)
    }
    fprintf(file,",");
    fprintf(file,"%x,", start_time);                            //First timestamp
    fprintf(file,"%x,", end_time);                              //Last timestamp
    for (int i = 0; i < timestamps.compressed_timestamp_count; i++) {
        fprintf(file, "%02x", timestamps.compressed_time[i]);   //Compressed timestamps
    }
    fprintf(file,"\n");
}

void write_text_to_file(FILE* file, int* first, int column_id, int count, char* string, long start_time, long end_time, Timestamps timestamps){
    total_value_count_with_lossless+=count;
    fprintf(file,"5,");                                         //Model id
    fprintf(file,"%x,", column_id);                             //Column id
    fprintf(file,"%x,", count-1);                               //End index
    fprintf(file,"%x,", count);                                 //Min value (for text this is the running length of the string)
    fprintf(file,",");                                          //Max value (not used for text)
    fprintf(file,"0,");                                         //Error (lossless compression, so always 0)
    fprintf(file,"%s,", string);                              //Values (the string)
    fprintf(file,"%x,", start_time);                            //First timestamp
    fprintf(file,"%x,", end_time);                              //Last timestamp
    for (int i = 0; i < timestamps.compressed_timestamp_count; i++){
        fprintf(file, "%02x", timestamps.compressed_time[i]);   //Compressed timestamps
    }
    fprintf(file,"\n");
}
