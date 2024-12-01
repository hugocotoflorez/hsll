// I DONT THINK THIS IS A GREAT SOLUTION
//
// #include "../include/vshkh.h"
// #include <unistd.h>
// #include <stdio.h>
//
// /* duplicate functions from keyboard.c */
// static void     __kp_action(Keypress kp);
// static void     __esc_special(char **buf);
// static Keypress __get_kp_from_char(char c);
// static Keypress __get_arrow_kp(char c);
// static void     __analize(char c);
// static Keypress __get_arrow(char *buf);
//
// void
// __kh_sudo_append(char *str)
// {
//     char *buf = str;
//     while (*buf)
//     {
//         /* If it is waiting for input and then
//          * it is called toggle, the first read
//          * would be executed if counter dont
//          * make read terminate.*/
//         if (*buf == 0x1b) // esc
//             /* It can be a escape keypress or a
//              * escape sequence, both cases are handled
//              * into the following function */
//             __esc_special(&buf);
//
//         else
//             __kp_action(__get_kp_from_char(*buf));
//
//         ++buf;
//     }
// }
//
// static Keypress
// __get_arrow(char *buf)
// {
//     Keypress kp;
//     int      supr_key;
//     int      supr_mod;
//     char     c;
//
//     if (buf == NULL)
//         printf("[DEBUG] NULL ENTRY\n");
//
//     else
//         sscanf(buf, "\x1b[%d;%d%c", &supr_key, &supr_mod, &c);
//
//     /* Get defaults */
//     kp = __get_arrow_kp(c);
//     __supr_get_mods(&kp, supr_mod, supr_key);
//
//     return kp;
// }
//
// static void
// __analize(char c)
// {
//     __kp_action(__get_kp_from_char(c));
// }
//
// static Keypress
// __get_kp_from_char(char c)
// {
//     Keypress kp;
//     /* defaults, can be changed in this function */
//     kp.mods = NO_MOD;
//     kp.c    = c;
//
//     switch (c)
//     {
//         // control keypress
//         case 0x0 ... 0x1F:
//             kp.mods |= (CTRL_MOD | SHIFT_MOD);
//             kp.c = c + '@'; // adjust key to representable key
//             break;
//
//             // clang-format off
//         case 'A' ... 'Z':
//             /* Chars that uses shift modifier */
//         case '<': case '_': case '>': case '?':
//         case ')': case '!': case '#': case '$':
//         case '%': case '*': case '(': case ':':
//         case '{': case '|': case '}': case '~':
//         case '@': case '^': case '&':
//             // clang-format on
//             kp.mods = SHIFT_MOD;
//             break;
//     }
//     return kp;
// }
//
// static Keypress
// __get_arrow_kp(char c)
// {
//     switch (c)
//     {
//         case 'A':
//             return ((Keypress) {
//             .c    = ARROW_UP,
//             .mods = IS_ARROW,
//             });
//
//         case 'B':
//             return ((Keypress) {
//             .c    = ARROW_DOWN,
//             .mods = IS_ARROW,
//             });
//
//         case 'C':
//             return ((Keypress) {
//             .c    = ARROW_RIGHT,
//             .mods = IS_ARROW,
//             });
//
//         case 'D':
//             return ((Keypress) {
//             .c    = ARROW_LEFT,
//             .mods = IS_ARROW,
//             });
//     }
//     return INVALID_KP;
// }
//
// static void
// __esc_special(char **buf)
// {
//     ssize_t  n;
//     Keypress kp;
//
//
//     if (buf == NULL)
//         return;
//
//     /* Get the remaining of the keypress and store it in
//      * buf as if all was read together. Given the bytes read
//      * it can be determined what is the type of the keypress */
//     switch (n = read(STDIN_FILENO, buf + 1, BUFSIZE - 1))
//     {
//         case 2:
//             /* All escape sequences start with \e[, so if the
//              * first characters appears but not the second it is not
//              * a scape sequence */
//             if ((*buf)[1] != '[')
//                 goto __normal__; // just to avoid nesting
//
//             /* Check if keypress is a single arrow without
//              * mods. (\e[A - \e[D representation ) If not,
//              * analize keypresses individually */
//             kp = __get_arrow_kp((*buf)[2]);
//             if (kh_valid_kp(kp))
//             {
//                 __kp_action(kp);
//                 return;
//             }
//
//         __normal__:
//             __analize((*buf)[0]);
//             __analize((*buf)[1]);
//             __analize((*buf)[2]);
//
//             return;
//
//         case 1:
//             // Alt mod
//             /* Analize single key for allow alt+ctrl */
//             kp = __get_kp_from_char((*buf)[1]);
//             /* This piece of code is only called
//              * if alt is used with ctrl without shift, or
//              * without mods, so as ctrl set shift mod I
//              * have to unset it manually. */
//             if (kp.mods & CTRL_MOD)
//                 kp.mods &= ~SHIFT_MOD;
//             /* Char is represented as upercase because it
//              * calls ctrl logic that is limited to upercase
//              * letters and a little symbols. */
//             kp.mods |= ALT_MOD;
//             __kp_action(kp);
//             return;
//
//         case 0:  // no input
//         case -1: // eof -> no input
//             __analize((*buf)[0]);
//             break;
//
//         default:
//             /* supr mods end with 'u'. Also
//              * alt-ctrl-shift-a, so I change some
//              * code to allow this behaviour.*/
//             if ((*buf)[n] == 'u')
//                 kp = __get_supr_kp(*buf);
//
//             /* arrow + mods end with 'A' - 'D' */
//             else if ((*buf)[n] >= 'A' && (*buf)[n] <= 'D')
//                 kp = __get_arrow(*buf);
//
//             __kp_action(kp);
//             break;
//     }
// }
//
// static void
// __kp_action(Keypress kp)
// {
//     Keybind  kb;
//     BindFunc func;
//
//     kb = kh_bind_new();
//     kh_bind_append(&kb, kp);
//
//     func = kh_bind_get(kb);
//     if (func)
//         func();
//     else
//         buffer_add(kp);
// }
