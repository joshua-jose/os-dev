# Load the kernel, one sector at a time
# Address the load area using segments so we can address more memory

load_kernel:
    pusha
    /*
    // Load using segments, put segment into es
    mov $(LOAD_AREA >> 4), %bx # Load into 0xA00:xxx
    mov %bx, %es
    

    mov $KERNEL_SECTOR_START, %cl  # Select sector
    mov $1, %al # Load one sector at a time
    mov $KERNEL_SECTORS, %dx # how many times
load_kernel_looptop:
    call disk_load
    
    mov %es, %bx # Increase segment count
    add $(SECTOR_SIZE >> 4), %bx
    mov %bx, %es
    xor %bx, %bx # Load %es and clear %bx
    
    inc %cl # next sector
    dec %dx # reduce counter
    cmp $0, %dx # Check if counter is ended
    
    jne load_kernel_looptop
    */
    
    mov $(LOAD_AREA >> 4), %bx # Read from 0xA00:xxx
    mov %bx, %es
    xor %bx, %bx # Load %es and clear %bx
    mov $1, %dx # only load 1 sector
    mov $KERNEL_SECTOR_START, %cx  # Select sector
    call disk_load

    # Reset ES as disk_load does not preserve it
    mov $(LOAD_AREA >> 4), %bx # Read from 0xA00:xxx
    mov %bx, %es
    xor %bx, %bx # Load %es and clear %bx

    call elf_load

load_kernel_end:
    xor %ax, %ax
    mov %ax, %es # clear segment register
    popa
    ret

kernel_segment: .word 0

// Load all kernel using one CHS
    /*
    mov $LOAD_AREA, %bx # Load kernel to lower memory
    mov $KERNEL_SECTORS, %al # Load 1 sector
    mov $KERNEL_SECTOR_START, %cl  # Select sector
    
    call disk_load # Load data from disk using above arguments
    */
    /*
    mov $32, %al # Load n sector
    mov $3, %cx  # Select sector
    mov $0, %bx # Load kernel to lower memory
    mov $0xA00, %dx
    call lba_load # Load data from disk using above arguments
    */

// Load in chunks
/*
    
    mov $KERNEL_SECTORS, %ax # Value to be divided
    mov $CHUNK_SIZE, %cl     # To divide by
    div %cl                  # divide sectors by 32
    
    mov %ax, %dx             # keep result in dx

    mov $LOAD_AREA, %bx # Load kernel to lower memory
    mov $CHUNK_SIZE, %al # Load 32 sector
    mov $KERNEL_SECTOR_START, %cl  # Select sector
    cmp $0, %dl # is quotient 0? (less than 32 sectors)
    je load_last_chunk

load_loop:
    call disk_load # Load data from disk using above arguments
    add $CHUNK_SIZE, %cl # Increase sector start by 32
    add $(CHUNK_SIZE*SECTOR_SIZE), %bx # Move load area forward

    dec %dl # Decrease chunks to load by 1
    cmp $0, %dl # is quotient 0? (less than 32 sectors)
    jne load_loop

load_last_chunk:
    
    cmp $0, %dh
    je load_kernel_end # If there are no more sectors to read...
    mov %dh, %al # Load remaining sector    
    call disk_load # Load data from disk using above arguments
    */

