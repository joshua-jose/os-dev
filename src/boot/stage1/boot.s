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
bpbBytesPerSector:      .word 512  # Size of each sector in bytes
bpbSectorsPerCluster:   .byte 1    # Number of sectors per cluster
bpbReservedSectors:     .word 2+KERNEL_SECTORS    # Reserved sectors
bpbNumberOfFATs:        .byte 1    # How many tables 
bpbRootEntries:         .word 224  # Entries in root directory
bpbTotalSectors:        .word 28800 # Number of sectors in the volume, if 0, then there are more than 65535 sectors
bpbMediaDescriptor:     .byte 0xF0
bpbSectorsPerFAT:       .word 9    # How big the FAT is in sectors
bpbSectorsPerTrack:     .word 18
bpbHeadsPerCylinder:    .word 2
bpbHiddenSectors:       .long 2+KERNEL_SECTORS    # Number of sectors until the start of the partition
bpbTotalSectorsBig:     .long 0    # If totalSectors is 0, then use this value instead for number of sectors
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
    mov $0x7c00, %sp # Move 0x7c00 (The base of our stack) into the sp (stack pointer) register. Says where the stack is.
    mov $0x7c00, %bp # Move 0x7c00 (The base of our stack) into the bp (base pointer) register. Says where the stack starts.

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
