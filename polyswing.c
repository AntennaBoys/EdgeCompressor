#include "polyswing.h"
#include "constants.h"
#include "bitreader.h"


void updateATA(Mat* ATA, long new_x);
void updateATY(Mat* ATY, long new_x, double new_y);
Mat* updateAll(Mat* ATA, Mat* ATY, long new_x, double new_y, struct polySwing *model);
terms ingest(long x, double y, int printmat, Mat* ATA, Mat* ATY, struct polySwing *model);
void writeToFile(FILE *file, struct polySwing model, int index, char* start, int id);
void resetStruct(struct polySwing *data);



// int main(){
//     char* dataPath = "C:/Users/power/CLionProjects/EdgeCompressor/out.csv";
//     struct polySwing dataLat;
//     double error = 0.001;
//     dataLat.length = 0;
//     dataLat.error_bound = error;
//     dataLat.deltaTime = 0;
//     dataLat.terminateSegment = 0;
//     struct polySwing dataLong;
//     dataLong.length = 0;
//     dataLong.error_bound = error;
//     dataLong.deltaTime = 0;
//     dataLong.terminateSegment = 0;
//     int index = 0;

//     int latiCount = 0;
//     int longCount = 0;
//     char* longFirst = "";
//     char* latFirst = "";

//     FILE *latfpt;
//     latfpt = fopen("D:\\IncrementalPoly\\test.csv", "w+");
//     fprintf(latfpt,"pow2, pow1, pow0\n");
//     fclose(latfpt);

//     FILE *test;
//     test = fopen("D:\\IncrementalPoly\\lower.csv", "w+");
//     fprintf(test,"pow2, pow1, pow0\n");


//     FILE *test1;
//     test1 = fopen("D:\\IncrementalPoly\\upper.csv", "w+");
//     fprintf(test1,"pow2, pow1, pow0\n");

//     FILE *test2;
//     test2 = fopen("D:\\IncrementalPoly\\current.csv", "w+");
//     fprintf(test2,"pow2, pow1, pow0\n");

//     FILE *test3;
//     test3 = fopen("D:\\IncrementalPoly\\break.csv", "w+");
//     fprintf(test3,"break\n");


//     FILE* tsFile = fopen("D:\\IncrementalPoly\\ts.csv", "w+");
//     FILE* stream = fopen(dataPath, "r");
//     char line[1024];
//     fprintf(tsFile, "ts, lat\n");
//     struct tm tmVar;
//     time_t timeVar;

//     FILE *latfptRe;
//     FILE *longfptRe;
//     latfptRe = fopen("D:\\stocks\\repos\\ShipMapPoints\\getdata\\polylat.json", "w+");
//     longfptRe = fopen("D:\\stocks\\repos\\ShipMapPoints\\getdata\\polylong.json", "w+");
//     fprintf(latfptRe,"{\"models\":[\n");
//     fprintf(longfptRe,"{\"models\":[\n");

//     while (fgets(line, 1024, stream))
//     {
//         char* lat = strdup(line);
//         char* longs = strdup(line);
//         char* ts = strdup(line);
//         char* errorPointer;
//         double latVal = strtod(getfield(lat, 5), &errorPointer);
//         double longVal = strtod(getfield(longs, 6), &errorPointer);
//         char* timestampTemp = getfield(ts, 2);

//         //printf("%s\n", timestampTemp);
//         //01/09/2022 00:00:0
//         if(sscanf(timestampTemp, "%d/%d/%d %d:%d:%d", &tmVar.tm_mday, &tmVar.tm_mon, &tmVar.tm_year, &tmVar.tm_hour, &tmVar.tm_min, &tmVar.tm_sec)==6){
//             tmVar.tm_year -= 1900;
//             tmVar.tm_mon -= 1;
//             tmVar.tm_isdst = 1;
//             timeVar = mktime(&tmVar)+3600;
//         }
//         else
//             continue;
//         //printf("to now %d\n", index);
//         if (index == 0){
//             dataLat.first_timestamp = (long)timeVar;
//         }
//         if ((long)timeVar == dataLat.last_timestamp){
//             continue;
//         }

