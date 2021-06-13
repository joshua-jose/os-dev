.code16
.global _start

_start:
    mov $0x7c00, %bp # Move 0x7c00 (The base of our stack) into the bp (base pointer) register
    mov %bp, %sp # Move the bp value (base pointer) into the sp (stack pointer) register

    mov $msg, %bx
    call printf

    hlt # halt
    jmp . # Infinite loop

.include "print.s"

msg: .asciz "Hello World!"

.fill 510-(.-_start), 1, 0 # fill every remaining byte from the start to byte 510 with 0
.word 0xaa55 # last two bytes are 0x55AA, to make 512
