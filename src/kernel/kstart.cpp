extern "C" void _start(){
    // Load memory manager
    // Load file system driver
    // Load k modules into memory and link
    int i = 0;
    volatile int* vga_start = (int*)(0xB8000);
    for (int i=0; i<100;i++){
        *(vga_start+i) = 0x1f201f20;
    }
    
    /*
    asm(R"(mov $0xB8000, %ecx # Move start of memory into ecx
        print_looptop:
            mov (%ebx), %al # Load a character into ax
            cmp $0, %al # Check if ax (a character) is 0
            je print_end # if the char is 0, end

            movb %al, (%ecx) # Move the character into video memory
            inc %ebx # Increment string pointer
            add $2, %ecx # Increment video memory pointer by 2

            jmp print_looptop # return to top of loop
    print_end:)");
    */
}