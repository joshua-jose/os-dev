#include <cstdint>
#define VGA_START 0xB8000

// Define _start, and put it in special section ._start
// This means the linker can put it at the start of the sector
extern "C" void _start() __attribute__ ((section("._start")));

// Memory is volatile - Don't optimise it away even if it seems nothing is happening
// 16 bits per character, and the pointer is constant
volatile uint16_t* const vmem = (volatile uint16_t*)VGA_START;

struct GDT{

};

typedef struct __attribute__((packed)){
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

} page_entry_t;

void printk(char* in_string, uint8_t colour_code=0x0f){

    int line = 0;
    int column = 0;
    // Loop until character is null
    for(int i=0; in_string[i]!='\0'; i++){
        
        // New line causes it to move to next line, and resets column
        if (in_string[i] == '\n'){
            line++;
            column = 0;
            continue;
        }

        int offset = (line*80)+column; // Position on screen
        uint16_t code = (colour_code << 8) | (uint8_t)in_string[i];
        vmem[offset] = code;
        column++;
    }
}

void  _start(){
    // Load memory manager
    // Load file system driver
    // Load k modules into memory and link
    
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
   
};
