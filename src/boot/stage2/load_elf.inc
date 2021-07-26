.equ ELF_64_BIT, 2
.equ ELF_LITTLE_ENDIAN, 1
.equ ELF_SYSTEM_V, 0
.equ ELF_EXEC, 2
.equ ELF_X86_64, 0x3E

// Check a single byte against a value
.macro elf_check_byte offset, value
    movb %es:\offset, %al
    cmp $\value, %al // 2 signifies 64 bit 
    jne invalid_elf
.endm

// Load an ELF file!
elf_diskload:
    // Assume first sector of file is loaded

    // Check for ELF signature
    call check_elf_signature

    elf_check_byte 4, ELF_64_BIT        // Check for 64 bit (2)
    elf_check_byte 5, ELF_LITTLE_ENDIAN // 1 signified little endian
    elf_check_byte 7, ELF_SYSTEM_V      // 0 signified System V ABI
    elf_check_byte 16, ELF_EXEC         // 2 signified executable
    elf_check_byte 18, ELF_X86_64       // 0x3E signified built for x86_64
    // If it passed all these checks, we are very very certain we can load this

    // Take a bunch of useful data from the header
    movw %es:0x20, %ax // Offset of program header
    movw %ax, (program_header_offset)
    movw %es:0x28, %ax // Offset of section header
    movw %ax, (section_header_offset_low)
    movw %es:0x2a, %ax // Offset of section header
    movw %ax, (section_header_offset_high)

    movw %es:0x34, %ax // Size of ELF header
    movw %ax, (elf_header_size)
    movw %es:0x36, %ax // Size of a program header entry
    movw %ax, (program_header_entry_size)
    movw %es:0x38, %ax // Number of program header entries
    movw %ax, (program_header_entries)
    movw %es:0x3a, %ax // Size of a section header entry
    movw %ax, (section_header_entry_size)
    movw %es:0x3c, %ax // Number of section header entries
    movw %ax, (section_header_entries)

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

    //Load section headers (which live at the end of the file)
    //call elf_load_section_headers

    // All data *should* be in memory
    ret // We're done (for now)

// Quite hard since we're gonna need to load from far away sections
// We move the section header into the start of the program, where there are 0s anyways
elf_load_section_headers:
    mov %es, %ax
    push %ax # Save es

    // Where to load from
    // Load values in preperation for division
    mov (section_header_offset_low), %ax
    mov (section_header_offset_high), %dx
    mov $SECTOR_SIZE, %bx
    div %bx // ax contains the starting sector
    mov %ax, %cx // Where the disk load function wants it

    // Where to put it
    mov %dx, %di // How far the section header is offset from a page
    xor %dx, %dx
    movw (program_header_entries), %ax
    movw (program_header_entry_size), %bx
    mul %bx
    // Size of program header
    movw (elf_header_size), %bx // End of ELF header
    add %bx, %ax // Add program header size to ELF header size
    // ax now contains the end of all the ELF data at the start of the file

    add %di, %ax # Section header is not 512 aligned! Where the header will *actually* be
    movw %ax, %es:0x28 // Put this offset as new section header offset
    sub %di, %ax // Now we load the sector

    shr $4, %ax // Memory offset to segment offset
    mov %es, %bx
    add %ax, %bx
    mov %bx, %es // change memory segment so that it loads to correct place

    // How much to load
    xor %dx, %dx // clear dx
    mov (section_header_entry_size), %ax
    mov (section_header_entries), %bx
    mul %bx // Get size of section header
    xor %dx, %dx // clear dx
    mov $SECTOR_SIZE, %bx
    div %bx // ax: number of sectors to load
    add $1, %ax // In case there is a remainder
    mov %ax, %dx // where disk load function wants it
    
    // cx already contains starting sector offset from start of elf
    // dx contains number of sectors already
    xor %bx, %bx
    add $KERNEL_SECTOR_START, %cx // offset from start of disk

    call disk_load

    pop %ax
    mov %ax, %es # Restore es
    ret


// Actually load a single segment 
elf_load_segment:
    push %cx
    push %bx
    mov %es, %ax
    push %ax # Save es

    /*
    // Get the end of the loadable section
    movw %es:8(%bx), %ax // Segment offset
    movw (last_segment_offset), %dx
    cmp %dx, %ax
    jle continue_elf_segment_load # If current segment offset <= biggest one, then just ignore it
    
    mov %ax, (last_segment_offset)
    movw %es:32(%bx), %dx // Segment size
    add %dx, %ax
    mov %ax, (last_segment_end) // Add offset and size to get end of last segment
    */

continue_elf_segment_load:
    movw %es:(%bx), %ax // type of segment
    cmp $1, %ax // Is this segment loadable?
    jne elf_load_segment_leave

    // Load segment into memory
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
    add $1, %ax // In case theres a remainder
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
    section_header_offset_low:  .word 0
    section_header_offset_high: .word 0
    elf_header_size:            .word 0
    program_header_entry_size:  .word 0
    program_header_entries:     .word 0
    section_header_entry_size:  .word 0
    section_header_entries:     .word 0
    last_segment_end:           .word 0
    last_segment_offset:        .word 0


no_elf_signature_msg: .asciz "\nCould not find a valid ELF file to load"
invalid_elf_msg: .asciz "\nELF file invalid"
