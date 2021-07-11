.equ CR0_TS, 0b100 # If bit is set, cause #NM exception on FPU op
.equ CR0_EM, 0b010 # If set, cause #UD on FPU op
.equ FORCE_FPU, ~(CR0_TS+CR0_EM) # Force FPU usage on FPU op
.equ SETUP_FPU_CR0, 0b110010

test_fpu:
    # Test for FPU support
    mov %cr0, %edx # load CR0
    and $FORCE_FPU, %edx # unset TS and EM
    mov %edx, %cr0 # Save CR0

    fninit # Init FPU with defaults
    fnstsw (.testword) # Store status into testword

    cmpw $0, (.testword) # see if the FPU has written its state
    jne nofpu

    # Set up FPU
    mov %cr0, %edx # load CR0
    or $SETUP_FPU_CR0, %edx
    mov %edx, %cr0 # Save CR0

    ret

nofpu:
    mov $nofpu_msg, %ebx # Move string pointer into ebx
    call print32
    hlt

test_sse:
    mov $0x1, %eax
    cpuid
    test $(1<<25), %edx
    jz no_sse

    mov %cr0, %eax
    and $0xFFFB, %ax 		#clear coprocessor emulation CR0.EM
    or  $0x2, %ax			#set coprocessor monitoring  CR0.MP
    mov %eax, %cr0 
    
    mov %cr4, %eax 
    or $(3 << 9), %ax 		#set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov %eax, %cr4 
    ret

no_sse:
    mov $no_sse_msg, %ebx # Move string pointer into ebx
    call print32
    hlt

nofpu_msg: .asciz  "No FPU detected        "
no_sse_msg: .asciz "No SSE detected        "
.testword: .word 0x56FE
