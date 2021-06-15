enable_a20:
    call test_a20
    cmp $1, %eax # Returns 1(true) if A20 is enabled
    jne continue_enable_a20 # A20 is not enabled, so jump to enable routine

    ret # A20 is enabled, so return
continue_enable_a20:
    # Sets a bit to enable A20 (Fast A20 Gate method)
    in $0x92, %al
    or %al, 2
    out %al, $0x92 

    ret

# Function Test_a20, returns 1 if enabled, 0 if not enabled
test_a20:
    mov $0x112345, %edi  #odd megabyte address.
    mov $0x012345, %esi  #even megabyte address.
    mov %esi, (%esi)     #making sure that both addresses contain different values.
    mov %edi, (%edi)     #(if A20 line is cleared the two pointers would point to the address 0x012345 that would contain 0x112345 (edi)) 
    cmpsd             #compare addresses to see if they're equivalent.

    # If they are equal, odd megabyte address wrapped around to even megabyte address, so A20 is not enabled
    jne not_enabled # A20 is not enabled

    mov $1, %eax # True
    ret # A20 is enabled, so return

    not_enabled:
    mov $0, %eax # False
    ret
