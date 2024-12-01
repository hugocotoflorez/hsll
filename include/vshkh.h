/*********************************************************
 * VSHKH                                                 *
 *********************************************************
 * Author: Hugo Coto Florez                              *
 *                                                       *
 * Wellcome to Very Simple Hugo's Keyboard Handler       *
 * The aim of this proyect is to give an easy to         *
 * use keyboard handler. This proyect is part of         *
 * nhoutpad2.                                            *
 *                                                       *
 * Contact info: hugo.coto@rai.usc.es                    *
 * -> github.com/hugocotoflorez(/vshkh)                  *
 *********************************************************/

/*********************************************************
 * Function Apendix                                      *
 *********************************************************
 * Arrowkey kh_is_arrow(Keypress);                       *
 * void     kh_bind_create(char*, BindFunc){MACRO}       *
 * Keybind  kh_bind_parse(const char *str);              *
 * Keybind  kh_bind_append(Keybind *kb, Keypress kp);    *
 * void     kh_bind_set_func(Keybind kb, BindFunc func); *
 * Keybind  kh_bind_new();                               *
 * BindFunc kh_bind_get(Keybind kb);                     *
 * void     kh_bind_remove(Keybind);                     *
 * void     kh_bind_add(Keybind);                        *
 * int      kh_valid_kb(Keybind);                        *
 * void     kh_flush();                                  *
 * Keypress kh_wait();                                   *
 * Keypress kh_get();                                    *
 * void     kh_end();                                    *
 * void     kh_pause();                                  *
 * void     kh_start();                                  *
 * void     kh_toggle();                                 *
 * int      kh_has_ctrl(Keypress);                       *
 * int      kh_has_shift(Keypress);                      *
 * int      kh_has_alt(Keypress);                        *
 * int      kh_valid_kp(Keypress);                       *
 * char    *get_arrow_str(Arrowkey, Mods, char *);       *
 *********************************************************/

#if defined(ASCII_TABLE_REPR) && !defined(ASCII_TABLE_REPR_DEFINED)
#define ASCII_TABLE_REPR_DEFINED

/* Ascii representable form of all 7-bit value */
static const char *REPR[] = {
    "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS",  "HT",
    "LF",  "VT",  "FF",  "CR",  "SO",  "SI",  "DLE", "DC1", "DC2", "DC3",
    "DC4", "NAK", "SYN", "ETB", "CAN", "EM",  "SUB", "ESC", "FS",  "GS",
    "RS",  "US",  "SP",  "!",   "\"",  "#",   "$",   "%",   "&",   "'",
    "(",   ")",   "*",   "+",   ",",   "-",   ".",   "/",   "0",   "1",
    "2",   "3",   "4",   "5",   "6",   "7",   "8",   "9",   ":",   ";",
    "<",   "=",   ">",   "?",   "@",   "A",   "B",   "C",   "D",   "E",
    "F",   "G",   "H",   "I",   "J",   "K",   "L",   "M",   "N",   "O",
    "P",   "Q",   "R",   "S",   "T",   "U",   "V",   "W",   "X",   "Y",
    "Z",   "[",   "\\",  "]",   "^",   "_",   "`",   "a",   "b",   "c",
    "d",   "e",   "f",   "g",   "h",   "i",   "j",   "k",   "l",   "m",
    "n",   "o",   "p",   "q",   "r",   "s",   "t",   "u",   "v",   "w",
    "x",   "y",   "z",   "{",   "|",   "}",   "~",   "DEL",
};
#endif

#if defined(ASCII_TABLE_DESC) && !defined(ASCII_TABLE_DESC_DEFINED)
#define ASCII_TABLE_DESC_DEFINED

