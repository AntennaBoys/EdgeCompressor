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

    for(float i = 0.0; i < 1; i++){
        
        struct SelectedModel* data = init_selectedModel();
        data.select
        compress_value(data, 10101.21321);
        compress_value(data, 324.234);
        compress_value(data, 234.12);
        compress_value(data, 234.12);
        compress_value(data, 234.12);
        compress_value(data, 234.12);
        compress_value(data, 234.12);
        


        
        for(int i = 0; i< data->compressed_values.bytes_counter; i++){
            printf("compressed value: %d\n", data->compressed_values.bytes[i]);
        }

        // compress_value(data, 545.12);

    }



    getchar();
   
}

