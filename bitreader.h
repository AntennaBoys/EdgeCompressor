#ifndef BITREADER
#define BITREADER
#include <stdint.h>
struct BitReader{
    size_t nextBit;
    int bytesCount;
    uint8_t* bytes;
}typedef BitReader;

BitReader tryNewBitreader(uint8_t* bytes, int byteCount);
uint32_t read_bits(BitReader* bitReader, int number_of_bits);
#endif