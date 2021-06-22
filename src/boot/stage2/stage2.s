.code16
.text

# Entry point
jmp enter_protected_mode

.equ VGA_START, 0xB8000
# GDT must be defined before trying to enter protected mode
.include "gdt.inc"
enter_protected_mode:
    
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

.fill 2048-(.-.text), 1, 0  # fill every remaining byte from the start to byte 2048 with 0
#This fills 4 sectors 
