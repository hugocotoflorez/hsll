#include "../include/vshcfp.h"
#include <stddef.h>
#include <stdio.h> // debug print
#include <stdlib.h>
#include <string.h>

/* Initialize a hashmap of a given size */
void
__hashmap_new(__HashTable *table, size_t size)
{
        table->node_arr = calloc(size, sizeof(HashTableNode));
        table->size = size;
}

/* Add a value to a hashmap */
void
__hashmap_add(__HashTable *table, const char *key, void *value)
{
        HashTableNode *node;
        int index;

        index = __hashmap_key(*table, key);
        node = table->node_arr + index;

        while (node->next)
                node = node->next;

        node->key = strdup(key);
        node->value = value;
        node->next = calloc(1, sizeof(HashTableNode));
}

/* Remove a key-value pair from a hashmap */
void
__hashmap_pop(__HashTable *table, const char *key)
{
        /* TODO: this is a piece of shit */
        HashTableNode *node;
        HashTableNode *last;
        int index;

        index = __hashmap_key(*table, key);
        node = table->node_arr + index;
        last = table->node_arr + index;

        /* Get the node with key KEY */
        while (node->next && strcmp(node->key, key))
                node = node->next;

        /* Key is not in the list */
        if (!node)
                return;

        /* Get the last entry */
        while (last->next)
                last = last->next;

        free(node->key);
        node->key = last->key;
        node->value = last->value;

        while (node->next != last)
                node = node->next;

        node->next = NULL;

        free(last);
}

/* Get the value of a key in a hashmap or null if key not found*/
void *
__hashmap_get(__HashTable table, const char *key, void **value)
{
        HashTableNode *node;
        size_t index;

        if (!key || !table.size)
                return NULL;

        index = __hashmap_key(table, key);
        node = table.node_arr + index;

        while (node->next && strcmp(node->key, key))
                node = node->next;

        if (value)
                *value = node->value;

        return node->value;
}

/* Get the numeric key given a string key and a hash table */
size_t
__hashmap_key(__HashTable table, const char *key)
{
        /* Total sum algorithm */
        unsigned sum = 0;

        if (!key || !table.size)
                return 0;

        for (int i = 0; key[i]; ++i)
                sum += i * key[i];

        return sum % table.size;
}

void
__hashmap_destroy(__HashTable *table)
{
        HashTableNode *node;
        HashTableNode *next;

        if (!table)
                return;

        for (size_t i = 0; i < table->size; i++)
        {
                node = table->node_arr + i;

                while (node)
                {
                        next = node->next;

                        free(node->key);
                        free(node->value);
                        if (node != table->node_arr + i)
                                free(node);

                        node = next;
                }
        }

        free(table->node_arr);
        table->node_arr = NULL;
        table->size = 0;
}
