#include "../include/vshkh.h"
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/* Globals that must not be changed manually */
static int       ENABLED = 1; // set to 0 to pause handler
static int       STARTED = 0; // control if it is already started
static int       QUIT    = 0; // set to 1 to exit thread
static pthread_t main_thread; // thread id

/* Terminal flags used to reset it when handler is closed */
static struct termios origin_termios;
static int            flags;

/* Sleep time if nothing read */
#define ST_NRD 10000
/* Sleep time if disabled */
#define ST_DIS 10000
/* sleep time while waiting in kh_wait */
#define ST_WAIT 10000

/* Buffer size */
#define BUFSIZE 10

/* SUPR + SHIFT + key mods characters
 * Uper case characters are placed over
 * lower case ones that share physical key. */
/* The '?' except for the real '?' can be deleted, but
 * still here for debuging reasons. It only represents a
 * key that dont have to be represented while using
 * supr + shift mod */
static char supr_shift_lookup[] = {
    [0] = '?',   [1] = '?',   [2] = '?',   [3] = '?',   [4] = '?',
    [5] = '?',   [6] = '?',   [7] = '?',   [8] = '?',   [9] = '?',
    [10] = '?',  [11] = '?',  [12] = '?',  [13] = '?',  [14] = '?',
    [15] = '?',  [16] = '?',  [17] = '?',  [18] = '?',  [19] = '?',
    [20] = '?',  [21] = '?',  [22] = '?',  [23] = '?',  [24] = '?',
    [25] = '?',  [26] = '?',  [27] = '?',  [28] = '?',  [29] = '?',
    [30] = '?',  [31] = '?',  [32] = '?',  [33] = '?',  [34] = '?',
    [35] = '?',  [36] = '?',  [37] = '?',  [38] = '?',  [39] = '?',
    [40] = '?',  [41] = '?',  [42] = '?',  [43] = '?',  [44] = '<',
    [45] = '_',  [46] = '>',  [47] = '?',  [48] = ')',  [49] = '!',
    [50] = '@',  [51] = '#',  [52] = '$',  [53] = '%',  [54] = '^',
    [55] = '&',  [56] = '*',  [57] = '(',  [58] = '?',  [59] = ':',
    [60] = '?',  [61] = '?',  [62] = '?',  [63] = '?',  [64] = '?',
    [65] = '?',  [66] = '?',  [67] = '?',  [68] = '?',  [69] = '?',
    [70] = '?',  [71] = '?',  [72] = '?',  [73] = '?',  [74] = '?',
    [75] = '?',  [76] = '?',  [77] = '?',  [78] = '?',  [79] = '?',
    [80] = '?',  [81] = '?',  [82] = '?',  [83] = '?',  [84] = '?',
    [85] = '?',  [86] = '?',  [87] = '?',  [88] = '?',  [89] = '?',
    [90] = '?',  [91] = '{',  [92] = '|',  [93] = '}',  [94] = '?',
    [95] = '?',  [96] = '~',  [97] = 'A',  [98] = 'B',  [99] = 'C',
    [100] = 'D', [101] = 'E', [102] = 'F', [103] = 'G', [104] = 'H',
    [105] = 'I', [106] = 'J', [107] = 'K', [108] = 'L', [109] = 'M',
    [110] = 'N', [111] = 'O', [112] = 'P', [113] = 'Q', [114] = 'R',
    [115] = 'S', [116] = 'T', [117] = 'U', [118] = 'V', [119] = 'W',
    [120] = 'X', [121] = 'Y', [122] = 'Z', [123] = '?', [124] = '?',
    [125] = '?', [126] = '?', [127] = '?',
};

/* Disable all terminal binds and especial behaviour to catch all raw
 * input. Previous flags and settings are stored in global FLAGS and
 * ORIGIN_TERMIOS. */
