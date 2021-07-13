.code16
.text

# Entry point
jmp enter_protected_mode
.include "consts.inc" # Constants

# GDT must be defined before trying to enter protected mode
.include "gdt.inc"
.include "a20.inc"

enter_protected_mode:
    call enable_a20 # Enable the A20 line
    cli # Disable interrupts
    
    lgdt (gdt_descriptor) # Load our GDT
    # Set the protected mode bit
    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0
    # Long jump to start of protected mode
    jmp $codeseg, $start_protected_mode

# ----------------------------- 32 bits ------------------------------
.code32
.include "boot32.inc"

# ----------------------------- 64 bits ------------------------------
.code64
.include "boot64.inc"

.fill STAGE2_SIZE-(.-.text), 1, 0  # fill every remaining byte from the start to byte 1024 with 0
#This fills 2 sectors 
