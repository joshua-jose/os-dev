.include "consts.inc" # Constants

start_long_mode:
    mov $VGA_START, %edi
    mov $0xff20ef20cf20af20, %rax # String for coloured spaces
    mov $500, %ecx
    rep stosq # Store quad in %rax into VGA memory, rep 500 times
    mov %bp, %sp # Clear the stack 
    
    mov (LOAD_AREA+0x18), %rax # Program entry point

    # TODO: get entry point from ELF
    call *%rax # Call _start in kernel (at address in rax)
    hlt
    jmp .

// Address where the program entry point is
elf_entry_point: .quad 0
