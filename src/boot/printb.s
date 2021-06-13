realprint: # args: string pointer in bx
    mov $0xe, %ah # Set to teletype function (0xe)
    mov %bx, %si # Move the string pointer into %si
realprint_looptop:
    lodsb # load si (a character) into al
    cmp $0, %al # Check if al (a character) is 0
    je realprint_end # if the char is 0, end
    int $0x10 # call print
    jmp realprint_looptop # return to top of loop
realprint_end:
    ret # Return
    