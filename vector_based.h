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

    long int startTime;
    long int endTime;
    unsigned int length;
    Vector vec;
    
    int currentDelta;
    int prevDelta;
    
} Vector_based;

Vector_based getVector_based();
int fitValuesVectorBased(Vector_based *data, long timeStamp, double latitude, double longitude);


#endif