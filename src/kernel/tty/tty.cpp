#include "kernel/kapi.hpp"
#include "tty.hpp"
#include "esh.h"

int strcmp(const char *X, const char *Y)
{
    while (*X) {
        // if characters differ, or end of the second string is reached
        if (*X != *Y) {
            break;
        }
 
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


void esh_command_cb(esh_t * esh, int argc, char ** argv, void * arg)
{
    (void) esh;
    (void) arg;
    (void) argc;
    (void) argv;

    
    if (argc && !strcmp(argv[0], "help")) {
        const char help_string[] = "So... uhhh... there are no commands...\n";
        printk((char*)&help_string);
    }    
}

esh_t* esh;

void tty_init(){
    esh = esh_init();
    esh_register_command(esh, esh_command_cb);
    esh_register_print(esh, esh_print_cb);

    esh_rx(esh, '\n');
};