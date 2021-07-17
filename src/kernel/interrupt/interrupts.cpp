#include "kernel/kapi.hpp"
#include "idt.hpp"
#include "interrupts.hpp"
#include "pic_driver.hpp"
#include "kernel/devices/keyboard.hpp"
#include <stdio.h>

static char fxsave_region[512] __attribute__((aligned(16))); // Save floating point register states
static std::function<void()> IRQs[15]; // An array of ISRs (the actual func type pointers)

void interrupts_init(){
    pic_remap(32); // Remap PIC interrupts to above CPU exceptions
    // Only enable keyboard interrupts and timer interrupts (rest are undesired)
    outb(0x21,0xfc);
    outb(0xa1,0xff);

    irq_register(0, timer_isr);
    irq_register(1, keyboard_isr);

    // Initialise the IDT with our table of interrupt pointers
    idt_init(isr_table);
}

void irq_register(int8_t irq_number, std::function<void()> handler){
    IRQs[irq_number] = handler;
}

// -------------------------------------------------------------------

void interrupt_handler(int vector){
    // Store floating point register
    asm volatile("fxsave %0;"::"m"(fxsave_region));
    //__builtin_ia32_fxsave64 ((void*)&fxsave_region);

    int irq_offset = pic_get_irq_offset(); // Offset could have changed!
    // Is this interrupt within the region of IRQs?
    if (vector >= irq_offset && vector <= irq_offset+8){
        int8_t IRQ_vector =  vector - irq_offset;
        if (IRQs[IRQ_vector]) // Is there a function assigned to this IRQ?
            IRQs[IRQ_vector]();

        pic_ack(IRQ_vector);
    }
    
    asm volatile("fxrstor %0;"::"m"(fxsave_region));
    //__builtin_ia32_fxrstor64 ((void*)&fxsave_region);
}

// TODO: move.
// Make keyboard register one of these
void keyboard_isr(){
    // Read scancode from keyboard device
    scancode_t scancode = inb(0x60);
    // Tell driver we recieved a scancode
    keyboard_recieve_scancode(scancode);
}
// get display handler to register a cb to this
void timer_isr(){
    fflush(stdout);
}