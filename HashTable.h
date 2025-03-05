#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_OF_BUCKET 200000

struct Node
{
    uint64_t key;
    int val;
    struct Node *next;
};


uint64_t hash_func(uint64_t key);
struct Node *search_node(struct Node *tbl, uint64_t key);
int add_node(struct Node *tbl, uint64_t key, int val);
struct Node *init_hash_tbl();