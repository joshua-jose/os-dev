# The very first part of the bootloader
.code16
.global _start
.text

.include "consts.inc" # Constants

# ------------------------------------------------------
BPB: # BIOS Parameter Block (Defines data about a drive)
jmp _start
nop
bpbOEMIdentifier:       .quad 0x0
bpbBytesPerSector:      .word 512
bpbSectorsPerCluster:   .byte 1
bpbReservedSectors:     .word 4
bpbNumberOfFATs:        .byte 2
bpbRootEntries:         .word 224
bpbTotalSectors:        .word 2880
bpbMediaDescriptor:     .byte 0xF0
bpbSectorsPerFAT:       .word 9
bpbSectorsPerTrack:     .word 18
bpbHeadsPerCylinder:    .word 2
bpbHiddenSectors:       .long 4
bpbTotalSectorsBig:     .long 0
ebpbDriveNumber:        .byte 0
ebpbUnused:             .byte 0
ebpbExtBootSignature:   .byte 0x29
ebpbSerialNumber:       .long 0xa0a1a2a3
ebpbVolumeLabel:        .ascii "AOS Floppy "
ebpbFileSystem:         .ascii "FAT12   "
# ------------------------------------------------------

_start:
    jmp $0,$_start_continue # Make sure we're at 0:7c00 rather than 7c00:0
_start_continue:
    xor %ax, %ax # AX = 0
    mov %ax, %ds # Data and extended segment is 0
    mov %ax, %es

    # BIOS puts the number of the boot disk into %dl at the start
    mov %dl, (BOOT_DISK) # Move the disk number into the BOOT_DISK VARIABLE

    mov %ax, %ss # Stack segment is 0
    mov $0x7c00, %bp # Move 0x7c00 (The base of our stack) into the bp (base pointer) register
    mov %bp, %sp # Move the bp value (base pointer) into the sp (stack pointer) register


    mov $msg, %bx # Move string pointer into bx
    call realprint # print

    # Tell BIOS that we intend to end up in 64 bit long mode
    mov $0xEC00, %ax # Magic number for mode telling 
    mov $2, %bl # 2 is for long mode
    int $0x15 # Tell the BIOS

    call load_stage_2 # Load the second stage of the bootloader
    call load_kernel # Load the kernel into memory
    call enable_a20 # Enable the A20 line
    
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
.include "load_kernel.inc"

msg: .asciz "Hello World!\r\n"
read_failed_msg: .asciz "\nDisk read failed"
BOOT_DISK: .byte 0 # The disk we booted from

/*
.org 0x1B8

# MBR
.org 0x1BE
.byte 0x80 # Boot indicator (this is bootable)
# Partition start
.byte 0
.byte 0b00000101
.byte 0
.byte 0x1 # FAT12 bootable, first 32MB
# Partition end
.byte 0 # Head
.byte 0b00111111 # sector and cylinder
.byte 0 # Cylinder
.long 5
.long 0xFFFF
*/
.org 510  # fill every remaining byte from the start to byte 510 with 0
.word 0xaa55 # last two bytes are 0x55AA, to make 512
