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
};

void select_gorilla(struct SelectedModel* model, size_t start_index, struct Gorilla* gorilla, float *uncompressed_values);
void select_pmc_mean(struct SelectedModel* model, size_t start_index, struct PMCMean* pmcmean);
void select_swing(struct SelectedModel* model, size_t start_index, struct swing* swing);

struct SelectedModel init_selectedModel();