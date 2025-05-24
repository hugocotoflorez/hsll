#define ASCII_TABLE_REPR
#include "../include/vshkh.h"
#include <stdio.h>

/* Return one of the values above.
 * Arrowkey can be used as a boolean
 * value, so if return value is false,
 * keypress is not an arrow key. */
Arrowkey
kh_is_arrow(Keypress kp)
{
        if (kp.mods & IS_ARROW)
                return kp.c;

        return NO_ARROW;
}

/* Following functions return >0 if keypress has
 * the modifier in thekeypress or 0 otherwise */
inline int
kh_has_ctrl(Keypress kp)
{
        return kp.mods & CTRL_MOD;
}

inline int
kh_has_shift(Keypress kp)
{
        return kp.mods & SHIFT_MOD;
}

inline int
kh_has_alt(Keypress kp)
{
        return kp.mods & ALT_MOD;
}

inline int
kh_has_supr(Keypress kp)
{
        return kp.mods & SUPR_MOD;
}

inline int
kp_is_equal(Keypress kp1, Keypress kp2)
{
        return kp1.c == kp2.c && kp1.mods == kp2.mods;
}

int
kb_len(Keybind kb)
{
        for (int i = 0; i < KEYBINDLEN; i++)
                if (kp_is_equal(kb.kp[i], INVALID_KP))
                        return i;

        return KEYBINDLEN;
}

int
kb_left_match(Keybind prefix, Keybind kb)
{
        for (int i = 0; i < kb_len(prefix); i++)
                if (!kp_is_equal(kb.kp[i], prefix.kp[i]))
                        return 0;
        return 1;
}

/* Check if keypresses of both keybinds are the same,
 * return 0 is they are not equal, != 0 otherwise */
int
kb_is_equal(Keybind kb1, Keybind kb2)
{
        int len = kb_len(kb1);
        if (len != kb_len(kb2))
                return 0;

        for (int i = 0; i < len; i++) {
                if (!kp_is_equal(kb1.kp[i], kb2.kp[i]))
                        return 0;
        }
        return 1;
}

/* return >0 if keybind is valid or 0 otherwise */
inline int
kh_valid_kp(Keypress kp)
{
        return !kp_is_equal(kp, INVALID_KP);
}

/* return >0 if keybind is valid or 0 otherwise */
inline int
kh_valid_kb(Keybind kb)
{
        return !kb_is_equal(kb, INVALID_KB);
}

/* Print a keypress using symbols to represent mods.
 * This function can be used for debugging or for information,
 * but should not be used for final purposes */
void
kh_repr_kp(Keypress kp)
{
        if (kh_has_supr(kp))
                printf("❖");

        if (kh_has_ctrl(kp))
                printf("^");

        if (kh_has_alt(kp))
                printf("⎇");

        if (kh_has_shift(kp))
                printf("⇧");

        if (kh_is_arrow(kp))
                switch (kp.c) {
                case ARROW_UP:
                        printf("");
                        break;
                case ARROW_DOWN:
                        printf("");
                        break;
                case ARROW_LEFT:
                        printf("");
                        break;
                case ARROW_RIGHT:
                        printf("");
                        break;
                }

        else
                printf("%s", REPR[(int) kp.c]);

        putchar('\n');
}

/* Get a string that can be used to parse an arrow key with modifiers.
 * As arrows can be represented with a single char it is used # + a-d
 * or A-D for shift mod. This function does this automatically. */
char *
get_arrow_str(Arrowkey arrow, Mods mods, char *str)
{
        int i = 0;
        if (mods & CTRL_MOD)
                str[i++] = '^';
        if (mods & ALT_MOD)
                str[i++] = '&';
        if (mods & SUPR_MOD)
                str[i++] = '@';

        str[i++] = '#';

        str[i] = 'a' + arrow - 1;

        if (mods & SHIFT_MOD)
                str[i] -= 'a' - 'A';

        return str;
}

/* Get the representation form of KB and store it in S */
char *
kb_repr(Keybind kb, char *s)
{
        Keypress kp;
        int s_index = 0;
        for (int i = 0; i < KEYBINDLEN; i++) {
                kp = kb.kp[i];
                if (kh_has_supr(kp))
                        s[s_index++] = '@';

                if (kh_has_ctrl(kp))
                        s[s_index++] = '^';

                if (kh_has_alt(kp))
                        s[s_index++] = '&';

                if (kh_is_arrow(kp)) {
                        s[s_index++] = '#';
                        switch (kp.c) {
                        case ARROW_UP:
                                s[s_index++] = 'a';
                                break;
                        case ARROW_DOWN:
                                s[s_index++] = 'b';
                                break;
                        case ARROW_LEFT:
                                s[s_index++] = 'c';
                                break;
                        case ARROW_RIGHT:
                                s[s_index++] = 'd';
                                break;
                        }
                }

                else if (kh_has_shift(kp))
                        s[s_index++] = kp.c + 'A' - 'a';
                else
                        s[s_index++] = kp.c;
        }

        return s;
}
