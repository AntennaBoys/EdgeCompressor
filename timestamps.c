//
// Created by danie on 17-11-2022.
//

#include "timestamps.h"
const uint8_t SIZE_OF_LONG = (uint8_t) sizeof(long) * 8;

uint8_t* compress_regular_residual_timestamps(long* timestamps, long timestamp_count);

uint8_t* long_to_bytes(long value){
    uint8_t* result;
    result = malloc(8 * sizeof(*result));
    if(!result){
        printf("MALLOC ERROR (long_to_bytes)");
    }
    result[7] = value & 0xFF;
    result[6] = (value & 0xFF00) >> 8;
    result[5] = (value & 0xFF0000) >> 16;
    result[4] = (value & 0xFF000000) >> 24;
    result[3] = (value & 0xFF00000000) >> 32;
    result[2] = (value & 0xFF0000000000) >> 40;
    result[1] = (value & 0xFF000000000000) >> 48;
    result[0] = (value & 0xFF00000000000000) >> 52;
    return result;
}

// check for timestamp count before calling this function
// it needs to be more than 2 as the first and last timestamp already is saved
int uncompressed_timestamps_are_regular(long* timestamps, long timestamp_count){
    long interval = timestamps[0] - timestamps[1];
    long previous_timestamp = timestamps[0];
    for(int i = 1; i < timestamp_count; i++){
        if(timestamps[i] - previous_timestamp != interval){
            return 0;
        }
        previous_timestamp = timestamps[i];
    }
    return 1;
}

uint8_t* compress_residual_timestamps(long* timestamps, long timestamp_count){
    if(uncompressed_timestamps_are_regular(timestamps, timestamp_count)){
        compress_regular_residual_timestamps(timestamps, timestamp_count);
    }else{
        //handle irregular timestamps
    }
}

uint8_t* compress_regular_residual_timestamps(long* timestamps, long timestamp_count){
    uint8_t leading_zeroes = leading_zeros(timestamp_count);
    int number_of_bits_to_write = ((SIZE_OF_LONG*timestamp_count)-leading_zeroes)+1;
    uint8_t number_of_bytes_to_write = (uint8_t)ceilf(number_of_bits_to_write/8);
    uint8_t* result;
    result = malloc(number_of_bytes_to_write * sizeof(*result));
    if(!result){
        printf("MALLOC ERROR (compress_regular_residual_timestamps)");
    }
    uint8_t* timestamp_count_as_bytes = long_to_bytes(timestamp_count);
    int start_index = 8 - number_of_bytes_to_write;
    for(int i = 0; i < number_of_bytes_to_write; i++){
        result[i] = timestamp_count_as_bytes[start_index + i];
    }

    free(timestamp_count_as_bytes);
    return result;
}

uint8_t* compress_irregular_residual_timestamps(long* timestamps, long timestamp_count){
    
}