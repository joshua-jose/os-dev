#include <cstdint>
#include "kernel/kapi.hpp"

 #define LINE_LENGTH 80

void enable_cursor(uint8_t cursor_start=0, uint8_t cursor_end=15) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void update_cursor(int column, int line) {
    uint16_t pos = line * LINE_LENGTH + column;
 
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void kputc(char c, uint8_t colour_code=0x0f){
    static int column = 0;
    static int line = 0;

    if (c == '\n'){
        column = 0;
        line++; // Goes to start of next line
    }
    else if (c == '\b'){ 
        // if not at start of column, go up
        if (column > 0)
            column--;
        else{
            // Go to end of line above
            column = 79;
            if (line > 0)
                line--;
        }
    }
    else{
        int offset = (line*LINE_LENGTH)+column; // Position on screen
        uint16_t code = (colour_code << 8) | c;
        vmem[offset] = code;
        column++;

        if (column % LINE_LENGTH == 0){
            column = 0;
            line++;
        };
    }

    if (line >= 25){
            line = 0;
            // Clear screen
            for (int i=0; i<2000;i++)
                vmem[i] = 0x0f20;
    }
    update_cursor(column, line);
}

void printk(const char* in_string,int start_column, uint8_t colour_code){

    (void) start_column;
    // Loop until character is null
    for (int i=0; in_string[i]!='\0'; i++){
        // Send character to display
        kputc(in_string[i], colour_code);
    }
}



void fputc(char c){
    kputc(c);
    
}