#include <cstdint>
#include "kernel/kapi.hpp"


void printk(char* in_string,int start_column, uint8_t colour_code){

    int line = 0;
    int column = start_column;
    // Loop until character is null
    for (int i=0; in_string[i]!='\0'; i++){
        
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