static void
__enable_raw_mode()
{
    struct termios raw_opts;
    tcgetattr(STDIN_FILENO, &origin_termios);
    raw_opts = origin_termios;
    cfmakeraw(&raw_opts);
    raw_opts.c_oflag |= (OPOST | ONLCR); // '\n' -> '\r\n'
    tcsetattr(STDIN_FILENO, TCSANOW, &raw_opts);
    flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

/* Return terminal to normal mode. this function HAVE TO BE CALLED AFTER
 * ENABLING RAW MODE. If not udefined behaviour is get. */
static void
__disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &origin_termios);
    fcntl(STDIN_FILENO, F_SETFL, flags);
    /* I think this problem is now solved. Im not going to remove
     * the comment because I dont fix it so it disapears magically
     * so i dont know if it was lucky or it could came back */
    // system("reset"); // force the terminal to reset. ^C failed.
}

/* Take an action: call a function is kp is binded or
 * add it to keypresses buffer */
/* Todo: allow multiple kp binds */
static void
__kp_action(Keypress kp)
{
    Keybind  kb;
    BindFunc func;

    kb = kh_bind_new();
    kh_bind_append(&kb, kp);

    func = kh_bind_get(kb);
    if (func)
        func();
    else
        buffer_add(kp);
}


/* Given a char return the default keypress
 * it only check for single chars (ctrl, shift)
 * other modifier dont call this function (directly) */
static Keypress
__get_kp_from_char(char c)
{
    Keypress kp;
    /* defaults, can be changed in this function */
    kp.mods = NO_MOD;
    kp.c    = c;

    switch (c)
    {
        // control keypress
        case 0x0 ... 0x1F:
            kp.mods |= (CTRL_MOD | SHIFT_MOD);
            kp.c = c + '@'; // adjust key to representable key
            break;

            // clang-format off
        case 'A' ... 'Z':
            /* Chars that uses shift modifier */
        case '<': case '_': case '>': case '?':
        case ')': case '!': case '#': case '$':
        case '%': case '*': case '(': case ':':
        case '{': case '|': case '}': case '~':
        case '@': case '^': case '&':
            // clang-format on
            kp.mods = SHIFT_MOD;
            break;
    }
    return kp;
}


/* Temporal? given a raw read char, assign it to a keypress
 * and take the action related to the keypress */
static void
__analize(char c)
{
    __kp_action(__get_kp_from_char(c));
}

/* Get default arrow keypress
 * from direction character (A-D) */
static Keypress
__get_arrow_kp(char c)
{
    switch (c)
    {
        case 'A':
            return ((Keypress) {
            .c    = ARROW_UP,
            .mods = IS_ARROW,
            });

        case 'B':
            return ((Keypress) {
            .c    = ARROW_DOWN,
            .mods = IS_ARROW,
            });

        case 'C':
            return ((Keypress) {
            .c    = ARROW_RIGHT,
            .mods = IS_ARROW,
            });

        case 'D':
            return ((Keypress) {
            .c    = ARROW_LEFT,
            .mods = IS_ARROW,
            });
    }
    return INVALID_KP;
}

/* Get the char associated with the super mod. It can not be
 * calculated directly because shift mod dont change input
 * char, so it uses a lookup table that places characters that
 * should be represented if shift is used above the characters
 * that share physical key without shift mod */
static void
__supr_get_char(Keypress *kp, int supr_mod, int supr_key)
{
    switch (supr_mod) /* SUPR MODS */
    {
        case 0x2:  // arrow + shift
        case 0x3:  // arrow + alt
        case 0x4:  // arrow + alt + shift
        case 0x8:  // arrow + ctrl + alt + shift
        case 0xA:  // supr + shift
        case 0xC:  // supr + shift + alt
        case 0xE:  // supr + shift + ctrl
        case 0x10: // supr + shift + ctrl + alt
            kp->c = supr_shift_lookup[supr_key];
            break;

        default:
            kp->c = supr_key;
            break;
    }
}

