#include "kernel/kapi.hpp"
#include "kernel/interrupt/interrupts.hpp"

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

// Define _start, and put it in special section ._start
// This means the linker can put it at the start of the sector
// Also define as a C style function, so the name isn't mangled
extern "C"
__attribute__ ((section("._start")))
void  _start(){
    // Load memory manager
    // Load file system driver
    // Load k modules into memory and link

    interrupts_init();
    /*
    uint8_t colour_code = 0x1f;
    uint8_t character = 0x20;
    int phrase = (colour_code << 8) | character;
    char test[] = "64 bit C++ test code\ntest test";
    
    while (true){
        
        for (int i=0; i<2000;i+=2){
            // Put coloured space in two slots
            vmem[i] = phrase;
            vmem[i+1] = phrase;

            // Change the colour every two spaces
            colour_code+= 0x10;
            phrase = (colour_code << 8) | character;

            if (i%40 == 0) colour_code+= 0x10;
        }
        
        printk(test);
        for (int i=0;i<0xFF00000;i++) asm("nop");
    }
   */
};
