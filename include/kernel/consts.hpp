#pragma once

#define VGA_START 0xB8000
#define GDT_OFFSET_KERNEL_CODE 0x08

// Memory is volatile - Don't optimise it away even if it seems nothing is happening
// 16 bits per character, and the pointer is constant
volatile uint16_t* const vmem = (volatile uint16_t*)VGA_START;
