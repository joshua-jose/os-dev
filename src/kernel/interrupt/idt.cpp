#include "kernel/kapi.hpp"
#include "kernel/interrupt/idt.hpp"

__attribute__((aligned(0x1000))) 
static idt_entry_t idt[NUM_INTERRUPTS]; // The interrupt descriptor table
static idtr_t idtr; // The IDT descriptor that will be loaded

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];
 
    descriptor->isr_low       = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs     = GDT_OFFSET_KERNEL_CODE;
    descriptor->ist           = 0;
    descriptor->attributes    = flags;
    descriptor->isr_mid       = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high      = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved      = 0;
}

// Initialise interrupts
void idt_init(uintptr_t* interrupt_vectors){
    idtr.base = (uintptr_t)&idt[0]; // address of base of idt
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * NUM_INTERRUPTS; // Size of idt (num_entries * sizeof_entry)

    for (uint16_t vector = 0; vector < NUM_INTERRUPTS; vector++) {
        idt_set_descriptor(vector, (void*)interrupt_vectors[vector], 0x8E);
    }

    // Only enable keyboard interrupts (rest are undesired)
    outb(0x21,0xfd);
    outb(0xa1,0xff);
 
    asm volatile ("lidt %0" : : "memory"(idtr)); // load the new IDT
    asm volatile ("sti"); // set the interrupt flag
}