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


void keyboard_recieve_scancode(scancode_t scancode);
void keyboard_scancode_to_key(scancode_t scancode, bool escaped=false);

void keyboard_bitmap_set(scancode_t scancode,bool escaped=false);
bool keyboard_bitmap_get(scancode_t scancode, bool escaped=false);