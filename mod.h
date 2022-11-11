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

struct SelectedModel {
    uint8_t model_type_id;
    size_t end_index;
    float min_value;
    float max_value;
    int values_capacity;
    uint8_t* values;
};

struct bytes_per_value{
    uint8_t id;
    float bytes;
};

void select_gorilla(struct SelectedModel* model, size_t start_index, struct Gorilla* gorilla, float *uncompressed_values);
void select_pmc_mean(struct SelectedModel* model, size_t start_index, struct PMCMean* pmcmean);
void select_swing(struct SelectedModel* model, size_t start_index, Swing* swing);
void selectPolySwing(struct SelectedModel* model, size_t start_index, Poly_swing* polySwing);

void selectModel(struct SelectedModel* data, size_t start_index, struct PMCMean* pmcmean, Swing* swing, struct Gorilla* gorilla, Poly_swing* polyswing, float *uncompressed_values);

struct SelectedModel getSelectedModel(void);
void resetSelectedModel(struct SelectedModel* model);
void deleteSelectedModel(struct SelectedModel* model);
#endif