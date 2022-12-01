#ifndef BITREADER
#define BITREADER
#include <stdint.h>
#include <stdlib.h>
struct BitReader{
    uint8_t* bytes;
    int bytesCount;
    size_t nextBit;

}typedef BitReader;

BitReader tryNewBitreader(uint8_t* bytes, int byteCount);
uint32_t read_bits(BitReader* bitReader, uint8_t numberOfBits);
uint32_t read_bit(BitReader* bitReader);
float intToFloat(uint32_t value);
#endif