#ifndef MOD
#define MOD

#include <stdint.h>
#include <stdlib.h>

struct SelectedModel {
    uint8_t model_type_id;
    size_t end_index;
    float min_value;
    float max_value;
    int values_capacity;
    uint8_t* values;
};

struct SelectedModel init_selectedModel();

#endif