#include "argument_handler.h"

void removeQuotesFromArgs(char* str){
    if(str[0] == '\''){
        str = &str[1];
    }
    if(str[strlen(str)-1] == '\''){
        str[strlen(str)-1] = '\0';
    }
}

Arguments handleArguments(int argc, char *argv[])
{
    Arguments argStruct;
    argStruct.cols = NULL;
    argStruct.containsPosition = 0;
    argStruct.timestampCol = -1;

    int c;
    int digit_optind = 0;
    const char s[2] = " ";
    char *token;
    int count = 0;



    while (1)
    {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"position", required_argument, 0, 'p'},
            {"columns", required_argument, 0, 'c'},
            {"timestamps", required_argument, 0, 't'},
            {0, 0, 0, 0}};

        c = getopt_long(argc, argv, "p:c:t:",
                        long_options, &option_index);
        if (c == -1)
            break;
        // printf("DEBUG: %d\n", debug);
        
        switch (c)
        {
        case 'p':
            // Debug mode seems to add single quotation marks around the arguments. 
            // The following two if's remove those
            if(optarg[0] == '\''){
                optarg = &optarg[1];
            }
            if(optarg[strlen(optarg)-1] == '\''){
                optarg[strlen(optarg)-1] = '\0';
            }

            count = 0;
            token = strtok(optarg, s);
            while( token != NULL ) {
                count++;
                // Handle args here
                printf( " %s\n", token );
                if(count==1) argStruct.latCol.col = atoi(token);
                if(count==2) argStruct.longCol.col = atoi(token);
                if(count==3){
                    argStruct.latCol.error  = atof(token); 
                    argStruct.longCol.error = atof(token);
                    argStruct.containsPosition = 1;
                } 

                if(count>3){
                    printf("Too many arguments for position. Arguments should be: <lat> <long> <error>\n");
                    exit(1);
                }

                // Get next arg
                token = strtok(NULL, s); // NULL is not a mistake!
            }
            
            if(count<3){
                printf("Too few arguments for position. Arguments should be: <lat> <long> <error>\n");
                exit(1);
            }

            break;
        case 'c':
            //From documentation. Not sure what it does
            if (digit_optind != 0 && digit_optind != this_option_optind)
                printf("digits occur in two different argv-elements.\n");
            digit_optind = this_option_optind;
            
            // Debug mode seems to add single quotation marks around the arguments. 
            // The following two if's remove those
            if(optarg[0] == '\''){
                optarg = &optarg[1];
            }
            if(optarg[strlen(optarg)-1] == '\''){
                optarg[strlen(optarg)-1] = '\0';
            }

            count = 0;

            token = strtok(optarg, s);
            while( token != NULL ) {
                
                int column = count / 3;
                // Handle arg here
                if(count%3 == 0){
                    if(argStruct.cols == NULL){
                        argStruct.cols = malloc(sizeof(Cols));
                        // argStruct.cols->currentSize = 1;
                        argStruct.numberOfCols = 1;
                        argStruct.cols[0].col = atoi(token);
                        printf("First size: %d\n", sizeof(*argStruct.cols));
                    } else {
                        // argStruct.cols->currentSize++;
                        argStruct.numberOfCols++;
                        printf("SIZE: %d\n", sizeof(*(argStruct.cols)) * argStruct.numberOfCols);
                        argStruct.cols = realloc(argStruct.cols, sizeof(*(argStruct.cols)) * argStruct.numberOfCols);
                        argStruct.cols[column].col = atoi(token);
                    }
                    printf("Column: %s\n", token);
                }
                if(count%3 == 1){
                    printf("%s\n", token);
                    argStruct.cols[column].error = atof(token); 
                    printf("Error: %s\n", token);
                }
                if(count%3 == 2){
                    if(*token == 'A'){ // Absolute
                        argStruct.cols[column].isAbsolute = 1;
                    }
                    if(*token == 'R'){ // Relative
                        argStruct.cols[column].isAbsolute = 0;
                    }
                    printf("A/R: %s\n", token);
                }
                token = strtok(NULL, s); // NULL is not a mistake!
                count++;
            }

            if(count % 3 != 0){
                printf("Not the expected number of arguments for columns. Number of parameters should be divisible by 3 and follow the following format:\n");
                printf("<column (int)> <error (float)> <absolute (A) / relative (R)>\n");
                exit(1);
            }
            break;
        case 't':
            if(optarg[0] == '\''){
                optarg = &optarg[1];
            }
            if(optarg[strlen(optarg)-1] == '\''){
                optarg[strlen(optarg)-1] = '\0';
            }
            argStruct.timestampCol = atoi(optarg);
            break;
        default:
            printf("Unknown option, exiting ...\n");
            exit(1);
        }
    }

    if(argStruct.timestampCol<1){
        printf("Timestamp column must be specified, and it must be above 0. It should follow the following format:\n");
        printf("--timestamps <column (int)>\n");
        exit(1);
    }

    // From documentation ...
    if (optind < argc)
    {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }

    return argStruct;
}