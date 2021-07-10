.include "consts.inc" # Constants

.equ CR0_TS, 0b100 # If bit is set, cause #NM exception on FPU op
.equ CR0_EM, 0b010 # If set, cause #UD on FPU op
.equ FORCE_FPU, 0xFFFFFFFF - (CR0_TS+CR0_EM) # Force FPU usage on FPU op

start_long_mode:
    mov $VGA_START, %edi
    mov $0xff20ef20cf20af20, %rax # String for coloured spaces
    mov $500, %ecx
    rep stosq # Store quad in %rax into VGA memory, rep 500 times

    /*
    # Test for FPU support
    mov %cr0, %edx # load CR0
    and $FORCE_FPU, %edx # unset TS and EM
    mov %edx, %cr0 # Save CR0

    fninit # Init FPU with defaults
    fnstsw $(.testword) # Store status into testword

    cmp $0, (.testword))
    */

    call KERNEL_SPACE # Call _start in kernel
    hlt
    jmp .

.testword: .word 0x56FE
