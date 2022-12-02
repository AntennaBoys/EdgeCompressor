//
// Created by danie on 02-12-2022.
//

#ifndef COMPRESSEXEC_HUFFMAN_H
#define COMPRESSEXEC_HUFFMAN_H
#include <stdint.h>

typedef struct Huffman_node{
    uint32_t count;
    struct Huffman_node* left;
    struct Huffman_node* right;
    char item;
}Huffman_node;

typedef struct Min_heap{
    Huffman_node **nodes;
    uint32_t size;
    uint32_t capacity;
} Min_heap;
#endif //COMPRESSEXEC_HUFFMAN_H
