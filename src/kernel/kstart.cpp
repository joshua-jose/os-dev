#include "kernel/kapi.hpp"
#include "kernel/interrupt/interrupts.hpp"
#include "kernel/tty/tty.hpp"
#include <stdio.h>
#include <string>

/*
Todo:
recompile compiler without SSE
proper display/ terminal manager (rewrite printk.cpp into display/.., seperate VGA display driver and terminal code)
dont get keyboard to directly call the shell rx, get it to put keys into a stack / file or smth somewhere, best to put it
in stdin actually...
get keyboard driver to register an interrupt handler itself
Memory management (PMM, VMM, PFA, get memory map from bootloader, GDT, LDT, paging)
compile kernel as ELF file and let bootloader load that
bootloader reads FS (fat 16) 
higher-half kernel
change fs to fat 16
CPU exception handler
move _start to crt0
handle spurious IRQs
use APIC instead of PIC
replace newlib memory allocator with something like liballoc so we can use mmap()
AHCI support
keyboard handle different scancode sets
simple fs driver that lets other drivers hook /dev/..
display fb appears under /dev/fb
put TTY/ stdout/stdin under dev/..
keyboard needs to support multiple layouts (load keyboard layout as config file dynamically?)
keyboard -> display pipeline should handle UTF-8 strings
ATA fs driver (with fat 16 driver)
PCI support
    USB support
    USB mass storage
Video mode changing

kernel task scheduler
multiple processor support
mouse support
Userland...
SVGAlib inclusion
X server/ LVGL?
run doom
*/

struct GDT{

};

typedef struct{
    uint64_t present:1;
    uint64_t write_enabled:1;
    uint64_t user_access:1;
    uint64_t write_through:1;
    uint64_t cache_disable:1;
    uint64_t accessed:1; 
    uint64_t dirty:1;
    uint64_t reserved_0: 1;
    uint64_t global:1;
    uint64_t reserved_1:2;
    uint64_t page_address:52;
    uint64_t no_execute:1;

}__attribute__((packed)) page_entry_t;


void kmain(){
    // Load memory manager
    // Load file system driver
    // Load k modules into memory and link

    // Clear screen
    for (int i=0; i<2000;i++)
        vmem[i] = 0x0f20;

    interrupts_init();
    tty_init();
    printf("Test");

    while (true)
        asm volatile ("hlt;jmp .-1;");
    

    /*
    for (;;) {
        int c = getchar();
        if (c > 0 && c <= 255) {
            if (c == '\r') {
                c = '\n';
            }
            esh_rx(esh, c);
        }
    }
    */
};

extern uintptr_t _sbss;
extern uintptr_t _ebss;
extern uintptr_t _etext;
extern uintptr_t _sdata;
extern uintptr_t _edata;
extern uintptr_t _end;

// Define _start, and put it in special section ._start
// This means the linker can put it at the start of the sector
// Also define as a C style function, so the name isn't mangled

extern "C" void __libc_init_array();
extern "C" void _init(void) {;}

extern "C"
__attribute__ ((section("._start")))
void  _start(){
    // Initialize C runtime environment

    // Make sure BSS is initialised to 0
    for (uintptr_t *bss_ptr = &_sbss; bss_ptr < &_ebss;) {
        *bss_ptr++ = 0;
    }

    /*
    // Make sure that the data section is initialized
    uintptr_t *init_values_ptr = &_etext;
    uintptr_t *data_ptr = &_sdata;
    if (init_values_ptr != data_ptr) {
        for (; data_ptr < &_edata;) {
            *data_ptr++ = *init_values_ptr++;
        }
    }
    */

    // Run C++ constructors / any other initialisation
    __libc_init_array();

    kmain();
}

