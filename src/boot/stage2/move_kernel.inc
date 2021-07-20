.include "consts.inc"

# Move the kernel to higher memory
/* TODO:
Parse ELF header to get 
    * offset of segments 
    * desired virtual address
    * entry point
    * file_size and memory_size
        * when memory_size and file_size differ, then the memory after file_size up to memory_size is to be filled with 0s
    
Find section table and set up each section correctly (mainly BSS) 
*/
move_kernel:
    mov $(LOAD_AREA+0x1000), %esi # Source (index) is the load area 
    mov $KERNEL_SPACE, %edi # Destination (index) is kernel space
    mov $(KERNEL_SECTORS*512), %ecx # Loop 512*sectors times, so however many bytes

    rep movsb (%esi), (%edi) # Move all that data into higher memory
    ret
