struct PMCMean {
    float error;
    float minValue;
    float maxValue;
    float sumOfValues;
    float length;
};

int fitValue(struct PMCMean*, float);
int isValueWithinErrorBound(struct PMCMean*, float, float);
int equalOrNAN(float, float);
int isNan(float);
float get_bytes_per_value_pmc(struct PMCMean* data);
float get_model_pmcmean(struct PMCMean* data);