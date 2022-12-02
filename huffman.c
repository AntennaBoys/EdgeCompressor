//
// Created by danie on 02-12-2022.
// Adapted from https://www.programiz.com/dsa/huffman-coding
//

#include "huffman.h"
#include "stdio.h"
#include <stdlib.h>
// Create nodes
Huffman_node *new_node(char item, uint32_t count) {
    struct Huffman_node *node = calloc(1 , sizeof(Huffman_node));
    if(!node){
        printf("Calloc error (New Node)");
    }
    node->left = NULL;
    node->right = NULL;
    node->item = item;
    node->count = count;
    return node;
}

Min_heap *create_heap(uint32_t capacity){
    Min_heap *heap = calloc(1 ,sizeof(Min_heap));
    heap->size = capacity;
    heap->capacity = capacity;
    heap->nodes = calloc(capacity, sizeof(Huffman_node *));
    return heap;
}

void swap_nodes(Huffman_node** a, Huffman_node** b){
    Huffman_node *temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_heap(Min_heap* heap, int index){
    int smallest = index;
    int left_index = 2 * index + 1;
    int right_index = 2 * index + 2;
    if(left_index < heap->size && heap->nodes[left_index]->count < heap->nodes[smallest]->count){
        smallest = left_index;
    }
    if(right_index < heap->size && heap->nodes[right_index]->count < heap->nodes[smallest]->count){
        smallest = right_index;
    }
    if(smallest != index){
        swap_nodes(&heap->nodes[smallest], &heap->nodes[index]);
        heapify_heap(heap, smallest);
    }
}

Huffman_node *get_min_node(Min_heap* heap){
    Huffman_node *node = heap->nodes[0];
    heap->nodes[0] = heap->nodes[heap->size-1];
    heap->size--;
    heapify_heap(heap, 0);
    return node;
}

void insert_to_heap(Min_heap *heap, Huffman_node *min_node){
    heap->size++;
    int i = heap->size - 1;
    while(i && min_node.count < heap->nodes[(i-1)/2]->count){
        heap->nodes[i] = heap->nodes[(i-1)/2];
        i = (i-1)/2;
    }
}

void build_heap(Min_heap *heap){
    int last_index = heap->size-1;
    for(int i = (last_index-1)/2; i > 0; i--){
        heapify_heap(heap, i);
    }
}

int node_is_leaf(Huffman_node *node){
    return !(node->left) && !(node->right);
}

Min_heap *create_and_build_heap(char* item_array, int* count_array, int size){
    Min_heap *heap = create_heap(size);
    for(int i = 0; i < size; i++){
        heap->nodes[i] = new_node(item_array[i], count_array[i]);
    }
    build_heap(heap);
    return heap;
}

Huffman_node *build_huffman_tree(char* item_array, int* count_array, int size){
    Huffman_node *top;
    Huffman_node *left;
    Huffman_node *right;
    Min_heap *heap = create_and_build_heap(item_array, count_array, size);
    while(heap->size != 1){
        left = get_min_node(heap);
        right = get_min_node(heap);
        top = new_node('!', left->count + right->count);
        top->left = left;
        top->right = right;
        insert_to_heap(heap, top);
    }
    return get_min_node(heap);
}