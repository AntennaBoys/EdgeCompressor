#include "mod.h"
#include "uncompressed_data_maneger.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "paths.h"
#include <math.h>
#include "jsonprint.h"
#include <limits.h>


#define ERROR_BOUND 0.009
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
    UncompressedData latData = createUncompressedDataManeger(outPutCsvFileLat);
    UncompressedData longData = createUncompressedDataManeger(outPutCsvFileLong);

    

    FILE* stream = fopen(dataPath, "r");
    char line[1024];

    int longFirst = 1;
    int latFirst = 1;
    long timestamp = 0;
    struct tm tmVar;
    while(fgets(line, 1024, stream)){
        char* latStr = _strdup(line);
        char* longStr = _strdup(line);
        char* ts = _strdup(line);
        char* errorPointer;
        const char* timestampTemp = getfield(ts, 2);

        //01/09/2022 00:00:0
        if(sscanf(timestampTemp, "%d/%d/%d %d:%d:%d", &tmVar.tm_mday, &tmVar.tm_mon, &tmVar.tm_year, &tmVar.tm_hour, &tmVar.tm_min, &tmVar.tm_sec)==6){
            tmVar.tm_year -= 1900;
            tmVar.tm_mon -= 1;
            tmVar.tm_isdst = 1;
            long time = mktime(&tmVar)+3600;
            timestamp = time == timestamp ? time + 1 : time;
            insertData(&latData, timestamp, strtof(getfield(latStr, 5), &errorPointer), &latFirst);
            insertData(&longData, timestamp, strtof(getfield(longStr, 6), &errorPointer), &longFirst);
                
            

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
    if(latData.currentSize > 0){
        forceCompressData(&latData, latFirst);
    }
    if(longData.currentSize > 0){
        forceCompressData(&longData, longFirst);
    }
    deleteUncompressedDataManeger(&latData);
    deleteUncompressedDataManeger(&longData);
    closeFile(stream);
}


















