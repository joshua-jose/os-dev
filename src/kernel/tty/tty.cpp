#include "kernel/kapi.hpp"
#include "tty.hpp"
#include "esh.h"

int strcmp(const char *X, const char *Y)
{
    while (*X) {
        // if characters differ, or end of the second string is reached
        if (*X != *Y) 
            break;
        
        // move to the next pair of characters
        X++;
        Y++;
    }
 
    // return the ASCII difference after converting `char*` to `unsigned char*`
    return *(const unsigned char*)X - *(const unsigned char*)Y;
}

void esh_print_cb(esh_t * esh, char c, void * arg)
{
    // prevent compiler from complaining
    (void) esh; 
    (void) arg;
    
    fputc(c);
}

void show_colours(){
    uint8_t colour_code = 0x1f;
    uint8_t character = 0x20;
    int phrase = (colour_code << 8) | character;
    
    for (int z=0;z<5;z++){
        
        for (int i=0; i<2000;i+=2){
            // Put coloured space in two slots
            vmem[i] = phrase;
            vmem[i+1] = phrase;

            // Change the colour every two spaces
            colour_code+= 0x10;
            phrase = (colour_code << 8) | character;

            if (i%40 == 0) colour_code+= 0x10;
        }
        
        for (int i=0;i<0xFF00000;i++) asm("nop");
    }
    for (int i=0; i<2000;i++)
        vmem[i] = 0x0f20;
}


void esh_command_cb(esh_t * esh, int argc, char ** argv, void * arg)
{
    (void) esh;
    (void) arg;
    if (argc){
        if (!strcmp(argv[0], "help")) 
            printk("So... uhhh... there are no commands...\n");
        else if (!strcmp(argv[0], "colours")) 
            show_colours();
        else
            printk("Command not found\n");       
    }
    else 
        printk("Command not found\n");
    
}

esh_t* esh;

void tty_init(){
    esh = esh_init();
    esh_register_command(esh, esh_command_cb);
    esh_register_print(esh, esh_print_cb);

    esh_rx(esh, '\n');
};