#ifndef GORILLA
#define GORILLA

#include <stdint.h>
#include <stdlib.h>
#include "bitreader.h"
#include "constants.h"
#include <stdio.h>
#include <limits.h>

struct Bit_vec_builder {
    uint8_t current_byte ;
    uint8_t remaining_bits;
    int bytes_capacity;
    int bytes_counter;
    uint8_t* bytes;
} typedef Bit_vec_builder;

struct Gorilla {

    float last_value;
    uint8_t last_leading_zero_bits;
    uint8_t last_trailing_zero_bits;
    Bit_vec_builder compressed_values;
    size_t length;
} typedef Gorilla;

void append_bits(Bit_vec_builder* data, long bits, uint8_t number_of_bits);
void append_a_zero_bit(Bit_vec_builder* data);
void append_a_one_bit(Bit_vec_builder* data);
void fitValueGorilla(Gorilla* data, float value);
float get_bytes_per_value_gorilla(Gorilla* data);
size_t get_length_gorilla(Gorilla* data);
size_t len(Bit_vec_builder* data);
uint8_t* get_compressed_values(Gorilla* data);

uint8_t leading_zeros(uint32_t num);
uint8_t trailing_zeros(uint32_t num);
uint32_t float_to_bit(float val);

Gorilla get_gorilla(void);
void reset_gorilla(Gorilla* gorilla);
void delete_gorilla(Gorilla* gorilla);
float* grid_gorilla(uint8_t* values, int values_count, int timestamp_count);
uint8_t* finish_with_one_bits(Bit_vec_builder* data);
#endif