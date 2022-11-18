//
// Created by danie on 17-11-2022.
//

#ifndef COMPRESSEXEC_TIMESTAMPS_H
#define COMPRESSEXEC_TIMESTAMPS_H
#include <stdint.h>
#include "gorilla.h"
#include "math.h"

typedef struct Timestamps{
    uint8_t* compressed_time;
    int compressed_timestamp_count;
} Timestamps;

void free_timestamps(Timestamps* timestamps);
Timestamps compress_residual_timestamps(long* timestamps, long timestamp_count);

#endif //COMPRESSEXEC_TIMESTAMPS_H
