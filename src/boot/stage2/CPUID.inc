get_CPUID_support:
    pushfl # Actually save EFLAGS

    pushfl # Store EFLAGS

    xorl $0x00200000, (%esp)             # Invert ID bit (xor with newest value in stack which is flags)
    popfl                                #Load stored EFLAGS (with ID bit inverted)
    pushfl                               #Store EFLAGS again (ID bit may or may not be inverted)
    pop %eax                             #eax = modified EFLAGS (ID bit may or may not be inverted)
    xor (%esp), %eax                     #eax = whichever bits were changed
    popfl                                #Restore original EFLAGS
    and $0x00200000, %eax                #eax = zero if ID bit can't be changed, else non-zero

    jz no_CPUID # If eax is 0, we cant change the ID bit, so no CPUID support
    ret # Otherwise, return as normal

detect_long_mode_support:
    call get_CPUID_support
    
    mov $0x80000001, %eax # Get Long Mode support function
    cpuid # call CPUID
    test $(1<<29), %edx # AND value against long mode bit
    jz no_long_mode # If no match, no long mode support
    ret

no_long_mode:
    mov $long_mode_error_msg, %ebx # Move string pointer into ebx
    call print32
    hlt # This is a 64 bit OS.

no_CPUID:
    mov $CPUID_error_msg, %ebx # Move string pointer into ebx
    call print32
    hlt # What.

long_mode_error_msg: .asciz "!!- No Long Mode support - !!"
CPUID_error_msg:     .asciz "!!-  No CPUID support    - !!"
