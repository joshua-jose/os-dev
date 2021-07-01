.include "consts.inc"

# Move the kernel to higher memory
move_kernel:
    mov $LOAD_AREA, %esi # Source (index) is the load area 
    mov $KERNEL_SPACE, %edi # Destination (index) is kernel space
    mov $(KERNEL_SECTORS*512), %ecx # Loop 512*sectors times, so however many bytes

    rep movsb (%esi), (%edi) # Move all that data into higher memory
    ret
