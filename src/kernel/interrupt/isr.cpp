#include "kernel/kapi.hpp"
#include "kernel/interrupt/idt.hpp"
#include "kernel/interrupt/interrupts.hpp"
#include "kernel/interrupt/isr.hpp"

// An array of ISRs (the actual func type pointers)
static void (*ISRs[NUM_INTERRUPTS])(interrupt_frame_t*);

void ISRs_register(uintptr_t* interrupt_vectors){
     for (int i=0;i<NUM_INTERRUPTS;i++)
        ISRs[i] = generic_isr;
    
    ISRs[8+1] = keyboard_isr; // Keyboard interrupt (IRQ1, @ vector 8+1)

    for (int i=0;i<NUM_INTERRUPTS;i++)
        interrupt_vectors[i] = (uintptr_t)ISRs[i];
        
}

// Generic ISR handler
void generic_isr(interrupt_frame_t*){
    // Send an ack (0x20) to the PIC (on port 0x20)
    outb(0x20,0x20);
}

const char scancode_to_char[] = {
         0 ,  0 , '1', '2',
        '3', '4', '5', '6',
        '7', '8', '9', '0',
        '-', '=',  0 ,  0 ,
        'q', 'w', 'e', 'r',
        't', 'y', 'u', 'i',
        'o', 'p', '[', ']',
         0 ,  0 , 'a', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', ';',
        '\'','`',  0 , '\\',
        'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',',
        '.', '/',  0 , '*',
         0 , ' '
};

static int x=0;
void keyboard_isr(interrupt_frame_t*){
    // Read scancode from keyboard device
    uint8_t scancode = inb(0x60);

    if (scancode <= 58){
        char key = scancode_to_char[scancode];
        printk(&key,x);
        x++;
    }
    
    // Send an ack (0x20) to the PIC (on port 0x20)
    outb(0x20,0x20);
}

