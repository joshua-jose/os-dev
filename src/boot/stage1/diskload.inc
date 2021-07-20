# Function
# Loads data from disk into specified location
# Arguments:
# bx: destination offset
# es: destination segment
# cx: Beginning sector (as LBA)
.global disk_load // let load_kernel link against this
disk_load_sector:
    pusha # Push all register values from stack

    # Turn LBA into CHS
    /*
    Temp = LBA / (Sectors per Track)
    Sector = (LBA % (Sectors per Track)) + 1
    Head = Temp % (Number of Heads)
    Cylinder = Temp / (Number of Heads)
    */
    
    mov %cx, (abs_sector)
    mov %bx, (destination)
    mov %es, %bx
    mov %bx, (segment)
    xor %bx, %bx // clear bx

    // Calculate CHS address
    // ---------------------------------------------------------------------------
    # LBA address is at cl
    //xor %ax, %ax # Clear ax 
    xor %dx, %dx # clear dx
    mov %cx, %ax // Move beginning sector to be divided
    mov (bpbSectorsPerTrack), %bx # Divide by sectors per track
    div %bx # LBA / (Sectors per Track)

    inc %dx # remainder, truncate later by only taking dl
    mov %dl, (sector) # Sector is  (LBA % (Sectors per Track)) + 1

    xor %dx, %dx # ax = quotient, so ax = Temp =  LBA / (Sectors per Track)

    mov (bpbHeadsPerCylinder), %bx # divide by heads per cylinder
    div %bx # Temp / (Number of Heads)

    mov %dl, (head) # Head = Temp % (Number of Heads)
    mov %al, (cylinder) # cylinder = Temp / (Number of Heads)
    // ---------------------------------------------------------------------------
    // Load proper values back into registers
    mov (segment), %bx 
    mov %bx, %es
    mov (destination),  %bx
    mov $1, %al
    mov (sector), %cl
    
    mov (BOOT_DISK), %dl # Select the disk we want to read from (our boot disk)
    #mov (cylinder), %ch # Select cylinder
    #mov (head), %dh
    
    mov $2, %ah # BIOS function to read from disk
    
    mov (cylinder), %ch # Select cylinder
    and $0xff, %ch # Only part of cylinder selector

    # CL = Sector | ((cylinder >> 2) & 0xC0)
    mov (cylinder), %dh
    shr $2, %dh
    and $0xC0, %dh
    or %dh, %cl
    mov (head), %dh  # Select head

    clc # Clear carry
    int $0x13 # Call BIOS interrupt to read from disk
    jc read_failed # BIOS function sets carry bit if disk read failed
    # So, jump to error routine if function errored
    
    popa # Pop all register values from stack
    ret

# es: destination segment
# bx: destination offset
# cx: beginning sector (as LBA + 1)
# dx: number of sectors to load

disk_load:
    call disk_load_sector
    
    mov %es, %bx # Increase segment count
    add $(SECTOR_SIZE >> 4), %bx
    mov %bx, %es
    xor %bx, %bx # Load %es and clear %bx
    
    inc %cx # next sector
    dec %dx # reduce counter
    cmp $0, %dx # Check if counter is ended
    
    jne disk_load
    ret

disk_load_small:
    call disk_load_sector
    
    add $SECTOR_SIZE, %bx
    
    inc %cx # next sector
    dec %dx # reduce counter
    cmp $0, %dx # Check if counter is ended
    
    jne disk_load_small
    ret

load_config:
    segment:      .word 0
    destination:  .word 0
    sector_count: .byte 0
    temp:         .word 0
    cylinder:     .byte 0
    head:         .byte 0
    sector:       .byte 0
    abs_sector:   .word 0


# Drive read error routine, print error message and halt.
read_failed:
    mov (abs_sector), %di
    mov $1, %ah # BIOS function to get status of last disk operation
    int $0x13

    mov $read_failed_msg, %bx # Move string pointer into bx
    call realprint # print
    #popa # Debugging purposes
    hlt
    jmp .-1

read_failed_msg: .asciz "\nDisk read failed"
    