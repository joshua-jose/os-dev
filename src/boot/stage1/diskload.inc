# Sectors per track: bpbSectorsPerTrack
# Number of heads: bpbHeadsPerCylinder

# Function
# Loads data from disk into specified location
# Arguments:
# al: how many sectors to read
# bx: destination address
# cl: Beginning sector
disk_load:
    pusha # Push all register values from stack

    # Turn LBA into CHS
    /*
    Temp = LBA / (Sectors per Track)
    Sector = (LBA % (Sectors per Track)) + 1
    Head = Temp % (Number of Heads)
    Cylinder = Temp / (Number of Heads)
    */
    
    mov %cl, (abs_sector)
    mov %bx, (destination)
    mov %al, (sector_count)
    mov %es, %bx
    mov %bx, (segment)
    xor %bx, %bx

    // Calculate CHS address
    // ---------------------------------------------------------------------------
    # LBA address is at cl
    xor %ax, %ax # Clear ax 
    mov %cl, %al // Move beginning sector to be divided
    dec %al # LBA starts at 0, CHS starts at 1
    mov $18, %bl # Divide by sectors per track
    div %bl #  LBA / (Sectors per Track)

    inc %ah # remainder is in ah
    mov %ah, (sector) # Sector is  (LBA % (Sectors per Track)) + 1
    
    xor %ah, %ah # ax = al, so just LBA / (Sectors per Track)

    mov $2, %bl
    div %bl # Temp / (Number of Heads)

    mov %ah, (head) # Head = Temp % (Number of Heads)
    mov %al, (cylinder) # cylinder = Temp / (Number of Heads)
    // ---------------------------------------------------------------------------
    // Load proper values back into registers
    mov (segment), %bx 
    mov %bx, %es
    mov (destination),  %bx
    mov (sector_count), %al
    mov (sector), %cl
    mov (BOOT_DISK), %dl # Select the disk we want to read from (our boot disk)
    mov (cylinder), %ch # Select cylinder
    mov (head), %dh
    
    mov $2, %ah # BIOS function to read from disk
    
    /*
    mov (cylinder), %ch # Select cylinder
    and $0xff, %ch # Only part of cylinder selector

    # CL = Sector | ((cylinder >> 2) & 0xC0)
    mov (cylinder), %dh
    shr $2, %dh
    and $0xC0, %dh
    or %dh, %cl
    mov (head), %dh  # Select head
    */
    /*
    mov $0x02, %ah # BIOS function to read from disk
    mov (BOOT_DISK), %dl # Select the disk we want to read from (our boot disk)
    mov $0x00, %ch # Select cylinder 0
    mov $0x00, %dh  # Select head 0
    */

    clc # Clear carry
    int $0x13 # Call BIOS interrupt to read from disk
    jc read_failed # BIOS function sets carry bit if disk read failed
    # So, jump to error routine if function errored
    

    popa # Pop all register values from stack
    ret

# Function
# Loads data from disk into specified location
# Arguments:
# al: how many sectors to read
# bx: destination address
# cx: LBA address
# dx: destination segment
/*
lba_load:
    pusha
    mov %al, (lbap_num_sectors)
    mov %bx, (lbap_buffer_offset)
    mov %cx, (lbap_lba_low)
    mov %dx, (lbap_buffer_segment)

    mov $lba_packet, %si
    mov $0x42, %ah # Extended read command
    mov (BOOT_DISK), %dl
    int $0x13
    jc read_failed

    popa
    ret

.align 4
lba_packet:
    lbap_size:           .byte 16 # size of packet
    lbap_zero:           .byte 0
    lbap_num_sectors:    .word 0  # How many sectors to load (no more than 127)
    lbap_buffer_offset:  .word 0 
    lbap_buffer_segment: .word 0 
    lbap_lba_low:        .double 0
    lbap_lba_high:       .double 0 # Only use 16 bits!
*/

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
    mov $1, %ah # BIOS function to get status of last disk operation
    int $0x13

    mov $read_failed_msg, %bx # Move string pointer into bx
    call realprint # print
    #popa # Debugging purposes
    mov (abs_sector), %di
    hlt
    jmp .-1

read_failed_msg: .asciz "\nDisk read failed"
    