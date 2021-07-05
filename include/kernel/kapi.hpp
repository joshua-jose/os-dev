#pragma once
#include <cstdint>

#define VGA_START 0xB8000
#define GDT_OFFSET_KERNEL_CODE 0x08
#define NUM_INTERRUPTS 256

// Memory is volatile - Don't optimise it away even if it seems nothing is happening
// 16 bits per character, and the pointer is constant
volatile uint16_t* const vmem = (volatile uint16_t*)VGA_START;

void printk(char* in_string,int start_column=0, uint8_t colour_code=0x0f);

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}
