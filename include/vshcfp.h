#ifndef VSHCFP_H
#define VSHCFP_H

#include "hcf_hashmap.h"

/* Type aliases */
typedef __HashTable HcfOpts;  // Options hash of hashes
typedef __HashTable HcfField; // fields hash

/* Implementation details */

/* The following numbers are just used to adjust hashmap memory efficiency.
 * The numbers dont set a maximum. The closest the number is to the reality
 * it would be more memory and time efficient. */

/* Number of fields in each file */
#define FIELDS_N 10
/* Number of key-value entries in each field */
#define OPTS_N 10

/* VSHCFP interface */

/* Load a file. File fields and options have to be defined following
 * the HCF standard. They are stored in a HcfOpts struct that have to
 * be destroyed. */
HcfOpts hcf_load(const char *file);

/* Get the field map given their name. */
HcfField *hcf_get_field(HcfOpts opts, const char *field);

/* Get the value of a given key of a field. */
char *hcf_get_value(HcfField field, const char *key);

/* Get a value given a field name and the options data structure */
char *hcf_get(HcfOpts opts, const char *field, const char *key);

/* Manually add a value */
void hcf_add(HcfField *field, const char *key, char *value);

/* destroy a yet created Options table */
void hcf_destroy(HcfOpts *opts);


#endif