//         int resLat = fitValues_swing(&dataLat, (long)timeVar, latVal);
//         int resLong = fitValues_swing(&dataLong, (long)timeVar, longVal);

//         if(resLat && index > 2){
//             fprintf(tsFile, "%ld, %lf\n", dataLat.deltaTime, latVal);
//         }
//         /*if (index == 420){
//             break;
//         }*/
//         if (index > 2){
//             fprintf(test, "%.20f,%.20f,%.20f\n", dataLat.lower.pow2, dataLat.lower.pow1, dataLat.lower.pow0);
//             fprintf(test1, "%.20f,%.20f,%.20f\n", dataLat.upper.pow2, dataLat.upper.pow1, dataLat.upper.pow0);
//             fprintf(test2, "%.20f,%.20f,%.20f\n", dataLat.current.pow2, dataLat.current.pow1, dataLat.current.pow0);
//             if (!resLat){
//                 fprintf(test3, "%d\n", index-3);
//             }
//         }


//         if (!resLat){
//             writeToFile(latfptRe, dataLat, index, latFirst, latiCount++);
//             resetStruct(&dataLat);
//             latFirst = ",";
//             dataLat.terminateSegment = 0;
//             fitValues_swing(&dataLat, (long)timeVar, latVal);
//             //printf("can fit %d", index);
//             //printf("Can do to: %ld", (long)timeVar);
//             //break;
//         }
//         if (!resLong){
//             writeToFile(longfptRe, dataLong, index, longFirst, longCount++);
//             resetStruct(&dataLong);
//             longFirst = ",";
//             dataLong.terminateSegment = 0;
//             fitValues_swing(&dataLong, (long)timeVar, longVal);
//         }
//         index++;
//         free(lat);
//         free(longs);
//         free(ts);
//     }


//     writeToFile(latfptRe, dataLat, index, latFirst, latiCount++);
//     writeToFile(longfptRe, dataLong, index, longFirst, longCount++);
//     fclose(stream);
//     fclose(tsFile);
//     fprintf(latfptRe,"]}\n");
//     fprintf(longfptRe,"]}\n");
//     fclose(latfptRe);
//     fclose(longfptRe);
//     fclose(test);
//     fclose(test1);
//     fclose(test2);
//     fclose(test3);
//     printf("results:\nlatitude = %d\nlongitude = %d\n", latiCount, longCount);

//     return 0;
// }

