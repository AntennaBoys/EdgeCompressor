

#include <stdio.h>
#include <limits.h>

#include "gorilla.h"

const int debug = 0;

const uint8 VALUE_SIZE_IN_BYTES = (uint8) sizeof(float);
const uint8 VALUE_SIZE_IN_BITS = (uint8) (8 * (uint8) sizeof(float));

const uint8 SIZE_OF_32INT = (uint8) sizeof(int32_t) * 8;

uint8 leading_zeros(int32_t num){
    // Equivalent to
    // 10000000 00000000 00000000 00000000
    int msb = 1 << (SIZE_OF_32INT - 1);

    uint8 count = 0;

    /* Iterate over each bit */
    for(int i=0; i<SIZE_OF_32INT; i++)
    {
        /* If leading set bit is found */
        if((num << i) & msb)
        {
            /* Terminate the loop */
            break;
        }

        count++;
    }

    if(debug) printf("Total number of leading zeros in %d is %d", num, count);
    return count;
}
uint8 trailing_zeros(int32_t num){
    
    uint8 count = 0;

    /* Iterate over each bit of the number */
    for(int i=0; i< SIZE_OF_32INT;  i++)
    {
        /* If set bit is found the terminate from loop*/
        if((num >> i ) & 1)
        {
            /* Terminate from loop */
            break;
        }

        /* Increment trailing zeros count */
        count++;
    }

    if(debug) printf("Total number of trailing zeros in %d is %d.", num, count);
    return count;
}

int32_t floatToBit(float val){
    return *(int32_t *)&val;
}


struct Gorilla init(){
    struct Gorilla data;
    data.last_value = 0;
    data.last_leading_zero_bits = UCHAR_MAX;
    data.last_trailing_zero_bits = 0;

    data.compressed_values.current_byte = 0;
    data.compressed_values.remaining_bits = 8;

    //Initialise bytes array to NULL values
    data.compressed_values.bytes_counter = 0;
    
    //is this correct?
    data.compressed_values.bytes_capacity = 1;
    data.compressed_values.bytes = (uint8*) malloc (data.compressed_values.bytes_capacity * sizeof(uint8));
    if(data.compressed_values.bytes == NULL){
        printf("MALLOC ERROR\n");
    }
    // for(int i = 0; i < sizeof(data.compressed_values.bytes)/sizeof(data.compressed_values.bytes[0]); i++){
    //     data.compressed_values.bytes[i] = NULL;
    // }
    data.length = 0;
    
    return data;
}

void compress_value(struct Gorilla* data, float value){
    int32_t value_as_integer = floatToBit(value); // Læs den binære repræsentation af float value som en integer, som vi herefter kan lave bitwise operationer på
    int32_t last_value_as_integer = floatToBit(data->last_value);
    int32_t value_xor_last_value = value_as_integer ^ last_value_as_integer;
    
    if(data->compressed_values.bytes_counter == 0){
        // TODO: &(data->compressed_values) ?????????????????
        printf("====================\n");
        append_bits(&data->compressed_values, value_as_integer, VALUE_SIZE_IN_BITS);

    } else if (value_xor_last_value == 0){
        append_a_zero_bit(&data->compressed_values);
        // printf("ZERO BIT\n");
    } else {
        uint8 leading_zero_bits = leading_zeros(value_xor_last_value);
        uint8 trailing_zero_bits = trailing_zeros(value_xor_last_value);
        append_a_one_bit(&data->compressed_values); //???????????????????

        if(leading_zero_bits >= data->last_leading_zero_bits
            && trailing_zero_bits >= data->last_trailing_zero_bits)
        {
            append_a_zero_bit(&data->compressed_values);
            uint8 meaningful_bits = VALUE_SIZE_IN_BITS 
                - data->last_leading_zero_bits 
                - data->last_trailing_zero_bits;
            append_bits(&data->compressed_values, 
                value_xor_last_value >> data->last_trailing_zero_bits, 
                meaningful_bits
            );

        } else {
            append_a_one_bit(&data->compressed_values);
            append_bits(&data->compressed_values, leading_zero_bits, 5);
            uint8 meaningful_bits = VALUE_SIZE_IN_BITS - leading_zero_bits - trailing_zero_bits;
            append_bits(&data->compressed_values, meaningful_bits, 6);
            append_bits(&data->compressed_values, value_xor_last_value >> trailing_zero_bits, meaningful_bits);

            data->last_leading_zero_bits = leading_zero_bits;
            data->last_trailing_zero_bits = trailing_zero_bits;

        }
        // uint8 leading_zero_bits = value_xor_last_value

    }
    
    data->last_value = value;
    data->length++;
}






void append_a_zero_bit(struct BitVecBuilder* data){
    append_bits(data, 0, 1);
}

void append_a_one_bit(struct BitVecBuilder* data){
    append_bits(data, 1, 1);
}


void append_bits(struct BitVecBuilder* data, long bits, uint8 number_of_bits){
    uint8 _number_of_bits = number_of_bits;

    while(_number_of_bits > 0){
        uint8 bits_written;

        if(_number_of_bits > data->remaining_bits){
            uint8 shift = _number_of_bits -  data->remaining_bits;
            data -> current_byte |= (uint8)((bits >> shift) & ((1 << data->remaining_bits) - 1));
            bits_written = data->remaining_bits;
        } else {
            uint8 shift = data->remaining_bits - _number_of_bits;
            data -> current_byte |= (uint8)(bits << shift);
            bits_written = _number_of_bits;
        }
        _number_of_bits -= bits_written;
        data->remaining_bits -= bits_written;

        if(data->remaining_bits == 0){

            //is this correct? probs
            //printf("%d\n", 4 * data->bytes_capacity * sizeof(uint8));
            data->bytes_capacity++;

            data->bytes = realloc(data->bytes, 4 * data->bytes_capacity * sizeof(uint8));

            if(data->bytes == NULL){
                printf("REALLOC ERROR\n");
            }

            data->bytes[data->bytes_counter] = data->current_byte;
            data->bytes_counter = data->bytes_counter+1;
            data->current_byte = 0;
            data->remaining_bits = 8;   
        }
    }

    //data->bytes_counter = 0;
}