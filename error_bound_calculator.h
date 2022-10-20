struct running_mean{
    double value;
    int lenght;
} typedef running_mean;
float calculate_error_bound(running_mean mean,float lastValue, int segmentLengt);