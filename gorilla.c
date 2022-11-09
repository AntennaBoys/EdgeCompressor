#include "gorilla.h"



const int debug = 0;

const uint8_t SIZE_OF_32INT = (uint8_t) sizeof(int32_t) * 8;

uint8_t leading_zeros(int32_t num){
    // Equivalent to
    // 10000000 00000000 00000000 00000000
    int msb = 1 << (SIZE_OF_32INT - 1);

    uint8_t count = 0;

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
uint8_t trailing_zeros(int32_t num){
    
    uint8_t count = 0;

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


void fitValueGorilla(struct Gorilla* data, float value){
    int32_t value_as_integer = floatToBit(value); // Læs den binære repræsentation af float value som en integer, som vi herefter kan lave bitwise operationer på
    int32_t last_value_as_integer = floatToBit(data->last_value);
    int32_t value_xor_last_value = value_as_integer ^ last_value_as_integer;
    
    if(data->compressed_values.bytes_counter == 0){
        // TODO: &(data->compressed_values) ?????????????????
        //printf("====================\n");
        append_bits(&data->compressed_values, value_as_integer, VALUE_SIZE_IN_BITS);

    } else if (value_xor_last_value == 0){
        append_a_zero_bit(&data->compressed_values);
        // printf("ZERO BIT\n");
    } else {
        uint8_t leading_zero_bits = leading_zeros(value_xor_last_value);
        uint8_t trailing_zero_bits = trailing_zeros(value_xor_last_value);
        append_a_one_bit(&data->compressed_values); //???????????????????

        if(leading_zero_bits >= data->last_leading_zero_bits
            && trailing_zero_bits >= data->last_trailing_zero_bits)
        {
            append_a_zero_bit(&data->compressed_values);
            uint8_t meaningful_bits = VALUE_SIZE_IN_BITS 
                - data->last_leading_zero_bits 
                - data->last_trailing_zero_bits;
            append_bits(&data->compressed_values, 
                value_xor_last_value >> data->last_trailing_zero_bits, 
                meaningful_bits
            );

        } else {
            append_a_one_bit(&data->compressed_values);
            append_bits(&data->compressed_values, leading_zero_bits, 5);
            uint8_t meaningful_bits = VALUE_SIZE_IN_BITS - leading_zero_bits - trailing_zero_bits;
            append_bits(&data->compressed_values, meaningful_bits, 6);
            append_bits(&data->compressed_values, value_xor_last_value >> trailing_zero_bits, meaningful_bits);

            data->last_leading_zero_bits = leading_zero_bits;
            data->last_trailing_zero_bits = trailing_zero_bits;

        }
        // uint8_t leading_zero_bits = value_xor_last_value

    }
    
    data->last_value = value;
    data->length++;
}

float get_bytes_per_value_gorilla(struct Gorilla* data){
    return (float) len(&data->compressed_values) / (float) data->length;
}

size_t len(struct BitVecBuilder* data){
    return data->bytes_counter + (size_t) (data->remaining_bits != 8);
}

uint8_t* finish(struct BitVecBuilder* data){
  if (data->remaining_bits != 8){
    data->bytes_capacity++;
    data->bytes = realloc(data->bytes, 4 * data->bytes_capacity * sizeof(*data->bytes));
    data->bytes[data->bytes_counter++] = data->current_byte;
  }
  return data->bytes;
}

uint8_t* get_compressed_values(struct Gorilla* data){
  return finish(&data->compressed_values);
}

size_t get_length_gorilla(struct Gorilla* data){
    return data->length;
}





void append_a_zero_bit(struct BitVecBuilder* data){
    append_bits(data, 0, 1);
}

void append_a_one_bit(struct BitVecBuilder* data){
    append_bits(data, 1, 1);
}


void append_bits(struct BitVecBuilder* data, long bits, uint8_t number_of_bits){
    uint8_t _number_of_bits = number_of_bits;

    while(_number_of_bits > 0){
        uint8_t bits_written;

        if(_number_of_bits > data->remaining_bits){
            uint8_t shift = _number_of_bits -  data->remaining_bits;
            data -> current_byte |= (uint8_t)((bits >> shift) & ((1 << data->remaining_bits) - 1));
            bits_written = data->remaining_bits;
        } else {
            uint8_t shift = data->remaining_bits - _number_of_bits;
            data -> current_byte |= (uint8_t)(bits << shift);
            bits_written = _number_of_bits;
        }
        _number_of_bits -= bits_written;
        data->remaining_bits -= bits_written;

        if(data->remaining_bits == 0){

            //is this correct? probs
            //printf("%d\n", 4 * data->bytes_capacity * sizeof(uint8_t));
            data->bytes_capacity++;

            data->bytes = realloc(data->bytes, 4 * data->bytes_capacity * sizeof(*data->bytes));
            if(data->bytes == NULL){
                printf("REALLOC ERROR(append_bits)\n");
            }

            data->bytes[data->bytes_counter] = data->current_byte;
            data->bytes_counter = data->bytes_counter+1;
            data->current_byte = 0;
            data->remaining_bits = 8;   
        }
    }
}

struct Gorilla getGorilla(){
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
    data.compressed_values.bytes = (uint8_t*) malloc (data.compressed_values.bytes_capacity * sizeof(uint8_t));
    if(data.compressed_values.bytes == NULL){
        printf("MALLOC ERROR\n");
    }
    // for(int i = 0; i < sizeof(data.compressed_values.bytes)/sizeof(data.compressed_values.bytes[0]); i++){
    //     data.compressed_values.bytes[i] = NULL;
    // }
    data.length = 0;
    
    return data;
}

void resetGorilla(struct Gorilla* gorilla){
    gorilla->last_leading_zero_bits = UCHAR_MAX;
    gorilla->last_trailing_zero_bits = 0;
    gorilla->last_value = 0;
    gorilla->length = 0;
    
    gorilla->compressed_values.current_byte = 0;
    gorilla->compressed_values.remaining_bits = 8;
    gorilla->compressed_values.bytes_capacity = 1;
    gorilla->compressed_values.bytes_counter = 0;
    gorilla->compressed_values.bytes = realloc(gorilla->compressed_values.bytes, 4 * gorilla->compressed_values.bytes_capacity * sizeof(*gorilla->compressed_values.bytes));
    if(gorilla->compressed_values.bytes == NULL){
        printf("REALLOC ERROR\n");
    }
}

void deleteGorilla(struct Gorilla* gorilla){
    free(gorilla->compressed_values.bytes);
}


float* gridGorilla(uint8_t* values, int valuesCount, int timestampCount){
    float* result;
    result = malloc(timestampCount * sizeof(*result));
    if(!result){
        printf("CALLOC ERROR (gridGorilla: result)\n");
    }
    int resultIndex = 0;
    BitReader bitReader = tryNewBitreader(values, valuesCount);
    int leadingZeros = 255;
    int trailingZeros = 0;
    uint32_t lastValue = read_bits(&bitReader, VALUE_SIZE_IN_BITS);
    result[resultIndex++] = intToFloat(lastValue);
    for(int i = 0; i < timestampCount-1; i++){
        if(read_bit(&bitReader)){
            if(read_bit(&bitReader)){
                leadingZeros = read_bits(&bitReader, 5);
                uint8_t meaningfulBits = read_bits(&bitReader, 6);
                trailingZeros = VALUE_SIZE_IN_BITS - meaningfulBits - leadingZeros;
            }

            uint8_t meaningfulBits = VALUE_SIZE_IN_BITS - leadingZeros - trailingZeros;
            uint32_t value = read_bits(&bitReader, meaningfulBits);
            value <<= trailingZeros;
            value ^= lastValue;
            lastValue = value;
        }
        result[resultIndex++] = intToFloat(lastValue);
    }
    return result;
}

