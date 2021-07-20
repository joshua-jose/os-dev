/*TARGET(binary)*/
/*OUTPUT_FORMAT(elf64-x86-64)*/
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 1M;
 
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(._start);
		*(.text*);
		_etext = .;
	}

    /* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		_sdata = .;
		*(.data*)
		_edata = .;
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata*)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		_sbss = .;
		*(COMMON)
		*(.bss*)
		_ebss = .;
	}

    . = ALIGN(64);
	_end =.;
 
}
