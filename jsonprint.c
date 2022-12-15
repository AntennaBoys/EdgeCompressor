#include "jsonprint.h"
#include "paths.h"


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

void writeModelToFile(FILE* file, Timestamps timestamps, Selected_model model, int* first, int start_time, int end_time, double error, int column_id){

    fprintf(file,"%d,", model.model_type_id);                   //Model id
    fprintf(file,"%x,", column_id);                             //Column id
    fprintf(file,"%x,", (int)model.end_index);                  //End index
    fprintf(file,"%x,", model.min_value);                       //Min value
    fprintf(file,"%x,", model.max_value);                       //Max value
    fprintf(file,"%x,", error);                                //Model error
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
    fprintf(file,"5,");                                         //Model id
    fprintf(file,"%x,", column_id);                             //Column id
    fprintf(file,"%x,", count-1);                               //End index
    fprintf(file,"%x,", count);                                 //Min value (for text this is the running length of the string)
    fprintf(file,",");                                          //Max value (not used for text)
    fprintf(file,"0,");                                         //Error (lossless compression, so always 0)
    fprintf(file,"%x,", string);                              //Values (the string)
    fprintf(file,"%x,", start_time);                            //First timestamp
    fprintf(file,"%x,", end_time);                              //Last timestamp
    for (int i = 0; i < timestamps.compressed_timestamp_count; i++){
        fprintf(file, "%02x", timestamps.compressed_time[i]);   //Compressed timestamps
    }
    fprintf(file,"\n");
}
