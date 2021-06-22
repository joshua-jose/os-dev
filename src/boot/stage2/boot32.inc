.include "print32.inc"
.include "CPUID.inc"
.include "paging.inc"

# Enter 32 bit mode :)
start_protected_mode:
    
    # Load dataseg
    mov $dataseg, %ax
    # Point segment registers to new data seg
    mov %ax, %ds
    mov %ax, %ss
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    # Clear the screen
    mov $2000, %ecx # Loop 80*25 times (size of video memory)
    mov $VGA_START, %ebx # Start of video memory
    clear:
        movb $' , (%ebx, %ecx,2) # Move a space into (start + offset * 2)
    loop clear # Loop 2000 times

    # Test print
    mov $msg2, %ebx # Move string pointer into ebx
    call print32
    
    call detect_long_mode_support
    call enter_long_mode

enter_long_mode:
    # Set up simple paging (required!)
    call setup_identity_paging
    
    # Flip the long mode bit
    mov $0xC0000080, %ecx #EFER MSR (Model specific register)
    rdmsr # Read from MSR into eax
    or $(1<<8), %eax # Flip long mode bit to enable (8th bit)
    wrmsr # Write from eax to MSR
    
    mov %cr0, %eax # Move CR0 into eax
    or $(1<< 31), %eax # Set PG (enable paging)
    mov %eax, %cr0 # Move modified data back into cr0
    
    call gdt_update_64 # Update GDT to a 64 bit GDT

    # Processor is now in 64 bit mode
    jmp $codeseg, $start_long_mode # Far jump to long mode start

msg2: .asciz "Hello from 32 bits!"
