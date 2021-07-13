#include "pic_driver.hpp"

// We put PIC1 and PIC2 IRQs next to teachother for simplicity 
static int irq_offset = IRQ_DEFAULT_OFFSET; 

void pic_remap(int new_offset){

    // To remap the PIC, we need to initialise it
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // Initialise without ICW4
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    outb(PIC1_DATA, new_offset);               // ICW2: PIC1 vector offset
	outb(PIC2_DATA, new_offset+8);             // ICW2: PIC2 vector offset

    outb(PIC1_DATA, PIC1_ADD_SECONDARY);       // ICW3: tell PIC1 that there is a PIC2 at IRQ2 (0000 0100)
	outb(PIC2_DATA, PIC2_SET_CASCADE);         // ICW3: tell Slave PIC its cascade identity (0000 0010)

    outb(PIC1_DATA, ICW4_8086);                // Put PIC into x86 mode
	outb(PIC2_DATA, ICW4_8086);
    
    irq_offset = new_offset;

}

void pic_ack(int irq){
    // If the IRQ is from PIC2, send it an ack too
    if(irq >= 8)
      outb(PIC2_COMMAND,PIC_EOI);
    // Send an ack (0x20) to PIC1 (on port 0x20)
    outb(PIC1_COMMAND, PIC_EOI);
}

int pic_get_irq_offset(){
    return irq_offset;
}