/*********************************************************
 * VSHKH                                                 *
 *********************************************************
 * Author: Hugo Coto Florez                              *
 *                                                       *
 * Wellcome to Very Simple Hugo's Keyboard Handler       *
 * The aim of this proyect is to give an easy API        *
 * for a keyboard handler.                               *
 *                                                       *
 * Contact info: hugo.coto@rai.usc.es                    *
 * -> github.com/hugocotoflorez(/vshkh)                  *
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

/***************************************************
 * ---| MISC                                  |--- *
 ***************************************************/

/* User dont have to handle this struct manually,
 * but due to Author's metodology it is shared
 * to allow custom implementations.
 * See kh_has_ctrl and related. */
typedef enum {
        NO_MOD = 0,
        CTRL_MOD = 1 << 1,
        SHIFT_MOD = 1 << 2,
        ALT_MOD = 1 << 3,
        SUPR_MOD = 1 << 4,
        IS_ARROW = 1 << 5,
        IS_INVALID = 1 << 6,
} Mods;

/* Keypress return a character with the mods used.
 * If an upercase letter is returned, c is an upercase
 * letter and shift mod is set.
 * If Keypress is typecasted to char it store the char
 * inside Keypress */
typedef struct {
        char c;
        Mods mods;
} Keypress;

#define INVALID_KP                          \
        (Keypress)                          \
        {                                   \
                .c = 0, .mods = IS_INVALID, \
        }

#include "keybinds.h"
#include "keyboard.h"

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
#define kh_bind_create(str, action)                    \
        do {                                           \
                Keybind __kb__ = kh_bind_parse((str)); \
                kh_bind_set_func(&__kb__, (action));   \
                kh_bind_add(__kb__);                   \
        } while (0)


/***************************************************
 * ---| Utils (utils.c)                       |--- *
 ***************************************************/

typedef enum {
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

/* Return 1 if KB starts with PREFIX or 0 */
int kb_left_match(Keybind prefix, Keybind kb);

int kb_len(Keybind kb);

/* Print to stdout the representation using special
 * characters to represent mods (see parsing format) */
void kh_repr_kp(Keypress);

/* Get the representation form of KB and store it in S */
char *kb_repr(Keybind kb, char *s);

#include "array.h"
#include "buffer.h"

#endif
