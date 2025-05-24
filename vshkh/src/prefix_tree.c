#include "../include/prefix_tree.h"
#include <stdlib.h>
#include <string.h>

/* Prefix Tree
 *
 * Author: Hugo Coto Florez
 * Github: https://github.com/hugocotoflorez
 *
 * About: "Generic" prefix tree implementation.
 * Designed to be used in vshkh to manage
 * multi-keypress aliases. */

/* Get num of branches given the level */
static inline int
get_branches(int level)
{
        return level == 0 ? MAX_BRANCHES_L1 :
               level == 1 ? MAX_BRANCHES_L2 :
                            MAX_BRANCHES;
}

/* Create a new array o braches */
static PT_node *
create_branches(PT_node *base)
{
        base->branches = malloc(sizeof *base * get_branches(base->level));
        for (int i = 0; i < get_branches(base->level); i++)
                base->branches[i] = (PT_node) {
                        .branches = NULL,
                        .level = base->level + 1,
                        .parent = base,
                        .valid = 0,
                };
        return base->branches;
}

/* Create and return a new PT */
PT_node
PT_create()
{
        PT_node pt;
        memset(&pt, 0, sizeof pt);
        create_branches(&pt);
        return pt;
}

/* Get the data at KEY or PT_INVALID from BASE PT */
PT_data
PT_get(PT_node *const base, const char *key, PT_hash hash_func)
{
        if (base == NULL)
                return PT_INVALID;

        if (key[base->level] == '\0') {
                if (base->valid == 0)
                        return PT_INVALID;

                if (!strcmp(base->key, key))
                        return base->data;

                /* WARNING! You can spin forever if you had the maximum number of possible entries. */
                return PT_get(base->parent->branches +
                              (hash_func(get_branches(base->level), base->level, key) +
                               1 % get_branches(base->level)),
                              key, hash_func);
        }

        if (base->branches == NULL)
                return PT_INVALID;

        return PT_get(base->branches + hash_func(get_branches(base->level), base->level, key),
                      key, hash_func);
}

/* Add DATA to KEY at BASE PT */
void
PT_add(PT_node *base, PT_data data, const char *key, PT_hash hash_func)
{
        if (base->branches == NULL) {
                create_branches(base);
        }

        if (key[base->level] == '\0') {
                if (base->valid == 0) {
                        base->valid = 1;
                        base->data = data;
                        base->key = strdup(key);
                        return;
                }

                /* WARNING! You can spin forever if you had the maximum number of possible entries. */
                return PT_add(base->parent->branches +
                              (hash_func(get_branches(base->level), base->level, key) +
                               1 % get_branches(base->level)),
                              data, key, hash_func);
        }

        PT_add(base->branches + hash_func(get_branches(base->level), base->level, key),
               data, key, hash_func);
}

/* Destroy BASE PT */
void
PT_destroy(PT_node *base)
{
        if (base->branches == NULL)
                return;

        for (int i = 0; i < get_branches(base->level); i++) {
                if (base->branches[i].key)
                        free((void *) base->branches[i].key);
                PT_destroy(base->branches + i);
        }

        free(base->branches);
}
