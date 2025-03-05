#include "HashTable.h"

uint64_t hash_func(uint64_t key)
{
    return key % NUM_OF_BUCKET;
}

struct Node *init_hash_tbl(){
    struct Node *new = (struct Node *)malloc(sizeof(struct Node) * NUM_OF_BUCKET);
    for (int i = 0; i < NUM_OF_BUCKET; i++){
        new[i].key = -1;
        new[i].val = -1;
        new[i].next = NULL;
    }
    
    return new;
}

struct Node *search_node(struct Node *tbl, uint64_t key){
    struct Node *head = &tbl[hash_func(key)];

    if (head->val > -1){
        do{
            if (head->key == key)
                return head;
            else{
                if (head->next != NULL)
                    head = head->next;
                else
                    return NULL;
            }
        } while (head->key != key);
    }
    else
        return NULL;
}


int add_node(struct Node *tbl, uint64_t key, int val){

    struct Node *head = &tbl[hash_func(key)];
    
    if (head->val > -1){
        do{
            if (head->key == key)
                return 0;
            else{
                if (head->next != NULL)
                    head = head->next;
                else{
                    struct Node *new = malloc(sizeof(struct Node));
                    new->key = key;
                    new->val = val;
                    new->next = NULL;

                    head->next = new;

                    return 1;
                }
            }
        } while (head->key != key);
    }
    else{
        head->key = key;
        head->val = val;

        return 1;
    }

}