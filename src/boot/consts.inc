# This file contains all bootloader wide constants

# BIOS loads boot sector to 0x7C00 to 0x7DFF, so load right after
.equ STAGE2_SPACE, 0x7e00
.equ LOAD_AREA, 0xA000 # Where the BIOS can load data to, available up to 0x7FFFF, but we will use up to 0x70000 (0x66000) of space, or 816 boot sectors (408kb)
.equ KERNEL_SPACE, 0x100000 # Where the kernel will live (At 1MB boundary)

.equ STAGE2_SIZE, 1024 # Size of stage 2, in bytes
.equ STAGE2_SECTORS, 2 # Number of sectors in stage 2, should be (STAGE2_SIZE)/512

.equ KERNEL_SECTOR_START, STAGE2_SECTORS + 2 # Where the kernel sectors will start (after stage 2 sectors)
.equ KERNEL_SECTORS, 48

.equ VGA_START, 0xB8000
