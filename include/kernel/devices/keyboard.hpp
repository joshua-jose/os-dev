#pragma once
#include "kernel/kapi.hpp"

typedef uint8_t scancode_t;

#define ESCAPE_CODE 0xE0
#define RELEASED_BIT 0x80
#define SINGLE_BYTE_END 0x60
#define BITMAP_TABLE_SIZE 8

#define KEY_CAPS_LOCK 0x3A
#define KEY_LEFT_SHIFT 0x2A
#define KEY_RIGHT_SHIFT 0x36
#define KEY_LEFT_CTRL 0x1D

// Escaped codes
#define KEY_RIGHT_CTRL 0x1D
#define KEY_HOME 0x47
#define KEY_CURSOR_UP 0x48
#define KEY_CURSOR_DOWN 0x50
#define KEY_CURSOR_LEFT 0x4B
#define KEY_CURSOR_RIGHT 0x4D
#define KEY_DELETE 0x53
#define KEY_END 0x4F

// ASCII codes
const char scancode_to_char[] = {
         0 , 27 , '1', '2',
        '3', '4', '5', '6',
        '7', '8', '9', '0',
        '-', '=', '\b', 9 ,
        'q', 'w', 'e', 'r',
        't', 'y', 'u', 'i',
        'o', 'p', '[', ']',
       '\n',  0 , 'a', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', ';',
        '\'','`',  0 , '\\',
        'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',',
        '.', '/',  0 , '*',
         0 , ' '
};

const char shifted_scancode_to_char[] = {
         0 , 27 , '!', '\"',
        '#', '$', '%', '^',
        '&', '*', '(', ')',
        '_', '+', '\b', 9 ,
        'Q', 'W', 'E', 'R',
        'T', 'Y', 'U', 'I',
        'O', 'P', '{', '}',
       '\n',  0 , 'A', 'S',
        'D', 'F', 'G', 'H',
        'J', 'K', 'L', ':',
        '@','~',  0 , '|',
        'Z', 'X', 'C', 'B',
        'B', 'N', 'M', '<',
        '>', '?',  0 , '*',
         0 , ' ' 
};

void keyboard_recieve_scancode(scancode_t scancode);
void keyboard_scancode_to_key(scancode_t scancode, bool escaped=false);

void keyboard_bitmap_set(scancode_t scancode,bool escaped=false);
bool keyboard_bitmap_get(scancode_t scancode, bool escaped=false);