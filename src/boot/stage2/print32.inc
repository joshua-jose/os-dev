# Prints to the screen at start of line
# Parameters:
# String (%ebx)
print32:
    mov $VGA_START, %ecx # Move start of memory into ecx
print_looptop:
    mov (%ebx), %al # Load a character into ax 
    cmp $0, %al # Check if ax (a character) is 0
    je print_end # if the char is 0, end

    movb %al, (%ecx) # Move the character into video memory
    inc %ebx # Increment string pointer
    add $2, %ecx # Increment video memory pointer by 2

    jmp print_looptop # return to top of loop

print_end:
    ret
    