
/* Dynamic DDR frequency */


  
.globl __ddf
__ddf:
	push {r4 - r10, lr}

	mov r5, r2		@ r5: fr100n
	mov r6, r0		@ r6: upctl
	add r7, r6, #0x8000	@ r8: sysmgr
	mov r8, r1		@ r7: phy
	ldr r9, [r6]		@ r9: scfg
	mov r10, r3		@ r10: plldiv

	bic r0, r9, #1
	str r0, [r6]		@ disable HW lowpower

	mov r0, #5
	bl 1f			@ move to lowpower
	
	mov r0, r10
	bl 2f			@ change frequency

	mov r0, #4
	sub r1, r8, #0x1c00
	bic r1, r1, #0xff
	str r0, [r1]

	mov r2, #0x100
100:
	sub r2, r2, #1
	cmp r2, #0
	nop
	bne 100b

	mov r0, #0
	str r0, [r1]

	@ move to config
	mov r0, #1
	bl 1f
	
	@set new tog
	lsl r0, r5, #3
	add r0, r0, r5
	add r0, r0, r5
	str r0, [r6, #0xc0]
	sub r0, r5, #1
	str r5, [r6, #0xcc]
	
	@DQS
	ldr r0, =0x7ffff000
	str r0, [r7, #0x54]
	mov r0, #0x81
	str r0, [r7, #4]
3:
	ldr r0, [r7, #12]
	tst r0, #0x10
	beq 3b

	@move to access
	mov r0, #3
	bl 1f

	@finish
	mov r0, #0
	str r0, [r6, #0x208]
	str r9, [r6]

	mov r0, #0

	pop {r4 - r10, lr}
	mov pc, lr
	

	@__stmov
1:
	ldr r2, =0x43120
4:
	ldr r1, [r6, #8]  @@ r6, upctl
	and r1, r1, #7
	cmp r0, r1

	moveq pc, lr

	lsl r3, r1, #2
	suble r3, r3, #4
	lsr r3, r2, r3
	and r3, r3, #15
	str r3, [r6, #4]
	b 4b

	@__pllc
2:
	mov r1, #1
	str r1, [r8, #12]

	and r1, r0, #0xff
	str r1, [r8]

	lsr r1, r0, #8
	and r1, r1, #0xff
	str r1, [r8, #4]

	mov r1, #0
	str r1, [r8, #16]

	mov r1, #2
	str r1, [r8, #8]

	mov r1, #3
	str r1, [r8, #12]

	mov r1, #1
	str r1, [r8, #16]

	ldr r2, =6666
5:
	ldr r1, [r8, #20]
	tst r1, #1
	bne 6f
	sub r2, r2, #1
	cmp r2, #0
	bne 5b
6:
	mov r1, #2
	str r1, [r8, #12]
	mov pc, lr

