#include "mod.h"


#define MODEL_COUNT 4


void select_model(Selected_model* data, size_t start_index, Pmc_mean* pmc_mean, Swing* swing, Gorilla* gorilla, Poly_swing* polyswing, float *uncompressed_values){
    struct bytes_per_value bytes[MODEL_COUNT];

    bytes[0].id = PMC_MEAN_ID;
    bytes[0].bytes = get_bytes_per_value_pmc(pmc_mean);
    bytes[1].id = SWING_ID;
    bytes[1].bytes = get_bytes_per_value_swing(swing);
    bytes[2].id = GORILLA_ID;
    bytes[2].bytes = get_bytes_per_value_gorilla(gorilla);
    bytes[3].id = POLYSWING_ID;
    bytes[3].bytes = get_bytes_per_value_polyswing(polyswing);
    printf("PMCMean bpv: %f\nSwing bpv: %f\nGorilla bpv: %f\nPolySwing bpv: %f\n\n", bytes[0].bytes, bytes[1].bytes, bytes[2].bytes, bytes[3].bytes);
    struct bytes_per_value selected_model;

    for(int i=0; i < MODEL_COUNT; i++){
        if (i==0){
            selected_model = bytes[i];
        }
        else if(selected_model.bytes > bytes[i].bytes){
            selected_model = bytes[i];
        }
    }

    switch((int) selected_model.id){
      case PMC_MEAN_ID:
        select_pmc_mean(data, start_index, pmc_mean);
        break;
      case SWING_ID:
        select_swing(data,start_index,swing);
        break;
      case GORILLA_ID:
        select_gorilla(data, start_index, gorilla, uncompressed_values);
        break;
      case POLYSWING_ID:
        select_poly_swing(data, start_index, polyswing);
        break;
    }
}
void select_pmc_mean(Selected_model* model, size_t start_index, Pmc_mean* pmc_mean){
  float value = get_model_pmcmean(pmc_mean);
  size_t end_index = start_index + get_length_pmcmean(pmc_mean) - 1;

  model->model_type_id = (uint8_t) PMC_MEAN_ID;
  model->values_capacity = 0;
  model->values[0] = 0;
  model->end_index = end_index;
  model->min_value = value;
  model->max_value = 0;
}

void select_swing(Selected_model* model, size_t start_index, Swing* swing){
  float arr[2] = {0,0};

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

void select_vector_based(Selected_model* model, Vector_based* vector_model){
  Bit_vec_builder bit_vec_builder;
  bit_vec_builder.current_byte = 0;
  bit_vec_builder.remaining_bits = 8;
  bit_vec_builder.bytes_counter = 0;
  bit_vec_builder.bytes_capacity = 4;
  bit_vec_builder.bytes = (uint8_t*) calloc (bit_vec_builder.bytes_capacity, sizeof(uint8_t));
  if(bit_vec_builder.bytes == NULL){
      printf("MALLOC ERROR (select_vector_based)\n");
  }

  model->values_capacity = 8;
  model->values = realloc(model->values, model->values_capacity * sizeof(*model->values));
  if(model->values == NULL){
      printf("REALLOC ERROR (selecPolySwing)\n");
  }

  model->min_value = vector_model->start.latitude;
  model->max_value = vector_model->start.longitude;

  int32_t lat_as_int = float_to_bit((float)vector_model->vec.y);
  int32_t long_as_int = float_to_bit((float)vector_model->vec.x);
  append_bits(&bit_vec_builder, lat_as_int, VALUE_SIZE_IN_BITS);
  append_bits(&bit_vec_builder, long_as_int, VALUE_SIZE_IN_BITS);

  for(int i = 0; i < 8; i++){
    model->values[i] = bit_vec_builder.bytes[i];
  }

  size_t end_index = vector_model->model_length - 1;
  model->model_type_id = (uint8_t) VECTOR_ID;
  model->end_index = end_index;
  free(bit_vec_builder.bytes );
}

void select_gorilla(Selected_model* model, size_t start_index, Gorilla* gorilla, float *uncompressed_values){
    size_t end_index = start_index + get_length_gorilla(gorilla) - 1;

    float max = uncompressed_values[start_index];
    float min = uncompressed_values[start_index];
    get_compressed_values(gorilla);
    for(int i = 0; i < gorilla->length; i++){
      float temp_val = uncompressed_values[i+start_index];
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
    
    model->values_capacity = gorilla->compressed_values.bytes_counter;

    model->values = realloc(model->values, model->values_capacity * sizeof(*model->values));
    if(model->values == NULL){
        printf("REALLOC ERROR (select_gorilla)\n");
    }

    memcpy(model->values, gorilla->compressed_values.bytes, model->values_capacity * sizeof(*model->values));

}

void select_poly_swing(Selected_model* model, size_t start_index, Poly_swing* poly_swing){
  size_t end_index = start_index + poly_swing->length - 1;
  float pow0 = poly_swing->current.pow0;
  float pow1 = poly_swing->current.pow1;
  Bit_vec_builder bit_vec_builder;
  bit_vec_builder.current_byte = 0;
  bit_vec_builder.remaining_bits = 8;
  model->values_capacity = 4;
  model->values = realloc(model->values, model->values_capacity * sizeof(*model->values));
  if(model->values == NULL){
      printf("REALLOC ERROR (selecPolySwing)\n");
  }
  bit_vec_builder.bytes_counter = 0;

  bit_vec_builder.bytes_capacity = 4;
  bit_vec_builder.bytes = (uint8_t*) calloc (bit_vec_builder.bytes_capacity, sizeof(uint8_t));
  if(bit_vec_builder.bytes == NULL){
      printf("MALLOC ERROR\n");
  }
  int32_t floatAsInt = float_to_bit((float)poly_swing->current.pow2);
  append_bits(&bit_vec_builder, floatAsInt, VALUE_SIZE_IN_BITS);
  for(int i = 0; i < 4; i++){
    model->values[i] = bit_vec_builder.bytes[i];
  }
  model->end_index = end_index;
  model->min_value = pow0;
  model->max_value = pow1;
  model->model_type_id = POLYSWING_ID;
  model->values_capacity = 4;
}

Selected_model get_selected_model(){
    Selected_model mod;
    mod.model_type_id = 0;
    mod.end_index = 0;
    mod.min_value = 0;
    mod.max_value = 0;
    mod.values_capacity = 1;
    mod.values = (uint8_t*) calloc (mod.values_capacity, sizeof(uint8_t));
    if(mod.values == NULL){
        printf("MALLOC ERROR (mod)\n");
    }
    return mod;
}

void reset_selected_model(Selected_model* model){
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

void delete_selected_model(Selected_model* model){
  free(model->values);
}