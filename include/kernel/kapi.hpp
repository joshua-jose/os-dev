#pragma once
#include <cstdint>

#define GDT_OFFSET_KERNEL_CODE 0x08
#define NUM_INTERRUPTS 256
#define IRQ_START 8

void printk(const char* in_string, int start_column=0);
void fputc(char c);

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
