#include <cstdint>
#include "kernel/kapi.hpp"

 #define LINE_LENGTH 80

void kputc(char c, uint8_t colour_code=0x0f){
    static int column = 0;
    static int line = 0;

    if (c == '\n'){
        column = 0;
        line++;
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
}

void printk(char* in_string,int start_column, uint8_t colour_code){

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
