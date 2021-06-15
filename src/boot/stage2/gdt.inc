gdt_nulldesc:
    .long 0
    .long 0

gdt_codedesc:
    .word 0xFFFF # First part of the code segment upper limit (Where the code seg ends)
    .word 0x0000 # The base of the code segment (Where the code seg starts)
    .byte 0x00 # Second part of the base

    # Present bit, privilege (2 bits), descriptor type, executable, direction/conforming, read/write, access bit
    .byte 0b10011010 # Access byte
    # Granularity (1 for 4kb, 0 for 1 byte), size (1 for 32 bit), 0, 0
    # Last 4 bits are second part of code seg limit
    .byte 0b11001111 # Flags
    .byte 0x00 # Last part of the base

gdt_datadesc:
    .word 0xFFFF # First part of the code segment upper limit (Where the code seg ends)
    .word 0x0000 # The base of the code segment (Where the code seg starts)
    .byte 0x00 # Second part of the base

    # Present bit, privilege (2 bits), descriptor type, executable, direction/conforming, read/write, access bit
    .byte 0b10010010 # Access byte
    # Granularity (1 for 4kb, 0 for 1 byte), size (1 for 32 bit), 0, 0
    # Last 4 bits are second part of code seg limit
    .byte 0b11001111 # Flags
    .byte 0x00 # Last part of the base
gdt_end: 

gdt_descriptor:
    .word gdt_end - gdt_nulldesc - 1 # GDT size
    .long gdt_nulldesc # GDT offset (location of the GDT)

.equ codeseg, gdt_codedesc - gdt_nulldesc
.equ dataseg, gdt_datadesc - gdt_nulldesc
    