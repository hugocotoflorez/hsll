#ifndef PT_H
#define PT_H

/* Prefix Tree
 *
 * Author: Hugo Coto Florez
 *
 * What is that:
 *    Each node contains the data associated
 *    with this prefix if VALID bit is 1. Each
 *    level L contains the key at index L.
 */

#include "vshkh.h"

typedef  Keybind PT_data;

/* Max splits from same prefix.
 * For level 1, 2, other */
#define MAX_BRANCHES 8
#define MAX_BRANCHES_L1 64
#define MAX_BRANCHES_L2 32

typedef struct PT_node {
        struct PT_node *parent;
        struct PT_node *branches;
        const char *key;
        unsigned level : 15;
        unsigned valid : 1;
        PT_data data;
} PT_node;

#define PT_INVALID INVALID_KB

typedef int (*PT_hash)(int max_branches, int level, const char *key);

/* Create and return a new PT */
PT_node PT_create();
/* Get the data at KEY or PT_INVALID from BASE PT */
PT_data PT_get(PT_node *const base, const char *key, PT_hash hash_func);
/* Add DATA to KEY at BASE PT */
void PT_add(PT_node *base, PT_data data, const char *key, PT_hash hash_func);
/* Destroy BASE PT */
void PT_destroy(PT_node *base);


#endif
