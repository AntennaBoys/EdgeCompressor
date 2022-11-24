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

#include "argument_handler.h"

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



    FILE* position_file = openFile("position.json");
    Vector_based vb = get_vector_based();

    Uncompressed_data* dataList = malloc(args.numberOfCols * sizeof(Uncompressed_data));

    FILE *stream = fopen(dataPath, "r");
    char line[1024];

    int longFirst = 1;
    int latFirst = 1;
    int position_first = 1;
    long timestamp = 0;
    struct tm tmVar;



    for(int i = 0; i < args.numberOfCols; i++){
        char filename[10];
        snprintf(filename, sizeof(filename), "file_%d", args.cols[i].col);
        printf("filename: %s", filename);
        dataList[i] = create_uncompressed_data_maneger(filename);
    }

    while (fgets(line, 1024, stream))
    {
        char *latStr = strdup(line);
        char *longStr = strdup(line);
        char *ts = strdup(line);
        char *errorPointer;
        const char *timestampTemp = getfield(ts, args.timestampCol);

        // 01/09/2022 00:00:0
        if (sscanf(timestampTemp, "%d/%d/%d %d:%d:%d", &tmVar.tm_mday, &tmVar.tm_mon, &tmVar.tm_year, &tmVar.tm_hour, &tmVar.tm_min, &tmVar.tm_sec) == 6)
        {
            tmVar.tm_year -= 1900;
            tmVar.tm_mon -= 1;
            tmVar.tm_isdst = 1;
            long time = mktime(&tmVar) + 3600;
            timestamp = time == timestamp ? time + 1 : time;
            // insert_data(&latData, timestamp, strtof(getfield(latStr, 5), &errorPointer), &latFirst);
            // insert_data(&longData, timestamp, strtof(getfield(longStr, 6), &errorPointer), &longFirst);
            int latCol = args.latCol.col;
            int longCol = args.longCol.col;
            float error = args.latCol.error;

            // Compress position data if position columns are specified in input parameters
            if(args.containsPosition){
                insert_vector_based_data(position_file, &vb, timestamp, strtof(getfield(latStr, latCol), &errorPointer), strtof(getfield(longStr, longCol), &errorPointer), &position_first, error);
            }

            // Compress all other columns specified in input parameters
            for(int i = 0; i < args.numberOfCols; i++){
                
                char* str = strdup(line);
                int col = args.cols[i].col;
                insert_data(&dataList[i], timestamp, strtof(getfield(str, col), &errorPointer), &dataList[i].first, args.cols[i].error);
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
        force_compress_data(&dataList[i], dataList[i].first, args.cols[i].error);
        closeFile(dataList[i].output);
    }


    print_vector_based(position_file, &vb, &position_first);
    closeFile(position_file);
    fclose(stream);
}