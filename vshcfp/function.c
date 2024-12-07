#include "../include/vshcfp.h"

inline HcfField *
hcf_get_field(HcfOpts opts, const char *field)
{
    return __hashmap_get(opts, field, NULL);
}

inline char *
hcf_get_value(HcfField field, const char *key)
{
    return __hashmap_get(field, key, NULL);
}

char *
hcf_get(HcfOpts opts, const char *field, const char *key)
{
    __HashTable *table = __hashmap_get(opts, field, NULL);
    if (table)
        return __hashmap_get(*table, key, NULL);

    return NULL;
}

void
hcf_add(HcfField *field, const char *key, char *value)
{
    if (field)
        __hashmap_add(field, key, value);
}