int fitValuesPolySwing(struct polySwing *data, long timeStamp, double value){
    double maximum_deviation = fabs(value * (data->error_bound / 100.1));

    data->deltaTime = timeStamp - data->first_timestamp;
    if (data->length == 0) {
        // Line 1 - 2 of Algorithm 1 in the Swing and Slide paper.
        data->first_timestamp = timeStamp;
        data->last_timestamp = timeStamp;
        data->first_value = value;
        data->length += 1;
        data->ATA = newmat(3,3, 0);
        data->ATY = newmat(3,1, 0);
        return 1;
    }
    else if (data->length == 1) {
        // Line 3 of Algorithm 1 in the Swing and Slide paper.
        data->second_value = value;
        data->second_timestamp = timeStamp;
        data->length += 1;
        return 1;
    } else if (data->length == 2){
        data->last_timestamp = timeStamp;
        ingest(0, data->first_value,0, data->ATA, data->ATY, data);
        ingest(data->second_timestamp - data->first_timestamp, data->second_value,0, data->ATA, data->ATY, data);

        Mat* upperATA = copyvalue(data->ATA);
        Mat* upperATY = copyvalue(data->ATY);
        data->upper = ingest(data->deltaTime, value+maximum_deviation, 0, upperATA, upperATY, data);
        freemat(upperATA);
        freemat(upperATY);

        Mat* lowerATA = copyvalue(data->ATA);
        Mat* lowerATY = copyvalue(data->ATY);
        data->lower = ingest(data->deltaTime, value-maximum_deviation, 0, lowerATA, lowerATY, data);
        freemat(lowerATA);
        freemat(lowerATY);

        data->current = ingest(data->deltaTime, value,0, data->ATA, data->ATY, data);
        data->length += 1;
        return 1;
    }
    else {
        // Line 6 of Algorithm 1 in the Swing and Slide paper.
        double upper_bound_approximate_value = data->upper.pow2 * data->deltaTime * data->deltaTime + data->upper.pow1 * data->deltaTime + data->upper.pow0;
        double lower_bound_approximate_value = data->lower.pow2 * data->deltaTime * data->deltaTime + data->lower.pow1 * data->deltaTime + data->lower.pow0;

        //printf("\n");
        //printf("%f\n", upper_bound_approximate_value);
        //printf("%f", lower_bound_approximate_value);
        //printf("\n");
        if (upper_bound_approximate_value < lower_bound_approximate_value){
            //printf("e");
        }

        if (upper_bound_approximate_value + maximum_deviation < value
            || lower_bound_approximate_value - maximum_deviation > value)
        {
            return 0;
        } else {
            data->last_timestamp = timeStamp;

            // Line 17 of Algorithm 1 in the Swing and Slide paper.
            if (upper_bound_approximate_value - maximum_deviation > value) {
                Mat* upperATA = copyvalue(data->ATA);
                Mat* upperATY = copyvalue(data->ATY);
                data->upper = ingest(data->deltaTime, value+maximum_deviation, 0, upperATA, upperATY, data);
                freemat(upperATA);
                freemat(upperATY);
            }
            if (lower_bound_approximate_value + maximum_deviation < value) {
                Mat* lowerATA = copyvalue(data->ATA);
                Mat* lowerATY = copyvalue(data->ATY);
                data->lower = ingest(data->deltaTime, value-maximum_deviation, 0, lowerATA, lowerATY, data);
                freemat(lowerATA);
                freemat(lowerATY);
            }
            if (data->terminateSegment){
                return 0;
            }
            struct terms temp = ingest(data->deltaTime, value,1, data->ATA, data->ATY, data);
            if (data->terminateSegment){
                return 0;
            }
            else {
                data->current = temp;
            }
            data->length += 1;

            return 1;
            }
    }
}


Mat* updateAll(Mat* ATA, Mat* ATY, long new_x, double new_y, struct polySwing *model){
    updateATA(ATA, new_x);
    updateATY(ATY, new_x, new_y);

    Mat* _ATA, *_ATY, *res;

    // Get upper left element in ATA. This number describes the number of points we have ingested
    int elems = get(ATA, 1,1);

    // Return first point in ATY if we only have ingested one point (i.e. the y-value of the point)
    if(elems == 1){
        res = newmat(1,1,get(ATY,1,1));
    }
        // If two points are ingested, operate on the upper left 2x2 submatrix (of ATA) and first two elements of ATY.
        // This makes a linear regression
    else if(elems == 2){
        _ATA = submat(ATA, 1,2,1,2);
        Mat* sub = submat(ATY, 1,1,1,2);
        _ATY = transpose(sub);
        Mat* ATA_inv = inverse(_ATA);
        //showmat(_ATA);
        //showmat(ATA_inv);
        //exit(23);
        res = multiply(ATA_inv, _ATY);
        freemat(_ATA);
        freemat(_ATY);
        freemat(sub);
        freemat(ATA_inv);
    }
        // If three or more points are ingested, operate on the entire matrix to do a polynomial regression
    else if(elems >= 3){
        _ATA = ATA;
        _ATY = ATY;

        if(det(_ATA) != 0){
            Mat* ATA_inv = inverse(_ATA);
            res = multiply(ATA_inv, _ATY);
            //showmat(_ATA);
            //showmat(ATA_inv);
            freemat(ATA_inv);
        } else {
            printf("wrong\n");
            model->terminateSegment = 1;
            res = newmat(3,3,0);
        }
    }
    return res;
}

