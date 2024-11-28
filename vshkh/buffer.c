#include "../include/vshkh.h"
#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>

/* keypress buffer */
Buffer kp_buf;

/* keypress buffer mutex */
static pthread_mutex_t mutex;

/* Returns 1 if ptr is not in buf data valid addresses */
static inline int
__out_of_bounds(Keypress *ptr)
{
    return ptr - kp_buf.data >= kp_buf.length && ptr >= kp_buf.data;
}

/* Move ptr 1. if it is equal to other pointer (in/out), return NULL and
 * pointer is set to the same address as the other (in\out) pointer.
 * Otherwise it returns the new address of ptr. */
static Keypress *
__move_forward(Keypress **ptr)
{
    ++*ptr;

    if (__out_of_bounds(*ptr))
        *ptr = kp_buf.data;

    if (*ptr == kp_buf.in_ptr)
        return NULL;

    if (*ptr == kp_buf.out_ptr)
        return NULL;

    return *ptr;
}

/* Move ptr n positions. If it cant avance more it return NULL and
 * pointer is set to the same address as the other (in\out) pointer
 * otherwise it returns the new vaulue of pointer.*/
static Keypress *
__move_ptr(Keypress **ptr, int n)
{
    while (n--)
    {
        if (!__move_forward(ptr))
            return NULL;
    }
    return *ptr;
}

static void
__set_invalid()
{
    for (int i = 0; i < kp_buf.length; i++)
        kp_buf.data[i] = INVALID_KP;
}

/* Initialize the buffer */
void
buffer_new(int size)
{
    pthread_mutex_lock(&mutex);
    kp_buf.data    = malloc(sizeof(Keypress) * size);
    kp_buf.out_ptr = kp_buf.data;
    kp_buf.in_ptr  = kp_buf.data;
    kp_buf.length  = size;
    __set_invalid();
    pthread_mutex_unlock(&mutex);
};

/* Add a keypress to the buffer and
 * return added keypress. */
Keypress
buffer_add(Keypress kp)
{
    pthread_mutex_lock(&mutex);

    /* Buffer is full */
    if (kh_valid_kp(*kp_buf.in_ptr))
    {
        pthread_mutex_unlock(&mutex);
        return INVALID_KP;
    }

    *kp_buf.in_ptr = kp;
    __move_ptr(&kp_buf.in_ptr, 1);

    pthread_mutex_unlock(&mutex);
    return kp;
}

/* Remove an element from the buffer
 * and return it */
Keypress
buffer_pop()
{
    Keypress kp;
    pthread_mutex_lock(&mutex);

    if (!kh_valid_kp(*kp_buf.out_ptr))
    {
        pthread_mutex_unlock(&mutex);
        return INVALID_KP;
    }

    kp              = *kp_buf.out_ptr;
    *kp_buf.out_ptr = INVALID_KP;

    __move_ptr(&kp_buf.out_ptr, 1);

    pthread_mutex_unlock(&mutex);
    return kp;
}

/* Get the top of the buffer */
inline Keypress
buffer_top()
{
    return *kp_buf.out_ptr;
}

/* Change buffer size. SIZE must be bigger than
 * previous size. */
int
buffer_chsize(int size)
{
    pthread_mutex_lock(&mutex);
    if (size > kp_buf.length)
    {
        kp_buf.data    = realloc(kp_buf.data, sizeof(Keypress) * size);
        kp_buf.out_ptr = kp_buf.data;
        kp_buf.in_ptr  = kp_buf.data;
        kp_buf.length  = size;
    }
    pthread_mutex_unlock(&mutex);
    return kp_buf.length;
}

/* Destroy the buffer, all data
 * in the buffer are lost */
void
buffer_destroy()
{
    pthread_mutex_lock(&mutex);
    free(kp_buf.data);
    kp_buf = (Buffer) { 0 };
    pthread_mutex_unlock(&mutex);
}
