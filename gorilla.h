#ifndef GORILLA
#define GORILLA

#include <stdint.h>
#include <stdlib.h>



struct BitVecBuilder {
    uint8_t current_byte ;
    uint8_t remaining_bits;
    int bytes_capacity;
    int bytes_counter;
    uint8_t* bytes;
};

struct Gorilla {

    float last_value;
    uint8_t last_leading_zero_bits;
    uint8_t last_trailing_zero_bits;
    struct BitVecBuilder compressed_values;
    size_t length;
};

void append_bits(struct BitVecBuilder* data, long bits, uint8_t number_of_bits);
void append_a_zero_bit(struct BitVecBuilder* data);
void append_a_one_bit(struct BitVecBuilder* data);
void fitValueGorilla(struct Gorilla* data, float value);
float get_bytes_per_value_gorilla(struct Gorilla* data);
size_t get_length_gorilla(struct Gorilla* data);
size_t len(struct BitVecBuilder* data);
uint8_t* get_compressed_values(struct Gorilla* data);

uint8_t leading_zeros(int32_t num);
uint8_t trailing_zeros(int32_t num);
int32_t floatToBit(float val);

struct Gorilla;
struct BitVecBuilder;
struct Gorilla getGorilla(void);
void resetGorilla(struct Gorilla* gorilla);
void deleteGorilla(struct Gorilla* gorilla);
float* gridGorilla(uint8_t* values, int valuesCount, int timestampCount);
#endif