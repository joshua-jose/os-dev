# The very first part of the bootloader
.code16
.global _start
.text

_start:
    mov $0x7c00, %bp # Move 0x7c00 (The base of our stack) into the bp (base pointer) register
    mov %bp, %sp # Move the bp value (base pointer) into the sp (stack pointer) register

    mov $msg, %bx # Move string pointer into bx
    call realprint # print

    jmp enter_protected_mode # attempt to enter protected mode
# End of _start

# GDT must be defined before trying to enter protected mode
.include "gdt.s"

enter_protected_mode:
    call enable_a20 # Enable the A20 line
    cli # Disable interrupts
    lgdt (gdt_descriptor) # Load our GDT
    # Set the protected mode bit
    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0

    # Long jump to start of protected mode
    jmp $codeseg, $start_protected_mode

.include "a20.s"
.include "printb.s"

# ----------------------------- 32 bits ------------------------------
.code32
# Enter 32 bit mode :)
start_protected_mode:
    # Load dataseg
    mov $dataseg, %ax
    # Point segment registers to new data seg
    mov %ax, %ds
    mov %ax, %ss
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    movb $'H, 0xb8000 # Move the letter into the first part of video memory
    movb $'e, 0xb8002 
    movb $'l, 0xb8004 
    movb $'l, 0xb8006 
    movb $'o, 0xb8008 

    movb $' , 0xb800a 
    movb $' , 0xb800c 
    movb $' , 0xb800e 

    jmp .
    hlt

# --------------------------------------------------------------------

msg: .asciz "Hello World!"

.org 510  # fill every remaining byte from the start to byte 510 with 0
.word 0xaa55 # last two bytes are 0x55AA, to make 512
