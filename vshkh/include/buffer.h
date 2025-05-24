#ifndef BUFFER_H
#define BUFFER_H

/***************************************************
 * ---| Buffer (buffer.c)                     |--- *
 ***************************************************/

/* As mentioned above, I share this info to allow
 * user modify and customize the behaviour of the
 * handler.
 * BUT if nothing is changed here all would work as
 * expected, so please dont touch anything if you are
 * not sure about what are you doing. Thanks. */

/* Buffer is a circular buffer of dynamic size.
 * It aims to be easy to use and flexible, as well
 * as fast.
 * As this implementation is done to store keypresses
 * the types and other stuff are based on author
 * knowledge about future usage. */

/* Current implementation is thread-safe */

typedef struct {
        int length; /* Number of elements in buffer. */
        Keypress *data; /* Elements in the buffer */
        Keypress *out_ptr; /* Pointer to next element that have
                            * to be processed */
        Keypress *in_ptr; /* Pointer to the next position
                           * where a new element have to
                           * be stored */
} Buffer;

/* As this buffer is not going to be used more than
 * once Im going to use kp_buffer as the
 * buffer on which every function call applied. */
extern Buffer kp_buf;

/* If this is defined it grow the buffer if new element cant
 * be added. Otherwise a error would be returned */
#define FIX_OVERFLOW 1

/* Initialize the buffer */
void buffer_new(int);

/* Add a keypress to the buffer and
 * return added keypress. */
Keypress buffer_add(Keypress);

/* Remove an element from the buffer
 * and return it. If no elements found,
 * it returns INVALID_KP. */
Keypress buffer_pop();

/* Get the top of the buffer */
Keypress buffer_top();

/* Change buffer size. SIZE must be bigger than
 * previous size. */
int buffer_chsize(int);

/* Destroy the buffer, all data
 * in the buffer are lost */
void buffer_destroy();


#endif
