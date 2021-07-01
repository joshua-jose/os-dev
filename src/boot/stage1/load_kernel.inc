load_kernel:

    mov $LOAD_AREA, %bx # Load kernel to lower memory
    mov $KERNEL_SECTORS, %al # Load 1 sector
    mov $KERNEL_SECTOR_START, %cl  # Select sector
    
    call disk_load # Load data from disk using above arguments
    ret