void updateATY(Mat* ATY, long new_x, double new_y){
    float current = get(ATY, 1,1);
    set(ATY, 1, 1, current + pow(new_x, 0)*new_y);
    current = get(ATY, 1,2);
    set(ATY, 1, 2, current + pow(new_x, 1)*new_y);
    current = get(ATY, 1,3);
    set(ATY, 1, 3, current + pow(new_x, 2)*new_y);
}

void updateATA(Mat* ATA, long new_x){
    //Øverste række
    float current = get(ATA, 1,1);
    set(ATA, 1, 1, current + pow(new_x, 0));
    current = get(ATA, 1,2);
    set(ATA, 1, 2, current + pow(new_x, 1));
    current = get(ATA, 1,3);
    set(ATA, 1, 3, current + pow(new_x, 2));
    //Mellemste række
    current = get(ATA, 2,1);
    set(ATA, 2, 1, current + pow(new_x, 1));
    current = get(ATA, 2,2);
    set(ATA, 2, 2, current + pow(new_x, 2));
    current = get(ATA, 2,3);
    set(ATA, 2, 3, current + pow(new_x, 3));
    //Nederste række
    current = get(ATA, 3,1);
    set(ATA, 3, 1, current + pow(new_x, 2));
    current = get(ATA, 3,2);
    set(ATA, 3, 2, current + pow(new_x, 3));
    current = get(ATA, 3,3);
    set(ATA, 3, 3, current + pow(new_x, 4));
}

terms ingest(long x, double y, int printmat, Mat* ATA, Mat* ATY, struct polySwing *model){
    Mat* res = updateAll(ATA, ATY, x, y, model);
    if(printmat){
        //printf("%ld, %lf\n", x,y);
        //showmat(res);
    }

    terms result = {0,0,0};
    int pow = get(ATA, 1,1);

    if (pow > 2){
        result.pow2 = get(res, 3, 1);
        result.pow1 = get(res, 2, 1);
        result.pow0 = get(res, 1, 1);
        // if (printmat && !model->terminateSegment){
        //     FILE *latfpt;
        //     latfpt = fopen("D:\\IncrementalPoly\\test.csv", "a");
        //     fprintf(latfpt,"%.20f,%.20f,%.20f\n", result.pow2, result.pow1, result.pow0);
        //     fclose(latfpt);
        // }
    }
    freemat(res);
    return result;
}


void resetStruct(struct polySwing *data){
    data->length = 0;
}

float get_bytes_per_value_polyswing(struct polySwing* data){
    return (float) (3 * VALUE_SIZE_IN_BYTES) / (float) data->length;
}

struct polySwing getPolySwing(double errorBound){
    struct polySwing model;
    model.error_bound = errorBound;
    model.length = 0;
    model.deltaTime = 0;
    model.terminateSegment = 0;
    return model;
}

void deletePolySwing(struct polySwing* polySwing){
    freemat(polySwing->ATA);
    freemat(polySwing->ATY);
}

float* gridPolySwing(float pow0, float pow1, uint8_t* values, long* timestamps, int timestampCount){
    BitReader bitReader = tryNewBitreader(values, 4);
    float pow2 = intToFloat(read_bits(&bitReader, 32));
    float* result;
    result = malloc(timestampCount * sizeof(*result));
    if(!result){
        printf("CALLOC ERROR (gridPolySwing: result)\n");
    }
    for(int i = 0; i < timestampCount; i++){
        result[i] = pow1 * timestamps[i] + pow2 * (timestamps[i] * timestamps[i]) + pow0;
    }
    return result;
}