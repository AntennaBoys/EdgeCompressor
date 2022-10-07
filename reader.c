#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "paths.h"
#include "math.h"
#include "swing.h"

int RESET_BOTH = 0;
double ERROR_BOUND = 0.022;

void resetStruct(struct swing *data);
struct swing getStruct(double errorBound);
void writeToFile(FILE *file, struct swing model, int index, char* start);
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
                 
    struct swing dataLat = getStruct(ERROR_BOUND);
    struct swing dataLong = getStruct(ERROR_BOUND);
    int index = 0;

    FILE* stream = fopen(dataPath, "r");
    char line[1024];
    FILE *latfpt;
    FILE *longfpt;

    latfpt = fopen(outPutCsvFileLat, "w+");
    longfpt = fopen(outPutCsvFileLong, "w+");
    fprintf(latfpt,"{\"models\":[\n");
    fprintf(longfpt,"{\"models\":[\n");
    int latiCount = 0;
    int longCount = 0;
    char* longFirst = "";
    char* latFirst = "";
    while (fgets(line, 1024, stream))
    {
        char* lat = strdup(line);
        char* longs = strdup(line);
        char* ts = strdup(line);
        char* errorPointer;
        double latVal = strtod(getfield(lat, 5), &errorPointer);
        double longVal = strtod(getfield(longs, 6), &errorPointer);
        char* timestampTemp = getfield(ts, 2);

        struct tm tmVar;
        time_t timeVar;
        //01/09/2022 00:00:0
        if(sscanf(timestampTemp, "%d/%d/%d %d:%d:%d", &tmVar.tm_mday, &tmVar.tm_mon, &tmVar.tm_year, &tmVar.tm_hour, &tmVar.tm_min, &tmVar.tm_sec)==6){
            tmVar.tm_year -= 1900;
            tmVar.tm_mon -= 1;
            timeVar = mktime(&tmVar)+3600;
        }
        else
            continue;
            
        int resLat = fitValues(&dataLat, (long)timeVar, latVal);
        int resLong = fitValues(&dataLong, (long)timeVar, longVal);
        // if ((resLat && !RESET_BOTH) || (resLat && resLong)){
        //     //printf("%ld", (long)timeVar);
        // }
        if(!resLat || RESET_BOTH && !resLong){ // is 
            latiCount++;
            writeToFile(latfpt, dataLat, index, latFirst); //print to file
            latFirst = ",";
            printf("%ld\n", (long)timeVar);
            resetStruct(&dataLat);
        }
        // if ((resLong && !RESET_BOTH) || (resLat && resLong)){ 
        //     //printf("%ld", (long)timeVar);
        // }
        if(!resLong || RESET_BOTH && !resLat){
            longCount++;
            writeToFile(longfpt, dataLong, index, longFirst);
            longFirst = ",";
            printf("%ld\n", (long)timeVar);
            resetStruct(&dataLong);
        }
        if(!resLat || !resLong) { 
            printf("%d\n",index);
        }
        index++;
        // NOTE strtok clobbers tmp
        free(lat);
        free(longs);
        free(ts);
    }
    printf("results:\nlatitude = %d\nlongitude = %d\n", latiCount, longCount);
    fprintf(latfpt,"]}\n");
    fprintf(longfpt,"]}\n");
    fclose(latfpt);
    fclose(longfpt);
    fclose(stream);
}

void resetStruct(struct swing *data){
    data->first_timestamp = 0;
    data->last_timestamp = 0;
    data->first_value = NAN;
    data->upper_bound_slope = NAN;
    data->upper_bound_intercept = NAN;
    data->lower_bound_slope = NAN;
    data->lower_bound_intercept = NAN;
    data->length = 0;
}

struct swing getStruct(double errorBound){
    struct swing data;
    data.error_bound = 0.022;
    data.first_timestamp = 0;
    data.last_timestamp = 0;
    data.first_value = NAN;
    data.upper_bound_slope = NAN;
    data.upper_bound_intercept = NAN;
    data.lower_bound_slope = NAN;
    data.lower_bound_intercept = NAN;
    data.length = 0;
    return data;
}

void writeToFile(FILE *file, struct swing model, int index, char* start){
    double first_value = getModelFirst(model);
    double last_value = getModelLast(model);

    fprintf(file,"  %s{\n", start);
    fprintf(file,"   \"end_index\":%d,\n", index);
    fprintf(file,"   \"min_value\":%lf,\n", first_value < last_value ? first_value : last_value);
    fprintf(file,"   \"max_value\":%lf,\n", first_value >= last_value ? first_value : last_value);
    fprintf(file,"   \"values\":%d,\n", (first_value < last_value));
    fprintf(file,"   \"start_time\":%ld,\n", model.first_timestamp);
    fprintf(file,"   \"end_time\":%ld\n", model.last_timestamp);
    fprintf(file,"  }\n");

    
}