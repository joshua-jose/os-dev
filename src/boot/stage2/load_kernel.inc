# Load the kernel, one sector at a time
# Address the load area using segments so we can address more memory

load_kernel:
    pusha
    
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

    call elf_diskload

load_kernel_end:
    xor %ax, %ax
    mov %ax, %es # clear segment register
    popa
    ret

kernel_segment: .word 0
