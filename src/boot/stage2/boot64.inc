.include "consts.inc" # Constants

start_long_mode:
    mov $VGA_START, %edi
    mov $0xff20ef20cf20af20, %rax # String for blue spaces
    mov $500, %ecx
    rep stosq # Store quad in %rax into VGA memory, rep 500 times

    call KERNEL_SPACE # Call _start in kernel
    hlt
    jmp .
