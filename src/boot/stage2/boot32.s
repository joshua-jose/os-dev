.code16
.text

# Entry point
jmp enter_protected_mode

# GDT must be defined before trying to enter protected mode
.include "stage2/gdt.inc"

enter_protected_mode:
    lgdt (gdt_descriptor) # Load our GDT
    # Set the protected mode bit
    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0

    # Long jump to start of protected mode
    jmp $codeseg, $start_protected_mode

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

    # Clear the screen
    mov $2000, %ecx # Loop 80*25 times (size of video memory)
    mov $0xb8000, %ebx # Start of video memory
    clear:
        movb $' , (%ebx, %ecx,2) # Move a space into (start + offset * 2)
    loop clear # Loop 2000 times

    # Test print
    mov $msg2, %ebx # Move string pointer into ebx
    mov $0xb8000, %ecx # Move start of memory into ecx
print_looptop:
    mov (%ebx), %al # Load a character into ax 
    cmp $0, %al # Check if ax (a character) is 0
    je print_end # if the char is 0, end

    movb %al, (%ecx) # Move the character into video memory
    inc %ebx # Increment string pointer
    add $2, %ecx # Increment video memory pointer by 2

    jmp print_looptop # return to top of loop

print_end:
    jmp .

    hlt

# --------------------------------------------------------------------

msg2: .asciz "Hello World from 32 bits!"
.fill 2048-(.-.text), 1, 0  # fill every remaining byte from the start to byte 2048 with 0

