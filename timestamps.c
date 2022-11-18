//
// Created by danie on 17-11-2022.
//

#include "timestamps.h"
#include "gorilla.h"
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
    Bit_vec_builder compressed_timestamps;
    compressed_timestamps.current_byte = 0;
    compressed_timestamps.remaining_bits = 8;
    compressed_timestamps.bytes_counter = 0;
    compressed_timestamps.bytes_capacity = 4;
    compressed_timestamps.bytes = (uint8_t*) malloc (compressed_timestamps.bytes_capacity * sizeof(uint8_t));
    if(compressed_timestamps.bytes == NULL){
        printf("MALLOC ERROR (select_vector_based)\n");
    }
    append_a_one_bit(&compressed_timestamps);
    long last_delta = timestamps[1] - timestamps[0];

}

//fn compress_irregular_residual_timestamps(uncompressed_timestamps: &[Timestamp]) -> Vec<u8> {
//    let mut compressed_timestamps = BitVecBuilder::new();
//    compressed_timestamps.append_a_one_bit();
//
//    // Store the second timestamp as a delta using 14 bits.
//    let mut last_delta = uncompressed_timestamps[1] - uncompressed_timestamps[0];
//    compressed_timestamps.append_bits(last_delta as u32, 14); // 14-bit delta is max four hours.
//
//    // Encode the timestamps from the third timestamp to the second to last.
//    // A delta-of-delta is computed and then encoded in buckets of different
//    // sizes. Assumes that the delta-of-delta can fit in at most 32 bits.
//    let mut last_timestamp = uncompressed_timestamps[1];
//    for timestamp in &uncompressed_timestamps[2..uncompressed_timestamps.len() - 1] {
//        let delta = timestamp - last_timestamp;
//        let delta_of_delta = delta - last_delta;
//
//        match delta_of_delta {
//            0 => compressed_timestamps.append_a_zero_bit(),
//            -63..=64 => {
//                compressed_timestamps.append_bits(0b10, 2);
//                compressed_timestamps.append_bits(delta_of_delta as u32, 7);
//            }
//            -255..=256 => {
//                compressed_timestamps.append_bits(0b110, 3);
//                compressed_timestamps.append_bits(delta_of_delta as u32, 9);
//            }
//            -2047..=2048 => {
//                compressed_timestamps.append_bits(0b1110, 4);
//                compressed_timestamps.append_bits(delta_of_delta as u32, 12);
//            }
//            _ => {
//                compressed_timestamps.append_bits(0b1111, 4);
//                compressed_timestamps.append_bits(delta_of_delta as u32, 32);
//            }
//        }
//        last_delta = delta;
//        last_timestamp = *timestamp;
//    }
//
//    // All remaining bits in the byte the BitVecBuilder is currently packing
//    // bits into is set to one to indicate that all timestamps are decompressed.
//    compressed_timestamps.finish_with_one_bits()
//}