/* Ascii description of all 7-bit value */
static const char *DESC[] = {
    "Null character",
    "Start of Heading",
    "Start of Text",
    "End of Text",
    "End of Transmission",
    "Enquiry",
    "Acknowledge",
    "Bell, Alert",
    "Backspace",
    "Horizontal Tab",
    "Line Feed",
    "Vertical Tabulation",
    "Form Feed",
    "Carriage Return",
    "Shift Out",
    "Shift In",
    "Data Link Escape",
    "Device Control One (XON)",
    "Device Control Two",
    "Device Control Three (XOFF)",
    "Device Control Four",
    "Negative Acknowledge",
    "Synchronous Idle",
    "End of Transmission Block",
    "Cancel",
    "End of medium",
    "Substitute",
    "Escape",
    "File Separator",
    "Group Separator",
    "Record Separator",
    "Unit Separator",
    "Space",
    "Exclamation mark",
    "Double quotes (or speech marks)",
    "Number sign",
    "Dollar",
    "Per cent sign",
    "Ampersand",
    "Single quote",
    "Open parenthesis (or open bracket)",
    "Close parenthesis (or close bracket)",
    "Asterisk",
    "Plus",
    "Comma",
    "Hyphen-minus",
    "Period, dot or full stop",
    "Slash or divide",
    "Zero",
    "One",
    "Two",
    "Three",
    "Four",
    "Five",
    "Six",
    "Seven",
    "Eight",
    "Nine",
    "Colon",
    "Semicolon",
    "Less than (or open angled bracket)",
    "Equals",
    "Greater than (or close angled bracket)",
    "Question mark",
    "At sign",
    "Uppercase A",
    "Uppercase B",
    "Uppercase C",
    "Uppercase D",
    "Uppercase E",
    "Uppercase F",
    "Uppercase G",
    "Uppercase H",
    "Uppercase I",
    "Uppercase J",
    "Uppercase K",
    "Uppercase L",
    "Uppercase M",
    "Uppercase N",
    "Uppercase O",
    "Uppercase P",
    "Uppercase Q",
    "Uppercase R",
    "Uppercase S",
    "Uppercase T",
    "Uppercase U",
    "Uppercase V",
    "Uppercase W",
    "Uppercase X",
    "Uppercase Y",
    "Uppercase Z",
    "Opening bracket",
    "Backslash",
    "Closing bracket",
    "Caret - circumflex",
    "Underscore",
    "Grave accent",
    "Lowercase a",
    "Lowercase b",
    "Lowercase c",
    "Lowercase d",
    "Lowercase e",
    "Lowercase f",
    "Lowercase g",
    "Lowercase h",
    "Lowercase i",
    "Lowercase j",
    "Lowercase k",
    "Lowercase l",
    "Lowercase m",
    "Lowercase n",
    "Lowercase o",
    "Lowercase p",
    "Lowercase q",
    "Lowercase r",
    "Lowercase s",
    "Lowercase t",
    "Lowercase u",
    "Lowercase v",
    "Lowercase w",
    "Lowercase x",
    "Lowercase y",
    "Lowercase z",
    "Opening brace",
    "Vertical bar",
    "Closing brace",
    "Equivalency sign - tilde",
    "Delete",
};
#endif

#ifndef VSHKH_H
#define VSHKH_H

/* hsll extension: add a string as if it was read
 * from keyboard */
void __kh_sudo_append(char *str);

/***************************************************
 * ---| MISC                                  |--- *
 ***************************************************/

/* User dont have to handle this struct manually,
 * but due to Author's metodology it is shared
 * to allow custom implementations.
 * See kh_has_ctrl and related. */
typedef enum
{
    NO_MOD     = 0,
    CTRL_MOD   = 1 << 1,
    SHIFT_MOD  = 1 << 2,
    ALT_MOD    = 1 << 3,
    SUPR_MOD   = 1 << 4,
    IS_ARROW   = 1 << 5,
    IS_INVALID = 1 << 6,
} Mods;

/* Keypress return a character with the mods used.
 * If an upercase letter is returned, c is an upercase
 * letter and shift mod is set.
 * If Keypress is typecasted to char it store the char
 * inside Keypress */
typedef struct
{
    char c;
    Mods mods;
} Keypress;

#define INVALID_KP \
    (Keypress) { .c = 0, .mods = IS_INVALID, }

