//
// Created by danie on 17-11-2022.
//

#include "timestamps.h"
#include "gorilla.h"
const uint8_t SIZE_OF_LONG = (uint8_t) sizeof(long) * 8;

Timestamps compress_regular_residual_timestamps(long* timestamps, long timestamp_count);
Timestamps compress_irregular_residual_timestamps(long* timestamps, long timestamp_count);

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

Timestamps compress_residual_timestamps(long* timestamps, long timestamp_count){
    if(timestamp_count <= 2){
        return (Timestamps){.compressed_timestamp_count=0};
    }
    if(uncompressed_timestamps_are_regular(timestamps, timestamp_count)){
        return compress_regular_residual_timestamps(timestamps, timestamp_count);
    }else{
        return compress_irregular_residual_timestamps(timestamps, timestamp_count);
    }
}

Timestamps compress_regular_residual_timestamps(long* timestamps, long timestamp_count){
    uint8_t leading_zeroes = leading_zeros(timestamp_count);
    int number_of_bits_to_write = ((SIZE_OF_LONG*timestamp_count)-leading_zeroes)+1;
    uint8_t number_of_bytes_to_write = (uint8_t)ceilf(number_of_bits_to_write/8);
    Timestamps result;
    result.compressed_time = malloc(number_of_bytes_to_write * sizeof(*result.compressed_time));
    if(!result.compressed_time){
        printf("MALLOC ERROR (compress_regular_residual_timestamps)");
    }
    uint8_t* timestamp_count_as_bytes = long_to_bytes(timestamp_count);
    int start_index = 8 - number_of_bytes_to_write;
    for(int i = 0; i < number_of_bytes_to_write; i++){
        result.compressed_time[i] = timestamp_count_as_bytes[start_index + i];
    }

    free(timestamp_count_as_bytes);
    result.compressed_timestamp_count = number_of_bytes_to_write;
    return result;
}

Timestamps compress_irregular_residual_timestamps(long* timestamps, long timestamp_count){
    Bit_vec_builder compressed_timestamps;
    compressed_timestamps.current_byte = 0;
    compressed_timestamps.remaining_bits = 8;
    compressed_timestamps.bytes_counter = 0;
    compressed_timestamps.bytes_capacity = 4;
    compressed_timestamps.bytes = (uint8_t*) malloc (compressed_timestamps.bytes_capacity * sizeof(uint8_t));
    if(compressed_timestamps.bytes == NULL){
        printf("MALLOC ERROR (compress_irregular_residual_timestamps)\n");
    }
    append_a_one_bit(&compressed_timestamps);
    long last_delta = timestamps[1] - timestamps[0];
    append_bits(&compressed_timestamps, last_delta, 14);
    long last_timestamp = timestamps[1];
    for(int i = 2; i < timestamp_count-1; i++){
        long timestamp = timestamps[i];
        long delta = timestamp - last_timestamp;
        long delta_of_delta = delta - last_delta;
        if(delta_of_delta == 0){
            append_a_zero_bit(&compressed_timestamps);
        }else if(delta_of_delta >= -63 && delta_of_delta <= 64){
            append_bits(&compressed_timestamps, 0b10, 2);
            append_bits(&compressed_timestamps, delta_of_delta, 7);
        }else if(delta_of_delta >= -255 && delta_of_delta <= 256){
            append_bits(&compressed_timestamps, 0b110, 3);
            append_bits(&compressed_timestamps, delta_of_delta, 9);
        }else if(delta_of_delta >= -2047 && delta_of_delta <= 2048){
            append_bits(&compressed_timestamps, 0b1110, 4);
            append_bits(&compressed_timestamps, delta_of_delta, 12);
        }else{
            append_bits(&compressed_timestamps, 0b1111, 4);
            append_bits(&compressed_timestamps, delta_of_delta, 32);
        }
        last_delta = delta;
        last_timestamp = timestamp;
    }
    Timestamps result = (Timestamps){.compressed_time = finish_with_one_bits(&compressed_timestamps),
                                     .compressed_timestamp_count = compressed_timestamps.bytes_counter};
    return result;
}

void free_timestamps(Timestamps* timestamps){
    free(timestamps->compressed_time);
}