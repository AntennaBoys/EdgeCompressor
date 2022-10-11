#include "gorilla.h"

struct SelectedModel {
    uint8 model_type_id;
    size_t end_index;
    float min_value;
    float max_value;
    int values_capacity;
    uint8* values;
};

struct SelectedModel init_selectedModel();