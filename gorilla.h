#ifndef GORILLA
#define GORILLA

#include <stdint.h>
#include <stdlib.h>
#include <math.h>


typedef unsigned char uint8;

struct BitVecBuilder {
    uint8 current_byte ;
    uint8 remaining_bits ;
    uint8* bytes;
    int bytes_capacity;
    int bytes_counter;
};

struct Gorilla {

    float last_value;
    uint8 last_leading_zero_bits;
    uint8 last_trailing_zero_bits;
    struct BitVecBuilder compressed_values;
    size_t length;

    //values used for lossy gorilla
    float start_value;
    int first_iteration;

};

void append_bits(struct BitVecBuilder* data, long bits, uint8 number_of_bits);
void append_a_zero_bit(struct BitVecBuilder* data);
void append_a_one_bit(struct BitVecBuilder* data);
void compress_value(struct Gorilla* data, float value);

uint8 leading_zeros(int32_t num);
uint8 trailing_zeros(int32_t num);
int32_t floatToBit(float val);

struct Gorilla;
struct BitVecBuilder;
struct Gorilla init();


#endif