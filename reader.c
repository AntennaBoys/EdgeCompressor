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
#define INITIAL_BUFFER 500
#define GORILLA_MAX 50

void resetGorilla(struct Gorilla* gorilla);
void resetSelectedModel(struct SelectedModel* model);
void resetSwing(struct swing *data);
struct swing getSwing(double errorBound);
struct PMCMean getPMC(double errorBound);
struct SelectedModel init_selectedModel();
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
    struct swing swingLat = getSwing(ERROR_BOUND);
    struct swing swingLong = getSwing(ERROR_BOUND);
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

    float *latBuffer = (float*) calloc(INITIAL_BUFFER, sizeof(float));
    int latBufferCount = 0;
    int startLatIndex = 0;
    int currentLatIndex = 0;
    int maxLatBufferCount = INITIAL_BUFFER;
    int latPMCCanFitMore = 1;
    int latSwingCanFitMore = 1;
    int latGorillaCanFitMore = 1;

    float *longBuffer = (float*) calloc(INITIAL_BUFFER, sizeof(float));
    int longBufferCount = 0;
    int startLongIndex = 0;
    int currentLongIndex = 0;
    int maxLongBufferCount = INITIAL_BUFFER;
    int longPMCCanFitMore = 1;
    int longSwingCanFitMore = 1;
    int longGorillaCanFitMore = 1;

    long *timeBuffer = (long*) calloc(INITIAL_BUFFER, sizeof(long));
    int timeBufferCount = 0;
    int maxTimeBufferCount = INITIAL_BUFFER;

    int resetLong = 1;
    int resetLat = 1;

    int currentModel = 0; // start at PMCmean

    int yetEmittedLat = 0;
    int yetEmittedLong = 0;

    int endOfInput = 0;
    while(yetEmittedLat != 0 || yetEmittedLat != 0 || !endOfInput)
    {
        if(!endOfInput && (currentLatIndex == latBufferCount || currentLongIndex == longBufferCount))
        {
            if(fgets(line, 1024, stream)){
                char* latStr = strdup(line);
                char* longStr = strdup(line);
                char* ts = strdup(line);
                char* errorPointer;
                const char* timestampTemp = getfield(ts, 2);

                //01/09/2022 00:00:0
                if(sscanf(timestampTemp, "%d/%d/%d %d:%d:%d", &tmVar.tm_mday, &tmVar.tm_mon, &tmVar.tm_year, &tmVar.tm_hour, &tmVar.tm_min, &tmVar.tm_sec)==6){
                    tmVar.tm_year -= 1900;
                    tmVar.tm_mon -= 1;
                    //TODO: implement sudocode line 14-16, 18

                    // SudoC: line  17
                    timeBuffer[timeBufferCount++] = strtod(getfield(latStr, 5), &errorPointer);
                    latBuffer[latBufferCount++] = strtod(getfield(longStr, 6), &errorPointer);
                    longBuffer[longBufferCount++] = mktime(&tmVar)+3600;
                    // ensure we have enough space in the buffers
                    if(timeBufferCount + 1 == maxTimeBufferCount){
                        maxTimeBufferCount += 10;
                        timeBuffer = realloc(timeBuffer, maxTimeBufferCount * sizeof(*timeBuffer));
                        if(timeBuffer == NULL){
                            printf("REALLOC ERROR (timeBuffer)\n");
                        }
                    }
                    if(latBufferCount + 1 == maxLatBufferCount){
                        maxLatBufferCount += 10;
                        latBuffer = realloc(latBuffer, maxLatBufferCount);
                        if(latBuffer == NULL){
                            printf("REALLOC ERROR (latBuffer)\n");
                        }
                    }
                    if(longBufferCount + 1 == maxLongBufferCount){
                        maxLongBufferCount += 10;
                        longBuffer = realloc(longBuffer, maxLongBufferCount * sizeof(*longBuffer));
                        if(longBuffer == NULL){
                            printf("REALLOC ERROR (longBuffer)\n");
                        }
                    }
                }
                else
                    continue;
            }else{
                endOfInput = 1;
            }
        }
        if(latPMCCanFitMore || latSwingCanFitMore || latGorillaCanFitMore){
            latPMCCanFitMore = latPMCCanFitMore && fitValuePMC(&PMCLat, latBuffer[currentLatIndex]);
            latSwingCanFitMore = latSwingCanFitMore && fitValueSwing(&swingLat, timeBuffer[currentLatIndex], latBuffer[currentLatIndex]);
            if(latGorillaCanFitMore){
                fitValueGorilla(&gorillaLat, latBuffer[currentLatIndex]);
                latGorillaCanFitMore = gorillaLat.length < GORILLA_MAX;
            }
            currentLatIndex++;


        }else{
            selectModel(&selectedModelLat, startLatIndex, &PMCLat, &swingLat, &gorillaLat, latBuffer);
            currentLatIndex = selectedModelLat.end_index;
            if(currentLatIndex+1 == latBufferCount){
                latBuffer[0] = latBuffer[currentLatIndex];
                currentLatIndex = 0;
                latBufferCount = 1;
            }
            latiCount++;
            // Gorilla need to print all compressed values. 
            // For other models there is only one value
            int valuesCount = selectedModelLat.model_type_id == GORILLA_ID ? gorillaLat.compressed_values.bytes_counter : 1;
            writeModelToFile(latfpt, selectedModelLat, latFirst, timeBuffer[startLatIndex], timeBuffer[currentLatIndex]);
            latFirst = 0;
            startLatIndex = currentLatIndex;
            resetGorilla(&gorillaLat);
            resetPMC(&PMCLat);
          resetSwing(&swingLat);
            resetSelectedModel(&selectedModelLat);
        }
        if(longPMCCanFitMore || longSwingCanFitMore || longGorillaCanFitMore){
            longPMCCanFitMore = longPMCCanFitMore && fitValuePMC(&PMCLong, longBuffer[currentLongIndex]);
            longSwingCanFitMore = longSwingCanFitMore && fitValueSwing(&swingLong, timeBuffer[currentLongIndex], longBuffer[currentLongIndex]);
            if(longGorillaCanFitMore){
                fitValueGorilla(&gorillaLong, longBuffer[currentLongIndex]);
                longGorillaCanFitMore = gorillaLong.length < GORILLA_MAX;
            }
            currentLongIndex++;


        }else{
            selectModel(&selectedModelLong, startLongIndex, &PMCLong, &swingLong, &gorillaLong, longBuffer);
            currentLongIndex = selectedModelLong.end_index;
            if(currentLongIndex+1 == longBufferCount){
                longBuffer[0] = longBuffer[currentLongIndex];
                currentLongIndex = 0;
                longBufferCount = 1;
            }
            longCount++;
            // Gorilla need to print all compressed values. 
            // For other models there is only one value
            int valuesCount = selectedModelLong.model_type_id == GORILLA_ID ? gorillaLong.compressed_values.bytes_counter : 1;
            writeModelToFile(longfpt, selectedModelLong, longFirst, timeBuffer[startLongIndex], timeBuffer[currentLongIndex]);
            longFirst = 0;
            startLongIndex = currentLongIndex;
            resetGorilla(&gorillaLong);
            resetPMC(&PMCLong);
          resetSwing(&swingLong);
            resetSelectedModel(&selectedModelLong);
        }
    }
    //writeGorillaToFile(longfpt, dataLong, index, longFirst);
    //writeSwingToFile(longfpt, dataLong, index, longFirst);
    //writePMCMeanToFile(longfpt, dataLong, index, longFirst);
    
    //writeGorillaToFile(latfpt, dataLat, index, latFirst);
    //writeSwingToFile(latfpt, dataLat, index, latFirst);
    //writePMCMeanToFile(latfpt, dataLat, index, latFirst);
    printf("results:\nlatitude = %d\nlongitude = %d\n", latiCount, longCount);
    closeFile(latfpt);
    closeFile(longfpt);
    closeFile(stream);
}

void resetSwing(struct swing *data){
  data->first_timestamp = 0;
  data->last_timestamp = 0;
  data->first_value = NAN;
  data->upper_bound_slope = NAN;
  data->upper_bound_intercept = NAN;
  data->lower_bound_slope = NAN;
  data->lower_bound_intercept = NAN;
  data->length = 0;
}


struct swing getSwing(double errorBound){
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
    gorilla->compressed_values.bytes = realloc(gorilla->compressed_values.bytes, 4 * gorilla->compressed_values.bytes_capacity * sizeof(uint8_t));
    if(gorilla->compressed_values.bytes == NULL){
        printf("REALLOC ERROR\n");
    }
}

void resetSelectedModel(struct SelectedModel* model){
    model->model_type_id = 0;
    model->end_index = 0;
    model->min_value = 0;
    model->max_value = 0;
    model->values_capacity = 1;
    model->values = realloc(model->values, model->values_capacity * sizeof(uint8_t));
    if(model->values == NULL){
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
    mod.values = (uint8_t*) malloc (mod.values_capacity * sizeof(uint8_t));
    if(mod.values == NULL){
        printf("MALLOC ERROR (mod)\n");
    }
    return mod;
}





