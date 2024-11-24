#ifndef HCF_HASHMAP_H
#define HCF_HASHMAP_H


#include <stddef.h>
/* Hash table */

// The idea is to store values using strdup for strings and  be able
// to store pointers to hash tables
typedef struct __hash_table_node
{
    struct __hash_table_node *next;  // allow collisions
    char                     *key;   // current element key
    void                     *value; // current element value
} HashTableNode;

typedef struct
{
    size_t         size;     // elements in node_arr
    HashTableNode *node_arr; // array of nodes
} __HashTable;

/* Initialize a hashmap of a given size */
void __hashmap_new(__HashTable *table, size_t size);
/* Add a value to a hashmap */
void __hashmap_add(__HashTable *table, const char *key, void *value);
/* Remove a key-value pair from a hashmap */
void __hashmap_pop(__HashTable *table, const char *key);
/* Get the value of a key in a hashmap or null if key not found*/
void *__hashmap_get(__HashTable table, const char *key, void **value);
/* Get the numeric key given a string key and a hash table */
size_t __hashmap_key(__HashTable table, const char *key);
/* */
void __hashmap_destroy(__HashTable *table);

#endif // !HCF_HASHMAP_H