/* Get the modifiers given a mod and a key (from a supr like entry). */
static void
__supr_get_mods(Keypress *kp, int supr_mod, int supr_key)
{
    switch (supr_mod) /* SUPR MODS */
    {
        case 0x2: // arrow + shift
            kp->mods |= (SHIFT_MOD);
            break;
        case 0x3: // arrow + alt
            kp->mods |= (ALT_MOD);
            break;
        case 0x4: // arrow + alt + shift
            kp->mods |= (ALT_MOD | SHIFT_MOD);
            break;
        case 0x5: // arrow + ctrl
            kp->mods |= (CTRL_MOD);
            break;
        case 0x7: // arrow + ctrl + alt
            kp->mods |= (CTRL_MOD | ALT_MOD);
            break;
        case 0x8: // arrow + ctrl + alt + shift
            kp->mods |= (CTRL_MOD | ALT_MOD | SHIFT_MOD);
            break;
        case 0x9:
            /* This is the base case for supr mods, but as it
             * is shared for arrow keypresses, I add the super mod
             * here because othersize arrow dont have this mod. In
             * the supr cases remaining the supr mod is add after
             * (or before i dont remember) calling this function */
            kp->mods |= (SUPR_MOD);
            break;
        case 0xA: // shift
            kp->mods |= (SUPR_MOD | SHIFT_MOD);
            break;
        case 0xB: // alt
            kp->mods |= (SUPR_MOD | ALT_MOD);
            break;
        case 0xD: // ctrl
            kp->mods |= (SUPR_MOD | CTRL_MOD);
            break;
        case 0xC: // shift + alt
            kp->mods |= (SUPR_MOD | SHIFT_MOD | ALT_MOD);
            break;
        case 0xE: // shift + ctrl
            kp->mods |= (SUPR_MOD | CTRL_MOD | SHIFT_MOD);
            break;
        case 0xF: // ctrl + alt
            kp->mods |= (SUPR_MOD | CTRL_MOD | ALT_MOD);
            break;
        case 0x10: // shift + ctrl + alt
            kp->mods |= (SUPR_MOD | CTRL_MOD | SHIFT_MOD | ALT_MOD);
            break;

        default:
            printf("Undetected state: %d - %d\n", supr_key, supr_mod);
            break;
    }
}

/* Get the arrow keypress given an extended format of
 * representation stored in buf string, of size n. */
static Keypress
__get_arrow(char *buf)
{
    Keypress kp;
    int      supr_key;
    int      supr_mod;
    char     c;

    if (buf == NULL)
        printf("[DEBUG] NULL ENTRY\n");

    else
        sscanf(buf, "\x1b[%d;%d%c", &supr_key, &supr_mod, &c);

    /* Get defaults */
    kp = __get_arrow_kp(c);
    __supr_get_mods(&kp, supr_mod, supr_key);

    return kp;
}

/* Return a keypress given an array of characters that
 * represents the keypress with the mods. Supr mods and
 * arrows use this representation. This function is not
 * used for arrows. */
static Keypress
__get_supr_kp(char *buf)
{
    Keypress kp;
    int      supr_key;
    int      supr_mod;


    if (buf == NULL)
        printf("[DEBUG] NULL ENTRY\n");

    else
        sscanf(buf, "\x1b[%d;%du", &supr_key, &supr_mod);

    __supr_get_mods(&kp, supr_mod, supr_key);
    __supr_get_char(&kp, supr_mod, supr_key);

    return kp;
}

/* This function try to read after a ESC keypress
 * to get those special keys  that uses a representation
 * format starting at ESC, or escape sequences. This function
 * analize the keypresses and do whatever is needed without
 * returning anything */
static void
__esc_special(char *buf)
{
    ssize_t  n;
    Keypress kp;


    if (buf == NULL)
    {
        printf("[DEBUG] NULL ENTRY\n");
        return;
    }

    /* Get the remaining of the keypress and store it in
     * buf as if all was read together. Given the bytes read
     * it can be determined what is the type of the keypress */
    switch (n = read(STDIN_FILENO, buf + 1, BUFSIZE - 1))
    {
        case 2:
            /* All escape sequences start with \e[, so if the
             * first characters appears but not the second it is not
             * a scape sequence */
            if (buf[1] != '[')
                goto __normal__; // just to avoid nesting

            /* Check if keypress is a single arrow without
             * mods. (\e[A - \e[D representation ) If not,
             * analize keypresses individually */
            kp = __get_arrow_kp(buf[2]);
            if (kh_valid_kp(kp))
            {
                __kp_action(kp);
                return;
            }

        __normal__:
            __analize(buf[0]);
            __analize(buf[1]);
            __analize(buf[2]);

            return;

        case 1:
            // Alt mod
            /* Analize single key for allow alt+ctrl */
            kp = __get_kp_from_char(buf[1]);
            /* This piece of code is only called
             * if alt is used with ctrl without shift, or
             * without mods, so as ctrl set shift mod I
             * have to unset it manually. */
            if (kp.mods & CTRL_MOD)
                kp.mods &= ~SHIFT_MOD;
            /* Char is represented as upercase because it
             * calls ctrl logic that is limited to upercase
             * letters and a little symbols. */
            kp.mods |= ALT_MOD;
            __kp_action(kp);
            return;

        case 0:  // no input
        case -1: // eof -> no input
            __analize(buf[0]);
            break;

        default:
            /* supr mods end with 'u'. Also
             * alt-ctrl-shift-a, so I change some
             * code to allow this behaviour.*/
            if (buf[n] == 'u')
                kp = __get_supr_kp(buf);

            /* arrow + mods end with 'A' - 'D' */
            else if (buf[n] >= 'A' && buf[n] <= 'D')
                kp = __get_arrow(buf);

            __kp_action(kp);
            break;
    }
}

