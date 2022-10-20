#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "paths.h"
#include "math.h"
#include "swing.h"
#include "jsonprint.h"
#include "gorilla.h"
#include "error_bound_calculator.h"
#include "PMCMean.h"

int RESET_BOTH = 0;
double ERROR_BOUND = 0.009;

void resetStruct(struct swing *data);
struct swing getStruct(double errorBound);
struct PMCMean getPMC(double errorBound);
void resetPMC(struct PMCMean *pmc);
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
                 
    // struct Gorilla dataLat = init();
    // struct Gorilla dataLong = init();
    //struct PMCMean dataLat = getPMC(ERROR_BOUND);
    //struct PMCMean dataLong = getPMC(ERROR_BOUND);
    struct swing dataLat = getStruct(ERROR_BOUND);
    struct swing dataLong = getStruct(ERROR_BOUND);
    running_mean latMean = {0,0};
    running_mean longMean = {0,0};
    int index = 0;

    FILE* stream = fopen(dataPath, "r");
    char line[1024];
    FILE *latfpt = openFile(outPutCsvFileLat);
    FILE *longfpt = openFile(outPutCsvFileLong);

    int latiCount = 0;
    int longCount = 0;
    int longFirst = 1;
    int latFirst = 1;
    struct tm tmVar;
    time_t timeVar;
    while (fgets(line, 1024, stream))
    {
        char* lat = strdup(line);
        char* longs = strdup(line);
        char* ts = strdup(line);
        char* errorPointer;
        double latVal = strtod(getfield(lat, 5), &errorPointer);
        double longVal = strtod(getfield(longs, 6), &errorPointer);
        char* timestampTemp = getfield(ts, 2);

        
        //01/09/2022 00:00:0
        if(sscanf(timestampTemp, "%d/%d/%d %d:%d:%d", &tmVar.tm_mday, &tmVar.tm_mon, &tmVar.tm_year, &tmVar.tm_hour, &tmVar.tm_min, &tmVar.tm_sec)==6){
            tmVar.tm_year -= 1900;
            tmVar.tm_mon -= 1;
            timeVar = mktime(&tmVar)+3600;
        }
        else
            continue;
        
        if(latFirst != 0){
            // do nothing
        }else{
            latMean.lenght = 1;
            latMean.value = latVal;
        }
        if(longFirst != 0){
            // do nothing
        }else{
            longMean.lenght = 1;
            longMean.value = longVal;
        }
        calculate_error_bound(latMean, latVal, 1);
        //int resLat = fitValues(&dataLat, (long)timeVar, latVal, latMean.value);
        int resLat = fitValues(&dataLat, (long)timeVar, latVal, latVal);
        //int resLat = fitValue(&dataLat, latVal);

        calculate_error_bound(longMean, longVal, 1);
        //int resLong = fitValues(&dataLong, (long)timeVar, longVal, longMean.value);
        int resLong = fitValues(&dataLong, (long)timeVar, longVal, longVal);
        //int resLong = fitValue(&dataLong, longVal);

        // int resLong = 1;
        // int resLat = 1;
        // compress_value(&dataLat, latVal);
        // compress_value(&dataLong, longVal);


        if ((resLat && !RESET_BOTH) || (resLat && resLong)){
            //printf("%ld\n", (long)timeVar);
        }
        if(!resLat || RESET_BOTH && !resLong){ // is 
            latiCount++;
            // writeGorillaToFile(latfpt, dataLat, index, latFirst); //print to file
            writeSwingToFile(latfpt, dataLat, index, latFirst);
            //writePMCMeanToFile(latfpt, dataLat, index, latFirst);
            latFirst = 0;
            //printf("%ld\n", (long)timeVar);
            resetStruct(&dataLat);
            //resetPMC(&dataLat);
            if(!resLat)
               fitValues(&dataLat, (long)timeVar, latVal, latVal);
               //fitValue(&dataLat, latVal);

        }
        if ((resLong && !RESET_BOTH) || (resLat && resLong)){ 
            //printf("%ld\n", (long)timeVar);
        }
        if(!resLong || RESET_BOTH && !resLat){
            longCount++;
            //writeGorillaToFile(longfpt, dataLong, index, longFirst);
            writeSwingToFile(longfpt, dataLong, index, longFirst);
            //writePMCMeanToFile(longfpt, dataLong, index, longFirst);
            longFirst = 0;
            printf("%ld\n", (long)timeVar);
            resetStruct(&dataLong);
            //resetPMC(&dataLong);
            if(!resLong)
               fitValues(&dataLong, (long)timeVar, longVal, longVal);
               //fitValue(&dataLong, longVal);
        }
        if(!resLat || !resLong) { 
            //printf("%d\n",index);
        }
        index++;
        // NOTE strtok clobbers tmp
        free(lat);
        free(longs);
        free(ts);
    }
    longCount++;
    //writeGorillaToFile(longfpt, dataLong, index, longFirst);
    writeSwingToFile(longfpt, dataLong, index, longFirst);
    //writePMCMeanToFile(longfpt, dataLong, index, longFirst);
    latiCount++;
    //writeGorillaToFile(latfpt, dataLat, index, latFirst);
    writeSwingToFile(latfpt, dataLat, index, latFirst);
    //writePMCMeanToFile(latfpt, dataLat, index, latFirst);
    printf("results:\nlatitude = %d\nlongitude = %d\n", latiCount, longCount);
    closeFile(latfpt);
    closeFile(longfpt);
    closeFile(stream);
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
    data.error_bound = errorBound;
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

void resetPMC(struct PMCMean *pmc){
    pmc->minValue = NAN;
    pmc->maxValue = NAN;
    pmc->sumOfValues = 0;
    pmc->length = 0;
}

struct PMCMean getPMC(double errorBound){
    struct PMCMean data;
    data.error = errorBound;
    data.minValue = NAN;
    data.maxValue = NAN;
    data.sumOfValues = 0;
    data.length = 0;
    return data;
}



