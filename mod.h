#ifndef MOD
#define MOD
#include "gorilla.h"
#include "swing.h"
#include "PMCMean.h"
#include "polyswing.h"
#include <stdio.h>
#include <string.h>

#define PMC_MEAN_ID 0
#define SWING_ID 1
#define GORILLA_ID 2
#define POLYSWING_ID 3

struct Selected_model {
    uint8_t model_type_id;
    size_t end_index;
    float min_value;
    float max_value;
    int values_capacity;
    uint8_t* values;
} typedef Selected_model;

struct bytes_per_value{
    uint8_t id;
    float bytes;
};

void select_gorilla(Selected_model* model, size_t start_index, Gorilla* gorilla, float *uncompressed_values);
void select_pmc_mean(Selected_model* model, size_t start_index, Pmc_mean* pmc_mean);
void select_swing(Selected_model* model, size_t start_index, Swing* swing);
void select_poly_swing(Selected_model* model, size_t start_index, Poly_swing* poly_swing);

void select_model(Selected_model* data, size_t start_index, Pmc_mean* pmc_mean, Swing* swing, Gorilla* gorilla, Poly_swing* polyswing, float *uncompressed_values);

Selected_model get_selected_model(void);
void reset_selected_model(Selected_model* model);
void delete_selected_model(Selected_model* model);
#endif