// Check a single byte against a value
.macro elf_check_byte offset, value
    movb %es:\offset, %al
    cmp $\value, %al // 2 signifies 64 bit 
    jne invalid_elf
.endm

// Load an ELF file!
elf_load:
    // Load first sector: 512 bytes

    // Check for ELF signature
    call check_elf_signature

    elf_check_byte 4, 2     // Check for 64 bit (2)
    elf_check_byte 5, 1     // 1 signified little endian
    elf_check_byte 7, 0     // 0 signified System V ABI
    elf_check_byte 16, 2    // 2 signified executable
    elf_check_byte 18, 0x3E // 0x3E signified built for x86_64
    // If it passed all these checks, we are very very certain we can load this

    // Take a bunch of useful data from the header
    movw %es:0x20, %ax // Offset of program header
    movw %ax, (program_header_offset)

    movw %es:0x34, %ax // Size of ELF header
    movw %ax, (elf_header_size)

    movw %es:0x36, %ax // Size of a program header entry
    movw %ax, (program_header_entry_size)

    movw %es:0x38, %ax // Number of program header entries
    movw %ax, (program_header_entries)

    // Load all of program header here (it could be bigger than 512 bytes)

    // Load all program segments from the program header
    // Load all the segments from the ELF files
    mov (program_header_offset), %bx
    movw (program_header_entries), %cx // Number of segments to load
elf_load_segments:
    call elf_load_segment
    // next entry
    mov (program_header_entry_size), %ax
    add %ax, %bx
    // repeat
    loop elf_load_segments
    
    // All data *should* be in memory
    ret // We're done (for now)


// Actually load a single segment 
elf_load_segment:
    push %cx
    push %bx
    mov %es, %ax
    push %ax # Save es

    movw %es:(%bx), %ax // type of segment
    cmp $1, %ax // Is this segment loadable?
    jne elf_load_segment_leave

    xor %dx, %dx // div uses this reg

    // Where to load from
    // Get the segments offset and size
    movw %es:8(%bx), %ax // segment offset
    mov $SECTOR_SIZE, %si
    divw %si // get sector offset
    add $KERNEL_SECTOR_START, %ax // The ELF file doesn't start at the start of the drive!
    mov %ax, %cx // Sector select for disk load

    xor %dx, %dx // div uses this reg

    // How much to load
    movw %es:32(%bx), %ax // Size of this segment
    divw %si // get that in number of sectors
    mov %ax, %dx // give value to disk load function
    
    // Where to load to
    movw %es:8(%bx), %ax // segment offset (we are gonna load to this place in memory)
    shr $4 , %ax  // Turn from memory offset to segment offset
    mov %es, %bx
    add %ax, %bx
    mov %bx, %es // change memory segment so that it loads to correct place

    xor %bx, %bx // Clear bx as its used as an offset by disk_load!
    call disk_load
elf_load_segment_leave:
    pop %ax
    mov %ax, %es # Restore es
    pop %bx
    pop %cx
    ret


check_elf_signature:
    // Signature is 0x7F,E,L,F
    mov $0, %bx // Move start of file into %ax
    // Repeat for each value
.irp sig_byte ,0x7F,'E,'L,'F
    movb %es:(%bx), %al // Move byte into bx
    cmp $\sig_byte, %al // Compare value to signature
    jne no_elf_signature
    inc %bx // Next byte
.endr
    ret

// error messages
invalid_elf:
    mov $invalid_elf_msg, %bx
    call realprint
    hlt
    jmp .-1

no_elf_signature:
    mov $no_elf_signature_msg, %bx # Move string pointer into bx
    call realprint # print
    hlt
    jmp .-1

// Info we need to load an elf file
elf_loading_info:
    program_header_offset:      .word 0
    elf_header_size:            .word 0
    program_header_entry_size:  .word 0
    program_header_entries:     .word 0


no_elf_signature_msg: .asciz "\nCould not find a valid ELF file to load"
invalid_elf_msg: .asciz "\nELF file invalid"
