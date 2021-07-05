#include "kernel/kapi.hpp"
#include "kernel/interrupt/idt.hpp"
#include "kernel/interrupt/interrupts.hpp"
#include "kernel/interrupt/ISRs.hpp"

void interrupts_init(){
    // Pass to idt_init an array of function pointers (64 bit addresses)
    uintptr_t interrupt_vectors[NUM_INTERRUPTS];

    // Find all ISRs and put in array
    ISRs_register((uintptr_t*)&interrupt_vectors);

    // Initialise the IDT
    idt_init(interrupt_vectors);
}