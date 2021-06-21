enable_a20:
    call test_a20

    cmp $1, %ax # Returns 1(true) if A20 is enabled
    jne a20_bios_func # A20 is not enabled, so jump to enable routine
    ret # A20 is enabled, so return
    
a20_bios_func:
    mov     $0x2403, %ax            #--- A20-Gate Support ---
    int     $0x15
    jb      fast_a20                #INT 15h is not supported
    cmp     $0, %ah
    jnz     fast_a20                #INT 15h is not supported
    
    mov     $0x2402, %ax            #--- A20-Gate Status ---
    int     $0x15
    jb      fast_a20                #couldn't get status
    cmp     $0, %ah
    jnz     fast_a20                #couldn't get status
    
    cmp     $1, %al
    jz      test_bios_func          #A20 is already activated
    
    mov     $0x2401, %ax            #--- A20-Gate Activate ---
    int     $0x15 # Hopefully, A20 is activated

test_bios_func:
    call test_a20

    cmp $1, %ax # Returns 1(true) if A20 is enabled
    jne fast_a20 # A20 is not enabled, so jump to enable routine
    ret # A20 is enabled, so return

fast_a20:
    # Sets a bit to enable A20 (Fast A20 Gate method)
    in $0x92, %al
    test $2, %al # Check to see if Fast A20 bit is set
    jnz after_fast_a20 # Jump away if it is
    or %al, 2 # Set fast A20 bit
    and $0xFE, %al # Unset fast reset bit
    out %al, $0x92 

after_fast_a20:

    # Test A20 again
    call test_a20
    cmp $1, %ax # Returns 1(true) if A20 is enabled
    jne failed_a20 # A20 is not enabled, so fail
    
    ret # A20 is enabled, return

failed_a20:
    mov $failed_message, %bx # Move string pointer into bx
    call realprint # print
    hlt
    jmp .-1

# Function Test_a20, returns 1 if enabled, 0 if not enabled
test_a20:
    # Push values and disable interrupts
    pushf
    push %ds
    push %es
    cli
    
    xor %ax, %ax # ax = 0
    mov %ax, %es # Move 0 into es
 
    not %ax # ax = 0xFFFF
    mov %ax, %ds # Move 0xFFFF into ds
 
    mov $0x0500, %di 
    mov $0x0510, %si
 
    movb %es:(%di), %al
    push %ax
 
    movb %ds:(%si), %al
    push %ax
 
    movb $0x00, %es:(%di)
    movb $0xFF, %ds:(%si)
 
    cmpb $0xFF, %es:(%di)
 
    pop %ax
    movb %al, %ds:(%si)
 
    pop %ax
    movb %al, %es:(%di)
    
    mov $0, %ax # A20 is disabled
    je test_a20_exit
    mov $1, %ax # A20 is enabled    

test_a20_exit:
    # Pop any values and restore interrupts
    pop %es
    pop %ds
    popf
    sti
    ret

failed_message: .asciz "\nSetting A20 Failed"
