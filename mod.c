#include "mod.h"
#include <stdio.h>
#include <string.h>




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

    switch((int) selectedModel.id){
      case PMC_MEAN_ID:
        select_pmc_mean(data, start_index, pmcmean);
        break;
      case SWING_ID:
        select_swing(data,start_index,swing);
        break;
      case GORILLA_ID:
        select_gorilla(data, start_index, gorilla, uncompressed_values);
        break;
    }
}
void select_pmc_mean(struct SelectedModel* model, size_t start_index, struct PMCMean* pmcmean){
  float value = get_model_pmcmean(pmcmean);
  size_t end_index = start_index + get_length_pmcmean(pmcmean) - 1;

  model->model_type_id = (uint8_t) PMC_MEAN_ID;
  model->end_index = end_index;
  model->min_value = value;
  model->max_value = value;
}

void select_swing(struct SelectedModel* model, size_t start_index, struct swing* swing){
  float arr[2];

  get_model_swing(swing, arr);

  float start_value = arr[0];
  float end_value = arr[1];

  size_t end_index = start_index + get_length_swing(swing) - 1;
  if (start_value < end_value){
    model->max_value = end_value;
    model->min_value = start_value;
  }
  else{
    model->max_value = start_value;
    model->min_value = end_value;
  }
  model->model_type_id = (uint8_t) SWING_ID;
  model->end_index = end_index;
  model->values[0] = (uint8_t) (start_value < end_value);

}



void select_gorilla(struct SelectedModel* model, size_t start_index, struct Gorilla* gorilla, float *uncompressed_values){
    size_t end_index = start_index + get_length_gorilla(gorilla) - 1;
    model->values_capacity = end_index-start_index;

    model->values = realloc(model->values, model->values_capacity * sizeof(*model->values));

    float max = uncompressed_values[start_index];
    float min = uncompressed_values[start_index];

    for(int i = 0; i<model->values_capacity; i++){
      float temp_val = uncompressed_values[i+start_index];
      model->values[i] = temp_val;
      if (max < temp_val){
        max = temp_val;
      }
      else if(min > temp_val){
        min = temp_val;
      }
    }

    model->model_type_id = (uint8_t) GORILLA_ID;
    model->end_index = end_index;
    model->min_value = min;
    model->max_value = max;
    model->values = get_compressed_values(gorilla);

}


