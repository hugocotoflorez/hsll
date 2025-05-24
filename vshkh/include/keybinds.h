#ifndef KEYBINDS_H
#define KEYBINDS_H

/***************************************************
 * ---| Keybinds (keybinds.c)                 |--- *
 ***************************************************/

/* The keyboard handler can also execute a
 * void(*foo)(void) function if set to a key
 * or sequence of keys. Funtions below are
 * used to set or delete keybinds. */

/* Function prototype */
typedef void (*BindFunc)();

/* Max number of keypresses that can be stored
 * in a single keybind */
#define KEYBINDLEN 2

/* A keybind is a sequence of keypresses.
 * The keypresses should be at least 1, and
 * less or equal than KEYBINDLEN. KP is an
 * array of keypresses, NULL terminated. */
typedef struct {
        Keypress kp[KEYBINDLEN + 1]; // all keypresses have to be valid
        BindFunc func; // function that is going to be executed
} Keybind;

#define INVALID_KB (Keybind){ 0 }

/* return >0 if keybind is valid or 0 otherwise */
int kh_valid_kb(Keybind);

/* Add a keybind. If same sequences of keypresses are
 * yet stored, it only modified func. Note
 * that previous one would be overwritten. */
void kh_bind_add(Keybind);

/* Remove a keybind if exists */
void kh_bind_remove(Keybind);

/* Get the function from keybind that share KB
 * keypresses or NULL if not found */
BindFunc kh_bind_get(Keybind kb);

/* Initialize an empty keybind */
Keybind kh_bind_new();

/* Set the func to a keybind. This call dont
 * modify yet stored keybinds, so it is needed
 * to be called BEFORE add */
void kh_bind_set_func(Keybind *kb, BindFunc func);

/* Append a keypress to the keybind */
Keybind kh_bind_append(Keybind *kb, Keypress kp);


/* Return a Keybind with the same sequence at
 * str, being str a correct formatted textual
 * keybind. If some error happened return an
 * invalid Keybind. It can be tested using
 * kh_valid_kb(Keybind).
 * See STR Keybind format before. */
Keybind kh_bind_parse(const char *str);


#endif
