#include "gorilla.h"
#include "swing.h"
#include "PMCMean.h"
#include "mod.h"
#include <stdio.h>
#include "vector_based.h"
int main(){

    double lo1 = 10, lat1 = 20;
    double lo2 = 11, lat2 = 21;
    double lo3 = 12, lat3 = 26;
    double lo4 = 13, lat4 = 23;
    Vector_based vb = getVector_based();
    printf("fit: %d\n", fitValuesVectorBased(&vb, 1, lat1, lo1));
    printf("fit: %d\n", fitValuesVectorBased(&vb, 2, lat2, lo2));
    printf("fit: %d\n", fitValuesVectorBased(&vb, 3, lat3, lo3));
    printf("fit: %d\n", fitValuesVectorBased(&vb, 4, lat4, lo4));

    getchar();
   
}

