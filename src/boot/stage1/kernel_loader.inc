load_kernel:

    mov $LOAD_AREA, %bx # Load kernel to lower memory
    mov $KERNEL_SECTORS, %al # Load 1 sector
    mov $KERNEL_SECTOR_START, %cl  # Select sector
    
    call disk_load # Load data from disk using above arguments

    mov $LOAD_AREA, %esi # Source (index) is the load area 
    mov $KERNEL_SPACE, %edi # Destination (index) is kernel space
    mov $(KERNEL_SECTORS*512), %ecx # Loop 512*sectors times, so however many bytes

    rep movsb (%esi), (%edi) # Move all that data into higher memory
    ret

