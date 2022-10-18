#include "gorilla.h"
#include <stdio.h>
int main(){
    //struct Gorilla data = init();
    // compress_value(&data, 10032.89798);
    // compress_value(&data, 2342.89798);

    // compress_value(&data, 3.345);
    // compress_value(&data, 345.89798);
    // compress_value(&data, 545.12);
    //     compress_value(&data, 1.12);


    //compress_value(&data, 5453.12);

        
    struct Gorilla* data = malloc(sizeof(struct Gorilla));
    *data = init();
    //compress_value(data, 10101.21321);
    compress_value(data, 234.234);
    compress_value(data, 234.12);
    compress_value(data, 234.13);
    compress_value(data, 234.14);
    compress_value(data, 234.12);
    compress_value(data, 234.14);
    compress_value(data, 1000.14);
    compress_value(data, 1000.18);
    compress_value(data, 235.14);
    compress_value(data, 234.14);
    compress_value(data, 2340.14);
    


    
    for(int i = 0; i< data->compressed_values.bytes_counter; i++){
        printf("compressed value: %d\n", data->compressed_values.bytes[i]);
    }

    //compress_value(data, 545.12);

    getchar();
   
}

