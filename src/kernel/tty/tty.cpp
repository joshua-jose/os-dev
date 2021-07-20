#include "kernel/kapi.hpp"
#include "tty.hpp"
#include "esh/esh.h"
#include "kernel/devices/display.hpp"
#include "kernel/devices/keyboard.hpp"
#include <cstring>
#include <cstdio>

void tty_print_cb(esh_t * esh, char c, void * arg)
{
    // prevent compiler from complaining
    (void) esh; 
    (void) arg;
    
    fputc(c);
}

void show_colours(){
    uint8_t colour_code = 0x1f;
    
    for (int z=0;z<5;z++){
        
        for (int i=0; i<2000;i+=2){
            // Put coloured space in two slots
            display_putchar(' ', colour_code, i);
            display_putchar(' ', colour_code, i+1);

            // Change the colour every two spaces
            colour_code+= 0x10;

            if (i%40 == 0) colour_code+= 0x10;
        }
        
        for (int i=0;i<0xFF00000;i++) asm("nop");
    }
    printk("\e[3J"); // ANSI code for screen clear
}


void tty_command_cb(esh_t * esh, int argc, char ** argv, void * arg)
{
    (void) esh;
    (void) arg;
    if (argc){
        if (!strcmp(argv[0], "help")) 
            printk("So... uhhh... there are no commands...\n");
        else if (!strcmp(argv[0], "colours")) 
            show_colours();
        else if (!strcmp(argv[0], "clear")) 
             printk("\e[3J");
        else
            printk("Command not found\n");       
    }
    else 
        printk("Command not found\n");
    
}

esh_t* esh;

void tty_init(){
    esh = esh_init();
    esh_register_command(esh, tty_command_cb);
    esh_register_print(esh, tty_print_cb);

    esh_rx(esh, '\n');
    esh_hist_init(esh);

};

void tty_input_loop(){
    for (;;) {
        int c = keyboard_buffer_getc();
        
        if (c > 0 && c <= 255)
            esh_rx(esh, c);
        
    }
}