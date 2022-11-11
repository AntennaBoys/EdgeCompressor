#ifndef PMCMEAN
#define PMCMEAN
#include <stdint.h>
#include <stdlib.h>
struct Pmc_mean {
    double error;
    float minValue;
    float maxValue;
    float sumOfValues;
    size_t length;
} typedef Pmc_mean;

int fitValuePMC(Pmc_mean *data, float value);
int isValueWithinErrorBound(Pmc_mean*, float, float);
int equalOrNAN_pmc(float, float);
int isNan_pmc(float);
float get_bytes_per_value_pmc(Pmc_mean* data);
float get_model_pmcmean(Pmc_mean* data);
size_t get_length_pmcmean (Pmc_mean* data);

Pmc_mean getPMCMean(double errorBound);
void resetPMCMean(Pmc_mean *pmc);
float* gridPMCMean(float value, int timestampCount);

#endif