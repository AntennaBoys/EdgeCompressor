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


#define ERROR_BOUND 0.1
#define INITIAL_BUFFER 200
#define GORILLA_MAX 50





const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ",");
         tok && *tok;
         tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

int main()
{
    Uncompressed_data latData = create_uncompressed_data_maneger(outPutCsvFileLat);
    Uncompressed_data longData = create_uncompressed_data_maneger(outPutCsvFileLong);

    Vector_based vb = getVector_based();
    

    FILE* stream = fopen(dataPath, "r");
    char line[1024];

    int longFirst = 1;
    int latFirst = 1;
    long timestamp = 0;
    struct tm tmVar;

    FILE *test1;
    test1 = fopen("/home/teis/git/EdgeCompressor/vector_plot.csv", "w+");
    fprintf(test1,"lat, long\n");
    fclose(test1);

    while(fgets(line, 1024, stream)){
        char* latStr = strdup(line);
        char* longStr = strdup(line);
        char* ts = strdup(line);
        char* errorPointer;
        const char* timestampTemp = getfield(ts, 2);

        //01/09/2022 00:00:0
        if(sscanf(timestampTemp, "%d/%d/%d %d:%d:%d", &tmVar.tm_mday, &tmVar.tm_mon, &tmVar.tm_year, &tmVar.tm_hour, &tmVar.tm_min, &tmVar.tm_sec)==6){
            tmVar.tm_year -= 1900;
            tmVar.tm_mon -= 1;
            tmVar.tm_isdst = 1;
            long time = mktime(&tmVar)+3600;
            timestamp = time == timestamp ? time + 1 : time;
            // insert_data(&latData, timestamp, strtof(getfield(latStr, 5), &errorPointer), &latFirst);
            // insert_data(&longData, timestamp, strtof(getfield(longStr, 6), &errorPointer), &longFirst);
            fitValuesVectorBased(&vb, timestamp, strtof(getfield(latStr, 5), &errorPointer), strtof(getfield(longStr, 6), &errorPointer));
            

            free(longStr);
            free(latStr);
            free(ts);
        }else {
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
    closeFile(stream);
}


















