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

#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <ctype.h>


#define ERROR_BOUND 0.1
#define INITIAL_BUFFER 200
#define GORILLA_MAX 50
#define VECTOR_TRUE 1
#define VECTOR_FALSE 0


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

int main(int argc, char *argv[])
{
int c;
    int digit_optind = 0;

   while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"position", required_argument, 0,  'p' },
            {"columns",  required_argument, 0,  'c' },
            {0,         0,                 0,  0 }
        };

       c = getopt_long(argc, argv, "pc:",
                 long_options, &option_index);
        if (c == -1)
            break;
        // printf("DEBUG: %d\n", debug);
       switch (c) {
        case 'p':
            printf("option %s", long_options[option_index].name);
            if (optarg)
                printf(" with arg %s", optarg);
            printf("\n");
            break;

        case 'c':
            if (digit_optind != 0 && digit_optind != this_option_optind)
              printf("digits occur in two different argv-elements.\n");
            digit_optind = this_option_optind;
            printf("option %c\n", c);
            if (optarg)
                printf(" with arg %s\n", optarg);
            break;
        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

   if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }


    Uncompressed_data latData = create_uncompressed_data_maneger(outPutCsvFileLat, VECTOR_TRUE);
    Uncompressed_data longData = create_uncompressed_data_maneger(outPutCsvFileLong, VECTOR_TRUE);
    FILE* position = openFile(outPutCsvFilePosition);
    Vector_based vb = get_vector_based();
    

    FILE* stream = fopen(dataPath, "r");
    char line[1024];

    int longFirst = 1;
    int latFirst = 1;
    int position_first = 1;
    long timestamp = 0;
    struct tm tmVar;



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
            //insert_data(&latData, timestamp, strtof(getfield(latStr, 5), &errorPointer), &latFirst);
            //insert_data(&longData, timestamp, strtof(getfield(longStr, 6), &errorPointer), &longFirst);
            insert_vector_based_data(position, &vb, timestamp, strtof(getfield(latStr, 5), &errorPointer), strtof(getfield(longStr, 6), &errorPointer), &position_first);
            //fit_values_vector_based(&vb, timestamp, strtof(getfield(latStr, 5), &errorPointer), strtof(getfield(longStr, 6), &errorPointer));
            

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
    print_vector_based(position, &vb, &position_first);
    closeFile(position);
    fclose(stream);
}