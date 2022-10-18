#include "gorilla.h"
#include "swing.h"
#include "PMCMean.h"

struct SelectedModel {
    uint8 model_type_id;
    size_t end_index;
    float min_value;
    float max_value;
    int values_capacity;
    uint8* values;
};

struct bytes_per_value{
    uint8 id;
    float bytes;
}


struct SelectedModel init_selectedModel();