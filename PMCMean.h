struct PMCMean {
    float error;
    float minValue;
    float maxValue;
    float sumOfValues;
    float length;
};
int fitValue(struct PMCMean* data, float value);