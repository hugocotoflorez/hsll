#include "../include/vshkh.h"
#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Array kb_array;
static pthread_mutex_t mutex;

/* Value used to increment the buffer. Defined in header file.
 * new size = old size + BUFINC */
/* #define BUFINC 4; */

/* Get the adress of the first occurence of kb or NULL */
static Keybind *__array_get(Keybind kb);

/* Get the adress of the first occurence of INVALID_KB or NULL */
static Keybind *__get_invalid();

/* Get the first empty adress or used by an invalid kb */
static Keybind *__get_empty();

/* Grow the array */
static void __grow();

/* Pop the element at index I. Index I have to be valid.*/
static Keybind __pop(int i);

/* Insert an element, dont change any variable or check anything.
 * This function dont increment array size */
static Keybind __insert(Keybind *kbptr, Keybind kb);

/* Add a keybind to the buffer and
 * return added keybind. Thread-unsafe */
static Keybind __array_add_unsafe(Keybind kb);

static inline Keybind
__insert(Keybind *kbptr, Keybind kb)
{
        return (*kbptr = kb);
}

// grow kb_array by BUFINC
static void
__grow()
{
        kb_array.alloc_size += BUFINC;
        kb_array.data = realloc(kb_array.data, kb_array.alloc_size * sizeof(Keybind));
        assert(kb_array.data);
}

static Keybind
__pop(int i)
{
        /* Just mark element as invalid, dont move elements */
        Keybind ret = kb_array.data[i];
        kb_array.data[i] = INVALID_KB;
        return ret;
}

inline static Keybind *
__get_invalid()
{
        return __array_get(INVALID_KB);
}

static Keybind *
__get_empty()
{
        if (kb_array.alloc_size == kb_array.length)
                /* return the first empty keybind from data array. It returns
                 * NULL if no invalid entry were found */
                return __get_invalid();
        else
                /* Return the last empty position and increment length */
                return kb_array.data + kb_array.length++;
        return NULL;
}

/* Get the first adress of kp if it is in the array */
static Keybind *
__array_get(Keybind kb)
{
        for (int i = 0; i < kb_array.length; i++)
                /* Return the address of the keybind if its invalid */
                if (kb_is_equal(kb, kb_array.data[i]))
                        return kb_array.data + i;
        return NULL;
}

/* Return 0 if keybind is not in array, otherwise true value */
int
array_exist(Keybind kb)
{
        int ret;

        pthread_mutex_lock(&mutex);
        ret = __array_get(kb) != NULL;
        pthread_mutex_unlock(&mutex);

        return ret;
}

/* Modify keybind data with the new Keybind values */
void
array_modify(Keybind kb)
{
        pthread_mutex_lock(&mutex);
        Keybind *pos = __array_get(kb);
        if (pos)
                *pos = kb;
        pthread_mutex_unlock(&mutex);
}

/* Initialize the array*/
void
array_new()
{
        pthread_mutex_lock(&mutex);
        kb_array = (Array) {
                .data = NULL,
                .length = 0,
                .alloc_size = 0,
        };
        pthread_mutex_unlock(&mutex);
}

/* Add a keybind to the buffer and
 * return added keybind. Thread-unsafe */
static Keybind
__array_add_unsafe(Keybind kb)
{
        Keybind *cell = __get_empty();

        if (cell)
                return __insert(cell, kb);

        __grow();

        return __array_add_unsafe(kb);
}

/* Add a keybind to the buffer and
 * return added keybind. */
Keybind
array_add(Keybind kb)
{
        Keybind *cell;
        Keybind ret;

        pthread_mutex_lock(&mutex);

        cell = __get_empty();
        if (cell)
                ret = __insert(cell, kb);

        else
        {
                __grow();
                ret = __array_add_unsafe(kb);
        }

        pthread_mutex_unlock(&mutex);

        return ret;
}

/* Remove a keybind KB from the array
 * and return it */
Keybind
array_pop(Keybind kb)
{
        Keybind ret;

        pthread_mutex_lock(&mutex);
        ret = __pop(kb_array.data - __array_get(kb));
        pthread_mutex_unlock(&mutex);

        return ret;
}

/* Get the function from keybind.*/
BindFunc
array_exec(Keybind kb)
{
        BindFunc ret;

        pthread_mutex_lock(&mutex);
        ret = __array_get(kb)->func;
        pthread_mutex_unlock(&mutex);

        return ret;
}

/* Destroy the buffer, all data
 * in the buffer are lost */
void
array_destroy()
{
        pthread_mutex_lock(&mutex);
        free(kb_array.data);
        // set all fields as unset
        kb_array = (Array) { 0 };
        pthread_mutex_unlock(&mutex);
}

/* Return the number of kb with
 * whe same prefix */
int
array_prefix_get(Keybind prefix)
{
        int counter = 0;
        for (int i = 0; i < kb_array.length; i++)
                /* Return the address of the keybind if its invalid */
                if (kb_left_match(prefix, kb_array.data[i]))
                        ++counter;
        return counter;
}
