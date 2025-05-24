#ifndef ARRAY_H
#define ARRAY_H

/***************************************************
 * ---| Array (dynarray.c)                    |--- *
 ***************************************************/

/* Dynamic array for store keybinds. DATA array entries
 * can be both valid or invalid keybinds. Invalid keybinds
 * could be overwritten. If array is full, it would increment
 * by ARRINC. */

/* Current implementation is thread-safe */

typedef struct {
        int length; /* Number of elements in buffer. */
        int alloc_size; /* Size of data, max number of elements
                         * that can be stored in data without
                         * overflow */
        Keybind *data; /* Elements in the buffer */
} Array;

/* Array grow as
 * new size = old size + ARRINC */
#define BUFINC 4

/* As this array is not going to be used more than
 * once, Im going to use kp_array as the
 * buffer on which every function call applied. */
extern Array kb_array;

/* Initialize the array*/
void array_new();

/* Add a keybind to the buffer and
 * return added keybind. */
Keybind array_add(Keybind kb);

/* Remove an keybind from the array
 * and return it */
Keybind array_pop(Keybind);

/* Get the function if keyfind is found or NULL.
 * It is posible to call it as array_exec(kb)(); */
BindFunc array_exec(Keybind);

/* Return 0 if keybind is not in array, otherwise true value */
int array_exist(Keybind kb);

/* Modify keybind data with the new Keybind values */
void array_modify(Keybind kb);

/* Destroy the buffer, all data
 * in the buffer are lost */
void array_destroy();


#endif
