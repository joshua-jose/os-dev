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
    call load_elf
    ret

    # Kernel ELF Header is at $LOAD_AREA
    # Section headers are after the ELF header and program headers

    #mov $(LOAD_AREA+0x1000), %esi # Source (index) is the load area 
    #mov $KERNEL_SPACE, %edi # Destination (index) is kernel space
    #mov $(KERNEL_SECTORS*512), %ecx # Loop 512*sectors times, so however many bytes

    #rep movsb (%esi), (%edi) # Move all that data into higher memory
    #ret

load_elf:
    

    xor %eax, %eax
    xor %ebx, %ebx
    xor %ecx, %ecx
    mov (program_header_offset), %ax 
    mov (program_header_entry_size), %bx
    mov (program_header_entries), %cx

    xor %esi, %esi # offset of current header from start of headers

    lea LOAD_AREA(%eax), %eax
load_elf_sections: 
    call load_elf_section
    add %ebx, %esi
    loop load_elf_sections

    ret  

load_elf_section:
    pusha
    cld # forward direction
    mov 16(%eax, %esi), %edi # Where in v mem
    mov 32(%eax, %esi), %ecx # size in file
    mov 40(%eax, %esi), %edx # size in memory
    mov  8(%eax, %esi), %esi # Offset in the file

    sub %ecx, %edx # difference in file and memory space

    # Move data from the file to final place in memory
    add $LOAD_AREA, %esi # offset in file to actual place in memory
    rep movsb (%esi), (%edi) # Move all that data into higher memory

    # Fill remaining space with 0s
    mov %edx, %ecx # move that difference into the loop counter
    rep stosl (%edi) # edi should contain where movsb left off

    popa 
    ret
