# The very first part of the bootloader
.code16
.global _start
.text

# BIOS loads boot sector to 0x7C00 to 0x7DFF, so load right after
.equ STAGE2_SPACE, 0x7e00

_start:
    mov %dl, (BOOT_DISK) # Move the disk number into the BOOT_DISK VARIABLE

    mov $0x7c00, %bp # Move 0x7c00 (The base of our stack) into the bp (base pointer) register
    mov %bp, %sp # Move the bp value (base pointer) into the sp (stack pointer) register

    mov $msg, %bx # Move string pointer into bx
    call realprint # print

    call load_stage_2 # Load the second stage of the bootloader

    call enable_a20 # Enable the A20 line
    cli # Disable interrupts
    jmp STAGE2_SPACE # Enter stage 2 of bootloader, attempt to enter protected mode.
# End of _start

load_stage_2:
    pusha # Push all register values from stack
    mov $0x02, %ah # BIOS function to read from disk
    mov $STAGE2_SPACE, %bx # Read to STAGE2_SPACE, where stage 2 will be loaded
    mov $4, %al # Read 4 sectors
    mov (BOOT_DISK), %dl # Select the disk we want to read from (our boot disk)
    mov $0x00, %ch # Select cylinder 0
    mov $0x00, %dh  # Select head 0
    mov $0x02, %cl  # Select sector
    
    int $0x13 # Call BIOS interrupt to read from disk
    popa # Pop all register values from stack
    jc read_failed # BIOS function sets carry bit if disk read failed
    # So, jump to error routine if function errored
    ret

# Drive read error routine, print error message and halt.
read_failed:
    mov $read_failed_msg, %bx # Move string pointer into bx
    call realprint # print
    jmp .

.include "printb.inc"
.include "a20.inc"

msg: .asciz "Hello World!\n"
read_failed_msg: .asciz "\nDisk read failed"
BOOT_DISK: .byte 0 # The disk we booted from


.org 510  # fill every remaining byte from the start to byte 510 with 0
.word 0xaa55 # last two bytes are 0x55AA, to make 512
