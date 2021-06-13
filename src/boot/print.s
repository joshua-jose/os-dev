printf: # args: string pointer in bx
    mov $0xe, %ah # Set to teletype function (0xe)
    mov %bx, %si # Move the string pointer into %si
printf_looptop:
    lodsb # load si (a character) into al
    cmp $0, %al # Check if al (a character) is 0
    je printf_end # if the char is 0, end
    int $0x10 # call print
    jmp printf_looptop # return to top of loop
printf_end:
    ret # Return
    