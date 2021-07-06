#include "keyboard.hpp"
#include "kernel/tty/esh.h"

const char scancode_to_char[] = {
         0 , 27 , '1', '2',
        '3', '4', '5', '6',
        '7', '8', '9', '0',
        '-', '=',  8 ,  9 ,
        'q', 'w', 'e', 'r',
        't', 'y', 'u', 'i',
        'o', 'p', '[', ']',
       '\n',  0 , 'a', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', ';',
        '\'','`',  0 , '\\',
        'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',',
        '.', '/',  0 , '*',
         0 , ' '
};

// 0x1D - Left ctrl pressed
// 0x2A	- left shift pressed
// 0x36	- right shift pressed
// 0x38	- left alt pressed
// 0x36	- right shift pressed
// 0x3A	- CapsLock pressed

// 0x80 + key : key released

extern esh_t* esh;

void keyboard_recieve_scancode(scancode_t scancode){
    static int x=0;
     if (scancode <= 58){
        char key = scancode_to_char[scancode];
        // Keep the enter key
        if ((int)key < 32 && scancode != 0x1C) return;
        esh_rx(esh, key);
        //printk(&key,x);
        x++;
    }

}