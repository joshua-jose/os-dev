# The very first part of the bootloader
.code16
.global _start
.text

.include "consts.inc" # Constants

_start:
    # BIOS puts the number of the boot disk into %dl at the start
    mov %dl, (BOOT_DISK) # Move the disk number into the BOOT_DISK VARIABLE

    mov $0x7c00, %bp # Move 0x7c00 (The base of our stack) into the bp (base pointer) register
    mov %bp, %sp # Move the bp value (base pointer) into the sp (stack pointer) register

    mov $msg, %bx # Move string pointer into bx
    call realprint # print

    # Tell BIOS that we intend to end up in 64 bit long mode
    mov $0xEC00, %ax # Magic number for mode telling 
    mov $2, %bl # 2 is for long mode
    int $0x15 # Tell the BIOS

    call load_stage_2 # Load the second stage of the bootloader
    call enable_a20 # Enable the A20 line
    call load_kernel # Load the kernel into memory
    
    cli # Disable interrupts
    jmp STAGE2_SPACE # Enter stage 2 of bootloader, attempt to enter protected mode.
# End of _start

load_stage_2:
    mov $STAGE2_SPACE, %bx # Read to STAGE2_SPACE, where stage 2 will be loaded
    mov $STAGE2_SECTORS, %al # Read enough sectors to read stage 2
    mov $0x02, %cl  # Select sector
    
    call disk_load # Load data from disk using above arguments
    ret

.include "printb.inc"
.include "a20.inc"
.include "diskload.inc"
.include "kernel_loader.inc"

msg: .asciz "Hello World!\r\n"
read_failed_msg: .asciz "\nDisk read failed"
BOOT_DISK: .byte 0 # The disk we booted from


.org 510  # fill every remaining byte from the start to byte 510 with 0
.word 0xaa55 # last two bytes are 0x55AA, to make 512
