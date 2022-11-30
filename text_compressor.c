//
// Created by danie on 30-11-2022.
//

#include "text_compressor.h"
#include <string.h>
Text_compressor get_text_compressor(){
    return (Text_compressor){.string = "", .count = 0};
}


int fit_string(Text_compressor* data, char* string){
    if(data->count == 0){
        data->string = string;
        data->count++;
        return 1;
    }else if(strcmp(data->string, string) == 0){ //strcmp returns 0 if they are equal
        data->count++;
        return 1;
    }
    return 0;
}

void reset_text_compressor(Text_compressor* data){
    data->count = 0;
}