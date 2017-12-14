#include <config.h>
#include <asm/system.h>
	
	/* form a first-level section entry */
.macro FL_SECTION_ENTRY base,prot
	.word (\base << 20) | (\prot ) 
.endm

.section .mmudata, "a"
	.align 14
/* the following alignment creates the mmu table at address 0x4000. */
	.globl mmu_table

//EXPORT mmutable

mmu_table:

#if 0
        .set __base, 0
        .rept 0x1000 -0x0
        FL_SECTION_ENTRY __base,(SEC_PROT_RW_RW | SEC_SO_MEM)
        .set __base, __base + 1
        .endr
#endif


#if 1
        .set __base, 0
		/*  ddr */
		.rept 0x3c0 - 0x0
		FL_SECTION_ENTRY __base,(SEC_PROT_RW_RW | SEC_SO_MEM)
		.set __base, __base + 1
		.endr

		.set __base,0x3c0
		/** SRAM memory **/
		.rept 0x3c1 - 0x3c0
		FL_SECTION_ENTRY __base,(SEC_PROT_RW_RW | SEC_WB)
		.set __base, __base + 1
		.endr


		/* N/A */
		.rept 0x400 -0x3c1
		.word 0x00000000
		.endr


		.set __base,0x400
		/** DDR memory **/
		.rept 0x800 - 0x400
		FL_SECTION_ENTRY __base,(SEC_PROT_RW_RW | SEC_WB)
		.set __base, __base + 1
		.endr


		.set __base,0x800
		/** DDR memory **/
		.rept 0xa00 - 0x800
		FL_SECTION_ENTRY __base,(SEC_PROT_RW_RW | SEC_WB)
		.set __base, __base + 1
		.endr


		.set __base,0xa00		
		/** N/A memory **/
		.rept 0x1000 - 0xa00
		FL_SECTION_ENTRY __base,(SEC_PROT_RW_RW | SEC_SO_MEM)
		.set __base, __base + 1
		.endr


#endif
