#ifndef KEYBOARD_H
#define KEYBOARD_H

/***************************************************
 * ---| VSHKH interface (keyboard.c)          |--- *
 ***************************************************/

/* The keyboard handler would be initialized in
 * a different thread, so this funcion returns
 * once it is created. This function also wake up
 * the handler if paused. */
void kh_start();

/* Pause the handler, the input would be read
 * once it starts again */
void kh_pause();

/* Toggle the handler status, the input would be read
 * once it starts again. kh_pause pauses the handler and
 * kh_start starts the handler, both functions can be called
 * more than once without breaking the program (i hope). */
void kh_toggle();

/* Active coocked mode and use escape sequences and mods as
 * keybind options and modifiers */
void kh_set_coocked();

/* Disable coocked mode and thread all read chars as raw chars */
void kh_set_raw();

/* Close the handler and restore all values
 * to default */
void kh_end();

/* Get a keypress that is waiting in buffer and
 * remove it from buffer. If no keypress is in
 * buffer it return a keypress that return 0
 * if passed to kh_valid_kp. */
Keypress kh_get();

/* Wait for keyboard input and return the first
 * keypress */
Keypress kh_wait();

/* Ignore buffered keypresses and empty the
 * buffer. Removed keypressed cant be accesed
 * after calling this funcion. */
void kh_flush();


#endif
