#pragma once
#include "kernel/kapi.hpp"
#include "kernel/interrupt/idt.hpp"

void ISRs_register(uintptr_t* interrupt_vectors);

__attribute__((__interrupt__))
void generic_isr(interrupt_frame_t*);

__attribute__((__interrupt__))
void keyboard_isr(interrupt_frame_t*);