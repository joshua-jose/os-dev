#pragma once
#include "kernel/kapi.hpp"

#define PIC1		    0x20		/* IO base address for master PIC */
#define PIC2		    0xA0		/* IO base address for slave PIC */

// PIC IO ports
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	   (PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	   (PIC2+1)

#define PIC_EOI	    	0x20		/* End-of-interrupt command code */

#define ICW1_INIT	    0x10		/* Initialization - required! */
#define ICW1_ICW4	    0x01		/* ICW4 (not) needed */
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define PIC1_ADD_SECONDARY 4
#define PIC2_SET_CASCADE 2 

#define IRQ_DEFAULT_OFFSET 8

void pic_ack(int irq);
void pic_remap(int new_offset);
int pic_get_irq_offset();