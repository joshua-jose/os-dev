#include <cstdint>

// Define _start, and put it in special section ._start
// This means the linker can put it at the start of the sector
// Also define as a C style function, so the name isn't mangled

extern uint8_t _sbss;
extern uint8_t _ebss;
extern uint8_t _etext;
extern uint8_t _sdata;
extern uint8_t _edata;
extern uint8_t _end;

extern "C" void __libc_init_array();
extern "C" void _init() {return;}

// Kernel main function
void kmain();

extern "C"
__attribute__ ((section("._start")))
void  _start(){
    // Initialize C runtime environment

    // Make sure BSS is initialised to 0
    for (uint8_t *bss_ptr = &_sbss; bss_ptr < &_ebss;) {
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