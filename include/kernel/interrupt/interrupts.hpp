#pragma once
#include "kernel/kapi.hpp"
#include "kernel/interrupt/idt.hpp"


#include <functional>

// The table of isr stub pointers
extern uintptr_t isr_table[NUM_INTERRUPTS];

void interrupts_init();
void irq_ack();
void irq_register(int8_t irq_number, std::function<void()> handler);

extern "C"
void interrupt_handler(int vector);

void keyboard_isr();
void timer_isr();