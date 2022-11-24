#ifndef VECTOR_BASED
#define VECTOR_BASED

typedef struct Position {
    double latitude;
    double longitude;
} Position;

typedef struct Vector {
    double x;
    double y;
} Vector;

typedef struct Vector_based {
    Position prev;
    Position current;
    Position start;

    long int start_time;
    long int end_time;
    unsigned int length;
    unsigned int model_length;
    Vector vec;
    
    int current_delta;
    long* timestamps;
    int max_timestamps;
    int current_timestamp_index;
    
} Vector_based;

Vector_based get_vector_based();
int fit_values_vector_based(Vector_based *data, long time_stamp, double latitude, double longitude, float error);
void reset_vector_based(Vector_based* vb);
void free_vectorbased(Vector_based* vb);

#endif