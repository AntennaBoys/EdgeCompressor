#ifndef PMCMEAN
#define PMCMEAN
#include <stdint.h>
#include <stdlib.h>
struct PMCMean {
    double error;
    float minValue;
    float maxValue;
    float sumOfValues;
    size_t length;
};

int fitValuePMC(struct PMCMean *data, float value);
int isValueWithinErrorBound(struct PMCMean*, float, float);
int equalOrNAN_pmc(float, float);
int isNan_pmc(float);
float get_bytes_per_value_pmc(struct PMCMean* data);
float get_model_pmcmean(struct PMCMean* data);
size_t get_length_pmcmean (struct PMCMean* data);

struct PMCMean getPMCMean(double errorBound);
void resetPMCMean(struct PMCMean *pmc);
float* gridPMCMean(float value, int timestampCount);

#endif