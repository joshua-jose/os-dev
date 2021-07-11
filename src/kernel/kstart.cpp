#include "kernel/kapi.hpp"
#include "kernel/interrupt/interrupts.hpp"
#include "kernel/tty/tty.hpp"
#include <stdio.h>

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

    char buf[30];
    sprintf(buf,"testing sprintf: %d,%d\n",1,2);
    printk(buf);

    interrupts_init();
    tty_init();


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
extern "C"
__attribute__ ((section("._start")))
void  _start(){
    // Initialize C runtime environment

    // Make sure BSS is initialised to 0
    for (uintptr_t *bss_ptr = &_sbss; bss_ptr < &_ebss;) {
        *bss_ptr++ = 0;
    }

    // Make sure that the data section is initialized
    uintptr_t *init_values_ptr = &_etext;
    uintptr_t *data_ptr = &_sdata;
    if (init_values_ptr != data_ptr) {
        for (; data_ptr < &_edata;) {
            *data_ptr++ = *init_values_ptr++;
        }
    }

    kmain();
}

