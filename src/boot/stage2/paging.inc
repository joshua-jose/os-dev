.equ page_table_entry, 0x1000

# Identity page the first 2MB of memory
setup_identity_paging:
    mov $page_table_entry, %edi
    mov %edi, %cr3 # Tell MMU that page table starts at $page_table_entry

    xor %eax, %eax       # eax = 0
    mov $4096, %ecx      # Loop 4096 times
    rep stosl          # Clear page table
    mov $page_table_entry, %edi # Move start of PML4T back into edi

    movl $0x2003, (%edi) # Next entry (PDPT) at 0x2000, set Read/Write and Present bit (allow read/write and say page is present)
    add $0x1000, %edi # Point to next entry (PDPT)

    movl $0x3003, (%edi) # Next entry (PDT) at 0x2000, set Read/Write and Present bit (allow read/write and say page is present)
    add $0x1000, %edi # Point to next entry (PDT)

    movl $0x4003, (%edi) # Next entry (PT) at 0x2000, set Read/Write and Present bit (allow read/write and say page is present)
    add $0x1000, %edi # Point to next entry (PT)

    mov $0x00000003, %ebx # Move flag bits into ebx (R/W enable, present enabled)
    mov $512, %ecx # Loop 512 times to add 512 entries

    add_entry:
        movl %ebx, (%edi) # Move flags into entry
        add $0x1000, %ebx # Change page table entry to next section of memory
        add $8, %edi # Move to next entry
        loop add_entry

    # All page table entries added, 2MB identity paged

    # Enable PAE
    mov %cr4, %eax # Move value of CR4 into EAX
    or $(1<<5), %eax  # OR eax with 1<<5 (enable 5th bit)
    mov %eax, %cr4 # Move back 
    ret
