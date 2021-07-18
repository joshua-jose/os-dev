#pragma once
#include "kernel/kapi.hpp"
#include <cstdint>

#define VGA_START 0xB8000
#define VGA_LINE_LENGTH 80
#define VGA_SCREEN_HEIGHT 25
#define NUM_CHARS VGA_LINE_LENGTH*VGA_SCREEN_HEIGHT

void display_init();
void display_clear();
void display_putchar(char c, uint8_t colour_code);
void display_putchar(char c, uint8_t colour_code, int offset);

void terminal_init();
void terminal_inc_column(int n=1);
void terminal_dec_column(int n=1);
void terminal_update_cursor();

void terminal_scroll();
void terminal_putc(char c);
void terminal_clear_line(int line);
void terminal_fg_colour();
void terminal_bg_colour();
bool terminal_handle_ansi(char c);
