#ifndef PMCMEAN
#define PMCMEAN

struct PMCMean {
    float error;
    float minValue;
    float maxValue;
    float sumOfValues;
    float length;
};

int fitValuePMC(struct PMCMean *data, float value);
int isValueWithinErrorBound(struct PMCMean*, float, float);
int equalOrNAN(float, float);
int isNan(float);
float get_bytes_per_value_pmc(struct PMCMean* data);
float get_model_pmcmean(struct PMCMean* data);
size_t get_length_pmcmean (struct PMCMean* data);

struct PMCMean getPMCMean(double errorBound);
void resetPMCMean(struct PMCMean *pmc);
float* gridPMCMean(struct SelectedModel model, int timestampCount);

#endif