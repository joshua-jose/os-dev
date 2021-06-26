# Function
# Loads data from disk into specified location
# Arguments:
# bx: destination address
# al: how many sectors to read
# cl: Beginning sector
disk_load:
    pusha # Push all register values from stack
    mov $0x02, %ah # BIOS function to read from disk
    mov (BOOT_DISK), %dl # Select the disk we want to read from (our boot disk)
    mov $0x00, %ch # Select cylinder 0
    mov $0x00, %dh  # Select head 0
    
    int $0x13 # Call BIOS interrupt to read from disk
    popa # Pop all register values from stack
    jc read_failed # BIOS function sets carry bit if disk read failed
    # So, jump to error routine if function errored
    ret

# Drive read error routine, print error message and halt.
read_failed:
    mov $read_failed_msg, %bx # Move string pointer into bx
    call realprint # print
    hlt
    jmp .-1
    