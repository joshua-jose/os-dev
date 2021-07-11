#include "kernel/kapi.hpp"
#include "idt.hpp"
#include "interrupts.hpp"
#include "isr.hpp"
#include "kernel/devices/keyboard.hpp"

//#pragma GCC push_options
//#pragma GCC target "no-sse"

// An array of ISRs (the actual func type pointers)
static void (*ISRs[NUM_INTERRUPTS])(interrupt_frame_t*);

// Save floating point register states
char fxsave_region[512] __attribute__((aligned(16)));

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

void keyboard_isr(interrupt_frame_t*){
    asm volatile("fxsave %0;"::"m"(fxsave_region));
    // Read scancode from keyboard device
    scancode_t scancode = inb(0x60);
    // Tell driver we recieved a scancode
    keyboard_recieve_scancode(scancode);

    // Send an ack (0x20) to the PIC (on port 0x20)
    outb(0x20,0x20);
    asm volatile("fxrstor %0;"::"m"(fxsave_region));
}
//#pragma GCC pop_options
