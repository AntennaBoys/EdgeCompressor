#include "mod.h"
#include "uncompressed_data_maneger.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "paths.h"
#include <math.h>
#include "jsonprint.h"
#include <limits.h>
#include "vector_based.h"
#include "text_compressor.h"
#include "argument_handler.h"
#include "huffman.h"
#include "zipping.h"

#include <unistd.h>

#define ERROR_BOUND 0.1
#define INITIAL_BUFFER 200
#define GORILLA_MAX 50
#define VECTOR_TRUE 1
#define VECTOR_FALSE 0



const char *getfield(char *line, int num)
{
    const char *tok;
    for (tok = strtok(line, ",");
         tok && *tok;
         tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    paths_init();
    Arguments args = handleArguments(argc, argv);



    FILE* output_file = openFile("output.csv");
    Vector_based vb = get_vector_based();

    Uncompressed_data* dataList = calloc(args.numberOfCols, sizeof(Uncompressed_data));
    Text_compressor* text_compressors;
    if(args.number_of_text_cols > 0){
        text_compressors = calloc(args.number_of_text_cols, sizeof(*text_compressors));
    }

    FILE *stream = fopen(dataPath, "r");
    char line[1024];

    int first_print = 1;
    long timestamp = 0;
    struct tm tmVar;
    int cur_id = 0;
    if(args.containsPosition){
        cur_id++;
    }

    for(int i = 0; i < args.numberOfCols; i++){
        dataList[i] = create_uncompressed_data_maneger(output_file, cur_id++, &first_print);
    }
    for(int i = 0; i < args.number_of_text_cols; i++){
        text_compressors[i] = get_text_compressor(cur_id++);
    }

    while (fgets(line, 1024, stream))
    {
        char *latStr = strdup(line);
        char *longStr = strdup(line);
        char *ts = strdup(line);
        char *errorPointer;
        const char *timestampTemp = getfield(ts, args.timestampCol);
        long time;
        char* endptr;
        // 01/09/2022 00:00:0
        //if (sscanf(timestampTemp, "%d/%d/%d %d:%d:%d", &tmVar.tm_mday, &tmVar.tm_mon, &tmVar.tm_year, &tmVar.tm_hour, &tmVar.tm_min, &tmVar.tm_sec) == 6)
        time = strtol(timestampTemp, &endptr, 10);
        if(time != 0)
        {
            //tmVar.tm_year -= 1900;
            //tmVar.tm_mon -= 1;
            //tmVar.tm_isdst = 1;
            //long time = mktime(&tmVar) + 3600;
            timestamp = time == timestamp ? time + 1 : time;
            // insert_data(&latData, timestamp, strtof(getfield(latStr, 5), &errorPointer), &latFirst);
            // insert_data(&longData, timestamp, strtof(getfield(longStr, 6), &errorPointer), &longFirst);
            int latCol = args.latCol.col;
            int longCol = args.longCol.col;
            float error = args.latCol.error;

            // Compress position data if position columns are specified in input parameters
            if(args.containsPosition){
                insert_vector_based_data(output_file, &vb, timestamp, strtof(getfield(latStr, latCol), &errorPointer), strtof(getfield(longStr, longCol), &errorPointer), &first_print, error);
            }

            // Compress all other columns specified in input parameters
            for(int i = 0; i < args.numberOfCols; i++){
                
                char* str = strdup(line);
                int col = args.cols[i].col;

                insert_data(&dataList[i], timestamp, strtof(getfield(str, col), &errorPointer), dataList[i].first, args.cols[i].error, args.cols->isAbsolute);
                free(str);
              }
            for(int i = 0; i < args.number_of_text_cols; i++){
                char* str = strdup(line);
                int col = args.text_cols[i];
                char* field = getfield(str, col);
                if(!fit_string(&text_compressors[i], field, timestamp)){
                    print_compressed_text(&text_compressors[i], output_file, &first_print);
                    reset_text_compressor(&text_compressors[i], field, timestamp);
                }
            }
            // fit_values_vector_based(&vb, timestamp, strtof(getfield(latStr, 5), &errorPointer), strtof(getfield(longStr, 6), &errorPointer));



            free(longStr);
            free(latStr);
            free(ts);
        }
        else
        {
            free(latStr);
            free(longStr);
            free(ts);
            continue;
        }
    }
    // if(latData.current_size > 0){
    //     force_compress_data(&latData, latFirst);
    // }
    // if(longData.current_size > 0){
    //     force_compress_data(&longData, longFirst);
    // }
    // delete_uncompressed_data_maneger(&latData);
    // delete_uncompressed_data_maneger(&longData);
    for(int i = 0; i < args.numberOfCols; i++){
        if(dataList[i].current_size > 0)
            force_compress_data(&dataList[i], &first_print, args.cols[i].error);
        free(dataList[i].timestamps);
        free(dataList[i].values);
    }
    for(int i = 0; i < args.number_of_text_cols; i++){
        if(text_compressors[i].count > 0)
            print_compressed_text(&text_compressors[i], output_file, &first_print);
        free(text_compressors[i].timestamps);
    }


    if(args.containsPosition){
        print_vector_based(output_file, &vb, &first_print);
    }
    closeFile(output_file);

    free(args.cols);
    free(dataList);
    free_vectorbased(&vb);
    fclose(stream);
    char* uncompressed_file = calloc((strlen(outPutCsvFile) + strlen("output.csv")+1), sizeof(char));
    strcpy(uncompressed_file, outPutCsvFile);
    strcat(uncompressed_file, "output.csv");
    char* compressed_file = calloc((strlen(outPutCsvFile) + strlen("output_compressed.bin")+1), sizeof(char));
    strcpy(compressed_file, outPutCsvFile);
    strcat(compressed_file, "output_compressed.bin");
    begin_huffman_encoding(uncompressed_file, compressed_file);
}