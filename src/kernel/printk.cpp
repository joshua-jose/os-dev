#include <cstdint>
#include "kernel/kapi.hpp"
#include "kernel/devices/display.hpp"


void printk(const char* in_string,int start_column){

    (void) start_column;
    // Loop until character is null
    for (int i=0; in_string[i]!='\0'; i++){
        // Send character to display
        terminal_putc(in_string[i]);
    }
}

void fputc(char c){
    terminal_putc(c);
}
