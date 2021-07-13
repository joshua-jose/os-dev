.equ CR0_TS, 0b100 # If bit is set, cause #NM exception on FPU op
.equ CR0_EM, 0b010 # If set, cause #UD on FPU op
.equ FORCE_FPU, ~(CR0_TS+CR0_EM) # Force FPU usage on FPU op
.equ SETUP_FPU_CR0, 0b110010
.equ ENABLE_OSXSAVE, (1 << 18)

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

test_avx:
    // Test for OSXSAVE bit
    mov $0x1, %eax
    cpuid
    test $(1<<26), %ecx # Look for XSAVE bit
    jz no_avx

    mov %cr0, %edx # load CR0
    or $ENABLE_OSXSAVE, %edx
    mov %edx, %cr0 # Save CR0

    test $(1<<28), %ecx // AVX bit
    jz no_avx

    # AVX support is broken on virtualbox...
    #call enable_avx 

no_avx:                     # AVX is not required
    ret

enable_avx:
    push %eax
    push %ecx
    push %edx
 
    xor %ecx, %ecx
    xgetbv      #Load XCR0 register
    or $7, %eax #Set AVX, SSE, X87 bits
    xsetbv      #Save back to XCR0
 
    pop %edx
    pop %ecx
    pop %eax
    ret


nofpu_msg: .asciz  "No FPU detected        "
no_sse_msg: .asciz "No SSE detected        "
.testword: .word 0x56FE
