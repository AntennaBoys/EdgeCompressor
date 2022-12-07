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
    while(i && min_node->count < heap->nodes[(i-1)/2]->count){
        heap->nodes[i] = heap->nodes[(i-1)/2];
        i = (i-1)/2;
    }
    heap->nodes[i] = min_node;
}

void build_heap(Min_heap *heap){
    int last_index = heap->size-1;
    for(int i = (last_index-1)/2; i >= 0; i--){
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

void print_huffman_codes(FILE* output, Huffman_node *root, int *array, int top, int* first){
    if(root->left){
        array[top] = 0;
        print_huffman_codes(output, root->left, array, top + 1, first);
    }
    if(root->right){
        array[top] = 1;
        print_huffman_codes(output, root->right, array, top + 1, first);
    }
    if(node_is_leaf(root)){
        int code = 0;
        if(*first){
            fprintf(output,"%c:", root->item);
            *first = 0;
        }else{
            fprintf(output,",%c:", root->item);
        }
        for(int i = 0; i < top; i++){
            if(array[i]){
                code++;
                code = code << 1;
            }else{
                code = code << 1;
            }
        }
        fprintf(output,"%d", code);
    }
}

int get_binary_for_char(Huffman_node* root, int *array, int top, char c, Binary_storage* result){
    int done = 0;
    if(root->left){
        array[top] = 0;
        done = get_binary_for_char(root->left, array, top + 1, c, result);
        if(done){
            return 1;
        }
    }
    if(root->right){
        array[top] = 1;
        done = get_binary_for_char(root->right, array, top + 1, c, result);
        if(done){
            return 1;
        }
    }
    if(node_is_leaf(root) && root->item == c){
        result->bit_count=top;
        for(int i = 0; i < top; i++){
            result->bits[i] = array[i];
        }
        return 1;
    }else{
        return 0;
    }
}

int count_occurences(int** count_list, char ** item_list, char* input_file){
    char line[1024];
    int line_index = 0;
    int counts[256] = {0};
    FILE* input = fopen(input_file,"r");
    while(fgets(line, 1024, input)){
        line_index++;
        for(int i = 0; line[i]; i++){
            counts[line[i]]++;
        }
    }
    fclose(input);
    int overall_count = 0;
    *count_list = calloc(1, sizeof(**count_list));
    *item_list = calloc(1, sizeof(**item_list));
    for(int i = 0; i < 256; i++){
        if(counts[i] != 0){
            overall_count++;
            *count_list = realloc(*count_list, overall_count * sizeof(**count_list));
            *item_list = realloc(*item_list, overall_count * sizeof(**item_list));
            (*count_list)[overall_count-1] = counts[i];
            (*item_list)[overall_count - 1] = (char) (i);
        }
    }
    return overall_count;
}
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}
int main() {
    char *items;
    int *counts;
    char* input_file_path = "C:\\Users\\danie\\Documents\\GitHub\\ShipMapPoints\\output.csv";
    FILE *output = fopen("C:\\Users\\danie\\Documents\\GitHub\\ShipMapPoints\\output_compressed.txt","w+");

    int size = count_occurences(&counts, &items,input_file_path);

    Huffman_node *nodes = build_huffman_tree(items, counts, size);
    int array[50];
    int first = 1;
    print_huffman_codes(output, nodes, array, 0, &first);
    fprintf(output, "\n");
    FILE* input = fopen(input_file_path,"r");
    char c;
    int bit_size = 0;
    unsigned char char_out = 0;
    int char_out_size = 0;
    while((c = (char)fgetc(input)) != EOF){
        Binary_storage binary = {.bits = {0}, .bit_count = 0};
        if(get_binary_for_char(nodes, array, 0, c, &binary)){
            for(int i = 0; i < binary.bit_count; i++){
                char_out += binary.bits[i];
                if(char_out_size == 7){
                    fprintf(output, "%c", char_out);
                    char_out_size = 0;
                    char_out = 0;
                }else{
                    char_out = char_out << 1;
                    char_out_size++;
                }
            }
        }
    }
    char_out = char_out << (7 - char_out_size);
    fprintf(output, "%c", char_out);
    fclose(input);
    fclose(output);
}