#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "math.h"
#include "swing.h"


void resetStruct(struct swing *data);
struct swing getStruct(double errorBound);

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
    struct swing dataLat = getStruct(0.022);
    struct swing dataLong = getStruct(0.022);
    int i = 0;

    FILE* stream = fopen("C:\\Users\\power\\CLionProjects\\compressexec\\out.csv", "r");
    char line[1024];
    FILE *fpt;

    fpt = fopen("D:\\stocks\\repos\\getdata\\lines.csv", "w+");
    fprintf(fpt,"0\n");
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
        if (resLat && resLong){
            i+=1;
            //printf("%ld", (long)timeVar);
        }
        else {
            resetStruct(&dataLat);
            resetStruct(&dataLong);
            printf("%d\n",i);
            fprintf(fpt,"%d\n", i);
            i += 1;
            printf("%ld\n", (long)timeVar);
        }

        // NOTE strtok clobbers tmp
        free(lat);
        free(longs);
        free(ts);
    }
    fclose(fpt);
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
