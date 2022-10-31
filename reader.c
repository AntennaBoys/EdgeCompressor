#include "mod.h"
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
                 
    struct Gorilla gorillaLat = getGorilla();
    struct Gorilla gorillaLong = getGorilla();
    struct PMCMean PMCLat = getPMCMean(ERROR_BOUND);
    struct PMCMean PMCLong = getPMCMean(ERROR_BOUND);
    struct swing swingLat = getSwing(ERROR_BOUND);
    struct swing swingLong = getSwing(ERROR_BOUND);
    struct SelectedModel selectedModelLat = getSelectedModel();
    struct SelectedModel selectedModelLong = getSelectedModel();
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

    long *latTimeBuffer = (long*) calloc(INITIAL_BUFFER, sizeof(long));
    long *longTimeBuffer = (long*) calloc(INITIAL_BUFFER, sizeof(long));
    int maxTimeBufferCount = INITIAL_BUFFER;

    int resetLong = 1;
    int resetLat = 1;

    int currentModel = 0; // start at PMCmean

    int yetEmittedLat = 0;
    int yetEmittedLong = 0;

    int endOfInput = 0;
    while((!(latBufferCount == currentLatIndex) || !(longBufferCount == currentLongIndex))|| !endOfInput)
    {
        if(!endOfInput && (currentLatIndex == latBufferCount || currentLongIndex == longBufferCount))
        {
            if(fgets(line, 1024, stream)){
                char* latStr = _strdup(line);
                char* longStr = _strdup(line);
                char* ts = _strdup(line);
                char* errorPointer;
                const char* timestampTemp = getfield(ts, 2);

                //01/09/2022 00:00:0
                if(sscanf(timestampTemp, "%d/%d/%d %d:%d:%d", &tmVar.tm_mday, &tmVar.tm_mon, &tmVar.tm_year, &tmVar.tm_hour, &tmVar.tm_min, &tmVar.tm_sec)==6){
                    tmVar.tm_year -= 1900;
                    tmVar.tm_mon -= 1;
                    //TODO: implement sudocode line 14-16, 18

                    // SudoC: line  17
                    latTimeBuffer[latBufferCount] = mktime(&tmVar)+3600;
                    longTimeBuffer[longBufferCount] = mktime(&tmVar)+3600;
                    latBuffer[latBufferCount++] = strtod(getfield(latStr, 5), &errorPointer);
                    longBuffer[longBufferCount++] = strtod(getfield(longStr, 6), &errorPointer);
                    // ensure we have enough space in the buffers
                    
                    if(latBufferCount + 1 == maxLatBufferCount){
                        maxLatBufferCount += 10;
                        latBuffer = realloc(latBuffer, maxLatBufferCount * sizeof(*latBuffer));
                        if(latBuffer == NULL){
                            printf("REALLOC ERROR (latBuffer)\n");
                        }
                        latTimeBuffer = realloc(latTimeBuffer, maxLatBufferCount * sizeof(*latTimeBuffer));
                        if(latTimeBuffer == NULL){
                            printf("REALLOC ERROR (timeBuffer)\n");
                        }
                    }
                    if(longBufferCount + 1 == maxLongBufferCount){
                        maxLongBufferCount += 10;
                        longBuffer = realloc(longBuffer, maxLongBufferCount * sizeof(*longBuffer));
                        if(longBuffer == NULL){
                            printf("REALLOC ERROR (longBuffer)\n");
                        }
                        longTimeBuffer = realloc(longTimeBuffer, maxLongBufferCount * sizeof(*longTimeBuffer));
                        if(longTimeBuffer == NULL){
                            printf("REALLOC ERROR (timeBuffer)\n");
                        }
                    }
                }
                else {
                  free(latStr);
                  free(longStr);
                  free(ts);
                  continue;
                }
                free(longStr);
                free(latStr);
                free(ts);
            }else{
                endOfInput = 1;
            }
        }else{

            
            if((latPMCCanFitMore || latSwingCanFitMore || latGorillaCanFitMore) && currentLatIndex < latBufferCount){
                if (latPMCCanFitMore){
                    latPMCCanFitMore = fitValuePMC(&PMCLat, latBuffer[currentLatIndex]);
                }
                if (latSwingCanFitMore){
                    latSwingCanFitMore = fitValueSwing(&swingLat, latTimeBuffer[currentLatIndex], latBuffer[currentLatIndex]);
                }
                if(latGorillaCanFitMore){
                    fitValueGorilla(&gorillaLat, latBuffer[currentLatIndex]);
                    latGorillaCanFitMore = gorillaLat.length < GORILLA_MAX;
                }
                if(latPMCCanFitMore || latSwingCanFitMore || latGorillaCanFitMore){
                    currentLatIndex++;
                }


            }else if(latBufferCount > 0){
                selectModel(&selectedModelLat, startLatIndex, &PMCLat, &swingLat, &gorillaLat, latBuffer);
                currentLatIndex = selectedModelLat.end_index+1;
                writeModelToFile(latfpt, selectedModelLat, latFirst, latTimeBuffer[startLatIndex], latTimeBuffer[currentLatIndex-1]);
                latFirst = 0;
                for (int i = 0; i+currentLatIndex < latBufferCount-1; i++){
                    latBuffer[i] = latBuffer[i+currentLatIndex];
                    latTimeBuffer[i] = latTimeBuffer[i+currentLatIndex];
                }
                latBufferCount = latBufferCount - currentLatIndex;
                currentLatIndex = 0;
                
                latiCount++;
                // Gorilla need to print all compressed values. 
                // For other models there is only one value
                startLatIndex = currentLatIndex;
                resetGorilla(&gorillaLat);
                resetPMC(&PMCLat);
                resetSwing(&swingLat);
                resetSelectedModel(&selectedModelLat);
                latPMCCanFitMore = 1;
                latSwingCanFitMore = 1;
                latGorillaCanFitMore = 1;
            }
            if((longPMCCanFitMore || longSwingCanFitMore || longGorillaCanFitMore) && currentLongIndex < longBufferCount){
                if (longPMCCanFitMore){
                    longPMCCanFitMore = fitValuePMC(&PMCLong, longBuffer[currentLongIndex]);
                }
                if (longSwingCanFitMore){
                    longSwingCanFitMore = fitValueSwing(&swingLong, longTimeBuffer[currentLongIndex], longBuffer[currentLongIndex]);
                }
                if(longGorillaCanFitMore){
                    fitValueGorilla(&gorillaLong, longBuffer[currentLongIndex]);
                    longGorillaCanFitMore = gorillaLong.length < GORILLA_MAX;
                }
                if(longPMCCanFitMore || longSwingCanFitMore || longGorillaCanFitMore){
                    currentLongIndex++;
                }


            }else if(longBufferCount > 0){
                selectModel(&selectedModelLong, startLongIndex, &PMCLong, &swingLong, &gorillaLong, longBuffer);
                currentLongIndex = selectedModelLong.end_index+1;
                writeModelToFile(longfpt, selectedModelLong, longFirst, longTimeBuffer[startLongIndex], longTimeBuffer[currentLongIndex-1]);
                longFirst = 0;
                for (int i = 0; i+currentLongIndex < longBufferCount-1; i++){
                    longBuffer[i] = longBuffer[i+currentLongIndex];
                    longTimeBuffer[i] = longTimeBuffer[i+currentLongIndex];
                }
                longBufferCount = longBufferCount - currentLongIndex;
                currentLongIndex = 0;

                longCount++;
                // Gorilla need to print all compressed values. 
                // For other models there is only one value
                startLongIndex = currentLongIndex;
                resetGorilla(&gorillaLong);
                resetPMC(&PMCLong);
                resetSwing(&swingLong);
                resetSelectedModel(&selectedModelLong);
                longPMCCanFitMore = 1;
                longSwingCanFitMore = 1;
                longGorillaCanFitMore = 1;
            }
        }
    }
    if(latBufferCount > 0){
        selectModel(&selectedModelLat, startLatIndex, &PMCLat, &swingLat, &gorillaLat, latBuffer);
        currentLatIndex = selectedModelLat.end_index+1;
        writeModelToFile(latfpt, selectedModelLat, latFirst, latTimeBuffer[startLatIndex], latTimeBuffer[currentLatIndex-1]);
    }
    if(longBufferCount > 0){
        selectModel(&selectedModelLong, startLongIndex, &PMCLong, &swingLong, &gorillaLong, longBuffer);
        currentLongIndex = selectedModelLong.end_index+1;
        writeModelToFile(longfpt, selectedModelLong, longFirst, longTimeBuffer[startLongIndex], longTimeBuffer[currentLongIndex-1]);
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
    free(latBuffer);
    free(longBuffer);
}


















