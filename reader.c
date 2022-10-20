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
#include "mod.h"

#define ERROR_BOUND 0.009
#define DATA_INTAKE_SIZE 50

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
                 
    struct Gorilla gorillaLat = init_gorilla();
    struct Gorilla gorillaLong = init_gorilla();
    struct PMCMean PMCLat = getPMC(ERROR_BOUND);
    struct PMCMean PMCLong = getPMC(ERROR_BOUND);
    struct swing swingLat = getStruct(ERROR_BOUND);
    struct swing swingLong = getStruct(ERROR_BOUND);
    struct SelectedModel selectedModelLat = init_selectedModel();
    struct SelectedModel selectedModelLong = init_selectedModel();
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

    double latVals[DATA_INTAKE_SIZE];
    int latValCount = 0;
    double longVals[DATA_INTAKE_SIZE];
    int longValCount = 0;
    struct tm tmVars[DATA_INTAKE_SIZE];
    int timeStampsCount = 0;
    int resetLong = 1;
    int resetLat = 1;
    while (fgets(line, 1024, stream))
    {
        if(timeStampsCount == DATA_INTAKE_SIZE){
            int dataTracker = 0;
            while(dataTracker < DATA_INTAKE_SIZE){
                // Reselect models if a data point did not fit in a segment
                //TODO: husk at flippe output fra fits
                if(resetLat){
                    selectModel(selectedModelLat, dataTracker, PMCLat, swingLat, gorillaLat, latVals)
                }
                if(resetLong){
                    selectModel(selectedModelLong, dataTracker, PMCLong, swingLong, gorillaLong, longVals)
                }
                switch ((int)selectedModelLat.model_type_id)
                {
                case PMC_MEAN_ID:
                    resetLat = !fitValuePMC(PMCLat, latVals[dataTracker])
                    break;
                case SWING_ID:

                    break;
                
                case GORILLA_ID:

                    break;
                default:
                    printf("SOMETHING WENT HORRIBLY WRONG");
                    exit(1)
                    break;
                }

                dataTracker++;
            }
            
            
        }
        char* lat = strdup(line);
        char* longs = strdup(line);
        char* ts = strdup(line);
        char* errorPointer;
        latVals[latValCount] = strtod(getfield(lat, 5), &errorPointer);
        longVals[longValCount] = strtod(getfield(longs, 6), &errorPointer);
        char* timestampTemp = getfield(ts, 2);

        
        //01/09/2022 00:00:0
        if(sscanf(timestampTemp, "%d/%d/%d %d:%d:%d", &tmVar.tm_mday, &tmVar.tm_mon, &tmVar.tm_year, &tmVar.tm_hour, &tmVar.tm_min, &tmVar.tm_sec)==6){
            tmVar.tm_year -= 1900;
            tmVar.tm_mon -= 1;
            tmVars[timeStampsCount++] = mktime(&tmVar)+3600;
            latValCount++;
            longValCount++;
        }
        else
            continue;
        
        
       
        //int resLat = fitValuesSwing(&dataLat, (long)timeVar, latVal, latMean.value);
        int resLat = fitValues(&dataLat, (long)timeVar, latVal, latVal);
        //int resLat = fitValuePMC(&dataLat, latVal);

        calculate_error_bound(longMean, longVal, 1);
        //int resLong = fitValuesSwing(&dataLong, (long)timeVar, longVal, longMean.value);
        int resLong = fitValues(&dataLong, (long)timeVar, longVal, longVal);
        //int resLong = fitValuePMC(&dataLong, longVal);

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
               //fitValuePMC(&dataLat, latVal);

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
               //fitValuePMC(&dataLong, longVal);
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
void resetGorilla(struct Gorilla* gorilla){
    gorilla->last_leading_zero_bits = UCHAR_MAX;
    gorilla->last_trailing_zero_bits = 0;
    gorilla->last_value = 0;
    gorilla->length = 0;
    
    gorilla->compressed_values.current_byte = 0;
    gorilla->compressed_values.remaining_bits = 8;
    gorilla->compressed_values.bytes_capacity = 1;
    gorilla->compressed_values.bytes = realloc(data->bytes, 4 * data->bytes_capacity * sizeof(uint8))
    if(data.compressed_values.bytes == NULL){
        printf("REALLOC ERROR\n");
    }
}

void resetSelectedModel(struct SelectedModel* model){
    model->model_type_id = 0;
    model->end_index = 0;
    model->min_value = 0;
    model->max_value = 0;
    model->values_capacity = 1;
    model->values = realloc(model->values, model->values_capacity * sizeof(uint8));
    if(mod.values == NULL){
        printf("REALLOC ERROR (mod)\n");
    }
}

struct SelectedModel init_selectedModel(){
    struct SelectedModel mod;
    mod.model_type_id = 0;
    mod.end_index = 0;
    mod.min_value = 0;
    mod.max_value = 0;
    mod.values_capacity = 1;
    mod.values = (uint8*) malloc (mod.values_capacity * sizeof(uint8));
    if(mod.values == NULL){
        printf("MALLOC ERROR (mod)\n");
    }
    return mod;
}





