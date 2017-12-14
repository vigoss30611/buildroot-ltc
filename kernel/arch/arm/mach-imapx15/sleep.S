#include <linux/linkage.h>
#include <asm/assembler.h>
#include <asm/memory.h>

#include <mach/sram.h>

	.text

ENTRY(imapx800_cpu_save)

	stmfd	sp!, { r3 - r12, lr }
	ldr	r3, =resume_with_mmu
	bl	cpu_suspend

	ldr	r0, =pm_cpu_sleep
	ldr	r0, [ r0 ]
	mov	pc, r0

resume_with_mmu:
	ldmfd	sp!, { r3 - r12, pc }

	.ltorg

	.word	0x2bedf00d

ENTRY(imapx15_cpu_resume)
	bl  v7_invalidate_l1
	b	cpu_resume

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global dfs_enter
dfs_enter:
        .func

		@backup stack
		stmfd	sp!, {r1}
        ldr     r1, =dfs_stack
        str		sp, [r1]
        dsb
		@set new stack in sram
        ldr     r1, =sram_stack
        add     r1, r1, #IMAP_SRAM_STACK_SIZE
        sub     r1, r1, #4
        mov     sp, r1

		bx		lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global dfs_exit
dfs_exit:
        .func

		@restore stack
        ldr     r1, =dfs_stack
        ldr		r1, [r1]
        mov     sp, r1

		nop
        pop		{r1}

		bx		lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
