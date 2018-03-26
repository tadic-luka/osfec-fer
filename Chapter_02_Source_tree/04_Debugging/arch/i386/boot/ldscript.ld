/*! linker script for memory layout */

/* Its parsed as C before used in linking! */

ENTRY(arch_startup)

SECTIONS {
	.boot_code LOAD_ADDR :
	{

		/* instructions */
		*?/boot/startup.asm.o ( * )
		*?/boot/copy.o ( * )
	}
	kernel_code_LMA = LOAD_ADDR + SIZEOF(.boot_code);
	kernel_code_VMA = RAM_START;
	.kernel_code RAM_START: AT(kernel_code_LMA)
	{
		kernel_code_addr = .;

		* ( .text *)
	}
	.rodata :
	{
		* ( .rodata *)
	}
	.data :
	{
		* ( .data *)
	}
	.bss :
	{
		*( .bss* COMMON* )

		. = ALIGN (4096);
	}

	system_stack = STACK_ADDR;

	/*
	 * what with other sections generated with gcc (various versions)?
	 * if optimizing for size discard them with: /DISCARD/ : { *(*) }
	 * if debugging with qemu/gdb they must be included
	 * - they can be included implicitly, without declaring them here
	 * - but they will be put behind "kernel_end_addr" so its best to use
	 *   multiboot loader (QEMU is that)
	 */

	/DISCARD/ : { *( .eh_frame *) }
#ifndef DEBUG
	/DISCARD/ : { *(*) }
#endif
	kernel_end_addr = .;
}
