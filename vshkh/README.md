# vshkh /vɛsk/

Very Simple Hugo's Keyboard Handler

## About

Description should be written here.

### Pronunciation
"vshkh" is pronounced as /vɛsk/, which rhymes with "desk" and "pesk."
- *V* as in "van"
- *E* as in "bet"
- *S* as in "sit"
- *K* as in "kick"

## User Interface

### Basic usage

Create a bind
```c
void func() {printf("Hello\n");}
kh_bind_create("@^h", func);
```

Initialize the handler
``` c
kh_start();
```
Read if has input
```c
Keypress kp;
kp = kh_wait();
```
Close the handler
```c
kh_end();
```

Function descriptions and those stuff are written in vshkh.h

## Binds formatting

### Mods parsing format

Using 'a' as example char

| Mods | format |
| :---: | :---: |
| **Normal** | "a" |
| **Shift** | "A" |
| **ctrl** | "^A" |
| **alt** | "&a" |
| **Supr** | "@a" |
| **Shift + ctrl** | Not allowed |
| **Shift + alt** | "&A" |
| **Shift + supr** | "@A" |
| **Shift + ctrl + alt** | "&^A" |
| **Shift + ctrl + supr** | "^@A" |
| **Shift + alt + supr** | &@A" |
| **Shift + ctrl + alt + supr** | "^&@A" |
| **ctrl + alt** | "^&a" |
| **ctrl + supr** | "^@a" |
| **ctrl + alt + supr** | "^&@a" |
| **alt + supr** | "&@a" |

| Arrow | format |
| :---: | :---: |
| **UP** | "#a" |
| **DOWN** | "#b" |
| **LEFT** | "#c" |
| **RIGHT** | "#d" |
| **Shift + up** | "#A" |
| **Shift + ctrl + alt + supr up** | "^&@#A" |

> [!CAUTION]
> Ctrl without supr dont allow lowercase chars

## Potential issues + todos

- **Potential issue: CTRL + lowercase is read as ctrl + upercase and ctrl + shift cant be read**
- CTRL is too limited
- CTRL + SHIFT + KEY: crash
- CTRL + 1,9,0 dont exists
- right + left + down at the same time breaks the handler. with -O3 it doesnt happened xd