/***************************************************
 * ---| VSHHK interface (keyboard.c)          |--- *
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


/***************************************************
 * ---| Keybinds (keybinds.c)                 |--- *
 ***************************************************/

/* The keyboard handler can also execute a
 * void(*foo)(void) function if set to a key
 * or sequence of keys. Funtions below are
 * used to set or delete keybinds. */

/* Function prototype */
typedef void (*BindFunc)(void);

/* Max number of keypresses that can be stored
 * in a single keybind */
#define KEYBINDLEN 2

/* A keybind is a sequence of keypresses.
 * The keypresses should be at least 1, and
 * less or equal than KEYBINDLEN. KP is an
 * array of keypresses, NULL terminated. */
typedef struct
{
    Keypress kp[KEYBINDLEN + 1]; // all keypresses have to be valid
    BindFunc func;               // function that is going to be executed
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

/******************************************************
 * ---| STR Keybind format                       |--- *
 ******************************************************/

/* the max keypresses are stored in KEYBINDLEN
 * Modifiers:
 *  - Ctrl: represented as ^. Example: ctrl+a -> ^A
 *      -> NOTE!! char HAVE TO be UPERCASE
 *      -> NOTE!! ctrl + 1...0 dont exist
 *  - Shift: just use an upercase char
 *  - Alt: represented as &. Example: alt+a -> &a
 *  - Ctrl+Alt: represented as ^&. Example: ctrl+alt+a -> ^&A
 *      -> NOTE!! cant map alt+ctrl+upercase
 *  - SUPR: represented as @. Example: supr+a -> @a
 *  - SUPR + mod: concatenate, except that ctrl dont allow lowercase
 * Characters:
 *  - alphas from a-z
 *  - symbols
 * Special symbols: \^ -> ^ (for any char)
 */

/* This macro can be used to allow user create a new
 * bind without write a lot. It is only a wrapper for
 * previos declared functions */
#define kh_bind_create(str, action)            \
    do                                         \
    {                                          \
        Keybind __kb__ = kh_bind_parse((str)); \
        kh_bind_set_func(&__kb__, (action));   \
        kh_bind_add(__kb__);                   \
    } while (0)


/***************************************************
 * ---| Utils (utils.c)                       |--- *
 ***************************************************/

typedef enum
{
    NO_ARROW = 0,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_RIGHT,
    ARROW_LEFT,
} Arrowkey;

/* Return one of the values above.
 * Arrowkey can be used as a boolean
 * value, so if return value is false,
 * keypress is not an arrow key. */
Arrowkey kh_is_arrow(Keypress);

/* Return the format string given an arrow and the mods.
 * The char* argument is a string where the formatted string
 * would be written. Is the same that the function returns.*/
char *get_arrow_str(Arrowkey, Mods, char *);

/* Following functions return >0 if keypress has
 * the modifier in the function name pressed or
 * 0 otherwise */
int kh_has_ctrl(Keypress);
int kh_has_shift(Keypress);
int kh_has_alt(Keypress);
int kh_has_supr(Keypress);
int kh_valid_kp(Keypress);

int kb_is_equal(Keybind, Keybind);
int kp_is_equal(Keypress, Keypress);

/* Print to stdout the representation using special
 * characters to represent mods (see parsing format) */
void kh_repr_kp(Keypress);

/***************************************************
 * ---| Array (dynarray.c)                    |--- *
 ***************************************************/

/* Dynamic array for store keybinds. DATA array entries
 * can be both valid or invalid keybinds. Invalid keybinds
 * could be overwritten. If array is full, it would increment
 * by ARRINC. */

/* Current implementation is thread-safe */

typedef struct
{
    int length;     /* Number of elements in buffer. */
    int alloc_size; /* Size of data, max number of elements
                     * that can be stored in data without
                     * overflow */
    Keybind *data;  /* Elements in the buffer */
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

typedef struct
{
    int       length;  /* Number of elements in buffer. */
    Keypress *data;    /* Elements in the buffer */
    Keypress *out_ptr; /* Pointer to next element that have
                        * to be processed */
    Keypress *in_ptr;  /* Pointer to the next position
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