/* Main method called into a thread creation. It read from stdin
 * and call the needed functions to execute the binded functions or
 * add keypresses to the buffer */
static void *
__keyboard_handler()
{
    ssize_t n;
    char    buf[BUFSIZE];

    __enable_raw_mode();

    while (!QUIT)
    {
        if (ENABLED)
            switch (n = read(STDIN_FILENO, buf, 1))
            {
                case -1:
                case 0:
                    /* If nothing was read, sleep a little to
                     * avoid use too much cpu */
                    usleep(ST_NRD);
                    break;

                default:              /* Something is read */
                    if (*buf == 0x1b) // esc
                        /* It can be a escape keypress or a
                         * escape sequence, both cases are handled
                         * into the following function */
                        __esc_special(buf);

                    else
                        __analize(*buf);

                    break;
            }

        else // keyboard handled is not enabled
            usleep(ST_DIS);
    }

    /* Disable raw mode at exit */
    __disable_raw_mode();

    return NULL;
}

/* Forcefully kill handler. Thread is not terminated. It is recommended
 * to call kh_end instead. */
static void
__die()
{
    QUIT = 1;
    /* Althougt the raw mode is disabled once the handler
     * exists, calling pthread join from here give me some
     * problems where it is not disabled so I came up with
     * this (temporal) solution. */
    __disable_raw_mode();
}

/* Automatically initialize the buffer, if gcc or clang is used
 * this function can be removed, but both initializations have to be
 * managed manually */
static __attribute__((constructor)) void
__init__()
{
    buffer_new(10);
    array_new();
}

/* Get a keypress that is waiting in buffer and
 * remove it from buffer. If no keypress is in
 * buffer it return a keypress that return 0
 * if passed to kh_valid_kp. */
Keypress
kh_get()
{
    return buffer_pop();
}

/* The keyboard handler would be initialized in
 * a different thread, so this funcion returns
 * once it is created. This function also wake up
 * the handler if paused. */
void
kh_start()
{
    ENABLED = 1;
    if (!STARTED)
        pthread_create(&main_thread, NULL, __keyboard_handler, NULL);
}

/* Pause the handler, the input would be read
 * once it starts again */
void
kh_pause()
{
    ENABLED = 0;
}

/* Toggle the handler status, the input would be read
 * once it starts again. kh_pause pauses the handler and
 * kh_start starts the handler, both functions can be called
 * more than once without breaking the program (i hope). */
void
kh_toggle()
{
    ENABLED ^= 0x1;
}

/* Close the handler and restore all values
 * to default */
void
kh_end()
{
    __die();
    pthread_join(main_thread, NULL);
    ENABLED = 1;
    STARTED = 0;
    QUIT    = 0;
}

/* Wait for keyboard input and return the first
 * keypress */
Keypress
kh_wait()
{
    Keypress kp;
    /* This can be a quite lazy approach about
     * how to wait for input. But it works */
    while (!kh_valid_kp(kp = kh_get()))
        usleep(ST_WAIT);
    return kp;
}

/* Ignore buffered keypresses and empty the
 * buffer. Removed keypressed cant be accesed
 * after calling this funcion. UNTESTED*/
void
kh_flush()
{
    while (kh_valid_kp(buffer_pop()))
        ;
}
