
/home/warits/code/works/irom/infotm-irom/out/vmirom:     file format elf32-littlearm


Disassembly of section .text:

00000000 <_start>:
       0:	ea000012 	b	50 <reset>
       4:	ebfffffe 	bl	4 <_start+0x4>
       8:	ebfffffe 	bl	8 <_start+0x8>
       c:	ebfffffe 	bl	c <_start+0xc>
      10:	ebfffffe 	bl	10 <_start+0x10>
      14:	ebfffffe 	bl	14 <_start+0x14>
      18:	ebfffffe 	bl	18 <_start+0x18>
      1c:	ebfffffe 	bl	1c <_start+0x1c>
      20:	00000000 	.word	0x00000000
      24:	00000001 	.word	0x00000001
      28:	00000002 	.word	0x00000002
      2c:	00000003 	.word	0x00000003
      30:	00000004 	.word	0x00000004
      34:	00000005 	.word	0x00000005
      38:	00000006 	.word	0x00000006
      3c:	00000007 	.word	0x00000007

00000040 <_armboot_start>:
      40:	00000000 	.word	0x00000000

00000044 <_bss_start>:
      44:	3c00d128 	.word	0x3c00d128

00000048 <_bss_end>:
      48:	3c00dda0 	.word	0x3c00dda0

0000004c <__stack>:
      4c:	3c00cfe0 	.word	0x3c00cfe0

00000050 <reset>:
      50:	e10f0000 	mrs	r0, CPSR
      54:	e3c0001f 	bic	r0, r0, #31	; 0x1f
      58:	e38000d3 	orr	r0, r0, #211	; 0xd3
      5c:	e129f000 	msr	CPSR_fc, r0
      60:	eb00000e 	bl	a0 <core_init_crit>
      64:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
      68:	e7e10050 	ubfx	r0, r0, #0, #2
      6c:	e3500000 	cmp	r0, #0	; 0x0
      70:	1a000019 	bne	dc <core_hold>
      74:	e51fd030 	ldr	sp, [pc, #-48]	; 4c <__stack>
      78:	eb00046e 	bl	1238 <chip_test>
      7c:	e51f0040 	ldr	r0, [pc, #-64]	; 44 <_bss_start>
      80:	e51f1040 	ldr	r1, [pc, #-64]	; 48 <_bss_end>
      84:	e3a02000 	mov	r2, #0	; 0x0

00000088 <clbss_l>:
      88:	e5802000 	str	r2, [r0]
      8c:	e2800004 	add	r0, r0, #4	; 0x4
      90:	e1500001 	cmp	r0, r1
      94:	1afffffb 	bne	88 <clbss_l>
      98:	e51ff004 	ldr	pc, [pc, #-4]	; 9c <_main>

0000009c <_main>:
      9c:	00000550 	.word	0x00000550

000000a0 <core_init_crit>:
      a0:	e3a00000 	mov	r0, #0	; 0x0
      a4:	ee080f17 	mcr	15, 0, r0, cr8, cr7, {0}
      a8:	ee070f15 	mcr	15, 0, r0, cr7, cr5, {0}
      ac:	ee110f10 	mrc	15, 0, r0, cr1, cr0, {0}
      b0:	e3c00a02 	bic	r0, r0, #8192	; 0x2000
      b4:	e3c00007 	bic	r0, r0, #7	; 0x7
      b8:	e3800002 	orr	r0, r0, #2	; 0x2
      bc:	e3800b02 	orr	r0, r0, #2048	; 0x800
      c0:	ee010f10 	mcr	15, 0, r0, cr1, cr0, {0}
      c4:	e59f1070 	ldr	r1, [pc, #112]	; 13c <low_power+0x8>
      c8:	e59f0070 	ldr	r0, [pc, #112]	; 140 <low_power+0xc>
      cc:	e5810034 	str	r0, [r1, #52]
      d0:	e59f006c 	ldr	r0, [pc, #108]	; 144 <low_power+0x10>
      d4:	e5810034 	str	r0, [r1, #52]
      d8:	e1a0f00e 	mov	pc, lr

000000dc <core_hold>:
      dc:	e59f3064 	ldr	r3, [pc, #100]	; 148 <low_power+0x14>
      e0:	e5931000 	ldr	r1, [r3]
      e4:	e3a0203b 	mov	r2, #59	; 0x3b
      e8:	e1510002 	cmp	r1, r2
      ec:	1a000010 	bne	134 <low_power>
      f0:	e593100c 	ldr	r1, [r3, #12]
      f4:	e1a02401 	lsl	r2, r1, #8
      f8:	e5931008 	ldr	r1, [r3, #8]
      fc:	e1822001 	orr	r2, r2, r1
     100:	e1a02402 	lsl	r2, r2, #8
     104:	e5931004 	ldr	r1, [r3, #4]
     108:	e1822001 	orr	r2, r2, r1
     10c:	e1a01402 	lsl	r1, r2, #8
     110:	e1a02100 	lsl	r2, r0, #2
     114:	e0821001 	add	r1, r2, r1
     118:	e3a02001 	mov	r2, #1	; 0x1
     11c:	e5812000 	str	r2, [r1]
     120:	f57ff04f 	.word	0xf57ff04f

00000124 <hold_l>:
     124:	e5910000 	ldr	r0, [r1]
     128:	e3500001 	cmp	r0, #1	; 0x1
     12c:	0afffffc 	beq	124 <hold_l>
     130:	e1a0f000 	mov	pc, r0

00000134 <low_power>:
     134:	e320f003 	.word	0xe320f003
     138:	eafffffe 	b	138 <low_power+0x4>
     13c:	21b00600 	.word	0x21b00600
     140:	12345678 	.word	0x12345678
     144:	87654321 	.word	0x87654321
     148:	21e09c2c 	.word	0x21e09c2c
     14c:	e1a00000 	.word	0xe1a00000

00000150 <bl_load_isi>:
     150:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     154:	e1a04001 	mov	r4, r1
     158:	e24dd02c 	sub	sp, sp, #44	; 0x2c
     15c:	e58d0024 	str	r0, [sp, #36]
     160:	e3a01001 	mov	r1, #1	; 0x1
     164:	e1a00004 	mov	r0, r4
     168:	e1a0b002 	mov	fp, r2
     16c:	eb001901 	bl	6578 <vs_assign_by_id>
     170:	e2507000 	subs	r7, r0, #0	; 0x0
     174:	0a000008 	beq	19c <bl_load_isi+0x4c>
     178:	e59f31a0 	ldr	r3, [pc, #416]	; 320 <bl_load_isi+0x1d0>
     17c:	e1a00004 	mov	r0, r4
     180:	e5934000 	ldr	r4, [r3]
     184:	eb001805 	bl	61a0 <vs_device_name>
     188:	e1a02007 	mov	r2, r7
     18c:	e1a01000 	mov	r1, r0
     190:	e59f018c 	ldr	r0, [pc, #396]	; 324 <bl_load_isi+0x1d4>
     194:	e12fff34 	blx	r4
     198:	ea00005d 	b	314 <bl_load_isi+0x1c4>
     19c:	e59f1184 	ldr	r1, [pc, #388]	; 328 <bl_load_isi+0x1d8>
     1a0:	e1a00004 	mov	r0, r4
     1a4:	eb001834 	bl	627c <vs_is_device>
     1a8:	e3500000 	cmp	r0, #0	; 0x0
     1ac:	03a01b01 	moveq	r1, #1024	; 0x400
     1b0:	058d1008 	streq	r1, [sp, #8]
     1b4:	0a000006 	beq	1d4 <bl_load_isi+0x84>
     1b8:	e1a00007 	mov	r0, r7
     1bc:	eb0029df 	bl	a940 <nand_get_config>
     1c0:	e3500000 	cmp	r0, #0	; 0x0
     1c4:	03e07033 	mvneq	r7, #51	; 0x33
     1c8:	0a000051 	beq	314 <bl_load_isi+0x1c4>
     1cc:	e5900018 	ldr	r0, [r0, #24]
     1d0:	e58d0008 	str	r0, [sp, #8]
     1d4:	e59f2150 	ldr	r2, [pc, #336]	; 32c <bl_load_isi+0x1dc>
     1d8:	e89b0003 	ldm	fp, {r0, r1}
     1dc:	e3a03000 	mov	r3, #0	; 0x0
     1e0:	e0922000 	adds	r2, r2, r0
     1e4:	e59f0144 	ldr	r0, [pc, #324]	; 330 <bl_load_isi+0x1e0>
     1e8:	e0a33001 	adc	r3, r3, r1
     1ec:	e3e01000 	mvn	r1, #0	; 0x0
     1f0:	e0022000 	and	r2, r2, r0
     1f4:	e0033001 	and	r3, r3, r1
     1f8:	e88b000c 	stm	fp, {r2, r3}
     1fc:	e59d3008 	ldr	r3, [sp, #8]
     200:	e3a04000 	mov	r4, #0	; 0x0
     204:	e1a02003 	mov	r2, r3
     208:	e1a03fc2 	asr	r3, r2, #31
     20c:	e1cd21f8 	strd	r2, [sp, #24]
     210:	e58d400c 	str	r4, [sp, #12]
     214:	e1a06004 	mov	r6, r4
     218:	e1a0a004 	mov	sl, r4
     21c:	e59d8008 	ldr	r8, [sp, #8]
     220:	e1540008 	cmp	r4, r8
     224:	1a00000e 	bne	264 <bl_load_isi+0x114>
     228:	e59d0024 	ldr	r0, [sp, #36]
     22c:	eb000657 	bl	1b90 <isi_check_header>
     230:	e2502000 	subs	r2, r0, #0	; 0x0
     234:	e59f00f8 	ldr	r0, [pc, #248]	; 334 <bl_load_isi+0x1e4>
     238:	1a000007 	bne	25c <bl_load_isi+0x10c>
     23c:	e59d9024 	ldr	r9, [sp, #36]
     240:	e599300c 	ldr	r3, [r9, #12]
     244:	e2836c02 	add	r6, r3, #512	; 0x200
     248:	e1a01006 	mov	r1, r6
     24c:	e59f30cc 	ldr	r3, [pc, #204]	; 320 <bl_load_isi+0x1d0>
     250:	e1a0e00f 	mov	lr, pc
     254:	e593f000 	ldr	pc, [r3]
     258:	ea000001 	b	264 <bl_load_isi+0x114>
     25c:	e59f00d4 	ldr	r0, [pc, #212]	; 338 <bl_load_isi+0x1e8>
     260:	ea00000c 	b	298 <bl_load_isi+0x148>
     264:	e2565000 	subs	r5, r6, #0	; 0x0
     268:	13a05001 	movne	r5, #1	; 0x1
     26c:	e1540006 	cmp	r4, r6
     270:	e59d8024 	ldr	r8, [sp, #36]
     274:	b3a05000 	movlt	r5, #0	; 0x0
     278:	e3550000 	cmp	r5, #0	; 0x0
     27c:	e0880004 	add	r0, r8, r4
     280:	0a000006 	beq	2a0 <bl_load_isi+0x150>
     284:	e1a00008 	mov	r0, r8
     288:	eb000699 	bl	1cf4 <isi_check_data>
     28c:	e3500000 	cmp	r0, #0	; 0x0
     290:	0a00001f 	beq	314 <bl_load_isi+0x1c4>
     294:	e59f00a0 	ldr	r0, [pc, #160]	; 33c <bl_load_isi+0x1ec>
     298:	e3a01000 	mov	r1, #0	; 0x0
     29c:	ea000004 	b	2b4 <bl_load_isi+0x164>
     2a0:	e35a0004 	cmp	sl, #4	; 0x4
     2a4:	e28aa001 	add	sl, sl, #1	; 0x1
     2a8:	da000005 	ble	2c4 <bl_load_isi+0x174>
     2ac:	e59f0088 	ldr	r0, [pc, #136]	; 33c <bl_load_isi+0x1ec>
     2b0:	e1a01005 	mov	r1, r5
     2b4:	e59f3064 	ldr	r3, [pc, #100]	; 320 <bl_load_isi+0x1d0>
     2b8:	e1a0e00f 	mov	lr, pc
     2bc:	e593f000 	ldr	pc, [r3]
     2c0:	ea000012 	b	310 <bl_load_isi+0x1c0>
     2c4:	e59d9008 	ldr	r9, [sp, #8]
     2c8:	e89b000c 	ldm	fp, {r2, r3}
     2cc:	e58d9000 	str	r9, [sp]
     2d0:	e58d5004 	str	r5, [sp, #4]
     2d4:	eb001801 	bl	62e0 <vs_read>
     2d8:	e89b000c 	ldm	fp, {r2, r3}
     2dc:	e1cd81d8 	ldrd	r8, [sp, #24]
     2e0:	e0922008 	adds	r2, r2, r8
     2e4:	e0a33009 	adc	r3, r3, r9
     2e8:	e59d100c 	ldr	r1, [sp, #12]
     2ec:	e59d9008 	ldr	r9, [sp, #8]
     2f0:	e2811001 	add	r1, r1, #1	; 0x1
     2f4:	e1500009 	cmp	r0, r9
     2f8:	00844009 	addeq	r4, r4, r9
     2fc:	01a0a005 	moveq	sl, r5
     300:	e3510080 	cmp	r1, #128	; 0x80
     304:	e88b000c 	stm	fp, {r2, r3}
     308:	e58d100c 	str	r1, [sp, #12]
     30c:	1affffc2 	bne	21c <bl_load_isi+0xcc>
     310:	e3e07000 	mvn	r7, #0	; 0x0
     314:	e1a00007 	mov	r0, r7
     318:	e28dd02c 	add	sp, sp, #44	; 0x2c
     31c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     320:	3c00d694 	.word	0x3c00d694
     324:	00019d7f 	.word	0x00019d7f
     328:	00019da3 	.word	0x00019da3
     32c:	0001ffff 	.word	0x0001ffff
     330:	fffe0000 	.word	0xfffe0000
     334:	00019da7 	.word	0x00019da7
     338:	00019dbe 	.word	0x00019dbe
     33c:	00019dd0 	.word	0x00019dd0

00000340 <try_get_bl>:
     340:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     344:	e3a02602 	mov	r2, #2097152	; 0x200000
     348:	e24dd00c 	sub	sp, sp, #12	; 0xc
     34c:	e3a03000 	mov	r3, #0	; 0x0
     350:	e88d000c 	stm	sp, {r2, r3}
     354:	e1a07001 	mov	r7, r1
     358:	e1a06000 	mov	r6, r0
     35c:	eb0000ab 	bl	610 <boot_device>
     360:	e1a04000 	mov	r4, r0
     364:	eb001746 	bl	6084 <vs_device_bootable>
     368:	e2505000 	subs	r5, r0, #0	; 0x0
     36c:	1a000008 	bne	394 <try_get_bl+0x54>
     370:	e59f308c 	ldr	r3, [pc, #140]	; 404 <try_get_bl+0xc4>
     374:	e1a00004 	mov	r0, r4
     378:	e5934000 	ldr	r4, [r3]
     37c:	eb001787 	bl	61a0 <vs_device_name>
     380:	e1a01000 	mov	r1, r0
     384:	e59f007c 	ldr	r0, [pc, #124]	; 408 <try_get_bl+0xc8>
     388:	e12fff34 	blx	r4
     38c:	e1a00005 	mov	r0, r5
     390:	ea000019 	b	3fc <try_get_bl+0xbc>
     394:	e1a00004 	mov	r0, r4
     398:	eb00176e 	bl	6158 <vs_device_boot0>
     39c:	e3500000 	cmp	r0, #0	; 0x0
     3a0:	13a02000 	movne	r2, #0	; 0x0
     3a4:	13a03000 	movne	r3, #0	; 0x0
     3a8:	188d000c 	stmne	sp, {r2, r3}
     3ac:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
     3b0:	e1a01004 	mov	r1, r4
     3b4:	e1a0200d 	mov	r2, sp
     3b8:	e3a03000 	mov	r3, #0	; 0x0
     3bc:	ebffff63 	bl	150 <bl_load_isi>
     3c0:	e3500000 	cmp	r0, #0	; 0x0
     3c4:	13a00000 	movne	r0, #0	; 0x0
     3c8:	1a00000b 	bne	3fc <try_get_bl+0xbc>
     3cc:	e3a0330f 	mov	r3, #1006632960	; 0x3c000000
     3d0:	e5932008 	ldr	r2, [r3, #8]
     3d4:	e3c22102 	bic	r2, r2, #-2147483648	; 0x80000000
     3d8:	e5862000 	str	r2, [r6]
     3dc:	e5874000 	str	r4, [r7]
     3e0:	e5933008 	ldr	r3, [r3, #8]
     3e4:	e3530000 	cmp	r3, #0	; 0x0
     3e8:	aa000002 	bge	3f8 <try_get_bl+0xb8>
     3ec:	e3a00001 	mov	r0, #1	; 0x1
     3f0:	e59f1014 	ldr	r1, [pc, #20]	; 40c <try_get_bl+0xcc>
     3f4:	eb001c33 	bl	74c8 <cdbg_verify_burn_enable>
     3f8:	e3a00001 	mov	r0, #1	; 0x1
     3fc:	e28dd00c 	add	sp, sp, #12	; 0xc
     400:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     404:	3c00d694 	.word	0x3c00d694
     408:	00019de2 	.word	0x00019de2
     40c:	3c000100 	.word	0x3c000100

00000410 <try_boot>:
     410:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
     414:	e3a03000 	mov	r3, #0	; 0x0
     418:	e24dd008 	sub	sp, sp, #8	; 0x8
     41c:	e1a07000 	mov	r7, r0
     420:	e1a00003 	mov	r0, r3
     424:	e58d3004 	str	r3, [sp, #4]
     428:	e58d3000 	str	r3, [sp]
     42c:	e1a04003 	mov	r4, r3
     430:	eb00005d 	bl	5ac <set_xom>
     434:	eb000062 	bl	5c4 <boot_info>
     438:	e28d8004 	add	r8, sp, #4	; 0x4
     43c:	e1a05000 	mov	r5, r0
     440:	ea000008 	b	468 <try_boot+0x58>
     444:	eb000170 	bl	a0c <gtc_time>
     448:	e1a00004 	mov	r0, r4
     44c:	eb000056 	bl	5ac <set_xom>
     450:	e1a00008 	mov	r0, r8
     454:	e1a0100d 	mov	r1, sp
     458:	ebffffb8 	bl	340 <try_get_bl>
     45c:	e3500000 	cmp	r0, #0	; 0x0
     460:	13a04001 	movne	r4, #1	; 0x1
     464:	1a000003 	bne	478 <try_boot+0x68>
     468:	e1540005 	cmp	r4, r5
     46c:	e2844001 	add	r4, r4, #1	; 0x1
     470:	bafffff3 	blt	444 <try_boot+0x34>
     474:	e3a04000 	mov	r4, #0	; 0x0
     478:	eb000163 	bl	a0c <gtc_time>
     47c:	e3a00001 	mov	r0, #1	; 0x1
     480:	eb000177 	bl	a64 <ecfg_check_flag>
     484:	e3500000 	cmp	r0, #0	; 0x0
     488:	1a00000c 	bne	4c0 <try_boot+0xb0>
     48c:	e3570000 	cmp	r7, #0	; 0x0
     490:	1a00000a 	bne	4c0 <try_boot+0xb0>
     494:	eb0000da 	bl	804 <get_reset_stats>
     498:	e3500000 	cmp	r0, #0	; 0x0
     49c:	13500002 	cmpne	r0, #2	; 0x2
     4a0:	1a000006 	bne	4c0 <try_boot+0xb0>
     4a4:	e3500002 	cmp	r0, #2	; 0x2
     4a8:	1a000001 	bne	4b4 <try_boot+0xa4>
     4ac:	e28d0004 	add	r0, sp, #4	; 0x4
     4b0:	eb0000e0 	bl	838 <update_mask_on_reset>
     4b4:	e59d0004 	ldr	r0, [sp, #4]
     4b8:	eb001e81 	bl	7ec4 <burn>
     4bc:	eb000152 	bl	a0c <gtc_time>
     4c0:	e3540000 	cmp	r4, #0	; 0x0
     4c4:	0a000004 	beq	4dc <try_boot+0xcc>
     4c8:	e3a0330f 	mov	r3, #1006632960	; 0x3c000000
     4cc:	e5930014 	ldr	r0, [r3, #20]
     4d0:	e59d1000 	ldr	r1, [sp]
     4d4:	eb001c06 	bl	74f4 <cdbg_jump>
     4d8:	eafffffe 	b	4d8 <try_boot+0xc8>
     4dc:	eb001c36 	bl	75bc <cdbg_shell>
     4e0:	eafffffe 	b	4e0 <try_boot+0xd0>

000004e4 <init_data_section>:
     4e4:	e59f1010 	ldr	r1, [pc, #16]	; 4fc <init_data_section+0x18>
     4e8:	e59f0010 	ldr	r0, [pc, #16]	; 500 <init_data_section+0x1c>
     4ec:	e1510000 	cmp	r1, r0
     4f0:	012fff1e 	bxeq	lr
     4f4:	e59f2008 	ldr	r2, [pc, #8]	; 504 <init_data_section+0x20>
     4f8:	ea00043c 	b	15f0 <memcpy>
     4fc:	0001c9b8 	.word	0x0001c9b8
     500:	3c00d000 	.word	0x3c00d000
     504:	00000128 	.word	0x00000128

00000508 <initialization>:
     508:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     50c:	e24dd004 	sub	sp, sp, #4	; 0x4
     510:	ebfffff3 	bl	4e4 <init_data_section>
     514:	eb0001c9 	bl	c40 <init_efuse>
     518:	eb000028 	bl	5c0 <init_xom>
     51c:	eb0000f8 	bl	904 <init_lowlevel>
     520:	eb00055c 	bl	1a98 <init_malloc>
     524:	eb002c3e 	bl	b624 <init_timer>
     528:	eb000144 	bl	a40 <init_gtc>
     52c:	eb002bcf 	bl	b470 <init_serial>
     530:	eb001fbe 	bl	8430 <init_mmc>
     534:	eb0029bd 	bl	ac30 <init_nand>
     538:	eb0000b0 	bl	800 <init_crypto>
     53c:	eb000295 	bl	f98 <init_alternate>
     540:	eb000039 	bl	62c <init_boot_sequence>
     544:	e3a00000 	mov	r0, #0	; 0x0
     548:	e28dd004 	add	sp, sp, #4	; 0x4
     54c:	e8bd8000 	pop	{pc}

00000550 <main>:
     550:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     554:	e3a00000 	mov	r0, #0	; 0x0
     558:	e24dd004 	sub	sp, sp, #4	; 0x4
     55c:	eb001bbd 	bl	7458 <cdbg_log_toggle>
     560:	ebffffe8 	bl	508 <initialization>
     564:	eb000128 	bl	a0c <gtc_time>
     568:	e3a00001 	mov	r0, #1	; 0x1
     56c:	eb001bb9 	bl	7458 <cdbg_log_toggle>
     570:	e59f001c 	ldr	r0, [pc, #28]	; 594 <main+0x44>
     574:	e59f301c 	ldr	r3, [pc, #28]	; 598 <main+0x48>
     578:	e1a0e00f 	mov	lr, pc
     57c:	e593f000 	ldr	pc, [r3]
     580:	e3a00000 	mov	r0, #0	; 0x0
     584:	eb001bb3 	bl	7458 <cdbg_log_toggle>
     588:	e3a00000 	mov	r0, #0	; 0x0
     58c:	ebffff9f 	bl	410 <try_boot>
     590:	eafffffe 	b	590 <main+0x40>
     594:	00019dfc 	.word	0x00019dfc
     598:	3c00d694 	.word	0x3c00d694

0000059c <get_xom>:
     59c:	e59f3004 	ldr	r3, [pc, #4]	; 5a8 <get_xom+0xc>
     5a0:	e1d300b0 	ldrh	r0, [r3]
     5a4:	e12fff1e 	bx	lr
     5a8:	3c00d128 	.word	0x3c00d128

000005ac <set_xom>:
     5ac:	e59f3008 	ldr	r3, [pc, #8]	; 5bc <set_xom+0x10>
     5b0:	e200001f 	and	r0, r0, #31	; 0x1f
     5b4:	e1c300b0 	strh	r0, [r3]
     5b8:	e12fff1e 	bx	lr
     5bc:	3c00d128 	.word	0x3c00d128

000005c0 <init_xom>:
     5c0:	e12fff1e 	bx	lr

000005c4 <boot_info>:
     5c4:	e92d4010 	push	{r4, lr}
     5c8:	eb00012e 	bl	a88 <ecfg_paras>
     5cc:	e1a04000 	mov	r4, r0
     5d0:	e3a00b01 	mov	r0, #1024	; 0x400
     5d4:	eb000122 	bl	a64 <ecfg_check_flag>
     5d8:	e59f3028 	ldr	r3, [pc, #40]	; 608 <boot_info+0x44>
     5dc:	e3500000 	cmp	r0, #0	; 0x0
     5e0:	11d330b0 	ldrhne	r3, [r3]
     5e4:	01d330b0 	ldrheq	r3, [r3]
     5e8:	1203301f 	andne	r3, r3, #31	; 0x1f
     5ec:	0203301f 	andeq	r3, r3, #31	; 0x1f
     5f0:	159f2014 	ldrne	r2, [pc, #20]	; 60c <boot_info+0x48>
     5f4:	11a03083 	lslne	r3, r3, #1
     5f8:	00843083 	addeq	r3, r4, r3, lsl #1
     5fc:	119200b3 	ldrhne	r0, [r2, r3]
     600:	01d302b0 	ldrheq	r0, [r3, #32]
     604:	e8bd8010 	pop	{r4, pc}
     608:	3c00d128 	.word	0x3c00d128
     60c:	3c00d000 	.word	0x3c00d000

00000610 <boot_device>:
     610:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     614:	e24dd004 	sub	sp, sp, #4	; 0x4
     618:	ebffffe9 	bl	5c4 <boot_info>
     61c:	e1a00620 	lsr	r0, r0, #12
     620:	e6ff0070 	uxth	r0, r0
     624:	e28dd004 	add	sp, sp, #4	; 0x4
     628:	e8bd8000 	pop	{pc}

0000062c <init_boot_sequence>:
     62c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     630:	e24dd004 	sub	sp, sp, #4	; 0x4
     634:	eb00024d 	bl	f70 <get_boot_pin_value>
     638:	e2000003 	and	r0, r0, #3	; 0x3
     63c:	e3500001 	cmp	r0, #1	; 0x1
     640:	0a000002 	beq	650 <init_boot_sequence+0x24>
     644:	e3500002 	cmp	r0, #2	; 0x2
     648:	1a00000d 	bne	684 <init_boot_sequence+0x58>
     64c:	ea000007 	b	670 <init_boot_sequence+0x44>
     650:	e59f3034 	ldr	r3, [pc, #52]	; 68c <init_boot_sequence+0x60>
     654:	e1d320b6 	ldrh	r2, [r3, #6]
     658:	e1d310b8 	ldrh	r1, [r3, #8]
     65c:	e1c320b8 	strh	r2, [r3, #8]
     660:	e1d320ba 	ldrh	r2, [r3, #10]
     664:	e1c310ba 	strh	r1, [r3, #10]
     668:	e1c320b6 	strh	r2, [r3, #6]
     66c:	ea000004 	b	684 <init_boot_sequence+0x58>
     670:	e59f3014 	ldr	r3, [pc, #20]	; 68c <init_boot_sequence+0x60>
     674:	e1d320b6 	ldrh	r2, [r3, #6]
     678:	e1d310b8 	ldrh	r1, [r3, #8]
     67c:	e1c320b8 	strh	r2, [r3, #8]
     680:	e1c310b6 	strh	r1, [r3, #6]
     684:	e28dd004 	add	sp, sp, #4	; 0x4
     688:	e8bd8000 	pop	{pc}
     68c:	3c00d000 	.word	0x3c00d000

00000690 <set_pll>:
     690:	e3500003 	cmp	r0, #3	; 0x3
     694:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
     698:	e1a02001 	mov	r2, r1
     69c:	88bd85f0 	pophi	{r4, r5, r6, r7, r8, sl, pc}
     6a0:	e3a03018 	mov	r3, #24	; 0x18
     6a4:	e0030093 	mul	r3, r3, r0
     6a8:	e59f808c 	ldr	r8, [pc, #140]	; 73c <set_pll+0xac>
     6ac:	e59f108c 	ldr	r1, [pc, #140]	; 740 <set_pll+0xb0>
     6b0:	e59f008c 	ldr	r0, [pc, #140]	; 744 <set_pll+0xb4>
     6b4:	e59fc08c 	ldr	ip, [pc, #140]	; 748 <set_pll+0xb8>
     6b8:	e59f408c 	ldr	r4, [pc, #140]	; 74c <set_pll+0xbc>
     6bc:	e59fa08c 	ldr	sl, [pc, #140]	; 750 <set_pll+0xc0>
     6c0:	e0838008 	add	r8, r3, r8
     6c4:	e083c00c 	add	ip, r3, ip
     6c8:	e0831001 	add	r1, r3, r1
     6cc:	e3a07001 	mov	r7, #1	; 0x1
     6d0:	e0830000 	add	r0, r3, r0
     6d4:	e1a06422 	lsr	r6, r2, #8
     6d8:	e0834004 	add	r4, r3, r4
     6dc:	e3a05000 	mov	r5, #0	; 0x0
     6e0:	e6ef2072 	uxtb	r2, r2
     6e4:	e083a00a 	add	sl, r3, sl
     6e8:	e3a03002 	mov	r3, #2	; 0x2
     6ec:	e5c87000 	strb	r7, [r8]
     6f0:	e5c12000 	strb	r2, [r1]
     6f4:	e5c06000 	strb	r6, [r0]
     6f8:	e5cc5000 	strb	r5, [ip]
     6fc:	e5c43000 	strb	r3, [r4]
     700:	e0833007 	add	r3, r3, r7
     704:	e5c83000 	strb	r3, [r8]
     708:	e1a02005 	mov	r2, r5
     70c:	e5cc7000 	strb	r7, [ip]
     710:	e3011a0a 	movw	r1, #6666	; 0x1a0a
     714:	ea000001 	b	720 <set_pll+0x90>
     718:	e1520001 	cmp	r2, r1
     71c:	0a000003 	beq	730 <set_pll+0xa0>
     720:	e5da3000 	ldrb	r3, [sl]
     724:	e2822001 	add	r2, r2, #1	; 0x1
     728:	e3130001 	tst	r3, #1	; 0x1
     72c:	0afffff9 	beq	718 <set_pll+0x88>
     730:	e3a03002 	mov	r3, #2	; 0x2
     734:	e5883000 	str	r3, [r8]
     738:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
     73c:	21e0a00c 	.word	0x21e0a00c
     740:	21e0a000 	.word	0x21e0a000
     744:	21e0a004 	.word	0x21e0a004
     748:	21e0a010 	.word	0x21e0a010
     74c:	21e0a008 	.word	0x21e0a008
     750:	21e0a014 	.word	0x21e0a014

00000754 <set_clk_divider>:
     754:	e3500005 	cmp	r0, #5	; 0x5
     758:	812fff1e 	bxhi	lr
     75c:	e59fc020 	ldr	ip, [pc, #32]	; 784 <set_clk_divider+0x30>
     760:	e1a03280 	lsl	r3, r0, #5
     764:	e083c00c 	add	ip, r3, ip
     768:	e3a02000 	mov	r2, #0	; 0x0
     76c:	e7d13002 	ldrb	r3, [r1, r2]
     770:	e2822001 	add	r2, r2, #1	; 0x1
     774:	e3520008 	cmp	r2, #8	; 0x8
     778:	e4cc3004 	strb	r3, [ip], #4
     77c:	1afffffa 	bne	76c <set_clk_divider+0x18>
     780:	e12fff1e 	bx	lr
     784:	21e0a070 	.word	0x21e0a070

00000788 <module_set_clock>:
     788:	e59f3020 	ldr	r3, [pc, #32]	; 7b0 <module_set_clock+0x28>
     78c:	e1500003 	cmp	r0, r3
     790:	13a03001 	movne	r3, #1	; 0x1
     794:	1580300c 	strne	r3, [r0, #12]
     798:	e2013007 	and	r3, r1, #7	; 0x7
     79c:	e5803000 	str	r3, [r0]
     7a0:	e3a03000 	mov	r3, #0	; 0x0
     7a4:	e5802004 	str	r2, [r0, #4]
     7a8:	e580300c 	str	r3, [r0, #12]
     7ac:	e12fff1e 	bx	lr
     7b0:	21e0a360 	.word	0x21e0a360

000007b4 <module_enable>:
     7b4:	e3a020ff 	mov	r2, #255	; 0xff
     7b8:	e3a01000 	mov	r1, #0	; 0x0
     7bc:	e3a03011 	mov	r3, #17	; 0x11
     7c0:	e580100c 	str	r1, [r0, #12]
     7c4:	e5802000 	str	r2, [r0]
     7c8:	e5802004 	str	r2, [r0, #4]
     7cc:	e5803008 	str	r3, [r0, #8]
     7d0:	e5903008 	ldr	r3, [r0, #8]
     7d4:	e3c33010 	bic	r3, r3, #16	; 0x10
     7d8:	e5803008 	str	r3, [r0, #8]
     7dc:	e5802018 	str	r2, [r0, #24]
     7e0:	e5801000 	str	r1, [r0]
     7e4:	e580200c 	str	r2, [r0, #12]
     7e8:	e12fff1e 	bx	lr

000007ec <init_igps_ram>:
     7ec:	e59f3008 	ldr	r3, [pc, #8]	; 7fc <init_igps_ram+0x10>
     7f0:	e3a02001 	mov	r2, #1	; 0x1
     7f4:	e5832c10 	str	r2, [r3, #3088]
     7f8:	e12fff1e 	bx	lr
     7fc:	21e0a000 	.word	0x21e0a000

00000800 <init_crypto>:
     800:	e12fff1e 	bx	lr

00000804 <get_reset_stats>:
     804:	e59f2028 	ldr	r2, [pc, #40]	; 834 <get_reset_stats+0x30>
     808:	e5923c10 	ldr	r3, [r2, #3088]
     80c:	e2130002 	ands	r0, r3, #2	; 0x2
     810:	012fff1e 	bxeq	lr
     814:	e5920c2c 	ldr	r0, [r2, #3116]
     818:	e3100001 	tst	r0, #1	; 0x1
     81c:	03a00001 	moveq	r0, #1	; 0x1
     820:	012fff1e 	bxeq	lr
     824:	e3500037 	cmp	r0, #55	; 0x37
     828:	03a00002 	moveq	r0, #2	; 0x2
     82c:	13a00003 	movne	r0, #3	; 0x3
     830:	e12fff1e 	bx	lr
     834:	21e09000 	.word	0x21e09000

00000838 <update_mask_on_reset>:
     838:	e59f3018 	ldr	r3, [pc, #24]	; 858 <update_mask_on_reset+0x20>
     83c:	e5932c30 	ldr	r2, [r3, #3120]
     840:	e3120080 	tst	r2, #128	; 0x80
     844:	13a03001 	movne	r3, #1	; 0x1
     848:	1202200f 	andne	r2, r2, #15	; 0xf
     84c:	11e03213 	mvnne	r3, r3, lsl r2
     850:	15803000 	strne	r3, [r0]
     854:	e12fff1e 	bx	lr
     858:	21e09000 	.word	0x21e09000

0000085c <get_wakeup_entry>:
     85c:	e59f301c 	ldr	r3, [pc, #28]	; 880 <get_wakeup_entry+0x24>
     860:	e5931c38 	ldr	r1, [r3, #3128]
     864:	e5932c34 	ldr	r2, [r3, #3124]
     868:	e5930c30 	ldr	r0, [r3, #3120]
     86c:	e1822401 	orr	r2, r2, r1, lsl #8
     870:	e5933c2c 	ldr	r3, [r3, #3116]
     874:	e1800402 	orr	r0, r0, r2, lsl #8
     878:	e1830400 	orr	r0, r3, r0, lsl #8
     87c:	e12fff1e 	bx	lr
     880:	21e09000 	.word	0x21e09000

00000884 <system_reset>:
     884:	e59f3010 	ldr	r3, [pc, #16]	; 89c <system_reset+0x18>
     888:	e3a02001 	mov	r2, #1	; 0x1
     88c:	e5832c2c 	str	r2, [r3, #3116]
     890:	e2822002 	add	r2, r2, #2	; 0x2
     894:	e5832c00 	str	r2, [r3, #3072]
     898:	e12fff1e 	bx	lr
     89c:	21e09000 	.word	0x21e09000

000008a0 <pads_init_module>:
     8a0:	e92d4030 	push	{r4, r5, lr}
     8a4:	e1a05000 	mov	r5, r0
     8a8:	e24dd004 	sub	sp, sp, #4	; 0x4
     8ac:	e59f404c 	ldr	r4, [pc, #76]	; 900 <pads_init_module+0x60>
     8b0:	ea00000d 	b	8ec <pads_init_module+0x4c>
     8b4:	e1500005 	cmp	r0, r5
     8b8:	1a00000a 	bne	8e8 <pads_init_module+0x48>
     8bc:	e3a01000 	mov	r1, #0	; 0x0
     8c0:	e1a02001 	mov	r2, r1
     8c4:	e5140004 	ldr	r0, [r4, #-4]
     8c8:	eb0000f9 	bl	cb4 <pads_chmod>
     8cc:	e3a00101 	mov	r0, #1073741824	; 0x40000000
     8d0:	eb000063 	bl	a64 <ecfg_check_flag>
     8d4:	e3500000 	cmp	r0, #0	; 0x0
     8d8:	0a000002 	beq	8e8 <pads_init_module+0x48>
     8dc:	e8140005 	ldmda	r4, {r0, r2}
     8e0:	e3a01001 	mov	r1, #1	; 0x1
     8e4:	eb000147 	bl	e08 <pads_pull>
     8e8:	e284400c 	add	r4, r4, #12	; 0xc
     8ec:	e5140008 	ldr	r0, [r4, #-8]
     8f0:	e3500000 	cmp	r0, #0	; 0x0
     8f4:	1affffee 	bne	8b4 <pads_init_module+0x14>
     8f8:	e28dd004 	add	sp, sp, #4	; 0x4
     8fc:	e8bd8030 	pop	{r4, r5, pc}
     900:	3c00d014 	.word	0x3c00d014

00000904 <init_lowlevel>:
     904:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     908:	e24dd044 	sub	sp, sp, #68	; 0x44
     90c:	e3a01000 	mov	r1, #0	; 0x0
     910:	e3a02040 	mov	r2, #64	; 0x40
     914:	e1a0000d 	mov	r0, sp
     918:	eb00032c 	bl	15d0 <memset>
     91c:	e3a03063 	mov	r3, #99	; 0x63
     920:	e5cd3010 	strb	r3, [sp, #16]
     924:	e3e03073 	mvn	r3, #115	; 0x73
     928:	e5cd3011 	strb	r3, [sp, #17]
     92c:	e28330a5 	add	r3, r3, #165	; 0xa5
     930:	e5cd3012 	strb	r3, [sp, #18]
     934:	e3a0301f 	mov	r3, #31	; 0x1f
     938:	e5cd3018 	strb	r3, [sp, #24]
     93c:	e283305d 	add	r3, r3, #93	; 0x5d
     940:	e5cd3019 	strb	r3, [sp, #25]
     944:	e3e0300f 	mvn	r3, #15	; 0xf
     948:	e5cd301a 	strb	r3, [sp, #26]
     94c:	e2833020 	add	r3, r3, #32	; 0x20
     950:	e5cd3020 	strb	r3, [sp, #32]
     954:	e2833032 	add	r3, r3, #50	; 0x32
     958:	e5cd3021 	strb	r3, [sp, #33]
     95c:	e3a03002 	mov	r3, #2	; 0x2
     960:	e5cd3029 	strb	r3, [sp, #41]
     964:	e3e0301f 	mvn	r3, #31	; 0x1f
     968:	e5cd3030 	strb	r3, [sp, #48]
     96c:	e3e0307c 	mvn	r3, #124	; 0x7c
     970:	e5cd3031 	strb	r3, [sp, #49]
     974:	e283308c 	add	r3, r3, #140	; 0x8c
     978:	e3a02008 	mov	r2, #8	; 0x8
     97c:	e5cd3032 	strb	r3, [sp, #50]
     980:	e3a03001 	mov	r3, #1	; 0x1
     984:	e5cd202a 	strb	r2, [sp, #42]
     988:	e58d3038 	str	r3, [sp, #56]
     98c:	e5cd2022 	strb	r2, [sp, #34]
     990:	e3a01000 	mov	r1, #0	; 0x0
     994:	e3e00000 	mvn	r0, #0	; 0x0
     998:	e59f3044 	ldr	r3, [pc, #68]	; 9e4 <init_lowlevel+0xe0>
     99c:	e1a02101 	lsl	r2, r1, #2
     9a0:	e2811001 	add	r1, r1, #1	; 0x1
     9a4:	e0823003 	add	r3, r2, r3
     9a8:	e3510008 	cmp	r1, #8	; 0x8
     9ac:	e5830000 	str	r0, [r3]
     9b0:	1afffff8 	bne	998 <init_lowlevel+0x94>
     9b4:	e3012042 	movw	r2, #4162	; 0x1042
     9b8:	e28d0040 	add	r0, sp, #64	; 0x40
     9bc:	e3013031 	movw	r3, #4145	; 0x1031
     9c0:	e16024b0 	strh	r2, [r0, #-64]!
     9c4:	e1cd30b4 	strh	r3, [sp, #4]
     9c8:	e1a0000d 	mov	r0, sp
     9cc:	e302304f 	movw	r3, #8271	; 0x204f
     9d0:	e1cd30b2 	strh	r3, [sp, #2]
     9d4:	e1cd20b6 	strh	r2, [sp, #6]
     9d8:	eb0001c6 	bl	10f8 <set_clk>
     9dc:	e28dd044 	add	sp, sp, #68	; 0x44
     9e0:	e8bd8000 	pop	{pc}
     9e4:	21b01180 	.word	0x21b01180

000009e8 <module_reset>:
     9e8:	e3a030ff 	mov	r3, #255	; 0xff
     9ec:	e5803000 	str	r3, [r0]
     9f0:	e92d4010 	push	{r4, lr}
     9f4:	e1a04000 	mov	r4, r0
     9f8:	e3a0000a 	mov	r0, #10	; 0xa
     9fc:	eb002ac4 	bl	b514 <udelay>
     a00:	e3a03000 	mov	r3, #0	; 0x0
     a04:	e5843000 	str	r3, [r4]
     a08:	e8bd8010 	pop	{r4, pc}

00000a0c <gtc_time>:
     a0c:	e59f3020 	ldr	r3, [pc, #32]	; a34 <gtc_time+0x28>
     a10:	e92d4010 	push	{r4, lr}
     a14:	e5930000 	ldr	r0, [r3]
     a18:	e59f3018 	ldr	r3, [pc, #24]	; a38 <gtc_time+0x2c>
     a1c:	e5934000 	ldr	r4, [r3]
     a20:	eb002adf 	bl	b5a4 <get_timer>
     a24:	e1a01000 	mov	r1, r0
     a28:	e59f000c 	ldr	r0, [pc, #12]	; a3c <gtc_time+0x30>
     a2c:	e12fff34 	blx	r4
     a30:	e8bd8010 	pop	{r4, pc}
     a34:	3c00d12c 	.word	0x3c00d12c
     a38:	3c00d694 	.word	0x3c00d694
     a3c:	00019e01 	.word	0x00019e01

00000a40 <init_gtc>:
     a40:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     a44:	e3a00000 	mov	r0, #0	; 0x0
     a48:	e24dd004 	sub	sp, sp, #4	; 0x4
     a4c:	eb002ad4 	bl	b5a4 <get_timer>
     a50:	e59f3008 	ldr	r3, [pc, #8]	; a60 <init_gtc+0x20>
     a54:	e5830000 	str	r0, [r3]
     a58:	e28dd004 	add	sp, sp, #4	; 0x4
     a5c:	e8bd8000 	pop	{pc}
     a60:	3c00d12c 	.word	0x3c00d12c

00000a64 <ecfg_check_flag>:
     a64:	e3500801 	cmp	r0, #65536	; 0x10000
     a68:	0a000004 	beq	a80 <ecfg_check_flag+0x1c>
     a6c:	e3500101 	cmp	r0, #1073741824	; 0x40000000
     a70:	0a000002 	beq	a80 <ecfg_check_flag+0x1c>
     a74:	e3500001 	cmp	r0, #1	; 0x1
     a78:	13a00001 	movne	r0, #1	; 0x1
     a7c:	112fff1e 	bxne	lr
     a80:	e3a00000 	mov	r0, #0	; 0x0
     a84:	e12fff1e 	bx	lr

00000a88 <ecfg_paras>:
     a88:	e59f0000 	ldr	r0, [pc, #0]	; a90 <ecfg_paras+0x8>
     a8c:	e12fff1e 	bx	lr
     a90:	3c00d698 	.word	0x3c00d698

00000a94 <efuse_mac>:
     a94:	e3a00000 	mov	r0, #0	; 0x0
     a98:	e3a01000 	mov	r1, #0	; 0x0
     a9c:	e12fff1e 	bx	lr

00000aa0 <efuse_info>:
     aa0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     aa4:	e1a04000 	mov	r4, r0
     aa8:	e24dd004 	sub	sp, sp, #4	; 0x4
     aac:	e59f6144 	ldr	r6, [pc, #324]	; bf8 <efuse_info+0x158>
     ab0:	e59f0144 	ldr	r0, [pc, #324]	; bfc <efuse_info+0x15c>
     ab4:	e1a0e00f 	mov	lr, pc
     ab8:	e596f000 	ldr	pc, [r6]
     abc:	e59f013c 	ldr	r0, [pc, #316]	; c00 <efuse_info+0x160>
     ac0:	e1a0e00f 	mov	lr, pc
     ac4:	e596f000 	ldr	pc, [r6]
     ac8:	e3540000 	cmp	r4, #0	; 0x0
     acc:	0a00003c 	beq	bc4 <efuse_info+0x124>
     ad0:	e59f412c 	ldr	r4, [pc, #300]	; c04 <efuse_info+0x164>
     ad4:	e59f012c 	ldr	r0, [pc, #300]	; c08 <efuse_info+0x168>
     ad8:	e5941000 	ldr	r1, [r4]
     adc:	e1a0e00f 	mov	lr, pc
     ae0:	e596f000 	ldr	pc, [r6]
     ae4:	e5d41004 	ldrb	r1, [r4, #4]
     ae8:	e59f011c 	ldr	r0, [pc, #284]	; c0c <efuse_info+0x16c>
     aec:	e1a0e00f 	mov	lr, pc
     af0:	e596f000 	ldr	pc, [r6]
     af4:	e5d41005 	ldrb	r1, [r4, #5]
     af8:	e59f0110 	ldr	r0, [pc, #272]	; c10 <efuse_info+0x170>
     afc:	e1a0e00f 	mov	lr, pc
     b00:	e596f000 	ldr	pc, [r6]
     b04:	e5d41006 	ldrb	r1, [r4, #6]
     b08:	e59f0104 	ldr	r0, [pc, #260]	; c14 <efuse_info+0x174>
     b0c:	e1a0e00f 	mov	lr, pc
     b10:	e596f000 	ldr	pc, [r6]
     b14:	e5d41007 	ldrb	r1, [r4, #7]
     b18:	e59f00f8 	ldr	r0, [pc, #248]	; c18 <efuse_info+0x178>
     b1c:	e1a0e00f 	mov	lr, pc
     b20:	e596f000 	ldr	pc, [r6]
     b24:	e1d410b8 	ldrh	r1, [r4, #8]
     b28:	e59f00ec 	ldr	r0, [pc, #236]	; c1c <efuse_info+0x17c>
     b2c:	e1a0e00f 	mov	lr, pc
     b30:	e596f000 	ldr	pc, [r6]
     b34:	e1d410ba 	ldrh	r1, [r4, #10]
     b38:	e59f00e0 	ldr	r0, [pc, #224]	; c20 <efuse_info+0x180>
     b3c:	e1a0e00f 	mov	lr, pc
     b40:	e596f000 	ldr	pc, [r6]
     b44:	e1d410bc 	ldrh	r1, [r4, #12]
     b48:	e59f00d4 	ldr	r0, [pc, #212]	; c24 <efuse_info+0x184>
     b4c:	e1a0e00f 	mov	lr, pc
     b50:	e596f000 	ldr	pc, [r6]
     b54:	e1d410be 	ldrh	r1, [r4, #14]
     b58:	e59f00c8 	ldr	r0, [pc, #200]	; c28 <efuse_info+0x188>
     b5c:	e1a0e00f 	mov	lr, pc
     b60:	e596f000 	ldr	pc, [r6]
     b64:	e1a05004 	mov	r5, r4
     b68:	e3a04000 	mov	r4, #0	; 0x0
     b6c:	e1a01004 	mov	r1, r4
     b70:	e5952010 	ldr	r2, [r5, #16]
     b74:	e2844001 	add	r4, r4, #1	; 0x1
     b78:	e59f00ac 	ldr	r0, [pc, #172]	; c2c <efuse_info+0x18c>
     b7c:	e1a0e00f 	mov	lr, pc
     b80:	e596f000 	ldr	pc, [r6]
     b84:	e3540004 	cmp	r4, #4	; 0x4
     b88:	e2855004 	add	r5, r5, #4	; 0x4
     b8c:	1afffff6 	bne	b6c <efuse_info+0xcc>
     b90:	e59f6060 	ldr	r6, [pc, #96]	; bf8 <efuse_info+0x158>
     b94:	e59f5068 	ldr	r5, [pc, #104]	; c04 <efuse_info+0x164>
     b98:	e284400c 	add	r4, r4, #12	; 0xc
     b9c:	e1a03084 	lsl	r3, r4, #1
     ba0:	e1a01004 	mov	r1, r4
     ba4:	e19520b3 	ldrh	r2, [r5, r3]
     ba8:	e2844001 	add	r4, r4, #1	; 0x1
     bac:	e59f007c 	ldr	r0, [pc, #124]	; c30 <efuse_info+0x190>
     bb0:	e1a0e00f 	mov	lr, pc
     bb4:	e596f000 	ldr	pc, [r6]
     bb8:	e354001c 	cmp	r4, #28	; 0x1c
     bbc:	1afffff6 	bne	b9c <efuse_info+0xfc>
     bc0:	ea00000a 	b	bf0 <efuse_info+0x150>
     bc4:	e59f4068 	ldr	r4, [pc, #104]	; c34 <efuse_info+0x194>
     bc8:	e59f7068 	ldr	r7, [pc, #104]	; c38 <efuse_info+0x198>
     bcc:	e1a05006 	mov	r5, r6
     bd0:	e5942000 	ldr	r2, [r4]
     bd4:	e1a01004 	mov	r1, r4
     bd8:	e59f005c 	ldr	r0, [pc, #92]	; c3c <efuse_info+0x19c>
     bdc:	e2844004 	add	r4, r4, #4	; 0x4
     be0:	e1a0e00f 	mov	lr, pc
     be4:	e595f000 	ldr	pc, [r5]
     be8:	e1540007 	cmp	r4, r7
     bec:	1afffff7 	bne	bd0 <efuse_info+0x130>
     bf0:	e28dd004 	add	sp, sp, #4	; 0x4
     bf4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     bf8:	3c00d694 	.word	0x3c00d694
     bfc:	00019e09 	.word	0x00019e09
     c00:	00019e1e 	.word	0x00019e1e
     c04:	3c00d698 	.word	0x3c00d698
     c08:	00019e35 	.word	0x00019e35
     c0c:	00019e44 	.word	0x00019e44
     c10:	00019e53 	.word	0x00019e53
     c14:	00019e66 	.word	0x00019e66
     c18:	00019e75 	.word	0x00019e75
     c1c:	00019e8a 	.word	0x00019e8a
     c20:	00019e97 	.word	0x00019e97
     c24:	00019ea7 	.word	0x00019ea7
     c28:	00019eb8 	.word	0x00019eb8
     c2c:	00019eca 	.word	0x00019eca
     c30:	00019eda 	.word	0x00019eda
     c34:	21e0d9b4 	.word	0x21e0d9b4
     c38:	21e0da94 	.word	0x21e0da94
     c3c:	00019eeb 	.word	0x00019eeb

00000c40 <init_efuse>:
     c40:	e59f0008 	ldr	r0, [pc, #8]	; c50 <init_efuse+0x10>
     c44:	e3a01000 	mov	r1, #0	; 0x0
     c48:	e3a02038 	mov	r2, #56	; 0x38
     c4c:	ea00025f 	b	15d0 <memset>
     c50:	3c00d698 	.word	0x3c00d698

00000c54 <assert_linkpc>:
     c54:	e3a00001 	mov	r0, #1	; 0x1
     c58:	e12fff1e 	bx	lr

00000c5c <pads_states>:
     c5c:	e3500000 	cmp	r0, #0	; 0x0
     c60:	e2803007 	add	r3, r0, #7	; 0x7
     c64:	a1a03000 	movge	r3, r0
     c68:	e1a031c3 	asr	r3, r3, #3
     c6c:	e59f203c 	ldr	r2, [pc, #60]	; cb0 <pads_states+0x54>
     c70:	e1a03103 	lsl	r3, r3, #2
     c74:	e0832002 	add	r2, r3, r2
     c78:	e5921000 	ldr	r1, [r2]
     c7c:	e3a03001 	mov	r3, #1	; 0x1
     c80:	e2002007 	and	r2, r0, #7	; 0x7
     c84:	e0113213 	ands	r3, r1, r3, lsl r2
     c88:	01a00003 	moveq	r0, r3
     c8c:	012fff1e 	bxeq	lr
     c90:	e1a03300 	lsl	r3, r0, #6
     c94:	e2833282 	add	r3, r3, #536870920	; 0x20000008
     c98:	e283360f 	add	r3, r3, #15728640	; 0xf00000
     c9c:	e5933000 	ldr	r3, [r3]
     ca0:	e3530000 	cmp	r3, #0	; 0x0
     ca4:	13a00002 	movne	r0, #2	; 0x2
     ca8:	03a00001 	moveq	r0, #1	; 0x1
     cac:	e12fff1e 	bx	lr
     cb0:	21e09064 	.word	0x21e09064

00000cb4 <pads_chmod>:
     cb4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     cb8:	e1b08840 	asrs	r8, r0, #16
     cbc:	e1a00800 	lsl	r0, r0, #16
     cc0:	03a03000 	moveq	r3, #0	; 0x0
     cc4:	13a03001 	movne	r3, #1	; 0x1
     cc8:	e1a00820 	lsr	r0, r0, #16
     ccc:	e1580000 	cmp	r8, r0
     cd0:	a3a0c000 	movge	ip, #0	; 0x0
     cd4:	b203c001 	andlt	ip, r3, #1	; 0x1
     cd8:	e35c0000 	cmp	ip, #0	; 0x0
     cdc:	e24dd004 	sub	sp, sp, #4	; 0x4
     ce0:	e1a06001 	mov	r6, r1
     ce4:	e1a0b002 	mov	fp, r2
     ce8:	1a000040 	bne	df0 <pads_chmod+0x13c>
     cec:	e2803883 	add	r3, r0, #8585216	; 0x830000
     cf0:	e2833903 	add	r3, r3, #49152	; 0xc000
     cf4:	e1a02303 	lsl	r2, r3, #6
     cf8:	e3580000 	cmp	r8, #0	; 0x0
     cfc:	01a08000 	moveq	r8, r0
     d00:	e2827008 	add	r7, r2, #8	; 0x8
     d04:	e282a004 	add	sl, r2, #4	; 0x4
     d08:	e1a04000 	mov	r4, r0
     d0c:	e240501f 	sub	r5, r0, #31	; 0x1f
     d10:	e3a09001 	mov	r9, #1	; 0x1
     d14:	ea000033 	b	de8 <pads_chmod+0x134>
     d18:	e3540000 	cmp	r4, #0	; 0x0
     d1c:	e2843007 	add	r3, r4, #7	; 0x7
     d20:	a1a03004 	movge	r3, r4
     d24:	e3550006 	cmp	r5, #6	; 0x6
     d28:	959f20c8 	ldrls	r2, [pc, #200]	; df8 <pads_chmod+0x144>
     d2c:	e1a011c3 	asr	r1, r3, #3
     d30:	95923008 	ldrls	r3, [r2, #8]
     d34:	e2040007 	and	r0, r4, #7	; 0x7
     d38:	91833519 	orrls	r3, r3, r9, lsl r5
     d3c:	95823008 	strls	r3, [r2, #8]
     d40:	e3560001 	cmp	r6, #1	; 0x1
     d44:	0a00000a 	beq	d74 <pads_chmod+0xc0>
     d48:	e3560002 	cmp	r6, #2	; 0x2
     d4c:	0a000011 	beq	d98 <pads_chmod+0xe4>
     d50:	e3560000 	cmp	r6, #0	; 0x0
     d54:	1a00001a 	bne	dc4 <pads_chmod+0x110>
     d58:	e59f309c 	ldr	r3, [pc, #156]	; dfc <pads_chmod+0x148>
     d5c:	e1a02101 	lsl	r2, r1, #2
     d60:	e0823003 	add	r3, r2, r3
     d64:	e5932000 	ldr	r2, [r3]
     d68:	e1c22019 	bic	r2, r2, r9, lsl r0
     d6c:	e5832000 	str	r2, [r3]
     d70:	ea000018 	b	dd8 <pads_chmod+0x124>
     d74:	e59f3080 	ldr	r3, [pc, #128]	; dfc <pads_chmod+0x148>
     d78:	e1a02101 	lsl	r2, r1, #2
     d7c:	e0823003 	add	r3, r2, r3
     d80:	e5932000 	ldr	r2, [r3]
     d84:	e1822016 	orr	r2, r2, r6, lsl r0
     d88:	e5832000 	str	r2, [r3]
     d8c:	e3a03000 	mov	r3, #0	; 0x0
     d90:	e5873000 	str	r3, [r7]
     d94:	ea00000f 	b	dd8 <pads_chmod+0x124>
     d98:	e59f205c 	ldr	r2, [pc, #92]	; dfc <pads_chmod+0x148>
     d9c:	e1a03101 	lsl	r3, r1, #2
     da0:	e0832002 	add	r2, r3, r2
     da4:	e5923000 	ldr	r3, [r2]
     da8:	e25b1000 	subs	r1, fp, #0	; 0x0
     dac:	13a01001 	movne	r1, #1	; 0x1
     db0:	e1833019 	orr	r3, r3, r9, lsl r0
     db4:	e5823000 	str	r3, [r2]
     db8:	e5879000 	str	r9, [r7]
     dbc:	e58a1000 	str	r1, [sl]
     dc0:	ea000004 	b	dd8 <pads_chmod+0x124>
     dc4:	e59f0034 	ldr	r0, [pc, #52]	; e00 <pads_chmod+0x14c>
     dc8:	e1a01006 	mov	r1, r6
     dcc:	e59f2030 	ldr	r2, [pc, #48]	; e04 <pads_chmod+0x150>
     dd0:	e1a0e00f 	mov	lr, pc
     dd4:	e592f000 	ldr	pc, [r2]
     dd8:	e2844001 	add	r4, r4, #1	; 0x1
     ddc:	e2877040 	add	r7, r7, #64	; 0x40
     de0:	e28aa040 	add	sl, sl, #64	; 0x40
     de4:	e2855001 	add	r5, r5, #1	; 0x1
     de8:	e1540008 	cmp	r4, r8
     dec:	daffffc9 	ble	d18 <pads_chmod+0x64>
     df0:	e28dd004 	add	sp, sp, #4	; 0x4
     df4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     df8:	21e09000 	.word	0x21e09000
     dfc:	21e09064 	.word	0x21e09064
     e00:	00019efb 	.word	0x00019efb
     e04:	3c00d694 	.word	0x3c00d694

00000e08 <pads_pull>:
     e08:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
     e0c:	e1b06840 	asrs	r6, r0, #16
     e10:	e1a00800 	lsl	r0, r0, #16
     e14:	03a03000 	moveq	r3, #0	; 0x0
     e18:	13a03001 	movne	r3, #1	; 0x1
     e1c:	e1a00820 	lsr	r0, r0, #16
     e20:	e1560000 	cmp	r6, r0
     e24:	a3a03000 	movge	r3, #0	; 0x0
     e28:	b2033001 	andlt	r3, r3, #1	; 0x1
     e2c:	e3530000 	cmp	r3, #0	; 0x0
     e30:	e1a09001 	mov	r9, r1
     e34:	e1a0a002 	mov	sl, r2
     e38:	18bd87f0 	popne	{r4, r5, r6, r7, r8, r9, sl, pc}
     e3c:	e3560000 	cmp	r6, #0	; 0x0
     e40:	01a06000 	moveq	r6, r0
     e44:	e1a0c000 	mov	ip, r0
     e48:	e1a07003 	mov	r7, r3
     e4c:	e240501f 	sub	r5, r0, #31	; 0x1f
     e50:	e3a08001 	mov	r8, #1	; 0x1
     e54:	ea000020 	b	edc <pads_pull+0xd4>
     e58:	e35c0000 	cmp	ip, #0	; 0x0
     e5c:	e28c3007 	add	r3, ip, #7	; 0x7
     e60:	a1a0300c 	movge	r3, ip
     e64:	e1a031c3 	asr	r3, r3, #3
     e68:	e3590000 	cmp	r9, #0	; 0x0
     e6c:	e1a00103 	lsl	r0, r3, #2
     e70:	e20c4007 	and	r4, ip, #7	; 0x7
     e74:	059f206c 	ldreq	r2, [pc, #108]	; ee8 <pads_pull+0xe0>
     e78:	00802002 	addeq	r2, r0, r2
     e7c:	05923000 	ldreq	r3, [r2]
     e80:	01c33418 	biceq	r3, r3, r8, lsl r4
     e84:	0a000010 	beq	ecc <pads_pull+0xc4>
     e88:	e3550006 	cmp	r5, #6	; 0x6
     e8c:	859f2058 	ldrhi	r2, [pc, #88]	; eec <pads_pull+0xe4>
     e90:	91a03005 	movls	r3, r5
     e94:	81a03004 	movhi	r3, r4
     e98:	959f2050 	ldrls	r2, [pc, #80]	; ef0 <pads_pull+0xe8>
     e9c:	80802002 	addhi	r2, r0, r2
     ea0:	e01a1718 	ands	r1, sl, r8, lsl r7
     ea4:	e1a01318 	lsl	r1, r8, r3
     ea8:	15923000 	ldrne	r3, [r2]
     eac:	05923000 	ldreq	r3, [r2]
     eb0:	11813003 	orrne	r3, r1, r3
     eb4:	01c33001 	biceq	r3, r3, r1
     eb8:	e5823000 	str	r3, [r2]
     ebc:	e59f2024 	ldr	r2, [pc, #36]	; ee8 <pads_pull+0xe0>
     ec0:	e0802002 	add	r2, r0, r2
     ec4:	e5923000 	ldr	r3, [r2]
     ec8:	e1833418 	orr	r3, r3, r8, lsl r4
     ecc:	e5823000 	str	r3, [r2]
     ed0:	e28cc001 	add	ip, ip, #1	; 0x1
     ed4:	e2877001 	add	r7, r7, #1	; 0x1
     ed8:	e2855001 	add	r5, r5, #1	; 0x1
     edc:	e15c0006 	cmp	ip, r6
     ee0:	daffffdc 	ble	e58 <pads_pull+0x50>
     ee4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
     ee8:	21e09014 	.word	0x21e09014
     eec:	21e091a4 	.word	0x21e091a4
     ef0:	21e0900c 	.word	0x21e0900c

00000ef4 <pads_setpin>:
     ef4:	e1b0c840 	asrs	ip, r0, #16
     ef8:	e1a00800 	lsl	r0, r0, #16
     efc:	03a03000 	moveq	r3, #0	; 0x0
     f00:	13a03001 	movne	r3, #1	; 0x1
     f04:	e1a00820 	lsr	r0, r0, #16
     f08:	e15c0000 	cmp	ip, r0
     f0c:	a3a03000 	movge	r3, #0	; 0x0
     f10:	b2033001 	andlt	r3, r3, #1	; 0x1
     f14:	e3530000 	cmp	r3, #0	; 0x0
     f18:	112fff1e 	bxne	lr
     f1c:	e1a03300 	lsl	r3, r0, #6
     f20:	e2832242 	add	r2, r3, #536870916	; 0x20000004
     f24:	e35c0000 	cmp	ip, #0	; 0x0
     f28:	01a0c000 	moveq	ip, r0
     f2c:	e282260f 	add	r2, r2, #15728640	; 0xf00000
     f30:	ea000002 	b	f40 <pads_setpin+0x4c>
     f34:	e2513000 	subs	r3, r1, #0	; 0x0
     f38:	13a03001 	movne	r3, #1	; 0x1
     f3c:	e4823040 	str	r3, [r2], #64
     f40:	e150000c 	cmp	r0, ip
     f44:	e2800001 	add	r0, r0, #1	; 0x1
     f48:	dafffff9 	ble	f34 <pads_setpin+0x40>
     f4c:	e12fff1e 	bx	lr

00000f50 <pads_getpin>:
     f50:	e59f3014 	ldr	r3, [pc, #20]	; f6c <pads_getpin+0x1c>
     f54:	e6f30070 	uxtah	r0, r3, r0
     f58:	e1a00300 	lsl	r0, r0, #6
     f5c:	e5900000 	ldr	r0, [r0]
     f60:	e2500000 	subs	r0, r0, #0	; 0x0
     f64:	13a00001 	movne	r0, #1	; 0x1
     f68:	e12fff1e 	bx	lr
     f6c:	0083c000 	.word	0x0083c000

00000f70 <get_boot_pin_value>:
     f70:	e59f301c 	ldr	r3, [pc, #28]	; f94 <get_boot_pin_value+0x24>
     f74:	e5930f40 	ldr	r0, [r3, #3904]
     f78:	e5933f00 	ldr	r3, [r3, #3840]
     f7c:	e3500000 	cmp	r0, #0	; 0x0
     f80:	13a00002 	movne	r0, #2	; 0x2
     f84:	03a00000 	moveq	r0, #0	; 0x0
     f88:	e3530000 	cmp	r3, #0	; 0x0
     f8c:	13800001 	orrne	r0, r0, #1	; 0x1
     f90:	e12fff1e 	bx	lr
     f94:	20f01000 	.word	0x20f01000

00000f98 <init_alternate>:
     f98:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     f9c:	e59f006c 	ldr	r0, [pc, #108]	; 1010 <init_alternate+0x78>
     fa0:	e24dd004 	sub	sp, sp, #4	; 0x4
     fa4:	ebfffe02 	bl	7b4 <module_enable>
     fa8:	e59f0064 	ldr	r0, [pc, #100]	; 1014 <init_alternate+0x7c>
     fac:	e3a01001 	mov	r1, #1	; 0x1
     fb0:	e3a02000 	mov	r2, #0	; 0x0
     fb4:	ebffff3e 	bl	cb4 <pads_chmod>
     fb8:	e3a01000 	mov	r1, #0	; 0x0
     fbc:	e1a02001 	mov	r2, r1
     fc0:	e59f004c 	ldr	r0, [pc, #76]	; 1014 <init_alternate+0x7c>
     fc4:	ebffff8f 	bl	e08 <pads_pull>
     fc8:	e3a00025 	mov	r0, #37	; 0x25
     fcc:	e3a01001 	mov	r1, #1	; 0x1
     fd0:	e3a02000 	mov	r2, #0	; 0x0
     fd4:	ebffff36 	bl	cb4 <pads_chmod>
     fd8:	e59f3038 	ldr	r3, [pc, #56]	; 1018 <init_alternate+0x80>
     fdc:	e5933940 	ldr	r3, [r3, #2368]
     fe0:	e3530000 	cmp	r3, #0	; 0x0
     fe4:	0a000006 	beq	1004 <init_alternate+0x6c>
     fe8:	e59f302c 	ldr	r3, [pc, #44]	; 101c <init_alternate+0x84>
     fec:	e3a0c000 	mov	ip, #0	; 0x0
     ff0:	e1a0100c 	mov	r1, ip
     ff4:	e59f0024 	ldr	r0, [pc, #36]	; 1020 <init_alternate+0x88>
     ff8:	e1a0200c 	mov	r2, ip
     ffc:	e583c3e0 	str	ip, [r3, #992]
    1000:	ebffff2b 	bl	cb4 <pads_chmod>
    1004:	e3a00000 	mov	r0, #0	; 0x0
    1008:	e28dd004 	add	sp, sp, #4	; 0x4
    100c:	e8bd8000 	pop	{pc}
    1010:	21e13c00 	.word	0x21e13c00
    1014:	007d007c 	.word	0x007d007c
    1018:	20f00000 	.word	0x20f00000
    101c:	21e09000 	.word	0x21e09000
    1020:	0024001f 	.word	0x0024001f

00001024 <cpu_recase>:
    1024:	e92d4010 	push	{r4, lr}
    1028:	e59f10a8 	ldr	r1, [pc, #168]	; 10d8 <cpu_recase+0xb4>
    102c:	e3a02003 	mov	r2, #3	; 0x3
    1030:	e1a04000 	mov	r4, r0
    1034:	eb000134 	bl	150c <strncmp>
    1038:	e3500000 	cmp	r0, #0	; 0x0
    103c:	0a00000b 	beq	1070 <cpu_recase+0x4c>
    1040:	e1a00004 	mov	r0, r4
    1044:	e59f1090 	ldr	r1, [pc, #144]	; 10dc <cpu_recase+0xb8>
    1048:	e3a02003 	mov	r2, #3	; 0x3
    104c:	eb00012e 	bl	150c <strncmp>
    1050:	e3500000 	cmp	r0, #0	; 0x0
    1054:	0a000005 	beq	1070 <cpu_recase+0x4c>
    1058:	e1a00004 	mov	r0, r4
    105c:	e59f107c 	ldr	r1, [pc, #124]	; 10e0 <cpu_recase+0xbc>
    1060:	e3a02003 	mov	r2, #3	; 0x3
    1064:	eb000128 	bl	150c <strncmp>
    1068:	e3500000 	cmp	r0, #0	; 0x0
    106c:	1a000001 	bne	1078 <cpu_recase+0x54>
    1070:	e3a02002 	mov	r2, #2	; 0x2
    1074:	ea000006 	b	1094 <cpu_recase+0x70>
    1078:	e1a00004 	mov	r0, r4
    107c:	e59f1060 	ldr	r1, [pc, #96]	; 10e4 <cpu_recase+0xc0>
    1080:	e3a02004 	mov	r2, #4	; 0x4
    1084:	eb000120 	bl	150c <strncmp>
    1088:	e3500000 	cmp	r0, #0	; 0x0
    108c:	18bd8010 	popne	{r4, pc}
    1090:	e3a02001 	mov	r2, #1	; 0x1
    1094:	e59f304c 	ldr	r3, [pc, #76]	; 10e8 <cpu_recase+0xc4>
    1098:	e59f4048 	ldr	r4, [pc, #72]	; 10e8 <cpu_recase+0xc4>
    109c:	e59f1048 	ldr	r1, [pc, #72]	; 10ec <cpu_recase+0xc8>
    10a0:	e5832000 	str	r2, [r3]
    10a4:	e5942000 	ldr	r2, [r4]
    10a8:	e5913000 	ldr	r3, [r1]
    10ac:	e1520003 	cmp	r2, r3
    10b0:	08bd8010 	popeq	{r4, pc}
    10b4:	e1a00002 	mov	r0, r2
    10b8:	e5812000 	str	r2, [r1]
    10bc:	eb0000d6 	bl	141c <set_mux>
    10c0:	e5941000 	ldr	r1, [r4]
    10c4:	e59f0024 	ldr	r0, [pc, #36]	; 10f0 <cpu_recase+0xcc>
    10c8:	e59f3024 	ldr	r3, [pc, #36]	; 10f4 <cpu_recase+0xd0>
    10cc:	e1a0e00f 	mov	lr, pc
    10d0:	e593f000 	ldr	pc, [r3]
    10d4:	e8bd8010 	pop	{r4, pc}
    10d8:	00019da3 	.word	0x00019da3
    10dc:	00019f12 	.word	0x00019f12
    10e0:	00019f16 	.word	0x00019f16
    10e4:	00019f1a 	.word	0x00019f1a
    10e8:	3c00d130 	.word	0x3c00d130
    10ec:	3c00d090 	.word	0x3c00d090
    10f0:	00019f1f 	.word	0x00019f1f
    10f4:	3c00d694 	.word	0x3c00d694

000010f8 <set_clk>:
    10f8:	e92d4030 	push	{r4, r5, lr}
    10fc:	e1d010b2 	ldrh	r1, [r0, #2]
    1100:	e24dd004 	sub	sp, sp, #4	; 0x4
    1104:	e3510000 	cmp	r1, #0	; 0x0
    1108:	e1a04000 	mov	r4, r0
    110c:	0a000001 	beq	1118 <set_clk+0x20>
    1110:	e3a00001 	mov	r0, #1	; 0x1
    1114:	ebfffd5d 	bl	690 <set_pll>
    1118:	e1d410b4 	ldrh	r1, [r4, #4]
    111c:	e3510000 	cmp	r1, #0	; 0x0
    1120:	0a000001 	beq	112c <set_clk+0x34>
    1124:	e3a00002 	mov	r0, #2	; 0x2
    1128:	ebfffd58 	bl	690 <set_pll>
    112c:	e1d410b6 	ldrh	r1, [r4, #6]
    1130:	e3510000 	cmp	r1, #0	; 0x0
    1134:	0a000001 	beq	1140 <set_clk+0x48>
    1138:	e3a00003 	mov	r0, #3	; 0x3
    113c:	ebfffd53 	bl	690 <set_pll>
    1140:	e59f00c4 	ldr	r0, [pc, #196]	; 120c <set_clk+0x114>
    1144:	e3a01000 	mov	r1, #0	; 0x0
    1148:	e3a02003 	mov	r2, #3	; 0x3
    114c:	ebfffd8d 	bl	788 <module_set_clock>
    1150:	e59f00b8 	ldr	r0, [pc, #184]	; 1210 <set_clk+0x118>
    1154:	e3a01001 	mov	r1, #1	; 0x1
    1158:	e3a0200b 	mov	r2, #11	; 0xb
    115c:	ebfffd89 	bl	788 <module_set_clock>
    1160:	e59f00ac 	ldr	r0, [pc, #172]	; 1214 <set_clk+0x11c>
    1164:	e3a01001 	mov	r1, #1	; 0x1
    1168:	e3a02005 	mov	r2, #5	; 0x5
    116c:	ebfffd85 	bl	788 <module_set_clock>
    1170:	e59f00a0 	ldr	r0, [pc, #160]	; 1218 <set_clk+0x120>
    1174:	e3a01001 	mov	r1, #1	; 0x1
    1178:	e3a02013 	mov	r2, #19	; 0x13
    117c:	ebfffd81 	bl	788 <module_set_clock>
    1180:	e59f0094 	ldr	r0, [pc, #148]	; 121c <set_clk+0x124>
    1184:	e3a01002 	mov	r1, #2	; 0x2
    1188:	e3a0200f 	mov	r2, #15	; 0xf
    118c:	ebfffd7d 	bl	788 <module_set_clock>
    1190:	e59f0088 	ldr	r0, [pc, #136]	; 1220 <set_clk+0x128>
    1194:	e3a01002 	mov	r1, #2	; 0x2
    1198:	e3a02005 	mov	r2, #5	; 0x5
    119c:	ebfffd79 	bl	788 <module_set_clock>
    11a0:	e59f007c 	ldr	r0, [pc, #124]	; 1224 <set_clk+0x12c>
    11a4:	e3a01002 	mov	r1, #2	; 0x2
    11a8:	e3a0200b 	mov	r2, #11	; 0xb
    11ac:	ebfffd75 	bl	788 <module_set_clock>
    11b0:	e59f0070 	ldr	r0, [pc, #112]	; 1228 <set_clk+0x130>
    11b4:	e3a01002 	mov	r1, #2	; 0x2
    11b8:	e3a02019 	mov	r2, #25	; 0x19
    11bc:	ebfffd71 	bl	788 <module_set_clock>
    11c0:	e59f4064 	ldr	r4, [pc, #100]	; 122c <set_clk+0x134>
    11c4:	e59f5040 	ldr	r5, [pc, #64]	; 120c <set_clk+0x114>
    11c8:	e3a01003 	mov	r1, #3	; 0x3
    11cc:	e1a00004 	mov	r0, r4
    11d0:	e1a02001 	mov	r2, r1
    11d4:	e2844010 	add	r4, r4, #16	; 0x10
    11d8:	ebfffd6a 	bl	788 <module_set_clock>
    11dc:	e1540005 	cmp	r4, r5
    11e0:	1afffff8 	bne	11c8 <set_clk+0xd0>
    11e4:	e59f0044 	ldr	r0, [pc, #68]	; 1230 <set_clk+0x138>
    11e8:	e3a01003 	mov	r1, #3	; 0x3
    11ec:	e3a02013 	mov	r2, #19	; 0x13
    11f0:	ebfffd64 	bl	788 <module_set_clock>
    11f4:	e59f0038 	ldr	r0, [pc, #56]	; 1234 <set_clk+0x13c>
    11f8:	e3a01000 	mov	r1, #0	; 0x0
    11fc:	e3a02005 	mov	r2, #5	; 0x5
    1200:	e28dd004 	add	sp, sp, #4	; 0x4
    1204:	e8bd4030 	pop	{r4, r5, lr}
    1208:	eafffd5e 	b	788 <module_set_clock>
    120c:	21e0a190 	.word	0x21e0a190
    1210:	21e0a250 	.word	0x21e0a250
    1214:	21e0a270 	.word	0x21e0a270
    1218:	21e0a2c0 	.word	0x21e0a2c0
    121c:	21e0a240 	.word	0x21e0a240
    1220:	21e0a2f0 	.word	0x21e0a2f0
    1224:	21e0a2e0 	.word	0x21e0a2e0
    1228:	21e0a290 	.word	0x21e0a290
    122c:	21e0a130 	.word	0x21e0a130
    1230:	21e0a260 	.word	0x21e0a260
    1234:	21e0a360 	.word	0x21e0a360

00001238 <chip_test>:
    1238:	e59f0044 	ldr	r0, [pc, #68]	; 1284 <end+0x4>
    123c:	e5901000 	ldr	r1, [r0]
    1240:	e59f2040 	ldr	r2, [pc, #64]	; 1288 <end+0x8>
    1244:	e1520001 	cmp	r2, r1
    1248:	1a00000c 	bne	1280 <end>
    124c:	e5901004 	ldr	r1, [r0, #4]
    1250:	e59f2034 	ldr	r2, [pc, #52]	; 128c <end+0xc>
    1254:	e1520001 	cmp	r2, r1
    1258:	1a000008 	bne	1280 <end>
    125c:	e5901008 	ldr	r1, [r0, #8]
    1260:	e59f2028 	ldr	r2, [pc, #40]	; 1290 <end+0x10>
    1264:	e1520001 	cmp	r2, r1
    1268:	1a000004 	bne	1280 <end>
    126c:	e590100c 	ldr	r1, [r0, #12]
    1270:	e59f201c 	ldr	r2, [pc, #28]	; 1294 <end+0x14>
    1274:	e1520001 	cmp	r2, r1
    1278:	1a000000 	bne	1280 <end>
    127c:	e3a0f30f 	mov	pc, #1006632960	; 0x3c000000

00001280 <end>:
    1280:	e1a0f00e 	.word	0xe1a0f00e
    1284:	3c007ff0 	.word	0x3c007ff0
    1288:	6f666e69 	.word	0x6f666e69
    128c:	612d6d74 	.word	0x612d6d74
    1290:	2d6e696c 	.word	0x2d6e696c
    1294:	796c6e6f 	.word	0x796c6e6f

00001298 <module_get_clock>:
    1298:	e59f3134 	ldr	r3, [pc, #308]	; 13d4 <module_get_clock+0x13c>
    129c:	e1500003 	cmp	r0, r3
    12a0:	0a000041 	beq	13ac <module_get_clock+0x114>
    12a4:	8a000017 	bhi	1308 <module_get_clock+0x70>
    12a8:	e24330f0 	sub	r3, r3, #240	; 0xf0
    12ac:	e1500003 	cmp	r0, r3
    12b0:	0a000039 	beq	139c <module_get_clock+0x104>
    12b4:	8a000007 	bhi	12d8 <module_get_clock+0x40>
    12b8:	e3500003 	cmp	r0, #3	; 0x3
    12bc:	059f0114 	ldreq	r0, [pc, #276]	; 13d8 <module_get_clock+0x140>
    12c0:	012fff1e 	bxeq	lr
    12c4:	859f3110 	ldrhi	r3, [pc, #272]	; 13dc <module_get_clock+0x144>
    12c8:	8a000008 	bhi	12f0 <module_get_clock+0x58>
    12cc:	e3500001 	cmp	r0, #1	; 0x1
    12d0:	1a000033 	bne	13a4 <module_get_clock+0x10c>
    12d4:	ea00003c 	b	13cc <module_get_clock+0x134>
    12d8:	e59f3100 	ldr	r3, [pc, #256]	; 13e0 <module_get_clock+0x148>
    12dc:	e1500003 	cmp	r0, r3
    12e0:	0a00002d 	beq	139c <module_get_clock+0x104>
    12e4:	92433010 	subls	r3, r3, #16	; 0x10
    12e8:	9a000003 	bls	12fc <module_get_clock+0x64>
    12ec:	e59f30f0 	ldr	r3, [pc, #240]	; 13e4 <module_get_clock+0x14c>
    12f0:	e1500003 	cmp	r0, r3
    12f4:	0a000028 	beq	139c <module_get_clock+0x104>
    12f8:	e2833010 	add	r3, r3, #16	; 0x10
    12fc:	e1500003 	cmp	r0, r3
    1300:	1a000027 	bne	13a4 <module_get_clock+0x10c>
    1304:	ea000024 	b	139c <module_get_clock+0x104>
    1308:	e59f30d8 	ldr	r3, [pc, #216]	; 13e8 <module_get_clock+0x150>
    130c:	e1500003 	cmp	r0, r3
    1310:	059f00d4 	ldreq	r0, [pc, #212]	; 13ec <module_get_clock+0x154>
    1314:	012fff1e 	bxeq	lr
    1318:	8a00000f 	bhi	135c <module_get_clock+0xc4>
    131c:	e2433060 	sub	r3, r3, #96	; 0x60
    1320:	e1500003 	cmp	r0, r3
    1324:	0a000020 	beq	13ac <module_get_clock+0x114>
    1328:	8a000003 	bhi	133c <module_get_clock+0xa4>
    132c:	e2433010 	sub	r3, r3, #16	; 0x10
    1330:	e1500003 	cmp	r0, r3
    1334:	1a00001a 	bne	13a4 <module_get_clock+0x10c>
    1338:	ea00001b 	b	13ac <module_get_clock+0x114>
    133c:	e59f30ac 	ldr	r3, [pc, #172]	; 13f0 <module_get_clock+0x158>
    1340:	e1500003 	cmp	r0, r3
    1344:	059f00a8 	ldreq	r0, [pc, #168]	; 13f4 <module_get_clock+0x15c>
    1348:	012fff1e 	bxeq	lr
    134c:	e2833020 	add	r3, r3, #32	; 0x20
    1350:	e1500003 	cmp	r0, r3
    1354:	1a000012 	bne	13a4 <module_get_clock+0x10c>
    1358:	ea000015 	b	13b4 <module_get_clock+0x11c>
    135c:	e59f3094 	ldr	r3, [pc, #148]	; 13f8 <module_get_clock+0x160>
    1360:	e1500003 	cmp	r0, r3
    1364:	059f0090 	ldreq	r0, [pc, #144]	; 13fc <module_get_clock+0x164>
    1368:	012fff1e 	bxeq	lr
    136c:	8a000003 	bhi	1380 <module_get_clock+0xe8>
    1370:	e2433020 	sub	r3, r3, #32	; 0x20
    1374:	e1500003 	cmp	r0, r3
    1378:	1a000009 	bne	13a4 <module_get_clock+0x10c>
    137c:	ea00000e 	b	13bc <module_get_clock+0x124>
    1380:	e59f3078 	ldr	r3, [pc, #120]	; 1400 <module_get_clock+0x168>
    1384:	e1500003 	cmp	r0, r3
    1388:	0a000003 	beq	139c <module_get_clock+0x104>
    138c:	e2833040 	add	r3, r3, #64	; 0x40
    1390:	e1500003 	cmp	r0, r3
    1394:	1a000002 	bne	13a4 <module_get_clock+0x10c>
    1398:	ea000009 	b	13c4 <module_get_clock+0x12c>
    139c:	e59f0060 	ldr	r0, [pc, #96]	; 1404 <module_get_clock+0x16c>
    13a0:	e12fff1e 	bx	lr
    13a4:	e3a00000 	mov	r0, #0	; 0x0
    13a8:	e12fff1e 	bx	lr
    13ac:	e59f0054 	ldr	r0, [pc, #84]	; 1408 <module_get_clock+0x170>
    13b0:	e12fff1e 	bx	lr
    13b4:	e59f0050 	ldr	r0, [pc, #80]	; 140c <module_get_clock+0x174>
    13b8:	e12fff1e 	bx	lr
    13bc:	e59f004c 	ldr	r0, [pc, #76]	; 1410 <module_get_clock+0x178>
    13c0:	e12fff1e 	bx	lr
    13c4:	e59f0048 	ldr	r0, [pc, #72]	; 1414 <module_get_clock+0x17c>
    13c8:	e12fff1e 	bx	lr
    13cc:	e59f0044 	ldr	r0, [pc, #68]	; 1418 <module_get_clock+0x180>
    13d0:	e12fff1e 	bx	lr
    13d4:	21e0a240 	.word	0x21e0a240
    13d8:	05fd8220 	.word	0x05fd8220
    13dc:	21e0a130 	.word	0x21e0a130
    13e0:	21e0a170 	.word	0x21e0a170
    13e4:	21e0a180 	.word	0x21e0a180
    13e8:	21e0a2c0 	.word	0x21e0a2c0
    13ec:	016e3600 	.word	0x016e3600
    13f0:	21e0a270 	.word	0x21e0a270
    13f4:	04c4b400 	.word	0x04c4b400
    13f8:	21e0a2f0 	.word	0x21e0a2f0
    13fc:	05f5e100 	.word	0x05f5e100
    1400:	21e0a320 	.word	0x21e0a320
    1404:	0bfb0440 	.word	0x0bfb0440
    1408:	02625a00 	.word	0x02625a00
    140c:	0160203b 	.word	0x0160203b
    1410:	02faf080 	.word	0x02faf080
    1414:	07fcad80 	.word	0x07fcad80
    1418:	0ff95b00 	.word	0x0ff95b00

0000141c <set_mux>:
    141c:	e59f3004 	ldr	r3, [pc, #4]	; 1428 <set_mux+0xc>
    1420:	e5830000 	str	r0, [r3]
    1424:	e12fff1e 	bx	lr
    1428:	21e09000 	.word	0x21e09000

0000142c <strcpy>:
    142c:	e3a02000 	mov	r2, #0	; 0x0
    1430:	e7d13002 	ldrb	r3, [r1, r2]
    1434:	e3530000 	cmp	r3, #0	; 0x0
    1438:	e7c03002 	strb	r3, [r0, r2]
    143c:	e2822001 	add	r2, r2, #1	; 0x1
    1440:	1afffffa 	bne	1430 <strcpy+0x4>
    1444:	e12fff1e 	bx	lr

00001448 <strncpy>:
    1448:	e3a0c000 	mov	ip, #0	; 0x0
    144c:	e3520000 	cmp	r2, #0	; 0x0
    1450:	e2422001 	sub	r2, r2, #1	; 0x1
    1454:	012fff1e 	bxeq	lr
    1458:	e7d1300c 	ldrb	r3, [r1, ip]
    145c:	e3530000 	cmp	r3, #0	; 0x0
    1460:	e7c0300c 	strb	r3, [r0, ip]
    1464:	e28cc001 	add	ip, ip, #1	; 0x1
    1468:	1afffff7 	bne	144c <strncpy+0x4>
    146c:	e12fff1e 	bx	lr

00001470 <strncat>:
    1470:	e92d4030 	push	{r4, r5, lr}
    1474:	e2525000 	subs	r5, r2, #0	; 0x0
    1478:	e1a04001 	mov	r4, r1
    147c:	11a0c000 	movne	ip, r0
    1480:	1a000001 	bne	148c <strncat+0x1c>
    1484:	e8bd8030 	pop	{r4, r5, pc}
    1488:	e28cc001 	add	ip, ip, #1	; 0x1
    148c:	e5dc3000 	ldrb	r3, [ip]
    1490:	e3530000 	cmp	r3, #0	; 0x0
    1494:	1afffffb 	bne	1488 <strncat+0x18>
    1498:	ea000004 	b	14b0 <strncat+0x40>
    149c:	e1550002 	cmp	r5, r2
    14a0:	1a000004 	bne	14b8 <strncat+0x48>
    14a4:	e3a03000 	mov	r3, #0	; 0x0
    14a8:	e5cc3000 	strb	r3, [ip]
    14ac:	e8bd8030 	pop	{r4, r5, pc}
    14b0:	e1a02003 	mov	r2, r3
    14b4:	e1a0100c 	mov	r1, ip
    14b8:	e7d43002 	ldrb	r3, [r4, r2]
    14bc:	e28cc001 	add	ip, ip, #1	; 0x1
    14c0:	e3530000 	cmp	r3, #0	; 0x0
    14c4:	e7c13002 	strb	r3, [r1, r2]
    14c8:	e2822001 	add	r2, r2, #1	; 0x1
    14cc:	1afffff2 	bne	149c <strncat+0x2c>
    14d0:	e8bd8030 	pop	{r4, r5, pc}

000014d4 <strcmp>:
    14d4:	e92d4010 	push	{r4, lr}
    14d8:	e3a02000 	mov	r2, #0	; 0x0
    14dc:	e1a04000 	mov	r4, r0
    14e0:	e7d4c002 	ldrb	ip, [r4, r2]
    14e4:	e7d13002 	ldrb	r3, [r1, r2]
    14e8:	e2822001 	add	r2, r2, #1	; 0x1
    14ec:	e063300c 	rsb	r3, r3, ip
    14f0:	e6ef0073 	uxtb	r0, r3
    14f4:	e3500000 	cmp	r0, #0	; 0x0
    14f8:	1a000001 	bne	1504 <strcmp+0x30>
    14fc:	e35c0000 	cmp	ip, #0	; 0x0
    1500:	1afffff6 	bne	14e0 <strcmp+0xc>
    1504:	e6af0070 	sxtb	r0, r0
    1508:	e8bd8010 	pop	{r4, pc}

0000150c <strncmp>:
    150c:	e92d4030 	push	{r4, r5, lr}
    1510:	e3a0c000 	mov	ip, #0	; 0x0
    1514:	e1a05000 	mov	r5, r0
    1518:	e1a04001 	mov	r4, r1
    151c:	ea000009 	b	1548 <strncmp+0x3c>
    1520:	e7d5100c 	ldrb	r1, [r5, ip]
    1524:	e7d4300c 	ldrb	r3, [r4, ip]
    1528:	e2422001 	sub	r2, r2, #1	; 0x1
    152c:	e0633001 	rsb	r3, r3, r1
    1530:	e6ef0073 	uxtb	r0, r3
    1534:	e3500000 	cmp	r0, #0	; 0x0
    1538:	e28cc001 	add	ip, ip, #1	; 0x1
    153c:	1a000004 	bne	1554 <strncmp+0x48>
    1540:	e3510000 	cmp	r1, #0	; 0x0
    1544:	0a000002 	beq	1554 <strncmp+0x48>
    1548:	e3520000 	cmp	r2, #0	; 0x0
    154c:	1afffff3 	bne	1520 <strncmp+0x14>
    1550:	e1a00002 	mov	r0, r2
    1554:	e6af0070 	sxtb	r0, r0
    1558:	e8bd8030 	pop	{r4, r5, pc}

0000155c <strchr>:
    155c:	e1a03000 	mov	r3, r0
    1560:	e6ef1071 	uxtb	r1, r1
    1564:	ea000002 	b	1574 <strchr+0x18>
    1568:	e3500000 	cmp	r0, #0	; 0x0
    156c:	012fff1e 	bxeq	lr
    1570:	e2833001 	add	r3, r3, #1	; 0x1
    1574:	e5d30000 	ldrb	r0, [r3]
    1578:	e1500001 	cmp	r0, r1
    157c:	1afffff9 	bne	1568 <strchr+0xc>
    1580:	e1a00003 	mov	r0, r3
    1584:	e12fff1e 	bx	lr

00001588 <strlen>:
    1588:	e1a02000 	mov	r2, r0
    158c:	ea000000 	b	1594 <strlen+0xc>
    1590:	e2800001 	add	r0, r0, #1	; 0x1
    1594:	e5d03000 	ldrb	r3, [r0]
    1598:	e3530000 	cmp	r3, #0	; 0x0
    159c:	1afffffb 	bne	1590 <strlen+0x8>
    15a0:	e0620000 	rsb	r0, r2, r0
    15a4:	e12fff1e 	bx	lr

000015a8 <strnlen>:
    15a8:	e3a02000 	mov	r2, #0	; 0x0
    15ac:	e1510002 	cmp	r1, r2
    15b0:	e080c002 	add	ip, r0, r2
    15b4:	0a000003 	beq	15c8 <strnlen+0x20>
    15b8:	e7d03002 	ldrb	r3, [r0, r2]
    15bc:	e2822001 	add	r2, r2, #1	; 0x1
    15c0:	e3530000 	cmp	r3, #0	; 0x0
    15c4:	1afffff8 	bne	15ac <strnlen+0x4>
    15c8:	e060000c 	rsb	r0, r0, ip
    15cc:	e12fff1e 	bx	lr

000015d0 <memset>:
    15d0:	e3a03000 	mov	r3, #0	; 0x0
    15d4:	ea000001 	b	15e0 <memset+0x10>
    15d8:	e7c01003 	strb	r1, [r0, r3]
    15dc:	e2833001 	add	r3, r3, #1	; 0x1
    15e0:	e3520000 	cmp	r2, #0	; 0x0
    15e4:	e2422001 	sub	r2, r2, #1	; 0x1
    15e8:	1afffffa 	bne	15d8 <memset+0x8>
    15ec:	e12fff1e 	bx	lr

000015f0 <memcpy>:
    15f0:	e3a0c000 	mov	ip, #0	; 0x0
    15f4:	ea000002 	b	1604 <memcpy+0x14>
    15f8:	e7d1300c 	ldrb	r3, [r1, ip]
    15fc:	e7c0300c 	strb	r3, [r0, ip]
    1600:	e28cc001 	add	ip, ip, #1	; 0x1
    1604:	e3520000 	cmp	r2, #0	; 0x0
    1608:	e2422001 	sub	r2, r2, #1	; 0x1
    160c:	1afffff9 	bne	15f8 <memcpy+0x8>
    1610:	e12fff1e 	bx	lr

00001614 <memmove>:
    1614:	e1500001 	cmp	r0, r1
    1618:	e92d4010 	push	{r4, lr}
    161c:	08bd8010 	popeq	{r4, pc}
    1620:	93a0c000 	movls	ip, #0	; 0x0
    1624:	9a000003 	bls	1638 <memmove+0x24>
    1628:	ea000006 	b	1648 <memmove+0x34>
    162c:	e7d1300c 	ldrb	r3, [r1, ip]
    1630:	e7c0300c 	strb	r3, [r0, ip]
    1634:	e28cc001 	add	ip, ip, #1	; 0x1
    1638:	e3520000 	cmp	r2, #0	; 0x0
    163c:	e2422001 	sub	r2, r2, #1	; 0x1
    1640:	1afffff9 	bne	162c <memmove+0x18>
    1644:	e8bd8010 	pop	{r4, pc}
    1648:	e0814002 	add	r4, r1, r2
    164c:	e3a0c000 	mov	ip, #0	; 0x0
    1650:	e0801002 	add	r1, r0, r2
    1654:	ea000001 	b	1660 <memmove+0x4c>
    1658:	e7d4300c 	ldrb	r3, [r4, ip]
    165c:	e7c1300c 	strb	r3, [r1, ip]
    1660:	e3520000 	cmp	r2, #0	; 0x0
    1664:	e24cc001 	sub	ip, ip, #1	; 0x1
    1668:	e2422001 	sub	r2, r2, #1	; 0x1
    166c:	1afffff9 	bne	1658 <memmove+0x44>
    1670:	e8bd8010 	pop	{r4, pc}

00001674 <memcmp>:
    1674:	e92d4030 	push	{r4, r5, lr}
    1678:	e3a0c000 	mov	ip, #0	; 0x0
    167c:	e1a04001 	mov	r4, r1
    1680:	e1a05000 	mov	r5, r0
    1684:	e1a01002 	mov	r1, r2
    1688:	ea000005 	b	16a4 <memcmp+0x30>
    168c:	e7d5200c 	ldrb	r2, [r5, ip]
    1690:	e7d4300c 	ldrb	r3, [r4, ip]
    1694:	e28cc001 	add	ip, ip, #1	; 0x1
    1698:	e0520003 	subs	r0, r2, r3
    169c:	18bd8030 	popne	{r4, r5, pc}
    16a0:	e2411001 	sub	r1, r1, #1	; 0x1
    16a4:	e3510000 	cmp	r1, #0	; 0x0
    16a8:	1afffff7 	bne	168c <memcmp+0x18>
    16ac:	e1a00001 	mov	r0, r1
    16b0:	e8bd8030 	pop	{r4, r5, pc}

000016b4 <so_m_isused>:
    16b4:	e5903004 	ldr	r3, [r0, #4]
    16b8:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    16bc:	03a00000 	moveq	r0, #0	; 0x0
    16c0:	13a00001 	movne	r0, #1	; 0x1
    16c4:	e12fff1e 	bx	lr

000016c8 <so_m_set_used>:
    16c8:	e5903004 	ldr	r3, [r0, #4]
    16cc:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    16d0:	e5803004 	str	r3, [r0, #4]
    16d4:	e12fff1e 	bx	lr

000016d8 <so_m_unset_used>:
    16d8:	e5903004 	ldr	r3, [r0, #4]
    16dc:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    16e0:	e5803004 	str	r3, [r0, #4]
    16e4:	e12fff1e 	bx	lr

000016e8 <so_m_get_size>:
    16e8:	e5900004 	ldr	r0, [r0, #4]
    16ec:	e3c00103 	bic	r0, r0, #-1073741824	; 0xc0000000
    16f0:	e12fff1e 	bx	lr

000016f4 <so_m_set_size>:
    16f4:	e5903004 	ldr	r3, [r0, #4]
    16f8:	e3c11103 	bic	r1, r1, #-1073741824	; 0xc0000000
    16fc:	e2033103 	and	r3, r3, #-1073741824	; 0xc0000000
    1700:	e1811003 	orr	r1, r1, r3
    1704:	e5801004 	str	r1, [r0, #4]
    1708:	e12fff1e 	bx	lr

0000170c <so_m_alloc>:
    170c:	e59f3048 	ldr	r3, [pc, #72]	; 175c <so_m_alloc+0x50>
    1710:	e3a02000 	mov	r2, #0	; 0x0
    1714:	e593c000 	ldr	ip, [r3]
    1718:	e5931008 	ldr	r1, [r3, #8]
    171c:	ea000007 	b	1740 <so_m_alloc+0x34>
    1720:	e5903004 	ldr	r3, [r0, #4]
    1724:	e3530000 	cmp	r3, #0	; 0x0
    1728:	1a000004 	bne	1740 <so_m_alloc+0x34>
    172c:	e59f2028 	ldr	r2, [pc, #40]	; 175c <so_m_alloc+0x50>
    1730:	e5923004 	ldr	r3, [r2, #4]
    1734:	e2433001 	sub	r3, r3, #1	; 0x1
    1738:	e5823004 	str	r3, [r2, #4]
    173c:	e12fff1e 	bx	lr
    1740:	e152000c 	cmp	r2, ip
    1744:	e1a00001 	mov	r0, r1
    1748:	e2822001 	add	r2, r2, #1	; 0x1
    174c:	e2811010 	add	r1, r1, #16	; 0x10
    1750:	3afffff2 	bcc	1720 <so_m_alloc+0x14>
    1754:	e3a00000 	mov	r0, #0	; 0x0
    1758:	e12fff1e 	bx	lr
    175c:	3c00d6d4 	.word	0x3c00d6d4

00001760 <so_m_remove>:
    1760:	e3a03000 	mov	r3, #0	; 0x0
    1764:	e59f2014 	ldr	r2, [pc, #20]	; 1780 <so_m_remove+0x20>
    1768:	e5803000 	str	r3, [r0]
    176c:	e5803004 	str	r3, [r0, #4]
    1770:	e5923004 	ldr	r3, [r2, #4]
    1774:	e2833001 	add	r3, r3, #1	; 0x1
    1778:	e5823004 	str	r3, [r2, #4]
    177c:	e12fff1e 	bx	lr
    1780:	3c00d6d4 	.word	0x3c00d6d4

00001784 <malloc>:
    1784:	e3500000 	cmp	r0, #0	; 0x0
    1788:	e92d4070 	push	{r4, r5, r6, lr}
    178c:	0a000047 	beq	18b0 <malloc+0x12c>
    1790:	e59f1120 	ldr	r1, [pc, #288]	; 18b8 <malloc+0x134>
    1794:	e280301f 	add	r3, r0, #31	; 0x1f
    1798:	e5b12014 	ldr	r2, [r1, #20]!
    179c:	e3c3401f 	bic	r4, r3, #31	; 0x1f
    17a0:	e2425008 	sub	r5, r2, #8	; 0x8
    17a4:	ea000009 	b	17d0 <malloc+0x4c>
    17a8:	e5953004 	ldr	r3, [r5, #4]
    17ac:	e2130103 	ands	r0, r3, #-1073741824	; 0xc0000000
    17b0:	e3c32103 	bic	r2, r3, #-1073741824	; 0xc0000000
    17b4:	1a000003 	bne	17c8 <malloc+0x44>
    17b8:	e1520004 	cmp	r2, r4
    17bc:	3a000001 	bcc	17c8 <malloc+0x44>
    17c0:	9a000030 	bls	1888 <malloc+0x104>
    17c4:	ea000005 	b	17e0 <malloc+0x5c>
    17c8:	e5953008 	ldr	r3, [r5, #8]
    17cc:	e2435008 	sub	r5, r3, #8	; 0x8
    17d0:	e2853008 	add	r3, r5, #8	; 0x8
    17d4:	e1530001 	cmp	r3, r1
    17d8:	1afffff2 	bne	17a8 <malloc+0x24>
    17dc:	ea000033 	b	18b0 <malloc+0x12c>
    17e0:	e59f30d0 	ldr	r3, [pc, #208]	; 18b8 <malloc+0x134>
    17e4:	e1a01000 	mov	r1, r0
    17e8:	e5932008 	ldr	r2, [r3, #8]
    17ec:	e5930000 	ldr	r0, [r3]
    17f0:	ea00000a 	b	1820 <malloc+0x9c>
    17f4:	e5923004 	ldr	r3, [r2, #4]
    17f8:	e2822010 	add	r2, r2, #16	; 0x10
    17fc:	e3530000 	cmp	r3, #0	; 0x0
    1800:	1a000006 	bne	1820 <malloc+0x9c>
    1804:	e59f20ac 	ldr	r2, [pc, #172]	; 18b8 <malloc+0x134>
    1808:	e3560000 	cmp	r6, #0	; 0x0
    180c:	e5923004 	ldr	r3, [r2, #4]
    1810:	e2433001 	sub	r3, r3, #1	; 0x1
    1814:	e5823004 	str	r3, [r2, #4]
    1818:	1a000005 	bne	1834 <malloc+0xb0>
    181c:	ea000023 	b	18b0 <malloc+0x12c>
    1820:	e1510000 	cmp	r1, r0
    1824:	e1a06002 	mov	r6, r2
    1828:	e2811001 	add	r1, r1, #1	; 0x1
    182c:	3afffff0 	bcc	17f4 <malloc+0x70>
    1830:	ea00001e 	b	18b0 <malloc+0x12c>
    1834:	e5953004 	ldr	r3, [r5, #4]
    1838:	e5962004 	ldr	r2, [r6, #4]
    183c:	e0643003 	rsb	r3, r4, r3
    1840:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    1844:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1848:	e1833002 	orr	r3, r3, r2
    184c:	e5863004 	str	r3, [r6, #4]
    1850:	e5951000 	ldr	r1, [r5]
    1854:	e5950004 	ldr	r0, [r5, #4]
    1858:	e1a0c005 	mov	ip, r5
    185c:	e1a03006 	mov	r3, r6
    1860:	e0841001 	add	r1, r4, r1
    1864:	e3c42103 	bic	r2, r4, #-1073741824	; 0xc0000000
    1868:	e2000103 	and	r0, r0, #-1073741824	; 0xc0000000
    186c:	e5bc4008 	ldr	r4, [ip, #8]!
    1870:	e4831008 	str	r1, [r3], #8
    1874:	e1822000 	orr	r2, r2, r0
    1878:	e5864008 	str	r4, [r6, #8]
    187c:	e5843004 	str	r3, [r4, #4]
    1880:	e985000c 	stmib	r5, {r2, r3}
    1884:	e586c00c 	str	ip, [r6, #12]
    1888:	e5953004 	ldr	r3, [r5, #4]
    188c:	e59f1024 	ldr	r1, [pc, #36]	; 18b8 <malloc+0x134>
    1890:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    1894:	e5853004 	str	r3, [r5, #4]
    1898:	e5912010 	ldr	r2, [r1, #16]
    189c:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    18a0:	e0833002 	add	r3, r3, r2
    18a4:	e5813010 	str	r3, [r1, #16]
    18a8:	e5950000 	ldr	r0, [r5]
    18ac:	e8bd8070 	pop	{r4, r5, r6, pc}
    18b0:	e3a00000 	mov	r0, #0	; 0x0
    18b4:	e8bd8070 	pop	{r4, r5, r6, pc}
    18b8:	3c00d6d4 	.word	0x3c00d6d4

000018bc <__sOMERGe>:
    18bc:	e5902004 	ldr	r2, [r0, #4]
    18c0:	e92d4030 	push	{r4, r5, lr}
    18c4:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    18c8:	e1a05000 	mov	r5, r0
    18cc:	e1a0c001 	mov	ip, r1
    18d0:	18bd8030 	popne	{r4, r5, pc}
    18d4:	e5913004 	ldr	r3, [r1, #4]
    18d8:	e2134103 	ands	r4, r3, #-1073741824	; 0xc0000000
    18dc:	18bd8030 	popne	{r4, r5, pc}
    18e0:	e0823003 	add	r3, r2, r3
    18e4:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    18e8:	e59f0030 	ldr	r0, [pc, #48]	; 1920 <__sOMERGe+0x64>
    18ec:	e5853004 	str	r3, [r5, #4]
    18f0:	e5814000 	str	r4, [r1]
    18f4:	e5814004 	str	r4, [r1, #4]
    18f8:	e5903004 	ldr	r3, [r0, #4]
    18fc:	e591100c 	ldr	r1, [r1, #12]
    1900:	e59c2008 	ldr	r2, [ip, #8]
    1904:	e2833001 	add	r3, r3, #1	; 0x1
    1908:	e5812000 	str	r2, [r1]
    190c:	e5821004 	str	r1, [r2, #4]
    1910:	e5803004 	str	r3, [r0, #4]
    1914:	e58c4008 	str	r4, [ip, #8]
    1918:	e58c400c 	str	r4, [ip, #12]
    191c:	e8bd8030 	pop	{r4, r5, pc}
    1920:	3c00d6d4 	.word	0x3c00d6d4

00001924 <free>:
    1924:	e3500000 	cmp	r0, #0	; 0x0
    1928:	159f2090 	ldrne	r2, [pc, #144]	; 19c0 <free+0x9c>
    192c:	e92d4010 	push	{r4, lr}
    1930:	15b23014 	ldrne	r3, [r2, #20]!
    1934:	1a000004 	bne	194c <free+0x28>
    1938:	e8bd8010 	pop	{r4, pc}
    193c:	e5943000 	ldr	r3, [r4]
    1940:	e1500003 	cmp	r0, r3
    1944:	0a000005 	beq	1960 <free+0x3c>
    1948:	e5943008 	ldr	r3, [r4, #8]
    194c:	e2434008 	sub	r4, r3, #8	; 0x8
    1950:	e2843008 	add	r3, r4, #8	; 0x8
    1954:	e1530002 	cmp	r3, r2
    1958:	1afffff7 	bne	193c <free+0x18>
    195c:	e8bd8010 	pop	{r4, pc}
    1960:	e5943004 	ldr	r3, [r4, #4]
    1964:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    1968:	08bd8010 	popeq	{r4, pc}
    196c:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1970:	e59f1048 	ldr	r1, [pc, #72]	; 19c0 <free+0x9c>
    1974:	e5843004 	str	r3, [r4, #4]
    1978:	e5912010 	ldr	r2, [r1, #16]
    197c:	e5940008 	ldr	r0, [r4, #8]
    1980:	e0632002 	rsb	r2, r3, r2
    1984:	e59f3038 	ldr	r3, [pc, #56]	; 19c4 <free+0xa0>
    1988:	e5812010 	str	r2, [r1, #16]
    198c:	e1500003 	cmp	r0, r3
    1990:	0a000002 	beq	19a0 <free+0x7c>
    1994:	e2401008 	sub	r1, r0, #8	; 0x8
    1998:	e1a00004 	mov	r0, r4
    199c:	ebffffc6 	bl	18bc <__sOMERGe>
    19a0:	e594000c 	ldr	r0, [r4, #12]
    19a4:	e59f3018 	ldr	r3, [pc, #24]	; 19c4 <free+0xa0>
    19a8:	e1500003 	cmp	r0, r3
    19ac:	08bd8010 	popeq	{r4, pc}
    19b0:	e2400008 	sub	r0, r0, #8	; 0x8
    19b4:	e1a01004 	mov	r1, r4
    19b8:	e8bd4010 	pop	{r4, lr}
    19bc:	eaffffbe 	b	18bc <__sOMERGe>
    19c0:	3c00d6d4 	.word	0x3c00d6d4
    19c4:	3c00d6e8 	.word	0x3c00d6e8

000019c8 <__sOINFo>:
    19c8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    19cc:	e59f50a8 	ldr	r5, [pc, #168]	; 1a7c <__sOINFo+0xb4>
    19d0:	e24dd004 	sub	sp, sp, #4	; 0x4
    19d4:	e59f60a4 	ldr	r6, [pc, #164]	; 1a80 <__sOINFo+0xb8>
    19d8:	e8950006 	ldm	r5, {r1, r2}
    19dc:	e59f00a0 	ldr	r0, [pc, #160]	; 1a84 <__sOINFo+0xbc>
    19e0:	e1a0e00f 	mov	lr, pc
    19e4:	e596f000 	ldr	pc, [r6]
    19e8:	e2853008 	add	r3, r5, #8	; 0x8
    19ec:	e8930098 	ldm	r3, {r3, r4, r7}
    19f0:	e2444b01 	sub	r4, r4, #1024	; 0x400
    19f4:	e0634004 	rsb	r4, r3, r4
    19f8:	e3a00064 	mov	r0, #100	; 0x64
    19fc:	e1a01004 	mov	r1, r4
    1a00:	e0000790 	mul	r0, r0, r7
    1a04:	eb004244 	bl	1231c <__aeabi_uidiv>
    1a08:	e1a01004 	mov	r1, r4
    1a0c:	e1a03000 	mov	r3, r0
    1a10:	e1a02007 	mov	r2, r7
    1a14:	e59f006c 	ldr	r0, [pc, #108]	; 1a88 <__sOINFo+0xc0>
    1a18:	e1a0e00f 	mov	lr, pc
    1a1c:	e596f000 	ldr	pc, [r6]
    1a20:	e59f0064 	ldr	r0, [pc, #100]	; 1a8c <__sOINFo+0xc4>
    1a24:	e1a0e00f 	mov	lr, pc
    1a28:	e596f000 	ldr	pc, [r6]
    1a2c:	e59f005c 	ldr	r0, [pc, #92]	; 1a90 <__sOINFo+0xc8>
    1a30:	e1a0e00f 	mov	lr, pc
    1a34:	e596f000 	ldr	pc, [r6]
    1a38:	e5b53014 	ldr	r3, [r5, #20]!
    1a3c:	ea000007 	b	1a60 <__sOINFo+0x98>
    1a40:	e8940006 	ldm	r4, {r1, r2}
    1a44:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    1a48:	03a03000 	moveq	r3, #0	; 0x0
    1a4c:	13a03001 	movne	r3, #1	; 0x1
    1a50:	e3c22103 	bic	r2, r2, #-1073741824	; 0xc0000000
    1a54:	e1a0e00f 	mov	lr, pc
    1a58:	e596f000 	ldr	pc, [r6]
    1a5c:	e5943008 	ldr	r3, [r4, #8]
    1a60:	e2434008 	sub	r4, r3, #8	; 0x8
    1a64:	e2843008 	add	r3, r4, #8	; 0x8
    1a68:	e1530005 	cmp	r3, r5
    1a6c:	e59f0020 	ldr	r0, [pc, #32]	; 1a94 <__sOINFo+0xcc>
    1a70:	1afffff2 	bne	1a40 <__sOINFo+0x78>
    1a74:	e28dd004 	add	sp, sp, #4	; 0x4
    1a78:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1a7c:	3c00d6d4 	.word	0x3c00d6d4
    1a80:	3c00d694 	.word	0x3c00d694
    1a84:	00019f35 	.word	0x00019f35
    1a88:	00019f4f 	.word	0x00019f4f
    1a8c:	00019f74 	.word	0x00019f74
    1a90:	00019e20 	.word	0x00019e20
    1a94:	00019f85 	.word	0x00019f85

00001a98 <init_malloc>:
    1a98:	e59f20e4 	ldr	r2, [pc, #228]	; 1b84 <init_malloc+0xec>
    1a9c:	e59f30e4 	ldr	r3, [pc, #228]	; 1b88 <init_malloc+0xf0>
    1aa0:	e3c2001f 	bic	r0, r2, #31	; 0x1f
    1aa4:	e3c3301f 	bic	r3, r3, #31	; 0x1f
    1aa8:	e0602003 	rsb	r2, r0, r3
    1aac:	e92d4010 	push	{r4, lr}
    1ab0:	e59f40d4 	ldr	r4, [pc, #212]	; 1b8c <init_malloc+0xf4>
    1ab4:	e3520b01 	cmp	r2, #1024	; 0x400
    1ab8:	e5840008 	str	r0, [r4, #8]
    1abc:	e584300c 	str	r3, [r4, #12]
    1ac0:	93e00000 	mvnls	r0, #0	; 0x0
    1ac4:	98bd8010 	popls	{r4, pc}
    1ac8:	e284c014 	add	ip, r4, #20	; 0x14
    1acc:	e3a03040 	mov	r3, #64	; 0x40
    1ad0:	e3a01000 	mov	r1, #0	; 0x0
    1ad4:	e3a02b01 	mov	r2, #1024	; 0x400
    1ad8:	e584c018 	str	ip, [r4, #24]
    1adc:	e584c014 	str	ip, [r4, #20]
    1ae0:	e5843004 	str	r3, [r4, #4]
    1ae4:	e5843000 	str	r3, [r4]
    1ae8:	ebfffeb8 	bl	15d0 <memset>
    1aec:	e5940008 	ldr	r0, [r4, #8]
    1af0:	e594c000 	ldr	ip, [r4]
    1af4:	e1a01000 	mov	r1, r0
    1af8:	e3a02000 	mov	r2, #0	; 0x0
    1afc:	ea000006 	b	1b1c <init_malloc+0x84>
    1b00:	e59e3004 	ldr	r3, [lr, #4]
    1b04:	e3530000 	cmp	r3, #0	; 0x0
    1b08:	059f207c 	ldreq	r2, [pc, #124]	; 1b8c <init_malloc+0xf4>
    1b0c:	05923004 	ldreq	r3, [r2, #4]
    1b10:	02433001 	subeq	r3, r3, #1	; 0x1
    1b14:	05823004 	streq	r3, [r2, #4]
    1b18:	0a000005 	beq	1b34 <init_malloc+0x9c>
    1b1c:	e152000c 	cmp	r2, ip
    1b20:	e1a0e001 	mov	lr, r1
    1b24:	e2822001 	add	r2, r2, #1	; 0x1
    1b28:	e2811010 	add	r1, r1, #16	; 0x10
    1b2c:	3afffff3 	bcc	1b00 <init_malloc+0x68>
    1b30:	e3a0e000 	mov	lr, #0	; 0x0
    1b34:	e59fc050 	ldr	ip, [pc, #80]	; 1b8c <init_malloc+0xf4>
    1b38:	e59e2004 	ldr	r2, [lr, #4]
    1b3c:	e59c300c 	ldr	r3, [ip, #12]
    1b40:	e1a0100e 	mov	r1, lr
    1b44:	e2433b01 	sub	r3, r3, #1024	; 0x400
    1b48:	e0603003 	rsb	r3, r0, r3
    1b4c:	e59c4018 	ldr	r4, [ip, #24]
    1b50:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    1b54:	e2800b01 	add	r0, r0, #1024	; 0x400
    1b58:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1b5c:	e4810008 	str	r0, [r1], #8
    1b60:	e1833002 	orr	r3, r3, r2
    1b64:	e28c2014 	add	r2, ip, #20	; 0x14
    1b68:	e58e2008 	str	r2, [lr, #8]
    1b6c:	e58c1018 	str	r1, [ip, #24]
    1b70:	e5841000 	str	r1, [r4]
    1b74:	e58e3004 	str	r3, [lr, #4]
    1b78:	e58e400c 	str	r4, [lr, #12]
    1b7c:	e3a00000 	mov	r0, #0	; 0x0
    1b80:	e8bd8010 	pop	{r4, pc}
    1b84:	3c00e01f 	.word	0x3c00e01f
    1b88:	3c018000 	.word	0x3c018000
    1b8c:	3c00d6d4 	.word	0x3c00d6d4

00001b90 <isi_check_header>:
    1b90:	e92d4070 	push	{r4, r5, r6, lr}
    1b94:	e5902000 	ldr	r2, [r0]
    1b98:	e59f3088 	ldr	r3, [pc, #136]	; 1c28 <isi_check_header+0x98>
    1b9c:	e1a05000 	mov	r5, r0
    1ba0:	e1520003 	cmp	r2, r3
    1ba4:	e24dd008 	sub	sp, sp, #8	; 0x8
    1ba8:	13e0001e 	mvnne	r0, #30	; 0x1e
    1bac:	1a00001b 	bne	1c20 <isi_check_header+0x90>
    1bb0:	e3a06000 	mov	r6, #0	; 0x0
    1bb4:	e595401c 	ldr	r4, [r5, #28]
    1bb8:	e3a01c02 	mov	r1, #512	; 0x200
    1bbc:	e585601c 	str	r6, [r5, #28]
    1bc0:	e1a00006 	mov	r0, r6
    1bc4:	eb0013ea 	bl	6b74 <hash_init>
    1bc8:	e3a01c02 	mov	r1, #512	; 0x200
    1bcc:	e1a00005 	mov	r0, r5
    1bd0:	eb0013c2 	bl	6ae0 <hash_data>
    1bd4:	e28d0004 	add	r0, sp, #4	; 0x4
    1bd8:	eb0013a4 	bl	6a70 <hash_value>
    1bdc:	e585401c 	str	r4, [r5, #28]
    1be0:	e59d3004 	ldr	r3, [sp, #4]
    1be4:	e1540003 	cmp	r4, r3
    1be8:	13e0001f 	mvnne	r0, #31	; 0x1f
    1bec:	1a00000b 	bne	1c20 <isi_check_header+0x90>
    1bf0:	e5951004 	ldr	r1, [r5, #4]
    1bf4:	e59f4030 	ldr	r4, [pc, #48]	; 1c2c <isi_check_header+0x9c>
    1bf8:	e3c114ff 	bic	r1, r1, #-16777216	; 0xff000000
    1bfc:	e59f002c 	ldr	r0, [pc, #44]	; 1c30 <isi_check_header+0xa0>
    1c00:	e1a0e00f 	mov	lr, pc
    1c04:	e594f000 	ldr	pc, [r4]
    1c08:	e5d51007 	ldrb	r1, [r5, #7]
    1c0c:	e59f0020 	ldr	r0, [pc, #32]	; 1c34 <isi_check_header+0xa4>
    1c10:	e201100f 	and	r1, r1, #15	; 0xf
    1c14:	e1a0e00f 	mov	lr, pc
    1c18:	e594f000 	ldr	pc, [r4]
    1c1c:	e1a00006 	mov	r0, r6
    1c20:	e28dd008 	add	sp, sp, #8	; 0x8
    1c24:	e8bd8070 	pop	{r4, r5, r6, pc}
    1c28:	6973692b 	.word	0x6973692b
    1c2c:	3c00d694 	.word	0x3c00d694
    1c30:	00019fad 	.word	0x00019fad
    1c34:	00019fbb 	.word	0x00019fbb

00001c38 <isi_check_data_separate>:
    1c38:	e92d4070 	push	{r4, r5, r6, lr}
    1c3c:	e1a05000 	mov	r5, r0
    1c40:	e5d00007 	ldrb	r0, [r0, #7]
    1c44:	e24dd028 	sub	sp, sp, #40	; 0x28
    1c48:	e1a04001 	mov	r4, r1
    1c4c:	e200000f 	and	r0, r0, #15	; 0xf
    1c50:	e595100c 	ldr	r1, [r5, #12]
    1c54:	eb0013c6 	bl	6b74 <hash_init>
    1c58:	e28d6008 	add	r6, sp, #8	; 0x8
    1c5c:	e595100c 	ldr	r1, [r5, #12]
    1c60:	e1a00004 	mov	r0, r4
    1c64:	eb00139d 	bl	6ae0 <hash_data>
    1c68:	e1a00006 	mov	r0, r6
    1c6c:	eb00137f 	bl	6a70 <hash_value>
    1c70:	eb001377 	bl	6a54 <hash_len>
    1c74:	e2854020 	add	r4, r5, #32	; 0x20
    1c78:	e1a02000 	mov	r2, r0
    1c7c:	e1a01004 	mov	r1, r4
    1c80:	e1a00006 	mov	r0, r6
    1c84:	ebfffe7a 	bl	1674 <memcmp>
    1c88:	e3500000 	cmp	r0, #0	; 0x0
    1c8c:	0a000014 	beq	1ce4 <isi_check_data_separate+0xac>
    1c90:	e595100c 	ldr	r1, [r5, #12]
    1c94:	e59f3050 	ldr	r3, [pc, #80]	; 1cec <isi_check_data_separate+0xb4>
    1c98:	e59f0050 	ldr	r0, [pc, #80]	; 1cf0 <isi_check_data_separate+0xb8>
    1c9c:	e1a0e00f 	mov	lr, pc
    1ca0:	e593f000 	ldr	pc, [r3]
    1ca4:	eb00136a 	bl	6a54 <hash_len>
    1ca8:	e1a01004 	mov	r1, r4
    1cac:	e1a03000 	mov	r3, r0
    1cb0:	e3a02001 	mov	r2, #1	; 0x1
    1cb4:	e1a00004 	mov	r0, r4
    1cb8:	e3a04020 	mov	r4, #32	; 0x20
    1cbc:	e58d4000 	str	r4, [sp]
    1cc0:	eb0034f9 	bl	f0ac <print_buffer>
    1cc4:	eb001362 	bl	6a54 <hash_len>
    1cc8:	e1a01006 	mov	r1, r6
    1ccc:	e1a03000 	mov	r3, r0
    1cd0:	e3a02001 	mov	r2, #1	; 0x1
    1cd4:	e1a00006 	mov	r0, r6
    1cd8:	e58d4000 	str	r4, [sp]
    1cdc:	eb0034f2 	bl	f0ac <print_buffer>
    1ce0:	e3e00020 	mvn	r0, #32	; 0x20
    1ce4:	e28dd028 	add	sp, sp, #40	; 0x28
    1ce8:	e8bd8070 	pop	{r4, r5, r6, pc}
    1cec:	3c00d694 	.word	0x3c00d694
    1cf0:	00019fce 	.word	0x00019fce

00001cf4 <isi_check_data>:
    1cf4:	e2801c02 	add	r1, r0, #512	; 0x200
    1cf8:	eaffffce 	b	1c38 <isi_check_data_separate>

00001cfc <isi_check>:
    1cfc:	e92d4010 	push	{r4, lr}
    1d00:	e1a04000 	mov	r4, r0
    1d04:	ebffffa1 	bl	1b90 <isi_check_header>
    1d08:	e3500000 	cmp	r0, #0	; 0x0
    1d0c:	18bd8010 	popne	{r4, pc}
    1d10:	e1a00004 	mov	r0, r4
    1d14:	e8bd4010 	pop	{r4, lr}
    1d18:	eafffff5 	b	1cf4 <isi_check_data>

00001d1c <load_serial_ymodem>:
    1d1c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    1d20:	e24dde41 	sub	sp, sp, #1040	; 0x410
    1d24:	e28d4b01 	add	r4, sp, #1024	; 0x400
    1d28:	e284400c 	add	r4, r4, #12	; 0xc
    1d2c:	e3a03002 	mov	r3, #2	; 0x2
    1d30:	e1a07000 	mov	r7, r0
    1d34:	e1a01004 	mov	r1, r4
    1d38:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1d3c:	e58d3404 	str	r3, [sp, #1028]
    1d40:	eb0040fb 	bl	12134 <xyzModem_stream_open>
    1d44:	e3500000 	cmp	r0, #0	; 0x0
    1d48:	1a000012 	bne	1d98 <load_serial_ymodem+0x7c>
    1d4c:	e1a08004 	mov	r8, r4
    1d50:	e1a05000 	mov	r5, r0
    1d54:	e1a04000 	mov	r4, r0
    1d58:	e1a0600d 	mov	r6, sp
    1d5c:	ea000001 	b	1d68 <load_serial_ymodem+0x4c>
    1d60:	e085500c 	add	r5, r5, ip
    1d64:	ebfffe21 	bl	15f0 <memcpy>
    1d68:	e3a01b01 	mov	r1, #1024	; 0x400
    1d6c:	e1a02008 	mov	r2, r8
    1d70:	e1a0000d 	mov	r0, sp
    1d74:	eb00405a 	bl	11ee4 <xyzModem_stream_read>
    1d78:	e0843007 	add	r3, r4, r7
    1d7c:	e250c000 	subs	ip, r0, #0	; 0x0
    1d80:	e1a0100d 	mov	r1, sp
    1d84:	e1a0200c 	mov	r2, ip
    1d88:	e1a00003 	mov	r0, r3
    1d8c:	e084400c 	add	r4, r4, ip
    1d90:	cafffff2 	bgt	1d60 <load_serial_ymodem+0x44>
    1d94:	ea000007 	b	1db8 <load_serial_ymodem+0x9c>
    1d98:	e59f3054 	ldr	r3, [pc, #84]	; 1df4 <load_serial_ymodem+0xd8>
    1d9c:	e59d040c 	ldr	r0, [sp, #1036]
    1da0:	e5934000 	ldr	r4, [r3]
    1da4:	eb003cd5 	bl	11100 <xyzModem_error>
    1da8:	e1a01000 	mov	r1, r0
    1dac:	e59f0044 	ldr	r0, [pc, #68]	; 1df8 <load_serial_ymodem+0xdc>
    1db0:	e12fff34 	blx	r4
    1db4:	e3a05000 	mov	r5, #0	; 0x0
    1db8:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1dbc:	e280000c 	add	r0, r0, #12	; 0xc
    1dc0:	eb003cb6 	bl	110a0 <xyzModem_stream_close>
    1dc4:	e3a00000 	mov	r0, #0	; 0x0
    1dc8:	e59f102c 	ldr	r1, [pc, #44]	; 1dfc <load_serial_ymodem+0xe0>
    1dcc:	eb003f64 	bl	11b64 <xyzModem_stream_terminate>
    1dd0:	e1a01005 	mov	r1, r5
    1dd4:	e59f0024 	ldr	r0, [pc, #36]	; 1e00 <load_serial_ymodem+0xe4>
    1dd8:	e1a02005 	mov	r2, r5
    1ddc:	e59f3010 	ldr	r3, [pc, #16]	; 1df4 <load_serial_ymodem+0xd8>
    1de0:	e1a0e00f 	mov	lr, pc
    1de4:	e593f000 	ldr	pc, [r3]
    1de8:	e1a00007 	mov	r0, r7
    1dec:	e28dde41 	add	sp, sp, #1040	; 0x410
    1df0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    1df4:	3c00d694 	.word	0x3c00d694
    1df8:	0001a0ca 	.word	0x0001a0ca
    1dfc:	00001e04 	.word	0x00001e04
    1e00:	00019ff2 	.word	0x00019ff2

00001e04 <getcxmodem>:
    1e04:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    1e08:	e24dd004 	sub	sp, sp, #4	; 0x4
    1e0c:	eb003955 	bl	10368 <tstc>
    1e10:	e3500000 	cmp	r0, #0	; 0x0
    1e14:	1a000002 	bne	1e24 <getcxmodem+0x20>
    1e18:	e3e00000 	mvn	r0, #0	; 0x0
    1e1c:	e28dd004 	add	sp, sp, #4	; 0x4
    1e20:	e8bd8000 	pop	{pc}
    1e24:	e28dd004 	add	sp, sp, #4	; 0x4
    1e28:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    1e2c:	ea00394e 	b	1036c <getc>

00001e30 <do_load_serial_bin>:
    1e30:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1e34:	e3520001 	cmp	r2, #1	; 0x1
    1e38:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1e3c:	e1a06002 	mov	r6, r2
    1e40:	e1a07000 	mov	r7, r0
    1e44:	e1a05003 	mov	r5, r3
    1e48:	ca000006 	bgt	1e68 <do_load_serial_bin+0x38>
    1e4c:	e59f0120 	ldr	r0, [pc, #288]	; 1f74 <do_load_serial_bin+0x144>
    1e50:	e3a06000 	mov	r6, #0	; 0x0
    1e54:	e59f311c 	ldr	r3, [pc, #284]	; 1f78 <do_load_serial_bin+0x148>
    1e58:	e1a0e00f 	mov	lr, pc
    1e5c:	e593f000 	ldr	pc, [r3]
    1e60:	e1a04006 	mov	r4, r6
    1e64:	ea00000c 	b	1e9c <do_load_serial_bin+0x6c>
    1e68:	e5930004 	ldr	r0, [r3, #4]
    1e6c:	e3a01000 	mov	r1, #0	; 0x0
    1e70:	e3a02010 	mov	r2, #16	; 0x10
    1e74:	eb003524 	bl	f30c <simple_strtoul>
    1e78:	e3560003 	cmp	r6, #3	; 0x3
    1e7c:	e1a04000 	mov	r4, r0
    1e80:	13a06000 	movne	r6, #0	; 0x0
    1e84:	1a000004 	bne	1e9c <do_load_serial_bin+0x6c>
    1e88:	e5950008 	ldr	r0, [r5, #8]
    1e8c:	e3a01000 	mov	r1, #0	; 0x0
    1e90:	e3a02010 	mov	r2, #16	; 0x10
    1e94:	eb00351c 	bl	f30c <simple_strtoul>
    1e98:	e1a06000 	mov	r6, r0
    1e9c:	e5950000 	ldr	r0, [r5]
    1ea0:	e59f10d4 	ldr	r1, [pc, #212]	; 1f7c <do_load_serial_bin+0x14c>
    1ea4:	ebfffd8a 	bl	14d4 <strcmp>
    1ea8:	e3500000 	cmp	r0, #0	; 0x0
    1eac:	1a000007 	bne	1ed0 <do_load_serial_bin+0xa0>
    1eb0:	e1a01004 	mov	r1, r4
    1eb4:	e59f00c4 	ldr	r0, [pc, #196]	; 1f80 <do_load_serial_bin+0x150>
    1eb8:	e59f30b8 	ldr	r3, [pc, #184]	; 1f78 <do_load_serial_bin+0x148>
    1ebc:	e1a0e00f 	mov	lr, pc
    1ec0:	e593f000 	ldr	pc, [r3]
    1ec4:	e1a00004 	mov	r0, r4
    1ec8:	ebffff93 	bl	1d1c <load_serial_ymodem>
    1ecc:	ea000025 	b	1f68 <do_load_serial_bin+0x138>
    1ed0:	e5950000 	ldr	r0, [r5]
    1ed4:	e59f10a8 	ldr	r1, [pc, #168]	; 1f84 <do_load_serial_bin+0x154>
    1ed8:	ebfffd7d 	bl	14d4 <strcmp>
    1edc:	e3500000 	cmp	r0, #0	; 0x0
    1ee0:	e59f5090 	ldr	r5, [pc, #144]	; 1f78 <do_load_serial_bin+0x148>
    1ee4:	1a00001b 	bne	1f58 <do_load_serial_bin+0x128>
    1ee8:	e1a02006 	mov	r2, r6
    1eec:	e59f0094 	ldr	r0, [pc, #148]	; 1f88 <do_load_serial_bin+0x158>
    1ef0:	e1a01004 	mov	r1, r4
    1ef4:	e1a0e00f 	mov	lr, pc
    1ef8:	e595f000 	ldr	pc, [r5]
    1efc:	e1a00004 	mov	r0, r4
    1f00:	e1a01006 	mov	r1, r6
    1f04:	eb003e79 	bl	118f0 <control_send>
    1f08:	e3500000 	cmp	r0, #0	; 0x0
    1f0c:	aa000005 	bge	1f28 <do_load_serial_bin+0xf8>
    1f10:	e59d0004 	ldr	r0, [sp, #4]
    1f14:	e5954000 	ldr	r4, [r5]
    1f18:	eb003c78 	bl	11100 <xyzModem_error>
    1f1c:	e1a01000 	mov	r1, r0
    1f20:	e59f0064 	ldr	r0, [pc, #100]	; 1f8c <do_load_serial_bin+0x15c>
    1f24:	e12fff34 	blx	r4
    1f28:	e28d0004 	add	r0, sp, #4	; 0x4
    1f2c:	eb003c5b 	bl	110a0 <xyzModem_stream_close>
    1f30:	e3a00000 	mov	r0, #0	; 0x0
    1f34:	e59f1054 	ldr	r1, [pc, #84]	; 1f90 <do_load_serial_bin+0x160>
    1f38:	eb003f09 	bl	11b64 <xyzModem_stream_terminate>
    1f3c:	e1a01006 	mov	r1, r6
    1f40:	e59f004c 	ldr	r0, [pc, #76]	; 1f94 <do_load_serial_bin+0x164>
    1f44:	e1a02006 	mov	r2, r6
    1f48:	e59f3028 	ldr	r3, [pc, #40]	; 1f78 <do_load_serial_bin+0x148>
    1f4c:	e1a0e00f 	mov	lr, pc
    1f50:	e593f000 	ldr	pc, [r3]
    1f54:	ea000003 	b	1f68 <do_load_serial_bin+0x138>
    1f58:	e5971014 	ldr	r1, [r7, #20]
    1f5c:	e59f0034 	ldr	r0, [pc, #52]	; 1f98 <do_load_serial_bin+0x168>
    1f60:	e1a0e00f 	mov	lr, pc
    1f64:	e595f000 	ldr	pc, [r5]
    1f68:	e3a00000 	mov	r0, #0	; 0x0
    1f6c:	e28dd00c 	add	sp, sp, #12	; 0xc
    1f70:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1f74:	0001a01a 	.word	0x0001a01a
    1f78:	3c00d694 	.word	0x3c00d694
    1f7c:	0001a034 	.word	0x0001a034
    1f80:	0001a037 	.word	0x0001a037
    1f84:	0001a07a 	.word	0x0001a07a
    1f88:	0001a07d 	.word	0x0001a07d
    1f8c:	0001a0ca 	.word	0x0001a0ca
    1f90:	00001e04 	.word	0x00001e04
    1f94:	00019ff2 	.word	0x00019ff2
    1f98:	0001a0c9 	.word	0x0001a0c9

00001f9c <do_mem_cp>:
    1f9c:	e3520004 	cmp	r2, #4	; 0x4
    1fa0:	e92d4070 	push	{r4, r5, r6, lr}
    1fa4:	e1a04003 	mov	r4, r3
    1fa8:	0a000002 	beq	1fb8 <do_mem_cp+0x1c>
    1fac:	eb0003e2 	bl	2f3c <cmd_usage>
    1fb0:	e3a00001 	mov	r0, #1	; 0x1
    1fb4:	e8bd8070 	pop	{r4, r5, r6, pc}
    1fb8:	e5930004 	ldr	r0, [r3, #4]
    1fbc:	e3a01000 	mov	r1, #0	; 0x0
    1fc0:	e3a02010 	mov	r2, #16	; 0x10
    1fc4:	eb0034d0 	bl	f30c <simple_strtoul>
    1fc8:	e3a01000 	mov	r1, #0	; 0x0
    1fcc:	e3a02010 	mov	r2, #16	; 0x10
    1fd0:	e1a06000 	mov	r6, r0
    1fd4:	e5940008 	ldr	r0, [r4, #8]
    1fd8:	eb0034cb 	bl	f30c <simple_strtoul>
    1fdc:	e3a01000 	mov	r1, #0	; 0x0
    1fe0:	e1a05000 	mov	r5, r0
    1fe4:	e3a02010 	mov	r2, #16	; 0x10
    1fe8:	e594000c 	ldr	r0, [r4, #12]
    1fec:	eb0034c6 	bl	f30c <simple_strtoul>
    1ff0:	e2800003 	add	r0, r0, #3	; 0x3
    1ff4:	e1b00120 	lsrs	r0, r0, #2
    1ff8:	1a000002 	bne	2008 <do_mem_cp+0x6c>
    1ffc:	ea000006 	b	201c <do_mem_cp+0x80>
    2000:	e5163004 	ldr	r3, [r6, #-4]
    2004:	e5053004 	str	r3, [r5, #-4]
    2008:	e3500000 	cmp	r0, #0	; 0x0
    200c:	e2866004 	add	r6, r6, #4	; 0x4
    2010:	e2400001 	sub	r0, r0, #1	; 0x1
    2014:	e2855004 	add	r5, r5, #4	; 0x4
    2018:	1afffff8 	bne	2000 <do_mem_cp+0x64>
    201c:	e3a00000 	mov	r0, #0	; 0x0
    2020:	e8bd8070 	pop	{r4, r5, r6, pc}

00002024 <do_mem_cmp>:
    2024:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    2028:	e3520004 	cmp	r2, #4	; 0x4
    202c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    2030:	e1a04003 	mov	r4, r3
    2034:	0a000002 	beq	2044 <do_mem_cmp+0x20>
    2038:	eb0003bf 	bl	2f3c <cmd_usage>
    203c:	e3a04001 	mov	r4, #1	; 0x1
    2040:	ea000029 	b	20ec <do_mem_cmp+0xc8>
    2044:	e5930004 	ldr	r0, [r3, #4]
    2048:	e3a01000 	mov	r1, #0	; 0x0
    204c:	e3a02010 	mov	r2, #16	; 0x10
    2050:	eb0034ad 	bl	f30c <simple_strtoul>
    2054:	e3a01000 	mov	r1, #0	; 0x0
    2058:	e3a02010 	mov	r2, #16	; 0x10
    205c:	e1a07000 	mov	r7, r0
    2060:	e5940008 	ldr	r0, [r4, #8]
    2064:	eb0034a8 	bl	f30c <simple_strtoul>
    2068:	e3a01000 	mov	r1, #0	; 0x0
    206c:	e1a06000 	mov	r6, r0
    2070:	e3a02010 	mov	r2, #16	; 0x10
    2074:	e594000c 	ldr	r0, [r4, #12]
    2078:	eb0034a3 	bl	f30c <simple_strtoul>
    207c:	e2800003 	add	r0, r0, #3	; 0x3
    2080:	e1a00120 	lsr	r0, r0, #2
    2084:	e3a05000 	mov	r5, #0	; 0x0
    2088:	ea00000f 	b	20cc <do_mem_cmp+0xa8>
    208c:	e5972000 	ldr	r2, [r7]
    2090:	e5963000 	ldr	r3, [r6]
    2094:	e1520003 	cmp	r2, r3
    2098:	0a000008 	beq	20c0 <do_mem_cmp+0x9c>
    209c:	e58d3000 	str	r3, [sp]
    20a0:	e1a01007 	mov	r1, r7
    20a4:	e1a03006 	mov	r3, r6
    20a8:	e59f0048 	ldr	r0, [pc, #72]	; 20f8 <do_mem_cmp+0xd4>
    20ac:	e59fc048 	ldr	ip, [pc, #72]	; 20fc <do_mem_cmp+0xd8>
    20b0:	e1a0e00f 	mov	lr, pc
    20b4:	e59cf000 	ldr	pc, [ip]
    20b8:	e3a04001 	mov	r4, #1	; 0x1
    20bc:	ea000005 	b	20d8 <do_mem_cmp+0xb4>
    20c0:	e2855001 	add	r5, r5, #1	; 0x1
    20c4:	e2877004 	add	r7, r7, #4	; 0x4
    20c8:	e2866004 	add	r6, r6, #4	; 0x4
    20cc:	e1500005 	cmp	r0, r5
    20d0:	1affffed 	bne	208c <do_mem_cmp+0x68>
    20d4:	e3a04000 	mov	r4, #0	; 0x0
    20d8:	e1a01005 	mov	r1, r5
    20dc:	e59f001c 	ldr	r0, [pc, #28]	; 2100 <do_mem_cmp+0xdc>
    20e0:	e59f3014 	ldr	r3, [pc, #20]	; 20fc <do_mem_cmp+0xd8>
    20e4:	e1a0e00f 	mov	lr, pc
    20e8:	e593f000 	ldr	pc, [r3]
    20ec:	e1a00004 	mov	r0, r4
    20f0:	e28dd00c 	add	sp, sp, #12	; 0xc
    20f4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    20f8:	0001a1cf 	.word	0x0001a1cf
    20fc:	3c00d694 	.word	0x3c00d694
    2100:	0001a207 	.word	0x0001a207

00002104 <do_mem_mw>:
    2104:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    2108:	e1a06002 	mov	r6, r2
    210c:	e2422003 	sub	r2, r2, #3	; 0x3
    2110:	e3520001 	cmp	r2, #1	; 0x1
    2114:	e24dd004 	sub	sp, sp, #4	; 0x4
    2118:	e1a04003 	mov	r4, r3
    211c:	9a000002 	bls	212c <do_mem_mw+0x28>
    2120:	eb000385 	bl	2f3c <cmd_usage>
    2124:	e3a00001 	mov	r0, #1	; 0x1
    2128:	ea000016 	b	2188 <do_mem_mw+0x84>
    212c:	e3a01000 	mov	r1, #0	; 0x0
    2130:	e3a02010 	mov	r2, #16	; 0x10
    2134:	e5930004 	ldr	r0, [r3, #4]
    2138:	eb003473 	bl	f30c <simple_strtoul>
    213c:	e3a01000 	mov	r1, #0	; 0x0
    2140:	e1a05000 	mov	r5, r0
    2144:	e3a02010 	mov	r2, #16	; 0x10
    2148:	e5940008 	ldr	r0, [r4, #8]
    214c:	eb00346e 	bl	f30c <simple_strtoul>
    2150:	e3560004 	cmp	r6, #4	; 0x4
    2154:	e1a07000 	mov	r7, r0
    2158:	13a00001 	movne	r0, #1	; 0x1
    215c:	1a000006 	bne	217c <do_mem_mw+0x78>
    2160:	e594000c 	ldr	r0, [r4, #12]
    2164:	e3a01000 	mov	r1, #0	; 0x0
    2168:	e3a02010 	mov	r2, #16	; 0x10
    216c:	eb003466 	bl	f30c <simple_strtoul>
    2170:	ea000001 	b	217c <do_mem_mw+0x78>
    2174:	e5057004 	str	r7, [r5, #-4]
    2178:	e2400001 	sub	r0, r0, #1	; 0x1
    217c:	e3500000 	cmp	r0, #0	; 0x0
    2180:	e2855004 	add	r5, r5, #4	; 0x4
    2184:	1afffffa 	bne	2174 <do_mem_mw+0x70>
    2188:	e28dd004 	add	sp, sp, #4	; 0x4
    218c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00002190 <do_mem_md>:
    2190:	e92d4070 	push	{r4, r5, r6, lr}
    2194:	e3520001 	cmp	r2, #1	; 0x1
    2198:	e24dd008 	sub	sp, sp, #8	; 0x8
    219c:	e1a04002 	mov	r4, r2
    21a0:	e1a05003 	mov	r5, r3
    21a4:	ca000002 	bgt	21b4 <do_mem_md+0x24>
    21a8:	eb000363 	bl	2f3c <cmd_usage>
    21ac:	e3a00001 	mov	r0, #1	; 0x1
    21b0:	ea000014 	b	2208 <do_mem_md+0x78>
    21b4:	e5930004 	ldr	r0, [r3, #4]
    21b8:	e3a01000 	mov	r1, #0	; 0x0
    21bc:	e3a02010 	mov	r2, #16	; 0x10
    21c0:	eb003451 	bl	f30c <simple_strtoul>
    21c4:	e3540002 	cmp	r4, #2	; 0x2
    21c8:	e1a06000 	mov	r6, r0
    21cc:	03a030a0 	moveq	r3, #160	; 0xa0
    21d0:	0a000004 	beq	21e8 <do_mem_md+0x58>
    21d4:	e5950008 	ldr	r0, [r5, #8]
    21d8:	e3a01000 	mov	r1, #0	; 0x0
    21dc:	e3a02010 	mov	r2, #16	; 0x10
    21e0:	eb003449 	bl	f30c <simple_strtoul>
    21e4:	e1a03000 	mov	r3, r0
    21e8:	e3a0c004 	mov	ip, #4	; 0x4
    21ec:	e1a00006 	mov	r0, r6
    21f0:	e1a0200c 	mov	r2, ip
    21f4:	e1a03123 	lsr	r3, r3, #2
    21f8:	e1a01006 	mov	r1, r6
    21fc:	e58dc000 	str	ip, [sp]
    2200:	eb0033a9 	bl	f0ac <print_buffer>
    2204:	e3a00000 	mov	r0, #0	; 0x0
    2208:	e28dd008 	add	sp, sp, #8	; 0x8
    220c:	e8bd8070 	pop	{r4, r5, r6, pc}

00002210 <do_nand>:
    2210:	e3520001 	cmp	r2, #1	; 0x1
    2214:	e92d4070 	push	{r4, r5, r6, lr}
    2218:	e1a06002 	mov	r6, r2
    221c:	e1a05003 	mov	r5, r3
    2220:	da000020 	ble	22a8 <do_nand+0x98>
    2224:	e5930004 	ldr	r0, [r3, #4]
    2228:	e59f10a0 	ldr	r1, [pc, #160]	; 22d0 <do_nand+0xc0>
    222c:	e3a02007 	mov	r2, #7	; 0x7
    2230:	ebfffcb5 	bl	150c <strncmp>
    2234:	e2504000 	subs	r4, r0, #0	; 0x0
    2238:	1a000001 	bne	2244 <do_nand+0x34>
    223c:	eb002274 	bl	ac14 <ecc_print_stat>
    2240:	ea000009 	b	226c <do_nand+0x5c>
    2244:	e5950004 	ldr	r0, [r5, #4]
    2248:	e59f1084 	ldr	r1, [pc, #132]	; 22d4 <do_nand+0xc4>
    224c:	e3a02003 	mov	r2, #3	; 0x3
    2250:	ebfffcad 	bl	150c <strncmp>
    2254:	e2504000 	subs	r4, r0, #0	; 0x0
    2258:	1a000005 	bne	2274 <do_nand+0x64>
    225c:	e3560002 	cmp	r6, #2	; 0x2
    2260:	d3a00000 	movle	r0, #0	; 0x0
    2264:	c3a00001 	movgt	r0, #1	; 0x1
    2268:	eb00223b 	bl	ab5c <base_scan_badblocks>
    226c:	e1a00004 	mov	r0, r4
    2270:	e8bd8070 	pop	{r4, r5, r6, pc}
    2274:	e5950004 	ldr	r0, [r5, #4]
    2278:	e59f1058 	ldr	r1, [pc, #88]	; 22d8 <do_nand+0xc8>
    227c:	e3a02007 	mov	r2, #7	; 0x7
    2280:	ebfffca1 	bl	150c <strncmp>
    2284:	e2504000 	subs	r4, r0, #0	; 0x0
    2288:	13a00000 	movne	r0, #0	; 0x0
    228c:	18bd8070 	popne	{r4, r5, r6, pc}
    2290:	e3560002 	cmp	r6, #2	; 0x2
    2294:	1a000005 	bne	22b0 <do_nand+0xa0>
    2298:	e59f003c 	ldr	r0, [pc, #60]	; 22dc <do_nand+0xcc>
    229c:	e59f303c 	ldr	r3, [pc, #60]	; 22e0 <do_nand+0xd0>
    22a0:	e1a0e00f 	mov	lr, pc
    22a4:	e593f000 	ldr	pc, [r3]
    22a8:	e3e00000 	mvn	r0, #0	; 0x0
    22ac:	e8bd8070 	pop	{r4, r5, r6, pc}
    22b0:	e3a02010 	mov	r2, #16	; 0x10
    22b4:	e1a01004 	mov	r1, r4
    22b8:	e5950008 	ldr	r0, [r5, #8]
    22bc:	eb003412 	bl	f30c <simple_strtoul>
    22c0:	e1a01004 	mov	r1, r4
    22c4:	eb002253 	bl	ac18 <base_block_markbad>
    22c8:	e1a00004 	mov	r0, r4
    22cc:	e8bd8070 	pop	{r4, r5, r6, pc}
    22d0:	0001a2d1 	.word	0x0001a2d1
    22d4:	0001a2dd 	.word	0x0001a2dd
    22d8:	0001a2d9 	.word	0x0001a2d9
    22dc:	0001a2e1 	.word	0x0001a2e1
    22e0:	3c00d694 	.word	0x3c00d694

000022e4 <do_sc>:
    22e4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    22e8:	e3520002 	cmp	r2, #2	; 0x2
    22ec:	e24ddf47 	sub	sp, sp, #284	; 0x11c
    22f0:	0a000002 	beq	2300 <do_sc+0x1c>
    22f4:	eb000310 	bl	2f3c <cmd_usage>
    22f8:	e3a00001 	mov	r0, #1	; 0x1
    22fc:	ea000060 	b	2484 <do_sc+0x1a0>
    2300:	e5930004 	ldr	r0, [r3, #4]
    2304:	e3a01000 	mov	r1, #0	; 0x0
    2308:	e3a02010 	mov	r2, #16	; 0x10
    230c:	eb0033fe 	bl	f30c <simple_strtoul>
    2310:	e28d4f41 	add	r4, sp, #260	; 0x104
    2314:	e2844002 	add	r4, r4, #2	; 0x2
    2318:	e59f116c 	ldr	r1, [pc, #364]	; 248c <do_sc+0x1a8>
    231c:	e1a06000 	mov	r6, r0
    2320:	e3a02012 	mov	r2, #18	; 0x12
    2324:	e1a00004 	mov	r0, r4
    2328:	ebfffcb0 	bl	15f0 <memcpy>
    232c:	e3a01000 	mov	r1, #0	; 0x0
    2330:	ea000004 	b	2348 <do_sc+0x64>
    2334:	e7d12006 	ldrb	r2, [r1, r6]
    2338:	e7d43001 	ldrb	r3, [r4, r1]
    233c:	e2811001 	add	r1, r1, #1	; 0x1
    2340:	e1520003 	cmp	r2, r3
    2344:	1a000014 	bne	239c <do_sc+0xb8>
    2348:	e3510012 	cmp	r1, #18	; 0x12
    234c:	e0815006 	add	r5, r1, r6
    2350:	1afffff7 	bne	2334 <do_sc+0x50>
    2354:	e2851eff 	add	r1, r5, #4080	; 0xff0
    2358:	e281100f 	add	r1, r1, #15	; 0xf
    235c:	e1a02005 	mov	r2, r5
    2360:	e5d23000 	ldrb	r3, [r2]
    2364:	e353000a 	cmp	r3, #10	; 0xa
    2368:	1a000005 	bne	2384 <do_sc+0xa0>
    236c:	e5d23001 	ldrb	r3, [r2, #1]
    2370:	e353005d 	cmp	r3, #93	; 0x5d
    2374:	1a000002 	bne	2384 <do_sc+0xa0>
    2378:	e5d23002 	ldrb	r3, [r2, #2]
    237c:	e353005d 	cmp	r3, #93	; 0x5d
    2380:	0a000003 	beq	2394 <do_sc+0xb0>
    2384:	e1520001 	cmp	r2, r1
    2388:	e2822001 	add	r2, r2, #1	; 0x1
    238c:	1afffff3 	bne	2360 <do_sc+0x7c>
    2390:	ea000001 	b	239c <do_sc+0xb8>
    2394:	e3550000 	cmp	r5, #0	; 0x0
    2398:	1a000006 	bne	23b8 <do_sc+0xd4>
    239c:	e59f00ec 	ldr	r0, [pc, #236]	; 2490 <do_sc+0x1ac>
    23a0:	e1a01006 	mov	r1, r6
    23a4:	e59f30e8 	ldr	r3, [pc, #232]	; 2494 <do_sc+0x1b0>
    23a8:	e1a0e00f 	mov	lr, pc
    23ac:	e593f000 	ldr	pc, [r3]
    23b0:	e3e00000 	mvn	r0, #0	; 0x0
    23b4:	ea000032 	b	2484 <do_sc+0x1a0>
    23b8:	e59f40d4 	ldr	r4, [pc, #212]	; 2494 <do_sc+0x1b0>
    23bc:	e1a01006 	mov	r1, r6
    23c0:	e59f00d0 	ldr	r0, [pc, #208]	; 2498 <do_sc+0x1b4>
    23c4:	e28d6008 	add	r6, sp, #8	; 0x8
    23c8:	e1a0e00f 	mov	lr, pc
    23cc:	e594f000 	ldr	pc, [r4]
    23d0:	e2466002 	sub	r6, r6, #2	; 0x2
    23d4:	e1a07004 	mov	r7, r4
    23d8:	ea000000 	b	23e0 <do_sc+0xfc>
    23dc:	e2855001 	add	r5, r5, #1	; 0x1
    23e0:	e5d53000 	ldrb	r3, [r5]
    23e4:	e353002d 	cmp	r3, #45	; 0x2d
    23e8:	1353000a 	cmpne	r3, #10	; 0xa
    23ec:	0afffffa 	beq	23dc <do_sc+0xf8>
    23f0:	e353005b 	cmp	r3, #91	; 0x5b
    23f4:	0afffff8 	beq	23dc <do_sc+0xf8>
    23f8:	e353005d 	cmp	r3, #93	; 0x5d
    23fc:	0a000001 	beq	2408 <do_sc+0x124>
    2400:	e1a02006 	mov	r2, r6
    2404:	ea000005 	b	2420 <do_sc+0x13c>
    2408:	e5d53001 	ldrb	r3, [r5, #1]
    240c:	e353005d 	cmp	r3, #93	; 0x5d
    2410:	1afffffa 	bne	2400 <do_sc+0x11c>
    2414:	ea00000c 	b	244c <do_sc+0x168>
    2418:	e5423001 	strb	r3, [r2, #-1]
    241c:	e2855001 	add	r5, r5, #1	; 0x1
    2420:	e5d53000 	ldrb	r3, [r5]
    2424:	e1a01002 	mov	r1, r2
    2428:	e353000a 	cmp	r3, #10	; 0xa
    242c:	1353005d 	cmpne	r3, #93	; 0x5d
    2430:	03a04000 	moveq	r4, #0	; 0x0
    2434:	13a04001 	movne	r4, #1	; 0x1
    2438:	e2822001 	add	r2, r2, #1	; 0x1
    243c:	1afffff5 	bne	2418 <do_sc+0x134>
    2440:	e3550000 	cmp	r5, #0	; 0x0
    2444:	e5c14000 	strb	r4, [r1]
    2448:	1a000005 	bne	2464 <do_sc+0x180>
    244c:	e59f0048 	ldr	r0, [pc, #72]	; 249c <do_sc+0x1b8>
    2450:	e59f303c 	ldr	r3, [pc, #60]	; 2494 <do_sc+0x1b0>
    2454:	e1a0e00f 	mov	lr, pc
    2458:	e593f000 	ldr	pc, [r3]
    245c:	e3a00000 	mov	r0, #0	; 0x0
    2460:	ea000007 	b	2484 <do_sc+0x1a0>
    2464:	e1a01006 	mov	r1, r6
    2468:	e59f0030 	ldr	r0, [pc, #48]	; 24a0 <do_sc+0x1bc>
    246c:	e1a0e00f 	mov	lr, pc
    2470:	e597f000 	ldr	pc, [r7]
    2474:	e1a00006 	mov	r0, r6
    2478:	e1a01004 	mov	r1, r4
    247c:	eb0037f6 	bl	1045c <run_command>
    2480:	eaffffd6 	b	23e0 <do_sc+0xfc>
    2484:	e28ddf47 	add	sp, sp, #284	; 0x11c
    2488:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    248c:	00019be8 	.word	0x00019be8
    2490:	0001a320 	.word	0x0001a320
    2494:	3c00d694 	.word	0x3c00d694
    2498:	0001a33d 	.word	0x0001a33d
    249c:	0001a356 	.word	0x0001a356
    24a0:	0001a363 	.word	0x0001a363

000024a4 <do_ls>:
    24a4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    24a8:	e59f001c 	ldr	r0, [pc, #28]	; 24cc <do_ls+0x28>
    24ac:	e24dd004 	sub	sp, sp, #4	; 0x4
    24b0:	e59f1018 	ldr	r1, [pc, #24]	; 24d0 <do_ls+0x2c>
    24b4:	e59f3018 	ldr	r3, [pc, #24]	; 24d4 <do_ls+0x30>
    24b8:	e1a0e00f 	mov	lr, pc
    24bc:	e593f000 	ldr	pc, [r3]
    24c0:	e3a00000 	mov	r0, #0	; 0x0
    24c4:	e28dd004 	add	sp, sp, #4	; 0x4
    24c8:	e8bd8000 	pop	{pc}
    24cc:	0001a0ca 	.word	0x0001a0ca
    24d0:	3c00d094 	.word	0x3c00d094
    24d4:	3c00d694 	.word	0x3c00d694

000024d8 <do_linkpc>:
    24d8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    24dc:	e24dd004 	sub	sp, sp, #4	; 0x4
    24e0:	eb0030e7 	bl	e884 <linkpc_loop>
    24e4:	e3a00000 	mov	r0, #0	; 0x0
    24e8:	e28dd004 	add	sp, sp, #4	; 0x4
    24ec:	e8bd8000 	pop	{pc}

000024f0 <do_pads>:
    24f0:	e3520002 	cmp	r2, #2	; 0x2
    24f4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    24f8:	d59f0194 	ldrle	r0, [pc, #404]	; 2694 <do_pads+0x1a4>
    24fc:	e1a05002 	mov	r5, r2
    2500:	e1a06003 	mov	r6, r3
    2504:	da000015 	ble	2560 <do_pads+0x70>
    2508:	e5930008 	ldr	r0, [r3, #8]
    250c:	e3a01000 	mov	r1, #0	; 0x0
    2510:	e3a0200a 	mov	r2, #10	; 0xa
    2514:	eb00337c 	bl	f30c <simple_strtoul>
    2518:	e3550003 	cmp	r5, #3	; 0x3
    251c:	e1a04000 	mov	r4, r0
    2520:	01a07000 	moveq	r7, r0
    2524:	0a000004 	beq	253c <do_pads+0x4c>
    2528:	e596000c 	ldr	r0, [r6, #12]
    252c:	e3a01000 	mov	r1, #0	; 0x0
    2530:	e3a0200a 	mov	r2, #10	; 0xa
    2534:	eb003374 	bl	f30c <simple_strtoul>
    2538:	e1a07000 	mov	r7, r0
    253c:	e1943007 	orrs	r3, r4, r7
    2540:	4a000005 	bmi	255c <do_pads+0x6c>
    2544:	e1a05004 	mov	r5, r4
    2548:	e2866004 	add	r6, r6, #4	; 0x4
    254c:	e59f9144 	ldr	r9, [pc, #324]	; 2698 <do_pads+0x1a8>
    2550:	e59f8144 	ldr	r8, [pc, #324]	; 269c <do_pads+0x1ac>
    2554:	e59fa144 	ldr	sl, [pc, #324]	; 26a0 <do_pads+0x1b0>
    2558:	ea000049 	b	2684 <do_pads+0x194>
    255c:	e59f0140 	ldr	r0, [pc, #320]	; 26a4 <do_pads+0x1b4>
    2560:	e59f3134 	ldr	r3, [pc, #308]	; 269c <do_pads+0x1ac>
    2564:	e1a0e00f 	mov	lr, pc
    2568:	e593f000 	ldr	pc, [r3]
    256c:	ea000046 	b	268c <do_pads+0x19c>
    2570:	e5960000 	ldr	r0, [r6]
    2574:	e59f111c 	ldr	r1, [pc, #284]	; 2698 <do_pads+0x1a8>
    2578:	e3a02002 	mov	r2, #2	; 0x2
    257c:	ebfffbe2 	bl	150c <strncmp>
    2580:	e3500000 	cmp	r0, #0	; 0x0
    2584:	1a00000b 	bne	25b8 <do_pads+0xc8>
    2588:	e1a02000 	mov	r2, r0
    258c:	e3a01001 	mov	r1, #1	; 0x1
    2590:	e1a00005 	mov	r0, r5
    2594:	ebfff9c6 	bl	cb4 <pads_chmod>
    2598:	e1a00005 	mov	r0, r5
    259c:	e5984000 	ldr	r4, [r8]
    25a0:	ebfffa6a 	bl	f50 <pads_getpin>
    25a4:	e1a01005 	mov	r1, r5
    25a8:	e1a02000 	mov	r2, r0
    25ac:	e59f00f4 	ldr	r0, [pc, #244]	; 26a8 <do_pads+0x1b8>
    25b0:	e12fff34 	blx	r4
    25b4:	ea000031 	b	2680 <do_pads+0x190>
    25b8:	e59f10ec 	ldr	r1, [pc, #236]	; 26ac <do_pads+0x1bc>
    25bc:	e3a02004 	mov	r2, #4	; 0x4
    25c0:	e5960000 	ldr	r0, [r6]
    25c4:	ebfffbd0 	bl	150c <strncmp>
    25c8:	e3500000 	cmp	r0, #0	; 0x0
    25cc:	01a00005 	moveq	r0, r5
    25d0:	03a01002 	moveq	r1, #2	; 0x2
    25d4:	03a02001 	moveq	r2, #1	; 0x1
    25d8:	0a000011 	beq	2624 <do_pads+0x134>
    25dc:	e59f10cc 	ldr	r1, [pc, #204]	; 26b0 <do_pads+0x1c0>
    25e0:	e3a02003 	mov	r2, #3	; 0x3
    25e4:	e5960000 	ldr	r0, [r6]
    25e8:	ebfffbc7 	bl	150c <strncmp>
    25ec:	e3500000 	cmp	r0, #0	; 0x0
    25f0:	01a02000 	moveq	r2, r0
    25f4:	03a01002 	moveq	r1, #2	; 0x2
    25f8:	01a00005 	moveq	r0, r5
    25fc:	0a000008 	beq	2624 <do_pads+0x134>
    2600:	e5960000 	ldr	r0, [r6]
    2604:	e59f10a8 	ldr	r1, [pc, #168]	; 26b4 <do_pads+0x1c4>
    2608:	e3a02004 	mov	r2, #4	; 0x4
    260c:	ebfffbbe 	bl	150c <strncmp>
    2610:	e3500000 	cmp	r0, #0	; 0x0
    2614:	1a000004 	bne	262c <do_pads+0x13c>
    2618:	e1a01000 	mov	r1, r0
    261c:	e1a02001 	mov	r2, r1
    2620:	e1a00005 	mov	r0, r5
    2624:	ebfff9a2 	bl	cb4 <pads_chmod>
    2628:	ea000014 	b	2680 <do_pads+0x190>
    262c:	e5960000 	ldr	r0, [r6]
    2630:	e59f1080 	ldr	r1, [pc, #128]	; 26b8 <do_pads+0x1c8>
    2634:	e3a02002 	mov	r2, #2	; 0x2
    2638:	ebfffbb3 	bl	150c <strncmp>
    263c:	e3500000 	cmp	r0, #0	; 0x0
    2640:	1a00000c 	bne	2678 <do_pads+0x188>
    2644:	e1a00005 	mov	r0, r5
    2648:	ebfff983 	bl	c5c <pads_states>
    264c:	e3500000 	cmp	r0, #0	; 0x0
    2650:	e5983000 	ldr	r3, [r8]
    2654:	059f2060 	ldreq	r2, [pc, #96]	; 26bc <do_pads+0x1cc>
    2658:	0a000002 	beq	2668 <do_pads+0x178>
    265c:	e3500001 	cmp	r0, #1	; 0x1
    2660:	01a02009 	moveq	r2, r9
    2664:	11a0200a 	movne	r2, sl
    2668:	e59f0050 	ldr	r0, [pc, #80]	; 26c0 <do_pads+0x1d0>
    266c:	e1a01005 	mov	r1, r5
    2670:	e12fff33 	blx	r3
    2674:	ea000001 	b	2680 <do_pads+0x190>
    2678:	e59f0044 	ldr	r0, [pc, #68]	; 26c4 <do_pads+0x1d4>
    267c:	eaffffb7 	b	2560 <do_pads+0x70>
    2680:	e2855001 	add	r5, r5, #1	; 0x1
    2684:	e1550007 	cmp	r5, r7
    2688:	daffffb8 	ble	2570 <do_pads+0x80>
    268c:	e3a00000 	mov	r0, #0	; 0x0
    2690:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    2694:	0001a387 	.word	0x0001a387
    2698:	0001a3c4 	.word	0x0001a3c4
    269c:	3c00d694 	.word	0x3c00d694
    26a0:	0001c4ff 	.word	0x0001c4ff
    26a4:	0001a39f 	.word	0x0001a39f
    26a8:	0001a3c7 	.word	0x0001a3c7
    26ac:	0001a3db 	.word	0x0001a3db
    26b0:	0001a3e0 	.word	0x0001a3e0
    26b4:	0001a3e4 	.word	0x0001a3e4
    26b8:	0001a3e9 	.word	0x0001a3e9
    26bc:	0001a3ec 	.word	0x0001a3ec
    26c0:	0001a3f5 	.word	0x0001a3f5
    26c4:	0001a40a 	.word	0x0001a40a

000026c8 <do_mux>:
    26c8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    26cc:	e3520001 	cmp	r2, #1	; 0x1
    26d0:	e24dd004 	sub	sp, sp, #4	; 0x4
    26d4:	da000005 	ble	26f0 <do_mux+0x28>
    26d8:	e5930004 	ldr	r0, [r3, #4]
    26dc:	e3a01000 	mov	r1, #0	; 0x0
    26e0:	e3a0200a 	mov	r2, #10	; 0xa
    26e4:	eb003308 	bl	f30c <simple_strtoul>
    26e8:	e6ff0070 	uxth	r0, r0
    26ec:	ebfffb4a 	bl	141c <set_mux>
    26f0:	e59f301c 	ldr	r3, [pc, #28]	; 2714 <do_mux+0x4c>
    26f4:	e59f001c 	ldr	r0, [pc, #28]	; 2718 <do_mux+0x50>
    26f8:	e5931000 	ldr	r1, [r3]
    26fc:	e59f3018 	ldr	r3, [pc, #24]	; 271c <do_mux+0x54>
    2700:	e1a0e00f 	mov	lr, pc
    2704:	e593f000 	ldr	pc, [r3]
    2708:	e3a00000 	mov	r0, #0	; 0x0
    270c:	e28dd004 	add	sp, sp, #4	; 0x4
    2710:	e8bd8000 	pop	{pc}
    2714:	21e09000 	.word	0x21e09000
    2718:	0001a423 	.word	0x0001a423
    271c:	3c00d694 	.word	0x3c00d694

00002720 <do_mac>:
    2720:	e59f3020 	ldr	r3, [pc, #32]	; 2748 <do_mac+0x28>
    2724:	e92d4010 	push	{r4, lr}
    2728:	e5934000 	ldr	r4, [r3]
    272c:	ebfff8d8 	bl	a94 <efuse_mac>
    2730:	e1a02000 	mov	r2, r0
    2734:	e1a03001 	mov	r3, r1
    2738:	e59f000c 	ldr	r0, [pc, #12]	; 274c <do_mac+0x2c>
    273c:	e12fff34 	blx	r4
    2740:	e3a00000 	mov	r0, #0	; 0x0
    2744:	e8bd8010 	pop	{r4, pc}
    2748:	3c00d694 	.word	0x3c00d694
    274c:	0001a435 	.word	0x0001a435

00002750 <do_boot>:
    2750:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2754:	e3a00000 	mov	r0, #0	; 0x0
    2758:	e24dd004 	sub	sp, sp, #4	; 0x4
    275c:	ebfff72b 	bl	410 <try_boot>
    2760:	e3a00000 	mov	r0, #0	; 0x0
    2764:	e28dd004 	add	sp, sp, #4	; 0x4
    2768:	e8bd8000 	pop	{pc}

0000276c <do_efuse>:
    276c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2770:	e3520001 	cmp	r2, #1	; 0x1
    2774:	e24dd004 	sub	sp, sp, #4	; 0x4
    2778:	da000006 	ble	2798 <do_efuse+0x2c>
    277c:	e5930004 	ldr	r0, [r3, #4]
    2780:	e59f1024 	ldr	r1, [pc, #36]	; 27ac <do_efuse+0x40>
    2784:	e3a02002 	mov	r2, #2	; 0x2
    2788:	ebfffb5f 	bl	150c <strncmp>
    278c:	e3500000 	cmp	r0, #0	; 0x0
    2790:	02800001 	addeq	r0, r0, #1	; 0x1
    2794:	0a000000 	beq	279c <do_efuse+0x30>
    2798:	e3a00000 	mov	r0, #0	; 0x0
    279c:	ebfff8bf 	bl	aa0 <efuse_info>
    27a0:	e3a00000 	mov	r0, #0	; 0x0
    27a4:	e28dd004 	add	sp, sp, #4	; 0x4
    27a8:	e8bd8000 	pop	{pc}
    27ac:	0001a44d 	.word	0x0001a44d

000027b0 <do_pi>:
    27b0:	e92d4030 	push	{r4, r5, lr}
    27b4:	e3520001 	cmp	r2, #1	; 0x1
    27b8:	e24dd004 	sub	sp, sp, #4	; 0x4
    27bc:	e1a04002 	mov	r4, r2
    27c0:	e1a05003 	mov	r5, r3
    27c4:	da000015 	ble	2820 <do_pi+0x70>
    27c8:	e59f10ac 	ldr	r1, [pc, #172]	; 287c <do_pi+0xcc>
    27cc:	e5930004 	ldr	r0, [r3, #4]
    27d0:	e3a02004 	mov	r2, #4	; 0x4
    27d4:	ebfffb4c 	bl	150c <strncmp>
    27d8:	e2501000 	subs	r1, r0, #0	; 0x0
    27dc:	1a00000d 	bne	2818 <do_pi+0x68>
    27e0:	e3540002 	cmp	r4, #2	; 0x2
    27e4:	01a04001 	moveq	r4, r1
    27e8:	0a000003 	beq	27fc <do_pi+0x4c>
    27ec:	e5950008 	ldr	r0, [r5, #8]
    27f0:	e3a02010 	mov	r2, #16	; 0x10
    27f4:	eb0032c4 	bl	f30c <simple_strtoul>
    27f8:	e1a04000 	mov	r4, r0
    27fc:	e59f007c 	ldr	r0, [pc, #124]	; 2880 <do_pi+0xd0>
    2800:	e1a01004 	mov	r1, r4
    2804:	e59f3078 	ldr	r3, [pc, #120]	; 2884 <do_pi+0xd4>
    2808:	e1a0e00f 	mov	lr, pc
    280c:	e593f000 	ldr	pc, [r3]
    2810:	e1a00004 	mov	r0, r4
    2814:	eb0015aa 	bl	7ec4 <burn>
    2818:	e3a04000 	mov	r4, #0	; 0x0
    281c:	ea000013 	b	2870 <do_pi+0xc0>
    2820:	e3a00481 	mov	r0, #-2130706432	; 0x81000000
    2824:	eb00364c 	bl	1015c <ius_check_header>
    2828:	e2504000 	subs	r4, r0, #0	; 0x0
    282c:	159f0054 	ldrne	r0, [pc, #84]	; 2888 <do_pi+0xd8>
    2830:	1a000005 	bne	284c <do_pi+0x9c>
    2834:	e3a0000d 	mov	r0, #13	; 0xd
    2838:	e3a01001 	mov	r1, #1	; 0x1
    283c:	eb000f4d 	bl	6578 <vs_assign_by_id>
    2840:	e2504000 	subs	r4, r0, #0	; 0x0
    2844:	0a000005 	beq	2860 <do_pi+0xb0>
    2848:	e59f003c 	ldr	r0, [pc, #60]	; 288c <do_pi+0xdc>
    284c:	e1a01004 	mov	r1, r4
    2850:	e59f302c 	ldr	r3, [pc, #44]	; 2884 <do_pi+0xd4>
    2854:	e1a0e00f 	mov	lr, pc
    2858:	e593f000 	ldr	pc, [r3]
    285c:	ea000003 	b	2870 <do_pi+0xc0>
    2860:	e3a00481 	mov	r0, #-2130706432	; 0x81000000
    2864:	e3a0100d 	mov	r1, #13	; 0xd
    2868:	e1a02004 	mov	r2, r4
    286c:	eb00150c 	bl	7ca4 <burn_images>
    2870:	e1a00004 	mov	r0, r4
    2874:	e28dd004 	add	sp, sp, #4	; 0x4
    2878:	e8bd8030 	pop	{r4, r5, pc}
    287c:	0001a450 	.word	0x0001a450
    2880:	0001a455 	.word	0x0001a455
    2884:	3c00d694 	.word	0x3c00d694
    2888:	0001a47f 	.word	0x0001a47f
    288c:	0001a49f 	.word	0x0001a49f

00002890 <do_spbyte>:
    2890:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2894:	e5930004 	ldr	r0, [r3, #4]
    2898:	e24dd004 	sub	sp, sp, #4	; 0x4
    289c:	e3a01000 	mov	r1, #0	; 0x0
    28a0:	e3a0200a 	mov	r2, #10	; 0xa
    28a4:	eb003298 	bl	f30c <simple_strtoul>
    28a8:	eb002589 	bl	bed4 <spi_set_bytes>
    28ac:	e3a00000 	mov	r0, #0	; 0x0
    28b0:	e28dd004 	add	sp, sp, #4	; 0x4
    28b4:	e8bd8000 	pop	{pc}

000028b8 <do_cfg>:
    28b8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    28bc:	e5930004 	ldr	r0, [r3, #4]
    28c0:	e24dd004 	sub	sp, sp, #4	; 0x4
    28c4:	e3a01000 	mov	r1, #0	; 0x0
    28c8:	e3a02010 	mov	r2, #16	; 0x10
    28cc:	eb00328e 	bl	f30c <simple_strtoul>
    28d0:	eb00139a 	bl	7740 <cdbg_config_isi>
    28d4:	e3a00000 	mov	r0, #0	; 0x0
    28d8:	e28dd004 	add	sp, sp, #4	; 0x4
    28dc:	e8bd8000 	pop	{pc}

000028e0 <do_sleep>:
    28e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    28e4:	e3a01000 	mov	r1, #0	; 0x0
    28e8:	e24dd004 	sub	sp, sp, #4	; 0x4
    28ec:	e3a0200a 	mov	r2, #10	; 0xa
    28f0:	e5930004 	ldr	r0, [r3, #4]
    28f4:	eb003284 	bl	f30c <simple_strtoul>
    28f8:	e59f3020 	ldr	r3, [pc, #32]	; 2920 <do_sleep+0x40>
    28fc:	e0000093 	mul	r0, r3, r0
    2900:	eb002303 	bl	b514 <udelay>
    2904:	e59f0018 	ldr	r0, [pc, #24]	; 2924 <do_sleep+0x44>
    2908:	e59f3018 	ldr	r3, [pc, #24]	; 2928 <do_sleep+0x48>
    290c:	e1a0e00f 	mov	lr, pc
    2910:	e593f000 	ldr	pc, [r3]
    2914:	e3a00000 	mov	r0, #0	; 0x0
    2918:	e28dd004 	add	sp, sp, #4	; 0x4
    291c:	e8bd8000 	pop	{pc}
    2920:	000f4240 	.word	0x000f4240
    2924:	0001a4ba 	.word	0x0001a4ba
    2928:	3c00d694 	.word	0x3c00d694

0000292c <do_go>:
    292c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2930:	e5930004 	ldr	r0, [r3, #4]
    2934:	e24dd004 	sub	sp, sp, #4	; 0x4
    2938:	e3a02010 	mov	r2, #16	; 0x10
    293c:	e3a01000 	mov	r1, #0	; 0x0
    2940:	eb003271 	bl	f30c <simple_strtoul>
    2944:	e3a01000 	mov	r1, #0	; 0x0
    2948:	eb0012e9 	bl	74f4 <cdbg_jump>
    294c:	e3a00000 	mov	r0, #0	; 0x0
    2950:	e28dd004 	add	sp, sp, #4	; 0x4
    2954:	e8bd8000 	pop	{pc}

00002958 <do_ml>:
    2958:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    295c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2960:	ebfffc18 	bl	19c8 <__sOINFo>
    2964:	e3a00000 	mov	r0, #0	; 0x0
    2968:	e28dd004 	add	sp, sp, #4	; 0x4
    296c:	e8bd8000 	pop	{pc}

00002970 <do_bp>:
    2970:	e92d4030 	push	{r4, r5, lr}
    2974:	e3520001 	cmp	r2, #1	; 0x1
    2978:	e24dd004 	sub	sp, sp, #4	; 0x4
    297c:	da000005 	ble	2998 <do_bp+0x28>
    2980:	e5930004 	ldr	r0, [r3, #4]
    2984:	e3a01000 	mov	r1, #0	; 0x0
    2988:	e3a02010 	mov	r2, #16	; 0x10
    298c:	eb00325e 	bl	f30c <simple_strtoul>
    2990:	e6ff0070 	uxth	r0, r0
    2994:	ebfff704 	bl	5ac <set_xom>
    2998:	e59f5034 	ldr	r5, [pc, #52]	; 29d4 <do_bp+0x64>
    299c:	e3a00001 	mov	r0, #1	; 0x1
    29a0:	e5954000 	ldr	r4, [r5]
    29a4:	ebfff6fc 	bl	59c <get_xom>
    29a8:	e1a01000 	mov	r1, r0
    29ac:	e59f0024 	ldr	r0, [pc, #36]	; 29d8 <do_bp+0x68>
    29b0:	e12fff34 	blx	r4
    29b4:	e5954000 	ldr	r4, [r5]
    29b8:	ebfff701 	bl	5c4 <boot_info>
    29bc:	e1a01000 	mov	r1, r0
    29c0:	e59f0014 	ldr	r0, [pc, #20]	; 29dc <do_bp+0x6c>
    29c4:	e12fff34 	blx	r4
    29c8:	e3a00000 	mov	r0, #0	; 0x0
    29cc:	e28dd004 	add	sp, sp, #4	; 0x4
    29d0:	e8bd8030 	pop	{r4, r5, pc}
    29d4:	3c00d694 	.word	0x3c00d694
    29d8:	0001a4c5 	.word	0x0001a4c5
    29dc:	0001a4db 	.word	0x0001a4db

000029e0 <do_reset>:
    29e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    29e4:	e24dd004 	sub	sp, sp, #4	; 0x4
    29e8:	ebfff7a5 	bl	884 <system_reset>
    29ec:	e3a00000 	mov	r0, #0	; 0x0
    29f0:	e28dd004 	add	sp, sp, #4	; 0x4
    29f4:	e8bd8000 	pop	{pc}

000029f8 <output_debug>:
    29f8:	e24dd010 	sub	sp, sp, #16	; 0x10
    29fc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2a00:	e24dd004 	sub	sp, sp, #4	; 0x4
    2a04:	e28dc008 	add	ip, sp, #8	; 0x8
    2a08:	e3700002 	cmn	r0, #2	; 0x2
    2a0c:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    2a10:	e1a01000 	mov	r1, r0
    2a14:	0a000012 	beq	2a64 <output_debug+0x6c>
    2a18:	e3700001 	cmn	r0, #1	; 0x1
    2a1c:	0a000008 	beq	2a44 <output_debug+0x4c>
    2a20:	e3700003 	cmn	r0, #3	; 0x3
    2a24:	1a000010 	bne	2a6c <output_debug+0x74>
    2a28:	e59f30a4 	ldr	r3, [pc, #164]	; 2ad4 <output_debug+0xdc>
    2a2c:	e59f00a4 	ldr	r0, [pc, #164]	; 2ad8 <output_debug+0xe0>
    2a30:	e5931000 	ldr	r1, [r3]
    2a34:	e59f30a0 	ldr	r3, [pc, #160]	; 2adc <output_debug+0xe4>
    2a38:	e1a0e00f 	mov	lr, pc
    2a3c:	e593f000 	ldr	pc, [r3]
    2a40:	ea00001e 	b	2ac0 <output_debug+0xc8>
    2a44:	e59f3088 	ldr	r3, [pc, #136]	; 2ad4 <output_debug+0xdc>
    2a48:	e59f0090 	ldr	r0, [pc, #144]	; 2ae0 <output_debug+0xe8>
    2a4c:	e5932000 	ldr	r2, [r3]
    2a50:	e28d1010 	add	r1, sp, #16	; 0x10
    2a54:	e59f3080 	ldr	r3, [pc, #128]	; 2adc <output_debug+0xe4>
    2a58:	e1a0e00f 	mov	lr, pc
    2a5c:	e593f000 	ldr	pc, [r3]
    2a60:	ea000016 	b	2ac0 <output_debug+0xc8>
    2a64:	e59f0078 	ldr	r0, [pc, #120]	; 2ae4 <output_debug+0xec>
    2a68:	ea000003 	b	2a7c <output_debug+0x84>
    2a6c:	e59d300c 	ldr	r3, [sp, #12]
    2a70:	e3530001 	cmp	r3, #1	; 0x1
    2a74:	1a000004 	bne	2a8c <output_debug+0x94>
    2a78:	e59f0068 	ldr	r0, [pc, #104]	; 2ae8 <output_debug+0xf0>
    2a7c:	e59f3058 	ldr	r3, [pc, #88]	; 2adc <output_debug+0xe4>
    2a80:	e1a0e00f 	mov	lr, pc
    2a84:	e593f000 	ldr	pc, [r3]
    2a88:	ea00000c 	b	2ac0 <output_debug+0xc8>
    2a8c:	e3530002 	cmp	r3, #2	; 0x2
    2a90:	e59fc044 	ldr	ip, [pc, #68]	; 2adc <output_debug+0xe4>
    2a94:	1a000003 	bne	2aa8 <output_debug+0xb0>
    2a98:	e59f004c 	ldr	r0, [pc, #76]	; 2aec <output_debug+0xf4>
    2a9c:	e1a0e00f 	mov	lr, pc
    2aa0:	e59cf000 	ldr	pc, [ip]
    2aa4:	ea000005 	b	2ac0 <output_debug+0xc8>
    2aa8:	e59f3024 	ldr	r3, [pc, #36]	; 2ad4 <output_debug+0xdc>
    2aac:	e59f003c 	ldr	r0, [pc, #60]	; 2af0 <output_debug+0xf8>
    2ab0:	e5933000 	ldr	r3, [r3]
    2ab4:	e28d2010 	add	r2, sp, #16	; 0x10
    2ab8:	e1a0e00f 	mov	lr, pc
    2abc:	e59cf000 	ldr	pc, [ip]
    2ac0:	e3a00000 	mov	r0, #0	; 0x0
    2ac4:	e28dd004 	add	sp, sp, #4	; 0x4
    2ac8:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    2acc:	e28dd010 	add	sp, sp, #16	; 0x10
    2ad0:	e12fff1e 	bx	lr
    2ad4:	3c00d138 	.word	0x3c00d138
    2ad8:	0001a666 	.word	0x0001a666
    2adc:	3c00d694 	.word	0x3c00d694
    2ae0:	0001a68e 	.word	0x0001a68e
    2ae4:	0001a69d 	.word	0x0001a69d
    2ae8:	0001a6b7 	.word	0x0001a6b7
    2aec:	0001a6c2 	.word	0x0001a6c2
    2af0:	0001a6d0 	.word	0x0001a6d0

00002af4 <do_vs>:
    2af4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    2af8:	e3520006 	cmp	r2, #6	; 0x6
    2afc:	e24dd024 	sub	sp, sp, #36	; 0x24
    2b00:	e1a06000 	mov	r6, r0
    2b04:	e1a04003 	mov	r4, r3
    2b08:	1a000005 	bne	2b24 <do_vs+0x30>
    2b0c:	e5930014 	ldr	r0, [r3, #20]
    2b10:	e3a01000 	mov	r1, #0	; 0x0
    2b14:	e282200a 	add	r2, r2, #10	; 0xa
    2b18:	eb0031fb 	bl	f30c <simple_strtoul>
    2b1c:	e1a05000 	mov	r5, r0
    2b20:	ea00008f 	b	2d64 <do_vs+0x270>
    2b24:	e2423001 	sub	r3, r2, #1	; 0x1
    2b28:	e3530005 	cmp	r3, #5	; 0x5
    2b2c:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    2b30:	ea0000c4 	b	2e48 <do_vs+0x354>
    2b34:	00002e48 	.word	0x00002e48
    2b38:	00002b4c 	.word	0x00002b4c
    2b3c:	00002c44 	.word	0x00002c44
    2b40:	00002cb4 	.word	0x00002cb4
    2b44:	00002d60 	.word	0x00002d60
    2b48:	00002d60 	.word	0x00002d60
    2b4c:	e5940004 	ldr	r0, [r4, #4]
    2b50:	e59f1310 	ldr	r1, [pc, #784]	; 2e68 <do_vs+0x374>
    2b54:	ebfffa5e 	bl	14d4 <strcmp>
    2b58:	e2505000 	subs	r5, r0, #0	; 0x0
    2b5c:	1a000025 	bne	2bf8 <do_vs+0x104>
    2b60:	e59f4304 	ldr	r4, [pc, #772]	; 2e6c <do_vs+0x378>
    2b64:	e59f0304 	ldr	r0, [pc, #772]	; 2e70 <do_vs+0x37c>
    2b68:	e1a0e00f 	mov	lr, pc
    2b6c:	e594f000 	ldr	pc, [r4]
    2b70:	e3a0802c 	mov	r8, #44	; 0x2c
    2b74:	e59f72f8 	ldr	r7, [pc, #760]	; 2e74 <do_vs+0x380>
    2b78:	ea000003 	b	2b8c <do_vs+0x98>
    2b7c:	e0227598 	mla	r2, r8, r5, r7
    2b80:	e1a0e00f 	mov	lr, pc
    2b84:	e594f000 	ldr	pc, [r4]
    2b88:	e2855001 	add	r5, r5, #1	; 0x1
    2b8c:	eb000da4 	bl	6224 <vs_device_count>
    2b90:	e1550000 	cmp	r5, r0
    2b94:	e1a01005 	mov	r1, r5
    2b98:	e59f02d8 	ldr	r0, [pc, #728]	; 2e78 <do_vs+0x384>
    2b9c:	e59f62c8 	ldr	r6, [pc, #712]	; 2e6c <do_vs+0x378>
    2ba0:	bafffff5 	blt	2b7c <do_vs+0x88>
    2ba4:	e59f02d0 	ldr	r0, [pc, #720]	; 2e7c <do_vs+0x388>
    2ba8:	e1a0e00f 	mov	lr, pc
    2bac:	e596f000 	ldr	pc, [r6]
    2bb0:	eb000ca1 	bl	5e3c <vs_is_assigned>
    2bb4:	e2504000 	subs	r4, r0, #0	; 0x0
    2bb8:	0a000009 	beq	2be4 <do_vs+0xf0>
    2bbc:	e59f32bc 	ldr	r3, [pc, #700]	; 2e80 <do_vs+0x38c>
    2bc0:	e5965000 	ldr	r5, [r6]
    2bc4:	e5934000 	ldr	r4, [r3]
    2bc8:	e1a00004 	mov	r0, r4
    2bcc:	eb000d15 	bl	6028 <vs_align>
    2bd0:	e1a01004 	mov	r1, r4
    2bd4:	e1a02000 	mov	r2, r0
    2bd8:	e59f02a4 	ldr	r0, [pc, #676]	; 2e84 <do_vs+0x390>
    2bdc:	e12fff35 	blx	r5
    2be0:	ea00009d 	b	2e5c <do_vs+0x368>
    2be4:	e59f029c 	ldr	r0, [pc, #668]	; 2e88 <do_vs+0x394>
    2be8:	e1a0e00f 	mov	lr, pc
    2bec:	e596f000 	ldr	pc, [r6]
    2bf0:	e1a00004 	mov	r0, r4
    2bf4:	ea000099 	b	2e60 <do_vs+0x36c>
    2bf8:	e5940004 	ldr	r0, [r4, #4]
    2bfc:	e59f1288 	ldr	r1, [pc, #648]	; 2e8c <do_vs+0x398>
    2c00:	ebfffa33 	bl	14d4 <strcmp>
    2c04:	e2505000 	subs	r5, r0, #0	; 0x0
    2c08:	1a000087 	bne	2e2c <do_vs+0x338>
    2c0c:	eb000e4a 	bl	653c <vs_reset>
    2c10:	e28d4020 	add	r4, sp, #32	; 0x20
    2c14:	e5240018 	str	r0, [r4, #-24]!
    2c18:	e59f126c 	ldr	r1, [pc, #620]	; 2e8c <do_vs+0x398>
    2c1c:	e2840008 	add	r0, r4, #8	; 0x8
    2c20:	ebfffa01 	bl	142c <strcpy>
    2c24:	e3a03001 	mov	r3, #1	; 0x1
    2c28:	e58d300c 	str	r3, [sp, #12]
    2c2c:	e28d3018 	add	r3, sp, #24	; 0x18
    2c30:	e8930003 	ldm	r3, {r0, r1}
    2c34:	e88d0003 	stm	sp, {r0, r1}
    2c38:	e894000f 	ldm	r4, {r0, r1, r2, r3}
    2c3c:	ebffff6d 	bl	29f8 <output_debug>
    2c40:	ea000019 	b	2cac <do_vs+0x1b8>
    2c44:	e5940004 	ldr	r0, [r4, #4]
    2c48:	e59f1240 	ldr	r1, [pc, #576]	; 2e90 <do_vs+0x39c>
    2c4c:	ebfffa20 	bl	14d4 <strcmp>
    2c50:	e2505000 	subs	r5, r0, #0	; 0x0
    2c54:	1a000074 	bne	2e2c <do_vs+0x338>
    2c58:	e5940008 	ldr	r0, [r4, #8]
    2c5c:	e3a01001 	mov	r1, #1	; 0x1
    2c60:	eb000e62 	bl	65f0 <vs_assign_by_name>
    2c64:	e2507000 	subs	r7, r0, #0	; 0x0
    2c68:	e59f61fc 	ldr	r6, [pc, #508]	; 2e6c <do_vs+0x378>
    2c6c:	1a00000b 	bne	2ca0 <do_vs+0x1ac>
    2c70:	e5943008 	ldr	r3, [r4, #8]
    2c74:	e59f2218 	ldr	r2, [pc, #536]	; 2e94 <do_vs+0x3a0>
    2c78:	e1a00003 	mov	r0, r3
    2c7c:	e5823000 	str	r3, [r2]
    2c80:	eb000d69 	bl	622c <vs_device_id>
    2c84:	e59f31f4 	ldr	r3, [pc, #500]	; 2e80 <do_vs+0x38c>
    2c88:	e5830000 	str	r0, [r3]
    2c8c:	e59f0204 	ldr	r0, [pc, #516]	; 2e98 <do_vs+0x3a4>
    2c90:	e1a0e00f 	mov	lr, pc
    2c94:	e596f000 	ldr	pc, [r6]
    2c98:	e1a00007 	mov	r0, r7
    2c9c:	ea00006f 	b	2e60 <do_vs+0x36c>
    2ca0:	e59f01f4 	ldr	r0, [pc, #500]	; 2e9c <do_vs+0x3a8>
    2ca4:	e1a0e00f 	mov	lr, pc
    2ca8:	e596f000 	ldr	pc, [r6]
    2cac:	e1a00005 	mov	r0, r5
    2cb0:	ea00006a 	b	2e60 <do_vs+0x36c>
    2cb4:	e3a02010 	mov	r2, #16	; 0x10
    2cb8:	e5940008 	ldr	r0, [r4, #8]
    2cbc:	e3a01000 	mov	r1, #0	; 0x0
    2cc0:	eb0031e9 	bl	f46c <simple_strtoull>
    2cc4:	e3a02010 	mov	r2, #16	; 0x10
    2cc8:	e1a07000 	mov	r7, r0
    2ccc:	e1a08001 	mov	r8, r1
    2cd0:	e594000c 	ldr	r0, [r4, #12]
    2cd4:	e3a01000 	mov	r1, #0	; 0x0
    2cd8:	eb0031e3 	bl	f46c <simple_strtoull>
    2cdc:	e59f11bc 	ldr	r1, [pc, #444]	; 2ea0 <do_vs+0x3ac>
    2ce0:	e1a05000 	mov	r5, r0
    2ce4:	e28d0010 	add	r0, sp, #16	; 0x10
    2ce8:	ebfff9cf 	bl	142c <strcpy>
    2cec:	e3a03002 	mov	r3, #2	; 0x2
    2cf0:	e5940004 	ldr	r0, [r4, #4]
    2cf4:	e59f11a4 	ldr	r1, [pc, #420]	; 2ea0 <do_vs+0x3ac>
    2cf8:	e58d300c 	str	r3, [sp, #12]
    2cfc:	ebfff9f4 	bl	14d4 <strcmp>
    2d00:	e3500000 	cmp	r0, #0	; 0x0
    2d04:	1a000004 	bne	2d1c <do_vs+0x228>
    2d08:	e1a00007 	mov	r0, r7
    2d0c:	e1a01008 	mov	r1, r8
    2d10:	e1a02005 	mov	r2, r5
    2d14:	eb000c87 	bl	5f38 <vs_erase>
    2d18:	ea000008 	b	2d40 <do_vs+0x24c>
    2d1c:	e5940004 	ldr	r0, [r4, #4]
    2d20:	e59f117c 	ldr	r1, [pc, #380]	; 2ea4 <do_vs+0x3b0>
    2d24:	ebfff9ea 	bl	14d4 <strcmp>
    2d28:	e3500000 	cmp	r0, #0	; 0x0
    2d2c:	1a00003e 	bne	2e2c <do_vs+0x338>
    2d30:	e1a00007 	mov	r0, r7
    2d34:	e1a01008 	mov	r1, r8
    2d38:	e1a02005 	mov	r2, r5
    2d3c:	eb000c9b 	bl	5fb0 <vs_scrub>
    2d40:	e58d0008 	str	r0, [sp, #8]
    2d44:	e28d2018 	add	r2, sp, #24	; 0x18
    2d48:	e8920003 	ldm	r2, {r0, r1}
    2d4c:	e88d0003 	stm	sp, {r0, r1}
    2d50:	e28d3008 	add	r3, sp, #8	; 0x8
    2d54:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    2d58:	ebffff26 	bl	29f8 <output_debug>
    2d5c:	ea00003e 	b	2e5c <do_vs+0x368>
    2d60:	e3a05000 	mov	r5, #0	; 0x0
    2d64:	e3a01000 	mov	r1, #0	; 0x0
    2d68:	e3a02010 	mov	r2, #16	; 0x10
    2d6c:	e5940008 	ldr	r0, [r4, #8]
    2d70:	eb003165 	bl	f30c <simple_strtoul>
    2d74:	e3a02010 	mov	r2, #16	; 0x10
    2d78:	e1a09000 	mov	r9, r0
    2d7c:	e3a01000 	mov	r1, #0	; 0x0
    2d80:	e594000c 	ldr	r0, [r4, #12]
    2d84:	eb0031b8 	bl	f46c <simple_strtoull>
    2d88:	e3a02010 	mov	r2, #16	; 0x10
    2d8c:	e1a08000 	mov	r8, r0
    2d90:	e1a0a001 	mov	sl, r1
    2d94:	e5940010 	ldr	r0, [r4, #16]
    2d98:	e3a01000 	mov	r1, #0	; 0x0
    2d9c:	e08db002 	add	fp, sp, r2
    2da0:	eb0031b1 	bl	f46c <simple_strtoull>
    2da4:	e3a03000 	mov	r3, #0	; 0x0
    2da8:	e59f10f8 	ldr	r1, [pc, #248]	; 2ea8 <do_vs+0x3b4>
    2dac:	e1a07000 	mov	r7, r0
    2db0:	e1a0000b 	mov	r0, fp
    2db4:	e58d300c 	str	r3, [sp, #12]
    2db8:	ebfff99b 	bl	142c <strcpy>
    2dbc:	e5940004 	ldr	r0, [r4, #4]
    2dc0:	e59f10e4 	ldr	r1, [pc, #228]	; 2eac <do_vs+0x3b8>
    2dc4:	ebfff9c2 	bl	14d4 <strcmp>
    2dc8:	e3500000 	cmp	r0, #0	; 0x0
    2dcc:	1a000006 	bne	2dec <do_vs+0x2f8>
    2dd0:	e1a00009 	mov	r0, r9
    2dd4:	e1a02008 	mov	r2, r8
    2dd8:	e1a0300a 	mov	r3, sl
    2ddc:	e58d7000 	str	r7, [sp]
    2de0:	e58d5004 	str	r5, [sp, #4]
    2de4:	eb000c3d 	bl	5ee0 <vs_write>
    2de8:	eaffffd4 	b	2d40 <do_vs+0x24c>
    2dec:	e5940004 	ldr	r0, [r4, #4]
    2df0:	e59f10b8 	ldr	r1, [pc, #184]	; 2eb0 <do_vs+0x3bc>
    2df4:	ebfff9b6 	bl	14d4 <strcmp>
    2df8:	e3500000 	cmp	r0, #0	; 0x0
    2dfc:	1a00000a 	bne	2e2c <do_vs+0x338>
    2e00:	e1a00009 	mov	r0, r9
    2e04:	e1a02008 	mov	r2, r8
    2e08:	e1a0300a 	mov	r3, sl
    2e0c:	e58d7000 	str	r7, [sp]
    2e10:	e58d5004 	str	r5, [sp, #4]
    2e14:	eb000d31 	bl	62e0 <vs_read>
    2e18:	e59f1094 	ldr	r1, [pc, #148]	; 2eb4 <do_vs+0x3c0>
    2e1c:	e58d0008 	str	r0, [sp, #8]
    2e20:	e1a0000b 	mov	r0, fp
    2e24:	ebfff980 	bl	142c <strcpy>
    2e28:	eaffffc5 	b	2d44 <do_vs+0x250>
    2e2c:	e59f0084 	ldr	r0, [pc, #132]	; 2eb8 <do_vs+0x3c4>
    2e30:	e5961014 	ldr	r1, [r6, #20]
    2e34:	e59f3030 	ldr	r3, [pc, #48]	; 2e6c <do_vs+0x378>
    2e38:	e1a0e00f 	mov	lr, pc
    2e3c:	e593f000 	ldr	pc, [r3]
    2e40:	e3e00000 	mvn	r0, #0	; 0x0
    2e44:	ea000005 	b	2e60 <do_vs+0x36c>
    2e48:	e5901014 	ldr	r1, [r0, #20]
    2e4c:	e59f3018 	ldr	r3, [pc, #24]	; 2e6c <do_vs+0x378>
    2e50:	e59f0060 	ldr	r0, [pc, #96]	; 2eb8 <do_vs+0x3c4>
    2e54:	e1a0e00f 	mov	lr, pc
    2e58:	e593f000 	ldr	pc, [r3]
    2e5c:	e3a00000 	mov	r0, #0	; 0x0
    2e60:	e28dd024 	add	sp, sp, #36	; 0x24
    2e64:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    2e68:	0001a50a 	.word	0x0001a50a
    2e6c:	3c00d694 	.word	0x3c00d694
    2e70:	0001a6e7 	.word	0x0001a6e7
    2e74:	00013190 	.word	0x00013190
    2e78:	0001a6f5 	.word	0x0001a6f5
    2e7c:	0001a6fd 	.word	0x0001a6fd
    2e80:	3c00d138 	.word	0x3c00d138
    2e84:	0001a70f 	.word	0x0001a70f
    2e88:	0001a719 	.word	0x0001a719
    2e8c:	0001a4f8 	.word	0x0001a4f8
    2e90:	0001c2d3 	.word	0x0001c2d3
    2e94:	3c00d13c 	.word	0x3c00d13c
    2e98:	0001a72c 	.word	0x0001a72c
    2e9c:	0001a738 	.word	0x0001a738
    2ea0:	0001a765 	.word	0x0001a765
    2ea4:	0001a76b 	.word	0x0001a76b
    2ea8:	0001a771 	.word	0x0001a771
    2eac:	0001a77a 	.word	0x0001a77a
    2eb0:	0001a780 	.word	0x0001a780
    2eb4:	0001a785 	.word	0x0001a785
    2eb8:	0001a0c9 	.word	0x0001a0c9

00002ebc <cmd_get_data_size>:
    2ebc:	e92d4030 	push	{r4, r5, lr}
    2ec0:	e24dd004 	sub	sp, sp, #4	; 0x4
    2ec4:	e1a04001 	mov	r4, r1
    2ec8:	e1a05000 	mov	r5, r0
    2ecc:	ebfff9ad 	bl	1588 <strlen>
    2ed0:	e3500002 	cmp	r0, #2	; 0x2
    2ed4:	da000015 	ble	2f30 <cmd_get_data_size+0x74>
    2ed8:	e0850000 	add	r0, r5, r0
    2edc:	e5503002 	ldrb	r3, [r0, #-2]
    2ee0:	e353002e 	cmp	r3, #46	; 0x2e
    2ee4:	1a000011 	bne	2f30 <cmd_get_data_size+0x74>
    2ee8:	e5500001 	ldrb	r0, [r0, #-1]
    2eec:	e350006c 	cmp	r0, #108	; 0x6c
    2ef0:	03a04004 	moveq	r4, #4	; 0x4
    2ef4:	0a00000d 	beq	2f30 <cmd_get_data_size+0x74>
    2ef8:	8a000002 	bhi	2f08 <cmd_get_data_size+0x4c>
    2efc:	e3500062 	cmp	r0, #98	; 0x62
    2f00:	1a000009 	bne	2f2c <cmd_get_data_size+0x70>
    2f04:	ea000006 	b	2f24 <cmd_get_data_size+0x68>
    2f08:	e3500073 	cmp	r0, #115	; 0x73
    2f0c:	03e04001 	mvneq	r4, #1	; 0x1
    2f10:	0a000006 	beq	2f30 <cmd_get_data_size+0x74>
    2f14:	e3500077 	cmp	r0, #119	; 0x77
    2f18:	03a04002 	moveq	r4, #2	; 0x2
    2f1c:	1a000002 	bne	2f2c <cmd_get_data_size+0x70>
    2f20:	ea000002 	b	2f30 <cmd_get_data_size+0x74>
    2f24:	e3a04001 	mov	r4, #1	; 0x1
    2f28:	ea000000 	b	2f30 <cmd_get_data_size+0x74>
    2f2c:	e3e04000 	mvn	r4, #0	; 0x0
    2f30:	e1a00004 	mov	r0, r4
    2f34:	e28dd004 	add	sp, sp, #4	; 0x4
    2f38:	e8bd8030 	pop	{r4, r5, pc}

00002f3c <cmd_usage>:
    2f3c:	e92d4030 	push	{r4, r5, lr}
    2f40:	e1a04000 	mov	r4, r0
    2f44:	e24dd004 	sub	sp, sp, #4	; 0x4
    2f48:	e59f5054 	ldr	r5, [pc, #84]	; 2fa4 <cmd_usage+0x68>
    2f4c:	e59f0054 	ldr	r0, [pc, #84]	; 2fa8 <cmd_usage+0x6c>
    2f50:	e5941000 	ldr	r1, [r4]
    2f54:	e5942010 	ldr	r2, [r4, #16]
    2f58:	e1a0e00f 	mov	lr, pc
    2f5c:	e595f000 	ldr	pc, [r5]
    2f60:	e59f0044 	ldr	r0, [pc, #68]	; 2fac <cmd_usage+0x70>
    2f64:	e5941000 	ldr	r1, [r4]
    2f68:	e1a0e00f 	mov	lr, pc
    2f6c:	e595f000 	ldr	pc, [r5]
    2f70:	e5940014 	ldr	r0, [r4, #20]
    2f74:	e3500000 	cmp	r0, #0	; 0x0
    2f78:	1a000003 	bne	2f8c <cmd_usage+0x50>
    2f7c:	e59f002c 	ldr	r0, [pc, #44]	; 2fb0 <cmd_usage+0x74>
    2f80:	eb0034cf 	bl	102c4 <puts>
    2f84:	e3a00001 	mov	r0, #1	; 0x1
    2f88:	ea000003 	b	2f9c <cmd_usage+0x60>
    2f8c:	eb0034cc 	bl	102c4 <puts>
    2f90:	e3a0000a 	mov	r0, #10	; 0xa
    2f94:	eb0034f2 	bl	10364 <putc>
    2f98:	e3a00000 	mov	r0, #0	; 0x0
    2f9c:	e28dd004 	add	sp, sp, #4	; 0x4
    2fa0:	e8bd8030 	pop	{r4, r5, pc}
    2fa4:	3c00d694 	.word	0x3c00d694
    2fa8:	0001a87a 	.word	0x0001a87a
    2fac:	0001a884 	.word	0x0001a884
    2fb0:	0001a88f 	.word	0x0001a88f

00002fb4 <find_cmd_tbl>:
    2fb4:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    2fb8:	e1a05001 	mov	r5, r1
    2fbc:	e24dd004 	sub	sp, sp, #4	; 0x4
    2fc0:	e3a0102e 	mov	r1, #46	; 0x2e
    2fc4:	e1a07000 	mov	r7, r0
    2fc8:	e1a08002 	mov	r8, r2
    2fcc:	ebfff962 	bl	155c <strchr>
    2fd0:	e3500000 	cmp	r0, #0	; 0x0
    2fd4:	10676000 	rsbne	r6, r7, r0
    2fd8:	1a000002 	bne	2fe8 <find_cmd_tbl+0x34>
    2fdc:	e1a00007 	mov	r0, r7
    2fe0:	ebfff968 	bl	1588 <strlen>
    2fe4:	e1a06000 	mov	r6, r0
    2fe8:	e3a03018 	mov	r3, #24	; 0x18
    2fec:	e02a5893 	mla	sl, r3, r8, r5
    2ff0:	e1a04005 	mov	r4, r5
    2ff4:	e1a08005 	mov	r8, r5
    2ff8:	e3a05000 	mov	r5, #0	; 0x0
    2ffc:	ea00000a 	b	302c <find_cmd_tbl+0x78>
    3000:	e5941000 	ldr	r1, [r4]
    3004:	ebfff940 	bl	150c <strncmp>
    3008:	e3500000 	cmp	r0, #0	; 0x0
    300c:	1a000005 	bne	3028 <find_cmd_tbl+0x74>
    3010:	e5940000 	ldr	r0, [r4]
    3014:	ebfff95b 	bl	1588 <strlen>
    3018:	e1560000 	cmp	r6, r0
    301c:	0a000009 	beq	3048 <find_cmd_tbl+0x94>
    3020:	e2855001 	add	r5, r5, #1	; 0x1
    3024:	e1a08004 	mov	r8, r4
    3028:	e2844018 	add	r4, r4, #24	; 0x18
    302c:	e154000a 	cmp	r4, sl
    3030:	e1a02006 	mov	r2, r6
    3034:	e1a00007 	mov	r0, r7
    3038:	1afffff0 	bne	3000 <find_cmd_tbl+0x4c>
    303c:	e3550001 	cmp	r5, #1	; 0x1
    3040:	01a04008 	moveq	r4, r8
    3044:	13a04000 	movne	r4, #0	; 0x0
    3048:	e1a00004 	mov	r0, r4
    304c:	e28dd004 	add	sp, sp, #4	; 0x4
    3050:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00003054 <find_cmd>:
    3054:	e59f1014 	ldr	r1, [pc, #20]	; 3070 <find_cmd+0x1c>
    3058:	e59f2014 	ldr	r2, [pc, #20]	; 3074 <find_cmd+0x20>
    305c:	e59f3014 	ldr	r3, [pc, #20]	; 3078 <find_cmd+0x24>
    3060:	e0612002 	rsb	r2, r1, r2
    3064:	e1a021c2 	asr	r2, r2, #3
    3068:	e0020293 	mul	r2, r3, r2
    306c:	eaffffd0 	b	2fb4 <find_cmd_tbl>
    3070:	0001c640 	.word	0x0001c640
    3074:	0001c9b8 	.word	0x0001c9b8
    3078:	aaaaaaab 	.word	0xaaaaaaab

0000307c <_do_help>:
    307c:	e1a0c00d 	mov	ip, sp
    3080:	e92ddff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}
    3084:	e24cb004 	sub	fp, ip, #4	; 0x4
    3088:	e24dd004 	sub	sp, sp, #4	; 0x4
    308c:	e59b8004 	ldr	r8, [fp, #4]
    3090:	e1a07000 	mov	r7, r0
    3094:	e3580001 	cmp	r8, #1	; 0x1
    3098:	e1a0a001 	mov	sl, r1
    309c:	159b5008 	ldrne	r5, [fp, #8]
    30a0:	13a06001 	movne	r6, #1	; 0x1
    30a4:	13a04000 	movne	r4, #0	; 0x0
    30a8:	159f9158 	ldrne	r9, [pc, #344]	; 3208 <_do_help+0x18c>
    30ac:	1a00004e 	bne	31ec <_do_help+0x170>
    30b0:	e1a03101 	lsl	r3, r1, #2
    30b4:	e283300e 	add	r3, r3, #14	; 0xe
    30b8:	e3c33007 	bic	r3, r3, #7	; 0x7
    30bc:	e1a0900d 	mov	r9, sp
    30c0:	e063d00d 	rsb	sp, r3, sp
    30c4:	e1a0800d 	mov	r8, sp
    30c8:	e3a03000 	mov	r3, #0	; 0x0
    30cc:	ea000002 	b	30dc <_do_help+0x60>
    30d0:	e7880103 	str	r0, [r8, r3, lsl #2]
    30d4:	e2800018 	add	r0, r0, #24	; 0x18
    30d8:	e2833001 	add	r3, r3, #1	; 0x1
    30dc:	e153000a 	cmp	r3, sl
    30e0:	bafffffa 	blt	30d0 <_do_help+0x54>
    30e4:	e24a7001 	sub	r7, sl, #1	; 0x1
    30e8:	ea000015 	b	3144 <_do_help+0xc8>
    30ec:	e5943000 	ldr	r3, [r4]
    30f0:	e5942004 	ldr	r2, [r4, #4]
    30f4:	e5930000 	ldr	r0, [r3]
    30f8:	e5921000 	ldr	r1, [r2]
    30fc:	ebfff8f4 	bl	14d4 <strcmp>
    3100:	e3500000 	cmp	r0, #0	; 0x0
    3104:	c5942000 	ldrgt	r2, [r4]
    3108:	c5943004 	ldrgt	r3, [r4, #4]
    310c:	e2855001 	add	r5, r5, #1	; 0x1
    3110:	c5843000 	strgt	r3, [r4]
    3114:	c5842004 	strgt	r2, [r4, #4]
    3118:	c2866001 	addgt	r6, r6, #1	; 0x1
    311c:	e1550007 	cmp	r5, r7
    3120:	e2844004 	add	r4, r4, #4	; 0x4
    3124:	bafffff0 	blt	30ec <_do_help+0x70>
    3128:	e3560000 	cmp	r6, #0	; 0x0
    312c:	1a000003 	bne	3140 <_do_help+0xc4>
    3130:	e3a05000 	mov	r5, #0	; 0x0
    3134:	e1a04005 	mov	r4, r5
    3138:	e59f70c8 	ldr	r7, [pc, #200]	; 3208 <_do_help+0x18c>
    313c:	ea000016 	b	319c <_do_help+0x120>
    3140:	e2477001 	sub	r7, r7, #1	; 0x1
    3144:	e3570000 	cmp	r7, #0	; 0x0
    3148:	dafffff8 	ble	3130 <_do_help+0xb4>
    314c:	e3a05000 	mov	r5, #0	; 0x0
    3150:	e1a04008 	mov	r4, r8
    3154:	e1a06005 	mov	r6, r5
    3158:	eaffffe3 	b	30ec <_do_help+0x70>
    315c:	e7983004 	ldr	r3, [r8, r4]
    3160:	e5936010 	ldr	r6, [r3, #16]
    3164:	eb003481 	bl	10370 <ctrlc>
    3168:	e3500000 	cmp	r0, #0	; 0x0
    316c:	e1a03006 	mov	r3, r6
    3170:	e3a01008 	mov	r1, #8	; 0x8
    3174:	e59f0090 	ldr	r0, [pc, #144]	; 320c <_do_help+0x190>
    3178:	13a04001 	movne	r4, #1	; 0x1
    317c:	1a00000a 	bne	31ac <_do_help+0x130>
    3180:	e3560000 	cmp	r6, #0	; 0x0
    3184:	0a000003 	beq	3198 <_do_help+0x11c>
    3188:	e7982004 	ldr	r2, [r8, r4]
    318c:	e5922000 	ldr	r2, [r2]
    3190:	e1a0e00f 	mov	lr, pc
    3194:	e597f000 	ldr	pc, [r7]
    3198:	e2844004 	add	r4, r4, #4	; 0x4
    319c:	e155000a 	cmp	r5, sl
    31a0:	e2855001 	add	r5, r5, #1	; 0x1
    31a4:	baffffec 	blt	315c <_do_help+0xe0>
    31a8:	e3a04000 	mov	r4, #0	; 0x0
    31ac:	e1a0d009 	mov	sp, r9
    31b0:	ea000011 	b	31fc <_do_help+0x180>
    31b4:	e5950004 	ldr	r0, [r5, #4]
    31b8:	ebffff7d 	bl	2fb4 <find_cmd_tbl>
    31bc:	e3500000 	cmp	r0, #0	; 0x0
    31c0:	0a000002 	beq	31d0 <_do_help+0x154>
    31c4:	ebffff5c 	bl	2f3c <cmd_usage>
    31c8:	e1844000 	orr	r4, r4, r0
    31cc:	ea000004 	b	31e4 <_do_help+0x168>
    31d0:	e5951004 	ldr	r1, [r5, #4]
    31d4:	e59f0034 	ldr	r0, [pc, #52]	; 3210 <_do_help+0x194>
    31d8:	e1a0e00f 	mov	lr, pc
    31dc:	e599f000 	ldr	pc, [r9]
    31e0:	e3a04001 	mov	r4, #1	; 0x1
    31e4:	e2866001 	add	r6, r6, #1	; 0x1
    31e8:	e2855004 	add	r5, r5, #4	; 0x4
    31ec:	e1560008 	cmp	r6, r8
    31f0:	e1a01007 	mov	r1, r7
    31f4:	e1a0200a 	mov	r2, sl
    31f8:	baffffed 	blt	31b4 <_do_help+0x138>
    31fc:	e1a00004 	mov	r0, r4
    3200:	e24bd028 	sub	sp, fp, #40	; 0x28
    3204:	e89daff0 	ldm	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
    3208:	3c00d694 	.word	0x3c00d694
    320c:	0001a8b0 	.word	0x0001a8b0
    3210:	0001a8ba 	.word	0x0001a8ba

00003214 <do_help>:
    3214:	e92d4030 	push	{r4, r5, lr}
    3218:	e59fc038 	ldr	ip, [pc, #56]	; 3258 <do_help+0x44>
    321c:	e59fe038 	ldr	lr, [pc, #56]	; 325c <do_help+0x48>
    3220:	e1a05001 	mov	r5, r1
    3224:	e06ec00c 	rsb	ip, lr, ip
    3228:	e59f1030 	ldr	r1, [pc, #48]	; 3260 <do_help+0x4c>
    322c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    3230:	e1a04000 	mov	r4, r0
    3234:	e1a0c1cc 	asr	ip, ip, #3
    3238:	e88d000c 	stm	sp, {r2, r3}
    323c:	e0010c91 	mul	r1, r1, ip
    3240:	e1a0000e 	mov	r0, lr
    3244:	e1a02004 	mov	r2, r4
    3248:	e1a03005 	mov	r3, r5
    324c:	ebffff8a 	bl	307c <_do_help>
    3250:	e28dd00c 	add	sp, sp, #12	; 0xc
    3254:	e8bd8030 	pop	{r4, r5, pc}
    3258:	0001c9b8 	.word	0x0001c9b8
    325c:	0001c640 	.word	0x0001c640
    3260:	aaaaaaab 	.word	0xaaaaaaab

00003264 <tt_blk_trolling>:
    3264:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3268:	e3a01001 	mov	r1, #1	; 0x1
    326c:	e24dd02c 	sub	sp, sp, #44	; 0x2c
    3270:	e1a04002 	mov	r4, r2
    3274:	e1a05003 	mov	r5, r3
    3278:	e58d0010 	str	r0, [sp, #16]
    327c:	e59da050 	ldr	sl, [sp, #80]
    3280:	eb000cda 	bl	65f0 <vs_assign_by_name>
    3284:	e3500000 	cmp	r0, #0	; 0x0
    3288:	0a000005 	beq	32a4 <tt_blk_trolling+0x40>
    328c:	e59d1010 	ldr	r1, [sp, #16]
    3290:	e59f03d4 	ldr	r0, [pc, #980]	; 366c <tt_blk_trolling+0x408>
    3294:	e59f33d4 	ldr	r3, [pc, #980]	; 3670 <tt_blk_trolling+0x40c>
    3298:	e1a0e00f 	mov	lr, pc
    329c:	e593f000 	ldr	pc, [r3]
    32a0:	ea000026 	b	3340 <tt_blk_trolling+0xdc>
    32a4:	e1940005 	orrs	r0, r4, r5
    32a8:	03a04502 	moveq	r4, #8388608	; 0x800000
    32ac:	03a06502 	moveq	r6, #8388608	; 0x800000
    32b0:	03a07000 	moveq	r7, #0	; 0x0
    32b4:	0a00000b 	beq	32e8 <tt_blk_trolling+0x84>
    32b8:	e3550000 	cmp	r5, #0	; 0x0
    32bc:	1a000006 	bne	32dc <tt_blk_trolling+0x78>
    32c0:	e3540502 	cmp	r4, #8388608	; 0x800000
    32c4:	2a000004 	bcs	32dc <tt_blk_trolling+0x78>
    32c8:	e3a0000e 	mov	r0, #14	; 0xe
    32cc:	eb000b55 	bl	6028 <vs_align>
    32d0:	e1a06004 	mov	r6, r4
    32d4:	e1a07005 	mov	r7, r5
    32d8:	ea000002 	b	32e8 <tt_blk_trolling+0x84>
    32dc:	e1a06004 	mov	r6, r4
    32e0:	e1a07005 	mov	r7, r5
    32e4:	e3a04502 	mov	r4, #8388608	; 0x800000
    32e8:	e3a0000e 	mov	r0, #14	; 0xe
    32ec:	eb000b4d 	bl	6028 <vs_align>
    32f0:	e1a01000 	mov	r1, r0
    32f4:	e1a00004 	mov	r0, r4
    32f8:	eb003c07 	bl	1231c <__aeabi_uidiv>
    32fc:	e1a01000 	mov	r1, r0
    3300:	e3a00000 	mov	r0, #0	; 0x0
    3304:	eb00054e 	bl	4844 <tt_rnd_get_int>
    3308:	e1a04000 	mov	r4, r0
    330c:	e3a0000e 	mov	r0, #14	; 0xe
    3310:	eb000b44 	bl	6028 <vs_align>
    3314:	e0040094 	mul	r4, r4, r0
    3318:	e28d0024 	add	r0, sp, #36	; 0x24
    331c:	e1a01004 	mov	r1, r4
    3320:	e1a02004 	mov	r2, r4
    3324:	eb00056f 	bl	48e8 <tt_rnd_get_data>
    3328:	e250b000 	subs	fp, r0, #0	; 0x0
    332c:	1a000005 	bne	3348 <tt_blk_trolling+0xe4>
    3330:	e59f033c 	ldr	r0, [pc, #828]	; 3674 <tt_blk_trolling+0x410>
    3334:	e59f3334 	ldr	r3, [pc, #820]	; 3670 <tt_blk_trolling+0x40c>
    3338:	e1a0e00f 	mov	lr, pc
    333c:	e593f000 	ldr	pc, [r3]
    3340:	e3e00000 	mvn	r0, #0	; 0x0
    3344:	ea0000c6 	b	3664 <tt_blk_trolling+0x400>
    3348:	e59d3024 	ldr	r3, [sp, #36]
    334c:	e3a0000e 	mov	r0, #14	; 0xe
    3350:	e0564003 	subs	r4, r6, r3
    3354:	e0c75fc3 	sbc	r5, r7, r3, asr #31
    3358:	eb000b32 	bl	6028 <vs_align>
    335c:	e3550000 	cmp	r5, #0	; 0x0
    3360:	e1cd41f8 	strd	r4, [sp, #24]
    3364:	1a000005 	bne	3380 <tt_blk_trolling+0x11c>
    3368:	e1a01000 	mov	r1, r0
    336c:	e1a00004 	mov	r0, r4
    3370:	eb003be9 	bl	1231c <__aeabi_uidiv>
    3374:	e58d501c 	str	r5, [sp, #28]
    3378:	e58d0018 	str	r0, [sp, #24]
    337c:	ea000002 	b	338c <tt_blk_trolling+0x128>
    3380:	e1a01000 	mov	r1, r0
    3384:	e28d0018 	add	r0, sp, #24	; 0x18
    3388:	eb002fa2 	bl	f218 <__div64_32>
    338c:	e59d1018 	ldr	r1, [sp, #24]
    3390:	e3a00000 	mov	r0, #0	; 0x0
    3394:	eb00052a 	bl	4844 <tt_rnd_get_int>
    3398:	e1a04000 	mov	r4, r0
    339c:	e3a0000e 	mov	r0, #14	; 0xe
    33a0:	eb000b20 	bl	6028 <vs_align>
    33a4:	e0c98490 	smull	r8, r9, r0, r4
    33a8:	e59d1010 	ldr	r1, [sp, #16]
    33ac:	e1a0200b 	mov	r2, fp
    33b0:	e59d3024 	ldr	r3, [sp, #36]
    33b4:	e59f02bc 	ldr	r0, [pc, #700]	; 3678 <tt_blk_trolling+0x414>
    33b8:	e88d0300 	stm	sp, {r8, r9}
    33bc:	e59f42ac 	ldr	r4, [pc, #684]	; 3670 <tt_blk_trolling+0x40c>
    33c0:	e1a0e00f 	mov	lr, pc
    33c4:	e594f000 	ldr	pc, [r4]
    33c8:	e59d0010 	ldr	r0, [sp, #16]
    33cc:	eb000b96 	bl	622c <vs_device_id>
    33d0:	eb000b81 	bl	61dc <vs_device_erasable>
    33d4:	e3500000 	cmp	r0, #0	; 0x0
    33d8:	0a000006 	beq	33f8 <tt_blk_trolling+0x194>
    33dc:	e59f0298 	ldr	r0, [pc, #664]	; 367c <tt_blk_trolling+0x418>
    33e0:	e1a0e00f 	mov	lr, pc
    33e4:	e594f000 	ldr	pc, [r4]
    33e8:	e1a00008 	mov	r0, r8
    33ec:	e1a01009 	mov	r1, r9
    33f0:	e59d2024 	ldr	r2, [sp, #36]
    33f4:	eb000acf 	bl	5f38 <vs_erase>
    33f8:	e59f0280 	ldr	r0, [pc, #640]	; 3680 <tt_blk_trolling+0x41c>
    33fc:	e59f326c 	ldr	r3, [pc, #620]	; 3670 <tt_blk_trolling+0x40c>
    3400:	e1a0e00f 	mov	lr, pc
    3404:	e593f000 	ldr	pc, [r3]
    3408:	e35a0000 	cmp	sl, #0	; 0x0
    340c:	13a07000 	movne	r7, #0	; 0x0
    3410:	159d6024 	ldrne	r6, [sp, #36]
    3414:	1a00001c 	bne	348c <tt_blk_trolling+0x228>
    3418:	ea00001f 	b	349c <tt_blk_trolling+0x238>
    341c:	e3560902 	cmp	r6, #32768	; 0x8000
    3420:	b1a01006 	movlt	r1, r6
    3424:	a3a01902 	movge	r1, #32768	; 0x8000
    3428:	e58d100c 	str	r1, [sp, #12]
    342c:	e59d1024 	ldr	r1, [sp, #36]
    3430:	e59d200c 	ldr	r2, [sp, #12]
    3434:	e0661001 	rsb	r1, r6, r1
    3438:	e08b1001 	add	r1, fp, r1
    343c:	ebfff86b 	bl	15f0 <memcpy>
    3440:	e3a00000 	mov	r0, #0	; 0x0
    3444:	eb002056 	bl	b5a4 <get_timer>
    3448:	e59d3024 	ldr	r3, [sp, #36]
    344c:	e3a01000 	mov	r1, #0	; 0x0
    3450:	e0663003 	rsb	r3, r6, r3
    3454:	e0984003 	adds	r4, r8, r3
    3458:	e0a95fc3 	adc	r5, r9, r3, asr #31
    345c:	e1a03005 	mov	r3, r5
    3460:	e59d500c 	ldr	r5, [sp, #12]
    3464:	e58d0008 	str	r0, [sp, #8]
    3468:	e1a02004 	mov	r2, r4
    346c:	e1a0000a 	mov	r0, sl
    3470:	e58d1004 	str	r1, [sp, #4]
    3474:	e58d5000 	str	r5, [sp]
    3478:	eb000a98 	bl	5ee0 <vs_write>
    347c:	e59d0008 	ldr	r0, [sp, #8]
    3480:	eb002047 	bl	b5a4 <get_timer>
    3484:	e0656006 	rsb	r6, r5, r6
    3488:	e0877000 	add	r7, r7, r0
    348c:	e3560000 	cmp	r6, #0	; 0x0
    3490:	e1a0000a 	mov	r0, sl
    3494:	1affffe0 	bne	341c <tt_blk_trolling+0x1b8>
    3498:	ea00000b 	b	34cc <tt_blk_trolling+0x268>
    349c:	e1a0000a 	mov	r0, sl
    34a0:	eb00203f 	bl	b5a4 <get_timer>
    34a4:	e59d1024 	ldr	r1, [sp, #36]
    34a8:	e1a04000 	mov	r4, r0
    34ac:	e1a02008 	mov	r2, r8
    34b0:	e1a03009 	mov	r3, r9
    34b4:	e1a0000b 	mov	r0, fp
    34b8:	e88d0402 	stm	sp, {r1, sl}
    34bc:	eb000a87 	bl	5ee0 <vs_write>
    34c0:	e1a00004 	mov	r0, r4
    34c4:	eb002036 	bl	b5a4 <get_timer>
    34c8:	e1a07000 	mov	r7, r0
    34cc:	e59d0024 	ldr	r0, [sp, #36]
    34d0:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    34d4:	e1a00540 	asr	r0, r0, #10
    34d8:	e1a01007 	mov	r1, r7
    34dc:	e0000093 	mul	r0, r3, r0
    34e0:	eb003b8d 	bl	1231c <__aeabi_uidiv>
    34e4:	e59f3184 	ldr	r3, [pc, #388]	; 3670 <tt_blk_trolling+0x40c>
    34e8:	e58d0014 	str	r0, [sp, #20]
    34ec:	e59f0190 	ldr	r0, [pc, #400]	; 3684 <tt_blk_trolling+0x420>
    34f0:	e1a0e00f 	mov	lr, pc
    34f4:	e593f000 	ldr	pc, [r3]
    34f8:	e35a0000 	cmp	sl, #0	; 0x0
    34fc:	13a07000 	movne	r7, #0	; 0x0
    3500:	159d6024 	ldrne	r6, [sp, #36]
    3504:	1a00001b 	bne	3578 <tt_blk_trolling+0x314>
    3508:	ea00001e 	b	3588 <tt_blk_trolling+0x324>
    350c:	eb002024 	bl	b5a4 <get_timer>
    3510:	e59d3024 	ldr	r3, [sp, #36]
    3514:	e3560902 	cmp	r6, #32768	; 0x8000
    3518:	b1a04006 	movlt	r4, r6
    351c:	a3a04902 	movge	r4, #32768	; 0x8000
    3520:	e0663003 	rsb	r3, r6, r3
    3524:	e1a05000 	mov	r5, r0
    3528:	e0980003 	adds	r0, r8, r3
    352c:	e0a91fc3 	adc	r1, r9, r3, asr #31
    3530:	e1a02000 	mov	r2, r0
    3534:	e1a03001 	mov	r3, r1
    3538:	e1a0000a 	mov	r0, sl
    353c:	e3a01000 	mov	r1, #0	; 0x0
    3540:	e58d1004 	str	r1, [sp, #4]
    3544:	e58d4000 	str	r4, [sp]
    3548:	eb000b64 	bl	62e0 <vs_read>
    354c:	e1a00005 	mov	r0, r5
    3550:	eb002013 	bl	b5a4 <get_timer>
    3554:	e1a05000 	mov	r5, r0
    3558:	e59d0024 	ldr	r0, [sp, #36]
    355c:	e1a0100a 	mov	r1, sl
    3560:	e0660000 	rsb	r0, r6, r0
    3564:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    3568:	e1a02004 	mov	r2, r4
    356c:	ebfff81f 	bl	15f0 <memcpy>
    3570:	e0877005 	add	r7, r7, r5
    3574:	e0646006 	rsb	r6, r4, r6
    3578:	e3a00000 	mov	r0, #0	; 0x0
    357c:	e1560000 	cmp	r6, r0
    3580:	1affffe1 	bne	350c <tt_blk_trolling+0x2a8>
    3584:	ea00000b 	b	35b8 <tt_blk_trolling+0x354>
    3588:	e1a0000a 	mov	r0, sl
    358c:	eb002004 	bl	b5a4 <get_timer>
    3590:	e59d1024 	ldr	r1, [sp, #36]
    3594:	e1a04000 	mov	r4, r0
    3598:	e1a02008 	mov	r2, r8
    359c:	e1a03009 	mov	r3, r9
    35a0:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    35a4:	e88d0402 	stm	sp, {r1, sl}
    35a8:	eb000b4c 	bl	62e0 <vs_read>
    35ac:	e1a00004 	mov	r0, r4
    35b0:	eb001ffb 	bl	b5a4 <get_timer>
    35b4:	e1a07000 	mov	r7, r0
    35b8:	e59d0024 	ldr	r0, [sp, #36]
    35bc:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    35c0:	e1a00540 	asr	r0, r0, #10
    35c4:	e0000093 	mul	r0, r3, r0
    35c8:	e1a01007 	mov	r1, r7
    35cc:	eb003b52 	bl	1231c <__aeabi_uidiv>
    35d0:	e59f4098 	ldr	r4, [pc, #152]	; 3670 <tt_blk_trolling+0x40c>
    35d4:	e1a05000 	mov	r5, r0
    35d8:	e59f00a8 	ldr	r0, [pc, #168]	; 3688 <tt_blk_trolling+0x424>
    35dc:	e1a0e00f 	mov	lr, pc
    35e0:	e594f000 	ldr	pc, [r4]
    35e4:	e1a0000b 	mov	r0, fp
    35e8:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    35ec:	e59d2024 	ldr	r2, [sp, #36]
    35f0:	e3a0300c 	mov	r3, #12	; 0xc
    35f4:	eb00043e 	bl	46f4 <tt_cmp_buffer>
    35f8:	e3500000 	cmp	r0, #0	; 0x0
    35fc:	0a000007 	beq	3620 <tt_blk_trolling+0x3bc>
    3600:	e58d0000 	str	r0, [sp]
    3604:	e59d1010 	ldr	r1, [sp, #16]
    3608:	e59d2014 	ldr	r2, [sp, #20]
    360c:	e1a03005 	mov	r3, r5
    3610:	e59f0074 	ldr	r0, [pc, #116]	; 368c <tt_blk_trolling+0x428>
    3614:	e1a0e00f 	mov	lr, pc
    3618:	e594f000 	ldr	pc, [r4]
    361c:	ea000005 	b	3638 <tt_blk_trolling+0x3d4>
    3620:	e59d1010 	ldr	r1, [sp, #16]
    3624:	e59d2014 	ldr	r2, [sp, #20]
    3628:	e1a03005 	mov	r3, r5
    362c:	e59f005c 	ldr	r0, [pc, #92]	; 3690 <tt_blk_trolling+0x42c>
    3630:	e1a0e00f 	mov	lr, pc
    3634:	e594f000 	ldr	pc, [r4]
    3638:	e59f0054 	ldr	r0, [pc, #84]	; 3694 <tt_blk_trolling+0x430>
    363c:	e59d1024 	ldr	r1, [sp, #36]
    3640:	e890000c 	ldm	r0, {r2, r3}
    3644:	e0922001 	adds	r2, r2, r1
    3648:	e0a33fc1 	adc	r3, r3, r1, asr #31
    364c:	e880000c 	stm	r0, {r2, r3}
    3650:	e59f0040 	ldr	r0, [pc, #64]	; 3698 <tt_blk_trolling+0x434>
    3654:	e59f3014 	ldr	r3, [pc, #20]	; 3670 <tt_blk_trolling+0x40c>
    3658:	e1a0e00f 	mov	lr, pc
    365c:	e593f000 	ldr	pc, [r3]
    3660:	e3a00000 	mov	r0, #0	; 0x0
    3664:	e28dd02c 	add	sp, sp, #44	; 0x2c
    3668:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    366c:	0001aa61 	.word	0x0001aa61
    3670:	3c00d694 	.word	0x3c00d694
    3674:	0001aa7f 	.word	0x0001aa7f
    3678:	0001aa9f 	.word	0x0001aa9f
    367c:	0001c1c8 	.word	0x0001c1c8
    3680:	0001a24f 	.word	0x0001a24f
    3684:	0001b5b1 	.word	0x0001b5b1
    3688:	0001aad3 	.word	0x0001aad3
    368c:	0001aad6 	.word	0x0001aad6
    3690:	0001ab10 	.word	0x0001ab10
    3694:	3c00d140 	.word	0x3c00d140
    3698:	0001a882 	.word	0x0001a882

0000369c <do_tt_blk>:
    369c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    36a0:	e3520001 	cmp	r2, #1	; 0x1
    36a4:	e24dd00c 	sub	sp, sp, #12	; 0xc
    36a8:	e1a05002 	mov	r5, r2
    36ac:	e1a07003 	mov	r7, r3
    36b0:	ca000005 	bgt	36cc <do_tt_blk+0x30>
    36b4:	e59f01bc 	ldr	r0, [pc, #444]	; 3878 <do_tt_blk+0x1dc>
    36b8:	e59f31bc 	ldr	r3, [pc, #444]	; 387c <do_tt_blk+0x1e0>
    36bc:	e1a0e00f 	mov	lr, pc
    36c0:	e593f000 	ldr	pc, [r3]
    36c4:	e3e00000 	mvn	r0, #0	; 0x0
    36c8:	ea000068 	b	3870 <do_tt_blk+0x1d4>
    36cc:	e3520002 	cmp	r2, #2	; 0x2
    36d0:	03a04000 	moveq	r4, #0	; 0x0
    36d4:	01a06004 	moveq	r6, r4
    36d8:	0a000025 	beq	3774 <do_tt_blk+0xd8>
    36dc:	e3a01000 	mov	r1, #0	; 0x0
    36e0:	e3a0200a 	mov	r2, #10	; 0xa
    36e4:	e5930008 	ldr	r0, [r3, #8]
    36e8:	eb002f07 	bl	f30c <simple_strtoul>
    36ec:	e59f118c 	ldr	r1, [pc, #396]	; 3880 <do_tt_blk+0x1e4>
    36f0:	e1a04000 	mov	r4, r0
    36f4:	e3a02005 	mov	r2, #5	; 0x5
    36f8:	e5970004 	ldr	r0, [r7, #4]
    36fc:	ebfff782 	bl	150c <strncmp>
    3700:	e3500000 	cmp	r0, #0	; 0x0
    3704:	059f0178 	ldreq	r0, [pc, #376]	; 3884 <do_tt_blk+0x1e8>
    3708:	01a01004 	moveq	r1, r4
    370c:	059f3168 	ldreq	r3, [pc, #360]	; 387c <do_tt_blk+0x1e0>
    3710:	0a000008 	beq	3738 <do_tt_blk+0x9c>
    3714:	e59f116c 	ldr	r1, [pc, #364]	; 3888 <do_tt_blk+0x1ec>
    3718:	e5970004 	ldr	r0, [r7, #4]
    371c:	e3a02003 	mov	r2, #3	; 0x3
    3720:	ebfff779 	bl	150c <strncmp>
    3724:	e3500000 	cmp	r0, #0	; 0x0
    3728:	e59f314c 	ldr	r3, [pc, #332]	; 387c <do_tt_blk+0x1e0>
    372c:	059f0150 	ldreq	r0, [pc, #336]	; 3884 <do_tt_blk+0x1e8>
    3730:	159f0154 	ldrne	r0, [pc, #340]	; 388c <do_tt_blk+0x1f0>
    3734:	e1a01004 	mov	r1, r4
    3738:	e1a0e00f 	mov	lr, pc
    373c:	e593f000 	ldr	pc, [r3]
    3740:	e3550003 	cmp	r5, #3	; 0x3
    3744:	03a06000 	moveq	r6, #0	; 0x0
    3748:	0a000009 	beq	3774 <do_tt_blk+0xd8>
    374c:	e3a01000 	mov	r1, #0	; 0x0
    3750:	e597000c 	ldr	r0, [r7, #12]
    3754:	e3a02010 	mov	r2, #16	; 0x10
    3758:	eb002eeb 	bl	f30c <simple_strtoul>
    375c:	e1a06000 	mov	r6, r0
    3760:	e1a01006 	mov	r1, r6
    3764:	e59f0124 	ldr	r0, [pc, #292]	; 3890 <do_tt_blk+0x1f4>
    3768:	e59f310c 	ldr	r3, [pc, #268]	; 387c <do_tt_blk+0x1e0>
    376c:	e1a0e00f 	mov	lr, pc
    3770:	e593f000 	ldr	pc, [r3]
    3774:	e3a00000 	mov	r0, #0	; 0x0
    3778:	eb001f89 	bl	b5a4 <get_timer>
    377c:	e59f1110 	ldr	r1, [pc, #272]	; 3894 <do_tt_blk+0x1f8>
    3780:	e3a02000 	mov	r2, #0	; 0x0
    3784:	e3a03000 	mov	r3, #0	; 0x0
    3788:	e881000c 	stm	r1, {r2, r3}
    378c:	e1a09a04 	lsl	r9, r4, #20
    3790:	e2875004 	add	r5, r7, #4	; 0x4
    3794:	e1a08b24 	lsr	r8, r4, #22
    3798:	e1a07504 	lsl	r7, r4, #10
    379c:	e1a0a624 	lsr	sl, r4, #12
    37a0:	e1a0b000 	mov	fp, r0
    37a4:	e3a04000 	mov	r4, #0	; 0x0
    37a8:	e59f30cc 	ldr	r3, [pc, #204]	; 387c <do_tt_blk+0x1e0>
    37ac:	e1a01004 	mov	r1, r4
    37b0:	e59f00e0 	ldr	r0, [pc, #224]	; 3898 <do_tt_blk+0x1fc>
    37b4:	e1a0e00f 	mov	lr, pc
    37b8:	e593f000 	ldr	pc, [r3]
    37bc:	e59f10bc 	ldr	r1, [pc, #188]	; 3880 <do_tt_blk+0x1e4>
    37c0:	e3a02005 	mov	r2, #5	; 0x5
    37c4:	e5950000 	ldr	r0, [r5]
    37c8:	ebfff74f 	bl	150c <strncmp>
    37cc:	e3500000 	cmp	r0, #0	; 0x0
    37d0:	e3a02003 	mov	r2, #3	; 0x3
    37d4:	e59f10ac 	ldr	r1, [pc, #172]	; 3888 <do_tt_blk+0x1ec>
    37d8:	e1a03008 	mov	r3, r8
    37dc:	05950000 	ldreq	r0, [r5]
    37e0:	01a02007 	moveq	r2, r7
    37e4:	0a000008 	beq	380c <do_tt_blk+0x170>
    37e8:	e5950000 	ldr	r0, [r5]
    37ec:	ebfff746 	bl	150c <strncmp>
    37f0:	e3500000 	cmp	r0, #0	; 0x0
    37f4:	05950000 	ldreq	r0, [r5]
    37f8:	15950000 	ldrne	r0, [r5]
    37fc:	e1a02009 	mov	r2, r9
    3800:	e1a0300a 	mov	r3, sl
    3804:	01a02007 	moveq	r2, r7
    3808:	01a03008 	moveq	r3, r8
    380c:	e58d6000 	str	r6, [sp]
    3810:	ebfffe93 	bl	3264 <tt_blk_trolling>
    3814:	eb0032d3 	bl	10368 <tstc>
    3818:	e3500000 	cmp	r0, #0	; 0x0
    381c:	e2844001 	add	r4, r4, #1	; 0x1
    3820:	0affffe0 	beq	37a8 <do_tt_blk+0x10c>
    3824:	eb0032d0 	bl	1036c <getc>
    3828:	e3500003 	cmp	r0, #3	; 0x3
    382c:	1affffdd 	bne	37a8 <do_tt_blk+0x10c>
    3830:	e1a0000b 	mov	r0, fp
    3834:	eb001f5a 	bl	b5a4 <get_timer>
    3838:	e59f3054 	ldr	r3, [pc, #84]	; 3894 <do_tt_blk+0x1f8>
    383c:	e30e1a60 	movw	r1, #60000	; 0xea60
    3840:	e5934000 	ldr	r4, [r3]
    3844:	e5933004 	ldr	r3, [r3, #4]
    3848:	e1a04a24 	lsr	r4, r4, #20
    384c:	e1844603 	orr	r4, r4, r3, lsl #12
    3850:	eb003ab1 	bl	1231c <__aeabi_uidiv>
    3854:	e1a01004 	mov	r1, r4
    3858:	e1a02000 	mov	r2, r0
    385c:	e59f3018 	ldr	r3, [pc, #24]	; 387c <do_tt_blk+0x1e0>
    3860:	e59f0034 	ldr	r0, [pc, #52]	; 389c <do_tt_blk+0x200>
    3864:	e1a0e00f 	mov	lr, pc
    3868:	e593f000 	ldr	pc, [r3]
    386c:	e3a00000 	mov	r0, #0	; 0x0
    3870:	e28dd00c 	add	sp, sp, #12	; 0xc
    3874:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    3878:	0001ab3d 	.word	0x0001ab3d
    387c:	3c00d694 	.word	0x3c00d694
    3880:	0001ab84 	.word	0x0001ab84
    3884:	0001ab8a 	.word	0x0001ab8a
    3888:	0001a53f 	.word	0x0001a53f
    388c:	0001abae 	.word	0x0001abae
    3890:	0001abd2 	.word	0x0001abd2
    3894:	3c00d140 	.word	0x3c00d140
    3898:	0001abf5 	.word	0x0001abf5
    389c:	0001abff 	.word	0x0001abff

000038a0 <__show_strbuf>:
    38a0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    38a4:	e59f4058 	ldr	r4, [pc, #88]	; 3904 <__show_strbuf+0x64>
    38a8:	e24dd004 	sub	sp, sp, #4	; 0x4
    38ac:	e1a06001 	mov	r6, r1
    38b0:	e1a01000 	mov	r1, r0
    38b4:	e59f004c 	ldr	r0, [pc, #76]	; 3908 <__show_strbuf+0x68>
    38b8:	e1a05002 	mov	r5, r2
    38bc:	e1a07004 	mov	r7, r4
    38c0:	e1a0e00f 	mov	lr, pc
    38c4:	e594f000 	ldr	pc, [r4]
    38c8:	e3a04000 	mov	r4, #0	; 0x0
    38cc:	ea000003 	b	38e0 <__show_strbuf+0x40>
    38d0:	e7d61004 	ldrb	r1, [r6, r4]
    38d4:	e1a0e00f 	mov	lr, pc
    38d8:	e597f000 	ldr	pc, [r7]
    38dc:	e2844001 	add	r4, r4, #1	; 0x1
    38e0:	e1540005 	cmp	r4, r5
    38e4:	e59f0020 	ldr	r0, [pc, #32]	; 390c <__show_strbuf+0x6c>
    38e8:	bafffff8 	blt	38d0 <__show_strbuf+0x30>
    38ec:	e59f001c 	ldr	r0, [pc, #28]	; 3910 <__show_strbuf+0x70>
    38f0:	e59f300c 	ldr	r3, [pc, #12]	; 3904 <__show_strbuf+0x64>
    38f4:	e1a0e00f 	mov	lr, pc
    38f8:	e593f000 	ldr	pc, [r3]
    38fc:	e28dd004 	add	sp, sp, #4	; 0x4
    3900:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    3904:	3c00d694 	.word	0x3c00d694
    3908:	0001ac4d 	.word	0x0001ac4d
    390c:	0001c381 	.word	0x0001c381
    3910:	0001a882 	.word	0x0001a882

00003914 <do_crypto_quick>:
    3914:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    3918:	e24dd0f0 	sub	sp, sp, #240	; 0xf0
    391c:	e28da0e0 	add	sl, sp, #224	; 0xe0
    3920:	e3a04010 	mov	r4, #16	; 0x10
    3924:	e28d70c0 	add	r7, sp, #192	; 0xc0
    3928:	e1a02004 	mov	r2, r4
    392c:	e3a09020 	mov	r9, #32	; 0x20
    3930:	e59f11f0 	ldr	r1, [pc, #496]	; 3b28 <do_crypto_quick+0x214>
    3934:	e1a0000a 	mov	r0, sl
    3938:	e3a05040 	mov	r5, #64	; 0x40
    393c:	e28d6080 	add	r6, sp, #128	; 0x80
    3940:	ebfff72a 	bl	15f0 <memcpy>
    3944:	e1a02009 	mov	r2, r9
    3948:	e59f11dc 	ldr	r1, [pc, #476]	; 3b2c <do_crypto_quick+0x218>
    394c:	e1a00007 	mov	r0, r7
    3950:	ebfff726 	bl	15f0 <memcpy>
    3954:	e08d8005 	add	r8, sp, r5
    3958:	e1a02005 	mov	r2, r5
    395c:	e59f11cc 	ldr	r1, [pc, #460]	; 3b30 <do_crypto_quick+0x21c>
    3960:	e1a00006 	mov	r0, r6
    3964:	ebfff721 	bl	15f0 <memcpy>
    3968:	e1a02005 	mov	r2, r5
    396c:	e59f11c0 	ldr	r1, [pc, #448]	; 3b34 <do_crypto_quick+0x220>
    3970:	e1a00008 	mov	r0, r8
    3974:	ebfff71d 	bl	15f0 <memcpy>
    3978:	e1a02004 	mov	r2, r4
    397c:	e1a0100a 	mov	r1, sl
    3980:	e59f01b0 	ldr	r0, [pc, #432]	; 3b38 <do_crypto_quick+0x224>
    3984:	ebffffc5 	bl	38a0 <__show_strbuf>
    3988:	e1a02009 	mov	r2, r9
    398c:	e1a01007 	mov	r1, r7
    3990:	e59f01a4 	ldr	r0, [pc, #420]	; 3b3c <do_crypto_quick+0x228>
    3994:	ebffffc1 	bl	38a0 <__show_strbuf>
    3998:	e1a01006 	mov	r1, r6
    399c:	e1a02005 	mov	r2, r5
    39a0:	e59f0198 	ldr	r0, [pc, #408]	; 3b40 <do_crypto_quick+0x22c>
    39a4:	ebffffbd 	bl	38a0 <__show_strbuf>
    39a8:	e1a01008 	mov	r1, r8
    39ac:	e1a02005 	mov	r2, r5
    39b0:	e59f018c 	ldr	r0, [pc, #396]	; 3b44 <do_crypto_quick+0x230>
    39b4:	ebffffb9 	bl	38a0 <__show_strbuf>
    39b8:	e1a01007 	mov	r1, r7
    39bc:	e1a0200a 	mov	r2, sl
    39c0:	e3a00001 	mov	r0, #1	; 0x1
    39c4:	eb000b4c 	bl	66fc <ss_aes_init>
    39c8:	e1a0100d 	mov	r1, sp
    39cc:	e1a02005 	mov	r2, r5
    39d0:	e1a00006 	mov	r0, r6
    39d4:	eb000b37 	bl	66b8 <ss_aes_data>
    39d8:	e1a0100d 	mov	r1, sp
    39dc:	e1a02005 	mov	r2, r5
    39e0:	e59f0160 	ldr	r0, [pc, #352]	; 3b48 <do_crypto_quick+0x234>
    39e4:	ebffffad 	bl	38a0 <__show_strbuf>
    39e8:	e1a01007 	mov	r1, r7
    39ec:	e1a0200a 	mov	r2, sl
    39f0:	e3a00000 	mov	r0, #0	; 0x0
    39f4:	eb000b40 	bl	66fc <ss_aes_init>
    39f8:	e59f714c 	ldr	r7, [pc, #332]	; 3b4c <do_crypto_quick+0x238>
    39fc:	e1a0100d 	mov	r1, sp
    3a00:	e1a02005 	mov	r2, r5
    3a04:	e1a00008 	mov	r0, r8
    3a08:	eb000b2a 	bl	66b8 <ss_aes_data>
    3a0c:	e1a0100d 	mov	r1, sp
    3a10:	e1a02005 	mov	r2, r5
    3a14:	e59f0134 	ldr	r0, [pc, #308]	; 3b50 <do_crypto_quick+0x23c>
    3a18:	ebffffa0 	bl	38a0 <__show_strbuf>
    3a1c:	e1a02007 	mov	r2, r7
    3a20:	e3a01000 	mov	r1, #0	; 0x0
    3a24:	e3a00001 	mov	r0, #1	; 0x1
    3a28:	eb000b33 	bl	66fc <ss_aes_init>
    3a2c:	e1a0100d 	mov	r1, sp
    3a30:	e1a02005 	mov	r2, r5
    3a34:	e1a00006 	mov	r0, r6
    3a38:	eb000b1e 	bl	66b8 <ss_aes_data>
    3a3c:	e1a0100d 	mov	r1, sp
    3a40:	e1a02005 	mov	r2, r5
    3a44:	e59f0108 	ldr	r0, [pc, #264]	; 3b54 <do_crypto_quick+0x240>
    3a48:	ebffff94 	bl	38a0 <__show_strbuf>
    3a4c:	e3a00000 	mov	r0, #0	; 0x0
    3a50:	e1a02007 	mov	r2, r7
    3a54:	e1a01000 	mov	r1, r0
    3a58:	eb000b27 	bl	66fc <ss_aes_init>
    3a5c:	e1a0100d 	mov	r1, sp
    3a60:	e1a02005 	mov	r2, r5
    3a64:	e1a0000d 	mov	r0, sp
    3a68:	eb000b12 	bl	66b8 <ss_aes_data>
    3a6c:	e1a02005 	mov	r2, r5
    3a70:	e1a0100d 	mov	r1, sp
    3a74:	e59f00dc 	ldr	r0, [pc, #220]	; 3b58 <do_crypto_quick+0x244>
    3a78:	ebffff88 	bl	38a0 <__show_strbuf>
    3a7c:	e1a01005 	mov	r1, r5
    3a80:	e3a00001 	mov	r0, #1	; 0x1
    3a84:	eb000c3a 	bl	6b74 <hash_init>
    3a88:	e1a01005 	mov	r1, r5
    3a8c:	e1a00006 	mov	r0, r6
    3a90:	eb000c12 	bl	6ae0 <hash_data>
    3a94:	e1a0000d 	mov	r0, sp
    3a98:	eb000bf4 	bl	6a70 <hash_value>
    3a9c:	eb000bec 	bl	6a54 <hash_len>
    3aa0:	e1a0100d 	mov	r1, sp
    3aa4:	e1a02000 	mov	r2, r0
    3aa8:	e59f00ac 	ldr	r0, [pc, #172]	; 3b5c <do_crypto_quick+0x248>
    3aac:	ebffff7b 	bl	38a0 <__show_strbuf>
    3ab0:	e1a01005 	mov	r1, r5
    3ab4:	e3a00002 	mov	r0, #2	; 0x2
    3ab8:	eb000c2d 	bl	6b74 <hash_init>
    3abc:	e1a01005 	mov	r1, r5
    3ac0:	e1a00006 	mov	r0, r6
    3ac4:	eb000c05 	bl	6ae0 <hash_data>
    3ac8:	e1a0000d 	mov	r0, sp
    3acc:	eb000be7 	bl	6a70 <hash_value>
    3ad0:	eb000bdf 	bl	6a54 <hash_len>
    3ad4:	e1a0100d 	mov	r1, sp
    3ad8:	e1a02000 	mov	r2, r0
    3adc:	e59f007c 	ldr	r0, [pc, #124]	; 3b60 <do_crypto_quick+0x24c>
    3ae0:	ebffff6e 	bl	38a0 <__show_strbuf>
    3ae4:	e1a01005 	mov	r1, r5
    3ae8:	e3a00003 	mov	r0, #3	; 0x3
    3aec:	eb000c20 	bl	6b74 <hash_init>
    3af0:	e1a01005 	mov	r1, r5
    3af4:	e1a00006 	mov	r0, r6
    3af8:	eb000bf8 	bl	6ae0 <hash_data>
    3afc:	e1a0000d 	mov	r0, sp
    3b00:	eb000bda 	bl	6a70 <hash_value>
    3b04:	eb000bd2 	bl	6a54 <hash_len>
    3b08:	e1a0100d 	mov	r1, sp
    3b0c:	e1a02000 	mov	r2, r0
    3b10:	e59f004c 	ldr	r0, [pc, #76]	; 3b64 <do_crypto_quick+0x250>
    3b14:	ebffff61 	bl	38a0 <__show_strbuf>
    3b18:	e1a0400d 	mov	r4, sp
    3b1c:	e3a00000 	mov	r0, #0	; 0x0
    3b20:	e28dd0f0 	add	sp, sp, #240	; 0xf0
    3b24:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3b28:	000130c8 	.word	0x000130c8
    3b2c:	000130a8 	.word	0x000130a8
    3b30:	00013068 	.word	0x00013068
    3b34:	00013028 	.word	0x00013028
    3b38:	0001ac52 	.word	0x0001ac52
    3b3c:	0001ac55 	.word	0x0001ac55
    3b40:	0001ac59 	.word	0x0001ac59
    3b44:	0001ac64 	.word	0x0001ac64
    3b48:	0001ac75 	.word	0x0001ac75
    3b4c:	3c00d094 	.word	0x3c00d094
    3b50:	0001ac88 	.word	0x0001ac88
    3b54:	0001ac71 	.word	0x0001ac71
    3b58:	0001ac84 	.word	0x0001ac84
    3b5c:	0001ac97 	.word	0x0001ac97
    3b60:	0001ac9b 	.word	0x0001ac9b
    3b64:	0001aca0 	.word	0x0001aca0

00003b68 <tt_crypto_hash>:
    3b68:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    3b6c:	e3a0548e 	mov	r5, #-1912602624	; 0x8e000000
    3b70:	e5951000 	ldr	r1, [r5]
    3b74:	e59f2220 	ldr	r2, [pc, #544]	; 3d9c <tt_crypto_hash+0x234>
    3b78:	e24dd038 	sub	sp, sp, #56	; 0x38
    3b7c:	e59f321c 	ldr	r3, [pc, #540]	; 3da0 <tt_crypto_hash+0x238>
    3b80:	e1510002 	cmp	r1, r2
    3b84:	e28dc028 	add	ip, sp, #40	; 0x28
    3b88:	e1a06000 	mov	r6, r0
    3b8c:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    3b90:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    3b94:	0a000006 	beq	3bb4 <tt_crypto_hash+0x4c>
    3b98:	e59f0204 	ldr	r0, [pc, #516]	; 3da4 <tt_crypto_hash+0x23c>
    3b9c:	e59f3204 	ldr	r3, [pc, #516]	; 3da8 <tt_crypto_hash+0x240>
    3ba0:	e1a0e00f 	mov	lr, pc
    3ba4:	e593f000 	ldr	pc, [r3]
    3ba8:	e59f01fc 	ldr	r0, [pc, #508]	; 3dac <tt_crypto_hash+0x244>
    3bac:	eb001e58 	bl	b514 <udelay>
    3bb0:	ea000008 	b	3bd8 <tt_crypto_hash+0x70>
    3bb4:	e5951004 	ldr	r1, [r5, #4]
    3bb8:	e3a04000 	mov	r4, #0	; 0x0
    3bbc:	e3510003 	cmp	r1, #3	; 0x3
    3bc0:	e5854000 	str	r4, [r5]
    3bc4:	e59fc1dc 	ldr	ip, [pc, #476]	; 3da8 <tt_crypto_hash+0x240>
    3bc8:	9a000004 	bls	3be0 <tt_crypto_hash+0x78>
    3bcc:	e59f01dc 	ldr	r0, [pc, #476]	; 3db0 <tt_crypto_hash+0x248>
    3bd0:	e1a0e00f 	mov	lr, pc
    3bd4:	e59cf000 	ldr	pc, [ip]
    3bd8:	e3e0c000 	mvn	ip, #0	; 0x0
    3bdc:	ea00006b 	b	3d90 <tt_crypto_hash+0x228>
    3be0:	e28d2038 	add	r2, sp, #56	; 0x38
    3be4:	e0823101 	add	r3, r2, r1, lsl #2
    3be8:	e5131010 	ldr	r1, [r3, #-16]
    3bec:	e5952008 	ldr	r2, [r5, #8]
    3bf0:	e59f01bc 	ldr	r0, [pc, #444]	; 3db4 <tt_crypto_hash+0x24c>
    3bf4:	e1a0e00f 	mov	lr, pc
    3bf8:	e59cf000 	ldr	pc, [ip]
    3bfc:	e9950003 	ldmib	r5, {r0, r1}
    3c00:	eb000bdb 	bl	6b74 <hash_init>
    3c04:	eb000b92 	bl	6a54 <hash_len>
    3c08:	e1a03000 	mov	r3, r0
    3c0c:	e59f01a4 	ldr	r0, [pc, #420]	; 3db8 <tt_crypto_hash+0x250>
    3c10:	e3a0c020 	mov	ip, #32	; 0x20
    3c14:	e1a01000 	mov	r1, r0
    3c18:	e3a02001 	mov	r2, #1	; 0x1
    3c1c:	e58dc000 	str	ip, [sp]
    3c20:	eb002d21 	bl	f0ac <print_buffer>
    3c24:	e3560000 	cmp	r6, #0	; 0x0
    3c28:	0a000022 	beq	3cb8 <tt_crypto_hash+0x150>
    3c2c:	e1a09005 	mov	r9, r5
    3c30:	e1a0a004 	mov	sl, r4
    3c34:	e5955008 	ldr	r5, [r5, #8]
    3c38:	e2868901 	add	r8, r6, #16384	; 0x4000
    3c3c:	ea000019 	b	3ca8 <tt_crypto_hash+0x140>
    3c40:	e5991008 	ldr	r1, [r9, #8]
    3c44:	e3550902 	cmp	r5, #32768	; 0x8000
    3c48:	b1a04005 	movlt	r4, r5
    3c4c:	a3a04902 	movge	r4, #32768	; 0x8000
    3c50:	e0651001 	rsb	r1, r5, r1
    3c54:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3c58:	e2811030 	add	r1, r1, #48	; 0x30
    3c5c:	e1a02004 	mov	r2, r4
    3c60:	ebfff662 	bl	15f0 <memcpy>
    3c64:	e3a00000 	mov	r0, #0	; 0x0
    3c68:	eb001e4d 	bl	b5a4 <get_timer>
    3c6c:	e3550901 	cmp	r5, #16384	; 0x4000
    3c70:	b1a01005 	movlt	r1, r5
    3c74:	a3a01901 	movge	r1, #16384	; 0x4000
    3c78:	e1a07000 	mov	r7, r0
    3c7c:	e1a00006 	mov	r0, r6
    3c80:	eb000b96 	bl	6ae0 <hash_data>
    3c84:	e3540902 	cmp	r4, #32768	; 0x8000
    3c88:	e0645005 	rsb	r5, r4, r5
    3c8c:	1a000002 	bne	3c9c <tt_crypto_hash+0x134>
    3c90:	e3a01901 	mov	r1, #16384	; 0x4000
    3c94:	e1a00008 	mov	r0, r8
    3c98:	eb000b90 	bl	6ae0 <hash_data>
    3c9c:	e1a00007 	mov	r0, r7
    3ca0:	eb001e3f 	bl	b5a4 <get_timer>
    3ca4:	e08aa000 	add	sl, sl, r0
    3ca8:	e3550000 	cmp	r5, #0	; 0x0
    3cac:	e1a00006 	mov	r0, r6
    3cb0:	1affffe2 	bne	3c40 <tt_crypto_hash+0xd8>
    3cb4:	ea000008 	b	3cdc <tt_crypto_hash+0x174>
    3cb8:	e1a00006 	mov	r0, r6
    3cbc:	eb001e38 	bl	b5a4 <get_timer>
    3cc0:	e5951008 	ldr	r1, [r5, #8]
    3cc4:	e1a04000 	mov	r4, r0
    3cc8:	e59f00ec 	ldr	r0, [pc, #236]	; 3dbc <tt_crypto_hash+0x254>
    3ccc:	eb000b83 	bl	6ae0 <hash_data>
    3cd0:	e1a00004 	mov	r0, r4
    3cd4:	eb001e32 	bl	b5a4 <get_timer>
    3cd8:	e1a0a000 	mov	sl, r0
    3cdc:	e28d4008 	add	r4, sp, #8	; 0x8
    3ce0:	e3a0648e 	mov	r6, #-1912602624	; 0x8e000000
    3ce4:	e1a00004 	mov	r0, r4
    3ce8:	eb000b60 	bl	6a70 <hash_value>
    3cec:	e5965008 	ldr	r5, [r6, #8]
    3cf0:	eb000b57 	bl	6a54 <hash_len>
    3cf4:	e3a0c020 	mov	ip, #32	; 0x20
    3cf8:	e1a03000 	mov	r3, r0
    3cfc:	e1a01004 	mov	r1, r4
    3d00:	e3a02001 	mov	r2, #1	; 0x1
    3d04:	e1a00004 	mov	r0, r4
    3d08:	e58dc000 	str	ip, [sp]
    3d0c:	eb002ce6 	bl	f0ac <print_buffer>
    3d10:	e5963004 	ldr	r3, [r6, #4]
    3d14:	e28d1038 	add	r1, sp, #56	; 0x38
    3d18:	e0812103 	add	r2, r1, r3, lsl #2
    3d1c:	e59f3084 	ldr	r3, [pc, #132]	; 3da8 <tt_crypto_hash+0x240>
    3d20:	e5127010 	ldr	r7, [r2, #-16]
    3d24:	e5938000 	ldr	r8, [r3]
    3d28:	eb000b49 	bl	6a54 <hash_len>
    3d2c:	e1a01004 	mov	r1, r4
    3d30:	e1a02000 	mov	r2, r0
    3d34:	e59f007c 	ldr	r0, [pc, #124]	; 3db8 <tt_crypto_hash+0x250>
    3d38:	ebfff64d 	bl	1674 <memcmp>
    3d3c:	e1a05525 	lsr	r5, r5, #10
    3d40:	e3500000 	cmp	r0, #0	; 0x0
    3d44:	e59f3074 	ldr	r3, [pc, #116]	; 3dc0 <tt_crypto_hash+0x258>
    3d48:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    3d4c:	e1a0100a 	mov	r1, sl
    3d50:	e0000590 	mul	r0, r0, r5
    3d54:	e59f4068 	ldr	r4, [pc, #104]	; 3dc4 <tt_crypto_hash+0x25c>
    3d58:	01a04003 	moveq	r4, r3
    3d5c:	eb00396e 	bl	1231c <__aeabi_uidiv>
    3d60:	e1a01007 	mov	r1, r7
    3d64:	e1a03000 	mov	r3, r0
    3d68:	e1a02004 	mov	r2, r4
    3d6c:	e59f0054 	ldr	r0, [pc, #84]	; 3dc8 <tt_crypto_hash+0x260>
    3d70:	e12fff38 	blx	r8
    3d74:	e59f1050 	ldr	r1, [pc, #80]	; 3dcc <tt_crypto_hash+0x264>
    3d78:	e5960008 	ldr	r0, [r6, #8]
    3d7c:	e891000c 	ldm	r1, {r2, r3}
    3d80:	e0922000 	adds	r2, r2, r0
    3d84:	e2a33000 	adc	r3, r3, #0	; 0x0
    3d88:	e881000c 	stm	r1, {r2, r3}
    3d8c:	e3a0c000 	mov	ip, #0	; 0x0
    3d90:	e1a0000c 	mov	r0, ip
    3d94:	e28dd038 	add	sp, sp, #56	; 0x38
    3d98:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3d9c:	99997777 	.word	0x99997777
    3da0:	000130d8 	.word	0x000130d8
    3da4:	0001aca7 	.word	0x0001aca7
    3da8:	3c00d694 	.word	0x3c00d694
    3dac:	004c4b40 	.word	0x004c4b40
    3db0:	0001acc6 	.word	0x0001acc6
    3db4:	0001ace1 	.word	0x0001ace1
    3db8:	8e000010 	.word	0x8e000010
    3dbc:	8e000030 	.word	0x8e000030
    3dc0:	0001acfc 	.word	0x0001acfc
    3dc4:	0001ad01 	.word	0x0001ad01
    3dc8:	0001ad08 	.word	0x0001ad08
    3dcc:	3c00d150 	.word	0x3c00d150

00003dd0 <tt_crypto_aes>:
    3dd0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3dd4:	e24dd014 	sub	sp, sp, #20	; 0x14
    3dd8:	e1a06000 	mov	r6, r0
    3ddc:	e3a01000 	mov	r1, #0	; 0x0
    3de0:	e28d000c 	add	r0, sp, #12	; 0xc
    3de4:	e3a02502 	mov	r2, #8388608	; 0x800000
    3de8:	eb0002be 	bl	48e8 <tt_rnd_get_data>
    3dec:	e250b000 	subs	fp, r0, #0	; 0x0
    3df0:	e59f4340 	ldr	r4, [pc, #832]	; 4138 <tt_crypto_aes+0x368>
    3df4:	059f0340 	ldreq	r0, [pc, #832]	; 413c <tt_crypto_aes+0x36c>
    3df8:	0a00005c 	beq	3f70 <tt_crypto_aes+0x1a0>
    3dfc:	e59d200c 	ldr	r2, [sp, #12]
    3e00:	e1a0100b 	mov	r1, fp
    3e04:	e3c2200f 	bic	r2, r2, #15	; 0xf
    3e08:	e1a03002 	mov	r3, r2
    3e0c:	e58d200c 	str	r2, [sp, #12]
    3e10:	e59f0328 	ldr	r0, [pc, #808]	; 4140 <tt_crypto_aes+0x370>
    3e14:	e1a02006 	mov	r2, r6
    3e18:	e1a0e00f 	mov	lr, pc
    3e1c:	e594f000 	ldr	pc, [r4]
    3e20:	e59f031c 	ldr	r0, [pc, #796]	; 4144 <tt_crypto_aes+0x374>
    3e24:	e1a0e00f 	mov	lr, pc
    3e28:	e594f000 	ldr	pc, [r4]
    3e2c:	e1a0100b 	mov	r1, fp
    3e30:	e59d200c 	ldr	r2, [sp, #12]
    3e34:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3e38:	ebfff5ec 	bl	15f0 <memcpy>
    3e3c:	e59f0304 	ldr	r0, [pc, #772]	; 4148 <tt_crypto_aes+0x378>
    3e40:	e1a0e00f 	mov	lr, pc
    3e44:	e594f000 	ldr	pc, [r4]
    3e48:	e3a01000 	mov	r1, #0	; 0x0
    3e4c:	e59f22f8 	ldr	r2, [pc, #760]	; 414c <tt_crypto_aes+0x37c>
    3e50:	e3a00001 	mov	r0, #1	; 0x1
    3e54:	eb000a28 	bl	66fc <ss_aes_init>
    3e58:	e59f02f0 	ldr	r0, [pc, #752]	; 4150 <tt_crypto_aes+0x380>
    3e5c:	e1a0e00f 	mov	lr, pc
    3e60:	e594f000 	ldr	pc, [r4]
    3e64:	e3560000 	cmp	r6, #0	; 0x0
    3e68:	159d500c 	ldrne	r5, [sp, #12]
    3e6c:	13a0a000 	movne	sl, #0	; 0x0
    3e70:	12868901 	addne	r8, r6, #16384	; 0x4000
    3e74:	1a000022 	bne	3f04 <tt_crypto_aes+0x134>
    3e78:	ea000025 	b	3f14 <tt_crypto_aes+0x144>
    3e7c:	e59d100c 	ldr	r1, [sp, #12]
    3e80:	e3550902 	cmp	r5, #32768	; 0x8000
    3e84:	b1a07005 	movlt	r7, r5
    3e88:	a3a07902 	movge	r7, #32768	; 0x8000
    3e8c:	e0651001 	rsb	r1, r5, r1
    3e90:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3e94:	e1a02007 	mov	r2, r7
    3e98:	ebfff5d4 	bl	15f0 <memcpy>
    3e9c:	e3a00000 	mov	r0, #0	; 0x0
    3ea0:	eb001dbf 	bl	b5a4 <get_timer>
    3ea4:	e3550901 	cmp	r5, #16384	; 0x4000
    3ea8:	b1a02005 	movlt	r2, r5
    3eac:	a3a02901 	movge	r2, #16384	; 0x4000
    3eb0:	e1a04000 	mov	r4, r0
    3eb4:	e1a01006 	mov	r1, r6
    3eb8:	e1a00006 	mov	r0, r6
    3ebc:	eb0009fd 	bl	66b8 <ss_aes_data>
    3ec0:	e3570902 	cmp	r7, #32768	; 0x8000
    3ec4:	1a000003 	bne	3ed8 <tt_crypto_aes+0x108>
    3ec8:	e1a01008 	mov	r1, r8
    3ecc:	e3a02901 	mov	r2, #16384	; 0x4000
    3ed0:	e1a00008 	mov	r0, r8
    3ed4:	eb0009f7 	bl	66b8 <ss_aes_data>
    3ed8:	e1a00004 	mov	r0, r4
    3edc:	eb001db0 	bl	b5a4 <get_timer>
    3ee0:	e1a04000 	mov	r4, r0
    3ee4:	e59d000c 	ldr	r0, [sp, #12]
    3ee8:	e1a01006 	mov	r1, r6
    3eec:	e0650000 	rsb	r0, r5, r0
    3ef0:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    3ef4:	e1a02007 	mov	r2, r7
    3ef8:	ebfff5bc 	bl	15f0 <memcpy>
    3efc:	e08aa004 	add	sl, sl, r4
    3f00:	e0675005 	rsb	r5, r7, r5
    3f04:	e3550000 	cmp	r5, #0	; 0x0
    3f08:	e1a00006 	mov	r0, r6
    3f0c:	1affffda 	bne	3e7c <tt_crypto_aes+0xac>
    3f10:	ea000009 	b	3f3c <tt_crypto_aes+0x16c>
    3f14:	e1a00006 	mov	r0, r6
    3f18:	eb001da1 	bl	b5a4 <get_timer>
    3f1c:	e1a04000 	mov	r4, r0
    3f20:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3f24:	e1a01000 	mov	r1, r0
    3f28:	e59d200c 	ldr	r2, [sp, #12]
    3f2c:	eb0009e1 	bl	66b8 <ss_aes_data>
    3f30:	e1a00004 	mov	r0, r4
    3f34:	eb001d9a 	bl	b5a4 <get_timer>
    3f38:	e1a0a000 	mov	sl, r0
    3f3c:	e59d300c 	ldr	r3, [sp, #12]
    3f40:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3f44:	e1a0100b 	mov	r1, fp
    3f48:	e3a02010 	mov	r2, #16	; 0x10
    3f4c:	e58d3004 	str	r3, [sp, #4]
    3f50:	ebfff5c7 	bl	1674 <memcmp>
    3f54:	e3500000 	cmp	r0, #0	; 0x0
    3f58:	e59f41d8 	ldr	r4, [pc, #472]	; 4138 <tt_crypto_aes+0x368>
    3f5c:	1a000007 	bne	3f80 <tt_crypto_aes+0x1b0>
    3f60:	e59f01ec 	ldr	r0, [pc, #492]	; 4154 <tt_crypto_aes+0x384>
    3f64:	e1a0e00f 	mov	lr, pc
    3f68:	e594f000 	ldr	pc, [r4]
    3f6c:	e59f01e4 	ldr	r0, [pc, #484]	; 4158 <tt_crypto_aes+0x388>
    3f70:	e1a0e00f 	mov	lr, pc
    3f74:	e594f000 	ldr	pc, [r4]
    3f78:	e3e00000 	mvn	r0, #0	; 0x0
    3f7c:	ea00006b 	b	4130 <tt_crypto_aes+0x360>
    3f80:	e3a00000 	mov	r0, #0	; 0x0
    3f84:	e1a01000 	mov	r1, r0
    3f88:	e59f21bc 	ldr	r2, [pc, #444]	; 414c <tt_crypto_aes+0x37c>
    3f8c:	eb0009da 	bl	66fc <ss_aes_init>
    3f90:	e59f01c4 	ldr	r0, [pc, #452]	; 415c <tt_crypto_aes+0x38c>
    3f94:	e1a0e00f 	mov	lr, pc
    3f98:	e594f000 	ldr	pc, [r4]
    3f9c:	e3560000 	cmp	r6, #0	; 0x0
    3fa0:	159d500c 	ldrne	r5, [sp, #12]
    3fa4:	13a08000 	movne	r8, #0	; 0x0
    3fa8:	12869901 	addne	r9, r6, #16384	; 0x4000
    3fac:	1a000022 	bne	403c <tt_crypto_aes+0x26c>
    3fb0:	ea000025 	b	404c <tt_crypto_aes+0x27c>
    3fb4:	eb001d7a 	bl	b5a4 <get_timer>
    3fb8:	e59d100c 	ldr	r1, [sp, #12]
    3fbc:	e3550902 	cmp	r5, #32768	; 0x8000
    3fc0:	b1a07005 	movlt	r7, r5
    3fc4:	a3a07902 	movge	r7, #32768	; 0x8000
    3fc8:	e0651001 	rsb	r1, r5, r1
    3fcc:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3fd0:	e1a02007 	mov	r2, r7
    3fd4:	e1a04000 	mov	r4, r0
    3fd8:	e1a00006 	mov	r0, r6
    3fdc:	ebfff583 	bl	15f0 <memcpy>
    3fe0:	e3550901 	cmp	r5, #16384	; 0x4000
    3fe4:	b1a02005 	movlt	r2, r5
    3fe8:	a3a02901 	movge	r2, #16384	; 0x4000
    3fec:	e1a00006 	mov	r0, r6
    3ff0:	e1a01006 	mov	r1, r6
    3ff4:	eb0009af 	bl	66b8 <ss_aes_data>
    3ff8:	e3570902 	cmp	r7, #32768	; 0x8000
    3ffc:	1a000003 	bne	4010 <tt_crypto_aes+0x240>
    4000:	e1a01009 	mov	r1, r9
    4004:	e3a02901 	mov	r2, #16384	; 0x4000
    4008:	e1a00009 	mov	r0, r9
    400c:	eb0009a9 	bl	66b8 <ss_aes_data>
    4010:	e1a00004 	mov	r0, r4
    4014:	eb001d62 	bl	b5a4 <get_timer>
    4018:	e1a04000 	mov	r4, r0
    401c:	e59d000c 	ldr	r0, [sp, #12]
    4020:	e1a01006 	mov	r1, r6
    4024:	e0650000 	rsb	r0, r5, r0
    4028:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    402c:	e1a02007 	mov	r2, r7
    4030:	ebfff56e 	bl	15f0 <memcpy>
    4034:	e0888004 	add	r8, r8, r4
    4038:	e0675005 	rsb	r5, r7, r5
    403c:	e3a00000 	mov	r0, #0	; 0x0
    4040:	e1550000 	cmp	r5, r0
    4044:	1affffda 	bne	3fb4 <tt_crypto_aes+0x1e4>
    4048:	ea000009 	b	4074 <tt_crypto_aes+0x2a4>
    404c:	e1a00006 	mov	r0, r6
    4050:	eb001d53 	bl	b5a4 <get_timer>
    4054:	e1a04000 	mov	r4, r0
    4058:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    405c:	e1a01000 	mov	r1, r0
    4060:	e59d200c 	ldr	r2, [sp, #12]
    4064:	eb000993 	bl	66b8 <ss_aes_data>
    4068:	e1a00004 	mov	r0, r4
    406c:	eb001d4c 	bl	b5a4 <get_timer>
    4070:	e1a08000 	mov	r8, r0
    4074:	e59d3004 	ldr	r3, [sp, #4]
    4078:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    407c:	e1a00543 	asr	r0, r3, #10
    4080:	e1a0100a 	mov	r1, sl
    4084:	e0000094 	mul	r0, r4, r0
    4088:	eb003925 	bl	12524 <__aeabi_idiv>
    408c:	e1a06000 	mov	r6, r0
    4090:	e59d000c 	ldr	r0, [sp, #12]
    4094:	e1a01008 	mov	r1, r8
    4098:	e1a00540 	asr	r0, r0, #10
    409c:	e0000094 	mul	r0, r4, r0
    40a0:	eb00391f 	bl	12524 <__aeabi_idiv>
    40a4:	e59f408c 	ldr	r4, [pc, #140]	; 4138 <tt_crypto_aes+0x368>
    40a8:	e1a05000 	mov	r5, r0
    40ac:	e59f00ac 	ldr	r0, [pc, #172]	; 4160 <tt_crypto_aes+0x390>
    40b0:	e1a0e00f 	mov	lr, pc
    40b4:	e594f000 	ldr	pc, [r4]
    40b8:	e1a0000b 	mov	r0, fp
    40bc:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    40c0:	e59d200c 	ldr	r2, [sp, #12]
    40c4:	e3a0300c 	mov	r3, #12	; 0xc
    40c8:	eb000189 	bl	46f4 <tt_cmp_buffer>
    40cc:	e3500000 	cmp	r0, #0	; 0x0
    40d0:	0a000006 	beq	40f0 <tt_crypto_aes+0x320>
    40d4:	e1a03000 	mov	r3, r0
    40d8:	e1a01006 	mov	r1, r6
    40dc:	e1a02005 	mov	r2, r5
    40e0:	e59f007c 	ldr	r0, [pc, #124]	; 4164 <tt_crypto_aes+0x394>
    40e4:	e1a0e00f 	mov	lr, pc
    40e8:	e594f000 	ldr	pc, [r4]
    40ec:	ea000004 	b	4104 <tt_crypto_aes+0x334>
    40f0:	e1a01006 	mov	r1, r6
    40f4:	e1a02005 	mov	r2, r5
    40f8:	e59f0068 	ldr	r0, [pc, #104]	; 4168 <tt_crypto_aes+0x398>
    40fc:	e1a0e00f 	mov	lr, pc
    4100:	e594f000 	ldr	pc, [r4]
    4104:	e59f0060 	ldr	r0, [pc, #96]	; 416c <tt_crypto_aes+0x39c>
    4108:	e59d100c 	ldr	r1, [sp, #12]
    410c:	e890000c 	ldm	r0, {r2, r3}
    4110:	e0922001 	adds	r2, r2, r1
    4114:	e0a33fc1 	adc	r3, r3, r1, asr #31
    4118:	e880000c 	stm	r0, {r2, r3}
    411c:	e59f0030 	ldr	r0, [pc, #48]	; 4154 <tt_crypto_aes+0x384>
    4120:	e59f3010 	ldr	r3, [pc, #16]	; 4138 <tt_crypto_aes+0x368>
    4124:	e1a0e00f 	mov	lr, pc
    4128:	e593f000 	ldr	pc, [r3]
    412c:	e3a00000 	mov	r0, #0	; 0x0
    4130:	e28dd014 	add	sp, sp, #20	; 0x14
    4134:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4138:	3c00d694 	.word	0x3c00d694
    413c:	0001ad28 	.word	0x0001ad28
    4140:	0001ad4b 	.word	0x0001ad4b
    4144:	0001a78d 	.word	0x0001a78d
    4148:	0001b5c5 	.word	0x0001b5c5
    414c:	3c00d094 	.word	0x3c00d094
    4150:	0001c1c8 	.word	0x0001c1c8
    4154:	0001a882 	.word	0x0001a882
    4158:	0001ad77 	.word	0x0001ad77
    415c:	0001bc3b 	.word	0x0001bc3b
    4160:	0001aad3 	.word	0x0001aad3
    4164:	0001ad98 	.word	0x0001ad98
    4168:	0001add6 	.word	0x0001add6
    416c:	3c00d148 	.word	0x3c00d148

00004170 <do_tt_crypto>:
    4170:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    4174:	e3520001 	cmp	r2, #1	; 0x1
    4178:	e24dd004 	sub	sp, sp, #4	; 0x4
    417c:	d3a07000 	movle	r7, #0	; 0x0
    4180:	da000009 	ble	41ac <do_tt_crypto+0x3c>
    4184:	e5930004 	ldr	r0, [r3, #4]
    4188:	e3a01000 	mov	r1, #0	; 0x0
    418c:	e3a02010 	mov	r2, #16	; 0x10
    4190:	eb002c5d 	bl	f30c <simple_strtoul>
    4194:	e1a07000 	mov	r7, r0
    4198:	e1a01007 	mov	r1, r7
    419c:	e59f0114 	ldr	r0, [pc, #276]	; 42b8 <do_tt_crypto+0x148>
    41a0:	e59f3114 	ldr	r3, [pc, #276]	; 42bc <do_tt_crypto+0x14c>
    41a4:	e1a0e00f 	mov	lr, pc
    41a8:	e593f000 	ldr	pc, [r3]
    41ac:	e3a00000 	mov	r0, #0	; 0x0
    41b0:	eb001cfb 	bl	b5a4 <get_timer>
    41b4:	e59f1104 	ldr	r1, [pc, #260]	; 42c0 <do_tt_crypto+0x150>
    41b8:	e3a02000 	mov	r2, #0	; 0x0
    41bc:	e3a03000 	mov	r3, #0	; 0x0
    41c0:	e881000c 	stm	r1, {r2, r3}
    41c4:	e59f10f8 	ldr	r1, [pc, #248]	; 42c4 <do_tt_crypto+0x154>
    41c8:	e59fa0ec 	ldr	sl, [pc, #236]	; 42bc <do_tt_crypto+0x14c>
    41cc:	e881000c 	stm	r1, {r2, r3}
    41d0:	e1a08000 	mov	r8, r0
    41d4:	e3a06000 	mov	r6, #0	; 0x0
    41d8:	e3a01006 	mov	r1, #6	; 0x6
    41dc:	e3a00000 	mov	r0, #0	; 0x0
    41e0:	eb000197 	bl	4844 <tt_rnd_get_int>
    41e4:	e3a01002 	mov	r1, #2	; 0x2
    41e8:	e1a04000 	mov	r4, r0
    41ec:	e3a00000 	mov	r0, #0	; 0x0
    41f0:	eb000193 	bl	4844 <tt_rnd_get_int>
    41f4:	e1a01006 	mov	r1, r6
    41f8:	e1a03000 	mov	r3, r0
    41fc:	e1a05000 	mov	r5, r0
    4200:	e1a02004 	mov	r2, r4
    4204:	e59f00bc 	ldr	r0, [pc, #188]	; 42c8 <do_tt_crypto+0x158>
    4208:	e1a0e00f 	mov	lr, pc
    420c:	e59af000 	ldr	pc, [sl]
    4210:	e3540003 	cmp	r4, #3	; 0x3
    4214:	9a000004 	bls	422c <do_tt_crypto+0xbc>
    4218:	e3550000 	cmp	r5, #0	; 0x0
    421c:	11a00007 	movne	r0, r7
    4220:	03a00000 	moveq	r0, #0	; 0x0
    4224:	ebfffee9 	bl	3dd0 <tt_crypto_aes>
    4228:	ea000003 	b	423c <do_tt_crypto+0xcc>
    422c:	e3550000 	cmp	r5, #0	; 0x0
    4230:	11a00007 	movne	r0, r7
    4234:	03a00000 	moveq	r0, #0	; 0x0
    4238:	ebfffe4a 	bl	3b68 <tt_crypto_hash>
    423c:	eb003049 	bl	10368 <tstc>
    4240:	e3500000 	cmp	r0, #0	; 0x0
    4244:	e2866001 	add	r6, r6, #1	; 0x1
    4248:	0affffe2 	beq	41d8 <do_tt_crypto+0x68>
    424c:	eb003046 	bl	1036c <getc>
    4250:	e3500003 	cmp	r0, #3	; 0x3
    4254:	1affffdf 	bne	41d8 <do_tt_crypto+0x68>
    4258:	e1a00008 	mov	r0, r8
    425c:	eb001cd0 	bl	b5a4 <get_timer>
    4260:	e59f305c 	ldr	r3, [pc, #92]	; 42c4 <do_tt_crypto+0x154>
    4264:	e59f2054 	ldr	r2, [pc, #84]	; 42c0 <do_tt_crypto+0x150>
    4268:	e5934000 	ldr	r4, [r3]
    426c:	e5925000 	ldr	r5, [r2]
    4270:	e5933004 	ldr	r3, [r3, #4]
    4274:	e5922004 	ldr	r2, [r2, #4]
    4278:	e30e1a60 	movw	r1, #60000	; 0xea60
    427c:	e1a04a24 	lsr	r4, r4, #20
    4280:	e1a05a25 	lsr	r5, r5, #20
    4284:	e1844603 	orr	r4, r4, r3, lsl #12
    4288:	e1855602 	orr	r5, r5, r2, lsl #12
    428c:	eb003822 	bl	1231c <__aeabi_uidiv>
    4290:	e1a01004 	mov	r1, r4
    4294:	e1a03000 	mov	r3, r0
    4298:	e1a02005 	mov	r2, r5
    429c:	e59f0028 	ldr	r0, [pc, #40]	; 42cc <do_tt_crypto+0x15c>
    42a0:	e59fc014 	ldr	ip, [pc, #20]	; 42bc <do_tt_crypto+0x14c>
    42a4:	e1a0e00f 	mov	lr, pc
    42a8:	e59cf000 	ldr	pc, [ip]
    42ac:	e3a00000 	mov	r0, #0	; 0x0
    42b0:	e28dd004 	add	sp, sp, #4	; 0x4
    42b4:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    42b8:	0001ae07 	.word	0x0001ae07
    42bc:	3c00d694 	.word	0x3c00d694
    42c0:	3c00d150 	.word	0x3c00d150
    42c4:	3c00d148 	.word	0x3c00d148
    42c8:	0001ae2d 	.word	0x0001ae2d
    42cc:	0001ae4c 	.word	0x0001ae4c

000042d0 <tt_ff_trolling>:
    42d0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    42d4:	e3a01001 	mov	r1, #1	; 0x1
    42d8:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    42dc:	e58d200c 	str	r2, [sp, #12]
    42e0:	e58d3008 	str	r3, [sp, #8]
    42e4:	e1a05000 	mov	r5, r0
    42e8:	eb0008c0 	bl	65f0 <vs_assign_by_name>
    42ec:	e2506000 	subs	r6, r0, #0	; 0x0
    42f0:	0a000005 	beq	430c <tt_ff_trolling+0x3c>
    42f4:	e1a01005 	mov	r1, r5
    42f8:	e59f0330 	ldr	r0, [pc, #816]	; 4630 <tt_ff_trolling+0x360>
    42fc:	e59f3330 	ldr	r3, [pc, #816]	; 4634 <tt_ff_trolling+0x364>
    4300:	e1a0e00f 	mov	lr, pc
    4304:	e593f000 	ldr	pc, [r3]
    4308:	ea00000c 	b	4340 <tt_ff_trolling+0x70>
    430c:	e3a0000e 	mov	r0, #14	; 0xe
    4310:	eb000744 	bl	6028 <vs_align>
    4314:	e1a01000 	mov	r1, r0
    4318:	e28d0018 	add	r0, sp, #24	; 0x18
    431c:	e5201004 	str	r1, [r0, #-4]!
    4320:	e1a02001 	mov	r2, r1
    4324:	eb00016f 	bl	48e8 <tt_rnd_get_data>
    4328:	e250a000 	subs	sl, r0, #0	; 0x0
    432c:	e59f4300 	ldr	r4, [pc, #768]	; 4634 <tt_ff_trolling+0x364>
    4330:	1a000004 	bne	4348 <tt_ff_trolling+0x78>
    4334:	e59f02fc 	ldr	r0, [pc, #764]	; 4638 <tt_ff_trolling+0x368>
    4338:	e1a0e00f 	mov	lr, pc
    433c:	e594f000 	ldr	pc, [r4]
    4340:	e3e00000 	mvn	r0, #0	; 0x0
    4344:	ea0000a9 	b	45f0 <tt_ff_trolling+0x320>
    4348:	e59d2014 	ldr	r2, [sp, #20]
    434c:	e1a0100a 	mov	r1, sl
    4350:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    4354:	ebfff4a5 	bl	15f0 <memcpy>
    4358:	e3a01501 	mov	r1, #4194304	; 0x400000
    435c:	e59f02d8 	ldr	r0, [pc, #728]	; 463c <tt_ff_trolling+0x36c>
    4360:	e1a0e00f 	mov	lr, pc
    4364:	e594f000 	ldr	pc, [r4]
    4368:	e1a00005 	mov	r0, r5
    436c:	eb0007ae 	bl	622c <vs_device_id>
    4370:	e1a0b004 	mov	fp, r4
    4374:	e3a05000 	mov	r5, #0	; 0x0
    4378:	e3a04000 	mov	r4, #0	; 0x0
    437c:	e3a08501 	mov	r8, #4194304	; 0x400000
    4380:	e3a09000 	mov	r9, #0	; 0x0
    4384:	e1a07000 	mov	r7, r0
    4388:	ea00003d 	b	4484 <tt_ff_trolling+0x1b4>
    438c:	e1a00007 	mov	r0, r7
    4390:	eb000791 	bl	61dc <vs_device_erasable>
    4394:	e3500000 	cmp	r0, #0	; 0x0
    4398:	0a000003 	beq	43ac <tt_ff_trolling+0xdc>
    439c:	e1a00004 	mov	r0, r4
    43a0:	e1a01005 	mov	r1, r5
    43a4:	e3a02501 	mov	r2, #4194304	; 0x400000
    43a8:	eb0006e2 	bl	5f38 <vs_erase>
    43ac:	e1a00007 	mov	r0, r7
    43b0:	e59f1288 	ldr	r1, [pc, #648]	; 4640 <tt_ff_trolling+0x370>
    43b4:	eb0007b0 	bl	627c <vs_is_device>
    43b8:	e3500000 	cmp	r0, #0	; 0x0
    43bc:	1a000009 	bne	43e8 <tt_ff_trolling+0x118>
    43c0:	e1a00007 	mov	r0, r7
    43c4:	e59f1278 	ldr	r1, [pc, #632]	; 4644 <tt_ff_trolling+0x374>
    43c8:	eb0007ab 	bl	627c <vs_is_device>
    43cc:	e3500000 	cmp	r0, #0	; 0x0
    43d0:	1a000004 	bne	43e8 <tt_ff_trolling+0x118>
    43d4:	e1a00007 	mov	r0, r7
    43d8:	e59f1268 	ldr	r1, [pc, #616]	; 4648 <tt_ff_trolling+0x378>
    43dc:	eb0007a6 	bl	627c <vs_is_device>
    43e0:	e3500000 	cmp	r0, #0	; 0x0
    43e4:	0a00000a 	beq	4414 <tt_ff_trolling+0x144>
    43e8:	e1a00004 	mov	r0, r4
    43ec:	e1a01005 	mov	r1, r5
    43f0:	eb0019c0 	bl	aaf8 <nnd_vs_isbad>
    43f4:	e3500000 	cmp	r0, #0	; 0x0
    43f8:	0a000005 	beq	4414 <tt_ff_trolling+0x144>
    43fc:	e59f0248 	ldr	r0, [pc, #584]	; 464c <tt_ff_trolling+0x37c>
    4400:	e1a02004 	mov	r2, r4
    4404:	e1a03005 	mov	r3, r5
    4408:	e1a0e00f 	mov	lr, pc
    440c:	e59bf000 	ldr	pc, [fp]
    4410:	ea000018 	b	4478 <tt_ff_trolling+0x1a8>
    4414:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    4418:	e5816000 	str	r6, [r1]
    441c:	e1a00001 	mov	r0, r1
    4420:	e59d1014 	ldr	r1, [sp, #20]
    4424:	e1a02004 	mov	r2, r4
    4428:	e58d1000 	str	r1, [sp]
    442c:	e1a03005 	mov	r3, r5
    4430:	e3a01000 	mov	r1, #0	; 0x0
    4434:	e58d1004 	str	r1, [sp, #4]
    4438:	eb0006a8 	bl	5ee0 <vs_write>
    443c:	e3500000 	cmp	r0, #0	; 0x0
    4440:	aa000006 	bge	4460 <tt_ff_trolling+0x190>
    4444:	e1a02004 	mov	r2, r4
    4448:	e1a03005 	mov	r3, r5
    444c:	e59f01fc 	ldr	r0, [pc, #508]	; 4650 <tt_ff_trolling+0x380>
    4450:	e59f11dc 	ldr	r1, [pc, #476]	; 4634 <tt_ff_trolling+0x364>
    4454:	e1a0e00f 	mov	lr, pc
    4458:	e591f000 	ldr	pc, [r1]
    445c:	ea000010 	b	44a4 <tt_ff_trolling+0x1d4>
    4460:	e316001f 	tst	r6, #31	; 0x1f
    4464:	1a000003 	bne	4478 <tt_ff_trolling+0x1a8>
    4468:	e59f01e4 	ldr	r0, [pc, #484]	; 4654 <tt_ff_trolling+0x384>
    446c:	e1a01106 	lsl	r1, r6, #2
    4470:	e1a0e00f 	mov	lr, pc
    4474:	e59bf000 	ldr	pc, [fp]
    4478:	e0944008 	adds	r4, r4, r8
    447c:	e0a55009 	adc	r5, r5, r9
    4480:	e2866001 	add	r6, r6, #1	; 0x1
    4484:	e59d3008 	ldr	r3, [sp, #8]
    4488:	e1530005 	cmp	r3, r5
    448c:	8affffbe 	bhi	438c <tt_ff_trolling+0xbc>
    4490:	1a000058 	bne	45f8 <tt_ff_trolling+0x328>
    4494:	e59d100c 	ldr	r1, [sp, #12]
    4498:	e1510004 	cmp	r1, r4
    449c:	8affffba 	bhi	438c <tt_ff_trolling+0xbc>
    44a0:	ea000054 	b	45f8 <tt_ff_trolling+0x328>
    44a4:	e59f4188 	ldr	r4, [pc, #392]	; 4634 <tt_ff_trolling+0x364>
    44a8:	e59f01a8 	ldr	r0, [pc, #424]	; 4658 <tt_ff_trolling+0x388>
    44ac:	e3a01501 	mov	r1, #4194304	; 0x400000
    44b0:	e1a0e00f 	mov	lr, pc
    44b4:	e594f000 	ldr	pc, [r4]
    44b8:	e1a0b004 	mov	fp, r4
    44bc:	e3a06000 	mov	r6, #0	; 0x0
    44c0:	e28aa004 	add	sl, sl, #4	; 0x4
    44c4:	e3a04000 	mov	r4, #0	; 0x0
    44c8:	e3a05000 	mov	r5, #0	; 0x0
    44cc:	e3a08501 	mov	r8, #4194304	; 0x400000
    44d0:	e3a09000 	mov	r9, #0	; 0x0
    44d4:	ea00003d 	b	45d0 <tt_ff_trolling+0x300>
    44d8:	e1a00007 	mov	r0, r7
    44dc:	e59f115c 	ldr	r1, [pc, #348]	; 4640 <tt_ff_trolling+0x370>
    44e0:	eb000765 	bl	627c <vs_is_device>
    44e4:	e3500000 	cmp	r0, #0	; 0x0
    44e8:	1a000009 	bne	4514 <tt_ff_trolling+0x244>
    44ec:	e1a00007 	mov	r0, r7
    44f0:	e59f114c 	ldr	r1, [pc, #332]	; 4644 <tt_ff_trolling+0x374>
    44f4:	eb000760 	bl	627c <vs_is_device>
    44f8:	e3500000 	cmp	r0, #0	; 0x0
    44fc:	1a000004 	bne	4514 <tt_ff_trolling+0x244>
    4500:	e1a00007 	mov	r0, r7
    4504:	e59f113c 	ldr	r1, [pc, #316]	; 4648 <tt_ff_trolling+0x378>
    4508:	eb00075b 	bl	627c <vs_is_device>
    450c:	e3500000 	cmp	r0, #0	; 0x0
    4510:	0a00000a 	beq	4540 <tt_ff_trolling+0x270>
    4514:	e1a00004 	mov	r0, r4
    4518:	e1a01005 	mov	r1, r5
    451c:	eb001975 	bl	aaf8 <nnd_vs_isbad>
    4520:	e3500000 	cmp	r0, #0	; 0x0
    4524:	0a000005 	beq	4540 <tt_ff_trolling+0x270>
    4528:	e59f011c 	ldr	r0, [pc, #284]	; 464c <tt_ff_trolling+0x37c>
    452c:	e1a02004 	mov	r2, r4
    4530:	e1a03005 	mov	r3, r5
    4534:	e1a0e00f 	mov	lr, pc
    4538:	e59bf000 	ldr	pc, [fp]
    453c:	ea000020 	b	45c4 <tt_ff_trolling+0x2f4>
    4540:	e59d1014 	ldr	r1, [sp, #20]
    4544:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    4548:	e58d1000 	str	r1, [sp]
    454c:	e1a02004 	mov	r2, r4
    4550:	e3a01000 	mov	r1, #0	; 0x0
    4554:	e1a03005 	mov	r3, r5
    4558:	e58d1004 	str	r1, [sp, #4]
    455c:	eb00075f 	bl	62e0 <vs_read>
    4560:	e316001f 	tst	r6, #31	; 0x1f
    4564:	1a000003 	bne	4578 <tt_ff_trolling+0x2a8>
    4568:	e59f00e4 	ldr	r0, [pc, #228]	; 4654 <tt_ff_trolling+0x384>
    456c:	e1a01106 	lsl	r1, r6, #2
    4570:	e1a0e00f 	mov	lr, pc
    4574:	e59bf000 	ldr	pc, [fp]
    4578:	e3a0348e 	mov	r3, #-1912602624	; 0x8e000000
    457c:	e5933000 	ldr	r3, [r3]
    4580:	e1530006 	cmp	r3, r6
    4584:	1a000007 	bne	45a8 <tt_ff_trolling+0x2d8>
    4588:	e59d2014 	ldr	r2, [sp, #20]
    458c:	e1a0000a 	mov	r0, sl
    4590:	e2422004 	sub	r2, r2, #4	; 0x4
    4594:	e59f10c0 	ldr	r1, [pc, #192]	; 465c <tt_ff_trolling+0x38c>
    4598:	e3a0300a 	mov	r3, #10	; 0xa
    459c:	eb000054 	bl	46f4 <tt_cmp_buffer>
    45a0:	e3500000 	cmp	r0, #0	; 0x0
    45a4:	0a000006 	beq	45c4 <tt_ff_trolling+0x2f4>
    45a8:	e1a02004 	mov	r2, r4
    45ac:	e1a03005 	mov	r3, r5
    45b0:	e59f00a8 	ldr	r0, [pc, #168]	; 4660 <tt_ff_trolling+0x390>
    45b4:	e59f1078 	ldr	r1, [pc, #120]	; 4634 <tt_ff_trolling+0x364>
    45b8:	e1a0e00f 	mov	lr, pc
    45bc:	e591f000 	ldr	pc, [r1]
    45c0:	ea000018 	b	4628 <tt_ff_trolling+0x358>
    45c4:	e0944008 	adds	r4, r4, r8
    45c8:	e0a55009 	adc	r5, r5, r9
    45cc:	e2866001 	add	r6, r6, #1	; 0x1
    45d0:	e59d3008 	ldr	r3, [sp, #8]
    45d4:	e1530005 	cmp	r3, r5
    45d8:	8affffbe 	bhi	44d8 <tt_ff_trolling+0x208>
    45dc:	1a00000a 	bne	460c <tt_ff_trolling+0x33c>
    45e0:	e59d100c 	ldr	r1, [sp, #12]
    45e4:	e1510004 	cmp	r1, r4
    45e8:	8affffba 	bhi	44d8 <tt_ff_trolling+0x208>
    45ec:	ea000006 	b	460c <tt_ff_trolling+0x33c>
    45f0:	e28dd01c 	add	sp, sp, #28	; 0x1c
    45f4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    45f8:	e59f0064 	ldr	r0, [pc, #100]	; 4664 <tt_ff_trolling+0x394>
    45fc:	e59f3030 	ldr	r3, [pc, #48]	; 4634 <tt_ff_trolling+0x364>
    4600:	e1a0e00f 	mov	lr, pc
    4604:	e593f000 	ldr	pc, [r3]
    4608:	eaffffa5 	b	44a4 <tt_ff_trolling+0x1d4>
    460c:	e59f0050 	ldr	r0, [pc, #80]	; 4664 <tt_ff_trolling+0x394>
    4610:	e59f401c 	ldr	r4, [pc, #28]	; 4634 <tt_ff_trolling+0x364>
    4614:	e1a0e00f 	mov	lr, pc
    4618:	e594f000 	ldr	pc, [r4]
    461c:	e59f0044 	ldr	r0, [pc, #68]	; 4668 <tt_ff_trolling+0x398>
    4620:	e1a0e00f 	mov	lr, pc
    4624:	e594f000 	ldr	pc, [r4]
    4628:	e3a00000 	mov	r0, #0	; 0x0
    462c:	eaffffef 	b	45f0 <tt_ff_trolling+0x320>
    4630:	0001aed0 	.word	0x0001aed0
    4634:	3c00d694 	.word	0x3c00d694
    4638:	0001aeed 	.word	0x0001aeed
    463c:	0001af0c 	.word	0x0001af0c
    4640:	00019da3 	.word	0x00019da3
    4644:	00019f16 	.word	0x00019f16
    4648:	00019f12 	.word	0x00019f12
    464c:	0001af2a 	.word	0x0001af2a
    4650:	0001af49 	.word	0x0001af49
    4654:	0001af61 	.word	0x0001af61
    4658:	0001af68 	.word	0x0001af68
    465c:	8e000004 	.word	0x8e000004
    4660:	0001af88 	.word	0x0001af88
    4664:	0001af98 	.word	0x0001af98
    4668:	0001af9f 	.word	0x0001af9f

0000466c <do_tt_ff>:
    466c:	e92d4030 	push	{r4, r5, lr}
    4670:	e3520001 	cmp	r2, #1	; 0x1
    4674:	e24dd004 	sub	sp, sp, #4	; 0x4
    4678:	e1a05003 	mov	r5, r3
    467c:	ca000005 	bgt	4698 <do_tt_ff+0x2c>
    4680:	e59f0060 	ldr	r0, [pc, #96]	; 46e8 <do_tt_ff+0x7c>
    4684:	e59f3060 	ldr	r3, [pc, #96]	; 46ec <do_tt_ff+0x80>
    4688:	e1a0e00f 	mov	lr, pc
    468c:	e593f000 	ldr	pc, [r3]
    4690:	e3e00000 	mvn	r0, #0	; 0x0
    4694:	ea000011 	b	46e0 <do_tt_ff+0x74>
    4698:	e3520002 	cmp	r2, #2	; 0x2
    469c:	03a04000 	moveq	r4, #0	; 0x0
    46a0:	0a000009 	beq	46cc <do_tt_ff+0x60>
    46a4:	e3a01000 	mov	r1, #0	; 0x0
    46a8:	e5930008 	ldr	r0, [r3, #8]
    46ac:	e3a0200a 	mov	r2, #10	; 0xa
    46b0:	eb002b15 	bl	f30c <simple_strtoul>
    46b4:	e1a04000 	mov	r4, r0
    46b8:	e1a01004 	mov	r1, r4
    46bc:	e59f002c 	ldr	r0, [pc, #44]	; 46f0 <do_tt_ff+0x84>
    46c0:	e59f3024 	ldr	r3, [pc, #36]	; 46ec <do_tt_ff+0x80>
    46c4:	e1a0e00f 	mov	lr, pc
    46c8:	e593f000 	ldr	pc, [r3]
    46cc:	e5950004 	ldr	r0, [r5, #4]
    46d0:	e1a03624 	lsr	r3, r4, #12
    46d4:	e1a02a04 	lsl	r2, r4, #20
    46d8:	ebfffefc 	bl	42d0 <tt_ff_trolling>
    46dc:	e3a00000 	mov	r0, #0	; 0x0
    46e0:	e28dd004 	add	sp, sp, #4	; 0x4
    46e4:	e8bd8030 	pop	{r4, r5, pc}
    46e8:	0001afb7 	.word	0x0001afb7
    46ec:	3c00d694 	.word	0x3c00d694
    46f0:	0001affd 	.word	0x0001affd

000046f4 <tt_cmp_buffer>:
    46f4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    46f8:	e3a04000 	mov	r4, #0	; 0x0
    46fc:	e24dd004 	sub	sp, sp, #4	; 0x4
    4700:	e1a0a002 	mov	sl, r2
    4704:	e1a07000 	mov	r7, r0
    4708:	e1a06001 	mov	r6, r1
    470c:	e1a08003 	mov	r8, r3
    4710:	e1a09142 	asr	r9, r2, #2
    4714:	e1a05004 	mov	r5, r4
    4718:	e59fb0e4 	ldr	fp, [pc, #228]	; 4804 <tt_cmp_buffer+0x110>
    471c:	ea00001f 	b	47a0 <tt_cmp_buffer+0xac>
    4720:	e5970000 	ldr	r0, [r7]
    4724:	e5961000 	ldr	r1, [r6]
    4728:	e1500001 	cmp	r0, r1
    472c:	0a000018 	beq	4794 <tt_cmp_buffer+0xa0>
    4730:	e5d72000 	ldrb	r2, [r7]
    4734:	e5d63000 	ldrb	r3, [r6]
    4738:	e1520003 	cmp	r2, r3
    473c:	e5d72001 	ldrb	r2, [r7, #1]
    4740:	e5d63001 	ldrb	r3, [r6, #1]
    4744:	12855001 	addne	r5, r5, #1	; 0x1
    4748:	e1520003 	cmp	r2, r3
    474c:	e5d72002 	ldrb	r2, [r7, #2]
    4750:	e5d63002 	ldrb	r3, [r6, #2]
    4754:	12855001 	addne	r5, r5, #1	; 0x1
    4758:	e1520003 	cmp	r2, r3
    475c:	e5d72003 	ldrb	r2, [r7, #3]
    4760:	e5d63003 	ldrb	r3, [r6, #3]
    4764:	12855001 	addne	r5, r5, #1	; 0x1
    4768:	e1520003 	cmp	r2, r3
    476c:	12855001 	addne	r5, r5, #1	; 0x1
    4770:	e3580000 	cmp	r8, #0	; 0x0
    4774:	0a000006 	beq	4794 <tt_cmp_buffer+0xa0>
    4778:	e1a02000 	mov	r2, r0
    477c:	e1a03001 	mov	r3, r1
    4780:	e59f0080 	ldr	r0, [pc, #128]	; 4808 <tt_cmp_buffer+0x114>
    4784:	e1a01104 	lsl	r1, r4, #2
    4788:	e1a0e00f 	mov	lr, pc
    478c:	e59bf000 	ldr	pc, [fp]
    4790:	e2488001 	sub	r8, r8, #1	; 0x1
    4794:	e2844001 	add	r4, r4, #1	; 0x1
    4798:	e2877004 	add	r7, r7, #4	; 0x4
    479c:	e2866004 	add	r6, r6, #4	; 0x4
    47a0:	e1540009 	cmp	r4, r9
    47a4:	baffffdd 	blt	4720 <tt_cmp_buffer+0x2c>
    47a8:	e3ca9003 	bic	r9, sl, #3	; 0x3
    47ac:	e3a04000 	mov	r4, #0	; 0x0
    47b0:	e20aa003 	and	sl, sl, #3	; 0x3
    47b4:	e59fb048 	ldr	fp, [pc, #72]	; 4804 <tt_cmp_buffer+0x110>
    47b8:	ea00000c 	b	47f0 <tt_cmp_buffer+0xfc>
    47bc:	e7d72004 	ldrb	r2, [r7, r4]
    47c0:	e7d63004 	ldrb	r3, [r6, r4]
    47c4:	e1520003 	cmp	r2, r3
    47c8:	0a000007 	beq	47ec <tt_cmp_buffer+0xf8>
    47cc:	e3580000 	cmp	r8, #0	; 0x0
    47d0:	e2855001 	add	r5, r5, #1	; 0x1
    47d4:	0a000004 	beq	47ec <tt_cmp_buffer+0xf8>
    47d8:	e0841009 	add	r1, r4, r9
    47dc:	e59f0028 	ldr	r0, [pc, #40]	; 480c <tt_cmp_buffer+0x118>
    47e0:	e1a0e00f 	mov	lr, pc
    47e4:	e59bf000 	ldr	pc, [fp]
    47e8:	e2488001 	sub	r8, r8, #1	; 0x1
    47ec:	e2844001 	add	r4, r4, #1	; 0x1
    47f0:	e154000a 	cmp	r4, sl
    47f4:	bafffff0 	blt	47bc <tt_cmp_buffer+0xc8>
    47f8:	e1a00005 	mov	r0, r5
    47fc:	e28dd004 	add	sp, sp, #4	; 0x4
    4800:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4804:	3c00d694 	.word	0x3c00d694
    4808:	0001b024 	.word	0x0001b024
    480c:	0001b04f 	.word	0x0001b04f

00004810 <tt_rnd_seed>:
    4810:	e92d4030 	push	{r4, r5, lr}
    4814:	e59f5024 	ldr	r5, [pc, #36]	; 4840 <tt_rnd_seed+0x30>
    4818:	e24dd004 	sub	sp, sp, #4	; 0x4
    481c:	e3a00000 	mov	r0, #0	; 0x0
    4820:	e5954000 	ldr	r4, [r5]
    4824:	eb001b5e 	bl	b5a4 <get_timer>
    4828:	e30f3ffc 	movw	r3, #65532	; 0xfffc
    482c:	e6e40070 	uxtab	r0, r4, r0
    4830:	e0003003 	and	r3, r0, r3
    4834:	e5853000 	str	r3, [r5]
    4838:	e28dd004 	add	sp, sp, #4	; 0x4
    483c:	e8bd8030 	pop	{r4, r5, pc}
    4840:	3c00d158 	.word	0x3c00d158

00004844 <tt_rnd_get_int>:
    4844:	e59fc094 	ldr	ip, [pc, #148]	; 48e0 <tt_rnd_get_int+0x9c>
    4848:	e3a02102 	mov	r2, #-2147483648	; 0x80000000
    484c:	e59c3000 	ldr	r3, [ip]
    4850:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4854:	e19330b2 	ldrh	r3, [r3, r2]
    4858:	e24dd004 	sub	sp, sp, #4	; 0x4
    485c:	e3c33003 	bic	r3, r3, #3	; 0x3
    4860:	e6ff3073 	uxth	r3, r3
    4864:	e19340b2 	ldrh	r4, [r3, r2]
    4868:	e58c3000 	str	r3, [ip]
    486c:	e1a07000 	mov	r7, r0
    4870:	e1a06001 	mov	r6, r1
    4874:	ebffffe5 	bl	4810 <tt_rnd_seed>
    4878:	e1a00004 	mov	r0, r4
    487c:	eb003872 	bl	12a4c <__aeabi_ui2d>
    4880:	e3a02000 	mov	r2, #0	; 0x0
    4884:	e59f3058 	ldr	r3, [pc, #88]	; 48e4 <tt_rnd_get_int+0xa0>
    4888:	eb00394a 	bl	12db8 <__aeabi_ddiv>
    488c:	e1a04000 	mov	r4, r0
    4890:	e0670006 	rsb	r0, r7, r6
    4894:	e1a05001 	mov	r5, r1
    4898:	eb003874 	bl	12a70 <__aeabi_i2d>
    489c:	e1a02000 	mov	r2, r0
    48a0:	e1a03001 	mov	r3, r1
    48a4:	e1a00004 	mov	r0, r4
    48a8:	e1a01005 	mov	r1, r5
    48ac:	eb0038a6 	bl	12b4c <__aeabi_dmul>
    48b0:	e1a04000 	mov	r4, r0
    48b4:	e1a00007 	mov	r0, r7
    48b8:	e1a05001 	mov	r5, r1
    48bc:	eb00386b 	bl	12a70 <__aeabi_i2d>
    48c0:	e1a02000 	mov	r2, r0
    48c4:	e1a03001 	mov	r3, r1
    48c8:	e1a00004 	mov	r0, r4
    48cc:	e1a01005 	mov	r1, r5
    48d0:	eb0037b2 	bl	127a0 <__adddf3>
    48d4:	eb0039b8 	bl	12fbc <__aeabi_d2iz>
    48d8:	e28dd004 	add	sp, sp, #4	; 0x4
    48dc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    48e0:	3c00d158 	.word	0x3c00d158
    48e4:	40efffe0 	.word	0x40efffe0

000048e8 <tt_rnd_get_data>:
    48e8:	e92d4010 	push	{r4, lr}
    48ec:	e1a04000 	mov	r4, r0
    48f0:	e1a00001 	mov	r0, r1
    48f4:	e1a01002 	mov	r1, r2
    48f8:	ebffffd1 	bl	4844 <tt_rnd_get_int>
    48fc:	e59f3010 	ldr	r3, [pc, #16]	; 4914 <tt_rnd_get_data+0x2c>
    4900:	e5840000 	str	r0, [r4]
    4904:	e5930000 	ldr	r0, [r3]
    4908:	e3c00003 	bic	r0, r0, #3	; 0x3
    490c:	e2800102 	add	r0, r0, #-2147483648	; 0x80000000
    4910:	e8bd8010 	pop	{r4, pc}
    4914:	3c00d158 	.word	0x3c00d158

00004918 <do_tt_nbi>:
    4918:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    491c:	e3a00001 	mov	r0, #1	; 0x1
    4920:	e24dd054 	sub	sp, sp, #84	; 0x54
    4924:	ebffef1c 	bl	59c <get_xom>
    4928:	e59f30d0 	ldr	r3, [pc, #208]	; 4a00 <do_tt_nbi+0xe8>
    492c:	e28d4010 	add	r4, sp, #16	; 0x10
    4930:	e1a0c003 	mov	ip, r3
    4934:	e1a06000 	mov	r6, r0
    4938:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    493c:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    4940:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    4944:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    4948:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    494c:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    4950:	e89c000f 	ldm	ip, {r0, r1, r2, r3}
    4954:	e59fa0a8 	ldr	sl, [pc, #168]	; 4a04 <do_tt_nbi+0xec>
    4958:	e59f80a8 	ldr	r8, [pc, #168]	; 4a08 <do_tt_nbi+0xf0>
    495c:	e59f70a8 	ldr	r7, [pc, #168]	; 4a0c <do_tt_nbi+0xf4>
    4960:	e884000f 	stm	r4, {r0, r1, r2, r3}
    4964:	e3a0400a 	mov	r4, #10	; 0xa
    4968:	e1a00004 	mov	r0, r4
    496c:	ebffef0e 	bl	5ac <set_xom>
    4970:	eb0018aa 	bl	ac20 <nand_rescan_bootcfg>
    4974:	e3a00000 	mov	r0, #0	; 0x0
    4978:	eb0017f0 	bl	a940 <nand_get_config>
    497c:	e5d03005 	ldrb	r3, [r0, #5]
    4980:	e5d02006 	ldrb	r2, [r0, #6]
    4984:	e3530000 	cmp	r3, #0	; 0x0
    4988:	15d03003 	ldrbne	r3, [r0, #3]
    498c:	128d1050 	addne	r1, sp, #80	; 0x50
    4990:	10813103 	addne	r3, r1, r3, lsl #2
    4994:	e5d01000 	ldrb	r1, [r0]
    4998:	15133040 	ldrne	r3, [r3, #-64]
    499c:	e3510000 	cmp	r1, #0	; 0x0
    49a0:	e590e018 	ldr	lr, [r0, #24]
    49a4:	e59a5000 	ldr	r5, [sl]
    49a8:	e5d0c002 	ldrb	ip, [r0, #2]
    49ac:	059f105c 	ldreq	r1, [pc, #92]	; 4a10 <do_tt_nbi+0xf8>
    49b0:	0a000002 	beq	49c0 <do_tt_nbi+0xa8>
    49b4:	e3510001 	cmp	r1, #1	; 0x1
    49b8:	01a01008 	moveq	r1, r8
    49bc:	11a01007 	movne	r1, r7
    49c0:	e58d1008 	str	r1, [sp, #8]
    49c4:	e88d1008 	stm	sp, {r3, ip}
    49c8:	e5d03001 	ldrb	r3, [r0, #1]
    49cc:	e1a01004 	mov	r1, r4
    49d0:	e58d300c 	str	r3, [sp, #12]
    49d4:	e2844001 	add	r4, r4, #1	; 0x1
    49d8:	e1a0300e 	mov	r3, lr
    49dc:	e59f0030 	ldr	r0, [pc, #48]	; 4a14 <do_tt_nbi+0xfc>
    49e0:	e12fff35 	blx	r5
    49e4:	e354001c 	cmp	r4, #28	; 0x1c
    49e8:	1affffde 	bne	4968 <do_tt_nbi+0x50>
    49ec:	e1a00006 	mov	r0, r6
    49f0:	ebffeeed 	bl	5ac <set_xom>
    49f4:	e3a00000 	mov	r0, #0	; 0x0
    49f8:	e28dd054 	add	sp, sp, #84	; 0x54
    49fc:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    4a00:	000130e8 	.word	0x000130e8
    4a04:	3c00d694 	.word	0x3c00d694
    4a08:	0001b088 	.word	0x0001b088
    4a0c:	0001b081 	.word	0x0001b081
    4a10:	0001b07a 	.word	0x0001b07a
    4a14:	0001b08d 	.word	0x0001b08d

00004a18 <tt_simple_test>:
    4a18:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    4a1c:	e1a0a001 	mov	sl, r1
    4a20:	e24dd044 	sub	sp, sp, #68	; 0x44
    4a24:	e3a01001 	mov	r1, #1	; 0x1
    4a28:	e1a05000 	mov	r5, r0
    4a2c:	eb0006ef 	bl	65f0 <vs_assign_by_name>
    4a30:	e3500000 	cmp	r0, #0	; 0x0
    4a34:	13e00000 	mvnne	r0, #0	; 0x0
    4a38:	1a000048 	bne	4b60 <tt_simple_test+0x148>
    4a3c:	e1a00005 	mov	r0, r5
    4a40:	e59f1120 	ldr	r1, [pc, #288]	; 4b68 <tt_simple_test+0x150>
    4a44:	e3a02003 	mov	r2, #3	; 0x3
    4a48:	ebfff2af 	bl	150c <strncmp>
    4a4c:	e3500000 	cmp	r0, #0	; 0x0
    4a50:	1a000001 	bne	4a5c <tt_simple_test+0x44>
    4a54:	eb0017f6 	bl	aa34 <fnd_align>
    4a58:	ea000001 	b	4a64 <tt_simple_test+0x4c>
    4a5c:	e3a0000e 	mov	r0, #14	; 0xe
    4a60:	eb000570 	bl	6028 <vs_align>
    4a64:	e1a06400 	lsl	r6, r0, #8
    4a68:	e1a00005 	mov	r0, r5
    4a6c:	eb0005ee 	bl	622c <vs_device_id>
    4a70:	e59f80f4 	ldr	r8, [pc, #244]	; 4b6c <tt_simple_test+0x154>
    4a74:	e1a07000 	mov	r7, r0
    4a78:	e5984000 	ldr	r4, [r8]
    4a7c:	eb000569 	bl	6028 <vs_align>
    4a80:	e1a01005 	mov	r1, r5
    4a84:	e1a02000 	mov	r2, r0
    4a88:	e59f00e0 	ldr	r0, [pc, #224]	; 4b70 <tt_simple_test+0x158>
    4a8c:	e12fff34 	blx	r4
    4a90:	e35a0000 	cmp	sl, #0	; 0x0
    4a94:	1a00001a 	bne	4b04 <tt_simple_test+0xec>
    4a98:	e1a00007 	mov	r0, r7
    4a9c:	eb0005ce 	bl	61dc <vs_device_erasable>
    4aa0:	e3500000 	cmp	r0, #0	; 0x0
    4aa4:	0a000009 	beq	4ad0 <tt_simple_test+0xb8>
    4aa8:	e59f00c4 	ldr	r0, [pc, #196]	; 4b74 <tt_simple_test+0x15c>
    4aac:	e1a0e00f 	mov	lr, pc
    4ab0:	e598f000 	ldr	pc, [r8]
    4ab4:	e59f10bc 	ldr	r1, [pc, #188]	; 4b78 <tt_simple_test+0x160>
    4ab8:	e1a0000d 	mov	r0, sp
    4abc:	e1a02006 	mov	r2, r6
    4ac0:	eb002d9d 	bl	1013c <sprintf>
    4ac4:	e1a0000d 	mov	r0, sp
    4ac8:	e1a0100a 	mov	r1, sl
    4acc:	eb002e62 	bl	1045c <run_command>
    4ad0:	e59f3094 	ldr	r3, [pc, #148]	; 4b6c <tt_simple_test+0x154>
    4ad4:	e59f00a0 	ldr	r0, [pc, #160]	; 4b7c <tt_simple_test+0x164>
    4ad8:	e1a0e00f 	mov	lr, pc
    4adc:	e593f000 	ldr	pc, [r3]
    4ae0:	e1a02006 	mov	r2, r6
    4ae4:	e59f1094 	ldr	r1, [pc, #148]	; 4b80 <tt_simple_test+0x168>
    4ae8:	e1a0000d 	mov	r0, sp
    4aec:	eb002d92 	bl	1013c <sprintf>
    4af0:	e1a0000d 	mov	r0, sp
    4af4:	e3a01000 	mov	r1, #0	; 0x0
    4af8:	e1a0400d 	mov	r4, sp
    4afc:	eb002e56 	bl	1045c <run_command>
    4b00:	ea000015 	b	4b5c <tt_simple_test+0x144>
    4b04:	e59f0078 	ldr	r0, [pc, #120]	; 4b84 <tt_simple_test+0x16c>
    4b08:	e1a0e00f 	mov	lr, pc
    4b0c:	e598f000 	ldr	pc, [r8]
    4b10:	e1a02006 	mov	r2, r6
    4b14:	e59f106c 	ldr	r1, [pc, #108]	; 4b88 <tt_simple_test+0x170>
    4b18:	e1a0000d 	mov	r0, sp
    4b1c:	eb002d86 	bl	1013c <sprintf>
    4b20:	e3a01000 	mov	r1, #0	; 0x0
    4b24:	e1a0000d 	mov	r0, sp
    4b28:	eb002e4b 	bl	1045c <run_command>
    4b2c:	e59f0058 	ldr	r0, [pc, #88]	; 4b8c <tt_simple_test+0x174>
    4b30:	e1a0e00f 	mov	lr, pc
    4b34:	e598f000 	ldr	pc, [r8]
    4b38:	e1a02006 	mov	r2, r6
    4b3c:	e59f104c 	ldr	r1, [pc, #76]	; 4b90 <tt_simple_test+0x178>
    4b40:	e3a03003 	mov	r3, #3	; 0x3
    4b44:	e59f0048 	ldr	r0, [pc, #72]	; 4b94 <tt_simple_test+0x17c>
    4b48:	ebfffee9 	bl	46f4 <tt_cmp_buffer>
    4b4c:	e59f0044 	ldr	r0, [pc, #68]	; 4b98 <tt_simple_test+0x180>
    4b50:	e1a0400d 	mov	r4, sp
    4b54:	e1a0e00f 	mov	lr, pc
    4b58:	e598f000 	ldr	pc, [r8]
    4b5c:	e3a00000 	mov	r0, #0	; 0x0
    4b60:	e28dd044 	add	sp, sp, #68	; 0x44
    4b64:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    4b68:	00019f16 	.word	0x00019f16
    4b6c:	3c00d694 	.word	0x3c00d694
    4b70:	0001b389 	.word	0x0001b389
    4b74:	0001b0a9 	.word	0x0001b0a9
    4b78:	0001b0b6 	.word	0x0001b0b6
    4b7c:	0001b0cb 	.word	0x0001b0cb
    4b80:	0001b0d8 	.word	0x0001b0d8
    4b84:	0001b0f6 	.word	0x0001b0f6
    4b88:	0001b103 	.word	0x0001b103
    4b8c:	0001b120 	.word	0x0001b120
    4b90:	50008000 	.word	0x50008000
    4b94:	40008000 	.word	0x40008000
    4b98:	0001a882 	.word	0x0001a882

00004b9c <do_tt_randomizer>:
    4b9c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4ba0:	e24dd064 	sub	sp, sp, #100	; 0x64
    4ba4:	e1a06002 	mov	r6, r2
    4ba8:	e28d0010 	add	r0, sp, #16	; 0x10
    4bac:	e59f12dc 	ldr	r1, [pc, #732]	; 4e90 <do_tt_randomizer+0x2f4>
    4bb0:	e3a02050 	mov	r2, #80	; 0x50
    4bb4:	e1a05003 	mov	r5, r3
    4bb8:	ebfff28c 	bl	15f0 <memcpy>
    4bbc:	e3560001 	cmp	r6, #1	; 0x1
    4bc0:	d59f02cc 	ldrle	r0, [pc, #716]	; 4e94 <do_tt_randomizer+0x2f8>
    4bc4:	da000009 	ble	4bf0 <do_tt_randomizer+0x54>
    4bc8:	e5950004 	ldr	r0, [r5, #4]
    4bcc:	e59f12c4 	ldr	r1, [pc, #708]	; 4e98 <do_tt_randomizer+0x2fc>
    4bd0:	e3a02003 	mov	r2, #3	; 0x3
    4bd4:	ebfff24c 	bl	150c <strncmp>
    4bd8:	e2504000 	subs	r4, r0, #0	; 0x0
    4bdc:	e2858004 	add	r8, r5, #4	; 0x4
    4be0:	1a000010 	bne	4c28 <do_tt_randomizer+0x8c>
    4be4:	e3560002 	cmp	r6, #2	; 0x2
    4be8:	1a000005 	bne	4c04 <do_tt_randomizer+0x68>
    4bec:	e59f02a8 	ldr	r0, [pc, #680]	; 4e9c <do_tt_randomizer+0x300>
    4bf0:	e59f32a8 	ldr	r3, [pc, #680]	; 4ea0 <do_tt_randomizer+0x304>
    4bf4:	e1a0e00f 	mov	lr, pc
    4bf8:	e593f000 	ldr	pc, [r3]
    4bfc:	e3e00000 	mvn	r0, #0	; 0x0
    4c00:	ea0000a0 	b	4e88 <do_tt_randomizer+0x2ec>
    4c04:	e1a01004 	mov	r1, r4
    4c08:	e3a02010 	mov	r2, #16	; 0x10
    4c0c:	e5950008 	ldr	r0, [r5, #8]
    4c10:	eb0029bd 	bl	f30c <simple_strtoul>
    4c14:	ebffee64 	bl	5ac <set_xom>
    4c18:	e1a00004 	mov	r0, r4
    4c1c:	eb0017fa 	bl	ac0c <nand_rescan_config>
    4c20:	e1a00004 	mov	r0, r4
    4c24:	ea000000 	b	4c2c <do_tt_randomizer+0x90>
    4c28:	e3a00001 	mov	r0, #1	; 0x1
    4c2c:	eb001743 	bl	a940 <nand_get_config>
    4c30:	e3500000 	cmp	r0, #0	; 0x0
    4c34:	e1a05000 	mov	r5, r0
    4c38:	059f0264 	ldreq	r0, [pc, #612]	; 4ea4 <do_tt_randomizer+0x308>
    4c3c:	0affffeb 	beq	4bf0 <do_tt_randomizer+0x54>
    4c40:	e3a00001 	mov	r0, #1	; 0x1
    4c44:	e3a01000 	mov	r1, #0	; 0x0
    4c48:	e59fb250 	ldr	fp, [pc, #592]	; 4ea0 <do_tt_randomizer+0x304>
    4c4c:	e58d0008 	str	r0, [sp, #8]
    4c50:	e58d100c 	str	r1, [sp, #12]
    4c54:	e59d000c 	ldr	r0, [sp, #12]
    4c58:	e59f2248 	ldr	r2, [pc, #584]	; 4ea8 <do_tt_randomizer+0x30c>
    4c5c:	e3a09000 	mov	r9, #0	; 0x0
    4c60:	e7923000 	ldr	r3, [r2, r0]
    4c64:	e59f0240 	ldr	r0, [pc, #576]	; 4eac <do_tt_randomizer+0x310>
    4c68:	e1a01003 	mov	r1, r3
    4c6c:	e585304c 	str	r3, [r5, #76]
    4c70:	e59f2228 	ldr	r2, [pc, #552]	; 4ea0 <do_tt_randomizer+0x304>
    4c74:	e1a0e00f 	mov	lr, pc
    4c78:	e592f000 	ldr	pc, [r2]
    4c7c:	e59f1224 	ldr	r1, [pc, #548]	; 4ea8 <do_tt_randomizer+0x30c>
    4c80:	e59d200c 	ldr	r2, [sp, #12]
    4c84:	e59d0008 	ldr	r0, [sp, #8]
    4c88:	e0813002 	add	r3, r1, r2
    4c8c:	e3a0100a 	mov	r1, #10	; 0xa
    4c90:	e58d3000 	str	r3, [sp]
    4c94:	eb0036aa 	bl	12744 <__aeabi_idivmod>
    4c98:	e28da010 	add	sl, sp, #16	; 0x10
    4c9c:	e58d1004 	str	r1, [sp, #4]
    4ca0:	ea00006a 	b	4e50 <do_tt_randomizer+0x2b4>
    4ca4:	e4972004 	ldr	r2, [r7], #4
    4ca8:	e1a01004 	mov	r1, r4
    4cac:	e586203c 	str	r2, [r6, #60]
    4cb0:	e2844001 	add	r4, r4, #1	; 0x1
    4cb4:	e59f01f4 	ldr	r0, [pc, #500]	; 4eb0 <do_tt_randomizer+0x314>
    4cb8:	e59f31e0 	ldr	r3, [pc, #480]	; 4ea0 <do_tt_randomizer+0x304>
    4cbc:	e1a0e00f 	mov	lr, pc
    4cc0:	e593f000 	ldr	pc, [r3]
    4cc4:	e3540004 	cmp	r4, #4	; 0x4
    4cc8:	e2866004 	add	r6, r6, #4	; 0x4
    4ccc:	1afffff4 	bne	4ca4 <do_tt_randomizer+0x108>
    4cd0:	e3a00001 	mov	r0, #1	; 0x1
    4cd4:	e5c50001 	strb	r0, [r5, #1]
    4cd8:	e3a00000 	mov	r0, #0	; 0x0
    4cdc:	eb001a30 	bl	b5a4 <get_timer>
    4ce0:	e1a04000 	mov	r4, r0
    4ce4:	e1a00005 	mov	r0, r5
    4ce8:	eb0017ce 	bl	ac28 <nand_init_randomizer>
    4cec:	e1a00004 	mov	r0, r4
    4cf0:	eb001a2b 	bl	b5a4 <get_timer>
    4cf4:	e1a01000 	mov	r1, r0
    4cf8:	e59f01b4 	ldr	r0, [pc, #436]	; 4eb4 <do_tt_randomizer+0x318>
    4cfc:	e1a0e00f 	mov	lr, pc
    4d00:	e59bf000 	ldr	pc, [fp]
    4d04:	e3a01000 	mov	r1, #0	; 0x0
    4d08:	e5980000 	ldr	r0, [r8]
    4d0c:	ebffff41 	bl	4a18 <tt_simple_test>
    4d10:	e3a01000 	mov	r1, #0	; 0x0
    4d14:	e5c51001 	strb	r1, [r5, #1]
    4d18:	e59f0198 	ldr	r0, [pc, #408]	; 4eb8 <do_tt_randomizer+0x31c>
    4d1c:	e1a0e00f 	mov	lr, pc
    4d20:	e59bf000 	ldr	pc, [fp]
    4d24:	e3a01001 	mov	r1, #1	; 0x1
    4d28:	e5980000 	ldr	r0, [r8]
    4d2c:	ebffff39 	bl	4a18 <tt_simple_test>
    4d30:	e3a02001 	mov	r2, #1	; 0x1
    4d34:	e5c52001 	strb	r2, [r5, #1]
    4d38:	e59f017c 	ldr	r0, [pc, #380]	; 4ebc <do_tt_randomizer+0x320>
    4d3c:	e1a0e00f 	mov	lr, pc
    4d40:	e59bf000 	ldr	pc, [fp]
    4d44:	e3a01001 	mov	r1, #1	; 0x1
    4d48:	e5980000 	ldr	r0, [r8]
    4d4c:	ebffff31 	bl	4a18 <tt_simple_test>
    4d50:	e2899001 	add	r9, r9, #1	; 0x1
    4d54:	e59f0164 	ldr	r0, [pc, #356]	; 4ec0 <do_tt_randomizer+0x324>
    4d58:	e1a0e00f 	mov	lr, pc
    4d5c:	e59bf000 	ldr	pc, [fp]
    4d60:	e1a00009 	mov	r0, r9
    4d64:	e3a01005 	mov	r1, #5	; 0x5
    4d68:	eb003675 	bl	12744 <__aeabi_idivmod>
    4d6c:	e28d3060 	add	r3, sp, #96	; 0x60
    4d70:	e083c201 	add	ip, r3, r1, lsl #4
    4d74:	e51c3050 	ldr	r3, [ip, #-80]
    4d78:	e1a00005 	mov	r0, r5
    4d7c:	e585303c 	str	r3, [r5, #60]
    4d80:	e51c304c 	ldr	r3, [ip, #-76]
    4d84:	e5853040 	str	r3, [r5, #64]
    4d88:	e51c3048 	ldr	r3, [ip, #-72]
    4d8c:	e5853044 	str	r3, [r5, #68]
    4d90:	e51c3044 	ldr	r3, [ip, #-68]
    4d94:	e5853048 	str	r3, [r5, #72]
    4d98:	eb0017a2 	bl	ac28 <nand_init_randomizer>
    4d9c:	e3a01001 	mov	r1, #1	; 0x1
    4da0:	e5980000 	ldr	r0, [r8]
    4da4:	ebffff1b 	bl	4a18 <tt_simple_test>
    4da8:	e59f0114 	ldr	r0, [pc, #276]	; 4ec4 <do_tt_randomizer+0x328>
    4dac:	e1a0e00f 	mov	lr, pc
    4db0:	e59bf000 	ldr	pc, [fp]
    4db4:	e59a3000 	ldr	r3, [sl]
    4db8:	e1a00005 	mov	r0, r5
    4dbc:	e585303c 	str	r3, [r5, #60]
    4dc0:	e59a3004 	ldr	r3, [sl, #4]
    4dc4:	e5853040 	str	r3, [r5, #64]
    4dc8:	e59a3008 	ldr	r3, [sl, #8]
    4dcc:	e5853044 	str	r3, [r5, #68]
    4dd0:	e59a300c 	ldr	r3, [sl, #12]
    4dd4:	e28aa010 	add	sl, sl, #16	; 0x10
    4dd8:	e5853048 	str	r3, [r5, #72]
    4ddc:	eb001791 	bl	ac28 <nand_init_randomizer>
    4de0:	e3a01001 	mov	r1, #1	; 0x1
    4de4:	e5980000 	ldr	r0, [r8]
    4de8:	ebffff0a 	bl	4a18 <tt_simple_test>
    4dec:	e59f00d4 	ldr	r0, [pc, #212]	; 4ec8 <do_tt_randomizer+0x32c>
    4df0:	e1a0e00f 	mov	lr, pc
    4df4:	e59bf000 	ldr	pc, [fp]
    4df8:	e59f20a8 	ldr	r2, [pc, #168]	; 4ea8 <do_tt_randomizer+0x30c>
    4dfc:	e59d1004 	ldr	r1, [sp, #4]
    4e00:	e1a00005 	mov	r0, r5
    4e04:	e7923101 	ldr	r3, [r2, r1, lsl #2]
    4e08:	e585304c 	str	r3, [r5, #76]
    4e0c:	eb001785 	bl	ac28 <nand_init_randomizer>
    4e10:	e3a01001 	mov	r1, #1	; 0x1
    4e14:	e5980000 	ldr	r0, [r8]
    4e18:	ebfffefe 	bl	4a18 <tt_simple_test>
    4e1c:	e59f00a8 	ldr	r0, [pc, #168]	; 4ecc <do_tt_randomizer+0x330>
    4e20:	e1a0e00f 	mov	lr, pc
    4e24:	e59bf000 	ldr	pc, [fp]
    4e28:	e59d0000 	ldr	r0, [sp]
    4e2c:	e5903000 	ldr	r3, [r0]
    4e30:	e1a00005 	mov	r0, r5
    4e34:	e585304c 	str	r3, [r5, #76]
    4e38:	eb00177a 	bl	ac28 <nand_init_randomizer>
    4e3c:	e5980000 	ldr	r0, [r8]
    4e40:	e3a01001 	mov	r1, #1	; 0x1
    4e44:	ebfffef3 	bl	4a18 <tt_simple_test>
    4e48:	e3590004 	cmp	r9, #4	; 0x4
    4e4c:	0a000004 	beq	4e64 <do_tt_randomizer+0x2c8>
    4e50:	e28d1010 	add	r1, sp, #16	; 0x10
    4e54:	e0817209 	add	r7, r1, r9, lsl #4
    4e58:	e1a06005 	mov	r6, r5
    4e5c:	e3a04000 	mov	r4, #0	; 0x0
    4e60:	eaffff8f 	b	4ca4 <do_tt_randomizer+0x108>
    4e64:	e59d2008 	ldr	r2, [sp, #8]
    4e68:	e59d300c 	ldr	r3, [sp, #12]
    4e6c:	e2822001 	add	r2, r2, #1	; 0x1
    4e70:	e2833004 	add	r3, r3, #4	; 0x4
    4e74:	e3520006 	cmp	r2, #6	; 0x6
    4e78:	e58d2008 	str	r2, [sp, #8]
    4e7c:	e58d300c 	str	r3, [sp, #12]
    4e80:	1affff73 	bne	4c54 <do_tt_randomizer+0xb8>
    4e84:	e3a00000 	mov	r0, #0	; 0x0
    4e88:	e28dd064 	add	sp, sp, #100	; 0x64
    4e8c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4e90:	00013128 	.word	0x00013128
    4e94:	0001b12f 	.word	0x0001b12f
    4e98:	00019da3 	.word	0x00019da3
    4e9c:	0001b14f 	.word	0x0001b14f
    4ea0:	3c00d694 	.word	0x3c00d694
    4ea4:	0001b166 	.word	0x0001b166
    4ea8:	0000ef70 	.word	0x0000ef70
    4eac:	0001b18b 	.word	0x0001b18b
    4eb0:	0001b1a3 	.word	0x0001b1a3
    4eb4:	0001b1bd 	.word	0x0001b1bd
    4eb8:	0001b1e0 	.word	0x0001b1e0
    4ebc:	0001b209 	.word	0x0001b209
    4ec0:	0001b231 	.word	0x0001b231
    4ec4:	0001b259 	.word	0x0001b259
    4ec8:	0001b286 	.word	0x0001b286
    4ecc:	0001b2ae 	.word	0x0001b2ae

00004ed0 <tt_nand_cross_cmp>:
    4ed0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4ed4:	e24dd004 	sub	sp, sp, #4	; 0x4
    4ed8:	e59da028 	ldr	sl, [sp, #40]
    4edc:	e3530000 	cmp	r3, #0	; 0x0
    4ee0:	e1a05002 	mov	r5, r2
    4ee4:	13a02000 	movne	r2, #0	; 0x0
    4ee8:	059a2014 	ldreq	r2, [sl, #20]
    4eec:	e3530000 	cmp	r3, #0	; 0x0
    4ef0:	e1a06001 	mov	r6, r1
    4ef4:	159a3014 	ldrne	r3, [sl, #20]
    4ef8:	e59a1018 	ldr	r1, [sl, #24]
    4efc:	e1a07000 	mov	r7, r0
    4f00:	e0838001 	add	r8, r3, r1
    4f04:	e082b001 	add	fp, r2, r1
    4f08:	e3a04000 	mov	r4, #0	; 0x0
    4f0c:	e2689000 	rsb	r9, r8, #0	; 0x0
    4f10:	e3a03000 	mov	r3, #0	; 0x0
    4f14:	e1550003 	cmp	r5, r3
    4f18:	e1a00007 	mov	r0, r7
    4f1c:	e1a01006 	mov	r1, r6
    4f20:	e087700b 	add	r7, r7, fp
    4f24:	e0866008 	add	r6, r6, r8
    4f28:	e0855009 	add	r5, r5, r9
    4f2c:	da000003 	ble	4f40 <tt_nand_cross_cmp+0x70>
    4f30:	e59a2018 	ldr	r2, [sl, #24]
    4f34:	ebfffdee 	bl	46f4 <tt_cmp_buffer>
    4f38:	e0844000 	add	r4, r4, r0
    4f3c:	eafffff3 	b	4f10 <tt_nand_cross_cmp+0x40>
    4f40:	e1a00004 	mov	r0, r4
    4f44:	e28dd004 	add	sp, sp, #4	; 0x4
    4f48:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

00004f4c <do_tt_sysinfo>:
    4f4c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4f50:	e3520001 	cmp	r2, #1	; 0x1
    4f54:	e24dd044 	sub	sp, sp, #68	; 0x44
    4f58:	e1a04003 	mov	r4, r3
    4f5c:	ca000003 	bgt	4f70 <do_tt_sysinfo+0x24>
    4f60:	e59f01ac 	ldr	r0, [pc, #428]	; 5114 <do_tt_sysinfo+0x1c8>
    4f64:	e59f31ac 	ldr	r3, [pc, #428]	; 5118 <do_tt_sysinfo+0x1cc>
    4f68:	e1a0e00f 	mov	lr, pc
    4f6c:	e593f000 	ldr	pc, [r3]
    4f70:	e3a00001 	mov	r0, #1	; 0x1
    4f74:	eb001671 	bl	a940 <nand_get_config>
    4f78:	e2505000 	subs	r5, r0, #0	; 0x0
    4f7c:	1a000004 	bne	4f94 <do_tt_sysinfo+0x48>
    4f80:	e59f0194 	ldr	r0, [pc, #404]	; 511c <do_tt_sysinfo+0x1d0>
    4f84:	e59f318c 	ldr	r3, [pc, #396]	; 5118 <do_tt_sysinfo+0x1cc>
    4f88:	e1a0e00f 	mov	lr, pc
    4f8c:	e593f000 	ldr	pc, [r3]
    4f90:	ea000004 	b	4fa8 <do_tt_sysinfo+0x5c>
    4f94:	e59f0184 	ldr	r0, [pc, #388]	; 5120 <do_tt_sysinfo+0x1d4>
    4f98:	e3a01001 	mov	r1, #1	; 0x1
    4f9c:	eb000593 	bl	65f0 <vs_assign_by_name>
    4fa0:	e2507000 	subs	r7, r0, #0	; 0x0
    4fa4:	0a000001 	beq	4fb0 <do_tt_sysinfo+0x64>
    4fa8:	e3e00000 	mvn	r0, #0	; 0x0
    4fac:	ea000056 	b	510c <do_tt_sysinfo+0x1c0>
    4fb0:	e1a01007 	mov	r1, r7
    4fb4:	e3a0200a 	mov	r2, #10	; 0xa
    4fb8:	e5940004 	ldr	r0, [r4, #4]
    4fbc:	eb0028d2 	bl	f30c <simple_strtoul>
    4fc0:	e5850014 	str	r0, [r5, #20]
    4fc4:	e1a04000 	mov	r4, r0
    4fc8:	eb001699 	bl	aa34 <fnd_align>
    4fcc:	e1a06480 	lsl	r6, r0, #9
    4fd0:	e1a01004 	mov	r1, r4
    4fd4:	e59f513c 	ldr	r5, [pc, #316]	; 5118 <do_tt_sysinfo+0x1cc>
    4fd8:	e59f0144 	ldr	r0, [pc, #324]	; 5124 <do_tt_sysinfo+0x1d8>
    4fdc:	e1a02006 	mov	r2, r6
    4fe0:	e1a0e00f 	mov	lr, pc
    4fe4:	e595f000 	ldr	pc, [r5]
    4fe8:	e1a0400d 	mov	r4, sp
    4fec:	e1a01007 	mov	r1, r7
    4ff0:	e59f0130 	ldr	r0, [pc, #304]	; 5128 <do_tt_sysinfo+0x1dc>
    4ff4:	e1a0e00f 	mov	lr, pc
    4ff8:	e595f000 	ldr	pc, [r5]
    4ffc:	e59f0128 	ldr	r0, [pc, #296]	; 512c <do_tt_sysinfo+0x1e0>
    5000:	e1a0e00f 	mov	lr, pc
    5004:	e595f000 	ldr	pc, [r5]
    5008:	e1a02006 	mov	r2, r6
    500c:	e59f111c 	ldr	r1, [pc, #284]	; 5130 <do_tt_sysinfo+0x1e4>
    5010:	e1a0000d 	mov	r0, sp
    5014:	eb002c48 	bl	1013c <sprintf>
    5018:	e3a01000 	mov	r1, #0	; 0x0
    501c:	e1a0000d 	mov	r0, sp
    5020:	eb002d0d 	bl	1045c <run_command>
    5024:	eb002ccf 	bl	10368 <tstc>
    5028:	e2873001 	add	r3, r7, #1	; 0x1
    502c:	e3500000 	cmp	r0, #0	; 0x0
    5030:	e1a07003 	mov	r7, r3
    5034:	0a000002 	beq	5044 <do_tt_sysinfo+0xf8>
    5038:	eb002ccb 	bl	1036c <getc>
    503c:	e3500003 	cmp	r0, #3	; 0x3
    5040:	0a000030 	beq	5108 <do_tt_sysinfo+0x1bc>
    5044:	e59f00e8 	ldr	r0, [pc, #232]	; 5134 <do_tt_sysinfo+0x1e8>
    5048:	e1a0e00f 	mov	lr, pc
    504c:	e595f000 	ldr	pc, [r5]
    5050:	e1a02006 	mov	r2, r6
    5054:	e59f10dc 	ldr	r1, [pc, #220]	; 5138 <do_tt_sysinfo+0x1ec>
    5058:	e1a0000d 	mov	r0, sp
    505c:	eb002c36 	bl	1013c <sprintf>
    5060:	e3a01000 	mov	r1, #0	; 0x0
    5064:	e1a0000d 	mov	r0, sp
    5068:	eb002cfb 	bl	1045c <run_command>
    506c:	eb002cbd 	bl	10368 <tstc>
    5070:	e3500000 	cmp	r0, #0	; 0x0
    5074:	0a000002 	beq	5084 <do_tt_sysinfo+0x138>
    5078:	eb002cbb 	bl	1036c <getc>
    507c:	e3500003 	cmp	r0, #3	; 0x3
    5080:	0a000020 	beq	5108 <do_tt_sysinfo+0x1bc>
    5084:	e59f00b0 	ldr	r0, [pc, #176]	; 513c <do_tt_sysinfo+0x1f0>
    5088:	e1a0e00f 	mov	lr, pc
    508c:	e595f000 	ldr	pc, [r5]
    5090:	e1a02006 	mov	r2, r6
    5094:	e59f10a4 	ldr	r1, [pc, #164]	; 5140 <do_tt_sysinfo+0x1f4>
    5098:	e1a0000d 	mov	r0, sp
    509c:	eb002c26 	bl	1013c <sprintf>
    50a0:	e3a01000 	mov	r1, #0	; 0x0
    50a4:	e1a0000d 	mov	r0, sp
    50a8:	eb002ceb 	bl	1045c <run_command>
    50ac:	eb002cad 	bl	10368 <tstc>
    50b0:	e3500000 	cmp	r0, #0	; 0x0
    50b4:	0a000002 	beq	50c4 <do_tt_sysinfo+0x178>
    50b8:	eb002cab 	bl	1036c <getc>
    50bc:	e3500003 	cmp	r0, #3	; 0x3
    50c0:	0a000010 	beq	5108 <do_tt_sysinfo+0x1bc>
    50c4:	e59f0078 	ldr	r0, [pc, #120]	; 5144 <do_tt_sysinfo+0x1f8>
    50c8:	e1a0e00f 	mov	lr, pc
    50cc:	e595f000 	ldr	pc, [r5]
    50d0:	e59f1070 	ldr	r1, [pc, #112]	; 5148 <do_tt_sysinfo+0x1fc>
    50d4:	e1a02006 	mov	r2, r6
    50d8:	e3a0300c 	mov	r3, #12	; 0xc
    50dc:	e59f0068 	ldr	r0, [pc, #104]	; 514c <do_tt_sysinfo+0x200>
    50e0:	ebfffd83 	bl	46f4 <tt_cmp_buffer>
    50e4:	e59f0064 	ldr	r0, [pc, #100]	; 5150 <do_tt_sysinfo+0x204>
    50e8:	e1a0e00f 	mov	lr, pc
    50ec:	e595f000 	ldr	pc, [r5]
    50f0:	eb002c9c 	bl	10368 <tstc>
    50f4:	e3500000 	cmp	r0, #0	; 0x0
    50f8:	0affffbb 	beq	4fec <do_tt_sysinfo+0xa0>
    50fc:	eb002c9a 	bl	1036c <getc>
    5100:	e3500003 	cmp	r0, #3	; 0x3
    5104:	1affffb8 	bne	4fec <do_tt_sysinfo+0xa0>
    5108:	e3a00000 	mov	r0, #0	; 0x0
    510c:	e28dd044 	add	sp, sp, #68	; 0x44
    5110:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5114:	0001b2db 	.word	0x0001b2db
    5118:	3c00d694 	.word	0x3c00d694
    511c:	0001b2f2 	.word	0x0001b2f2
    5120:	00019f16 	.word	0x00019f16
    5124:	0001b30d 	.word	0x0001b30d
    5128:	0001abf5 	.word	0x0001abf5
    512c:	0001b0a9 	.word	0x0001b0a9
    5130:	0001b32e 	.word	0x0001b32e
    5134:	0001b0cb 	.word	0x0001b0cb
    5138:	0001b342 	.word	0x0001b342
    513c:	0001b0f6 	.word	0x0001b0f6
    5140:	0001b35f 	.word	0x0001b35f
    5144:	0001b120 	.word	0x0001b120
    5148:	50008000 	.word	0x50008000
    514c:	40008000 	.word	0x40008000
    5150:	0001a882 	.word	0x0001a882

00005154 <do_tt_sn>:
    5154:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    5158:	e3520001 	cmp	r2, #1	; 0x1
    515c:	e24dd044 	sub	sp, sp, #68	; 0x44
    5160:	e1a04002 	mov	r4, r2
    5164:	e1a05003 	mov	r5, r3
    5168:	ca000003 	bgt	517c <do_tt_sn+0x28>
    516c:	e59f01d8 	ldr	r0, [pc, #472]	; 534c <do_tt_sn+0x1f8>
    5170:	e59f31d8 	ldr	r3, [pc, #472]	; 5350 <do_tt_sn+0x1fc>
    5174:	e1a0e00f 	mov	lr, pc
    5178:	e593f000 	ldr	pc, [r3]
    517c:	e5950004 	ldr	r0, [r5, #4]
    5180:	e3a01001 	mov	r1, #1	; 0x1
    5184:	eb000519 	bl	65f0 <vs_assign_by_name>
    5188:	e250a000 	subs	sl, r0, #0	; 0x0
    518c:	13e00000 	mvnne	r0, #0	; 0x0
    5190:	1a00006b 	bne	5344 <do_tt_sn+0x1f0>
    5194:	e5950004 	ldr	r0, [r5, #4]
    5198:	e59f11b4 	ldr	r1, [pc, #436]	; 5354 <do_tt_sn+0x200>
    519c:	e3a02003 	mov	r2, #3	; 0x3
    51a0:	ebfff0d9 	bl	150c <strncmp>
    51a4:	e3500000 	cmp	r0, #0	; 0x0
    51a8:	13a07501 	movne	r7, #4194304	; 0x400000
    51ac:	1a000001 	bne	51b8 <do_tt_sn+0x64>
    51b0:	eb00161f 	bl	aa34 <fnd_align>
    51b4:	e1a07500 	lsl	r7, r0, #10
    51b8:	e5950004 	ldr	r0, [r5, #4]
    51bc:	eb00041a 	bl	622c <vs_device_id>
    51c0:	e3540002 	cmp	r4, #2	; 0x2
    51c4:	e1a08000 	mov	r8, r0
    51c8:	da000007 	ble	51ec <do_tt_sn+0x98>
    51cc:	e3a01000 	mov	r1, #0	; 0x0
    51d0:	e3a0200a 	mov	r2, #10	; 0xa
    51d4:	e5950008 	ldr	r0, [r5, #8]
    51d8:	eb00284b 	bl	f30c <simple_strtoul>
    51dc:	e1a04000 	mov	r4, r0
    51e0:	e1a00008 	mov	r0, r8
    51e4:	eb00038f 	bl	6028 <vs_align>
    51e8:	e0070094 	mul	r7, r4, r0
    51ec:	e59f615c 	ldr	r6, [pc, #348]	; 5350 <do_tt_sn+0x1fc>
    51f0:	e1a00008 	mov	r0, r8
    51f4:	e5954004 	ldr	r4, [r5, #4]
    51f8:	e5965000 	ldr	r5, [r6]
    51fc:	eb000389 	bl	6028 <vs_align>
    5200:	e1a01004 	mov	r1, r4
    5204:	e1a02000 	mov	r2, r0
    5208:	e59f0148 	ldr	r0, [pc, #328]	; 5358 <do_tt_sn+0x204>
    520c:	e12fff35 	blx	r5
    5210:	e1a0400d 	mov	r4, sp
    5214:	e1a0100a 	mov	r1, sl
    5218:	e59f013c 	ldr	r0, [pc, #316]	; 535c <do_tt_sn+0x208>
    521c:	e1a0e00f 	mov	lr, pc
    5220:	e596f000 	ldr	pc, [r6]
    5224:	e1a00008 	mov	r0, r8
    5228:	eb0003eb 	bl	61dc <vs_device_erasable>
    522c:	e28a3001 	add	r3, sl, #1	; 0x1
    5230:	e3500000 	cmp	r0, #0	; 0x0
    5234:	e1a0a003 	mov	sl, r3
    5238:	e59f0120 	ldr	r0, [pc, #288]	; 5360 <do_tt_sn+0x20c>
    523c:	0a00000e 	beq	527c <do_tt_sn+0x128>
    5240:	e1a0e00f 	mov	lr, pc
    5244:	e596f000 	ldr	pc, [r6]
    5248:	e1a02007 	mov	r2, r7
    524c:	e59f1110 	ldr	r1, [pc, #272]	; 5364 <do_tt_sn+0x210>
    5250:	e1a0000d 	mov	r0, sp
    5254:	eb002bb8 	bl	1013c <sprintf>
    5258:	e3a01000 	mov	r1, #0	; 0x0
    525c:	e1a0000d 	mov	r0, sp
    5260:	eb002c7d 	bl	1045c <run_command>
    5264:	eb002c3f 	bl	10368 <tstc>
    5268:	e3500000 	cmp	r0, #0	; 0x0
    526c:	0a000002 	beq	527c <do_tt_sn+0x128>
    5270:	eb002c3d 	bl	1036c <getc>
    5274:	e3500003 	cmp	r0, #3	; 0x3
    5278:	0a000030 	beq	5340 <do_tt_sn+0x1ec>
    527c:	e59f00e4 	ldr	r0, [pc, #228]	; 5368 <do_tt_sn+0x214>
    5280:	e1a0e00f 	mov	lr, pc
    5284:	e596f000 	ldr	pc, [r6]
    5288:	e1a02007 	mov	r2, r7
    528c:	e59f10d8 	ldr	r1, [pc, #216]	; 536c <do_tt_sn+0x218>
    5290:	e1a0000d 	mov	r0, sp
    5294:	eb002ba8 	bl	1013c <sprintf>
    5298:	e3a01000 	mov	r1, #0	; 0x0
    529c:	e1a0000d 	mov	r0, sp
    52a0:	eb002c6d 	bl	1045c <run_command>
    52a4:	eb002c2f 	bl	10368 <tstc>
    52a8:	e3500000 	cmp	r0, #0	; 0x0
    52ac:	0a000002 	beq	52bc <do_tt_sn+0x168>
    52b0:	eb002c2d 	bl	1036c <getc>
    52b4:	e3500003 	cmp	r0, #3	; 0x3
    52b8:	0a000020 	beq	5340 <do_tt_sn+0x1ec>
    52bc:	e59f00ac 	ldr	r0, [pc, #172]	; 5370 <do_tt_sn+0x21c>
    52c0:	e1a0e00f 	mov	lr, pc
    52c4:	e596f000 	ldr	pc, [r6]
    52c8:	e1a02007 	mov	r2, r7
    52cc:	e59f10a0 	ldr	r1, [pc, #160]	; 5374 <do_tt_sn+0x220>
    52d0:	e1a0000d 	mov	r0, sp
    52d4:	eb002b98 	bl	1013c <sprintf>
    52d8:	e3a01000 	mov	r1, #0	; 0x0
    52dc:	e1a0000d 	mov	r0, sp
    52e0:	eb002c5d 	bl	1045c <run_command>
    52e4:	eb002c1f 	bl	10368 <tstc>
    52e8:	e3500000 	cmp	r0, #0	; 0x0
    52ec:	0a000002 	beq	52fc <do_tt_sn+0x1a8>
    52f0:	eb002c1d 	bl	1036c <getc>
    52f4:	e3500003 	cmp	r0, #3	; 0x3
    52f8:	0a000010 	beq	5340 <do_tt_sn+0x1ec>
    52fc:	e59f0074 	ldr	r0, [pc, #116]	; 5378 <do_tt_sn+0x224>
    5300:	e1a0e00f 	mov	lr, pc
    5304:	e596f000 	ldr	pc, [r6]
    5308:	e59f106c 	ldr	r1, [pc, #108]	; 537c <do_tt_sn+0x228>
    530c:	e1a02007 	mov	r2, r7
    5310:	e3a0300c 	mov	r3, #12	; 0xc
    5314:	e59f0064 	ldr	r0, [pc, #100]	; 5380 <do_tt_sn+0x22c>
    5318:	ebfffcf5 	bl	46f4 <tt_cmp_buffer>
    531c:	e59f0060 	ldr	r0, [pc, #96]	; 5384 <do_tt_sn+0x230>
    5320:	e1a0e00f 	mov	lr, pc
    5324:	e596f000 	ldr	pc, [r6]
    5328:	eb002c0e 	bl	10368 <tstc>
    532c:	e3500000 	cmp	r0, #0	; 0x0
    5330:	0affffb7 	beq	5214 <do_tt_sn+0xc0>
    5334:	eb002c0c 	bl	1036c <getc>
    5338:	e3500003 	cmp	r0, #3	; 0x3
    533c:	1affffb4 	bne	5214 <do_tt_sn+0xc0>
    5340:	e3a00000 	mov	r0, #0	; 0x0
    5344:	e28dd044 	add	sp, sp, #68	; 0x44
    5348:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    534c:	0001b37b 	.word	0x0001b37b
    5350:	3c00d694 	.word	0x3c00d694
    5354:	00019f16 	.word	0x00019f16
    5358:	0001b388 	.word	0x0001b388
    535c:	0001abf5 	.word	0x0001abf5
    5360:	0001b0a9 	.word	0x0001b0a9
    5364:	0001b32e 	.word	0x0001b32e
    5368:	0001b0cb 	.word	0x0001b0cb
    536c:	0001b342 	.word	0x0001b342
    5370:	0001b0f6 	.word	0x0001b0f6
    5374:	0001b35f 	.word	0x0001b35f
    5378:	0001b120 	.word	0x0001b120
    537c:	50008000 	.word	0x50008000
    5380:	40008000 	.word	0x40008000
    5384:	0001a882 	.word	0x0001a882

00005388 <tt_nand_trolling>:
    5388:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    538c:	e24dd034 	sub	sp, sp, #52	; 0x34
    5390:	e58d000c 	str	r0, [sp, #12]
    5394:	e1a06001 	mov	r6, r1
    5398:	e3a00001 	mov	r0, #1	; 0x1
    539c:	e3a01064 	mov	r1, #100	; 0x64
    53a0:	e1a09002 	mov	r9, r2
    53a4:	ebfffd26 	bl	4844 <tt_rnd_get_int>
    53a8:	e3500032 	cmp	r0, #50	; 0x32
    53ac:	c5991024 	ldrgt	r1, [r9, #36]
    53b0:	c58d1010 	strgt	r1, [sp, #16]
    53b4:	ca000003 	bgt	53c8 <tt_nand_trolling+0x40>
    53b8:	e3a00001 	mov	r0, #1	; 0x1
    53bc:	e5991024 	ldr	r1, [r9, #36]
    53c0:	ebfffd1f 	bl	4844 <tt_rnd_get_int>
    53c4:	e58d0010 	str	r0, [sp, #16]
    53c8:	e59d200c 	ldr	r2, [sp, #12]
    53cc:	e5991018 	ldr	r1, [r9, #24]
    53d0:	e3520002 	cmp	r2, #2	; 0x2
    53d4:	05993014 	ldreq	r3, [r9, #20]
    53d8:	059d2010 	ldreq	r2, [sp, #16]
    53dc:	159d3010 	ldrne	r3, [sp, #16]
    53e0:	00833001 	addeq	r3, r3, r1
    53e4:	00010293 	muleq	r1, r3, r2
    53e8:	10010391 	mulne	r1, r1, r3
    53ec:	e28d002c 	add	r0, sp, #44	; 0x2c
    53f0:	e1a02001 	mov	r2, r1
    53f4:	ebfffd3b 	bl	48e8 <tt_rnd_get_data>
    53f8:	e3500000 	cmp	r0, #0	; 0x0
    53fc:	e58d0014 	str	r0, [sp, #20]
    5400:	1a000004 	bne	5418 <tt_nand_trolling+0x90>
    5404:	e59f0330 	ldr	r0, [pc, #816]	; 573c <tt_nand_trolling+0x3b4>
    5408:	e59f3330 	ldr	r3, [pc, #816]	; 5740 <tt_nand_trolling+0x3b8>
    540c:	e1a0e00f 	mov	lr, pc
    5410:	e593f000 	ldr	pc, [r3]
    5414:	ea000028 	b	54bc <tt_nand_trolling+0x134>
    5418:	e5991028 	ldr	r1, [r9, #40]
    541c:	e3a00000 	mov	r0, #0	; 0x0
    5420:	ebfffd07 	bl	4844 <tt_rnd_get_int>
    5424:	e5995024 	ldr	r5, [r9, #36]
    5428:	e59d2010 	ldr	r2, [sp, #16]
    542c:	e1a04000 	mov	r4, r0
    5430:	e0621005 	rsb	r1, r2, r5
    5434:	e3a00000 	mov	r0, #0	; 0x0
    5438:	ebfffd01 	bl	4844 <tt_rnd_get_int>
    543c:	e59d300c 	ldr	r3, [sp, #12]
    5440:	e0240495 	mla	r4, r5, r4, r0
    5444:	e3530000 	cmp	r3, #0	; 0x0
    5448:	05963018 	ldreq	r3, [r6, #24]
    544c:	15993018 	ldrne	r3, [r9, #24]
    5450:	e1a05fc4 	asr	r5, r4, #31
    5454:	e0876394 	umull	r6, r7, r4, r3
    5458:	e3a01000 	mov	r1, #0	; 0x0
    545c:	e3a02010 	mov	r2, #16	; 0x10
    5460:	e28d001c 	add	r0, sp, #28	; 0x1c
    5464:	e0277593 	mla	r7, r3, r5, r7
    5468:	ebfff058 	bl	15d0 <memset>
    546c:	e59f32d0 	ldr	r3, [pc, #720]	; 5744 <tt_nand_trolling+0x3bc>
    5470:	e59d200c 	ldr	r2, [sp, #12]
    5474:	e58d301c 	str	r3, [sp, #28]
    5478:	e59f32c8 	ldr	r3, [pc, #712]	; 5748 <tt_nand_trolling+0x3c0>
    547c:	e28d1030 	add	r1, sp, #48	; 0x30
    5480:	e58d3020 	str	r3, [sp, #32]
    5484:	e59f32c0 	ldr	r3, [pc, #704]	; 574c <tt_nand_trolling+0x3c4>
    5488:	e59f82b0 	ldr	r8, [pc, #688]	; 5740 <tt_nand_trolling+0x3b8>
    548c:	e58d3024 	str	r3, [sp, #36]
    5490:	e0813102 	add	r3, r1, r2, lsl #2
    5494:	e513b014 	ldr	fp, [r3, #-20]
    5498:	e3a01001 	mov	r1, #1	; 0x1
    549c:	e1a0000b 	mov	r0, fp
    54a0:	eb000452 	bl	65f0 <vs_assign_by_name>
    54a4:	e3500000 	cmp	r0, #0	; 0x0
    54a8:	0a000005 	beq	54c4 <tt_nand_trolling+0x13c>
    54ac:	e1a0100b 	mov	r1, fp
    54b0:	e59f0298 	ldr	r0, [pc, #664]	; 5750 <tt_nand_trolling+0x3c8>
    54b4:	e1a0e00f 	mov	lr, pc
    54b8:	e598f000 	ldr	pc, [r8]
    54bc:	e3e00000 	mvn	r0, #0	; 0x0
    54c0:	ea00009b 	b	5734 <tt_nand_trolling+0x3ac>
    54c4:	e59d2014 	ldr	r2, [sp, #20]
    54c8:	e59d302c 	ldr	r3, [sp, #44]
    54cc:	e59f0280 	ldr	r0, [pc, #640]	; 5754 <tt_nand_trolling+0x3cc>
    54d0:	e1a0100b 	mov	r1, fp
    54d4:	e88d00c0 	stm	sp, {r6, r7}
    54d8:	e1a0e00f 	mov	lr, pc
    54dc:	e598f000 	ldr	pc, [r8]
    54e0:	e1a00006 	mov	r0, r6
    54e4:	e1a01007 	mov	r1, r7
    54e8:	eb0002a1 	bl	5f74 <vs_isbad>
    54ec:	e250a000 	subs	sl, r0, #0	; 0x0
    54f0:	0a000004 	beq	5508 <tt_nand_trolling+0x180>
    54f4:	e59f025c 	ldr	r0, [pc, #604]	; 5758 <tt_nand_trolling+0x3d0>
    54f8:	e1a0e00f 	mov	lr, pc
    54fc:	e598f000 	ldr	pc, [r8]
    5500:	e3e0000a 	mvn	r0, #10	; 0xa
    5504:	ea00008a 	b	5734 <tt_nand_trolling+0x3ac>
    5508:	e1a01007 	mov	r1, r7
    550c:	e59d202c 	ldr	r2, [sp, #44]
    5510:	e1a00006 	mov	r0, r6
    5514:	eb000287 	bl	5f38 <vs_erase>
    5518:	e1a0000a 	mov	r0, sl
    551c:	eb001820 	bl	b5a4 <get_timer>
    5520:	e59d102c 	ldr	r1, [sp, #44]
    5524:	e1a02006 	mov	r2, r6
    5528:	e1a03007 	mov	r3, r7
    552c:	e1a04000 	mov	r4, r0
    5530:	e59d0014 	ldr	r0, [sp, #20]
    5534:	e88d0402 	stm	sp, {r1, sl}
    5538:	eb000268 	bl	5ee0 <vs_write>
    553c:	e1a00004 	mov	r0, r4
    5540:	eb001817 	bl	b5a4 <get_timer>
    5544:	e1a01000 	mov	r1, r0
    5548:	e59d002c 	ldr	r0, [sp, #44]
    554c:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    5550:	e1a00540 	asr	r0, r0, #10
    5554:	e0000095 	mul	r0, r5, r0
    5558:	eb00336f 	bl	1231c <__aeabi_uidiv>
    555c:	e58d0008 	str	r0, [sp, #8]
    5560:	e1a0000a 	mov	r0, sl
    5564:	eb00180e 	bl	b5a4 <get_timer>
    5568:	e59d102c 	ldr	r1, [sp, #44]
    556c:	e1a02006 	mov	r2, r6
    5570:	e1a03007 	mov	r3, r7
    5574:	e1a04000 	mov	r4, r0
    5578:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    557c:	e88d0402 	stm	sp, {r1, sl}
    5580:	eb000356 	bl	62e0 <vs_read>
    5584:	e1a00004 	mov	r0, r4
    5588:	eb001805 	bl	b5a4 <get_timer>
    558c:	e59d402c 	ldr	r4, [sp, #44]
    5590:	e1a01000 	mov	r1, r0
    5594:	e1a00544 	asr	r0, r4, #10
    5598:	e0000095 	mul	r0, r5, r0
    559c:	eb00335e 	bl	1231c <__aeabi_uidiv>
    55a0:	e1a02004 	mov	r2, r4
    55a4:	e1a05000 	mov	r5, r0
    55a8:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    55ac:	e59d0014 	ldr	r0, [sp, #20]
    55b0:	e3a0300c 	mov	r3, #12	; 0xc
    55b4:	ebfffc4e 	bl	46f4 <tt_cmp_buffer>
    55b8:	e3500000 	cmp	r0, #0	; 0x0
    55bc:	0a000007 	beq	55e0 <tt_nand_trolling+0x258>
    55c0:	e58d0000 	str	r0, [sp]
    55c4:	e1a0100b 	mov	r1, fp
    55c8:	e59d2008 	ldr	r2, [sp, #8]
    55cc:	e1a03005 	mov	r3, r5
    55d0:	e59f0184 	ldr	r0, [pc, #388]	; 575c <tt_nand_trolling+0x3d4>
    55d4:	e1a0e00f 	mov	lr, pc
    55d8:	e598f000 	ldr	pc, [r8]
    55dc:	ea000005 	b	55f8 <tt_nand_trolling+0x270>
    55e0:	e1a0100b 	mov	r1, fp
    55e4:	e59d2008 	ldr	r2, [sp, #8]
    55e8:	e1a03005 	mov	r3, r5
    55ec:	e59f016c 	ldr	r0, [pc, #364]	; 5760 <tt_nand_trolling+0x3d8>
    55f0:	e1a0e00f 	mov	lr, pc
    55f4:	e598f000 	ldr	pc, [r8]
    55f8:	e59d102c 	ldr	r1, [sp, #44]
    55fc:	e59f0160 	ldr	r0, [pc, #352]	; 5764 <tt_nand_trolling+0x3dc>
    5600:	e890000c 	ldm	r0, {r2, r3}
    5604:	e0922001 	adds	r2, r2, r1
    5608:	e0a33fc1 	adc	r3, r3, r1, asr #31
    560c:	e59d100c 	ldr	r1, [sp, #12]
    5610:	e880000c 	stm	r0, {r2, r3}
    5614:	e3510000 	cmp	r1, #0	; 0x0
    5618:	0a000040 	beq	5720 <tt_nand_trolling+0x398>
    561c:	e2618003 	rsb	r8, r1, #3	; 0x3
    5620:	e28d2030 	add	r2, sp, #48	; 0x30
    5624:	e0823108 	add	r3, r2, r8, lsl #2
    5628:	e513a014 	ldr	sl, [r3, #-20]
    562c:	e3a01001 	mov	r1, #1	; 0x1
    5630:	e1a0000a 	mov	r0, sl
    5634:	eb0003ed 	bl	65f0 <vs_assign_by_name>
    5638:	e3500000 	cmp	r0, #0	; 0x0
    563c:	0a000005 	beq	5658 <tt_nand_trolling+0x2d0>
    5640:	e1a0100a 	mov	r1, sl
    5644:	e59f0104 	ldr	r0, [pc, #260]	; 5750 <tt_nand_trolling+0x3c8>
    5648:	e59f30f0 	ldr	r3, [pc, #240]	; 5740 <tt_nand_trolling+0x3b8>
    564c:	e1a0e00f 	mov	lr, pc
    5650:	e593f000 	ldr	pc, [r3]
    5654:	eaffff98 	b	54bc <tt_nand_trolling+0x134>
    5658:	e3580001 	cmp	r8, #1	; 0x1
    565c:	15990014 	ldrne	r0, [r9, #20]
    5660:	e5993018 	ldr	r3, [r9, #24]
    5664:	e59d1010 	ldr	r1, [sp, #16]
    5668:	e0803003 	add	r3, r0, r3
    566c:	e0030391 	mul	r3, r1, r3
    5670:	e3a00000 	mov	r0, #0	; 0x0
    5674:	e58d302c 	str	r3, [sp, #44]
    5678:	eb0017c9 	bl	b5a4 <get_timer>
    567c:	e59d102c 	ldr	r1, [sp, #44]
    5680:	e1a02006 	mov	r2, r6
    5684:	e1a04000 	mov	r4, r0
    5688:	e1a03007 	mov	r3, r7
    568c:	e58d1000 	str	r1, [sp]
    5690:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5694:	e3a01000 	mov	r1, #0	; 0x0
    5698:	e58d1004 	str	r1, [sp, #4]
    569c:	eb00030f 	bl	62e0 <vs_read>
    56a0:	e1a00004 	mov	r0, r4
    56a4:	eb0017be 	bl	b5a4 <get_timer>
    56a8:	e59d402c 	ldr	r4, [sp, #44]
    56ac:	e1a01000 	mov	r1, r0
    56b0:	e1a03544 	asr	r3, r4, #10
    56b4:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    56b8:	e0000390 	mul	r0, r0, r3
    56bc:	eb003316 	bl	1231c <__aeabi_uidiv>
    56c0:	e3580002 	cmp	r8, #2	; 0x2
    56c4:	13a03000 	movne	r3, #0	; 0x0
    56c8:	03a03001 	moveq	r3, #1	; 0x1
    56cc:	e1a05000 	mov	r5, r0
    56d0:	e1a02004 	mov	r2, r4
    56d4:	e59d0014 	ldr	r0, [sp, #20]
    56d8:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    56dc:	e58d9000 	str	r9, [sp]
    56e0:	ebfffdfa 	bl	4ed0 <tt_nand_cross_cmp>
    56e4:	e3500000 	cmp	r0, #0	; 0x0
    56e8:	e59f8050 	ldr	r8, [pc, #80]	; 5740 <tt_nand_trolling+0x3b8>
    56ec:	0a000006 	beq	570c <tt_nand_trolling+0x384>
    56f0:	e1a03000 	mov	r3, r0
    56f4:	e1a0100a 	mov	r1, sl
    56f8:	e1a02005 	mov	r2, r5
    56fc:	e59f0064 	ldr	r0, [pc, #100]	; 5768 <tt_nand_trolling+0x3e0>
    5700:	e1a0e00f 	mov	lr, pc
    5704:	e598f000 	ldr	pc, [r8]
    5708:	ea000004 	b	5720 <tt_nand_trolling+0x398>
    570c:	e1a0100a 	mov	r1, sl
    5710:	e1a02005 	mov	r2, r5
    5714:	e59f0050 	ldr	r0, [pc, #80]	; 576c <tt_nand_trolling+0x3e4>
    5718:	e1a0e00f 	mov	lr, pc
    571c:	e598f000 	ldr	pc, [r8]
    5720:	e59f0048 	ldr	r0, [pc, #72]	; 5770 <tt_nand_trolling+0x3e8>
    5724:	e59f3014 	ldr	r3, [pc, #20]	; 5740 <tt_nand_trolling+0x3b8>
    5728:	e1a0e00f 	mov	lr, pc
    572c:	e593f000 	ldr	pc, [r3]
    5730:	e3a00000 	mov	r0, #0	; 0x0
    5734:	e28dd034 	add	sp, sp, #52	; 0x34
    5738:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    573c:	0001b3a6 	.word	0x0001b3a6
    5740:	3c00d694 	.word	0x3c00d694
    5744:	00019da3 	.word	0x00019da3
    5748:	00019f12 	.word	0x00019f12
    574c:	00019f16 	.word	0x00019f16
    5750:	0001b3c5 	.word	0x0001b3c5
    5754:	0001b3e2 	.word	0x0001b3e2
    5758:	0001b412 	.word	0x0001b412
    575c:	0001b43a 	.word	0x0001b43a
    5760:	0001b473 	.word	0x0001b473
    5764:	3c00d160 	.word	0x3c00d160
    5768:	0001b49f 	.word	0x0001b49f
    576c:	0001b4cc 	.word	0x0001b4cc
    5770:	0001a882 	.word	0x0001a882

00005774 <do_tt_ndloop>:
    5774:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5778:	e3a00001 	mov	r0, #1	; 0x1
    577c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5780:	eb00146e 	bl	a940 <nand_get_config>
    5784:	e2505000 	subs	r5, r0, #0	; 0x0
    5788:	1a000005 	bne	57a4 <do_tt_ndloop+0x30>
    578c:	e59f00d0 	ldr	r0, [pc, #208]	; 5864 <do_tt_ndloop+0xf0>
    5790:	e59f30d0 	ldr	r3, [pc, #208]	; 5868 <do_tt_ndloop+0xf4>
    5794:	e1a0e00f 	mov	lr, pc
    5798:	e593f000 	ldr	pc, [r3]
    579c:	e3e00001 	mvn	r0, #1	; 0x1
    57a0:	ea00002d 	b	585c <do_tt_ndloop+0xe8>
    57a4:	e3a00000 	mov	r0, #0	; 0x0
    57a8:	eb00177d 	bl	b5a4 <get_timer>
    57ac:	e59f10b8 	ldr	r1, [pc, #184]	; 586c <do_tt_ndloop+0xf8>
    57b0:	e3a02000 	mov	r2, #0	; 0x0
    57b4:	e3a03000 	mov	r3, #0	; 0x0
    57b8:	e881000c 	stm	r1, {r2, r3}
    57bc:	e59f70a4 	ldr	r7, [pc, #164]	; 5868 <do_tt_ndloop+0xf4>
    57c0:	e1a06000 	mov	r6, r0
    57c4:	e3a04000 	mov	r4, #0	; 0x0
    57c8:	e1a01004 	mov	r1, r4
    57cc:	e59f009c 	ldr	r0, [pc, #156]	; 5870 <do_tt_ndloop+0xfc>
    57d0:	e1a0e00f 	mov	lr, pc
    57d4:	e597f000 	ldr	pc, [r7]
    57d8:	e3a01000 	mov	r1, #0	; 0x0
    57dc:	e1a02005 	mov	r2, r5
    57e0:	e3a00001 	mov	r0, #1	; 0x1
    57e4:	ebfffee7 	bl	5388 <tt_nand_trolling>
    57e8:	e3500000 	cmp	r0, #0	; 0x0
    57ec:	e2844001 	add	r4, r4, #1	; 0x1
    57f0:	1a000003 	bne	5804 <do_tt_ndloop+0x90>
    57f4:	e1a01000 	mov	r1, r0
    57f8:	e1a02005 	mov	r2, r5
    57fc:	e3a00002 	mov	r0, #2	; 0x2
    5800:	ebfffee0 	bl	5388 <tt_nand_trolling>
    5804:	eb002ad7 	bl	10368 <tstc>
    5808:	e3500000 	cmp	r0, #0	; 0x0
    580c:	0affffed 	beq	57c8 <do_tt_ndloop+0x54>
    5810:	eb002ad5 	bl	1036c <getc>
    5814:	e3500003 	cmp	r0, #3	; 0x3
    5818:	1affffea 	bne	57c8 <do_tt_ndloop+0x54>
    581c:	e1a00006 	mov	r0, r6
    5820:	eb00175f 	bl	b5a4 <get_timer>
    5824:	e59f3040 	ldr	r3, [pc, #64]	; 586c <do_tt_ndloop+0xf8>
    5828:	e30e1a60 	movw	r1, #60000	; 0xea60
    582c:	e5934000 	ldr	r4, [r3]
    5830:	e5933004 	ldr	r3, [r3, #4]
    5834:	e1a04a24 	lsr	r4, r4, #20
    5838:	e1844603 	orr	r4, r4, r3, lsl #12
    583c:	eb003338 	bl	12524 <__aeabi_idiv>
    5840:	e1a01004 	mov	r1, r4
    5844:	e1a02000 	mov	r2, r0
    5848:	e59f3018 	ldr	r3, [pc, #24]	; 5868 <do_tt_ndloop+0xf4>
    584c:	e59f0020 	ldr	r0, [pc, #32]	; 5874 <do_tt_ndloop+0x100>
    5850:	e1a0e00f 	mov	lr, pc
    5854:	e593f000 	ldr	pc, [r3]
    5858:	e3a00000 	mov	r0, #0	; 0x0
    585c:	e28dd004 	add	sp, sp, #4	; 0x4
    5860:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5864:	0001b4ec 	.word	0x0001b4ec
    5868:	3c00d694 	.word	0x3c00d694
    586c:	3c00d160 	.word	0x3c00d160
    5870:	0001abf5 	.word	0x0001abf5
    5874:	0001abff 	.word	0x0001abff

00005878 <do_tt_nand>:
    5878:	e3520001 	cmp	r2, #1	; 0x1
    587c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    5880:	e1a08002 	mov	r8, r2
    5884:	e1a0a003 	mov	sl, r3
    5888:	c59f90dc 	ldrgt	r9, [pc, #220]	; 596c <do_tt_nand+0xf4>
    588c:	c3a06001 	movgt	r6, #1	; 0x1
    5890:	ca000005 	bgt	58ac <do_tt_nand+0x34>
    5894:	e59f00d4 	ldr	r0, [pc, #212]	; 5970 <do_tt_nand+0xf8>
    5898:	e59f30cc 	ldr	r3, [pc, #204]	; 596c <do_tt_nand+0xf4>
    589c:	e1a0e00f 	mov	lr, pc
    58a0:	e593f000 	ldr	pc, [r3]
    58a4:	e3e00000 	mvn	r0, #0	; 0x0
    58a8:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    58ac:	e3a01000 	mov	r1, #0	; 0x0
    58b0:	e3a0200a 	mov	r2, #10	; 0xa
    58b4:	e79a0106 	ldr	r0, [sl, r6, lsl #2]
    58b8:	eb002693 	bl	f30c <simple_strtoul>
    58bc:	e1a07000 	mov	r7, r0
    58c0:	ebffeb39 	bl	5ac <set_xom>
    58c4:	e3a00000 	mov	r0, #0	; 0x0
    58c8:	eb0014cf 	bl	ac0c <nand_rescan_config>
    58cc:	e3a00000 	mov	r0, #0	; 0x0
    58d0:	eb00141a 	bl	a940 <nand_get_config>
    58d4:	e1a05000 	mov	r5, r0
    58d8:	e3a00001 	mov	r0, #1	; 0x1
    58dc:	eb001417 	bl	a940 <nand_get_config>
    58e0:	e3500000 	cmp	r0, #0	; 0x0
    58e4:	13550000 	cmpne	r5, #0	; 0x0
    58e8:	e1a04000 	mov	r4, r0
    58ec:	1a000005 	bne	5908 <do_tt_nand+0x90>
    58f0:	e59f007c 	ldr	r0, [pc, #124]	; 5974 <do_tt_nand+0xfc>
    58f4:	e59f3070 	ldr	r3, [pc, #112]	; 596c <do_tt_nand+0xf4>
    58f8:	e1a0e00f 	mov	lr, pc
    58fc:	e593f000 	ldr	pc, [r3]
    5900:	e3e00001 	mvn	r0, #1	; 0x1
    5904:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    5908:	e1a01007 	mov	r1, r7
    590c:	e59f0064 	ldr	r0, [pc, #100]	; 5978 <do_tt_nand+0x100>
    5910:	e1a0e00f 	mov	lr, pc
    5914:	e599f000 	ldr	pc, [r9]
    5918:	e3a00000 	mov	r0, #0	; 0x0
    591c:	e1a01005 	mov	r1, r5
    5920:	e1a02004 	mov	r2, r4
    5924:	ebfffe97 	bl	5388 <tt_nand_trolling>
    5928:	e370000b 	cmn	r0, #11	; 0xb
    592c:	0afffff9 	beq	5918 <do_tt_nand+0xa0>
    5930:	e3a00001 	mov	r0, #1	; 0x1
    5934:	e1a01005 	mov	r1, r5
    5938:	e1a02004 	mov	r2, r4
    593c:	ebfffe91 	bl	5388 <tt_nand_trolling>
    5940:	e370000b 	cmn	r0, #11	; 0xb
    5944:	0afffff9 	beq	5930 <do_tt_nand+0xb8>
    5948:	e1a01005 	mov	r1, r5
    594c:	e1a02004 	mov	r2, r4
    5950:	e2866001 	add	r6, r6, #1	; 0x1
    5954:	e3a00002 	mov	r0, #2	; 0x2
    5958:	ebfffe8a 	bl	5388 <tt_nand_trolling>
    595c:	e1560008 	cmp	r6, r8
    5960:	baffffd1 	blt	58ac <do_tt_nand+0x34>
    5964:	e3a00000 	mov	r0, #0	; 0x0
    5968:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    596c:	3c00d694 	.word	0x3c00d694
    5970:	0001b51e 	.word	0x0001b51e
    5974:	0001b4ec 	.word	0x0001b4ec
    5978:	0001b554 	.word	0x0001b554

0000597c <tt_wire_get_opt>:
    597c:	e5900008 	ldr	r0, [r0, #8]
    5980:	e12fff1e 	bx	lr

00005984 <tt_wire_get_len>:
    5984:	e590000c 	ldr	r0, [r0, #12]
    5988:	e12fff1e 	bx	lr

0000598c <tt_wire_make_data>:
    598c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5990:	e24dd004 	sub	sp, sp, #4	; 0x4
    5994:	e1a04000 	mov	r4, r0
    5998:	e1a07002 	mov	r7, r2
    599c:	e1a06003 	mov	r6, r3
    59a0:	ebffef12 	bl	15f0 <memcpy>
    59a4:	e59f3040 	ldr	r3, [pc, #64]	; 59ec <tt_wire_make_data+0x60>
    59a8:	e5846008 	str	r6, [r4, #8]
    59ac:	e5843000 	str	r3, [r4]
    59b0:	e59d3018 	ldr	r3, [sp, #24]
    59b4:	e3a05000 	mov	r5, #0	; 0x0
    59b8:	e584300c 	str	r3, [r4, #12]
    59bc:	e1a01007 	mov	r1, r7
    59c0:	e5845004 	str	r5, [r4, #4]
    59c4:	e1a00005 	mov	r0, r5
    59c8:	eb000469 	bl	6b74 <hash_init>
    59cc:	e1a01007 	mov	r1, r7
    59d0:	e1a00004 	mov	r0, r4
    59d4:	eb000441 	bl	6ae0 <hash_data>
    59d8:	e2840004 	add	r0, r4, #4	; 0x4
    59dc:	eb000423 	bl	6a70 <hash_value>
    59e0:	e1a00005 	mov	r0, r5
    59e4:	e28dd004 	add	sp, sp, #4	; 0x4
    59e8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    59ec:	7766dd80 	.word	0x7766dd80

000059f0 <tt_wire_check_valid>:
    59f0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    59f4:	e5903000 	ldr	r3, [r0]
    59f8:	e59f2084 	ldr	r2, [pc, #132]	; 5a84 <tt_wire_check_valid+0x94>
    59fc:	e24dd00c 	sub	sp, sp, #12	; 0xc
    5a00:	e1530002 	cmp	r3, r2
    5a04:	e1a04000 	mov	r4, r0
    5a08:	e1a06001 	mov	r6, r1
    5a0c:	e5907004 	ldr	r7, [r0, #4]
    5a10:	0a000006 	beq	5a30 <tt_wire_check_valid+0x40>
    5a14:	e1a01003 	mov	r1, r3
    5a18:	e59f0068 	ldr	r0, [pc, #104]	; 5a88 <tt_wire_check_valid+0x98>
    5a1c:	e59f3068 	ldr	r3, [pc, #104]	; 5a8c <tt_wire_check_valid+0x9c>
    5a20:	e1a0e00f 	mov	lr, pc
    5a24:	e593f000 	ldr	pc, [r3]
    5a28:	e3a00000 	mov	r0, #0	; 0x0
    5a2c:	ea000012 	b	5a7c <tt_wire_check_valid+0x8c>
    5a30:	e3a05000 	mov	r5, #0	; 0x0
    5a34:	e5805004 	str	r5, [r0, #4]
    5a38:	e1a00005 	mov	r0, r5
    5a3c:	eb00044c 	bl	6b74 <hash_init>
    5a40:	e1a01006 	mov	r1, r6
    5a44:	e1a00004 	mov	r0, r4
    5a48:	eb000424 	bl	6ae0 <hash_data>
    5a4c:	e28d0004 	add	r0, sp, #4	; 0x4
    5a50:	eb000406 	bl	6a70 <hash_value>
    5a54:	e59d1004 	ldr	r1, [sp, #4]
    5a58:	e1510007 	cmp	r1, r7
    5a5c:	03a00001 	moveq	r0, #1	; 0x1
    5a60:	0a000005 	beq	5a7c <tt_wire_check_valid+0x8c>
    5a64:	e59f0024 	ldr	r0, [pc, #36]	; 5a90 <tt_wire_check_valid+0xa0>
    5a68:	e1a02007 	mov	r2, r7
    5a6c:	e59f3018 	ldr	r3, [pc, #24]	; 5a8c <tt_wire_check_valid+0x9c>
    5a70:	e1a0e00f 	mov	lr, pc
    5a74:	e593f000 	ldr	pc, [r3]
    5a78:	e1a00005 	mov	r0, r5
    5a7c:	e28dd00c 	add	sp, sp, #12	; 0xc
    5a80:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5a84:	7766dd80 	.word	0x7766dd80
    5a88:	0001b5ce 	.word	0x0001b5ce
    5a8c:	3c00d694 	.word	0x3c00d694
    5a90:	0001b5f7 	.word	0x0001b5f7

00005a94 <do_tt_wire>:
    5a94:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    5a98:	e3520001 	cmp	r2, #1	; 0x1
    5a9c:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    5aa0:	e1a04003 	mov	r4, r3
    5aa4:	d59f032c 	ldrle	r0, [pc, #812]	; 5dd8 <do_tt_wire+0x344>
    5aa8:	da00002b 	ble	5b5c <do_tt_wire+0xc8>
    5aac:	e3a02003 	mov	r2, #3	; 0x3
    5ab0:	e5930004 	ldr	r0, [r3, #4]
    5ab4:	e59f1320 	ldr	r1, [pc, #800]	; 5ddc <do_tt_wire+0x348>
    5ab8:	ebffee93 	bl	150c <strncmp>
    5abc:	e3500000 	cmp	r0, #0	; 0x0
    5ac0:	059f2318 	ldreq	r2, [pc, #792]	; 5de0 <do_tt_wire+0x34c>
    5ac4:	0a000006 	beq	5ae4 <do_tt_wire+0x50>
    5ac8:	e5940004 	ldr	r0, [r4, #4]
    5acc:	e59f1310 	ldr	r1, [pc, #784]	; 5de4 <do_tt_wire+0x350>
    5ad0:	e3a02003 	mov	r2, #3	; 0x3
    5ad4:	ebffee8c 	bl	150c <strncmp>
    5ad8:	e3500000 	cmp	r0, #0	; 0x0
    5adc:	1a000009 	bne	5b08 <do_tt_wire+0x74>
    5ae0:	e59f2300 	ldr	r2, [pc, #768]	; 5de8 <do_tt_wire+0x354>
    5ae4:	e59f3300 	ldr	r3, [pc, #768]	; 5dec <do_tt_wire+0x358>
    5ae8:	e3a00000 	mov	r0, #0	; 0x0
    5aec:	e5832000 	str	r2, [r3]
    5af0:	eb0016ab 	bl	b5a4 <get_timer>
    5af4:	e59f82f4 	ldr	r8, [pc, #756]	; 5df0 <do_tt_wire+0x35c>
    5af8:	e59f92ec 	ldr	r9, [pc, #748]	; 5dec <do_tt_wire+0x358>
    5afc:	e1a0a008 	mov	sl, r8
    5b00:	e58d000c 	str	r0, [sp, #12]
    5b04:	ea000006 	b	5b24 <do_tt_wire+0x90>
    5b08:	e5941004 	ldr	r1, [r4, #4]
    5b0c:	e59f02e0 	ldr	r0, [pc, #736]	; 5df4 <do_tt_wire+0x360>
    5b10:	e59f32d8 	ldr	r3, [pc, #728]	; 5df0 <do_tt_wire+0x35c>
    5b14:	e1a0e00f 	mov	lr, pc
    5b18:	e593f000 	ldr	pc, [r3]
    5b1c:	e3e00000 	mvn	r0, #0	; 0x0
    5b20:	ea0000aa 	b	5dd0 <do_tt_wire+0x33c>
    5b24:	e59f02cc 	ldr	r0, [pc, #716]	; 5df8 <do_tt_wire+0x364>
    5b28:	eb001679 	bl	b514 <udelay>
    5b2c:	e59f02c8 	ldr	r0, [pc, #712]	; 5dfc <do_tt_wire+0x368>
    5b30:	e1a0e00f 	mov	lr, pc
    5b34:	e598f000 	ldr	pc, [r8]
    5b38:	e5993000 	ldr	r3, [r9]
    5b3c:	e1a0e00f 	mov	lr, pc
    5b40:	e593f000 	ldr	pc, [r3]
    5b44:	e3500000 	cmp	r0, #0	; 0x0
    5b48:	a3a07000 	movge	r7, #0	; 0x0
    5b4c:	a3a06020 	movge	r6, #32	; 0x20
    5b50:	a59fb2a8 	ldrge	fp, [pc, #680]	; 5e00 <do_tt_wire+0x36c>
    5b54:	aa000004 	bge	5b6c <do_tt_wire+0xd8>
    5b58:	e59f02a4 	ldr	r0, [pc, #676]	; 5e04 <do_tt_wire+0x370>
    5b5c:	e59f328c 	ldr	r3, [pc, #652]	; 5df0 <do_tt_wire+0x35c>
    5b60:	e1a0e00f 	mov	lr, pc
    5b64:	e593f000 	ldr	pc, [r3]
    5b68:	eaffffeb 	b	5b1c <do_tt_wire+0x88>
    5b6c:	e3570000 	cmp	r7, #0	; 0x0
    5b70:	e1a01006 	mov	r1, r6
    5b74:	e59f028c 	ldr	r0, [pc, #652]	; 5e08 <do_tt_wire+0x374>
    5b78:	1a00002f 	bne	5c3c <do_tt_wire+0x1a8>
    5b7c:	e59f0288 	ldr	r0, [pc, #648]	; 5e0c <do_tt_wire+0x378>
    5b80:	e1a0e00f 	mov	lr, pc
    5b84:	e598f000 	ldr	pc, [r8]
    5b88:	e1a00007 	mov	r0, r7
    5b8c:	eb001684 	bl	b5a4 <get_timer>
    5b90:	e1a01006 	mov	r1, r6
    5b94:	e1a04000 	mov	r4, r0
    5b98:	e5993000 	ldr	r3, [r9]
    5b9c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5ba0:	e1a0e00f 	mov	lr, pc
    5ba4:	e593f004 	ldr	pc, [r3, #4]
    5ba8:	e1a05000 	mov	r5, r0
    5bac:	e1a00004 	mov	r0, r4
    5bb0:	eb00167b 	bl	b5a4 <get_timer>
    5bb4:	e3550000 	cmp	r5, #0	; 0x0
    5bb8:	e1a04000 	mov	r4, r0
    5bbc:	e1a01006 	mov	r1, r6
    5bc0:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5bc4:	ba00003d 	blt	5cc0 <do_tt_wire+0x22c>
    5bc8:	ebffff88 	bl	59f0 <tt_wire_check_valid>
    5bcc:	e3500000 	cmp	r0, #0	; 0x0
    5bd0:	e1a01004 	mov	r1, r4
    5bd4:	e1a03546 	asr	r3, r6, #10
    5bd8:	1a000007 	bne	5bfc <do_tt_wire+0x168>
    5bdc:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    5be0:	e0000390 	mul	r0, r0, r3
    5be4:	eb00324e 	bl	12524 <__aeabi_idiv>
    5be8:	e1a01000 	mov	r1, r0
    5bec:	e59f021c 	ldr	r0, [pc, #540]	; 5e10 <do_tt_wire+0x37c>
    5bf0:	e1a0e00f 	mov	lr, pc
    5bf4:	e59af000 	ldr	pc, [sl]
    5bf8:	eaffffc9 	b	5b24 <do_tt_wire+0x90>
    5bfc:	e59fc210 	ldr	ip, [pc, #528]	; 5e14 <do_tt_wire+0x380>
    5c00:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    5c04:	e0000392 	mul	r0, r2, r3
    5c08:	e89c000c 	ldm	ip, {r2, r3}
    5c0c:	e0922006 	adds	r2, r2, r6
    5c10:	e0a33fc6 	adc	r3, r3, r6, asr #31
    5c14:	e88c000c 	stm	ip, {r2, r3}
    5c18:	eb003241 	bl	12524 <__aeabi_idiv>
    5c1c:	e1a01000 	mov	r1, r0
    5c20:	e59f01f0 	ldr	r0, [pc, #496]	; 5e18 <do_tt_wire+0x384>
    5c24:	e1a0e00f 	mov	lr, pc
    5c28:	e598f000 	ldr	pc, [r8]
    5c2c:	e3a0348e 	mov	r3, #-1912602624	; 0x8e000000
    5c30:	e5937008 	ldr	r7, [r3, #8]
    5c34:	e593600c 	ldr	r6, [r3, #12]
    5c38:	ea000044 	b	5d50 <do_tt_wire+0x2bc>
    5c3c:	e3570001 	cmp	r7, #1	; 0x1
    5c40:	1a000030 	bne	5d08 <do_tt_wire+0x274>
    5c44:	e1a0e00f 	mov	lr, pc
    5c48:	e598f000 	ldr	pc, [r8]
    5c4c:	e1a01007 	mov	r1, r7
    5c50:	e3a00000 	mov	r0, #0	; 0x0
    5c54:	ebfffafa 	bl	4844 <tt_rnd_get_int>
    5c58:	e3a01020 	mov	r1, #32	; 0x20
    5c5c:	e1a07000 	mov	r7, r0
    5c60:	e3a02502 	mov	r2, #8388608	; 0x800000
    5c64:	e28d0014 	add	r0, sp, #20	; 0x14
    5c68:	ebfffb1e 	bl	48e8 <tt_rnd_get_data>
    5c6c:	e59dc014 	ldr	ip, [sp, #20]
    5c70:	e1a01000 	mov	r1, r0
    5c74:	e1a02006 	mov	r2, r6
    5c78:	e1a03007 	mov	r3, r7
    5c7c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5c80:	e58dc000 	str	ip, [sp]
    5c84:	ebffff40 	bl	598c <tt_wire_make_data>
    5c88:	e3a00000 	mov	r0, #0	; 0x0
    5c8c:	eb001644 	bl	b5a4 <get_timer>
    5c90:	e1a01006 	mov	r1, r6
    5c94:	e1a05000 	mov	r5, r0
    5c98:	e5993000 	ldr	r3, [r9]
    5c9c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5ca0:	e1a0e00f 	mov	lr, pc
    5ca4:	e593f008 	ldr	pc, [r3, #8]
    5ca8:	e1a04000 	mov	r4, r0
    5cac:	e1a00005 	mov	r0, r5
    5cb0:	eb00163b 	bl	b5a4 <get_timer>
    5cb4:	e3540000 	cmp	r4, #0	; 0x0
    5cb8:	e1a01000 	mov	r1, r0
    5cbc:	aa000003 	bge	5cd0 <do_tt_wire+0x23c>
    5cc0:	e59f0154 	ldr	r0, [pc, #340]	; 5e1c <do_tt_wire+0x388>
    5cc4:	e1a0e00f 	mov	lr, pc
    5cc8:	e59af000 	ldr	pc, [sl]
    5ccc:	eaffff94 	b	5b24 <do_tt_wire+0x90>
    5cd0:	e89b000c 	ldm	fp, {r2, r3}
    5cd4:	e0922006 	adds	r2, r2, r6
    5cd8:	e0a33fc6 	adc	r3, r3, r6, asr #31
    5cdc:	e59d6014 	ldr	r6, [sp, #20]
    5ce0:	e88b000c 	stm	fp, {r2, r3}
    5ce4:	e1a00546 	asr	r0, r6, #10
    5ce8:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    5cec:	e0000092 	mul	r0, r2, r0
    5cf0:	eb00320b 	bl	12524 <__aeabi_idiv>
    5cf4:	e1a01000 	mov	r1, r0
    5cf8:	e59f0120 	ldr	r0, [pc, #288]	; 5e20 <do_tt_wire+0x38c>
    5cfc:	e1a0e00f 	mov	lr, pc
    5d00:	e598f000 	ldr	pc, [r8]
    5d04:	ea000011 	b	5d50 <do_tt_wire+0x2bc>
    5d08:	e3570003 	cmp	r7, #3	; 0x3
    5d0c:	1a000004 	bne	5d24 <do_tt_wire+0x290>
    5d10:	e59f010c 	ldr	r0, [pc, #268]	; 5e24 <do_tt_wire+0x390>
    5d14:	e59f30d4 	ldr	r3, [pc, #212]	; 5df0 <do_tt_wire+0x35c>
    5d18:	e1a0e00f 	mov	lr, pc
    5d1c:	e593f000 	ldr	pc, [r3]
    5d20:	ea000010 	b	5d68 <do_tt_wire+0x2d4>
    5d24:	e3570002 	cmp	r7, #2	; 0x2
    5d28:	1a000003 	bne	5d3c <do_tt_wire+0x2a8>
    5d2c:	e59f00f4 	ldr	r0, [pc, #244]	; 5e28 <do_tt_wire+0x394>
    5d30:	e1a0e00f 	mov	lr, pc
    5d34:	e598f000 	ldr	pc, [r8]
    5d38:	eaffff79 	b	5b24 <do_tt_wire+0x90>
    5d3c:	e59f00e8 	ldr	r0, [pc, #232]	; 5e2c <do_tt_wire+0x398>
    5d40:	e59f30a8 	ldr	r3, [pc, #168]	; 5df0 <do_tt_wire+0x35c>
    5d44:	e1a0e00f 	mov	lr, pc
    5d48:	e593f000 	ldr	pc, [r3]
    5d4c:	eafffffe 	b	5d4c <do_tt_wire+0x2b8>
    5d50:	eb002984 	bl	10368 <tstc>
    5d54:	e3500000 	cmp	r0, #0	; 0x0
    5d58:	0affff83 	beq	5b6c <do_tt_wire+0xd8>
    5d5c:	eb002982 	bl	1036c <getc>
    5d60:	e3500003 	cmp	r0, #3	; 0x3
    5d64:	1affff80 	bne	5b6c <do_tt_wire+0xd8>
    5d68:	e59d000c 	ldr	r0, [sp, #12]
    5d6c:	eb00160c 	bl	b5a4 <get_timer>
    5d70:	e59f3088 	ldr	r3, [pc, #136]	; 5e00 <do_tt_wire+0x36c>
    5d74:	e59f4074 	ldr	r4, [pc, #116]	; 5df0 <do_tt_wire+0x35c>
    5d78:	e893000a 	ldm	r3, {r1, r3}
    5d7c:	e1a01a21 	lsr	r1, r1, #20
    5d80:	e1811603 	orr	r1, r1, r3, lsl #12
    5d84:	e1a05000 	mov	r5, r0
    5d88:	e59f00a0 	ldr	r0, [pc, #160]	; 5e30 <do_tt_wire+0x39c>
    5d8c:	e1a0e00f 	mov	lr, pc
    5d90:	e594f000 	ldr	pc, [r4]
    5d94:	e59f3078 	ldr	r3, [pc, #120]	; 5e14 <do_tt_wire+0x380>
    5d98:	e59f0094 	ldr	r0, [pc, #148]	; 5e34 <do_tt_wire+0x3a0>
    5d9c:	e893000a 	ldm	r3, {r1, r3}
    5da0:	e1a01a21 	lsr	r1, r1, #20
    5da4:	e1811603 	orr	r1, r1, r3, lsl #12
    5da8:	e1a0e00f 	mov	lr, pc
    5dac:	e594f000 	ldr	pc, [r4]
    5db0:	e30e1a60 	movw	r1, #60000	; 0xea60
    5db4:	e1a00005 	mov	r0, r5
    5db8:	eb0031d9 	bl	12524 <__aeabi_idiv>
    5dbc:	e1a01000 	mov	r1, r0
    5dc0:	e59f0070 	ldr	r0, [pc, #112]	; 5e38 <do_tt_wire+0x3a4>
    5dc4:	e1a0e00f 	mov	lr, pc
    5dc8:	e594f000 	ldr	pc, [r4]
    5dcc:	e3a00000 	mov	r0, #0	; 0x0
    5dd0:	e28dd01c 	add	sp, sp, #28	; 0x1c
    5dd4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5dd8:	0001b620 	.word	0x0001b620
    5ddc:	0001b669 	.word	0x0001b669
    5de0:	00013178 	.word	0x00013178
    5de4:	0001b66d 	.word	0x0001b66d
    5de8:	00013190 	.word	0x00013190
    5dec:	3c00d178 	.word	0x3c00d178
    5df0:	3c00d694 	.word	0x3c00d694
    5df4:	0001b671 	.word	0x0001b671
    5df8:	001e8480 	.word	0x001e8480
    5dfc:	0001b691 	.word	0x0001b691
    5e00:	3c00d170 	.word	0x3c00d170
    5e04:	0001b6b3 	.word	0x0001b6b3
    5e08:	0001b720 	.word	0x0001b720
    5e0c:	0001b6d6 	.word	0x0001b6d6
    5e10:	0001b702 	.word	0x0001b702
    5e14:	3c00d168 	.word	0x3c00d168
    5e18:	0001b711 	.word	0x0001b711
    5e1c:	0001b6f2 	.word	0x0001b6f2
    5e20:	0001b73c 	.word	0x0001b73c
    5e24:	0001b74b 	.word	0x0001b74b
    5e28:	0001b75d 	.word	0x0001b75d
    5e2c:	0001b776 	.word	0x0001b776
    5e30:	0001b7a3 	.word	0x0001b7a3
    5e34:	0001b7bd 	.word	0x0001b7bd
    5e38:	0001b7db 	.word	0x0001b7db

00005e3c <vs_is_assigned>:
    5e3c:	e59f300c 	ldr	r3, [pc, #12]	; 5e50 <vs_is_assigned+0x14>
    5e40:	e5930000 	ldr	r0, [r3]
    5e44:	e2500000 	subs	r0, r0, #0	; 0x0
    5e48:	13a00001 	movne	r0, #1	; 0x1
    5e4c:	e12fff1e 	bx	lr
    5e50:	3c00d180 	.word	0x3c00d180

00005e54 <vs_is_opt_available>:
    5e54:	e350000d 	cmp	r0, #13	; 0xd
    5e58:	959f2078 	ldrls	r2, [pc, #120]	; 5ed8 <vs_is_opt_available+0x84>
    5e5c:	93a0302c 	movls	r3, #44	; 0x2c
    5e60:	859f3074 	ldrhi	r3, [pc, #116]	; 5edc <vs_is_opt_available+0x88>
    5e64:	90202093 	mlals	r0, r3, r0, r2
    5e68:	85930000 	ldrhi	r0, [r3]
    5e6c:	e3500000 	cmp	r0, #0	; 0x0
    5e70:	0a000016 	beq	5ed0 <vs_is_opt_available+0x7c>
    5e74:	e3510005 	cmp	r1, #5	; 0x5
    5e78:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    5e7c:	ea000013 	b	5ed0 <vs_is_opt_available+0x7c>
    5e80:	00005e98 	.word	0x00005e98
    5e84:	00005ea0 	.word	0x00005ea0
    5e88:	00005ea8 	.word	0x00005ea8
    5e8c:	00005eb0 	.word	0x00005eb0
    5e90:	00005eb8 	.word	0x00005eb8
    5e94:	00005ec0 	.word	0x00005ec0
    5e98:	e590300c 	ldr	r3, [r0, #12]
    5e9c:	ea000008 	b	5ec4 <vs_is_opt_available+0x70>
    5ea0:	e5903010 	ldr	r3, [r0, #16]
    5ea4:	ea000006 	b	5ec4 <vs_is_opt_available+0x70>
    5ea8:	e5903014 	ldr	r3, [r0, #20]
    5eac:	ea000004 	b	5ec4 <vs_is_opt_available+0x70>
    5eb0:	e5903018 	ldr	r3, [r0, #24]
    5eb4:	ea000002 	b	5ec4 <vs_is_opt_available+0x70>
    5eb8:	e590301c 	ldr	r3, [r0, #28]
    5ebc:	ea000000 	b	5ec4 <vs_is_opt_available+0x70>
    5ec0:	e5903020 	ldr	r3, [r0, #32]
    5ec4:	e2530000 	subs	r0, r3, #0	; 0x0
    5ec8:	13a00001 	movne	r0, #1	; 0x1
    5ecc:	e12fff1e 	bx	lr
    5ed0:	e3a00000 	mov	r0, #0	; 0x0
    5ed4:	e12fff1e 	bx	lr
    5ed8:	00013190 	.word	0x00013190
    5edc:	3c00d180 	.word	0x3c00d180

00005ee0 <vs_write>:
    5ee0:	e92d4010 	push	{r4, lr}
    5ee4:	e24dd008 	sub	sp, sp, #8	; 0x8
    5ee8:	e59d4010 	ldr	r4, [sp, #16]
    5eec:	e3540000 	cmp	r4, #0	; 0x0
    5ef0:	01a00004 	moveq	r0, r4
    5ef4:	0a00000c 	beq	5f2c <vs_write+0x4c>
    5ef8:	e59f1034 	ldr	r1, [pc, #52]	; 5f34 <vs_write+0x54>
    5efc:	e5911000 	ldr	r1, [r1]
    5f00:	e3510000 	cmp	r1, #0	; 0x0
    5f04:	03e00001 	mvneq	r0, #1	; 0x1
    5f08:	0a000007 	beq	5f2c <vs_write+0x4c>
    5f0c:	e591c014 	ldr	ip, [r1, #20]
    5f10:	e35c0000 	cmp	ip, #0	; 0x0
    5f14:	03e00002 	mvneq	r0, #2	; 0x2
    5f18:	0a000003 	beq	5f2c <vs_write+0x4c>
    5f1c:	e59d1014 	ldr	r1, [sp, #20]
    5f20:	e58d4000 	str	r4, [sp]
    5f24:	e58d1004 	str	r1, [sp, #4]
    5f28:	e12fff3c 	blx	ip
    5f2c:	e28dd008 	add	sp, sp, #8	; 0x8
    5f30:	e8bd8010 	pop	{r4, pc}
    5f34:	3c00d180 	.word	0x3c00d180

00005f38 <vs_erase>:
    5f38:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5f3c:	e59f302c 	ldr	r3, [pc, #44]	; 5f70 <vs_erase+0x38>
    5f40:	e24dd004 	sub	sp, sp, #4	; 0x4
    5f44:	e5933000 	ldr	r3, [r3]
    5f48:	e3530000 	cmp	r3, #0	; 0x0
    5f4c:	03e00001 	mvneq	r0, #1	; 0x1
    5f50:	0a000004 	beq	5f68 <vs_erase+0x30>
    5f54:	e593301c 	ldr	r3, [r3, #28]
    5f58:	e3530000 	cmp	r3, #0	; 0x0
    5f5c:	03e00002 	mvneq	r0, #2	; 0x2
    5f60:	0a000000 	beq	5f68 <vs_erase+0x30>
    5f64:	e12fff33 	blx	r3
    5f68:	e28dd004 	add	sp, sp, #4	; 0x4
    5f6c:	e8bd8000 	pop	{pc}
    5f70:	3c00d180 	.word	0x3c00d180

00005f74 <vs_isbad>:
    5f74:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5f78:	e59f302c 	ldr	r3, [pc, #44]	; 5fac <vs_isbad+0x38>
    5f7c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5f80:	e5933000 	ldr	r3, [r3]
    5f84:	e3530000 	cmp	r3, #0	; 0x0
    5f88:	03e00001 	mvneq	r0, #1	; 0x1
    5f8c:	0a000004 	beq	5fa4 <vs_isbad+0x30>
    5f90:	e5933024 	ldr	r3, [r3, #36]
    5f94:	e3530000 	cmp	r3, #0	; 0x0
    5f98:	03e00002 	mvneq	r0, #2	; 0x2
    5f9c:	0a000000 	beq	5fa4 <vs_isbad+0x30>
    5fa0:	e12fff33 	blx	r3
    5fa4:	e28dd004 	add	sp, sp, #4	; 0x4
    5fa8:	e8bd8000 	pop	{pc}
    5fac:	3c00d180 	.word	0x3c00d180

00005fb0 <vs_scrub>:
    5fb0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5fb4:	e59f302c 	ldr	r3, [pc, #44]	; 5fe8 <vs_scrub+0x38>
    5fb8:	e24dd004 	sub	sp, sp, #4	; 0x4
    5fbc:	e5933000 	ldr	r3, [r3]
    5fc0:	e3530000 	cmp	r3, #0	; 0x0
    5fc4:	03e00001 	mvneq	r0, #1	; 0x1
    5fc8:	0a000004 	beq	5fe0 <vs_scrub+0x30>
    5fcc:	e5933020 	ldr	r3, [r3, #32]
    5fd0:	e3530000 	cmp	r3, #0	; 0x0
    5fd4:	03e00002 	mvneq	r0, #2	; 0x2
    5fd8:	0a000000 	beq	5fe0 <vs_scrub+0x30>
    5fdc:	e12fff33 	blx	r3
    5fe0:	e28dd004 	add	sp, sp, #4	; 0x4
    5fe4:	e8bd8000 	pop	{pc}
    5fe8:	3c00d180 	.word	0x3c00d180

00005fec <vs_special>:
    5fec:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5ff0:	e59f302c 	ldr	r3, [pc, #44]	; 6024 <vs_special+0x38>
    5ff4:	e24dd004 	sub	sp, sp, #4	; 0x4
    5ff8:	e5933000 	ldr	r3, [r3]
    5ffc:	e3530000 	cmp	r3, #0	; 0x0
    6000:	03e00001 	mvneq	r0, #1	; 0x1
    6004:	0a000004 	beq	601c <vs_special+0x30>
    6008:	e5933028 	ldr	r3, [r3, #40]
    600c:	e3530000 	cmp	r3, #0	; 0x0
    6010:	03e00002 	mvneq	r0, #2	; 0x2
    6014:	0a000000 	beq	601c <vs_special+0x30>
    6018:	e12fff33 	blx	r3
    601c:	e28dd004 	add	sp, sp, #4	; 0x4
    6020:	e8bd8000 	pop	{pc}
    6024:	3c00d180 	.word	0x3c00d180

00006028 <vs_align>:
    6028:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    602c:	e350000e 	cmp	r0, #14	; 0xe
    6030:	059f3044 	ldreq	r3, [pc, #68]	; 607c <vs_align+0x54>
    6034:	e24dd004 	sub	sp, sp, #4	; 0x4
    6038:	05930000 	ldreq	r0, [r3]
    603c:	0a000004 	beq	6054 <vs_align+0x2c>
    6040:	e350000d 	cmp	r0, #13	; 0xd
    6044:	8a000009 	bhi	6070 <vs_align+0x48>
    6048:	e59f2030 	ldr	r2, [pc, #48]	; 6080 <vs_align+0x58>
    604c:	e3a0302c 	mov	r3, #44	; 0x2c
    6050:	e0202093 	mla	r0, r3, r0, r2
    6054:	e3500000 	cmp	r0, #0	; 0x0
    6058:	0a000004 	beq	6070 <vs_align+0x48>
    605c:	e590000c 	ldr	r0, [r0, #12]
    6060:	e3500000 	cmp	r0, #0	; 0x0
    6064:	0a000001 	beq	6070 <vs_align+0x48>
    6068:	e12fff30 	blx	r0
    606c:	ea000000 	b	6074 <vs_align+0x4c>
    6070:	e3a00000 	mov	r0, #0	; 0x0
    6074:	e28dd004 	add	sp, sp, #4	; 0x4
    6078:	e8bd8000 	pop	{pc}
    607c:	3c00d180 	.word	0x3c00d180
    6080:	00013190 	.word	0x00013190

00006084 <vs_device_bootable>:
    6084:	e350000e 	cmp	r0, #14	; 0xe
    6088:	059f3030 	ldreq	r3, [pc, #48]	; 60c0 <vs_device_bootable+0x3c>
    608c:	05930000 	ldreq	r0, [r3]
    6090:	0a000004 	beq	60a8 <vs_device_bootable+0x24>
    6094:	e350000d 	cmp	r0, #13	; 0xd
    6098:	8a000006 	bhi	60b8 <vs_device_bootable+0x34>
    609c:	e59f2020 	ldr	r2, [pc, #32]	; 60c4 <vs_device_bootable+0x40>
    60a0:	e3a0302c 	mov	r3, #44	; 0x2c
    60a4:	e0202093 	mla	r0, r3, r0, r2
    60a8:	e3500000 	cmp	r0, #0	; 0x0
    60ac:	15903008 	ldrne	r3, [r0, #8]
    60b0:	12030001 	andne	r0, r3, #1	; 0x1
    60b4:	112fff1e 	bxne	lr
    60b8:	e3a00000 	mov	r0, #0	; 0x0
    60bc:	e12fff1e 	bx	lr
    60c0:	3c00d180 	.word	0x3c00d180
    60c4:	00013190 	.word	0x00013190

000060c8 <vs_device_burnsrc>:
    60c8:	e350000e 	cmp	r0, #14	; 0xe
    60cc:	059f3034 	ldreq	r3, [pc, #52]	; 6108 <vs_device_burnsrc+0x40>
    60d0:	05930000 	ldreq	r0, [r3]
    60d4:	0a000004 	beq	60ec <vs_device_burnsrc+0x24>
    60d8:	e350000d 	cmp	r0, #13	; 0xd
    60dc:	8a000007 	bhi	6100 <vs_device_burnsrc+0x38>
    60e0:	e59f2024 	ldr	r2, [pc, #36]	; 610c <vs_device_burnsrc+0x44>
    60e4:	e3a0302c 	mov	r3, #44	; 0x2c
    60e8:	e0202093 	mla	r0, r3, r0, r2
    60ec:	e3500000 	cmp	r0, #0	; 0x0
    60f0:	15903008 	ldrne	r3, [r0, #8]
    60f4:	11a030a3 	lsrne	r3, r3, #1
    60f8:	12030001 	andne	r0, r3, #1	; 0x1
    60fc:	112fff1e 	bxne	lr
    6100:	e3a00000 	mov	r0, #0	; 0x0
    6104:	e12fff1e 	bx	lr
    6108:	3c00d180 	.word	0x3c00d180
    610c:	00013190 	.word	0x00013190

00006110 <vs_device_burndst>:
    6110:	e350000e 	cmp	r0, #14	; 0xe
    6114:	059f3034 	ldreq	r3, [pc, #52]	; 6150 <vs_device_burndst+0x40>
    6118:	05930000 	ldreq	r0, [r3]
    611c:	0a000004 	beq	6134 <vs_device_burndst+0x24>
    6120:	e350000d 	cmp	r0, #13	; 0xd
    6124:	8a000007 	bhi	6148 <vs_device_burndst+0x38>
    6128:	e59f2024 	ldr	r2, [pc, #36]	; 6154 <vs_device_burndst+0x44>
    612c:	e3a0302c 	mov	r3, #44	; 0x2c
    6130:	e0202093 	mla	r0, r3, r0, r2
    6134:	e3500000 	cmp	r0, #0	; 0x0
    6138:	15903008 	ldrne	r3, [r0, #8]
    613c:	11a03123 	lsrne	r3, r3, #2
    6140:	12030001 	andne	r0, r3, #1	; 0x1
    6144:	112fff1e 	bxne	lr
    6148:	e3a00000 	mov	r0, #0	; 0x0
    614c:	e12fff1e 	bx	lr
    6150:	3c00d180 	.word	0x3c00d180
    6154:	00013190 	.word	0x00013190

00006158 <vs_device_boot0>:
    6158:	e350000e 	cmp	r0, #14	; 0xe
    615c:	059f3034 	ldreq	r3, [pc, #52]	; 6198 <vs_device_boot0+0x40>
    6160:	05930000 	ldreq	r0, [r3]
    6164:	0a000004 	beq	617c <vs_device_boot0+0x24>
    6168:	e350000d 	cmp	r0, #13	; 0xd
    616c:	8a000007 	bhi	6190 <vs_device_boot0+0x38>
    6170:	e59f2024 	ldr	r2, [pc, #36]	; 619c <vs_device_boot0+0x44>
    6174:	e3a0302c 	mov	r3, #44	; 0x2c
    6178:	e0202093 	mla	r0, r3, r0, r2
    617c:	e3500000 	cmp	r0, #0	; 0x0
    6180:	15903008 	ldrne	r3, [r0, #8]
    6184:	11a031a3 	lsrne	r3, r3, #3
    6188:	12030001 	andne	r0, r3, #1	; 0x1
    618c:	112fff1e 	bxne	lr
    6190:	e3a00000 	mov	r0, #0	; 0x0
    6194:	e12fff1e 	bx	lr
    6198:	3c00d180 	.word	0x3c00d180
    619c:	00013190 	.word	0x00013190

000061a0 <vs_device_name>:
    61a0:	e350000e 	cmp	r0, #14	; 0xe
    61a4:	059f3028 	ldreq	r3, [pc, #40]	; 61d4 <vs_device_name+0x34>
    61a8:	05930000 	ldreq	r0, [r3]
    61ac:	0a000004 	beq	61c4 <vs_device_name+0x24>
    61b0:	e350000d 	cmp	r0, #13	; 0xd
    61b4:	8a000004 	bhi	61cc <vs_device_name+0x2c>
    61b8:	e59f2018 	ldr	r2, [pc, #24]	; 61d8 <vs_device_name+0x38>
    61bc:	e3a0302c 	mov	r3, #44	; 0x2c
    61c0:	e0202093 	mla	r0, r3, r0, r2
    61c4:	e3500000 	cmp	r0, #0	; 0x0
    61c8:	112fff1e 	bxne	lr
    61cc:	e3a00000 	mov	r0, #0	; 0x0
    61d0:	e12fff1e 	bx	lr
    61d4:	3c00d180 	.word	0x3c00d180
    61d8:	00013190 	.word	0x00013190

000061dc <vs_device_erasable>:
    61dc:	e350000e 	cmp	r0, #14	; 0xe
    61e0:	059f3034 	ldreq	r3, [pc, #52]	; 621c <vs_device_erasable+0x40>
    61e4:	05930000 	ldreq	r0, [r3]
    61e8:	0a000004 	beq	6200 <vs_device_erasable+0x24>
    61ec:	e350000d 	cmp	r0, #13	; 0xd
    61f0:	8a000007 	bhi	6214 <vs_device_erasable+0x38>
    61f4:	e59f2024 	ldr	r2, [pc, #36]	; 6220 <vs_device_erasable+0x44>
    61f8:	e3a0302c 	mov	r3, #44	; 0x2c
    61fc:	e0202093 	mla	r0, r3, r0, r2
    6200:	e3500000 	cmp	r0, #0	; 0x0
    6204:	15903008 	ldrne	r3, [r0, #8]
    6208:	11a03223 	lsrne	r3, r3, #4
    620c:	12030001 	andne	r0, r3, #1	; 0x1
    6210:	112fff1e 	bxne	lr
    6214:	e3a00000 	mov	r0, #0	; 0x0
    6218:	e12fff1e 	bx	lr
    621c:	3c00d180 	.word	0x3c00d180
    6220:	00013190 	.word	0x00013190

00006224 <vs_device_count>:
    6224:	e3a0000e 	mov	r0, #14	; 0xe
    6228:	e12fff1e 	bx	lr

0000622c <vs_device_id>:
    622c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6230:	e59f7040 	ldr	r7, [pc, #64]	; 6278 <vs_device_id+0x4c>
    6234:	e24dd004 	sub	sp, sp, #4	; 0x4
    6238:	e1a05000 	mov	r5, r0
    623c:	e3a04000 	mov	r4, #0	; 0x0
    6240:	e3a0602c 	mov	r6, #44	; 0x2c
    6244:	e0217496 	mla	r1, r6, r4, r7
    6248:	e1a00005 	mov	r0, r5
    624c:	e3a02010 	mov	r2, #16	; 0x10
    6250:	ebffecad 	bl	150c <strncmp>
    6254:	e3500000 	cmp	r0, #0	; 0x0
    6258:	0a000003 	beq	626c <vs_device_id+0x40>
    625c:	e2844001 	add	r4, r4, #1	; 0x1
    6260:	e354000e 	cmp	r4, #14	; 0xe
    6264:	1afffff6 	bne	6244 <vs_device_id+0x18>
    6268:	e2844001 	add	r4, r4, #1	; 0x1
    626c:	e1a00004 	mov	r0, r4
    6270:	e28dd004 	add	sp, sp, #4	; 0x4
    6274:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6278:	00013190 	.word	0x00013190

0000627c <vs_is_device>:
    627c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6280:	e350000e 	cmp	r0, #14	; 0xe
    6284:	059f304c 	ldreq	r3, [pc, #76]	; 62d8 <vs_is_device+0x5c>
    6288:	e1a0c001 	mov	ip, r1
    628c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6290:	05931000 	ldreq	r1, [r3]
    6294:	0a000004 	beq	62ac <vs_is_device+0x30>
    6298:	e350000d 	cmp	r0, #13	; 0xd
    629c:	8a00000a 	bhi	62cc <vs_is_device+0x50>
    62a0:	e59f2034 	ldr	r2, [pc, #52]	; 62dc <vs_is_device+0x60>
    62a4:	e3a0302c 	mov	r3, #44	; 0x2c
    62a8:	e0212093 	mla	r1, r3, r0, r2
    62ac:	e3510000 	cmp	r1, #0	; 0x0
    62b0:	0a000005 	beq	62cc <vs_is_device+0x50>
    62b4:	e1a0000c 	mov	r0, ip
    62b8:	e3a02010 	mov	r2, #16	; 0x10
    62bc:	ebffec92 	bl	150c <strncmp>
    62c0:	e2700001 	rsbs	r0, r0, #1	; 0x1
    62c4:	33a00000 	movcc	r0, #0	; 0x0
    62c8:	ea000000 	b	62d0 <vs_is_device+0x54>
    62cc:	e3a00000 	mov	r0, #0	; 0x0
    62d0:	e28dd004 	add	sp, sp, #4	; 0x4
    62d4:	e8bd8000 	pop	{pc}
    62d8:	3c00d180 	.word	0x3c00d180
    62dc:	00013190 	.word	0x00013190

000062e0 <vs_read>:
    62e0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    62e4:	e59f4234 	ldr	r4, [pc, #564]	; 6520 <vs_read+0x240>
    62e8:	e1a09000 	mov	r9, r0
    62ec:	e5940000 	ldr	r0, [r4]
    62f0:	e24dd024 	sub	sp, sp, #36	; 0x24
    62f4:	e3500000 	cmp	r0, #0	; 0x0
    62f8:	e1a06002 	mov	r6, r2
    62fc:	e1a07003 	mov	r7, r3
    6300:	e59d5048 	ldr	r5, [sp, #72]
    6304:	03e04001 	mvneq	r4, #1	; 0x1
    6308:	0a000081 	beq	6514 <vs_read+0x234>
    630c:	e5903010 	ldr	r3, [r0, #16]
    6310:	e3530000 	cmp	r3, #0	; 0x0
    6314:	03e04002 	mvneq	r4, #2	; 0x2
    6318:	0a00007d 	beq	6514 <vs_read+0x234>
    631c:	e3550000 	cmp	r5, #0	; 0x0
    6320:	01a04005 	moveq	r4, r5
    6324:	0a00007a 	beq	6514 <vs_read+0x234>
    6328:	e59f11f4 	ldr	r1, [pc, #500]	; 6524 <vs_read+0x244>
    632c:	e3a02003 	mov	r2, #3	; 0x3
    6330:	ebffec75 	bl	150c <strncmp>
    6334:	e3500000 	cmp	r0, #0	; 0x0
    6338:	1a00000a 	bne	6368 <vs_read+0x88>
    633c:	e59d204c 	ldr	r2, [sp, #76]
    6340:	e58d5000 	str	r5, [sp]
    6344:	e58d2004 	str	r2, [sp, #4]
    6348:	e5941000 	ldr	r1, [r4]
    634c:	e1a00009 	mov	r0, r9
    6350:	e1a02006 	mov	r2, r6
    6354:	e1a03007 	mov	r3, r7
    6358:	e1a0e00f 	mov	lr, pc
    635c:	e591f010 	ldr	pc, [r1, #16]
    6360:	e1a04000 	mov	r4, r0
    6364:	ea00006a 	b	6514 <vs_read+0x234>
    6368:	e3a0000e 	mov	r0, #14	; 0xe
    636c:	ebffff2d 	bl	6028 <vs_align>
    6370:	e2402001 	sub	r2, r0, #1	; 0x1
    6374:	e0124006 	ands	r4, r2, r6
    6378:	10644000 	rsbne	r4, r4, r0
    637c:	e0643005 	rsb	r3, r4, r5
    6380:	e0032002 	and	r2, r3, r2
    6384:	e192a004 	orrs	sl, r2, r4
    6388:	e1a08000 	mov	r8, r0
    638c:	e58d3014 	str	r3, [sp, #20]
    6390:	e58d200c 	str	r2, [sp, #12]
    6394:	e58da018 	str	sl, [sp, #24]
    6398:	01a0400a 	moveq	r4, sl
    639c:	058da01c 	streq	sl, [sp, #28]
    63a0:	0a00002c 	beq	6458 <vs_read+0x178>
    63a4:	ebffecf6 	bl	1784 <malloc>
    63a8:	e250b000 	subs	fp, r0, #0	; 0x0
    63ac:	e58db01c 	str	fp, [sp, #28]
    63b0:	1a00000c 	bne	63e8 <vs_read+0x108>
    63b4:	e59f416c 	ldr	r4, [pc, #364]	; 6528 <vs_read+0x248>
    63b8:	e59f016c 	ldr	r0, [pc, #364]	; 652c <vs_read+0x24c>
    63bc:	e3001115 	movw	r1, #277	; 0x115
    63c0:	e1a0e00f 	mov	lr, pc
    63c4:	e594f000 	ldr	pc, [r4]
    63c8:	e59f0160 	ldr	r0, [pc, #352]	; 6530 <vs_read+0x250>
    63cc:	e59f1160 	ldr	r1, [pc, #352]	; 6534 <vs_read+0x254>
    63d0:	e1a0e00f 	mov	lr, pc
    63d4:	e594f000 	ldr	pc, [r4]
    63d8:	e59f0158 	ldr	r0, [pc, #344]	; 6538 <vs_read+0x258>
    63dc:	e1a0e00f 	mov	lr, pc
    63e0:	e594f000 	ldr	pc, [r4]
    63e4:	eafffffe 	b	63e4 <vs_read+0x104>
    63e8:	e3540000 	cmp	r4, #0	; 0x0
    63ec:	0a000019 	beq	6458 <vs_read+0x178>
    63f0:	e59f2128 	ldr	r2, [pc, #296]	; 6520 <vs_read+0x240>
    63f4:	e2683000 	rsb	r3, r8, #0	; 0x0
    63f8:	e5921000 	ldr	r1, [r2]
    63fc:	e59d204c 	ldr	r2, [sp, #76]
    6400:	e006a003 	and	sl, r6, r3
    6404:	e3a0b000 	mov	fp, #0	; 0x0
    6408:	e58d2004 	str	r2, [sp, #4]
    640c:	e1a0300b 	mov	r3, fp
    6410:	e58d8000 	str	r8, [sp]
    6414:	e1a0200a 	mov	r2, sl
    6418:	e1a0e00f 	mov	lr, pc
    641c:	e591f010 	ldr	pc, [r1, #16]
    6420:	e250b000 	subs	fp, r0, #0	; 0x0
    6424:	b1a0400b 	movlt	r4, fp
    6428:	ba000037 	blt	650c <vs_read+0x22c>
    642c:	e59d201c 	ldr	r2, [sp, #28]
    6430:	e0641008 	rsb	r1, r4, r8
    6434:	e0821001 	add	r1, r2, r1
    6438:	e1a00009 	mov	r0, r9
    643c:	e1a02004 	mov	r2, r4
    6440:	ebffec6a 	bl	15f0 <memcpy>
    6444:	e0966004 	adds	r6, r6, r4
    6448:	e59d5014 	ldr	r5, [sp, #20]
    644c:	e0899004 	add	r9, r9, r4
    6450:	e2a77000 	adc	r7, r7, #0	; 0x0
    6454:	e1a0400b 	mov	r4, fp
    6458:	e59d300c 	ldr	r3, [sp, #12]
    645c:	e0555003 	subs	r5, r5, r3
    6460:	0a00000f 	beq	64a4 <vs_read+0x1c4>
    6464:	e59f30b4 	ldr	r3, [pc, #180]	; 6520 <vs_read+0x240>
    6468:	e59da04c 	ldr	sl, [sp, #76]
    646c:	e58d5000 	str	r5, [sp]
    6470:	e5931000 	ldr	r1, [r3]
    6474:	e1a00009 	mov	r0, r9
    6478:	e58da004 	str	sl, [sp, #4]
    647c:	e1a02006 	mov	r2, r6
    6480:	e1a03007 	mov	r3, r7
    6484:	e1a0e00f 	mov	lr, pc
    6488:	e591f010 	ldr	pc, [r1, #16]
    648c:	e3500000 	cmp	r0, #0	; 0x0
    6490:	ba000012 	blt	64e0 <vs_read+0x200>
    6494:	e0966005 	adds	r6, r6, r5
    6498:	e0a77fc5 	adc	r7, r7, r5, asr #31
    649c:	e0844000 	add	r4, r4, r0
    64a0:	e0899005 	add	r9, r9, r5
    64a4:	e59db00c 	ldr	fp, [sp, #12]
    64a8:	e35b0000 	cmp	fp, #0	; 0x0
    64ac:	0a000013 	beq	6500 <vs_read+0x220>
    64b0:	e59d204c 	ldr	r2, [sp, #76]
    64b4:	e59f3064 	ldr	r3, [pc, #100]	; 6520 <vs_read+0x240>
    64b8:	e58d2004 	str	r2, [sp, #4]
    64bc:	e58d8000 	str	r8, [sp]
    64c0:	e5931000 	ldr	r1, [r3]
    64c4:	e1a02006 	mov	r2, r6
    64c8:	e1a03007 	mov	r3, r7
    64cc:	e59d001c 	ldr	r0, [sp, #28]
    64d0:	e1a0e00f 	mov	lr, pc
    64d4:	e591f010 	ldr	pc, [r1, #16]
    64d8:	e3500000 	cmp	r0, #0	; 0x0
    64dc:	aa000001 	bge	64e8 <vs_read+0x208>
    64e0:	e1a04000 	mov	r4, r0
    64e4:	ea000005 	b	6500 <vs_read+0x220>
    64e8:	e1a00009 	mov	r0, r9
    64ec:	e59d101c 	ldr	r1, [sp, #28]
    64f0:	e59d200c 	ldr	r2, [sp, #12]
    64f4:	ebffec3d 	bl	15f0 <memcpy>
    64f8:	e59d300c 	ldr	r3, [sp, #12]
    64fc:	e0844003 	add	r4, r4, r3
    6500:	e59da018 	ldr	sl, [sp, #24]
    6504:	e35a0000 	cmp	sl, #0	; 0x0
    6508:	0a000001 	beq	6514 <vs_read+0x234>
    650c:	e59d001c 	ldr	r0, [sp, #28]
    6510:	ebffed03 	bl	1924 <free>
    6514:	e1a00004 	mov	r0, r4
    6518:	e28dd024 	add	sp, sp, #36	; 0x24
    651c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6520:	3c00d180 	.word	0x3c00d180
    6524:	00019f16 	.word	0x00019f16
    6528:	3c00d694 	.word	0x3c00d694
    652c:	0001b804 	.word	0x0001b804
    6530:	0001a0ca 	.word	0x0001a0ca
    6534:	0001b816 	.word	0x0001b816
    6538:	0001b821 	.word	0x0001b821

0000653c <vs_reset>:
    653c:	e92d4010 	push	{r4, lr}
    6540:	e59f402c 	ldr	r4, [pc, #44]	; 6574 <vs_reset+0x38>
    6544:	e5940000 	ldr	r0, [r4]
    6548:	ebffeab5 	bl	1024 <cpu_recase>
    654c:	e5944000 	ldr	r4, [r4]
    6550:	e3540000 	cmp	r4, #0	; 0x0
    6554:	03e00001 	mvneq	r0, #1	; 0x1
    6558:	08bd8010 	popeq	{r4, pc}
    655c:	e5944018 	ldr	r4, [r4, #24]
    6560:	e3540000 	cmp	r4, #0	; 0x0
    6564:	03e00002 	mvneq	r0, #2	; 0x2
    6568:	08bd8010 	popeq	{r4, pc}
    656c:	e12fff34 	blx	r4
    6570:	e8bd8010 	pop	{r4, pc}
    6574:	3c00d180 	.word	0x3c00d180

00006578 <vs_assign_by_id>:
    6578:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    657c:	e59fc05c 	ldr	ip, [pc, #92]	; 65e0 <vs_assign_by_id+0x68>
    6580:	e3a03000 	mov	r3, #0	; 0x0
    6584:	e350000d 	cmp	r0, #13	; 0xd
    6588:	e24dd004 	sub	sp, sp, #4	; 0x4
    658c:	e58c3000 	str	r3, [ip]
    6590:	8a00000b 	bhi	65c4 <vs_assign_by_id+0x4c>
    6594:	e59f2048 	ldr	r2, [pc, #72]	; 65e4 <vs_assign_by_id+0x6c>
    6598:	e283302c 	add	r3, r3, #44	; 0x2c
    659c:	e0232093 	mla	r3, r3, r0, r2
    65a0:	e3510000 	cmp	r1, #0	; 0x0
    65a4:	e58c3000 	str	r3, [ip]
    65a8:	01a00001 	moveq	r0, r1
    65ac:	0a000002 	beq	65bc <vs_assign_by_id+0x44>
    65b0:	e28dd004 	add	sp, sp, #4	; 0x4
    65b4:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    65b8:	eaffffdf 	b	653c <vs_reset>
    65bc:	e28dd004 	add	sp, sp, #4	; 0x4
    65c0:	e8bd8000 	pop	{pc}
    65c4:	e1a01000 	mov	r1, r0
    65c8:	e59f3018 	ldr	r3, [pc, #24]	; 65e8 <vs_assign_by_id+0x70>
    65cc:	e59f0018 	ldr	r0, [pc, #24]	; 65ec <vs_assign_by_id+0x74>
    65d0:	e1a0e00f 	mov	lr, pc
    65d4:	e593f000 	ldr	pc, [r3]
    65d8:	e3e00000 	mvn	r0, #0	; 0x0
    65dc:	eafffff6 	b	65bc <vs_assign_by_id+0x44>
    65e0:	3c00d180 	.word	0x3c00d180
    65e4:	00013190 	.word	0x00013190
    65e8:	3c00d694 	.word	0x3c00d694
    65ec:	0001b852 	.word	0x0001b852

000065f0 <vs_assign_by_name>:
    65f0:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    65f4:	e59f3084 	ldr	r3, [pc, #132]	; 6680 <vs_assign_by_name+0x90>
    65f8:	e3a02000 	mov	r2, #0	; 0x0
    65fc:	e24dd004 	sub	sp, sp, #4	; 0x4
    6600:	e5832000 	str	r2, [r3]
    6604:	e1a06000 	mov	r6, r0
    6608:	e1a07001 	mov	r7, r1
    660c:	e1a04002 	mov	r4, r2
    6610:	e3a0a02c 	mov	sl, #44	; 0x2c
    6614:	e59f8068 	ldr	r8, [pc, #104]	; 6684 <vs_assign_by_name+0x94>
    6618:	ea00000d 	b	6654 <vs_assign_by_name+0x64>
    661c:	e025849a 	mla	r5, sl, r4, r8
    6620:	e1a00005 	mov	r0, r5
    6624:	ebffebb8 	bl	150c <strncmp>
    6628:	e3500000 	cmp	r0, #0	; 0x0
    662c:	1a000007 	bne	6650 <vs_assign_by_name+0x60>
    6630:	e59f3048 	ldr	r3, [pc, #72]	; 6680 <vs_assign_by_name+0x90>
    6634:	e3570000 	cmp	r7, #0	; 0x0
    6638:	e5835000 	str	r5, [r3]
    663c:	01a00007 	moveq	r0, r7
    6640:	0a00000c 	beq	6678 <vs_assign_by_name+0x88>
    6644:	e28dd004 	add	sp, sp, #4	; 0x4
    6648:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    664c:	eaffffba 	b	653c <vs_reset>
    6650:	e2844001 	add	r4, r4, #1	; 0x1
    6654:	e354000e 	cmp	r4, #14	; 0xe
    6658:	e1a01006 	mov	r1, r6
    665c:	e3a02010 	mov	r2, #16	; 0x10
    6660:	1affffed 	bne	661c <vs_assign_by_name+0x2c>
    6664:	e59f001c 	ldr	r0, [pc, #28]	; 6688 <vs_assign_by_name+0x98>
    6668:	e59f301c 	ldr	r3, [pc, #28]	; 668c <vs_assign_by_name+0x9c>
    666c:	e1a0e00f 	mov	lr, pc
    6670:	e593f000 	ldr	pc, [r3]
    6674:	e3e00000 	mvn	r0, #0	; 0x0
    6678:	e28dd004 	add	sp, sp, #4	; 0x4
    667c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    6680:	3c00d180 	.word	0x3c00d180
    6684:	00013190 	.word	0x00013190
    6688:	0001b86b 	.word	0x0001b86b
    668c:	3c00d694 	.word	0x3c00d694

00006690 <ss_lock_isk>:
    6690:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6694:	e24dd004 	sub	sp, sp, #4	; 0x4
    6698:	eb0001d5 	bl	6df4 <security_enabled>
    669c:	e3500000 	cmp	r0, #0	; 0x0
    66a0:	0a000002 	beq	66b0 <ss_lock_isk+0x20>
    66a4:	e28dd004 	add	sp, sp, #4	; 0x4
    66a8:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    66ac:	ea00002e 	b	676c <block_cipher_lock_isk>
    66b0:	e28dd004 	add	sp, sp, #4	; 0x4
    66b4:	e8bd8000 	pop	{pc}

000066b8 <ss_aes_data>:
    66b8:	e92d4070 	push	{r4, r5, r6, lr}
    66bc:	e1a05000 	mov	r5, r0
    66c0:	e1a06001 	mov	r6, r1
    66c4:	e1a04002 	mov	r4, r2
    66c8:	eb0001c9 	bl	6df4 <security_enabled>
    66cc:	e3500000 	cmp	r0, #0	; 0x0
    66d0:	0a000007 	beq	66f4 <ss_aes_data+0x3c>
    66d4:	e284200f 	add	r2, r4, #15	; 0xf
    66d8:	e1a00005 	mov	r0, r5
    66dc:	e1a01006 	mov	r1, r6
    66e0:	e3c2200f 	bic	r2, r2, #15	; 0xf
    66e4:	eb0000a0 	bl	696c <block_cipher_operation>
    66e8:	e3500000 	cmp	r0, #0	; 0x0
    66ec:	13e00000 	mvnne	r0, #0	; 0x0
    66f0:	18bd8070 	popne	{r4, r5, r6, pc}
    66f4:	e3a00000 	mov	r0, #0	; 0x0
    66f8:	e8bd8070 	pop	{r4, r5, r6, pc}

000066fc <ss_aes_init>:
    66fc:	e92d4070 	push	{r4, r5, r6, lr}
    6700:	e1a05000 	mov	r5, r0
    6704:	e1a04001 	mov	r4, r1
    6708:	e1a06002 	mov	r6, r2
    670c:	eb0001b8 	bl	6df4 <security_enabled>
    6710:	e3500000 	cmp	r0, #0	; 0x0
    6714:	0a00000b 	beq	6748 <ss_aes_init+0x4c>
    6718:	e3540000 	cmp	r4, #0	; 0x0
    671c:	01a00005 	moveq	r0, r5
    6720:	01a02004 	moveq	r2, r4
    6724:	01a03006 	moveq	r3, r6
    6728:	03a01001 	moveq	r1, #1	; 0x1
    672c:	11a00005 	movne	r0, r5
    6730:	11a02004 	movne	r2, r4
    6734:	11a03006 	movne	r3, r6
    6738:	13a01000 	movne	r1, #0	; 0x0
    673c:	eb00002c 	bl	67f4 <block_cipher_aes_init>
    6740:	e3500000 	cmp	r0, #0	; 0x0
    6744:	1a000001 	bne	6750 <ss_aes_init+0x54>
    6748:	e3a00000 	mov	r0, #0	; 0x0
    674c:	e8bd8070 	pop	{r4, r5, r6, pc}
    6750:	e3e00000 	mvn	r0, #0	; 0x0
    6754:	e8bd8070 	pop	{r4, r5, r6, pc}

00006758 <check_register_bit>:
    6758:	e3a03001 	mov	r3, #1	; 0x1
    675c:	e0103113 	ands	r3, r0, r3, lsl r1
    6760:	03a00000 	moveq	r0, #0	; 0x0
    6764:	13a00001 	movne	r0, #1	; 0x1
    6768:	e12fff1e 	bx	lr

0000676c <block_cipher_lock_isk>:
    676c:	e59f200c 	ldr	r2, [pc, #12]	; 6780 <block_cipher_lock_isk+0x14>
    6770:	e5923400 	ldr	r3, [r2, #1024]
    6774:	e3833706 	orr	r3, r3, #1572864	; 0x180000
    6778:	e5823400 	str	r3, [r2, #1024]
    677c:	e12fff1e 	bx	lr
    6780:	23200000 	.word	0x23200000

00006784 <block_cipher_reset>:
    6784:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6788:	e59f4060 	ldr	r4, [pc, #96]	; 67f0 <block_cipher_reset+0x6c>
    678c:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    6790:	e5843400 	str	r3, [r4, #1024]
    6794:	e24dd004 	sub	sp, sp, #4	; 0x4
    6798:	eb00134c 	bl	b4d0 <get_ticks>
    679c:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    67a0:	e3a07000 	mov	r7, #0	; 0x0
    67a4:	e0966000 	adds	r6, r6, r0
    67a8:	e1a05004 	mov	r5, r4
    67ac:	e0a77001 	adc	r7, r7, r1
    67b0:	e5944400 	ldr	r4, [r4, #1024]
    67b4:	ea000008 	b	67dc <block_cipher_reset+0x58>
    67b8:	e5954400 	ldr	r4, [r5, #1024]
    67bc:	eb001343 	bl	b4d0 <get_ticks>
    67c0:	e1510007 	cmp	r1, r7
    67c4:	8a000002 	bhi	67d4 <block_cipher_reset+0x50>
    67c8:	1a000003 	bne	67dc <block_cipher_reset+0x58>
    67cc:	e1500006 	cmp	r0, r6
    67d0:	9a000001 	bls	67dc <block_cipher_reset+0x58>
    67d4:	e3a00001 	mov	r0, #1	; 0x1
    67d8:	ea000002 	b	67e8 <block_cipher_reset+0x64>
    67dc:	e3540000 	cmp	r4, #0	; 0x0
    67e0:	bafffff4 	blt	67b8 <block_cipher_reset+0x34>
    67e4:	e3a00000 	mov	r0, #0	; 0x0
    67e8:	e28dd004 	add	sp, sp, #4	; 0x4
    67ec:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    67f0:	23200000 	.word	0x23200000

000067f4 <block_cipher_aes_init>:
    67f4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    67f8:	e24dd004 	sub	sp, sp, #4	; 0x4
    67fc:	e1a05000 	mov	r5, r0
    6800:	e1a04001 	mov	r4, r1
    6804:	e1a06002 	mov	r6, r2
    6808:	e1a07003 	mov	r7, r3
    680c:	ebffffdc 	bl	6784 <block_cipher_reset>
    6810:	e3500000 	cmp	r0, #0	; 0x0
    6814:	1a00003d 	bne	6910 <block_cipher_aes_init+0x11c>
    6818:	e59f3134 	ldr	r3, [pc, #308]	; 6954 <block_cipher_aes_init+0x160>
    681c:	e3a02001 	mov	r2, #1	; 0x1
    6820:	e3550001 	cmp	r5, #1	; 0x1
    6824:	e5832404 	str	r2, [r3, #1028]
    6828:	8a000038 	bhi	6910 <block_cipher_aes_init+0x11c>
    682c:	e3540003 	cmp	r4, #3	; 0x3
    6830:	13540000 	cmpne	r4, #0	; 0x0
    6834:	0a000001 	beq	6840 <block_cipher_aes_init+0x4c>
    6838:	e1540002 	cmp	r4, r2
    683c:	1a000033 	bne	6910 <block_cipher_aes_init+0x11c>
    6840:	e1a03305 	lsl	r3, r5, #6
    6844:	e3833901 	orr	r3, r3, #16384	; 0x4000
    6848:	e3833004 	orr	r3, r3, #4	; 0x4
    684c:	e59f2100 	ldr	r2, [pc, #256]	; 6954 <block_cipher_aes_init+0x160>
    6850:	e1833984 	orr	r3, r3, r4, lsl #19
    6854:	e3540000 	cmp	r4, #0	; 0x0
    6858:	e5823400 	str	r3, [r2, #1024]
    685c:	1a000008 	bne	6884 <block_cipher_aes_init+0x90>
    6860:	e59f10f0 	ldr	r1, [pc, #240]	; 6958 <block_cipher_aes_init+0x164>
    6864:	e59f00f0 	ldr	r0, [pc, #240]	; 695c <block_cipher_aes_init+0x168>
    6868:	e59f30f0 	ldr	r3, [pc, #240]	; 6960 <block_cipher_aes_init+0x16c>
    686c:	e0862001 	add	r2, r6, r1
    6870:	e0823003 	add	r3, r2, r3
    6874:	e5933000 	ldr	r3, [r3]
    6878:	e4813004 	str	r3, [r1], #4
    687c:	e1510000 	cmp	r1, r0
    6880:	1afffff8 	bne	6868 <block_cipher_aes_init+0x74>
    6884:	e59f10d0 	ldr	r1, [pc, #208]	; 695c <block_cipher_aes_init+0x168>
    6888:	e59f00d4 	ldr	r0, [pc, #212]	; 6964 <block_cipher_aes_init+0x170>
    688c:	e59f30d4 	ldr	r3, [pc, #212]	; 6968 <block_cipher_aes_init+0x174>
    6890:	e0872001 	add	r2, r7, r1
    6894:	e0823003 	add	r3, r2, r3
    6898:	e5933000 	ldr	r3, [r3]
    689c:	e4813004 	str	r3, [r1], #4
    68a0:	e1510000 	cmp	r1, r0
    68a4:	1afffff8 	bne	688c <block_cipher_aes_init+0x98>
    68a8:	e3540000 	cmp	r4, #0	; 0x0
    68ac:	1a000008 	bne	68d4 <block_cipher_aes_init+0xe0>
    68b0:	eb001306 	bl	b4d0 <get_ticks>
    68b4:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    68b8:	e3a05000 	mov	r5, #0	; 0x0
    68bc:	e0944000 	adds	r4, r4, r0
    68c0:	e59f608c 	ldr	r6, [pc, #140]	; 6954 <block_cipher_aes_init+0x160>
    68c4:	e0a55001 	adc	r5, r5, r1
    68c8:	e596340c 	ldr	r3, [r6, #1036]
    68cc:	e3130008 	tst	r3, #8	; 0x8
    68d0:	0a000011 	beq	691c <block_cipher_aes_init+0x128>
    68d4:	eb0012fd 	bl	b4d0 <get_ticks>
    68d8:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    68dc:	e3a05000 	mov	r5, #0	; 0x0
    68e0:	e0944000 	adds	r4, r4, r0
    68e4:	e59f6068 	ldr	r6, [pc, #104]	; 6954 <block_cipher_aes_init+0x160>
    68e8:	e0a55001 	adc	r5, r5, r1
    68ec:	e596340c 	ldr	r3, [r6, #1036]
    68f0:	e59f205c 	ldr	r2, [pc, #92]	; 6954 <block_cipher_aes_init+0x160>
    68f4:	e3130010 	tst	r3, #16	; 0x10
    68f8:	0a00000e 	beq	6938 <block_cipher_aes_init+0x144>
    68fc:	e5923400 	ldr	r3, [r2, #1024]
    6900:	e3a00000 	mov	r0, #0	; 0x0
    6904:	e3833001 	orr	r3, r3, #1	; 0x1
    6908:	e5823400 	str	r3, [r2, #1024]
    690c:	ea000000 	b	6914 <block_cipher_aes_init+0x120>
    6910:	e3a00001 	mov	r0, #1	; 0x1
    6914:	e28dd004 	add	sp, sp, #4	; 0x4
    6918:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    691c:	eb0012eb 	bl	b4d0 <get_ticks>
    6920:	e1510005 	cmp	r1, r5
    6924:	8afffff9 	bhi	6910 <block_cipher_aes_init+0x11c>
    6928:	1affffe6 	bne	68c8 <block_cipher_aes_init+0xd4>
    692c:	e1500004 	cmp	r0, r4
    6930:	8afffff6 	bhi	6910 <block_cipher_aes_init+0x11c>
    6934:	eaffffe3 	b	68c8 <block_cipher_aes_init+0xd4>
    6938:	eb0012e4 	bl	b4d0 <get_ticks>
    693c:	e1510005 	cmp	r1, r5
    6940:	8afffff2 	bhi	6910 <block_cipher_aes_init+0x11c>
    6944:	1affffe8 	bne	68ec <block_cipher_aes_init+0xf8>
    6948:	e1500004 	cmp	r0, r4
    694c:	8affffef 	bhi	6910 <block_cipher_aes_init+0x11c>
    6950:	eaffffe5 	b	68ec <block_cipher_aes_init+0xf8>
    6954:	23200000 	.word	0x23200000
    6958:	23200420 	.word	0x23200420
    695c:	23200440 	.word	0x23200440
    6960:	dcdffbe0 	.word	0xdcdffbe0
    6964:	23200450 	.word	0x23200450
    6968:	dcdffbc0 	.word	0xdcdffbc0

0000696c <block_cipher_operation>:
    696c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6970:	e24dd00c 	sub	sp, sp, #12	; 0xc
    6974:	e1a08002 	mov	r8, r2
    6978:	e58d0004 	str	r0, [sp, #4]
    697c:	e1a09001 	mov	r9, r1
    6980:	e1a06002 	mov	r6, r2
    6984:	e59fa0c4 	ldr	sl, [pc, #196]	; 6a50 <block_cipher_operation+0xe4>
    6988:	e3a0b000 	mov	fp, #0	; 0x0
    698c:	ea000028 	b	6a34 <block_cipher_operation+0xc8>
    6990:	e59d3004 	ldr	r3, [sp, #4]
    6994:	e3560902 	cmp	r6, #32768	; 0x8000
    6998:	b1a07006 	movlt	r7, r6
    699c:	a3a07902 	movge	r7, #32768	; 0x8000
    69a0:	e0664008 	rsb	r4, r6, r8
    69a4:	e3a00000 	mov	r0, #0	; 0x0
    69a8:	e0831004 	add	r1, r3, r4
    69ac:	e1a02007 	mov	r2, r7
    69b0:	eb0000ed 	bl	6d6c <dma_sdma_init>
    69b4:	e3500000 	cmp	r0, #0	; 0x0
    69b8:	1a000021 	bne	6a44 <block_cipher_operation+0xd8>
    69bc:	e0891004 	add	r1, r9, r4
    69c0:	e2800001 	add	r0, r0, #1	; 0x1
    69c4:	e1a02007 	mov	r2, r7
    69c8:	eb0000e7 	bl	6d6c <dma_sdma_init>
    69cc:	e3500000 	cmp	r0, #0	; 0x0
    69d0:	1a00001b 	bne	6a44 <block_cipher_operation+0xd8>
    69d4:	eb0000d4 	bl	6d2c <dma_start>
    69d8:	e3a00001 	mov	r0, #1	; 0x1
    69dc:	eb0000d2 	bl	6d2c <dma_start>
    69e0:	eb0012ba 	bl	b4d0 <get_ticks>
    69e4:	e090400a 	adds	r4, r0, sl
    69e8:	e0a1500b 	adc	r5, r1, fp
    69ec:	ea000005 	b	6a08 <block_cipher_operation+0x9c>
    69f0:	eb0012b6 	bl	b4d0 <get_ticks>
    69f4:	e1510005 	cmp	r1, r5
    69f8:	8a000011 	bhi	6a44 <block_cipher_operation+0xd8>
    69fc:	1a000001 	bne	6a08 <block_cipher_operation+0x9c>
    6a00:	e1500004 	cmp	r0, r4
    6a04:	8a00000e 	bhi	6a44 <block_cipher_operation+0xd8>
    6a08:	e3a01003 	mov	r1, #3	; 0x3
    6a0c:	e3a00001 	mov	r0, #1	; 0x1
    6a10:	eb0000cb 	bl	6d44 <dma_get_status>
    6a14:	e3500000 	cmp	r0, #0	; 0x0
    6a18:	0afffff4 	beq	69f0 <block_cipher_operation+0x84>
    6a1c:	e3a00000 	mov	r0, #0	; 0x0
    6a20:	e3a01003 	mov	r1, #3	; 0x3
    6a24:	eb0000c6 	bl	6d44 <dma_get_status>
    6a28:	e3500000 	cmp	r0, #0	; 0x0
    6a2c:	0a000004 	beq	6a44 <block_cipher_operation+0xd8>
    6a30:	e0676006 	rsb	r6, r7, r6
    6a34:	e3560000 	cmp	r6, #0	; 0x0
    6a38:	1affffd4 	bne	6990 <block_cipher_operation+0x24>
    6a3c:	e1a00006 	mov	r0, r6
    6a40:	ea000000 	b	6a48 <block_cipher_operation+0xdc>
    6a44:	e3a00001 	mov	r0, #1	; 0x1
    6a48:	e28dd00c 	add	sp, sp, #12	; 0xc
    6a4c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6a50:	000f4240 	.word	0x000f4240

00006a54 <hash_len>:
    6a54:	e59f300c 	ldr	r3, [pc, #12]	; 6a68 <hash_len+0x14>
    6a58:	e5932000 	ldr	r2, [r3]
    6a5c:	e59f3008 	ldr	r3, [pc, #8]	; 6a6c <hash_len+0x18>
    6a60:	e7930202 	ldr	r0, [r3, r2, lsl #4]
    6a64:	e12fff1e 	bx	lr
    6a68:	3c00d184 	.word	0x3c00d184
    6a6c:	000133f8 	.word	0x000133f8

00006a70 <hash_value>:
    6a70:	e92d4070 	push	{r4, r5, r6, lr}
    6a74:	e59f6054 	ldr	r6, [pc, #84]	; 6ad0 <hash_value+0x60>
    6a78:	e1a05000 	mov	r5, r0
    6a7c:	e5963000 	ldr	r3, [r6]
    6a80:	e3530000 	cmp	r3, #0	; 0x0
    6a84:	0a000009 	beq	6ab0 <hash_value+0x40>
    6a88:	eb0000d9 	bl	6df4 <security_enabled>
    6a8c:	e2504000 	subs	r4, r0, #0	; 0x0
    6a90:	1a000006 	bne	6ab0 <hash_value+0x40>
    6a94:	e59f0038 	ldr	r0, [pc, #56]	; 6ad4 <hash_value+0x64>
    6a98:	e5961000 	ldr	r1, [r6]
    6a9c:	e59f3034 	ldr	r3, [pc, #52]	; 6ad8 <hash_value+0x68>
    6aa0:	e1a0e00f 	mov	lr, pc
    6aa4:	e593f000 	ldr	pc, [r3]
    6aa8:	e1a00004 	mov	r0, r4
    6aac:	e8bd8070 	pop	{r4, r5, r6, pc}
    6ab0:	e59f3018 	ldr	r3, [pc, #24]	; 6ad0 <hash_value+0x60>
    6ab4:	e1a00005 	mov	r0, r5
    6ab8:	e5932000 	ldr	r2, [r3]
    6abc:	e59f3018 	ldr	r3, [pc, #24]	; 6adc <hash_value+0x6c>
    6ac0:	e0833202 	add	r3, r3, r2, lsl #4
    6ac4:	e1a0e00f 	mov	lr, pc
    6ac8:	e593f00c 	ldr	pc, [r3, #12]
    6acc:	e8bd8070 	pop	{r4, r5, r6, pc}
    6ad0:	3c00d184 	.word	0x3c00d184
    6ad4:	0001b886 	.word	0x0001b886
    6ad8:	3c00d694 	.word	0x3c00d694
    6adc:	000133f8 	.word	0x000133f8

00006ae0 <hash_data>:
    6ae0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6ae4:	e59f5074 	ldr	r5, [pc, #116]	; 6b60 <hash_data+0x80>
    6ae8:	e24dd004 	sub	sp, sp, #4	; 0x4
    6aec:	e5953000 	ldr	r3, [r5]
    6af0:	e1a06000 	mov	r6, r0
    6af4:	e3530000 	cmp	r3, #0	; 0x0
    6af8:	e1a07001 	mov	r7, r1
    6afc:	0a000009 	beq	6b28 <hash_data+0x48>
    6b00:	eb0000bb 	bl	6df4 <security_enabled>
    6b04:	e2504000 	subs	r4, r0, #0	; 0x0
    6b08:	1a000006 	bne	6b28 <hash_data+0x48>
    6b0c:	e59f0050 	ldr	r0, [pc, #80]	; 6b64 <hash_data+0x84>
    6b10:	e5951000 	ldr	r1, [r5]
    6b14:	e59f304c 	ldr	r3, [pc, #76]	; 6b68 <hash_data+0x88>
    6b18:	e1a0e00f 	mov	lr, pc
    6b1c:	e593f000 	ldr	pc, [r3]
    6b20:	e1a00004 	mov	r0, r4
    6b24:	ea00000b 	b	6b58 <hash_data+0x78>
    6b28:	e59f003c 	ldr	r0, [pc, #60]	; 6b6c <hash_data+0x8c>
    6b2c:	e59f3034 	ldr	r3, [pc, #52]	; 6b68 <hash_data+0x88>
    6b30:	e1a0e00f 	mov	lr, pc
    6b34:	e593f000 	ldr	pc, [r3]
    6b38:	e59f3020 	ldr	r3, [pc, #32]	; 6b60 <hash_data+0x80>
    6b3c:	e1a00006 	mov	r0, r6
    6b40:	e5932000 	ldr	r2, [r3]
    6b44:	e59f3024 	ldr	r3, [pc, #36]	; 6b70 <hash_data+0x90>
    6b48:	e1a01007 	mov	r1, r7
    6b4c:	e0833202 	add	r3, r3, r2, lsl #4
    6b50:	e1a0e00f 	mov	lr, pc
    6b54:	e593f008 	ldr	pc, [r3, #8]
    6b58:	e28dd004 	add	sp, sp, #4	; 0x4
    6b5c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6b60:	3c00d184 	.word	0x3c00d184
    6b64:	0001b886 	.word	0x0001b886
    6b68:	3c00d694 	.word	0x3c00d694
    6b6c:	0001b8a7 	.word	0x0001b8a7
    6b70:	000133f8 	.word	0x000133f8

00006b74 <hash_init>:
    6b74:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6b78:	e3500003 	cmp	r0, #3	; 0x3
    6b7c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6b80:	e1a04000 	mov	r4, r0
    6b84:	e1a07001 	mov	r7, r1
    6b88:	9a000005 	bls	6ba4 <hash_init+0x30>
    6b8c:	e59f0064 	ldr	r0, [pc, #100]	; 6bf8 <hash_init+0x84>
    6b90:	e59f3064 	ldr	r3, [pc, #100]	; 6bfc <hash_init+0x88>
    6b94:	e1a0e00f 	mov	lr, pc
    6b98:	e593f000 	ldr	pc, [r3]
    6b9c:	e3e00000 	mvn	r0, #0	; 0x0
    6ba0:	ea000012 	b	6bf0 <hash_init+0x7c>
    6ba4:	e59f6054 	ldr	r6, [pc, #84]	; 6c00 <hash_init+0x8c>
    6ba8:	e3500000 	cmp	r0, #0	; 0x0
    6bac:	e5860000 	str	r0, [r6]
    6bb0:	0a000009 	beq	6bdc <hash_init+0x68>
    6bb4:	eb00008e 	bl	6df4 <security_enabled>
    6bb8:	e2505000 	subs	r5, r0, #0	; 0x0
    6bbc:	1a000006 	bne	6bdc <hash_init+0x68>
    6bc0:	e59f003c 	ldr	r0, [pc, #60]	; 6c04 <hash_init+0x90>
    6bc4:	e5961000 	ldr	r1, [r6]
    6bc8:	e59f302c 	ldr	r3, [pc, #44]	; 6bfc <hash_init+0x88>
    6bcc:	e1a0e00f 	mov	lr, pc
    6bd0:	e593f000 	ldr	pc, [r3]
    6bd4:	e1a00005 	mov	r0, r5
    6bd8:	ea000004 	b	6bf0 <hash_init+0x7c>
    6bdc:	e59f3024 	ldr	r3, [pc, #36]	; 6c08 <hash_init+0x94>
    6be0:	e1a00007 	mov	r0, r7
    6be4:	e0833204 	add	r3, r3, r4, lsl #4
    6be8:	e1a0e00f 	mov	lr, pc
    6bec:	e593f004 	ldr	pc, [r3, #4]
    6bf0:	e28dd004 	add	sp, sp, #4	; 0x4
    6bf4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6bf8:	0001b8b2 	.word	0x0001b8b2
    6bfc:	3c00d694 	.word	0x3c00d694
    6c00:	3c00d184 	.word	0x3c00d184
    6c04:	0001b886 	.word	0x0001b886
    6c08:	000133f8 	.word	0x000133f8

00006c0c <md5_value>:
    6c0c:	e92d4030 	push	{r4, r5, lr}
    6c10:	e59f3054 	ldr	r3, [pc, #84]	; 6c6c <md5_value+0x60>
    6c14:	e1a05000 	mov	r5, r0
    6c18:	e893000c 	ldm	r3, {r2, r3}
    6c1c:	e1823003 	orr	r3, r2, r3
    6c20:	e24dd024 	sub	sp, sp, #36	; 0x24
    6c24:	e3530000 	cmp	r3, #0	; 0x0
    6c28:	13e00000 	mvnne	r0, #0	; 0x0
    6c2c:	1a00000c 	bne	6c64 <md5_value+0x58>
    6c30:	e1a0000d 	mov	r0, sp
    6c34:	eb000178 	bl	721c <hash_module_get_result>
    6c38:	e3500000 	cmp	r0, #0	; 0x0
    6c3c:	e1a0400d 	mov	r4, sp
    6c40:	13e00001 	mvnne	r0, #1	; 0x1
    6c44:	1a000006 	bne	6c64 <md5_value+0x58>
    6c48:	e1a0200d 	mov	r2, sp
    6c4c:	e7d23000 	ldrb	r3, [r2, r0]
    6c50:	e7c53000 	strb	r3, [r5, r0]
    6c54:	e2800001 	add	r0, r0, #1	; 0x1
    6c58:	e3500010 	cmp	r0, #16	; 0x10
    6c5c:	1afffffa 	bne	6c4c <md5_value+0x40>
    6c60:	e3a00000 	mov	r0, #0	; 0x0
    6c64:	e28dd024 	add	sp, sp, #36	; 0x24
    6c68:	e8bd8030 	pop	{r4, r5, pc}
    6c6c:	3c00d188 	.word	0x3c00d188

00006c70 <md5_data>:
    6c70:	e59f305c 	ldr	r3, [pc, #92]	; 6cd4 <md5_data+0x64>
    6c74:	e92d4070 	push	{r4, r5, r6, lr}
    6c78:	e1a06001 	mov	r6, r1
    6c7c:	e5931004 	ldr	r1, [r3, #4]
    6c80:	e1a04006 	mov	r4, r6
    6c84:	e3a05000 	mov	r5, #0	; 0x0
    6c88:	e1550001 	cmp	r5, r1
    6c8c:	e593c000 	ldr	ip, [r3]
    6c90:	8a000002 	bhi	6ca0 <md5_data+0x30>
    6c94:	1a000003 	bne	6ca8 <md5_data+0x38>
    6c98:	e154000c 	cmp	r4, ip
    6c9c:	9a000001 	bls	6ca8 <md5_data+0x38>
    6ca0:	e3e00000 	mvn	r0, #0	; 0x0
    6ca4:	e8bd8070 	pop	{r4, r5, r6, pc}
    6ca8:	e1a0200c 	mov	r2, ip
    6cac:	e1a03001 	mov	r3, r1
    6cb0:	e59fc01c 	ldr	ip, [pc, #28]	; 6cd4 <md5_data+0x64>
    6cb4:	e0522004 	subs	r2, r2, r4
    6cb8:	e0c33005 	sbc	r3, r3, r5
    6cbc:	e1a01006 	mov	r1, r6
    6cc0:	e88c000c 	stm	ip, {r2, r3}
    6cc4:	eb0001b7 	bl	73a8 <hash_dma_transfer>
    6cc8:	e3500000 	cmp	r0, #0	; 0x0
    6ccc:	13e00001 	mvnne	r0, #1	; 0x1
    6cd0:	e8bd8070 	pop	{r4, r5, r6, pc}
    6cd4:	3c00d188 	.word	0x3c00d188

00006cd8 <md5_init>:
    6cd8:	e1a03ea0 	lsr	r3, r0, #29
    6cdc:	e1a0c180 	lsl	ip, r0, #3
    6ce0:	e92d4010 	push	{r4, lr}
    6ce4:	e59f403c 	ldr	r4, [pc, #60]	; 6d28 <md5_init+0x50>
    6ce8:	e1a0200c 	mov	r2, ip
    6cec:	e1a01003 	mov	r1, r3
    6cf0:	e3a00000 	mov	r0, #0	; 0x0
    6cf4:	e5843004 	str	r3, [r4, #4]
    6cf8:	e584c000 	str	ip, [r4]
    6cfc:	eb00018d 	bl	7338 <hash_module_init>
    6d00:	e3500000 	cmp	r0, #0	; 0x0
    6d04:	05942000 	ldreq	r2, [r4]
    6d08:	05943004 	ldreq	r3, [r4, #4]
    6d0c:	01a021a2 	lsreq	r2, r2, #3
    6d10:	01822e83 	orreq	r2, r2, r3, lsl #29
    6d14:	01a031a3 	lsreq	r3, r3, #3
    6d18:	13e00000 	mvnne	r0, #0	; 0x0
    6d1c:	05843004 	streq	r3, [r4, #4]
    6d20:	05842000 	streq	r2, [r4]
    6d24:	e8bd8010 	pop	{r4, pc}
    6d28:	3c00d188 	.word	0x3c00d188

00006d2c <dma_start>:
    6d2c:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6d30:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6d34:	e1a00400 	lsl	r0, r0, #8
    6d38:	e3a03001 	mov	r3, #1	; 0x1
    6d3c:	e5803000 	str	r3, [r0]
    6d40:	e12fff1e 	bx	lr

00006d44 <dma_get_status>:
    6d44:	e3a0c001 	mov	ip, #1	; 0x1
    6d48:	e1a0211c 	lsl	r2, ip, r1
    6d4c:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6d50:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6d54:	e1a01400 	lsl	r1, r0, #8
    6d58:	e5913024 	ldr	r3, [r1, #36]
    6d5c:	e0130002 	ands	r0, r3, r2
    6d60:	15812024 	strne	r2, [r1, #36]
    6d64:	11a0000c 	movne	r0, ip
    6d68:	e12fff1e 	bx	lr

00006d6c <dma_sdma_init>:
    6d6c:	e280c823 	add	ip, r0, #2293760	; 0x230000
    6d70:	e28cca02 	add	ip, ip, #8192	; 0x2000
    6d74:	e1a0040c 	lsl	r0, ip, #8
    6d78:	e2800004 	add	r0, r0, #4	; 0x4
    6d7c:	e5903000 	ldr	r3, [r0]
    6d80:	e3520801 	cmp	r2, #65536	; 0x10000
    6d84:	e3833001 	orr	r3, r3, #1	; 0x1
    6d88:	e5803000 	str	r3, [r0]
    6d8c:	e92d4010 	push	{r4, lr}
    6d90:	e3a03341 	mov	r3, #67108865	; 0x4000001
    6d94:	e1a0440c 	lsl	r4, ip, #8
    6d98:	e584300c 	str	r3, [r4, #12]
    6d9c:	31a02802 	lslcc	r2, r2, #16
    6da0:	31a0340c 	lslcc	r3, ip, #8
    6da4:	31a02822 	lsrcc	r2, r2, #16
    6da8:	32833010 	addcc	r3, r3, #16	; 0x10
    6dac:	35832000 	strcc	r2, [r3]
    6db0:	23a00001 	movcs	r0, #1	; 0x1
    6db4:	35841014 	strcc	r1, [r4, #20]
    6db8:	33a00000 	movcc	r0, #0	; 0x0
    6dbc:	e8bd8010 	pop	{r4, pc}

00006dc0 <security_cke_control>:
    6dc0:	e59fc014 	ldr	ip, [pc, #20]	; 6ddc <security_cke_control+0x1c>
    6dc4:	e3a02001 	mov	r2, #1	; 0x1
    6dc8:	e59c3000 	ldr	r3, [ip]
    6dcc:	e1c33012 	bic	r3, r3, r2, lsl r0
    6dd0:	e1833011 	orr	r3, r3, r1, lsl r0
    6dd4:	e58c3000 	str	r3, [ip]
    6dd8:	e12fff1e 	bx	lr
    6ddc:	23202000 	.word	0x23202000

00006de0 <security_enable>:
    6de0:	e59f3008 	ldr	r3, [pc, #8]	; 6df0 <security_enable+0x10>
    6de4:	e3a02001 	mov	r2, #1	; 0x1
    6de8:	e5832000 	str	r2, [r3]
    6dec:	e12fff1e 	bx	lr
    6df0:	3c00d190 	.word	0x3c00d190

00006df4 <security_enabled>:
    6df4:	e59f300c 	ldr	r3, [pc, #12]	; 6e08 <security_enabled+0x14>
    6df8:	e5930000 	ldr	r0, [r3]
    6dfc:	e2500000 	subs	r0, r0, #0	; 0x0
    6e00:	13a00001 	movne	r0, #1	; 0x1
    6e04:	e12fff1e 	bx	lr
    6e08:	3c00d190 	.word	0x3c00d190

00006e0c <dma_reset>:
    6e0c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6e10:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6e14:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6e18:	e1a07400 	lsl	r7, r0, #8
    6e1c:	e3a03002 	mov	r3, #2	; 0x2
    6e20:	e24dd004 	sub	sp, sp, #4	; 0x4
    6e24:	e5873000 	str	r3, [r7]
    6e28:	eb0011a8 	bl	b4d0 <get_ticks>
    6e2c:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    6e30:	e3a05000 	mov	r5, #0	; 0x0
    6e34:	e0944000 	adds	r4, r4, r0
    6e38:	e0a55001 	adc	r5, r5, r1
    6e3c:	e5976000 	ldr	r6, [r7]
    6e40:	ea000008 	b	6e68 <dma_reset+0x5c>
    6e44:	e5976000 	ldr	r6, [r7]
    6e48:	eb0011a0 	bl	b4d0 <get_ticks>
    6e4c:	e1510005 	cmp	r1, r5
    6e50:	8a000002 	bhi	6e60 <dma_reset+0x54>
    6e54:	1a000003 	bne	6e68 <dma_reset+0x5c>
    6e58:	e1500004 	cmp	r0, r4
    6e5c:	9a000001 	bls	6e68 <dma_reset+0x5c>
    6e60:	e3a00001 	mov	r0, #1	; 0x1
    6e64:	ea000002 	b	6e74 <dma_reset+0x68>
    6e68:	e3560000 	cmp	r6, #0	; 0x0
    6e6c:	1afffff4 	bne	6e44 <dma_reset+0x38>
    6e70:	e1a00006 	mov	r0, r6
    6e74:	e28dd004 	add	sp, sp, #4	; 0x4
    6e78:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00006e7c <sha1_value>:
    6e7c:	e92d4030 	push	{r4, r5, lr}
    6e80:	e59f3054 	ldr	r3, [pc, #84]	; 6edc <sha1_value+0x60>
    6e84:	e1a05000 	mov	r5, r0
    6e88:	e893000c 	ldm	r3, {r2, r3}
    6e8c:	e1823003 	orr	r3, r2, r3
    6e90:	e24dd024 	sub	sp, sp, #36	; 0x24
    6e94:	e3530000 	cmp	r3, #0	; 0x0
    6e98:	13e00000 	mvnne	r0, #0	; 0x0
    6e9c:	1a00000c 	bne	6ed4 <sha1_value+0x58>
    6ea0:	e1a0000d 	mov	r0, sp
    6ea4:	eb0000dc 	bl	721c <hash_module_get_result>
    6ea8:	e3500000 	cmp	r0, #0	; 0x0
    6eac:	e1a0400d 	mov	r4, sp
    6eb0:	13e00001 	mvnne	r0, #1	; 0x1
    6eb4:	1a000006 	bne	6ed4 <sha1_value+0x58>
    6eb8:	e1a0200d 	mov	r2, sp
    6ebc:	e7d23000 	ldrb	r3, [r2, r0]
    6ec0:	e7c53000 	strb	r3, [r5, r0]
    6ec4:	e2800001 	add	r0, r0, #1	; 0x1
    6ec8:	e3500014 	cmp	r0, #20	; 0x14
    6ecc:	1afffffa 	bne	6ebc <sha1_value+0x40>
    6ed0:	e3a00000 	mov	r0, #0	; 0x0
    6ed4:	e28dd024 	add	sp, sp, #36	; 0x24
    6ed8:	e8bd8030 	pop	{r4, r5, pc}
    6edc:	3c00d198 	.word	0x3c00d198

00006ee0 <sha1_data>:
    6ee0:	e59f305c 	ldr	r3, [pc, #92]	; 6f44 <sha1_data+0x64>
    6ee4:	e92d4070 	push	{r4, r5, r6, lr}
    6ee8:	e1a06001 	mov	r6, r1
    6eec:	e5931004 	ldr	r1, [r3, #4]
    6ef0:	e1a04006 	mov	r4, r6
    6ef4:	e3a05000 	mov	r5, #0	; 0x0
    6ef8:	e1550001 	cmp	r5, r1
    6efc:	e593c000 	ldr	ip, [r3]
    6f00:	8a000002 	bhi	6f10 <sha1_data+0x30>
    6f04:	1a000003 	bne	6f18 <sha1_data+0x38>
    6f08:	e154000c 	cmp	r4, ip
    6f0c:	9a000001 	bls	6f18 <sha1_data+0x38>
    6f10:	e3e00000 	mvn	r0, #0	; 0x0
    6f14:	e8bd8070 	pop	{r4, r5, r6, pc}
    6f18:	e1a0200c 	mov	r2, ip
    6f1c:	e1a03001 	mov	r3, r1
    6f20:	e59fc01c 	ldr	ip, [pc, #28]	; 6f44 <sha1_data+0x64>
    6f24:	e0522004 	subs	r2, r2, r4
    6f28:	e0c33005 	sbc	r3, r3, r5
    6f2c:	e1a01006 	mov	r1, r6
    6f30:	e88c000c 	stm	ip, {r2, r3}
    6f34:	eb00011b 	bl	73a8 <hash_dma_transfer>
    6f38:	e3500000 	cmp	r0, #0	; 0x0
    6f3c:	13e00001 	mvnne	r0, #1	; 0x1
    6f40:	e8bd8070 	pop	{r4, r5, r6, pc}
    6f44:	3c00d198 	.word	0x3c00d198

00006f48 <sha1_init>:
    6f48:	e1a03ea0 	lsr	r3, r0, #29
    6f4c:	e1a0c180 	lsl	ip, r0, #3
    6f50:	e92d4010 	push	{r4, lr}
    6f54:	e59f403c 	ldr	r4, [pc, #60]	; 6f98 <sha1_init+0x50>
    6f58:	e1a0200c 	mov	r2, ip
    6f5c:	e1a01003 	mov	r1, r3
    6f60:	e3a00001 	mov	r0, #1	; 0x1
    6f64:	e5843004 	str	r3, [r4, #4]
    6f68:	e584c000 	str	ip, [r4]
    6f6c:	eb0000f1 	bl	7338 <hash_module_init>
    6f70:	e3500000 	cmp	r0, #0	; 0x0
    6f74:	05942000 	ldreq	r2, [r4]
    6f78:	05943004 	ldreq	r3, [r4, #4]
    6f7c:	01a021a2 	lsreq	r2, r2, #3
    6f80:	01822e83 	orreq	r2, r2, r3, lsl #29
    6f84:	01a031a3 	lsreq	r3, r3, #3
    6f88:	13e00000 	mvnne	r0, #0	; 0x0
    6f8c:	05843004 	streq	r3, [r4, #4]
    6f90:	05842000 	streq	r2, [r4]
    6f94:	e8bd8010 	pop	{r4, pc}
    6f98:	3c00d198 	.word	0x3c00d198

00006f9c <sha256_value>:
    6f9c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6fa0:	e59f3028 	ldr	r3, [pc, #40]	; 6fd0 <sha256_value+0x34>
    6fa4:	e24dd004 	sub	sp, sp, #4	; 0x4
    6fa8:	e893000c 	ldm	r3, {r2, r3}
    6fac:	e1823003 	orr	r3, r2, r3
    6fb0:	e3530000 	cmp	r3, #0	; 0x0
    6fb4:	13e00000 	mvnne	r0, #0	; 0x0
    6fb8:	1a000002 	bne	6fc8 <sha256_value+0x2c>
    6fbc:	eb000096 	bl	721c <hash_module_get_result>
    6fc0:	e3500000 	cmp	r0, #0	; 0x0
    6fc4:	13e00001 	mvnne	r0, #1	; 0x1
    6fc8:	e28dd004 	add	sp, sp, #4	; 0x4
    6fcc:	e8bd8000 	pop	{pc}
    6fd0:	3c00d1a0 	.word	0x3c00d1a0

00006fd4 <sha256_data>:
    6fd4:	e59f305c 	ldr	r3, [pc, #92]	; 7038 <sha256_data+0x64>
    6fd8:	e92d4070 	push	{r4, r5, r6, lr}
    6fdc:	e1a06001 	mov	r6, r1
    6fe0:	e5931004 	ldr	r1, [r3, #4]
    6fe4:	e1a04006 	mov	r4, r6
    6fe8:	e3a05000 	mov	r5, #0	; 0x0
    6fec:	e1550001 	cmp	r5, r1
    6ff0:	e593c000 	ldr	ip, [r3]
    6ff4:	8a000002 	bhi	7004 <sha256_data+0x30>
    6ff8:	1a000003 	bne	700c <sha256_data+0x38>
    6ffc:	e154000c 	cmp	r4, ip
    7000:	9a000001 	bls	700c <sha256_data+0x38>
    7004:	e3e00000 	mvn	r0, #0	; 0x0
    7008:	e8bd8070 	pop	{r4, r5, r6, pc}
    700c:	e1a0200c 	mov	r2, ip
    7010:	e1a03001 	mov	r3, r1
    7014:	e59fc01c 	ldr	ip, [pc, #28]	; 7038 <sha256_data+0x64>
    7018:	e0522004 	subs	r2, r2, r4
    701c:	e0c33005 	sbc	r3, r3, r5
    7020:	e1a01006 	mov	r1, r6
    7024:	e88c000c 	stm	ip, {r2, r3}
    7028:	eb0000de 	bl	73a8 <hash_dma_transfer>
    702c:	e3500000 	cmp	r0, #0	; 0x0
    7030:	13e00001 	mvnne	r0, #1	; 0x1
    7034:	e8bd8070 	pop	{r4, r5, r6, pc}
    7038:	3c00d1a0 	.word	0x3c00d1a0

0000703c <sha256_init>:
    703c:	e1a03ea0 	lsr	r3, r0, #29
    7040:	e1a0c180 	lsl	ip, r0, #3
    7044:	e92d4010 	push	{r4, lr}
    7048:	e59f403c 	ldr	r4, [pc, #60]	; 708c <sha256_init+0x50>
    704c:	e1a0200c 	mov	r2, ip
    7050:	e1a01003 	mov	r1, r3
    7054:	e3a00003 	mov	r0, #3	; 0x3
    7058:	e5843004 	str	r3, [r4, #4]
    705c:	e584c000 	str	ip, [r4]
    7060:	eb0000b4 	bl	7338 <hash_module_init>
    7064:	e3500000 	cmp	r0, #0	; 0x0
    7068:	05942000 	ldreq	r2, [r4]
    706c:	05943004 	ldreq	r3, [r4, #4]
    7070:	01a021a2 	lsreq	r2, r2, #3
    7074:	01822e83 	orreq	r2, r2, r3, lsl #29
    7078:	01a031a3 	lsreq	r3, r3, #3
    707c:	13e00000 	mvnne	r0, #0	; 0x0
    7080:	05843004 	streq	r3, [r4, #4]
    7084:	05842000 	streq	r2, [r4]
    7088:	e8bd8010 	pop	{r4, pc}
    708c:	3c00d1a0 	.word	0x3c00d1a0

00007090 <crc32_init>:
    7090:	e59f300c 	ldr	r3, [pc, #12]	; 70a4 <crc32_init+0x14>
    7094:	e3e02000 	mvn	r2, #0	; 0x0
    7098:	e5832000 	str	r2, [r3]
    709c:	e3a00000 	mov	r0, #0	; 0x0
    70a0:	e12fff1e 	bx	lr
    70a4:	3c00d1a8 	.word	0x3c00d1a8

000070a8 <crc32_data>:
    70a8:	e59f311c 	ldr	r3, [pc, #284]	; 71cc <crc32_data+0x124>
    70ac:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    70b0:	e593c000 	ldr	ip, [r3]
    70b4:	e1a08000 	mov	r8, r0
    70b8:	e1a06001 	mov	r6, r1
    70bc:	e1a04000 	mov	r4, r0
    70c0:	e1a07001 	mov	r7, r1
    70c4:	e59f5104 	ldr	r5, [pc, #260]	; 71d0 <crc32_data+0x128>
    70c8:	ea000027 	b	716c <crc32_data+0xc4>
    70cc:	e5543008 	ldrb	r3, [r4, #-8]
    70d0:	e5542007 	ldrb	r2, [r4, #-7]
    70d4:	e02c3003 	eor	r3, ip, r3
    70d8:	e20330ff 	and	r3, r3, #255	; 0xff
    70dc:	e7953103 	ldr	r3, [r5, r3, lsl #2]
    70e0:	e5541006 	ldrb	r1, [r4, #-6]
    70e4:	e023342c 	eor	r3, r3, ip, lsr #8
    70e8:	e0232002 	eor	r2, r3, r2
    70ec:	e20220ff 	and	r2, r2, #255	; 0xff
    70f0:	e7952102 	ldr	r2, [r5, r2, lsl #2]
    70f4:	e5540005 	ldrb	r0, [r4, #-5]
    70f8:	e0222423 	eor	r2, r2, r3, lsr #8
    70fc:	e0221001 	eor	r1, r2, r1
    7100:	e20110ff 	and	r1, r1, #255	; 0xff
    7104:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    7108:	e5541004 	ldrb	r1, [r4, #-4]
    710c:	e0233422 	eor	r3, r3, r2, lsr #8
    7110:	e0230000 	eor	r0, r3, r0
    7114:	e20000ff 	and	r0, r0, #255	; 0xff
    7118:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    711c:	e5540003 	ldrb	r0, [r4, #-3]
    7120:	e0222423 	eor	r2, r2, r3, lsr #8
    7124:	e0221001 	eor	r1, r2, r1
    7128:	e20110ff 	and	r1, r1, #255	; 0xff
    712c:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    7130:	e554c002 	ldrb	ip, [r4, #-2]
    7134:	e0233422 	eor	r3, r3, r2, lsr #8
    7138:	e0230000 	eor	r0, r3, r0
    713c:	e20000ff 	and	r0, r0, #255	; 0xff
    7140:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    7144:	e5541001 	ldrb	r1, [r4, #-1]
    7148:	e0222423 	eor	r2, r2, r3, lsr #8
    714c:	e022c00c 	eor	ip, r2, ip
    7150:	e20cc0ff 	and	ip, ip, #255	; 0xff
    7154:	e795310c 	ldr	r3, [r5, ip, lsl #2]
    7158:	e0233422 	eor	r3, r3, r2, lsr #8
    715c:	e0231001 	eor	r1, r3, r1
    7160:	e20110ff 	and	r1, r1, #255	; 0xff
    7164:	e7952101 	ldr	r2, [r5, r1, lsl #2]
    7168:	e022c423 	eor	ip, r2, r3, lsr #8
    716c:	e3570007 	cmp	r7, #7	; 0x7
    7170:	e2844008 	add	r4, r4, #8	; 0x8
    7174:	e2477008 	sub	r7, r7, #8	; 0x8
    7178:	8affffd3 	bhi	70cc <crc32_data+0x24>
    717c:	e3c63007 	bic	r3, r6, #7	; 0x7
    7180:	e0880003 	add	r0, r8, r3
    7184:	e59f3040 	ldr	r3, [pc, #64]	; 71cc <crc32_data+0x124>
    7188:	e2161007 	ands	r1, r6, #7	; 0x7
    718c:	e583c000 	str	ip, [r3]
    7190:	0a00000b 	beq	71c4 <crc32_data+0x11c>
    7194:	e59f4034 	ldr	r4, [pc, #52]	; 71d0 <crc32_data+0x128>
    7198:	e3a02000 	mov	r2, #0	; 0x0
    719c:	e7d03002 	ldrb	r3, [r0, r2]
    71a0:	e2822001 	add	r2, r2, #1	; 0x1
    71a4:	e02c3003 	eor	r3, ip, r3
    71a8:	e20330ff 	and	r3, r3, #255	; 0xff
    71ac:	e7943103 	ldr	r3, [r4, r3, lsl #2]
    71b0:	e1510002 	cmp	r1, r2
    71b4:	e023c42c 	eor	ip, r3, ip, lsr #8
    71b8:	1afffff7 	bne	719c <crc32_data+0xf4>
    71bc:	e59f3008 	ldr	r3, [pc, #8]	; 71cc <crc32_data+0x124>
    71c0:	e583c000 	str	ip, [r3]
    71c4:	e3a00000 	mov	r0, #0	; 0x0
    71c8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    71cc:	3c00d1a8 	.word	0x3c00d1a8
    71d0:	00013438 	.word	0x00013438

000071d4 <crc32_value>:
    71d4:	e59f3010 	ldr	r3, [pc, #16]	; 71ec <crc32_value+0x18>
    71d8:	e5933000 	ldr	r3, [r3]
    71dc:	e1e03003 	mvn	r3, r3
    71e0:	e5803000 	str	r3, [r0]
    71e4:	e3a00000 	mov	r0, #0	; 0x0
    71e8:	e12fff1e 	bx	lr
    71ec:	3c00d1a8 	.word	0x3c00d1a8

000071f0 <crc32_le>:
    71f0:	e92d4010 	push	{r4, lr}
    71f4:	e59f401c 	ldr	r4, [pc, #28]	; 7218 <crc32_le+0x28>
    71f8:	e3e03000 	mvn	r3, #0	; 0x0
    71fc:	e1a00001 	mov	r0, r1
    7200:	e1a01002 	mov	r1, r2
    7204:	e5843000 	str	r3, [r4]
    7208:	ebffffa6 	bl	70a8 <crc32_data>
    720c:	e5940000 	ldr	r0, [r4]
    7210:	e1e00000 	mvn	r0, r0
    7214:	e8bd8010 	pop	{r4, pc}
    7218:	3c00d1a8 	.word	0x3c00d1a8

0000721c <hash_module_get_result>:
    721c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7220:	e1a08000 	mov	r8, r0
    7224:	eb0010a9 	bl	b4d0 <get_ticks>
    7228:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    722c:	e59f3088 	ldr	r3, [pc, #136]	; 72bc <hash_module_get_result+0xa0>
    7230:	e3a05000 	mov	r5, #0	; 0x0
    7234:	e0944000 	adds	r4, r4, r0
    7238:	e0a55001 	adc	r5, r5, r1
    723c:	e1a07003 	mov	r7, r3
    7240:	e593680c 	ldr	r6, [r3, #2060]
    7244:	ea000008 	b	726c <hash_module_get_result+0x50>
    7248:	e597680c 	ldr	r6, [r7, #2060]
    724c:	eb00109f 	bl	b4d0 <get_ticks>
    7250:	e1510005 	cmp	r1, r5
    7254:	8a000002 	bhi	7264 <hash_module_get_result+0x48>
    7258:	1a000003 	bne	726c <hash_module_get_result+0x50>
    725c:	e1500004 	cmp	r0, r4
    7260:	9a000001 	bls	726c <hash_module_get_result+0x50>
    7264:	e3a00001 	mov	r0, #1	; 0x1
    7268:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    726c:	e3160001 	tst	r6, #1	; 0x1
    7270:	0afffff4 	beq	7248 <hash_module_get_result+0x2c>
    7274:	e59f0044 	ldr	r0, [pc, #68]	; 72c0 <hash_module_get_result+0xa4>
    7278:	e59fc044 	ldr	ip, [pc, #68]	; 72c4 <hash_module_get_result+0xa8>
    727c:	e1a03000 	mov	r3, r0
    7280:	e6931008 	ldr	r1, [r3], r8
    7284:	e59f203c 	ldr	r2, [pc, #60]	; 72c8 <hash_module_get_result+0xac>
    7288:	e2800004 	add	r0, r0, #4	; 0x4
    728c:	e0832002 	add	r2, r3, r2
    7290:	e150000c 	cmp	r0, ip
    7294:	e5821000 	str	r1, [r2]
    7298:	1afffff7 	bne	727c <hash_module_get_result+0x60>
    729c:	e59f2018 	ldr	r2, [pc, #24]	; 72bc <hash_module_get_result+0xa0>
    72a0:	e3a03001 	mov	r3, #1	; 0x1
    72a4:	e582380c 	str	r3, [r2, #2060]
    72a8:	e5923800 	ldr	r3, [r2, #2048]
    72ac:	e3a00000 	mov	r0, #0	; 0x0
    72b0:	e3c33080 	bic	r3, r3, #128	; 0x80
    72b4:	e5823800 	str	r3, [r2, #2048]
    72b8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    72bc:	23200000 	.word	0x23200000
    72c0:	23200858 	.word	0x23200858
    72c4:	23200878 	.word	0x23200878
    72c8:	dcdff7a8 	.word	0xdcdff7a8

000072cc <hash_module_clear>:
    72cc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    72d0:	e59f405c 	ldr	r4, [pc, #92]	; 7334 <hash_module_clear+0x68>
    72d4:	e3a03002 	mov	r3, #2	; 0x2
    72d8:	e5843800 	str	r3, [r4, #2048]
    72dc:	e24dd004 	sub	sp, sp, #4	; 0x4
    72e0:	eb00107a 	bl	b4d0 <get_ticks>
    72e4:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    72e8:	e3a07000 	mov	r7, #0	; 0x0
    72ec:	e0966000 	adds	r6, r6, r0
    72f0:	e1a05004 	mov	r5, r4
    72f4:	e0a77001 	adc	r7, r7, r1
    72f8:	e5944800 	ldr	r4, [r4, #2048]
    72fc:	ea000008 	b	7324 <hash_module_clear+0x58>
    7300:	e5954800 	ldr	r4, [r5, #2048]
    7304:	eb001071 	bl	b4d0 <get_ticks>
    7308:	e1510007 	cmp	r1, r7
    730c:	8a000002 	bhi	731c <hash_module_clear+0x50>
    7310:	1a000003 	bne	7324 <hash_module_clear+0x58>
    7314:	e1500006 	cmp	r0, r6
    7318:	9a000001 	bls	7324 <hash_module_clear+0x58>
    731c:	e3a00001 	mov	r0, #1	; 0x1
    7320:	ea000001 	b	732c <hash_module_clear+0x60>
    7324:	e2140002 	ands	r0, r4, #2	; 0x2
    7328:	1afffff4 	bne	7300 <hash_module_clear+0x34>
    732c:	e28dd004 	add	sp, sp, #4	; 0x4
    7330:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    7334:	23200000 	.word	0x23200000

00007338 <hash_module_init>:
    7338:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    733c:	e59f4060 	ldr	r4, [pc, #96]	; 73a4 <hash_module_init+0x6c>
    7340:	e24dd004 	sub	sp, sp, #4	; 0x4
    7344:	e5943800 	ldr	r3, [r4, #2048]
    7348:	e1a05000 	mov	r5, r0
    734c:	e3c33080 	bic	r3, r3, #128	; 0x80
    7350:	e5843800 	str	r3, [r4, #2048]
    7354:	e1a07001 	mov	r7, r1
    7358:	e1a06002 	mov	r6, r2
    735c:	ebffffda 	bl	72cc <hash_module_clear>
    7360:	e3500000 	cmp	r0, #0	; 0x0
    7364:	13a00001 	movne	r0, #1	; 0x1
    7368:	1a00000b 	bne	739c <hash_module_init+0x64>
    736c:	e3a03001 	mov	r3, #1	; 0x1
    7370:	e1a02185 	lsl	r2, r5, #3
    7374:	e5843804 	str	r3, [r4, #2052]
    7378:	e5842800 	str	r2, [r4, #2048]
    737c:	e5847814 	str	r7, [r4, #2068]
    7380:	e5846810 	str	r6, [r4, #2064]
    7384:	e5943800 	ldr	r3, [r4, #2048]
    7388:	e3833080 	orr	r3, r3, #128	; 0x80
    738c:	e5843800 	str	r3, [r4, #2048]
    7390:	e5943800 	ldr	r3, [r4, #2048]
    7394:	e3833004 	orr	r3, r3, #4	; 0x4
    7398:	e5843800 	str	r3, [r4, #2048]
    739c:	e28dd004 	add	sp, sp, #4	; 0x4
    73a0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    73a4:	23200000 	.word	0x23200000

000073a8 <hash_dma_transfer>:
    73a8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    73ac:	e1a08001 	mov	r8, r1
    73b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    73b4:	e1a09000 	mov	r9, r0
    73b8:	e1a06001 	mov	r6, r1
    73bc:	e59fa090 	ldr	sl, [pc, #144]	; 7454 <hash_dma_transfer+0xac>
    73c0:	e3a0b000 	mov	fp, #0	; 0x0
    73c4:	ea00001b 	b	7438 <hash_dma_transfer+0x90>
    73c8:	e3560902 	cmp	r6, #32768	; 0x8000
    73cc:	b1a07006 	movlt	r7, r6
    73d0:	a3a07902 	movge	r7, #32768	; 0x8000
    73d4:	e0661008 	rsb	r1, r6, r8
    73d8:	e0891001 	add	r1, r9, r1
    73dc:	e3a00002 	mov	r0, #2	; 0x2
    73e0:	e1a02007 	mov	r2, r7
    73e4:	ebfffe60 	bl	6d6c <dma_sdma_init>
    73e8:	e3500000 	cmp	r0, #0	; 0x0
    73ec:	1a000015 	bne	7448 <hash_dma_transfer+0xa0>
    73f0:	e2800002 	add	r0, r0, #2	; 0x2
    73f4:	ebfffe4c 	bl	6d2c <dma_start>
    73f8:	eb001034 	bl	b4d0 <get_ticks>
    73fc:	e090400a 	adds	r4, r0, sl
    7400:	e0a1500b 	adc	r5, r1, fp
    7404:	ea000005 	b	7420 <hash_dma_transfer+0x78>
    7408:	eb001030 	bl	b4d0 <get_ticks>
    740c:	e1510005 	cmp	r1, r5
    7410:	8a00000c 	bhi	7448 <hash_dma_transfer+0xa0>
    7414:	1a000001 	bne	7420 <hash_dma_transfer+0x78>
    7418:	e1500004 	cmp	r0, r4
    741c:	8a000009 	bhi	7448 <hash_dma_transfer+0xa0>
    7420:	e3a01003 	mov	r1, #3	; 0x3
    7424:	e3a00002 	mov	r0, #2	; 0x2
    7428:	ebfffe45 	bl	6d44 <dma_get_status>
    742c:	e3500000 	cmp	r0, #0	; 0x0
    7430:	0afffff4 	beq	7408 <hash_dma_transfer+0x60>
    7434:	e0676006 	rsb	r6, r7, r6
    7438:	e3560000 	cmp	r6, #0	; 0x0
    743c:	1affffe1 	bne	73c8 <hash_dma_transfer+0x20>
    7440:	e1a00006 	mov	r0, r6
    7444:	ea000000 	b	744c <hash_dma_transfer+0xa4>
    7448:	e3a00001 	mov	r0, #1	; 0x1
    744c:	e28dd004 	add	sp, sp, #4	; 0x4
    7450:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7454:	000f4240 	.word	0x000f4240

00007458 <cdbg_log_toggle>:
    7458:	e59f3014 	ldr	r3, [pc, #20]	; 7474 <cdbg_log_toggle+0x1c>
    745c:	e3500000 	cmp	r0, #0	; 0x0
    7460:	e59f2010 	ldr	r2, [pc, #16]	; 7478 <cdbg_log_toggle+0x20>
    7464:	01a02003 	moveq	r2, r3
    7468:	e59f300c 	ldr	r3, [pc, #12]	; 747c <cdbg_log_toggle+0x24>
    746c:	e5832000 	str	r2, [r3]
    7470:	e12fff1e 	bx	lr
    7474:	00010270 	.word	0x00010270
    7478:	000102f4 	.word	0x000102f4
    747c:	3c00d694 	.word	0x3c00d694

00007480 <cdbg_wipe_bootarea>:
    7480:	e59f3018 	ldr	r3, [pc, #24]	; 74a0 <cdbg_wipe_bootarea+0x20>
    7484:	e59f1018 	ldr	r1, [pc, #24]	; 74a4 <cdbg_wipe_bootarea+0x24>
    7488:	e59f2018 	ldr	r2, [pc, #24]	; 74a8 <cdbg_wipe_bootarea+0x28>
    748c:	e5031004 	str	r1, [r3, #-4]
    7490:	e2833004 	add	r3, r3, #4	; 0x4
    7494:	e1530002 	cmp	r3, r2
    7498:	1afffffb 	bne	748c <cdbg_wipe_bootarea+0xc>
    749c:	e12fff1e 	bx	lr
    74a0:	3c000004 	.word	0x3c000004
    74a4:	eafffffe 	.word	0xeafffffe
    74a8:	3c008004 	.word	0x3c008004

000074ac <cdbg_verify_burn>:
    74ac:	e59f3004 	ldr	r3, [pc, #4]	; 74b8 <cdbg_verify_burn+0xc>
    74b0:	e5930000 	ldr	r0, [r3]
    74b4:	e12fff1e 	bx	lr
    74b8:	3c00d1ac 	.word	0x3c00d1ac

000074bc <cdbg_verify_burn_key>:
    74bc:	e59f0000 	ldr	r0, [pc, #0]	; 74c4 <cdbg_verify_burn_key+0x8>
    74c0:	e12fff1e 	bx	lr
    74c4:	3c00d1b0 	.word	0x3c00d1b0

000074c8 <cdbg_verify_burn_enable>:
    74c8:	e3500000 	cmp	r0, #0	; 0x0
    74cc:	e59f3018 	ldr	r3, [pc, #24]	; 74ec <cdbg_verify_burn_enable+0x24>
    74d0:	03a02000 	moveq	r2, #0	; 0x0
    74d4:	13a02001 	movne	r2, #1	; 0x1
    74d8:	e5832000 	str	r2, [r3]
    74dc:	012fff1e 	bxeq	lr
    74e0:	e59f0008 	ldr	r0, [pc, #8]	; 74f0 <cdbg_verify_burn_enable+0x28>
    74e4:	e3a02020 	mov	r2, #32	; 0x20
    74e8:	eaffe840 	b	15f0 <memcpy>
    74ec:	3c00d1ac 	.word	0x3c00d1ac
    74f0:	3c00d1b0 	.word	0x3c00d1b0

000074f4 <cdbg_jump>:
    74f4:	e92d4030 	push	{r4, r5, lr}
    74f8:	e1a04000 	mov	r4, r0
    74fc:	e24dd004 	sub	sp, sp, #4	; 0x4
    7500:	e1a05001 	mov	r5, r1
    7504:	ebfffc61 	bl	6690 <ss_lock_isk>
    7508:	e59f001c 	ldr	r0, [pc, #28]	; 752c <cdbg_jump+0x38>
    750c:	e1a01004 	mov	r1, r4
    7510:	e59f3018 	ldr	r3, [pc, #24]	; 7530 <cdbg_jump+0x3c>
    7514:	e1a0e00f 	mov	lr, pc
    7518:	e593f000 	ldr	pc, [r3]
    751c:	e3000bb8 	movw	r0, #3000	; 0xbb8
    7520:	eb000ffb 	bl	b514 <udelay>
    7524:	e1a00005 	mov	r0, r5
    7528:	e12fff34 	blx	r4
    752c:	0001b8f3 	.word	0x0001b8f3
    7530:	3c00d694 	.word	0x3c00d694

00007534 <cdbg_restore_os>:
    7534:	e92d4010 	push	{r4, lr}
    7538:	ebffe4c7 	bl	85c <get_wakeup_entry>
    753c:	e59f301c 	ldr	r3, [pc, #28]	; 7560 <cdbg_restore_os+0x2c>
    7540:	e1a04000 	mov	r4, r0
    7544:	e59f0018 	ldr	r0, [pc, #24]	; 7564 <cdbg_restore_os+0x30>
    7548:	e1a0e00f 	mov	lr, pc
    754c:	e593f000 	ldr	pc, [r3]
    7550:	e1a00004 	mov	r0, r4
    7554:	e3a01000 	mov	r1, #0	; 0x0
    7558:	e8bd4010 	pop	{r4, lr}
    755c:	eaffffe4 	b	74f4 <cdbg_jump>
    7560:	3c00d694 	.word	0x3c00d694
    7564:	0001b903 	.word	0x0001b903

00007568 <cdbg_get_pp>:
    7568:	e92d4070 	push	{r4, r5, r6, lr}
    756c:	e24dd008 	sub	sp, sp, #8	; 0x8
    7570:	e1a0000d 	mov	r0, sp
    7574:	e59f103c 	ldr	r1, [pc, #60]	; 75b8 <cdbg_get_pp+0x50>
    7578:	e3a02008 	mov	r2, #8	; 0x8
    757c:	ebffe81b 	bl	15f0 <memcpy>
    7580:	e1a0400d 	mov	r4, sp
    7584:	e1a0600d 	mov	r6, sp
    7588:	ea000004 	b	75a0 <cdbg_get_pp+0x38>
    758c:	eb002376 	bl	1036c <getc>
    7590:	e1500004 	cmp	r0, r4
    7594:	e2854001 	add	r4, r5, #1	; 0x1
    7598:	e1a05006 	mov	r5, r6
    759c:	1a000000 	bne	75a4 <cdbg_get_pp+0x3c>
    75a0:	e1a05004 	mov	r5, r4
    75a4:	e5d54000 	ldrb	r4, [r5]
    75a8:	e3540000 	cmp	r4, #0	; 0x0
    75ac:	1afffff6 	bne	758c <cdbg_get_pp+0x24>
    75b0:	e28dd008 	add	sp, sp, #8	; 0x8
    75b4:	e8bd8070 	pop	{r4, r5, r6, pc}
    75b8:	00019bfa 	.word	0x00019bfa

000075bc <cdbg_shell>:
    75bc:	e92d4010 	push	{r4, lr}
    75c0:	ebfffc32 	bl	6690 <ss_lock_isk>
    75c4:	e59f3048 	ldr	r3, [pc, #72]	; 7614 <cdbg_shell+0x58>
    75c8:	e59f1048 	ldr	r1, [pc, #72]	; 7618 <cdbg_shell+0x5c>
    75cc:	e59f2048 	ldr	r2, [pc, #72]	; 761c <cdbg_shell+0x60>
    75d0:	e5031004 	str	r1, [r3, #-4]
    75d4:	e2833004 	add	r3, r3, #4	; 0x4
    75d8:	e1530002 	cmp	r3, r2
    75dc:	1afffffb 	bne	75d0 <cdbg_shell+0x14>
    75e0:	e3a00001 	mov	r0, #1	; 0x1
    75e4:	eb002b3a 	bl	122d4 <ss_jtag_en>
    75e8:	e59f4030 	ldr	r4, [pc, #48]	; 7620 <cdbg_shell+0x64>
    75ec:	e59f3030 	ldr	r3, [pc, #48]	; 7624 <cdbg_shell+0x68>
    75f0:	e59f0030 	ldr	r0, [pc, #48]	; 7628 <cdbg_shell+0x6c>
    75f4:	e5843000 	str	r3, [r4]
    75f8:	eb00233d 	bl	102f4 <printf_on>
    75fc:	ebffffd9 	bl	7568 <cdbg_get_pp>
    7600:	e59f0024 	ldr	r0, [pc, #36]	; 762c <cdbg_shell+0x70>
    7604:	e1a0e00f 	mov	lr, pc
    7608:	e594f000 	ldr	pc, [r4]
    760c:	eb002641 	bl	10f18 <main_loop>
    7610:	eafffffd 	b	760c <cdbg_shell+0x50>
    7614:	3c000004 	.word	0x3c000004
    7618:	eafffffe 	.word	0xeafffffe
    761c:	3c008004 	.word	0x3c008004
    7620:	3c00d694 	.word	0x3c00d694
    7624:	000102f4 	.word	0x000102f4
    7628:	0001b912 	.word	0x0001b912
    762c:	0001b917 	.word	0x0001b917

00007630 <cdbg_do_config>:
    7630:	e92d4070 	push	{r4, r5, r6, lr}
    7634:	e59f30ec 	ldr	r3, [pc, #236]	; 7728 <cdbg_do_config+0xf8>
    7638:	e1a04000 	mov	r4, r0
    763c:	e1a06001 	mov	r6, r1
    7640:	e59f00e4 	ldr	r0, [pc, #228]	; 772c <cdbg_do_config+0xfc>
    7644:	e59f10e4 	ldr	r1, [pc, #228]	; 7730 <cdbg_do_config+0x100>
    7648:	e1a0e00f 	mov	lr, pc
    764c:	e593f000 	ldr	pc, [r3]
    7650:	e3a02000 	mov	r2, #0	; 0x0
    7654:	e59f00d4 	ldr	r0, [pc, #212]	; 7730 <cdbg_do_config+0x100>
    7658:	ea000006 	b	7678 <cdbg_do_config+0x48>
    765c:	e7943002 	ldr	r3, [r4, r2]
    7660:	e2822004 	add	r2, r2, #4	; 0x4
    7664:	e1530000 	cmp	r3, r0
    7668:	1a000002 	bne	7678 <cdbg_do_config+0x48>
    766c:	e3510000 	cmp	r1, #0	; 0x0
    7670:	1a000004 	bne	7688 <cdbg_do_config+0x58>
    7674:	ea000007 	b	7698 <cdbg_do_config+0x68>
    7678:	e1520006 	cmp	r2, r6
    767c:	e0841002 	add	r1, r4, r2
    7680:	bafffff5 	blt	765c <cdbg_do_config+0x2c>
    7684:	ea000003 	b	7698 <cdbg_do_config+0x68>
    7688:	e5910004 	ldr	r0, [r1, #4]
    768c:	eb001210 	bl	bed4 <spi_set_bytes>
    7690:	e3a05000 	mov	r5, #0	; 0x0
    7694:	ea000004 	b	76ac <cdbg_do_config+0x7c>
    7698:	e59f0094 	ldr	r0, [pc, #148]	; 7734 <cdbg_do_config+0x104>
    769c:	e59f3084 	ldr	r3, [pc, #132]	; 7728 <cdbg_do_config+0xf8>
    76a0:	e1a0e00f 	mov	lr, pc
    76a4:	e593f000 	ldr	pc, [r3]
    76a8:	e3e05000 	mvn	r5, #0	; 0x0
    76ac:	e59f1084 	ldr	r1, [pc, #132]	; 7738 <cdbg_do_config+0x108>
    76b0:	e59f0074 	ldr	r0, [pc, #116]	; 772c <cdbg_do_config+0xfc>
    76b4:	e59f306c 	ldr	r3, [pc, #108]	; 7728 <cdbg_do_config+0xf8>
    76b8:	e1a0e00f 	mov	lr, pc
    76bc:	e593f000 	ldr	pc, [r3]
    76c0:	e3a02000 	mov	r2, #0	; 0x0
    76c4:	e59f106c 	ldr	r1, [pc, #108]	; 7738 <cdbg_do_config+0x108>
    76c8:	ea000006 	b	76e8 <cdbg_do_config+0xb8>
    76cc:	e7943002 	ldr	r3, [r4, r2]
    76d0:	e2822004 	add	r2, r2, #4	; 0x4
    76d4:	e1530001 	cmp	r3, r1
    76d8:	1a000002 	bne	76e8 <cdbg_do_config+0xb8>
    76dc:	e3500000 	cmp	r0, #0	; 0x0
    76e0:	1a000004 	bne	76f8 <cdbg_do_config+0xc8>
    76e4:	ea000008 	b	770c <cdbg_do_config+0xdc>
    76e8:	e1520006 	cmp	r2, r6
    76ec:	e0840002 	add	r0, r4, r2
    76f0:	bafffff5 	blt	76cc <cdbg_do_config+0x9c>
    76f4:	ea000004 	b	770c <cdbg_do_config+0xdc>
    76f8:	eb000d43 	bl	ac0c <nand_rescan_config>
    76fc:	e3500000 	cmp	r0, #0	; 0x0
    7700:	01a03000 	moveq	r3, r0
    7704:	13e03000 	mvnne	r3, #0	; 0x0
    7708:	ea000004 	b	7720 <cdbg_do_config+0xf0>
    770c:	e59f3014 	ldr	r3, [pc, #20]	; 7728 <cdbg_do_config+0xf8>
    7710:	e59f0024 	ldr	r0, [pc, #36]	; 773c <cdbg_do_config+0x10c>
    7714:	e1a0e00f 	mov	lr, pc
    7718:	e593f000 	ldr	pc, [r3]
    771c:	e3e03000 	mvn	r3, #0	; 0x0
    7720:	e0000395 	mul	r0, r5, r3
    7724:	e8bd8070 	pop	{r4, r5, r6, pc}
    7728:	3c00d694 	.word	0x3c00d694
    772c:	0001b928 	.word	0x0001b928
    7730:	4a82467c 	.word	0x4a82467c
    7734:	0001b945 	.word	0x0001b945
    7738:	aca4570e 	.word	0xaca4570e
    773c:	0001b964 	.word	0x0001b964

00007740 <cdbg_config_isi>:
    7740:	e92d4030 	push	{r4, r5, lr}
    7744:	e24dd004 	sub	sp, sp, #4	; 0x4
    7748:	e1a04000 	mov	r4, r0
    774c:	ebffe96a 	bl	1cfc <isi_check>
    7750:	e2505000 	subs	r5, r0, #0	; 0x0
    7754:	0a000007 	beq	7778 <cdbg_config_isi+0x38>
    7758:	e59f002c 	ldr	r0, [pc, #44]	; 778c <cdbg_config_isi+0x4c>
    775c:	e1a01005 	mov	r1, r5
    7760:	e59f3028 	ldr	r3, [pc, #40]	; 7790 <cdbg_config_isi+0x50>
    7764:	e1a0e00f 	mov	lr, pc
    7768:	e593f000 	ldr	pc, [r3]
    776c:	e1a00005 	mov	r0, r5
    7770:	e28dd004 	add	sp, sp, #4	; 0x4
    7774:	e8bd8030 	pop	{r4, r5, pc}
    7778:	e594100c 	ldr	r1, [r4, #12]
    777c:	e2840c02 	add	r0, r4, #512	; 0x200
    7780:	e28dd004 	add	sp, sp, #4	; 0x4
    7784:	e8bd4030 	pop	{r4, r5, lr}
    7788:	eaffffa8 	b	7630 <cdbg_do_config>
    778c:	0001b984 	.word	0x0001b984
    7790:	3c00d694 	.word	0x3c00d694

00007794 <burn_image_loop>:
    7794:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7798:	e24dd04c 	sub	sp, sp, #76	; 0x4c
    779c:	e59db074 	ldr	fp, [sp, #116]
    77a0:	e1a06000 	mov	r6, r0
    77a4:	e35b0000 	cmp	fp, #0	; 0x0
    77a8:	e58d102c 	str	r1, [sp, #44]
    77ac:	e58d2028 	str	r2, [sp, #40]
    77b0:	e58d3024 	str	r3, [sp, #36]
    77b4:	e59da070 	ldr	sl, [sp, #112]
    77b8:	1a00000c 	bne	77f0 <burn_image_loop+0x5c>
    77bc:	e59f449c 	ldr	r4, [pc, #1180]	; 7c60 <burn_image_loop+0x4cc>
    77c0:	e59f049c 	ldr	r0, [pc, #1180]	; 7c64 <burn_image_loop+0x4d0>
    77c4:	e3a0102e 	mov	r1, #46	; 0x2e
    77c8:	e1a0e00f 	mov	lr, pc
    77cc:	e594f000 	ldr	pc, [r4]
    77d0:	e59f0490 	ldr	r0, [pc, #1168]	; 7c68 <burn_image_loop+0x4d4>
    77d4:	e59f1490 	ldr	r1, [pc, #1168]	; 7c6c <burn_image_loop+0x4d8>
    77d8:	e1a0e00f 	mov	lr, pc
    77dc:	e594f000 	ldr	pc, [r4]
    77e0:	e59f0488 	ldr	r0, [pc, #1160]	; 7c70 <burn_image_loop+0x4dc>
    77e4:	e1a0e00f 	mov	lr, pc
    77e8:	e594f000 	ldr	pc, [r4]
    77ec:	eafffffe 	b	77ec <burn_image_loop+0x58>
    77f0:	e59d002c 	ldr	r0, [sp, #44]
    77f4:	e59d1024 	ldr	r1, [sp, #36]
    77f8:	e1500001 	cmp	r0, r1
    77fc:	1a000005 	bne	7818 <burn_image_loop+0x84>
    7800:	e59f046c 	ldr	r0, [pc, #1132]	; 7c74 <burn_image_loop+0x4e0>
    7804:	e59f3454 	ldr	r3, [pc, #1108]	; 7c60 <burn_image_loop+0x4cc>
    7808:	e1a0e00f 	mov	lr, pc
    780c:	e593f000 	ldr	pc, [r3]
    7810:	e3e00001 	mvn	r0, #1	; 0x1
    7814:	ea00010f 	b	7c58 <burn_image_loop+0x4c4>
    7818:	e596c000 	ldr	ip, [r6]
    781c:	e3a03000 	mov	r3, #0	; 0x0
    7820:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    7824:	e002200c 	and	r2, r2, ip
    7828:	e1a01483 	lsl	r1, r3, #9
    782c:	e1811ba2 	orr	r1, r1, r2, lsr #23
    7830:	e1a0e482 	lsl	lr, r2, #9
    7834:	e1a0200e 	mov	r2, lr
    7838:	e1a03001 	mov	r3, r1
    783c:	e1cd23f0 	strd	r2, [sp, #48]
    7840:	e1cd83d0 	ldrd	r8, [sp, #48]
    7844:	e1d600b8 	ldrh	r0, [r6, #8]
    7848:	e3a02401 	mov	r2, #16777216	; 0x1000000
    784c:	e0988002 	adds	r8, r8, r2
    7850:	e5961004 	ldr	r1, [r6, #4]
    7854:	e3a03000 	mov	r3, #0	; 0x0
    7858:	e1a0cc2c 	lsr	ip, ip, #24
    785c:	e0a99003 	adc	r9, r9, r3
    7860:	e1a05000 	mov	r5, r0
    7864:	e3a04000 	mov	r4, #0	; 0x0
    7868:	e20cc03f 	and	ip, ip, #63	; 0x3f
    786c:	e59d0024 	ldr	r0, [sp, #36]
    7870:	e1cd83f0 	strd	r8, [sp, #48]
    7874:	e58dc03c 	str	ip, [sp, #60]
    7878:	e1848001 	orr	r8, r4, r1
    787c:	e1a09005 	mov	r9, r5
    7880:	e596500c 	ldr	r5, [r6, #12]
    7884:	ebfffa54 	bl	61dc <vs_device_erasable>
    7888:	e1500004 	cmp	r0, r4
    788c:	0a000006 	beq	78ac <burn_image_loop+0x118>
    7890:	e1a01004 	mov	r1, r4
    7894:	e59d0024 	ldr	r0, [sp, #36]
    7898:	ebfffb36 	bl	6578 <vs_assign_by_id>
    789c:	e1a00008 	mov	r0, r8
    78a0:	e1a01009 	mov	r1, r9
    78a4:	e1a0200a 	mov	r2, sl
    78a8:	ebfff9a2 	bl	5f38 <vs_erase>
    78ac:	e59d002c 	ldr	r0, [sp, #44]
    78b0:	e3a01001 	mov	r1, #1	; 0x1
    78b4:	ebfffb2f 	bl	6578 <vs_assign_by_id>
    78b8:	e59d003c 	ldr	r0, [sp, #60]
    78bc:	e2403002 	sub	r3, r0, #2	; 0x2
    78c0:	e3530001 	cmp	r3, #1	; 0x1
    78c4:	8a000018 	bhi	792c <burn_image_loop+0x198>
    78c8:	e3a01c02 	mov	r1, #512	; 0x200
    78cc:	e58d1000 	str	r1, [sp]
    78d0:	e59d1028 	ldr	r1, [sp, #40]
    78d4:	e1cd23d0 	ldrd	r2, [sp, #48]
    78d8:	e1a0000b 	mov	r0, fp
    78dc:	e58d1004 	str	r1, [sp, #4]
    78e0:	ebfffa7e 	bl	62e0 <vs_read>
    78e4:	e1a0000b 	mov	r0, fp
    78e8:	ebffe8a8 	bl	1b90 <isi_check_header>
    78ec:	e2504000 	subs	r4, r0, #0	; 0x0
    78f0:	1a00000d 	bne	792c <burn_image_loop+0x198>
    78f4:	e59b3004 	ldr	r3, [fp, #4]
    78f8:	e1b03e23 	lsrs	r3, r3, #28
    78fc:	0a00000a 	beq	792c <burn_image_loop+0x198>
    7900:	e59f0370 	ldr	r0, [pc, #880]	; 7c78 <burn_image_loop+0x4e4>
    7904:	e59f3354 	ldr	r3, [pc, #852]	; 7c60 <burn_image_loop+0x4cc>
    7908:	e1a0e00f 	mov	lr, pc
    790c:	e593f000 	ldr	pc, [r3]
    7910:	e59f2364 	ldr	r2, [pc, #868]	; 7c7c <burn_image_loop+0x4e8>
    7914:	e1a01004 	mov	r1, r4
    7918:	e3a00001 	mov	r0, #1	; 0x1
    791c:	ebfffb76 	bl	66fc <ss_aes_init>
    7920:	e3a02001 	mov	r2, #1	; 0x1
    7924:	e58d2044 	str	r2, [sp, #68]
    7928:	ea000001 	b	7934 <burn_image_loop+0x1a0>
    792c:	e3a03000 	mov	r3, #0	; 0x0
    7930:	e58d3044 	str	r3, [sp, #68]
    7934:	e59d003c 	ldr	r0, [sp, #60]
    7938:	e3500005 	cmp	r0, #5	; 0x5
    793c:	11a06005 	movne	r6, r5
    7940:	13a07901 	movne	r7, #16384	; 0x4000
    7944:	1a000010 	bne	798c <burn_image_loop+0x1f8>
    7948:	e59d0024 	ldr	r0, [sp, #36]
    794c:	ebfff9b5 	bl	6028 <vs_align>
    7950:	e1a04000 	mov	r4, r0
    7954:	e1a00005 	mov	r0, r5
    7958:	eb000c4d 	bl	aa94 <fnd_size_shrink>
    795c:	e1a06000 	mov	r6, r0
    7960:	e1a00005 	mov	r0, r5
    7964:	eb000c3c 	bl	aa5c <fnd_size_match>
    7968:	e3500000 	cmp	r0, #0	; 0x0
    796c:	11a07004 	movne	r7, r4
    7970:	1a000005 	bne	798c <burn_image_loop+0x1f8>
    7974:	e59f0304 	ldr	r0, [pc, #772]	; 7c80 <burn_image_loop+0x4ec>
    7978:	e59f32e0 	ldr	r3, [pc, #736]	; 7c60 <burn_image_loop+0x4cc>
    797c:	e1a0e00f 	mov	lr, pc
    7980:	e593f000 	ldr	pc, [r3]
    7984:	e3e00000 	mvn	r0, #0	; 0x0
    7988:	ea0000b2 	b	7c58 <burn_image_loop+0x4c4>
    798c:	e59d0024 	ldr	r0, [sp, #36]
    7990:	e59f12ec 	ldr	r1, [pc, #748]	; 7c84 <burn_image_loop+0x4f0>
    7994:	ebfffa38 	bl	627c <vs_is_device>
    7998:	e3500000 	cmp	r0, #0	; 0x0
    799c:	1a00000a 	bne	79cc <burn_image_loop+0x238>
    79a0:	e59d0024 	ldr	r0, [sp, #36]
    79a4:	e59f12dc 	ldr	r1, [pc, #732]	; 7c88 <burn_image_loop+0x4f4>
    79a8:	ebfffa33 	bl	627c <vs_is_device>
    79ac:	e3500000 	cmp	r0, #0	; 0x0
    79b0:	1a000005 	bne	79cc <burn_image_loop+0x238>
    79b4:	e59d0024 	ldr	r0, [sp, #36]
    79b8:	e59f12cc 	ldr	r1, [pc, #716]	; 7c8c <burn_image_loop+0x4f8>
    79bc:	ebfffa2e 	bl	627c <vs_is_device>
    79c0:	e3500000 	cmp	r0, #0	; 0x0
    79c4:	01a05000 	moveq	r5, r0
    79c8:	0a00001a 	beq	7a38 <burn_image_loop+0x2a4>
    79cc:	e59d0024 	ldr	r0, [sp, #36]
    79d0:	e59f12b4 	ldr	r1, [pc, #692]	; 7c8c <burn_image_loop+0x4f8>
    79d4:	ebfffa28 	bl	627c <vs_is_device>
    79d8:	e3500000 	cmp	r0, #0	; 0x0
    79dc:	0a00000a 	beq	7a0c <burn_image_loop+0x278>
    79e0:	e3a00000 	mov	r0, #0	; 0x0
    79e4:	eb000bd5 	bl	a940 <nand_get_config>
    79e8:	e3500000 	cmp	r0, #0	; 0x0
    79ec:	0a00000b 	beq	7a20 <burn_image_loop+0x28c>
    79f0:	e5903020 	ldr	r3, [r0, #32]
    79f4:	e3550902 	cmp	r5, #32768	; 0x8000
    79f8:	33a06902 	movcc	r6, #32768	; 0x8000
    79fc:	e3530802 	cmp	r3, #131072	; 0x20000
    7a00:	21a05003 	movcs	r5, r3
    7a04:	33a05802 	movcc	r5, #131072	; 0x20000
    7a08:	ea00000a 	b	7a38 <burn_image_loop+0x2a4>
    7a0c:	e3a00001 	mov	r0, #1	; 0x1
    7a10:	eb000bca 	bl	a940 <nand_get_config>
    7a14:	e3500000 	cmp	r0, #0	; 0x0
    7a18:	15905020 	ldrne	r5, [r0, #32]
    7a1c:	1a000005 	bne	7a38 <burn_image_loop+0x2a4>
    7a20:	e59f0268 	ldr	r0, [pc, #616]	; 7c90 <burn_image_loop+0x4fc>
    7a24:	e59f3234 	ldr	r3, [pc, #564]	; 7c60 <burn_image_loop+0x4cc>
    7a28:	e1a0e00f 	mov	lr, pc
    7a2c:	e593f000 	ldr	pc, [r3]
    7a30:	e3e00033 	mvn	r0, #51	; 0x33
    7a34:	ea000087 	b	7c58 <burn_image_loop+0x4c4>
    7a38:	e1a0100a 	mov	r1, sl
    7a3c:	e59f0250 	ldr	r0, [pc, #592]	; 7c94 <burn_image_loop+0x500>
    7a40:	e1a02007 	mov	r2, r7
    7a44:	e59f4214 	ldr	r4, [pc, #532]	; 7c60 <burn_image_loop+0x4cc>
    7a48:	e1a0e00f 	mov	lr, pc
    7a4c:	e594f000 	ldr	pc, [r4]
    7a50:	e2453001 	sub	r3, r5, #1	; 0x1
    7a54:	e1a00003 	mov	r0, r3
    7a58:	e1a01fc0 	asr	r1, r0, #31
    7a5c:	e066a00a 	rsb	sl, r6, sl
    7a60:	e1cd01f8 	strd	r0, [sp, #24]
    7a64:	e1a02005 	mov	r2, r5
    7a68:	e1a03fc2 	asr	r3, r2, #31
    7a6c:	e1a00007 	mov	r0, r7
    7a70:	e3a01000 	mov	r1, #0	; 0x0
    7a74:	e58da040 	str	sl, [sp, #64]
    7a78:	e58d6038 	str	r6, [sp, #56]
    7a7c:	e3a0a000 	mov	sl, #0	; 0x0
    7a80:	e1cd21f0 	strd	r2, [sp, #16]
    7a84:	e1cd00f8 	strd	r0, [sp, #8]
    7a88:	ea00006f 	b	7c4c <burn_image_loop+0x4b8>
    7a8c:	e59d1038 	ldr	r1, [sp, #56]
    7a90:	e0663001 	rsb	r3, r6, r1
    7a94:	e1b03a23 	lsrs	r3, r3, #20
    7a98:	0a000005 	beq	7ab4 <burn_image_loop+0x320>
    7a9c:	e59f01f4 	ldr	r0, [pc, #500]	; 7c98 <burn_image_loop+0x504>
    7aa0:	e1a01a26 	lsr	r1, r6, #20
    7aa4:	e59f21b4 	ldr	r2, [pc, #436]	; 7c60 <burn_image_loop+0x4cc>
    7aa8:	e1a0e00f 	mov	lr, pc
    7aac:	e592f000 	ldr	pc, [r2]
    7ab0:	e58d6038 	str	r6, [sp, #56]
    7ab4:	e35a0000 	cmp	sl, #0	; 0x0
    7ab8:	d3a00000 	movle	r0, #0	; 0x0
    7abc:	c3a00001 	movgt	r0, #1	; 0x1
    7ac0:	e28aa001 	add	sl, sl, #1	; 0x1
    7ac4:	eb001860 	bl	dc4c <led_ctrl>
    7ac8:	e35a0032 	cmp	sl, #50	; 0x32
    7acc:	e3a01000 	mov	r1, #0	; 0x0
    7ad0:	e59d002c 	ldr	r0, [sp, #44]
    7ad4:	c24aa064 	subgt	sl, sl, #100	; 0x64
    7ad8:	ebfffaa6 	bl	6578 <vs_assign_by_id>
    7adc:	e59d303c 	ldr	r3, [sp, #60]
    7ae0:	e3530005 	cmp	r3, #5	; 0x5
    7ae4:	11a01007 	movne	r1, r7
    7ae8:	1a000001 	bne	7af4 <burn_image_loop+0x360>
    7aec:	eb000bd0 	bl	aa34 <fnd_align>
    7af0:	e1a01000 	mov	r1, r0
    7af4:	e58d1000 	str	r1, [sp]
    7af8:	e59d1028 	ldr	r1, [sp, #40]
    7afc:	e1cd23d0 	ldrd	r2, [sp, #48]
    7b00:	e1a0000b 	mov	r0, fp
    7b04:	e58d1004 	str	r1, [sp, #4]
    7b08:	ebfff9f4 	bl	62e0 <vs_read>
    7b0c:	e59d2044 	ldr	r2, [sp, #68]
    7b10:	e3520000 	cmp	r2, #0	; 0x0
    7b14:	0a000003 	beq	7b28 <burn_image_loop+0x394>
    7b18:	e1a0000b 	mov	r0, fp
    7b1c:	e1a0100b 	mov	r1, fp
    7b20:	e1a02007 	mov	r2, r7
    7b24:	ebfffae3 	bl	66b8 <ss_aes_data>
    7b28:	e59d0024 	ldr	r0, [sp, #36]
    7b2c:	e3a01000 	mov	r1, #0	; 0x0
    7b30:	ebfffa90 	bl	6578 <vs_assign_by_id>
    7b34:	e3500000 	cmp	r0, #0	; 0x0
    7b38:	1a00002e 	bne	7bf8 <burn_image_loop+0x464>
    7b3c:	e3550000 	cmp	r5, #0	; 0x0
    7b40:	0a00001f 	beq	7bc4 <burn_image_loop+0x430>
    7b44:	e1cd01d8 	ldrd	r0, [sp, #24]
    7b48:	e0082000 	and	r2, r8, r0
    7b4c:	e0093001 	and	r3, r9, r1
    7b50:	e1921003 	orrs	r1, r2, r3
    7b54:	0a000012 	beq	7ba4 <burn_image_loop+0x410>
    7b58:	ea000019 	b	7bc4 <burn_image_loop+0x430>
    7b5c:	e59d1040 	ldr	r1, [sp, #64]
    7b60:	e1510005 	cmp	r1, r5
    7b64:	e0651001 	rsb	r1, r5, r1
    7b68:	e58d1040 	str	r1, [sp, #64]
    7b6c:	da000006 	ble	7b8c <burn_image_loop+0x3f8>
    7b70:	e59f10e8 	ldr	r1, [pc, #232]	; 7c60 <burn_image_loop+0x4cc>
    7b74:	e1a0e00f 	mov	lr, pc
    7b78:	e591f000 	ldr	pc, [r1]
    7b7c:	e1cd21d0 	ldrd	r2, [sp, #16]
    7b80:	e0988002 	adds	r8, r8, r2
    7b84:	e0a99003 	adc	r9, r9, r3
    7b88:	ea000005 	b	7ba4 <burn_image_loop+0x410>
    7b8c:	e59f0108 	ldr	r0, [pc, #264]	; 7c9c <burn_image_loop+0x508>
    7b90:	e59f30c8 	ldr	r3, [pc, #200]	; 7c60 <burn_image_loop+0x4cc>
    7b94:	e1a0e00f 	mov	lr, pc
    7b98:	e593f000 	ldr	pc, [r3]
    7b9c:	e3e00034 	mvn	r0, #52	; 0x34
    7ba0:	ea00002c 	b	7c58 <burn_image_loop+0x4c4>
    7ba4:	e1a00008 	mov	r0, r8
    7ba8:	e1a01009 	mov	r1, r9
    7bac:	ebfff8f0 	bl	5f74 <vs_isbad>
    7bb0:	e3500000 	cmp	r0, #0	; 0x0
    7bb4:	e1a02008 	mov	r2, r8
    7bb8:	e1a03009 	mov	r3, r9
    7bbc:	e59f00dc 	ldr	r0, [pc, #220]	; 7ca0 <burn_image_loop+0x50c>
    7bc0:	1affffe5 	bne	7b5c <burn_image_loop+0x3c8>
    7bc4:	e59d303c 	ldr	r3, [sp, #60]
    7bc8:	e3530005 	cmp	r3, #5	; 0x5
    7bcc:	11a01007 	movne	r1, r7
    7bd0:	1a000001 	bne	7bdc <burn_image_loop+0x448>
    7bd4:	eb000b96 	bl	aa34 <fnd_align>
    7bd8:	e1a01000 	mov	r1, r0
    7bdc:	e58d1000 	str	r1, [sp]
    7be0:	e1a0000b 	mov	r0, fp
    7be4:	e3a01000 	mov	r1, #0	; 0x0
    7be8:	e1a02008 	mov	r2, r8
    7bec:	e1a03009 	mov	r3, r9
    7bf0:	e58d1004 	str	r1, [sp, #4]
    7bf4:	ebfff8b9 	bl	5ee0 <vs_write>
    7bf8:	e59d003c 	ldr	r0, [sp, #60]
    7bfc:	e3500001 	cmp	r0, #1	; 0x1
    7c00:	91a04007 	movls	r4, r7
    7c04:	83a04000 	movhi	r4, #0	; 0x0
    7c08:	e3500005 	cmp	r0, #5	; 0x5
    7c0c:	11a0c007 	movne	ip, r7
    7c10:	1a000001 	bne	7c1c <burn_image_loop+0x488>
    7c14:	eb000b86 	bl	aa34 <fnd_align>
    7c18:	e1a0c000 	mov	ip, r0
    7c1c:	e1cd03d0 	ldrd	r0, [sp, #48]
    7c20:	e1560007 	cmp	r6, r7
    7c24:	31a0e006 	movcc	lr, r6
    7c28:	21a0e007 	movcs	lr, r7
    7c2c:	e090200c 	adds	r2, r0, ip
    7c30:	e2a13000 	adc	r3, r1, #0	; 0x0
    7c34:	e1cd23f0 	strd	r2, [sp, #48]
    7c38:	e1cd20d8 	ldrd	r2, [sp, #8]
    7c3c:	e0988002 	adds	r8, r8, r2
    7c40:	e08bb004 	add	fp, fp, r4
    7c44:	e0a99003 	adc	r9, r9, r3
    7c48:	e06e6006 	rsb	r6, lr, r6
    7c4c:	e3560000 	cmp	r6, #0	; 0x0
    7c50:	1affff8d 	bne	7a8c <burn_image_loop+0x2f8>
    7c54:	e1a00006 	mov	r0, r6
    7c58:	e28dd04c 	add	sp, sp, #76	; 0x4c
    7c5c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7c60:	3c00d694 	.word	0x3c00d694
    7c64:	0001b804 	.word	0x0001b804
    7c68:	0001a0ca 	.word	0x0001a0ca
    7c6c:	0001b99f 	.word	0x0001b99f
    7c70:	0001b9a6 	.word	0x0001b9a6
    7c74:	0001b9bc 	.word	0x0001b9bc
    7c78:	0001b9e9 	.word	0x0001b9e9
    7c7c:	3c00d094 	.word	0x3c00d094
    7c80:	0001ba02 	.word	0x0001ba02
    7c84:	00019f12 	.word	0x00019f12
    7c88:	00019f16 	.word	0x00019f16
    7c8c:	00019da3 	.word	0x00019da3
    7c90:	0001ba1f 	.word	0x0001ba1f
    7c94:	0001ba37 	.word	0x0001ba37
    7c98:	0001ba46 	.word	0x0001ba46
    7c9c:	0001ba72 	.word	0x0001ba72
    7ca0:	0001ba53 	.word	0x0001ba53

00007ca4 <burn_images>:
    7ca4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7ca8:	e590b01c 	ldr	fp, [r0, #28]
    7cac:	e24dd014 	sub	sp, sp, #20	; 0x14
    7cb0:	e59f41dc 	ldr	r4, [pc, #476]	; 7e94 <burn_images+0x1f0>
    7cb4:	e1a05000 	mov	r5, r0
    7cb8:	e58d100c 	str	r1, [sp, #12]
    7cbc:	e59f01d4 	ldr	r0, [pc, #468]	; 7e98 <burn_images+0x1f4>
    7cc0:	e1a0100b 	mov	r1, fp
    7cc4:	e3a07000 	mov	r7, #0	; 0x0
    7cc8:	e58d2008 	str	r2, [sp, #8]
    7ccc:	e1a0e00f 	mov	lr, pc
    7cd0:	e594f000 	ldr	pc, [r4]
    7cd4:	e2856020 	add	r6, r5, #32	; 0x20
    7cd8:	e1a09004 	mov	r9, r4
    7cdc:	e1a0a007 	mov	sl, r7
    7ce0:	ea000061 	b	7e6c <burn_images+0x1c8>
    7ce4:	e59f01b0 	ldr	r0, [pc, #432]	; 7e9c <burn_images+0x1f8>
    7ce8:	e1a01007 	mov	r1, r7
    7cec:	e1a0e00f 	mov	lr, pc
    7cf0:	e599f000 	ldr	pc, [r9]
    7cf4:	e5d63003 	ldrb	r3, [r6, #3]
    7cf8:	e5968008 	ldr	r8, [r6, #8]
    7cfc:	e203503f 	and	r5, r3, #63	; 0x3f
    7d00:	e3550003 	cmp	r5, #3	; 0x3
    7d04:	0a000010 	beq	7d4c <burn_images+0xa8>
    7d08:	ca000006 	bgt	7d28 <burn_images+0x84>
    7d0c:	e3550001 	cmp	r5, #1	; 0x1
    7d10:	0a000013 	beq	7d64 <burn_images+0xc0>
    7d14:	c1a04828 	lsrgt	r4, r8, #16
    7d18:	ca000026 	bgt	7db8 <burn_images+0x114>
    7d1c:	e3550000 	cmp	r5, #0	; 0x0
    7d20:	0a000059 	beq	7e8c <burn_images+0x1e8>
    7d24:	ea00001f 	b	7da8 <burn_images+0x104>
    7d28:	e3550005 	cmp	r5, #5	; 0x5
    7d2c:	0a000008 	beq	7d54 <burn_images+0xb0>
    7d30:	b59f0168 	ldrlt	r0, [pc, #360]	; 7ea0 <burn_images+0x1fc>
    7d34:	ba000007 	blt	7d58 <burn_images+0xb4>
    7d38:	e3550030 	cmp	r5, #48	; 0x30
    7d3c:	0a00000e 	beq	7d7c <burn_images+0xd8>
    7d40:	e3550031 	cmp	r5, #49	; 0x31
    7d44:	1a000017 	bne	7da8 <burn_images+0x104>
    7d48:	ea000010 	b	7d90 <burn_images+0xec>
    7d4c:	e59f0150 	ldr	r0, [pc, #336]	; 7ea4 <burn_images+0x200>
    7d50:	ea000000 	b	7d58 <burn_images+0xb4>
    7d54:	e59f014c 	ldr	r0, [pc, #332]	; 7ea8 <burn_images+0x204>
    7d58:	ebfff933 	bl	622c <vs_device_id>
    7d5c:	e1a04000 	mov	r4, r0
    7d60:	ea000014 	b	7db8 <burn_images+0x114>
    7d64:	e59f0140 	ldr	r0, [pc, #320]	; 7eac <burn_images+0x208>
    7d68:	e1a01008 	mov	r1, r8
    7d6c:	e1a0a008 	mov	sl, r8
    7d70:	e1a0e00f 	mov	lr, pc
    7d74:	e599f000 	ldr	pc, [r9]
    7d78:	ea000043 	b	7e8c <burn_images+0x1e8>
    7d7c:	e59f012c 	ldr	r0, [pc, #300]	; 7eb0 <burn_images+0x20c>
    7d80:	e1a0e00f 	mov	lr, pc
    7d84:	e599f000 	ldr	pc, [r9]
    7d88:	ebffe2bd 	bl	884 <system_reset>
    7d8c:	ea00003e 	b	7e8c <burn_images+0x1e8>
    7d90:	e59f011c 	ldr	r0, [pc, #284]	; 7eb4 <burn_images+0x210>
    7d94:	e1a0e00f 	mov	lr, pc
    7d98:	e599f000 	ldr	pc, [r9]
    7d9c:	e3a00001 	mov	r0, #1	; 0x1
    7da0:	ebffe19a 	bl	410 <try_boot>
    7da4:	ea000038 	b	7e8c <burn_images+0x1e8>
    7da8:	e59f0108 	ldr	r0, [pc, #264]	; 7eb8 <burn_images+0x214>
    7dac:	e1a0e00f 	mov	lr, pc
    7db0:	e599f000 	ldr	pc, [r9]
    7db4:	ea00002a 	b	7e64 <burn_images+0x1c0>
    7db8:	e354000f 	cmp	r4, #15	; 0xf
    7dbc:	0a00000a 	beq	7dec <burn_images+0x148>
    7dc0:	e1a00004 	mov	r0, r4
    7dc4:	e3a01001 	mov	r1, #1	; 0x1
    7dc8:	ebfff9ea 	bl	6578 <vs_assign_by_id>
    7dcc:	e3500000 	cmp	r0, #0	; 0x0
    7dd0:	0a000005 	beq	7dec <burn_images+0x148>
    7dd4:	e59f00e0 	ldr	r0, [pc, #224]	; 7ebc <burn_images+0x218>
    7dd8:	e59f30b4 	ldr	r3, [pc, #180]	; 7e94 <burn_images+0x1f0>
    7ddc:	e1a0e00f 	mov	lr, pc
    7de0:	e593f000 	ldr	pc, [r3]
    7de4:	e3e04002 	mvn	r4, #2	; 0x2
    7de8:	ea000022 	b	7e78 <burn_images+0x1d4>
    7dec:	e3550001 	cmp	r5, #1	; 0x1
    7df0:	e1a0c828 	lsr	ip, r8, #16
    7df4:	01a0e00a 	moveq	lr, sl
    7df8:	13a0e30f 	movne	lr, #1006632960	; 0x3c000000
    7dfc:	e1a0c80c 	lsl	ip, ip, #16
    7e00:	e1a03004 	mov	r3, r4
    7e04:	e1a00006 	mov	r0, r6
    7e08:	e59d100c 	ldr	r1, [sp, #12]
    7e0c:	e59d2008 	ldr	r2, [sp, #8]
    7e10:	e88d5000 	stm	sp, {ip, lr}
    7e14:	ebfffe5e 	bl	7794 <burn_image_loop>
    7e18:	e3500000 	cmp	r0, #0	; 0x0
    7e1c:	aa000006 	bge	7e3c <burn_images+0x198>
    7e20:	e1a01007 	mov	r1, r7
    7e24:	e59f0094 	ldr	r0, [pc, #148]	; 7ec0 <burn_images+0x21c>
    7e28:	e59f3064 	ldr	r3, [pc, #100]	; 7e94 <burn_images+0x1f0>
    7e2c:	e1a0e00f 	mov	lr, pc
    7e30:	e593f000 	ldr	pc, [r3]
    7e34:	e3e04000 	mvn	r4, #0	; 0x0
    7e38:	ea00000e 	b	7e78 <burn_images+0x1d4>
    7e3c:	e3550001 	cmp	r5, #1	; 0x1
    7e40:	1a000003 	bne	7e54 <burn_images+0x1b0>
    7e44:	e5960004 	ldr	r0, [r6, #4]
    7e48:	e3a01000 	mov	r1, #0	; 0x0
    7e4c:	ebfffda8 	bl	74f4 <cdbg_jump>
    7e50:	ea000003 	b	7e64 <burn_images+0x1c0>
    7e54:	e3550000 	cmp	r5, #0	; 0x0
    7e58:	1a000001 	bne	7e64 <burn_images+0x1c0>
    7e5c:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
    7e60:	ebfffe36 	bl	7740 <cdbg_config_isi>
    7e64:	e2877001 	add	r7, r7, #1	; 0x1
    7e68:	e2866010 	add	r6, r6, #16	; 0x10
    7e6c:	e157000b 	cmp	r7, fp
    7e70:	baffff9b 	blt	7ce4 <burn_images+0x40>
    7e74:	e3a04000 	mov	r4, #0	; 0x0
    7e78:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
    7e7c:	ebffe6a8 	bl	1924 <free>
    7e80:	e1a00004 	mov	r0, r4
    7e84:	e28dd014 	add	sp, sp, #20	; 0x14
    7e88:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7e8c:	e3a0400f 	mov	r4, #15	; 0xf
    7e90:	eaffffd5 	b	7dec <burn_images+0x148>
    7e94:	3c00d694 	.word	0x3c00d694
    7e98:	0001ba90 	.word	0x0001ba90
    7e9c:	0001baad 	.word	0x0001baad
    7ea0:	00019f12 	.word	0x00019f12
    7ea4:	00019da3 	.word	0x00019da3
    7ea8:	00019f16 	.word	0x00019f16
    7eac:	0001bac3 	.word	0x0001bac3
    7eb0:	0001bae0 	.word	0x0001bae0
    7eb4:	0001baf5 	.word	0x0001baf5
    7eb8:	0001bb08 	.word	0x0001bb08
    7ebc:	0001bb1c 	.word	0x0001bb1c
    7ec0:	0001bb39 	.word	0x0001bb39

00007ec4 <burn>:
    7ec4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7ec8:	e1a09000 	mov	r9, r0
    7ecc:	e24dd00c 	sub	sp, sp, #12	; 0xc
    7ed0:	e3a00c02 	mov	r0, #512	; 0x200
    7ed4:	ebffe62a 	bl	1784 <malloc>
    7ed8:	e2507000 	subs	r7, r0, #0	; 0x0
    7edc:	1a000004 	bne	7ef4 <burn+0x30>
    7ee0:	e59f019c 	ldr	r0, [pc, #412]	; 8084 <burn+0x1c0>
    7ee4:	e59f319c 	ldr	r3, [pc, #412]	; 8088 <burn+0x1c4>
    7ee8:	e1a0e00f 	mov	lr, pc
    7eec:	e593f000 	ldr	pc, [r3]
    7ef0:	ea000060 	b	8078 <burn+0x1b4>
    7ef4:	ebffe1c5 	bl	610 <boot_device>
    7ef8:	e3a05000 	mov	r5, #0	; 0x0
    7efc:	e59f8184 	ldr	r8, [pc, #388]	; 8088 <burn+0x1c4>
    7f00:	e3a0bc02 	mov	fp, #512	; 0x200
    7f04:	e1a0a000 	mov	sl, r0
    7f08:	ea00004c 	b	8040 <burn+0x17c>
    7f0c:	e1a00005 	mov	r0, r5
    7f10:	ebfff86c 	bl	60c8 <vs_device_burnsrc>
    7f14:	e3500000 	cmp	r0, #0	; 0x0
    7f18:	0a000047 	beq	803c <burn+0x178>
    7f1c:	e155000a 	cmp	r5, sl
    7f20:	0a000045 	beq	803c <burn+0x178>
    7f24:	e3a03001 	mov	r3, #1	; 0x1
    7f28:	e0193513 	ands	r3, r9, r3, lsl r5
    7f2c:	1a000042 	bne	803c <burn+0x178>
    7f30:	ebffe2b5 	bl	a0c <gtc_time>
    7f34:	e1a00005 	mov	r0, r5
    7f38:	e59f114c 	ldr	r1, [pc, #332]	; 808c <burn+0x1c8>
    7f3c:	ebfff8ce 	bl	627c <vs_is_device>
    7f40:	e3500000 	cmp	r0, #0	; 0x0
    7f44:	1a000004 	bne	7f5c <burn+0x98>
    7f48:	e1a00005 	mov	r0, r5
    7f4c:	e59f113c 	ldr	r1, [pc, #316]	; 8090 <burn+0x1cc>
    7f50:	ebfff8c9 	bl	627c <vs_is_device>
    7f54:	e3500000 	cmp	r0, #0	; 0x0
    7f58:	0a00000c 	beq	7f90 <burn+0xcc>
    7f5c:	ebffe33c 	bl	c54 <assert_linkpc>
    7f60:	e3500000 	cmp	r0, #0	; 0x0
    7f64:	0a000009 	beq	7f90 <burn+0xcc>
    7f68:	eb001a45 	bl	e884 <linkpc_loop>
    7f6c:	e2506000 	subs	r6, r0, #0	; 0x0
    7f70:	0a000038 	beq	8058 <burn+0x194>
    7f74:	e1a00005 	mov	r0, r5
    7f78:	e5984000 	ldr	r4, [r8]
    7f7c:	ebfff887 	bl	61a0 <vs_device_name>
    7f80:	e1a02006 	mov	r2, r6
    7f84:	e1a01000 	mov	r1, r0
    7f88:	e59f0104 	ldr	r0, [pc, #260]	; 8094 <burn+0x1d0>
    7f8c:	ea00000a 	b	7fbc <burn+0xf8>
    7f90:	e1a00005 	mov	r0, r5
    7f94:	e3a01001 	mov	r1, #1	; 0x1
    7f98:	ebfff976 	bl	6578 <vs_assign_by_id>
    7f9c:	e2506000 	subs	r6, r0, #0	; 0x0
    7fa0:	0a000007 	beq	7fc4 <burn+0x100>
    7fa4:	e1a00005 	mov	r0, r5
    7fa8:	e5984000 	ldr	r4, [r8]
    7fac:	ebfff87b 	bl	61a0 <vs_device_name>
    7fb0:	e1a01000 	mov	r1, r0
    7fb4:	e59f00dc 	ldr	r0, [pc, #220]	; 8098 <burn+0x1d4>
    7fb8:	e1a02006 	mov	r2, r6
    7fbc:	e12fff34 	blx	r4
    7fc0:	ea00001d 	b	803c <burn+0x178>
    7fc4:	e1a01007 	mov	r1, r7
    7fc8:	e59f00cc 	ldr	r0, [pc, #204]	; 809c <burn+0x1d8>
    7fcc:	e3a02401 	mov	r2, #16777216	; 0x1000000
    7fd0:	e3a03c02 	mov	r3, #512	; 0x200
    7fd4:	e58d6000 	str	r6, [sp]
    7fd8:	e1a0e00f 	mov	lr, pc
    7fdc:	e598f000 	ldr	pc, [r8]
    7fe0:	e3a02401 	mov	r2, #16777216	; 0x1000000
    7fe4:	e3a03000 	mov	r3, #0	; 0x0
    7fe8:	e1a00007 	mov	r0, r7
    7fec:	e58d6004 	str	r6, [sp, #4]
    7ff0:	e58db000 	str	fp, [sp]
    7ff4:	ebfff8b9 	bl	62e0 <vs_read>
    7ff8:	e1a00007 	mov	r0, r7
    7ffc:	eb002056 	bl	1015c <ius_check_header>
    8000:	e2504000 	subs	r4, r0, #0	; 0x0
    8004:	1a00000c 	bne	803c <burn+0x178>
    8008:	e1a01005 	mov	r1, r5
    800c:	e59f3074 	ldr	r3, [pc, #116]	; 8088 <burn+0x1c4>
    8010:	e59f0088 	ldr	r0, [pc, #136]	; 80a0 <burn+0x1dc>
    8014:	e1a0e00f 	mov	lr, pc
    8018:	e593f000 	ldr	pc, [r3]
    801c:	e1a00004 	mov	r0, r4
    8020:	e1a01004 	mov	r1, r4
    8024:	ebfffd27 	bl	74c8 <cdbg_verify_burn_enable>
    8028:	e1a00007 	mov	r0, r7
    802c:	e1a01005 	mov	r1, r5
    8030:	e1a02004 	mov	r2, r4
    8034:	ebffff1a 	bl	7ca4 <burn_images>
    8038:	ea000006 	b	8058 <burn+0x194>
    803c:	e2855001 	add	r5, r5, #1	; 0x1
    8040:	ebfff877 	bl	6224 <vs_device_count>
    8044:	e1550000 	cmp	r5, r0
    8048:	baffffaf 	blt	7f0c <burn+0x48>
    804c:	e1a00007 	mov	r0, r7
    8050:	ebffe633 	bl	1924 <free>
    8054:	ea000007 	b	8078 <burn+0x1b4>
    8058:	e59f0044 	ldr	r0, [pc, #68]	; 80a4 <burn+0x1e0>
    805c:	e59f3024 	ldr	r3, [pc, #36]	; 8088 <burn+0x1c4>
    8060:	e1a0e00f 	mov	lr, pc
    8064:	e593f000 	ldr	pc, [r3]
    8068:	e1a00007 	mov	r0, r7
    806c:	ebffe62c 	bl	1924 <free>
    8070:	ebfffd51 	bl	75bc <cdbg_shell>
    8074:	eafffffe 	b	8074 <burn+0x1b0>
    8078:	e3e00000 	mvn	r0, #0	; 0x0
    807c:	e28dd00c 	add	sp, sp, #12	; 0xc
    8080:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    8084:	0001bb51 	.word	0x0001bb51
    8088:	3c00d694 	.word	0x3c00d694
    808c:	0001b66d 	.word	0x0001b66d
    8090:	0001b669 	.word	0x0001b669
    8094:	0001bb71 	.word	0x0001bb71
    8098:	0001bb93 	.word	0x0001bb93
    809c:	0001bbb2 	.word	0x0001bbb2
    80a0:	0001bbe2 	.word	0x0001bbe2
    80a4:	0001bbfa 	.word	0x0001bbfa

000080a8 <mmc_send_cmd>:
    80a8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    80ac:	e24dd004 	sub	sp, sp, #4	; 0x4
    80b0:	e1a0e00f 	mov	lr, pc
    80b4:	e590f104 	ldr	pc, [r0, #260]
    80b8:	e28dd004 	add	sp, sp, #4	; 0x4
    80bc:	e8bd8000 	pop	{pc}

000080c0 <mmc_set_blocklen>:
    80c0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    80c4:	e3a03015 	mov	r3, #21	; 0x15
    80c8:	e24dd024 	sub	sp, sp, #36	; 0x24
    80cc:	e3a02000 	mov	r2, #0	; 0x0
    80d0:	e58d3004 	str	r3, [sp, #4]
    80d4:	e3a03010 	mov	r3, #16	; 0x10
    80d8:	e58d1008 	str	r1, [sp, #8]
    80dc:	e1cd30b0 	strh	r3, [sp]
    80e0:	e58d201c 	str	r2, [sp, #28]
    80e4:	e1a0100d 	mov	r1, sp
    80e8:	e1a0e00f 	mov	lr, pc
    80ec:	e590f104 	ldr	pc, [r0, #260]
    80f0:	e28dd024 	add	sp, sp, #36	; 0x24
    80f4:	e8bd8000 	pop	{pc}

000080f8 <find_mmc_device>:
    80f8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    80fc:	e59f3048 	ldr	r3, [pc, #72]	; 814c <find_mmc_device+0x54>
    8100:	e24dd004 	sub	sp, sp, #4	; 0x4
    8104:	e1a01000 	mov	r1, r0
    8108:	e1a0c003 	mov	ip, r3
    810c:	e5932000 	ldr	r2, [r3]
    8110:	ea000004 	b	8128 <find_mmc_device+0x30>
    8114:	e592309c 	ldr	r3, [r2, #156]
    8118:	e1a00002 	mov	r0, r2
    811c:	e1530001 	cmp	r3, r1
    8120:	0a000007 	beq	8144 <find_mmc_device+0x4c>
    8124:	e5922000 	ldr	r2, [r2]
    8128:	e152000c 	cmp	r2, ip
    812c:	1afffff8 	bne	8114 <find_mmc_device+0x1c>
    8130:	e59f0018 	ldr	r0, [pc, #24]	; 8150 <find_mmc_device+0x58>
    8134:	e59f3018 	ldr	r3, [pc, #24]	; 8154 <find_mmc_device+0x5c>
    8138:	e1a0e00f 	mov	lr, pc
    813c:	e593f000 	ldr	pc, [r3]
    8140:	e3a00000 	mov	r0, #0	; 0x0
    8144:	e28dd004 	add	sp, sp, #4	; 0x4
    8148:	e8bd8000 	pop	{pc}
    814c:	3c00d1d0 	.word	0x3c00d1d0
    8150:	0001bc1d 	.word	0x0001bc1d
    8154:	3c00d694 	.word	0x3c00d694

00008158 <mmc_send_ext_csd>:
    8158:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    815c:	e3a03015 	mov	r3, #21	; 0x15
    8160:	e24dd034 	sub	sp, sp, #52	; 0x34
    8164:	e58d3004 	str	r3, [sp, #4]
    8168:	e3a03c02 	mov	r3, #512	; 0x200
    816c:	e3a02000 	mov	r2, #0	; 0x0
    8170:	e3a0c001 	mov	ip, #1	; 0x1
    8174:	e58d302c 	str	r3, [sp, #44]
    8178:	e3a03008 	mov	r3, #8	; 0x8
    817c:	e58d1020 	str	r1, [sp, #32]
    8180:	e58d201c 	str	r2, [sp, #28]
    8184:	e58d2008 	str	r2, [sp, #8]
    8188:	e58dc024 	str	ip, [sp, #36]
    818c:	e1cd30b0 	strh	r3, [sp]
    8190:	e58dc028 	str	ip, [sp, #40]
    8194:	e1a0100d 	mov	r1, sp
    8198:	e28d2020 	add	r2, sp, #32	; 0x20
    819c:	e1a0e00f 	mov	lr, pc
    81a0:	e590f104 	ldr	pc, [r0, #260]
    81a4:	e28dd034 	add	sp, sp, #52	; 0x34
    81a8:	e8bd8000 	pop	{pc}

000081ac <sd_switch>:
    81ac:	e3e0c4ff 	mvn	ip, #-16777216	; 0xff000000
    81b0:	e1a02102 	lsl	r2, r2, #2
    81b4:	e18ccf81 	orr	ip, ip, r1, lsl #31
    81b8:	e3a0100f 	mov	r1, #15	; 0xf
    81bc:	e1ccc211 	bic	ip, ip, r1, lsl r2
    81c0:	e18cc213 	orr	ip, ip, r3, lsl r2
    81c4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    81c8:	e3a03015 	mov	r3, #21	; 0x15
    81cc:	e24dd034 	sub	sp, sp, #52	; 0x34
    81d0:	e58d3004 	str	r3, [sp, #4]
    81d4:	e3a03000 	mov	r3, #0	; 0x0
    81d8:	e58d301c 	str	r3, [sp, #28]
    81dc:	e59d3038 	ldr	r3, [sp, #56]
    81e0:	e3a02001 	mov	r2, #1	; 0x1
    81e4:	e58d3020 	str	r3, [sp, #32]
    81e8:	e3a03040 	mov	r3, #64	; 0x40
    81ec:	e58d302c 	str	r3, [sp, #44]
    81f0:	e3a03006 	mov	r3, #6	; 0x6
    81f4:	e58d2024 	str	r2, [sp, #36]
    81f8:	e58d2028 	str	r2, [sp, #40]
    81fc:	e58dc008 	str	ip, [sp, #8]
    8200:	e1cd30b0 	strh	r3, [sp]
    8204:	e1a0100d 	mov	r1, sp
    8208:	e28d2020 	add	r2, sp, #32	; 0x20
    820c:	e1a0e00f 	mov	lr, pc
    8210:	e590f104 	ldr	pc, [r0, #260]
    8214:	e28dd034 	add	sp, sp, #52	; 0x34
    8218:	e8bd8000 	pop	{pc}

0000821c <mmc_set_ios>:
    821c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8220:	e24dd004 	sub	sp, sp, #4	; 0x4
    8224:	e1a0e00f 	mov	lr, pc
    8228:	e590f108 	ldr	pc, [r0, #264]
    822c:	e28dd004 	add	sp, sp, #4	; 0x4
    8230:	e8bd8000 	pop	{pc}

00008234 <mmc_set_clock>:
    8234:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8238:	e590203c 	ldr	r2, [r0, #60]
    823c:	e590c038 	ldr	ip, [r0, #56]
    8240:	e1510002 	cmp	r1, r2
    8244:	21a01002 	movcs	r1, r2
    8248:	e24dd004 	sub	sp, sp, #4	; 0x4
    824c:	e151000c 	cmp	r1, ip
    8250:	25801048 	strcs	r1, [r0, #72]
    8254:	3580c048 	strcc	ip, [r0, #72]
    8258:	e1a0e00f 	mov	lr, pc
    825c:	e590f108 	ldr	pc, [r0, #264]
    8260:	e28dd004 	add	sp, sp, #4	; 0x4
    8264:	e8bd8000 	pop	{pc}

00008268 <mmc_set_bus_width>:
    8268:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    826c:	e5801044 	str	r1, [r0, #68]
    8270:	e24dd004 	sub	sp, sp, #4	; 0x4
    8274:	e1a0e00f 	mov	lr, pc
    8278:	e590f108 	ldr	pc, [r0, #264]
    827c:	e28dd004 	add	sp, sp, #4	; 0x4
    8280:	e8bd8000 	pop	{pc}

00008284 <mmc_send_if_cond>:
    8284:	e92d4010 	push	{r4, lr}
    8288:	e590202c 	ldr	r2, [r0, #44]
    828c:	e59f3060 	ldr	r3, [pc, #96]	; 82f4 <mmc_send_if_cond+0x70>
    8290:	e24dd020 	sub	sp, sp, #32	; 0x20
    8294:	e0023003 	and	r3, r2, r3
    8298:	e3530000 	cmp	r3, #0	; 0x0
    829c:	e28d1020 	add	r1, sp, #32	; 0x20
    82a0:	e3a03008 	mov	r3, #8	; 0x8
    82a4:	e300c1aa 	movw	ip, #426	; 0x1aa
    82a8:	e16132b0 	strh	r3, [r1, #-32]!
    82ac:	03a0c0aa 	moveq	ip, #170	; 0xaa
    82b0:	e3a02000 	mov	r2, #0	; 0x0
    82b4:	e3a03015 	mov	r3, #21	; 0x15
    82b8:	e98d1008 	stmib	sp, {r3, ip}
    82bc:	e58d201c 	str	r2, [sp, #28]
    82c0:	e1a0100d 	mov	r1, sp
    82c4:	e5903104 	ldr	r3, [r0, #260]
    82c8:	e1a04000 	mov	r4, r0
    82cc:	e12fff33 	blx	r3
    82d0:	e3500000 	cmp	r0, #0	; 0x0
    82d4:	1a000004 	bne	82ec <mmc_send_if_cond+0x68>
    82d8:	e5dd300c 	ldrb	r3, [sp, #12]
    82dc:	e35300aa 	cmp	r3, #170	; 0xaa
    82e0:	059f3010 	ldreq	r3, [pc, #16]	; 82f8 <mmc_send_if_cond+0x74>
    82e4:	13e00010 	mvnne	r0, #16	; 0x10
    82e8:	05843030 	streq	r3, [r4, #48]
    82ec:	e28dd020 	add	sp, sp, #32	; 0x20
    82f0:	e8bd8010 	pop	{r4, pc}
    82f4:	00ff8000 	.word	0x00ff8000
    82f8:	00020020 	.word	0x00020020

000082fc <mmc_register>:
    82fc:	e5903110 	ldr	r3, [r0, #272]
    8300:	e59fc05c 	ldr	ip, [pc, #92]	; 8364 <mmc_register+0x68>
    8304:	e3530000 	cmp	r3, #0	; 0x0
    8308:	e3a03006 	mov	r3, #6	; 0x6
    830c:	e59c2000 	ldr	r2, [ip]
    8310:	e5803098 	str	r3, [r0, #152]
    8314:	e59f304c 	ldr	r3, [pc, #76]	; 8368 <mmc_register+0x6c>
    8318:	e2821001 	add	r1, r2, #1	; 0x1
    831c:	e58030f8 	str	r3, [r0, #248]
    8320:	e59f3044 	ldr	r3, [pc, #68]	; 836c <mmc_register+0x70>
    8324:	e580209c 	str	r2, [r0, #156]
    8328:	e58030fc 	str	r3, [r0, #252]
    832c:	e3a03001 	mov	r3, #1	; 0x1
    8330:	e58c1000 	str	r1, [ip]
    8334:	e5c030a4 	strb	r3, [r0, #164]
    8338:	03a03c02 	moveq	r3, #512	; 0x200
    833c:	05803110 	streq	r3, [r0, #272]
    8340:	e59f3028 	ldr	r3, [pc, #40]	; 8370 <mmc_register+0x74>
    8344:	e5800004 	str	r0, [r0, #4]
    8348:	e5932004 	ldr	r2, [r3, #4]
    834c:	e5803000 	str	r3, [r0]
    8350:	e5830004 	str	r0, [r3, #4]
    8354:	e5820000 	str	r0, [r2]
    8358:	e5802004 	str	r2, [r0, #4]
    835c:	e3a00000 	mov	r0, #0	; 0x0
    8360:	e12fff1e 	bx	lr
    8364:	3c00d0b4 	.word	0x3c00d0b4
    8368:	00009538 	.word	0x00009538
    836c:	00008a54 	.word	0x00008a54
    8370:	3c00d1d0 	.word	0x3c00d1d0

00008374 <mmc_get_dev>:
    8374:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8378:	e24dd004 	sub	sp, sp, #4	; 0x4
    837c:	ebffff5d 	bl	80f8 <find_mmc_device>
    8380:	e3500000 	cmp	r0, #0	; 0x0
    8384:	12800098 	addne	r0, r0, #152	; 0x98
    8388:	e28dd004 	add	sp, sp, #4	; 0x4
    838c:	e8bd8000 	pop	{pc}

00008390 <print_mmc_devices>:
    8390:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8394:	e59f3068 	ldr	r3, [pc, #104]	; 8404 <print_mmc_devices+0x74>
    8398:	e24dd004 	sub	sp, sp, #4	; 0x4
    839c:	e1a05000 	mov	r5, r0
    83a0:	e1a07003 	mov	r7, r3
    83a4:	e5934000 	ldr	r4, [r3]
    83a8:	e59f6058 	ldr	r6, [pc, #88]	; 8408 <print_mmc_devices+0x78>
    83ac:	ea00000a 	b	83dc <print_mmc_devices+0x4c>
    83b0:	e594209c 	ldr	r2, [r4, #156]
    83b4:	e1a0e00f 	mov	lr, pc
    83b8:	e596f000 	ldr	pc, [r6]
    83bc:	e5943000 	ldr	r3, [r4]
    83c0:	e59f0044 	ldr	r0, [pc, #68]	; 840c <print_mmc_devices+0x7c>
    83c4:	e1530007 	cmp	r3, r7
    83c8:	e1a01005 	mov	r1, r5
    83cc:	0a000001 	beq	83d8 <print_mmc_devices+0x48>
    83d0:	e1a0e00f 	mov	lr, pc
    83d4:	e596f000 	ldr	pc, [r6]
    83d8:	e5944000 	ldr	r4, [r4]
    83dc:	e1540007 	cmp	r4, r7
    83e0:	e2841008 	add	r1, r4, #8	; 0x8
    83e4:	e59f0024 	ldr	r0, [pc, #36]	; 8410 <print_mmc_devices+0x80>
    83e8:	1afffff0 	bne	83b0 <print_mmc_devices+0x20>
    83ec:	e59f0020 	ldr	r0, [pc, #32]	; 8414 <print_mmc_devices+0x84>
    83f0:	e59f3010 	ldr	r3, [pc, #16]	; 8408 <print_mmc_devices+0x78>
    83f4:	e1a0e00f 	mov	lr, pc
    83f8:	e593f000 	ldr	pc, [r3]
    83fc:	e28dd004 	add	sp, sp, #4	; 0x4
    8400:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8404:	3c00d1d0 	.word	0x3c00d1d0
    8408:	3c00d694 	.word	0x3c00d694
    840c:	0001bc3d 	.word	0x0001bc3d
    8410:	0001bc36 	.word	0x0001bc36
    8414:	0001a882 	.word	0x0001a882

00008418 <get_mmc_num>:
    8418:	e59f3004 	ldr	r3, [pc, #4]	; 8424 <get_mmc_num+0xc>
    841c:	e5930000 	ldr	r0, [r3]
    8420:	e12fff1e 	bx	lr
    8424:	3c00d0b4 	.word	0x3c00d0b4

00008428 <mmc_vs_align>:
    8428:	e3a00c02 	mov	r0, #512	; 0x200
    842c:	e12fff1e 	bx	lr

00008430 <init_mmc>:
    8430:	e59f3060 	ldr	r3, [pc, #96]	; 8498 <init_mmc+0x68>
    8434:	e59f2060 	ldr	r2, [pc, #96]	; 849c <init_mmc+0x6c>
    8438:	e92d4070 	push	{r4, r5, r6, lr}
    843c:	e3a04000 	mov	r4, #0	; 0x0
    8440:	e3a00b02 	mov	r0, #2048	; 0x800
    8444:	e5833004 	str	r3, [r3, #4]
    8448:	e5824000 	str	r4, [r2]
    844c:	e5833000 	str	r3, [r3]
    8450:	ebffe4cb 	bl	1784 <malloc>
    8454:	e1a05000 	mov	r5, r0
    8458:	e2806b01 	add	r6, r0, #1024	; 0x400
    845c:	e1a00005 	mov	r0, r5
    8460:	e3a01000 	mov	r1, #0	; 0x0
    8464:	e3a02f4e 	mov	r2, #312	; 0x138
    8468:	ebffe458 	bl	15d0 <memset>
    846c:	e5856130 	str	r6, [r5, #304]
    8470:	e1a00004 	mov	r0, r4
    8474:	e1a01005 	mov	r1, r5
    8478:	eb000490 	bl	96c0 <imap_sdhc_initialize>
    847c:	e3500000 	cmp	r0, #0	; 0x0
    8480:	e2844001 	add	r4, r4, #1	; 0x1
    8484:	e2855f4e 	add	r5, r5, #312	; 0x138
    8488:	18bd8070 	popne	{r4, r5, r6, pc}
    848c:	e3540003 	cmp	r4, #3	; 0x3
    8490:	1afffff1 	bne	845c <init_mmc+0x2c>
    8494:	e8bd8070 	pop	{r4, r5, r6, pc}
    8498:	3c00d1d0 	.word	0x3c00d1d0
    849c:	3c00d0b4 	.word	0x3c00d0b4

000084a0 <sd_change_freq>:
    84a0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    84a4:	e5903050 	ldr	r3, [r0, #80]
    84a8:	e3a01000 	mov	r1, #0	; 0x0
    84ac:	e2132b01 	ands	r2, r3, #1024	; 0x400
    84b0:	e1a04000 	mov	r4, r0
    84b4:	e580104c 	str	r1, [r0, #76]
    84b8:	e24dd084 	sub	sp, sp, #132	; 0x84
    84bc:	11a00001 	movne	r0, r1
    84c0:	1a000070 	bne	8688 <sd_change_freq+0x1e8>
    84c4:	e1d438b0 	ldrh	r3, [r4, #128]
    84c8:	e3a05015 	mov	r5, #21	; 0x15
    84cc:	e1a03803 	lsl	r3, r3, #16
    84d0:	e58d3050 	str	r3, [sp, #80]
    84d4:	e28d6048 	add	r6, sp, #72	; 0x48
    84d8:	e3a03037 	mov	r3, #55	; 0x37
    84dc:	e1cd34b8 	strh	r3, [sp, #72]
    84e0:	e58d504c 	str	r5, [sp, #76]
    84e4:	e58d2064 	str	r2, [sp, #100]
    84e8:	e1a01006 	mov	r1, r6
    84ec:	e1a0e00f 	mov	lr, pc
    84f0:	e594f104 	ldr	pc, [r4, #260]
    84f4:	e3500000 	cmp	r0, #0	; 0x0
    84f8:	1a000062 	bne	8688 <sd_change_freq+0x1e8>
    84fc:	e3a03033 	mov	r3, #51	; 0x33
    8500:	e58d504c 	str	r5, [sp, #76]
    8504:	e1a09006 	mov	r9, r6
    8508:	e58d0064 	str	r0, [sp, #100]
    850c:	e1cd34b8 	strh	r3, [sp, #72]
    8510:	e58d0050 	str	r0, [sp, #80]
    8514:	e3a05003 	mov	r5, #3	; 0x3
    8518:	e28da078 	add	sl, sp, #120	; 0x78
    851c:	e3a08008 	mov	r8, #8	; 0x8
    8520:	e3a06001 	mov	r6, #1	; 0x1
    8524:	e28d7068 	add	r7, sp, #104	; 0x68
    8528:	e58da068 	str	sl, [sp, #104]
    852c:	e58d8074 	str	r8, [sp, #116]
    8530:	e58d6070 	str	r6, [sp, #112]
    8534:	e58d606c 	str	r6, [sp, #108]
    8538:	e1a00004 	mov	r0, r4
    853c:	e1a01009 	mov	r1, r9
    8540:	e1a02007 	mov	r2, r7
    8544:	e1a0e00f 	mov	lr, pc
    8548:	e594f104 	ldr	pc, [r4, #260]
    854c:	e250b000 	subs	fp, r0, #0	; 0x0
    8550:	0a000003 	beq	8564 <sd_change_freq+0xc4>
    8554:	e3550000 	cmp	r5, #0	; 0x0
    8558:	e2455001 	sub	r5, r5, #1	; 0x1
    855c:	1afffff1 	bne	8528 <sd_change_freq+0x88>
    8560:	ea000047 	b	8684 <sd_change_freq+0x1e4>
    8564:	e59d3078 	ldr	r3, [sp, #120]
    8568:	e59d007c 	ldr	r0, [sp, #124]
    856c:	e1a01c03 	lsl	r1, r3, #24
    8570:	e2032cff 	and	r2, r3, #65280	; 0xff00
    8574:	e1811c23 	orr	r1, r1, r3, lsr #24
    8578:	e1811402 	orr	r1, r1, r2, lsl #8
    857c:	e20338ff 	and	r3, r3, #16711680	; 0xff0000
    8580:	e1811423 	orr	r1, r1, r3, lsr #8
    8584:	e1a03c00 	lsl	r3, r0, #24
    8588:	e1833c20 	orr	r3, r3, r0, lsr #24
    858c:	e2002cff 	and	r2, r0, #65280	; 0xff00
    8590:	e1a0cc21 	lsr	ip, r1, #24
    8594:	e1833402 	orr	r3, r3, r2, lsl #8
    8598:	e20008ff 	and	r0, r0, #16711680	; 0xff0000
    859c:	e20cc00f 	and	ip, ip, #15	; 0xf
    85a0:	e1833420 	orr	r3, r3, r0, lsr #8
    85a4:	e35c0001 	cmp	ip, #1	; 0x1
    85a8:	e584305c 	str	r3, [r4, #92]
    85ac:	e5841058 	str	r1, [r4, #88]
    85b0:	059f30d8 	ldreq	r3, [pc, #216]	; 8690 <sd_change_freq+0x1f0>
    85b4:	0a000004 	beq	85cc <sd_change_freq+0x12c>
    85b8:	3a000002 	bcc	85c8 <sd_change_freq+0x128>
    85bc:	e35c0002 	cmp	ip, #2	; 0x2
    85c0:	059f30cc 	ldreq	r3, [pc, #204]	; 8694 <sd_change_freq+0x1f4>
    85c4:	0a000000 	beq	85cc <sd_change_freq+0x12c>
    85c8:	e59f30c8 	ldr	r3, [pc, #200]	; 8698 <sd_change_freq+0x1f8>
    85cc:	e5843030 	str	r3, [r4, #48]
    85d0:	e5943058 	ldr	r3, [r4, #88]
    85d4:	e5942030 	ldr	r2, [r4, #48]
    85d8:	e3130701 	tst	r3, #262144	; 0x40000
    85dc:	1594304c 	ldrne	r3, [r4, #76]
    85e0:	13833c01 	orrne	r3, r3, #256	; 0x100
    85e4:	1584304c 	strne	r3, [r4, #76]
    85e8:	e59f30a8 	ldr	r3, [pc, #168]	; 8698 <sd_change_freq+0x1f8>
    85ec:	e1520003 	cmp	r2, r3
    85f0:	13a05004 	movne	r5, #4	; 0x4
    85f4:	128d6008 	addne	r6, sp, #8	; 0x8
    85f8:	1a000007 	bne	861c <sd_change_freq+0x17c>
    85fc:	ea000020 	b	8684 <sd_change_freq+0x1e4>
    8600:	e58d6000 	str	r6, [sp]
    8604:	ebfffee8 	bl	81ac <sd_switch>
    8608:	e3500000 	cmp	r0, #0	; 0x0
    860c:	1a00001d 	bne	8688 <sd_change_freq+0x1e8>
    8610:	e59d3024 	ldr	r3, [sp, #36]
    8614:	e3130c02 	tst	r3, #512	; 0x200
    8618:	0a000005 	beq	8634 <sd_change_freq+0x194>
    861c:	e3a01000 	mov	r1, #0	; 0x0
    8620:	e2555001 	subs	r5, r5, #1	; 0x1
    8624:	e1a00004 	mov	r0, r4
    8628:	e1a02001 	mov	r2, r1
    862c:	e3a03001 	mov	r3, #1	; 0x1
    8630:	2afffff2 	bcs	8600 <sd_change_freq+0x160>
    8634:	e59d3014 	ldr	r3, [sp, #20]
    8638:	e3130c02 	tst	r3, #512	; 0x200
    863c:	0a000010 	beq	8684 <sd_change_freq+0x1e4>
    8640:	e3a01001 	mov	r1, #1	; 0x1
    8644:	e28dc008 	add	ip, sp, #8	; 0x8
    8648:	e1a00004 	mov	r0, r4
    864c:	e3a02000 	mov	r2, #0	; 0x0
    8650:	e1a03001 	mov	r3, r1
    8654:	e58dc000 	str	ip, [sp]
    8658:	ebfffed3 	bl	81ac <sd_switch>
    865c:	e3500000 	cmp	r0, #0	; 0x0
    8660:	1a000008 	bne	8688 <sd_change_freq+0x1e8>
    8664:	e59d3018 	ldr	r3, [sp, #24]
    8668:	e1a03c03 	lsl	r3, r3, #24
    866c:	e203340f 	and	r3, r3, #251658240	; 0xf000000
    8670:	e3530401 	cmp	r3, #16777216	; 0x1000000
    8674:	0594304c 	ldreq	r3, [r4, #76]
    8678:	03833001 	orreq	r3, r3, #1	; 0x1
    867c:	0584304c 	streq	r3, [r4, #76]
    8680:	ea000000 	b	8688 <sd_change_freq+0x1e8>
    8684:	e1a0000b 	mov	r0, fp
    8688:	e28dd084 	add	sp, sp, #132	; 0x84
    868c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    8690:	0002001a 	.word	0x0002001a
    8694:	00020020 	.word	0x00020020
    8698:	00020010 	.word	0x00020010

0000869c <sd_send_op_cond>:
    869c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    86a0:	e59f9140 	ldr	r9, [pc, #320]	; 87e8 <sd_send_op_cond+0x14c>
    86a4:	e24dd020 	sub	sp, sp, #32	; 0x20
    86a8:	e1a04000 	mov	r4, r0
    86ac:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    86b0:	e3a0a015 	mov	sl, #21	; 0x15
    86b4:	e3a06000 	mov	r6, #0	; 0x0
    86b8:	e1a0700d 	mov	r7, sp
    86bc:	e3a08001 	mov	r8, #1	; 0x1
    86c0:	e3a03037 	mov	r3, #55	; 0x37
    86c4:	e1cd30b0 	strh	r3, [sp]
    86c8:	e1a0100d 	mov	r1, sp
    86cc:	e1a02006 	mov	r2, r6
    86d0:	e58da004 	str	sl, [sp, #4]
    86d4:	e58d6008 	str	r6, [sp, #8]
    86d8:	e58d601c 	str	r6, [sp, #28]
    86dc:	e1a00004 	mov	r0, r4
    86e0:	e1a0e00f 	mov	lr, pc
    86e4:	e594f104 	ldr	pc, [r4, #260]
    86e8:	e2503000 	subs	r3, r0, #0	; 0x0
    86ec:	e1a0100d 	mov	r1, sp
    86f0:	e3a02000 	mov	r2, #0	; 0x0
    86f4:	e59fe0f0 	ldr	lr, [pc, #240]	; 87ec <sd_send_op_cond+0x150>
    86f8:	e1a00004 	mov	r0, r4
    86fc:	1a000036 	bne	87dc <sd_send_op_cond+0x140>
    8700:	e594302c 	ldr	r3, [r4, #44]
    8704:	e594c030 	ldr	ip, [r4, #48]
    8708:	e003e00e 	and	lr, r3, lr
    870c:	e15c0009 	cmp	ip, r9
    8710:	038e3101 	orreq	r3, lr, #1073741824	; 0x40000000
    8714:	e58de008 	str	lr, [sp, #8]
    8718:	058d3008 	streq	r3, [sp, #8]
    871c:	e3a03029 	mov	r3, #41	; 0x29
    8720:	e1cd30b0 	strh	r3, [sp]
    8724:	e58d8004 	str	r8, [sp, #4]
    8728:	e5943104 	ldr	r3, [r4, #260]
    872c:	e12fff33 	blx	r3
    8730:	e2503000 	subs	r3, r0, #0	; 0x0
    8734:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    8738:	1a000027 	bne	87dc <sd_send_op_cond+0x140>
    873c:	eb000b74 	bl	b514 <udelay>
    8740:	e59d300c 	ldr	r3, [sp, #12]
    8744:	e3530000 	cmp	r3, #0	; 0x0
    8748:	ba000002 	blt	8758 <sd_send_op_cond+0xbc>
    874c:	e2555001 	subs	r5, r5, #1	; 0x1
    8750:	2affffda 	bcs	86c0 <sd_send_op_cond+0x24>
    8754:	ea00001f 	b	87d8 <sd_send_op_cond+0x13c>
    8758:	e3550000 	cmp	r5, #0	; 0x0
    875c:	da00001d 	ble	87d8 <sd_send_op_cond+0x13c>
    8760:	e59f3080 	ldr	r3, [pc, #128]	; 87e8 <sd_send_op_cond+0x14c>
    8764:	e5942030 	ldr	r2, [r4, #48]
    8768:	e1520003 	cmp	r2, r3
    876c:	12433010 	subne	r3, r3, #16	; 0x10
    8770:	15843030 	strne	r3, [r4, #48]
    8774:	e5943050 	ldr	r3, [r4, #80]
    8778:	e3130b01 	tst	r3, #1024	; 0x400
    877c:	0a00000c 	beq	87b4 <sd_send_op_cond+0x118>
    8780:	e3a03001 	mov	r3, #1	; 0x1
    8784:	e3a02000 	mov	r2, #0	; 0x0
    8788:	e58d3004 	str	r3, [sp, #4]
    878c:	e3a0303a 	mov	r3, #58	; 0x3a
    8790:	e1cd30b0 	strh	r3, [sp]
    8794:	e58d2008 	str	r2, [sp, #8]
    8798:	e58d201c 	str	r2, [sp, #28]
    879c:	e1a00004 	mov	r0, r4
    87a0:	e1a0100d 	mov	r1, sp
    87a4:	e1a0e00f 	mov	lr, pc
    87a8:	e594f104 	ldr	pc, [r4, #260]
    87ac:	e2503000 	subs	r3, r0, #0	; 0x0
    87b0:	1a000009 	bne	87dc <sd_send_op_cond+0x140>
    87b4:	e59d200c 	ldr	r2, [sp, #12]
    87b8:	e3a03000 	mov	r3, #0	; 0x0
    87bc:	e1c438b0 	strh	r3, [r4, #128]
    87c0:	e1a03f22 	lsr	r3, r2, #30
    87c4:	e2033001 	and	r3, r3, #1	; 0x1
    87c8:	e5843040 	str	r3, [r4, #64]
    87cc:	e5842054 	str	r2, [r4, #84]
    87d0:	e3a03000 	mov	r3, #0	; 0x0
    87d4:	ea000000 	b	87dc <sd_send_op_cond+0x140>
    87d8:	e3e03010 	mvn	r3, #16	; 0x10
    87dc:	e1a00003 	mov	r0, r3
    87e0:	e28dd020 	add	sp, sp, #32	; 0x20
    87e4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    87e8:	00020020 	.word	0x00020020
    87ec:	00ff8000 	.word	0x00ff8000

000087f0 <mmc_go_idle>:
    87f0:	e92d4010 	push	{r4, lr}
    87f4:	e1a04000 	mov	r4, r0
    87f8:	e24dd020 	sub	sp, sp, #32	; 0x20
    87fc:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    8800:	eb000b43 	bl	b514 <udelay>
    8804:	e3a02000 	mov	r2, #0	; 0x0
    8808:	e1a00004 	mov	r0, r4
    880c:	e1cd20b0 	strh	r2, [sp]
    8810:	e58d2008 	str	r2, [sp, #8]
    8814:	e58d2004 	str	r2, [sp, #4]
    8818:	e58d201c 	str	r2, [sp, #28]
    881c:	e1a0100d 	mov	r1, sp
    8820:	e1a0e00f 	mov	lr, pc
    8824:	e594f104 	ldr	pc, [r4, #260]
    8828:	e2504000 	subs	r4, r0, #0	; 0x0
    882c:	1a000001 	bne	8838 <mmc_go_idle+0x48>
    8830:	e2800e7d 	add	r0, r0, #2000	; 0x7d0
    8834:	eb000b36 	bl	b514 <udelay>
    8838:	e1a00004 	mov	r0, r4
    883c:	e28dd020 	add	sp, sp, #32	; 0x20
    8840:	e8bd8010 	pop	{r4, pc}

00008844 <mmc_send_op_cond>:
    8844:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    8848:	e24dd020 	sub	sp, sp, #32	; 0x20
    884c:	e1a05000 	mov	r5, r0
    8850:	e3a06001 	mov	r6, #1	; 0x1
    8854:	ebffffe5 	bl	87f0 <mmc_go_idle>
    8858:	e3a02000 	mov	r2, #0	; 0x0
    885c:	e1cd60b0 	strh	r6, [sp]
    8860:	e58d6004 	str	r6, [sp, #4]
    8864:	e58d2008 	str	r2, [sp, #8]
    8868:	e58d201c 	str	r2, [sp, #28]
    886c:	e1a00005 	mov	r0, r5
    8870:	e1a0100d 	mov	r1, sp
    8874:	e1a0e00f 	mov	lr, pc
    8878:	e595f104 	ldr	pc, [r5, #260]
    887c:	e2504000 	subs	r4, r0, #0	; 0x0
    8880:	e1a0700d 	mov	r7, sp
    8884:	1a000039 	bne	8970 <mmc_send_op_cond+0x12c>
    8888:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    888c:	eb000b20 	bl	b514 <udelay>
    8890:	e1a08006 	mov	r8, r6
    8894:	e1a0700d 	mov	r7, sp
    8898:	e1a06004 	mov	r6, r4
    889c:	e3024710 	movw	r4, #10000	; 0x2710
    88a0:	e595302c 	ldr	r3, [r5, #44]
    88a4:	e3a02001 	mov	r2, #1	; 0x1
    88a8:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    88ac:	e1cd20b0 	strh	r2, [sp]
    88b0:	e58d3008 	str	r3, [sp, #8]
    88b4:	e1a02006 	mov	r2, r6
    88b8:	e58d8004 	str	r8, [sp, #4]
    88bc:	e58d601c 	str	r6, [sp, #28]
    88c0:	e1a0100d 	mov	r1, sp
    88c4:	e1a00005 	mov	r0, r5
    88c8:	e1a0e00f 	mov	lr, pc
    88cc:	e595f104 	ldr	pc, [r5, #260]
    88d0:	e2502000 	subs	r2, r0, #0	; 0x0
    88d4:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    88d8:	11a04002 	movne	r4, r2
    88dc:	1a000023 	bne	8970 <mmc_send_op_cond+0x12c>
    88e0:	eb000b0b 	bl	b514 <udelay>
    88e4:	e59d300c 	ldr	r3, [sp, #12]
    88e8:	e3530000 	cmp	r3, #0	; 0x0
    88ec:	ba000002 	blt	88fc <mmc_send_op_cond+0xb8>
    88f0:	e2544001 	subs	r4, r4, #1	; 0x1
    88f4:	2affffe9 	bcs	88a0 <mmc_send_op_cond+0x5c>
    88f8:	ea00001b 	b	896c <mmc_send_op_cond+0x128>
    88fc:	e3540000 	cmp	r4, #0	; 0x0
    8900:	da000019 	ble	896c <mmc_send_op_cond+0x128>
    8904:	e5953050 	ldr	r3, [r5, #80]
    8908:	e3130b01 	tst	r3, #1024	; 0x400
    890c:	0a00000b 	beq	8940 <mmc_send_op_cond+0xfc>
    8910:	e3a0303a 	mov	r3, #58	; 0x3a
    8914:	e58d8004 	str	r8, [sp, #4]
    8918:	e1cd30b0 	strh	r3, [sp]
    891c:	e58d6008 	str	r6, [sp, #8]
    8920:	e58d601c 	str	r6, [sp, #28]
    8924:	e1a0100d 	mov	r1, sp
    8928:	e1a02006 	mov	r2, r6
    892c:	e1a00005 	mov	r0, r5
    8930:	e1a0e00f 	mov	lr, pc
    8934:	e595f104 	ldr	pc, [r5, #260]
    8938:	e2504000 	subs	r4, r0, #0	; 0x0
    893c:	1a00000b 	bne	8970 <mmc_send_op_cond+0x12c>
    8940:	e59d300c 	ldr	r3, [sp, #12]
    8944:	e3a01801 	mov	r1, #65536	; 0x10000
    8948:	e1a02f23 	lsr	r2, r3, #30
    894c:	e2022001 	and	r2, r2, #1	; 0x1
    8950:	e5852040 	str	r2, [r5, #64]
    8954:	e3a02000 	mov	r2, #0	; 0x0
    8958:	e5851030 	str	r1, [r5, #48]
    895c:	e1c528b0 	strh	r2, [r5, #128]
    8960:	e5853054 	str	r3, [r5, #84]
    8964:	e3a04000 	mov	r4, #0	; 0x0
    8968:	ea000000 	b	8970 <mmc_send_op_cond+0x12c>
    896c:	e3e04010 	mvn	r4, #16	; 0x10
    8970:	e1a00004 	mov	r0, r4
    8974:	e28dd020 	add	sp, sp, #32	; 0x20
    8978:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000897c <mmc_send_status>:
    897c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8980:	e1d038b0 	ldrh	r3, [r0, #128]
    8984:	e24dd024 	sub	sp, sp, #36	; 0x24
    8988:	e1a03803 	lsl	r3, r3, #16
    898c:	e58d3008 	str	r3, [sp, #8]
    8990:	e3a03000 	mov	r3, #0	; 0x0
    8994:	e3a02015 	mov	r2, #21	; 0x15
    8998:	e58d301c 	str	r3, [sp, #28]
    899c:	e3a0300d 	mov	r3, #13	; 0xd
    89a0:	e1a06000 	mov	r6, r0
    89a4:	e58d2004 	str	r2, [sp, #4]
    89a8:	e1a04001 	mov	r4, r1
    89ac:	e1cd30b0 	strh	r3, [sp]
    89b0:	e1a0700d 	mov	r7, sp
    89b4:	e1a0100d 	mov	r1, sp
    89b8:	e3a02000 	mov	r2, #0	; 0x0
    89bc:	e1a00006 	mov	r0, r6
    89c0:	e1a0e00f 	mov	lr, pc
    89c4:	e596f104 	ldr	pc, [r6, #260]
    89c8:	e2505000 	subs	r5, r0, #0	; 0x0
    89cc:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    89d0:	1a000018 	bne	8a38 <mmc_send_status+0xbc>
    89d4:	e59d300c 	ldr	r3, [sp, #12]
    89d8:	e3130c01 	tst	r3, #256	; 0x100
    89dc:	1a00000e 	bne	8a1c <mmc_send_status+0xa0>
    89e0:	eb000acb 	bl	b514 <udelay>
    89e4:	e59d100c 	ldr	r1, [sp, #12]
    89e8:	e59f3054 	ldr	r3, [pc, #84]	; 8a44 <mmc_send_status+0xc8>
    89ec:	e2444001 	sub	r4, r4, #1	; 0x1
    89f0:	e0013003 	and	r3, r1, r3
    89f4:	e3530000 	cmp	r3, #0	; 0x0
    89f8:	0a000005 	beq	8a14 <mmc_send_status+0x98>
    89fc:	e59f0044 	ldr	r0, [pc, #68]	; 8a48 <mmc_send_status+0xcc>
    8a00:	e59f3044 	ldr	r3, [pc, #68]	; 8a4c <mmc_send_status+0xd0>
    8a04:	e1a0e00f 	mov	lr, pc
    8a08:	e593f000 	ldr	pc, [r3]
    8a0c:	e3e05011 	mvn	r5, #17	; 0x11
    8a10:	ea000008 	b	8a38 <mmc_send_status+0xbc>
    8a14:	e3740001 	cmn	r4, #1	; 0x1
    8a18:	1affffe5 	bne	89b4 <mmc_send_status+0x38>
    8a1c:	e3540000 	cmp	r4, #0	; 0x0
    8a20:	1a000004 	bne	8a38 <mmc_send_status+0xbc>
    8a24:	e59f0024 	ldr	r0, [pc, #36]	; 8a50 <mmc_send_status+0xd4>
    8a28:	e59f301c 	ldr	r3, [pc, #28]	; 8a4c <mmc_send_status+0xd0>
    8a2c:	e1a0e00f 	mov	lr, pc
    8a30:	e593f000 	ldr	pc, [r3]
    8a34:	e3e05012 	mvn	r5, #18	; 0x12
    8a38:	e1a00005 	mov	r0, r5
    8a3c:	e28dd024 	add	sp, sp, #36	; 0x24
    8a40:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8a44:	fdf94080 	.word	0xfdf94080
    8a48:	0001bc41 	.word	0x0001bc41
    8a4c:	3c00d694 	.word	0x3c00d694
    8a50:	0001bc57 	.word	0x0001bc57

00008a54 <mmc_bwrite>:
    8a54:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8a58:	e24dd034 	sub	sp, sp, #52	; 0x34
    8a5c:	e1a06001 	mov	r6, r1
    8a60:	e1a0b002 	mov	fp, r2
    8a64:	e1a0a003 	mov	sl, r3
    8a68:	ebfffda2 	bl	80f8 <find_mmc_device>
    8a6c:	e2504000 	subs	r4, r0, #0	; 0x0
    8a70:	0a000060 	beq	8bf8 <mmc_bwrite+0x1a4>
    8a74:	e594108c 	ldr	r1, [r4, #140]
    8a78:	ebfffd90 	bl	80c0 <mmc_set_blocklen>
    8a7c:	e3500000 	cmp	r0, #0	; 0x0
    8a80:	01a09000 	moveq	r9, r0
    8a84:	01a0700b 	moveq	r7, fp
    8a88:	0a000005 	beq	8aa4 <mmc_bwrite+0x50>
    8a8c:	e594108c 	ldr	r1, [r4, #140]
    8a90:	e59f016c 	ldr	r0, [pc, #364]	; 8c04 <mmc_bwrite+0x1b0>
    8a94:	e59f316c 	ldr	r3, [pc, #364]	; 8c08 <mmc_bwrite+0x1b4>
    8a98:	e1a0e00f 	mov	lr, pc
    8a9c:	e593f000 	ldr	pc, [r3]
    8aa0:	ea000054 	b	8bf8 <mmc_bwrite+0x1a4>
    8aa4:	e5943110 	ldr	r3, [r4, #272]
    8aa8:	e59420a8 	ldr	r2, [r4, #168]
    8aac:	e1570003 	cmp	r7, r3
    8ab0:	31a05007 	movcc	r5, r7
    8ab4:	21a05003 	movcs	r5, r3
    8ab8:	e0858006 	add	r8, r5, r6
    8abc:	e1580002 	cmp	r8, r2
    8ac0:	9a000005 	bls	8adc <mmc_bwrite+0x88>
    8ac4:	e59f0140 	ldr	r0, [pc, #320]	; 8c0c <mmc_bwrite+0x1b8>
    8ac8:	e1a01008 	mov	r1, r8
    8acc:	e59f3134 	ldr	r3, [pc, #308]	; 8c08 <mmc_bwrite+0x1b4>
    8ad0:	e1a0e00f 	mov	lr, pc
    8ad4:	e593f000 	ldr	pc, [r3]
    8ad8:	ea00003d 	b	8bd4 <mmc_bwrite+0x180>
    8adc:	e3550001 	cmp	r5, #1	; 0x1
    8ae0:	93a03018 	movls	r3, #24	; 0x18
    8ae4:	91cd30b0 	strhls	r3, [sp]
    8ae8:	e5943040 	ldr	r3, [r4, #64]
    8aec:	83a02019 	movhi	r2, #25	; 0x19
    8af0:	81cd20b0 	strhhi	r2, [sp]
    8af4:	e3530000 	cmp	r3, #0	; 0x0
    8af8:	0594308c 	ldreq	r3, [r4, #140]
    8afc:	e3a02015 	mov	r2, #21	; 0x15
    8b00:	00030693 	muleq	r3, r3, r6
    8b04:	158d6008 	strne	r6, [sp, #8]
    8b08:	058d3008 	streq	r3, [sp, #8]
    8b0c:	e58d2004 	str	r2, [sp, #4]
    8b10:	e58d901c 	str	r9, [sp, #28]
    8b14:	e58da020 	str	sl, [sp, #32]
    8b18:	e58d5028 	str	r5, [sp, #40]
    8b1c:	e594308c 	ldr	r3, [r4, #140]
    8b20:	e3a02002 	mov	r2, #2	; 0x2
    8b24:	e58d2024 	str	r2, [sp, #36]
    8b28:	e58d302c 	str	r3, [sp, #44]
    8b2c:	e1a00004 	mov	r0, r4
    8b30:	e1a0100d 	mov	r1, sp
    8b34:	e28d2020 	add	r2, sp, #32	; 0x20
    8b38:	e1a0e00f 	mov	lr, pc
    8b3c:	e594f104 	ldr	pc, [r4, #260]
    8b40:	e3500000 	cmp	r0, #0	; 0x0
    8b44:	0a000004 	beq	8b5c <mmc_bwrite+0x108>
    8b48:	e59f00c0 	ldr	r0, [pc, #192]	; 8c10 <mmc_bwrite+0x1bc>
    8b4c:	e59f30b4 	ldr	r3, [pc, #180]	; 8c08 <mmc_bwrite+0x1b4>
    8b50:	e1a0e00f 	mov	lr, pc
    8b54:	e593f000 	ldr	pc, [r3]
    8b58:	ea00001d 	b	8bd4 <mmc_bwrite+0x180>
    8b5c:	e5943050 	ldr	r3, [r4, #80]
    8b60:	e1a03523 	lsr	r3, r3, #10
    8b64:	e2233001 	eor	r3, r3, #1	; 0x1
    8b68:	e3550001 	cmp	r5, #1	; 0x1
    8b6c:	93a03000 	movls	r3, #0	; 0x0
    8b70:	82033001 	andhi	r3, r3, #1	; 0x1
    8b74:	e3530000 	cmp	r3, #0	; 0x0
    8b78:	0a000011 	beq	8bc4 <mmc_bwrite+0x170>
    8b7c:	e3a0200c 	mov	r2, #12	; 0xc
    8b80:	e3a0301d 	mov	r3, #29	; 0x1d
    8b84:	e1cd20b0 	strh	r2, [sp]
    8b88:	e98d0208 	stmib	sp, {r3, r9}
    8b8c:	e58d901c 	str	r9, [sp, #28]
    8b90:	e1a00004 	mov	r0, r4
    8b94:	e1a0100d 	mov	r1, sp
    8b98:	e1a02009 	mov	r2, r9
    8b9c:	e1a0e00f 	mov	lr, pc
    8ba0:	e594f104 	ldr	pc, [r4, #260]
    8ba4:	e3500000 	cmp	r0, #0	; 0x0
    8ba8:	0a000005 	beq	8bc4 <mmc_bwrite+0x170>
    8bac:	e59f0060 	ldr	r0, [pc, #96]	; 8c14 <mmc_bwrite+0x1c0>
    8bb0:	e59f3050 	ldr	r3, [pc, #80]	; 8c08 <mmc_bwrite+0x1b4>
    8bb4:	e1a0e00f 	mov	lr, pc
    8bb8:	e593f000 	ldr	pc, [r3]
    8bbc:	e1a00009 	mov	r0, r9
    8bc0:	ea00000d 	b	8bfc <mmc_bwrite+0x1a8>
    8bc4:	e1a00004 	mov	r0, r4
    8bc8:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8bcc:	ebffff6a 	bl	897c <mmc_send_status>
    8bd0:	ea000001 	b	8bdc <mmc_bwrite+0x188>
    8bd4:	e3550000 	cmp	r5, #0	; 0x0
    8bd8:	1a000006 	bne	8bf8 <mmc_bwrite+0x1a4>
    8bdc:	e0577005 	subs	r7, r7, r5
    8be0:	e594008c 	ldr	r0, [r4, #140]
    8be4:	102aa590 	mlane	sl, r0, r5, sl
    8be8:	11a06008 	movne	r6, r8
    8bec:	1affffac 	bne	8aa4 <mmc_bwrite+0x50>
    8bf0:	e000009b 	mul	r0, fp, r0
    8bf4:	ea000000 	b	8bfc <mmc_bwrite+0x1a8>
    8bf8:	e3a00000 	mov	r0, #0	; 0x0
    8bfc:	e28dd034 	add	sp, sp, #52	; 0x34
    8c00:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    8c04:	0001bc73 	.word	0x0001bc73
    8c08:	3c00d694 	.word	0x3c00d694
    8c0c:	0001bc7d 	.word	0x0001bc7d
    8c10:	0001bca9 	.word	0x0001bca9
    8c14:	0001bcbb 	.word	0x0001bcbb

00008c18 <mmc2_vs_write>:
    8c18:	e1a024a2 	lsr	r2, r2, #9
    8c1c:	e1821b83 	orr	r1, r2, r3, lsl #23
    8c20:	e59d2000 	ldr	r2, [sp]
    8c24:	e1a03000 	mov	r3, r0
    8c28:	e1a024c2 	asr	r2, r2, #9
    8c2c:	e3a00002 	mov	r0, #2	; 0x2
    8c30:	eaffff87 	b	8a54 <mmc_bwrite>

00008c34 <mmc1_vs_write>:
    8c34:	e1a024a2 	lsr	r2, r2, #9
    8c38:	e1821b83 	orr	r1, r2, r3, lsl #23
    8c3c:	e59d2000 	ldr	r2, [sp]
    8c40:	e1a03000 	mov	r3, r0
    8c44:	e1a024c2 	asr	r2, r2, #9
    8c48:	e3a00001 	mov	r0, #1	; 0x1
    8c4c:	eaffff80 	b	8a54 <mmc_bwrite>

00008c50 <mmc0_vs_write>:
    8c50:	e1a024a2 	lsr	r2, r2, #9
    8c54:	e1821b83 	orr	r1, r2, r3, lsl #23
    8c58:	e59d2000 	ldr	r2, [sp]
    8c5c:	e1a03000 	mov	r3, r0
    8c60:	e1a024c2 	asr	r2, r2, #9
    8c64:	e3a00000 	mov	r0, #0	; 0x0
    8c68:	eaffff79 	b	8a54 <mmc_bwrite>

00008c6c <mmc_switch>:
    8c6c:	e92d4030 	push	{r4, r5, lr}
    8c70:	e1a02802 	lsl	r2, r2, #16
    8c74:	e3822403 	orr	r2, r2, #50331648	; 0x3000000
    8c78:	e24dd024 	sub	sp, sp, #36	; 0x24
    8c7c:	e1822403 	orr	r2, r2, r3, lsl #8
    8c80:	e3a0301d 	mov	r3, #29	; 0x1d
    8c84:	e3a01000 	mov	r1, #0	; 0x0
    8c88:	e58d3004 	str	r3, [sp, #4]
    8c8c:	e3a03006 	mov	r3, #6	; 0x6
    8c90:	e58d2008 	str	r2, [sp, #8]
    8c94:	e1cd30b0 	strh	r3, [sp]
    8c98:	e1a02001 	mov	r2, r1
    8c9c:	e58d101c 	str	r1, [sp, #28]
    8ca0:	e1a0100d 	mov	r1, sp
    8ca4:	e1a04000 	mov	r4, r0
    8ca8:	e1a0e00f 	mov	lr, pc
    8cac:	e590f104 	ldr	pc, [r0, #260]
    8cb0:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8cb4:	e1a05000 	mov	r5, r0
    8cb8:	e1a00004 	mov	r0, r4
    8cbc:	ebffff2e 	bl	897c <mmc_send_status>
    8cc0:	e1a00005 	mov	r0, r5
    8cc4:	e28dd024 	add	sp, sp, #36	; 0x24
    8cc8:	e8bd8030 	pop	{r4, r5, pc}

00008ccc <mmc_change_freq>:
    8ccc:	e92d4070 	push	{r4, r5, r6, lr}
    8cd0:	e5903050 	ldr	r3, [r0, #80]
    8cd4:	e24ddc02 	sub	sp, sp, #512	; 0x200
    8cd8:	e3130b01 	tst	r3, #1024	; 0x400
    8cdc:	e3a03000 	mov	r3, #0	; 0x0
    8ce0:	e1a04000 	mov	r4, r0
    8ce4:	e580304c 	str	r3, [r0, #76]
    8ce8:	1a000020 	bne	8d70 <mmc_change_freq+0xa4>
    8cec:	e5902030 	ldr	r2, [r0, #48]
    8cf0:	e59f3084 	ldr	r3, [pc, #132]	; 8d7c <mmc_change_freq+0xb0>
    8cf4:	e1520003 	cmp	r2, r3
    8cf8:	9a00001c 	bls	8d70 <mmc_change_freq+0xa4>
    8cfc:	e3a03c01 	mov	r3, #256	; 0x100
    8d00:	e580304c 	str	r3, [r0, #76]
    8d04:	e1a0100d 	mov	r1, sp
    8d08:	ebfffd12 	bl	8158 <mmc_send_ext_csd>
    8d0c:	e3500000 	cmp	r0, #0	; 0x0
    8d10:	e1a0600d 	mov	r6, sp
    8d14:	1a000016 	bne	8d74 <mmc_change_freq+0xa8>
    8d18:	e3a01001 	mov	r1, #1	; 0x1
    8d1c:	e1a00004 	mov	r0, r4
    8d20:	e3a020b9 	mov	r2, #185	; 0xb9
    8d24:	e1a03001 	mov	r3, r1
    8d28:	e5dd50c4 	ldrb	r5, [sp, #196]
    8d2c:	ebffffce 	bl	8c6c <mmc_switch>
    8d30:	e3500000 	cmp	r0, #0	; 0x0
    8d34:	1a00000e 	bne	8d74 <mmc_change_freq+0xa8>
    8d38:	e1a0100d 	mov	r1, sp
    8d3c:	e1a00004 	mov	r0, r4
    8d40:	ebfffd04 	bl	8158 <mmc_send_ext_csd>
    8d44:	e3500000 	cmp	r0, #0	; 0x0
    8d48:	1a000009 	bne	8d74 <mmc_change_freq+0xa8>
    8d4c:	e5dd30b9 	ldrb	r3, [sp, #185]
    8d50:	e3530000 	cmp	r3, #0	; 0x0
    8d54:	0a000006 	beq	8d74 <mmc_change_freq+0xa8>
    8d58:	e594304c 	ldr	r3, [r4, #76]
    8d5c:	e3150002 	tst	r5, #2	; 0x2
    8d60:	13833011 	orrne	r3, r3, #17	; 0x11
    8d64:	03833001 	orreq	r3, r3, #1	; 0x1
    8d68:	e584304c 	str	r3, [r4, #76]
    8d6c:	ea000000 	b	8d74 <mmc_change_freq+0xa8>
    8d70:	e3a00000 	mov	r0, #0	; 0x0
    8d74:	e28ddc02 	add	sp, sp, #512	; 0x200
    8d78:	e8bd8070 	pop	{r4, r5, r6, pc}
    8d7c:	0001003f 	.word	0x0001003f

00008d80 <mmc_startup>:
    8d80:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    8d84:	e5903050 	ldr	r3, [r0, #80]
    8d88:	e24ddf8e 	sub	sp, sp, #568	; 0x238
    8d8c:	e3130b01 	tst	r3, #1024	; 0x400
    8d90:	13a0300a 	movne	r3, #10	; 0xa
    8d94:	03a03002 	moveq	r3, #2	; 0x2
    8d98:	e28d1c02 	add	r1, sp, #512	; 0x200
    8d9c:	e3a02000 	mov	r2, #0	; 0x0
    8da0:	e1c131b0 	strh	r3, [r1, #16]
    8da4:	e28d5e21 	add	r5, sp, #528	; 0x210
    8da8:	e3a03007 	mov	r3, #7	; 0x7
    8dac:	e58d3214 	str	r3, [sp, #532]
    8db0:	e58d2218 	str	r2, [sp, #536]
    8db4:	e58d222c 	str	r2, [sp, #556]
    8db8:	e1a01005 	mov	r1, r5
    8dbc:	e1a06000 	mov	r6, r0
    8dc0:	e1a0e00f 	mov	lr, pc
    8dc4:	e590f104 	ldr	pc, [r0, #260]
    8dc8:	e2504000 	subs	r4, r0, #0	; 0x0
    8dcc:	1a000159 	bne	9338 <mmc_startup+0x5b8>
    8dd0:	e3a02010 	mov	r2, #16	; 0x10
    8dd4:	e2860070 	add	r0, r6, #112	; 0x70
    8dd8:	e285100c 	add	r1, r5, #12	; 0xc
    8ddc:	ebffe203 	bl	15f0 <memcpy>
    8de0:	e5963050 	ldr	r3, [r6, #80]
    8de4:	e2132b01 	ands	r2, r3, #1024	; 0x400
    8de8:	1a000013 	bne	8e3c <mmc_startup+0xbc>
    8dec:	e1d638b0 	ldrh	r3, [r6, #128]
    8df0:	e3a00003 	mov	r0, #3	; 0x3
    8df4:	e1a03803 	lsl	r3, r3, #16
    8df8:	e58d3218 	str	r3, [sp, #536]
    8dfc:	e3a03015 	mov	r3, #21	; 0x15
    8e00:	e58d3214 	str	r3, [sp, #532]
    8e04:	e28d3c02 	add	r3, sp, #512	; 0x200
    8e08:	e1c301b0 	strh	r0, [r3, #16]
    8e0c:	e58d222c 	str	r2, [sp, #556]
    8e10:	e1a01005 	mov	r1, r5
    8e14:	e1a00006 	mov	r0, r6
    8e18:	e1a0e00f 	mov	lr, pc
    8e1c:	e596f104 	ldr	pc, [r6, #260]
    8e20:	e2504000 	subs	r4, r0, #0	; 0x0
    8e24:	1a000143 	bne	9338 <mmc_startup+0x5b8>
    8e28:	e5963030 	ldr	r3, [r6, #48]
    8e2c:	e3130802 	tst	r3, #131072	; 0x20000
    8e30:	128d1c02 	addne	r1, sp, #512	; 0x200
    8e34:	11d111be 	ldrhne	r1, [r1, #30]
    8e38:	11c618b0 	strhne	r1, [r6, #128]
    8e3c:	e1d638b0 	ldrh	r3, [r6, #128]
    8e40:	e3a01000 	mov	r1, #0	; 0x0
    8e44:	e1a03803 	lsl	r3, r3, #16
    8e48:	e3a02007 	mov	r2, #7	; 0x7
    8e4c:	e58d3218 	str	r3, [sp, #536]
    8e50:	e3a00009 	mov	r0, #9	; 0x9
    8e54:	e28d3c02 	add	r3, sp, #512	; 0x200
    8e58:	e1c301b0 	strh	r0, [r3, #16]
    8e5c:	e58d2214 	str	r2, [sp, #532]
    8e60:	e58d122c 	str	r1, [sp, #556]
    8e64:	e1a02001 	mov	r2, r1
    8e68:	e1a00006 	mov	r0, r6
    8e6c:	e28d1e21 	add	r1, sp, #528	; 0x210
    8e70:	e1a0e00f 	mov	lr, pc
    8e74:	e596f104 	ldr	pc, [r6, #260]
    8e78:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8e7c:	e1a04000 	mov	r4, r0
    8e80:	e1a00006 	mov	r0, r6
    8e84:	ebfffebc 	bl	897c <mmc_send_status>
    8e88:	e3540000 	cmp	r4, #0	; 0x0
    8e8c:	1a000129 	bne	9338 <mmc_startup+0x5b8>
    8e90:	e5963030 	ldr	r3, [r6, #48]
    8e94:	e59d021c 	ldr	r0, [sp, #540]
    8e98:	e59d2224 	ldr	r2, [sp, #548]
    8e9c:	e59d1228 	ldr	r1, [sp, #552]
    8ea0:	e3530801 	cmp	r3, #65536	; 0x10000
    8ea4:	e59d3220 	ldr	r3, [sp, #544]
    8ea8:	e5862068 	str	r2, [r6, #104]
    8eac:	e5863064 	str	r3, [r6, #100]
    8eb0:	e586106c 	str	r1, [r6, #108]
    8eb4:	e5860060 	str	r0, [r6, #96]
    8eb8:	1a000013 	bne	8f0c <mmc_startup+0x18c>
    8ebc:	e1a03d20 	lsr	r3, r0, #26
    8ec0:	e203300f 	and	r3, r3, #15	; 0xf
    8ec4:	e3530004 	cmp	r3, #4	; 0x4
    8ec8:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    8ecc:	ea00000c 	b	8f04 <mmc_startup+0x184>
    8ed0:	00008f04 	.word	0x00008f04
    8ed4:	00008ee4 	.word	0x00008ee4
    8ed8:	00008eec 	.word	0x00008eec
    8edc:	00008ef4 	.word	0x00008ef4
    8ee0:	00008efc 	.word	0x00008efc
    8ee4:	e59f3458 	ldr	r3, [pc, #1112]	; 9344 <mmc_startup+0x5c4>
    8ee8:	ea000006 	b	8f08 <mmc_startup+0x188>
    8eec:	e59f3454 	ldr	r3, [pc, #1108]	; 9348 <mmc_startup+0x5c8>
    8ef0:	ea000004 	b	8f08 <mmc_startup+0x188>
    8ef4:	e59f3450 	ldr	r3, [pc, #1104]	; 934c <mmc_startup+0x5cc>
    8ef8:	ea000002 	b	8f08 <mmc_startup+0x188>
    8efc:	e59f344c 	ldr	r3, [pc, #1100]	; 9350 <mmc_startup+0x5d0>
    8f00:	ea000000 	b	8f08 <mmc_startup+0x188>
    8f04:	e59f3448 	ldr	r3, [pc, #1096]	; 9354 <mmc_startup+0x5d4>
    8f08:	e5863030 	str	r3, [r6, #48]
    8f0c:	e28d1c02 	add	r1, sp, #512	; 0x200
    8f10:	e1d132b2 	ldrh	r3, [r1, #34]
    8f14:	e3a0e001 	mov	lr, #1	; 0x1
    8f18:	e203300f 	and	r3, r3, #15	; 0xf
    8f1c:	e1a0c31e 	lsl	ip, lr, r3
    8f20:	e59d121c 	ldr	r1, [sp, #540]
    8f24:	e59f242c 	ldr	r2, [pc, #1068]	; 9358 <mmc_startup+0x5d8>
    8f28:	e1a031a1 	lsr	r3, r1, #3
    8f2c:	e203300f 	and	r3, r3, #15	; 0xf
    8f30:	e7920103 	ldr	r0, [r2, r3, lsl #2]
    8f34:	e59f3420 	ldr	r3, [pc, #1056]	; 935c <mmc_startup+0x5dc>
    8f38:	e2011007 	and	r1, r1, #7	; 0x7
    8f3c:	e7933101 	ldr	r3, [r3, r1, lsl #2]
    8f40:	e5962030 	ldr	r2, [r6, #48]
    8f44:	e0030093 	mul	r3, r3, r0
    8f48:	e3120802 	tst	r2, #131072	; 0x20000
    8f4c:	e5863084 	str	r3, [r6, #132]
    8f50:	e586c088 	str	ip, [r6, #136]
    8f54:	059d3228 	ldreq	r3, [sp, #552]
    8f58:	1586c08c 	strne	ip, [r6, #140]
    8f5c:	01a03b23 	lsreq	r3, r3, #22
    8f60:	0203300f 	andeq	r3, r3, #15	; 0xf
    8f64:	01a0331e 	lsleq	r3, lr, r3
    8f68:	0586308c 	streq	r3, [r6, #140]
    8f6c:	e5963040 	ldr	r3, [r6, #64]
    8f70:	e5962064 	ldr	r2, [r6, #100]
    8f74:	e3530000 	cmp	r3, #0	; 0x0
    8f78:	e5960068 	ldr	r0, [r6, #104]
    8f7c:	0a000007 	beq	8fa0 <mmc_startup+0x220>
    8f80:	e202203f 	and	r2, r2, #63	; 0x3f
    8f84:	e1a03820 	lsr	r3, r0, #16
    8f88:	e1833802 	orr	r3, r3, r2, lsl #16
    8f8c:	e1a08003 	mov	r8, r3
    8f90:	e3a09000 	mov	r9, #0	; 0x0
    8f94:	e3a04008 	mov	r4, #8	; 0x8
    8f98:	e3a05000 	mov	r5, #0	; 0x0
    8f9c:	ea000009 	b	8fc8 <mmc_startup+0x248>
    8fa0:	e1a03b02 	lsl	r3, r2, #22
    8fa4:	e1a03b23 	lsr	r3, r3, #22
    8fa8:	e200190e 	and	r1, r0, #229376	; 0x38000
    8fac:	e1a02f20 	lsr	r2, r0, #30
    8fb0:	e1822103 	orr	r2, r2, r3, lsl #2
    8fb4:	e1a017a1 	lsr	r1, r1, #15
    8fb8:	e1a08002 	mov	r8, r2
    8fbc:	e3a09000 	mov	r9, #0	; 0x0
    8fc0:	e1a04001 	mov	r4, r1
    8fc4:	e3a05000 	mov	r5, #0	; 0x0
    8fc8:	e3a02001 	mov	r2, #1	; 0x1
    8fcc:	e0980002 	adds	r0, r8, r2
    8fd0:	e3a03000 	mov	r3, #0	; 0x0
    8fd4:	e0a91003 	adc	r1, r9, r3
    8fd8:	e2842002 	add	r2, r4, #2	; 0x2
    8fdc:	eb0025e5 	bl	12778 <__aeabi_llsl>
    8fe0:	e596c088 	ldr	ip, [r6, #136]
    8fe4:	e0832c90 	umull	r2, r3, r0, ip
    8fe8:	e023319c 	mla	r3, ip, r1, r3
    8fec:	e35c0c02 	cmp	ip, #512	; 0x200
    8ff0:	e1c629f0 	strd	r2, [r6, #144]
    8ff4:	83a03c02 	movhi	r3, #512	; 0x200
    8ff8:	85863088 	strhi	r3, [r6, #136]
    8ffc:	e596308c 	ldr	r3, [r6, #140]
    9000:	e3530c02 	cmp	r3, #512	; 0x200
    9004:	83a03c02 	movhi	r3, #512	; 0x200
    9008:	8586308c 	strhi	r3, [r6, #140]
    900c:	e5963050 	ldr	r3, [r6, #80]
    9010:	e2131b01 	ands	r1, r3, #1024	; 0x400
    9014:	1a00000f 	bne	9058 <mmc_startup+0x2d8>
    9018:	e1d638b0 	ldrh	r3, [r6, #128]
    901c:	e3a0201d 	mov	r2, #29	; 0x1d
    9020:	e1a03803 	lsl	r3, r3, #16
    9024:	e58d2214 	str	r2, [sp, #532]
    9028:	e58d3218 	str	r3, [sp, #536]
    902c:	e28d2c02 	add	r2, sp, #512	; 0x200
    9030:	e3a03007 	mov	r3, #7	; 0x7
    9034:	e1c231b0 	strh	r3, [r2, #16]
    9038:	e58d122c 	str	r1, [sp, #556]
    903c:	e1a02001 	mov	r2, r1
    9040:	e1a00006 	mov	r0, r6
    9044:	e28d1e21 	add	r1, sp, #528	; 0x210
    9048:	e1a0e00f 	mov	lr, pc
    904c:	e596f104 	ldr	pc, [r6, #260]
    9050:	e2504000 	subs	r4, r0, #0	; 0x0
    9054:	1a0000b7 	bne	9338 <mmc_startup+0x5b8>
    9058:	e5962030 	ldr	r2, [r6, #48]
    905c:	e3e03000 	mvn	r3, #0	; 0x0
    9060:	e3120802 	tst	r2, #131072	; 0x20000
    9064:	e5c63082 	strb	r3, [r6, #130]
    9068:	1a00001e 	bne	90e8 <mmc_startup+0x368>
    906c:	e59f32ec 	ldr	r3, [pc, #748]	; 9360 <mmc_startup+0x5e0>
    9070:	e1520003 	cmp	r2, r3
    9074:	9a00001b 	bls	90e8 <mmc_startup+0x368>
    9078:	e28d1010 	add	r1, sp, #16	; 0x10
    907c:	e1a00006 	mov	r0, r6
    9080:	ebfffc34 	bl	8158 <mmc_send_ext_csd>
    9084:	e5dd30d0 	ldrb	r3, [sp, #208]
    9088:	e2701001 	rsbs	r1, r0, #1	; 0x1
    908c:	33a01000 	movcc	r1, #0	; 0x0
    9090:	e3530001 	cmp	r3, #1	; 0x1
    9094:	93a01000 	movls	r1, #0	; 0x0
    9098:	e3510000 	cmp	r1, #0	; 0x0
    909c:	0a00000d 	beq	90d8 <mmc_startup+0x358>
    90a0:	e5dd20e6 	ldrb	r2, [sp, #230]
    90a4:	e5dd10e5 	ldrb	r1, [sp, #229]
    90a8:	e5dd00e4 	ldrb	r0, [sp, #228]
    90ac:	e1a02802 	lsl	r2, r2, #16
    90b0:	e1822401 	orr	r2, r2, r1, lsl #8
    90b4:	e5dd10e7 	ldrb	r1, [sp, #231]
    90b8:	e1822000 	orr	r2, r2, r0
    90bc:	e1822c01 	orr	r2, r2, r1, lsl #24
    90c0:	e1a03fc2 	asr	r3, r2, #31
    90c4:	e1a01483 	lsl	r1, r3, #9
    90c8:	e1811ba2 	orr	r1, r1, r2, lsr #23
    90cc:	e1a02482 	lsl	r2, r2, #9
    90d0:	e5861094 	str	r1, [r6, #148]
    90d4:	e5862090 	str	r2, [r6, #144]
    90d8:	e5dd30b0 	ldrb	r3, [sp, #176]
    90dc:	e3130001 	tst	r3, #1	; 0x1
    90e0:	15dd30c3 	ldrbne	r3, [sp, #195]
    90e4:	15c63082 	strbne	r3, [r6, #130]
    90e8:	e5963030 	ldr	r3, [r6, #48]
    90ec:	e3130802 	tst	r3, #131072	; 0x20000
    90f0:	0a000002 	beq	9100 <mmc_startup+0x380>
    90f4:	e1a00006 	mov	r0, r6
    90f8:	ebfffce8 	bl	84a0 <sd_change_freq>
    90fc:	ea000001 	b	9108 <mmc_startup+0x388>
    9100:	e1a00006 	mov	r0, r6
    9104:	ebfffef0 	bl	8ccc <mmc_change_freq>
    9108:	e3500000 	cmp	r0, #0	; 0x0
    910c:	e1a05000 	mov	r5, r0
    9110:	11a04000 	movne	r4, r0
    9114:	1a000087 	bne	9338 <mmc_startup+0x5b8>
    9118:	e596304c 	ldr	r3, [r6, #76]
    911c:	e5962050 	ldr	r2, [r6, #80]
    9120:	e5961030 	ldr	r1, [r6, #48]
    9124:	e0022003 	and	r2, r2, r3
    9128:	e3110802 	tst	r1, #131072	; 0x20000
    912c:	e586204c 	str	r2, [r6, #76]
    9130:	e2023c01 	and	r3, r2, #256	; 0x100
    9134:	0a00002c 	beq	91ec <mmc_startup+0x46c>
    9138:	e3530000 	cmp	r3, #0	; 0x0
    913c:	0a000023 	beq	91d0 <mmc_startup+0x450>
    9140:	e1d638b0 	ldrh	r3, [r6, #128]
    9144:	e28d2c02 	add	r2, sp, #512	; 0x200
    9148:	e1a03803 	lsl	r3, r3, #16
    914c:	e58d3218 	str	r3, [sp, #536]
    9150:	e3a08015 	mov	r8, #21	; 0x15
    9154:	e3a03037 	mov	r3, #55	; 0x37
    9158:	e28d9e21 	add	r9, sp, #528	; 0x210
    915c:	e1c231b0 	strh	r3, [r2, #16]
    9160:	e58d022c 	str	r0, [sp, #556]
    9164:	e58d8214 	str	r8, [sp, #532]
    9168:	e1a00006 	mov	r0, r6
    916c:	e1a01009 	mov	r1, r9
    9170:	e1a02005 	mov	r2, r5
    9174:	e1a0e00f 	mov	lr, pc
    9178:	e596f104 	ldr	pc, [r6, #260]
    917c:	e2504000 	subs	r4, r0, #0	; 0x0
    9180:	1a00006c 	bne	9338 <mmc_startup+0x5b8>
    9184:	e3a03002 	mov	r3, #2	; 0x2
    9188:	e3a01006 	mov	r1, #6	; 0x6
    918c:	e28d0c02 	add	r0, sp, #512	; 0x200
    9190:	e1c011b0 	strh	r1, [r0, #16]
    9194:	e58d8214 	str	r8, [sp, #532]
    9198:	e58d3218 	str	r3, [sp, #536]
    919c:	e58d522c 	str	r5, [sp, #556]
    91a0:	e1a01009 	mov	r1, r9
    91a4:	e1a02005 	mov	r2, r5
    91a8:	e1a00006 	mov	r0, r6
    91ac:	e1a0e00f 	mov	lr, pc
    91b0:	e596f104 	ldr	pc, [r6, #260]
    91b4:	e2504000 	subs	r4, r0, #0	; 0x0
    91b8:	1a00005e 	bne	9338 <mmc_startup+0x5b8>
    91bc:	e3a03004 	mov	r3, #4	; 0x4
    91c0:	e5863044 	str	r3, [r6, #68]
    91c4:	e1a00006 	mov	r0, r6
    91c8:	e1a0e00f 	mov	lr, pc
    91cc:	e596f108 	ldr	pc, [r6, #264]
    91d0:	e596304c 	ldr	r3, [r6, #76]
    91d4:	e3130001 	tst	r3, #1	; 0x1
    91d8:	11a00006 	movne	r0, r6
    91dc:	159f1180 	ldrne	r1, [pc, #384]	; 9364 <mmc_startup+0x5e4>
    91e0:	01a00006 	moveq	r0, r6
    91e4:	059f117c 	ldreq	r1, [pc, #380]	; 9368 <mmc_startup+0x5e8>
    91e8:	ea000022 	b	9278 <mmc_startup+0x4f8>
    91ec:	e3530000 	cmp	r3, #0	; 0x0
    91f0:	0a000008 	beq	9218 <mmc_startup+0x498>
    91f4:	e3a01001 	mov	r1, #1	; 0x1
    91f8:	e1a03001 	mov	r3, r1
    91fc:	e1a00006 	mov	r0, r6
    9200:	e3a020b7 	mov	r2, #183	; 0xb7
    9204:	ebfffe98 	bl	8c6c <mmc_switch>
    9208:	e2504000 	subs	r4, r0, #0	; 0x0
    920c:	03a03004 	moveq	r3, #4	; 0x4
    9210:	1a000048 	bne	9338 <mmc_startup+0x5b8>
    9214:	ea000009 	b	9240 <mmc_startup+0x4c0>
    9218:	e3120c02 	tst	r2, #512	; 0x200
    921c:	0a00000b 	beq	9250 <mmc_startup+0x4d0>
    9220:	e1a00006 	mov	r0, r6
    9224:	e3a01001 	mov	r1, #1	; 0x1
    9228:	e3a020b7 	mov	r2, #183	; 0xb7
    922c:	e3a03002 	mov	r3, #2	; 0x2
    9230:	ebfffe8d 	bl	8c6c <mmc_switch>
    9234:	e2504000 	subs	r4, r0, #0	; 0x0
    9238:	1a00003e 	bne	9338 <mmc_startup+0x5b8>
    923c:	e3a03008 	mov	r3, #8	; 0x8
    9240:	e5863044 	str	r3, [r6, #68]
    9244:	e1a00006 	mov	r0, r6
    9248:	e1a0e00f 	mov	lr, pc
    924c:	e596f108 	ldr	pc, [r6, #264]
    9250:	e596304c 	ldr	r3, [r6, #76]
    9254:	e3130001 	tst	r3, #1	; 0x1
    9258:	01a00006 	moveq	r0, r6
    925c:	059f1108 	ldreq	r1, [pc, #264]	; 936c <mmc_startup+0x5ec>
    9260:	0a000004 	beq	9278 <mmc_startup+0x4f8>
    9264:	e3130010 	tst	r3, #16	; 0x10
    9268:	159f1100 	ldrne	r1, [pc, #256]	; 9370 <mmc_startup+0x5f0>
    926c:	059f1100 	ldreq	r1, [pc, #256]	; 9374 <mmc_startup+0x5f4>
    9270:	11a00006 	movne	r0, r6
    9274:	01a00006 	moveq	r0, r6
    9278:	ebfffbed 	bl	8234 <mmc_set_clock>
    927c:	e5964094 	ldr	r4, [r6, #148]
    9280:	e5960090 	ldr	r0, [r6, #144]
    9284:	e5961088 	ldr	r1, [r6, #136]
    9288:	e3a03000 	mov	r3, #0	; 0x0
    928c:	e3540000 	cmp	r4, #0	; 0x0
    9290:	e5c630a3 	strb	r3, [r6, #163]
    9294:	e5c630a2 	strb	r3, [r6, #162]
    9298:	e58610ac 	str	r1, [r6, #172]
    929c:	e58d0230 	str	r0, [sp, #560]
    92a0:	e58d4234 	str	r4, [sp, #564]
    92a4:	1a000003 	bne	92b8 <mmc_startup+0x538>
    92a8:	eb00241b 	bl	1231c <__aeabi_uidiv>
    92ac:	e58d4234 	str	r4, [sp, #564]
    92b0:	e58d0230 	str	r0, [sp, #560]
    92b4:	ea000001 	b	92c0 <mmc_startup+0x540>
    92b8:	e28d0e23 	add	r0, sp, #560	; 0x230
    92bc:	eb0017d5 	bl	f218 <__div64_32>
    92c0:	e5d6007f 	ldrb	r0, [r6, #127]
    92c4:	e59d1230 	ldr	r1, [sp, #560]
    92c8:	e5962070 	ldr	r2, [r6, #112]
    92cc:	e5963078 	ldr	r3, [r6, #120]
    92d0:	e58610a8 	str	r1, [r6, #168]
    92d4:	e1803403 	orr	r3, r0, r3, lsl #8
    92d8:	e1a02422 	lsr	r2, r2, #8
    92dc:	e59f1094 	ldr	r1, [pc, #148]	; 9378 <mmc_startup+0x5f8>
    92e0:	e28600b0 	add	r0, r6, #176	; 0xb0
    92e4:	eb001b94 	bl	1013c <sprintf>
    92e8:	e596c074 	ldr	ip, [r6, #116]
    92ec:	e5d62070 	ldrb	r2, [r6, #112]
    92f0:	e1a0482c 	lsr	r4, ip, #16
    92f4:	e1a0542c 	lsr	r5, ip, #8
    92f8:	e1a03c2c 	lsr	r3, ip, #24
    92fc:	e20440ff 	and	r4, r4, #255	; 0xff
    9300:	e20cc0ff 	and	ip, ip, #255	; 0xff
    9304:	e59f1070 	ldr	r1, [pc, #112]	; 937c <mmc_startup+0x5fc>
    9308:	e20550ff 	and	r5, r5, #255	; 0xff
    930c:	e28600d9 	add	r0, r6, #217	; 0xd9
    9310:	e88d1030 	stm	sp, {r4, r5, ip}
    9314:	eb001b88 	bl	1013c <sprintf>
    9318:	e5962078 	ldr	r2, [r6, #120]
    931c:	e28600ee 	add	r0, r6, #238	; 0xee
    9320:	e1a03c22 	lsr	r3, r2, #24
    9324:	e203300f 	and	r3, r3, #15	; 0xf
    9328:	e1a02e22 	lsr	r2, r2, #28
    932c:	e59f104c 	ldr	r1, [pc, #76]	; 9380 <mmc_startup+0x600>
    9330:	eb001b81 	bl	1013c <sprintf>
    9334:	e3a04000 	mov	r4, #0	; 0x0
    9338:	e1a00004 	mov	r0, r4
    933c:	e28ddf8e 	add	sp, sp, #568	; 0x238
    9340:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    9344:	00010014 	.word	0x00010014
    9348:	00010022 	.word	0x00010022
    934c:	00010030 	.word	0x00010030
    9350:	00010040 	.word	0x00010040
    9354:	00010012 	.word	0x00010012
    9358:	00013848 	.word	0x00013848
    935c:	00013838 	.word	0x00013838
    9360:	0001003f 	.word	0x0001003f
    9364:	02faf080 	.word	0x02faf080
    9368:	017d7840 	.word	0x017d7840
    936c:	01312d00 	.word	0x01312d00
    9370:	03197500 	.word	0x03197500
    9374:	018cba80 	.word	0x018cba80
    9378:	0001bcd6 	.word	0x0001bcd6
    937c:	0001bce8 	.word	0x0001bce8
    9380:	0001bcf3 	.word	0x0001bcf3

00009384 <mmc_init>:
    9384:	e92d4010 	push	{r4, lr}
    9388:	ebfffb5a 	bl	80f8 <find_mmc_device>
    938c:	e1a04000 	mov	r4, r0
    9390:	e1a0e00f 	mov	lr, pc
    9394:	e590f10c 	ldr	pc, [r0, #268]
    9398:	e3500000 	cmp	r0, #0	; 0x0
    939c:	18bd8010 	popne	{r4, pc}
    93a0:	e3a03001 	mov	r3, #1	; 0x1
    93a4:	e5843044 	str	r3, [r4, #68]
    93a8:	e1a00004 	mov	r0, r4
    93ac:	e1a0e00f 	mov	lr, pc
    93b0:	e594f108 	ldr	pc, [r4, #264]
    93b4:	e1a00004 	mov	r0, r4
    93b8:	e59f1060 	ldr	r1, [pc, #96]	; 9420 <mmc_init+0x9c>
    93bc:	ebfffb9c 	bl	8234 <mmc_set_clock>
    93c0:	e1a00004 	mov	r0, r4
    93c4:	ebfffd09 	bl	87f0 <mmc_go_idle>
    93c8:	e3500000 	cmp	r0, #0	; 0x0
    93cc:	18bd8010 	popne	{r4, pc}
    93d0:	e5c40083 	strb	r0, [r4, #131]
    93d4:	e1a00004 	mov	r0, r4
    93d8:	ebfffba9 	bl	8284 <mmc_send_if_cond>
    93dc:	e1a00004 	mov	r0, r4
    93e0:	ebfffcad 	bl	869c <sd_send_op_cond>
    93e4:	e3700013 	cmn	r0, #19	; 0x13
    93e8:	1a000009 	bne	9414 <mmc_init+0x90>
    93ec:	e1a00004 	mov	r0, r4
    93f0:	ebfffd13 	bl	8844 <mmc_send_op_cond>
    93f4:	e3500000 	cmp	r0, #0	; 0x0
    93f8:	0a000005 	beq	9414 <mmc_init+0x90>
    93fc:	e59f0020 	ldr	r0, [pc, #32]	; 9424 <mmc_init+0xa0>
    9400:	e59f3020 	ldr	r3, [pc, #32]	; 9428 <mmc_init+0xa4>
    9404:	e1a0e00f 	mov	lr, pc
    9408:	e593f000 	ldr	pc, [r3]
    940c:	e3e00010 	mvn	r0, #16	; 0x10
    9410:	e8bd8010 	pop	{r4, pc}
    9414:	e1a00004 	mov	r0, r4
    9418:	e8bd4010 	pop	{r4, lr}
    941c:	eafffe57 	b	8d80 <mmc_startup>
    9420:	00061a80 	.word	0x00061a80
    9424:	0001bcf9 	.word	0x0001bcf9
    9428:	3c00d694 	.word	0x3c00d694

0000942c <mmc2_vs_reset>:
    942c:	e3a00002 	mov	r0, #2	; 0x2
    9430:	eaffffd3 	b	9384 <mmc_init>

00009434 <mmc1_vs_reset>:
    9434:	e3a00001 	mov	r0, #1	; 0x1
    9438:	eaffffd1 	b	9384 <mmc_init>

0000943c <mmc0_vs_reset>:
    943c:	e3a00000 	mov	r0, #0	; 0x0
    9440:	eaffffcf 	b	9384 <mmc_init>

00009444 <mmc_read_blocks>:
    9444:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    9448:	e3530001 	cmp	r3, #1	; 0x1
    944c:	e24dd030 	sub	sp, sp, #48	; 0x30
    9450:	e1a06003 	mov	r6, r3
    9454:	83a03012 	movhi	r3, #18	; 0x12
    9458:	93a03011 	movls	r3, #17	; 0x11
    945c:	e1cd30b0 	strh	r3, [sp]
    9460:	e5903040 	ldr	r3, [r0, #64]
    9464:	e3a07000 	mov	r7, #0	; 0x0
    9468:	e3530000 	cmp	r3, #0	; 0x0
    946c:	05903088 	ldreq	r3, [r0, #136]
    9470:	158d2008 	strne	r2, [sp, #8]
    9474:	00030293 	muleq	r3, r3, r2
    9478:	058d3008 	streq	r3, [sp, #8]
    947c:	e5903088 	ldr	r3, [r0, #136]
    9480:	e3a02015 	mov	r2, #21	; 0x15
    9484:	e58d302c 	str	r3, [sp, #44]
    9488:	e3a03001 	mov	r3, #1	; 0x1
    948c:	e58d2004 	str	r2, [sp, #4]
    9490:	e58d1020 	str	r1, [sp, #32]
    9494:	e1a04000 	mov	r4, r0
    9498:	e58d3024 	str	r3, [sp, #36]
    949c:	e58d701c 	str	r7, [sp, #28]
    94a0:	e58d6028 	str	r6, [sp, #40]
    94a4:	e1a0100d 	mov	r1, sp
    94a8:	e28d2020 	add	r2, sp, #32	; 0x20
    94ac:	e1a0e00f 	mov	lr, pc
    94b0:	e594f104 	ldr	pc, [r4, #260]
    94b4:	e2505000 	subs	r5, r0, #0	; 0x0
    94b8:	e1a0800d 	mov	r8, sp
    94bc:	11a00007 	movne	r0, r7
    94c0:	1a000018 	bne	9528 <mmc_read_blocks+0xe4>
    94c4:	e3560001 	cmp	r6, #1	; 0x1
    94c8:	9a000012 	bls	9518 <mmc_read_blocks+0xd4>
    94cc:	e3a0301d 	mov	r3, #29	; 0x1d
    94d0:	e58d3004 	str	r3, [sp, #4]
    94d4:	e3a0300c 	mov	r3, #12	; 0xc
    94d8:	e1cd30b0 	strh	r3, [sp]
    94dc:	e58d5008 	str	r5, [sp, #8]
    94e0:	e58d501c 	str	r5, [sp, #28]
    94e4:	e1a0100d 	mov	r1, sp
    94e8:	e1a00004 	mov	r0, r4
    94ec:	e1a02005 	mov	r2, r5
    94f0:	e1a0e00f 	mov	lr, pc
    94f4:	e594f104 	ldr	pc, [r4, #260]
    94f8:	e3500000 	cmp	r0, #0	; 0x0
    94fc:	0a000005 	beq	9518 <mmc_read_blocks+0xd4>
    9500:	e59f0028 	ldr	r0, [pc, #40]	; 9530 <mmc_read_blocks+0xec>
    9504:	e59f3028 	ldr	r3, [pc, #40]	; 9534 <mmc_read_blocks+0xf0>
    9508:	e1a0e00f 	mov	lr, pc
    950c:	e593f000 	ldr	pc, [r3]
    9510:	e1a00005 	mov	r0, r5
    9514:	ea000003 	b	9528 <mmc_read_blocks+0xe4>
    9518:	e1a00004 	mov	r0, r4
    951c:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    9520:	ebfffd15 	bl	897c <mmc_send_status>
    9524:	e1a00006 	mov	r0, r6
    9528:	e28dd030 	add	sp, sp, #48	; 0x30
    952c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    9530:	0001bcbb 	.word	0x0001bcbb
    9534:	3c00d694 	.word	0x3c00d694

00009538 <mmc_bread>:
    9538:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    953c:	e252a000 	subs	sl, r2, #0	; 0x0
    9540:	e24dd004 	sub	sp, sp, #4	; 0x4
    9544:	e1a07001 	mov	r7, r1
    9548:	e1a08003 	mov	r8, r3
    954c:	0a000027 	beq	95f0 <mmc_bread+0xb8>
    9550:	ebfffae8 	bl	80f8 <find_mmc_device>
    9554:	e2506000 	subs	r6, r0, #0	; 0x0
    9558:	0a000024 	beq	95f0 <mmc_bread+0xb8>
    955c:	e59620a8 	ldr	r2, [r6, #168]
    9560:	e08a1007 	add	r1, sl, r7
    9564:	e1510002 	cmp	r1, r2
    9568:	9a000004 	bls	9580 <mmc_bread+0x48>
    956c:	e59f0088 	ldr	r0, [pc, #136]	; 95fc <mmc_bread+0xc4>
    9570:	e59f3088 	ldr	r3, [pc, #136]	; 9600 <mmc_bread+0xc8>
    9574:	e1a0e00f 	mov	lr, pc
    9578:	e593f000 	ldr	pc, [r3]
    957c:	ea00001b 	b	95f0 <mmc_bread+0xb8>
    9580:	e5961088 	ldr	r1, [r6, #136]
    9584:	ebfffacd 	bl	80c0 <mmc_set_blocklen>
    9588:	e3500000 	cmp	r0, #0	; 0x0
    958c:	01a0500a 	moveq	r5, sl
    9590:	0a000005 	beq	95ac <mmc_bread+0x74>
    9594:	e5961088 	ldr	r1, [r6, #136]
    9598:	e59f0064 	ldr	r0, [pc, #100]	; 9604 <mmc_bread+0xcc>
    959c:	e59f305c 	ldr	r3, [pc, #92]	; 9600 <mmc_bread+0xc8>
    95a0:	e1a0e00f 	mov	lr, pc
    95a4:	e593f000 	ldr	pc, [r3]
    95a8:	ea000010 	b	95f0 <mmc_bread+0xb8>
    95ac:	e5964110 	ldr	r4, [r6, #272]
    95b0:	e1a02007 	mov	r2, r7
    95b4:	e1550004 	cmp	r5, r4
    95b8:	31a04005 	movcc	r4, r5
    95bc:	e1a01008 	mov	r1, r8
    95c0:	e1a00006 	mov	r0, r6
    95c4:	e1a03004 	mov	r3, r4
    95c8:	ebffff9d 	bl	9444 <mmc_read_blocks>
    95cc:	e1500004 	cmp	r0, r4
    95d0:	e0877000 	add	r7, r7, r0
    95d4:	1a000005 	bne	95f0 <mmc_bread+0xb8>
    95d8:	e0555000 	subs	r5, r5, r0
    95dc:	e5962088 	ldr	r2, [r6, #136]
    95e0:	10288092 	mlane	r8, r2, r0, r8
    95e4:	1afffff0 	bne	95ac <mmc_bread+0x74>
    95e8:	e000029a 	mul	r0, sl, r2
    95ec:	ea000000 	b	95f4 <mmc_bread+0xbc>
    95f0:	e3a00000 	mov	r0, #0	; 0x0
    95f4:	e28dd004 	add	sp, sp, #4	; 0x4
    95f8:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    95fc:	0001bc7d 	.word	0x0001bc7d
    9600:	3c00d694 	.word	0x3c00d694
    9604:	0001bd22 	.word	0x0001bd22

00009608 <mmc2_vs_read>:
    9608:	e1a024a2 	lsr	r2, r2, #9
    960c:	e1821b83 	orr	r1, r2, r3, lsl #23
    9610:	e59d2000 	ldr	r2, [sp]
    9614:	e1a03000 	mov	r3, r0
    9618:	e1a024c2 	asr	r2, r2, #9
    961c:	e3a00002 	mov	r0, #2	; 0x2
    9620:	eaffffc4 	b	9538 <mmc_bread>

00009624 <mmc1_vs_read>:
    9624:	e1a024a2 	lsr	r2, r2, #9
    9628:	e1821b83 	orr	r1, r2, r3, lsl #23
    962c:	e59d2000 	ldr	r2, [sp]
    9630:	e1a03000 	mov	r3, r0
    9634:	e1a024c2 	asr	r2, r2, #9
    9638:	e3a00001 	mov	r0, #1	; 0x1
    963c:	eaffffbd 	b	9538 <mmc_bread>

00009640 <mmc0_vs_read>:
    9640:	e1a024a2 	lsr	r2, r2, #9
    9644:	e1821b83 	orr	r1, r2, r3, lsl #23
    9648:	e59d2000 	ldr	r2, [sp]
    964c:	e1a03000 	mov	r3, r0
    9650:	e1a024c2 	asr	r2, r2, #9
    9654:	e3a00000 	mov	r0, #0	; 0x0
    9658:	eaffffb6 	b	9538 <mmc_bread>

0000965c <mci_pull_data32>:
    965c:	e1a02142 	asr	r2, r2, #2
    9660:	e3a0c000 	mov	ip, #0	; 0x0
    9664:	ea000003 	b	9678 <mci_pull_data32+0x1c>
    9668:	e5903118 	ldr	r3, [r0, #280]
    966c:	e5933200 	ldr	r3, [r3, #512]
    9670:	e781300c 	str	r3, [r1, ip]
    9674:	e28cc004 	add	ip, ip, #4	; 0x4
    9678:	e3520000 	cmp	r2, #0	; 0x0
    967c:	e2422001 	sub	r2, r2, #1	; 0x1
    9680:	cafffff8 	bgt	9668 <mci_pull_data32+0xc>
    9684:	e12fff1e 	bx	lr

00009688 <mci_push_data32>:
    9688:	e92d4010 	push	{r4, lr}
    968c:	e3a0c000 	mov	ip, #0	; 0x0
    9690:	e1a04000 	mov	r4, r0
    9694:	e1a00001 	mov	r0, r1
    9698:	e1a01142 	asr	r1, r2, #2
    969c:	ea000003 	b	96b0 <mci_push_data32+0x28>
    96a0:	e790200c 	ldr	r2, [r0, ip]
    96a4:	e5943118 	ldr	r3, [r4, #280]
    96a8:	e28cc004 	add	ip, ip, #4	; 0x4
    96ac:	e5832200 	str	r2, [r3, #512]
    96b0:	e3510000 	cmp	r1, #0	; 0x0
    96b4:	e2411001 	sub	r1, r1, #1	; 0x1
    96b8:	cafffff8 	bgt	96a0 <mci_push_data32+0x18>
    96bc:	e8bd8010 	pop	{r4, pc}

000096c0 <imap_sdhc_initialize>:
    96c0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    96c4:	e3510000 	cmp	r1, #0	; 0x0
    96c8:	e24dd004 	sub	sp, sp, #4	; 0x4
    96cc:	1a000005 	bne	96e8 <imap_sdhc_initialize+0x28>
    96d0:	e59f00ec 	ldr	r0, [pc, #236]	; 97c4 <imap_sdhc_initialize+0x104>
    96d4:	e59f30ec 	ldr	r3, [pc, #236]	; 97c8 <imap_sdhc_initialize+0x108>
    96d8:	e1a0e00f 	mov	lr, pc
    96dc:	e593f000 	ldr	pc, [r3]
    96e0:	e3e00004 	mvn	r0, #4	; 0x4
    96e4:	ea000034 	b	97bc <imap_sdhc_initialize+0xfc>
    96e8:	e3500000 	cmp	r0, #0	; 0x0
    96ec:	03a0359d 	moveq	r3, #658505728	; 0x27400000
    96f0:	0a000005 	beq	970c <imap_sdhc_initialize+0x4c>
    96f4:	e3500001 	cmp	r0, #1	; 0x1
    96f8:	059f30cc 	ldreq	r3, [pc, #204]	; 97cc <imap_sdhc_initialize+0x10c>
    96fc:	0a000002 	beq	970c <imap_sdhc_initialize+0x4c>
    9700:	e3500002 	cmp	r0, #2	; 0x2
    9704:	1a000018 	bne	976c <imap_sdhc_initialize+0xac>
    9708:	e59f30c0 	ldr	r3, [pc, #192]	; 97d0 <imap_sdhc_initialize+0x110>
    970c:	e5813118 	str	r3, [r1, #280]
    9710:	e59f30bc 	ldr	r3, [pc, #188]	; 97d4 <imap_sdhc_initialize+0x114>
    9714:	e591c130 	ldr	ip, [r1, #304]
    9718:	e5813104 	str	r3, [r1, #260]
    971c:	e59f30b4 	ldr	r3, [pc, #180]	; 97d8 <imap_sdhc_initialize+0x118>
    9720:	e3a02000 	mov	r2, #0	; 0x0
    9724:	e5813108 	str	r3, [r1, #264]
    9728:	e59f30ac 	ldr	r3, [pc, #172]	; 97dc <imap_sdhc_initialize+0x11c>
    972c:	e5810114 	str	r0, [r1, #276]
    9730:	e581310c 	str	r3, [r1, #268]
    9734:	e59f30a4 	ldr	r3, [pc, #164]	; 97e0 <imap_sdhc_initialize+0x120>
    9738:	e5812034 	str	r2, [r1, #52]
    973c:	e581302c 	str	r3, [r1, #44]
    9740:	e3003101 	movw	r3, #257	; 0x101
    9744:	e5813050 	str	r3, [r1, #80]
    9748:	e59f3094 	ldr	r3, [pc, #148]	; 97e4 <imap_sdhc_initialize+0x124>
    974c:	e5812028 	str	r2, [r1, #40]
    9750:	e5813038 	str	r3, [r1, #56]
    9754:	e59f308c 	ldr	r3, [pc, #140]	; 97e8 <imap_sdhc_initialize+0x128>
    9758:	e5812110 	str	r2, [r1, #272]
    975c:	e581303c 	str	r3, [r1, #60]
    9760:	e1a0000c 	mov	r0, ip
    9764:	e2822010 	add	r2, r2, #16	; 0x10
    9768:	ea000005 	b	9784 <imap_sdhc_initialize+0xc4>
    976c:	e59f0078 	ldr	r0, [pc, #120]	; 97ec <imap_sdhc_initialize+0x12c>
    9770:	e59f3050 	ldr	r3, [pc, #80]	; 97c8 <imap_sdhc_initialize+0x108>
    9774:	e1a0e00f 	mov	lr, pc
    9778:	e593f000 	ldr	pc, [r3]
    977c:	e3e00000 	mvn	r0, #0	; 0x0
    9780:	ea00000d 	b	97bc <imap_sdhc_initialize+0xfc>
    9784:	e5913130 	ldr	r3, [r1, #304]
    9788:	e0823003 	add	r3, r2, r3
    978c:	e2822010 	add	r2, r2, #16	; 0x10
    9790:	e3520b01 	cmp	r2, #1024	; 0x400
    9794:	e580300c 	str	r3, [r0, #12]
    9798:	e2800010 	add	r0, r0, #16	; 0x10
    979c:	1afffff8 	bne	9784 <imap_sdhc_initialize+0xc4>
    97a0:	e5912130 	ldr	r2, [r1, #304]
    97a4:	e3a03020 	mov	r3, #32	; 0x20
    97a8:	e58c33f0 	str	r3, [ip, #1008]
    97ac:	e58c23fc 	str	r2, [ip, #1020]
    97b0:	e1a00001 	mov	r0, r1
    97b4:	ebfffad0 	bl	82fc <mmc_register>
    97b8:	e3a00000 	mov	r0, #0	; 0x0
    97bc:	e28dd004 	add	sp, sp, #4	; 0x4
    97c0:	e8bd8000 	pop	{pc}
    97c4:	0001bd2c 	.word	0x0001bd2c
    97c8:	3c00d694 	.word	0x3c00d694
    97cc:	27200000 	.word	0x27200000
    97d0:	27300000 	.word	0x27300000
    97d4:	00009860 	.word	0x00009860
    97d8:	00009d38 	.word	0x00009d38
    97dc:	00009b80 	.word	0x00009b80
    97e0:	00ff8000 	.word	0x00ff8000
    97e4:	00061a80 	.word	0x00061a80
    97e8:	018cba80 	.word	0x018cba80
    97ec:	0001bd42 	.word	0x0001bd42

000097f0 <mci_send_cmd>:
    97f0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    97f4:	e5903118 	ldr	r3, [r0, #280]
    97f8:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    97fc:	e5832028 	str	r2, [r3, #40]
    9800:	e5903118 	ldr	r3, [r0, #280]
    9804:	e24dd004 	sub	sp, sp, #4	; 0x4
    9808:	e583102c 	str	r1, [r3, #44]
    980c:	e1a04000 	mov	r4, r0
    9810:	eb00072e 	bl	b4d0 <get_ticks>
    9814:	e59f5040 	ldr	r5, [pc, #64]	; 985c <mci_send_cmd+0x6c>
    9818:	e1a06000 	mov	r6, r0
    981c:	e1a07001 	mov	r7, r1
    9820:	e5943118 	ldr	r3, [r4, #280]
    9824:	e593302c 	ldr	r3, [r3, #44]
    9828:	e3530000 	cmp	r3, #0	; 0x0
    982c:	aa000008 	bge	9854 <mci_send_cmd+0x64>
    9830:	eb000726 	bl	b4d0 <get_ticks>
    9834:	e1a02000 	mov	r2, r0
    9838:	e1a03001 	mov	r3, r1
    983c:	e0522006 	subs	r2, r2, r6
    9840:	e0c33007 	sbc	r3, r3, r7
    9844:	e3530000 	cmp	r3, #0	; 0x0
    9848:	1a000001 	bne	9854 <mci_send_cmd+0x64>
    984c:	e1520005 	cmp	r2, r5
    9850:	9afffff2 	bls	9820 <mci_send_cmd+0x30>
    9854:	e28dd004 	add	sp, sp, #4	; 0x4
    9858:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    985c:	0007a11f 	.word	0x0007a11f

00009860 <imap_sdhc_send_cmd>:
    9860:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    9864:	e252a000 	subs	sl, r2, #0	; 0x0
    9868:	e1a04000 	mov	r4, r0
    986c:	e1a08001 	mov	r8, r1
    9870:	0a000039 	beq	995c <imap_sdhc_send_cmd+0xfc>
    9874:	e99a000c 	ldmib	sl, {r2, r3}
    9878:	e59a100c 	ldr	r1, [sl, #12]
    987c:	e3120001 	tst	r2, #1	; 0x1
    9880:	e0000193 	mul	r0, r3, r1
    9884:	15942118 	ldrne	r2, [r4, #280]
    9888:	11a03801 	lslne	r3, r1, #16
    988c:	13833001 	orrne	r3, r3, #1	; 0x1
    9890:	15823100 	strne	r3, [r2, #256]
    9894:	e59a200c 	ldr	r2, [sl, #12]
    9898:	e5943118 	ldr	r3, [r4, #280]
    989c:	e1a0e000 	mov	lr, r0
    98a0:	e583201c 	str	r2, [r3, #28]
    98a4:	e5943118 	ldr	r3, [r4, #280]
    98a8:	e3a05000 	mov	r5, #0	; 0x0
    98ac:	e5830020 	str	r0, [r3, #32]
    98b0:	e5942118 	ldr	r2, [r4, #280]
    98b4:	e3019ff0 	movw	r9, #8176	; 0x1ff0
    98b8:	e5923000 	ldr	r3, [r2]
    98bc:	e3a0714a 	mov	r7, #-2147483630	; 0x80000012
    98c0:	e3833020 	orr	r3, r3, #32	; 0x20
    98c4:	e5823000 	str	r3, [r2]
    98c8:	e59a6000 	ldr	r6, [sl]
    98cc:	e594c130 	ldr	ip, [r4, #304]
    98d0:	e15e0009 	cmp	lr, r9
    98d4:	31a0100e 	movcc	r1, lr
    98d8:	21a01009 	movcs	r1, r9
    98dc:	e59c0004 	ldr	r0, [ip, #4]
    98e0:	e59f3288 	ldr	r3, [pc, #648]	; 9b70 <imap_sdhc_send_cmd+0x310>
    98e4:	e1a02981 	lsl	r2, r1, #19
    98e8:	e0003003 	and	r3, r0, r3
    98ec:	e1a029a2 	lsr	r2, r2, #19
    98f0:	e1822003 	orr	r2, r2, r3
    98f4:	e05ee001 	subs	lr, lr, r1
    98f8:	e0853006 	add	r3, r5, r6
    98fc:	e98c000c 	stmib	ip, {r2, r3}
    9900:	e58c7000 	str	r7, [ip]
    9904:	e1a0200c 	mov	r2, ip
    9908:	e0855001 	add	r5, r5, r1
    990c:	e28cc010 	add	ip, ip, #16	; 0x10
    9910:	1affffee 	bne	98d0 <imap_sdhc_send_cmd+0x70>
    9914:	e3c73002 	bic	r3, r7, #2	; 0x2
    9918:	e3833004 	orr	r3, r3, #4	; 0x4
    991c:	e5823000 	str	r3, [r2]
    9920:	e5942130 	ldr	r2, [r4, #304]
    9924:	e5923000 	ldr	r3, [r2]
    9928:	e3833008 	orr	r3, r3, #8	; 0x8
    992c:	e5823000 	str	r3, [r2]
    9930:	e5942118 	ldr	r2, [r4, #280]
    9934:	e5923000 	ldr	r3, [r2]
    9938:	e3833402 	orr	r3, r3, #33554432	; 0x2000000
    993c:	e5823000 	str	r3, [r2]
    9940:	e5942118 	ldr	r2, [r4, #280]
    9944:	e5923080 	ldr	r3, [r2, #128]
    9948:	e3833080 	orr	r3, r3, #128	; 0x80
    994c:	e5823080 	str	r3, [r2, #128]
    9950:	e5942118 	ldr	r2, [r4, #280]
    9954:	e3a03001 	mov	r3, #1	; 0x1
    9958:	e5823084 	str	r3, [r2, #132]
    995c:	e1d800b0 	ldrh	r0, [r8]
    9960:	e5982004 	ldr	r2, [r8, #4]
    9964:	e350000c 	cmp	r0, #12	; 0xc
    9968:	03a03901 	moveq	r3, #16384	; 0x4000
    996c:	13a03a02 	movne	r3, #8192	; 0x2000
    9970:	e3120004 	tst	r2, #4	; 0x4
    9974:	13833c01 	orrne	r3, r3, #256	; 0x100
    9978:	e3120001 	tst	r2, #1	; 0x1
    997c:	e3831202 	orr	r1, r3, #536870912	; 0x20000000
    9980:	0a000002 	beq	9990 <imap_sdhc_send_cmd+0x130>
    9984:	e3120002 	tst	r2, #2	; 0x2
    9988:	e3811040 	orr	r1, r1, #64	; 0x40
    998c:	13811080 	orrne	r1, r1, #128	; 0x80
    9990:	e35a0000 	cmp	sl, #0	; 0x0
    9994:	0a000003 	beq	99a8 <imap_sdhc_send_cmd+0x148>
    9998:	e59a3004 	ldr	r3, [sl, #4]
    999c:	e3811c02 	orr	r1, r1, #512	; 0x200
    99a0:	e3130002 	tst	r3, #2	; 0x2
    99a4:	13811b01 	orrne	r1, r1, #1024	; 0x400
    99a8:	e594312c 	ldr	r3, [r4, #300]
    99ac:	e1811000 	orr	r1, r1, r0
    99b0:	e3530000 	cmp	r3, #0	; 0x0
    99b4:	13a03000 	movne	r3, #0	; 0x0
    99b8:	1584312c 	strne	r3, [r4, #300]
    99bc:	e5982008 	ldr	r2, [r8, #8]
    99c0:	e5943118 	ldr	r3, [r4, #280]
    99c4:	13811902 	orrne	r1, r1, #32768	; 0x8000
    99c8:	e5832028 	str	r2, [r3, #40]
    99cc:	e5943118 	ldr	r3, [r4, #280]
    99d0:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    99d4:	e583102c 	str	r1, [r3, #44]
    99d8:	eb0006bc 	bl	b4d0 <get_ticks>
    99dc:	e3005bb8 	movw	r5, #3000	; 0xbb8
    99e0:	e1a06000 	mov	r6, r0
    99e4:	e1a07001 	mov	r7, r1
    99e8:	eb0006b8 	bl	b4d0 <get_ticks>
    99ec:	e1a02000 	mov	r2, r0
    99f0:	e1a03001 	mov	r3, r1
    99f4:	e0522006 	subs	r2, r2, r6
    99f8:	e0c33007 	sbc	r3, r3, r7
    99fc:	e3530000 	cmp	r3, #0	; 0x0
    9a00:	1a000007 	bne	9a24 <imap_sdhc_send_cmd+0x1c4>
    9a04:	e1520005 	cmp	r2, r5
    9a08:	8a000005 	bhi	9a24 <imap_sdhc_send_cmd+0x1c4>
    9a0c:	e5943118 	ldr	r3, [r4, #280]
    9a10:	e5932044 	ldr	r2, [r3, #68]
    9a14:	e2831044 	add	r1, r3, #68	; 0x44
    9a18:	e3120004 	tst	r2, #4	; 0x4
    9a1c:	0afffff1 	beq	99e8 <imap_sdhc_send_cmd+0x188>
    9a20:	ea000005 	b	9a3c <imap_sdhc_send_cmd+0x1dc>
    9a24:	e59f0148 	ldr	r0, [pc, #328]	; 9b74 <imap_sdhc_send_cmd+0x314>
    9a28:	e59f3148 	ldr	r3, [pc, #328]	; 9b78 <imap_sdhc_send_cmd+0x318>
    9a2c:	e1a0e00f 	mov	lr, pc
    9a30:	e593f000 	ldr	pc, [r3]
    9a34:	e3e00011 	mvn	r0, #17	; 0x11
    9a38:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    9a3c:	e5915000 	ldr	r5, [r1]
    9a40:	e3003142 	movw	r3, #322	; 0x142
    9a44:	e0053003 	and	r3, r5, r3
    9a48:	e3530000 	cmp	r3, #0	; 0x0
    9a4c:	e3a03004 	mov	r3, #4	; 0x4
    9a50:	e5813000 	str	r3, [r1]
    9a54:	0a00000e 	beq	9a94 <imap_sdhc_send_cmd+0x234>
    9a58:	e1d810b0 	ldrh	r1, [r8]
    9a5c:	e5943118 	ldr	r3, [r4, #280]
    9a60:	e3510037 	cmp	r1, #55	; 0x37
    9a64:	13510008 	cmpne	r1, #8	; 0x8
    9a68:	e5835044 	str	r5, [r3, #68]
    9a6c:	0a000004 	beq	9a84 <imap_sdhc_send_cmd+0x224>
    9a70:	e59f0104 	ldr	r0, [pc, #260]	; 9b7c <imap_sdhc_send_cmd+0x31c>
    9a74:	e1a02005 	mov	r2, r5
    9a78:	e59f30f8 	ldr	r3, [pc, #248]	; 9b78 <imap_sdhc_send_cmd+0x318>
    9a7c:	e1a0e00f 	mov	lr, pc
    9a80:	e593f000 	ldr	pc, [r3]
    9a84:	e3150c01 	tst	r5, #256	; 0x100
    9a88:	13e00012 	mvnne	r0, #18	; 0x12
    9a8c:	03e00011 	mvneq	r0, #17	; 0x11
    9a90:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    9a94:	e5983004 	ldr	r3, [r8, #4]
    9a98:	e5942118 	ldr	r2, [r4, #280]
    9a9c:	e3130002 	tst	r3, #2	; 0x2
    9aa0:	05923030 	ldreq	r3, [r2, #48]
    9aa4:	0588300c 	streq	r3, [r8, #12]
    9aa8:	0a00000a 	beq	9ad8 <imap_sdhc_send_cmd+0x278>
    9aac:	e592303c 	ldr	r3, [r2, #60]
    9ab0:	e588300c 	str	r3, [r8, #12]
    9ab4:	e5943118 	ldr	r3, [r4, #280]
    9ab8:	e5933038 	ldr	r3, [r3, #56]
    9abc:	e5883010 	str	r3, [r8, #16]
    9ac0:	e5943118 	ldr	r3, [r4, #280]
    9ac4:	e5933034 	ldr	r3, [r3, #52]
    9ac8:	e5883014 	str	r3, [r8, #20]
    9acc:	e5943118 	ldr	r3, [r4, #280]
    9ad0:	e5933030 	ldr	r3, [r3, #48]
    9ad4:	e5883018 	str	r3, [r8, #24]
    9ad8:	e35a0000 	cmp	sl, #0	; 0x0
    9adc:	01a0000a 	moveq	r0, sl
    9ae0:	08bd87f0 	popeq	{r4, r5, r6, r7, r8, r9, sl, pc}
    9ae4:	e594c118 	ldr	ip, [r4, #280]
    9ae8:	e59a3004 	ldr	r3, [sl, #4]
    9aec:	e28c7044 	add	r7, ip, #68	; 0x44
    9af0:	e2030001 	and	r0, r3, #1	; 0x1
    9af4:	e28c608c 	add	r6, ip, #140	; 0x8c
    9af8:	e5975000 	ldr	r5, [r7]
    9afc:	e3500000 	cmp	r0, #0	; 0x0
    9b00:	e596e000 	ldr	lr, [r6]
    9b04:	e2051008 	and	r1, r5, #8	; 0x8
    9b08:	0a000005 	beq	9b24 <imap_sdhc_send_cmd+0x2c4>
    9b0c:	e3510000 	cmp	r1, #0	; 0x0
    9b10:	0afffff8 	beq	9af8 <imap_sdhc_send_cmd+0x298>
    9b14:	e31e0002 	tst	lr, #2	; 0x2
    9b18:	0afffff6 	beq	9af8 <imap_sdhc_send_cmd+0x298>
    9b1c:	e3a03002 	mov	r3, #2	; 0x2
    9b20:	ea000004 	b	9b38 <imap_sdhc_send_cmd+0x2d8>
    9b24:	e3510000 	cmp	r1, #0	; 0x0
    9b28:	0afffff2 	beq	9af8 <imap_sdhc_send_cmd+0x298>
    9b2c:	e31e0001 	tst	lr, #1	; 0x1
    9b30:	0afffff0 	beq	9af8 <imap_sdhc_send_cmd+0x298>
    9b34:	e3a03001 	mov	r3, #1	; 0x1
    9b38:	e58c308c 	str	r3, [ip, #140]
    9b3c:	e5943118 	ldr	r3, [r4, #280]
    9b40:	e3a00000 	mov	r0, #0	; 0x0
    9b44:	e5835044 	str	r5, [r3, #68]
    9b48:	e5942118 	ldr	r2, [r4, #280]
    9b4c:	e5923000 	ldr	r3, [r2]
    9b50:	e3c33402 	bic	r3, r3, #33554432	; 0x2000000
    9b54:	e3833004 	orr	r3, r3, #4	; 0x4
    9b58:	e5823000 	str	r3, [r2]
    9b5c:	e5942118 	ldr	r2, [r4, #280]
    9b60:	e5923080 	ldr	r3, [r2, #128]
    9b64:	e3c33080 	bic	r3, r3, #128	; 0x80
    9b68:	e5823080 	str	r3, [r2, #128]
    9b6c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    9b70:	03ffe000 	.word	0x03ffe000
    9b74:	0001bd54 	.word	0x0001bd54
    9b78:	3c00d694 	.word	0x3c00d694
    9b7c:	0001bd62 	.word	0x0001bd62

00009b80 <imap_sdhc_init>:
    9b80:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    9b84:	e5901114 	ldr	r1, [r0, #276]
    9b88:	e3a02000 	mov	r2, #0	; 0x0
    9b8c:	e3a03001 	mov	r3, #1	; 0x1
    9b90:	e3510001 	cmp	r1, #1	; 0x1
    9b94:	e24dd004 	sub	sp, sp, #4	; 0x4
    9b98:	e1a04000 	mov	r4, r0
    9b9c:	e5802048 	str	r2, [r0, #72]
    9ba0:	e580312c 	str	r3, [r0, #300]
    9ba4:	e580211c 	str	r2, [r0, #284]
    9ba8:	0a000007 	beq	9bcc <imap_sdhc_init+0x4c>
    9bac:	3a000002 	bcc	9bbc <imap_sdhc_init+0x3c>
    9bb0:	e3510002 	cmp	r1, #2	; 0x2
    9bb4:	1a00000c 	bne	9bec <imap_sdhc_init+0x6c>
    9bb8:	ea000007 	b	9bdc <imap_sdhc_init+0x5c>
    9bbc:	e3a0000d 	mov	r0, #13	; 0xd
    9bc0:	ebffdb36 	bl	8a0 <pads_init_module>
    9bc4:	e59f0148 	ldr	r0, [pc, #328]	; 9d14 <imap_sdhc_init+0x194>
    9bc8:	ea000006 	b	9be8 <imap_sdhc_init+0x68>
    9bcc:	e3a0000e 	mov	r0, #14	; 0xe
    9bd0:	ebffdb32 	bl	8a0 <pads_init_module>
    9bd4:	e59f013c 	ldr	r0, [pc, #316]	; 9d18 <imap_sdhc_init+0x198>
    9bd8:	ea000002 	b	9be8 <imap_sdhc_init+0x68>
    9bdc:	e3a0000f 	mov	r0, #15	; 0xf
    9be0:	ebffdb2e 	bl	8a0 <pads_init_module>
    9be4:	e59f0130 	ldr	r0, [pc, #304]	; 9d1c <imap_sdhc_init+0x19c>
    9be8:	ebffdaf1 	bl	7b4 <module_enable>
    9bec:	e5942118 	ldr	r2, [r4, #280]
    9bf0:	e59f5128 	ldr	r5, [pc, #296]	; 9d20 <imap_sdhc_init+0x1a0>
    9bf4:	e5923070 	ldr	r3, [r2, #112]
    9bf8:	e1a03943 	asr	r3, r3, #18
    9bfc:	e2033007 	and	r3, r3, #7	; 0x7
    9c00:	e3530002 	cmp	r3, #2	; 0x2
    9c04:	03a03003 	moveq	r3, #3	; 0x3
    9c08:	13a03002 	movne	r3, #2	; 0x2
    9c0c:	e5843120 	str	r3, [r4, #288]
    9c10:	e3a03007 	mov	r3, #7	; 0x7
    9c14:	e5823000 	str	r3, [r2]
    9c18:	e59f3104 	ldr	r3, [pc, #260]	; 9d24 <imap_sdhc_init+0x1a4>
    9c1c:	e5843124 	str	r3, [r4, #292]
    9c20:	e59f3100 	ldr	r3, [pc, #256]	; 9d28 <imap_sdhc_init+0x1a8>
    9c24:	e5843128 	str	r3, [r4, #296]
    9c28:	eb000628 	bl	b4d0 <get_ticks>
    9c2c:	e1a06000 	mov	r6, r0
    9c30:	e1a07001 	mov	r7, r1
    9c34:	e5943118 	ldr	r3, [r4, #280]
    9c38:	e3a00001 	mov	r0, #1	; 0x1
    9c3c:	e5933000 	ldr	r3, [r3]
    9c40:	e3130007 	tst	r3, #7	; 0x7
    9c44:	0a00000d 	beq	9c80 <imap_sdhc_init+0x100>
    9c48:	eb000631 	bl	b514 <udelay>
    9c4c:	eb00061f 	bl	b4d0 <get_ticks>
    9c50:	e1a02000 	mov	r2, r0
    9c54:	e1a03001 	mov	r3, r1
    9c58:	e0522006 	subs	r2, r2, r6
    9c5c:	e0c33007 	sbc	r3, r3, r7
    9c60:	e3530000 	cmp	r3, #0	; 0x0
    9c64:	1a000001 	bne	9c70 <imap_sdhc_init+0xf0>
    9c68:	e1520005 	cmp	r2, r5
    9c6c:	9afffff0 	bls	9c34 <imap_sdhc_init+0xb4>
    9c70:	e59f00b4 	ldr	r0, [pc, #180]	; 9d2c <imap_sdhc_init+0x1ac>
    9c74:	e59f30b4 	ldr	r3, [pc, #180]	; 9d30 <imap_sdhc_init+0x1b0>
    9c78:	e1a0e00f 	mov	lr, pc
    9c7c:	e593f000 	ldr	pc, [r3]
    9c80:	e5943118 	ldr	r3, [r4, #280]
    9c84:	e3a01000 	mov	r1, #0	; 0x0
    9c88:	e5831024 	str	r1, [r3, #36]
    9c8c:	e5943118 	ldr	r3, [r4, #280]
    9c90:	e3e02000 	mvn	r2, #0	; 0x0
    9c94:	e5832044 	str	r2, [r3, #68]
    9c98:	e5943118 	ldr	r3, [r4, #280]
    9c9c:	e1a00001 	mov	r0, r1
    9ca0:	e5832014 	str	r2, [r3, #20]
    9ca4:	e5942118 	ldr	r2, [r4, #280]
    9ca8:	e592304c 	ldr	r3, [r2, #76]
    9cac:	e1a03283 	lsl	r3, r3, #5
    9cb0:	e1a03b23 	lsr	r3, r3, #22
    9cb4:	e1833803 	orr	r3, r3, r3, lsl #16
    9cb8:	e582304c 	str	r3, [r2, #76]
    9cbc:	e5943118 	ldr	r3, [r4, #280]
    9cc0:	e5831010 	str	r1, [r3, #16]
    9cc4:	e5942118 	ldr	r2, [r4, #280]
    9cc8:	e5923004 	ldr	r3, [r2, #4]
    9ccc:	e3833001 	orr	r3, r3, #1	; 0x1
    9cd0:	e5823004 	str	r3, [r2, #4]
    9cd4:	e5942118 	ldr	r2, [r4, #280]
    9cd8:	e59f3054 	ldr	r3, [pc, #84]	; 9d34 <imap_sdhc_init+0x1b4>
    9cdc:	e5823058 	str	r3, [r2, #88]
    9ce0:	e5942118 	ldr	r2, [r4, #280]
    9ce4:	e3003103 	movw	r3, #259	; 0x103
    9ce8:	e5823090 	str	r3, [r2, #144]
    9cec:	e5942130 	ldr	r2, [r4, #304]
    9cf0:	e5943118 	ldr	r3, [r4, #280]
    9cf4:	e5832088 	str	r2, [r3, #136]
    9cf8:	e5942118 	ldr	r2, [r4, #280]
    9cfc:	e5923080 	ldr	r3, [r2, #128]
    9d00:	e3c3307c 	bic	r3, r3, #124	; 0x7c
    9d04:	e3833008 	orr	r3, r3, #8	; 0x8
    9d08:	e5823080 	str	r3, [r2, #128]
    9d0c:	e28dd004 	add	sp, sp, #4	; 0x4
    9d10:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    9d14:	21e39000 	.word	0x21e39000
    9d18:	21e38800 	.word	0x21e38800
    9d1c:	21e38c00 	.word	0x21e38c00
    9d20:	0007a11f 	.word	0x0007a11f
    9d24:	0000965c 	.word	0x0000965c
    9d28:	00009688 	.word	0x00009688
    9d2c:	0001bd7a 	.word	0x0001bd7a
    9d30:	3c00d694 	.word	0x3c00d694
    9d34:	008c2800 	.word	0x008c2800

00009d38 <imap_sdhc_set_ios>:
    9d38:	e5902048 	ldr	r2, [r0, #72]
    9d3c:	e590311c 	ldr	r3, [r0, #284]
    9d40:	e92d4070 	push	{r4, r5, r6, lr}
    9d44:	e1520003 	cmp	r2, r3
    9d48:	e1a05000 	mov	r5, r0
    9d4c:	0a00002a 	beq	9dfc <imap_sdhc_set_ios+0xc4>
    9d50:	e5903114 	ldr	r3, [r0, #276]
    9d54:	e59f00d0 	ldr	r0, [pc, #208]	; 9e2c <imap_sdhc_set_ios+0xf4>
    9d58:	e0830000 	add	r0, r3, r0
    9d5c:	e1a00200 	lsl	r0, r0, #4
    9d60:	ebffdd4c 	bl	1298 <module_get_clock>
    9d64:	e5954048 	ldr	r4, [r5, #72]
    9d68:	e1a06000 	mov	r6, r0
    9d6c:	e1a01004 	mov	r1, r4
    9d70:	eb0021e5 	bl	1250c <__aeabi_uidivmod>
    9d74:	e3510000 	cmp	r1, #0	; 0x0
    9d78:	0a000005 	beq	9d94 <imap_sdhc_set_ios+0x5c>
    9d7c:	e1a00006 	mov	r0, r6
    9d80:	e1a01004 	mov	r1, r4
    9d84:	eb002164 	bl	1231c <__aeabi_uidiv>
    9d88:	e1a000a0 	lsr	r0, r0, #1
    9d8c:	e2806001 	add	r6, r0, #1	; 0x1
    9d90:	ea000003 	b	9da4 <imap_sdhc_set_ios+0x6c>
    9d94:	e1a00006 	mov	r0, r6
    9d98:	e1a01004 	mov	r1, r4
    9d9c:	eb00215e 	bl	1231c <__aeabi_uidiv>
    9da0:	e1a060a0 	lsr	r6, r0, #1
    9da4:	e5953118 	ldr	r3, [r5, #280]
    9da8:	e3a04000 	mov	r4, #0	; 0x0
    9dac:	e5834010 	str	r4, [r3, #16]
    9db0:	e5953118 	ldr	r3, [r5, #280]
    9db4:	e1a00005 	mov	r0, r5
    9db8:	e583400c 	str	r4, [r3, #12]
    9dbc:	e59f106c 	ldr	r1, [pc, #108]	; 9e30 <imap_sdhc_set_ios+0xf8>
    9dc0:	e1a02004 	mov	r2, r4
    9dc4:	ebfffe89 	bl	97f0 <mci_send_cmd>
    9dc8:	e5953118 	ldr	r3, [r5, #280]
    9dcc:	e1a00005 	mov	r0, r5
    9dd0:	e5836008 	str	r6, [r3, #8]
    9dd4:	e59f1054 	ldr	r1, [pc, #84]	; 9e30 <imap_sdhc_set_ios+0xf8>
    9dd8:	e1a02004 	mov	r2, r4
    9ddc:	ebfffe83 	bl	97f0 <mci_send_cmd>
    9de0:	e5952118 	ldr	r2, [r5, #280]
    9de4:	e3a03001 	mov	r3, #1	; 0x1
    9de8:	e5823010 	str	r3, [r2, #16]
    9dec:	e1a00005 	mov	r0, r5
    9df0:	e1a02004 	mov	r2, r4
    9df4:	e59f1034 	ldr	r1, [pc, #52]	; 9e30 <imap_sdhc_set_ios+0xf8>
    9df8:	ebfffe7c 	bl	97f0 <mci_send_cmd>
    9dfc:	e5953044 	ldr	r3, [r5, #68]
    9e00:	e3530004 	cmp	r3, #4	; 0x4
    9e04:	05952118 	ldreq	r2, [r5, #280]
    9e08:	03a03001 	moveq	r3, #1	; 0x1
    9e0c:	05823018 	streq	r3, [r2, #24]
    9e10:	08bd8070 	popeq	{r4, r5, r6, pc}
    9e14:	e3530008 	cmp	r3, #8	; 0x8
    9e18:	e5950118 	ldr	r0, [r5, #280]
    9e1c:	059f3010 	ldreq	r3, [pc, #16]	; 9e34 <imap_sdhc_set_ios+0xfc>
    9e20:	13a03000 	movne	r3, #0	; 0x0
    9e24:	e5803018 	str	r3, [r0, #24]
    9e28:	e8bd8070 	pop	{r4, r5, r6, pc}
    9e2c:	021e0a24 	.word	0x021e0a24
    9e30:	00202000 	.word	0x00202000
    9e34:	00010001 	.word	0x00010001

00009e38 <nand_irom_read_page_raw>:
    9e38:	e92d4030 	push	{r4, r5, lr}
    9e3c:	e3a00000 	mov	r0, #0	; 0x0
    9e40:	e24dd00c 	sub	sp, sp, #12	; 0xc
    9e44:	e58d0004 	str	r0, [sp, #4]
    9e48:	e3a0130a 	mov	r1, #671088640	; 0x28000000
    9e4c:	e2800001 	add	r0, r0, #1	; 0x1
    9e50:	e5810080 	str	r0, [r1, #128]
    9e54:	e1a04002 	mov	r4, r2
    9e58:	e1a05003 	mov	r5, r3
    9e5c:	e5913080 	ldr	r3, [r1, #128]
    9e60:	e3a0c30a 	mov	ip, #671088640	; 0x28000000
    9e64:	e203301f 	and	r3, r3, #31	; 0x1f
    9e68:	e58d3004 	str	r3, [sp, #4]
    9e6c:	e59d3004 	ldr	r3, [sp, #4]
    9e70:	e3530000 	cmp	r3, #0	; 0x0
    9e74:	1afffff8 	bne	9e5c <nand_irom_read_page_raw+0x24>
    9e78:	e59f3108 	ldr	r3, [pc, #264]	; 9f88 <nand_irom_read_page_raw+0x150>
    9e7c:	e1a02444 	asr	r2, r4, #8
    9e80:	e5d33016 	ldrb	r3, [r3, #22]
    9e84:	e3a00c41 	mov	r0, #16640	; 0x4100
    9e88:	e3530004 	cmp	r3, #4	; 0x4
    9e8c:	e3a03001 	mov	r3, #1	; 0x1
    9e90:	e58c3054 	str	r3, [ip, #84]
    9e94:	e3013c64 	movw	r3, #7268	; 0x1c64
    9e98:	e58c3060 	str	r3, [ip, #96]
    9e9c:	e3a03901 	mov	r3, #16384	; 0x4000
    9ea0:	e58c3060 	str	r3, [ip, #96]
    9ea4:	81a03844 	asrhi	r3, r4, #16
    9ea8:	820330ff 	andhi	r3, r3, #255	; 0xff
    9eac:	e20220ff 	and	r2, r2, #255	; 0xff
    9eb0:	e20410ff 	and	r1, r4, #255	; 0xff
    9eb4:	81833000 	orrhi	r3, r3, r0
    9eb8:	e3811c41 	orr	r1, r1, #16640	; 0x4100
    9ebc:	e3822c41 	orr	r2, r2, #16640	; 0x4100
    9ec0:	e58c0060 	str	r0, [ip, #96]
    9ec4:	e58c0060 	str	r0, [ip, #96]
    9ec8:	e58c1060 	str	r1, [ip, #96]
    9ecc:	e58c2060 	str	r2, [ip, #96]
    9ed0:	858c3060 	strhi	r3, [ip, #96]
    9ed4:	e3042030 	movw	r2, #16432	; 0x4030
    9ed8:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    9edc:	e5832060 	str	r2, [r3, #96]
    9ee0:	e3012c64 	movw	r2, #7268	; 0x1c64
    9ee4:	e5832060 	str	r2, [r3, #96]
    9ee8:	e2422b02 	sub	r2, r2, #2048	; 0x800
    9eec:	e5832060 	str	r2, [r3, #96]
    9ef0:	e3a01000 	mov	r1, #0	; 0x0
    9ef4:	e3a02341 	mov	r2, #67108865	; 0x4000001
    9ef8:	e5831164 	str	r1, [r3, #356]
    9efc:	e5831020 	str	r1, [r3, #32]
    9f00:	e5831024 	str	r1, [r3, #36]
    9f04:	e5831028 	str	r1, [r3, #40]
    9f08:	e583102c 	str	r1, [r3, #44]
    9f0c:	e5831030 	str	r1, [r3, #48]
    9f10:	e5832044 	str	r2, [r3, #68]
    9f14:	e3a02c03 	mov	r2, #768	; 0x300
    9f18:	e5832018 	str	r2, [r3, #24]
    9f1c:	e3a00b01 	mov	r0, #1024	; 0x400
    9f20:	e3a02001 	mov	r2, #1	; 0x1
    9f24:	e583001c 	str	r0, [r3, #28]
    9f28:	e583111c 	str	r1, [r3, #284]
    9f2c:	e5830040 	str	r0, [r3, #64]
    9f30:	e5832048 	str	r2, [r3, #72]
    9f34:	e2822b3e 	add	r2, r2, #63488	; 0xf800
    9f38:	e5835038 	str	r5, [r3, #56]
    9f3c:	e5832060 	str	r2, [r3, #96]
    9f40:	e3a0200e 	mov	r2, #14	; 0xe
    9f44:	e5831054 	str	r1, [r3, #84]
    9f48:	e583204c 	str	r2, [r3, #76]
    9f4c:	e3a02005 	mov	r2, #5	; 0x5
    9f50:	e1a0c003 	mov	ip, r3
    9f54:	e5832084 	str	r2, [r3, #132]
    9f58:	e59c308c 	ldr	r3, [ip, #140]
    9f5c:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    9f60:	e3130020 	tst	r3, #32	; 0x20
    9f64:	0afffffb 	beq	9f58 <nand_irom_read_page_raw+0x120>
    9f68:	e3a0300f 	mov	r3, #15	; 0xf
    9f6c:	e582304c 	str	r3, [r2, #76]
    9f70:	e3a00000 	mov	r0, #0	; 0x0
    9f74:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    9f78:	e582308c 	str	r3, [r2, #140]
    9f7c:	e5820164 	str	r0, [r2, #356]
    9f80:	e28dd00c 	add	sp, sp, #12	; 0xc
    9f84:	e8bd8030 	pop	{r4, r5, pc}
    9f88:	3c00d6fc 	.word	0x3c00d6fc

00009f8c <nand_irom_read_page>:
    9f8c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    9f90:	e59fc498 	ldr	ip, [pc, #1176]	; a430 <nand_irom_read_page+0x4a4>
    9f94:	e24dd044 	sub	sp, sp, #68	; 0x44
    9f98:	e5dc401a 	ldrb	r4, [ip, #26]
    9f9c:	e59fc490 	ldr	ip, [pc, #1168]	; a434 <nand_irom_read_page+0x4a8>
    9fa0:	e3540010 	cmp	r4, #16	; 0x10
    9fa4:	13a04000 	movne	r4, #0	; 0x0
    9fa8:	03a04001 	moveq	r4, #1	; 0x1
    9fac:	e59cc000 	ldr	ip, [ip]
    9fb0:	e3500000 	cmp	r0, #0	; 0x0
    9fb4:	e58dc028 	str	ip, [sp, #40]
    9fb8:	e58d4034 	str	r4, [sp, #52]
    9fbc:	e58d0020 	str	r0, [sp, #32]
    9fc0:	e1a05002 	mov	r5, r2
    9fc4:	e58d301c 	str	r3, [sp, #28]
    9fc8:	e59c6058 	ldr	r6, [ip, #88]
    9fcc:	1a000009 	bne	9ff8 <nand_irom_read_page+0x6c>
    9fd0:	e2812001 	add	r2, r1, #1	; 0x1
    9fd4:	e1a03502 	lsl	r3, r2, #10
    9fd8:	e0833102 	add	r3, r3, r2, lsl #2
    9fdc:	e3a02f46 	mov	r2, #280	; 0x118
    9fe0:	e0223192 	mla	r2, r2, r1, r3
    9fe4:	e300351c 	movw	r3, #1308	; 0x51c
    9fe8:	e0030193 	mul	r3, r3, r1
    9fec:	e58d202c 	str	r2, [sp, #44]
    9ff0:	e58d3030 	str	r3, [sp, #48]
    9ff4:	ea000007 	b	a018 <nand_irom_read_page+0x8c>
    9ff8:	e59d0028 	ldr	r0, [sp, #40]
    9ffc:	e3a03001 	mov	r3, #1	; 0x1
    a000:	e5902050 	ldr	r2, [r0, #80]
    a004:	e3a01000 	mov	r1, #0	; 0x0
    a008:	e1a03213 	lsl	r3, r3, r2
    a00c:	e2833014 	add	r3, r3, #20	; 0x14
    a010:	e58d302c 	str	r3, [sp, #44]
    a014:	e58d1030 	str	r1, [sp, #48]
    a018:	e3a02000 	mov	r2, #0	; 0x0
    a01c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    a020:	e58d203c 	str	r2, [sp, #60]
    a024:	e2822001 	add	r2, r2, #1	; 0x1
    a028:	e5832080 	str	r2, [r3, #128]
    a02c:	e1a02003 	mov	r2, r3
    a030:	e5923080 	ldr	r3, [r2, #128]
    a034:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    a038:	e203301f 	and	r3, r3, #31	; 0x1f
    a03c:	e58d303c 	str	r3, [sp, #60]
    a040:	e59d303c 	ldr	r3, [sp, #60]
    a044:	e3530000 	cmp	r3, #0	; 0x0
    a048:	1afffff8 	bne	a030 <nand_irom_read_page+0xa4>
    a04c:	e59d302c 	ldr	r3, [sp, #44]
    a050:	e59d9034 	ldr	r9, [sp, #52]
    a054:	e1a02443 	asr	r2, r3, #8
    a058:	e1a0c953 	asr	ip, r3, r9
    a05c:	e1a02952 	asr	r2, r2, r9
    a060:	e59f33c8 	ldr	r3, [pc, #968]	; a430 <nand_irom_read_page+0x4a4>
    a064:	e1a00445 	asr	r0, r5, #8
    a068:	e5d33016 	ldrb	r3, [r3, #22]
    a06c:	e20c10ff 	and	r1, ip, #255	; 0xff
    a070:	e3530004 	cmp	r3, #4	; 0x4
    a074:	e3a03001 	mov	r3, #1	; 0x1
    a078:	e5843054 	str	r3, [r4, #84]
    a07c:	e3013c64 	movw	r3, #7268	; 0x1c64
    a080:	e5843060 	str	r3, [r4, #96]
    a084:	e3a03901 	mov	r3, #16384	; 0x4000
    a088:	e5843060 	str	r3, [r4, #96]
    a08c:	81a03845 	asrhi	r3, r5, #16
    a090:	820330ff 	andhi	r3, r3, #255	; 0xff
    a094:	e58dc024 	str	ip, [sp, #36]
    a098:	e20220ff 	and	r2, r2, #255	; 0xff
    a09c:	e20000ff 	and	r0, r0, #255	; 0xff
    a0a0:	e205c0ff 	and	ip, r5, #255	; 0xff
    a0a4:	83833c41 	orrhi	r3, r3, #16640	; 0x4100
    a0a8:	e3822c41 	orr	r2, r2, #16640	; 0x4100
    a0ac:	e3800c41 	orr	r0, r0, #16640	; 0x4100
    a0b0:	e3811c41 	orr	r1, r1, #16640	; 0x4100
    a0b4:	e38ccc41 	orr	ip, ip, #16640	; 0x4100
    a0b8:	e5841060 	str	r1, [r4, #96]
    a0bc:	e3a0b30a 	mov	fp, #671088640	; 0x28000000
    a0c0:	e5842060 	str	r2, [r4, #96]
    a0c4:	e584c060 	str	ip, [r4, #96]
    a0c8:	e5840060 	str	r0, [r4, #96]
    a0cc:	85843060 	strhi	r3, [r4, #96]
    a0d0:	e59d0020 	ldr	r0, [sp, #32]
    a0d4:	e3043030 	movw	r3, #16432	; 0x4030
    a0d8:	e58b3060 	str	r3, [fp, #96]
    a0dc:	e3013c64 	movw	r3, #7268	; 0x1c64
    a0e0:	e58b3060 	str	r3, [fp, #96]
    a0e4:	e3a02001 	mov	r2, #1	; 0x1
    a0e8:	e2433b02 	sub	r3, r3, #2048	; 0x800
    a0ec:	e3500000 	cmp	r0, #0	; 0x0
    a0f0:	e58b3060 	str	r3, [fp, #96]
    a0f4:	e58b2164 	str	r2, [fp, #356]
    a0f8:	1a00003a 	bne	a1e8 <nand_irom_read_page+0x25c>
    a0fc:	e20570ff 	and	r7, r5, #255	; 0xff
    a100:	e1a03107 	lsl	r3, r7, #2
    a104:	e2833001 	add	r3, r3, #1	; 0x1
    a108:	e59f2328 	ldr	r2, [pc, #808]	; a438 <nand_irom_read_page+0x4ac>
    a10c:	e58d3018 	str	r3, [sp, #24]
    a110:	e1a00187 	lsl	r0, r7, #3
    a114:	e1a01087 	lsl	r1, r7, #1
    a118:	e59d4018 	ldr	r4, [sp, #24]
    a11c:	e1a0a287 	lsl	sl, r7, #5
    a120:	e082300a 	add	r3, r2, sl
    a124:	e2809001 	add	r9, r0, #1	; 0x1
    a128:	e2818001 	add	r8, r1, #1	; 0x1
    a12c:	e7920208 	ldr	r0, [r2, r8, lsl #4]
    a130:	e593c018 	ldr	ip, [r3, #24]
    a134:	e7926109 	ldr	r6, [r2, r9, lsl #2]
    a138:	e7921287 	ldr	r1, [r2, r7, lsl #5]
    a13c:	e593500c 	ldr	r5, [r3, #12]
    a140:	e7922184 	ldr	r2, [r2, r4, lsl #3]
    a144:	e5934014 	ldr	r4, [r3, #20]
    a148:	e593301c 	ldr	r3, [r3, #28]
    a14c:	e1811806 	orr	r1, r1, r6, lsl #16
    a150:	e18cc803 	orr	ip, ip, r3, lsl #16
    a154:	e59d3034 	ldr	r3, [sp, #52]
    a158:	e1822805 	orr	r2, r2, r5, lsl #16
    a15c:	e1800804 	orr	r0, r0, r4, lsl #16
    a160:	e3530000 	cmp	r3, #0	; 0x0
    a164:	e58b1140 	str	r1, [fp, #320]
    a168:	e58b2144 	str	r2, [fp, #324]
    a16c:	e58b0148 	str	r0, [fp, #328]
    a170:	e58bc14c 	str	ip, [fp, #332]
    a174:	0a000007 	beq	a198 <nand_irom_read_page+0x20c>
    a178:	e59f32bc 	ldr	r3, [pc, #700]	; a43c <nand_irom_read_page+0x4b0>
    a17c:	e59d4018 	ldr	r4, [sp, #24]
    a180:	e083200a 	add	r2, r3, sl
    a184:	e793c208 	ldr	ip, [r3, r8, lsl #4]
    a188:	e7936109 	ldr	r6, [r3, r9, lsl #2]
    a18c:	e7931287 	ldr	r1, [r3, r7, lsl #5]
    a190:	e7933184 	ldr	r3, [r3, r4, lsl #3]
    a194:	ea000006 	b	a1b4 <nand_irom_read_page+0x228>
    a198:	e59f32a0 	ldr	r3, [pc, #672]	; a440 <nand_irom_read_page+0x4b4>
    a19c:	e7936109 	ldr	r6, [r3, r9, lsl #2]
    a1a0:	e59d9018 	ldr	r9, [sp, #24]
    a1a4:	e083200a 	add	r2, r3, sl
    a1a8:	e793c208 	ldr	ip, [r3, r8, lsl #4]
    a1ac:	e7931287 	ldr	r1, [r3, r7, lsl #5]
    a1b0:	e7933189 	ldr	r3, [r3, r9, lsl #3]
    a1b4:	e5920018 	ldr	r0, [r2, #24]
    a1b8:	e592400c 	ldr	r4, [r2, #12]
    a1bc:	e5925014 	ldr	r5, [r2, #20]
    a1c0:	e592201c 	ldr	r2, [r2, #28]
    a1c4:	e1811806 	orr	r1, r1, r6, lsl #16
    a1c8:	e1833804 	orr	r3, r3, r4, lsl #16
    a1cc:	e18cc805 	orr	ip, ip, r5, lsl #16
    a1d0:	e1800802 	orr	r0, r0, r2, lsl #16
    a1d4:	e58b1150 	str	r1, [fp, #336]
    a1d8:	e58b3154 	str	r3, [fp, #340]
    a1dc:	e58bc158 	str	ip, [fp, #344]
    a1e0:	e58b015c 	str	r0, [fp, #348]
    a1e4:	ea000032 	b	a2b4 <nand_irom_read_page+0x328>
    a1e8:	e1a03612 	lsl	r3, r2, r6
    a1ec:	e59dc028 	ldr	ip, [sp, #40]
    a1f0:	e2433001 	sub	r3, r3, #1	; 0x1
    a1f4:	e59c2060 	ldr	r2, [ip, #96]
    a1f8:	e2422001 	sub	r2, r2, #1	; 0x1
    a1fc:	e0033002 	and	r3, r3, r2
    a200:	e0033005 	and	r3, r3, r5
    a204:	e08c1203 	add	r1, ip, r3, lsl #4
    a208:	e1a02001 	mov	r2, r1
    a20c:	e1d237b2 	ldrh	r3, [r2, #114]
    a210:	e1d106ba 	ldrh	r0, [r1, #106]
    a214:	e58d300c 	str	r3, [sp, #12]
    a218:	e1d23ebc 	ldrh	r3, [r2, #236]
    a21c:	e1d157b0 	ldrh	r5, [r1, #112]
    a220:	e1d16fb0 	ldrh	r6, [r1, #240]
    a224:	e1d17fb4 	ldrh	r7, [r1, #244]
    a228:	e58d0014 	str	r0, [sp, #20]
    a22c:	e1d146b8 	ldrh	r4, [r1, #104]
    a230:	e1d116be 	ldrh	r1, [r1, #110]
    a234:	e58d3004 	str	r3, [sp, #4]
    a238:	e59d3014 	ldr	r3, [sp, #20]
    a23c:	e58d1010 	str	r1, [sp, #16]
    a240:	e1d297b6 	ldrh	r9, [r2, #118]
    a244:	e1d2c6bc 	ldrh	ip, [r2, #108]
    a248:	e1844803 	orr	r4, r4, r3, lsl #16
    a24c:	e59d3010 	ldr	r3, [sp, #16]
    a250:	e58d9008 	str	r9, [sp, #8]
    a254:	e1d21eb8 	ldrh	r1, [r2, #232]
    a258:	e1d29eba 	ldrh	r9, [r2, #234]
    a25c:	e18cc803 	orr	ip, ip, r3, lsl #16
    a260:	e59d300c 	ldr	r3, [sp, #12]
    a264:	e1d207b4 	ldrh	r0, [r2, #116]
    a268:	e1d2aebe 	ldrh	sl, [r2, #238]
    a26c:	e1d28fb2 	ldrh	r8, [r2, #242]
    a270:	e1855803 	orr	r5, r5, r3, lsl #16
    a274:	e1d22fb6 	ldrh	r2, [r2, #246]
    a278:	e59d3008 	ldr	r3, [sp, #8]
    a27c:	e1811809 	orr	r1, r1, r9, lsl #16
    a280:	e59d9004 	ldr	r9, [sp, #4]
    a284:	e1800803 	orr	r0, r0, r3, lsl #16
    a288:	e1866808 	orr	r6, r6, r8, lsl #16
    a28c:	e189380a 	orr	r3, r9, sl, lsl #16
    a290:	e1877802 	orr	r7, r7, r2, lsl #16
    a294:	e58b4140 	str	r4, [fp, #320]
    a298:	e58bc144 	str	ip, [fp, #324]
    a29c:	e58b5148 	str	r5, [fp, #328]
    a2a0:	e58b014c 	str	r0, [fp, #332]
    a2a4:	e58b1150 	str	r1, [fp, #336]
    a2a8:	e58b3154 	str	r3, [fp, #340]
    a2ac:	e58b6158 	str	r6, [fp, #344]
    a2b0:	e58b715c 	str	r7, [fp, #348]
    a2b4:	e59dc030 	ldr	ip, [sp, #48]
    a2b8:	e59d0034 	ldr	r0, [sp, #52]
    a2bc:	e59d2020 	ldr	r2, [sp, #32]
    a2c0:	e1a0305c 	asr	r3, ip, r0
    a2c4:	e59d1024 	ldr	r1, [sp, #36]
    a2c8:	e3a0c30a 	mov	ip, #671088640	; 0x28000000
    a2cc:	e1833801 	orr	r3, r3, r1, lsl #16
    a2d0:	e3520000 	cmp	r2, #0	; 0x0
    a2d4:	e3a02000 	mov	r2, #0	; 0x0
    a2d8:	e58c2020 	str	r2, [ip, #32]
    a2dc:	e58c2024 	str	r2, [ip, #36]
    a2e0:	e58c2028 	str	r2, [ip, #40]
    a2e4:	e58c202c 	str	r2, [ip, #44]
    a2e8:	e58c3030 	str	r3, [ip, #48]
    a2ec:	e3a03341 	mov	r3, #67108865	; 0x4000001
    a2f0:	e58c3044 	str	r3, [ip, #68]
    a2f4:	1a000008 	bne	a31c <nand_irom_read_page+0x390>
    a2f8:	e59f3144 	ldr	r3, [pc, #324]	; a444 <nand_irom_read_page+0x4b8>
    a2fc:	e58c3018 	str	r3, [ip, #24]
    a300:	e59f3140 	ldr	r3, [pc, #320]	; a448 <nand_irom_read_page+0x4bc>
    a304:	e58c301c 	str	r3, [ip, #28]
    a308:	e3a03a05 	mov	r3, #20480	; 0x5000
    a30c:	e58c311c 	str	r3, [ip, #284]
    a310:	e2833bed 	add	r3, r3, #242688	; 0x3b400
    a314:	e58c3040 	str	r3, [ip, #64]
    a318:	ea000019 	b	a384 <nand_irom_read_page+0x3f8>
    a31c:	e59d3028 	ldr	r3, [sp, #40]
    a320:	e59d4028 	ldr	r4, [sp, #40]
    a324:	e5932168 	ldr	r2, [r3, #360]
    a328:	e593116c 	ldr	r1, [r3, #364]
    a32c:	e1a02202 	lsl	r2, r2, #4
    a330:	e59f3114 	ldr	r3, [pc, #276]	; a44c <nand_irom_read_page+0x4c0>
    a334:	e1822881 	orr	r2, r2, r1, lsl #17
    a338:	e1823003 	orr	r3, r2, r3
    a33c:	e58c3018 	str	r3, [ip, #24]
    a340:	e5940050 	ldr	r0, [r4, #80]
    a344:	e5941170 	ldr	r1, [r4, #368]
    a348:	e5943174 	ldr	r3, [r4, #372]
    a34c:	e240200a 	sub	r2, r0, #10	; 0xa
    a350:	e0833211 	add	r3, r3, r1, lsl r2
    a354:	e3a02001 	mov	r2, #1	; 0x1
    a358:	e1a02012 	lsl	r2, r2, r0
    a35c:	e59d902c 	ldr	r9, [sp, #44]
    a360:	e3822705 	orr	r2, r2, #1310720	; 0x140000
    a364:	e1893803 	orr	r3, r9, r3, lsl #16
    a368:	e58c301c 	str	r3, [ip, #28]
    a36c:	e5943178 	ldr	r3, [r4, #376]
    a370:	e594117c 	ldr	r1, [r4, #380]
    a374:	e1a03403 	lsl	r3, r3, #8
    a378:	e1833801 	orr	r3, r3, r1, lsl #16
    a37c:	e58c311c 	str	r3, [ip, #284]
    a380:	e58c2040 	str	r2, [ip, #64]
    a384:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    a388:	e3a03005 	mov	r3, #5	; 0x5
    a38c:	e5823048 	str	r3, [r2, #72]
    a390:	e59dc01c 	ldr	ip, [sp, #28]
    a394:	e59d3068 	ldr	r3, [sp, #104]
    a398:	e582c038 	str	ip, [r2, #56]
    a39c:	e582303c 	str	r3, [r2, #60]
    a3a0:	e30f3801 	movw	r3, #63489	; 0xf801
    a3a4:	e5823060 	str	r3, [r2, #96]
    a3a8:	e3a03000 	mov	r3, #0	; 0x0
    a3ac:	e5823054 	str	r3, [r2, #84]
    a3b0:	e283300e 	add	r3, r3, #14	; 0xe
    a3b4:	e582304c 	str	r3, [r2, #76]
    a3b8:	e3a03007 	mov	r3, #7	; 0x7
    a3bc:	e5823084 	str	r3, [r2, #132]
    a3c0:	e592308c 	ldr	r3, [r2, #140]
    a3c4:	e3a0130a 	mov	r1, #671088640	; 0x28000000
    a3c8:	e3130020 	tst	r3, #32	; 0x20
    a3cc:	0afffffb 	beq	a3c0 <nand_irom_read_page+0x434>
    a3d0:	e59d0020 	ldr	r0, [sp, #32]
    a3d4:	e3a0300f 	mov	r3, #15	; 0xf
    a3d8:	e3500000 	cmp	r0, #0	; 0x0
    a3dc:	e5910108 	ldr	r0, [r1, #264]
    a3e0:	e581304c 	str	r3, [r1, #76]
    a3e4:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    a3e8:	e581308c 	str	r3, [r1, #140]
    a3ec:	e3a03000 	mov	r3, #0	; 0x0
    a3f0:	e5813164 	str	r3, [r1, #356]
    a3f4:	1a000001 	bne	a400 <nand_irom_read_page+0x474>
    a3f8:	e3100001 	tst	r0, #1	; 0x1
    a3fc:	ea000007 	b	a420 <nand_irom_read_page+0x494>
    a400:	e59d1028 	ldr	r1, [sp, #40]
    a404:	e3a02001 	mov	r2, #1	; 0x1
    a408:	e5913050 	ldr	r3, [r1, #80]
    a40c:	e243300a 	sub	r3, r3, #10	; 0xa
    a410:	e1a03312 	lsl	r3, r2, r3
    a414:	e1a02312 	lsl	r2, r2, r3
    a418:	e2422001 	sub	r2, r2, #1	; 0x1
    a41c:	e1120000 	tst	r2, r0
    a420:	13e00000 	mvnne	r0, #0	; 0x0
    a424:	03a00000 	moveq	r0, #0	; 0x0
    a428:	e28dd044 	add	sp, sp, #68	; 0x44
    a42c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a430:	3c00d6fc 	.word	0x3c00d6fc
    a434:	3c00d1d8 	.word	0x3c00d1d8
    a438:	00013888 	.word	0x00013888
    a43c:	000179e8 	.word	0x000179e8
    a440:	000159e8 	.word	0x000159e8
    a444:	000403c9 	.word	0x000403c9
    a448:	01180404 	.word	0x01180404
    a44c:	00010309 	.word	0x00010309

0000a450 <nand_irom_set_retry_level>:
    a450:	e92d4030 	push	{r4, r5, lr}
    a454:	e59f30ec 	ldr	r3, [pc, #236]	; a548 <nand_irom_set_retry_level+0xf8>
    a458:	e59f40ec 	ldr	r4, [pc, #236]	; a54c <nand_irom_set_retry_level+0xfc>
    a45c:	e593c000 	ldr	ip, [r3]
    a460:	e24dd00c 	sub	sp, sp, #12	; 0xc
    a464:	e3a01f6e 	mov	r1, #440	; 0x1b8
    a468:	e3a02000 	mov	r2, #0	; 0x0
    a46c:	e79c3001 	ldr	r3, [ip, r1]
    a470:	e1530004 	cmp	r3, r4
    a474:	1a000002 	bne	a484 <nand_irom_set_retry_level+0x34>
    a478:	e1520000 	cmp	r2, r0
    a47c:	e2822001 	add	r2, r2, #1	; 0x1
    a480:	0a000003 	beq	a494 <nand_irom_set_retry_level+0x44>
    a484:	e2811004 	add	r1, r1, #4	; 0x4
    a488:	e3510a01 	cmp	r1, #4096	; 0x1000
    a48c:	1afffff6 	bne	a46c <nand_irom_set_retry_level+0x1c>
    a490:	ea00002a 	b	a540 <nand_irom_set_retry_level+0xf0>
    a494:	e3510a01 	cmp	r1, #4096	; 0x1000
    a498:	aa000028 	bge	a540 <nand_irom_set_retry_level+0xf0>
    a49c:	e08c3001 	add	r3, ip, r1
    a4a0:	e1d350b4 	ldrh	r5, [r3, #4]
    a4a4:	e3a03000 	mov	r3, #0	; 0x0
    a4a8:	e58d3004 	str	r3, [sp, #4]
    a4ac:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    a4b0:	e2833001 	add	r3, r3, #1	; 0x1
    a4b4:	e5823080 	str	r3, [r2, #128]
    a4b8:	e5923080 	ldr	r3, [r2, #128]
    a4bc:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    a4c0:	e203301f 	and	r3, r3, #31	; 0x1f
    a4c4:	e58d3004 	str	r3, [sp, #4]
    a4c8:	e59d0004 	ldr	r0, [sp, #4]
    a4cc:	e3500000 	cmp	r0, #0	; 0x0
    a4d0:	1afffff8 	bne	a4b8 <nand_irom_set_retry_level+0x68>
    a4d4:	e3a03001 	mov	r3, #1	; 0x1
    a4d8:	e1a02000 	mov	r2, r0
    a4dc:	e5843054 	str	r3, [r4, #84]
    a4e0:	e08c1001 	add	r1, ip, r1
    a4e4:	e1a00004 	mov	r0, r4
    a4e8:	ea000001 	b	a4f4 <nand_irom_set_retry_level+0xa4>
    a4ec:	e1d130b4 	ldrh	r3, [r1, #4]
    a4f0:	e5803060 	str	r3, [r0, #96]
    a4f4:	e1520005 	cmp	r2, r5
    a4f8:	e2811002 	add	r1, r1, #2	; 0x2
    a4fc:	e2822001 	add	r2, r2, #1	; 0x1
    a500:	bafffff9 	blt	a4ec <nand_irom_set_retry_level+0x9c>
    a504:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    a508:	e3a03000 	mov	r3, #0	; 0x0
    a50c:	e5823054 	str	r3, [r2, #84]
    a510:	e283300e 	add	r3, r3, #14	; 0xe
    a514:	e582304c 	str	r3, [r2, #76]
    a518:	e3a03001 	mov	r3, #1	; 0x1
    a51c:	e5823084 	str	r3, [r2, #132]
    a520:	e592308c 	ldr	r3, [r2, #140]
    a524:	e3a0130a 	mov	r1, #671088640	; 0x28000000
    a528:	e3130001 	tst	r3, #1	; 0x1
    a52c:	0afffffb 	beq	a520 <nand_irom_set_retry_level+0xd0>
    a530:	e3a0300f 	mov	r3, #15	; 0xf
    a534:	e581304c 	str	r3, [r1, #76]
    a538:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    a53c:	e581308c 	str	r3, [r1, #140]
    a540:	e28dd00c 	add	sp, sp, #12	; 0xc
    a544:	e8bd8030 	pop	{r4, r5, pc}
    a548:	3c00d1d8 	.word	0x3c00d1d8
    a54c:	a56aa78a 	.word	0xa56aa78a

0000a550 <nand_irom_read>:
    a550:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a554:	e59fc25c 	ldr	ip, [pc, #604]	; a7b8 <nand_irom_read+0x268>
    a558:	e24dd044 	sub	sp, sp, #68	; 0x44
    a55c:	e59c7000 	ldr	r7, [ip]
    a560:	e58d0010 	str	r0, [sp, #16]
    a564:	e1a08001 	mov	r8, r1
    a568:	e5970000 	ldr	r0, [r7]
    a56c:	e59f1248 	ldr	r1, [pc, #584]	; a7bc <nand_irom_read+0x26c>
    a570:	e58d300c 	str	r3, [sp, #12]
    a574:	e1500001 	cmp	r0, r1
    a578:	13e06000 	mvnne	r6, #0	; 0x0
    a57c:	e1a01002 	mov	r1, r2
    a580:	1a000089 	bne	a7ac <nand_irom_read+0x25c>
    a584:	e59f3234 	ldr	r3, [pc, #564]	; a7c0 <nand_irom_read+0x270>
    a588:	e597219c 	ldr	r2, [r7, #412]
    a58c:	e59dc00c 	ldr	ip, [sp, #12]
    a590:	e1520003 	cmp	r2, r3
    a594:	059721a4 	ldreq	r2, [r7, #420]
    a598:	13a00000 	movne	r0, #0	; 0x0
    a59c:	158d0024 	strne	r0, [sp, #36]
    a5a0:	058d2024 	streq	r2, [sp, #36]
    a5a4:	e3580000 	cmp	r8, #0	; 0x0
    a5a8:	059f3214 	ldreq	r3, [pc, #532]	; a7c4 <nand_irom_read+0x274>
    a5ac:	15973050 	ldrne	r3, [r7, #80]
    a5b0:	0593300c 	ldreq	r3, [r3, #12]
    a5b4:	e3a02001 	mov	r2, #1	; 0x1
    a5b8:	e58d3020 	str	r3, [sp, #32]
    a5bc:	e59d0020 	ldr	r0, [sp, #32]
    a5c0:	e24c3001 	sub	r3, ip, #1	; 0x1
    a5c4:	e0833012 	add	r3, r3, r2, lsl r0
    a5c8:	e1a01051 	asr	r1, r1, r0
    a5cc:	e1a03053 	asr	r3, r3, r0
    a5d0:	e3a06000 	mov	r6, #0	; 0x0
    a5d4:	e58d301c 	str	r3, [sp, #28]
    a5d8:	e58d1014 	str	r1, [sp, #20]
    a5dc:	e1a0b006 	mov	fp, r6
    a5e0:	ea000066 	b	a780 <nand_irom_read+0x230>
    a5e4:	e3a05000 	mov	r5, #0	; 0x0
    a5e8:	e3580002 	cmp	r8, #2	; 0x2
    a5ec:	05992030 	ldreq	r2, [r9, #48]
    a5f0:	05973058 	ldreq	r3, [r7, #88]
    a5f4:	059d1014 	ldreq	r1, [sp, #20]
    a5f8:	00812312 	addeq	r2, r1, r2, lsl r3
    a5fc:	0a000007 	beq	a620 <nand_irom_read+0xd0>
    a600:	e3580001 	cmp	r8, #1	; 0x1
    a604:	e5972058 	ldr	r2, [r7, #88]
    a608:	05993020 	ldreq	r3, [r9, #32]
    a60c:	059dc014 	ldreq	ip, [sp, #20]
    a610:	15993010 	ldrne	r3, [r9, #16]
    a614:	159d0014 	ldrne	r0, [sp, #20]
    a618:	008c2213 	addeq	r2, ip, r3, lsl r2
    a61c:	10802213 	addne	r2, r0, r3, lsl r2
    a620:	e28dc02c 	add	ip, sp, #44	; 0x2c
    a624:	e58dc000 	str	ip, [sp]
    a628:	e082a00b 	add	sl, r2, fp
    a62c:	e59dc00c 	ldr	ip, [sp, #12]
    a630:	e1a0200a 	mov	r2, sl
    a634:	e59d3018 	ldr	r3, [sp, #24]
    a638:	e1a00008 	mov	r0, r8
    a63c:	e3a01000 	mov	r1, #0	; 0x0
    a640:	e58dc004 	str	ip, [sp, #4]
    a644:	ebfffe50 	bl	9f8c <nand_irom_read_page>
    a648:	e2502000 	subs	r2, r0, #0	; 0x0
    a64c:	13a02001 	movne	r2, #1	; 0x1
    a650:	e3550000 	cmp	r5, #0	; 0x0
    a654:	13a03000 	movne	r3, #0	; 0x0
    a658:	02023001 	andeq	r3, r2, #1	; 0x1
    a65c:	e3530000 	cmp	r3, #0	; 0x0
    a660:	e1a06000 	mov	r6, r0
    a664:	0a000011 	beq	a6b0 <nand_irom_read+0x160>
    a668:	e1a0200a 	mov	r2, sl
    a66c:	e3a0cb01 	mov	ip, #1024	; 0x400
    a670:	e1a00008 	mov	r0, r8
    a674:	e3a01000 	mov	r1, #0	; 0x0
    a678:	e59d3018 	ldr	r3, [sp, #24]
    a67c:	e58dc000 	str	ip, [sp]
    a680:	ebfffdec 	bl	9e38 <nand_irom_read_page_raw>
    a684:	e3a02000 	mov	r2, #0	; 0x0
    a688:	e59d0018 	ldr	r0, [sp, #24]
    a68c:	e7d03002 	ldrb	r3, [r0, r2]
    a690:	e2822001 	add	r2, r2, #1	; 0x1
    a694:	e35300ff 	cmp	r3, #255	; 0xff
    a698:	1a000006 	bne	a6b8 <nand_irom_read+0x168>
    a69c:	e3520b01 	cmp	r2, #1024	; 0x400
    a6a0:	1afffff8 	bne	a688 <nand_irom_read+0x138>
    a6a4:	e3a05000 	mov	r5, #0	; 0x0
    a6a8:	e1a06005 	mov	r6, r5
    a6ac:	ea00000f 	b	a6f0 <nand_irom_read+0x1a0>
    a6b0:	e3520000 	cmp	r2, #0	; 0x0
    a6b4:	0a00000d 	beq	a6f0 <nand_irom_read+0x1a0>
    a6b8:	e59d1024 	ldr	r1, [sp, #36]
    a6bc:	e1550001 	cmp	r5, r1
    a6c0:	ca00000e 	bgt	a700 <nand_irom_read+0x1b0>
    a6c4:	e59741a8 	ldr	r4, [r7, #424]
    a6c8:	e2844001 	add	r4, r4, #1	; 0x1
    a6cc:	e1540001 	cmp	r4, r1
    a6d0:	c3a04000 	movgt	r4, #0	; 0x0
    a6d4:	e1a00004 	mov	r0, r4
    a6d8:	ebffff5c 	bl	a450 <nand_irom_set_retry_level>
    a6dc:	e59d2024 	ldr	r2, [sp, #36]
    a6e0:	e58741a8 	str	r4, [r7, #424]
    a6e4:	e1550002 	cmp	r5, r2
    a6e8:	0a000004 	beq	a700 <nand_irom_read+0x1b0>
    a6ec:	e2855001 	add	r5, r5, #1	; 0x1
    a6f0:	e59d3024 	ldr	r3, [sp, #36]
    a6f4:	e3530000 	cmp	r3, #0	; 0x0
    a6f8:	13560000 	cmpne	r6, #0	; 0x0
    a6fc:	1affffb9 	bne	a5e8 <nand_irom_read+0x98>
    a700:	e3550000 	cmp	r5, #0	; 0x0
    a704:	da000005 	ble	a720 <nand_irom_read+0x1d0>
    a708:	e59741a0 	ldr	r4, [r7, #416]
    a70c:	e3540000 	cmp	r4, #0	; 0x0
    a710:	1a000002 	bne	a720 <nand_irom_read+0x1d0>
    a714:	e1a00004 	mov	r0, r4
    a718:	ebffff4c 	bl	a450 <nand_irom_set_retry_level>
    a71c:	e58741a8 	str	r4, [r7, #424]
    a720:	e3560000 	cmp	r6, #0	; 0x0
    a724:	1a000009 	bne	a750 <nand_irom_read+0x200>
    a728:	e3550000 	cmp	r5, #0	; 0x0
    a72c:	da000012 	ble	a77c <nand_irom_read+0x22c>
    a730:	e1a0200a 	mov	r2, sl
    a734:	e1a03005 	mov	r3, r5
    a738:	e59f0088 	ldr	r0, [pc, #136]	; a7c8 <nand_irom_read+0x278>
    a73c:	e1a01008 	mov	r1, r8
    a740:	e59fc084 	ldr	ip, [pc, #132]	; a7cc <nand_irom_read+0x27c>
    a744:	e1a0e00f 	mov	lr, pc
    a748:	e59cf000 	ldr	pc, [ip]
    a74c:	ea00000a 	b	a77c <nand_irom_read+0x22c>
    a750:	e59f0078 	ldr	r0, [pc, #120]	; a7d0 <nand_irom_read+0x280>
    a754:	e1a0200a 	mov	r2, sl
    a758:	e1a01008 	mov	r1, r8
    a75c:	e1a03006 	mov	r3, r6
    a760:	e59fc064 	ldr	ip, [pc, #100]	; a7cc <nand_irom_read+0x27c>
    a764:	e1a0e00f 	mov	lr, pc
    a768:	e59cf000 	ldr	pc, [ip]
    a76c:	e59d0008 	ldr	r0, [sp, #8]
    a770:	e2899004 	add	r9, r9, #4	; 0x4
    a774:	e1590000 	cmp	r9, r0
    a778:	1affff99 	bne	a5e4 <nand_irom_read+0x94>
    a77c:	e28bb001 	add	fp, fp, #1	; 0x1
    a780:	e59d101c 	ldr	r1, [sp, #28]
    a784:	e15b0001 	cmp	fp, r1
    a788:	aa000007 	bge	a7ac <nand_irom_read+0x25c>
    a78c:	e59d2010 	ldr	r2, [sp, #16]
    a790:	e59d3020 	ldr	r3, [sp, #32]
    a794:	e287c010 	add	ip, r7, #16	; 0x10
    a798:	e082231b 	add	r2, r2, fp, lsl r3
    a79c:	e1a09007 	mov	r9, r7
    a7a0:	e58d2018 	str	r2, [sp, #24]
    a7a4:	e58dc008 	str	ip, [sp, #8]
    a7a8:	eaffff8d 	b	a5e4 <nand_irom_read+0x94>
    a7ac:	e1a00006 	mov	r0, r6
    a7b0:	e28dd044 	add	sp, sp, #68	; 0x44
    a7b4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a7b8:	3c00d1d8 	.word	0x3c00d1d8
    a7bc:	616f6f74 	.word	0x616f6f74
    a7c0:	349a8756 	.word	0x349a8756
    a7c4:	3c00d6fc 	.word	0x3c00d6fc
    a7c8:	0001bd8d 	.word	0x0001bd8d
    a7cc:	3c00d694 	.word	0x3c00d694
    a7d0:	0001bda6 	.word	0x0001bda6

0000a7d4 <print_nand_cfg>:
    a7d4:	e92d4030 	push	{r4, r5, lr}
    a7d8:	e1a05000 	mov	r5, r0
    a7dc:	e24dd004 	sub	sp, sp, #4	; 0x4
    a7e0:	e59f4110 	ldr	r4, [pc, #272]	; a8f8 <print_nand_cfg+0x124>
    a7e4:	e59f0110 	ldr	r0, [pc, #272]	; a8fc <print_nand_cfg+0x128>
    a7e8:	e1a0e00f 	mov	lr, pc
    a7ec:	e594f000 	ldr	pc, [r4]
    a7f0:	e5d51000 	ldrb	r1, [r5]
    a7f4:	e59f0104 	ldr	r0, [pc, #260]	; a900 <print_nand_cfg+0x12c>
    a7f8:	e1a0e00f 	mov	lr, pc
    a7fc:	e594f000 	ldr	pc, [r4]
    a800:	e5d51001 	ldrb	r1, [r5, #1]
    a804:	e59f00f8 	ldr	r0, [pc, #248]	; a904 <print_nand_cfg+0x130>
    a808:	e1a0e00f 	mov	lr, pc
    a80c:	e594f000 	ldr	pc, [r4]
    a810:	e5d51002 	ldrb	r1, [r5, #2]
    a814:	e59f00ec 	ldr	r0, [pc, #236]	; a908 <print_nand_cfg+0x134>
    a818:	e1a0e00f 	mov	lr, pc
    a81c:	e594f000 	ldr	pc, [r4]
    a820:	e5d51003 	ldrb	r1, [r5, #3]
    a824:	e59f00e0 	ldr	r0, [pc, #224]	; a90c <print_nand_cfg+0x138>
    a828:	e1a0e00f 	mov	lr, pc
    a82c:	e594f000 	ldr	pc, [r4]
    a830:	e5d51004 	ldrb	r1, [r5, #4]
    a834:	e59f00d4 	ldr	r0, [pc, #212]	; a910 <print_nand_cfg+0x13c>
    a838:	e1a0e00f 	mov	lr, pc
    a83c:	e594f000 	ldr	pc, [r4]
    a840:	e5d51005 	ldrb	r1, [r5, #5]
    a844:	e59f00c8 	ldr	r0, [pc, #200]	; a914 <print_nand_cfg+0x140>
    a848:	e1a0e00f 	mov	lr, pc
    a84c:	e594f000 	ldr	pc, [r4]
    a850:	e595100c 	ldr	r1, [r5, #12]
    a854:	e59f00bc 	ldr	r0, [pc, #188]	; a918 <print_nand_cfg+0x144>
    a858:	e1a0e00f 	mov	lr, pc
    a85c:	e594f000 	ldr	pc, [r4]
    a860:	e5951010 	ldr	r1, [r5, #16]
    a864:	e59f00b0 	ldr	r0, [pc, #176]	; a91c <print_nand_cfg+0x148>
    a868:	e1a0e00f 	mov	lr, pc
    a86c:	e594f000 	ldr	pc, [r4]
    a870:	e5951014 	ldr	r1, [r5, #20]
    a874:	e59f00a4 	ldr	r0, [pc, #164]	; a920 <print_nand_cfg+0x14c>
    a878:	e1a0e00f 	mov	lr, pc
    a87c:	e594f000 	ldr	pc, [r4]
    a880:	e5951018 	ldr	r1, [r5, #24]
    a884:	e59f0098 	ldr	r0, [pc, #152]	; a924 <print_nand_cfg+0x150>
    a888:	e1a0e00f 	mov	lr, pc
    a88c:	e594f000 	ldr	pc, [r4]
    a890:	e595101c 	ldr	r1, [r5, #28]
    a894:	e59f008c 	ldr	r0, [pc, #140]	; a928 <print_nand_cfg+0x154>
    a898:	e1a0e00f 	mov	lr, pc
    a89c:	e594f000 	ldr	pc, [r4]
    a8a0:	e5951020 	ldr	r1, [r5, #32]
    a8a4:	e59f0080 	ldr	r0, [pc, #128]	; a92c <print_nand_cfg+0x158>
    a8a8:	e1a0e00f 	mov	lr, pc
    a8ac:	e594f000 	ldr	pc, [r4]
    a8b0:	e595102c 	ldr	r1, [r5, #44]
    a8b4:	e59f0074 	ldr	r0, [pc, #116]	; a930 <print_nand_cfg+0x15c>
    a8b8:	e1a0e00f 	mov	lr, pc
    a8bc:	e594f000 	ldr	pc, [r4]
    a8c0:	e5951034 	ldr	r1, [r5, #52]
    a8c4:	e59f0068 	ldr	r0, [pc, #104]	; a934 <print_nand_cfg+0x160>
    a8c8:	e1a0e00f 	mov	lr, pc
    a8cc:	e594f000 	ldr	pc, [r4]
    a8d0:	e5951038 	ldr	r1, [r5, #56]
    a8d4:	e59f005c 	ldr	r0, [pc, #92]	; a938 <print_nand_cfg+0x164>
    a8d8:	e1a0e00f 	mov	lr, pc
    a8dc:	e594f000 	ldr	pc, [r4]
    a8e0:	e5d51006 	ldrb	r1, [r5, #6]
    a8e4:	e59f0050 	ldr	r0, [pc, #80]	; a93c <print_nand_cfg+0x168>
    a8e8:	e1a0e00f 	mov	lr, pc
    a8ec:	e594f000 	ldr	pc, [r4]
    a8f0:	e28dd004 	add	sp, sp, #4	; 0x4
    a8f4:	e8bd8030 	pop	{r4, r5, pc}
    a8f8:	3c00d694 	.word	0x3c00d694
    a8fc:	0001bdbd 	.word	0x0001bdbd
    a900:	0001bdcf 	.word	0x0001bdcf
    a904:	0001bdd7 	.word	0x0001bdd7
    a908:	0001bde7 	.word	0x0001bde7
    a90c:	0001bdf2 	.word	0x0001bdf2
    a910:	0001be02 	.word	0x0001be02
    a914:	0001be12 	.word	0x0001be12
    a918:	0001be22 	.word	0x0001be22
    a91c:	0001be2c 	.word	0x0001be2c
    a920:	0001be36 	.word	0x0001be36
    a924:	0001be43 	.word	0x0001be43
    a928:	0001be51 	.word	0x0001be51
    a92c:	0001be60 	.word	0x0001be60
    a930:	0001be6f 	.word	0x0001be6f
    a934:	0001be7b 	.word	0x0001be7b
    a938:	0001be88 	.word	0x0001be88
    a93c:	0001be96 	.word	0x0001be96

0000a940 <nand_get_config>:
    a940:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    a944:	e3500000 	cmp	r0, #0	; 0x0
    a948:	e24dd004 	sub	sp, sp, #4	; 0x4
    a94c:	0a000007 	beq	a970 <nand_get_config+0x30>
    a950:	e3500001 	cmp	r0, #1	; 0x1
    a954:	1a00000b 	bne	a988 <nand_get_config+0x48>
    a958:	e59f0048 	ldr	r0, [pc, #72]	; a9a8 <nand_get_config+0x68>
    a95c:	e5903000 	ldr	r3, [r0]
    a960:	e3530000 	cmp	r3, #0	; 0x0
    a964:	128000c0 	addne	r0, r0, #192	; 0xc0
    a968:	0a00000b 	beq	a99c <nand_get_config+0x5c>
    a96c:	ea00000b 	b	a9a0 <nand_get_config+0x60>
    a970:	e59f0030 	ldr	r0, [pc, #48]	; a9a8 <nand_get_config+0x68>
    a974:	e5903004 	ldr	r3, [r0, #4]
    a978:	e3530000 	cmp	r3, #0	; 0x0
    a97c:	12800014 	addne	r0, r0, #20	; 0x14
    a980:	0a000005 	beq	a99c <nand_get_config+0x5c>
    a984:	ea000005 	b	a9a0 <nand_get_config+0x60>
    a988:	e1a01000 	mov	r1, r0
    a98c:	e59f3018 	ldr	r3, [pc, #24]	; a9ac <nand_get_config+0x6c>
    a990:	e59f0018 	ldr	r0, [pc, #24]	; a9b0 <nand_get_config+0x70>
    a994:	e1a0e00f 	mov	lr, pc
    a998:	e593f000 	ldr	pc, [r3]
    a99c:	e3a00000 	mov	r0, #0	; 0x0
    a9a0:	e28dd004 	add	sp, sp, #4	; 0x4
    a9a4:	e8bd8000 	pop	{pc}
    a9a8:	3c00d6fc 	.word	0x3c00d6fc
    a9ac:	3c00d694 	.word	0x3c00d694
    a9b0:	0001bea0 	.word	0x0001bea0

0000a9b4 <bnd_vs_align>:
    a9b4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    a9b8:	e3a00000 	mov	r0, #0	; 0x0
    a9bc:	e24dd004 	sub	sp, sp, #4	; 0x4
    a9c0:	ebffffde 	bl	a940 <nand_get_config>
    a9c4:	e3500000 	cmp	r0, #0	; 0x0
    a9c8:	15900018 	ldrne	r0, [r0, #24]
    a9cc:	e28dd004 	add	sp, sp, #4	; 0x4
    a9d0:	e8bd8000 	pop	{pc}

0000a9d4 <nnd_vs_align>:
    a9d4:	e59f3020 	ldr	r3, [pc, #32]	; a9fc <nnd_vs_align+0x28>
    a9d8:	e5931000 	ldr	r1, [r3]
    a9dc:	e59f301c 	ldr	r3, [pc, #28]	; aa00 <nnd_vs_align+0x2c>
    a9e0:	e5912000 	ldr	r2, [r1]
    a9e4:	e1520003 	cmp	r2, r3
    a9e8:	05912050 	ldreq	r2, [r1, #80]
    a9ec:	03a03001 	moveq	r3, #1	; 0x1
    a9f0:	13a00000 	movne	r0, #0	; 0x0
    a9f4:	01a00213 	lsleq	r0, r3, r2
    a9f8:	e12fff1e 	bx	lr
    a9fc:	3c00d1d8 	.word	0x3c00d1d8
    aa00:	616f6f74 	.word	0x616f6f74

0000aa04 <fnd_vs_align>:
    aa04:	e59f3020 	ldr	r3, [pc, #32]	; aa2c <fnd_vs_align+0x28>
    aa08:	e5931000 	ldr	r1, [r3]
    aa0c:	e59f301c 	ldr	r3, [pc, #28]	; aa30 <fnd_vs_align+0x2c>
    aa10:	e5912000 	ldr	r2, [r1]
    aa14:	e1520003 	cmp	r2, r3
    aa18:	05912050 	ldreq	r2, [r1, #80]
    aa1c:	03a03001 	moveq	r3, #1	; 0x1
    aa20:	13a00000 	movne	r0, #0	; 0x0
    aa24:	01a00213 	lsleq	r0, r3, r2
    aa28:	e12fff1e 	bx	lr
    aa2c:	3c00d1d8 	.word	0x3c00d1d8
    aa30:	616f6f74 	.word	0x616f6f74

0000aa34 <fnd_align>:
    aa34:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    aa38:	e3a00001 	mov	r0, #1	; 0x1
    aa3c:	e24dd004 	sub	sp, sp, #4	; 0x4
    aa40:	ebffffbe 	bl	a940 <nand_get_config>
    aa44:	e3500000 	cmp	r0, #0	; 0x0
    aa48:	15902018 	ldrne	r2, [r0, #24]
    aa4c:	15903014 	ldrne	r3, [r0, #20]
    aa50:	10830002 	addne	r0, r3, r2
    aa54:	e28dd004 	add	sp, sp, #4	; 0x4
    aa58:	e8bd8000 	pop	{pc}

0000aa5c <fnd_size_match>:
    aa5c:	e92d4010 	push	{r4, lr}
    aa60:	e1a04000 	mov	r4, r0
    aa64:	e3a00001 	mov	r0, #1	; 0x1
    aa68:	ebffffb4 	bl	a940 <nand_get_config>
    aa6c:	e3500000 	cmp	r0, #0	; 0x0
    aa70:	08bd8010 	popeq	{r4, pc}
    aa74:	e5903018 	ldr	r3, [r0, #24]
    aa78:	e5901014 	ldr	r1, [r0, #20]
    aa7c:	e1a00004 	mov	r0, r4
    aa80:	e0811003 	add	r1, r1, r3
    aa84:	eb001ea0 	bl	1250c <__aeabi_uidivmod>
    aa88:	e2710001 	rsbs	r0, r1, #1	; 0x1
    aa8c:	33a00000 	movcc	r0, #0	; 0x0
    aa90:	e8bd8010 	pop	{r4, pc}

0000aa94 <fnd_size_shrink>:
    aa94:	e92d4030 	push	{r4, r5, lr}
    aa98:	e1a05000 	mov	r5, r0
    aa9c:	e24dd004 	sub	sp, sp, #4	; 0x4
    aaa0:	e3a00001 	mov	r0, #1	; 0x1
    aaa4:	ebffffa5 	bl	a940 <nand_get_config>
    aaa8:	e3500000 	cmp	r0, #0	; 0x0
    aaac:	0a000005 	beq	aac8 <fnd_size_shrink+0x34>
    aab0:	e5904018 	ldr	r4, [r0, #24]
    aab4:	e5901014 	ldr	r1, [r0, #20]
    aab8:	e1a00005 	mov	r0, r5
    aabc:	e0841001 	add	r1, r4, r1
    aac0:	eb001e15 	bl	1231c <__aeabi_uidiv>
    aac4:	e0000094 	mul	r0, r4, r0
    aac8:	e28dd004 	add	sp, sp, #4	; 0x4
    aacc:	e8bd8030 	pop	{r4, r5, pc}

0000aad0 <bnd_vs_erase>:
    aad0:	e3a00000 	mov	r0, #0	; 0x0
    aad4:	e12fff1e 	bx	lr

0000aad8 <fnd_vs_erase>:
    aad8:	e3a00000 	mov	r0, #0	; 0x0
    aadc:	e12fff1e 	bx	lr

0000aae0 <base_vs_erase>:
    aae0:	e3a00000 	mov	r0, #0	; 0x0
    aae4:	e12fff1e 	bx	lr

0000aae8 <nnd_vs_erase>:
    aae8:	e3a00000 	mov	r0, #0	; 0x0
    aaec:	e12fff1e 	bx	lr

0000aaf0 <nnd_vs_scrub>:
    aaf0:	e3a00000 	mov	r0, #0	; 0x0
    aaf4:	e12fff1e 	bx	lr

0000aaf8 <nnd_vs_isbad>:
    aaf8:	e3a00000 	mov	r0, #0	; 0x0
    aafc:	e12fff1e 	bx	lr

0000ab00 <bnd_vs_isbad>:
    ab00:	e3a00000 	mov	r0, #0	; 0x0
    ab04:	e12fff1e 	bx	lr

0000ab08 <fnd_vs_isbad>:
    ab08:	e3a00000 	mov	r0, #0	; 0x0
    ab0c:	e12fff1e 	bx	lr

0000ab10 <bnd_vs_read>:
    ab10:	e92d4010 	push	{r4, lr}
    ab14:	e3530000 	cmp	r3, #0	; 0x0
    ab18:	e59d4008 	ldr	r4, [sp, #8]
    ab1c:	ca000007 	bgt	ab40 <bnd_vs_read+0x30>
    ab20:	1a000001 	bne	ab2c <bnd_vs_read+0x1c>
    ab24:	e3520802 	cmp	r2, #131072	; 0x20000
    ab28:	2a000004 	bcs	ab40 <bnd_vs_read+0x30>
    ab2c:	e3a01000 	mov	r1, #0	; 0x0
    ab30:	e1a03004 	mov	r3, r4
    ab34:	ebfffe85 	bl	a550 <nand_irom_read>
    ab38:	e3500000 	cmp	r0, #0	; 0x0
    ab3c:	0a000000 	beq	ab44 <bnd_vs_read+0x34>
    ab40:	e3a04000 	mov	r4, #0	; 0x0
    ab44:	e1a00004 	mov	r0, r4
    ab48:	e8bd8010 	pop	{r4, pc}

0000ab4c <bnd_vs_write>:
    ab4c:	e3a00000 	mov	r0, #0	; 0x0
    ab50:	e12fff1e 	bx	lr

0000ab54 <nnd_vs_reset>:
    ab54:	e3a00000 	mov	r0, #0	; 0x0
    ab58:	e12fff1e 	bx	lr

0000ab5c <base_scan_badblocks>:
    ab5c:	e3a00000 	mov	r0, #0	; 0x0
    ab60:	e12fff1e 	bx	lr

0000ab64 <base_vs_read>:
    ab64:	e3a00000 	mov	r0, #0	; 0x0
    ab68:	e12fff1e 	bx	lr

0000ab6c <base_vs_write>:
    ab6c:	e3a00000 	mov	r0, #0	; 0x0
    ab70:	e12fff1e 	bx	lr

0000ab74 <nnd_vs_write>:
    ab74:	e59d0000 	ldr	r0, [sp]
    ab78:	e12fff1e 	bx	lr

0000ab7c <nnd_vs_read>:
    ab7c:	e92d4070 	push	{r4, r5, r6, lr}
    ab80:	e1a04002 	mov	r4, r2
    ab84:	e59f2064 	ldr	r2, [pc, #100]	; abf0 <nnd_vs_read+0x74>
    ab88:	e1a05003 	mov	r5, r3
    ab8c:	e0922004 	adds	r2, r2, r4
    ab90:	e3e03000 	mvn	r3, #0	; 0x0
    ab94:	e0a33005 	adc	r3, r3, r5
    ab98:	e3530000 	cmp	r3, #0	; 0x0
    ab9c:	e1a01000 	mov	r1, r0
    aba0:	e59d6010 	ldr	r6, [sp, #16]
    aba4:	1a000002 	bne	abb4 <nnd_vs_read+0x38>
    aba8:	e3520502 	cmp	r2, #8388608	; 0x800000
    abac:	32442502 	subcc	r2, r4, #8388608	; 0x800000
    abb0:	3a000006 	bcc	abd0 <nnd_vs_read+0x54>
    abb4:	e3550000 	cmp	r5, #0	; 0x0
    abb8:	ca000002 	bgt	abc8 <nnd_vs_read+0x4c>
    abbc:	1a000008 	bne	abe4 <nnd_vs_read+0x68>
    abc0:	e3540401 	cmp	r4, #16777216	; 0x1000000
    abc4:	3a000006 	bcc	abe4 <nnd_vs_read+0x68>
    abc8:	e1a00001 	mov	r0, r1
    abcc:	e28424ff 	add	r2, r4, #-16777216	; 0xff000000
    abd0:	e3a01001 	mov	r1, #1	; 0x1
    abd4:	e1a03006 	mov	r3, r6
    abd8:	ebfffe5c 	bl	a550 <nand_irom_read>
    abdc:	e3500000 	cmp	r0, #0	; 0x0
    abe0:	0a000000 	beq	abe8 <nnd_vs_read+0x6c>
    abe4:	e3a06000 	mov	r6, #0	; 0x0
    abe8:	e1a00006 	mov	r0, r6
    abec:	e8bd8070 	pop	{r4, r5, r6, pc}
    abf0:	ff800000 	.word	0xff800000

0000abf4 <fnd_vs_reset>:
    abf4:	e3a00000 	mov	r0, #0	; 0x0
    abf8:	e12fff1e 	bx	lr

0000abfc <fnd_vs_write>:
    abfc:	e3a00000 	mov	r0, #0	; 0x0
    ac00:	e12fff1e 	bx	lr

0000ac04 <fnd_vs_read>:
    ac04:	e3a00000 	mov	r0, #0	; 0x0
    ac08:	e12fff1e 	bx	lr

0000ac0c <nand_rescan_config>:
    ac0c:	e3a00000 	mov	r0, #0	; 0x0
    ac10:	e12fff1e 	bx	lr

0000ac14 <ecc_print_stat>:
    ac14:	e12fff1e 	bx	lr

0000ac18 <base_block_markbad>:
    ac18:	e3a00000 	mov	r0, #0	; 0x0
    ac1c:	e12fff1e 	bx	lr

0000ac20 <nand_rescan_bootcfg>:
    ac20:	e3a00000 	mov	r0, #0	; 0x0
    ac24:	e12fff1e 	bx	lr

0000ac28 <nand_init_randomizer>:
    ac28:	e3a00000 	mov	r0, #0	; 0x0
    ac2c:	e12fff1e 	bx	lr

0000ac30 <init_nand>:
    ac30:	e92d4010 	push	{r4, lr}
    ac34:	e3a00a01 	mov	r0, #4096	; 0x1000
    ac38:	ebffdad1 	bl	1784 <malloc>
    ac3c:	e59f303c 	ldr	r3, [pc, #60]	; ac80 <init_nand+0x50>
    ac40:	e3500000 	cmp	r0, #0	; 0x0
    ac44:	e5830000 	str	r0, [r3]
    ac48:	03e00000 	mvneq	r0, #0	; 0x0
    ac4c:	08bd8010 	popeq	{r4, pc}
    ac50:	e59f402c 	ldr	r4, [pc, #44]	; ac84 <init_nand+0x54>
    ac54:	e3a01000 	mov	r1, #0	; 0x0
    ac58:	e3a02a01 	mov	r2, #4096	; 0x1000
    ac5c:	ebffda5b 	bl	15d0 <memset>
    ac60:	e1a00004 	mov	r0, r4
    ac64:	e3a01000 	mov	r1, #0	; 0x0
    ac68:	e3a02f5b 	mov	r2, #364	; 0x16c
    ac6c:	ebffda57 	bl	15d0 <memset>
    ac70:	e3a03001 	mov	r3, #1	; 0x1
    ac74:	e5843004 	str	r3, [r4, #4]
    ac78:	e3a00000 	mov	r0, #0	; 0x0
    ac7c:	e8bd8010 	pop	{r4, pc}
    ac80:	3c00d1d8 	.word	0x3c00d1d8
    ac84:	3c00d6fc 	.word	0x3c00d6fc

0000ac88 <bnd_vs_reset>:
    ac88:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ac8c:	e3a00000 	mov	r0, #0	; 0x0
    ac90:	e24dd02c 	sub	sp, sp, #44	; 0x2c
    ac94:	ebffff29 	bl	a940 <nand_get_config>
    ac98:	e3500000 	cmp	r0, #0	; 0x0
    ac9c:	0a00018b 	beq	b2d0 <bnd_vs_reset+0x648>
    aca0:	e5d03008 	ldrb	r3, [r0, #8]
    aca4:	e3530000 	cmp	r3, #0	; 0x0
    aca8:	1a000188 	bne	b2d0 <bnd_vs_reset+0x648>
    acac:	e59f3628 	ldr	r3, [pc, #1576]	; b2dc <bnd_vs_reset+0x654>
    acb0:	e59f7628 	ldr	r7, [pc, #1576]	; b2e0 <bnd_vs_reset+0x658>
    acb4:	e5938000 	ldr	r8, [r3]
    acb8:	ebffd772 	bl	a88 <ecfg_paras>
    acbc:	e1a06000 	mov	r6, r0
    acc0:	ebffd652 	bl	610 <boot_device>
    acc4:	e3500000 	cmp	r0, #0	; 0x0
    acc8:	1a000027 	bne	ad6c <bnd_vs_reset+0xe4>
    accc:	ebffd63c 	bl	5c4 <boot_info>
    acd0:	e1a05000 	mov	r5, r0
    acd4:	e59f0608 	ldr	r0, [pc, #1544]	; b2e4 <bnd_vs_reset+0x65c>
    acd8:	ebffd742 	bl	9e8 <module_reset>
    acdc:	e59f0600 	ldr	r0, [pc, #1536]	; b2e4 <bnd_vs_reset+0x65c>
    ace0:	ebffd6b3 	bl	7b4 <module_enable>
    ace4:	e59f45fc 	ldr	r4, [pc, #1532]	; b2e8 <bnd_vs_reset+0x660>
    ace8:	e1a03425 	lsr	r3, r5, #8
    acec:	e1a023a5 	lsr	r2, r5, #7
    acf0:	e203300f 	and	r3, r3, #15	; 0xf
    acf4:	e2022001 	and	r2, r2, #1	; 0x1
    acf8:	e3a0a001 	mov	sl, #1	; 0x1
    acfc:	e6ef1073 	uxtb	r1, r3
    ad00:	e5c43017 	strb	r3, [r4, #23]
    ad04:	e5c42019 	strb	r2, [r4, #25]
    ad08:	e59f05dc 	ldr	r0, [pc, #1500]	; b2ec <bnd_vs_reset+0x664>
    ad0c:	e584a004 	str	sl, [r4, #4]
    ad10:	e1a0e00f 	mov	lr, pc
    ad14:	e597f000 	ldr	pc, [r7]
    ad18:	e3150070 	tst	r5, #112	; 0x70
    ad1c:	e1a03125 	lsr	r3, r5, #2
    ad20:	e59420d8 	ldr	r2, [r4, #216]
    ad24:	03a0c008 	moveq	ip, #8	; 0x8
    ad28:	13a0c010 	movne	ip, #16	; 0x10
    ad2c:	e2030003 	and	r0, r3, #3	; 0x3
    ad30:	e3150002 	tst	r5, #2	; 0x2
    ad34:	e3a03b01 	mov	r3, #1024	; 0x400
    ad38:	13a01005 	movne	r1, #5	; 0x5
    ad3c:	03a01004 	moveq	r1, #4	; 0x4
    ad40:	e005500a 	and	r5, r5, sl
    ad44:	e584302c 	str	r3, [r4, #44]
    ad48:	e1b02522 	lsrs	r2, r2, #10
    ad4c:	e3a0300a 	mov	r3, #10	; 0xa
    ad50:	e5c41016 	strb	r1, [r4, #22]
    ad54:	e5c45015 	strb	r5, [r4, #21]
    ad58:	e584300c 	str	r3, [r4, #12]
    ad5c:	e5c4c01a 	strb	ip, [r4, #26]
    ad60:	e5c40014 	strb	r0, [r4, #20]
    ad64:	1a000006 	bne	ad84 <bnd_vs_reset+0xfc>
    ad68:	ea000003 	b	ad7c <bnd_vs_reset+0xf4>
    ad6c:	e59f057c 	ldr	r0, [pc, #1404]	; b2f0 <bnd_vs_reset+0x668>
    ad70:	e1a0e00f 	mov	lr, pc
    ad74:	e597f000 	ldr	pc, [r7]
    ad78:	ea000154 	b	b2d0 <bnd_vs_reset+0x648>
    ad7c:	e1a03002 	mov	r3, r2
    ad80:	ea000010 	b	adc8 <bnd_vs_reset+0x140>
    ad84:	e1a03802 	lsl	r3, r2, #16
    ad88:	e1a03823 	lsr	r3, r3, #16
    ad8c:	e3530000 	cmp	r3, #0	; 0x0
    ad90:	01a02842 	asreq	r2, r2, #16
    ad94:	11a0300a 	movne	r3, sl
    ad98:	03a03011 	moveq	r3, #17	; 0x11
    ad9c:	e31200ff 	tst	r2, #255	; 0xff
    ada0:	01a02442 	asreq	r2, r2, #8
    ada4:	02833008 	addeq	r3, r3, #8	; 0x8
    ada8:	e312000f 	tst	r2, #15	; 0xf
    adac:	01a02242 	asreq	r2, r2, #4
    adb0:	02833004 	addeq	r3, r3, #4	; 0x4
    adb4:	e3120003 	tst	r2, #3	; 0x3
    adb8:	01a02142 	asreq	r2, r2, #2
    adbc:	02833002 	addeq	r3, r3, #2	; 0x2
    adc0:	e3120001 	tst	r2, #1	; 0x1
    adc4:	02833001 	addeq	r3, r3, #1	; 0x1
    adc8:	e59f4518 	ldr	r4, [pc, #1304]	; b2e8 <bnd_vs_reset+0x660>
    adcc:	e2433001 	sub	r3, r3, #1	; 0x1
    add0:	e1a02000 	mov	r2, r0
    add4:	e5843008 	str	r3, [r4, #8]
    add8:	e1a0100c 	mov	r1, ip
    addc:	e59f0510 	ldr	r0, [pc, #1296]	; b2f4 <bnd_vs_reset+0x66c>
    ade0:	e59f34f8 	ldr	r3, [pc, #1272]	; b2e0 <bnd_vs_reset+0x658>
    ade4:	e1a0e00f 	mov	lr, pc
    ade8:	e593f000 	ldr	pc, [r3]
    adec:	e3a00c02 	mov	r0, #512	; 0x200
    adf0:	ebffd71b 	bl	a64 <ecfg_check_flag>
    adf4:	e3500000 	cmp	r0, #0	; 0x0
    adf8:	0a000008 	beq	ae20 <bnd_vs_reset+0x198>
    adfc:	e30c30c0 	movw	r3, #49344	; 0xc0c0
    ae00:	e584304c 	str	r3, [r4, #76]
    ae04:	e3e034ae 	mvn	r3, #-1375731712	; 0xae000000
    ae08:	e5843040 	str	r3, [r4, #64]
    ae0c:	e59f34e4 	ldr	r3, [pc, #1252]	; b2f8 <bnd_vs_reset+0x670>
    ae10:	e5843044 	str	r3, [r4, #68]
    ae14:	e3043023 	movw	r3, #16419	; 0x4023
    ae18:	e5843048 	str	r3, [r4, #72]
    ae1c:	ea000008 	b	ae44 <bnd_vs_reset+0x1bc>
    ae20:	e5d63007 	ldrb	r3, [r6, #7]
    ae24:	e1d610ba 	ldrh	r1, [r6, #10]
    ae28:	e1a03c03 	lsl	r3, r3, #24
    ae2c:	e1e03c23 	mvn	r3, r3, lsr #24
    ae30:	e1d620bc 	ldrh	r2, [r6, #12]
    ae34:	e1e03c03 	mvn	r3, r3, lsl #24
    ae38:	e584204c 	str	r2, [r4, #76]
    ae3c:	e5843040 	str	r3, [r4, #64]
    ae40:	e5841048 	str	r1, [r4, #72]
    ae44:	e3a00c01 	mov	r0, #256	; 0x100
    ae48:	ebffd705 	bl	a64 <ecfg_check_flag>
    ae4c:	e3500000 	cmp	r0, #0	; 0x0
    ae50:	01d630be 	ldrheq	r3, [r6, #14]
    ae54:	e59f248c 	ldr	r2, [pc, #1164]	; b2e8 <bnd_vs_reset+0x660>
    ae58:	13003994 	movwne	r3, #2452	; 0x994
    ae5c:	e5823060 	str	r3, [r2, #96]
    ae60:	e59f2480 	ldr	r2, [pc, #1152]	; b2e8 <bnd_vs_reset+0x660>
    ae64:	e59f3490 	ldr	r3, [pc, #1168]	; b2fc <bnd_vs_reset+0x674>
    ae68:	e5d2501a 	ldrb	r5, [r2, #26]
    ae6c:	e58230b4 	str	r3, [r2, #180]
    ae70:	e3550010 	cmp	r5, #16	; 0x10
    ae74:	059f3484 	ldreq	r3, [pc, #1156]	; b300 <bnd_vs_reset+0x678>
    ae78:	159f3484 	ldrne	r3, [pc, #1156]	; b304 <bnd_vs_reset+0x67c>
    ae7c:	e3a00010 	mov	r0, #16	; 0x10
    ae80:	e58230b8 	str	r3, [r2, #184]
    ae84:	e59f345c 	ldr	r3, [pc, #1116]	; b2e8 <bnd_vs_reset+0x660>
    ae88:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    ae8c:	e5931060 	ldr	r1, [r3, #96]
    ae90:	e5821160 	str	r1, [r2, #352]
    ae94:	e593904c 	ldr	r9, [r3, #76]
    ae98:	e5d34014 	ldrb	r4, [r3, #20]
    ae9c:	e5936040 	ldr	r6, [r3, #64]
    aea0:	e593a044 	ldr	sl, [r3, #68]
    aea4:	e5937048 	ldr	r7, [r3, #72]
    aea8:	ebffd67c 	bl	8a0 <pads_init_module>
    aeac:	e3550010 	cmp	r5, #16	; 0x10
    aeb0:	1a000001 	bne	aebc <bnd_vs_reset+0x234>
    aeb4:	e3a00011 	mov	r0, #17	; 0x11
    aeb8:	ebffd678 	bl	8a0 <pads_init_module>
    aebc:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    aec0:	e3550010 	cmp	r5, #16	; 0x10
    aec4:	13a03000 	movne	r3, #0	; 0x0
    aec8:	03a03001 	moveq	r3, #1	; 0x1
    aecc:	e58231b8 	str	r3, [r2, #440]
    aed0:	e59230c8 	ldr	r3, [r2, #200]
    aed4:	e3540000 	cmp	r4, #0	; 0x0
    aed8:	e3c33070 	bic	r3, r3, #112	; 0x70
    aedc:	e3833040 	orr	r3, r3, #64	; 0x40
    aee0:	e58230c8 	str	r3, [r2, #200]
    aee4:	058240c4 	streq	r4, [r2, #196]
    aee8:	059230c8 	ldreq	r3, [r2, #200]
    aeec:	03833001 	orreq	r3, r3, #1	; 0x1
    aef0:	0a000005 	beq	af0c <bnd_vs_reset+0x284>
    aef4:	e3540001 	cmp	r4, #1	; 0x1
    aef8:	13a03002 	movne	r3, #2	; 0x2
    aefc:	058240c4 	streq	r4, [r2, #196]
    af00:	158230c4 	strne	r3, [r2, #196]
    af04:	e59230c8 	ldr	r3, [r2, #200]
    af08:	e3c33001 	bic	r3, r3, #1	; 0x1
    af0c:	e58230c8 	str	r3, [r2, #200]
    af10:	e3c624ff 	bic	r2, r6, #-16777216	; 0xff000000
    af14:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    af18:	e3c2260e 	bic	r2, r2, #14680064	; 0xe00000
    af1c:	e5832000 	str	r2, [r3]
    af20:	e1a01c26 	lsr	r1, r6, #24
    af24:	e30e2001 	movw	r2, #57345	; 0xe001
    af28:	e5831004 	str	r1, [r3, #4]
    af2c:	e583200c 	str	r2, [r3, #12]
    af30:	e3a02000 	mov	r2, #0	; 0x0
    af34:	e58370d0 	str	r7, [r3, #208]
    af38:	e583a008 	str	sl, [r3, #8]
    af3c:	e58390d4 	str	r9, [r3, #212]
    af40:	e58d2024 	str	r2, [sp, #36]
    af44:	e2822001 	add	r2, r2, #1	; 0x1
    af48:	e5832080 	str	r2, [r3, #128]
    af4c:	e1a02003 	mov	r2, r3
    af50:	e5923080 	ldr	r3, [r2, #128]
    af54:	e3a0130a 	mov	r1, #671088640	; 0x28000000
    af58:	e203301f 	and	r3, r3, #31	; 0x1f
    af5c:	e58d3024 	str	r3, [sp, #36]
    af60:	e59d0024 	ldr	r0, [sp, #36]
    af64:	e3500000 	cmp	r0, #0	; 0x0
    af68:	1afffff8 	bne	af50 <bnd_vs_reset+0x2c8>
    af6c:	e3a02001 	mov	r2, #1	; 0x1
    af70:	e30430ff 	movw	r3, #16639	; 0x40ff
    af74:	e5812054 	str	r2, [r1, #84]
    af78:	e5813060 	str	r3, [r1, #96]
    af7c:	e3013c64 	movw	r3, #7268	; 0x1c64
    af80:	e5813060 	str	r3, [r1, #96]
    af84:	e3a03014 	mov	r3, #20	; 0x14
    af88:	e5813060 	str	r3, [r1, #96]
    af8c:	e5810054 	str	r0, [r1, #84]
    af90:	e591304c 	ldr	r3, [r1, #76]
    af94:	e3e03001 	mvn	r3, #1	; 0x1
    af98:	e581304c 	str	r3, [r1, #76]
    af9c:	e5812084 	str	r2, [r1, #132]
    afa0:	e1a06001 	mov	r6, r1
    afa4:	eb000149 	bl	b4d0 <get_ticks>
    afa8:	e3045e20 	movw	r5, #20000	; 0x4e20
    afac:	e1a04000 	mov	r4, r0
    afb0:	eb000146 	bl	b4d0 <get_ticks>
    afb4:	e0640000 	rsb	r0, r4, r0
    afb8:	e1500005 	cmp	r0, r5
    afbc:	da000005 	ble	afd8 <bnd_vs_reset+0x350>
    afc0:	e59f0340 	ldr	r0, [pc, #832]	; b308 <bnd_vs_reset+0x680>
    afc4:	e59f3314 	ldr	r3, [pc, #788]	; b2e0 <bnd_vs_reset+0x658>
    afc8:	e1a0e00f 	mov	lr, pc
    afcc:	e593f000 	ldr	pc, [r3]
    afd0:	e3e05000 	mvn	r5, #0	; 0x0
    afd4:	ea000003 	b	afe8 <bnd_vs_reset+0x360>
    afd8:	e596308c 	ldr	r3, [r6, #140]
    afdc:	e3130001 	tst	r3, #1	; 0x1
    afe0:	0afffff2 	beq	afb0 <bnd_vs_reset+0x328>
    afe4:	e3a05000 	mov	r5, #0	; 0x0
    afe8:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    afec:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    aff0:	e583208c 	str	r2, [r3, #140]
    aff4:	e3550000 	cmp	r5, #0	; 0x0
    aff8:	e3a0200f 	mov	r2, #15	; 0xf
    affc:	e583204c 	str	r2, [r3, #76]
    b000:	1a0000b2 	bne	b2d0 <bnd_vs_reset+0x648>
    b004:	e59f42d0 	ldr	r4, [pc, #720]	; b2dc <bnd_vs_reset+0x654>
    b008:	e1a01005 	mov	r1, r5
    b00c:	e3a02a01 	mov	r2, #4096	; 0x1000
    b010:	e5940000 	ldr	r0, [r4]
    b014:	ebffd96d 	bl	15d0 <memset>
    b018:	e59f12c8 	ldr	r1, [pc, #712]	; b2e8 <bnd_vs_reset+0x660>
    b01c:	e3a03a01 	mov	r3, #4096	; 0x1000
    b020:	e591200c 	ldr	r2, [r1, #12]
    b024:	e1a06005 	mov	r6, r5
    b028:	e1a02253 	asr	r2, r3, r2
    b02c:	e1a09004 	mov	r9, r4
    b030:	e58d200c 	str	r2, [sp, #12]
    b034:	e1a0a001 	mov	sl, r1
    b038:	e1a07005 	mov	r7, r5
    b03c:	e300bfff 	movw	fp, #4095	; 0xfff
    b040:	e3a05000 	mov	r5, #0	; 0x0
    b044:	e5993000 	ldr	r3, [r9]
    b048:	e59ac02c 	ldr	ip, [sl, #44]
    b04c:	e0833006 	add	r3, r3, r6
    b050:	e58dc004 	str	ip, [sp, #4]
    b054:	e3a00000 	mov	r0, #0	; 0x0
    b058:	e28dc010 	add	ip, sp, #16	; 0x10
    b05c:	e1a01007 	mov	r1, r7
    b060:	e1a02005 	mov	r2, r5
    b064:	e58dc000 	str	ip, [sp]
    b068:	ebfffbc7 	bl	9f8c <nand_irom_read_page>
    b06c:	e3500000 	cmp	r0, #0	; 0x0
    b070:	1a00002f 	bne	b134 <bnd_vs_reset+0x4ac>
    b074:	e5991000 	ldr	r1, [r9]
    b078:	e59a202c 	ldr	r2, [sl, #44]
    b07c:	e0811006 	add	r1, r1, r6
    b080:	ebfff05a 	bl	71f0 <crc32_le>
    b084:	e59d2010 	ldr	r2, [sp, #16]
    b088:	e1500002 	cmp	r0, r2
    b08c:	0a000021 	beq	b118 <bnd_vs_reset+0x490>
    b090:	e1a01000 	mov	r1, r0
    b094:	e1a03005 	mov	r3, r5
    b098:	e59f026c 	ldr	r0, [pc, #620]	; b30c <bnd_vs_reset+0x684>
    b09c:	e59fc23c 	ldr	ip, [pc, #572]	; b2e0 <bnd_vs_reset+0x658>
    b0a0:	e58d6000 	str	r6, [sp]
    b0a4:	e1a0e00f 	mov	lr, pc
    b0a8:	e59cf000 	ldr	pc, [ip]
    b0ac:	e5990000 	ldr	r0, [r9]
    b0b0:	e0800006 	add	r0, r0, r6
    b0b4:	ebffdab5 	bl	1b90 <isi_check_header>
    b0b8:	e2504000 	subs	r4, r0, #0	; 0x0
    b0bc:	1a00003e 	bne	b1bc <bnd_vs_reset+0x534>
    b0c0:	e59f3214 	ldr	r3, [pc, #532]	; b2dc <bnd_vs_reset+0x654>
    b0c4:	e1a01004 	mov	r1, r4
    b0c8:	e5930000 	ldr	r0, [r3]
    b0cc:	e3a02a01 	mov	r2, #4096	; 0x1000
    b0d0:	ebffd93e 	bl	15d0 <memset>
    b0d4:	e2853c06 	add	r3, r5, #1536	; 0x600
    b0d8:	e588301c 	str	r3, [r8, #28]
    b0dc:	e59f322c 	ldr	r3, [pc, #556]	; b310 <bnd_vs_reset+0x688>
    b0e0:	e2852c02 	add	r2, r5, #512	; 0x200
    b0e4:	e2851b01 	add	r1, r5, #1024	; 0x400
    b0e8:	e5883000 	str	r3, [r8]
    b0ec:	e5882014 	str	r2, [r8, #20]
    b0f0:	e5881018 	str	r1, [r8, #24]
    b0f4:	e5884058 	str	r4, [r8, #88]
    b0f8:	e5885010 	str	r5, [r8, #16]
    b0fc:	e1a01005 	mov	r1, r5
    b100:	e1a02006 	mov	r2, r6
    b104:	e59f0208 	ldr	r0, [pc, #520]	; b314 <bnd_vs_reset+0x68c>
    b108:	e59f31d0 	ldr	r3, [pc, #464]	; b2e0 <bnd_vs_reset+0x658>
    b10c:	e1a0e00f 	mov	lr, pc
    b110:	e593f000 	ldr	pc, [r3]
    b114:	ea00006a 	b	b2c4 <bnd_vs_reset+0x63c>
    b118:	e3570000 	cmp	r7, #0	; 0x0
    b11c:	1a000036 	bne	b1fc <bnd_vs_reset+0x574>
    b120:	e5983000 	ldr	r3, [r8]
    b124:	e59f21e4 	ldr	r2, [pc, #484]	; b310 <bnd_vs_reset+0x688>
    b128:	e1530002 	cmp	r3, r2
    b12c:	1a00002e 	bne	b1ec <bnd_vs_reset+0x564>
    b130:	ea000031 	b	b1fc <bnd_vs_reset+0x574>
    b134:	e205307f 	and	r3, r5, #127	; 0x7f
    b138:	e1934007 	orrs	r4, r3, r7
    b13c:	1a00001e 	bne	b1bc <bnd_vs_reset+0x534>
    b140:	e5993000 	ldr	r3, [r9]
    b144:	e59ac02c 	ldr	ip, [sl, #44]
    b148:	e0833006 	add	r3, r3, r6
    b14c:	e1a01004 	mov	r1, r4
    b150:	e1a02005 	mov	r2, r5
    b154:	e1a00004 	mov	r0, r4
    b158:	e58dc000 	str	ip, [sp]
    b15c:	ebfffb35 	bl	9e38 <nand_irom_read_page_raw>
    b160:	e5993000 	ldr	r3, [r9]
    b164:	e1a00004 	mov	r0, r4
    b168:	e0831006 	add	r1, r3, r6
    b16c:	e59a202c 	ldr	r2, [sl, #44]
    b170:	ea000003 	b	b184 <bnd_vs_reset+0x4fc>
    b174:	e7d13000 	ldrb	r3, [r1, r0]
    b178:	e2800002 	add	r0, r0, #2	; 0x2
    b17c:	e35300ff 	cmp	r3, #255	; 0xff
    b180:	1a000002 	bne	b190 <bnd_vs_reset+0x508>
    b184:	e1500002 	cmp	r0, r2
    b188:	3afffff9 	bcc	b174 <bnd_vs_reset+0x4ec>
    b18c:	ea00000b 	b	b1c0 <bnd_vs_reset+0x538>
    b190:	e242c001 	sub	ip, r2, #1	; 0x1
    b194:	e3a00000 	mov	r0, #0	; 0x0
    b198:	ea000004 	b	b1b0 <bnd_vs_reset+0x528>
    b19c:	e5d12000 	ldrb	r2, [r1]
    b1a0:	e5f13001 	ldrb	r3, [r1, #1]!
    b1a4:	e1520003 	cmp	r2, r3
    b1a8:	1a000004 	bne	b1c0 <bnd_vs_reset+0x538>
    b1ac:	e2800001 	add	r0, r0, #1	; 0x1
    b1b0:	e150000c 	cmp	r0, ip
    b1b4:	3afffff8 	bcc	b19c <bnd_vs_reset+0x514>
    b1b8:	ea000000 	b	b1c0 <bnd_vs_reset+0x538>
    b1bc:	e3a00000 	mov	r0, #0	; 0x0
    b1c0:	e59a302c 	ldr	r3, [sl, #44]
    b1c4:	e2433001 	sub	r3, r3, #1	; 0x1
    b1c8:	e1500003 	cmp	r0, r3
    b1cc:	3a000006 	bcc	b1ec <bnd_vs_reset+0x564>
    b1d0:	e1a01005 	mov	r1, r5
    b1d4:	e59f013c 	ldr	r0, [pc, #316]	; b318 <bnd_vs_reset+0x690>
    b1d8:	e59f3100 	ldr	r3, [pc, #256]	; b2e0 <bnd_vs_reset+0x658>
    b1dc:	e1a0e00f 	mov	lr, pc
    b1e0:	e593f000 	ldr	pc, [r3]
    b1e4:	e2855080 	add	r5, r5, #128	; 0x80
    b1e8:	ea000000 	b	b1f0 <bnd_vs_reset+0x568>
    b1ec:	e2855001 	add	r5, r5, #1	; 0x1
    b1f0:	e155000b 	cmp	r5, fp
    b1f4:	daffff92 	ble	b044 <bnd_vs_reset+0x3bc>
    b1f8:	ea000001 	b	b204 <bnd_vs_reset+0x57c>
    b1fc:	e155000b 	cmp	r5, fp
    b200:	da000002 	ble	b210 <bnd_vs_reset+0x588>
    b204:	e3a03000 	mov	r3, #0	; 0x0
    b208:	e5883000 	str	r3, [r8]
    b20c:	ea00002f 	b	b2d0 <bnd_vs_reset+0x648>
    b210:	e5982000 	ldr	r2, [r8]
    b214:	e59f30f4 	ldr	r3, [pc, #244]	; b310 <bnd_vs_reset+0x688>
    b218:	e59a102c 	ldr	r1, [sl, #44]
    b21c:	e1520003 	cmp	r2, r3
    b220:	1a00000a 	bne	b250 <bnd_vs_reset+0x5c8>
    b224:	e5983004 	ldr	r3, [r8, #4]
    b228:	e0861001 	add	r1, r6, r1
    b22c:	e1530001 	cmp	r3, r1
    b230:	9a000006 	bls	b250 <bnd_vs_reset+0x5c8>
    b234:	e151000b 	cmp	r1, fp
    b238:	e1a06001 	mov	r6, r1
    b23c:	ca000003 	bgt	b250 <bnd_vs_reset+0x5c8>
    b240:	e59dc00c 	ldr	ip, [sp, #12]
    b244:	e2877001 	add	r7, r7, #1	; 0x1
    b248:	e157000c 	cmp	r7, ip
    b24c:	baffff7b 	blt	b040 <bnd_vs_reset+0x3b8>
    b250:	e1a01002 	mov	r1, r2
    b254:	e59f4084 	ldr	r4, [pc, #132]	; b2e0 <bnd_vs_reset+0x658>
    b258:	e5982004 	ldr	r2, [r8, #4]
    b25c:	e59f00b8 	ldr	r0, [pc, #184]	; b31c <bnd_vs_reset+0x694>
    b260:	e1a0e00f 	mov	lr, pc
    b264:	e594f000 	ldr	pc, [r4]
    b268:	e5982000 	ldr	r2, [r8]
    b26c:	e59f309c 	ldr	r3, [pc, #156]	; b310 <bnd_vs_reset+0x688>
    b270:	e1520003 	cmp	r2, r3
    b274:	1a000012 	bne	b2c4 <bnd_vs_reset+0x63c>
    b278:	e59f3068 	ldr	r3, [pc, #104]	; b2e8 <bnd_vs_reset+0x660>
    b27c:	e58380bc 	str	r8, [r3, #188]
    b280:	e598219c 	ldr	r2, [r8, #412]
    b284:	e59f3094 	ldr	r3, [pc, #148]	; b320 <bnd_vs_reset+0x698>
    b288:	e1520003 	cmp	r2, r3
    b28c:	1a00000c 	bne	b2c4 <bnd_vs_reset+0x63c>
    b290:	ebffd55b 	bl	804 <get_reset_stats>
    b294:	e3500001 	cmp	r0, #1	; 0x1
    b298:	9a000009 	bls	b2c4 <bnd_vs_reset+0x63c>
    b29c:	e59f2080 	ldr	r2, [pc, #128]	; b324 <bnd_vs_reset+0x69c>
    b2a0:	e5921c40 	ldr	r1, [r2, #3136]
    b2a4:	e3510000 	cmp	r1, #0	; 0x0
    b2a8:	0a000005 	beq	b2c4 <bnd_vs_reset+0x63c>
    b2ac:	e3a03000 	mov	r3, #0	; 0x0
    b2b0:	e58811a8 	str	r1, [r8, #424]
    b2b4:	e59f006c 	ldr	r0, [pc, #108]	; b328 <bnd_vs_reset+0x6a0>
    b2b8:	e5823c40 	str	r3, [r2, #3136]
    b2bc:	e1a0e00f 	mov	lr, pc
    b2c0:	e594f000 	ldr	pc, [r4]
    b2c4:	e59f301c 	ldr	r3, [pc, #28]	; b2e8 <bnd_vs_reset+0x660>
    b2c8:	e3a02001 	mov	r2, #1	; 0x1
    b2cc:	e5c3201c 	strb	r2, [r3, #28]
    b2d0:	e3a00000 	mov	r0, #0	; 0x0
    b2d4:	e28dd02c 	add	sp, sp, #44	; 0x2c
    b2d8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b2dc:	3c00d1d8 	.word	0x3c00d1d8
    b2e0:	3c00d694 	.word	0x3c00d694
    b2e4:	21e3c000 	.word	0x21e3c000
    b2e8:	3c00d6fc 	.word	0x3c00d6fc
    b2ec:	0001bebe 	.word	0x0001bebe
    b2f0:	0001bed8 	.word	0x0001bed8
    b2f4:	0001bef6 	.word	0x0001bef6
    b2f8:	00fffaf0 	.word	0x00fffaf0
    b2fc:	00013888 	.word	0x00013888
    b300:	000179e8 	.word	0x000179e8
    b304:	000159e8 	.word	0x000159e8
    b308:	0001bf12 	.word	0x0001bf12
    b30c:	0001bf2a 	.word	0x0001bf2a
    b310:	616f6f74 	.word	0x616f6f74
    b314:	0001bf40 	.word	0x0001bf40
    b318:	0001bf52 	.word	0x0001bf52
    b31c:	0001bf77 	.word	0x0001bf77
    b320:	349a8756 	.word	0x349a8756
    b324:	21e09000 	.word	0x21e09000
    b328:	0001bf89 	.word	0x0001bf89

0000b32c <serial_tstc>:
    b32c:	e59f3010 	ldr	r3, [pc, #16]	; b344 <serial_tstc+0x18>
    b330:	e5930018 	ldr	r0, [r3, #24]
    b334:	e1a00220 	lsr	r0, r0, #4
    b338:	e2200001 	eor	r0, r0, #1	; 0x1
    b33c:	e2000001 	and	r0, r0, #1	; 0x1
    b340:	e12fff1e 	bx	lr
    b344:	20803000 	.word	0x20803000

0000b348 <serial_putc>:
    b348:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    b34c:	e24dd004 	sub	sp, sp, #4	; 0x4
    b350:	e1a06000 	mov	r6, r0
    b354:	eb00005d 	bl	b4d0 <get_ticks>
    b358:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    b35c:	e3a05000 	mov	r5, #0	; 0x0
    b360:	e0944000 	adds	r4, r4, r0
    b364:	e59f7034 	ldr	r7, [pc, #52]	; b3a0 <serial_putc+0x58>
    b368:	e0a55001 	adc	r5, r5, r1
    b36c:	e5973018 	ldr	r3, [r7, #24]
    b370:	e3130020 	tst	r3, #32	; 0x20
    b374:	0a000005 	beq	b390 <serial_putc+0x48>
    b378:	eb000054 	bl	b4d0 <get_ticks>
    b37c:	e1510005 	cmp	r1, r5
    b380:	8a000002 	bhi	b390 <serial_putc+0x48>
    b384:	1afffff8 	bne	b36c <serial_putc+0x24>
    b388:	e1500004 	cmp	r0, r4
    b38c:	9afffff6 	bls	b36c <serial_putc+0x24>
    b390:	e59f3008 	ldr	r3, [pc, #8]	; b3a0 <serial_putc+0x58>
    b394:	e5836000 	str	r6, [r3]
    b398:	e28dd004 	add	sp, sp, #4	; 0x4
    b39c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    b3a0:	20803000 	.word	0x20803000

0000b3a4 <serial_puts>:
    b3a4:	e92d4030 	push	{r4, r5, lr}
    b3a8:	e1a05000 	mov	r5, r0
    b3ac:	e24dd004 	sub	sp, sp, #4	; 0x4
    b3b0:	e3a04000 	mov	r4, #0	; 0x0
    b3b4:	ea000006 	b	b3d4 <serial_puts+0x30>
    b3b8:	e353000a 	cmp	r3, #10	; 0xa
    b3bc:	1a000001 	bne	b3c8 <serial_puts+0x24>
    b3c0:	e3a0000d 	mov	r0, #13	; 0xd
    b3c4:	ebffffdf 	bl	b348 <serial_putc>
    b3c8:	e7d50004 	ldrb	r0, [r5, r4]
    b3cc:	ebffffdd 	bl	b348 <serial_putc>
    b3d0:	e2844001 	add	r4, r4, #1	; 0x1
    b3d4:	e7d53004 	ldrb	r3, [r5, r4]
    b3d8:	e3530000 	cmp	r3, #0	; 0x0
    b3dc:	1afffff5 	bne	b3b8 <serial_puts+0x14>
    b3e0:	e28dd004 	add	sp, sp, #4	; 0x4
    b3e4:	e8bd8030 	pop	{r4, r5, pc}

0000b3e8 <serial_getc>:
    b3e8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    b3ec:	e59f2024 	ldr	r2, [pc, #36]	; b418 <serial_getc+0x30>
    b3f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    b3f4:	e5923018 	ldr	r3, [r2, #24]
    b3f8:	e3130010 	tst	r3, #16	; 0x10
    b3fc:	1afffffc 	bne	b3f4 <serial_getc+0xc>
    b400:	ebffe502 	bl	4810 <tt_rnd_seed>
    b404:	e59f300c 	ldr	r3, [pc, #12]	; b418 <serial_getc+0x30>
    b408:	e5930000 	ldr	r0, [r3]
    b40c:	e6ef0070 	uxtb	r0, r0
    b410:	e28dd004 	add	sp, sp, #4	; 0x4
    b414:	e8bd8000 	pop	{pc}
    b418:	20803000 	.word	0x20803000

0000b41c <serial_setbrg>:
    b41c:	e92d4010 	push	{r4, lr}
    b420:	e59f0038 	ldr	r0, [pc, #56]	; b460 <serial_setbrg+0x44>
    b424:	ebffd79b 	bl	1298 <module_get_clock>
    b428:	e59f1034 	ldr	r1, [pc, #52]	; b464 <serial_setbrg+0x48>
    b42c:	e1a04000 	mov	r4, r0
    b430:	eb001c3b 	bl	12524 <__aeabi_idiv>
    b434:	e6bf3070 	sxth	r3, r0
    b438:	e59f0028 	ldr	r0, [pc, #40]	; b468 <serial_setbrg+0x4c>
    b43c:	e59f1020 	ldr	r1, [pc, #32]	; b464 <serial_setbrg+0x48>
    b440:	e0204390 	mla	r0, r0, r3, r4
    b444:	e59f4020 	ldr	r4, [pc, #32]	; b46c <serial_setbrg+0x50>
    b448:	e1a00300 	lsl	r0, r0, #6
    b44c:	e5843024 	str	r3, [r4, #36]
    b450:	eb001c33 	bl	12524 <__aeabi_idiv>
    b454:	e200003f 	and	r0, r0, #63	; 0x3f
    b458:	e5840028 	str	r0, [r4, #40]
    b45c:	e8bd8010 	pop	{r4, pc}
    b460:	21e0a290 	.word	0x21e0a290
    b464:	001c2000 	.word	0x001c2000
    b468:	ffe3e000 	.word	0xffe3e000
    b46c:	20803000 	.word	0x20803000

0000b470 <init_serial>:
    b470:	e92d4010 	push	{r4, lr}
    b474:	e59f004c 	ldr	r0, [pc, #76]	; b4c8 <init_serial+0x58>
    b478:	e59f404c 	ldr	r4, [pc, #76]	; b4cc <init_serial+0x5c>
    b47c:	ebffd4cc 	bl	7b4 <module_enable>
    b480:	e3a00013 	mov	r0, #19	; 0x13
    b484:	ebffd505 	bl	8a0 <pads_init_module>
    b488:	e594302c 	ldr	r3, [r4, #44]
    b48c:	e3c330ff 	bic	r3, r3, #255	; 0xff
    b490:	e3833060 	orr	r3, r3, #96	; 0x60
    b494:	e584302c 	str	r3, [r4, #44]
    b498:	ebffffdf 	bl	b41c <serial_setbrg>
    b49c:	e5943030 	ldr	r3, [r4, #48]
    b4a0:	e3a00000 	mov	r0, #0	; 0x0
    b4a4:	e3c33cff 	bic	r3, r3, #65280	; 0xff00
    b4a8:	e3c33087 	bic	r3, r3, #135	; 0x87
    b4ac:	e3833c03 	orr	r3, r3, #768	; 0x300
    b4b0:	e3833001 	orr	r3, r3, #1	; 0x1
    b4b4:	e5843030 	str	r3, [r4, #48]
    b4b8:	e594302c 	ldr	r3, [r4, #44]
    b4bc:	e3833010 	orr	r3, r3, #16	; 0x10
    b4c0:	e584302c 	str	r3, [r4, #44]
    b4c4:	e8bd8010 	pop	{r4, pc}
    b4c8:	21e12000 	.word	0x21e12000
    b4cc:	20803000 	.word	0x20803000

0000b4d0 <get_ticks>:
    b4d0:	e59f1038 	ldr	r1, [pc, #56]	; b510 <get_ticks+0x40>
    b4d4:	e5913204 	ldr	r3, [r1, #516]
    b4d8:	e5912200 	ldr	r2, [r1, #512]
    b4dc:	e591c204 	ldr	ip, [r1, #516]
    b4e0:	e15c0003 	cmp	ip, r3
    b4e4:	15913200 	ldrne	r3, [r1, #512]
    b4e8:	01a00002 	moveq	r0, r2
    b4ec:	03a01000 	moveq	r1, #0	; 0x0
    b4f0:	11a00003 	movne	r0, r3
    b4f4:	13a01000 	movne	r1, #0	; 0x0
    b4f8:	e3a02000 	mov	r2, #0	; 0x0
    b4fc:	e1822000 	orr	r2, r2, r0
    b500:	e18c3001 	orr	r3, ip, r1
    b504:	e1a01003 	mov	r1, r3
    b508:	e1a00002 	mov	r0, r2
    b50c:	e12fff1e 	bx	lr
    b510:	21b00000 	.word	0x21b00000

0000b514 <udelay>:
    b514:	e59f1084 	ldr	r1, [pc, #132]	; b5a0 <udelay+0x8c>
    b518:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    b51c:	e5913204 	ldr	r3, [r1, #516]
    b520:	e5912200 	ldr	r2, [r1, #512]
    b524:	e591c204 	ldr	ip, [r1, #516]
    b528:	e15c0003 	cmp	ip, r3
    b52c:	15913200 	ldrne	r3, [r1, #512]
    b530:	01a04002 	moveq	r4, r2
    b534:	03a05000 	moveq	r5, #0	; 0x0
    b538:	11a04003 	movne	r4, r3
    b53c:	13a05000 	movne	r5, #0	; 0x0
    b540:	e3a02000 	mov	r2, #0	; 0x0
    b544:	e1822004 	orr	r2, r2, r4
    b548:	e18c3005 	orr	r3, ip, r5
    b54c:	e0926000 	adds	r6, r2, r0
    b550:	e59fc048 	ldr	ip, [pc, #72]	; b5a0 <udelay+0x8c>
    b554:	e2a37000 	adc	r7, r3, #0	; 0x0
    b558:	e59c3204 	ldr	r3, [ip, #516]
    b55c:	e59c2200 	ldr	r2, [ip, #512]
    b560:	e59c1204 	ldr	r1, [ip, #516]
    b564:	e1510003 	cmp	r1, r3
    b568:	159c3200 	ldrne	r3, [ip, #512]
    b56c:	01a04002 	moveq	r4, r2
    b570:	03a05000 	moveq	r5, #0	; 0x0
    b574:	11a04003 	movne	r4, r3
    b578:	13a05000 	movne	r5, #0	; 0x0
    b57c:	e1851001 	orr	r1, r5, r1
    b580:	e3a02000 	mov	r2, #0	; 0x0
    b584:	e1570001 	cmp	r7, r1
    b588:	e1840002 	orr	r0, r4, r2
    b58c:	8afffff1 	bhi	b558 <udelay+0x44>
    b590:	18bd80f0 	popne	{r4, r5, r6, r7, pc}
    b594:	e1560000 	cmp	r6, r0
    b598:	8affffee 	bhi	b558 <udelay+0x44>
    b59c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    b5a0:	21b00000 	.word	0x21b00000

0000b5a4 <get_timer>:
    b5a4:	e59f2074 	ldr	r2, [pc, #116]	; b620 <get_timer+0x7c>
    b5a8:	e92d4070 	push	{r4, r5, r6, lr}
    b5ac:	e5923204 	ldr	r3, [r2, #516]
    b5b0:	e1a06000 	mov	r6, r0
    b5b4:	e5921200 	ldr	r1, [r2, #512]
    b5b8:	e5920204 	ldr	r0, [r2, #516]
    b5bc:	e24dd008 	sub	sp, sp, #8	; 0x8
    b5c0:	e1500003 	cmp	r0, r3
    b5c4:	15923200 	ldrne	r3, [r2, #512]
    b5c8:	01a04001 	moveq	r4, r1
    b5cc:	03a05000 	moveq	r5, #0	; 0x0
    b5d0:	11a04003 	movne	r4, r3
    b5d4:	13a05000 	movne	r5, #0	; 0x0
    b5d8:	e1a03000 	mov	r3, r0
    b5dc:	e3a02000 	mov	r2, #0	; 0x0
    b5e0:	e1851003 	orr	r1, r5, r3
    b5e4:	e1840002 	orr	r0, r4, r2
    b5e8:	e1510002 	cmp	r1, r2
    b5ec:	e88d0003 	stm	sp, {r0, r1}
    b5f0:	1a000003 	bne	b604 <get_timer+0x60>
    b5f4:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    b5f8:	eb001b47 	bl	1231c <__aeabi_uidiv>
    b5fc:	e58d0000 	str	r0, [sp]
    b600:	ea000002 	b	b610 <get_timer+0x6c>
    b604:	e1a0000d 	mov	r0, sp
    b608:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    b60c:	eb000f01 	bl	f218 <__div64_32>
    b610:	e59d0000 	ldr	r0, [sp]
    b614:	e0660000 	rsb	r0, r6, r0
    b618:	e28dd008 	add	sp, sp, #8	; 0x8
    b61c:	e8bd8070 	pop	{r4, r5, r6, pc}
    b620:	21b00000 	.word	0x21b00000

0000b624 <init_timer>:
    b624:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    b628:	e3a00003 	mov	r0, #3	; 0x3
    b62c:	e24dd004 	sub	sp, sp, #4	; 0x4
    b630:	ebffd718 	bl	1298 <module_get_clock>
    b634:	e59f1030 	ldr	r1, [pc, #48]	; b66c <init_timer+0x48>
    b638:	eb001bb9 	bl	12524 <__aeabi_idiv>
    b63c:	e2401001 	sub	r1, r0, #1	; 0x1
    b640:	e59f2028 	ldr	r2, [pc, #40]	; b670 <init_timer+0x4c>
    b644:	e1a01401 	lsl	r1, r1, #8
    b648:	e3a03000 	mov	r3, #0	; 0x0
    b64c:	e3811001 	orr	r1, r1, #1	; 0x1
    b650:	e5823208 	str	r3, [r2, #520]
    b654:	e1a00003 	mov	r0, r3
    b658:	e5823200 	str	r3, [r2, #512]
    b65c:	e5823204 	str	r3, [r2, #516]
    b660:	e5821208 	str	r1, [r2, #520]
    b664:	e28dd004 	add	sp, sp, #4	; 0x4
    b668:	e8bd8000 	pop	{pc}
    b66c:	000f4240 	.word	0x000f4240
    b670:	21b00000 	.word	0x21b00000

0000b674 <e2p_vs_align>:
    b674:	e3a00010 	mov	r0, #16	; 0x10
    b678:	e12fff1e 	bx	lr

0000b67c <e2p_vs_reset>:
    b67c:	e3a00000 	mov	r0, #0	; 0x0
    b680:	e12fff1e 	bx	lr

0000b684 <iic_address_num>:
    b684:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    b688:	e24dd004 	sub	sp, sp, #4	; 0x4
    b68c:	ebffd3cc 	bl	5c4 <boot_info>
    b690:	e59f3014 	ldr	r3, [pc, #20]	; b6ac <iic_address_num+0x28>
    b694:	e1a00080 	lsl	r0, r0, #1
    b698:	e20000ff 	and	r0, r0, #255	; 0xff
    b69c:	e5830000 	str	r0, [r3]
    b6a0:	e3a00001 	mov	r0, #1	; 0x1
    b6a4:	e28dd004 	add	sp, sp, #4	; 0x4
    b6a8:	e8bd8000 	pop	{pc}
    b6ac:	3c00d1dc 	.word	0x3c00d1dc

0000b6b0 <iic_read_get_address>:
    b6b0:	e1d020b2 	ldrh	r2, [r0, #2]
    b6b4:	e590300c 	ldr	r3, [r0, #12]
    b6b8:	e92d4010 	push	{r4, lr}
    b6bc:	e0833002 	add	r3, r3, r2
    b6c0:	e1c030b2 	strh	r3, [r0, #2]
    b6c4:	e1a04000 	mov	r4, r0
    b6c8:	ebffffed 	bl	b684 <iic_address_num>
    b6cc:	e3500000 	cmp	r0, #0	; 0x0
    b6d0:	1a000008 	bne	b6f8 <iic_read_get_address+0x48>
    b6d4:	e1d430b2 	ldrh	r3, [r4, #2]
    b6d8:	e3530c01 	cmp	r3, #256	; 0x100
    b6dc:	1a000005 	bne	b6f8 <iic_read_get_address+0x48>
    b6e0:	e1d430b0 	ldrh	r3, [r4]
    b6e4:	e1c400b2 	strh	r0, [r4, #2]
    b6e8:	e2833002 	add	r3, r3, #2	; 0x2
    b6ec:	e1c430b0 	strh	r3, [r4]
    b6f0:	e3a03001 	mov	r3, #1	; 0x1
    b6f4:	e1c431b0 	strh	r3, [r4, #16]
    b6f8:	ebffffe1 	bl	b684 <iic_address_num>
    b6fc:	e3500000 	cmp	r0, #0	; 0x0
    b700:	e5941008 	ldr	r1, [r4, #8]
    b704:	0a000004 	beq	b71c <iic_read_get_address+0x6c>
    b708:	e3510000 	cmp	r1, #0	; 0x0
    b70c:	13a03000 	movne	r3, #0	; 0x0
    b710:	15843008 	strne	r3, [r4, #8]
    b714:	1a00000e 	bne	b754 <iic_read_get_address+0xa4>
    b718:	ea000010 	b	b760 <iic_read_get_address+0xb0>
    b71c:	e1d430b2 	ldrh	r3, [r4, #2]
    b720:	e2632c01 	rsb	r2, r3, #256	; 0x100
    b724:	e1520001 	cmp	r2, r1
    b728:	e5842004 	str	r2, [r4, #4]
    b72c:	2a000005 	bcs	b748 <iic_read_get_address+0x98>
    b730:	e0623001 	rsb	r3, r2, r1
    b734:	e5843008 	str	r3, [r4, #8]
    b738:	e3a03001 	mov	r3, #1	; 0x1
    b73c:	e1c431b0 	strh	r3, [r4, #16]
    b740:	e584200c 	str	r2, [r4, #12]
    b744:	ea000003 	b	b758 <iic_read_get_address+0xa8>
    b748:	e3510000 	cmp	r1, #0	; 0x0
    b74c:	0a000003 	beq	b760 <iic_read_get_address+0xb0>
    b750:	e5840008 	str	r0, [r4, #8]
    b754:	e584100c 	str	r1, [r4, #12]
    b758:	e3a00001 	mov	r0, #1	; 0x1
    b75c:	e8bd8010 	pop	{r4, pc}
    b760:	e3a00000 	mov	r0, #0	; 0x0
    b764:	e8bd8010 	pop	{r4, pc}

0000b768 <iic_write_get_address>:
    b768:	e1d020b2 	ldrh	r2, [r0, #2]
    b76c:	e590300c 	ldr	r3, [r0, #12]
    b770:	e92d4010 	push	{r4, lr}
    b774:	e0833002 	add	r3, r3, r2
    b778:	e1c030b2 	strh	r3, [r0, #2]
    b77c:	e1a04000 	mov	r4, r0
    b780:	ebffffbf 	bl	b684 <iic_address_num>
    b784:	e3500000 	cmp	r0, #0	; 0x0
    b788:	1a000008 	bne	b7b0 <iic_write_get_address+0x48>
    b78c:	e1d430b2 	ldrh	r3, [r4, #2]
    b790:	e3530c01 	cmp	r3, #256	; 0x100
    b794:	1a000005 	bne	b7b0 <iic_write_get_address+0x48>
    b798:	e1d430b0 	ldrh	r3, [r4]
    b79c:	e1c400b2 	strh	r0, [r4, #2]
    b7a0:	e2833002 	add	r3, r3, #2	; 0x2
    b7a4:	e1c430b0 	strh	r3, [r4]
    b7a8:	e3a03001 	mov	r3, #1	; 0x1
    b7ac:	e1c431b0 	strh	r3, [r4, #16]
    b7b0:	e1d430b2 	ldrh	r3, [r4, #2]
    b7b4:	e5940008 	ldr	r0, [r4, #8]
    b7b8:	e203300f 	and	r3, r3, #15	; 0xf
    b7bc:	e2632010 	rsb	r2, r3, #16	; 0x10
    b7c0:	e1520000 	cmp	r2, r0
    b7c4:	e5842004 	str	r2, [r4, #4]
    b7c8:	2a000004 	bcs	b7e0 <iic_write_get_address+0x78>
    b7cc:	e0623000 	rsb	r3, r2, r0
    b7d0:	e5843008 	str	r3, [r4, #8]
    b7d4:	e584200c 	str	r2, [r4, #12]
    b7d8:	e3a00001 	mov	r0, #1	; 0x1
    b7dc:	e8bd8010 	pop	{r4, pc}
    b7e0:	e3500000 	cmp	r0, #0	; 0x0
    b7e4:	13a03000 	movne	r3, #0	; 0x0
    b7e8:	1584000c 	strne	r0, [r4, #12]
    b7ec:	15843008 	strne	r3, [r4, #8]
    b7f0:	13a00001 	movne	r0, #1	; 0x1
    b7f4:	e8bd8010 	pop	{r4, pc}

0000b7f8 <iic_word_addr_func>:
    b7f8:	e92d4030 	push	{r4, r5, lr}
    b7fc:	e24dd004 	sub	sp, sp, #4	; 0x4
    b800:	e1a04000 	mov	r4, r0
    b804:	e1a05001 	mov	r5, r1
    b808:	ebffff9d 	bl	b684 <iic_address_num>
    b80c:	e3500000 	cmp	r0, #0	; 0x0
    b810:	16ff0074 	uxthne	r0, r4
    b814:	1a00000c 	bne	b84c <iic_word_addr_func+0x54>
    b818:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    b81c:	e0051003 	and	r1, r5, r3
    b820:	e3a020ff 	mov	r2, #255	; 0xff
    b824:	e3510000 	cmp	r1, #0	; 0x0
    b828:	e0040002 	and	r0, r4, r2
    b82c:	aa000005 	bge	b848 <iic_word_addr_func+0x50>
    b830:	e3e02000 	mvn	r2, #0	; 0x0
    b834:	e0900002 	adds	r0, r0, r2
    b838:	e3e020ff 	mvn	r2, #255	; 0xff
    b83c:	e1800002 	orr	r0, r0, r2
    b840:	e3a02001 	mov	r2, #1	; 0x1
    b844:	e0900002 	adds	r0, r0, r2
    b848:	e6ff0070 	uxth	r0, r0
    b84c:	e28dd004 	add	sp, sp, #4	; 0x4
    b850:	e8bd8030 	pop	{r4, r5, pc}

0000b854 <iic_addr_func>:
    b854:	e92d4030 	push	{r4, r5, lr}
    b858:	e24dd004 	sub	sp, sp, #4	; 0x4
    b85c:	e1a04000 	mov	r4, r0
    b860:	e1a05001 	mov	r5, r1
    b864:	ebffff86 	bl	b684 <iic_address_num>
    b868:	e3500000 	cmp	r0, #0	; 0x0
    b86c:	e59f003c 	ldr	r0, [pc, #60]	; b8b0 <iic_addr_func+0x5c>
    b870:	11d000b0 	ldrhne	r0, [r0]
    b874:	1a00000b 	bne	b8a8 <iic_addr_func+0x54>
    b878:	e1a02005 	mov	r2, r5
    b87c:	e1a03fc2 	asr	r3, r2, #31
    b880:	e1a02fc3 	asr	r2, r3, #31
    b884:	e20220ff 	and	r2, r2, #255	; 0xff
    b888:	e3a03000 	mov	r3, #0	; 0x0
    b88c:	e0922004 	adds	r2, r2, r4
    b890:	e0a33005 	adc	r3, r3, r5
    b894:	e5900000 	ldr	r0, [r0]
    b898:	e1a01422 	lsr	r1, r2, #8
    b89c:	e1811c03 	orr	r1, r1, r3, lsl #24
    b8a0:	e1800081 	orr	r0, r0, r1, lsl #1
    b8a4:	e6ff0070 	uxth	r0, r0
    b8a8:	e28dd004 	add	sp, sp, #4	; 0x4
    b8ac:	e8bd8030 	pop	{r4, r5, pc}
    b8b0:	3c00d1dc 	.word	0x3c00d1dc

0000b8b4 <e2p_vs_write>:
    b8b4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b8b8:	e1a01003 	mov	r1, r3
    b8bc:	e24dd024 	sub	sp, sp, #36	; 0x24
    b8c0:	e1a06000 	mov	r6, r0
    b8c4:	e1a00002 	mov	r0, r2
    b8c8:	e1a05003 	mov	r5, r3
    b8cc:	e1a04002 	mov	r4, r2
    b8d0:	ebffffdf 	bl	b854 <iic_addr_func>
    b8d4:	e1a01005 	mov	r1, r5
    b8d8:	e1cd00bc 	strh	r0, [sp, #12]
    b8dc:	e1a00004 	mov	r0, r4
    b8e0:	ebffffc4 	bl	b7f8 <iic_word_addr_func>
    b8e4:	e3a03000 	mov	r3, #0	; 0x0
    b8e8:	e59d2048 	ldr	r2, [sp, #72]
    b8ec:	e1a05003 	mov	r5, r3
    b8f0:	e58d3018 	str	r3, [sp, #24]
    b8f4:	e28dc00c 	add	ip, sp, #12	; 0xc
    b8f8:	e3a03001 	mov	r3, #1	; 0x1
    b8fc:	e1cd00be 	strh	r0, [sp, #14]
    b900:	e58d2014 	str	r2, [sp, #20]
    b904:	e1cd31bc 	strh	r3, [sp, #28]
    b908:	e59fb10c 	ldr	fp, [pc, #268]	; ba1c <e2p_vs_write+0x168>
    b90c:	e3a0a001 	mov	sl, #1	; 0x1
    b910:	e1a08005 	mov	r8, r5
    b914:	e28c9012 	add	r9, ip, #18	; 0x12
    b918:	ea000037 	b	b9fc <e2p_vs_write+0x148>
    b91c:	ebffff58 	bl	b684 <iic_address_num>
    b920:	e3500000 	cmp	r0, #0	; 0x0
    b924:	e1dd10be 	ldrh	r1, [sp, #14]
    b928:	11a03421 	lsrne	r3, r1, #8
    b92c:	15cd301e 	strbne	r3, [sp, #30]
    b930:	15cd101f 	strbne	r1, [sp, #31]
    b934:	13a07002 	movne	r7, #2	; 0x2
    b938:	1a000005 	bne	b954 <e2p_vs_write+0xa0>
    b93c:	e6ef1071 	uxtb	r1, r1
    b940:	e5cd101e 	strb	r1, [sp, #30]
    b944:	e59f00d4 	ldr	r0, [pc, #212]	; ba20 <e2p_vs_write+0x16c>
    b948:	e1a0e00f 	mov	lr, pc
    b94c:	e59bf000 	ldr	pc, [fp]
    b950:	e3a07001 	mov	r7, #1	; 0x1
    b954:	ebfffedd 	bl	b4d0 <get_ticks>
    b958:	e1dd31bc 	ldrh	r3, [sp, #28]
    b95c:	e1a04000 	mov	r4, r0
    b960:	e3530000 	cmp	r3, #0	; 0x0
    b964:	0a000016 	beq	b9c4 <e2p_vs_write+0x110>
    b968:	e3a0c000 	mov	ip, #0	; 0x0
    b96c:	e5dd200c 	ldrb	r2, [sp, #12]
    b970:	e1a00008 	mov	r0, r8
    b974:	e1a0100a 	mov	r1, sl
    b978:	e1a03008 	mov	r3, r8
    b97c:	e1cdc1bc 	strh	ip, [sp, #28]
    b980:	e58da000 	str	sl, [sp]
    b984:	e58d8004 	str	r8, [sp, #4]
    b988:	eb000137 	bl	be6c <iic_init>
    b98c:	e3500000 	cmp	r0, #0	; 0x0
    b990:	1a00000b 	bne	b9c4 <e2p_vs_write+0x110>
    b994:	e1a00008 	mov	r0, r8
    b998:	ea00001d 	b	ba14 <e2p_vs_write+0x160>
    b99c:	ebfffecb 	bl	b4d0 <get_ticks>
    b9a0:	e1540000 	cmp	r4, r0
    b9a4:	2a000008 	bcs	b9cc <e2p_vs_write+0x118>
    b9a8:	e59f0074 	ldr	r0, [pc, #116]	; ba24 <e2p_vs_write+0x170>
    b9ac:	e1a0e00f 	mov	lr, pc
    b9b0:	e59bf000 	ldr	pc, [fp]
    b9b4:	e59d3018 	ldr	r3, [sp, #24]
    b9b8:	e0853003 	add	r3, r5, r3
    b9bc:	e6ff5073 	uxth	r5, r3
    b9c0:	ea00000b 	b	b9f4 <e2p_vs_write+0x140>
    b9c4:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    b9c8:	e2844e12 	add	r4, r4, #288	; 0x120
    b9cc:	e59dc018 	ldr	ip, [sp, #24]
    b9d0:	e1a01009 	mov	r1, r9
    b9d4:	e1a02007 	mov	r2, r7
    b9d8:	e1a03006 	mov	r3, r6
    b9dc:	e3a00000 	mov	r0, #0	; 0x0
    b9e0:	e58dc000 	str	ip, [sp]
    b9e4:	e58da004 	str	sl, [sp, #4]
    b9e8:	eb000078 	bl	bbd0 <iicreg_write>
    b9ec:	e3500000 	cmp	r0, #0	; 0x0
    b9f0:	0affffe9 	beq	b99c <e2p_vs_write+0xe8>
    b9f4:	e59d3018 	ldr	r3, [sp, #24]
    b9f8:	e0866003 	add	r6, r6, r3
    b9fc:	e28d000c 	add	r0, sp, #12	; 0xc
    ba00:	ebffff58 	bl	b768 <iic_write_get_address>
    ba04:	e3500000 	cmp	r0, #0	; 0x0
    ba08:	1affffc3 	bne	b91c <e2p_vs_write+0x68>
    ba0c:	e59d2048 	ldr	r2, [sp, #72]
    ba10:	e0650002 	rsb	r0, r5, r2
    ba14:	e28dd024 	add	sp, sp, #36	; 0x24
    ba18:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ba1c:	3c00d694 	.word	0x3c00d694
    ba20:	0001bf9b 	.word	0x0001bf9b
    ba24:	0001bfa5 	.word	0x0001bfa5

0000ba28 <e2p_vs_read>:
    ba28:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ba2c:	e1a01003 	mov	r1, r3
    ba30:	e24dd024 	sub	sp, sp, #36	; 0x24
    ba34:	e1a06000 	mov	r6, r0
    ba38:	e1a00002 	mov	r0, r2
    ba3c:	e1a05003 	mov	r5, r3
    ba40:	e1a04002 	mov	r4, r2
    ba44:	e59d7048 	ldr	r7, [sp, #72]
    ba48:	ebffff81 	bl	b854 <iic_addr_func>
    ba4c:	e1a01005 	mov	r1, r5
    ba50:	e1cd00bc 	strh	r0, [sp, #12]
    ba54:	e1a00004 	mov	r0, r4
    ba58:	ebffff66 	bl	b7f8 <iic_word_addr_func>
    ba5c:	e28db00c 	add	fp, sp, #12	; 0xc
    ba60:	e3a03000 	mov	r3, #0	; 0x0
    ba64:	e3a02001 	mov	r2, #1	; 0x1
    ba68:	e1cd00be 	strh	r0, [sp, #14]
    ba6c:	e1cd21bc 	strh	r2, [sp, #28]
    ba70:	e58d7014 	str	r7, [sp, #20]
    ba74:	e58d3018 	str	r3, [sp, #24]
    ba78:	e1a05003 	mov	r5, r3
    ba7c:	e3a09001 	mov	r9, #1	; 0x1
    ba80:	e28ba012 	add	sl, fp, #18	; 0x12
    ba84:	ea00002b 	b	bb38 <e2p_vs_read+0x110>
    ba88:	ebfffefd 	bl	b684 <iic_address_num>
    ba8c:	e1dd20be 	ldrh	r2, [sp, #14]
    ba90:	e3500000 	cmp	r0, #0	; 0x0
    ba94:	11a03422 	lsrne	r3, r2, #8
    ba98:	15cd301e 	strbne	r3, [sp, #30]
    ba9c:	15cd201f 	strbne	r2, [sp, #31]
    baa0:	13a08002 	movne	r8, #2	; 0x2
    baa4:	05cd201e 	strbeq	r2, [sp, #30]
    baa8:	03a08001 	moveq	r8, #1	; 0x1
    baac:	ebfffe87 	bl	b4d0 <get_ticks>
    bab0:	e1dd31bc 	ldrh	r3, [sp, #28]
    bab4:	e1a04000 	mov	r4, r0
    bab8:	e3530000 	cmp	r3, #0	; 0x0
    babc:	0a000010 	beq	bb04 <e2p_vs_read+0xdc>
    bac0:	e3a0c000 	mov	ip, #0	; 0x0
    bac4:	e5dd200c 	ldrb	r2, [sp, #12]
    bac8:	e1a00005 	mov	r0, r5
    bacc:	e1a01009 	mov	r1, r9
    bad0:	e1a03005 	mov	r3, r5
    bad4:	e1cdc1bc 	strh	ip, [sp, #28]
    bad8:	e58d9000 	str	r9, [sp]
    badc:	e58d5004 	str	r5, [sp, #4]
    bae0:	eb0000e1 	bl	be6c <iic_init>
    bae4:	e3500000 	cmp	r0, #0	; 0x0
    bae8:	1a000005 	bne	bb04 <e2p_vs_read+0xdc>
    baec:	e1a07005 	mov	r7, r5
    baf0:	ea000014 	b	bb48 <e2p_vs_read+0x120>
    baf4:	ebfffe75 	bl	b4d0 <get_ticks>
    baf8:	e1540000 	cmp	r4, r0
    bafc:	2a000002 	bcs	bb0c <e2p_vs_read+0xe4>
    bb00:	ea00000a 	b	bb30 <e2p_vs_read+0x108>
    bb04:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    bb08:	e2844e12 	add	r4, r4, #288	; 0x120
    bb0c:	e59dc018 	ldr	ip, [sp, #24]
    bb10:	e1a0100a 	mov	r1, sl
    bb14:	e1a02008 	mov	r2, r8
    bb18:	e1a03006 	mov	r3, r6
    bb1c:	e3a00000 	mov	r0, #0	; 0x0
    bb20:	e58dc000 	str	ip, [sp]
    bb24:	eb000061 	bl	bcb0 <iic_read_func>
    bb28:	e3500000 	cmp	r0, #0	; 0x0
    bb2c:	0afffff0 	beq	baf4 <e2p_vs_read+0xcc>
    bb30:	e59d3018 	ldr	r3, [sp, #24]
    bb34:	e0866003 	add	r6, r6, r3
    bb38:	e1a0000b 	mov	r0, fp
    bb3c:	ebfffedb 	bl	b6b0 <iic_read_get_address>
    bb40:	e3500000 	cmp	r0, #0	; 0x0
    bb44:	1affffcf 	bne	ba88 <e2p_vs_read+0x60>
    bb48:	e1a00007 	mov	r0, r7
    bb4c:	e28dd024 	add	sp, sp, #36	; 0x24
    bb50:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

0000bb54 <iicreg_read>:
    bb54:	e3500001 	cmp	r0, #1	; 0x1
    bb58:	e92d4070 	push	{r4, r5, r6, lr}
    bb5c:	83a00000 	movhi	r0, #0	; 0x0
    bb60:	e1a05001 	mov	r5, r1
    bb64:	e1a04002 	mov	r4, r2
    bb68:	88bd8070 	pophi	{r4, r5, r6, pc}
    bb6c:	e59f3058 	ldr	r3, [pc, #88]	; bbcc <iicreg_read+0x78>
    bb70:	e5932070 	ldr	r2, [r3, #112]
    bb74:	e2022008 	and	r2, r2, #8	; 0x8
    bb78:	ea000000 	b	bb80 <iicreg_read+0x2c>
    bb7c:	e5931010 	ldr	r1, [r3, #16]
    bb80:	e3520000 	cmp	r2, #0	; 0x0
    bb84:	1afffffc 	bne	bb7c <iicreg_read+0x28>
    bb88:	e59f103c 	ldr	r1, [pc, #60]	; bbcc <iicreg_read+0x78>
    bb8c:	e1a00004 	mov	r0, r4
    bb90:	e3a06c01 	mov	r6, #256	; 0x100
    bb94:	e591c070 	ldr	ip, [r1, #112]
    bb98:	e2503000 	subs	r3, r0, #0	; 0x0
    bb9c:	13a03001 	movne	r3, #1	; 0x1
    bba0:	e01330ac 	ands	r3, r3, ip, lsr #1
    bba4:	15816010 	strne	r6, [r1, #16]
    bba8:	12400001 	subne	r0, r0, #1	; 0x1
    bbac:	e31c0008 	tst	ip, #8	; 0x8
    bbb0:	15913010 	ldrne	r3, [r1, #16]
    bbb4:	12822001 	addne	r2, r2, #1	; 0x1
    bbb8:	14c53001 	strbne	r3, [r5], #1
    bbbc:	e1520004 	cmp	r2, r4
    bbc0:	1afffff3 	bne	bb94 <iicreg_read+0x40>
    bbc4:	e3a00001 	mov	r0, #1	; 0x1
    bbc8:	e8bd8070 	pop	{r4, r5, r6, pc}
    bbcc:	20300000 	.word	0x20300000

0000bbd0 <iicreg_write>:
    bbd0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    bbd4:	e59d4018 	ldr	r4, [sp, #24]
    bbd8:	e3500001 	cmp	r0, #1	; 0x1
    bbdc:	e1a05001 	mov	r5, r1
    bbe0:	e5dd801c 	ldrb	r8, [sp, #28]
    bbe4:	e1a01003 	mov	r1, r3
    bbe8:	e0847002 	add	r7, r4, r2
    bbec:	8a00002c 	bhi	bca4 <iicreg_write+0xd4>
    bbf0:	e59f00b4 	ldr	r0, [pc, #180]	; bcac <iicreg_write+0xdc>
    bbf4:	e5903034 	ldr	r3, [r0, #52]
    bbf8:	e213c040 	ands	ip, r3, #64	; 0x40
    bbfc:	15903080 	ldrne	r3, [r0, #128]
    bc00:	15903054 	ldrne	r3, [r0, #84]
    bc04:	13a00000 	movne	r0, #0	; 0x0
    bc08:	18bd81f0 	popne	{r4, r5, r6, r7, r8, pc}
    bc0c:	e5903070 	ldr	r3, [r0, #112]
    bc10:	e3130004 	tst	r3, #4	; 0x4
    bc14:	11a06000 	movne	r6, r0
    bc18:	11a0000c 	movne	r0, ip
    bc1c:	11a0c006 	movne	ip, r6
    bc20:	1a00000d 	bne	bc5c <iicreg_write+0x8c>
    bc24:	ea00001e 	b	bca4 <iicreg_write+0xd4>
    bc28:	e5963070 	ldr	r3, [r6, #112]
    bc2c:	e3130002 	tst	r3, #2	; 0x2
    bc30:	0afffffc 	beq	bc28 <iicreg_write+0x58>
    bc34:	e3520000 	cmp	r2, #0	; 0x0
    bc38:	14d53001 	ldrbne	r3, [r5], #1
    bc3c:	12422001 	subne	r2, r2, #1	; 0x1
    bc40:	1a000003 	bne	bc54 <iicreg_write+0x84>
    bc44:	e3540000 	cmp	r4, #0	; 0x0
    bc48:	0a000002 	beq	bc58 <iicreg_write+0x88>
    bc4c:	e4d13001 	ldrb	r3, [r1], #1
    bc50:	e2444001 	sub	r4, r4, #1	; 0x1
    bc54:	e58c3010 	str	r3, [ip, #16]
    bc58:	e2800001 	add	r0, r0, #1	; 0x1
    bc5c:	e1500007 	cmp	r0, r7
    bc60:	3afffff0 	bcc	bc28 <iicreg_write+0x58>
    bc64:	e59f2040 	ldr	r2, [pc, #64]	; bcac <iicreg_write+0xdc>
    bc68:	e5923070 	ldr	r3, [r2, #112]
    bc6c:	e3130004 	tst	r3, #4	; 0x4
    bc70:	0afffffc 	beq	bc68 <iicreg_write+0x98>
    bc74:	e3580000 	cmp	r8, #0	; 0x0
    bc78:	159f202c 	ldrne	r2, [pc, #44]	; bcac <iicreg_write+0xdc>
    bc7c:	0a000006 	beq	bc9c <iicreg_write+0xcc>
    bc80:	e5923034 	ldr	r3, [r2, #52]
    bc84:	e3130c02 	tst	r3, #512	; 0x200
    bc88:	0afffffc 	beq	bc80 <iicreg_write+0xb0>
    bc8c:	e59f3018 	ldr	r3, [pc, #24]	; bcac <iicreg_write+0xdc>
    bc90:	e3010964 	movw	r0, #6500	; 0x1964
    bc94:	e5933060 	ldr	r3, [r3, #96]
    bc98:	ebfffe1d 	bl	b514 <udelay>
    bc9c:	e3a00001 	mov	r0, #1	; 0x1
    bca0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    bca4:	e3a00000 	mov	r0, #0	; 0x0
    bca8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    bcac:	20300000 	.word	0x20300000

0000bcb0 <iic_read_func>:
    bcb0:	e92d4070 	push	{r4, r5, r6, lr}
    bcb4:	e59f4080 	ldr	r4, [pc, #128]	; bd3c <iic_read_func+0x8c>
    bcb8:	e1a06003 	mov	r6, r3
    bcbc:	e5d43000 	ldrb	r3, [r4]
    bcc0:	e24dd008 	sub	sp, sp, #8	; 0x8
    bcc4:	e243300a 	sub	r3, r3, #10	; 0xa
    bcc8:	e6ef3073 	uxtb	r3, r3
    bccc:	e3530001 	cmp	r3, #1	; 0x1
    bcd0:	e1a05000 	mov	r5, r0
    bcd4:	9a00000b 	bls	bd08 <iic_read_func+0x58>
    bcd8:	e3a0e000 	mov	lr, #0	; 0x0
    bcdc:	e1a0300e 	mov	r3, lr
    bce0:	e3a0c001 	mov	ip, #1	; 0x1
    bce4:	e58dc004 	str	ip, [sp, #4]
    bce8:	e58de000 	str	lr, [sp]
    bcec:	ebffffb7 	bl	bbd0 <iicreg_write>
    bcf0:	e3500000 	cmp	r0, #0	; 0x0
    bcf4:	03a0300c 	moveq	r3, #12	; 0xc
    bcf8:	05c43000 	strbeq	r3, [r4]
    bcfc:	0a00000c 	beq	bd34 <iic_read_func+0x84>
    bd00:	e3a0300a 	mov	r3, #10	; 0xa
    bd04:	e5c43000 	strb	r3, [r4]
    bd08:	e59d2018 	ldr	r2, [sp, #24]
    bd0c:	e1a00005 	mov	r0, r5
    bd10:	e1a01006 	mov	r1, r6
    bd14:	ebffff8e 	bl	bb54 <iicreg_read>
    bd18:	e59f201c 	ldr	r2, [pc, #28]	; bd3c <iic_read_func+0x8c>
    bd1c:	e3500000 	cmp	r0, #0	; 0x0
    bd20:	13a0300d 	movne	r3, #13	; 0xd
    bd24:	03a0300b 	moveq	r3, #11	; 0xb
    bd28:	15c23000 	strbne	r3, [r2]
    bd2c:	13a00001 	movne	r0, #1	; 0x1
    bd30:	05c23000 	strbeq	r3, [r2]
    bd34:	e28dd008 	add	sp, sp, #8	; 0x8
    bd38:	e8bd8070 	pop	{r4, r5, r6, pc}
    bd3c:	3c00d1e0 	.word	0x3c00d1e0

0000bd40 <iic_writeread>:
    bd40:	e92d4070 	push	{r4, r5, r6, lr}
    bd44:	e59f4088 	ldr	r4, [pc, #136]	; bdd4 <iic_writeread+0x94>
    bd48:	e24dd010 	sub	sp, sp, #16	; 0x10
    bd4c:	e5d43000 	ldrb	r3, [r4]
    bd50:	e5cd100f 	strb	r1, [sp, #15]
    bd54:	e243300a 	sub	r3, r3, #10	; 0xa
    bd58:	e6ef3073 	uxtb	r3, r3
    bd5c:	e3530001 	cmp	r3, #1	; 0x1
    bd60:	e1a06002 	mov	r6, r2
    bd64:	e1a05000 	mov	r5, r0
    bd68:	9a00000c 	bls	bda0 <iic_writeread+0x60>
    bd6c:	e3a0c000 	mov	ip, #0	; 0x0
    bd70:	e1a0300c 	mov	r3, ip
    bd74:	e28d100f 	add	r1, sp, #15	; 0xf
    bd78:	e3a02001 	mov	r2, #1	; 0x1
    bd7c:	e58dc000 	str	ip, [sp]
    bd80:	e58dc004 	str	ip, [sp, #4]
    bd84:	ebffff91 	bl	bbd0 <iicreg_write>
    bd88:	e3500000 	cmp	r0, #0	; 0x0
    bd8c:	03a0300c 	moveq	r3, #12	; 0xc
    bd90:	05c43000 	strbeq	r3, [r4]
    bd94:	0a00000c 	beq	bdcc <iic_writeread+0x8c>
    bd98:	e3a0300a 	mov	r3, #10	; 0xa
    bd9c:	e5c43000 	strb	r3, [r4]
    bda0:	e3a02001 	mov	r2, #1	; 0x1
    bda4:	e1a00005 	mov	r0, r5
    bda8:	e1a01006 	mov	r1, r6
    bdac:	ebffff68 	bl	bb54 <iicreg_read>
    bdb0:	e59f201c 	ldr	r2, [pc, #28]	; bdd4 <iic_writeread+0x94>
    bdb4:	e3500000 	cmp	r0, #0	; 0x0
    bdb8:	13a0300d 	movne	r3, #13	; 0xd
    bdbc:	03a0300b 	moveq	r3, #11	; 0xb
    bdc0:	15c23000 	strbne	r3, [r2]
    bdc4:	13a00001 	movne	r0, #1	; 0x1
    bdc8:	05c23000 	strbeq	r3, [r2]
    bdcc:	e28dd010 	add	sp, sp, #16	; 0x10
    bdd0:	e8bd8070 	pop	{r4, r5, r6, pc}
    bdd4:	3c00d1e0 	.word	0x3c00d1e0

0000bdd8 <iicreg_init>:
    bdd8:	e92d4030 	push	{r4, r5, lr}
    bddc:	e1a04000 	mov	r4, r0
    bde0:	e24dd004 	sub	sp, sp, #4	; 0x4
    bde4:	e59f0074 	ldr	r0, [pc, #116]	; be60 <iicreg_init+0x88>
    bde8:	e1a05002 	mov	r5, r2
    bdec:	ebffd529 	bl	1298 <module_get_clock>
    bdf0:	e3540001 	cmp	r4, #1	; 0x1
    bdf4:	83a00000 	movhi	r0, #0	; 0x0
    bdf8:	8a000016 	bhi	be58 <iicreg_init+0x80>
    bdfc:	e59f1060 	ldr	r1, [pc, #96]	; be64 <iicreg_init+0x8c>
    be00:	eb001945 	bl	1231c <__aeabi_uidiv>
    be04:	e59f205c 	ldr	r2, [pc, #92]	; be68 <iicreg_init+0x90>
    be08:	e1a010a0 	lsr	r1, r0, #1
    be0c:	e3a0c000 	mov	ip, #0	; 0x0
    be10:	e3a03043 	mov	r3, #67	; 0x43
    be14:	e582c06c 	str	ip, [r2, #108]
    be18:	e5823000 	str	r3, [r2]
    be1c:	e5820014 	str	r0, [r2, #20]
    be20:	e5820018 	str	r0, [r2, #24]
    be24:	e58210a0 	str	r1, [r2, #160]
    be28:	e59230a0 	ldr	r3, [r2, #160]
    be2c:	e1a000a5 	lsr	r0, r5, #1
    be30:	e3c33c01 	bic	r3, r3, #256	; 0x100
    be34:	e58230a0 	str	r3, [r2, #160]
    be38:	e5923000 	ldr	r3, [r2]
    be3c:	e3a01001 	mov	r1, #1	; 0x1
    be40:	e3833020 	orr	r3, r3, #32	; 0x20
    be44:	e5823000 	str	r3, [r2]
    be48:	e582c030 	str	ip, [r2, #48]
    be4c:	e5820004 	str	r0, [r2, #4]
    be50:	e582106c 	str	r1, [r2, #108]
    be54:	e1a00001 	mov	r0, r1
    be58:	e28dd004 	add	sp, sp, #4	; 0x4
    be5c:	e8bd8030 	pop	{r4, r5, pc}
    be60:	21e0a360 	.word	0x21e0a360
    be64:	0009c400 	.word	0x0009c400
    be68:	20300000 	.word	0x20300000

0000be6c <iic_init>:
    be6c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    be70:	e59fc054 	ldr	ip, [pc, #84]	; becc <iic_init+0x60>
    be74:	e24dd004 	sub	sp, sp, #4	; 0x4
    be78:	e3a0e00d 	mov	lr, #13	; 0xd
    be7c:	e1a0a000 	mov	sl, r0
    be80:	e59f0048 	ldr	r0, [pc, #72]	; bed0 <iic_init+0x64>
    be84:	e1a04001 	mov	r4, r1
    be88:	e1a05002 	mov	r5, r2
    be8c:	e1a06003 	mov	r6, r3
    be90:	e5cce000 	strb	lr, [ip]
    be94:	e5dd7020 	ldrb	r7, [sp, #32]
    be98:	e5dd8024 	ldrb	r8, [sp, #36]
    be9c:	ebffd244 	bl	7b4 <module_enable>
    bea0:	e3a0000a 	mov	r0, #10	; 0xa
    bea4:	ebffd27d 	bl	8a0 <pads_init_module>
    bea8:	e1a0000a 	mov	r0, sl
    beac:	e1a01004 	mov	r1, r4
    beb0:	e1a02005 	mov	r2, r5
    beb4:	e1a03006 	mov	r3, r6
    beb8:	e58d7020 	str	r7, [sp, #32]
    bebc:	e58d8024 	str	r8, [sp, #36]
    bec0:	e28dd004 	add	sp, sp, #4	; 0x4
    bec4:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    bec8:	eaffffc2 	b	bdd8 <iicreg_init>
    becc:	3c00d1e0 	.word	0x3c00d1e0
    bed0:	21e10c00 	.word	0x21e10c00

0000bed4 <spi_set_bytes>:
    bed4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    bed8:	e59f3020 	ldr	r3, [pc, #32]	; bf00 <spi_set_bytes+0x2c>
    bedc:	e1a01000 	mov	r1, r0
    bee0:	e5830000 	str	r0, [r3]
    bee4:	e24dd004 	sub	sp, sp, #4	; 0x4
    bee8:	e59f0014 	ldr	r0, [pc, #20]	; bf04 <spi_set_bytes+0x30>
    beec:	e59f3014 	ldr	r3, [pc, #20]	; bf08 <spi_set_bytes+0x34>
    bef0:	e1a0e00f 	mov	lr, pc
    bef4:	e593f000 	ldr	pc, [r3]
    bef8:	e28dd004 	add	sp, sp, #4	; 0x4
    befc:	e8bd8000 	pop	{pc}
    bf00:	3c00d0b8 	.word	0x3c00d0b8
    bf04:	0001bfac 	.word	0x0001bfac
    bf08:	3c00d694 	.word	0x3c00d694

0000bf0c <spi_fifo_clear>:
    bf0c:	e59f2028 	ldr	r2, [pc, #40]	; bf3c <spi_fifo_clear+0x30>
    bf10:	e592300c 	ldr	r3, [r2, #12]
    bf14:	e3130010 	tst	r3, #16	; 0x10
    bf18:	1afffffc 	bne	bf10 <spi_fifo_clear+0x4>
    bf1c:	ea000001 	b	bf28 <spi_fifo_clear+0x1c>
    bf20:	e5923008 	ldr	r3, [r2, #8]
    bf24:	ea000000 	b	bf2c <spi_fifo_clear+0x20>
    bf28:	e59f200c 	ldr	r2, [pc, #12]	; bf3c <spi_fifo_clear+0x30>
    bf2c:	e592300c 	ldr	r3, [r2, #12]
    bf30:	e3130004 	tst	r3, #4	; 0x4
    bf34:	1afffff9 	bne	bf20 <spi_fifo_clear+0x14>
    bf38:	e12fff1e 	bx	lr
    bf3c:	20500000 	.word	0x20500000

0000bf40 <flash_vs_align>:
    bf40:	e3a00c01 	mov	r0, #256	; 0x100
    bf44:	e12fff1e 	bx	lr

0000bf48 <spi_exchange>:
    bf48:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    bf4c:	e5905008 	ldr	r5, [r0, #8]
    bf50:	e3510001 	cmp	r1, #1	; 0x1
    bf54:	e24dd00c 	sub	sp, sp, #12	; 0xc
    bf58:	e1a0a000 	mov	sl, r0
    bf5c:	e5908004 	ldr	r8, [r0, #4]
    bf60:	d1a04005 	movle	r4, r5
    bf64:	d3a07000 	movle	r7, #0	; 0x0
    bf68:	da000020 	ble	bff0 <spi_exchange+0xa8>
    bf6c:	e280a00c 	add	sl, r0, #12	; 0xc
    bf70:	e1a01008 	mov	r1, r8
    bf74:	e1a0000d 	mov	r0, sp
    bf78:	e1a02005 	mov	r2, r5
    bf7c:	ebffd59b 	bl	15f0 <memcpy>
    bf80:	e99a0012 	ldmib	sl, {r1, r4}
    bf84:	e0843005 	add	r3, r4, r5
    bf88:	e3530008 	cmp	r3, #8	; 0x8
    bf8c:	a3a03008 	movge	r3, #8	; 0x8
    bf90:	e6ef6073 	uxtb	r6, r3
    bf94:	e3510000 	cmp	r1, #0	; 0x0
    bf98:	e1a0700d 	mov	r7, sp
    bf9c:	059a8000 	ldreq	r8, [sl]
    bfa0:	00667084 	rsbeq	r7, r6, r4, lsl #1
    bfa4:	0a000008 	beq	bfcc <spi_exchange+0x84>
    bfa8:	e0654006 	rsb	r4, r5, r6
    bfac:	e0870005 	add	r0, r7, r5
    bfb0:	e1a02004 	mov	r2, r4
    bfb4:	ebffd58d 	bl	15f0 <memcpy>
    bfb8:	e99a000c 	ldmib	sl, {r2, r3}
    bfbc:	e0855003 	add	r5, r5, r3
    bfc0:	e0828004 	add	r8, r2, r4
    bfc4:	e3a07000 	mov	r7, #0	; 0x0
    bfc8:	e0664005 	rsb	r4, r6, r5
    bfcc:	e3a02000 	mov	r2, #0	; 0x0
    bfd0:	e1a0000d 	mov	r0, sp
    bfd4:	e59f126c 	ldr	r1, [pc, #620]	; c248 <spi_exchange+0x300>
    bfd8:	ea000002 	b	bfe8 <spi_exchange+0xa0>
    bfdc:	e7d03002 	ldrb	r3, [r0, r2]
    bfe0:	e2822001 	add	r2, r2, #1	; 0x1
    bfe4:	e5813008 	str	r3, [r1, #8]
    bfe8:	e1520006 	cmp	r2, r6
    bfec:	bafffffa 	blt	bfdc <spi_exchange+0x94>
    bff0:	e59f6250 	ldr	r6, [pc, #592]	; c248 <spi_exchange+0x300>
    bff4:	e3a0b005 	mov	fp, #5	; 0x5
    bff8:	e5969018 	ldr	r9, [r6, #24]
    bffc:	e3190004 	tst	r9, #4	; 0x4
    c000:	0a000030 	beq	c0c8 <spi_exchange+0x180>
    c004:	e3550003 	cmp	r5, #3	; 0x3
    c008:	da000005 	ble	c024 <spi_exchange+0xdc>
    c00c:	e5963008 	ldr	r3, [r6, #8]
    c010:	e2455004 	sub	r5, r5, #4	; 0x4
    c014:	e5963008 	ldr	r3, [r6, #8]
    c018:	e5963008 	ldr	r3, [r6, #8]
    c01c:	e5963008 	ldr	r3, [r6, #8]
    c020:	ea000028 	b	c0c8 <spi_exchange+0x180>
    c024:	e3550000 	cmp	r5, #0	; 0x0
    c028:	0a000003 	beq	c03c <spi_exchange+0xf4>
    c02c:	e2555001 	subs	r5, r5, #1	; 0x1
    c030:	e5963008 	ldr	r3, [r6, #8]
    c034:	1afffffc 	bne	c02c <spi_exchange+0xe4>
    c038:	ea000022 	b	c0c8 <spi_exchange+0x180>
    c03c:	e59a3000 	ldr	r3, [sl]
    c040:	e3530000 	cmp	r3, #0	; 0x0
    c044:	1a000004 	bne	c05c <spi_exchange+0x114>
    c048:	e59f01fc 	ldr	r0, [pc, #508]	; c24c <spi_exchange+0x304>
    c04c:	e59f31fc 	ldr	r3, [pc, #508]	; c250 <spi_exchange+0x308>
    c050:	e1a0e00f 	mov	lr, pc
    c054:	e593f000 	ldr	pc, [r3]
    c058:	ea00001a 	b	c0c8 <spi_exchange+0x180>
    c05c:	e3540003 	cmp	r4, #3	; 0x3
    c060:	da00000e 	ble	c0a0 <spi_exchange+0x158>
    c064:	e5962008 	ldr	r2, [r6, #8]
    c068:	e1a03008 	mov	r3, r8
    c06c:	e4c32001 	strb	r2, [r3], #1
    c070:	e5962008 	ldr	r2, [r6, #8]
    c074:	e2831001 	add	r1, r3, #1	; 0x1
    c078:	e5c82001 	strb	r2, [r8, #1]
    c07c:	e5962008 	ldr	r2, [r6, #8]
    c080:	e2544004 	subs	r4, r4, #4	; 0x4
    c084:	e5c32001 	strb	r2, [r3, #1]
    c088:	e5963008 	ldr	r3, [r6, #8]
    c08c:	e2812001 	add	r2, r1, #1	; 0x1
    c090:	e5c13001 	strb	r3, [r1, #1]
    c094:	0a00005e 	beq	c214 <spi_exchange+0x2cc>
    c098:	e2828001 	add	r8, r2, #1	; 0x1
    c09c:	ea000009 	b	c0c8 <spi_exchange+0x180>
    c0a0:	e3540000 	cmp	r4, #0	; 0x0
    c0a4:	0a00005a 	beq	c214 <spi_exchange+0x2cc>
    c0a8:	e59f2198 	ldr	r2, [pc, #408]	; c248 <spi_exchange+0x300>
    c0ac:	e1a01005 	mov	r1, r5
    c0b0:	e5923008 	ldr	r3, [r2, #8]
    c0b4:	e7c83001 	strb	r3, [r8, r1]
    c0b8:	e2811001 	add	r1, r1, #1	; 0x1
    c0bc:	e1510004 	cmp	r1, r4
    c0c0:	1afffffa 	bne	c0b0 <spi_exchange+0x168>
    c0c4:	ea000052 	b	c214 <spi_exchange+0x2cc>
    c0c8:	e3190008 	tst	r9, #8	; 0x8
    c0cc:	0a00002a 	beq	c17c <spi_exchange+0x234>
    c0d0:	e59a3004 	ldr	r3, [sl, #4]
    c0d4:	e3530000 	cmp	r3, #0	; 0x0
    c0d8:	0a000019 	beq	c144 <spi_exchange+0x1fc>
    c0dc:	e3540003 	cmp	r4, #3	; 0x3
    c0e0:	da00000d 	ble	c11c <spi_exchange+0x1d4>
    c0e4:	e1a03008 	mov	r3, r8
    c0e8:	e4d32001 	ldrb	r2, [r3], #1
    c0ec:	e2544004 	subs	r4, r4, #4	; 0x4
    c0f0:	e5862008 	str	r2, [r6, #8]
    c0f4:	e5d82001 	ldrb	r2, [r8, #1]
    c0f8:	e2831001 	add	r1, r3, #1	; 0x1
    c0fc:	e5862008 	str	r2, [r6, #8]
    c100:	e5d33001 	ldrb	r3, [r3, #1]
    c104:	e2818002 	add	r8, r1, #2	; 0x2
    c108:	e5863008 	str	r3, [r6, #8]
    c10c:	e5d13001 	ldrb	r3, [r1, #1]
    c110:	e5863008 	str	r3, [r6, #8]
    c114:	1a000018 	bne	c17c <spi_exchange+0x234>
    c118:	ea00003d 	b	c214 <spi_exchange+0x2cc>
    c11c:	e3540000 	cmp	r4, #0	; 0x0
    c120:	0a00003b 	beq	c214 <spi_exchange+0x2cc>
    c124:	e59f111c 	ldr	r1, [pc, #284]	; c248 <spi_exchange+0x300>
    c128:	e3a02000 	mov	r2, #0	; 0x0
    c12c:	e7d83002 	ldrb	r3, [r8, r2]
    c130:	e2822001 	add	r2, r2, #1	; 0x1
    c134:	e1520004 	cmp	r2, r4
    c138:	e5813008 	str	r3, [r1, #8]
    c13c:	1afffffa 	bne	c12c <spi_exchange+0x1e4>
    c140:	ea000033 	b	c214 <spi_exchange+0x2cc>
    c144:	e3570003 	cmp	r7, #3	; 0x3
    c148:	da000005 	ble	c164 <spi_exchange+0x21c>
    c14c:	e586b008 	str	fp, [r6, #8]
    c150:	e2477004 	sub	r7, r7, #4	; 0x4
    c154:	e586b008 	str	fp, [r6, #8]
    c158:	e586b008 	str	fp, [r6, #8]
    c15c:	e586b008 	str	fp, [r6, #8]
    c160:	ea000005 	b	c17c <spi_exchange+0x234>
    c164:	e3570000 	cmp	r7, #0	; 0x0
    c168:	0a000003 	beq	c17c <spi_exchange+0x234>
    c16c:	e3a03005 	mov	r3, #5	; 0x5
    c170:	e2577001 	subs	r7, r7, #1	; 0x1
    c174:	e5863008 	str	r3, [r6, #8]
    c178:	1afffffb 	bne	c16c <spi_exchange+0x224>
    c17c:	e3190002 	tst	r9, #2	; 0x2
    c180:	0affff9c 	beq	bff8 <spi_exchange+0xb0>
    c184:	ea000009 	b	c1b0 <spi_exchange+0x268>
    c188:	e596300c 	ldr	r3, [r6, #12]
    c18c:	e2455001 	sub	r5, r5, #1	; 0x1
    c190:	e3130004 	tst	r3, #4	; 0x4
    c194:	15963008 	ldrne	r3, [r6, #8]
    c198:	1a000006 	bne	c1b8 <spi_exchange+0x270>
    c19c:	e59f00b0 	ldr	r0, [pc, #176]	; c254 <spi_exchange+0x30c>
    c1a0:	e1a01005 	mov	r1, r5
    c1a4:	e1a0e00f 	mov	lr, pc
    c1a8:	e597f000 	ldr	pc, [r7]
    c1ac:	ea000001 	b	c1b8 <spi_exchange+0x270>
    c1b0:	e59f6090 	ldr	r6, [pc, #144]	; c248 <spi_exchange+0x300>
    c1b4:	e59f7094 	ldr	r7, [pc, #148]	; c250 <spi_exchange+0x308>
    c1b8:	e3550000 	cmp	r5, #0	; 0x0
    c1bc:	1afffff1 	bne	c188 <spi_exchange+0x240>
    c1c0:	ea00000a 	b	c1f0 <spi_exchange+0x2a8>
    c1c4:	e595300c 	ldr	r3, [r5, #12]
    c1c8:	e2444001 	sub	r4, r4, #1	; 0x1
    c1cc:	e3130004 	tst	r3, #4	; 0x4
    c1d0:	15953008 	ldrne	r3, [r5, #8]
    c1d4:	14c83001 	strbne	r3, [r8], #1
    c1d8:	1a000006 	bne	c1f8 <spi_exchange+0x2b0>
    c1dc:	e59f0074 	ldr	r0, [pc, #116]	; c258 <spi_exchange+0x310>
    c1e0:	e1a01004 	mov	r1, r4
    c1e4:	e1a0e00f 	mov	lr, pc
    c1e8:	e596f000 	ldr	pc, [r6]
    c1ec:	ea000001 	b	c1f8 <spi_exchange+0x2b0>
    c1f0:	e59f5050 	ldr	r5, [pc, #80]	; c248 <spi_exchange+0x300>
    c1f4:	e59f6054 	ldr	r6, [pc, #84]	; c250 <spi_exchange+0x308>
    c1f8:	e59a3000 	ldr	r3, [sl]
    c1fc:	e3530000 	cmp	r3, #0	; 0x0
    c200:	13540000 	cmpne	r4, #0	; 0x0
    c204:	1affffee 	bne	c1c4 <spi_exchange+0x27c>
    c208:	e59f3038 	ldr	r3, [pc, #56]	; c248 <spi_exchange+0x300>
    c20c:	e3a02003 	mov	r2, #3	; 0x3
    c210:	e5832020 	str	r2, [r3, #32]
    c214:	e59f202c 	ldr	r2, [pc, #44]	; c248 <spi_exchange+0x300>
    c218:	e592300c 	ldr	r3, [r2, #12]
    c21c:	e3130010 	tst	r3, #16	; 0x10
    c220:	1afffffc 	bne	c218 <spi_exchange+0x2d0>
    c224:	ea000001 	b	c230 <spi_exchange+0x2e8>
    c228:	e5923008 	ldr	r3, [r2, #8]
    c22c:	ea000000 	b	c234 <spi_exchange+0x2ec>
    c230:	e59f2010 	ldr	r2, [pc, #16]	; c248 <spi_exchange+0x300>
    c234:	e592300c 	ldr	r3, [r2, #12]
    c238:	e3130004 	tst	r3, #4	; 0x4
    c23c:	1afffff9 	bne	c228 <spi_exchange+0x2e0>
    c240:	e28dd00c 	add	sp, sp, #12	; 0xc
    c244:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    c248:	20500000 	.word	0x20500000
    c24c:	0001bfc3 	.word	0x0001bfc3
    c250:	3c00d694 	.word	0x3c00d694
    c254:	0001bfda 	.word	0x0001bfda
    c258:	0001bff2 	.word	0x0001bff2

0000c25c <flash_vs_read>:
    c25c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    c260:	e24dd024 	sub	sp, sp, #36	; 0x24
    c264:	e28d4018 	add	r4, sp, #24	; 0x18
    c268:	e2841003 	add	r1, r4, #3	; 0x3
    c26c:	e59da040 	ldr	sl, [sp, #64]
    c270:	e58d1004 	str	r1, [sp, #4]
    c274:	e3a01005 	mov	r1, #5	; 0x5
    c278:	e3a0c000 	mov	ip, #0	; 0x0
    c27c:	e58d1008 	str	r1, [sp, #8]
    c280:	e2811006 	add	r1, r1, #6	; 0x6
    c284:	e58dc018 	str	ip, [sp, #24]
    c288:	e2845004 	add	r5, r4, #4	; 0x4
    c28c:	e58d000c 	str	r0, [sp, #12]
    c290:	e58dc010 	str	ip, [sp, #16]
    c294:	e5cd101b 	strb	r1, [sp, #27]
    c298:	e1a06002 	mov	r6, r2
    c29c:	e1a07003 	mov	r7, r3
    c2a0:	e58dc000 	str	ip, [sp]
    c2a4:	e1a0400a 	mov	r4, sl
    c2a8:	e1a0800d 	mov	r8, sp
    c2ac:	ea00000c 	b	c2e4 <flash_vs_read+0x88>
    c2b0:	e3540a01 	cmp	r4, #4096	; 0x1000
    c2b4:	b1a03004 	movlt	r3, r4
    c2b8:	a3a03a01 	movge	r3, #4096	; 0x1000
    c2bc:	e5852000 	str	r2, [r5]
    c2c0:	e58d3014 	str	r3, [sp, #20]
    c2c4:	ebffff1f 	bl	bf48 <spi_exchange>
    c2c8:	e59d2014 	ldr	r2, [sp, #20]
    c2cc:	e59d300c 	ldr	r3, [sp, #12]
    c2d0:	e0966002 	adds	r6, r6, r2
    c2d4:	e0833002 	add	r3, r3, r2
    c2d8:	e0a77fc2 	adc	r7, r7, r2, asr #31
    c2dc:	e58d300c 	str	r3, [sp, #12]
    c2e0:	e0624004 	rsb	r4, r2, r4
    c2e4:	e1a02c06 	lsl	r2, r6, #24
    c2e8:	e2061cff 	and	r1, r6, #65280	; 0xff00
    c2ec:	e1822c26 	orr	r2, r2, r6, lsr #24
    c2f0:	e1822401 	orr	r2, r2, r1, lsl #8
    c2f4:	e20638ff 	and	r3, r6, #16711680	; 0xff0000
    c2f8:	e1822423 	orr	r2, r2, r3, lsr #8
    c2fc:	e3540000 	cmp	r4, #0	; 0x0
    c300:	e1a02442 	asr	r2, r2, #8
    c304:	e1a0000d 	mov	r0, sp
    c308:	e3a01002 	mov	r1, #2	; 0x2
    c30c:	1affffe7 	bne	c2b0 <flash_vs_read+0x54>
    c310:	e1a0000a 	mov	r0, sl
    c314:	e28dd024 	add	sp, sp, #36	; 0x24
    c318:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

0000c31c <flash_poll>:
    c31c:	e92d4030 	push	{r4, r5, lr}
    c320:	e24dd024 	sub	sp, sp, #36	; 0x24
    c324:	e28dc018 	add	ip, sp, #24	; 0x18
    c328:	e28c3001 	add	r3, ip, #1	; 0x1
    c32c:	e3a02000 	mov	r2, #0	; 0x0
    c330:	e3a01001 	mov	r1, #1	; 0x1
    c334:	e58d300c 	str	r3, [sp, #12]
    c338:	e3a03005 	mov	r3, #5	; 0x5
    c33c:	e58d2018 	str	r2, [sp, #24]
    c340:	e58d2010 	str	r2, [sp, #16]
    c344:	e58d1014 	str	r1, [sp, #20]
    c348:	e5cd3018 	strb	r3, [sp, #24]
    c34c:	e1a04000 	mov	r4, r0
    c350:	e58d201c 	str	r2, [sp, #28]
    c354:	e88d1004 	stm	sp, {r2, ip}
    c358:	e58d1008 	str	r1, [sp, #8]
    c35c:	e1a0500d 	mov	r5, sp
    c360:	e1a0000d 	mov	r0, sp
    c364:	e3a01002 	mov	r1, #2	; 0x2
    c368:	ebfffef6 	bl	bf48 <spi_exchange>
    c36c:	e5dd3019 	ldrb	r3, [sp, #25]
    c370:	e3540000 	cmp	r4, #0	; 0x0
    c374:	11a030a3 	lsrne	r3, r3, #1
    c378:	12233001 	eorne	r3, r3, #1	; 0x1
    c37c:	e2033001 	and	r3, r3, #1	; 0x1
    c380:	e3530000 	cmp	r3, #0	; 0x0
    c384:	1afffff5 	bne	c360 <flash_poll+0x44>
    c388:	e28dd024 	add	sp, sp, #36	; 0x24
    c38c:	e8bd8030 	pop	{r4, r5, pc}

0000c390 <flash_wren>:
    c390:	e92d4010 	push	{r4, lr}
    c394:	e24dd020 	sub	sp, sp, #32	; 0x20
    c398:	e3a04001 	mov	r4, #1	; 0x1
    c39c:	e28dc018 	add	ip, sp, #24	; 0x18
    c3a0:	e08c3004 	add	r3, ip, r4
    c3a4:	e3a02000 	mov	r2, #0	; 0x0
    c3a8:	e58d3010 	str	r3, [sp, #16]
    c3ac:	e3a03006 	mov	r3, #6	; 0x6
    c3b0:	e58d2018 	str	r2, [sp, #24]
    c3b4:	e1a0000d 	mov	r0, sp
    c3b8:	e5cd3018 	strb	r3, [sp, #24]
    c3bc:	e1a01004 	mov	r1, r4
    c3c0:	e3a03004 	mov	r3, #4	; 0x4
    c3c4:	e58d200c 	str	r2, [sp, #12]
    c3c8:	e5cd3019 	strb	r3, [sp, #25]
    c3cc:	e58d201c 	str	r2, [sp, #28]
    c3d0:	e88d1004 	stm	sp, {r2, ip}
    c3d4:	e58d4008 	str	r4, [sp, #8]
    c3d8:	e58d4014 	str	r4, [sp, #20]
    c3dc:	ebfffed9 	bl	bf48 <spi_exchange>
    c3e0:	e1a00004 	mov	r0, r4
    c3e4:	ebffffcc 	bl	c31c <flash_poll>
    c3e8:	e28dd020 	add	sp, sp, #32	; 0x20
    c3ec:	e8bd8010 	pop	{r4, pc}

0000c3f0 <flash_vs_erase>:
    c3f0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    c3f4:	e24dd034 	sub	sp, sp, #52	; 0x34
    c3f8:	e28d4028 	add	r4, sp, #40	; 0x28
    c3fc:	e3a06001 	mov	r6, #1	; 0x1
    c400:	e59f109c 	ldr	r1, [pc, #156]	; c4a4 <flash_vs_erase+0xb4>
    c404:	e3a02008 	mov	r2, #8	; 0x8
    c408:	e1a00004 	mov	r0, r4
    c40c:	ebffd477 	bl	15f0 <memcpy>
    c410:	e0843006 	add	r3, r4, r6
    c414:	e2842003 	add	r2, r4, #3	; 0x3
    c418:	e3a05000 	mov	r5, #0	; 0x0
    c41c:	e28d7004 	add	r7, sp, #4	; 0x4
    c420:	e58d3008 	str	r3, [sp, #8]
    c424:	e3a03002 	mov	r3, #2	; 0x2
    c428:	e58d300c 	str	r3, [sp, #12]
    c42c:	e58d2014 	str	r2, [sp, #20]
    c430:	e58d4020 	str	r4, [sp, #32]
    c434:	e58d5004 	str	r5, [sp, #4]
    c438:	e58d5010 	str	r5, [sp, #16]
    c43c:	e58d6018 	str	r6, [sp, #24]
    c440:	e58d501c 	str	r5, [sp, #28]
    c444:	e58d6024 	str	r6, [sp, #36]
    c448:	ebffffd0 	bl	c390 <flash_wren>
    c44c:	e1a01006 	mov	r1, r6
    c450:	e1a00007 	mov	r0, r7
    c454:	ebfffebb 	bl	bf48 <spi_exchange>
    c458:	e1a00005 	mov	r0, r5
    c45c:	ebffffae 	bl	c31c <flash_poll>
    c460:	e2870018 	add	r0, r7, #24	; 0x18
    c464:	e1a01006 	mov	r1, r6
    c468:	ebfffeb6 	bl	bf48 <spi_exchange>
    c46c:	e1a01006 	mov	r1, r6
    c470:	e1a00007 	mov	r0, r7
    c474:	ebfffeb3 	bl	bf48 <spi_exchange>
    c478:	e1a00005 	mov	r0, r5
    c47c:	ebffffa6 	bl	c31c <flash_poll>
    c480:	ebffffc2 	bl	c390 <flash_wren>
    c484:	e1a01006 	mov	r1, r6
    c488:	e287000c 	add	r0, r7, #12	; 0xc
    c48c:	ebfffead 	bl	bf48 <spi_exchange>
    c490:	e1a00005 	mov	r0, r5
    c494:	ebffffa0 	bl	c31c <flash_poll>
    c498:	e1a00005 	mov	r0, r5
    c49c:	e28dd034 	add	sp, sp, #52	; 0x34
    c4a0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    c4a4:	00019c02 	.word	0x00019c02

0000c4a8 <flash_vs_write>:
    c4a8:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    c4ac:	e3a01004 	mov	r1, #4	; 0x4
    c4b0:	e24dd020 	sub	sp, sp, #32	; 0x20
    c4b4:	e3a0c000 	mov	ip, #0	; 0x0
    c4b8:	e59d9040 	ldr	r9, [sp, #64]
    c4bc:	e58d1008 	str	r1, [sp, #8]
    c4c0:	e2811002 	add	r1, r1, #2	; 0x2
    c4c4:	e58dc018 	str	ip, [sp, #24]
    c4c8:	e28d701c 	add	r7, sp, #28	; 0x1c
    c4cc:	e5cd1018 	strb	r1, [sp, #24]
    c4d0:	e3a01002 	mov	r1, #2	; 0x2
    c4d4:	e58dc00c 	str	ip, [sp, #12]
    c4d8:	e58d0010 	str	r0, [sp, #16]
    c4dc:	e5cd101b 	strb	r1, [sp, #27]
    c4e0:	e1a04002 	mov	r4, r2
    c4e4:	e1a05003 	mov	r5, r3
    c4e8:	e58dc000 	str	ip, [sp]
    c4ec:	e58d7004 	str	r7, [sp, #4]
    c4f0:	e1a06009 	mov	r6, r9
    c4f4:	e59fa080 	ldr	sl, [pc, #128]	; c57c <flash_vs_write+0xd4>
    c4f8:	e1a0800d 	mov	r8, sp
    c4fc:	ea000011 	b	c548 <flash_vs_write+0xa0>
    c500:	e59a3000 	ldr	r3, [sl]
    c504:	e5872000 	str	r2, [r7]
    c508:	e1560003 	cmp	r6, r3
    c50c:	d58d6014 	strle	r6, [sp, #20]
    c510:	c58d3014 	strgt	r3, [sp, #20]
    c514:	ebffff9d 	bl	c390 <flash_wren>
    c518:	e1a0000d 	mov	r0, sp
    c51c:	e3a01002 	mov	r1, #2	; 0x2
    c520:	ebfffe88 	bl	bf48 <spi_exchange>
    c524:	e3a00000 	mov	r0, #0	; 0x0
    c528:	ebffff7b 	bl	c31c <flash_poll>
    c52c:	e59d2014 	ldr	r2, [sp, #20]
    c530:	e59d3010 	ldr	r3, [sp, #16]
    c534:	e0944002 	adds	r4, r4, r2
    c538:	e0833002 	add	r3, r3, r2
    c53c:	e0a55fc2 	adc	r5, r5, r2, asr #31
    c540:	e58d3010 	str	r3, [sp, #16]
    c544:	e0626006 	rsb	r6, r2, r6
    c548:	e1a02c04 	lsl	r2, r4, #24
    c54c:	e1822c24 	orr	r2, r2, r4, lsr #24
    c550:	e2041cff 	and	r1, r4, #65280	; 0xff00
    c554:	e1822401 	orr	r2, r2, r1, lsl #8
    c558:	e20438ff 	and	r3, r4, #16711680	; 0xff0000
    c55c:	e1822423 	orr	r2, r2, r3, lsr #8
    c560:	e3c220ff 	bic	r2, r2, #255	; 0xff
    c564:	e3560000 	cmp	r6, #0	; 0x0
    c568:	e3822002 	orr	r2, r2, #2	; 0x2
    c56c:	1affffe3 	bne	c500 <flash_vs_write+0x58>
    c570:	e1a00009 	mov	r0, r9
    c574:	e28dd020 	add	sp, sp, #32	; 0x20
    c578:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    c57c:	3c00d0b8 	.word	0x3c00d0b8

0000c580 <spi_init_pads>:
    c580:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    c584:	e3a0000b 	mov	r0, #11	; 0xb
    c588:	e24dd004 	sub	sp, sp, #4	; 0x4
    c58c:	ebffd0c3 	bl	8a0 <pads_init_module>
    c590:	e3a00000 	mov	r0, #0	; 0x0
    c594:	e28dd004 	add	sp, sp, #4	; 0x4
    c598:	e8bd8000 	pop	{pc}

0000c59c <spi_init_clk>:
    c59c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    c5a0:	e59f00e8 	ldr	r0, [pc, #232]	; c690 <spi_init_clk+0xf4>
    c5a4:	e24dd00c 	sub	sp, sp, #12	; 0xc
    c5a8:	ebffd33a 	bl	1298 <module_get_clock>
    c5ac:	e3a05002 	mov	r5, #2	; 0x2
    c5b0:	e59fa0dc 	ldr	sl, [pc, #220]	; c694 <spi_init_clk+0xf8>
    c5b4:	e1a08000 	mov	r8, r0
    c5b8:	ea00000c 	b	c5f0 <spi_init_clk+0x54>
    c5bc:	e1a06004 	mov	r6, r4
    c5c0:	e1a01007 	mov	r1, r7
    c5c4:	e1a00008 	mov	r0, r8
    c5c8:	eb0017d5 	bl	12524 <__aeabi_idiv>
    c5cc:	e150000a 	cmp	r0, sl
    c5d0:	e0877005 	add	r7, r7, r5
    c5d4:	e2864001 	add	r4, r6, #1	; 0x1
    c5d8:	da000007 	ble	c5fc <spi_init_clk+0x60>
    c5dc:	e3540c01 	cmp	r4, #256	; 0x100
    c5e0:	1afffff5 	bne	c5bc <spi_init_clk+0x20>
    c5e4:	e2855002 	add	r5, r5, #2	; 0x2
    c5e8:	e3550c01 	cmp	r5, #256	; 0x100
    c5ec:	0a000003 	beq	c600 <spi_init_clk+0x64>
    c5f0:	e1a07005 	mov	r7, r5
    c5f4:	e3a06000 	mov	r6, #0	; 0x0
    c5f8:	eafffff0 	b	c5c0 <spi_init_clk+0x24>
    c5fc:	e1a04006 	mov	r4, r6
    c600:	e35400ff 	cmp	r4, #255	; 0xff
    c604:	d35500fe 	cmple	r5, #254	; 0xfe
    c608:	da000007 	ble	c62c <spi_init_clk+0x90>
    c60c:	e1a01005 	mov	r1, r5
    c610:	e1a02004 	mov	r2, r4
    c614:	e59f007c 	ldr	r0, [pc, #124]	; c698 <spi_init_clk+0xfc>
    c618:	e59f307c 	ldr	r3, [pc, #124]	; c69c <spi_init_clk+0x100>
    c61c:	e1a0e00f 	mov	lr, pc
    c620:	e593f000 	ldr	pc, [r3]
    c624:	e3a05002 	mov	r5, #2	; 0x2
    c628:	e3a04027 	mov	r4, #39	; 0x27
    c62c:	e2841001 	add	r1, r4, #1	; 0x1
    c630:	e0010195 	mul	r1, r5, r1
    c634:	e1a00008 	mov	r0, r8
    c638:	eb0017b9 	bl	12524 <__aeabi_idiv>
    c63c:	e1a01008 	mov	r1, r8
    c640:	e58d0000 	str	r0, [sp]
    c644:	e1a02005 	mov	r2, r5
    c648:	e1a03004 	mov	r3, r4
    c64c:	e59f004c 	ldr	r0, [pc, #76]	; c6a0 <spi_init_clk+0x104>
    c650:	e59fc044 	ldr	ip, [pc, #68]	; c69c <spi_init_clk+0x100>
    c654:	e1a0e00f 	mov	lr, pc
    c658:	e59cf000 	ldr	pc, [ip]
    c65c:	e59f2040 	ldr	r2, [pc, #64]	; c6a4 <spi_init_clk+0x108>
    c660:	e3a03000 	mov	r3, #0	; 0x0
    c664:	e1a01404 	lsl	r1, r4, #8
    c668:	e5823004 	str	r3, [r2, #4]
    c66c:	e38110c7 	orr	r1, r1, #199	; 0xc7
    c670:	e5823014 	str	r3, [r2, #20]
    c674:	e1a00003 	mov	r0, r3
    c678:	e283300a 	add	r3, r3, #10	; 0xa
    c67c:	e5825010 	str	r5, [r2, #16]
    c680:	e5821000 	str	r1, [r2]
    c684:	e5823004 	str	r3, [r2, #4]
    c688:	e28dd00c 	add	sp, sp, #12	; 0xc
    c68c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    c690:	21e0a360 	.word	0x21e0a360
    c694:	00551928 	.word	0x00551928
    c698:	0001c00a 	.word	0x0001c00a
    c69c:	3c00d694 	.word	0x3c00d694
    c6a0:	0001c038 	.word	0x0001c038
    c6a4:	20500000 	.word	0x20500000

0000c6a8 <flash_vs_reset>:
    c6a8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    c6ac:	e59f0044 	ldr	r0, [pc, #68]	; c6f8 <flash_vs_reset+0x50>
    c6b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    c6b4:	ebffd03e 	bl	7b4 <module_enable>
    c6b8:	ebffffb7 	bl	c59c <spi_init_clk>
    c6bc:	e3a0000b 	mov	r0, #11	; 0xb
    c6c0:	ebffd076 	bl	8a0 <pads_init_module>
    c6c4:	e59f2030 	ldr	r2, [pc, #48]	; c6fc <flash_vs_reset+0x54>
    c6c8:	e592300c 	ldr	r3, [r2, #12]
    c6cc:	e3130010 	tst	r3, #16	; 0x10
    c6d0:	1afffffc 	bne	c6c8 <flash_vs_reset+0x20>
    c6d4:	ea000001 	b	c6e0 <flash_vs_reset+0x38>
    c6d8:	e5923008 	ldr	r3, [r2, #8]
    c6dc:	ea000000 	b	c6e4 <flash_vs_reset+0x3c>
    c6e0:	e59f2014 	ldr	r2, [pc, #20]	; c6fc <flash_vs_reset+0x54>
    c6e4:	e592300c 	ldr	r3, [r2, #12]
    c6e8:	e2130004 	ands	r0, r3, #4	; 0x4
    c6ec:	1afffff9 	bne	c6d8 <flash_vs_reset+0x30>
    c6f0:	e28dd004 	add	sp, sp, #4	; 0x4
    c6f4:	e8bd8000 	pop	{pc}
    c6f8:	21e11400 	.word	0x21e11400
    c6fc:	20500000 	.word	0x20500000

0000c700 <ram_addr2offs>:
    c700:	e3500000 	cmp	r0, #0	; 0x0
    c704:	e3a01000 	mov	r1, #0	; 0x0
    c708:	a3a02102 	movge	r2, #-2147483648	; 0x80000000
    c70c:	b3a02102 	movlt	r2, #-2147483648	; 0x80000000
    c710:	a3a03000 	movge	r3, #0	; 0x0
    c714:	b3e03000 	mvnlt	r3, #0	; 0x0
    c718:	e0922000 	adds	r2, r2, r0
    c71c:	e0a33001 	adc	r3, r3, r1
    c720:	e1a01003 	mov	r1, r3
    c724:	e1a00002 	mov	r0, r2
    c728:	e12fff1e 	bx	lr

0000c72c <ram_offs2addr>:
    c72c:	e2800102 	add	r0, r0, #-2147483648	; 0x80000000
    c730:	e12fff1e 	bx	lr

0000c734 <ram_vs_align>:
    c734:	e3a00c02 	mov	r0, #512	; 0x200
    c738:	e12fff1e 	bx	lr

0000c73c <ram_vs_reset>:
    c73c:	e3a00000 	mov	r0, #0	; 0x0
    c740:	e12fff1e 	bx	lr

0000c744 <ram_vs_write>:
    c744:	e92d4010 	push	{r4, lr}
    c748:	e59d4008 	ldr	r4, [sp, #8]
    c74c:	e1a01000 	mov	r1, r0
    c750:	e2820102 	add	r0, r2, #-2147483648	; 0x80000000
    c754:	e1a02004 	mov	r2, r4
    c758:	ebffd3a4 	bl	15f0 <memcpy>
    c75c:	e1a00004 	mov	r0, r4
    c760:	e8bd8010 	pop	{r4, pc}

0000c764 <ram_vs_read>:
    c764:	e92d4010 	push	{r4, lr}
    c768:	e59d4008 	ldr	r4, [sp, #8]
    c76c:	e2821102 	add	r1, r2, #-2147483648	; 0x80000000
    c770:	e1a02004 	mov	r2, r4
    c774:	ebffd39d 	bl	15f0 <memcpy>
    c778:	e1a00004 	mov	r0, r4
    c77c:	e8bd8010 	pop	{r4, pc}

0000c780 <CreateAtaCFIS>:
    c780:	e3a0c000 	mov	ip, #0	; 0x0
    c784:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    c788:	e1a05003 	mov	r5, r3
    c78c:	e1a04000 	mov	r4, r0
    c790:	e1a06001 	mov	r6, r1
    c794:	e1a08002 	mov	r8, r2
    c798:	e1a0300c 	mov	r3, ip
    c79c:	e7c4300c 	strb	r3, [r4, ip]
    c7a0:	e28cc001 	add	ip, ip, #1	; 0x1
    c7a4:	e35c0014 	cmp	ip, #20	; 0x14
    c7a8:	e3a07000 	mov	r7, #0	; 0x0
    c7ac:	1afffffa 	bne	c79c <CreateAtaCFIS+0x1c>
    c7b0:	e5d43001 	ldrb	r3, [r4, #1]
    c7b4:	e59d2018 	ldr	r2, [sp, #24]
    c7b8:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    c7bc:	e1520007 	cmp	r2, r7
    c7c0:	e6ef3073 	uxtb	r3, r3
    c7c4:	03a02080 	moveq	r2, #128	; 0x80
    c7c8:	11a02007 	movne	r2, r7
    c7cc:	e3c33080 	bic	r3, r3, #128	; 0x80
    c7d0:	e35600ca 	cmp	r6, #202	; 0xca
    c7d4:	135600c8 	cmpne	r6, #200	; 0xc8
    c7d8:	e1822003 	orr	r2, r2, r3
    c7dc:	e3a03027 	mov	r3, #39	; 0x27
    c7e0:	e5c43000 	strb	r3, [r4]
    c7e4:	01a03c25 	lsreq	r3, r5, #24
    c7e8:	e1a0c428 	lsr	ip, r8, #8
    c7ec:	e1a01425 	lsr	r1, r5, #8
    c7f0:	e1a00825 	lsr	r0, r5, #16
    c7f4:	0203300f 	andeq	r3, r3, #15	; 0xf
    c7f8:	e5c47007 	strb	r7, [r4, #7]
    c7fc:	e5c42001 	strb	r2, [r4, #1]
    c800:	e5c4700b 	strb	r7, [r4, #11]
    c804:	e5c4c00d 	strb	ip, [r4, #13]
    c808:	e5c41005 	strb	r1, [r4, #5]
    c80c:	e5c40006 	strb	r0, [r4, #6]
    c810:	e5c46002 	strb	r6, [r4, #2]
    c814:	e5c47003 	strb	r7, [r4, #3]
    c818:	e5c4800c 	strb	r8, [r4, #12]
    c81c:	e5c45004 	strb	r5, [r4, #4]
    c820:	05c43007 	strbeq	r3, [r4, #7]
    c824:	0a000003 	beq	c838 <CreateAtaCFIS+0xb8>
    c828:	e3560035 	cmp	r6, #53	; 0x35
    c82c:	13560025 	cmpne	r6, #37	; 0x25
    c830:	01a03c25 	lsreq	r3, r5, #24
    c834:	05c43008 	strbeq	r3, [r4, #8]
    c838:	e59d201c 	ldr	r2, [sp, #28]
    c83c:	e5d43007 	ldrb	r3, [r4, #7]
    c840:	e3520000 	cmp	r2, #0	; 0x0
    c844:	e3a01000 	mov	r1, #0	; 0x0
    c848:	13a02004 	movne	r2, #4	; 0x4
    c84c:	03a02000 	moveq	r2, #0	; 0x0
    c850:	e3833040 	orr	r3, r3, #64	; 0x40
    c854:	e35600ec 	cmp	r6, #236	; 0xec
    c858:	e5c43007 	strb	r3, [r4, #7]
    c85c:	e5c4200f 	strb	r2, [r4, #15]
    c860:	e5c41009 	strb	r1, [r4, #9]
    c864:	e5c4100a 	strb	r1, [r4, #10]
    c868:	e5c4100e 	strb	r1, [r4, #14]
    c86c:	e5841010 	str	r1, [r4, #16]
    c870:	05c41007 	strbeq	r1, [r4, #7]
    c874:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000c878 <CreateCommandHeader>:
    c878:	e92d4070 	push	{r4, r5, r6, lr}
    c87c:	e59d6018 	ldr	r6, [sp, #24]
    c880:	e1a04000 	mov	r4, r0
    c884:	e1a00001 	mov	r0, r1
    c888:	e3a01000 	mov	r1, #0	; 0x0
    c88c:	e1a05003 	mov	r5, r3
    c890:	e084c280 	add	ip, r4, r0, lsl #5
    c894:	e1a03001 	mov	r3, r1
    c898:	e7c1300c 	strb	r3, [r1, ip]
    c89c:	e2811001 	add	r1, r1, #1	; 0x1
    c8a0:	e3510020 	cmp	r1, #32	; 0x20
    c8a4:	1afffffb 	bne	c898 <CreateCommandHeader+0x20>
    c8a8:	e3a03000 	mov	r3, #0	; 0x0
    c8ac:	e58c2008 	str	r2, [ip, #8]
    c8b0:	e58c300c 	str	r3, [ip, #12]
    c8b4:	e1cc50b2 	strh	r5, [ip, #2]
    c8b8:	e1560003 	cmp	r6, r3
    c8bc:	e7d43280 	ldrb	r3, [r4, r0, lsl #5]
    c8c0:	13c3301c 	bicne	r3, r3, #28	; 0x1c
    c8c4:	03c3301a 	biceq	r3, r3, #26	; 0x1a
    c8c8:	13833003 	orrne	r3, r3, #3	; 0x3
    c8cc:	03833005 	orreq	r3, r3, #5	; 0x5
    c8d0:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    c8d4:	e59d3010 	ldr	r3, [sp, #16]
    c8d8:	e7d42280 	ldrb	r2, [r4, r0, lsl #5]
    c8dc:	e3530000 	cmp	r3, #0	; 0x0
    c8e0:	13a03020 	movne	r3, #32	; 0x20
    c8e4:	03a03000 	moveq	r3, #0	; 0x0
    c8e8:	e3c22020 	bic	r2, r2, #32	; 0x20
    c8ec:	e1833002 	orr	r3, r3, r2
    c8f0:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    c8f4:	e6ef2073 	uxtb	r2, r3
    c8f8:	e59d3014 	ldr	r3, [sp, #20]
    c8fc:	e3c22040 	bic	r2, r2, #64	; 0x40
    c900:	e3530000 	cmp	r3, #0	; 0x0
    c904:	13a03040 	movne	r3, #64	; 0x40
    c908:	03a03000 	moveq	r3, #0	; 0x0
    c90c:	e1833002 	orr	r3, r3, r2
    c910:	e6ef3073 	uxtb	r3, r3
    c914:	e3c33080 	bic	r3, r3, #128	; 0x80
    c918:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    c91c:	e5dc3001 	ldrb	r3, [ip, #1]
    c920:	e3560000 	cmp	r6, #0	; 0x0
    c924:	e3c330f0 	bic	r3, r3, #240	; 0xf0
    c928:	e6ef3073 	uxtb	r3, r3
    c92c:	13a01002 	movne	r1, #2	; 0x2
    c930:	03a01000 	moveq	r1, #0	; 0x0
    c934:	e3c33002 	bic	r3, r3, #2	; 0x2
    c938:	e59d001c 	ldr	r0, [sp, #28]
    c93c:	e1811003 	orr	r1, r1, r3
    c940:	e6ef3071 	uxtb	r3, r1
    c944:	e2500000 	subs	r0, r0, #0	; 0x0
    c948:	13a00001 	movne	r0, #1	; 0x1
    c94c:	e3c33001 	bic	r3, r3, #1	; 0x1
    c950:	e1803003 	orr	r3, r0, r3
    c954:	e6ef3073 	uxtb	r3, r3
    c958:	e3c33004 	bic	r3, r3, #4	; 0x4
    c95c:	e3a02000 	mov	r2, #0	; 0x0
    c960:	e1833100 	orr	r3, r3, r0, lsl #2
    c964:	e58c201c 	str	r2, [ip, #28]
    c968:	e58c2004 	str	r2, [ip, #4]
    c96c:	e5cc3001 	strb	r3, [ip, #1]
    c970:	e58c2010 	str	r2, [ip, #16]
    c974:	e58c2014 	str	r2, [ip, #20]
    c978:	e58c2018 	str	r2, [ip, #24]
    c97c:	e8bd8070 	pop	{r4, r5, r6, pc}

0000c980 <get_max_lba>:
    c980:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    c984:	e3a04000 	mov	r4, #0	; 0x0
    c988:	e2807c05 	add	r7, r0, #1280	; 0x500
    c98c:	e1a06000 	mov	r6, r0
    c990:	e24dd010 	sub	sp, sp, #16	; 0x10
    c994:	e3a010ec 	mov	r1, #236	; 0xec
    c998:	e3a02001 	mov	r2, #1	; 0x1
    c99c:	e1a03004 	mov	r3, r4
    c9a0:	e1a00007 	mov	r0, r7
    c9a4:	e58d4000 	str	r4, [sp]
    c9a8:	e58d4004 	str	r4, [sp, #4]
    c9ac:	e2868b01 	add	r8, r6, #1024	; 0x400
    c9b0:	ebffff72 	bl	c780 <CreateAtaCFIS>
    c9b4:	e2862b02 	add	r2, r6, #2048	; 0x800
    c9b8:	e2863d16 	add	r3, r6, #1408	; 0x580
    c9bc:	e1a01004 	mov	r1, r4
    c9c0:	e7c31004 	strb	r1, [r3, r4]
    c9c4:	e2844001 	add	r4, r4, #1	; 0x1
    c9c8:	e3540010 	cmp	r4, #16	; 0x10
    c9cc:	e3a05000 	mov	r5, #0	; 0x0
    c9d0:	1afffffa 	bne	c9c0 <get_max_lba+0x40>
    c9d4:	e5872080 	str	r2, [r7, #128]
    c9d8:	e596258c 	ldr	r2, [r6, #1420]
    c9dc:	e5865584 	str	r5, [r6, #1412]
    c9e0:	e1a03b22 	lsr	r3, r2, #22
    c9e4:	e1a03b03 	lsl	r3, r3, #22
    c9e8:	e1e024a3 	mvn	r2, r3, lsr #9
    c9ec:	e1e02482 	mvn	r2, r2, lsl #9
    c9f0:	e1a03c22 	lsr	r3, r2, #24
    c9f4:	e3c33080 	bic	r3, r3, #128	; 0x80
    c9f8:	e586258c 	str	r2, [r6, #1420]
    c9fc:	e1a01005 	mov	r1, r5
    ca00:	e5c6358f 	strb	r3, [r6, #1423]
    ca04:	e1a02007 	mov	r2, r7
    ca08:	e3a03001 	mov	r3, #1	; 0x1
    ca0c:	e1a00006 	mov	r0, r6
    ca10:	e58d5000 	str	r5, [sp]
    ca14:	e58d5004 	str	r5, [sp, #4]
    ca18:	e58d5008 	str	r5, [sp, #8]
    ca1c:	e58d500c 	str	r5, [sp, #12]
    ca20:	ebffff94 	bl	c878 <CreateCommandHeader>
    ca24:	e1a01005 	mov	r1, r5
    ca28:	e1a03005 	mov	r3, r5
    ca2c:	e7c83001 	strb	r3, [r8, r1]
    ca30:	e2811001 	add	r1, r1, #1	; 0x1
    ca34:	e3510c01 	cmp	r1, #256	; 0x100
    ca38:	1afffffb 	bne	ca2c <get_max_lba+0xac>
    ca3c:	e59f20c8 	ldr	r2, [pc, #200]	; cb0c <get_max_lba+0x18c>
    ca40:	e3e03000 	mvn	r3, #0	; 0x0
    ca44:	e5826100 	str	r6, [r2, #256]
    ca48:	e5828108 	str	r8, [r2, #264]
    ca4c:	e5823110 	str	r3, [r2, #272]
    ca50:	e5823130 	str	r3, [r2, #304]
    ca54:	e5923118 	ldr	r3, [r2, #280]
    ca58:	e1a00002 	mov	r0, r2
    ca5c:	e3833010 	orr	r3, r3, #16	; 0x10
    ca60:	e5823118 	str	r3, [r2, #280]
    ca64:	e5923118 	ldr	r3, [r2, #280]
    ca68:	e3833001 	orr	r3, r3, #1	; 0x1
    ca6c:	e5823118 	str	r3, [r2, #280]
    ca70:	e3a03001 	mov	r3, #1	; 0x1
    ca74:	e5823138 	str	r3, [r2, #312]
    ca78:	e5903110 	ldr	r3, [r0, #272]
    ca7c:	e59f1088 	ldr	r1, [pc, #136]	; cb0c <get_max_lba+0x18c>
    ca80:	e3130002 	tst	r3, #2	; 0x2
    ca84:	0afffffb 	beq	ca78 <get_max_lba+0xf8>
    ca88:	e3e03000 	mvn	r3, #0	; 0x0
    ca8c:	e5912008 	ldr	r2, [r1, #8]
    ca90:	e5813110 	str	r3, [r1, #272]
    ca94:	e5813008 	str	r3, [r1, #8]
    ca98:	e5912118 	ldr	r2, [r1, #280]
    ca9c:	e30c3011 	movw	r3, #49169	; 0xc011
    caa0:	e0023003 	and	r3, r2, r3
    caa4:	e3530000 	cmp	r3, #0	; 0x0
    caa8:	e596e8c8 	ldr	lr, [r6, #2248]
    caac:	e59648cc 	ldr	r4, [r6, #2252]
    cab0:	0a00000e 	beq	caf0 <get_max_lba+0x170>
    cab4:	e5913118 	ldr	r3, [r1, #280]
    cab8:	e1a0c001 	mov	ip, r1
    cabc:	e3c33001 	bic	r3, r3, #1	; 0x1
    cac0:	e5813118 	str	r3, [r1, #280]
    cac4:	e59c3118 	ldr	r3, [ip, #280]
    cac8:	e59f203c 	ldr	r2, [pc, #60]	; cb0c <get_max_lba+0x18c>
    cacc:	e3130902 	tst	r3, #32768	; 0x8000
    cad0:	1afffffb 	bne	cac4 <get_max_lba+0x144>
    cad4:	e5923118 	ldr	r3, [r2, #280]
    cad8:	e1a01002 	mov	r1, r2
    cadc:	e3c33010 	bic	r3, r3, #16	; 0x10
    cae0:	e5823118 	str	r3, [r2, #280]
    cae4:	e5913118 	ldr	r3, [r1, #280]
    cae8:	e3130901 	tst	r3, #16384	; 0x4000
    caec:	1afffffc 	bne	cae4 <get_max_lba+0x164>
    caf0:	e59f2014 	ldr	r2, [pc, #20]	; cb0c <get_max_lba+0x18c>
    caf4:	e5923120 	ldr	r3, [r2, #288]
    caf8:	e2130088 	ands	r0, r3, #136	; 0x88
    cafc:	1afffffc 	bne	caf4 <get_max_lba+0x174>
    cb00:	e08e0000 	add	r0, lr, r0
    cb04:	e28dd010 	add	sp, sp, #16	; 0x10
    cb08:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    cb0c:	28200000 	.word	0x28200000

0000cb10 <sata_vs_align>:
    cb10:	e3a00c02 	mov	r0, #512	; 0x200
    cb14:	e12fff1e 	bx	lr

0000cb18 <init_sata>:
    cb18:	e12fff1e 	bx	lr

0000cb1c <sata_vs_write>:
    cb1c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    cb20:	e1a08000 	mov	r8, r0
    cb24:	e24dd034 	sub	sp, sp, #52	; 0x34
    cb28:	e3a00c0e 	mov	r0, #3584	; 0xe00
    cb2c:	e1a06002 	mov	r6, r2
    cb30:	e1a07003 	mov	r7, r3
    cb34:	ebffd312 	bl	1784 <malloc>
    cb38:	e3500000 	cmp	r0, #0	; 0x0
    cb3c:	e58d0014 	str	r0, [sp, #20]
    cb40:	1a000004 	bne	cb58 <sata_vs_write+0x3c>
    cb44:	e59f0680 	ldr	r0, [pc, #1664]	; d1cc <sata_vs_write+0x6b0>
    cb48:	e59f3680 	ldr	r3, [pc, #1664]	; d1d0 <sata_vs_write+0x6b4>
    cb4c:	e1a0e00f 	mov	lr, pc
    cb50:	e593f000 	ldr	pc, [r3]
    cb54:	ea0000ea 	b	cf04 <sata_vs_write+0x3e8>
    cb58:	e59d0014 	ldr	r0, [sp, #20]
    cb5c:	e2803fff 	add	r3, r0, #1020	; 0x3fc
    cb60:	e2833003 	add	r3, r3, #3	; 0x3
    cb64:	e3c33fff 	bic	r3, r3, #1020	; 0x3fc
    cb68:	e3c33003 	bic	r3, r3, #3	; 0x3
    cb6c:	e1530000 	cmp	r3, r0
    cb70:	e58d3018 	str	r3, [sp, #24]
    cb74:	02833a01 	addeq	r3, r3, #4096	; 0x1000
    cb78:	058d3018 	streq	r3, [sp, #24]
    cb7c:	e59d0018 	ldr	r0, [sp, #24]
    cb80:	ebffff7e 	bl	c980 <get_max_lba>
    cb84:	e59d1058 	ldr	r1, [sp, #88]
    cb88:	e1a0c000 	mov	ip, r0
    cb8c:	e1a03b81 	lsl	r3, r1, #23
    cb90:	e1a03ba3 	lsr	r3, r3, #23
    cb94:	e3530000 	cmp	r3, #0	; 0x0
    cb98:	1a0000d3 	bne	ceec <sata_vs_write+0x3d0>
    cb9c:	e30041ff 	movw	r4, #511	; 0x1ff
    cba0:	e3a05000 	mov	r5, #0	; 0x0
    cba4:	e0062004 	and	r2, r6, r4
    cba8:	e0073005 	and	r3, r7, r5
    cbac:	e1920003 	orrs	r0, r2, r3
    cbb0:	1a0000cd 	bne	ceec <sata_vs_write+0x3d0>
    cbb4:	e3180001 	tst	r8, #1	; 0x1
    cbb8:	1a0000cb 	bne	ceec <sata_vs_write+0x3d0>
    cbbc:	e1a00007 	mov	r0, r7
    cbc0:	e1a01fc0 	asr	r1, r0, #31
    cbc4:	e1a02fc1 	asr	r2, r1, #31
    cbc8:	e1a03002 	mov	r3, r2
    cbcc:	e0022004 	and	r2, r2, r4
    cbd0:	e0922006 	adds	r2, r2, r6
    cbd4:	e0033005 	and	r3, r3, r5
    cbd8:	e0a33007 	adc	r3, r3, r7
    cbdc:	e1a014a2 	lsr	r1, r2, #9
    cbe0:	e1a0600c 	mov	r6, ip
    cbe4:	e1a07fc6 	asr	r7, r6, #31
    cbe8:	e1811b83 	orr	r1, r1, r3, lsl #23
    cbec:	e3570000 	cmp	r7, #0	; 0x0
    cbf0:	e58d1028 	str	r1, [sp, #40]
    cbf4:	e1a00001 	mov	r0, r1
    cbf8:	e3a01000 	mov	r1, #0	; 0x0
    cbfc:	8a000002 	bhi	cc0c <sata_vs_write+0xf0>
    cc00:	1a0000b9 	bne	ceec <sata_vs_write+0x3d0>
    cc04:	e1560000 	cmp	r6, r0
    cc08:	9a0000b7 	bls	ceec <sata_vs_write+0x3d0>
    cc0c:	e59d2058 	ldr	r2, [sp, #88]
    cc10:	e3520402 	cmp	r2, #33554432	; 0x2000000
    cc14:	ba000007 	blt	cc38 <sata_vs_write+0x11c>
    cc18:	e1a02006 	mov	r2, r6
    cc1c:	e1a03007 	mov	r3, r7
    cc20:	e0522000 	subs	r2, r2, r0
    cc24:	e0c33001 	sbc	r3, r3, r1
    cc28:	e3530000 	cmp	r3, #0	; 0x0
    cc2c:	1a000001 	bne	cc38 <sata_vs_write+0x11c>
    cc30:	e3520801 	cmp	r2, #65536	; 0x10000
    cc34:	3a0000ac 	bcc	ceec <sata_vs_write+0x3d0>
    cc38:	e59d3058 	ldr	r3, [sp, #88]
    cc3c:	e3530401 	cmp	r3, #16777216	; 0x1000000
    cc40:	ba000007 	blt	cc64 <sata_vs_write+0x148>
    cc44:	e1a02006 	mov	r2, r6
    cc48:	e1a03007 	mov	r3, r7
    cc4c:	e0522000 	subs	r2, r2, r0
    cc50:	e0c33001 	sbc	r3, r3, r1
    cc54:	e3530000 	cmp	r3, #0	; 0x0
    cc58:	1a000001 	bne	cc64 <sata_vs_write+0x148>
    cc5c:	e3520902 	cmp	r2, #32768	; 0x8000
    cc60:	3a0000a1 	bcc	ceec <sata_vs_write+0x3d0>
    cc64:	e59dc058 	ldr	ip, [sp, #88]
    cc68:	e35c0502 	cmp	ip, #8388608	; 0x800000
    cc6c:	ba000007 	blt	cc90 <sata_vs_write+0x174>
    cc70:	e1a02006 	mov	r2, r6
    cc74:	e1a03007 	mov	r3, r7
    cc78:	e0522000 	subs	r2, r2, r0
    cc7c:	e0c33001 	sbc	r3, r3, r1
    cc80:	e3530000 	cmp	r3, #0	; 0x0
    cc84:	1a000001 	bne	cc90 <sata_vs_write+0x174>
    cc88:	e3520901 	cmp	r2, #16384	; 0x4000
    cc8c:	3a000096 	bcc	ceec <sata_vs_write+0x3d0>
    cc90:	e59d2058 	ldr	r2, [sp, #88]
    cc94:	e3520501 	cmp	r2, #4194304	; 0x400000
    cc98:	ba000007 	blt	ccbc <sata_vs_write+0x1a0>
    cc9c:	e1a02006 	mov	r2, r6
    cca0:	e1a03007 	mov	r3, r7
    cca4:	e0522000 	subs	r2, r2, r0
    cca8:	e0c33001 	sbc	r3, r3, r1
    ccac:	e3530000 	cmp	r3, #0	; 0x0
    ccb0:	1a000001 	bne	ccbc <sata_vs_write+0x1a0>
    ccb4:	e3520a02 	cmp	r2, #8192	; 0x2000
    ccb8:	3a00008b 	bcc	ceec <sata_vs_write+0x3d0>
    ccbc:	e59d3058 	ldr	r3, [sp, #88]
    ccc0:	e3530602 	cmp	r3, #2097152	; 0x200000
    ccc4:	ba000007 	blt	cce8 <sata_vs_write+0x1cc>
    ccc8:	e1a02006 	mov	r2, r6
    cccc:	e1a03007 	mov	r3, r7
    ccd0:	e0522000 	subs	r2, r2, r0
    ccd4:	e0c33001 	sbc	r3, r3, r1
    ccd8:	e3530000 	cmp	r3, #0	; 0x0
    ccdc:	1a000001 	bne	cce8 <sata_vs_write+0x1cc>
    cce0:	e3520a01 	cmp	r2, #4096	; 0x1000
    cce4:	3a000080 	bcc	ceec <sata_vs_write+0x3d0>
    cce8:	e59dc058 	ldr	ip, [sp, #88]
    ccec:	e35c0601 	cmp	ip, #1048576	; 0x100000
    ccf0:	ba000007 	blt	cd14 <sata_vs_write+0x1f8>
    ccf4:	e1a02006 	mov	r2, r6
    ccf8:	e1a03007 	mov	r3, r7
    ccfc:	e0522000 	subs	r2, r2, r0
    cd00:	e0c33001 	sbc	r3, r3, r1
    cd04:	e3530000 	cmp	r3, #0	; 0x0
    cd08:	1a000001 	bne	cd14 <sata_vs_write+0x1f8>
    cd0c:	e3520b02 	cmp	r2, #2048	; 0x800
    cd10:	3a000075 	bcc	ceec <sata_vs_write+0x3d0>
    cd14:	e59d2058 	ldr	r2, [sp, #88]
    cd18:	e3520702 	cmp	r2, #524288	; 0x80000
    cd1c:	ba000007 	blt	cd40 <sata_vs_write+0x224>
    cd20:	e1a02006 	mov	r2, r6
    cd24:	e1a03007 	mov	r3, r7
    cd28:	e0522000 	subs	r2, r2, r0
    cd2c:	e0c33001 	sbc	r3, r3, r1
    cd30:	e3530000 	cmp	r3, #0	; 0x0
    cd34:	1a000001 	bne	cd40 <sata_vs_write+0x224>
    cd38:	e3520b01 	cmp	r2, #1024	; 0x400
    cd3c:	3a00006a 	bcc	ceec <sata_vs_write+0x3d0>
    cd40:	e59d3058 	ldr	r3, [sp, #88]
    cd44:	e3530701 	cmp	r3, #262144	; 0x40000
    cd48:	ba000007 	blt	cd6c <sata_vs_write+0x250>
    cd4c:	e1a02006 	mov	r2, r6
    cd50:	e1a03007 	mov	r3, r7
    cd54:	e0522000 	subs	r2, r2, r0
    cd58:	e0c33001 	sbc	r3, r3, r1
    cd5c:	e3530000 	cmp	r3, #0	; 0x0
    cd60:	1a000001 	bne	cd6c <sata_vs_write+0x250>
    cd64:	e3520c02 	cmp	r2, #512	; 0x200
    cd68:	3a00005f 	bcc	ceec <sata_vs_write+0x3d0>
    cd6c:	e59dc058 	ldr	ip, [sp, #88]
    cd70:	e35c0802 	cmp	ip, #131072	; 0x20000
    cd74:	ba000008 	blt	cd9c <sata_vs_write+0x280>
    cd78:	e3e020ff 	mvn	r2, #255	; 0xff
    cd7c:	e0922006 	adds	r2, r2, r6
    cd80:	e3e03000 	mvn	r3, #0	; 0x0
    cd84:	e0a33007 	adc	r3, r3, r7
    cd88:	e1510003 	cmp	r1, r3
    cd8c:	8a000056 	bhi	ceec <sata_vs_write+0x3d0>
    cd90:	1a000001 	bne	cd9c <sata_vs_write+0x280>
    cd94:	e1500002 	cmp	r0, r2
    cd98:	8a000053 	bhi	ceec <sata_vs_write+0x3d0>
    cd9c:	e59d2058 	ldr	r2, [sp, #88]
    cda0:	e3520801 	cmp	r2, #65536	; 0x10000
    cda4:	ba000007 	blt	cdc8 <sata_vs_write+0x2ac>
    cda8:	e1a02006 	mov	r2, r6
    cdac:	e1a03007 	mov	r3, r7
    cdb0:	e0522000 	subs	r2, r2, r0
    cdb4:	e0c33001 	sbc	r3, r3, r1
    cdb8:	e3530000 	cmp	r3, #0	; 0x0
    cdbc:	1a000001 	bne	cdc8 <sata_vs_write+0x2ac>
    cdc0:	e352007f 	cmp	r2, #127	; 0x7f
    cdc4:	9a000048 	bls	ceec <sata_vs_write+0x3d0>
    cdc8:	e59d3058 	ldr	r3, [sp, #88]
    cdcc:	e3530902 	cmp	r3, #32768	; 0x8000
    cdd0:	ba000007 	blt	cdf4 <sata_vs_write+0x2d8>
    cdd4:	e1a02006 	mov	r2, r6
    cdd8:	e1a03007 	mov	r3, r7
    cddc:	e0522000 	subs	r2, r2, r0
    cde0:	e0c33001 	sbc	r3, r3, r1
    cde4:	e3530000 	cmp	r3, #0	; 0x0
    cde8:	1a000001 	bne	cdf4 <sata_vs_write+0x2d8>
    cdec:	e352003f 	cmp	r2, #63	; 0x3f
    cdf0:	9a00003d 	bls	ceec <sata_vs_write+0x3d0>
    cdf4:	e59dc058 	ldr	ip, [sp, #88]
    cdf8:	e35c0901 	cmp	ip, #16384	; 0x4000
    cdfc:	ba000007 	blt	ce20 <sata_vs_write+0x304>
    ce00:	e1a02006 	mov	r2, r6
    ce04:	e1a03007 	mov	r3, r7
    ce08:	e0522000 	subs	r2, r2, r0
    ce0c:	e0c33001 	sbc	r3, r3, r1
    ce10:	e3530000 	cmp	r3, #0	; 0x0
    ce14:	1a000001 	bne	ce20 <sata_vs_write+0x304>
    ce18:	e352001f 	cmp	r2, #31	; 0x1f
    ce1c:	9a000032 	bls	ceec <sata_vs_write+0x3d0>
    ce20:	e59d2058 	ldr	r2, [sp, #88]
    ce24:	e3520a02 	cmp	r2, #8192	; 0x2000
    ce28:	ba000007 	blt	ce4c <sata_vs_write+0x330>
    ce2c:	e1a02006 	mov	r2, r6
    ce30:	e1a03007 	mov	r3, r7
    ce34:	e0522000 	subs	r2, r2, r0
    ce38:	e0c33001 	sbc	r3, r3, r1
    ce3c:	e3530000 	cmp	r3, #0	; 0x0
    ce40:	1a000001 	bne	ce4c <sata_vs_write+0x330>
    ce44:	e352000f 	cmp	r2, #15	; 0xf
    ce48:	9a000027 	bls	ceec <sata_vs_write+0x3d0>
    ce4c:	e59d3058 	ldr	r3, [sp, #88]
    ce50:	e3530a01 	cmp	r3, #4096	; 0x1000
    ce54:	ba000007 	blt	ce78 <sata_vs_write+0x35c>
    ce58:	e1a02006 	mov	r2, r6
    ce5c:	e1a03007 	mov	r3, r7
    ce60:	e0522000 	subs	r2, r2, r0
    ce64:	e0c33001 	sbc	r3, r3, r1
    ce68:	e3530000 	cmp	r3, #0	; 0x0
    ce6c:	1a000001 	bne	ce78 <sata_vs_write+0x35c>
    ce70:	e3520007 	cmp	r2, #7	; 0x7
    ce74:	9a00001c 	bls	ceec <sata_vs_write+0x3d0>
    ce78:	e59dc058 	ldr	ip, [sp, #88]
    ce7c:	e35c0b02 	cmp	ip, #2048	; 0x800
    ce80:	ba000007 	blt	cea4 <sata_vs_write+0x388>
    ce84:	e1a02006 	mov	r2, r6
    ce88:	e1a03007 	mov	r3, r7
    ce8c:	e0522000 	subs	r2, r2, r0
    ce90:	e0c33001 	sbc	r3, r3, r1
    ce94:	e3530000 	cmp	r3, #0	; 0x0
    ce98:	1a000001 	bne	cea4 <sata_vs_write+0x388>
    ce9c:	e3520003 	cmp	r2, #3	; 0x3
    cea0:	9a000011 	bls	ceec <sata_vs_write+0x3d0>
    cea4:	e59d2058 	ldr	r2, [sp, #88]
    cea8:	e3520b01 	cmp	r2, #1024	; 0x400
    ceac:	ba000007 	blt	ced0 <sata_vs_write+0x3b4>
    ceb0:	e1a02006 	mov	r2, r6
    ceb4:	e1a03007 	mov	r3, r7
    ceb8:	e0522000 	subs	r2, r2, r0
    cebc:	e0c33001 	sbc	r3, r3, r1
    cec0:	e3530000 	cmp	r3, #0	; 0x0
    cec4:	1a000001 	bne	ced0 <sata_vs_write+0x3b4>
    cec8:	e3520001 	cmp	r2, #1	; 0x1
    cecc:	9a000006 	bls	ceec <sata_vs_write+0x3d0>
    ced0:	e59d3058 	ldr	r3, [sp, #88]
    ced4:	e3530c02 	cmp	r3, #512	; 0x200
    ced8:	ba00000b 	blt	cf0c <sata_vs_write+0x3f0>
    cedc:	e1560000 	cmp	r6, r0
    cee0:	1a000009 	bne	cf0c <sata_vs_write+0x3f0>
    cee4:	e1570001 	cmp	r7, r1
    cee8:	1a000007 	bne	cf0c <sata_vs_write+0x3f0>
    ceec:	e59f02e0 	ldr	r0, [pc, #736]	; d1d4 <sata_vs_write+0x6b8>
    cef0:	e59f32d8 	ldr	r3, [pc, #728]	; d1d0 <sata_vs_write+0x6b4>
    cef4:	e1a0e00f 	mov	lr, pc
    cef8:	e593f000 	ldr	pc, [r3]
    cefc:	e59d0014 	ldr	r0, [sp, #20]
    cf00:	ebffd287 	bl	1924 <free>
    cf04:	e3e04000 	mvn	r4, #0	; 0x0
    cf08:	ea0000ac 	b	d1c0 <sata_vs_write+0x6a4>
    cf0c:	e59dc018 	ldr	ip, [sp, #24]
    cf10:	e59d0018 	ldr	r0, [sp, #24]
    cf14:	e59d1018 	ldr	r1, [sp, #24]
    cf18:	e28ccb01 	add	ip, ip, #1024	; 0x400
    cf1c:	e2800c05 	add	r0, r0, #1280	; 0x500
    cf20:	e2811d16 	add	r1, r1, #1408	; 0x580
    cf24:	e59fb2ac 	ldr	fp, [pc, #684]	; d1d8 <sata_vs_write+0x6bc>
    cf28:	e59d5058 	ldr	r5, [sp, #88]
    cf2c:	e58d8024 	str	r8, [sp, #36]
    cf30:	e58dc01c 	str	ip, [sp, #28]
    cf34:	e58d0020 	str	r0, [sp, #32]
    cf38:	e58d102c 	str	r1, [sp, #44]
    cf3c:	e3e024fe 	mvn	r2, #-33554432	; 0xfe000000
    cf40:	e1550002 	cmp	r5, r2
    cf44:	c3a09501 	movgt	r9, #4194304	; 0x400000
    cf48:	c3a08008 	movgt	r8, #8	; 0x8
    cf4c:	c3a0a801 	movgt	sl, #65536	; 0x10000
    cf50:	ca00001c 	bgt	cfc8 <sata_vs_write+0x4ac>
    cf54:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    cf58:	e1550003 	cmp	r5, r3
    cf5c:	c3a09501 	movgt	r9, #4194304	; 0x400000
    cf60:	c3a08004 	movgt	r8, #4	; 0x4
    cf64:	c3a0a902 	movgt	sl, #32768	; 0x8000
    cf68:	ca000016 	bgt	cfc8 <sata_vs_write+0x4ac>
    cf6c:	e59fc268 	ldr	ip, [pc, #616]	; d1dc <sata_vs_write+0x6c0>
    cf70:	e155000c 	cmp	r5, ip
    cf74:	c3a09501 	movgt	r9, #4194304	; 0x400000
    cf78:	c3a08002 	movgt	r8, #2	; 0x2
    cf7c:	c3a0a901 	movgt	sl, #16384	; 0x4000
    cf80:	ca000010 	bgt	cfc8 <sata_vs_write+0x4ac>
    cf84:	e59f0254 	ldr	r0, [pc, #596]	; d1e0 <sata_vs_write+0x6c4>
    cf88:	e1550000 	cmp	r5, r0
    cf8c:	c3a09501 	movgt	r9, #4194304	; 0x400000
    cf90:	c3a08001 	movgt	r8, #1	; 0x1
    cf94:	c3a0aa02 	movgt	sl, #8192	; 0x2000
    cf98:	ca00000a 	bgt	cfc8 <sata_vs_write+0x4ac>
    cf9c:	e59f1240 	ldr	r1, [pc, #576]	; d1e4 <sata_vs_write+0x6c8>
    cfa0:	e2453c02 	sub	r3, r5, #512	; 0x200
    cfa4:	e1530001 	cmp	r3, r1
    cfa8:	8a000080 	bhi	d1b0 <sata_vs_write+0x694>
    cfac:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    cfb0:	e3550000 	cmp	r5, #0	; 0x0
    cfb4:	e2833003 	add	r3, r3, #3	; 0x3
    cfb8:	a1a03005 	movge	r3, r5
    cfbc:	e1a0a4c3 	asr	sl, r3, #9
    cfc0:	e1a0948a 	lsl	r9, sl, #9
    cfc4:	e3a08001 	mov	r8, #1	; 0x1
    cfc8:	e3a04000 	mov	r4, #0	; 0x0
    cfcc:	e1a0200a 	mov	r2, sl
    cfd0:	e59d3028 	ldr	r3, [sp, #40]
    cfd4:	e59d0020 	ldr	r0, [sp, #32]
    cfd8:	e3a01035 	mov	r1, #53	; 0x35
    cfdc:	e58d4000 	str	r4, [sp]
    cfe0:	e58d4004 	str	r4, [sp, #4]
    cfe4:	ebfffde5 	bl	c780 <CreateAtaCFIS>
    cfe8:	e1a03208 	lsl	r3, r8, #4
    cfec:	e1a02004 	mov	r2, r4
    cff0:	e59dc02c 	ldr	ip, [sp, #44]
    cff4:	e7cc2004 	strb	r2, [ip, r4]
    cff8:	e2844001 	add	r4, r4, #1	; 0x1
    cffc:	e1540003 	cmp	r4, r3
    d000:	3afffffa 	bcc	cff0 <sata_vs_write+0x4d4>
    d004:	e2493001 	sub	r3, r9, #1	; 0x1
    d008:	e3a01000 	mov	r1, #0	; 0x0
    d00c:	e1a04503 	lsl	r4, r3, #10
    d010:	e59dc024 	ldr	ip, [sp, #36]
    d014:	ea00000b 	b	d048 <sata_vs_write+0x52c>
    d018:	e3a03000 	mov	r3, #0	; 0x0
    d01c:	e58e3004 	str	r3, [lr, #4]
    d020:	e59d3020 	ldr	r3, [sp, #32]
    d024:	e783c202 	str	ip, [r3, r2, lsl #4]
    d028:	e5902004 	ldr	r2, [r0, #4]
    d02c:	e08cc009 	add	ip, ip, r9
    d030:	e1843b22 	orr	r3, r4, r2, lsr #22
    d034:	e1a02563 	ror	r2, r3, #10
    d038:	e1a03c22 	lsr	r3, r2, #24
    d03c:	e3c33080 	bic	r3, r3, #128	; 0x80
    d040:	e5802004 	str	r2, [r0, #4]
    d044:	e5c03007 	strb	r3, [r0, #7]
    d048:	e59d0020 	ldr	r0, [sp, #32]
    d04c:	e2812008 	add	r2, r1, #8	; 0x8
    d050:	e0803201 	add	r3, r0, r1, lsl #4
    d054:	e080e202 	add	lr, r0, r2, lsl #4
    d058:	e1510008 	cmp	r1, r8
    d05c:	e2830088 	add	r0, r3, #136	; 0x88
    d060:	e2813001 	add	r3, r1, #1	; 0x1
    d064:	e6ff1073 	uxth	r1, r3
    d068:	3affffea 	bcc	d018 <sata_vs_write+0x4fc>
    d06c:	e3a04000 	mov	r4, #0	; 0x0
    d070:	e1a03008 	mov	r3, r8
    d074:	e3a0c001 	mov	ip, #1	; 0x1
    d078:	e59d0018 	ldr	r0, [sp, #24]
    d07c:	e1a01004 	mov	r1, r4
    d080:	e59d2020 	ldr	r2, [sp, #32]
    d084:	e88d1010 	stm	sp, {r4, ip}
    d088:	e58d4008 	str	r4, [sp, #8]
    d08c:	e58d400c 	str	r4, [sp, #12]
    d090:	ebfffdf8 	bl	c878 <CreateCommandHeader>
    d094:	e1a03004 	mov	r3, r4
    d098:	e59d001c 	ldr	r0, [sp, #28]
    d09c:	e7c03004 	strb	r3, [r0, r4]
    d0a0:	e2844001 	add	r4, r4, #1	; 0x1
    d0a4:	e3540c01 	cmp	r4, #256	; 0x100
    d0a8:	1afffffa 	bne	d098 <sata_vs_write+0x57c>
    d0ac:	e59d1018 	ldr	r1, [sp, #24]
    d0b0:	e3e03000 	mvn	r3, #0	; 0x0
    d0b4:	e58b1100 	str	r1, [fp, #256]
    d0b8:	e58b0108 	str	r0, [fp, #264]
    d0bc:	e58b3110 	str	r3, [fp, #272]
    d0c0:	e58b3130 	str	r3, [fp, #304]
    d0c4:	e59b3118 	ldr	r3, [fp, #280]
    d0c8:	e3a02001 	mov	r2, #1	; 0x1
    d0cc:	e3833010 	orr	r3, r3, #16	; 0x10
    d0d0:	e58b3118 	str	r3, [fp, #280]
    d0d4:	e59b3118 	ldr	r3, [fp, #280]
    d0d8:	e3833001 	orr	r3, r3, #1	; 0x1
    d0dc:	e58b3118 	str	r3, [fp, #280]
    d0e0:	e58b2138 	str	r2, [fp, #312]
    d0e4:	e59b3110 	ldr	r3, [fp, #272]
    d0e8:	e3130001 	tst	r3, #1	; 0x1
    d0ec:	0afffffc 	beq	d0e4 <sata_vs_write+0x5c8>
    d0f0:	e59fc0e0 	ldr	ip, [pc, #224]	; d1d8 <sata_vs_write+0x6bc>
    d0f4:	e3e03000 	mvn	r3, #0	; 0x0
    d0f8:	e58c3110 	str	r3, [ip, #272]
    d0fc:	e58c3008 	str	r3, [ip, #8]
    d100:	e59c2118 	ldr	r2, [ip, #280]
    d104:	e30c3011 	movw	r3, #49169	; 0xc011
    d108:	e0023003 	and	r3, r2, r3
    d10c:	e3530000 	cmp	r3, #0	; 0x0
    d110:	0a00000c 	beq	d148 <sata_vs_write+0x62c>
    d114:	e59c3118 	ldr	r3, [ip, #280]
    d118:	e3c33001 	bic	r3, r3, #1	; 0x1
    d11c:	e58c3118 	str	r3, [ip, #280]
    d120:	e59b3118 	ldr	r3, [fp, #280]
    d124:	e3130902 	tst	r3, #32768	; 0x8000
    d128:	1afffffc 	bne	d120 <sata_vs_write+0x604>
    d12c:	e59f00a4 	ldr	r0, [pc, #164]	; d1d8 <sata_vs_write+0x6bc>
    d130:	e5903118 	ldr	r3, [r0, #280]
    d134:	e3c33010 	bic	r3, r3, #16	; 0x10
    d138:	e5803118 	str	r3, [r0, #280]
    d13c:	e59b3118 	ldr	r3, [fp, #280]
    d140:	e3130901 	tst	r3, #16384	; 0x4000
    d144:	1afffffc 	bne	d13c <sata_vs_write+0x620>
    d148:	e59b3120 	ldr	r3, [fp, #288]
    d14c:	e3130088 	tst	r3, #136	; 0x88
    d150:	1afffffc 	bne	d148 <sata_vs_write+0x62c>
    d154:	e0010899 	mul	r1, r9, r8
    d158:	e0555001 	subs	r5, r5, r1
    d15c:	0a000013 	beq	d1b0 <sata_vs_write+0x694>
    d160:	e59dc028 	ldr	ip, [sp, #40]
    d164:	e1b02b85 	lsls	r2, r5, #23
    d168:	13a02001 	movne	r2, #1	; 0x1
    d16c:	e08cc00a 	add	ip, ip, sl
    d170:	e3550000 	cmp	r5, #0	; 0x0
    d174:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    d178:	e2833003 	add	r3, r3, #3	; 0x3
    d17c:	e082200c 	add	r2, r2, ip
    d180:	a1a03005 	movge	r3, r5
    d184:	e3570000 	cmp	r7, #0	; 0x0
    d188:	e58dc028 	str	ip, [sp, #40]
    d18c:	e08234c3 	add	r3, r2, r3, asr #9
    d190:	8a000002 	bhi	d1a0 <sata_vs_write+0x684>
    d194:	1a000005 	bne	d1b0 <sata_vs_write+0x694>
    d198:	e1560003 	cmp	r6, r3
    d19c:	9a000003 	bls	d1b0 <sata_vs_write+0x694>
    d1a0:	e59d0024 	ldr	r0, [sp, #36]
    d1a4:	e0800001 	add	r0, r0, r1
    d1a8:	e58d0024 	str	r0, [sp, #36]
    d1ac:	eaffff62 	b	cf3c <sata_vs_write+0x420>
    d1b0:	e59d1058 	ldr	r1, [sp, #88]
    d1b4:	e59d0014 	ldr	r0, [sp, #20]
    d1b8:	e0654001 	rsb	r4, r5, r1
    d1bc:	ebffd1d8 	bl	1924 <free>
    d1c0:	e1a00004 	mov	r0, r4
    d1c4:	e28dd034 	add	sp, sp, #52	; 0x34
    d1c8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d1cc:	0001c05d 	.word	0x0001c05d
    d1d0:	3c00d694 	.word	0x3c00d694
    d1d4:	0001c080 	.word	0x0001c080
    d1d8:	28200000 	.word	0x28200000
    d1dc:	007fffff 	.word	0x007fffff
    d1e0:	003fffff 	.word	0x003fffff
    d1e4:	003ffdff 	.word	0x003ffdff

0000d1e8 <sata_vs_read>:
    d1e8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d1ec:	e1a08000 	mov	r8, r0
    d1f0:	e24dd034 	sub	sp, sp, #52	; 0x34
    d1f4:	e3a00c0e 	mov	r0, #3584	; 0xe00
    d1f8:	e1a06002 	mov	r6, r2
    d1fc:	e1a07003 	mov	r7, r3
    d200:	ebffd15f 	bl	1784 <malloc>
    d204:	e3500000 	cmp	r0, #0	; 0x0
    d208:	e58d0014 	str	r0, [sp, #20]
    d20c:	1a000004 	bne	d224 <sata_vs_read+0x3c>
    d210:	e59f0680 	ldr	r0, [pc, #1664]	; d898 <sata_vs_read+0x6b0>
    d214:	e59f3680 	ldr	r3, [pc, #1664]	; d89c <sata_vs_read+0x6b4>
    d218:	e1a0e00f 	mov	lr, pc
    d21c:	e593f000 	ldr	pc, [r3]
    d220:	ea0000ea 	b	d5d0 <sata_vs_read+0x3e8>
    d224:	e59d0014 	ldr	r0, [sp, #20]
    d228:	e2803fff 	add	r3, r0, #1020	; 0x3fc
    d22c:	e2833003 	add	r3, r3, #3	; 0x3
    d230:	e3c33fff 	bic	r3, r3, #1020	; 0x3fc
    d234:	e3c33003 	bic	r3, r3, #3	; 0x3
    d238:	e1530000 	cmp	r3, r0
    d23c:	e58d3018 	str	r3, [sp, #24]
    d240:	02833a01 	addeq	r3, r3, #4096	; 0x1000
    d244:	058d3018 	streq	r3, [sp, #24]
    d248:	e59d0018 	ldr	r0, [sp, #24]
    d24c:	ebfffdcb 	bl	c980 <get_max_lba>
    d250:	e59d1058 	ldr	r1, [sp, #88]
    d254:	e1a0c000 	mov	ip, r0
    d258:	e1a03b81 	lsl	r3, r1, #23
    d25c:	e1a03ba3 	lsr	r3, r3, #23
    d260:	e3530000 	cmp	r3, #0	; 0x0
    d264:	1a0000d3 	bne	d5b8 <sata_vs_read+0x3d0>
    d268:	e30041ff 	movw	r4, #511	; 0x1ff
    d26c:	e3a05000 	mov	r5, #0	; 0x0
    d270:	e0062004 	and	r2, r6, r4
    d274:	e0073005 	and	r3, r7, r5
    d278:	e1920003 	orrs	r0, r2, r3
    d27c:	1a0000cd 	bne	d5b8 <sata_vs_read+0x3d0>
    d280:	e3180001 	tst	r8, #1	; 0x1
    d284:	1a0000cb 	bne	d5b8 <sata_vs_read+0x3d0>
    d288:	e1a00007 	mov	r0, r7
    d28c:	e1a01fc0 	asr	r1, r0, #31
    d290:	e1a02fc1 	asr	r2, r1, #31
    d294:	e1a03002 	mov	r3, r2
    d298:	e0022004 	and	r2, r2, r4
    d29c:	e0922006 	adds	r2, r2, r6
    d2a0:	e0033005 	and	r3, r3, r5
    d2a4:	e0a33007 	adc	r3, r3, r7
    d2a8:	e1a014a2 	lsr	r1, r2, #9
    d2ac:	e1a0600c 	mov	r6, ip
    d2b0:	e1a07fc6 	asr	r7, r6, #31
    d2b4:	e1811b83 	orr	r1, r1, r3, lsl #23
    d2b8:	e3570000 	cmp	r7, #0	; 0x0
    d2bc:	e58d1028 	str	r1, [sp, #40]
    d2c0:	e1a00001 	mov	r0, r1
    d2c4:	e3a01000 	mov	r1, #0	; 0x0
    d2c8:	8a000002 	bhi	d2d8 <sata_vs_read+0xf0>
    d2cc:	1a0000b9 	bne	d5b8 <sata_vs_read+0x3d0>
    d2d0:	e1560000 	cmp	r6, r0
    d2d4:	9a0000b7 	bls	d5b8 <sata_vs_read+0x3d0>
    d2d8:	e59d2058 	ldr	r2, [sp, #88]
    d2dc:	e3520402 	cmp	r2, #33554432	; 0x2000000
    d2e0:	ba000007 	blt	d304 <sata_vs_read+0x11c>
    d2e4:	e1a02006 	mov	r2, r6
    d2e8:	e1a03007 	mov	r3, r7
    d2ec:	e0522000 	subs	r2, r2, r0
    d2f0:	e0c33001 	sbc	r3, r3, r1
    d2f4:	e3530000 	cmp	r3, #0	; 0x0
    d2f8:	1a000001 	bne	d304 <sata_vs_read+0x11c>
    d2fc:	e3520801 	cmp	r2, #65536	; 0x10000
    d300:	3a0000ac 	bcc	d5b8 <sata_vs_read+0x3d0>
    d304:	e59d3058 	ldr	r3, [sp, #88]
    d308:	e3530401 	cmp	r3, #16777216	; 0x1000000
    d30c:	ba000007 	blt	d330 <sata_vs_read+0x148>
    d310:	e1a02006 	mov	r2, r6
    d314:	e1a03007 	mov	r3, r7
    d318:	e0522000 	subs	r2, r2, r0
    d31c:	e0c33001 	sbc	r3, r3, r1
    d320:	e3530000 	cmp	r3, #0	; 0x0
    d324:	1a000001 	bne	d330 <sata_vs_read+0x148>
    d328:	e3520902 	cmp	r2, #32768	; 0x8000
    d32c:	3a0000a1 	bcc	d5b8 <sata_vs_read+0x3d0>
    d330:	e59d2058 	ldr	r2, [sp, #88]
    d334:	e3520502 	cmp	r2, #8388608	; 0x800000
    d338:	ba000007 	blt	d35c <sata_vs_read+0x174>
    d33c:	e1a02006 	mov	r2, r6
    d340:	e1a03007 	mov	r3, r7
    d344:	e0522000 	subs	r2, r2, r0
    d348:	e0c33001 	sbc	r3, r3, r1
    d34c:	e3530000 	cmp	r3, #0	; 0x0
    d350:	1a000001 	bne	d35c <sata_vs_read+0x174>
    d354:	e3520901 	cmp	r2, #16384	; 0x4000
    d358:	3a000096 	bcc	d5b8 <sata_vs_read+0x3d0>
    d35c:	e59d3058 	ldr	r3, [sp, #88]
    d360:	e3530501 	cmp	r3, #4194304	; 0x400000
    d364:	ba000007 	blt	d388 <sata_vs_read+0x1a0>
    d368:	e1a02006 	mov	r2, r6
    d36c:	e1a03007 	mov	r3, r7
    d370:	e0522000 	subs	r2, r2, r0
    d374:	e0c33001 	sbc	r3, r3, r1
    d378:	e3530000 	cmp	r3, #0	; 0x0
    d37c:	1a000001 	bne	d388 <sata_vs_read+0x1a0>
    d380:	e3520a02 	cmp	r2, #8192	; 0x2000
    d384:	3a00008b 	bcc	d5b8 <sata_vs_read+0x3d0>
    d388:	e59d2058 	ldr	r2, [sp, #88]
    d38c:	e3520602 	cmp	r2, #2097152	; 0x200000
    d390:	ba000007 	blt	d3b4 <sata_vs_read+0x1cc>
    d394:	e1a02006 	mov	r2, r6
    d398:	e1a03007 	mov	r3, r7
    d39c:	e0522000 	subs	r2, r2, r0
    d3a0:	e0c33001 	sbc	r3, r3, r1
    d3a4:	e3530000 	cmp	r3, #0	; 0x0
    d3a8:	1a000001 	bne	d3b4 <sata_vs_read+0x1cc>
    d3ac:	e3520a01 	cmp	r2, #4096	; 0x1000
    d3b0:	3a000080 	bcc	d5b8 <sata_vs_read+0x3d0>
    d3b4:	e59d3058 	ldr	r3, [sp, #88]
    d3b8:	e3530601 	cmp	r3, #1048576	; 0x100000
    d3bc:	ba000007 	blt	d3e0 <sata_vs_read+0x1f8>
    d3c0:	e1a02006 	mov	r2, r6
    d3c4:	e1a03007 	mov	r3, r7
    d3c8:	e0522000 	subs	r2, r2, r0
    d3cc:	e0c33001 	sbc	r3, r3, r1
    d3d0:	e3530000 	cmp	r3, #0	; 0x0
    d3d4:	1a000001 	bne	d3e0 <sata_vs_read+0x1f8>
    d3d8:	e3520b02 	cmp	r2, #2048	; 0x800
    d3dc:	3a000075 	bcc	d5b8 <sata_vs_read+0x3d0>
    d3e0:	e59d2058 	ldr	r2, [sp, #88]
    d3e4:	e3520702 	cmp	r2, #524288	; 0x80000
    d3e8:	ba000007 	blt	d40c <sata_vs_read+0x224>
    d3ec:	e1a02006 	mov	r2, r6
    d3f0:	e1a03007 	mov	r3, r7
    d3f4:	e0522000 	subs	r2, r2, r0
    d3f8:	e0c33001 	sbc	r3, r3, r1
    d3fc:	e3530000 	cmp	r3, #0	; 0x0
    d400:	1a000001 	bne	d40c <sata_vs_read+0x224>
    d404:	e3520b01 	cmp	r2, #1024	; 0x400
    d408:	3a00006a 	bcc	d5b8 <sata_vs_read+0x3d0>
    d40c:	e59d3058 	ldr	r3, [sp, #88]
    d410:	e3530701 	cmp	r3, #262144	; 0x40000
    d414:	ba000007 	blt	d438 <sata_vs_read+0x250>
    d418:	e1a02006 	mov	r2, r6
    d41c:	e1a03007 	mov	r3, r7
    d420:	e0522000 	subs	r2, r2, r0
    d424:	e0c33001 	sbc	r3, r3, r1
    d428:	e3530000 	cmp	r3, #0	; 0x0
    d42c:	1a000001 	bne	d438 <sata_vs_read+0x250>
    d430:	e3520c02 	cmp	r2, #512	; 0x200
    d434:	3a00005f 	bcc	d5b8 <sata_vs_read+0x3d0>
    d438:	e59d2058 	ldr	r2, [sp, #88]
    d43c:	e3520802 	cmp	r2, #131072	; 0x20000
    d440:	ba000008 	blt	d468 <sata_vs_read+0x280>
    d444:	e3e020ff 	mvn	r2, #255	; 0xff
    d448:	e0922006 	adds	r2, r2, r6
    d44c:	e3e03000 	mvn	r3, #0	; 0x0
    d450:	e0a33007 	adc	r3, r3, r7
    d454:	e1510003 	cmp	r1, r3
    d458:	8a000056 	bhi	d5b8 <sata_vs_read+0x3d0>
    d45c:	1a000001 	bne	d468 <sata_vs_read+0x280>
    d460:	e1500002 	cmp	r0, r2
    d464:	8a000053 	bhi	d5b8 <sata_vs_read+0x3d0>
    d468:	e59d3058 	ldr	r3, [sp, #88]
    d46c:	e3530801 	cmp	r3, #65536	; 0x10000
    d470:	ba000007 	blt	d494 <sata_vs_read+0x2ac>
    d474:	e1a02006 	mov	r2, r6
    d478:	e1a03007 	mov	r3, r7
    d47c:	e0522000 	subs	r2, r2, r0
    d480:	e0c33001 	sbc	r3, r3, r1
    d484:	e3530000 	cmp	r3, #0	; 0x0
    d488:	1a000001 	bne	d494 <sata_vs_read+0x2ac>
    d48c:	e352007f 	cmp	r2, #127	; 0x7f
    d490:	9a000048 	bls	d5b8 <sata_vs_read+0x3d0>
    d494:	e59d2058 	ldr	r2, [sp, #88]
    d498:	e3520902 	cmp	r2, #32768	; 0x8000
    d49c:	ba000007 	blt	d4c0 <sata_vs_read+0x2d8>
    d4a0:	e1a02006 	mov	r2, r6
    d4a4:	e1a03007 	mov	r3, r7
    d4a8:	e0522000 	subs	r2, r2, r0
    d4ac:	e0c33001 	sbc	r3, r3, r1
    d4b0:	e3530000 	cmp	r3, #0	; 0x0
    d4b4:	1a000001 	bne	d4c0 <sata_vs_read+0x2d8>
    d4b8:	e352003f 	cmp	r2, #63	; 0x3f
    d4bc:	9a00003d 	bls	d5b8 <sata_vs_read+0x3d0>
    d4c0:	e59d3058 	ldr	r3, [sp, #88]
    d4c4:	e3530901 	cmp	r3, #16384	; 0x4000
    d4c8:	ba000007 	blt	d4ec <sata_vs_read+0x304>
    d4cc:	e1a02006 	mov	r2, r6
    d4d0:	e1a03007 	mov	r3, r7
    d4d4:	e0522000 	subs	r2, r2, r0
    d4d8:	e0c33001 	sbc	r3, r3, r1
    d4dc:	e3530000 	cmp	r3, #0	; 0x0
    d4e0:	1a000001 	bne	d4ec <sata_vs_read+0x304>
    d4e4:	e352001f 	cmp	r2, #31	; 0x1f
    d4e8:	9a000032 	bls	d5b8 <sata_vs_read+0x3d0>
    d4ec:	e59d2058 	ldr	r2, [sp, #88]
    d4f0:	e3520a02 	cmp	r2, #8192	; 0x2000
    d4f4:	ba000007 	blt	d518 <sata_vs_read+0x330>
    d4f8:	e1a02006 	mov	r2, r6
    d4fc:	e1a03007 	mov	r3, r7
    d500:	e0522000 	subs	r2, r2, r0
    d504:	e0c33001 	sbc	r3, r3, r1
    d508:	e3530000 	cmp	r3, #0	; 0x0
    d50c:	1a000001 	bne	d518 <sata_vs_read+0x330>
    d510:	e352000f 	cmp	r2, #15	; 0xf
    d514:	9a000027 	bls	d5b8 <sata_vs_read+0x3d0>
    d518:	e59d3058 	ldr	r3, [sp, #88]
    d51c:	e3530a01 	cmp	r3, #4096	; 0x1000
    d520:	ba000007 	blt	d544 <sata_vs_read+0x35c>
    d524:	e1a02006 	mov	r2, r6
    d528:	e1a03007 	mov	r3, r7
    d52c:	e0522000 	subs	r2, r2, r0
    d530:	e0c33001 	sbc	r3, r3, r1
    d534:	e3530000 	cmp	r3, #0	; 0x0
    d538:	1a000001 	bne	d544 <sata_vs_read+0x35c>
    d53c:	e3520007 	cmp	r2, #7	; 0x7
    d540:	9a00001c 	bls	d5b8 <sata_vs_read+0x3d0>
    d544:	e59d2058 	ldr	r2, [sp, #88]
    d548:	e3520b02 	cmp	r2, #2048	; 0x800
    d54c:	ba000007 	blt	d570 <sata_vs_read+0x388>
    d550:	e1a02006 	mov	r2, r6
    d554:	e1a03007 	mov	r3, r7
    d558:	e0522000 	subs	r2, r2, r0
    d55c:	e0c33001 	sbc	r3, r3, r1
    d560:	e3530000 	cmp	r3, #0	; 0x0
    d564:	1a000001 	bne	d570 <sata_vs_read+0x388>
    d568:	e3520003 	cmp	r2, #3	; 0x3
    d56c:	9a000011 	bls	d5b8 <sata_vs_read+0x3d0>
    d570:	e59d3058 	ldr	r3, [sp, #88]
    d574:	e3530b01 	cmp	r3, #1024	; 0x400
    d578:	ba000007 	blt	d59c <sata_vs_read+0x3b4>
    d57c:	e1a02006 	mov	r2, r6
    d580:	e1a03007 	mov	r3, r7
    d584:	e0522000 	subs	r2, r2, r0
    d588:	e0c33001 	sbc	r3, r3, r1
    d58c:	e3530000 	cmp	r3, #0	; 0x0
    d590:	1a000001 	bne	d59c <sata_vs_read+0x3b4>
    d594:	e3520001 	cmp	r2, #1	; 0x1
    d598:	9a000006 	bls	d5b8 <sata_vs_read+0x3d0>
    d59c:	e59d2058 	ldr	r2, [sp, #88]
    d5a0:	e3520c02 	cmp	r2, #512	; 0x200
    d5a4:	ba00000b 	blt	d5d8 <sata_vs_read+0x3f0>
    d5a8:	e1560000 	cmp	r6, r0
    d5ac:	1a000009 	bne	d5d8 <sata_vs_read+0x3f0>
    d5b0:	e1570001 	cmp	r7, r1
    d5b4:	1a000007 	bne	d5d8 <sata_vs_read+0x3f0>
    d5b8:	e59f02e0 	ldr	r0, [pc, #736]	; d8a0 <sata_vs_read+0x6b8>
    d5bc:	e59f32d8 	ldr	r3, [pc, #728]	; d89c <sata_vs_read+0x6b4>
    d5c0:	e1a0e00f 	mov	lr, pc
    d5c4:	e593f000 	ldr	pc, [r3]
    d5c8:	e59d0014 	ldr	r0, [sp, #20]
    d5cc:	ebffd0d4 	bl	1924 <free>
    d5d0:	e3e04000 	mvn	r4, #0	; 0x0
    d5d4:	ea0000ac 	b	d88c <sata_vs_read+0x6a4>
    d5d8:	e59d3018 	ldr	r3, [sp, #24]
    d5dc:	e59d0018 	ldr	r0, [sp, #24]
    d5e0:	e59d1018 	ldr	r1, [sp, #24]
    d5e4:	e2833b01 	add	r3, r3, #1024	; 0x400
    d5e8:	e2800c05 	add	r0, r0, #1280	; 0x500
    d5ec:	e2811d16 	add	r1, r1, #1408	; 0x580
    d5f0:	e59fb2ac 	ldr	fp, [pc, #684]	; d8a4 <sata_vs_read+0x6bc>
    d5f4:	e59d5058 	ldr	r5, [sp, #88]
    d5f8:	e58d8024 	str	r8, [sp, #36]
    d5fc:	e58d301c 	str	r3, [sp, #28]
    d600:	e58d0020 	str	r0, [sp, #32]
    d604:	e58d102c 	str	r1, [sp, #44]
    d608:	e3e024fe 	mvn	r2, #-33554432	; 0xfe000000
    d60c:	e1550002 	cmp	r5, r2
    d610:	c3a09501 	movgt	r9, #4194304	; 0x400000
    d614:	c3a08008 	movgt	r8, #8	; 0x8
    d618:	c3a0a801 	movgt	sl, #65536	; 0x10000
    d61c:	ca00001c 	bgt	d694 <sata_vs_read+0x4ac>
    d620:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    d624:	e1550003 	cmp	r5, r3
    d628:	c3a09501 	movgt	r9, #4194304	; 0x400000
    d62c:	c3a08004 	movgt	r8, #4	; 0x4
    d630:	c3a0a902 	movgt	sl, #32768	; 0x8000
    d634:	ca000016 	bgt	d694 <sata_vs_read+0x4ac>
    d638:	e59f0268 	ldr	r0, [pc, #616]	; d8a8 <sata_vs_read+0x6c0>
    d63c:	e1550000 	cmp	r5, r0
    d640:	c3a09501 	movgt	r9, #4194304	; 0x400000
    d644:	c3a08002 	movgt	r8, #2	; 0x2
    d648:	c3a0a901 	movgt	sl, #16384	; 0x4000
    d64c:	ca000010 	bgt	d694 <sata_vs_read+0x4ac>
    d650:	e59f1254 	ldr	r1, [pc, #596]	; d8ac <sata_vs_read+0x6c4>
    d654:	e1550001 	cmp	r5, r1
    d658:	c3a09501 	movgt	r9, #4194304	; 0x400000
    d65c:	c3a08001 	movgt	r8, #1	; 0x1
    d660:	c3a0aa02 	movgt	sl, #8192	; 0x2000
    d664:	ca00000a 	bgt	d694 <sata_vs_read+0x4ac>
    d668:	e59f2240 	ldr	r2, [pc, #576]	; d8b0 <sata_vs_read+0x6c8>
    d66c:	e2453c02 	sub	r3, r5, #512	; 0x200
    d670:	e1530002 	cmp	r3, r2
    d674:	8a000080 	bhi	d87c <sata_vs_read+0x694>
    d678:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    d67c:	e3550000 	cmp	r5, #0	; 0x0
    d680:	e2833003 	add	r3, r3, #3	; 0x3
    d684:	a1a03005 	movge	r3, r5
    d688:	e1a0a4c3 	asr	sl, r3, #9
    d68c:	e1a0948a 	lsl	r9, sl, #9
    d690:	e3a08001 	mov	r8, #1	; 0x1
    d694:	e3a04000 	mov	r4, #0	; 0x0
    d698:	e1a0200a 	mov	r2, sl
    d69c:	e59d3028 	ldr	r3, [sp, #40]
    d6a0:	e59d0020 	ldr	r0, [sp, #32]
    d6a4:	e3a01025 	mov	r1, #37	; 0x25
    d6a8:	e58d4000 	str	r4, [sp]
    d6ac:	e58d4004 	str	r4, [sp, #4]
    d6b0:	ebfffc32 	bl	c780 <CreateAtaCFIS>
    d6b4:	e1a03208 	lsl	r3, r8, #4
    d6b8:	e1a02004 	mov	r2, r4
    d6bc:	e59d002c 	ldr	r0, [sp, #44]
    d6c0:	e7c02004 	strb	r2, [r0, r4]
    d6c4:	e2844001 	add	r4, r4, #1	; 0x1
    d6c8:	e1540003 	cmp	r4, r3
    d6cc:	3afffffa 	bcc	d6bc <sata_vs_read+0x4d4>
    d6d0:	e2493001 	sub	r3, r9, #1	; 0x1
    d6d4:	e3a01000 	mov	r1, #0	; 0x0
    d6d8:	e1a04503 	lsl	r4, r3, #10
    d6dc:	e59dc024 	ldr	ip, [sp, #36]
    d6e0:	ea00000b 	b	d714 <sata_vs_read+0x52c>
    d6e4:	e3a03000 	mov	r3, #0	; 0x0
    d6e8:	e58e3004 	str	r3, [lr, #4]
    d6ec:	e59d3020 	ldr	r3, [sp, #32]
    d6f0:	e783c202 	str	ip, [r3, r2, lsl #4]
    d6f4:	e5902004 	ldr	r2, [r0, #4]
    d6f8:	e08cc009 	add	ip, ip, r9
    d6fc:	e1843b22 	orr	r3, r4, r2, lsr #22
    d700:	e1a02563 	ror	r2, r3, #10
    d704:	e1a03c22 	lsr	r3, r2, #24
    d708:	e3c33080 	bic	r3, r3, #128	; 0x80
    d70c:	e5802004 	str	r2, [r0, #4]
    d710:	e5c03007 	strb	r3, [r0, #7]
    d714:	e59d0020 	ldr	r0, [sp, #32]
    d718:	e2812008 	add	r2, r1, #8	; 0x8
    d71c:	e0803201 	add	r3, r0, r1, lsl #4
    d720:	e080e202 	add	lr, r0, r2, lsl #4
    d724:	e1510008 	cmp	r1, r8
    d728:	e2830088 	add	r0, r3, #136	; 0x88
    d72c:	e2813001 	add	r3, r1, #1	; 0x1
    d730:	e6ff1073 	uxth	r1, r3
    d734:	3affffea 	bcc	d6e4 <sata_vs_read+0x4fc>
    d738:	e3a04000 	mov	r4, #0	; 0x0
    d73c:	e1a03008 	mov	r3, r8
    d740:	e59d0018 	ldr	r0, [sp, #24]
    d744:	e1a01004 	mov	r1, r4
    d748:	e59d2020 	ldr	r2, [sp, #32]
    d74c:	e58d4000 	str	r4, [sp]
    d750:	e58d4004 	str	r4, [sp, #4]
    d754:	e58d4008 	str	r4, [sp, #8]
    d758:	e58d400c 	str	r4, [sp, #12]
    d75c:	ebfffc45 	bl	c878 <CreateCommandHeader>
    d760:	e1a03004 	mov	r3, r4
    d764:	e59d101c 	ldr	r1, [sp, #28]
    d768:	e7c13004 	strb	r3, [r1, r4]
    d76c:	e2844001 	add	r4, r4, #1	; 0x1
    d770:	e3540c01 	cmp	r4, #256	; 0x100
    d774:	1afffffa 	bne	d764 <sata_vs_read+0x57c>
    d778:	e59d2018 	ldr	r2, [sp, #24]
    d77c:	e3e03000 	mvn	r3, #0	; 0x0
    d780:	e58b2100 	str	r2, [fp, #256]
    d784:	e58b1108 	str	r1, [fp, #264]
    d788:	e58b3110 	str	r3, [fp, #272]
    d78c:	e58b3130 	str	r3, [fp, #304]
    d790:	e59b3118 	ldr	r3, [fp, #280]
    d794:	e3833010 	orr	r3, r3, #16	; 0x10
    d798:	e58b3118 	str	r3, [fp, #280]
    d79c:	e59b3118 	ldr	r3, [fp, #280]
    d7a0:	e3833001 	orr	r3, r3, #1	; 0x1
    d7a4:	e58b3118 	str	r3, [fp, #280]
    d7a8:	e3a03001 	mov	r3, #1	; 0x1
    d7ac:	e58b3138 	str	r3, [fp, #312]
    d7b0:	e59b3110 	ldr	r3, [fp, #272]
    d7b4:	e3130001 	tst	r3, #1	; 0x1
    d7b8:	0afffffc 	beq	d7b0 <sata_vs_read+0x5c8>
    d7bc:	e59f00e0 	ldr	r0, [pc, #224]	; d8a4 <sata_vs_read+0x6bc>
    d7c0:	e3e03000 	mvn	r3, #0	; 0x0
    d7c4:	e5803110 	str	r3, [r0, #272]
    d7c8:	e5803008 	str	r3, [r0, #8]
    d7cc:	e5902118 	ldr	r2, [r0, #280]
    d7d0:	e30c3011 	movw	r3, #49169	; 0xc011
    d7d4:	e0023003 	and	r3, r2, r3
    d7d8:	e3530000 	cmp	r3, #0	; 0x0
    d7dc:	0a00000c 	beq	d814 <sata_vs_read+0x62c>
    d7e0:	e5903118 	ldr	r3, [r0, #280]
    d7e4:	e3c33001 	bic	r3, r3, #1	; 0x1
    d7e8:	e5803118 	str	r3, [r0, #280]
    d7ec:	e59b3118 	ldr	r3, [fp, #280]
    d7f0:	e3130902 	tst	r3, #32768	; 0x8000
    d7f4:	1afffffc 	bne	d7ec <sata_vs_read+0x604>
    d7f8:	e59f10a4 	ldr	r1, [pc, #164]	; d8a4 <sata_vs_read+0x6bc>
    d7fc:	e5913118 	ldr	r3, [r1, #280]
    d800:	e3c33010 	bic	r3, r3, #16	; 0x10
    d804:	e5813118 	str	r3, [r1, #280]
    d808:	e59b3118 	ldr	r3, [fp, #280]
    d80c:	e3130901 	tst	r3, #16384	; 0x4000
    d810:	1afffffc 	bne	d808 <sata_vs_read+0x620>
    d814:	e59b3120 	ldr	r3, [fp, #288]
    d818:	e3130088 	tst	r3, #136	; 0x88
    d81c:	1afffffc 	bne	d814 <sata_vs_read+0x62c>
    d820:	e0010899 	mul	r1, r9, r8
    d824:	e0555001 	subs	r5, r5, r1
    d828:	0a000013 	beq	d87c <sata_vs_read+0x694>
    d82c:	e59d0028 	ldr	r0, [sp, #40]
    d830:	e1b02b85 	lsls	r2, r5, #23
    d834:	13a02001 	movne	r2, #1	; 0x1
    d838:	e080000a 	add	r0, r0, sl
    d83c:	e3550000 	cmp	r5, #0	; 0x0
    d840:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    d844:	e2833003 	add	r3, r3, #3	; 0x3
    d848:	e0822000 	add	r2, r2, r0
    d84c:	a1a03005 	movge	r3, r5
    d850:	e3570000 	cmp	r7, #0	; 0x0
    d854:	e58d0028 	str	r0, [sp, #40]
    d858:	e08234c3 	add	r3, r2, r3, asr #9
    d85c:	8a000002 	bhi	d86c <sata_vs_read+0x684>
    d860:	1a000005 	bne	d87c <sata_vs_read+0x694>
    d864:	e1560003 	cmp	r6, r3
    d868:	9a000003 	bls	d87c <sata_vs_read+0x694>
    d86c:	e59d2024 	ldr	r2, [sp, #36]
    d870:	e0822001 	add	r2, r2, r1
    d874:	e58d2024 	str	r2, [sp, #36]
    d878:	eaffff62 	b	d608 <sata_vs_read+0x420>
    d87c:	e59d3058 	ldr	r3, [sp, #88]
    d880:	e59d0014 	ldr	r0, [sp, #20]
    d884:	e0654003 	rsb	r4, r5, r3
    d888:	ebffd025 	bl	1924 <free>
    d88c:	e1a00004 	mov	r0, r4
    d890:	e28dd034 	add	sp, sp, #52	; 0x34
    d894:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d898:	0001c0a3 	.word	0x0001c0a3
    d89c:	3c00d694 	.word	0x3c00d694
    d8a0:	0001c080 	.word	0x0001c080
    d8a4:	28200000 	.word	0x28200000
    d8a8:	007fffff 	.word	0x007fffff
    d8ac:	003fffff 	.word	0x003fffff
    d8b0:	003ffdff 	.word	0x003ffdff

0000d8b4 <sata_vs_reset>:
    d8b4:	e92d4070 	push	{r4, r5, r6, lr}
    d8b8:	e59f02d4 	ldr	r0, [pc, #724]	; db94 <sata_vs_reset+0x2e0>
    d8bc:	ebffcbbc 	bl	7b4 <module_enable>
    d8c0:	e59f32d0 	ldr	r3, [pc, #720]	; db98 <sata_vs_reset+0x2e4>
    d8c4:	e3a01000 	mov	r1, #0	; 0x0
    d8c8:	e3a0c001 	mov	ip, #1	; 0x1
    d8cc:	e3a06010 	mov	r6, #16	; 0x10
    d8d0:	e3a02006 	mov	r2, #6	; 0x6
    d8d4:	e583c830 	str	ip, [r3, #2096]
    d8d8:	e3a04002 	mov	r4, #2	; 0x2
    d8dc:	e5836820 	str	r6, [r3, #2080]
    d8e0:	e5831824 	str	r1, [r3, #2084]
    d8e4:	e5832828 	str	r2, [r3, #2088]
    d8e8:	e282200c 	add	r2, r2, #12	; 0xc
    d8ec:	e583482c 	str	r4, [r3, #2092]
    d8f0:	e5834834 	str	r4, [r3, #2100]
    d8f4:	e5831838 	str	r1, [r3, #2104]
    d8f8:	e583183c 	str	r1, [r3, #2108]
    d8fc:	e5831840 	str	r1, [r3, #2112]
    d900:	e5832844 	str	r2, [r3, #2116]
    d904:	e3a02009 	mov	r2, #9	; 0x9
    d908:	e583c848 	str	ip, [r3, #2120]
    d90c:	e3a05003 	mov	r5, #3	; 0x3
    d910:	e583184c 	str	r1, [r3, #2124]
    d914:	e5832850 	str	r2, [r3, #2128]
    d918:	e082200c 	add	r2, r2, ip
    d91c:	e5831854 	str	r1, [r3, #2132]
    d920:	e3040e20 	movw	r0, #20000	; 0x4e20
    d924:	e5832858 	str	r2, [r3, #2136]
    d928:	e583585c 	str	r5, [r3, #2140]
    d92c:	e583c860 	str	ip, [r3, #2144]
    d930:	e5831864 	str	r1, [r3, #2148]
    d934:	e5831868 	str	r1, [r3, #2152]
    d938:	e583486c 	str	r4, [r3, #2156]
    d93c:	e583c870 	str	ip, [r3, #2160]
    d940:	e5835874 	str	r5, [r3, #2164]
    d944:	e5836878 	str	r6, [r3, #2168]
    d948:	e5831830 	str	r1, [r3, #2096]
    d94c:	ebfff6f0 	bl	b514 <udelay>
    d950:	e59f2244 	ldr	r2, [pc, #580]	; db9c <sata_vs_reset+0x2e8>
    d954:	e5923004 	ldr	r3, [r2, #4]
    d958:	e1a01002 	mov	r1, r2
    d95c:	e3833001 	orr	r3, r3, #1	; 0x1
    d960:	e5823004 	str	r3, [r2, #4]
    d964:	e5913004 	ldr	r3, [r1, #4]
    d968:	e59f222c 	ldr	r2, [pc, #556]	; db9c <sata_vs_reset+0x2e8>
    d96c:	e3130001 	tst	r3, #1	; 0x1
    d970:	1afffffb 	bne	d964 <sata_vs_reset+0xb0>
    d974:	e5923000 	ldr	r3, [r2]
    d978:	e3a00b03 	mov	r0, #3072	; 0xc00
    d97c:	e3833306 	orr	r3, r3, #402653184	; 0x18000000
    d980:	e5823000 	str	r3, [r2]
    d984:	e3a03001 	mov	r3, #1	; 0x1
    d988:	e582300c 	str	r3, [r2, #12]
    d98c:	e5923118 	ldr	r3, [r2, #280]
    d990:	e3833707 	orr	r3, r3, #1835008	; 0x1c0000
    d994:	e5823118 	str	r3, [r2, #280]
    d998:	ebffcf79 	bl	1784 <malloc>
    d99c:	e2505000 	subs	r5, r0, #0	; 0x0
    d9a0:	1a000004 	bne	d9b8 <sata_vs_reset+0x104>
    d9a4:	e59f01f4 	ldr	r0, [pc, #500]	; dba0 <sata_vs_reset+0x2ec>
    d9a8:	e59f31f4 	ldr	r3, [pc, #500]	; dba4 <sata_vs_reset+0x2f0>
    d9ac:	e1a0e00f 	mov	lr, pc
    d9b0:	e593f000 	ldr	pc, [r3]
    d9b4:	ea000041 	b	dac0 <sata_vs_reset+0x20c>
    d9b8:	e2853fff 	add	r3, r5, #1020	; 0x3fc
    d9bc:	e2833003 	add	r3, r3, #3	; 0x3
    d9c0:	e59f11d4 	ldr	r1, [pc, #468]	; db9c <sata_vs_reset+0x2e8>
    d9c4:	e3c30fff 	bic	r0, r3, #1020	; 0x3fc
    d9c8:	e3c00003 	bic	r0, r0, #3	; 0x3
    d9cc:	e5913000 	ldr	r3, [r1]
    d9d0:	e1500005 	cmp	r0, r5
    d9d4:	02800a01 	addeq	r0, r0, #4096	; 0x1000
    d9d8:	e3130302 	tst	r3, #134217728	; 0x8000000
    d9dc:	0a00002e 	beq	da9c <sata_vs_reset+0x1e8>
    d9e0:	e5912118 	ldr	r2, [r1, #280]
    d9e4:	e30c3011 	movw	r3, #49169	; 0xc011
    d9e8:	e0023003 	and	r3, r2, r3
    d9ec:	e3530000 	cmp	r3, #0	; 0x0
    d9f0:	0a00000e 	beq	da30 <sata_vs_reset+0x17c>
    d9f4:	e5913118 	ldr	r3, [r1, #280]
    d9f8:	e1a0c001 	mov	ip, r1
    d9fc:	e3c33001 	bic	r3, r3, #1	; 0x1
    da00:	e5813118 	str	r3, [r1, #280]
    da04:	e59c3118 	ldr	r3, [ip, #280]
    da08:	e59f218c 	ldr	r2, [pc, #396]	; db9c <sata_vs_reset+0x2e8>
    da0c:	e3130902 	tst	r3, #32768	; 0x8000
    da10:	1afffffb 	bne	da04 <sata_vs_reset+0x150>
    da14:	e5923118 	ldr	r3, [r2, #280]
    da18:	e1a01002 	mov	r1, r2
    da1c:	e3c33010 	bic	r3, r3, #16	; 0x10
    da20:	e5823118 	str	r3, [r2, #280]
    da24:	e5913118 	ldr	r3, [r1, #280]
    da28:	e3130901 	tst	r3, #16384	; 0x4000
    da2c:	1afffffc 	bne	da24 <sata_vs_reset+0x170>
    da30:	e59f2164 	ldr	r2, [pc, #356]	; db9c <sata_vs_reset+0x2e8>
    da34:	e280eb01 	add	lr, r0, #1024	; 0x400
    da38:	e592312c 	ldr	r3, [r2, #300]
    da3c:	e1a01002 	mov	r1, r2
    da40:	e3c3300f 	bic	r3, r3, #15	; 0xf
    da44:	e582312c 	str	r3, [r2, #300]
    da48:	e5820100 	str	r0, [r2, #256]
    da4c:	e582e108 	str	lr, [r2, #264]
    da50:	e5923118 	ldr	r3, [r2, #280]
    da54:	e3833010 	orr	r3, r3, #16	; 0x10
    da58:	e5823118 	str	r3, [r2, #280]
    da5c:	e5923118 	ldr	r3, [r2, #280]
    da60:	e3833002 	orr	r3, r3, #2	; 0x2
    da64:	e5823118 	str	r3, [r2, #280]
    da68:	e5913128 	ldr	r3, [r1, #296]
    da6c:	e59f2128 	ldr	r2, [pc, #296]	; db9c <sata_vs_reset+0x2e8>
    da70:	e203300f 	and	r3, r3, #15	; 0xf
    da74:	e3530003 	cmp	r3, #3	; 0x3
    da78:	13530001 	cmpne	r3, #1	; 0x1
    da7c:	1afffff9 	bne	da68 <sata_vs_reset+0x1b4>
    da80:	e3e03000 	mvn	r3, #0	; 0x0
    da84:	e5823130 	str	r3, [r2, #304]
    da88:	e5923120 	ldr	r3, [r2, #288]
    da8c:	e59f1108 	ldr	r1, [pc, #264]	; db9c <sata_vs_reset+0x2e8>
    da90:	e3130089 	tst	r3, #137	; 0x89
    da94:	1afffffb 	bne	da88 <sata_vs_reset+0x1d4>
    da98:	ea00000a 	b	dac8 <sata_vs_reset+0x214>
    da9c:	e59f4100 	ldr	r4, [pc, #256]	; dba4 <sata_vs_reset+0x2f0>
    daa0:	e59f0100 	ldr	r0, [pc, #256]	; dba8 <sata_vs_reset+0x2f4>
    daa4:	e1a0e00f 	mov	lr, pc
    daa8:	e594f000 	ldr	pc, [r4]
    daac:	e59f00f8 	ldr	r0, [pc, #248]	; dbac <sata_vs_reset+0x2f8>
    dab0:	e1a0e00f 	mov	lr, pc
    dab4:	e594f000 	ldr	pc, [r4]
    dab8:	e1a00005 	mov	r0, r5
    dabc:	ebffcf98 	bl	1924 <free>
    dac0:	e3e00000 	mvn	r0, #0	; 0x0
    dac4:	e8bd8070 	pop	{r4, r5, r6, pc}
    dac8:	e5912118 	ldr	r2, [r1, #280]
    dacc:	e30c3011 	movw	r3, #49169	; 0xc011
    dad0:	e0023003 	and	r3, r2, r3
    dad4:	e3530000 	cmp	r3, #0	; 0x0
    dad8:	0a00000e 	beq	db18 <sata_vs_reset+0x264>
    dadc:	e5913118 	ldr	r3, [r1, #280]
    dae0:	e1a0c001 	mov	ip, r1
    dae4:	e3c33001 	bic	r3, r3, #1	; 0x1
    dae8:	e5813118 	str	r3, [r1, #280]
    daec:	e59c3118 	ldr	r3, [ip, #280]
    daf0:	e59f20a4 	ldr	r2, [pc, #164]	; db9c <sata_vs_reset+0x2e8>
    daf4:	e3130902 	tst	r3, #32768	; 0x8000
    daf8:	1afffffb 	bne	daec <sata_vs_reset+0x238>
    dafc:	e5923118 	ldr	r3, [r2, #280]
    db00:	e1a01002 	mov	r1, r2
    db04:	e3c33010 	bic	r3, r3, #16	; 0x10
    db08:	e5823118 	str	r3, [r2, #280]
    db0c:	e5913118 	ldr	r3, [r1, #280]
    db10:	e3130901 	tst	r3, #16384	; 0x4000
    db14:	1afffffc 	bne	db0c <sata_vs_reset+0x258>
    db18:	e59f207c 	ldr	r2, [pc, #124]	; db9c <sata_vs_reset+0x2e8>
    db1c:	e3e01000 	mvn	r1, #0	; 0x0
    db20:	e5820100 	str	r0, [r2, #256]
    db24:	e582e108 	str	lr, [r2, #264]
    db28:	e5923118 	ldr	r3, [r2, #280]
    db2c:	e3833010 	orr	r3, r3, #16	; 0x10
    db30:	e5823118 	str	r3, [r2, #280]
    db34:	e3e03502 	mvn	r3, #8388608	; 0x800000
    db38:	e5821130 	str	r1, [r2, #304]
    db3c:	e5821110 	str	r1, [r2, #272]
    db40:	e5821008 	str	r1, [r2, #8]
    db44:	e5823114 	str	r3, [r2, #276]
    db48:	e5923004 	ldr	r3, [r2, #4]
    db4c:	e1a01002 	mov	r1, r2
    db50:	e3833002 	orr	r3, r3, #2	; 0x2
    db54:	e5823004 	str	r3, [r2, #4]
    db58:	e5913128 	ldr	r3, [r1, #296]
    db5c:	e5912120 	ldr	r2, [r1, #288]
    db60:	e203300f 	and	r3, r3, #15	; 0xf
    db64:	e3530003 	cmp	r3, #3	; 0x3
    db68:	1afffffa 	bne	db58 <sata_vs_reset+0x2a4>
    db6c:	e2124089 	ands	r4, r2, #137	; 0x89
    db70:	1afffff8 	bne	db58 <sata_vs_reset+0x2a4>
    db74:	e1a00005 	mov	r0, r5
    db78:	ebffcf69 	bl	1924 <free>
    db7c:	e59f002c 	ldr	r0, [pc, #44]	; dbb0 <sata_vs_reset+0x2fc>
    db80:	e59f301c 	ldr	r3, [pc, #28]	; dba4 <sata_vs_reset+0x2f0>
    db84:	e1a0e00f 	mov	lr, pc
    db88:	e593f000 	ldr	pc, [r3]
    db8c:	e1a00004 	mov	r0, r4
    db90:	e8bd8070 	pop	{r4, r5, r6, pc}
    db94:	21e3c800 	.word	0x21e3c800
    db98:	21e3c000 	.word	0x21e3c000
    db9c:	28200000 	.word	0x28200000
    dba0:	0001c0c5 	.word	0x0001c0c5
    dba4:	3c00d694 	.word	0x3c00d694
    dba8:	0001c0e6 	.word	0x0001c0e6
    dbac:	0001c109 	.word	0x0001c109
    dbb0:	0001c11f 	.word	0x0001c11f

0000dbb4 <init_led>:
    dbb4:	e3a0c000 	mov	ip, #0	; 0x0
    dbb8:	e59f3010 	ldr	r3, [pc, #16]	; dbd0 <init_led+0x1c>
    dbbc:	e59f0010 	ldr	r0, [pc, #16]	; dbd4 <init_led+0x20>
    dbc0:	e1a0200c 	mov	r2, ip
    dbc4:	e3a01002 	mov	r1, #2	; 0x2
    dbc8:	e583c000 	str	ip, [r3]
    dbcc:	eaffcc38 	b	cb4 <pads_chmod>
    dbd0:	3c00d868 	.word	0x3c00d868
    dbd4:	002e002d 	.word	0x002e002d

0000dbd8 <led_off>:
    dbd8:	e3a02000 	mov	r2, #0	; 0x0
    dbdc:	e59f300c 	ldr	r3, [pc, #12]	; dbf0 <led_off+0x18>
    dbe0:	e1a01002 	mov	r1, r2
    dbe4:	e3a0002e 	mov	r0, #46	; 0x2e
    dbe8:	e5832000 	str	r2, [r3]
    dbec:	eaffccc0 	b	ef4 <pads_setpin>
    dbf0:	3c00d868 	.word	0x3c00d868

0000dbf4 <led_on>:
    dbf4:	e3a02001 	mov	r2, #1	; 0x1
    dbf8:	e59f300c 	ldr	r3, [pc, #12]	; dc0c <led_on+0x18>
    dbfc:	e1a01002 	mov	r1, r2
    dc00:	e3a0002e 	mov	r0, #46	; 0x2e
    dc04:	e5832000 	str	r2, [r3]
    dc08:	eaffccb9 	b	ef4 <pads_setpin>
    dc0c:	3c00d868 	.word	0x3c00d868

0000dc10 <led_pulse>:
    dc10:	e92d4010 	push	{r4, lr}
    dc14:	e1a04000 	mov	r4, r0
    dc18:	ebfffff5 	bl	dbf4 <led_on>
    dc1c:	e3a00005 	mov	r0, #5	; 0x5
    dc20:	ea000001 	b	dc2c <led_pulse+0x1c>
    dc24:	ebfffff2 	bl	dbf4 <led_on>
    dc28:	e3a00002 	mov	r0, #2	; 0x2
    dc2c:	ebfff638 	bl	b514 <udelay>
    dc30:	ebffffe8 	bl	dbd8 <led_off>
    dc34:	e3a00002 	mov	r0, #2	; 0x2
    dc38:	ebfff635 	bl	b514 <udelay>
    dc3c:	e3540000 	cmp	r4, #0	; 0x0
    dc40:	e2444001 	sub	r4, r4, #1	; 0x1
    dc44:	cafffff6 	bgt	dc24 <led_pulse+0x14>
    dc48:	e8bd8010 	pop	{r4, pc}

0000dc4c <led_ctrl>:
    dc4c:	e3500000 	cmp	r0, #0	; 0x0
    dc50:	0a000000 	beq	dc58 <led_ctrl+0xc>
    dc54:	eaffffe6 	b	dbf4 <led_on>
    dc58:	eaffffde 	b	dbd8 <led_off>

0000dc5c <led_toggle>:
    dc5c:	e59f300c 	ldr	r3, [pc, #12]	; dc70 <led_toggle+0x14>
    dc60:	e5930000 	ldr	r0, [r3]
    dc64:	e2700001 	rsbs	r0, r0, #1	; 0x1
    dc68:	33a00000 	movcc	r0, #0	; 0x0
    dc6c:	eafffff6 	b	dc4c <led_ctrl>
    dc70:	3c00d868 	.word	0x3c00d868

0000dc74 <udc_get_cable_state>:
    dc74:	e3a00001 	mov	r0, #1	; 0x1
    dc78:	e12fff1e 	bx	lr

0000dc7c <udc_disconnect>:
    dc7c:	e59f103c 	ldr	r1, [pc, #60]	; dcc0 <udc_disconnect+0x44>
    dc80:	e59f303c 	ldr	r3, [pc, #60]	; dcc4 <udc_disconnect+0x48>
    dc84:	e5912010 	ldr	r2, [r1, #16]
    dc88:	e3a00000 	mov	r0, #0	; 0x0
    dc8c:	e3822001 	orr	r2, r2, #1	; 0x1
    dc90:	e92d4010 	push	{r4, lr}
    dc94:	e5c30000 	strb	r0, [r3]
    dc98:	e1a04001 	mov	r4, r1
    dc9c:	e5812010 	str	r2, [r1, #16]
    dca0:	ea000000 	b	dca8 <udc_disconnect+0x2c>
    dca4:	ebfff61a 	bl	b514 <udelay>
    dca8:	e5943010 	ldr	r3, [r4, #16]
    dcac:	e3a0000a 	mov	r0, #10	; 0xa
    dcb0:	e2133001 	ands	r3, r3, #1	; 0x1
    dcb4:	1afffffa 	bne	dca4 <udc_disconnect+0x28>
    dcb8:	e1a00003 	mov	r0, r3
    dcbc:	e8bd8010 	pop	{r4, pc}
    dcc0:	27100000 	.word	0x27100000
    dcc4:	3c00d1ec 	.word	0x3c00d1ec

0000dcc8 <udc_event_handle>:
    dcc8:	e59f2768 	ldr	r2, [pc, #1896]	; e438 <udc_event_handle+0x770>
    dccc:	e59f3768 	ldr	r3, [pc, #1896]	; e43c <udc_event_handle+0x774>
    dcd0:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    dcd4:	e5934014 	ldr	r4, [r3, #20]
    dcd8:	e5923000 	ldr	r3, [r2]
    dcdc:	e59f0758 	ldr	r0, [pc, #1880]	; e43c <udc_event_handle+0x774>
    dce0:	e1530004 	cmp	r3, r4
    dce4:	e204ca03 	and	ip, r4, #12288	; 0x3000
    dce8:	15824000 	strne	r4, [r2]
    dcec:	e2043b03 	and	r3, r4, #3072	; 0xc00
    dcf0:	e35c0a01 	cmp	ip, #4096	; 0x1000
    dcf4:	e24dd008 	sub	sp, sp, #8	; 0x8
    dcf8:	e5803014 	str	r3, [r0, #20]
    dcfc:	0a000002 	beq	dd0c <udc_event_handle+0x44>
    dd00:	e35c0a02 	cmp	ip, #8192	; 0x2000
    dd04:	1a000034 	bne	dddc <udc_event_handle+0x114>
    dd08:	ea000025 	b	dda4 <udc_event_handle+0xdc>
    dd0c:	e3a03302 	mov	r3, #134217728	; 0x8000000
    dd10:	e5803b00 	str	r3, [r0, #2816]
    dd14:	e5803b40 	str	r3, [r0, #2880]
    dd18:	e59f3720 	ldr	r3, [pc, #1824]	; e440 <udc_event_handle+0x778>
    dd1c:	e3a02009 	mov	r2, #9	; 0x9
    dd20:	e580381c 	str	r3, [r0, #2076]
    dd24:	e3a03b02 	mov	r3, #2048	; 0x800
    dd28:	e5802814 	str	r2, [r0, #2068]
    dd2c:	e5802810 	str	r2, [r0, #2064]
    dd30:	e5803024 	str	r3, [r0, #36]
    dd34:	e2833301 	add	r3, r3, #67108864	; 0x4000000
    dd38:	e5803028 	str	r3, [r0, #40]
    dd3c:	e2833b01 	add	r3, r3, #1024	; 0x400
    dd40:	e5803104 	str	r3, [r0, #260]
    dd44:	e59f36f8 	ldr	r3, [pc, #1784]	; e444 <udc_event_handle+0x77c>
    dd48:	e1a05000 	mov	r5, r0
    dd4c:	e580305c 	str	r3, [r0, #92]
    dd50:	e5903010 	ldr	r3, [r0, #16]
    dd54:	e3c33d1f 	bic	r3, r3, #1984	; 0x7c0
    dd58:	e3833e43 	orr	r3, r3, #1072	; 0x430
    dd5c:	e5803010 	str	r3, [r0, #16]
    dd60:	ea000000 	b	dd68 <udc_event_handle+0xa0>
    dd64:	ebfff5ea 	bl	b514 <udelay>
    dd68:	e5953010 	ldr	r3, [r5, #16]
    dd6c:	e3a00001 	mov	r0, #1	; 0x1
    dd70:	e3130030 	tst	r3, #48	; 0x30
    dd74:	e59f16c0 	ldr	r1, [pc, #1728]	; e43c <udc_event_handle+0x774>
    dd78:	1afffff9 	bne	dd64 <udc_event_handle+0x9c>
    dd7c:	e59f36c4 	ldr	r3, [pc, #1732]	; e448 <udc_event_handle+0x780>
    dd80:	e2800009 	add	r0, r0, #9	; 0x9
    dd84:	e5932024 	ldr	r2, [r3, #36]
    dd88:	e59f36bc 	ldr	r3, [pc, #1724]	; e44c <udc_event_handle+0x784>
    dd8c:	e5813b10 	str	r3, [r1, #2832]
    dd90:	e3a03a01 	mov	r3, #4096	; 0x1000
    dd94:	e5812b14 	str	r2, [r1, #2836]
    dd98:	e5813014 	str	r3, [r1, #20]
    dd9c:	ebfff5dc 	bl	b514 <udelay>
    dda0:	ea00000d 	b	dddc <udc_event_handle+0x114>
    dda4:	e5902808 	ldr	r2, [r0, #2056]
    dda8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    ddac:	e5803b00 	str	r3, [r0, #2816]
    ddb0:	e1a020a2 	lsr	r2, r2, #1
    ddb4:	e59f368c 	ldr	r3, [pc, #1676]	; e448 <udc_event_handle+0x780>
    ddb8:	e5901018 	ldr	r1, [r0, #24]
    ddbc:	e2022003 	and	r2, r2, #3	; 0x3
    ddc0:	e5c3201c 	strb	r2, [r3, #28]
    ddc4:	e59f3684 	ldr	r3, [pc, #1668]	; e450 <udc_event_handle+0x788>
    ddc8:	e3811008 	orr	r1, r1, #8	; 0x8
    ddcc:	e3a02001 	mov	r2, #1	; 0x1
    ddd0:	e5801018 	str	r1, [r0, #24]
    ddd4:	e5832000 	str	r2, [r3]
    ddd8:	e580c014 	str	ip, [r0, #20]
    dddc:	e59f3658 	ldr	r3, [pc, #1624]	; e43c <udc_event_handle+0x774>
    dde0:	e3140701 	tst	r4, #262144	; 0x40000
    dde4:	e5939818 	ldr	r9, [r3, #2072]
    dde8:	0a000051 	beq	df34 <udc_event_handle+0x26c>
    ddec:	e3190001 	tst	r9, #1	; 0x1
    ddf0:	0a000042 	beq	df00 <udc_event_handle+0x238>
    ddf4:	e5935908 	ldr	r5, [r3, #2312]
    ddf8:	e3150001 	tst	r5, #1	; 0x1
    ddfc:	0a00003b 	beq	def0 <udc_event_handle+0x228>
    de00:	e59f3640 	ldr	r3, [pc, #1600]	; e448 <udc_event_handle+0x780>
    de04:	e5933020 	ldr	r3, [r3, #32]
    de08:	e2433002 	sub	r3, r3, #2	; 0x2
    de0c:	e3530003 	cmp	r3, #3	; 0x3
    de10:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    de14:	ea000035 	b	def0 <udc_event_handle+0x228>
    de18:	0000de58 	.word	0x0000de58
    de1c:	0000de28 	.word	0x0000de28
    de20:	0000de8c 	.word	0x0000de8c
    de24:	0000de8c 	.word	0x0000de8c
    de28:	e59f160c 	ldr	r1, [pc, #1548]	; e43c <udc_event_handle+0x774>
    de2c:	e59f0614 	ldr	r0, [pc, #1556]	; e448 <udc_event_handle+0x780>
    de30:	e5912910 	ldr	r2, [r1, #2320]
    de34:	e590c00c 	ldr	ip, [r0, #12]
    de38:	e3a03004 	mov	r3, #4	; 0x4
    de3c:	e3822702 	orr	r2, r2, #524288	; 0x80000
    de40:	e5803020 	str	r3, [r0, #32]
    de44:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    de48:	e5812910 	str	r2, [r1, #2320]
    de4c:	e581c914 	str	ip, [r1, #2324]
    de50:	e5813900 	str	r3, [r1, #2304]
    de54:	ea000025 	b	def0 <udc_event_handle+0x228>
    de58:	e59f15dc 	ldr	r1, [pc, #1500]	; e43c <udc_event_handle+0x774>
    de5c:	e59f05e4 	ldr	r0, [pc, #1508]	; e448 <udc_event_handle+0x780>
    de60:	e5913b10 	ldr	r3, [r1, #2832]
    de64:	e590c024 	ldr	ip, [r0, #36]
    de68:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    de6c:	e3833702 	orr	r3, r3, #524288	; 0x80000
    de70:	e5813b10 	str	r3, [r1, #2832]
    de74:	e3a02005 	mov	r2, #5	; 0x5
    de78:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    de7c:	e581cb14 	str	ip, [r1, #2836]
    de80:	e5802020 	str	r2, [r0, #32]
    de84:	e5813b00 	str	r3, [r1, #2816]
    de88:	ea000018 	b	def0 <udc_event_handle+0x228>
    de8c:	e59f05b4 	ldr	r0, [pc, #1460]	; e448 <udc_event_handle+0x780>
    de90:	e5d03015 	ldrb	r3, [r0, #21]
    de94:	e3530009 	cmp	r3, #9	; 0x9
    de98:	1a00000a 	bne	dec8 <udc_event_handle+0x200>
    de9c:	e59f1598 	ldr	r1, [pc, #1432]	; e43c <udc_event_handle+0x774>
    dea0:	e59f35ac 	ldr	r3, [pc, #1452]	; e454 <udc_event_handle+0x78c>
    dea4:	e3a02001 	mov	r2, #1	; 0x1
    dea8:	e5813920 	str	r3, [r1, #2336]
    deac:	e2433d07 	sub	r3, r3, #448	; 0x1c0
    deb0:	e5813b40 	str	r3, [r1, #2880]
    deb4:	e591381c 	ldr	r3, [r1, #2076]
    deb8:	e5c02000 	strb	r2, [r0]
    debc:	e3833701 	orr	r3, r3, #262144	; 0x40000
    dec0:	e3833002 	orr	r3, r3, #2	; 0x2
    dec4:	e581381c 	str	r3, [r1, #2076]
    dec8:	e59f256c 	ldr	r2, [pc, #1388]	; e43c <udc_event_handle+0x774>
    decc:	e59f1574 	ldr	r1, [pc, #1396]	; e448 <udc_event_handle+0x780>
    ded0:	e59f3574 	ldr	r3, [pc, #1396]	; e44c <udc_event_handle+0x784>
    ded4:	e5910024 	ldr	r0, [r1, #36]
    ded8:	e5823b10 	str	r3, [r2, #2832]
    dedc:	e3a03001 	mov	r3, #1	; 0x1
    dee0:	e5813020 	str	r3, [r1, #32]
    dee4:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    dee8:	e5820b14 	str	r0, [r2, #2836]
    deec:	e5823b00 	str	r3, [r2, #2816]
    def0:	e300320d 	movw	r3, #525	; 0x20d
    def4:	e59f2540 	ldr	r2, [pc, #1344]	; e43c <udc_event_handle+0x774>
    def8:	e0053003 	and	r3, r5, r3
    defc:	e5823908 	str	r3, [r2, #2312]
    df00:	e3190002 	tst	r9, #2	; 0x2
    df04:	0a00000a 	beq	df34 <udc_event_handle+0x26c>
    df08:	e59f352c 	ldr	r3, [pc, #1324]	; e43c <udc_event_handle+0x774>
    df0c:	e5932928 	ldr	r2, [r3, #2344]
    df10:	e3120001 	tst	r2, #1	; 0x1
    df14:	0a000003 	beq	df28 <udc_event_handle+0x260>
    df18:	e5933930 	ldr	r3, [r3, #2352]
    df1c:	e3d3120f 	bics	r1, r3, #-268435456	; 0xf0000000
    df20:	059f3520 	ldreq	r3, [pc, #1312]	; e448 <udc_event_handle+0x780>
    df24:	05c31002 	strbeq	r1, [r3, #2]
    df28:	e59f350c 	ldr	r3, [pc, #1292]	; e43c <udc_event_handle+0x774>
    df2c:	e2022005 	and	r2, r2, #5	; 0x5
    df30:	e5832928 	str	r2, [r3, #2344]
    df34:	e3140702 	tst	r4, #524288	; 0x80000
    df38:	0a00013c 	beq	e430 <udc_event_handle+0x768>
    df3c:	e3190801 	tst	r9, #65536	; 0x10000
    df40:	0a000124 	beq	e3d8 <udc_event_handle+0x710>
    df44:	e59f34f0 	ldr	r3, [pc, #1264]	; e43c <udc_event_handle+0x774>
    df48:	e593ab08 	ldr	sl, [r3, #2824]
    df4c:	e31a0001 	tst	sl, #1	; 0x1
    df50:	0a00002b 	beq	e004 <udc_event_handle+0x33c>
    df54:	e59f34ec 	ldr	r3, [pc, #1260]	; e448 <udc_event_handle+0x780>
    df58:	e5933020 	ldr	r3, [r3, #32]
    df5c:	e2433002 	sub	r3, r3, #2	; 0x2
    df60:	e3530003 	cmp	r3, #3	; 0x3
    df64:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    df68:	ea000025 	b	e004 <udc_event_handle+0x33c>
    df6c:	0000dfa8 	.word	0x0000dfa8
    df70:	0000df7c 	.word	0x0000df7c
    df74:	0000dfdc 	.word	0x0000dfdc
    df78:	0000dfdc 	.word	0x0000dfdc
    df7c:	e59f14c4 	ldr	r1, [pc, #1220]	; e448 <udc_event_handle+0x780>
    df80:	e59f24b4 	ldr	r2, [pc, #1204]	; e43c <udc_event_handle+0x774>
    df84:	e3a03004 	mov	r3, #4	; 0x4
    df88:	e591000c 	ldr	r0, [r1, #12]
    df8c:	e5813020 	str	r3, [r1, #32]
    df90:	e3a03702 	mov	r3, #524288	; 0x80000
    df94:	e5823910 	str	r3, [r2, #2320]
    df98:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    df9c:	e5820914 	str	r0, [r2, #2324]
    dfa0:	e5823900 	str	r3, [r2, #2304]
    dfa4:	ea000016 	b	e004 <udc_event_handle+0x33c>
    dfa8:	e59f148c 	ldr	r1, [pc, #1164]	; e43c <udc_event_handle+0x774>
    dfac:	e59f0494 	ldr	r0, [pc, #1172]	; e448 <udc_event_handle+0x780>
    dfb0:	e5913b10 	ldr	r3, [r1, #2832]
    dfb4:	e590c00c 	ldr	ip, [r0, #12]
    dfb8:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    dfbc:	e3833702 	orr	r3, r3, #524288	; 0x80000
    dfc0:	e5813b10 	str	r3, [r1, #2832]
    dfc4:	e3a02005 	mov	r2, #5	; 0x5
    dfc8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    dfcc:	e581cb14 	str	ip, [r1, #2836]
    dfd0:	e5802020 	str	r2, [r0, #32]
    dfd4:	e5813b00 	str	r3, [r1, #2816]
    dfd8:	ea000009 	b	e004 <udc_event_handle+0x33c>
    dfdc:	e59f2458 	ldr	r2, [pc, #1112]	; e43c <udc_event_handle+0x774>
    dfe0:	e59f1460 	ldr	r1, [pc, #1120]	; e448 <udc_event_handle+0x780>
    dfe4:	e59f3460 	ldr	r3, [pc, #1120]	; e44c <udc_event_handle+0x784>
    dfe8:	e5910024 	ldr	r0, [r1, #36]
    dfec:	e5823b10 	str	r3, [r2, #2832]
    dff0:	e3a03001 	mov	r3, #1	; 0x1
    dff4:	e5813020 	str	r3, [r1, #32]
    dff8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    dffc:	e5820b14 	str	r0, [r2, #2836]
    e000:	e5823b00 	str	r3, [r2, #2816]
    e004:	e31a0004 	tst	sl, #4	; 0x4
    e008:	159f342c 	ldrne	r3, [pc, #1068]	; e43c <udc_event_handle+0x774>
    e00c:	13a02004 	movne	r2, #4	; 0x4
    e010:	15832b08 	strne	r2, [r3, #2824]
    e014:	e31a0008 	tst	sl, #8	; 0x8
    e018:	0a0000ea 	beq	e3c8 <udc_event_handle+0x700>
    e01c:	e31a0040 	tst	sl, #64	; 0x40
    e020:	e59f1430 	ldr	r1, [pc, #1072]	; e458 <udc_event_handle+0x790>
    e024:	0a000006 	beq	e044 <udc_event_handle+0x37c>
    e028:	e59f340c 	ldr	r3, [pc, #1036]	; e43c <udc_event_handle+0x774>
    e02c:	e5933b14 	ldr	r3, [r3, #2836]
    e030:	e2432004 	sub	r2, r3, #4	; 0x4
    e034:	e2433008 	sub	r3, r3, #8	; 0x8
    e038:	e5812004 	str	r2, [r1, #4]
    e03c:	e5813000 	str	r3, [r1]
    e040:	ea000005 	b	e05c <udc_event_handle+0x394>
    e044:	e59f33fc 	ldr	r3, [pc, #1020]	; e448 <udc_event_handle+0x780>
    e048:	e5932024 	ldr	r2, [r3, #36]
    e04c:	e5923000 	ldr	r3, [r2]
    e050:	e5813000 	str	r3, [r1]
    e054:	e5923004 	ldr	r3, [r2, #4]
    e058:	e5813004 	str	r3, [r1, #4]
    e05c:	e59f23e4 	ldr	r2, [pc, #996]	; e448 <udc_event_handle+0x780>
    e060:	e5d20014 	ldrb	r0, [r2, #20]
    e064:	e3100080 	tst	r0, #128	; 0x80
    e068:	13a03002 	movne	r3, #2	; 0x2
    e06c:	03a03003 	moveq	r3, #3	; 0x3
    e070:	e5823020 	str	r3, [r2, #32]
    e074:	e59f23cc 	ldr	r2, [pc, #972]	; e448 <udc_event_handle+0x780>
    e078:	e1d211ba 	ldrh	r1, [r2, #26]
    e07c:	e3510000 	cmp	r1, #0	; 0x0
    e080:	03a03004 	moveq	r3, #4	; 0x4
    e084:	05823020 	streq	r3, [r2, #32]
    e088:	e59f33b8 	ldr	r3, [pc, #952]	; e448 <udc_event_handle+0x780>
    e08c:	e5d33015 	ldrb	r3, [r3, #21]
    e090:	e3530009 	cmp	r3, #9	; 0x9
    e094:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    e098:	ea0000ca 	b	e3c8 <udc_event_handle+0x700>
    e09c:	0000e360 	.word	0x0000e360
    e0a0:	0000e3c8 	.word	0x0000e3c8
    e0a4:	0000e3c8 	.word	0x0000e3c8
    e0a8:	0000e3c8 	.word	0x0000e3c8
    e0ac:	0000e3c8 	.word	0x0000e3c8
    e0b0:	0000e2ac 	.word	0x0000e2ac
    e0b4:	0000e0c4 	.word	0x0000e0c4
    e0b8:	0000e3c8 	.word	0x0000e3c8
    e0bc:	0000e3c8 	.word	0x0000e3c8
    e0c0:	0000e2fc 	.word	0x0000e2fc
    e0c4:	e3a00040 	mov	r0, #64	; 0x40
    e0c8:	ebffcdad 	bl	1784 <malloc>
    e0cc:	e2506000 	subs	r6, r0, #0	; 0x0
    e0d0:	0a0000bc 	beq	e3c8 <udc_event_handle+0x700>
    e0d4:	e59f536c 	ldr	r5, [pc, #876]	; e448 <udc_event_handle+0x780>
    e0d8:	e1d521b6 	ldrh	r2, [r5, #22]
    e0dc:	e1a03422 	lsr	r3, r2, #8
    e0e0:	e2433001 	sub	r3, r3, #1	; 0x1
    e0e4:	e3530005 	cmp	r3, #5	; 0x5
    e0e8:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    e0ec:	ea000054 	b	e244 <udc_event_handle+0x57c>
    e0f0:	0000e108 	.word	0x0000e108
    e0f4:	0000e120 	.word	0x0000e120
    e0f8:	0000e178 	.word	0x0000e178
    e0fc:	0000e244 	.word	0x0000e244
    e100:	0000e244 	.word	0x0000e244
    e104:	0000e14c 	.word	0x0000e14c
    e108:	e1a00006 	mov	r0, r6
    e10c:	e59f1348 	ldr	r1, [pc, #840]	; e45c <udc_event_handle+0x794>
    e110:	e3a02012 	mov	r2, #18	; 0x12
    e114:	ebffcd35 	bl	15f0 <memcpy>
    e118:	e3a03012 	mov	r3, #18	; 0x12
    e11c:	ea000028 	b	e1c4 <udc_event_handle+0x4fc>
    e120:	e59f3338 	ldr	r3, [pc, #824]	; e460 <udc_event_handle+0x798>
    e124:	e3a04020 	mov	r4, #32	; 0x20
    e128:	e3a0c000 	mov	ip, #0	; 0x0
    e12c:	e1a01003 	mov	r1, r3
    e130:	e1a00006 	mov	r0, r6
    e134:	e1a02004 	mov	r2, r4
    e138:	e5c3c003 	strb	ip, [r3, #3]
    e13c:	e5c34002 	strb	r4, [r3, #2]
    e140:	ebffcd2a 	bl	15f0 <memcpy>
    e144:	e5854004 	str	r4, [r5, #4]
    e148:	ea000045 	b	e264 <udc_event_handle+0x59c>
    e14c:	e1d531ba 	ldrh	r3, [r5, #26]
    e150:	e59fc30c 	ldr	ip, [pc, #780]	; e464 <udc_event_handle+0x79c>
    e154:	e353000a 	cmp	r3, #10	; 0xa
    e158:	23a0300a 	movcs	r3, #10	; 0xa
    e15c:	e5cc3000 	strb	r3, [ip]
    e160:	e1a0100c 	mov	r1, ip
    e164:	e1a00006 	mov	r0, r6
    e168:	e3a0200a 	mov	r2, #10	; 0xa
    e16c:	ebffcd1f 	bl	15f0 <memcpy>
    e170:	e3a0300a 	mov	r3, #10	; 0xa
    e174:	ea000012 	b	e1c4 <udc_event_handle+0x4fc>
    e178:	e21270ff 	ands	r7, r2, #255	; 0xff
    e17c:	13a03000 	movne	r3, #0	; 0x0
    e180:	03a03001 	moveq	r3, #1	; 0x1
    e184:	e3570005 	cmp	r7, #5	; 0x5
    e188:	d1a08003 	movle	r8, r3
    e18c:	c3838001 	orrgt	r8, r3, #1	; 0x1
    e190:	e3580000 	cmp	r8, #0	; 0x0
    e194:	e59f52ac 	ldr	r5, [pc, #684]	; e448 <udc_event_handle+0x780>
    e198:	0a00000b 	beq	e1cc <udc_event_handle+0x504>
    e19c:	e1d531ba 	ldrh	r3, [r5, #26]
    e1a0:	e59fc2c0 	ldr	ip, [pc, #704]	; e468 <udc_event_handle+0x7a0>
    e1a4:	e3530004 	cmp	r3, #4	; 0x4
    e1a8:	23a03004 	movcs	r3, #4	; 0x4
    e1ac:	e5cc3000 	strb	r3, [ip]
    e1b0:	e1a0100c 	mov	r1, ip
    e1b4:	e1a00006 	mov	r0, r6
    e1b8:	e3a02004 	mov	r2, #4	; 0x4
    e1bc:	ebffcd0b 	bl	15f0 <memcpy>
    e1c0:	e3a03004 	mov	r3, #4	; 0x4
    e1c4:	e5853004 	str	r3, [r5, #4]
    e1c8:	ea000025 	b	e264 <udc_event_handle+0x59c>
    e1cc:	e59f4298 	ldr	r4, [pc, #664]	; e46c <udc_event_handle+0x7a4>
    e1d0:	e7940107 	ldr	r0, [r4, r7, lsl #2]
    e1d4:	ebffcceb 	bl	1588 <strlen>
    e1d8:	e6ef0070 	uxtb	r0, r0
    e1dc:	e2803001 	add	r3, r0, #1	; 0x1
    e1e0:	e5d5201a 	ldrb	r2, [r5, #26]
    e1e4:	e1a03083 	lsl	r3, r3, #1
    e1e8:	e6ef3073 	uxtb	r3, r3
    e1ec:	e1530002 	cmp	r3, r2
    e1f0:	31a0e003 	movcc	lr, r3
    e1f4:	21a0e002 	movcs	lr, r2
    e1f8:	e3a03003 	mov	r3, #3	; 0x3
    e1fc:	e5c63001 	strb	r3, [r6, #1]
    e200:	e5c6e000 	strb	lr, [r6]
    e204:	e1a05008 	mov	r5, r8
    e208:	e3a02001 	mov	r2, #1	; 0x1
    e20c:	ea000005 	b	e228 <udc_event_handle+0x560>
    e210:	e7943107 	ldr	r3, [r4, r7, lsl #2]
    e214:	e0833002 	add	r3, r3, r2
    e218:	e5533001 	ldrb	r3, [r3, #-1]
    e21c:	e7c63082 	strb	r3, [r6, r2, lsl #1]
    e220:	e5c15001 	strb	r5, [r1, #1]
    e224:	e6ef207c 	uxtb	r2, ip
    e228:	e1500002 	cmp	r0, r2
    e22c:	e282c001 	add	ip, r2, #1	; 0x1
    e230:	e0861082 	add	r1, r6, r2, lsl #1
    e234:	aafffff5 	bge	e210 <udc_event_handle+0x548>
    e238:	e59f3208 	ldr	r3, [pc, #520]	; e448 <udc_event_handle+0x780>
    e23c:	e583e004 	str	lr, [r3, #4]
    e240:	ea000007 	b	e264 <udc_event_handle+0x59c>
    e244:	e59f21f0 	ldr	r2, [pc, #496]	; e43c <udc_event_handle+0x774>
    e248:	e3a03702 	mov	r3, #524288	; 0x80000
    e24c:	e5951024 	ldr	r1, [r5, #36]
    e250:	e5823910 	str	r3, [r2, #2320]
    e254:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    e258:	e5821914 	str	r1, [r2, #2324]
    e25c:	e5823900 	str	r3, [r2, #2304]
    e260:	ea00000e 	b	e2a0 <udc_event_handle+0x5d8>
    e264:	e59f11dc 	ldr	r1, [pc, #476]	; e448 <udc_event_handle+0x780>
    e268:	e59f01cc 	ldr	r0, [pc, #460]	; e43c <udc_event_handle+0x774>
    e26c:	e1d120b4 	ldrh	r2, [r1, #4]
    e270:	e1d131ba 	ldrh	r3, [r1, #26]
    e274:	e1520003 	cmp	r2, r3
    e278:	21a02003 	movcs	r2, r3
    e27c:	e3823702 	orr	r3, r2, #524288	; 0x80000
    e280:	e5803910 	str	r3, [r0, #2320]
    e284:	e3a03000 	mov	r3, #0	; 0x0
    e288:	e5c13001 	strb	r3, [r1, #1]
    e28c:	e2833321 	add	r3, r3, #-2080374784	; 0x84000000
    e290:	e5812004 	str	r2, [r1, #4]
    e294:	e5806914 	str	r6, [r0, #2324]
    e298:	e5803900 	str	r3, [r0, #2304]
    e29c:	e581600c 	str	r6, [r1, #12]
    e2a0:	e1a00006 	mov	r0, r6
    e2a4:	ebffcd9e 	bl	1924 <free>
    e2a8:	ea000046 	b	e3c8 <udc_event_handle+0x700>
    e2ac:	e59fc1bc 	ldr	ip, [pc, #444]	; e470 <udc_event_handle+0x7a8>
    e2b0:	e5dc3000 	ldrb	r3, [ip]
    e2b4:	e35300ff 	cmp	r3, #255	; 0xff
    e2b8:	1a000042 	bne	e3c8 <udc_event_handle+0x700>
    e2bc:	e59f1178 	ldr	r1, [pc, #376]	; e43c <udc_event_handle+0x774>
    e2c0:	e59f0180 	ldr	r0, [pc, #384]	; e448 <udc_event_handle+0x780>
    e2c4:	e5913800 	ldr	r3, [r1, #2048]
    e2c8:	e1d021b6 	ldrh	r2, [r0, #22]
    e2cc:	e3c33e7f 	bic	r3, r3, #2032	; 0x7f0
    e2d0:	e202207f 	and	r2, r2, #127	; 0x7f
    e2d4:	e1833202 	orr	r3, r3, r2, lsl #4
    e2d8:	e5813800 	str	r3, [r1, #2048]
    e2dc:	e5900024 	ldr	r0, [r0, #36]
    e2e0:	e3a03702 	mov	r3, #524288	; 0x80000
    e2e4:	e5cc2000 	strb	r2, [ip]
    e2e8:	e5813910 	str	r3, [r1, #2320]
    e2ec:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    e2f0:	e5810914 	str	r0, [r1, #2324]
    e2f4:	e5813900 	str	r3, [r1, #2304]
    e2f8:	ea000032 	b	e3c8 <udc_event_handle+0x700>
    e2fc:	e59f016c 	ldr	r0, [pc, #364]	; e470 <udc_event_handle+0x7a8>
    e300:	e5d03000 	ldrb	r3, [r0]
    e304:	e35300ff 	cmp	r3, #255	; 0xff
    e308:	0a00002e 	beq	e3c8 <udc_event_handle+0x700>
    e30c:	e59f1134 	ldr	r1, [pc, #308]	; e448 <udc_event_handle+0x780>
    e310:	e1d131b6 	ldrh	r3, [r1, #22]
    e314:	e3530000 	cmp	r3, #0	; 0x0
    e318:	03e03000 	mvneq	r3, #0	; 0x0
    e31c:	05911024 	ldreq	r1, [r1, #36]
    e320:	059f2114 	ldreq	r2, [pc, #276]	; e43c <udc_event_handle+0x774>
    e324:	05c03000 	strbeq	r3, [r0]
    e328:	0a000013 	beq	e37c <udc_event_handle+0x6b4>
    e32c:	e3530001 	cmp	r3, #1	; 0x1
    e330:	1a000024 	bne	e3c8 <udc_event_handle+0x700>
    e334:	e59f2100 	ldr	r2, [pc, #256]	; e43c <udc_event_handle+0x774>
    e338:	e59f3134 	ldr	r3, [pc, #308]	; e474 <udc_event_handle+0x7ac>
    e33c:	e5911024 	ldr	r1, [r1, #36]
    e340:	e5823920 	str	r3, [r2, #2336]
    e344:	e2833d07 	add	r3, r3, #448	; 0x1c0
    e348:	e5823b40 	str	r3, [r2, #2880]
    e34c:	e592381c 	ldr	r3, [r2, #2076]
    e350:	e3833701 	orr	r3, r3, #262144	; 0x40000
    e354:	e3833002 	orr	r3, r3, #2	; 0x2
    e358:	e582381c 	str	r3, [r2, #2076]
    e35c:	ea000006 	b	e37c <udc_event_handle+0x6b4>
    e360:	e210c01f 	ands	ip, r0, #31	; 0x1f
    e364:	1a000017 	bne	e3c8 <udc_event_handle+0x700>
    e368:	e3510000 	cmp	r1, #0	; 0x0
    e36c:	e59f50d4 	ldr	r5, [pc, #212]	; e448 <udc_event_handle+0x780>
    e370:	1a000004 	bne	e388 <udc_event_handle+0x6c0>
    e374:	e5951024 	ldr	r1, [r5, #36]
    e378:	e59f20bc 	ldr	r2, [pc, #188]	; e43c <udc_event_handle+0x774>
    e37c:	e3a03702 	mov	r3, #524288	; 0x80000
    e380:	e5823910 	str	r3, [r2, #2320]
    e384:	ea00000c 	b	e3bc <udc_event_handle+0x6f4>
    e388:	e1d531b6 	ldrh	r3, [r5, #22]
    e38c:	e59f20a8 	ldr	r2, [pc, #168]	; e43c <udc_event_handle+0x774>
    e390:	e3530002 	cmp	r3, #2	; 0x2
    e394:	23a03002 	movcs	r3, #2	; 0x2
    e398:	e6ef3073 	uxtb	r3, r3
    e39c:	e1530001 	cmp	r3, r1
    e3a0:	21a03001 	movcs	r3, r1
    e3a4:	e3830702 	orr	r0, r3, #524288	; 0x80000
    e3a8:	e28d1006 	add	r1, sp, #6	; 0x6
    e3ac:	e5c5c001 	strb	ip, [r5, #1]
    e3b0:	e5853004 	str	r3, [r5, #4]
    e3b4:	e585100c 	str	r1, [r5, #12]
    e3b8:	e5820910 	str	r0, [r2, #2320]
    e3bc:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    e3c0:	e5821914 	str	r1, [r2, #2324]
    e3c4:	e5823900 	str	r3, [r2, #2304]
    e3c8:	e3003209 	movw	r3, #521	; 0x209
    e3cc:	e59f2068 	ldr	r2, [pc, #104]	; e43c <udc_event_handle+0x774>
    e3d0:	e00a3003 	and	r3, sl, r3
    e3d4:	e5823b08 	str	r3, [r2, #2824]
    e3d8:	e3190701 	tst	r9, #262144	; 0x40000
    e3dc:	0a000013 	beq	e430 <udc_event_handle+0x768>
    e3e0:	e59f1054 	ldr	r1, [pc, #84]	; e43c <udc_event_handle+0x774>
    e3e4:	e5912b48 	ldr	r2, [r1, #2888]
    e3e8:	e3120001 	tst	r2, #1	; 0x1
    e3ec:	0a00000c 	beq	e424 <udc_event_handle+0x75c>
    e3f0:	e5913b50 	ldr	r3, [r1, #2896]
    e3f4:	e59f504c 	ldr	r5, [pc, #76]	; e448 <udc_event_handle+0x780>
    e3f8:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
    e3fc:	e3c3373e 	bic	r3, r3, #16252928	; 0xf80000
    e400:	e3530c02 	cmp	r3, #512	; 0x200
    e404:	05953028 	ldreq	r3, [r5, #40]
    e408:	13a03000 	movne	r3, #0	; 0x0
    e40c:	03833702 	orreq	r3, r3, #524288	; 0x80000
    e410:	05813b50 	streq	r3, [r1, #2896]
    e414:	05913b40 	ldreq	r3, [r1, #2880]
    e418:	15c53003 	strbne	r3, [r5, #3]
    e41c:	03833321 	orreq	r3, r3, #-2080374784	; 0x84000000
    e420:	05813b40 	streq	r3, [r1, #2880]
    e424:	e59f3010 	ldr	r3, [pc, #16]	; e43c <udc_event_handle+0x774>
    e428:	e2022005 	and	r2, r2, #5	; 0x5
    e42c:	e5832b48 	str	r2, [r3, #2888]
    e430:	e28dd008 	add	sp, sp, #8	; 0x8
    e434:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    e438:	3c00d1e8 	.word	0x3c00d1e8
    e43c:	27100000 	.word	0x27100000
    e440:	00010001 	.word	0x00010001
    e444:	0c001780 	.word	0x0c001780
    e448:	3c00d1ec 	.word	0x3c00d1ec
    e44c:	60080040 	.word	0x60080040
    e450:	3c00d1e4 	.word	0x3c00d1e4
    e454:	18088200 	.word	0x18088200
    e458:	3c00d200 	.word	0x3c00d200
    e45c:	00019c0a 	.word	0x00019c0a
    e460:	3c00d0e5 	.word	0x3c00d0e5
    e464:	3c00d105 	.word	0x3c00d105
    e468:	3c00d0bc 	.word	0x3c00d0bc
    e46c:	3c00d86c 	.word	0x3c00d86c
    e470:	3c00d0e4 	.word	0x3c00d0e4
    e474:	18088040 	.word	0x18088040

0000e478 <udc_write>:
    e478:	e59f30a4 	ldr	r3, [pc, #164]	; e524 <udc_write+0xac>
    e47c:	e3a02001 	mov	r2, #1	; 0x1
    e480:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    e484:	e583000c 	str	r0, [r3, #12]
    e488:	e1a05001 	mov	r5, r1
    e48c:	e5c32001 	strb	r2, [r3, #1]
    e490:	e1a04003 	mov	r4, r3
    e494:	e1a08002 	mov	r8, r2
    e498:	e59f6088 	ldr	r6, [pc, #136]	; e528 <udc_write+0xb0>
    e49c:	e59f7088 	ldr	r7, [pc, #136]	; e52c <udc_write+0xb4>
    e4a0:	ea00001b 	b	e514 <udc_write+0x9c>
    e4a4:	e3550701 	cmp	r5, #262144	; 0x40000
    e4a8:	b1a02005 	movlt	r2, r5
    e4ac:	a3a02701 	movge	r2, #262144	; 0x40000
    e4b0:	e292303f 	adds	r3, r2, #63	; 0x3f
    e4b4:	4282307e 	addmi	r3, r2, #126	; 0x7e
    e4b8:	e594100c 	ldr	r1, [r4, #12]
    e4bc:	e1a03343 	asr	r3, r3, #6
    e4c0:	e1823983 	orr	r3, r2, r3, lsl #19
    e4c4:	e5863930 	str	r3, [r6, #2352]
    e4c8:	e5861934 	str	r1, [r6, #2356]
    e4cc:	e5963920 	ldr	r3, [r6, #2336]
    e4d0:	e5842004 	str	r2, [r4, #4]
    e4d4:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    e4d8:	e5863920 	str	r3, [r6, #2336]
    e4dc:	e5c48002 	strb	r8, [r4, #2]
    e4e0:	ea000000 	b	e4e8 <udc_write+0x70>
    e4e4:	ebfffdf7 	bl	dcc8 <udc_event_handle>
    e4e8:	e5d43002 	ldrb	r3, [r4, #2]
    e4ec:	e3530000 	cmp	r3, #0	; 0x0
    e4f0:	0a000002 	beq	e500 <udc_write+0x88>
    e4f4:	e5973000 	ldr	r3, [r7]
    e4f8:	e3530000 	cmp	r3, #0	; 0x0
    e4fc:	1afffff8 	bne	e4e4 <udc_write+0x6c>
    e500:	e5943004 	ldr	r3, [r4, #4]
    e504:	e594200c 	ldr	r2, [r4, #12]
    e508:	e0635005 	rsb	r5, r3, r5
    e50c:	e0833002 	add	r3, r3, r2
    e510:	e584300c 	str	r3, [r4, #12]
    e514:	e3550000 	cmp	r5, #0	; 0x0
    e518:	1affffe1 	bne	e4a4 <udc_write+0x2c>
    e51c:	e1a00005 	mov	r0, r5
    e520:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    e524:	3c00d1ec 	.word	0x3c00d1ec
    e528:	27100000 	.word	0x27100000
    e52c:	3c00d0e0 	.word	0x3c00d0e0

0000e530 <udc_read>:
    e530:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    e534:	e59f30b4 	ldr	r3, [pc, #180]	; e5f0 <udc_read+0xc0>
    e538:	e24dd004 	sub	sp, sp, #4	; 0x4
    e53c:	e1a04001 	mov	r4, r1
    e540:	e1a05003 	mov	r5, r3
    e544:	e5830010 	str	r0, [r3, #16]
    e548:	e3a0a001 	mov	sl, #1	; 0x1
    e54c:	e59f60a0 	ldr	r6, [pc, #160]	; e5f4 <udc_read+0xc4>
    e550:	e59f80a0 	ldr	r8, [pc, #160]	; e5f8 <udc_read+0xc8>
    e554:	e1a07003 	mov	r7, r3
    e558:	ea00001f 	b	e5dc <udc_read+0xac>
    e55c:	e3540701 	cmp	r4, #262144	; 0x40000
    e560:	b1a02004 	movlt	r2, r4
    e564:	a3a02701 	movge	r2, #262144	; 0x40000
    e568:	e2823f7f 	add	r3, r2, #508	; 0x1fc
    e56c:	e2833003 	add	r3, r3, #3	; 0x3
    e570:	e5951010 	ldr	r1, [r5, #16]
    e574:	e1a034c3 	asr	r3, r3, #9
    e578:	e1823983 	orr	r3, r2, r3, lsl #19
    e57c:	e5863b50 	str	r3, [r6, #2896]
    e580:	e5861b54 	str	r1, [r6, #2900]
    e584:	e5963b40 	ldr	r3, [r6, #2880]
    e588:	e5852008 	str	r2, [r5, #8]
    e58c:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    e590:	e5863b40 	str	r3, [r6, #2880]
    e594:	e5c5a003 	strb	sl, [r5, #3]
    e598:	ea000002 	b	e5a8 <udc_read+0x78>
    e59c:	ebfffdc9 	bl	dcc8 <udc_event_handle>
    e5a0:	e3a00001 	mov	r0, #1	; 0x1
    e5a4:	ebfff3da 	bl	b514 <udelay>
    e5a8:	e5d53003 	ldrb	r3, [r5, #3]
    e5ac:	e3530000 	cmp	r3, #0	; 0x0
    e5b0:	0a000004 	beq	e5c8 <udc_read+0x98>
    e5b4:	e5983000 	ldr	r3, [r8]
    e5b8:	e3530000 	cmp	r3, #0	; 0x0
    e5bc:	1afffff6 	bne	e59c <udc_read+0x6c>
    e5c0:	e3e04000 	mvn	r4, #0	; 0x0
    e5c4:	ea000006 	b	e5e4 <udc_read+0xb4>
    e5c8:	e5973008 	ldr	r3, [r7, #8]
    e5cc:	e5972010 	ldr	r2, [r7, #16]
    e5d0:	e0634004 	rsb	r4, r3, r4
    e5d4:	e0833002 	add	r3, r3, r2
    e5d8:	e5873010 	str	r3, [r7, #16]
    e5dc:	e3540000 	cmp	r4, #0	; 0x0
    e5e0:	1affffdd 	bne	e55c <udc_read+0x2c>
    e5e4:	e1a00004 	mov	r0, r4
    e5e8:	e28dd004 	add	sp, sp, #4	; 0x4
    e5ec:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    e5f0:	3c00d1ec 	.word	0x3c00d1ec
    e5f4:	27100000 	.word	0x27100000
    e5f8:	3c00d0e0 	.word	0x3c00d0e0

0000e5fc <udc_connect>:
    e5fc:	e92d4070 	push	{r4, r5, r6, lr}
    e600:	e59f41e4 	ldr	r4, [pc, #484]	; e7ec <udc_connect+0x1f0>
    e604:	e3a02000 	mov	r2, #0	; 0x0
    e608:	e5943024 	ldr	r3, [r4, #36]
    e60c:	e3e01000 	mvn	r1, #0	; 0x0
    e610:	e1530002 	cmp	r3, r2
    e614:	e59f31d4 	ldr	r3, [pc, #468]	; e7f0 <udc_connect+0x1f4>
    e618:	e5c42003 	strb	r2, [r4, #3]
    e61c:	e5c31000 	strb	r1, [r3]
    e620:	e5c42000 	strb	r2, [r4]
    e624:	e5c42002 	strb	r2, [r4, #2]
    e628:	1a000002 	bne	e638 <udc_connect+0x3c>
    e62c:	e3a00028 	mov	r0, #40	; 0x28
    e630:	ebffcc53 	bl	1784 <malloc>
    e634:	e5840024 	str	r0, [r4, #36]
    e638:	e59f31ac 	ldr	r3, [pc, #428]	; e7ec <udc_connect+0x1f0>
    e63c:	e3a06001 	mov	r6, #1	; 0x1
    e640:	e59f51ac 	ldr	r5, [pc, #428]	; e7f4 <udc_connect+0x1f8>
    e644:	e59f01ac 	ldr	r0, [pc, #428]	; e7f8 <udc_connect+0x1fc>
    e648:	e5836020 	str	r6, [r3, #32]
    e64c:	ebffc858 	bl	7b4 <module_enable>
    e650:	e3a02004 	mov	r2, #4	; 0x4
    e654:	e3a03003 	mov	r3, #3	; 0x3
    e658:	e3a01000 	mov	r1, #0	; 0x0
    e65c:	e585243c 	str	r2, [r5, #1084]
    e660:	e5853440 	str	r3, [r5, #1088]
    e664:	e5852444 	str	r2, [r5, #1092]
    e668:	e5853448 	str	r3, [r5, #1096]
    e66c:	e585344c 	str	r3, [r5, #1100]
    e670:	e5853450 	str	r3, [r5, #1104]
    e674:	e5856454 	str	r6, [r5, #1108]
    e678:	e5856458 	str	r6, [r5, #1112]
    e67c:	e585145c 	str	r1, [r5, #1116]
    e680:	e5851460 	str	r1, [r5, #1120]
    e684:	e5953420 	ldr	r3, [r5, #1056]
    e688:	e1a00006 	mov	r0, r6
    e68c:	e3c33007 	bic	r3, r3, #7	; 0x7
    e690:	e3833005 	orr	r3, r3, #5	; 0x5
    e694:	e5853420 	str	r3, [r5, #1056]
    e698:	e5953424 	ldr	r3, [r5, #1060]
    e69c:	e3833003 	orr	r3, r3, #3	; 0x3
    e6a0:	e5853424 	str	r3, [r5, #1060]
    e6a4:	e595342c 	ldr	r3, [r5, #1068]
    e6a8:	e1833006 	orr	r3, r3, r6
    e6ac:	e585342c 	str	r3, [r5, #1068]
    e6b0:	e5954400 	ldr	r4, [r5, #1024]
    e6b4:	e384400f 	orr	r4, r4, #15	; 0xf
    e6b8:	e5854400 	str	r4, [r5, #1024]
    e6bc:	ebfff394 	bl	b514 <udelay>
    e6c0:	e3c4300e 	bic	r3, r4, #14	; 0xe
    e6c4:	e5853400 	str	r3, [r5, #1024]
    e6c8:	e1a00006 	mov	r0, r6
    e6cc:	e3c4400f 	bic	r4, r4, #15	; 0xf
    e6d0:	ebfff38f 	bl	b514 <udelay>
    e6d4:	e5854400 	str	r4, [r5, #1024]
    e6d8:	e1a00006 	mov	r0, r6
    e6dc:	ebfff38c 	bl	b514 <udelay>
    e6e0:	e59f2114 	ldr	r2, [pc, #276]	; e7fc <udc_connect+0x200>
    e6e4:	e59f3114 	ldr	r3, [pc, #276]	; e800 <udc_connect+0x204>
    e6e8:	e1a05002 	mov	r5, r2
    e6ec:	e582300c 	str	r3, [r2, #12]
    e6f0:	e5954010 	ldr	r4, [r5, #16]
    e6f4:	e3a00028 	mov	r0, #40	; 0x28
    e6f8:	ebfff385 	bl	b514 <udelay>
    e6fc:	e3540000 	cmp	r4, #0	; 0x0
    e700:	e59f40f4 	ldr	r4, [pc, #244]	; e7fc <udc_connect+0x200>
    e704:	aafffff9 	bge	e6f0 <udc_connect+0xf4>
    e708:	e5943010 	ldr	r3, [r4, #16]
    e70c:	e3a0000a 	mov	r0, #10	; 0xa
    e710:	e3833001 	orr	r3, r3, #1	; 0x1
    e714:	e5843010 	str	r3, [r4, #16]
    e718:	ebfff37d 	bl	b514 <udelay>
    e71c:	e1a05004 	mov	r5, r4
    e720:	e5954010 	ldr	r4, [r5, #16]
    e724:	e3a0000a 	mov	r0, #10	; 0xa
    e728:	ebfff379 	bl	b514 <udelay>
    e72c:	e3140001 	tst	r4, #1	; 0x1
    e730:	e59fc0c4 	ldr	ip, [pc, #196]	; e7fc <udc_connect+0x200>
    e734:	1afffff9 	bne	e720 <udc_connect+0x124>
    e738:	e3a03027 	mov	r3, #39	; 0x27
    e73c:	e58c3008 	str	r3, [ip, #8]
    e740:	e59c303c 	ldr	r3, [ip, #60]
    e744:	e59c3014 	ldr	r3, [ip, #20]
    e748:	e2130001 	ands	r0, r3, #1	; 0x1
    e74c:	1a000024 	bne	e7e4 <udc_connect+0x1e8>
    e750:	e3a03301 	mov	r3, #67108864	; 0x4000000
    e754:	e58c3800 	str	r3, [ip, #2048]
    e758:	e59f30a4 	ldr	r3, [pc, #164]	; e804 <udc_connect+0x208>
    e75c:	e59f20a4 	ldr	r2, [pc, #164]	; e808 <udc_connect+0x20c>
    e760:	e58c3018 	str	r3, [ip, #24]
    e764:	e59f30a0 	ldr	r3, [pc, #160]	; e80c <udc_connect+0x210>
    e768:	e59f10a0 	ldr	r1, [pc, #160]	; e810 <udc_connect+0x214>
    e76c:	e5823014 	str	r3, [r2, #20]
    e770:	e59f309c 	ldr	r3, [pc, #156]	; e814 <udc_connect+0x218>
    e774:	e5821010 	str	r1, [r2, #16]
    e778:	e5823004 	str	r3, [r2, #4]
    e77c:	e59f3094 	ldr	r3, [pc, #148]	; e818 <udc_connect+0x21c>
    e780:	e582100c 	str	r1, [r2, #12]
    e784:	e5823008 	str	r3, [r2, #8]
    e788:	ebfff385 	bl	b5a4 <get_timer>
    e78c:	e59f5058 	ldr	r5, [pc, #88]	; e7ec <udc_connect+0x1f0>
    e790:	e1a04000 	mov	r4, r0
    e794:	ebfffd4b 	bl	dcc8 <udc_event_handle>
    e798:	e5d53000 	ldrb	r3, [r5]
    e79c:	e1a00004 	mov	r0, r4
    e7a0:	e3530000 	cmp	r3, #0	; 0x0
    e7a4:	1a000003 	bne	e7b8 <udc_connect+0x1bc>
    e7a8:	ebfff37d 	bl	b5a4 <get_timer>
    e7ac:	e3500ffa 	cmp	r0, #1000	; 0x3e8
    e7b0:	9afffff7 	bls	e794 <udc_connect+0x198>
    e7b4:	ea00000a 	b	e7e4 <udc_connect+0x1e8>
    e7b8:	e59f203c 	ldr	r2, [pc, #60]	; e7fc <udc_connect+0x200>
    e7bc:	e59f3058 	ldr	r3, [pc, #88]	; e81c <udc_connect+0x220>
    e7c0:	e3a00000 	mov	r0, #0	; 0x0
    e7c4:	e5823920 	str	r3, [r2, #2336]
    e7c8:	e2833d07 	add	r3, r3, #448	; 0x1c0
    e7cc:	e5823b40 	str	r3, [r2, #2880]
    e7d0:	e592381c 	ldr	r3, [r2, #2076]
    e7d4:	e3833701 	orr	r3, r3, #262144	; 0x40000
    e7d8:	e3833002 	orr	r3, r3, #2	; 0x2
    e7dc:	e582381c 	str	r3, [r2, #2076]
    e7e0:	e8bd8070 	pop	{r4, r5, r6, pc}
    e7e4:	e3e00000 	mvn	r0, #0	; 0x0
    e7e8:	e8bd8070 	pop	{r4, r5, r6, pc}
    e7ec:	3c00d1ec 	.word	0x3c00d1ec
    e7f0:	3c00d0e4 	.word	0x3c00d0e4
    e7f4:	21e38000 	.word	0x21e38000
    e7f8:	21e38400 	.word	0x21e38400
    e7fc:	27100000 	.word	0x27100000
    e800:	4c001408 	.word	0x4c001408
    e804:	000c3c00 	.word	0x000c3c00
    e808:	3c00d86c 	.word	0x3c00d86c
    e80c:	0001c12f 	.word	0x0001c12f
    e810:	0001c144 	.word	0x0001c144
    e814:	0001c138 	.word	0x0001c138
    e818:	3c00d0c0 	.word	0x3c00d0c0
    e81c:	18088040 	.word	0x18088040

0000e820 <linkpc_set_stage>:
    e820:	e1a01000 	mov	r1, r0
    e824:	e59f0004 	ldr	r0, [pc, #4]	; e830 <linkpc_set_stage+0x10>
    e828:	e3a02010 	mov	r2, #16	; 0x10
    e82c:	eaffcb05 	b	1448 <strncpy>
    e830:	3c00d110 	.word	0x3c00d110

0000e834 <__response_ok>:
    e834:	e92d4010 	push	{r4, lr}
    e838:	e24dd020 	sub	sp, sp, #32	; 0x20
    e83c:	e59f1038 	ldr	r1, [pc, #56]	; e87c <__response_ok+0x48>
    e840:	e3a02005 	mov	r2, #5	; 0x5
    e844:	e1a0000d 	mov	r0, sp
    e848:	ebffcb68 	bl	15f0 <memcpy>
    e84c:	e3a0201b 	mov	r2, #27	; 0x1b
    e850:	e3a01000 	mov	r1, #0	; 0x0
    e854:	e28d0005 	add	r0, sp, #5	; 0x5
    e858:	ebffcb5c 	bl	15d0 <memset>
    e85c:	e1a0000d 	mov	r0, sp
    e860:	e3a01004 	mov	r1, #4	; 0x4
    e864:	ebffff03 	bl	e478 <udc_write>
    e868:	e59f0010 	ldr	r0, [pc, #16]	; e880 <__response_ok+0x4c>
    e86c:	e1a0400d 	mov	r4, sp
    e870:	ebfff327 	bl	b514 <udelay>
    e874:	e28dd020 	add	sp, sp, #32	; 0x20
    e878:	e8bd8010 	pop	{r4, pc}
    e87c:	00019c1c 	.word	0x00019c1c
    e880:	0007a120 	.word	0x0007a120

0000e884 <linkpc_loop>:
    e884:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    e888:	e59f0614 	ldr	r0, [pc, #1556]	; eea4 <linkpc_loop+0x620>
    e88c:	e24ddf5b 	sub	sp, sp, #364	; 0x16c
    e890:	e59f4610 	ldr	r4, [pc, #1552]	; eea8 <linkpc_loop+0x624>
    e894:	e1a0e00f 	mov	lr, pc
    e898:	e594f000 	ldr	pc, [r4]
    e89c:	ebffff56 	bl	e5fc <udc_connect>
    e8a0:	e3500000 	cmp	r0, #0	; 0x0
    e8a4:	13a04001 	movne	r4, #1	; 0x1
    e8a8:	1a000176 	bne	ee88 <linkpc_loop+0x604>
    e8ac:	e59f05f8 	ldr	r0, [pc, #1528]	; eeac <linkpc_loop+0x628>
    e8b0:	e1a0e00f 	mov	lr, pc
    e8b4:	e594f000 	ldr	pc, [r4]
    e8b8:	e28da094 	add	sl, sp, #148	; 0x94
    e8bc:	e59f05ec 	ldr	r0, [pc, #1516]	; eeb0 <linkpc_loop+0x62c>
    e8c0:	e1a0e00f 	mov	lr, pc
    e8c4:	e594f000 	ldr	pc, [r4]
    e8c8:	e28a2004 	add	r2, sl, #4	; 0x4
    e8cc:	e1a0b004 	mov	fp, r4
    e8d0:	e58d200c 	str	r2, [sp, #12]
    e8d4:	e59f35d8 	ldr	r3, [pc, #1496]	; eeb4 <linkpc_loop+0x630>
    e8d8:	e5931000 	ldr	r1, [r3]
    e8dc:	e3510000 	cmp	r1, #0	; 0x0
    e8e0:	159fc5d0 	ldrne	ip, [pc, #1488]	; eeb8 <linkpc_loop+0x634>
    e8e4:	028d4f45 	addeq	r4, sp, #276	; 0x114
    e8e8:	159c3000 	ldrne	r3, [ip]
    e8ec:	03a05040 	moveq	r5, #64	; 0x40
    e8f0:	1283430f 	addne	r4, r3, #1006632960	; 0x3c000000
    e8f4:	11a05001 	movne	r5, r1
    e8f8:	e1a01005 	mov	r1, r5
    e8fc:	e1a00004 	mov	r0, r4
    e900:	ebffff0a 	bl	e530 <udc_read>
    e904:	e3a02080 	mov	r2, #128	; 0x80
    e908:	e1a0000a 	mov	r0, sl
    e90c:	e3a01000 	mov	r1, #0	; 0x0
    e910:	ebffcb2e 	bl	15d0 <memset>
    e914:	e59f2598 	ldr	r2, [pc, #1432]	; eeb4 <linkpc_loop+0x630>
    e918:	e5923000 	ldr	r3, [r2]
    e91c:	e3530000 	cmp	r3, #0	; 0x0
    e920:	0a00002c 	beq	e9d8 <linkpc_loop+0x154>
    e924:	e28d4014 	add	r4, sp, #20	; 0x14
    e928:	e3a02080 	mov	r2, #128	; 0x80
    e92c:	e3a01000 	mov	r1, #0	; 0x0
    e930:	e1a00004 	mov	r0, r4
    e934:	ebffcb25 	bl	15d0 <memset>
    e938:	e59f057c 	ldr	r0, [pc, #1404]	; eebc <linkpc_loop+0x638>
    e93c:	e59f157c 	ldr	r1, [pc, #1404]	; eec0 <linkpc_loop+0x63c>
    e940:	e1a0e00f 	mov	lr, pc
    e944:	e59bf000 	ldr	pc, [fp]
    e948:	e59fc568 	ldr	ip, [pc, #1384]	; eeb8 <linkpc_loop+0x634>
    e94c:	e59f2560 	ldr	r2, [pc, #1376]	; eeb4 <linkpc_loop+0x630>
    e950:	e59c3000 	ldr	r3, [ip]
    e954:	e5920000 	ldr	r0, [r2]
    e958:	e0632000 	rsb	r2, r3, r0
    e95c:	e1550002 	cmp	r5, r2
    e960:	90833005 	addls	r3, r3, r5
    e964:	80833002 	addhi	r3, r3, r2
    e968:	e1530000 	cmp	r3, r0
    e96c:	e58c3000 	str	r3, [ip]
    e970:	3affffd7 	bcc	e8d4 <linkpc_loop+0x50>
    e974:	e59f3548 	ldr	r3, [pc, #1352]	; eec4 <linkpc_loop+0x640>
    e978:	e59fc534 	ldr	ip, [pc, #1332]	; eeb4 <linkpc_loop+0x630>
    e97c:	e5933000 	ldr	r3, [r3]
    e980:	e3530000 	cmp	r3, #0	; 0x0
    e984:	e59f353c 	ldr	r3, [pc, #1340]	; eec8 <linkpc_loop+0x644>
    e988:	159f153c 	ldrne	r1, [pc, #1340]	; eecc <linkpc_loop+0x648>
    e98c:	e5830000 	str	r0, [r3]
    e990:	e3a03000 	mov	r3, #0	; 0x0
    e994:	11a00004 	movne	r0, r4
    e998:	01a00004 	moveq	r0, r4
    e99c:	059f152c 	ldreq	r1, [pc, #1324]	; eed0 <linkpc_loop+0x64c>
    e9a0:	e58c3000 	str	r3, [ip]
    e9a4:	e28d4014 	add	r4, sp, #20	; 0x14
    e9a8:	eb0005e3 	bl	1013c <sprintf>
    e9ac:	e1a00004 	mov	r0, r4
    e9b0:	ebffcaf4 	bl	1588 <strlen>
    e9b4:	e1a01000 	mov	r1, r0
    e9b8:	e1a00004 	mov	r0, r4
    e9bc:	ebfffead 	bl	e478 <udc_write>
    e9c0:	e59f24f0 	ldr	r2, [pc, #1264]	; eeb8 <linkpc_loop+0x634>
    e9c4:	e59f0508 	ldr	r0, [pc, #1288]	; eed4 <linkpc_loop+0x650>
    e9c8:	e5921000 	ldr	r1, [r2]
    e9cc:	e1a0e00f 	mov	lr, pc
    e9d0:	e59bf000 	ldr	pc, [fp]
    e9d4:	eaffffbe 	b	e8d4 <linkpc_loop+0x50>
    e9d8:	e1a01004 	mov	r1, r4
    e9dc:	e59f04d8 	ldr	r0, [pc, #1240]	; eebc <linkpc_loop+0x638>
    e9e0:	e1a0e00f 	mov	lr, pc
    e9e4:	e59bf000 	ldr	pc, [fp]
    e9e8:	e59f14e8 	ldr	r1, [pc, #1256]	; eed8 <linkpc_loop+0x654>
    e9ec:	e1a0000a 	mov	r0, sl
    e9f0:	eb0005d1 	bl	1013c <sprintf>
    e9f4:	e1a00004 	mov	r0, r4
    e9f8:	e59f14dc 	ldr	r1, [pc, #1244]	; eedc <linkpc_loop+0x658>
    e9fc:	e3a02006 	mov	r2, #6	; 0x6
    ea00:	ebffcac1 	bl	150c <strncmp>
    ea04:	e3500000 	cmp	r0, #0	; 0x0
    ea08:	1a000002 	bne	ea18 <linkpc_loop+0x194>
    ea0c:	ebffff88 	bl	e834 <__response_ok>
    ea10:	ebffc79b 	bl	884 <system_reset>
    ea14:	ea00010e 	b	ee54 <linkpc_loop+0x5d0>
    ea18:	e1a00004 	mov	r0, r4
    ea1c:	e59f14bc 	ldr	r1, [pc, #1212]	; eee0 <linkpc_loop+0x65c>
    ea20:	e3a02007 	mov	r2, #7	; 0x7
    ea24:	ebffcab8 	bl	150c <strncmp>
    ea28:	e3500000 	cmp	r0, #0	; 0x0
    ea2c:	1a00002d 	bne	eae8 <linkpc_loop+0x264>
    ea30:	e2844007 	add	r4, r4, #7	; 0x7
    ea34:	e1a02004 	mov	r2, r4
    ea38:	e59f14a4 	ldr	r1, [pc, #1188]	; eee4 <linkpc_loop+0x660>
    ea3c:	e59f04a4 	ldr	r0, [pc, #1188]	; eee8 <linkpc_loop+0x664>
    ea40:	e1a0e00f 	mov	lr, pc
    ea44:	e59bf000 	ldr	pc, [fp]
    ea48:	e59f1480 	ldr	r1, [pc, #1152]	; eed0 <linkpc_loop+0x64c>
    ea4c:	e1a0000a 	mov	r0, sl
    ea50:	ebffca75 	bl	142c <strcpy>
    ea54:	e59f1490 	ldr	r1, [pc, #1168]	; eeec <linkpc_loop+0x668>
    ea58:	e1a00004 	mov	r0, r4
    ea5c:	e3a02007 	mov	r2, #7	; 0x7
    ea60:	ebffcaa9 	bl	150c <strncmp>
    ea64:	e3500000 	cmp	r0, #0	; 0x0
    ea68:	059d000c 	ldreq	r0, [sp, #12]
    ea6c:	059f147c 	ldreq	r1, [pc, #1148]	; eef0 <linkpc_loop+0x66c>
    ea70:	0a000010 	beq	eab8 <linkpc_loop+0x234>
    ea74:	e59f1478 	ldr	r1, [pc, #1144]	; eef4 <linkpc_loop+0x670>
    ea78:	e1a00004 	mov	r0, r4
    ea7c:	e3a02005 	mov	r2, #5	; 0x5
    ea80:	ebffcaa1 	bl	150c <strncmp>
    ea84:	e3500000 	cmp	r0, #0	; 0x0
    ea88:	059d000c 	ldreq	r0, [sp, #12]
    ea8c:	059f1464 	ldreq	r1, [pc, #1124]	; eef8 <linkpc_loop+0x674>
    ea90:	0a000008 	beq	eab8 <linkpc_loop+0x234>
    ea94:	e1a00004 	mov	r0, r4
    ea98:	e59f145c 	ldr	r1, [pc, #1116]	; eefc <linkpc_loop+0x678>
    ea9c:	e3a02008 	mov	r2, #8	; 0x8
    eaa0:	ebffca99 	bl	150c <strncmp>
    eaa4:	e3500000 	cmp	r0, #0	; 0x0
    eaa8:	1a000004 	bne	eac0 <linkpc_loop+0x23c>
    eaac:	e59f344c 	ldr	r3, [pc, #1100]	; ef00 <linkpc_loop+0x67c>
    eab0:	e59d000c 	ldr	r0, [sp, #12]
    eab4:	e5931008 	ldr	r1, [r3, #8]
    eab8:	ebffca5b 	bl	142c <strcpy>
    eabc:	ea0000e4 	b	ee54 <linkpc_loop+0x5d0>
    eac0:	e1a00004 	mov	r0, r4
    eac4:	e59f1438 	ldr	r1, [pc, #1080]	; ef04 <linkpc_loop+0x680>
    eac8:	e3a0200c 	mov	r2, #12	; 0xc
    eacc:	ebffca8e 	bl	150c <strncmp>
    ead0:	e3500000 	cmp	r0, #0	; 0x0
    ead4:	1a0000de 	bne	ee54 <linkpc_loop+0x5d0>
    ead8:	e59d000c 	ldr	r0, [sp, #12]
    eadc:	e59f1424 	ldr	r1, [pc, #1060]	; ef08 <linkpc_loop+0x684>
    eae0:	e3a02903 	mov	r2, #49152	; 0xc000
    eae4:	ea000023 	b	eb78 <linkpc_loop+0x2f4>
    eae8:	e1a00004 	mov	r0, r4
    eaec:	e59f1418 	ldr	r1, [pc, #1048]	; ef0c <linkpc_loop+0x688>
    eaf0:	e3a02009 	mov	r2, #9	; 0x9
    eaf4:	ebffca84 	bl	150c <strncmp>
    eaf8:	e2505000 	subs	r5, r0, #0	; 0x0
    eafc:	1a00001f 	bne	eb80 <linkpc_loop+0x2fc>
    eb00:	e59f33bc 	ldr	r3, [pc, #956]	; eec4 <linkpc_loop+0x640>
    eb04:	e59fc3ac 	ldr	ip, [pc, #940]	; eeb8 <linkpc_loop+0x634>
    eb08:	e5835000 	str	r5, [r3]
    eb0c:	e1a01005 	mov	r1, r5
    eb10:	e3a02010 	mov	r2, #16	; 0x10
    eb14:	e58c5000 	str	r5, [ip]
    eb18:	e2840009 	add	r0, r4, #9	; 0x9
    eb1c:	eb0001fa 	bl	f30c <simple_strtoul>
    eb20:	e59f238c 	ldr	r2, [pc, #908]	; eeb4 <linkpc_loop+0x630>
    eb24:	e1a01000 	mov	r1, r0
    eb28:	e5820000 	str	r0, [r2]
    eb2c:	e59f03dc 	ldr	r0, [pc, #988]	; ef10 <linkpc_loop+0x68c>
    eb30:	e1a0e00f 	mov	lr, pc
    eb34:	e59bf000 	ldr	pc, [fp]
    eb38:	e59f3374 	ldr	r3, [pc, #884]	; eeb4 <linkpc_loop+0x630>
    eb3c:	e5932000 	ldr	r2, [r3]
    eb40:	e3520000 	cmp	r2, #0	; 0x0
    eb44:	01a0000a 	moveq	r0, sl
    eb48:	059f13c4 	ldreq	r1, [pc, #964]	; ef14 <linkpc_loop+0x690>
    eb4c:	0a0000bf 	beq	ee50 <linkpc_loop+0x5cc>
    eb50:	e3520903 	cmp	r2, #49152	; 0xc000
    eb54:	91a0000a 	movls	r0, sl
    eb58:	959f13b8 	ldrls	r1, [pc, #952]	; ef18 <linkpc_loop+0x694>
    eb5c:	9a000005 	bls	eb78 <linkpc_loop+0x2f4>
    eb60:	e1a0000a 	mov	r0, sl
    eb64:	e59f13b0 	ldr	r1, [pc, #944]	; ef1c <linkpc_loop+0x698>
    eb68:	eb000573 	bl	1013c <sprintf>
    eb6c:	e59fc340 	ldr	ip, [pc, #832]	; eeb4 <linkpc_loop+0x630>
    eb70:	e58c5000 	str	r5, [ip]
    eb74:	ea0000b6 	b	ee54 <linkpc_loop+0x5d0>
    eb78:	eb00056f 	bl	1013c <sprintf>
    eb7c:	ea0000b4 	b	ee54 <linkpc_loop+0x5d0>
    eb80:	e1a00004 	mov	r0, r4
    eb84:	e59f1394 	ldr	r1, [pc, #916]	; ef20 <linkpc_loop+0x69c>
    eb88:	e3a02006 	mov	r2, #6	; 0x6
    eb8c:	ebffca5e 	bl	150c <strncmp>
    eb90:	e3500000 	cmp	r0, #0	; 0x0
    eb94:	1a00006b 	bne	ed48 <linkpc_loop+0x4c4>
    eb98:	e2844006 	add	r4, r4, #6	; 0x6
    eb9c:	e1a00004 	mov	r0, r4
    eba0:	e59f137c 	ldr	r1, [pc, #892]	; ef24 <linkpc_loop+0x6a0>
    eba4:	e3a02003 	mov	r2, #3	; 0x3
    eba8:	ebffca57 	bl	150c <strncmp>
    ebac:	e3500000 	cmp	r0, #0	; 0x0
    ebb0:	1a000007 	bne	ebd4 <linkpc_loop+0x350>
    ebb4:	e280030f 	add	r0, r0, #1006632960	; 0x3c000000
    ebb8:	ebffe2e0 	bl	7740 <cdbg_config_isi>
    ebbc:	e3500000 	cmp	r0, #0	; 0x0
    ebc0:	11a0000a 	movne	r0, sl
    ebc4:	159f135c 	ldrne	r1, [pc, #860]	; ef28 <linkpc_loop+0x6a4>
    ebc8:	01a0000a 	moveq	r0, sl
    ebcc:	059f12fc 	ldreq	r1, [pc, #764]	; eed0 <linkpc_loop+0x64c>
    ebd0:	ea00009e 	b	ee50 <linkpc_loop+0x5cc>
    ebd4:	e1a00004 	mov	r0, r4
    ebd8:	ebffdd93 	bl	622c <vs_device_id>
    ebdc:	e59f5348 	ldr	r5, [pc, #840]	; ef2c <linkpc_loop+0x6a8>
    ebe0:	e59fc348 	ldr	ip, [pc, #840]	; ef30 <linkpc_loop+0x6ac>
    ebe4:	e5953000 	ldr	r3, [r5]
    ebe8:	e1a07000 	mov	r7, r0
    ebec:	e58d3000 	str	r3, [sp]
    ebf0:	e59f033c 	ldr	r0, [pc, #828]	; ef34 <linkpc_loop+0x6b0>
    ebf4:	e59f133c 	ldr	r1, [pc, #828]	; ef38 <linkpc_loop+0x6b4>
    ebf8:	e1a02007 	mov	r2, r7
    ebfc:	e59c3000 	ldr	r3, [ip]
    ec00:	e1a0e00f 	mov	lr, pc
    ec04:	e59bf000 	ldr	pc, [fp]
    ec08:	e357000e 	cmp	r7, #14	; 0xe
    ec0c:	ca000006 	bgt	ec2c <linkpc_loop+0x3a8>
    ec10:	e1a00007 	mov	r0, r7
    ec14:	ebffdd70 	bl	61dc <vs_device_erasable>
    ec18:	e3500000 	cmp	r0, #0	; 0x0
    ec1c:	0a000005 	beq	ec38 <linkpc_loop+0x3b4>
    ec20:	e5953000 	ldr	r3, [r5]
    ec24:	e3530000 	cmp	r3, #0	; 0x0
    ec28:	1a000002 	bne	ec38 <linkpc_loop+0x3b4>
    ec2c:	e1a0000a 	mov	r0, sl
    ec30:	e59f1304 	ldr	r1, [pc, #772]	; ef3c <linkpc_loop+0x6b8>
    ec34:	ea000085 	b	ee50 <linkpc_loop+0x5cc>
    ec38:	e3a00901 	mov	r0, #16384	; 0x4000
    ec3c:	ebffcad0 	bl	1784 <malloc>
    ec40:	e2509000 	subs	r9, r0, #0	; 0x0
    ec44:	01a0000a 	moveq	r0, sl
    ec48:	059f12f0 	ldreq	r1, [pc, #752]	; ef40 <linkpc_loop+0x6bc>
    ec4c:	0a00007f 	beq	ee50 <linkpc_loop+0x5cc>
    ec50:	e3a01001 	mov	r1, #1	; 0x1
    ec54:	e1a00007 	mov	r0, r7
    ec58:	ebffde46 	bl	6578 <vs_assign_by_id>
    ec5c:	e3500000 	cmp	r0, #0	; 0x0
    ec60:	b1a0000a 	movlt	r0, sl
    ec64:	b59f12d8 	ldrlt	r1, [pc, #728]	; ef44 <linkpc_loop+0x6c0>
    ec68:	ba000032 	blt	ed38 <linkpc_loop+0x4b4>
    ec6c:	e3570001 	cmp	r7, #1	; 0x1
    ec70:	03a08004 	moveq	r8, #4	; 0x4
    ec74:	0a000006 	beq	ec94 <linkpc_loop+0x410>
    ec78:	e357000d 	cmp	r7, #13	; 0xd
    ec7c:	03a08001 	moveq	r8, #1	; 0x1
    ec80:	03a0700f 	moveq	r7, #15	; 0xf
    ec84:	0a000002 	beq	ec94 <linkpc_loop+0x410>
    ec88:	e3570000 	cmp	r7, #0	; 0x0
    ec8c:	03a08003 	moveq	r8, #3	; 0x3
    ec90:	13a08002 	movne	r8, #2	; 0x2
    ec94:	e1a04c08 	lsl	r4, r8, #24
    ec98:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
    ec9c:	e58d4154 	str	r4, [sp, #340]
    eca0:	ebfff696 	bl	c700 <ram_addr2offs>
    eca4:	e3a024ff 	mov	r2, #-16777216	; 0xff000000
    eca8:	e0922000 	adds	r2, r2, r0
    ecac:	e3e03000 	mvn	r3, #0	; 0x0
    ecb0:	e0a33001 	adc	r3, r3, r1
    ecb4:	e59f1270 	ldr	r1, [pc, #624]	; ef2c <linkpc_loop+0x6a8>
    ecb8:	e28d0f55 	add	r0, sp, #340	; 0x154
    ecbc:	e1d1c0b2 	ldrh	ip, [r1, #2]
    ecc0:	e1a014a2 	lsr	r1, r2, #9
    ecc4:	e1811b83 	orr	r1, r1, r3, lsl #23
    ecc8:	e59f2260 	ldr	r2, [pc, #608]	; ef30 <linkpc_loop+0x6ac>
    eccc:	e59f31f4 	ldr	r3, [pc, #500]	; eec8 <linkpc_loop+0x644>
    ecd0:	e5925000 	ldr	r5, [r2]
    ecd4:	e5936000 	ldr	r6, [r3]
    ecd8:	e1a0c80c 	lsl	ip, ip, #16
    ecdc:	e1844001 	orr	r4, r4, r1
    ece0:	e1a03007 	mov	r3, r7
    ece4:	e3a0100d 	mov	r1, #13	; 0xd
    ece8:	e3a02000 	mov	r2, #0	; 0x0
    ecec:	e58d4154 	str	r4, [sp, #340]
    ecf0:	e58d5158 	str	r5, [sp, #344]
    ecf4:	e58d6160 	str	r6, [sp, #352]
    ecf8:	e58dc000 	str	ip, [sp]
    ecfc:	e58dc15c 	str	ip, [sp, #348]
    ed00:	e58d9004 	str	r9, [sp, #4]
    ed04:	ebffe2a2 	bl	7794 <burn_image_loop>
    ed08:	e3580001 	cmp	r8, #1	; 0x1
    ed0c:	1a000004 	bne	ed24 <linkpc_loop+0x4a0>
    ed10:	ebfffec7 	bl	e834 <__response_ok>
    ed14:	e59d0158 	ldr	r0, [sp, #344]
    ed18:	e3a01000 	mov	r1, #0	; 0x0
    ed1c:	ebffe1f4 	bl	74f4 <cdbg_jump>
    ed20:	eafffffe 	b	ed20 <linkpc_loop+0x49c>
    ed24:	e3500000 	cmp	r0, #0	; 0x0
    ed28:	159f1218 	ldrne	r1, [pc, #536]	; ef48 <linkpc_loop+0x6c4>
    ed2c:	059f119c 	ldreq	r1, [pc, #412]	; eed0 <linkpc_loop+0x64c>
    ed30:	11a0000a 	movne	r0, sl
    ed34:	01a0000a 	moveq	r0, sl
    ed38:	eb0004ff 	bl	1013c <sprintf>
    ed3c:	e1a00009 	mov	r0, r9
    ed40:	ebffcaf7 	bl	1924 <free>
    ed44:	ea000042 	b	ee54 <linkpc_loop+0x5d0>
    ed48:	e1a00004 	mov	r0, r4
    ed4c:	e59f11f8 	ldr	r1, [pc, #504]	; ef4c <linkpc_loop+0x6c8>
    ed50:	e3a02008 	mov	r2, #8	; 0x8
    ed54:	ebffc9ec 	bl	150c <strncmp>
    ed58:	e3500000 	cmp	r0, #0	; 0x0
    ed5c:	1a000003 	bne	ed70 <linkpc_loop+0x4ec>
    ed60:	ebfffeb3 	bl	e834 <__response_ok>
    ed64:	e3a00001 	mov	r0, #1	; 0x1
    ed68:	ebffc5a8 	bl	410 <try_boot>
    ed6c:	ea000038 	b	ee54 <linkpc_loop+0x5d0>
    ed70:	e1a00004 	mov	r0, r4
    ed74:	e59f11d4 	ldr	r1, [pc, #468]	; ef50 <linkpc_loop+0x6cc>
    ed78:	e3a0200a 	mov	r2, #10	; 0xa
    ed7c:	ebffc9e2 	bl	150c <strncmp>
    ed80:	e2505000 	subs	r5, r0, #0	; 0x0
    ed84:	1a000019 	bne	edf0 <linkpc_loop+0x56c>
    ed88:	e28d1f59 	add	r1, sp, #356	; 0x164
    ed8c:	e284000b 	add	r0, r4, #11	; 0xb
    ed90:	e3a02010 	mov	r2, #16	; 0x10
    ed94:	eb00015c 	bl	f30c <simple_strtoul>
    ed98:	e59f3190 	ldr	r3, [pc, #400]	; ef30 <linkpc_loop+0x6ac>
    ed9c:	e1a01005 	mov	r1, r5
    eda0:	e5830000 	str	r0, [r3]
    eda4:	e59d0164 	ldr	r0, [sp, #356]
    eda8:	e3a02010 	mov	r2, #16	; 0x10
    edac:	e2800001 	add	r0, r0, #1	; 0x1
    edb0:	eb000155 	bl	f30c <simple_strtoul>
    edb4:	e59f4170 	ldr	r4, [pc, #368]	; ef2c <linkpc_loop+0x6a8>
    edb8:	e59fc170 	ldr	ip, [pc, #368]	; ef30 <linkpc_loop+0x6ac>
    edbc:	e1a02000 	mov	r2, r0
    edc0:	e5840000 	str	r0, [r4]
    edc4:	e59c1000 	ldr	r1, [ip]
    edc8:	e59f0184 	ldr	r0, [pc, #388]	; ef54 <linkpc_loop+0x6d0>
    edcc:	e1a0e00f 	mov	lr, pc
    edd0:	e59bf000 	ldr	pc, [fp]
    edd4:	e59fc154 	ldr	ip, [pc, #340]	; ef30 <linkpc_loop+0x6ac>
    edd8:	e1a0000a 	mov	r0, sl
    eddc:	e5943000 	ldr	r3, [r4]
    ede0:	e59f1170 	ldr	r1, [pc, #368]	; ef58 <linkpc_loop+0x6d4>
    ede4:	e59c2000 	ldr	r2, [ip]
    ede8:	eb0004d3 	bl	1013c <sprintf>
    edec:	ea000018 	b	ee54 <linkpc_loop+0x5d0>
    edf0:	e1a00004 	mov	r0, r4
    edf4:	e59f1160 	ldr	r1, [pc, #352]	; ef5c <linkpc_loop+0x6d8>
    edf8:	e3a0200d 	mov	r2, #13	; 0xd
    edfc:	ebffc9c2 	bl	150c <strncmp>
    ee00:	e3500000 	cmp	r0, #0	; 0x0
    ee04:	1a000006 	bne	ee24 <linkpc_loop+0x5a0>
    ee08:	e59f20f0 	ldr	r2, [pc, #240]	; ef00 <linkpc_loop+0x67c>
    ee0c:	e284100e 	add	r1, r4, #14	; 0xe
    ee10:	e5920008 	ldr	r0, [r2, #8]
    ee14:	ebffc984 	bl	142c <strcpy>
    ee18:	ebfffe85 	bl	e834 <__response_ok>
    ee1c:	ebfffe98 	bl	e884 <linkpc_loop>
    ee20:	ea00000b 	b	ee54 <linkpc_loop+0x5d0>
    ee24:	e1a00004 	mov	r0, r4
    ee28:	e59f1130 	ldr	r1, [pc, #304]	; ef60 <linkpc_loop+0x6dc>
    ee2c:	e3a02007 	mov	r2, #7	; 0x7
    ee30:	ebffc9b5 	bl	150c <strncmp>
    ee34:	e3500000 	cmp	r0, #0	; 0x0
    ee38:	1a000002 	bne	ee48 <linkpc_loop+0x5c4>
    ee3c:	ebfffe7c 	bl	e834 <__response_ok>
    ee40:	ebffe1dd 	bl	75bc <cdbg_shell>
    ee44:	eafffffe 	b	ee44 <linkpc_loop+0x5c0>
    ee48:	e59f1114 	ldr	r1, [pc, #276]	; ef64 <linkpc_loop+0x6e0>
    ee4c:	e1a0000a 	mov	r0, sl
    ee50:	eb0004b9 	bl	1013c <sprintf>
    ee54:	e3a01c01 	mov	r1, #256	; 0x100
    ee58:	e1a0000a 	mov	r0, sl
    ee5c:	ebffc9d1 	bl	15a8 <strnlen>
    ee60:	e1a01000 	mov	r1, r0
    ee64:	e1a0000a 	mov	r0, sl
    ee68:	ebfffd82 	bl	e478 <udc_write>
    ee6c:	e3500000 	cmp	r0, #0	; 0x0
    ee70:	0afffe97 	beq	e8d4 <linkpc_loop+0x50>
    ee74:	e59f00ec 	ldr	r0, [pc, #236]	; ef68 <linkpc_loop+0x6e4>
    ee78:	e59f3028 	ldr	r3, [pc, #40]	; eea8 <linkpc_loop+0x624>
    ee7c:	e1a0e00f 	mov	lr, pc
    ee80:	e593f000 	ldr	pc, [r3]
    ee84:	e3a04000 	mov	r4, #0	; 0x0
    ee88:	e59f00dc 	ldr	r0, [pc, #220]	; ef6c <linkpc_loop+0x6e8>
    ee8c:	e59f3014 	ldr	r3, [pc, #20]	; eea8 <linkpc_loop+0x624>
    ee90:	e1a0e00f 	mov	lr, pc
    ee94:	e593f000 	ldr	pc, [r3]
    ee98:	e1a00004 	mov	r0, r4
    ee9c:	e28ddf5b 	add	sp, sp, #364	; 0x16c
    eea0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    eea4:	0001c155 	.word	0x0001c155
    eea8:	3c00d694 	.word	0x3c00d694
    eeac:	0001a6be 	.word	0x0001a6be
    eeb0:	0001c164 	.word	0x0001c164
    eeb4:	3c00d218 	.word	0x3c00d218
    eeb8:	3c00d21c 	.word	0x3c00d21c
    eebc:	0001a0ca 	.word	0x0001a0ca
    eec0:	00019c3c 	.word	0x00019c3c
    eec4:	3c00d220 	.word	0x3c00d220
    eec8:	3c00d224 	.word	0x3c00d224
    eecc:	0001c177 	.word	0x0001c177
    eed0:	00019c1c 	.word	0x00019c1c
    eed4:	0001c17d 	.word	0x0001c17d
    eed8:	0001c1a0 	.word	0x0001c1a0
    eedc:	0001c1a5 	.word	0x0001c1a5
    eee0:	0001c1ac 	.word	0x0001c1ac
    eee4:	00019c48 	.word	0x00019c48
    eee8:	0001c1b4 	.word	0x0001c1b4
    eeec:	0001c1bc 	.word	0x0001c1bc
    eef0:	3c00d094 	.word	0x3c00d094
    eef4:	0001c1c4 	.word	0x0001c1c4
    eef8:	3c00d110 	.word	0x3c00d110
    eefc:	0001c1ca 	.word	0x0001c1ca
    ef00:	3c00d86c 	.word	0x3c00d86c
    ef04:	0001c1d3 	.word	0x0001c1d3
    ef08:	0001c236 	.word	0x0001c236
    ef0c:	0001c1e0 	.word	0x0001c1e0
    ef10:	0001c1ea 	.word	0x0001c1ea
    ef14:	0001c209 	.word	0x0001c209
    ef18:	0001c23b 	.word	0x0001c23b
    ef1c:	0001c222 	.word	0x0001c222
    ef20:	0001c244 	.word	0x0001c244
    ef24:	0001a543 	.word	0x0001a543
    ef28:	0001c24b 	.word	0x0001c24b
    ef2c:	3c00d22c 	.word	0x3c00d22c
    ef30:	3c00d228 	.word	0x3c00d228
    ef34:	0001c263 	.word	0x0001c263
    ef38:	00019c55 	.word	0x00019c55
    ef3c:	0001c273 	.word	0x0001c273
    ef40:	0001c28a 	.word	0x0001c28a
    ef44:	0001c298 	.word	0x0001c298
    ef48:	0001c2b1 	.word	0x0001c2b1
    ef4c:	0001c2c6 	.word	0x0001c2c6
    ef50:	0001c2cf 	.word	0x0001c2cf
    ef54:	0001c2da 	.word	0x0001c2da
    ef58:	0001c2f2 	.word	0x0001c2f2
    ef5c:	0001c30d 	.word	0x0001c30d
    ef60:	0001c31b 	.word	0x0001c31b
    ef64:	0001c323 	.word	0x0001c323
    ef68:	0001c337 	.word	0x0001c337
    ef6c:	0001c35a 	.word	0x0001c35a

0000ef70 <pp_list>:
    ef70:	00000994 	.word	0x00000994
    ef74:	00003771 	.word	0x00003771
    ef78:	00003739 	.word	0x00003739
    ef7c:	00003cb9 	.word	0x00003cb9
    ef80:	000029a0 	.word	0x000029a0
    ef84:	00003f91 	.word	0x00003f91
    ef88:	000031e1 	.word	0x000031e1
    ef8c:	00002e6b 	.word	0x00002e6b
    ef90:	0000275d 	.word	0x0000275d
    ef94:	00002297 	.word	0x00002297
    ef98:	00002378 	.word	0x00002378
    ef9c:	000034db 	.word	0x000034db
    efa0:	000029fc 	.word	0x000029fc
    efa4:	0000271c 	.word	0x0000271c
    efa8:	000036ec 	.word	0x000036ec
    efac:	00003ab9 	.word	0x00003ab9
    efb0:	00003218 	.word	0x00003218
    efb4:	0000307f 	.word	0x0000307f
    efb8:	0000267b 	.word	0x0000267b
    efbc:	00003d95 	.word	0x00003d95
    efc0:	00003e0f 	.word	0x00003e0f
    efc4:	00002d75 	.word	0x00002d75
    efc8:	000031a3 	.word	0x000031a3

0000efcc <print_size>:
    efcc:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    efd0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    efd4:	e1a05000 	mov	r5, r0
    efd8:	e1a08001 	mov	r8, r1
    efdc:	83a04101 	movhi	r4, #1073741824	; 0x40000000
    efe0:	83a07047 	movhi	r7, #71	; 0x47
    efe4:	8a000005 	bhi	f000 <print_size+0x34>
    efe8:	e59f30a8 	ldr	r3, [pc, #168]	; f098 <print_size+0xcc>
    efec:	e1500003 	cmp	r0, r3
    eff0:	83a0704d 	movhi	r7, #77	; 0x4d
    eff4:	93a0706b 	movls	r7, #107	; 0x6b
    eff8:	83a04601 	movhi	r4, #1048576	; 0x100000
    effc:	93a04b01 	movls	r4, #1024	; 0x400
    f000:	e1a01004 	mov	r1, r4
    f004:	e1a00005 	mov	r0, r5
    f008:	eb000cc3 	bl	1231c <__aeabi_uidiv>
    f00c:	e1a01004 	mov	r1, r4
    f010:	e1a06000 	mov	r6, r0
    f014:	e1a00005 	mov	r0, r5
    f018:	eb000d3b 	bl	1250c <__aeabi_uidivmod>
    f01c:	e2510000 	subs	r0, r1, #0	; 0x0
    f020:	01a04000 	moveq	r4, r0
    f024:	0a000009 	beq	f050 <print_size+0x84>
    f028:	e0625694 	mls	r2, r4, r6, r5
    f02c:	e1a030a4 	lsr	r3, r4, #1
    f030:	e3a0000a 	mov	r0, #10	; 0xa
    f034:	e1a01004 	mov	r1, r4
    f038:	e0203290 	mla	r0, r0, r2, r3
    f03c:	eb000cb6 	bl	1231c <__aeabi_uidiv>
    f040:	e3500009 	cmp	r0, #9	; 0x9
    f044:	e1a04000 	mov	r4, r0
    f048:	82866001 	addhi	r6, r6, #1	; 0x1
    f04c:	8240400a 	subhi	r4, r0, #10	; 0xa
    f050:	e1a01006 	mov	r1, r6
    f054:	e59f5040 	ldr	r5, [pc, #64]	; f09c <print_size+0xd0>
    f058:	e59f0040 	ldr	r0, [pc, #64]	; f0a0 <print_size+0xd4>
    f05c:	e1a0e00f 	mov	lr, pc
    f060:	e595f000 	ldr	pc, [r5]
    f064:	e3540000 	cmp	r4, #0	; 0x0
    f068:	0a000003 	beq	f07c <print_size+0xb0>
    f06c:	e1a01004 	mov	r1, r4
    f070:	e59f002c 	ldr	r0, [pc, #44]	; f0a4 <print_size+0xd8>
    f074:	e1a0e00f 	mov	lr, pc
    f078:	e595f000 	ldr	pc, [r5]
    f07c:	e1a01007 	mov	r1, r7
    f080:	e1a02008 	mov	r2, r8
    f084:	e59f001c 	ldr	r0, [pc, #28]	; f0a8 <print_size+0xdc>
    f088:	e59f300c 	ldr	r3, [pc, #12]	; f09c <print_size+0xd0>
    f08c:	e1a0e00f 	mov	lr, pc
    f090:	e593f000 	ldr	pc, [r3]
    f094:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    f098:	000fffff 	.word	0x000fffff
    f09c:	3c00d694 	.word	0x3c00d694
    f0a0:	0001c362 	.word	0x0001c362
    f0a4:	0001c367 	.word	0x0001c367
    f0a8:	0001c36c 	.word	0x0001c36c

0000f0ac <print_buffer>:
    f0ac:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    f0b0:	e24dd044 	sub	sp, sp, #68	; 0x44
    f0b4:	e59d6068 	ldr	r6, [sp, #104]
    f0b8:	e1a07002 	mov	r7, r2
    f0bc:	e0020692 	mul	r2, r2, r6
    f0c0:	e3520040 	cmp	r2, #64	; 0x40
    f0c4:	e1a09000 	mov	r9, r0
    f0c8:	e1a0a001 	mov	sl, r1
    f0cc:	e1a08003 	mov	r8, r3
    f0d0:	9a000003 	bls	f0e4 <print_buffer+0x38>
    f0d4:	e3a00040 	mov	r0, #64	; 0x40
    f0d8:	e1a01007 	mov	r1, r7
    f0dc:	eb000c8e 	bl	1231c <__aeabi_uidiv>
    f0e0:	e1a06000 	mov	r6, r0
    f0e4:	e3560000 	cmp	r6, #0	; 0x0
    f0e8:	1a00003c 	bne	f1e0 <print_buffer+0x134>
    f0ec:	e3a00010 	mov	r0, #16	; 0x10
    f0f0:	e1a01007 	mov	r1, r7
    f0f4:	eb000c88 	bl	1231c <__aeabi_uidiv>
    f0f8:	e1a06000 	mov	r6, r0
    f0fc:	ea000037 	b	f1e0 <print_buffer+0x134>
    f100:	e59f00f0 	ldr	r0, [pc, #240]	; f1f8 <print_buffer+0x14c>
    f104:	e1a01009 	mov	r1, r9
    f108:	e1a0e00f 	mov	lr, pc
    f10c:	e59bf000 	ldr	pc, [fp]
    f110:	e1560008 	cmp	r6, r8
    f114:	21a06008 	movcs	r6, r8
    f118:	e1a0500a 	mov	r5, sl
    f11c:	e3a04000 	mov	r4, #0	; 0x0
    f120:	ea00000f 	b	f164 <print_buffer+0xb8>
    f124:	e3570004 	cmp	r7, #4	; 0x4
    f128:	05951000 	ldreq	r1, [r5]
    f12c:	059f00c8 	ldreq	r0, [pc, #200]	; f1fc <print_buffer+0x150>
    f130:	078d1104 	streq	r1, [sp, r4, lsl #2]
    f134:	0a000006 	beq	f154 <print_buffer+0xa8>
    f138:	e3570002 	cmp	r7, #2	; 0x2
    f13c:	01d510b0 	ldrheq	r1, [r5]
    f140:	15d51000 	ldrbne	r1, [r5]
    f144:	01a03084 	lsleq	r3, r4, #1
    f148:	059f00b0 	ldreq	r0, [pc, #176]	; f200 <print_buffer+0x154>
    f14c:	018d10b3 	strheq	r1, [sp, r3]
    f150:	17cd1004 	strbne	r1, [sp, r4]
    f154:	e1a0e00f 	mov	lr, pc
    f158:	e59bf000 	ldr	pc, [fp]
    f15c:	e0855007 	add	r5, r5, r7
    f160:	e2844001 	add	r4, r4, #1	; 0x1
    f164:	e1540006 	cmp	r4, r6
    f168:	e59f0094 	ldr	r0, [pc, #148]	; f204 <print_buffer+0x158>
    f16c:	3affffec 	bcc	f124 <print_buffer+0x78>
    f170:	e0050697 	mul	r5, r7, r6
    f174:	e59f008c 	ldr	r0, [pc, #140]	; f208 <print_buffer+0x15c>
    f178:	e08aa005 	add	sl, sl, r5
    f17c:	eb000450 	bl	102c4 <puts>
    f180:	e3a04000 	mov	r4, #0	; 0x0
    f184:	ea000009 	b	f1b0 <print_buffer+0x104>
    f188:	e7dd0004 	ldrb	r0, [sp, r4]
    f18c:	e59f2078 	ldr	r2, [pc, #120]	; f20c <print_buffer+0x160>
    f190:	e2844001 	add	r4, r4, #1	; 0x1
    f194:	e7d23000 	ldrb	r3, [r2, r0]
    f198:	e3130097 	tst	r3, #151	; 0x97
    f19c:	0a000001 	beq	f1a8 <print_buffer+0xfc>
    f1a0:	e3100080 	tst	r0, #128	; 0x80
    f1a4:	0a000000 	beq	f1ac <print_buffer+0x100>
    f1a8:	e3a0002e 	mov	r0, #46	; 0x2e
    f1ac:	eb00046c 	bl	10364 <putc>
    f1b0:	e1540005 	cmp	r4, r5
    f1b4:	3afffff3 	bcc	f188 <print_buffer+0xdc>
    f1b8:	e59f0050 	ldr	r0, [pc, #80]	; f210 <print_buffer+0x164>
    f1bc:	e1a0e00f 	mov	lr, pc
    f1c0:	e59bf000 	ldr	pc, [fp]
    f1c4:	eb000469 	bl	10370 <ctrlc>
    f1c8:	e3500000 	cmp	r0, #0	; 0x0
    f1cc:	13e00000 	mvnne	r0, #0	; 0x0
    f1d0:	1a000006 	bne	f1f0 <print_buffer+0x144>
    f1d4:	e0899005 	add	r9, r9, r5
    f1d8:	e0668008 	rsb	r8, r6, r8
    f1dc:	ea000000 	b	f1e4 <print_buffer+0x138>
    f1e0:	e59fb02c 	ldr	fp, [pc, #44]	; f214 <print_buffer+0x168>
    f1e4:	e3580000 	cmp	r8, #0	; 0x0
    f1e8:	1affffc4 	bne	f100 <print_buffer+0x54>
    f1ec:	e1a00008 	mov	r0, r8
    f1f0:	e28dd044 	add	sp, sp, #68	; 0x44
    f1f4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    f1f8:	0001c373 	.word	0x0001c373
    f1fc:	0001bce2 	.word	0x0001bce2
    f200:	0001c37a 	.word	0x0001c37a
    f204:	0001c380 	.word	0x0001c380
    f208:	0001c386 	.word	0x0001c386
    f20c:	00019c7f 	.word	0x00019c7f
    f210:	0001a882 	.word	0x0001a882
    f214:	3c00d694 	.word	0x3c00d694

0000f218 <__div64_32>:
    f218:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    f21c:	e8900300 	ldm	r0, {r8, r9}
    f220:	e1510009 	cmp	r1, r9
    f224:	e1a0a000 	mov	sl, r0
    f228:	e1a04001 	mov	r4, r1
    f22c:	83a06000 	movhi	r6, #0	; 0x0
    f230:	83a07000 	movhi	r7, #0	; 0x0
    f234:	8a000007 	bhi	f258 <__div64_32+0x40>
    f238:	e1a00009 	mov	r0, r9
    f23c:	eb000c36 	bl	1231c <__aeabi_uidiv>
    f240:	e0030094 	mul	r3, r4, r0
    f244:	e3a06000 	mov	r6, #0	; 0x0
    f248:	e1a02006 	mov	r2, r6
    f24c:	e1a07000 	mov	r7, r0
    f250:	e0588002 	subs	r8, r8, r2
    f254:	e0c99003 	sbc	r9, r9, r3
    f258:	e1a00004 	mov	r0, r4
    f25c:	e3a01000 	mov	r1, #0	; 0x0
    f260:	e3a05000 	mov	r5, #0	; 0x0
    f264:	e3a04001 	mov	r4, #1	; 0x1
    f268:	ea000003 	b	f27c <__div64_32+0x64>
    f26c:	e0900000 	adds	r0, r0, r0
    f270:	e0a11001 	adc	r1, r1, r1
    f274:	e0944004 	adds	r4, r4, r4
    f278:	e0a55005 	adc	r5, r5, r5
    f27c:	e3510000 	cmp	r1, #0	; 0x0
    f280:	e3a03000 	mov	r3, #0	; 0x0
    f284:	ca000002 	bgt	f294 <__div64_32+0x7c>
    f288:	1a000002 	bne	f298 <__div64_32+0x80>
    f28c:	e3500000 	cmp	r0, #0	; 0x0
    f290:	0a000000 	beq	f298 <__div64_32+0x80>
    f294:	e3a03001 	mov	r3, #1	; 0x1
    f298:	e1590001 	cmp	r9, r1
    f29c:	e3a02000 	mov	r2, #0	; 0x0
    f2a0:	8a000002 	bhi	f2b0 <__div64_32+0x98>
    f2a4:	1a000002 	bne	f2b4 <__div64_32+0x9c>
    f2a8:	e1580000 	cmp	r8, r0
    f2ac:	9a000000 	bls	f2b4 <__div64_32+0x9c>
    f2b0:	e3a02001 	mov	r2, #1	; 0x1
    f2b4:	e0033002 	and	r3, r3, r2
    f2b8:	e31300ff 	tst	r3, #255	; 0xff
    f2bc:	1affffea 	bne	f26c <__div64_32+0x54>
    f2c0:	e1510009 	cmp	r1, r9
    f2c4:	8a000006 	bhi	f2e4 <__div64_32+0xcc>
    f2c8:	1a000001 	bne	f2d4 <__div64_32+0xbc>
    f2cc:	e1500008 	cmp	r0, r8
    f2d0:	8a000003 	bhi	f2e4 <__div64_32+0xcc>
    f2d4:	e0588000 	subs	r8, r8, r0
    f2d8:	e0c99001 	sbc	r9, r9, r1
    f2dc:	e0966004 	adds	r6, r6, r4
    f2e0:	e0a77005 	adc	r7, r7, r5
    f2e4:	e1b050a5 	lsrs	r5, r5, #1
    f2e8:	e1a04064 	rrx	r4, r4
    f2ec:	e1943005 	orrs	r3, r4, r5
    f2f0:	0a000002 	beq	f300 <__div64_32+0xe8>
    f2f4:	e1b010a1 	lsrs	r1, r1, #1
    f2f8:	e1a00060 	rrx	r0, r0
    f2fc:	eaffffef 	b	f2c0 <__div64_32+0xa8>
    f300:	e88a00c0 	stm	sl, {r6, r7}
    f304:	e1a00008 	mov	r0, r8
    f308:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

0000f30c <simple_strtoul>:
    f30c:	e92d4070 	push	{r4, r5, r6, lr}
    f310:	e5d03000 	ldrb	r3, [r0]
    f314:	e1a05001 	mov	r5, r1
    f318:	e3530030 	cmp	r3, #48	; 0x30
    f31c:	e1a01002 	mov	r1, r2
    f320:	1a00000d 	bne	f35c <simple_strtoul+0x50>
    f324:	e5f03001 	ldrb	r3, [r0, #1]!
    f328:	e3530078 	cmp	r3, #120	; 0x78
    f32c:	1a000007 	bne	f350 <simple_strtoul+0x44>
    f330:	e5d03001 	ldrb	r3, [r0, #1]
    f334:	e59f2084 	ldr	r2, [pc, #132]	; f3c0 <simple_strtoul+0xb4>
    f338:	e280c001 	add	ip, r0, #1	; 0x1
    f33c:	e7d23003 	ldrb	r3, [r2, r3]
    f340:	e3130044 	tst	r3, #68	; 0x44
    f344:	11a0000c 	movne	r0, ip
    f348:	13a01010 	movne	r1, #16	; 0x10
    f34c:	1a000004 	bne	f364 <simple_strtoul+0x58>
    f350:	e3510000 	cmp	r1, #0	; 0x0
    f354:	03a01008 	moveq	r1, #8	; 0x8
    f358:	ea000001 	b	f364 <simple_strtoul+0x58>
    f35c:	e3520000 	cmp	r2, #0	; 0x0
    f360:	03a0100a 	moveq	r1, #10	; 0xa
    f364:	e3a04000 	mov	r4, #0	; 0x0
    f368:	e59f6050 	ldr	r6, [pc, #80]	; f3c0 <simple_strtoul+0xb4>
    f36c:	ea000001 	b	f378 <simple_strtoul+0x6c>
    f370:	e0243491 	mla	r4, r1, r4, r3
    f374:	e2800001 	add	r0, r0, #1	; 0x1
    f378:	e5d0c000 	ldrb	ip, [r0]
    f37c:	e7d6200c 	ldrb	r2, [r6, ip]
    f380:	e24c3030 	sub	r3, ip, #48	; 0x30
    f384:	e3120044 	tst	r2, #68	; 0x44
    f388:	0a000008 	beq	f3b0 <simple_strtoul+0xa4>
    f38c:	e3120004 	tst	r2, #4	; 0x4
    f390:	1a000004 	bne	f3a8 <simple_strtoul+0x9c>
    f394:	e24c3020 	sub	r3, ip, #32	; 0x20
    f398:	e3120002 	tst	r2, #2	; 0x2
    f39c:	e6ef3073 	uxtb	r3, r3
    f3a0:	12433037 	subne	r3, r3, #55	; 0x37
    f3a4:	024c3037 	subeq	r3, ip, #55	; 0x37
    f3a8:	e1530001 	cmp	r3, r1
    f3ac:	3affffef 	bcc	f370 <simple_strtoul+0x64>
    f3b0:	e3550000 	cmp	r5, #0	; 0x0
    f3b4:	15850000 	strne	r0, [r5]
    f3b8:	e1a00004 	mov	r0, r4
    f3bc:	e8bd8070 	pop	{r4, r5, r6, pc}
    f3c0:	00019c7f 	.word	0x00019c7f

0000f3c4 <simple_strtol>:
    f3c4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    f3c8:	e5d03000 	ldrb	r3, [r0]
    f3cc:	e24dd004 	sub	sp, sp, #4	; 0x4
    f3d0:	e353002d 	cmp	r3, #45	; 0x2d
    f3d4:	1a000004 	bne	f3ec <simple_strtol+0x28>
    f3d8:	e2800001 	add	r0, r0, #1	; 0x1
    f3dc:	ebffffca 	bl	f30c <simple_strtoul>
    f3e0:	e2600000 	rsb	r0, r0, #0	; 0x0
    f3e4:	e28dd004 	add	sp, sp, #4	; 0x4
    f3e8:	e8bd8000 	pop	{pc}
    f3ec:	e28dd004 	add	sp, sp, #4	; 0x4
    f3f0:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    f3f4:	eaffffc4 	b	f30c <simple_strtoul>

0000f3f8 <ustrtoul>:
    f3f8:	e92d4010 	push	{r4, lr}
    f3fc:	e1a04001 	mov	r4, r1
    f400:	ebffffc1 	bl	f30c <simple_strtoul>
    f404:	e5941000 	ldr	r1, [r4]
    f408:	e5d13000 	ldrb	r3, [r1]
    f40c:	e353004b 	cmp	r3, #75	; 0x4b
    f410:	0a00000a 	beq	f440 <ustrtoul+0x48>
    f414:	8a000002 	bhi	f424 <ustrtoul+0x2c>
    f418:	e3530047 	cmp	r3, #71	; 0x47
    f41c:	18bd8010 	popne	{r4, pc}
    f420:	ea000004 	b	f438 <ustrtoul+0x40>
    f424:	e353004d 	cmp	r3, #77	; 0x4d
    f428:	0a000003 	beq	f43c <ustrtoul+0x44>
    f42c:	e353006b 	cmp	r3, #107	; 0x6b
    f430:	18bd8010 	popne	{r4, pc}
    f434:	ea000001 	b	f440 <ustrtoul+0x48>
    f438:	e1a00500 	lsl	r0, r0, #10
    f43c:	e1a00500 	lsl	r0, r0, #10
    f440:	e5d13001 	ldrb	r3, [r1, #1]
    f444:	e1a00500 	lsl	r0, r0, #10
    f448:	e3530069 	cmp	r3, #105	; 0x69
    f44c:	18bd8010 	popne	{r4, pc}
    f450:	e5d13002 	ldrb	r3, [r1, #2]
    f454:	e2812002 	add	r2, r1, #2	; 0x2
    f458:	e3530042 	cmp	r3, #66	; 0x42
    f45c:	02813003 	addeq	r3, r1, #3	; 0x3
    f460:	05843000 	streq	r3, [r4]
    f464:	15842000 	strne	r2, [r4]
    f468:	e8bd8010 	pop	{r4, pc}

0000f46c <simple_strtoull>:
    f46c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    f470:	e5d03000 	ldrb	r3, [r0]
    f474:	e1a07000 	mov	r7, r0
    f478:	e3530030 	cmp	r3, #48	; 0x30
    f47c:	e1a08001 	mov	r8, r1
    f480:	e1a06002 	mov	r6, r2
    f484:	1a00000d 	bne	f4c0 <simple_strtoull+0x54>
    f488:	e5f73001 	ldrb	r3, [r7, #1]!
    f48c:	e3530078 	cmp	r3, #120	; 0x78
    f490:	1a000007 	bne	f4b4 <simple_strtoull+0x48>
    f494:	e5d73001 	ldrb	r3, [r7, #1]
    f498:	e59f20ac 	ldr	r2, [pc, #172]	; f54c <simple_strtoull+0xe0>
    f49c:	e2870001 	add	r0, r7, #1	; 0x1
    f4a0:	e7d23003 	ldrb	r3, [r2, r3]
    f4a4:	e3130044 	tst	r3, #68	; 0x44
    f4a8:	11a07000 	movne	r7, r0
    f4ac:	13a06010 	movne	r6, #16	; 0x10
    f4b0:	1a000004 	bne	f4c8 <simple_strtoull+0x5c>
    f4b4:	e3560000 	cmp	r6, #0	; 0x0
    f4b8:	03a06008 	moveq	r6, #8	; 0x8
    f4bc:	ea000001 	b	f4c8 <simple_strtoull+0x5c>
    f4c0:	e3520000 	cmp	r2, #0	; 0x0
    f4c4:	03a0600a 	moveq	r6, #10	; 0xa
    f4c8:	e3a04000 	mov	r4, #0	; 0x0
    f4cc:	e3a05000 	mov	r5, #0	; 0x0
    f4d0:	e59f9074 	ldr	r9, [pc, #116]	; f54c <simple_strtoull+0xe0>
    f4d4:	e3a0a000 	mov	sl, #0	; 0x0
    f4d8:	ea000004 	b	f4f0 <simple_strtoull+0x84>
    f4dc:	e0832694 	umull	r2, r3, r4, r6
    f4e0:	e0233596 	mla	r3, r6, r5, r3
    f4e4:	e0904002 	adds	r4, r0, r2
    f4e8:	e0a15003 	adc	r5, r1, r3
    f4ec:	e2877001 	add	r7, r7, #1	; 0x1
    f4f0:	e5d7c000 	ldrb	ip, [r7]
    f4f4:	e7d9200c 	ldrb	r2, [r9, ip]
    f4f8:	e24c3030 	sub	r3, ip, #48	; 0x30
    f4fc:	e3120044 	tst	r2, #68	; 0x44
    f500:	0a00000c 	beq	f538 <simple_strtoull+0xcc>
    f504:	e3120004 	tst	r2, #4	; 0x4
    f508:	1a000004 	bne	f520 <simple_strtoull+0xb4>
    f50c:	e24c3020 	sub	r3, ip, #32	; 0x20
    f510:	e3120002 	tst	r2, #2	; 0x2
    f514:	e6ef3073 	uxtb	r3, r3
    f518:	12433037 	subne	r3, r3, #55	; 0x37
    f51c:	024c3037 	subeq	r3, ip, #55	; 0x37
    f520:	e1a00003 	mov	r0, r3
    f524:	e1a01fc0 	asr	r1, r0, #31
    f528:	e15a0001 	cmp	sl, r1
    f52c:	1a000001 	bne	f538 <simple_strtoull+0xcc>
    f530:	e1560000 	cmp	r6, r0
    f534:	8affffe8 	bhi	f4dc <simple_strtoull+0x70>
    f538:	e3580000 	cmp	r8, #0	; 0x0
    f53c:	15887000 	strne	r7, [r8]
    f540:	e1a01005 	mov	r1, r5
    f544:	e1a00004 	mov	r0, r4
    f548:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    f54c:	00019c7f 	.word	0x00019c7f

0000f550 <put_dec_trunc>:
    f550:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    f554:	e1a03421 	lsr	r3, r1, #8
    f558:	e1a04221 	lsr	r4, r1, #4
    f55c:	e204400f 	and	r4, r4, #15	; 0xf
    f560:	e203600f 	and	r6, r3, #15	; 0xf
    f564:	e1a05621 	lsr	r5, r1, #12
    f568:	e0842006 	add	r2, r4, r6
    f56c:	e201100f 	and	r1, r1, #15	; 0xf
    f570:	e3a03006 	mov	r3, #6	; 0x6
    f574:	e0822005 	add	r2, r2, r5
    f578:	e0221293 	mla	r2, r3, r2, r1
    f57c:	e3a080cd 	mov	r8, #205	; 0xcd
    f580:	e00c0298 	mul	ip, r8, r2
    f584:	e3a01009 	mov	r1, #9	; 0x9
    f588:	e0214591 	mla	r1, r1, r5, r4
    f58c:	e1a0c5ac 	lsr	ip, ip, #11
    f590:	e3a0700a 	mov	r7, #10	; 0xa
    f594:	e3a03005 	mov	r3, #5	; 0x5
    f598:	e0231693 	mla	r3, r3, r6, r1
    f59c:	e0622c97 	mls	r2, r7, ip, r2
    f5a0:	e093100c 	adds	r1, r3, ip
    f5a4:	e2822030 	add	r2, r2, #48	; 0x30
    f5a8:	e1a04000 	mov	r4, r0
    f5ac:	e4c02001 	strb	r2, [r0], #1
    f5b0:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    f5b4:	e0030198 	mul	r3, r8, r1
    f5b8:	e1a035a3 	lsr	r3, r3, #11
    f5bc:	e0621397 	mls	r2, r7, r3, r1
    f5c0:	e0831086 	add	r1, r3, r6, lsl #1
    f5c4:	e2822030 	add	r2, r2, #48	; 0x30
    f5c8:	e1913005 	orrs	r3, r1, r5
    f5cc:	e5c42001 	strb	r2, [r4, #1]
    f5d0:	e2800001 	add	r0, r0, #1	; 0x1
    f5d4:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    f5d8:	e3a0300d 	mov	r3, #13	; 0xd
    f5dc:	e0030193 	mul	r3, r3, r1
    f5e0:	e1a033a3 	lsr	r3, r3, #7
    f5e4:	e0621397 	mls	r2, r7, r3, r1
    f5e8:	e0931105 	adds	r1, r3, r5, lsl #2
    f5ec:	e2822030 	add	r2, r2, #48	; 0x30
    f5f0:	e4c02001 	strb	r2, [r0], #1
    f5f4:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    f5f8:	e0030198 	mul	r3, r8, r1
    f5fc:	e1a0c5a3 	lsr	ip, r3, #11
    f600:	e0631c97 	mls	r3, r7, ip, r1
    f604:	e35c0000 	cmp	ip, #0	; 0x0
    f608:	e2833030 	add	r3, r3, #48	; 0x30
    f60c:	e4c03001 	strb	r3, [r0], #1
    f610:	128c3030 	addne	r3, ip, #48	; 0x30
    f614:	14c03001 	strbne	r3, [r0], #1
    f618:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000f61c <string>:
    f61c:	e59fc0a4 	ldr	ip, [pc, #164]	; f6c8 <string+0xac>
    f620:	e3510000 	cmp	r1, #0	; 0x0
    f624:	e92d4070 	push	{r4, r5, r6, lr}
    f628:	11a06001 	movne	r6, r1
    f62c:	01a0600c 	moveq	r6, ip
    f630:	e1a01003 	mov	r1, r3
    f634:	e1a04000 	mov	r4, r0
    f638:	e1a00006 	mov	r0, r6
    f63c:	e1a05002 	mov	r5, r2
    f640:	ebffc7d8 	bl	15a8 <strnlen>
    f644:	e59d3010 	ldr	r3, [sp, #16]
    f648:	e1a0c000 	mov	ip, r0
    f64c:	e3130010 	tst	r3, #16	; 0x10
    f650:	01a02004 	moveq	r2, r4
    f654:	03a01020 	moveq	r1, #32	; 0x20
    f658:	0a000001 	beq	f664 <string+0x48>
    f65c:	ea000006 	b	f67c <string+0x60>
    f660:	e5421001 	strb	r1, [r2, #-1]
    f664:	e2455001 	sub	r5, r5, #1	; 0x1
    f668:	e2853001 	add	r3, r5, #1	; 0x1
    f66c:	e15c0003 	cmp	ip, r3
    f670:	e1a04002 	mov	r4, r2
    f674:	e2822001 	add	r2, r2, #1	; 0x1
    f678:	bafffff8 	blt	f660 <string+0x44>
    f67c:	e3a02000 	mov	r2, #0	; 0x0
    f680:	ea000002 	b	f690 <string+0x74>
    f684:	e7d63002 	ldrb	r3, [r6, r2]
    f688:	e7c43002 	strb	r3, [r4, r2]
    f68c:	e2822001 	add	r2, r2, #1	; 0x1
    f690:	e152000c 	cmp	r2, ip
    f694:	e0843002 	add	r3, r4, r2
    f698:	bafffff9 	blt	f684 <string+0x68>
    f69c:	e1a01003 	mov	r1, r3
    f6a0:	e1a02005 	mov	r2, r5
    f6a4:	e3a03020 	mov	r3, #32	; 0x20
    f6a8:	ea000000 	b	f6b0 <string+0x94>
    f6ac:	e5413001 	strb	r3, [r1, #-1]
    f6b0:	e15c0002 	cmp	ip, r2
    f6b4:	e1a00001 	mov	r0, r1
    f6b8:	e2422001 	sub	r2, r2, #1	; 0x1
    f6bc:	e2811001 	add	r1, r1, #1	; 0x1
    f6c0:	bafffff9 	blt	f6ac <string+0x90>
    f6c4:	e8bd8070 	pop	{r4, r5, r6, pc}
    f6c8:	0001c38b 	.word	0x0001c38b

0000f6cc <put_dec>:
    f6cc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    f6d0:	e24dd00c 	sub	sp, sp, #12	; 0xc
    f6d4:	e28d8008 	add	r8, sp, #8	; 0x8
    f6d8:	e16820f8 	strd	r2, [r8, #-8]!
    f6dc:	e1a07000 	mov	r7, r0
    f6e0:	e3a090cd 	mov	r9, #205	; 0xcd
    f6e4:	e3a0a00a 	mov	sl, #10	; 0xa
    f6e8:	e3a0b00d 	mov	fp, #13	; 0xd
    f6ec:	e59d5004 	ldr	r5, [sp, #4]
    f6f0:	e1a0000d 	mov	r0, sp
    f6f4:	e3550000 	cmp	r5, #0	; 0x0
    f6f8:	e59f10f0 	ldr	r1, [pc, #240]	; f7f0 <put_dec+0x124>
    f6fc:	e59d4000 	ldr	r4, [sp]
    f700:	1a00000f 	bne	f744 <put_dec+0x78>
    f704:	e59f30e8 	ldr	r3, [pc, #232]	; f7f4 <put_dec+0x128>
    f708:	e1a00004 	mov	r0, r4
    f70c:	e1540003 	cmp	r4, r3
    f710:	8a000004 	bhi	f728 <put_dec+0x5c>
    f714:	e1a00007 	mov	r0, r7
    f718:	e1a01004 	mov	r1, r4
    f71c:	ebffff8b 	bl	f550 <put_dec_trunc>
    f720:	e28dd00c 	add	sp, sp, #12	; 0xc
    f724:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    f728:	eb000b77 	bl	1250c <__aeabi_uidivmod>
    f72c:	e1a00004 	mov	r0, r4
    f730:	e1a06001 	mov	r6, r1
    f734:	e59f10b4 	ldr	r1, [pc, #180]	; f7f0 <put_dec+0x124>
    f738:	eb000af7 	bl	1231c <__aeabi_uidiv>
    f73c:	e88d0021 	stm	sp, {r0, r5}
    f740:	ea000001 	b	f74c <put_dec+0x80>
    f744:	ebfffeb3 	bl	f218 <__div64_32>
    f748:	e1a06000 	mov	r6, r0
    f74c:	e1a02226 	lsr	r2, r6, #4
    f750:	e1a0c426 	lsr	ip, r6, #8
    f754:	e20cc00f 	and	ip, ip, #15	; 0xf
    f758:	e202200f 	and	r2, r2, #15	; 0xf
    f75c:	e1a0e626 	lsr	lr, r6, #12
    f760:	e082500c 	add	r5, r2, ip
    f764:	e085500e 	add	r5, r5, lr
    f768:	e206300f 	and	r3, r6, #15	; 0xf
    f76c:	e3a01006 	mov	r1, #6	; 0x6
    f770:	e0253591 	mla	r5, r1, r5, r3
    f774:	e3a03009 	mov	r3, #9	; 0x9
    f778:	e0222e93 	mla	r2, r3, lr, r2
    f77c:	e3a01005 	mov	r1, #5	; 0x5
    f780:	e0000599 	mul	r0, r9, r5
    f784:	e0222c91 	mla	r2, r1, ip, r2
    f788:	e1a005a0 	lsr	r0, r0, #11
    f78c:	e0822000 	add	r2, r2, r0
    f790:	e0010299 	mul	r1, r9, r2
    f794:	e1a015a1 	lsr	r1, r1, #11
    f798:	e081c08c 	add	ip, r1, ip, lsl #1
    f79c:	e0030c9b 	mul	r3, fp, ip
    f7a0:	e1a033a3 	lsr	r3, r3, #7
    f7a4:	e083e10e 	add	lr, r3, lr, lsl #2
    f7a8:	e0040e99 	mul	r4, r9, lr
    f7ac:	e1a045a4 	lsr	r4, r4, #11
    f7b0:	e06ee49a 	mls	lr, sl, r4, lr
    f7b4:	e060509a 	mls	r0, sl, r0, r5
    f7b8:	e061219a 	mls	r1, sl, r1, r2
    f7bc:	e063c39a 	mls	r3, sl, r3, ip
    f7c0:	e2800030 	add	r0, r0, #48	; 0x30
    f7c4:	e2811030 	add	r1, r1, #48	; 0x30
    f7c8:	e2833030 	add	r3, r3, #48	; 0x30
    f7cc:	e28ee030 	add	lr, lr, #48	; 0x30
    f7d0:	e2844030 	add	r4, r4, #48	; 0x30
    f7d4:	e5c70000 	strb	r0, [r7]
    f7d8:	e5c71001 	strb	r1, [r7, #1]
    f7dc:	e5c73002 	strb	r3, [r7, #2]
    f7e0:	e5c7e003 	strb	lr, [r7, #3]
    f7e4:	e5c74004 	strb	r4, [r7, #4]
    f7e8:	e2877005 	add	r7, r7, #5	; 0x5
    f7ec:	eaffffbe 	b	f6ec <put_dec+0x20>
    f7f0:	000186a0 	.word	0x000186a0
    f7f4:	0001869f 	.word	0x0001869f

0000f7f8 <number>:
    f7f8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    f7fc:	e24dd05c 	sub	sp, sp, #92	; 0x5c
    f800:	e1a0a000 	mov	sl, r0
    f804:	e59d0080 	ldr	r0, [sp, #128]
    f808:	e59d808c 	ldr	r8, [sp, #140]
    f80c:	e250100a 	subs	r1, r0, #10	; 0xa
    f810:	13a01001 	movne	r1, #1	; 0x1
    f814:	e0011328 	and	r1, r1, r8, lsr #6
    f818:	e3180010 	tst	r8, #16	; 0x10
    f81c:	e58d1008 	str	r1, [sp, #8]
    f820:	e2081020 	and	r1, r8, #32	; 0x20
    f824:	13c88001 	bicne	r8, r8, #1	; 0x1
    f828:	e3180002 	tst	r8, #2	; 0x2
    f82c:	e1a06002 	mov	r6, r2
    f830:	e1a07003 	mov	r7, r3
    f834:	e59d5084 	ldr	r5, [sp, #132]
    f838:	e58d1004 	str	r1, [sp, #4]
    f83c:	0a000010 	beq	f884 <number+0x8c>
    f840:	e3530000 	cmp	r3, #0	; 0x0
    f844:	ba000002 	blt	f854 <number+0x5c>
    f848:	e3180004 	tst	r8, #4	; 0x4
    f84c:	0a000008 	beq	f874 <number+0x7c>
    f850:	ea000004 	b	f868 <number+0x70>
    f854:	e2766000 	rsbs	r6, r6, #0	; 0x0
    f858:	e2e77000 	rsc	r7, r7, #0	; 0x0
    f85c:	e2455001 	sub	r5, r5, #1	; 0x1
    f860:	e3a0902d 	mov	r9, #45	; 0x2d
    f864:	ea000007 	b	f888 <number+0x90>
    f868:	e2455001 	sub	r5, r5, #1	; 0x1
    f86c:	e3a0902b 	mov	r9, #43	; 0x2b
    f870:	ea000004 	b	f888 <number+0x90>
    f874:	e3180008 	tst	r8, #8	; 0x8
    f878:	12455001 	subne	r5, r5, #1	; 0x1
    f87c:	13a09020 	movne	r9, #32	; 0x20
    f880:	1a000000 	bne	f888 <number+0x90>
    f884:	e3a09000 	mov	r9, #0	; 0x0
    f888:	e59d2008 	ldr	r2, [sp, #8]
    f88c:	e3520000 	cmp	r2, #0	; 0x0
    f890:	0a000003 	beq	f8a4 <number+0xac>
    f894:	e59d3080 	ldr	r3, [sp, #128]
    f898:	e2455001 	sub	r5, r5, #1	; 0x1
    f89c:	e3530010 	cmp	r3, #16	; 0x10
    f8a0:	02455001 	subeq	r5, r5, #1	; 0x1
    f8a4:	e196c007 	orrs	ip, r6, r7
    f8a8:	03a03030 	moveq	r3, #48	; 0x30
    f8ac:	05cd3016 	strbeq	r3, [sp, #22]
    f8b0:	03a04001 	moveq	r4, #1	; 0x1
    f8b4:	0a000020 	beq	f93c <number+0x144>
    f8b8:	e59d0080 	ldr	r0, [sp, #128]
    f8bc:	e350000a 	cmp	r0, #10	; 0xa
    f8c0:	0a000017 	beq	f924 <number+0x12c>
    f8c4:	e3500010 	cmp	r0, #16	; 0x10
    f8c8:	03a01004 	moveq	r1, #4	; 0x4
    f8cc:	13a01003 	movne	r1, #3	; 0x3
    f8d0:	e58d100c 	str	r1, [sp, #12]
    f8d4:	e240b001 	sub	fp, r0, #1	; 0x1
    f8d8:	e3a04000 	mov	r4, #0	; 0x0
    f8dc:	e6ef3076 	uxtb	r3, r6
    f8e0:	e59f2164 	ldr	r2, [pc, #356]	; fa4c <number+0x254>
    f8e4:	e003300b 	and	r3, r3, fp
    f8e8:	e59dc004 	ldr	ip, [sp, #4]
    f8ec:	e7d23003 	ldrb	r3, [r2, r3]
    f8f0:	e1a00006 	mov	r0, r6
    f8f4:	e18c3003 	orr	r3, ip, r3
    f8f8:	e28dc016 	add	ip, sp, #22	; 0x16
    f8fc:	e1a01007 	mov	r1, r7
    f900:	e59d200c 	ldr	r2, [sp, #12]
    f904:	e7cc3004 	strb	r3, [ip, r4]
    f908:	eb000b93 	bl	1275c <__aeabi_llsr>
    f90c:	e1a06000 	mov	r6, r0
    f910:	e1960001 	orrs	r0, r6, r1
    f914:	e1a07001 	mov	r7, r1
    f918:	e2844001 	add	r4, r4, #1	; 0x1
    f91c:	1affffee 	bne	f8dc <number+0xe4>
    f920:	ea000005 	b	f93c <number+0x144>
    f924:	e28d4016 	add	r4, sp, #22	; 0x16
    f928:	e1a00004 	mov	r0, r4
    f92c:	e1a02006 	mov	r2, r6
    f930:	e1a03007 	mov	r3, r7
    f934:	ebffff64 	bl	f6cc <put_dec>
    f938:	e0644000 	rsb	r4, r4, r0
    f93c:	e59d3088 	ldr	r3, [sp, #136]
    f940:	e1540003 	cmp	r4, r3
    f944:	a1a01004 	movge	r1, r4
    f948:	b1a01003 	movlt	r1, r3
    f94c:	e3180011 	tst	r8, #17	; 0x11
    f950:	e061c005 	rsb	ip, r1, r5
    f954:	01a0300a 	moveq	r3, sl
    f958:	03a02020 	moveq	r2, #32	; 0x20
    f95c:	0a000001 	beq	f968 <number+0x170>
    f960:	ea000004 	b	f978 <number+0x180>
    f964:	e5432001 	strb	r2, [r3, #-1]
    f968:	e25cc001 	subs	ip, ip, #1	; 0x1
    f96c:	e1a0a003 	mov	sl, r3
    f970:	e2833001 	add	r3, r3, #1	; 0x1
    f974:	5afffffa 	bpl	f964 <number+0x16c>
    f978:	e3590000 	cmp	r9, #0	; 0x0
    f97c:	14ca9001 	strbne	r9, [sl], #1
    f980:	e59d2008 	ldr	r2, [sp, #8]
    f984:	e3520000 	cmp	r2, #0	; 0x0
    f988:	0a000006 	beq	f9a8 <number+0x1b0>
    f98c:	e59d3080 	ldr	r3, [sp, #128]
    f990:	e3530010 	cmp	r3, #16	; 0x10
    f994:	e3a03030 	mov	r3, #48	; 0x30
    f998:	e4ca3001 	strb	r3, [sl], #1
    f99c:	059d0004 	ldreq	r0, [sp, #4]
    f9a0:	03803058 	orreq	r3, r0, #88	; 0x58
    f9a4:	04ca3001 	strbeq	r3, [sl], #1
    f9a8:	e3180010 	tst	r8, #16	; 0x10
    f9ac:	1a000009 	bne	f9d8 <number+0x1e0>
    f9b0:	e3180001 	tst	r8, #1	; 0x1
    f9b4:	13a02030 	movne	r2, #48	; 0x30
    f9b8:	03a02020 	moveq	r2, #32	; 0x20
    f9bc:	e1a0300a 	mov	r3, sl
    f9c0:	ea000000 	b	f9c8 <number+0x1d0>
    f9c4:	e5432001 	strb	r2, [r3, #-1]
    f9c8:	e25cc001 	subs	ip, ip, #1	; 0x1
    f9cc:	e1a0a003 	mov	sl, r3
    f9d0:	e2833001 	add	r3, r3, #1	; 0x1
    f9d4:	5afffffa 	bpl	f9c4 <number+0x1cc>
    f9d8:	e1a0000a 	mov	r0, sl
    f9dc:	e3a03030 	mov	r3, #48	; 0x30
    f9e0:	ea000000 	b	f9e8 <number+0x1f0>
    f9e4:	e5403001 	strb	r3, [r0, #-1]
    f9e8:	e2411001 	sub	r1, r1, #1	; 0x1
    f9ec:	e1540001 	cmp	r4, r1
    f9f0:	e1a02000 	mov	r2, r0
    f9f4:	e2800001 	add	r0, r0, #1	; 0x1
    f9f8:	dafffff9 	ble	f9e4 <number+0x1ec>
    f9fc:	e28d1058 	add	r1, sp, #88	; 0x58
    fa00:	e0813004 	add	r3, r1, r4
    fa04:	e2431043 	sub	r1, r3, #67	; 0x43
    fa08:	ea000001 	b	fa14 <number+0x21c>
    fa0c:	e5d13001 	ldrb	r3, [r1, #1]
    fa10:	e5423001 	strb	r3, [r2, #-1]
    fa14:	e2544001 	subs	r4, r4, #1	; 0x1
    fa18:	e1a03002 	mov	r3, r2
    fa1c:	e2411001 	sub	r1, r1, #1	; 0x1
    fa20:	e2822001 	add	r2, r2, #1	; 0x1
    fa24:	5afffff8 	bpl	fa0c <number+0x214>
    fa28:	e3a02020 	mov	r2, #32	; 0x20
    fa2c:	ea000000 	b	fa34 <number+0x23c>
    fa30:	e5432001 	strb	r2, [r3, #-1]
    fa34:	e25cc001 	subs	ip, ip, #1	; 0x1
    fa38:	e1a00003 	mov	r0, r3
    fa3c:	e2833001 	add	r3, r3, #1	; 0x1
    fa40:	5afffffa 	bpl	fa30 <number+0x238>
    fa44:	e28dd05c 	add	sp, sp, #92	; 0x5c
    fa48:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    fa4c:	00019c6f 	.word	0x00019c6f

0000fa50 <vsprintf>:
    fa50:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    fa54:	e24dd084 	sub	sp, sp, #132	; 0x84
    fa58:	e58d0028 	str	r0, [sp, #40]
    fa5c:	e1a08001 	mov	r8, r1
    fa60:	e1a0a000 	mov	sl, r0
    fa64:	e28d107b 	add	r1, sp, #123	; 0x7b
    fa68:	e28d007d 	add	r0, sp, #125	; 0x7d
    fa6c:	e58d2024 	str	r2, [sp, #36]
    fa70:	e1a06002 	mov	r6, r2
    fa74:	e58d001c 	str	r0, [sp, #28]
    fa78:	e58d1018 	str	r1, [sp, #24]
    fa7c:	ea0001a3 	b	10110 <vsprintf+0x6c0>
    fa80:	e3530025 	cmp	r3, #37	; 0x25
    fa84:	1a00015f 	bne	10008 <vsprintf+0x5b8>
    fa88:	e3a07000 	mov	r7, #0	; 0x0
    fa8c:	e5f82001 	ldrb	r2, [r8, #1]!
    fa90:	e352002b 	cmp	r2, #43	; 0x2b
    fa94:	0a00000c 	beq	facc <vsprintf+0x7c>
    fa98:	8a000004 	bhi	fab0 <vsprintf+0x60>
    fa9c:	e3520020 	cmp	r2, #32	; 0x20
    faa0:	0a00000b 	beq	fad4 <vsprintf+0x84>
    faa4:	e3520023 	cmp	r2, #35	; 0x23
    faa8:	1a00000f 	bne	faec <vsprintf+0x9c>
    faac:	ea00000a 	b	fadc <vsprintf+0x8c>
    fab0:	e352002d 	cmp	r2, #45	; 0x2d
    fab4:	0a000002 	beq	fac4 <vsprintf+0x74>
    fab8:	e3520030 	cmp	r2, #48	; 0x30
    fabc:	1a00000a 	bne	faec <vsprintf+0x9c>
    fac0:	ea000007 	b	fae4 <vsprintf+0x94>
    fac4:	e3877010 	orr	r7, r7, #16	; 0x10
    fac8:	eaffffef 	b	fa8c <vsprintf+0x3c>
    facc:	e3877004 	orr	r7, r7, #4	; 0x4
    fad0:	eaffffed 	b	fa8c <vsprintf+0x3c>
    fad4:	e3877008 	orr	r7, r7, #8	; 0x8
    fad8:	eaffffeb 	b	fa8c <vsprintf+0x3c>
    fadc:	e3877040 	orr	r7, r7, #64	; 0x40
    fae0:	eaffffe9 	b	fa8c <vsprintf+0x3c>
    fae4:	e3877001 	orr	r7, r7, #1	; 0x1
    fae8:	eaffffe7 	b	fa8c <vsprintf+0x3c>
    faec:	e2423030 	sub	r3, r2, #48	; 0x30
    faf0:	e6ef3073 	uxtb	r3, r3
    faf4:	e3530009 	cmp	r3, #9	; 0x9
    faf8:	93a09000 	movls	r9, #0	; 0x0
    fafc:	93a0100a 	movls	r1, #10	; 0xa
    fb00:	9a000002 	bls	fb10 <vsprintf+0xc0>
    fb04:	ea000007 	b	fb28 <vsprintf+0xd8>
    fb08:	e0292991 	mla	r9, r1, r9, r2
    fb0c:	e2888001 	add	r8, r8, #1	; 0x1
    fb10:	e5d83000 	ldrb	r3, [r8]
    fb14:	e2432030 	sub	r2, r3, #48	; 0x30
    fb18:	e6ef3072 	uxtb	r3, r2
    fb1c:	e3530009 	cmp	r3, #9	; 0x9
    fb20:	9afffff8 	bls	fb08 <vsprintf+0xb8>
    fb24:	ea000008 	b	fb4c <vsprintf+0xfc>
    fb28:	e352002a 	cmp	r2, #42	; 0x2a
    fb2c:	13e09000 	mvnne	r9, #0	; 0x0
    fb30:	1a000005 	bne	fb4c <vsprintf+0xfc>
    fb34:	e5969000 	ldr	r9, [r6]
    fb38:	e2888001 	add	r8, r8, #1	; 0x1
    fb3c:	e3590000 	cmp	r9, #0	; 0x0
    fb40:	e2866004 	add	r6, r6, #4	; 0x4
    fb44:	b2699000 	rsblt	r9, r9, #0	; 0x0
    fb48:	b3877010 	orrlt	r7, r7, #16	; 0x10
    fb4c:	e5d83000 	ldrb	r3, [r8]
    fb50:	e353002e 	cmp	r3, #46	; 0x2e
    fb54:	13e02000 	mvnne	r2, #0	; 0x0
    fb58:	158d202c 	strne	r2, [sp, #44]
    fb5c:	1a00001d 	bne	fbd8 <vsprintf+0x188>
    fb60:	e5f82001 	ldrb	r2, [r8, #1]!
    fb64:	e2423030 	sub	r3, r2, #48	; 0x30
    fb68:	e6ef3073 	uxtb	r3, r3
    fb6c:	e3530009 	cmp	r3, #9	; 0x9
    fb70:	93a03000 	movls	r3, #0	; 0x0
    fb74:	958d302c 	strls	r3, [sp, #44]
    fb78:	93a0100a 	movls	r1, #10	; 0xa
    fb7c:	9a000004 	bls	fb94 <vsprintf+0x144>
    fb80:	ea000009 	b	fbac <vsprintf+0x15c>
    fb84:	e59d002c 	ldr	r0, [sp, #44]
    fb88:	e2888001 	add	r8, r8, #1	; 0x1
    fb8c:	e0202091 	mla	r0, r1, r0, r2
    fb90:	e58d002c 	str	r0, [sp, #44]
    fb94:	e5d83000 	ldrb	r3, [r8]
    fb98:	e2432030 	sub	r2, r3, #48	; 0x30
    fb9c:	e6ef3072 	uxtb	r3, r2
    fba0:	e3530009 	cmp	r3, #9	; 0x9
    fba4:	9afffff6 	bls	fb84 <vsprintf+0x134>
    fba8:	ea000005 	b	fbc4 <vsprintf+0x174>
    fbac:	e352002a 	cmp	r2, #42	; 0x2a
    fbb0:	1a000006 	bne	fbd0 <vsprintf+0x180>
    fbb4:	e5961000 	ldr	r1, [r6]
    fbb8:	e2888001 	add	r8, r8, #1	; 0x1
    fbbc:	e58d102c 	str	r1, [sp, #44]
    fbc0:	e2866004 	add	r6, r6, #4	; 0x4
    fbc4:	e59d202c 	ldr	r2, [sp, #44]
    fbc8:	e3520000 	cmp	r2, #0	; 0x0
    fbcc:	aa000001 	bge	fbd8 <vsprintf+0x188>
    fbd0:	e3a03000 	mov	r3, #0	; 0x0
    fbd4:	e58d302c 	str	r3, [sp, #44]
    fbd8:	e5d83000 	ldrb	r3, [r8]
    fbdc:	e353006c 	cmp	r3, #108	; 0x6c
    fbe0:	13530068 	cmpne	r3, #104	; 0x68
    fbe4:	0a000008 	beq	fc0c <vsprintf+0x1bc>
    fbe8:	e353004c 	cmp	r3, #76	; 0x4c
    fbec:	0a000006 	beq	fc0c <vsprintf+0x1bc>
    fbf0:	e353005a 	cmp	r3, #90	; 0x5a
    fbf4:	0a000004 	beq	fc0c <vsprintf+0x1bc>
    fbf8:	e353007a 	cmp	r3, #122	; 0x7a
    fbfc:	0a000002 	beq	fc0c <vsprintf+0x1bc>
    fc00:	e3530074 	cmp	r3, #116	; 0x74
    fc04:	13e02000 	mvnne	r2, #0	; 0x0
    fc08:	1a000007 	bne	fc2c <vsprintf+0x1dc>
    fc0c:	e353006c 	cmp	r3, #108	; 0x6c
    fc10:	e1a02003 	mov	r2, r3
    fc14:	e2888001 	add	r8, r8, #1	; 0x1
    fc18:	1a000003 	bne	fc2c <vsprintf+0x1dc>
    fc1c:	e5d83000 	ldrb	r3, [r8]
    fc20:	e353006c 	cmp	r3, #108	; 0x6c
    fc24:	02888001 	addeq	r8, r8, #1	; 0x1
    fc28:	03a0204c 	moveq	r2, #76	; 0x4c
    fc2c:	e5d83000 	ldrb	r3, [r8]
    fc30:	e353006e 	cmp	r3, #110	; 0x6e
    fc34:	0a0000df 	beq	ffb8 <vsprintf+0x568>
    fc38:	8a00000c 	bhi	fc70 <vsprintf+0x220>
    fc3c:	e3530063 	cmp	r3, #99	; 0x63
    fc40:	0a000017 	beq	fca4 <vsprintf+0x254>
    fc44:	8a000004 	bhi	fc5c <vsprintf+0x20c>
    fc48:	e3530025 	cmp	r3, #37	; 0x25
    fc4c:	0a0000ed 	beq	10008 <vsprintf+0x5b8>
    fc50:	e3530058 	cmp	r3, #88	; 0x58
    fc54:	1a0000e5 	bne	fff0 <vsprintf+0x5a0>
    fc58:	ea0000e0 	b	ffe0 <vsprintf+0x590>
    fc5c:	e3530064 	cmp	r3, #100	; 0x64
    fc60:	0a0000e0 	beq	ffe8 <vsprintf+0x598>
    fc64:	e3530069 	cmp	r3, #105	; 0x69
    fc68:	1a0000e0 	bne	fff0 <vsprintf+0x5a0>
    fc6c:	ea0000dd 	b	ffe8 <vsprintf+0x598>
    fc70:	e3530073 	cmp	r3, #115	; 0x73
    fc74:	0a000021 	beq	fd00 <vsprintf+0x2b0>
    fc78:	8a000004 	bhi	fc90 <vsprintf+0x240>
    fc7c:	e353006f 	cmp	r3, #111	; 0x6f
    fc80:	0a0000d3 	beq	ffd4 <vsprintf+0x584>
    fc84:	e3530070 	cmp	r3, #112	; 0x70
    fc88:	1a0000d8 	bne	fff0 <vsprintf+0x5a0>
    fc8c:	ea000023 	b	fd20 <vsprintf+0x2d0>
    fc90:	e3530075 	cmp	r3, #117	; 0x75
    fc94:	0a0000dd 	beq	10010 <vsprintf+0x5c0>
    fc98:	e3530078 	cmp	r3, #120	; 0x78
    fc9c:	1a0000d3 	bne	fff0 <vsprintf+0x5a0>
    fca0:	ea0000cd 	b	ffdc <vsprintf+0x58c>
    fca4:	e3170010 	tst	r7, #16	; 0x10
    fca8:	01a0300a 	moveq	r3, sl
    fcac:	03a02020 	moveq	r2, #32	; 0x20
    fcb0:	0a000001 	beq	fcbc <vsprintf+0x26c>
    fcb4:	ea000005 	b	fcd0 <vsprintf+0x280>
    fcb8:	e5432001 	strb	r2, [r3, #-1]
    fcbc:	e2499001 	sub	r9, r9, #1	; 0x1
    fcc0:	e3590000 	cmp	r9, #0	; 0x0
    fcc4:	e1a0a003 	mov	sl, r3
    fcc8:	e2833001 	add	r3, r3, #1	; 0x1
    fccc:	cafffff9 	bgt	fcb8 <vsprintf+0x268>
    fcd0:	e5963000 	ldr	r3, [r6]
    fcd4:	e1a0200a 	mov	r2, sl
    fcd8:	e4c23001 	strb	r3, [r2], #1
    fcdc:	e3a03020 	mov	r3, #32	; 0x20
    fce0:	ea000000 	b	fce8 <vsprintf+0x298>
    fce4:	e5423001 	strb	r3, [r2, #-1]
    fce8:	e2499001 	sub	r9, r9, #1	; 0x1
    fcec:	e3590000 	cmp	r9, #0	; 0x0
    fcf0:	e1a0a002 	mov	sl, r2
    fcf4:	e2822001 	add	r2, r2, #1	; 0x1
    fcf8:	cafffff9 	bgt	fce4 <vsprintf+0x294>
    fcfc:	ea0000ab 	b	ffb0 <vsprintf+0x560>
    fd00:	e58d7000 	str	r7, [sp]
    fd04:	e1a0000a 	mov	r0, sl
    fd08:	e5961000 	ldr	r1, [r6]
    fd0c:	e1a02009 	mov	r2, r9
    fd10:	e59d302c 	ldr	r3, [sp, #44]
    fd14:	ebfffe40 	bl	f61c <string>
    fd18:	e1a0a000 	mov	sl, r0
    fd1c:	ea0000a3 	b	ffb0 <vsprintf+0x560>
    fd20:	e596b000 	ldr	fp, [r6]
    fd24:	e35b0000 	cmp	fp, #0	; 0x0
    fd28:	1a000005 	bne	fd44 <vsprintf+0x2f4>
    fd2c:	e1a0000a 	mov	r0, sl
    fd30:	e1a02009 	mov	r2, r9
    fd34:	e59d302c 	ldr	r3, [sp, #44]
    fd38:	e59f13f0 	ldr	r1, [pc, #1008]	; 10130 <vsprintf+0x6e0>
    fd3c:	e58d7000 	str	r7, [sp]
    fd40:	ea000081 	b	ff4c <vsprintf+0x4fc>
    fd44:	e5d83001 	ldrb	r3, [r8, #1]
    fd48:	e2882001 	add	r2, r8, #1	; 0x1
    fd4c:	e353004d 	cmp	r3, #77	; 0x4d
    fd50:	0a000008 	beq	fd78 <vsprintf+0x328>
    fd54:	8a000002 	bhi	fd64 <vsprintf+0x314>
    fd58:	e3530049 	cmp	r3, #73	; 0x49
    fd5c:	1a00007c 	bne	ff54 <vsprintf+0x504>
    fd60:	ea000025 	b	fdfc <vsprintf+0x3ac>
    fd64:	e3530069 	cmp	r3, #105	; 0x69
    fd68:	0a000022 	beq	fdf8 <vsprintf+0x3a8>
    fd6c:	e353006d 	cmp	r3, #109	; 0x6d
    fd70:	1a000077 	bne	ff54 <vsprintf+0x504>
    fd74:	e3877040 	orr	r7, r7, #64	; 0x40
    fd78:	e1a03327 	lsr	r3, r7, #6
    fd7c:	e2233001 	eor	r3, r3, #1	; 0x1
    fd80:	e2034001 	and	r4, r3, #1	; 0x1
    fd84:	e3a0e000 	mov	lr, #0	; 0x0
    fd88:	e28dc05b 	add	ip, sp, #91	; 0x5b
    fd8c:	e3a0503a 	mov	r5, #58	; 0x3a
    fd90:	e7db200e 	ldrb	r2, [fp, lr]
    fd94:	e59f0398 	ldr	r0, [pc, #920]	; 10134 <vsprintf+0x6e4>
    fd98:	e202300f 	and	r3, r2, #15	; 0xf
    fd9c:	e7d02222 	ldrb	r2, [r0, r2, lsr #4]
    fda0:	e7d03003 	ldrb	r3, [r0, r3]
    fda4:	e1a0000c 	mov	r0, ip
    fda8:	e4c02001 	strb	r2, [r0], #1
    fdac:	e35e0005 	cmp	lr, #5	; 0x5
    fdb0:	03a01000 	moveq	r1, #0	; 0x0
    fdb4:	12041001 	andne	r1, r4, #1	; 0x1
    fdb8:	e3510000 	cmp	r1, #0	; 0x0
    fdbc:	e5cc3001 	strb	r3, [ip, #1]
    fdc0:	e28ee001 	add	lr, lr, #1	; 0x1
    fdc4:	e280c001 	add	ip, r0, #1	; 0x1
    fdc8:	128cc001 	addne	ip, ip, #1	; 0x1
    fdcc:	15c05001 	strbne	r5, [r0, #1]
    fdd0:	e35e0006 	cmp	lr, #6	; 0x6
    fdd4:	1affffed 	bne	fd90 <vsprintf+0x340>
    fdd8:	e3a03000 	mov	r3, #0	; 0x0
    fddc:	e5cc3000 	strb	r3, [ip]
    fde0:	e1a0000a 	mov	r0, sl
    fde4:	e3c7c040 	bic	ip, r7, #64	; 0x40
    fde8:	e1a02009 	mov	r2, r9
    fdec:	e59d302c 	ldr	r3, [sp, #44]
    fdf0:	e28d105b 	add	r1, sp, #91	; 0x5b
    fdf4:	ea000053 	b	ff48 <vsprintf+0x4f8>
    fdf8:	e3877040 	orr	r7, r7, #64	; 0x40
    fdfc:	e5d23001 	ldrb	r3, [r2, #1]
    fe00:	e3530036 	cmp	r3, #54	; 0x36
    fe04:	1a00002d 	bne	fec0 <vsprintf+0x470>
    fe08:	e1a03327 	lsr	r3, r7, #6
    fe0c:	e2233001 	eor	r3, r3, #1	; 0x1
    fe10:	e1a0500b 	mov	r5, fp
    fe14:	e3a04000 	mov	r4, #0	; 0x0
    fe18:	e203b001 	and	fp, r3, #1	; 0x1
    fe1c:	e28dc033 	add	ip, sp, #51	; 0x33
    fe20:	e5d52000 	ldrb	r2, [r5]
    fe24:	e59fe308 	ldr	lr, [pc, #776]	; 10134 <vsprintf+0x6e4>
    fe28:	e202300f 	and	r3, r2, #15	; 0xf
    fe2c:	e7de0003 	ldrb	r0, [lr, r3]
    fe30:	e7de3222 	ldrb	r3, [lr, r2, lsr #4]
    fe34:	e1a0100c 	mov	r1, ip
    fe38:	e4c13001 	strb	r3, [r1], #1
    fe3c:	e5cc0001 	strb	r0, [ip, #1]
    fe40:	e5d52001 	ldrb	r2, [r5, #1]
    fe44:	e59f02e8 	ldr	r0, [pc, #744]	; 10134 <vsprintf+0x6e4>
    fe48:	e202300f 	and	r3, r2, #15	; 0xf
    fe4c:	e7d0c003 	ldrb	ip, [r0, r3]
    fe50:	e59f32dc 	ldr	r3, [pc, #732]	; 10134 <vsprintf+0x6e4>
    fe54:	e3540007 	cmp	r4, #7	; 0x7
    fe58:	03a0e000 	moveq	lr, #0	; 0x0
    fe5c:	120be001 	andne	lr, fp, #1	; 0x1
    fe60:	e7d32222 	ldrb	r2, [r3, r2, lsr #4]
    fe64:	e35e0000 	cmp	lr, #0	; 0x0
    fe68:	e58d2014 	str	r2, [sp, #20]
    fe6c:	e2810001 	add	r0, r1, #1	; 0x1
    fe70:	e59de014 	ldr	lr, [sp, #20]
    fe74:	e2802001 	add	r2, r0, #1	; 0x1
    fe78:	e5c1e001 	strb	lr, [r1, #1]
    fe7c:	e2844001 	add	r4, r4, #1	; 0x1
    fe80:	e5c0c001 	strb	ip, [r0, #1]
    fe84:	13a0003a 	movne	r0, #58	; 0x3a
    fe88:	e282c001 	add	ip, r2, #1	; 0x1
    fe8c:	128cc001 	addne	ip, ip, #1	; 0x1
    fe90:	15c20001 	strbne	r0, [r2, #1]
    fe94:	e3540008 	cmp	r4, #8	; 0x8
    fe98:	e2855002 	add	r5, r5, #2	; 0x2
    fe9c:	1affffdf 	bne	fe20 <vsprintf+0x3d0>
    fea0:	e3a03000 	mov	r3, #0	; 0x0
    fea4:	e5cc3000 	strb	r3, [ip]
    fea8:	e1a0000a 	mov	r0, sl
    feac:	e3c7c040 	bic	ip, r7, #64	; 0x40
    feb0:	e1a02009 	mov	r2, r9
    feb4:	e59d302c 	ldr	r3, [sp, #44]
    feb8:	e28d1033 	add	r1, sp, #51	; 0x33
    febc:	ea000021 	b	ff48 <vsprintf+0x4f8>
    fec0:	e3530034 	cmp	r3, #52	; 0x34
    fec4:	13c77040 	bicne	r7, r7, #64	; 0x40
    fec8:	1a000021 	bne	ff54 <vsprintf+0x504>
    fecc:	e3a05000 	mov	r5, #0	; 0x0
    fed0:	e28d406d 	add	r4, sp, #109	; 0x6d
    fed4:	e7db1005 	ldrb	r1, [fp, r5]
    fed8:	e28d007d 	add	r0, sp, #125	; 0x7d
    fedc:	ebfffd9b 	bl	f550 <put_dec_trunc>
    fee0:	e59d101c 	ldr	r1, [sp, #28]
    fee4:	e2402001 	sub	r2, r0, #1	; 0x1
    fee8:	e0610000 	rsb	r0, r1, r0
    feec:	e3a01000 	mov	r1, #0	; 0x0
    fef0:	ea000002 	b	ff00 <vsprintf+0x4b0>
    fef4:	e5d23001 	ldrb	r3, [r2, #1]
    fef8:	e7c43001 	strb	r3, [r4, r1]
    fefc:	e2811001 	add	r1, r1, #1	; 0x1
    ff00:	e59d3018 	ldr	r3, [sp, #24]
    ff04:	e2422001 	sub	r2, r2, #1	; 0x1
    ff08:	e1520003 	cmp	r2, r3
    ff0c:	1afffff8 	bne	fef4 <vsprintf+0x4a4>
    ff10:	e3550003 	cmp	r5, #3	; 0x3
    ff14:	13a0e02e 	movne	lr, #46	; 0x2e
    ff18:	e0844000 	add	r4, r4, r0
    ff1c:	e2855001 	add	r5, r5, #1	; 0x1
    ff20:	14c4e001 	strbne	lr, [r4], #1
    ff24:	e3550004 	cmp	r5, #4	; 0x4
    ff28:	1affffe9 	bne	fed4 <vsprintf+0x484>
    ff2c:	e3a03000 	mov	r3, #0	; 0x0
    ff30:	e5c43000 	strb	r3, [r4]
    ff34:	e59d302c 	ldr	r3, [sp, #44]
    ff38:	e3c7c040 	bic	ip, r7, #64	; 0x40
    ff3c:	e1a0000a 	mov	r0, sl
    ff40:	e1a02009 	mov	r2, r9
    ff44:	e28d106d 	add	r1, sp, #109	; 0x6d
    ff48:	e58dc000 	str	ip, [sp]
    ff4c:	ebfffdb2 	bl	f61c <string>
    ff50:	ea00000d 	b	ff8c <vsprintf+0x53c>
    ff54:	e3a01010 	mov	r1, #16	; 0x10
    ff58:	e3790001 	cmn	r9, #1	; 0x1
    ff5c:	e58d1000 	str	r1, [sp]
    ff60:	e59d102c 	ldr	r1, [sp, #44]
    ff64:	e387c020 	orr	ip, r7, #32	; 0x20
    ff68:	02899009 	addeq	r9, r9, #9	; 0x9
    ff6c:	0387c021 	orreq	ip, r7, #33	; 0x21
    ff70:	e1a0000a 	mov	r0, sl
    ff74:	e1a0200b 	mov	r2, fp
    ff78:	e3a03000 	mov	r3, #0	; 0x0
    ff7c:	e58d9004 	str	r9, [sp, #4]
    ff80:	e58d1008 	str	r1, [sp, #8]
    ff84:	e58dc00c 	str	ip, [sp, #12]
    ff88:	ebfffe1a 	bl	f7f8 <number>
    ff8c:	e1a0a000 	mov	sl, r0
    ff90:	e1a02008 	mov	r2, r8
    ff94:	e5d23001 	ldrb	r3, [r2, #1]
    ff98:	e59fe198 	ldr	lr, [pc, #408]	; 10138 <vsprintf+0x6e8>
    ff9c:	e1a08002 	mov	r8, r2
    ffa0:	e7de3003 	ldrb	r3, [lr, r3]
    ffa4:	e2822001 	add	r2, r2, #1	; 0x1
    ffa8:	e3130007 	tst	r3, #7	; 0x7
    ffac:	1afffff8 	bne	ff94 <vsprintf+0x544>
    ffb0:	e2866004 	add	r6, r6, #4	; 0x4
    ffb4:	ea000054 	b	1010c <vsprintf+0x6bc>
    ffb8:	e59d0028 	ldr	r0, [sp, #40]
    ffbc:	e5963000 	ldr	r3, [r6]
    ffc0:	e2861004 	add	r1, r6, #4	; 0x4
    ffc4:	e060200a 	rsb	r2, r0, sl
    ffc8:	e1a06001 	mov	r6, r1
    ffcc:	e5832000 	str	r2, [r3]
    ffd0:	ea00004d 	b	1010c <vsprintf+0x6bc>
    ffd4:	e3a0c008 	mov	ip, #8	; 0x8
    ffd8:	ea00000d 	b	10014 <vsprintf+0x5c4>
    ffdc:	e3877020 	orr	r7, r7, #32	; 0x20
    ffe0:	e3a0c010 	mov	ip, #16	; 0x10
    ffe4:	ea00000a 	b	10014 <vsprintf+0x5c4>
    ffe8:	e3877002 	orr	r7, r7, #2	; 0x2
    ffec:	ea000007 	b	10010 <vsprintf+0x5c0>
    fff0:	e3a03025 	mov	r3, #37	; 0x25
    fff4:	e4ca3001 	strb	r3, [sl], #1
    fff8:	e5d83000 	ldrb	r3, [r8]
    fffc:	e3530000 	cmp	r3, #0	; 0x0
   10000:	02488001 	subeq	r8, r8, #1	; 0x1
   10004:	0a000040 	beq	1010c <vsprintf+0x6bc>
   10008:	e4ca3001 	strb	r3, [sl], #1
   1000c:	ea00003e 	b	1010c <vsprintf+0x6bc>
   10010:	e3a0c00a 	mov	ip, #10	; 0xa
   10014:	e352004c 	cmp	r2, #76	; 0x4c
   10018:	1a000009 	bne	10044 <vsprintf+0x5f4>
   1001c:	e59d1024 	ldr	r1, [sp, #36]
   10020:	e3a02000 	mov	r2, #0	; 0x0
   10024:	e0613006 	rsb	r3, r1, r6
   10028:	e3130007 	tst	r3, #7	; 0x7
   1002c:	02866004 	addeq	r6, r6, #4	; 0x4
   10030:	e896000a 	ldm	r6, {r1, r3}
   10034:	e1a05003 	mov	r5, r3
   10038:	e1824001 	orr	r4, r2, r1
   1003c:	e2866008 	add	r6, r6, #8	; 0x8
   10040:	ea000028 	b	100e8 <vsprintf+0x698>
   10044:	e352006c 	cmp	r2, #108	; 0x6c
   10048:	1a000006 	bne	10068 <vsprintf+0x618>
   1004c:	e5963000 	ldr	r3, [r6]
   10050:	e3170002 	tst	r7, #2	; 0x2
   10054:	e1a04003 	mov	r4, r3
   10058:	e3a05000 	mov	r5, #0	; 0x0
   1005c:	e2866004 	add	r6, r6, #4	; 0x4
   10060:	0a000020 	beq	100e8 <vsprintf+0x698>
   10064:	ea00001e 	b	100e4 <vsprintf+0x694>
   10068:	e352007a 	cmp	r2, #122	; 0x7a
   1006c:	1352005a 	cmpne	r2, #90	; 0x5a
   10070:	05963000 	ldreq	r3, [r6]
   10074:	02866004 	addeq	r6, r6, #4	; 0x4
   10078:	01a04003 	moveq	r4, r3
   1007c:	03a05000 	moveq	r5, #0	; 0x0
   10080:	0a000018 	beq	100e8 <vsprintf+0x698>
   10084:	e3520074 	cmp	r2, #116	; 0x74
   10088:	05963000 	ldreq	r3, [r6]
   1008c:	02866004 	addeq	r6, r6, #4	; 0x4
   10090:	0a00000b 	beq	100c4 <vsprintf+0x674>
   10094:	e3520068 	cmp	r2, #104	; 0x68
   10098:	e2861004 	add	r1, r6, #4	; 0x4
   1009c:	e5962000 	ldr	r2, [r6]
   100a0:	e2070002 	and	r0, r7, #2	; 0x2
   100a4:	1a000009 	bne	100d0 <vsprintf+0x680>
   100a8:	e6ff3072 	uxth	r3, r2
   100ac:	e3500000 	cmp	r0, #0	; 0x0
   100b0:	e1a06001 	mov	r6, r1
   100b4:	e1a04003 	mov	r4, r3
   100b8:	e3a05000 	mov	r5, #0	; 0x0
   100bc:	0a000009 	beq	100e8 <vsprintf+0x698>
   100c0:	e6bf3074 	sxth	r3, r4
   100c4:	e1a04003 	mov	r4, r3
   100c8:	e1a05fc4 	asr	r5, r4, #31
   100cc:	ea000005 	b	100e8 <vsprintf+0x698>
   100d0:	e3500000 	cmp	r0, #0	; 0x0
   100d4:	e1a06001 	mov	r6, r1
   100d8:	e1a04002 	mov	r4, r2
   100dc:	e3a05000 	mov	r5, #0	; 0x0
   100e0:	0a000000 	beq	100e8 <vsprintf+0x698>
   100e4:	e1a05fc4 	asr	r5, r4, #31
   100e8:	e59de02c 	ldr	lr, [sp, #44]
   100ec:	e1a0000a 	mov	r0, sl
   100f0:	e1a02004 	mov	r2, r4
   100f4:	e1a03005 	mov	r3, r5
   100f8:	e58dc000 	str	ip, [sp]
   100fc:	e98d4200 	stmib	sp, {r9, lr}
   10100:	e58d700c 	str	r7, [sp, #12]
   10104:	ebfffdbb 	bl	f7f8 <number>
   10108:	e1a0a000 	mov	sl, r0
   1010c:	e2888001 	add	r8, r8, #1	; 0x1
   10110:	e5d83000 	ldrb	r3, [r8]
   10114:	e3530000 	cmp	r3, #0	; 0x0
   10118:	1afffe58 	bne	fa80 <vsprintf+0x30>
   1011c:	e5ca3000 	strb	r3, [sl]
   10120:	e59d1028 	ldr	r1, [sp, #40]
   10124:	e061000a 	rsb	r0, r1, sl
   10128:	e28dd084 	add	sp, sp, #132	; 0x84
   1012c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   10130:	0001c392 	.word	0x0001c392
   10134:	00019c5e 	.word	0x00019c5e
   10138:	00019c7f 	.word	0x00019c7f

0001013c <sprintf>:
   1013c:	e92d000e 	push	{r1, r2, r3}
   10140:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   10144:	e59d1004 	ldr	r1, [sp, #4]
   10148:	e28d2008 	add	r2, sp, #8	; 0x8
   1014c:	ebfffe3f 	bl	fa50 <vsprintf>
   10150:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
   10154:	e28dd00c 	add	sp, sp, #12	; 0xc
   10158:	e12fff1e 	bx	lr

0001015c <ius_check_header>:
   1015c:	e92d4070 	push	{r4, r5, r6, lr}
   10160:	e5901000 	ldr	r1, [r0]
   10164:	e59f30e8 	ldr	r3, [pc, #232]	; 10254 <ius_check_header+0xf8>
   10168:	e24dd008 	sub	sp, sp, #8	; 0x8
   1016c:	e1510003 	cmp	r1, r3
   10170:	e1a04000 	mov	r4, r0
   10174:	0a000005 	beq	10190 <ius_check_header+0x34>
   10178:	e59f00d8 	ldr	r0, [pc, #216]	; 10258 <ius_check_header+0xfc>
   1017c:	e59f30d8 	ldr	r3, [pc, #216]	; 1025c <ius_check_header+0x100>
   10180:	e1a0e00f 	mov	lr, pc
   10184:	e593f000 	ldr	pc, [r3]
   10188:	e3e00000 	mvn	r0, #0	; 0x0
   1018c:	ea00002e 	b	1024c <ius_check_header+0xf0>
   10190:	ebffdcc5 	bl	74ac <cdbg_verify_burn>
   10194:	e3500000 	cmp	r0, #0	; 0x0
   10198:	0a000011 	beq	101e4 <ius_check_header+0x88>
   1019c:	e5943004 	ldr	r3, [r4, #4]
   101a0:	e3130002 	tst	r3, #2	; 0x2
   101a4:	1a000005 	bne	101c0 <ius_check_header+0x64>
   101a8:	e59f00b0 	ldr	r0, [pc, #176]	; 10260 <ius_check_header+0x104>
   101ac:	e59f30a8 	ldr	r3, [pc, #168]	; 1025c <ius_check_header+0x100>
   101b0:	e1a0e00f 	mov	lr, pc
   101b4:	e593f000 	ldr	pc, [r3]
   101b8:	e3e00021 	mvn	r0, #33	; 0x21
   101bc:	ea000022 	b	1024c <ius_check_header+0xf0>
   101c0:	ebffdcbd 	bl	74bc <cdbg_verify_burn_key>
   101c4:	e59f2098 	ldr	r2, [pc, #152]	; 10264 <ius_check_header+0x108>
   101c8:	e1a01000 	mov	r1, r0
   101cc:	e3a00000 	mov	r0, #0	; 0x0
   101d0:	ebffd949 	bl	66fc <ss_aes_init>
   101d4:	e2840010 	add	r0, r4, #16	; 0x10
   101d8:	e1a01000 	mov	r1, r0
   101dc:	e3a02e1f 	mov	r2, #496	; 0x1f0
   101e0:	ebffd934 	bl	66b8 <ss_aes_data>
   101e4:	e3a06000 	mov	r6, #0	; 0x0
   101e8:	e5945018 	ldr	r5, [r4, #24]
   101ec:	e3a01c02 	mov	r1, #512	; 0x200
   101f0:	e5846018 	str	r6, [r4, #24]
   101f4:	e1a00006 	mov	r0, r6
   101f8:	ebffda5d 	bl	6b74 <hash_init>
   101fc:	e3a01c02 	mov	r1, #512	; 0x200
   10200:	e1a00004 	mov	r0, r4
   10204:	ebffda35 	bl	6ae0 <hash_data>
   10208:	e28d0004 	add	r0, sp, #4	; 0x4
   1020c:	ebffda17 	bl	6a70 <hash_value>
   10210:	e59d2004 	ldr	r2, [sp, #4]
   10214:	e59f3040 	ldr	r3, [pc, #64]	; 1025c <ius_check_header+0x100>
   10218:	e1550002 	cmp	r5, r2
   1021c:	e5845018 	str	r5, [r4, #24]
   10220:	0a000005 	beq	1023c <ius_check_header+0xe0>
   10224:	e59f003c 	ldr	r0, [pc, #60]	; 10268 <ius_check_header+0x10c>
   10228:	e1a01005 	mov	r1, r5
   1022c:	e1a0e00f 	mov	lr, pc
   10230:	e593f000 	ldr	pc, [r3]
   10234:	e3e0001f 	mvn	r0, #31	; 0x1f
   10238:	ea000003 	b	1024c <ius_check_header+0xf0>
   1023c:	e59f0028 	ldr	r0, [pc, #40]	; 1026c <ius_check_header+0x110>
   10240:	e1a0e00f 	mov	lr, pc
   10244:	e593f000 	ldr	pc, [r3]
   10248:	e1a00006 	mov	r0, r6
   1024c:	e28dd008 	add	sp, sp, #8	; 0x8
   10250:	e8bd8070 	pop	{r4, r5, r6, pc}
   10254:	7375692b 	.word	0x7375692b
   10258:	0001c399 	.word	0x0001c399
   1025c:	3c00d694 	.word	0x3c00d694
   10260:	0001c3b3 	.word	0x0001c3b3
   10264:	3c00d094 	.word	0x3c00d094
   10268:	0001c3c2 	.word	0x0001c3c2
   1026c:	0001c3ea 	.word	0x0001c3ea

00010270 <printf_off>:
   10270:	e92d000f 	push	{r0, r1, r2, r3}
   10274:	e28dd010 	add	sp, sp, #16	; 0x10
   10278:	e12fff1e 	bx	lr

0001027c <disable_ctrlc>:
   1027c:	e59f300c 	ldr	r3, [pc, #12]	; 10290 <disable_ctrlc+0x14>
   10280:	e5932000 	ldr	r2, [r3]
   10284:	e5830000 	str	r0, [r3]
   10288:	e1a00002 	mov	r0, r2
   1028c:	e12fff1e 	bx	lr
   10290:	3c00d234 	.word	0x3c00d234

00010294 <had_ctrlc>:
   10294:	e59f3004 	ldr	r3, [pc, #4]	; 102a0 <had_ctrlc+0xc>
   10298:	e5930000 	ldr	r0, [r3]
   1029c:	e12fff1e 	bx	lr
   102a0:	3c00d230 	.word	0x3c00d230

000102a4 <clear_ctrlc>:
   102a4:	e59f3008 	ldr	r3, [pc, #8]	; 102b4 <clear_ctrlc+0x10>
   102a8:	e3a02000 	mov	r2, #0	; 0x0
   102ac:	e5832000 	str	r2, [r3]
   102b0:	e12fff1e 	bx	lr
   102b4:	3c00d230 	.word	0x3c00d230

000102b8 <dbg>:
   102b8:	e92d000f 	push	{r0, r1, r2, r3}
   102bc:	e28dd010 	add	sp, sp, #16	; 0x10
   102c0:	e12fff1e 	bx	lr

000102c4 <puts>:
   102c4:	eaffec36 	b	b3a4 <serial_puts>

000102c8 <vprintf>:
   102c8:	e92d4010 	push	{r4, lr}
   102cc:	e24ddd06 	sub	sp, sp, #384	; 0x180
   102d0:	e1a02001 	mov	r2, r1
   102d4:	e1a01000 	mov	r1, r0
   102d8:	e1a0000d 	mov	r0, sp
   102dc:	ebfffddb 	bl	fa50 <vsprintf>
   102e0:	e1a0000d 	mov	r0, sp
   102e4:	e1a0400d 	mov	r4, sp
   102e8:	ebffec2d 	bl	b3a4 <serial_puts>
   102ec:	e28ddd06 	add	sp, sp, #384	; 0x180
   102f0:	e8bd8010 	pop	{r4, pc}

000102f4 <printf_on>:
   102f4:	e92d000f 	push	{r0, r1, r2, r3}
   102f8:	e92d4010 	push	{r4, lr}
   102fc:	e24ddd06 	sub	sp, sp, #384	; 0x180
   10300:	e1a0000d 	mov	r0, sp
   10304:	e59d1188 	ldr	r1, [sp, #392]
   10308:	e28d2f63 	add	r2, sp, #396	; 0x18c
   1030c:	ebfffdcf 	bl	fa50 <vsprintf>
   10310:	e1a0000d 	mov	r0, sp
   10314:	e1a0400d 	mov	r4, sp
   10318:	ebffec21 	bl	b3a4 <serial_puts>
   1031c:	e28ddd06 	add	sp, sp, #384	; 0x180
   10320:	e8bd4010 	pop	{r4, lr}
   10324:	e28dd010 	add	sp, sp, #16	; 0x10
   10328:	e12fff1e 	bx	lr

0001032c <serial_printf>:
   1032c:	e92d000f 	push	{r0, r1, r2, r3}
   10330:	e92d4010 	push	{r4, lr}
   10334:	e24ddd06 	sub	sp, sp, #384	; 0x180
   10338:	e1a0000d 	mov	r0, sp
   1033c:	e59d1188 	ldr	r1, [sp, #392]
   10340:	e28d2f63 	add	r2, sp, #396	; 0x18c
   10344:	ebfffdc1 	bl	fa50 <vsprintf>
   10348:	e1a0000d 	mov	r0, sp
   1034c:	e1a0400d 	mov	r4, sp
   10350:	ebffec13 	bl	b3a4 <serial_puts>
   10354:	e28ddd06 	add	sp, sp, #384	; 0x180
   10358:	e8bd4010 	pop	{r4, lr}
   1035c:	e28dd010 	add	sp, sp, #16	; 0x10
   10360:	e12fff1e 	bx	lr

00010364 <putc>:
   10364:	eaffebf7 	b	b348 <serial_putc>

00010368 <tstc>:
   10368:	eaffebef 	b	b32c <serial_tstc>

0001036c <getc>:
   1036c:	eaffec1d 	b	b3e8 <serial_getc>

00010370 <ctrlc>:
   10370:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   10374:	e59f3040 	ldr	r3, [pc, #64]	; 103bc <ctrlc+0x4c>
   10378:	e24dd004 	sub	sp, sp, #4	; 0x4
   1037c:	e5933000 	ldr	r3, [r3]
   10380:	e3530000 	cmp	r3, #0	; 0x0
   10384:	1a000009 	bne	103b0 <ctrlc+0x40>
   10388:	ebffebe7 	bl	b32c <serial_tstc>
   1038c:	e3500000 	cmp	r0, #0	; 0x0
   10390:	0a000006 	beq	103b0 <ctrlc+0x40>
   10394:	ebffec13 	bl	b3e8 <serial_getc>
   10398:	e3500003 	cmp	r0, #3	; 0x3
   1039c:	059f301c 	ldreq	r3, [pc, #28]	; 103c0 <ctrlc+0x50>
   103a0:	03a02001 	moveq	r2, #1	; 0x1
   103a4:	05832000 	streq	r2, [r3]
   103a8:	01a00002 	moveq	r0, r2
   103ac:	0a000000 	beq	103b4 <ctrlc+0x44>
   103b0:	e3a00000 	mov	r0, #0	; 0x0
   103b4:	e28dd004 	add	sp, sp, #4	; 0x4
   103b8:	e8bd8000 	pop	{pc}
   103bc:	3c00d234 	.word	0x3c00d234
   103c0:	3c00d230 	.word	0x3c00d230

000103c4 <parse_line>:
   103c4:	e92d4010 	push	{r4, lr}
   103c8:	e3a04000 	mov	r4, #0	; 0x0
   103cc:	e1a02004 	mov	r2, r4
   103d0:	ea000000 	b	103d8 <parse_line+0x14>
   103d4:	e2800001 	add	r0, r0, #1	; 0x1
   103d8:	e5d03000 	ldrb	r3, [r0]
   103dc:	e3530009 	cmp	r3, #9	; 0x9
   103e0:	13530020 	cmpne	r3, #32	; 0x20
   103e4:	0afffffa 	beq	103d4 <parse_line+0x10>
   103e8:	e3530000 	cmp	r3, #0	; 0x0
   103ec:	0a00000c 	beq	10424 <parse_line+0x60>
   103f0:	e7810104 	str	r0, [r1, r4, lsl #2]
   103f4:	e2844001 	add	r4, r4, #1	; 0x1
   103f8:	ea000000 	b	10400 <parse_line+0x3c>
   103fc:	e2800001 	add	r0, r0, #1	; 0x1
   10400:	e5d03000 	ldrb	r3, [r0]
   10404:	e3530000 	cmp	r3, #0	; 0x0
   10408:	13530020 	cmpne	r3, #32	; 0x20
   1040c:	0a000002 	beq	1041c <parse_line+0x58>
   10410:	e3530009 	cmp	r3, #9	; 0x9
   10414:	1afffff8 	bne	103fc <parse_line+0x38>
   10418:	ea000003 	b	1042c <parse_line+0x68>
   1041c:	e3530000 	cmp	r3, #0	; 0x0
   10420:	1a000001 	bne	1042c <parse_line+0x68>
   10424:	e7813104 	str	r3, [r1, r4, lsl #2]
   10428:	ea000007 	b	1044c <parse_line+0x88>
   1042c:	e3540010 	cmp	r4, #16	; 0x10
   10430:	e5c02000 	strb	r2, [r0]
   10434:	1affffe6 	bne	103d4 <parse_line+0x10>
   10438:	e59f0014 	ldr	r0, [pc, #20]	; 10454 <parse_line+0x90>
   1043c:	e1a01004 	mov	r1, r4
   10440:	e59f3010 	ldr	r3, [pc, #16]	; 10458 <parse_line+0x94>
   10444:	e1a0e00f 	mov	lr, pc
   10448:	e593f000 	ldr	pc, [r3]
   1044c:	e1a00004 	mov	r0, r4
   10450:	e8bd8010 	pop	{r4, pc}
   10454:	0001c400 	.word	0x0001c400
   10458:	3c00d694 	.word	0x3c00d694

0001045c <run_command>:
   1045c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   10460:	e1a05000 	mov	r5, r0
   10464:	e24ddf93 	sub	sp, sp, #588	; 0x24c
   10468:	e1a0a001 	mov	sl, r1
   1046c:	ebffff8c 	bl	102a4 <clear_ctrlc>
   10470:	e3550000 	cmp	r5, #0	; 0x0
   10474:	0a0000ac 	beq	1072c <run_command+0x2d0>
   10478:	e5d53000 	ldrb	r3, [r5]
   1047c:	e3530000 	cmp	r3, #0	; 0x0
   10480:	0a0000a9 	beq	1072c <run_command+0x2d0>
   10484:	e1a00005 	mov	r0, r5
   10488:	ebffc43e 	bl	1588 <strlen>
   1048c:	e35000ff 	cmp	r0, #255	; 0xff
   10490:	9a000002 	bls	104a0 <run_command+0x44>
   10494:	e59f02a0 	ldr	r0, [pc, #672]	; 1073c <run_command+0x2e0>
   10498:	ebffff89 	bl	102c4 <puts>
   1049c:	ea0000a2 	b	1072c <run_command+0x2d0>
   104a0:	e28d4f41 	add	r4, sp, #260	; 0x104
   104a4:	e1a01005 	mov	r1, r5
   104a8:	e1a00004 	mov	r0, r4
   104ac:	e3a06000 	mov	r6, #0	; 0x0
   104b0:	ebffc3dd 	bl	142c <strcpy>
   104b4:	e3a07001 	mov	r7, #1	; 0x1
   104b8:	e1a0b006 	mov	fp, r6
   104bc:	e28d9f81 	add	r9, sp, #516	; 0x204
   104c0:	ea000091 	b	1070c <run_command+0x2b0>
   104c4:	e3510027 	cmp	r1, #39	; 0x27
   104c8:	1a000002 	bne	104d8 <run_command+0x7c>
   104cc:	e5523001 	ldrb	r3, [r2, #-1]
   104d0:	e353005c 	cmp	r3, #92	; 0x5c
   104d4:	12200001 	eorne	r0, r0, #1	; 0x1
   104d8:	e3500000 	cmp	r0, #0	; 0x0
   104dc:	1a000009 	bne	10508 <run_command+0xac>
   104e0:	e351003b 	cmp	r1, #59	; 0x3b
   104e4:	13a03000 	movne	r3, #0	; 0x0
   104e8:	03a03001 	moveq	r3, #1	; 0x1
   104ec:	e1520004 	cmp	r2, r4
   104f0:	03a03000 	moveq	r3, #0	; 0x0
   104f4:	e3530000 	cmp	r3, #0	; 0x0
   104f8:	0a000002 	beq	10508 <run_command+0xac>
   104fc:	e5523001 	ldrb	r3, [r2, #-1]
   10500:	e353005c 	cmp	r3, #92	; 0x5c
   10504:	1a000005 	bne	10520 <run_command+0xc4>
   10508:	e2822001 	add	r2, r2, #1	; 0x1
   1050c:	e5d21000 	ldrb	r1, [r2]
   10510:	e3510000 	cmp	r1, #0	; 0x0
   10514:	1affffea 	bne	104c4 <run_command+0x68>
   10518:	e1a08002 	mov	r8, r2
   1051c:	ea000001 	b	10528 <run_command+0xcc>
   10520:	e5c20000 	strb	r0, [r2]
   10524:	e2828001 	add	r8, r2, #1	; 0x1
   10528:	e1a00004 	mov	r0, r4
   1052c:	ebffc415 	bl	1588 <strlen>
   10530:	e3a0c000 	mov	ip, #0	; 0x0
   10534:	e3a0ec01 	mov	lr, #256	; 0x100
   10538:	e1a0100c 	mov	r1, ip
   1053c:	e28d5004 	add	r5, sp, #4	; 0x4
   10540:	ea000049 	b	1066c <run_command+0x210>
   10544:	e35c0003 	cmp	ip, #3	; 0x3
   10548:	e2400001 	sub	r0, r0, #1	; 0x1
   1054c:	e4d42001 	ldrb	r2, [r4], #1
   10550:	0a000035 	beq	1062c <run_command+0x1d0>
   10554:	e352005c 	cmp	r2, #92	; 0x5c
   10558:	13a03000 	movne	r3, #0	; 0x0
   1055c:	03a03001 	moveq	r3, #1	; 0x1
   10560:	e351005c 	cmp	r1, #92	; 0x5c
   10564:	03a03000 	moveq	r3, #0	; 0x0
   10568:	e3530000 	cmp	r3, #0	; 0x0
   1056c:	0a000004 	beq	10584 <run_command+0x128>
   10570:	e3500000 	cmp	r0, #0	; 0x0
   10574:	0a00003f 	beq	10678 <run_command+0x21c>
   10578:	e4d42001 	ldrb	r2, [r4], #1
   1057c:	e2400001 	sub	r0, r0, #1	; 0x1
   10580:	e3a0105c 	mov	r1, #92	; 0x5c
   10584:	e35c0003 	cmp	ip, #3	; 0x3
   10588:	979ff10c 	ldrls	pc, [pc, ip, lsl #2]
   1058c:	ea000035 	b	10668 <run_command+0x20c>
   10590:	000105a0 	.word	0x000105a0
   10594:	000105e4 	.word	0x000105e4
   10598:	0001061c 	.word	0x0001061c
   1059c:	0001062c 	.word	0x0001062c
   105a0:	e251105c 	subs	r1, r1, #92	; 0x5c
   105a4:	13a01001 	movne	r1, #1	; 0x1
   105a8:	e3520027 	cmp	r2, #39	; 0x27
   105ac:	13a03000 	movne	r3, #0	; 0x0
   105b0:	02013001 	andeq	r3, r1, #1	; 0x1
   105b4:	e3530000 	cmp	r3, #0	; 0x0
   105b8:	13a0c003 	movne	ip, #3	; 0x3
   105bc:	1a000022 	bne	1064c <run_command+0x1f0>
   105c0:	e3520024 	cmp	r2, #36	; 0x24
   105c4:	13a03000 	movne	r3, #0	; 0x0
   105c8:	02013001 	andeq	r3, r1, #1	; 0x1
   105cc:	e3530000 	cmp	r3, #0	; 0x0
   105d0:	13a0c001 	movne	ip, #1	; 0x1
   105d4:	13a02024 	movne	r2, #36	; 0x24
   105d8:	04c52001 	strbeq	r2, [r5], #1
   105dc:	024ee001 	subeq	lr, lr, #1	; 0x1
   105e0:	ea000020 	b	10668 <run_command+0x20c>
   105e4:	e352007b 	cmp	r2, #123	; 0x7b
   105e8:	13520028 	cmpne	r2, #40	; 0x28
   105ec:	13a03000 	movne	r3, #0	; 0x0
   105f0:	03a03001 	moveq	r3, #1	; 0x1
   105f4:	03a0c002 	moveq	ip, #2	; 0x2
   105f8:	0a00001a 	beq	10668 <run_command+0x20c>
   105fc:	e3a01024 	mov	r1, #36	; 0x24
   10600:	e25ee001 	subs	lr, lr, #1	; 0x1
   10604:	e4c51001 	strb	r1, [r5], #1
   10608:	0a000015 	beq	10664 <run_command+0x208>
   1060c:	e4c52001 	strb	r2, [r5], #1
   10610:	e24ee001 	sub	lr, lr, #1	; 0x1
   10614:	e1a0c003 	mov	ip, r3
   10618:	ea000012 	b	10668 <run_command+0x20c>
   1061c:	e352007d 	cmp	r2, #125	; 0x7d
   10620:	13520029 	cmpne	r2, #41	; 0x29
   10624:	0a00000e 	beq	10664 <run_command+0x208>
   10628:	ea00000e 	b	10668 <run_command+0x20c>
   1062c:	e3520027 	cmp	r2, #39	; 0x27
   10630:	13a03000 	movne	r3, #0	; 0x0
   10634:	03a03001 	moveq	r3, #1	; 0x1
   10638:	e351005c 	cmp	r1, #92	; 0x5c
   1063c:	03a03000 	moveq	r3, #0	; 0x0
   10640:	e3530000 	cmp	r3, #0	; 0x0
   10644:	0a000002 	beq	10654 <run_command+0x1f8>
   10648:	e3a0c000 	mov	ip, #0	; 0x0
   1064c:	e3a02027 	mov	r2, #39	; 0x27
   10650:	ea000004 	b	10668 <run_command+0x20c>
   10654:	e4c52001 	strb	r2, [r5], #1
   10658:	e24ee001 	sub	lr, lr, #1	; 0x1
   1065c:	e3a0c003 	mov	ip, #3	; 0x3
   10660:	ea000000 	b	10668 <run_command+0x20c>
   10664:	e3a0c000 	mov	ip, #0	; 0x0
   10668:	e1a01002 	mov	r1, r2
   1066c:	e3500000 	cmp	r0, #0	; 0x0
   10670:	135e0000 	cmpne	lr, #0	; 0x0
   10674:	1affffb2 	bne	10544 <run_command+0xe8>
   10678:	e35e0000 	cmp	lr, #0	; 0x0
   1067c:	15c5b000 	strbne	fp, [r5]
   10680:	0545e001 	strbeq	lr, [r5, #-1]
   10684:	e28d0004 	add	r0, sp, #4	; 0x4
   10688:	e1a01009 	mov	r1, r9
   1068c:	ebffff4c 	bl	103c4 <parse_line>
   10690:	e2505000 	subs	r5, r0, #0	; 0x0
   10694:	0a00000d 	beq	106d0 <run_command+0x274>
   10698:	e59d0204 	ldr	r0, [sp, #516]
   1069c:	ebffca6c 	bl	3054 <find_cmd>
   106a0:	e2504000 	subs	r4, r0, #0	; 0x0
   106a4:	1a000005 	bne	106c0 <run_command+0x264>
   106a8:	e59f0090 	ldr	r0, [pc, #144]	; 10740 <run_command+0x2e4>
   106ac:	e59d1204 	ldr	r1, [sp, #516]
   106b0:	e59f308c 	ldr	r3, [pc, #140]	; 10744 <run_command+0x2e8>
   106b4:	e1a0e00f 	mov	lr, pc
   106b8:	e593f000 	ldr	pc, [r3]
   106bc:	ea000003 	b	106d0 <run_command+0x274>
   106c0:	e5943004 	ldr	r3, [r4, #4]
   106c4:	e1550003 	cmp	r5, r3
   106c8:	da000002 	ble	106d8 <run_command+0x27c>
   106cc:	ebffca1a 	bl	2f3c <cmd_usage>
   106d0:	e3e06000 	mvn	r6, #0	; 0x0
   106d4:	ea00000b 	b	10708 <run_command+0x2ac>
   106d8:	e1a02005 	mov	r2, r5
   106dc:	e1a0100a 	mov	r1, sl
   106e0:	e1a03009 	mov	r3, r9
   106e4:	e1a0e00f 	mov	lr, pc
   106e8:	e594f00c 	ldr	pc, [r4, #12]
   106ec:	e3500000 	cmp	r0, #0	; 0x0
   106f0:	13e06000 	mvnne	r6, #0	; 0x0
   106f4:	e5944008 	ldr	r4, [r4, #8]
   106f8:	ebfffee5 	bl	10294 <had_ctrlc>
   106fc:	e3500000 	cmp	r0, #0	; 0x0
   10700:	1a000009 	bne	1072c <run_command+0x2d0>
   10704:	e0077004 	and	r7, r7, r4
   10708:	e1a04008 	mov	r4, r8
   1070c:	e5d43000 	ldrb	r3, [r4]
   10710:	e3530000 	cmp	r3, #0	; 0x0
   10714:	11a02004 	movne	r2, r4
   10718:	13a00000 	movne	r0, #0	; 0x0
   1071c:	1affff7a 	bne	1050c <run_command+0xb0>
   10720:	e3560000 	cmp	r6, #0	; 0x0
   10724:	01a06007 	moveq	r6, r7
   10728:	ea000000 	b	10730 <run_command+0x2d4>
   1072c:	e3e06000 	mvn	r6, #0	; 0x0
   10730:	e1a00006 	mov	r0, r6
   10734:	e28ddf93 	add	sp, sp, #588	; 0x24c
   10738:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   1073c:	0001c41f 	.word	0x0001c41f
   10740:	0001c435 	.word	0x0001c435
   10744:	3c00d694 	.word	0x3c00d694

00010748 <cread_add_char>:
   10748:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   1074c:	e3510000 	cmp	r1, #0	; 0x0
   10750:	e24dd004 	sub	sp, sp, #4	; 0x4
   10754:	e1a07000 	mov	r7, r0
   10758:	e1a04002 	mov	r4, r2
   1075c:	e1a00003 	mov	r0, r3
   10760:	e59d6018 	ldr	r6, [sp, #24]
   10764:	e59dc01c 	ldr	ip, [sp, #28]
   10768:	1a000003 	bne	1077c <cread_add_char+0x34>
   1076c:	e5922000 	ldr	r2, [r2]
   10770:	e5933000 	ldr	r3, [r3]
   10774:	e1520003 	cmp	r2, r3
   10778:	1a000026 	bne	10818 <cread_add_char+0xd0>
   1077c:	e5902000 	ldr	r2, [r0]
   10780:	e24c3001 	sub	r3, ip, #1	; 0x1
   10784:	e1520003 	cmp	r2, r3
   10788:	9a000003 	bls	1079c <cread_add_char+0x54>
   1078c:	e3a00007 	mov	r0, #7	; 0x7
   10790:	e28dd004 	add	sp, sp, #4	; 0x4
   10794:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
   10798:	eafffef1 	b	10364 <putc>
   1079c:	e3510000 	cmp	r1, #0	; 0x0
   107a0:	e2821001 	add	r1, r2, #1	; 0x1
   107a4:	e5801000 	str	r1, [r0]
   107a8:	0a00001a 	beq	10818 <cread_add_char+0xd0>
   107ac:	e5943000 	ldr	r3, [r4]
   107b0:	e0635001 	rsb	r5, r3, r1
   107b4:	e3550001 	cmp	r5, #1	; 0x1
   107b8:	9a000004 	bls	107d0 <cread_add_char+0x88>
   107bc:	e2830001 	add	r0, r3, #1	; 0x1
   107c0:	e0860000 	add	r0, r6, r0
   107c4:	e0861003 	add	r1, r6, r3
   107c8:	e2452001 	sub	r2, r5, #1	; 0x1
   107cc:	ebffc390 	bl	1614 <memmove>
   107d0:	e5943000 	ldr	r3, [r4]
   107d4:	e59f0074 	ldr	r0, [pc, #116]	; 10850 <cread_add_char+0x108>
   107d8:	e7c67003 	strb	r7, [r6, r3]
   107dc:	e5942000 	ldr	r2, [r4]
   107e0:	e59f306c 	ldr	r3, [pc, #108]	; 10854 <cread_add_char+0x10c>
   107e4:	e0862002 	add	r2, r6, r2
   107e8:	e1a01005 	mov	r1, r5
   107ec:	e1a0e00f 	mov	lr, pc
   107f0:	e593f000 	ldr	pc, [r3]
   107f4:	e5943000 	ldr	r3, [r4]
   107f8:	e2833001 	add	r3, r3, #1	; 0x1
   107fc:	e5843000 	str	r3, [r4]
   10800:	ea000000 	b	10808 <cread_add_char+0xc0>
   10804:	ebfffed6 	bl	10364 <putc>
   10808:	e2555001 	subs	r5, r5, #1	; 0x1
   1080c:	e3a00008 	mov	r0, #8	; 0x8
   10810:	1afffffb 	bne	10804 <cread_add_char+0xbc>
   10814:	ea00000b 	b	10848 <cread_add_char+0x100>
   10818:	e5943000 	ldr	r3, [r4]
   1081c:	e59f002c 	ldr	r0, [pc, #44]	; 10850 <cread_add_char+0x108>
   10820:	e7c67003 	strb	r7, [r6, r3]
   10824:	e5942000 	ldr	r2, [r4]
   10828:	e59f3024 	ldr	r3, [pc, #36]	; 10854 <cread_add_char+0x10c>
   1082c:	e0862002 	add	r2, r6, r2
   10830:	e3a01001 	mov	r1, #1	; 0x1
   10834:	e1a0e00f 	mov	lr, pc
   10838:	e593f000 	ldr	pc, [r3]
   1083c:	e5943000 	ldr	r3, [r4]
   10840:	e2833001 	add	r3, r3, #1	; 0x1
   10844:	e5843000 	str	r3, [r4]
   10848:	e28dd004 	add	sp, sp, #4	; 0x4
   1084c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   10850:	0001c458 	.word	0x0001c458
   10854:	3c00d694 	.word	0x3c00d694

00010858 <readline_into_buffer>:
   10858:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   1085c:	e59f367c 	ldr	r3, [pc, #1660]	; 10ee0 <readline_into_buffer+0x688>
   10860:	e24dd01c 	sub	sp, sp, #28	; 0x1c
   10864:	e593e000 	ldr	lr, [r3]
   10868:	e1a06001 	mov	r6, r1
   1086c:	e35e0000 	cmp	lr, #0	; 0x0
   10870:	1a000014 	bne	108c8 <readline_into_buffer+0x70>
   10874:	e59f3668 	ldr	r3, [pc, #1640]	; 10ee4 <readline_into_buffer+0x68c>
   10878:	e3e02000 	mvn	r2, #0	; 0x0
   1087c:	e5832000 	str	r2, [r3]
   10880:	e59f3660 	ldr	r3, [pc, #1632]	; 10ee8 <readline_into_buffer+0x690>
   10884:	e59fc660 	ldr	ip, [pc, #1632]	; 10eec <readline_into_buffer+0x694>
   10888:	e583e000 	str	lr, [r3]
   1088c:	e59f365c 	ldr	r3, [pc, #1628]	; 10ef0 <readline_into_buffer+0x698>
   10890:	e59f465c 	ldr	r4, [pc, #1628]	; 10ef4 <readline_into_buffer+0x69c>
   10894:	e583e000 	str	lr, [r3]
   10898:	e59f3658 	ldr	r3, [pc, #1624]	; 10ef8 <readline_into_buffer+0x6a0>
   1089c:	e1a0100e 	mov	r1, lr
   108a0:	e583e000 	str	lr, [r3]
   108a4:	e1a0300e 	mov	r3, lr
   108a8:	e784c001 	str	ip, [r4, r1]
   108ac:	e2811004 	add	r1, r1, #4	; 0x4
   108b0:	e3510010 	cmp	r1, #16	; 0x10
   108b4:	e4cc3100 	strb	r3, [ip], #256
   108b8:	1afffffa 	bne	108a8 <readline_into_buffer+0x50>
   108bc:	e59f361c 	ldr	r3, [pc, #1564]	; 10ee0 <readline_into_buffer+0x688>
   108c0:	e3a02001 	mov	r2, #1	; 0x1
   108c4:	e5832000 	str	r2, [r3]
   108c8:	ebfffe7d 	bl	102c4 <puts>
   108cc:	e3a03000 	mov	r3, #0	; 0x0
   108d0:	e58d3014 	str	r3, [sp, #20]
   108d4:	e58d3010 	str	r3, [sp, #16]
   108d8:	e1a05003 	mov	r5, r3
   108dc:	e3a09001 	mov	r9, #1	; 0x1
   108e0:	e59fb614 	ldr	fp, [pc, #1556]	; 10efc <readline_into_buffer+0x6a4>
   108e4:	e59fa5f8 	ldr	sl, [pc, #1528]	; 10ee4 <readline_into_buffer+0x68c>
   108e8:	ea000000 	b	108f0 <readline_into_buffer+0x98>
   108ec:	e3a05000 	mov	r5, #0	; 0x0
   108f0:	ebfffe9d 	bl	1036c <getc>
   108f4:	e6ef0070 	uxtb	r0, r0
   108f8:	e350000d 	cmp	r0, #13	; 0xd
   108fc:	1350000a 	cmpne	r0, #10	; 0xa
   10900:	13a04000 	movne	r4, #0	; 0x0
   10904:	03a04001 	moveq	r4, #1	; 0x1
   10908:	1a00000b 	bne	1093c <readline_into_buffer+0xe4>
   1090c:	e59f35e8 	ldr	r3, [pc, #1512]	; 10efc <readline_into_buffer+0x6a4>
   10910:	e59f05e8 	ldr	r0, [pc, #1512]	; 10f00 <readline_into_buffer+0x6a8>
   10914:	e1a0e00f 	mov	lr, pc
   10918:	e593f000 	ldr	pc, [r3]
   1091c:	e59d5010 	ldr	r5, [sp, #16]
   10920:	e3a03000 	mov	r3, #0	; 0x0
   10924:	e7c63005 	strb	r3, [r6, r5]
   10928:	e5d63000 	ldrb	r3, [r6]
   1092c:	e3530000 	cmp	r3, #0	; 0x0
   10930:	13530021 	cmpne	r3, #33	; 0x21
   10934:	0a000162 	beq	10ec4 <readline_into_buffer+0x66c>
   10938:	ea00014e 	b	10e78 <readline_into_buffer+0x620>
   1093c:	e3550000 	cmp	r5, #0	; 0x0
   10940:	0a000033 	beq	10a14 <readline_into_buffer+0x1bc>
   10944:	e3550001 	cmp	r5, #1	; 0x1
   10948:	1a00000c 	bne	10980 <readline_into_buffer+0x128>
   1094c:	e350005b 	cmp	r0, #91	; 0x5b
   10950:	05cd0009 	strbeq	r0, [sp, #9]
   10954:	03a05002 	moveq	r5, #2	; 0x2
   10958:	0affffe4 	beq	108f0 <readline_into_buffer+0x98>
   1095c:	e3a0cc01 	mov	ip, #256	; 0x100
   10960:	e5dd0008 	ldrb	r0, [sp, #8]
   10964:	e1a01009 	mov	r1, r9
   10968:	e28d2014 	add	r2, sp, #20	; 0x14
   1096c:	e28d3010 	add	r3, sp, #16	; 0x10
   10970:	e88d1040 	stm	sp, {r6, ip}
   10974:	e1a05004 	mov	r5, r4
   10978:	ebffff72 	bl	10748 <cread_add_char>
   1097c:	eaffffdb 	b	108f0 <readline_into_buffer+0x98>
   10980:	e2403041 	sub	r3, r0, #65	; 0x41
   10984:	e3530007 	cmp	r3, #7	; 0x7
   10988:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   1098c:	ea000009 	b	109b8 <readline_into_buffer+0x160>
   10990:	00010cfc 	.word	0x00010cfc
   10994:	00010d3c 	.word	0x00010d3c
   10998:	00010a04 	.word	0x00010a04
   1099c:	00010b10 	.word	0x00010b10
   109a0:	000109b8 	.word	0x000109b8
   109a4:	000109b8 	.word	0x000109b8
   109a8:	000109b8 	.word	0x000109b8
   109ac:	000109b0 	.word	0x000109b0
   109b0:	e3a05000 	mov	r5, #0	; 0x0
   109b4:	ea000043 	b	10ac8 <readline_into_buffer+0x270>
   109b8:	e28d1018 	add	r1, sp, #24	; 0x18
   109bc:	e0813005 	add	r3, r1, r5
   109c0:	e5430010 	strb	r0, [r3, #-16]
   109c4:	e2855001 	add	r5, r5, #1	; 0x1
   109c8:	e3a04000 	mov	r4, #0	; 0x0
   109cc:	e28d8010 	add	r8, sp, #16	; 0x10
   109d0:	e3a07c01 	mov	r7, #256	; 0x100
   109d4:	ea000004 	b	109ec <readline_into_buffer+0x194>
   109d8:	e28dc008 	add	ip, sp, #8	; 0x8
   109dc:	e7dc0004 	ldrb	r0, [ip, r4]
   109e0:	e88d00c0 	stm	sp, {r6, r7}
   109e4:	ebffff57 	bl	10748 <cread_add_char>
   109e8:	e2844001 	add	r4, r4, #1	; 0x1
   109ec:	e1540005 	cmp	r4, r5
   109f0:	e1a01009 	mov	r1, r9
   109f4:	e28d2014 	add	r2, sp, #20	; 0x14
   109f8:	e1a03008 	mov	r3, r8
   109fc:	1afffff5 	bne	109d8 <readline_into_buffer+0x180>
   10a00:	eaffffb9 	b	108ec <readline_into_buffer+0x94>
   10a04:	e3a00006 	mov	r0, #6	; 0x6
   10a08:	e350000b 	cmp	r0, #11	; 0xb
   10a0c:	e3a05000 	mov	r5, #0	; 0x0
   10a10:	ea000001 	b	10a1c <readline_into_buffer+0x1c4>
   10a14:	e350000b 	cmp	r0, #11	; 0xb
   10a18:	0a00005f 	beq	10b9c <readline_into_buffer+0x344>
   10a1c:	8a00000d 	bhi	10a58 <readline_into_buffer+0x200>
   10a20:	e3500004 	cmp	r0, #4	; 0x4
   10a24:	0a000042 	beq	10b34 <readline_into_buffer+0x2dc>
   10a28:	8a000005 	bhi	10a44 <readline_into_buffer+0x1ec>
   10a2c:	e3500002 	cmp	r0, #2	; 0x2
   10a30:	0a000037 	beq	10b14 <readline_into_buffer+0x2bc>
   10a34:	8a000028 	bhi	10adc <readline_into_buffer+0x284>
   10a38:	e3500001 	cmp	r0, #1	; 0x1
   10a3c:	1a000106 	bne	10e5c <readline_into_buffer+0x604>
   10a40:	ea000020 	b	10ac8 <readline_into_buffer+0x270>
   10a44:	e3500006 	cmp	r0, #6	; 0x6
   10a48:	0a000027 	beq	10aec <readline_into_buffer+0x294>
   10a4c:	3a000066 	bcc	10bec <readline_into_buffer+0x394>
   10a50:	e3500008 	cmp	r0, #8	; 0x8
   10a54:	ea000015 	b	10ab0 <readline_into_buffer+0x258>
   10a58:	e3500015 	cmp	r0, #21	; 0x15
   10a5c:	0a000070 	beq	10c24 <readline_into_buffer+0x3cc>
   10a60:	8a000007 	bhi	10a84 <readline_into_buffer+0x22c>
   10a64:	e350000f 	cmp	r0, #15	; 0xf
   10a68:	02299001 	eoreq	r9, r9, #1	; 0x1
   10a6c:	0affff9f 	beq	108f0 <readline_into_buffer+0x98>
   10a70:	e3500010 	cmp	r0, #16	; 0x10
   10a74:	0a0000a0 	beq	10cfc <readline_into_buffer+0x4a4>
   10a78:	e350000e 	cmp	r0, #14	; 0xe
   10a7c:	1a0000f6 	bne	10e5c <readline_into_buffer+0x604>
   10a80:	ea0000ad 	b	10d3c <readline_into_buffer+0x4e4>
   10a84:	e350001b 	cmp	r0, #27	; 0x1b
   10a88:	05cd0008 	strbeq	r0, [sp, #8]
   10a8c:	03a05001 	moveq	r5, #1	; 0x1
   10a90:	0affff96 	beq	108f0 <readline_into_buffer+0x98>
   10a94:	8a000002 	bhi	10aa4 <readline_into_buffer+0x24c>
   10a98:	e3500018 	cmp	r0, #24	; 0x18
   10a9c:	1a0000ee 	bne	10e5c <readline_into_buffer+0x604>
   10aa0:	ea00005f 	b	10c24 <readline_into_buffer+0x3cc>
   10aa4:	e350007f 	cmp	r0, #127	; 0x7f
   10aa8:	0a000075 	beq	10c84 <readline_into_buffer+0x42c>
   10aac:	e35000ff 	cmp	r0, #255	; 0xff
   10ab0:	1a0000e9 	bne	10e5c <readline_into_buffer+0x604>
   10ab4:	ea000072 	b	10c84 <readline_into_buffer+0x42c>
   10ab8:	ebfffe29 	bl	10364 <putc>
   10abc:	e59d3014 	ldr	r3, [sp, #20]
   10ac0:	e2433001 	sub	r3, r3, #1	; 0x1
   10ac4:	e58d3014 	str	r3, [sp, #20]
   10ac8:	e59d3014 	ldr	r3, [sp, #20]
   10acc:	e3a00008 	mov	r0, #8	; 0x8
   10ad0:	e3530000 	cmp	r3, #0	; 0x0
   10ad4:	1afffff7 	bne	10ab8 <readline_into_buffer+0x260>
   10ad8:	eaffff84 	b	108f0 <readline_into_buffer+0x98>
   10adc:	e3a03000 	mov	r3, #0	; 0x0
   10ae0:	e5c63000 	strb	r3, [r6]
   10ae4:	e3e00000 	mvn	r0, #0	; 0x0
   10ae8:	ea0000fa 	b	10ed8 <readline_into_buffer+0x680>
   10aec:	e59d2014 	ldr	r2, [sp, #20]
   10af0:	e59d3010 	ldr	r3, [sp, #16]
   10af4:	e1520003 	cmp	r2, r3
   10af8:	2affff7c 	bcs	108f0 <readline_into_buffer+0x98>
   10afc:	e7d60002 	ldrb	r0, [r6, r2]
   10b00:	ebfffe17 	bl	10364 <putc>
   10b04:	e59d3014 	ldr	r3, [sp, #20]
   10b08:	e2833001 	add	r3, r3, #1	; 0x1
   10b0c:	ea0000d0 	b	10e54 <readline_into_buffer+0x5fc>
   10b10:	e3a05000 	mov	r5, #0	; 0x0
   10b14:	e59d3014 	ldr	r3, [sp, #20]
   10b18:	e3530000 	cmp	r3, #0	; 0x0
   10b1c:	0affff73 	beq	108f0 <readline_into_buffer+0x98>
   10b20:	e3a00008 	mov	r0, #8	; 0x8
   10b24:	ebfffe0e 	bl	10364 <putc>
   10b28:	e59d3014 	ldr	r3, [sp, #20]
   10b2c:	e2433001 	sub	r3, r3, #1	; 0x1
   10b30:	ea0000c7 	b	10e54 <readline_into_buffer+0x5fc>
   10b34:	e59d2014 	ldr	r2, [sp, #20]
   10b38:	e59d3010 	ldr	r3, [sp, #16]
   10b3c:	e1520003 	cmp	r2, r3
   10b40:	2affff6a 	bcs	108f0 <readline_into_buffer+0x98>
   10b44:	e2433001 	sub	r3, r3, #1	; 0x1
   10b48:	e0534002 	subs	r4, r3, r2
   10b4c:	0a00000a 	beq	10b7c <readline_into_buffer+0x324>
   10b50:	e2821001 	add	r1, r2, #1	; 0x1
   10b54:	e0861001 	add	r1, r6, r1
   10b58:	e0860002 	add	r0, r6, r2
   10b5c:	e1a02004 	mov	r2, r4
   10b60:	ebffc2ab 	bl	1614 <memmove>
   10b64:	e59d2014 	ldr	r2, [sp, #20]
   10b68:	e59f0394 	ldr	r0, [pc, #916]	; 10f04 <readline_into_buffer+0x6ac>
   10b6c:	e0862002 	add	r2, r6, r2
   10b70:	e1a01004 	mov	r1, r4
   10b74:	e1a0e00f 	mov	lr, pc
   10b78:	e59bf000 	ldr	pc, [fp]
   10b7c:	e3a00020 	mov	r0, #32	; 0x20
   10b80:	ebfffdf7 	bl	10364 <putc>
   10b84:	e2444001 	sub	r4, r4, #1	; 0x1
   10b88:	e3a00008 	mov	r0, #8	; 0x8
   10b8c:	ebfffdf4 	bl	10364 <putc>
   10b90:	e3740001 	cmn	r4, #1	; 0x1
   10b94:	1afffffa 	bne	10b84 <readline_into_buffer+0x32c>
   10b98:	ea000053 	b	10cec <readline_into_buffer+0x494>
   10b9c:	e59d2014 	ldr	r2, [sp, #20]
   10ba0:	e59d3010 	ldr	r3, [sp, #16]
   10ba4:	e1520003 	cmp	r2, r3
   10ba8:	31a04002 	movcc	r4, r2
   10bac:	3a000002 	bcc	10bbc <readline_into_buffer+0x364>
   10bb0:	eaffff4e 	b	108f0 <readline_into_buffer+0x98>
   10bb4:	ebfffdea 	bl	10364 <putc>
   10bb8:	e2844001 	add	r4, r4, #1	; 0x1
   10bbc:	e59d3010 	ldr	r3, [sp, #16]
   10bc0:	e3a00020 	mov	r0, #32	; 0x20
   10bc4:	e1540003 	cmp	r4, r3
   10bc8:	3afffff9 	bcc	10bb4 <readline_into_buffer+0x35c>
   10bcc:	ea000000 	b	10bd4 <readline_into_buffer+0x37c>
   10bd0:	ebfffde3 	bl	10364 <putc>
   10bd4:	e59d2014 	ldr	r2, [sp, #20]
   10bd8:	e3a00008 	mov	r0, #8	; 0x8
   10bdc:	e1540002 	cmp	r4, r2
   10be0:	e2444001 	sub	r4, r4, #1	; 0x1
   10be4:	8afffff9 	bhi	10bd0 <readline_into_buffer+0x378>
   10be8:	ea000023 	b	10c7c <readline_into_buffer+0x424>
   10bec:	e59d3014 	ldr	r3, [sp, #20]
   10bf0:	e59d1010 	ldr	r1, [sp, #16]
   10bf4:	e1530001 	cmp	r3, r1
   10bf8:	2affff3c 	bcs	108f0 <readline_into_buffer+0x98>
   10bfc:	e0631001 	rsb	r1, r3, r1
   10c00:	e0862003 	add	r2, r6, r3
   10c04:	e59f02f8 	ldr	r0, [pc, #760]	; 10f04 <readline_into_buffer+0x6ac>
   10c08:	e1a0e00f 	mov	lr, pc
   10c0c:	e59bf000 	ldr	pc, [fp]
   10c10:	ea00008e 	b	10e50 <readline_into_buffer+0x5f8>
   10c14:	ebfffdd2 	bl	10364 <putc>
   10c18:	e59d3014 	ldr	r3, [sp, #20]
   10c1c:	e2433001 	sub	r3, r3, #1	; 0x1
   10c20:	e58d3014 	str	r3, [sp, #20]
   10c24:	e59d2014 	ldr	r2, [sp, #20]
   10c28:	e3a00008 	mov	r0, #8	; 0x8
   10c2c:	e3520000 	cmp	r2, #0	; 0x0
   10c30:	1afffff7 	bne	10c14 <readline_into_buffer+0x3bc>
   10c34:	e59d3010 	ldr	r3, [sp, #16]
   10c38:	e3530000 	cmp	r3, #0	; 0x0
   10c3c:	11a04002 	movne	r4, r2
   10c40:	1a000002 	bne	10c50 <readline_into_buffer+0x3f8>
   10c44:	eaffff29 	b	108f0 <readline_into_buffer+0x98>
   10c48:	ebfffdc5 	bl	10364 <putc>
   10c4c:	e2844001 	add	r4, r4, #1	; 0x1
   10c50:	e59d3010 	ldr	r3, [sp, #16]
   10c54:	e3a00020 	mov	r0, #32	; 0x20
   10c58:	e1540003 	cmp	r4, r3
   10c5c:	3afffff9 	bcc	10c48 <readline_into_buffer+0x3f0>
   10c60:	ea000000 	b	10c68 <readline_into_buffer+0x410>
   10c64:	ebfffdbe 	bl	10364 <putc>
   10c68:	e59d2014 	ldr	r2, [sp, #20]
   10c6c:	e3a00008 	mov	r0, #8	; 0x8
   10c70:	e1540002 	cmp	r4, r2
   10c74:	e2444001 	sub	r4, r4, #1	; 0x1
   10c78:	8afffff9 	bhi	10c64 <readline_into_buffer+0x40c>
   10c7c:	e58d2010 	str	r2, [sp, #16]
   10c80:	eaffff1a 	b	108f0 <readline_into_buffer+0x98>
   10c84:	e59d0014 	ldr	r0, [sp, #20]
   10c88:	e3500000 	cmp	r0, #0	; 0x0
   10c8c:	0affff17 	beq	108f0 <readline_into_buffer+0x98>
   10c90:	e59d2010 	ldr	r2, [sp, #16]
   10c94:	e2403001 	sub	r3, r0, #1	; 0x1
   10c98:	e0604002 	rsb	r4, r0, r2
   10c9c:	e0861000 	add	r1, r6, r0
   10ca0:	e1a02004 	mov	r2, r4
   10ca4:	e0860003 	add	r0, r6, r3
   10ca8:	e58d3014 	str	r3, [sp, #20]
   10cac:	ebffc258 	bl	1614 <memmove>
   10cb0:	e3a00008 	mov	r0, #8	; 0x8
   10cb4:	ebfffdaa 	bl	10364 <putc>
   10cb8:	e59d2014 	ldr	r2, [sp, #20]
   10cbc:	e59f0240 	ldr	r0, [pc, #576]	; 10f04 <readline_into_buffer+0x6ac>
   10cc0:	e0862002 	add	r2, r6, r2
   10cc4:	e1a01004 	mov	r1, r4
   10cc8:	e1a0e00f 	mov	lr, pc
   10ccc:	e59bf000 	ldr	pc, [fp]
   10cd0:	e3a00020 	mov	r0, #32	; 0x20
   10cd4:	ebfffda2 	bl	10364 <putc>
   10cd8:	e2444001 	sub	r4, r4, #1	; 0x1
   10cdc:	e3a00008 	mov	r0, #8	; 0x8
   10ce0:	ebfffd9f 	bl	10364 <putc>
   10ce4:	e3740001 	cmn	r4, #1	; 0x1
   10ce8:	1afffffa 	bne	10cd8 <readline_into_buffer+0x480>
   10cec:	e59d3010 	ldr	r3, [sp, #16]
   10cf0:	e2433001 	sub	r3, r3, #1	; 0x1
   10cf4:	e58d3010 	str	r3, [sp, #16]
   10cf8:	eafffefc 	b	108f0 <readline_into_buffer+0x98>
   10cfc:	e59a2000 	ldr	r2, [sl]
   10d00:	e3520000 	cmp	r2, #0	; 0x0
   10d04:	ba000024 	blt	10d9c <readline_into_buffer+0x544>
   10d08:	e2423001 	sub	r3, r2, #1	; 0x1
   10d0c:	e3730001 	cmn	r3, #1	; 0x1
   10d10:	059f01d0 	ldreq	r0, [pc, #464]	; 10ee8 <readline_into_buffer+0x690>
   10d14:	e58a3000 	str	r3, [sl]
   10d18:	05903000 	ldreq	r3, [r0]
   10d1c:	e59fc1cc 	ldr	ip, [pc, #460]	; 10ef0 <readline_into_buffer+0x698>
   10d20:	058a3000 	streq	r3, [sl]
   10d24:	e59a1000 	ldr	r1, [sl]
   10d28:	e59c3000 	ldr	r3, [ip]
   10d2c:	e1510003 	cmp	r1, r3
   10d30:	058a2000 	streq	r2, [sl]
   10d34:	1a000014 	bne	10d8c <readline_into_buffer+0x534>
   10d38:	ea000017 	b	10d9c <readline_into_buffer+0x544>
   10d3c:	e59a1000 	ldr	r1, [sl]
   10d40:	e3510000 	cmp	r1, #0	; 0x0
   10d44:	ba000014 	blt	10d9c <readline_into_buffer+0x544>
   10d48:	e59f21a0 	ldr	r2, [pc, #416]	; 10ef0 <readline_into_buffer+0x698>
   10d4c:	e5923000 	ldr	r3, [r2]
   10d50:	e1510003 	cmp	r1, r3
   10d54:	0a000010 	beq	10d9c <readline_into_buffer+0x544>
   10d58:	e59f3188 	ldr	r3, [pc, #392]	; 10ee8 <readline_into_buffer+0x690>
   10d5c:	e59fc18c 	ldr	ip, [pc, #396]	; 10ef0 <readline_into_buffer+0x698>
   10d60:	e5932000 	ldr	r2, [r3]
   10d64:	e2813001 	add	r3, r1, #1	; 0x1
   10d68:	e1530002 	cmp	r3, r2
   10d6c:	e58a3000 	str	r3, [sl]
   10d70:	c3a03000 	movgt	r3, #0	; 0x0
   10d74:	c58a3000 	strgt	r3, [sl]
   10d78:	e59a1000 	ldr	r1, [sl]
   10d7c:	e59c3000 	ldr	r3, [ip]
   10d80:	e1510003 	cmp	r1, r3
   10d84:	059f517c 	ldreq	r5, [pc, #380]	; 10f08 <readline_into_buffer+0x6b0>
   10d88:	0a00000a 	beq	10db8 <readline_into_buffer+0x560>
   10d8c:	e59f0160 	ldr	r0, [pc, #352]	; 10ef4 <readline_into_buffer+0x69c>
   10d90:	e7905101 	ldr	r5, [r0, r1, lsl #2]
   10d94:	e3550000 	cmp	r5, #0	; 0x0
   10d98:	1a000006 	bne	10db8 <readline_into_buffer+0x560>
   10d9c:	e3a00007 	mov	r0, #7	; 0x7
   10da0:	ebfffd6f 	bl	10364 <putc>
   10da4:	eafffed0 	b	108ec <readline_into_buffer+0x94>
   10da8:	ebfffd6d 	bl	10364 <putc>
   10dac:	e59d3014 	ldr	r3, [sp, #20]
   10db0:	e2433001 	sub	r3, r3, #1	; 0x1
   10db4:	e58d3014 	str	r3, [sp, #20]
   10db8:	e59d2014 	ldr	r2, [sp, #20]
   10dbc:	e3a00008 	mov	r0, #8	; 0x8
   10dc0:	e3520000 	cmp	r2, #0	; 0x0
   10dc4:	1afffff7 	bne	10da8 <readline_into_buffer+0x550>
   10dc8:	e59d3010 	ldr	r3, [sp, #16]
   10dcc:	e3530000 	cmp	r3, #0	; 0x0
   10dd0:	11a04002 	movne	r4, r2
   10dd4:	1a000002 	bne	10de4 <readline_into_buffer+0x58c>
   10dd8:	ea00000d 	b	10e14 <readline_into_buffer+0x5bc>
   10ddc:	ebfffd60 	bl	10364 <putc>
   10de0:	e2844001 	add	r4, r4, #1	; 0x1
   10de4:	e59d3010 	ldr	r3, [sp, #16]
   10de8:	e3a00020 	mov	r0, #32	; 0x20
   10dec:	e1540003 	cmp	r4, r3
   10df0:	3afffff9 	bcc	10ddc <readline_into_buffer+0x584>
   10df4:	ea000000 	b	10dfc <readline_into_buffer+0x5a4>
   10df8:	ebfffd59 	bl	10364 <putc>
   10dfc:	e59d2014 	ldr	r2, [sp, #20]
   10e00:	e3a00008 	mov	r0, #8	; 0x8
   10e04:	e1540002 	cmp	r4, r2
   10e08:	e2444001 	sub	r4, r4, #1	; 0x1
   10e0c:	8afffff9 	bhi	10df8 <readline_into_buffer+0x5a0>
   10e10:	e58d2010 	str	r2, [sp, #16]
   10e14:	e1a01005 	mov	r1, r5
   10e18:	e1a00006 	mov	r0, r6
   10e1c:	ebffc182 	bl	142c <strcpy>
   10e20:	e1a00006 	mov	r0, r6
   10e24:	ebffc1d7 	bl	1588 <strlen>
   10e28:	e59d3014 	ldr	r3, [sp, #20]
   10e2c:	e58d0010 	str	r0, [sp, #16]
   10e30:	e1530000 	cmp	r3, r0
   10e34:	2afffeac 	bcs	108ec <readline_into_buffer+0x94>
   10e38:	e0631000 	rsb	r1, r3, r0
   10e3c:	e0862003 	add	r2, r6, r3
   10e40:	e59f00bc 	ldr	r0, [pc, #188]	; 10f04 <readline_into_buffer+0x6ac>
   10e44:	e1a0e00f 	mov	lr, pc
   10e48:	e59bf000 	ldr	pc, [fp]
   10e4c:	e3a05000 	mov	r5, #0	; 0x0
   10e50:	e59d3010 	ldr	r3, [sp, #16]
   10e54:	e58d3014 	str	r3, [sp, #20]
   10e58:	eafffea4 	b	108f0 <readline_into_buffer+0x98>
   10e5c:	e3a0cc01 	mov	ip, #256	; 0x100
   10e60:	e1a01009 	mov	r1, r9
   10e64:	e28d2014 	add	r2, sp, #20	; 0x14
   10e68:	e28d3010 	add	r3, sp, #16	; 0x10
   10e6c:	e88d1040 	stm	sp, {r6, ip}
   10e70:	ebfffe34 	bl	10748 <cread_add_char>
   10e74:	eafffe9d 	b	108f0 <readline_into_buffer+0x98>
   10e78:	e59f4070 	ldr	r4, [pc, #112]	; 10ef0 <readline_into_buffer+0x698>
   10e7c:	e59f3070 	ldr	r3, [pc, #112]	; 10ef4 <readline_into_buffer+0x69c>
   10e80:	e5942000 	ldr	r2, [r4]
   10e84:	e1a01006 	mov	r1, r6
   10e88:	e7930102 	ldr	r0, [r3, r2, lsl #2]
   10e8c:	ebffc166 	bl	142c <strcpy>
   10e90:	e5943000 	ldr	r3, [r4]
   10e94:	e59f104c 	ldr	r1, [pc, #76]	; 10ee8 <readline_into_buffer+0x690>
   10e98:	e2833001 	add	r3, r3, #1	; 0x1
   10e9c:	e5912000 	ldr	r2, [r1]
   10ea0:	e3530003 	cmp	r3, #3	; 0x3
   10ea4:	c3a03000 	movgt	r3, #0	; 0x0
   10ea8:	e1530002 	cmp	r3, r2
   10eac:	e59f2044 	ldr	r2, [pc, #68]	; 10ef8 <readline_into_buffer+0x6a0>
   10eb0:	e5843000 	str	r3, [r4]
   10eb4:	c5813000 	strgt	r3, [r1]
   10eb8:	e5923000 	ldr	r3, [r2]
   10ebc:	e2833001 	add	r3, r3, #1	; 0x1
   10ec0:	e5823000 	str	r3, [r2]
   10ec4:	e59f3024 	ldr	r3, [pc, #36]	; 10ef0 <readline_into_buffer+0x698>
   10ec8:	e1a00005 	mov	r0, r5
   10ecc:	e5932000 	ldr	r2, [r3]
   10ed0:	e59f300c 	ldr	r3, [pc, #12]	; 10ee4 <readline_into_buffer+0x68c>
   10ed4:	e5832000 	str	r2, [r3]
   10ed8:	e28dd01c 	add	sp, sp, #28	; 0x1c
   10edc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   10ee0:	3c00d23c 	.word	0x3c00d23c
   10ee4:	3c00d120 	.word	0x3c00d120
   10ee8:	3c00d240 	.word	0x3c00d240
   10eec:	3c00d994 	.word	0x3c00d994
   10ef0:	3c00d244 	.word	0x3c00d244
   10ef4:	3c00d984 	.word	0x3c00d984
   10ef8:	3c00d238 	.word	0x3c00d238
   10efc:	3c00d694 	.word	0x3c00d694
   10f00:	0001a882 	.word	0x0001a882
   10f04:	0001c458 	.word	0x0001c458
   10f08:	0001a883 	.word	0x0001a883

00010f0c <readline>:
   10f0c:	e59f1000 	ldr	r1, [pc, #0]	; 10f14 <readline+0x8>
   10f10:	eafffe50 	b	10858 <readline_into_buffer>
   10f14:	3c00d884 	.word	0x3c00d884

00010f18 <main_loop>:
   10f18:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   10f1c:	e24dd004 	sub	sp, sp, #4	; 0x4
   10f20:	e59f0030 	ldr	r0, [pc, #48]	; 10f58 <main_loop+0x40>
   10f24:	ebfffff8 	bl	10f0c <readline>
   10f28:	e1a03000 	mov	r3, r0
   10f2c:	e3730001 	cmn	r3, #1	; 0x1
   10f30:	e3a01000 	mov	r1, #0	; 0x0
   10f34:	e59f0020 	ldr	r0, [pc, #32]	; 10f5c <main_loop+0x44>
   10f38:	1a000002 	bne	10f48 <main_loop+0x30>
   10f3c:	e59f001c 	ldr	r0, [pc, #28]	; 10f60 <main_loop+0x48>
   10f40:	ebfffcdf 	bl	102c4 <puts>
   10f44:	eafffff5 	b	10f20 <main_loop+0x8>
   10f48:	e3530000 	cmp	r3, #0	; 0x0
   10f4c:	dafffff3 	ble	10f20 <main_loop+0x8>
   10f50:	ebfffd41 	bl	1045c <run_command>
   10f54:	eafffff1 	b	10f20 <main_loop+0x8>
   10f58:	0001c478 	.word	0x0001c478
   10f5c:	3c00d884 	.word	0x3c00d884
   10f60:	0001a882 	.word	0x0001a882

00010f64 <parse_num>:
   10f64:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   10f68:	e1a08001 	mov	r8, r1
   10f6c:	e1a07002 	mov	r7, r2
   10f70:	e1a06003 	mov	r6, r3
   10f74:	ea000000 	b	10f7c <parse_num+0x18>
   10f78:	e2800001 	add	r0, r0, #1	; 0x1
   10f7c:	e5d03000 	ldrb	r3, [r0]
   10f80:	e3530020 	cmp	r3, #32	; 0x20
   10f84:	0afffffb 	beq	10f78 <parse_num+0x14>
   10f88:	ea000037 	b	1106c <parse_num+0x108>
   10f8c:	e3520000 	cmp	r2, #0	; 0x0
   10f90:	0a00000a 	beq	10fc0 <parse_num+0x5c>
   10f94:	e3530030 	cmp	r3, #48	; 0x30
   10f98:	1a000008 	bne	10fc0 <parse_num+0x5c>
   10f9c:	e5d02001 	ldrb	r2, [r0, #1]
   10fa0:	e2423041 	sub	r3, r2, #65	; 0x41
   10fa4:	e6ef3073 	uxtb	r3, r3
   10fa8:	e3530019 	cmp	r3, #25	; 0x19
   10fac:	92823020 	addls	r3, r2, #32	; 0x20
   10fb0:	96ef2073 	uxtbls	r2, r3
   10fb4:	e3520078 	cmp	r2, #120	; 0x78
   10fb8:	02800002 	addeq	r0, r0, #2	; 0x2
   10fbc:	03a04010 	moveq	r4, #16	; 0x10
   10fc0:	e4d0c001 	ldrb	ip, [r0], #1
   10fc4:	e24c3030 	sub	r3, ip, #48	; 0x30
   10fc8:	e24c2041 	sub	r2, ip, #65	; 0x41
   10fcc:	e6ef3073 	uxtb	r3, r3
   10fd0:	e6ef2072 	uxtb	r2, r2
   10fd4:	e3530009 	cmp	r3, #9	; 0x9
   10fd8:	83a01000 	movhi	r1, #0	; 0x0
   10fdc:	93a01001 	movls	r1, #1	; 0x1
   10fe0:	e3520005 	cmp	r2, #5	; 0x5
   10fe4:	83a02000 	movhi	r2, #0	; 0x0
   10fe8:	93a02001 	movls	r2, #1	; 0x1
   10fec:	e1913002 	orrs	r3, r1, r2
   10ff0:	1a000003 	bne	11004 <parse_num+0xa0>
   10ff4:	e24c3061 	sub	r3, ip, #97	; 0x61
   10ff8:	e6ef3073 	uxtb	r3, r3
   10ffc:	e3530005 	cmp	r3, #5	; 0x5
   11000:	8a00000f 	bhi	11044 <parse_num+0xe0>
   11004:	e3510000 	cmp	r1, #0	; 0x0
   11008:	124c2030 	subne	r2, ip, #48	; 0x30
   1100c:	1a000007 	bne	11030 <parse_num+0xcc>
   11010:	e24c3061 	sub	r3, ip, #97	; 0x61
   11014:	e6ef3073 	uxtb	r3, r3
   11018:	e3530005 	cmp	r3, #5	; 0x5
   1101c:	924c2057 	subls	r2, ip, #87	; 0x57
   11020:	9a000002 	bls	11030 <parse_num+0xcc>
   11024:	e3520000 	cmp	r2, #0	; 0x0
   11028:	0a000002 	beq	11038 <parse_num+0xd4>
   1102c:	e24c2037 	sub	r2, ip, #55	; 0x37
   11030:	e1520004 	cmp	r2, r4
   11034:	aa000002 	bge	11044 <parse_num+0xe0>
   11038:	e0252495 	mla	r5, r5, r4, r2
   1103c:	e3a02000 	mov	r2, #0	; 0x0
   11040:	ea00000c 	b	11078 <parse_num+0x114>
   11044:	e3560000 	cmp	r6, #0	; 0x0
   11048:	0a000012 	beq	11098 <parse_num+0x134>
   1104c:	e3a03000 	mov	r3, #0	; 0x0
   11050:	e7d62003 	ldrb	r2, [r6, r3]
   11054:	e2833001 	add	r3, r3, #1	; 0x1
   11058:	e3520000 	cmp	r2, #0	; 0x0
   1105c:	0a00000d 	beq	11098 <parse_num+0x134>
   11060:	e15c0002 	cmp	ip, r2
   11064:	1afffff9 	bne	11050 <parse_num+0xec>
   11068:	ea000005 	b	11084 <parse_num+0x120>
   1106c:	e3a02001 	mov	r2, #1	; 0x1
   11070:	e3a0400a 	mov	r4, #10	; 0xa
   11074:	e3a05000 	mov	r5, #0	; 0x0
   11078:	e5d03000 	ldrb	r3, [r0]
   1107c:	e3530000 	cmp	r3, #0	; 0x0
   11080:	1affffc1 	bne	10f8c <parse_num+0x28>
   11084:	e3570000 	cmp	r7, #0	; 0x0
   11088:	15870000 	strne	r0, [r7]
   1108c:	e5885000 	str	r5, [r8]
   11090:	e3a00001 	mov	r0, #1	; 0x1
   11094:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   11098:	e3a00000 	mov	r0, #0	; 0x0
   1109c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

000110a0 <xyzModem_stream_close>:
   110a0:	e59f2044 	ldr	r2, [pc, #68]	; 110ec <xyzModem_stream_close+0x4c>
   110a4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   110a8:	e5920428 	ldr	r0, [r2, #1064]
   110ac:	e59f303c 	ldr	r3, [pc, #60]	; 110f0 <xyzModem_stream_close+0x50>
   110b0:	e592c424 	ldr	ip, [r2, #1060]
   110b4:	e592e418 	ldr	lr, [r2, #1048]
   110b8:	e3500000 	cmp	r0, #0	; 0x0
   110bc:	e24dd00c 	sub	sp, sp, #12	; 0xc
   110c0:	e59f102c 	ldr	r1, [pc, #44]	; 110f4 <xyzModem_stream_close+0x54>
   110c4:	01a01003 	moveq	r1, r3
   110c8:	e59f3028 	ldr	r3, [pc, #40]	; 110f8 <xyzModem_stream_close+0x58>
   110cc:	e88d5000 	stm	sp, {ip, lr}
   110d0:	e593c000 	ldr	ip, [r3]
   110d4:	e59f0020 	ldr	r0, [pc, #32]	; 110fc <xyzModem_stream_close+0x5c>
   110d8:	e5923420 	ldr	r3, [r2, #1056]
   110dc:	e592241c 	ldr	r2, [r2, #1052]
   110e0:	e12fff3c 	blx	ip
   110e4:	e28dd00c 	add	sp, sp, #12	; 0xc
   110e8:	e8bd8000 	pop	{pc}
   110ec:	3c00d258 	.word	0x3c00d258
   110f0:	0001c47c 	.word	0x0001c47c
   110f4:	0001c482 	.word	0x0001c482
   110f8:	3c00d694 	.word	0x3c00d694
   110fc:	0001c486 	.word	0x0001c486

00011100 <xyzModem_error>:
   11100:	e2800008 	add	r0, r0, #8	; 0x8
   11104:	e3500007 	cmp	r0, #7	; 0x7
   11108:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
   1110c:	ea000017 	b	11170 <xyzModem_error+0x70>
   11110:	00011168 	.word	0x00011168
   11114:	00011160 	.word	0x00011160
   11118:	00011158 	.word	0x00011158
   1111c:	00011150 	.word	0x00011150
   11120:	00011148 	.word	0x00011148
   11124:	00011140 	.word	0x00011140
   11128:	00011130 	.word	0x00011130
   1112c:	00011138 	.word	0x00011138
   11130:	e59f0040 	ldr	r0, [pc, #64]	; 11178 <xyzModem_error+0x78>
   11134:	e12fff1e 	bx	lr
   11138:	e59f003c 	ldr	r0, [pc, #60]	; 1117c <xyzModem_error+0x7c>
   1113c:	e12fff1e 	bx	lr
   11140:	e59f0038 	ldr	r0, [pc, #56]	; 11180 <xyzModem_error+0x80>
   11144:	e12fff1e 	bx	lr
   11148:	e59f0034 	ldr	r0, [pc, #52]	; 11184 <xyzModem_error+0x84>
   1114c:	e12fff1e 	bx	lr
   11150:	e59f0030 	ldr	r0, [pc, #48]	; 11188 <xyzModem_error+0x88>
   11154:	e12fff1e 	bx	lr
   11158:	e59f002c 	ldr	r0, [pc, #44]	; 1118c <xyzModem_error+0x8c>
   1115c:	e12fff1e 	bx	lr
   11160:	e59f0028 	ldr	r0, [pc, #40]	; 11190 <xyzModem_error+0x90>
   11164:	e12fff1e 	bx	lr
   11168:	e59f0024 	ldr	r0, [pc, #36]	; 11194 <xyzModem_error+0x94>
   1116c:	e12fff1e 	bx	lr
   11170:	e59f0020 	ldr	r0, [pc, #32]	; 11198 <xyzModem_error+0x98>
   11174:	e12fff1e 	bx	lr
   11178:	0001c4c7 	.word	0x0001c4c7
   1117c:	0001c4e7 	.word	0x0001c4e7
   11180:	0001c4f9 	.word	0x0001c4f9
   11184:	0001c503 	.word	0x0001c503
   11188:	0001c50f 	.word	0x0001c50f
   1118c:	0001c519 	.word	0x0001c519
   11190:	0001c529 	.word	0x0001c529
   11194:	0001c53c 	.word	0x0001c53c
   11198:	0001c551 	.word	0x0001c551

0001119c <delay>:
   1119c:	e12fff1e 	bx	lr

000111a0 <init_num>:
   111a0:	e59f305c 	ldr	r3, [pc, #92]	; 11204 <init_num+0x64>
   111a4:	e3a01001 	mov	r1, #1	; 0x1
   111a8:	e5c31000 	strb	r1, [r3]
   111ac:	e59f3054 	ldr	r3, [pc, #84]	; 11208 <init_num+0x68>
   111b0:	e3a02000 	mov	r2, #0	; 0x0
   111b4:	e5c32000 	strb	r2, [r3]
   111b8:	e59f304c 	ldr	r3, [pc, #76]	; 1120c <init_num+0x6c>
   111bc:	e5c32000 	strb	r2, [r3]
   111c0:	e59f3048 	ldr	r3, [pc, #72]	; 11210 <init_num+0x70>
   111c4:	e5c32000 	strb	r2, [r3]
   111c8:	e59f3044 	ldr	r3, [pc, #68]	; 11214 <init_num+0x74>
   111cc:	e5c32000 	strb	r2, [r3]
   111d0:	e59f3040 	ldr	r3, [pc, #64]	; 11218 <init_num+0x78>
   111d4:	e5832000 	str	r2, [r3]
   111d8:	e59f303c 	ldr	r3, [pc, #60]	; 1121c <init_num+0x7c>
   111dc:	e5c32000 	strb	r2, [r3]
   111e0:	e59f3038 	ldr	r3, [pc, #56]	; 11220 <init_num+0x80>
   111e4:	e5c32000 	strb	r2, [r3]
   111e8:	e59f3034 	ldr	r3, [pc, #52]	; 11224 <init_num+0x84>
   111ec:	e5c32000 	strb	r2, [r3]
   111f0:	e59f3030 	ldr	r3, [pc, #48]	; 11228 <init_num+0x88>
   111f4:	e5c32000 	strb	r2, [r3]
   111f8:	e59f302c 	ldr	r3, [pc, #44]	; 1122c <init_num+0x8c>
   111fc:	e5c32000 	strb	r2, [r3]
   11200:	e12fff1e 	bx	lr
   11204:	3c00d124 	.word	0x3c00d124
   11208:	3c00d255 	.word	0x3c00d255
   1120c:	3c00d248 	.word	0x3c00d248
   11210:	3c00d249 	.word	0x3c00d249
   11214:	3c00d24a 	.word	0x3c00d24a
   11218:	3c00d24c 	.word	0x3c00d24c
   1121c:	3c00d250 	.word	0x3c00d250
   11220:	3c00d251 	.word	0x3c00d251
   11224:	3c00d252 	.word	0x3c00d252
   11228:	3c00d253 	.word	0x3c00d253
   1122c:	3c00d254 	.word	0x3c00d254

00011230 <out_message>:
   11230:	e92d4010 	push	{r4, lr}
   11234:	e59f30c8 	ldr	r3, [pc, #200]	; 11304 <out_message+0xd4>
   11238:	e59f40c8 	ldr	r4, [pc, #200]	; 11308 <out_message+0xd8>
   1123c:	e5d31000 	ldrb	r1, [r3]
   11240:	e59f00c4 	ldr	r0, [pc, #196]	; 1130c <out_message+0xdc>
   11244:	e1a0e00f 	mov	lr, pc
   11248:	e594f000 	ldr	pc, [r4]
   1124c:	e59f30bc 	ldr	r3, [pc, #188]	; 11310 <out_message+0xe0>
   11250:	e59f00bc 	ldr	r0, [pc, #188]	; 11314 <out_message+0xe4>
   11254:	e5d31000 	ldrb	r1, [r3]
   11258:	e1a0e00f 	mov	lr, pc
   1125c:	e594f000 	ldr	pc, [r4]
   11260:	e59f30b0 	ldr	r3, [pc, #176]	; 11318 <out_message+0xe8>
   11264:	e59f00b0 	ldr	r0, [pc, #176]	; 1131c <out_message+0xec>
   11268:	e5931000 	ldr	r1, [r3]
   1126c:	e1a0e00f 	mov	lr, pc
   11270:	e594f000 	ldr	pc, [r4]
   11274:	e59f30a4 	ldr	r3, [pc, #164]	; 11320 <out_message+0xf0>
   11278:	e59f00a4 	ldr	r0, [pc, #164]	; 11324 <out_message+0xf4>
   1127c:	e5d31000 	ldrb	r1, [r3]
   11280:	e1a0e00f 	mov	lr, pc
   11284:	e594f000 	ldr	pc, [r4]
   11288:	e59f3098 	ldr	r3, [pc, #152]	; 11328 <out_message+0xf8>
   1128c:	e59f0098 	ldr	r0, [pc, #152]	; 1132c <out_message+0xfc>
   11290:	e5d31000 	ldrb	r1, [r3]
   11294:	e1a0e00f 	mov	lr, pc
   11298:	e594f000 	ldr	pc, [r4]
   1129c:	e59f308c 	ldr	r3, [pc, #140]	; 11330 <out_message+0x100>
   112a0:	e59f008c 	ldr	r0, [pc, #140]	; 11334 <out_message+0x104>
   112a4:	e5d31000 	ldrb	r1, [r3]
   112a8:	e1a0e00f 	mov	lr, pc
   112ac:	e594f000 	ldr	pc, [r4]
   112b0:	e59f3080 	ldr	r3, [pc, #128]	; 11338 <out_message+0x108>
   112b4:	e59f0080 	ldr	r0, [pc, #128]	; 1133c <out_message+0x10c>
   112b8:	e5d31000 	ldrb	r1, [r3]
   112bc:	e1a0e00f 	mov	lr, pc
   112c0:	e594f000 	ldr	pc, [r4]
   112c4:	e59f3074 	ldr	r3, [pc, #116]	; 11340 <out_message+0x110>
   112c8:	e59f0074 	ldr	r0, [pc, #116]	; 11344 <out_message+0x114>
   112cc:	e5d31000 	ldrb	r1, [r3]
   112d0:	e1a0e00f 	mov	lr, pc
   112d4:	e594f000 	ldr	pc, [r4]
   112d8:	e59f3068 	ldr	r3, [pc, #104]	; 11348 <out_message+0x118>
   112dc:	e59f0068 	ldr	r0, [pc, #104]	; 1134c <out_message+0x11c>
   112e0:	e5d31000 	ldrb	r1, [r3]
   112e4:	e1a0e00f 	mov	lr, pc
   112e8:	e594f000 	ldr	pc, [r4]
   112ec:	e59f305c 	ldr	r3, [pc, #92]	; 11350 <out_message+0x120>
   112f0:	e59f005c 	ldr	r0, [pc, #92]	; 11354 <out_message+0x124>
   112f4:	e5d31000 	ldrb	r1, [r3]
   112f8:	e1a0e00f 	mov	lr, pc
   112fc:	e594f000 	ldr	pc, [r4]
   11300:	e8bd8010 	pop	{r4, pc}
   11304:	3c00d248 	.word	0x3c00d248
   11308:	3c00d694 	.word	0x3c00d694
   1130c:	0001c55f 	.word	0x0001c55f
   11310:	3c00d249 	.word	0x3c00d249
   11314:	0001c56d 	.word	0x0001c56d
   11318:	3c00d24c 	.word	0x3c00d24c
   1131c:	0001c580 	.word	0x0001c580
   11320:	3c00d250 	.word	0x3c00d250
   11324:	0001c592 	.word	0x0001c592
   11328:	3c00d251 	.word	0x3c00d251
   1132c:	0001c5a5 	.word	0x0001c5a5
   11330:	3c00d252 	.word	0x3c00d252
   11334:	0001c5bc 	.word	0x0001c5bc
   11338:	3c00d124 	.word	0x3c00d124
   1133c:	0001c5d4 	.word	0x0001c5d4
   11340:	3c00d253 	.word	0x3c00d253
   11344:	0001c5e3 	.word	0x0001c5e3
   11348:	3c00d254 	.word	0x3c00d254
   1134c:	0001c5fc 	.word	0x0001c5fc
   11350:	3c00d255 	.word	0x3c00d255
   11354:	0001c60e 	.word	0x0001c60e

00011358 <Procpacket>:
   11358:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   1135c:	e59fc1ac 	ldr	ip, [pc, #428]	; 11510 <Procpacket+0x1b8>
   11360:	e59f21ac 	ldr	r2, [pc, #428]	; 11514 <Procpacket+0x1bc>
   11364:	e5dc3000 	ldrb	r3, [ip]
   11368:	e1a06000 	mov	r6, r0
   1136c:	e5c03000 	strb	r3, [r0]
   11370:	e5923000 	ldr	r3, [r2]
   11374:	e1a05001 	mov	r5, r1
   11378:	e5c03001 	strb	r3, [r0, #1]
   1137c:	e5923000 	ldr	r3, [r2]
   11380:	e1e03003 	mvn	r3, r3
   11384:	e5c03002 	strb	r3, [r0, #2]
   11388:	e59f3188 	ldr	r3, [pc, #392]	; 11518 <Procpacket+0x1c0>
   1138c:	e5922000 	ldr	r2, [r2]
   11390:	e5933000 	ldr	r3, [r3]
   11394:	e1520003 	cmp	r2, r3
   11398:	1a000008 	bne	113c0 <Procpacket+0x68>
   1139c:	e59f3178 	ldr	r3, [pc, #376]	; 1151c <Procpacket+0x1c4>
   113a0:	e5d32000 	ldrb	r2, [r3]
   113a4:	e59f3174 	ldr	r3, [pc, #372]	; 11520 <Procpacket+0x1c8>
   113a8:	e3520001 	cmp	r2, #1	; 0x1
   113ac:	e5934000 	ldr	r4, [r3]
   113b0:	e5cc2000 	strb	r2, [ip]
   113b4:	03a07080 	moveq	r7, #128	; 0x80
   113b8:	13a07b01 	movne	r7, #1024	; 0x400
   113bc:	ea000004 	b	113d4 <Procpacket+0x7c>
   113c0:	e5dc3000 	ldrb	r3, [ip]
   113c4:	e3530001 	cmp	r3, #1	; 0x1
   113c8:	13a04b01 	movne	r4, #1024	; 0x400
   113cc:	03a04080 	moveq	r4, #128	; 0x80
   113d0:	e1a07004 	mov	r7, r4
   113d4:	e59f3148 	ldr	r3, [pc, #328]	; 11524 <Procpacket+0x1cc>
   113d8:	e5d33000 	ldrb	r3, [r3]
   113dc:	e3530000 	cmp	r3, #0	; 0x0
   113e0:	1a000008 	bne	11408 <Procpacket+0xb0>
   113e4:	e59f313c 	ldr	r3, [pc, #316]	; 11528 <Procpacket+0x1d0>
   113e8:	e2860003 	add	r0, r6, #3	; 0x3
   113ec:	e5931000 	ldr	r1, [r3]
   113f0:	e59f3134 	ldr	r3, [pc, #308]	; 1152c <Procpacket+0x1d4>
   113f4:	e1a02004 	mov	r2, r4
   113f8:	e5933000 	ldr	r3, [r3]
   113fc:	e0631001 	rsb	r1, r3, r1
   11400:	e0851001 	add	r1, r5, r1
   11404:	ebffc079 	bl	15f0 <memcpy>
   11408:	e59f111c 	ldr	r1, [pc, #284]	; 1152c <Procpacket+0x1d4>
   1140c:	e59f311c 	ldr	r3, [pc, #284]	; 11530 <Procpacket+0x1d8>
   11410:	e5912000 	ldr	r2, [r1]
   11414:	e5d33000 	ldrb	r3, [r3]
   11418:	e0642002 	rsb	r2, r4, r2
   1141c:	e3530000 	cmp	r3, #0	; 0x0
   11420:	e5812000 	str	r2, [r1]
   11424:	0a000006 	beq	11444 <Procpacket+0xec>
   11428:	e1a02007 	mov	r2, r7
   1142c:	e2860003 	add	r0, r6, #3	; 0x3
   11430:	e3a01000 	mov	r1, #0	; 0x0
   11434:	ebffc065 	bl	15d0 <memset>
   11438:	e59f30f4 	ldr	r3, [pc, #244]	; 11534 <Procpacket+0x1dc>
   1143c:	e3a02001 	mov	r2, #1	; 0x1
   11440:	e5c32000 	strb	r2, [r3]
   11444:	e59f30c8 	ldr	r3, [pc, #200]	; 11514 <Procpacket+0x1bc>
   11448:	e5932000 	ldr	r2, [r3]
   1144c:	e59f30c4 	ldr	r3, [pc, #196]	; 11518 <Procpacket+0x1c0>
   11450:	e5933000 	ldr	r3, [r3]
   11454:	e1520003 	cmp	r2, r3
   11458:	1a000004 	bne	11470 <Procpacket+0x118>
   1145c:	e2840003 	add	r0, r4, #3	; 0x3
   11460:	e0860000 	add	r0, r6, r0
   11464:	e0642007 	rsb	r2, r4, r7
   11468:	e3a0101a 	mov	r1, #26	; 0x1a
   1146c:	ebffc057 	bl	15d0 <memset>
   11470:	e3a00000 	mov	r0, #0	; 0x0
   11474:	e59f80bc 	ldr	r8, [pc, #188]	; 11538 <Procpacket+0x1e0>
   11478:	e1a04006 	mov	r4, r6
   1147c:	e1a0c000 	mov	ip, r0
   11480:	e1a05000 	mov	r5, r0
   11484:	e1a0342c 	lsr	r3, ip, #8
   11488:	e5d42003 	ldrb	r2, [r4, #3]
   1148c:	e1a03083 	lsl	r3, r3, #1
   11490:	e19810b3 	ldrh	r1, [r8, r3]
   11494:	e2800001 	add	r0, r0, #1	; 0x1
   11498:	e182340c 	orr	r3, r2, ip, lsl #8
   1149c:	e0233001 	eor	r3, r3, r1
   114a0:	e0852002 	add	r2, r5, r2
   114a4:	e1500007 	cmp	r0, r7
   114a8:	e6ffc073 	uxth	ip, r3
   114ac:	e6ef5072 	uxtb	r5, r2
   114b0:	e2844001 	add	r4, r4, #1	; 0x1
   114b4:	e59f107c 	ldr	r1, [pc, #124]	; 11538 <Procpacket+0x1e0>
   114b8:	3afffff1 	bcc	11484 <Procpacket+0x12c>
   114bc:	e59f3078 	ldr	r3, [pc, #120]	; 1153c <Procpacket+0x1e4>
   114c0:	e0860007 	add	r0, r6, r7
   114c4:	e5d33000 	ldrb	r3, [r3]
   114c8:	e3530000 	cmp	r3, #0	; 0x0
   114cc:	05c05003 	strbeq	r5, [r0, #3]
   114d0:	0a00000c 	beq	11508 <Procpacket+0x1b0>
   114d4:	e1a0342c 	lsr	r3, ip, #8
   114d8:	e1a03083 	lsl	r3, r3, #1
   114dc:	e19120b3 	ldrh	r2, [r1, r3]
   114e0:	e022240c 	eor	r2, r2, ip, lsl #8
   114e4:	e1a03442 	asr	r3, r2, #8
   114e8:	e20330ff 	and	r3, r3, #255	; 0xff
   114ec:	e1a03083 	lsl	r3, r3, #1
   114f0:	e19130b3 	ldrh	r3, [r1, r3]
   114f4:	e0233402 	eor	r3, r3, r2, lsl #8
   114f8:	e6ff3073 	uxth	r3, r3
   114fc:	e1a02423 	lsr	r2, r3, #8
   11500:	e5c03004 	strb	r3, [r0, #4]
   11504:	e5c02003 	strb	r2, [r0, #3]
   11508:	e3a00000 	mov	r0, #0	; 0x0
   1150c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   11510:	3c00d24a 	.word	0x3c00d24a
   11514:	3c00d24c 	.word	0x3c00d24c
   11518:	3c00dd94 	.word	0x3c00dd94
   1151c:	3c00dd9c 	.word	0x3c00dd9c
   11520:	3c00dd98 	.word	0x3c00dd98
   11524:	3c00d250 	.word	0x3c00d250
   11528:	3c00d6f4 	.word	0x3c00d6f4
   1152c:	3c00d6f8 	.word	0x3c00d6f8
   11530:	3c00d255 	.word	0x3c00d255
   11534:	3c00d254 	.word	0x3c00d254
   11538:	000199e8 	.word	0x000199e8
   1153c:	3c00d6f0 	.word	0x3c00d6f0

00011540 <procsend>:
   11540:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   11544:	e59f31f8 	ldr	r3, [pc, #504]	; 11744 <procsend+0x204>
   11548:	e1a06000 	mov	r6, r0
   1154c:	e5d30000 	ldrb	r0, [r3]
   11550:	e24dd004 	sub	sp, sp, #4	; 0x4
   11554:	e3500000 	cmp	r0, #0	; 0x0
   11558:	e1a0c002 	mov	ip, r2
   1155c:	e59f41e4 	ldr	r4, [pc, #484]	; 11748 <procsend+0x208>
   11560:	0a000065 	beq	116fc <procsend+0x1bc>
   11564:	e59f31e0 	ldr	r3, [pc, #480]	; 1174c <procsend+0x20c>
   11568:	e1a02b02 	lsl	r2, r2, #22
   1156c:	e1a02b22 	lsr	r2, r2, #22
   11570:	e583c000 	str	ip, [r3]
   11574:	e59f31d4 	ldr	r3, [pc, #468]	; 11750 <procsend+0x210>
   11578:	e59f01d4 	ldr	r0, [pc, #468]	; 11754 <procsend+0x214>
   1157c:	e3520000 	cmp	r2, #0	; 0x0
   11580:	e1a0152c 	lsr	r1, ip, #10
   11584:	e583c000 	str	ip, [r3]
   11588:	03a03b01 	moveq	r3, #1024	; 0x400
   1158c:	e5802000 	str	r2, [r0]
   11590:	e5841000 	str	r1, [r4]
   11594:	05803000 	streq	r3, [r0]
   11598:	0a000006 	beq	115b8 <procsend+0x78>
   1159c:	e2813001 	add	r3, r1, #1	; 0x1
   115a0:	e352007f 	cmp	r2, #127	; 0x7f
   115a4:	e59f21ac 	ldr	r2, [pc, #428]	; 11758 <procsend+0x218>
   115a8:	e5843000 	str	r3, [r4]
   115ac:	93a03001 	movls	r3, #1	; 0x1
   115b0:	83a03002 	movhi	r3, #2	; 0x2
   115b4:	e5c23000 	strb	r3, [r2]
   115b8:	e59f019c 	ldr	r0, [pc, #412]	; 1175c <procsend+0x21c>
   115bc:	ebffbff1 	bl	1588 <strlen>
   115c0:	e59f2198 	ldr	r2, [pc, #408]	; 11760 <procsend+0x220>
   115c4:	e3500080 	cmp	r0, #128	; 0x80
   115c8:	83a03002 	movhi	r3, #2	; 0x2
   115cc:	93a03001 	movls	r3, #1	; 0x1
   115d0:	85c23000 	strbhi	r3, [r2]
   115d4:	95c23000 	strbls	r3, [r2]
   115d8:	e59f3184 	ldr	r3, [pc, #388]	; 11764 <procsend+0x224>
   115dc:	83a07b01 	movhi	r7, #1024	; 0x400
   115e0:	e5d31000 	ldrb	r1, [r3]
   115e4:	93a07080 	movls	r7, #128	; 0x80
   115e8:	e3510000 	cmp	r1, #0	; 0x0
   115ec:	13a01000 	movne	r1, #0	; 0x0
   115f0:	12872005 	addne	r2, r7, #5	; 0x5
   115f4:	02872004 	addeq	r2, r7, #4	; 0x4
   115f8:	11a00006 	movne	r0, r6
   115fc:	01a00006 	moveq	r0, r6
   11600:	ebffbff2 	bl	15d0 <memset>
   11604:	e59f3154 	ldr	r3, [pc, #340]	; 11760 <procsend+0x220>
   11608:	e3a05000 	mov	r5, #0	; 0x0
   1160c:	e5d33000 	ldrb	r3, [r3]
   11610:	e59f0144 	ldr	r0, [pc, #324]	; 1175c <procsend+0x21c>
   11614:	e5c63000 	strb	r3, [r6]
   11618:	e3e03000 	mvn	r3, #0	; 0x0
   1161c:	e5c63002 	strb	r3, [r6, #2]
   11620:	e5c65001 	strb	r5, [r6, #1]
   11624:	ebffbfd7 	bl	1588 <strlen>
   11628:	e59f112c 	ldr	r1, [pc, #300]	; 1175c <procsend+0x21c>
   1162c:	e1a02000 	mov	r2, r0
   11630:	e1a04000 	mov	r4, r0
   11634:	e2860003 	add	r0, r6, #3	; 0x3
   11638:	ebffbf82 	bl	1448 <strncpy>
   1163c:	e59f3108 	ldr	r3, [pc, #264]	; 1174c <procsend+0x20c>
   11640:	e2844004 	add	r4, r4, #4	; 0x4
   11644:	e0860004 	add	r0, r6, r4
   11648:	e5932000 	ldr	r2, [r3]
   1164c:	e59f1114 	ldr	r1, [pc, #276]	; 11768 <procsend+0x228>
   11650:	e59f4114 	ldr	r4, [pc, #276]	; 1176c <procsend+0x22c>
   11654:	ebfffab8 	bl	1013c <sprintf>
   11658:	e1a00006 	mov	r0, r6
   1165c:	e1a0c005 	mov	ip, r5
   11660:	e1a0e005 	mov	lr, r5
   11664:	e1a0342c 	lsr	r3, ip, #8
   11668:	e5d02003 	ldrb	r2, [r0, #3]
   1166c:	e1a03083 	lsl	r3, r3, #1
   11670:	e19410b3 	ldrh	r1, [r4, r3]
   11674:	e2855001 	add	r5, r5, #1	; 0x1
   11678:	e182340c 	orr	r3, r2, ip, lsl #8
   1167c:	e0233001 	eor	r3, r3, r1
   11680:	e08e2002 	add	r2, lr, r2
   11684:	e1550007 	cmp	r5, r7
   11688:	e6ffc073 	uxth	ip, r3
   1168c:	e6efe072 	uxtb	lr, r2
   11690:	e2800001 	add	r0, r0, #1	; 0x1
   11694:	e59f10d0 	ldr	r1, [pc, #208]	; 1176c <procsend+0x22c>
   11698:	3afffff1 	bcc	11664 <procsend+0x124>
   1169c:	e59f30c0 	ldr	r3, [pc, #192]	; 11764 <procsend+0x224>
   116a0:	e0860007 	add	r0, r6, r7
   116a4:	e5d33000 	ldrb	r3, [r3]
   116a8:	e3530000 	cmp	r3, #0	; 0x0
   116ac:	05c0e003 	strbeq	lr, [r0, #3]
   116b0:	0a00000c 	beq	116e8 <procsend+0x1a8>
   116b4:	e1a0342c 	lsr	r3, ip, #8
   116b8:	e1a03083 	lsl	r3, r3, #1
   116bc:	e19120b3 	ldrh	r2, [r1, r3]
   116c0:	e022240c 	eor	r2, r2, ip, lsl #8
   116c4:	e1a03442 	asr	r3, r2, #8
   116c8:	e20330ff 	and	r3, r3, #255	; 0xff
   116cc:	e1a03083 	lsl	r3, r3, #1
   116d0:	e19130b3 	ldrh	r3, [r1, r3]
   116d4:	e0233402 	eor	r3, r3, r2, lsl #8
   116d8:	e6ff3073 	uxth	r3, r3
   116dc:	e1a02423 	lsr	r2, r3, #8
   116e0:	e5c03004 	strb	r3, [r0, #4]
   116e4:	e5c02003 	strb	r2, [r0, #3]
   116e8:	e59f3080 	ldr	r3, [pc, #128]	; 11770 <procsend+0x230>
   116ec:	e3a02001 	mov	r2, #1	; 0x1
   116f0:	e5c32000 	strb	r2, [r3]
   116f4:	e3a00000 	mov	r0, #0	; 0x0
   116f8:	ea00000f 	b	1173c <procsend+0x1fc>
   116fc:	e59f2070 	ldr	r2, [pc, #112]	; 11774 <procsend+0x234>
   11700:	e5943000 	ldr	r3, [r4]
   11704:	e5922000 	ldr	r2, [r2]
   11708:	e1520003 	cmp	r2, r3
   1170c:	8a00000a 	bhi	1173c <procsend+0x1fc>
   11710:	e59f3060 	ldr	r3, [pc, #96]	; 11778 <procsend+0x238>
   11714:	e59f2044 	ldr	r2, [pc, #68]	; 11760 <procsend+0x220>
   11718:	e5d33000 	ldrb	r3, [r3]
   1171c:	e1a00006 	mov	r0, r6
   11720:	e3530000 	cmp	r3, #0	; 0x0
   11724:	13a03001 	movne	r3, #1	; 0x1
   11728:	03a03002 	moveq	r3, #2	; 0x2
   1172c:	e5c23000 	strb	r3, [r2]
   11730:	e1a0200c 	mov	r2, ip
   11734:	ebffff07 	bl	11358 <Procpacket>
   11738:	e6ef0070 	uxtb	r0, r0
   1173c:	e28dd004 	add	sp, sp, #4	; 0x4
   11740:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   11744:	3c00d124 	.word	0x3c00d124
   11748:	3c00dd94 	.word	0x3c00dd94
   1174c:	3c00d6f8 	.word	0x3c00d6f8
   11750:	3c00d6f4 	.word	0x3c00d6f4
   11754:	3c00dd98 	.word	0x3c00dd98
   11758:	3c00dd9c 	.word	0x3c00dd9c
   1175c:	0001c61d 	.word	0x0001c61d
   11760:	3c00d24a 	.word	0x3c00d24a
   11764:	3c00d6f0 	.word	0x3c00d6f0
   11768:	0001bc3a 	.word	0x0001bc3a
   1176c:	000199e8 	.word	0x000199e8
   11770:	3c00d249 	.word	0x3c00d249
   11774:	3c00d24c 	.word	0x3c00d24c
   11778:	3c00d255 	.word	0x3c00d255

0001177c <CYGACC_COMM_IF_PUTC>:
   1177c:	e1a00001 	mov	r0, r1
   11780:	eafffaf7 	b	10364 <putc>

00011784 <real_send>:
   11784:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   11788:	e59f30ec 	ldr	r3, [pc, #236]	; 1187c <real_send+0xf8>
   1178c:	e24ddb01 	sub	sp, sp, #1024	; 0x400
   11790:	e5d33000 	ldrb	r3, [r3]
   11794:	e24dd00c 	sub	sp, sp, #12	; 0xc
   11798:	e3530000 	cmp	r3, #0	; 0x0
   1179c:	e1a0e000 	mov	lr, r0
   117a0:	e1a0c001 	mov	ip, r1
   117a4:	159f30d4 	ldrne	r3, [pc, #212]	; 11880 <real_send+0xfc>
   117a8:	13a02000 	movne	r2, #0	; 0x0
   117ac:	15832000 	strne	r2, [r3]
   117b0:	1a000006 	bne	117d0 <real_send+0x4c>
   117b4:	e59f30c8 	ldr	r3, [pc, #200]	; 11884 <real_send+0x100>
   117b8:	e5d33000 	ldrb	r3, [r3]
   117bc:	e3530000 	cmp	r3, #0	; 0x0
   117c0:	059f20b8 	ldreq	r2, [pc, #184]	; 11880 <real_send+0xfc>
   117c4:	05923000 	ldreq	r3, [r2]
   117c8:	02833001 	addeq	r3, r3, #1	; 0x1
   117cc:	05823000 	streq	r3, [r2]
   117d0:	e28d0008 	add	r0, sp, #8	; 0x8
   117d4:	e2400005 	sub	r0, r0, #5	; 0x5
   117d8:	e1a0100e 	mov	r1, lr
   117dc:	e1a0200c 	mov	r2, ip
   117e0:	ebffff56 	bl	11540 <procsend>
   117e4:	e2507000 	subs	r7, r0, #0	; 0x0
   117e8:	1a00001f 	bne	1186c <real_send+0xe8>
   117ec:	e59f3094 	ldr	r3, [pc, #148]	; 11888 <real_send+0x104>
   117f0:	e28d5008 	add	r5, sp, #8	; 0x8
   117f4:	e5d31000 	ldrb	r1, [r3]
   117f8:	e59f308c 	ldr	r3, [pc, #140]	; 1188c <real_send+0x108>
   117fc:	e3510001 	cmp	r1, #1	; 0x1
   11800:	e5d32000 	ldrb	r2, [r3]
   11804:	e3003404 	movw	r3, #1028	; 0x404
   11808:	11a06003 	movne	r6, r3
   1180c:	03a06084 	moveq	r6, #132	; 0x84
   11810:	e3520000 	cmp	r2, #0	; 0x0
   11814:	12866001 	addne	r6, r6, #1	; 0x1
   11818:	e2455005 	sub	r5, r5, #5	; 0x5
   1181c:	e3a04000 	mov	r4, #0	; 0x0
   11820:	e7d50004 	ldrb	r0, [r5, r4]
   11824:	e2844001 	add	r4, r4, #1	; 0x1
   11828:	ebfffacd 	bl	10364 <putc>
   1182c:	e1540006 	cmp	r4, r6
   11830:	3afffffa 	bcc	11820 <real_send+0x9c>
   11834:	e59f3054 	ldr	r3, [pc, #84]	; 11890 <real_send+0x10c>
   11838:	e5d33000 	ldrb	r3, [r3]
   1183c:	e3530000 	cmp	r3, #0	; 0x0
   11840:	13e07000 	mvnne	r7, #0	; 0x0
   11844:	1a000008 	bne	1186c <real_send+0xe8>
   11848:	e59f3030 	ldr	r3, [pc, #48]	; 11880 <real_send+0xfc>
   1184c:	e5932000 	ldr	r2, [r3]
   11850:	e59f303c 	ldr	r3, [pc, #60]	; 11894 <real_send+0x110>
   11854:	e5933000 	ldr	r3, [r3]
   11858:	e1520003 	cmp	r2, r3
   1185c:	059f3034 	ldreq	r3, [pc, #52]	; 11898 <real_send+0x114>
   11860:	03a02001 	moveq	r2, #1	; 0x1
   11864:	05c32000 	strbeq	r2, [r3]
   11868:	03a07003 	moveq	r7, #3	; 0x3
   1186c:	e1a00007 	mov	r0, r7
   11870:	e28dd00c 	add	sp, sp, #12	; 0xc
   11874:	e28ddb01 	add	sp, sp, #1024	; 0x400
   11878:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   1187c:	3c00d255 	.word	0x3c00d255
   11880:	3c00d24c 	.word	0x3c00d24c
   11884:	3c00d124 	.word	0x3c00d124
   11888:	3c00d24a 	.word	0x3c00d24a
   1188c:	3c00d6f0 	.word	0x3c00d6f0
   11890:	3c00d254 	.word	0x3c00d254
   11894:	3c00dd94 	.word	0x3c00dd94
   11898:	3c00d250 	.word	0x3c00d250

0001189c <CYGACC_COMM_IF_GETC_TIMEOUT>:
   1189c:	e92d4070 	push	{r4, r5, r6, lr}
   118a0:	e1a05001 	mov	r5, r1
   118a4:	e3a04000 	mov	r4, #0	; 0x0
   118a8:	e59f603c 	ldr	r6, [pc, #60]	; 118ec <CYGACC_COMM_IF_GETC_TIMEOUT+0x50>
   118ac:	ea000000 	b	118b4 <CYGACC_COMM_IF_GETC_TIMEOUT+0x18>
   118b0:	ebffe717 	bl	b514 <udelay>
   118b4:	ebfffaab 	bl	10368 <tstc>
   118b8:	e3500000 	cmp	r0, #0	; 0x0
   118bc:	e3a00014 	mov	r0, #20	; 0x14
   118c0:	1a000002 	bne	118d0 <CYGACC_COMM_IF_GETC_TIMEOUT+0x34>
   118c4:	e1540006 	cmp	r4, r6
   118c8:	e2844001 	add	r4, r4, #1	; 0x1
   118cc:	1afffff7 	bne	118b0 <CYGACC_COMM_IF_GETC_TIMEOUT+0x14>
   118d0:	ebfffaa4 	bl	10368 <tstc>
   118d4:	e3500000 	cmp	r0, #0	; 0x0
   118d8:	08bd8070 	popeq	{r4, r5, r6, pc}
   118dc:	ebfffaa2 	bl	1036c <getc>
   118e0:	e5c50000 	strb	r0, [r5]
   118e4:	e3a00001 	mov	r0, #1	; 0x1
   118e8:	e8bd8070 	pop	{r4, r5, r6, pc}
   118ec:	000186a0 	.word	0x000186a0

000118f0 <control_send>:
   118f0:	e59f323c 	ldr	r3, [pc, #572]	; 11b34 <control_send+0x244>
   118f4:	e3a02000 	mov	r2, #0	; 0x0
   118f8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   118fc:	e5c32000 	strb	r2, [r3]
   11900:	e59f3230 	ldr	r3, [pc, #560]	; 11b38 <control_send+0x248>
   11904:	e59f4230 	ldr	r4, [pc, #560]	; 11b3c <control_send+0x24c>
   11908:	e5c32000 	strb	r2, [r3]
   1190c:	e59f322c 	ldr	r3, [pc, #556]	; 11b40 <control_send+0x250>
   11910:	e59f522c 	ldr	r5, [pc, #556]	; 11b44 <control_send+0x254>
   11914:	e5832000 	str	r2, [r3]
   11918:	e3a03001 	mov	r3, #1	; 0x1
   1191c:	e59fe224 	ldr	lr, [pc, #548]	; 11b48 <control_send+0x258>
   11920:	e59f8224 	ldr	r8, [pc, #548]	; 11b4c <control_send+0x25c>
   11924:	e59f6224 	ldr	r6, [pc, #548]	; 11b50 <control_send+0x260>
   11928:	e59f7224 	ldr	r7, [pc, #548]	; 11b54 <control_send+0x264>
   1192c:	e59fc224 	ldr	ip, [pc, #548]	; 11b58 <control_send+0x268>
   11930:	e5c43000 	strb	r3, [r4]
   11934:	e59f3220 	ldr	r3, [pc, #544]	; 11b5c <control_send+0x26c>
   11938:	e24dd014 	sub	sp, sp, #20	; 0x14
   1193c:	e5c52000 	strb	r2, [r5]
   11940:	e5c32000 	strb	r2, [r3]
   11944:	e58d0004 	str	r0, [sp, #4]
   11948:	e1a0b001 	mov	fp, r1
   1194c:	e5c82000 	strb	r2, [r8]
   11950:	e5c62000 	strb	r2, [r6]
   11954:	e5c72000 	strb	r2, [r7]
   11958:	e5ce2000 	strb	r2, [lr]
   1195c:	e5cc2000 	strb	r2, [ip]
   11960:	e1a0a002 	mov	sl, r2
   11964:	e1a0900e 	mov	r9, lr
   11968:	e59f51f0 	ldr	r5, [pc, #496]	; 11b60 <control_send+0x270>
   1196c:	ea000000 	b	11974 <control_send+0x84>
   11970:	e3a0a000 	mov	sl, #0	; 0x0
   11974:	e3a00000 	mov	r0, #0	; 0x0
   11978:	e28d100f 	add	r1, sp, #15	; 0xf
   1197c:	ebffffc6 	bl	1189c <CYGACC_COMM_IF_GETC_TIMEOUT>
   11980:	e3500000 	cmp	r0, #0	; 0x0
   11984:	0afffffa 	beq	11974 <control_send+0x84>
   11988:	e5dd300f 	ldrb	r3, [sp, #15]
   1198c:	e3530018 	cmp	r3, #24	; 0x18
   11990:	0a000042 	beq	11aa0 <control_send+0x1b0>
   11994:	8a000004 	bhi	119ac <control_send+0xbc>
   11998:	e3530006 	cmp	r3, #6	; 0x6
   1199c:	0a00002c 	beq	11a54 <control_send+0x164>
   119a0:	e3530015 	cmp	r3, #21	; 0x15
   119a4:	1afffff1 	bne	11970 <control_send+0x80>
   119a8:	ea000010 	b	119f0 <control_send+0x100>
   119ac:	e3530043 	cmp	r3, #67	; 0x43
   119b0:	0a000001 	beq	119bc <control_send+0xcc>
   119b4:	e3530063 	cmp	r3, #99	; 0x63
   119b8:	1affffec 	bne	11970 <control_send+0x80>
   119bc:	e5d73000 	ldrb	r3, [r7]
   119c0:	e5d42000 	ldrb	r2, [r4]
   119c4:	e5d61000 	ldrb	r1, [r6]
   119c8:	e1833002 	orr	r3, r3, r2
   119cc:	e1913003 	orrs	r3, r1, r3
   119d0:	0affffe6 	beq	11970 <control_send+0x80>
   119d4:	e3510000 	cmp	r1, #0	; 0x0
   119d8:	159f2178 	ldrne	r2, [pc, #376]	; 11b58 <control_send+0x268>
   119dc:	13a03001 	movne	r3, #1	; 0x1
   119e0:	15c23000 	strbne	r3, [r2]
   119e4:	e3a03001 	mov	r3, #1	; 0x1
   119e8:	e5c53000 	strb	r3, [r5]
   119ec:	ea00002f 	b	11ab0 <control_send+0x1c0>
   119f0:	e59f313c 	ldr	r3, [pc, #316]	; 11b34 <control_send+0x244>
   119f4:	e5d33000 	ldrb	r3, [r3]
   119f8:	e3530000 	cmp	r3, #0	; 0x0
   119fc:	0a000008 	beq	11a24 <control_send+0x134>
   11a00:	e59f2138 	ldr	r2, [pc, #312]	; 11b40 <control_send+0x250>
   11a04:	e5923000 	ldr	r3, [r2]
   11a08:	e2433001 	sub	r3, r3, #1	; 0x1
   11a0c:	e5823000 	str	r3, [r2]
   11a10:	e3a03001 	mov	r3, #1	; 0x1
   11a14:	e5c43000 	strb	r3, [r4]
   11a18:	e3a03000 	mov	r3, #0	; 0x0
   11a1c:	e5c93000 	strb	r3, [r9]
   11a20:	ea000022 	b	11ab0 <control_send+0x1c0>
   11a24:	e5d42000 	ldrb	r2, [r4]
   11a28:	e3520000 	cmp	r2, #0	; 0x0
   11a2c:	15c53000 	strbne	r3, [r5]
   11a30:	1a000007 	bne	11a54 <control_send+0x164>
   11a34:	e5d93000 	ldrb	r3, [r9]
   11a38:	e3530000 	cmp	r3, #0	; 0x0
   11a3c:	15c92000 	strbne	r2, [r9]
   11a40:	e59f20f8 	ldr	r2, [pc, #248]	; 11b40 <control_send+0x250>
   11a44:	e5923000 	ldr	r3, [r2]
   11a48:	e2433001 	sub	r3, r3, #1	; 0x1
   11a4c:	e5823000 	str	r3, [r2]
   11a50:	ea000016 	b	11ab0 <control_send+0x1c0>
   11a54:	e5d42000 	ldrb	r2, [r4]
   11a58:	e3520000 	cmp	r2, #0	; 0x0
   11a5c:	1affffc3 	bne	11970 <control_send+0x80>
   11a60:	e5d91000 	ldrb	r1, [r9]
   11a64:	e3510000 	cmp	r1, #0	; 0x0
   11a68:	0a000005 	beq	11a84 <control_send+0x194>
   11a6c:	e59f30c0 	ldr	r3, [pc, #192]	; 11b34 <control_send+0x244>
   11a70:	e1a0a002 	mov	sl, r2
   11a74:	e5c32000 	strb	r2, [r3]
   11a78:	e3a03001 	mov	r3, #1	; 0x1
   11a7c:	e5c73000 	strb	r3, [r7]
   11a80:	eaffffbb 	b	11974 <control_send+0x84>
   11a84:	e5d83000 	ldrb	r3, [r8]
   11a88:	e3530000 	cmp	r3, #0	; 0x0
   11a8c:	13a03001 	movne	r3, #1	; 0x1
   11a90:	15c63000 	strbne	r3, [r6]
   11a94:	11a0a001 	movne	sl, r1
   11a98:	1affffb5 	bne	11974 <control_send+0x84>
   11a9c:	ea000003 	b	11ab0 <control_send+0x1c0>
   11aa0:	e28aa001 	add	sl, sl, #1	; 0x1
   11aa4:	e35a0002 	cmp	sl, #2	; 0x2
   11aa8:	1affffb1 	bne	11974 <control_send+0x84>
   11aac:	ea00001d 	b	11b28 <control_send+0x238>
   11ab0:	e59f20a0 	ldr	r2, [pc, #160]	; 11b58 <control_send+0x268>
   11ab4:	e5d23000 	ldrb	r3, [r2]
   11ab8:	e3530000 	cmp	r3, #0	; 0x0
   11abc:	1a000008 	bne	11ae4 <control_send+0x1f4>
   11ac0:	e59f207c 	ldr	r2, [pc, #124]	; 11b44 <control_send+0x254>
   11ac4:	e5d23000 	ldrb	r3, [r2]
   11ac8:	e3530000 	cmp	r3, #0	; 0x0
   11acc:	0a000004 	beq	11ae4 <control_send+0x1f4>
   11ad0:	e3a00004 	mov	r0, #4	; 0x4
   11ad4:	ebfffa22 	bl	10364 <putc>
   11ad8:	e3a03001 	mov	r3, #1	; 0x1
   11adc:	e5c83000 	strb	r3, [r8]
   11ae0:	eaffffa2 	b	11970 <control_send+0x80>
   11ae4:	e59d0004 	ldr	r0, [sp, #4]
   11ae8:	e1a0100b 	mov	r1, fp
   11aec:	ebffff24 	bl	11784 <real_send>
   11af0:	e3500003 	cmp	r0, #3	; 0x3
   11af4:	0affff9d 	beq	11970 <control_send+0x80>
   11af8:	e3500000 	cmp	r0, #0	; 0x0
   11afc:	1a00000a 	bne	11b2c <control_send+0x23c>
   11b00:	e5d93000 	ldrb	r3, [r9]
   11b04:	e3530000 	cmp	r3, #0	; 0x0
   11b08:	e5d43000 	ldrb	r3, [r4]
   11b0c:	15c90000 	strbne	r0, [r9]
   11b10:	e3530000 	cmp	r3, #0	; 0x0
   11b14:	13a03000 	movne	r3, #0	; 0x0
   11b18:	15c43000 	strbne	r3, [r4]
   11b1c:	12833001 	addne	r3, r3, #1	; 0x1
   11b20:	15c93000 	strbne	r3, [r9]
   11b24:	eaffff91 	b	11970 <control_send+0x80>
   11b28:	e3a00000 	mov	r0, #0	; 0x0
   11b2c:	e28dd014 	add	sp, sp, #20	; 0x14
   11b30:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   11b34:	3c00d249 	.word	0x3c00d249
   11b38:	3c00d24a 	.word	0x3c00d24a
   11b3c:	3c00d124 	.word	0x3c00d124
   11b40:	3c00d24c 	.word	0x3c00d24c
   11b44:	3c00d250 	.word	0x3c00d250
   11b48:	3c00d253 	.word	0x3c00d253
   11b4c:	3c00d248 	.word	0x3c00d248
   11b50:	3c00d251 	.word	0x3c00d251
   11b54:	3c00d252 	.word	0x3c00d252
   11b58:	3c00d255 	.word	0x3c00d255
   11b5c:	3c00d254 	.word	0x3c00d254
   11b60:	3c00d6f0 	.word	0x3c00d6f0

00011b64 <xyzModem_stream_terminate>:
   11b64:	e92d4030 	push	{r4, r5, lr}
   11b68:	e3500000 	cmp	r0, #0	; 0x0
   11b6c:	e24dd00c 	sub	sp, sp, #12	; 0xc
   11b70:	e1a04001 	mov	r4, r1
   11b74:	0a000020 	beq	11bfc <xyzModem_stream_terminate+0x98>
   11b78:	e59f4098 	ldr	r4, [pc, #152]	; 11c18 <xyzModem_stream_terminate+0xb4>
   11b7c:	e5943414 	ldr	r3, [r4, #1044]
   11b80:	e2433001 	sub	r3, r3, #1	; 0x1
   11b84:	e3530001 	cmp	r3, #1	; 0x1
   11b88:	8a000020 	bhi	11c10 <xyzModem_stream_terminate+0xac>
   11b8c:	e3a00018 	mov	r0, #24	; 0x18
   11b90:	ebfff9f3 	bl	10364 <putc>
   11b94:	e3a00018 	mov	r0, #24	; 0x18
   11b98:	ebfff9f1 	bl	10364 <putc>
   11b9c:	e3a00018 	mov	r0, #24	; 0x18
   11ba0:	ebfff9ef 	bl	10364 <putc>
   11ba4:	e3a00018 	mov	r0, #24	; 0x18
   11ba8:	ebfff9ed 	bl	10364 <putc>
   11bac:	e3a00008 	mov	r0, #8	; 0x8
   11bb0:	ebfff9eb 	bl	10364 <putc>
   11bb4:	e3a00008 	mov	r0, #8	; 0x8
   11bb8:	ebfff9e9 	bl	10364 <putc>
   11bbc:	e3a00008 	mov	r0, #8	; 0x8
   11bc0:	ebfff9e7 	bl	10364 <putc>
   11bc4:	e3a00008 	mov	r0, #8	; 0x8
   11bc8:	ebfff9e5 	bl	10364 <putc>
   11bcc:	e1a05004 	mov	r5, r4
   11bd0:	e28d4007 	add	r4, sp, #7	; 0x7
   11bd4:	e5953000 	ldr	r3, [r5]
   11bd8:	e1a01004 	mov	r1, r4
   11bdc:	e5d30000 	ldrb	r0, [r3]
   11be0:	ebffff2d 	bl	1189c <CYGACC_COMM_IF_GETC_TIMEOUT>
   11be4:	e3500000 	cmp	r0, #0	; 0x0
   11be8:	1afffff9 	bne	11bd4 <xyzModem_stream_terminate+0x70>
   11bec:	e59f3024 	ldr	r3, [pc, #36]	; 11c18 <xyzModem_stream_terminate+0xb4>
   11bf0:	e3a02001 	mov	r2, #1	; 0x1
   11bf4:	e583242c 	str	r2, [r3, #1068]
   11bf8:	ea000004 	b	11c10 <xyzModem_stream_terminate+0xac>
   11bfc:	e12fff34 	blx	r4
   11c00:	e3500000 	cmp	r0, #0	; 0x0
   11c04:	aafffffc 	bge	11bfc <xyzModem_stream_terminate+0x98>
   11c08:	e59f000c 	ldr	r0, [pc, #12]	; 11c1c <xyzModem_stream_terminate+0xb8>
   11c0c:	ebffe640 	bl	b514 <udelay>
   11c10:	e28dd00c 	add	sp, sp, #12	; 0xc
   11c14:	e8bd8030 	pop	{r4, r5, pc}
   11c18:	3c00d258 	.word	0x3c00d258
   11c1c:	0003d090 	.word	0x0003d090

00011c20 <xyzModem_get_hdr>:
   11c20:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   11c24:	e59f42a0 	ldr	r4, [pc, #672]	; 11ecc <xyzModem_get_hdr+0x2ac>
   11c28:	e24dd00c 	sub	sp, sp, #12	; 0xc
   11c2c:	e5943430 	ldr	r3, [r4, #1072]
   11c30:	e3530000 	cmp	r3, #0	; 0x0
   11c34:	0a000003 	beq	11c48 <xyzModem_get_hdr+0x28>
   11c38:	e3a00006 	mov	r0, #6	; 0x6
   11c3c:	ebfff9c8 	bl	10364 <putc>
   11c40:	e3a03000 	mov	r3, #0	; 0x0
   11c44:	e5843430 	str	r3, [r4, #1072]
   11c48:	e3a04000 	mov	r4, #0	; 0x0
   11c4c:	e59f6278 	ldr	r6, [pc, #632]	; 11ecc <xyzModem_get_hdr+0x2ac>
   11c50:	e1a05004 	mov	r5, r4
   11c54:	e28d7007 	add	r7, sp, #7	; 0x7
   11c58:	e5963000 	ldr	r3, [r6]
   11c5c:	e1a01007 	mov	r1, r7
   11c60:	e5d30000 	ldrb	r0, [r3]
   11c64:	ebffff0c 	bl	1189c <CYGACC_COMM_IF_GETC_TIMEOUT>
   11c68:	e3500000 	cmp	r0, #0	; 0x0
   11c6c:	e2855001 	add	r5, r5, #1	; 0x1
   11c70:	e59f2254 	ldr	r2, [pc, #596]	; 11ecc <xyzModem_get_hdr+0x2ac>
   11c74:	0a000021 	beq	11d00 <xyzModem_get_hdr+0xe0>
   11c78:	e5dd3007 	ldrb	r3, [sp, #7]
   11c7c:	e3530002 	cmp	r3, #2	; 0x2
   11c80:	0a00000c 	beq	11cb8 <xyzModem_get_hdr+0x98>
   11c84:	8a000002 	bhi	11c94 <xyzModem_get_hdr+0x74>
   11c88:	e3530001 	cmp	r3, #1	; 0x1
   11c8c:	1afffff1 	bne	11c58 <xyzModem_get_hdr+0x38>
   11c90:	ea000004 	b	11ca8 <xyzModem_get_hdr+0x88>
   11c94:	e3530004 	cmp	r3, #4	; 0x4
   11c98:	0a000012 	beq	11ce8 <xyzModem_get_hdr+0xc8>
   11c9c:	e3530018 	cmp	r3, #24	; 0x18
   11ca0:	1affffec 	bne	11c58 <xyzModem_get_hdr+0x38>
   11ca4:	ea000007 	b	11cc8 <xyzModem_get_hdr+0xa8>
   11ca8:	e592341c 	ldr	r3, [r2, #1052]
   11cac:	e2833001 	add	r3, r3, #1	; 0x1
   11cb0:	e582341c 	str	r3, [r2, #1052]
   11cb4:	ea00007c 	b	11eac <xyzModem_get_hdr+0x28c>
   11cb8:	e5923420 	ldr	r3, [r2, #1056]
   11cbc:	e2833001 	add	r3, r3, #1	; 0x1
   11cc0:	e5823420 	str	r3, [r2, #1056]
   11cc4:	ea000078 	b	11eac <xyzModem_get_hdr+0x28c>
   11cc8:	e5963424 	ldr	r3, [r6, #1060]
   11ccc:	e2844001 	add	r4, r4, #1	; 0x1
   11cd0:	e2833001 	add	r3, r3, #1	; 0x1
   11cd4:	e3540003 	cmp	r4, #3	; 0x3
   11cd8:	e5863424 	str	r3, [r6, #1060]
   11cdc:	1affffdd 	bne	11c58 <xyzModem_get_hdr+0x38>
   11ce0:	e3e00004 	mvn	r0, #4	; 0x4
   11ce4:	ea00006e 	b	11ea4 <xyzModem_get_hdr+0x284>
   11ce8:	e3550001 	cmp	r5, #1	; 0x1
   11cec:	1affffd9 	bne	11c58 <xyzModem_get_hdr+0x38>
   11cf0:	e3a00006 	mov	r0, #6	; 0x6
   11cf4:	ebfff99a 	bl	10364 <putc>
   11cf8:	e3e00003 	mvn	r0, #3	; 0x3
   11cfc:	ea000068 	b	11ea4 <xyzModem_get_hdr+0x284>
   11d00:	e1a05002 	mov	r5, r2
   11d04:	e28d4006 	add	r4, sp, #6	; 0x6
   11d08:	e5953000 	ldr	r3, [r5]
   11d0c:	e1a01004 	mov	r1, r4
   11d10:	e5d30000 	ldrb	r0, [r3]
   11d14:	ebfffee0 	bl	1189c <CYGACC_COMM_IF_GETC_TIMEOUT>
   11d18:	e3500000 	cmp	r0, #0	; 0x0
   11d1c:	1afffff9 	bne	11d08 <xyzModem_get_hdr+0xe8>
   11d20:	e59f01a8 	ldr	r0, [pc, #424]	; 11ed0 <xyzModem_get_hdr+0x2b0>
   11d24:	ebffe5fa 	bl	b514 <udelay>
   11d28:	ea00005a 	b	11e98 <xyzModem_get_hdr+0x278>
   11d2c:	e5943000 	ldr	r3, [r4]
   11d30:	e59f119c 	ldr	r1, [pc, #412]	; 11ed4 <xyzModem_get_hdr+0x2b4>
   11d34:	e5d30000 	ldrb	r0, [r3]
   11d38:	ebfffed7 	bl	1189c <CYGACC_COMM_IF_GETC_TIMEOUT>
   11d3c:	e3500000 	cmp	r0, #0	; 0x0
   11d40:	0a000054 	beq	11e98 <xyzModem_get_hdr+0x278>
   11d44:	e28d5008 	add	r5, sp, #8	; 0x8
   11d48:	e5753001 	ldrb	r3, [r5, #-1]!
   11d4c:	e2842004 	add	r2, r4, #4	; 0x4
   11d50:	e3530001 	cmp	r3, #1	; 0x1
   11d54:	03a03080 	moveq	r3, #128	; 0x80
   11d58:	13a03b01 	movne	r3, #1024	; 0x400
   11d5c:	e5843410 	str	r3, [r4, #1040]
   11d60:	e5842404 	str	r2, [r4, #1028]
   11d64:	e1a07004 	mov	r7, r4
   11d68:	e3a04000 	mov	r4, #0	; 0x0
   11d6c:	ea000008 	b	11d94 <xyzModem_get_hdr+0x174>
   11d70:	e5973000 	ldr	r3, [r7]
   11d74:	e5d30000 	ldrb	r0, [r3]
   11d78:	ebfffec7 	bl	1189c <CYGACC_COMM_IF_GETC_TIMEOUT>
   11d7c:	e3500000 	cmp	r0, #0	; 0x0
   11d80:	e0872004 	add	r2, r7, r4
   11d84:	e2844001 	add	r4, r4, #1	; 0x1
   11d88:	0a000042 	beq	11e98 <xyzModem_get_hdr+0x278>
   11d8c:	e5dd3007 	ldrb	r3, [sp, #7]
   11d90:	e5c23004 	strb	r3, [r2, #4]
   11d94:	e5973410 	ldr	r3, [r7, #1040]
   11d98:	e1a01005 	mov	r1, r5
   11d9c:	e1540003 	cmp	r4, r3
   11da0:	e59f6124 	ldr	r6, [pc, #292]	; 11ecc <xyzModem_get_hdr+0x2ac>
   11da4:	bafffff1 	blt	11d70 <xyzModem_get_hdr+0x150>
   11da8:	e5963000 	ldr	r3, [r6]
   11dac:	e59f1124 	ldr	r1, [pc, #292]	; 11ed8 <xyzModem_get_hdr+0x2b8>
   11db0:	e5d30000 	ldrb	r0, [r3]
   11db4:	ebfffeb8 	bl	1189c <CYGACC_COMM_IF_GETC_TIMEOUT>
   11db8:	e3500000 	cmp	r0, #0	; 0x0
   11dbc:	0a000035 	beq	11e98 <xyzModem_get_hdr+0x278>
   11dc0:	e5963428 	ldr	r3, [r6, #1064]
   11dc4:	e3530000 	cmp	r3, #0	; 0x0
   11dc8:	0a000005 	beq	11de4 <xyzModem_get_hdr+0x1c4>
   11dcc:	e5963000 	ldr	r3, [r6]
   11dd0:	e59f1104 	ldr	r1, [pc, #260]	; 11edc <xyzModem_get_hdr+0x2bc>
   11dd4:	e5d30000 	ldrb	r0, [r3]
   11dd8:	ebfffeaf 	bl	1189c <CYGACC_COMM_IF_GETC_TIMEOUT>
   11ddc:	e3500000 	cmp	r0, #0	; 0x0
   11de0:	0a00002c 	beq	11e98 <xyzModem_get_hdr+0x278>
   11de4:	e59f40e0 	ldr	r4, [pc, #224]	; 11ecc <xyzModem_get_hdr+0x2ac>
   11de8:	e5d43409 	ldrb	r3, [r4, #1033]
   11dec:	e5d42408 	ldrb	r2, [r4, #1032]
   11df0:	e0233002 	eor	r3, r3, r2
   11df4:	e35300ff 	cmp	r3, #255	; 0xff
   11df8:	0a000009 	beq	11e24 <xyzModem_get_hdr+0x204>
   11dfc:	e1a05004 	mov	r5, r4
   11e00:	e28d4006 	add	r4, sp, #6	; 0x6
   11e04:	e5953000 	ldr	r3, [r5]
   11e08:	e1a01004 	mov	r1, r4
   11e0c:	e5d30000 	ldrb	r0, [r3]
   11e10:	ebfffea1 	bl	1189c <CYGACC_COMM_IF_GETC_TIMEOUT>
   11e14:	e3500000 	cmp	r0, #0	; 0x0
   11e18:	1afffff9 	bne	11e04 <xyzModem_get_hdr+0x1e4>
   11e1c:	e3e00005 	mvn	r0, #5	; 0x5
   11e20:	ea00001f 	b	11ea4 <xyzModem_get_hdr+0x284>
   11e24:	e5943428 	ldr	r3, [r4, #1064]
   11e28:	e3530000 	cmp	r3, #0	; 0x0
   11e2c:	01a02003 	moveq	r2, r3
   11e30:	01a0c004 	moveq	ip, r4
   11e34:	05940410 	ldreq	r0, [r4, #1040]
   11e38:	01a01003 	moveq	r1, r3
   11e3c:	0a00000a 	beq	11e6c <xyzModem_get_hdr+0x24c>
   11e40:	e2840004 	add	r0, r4, #4	; 0x4
   11e44:	e5941410 	ldr	r1, [r4, #1040]
   11e48:	eb000110 	bl	12290 <cyg_crc16>
   11e4c:	e5d4340b 	ldrb	r3, [r4, #1035]
   11e50:	e5d4240a 	ldrb	r2, [r4, #1034]
   11e54:	e1833402 	orr	r3, r3, r2, lsl #8
   11e58:	e1500003 	cmp	r0, r3
   11e5c:	ea00000a 	b	11e8c <xyzModem_get_hdr+0x26c>
   11e60:	e5d33004 	ldrb	r3, [r3, #4]
   11e64:	e0813003 	add	r3, r1, r3
   11e68:	e6ff1073 	uxth	r1, r3
   11e6c:	e1520000 	cmp	r2, r0
   11e70:	e08c3002 	add	r3, ip, r2
   11e74:	e2822001 	add	r2, r2, #1	; 0x1
   11e78:	bafffff8 	blt	11e60 <xyzModem_get_hdr+0x240>
   11e7c:	e59f3048 	ldr	r3, [pc, #72]	; 11ecc <xyzModem_get_hdr+0x2ac>
   11e80:	e5d3240a 	ldrb	r2, [r3, #1034]
   11e84:	e20130ff 	and	r3, r1, #255	; 0xff
   11e88:	e1520003 	cmp	r2, r3
   11e8c:	13e00006 	mvnne	r0, #6	; 0x6
   11e90:	1a000003 	bne	11ea4 <xyzModem_get_hdr+0x284>
   11e94:	ea000001 	b	11ea0 <xyzModem_get_hdr+0x280>
   11e98:	e3e00002 	mvn	r0, #2	; 0x2
   11e9c:	ea000000 	b	11ea4 <xyzModem_get_hdr+0x284>
   11ea0:	e3a00000 	mov	r0, #0	; 0x0
   11ea4:	e28dd00c 	add	sp, sp, #12	; 0xc
   11ea8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   11eac:	e59f4018 	ldr	r4, [pc, #24]	; 11ecc <xyzModem_get_hdr+0x2ac>
   11eb0:	e59f1028 	ldr	r1, [pc, #40]	; 11ee0 <xyzModem_get_hdr+0x2c0>
   11eb4:	e5943000 	ldr	r3, [r4]
   11eb8:	e5d30000 	ldrb	r0, [r3]
   11ebc:	ebfffe76 	bl	1189c <CYGACC_COMM_IF_GETC_TIMEOUT>
   11ec0:	e3500000 	cmp	r0, #0	; 0x0
   11ec4:	0afffff3 	beq	11e98 <xyzModem_get_hdr+0x278>
   11ec8:	eaffff97 	b	11d2c <xyzModem_get_hdr+0x10c>
   11ecc:	3c00d258 	.word	0x3c00d258
   11ed0:	0003d090 	.word	0x0003d090
   11ed4:	3c00d661 	.word	0x3c00d661
   11ed8:	3c00d662 	.word	0x3c00d662
   11edc:	3c00d663 	.word	0x3c00d663
   11ee0:	3c00d660 	.word	0x3c00d660

00011ee4 <xyzModem_stream_read>:
   11ee4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   11ee8:	e59f5240 	ldr	r5, [pc, #576]	; 12130 <xyzModem_stream_read+0x24c>
   11eec:	e24dd004 	sub	sp, sp, #4	; 0x4
   11ef0:	e1a08000 	mov	r8, r0
   11ef4:	e1a07001 	mov	r7, r1
   11ef8:	e1a0b002 	mov	fp, r2
   11efc:	e3e06004 	mvn	r6, #4	; 0x4
   11f00:	e3a0a000 	mov	sl, #0	; 0x0
   11f04:	e1a09005 	mov	r9, r5
   11f08:	ea00007e 	b	12108 <xyzModem_stream_read+0x224>
   11f0c:	e5953410 	ldr	r3, [r5, #1040]
   11f10:	e3530000 	cmp	r3, #0	; 0x0
   11f14:	03a04014 	moveq	r4, #20	; 0x14
   11f18:	0a00005d 	beq	12094 <xyzModem_stream_read+0x1b0>
   11f1c:	ea000065 	b	120b8 <xyzModem_stream_read+0x1d4>
   11f20:	ebffff3e 	bl	11c20 <xyzModem_get_hdr>
   11f24:	e2506000 	subs	r6, r0, #0	; 0x0
   11f28:	1a000039 	bne	12014 <xyzModem_stream_read+0x130>
   11f2c:	e5d5240c 	ldrb	r2, [r5, #1036]
   11f30:	e5d51408 	ldrb	r1, [r5, #1032]
   11f34:	e2423001 	sub	r3, r2, #1	; 0x1
   11f38:	e1510002 	cmp	r1, r2
   11f3c:	e20330ff 	and	r3, r3, #255	; 0xff
   11f40:	1a00002d 	bne	11ffc <xyzModem_stream_read+0x118>
   11f44:	e599c414 	ldr	ip, [r9, #1044]
   11f48:	e2813001 	add	r3, r1, #1	; 0x1
   11f4c:	e5c9340c 	strb	r3, [r9, #1036]
   11f50:	e35c0001 	cmp	ip, #1	; 0x1
   11f54:	e3a03001 	mov	r3, #1	; 0x1
   11f58:	e5893430 	str	r3, [r9, #1072]
   11f5c:	0a000002 	beq	11f6c <xyzModem_stream_read+0x88>
   11f60:	e5993434 	ldr	r3, [r9, #1076]
   11f64:	e3530000 	cmp	r3, #0	; 0x0
   11f68:	1a000017 	bne	11fcc <xyzModem_stream_read+0xe8>
   11f6c:	e5953410 	ldr	r3, [r5, #1040]
   11f70:	e5950404 	ldr	r0, [r5, #1028]
   11f74:	e0802003 	add	r2, r0, r3
   11f78:	e5523001 	ldrb	r3, [r2, #-1]
   11f7c:	e353001a 	cmp	r3, #26	; 0x1a
   11f80:	1a00000f 	bne	11fc4 <xyzModem_stream_read+0xe0>
   11f84:	e5523002 	ldrb	r3, [r2, #-2]
   11f88:	e353001a 	cmp	r3, #26	; 0x1a
   11f8c:	1a00000c 	bne	11fc4 <xyzModem_stream_read+0xe0>
   11f90:	e5523003 	ldrb	r3, [r2, #-3]
   11f94:	e353001a 	cmp	r3, #26	; 0x1a
   11f98:	1a000009 	bne	11fc4 <xyzModem_stream_read+0xe0>
   11f9c:	ea000000 	b	11fa4 <xyzModem_stream_read+0xc0>
   11fa0:	e5851410 	str	r1, [r5, #1040]
   11fa4:	e5953410 	ldr	r3, [r5, #1040]
   11fa8:	e3530000 	cmp	r3, #0	; 0x0
   11fac:	e0802003 	add	r2, r0, r3
   11fb0:	e2431001 	sub	r1, r3, #1	; 0x1
   11fb4:	0a000002 	beq	11fc4 <xyzModem_stream_read+0xe0>
   11fb8:	e5523001 	ldrb	r3, [r2, #-1]
   11fbc:	e353001a 	cmp	r3, #26	; 0x1a
   11fc0:	0afffff6 	beq	11fa0 <xyzModem_stream_read+0xbc>
   11fc4:	e35c0001 	cmp	ip, #1	; 0x1
   11fc8:	0a00003a 	beq	120b8 <xyzModem_stream_read+0x1d4>
   11fcc:	e5951434 	ldr	r1, [r5, #1076]
   11fd0:	e3510000 	cmp	r1, #0	; 0x0
   11fd4:	0a000037 	beq	120b8 <xyzModem_stream_read+0x1d4>
   11fd8:	e5953438 	ldr	r3, [r5, #1080]
   11fdc:	e5950410 	ldr	r0, [r5, #1040]
   11fe0:	e0802003 	add	r2, r0, r3
   11fe4:	e1520001 	cmp	r2, r1
   11fe8:	80803001 	addhi	r3, r0, r1
   11fec:	80623003 	rsbhi	r3, r2, r3
   11ff0:	e5852438 	str	r2, [r5, #1080]
   11ff4:	85853410 	strhi	r3, [r5, #1040]
   11ff8:	ea00002e 	b	120b8 <xyzModem_stream_read+0x1d4>
   11ffc:	e1510003 	cmp	r1, r3
   12000:	13e06007 	mvnne	r6, #7	; 0x7
   12004:	1a00001a 	bne	12074 <xyzModem_stream_read+0x190>
   12008:	e3a00006 	mov	r0, #6	; 0x6
   1200c:	ebfff8d4 	bl	10364 <putc>
   12010:	ea00001f 	b	12094 <xyzModem_stream_read+0x1b0>
   12014:	e3760005 	cmn	r6, #5	; 0x5
   12018:	0a000021 	beq	120a4 <xyzModem_stream_read+0x1c0>
   1201c:	e3760004 	cmn	r6, #4	; 0x4
   12020:	1a000013 	bne	12074 <xyzModem_stream_read+0x190>
   12024:	e3a00006 	mov	r0, #6	; 0x6
   12028:	ebfff8cd 	bl	10364 <putc>
   1202c:	e5953414 	ldr	r3, [r5, #1044]
   12030:	e3530002 	cmp	r3, #2	; 0x2
   12034:	1a00000b 	bne	12068 <xyzModem_stream_read+0x184>
   12038:	e5950428 	ldr	r0, [r5, #1064]
   1203c:	e3500000 	cmp	r0, #0	; 0x0
   12040:	13a00043 	movne	r0, #67	; 0x43
   12044:	03a00015 	moveq	r0, #21	; 0x15
   12048:	ebfff8c5 	bl	10364 <putc>
   1204c:	e5953418 	ldr	r3, [r5, #1048]
   12050:	e2833001 	add	r3, r3, #1	; 0x1
   12054:	e5853418 	str	r3, [r5, #1048]
   12058:	ebfffef0 	bl	11c20 <xyzModem_get_hdr>
   1205c:	e1a06000 	mov	r6, r0
   12060:	e3a00006 	mov	r0, #6	; 0x6
   12064:	ebfff8be 	bl	10364 <putc>
   12068:	e3a03001 	mov	r3, #1	; 0x1
   1206c:	e585342c 	str	r3, [r5, #1068]
   12070:	ea000009 	b	1209c <xyzModem_stream_read+0x1b8>
   12074:	e5950428 	ldr	r0, [r5, #1064]
   12078:	e3500000 	cmp	r0, #0	; 0x0
   1207c:	13a00043 	movne	r0, #67	; 0x43
   12080:	03a00015 	moveq	r0, #21	; 0x15
   12084:	ebfff8b6 	bl	10364 <putc>
   12088:	e5953418 	ldr	r3, [r5, #1048]
   1208c:	e2833001 	add	r3, r3, #1	; 0x1
   12090:	e5853418 	str	r3, [r5, #1048]
   12094:	e2544001 	subs	r4, r4, #1	; 0x1
   12098:	2affffa0 	bcs	11f20 <xyzModem_stream_read+0x3c>
   1209c:	e3560000 	cmp	r6, #0	; 0x0
   120a0:	aa000004 	bge	120b8 <xyzModem_stream_read+0x1d4>
   120a4:	e59f3084 	ldr	r3, [pc, #132]	; 12130 <xyzModem_stream_read+0x24c>
   120a8:	e3e02000 	mvn	r2, #0	; 0x0
   120ac:	e58b6000 	str	r6, [fp]
   120b0:	e5832410 	str	r2, [r3, #1040]
   120b4:	ea00001a 	b	12124 <xyzModem_stream_read+0x240>
   120b8:	e595342c 	ldr	r3, [r5, #1068]
   120bc:	e3530000 	cmp	r3, #0	; 0x0
   120c0:	1a000010 	bne	12108 <xyzModem_stream_read+0x224>
   120c4:	e5954410 	ldr	r4, [r5, #1040]
   120c8:	e1a00008 	mov	r0, r8
   120cc:	e1570004 	cmp	r7, r4
   120d0:	b1a04007 	movlt	r4, r7
   120d4:	a1a04004 	movge	r4, r4
   120d8:	e1a02004 	mov	r2, r4
   120dc:	e5951404 	ldr	r1, [r5, #1028]
   120e0:	ebffbd42 	bl	15f0 <memcpy>
   120e4:	e5953410 	ldr	r3, [r5, #1040]
   120e8:	e5952404 	ldr	r2, [r5, #1028]
   120ec:	e0643003 	rsb	r3, r4, r3
   120f0:	e0822004 	add	r2, r2, r4
   120f4:	e5853410 	str	r3, [r5, #1040]
   120f8:	e5852404 	str	r2, [r5, #1028]
   120fc:	e0647007 	rsb	r7, r4, r7
   12100:	e0888004 	add	r8, r8, r4
   12104:	e08aa004 	add	sl, sl, r4
   12108:	e595342c 	ldr	r3, [r5, #1068]
   1210c:	e2733001 	rsbs	r3, r3, #1	; 0x1
   12110:	33a03000 	movcc	r3, #0	; 0x0
   12114:	e3570000 	cmp	r7, #0	; 0x0
   12118:	d3a03000 	movle	r3, #0	; 0x0
   1211c:	e3530000 	cmp	r3, #0	; 0x0
   12120:	1affff79 	bne	11f0c <xyzModem_stream_read+0x28>
   12124:	e1a0000a 	mov	r0, sl
   12128:	e28dd004 	add	sp, sp, #4	; 0x4
   1212c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   12130:	3c00d258 	.word	0x3c00d258

00012134 <xyzModem_stream_open>:
   12134:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
   12138:	e59f4140 	ldr	r4, [pc, #320]	; 12280 <xyzModem_stream_open+0x14c>
   1213c:	e24dd00c 	sub	sp, sp, #12	; 0xc
   12140:	e3a05000 	mov	r5, #0	; 0x0
   12144:	e28d3008 	add	r3, sp, #8	; 0x8
   12148:	e3a02001 	mov	r2, #1	; 0x1
   1214c:	e5235004 	str	r5, [r3, #-4]!
   12150:	e5842428 	str	r2, [r4, #1064]
   12154:	e5845410 	str	r5, [r4, #1040]
   12158:	e584542c 	str	r5, [r4, #1068]
   1215c:	e5845430 	str	r5, [r4, #1072]
   12160:	e5902004 	ldr	r2, [r0, #4]
   12164:	e5843000 	str	r3, [r4]
   12168:	e3a00043 	mov	r0, #67	; 0x43
   1216c:	e5842414 	str	r2, [r4, #1044]
   12170:	e5845418 	str	r5, [r4, #1048]
   12174:	e584541c 	str	r5, [r4, #1052]
   12178:	e5845420 	str	r5, [r4, #1056]
   1217c:	e5845424 	str	r5, [r4, #1060]
   12180:	e5845438 	str	r5, [r4, #1080]
   12184:	e5845434 	str	r5, [r4, #1076]
   12188:	e1a08001 	mov	r8, r1
   1218c:	ebfff874 	bl	10364 <putc>
   12190:	e5943414 	ldr	r3, [r4, #1044]
   12194:	e3530001 	cmp	r3, #1	; 0x1
   12198:	05c4340c 	strbeq	r3, [r4, #1036]
   1219c:	01a00005 	moveq	r0, r5
   121a0:	0a000034 	beq	12278 <xyzModem_stream_open+0x144>
   121a4:	e1a07004 	mov	r7, r4
   121a8:	e3a06014 	mov	r6, #20	; 0x14
   121ac:	e3a0400a 	mov	r4, #10	; 0xa
   121b0:	e1a0a005 	mov	sl, r5
   121b4:	ea00002b 	b	12268 <xyzModem_stream_open+0x134>
   121b8:	ebfffe98 	bl	11c20 <xyzModem_get_hdr>
   121bc:	e2505000 	subs	r5, r0, #0	; 0x0
   121c0:	1a000016 	bne	12220 <xyzModem_stream_open+0xec>
   121c4:	e59f20b4 	ldr	r2, [pc, #180]	; 12280 <xyzModem_stream_open+0x14c>
   121c8:	e5d23408 	ldrb	r3, [r2, #1032]
   121cc:	e3530000 	cmp	r3, #0	; 0x0
   121d0:	1a00000b 	bne	12204 <xyzModem_stream_open+0xd0>
   121d4:	e1a03002 	mov	r3, r2
   121d8:	e5930404 	ldr	r0, [r3, #1028]
   121dc:	e4d02001 	ldrb	r2, [r0], #1
   121e0:	e3520000 	cmp	r2, #0	; 0x0
   121e4:	e5830404 	str	r0, [r3, #1028]
   121e8:	1afffffa 	bne	121d8 <xyzModem_stream_open+0xa4>
   121ec:	e59f3090 	ldr	r3, [pc, #144]	; 12284 <xyzModem_stream_open+0x150>
   121f0:	e59f1090 	ldr	r1, [pc, #144]	; 12288 <xyzModem_stream_open+0x154>
   121f4:	ebfffb5a 	bl	10f64 <parse_num>
   121f8:	e59f3080 	ldr	r3, [pc, #128]	; 12280 <xyzModem_stream_open+0x14c>
   121fc:	e3a02001 	mov	r2, #1	; 0x1
   12200:	e5832430 	str	r2, [r3, #1072]
   12204:	e59f3074 	ldr	r3, [pc, #116]	; 12280 <xyzModem_stream_open+0x14c>
   12208:	e3a01000 	mov	r1, #0	; 0x0
   1220c:	e3a02001 	mov	r2, #1	; 0x1
   12210:	e5c3240c 	strb	r2, [r3, #1036]
   12214:	e5831410 	str	r1, [r3, #1040]
   12218:	e1a00001 	mov	r0, r1
   1221c:	ea000015 	b	12278 <xyzModem_stream_open+0x144>
   12220:	e3750003 	cmn	r5, #3	; 0x3
   12224:	1a00000d 	bne	12260 <xyzModem_stream_open+0x12c>
   12228:	e2444001 	sub	r4, r4, #1	; 0x1
   1222c:	e3540000 	cmp	r4, #0	; 0x0
   12230:	e59f0054 	ldr	r0, [pc, #84]	; 1228c <xyzModem_stream_open+0x158>
   12234:	d587a428 	strle	sl, [r7, #1064]
   12238:	ebffe4b5 	bl	b514 <udelay>
   1223c:	e5970428 	ldr	r0, [r7, #1064]
   12240:	e3500000 	cmp	r0, #0	; 0x0
   12244:	13a00043 	movne	r0, #67	; 0x43
   12248:	03a00015 	moveq	r0, #21	; 0x15
   1224c:	ebfff844 	bl	10364 <putc>
   12250:	e5973418 	ldr	r3, [r7, #1048]
   12254:	e2833001 	add	r3, r3, #1	; 0x1
   12258:	e5873418 	str	r3, [r7, #1048]
   1225c:	ea000001 	b	12268 <xyzModem_stream_open+0x134>
   12260:	e3750005 	cmn	r5, #5	; 0x5
   12264:	0a000001 	beq	12270 <xyzModem_stream_open+0x13c>
   12268:	e2566001 	subs	r6, r6, #1	; 0x1
   1226c:	2affffd1 	bcs	121b8 <xyzModem_stream_open+0x84>
   12270:	e5885000 	str	r5, [r8]
   12274:	e3e00000 	mvn	r0, #0	; 0x0
   12278:	e28dd00c 	add	sp, sp, #12	; 0xc
   1227c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
   12280:	3c00d258 	.word	0x3c00d258
   12284:	0001c389 	.word	0x0001c389
   12288:	3c00d68c 	.word	0x3c00d68c
   1228c:	0007a120 	.word	0x0007a120

00012290 <cyg_crc16>:
   12290:	e3a02000 	mov	r2, #0	; 0x0
   12294:	e92d4010 	push	{r4, lr}
   12298:	e1a0c000 	mov	ip, r0
   1229c:	e59f402c 	ldr	r4, [pc, #44]	; 122d0 <cyg_crc16+0x40>
   122a0:	e1a00002 	mov	r0, r2
   122a4:	ea000006 	b	122c4 <cyg_crc16+0x34>
   122a8:	e7dc3002 	ldrb	r3, [ip, r2]
   122ac:	e2822001 	add	r2, r2, #1	; 0x1
   122b0:	e0233420 	eor	r3, r3, r0, lsr #8
   122b4:	e1a03083 	lsl	r3, r3, #1
   122b8:	e19430b3 	ldrh	r3, [r4, r3]
   122bc:	e0233400 	eor	r3, r3, r0, lsl #8
   122c0:	e6ff0073 	uxth	r0, r3
   122c4:	e1520001 	cmp	r2, r1
   122c8:	bafffff6 	blt	122a8 <cyg_crc16+0x18>
   122cc:	e8bd8010 	pop	{r4, pc}
   122d0:	000199e8 	.word	0x000199e8

000122d4 <ss_jtag_en>:
   122d4:	e59f300c 	ldr	r3, [pc, #12]	; 122e8 <ss_jtag_en+0x14>
   122d8:	e2500000 	subs	r0, r0, #0	; 0x0
   122dc:	13a00001 	movne	r0, #1	; 0x1
   122e0:	e58308b8 	str	r0, [r3, #2232]
   122e4:	e12fff1e 	bx	lr
   122e8:	21e08000 	.word	0x21e08000

000122ec <raise>:
   122ec:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   122f0:	e1a01000 	mov	r1, r0
   122f4:	e24dd004 	sub	sp, sp, #4	; 0x4
   122f8:	e59f0014 	ldr	r0, [pc, #20]	; 12314 <raise+0x28>
   122fc:	e59f3014 	ldr	r3, [pc, #20]	; 12318 <raise+0x2c>
   12300:	e1a0e00f 	mov	lr, pc
   12304:	e593f000 	ldr	pc, [r3]
   12308:	e3a00000 	mov	r0, #0	; 0x0
   1230c:	e28dd004 	add	sp, sp, #4	; 0x4
   12310:	e8bd8000 	pop	{pc}
   12314:	0001c625 	.word	0x0001c625
   12318:	3c00d694 	.word	0x3c00d694

0001231c <__aeabi_uidiv>:
   1231c:	e2512001 	subs	r2, r1, #1	; 0x1
   12320:	012fff1e 	bxeq	lr
   12324:	3a000074 	bcc	124fc <__aeabi_uidiv+0x1e0>
   12328:	e1500001 	cmp	r0, r1
   1232c:	9a00006b 	bls	124e0 <__aeabi_uidiv+0x1c4>
   12330:	e1110002 	tst	r1, r2
   12334:	0a00006c 	beq	124ec <__aeabi_uidiv+0x1d0>
   12338:	e16f3f10 	clz	r3, r0
   1233c:	e16f2f11 	clz	r2, r1
   12340:	e0423003 	sub	r3, r2, r3
   12344:	e273301f 	rsbs	r3, r3, #31	; 0x1f
   12348:	10833083 	addne	r3, r3, r3, lsl #1
   1234c:	e3a02000 	mov	r2, #0	; 0x0
   12350:	108ff103 	addne	pc, pc, r3, lsl #2
   12354:	e1a00000 	nop			(mov r0,r0)
   12358:	e1500f81 	cmp	r0, r1, lsl #31
   1235c:	e0a22002 	adc	r2, r2, r2
   12360:	20400f81 	subcs	r0, r0, r1, lsl #31
   12364:	e1500f01 	cmp	r0, r1, lsl #30
   12368:	e0a22002 	adc	r2, r2, r2
   1236c:	20400f01 	subcs	r0, r0, r1, lsl #30
   12370:	e1500e81 	cmp	r0, r1, lsl #29
   12374:	e0a22002 	adc	r2, r2, r2
   12378:	20400e81 	subcs	r0, r0, r1, lsl #29
   1237c:	e1500e01 	cmp	r0, r1, lsl #28
   12380:	e0a22002 	adc	r2, r2, r2
   12384:	20400e01 	subcs	r0, r0, r1, lsl #28
   12388:	e1500d81 	cmp	r0, r1, lsl #27
   1238c:	e0a22002 	adc	r2, r2, r2
   12390:	20400d81 	subcs	r0, r0, r1, lsl #27
   12394:	e1500d01 	cmp	r0, r1, lsl #26
   12398:	e0a22002 	adc	r2, r2, r2
   1239c:	20400d01 	subcs	r0, r0, r1, lsl #26
   123a0:	e1500c81 	cmp	r0, r1, lsl #25
   123a4:	e0a22002 	adc	r2, r2, r2
   123a8:	20400c81 	subcs	r0, r0, r1, lsl #25
   123ac:	e1500c01 	cmp	r0, r1, lsl #24
   123b0:	e0a22002 	adc	r2, r2, r2
   123b4:	20400c01 	subcs	r0, r0, r1, lsl #24
   123b8:	e1500b81 	cmp	r0, r1, lsl #23
   123bc:	e0a22002 	adc	r2, r2, r2
   123c0:	20400b81 	subcs	r0, r0, r1, lsl #23
   123c4:	e1500b01 	cmp	r0, r1, lsl #22
   123c8:	e0a22002 	adc	r2, r2, r2
   123cc:	20400b01 	subcs	r0, r0, r1, lsl #22
   123d0:	e1500a81 	cmp	r0, r1, lsl #21
   123d4:	e0a22002 	adc	r2, r2, r2
   123d8:	20400a81 	subcs	r0, r0, r1, lsl #21
   123dc:	e1500a01 	cmp	r0, r1, lsl #20
   123e0:	e0a22002 	adc	r2, r2, r2
   123e4:	20400a01 	subcs	r0, r0, r1, lsl #20
   123e8:	e1500981 	cmp	r0, r1, lsl #19
   123ec:	e0a22002 	adc	r2, r2, r2
   123f0:	20400981 	subcs	r0, r0, r1, lsl #19
   123f4:	e1500901 	cmp	r0, r1, lsl #18
   123f8:	e0a22002 	adc	r2, r2, r2
   123fc:	20400901 	subcs	r0, r0, r1, lsl #18
   12400:	e1500881 	cmp	r0, r1, lsl #17
   12404:	e0a22002 	adc	r2, r2, r2
   12408:	20400881 	subcs	r0, r0, r1, lsl #17
   1240c:	e1500801 	cmp	r0, r1, lsl #16
   12410:	e0a22002 	adc	r2, r2, r2
   12414:	20400801 	subcs	r0, r0, r1, lsl #16
   12418:	e1500781 	cmp	r0, r1, lsl #15
   1241c:	e0a22002 	adc	r2, r2, r2
   12420:	20400781 	subcs	r0, r0, r1, lsl #15
   12424:	e1500701 	cmp	r0, r1, lsl #14
   12428:	e0a22002 	adc	r2, r2, r2
   1242c:	20400701 	subcs	r0, r0, r1, lsl #14
   12430:	e1500681 	cmp	r0, r1, lsl #13
   12434:	e0a22002 	adc	r2, r2, r2
   12438:	20400681 	subcs	r0, r0, r1, lsl #13
   1243c:	e1500601 	cmp	r0, r1, lsl #12
   12440:	e0a22002 	adc	r2, r2, r2
   12444:	20400601 	subcs	r0, r0, r1, lsl #12
   12448:	e1500581 	cmp	r0, r1, lsl #11
   1244c:	e0a22002 	adc	r2, r2, r2
   12450:	20400581 	subcs	r0, r0, r1, lsl #11
   12454:	e1500501 	cmp	r0, r1, lsl #10
   12458:	e0a22002 	adc	r2, r2, r2
   1245c:	20400501 	subcs	r0, r0, r1, lsl #10
   12460:	e1500481 	cmp	r0, r1, lsl #9
   12464:	e0a22002 	adc	r2, r2, r2
   12468:	20400481 	subcs	r0, r0, r1, lsl #9
   1246c:	e1500401 	cmp	r0, r1, lsl #8
   12470:	e0a22002 	adc	r2, r2, r2
   12474:	20400401 	subcs	r0, r0, r1, lsl #8
   12478:	e1500381 	cmp	r0, r1, lsl #7
   1247c:	e0a22002 	adc	r2, r2, r2
   12480:	20400381 	subcs	r0, r0, r1, lsl #7
   12484:	e1500301 	cmp	r0, r1, lsl #6
   12488:	e0a22002 	adc	r2, r2, r2
   1248c:	20400301 	subcs	r0, r0, r1, lsl #6
   12490:	e1500281 	cmp	r0, r1, lsl #5
   12494:	e0a22002 	adc	r2, r2, r2
   12498:	20400281 	subcs	r0, r0, r1, lsl #5
   1249c:	e1500201 	cmp	r0, r1, lsl #4
   124a0:	e0a22002 	adc	r2, r2, r2
   124a4:	20400201 	subcs	r0, r0, r1, lsl #4
   124a8:	e1500181 	cmp	r0, r1, lsl #3
   124ac:	e0a22002 	adc	r2, r2, r2
   124b0:	20400181 	subcs	r0, r0, r1, lsl #3
   124b4:	e1500101 	cmp	r0, r1, lsl #2
   124b8:	e0a22002 	adc	r2, r2, r2
   124bc:	20400101 	subcs	r0, r0, r1, lsl #2
   124c0:	e1500081 	cmp	r0, r1, lsl #1
   124c4:	e0a22002 	adc	r2, r2, r2
   124c8:	20400081 	subcs	r0, r0, r1, lsl #1
   124cc:	e1500001 	cmp	r0, r1
   124d0:	e0a22002 	adc	r2, r2, r2
   124d4:	20400001 	subcs	r0, r0, r1
   124d8:	e1a00002 	mov	r0, r2
   124dc:	e12fff1e 	bx	lr
   124e0:	03a00001 	moveq	r0, #1	; 0x1
   124e4:	13a00000 	movne	r0, #0	; 0x0
   124e8:	e12fff1e 	bx	lr
   124ec:	e16f2f11 	clz	r2, r1
   124f0:	e262201f 	rsb	r2, r2, #31	; 0x1f
   124f4:	e1a00230 	lsr	r0, r0, r2
   124f8:	e12fff1e 	bx	lr
   124fc:	e52de008 	str	lr, [sp, #-8]!
   12500:	eb0002c4 	bl	13018 <__div0>
   12504:	e3a00000 	mov	r0, #0	; 0x0
   12508:	e49df008 	ldr	pc, [sp], #8

0001250c <__aeabi_uidivmod>:
   1250c:	e92d4003 	push	{r0, r1, lr}
   12510:	ebffff81 	bl	1231c <__aeabi_uidiv>
   12514:	e8bd4006 	pop	{r1, r2, lr}
   12518:	e0030092 	mul	r3, r2, r0
   1251c:	e0411003 	sub	r1, r1, r3
   12520:	e12fff1e 	bx	lr

00012524 <__aeabi_idiv>:
   12524:	e3510000 	cmp	r1, #0	; 0x0
   12528:	e020c001 	eor	ip, r0, r1
   1252c:	0a000080 	beq	12734 <__aeabi_idiv+0x210>
   12530:	42611000 	rsbmi	r1, r1, #0	; 0x0
   12534:	e2512001 	subs	r2, r1, #1	; 0x1
   12538:	0a000070 	beq	12700 <__aeabi_idiv+0x1dc>
   1253c:	e1b03000 	movs	r3, r0
   12540:	42603000 	rsbmi	r3, r0, #0	; 0x0
   12544:	e1530001 	cmp	r3, r1
   12548:	9a00006f 	bls	1270c <__aeabi_idiv+0x1e8>
   1254c:	e1110002 	tst	r1, r2
   12550:	0a000071 	beq	1271c <__aeabi_idiv+0x1f8>
   12554:	e16f2f13 	clz	r2, r3
   12558:	e16f0f11 	clz	r0, r1
   1255c:	e0402002 	sub	r2, r0, r2
   12560:	e272201f 	rsbs	r2, r2, #31	; 0x1f
   12564:	10822082 	addne	r2, r2, r2, lsl #1
   12568:	e3a00000 	mov	r0, #0	; 0x0
   1256c:	108ff102 	addne	pc, pc, r2, lsl #2
   12570:	e1a00000 	nop			(mov r0,r0)
   12574:	e1530f81 	cmp	r3, r1, lsl #31
   12578:	e0a00000 	adc	r0, r0, r0
   1257c:	20433f81 	subcs	r3, r3, r1, lsl #31
   12580:	e1530f01 	cmp	r3, r1, lsl #30
   12584:	e0a00000 	adc	r0, r0, r0
   12588:	20433f01 	subcs	r3, r3, r1, lsl #30
   1258c:	e1530e81 	cmp	r3, r1, lsl #29
   12590:	e0a00000 	adc	r0, r0, r0
   12594:	20433e81 	subcs	r3, r3, r1, lsl #29
   12598:	e1530e01 	cmp	r3, r1, lsl #28
   1259c:	e0a00000 	adc	r0, r0, r0
   125a0:	20433e01 	subcs	r3, r3, r1, lsl #28
   125a4:	e1530d81 	cmp	r3, r1, lsl #27
   125a8:	e0a00000 	adc	r0, r0, r0
   125ac:	20433d81 	subcs	r3, r3, r1, lsl #27
   125b0:	e1530d01 	cmp	r3, r1, lsl #26
   125b4:	e0a00000 	adc	r0, r0, r0
   125b8:	20433d01 	subcs	r3, r3, r1, lsl #26
   125bc:	e1530c81 	cmp	r3, r1, lsl #25
   125c0:	e0a00000 	adc	r0, r0, r0
   125c4:	20433c81 	subcs	r3, r3, r1, lsl #25
   125c8:	e1530c01 	cmp	r3, r1, lsl #24
   125cc:	e0a00000 	adc	r0, r0, r0
   125d0:	20433c01 	subcs	r3, r3, r1, lsl #24
   125d4:	e1530b81 	cmp	r3, r1, lsl #23
   125d8:	e0a00000 	adc	r0, r0, r0
   125dc:	20433b81 	subcs	r3, r3, r1, lsl #23
   125e0:	e1530b01 	cmp	r3, r1, lsl #22
   125e4:	e0a00000 	adc	r0, r0, r0
   125e8:	20433b01 	subcs	r3, r3, r1, lsl #22
   125ec:	e1530a81 	cmp	r3, r1, lsl #21
   125f0:	e0a00000 	adc	r0, r0, r0
   125f4:	20433a81 	subcs	r3, r3, r1, lsl #21
   125f8:	e1530a01 	cmp	r3, r1, lsl #20
   125fc:	e0a00000 	adc	r0, r0, r0
   12600:	20433a01 	subcs	r3, r3, r1, lsl #20
   12604:	e1530981 	cmp	r3, r1, lsl #19
   12608:	e0a00000 	adc	r0, r0, r0
   1260c:	20433981 	subcs	r3, r3, r1, lsl #19
   12610:	e1530901 	cmp	r3, r1, lsl #18
   12614:	e0a00000 	adc	r0, r0, r0
   12618:	20433901 	subcs	r3, r3, r1, lsl #18
   1261c:	e1530881 	cmp	r3, r1, lsl #17
   12620:	e0a00000 	adc	r0, r0, r0
   12624:	20433881 	subcs	r3, r3, r1, lsl #17
   12628:	e1530801 	cmp	r3, r1, lsl #16
   1262c:	e0a00000 	adc	r0, r0, r0
   12630:	20433801 	subcs	r3, r3, r1, lsl #16
   12634:	e1530781 	cmp	r3, r1, lsl #15
   12638:	e0a00000 	adc	r0, r0, r0
   1263c:	20433781 	subcs	r3, r3, r1, lsl #15
   12640:	e1530701 	cmp	r3, r1, lsl #14
   12644:	e0a00000 	adc	r0, r0, r0
   12648:	20433701 	subcs	r3, r3, r1, lsl #14
   1264c:	e1530681 	cmp	r3, r1, lsl #13
   12650:	e0a00000 	adc	r0, r0, r0
   12654:	20433681 	subcs	r3, r3, r1, lsl #13
   12658:	e1530601 	cmp	r3, r1, lsl #12
   1265c:	e0a00000 	adc	r0, r0, r0
   12660:	20433601 	subcs	r3, r3, r1, lsl #12
   12664:	e1530581 	cmp	r3, r1, lsl #11
   12668:	e0a00000 	adc	r0, r0, r0
   1266c:	20433581 	subcs	r3, r3, r1, lsl #11
   12670:	e1530501 	cmp	r3, r1, lsl #10
   12674:	e0a00000 	adc	r0, r0, r0
   12678:	20433501 	subcs	r3, r3, r1, lsl #10
   1267c:	e1530481 	cmp	r3, r1, lsl #9
   12680:	e0a00000 	adc	r0, r0, r0
   12684:	20433481 	subcs	r3, r3, r1, lsl #9
   12688:	e1530401 	cmp	r3, r1, lsl #8
   1268c:	e0a00000 	adc	r0, r0, r0
   12690:	20433401 	subcs	r3, r3, r1, lsl #8
   12694:	e1530381 	cmp	r3, r1, lsl #7
   12698:	e0a00000 	adc	r0, r0, r0
   1269c:	20433381 	subcs	r3, r3, r1, lsl #7
   126a0:	e1530301 	cmp	r3, r1, lsl #6
   126a4:	e0a00000 	adc	r0, r0, r0
   126a8:	20433301 	subcs	r3, r3, r1, lsl #6
   126ac:	e1530281 	cmp	r3, r1, lsl #5
   126b0:	e0a00000 	adc	r0, r0, r0
   126b4:	20433281 	subcs	r3, r3, r1, lsl #5
   126b8:	e1530201 	cmp	r3, r1, lsl #4
   126bc:	e0a00000 	adc	r0, r0, r0
   126c0:	20433201 	subcs	r3, r3, r1, lsl #4
   126c4:	e1530181 	cmp	r3, r1, lsl #3
   126c8:	e0a00000 	adc	r0, r0, r0
   126cc:	20433181 	subcs	r3, r3, r1, lsl #3
   126d0:	e1530101 	cmp	r3, r1, lsl #2
   126d4:	e0a00000 	adc	r0, r0, r0
   126d8:	20433101 	subcs	r3, r3, r1, lsl #2
   126dc:	e1530081 	cmp	r3, r1, lsl #1
   126e0:	e0a00000 	adc	r0, r0, r0
   126e4:	20433081 	subcs	r3, r3, r1, lsl #1
   126e8:	e1530001 	cmp	r3, r1
   126ec:	e0a00000 	adc	r0, r0, r0
   126f0:	20433001 	subcs	r3, r3, r1
   126f4:	e35c0000 	cmp	ip, #0	; 0x0
   126f8:	42600000 	rsbmi	r0, r0, #0	; 0x0
   126fc:	e12fff1e 	bx	lr
   12700:	e13c0000 	teq	ip, r0
   12704:	42600000 	rsbmi	r0, r0, #0	; 0x0
   12708:	e12fff1e 	bx	lr
   1270c:	33a00000 	movcc	r0, #0	; 0x0
   12710:	01a00fcc 	asreq	r0, ip, #31
   12714:	03800001 	orreq	r0, r0, #1	; 0x1
   12718:	e12fff1e 	bx	lr
   1271c:	e16f2f11 	clz	r2, r1
   12720:	e262201f 	rsb	r2, r2, #31	; 0x1f
   12724:	e35c0000 	cmp	ip, #0	; 0x0
   12728:	e1a00233 	lsr	r0, r3, r2
   1272c:	42600000 	rsbmi	r0, r0, #0	; 0x0
   12730:	e12fff1e 	bx	lr
   12734:	e52de008 	str	lr, [sp, #-8]!
   12738:	eb000236 	bl	13018 <__div0>
   1273c:	e3a00000 	mov	r0, #0	; 0x0
   12740:	e49df008 	ldr	pc, [sp], #8

00012744 <__aeabi_idivmod>:
   12744:	e92d4003 	push	{r0, r1, lr}
   12748:	ebffff75 	bl	12524 <__aeabi_idiv>
   1274c:	e8bd4006 	pop	{r1, r2, lr}
   12750:	e0030092 	mul	r3, r2, r0
   12754:	e0411003 	sub	r1, r1, r3
   12758:	e12fff1e 	bx	lr

0001275c <__aeabi_llsr>:
   1275c:	e2523020 	subs	r3, r2, #32	; 0x20
   12760:	e262c020 	rsb	ip, r2, #32	; 0x20
   12764:	41a00230 	lsrmi	r0, r0, r2
   12768:	51a00331 	lsrpl	r0, r1, r3
   1276c:	41800c11 	orrmi	r0, r0, r1, lsl ip
   12770:	e1a01231 	lsr	r1, r1, r2
   12774:	e12fff1e 	bx	lr

00012778 <__aeabi_llsl>:
   12778:	e2523020 	subs	r3, r2, #32	; 0x20
   1277c:	e262c020 	rsb	ip, r2, #32	; 0x20
   12780:	41a01211 	lslmi	r1, r1, r2
   12784:	51a01310 	lslpl	r1, r0, r3
   12788:	41811c30 	orrmi	r1, r1, r0, lsr ip
   1278c:	e1a00210 	lsl	r0, r0, r2
   12790:	e12fff1e 	bx	lr

00012794 <__aeabi_drsub>:
   12794:	e2211102 	eor	r1, r1, #-2147483648	; 0x80000000
   12798:	ea000000 	b	127a0 <__adddf3>

0001279c <__aeabi_dsub>:
   1279c:	e2233102 	eor	r3, r3, #-2147483648	; 0x80000000

000127a0 <__adddf3>:
   127a0:	e92d4030 	push	{r4, r5, lr}
   127a4:	e1a04081 	lsl	r4, r1, #1
   127a8:	e1a05083 	lsl	r5, r3, #1
   127ac:	e1340005 	teq	r4, r5
   127b0:	01300002 	teqeq	r0, r2
   127b4:	1194c000 	orrsne	ip, r4, r0
   127b8:	1195c002 	orrsne	ip, r5, r2
   127bc:	11f0cac4 	mvnsne	ip, r4, asr #21
   127c0:	11f0cac5 	mvnsne	ip, r5, asr #21
   127c4:	0a000079 	beq	129b0 <__adddf3+0x210>
   127c8:	e1a04aa4 	lsr	r4, r4, #21
   127cc:	e0745aa5 	rsbs	r5, r4, r5, lsr #21
   127d0:	b2655000 	rsblt	r5, r5, #0	; 0x0
   127d4:	da000006 	ble	127f4 <__adddf3+0x54>
   127d8:	e0844005 	add	r4, r4, r5
   127dc:	e0202002 	eor	r2, r0, r2
   127e0:	e0213003 	eor	r3, r1, r3
   127e4:	e0220000 	eor	r0, r2, r0
   127e8:	e0231001 	eor	r1, r3, r1
   127ec:	e0202002 	eor	r2, r0, r2
   127f0:	e0213003 	eor	r3, r1, r3
   127f4:	e3550036 	cmp	r5, #54	; 0x36
   127f8:	88bd8030 	pophi	{r4, r5, pc}
   127fc:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   12800:	e1a01601 	lsl	r1, r1, #12
   12804:	e3a0c601 	mov	ip, #1048576	; 0x100000
   12808:	e18c1621 	orr	r1, ip, r1, lsr #12
   1280c:	0a000001 	beq	12818 <__adddf3+0x78>
   12810:	e2700000 	rsbs	r0, r0, #0	; 0x0
   12814:	e2e11000 	rsc	r1, r1, #0	; 0x0
   12818:	e3130102 	tst	r3, #-2147483648	; 0x80000000
   1281c:	e1a03603 	lsl	r3, r3, #12
   12820:	e18c3623 	orr	r3, ip, r3, lsr #12
   12824:	0a000001 	beq	12830 <__adddf3+0x90>
   12828:	e2722000 	rsbs	r2, r2, #0	; 0x0
   1282c:	e2e33000 	rsc	r3, r3, #0	; 0x0
   12830:	e1340005 	teq	r4, r5
   12834:	0a000057 	beq	12998 <__adddf3+0x1f8>
   12838:	e2444001 	sub	r4, r4, #1	; 0x1
   1283c:	e275e020 	rsbs	lr, r5, #32	; 0x20
   12840:	ba000005 	blt	1285c <__adddf3+0xbc>
   12844:	e1a0ce12 	lsl	ip, r2, lr
   12848:	e0900532 	adds	r0, r0, r2, lsr r5
   1284c:	e2a11000 	adc	r1, r1, #0	; 0x0
   12850:	e0900e13 	adds	r0, r0, r3, lsl lr
   12854:	e0b11553 	adcs	r1, r1, r3, asr r5
   12858:	ea000006 	b	12878 <__adddf3+0xd8>
   1285c:	e2455020 	sub	r5, r5, #32	; 0x20
   12860:	e28ee020 	add	lr, lr, #32	; 0x20
   12864:	e3520001 	cmp	r2, #1	; 0x1
   12868:	e1a0ce13 	lsl	ip, r3, lr
   1286c:	238cc002 	orrcs	ip, ip, #2	; 0x2
   12870:	e0900553 	adds	r0, r0, r3, asr r5
   12874:	e0b11fc3 	adcs	r1, r1, r3, asr #31
   12878:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   1287c:	5a000002 	bpl	1288c <__adddf3+0xec>
   12880:	e27cc000 	rsbs	ip, ip, #0	; 0x0
   12884:	e2f00000 	rscs	r0, r0, #0	; 0x0
   12888:	e2e11000 	rsc	r1, r1, #0	; 0x0
   1288c:	e3510601 	cmp	r1, #1048576	; 0x100000
   12890:	3a00000e 	bcc	128d0 <__adddf3+0x130>
   12894:	e3510602 	cmp	r1, #2097152	; 0x200000
   12898:	3a000006 	bcc	128b8 <__adddf3+0x118>
   1289c:	e1b010a1 	lsrs	r1, r1, #1
   128a0:	e1b00060 	rrxs	r0, r0
   128a4:	e1a0c06c 	rrx	ip, ip
   128a8:	e2844001 	add	r4, r4, #1	; 0x1
   128ac:	e1a02a84 	lsl	r2, r4, #21
   128b0:	e3720501 	cmn	r2, #4194304	; 0x400000
   128b4:	2a000055 	bcs	12a10 <__adddf3+0x270>
   128b8:	e35c0102 	cmp	ip, #-2147483648	; 0x80000000
   128bc:	01b0c0a0 	lsrseq	ip, r0, #1
   128c0:	e2b00000 	adcs	r0, r0, #0	; 0x0
   128c4:	e0a11a04 	adc	r1, r1, r4, lsl #20
   128c8:	e1811005 	orr	r1, r1, r5
   128cc:	e8bd8030 	pop	{r4, r5, pc}
   128d0:	e1b0c08c 	lsls	ip, ip, #1
   128d4:	e0b00000 	adcs	r0, r0, r0
   128d8:	e0a11001 	adc	r1, r1, r1
   128dc:	e3110601 	tst	r1, #1048576	; 0x100000
   128e0:	e2444001 	sub	r4, r4, #1	; 0x1
   128e4:	1afffff3 	bne	128b8 <__adddf3+0x118>
   128e8:	e3310000 	teq	r1, #0	; 0x0
   128ec:	01a01000 	moveq	r1, r0
   128f0:	03a00000 	moveq	r0, #0	; 0x0
   128f4:	e16f3f11 	clz	r3, r1
   128f8:	02833020 	addeq	r3, r3, #32	; 0x20
   128fc:	e243300b 	sub	r3, r3, #11	; 0xb
   12900:	e2532020 	subs	r2, r3, #32	; 0x20
   12904:	aa000007 	bge	12928 <__adddf3+0x188>
   12908:	e292200c 	adds	r2, r2, #12	; 0xc
   1290c:	da000004 	ble	12924 <__adddf3+0x184>
   12910:	e282c014 	add	ip, r2, #20	; 0x14
   12914:	e262200c 	rsb	r2, r2, #12	; 0xc
   12918:	e1a00c11 	lsl	r0, r1, ip
   1291c:	e1a01231 	lsr	r1, r1, r2
   12920:	ea000004 	b	12938 <__adddf3+0x198>
   12924:	e2822014 	add	r2, r2, #20	; 0x14
   12928:	d262c020 	rsble	ip, r2, #32	; 0x20
   1292c:	e1a01211 	lsl	r1, r1, r2
   12930:	d1811c30 	orrle	r1, r1, r0, lsr ip
   12934:	d1a00210 	lslle	r0, r0, r2
   12938:	e0544003 	subs	r4, r4, r3
   1293c:	a0811a04 	addge	r1, r1, r4, lsl #20
   12940:	a1811005 	orrge	r1, r1, r5
   12944:	a8bd8030 	popge	{r4, r5, pc}
   12948:	e1e04004 	mvn	r4, r4
   1294c:	e254401f 	subs	r4, r4, #31	; 0x1f
   12950:	aa00000d 	bge	1298c <__adddf3+0x1ec>
   12954:	e294400c 	adds	r4, r4, #12	; 0xc
   12958:	ca000005 	bgt	12974 <__adddf3+0x1d4>
   1295c:	e2844014 	add	r4, r4, #20	; 0x14
   12960:	e2642020 	rsb	r2, r4, #32	; 0x20
   12964:	e1a00430 	lsr	r0, r0, r4
   12968:	e1800211 	orr	r0, r0, r1, lsl r2
   1296c:	e1851431 	orr	r1, r5, r1, lsr r4
   12970:	e8bd8030 	pop	{r4, r5, pc}
   12974:	e264400c 	rsb	r4, r4, #12	; 0xc
   12978:	e2642020 	rsb	r2, r4, #32	; 0x20
   1297c:	e1a00230 	lsr	r0, r0, r2
   12980:	e1800411 	orr	r0, r0, r1, lsl r4
   12984:	e1a01005 	mov	r1, r5
   12988:	e8bd8030 	pop	{r4, r5, pc}
   1298c:	e1a00431 	lsr	r0, r1, r4
   12990:	e1a01005 	mov	r1, r5
   12994:	e8bd8030 	pop	{r4, r5, pc}
   12998:	e3340000 	teq	r4, #0	; 0x0
   1299c:	e2233601 	eor	r3, r3, #1048576	; 0x100000
   129a0:	02211601 	eoreq	r1, r1, #1048576	; 0x100000
   129a4:	02844001 	addeq	r4, r4, #1	; 0x1
   129a8:	12455001 	subne	r5, r5, #1	; 0x1
   129ac:	eaffffa1 	b	12838 <__adddf3+0x98>
   129b0:	e1f0cac4 	mvns	ip, r4, asr #21
   129b4:	11f0cac5 	mvnsne	ip, r5, asr #21
   129b8:	0a000018 	beq	12a20 <__adddf3+0x280>
   129bc:	e1340005 	teq	r4, r5
   129c0:	01300002 	teqeq	r0, r2
   129c4:	0a000003 	beq	129d8 <__adddf3+0x238>
   129c8:	e194c000 	orrs	ip, r4, r0
   129cc:	01a01003 	moveq	r1, r3
   129d0:	01a00002 	moveq	r0, r2
   129d4:	e8bd8030 	pop	{r4, r5, pc}
   129d8:	e1310003 	teq	r1, r3
   129dc:	13a01000 	movne	r1, #0	; 0x0
   129e0:	13a00000 	movne	r0, #0	; 0x0
   129e4:	18bd8030 	popne	{r4, r5, pc}
   129e8:	e1b0caa4 	lsrs	ip, r4, #21
   129ec:	1a000003 	bne	12a00 <__adddf3+0x260>
   129f0:	e1b00080 	lsls	r0, r0, #1
   129f4:	e0b11001 	adcs	r1, r1, r1
   129f8:	23811102 	orrcs	r1, r1, #-2147483648	; 0x80000000
   129fc:	e8bd8030 	pop	{r4, r5, pc}
   12a00:	e2944501 	adds	r4, r4, #4194304	; 0x400000
   12a04:	32811601 	addcc	r1, r1, #1048576	; 0x100000
   12a08:	38bd8030 	popcc	{r4, r5, pc}
   12a0c:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   12a10:	e385147f 	orr	r1, r5, #2130706432	; 0x7f000000
   12a14:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   12a18:	e3a00000 	mov	r0, #0	; 0x0
   12a1c:	e8bd8030 	pop	{r4, r5, pc}
   12a20:	e1f0cac4 	mvns	ip, r4, asr #21
   12a24:	11a01003 	movne	r1, r3
   12a28:	11a00002 	movne	r0, r2
   12a2c:	01f0cac5 	mvnseq	ip, r5, asr #21
   12a30:	11a03001 	movne	r3, r1
   12a34:	11a02000 	movne	r2, r0
   12a38:	e1904601 	orrs	r4, r0, r1, lsl #12
   12a3c:	01925603 	orrseq	r5, r2, r3, lsl #12
   12a40:	01310003 	teqeq	r1, r3
   12a44:	13811702 	orrne	r1, r1, #524288	; 0x80000
   12a48:	e8bd8030 	pop	{r4, r5, pc}

00012a4c <__aeabi_ui2d>:
   12a4c:	e3300000 	teq	r0, #0	; 0x0
   12a50:	03a01000 	moveq	r1, #0	; 0x0
   12a54:	012fff1e 	bxeq	lr
   12a58:	e92d4030 	push	{r4, r5, lr}
   12a5c:	e3a04b01 	mov	r4, #1024	; 0x400
   12a60:	e2844032 	add	r4, r4, #50	; 0x32
   12a64:	e3a05000 	mov	r5, #0	; 0x0
   12a68:	e3a01000 	mov	r1, #0	; 0x0
   12a6c:	eaffff9d 	b	128e8 <__adddf3+0x148>

00012a70 <__aeabi_i2d>:
   12a70:	e3300000 	teq	r0, #0	; 0x0
   12a74:	03a01000 	moveq	r1, #0	; 0x0
   12a78:	012fff1e 	bxeq	lr
   12a7c:	e92d4030 	push	{r4, r5, lr}
   12a80:	e3a04b01 	mov	r4, #1024	; 0x400
   12a84:	e2844032 	add	r4, r4, #50	; 0x32
   12a88:	e2105102 	ands	r5, r0, #-2147483648	; 0x80000000
   12a8c:	42600000 	rsbmi	r0, r0, #0	; 0x0
   12a90:	e3a01000 	mov	r1, #0	; 0x0
   12a94:	eaffff93 	b	128e8 <__adddf3+0x148>

00012a98 <__aeabi_f2d>:
   12a98:	e1b02080 	lsls	r2, r0, #1
   12a9c:	e1a011c2 	asr	r1, r2, #3
   12aa0:	e1a01061 	rrx	r1, r1
   12aa4:	e1a00e02 	lsl	r0, r2, #28
   12aa8:	121234ff 	andsne	r3, r2, #-16777216	; 0xff000000
   12aac:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   12ab0:	1221130e 	eorne	r1, r1, #939524096	; 0x38000000
   12ab4:	112fff1e 	bxne	lr
   12ab8:	e3320000 	teq	r2, #0	; 0x0
   12abc:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   12ac0:	012fff1e 	bxeq	lr
   12ac4:	e92d4030 	push	{r4, r5, lr}
   12ac8:	e3a04d0e 	mov	r4, #896	; 0x380
   12acc:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   12ad0:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   12ad4:	eaffff83 	b	128e8 <__adddf3+0x148>

00012ad8 <__aeabi_ul2d>:
   12ad8:	e1902001 	orrs	r2, r0, r1
   12adc:	012fff1e 	bxeq	lr
   12ae0:	e92d4030 	push	{r4, r5, lr}
   12ae4:	e3a05000 	mov	r5, #0	; 0x0
   12ae8:	ea000006 	b	12b08 <__aeabi_l2d+0x1c>

00012aec <__aeabi_l2d>:
   12aec:	e1902001 	orrs	r2, r0, r1
   12af0:	012fff1e 	bxeq	lr
   12af4:	e92d4030 	push	{r4, r5, lr}
   12af8:	e2115102 	ands	r5, r1, #-2147483648	; 0x80000000
   12afc:	5a000001 	bpl	12b08 <__aeabi_l2d+0x1c>
   12b00:	e2700000 	rsbs	r0, r0, #0	; 0x0
   12b04:	e2e11000 	rsc	r1, r1, #0	; 0x0
   12b08:	e3a04b01 	mov	r4, #1024	; 0x400
   12b0c:	e2844032 	add	r4, r4, #50	; 0x32
   12b10:	e1b0cb21 	lsrs	ip, r1, #22
   12b14:	0affff5c 	beq	1288c <__adddf3+0xec>
   12b18:	e3a02003 	mov	r2, #3	; 0x3
   12b1c:	e1b0c1ac 	lsrs	ip, ip, #3
   12b20:	12822003 	addne	r2, r2, #3	; 0x3
   12b24:	e1b0c1ac 	lsrs	ip, ip, #3
   12b28:	12822003 	addne	r2, r2, #3	; 0x3
   12b2c:	e08221ac 	add	r2, r2, ip, lsr #3
   12b30:	e2623020 	rsb	r3, r2, #32	; 0x20
   12b34:	e1a0c310 	lsl	ip, r0, r3
   12b38:	e1a00230 	lsr	r0, r0, r2
   12b3c:	e1800311 	orr	r0, r0, r1, lsl r3
   12b40:	e1a01231 	lsr	r1, r1, r2
   12b44:	e0844002 	add	r4, r4, r2
   12b48:	eaffff4f 	b	1288c <__adddf3+0xec>

00012b4c <__aeabi_dmul>:
   12b4c:	e92d4070 	push	{r4, r5, r6, lr}
   12b50:	e3a0c0ff 	mov	ip, #255	; 0xff
   12b54:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   12b58:	e01c4a21 	ands	r4, ip, r1, lsr #20
   12b5c:	101c5a23 	andsne	r5, ip, r3, lsr #20
   12b60:	1134000c 	teqne	r4, ip
   12b64:	1135000c 	teqne	r5, ip
   12b68:	0b00006f 	bleq	12d2c <__aeabi_dmul+0x1e0>
   12b6c:	e0844005 	add	r4, r4, r5
   12b70:	e0216003 	eor	r6, r1, r3
   12b74:	e1c11a8c 	bic	r1, r1, ip, lsl #21
   12b78:	e1c33a8c 	bic	r3, r3, ip, lsl #21
   12b7c:	e1905601 	orrs	r5, r0, r1, lsl #12
   12b80:	11925603 	orrsne	r5, r2, r3, lsl #12
   12b84:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   12b88:	e3833601 	orr	r3, r3, #1048576	; 0x100000
   12b8c:	0a00001c 	beq	12c04 <__aeabi_dmul+0xb8>
   12b90:	e08ec290 	umull	ip, lr, r0, r2
   12b94:	e3a05000 	mov	r5, #0	; 0x0
   12b98:	e0a5e291 	umlal	lr, r5, r1, r2
   12b9c:	e2062102 	and	r2, r6, #-2147483648	; 0x80000000
   12ba0:	e0a5e390 	umlal	lr, r5, r0, r3
   12ba4:	e3a06000 	mov	r6, #0	; 0x0
   12ba8:	e0a65391 	umlal	r5, r6, r1, r3
   12bac:	e33c0000 	teq	ip, #0	; 0x0
   12bb0:	138ee001 	orrne	lr, lr, #1	; 0x1
   12bb4:	e24440ff 	sub	r4, r4, #255	; 0xff
   12bb8:	e3560c02 	cmp	r6, #512	; 0x200
   12bbc:	e2c44c03 	sbc	r4, r4, #768	; 0x300
   12bc0:	2a000002 	bcs	12bd0 <__aeabi_dmul+0x84>
   12bc4:	e1b0e08e 	lsls	lr, lr, #1
   12bc8:	e0b55005 	adcs	r5, r5, r5
   12bcc:	e0a66006 	adc	r6, r6, r6
   12bd0:	e1821586 	orr	r1, r2, r6, lsl #11
   12bd4:	e1811aa5 	orr	r1, r1, r5, lsr #21
   12bd8:	e1a00585 	lsl	r0, r5, #11
   12bdc:	e1800aae 	orr	r0, r0, lr, lsr #21
   12be0:	e1a0e58e 	lsl	lr, lr, #11
   12be4:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   12be8:	835c0c07 	cmphi	ip, #1792	; 0x700
   12bec:	8a00000f 	bhi	12c30 <__aeabi_dmul+0xe4>
   12bf0:	e35e0102 	cmp	lr, #-2147483648	; 0x80000000
   12bf4:	01b0e0a0 	lsrseq	lr, r0, #1
   12bf8:	e2b00000 	adcs	r0, r0, #0	; 0x0
   12bfc:	e0a11a04 	adc	r1, r1, r4, lsl #20
   12c00:	e8bd8070 	pop	{r4, r5, r6, pc}
   12c04:	e2066102 	and	r6, r6, #-2147483648	; 0x80000000
   12c08:	e1861001 	orr	r1, r6, r1
   12c0c:	e1800002 	orr	r0, r0, r2
   12c10:	e0211003 	eor	r1, r1, r3
   12c14:	e05440ac 	subs	r4, r4, ip, lsr #1
   12c18:	c074500c 	rsbsgt	r5, r4, ip
   12c1c:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   12c20:	c8bd8070 	popgt	{r4, r5, r6, pc}
   12c24:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   12c28:	e3a0e000 	mov	lr, #0	; 0x0
   12c2c:	e2544001 	subs	r4, r4, #1	; 0x1
   12c30:	ca000058 	bgt	12d98 <__aeabi_dmul+0x24c>
   12c34:	e3740036 	cmn	r4, #54	; 0x36
   12c38:	d3a00000 	movle	r0, #0	; 0x0
   12c3c:	d2011102 	andle	r1, r1, #-2147483648	; 0x80000000
   12c40:	d8bd8070 	pople	{r4, r5, r6, pc}
   12c44:	e2644000 	rsb	r4, r4, #0	; 0x0
   12c48:	e2544020 	subs	r4, r4, #32	; 0x20
   12c4c:	aa000018 	bge	12cb4 <__aeabi_dmul+0x168>
   12c50:	e294400c 	adds	r4, r4, #12	; 0xc
   12c54:	ca00000b 	bgt	12c88 <__aeabi_dmul+0x13c>
   12c58:	e2844014 	add	r4, r4, #20	; 0x14
   12c5c:	e2645020 	rsb	r5, r4, #32	; 0x20
   12c60:	e1a03510 	lsl	r3, r0, r5
   12c64:	e1a00430 	lsr	r0, r0, r4
   12c68:	e1800511 	orr	r0, r0, r1, lsl r5
   12c6c:	e2012102 	and	r2, r1, #-2147483648	; 0x80000000
   12c70:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   12c74:	e0900fa3 	adds	r0, r0, r3, lsr #31
   12c78:	e0a21431 	adc	r1, r2, r1, lsr r4
   12c7c:	e19ee083 	orrs	lr, lr, r3, lsl #1
   12c80:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   12c84:	e8bd8070 	pop	{r4, r5, r6, pc}
   12c88:	e264400c 	rsb	r4, r4, #12	; 0xc
   12c8c:	e2645020 	rsb	r5, r4, #32	; 0x20
   12c90:	e1a03410 	lsl	r3, r0, r4
   12c94:	e1a00530 	lsr	r0, r0, r5
   12c98:	e1800411 	orr	r0, r0, r1, lsl r4
   12c9c:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   12ca0:	e0900fa3 	adds	r0, r0, r3, lsr #31
   12ca4:	e2a11000 	adc	r1, r1, #0	; 0x0
   12ca8:	e19ee083 	orrs	lr, lr, r3, lsl #1
   12cac:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   12cb0:	e8bd8070 	pop	{r4, r5, r6, pc}
   12cb4:	e2645020 	rsb	r5, r4, #32	; 0x20
   12cb8:	e18ee510 	orr	lr, lr, r0, lsl r5
   12cbc:	e1a03430 	lsr	r3, r0, r4
   12cc0:	e1833511 	orr	r3, r3, r1, lsl r5
   12cc4:	e1a00431 	lsr	r0, r1, r4
   12cc8:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   12ccc:	e1c00431 	bic	r0, r0, r1, lsr r4
   12cd0:	e0800fa3 	add	r0, r0, r3, lsr #31
   12cd4:	e19ee083 	orrs	lr, lr, r3, lsl #1
   12cd8:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   12cdc:	e8bd8070 	pop	{r4, r5, r6, pc}
   12ce0:	e3340000 	teq	r4, #0	; 0x0
   12ce4:	1a000008 	bne	12d0c <__aeabi_dmul+0x1c0>
   12ce8:	e2016102 	and	r6, r1, #-2147483648	; 0x80000000
   12cec:	e1b00080 	lsls	r0, r0, #1
   12cf0:	e0a11001 	adc	r1, r1, r1
   12cf4:	e3110601 	tst	r1, #1048576	; 0x100000
   12cf8:	02444001 	subeq	r4, r4, #1	; 0x1
   12cfc:	0afffffa 	beq	12cec <__aeabi_dmul+0x1a0>
   12d00:	e1811006 	orr	r1, r1, r6
   12d04:	e3350000 	teq	r5, #0	; 0x0
   12d08:	11a0f00e 	movne	pc, lr
   12d0c:	e2036102 	and	r6, r3, #-2147483648	; 0x80000000
   12d10:	e1b02082 	lsls	r2, r2, #1
   12d14:	e0a33003 	adc	r3, r3, r3
   12d18:	e3130601 	tst	r3, #1048576	; 0x100000
   12d1c:	02455001 	subeq	r5, r5, #1	; 0x1
   12d20:	0afffffa 	beq	12d10 <__aeabi_dmul+0x1c4>
   12d24:	e1833006 	orr	r3, r3, r6
   12d28:	e1a0f00e 	mov	pc, lr
   12d2c:	e134000c 	teq	r4, ip
   12d30:	e00c5a23 	and	r5, ip, r3, lsr #20
   12d34:	1135000c 	teqne	r5, ip
   12d38:	0a000006 	beq	12d58 <__aeabi_dmul+0x20c>
   12d3c:	e1906081 	orrs	r6, r0, r1, lsl #1
   12d40:	11926083 	orrsne	r6, r2, r3, lsl #1
   12d44:	1affffe5 	bne	12ce0 <__aeabi_dmul+0x194>
   12d48:	e0211003 	eor	r1, r1, r3
   12d4c:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   12d50:	e3a00000 	mov	r0, #0	; 0x0
   12d54:	e8bd8070 	pop	{r4, r5, r6, pc}
   12d58:	e1906081 	orrs	r6, r0, r1, lsl #1
   12d5c:	01a00002 	moveq	r0, r2
   12d60:	01a01003 	moveq	r1, r3
   12d64:	11926083 	orrsne	r6, r2, r3, lsl #1
   12d68:	0a00000f 	beq	12dac <__aeabi_dmul+0x260>
   12d6c:	e134000c 	teq	r4, ip
   12d70:	1a000001 	bne	12d7c <__aeabi_dmul+0x230>
   12d74:	e1906601 	orrs	r6, r0, r1, lsl #12
   12d78:	1a00000b 	bne	12dac <__aeabi_dmul+0x260>
   12d7c:	e135000c 	teq	r5, ip
   12d80:	1a000003 	bne	12d94 <__aeabi_dmul+0x248>
   12d84:	e1926603 	orrs	r6, r2, r3, lsl #12
   12d88:	11a00002 	movne	r0, r2
   12d8c:	11a01003 	movne	r1, r3
   12d90:	1a000005 	bne	12dac <__aeabi_dmul+0x260>
   12d94:	e0211003 	eor	r1, r1, r3
   12d98:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   12d9c:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   12da0:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   12da4:	e3a00000 	mov	r0, #0	; 0x0
   12da8:	e8bd8070 	pop	{r4, r5, r6, pc}
   12dac:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   12db0:	e381173e 	orr	r1, r1, #16252928	; 0xf80000
   12db4:	e8bd8070 	pop	{r4, r5, r6, pc}

00012db8 <__aeabi_ddiv>:
   12db8:	e92d4070 	push	{r4, r5, r6, lr}
   12dbc:	e3a0c0ff 	mov	ip, #255	; 0xff
   12dc0:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   12dc4:	e01c4a21 	ands	r4, ip, r1, lsr #20
   12dc8:	101c5a23 	andsne	r5, ip, r3, lsr #20
   12dcc:	1134000c 	teqne	r4, ip
   12dd0:	1135000c 	teqne	r5, ip
   12dd4:	0b00005c 	bleq	12f4c <__aeabi_ddiv+0x194>
   12dd8:	e0444005 	sub	r4, r4, r5
   12ddc:	e021e003 	eor	lr, r1, r3
   12de0:	e1925603 	orrs	r5, r2, r3, lsl #12
   12de4:	e1a01601 	lsl	r1, r1, #12
   12de8:	0a00004b 	beq	12f1c <__aeabi_ddiv+0x164>
   12dec:	e1a03603 	lsl	r3, r3, #12
   12df0:	e3a05201 	mov	r5, #268435456	; 0x10000000
   12df4:	e1853223 	orr	r3, r5, r3, lsr #4
   12df8:	e1833c22 	orr	r3, r3, r2, lsr #24
   12dfc:	e1a02402 	lsl	r2, r2, #8
   12e00:	e1855221 	orr	r5, r5, r1, lsr #4
   12e04:	e1855c20 	orr	r5, r5, r0, lsr #24
   12e08:	e1a06400 	lsl	r6, r0, #8
   12e0c:	e20e1102 	and	r1, lr, #-2147483648	; 0x80000000
   12e10:	e1550003 	cmp	r5, r3
   12e14:	01560002 	cmpeq	r6, r2
   12e18:	e2a440fd 	adc	r4, r4, #253	; 0xfd
   12e1c:	e2844c03 	add	r4, r4, #768	; 0x300
   12e20:	2a000001 	bcs	12e2c <__aeabi_ddiv+0x74>
   12e24:	e1b030a3 	lsrs	r3, r3, #1
   12e28:	e1a02062 	rrx	r2, r2
   12e2c:	e0566002 	subs	r6, r6, r2
   12e30:	e0c55003 	sbc	r5, r5, r3
   12e34:	e1b030a3 	lsrs	r3, r3, #1
   12e38:	e1a02062 	rrx	r2, r2
   12e3c:	e3a00601 	mov	r0, #1048576	; 0x100000
   12e40:	e3a0c702 	mov	ip, #524288	; 0x80000
   12e44:	e056e002 	subs	lr, r6, r2
   12e48:	e0d5e003 	sbcs	lr, r5, r3
   12e4c:	20466002 	subcs	r6, r6, r2
   12e50:	21a0500e 	movcs	r5, lr
   12e54:	2180000c 	orrcs	r0, r0, ip
   12e58:	e1b030a3 	lsrs	r3, r3, #1
   12e5c:	e1a02062 	rrx	r2, r2
   12e60:	e056e002 	subs	lr, r6, r2
   12e64:	e0d5e003 	sbcs	lr, r5, r3
   12e68:	20466002 	subcs	r6, r6, r2
   12e6c:	21a0500e 	movcs	r5, lr
   12e70:	218000ac 	orrcs	r0, r0, ip, lsr #1
   12e74:	e1b030a3 	lsrs	r3, r3, #1
   12e78:	e1a02062 	rrx	r2, r2
   12e7c:	e056e002 	subs	lr, r6, r2
   12e80:	e0d5e003 	sbcs	lr, r5, r3
   12e84:	20466002 	subcs	r6, r6, r2
   12e88:	21a0500e 	movcs	r5, lr
   12e8c:	2180012c 	orrcs	r0, r0, ip, lsr #2
   12e90:	e1b030a3 	lsrs	r3, r3, #1
   12e94:	e1a02062 	rrx	r2, r2
   12e98:	e056e002 	subs	lr, r6, r2
   12e9c:	e0d5e003 	sbcs	lr, r5, r3
   12ea0:	20466002 	subcs	r6, r6, r2
   12ea4:	21a0500e 	movcs	r5, lr
   12ea8:	218001ac 	orrcs	r0, r0, ip, lsr #3
   12eac:	e195e006 	orrs	lr, r5, r6
   12eb0:	0a00000d 	beq	12eec <__aeabi_ddiv+0x134>
   12eb4:	e1a05205 	lsl	r5, r5, #4
   12eb8:	e1855e26 	orr	r5, r5, r6, lsr #28
   12ebc:	e1a06206 	lsl	r6, r6, #4
   12ec0:	e1a03183 	lsl	r3, r3, #3
   12ec4:	e1833ea2 	orr	r3, r3, r2, lsr #29
   12ec8:	e1a02182 	lsl	r2, r2, #3
   12ecc:	e1b0c22c 	lsrs	ip, ip, #4
   12ed0:	1affffdb 	bne	12e44 <__aeabi_ddiv+0x8c>
   12ed4:	e3110601 	tst	r1, #1048576	; 0x100000
   12ed8:	1a000006 	bne	12ef8 <__aeabi_ddiv+0x140>
   12edc:	e1811000 	orr	r1, r1, r0
   12ee0:	e3a00000 	mov	r0, #0	; 0x0
   12ee4:	e3a0c102 	mov	ip, #-2147483648	; 0x80000000
   12ee8:	eaffffd5 	b	12e44 <__aeabi_ddiv+0x8c>
   12eec:	e3110601 	tst	r1, #1048576	; 0x100000
   12ef0:	01811000 	orreq	r1, r1, r0
   12ef4:	03a00000 	moveq	r0, #0	; 0x0
   12ef8:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   12efc:	835c0c07 	cmphi	ip, #1792	; 0x700
   12f00:	8affff4a 	bhi	12c30 <__aeabi_dmul+0xe4>
   12f04:	e055c003 	subs	ip, r5, r3
   12f08:	0056c002 	subseq	ip, r6, r2
   12f0c:	01b0c0a0 	lsrseq	ip, r0, #1
   12f10:	e2b00000 	adcs	r0, r0, #0	; 0x0
   12f14:	e0a11a04 	adc	r1, r1, r4, lsl #20
   12f18:	e8bd8070 	pop	{r4, r5, r6, pc}
   12f1c:	e20ee102 	and	lr, lr, #-2147483648	; 0x80000000
   12f20:	e18e1621 	orr	r1, lr, r1, lsr #12
   12f24:	e09440ac 	adds	r4, r4, ip, lsr #1
   12f28:	c074500c 	rsbsgt	r5, r4, ip
   12f2c:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   12f30:	c8bd8070 	popgt	{r4, r5, r6, pc}
   12f34:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   12f38:	e3a0e000 	mov	lr, #0	; 0x0
   12f3c:	e2544001 	subs	r4, r4, #1	; 0x1
   12f40:	eaffff3a 	b	12c30 <__aeabi_dmul+0xe4>
   12f44:	e185e006 	orr	lr, r5, r6
   12f48:	eaffff38 	b	12c30 <__aeabi_dmul+0xe4>
   12f4c:	e00c5a23 	and	r5, ip, r3, lsr #20
   12f50:	e134000c 	teq	r4, ip
   12f54:	0135000c 	teqeq	r5, ip
   12f58:	0affff93 	beq	12dac <__aeabi_dmul+0x260>
   12f5c:	e134000c 	teq	r4, ip
   12f60:	1a000006 	bne	12f80 <__aeabi_ddiv+0x1c8>
   12f64:	e1904601 	orrs	r4, r0, r1, lsl #12
   12f68:	1affff8f 	bne	12dac <__aeabi_dmul+0x260>
   12f6c:	e135000c 	teq	r5, ip
   12f70:	1affff87 	bne	12d94 <__aeabi_dmul+0x248>
   12f74:	e1a00002 	mov	r0, r2
   12f78:	e1a01003 	mov	r1, r3
   12f7c:	eaffff8a 	b	12dac <__aeabi_dmul+0x260>
   12f80:	e135000c 	teq	r5, ip
   12f84:	1a000004 	bne	12f9c <__aeabi_ddiv+0x1e4>
   12f88:	e1925603 	orrs	r5, r2, r3, lsl #12
   12f8c:	0affff6d 	beq	12d48 <__aeabi_dmul+0x1fc>
   12f90:	e1a00002 	mov	r0, r2
   12f94:	e1a01003 	mov	r1, r3
   12f98:	eaffff83 	b	12dac <__aeabi_dmul+0x260>
   12f9c:	e1906081 	orrs	r6, r0, r1, lsl #1
   12fa0:	11926083 	orrsne	r6, r2, r3, lsl #1
   12fa4:	1affff4d 	bne	12ce0 <__aeabi_dmul+0x194>
   12fa8:	e1904081 	orrs	r4, r0, r1, lsl #1
   12fac:	1affff78 	bne	12d94 <__aeabi_dmul+0x248>
   12fb0:	e1925083 	orrs	r5, r2, r3, lsl #1
   12fb4:	1affff63 	bne	12d48 <__aeabi_dmul+0x1fc>
   12fb8:	eaffff7b 	b	12dac <__aeabi_dmul+0x260>

00012fbc <__aeabi_d2iz>:
   12fbc:	e1a02081 	lsl	r2, r1, #1
   12fc0:	e2922602 	adds	r2, r2, #2097152	; 0x200000
   12fc4:	2a00000c 	bcs	12ffc <__aeabi_d2iz+0x40>
   12fc8:	5a000009 	bpl	12ff4 <__aeabi_d2iz+0x38>
   12fcc:	e3e03e3e 	mvn	r3, #992	; 0x3e0
   12fd0:	e0532ac2 	subs	r2, r3, r2, asr #21
   12fd4:	9a00000a 	bls	13004 <__aeabi_d2iz+0x48>
   12fd8:	e1a03581 	lsl	r3, r1, #11
   12fdc:	e3833102 	orr	r3, r3, #-2147483648	; 0x80000000
   12fe0:	e1833aa0 	orr	r3, r3, r0, lsr #21
   12fe4:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   12fe8:	e1a00233 	lsr	r0, r3, r2
   12fec:	12600000 	rsbne	r0, r0, #0	; 0x0
   12ff0:	e12fff1e 	bx	lr
   12ff4:	e3a00000 	mov	r0, #0	; 0x0
   12ff8:	e12fff1e 	bx	lr
   12ffc:	e1900601 	orrs	r0, r0, r1, lsl #12
   13000:	1a000002 	bne	13010 <__aeabi_d2iz+0x54>
   13004:	e2110102 	ands	r0, r1, #-2147483648	; 0x80000000
   13008:	03e00102 	mvneq	r0, #-2147483648	; 0x80000000
   1300c:	e12fff1e 	bx	lr
   13010:	e3a00000 	mov	r0, #0	; 0x0
   13014:	e12fff1e 	bx	lr

00013018 <__div0>:
   13018:	e92d4002 	push	{r1, lr}
   1301c:	e3a00008 	mov	r0, #8	; 0x8
   13020:	ebfffcb1 	bl	122ec <raise>
   13024:	e8bd8002 	pop	{r1, pc}
