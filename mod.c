#include "mod.h"
#include <stdio.h>

const uint8 PMC_MEAN_ID = 0;
const uint8 SWING_ID = 1;
const uint8 GORILLA_ID = 2;

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
void selectModel(struct SelectedModel* data, size_t start_index, struct PMCMean* pmcmean, struct swing* swing, struct Gorilla* gorilla, float *uncompressed_values){
    struct bytes_per_value bytes[3];

    bytes[0].id = PMC_MEAN_ID;
    bytes[0].bytes = get_bytes_per_value_pmc(pmcmean);
    bytes[1].id = SWING_ID;
    bytes[1].bytes = get_bytes_per_value_swing(swing);
    bytes[2].id = GORILLA_ID;
    bytes[2].bytes = get_bytes_per_value_gorilla(gorilla);

    struct bytes_per_value selectedModel;

    for(int i=0; i < 3; i++){
        if (i==0){
            selectedModel = bytes[i];
        }
        else if(selectedModel.bytes > bytes[i].bytes){
            selectedModel = bytes[i];
        }
    }

    switch(selectedModel.id){
        case PMC_MEAN_ID:
            break;
    }
    
}
void select_pmc_mean(struct SelectedModel* model, size_t start_index, struct PMCMean* pmcmean){
    float value = get_model_pmcmean(pmcmean);
    size_t end_index = start_index +
}


