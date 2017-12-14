
/home/bob/source_code/irom/irom/out/vmirom:     file format elf32-littlearm


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
      44:	0800d158 	.word	0x0800d158

00000048 <_bss_end>:
      48:	0800dca4 	.word	0x0800dca4

0000004c <__stack>:
      4c:	0800cfe0 	.word	0x0800cfe0

00000050 <reset>:
      50:	e3a00000 	mov	r0, #0	; 0x0
      54:	e3a01000 	mov	r1, #0	; 0x0
      58:	e3a02000 	mov	r2, #0	; 0x0
      5c:	e3a03000 	mov	r3, #0	; 0x0
      60:	e3a04000 	mov	r4, #0	; 0x0
      64:	e3a05000 	mov	r5, #0	; 0x0
      68:	e3a06000 	mov	r6, #0	; 0x0
      6c:	e3a07000 	mov	r7, #0	; 0x0
      70:	e3a08000 	mov	r8, #0	; 0x0
      74:	e3a09000 	mov	r9, #0	; 0x0
      78:	e3a0a000 	mov	sl, #0	; 0x0
      7c:	e3a0b000 	mov	fp, #0	; 0x0
      80:	e3a0c000 	mov	ip, #0	; 0x0
      84:	e3a0d000 	mov	sp, #0	; 0x0
      88:	e3a0e000 	mov	lr, #0	; 0x0
      8c:	e10f0000 	mrs	r0, CPSR
      90:	e3c0001f 	bic	r0, r0, #31	; 0x1f
      94:	e38000d3 	orr	r0, r0, #211	; 0xd3
      98:	e129f000 	msr	CPSR_fc, r0
      9c:	eb00000e 	bl	dc <core_init_crit>
      a0:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
      a4:	e7e10050 	ubfx	r0, r0, #0, #2
      a8:	e3500000 	cmp	r0, #0	; 0x0
      ac:	1a000014 	bne	104 <core_hold>
      b0:	e51fd06c 	ldr	sp, [pc, #-108]	; 4c <__stack>
      b4:	eb0003d6 	bl	1014 <chip_test>
      b8:	e51f007c 	ldr	r0, [pc, #-124]	; 44 <_bss_start>
      bc:	e51f107c 	ldr	r1, [pc, #-124]	; 48 <_bss_end>
      c0:	e3a02000 	mov	r2, #0	; 0x0

000000c4 <clbss_l>:
      c4:	e5802000 	str	r2, [r0]
      c8:	e2800004 	add	r0, r0, #4	; 0x4
      cc:	e1500001 	cmp	r0, r1
      d0:	1afffffb 	bne	c4 <clbss_l>
      d4:	e51ff004 	ldr	pc, [pc, #-4]	; d8 <_main>

000000d8 <_main>:
      d8:	00000540 	.word	0x00000540

000000dc <core_init_crit>:
      dc:	e3a00000 	mov	r0, #0	; 0x0
      e0:	ee080f17 	mcr	15, 0, r0, cr8, cr7, {0}
      e4:	ee070f15 	mcr	15, 0, r0, cr7, cr5, {0}
      e8:	ee110f10 	mrc	15, 0, r0, cr1, cr0, {0}
      ec:	e3c00a02 	bic	r0, r0, #8192	; 0x2000
      f0:	e3c00007 	bic	r0, r0, #7	; 0x7
      f4:	e3800002 	orr	r0, r0, #2	; 0x2
      f8:	e3800b02 	orr	r0, r0, #2048	; 0x800
      fc:	ee010f10 	mcr	15, 0, r0, cr1, cr0, {0}
     100:	e1a0f00e 	mov	pc, lr

00000104 <core_hold>:
     104:	e59f3058 	ldr	r3, [pc, #88]	; 164 <low_power+0x8>
     108:	e5931000 	ldr	r1, [r3]
     10c:	e3a0203b 	mov	r2, #59	; 0x3b
     110:	e1510002 	cmp	r1, r2
     114:	1a000010 	bne	15c <low_power>
     118:	e593100c 	ldr	r1, [r3, #12]
     11c:	e1a02401 	lsl	r2, r1, #8
     120:	e5931008 	ldr	r1, [r3, #8]
     124:	e1822001 	orr	r2, r2, r1
     128:	e1a02402 	lsl	r2, r2, #8
     12c:	e5931004 	ldr	r1, [r3, #4]
     130:	e1822001 	orr	r2, r2, r1
     134:	e1a01402 	lsl	r1, r2, #8
     138:	e1a02100 	lsl	r2, r0, #2
     13c:	e0821001 	add	r1, r2, r1
     140:	e3a02001 	mov	r2, #1	; 0x1
     144:	e5812000 	str	r2, [r1]
     148:	f57ff04f 	.word	0xf57ff04f

0000014c <hold_l>:
     14c:	e5910000 	ldr	r0, [r1]
     150:	e3500001 	cmp	r0, #1	; 0x1
     154:	0afffffc 	beq	14c <hold_l>
     158:	e1a0f000 	mov	pc, r0

0000015c <low_power>:
     15c:	e320f003 	.word	0xe320f003
     160:	eafffffe 	b	160 <low_power+0x4>
     164:	2d009c2c 	.word	0x2d009c2c
     168:	e1a00000 	.word	0xe1a00000
     16c:	e1a00000 	.word	0xe1a00000

00000170 <bl_load_isi>:
     170:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     174:	e1a04001 	mov	r4, r1
     178:	e24dd02c 	sub	sp, sp, #44	; 0x2c
     17c:	e58d0024 	str	r0, [sp, #36]
     180:	e3a01001 	mov	r1, #1	; 0x1
     184:	e1a00004 	mov	r0, r4
     188:	e1a09002 	mov	r9, r2
     18c:	eb0012ee 	bl	4d4c <vs_assign_by_id>
     190:	e2506000 	subs	r6, r0, #0	; 0x0
     194:	0a000008 	beq	1bc <bl_load_isi+0x4c>
     198:	e59f3178 	ldr	r3, [pc, #376]	; 318 <bl_load_isi+0x1a8>
     19c:	e1a00004 	mov	r0, r4
     1a0:	e5934000 	ldr	r4, [r3]
     1a4:	eb00134d 	bl	4ee0 <vs_device_name>
     1a8:	e1a02006 	mov	r2, r6
     1ac:	e1a01000 	mov	r1, r0
     1b0:	e59f0164 	ldr	r0, [pc, #356]	; 31c <bl_load_isi+0x1ac>
     1b4:	e12fff34 	blx	r4
     1b8:	ea000053 	b	30c <bl_load_isi+0x19c>
     1bc:	e1a00004 	mov	r0, r4
     1c0:	e59f1158 	ldr	r1, [pc, #344]	; 320 <bl_load_isi+0x1b0>
     1c4:	eb0013a4 	bl	505c <vs_is_device>
     1c8:	e8990030 	ldm	r9, {r4, r5}
     1cc:	e59f2150 	ldr	r2, [pc, #336]	; 324 <bl_load_isi+0x1b4>
     1d0:	e3a03000 	mov	r3, #0	; 0x0
     1d4:	e0922004 	adds	r2, r2, r4
     1d8:	e59f4148 	ldr	r4, [pc, #328]	; 328 <bl_load_isi+0x1b8>
     1dc:	e0a33005 	adc	r3, r3, r5
     1e0:	e3e05000 	mvn	r5, #0	; 0x0
     1e4:	e3500000 	cmp	r0, #0	; 0x0
     1e8:	e0033005 	and	r3, r3, r5
     1ec:	e0022004 	and	r2, r2, r4
     1f0:	03a08b01 	moveq	r8, #1024	; 0x400
     1f4:	13a08b02 	movne	r8, #2048	; 0x800
     1f8:	e889000c 	stm	r9, {r2, r3}
     1fc:	e1a02008 	mov	r2, r8
     200:	e1a03fc2 	asr	r3, r2, #31
     204:	e1cd21f8 	strd	r2, [sp, #24]
     208:	e1a07006 	mov	r7, r6
     20c:	e58d6008 	str	r6, [sp, #8]
     210:	e1a05006 	mov	r5, r6
     214:	e58d600c 	str	r6, [sp, #12]
     218:	e1570008 	cmp	r7, r8
     21c:	1a00000e 	bne	25c <bl_load_isi+0xec>
     220:	e59d0024 	ldr	r0, [sp, #36]
     224:	eb0005d3 	bl	1978 <isi_check_header>
     228:	e2502000 	subs	r2, r0, #0	; 0x0
     22c:	e59f00f8 	ldr	r0, [pc, #248]	; 32c <bl_load_isi+0x1bc>
     230:	1a000007 	bne	254 <bl_load_isi+0xe4>
     234:	e59da024 	ldr	sl, [sp, #36]
     238:	e59fb0d8 	ldr	fp, [pc, #216]	; 318 <bl_load_isi+0x1a8>
     23c:	e59a300c 	ldr	r3, [sl, #12]
     240:	e2835c02 	add	r5, r3, #512	; 0x200
     244:	e1a01005 	mov	r1, r5
     248:	e1a0e00f 	mov	lr, pc
     24c:	e59bf000 	ldr	pc, [fp]
     250:	ea000001 	b	25c <bl_load_isi+0xec>
     254:	e59f00d4 	ldr	r0, [pc, #212]	; 330 <bl_load_isi+0x1c0>
     258:	ea00000c 	b	290 <bl_load_isi+0x120>
     25c:	e2554000 	subs	r4, r5, #0	; 0x0
     260:	13a04001 	movne	r4, #1	; 0x1
     264:	e1570005 	cmp	r7, r5
     268:	e59d1024 	ldr	r1, [sp, #36]
     26c:	b3a04000 	movlt	r4, #0	; 0x0
     270:	e3540000 	cmp	r4, #0	; 0x0
     274:	e0810007 	add	r0, r1, r7
     278:	0a000006 	beq	298 <bl_load_isi+0x128>
     27c:	e1a00001 	mov	r0, r1
     280:	eb000615 	bl	1adc <isi_check_data>
     284:	e3500000 	cmp	r0, #0	; 0x0
     288:	0a00001f 	beq	30c <bl_load_isi+0x19c>
     28c:	e59f00a0 	ldr	r0, [pc, #160]	; 334 <bl_load_isi+0x1c4>
     290:	e3a01000 	mov	r1, #0	; 0x0
     294:	ea000006 	b	2b4 <bl_load_isi+0x144>
     298:	e59d200c 	ldr	r2, [sp, #12]
     29c:	e3520004 	cmp	r2, #4	; 0x4
     2a0:	e2822001 	add	r2, r2, #1	; 0x1
     2a4:	e58d200c 	str	r2, [sp, #12]
     2a8:	da000005 	ble	2c4 <bl_load_isi+0x154>
     2ac:	e59f0080 	ldr	r0, [pc, #128]	; 334 <bl_load_isi+0x1c4>
     2b0:	e1a01004 	mov	r1, r4
     2b4:	e59f305c 	ldr	r3, [pc, #92]	; 318 <bl_load_isi+0x1a8>
     2b8:	e1a0e00f 	mov	lr, pc
     2bc:	e593f000 	ldr	pc, [r3]
     2c0:	ea000010 	b	308 <bl_load_isi+0x198>
     2c4:	e899000c 	ldm	r9, {r2, r3}
     2c8:	e58d8000 	str	r8, [sp]
     2cc:	e58d4004 	str	r4, [sp, #4]
     2d0:	eb00137a 	bl	50c0 <vs_read>
     2d4:	e8990c00 	ldm	r9, {sl, fp}
     2d8:	e1cd21d8 	ldrd	r2, [sp, #24]
     2dc:	e09aa002 	adds	sl, sl, r2
     2e0:	e0abb003 	adc	fp, fp, r3
     2e4:	e59d3008 	ldr	r3, [sp, #8]
     2e8:	e1500008 	cmp	r0, r8
     2ec:	e2833001 	add	r3, r3, #1	; 0x1
     2f0:	00877008 	addeq	r7, r7, r8
     2f4:	058d400c 	streq	r4, [sp, #12]
     2f8:	e3530080 	cmp	r3, #128	; 0x80
     2fc:	e8890c00 	stm	r9, {sl, fp}
     300:	e58d3008 	str	r3, [sp, #8]
     304:	1affffc3 	bne	218 <bl_load_isi+0xa8>
     308:	e3e06000 	mvn	r6, #0	; 0x0
     30c:	e1a00006 	mov	r0, r6
     310:	e28dd02c 	add	sp, sp, #44	; 0x2c
     314:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     318:	0800d704 	.word	0x0800d704
     31c:	00010931 	.word	0x00010931
     320:	00010955 	.word	0x00010955
     324:	0001ffff 	.word	0x0001ffff
     328:	fffe0000 	.word	0xfffe0000
     32c:	0001095a 	.word	0x0001095a
     330:	00010971 	.word	0x00010971
     334:	00010983 	.word	0x00010983

00000338 <try_get_bl>:
     338:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     33c:	e3a02602 	mov	r2, #2097152	; 0x200000
     340:	e24dd00c 	sub	sp, sp, #12	; 0xc
     344:	e3a03000 	mov	r3, #0	; 0x0
     348:	e88d000c 	stm	sp, {r2, r3}
     34c:	e1a07001 	mov	r7, r1
     350:	e1a06000 	mov	r6, r0
     354:	eb0000aa 	bl	604 <boot_device>
     358:	e1a04000 	mov	r4, r0
     35c:	eb001298 	bl	4dc4 <vs_device_bootable>
     360:	e2505000 	subs	r5, r0, #0	; 0x0
     364:	1a000008 	bne	38c <try_get_bl+0x54>
     368:	e59f308c 	ldr	r3, [pc, #140]	; 3fc <try_get_bl+0xc4>
     36c:	e1a00004 	mov	r0, r4
     370:	e5934000 	ldr	r4, [r3]
     374:	eb0012d9 	bl	4ee0 <vs_device_name>
     378:	e1a01000 	mov	r1, r0
     37c:	e59f007c 	ldr	r0, [pc, #124]	; 400 <try_get_bl+0xc8>
     380:	e12fff34 	blx	r4
     384:	e1a00005 	mov	r0, r5
     388:	ea000019 	b	3f4 <try_get_bl+0xbc>
     38c:	e1a00004 	mov	r0, r4
     390:	eb0012c0 	bl	4e98 <vs_device_boot0>
     394:	e3500000 	cmp	r0, #0	; 0x0
     398:	13a02000 	movne	r2, #0	; 0x0
     39c:	13a03000 	movne	r3, #0	; 0x0
     3a0:	188d000c 	stmne	sp, {r2, r3}
     3a4:	e3a00302 	mov	r0, #134217728	; 0x8000000
     3a8:	e1a01004 	mov	r1, r4
     3ac:	e1a0200d 	mov	r2, sp
     3b0:	e3a03000 	mov	r3, #0	; 0x0
     3b4:	ebffff6d 	bl	170 <bl_load_isi>
     3b8:	e3500000 	cmp	r0, #0	; 0x0
     3bc:	13a00000 	movne	r0, #0	; 0x0
     3c0:	1a00000b 	bne	3f4 <try_get_bl+0xbc>
     3c4:	e3a03302 	mov	r3, #134217728	; 0x8000000
     3c8:	e5932008 	ldr	r2, [r3, #8]
     3cc:	e3c22102 	bic	r2, r2, #-2147483648	; 0x80000000
     3d0:	e5862000 	str	r2, [r6]
     3d4:	e5874000 	str	r4, [r7]
     3d8:	e5933008 	ldr	r3, [r3, #8]
     3dc:	e3530000 	cmp	r3, #0	; 0x0
     3e0:	aa000002 	bge	3f0 <try_get_bl+0xb8>
     3e4:	e3a00001 	mov	r0, #1	; 0x1
     3e8:	e59f1014 	ldr	r1, [pc, #20]	; 404 <try_get_bl+0xcc>
     3ec:	eb001759 	bl	6158 <cdbg_verify_burn_enable>
     3f0:	e3a00001 	mov	r0, #1	; 0x1
     3f4:	e28dd00c 	add	sp, sp, #12	; 0xc
     3f8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     3fc:	0800d704 	.word	0x0800d704
     400:	00010995 	.word	0x00010995
     404:	08000100 	.word	0x08000100

00000408 <try_boot>:
     408:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
     40c:	e3a03000 	mov	r3, #0	; 0x0
     410:	e24dd008 	sub	sp, sp, #8	; 0x8
     414:	e1a07000 	mov	r7, r0
     418:	e1a00003 	mov	r0, r3
     41c:	e58d3004 	str	r3, [sp, #4]
     420:	e58d3000 	str	r3, [sp]
     424:	e1a04003 	mov	r4, r3
     428:	eb00005b 	bl	59c <set_xom>
     42c:	eb000061 	bl	5b8 <boot_info>
     430:	e28d8004 	add	r8, sp, #4	; 0x4
     434:	e1a05000 	mov	r5, r0
     438:	ea000008 	b	460 <try_boot+0x58>
     43c:	eb00012e 	bl	8fc <gtc_time>
     440:	e1a00004 	mov	r0, r4
     444:	eb000054 	bl	59c <set_xom>
     448:	e1a00008 	mov	r0, r8
     44c:	e1a0100d 	mov	r1, sp
     450:	ebffffb8 	bl	338 <try_get_bl>
     454:	e3500000 	cmp	r0, #0	; 0x0
     458:	13a04001 	movne	r4, #1	; 0x1
     45c:	1a000003 	bne	470 <try_boot+0x68>
     460:	e1540005 	cmp	r4, r5
     464:	e2844001 	add	r4, r4, #1	; 0x1
     468:	bafffff3 	blt	43c <try_boot+0x34>
     46c:	e3a04000 	mov	r4, #0	; 0x0
     470:	eb000121 	bl	8fc <gtc_time>
     474:	e3a00001 	mov	r0, #1	; 0x1
     478:	eb000135 	bl	954 <ecfg_check_flag>
     47c:	e3500000 	cmp	r0, #0	; 0x0
     480:	1a00000c 	bne	4b8 <try_boot+0xb0>
     484:	e3570000 	cmp	r7, #0	; 0x0
     488:	1a00000a 	bne	4b8 <try_boot+0xb0>
     48c:	eb0000c9 	bl	7b8 <get_reset_stats>
     490:	e3500000 	cmp	r0, #0	; 0x0
     494:	13500002 	cmpne	r0, #2	; 0x2
     498:	1a000006 	bne	4b8 <try_boot+0xb0>
     49c:	e3500002 	cmp	r0, #2	; 0x2
     4a0:	1a000001 	bne	4ac <try_boot+0xa4>
     4a4:	e28d0004 	add	r0, sp, #4	; 0x4
     4a8:	eb0000cf 	bl	7ec <update_mask_on_reset>
     4ac:	e59d0004 	ldr	r0, [sp, #4]
     4b0:	eb0018e6 	bl	6850 <burn>
     4b4:	eb000110 	bl	8fc <gtc_time>
     4b8:	e3540000 	cmp	r4, #0	; 0x0
     4bc:	0a000004 	beq	4d4 <try_boot+0xcc>
     4c0:	e3a03302 	mov	r3, #134217728	; 0x8000000
     4c4:	e5930014 	ldr	r0, [r3, #20]
     4c8:	e59d1000 	ldr	r1, [sp]
     4cc:	eb00172c 	bl	6184 <cdbg_jump>
     4d0:	eafffffe 	b	4d0 <try_boot+0xc8>
     4d4:	eb00175c 	bl	624c <cdbg_shell>
     4d8:	eafffffe 	b	4d8 <try_boot+0xd0>

000004dc <init_data_section>:
     4dc:	e59f1010 	ldr	r1, [pc, #16]	; 4f4 <init_data_section+0x18>
     4e0:	e59f0010 	ldr	r0, [pc, #16]	; 4f8 <init_data_section+0x1c>
     4e4:	e1510000 	cmp	r1, r0
     4e8:	012fff1e 	bxeq	lr
     4ec:	e59f2008 	ldr	r2, [pc, #8]	; 4fc <init_data_section+0x20>
     4f0:	ea0003b8 	b	13d8 <memcpy>
     4f4:	000129d8 	.word	0x000129d8
     4f8:	0800d000 	.word	0x0800d000
     4fc:	00000154 	.word	0x00000154

00000500 <initialization>:
     500:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     504:	e24dd004 	sub	sp, sp, #4	; 0x4
     508:	ebfffff3 	bl	4dc <init_data_section>
     50c:	eb000027 	bl	5b0 <init_xom>
     510:	eb000098 	bl	778 <init_lowlevel>
     514:	eb0004d9 	bl	1880 <init_malloc>
     518:	eb002113 	bl	896c <init_timer>
     51c:	eb000103 	bl	930 <init_gtc>
     520:	eb0020f8 	bl	8908 <init_serial>
     524:	eb001a24 	bl	6dbc <init_mmc>
     528:	eb0000a1 	bl	7b4 <init_crypto>
     52c:	eb000235 	bl	e08 <init_alternate>
     530:	eb00001f 	bl	5b4 <init_boot_sequence>
     534:	e3a00000 	mov	r0, #0	; 0x0
     538:	e28dd004 	add	sp, sp, #4	; 0x4
     53c:	e8bd8000 	pop	{pc}

00000540 <main>:
     540:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     544:	e3a00001 	mov	r0, #1	; 0x1
     548:	e24dd004 	sub	sp, sp, #4	; 0x4
     54c:	eb0016e5 	bl	60e8 <cdbg_log_toggle>
     550:	ebffffea 	bl	500 <initialization>
     554:	eb0000e8 	bl	8fc <gtc_time>
     558:	e3a00001 	mov	r0, #1	; 0x1
     55c:	eb0016e1 	bl	60e8 <cdbg_log_toggle>
     560:	e59f001c 	ldr	r0, [pc, #28]	; 584 <main+0x44>
     564:	e59f301c 	ldr	r3, [pc, #28]	; 588 <main+0x48>
     568:	e1a0e00f 	mov	lr, pc
     56c:	e593f000 	ldr	pc, [r3]
     570:	e3a00001 	mov	r0, #1	; 0x1
     574:	eb0016db 	bl	60e8 <cdbg_log_toggle>
     578:	e3a00000 	mov	r0, #0	; 0x0
     57c:	ebffffa1 	bl	408 <try_boot>
     580:	eafffffe 	b	580 <main+0x40>
     584:	000109af 	.word	0x000109af
     588:	0800d704 	.word	0x0800d704

0000058c <get_xom>:
     58c:	e59f3004 	ldr	r3, [pc, #4]	; 598 <get_xom+0xc>
     590:	e1d300b0 	ldrh	r0, [r3]
     594:	e12fff1e 	bx	lr
     598:	0800d158 	.word	0x0800d158

0000059c <set_xom>:
     59c:	e59f3008 	ldr	r3, [pc, #8]	; 5ac <set_xom+0x10>
     5a0:	e200001f 	and	r0, r0, #31	; 0x1f
     5a4:	e1c300b0 	strh	r0, [r3]
     5a8:	e12fff1e 	bx	lr
     5ac:	0800d158 	.word	0x0800d158

000005b0 <init_xom>:
     5b0:	e12fff1e 	bx	lr

000005b4 <init_boot_sequence>:
     5b4:	e12fff1e 	bx	lr

000005b8 <boot_info>:
     5b8:	e92d4010 	push	{r4, lr}
     5bc:	eb0000ed 	bl	978 <ecfg_paras>
     5c0:	e1a04000 	mov	r4, r0
     5c4:	e3a00b01 	mov	r0, #1024	; 0x400
     5c8:	eb0000e1 	bl	954 <ecfg_check_flag>
     5cc:	e59f3028 	ldr	r3, [pc, #40]	; 5fc <boot_info+0x44>
     5d0:	e3500000 	cmp	r0, #0	; 0x0
     5d4:	11d330b0 	ldrhne	r3, [r3]
     5d8:	01d330b0 	ldrheq	r3, [r3]
     5dc:	1203301f 	andne	r3, r3, #31	; 0x1f
     5e0:	0203301f 	andeq	r3, r3, #31	; 0x1f
     5e4:	159f2014 	ldrne	r2, [pc, #20]	; 600 <boot_info+0x48>
     5e8:	11a03083 	lslne	r3, r3, #1
     5ec:	00843083 	addeq	r3, r4, r3, lsl #1
     5f0:	119200b3 	ldrhne	r0, [r2, r3]
     5f4:	01d302b0 	ldrheq	r0, [r3, #32]
     5f8:	e8bd8010 	pop	{r4, pc}
     5fc:	0800d158 	.word	0x0800d158
     600:	000105b4 	.word	0x000105b4

00000604 <boot_device>:
     604:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     608:	e24dd004 	sub	sp, sp, #4	; 0x4
     60c:	ebffffe9 	bl	5b8 <boot_info>
     610:	e1a00620 	lsr	r0, r0, #12
     614:	e6ff0070 	uxth	r0, r0
     618:	e28dd004 	add	sp, sp, #4	; 0x4
     61c:	e8bd8000 	pop	{pc}

00000620 <set_pll>:
     620:	e3500003 	cmp	r0, #3	; 0x3
     624:	e92d4070 	push	{r4, r5, r6, lr}
     628:	e1a06001 	mov	r6, r1
     62c:	88bd8070 	pophi	{r4, r5, r6, pc}
     630:	e3a03030 	mov	r3, #48	; 0x30
     634:	e0030093 	mul	r3, r3, r0
     638:	e3510e32 	cmp	r1, #800	; 0x320
     63c:	e283542d 	add	r5, r3, #754974720	; 0x2d000000
     640:	e2855a0a 	add	r5, r5, #40960	; 0xa000
     644:	33a03012 	movcc	r3, #18	; 0x12
     648:	3585300c 	strcc	r3, [r5, #12]
     64c:	31a06081 	lslcc	r6, r1, #1
     650:	e3a04001 	mov	r4, #1	; 0x1
     654:	e1a01004 	mov	r1, r4
     658:	e3a00018 	mov	r0, #24	; 0x18
     65c:	eb003b1f 	bl	f2e0 <__aeabi_idiv>
     660:	e1a01000 	mov	r1, r0
     664:	e1a00006 	mov	r0, r6
     668:	eb003b16 	bl	f2c8 <__aeabi_uidivmod>
     66c:	e3510000 	cmp	r1, #0	; 0x0
     670:	0a000003 	beq	684 <set_pll+0x64>
     674:	e2844001 	add	r4, r4, #1	; 0x1
     678:	e3540040 	cmp	r4, #64	; 0x40
     67c:	1afffff4 	bne	654 <set_pll+0x34>
     680:	e3a0403f 	mov	r4, #63	; 0x3f
     684:	e20430ff 	and	r3, r4, #255	; 0xff
     688:	e5853000 	str	r3, [r5]
     68c:	e1a01004 	mov	r1, r4
     690:	e3a00018 	mov	r0, #24	; 0x18
     694:	eb003b11 	bl	f2e0 <__aeabi_idiv>
     698:	e1a01000 	mov	r1, r0
     69c:	e1a00006 	mov	r0, r6
     6a0:	eb003a8c 	bl	f0d8 <__aeabi_uidiv>
     6a4:	e1a03440 	asr	r3, r0, #8
     6a8:	e3833010 	orr	r3, r3, #16	; 0x10
     6ac:	e20000ff 	and	r0, r0, #255	; 0xff
     6b0:	e5850004 	str	r0, [r5, #4]
     6b4:	e3a01001 	mov	r1, #1	; 0x1
     6b8:	e5853008 	str	r3, [r5, #8]
     6bc:	e3a02000 	mov	r2, #0	; 0x0
     6c0:	e3a03002 	mov	r3, #2	; 0x2
     6c4:	e5c5101c 	strb	r1, [r5, #28]
     6c8:	e5c52020 	strb	r2, [r5, #32]
     6cc:	e5c53024 	strb	r3, [r5, #36]
     6d0:	e0833001 	add	r3, r3, r1
     6d4:	e1a00002 	mov	r0, r2
     6d8:	e5c5301c 	strb	r3, [r5, #28]
     6dc:	e2852028 	add	r2, r5, #40	; 0x28
     6e0:	e5c51020 	strb	r1, [r5, #32]
     6e4:	e3011a0a 	movw	r1, #6666	; 0x1a0a
     6e8:	ea000001 	b	6f4 <set_pll+0xd4>
     6ec:	e1500001 	cmp	r0, r1
     6f0:	0a000003 	beq	704 <set_pll+0xe4>
     6f4:	e5d23000 	ldrb	r3, [r2]
     6f8:	e2800001 	add	r0, r0, #1	; 0x1
     6fc:	e3130001 	tst	r3, #1	; 0x1
     700:	0afffff9 	beq	6ec <set_pll+0xcc>
     704:	e3a03002 	mov	r3, #2	; 0x2
     708:	e585301c 	str	r3, [r5, #28]
     70c:	e8bd8070 	pop	{r4, r5, r6, pc}

00000710 <module_set_clock>:
     710:	e59f3020 	ldr	r3, [pc, #32]	; 738 <module_set_clock+0x28>
     714:	e1500003 	cmp	r0, r3
     718:	13a03001 	movne	r3, #1	; 0x1
     71c:	1580300c 	strne	r3, [r0, #12]
     720:	e2013007 	and	r3, r1, #7	; 0x7
     724:	e5803000 	str	r3, [r0]
     728:	e3a03000 	mov	r3, #0	; 0x0
     72c:	e5802004 	str	r2, [r0, #4]
     730:	e580300c 	str	r3, [r0, #12]
     734:	e12fff1e 	bx	lr
     738:	2d00a360 	.word	0x2d00a360

0000073c <module_enable>:
     73c:	e3a020ff 	mov	r2, #255	; 0xff
     740:	e3a01000 	mov	r1, #0	; 0x0
     744:	e3a03011 	mov	r3, #17	; 0x11
     748:	e580100c 	str	r1, [r0, #12]
     74c:	e5802000 	str	r2, [r0]
     750:	e5802004 	str	r2, [r0, #4]
     754:	e5803008 	str	r3, [r0, #8]
     758:	e5801014 	str	r1, [r0, #20]
     75c:	e5903008 	ldr	r3, [r0, #8]
     760:	e3c33010 	bic	r3, r3, #16	; 0x10
     764:	e5803008 	str	r3, [r0, #8]
     768:	e5802018 	str	r2, [r0, #24]
     76c:	e5801000 	str	r1, [r0]
     770:	e580200c 	str	r2, [r0, #12]
     774:	e12fff1e 	bx	lr

00000778 <init_lowlevel>:
     778:	e3a02000 	mov	r2, #0	; 0x0
     77c:	e3e01000 	mvn	r1, #0	; 0x0
     780:	e1a03102 	lsl	r3, r2, #2
     784:	e2833306 	add	r3, r3, #402653184	; 0x18000000
     788:	e2822001 	add	r2, r2, #1	; 0x1
     78c:	e2833d46 	add	r3, r3, #4480	; 0x1180
     790:	e3520008 	cmp	r2, #8	; 0x8
     794:	e5831000 	str	r1, [r3]
     798:	1afffff8 	bne	780 <init_lowlevel+0x8>
     79c:	e12fff1e 	bx	lr

000007a0 <init_igps_ram>:
     7a0:	e59f3008 	ldr	r3, [pc, #8]	; 7b0 <init_igps_ram+0x10>
     7a4:	e3a02001 	mov	r2, #1	; 0x1
     7a8:	e5832410 	str	r2, [r3, #1040]
     7ac:	e12fff1e 	bx	lr
     7b0:	2d00c000 	.word	0x2d00c000

000007b4 <init_crypto>:
     7b4:	e12fff1e 	bx	lr

000007b8 <get_reset_stats>:
     7b8:	e59f2028 	ldr	r2, [pc, #40]	; 7e8 <get_reset_stats+0x30>
     7bc:	e5923c10 	ldr	r3, [r2, #3088]
     7c0:	e2130002 	ands	r0, r3, #2	; 0x2
     7c4:	012fff1e 	bxeq	lr
     7c8:	e5920c2c 	ldr	r0, [r2, #3116]
     7cc:	e3100001 	tst	r0, #1	; 0x1
     7d0:	03a00001 	moveq	r0, #1	; 0x1
     7d4:	012fff1e 	bxeq	lr
     7d8:	e3500037 	cmp	r0, #55	; 0x37
     7dc:	03a00002 	moveq	r0, #2	; 0x2
     7e0:	13a00003 	movne	r0, #3	; 0x3
     7e4:	e12fff1e 	bx	lr
     7e8:	2d009000 	.word	0x2d009000

000007ec <update_mask_on_reset>:
     7ec:	e59f3018 	ldr	r3, [pc, #24]	; 80c <update_mask_on_reset+0x20>
     7f0:	e5932c30 	ldr	r2, [r3, #3120]
     7f4:	e3120080 	tst	r2, #128	; 0x80
     7f8:	13a03001 	movne	r3, #1	; 0x1
     7fc:	1202200f 	andne	r2, r2, #15	; 0xf
     800:	11e03213 	mvnne	r3, r3, lsl r2
     804:	15803000 	strne	r3, [r0]
     808:	e12fff1e 	bx	lr
     80c:	2d009000 	.word	0x2d009000

00000810 <get_wakeup_entry>:
     810:	e59f301c 	ldr	r3, [pc, #28]	; 834 <get_wakeup_entry+0x24>
     814:	e5931c38 	ldr	r1, [r3, #3128]
     818:	e5932c34 	ldr	r2, [r3, #3124]
     81c:	e5930c30 	ldr	r0, [r3, #3120]
     820:	e1822401 	orr	r2, r2, r1, lsl #8
     824:	e5933c2c 	ldr	r3, [r3, #3116]
     828:	e1800402 	orr	r0, r0, r2, lsl #8
     82c:	e1830400 	orr	r0, r3, r0, lsl #8
     830:	e12fff1e 	bx	lr
     834:	2d009000 	.word	0x2d009000

00000838 <system_reset>:
     838:	e59f3010 	ldr	r3, [pc, #16]	; 850 <system_reset+0x18>
     83c:	e3a02001 	mov	r2, #1	; 0x1
     840:	e5832c2c 	str	r2, [r3, #3116]
     844:	e2822002 	add	r2, r2, #2	; 0x2
     848:	e5832c00 	str	r2, [r3, #3072]
     84c:	e12fff1e 	bx	lr
     850:	2d009000 	.word	0x2d009000

00000854 <pads_init_module>:
     854:	e92d4070 	push	{r4, r5, r6, lr}
     858:	e1a06000 	mov	r6, r0
     85c:	e1a05001 	mov	r5, r1
     860:	e59f406c 	ldr	r4, [pc, #108]	; 8d4 <pads_init_module+0x80>
     864:	ea000016 	b	8c4 <pads_init_module+0x70>
     868:	e1500006 	cmp	r0, r6
     86c:	1a000013 	bne	8c0 <pads_init_module+0x6c>
     870:	e3a01000 	mov	r1, #0	; 0x0
     874:	e1a02001 	mov	r2, r1
     878:	e514000c 	ldr	r0, [r4, #-12]
     87c:	e1a03005 	mov	r3, r5
     880:	eb0000cc 	bl	bb8 <pads_chmod>
     884:	e5140004 	ldr	r0, [r4, #-4]
     888:	e3a01000 	mov	r1, #0	; 0x0
     88c:	e1500001 	cmp	r0, r1
     890:	e1a02001 	mov	r2, r1
     894:	e1a03005 	mov	r3, r5
     898:	da000000 	ble	8a0 <pads_init_module+0x4c>
     89c:	eb0000c5 	bl	bb8 <pads_chmod>
     8a0:	e594c000 	ldr	ip, [r4]
     8a4:	e3a01000 	mov	r1, #0	; 0x0
     8a8:	e15c0001 	cmp	ip, r1
     8ac:	e1a02001 	mov	r2, r1
     8b0:	e1a03005 	mov	r3, r5
     8b4:	e1a0000c 	mov	r0, ip
     8b8:	da000000 	ble	8c0 <pads_init_module+0x6c>
     8bc:	eb0000bd 	bl	bb8 <pads_chmod>
     8c0:	e2844014 	add	r4, r4, #20	; 0x14
     8c4:	e5140010 	ldr	r0, [r4, #-16]
     8c8:	e3500000 	cmp	r0, #0	; 0x0
     8cc:	1affffe5 	bne	868 <pads_init_module+0x14>
     8d0:	e8bd8070 	pop	{r4, r5, r6, pc}
     8d4:	0800d010 	.word	0x0800d010

000008d8 <module_reset>:
     8d8:	e3a030ff 	mov	r3, #255	; 0xff
     8dc:	e5803000 	str	r3, [r0]
     8e0:	e92d4010 	push	{r4, lr}
     8e4:	e1a04000 	mov	r4, r0
     8e8:	e3a0000a 	mov	r0, #10	; 0xa
     8ec:	eb002049 	bl	8a18 <udelay>
     8f0:	e3a03000 	mov	r3, #0	; 0x0
     8f4:	e5843000 	str	r3, [r4]
     8f8:	e8bd8010 	pop	{r4, pc}

000008fc <gtc_time>:
     8fc:	e59f3020 	ldr	r3, [pc, #32]	; 924 <gtc_time+0x28>
     900:	e92d4010 	push	{r4, lr}
     904:	e5930000 	ldr	r0, [r3]
     908:	e59f3018 	ldr	r3, [pc, #24]	; 928 <gtc_time+0x2c>
     90c:	e5934000 	ldr	r4, [r3]
     910:	eb00204c 	bl	8a48 <get_timer>
     914:	e1a01000 	mov	r1, r0
     918:	e59f000c 	ldr	r0, [pc, #12]	; 92c <gtc_time+0x30>
     91c:	e12fff34 	blx	r4
     920:	e8bd8010 	pop	{r4, pc}
     924:	0800d15c 	.word	0x0800d15c
     928:	0800d704 	.word	0x0800d704
     92c:	000109b4 	.word	0x000109b4

00000930 <init_gtc>:
     930:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     934:	e3a00000 	mov	r0, #0	; 0x0
     938:	e24dd004 	sub	sp, sp, #4	; 0x4
     93c:	eb002041 	bl	8a48 <get_timer>
     940:	e59f3008 	ldr	r3, [pc, #8]	; 950 <init_gtc+0x20>
     944:	e5830000 	str	r0, [r3]
     948:	e28dd004 	add	sp, sp, #4	; 0x4
     94c:	e8bd8000 	pop	{pc}
     950:	0800d15c 	.word	0x0800d15c

00000954 <ecfg_check_flag>:
     954:	e3500801 	cmp	r0, #65536	; 0x10000
     958:	0a000004 	beq	970 <ecfg_check_flag+0x1c>
     95c:	e3500101 	cmp	r0, #1073741824	; 0x40000000
     960:	0a000002 	beq	970 <ecfg_check_flag+0x1c>
     964:	e3500001 	cmp	r0, #1	; 0x1
     968:	13a00001 	movne	r0, #1	; 0x1
     96c:	112fff1e 	bxne	lr
     970:	e3a00000 	mov	r0, #0	; 0x0
     974:	e12fff1e 	bx	lr

00000978 <ecfg_paras>:
     978:	e59f0000 	ldr	r0, [pc, #0]	; 980 <ecfg_paras+0x8>
     97c:	e12fff1e 	bx	lr
     980:	0800d708 	.word	0x0800d708

00000984 <efuse_mac>:
     984:	e3a00000 	mov	r0, #0	; 0x0
     988:	e3a01000 	mov	r1, #0	; 0x0
     98c:	e12fff1e 	bx	lr

00000990 <efuse_info>:
     990:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     994:	e1a04000 	mov	r4, r0
     998:	e24dd004 	sub	sp, sp, #4	; 0x4
     99c:	e59f6144 	ldr	r6, [pc, #324]	; ae8 <efuse_info+0x158>
     9a0:	e59f0144 	ldr	r0, [pc, #324]	; aec <efuse_info+0x15c>
     9a4:	e1a0e00f 	mov	lr, pc
     9a8:	e596f000 	ldr	pc, [r6]
     9ac:	e59f013c 	ldr	r0, [pc, #316]	; af0 <efuse_info+0x160>
     9b0:	e1a0e00f 	mov	lr, pc
     9b4:	e596f000 	ldr	pc, [r6]
     9b8:	e3540000 	cmp	r4, #0	; 0x0
     9bc:	0a00003c 	beq	ab4 <efuse_info+0x124>
     9c0:	e59f412c 	ldr	r4, [pc, #300]	; af4 <efuse_info+0x164>
     9c4:	e59f012c 	ldr	r0, [pc, #300]	; af8 <efuse_info+0x168>
     9c8:	e5941000 	ldr	r1, [r4]
     9cc:	e1a0e00f 	mov	lr, pc
     9d0:	e596f000 	ldr	pc, [r6]
     9d4:	e5d41004 	ldrb	r1, [r4, #4]
     9d8:	e59f011c 	ldr	r0, [pc, #284]	; afc <efuse_info+0x16c>
     9dc:	e1a0e00f 	mov	lr, pc
     9e0:	e596f000 	ldr	pc, [r6]
     9e4:	e5d41005 	ldrb	r1, [r4, #5]
     9e8:	e59f0110 	ldr	r0, [pc, #272]	; b00 <efuse_info+0x170>
     9ec:	e1a0e00f 	mov	lr, pc
     9f0:	e596f000 	ldr	pc, [r6]
     9f4:	e5d41006 	ldrb	r1, [r4, #6]
     9f8:	e59f0104 	ldr	r0, [pc, #260]	; b04 <efuse_info+0x174>
     9fc:	e1a0e00f 	mov	lr, pc
     a00:	e596f000 	ldr	pc, [r6]
     a04:	e5d41007 	ldrb	r1, [r4, #7]
     a08:	e59f00f8 	ldr	r0, [pc, #248]	; b08 <efuse_info+0x178>
     a0c:	e1a0e00f 	mov	lr, pc
     a10:	e596f000 	ldr	pc, [r6]
     a14:	e1d410b8 	ldrh	r1, [r4, #8]
     a18:	e59f00ec 	ldr	r0, [pc, #236]	; b0c <efuse_info+0x17c>
     a1c:	e1a0e00f 	mov	lr, pc
     a20:	e596f000 	ldr	pc, [r6]
     a24:	e1d410ba 	ldrh	r1, [r4, #10]
     a28:	e59f00e0 	ldr	r0, [pc, #224]	; b10 <efuse_info+0x180>
     a2c:	e1a0e00f 	mov	lr, pc
     a30:	e596f000 	ldr	pc, [r6]
     a34:	e1d410bc 	ldrh	r1, [r4, #12]
     a38:	e59f00d4 	ldr	r0, [pc, #212]	; b14 <efuse_info+0x184>
     a3c:	e1a0e00f 	mov	lr, pc
     a40:	e596f000 	ldr	pc, [r6]
     a44:	e1d410be 	ldrh	r1, [r4, #14]
     a48:	e59f00c8 	ldr	r0, [pc, #200]	; b18 <efuse_info+0x188>
     a4c:	e1a0e00f 	mov	lr, pc
     a50:	e596f000 	ldr	pc, [r6]
     a54:	e1a05004 	mov	r5, r4
     a58:	e3a04000 	mov	r4, #0	; 0x0
     a5c:	e1a01004 	mov	r1, r4
     a60:	e5952010 	ldr	r2, [r5, #16]
     a64:	e2844001 	add	r4, r4, #1	; 0x1
     a68:	e59f00ac 	ldr	r0, [pc, #172]	; b1c <efuse_info+0x18c>
     a6c:	e1a0e00f 	mov	lr, pc
     a70:	e596f000 	ldr	pc, [r6]
     a74:	e3540004 	cmp	r4, #4	; 0x4
     a78:	e2855004 	add	r5, r5, #4	; 0x4
     a7c:	1afffff6 	bne	a5c <efuse_info+0xcc>
     a80:	e59f6060 	ldr	r6, [pc, #96]	; ae8 <efuse_info+0x158>
     a84:	e59f5068 	ldr	r5, [pc, #104]	; af4 <efuse_info+0x164>
     a88:	e284400c 	add	r4, r4, #12	; 0xc
     a8c:	e1a03084 	lsl	r3, r4, #1
     a90:	e1a01004 	mov	r1, r4
     a94:	e19520b3 	ldrh	r2, [r5, r3]
     a98:	e2844001 	add	r4, r4, #1	; 0x1
     a9c:	e59f007c 	ldr	r0, [pc, #124]	; b20 <efuse_info+0x190>
     aa0:	e1a0e00f 	mov	lr, pc
     aa4:	e596f000 	ldr	pc, [r6]
     aa8:	e354001c 	cmp	r4, #28	; 0x1c
     aac:	1afffff6 	bne	a8c <efuse_info+0xfc>
     ab0:	ea00000a 	b	ae0 <efuse_info+0x150>
     ab4:	e59f4068 	ldr	r4, [pc, #104]	; b24 <efuse_info+0x194>
     ab8:	e59f7068 	ldr	r7, [pc, #104]	; b28 <efuse_info+0x198>
     abc:	e1a05006 	mov	r5, r6
     ac0:	e5942000 	ldr	r2, [r4]
     ac4:	e1a01004 	mov	r1, r4
     ac8:	e59f005c 	ldr	r0, [pc, #92]	; b2c <efuse_info+0x19c>
     acc:	e2844004 	add	r4, r4, #4	; 0x4
     ad0:	e1a0e00f 	mov	lr, pc
     ad4:	e595f000 	ldr	pc, [r5]
     ad8:	e1540007 	cmp	r4, r7
     adc:	1afffff7 	bne	ac0 <efuse_info+0x130>
     ae0:	e28dd004 	add	sp, sp, #4	; 0x4
     ae4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     ae8:	0800d704 	.word	0x0800d704
     aec:	000109bc 	.word	0x000109bc
     af0:	000109d1 	.word	0x000109d1
     af4:	0800d708 	.word	0x0800d708
     af8:	000109e8 	.word	0x000109e8
     afc:	000109f7 	.word	0x000109f7
     b00:	00010a06 	.word	0x00010a06
     b04:	00010a19 	.word	0x00010a19
     b08:	00010a28 	.word	0x00010a28
     b0c:	00010a3d 	.word	0x00010a3d
     b10:	00010a4a 	.word	0x00010a4a
     b14:	00010a5a 	.word	0x00010a5a
     b18:	00010a6b 	.word	0x00010a6b
     b1c:	00010a7d 	.word	0x00010a7d
     b20:	00010a8d 	.word	0x00010a8d
     b24:	2d00d9b4 	.word	0x2d00d9b4
     b28:	2d00da94 	.word	0x2d00da94
     b2c:	00010a9e 	.word	0x00010a9e

00000b30 <init_efuse>:
     b30:	e59f0008 	ldr	r0, [pc, #8]	; b40 <init_efuse+0x10>
     b34:	e3a01000 	mov	r1, #0	; 0x0
     b38:	e3a02038 	mov	r2, #56	; 0x38
     b3c:	ea00021d 	b	13b8 <memset>
     b40:	0800d708 	.word	0x0800d708

00000b44 <assert_linkpc>:
     b44:	e3a00001 	mov	r0, #1	; 0x1
     b48:	e12fff1e 	bx	lr

00000b4c <pads_states>:
     b4c:	e3500000 	cmp	r0, #0	; 0x0
     b50:	e2803007 	add	r3, r0, #7	; 0x7
     b54:	a1a03000 	movge	r3, r0
     b58:	e1a0c000 	mov	ip, r0
     b5c:	e59f204c 	ldr	r2, [pc, #76]	; bb0 <pads_states+0x64>
     b60:	e59f004c 	ldr	r0, [pc, #76]	; bb4 <pads_states+0x68>
     b64:	e1a031c3 	asr	r3, r3, #3
     b68:	e3510000 	cmp	r1, #0	; 0x0
     b6c:	e1a03103 	lsl	r3, r3, #2
     b70:	e92d4010 	push	{r4, lr}
     b74:	e2833004 	add	r3, r3, #4	; 0x4
     b78:	11a04002 	movne	r4, r2
     b7c:	01a04000 	moveq	r4, r0
     b80:	e7931004 	ldr	r1, [r3, r4]
     b84:	e20c2007 	and	r2, ip, #7	; 0x7
     b88:	e3a03001 	mov	r3, #1	; 0x1
     b8c:	e0110213 	ands	r0, r1, r3, lsl r2
     b90:	08bd8010 	popeq	{r4, pc}
     b94:	e1a0318c 	lsl	r3, ip, #3
     b98:	e2833001 	add	r3, r3, #1	; 0x1
     b9c:	e7943183 	ldr	r3, [r4, r3, lsl #3]
     ba0:	e3530000 	cmp	r3, #0	; 0x0
     ba4:	13a00002 	movne	r0, #2	; 0x2
     ba8:	03a00001 	moveq	r0, #1	; 0x1
     bac:	e8bd8010 	pop	{r4, pc}
     bb0:	20f00000 	.word	0x20f00000
     bb4:	28f00000 	.word	0x28f00000

00000bb8 <pads_chmod>:
     bb8:	e3530000 	cmp	r3, #0	; 0x0
     bbc:	e59fc114 	ldr	ip, [pc, #276]	; cd8 <pads_chmod+0x120>
     bc0:	e59f3114 	ldr	r3, [pc, #276]	; cdc <pads_chmod+0x124>
     bc4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     bc8:	01a06003 	moveq	r6, r3
     bcc:	11a0600c 	movne	r6, ip
     bd0:	e59f3108 	ldr	r3, [pc, #264]	; ce0 <pads_chmod+0x128>
     bd4:	e59fc108 	ldr	ip, [pc, #264]	; ce4 <pads_chmod+0x12c>
     bd8:	e24dd00c 	sub	sp, sp, #12	; 0xc
     bdc:	01a0c003 	moveq	ip, r3
     be0:	e1b0a840 	asrs	sl, r0, #16
     be4:	e1a00800 	lsl	r0, r0, #16
     be8:	03a03000 	moveq	r3, #0	; 0x0
     bec:	13a03001 	movne	r3, #1	; 0x1
     bf0:	e1a00820 	lsr	r0, r0, #16
     bf4:	e15a0000 	cmp	sl, r0
     bf8:	a3a05000 	movge	r5, #0	; 0x0
     bfc:	b2035001 	andlt	r5, r3, #1	; 0x1
     c00:	e3550000 	cmp	r5, #0	; 0x0
     c04:	e1a07001 	mov	r7, r1
     c08:	e58d2004 	str	r2, [sp, #4]
     c0c:	1a00002f 	bne	cd0 <pads_chmod+0x118>
     c10:	e08c2300 	add	r2, ip, r0, lsl #6
     c14:	e35a0000 	cmp	sl, #0	; 0x0
     c18:	01a0a000 	moveq	sl, r0
     c1c:	e2828008 	add	r8, r2, #8	; 0x8
     c20:	e2829004 	add	r9, r2, #4	; 0x4
     c24:	e1a04000 	mov	r4, r0
     c28:	e2866004 	add	r6, r6, #4	; 0x4
     c2c:	e3a0b001 	mov	fp, #1	; 0x1
     c30:	ea000023 	b	cc4 <pads_chmod+0x10c>
     c34:	e3540000 	cmp	r4, #0	; 0x0
     c38:	a1a03004 	movge	r3, r4
     c3c:	e3570001 	cmp	r7, #1	; 0x1
     c40:	e1a0c1c3 	asr	ip, r3, #3
     c44:	e2045007 	and	r5, r4, #7	; 0x7
     c48:	0a000008 	beq	c70 <pads_chmod+0xb8>
     c4c:	e3570002 	cmp	r7, #2	; 0x2
     c50:	0a00000c 	beq	c88 <pads_chmod+0xd0>
     c54:	e2571000 	subs	r1, r7, #0	; 0x0
     c58:	e59f0088 	ldr	r0, [pc, #136]	; ce8 <pads_chmod+0x130>
     c5c:	1a000012 	bne	cac <pads_chmod+0xf4>
     c60:	e796310c 	ldr	r3, [r6, ip, lsl #2]
     c64:	e1c3351b 	bic	r3, r3, fp, lsl r5
     c68:	e786310c 	str	r3, [r6, ip, lsl #2]
     c6c:	ea000011 	b	cb8 <pads_chmod+0x100>
     c70:	e796310c 	ldr	r3, [r6, ip, lsl #2]
     c74:	e3a01000 	mov	r1, #0	; 0x0
     c78:	e1833517 	orr	r3, r3, r7, lsl r5
     c7c:	e786310c 	str	r3, [r6, ip, lsl #2]
     c80:	e5881000 	str	r1, [r8]
     c84:	ea00000b 	b	cb8 <pads_chmod+0x100>
     c88:	e796310c 	ldr	r3, [r6, ip, lsl #2]
     c8c:	e59d1004 	ldr	r1, [sp, #4]
     c90:	e183351b 	orr	r3, r3, fp, lsl r5
     c94:	e2512000 	subs	r2, r1, #0	; 0x0
     c98:	13a02001 	movne	r2, #1	; 0x1
     c9c:	e786310c 	str	r3, [r6, ip, lsl #2]
     ca0:	e588b000 	str	fp, [r8]
     ca4:	e5892000 	str	r2, [r9]
     ca8:	ea000002 	b	cb8 <pads_chmod+0x100>
     cac:	e59f3038 	ldr	r3, [pc, #56]	; cec <pads_chmod+0x134>
     cb0:	e1a0e00f 	mov	lr, pc
     cb4:	e593f000 	ldr	pc, [r3]
     cb8:	e2844001 	add	r4, r4, #1	; 0x1
     cbc:	e2888040 	add	r8, r8, #64	; 0x40
     cc0:	e2899040 	add	r9, r9, #64	; 0x40
     cc4:	e154000a 	cmp	r4, sl
     cc8:	e2843007 	add	r3, r4, #7	; 0x7
     ccc:	daffffd8 	ble	c34 <pads_chmod+0x7c>
     cd0:	e28dd00c 	add	sp, sp, #12	; 0xc
     cd4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     cd8:	2d009000 	.word	0x2d009000
     cdc:	2b009000 	.word	0x2b009000
     ce0:	28f00000 	.word	0x28f00000
     ce4:	20f00000 	.word	0x20f00000
     ce8:	00010aae 	.word	0x00010aae
     cec:	0800d704 	.word	0x0800d704

00000cf0 <pads_pull>:
     cf0:	e59f2068 	ldr	r2, [pc, #104]	; d60 <pads_pull+0x70>
     cf4:	e3530000 	cmp	r3, #0	; 0x0
     cf8:	e59fc064 	ldr	ip, [pc, #100]	; d64 <pads_pull+0x74>
     cfc:	11a0c002 	movne	ip, r2
     d00:	e1b02840 	asrs	r2, r0, #16
     d04:	e1a00800 	lsl	r0, r0, #16
     d08:	03a03000 	moveq	r3, #0	; 0x0
     d0c:	13a03001 	movne	r3, #1	; 0x1
     d10:	e1a00820 	lsr	r0, r0, #16
     d14:	e1520000 	cmp	r2, r0
     d18:	a3a03000 	movge	r3, #0	; 0x0
     d1c:	b2033001 	andlt	r3, r3, #1	; 0x1
     d20:	e3530000 	cmp	r3, #0	; 0x0
     d24:	112fff1e 	bxne	lr
     d28:	e08c3300 	add	r3, ip, r0, lsl #6
     d2c:	e3520000 	cmp	r2, #0	; 0x0
     d30:	01a02000 	moveq	r2, r0
     d34:	e283300c 	add	r3, r3, #12	; 0xc
     d38:	e3a0c001 	mov	ip, #1	; 0x1
     d3c:	ea000004 	b	d54 <pads_pull+0x64>
     d40:	e3510000 	cmp	r1, #0	; 0x0
     d44:	05831000 	streq	r1, [r3]
     d48:	1583c000 	strne	ip, [r3]
     d4c:	e2800001 	add	r0, r0, #1	; 0x1
     d50:	e2833040 	add	r3, r3, #64	; 0x40
     d54:	e1500002 	cmp	r0, r2
     d58:	dafffff8 	ble	d40 <pads_pull+0x50>
     d5c:	e12fff1e 	bx	lr
     d60:	20f00000 	.word	0x20f00000
     d64:	28f00000 	.word	0x28f00000

00000d68 <pads_setpin>:
     d68:	e59fc064 	ldr	ip, [pc, #100]	; dd4 <pads_setpin+0x6c>
     d6c:	e59f3064 	ldr	r3, [pc, #100]	; dd8 <pads_setpin+0x70>
     d70:	e3520000 	cmp	r2, #0	; 0x0
     d74:	11a02003 	movne	r2, r3
     d78:	01a0200c 	moveq	r2, ip
     d7c:	e1b0c840 	asrs	ip, r0, #16
     d80:	e1a00800 	lsl	r0, r0, #16
     d84:	03a03000 	moveq	r3, #0	; 0x0
     d88:	13a03001 	movne	r3, #1	; 0x1
     d8c:	e1a00820 	lsr	r0, r0, #16
     d90:	e15c0000 	cmp	ip, r0
     d94:	a3a03000 	movge	r3, #0	; 0x0
     d98:	b2033001 	andlt	r3, r3, #1	; 0x1
     d9c:	e3530000 	cmp	r3, #0	; 0x0
     da0:	112fff1e 	bxne	lr
     da4:	e0823300 	add	r3, r2, r0, lsl #6
     da8:	e35c0000 	cmp	ip, #0	; 0x0
     dac:	01a0c000 	moveq	ip, r0
     db0:	e2832004 	add	r2, r3, #4	; 0x4
     db4:	ea000002 	b	dc4 <pads_setpin+0x5c>
     db8:	e2513000 	subs	r3, r1, #0	; 0x0
     dbc:	13a03001 	movne	r3, #1	; 0x1
     dc0:	e4823040 	str	r3, [r2], #64
     dc4:	e150000c 	cmp	r0, ip
     dc8:	e2800001 	add	r0, r0, #1	; 0x1
     dcc:	dafffff9 	ble	db8 <pads_setpin+0x50>
     dd0:	e12fff1e 	bx	lr
     dd4:	28f00000 	.word	0x28f00000
     dd8:	20f00000 	.word	0x20f00000

00000ddc <pads_getpin>:
     ddc:	e59f201c 	ldr	r2, [pc, #28]	; e00 <pads_getpin+0x24>
     de0:	e59f301c 	ldr	r3, [pc, #28]	; e04 <pads_getpin+0x28>
     de4:	e3510000 	cmp	r1, #0	; 0x0
     de8:	01a03002 	moveq	r3, r2
     dec:	e6ff0070 	uxth	r0, r0
     df0:	e7930300 	ldr	r0, [r3, r0, lsl #6]
     df4:	e2500000 	subs	r0, r0, #0	; 0x0
     df8:	13a00001 	movne	r0, #1	; 0x1
     dfc:	e12fff1e 	bx	lr
     e00:	28f00000 	.word	0x28f00000
     e04:	20f00000 	.word	0x20f00000

00000e08 <init_alternate>:
     e08:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     e0c:	e59f0018 	ldr	r0, [pc, #24]	; e2c <init_alternate+0x24>
     e10:	e24dd004 	sub	sp, sp, #4	; 0x4
     e14:	ebfffe48 	bl	73c <module_enable>
     e18:	e59f0010 	ldr	r0, [pc, #16]	; e30 <init_alternate+0x28>
     e1c:	ebfffe46 	bl	73c <module_enable>
     e20:	e3a00000 	mov	r0, #0	; 0x0
     e24:	e28dd004 	add	sp, sp, #4	; 0x4
     e28:	e8bd8000 	pop	{pc}
     e2c:	2d013c00 	.word	0x2d013c00
     e30:	2b013c00 	.word	0x2b013c00

00000e34 <cpu_recase>:
     e34:	e92d4010 	push	{r4, lr}
     e38:	e59f109c 	ldr	r1, [pc, #156]	; edc <cpu_recase+0xa8>
     e3c:	e3a02004 	mov	r2, #4	; 0x4
     e40:	e1a04000 	mov	r4, r0
     e44:	eb00012a 	bl	12f4 <strncmp>
     e48:	e3500000 	cmp	r0, #0	; 0x0
     e4c:	1a000002 	bne	e5c <cpu_recase+0x28>
     e50:	eb0000bd 	bl	114c <get_mux>
     e54:	e3c00004 	bic	r0, r0, #4	; 0x4
     e58:	ea00000d 	b	e94 <cpu_recase+0x60>
     e5c:	e1a00004 	mov	r0, r4
     e60:	e59f1078 	ldr	r1, [pc, #120]	; ee0 <cpu_recase+0xac>
     e64:	e3a02005 	mov	r2, #5	; 0x5
     e68:	eb000121 	bl	12f4 <strncmp>
     e6c:	e3500000 	cmp	r0, #0	; 0x0
     e70:	0a000005 	beq	e8c <cpu_recase+0x58>
     e74:	e1a00004 	mov	r0, r4
     e78:	e59f1064 	ldr	r1, [pc, #100]	; ee4 <cpu_recase+0xb0>
     e7c:	e3a02004 	mov	r2, #4	; 0x4
     e80:	eb00011b 	bl	12f4 <strncmp>
     e84:	e3500000 	cmp	r0, #0	; 0x0
     e88:	18bd8010 	popne	{r4, pc}
     e8c:	eb0000ae 	bl	114c <get_mux>
     e90:	e3800004 	orr	r0, r0, #4	; 0x4
     e94:	e59f304c 	ldr	r3, [pc, #76]	; ee8 <cpu_recase+0xb4>
     e98:	e59f4048 	ldr	r4, [pc, #72]	; ee8 <cpu_recase+0xb4>
     e9c:	e59fc048 	ldr	ip, [pc, #72]	; eec <cpu_recase+0xb8>
     ea0:	e5830000 	str	r0, [r3]
     ea4:	e5942000 	ldr	r2, [r4]
     ea8:	e59c3000 	ldr	r3, [ip]
     eac:	e1520003 	cmp	r2, r3
     eb0:	08bd8010 	popeq	{r4, pc}
     eb4:	e1a00002 	mov	r0, r2
     eb8:	e3a01001 	mov	r1, #1	; 0x1
     ebc:	e58c2000 	str	r2, [ip]
     ec0:	eb00009d 	bl	113c <set_mux>
     ec4:	e5941000 	ldr	r1, [r4]
     ec8:	e59f0020 	ldr	r0, [pc, #32]	; ef0 <cpu_recase+0xbc>
     ecc:	e59f3020 	ldr	r3, [pc, #32]	; ef4 <cpu_recase+0xc0>
     ed0:	e1a0e00f 	mov	lr, pc
     ed4:	e593f000 	ldr	pc, [r3]
     ed8:	e8bd8010 	pop	{r4, pc}
     edc:	00010ac5 	.word	0x00010ac5
     ee0:	00010aca 	.word	0x00010aca
     ee4:	00010955 	.word	0x00010955
     ee8:	0800d160 	.word	0x0800d160
     eec:	0800d0b4 	.word	0x0800d0b4
     ef0:	00010ad0 	.word	0x00010ad0
     ef4:	0800d704 	.word	0x0800d704

00000ef8 <set_clk>:
     ef8:	e92d4030 	push	{r4, r5, lr}
     efc:	e3a00001 	mov	r0, #1	; 0x1
     f00:	e24dd004 	sub	sp, sp, #4	; 0x4
     f04:	e3a01e1e 	mov	r1, #480	; 0x1e0
     f08:	ebfffdc4 	bl	620 <set_pll>
     f0c:	e3a00002 	mov	r0, #2	; 0x2
     f10:	e3a01f96 	mov	r1, #600	; 0x258
     f14:	ebfffdc1 	bl	620 <set_pll>
     f18:	e3a00003 	mov	r0, #3	; 0x3
     f1c:	e3a01c03 	mov	r1, #768	; 0x300
     f20:	ebfffdbe 	bl	620 <set_pll>
     f24:	e59f00c0 	ldr	r0, [pc, #192]	; fec <set_clk+0xf4>
     f28:	e3a01001 	mov	r1, #1	; 0x1
     f2c:	e3a0200b 	mov	r2, #11	; 0xb
     f30:	ebfffdf6 	bl	710 <module_set_clock>
     f34:	e59f00b4 	ldr	r0, [pc, #180]	; ff0 <set_clk+0xf8>
     f38:	e3a01001 	mov	r1, #1	; 0x1
     f3c:	e3a02013 	mov	r2, #19	; 0x13
     f40:	ebfffdf2 	bl	710 <module_set_clock>
     f44:	e59fc0a8 	ldr	ip, [pc, #168]	; ff4 <set_clk+0xfc>
     f48:	e59f00a8 	ldr	r0, [pc, #168]	; ff8 <set_clk+0x100>
     f4c:	e59c3000 	ldr	r3, [ip]
     f50:	e3a01002 	mov	r1, #2	; 0x2
     f54:	e383307c 	orr	r3, r3, #124	; 0x7c
     f58:	e3a0200f 	mov	r2, #15	; 0xf
     f5c:	e58c3000 	str	r3, [ip]
     f60:	ebfffdea 	bl	710 <module_set_clock>
     f64:	e59f0090 	ldr	r0, [pc, #144]	; ffc <set_clk+0x104>
     f68:	e3a01002 	mov	r1, #2	; 0x2
     f6c:	e3a02019 	mov	r2, #25	; 0x19
     f70:	ebfffde6 	bl	710 <module_set_clock>
     f74:	e59f5084 	ldr	r5, [pc, #132]	; 1000 <set_clk+0x108>
     f78:	e3a04001 	mov	r4, #1	; 0x1
     f7c:	e3540005 	cmp	r4, #5	; 0x5
     f80:	e1a00005 	mov	r0, r5
     f84:	e3a01002 	mov	r1, #2	; 0x2
     f88:	e3a02003 	mov	r2, #3	; 0x3
     f8c:	0a000004 	beq	fa4 <set_clk+0xac>
     f90:	e1540001 	cmp	r4, r1
     f94:	059f0068 	ldreq	r0, [pc, #104]	; 1004 <set_clk+0x10c>
     f98:	01a01004 	moveq	r1, r4
     f9c:	02822002 	addeq	r2, r2, #2	; 0x2
     fa0:	ebfffdda 	bl	710 <module_set_clock>
     fa4:	e2844001 	add	r4, r4, #1	; 0x1
     fa8:	e3540007 	cmp	r4, #7	; 0x7
     fac:	e2855010 	add	r5, r5, #16	; 0x10
     fb0:	1afffff1 	bne	f7c <set_clk+0x84>
     fb4:	e59f004c 	ldr	r0, [pc, #76]	; 1008 <set_clk+0x110>
     fb8:	e3a01002 	mov	r1, #2	; 0x2
     fbc:	e3a0200f 	mov	r2, #15	; 0xf
     fc0:	ebfffdd2 	bl	710 <module_set_clock>
     fc4:	e59f0040 	ldr	r0, [pc, #64]	; 100c <set_clk+0x114>
     fc8:	e3a01002 	mov	r1, #2	; 0x2
     fcc:	e3a02005 	mov	r2, #5	; 0x5
     fd0:	ebfffdce 	bl	710 <module_set_clock>
     fd4:	e59f0034 	ldr	r0, [pc, #52]	; 1010 <set_clk+0x118>
     fd8:	e1a02004 	mov	r2, r4
     fdc:	e3a01002 	mov	r1, #2	; 0x2
     fe0:	e28dd004 	add	sp, sp, #4	; 0x4
     fe4:	e8bd4030 	pop	{r4, r5, lr}
     fe8:	eafffdc8 	b	710 <module_set_clock>
     fec:	2d00a250 	.word	0x2d00a250
     ff0:	2d00a2c0 	.word	0x2d00a2c0
     ff4:	2b00a000 	.word	0x2b00a000
     ff8:	2d00a240 	.word	0x2d00a240
     ffc:	2d00a290 	.word	0x2d00a290
    1000:	2d00a130 	.word	0x2d00a130
    1004:	2d00a140 	.word	0x2d00a140
    1008:	2d00a260 	.word	0x2d00a260
    100c:	2d00a2a0 	.word	0x2d00a2a0
    1010:	2d00a360 	.word	0x2d00a360

00001014 <chip_test>:
    1014:	e59f0044 	ldr	r0, [pc, #68]	; 1060 <end+0x4>
    1018:	e5901000 	ldr	r1, [r0]
    101c:	e59f2040 	ldr	r2, [pc, #64]	; 1064 <end+0x8>
    1020:	e1520001 	cmp	r2, r1
    1024:	1a00000c 	bne	105c <end>
    1028:	e5901004 	ldr	r1, [r0, #4]
    102c:	e59f2034 	ldr	r2, [pc, #52]	; 1068 <end+0xc>
    1030:	e1520001 	cmp	r2, r1
    1034:	1a000008 	bne	105c <end>
    1038:	e5901008 	ldr	r1, [r0, #8]
    103c:	e59f2028 	ldr	r2, [pc, #40]	; 106c <end+0x10>
    1040:	e1520001 	cmp	r2, r1
    1044:	1a000004 	bne	105c <end>
    1048:	e590100c 	ldr	r1, [r0, #12]
    104c:	e59f201c 	ldr	r2, [pc, #28]	; 1070 <end+0x14>
    1050:	e1520001 	cmp	r2, r1
    1054:	1a000000 	bne	105c <end>
    1058:	e59ff014 	ldr	pc, [pc, #20]	; 1074 <end+0x18>

0000105c <end>:
    105c:	e1a0f00e 	mov	pc, lr
    1060:	08000040 	.word	0x08000040
    1064:	6f666e69 	.word	0x6f666e69
    1068:	612d6d74 	.word	0x612d6d74
    106c:	2d6e696c 	.word	0x2d6e696c
    1070:	796c6e6f 	.word	0x796c6e6f
    1074:	08000200 	.word	0x08000200

00001078 <module_get_clock>:
    1078:	e59f3098 	ldr	r3, [pc, #152]	; 1118 <module_get_clock+0xa0>
    107c:	e1500003 	cmp	r0, r3
    1080:	0a00001c 	beq	10f8 <module_get_clock+0x80>
    1084:	8a00000a 	bhi	10b4 <module_get_clock+0x3c>
    1088:	e3500003 	cmp	r0, #3	; 0x3
    108c:	0a00001f 	beq	1110 <module_get_clock+0x98>
    1090:	8a000002 	bhi	10a0 <module_get_clock+0x28>
    1094:	e3500001 	cmp	r0, #1	; 0x1
    1098:	1a00001a 	bne	1108 <module_get_clock+0x90>
    109c:	ea00001b 	b	1110 <module_get_clock+0x98>
    10a0:	e59f3074 	ldr	r3, [pc, #116]	; 111c <module_get_clock+0xa4>
    10a4:	e1500003 	cmp	r0, r3
    10a8:	12833010 	addne	r3, r3, #16	; 0x10
    10ac:	1a000006 	bne	10cc <module_get_clock+0x54>
    10b0:	ea000010 	b	10f8 <module_get_clock+0x80>
    10b4:	e59f3064 	ldr	r3, [pc, #100]	; 1120 <module_get_clock+0xa8>
    10b8:	e1500003 	cmp	r0, r3
    10bc:	059f0060 	ldreq	r0, [pc, #96]	; 1124 <module_get_clock+0xac>
    10c0:	012fff1e 	bxeq	lr
    10c4:	8a000003 	bhi	10d8 <module_get_clock+0x60>
    10c8:	e2433010 	sub	r3, r3, #16	; 0x10
    10cc:	e1500003 	cmp	r0, r3
    10d0:	1a00000c 	bne	1108 <module_get_clock+0x90>
    10d4:	ea000007 	b	10f8 <module_get_clock+0x80>
    10d8:	e59f3048 	ldr	r3, [pc, #72]	; 1128 <module_get_clock+0xb0>
    10dc:	e1500003 	cmp	r0, r3
    10e0:	059f0044 	ldreq	r0, [pc, #68]	; 112c <module_get_clock+0xb4>
    10e4:	012fff1e 	bxeq	lr
    10e8:	e28330a0 	add	r3, r3, #160	; 0xa0
    10ec:	e1500003 	cmp	r0, r3
    10f0:	1a000004 	bne	1108 <module_get_clock+0x90>
    10f4:	ea000001 	b	1100 <module_get_clock+0x88>
    10f8:	e59f0030 	ldr	r0, [pc, #48]	; 1130 <module_get_clock+0xb8>
    10fc:	e12fff1e 	bx	lr
    1100:	e59f002c 	ldr	r0, [pc, #44]	; 1134 <module_get_clock+0xbc>
    1104:	e12fff1e 	bx	lr
    1108:	e3a00000 	mov	r0, #0	; 0x0
    110c:	e12fff1e 	bx	lr
    1110:	e59f0020 	ldr	r0, [pc, #32]	; 1138 <module_get_clock+0xc0>
    1114:	e12fff1e 	bx	lr
    1118:	2d00a260 	.word	0x2d00a260
    111c:	2d00a240 	.word	0x2d00a240
    1120:	2d00a2a0 	.word	0x2d00a2a0
    1124:	05e69ec0 	.word	0x05e69ec0
    1128:	2d00a2c0 	.word	0x2d00a2c0
    112c:	02dc6c00 	.word	0x02dc6c00
    1130:	016e3600 	.word	0x016e3600
    1134:	00e4e1c0 	.word	0x00e4e1c0
    1138:	01312d00 	.word	0x01312d00

0000113c <set_mux>:
    113c:	e59f3004 	ldr	r3, [pc, #4]	; 1148 <set_mux+0xc>
    1140:	e5830000 	str	r0, [r3]
    1144:	e12fff1e 	bx	lr
    1148:	2d009000 	.word	0x2d009000

0000114c <get_mux>:
    114c:	e59f3004 	ldr	r3, [pc, #4]	; 1158 <get_mux+0xc>
    1150:	e5930000 	ldr	r0, [r3]
    1154:	e12fff1e 	bx	lr
    1158:	2d009000 	.word	0x2d009000

0000115c <efuse_inject>:
    115c:	e92d4010 	push	{r4, lr}
    1160:	e24dd030 	sub	sp, sp, #48	; 0x30
    1164:	e59f109c 	ldr	r1, [pc, #156]	; 1208 <efuse_inject+0xac>
    1168:	e3a02020 	mov	r2, #32	; 0x20
    116c:	e1a0000d 	mov	r0, sp
    1170:	eb000098 	bl	13d8 <memcpy>
    1174:	e3a01000 	mov	r1, #0	; 0x0
    1178:	e3a02010 	mov	r2, #16	; 0x10
    117c:	e28d0020 	add	r0, sp, #32	; 0x20
    1180:	eb00008c 	bl	13b8 <memset>
    1184:	e3a0302e 	mov	r3, #46	; 0x2e
    1188:	e5cd3020 	strb	r3, [sp, #32]
    118c:	e59f1078 	ldr	r1, [pc, #120]	; 120c <efuse_inject+0xb0>
    1190:	e3a03016 	mov	r3, #22	; 0x16
    1194:	e5cd3021 	strb	r3, [sp, #33]
    1198:	e1a0400d 	mov	r4, sp
    119c:	e3a02000 	mov	r2, #0	; 0x0
    11a0:	e7d43002 	ldrb	r3, [r4, r2]
    11a4:	e2822001 	add	r2, r2, #1	; 0x1
    11a8:	e3520020 	cmp	r2, #32	; 0x20
    11ac:	e4813004 	str	r3, [r1], #4
    11b0:	1afffffa 	bne	11a0 <efuse_inject+0x44>
    11b4:	e59f1054 	ldr	r1, [pc, #84]	; 1210 <efuse_inject+0xb4>
    11b8:	e3a02000 	mov	r2, #0	; 0x0
    11bc:	e28d0020 	add	r0, sp, #32	; 0x20
    11c0:	e7d03002 	ldrb	r3, [r0, r2]
    11c4:	e2822001 	add	r2, r2, #1	; 0x1
    11c8:	e3520010 	cmp	r2, #16	; 0x10
    11cc:	e4813004 	str	r3, [r1], #4
    11d0:	1afffffa 	bne	11c0 <efuse_inject+0x64>
    11d4:	e59f3030 	ldr	r3, [pc, #48]	; 120c <efuse_inject+0xb0>
    11d8:	e3a02018 	mov	r2, #24	; 0x18
    11dc:	e5832100 	str	r2, [r3, #256]
    11e0:	e3a02001 	mov	r2, #1	; 0x1
    11e4:	e2433a01 	sub	r3, r3, #4096	; 0x1000
    11e8:	e5832000 	str	r2, [r3]
    11ec:	e3a04000 	mov	r4, #0	; 0x0
    11f0:	e2433a03 	sub	r3, r3, #12288	; 0x3000
    11f4:	e5834c34 	str	r4, [r3, #3124]
    11f8:	ebfffe4c 	bl	b30 <init_efuse>
    11fc:	e1a00004 	mov	r0, r4
    1200:	e28dd030 	add	sp, sp, #48	; 0x30
    1204:	e8bd8010 	pop	{r4, pc}
    1208:	000107bc 	.word	0x000107bc
    120c:	2d00e000 	.word	0x2d00e000
    1210:	2d00e080 	.word	0x2d00e080

00001214 <strcpy>:
    1214:	e3a02000 	mov	r2, #0	; 0x0
    1218:	e7d13002 	ldrb	r3, [r1, r2]
    121c:	e3530000 	cmp	r3, #0	; 0x0
    1220:	e7c03002 	strb	r3, [r0, r2]
    1224:	e2822001 	add	r2, r2, #1	; 0x1
    1228:	1afffffa 	bne	1218 <strcpy+0x4>
    122c:	e12fff1e 	bx	lr

00001230 <strncpy>:
    1230:	e3a0c000 	mov	ip, #0	; 0x0
    1234:	e3520000 	cmp	r2, #0	; 0x0
    1238:	e2422001 	sub	r2, r2, #1	; 0x1
    123c:	012fff1e 	bxeq	lr
    1240:	e7d1300c 	ldrb	r3, [r1, ip]
    1244:	e3530000 	cmp	r3, #0	; 0x0
    1248:	e7c0300c 	strb	r3, [r0, ip]
    124c:	e28cc001 	add	ip, ip, #1	; 0x1
    1250:	1afffff7 	bne	1234 <strncpy+0x4>
    1254:	e12fff1e 	bx	lr

00001258 <strncat>:
    1258:	e92d4030 	push	{r4, r5, lr}
    125c:	e2525000 	subs	r5, r2, #0	; 0x0
    1260:	e1a04001 	mov	r4, r1
    1264:	11a0c000 	movne	ip, r0
    1268:	1a000001 	bne	1274 <strncat+0x1c>
    126c:	e8bd8030 	pop	{r4, r5, pc}
    1270:	e28cc001 	add	ip, ip, #1	; 0x1
    1274:	e5dc3000 	ldrb	r3, [ip]
    1278:	e3530000 	cmp	r3, #0	; 0x0
    127c:	1afffffb 	bne	1270 <strncat+0x18>
    1280:	ea000004 	b	1298 <strncat+0x40>
    1284:	e1550002 	cmp	r5, r2
    1288:	1a000004 	bne	12a0 <strncat+0x48>
    128c:	e3a03000 	mov	r3, #0	; 0x0
    1290:	e5cc3000 	strb	r3, [ip]
    1294:	e8bd8030 	pop	{r4, r5, pc}
    1298:	e1a02003 	mov	r2, r3
    129c:	e1a0100c 	mov	r1, ip
    12a0:	e7d43002 	ldrb	r3, [r4, r2]
    12a4:	e28cc001 	add	ip, ip, #1	; 0x1
    12a8:	e3530000 	cmp	r3, #0	; 0x0
    12ac:	e7c13002 	strb	r3, [r1, r2]
    12b0:	e2822001 	add	r2, r2, #1	; 0x1
    12b4:	1afffff2 	bne	1284 <strncat+0x2c>
    12b8:	e8bd8030 	pop	{r4, r5, pc}

000012bc <strcmp>:
    12bc:	e92d4010 	push	{r4, lr}
    12c0:	e3a02000 	mov	r2, #0	; 0x0
    12c4:	e1a04000 	mov	r4, r0
    12c8:	e7d4c002 	ldrb	ip, [r4, r2]
    12cc:	e7d13002 	ldrb	r3, [r1, r2]
    12d0:	e2822001 	add	r2, r2, #1	; 0x1
    12d4:	e063300c 	rsb	r3, r3, ip
    12d8:	e6ef0073 	uxtb	r0, r3
    12dc:	e3500000 	cmp	r0, #0	; 0x0
    12e0:	1a000001 	bne	12ec <strcmp+0x30>
    12e4:	e35c0000 	cmp	ip, #0	; 0x0
    12e8:	1afffff6 	bne	12c8 <strcmp+0xc>
    12ec:	e6af0070 	sxtb	r0, r0
    12f0:	e8bd8010 	pop	{r4, pc}

000012f4 <strncmp>:
    12f4:	e92d4030 	push	{r4, r5, lr}
    12f8:	e3a0c000 	mov	ip, #0	; 0x0
    12fc:	e1a05000 	mov	r5, r0
    1300:	e1a04001 	mov	r4, r1
    1304:	ea000009 	b	1330 <strncmp+0x3c>
    1308:	e7d5100c 	ldrb	r1, [r5, ip]
    130c:	e7d4300c 	ldrb	r3, [r4, ip]
    1310:	e2422001 	sub	r2, r2, #1	; 0x1
    1314:	e0633001 	rsb	r3, r3, r1
    1318:	e6ef0073 	uxtb	r0, r3
    131c:	e3500000 	cmp	r0, #0	; 0x0
    1320:	e28cc001 	add	ip, ip, #1	; 0x1
    1324:	1a000004 	bne	133c <strncmp+0x48>
    1328:	e3510000 	cmp	r1, #0	; 0x0
    132c:	0a000002 	beq	133c <strncmp+0x48>
    1330:	e3520000 	cmp	r2, #0	; 0x0
    1334:	1afffff3 	bne	1308 <strncmp+0x14>
    1338:	e1a00002 	mov	r0, r2
    133c:	e6af0070 	sxtb	r0, r0
    1340:	e8bd8030 	pop	{r4, r5, pc}

00001344 <strchr>:
    1344:	e1a03000 	mov	r3, r0
    1348:	e6ef1071 	uxtb	r1, r1
    134c:	ea000002 	b	135c <strchr+0x18>
    1350:	e3500000 	cmp	r0, #0	; 0x0
    1354:	012fff1e 	bxeq	lr
    1358:	e2833001 	add	r3, r3, #1	; 0x1
    135c:	e5d30000 	ldrb	r0, [r3]
    1360:	e1500001 	cmp	r0, r1
    1364:	1afffff9 	bne	1350 <strchr+0xc>
    1368:	e1a00003 	mov	r0, r3
    136c:	e12fff1e 	bx	lr

00001370 <strlen>:
    1370:	e1a02000 	mov	r2, r0
    1374:	ea000000 	b	137c <strlen+0xc>
    1378:	e2800001 	add	r0, r0, #1	; 0x1
    137c:	e5d03000 	ldrb	r3, [r0]
    1380:	e3530000 	cmp	r3, #0	; 0x0
    1384:	1afffffb 	bne	1378 <strlen+0x8>
    1388:	e0620000 	rsb	r0, r2, r0
    138c:	e12fff1e 	bx	lr

00001390 <strnlen>:
    1390:	e3a02000 	mov	r2, #0	; 0x0
    1394:	e1510002 	cmp	r1, r2
    1398:	e080c002 	add	ip, r0, r2
    139c:	0a000003 	beq	13b0 <strnlen+0x20>
    13a0:	e7d03002 	ldrb	r3, [r0, r2]
    13a4:	e2822001 	add	r2, r2, #1	; 0x1
    13a8:	e3530000 	cmp	r3, #0	; 0x0
    13ac:	1afffff8 	bne	1394 <strnlen+0x4>
    13b0:	e060000c 	rsb	r0, r0, ip
    13b4:	e12fff1e 	bx	lr

000013b8 <memset>:
    13b8:	e3a03000 	mov	r3, #0	; 0x0
    13bc:	ea000001 	b	13c8 <memset+0x10>
    13c0:	e7c01003 	strb	r1, [r0, r3]
    13c4:	e2833001 	add	r3, r3, #1	; 0x1
    13c8:	e3520000 	cmp	r2, #0	; 0x0
    13cc:	e2422001 	sub	r2, r2, #1	; 0x1
    13d0:	1afffffa 	bne	13c0 <memset+0x8>
    13d4:	e12fff1e 	bx	lr

000013d8 <memcpy>:
    13d8:	e3a0c000 	mov	ip, #0	; 0x0
    13dc:	ea000002 	b	13ec <memcpy+0x14>
    13e0:	e7d1300c 	ldrb	r3, [r1, ip]
    13e4:	e7c0300c 	strb	r3, [r0, ip]
    13e8:	e28cc001 	add	ip, ip, #1	; 0x1
    13ec:	e3520000 	cmp	r2, #0	; 0x0
    13f0:	e2422001 	sub	r2, r2, #1	; 0x1
    13f4:	1afffff9 	bne	13e0 <memcpy+0x8>
    13f8:	e12fff1e 	bx	lr

000013fc <memmove>:
    13fc:	e1500001 	cmp	r0, r1
    1400:	e92d4010 	push	{r4, lr}
    1404:	08bd8010 	popeq	{r4, pc}
    1408:	93a0c000 	movls	ip, #0	; 0x0
    140c:	9a000003 	bls	1420 <memmove+0x24>
    1410:	ea000006 	b	1430 <memmove+0x34>
    1414:	e7d1300c 	ldrb	r3, [r1, ip]
    1418:	e7c0300c 	strb	r3, [r0, ip]
    141c:	e28cc001 	add	ip, ip, #1	; 0x1
    1420:	e3520000 	cmp	r2, #0	; 0x0
    1424:	e2422001 	sub	r2, r2, #1	; 0x1
    1428:	1afffff9 	bne	1414 <memmove+0x18>
    142c:	e8bd8010 	pop	{r4, pc}
    1430:	e0814002 	add	r4, r1, r2
    1434:	e3a0c000 	mov	ip, #0	; 0x0
    1438:	e0801002 	add	r1, r0, r2
    143c:	ea000001 	b	1448 <memmove+0x4c>
    1440:	e7d4300c 	ldrb	r3, [r4, ip]
    1444:	e7c1300c 	strb	r3, [r1, ip]
    1448:	e3520000 	cmp	r2, #0	; 0x0
    144c:	e24cc001 	sub	ip, ip, #1	; 0x1
    1450:	e2422001 	sub	r2, r2, #1	; 0x1
    1454:	1afffff9 	bne	1440 <memmove+0x44>
    1458:	e8bd8010 	pop	{r4, pc}

0000145c <memcmp>:
    145c:	e92d4030 	push	{r4, r5, lr}
    1460:	e3a0c000 	mov	ip, #0	; 0x0
    1464:	e1a04001 	mov	r4, r1
    1468:	e1a05000 	mov	r5, r0
    146c:	e1a01002 	mov	r1, r2
    1470:	ea000005 	b	148c <memcmp+0x30>
    1474:	e7d5200c 	ldrb	r2, [r5, ip]
    1478:	e7d4300c 	ldrb	r3, [r4, ip]
    147c:	e28cc001 	add	ip, ip, #1	; 0x1
    1480:	e0520003 	subs	r0, r2, r3
    1484:	18bd8030 	popne	{r4, r5, pc}
    1488:	e2411001 	sub	r1, r1, #1	; 0x1
    148c:	e3510000 	cmp	r1, #0	; 0x0
    1490:	1afffff7 	bne	1474 <memcmp+0x18>
    1494:	e1a00001 	mov	r0, r1
    1498:	e8bd8030 	pop	{r4, r5, pc}

0000149c <so_m_isused>:
    149c:	e5903004 	ldr	r3, [r0, #4]
    14a0:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    14a4:	03a00000 	moveq	r0, #0	; 0x0
    14a8:	13a00001 	movne	r0, #1	; 0x1
    14ac:	e12fff1e 	bx	lr

000014b0 <so_m_set_used>:
    14b0:	e5903004 	ldr	r3, [r0, #4]
    14b4:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    14b8:	e5803004 	str	r3, [r0, #4]
    14bc:	e12fff1e 	bx	lr

000014c0 <so_m_unset_used>:
    14c0:	e5903004 	ldr	r3, [r0, #4]
    14c4:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    14c8:	e5803004 	str	r3, [r0, #4]
    14cc:	e12fff1e 	bx	lr

000014d0 <so_m_get_size>:
    14d0:	e5900004 	ldr	r0, [r0, #4]
    14d4:	e3c00103 	bic	r0, r0, #-1073741824	; 0xc0000000
    14d8:	e12fff1e 	bx	lr

000014dc <so_m_set_size>:
    14dc:	e5903004 	ldr	r3, [r0, #4]
    14e0:	e3c11103 	bic	r1, r1, #-1073741824	; 0xc0000000
    14e4:	e2033103 	and	r3, r3, #-1073741824	; 0xc0000000
    14e8:	e1811003 	orr	r1, r1, r3
    14ec:	e5801004 	str	r1, [r0, #4]
    14f0:	e12fff1e 	bx	lr

000014f4 <so_m_alloc>:
    14f4:	e59f3048 	ldr	r3, [pc, #72]	; 1544 <so_m_alloc+0x50>
    14f8:	e3a02000 	mov	r2, #0	; 0x0
    14fc:	e593c000 	ldr	ip, [r3]
    1500:	e5931008 	ldr	r1, [r3, #8]
    1504:	ea000007 	b	1528 <so_m_alloc+0x34>
    1508:	e5903004 	ldr	r3, [r0, #4]
    150c:	e3530000 	cmp	r3, #0	; 0x0
    1510:	1a000004 	bne	1528 <so_m_alloc+0x34>
    1514:	e59f2028 	ldr	r2, [pc, #40]	; 1544 <so_m_alloc+0x50>
    1518:	e5923004 	ldr	r3, [r2, #4]
    151c:	e2433001 	sub	r3, r3, #1	; 0x1
    1520:	e5823004 	str	r3, [r2, #4]
    1524:	e12fff1e 	bx	lr
    1528:	e152000c 	cmp	r2, ip
    152c:	e1a00001 	mov	r0, r1
    1530:	e2822001 	add	r2, r2, #1	; 0x1
    1534:	e2811010 	add	r1, r1, #16	; 0x10
    1538:	3afffff2 	bcc	1508 <so_m_alloc+0x14>
    153c:	e3a00000 	mov	r0, #0	; 0x0
    1540:	e12fff1e 	bx	lr
    1544:	0800d744 	.word	0x0800d744

00001548 <so_m_remove>:
    1548:	e3a03000 	mov	r3, #0	; 0x0
    154c:	e59f2014 	ldr	r2, [pc, #20]	; 1568 <so_m_remove+0x20>
    1550:	e5803000 	str	r3, [r0]
    1554:	e5803004 	str	r3, [r0, #4]
    1558:	e5923004 	ldr	r3, [r2, #4]
    155c:	e2833001 	add	r3, r3, #1	; 0x1
    1560:	e5823004 	str	r3, [r2, #4]
    1564:	e12fff1e 	bx	lr
    1568:	0800d744 	.word	0x0800d744

0000156c <malloc>:
    156c:	e3500000 	cmp	r0, #0	; 0x0
    1570:	e92d4070 	push	{r4, r5, r6, lr}
    1574:	0a000047 	beq	1698 <malloc+0x12c>
    1578:	e59f1120 	ldr	r1, [pc, #288]	; 16a0 <malloc+0x134>
    157c:	e280301f 	add	r3, r0, #31	; 0x1f
    1580:	e5b12014 	ldr	r2, [r1, #20]!
    1584:	e3c3401f 	bic	r4, r3, #31	; 0x1f
    1588:	e2425008 	sub	r5, r2, #8	; 0x8
    158c:	ea000009 	b	15b8 <malloc+0x4c>
    1590:	e5953004 	ldr	r3, [r5, #4]
    1594:	e2130103 	ands	r0, r3, #-1073741824	; 0xc0000000
    1598:	e3c32103 	bic	r2, r3, #-1073741824	; 0xc0000000
    159c:	1a000003 	bne	15b0 <malloc+0x44>
    15a0:	e1520004 	cmp	r2, r4
    15a4:	3a000001 	bcc	15b0 <malloc+0x44>
    15a8:	9a000030 	bls	1670 <malloc+0x104>
    15ac:	ea000005 	b	15c8 <malloc+0x5c>
    15b0:	e5953008 	ldr	r3, [r5, #8]
    15b4:	e2435008 	sub	r5, r3, #8	; 0x8
    15b8:	e2853008 	add	r3, r5, #8	; 0x8
    15bc:	e1530001 	cmp	r3, r1
    15c0:	1afffff2 	bne	1590 <malloc+0x24>
    15c4:	ea000033 	b	1698 <malloc+0x12c>
    15c8:	e59f30d0 	ldr	r3, [pc, #208]	; 16a0 <malloc+0x134>
    15cc:	e1a01000 	mov	r1, r0
    15d0:	e5932008 	ldr	r2, [r3, #8]
    15d4:	e5930000 	ldr	r0, [r3]
    15d8:	ea00000a 	b	1608 <malloc+0x9c>
    15dc:	e5923004 	ldr	r3, [r2, #4]
    15e0:	e2822010 	add	r2, r2, #16	; 0x10
    15e4:	e3530000 	cmp	r3, #0	; 0x0
    15e8:	1a000006 	bne	1608 <malloc+0x9c>
    15ec:	e59f20ac 	ldr	r2, [pc, #172]	; 16a0 <malloc+0x134>
    15f0:	e3560000 	cmp	r6, #0	; 0x0
    15f4:	e5923004 	ldr	r3, [r2, #4]
    15f8:	e2433001 	sub	r3, r3, #1	; 0x1
    15fc:	e5823004 	str	r3, [r2, #4]
    1600:	1a000005 	bne	161c <malloc+0xb0>
    1604:	ea000023 	b	1698 <malloc+0x12c>
    1608:	e1510000 	cmp	r1, r0
    160c:	e1a06002 	mov	r6, r2
    1610:	e2811001 	add	r1, r1, #1	; 0x1
    1614:	3afffff0 	bcc	15dc <malloc+0x70>
    1618:	ea00001e 	b	1698 <malloc+0x12c>
    161c:	e5953004 	ldr	r3, [r5, #4]
    1620:	e5962004 	ldr	r2, [r6, #4]
    1624:	e0643003 	rsb	r3, r4, r3
    1628:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    162c:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1630:	e1833002 	orr	r3, r3, r2
    1634:	e5863004 	str	r3, [r6, #4]
    1638:	e5951000 	ldr	r1, [r5]
    163c:	e5950004 	ldr	r0, [r5, #4]
    1640:	e1a0c005 	mov	ip, r5
    1644:	e1a03006 	mov	r3, r6
    1648:	e0841001 	add	r1, r4, r1
    164c:	e3c42103 	bic	r2, r4, #-1073741824	; 0xc0000000
    1650:	e2000103 	and	r0, r0, #-1073741824	; 0xc0000000
    1654:	e5bc4008 	ldr	r4, [ip, #8]!
    1658:	e4831008 	str	r1, [r3], #8
    165c:	e1822000 	orr	r2, r2, r0
    1660:	e5864008 	str	r4, [r6, #8]
    1664:	e5843004 	str	r3, [r4, #4]
    1668:	e985000c 	stmib	r5, {r2, r3}
    166c:	e586c00c 	str	ip, [r6, #12]
    1670:	e5953004 	ldr	r3, [r5, #4]
    1674:	e59f1024 	ldr	r1, [pc, #36]	; 16a0 <malloc+0x134>
    1678:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    167c:	e5853004 	str	r3, [r5, #4]
    1680:	e5912010 	ldr	r2, [r1, #16]
    1684:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1688:	e0833002 	add	r3, r3, r2
    168c:	e5813010 	str	r3, [r1, #16]
    1690:	e5950000 	ldr	r0, [r5]
    1694:	e8bd8070 	pop	{r4, r5, r6, pc}
    1698:	e3a00000 	mov	r0, #0	; 0x0
    169c:	e8bd8070 	pop	{r4, r5, r6, pc}
    16a0:	0800d744 	.word	0x0800d744

000016a4 <__sOMERGe>:
    16a4:	e5902004 	ldr	r2, [r0, #4]
    16a8:	e92d4030 	push	{r4, r5, lr}
    16ac:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    16b0:	e1a05000 	mov	r5, r0
    16b4:	e1a0c001 	mov	ip, r1
    16b8:	18bd8030 	popne	{r4, r5, pc}
    16bc:	e5913004 	ldr	r3, [r1, #4]
    16c0:	e2134103 	ands	r4, r3, #-1073741824	; 0xc0000000
    16c4:	18bd8030 	popne	{r4, r5, pc}
    16c8:	e0823003 	add	r3, r2, r3
    16cc:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    16d0:	e59f0030 	ldr	r0, [pc, #48]	; 1708 <__sOMERGe+0x64>
    16d4:	e5853004 	str	r3, [r5, #4]
    16d8:	e5814000 	str	r4, [r1]
    16dc:	e5814004 	str	r4, [r1, #4]
    16e0:	e5903004 	ldr	r3, [r0, #4]
    16e4:	e591100c 	ldr	r1, [r1, #12]
    16e8:	e59c2008 	ldr	r2, [ip, #8]
    16ec:	e2833001 	add	r3, r3, #1	; 0x1
    16f0:	e5812000 	str	r2, [r1]
    16f4:	e5821004 	str	r1, [r2, #4]
    16f8:	e5803004 	str	r3, [r0, #4]
    16fc:	e58c4008 	str	r4, [ip, #8]
    1700:	e58c400c 	str	r4, [ip, #12]
    1704:	e8bd8030 	pop	{r4, r5, pc}
    1708:	0800d744 	.word	0x0800d744

0000170c <free>:
    170c:	e3500000 	cmp	r0, #0	; 0x0
    1710:	159f2090 	ldrne	r2, [pc, #144]	; 17a8 <free+0x9c>
    1714:	e92d4010 	push	{r4, lr}
    1718:	15b23014 	ldrne	r3, [r2, #20]!
    171c:	1a000004 	bne	1734 <free+0x28>
    1720:	e8bd8010 	pop	{r4, pc}
    1724:	e5943000 	ldr	r3, [r4]
    1728:	e1500003 	cmp	r0, r3
    172c:	0a000005 	beq	1748 <free+0x3c>
    1730:	e5943008 	ldr	r3, [r4, #8]
    1734:	e2434008 	sub	r4, r3, #8	; 0x8
    1738:	e2843008 	add	r3, r4, #8	; 0x8
    173c:	e1530002 	cmp	r3, r2
    1740:	1afffff7 	bne	1724 <free+0x18>
    1744:	e8bd8010 	pop	{r4, pc}
    1748:	e5943004 	ldr	r3, [r4, #4]
    174c:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    1750:	08bd8010 	popeq	{r4, pc}
    1754:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1758:	e59f1048 	ldr	r1, [pc, #72]	; 17a8 <free+0x9c>
    175c:	e5843004 	str	r3, [r4, #4]
    1760:	e5912010 	ldr	r2, [r1, #16]
    1764:	e5940008 	ldr	r0, [r4, #8]
    1768:	e0632002 	rsb	r2, r3, r2
    176c:	e59f3038 	ldr	r3, [pc, #56]	; 17ac <free+0xa0>
    1770:	e5812010 	str	r2, [r1, #16]
    1774:	e1500003 	cmp	r0, r3
    1778:	0a000002 	beq	1788 <free+0x7c>
    177c:	e2401008 	sub	r1, r0, #8	; 0x8
    1780:	e1a00004 	mov	r0, r4
    1784:	ebffffc6 	bl	16a4 <__sOMERGe>
    1788:	e594000c 	ldr	r0, [r4, #12]
    178c:	e59f3018 	ldr	r3, [pc, #24]	; 17ac <free+0xa0>
    1790:	e1500003 	cmp	r0, r3
    1794:	08bd8010 	popeq	{r4, pc}
    1798:	e2400008 	sub	r0, r0, #8	; 0x8
    179c:	e1a01004 	mov	r1, r4
    17a0:	e8bd4010 	pop	{r4, lr}
    17a4:	eaffffbe 	b	16a4 <__sOMERGe>
    17a8:	0800d744 	.word	0x0800d744
    17ac:	0800d758 	.word	0x0800d758

000017b0 <__sOINFo>:
    17b0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    17b4:	e59f50a8 	ldr	r5, [pc, #168]	; 1864 <__sOINFo+0xb4>
    17b8:	e24dd004 	sub	sp, sp, #4	; 0x4
    17bc:	e59f60a4 	ldr	r6, [pc, #164]	; 1868 <__sOINFo+0xb8>
    17c0:	e8950006 	ldm	r5, {r1, r2}
    17c4:	e59f00a0 	ldr	r0, [pc, #160]	; 186c <__sOINFo+0xbc>
    17c8:	e1a0e00f 	mov	lr, pc
    17cc:	e596f000 	ldr	pc, [r6]
    17d0:	e2853008 	add	r3, r5, #8	; 0x8
    17d4:	e8930098 	ldm	r3, {r3, r4, r7}
    17d8:	e2444b01 	sub	r4, r4, #1024	; 0x400
    17dc:	e0634004 	rsb	r4, r3, r4
    17e0:	e3a00064 	mov	r0, #100	; 0x64
    17e4:	e1a01004 	mov	r1, r4
    17e8:	e0000790 	mul	r0, r0, r7
    17ec:	eb003639 	bl	f0d8 <__aeabi_uidiv>
    17f0:	e1a01004 	mov	r1, r4
    17f4:	e1a03000 	mov	r3, r0
    17f8:	e1a02007 	mov	r2, r7
    17fc:	e59f006c 	ldr	r0, [pc, #108]	; 1870 <__sOINFo+0xc0>
    1800:	e1a0e00f 	mov	lr, pc
    1804:	e596f000 	ldr	pc, [r6]
    1808:	e59f0064 	ldr	r0, [pc, #100]	; 1874 <__sOINFo+0xc4>
    180c:	e1a0e00f 	mov	lr, pc
    1810:	e596f000 	ldr	pc, [r6]
    1814:	e59f005c 	ldr	r0, [pc, #92]	; 1878 <__sOINFo+0xc8>
    1818:	e1a0e00f 	mov	lr, pc
    181c:	e596f000 	ldr	pc, [r6]
    1820:	e5b53014 	ldr	r3, [r5, #20]!
    1824:	ea000007 	b	1848 <__sOINFo+0x98>
    1828:	e8940006 	ldm	r4, {r1, r2}
    182c:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    1830:	03a03000 	moveq	r3, #0	; 0x0
    1834:	13a03001 	movne	r3, #1	; 0x1
    1838:	e3c22103 	bic	r2, r2, #-1073741824	; 0xc0000000
    183c:	e1a0e00f 	mov	lr, pc
    1840:	e596f000 	ldr	pc, [r6]
    1844:	e5943008 	ldr	r3, [r4, #8]
    1848:	e2434008 	sub	r4, r3, #8	; 0x8
    184c:	e2843008 	add	r3, r4, #8	; 0x8
    1850:	e1530005 	cmp	r3, r5
    1854:	e59f0020 	ldr	r0, [pc, #32]	; 187c <__sOINFo+0xcc>
    1858:	1afffff2 	bne	1828 <__sOINFo+0x78>
    185c:	e28dd004 	add	sp, sp, #4	; 0x4
    1860:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1864:	0800d744 	.word	0x0800d744
    1868:	0800d704 	.word	0x0800d704
    186c:	00010ae6 	.word	0x00010ae6
    1870:	00010b00 	.word	0x00010b00
    1874:	00010b25 	.word	0x00010b25
    1878:	000109d3 	.word	0x000109d3
    187c:	00010b36 	.word	0x00010b36

00001880 <init_malloc>:
    1880:	e59f20e4 	ldr	r2, [pc, #228]	; 196c <init_malloc+0xec>
    1884:	e59f30e4 	ldr	r3, [pc, #228]	; 1970 <init_malloc+0xf0>
    1888:	e3c2001f 	bic	r0, r2, #31	; 0x1f
    188c:	e3c3301f 	bic	r3, r3, #31	; 0x1f
    1890:	e0602003 	rsb	r2, r0, r3
    1894:	e92d4010 	push	{r4, lr}
    1898:	e59f40d4 	ldr	r4, [pc, #212]	; 1974 <init_malloc+0xf4>
    189c:	e3520b01 	cmp	r2, #1024	; 0x400
    18a0:	e5840008 	str	r0, [r4, #8]
    18a4:	e584300c 	str	r3, [r4, #12]
    18a8:	93e00000 	mvnls	r0, #0	; 0x0
    18ac:	98bd8010 	popls	{r4, pc}
    18b0:	e284c014 	add	ip, r4, #20	; 0x14
    18b4:	e3a03040 	mov	r3, #64	; 0x40
    18b8:	e3a01000 	mov	r1, #0	; 0x0
    18bc:	e3a02b01 	mov	r2, #1024	; 0x400
    18c0:	e584c018 	str	ip, [r4, #24]
    18c4:	e584c014 	str	ip, [r4, #20]
    18c8:	e5843004 	str	r3, [r4, #4]
    18cc:	e5843000 	str	r3, [r4]
    18d0:	ebfffeb8 	bl	13b8 <memset>
    18d4:	e5940008 	ldr	r0, [r4, #8]
    18d8:	e594c000 	ldr	ip, [r4]
    18dc:	e1a01000 	mov	r1, r0
    18e0:	e3a02000 	mov	r2, #0	; 0x0
    18e4:	ea000006 	b	1904 <init_malloc+0x84>
    18e8:	e59e3004 	ldr	r3, [lr, #4]
    18ec:	e3530000 	cmp	r3, #0	; 0x0
    18f0:	059f207c 	ldreq	r2, [pc, #124]	; 1974 <init_malloc+0xf4>
    18f4:	05923004 	ldreq	r3, [r2, #4]
    18f8:	02433001 	subeq	r3, r3, #1	; 0x1
    18fc:	05823004 	streq	r3, [r2, #4]
    1900:	0a000005 	beq	191c <init_malloc+0x9c>
    1904:	e152000c 	cmp	r2, ip
    1908:	e1a0e001 	mov	lr, r1
    190c:	e2822001 	add	r2, r2, #1	; 0x1
    1910:	e2811010 	add	r1, r1, #16	; 0x10
    1914:	3afffff3 	bcc	18e8 <init_malloc+0x68>
    1918:	e3a0e000 	mov	lr, #0	; 0x0
    191c:	e59fc050 	ldr	ip, [pc, #80]	; 1974 <init_malloc+0xf4>
    1920:	e59e2004 	ldr	r2, [lr, #4]
    1924:	e59c300c 	ldr	r3, [ip, #12]
    1928:	e1a0100e 	mov	r1, lr
    192c:	e2433b01 	sub	r3, r3, #1024	; 0x400
    1930:	e0603003 	rsb	r3, r0, r3
    1934:	e59c4018 	ldr	r4, [ip, #24]
    1938:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    193c:	e2800b01 	add	r0, r0, #1024	; 0x400
    1940:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1944:	e4810008 	str	r0, [r1], #8
    1948:	e1833002 	orr	r3, r3, r2
    194c:	e28c2014 	add	r2, ip, #20	; 0x14
    1950:	e58e2008 	str	r2, [lr, #8]
    1954:	e58c1018 	str	r1, [ip, #24]
    1958:	e5841000 	str	r1, [r4]
    195c:	e58e3004 	str	r3, [lr, #4]
    1960:	e58e400c 	str	r4, [lr, #12]
    1964:	e3a00000 	mov	r0, #0	; 0x0
    1968:	e8bd8010 	pop	{r4, pc}
    196c:	0800f01f 	.word	0x0800f01f
    1970:	0801a000 	.word	0x0801a000
    1974:	0800d744 	.word	0x0800d744

00001978 <isi_check_header>:
    1978:	e92d4070 	push	{r4, r5, r6, lr}
    197c:	e5902000 	ldr	r2, [r0]
    1980:	e59f3088 	ldr	r3, [pc, #136]	; 1a10 <isi_check_header+0x98>
    1984:	e1a05000 	mov	r5, r0
    1988:	e1520003 	cmp	r2, r3
    198c:	e24dd008 	sub	sp, sp, #8	; 0x8
    1990:	13e0001e 	mvnne	r0, #30	; 0x1e
    1994:	1a00001b 	bne	1a08 <isi_check_header+0x90>
    1998:	e3a06000 	mov	r6, #0	; 0x0
    199c:	e595401c 	ldr	r4, [r5, #28]
    19a0:	e3a01c02 	mov	r1, #512	; 0x200
    19a4:	e585601c 	str	r6, [r5, #28]
    19a8:	e1a00006 	mov	r0, r6
    19ac:	eb000f94 	bl	5804 <hash_init>
    19b0:	e3a01c02 	mov	r1, #512	; 0x200
    19b4:	e1a00005 	mov	r0, r5
    19b8:	eb000f6c 	bl	5770 <hash_data>
    19bc:	e28d0004 	add	r0, sp, #4	; 0x4
    19c0:	eb000f4e 	bl	5700 <hash_value>
    19c4:	e585401c 	str	r4, [r5, #28]
    19c8:	e59d3004 	ldr	r3, [sp, #4]
    19cc:	e1540003 	cmp	r4, r3
    19d0:	13e0001f 	mvnne	r0, #31	; 0x1f
    19d4:	1a00000b 	bne	1a08 <isi_check_header+0x90>
    19d8:	e5951004 	ldr	r1, [r5, #4]
    19dc:	e59f4030 	ldr	r4, [pc, #48]	; 1a14 <isi_check_header+0x9c>
    19e0:	e3c114ff 	bic	r1, r1, #-16777216	; 0xff000000
    19e4:	e59f002c 	ldr	r0, [pc, #44]	; 1a18 <isi_check_header+0xa0>
    19e8:	e1a0e00f 	mov	lr, pc
    19ec:	e594f000 	ldr	pc, [r4]
    19f0:	e5d51007 	ldrb	r1, [r5, #7]
    19f4:	e59f0020 	ldr	r0, [pc, #32]	; 1a1c <isi_check_header+0xa4>
    19f8:	e201100f 	and	r1, r1, #15	; 0xf
    19fc:	e1a0e00f 	mov	lr, pc
    1a00:	e594f000 	ldr	pc, [r4]
    1a04:	e1a00006 	mov	r0, r6
    1a08:	e28dd008 	add	sp, sp, #8	; 0x8
    1a0c:	e8bd8070 	pop	{r4, r5, r6, pc}
    1a10:	6973692b 	.word	0x6973692b
    1a14:	0800d704 	.word	0x0800d704
    1a18:	00010b5e 	.word	0x00010b5e
    1a1c:	00010b6c 	.word	0x00010b6c

00001a20 <isi_check_data_separate>:
    1a20:	e92d4070 	push	{r4, r5, r6, lr}
    1a24:	e1a05000 	mov	r5, r0
    1a28:	e5d00007 	ldrb	r0, [r0, #7]
    1a2c:	e24dd028 	sub	sp, sp, #40	; 0x28
    1a30:	e1a04001 	mov	r4, r1
    1a34:	e200000f 	and	r0, r0, #15	; 0xf
    1a38:	e595100c 	ldr	r1, [r5, #12]
    1a3c:	eb000f70 	bl	5804 <hash_init>
    1a40:	e28d6008 	add	r6, sp, #8	; 0x8
    1a44:	e595100c 	ldr	r1, [r5, #12]
    1a48:	e1a00004 	mov	r0, r4
    1a4c:	eb000f47 	bl	5770 <hash_data>
    1a50:	e1a00006 	mov	r0, r6
    1a54:	eb000f29 	bl	5700 <hash_value>
    1a58:	eb000f21 	bl	56e4 <hash_len>
    1a5c:	e2854020 	add	r4, r5, #32	; 0x20
    1a60:	e1a02000 	mov	r2, r0
    1a64:	e1a01004 	mov	r1, r4
    1a68:	e1a00006 	mov	r0, r6
    1a6c:	ebfffe7a 	bl	145c <memcmp>
    1a70:	e3500000 	cmp	r0, #0	; 0x0
    1a74:	0a000014 	beq	1acc <isi_check_data_separate+0xac>
    1a78:	e595100c 	ldr	r1, [r5, #12]
    1a7c:	e59f3050 	ldr	r3, [pc, #80]	; 1ad4 <isi_check_data_separate+0xb4>
    1a80:	e59f0050 	ldr	r0, [pc, #80]	; 1ad8 <isi_check_data_separate+0xb8>
    1a84:	e1a0e00f 	mov	lr, pc
    1a88:	e593f000 	ldr	pc, [r3]
    1a8c:	eb000f14 	bl	56e4 <hash_len>
    1a90:	e1a01004 	mov	r1, r4
    1a94:	e1a03000 	mov	r3, r0
    1a98:	e3a02001 	mov	r2, #1	; 0x1
    1a9c:	e1a00004 	mov	r0, r4
    1aa0:	e3a04020 	mov	r4, #32	; 0x20
    1aa4:	e58d4000 	str	r4, [sp]
    1aa8:	eb0028ee 	bl	be68 <print_buffer>
    1aac:	eb000f0c 	bl	56e4 <hash_len>
    1ab0:	e1a01006 	mov	r1, r6
    1ab4:	e1a03000 	mov	r3, r0
    1ab8:	e3a02001 	mov	r2, #1	; 0x1
    1abc:	e1a00006 	mov	r0, r6
    1ac0:	e58d4000 	str	r4, [sp]
    1ac4:	eb0028e7 	bl	be68 <print_buffer>
    1ac8:	e3e00020 	mvn	r0, #32	; 0x20
    1acc:	e28dd028 	add	sp, sp, #40	; 0x28
    1ad0:	e8bd8070 	pop	{r4, r5, r6, pc}
    1ad4:	0800d704 	.word	0x0800d704
    1ad8:	00010b7f 	.word	0x00010b7f

00001adc <isi_check_data>:
    1adc:	e2801c02 	add	r1, r0, #512	; 0x200
    1ae0:	eaffffce 	b	1a20 <isi_check_data_separate>

00001ae4 <isi_check>:
    1ae4:	e92d4010 	push	{r4, lr}
    1ae8:	e1a04000 	mov	r4, r0
    1aec:	ebffffa1 	bl	1978 <isi_check_header>
    1af0:	e3500000 	cmp	r0, #0	; 0x0
    1af4:	18bd8010 	popne	{r4, pc}
    1af8:	e1a00004 	mov	r0, r4
    1afc:	e8bd4010 	pop	{r4, lr}
    1b00:	eafffff5 	b	1adc <isi_check_data>

00001b04 <load_serial_ymodem>:
    1b04:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    1b08:	e24dde41 	sub	sp, sp, #1040	; 0x410
    1b0c:	e28d4b01 	add	r4, sp, #1024	; 0x400
    1b10:	e284400c 	add	r4, r4, #12	; 0xc
    1b14:	e3a03002 	mov	r3, #2	; 0x2
    1b18:	e1a07000 	mov	r7, r0
    1b1c:	e1a01004 	mov	r1, r4
    1b20:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1b24:	e58d3404 	str	r3, [sp, #1028]
    1b28:	eb0034f0 	bl	eef0 <xyzModem_stream_open>
    1b2c:	e3500000 	cmp	r0, #0	; 0x0
    1b30:	1a000012 	bne	1b80 <load_serial_ymodem+0x7c>
    1b34:	e1a08004 	mov	r8, r4
    1b38:	e1a05000 	mov	r5, r0
    1b3c:	e1a04000 	mov	r4, r0
    1b40:	e1a0600d 	mov	r6, sp
    1b44:	ea000001 	b	1b50 <load_serial_ymodem+0x4c>
    1b48:	e085500c 	add	r5, r5, ip
    1b4c:	ebfffe21 	bl	13d8 <memcpy>
    1b50:	e3a01b01 	mov	r1, #1024	; 0x400
    1b54:	e1a02008 	mov	r2, r8
    1b58:	e1a0000d 	mov	r0, sp
    1b5c:	eb00344f 	bl	eca0 <xyzModem_stream_read>
    1b60:	e0843007 	add	r3, r4, r7
    1b64:	e250c000 	subs	ip, r0, #0	; 0x0
    1b68:	e1a0100d 	mov	r1, sp
    1b6c:	e1a0200c 	mov	r2, ip
    1b70:	e1a00003 	mov	r0, r3
    1b74:	e084400c 	add	r4, r4, ip
    1b78:	cafffff2 	bgt	1b48 <load_serial_ymodem+0x44>
    1b7c:	ea000007 	b	1ba0 <load_serial_ymodem+0x9c>
    1b80:	e59f3054 	ldr	r3, [pc, #84]	; 1bdc <load_serial_ymodem+0xd8>
    1b84:	e59d040c 	ldr	r0, [sp, #1036]
    1b88:	e5934000 	ldr	r4, [r3]
    1b8c:	eb0030ca 	bl	debc <xyzModem_error>
    1b90:	e1a01000 	mov	r1, r0
    1b94:	e59f0044 	ldr	r0, [pc, #68]	; 1be0 <load_serial_ymodem+0xdc>
    1b98:	e12fff34 	blx	r4
    1b9c:	e3a05000 	mov	r5, #0	; 0x0
    1ba0:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1ba4:	e280000c 	add	r0, r0, #12	; 0xc
    1ba8:	eb0030ab 	bl	de5c <xyzModem_stream_close>
    1bac:	e3a00000 	mov	r0, #0	; 0x0
    1bb0:	e59f102c 	ldr	r1, [pc, #44]	; 1be4 <load_serial_ymodem+0xe0>
    1bb4:	eb003359 	bl	e920 <xyzModem_stream_terminate>
    1bb8:	e1a01005 	mov	r1, r5
    1bbc:	e59f0024 	ldr	r0, [pc, #36]	; 1be8 <load_serial_ymodem+0xe4>
    1bc0:	e1a02005 	mov	r2, r5
    1bc4:	e59f3010 	ldr	r3, [pc, #16]	; 1bdc <load_serial_ymodem+0xd8>
    1bc8:	e1a0e00f 	mov	lr, pc
    1bcc:	e593f000 	ldr	pc, [r3]
    1bd0:	e1a00007 	mov	r0, r7
    1bd4:	e28dde41 	add	sp, sp, #1040	; 0x410
    1bd8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    1bdc:	0800d704 	.word	0x0800d704
    1be0:	00010c7b 	.word	0x00010c7b
    1be4:	00001bec 	.word	0x00001bec
    1be8:	00010ba3 	.word	0x00010ba3

00001bec <getcxmodem>:
    1bec:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    1bf0:	e24dd004 	sub	sp, sp, #4	; 0x4
    1bf4:	eb002d4a 	bl	d124 <tstc>
    1bf8:	e3500000 	cmp	r0, #0	; 0x0
    1bfc:	1a000002 	bne	1c0c <getcxmodem+0x20>
    1c00:	e3e00000 	mvn	r0, #0	; 0x0
    1c04:	e28dd004 	add	sp, sp, #4	; 0x4
    1c08:	e8bd8000 	pop	{pc}
    1c0c:	e28dd004 	add	sp, sp, #4	; 0x4
    1c10:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    1c14:	ea002d43 	b	d128 <getc>

00001c18 <do_load_serial_bin>:
    1c18:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1c1c:	e3520001 	cmp	r2, #1	; 0x1
    1c20:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1c24:	e1a06002 	mov	r6, r2
    1c28:	e1a07000 	mov	r7, r0
    1c2c:	e1a05003 	mov	r5, r3
    1c30:	ca000006 	bgt	1c50 <do_load_serial_bin+0x38>
    1c34:	e59f0120 	ldr	r0, [pc, #288]	; 1d5c <do_load_serial_bin+0x144>
    1c38:	e3a06000 	mov	r6, #0	; 0x0
    1c3c:	e59f311c 	ldr	r3, [pc, #284]	; 1d60 <do_load_serial_bin+0x148>
    1c40:	e1a0e00f 	mov	lr, pc
    1c44:	e593f000 	ldr	pc, [r3]
    1c48:	e1a04006 	mov	r4, r6
    1c4c:	ea00000c 	b	1c84 <do_load_serial_bin+0x6c>
    1c50:	e5930004 	ldr	r0, [r3, #4]
    1c54:	e3a01000 	mov	r1, #0	; 0x0
    1c58:	e3a02010 	mov	r2, #16	; 0x10
    1c5c:	eb002919 	bl	c0c8 <simple_strtoul>
    1c60:	e3560003 	cmp	r6, #3	; 0x3
    1c64:	e1a04000 	mov	r4, r0
    1c68:	13a06000 	movne	r6, #0	; 0x0
    1c6c:	1a000004 	bne	1c84 <do_load_serial_bin+0x6c>
    1c70:	e5950008 	ldr	r0, [r5, #8]
    1c74:	e3a01000 	mov	r1, #0	; 0x0
    1c78:	e3a02010 	mov	r2, #16	; 0x10
    1c7c:	eb002911 	bl	c0c8 <simple_strtoul>
    1c80:	e1a06000 	mov	r6, r0
    1c84:	e5950000 	ldr	r0, [r5]
    1c88:	e59f10d4 	ldr	r1, [pc, #212]	; 1d64 <do_load_serial_bin+0x14c>
    1c8c:	ebfffd8a 	bl	12bc <strcmp>
    1c90:	e3500000 	cmp	r0, #0	; 0x0
    1c94:	1a000007 	bne	1cb8 <do_load_serial_bin+0xa0>
    1c98:	e1a01004 	mov	r1, r4
    1c9c:	e59f00c4 	ldr	r0, [pc, #196]	; 1d68 <do_load_serial_bin+0x150>
    1ca0:	e59f30b8 	ldr	r3, [pc, #184]	; 1d60 <do_load_serial_bin+0x148>
    1ca4:	e1a0e00f 	mov	lr, pc
    1ca8:	e593f000 	ldr	pc, [r3]
    1cac:	e1a00004 	mov	r0, r4
    1cb0:	ebffff93 	bl	1b04 <load_serial_ymodem>
    1cb4:	ea000025 	b	1d50 <do_load_serial_bin+0x138>
    1cb8:	e5950000 	ldr	r0, [r5]
    1cbc:	e59f10a8 	ldr	r1, [pc, #168]	; 1d6c <do_load_serial_bin+0x154>
    1cc0:	ebfffd7d 	bl	12bc <strcmp>
    1cc4:	e3500000 	cmp	r0, #0	; 0x0
    1cc8:	e59f5090 	ldr	r5, [pc, #144]	; 1d60 <do_load_serial_bin+0x148>
    1ccc:	1a00001b 	bne	1d40 <do_load_serial_bin+0x128>
    1cd0:	e1a02006 	mov	r2, r6
    1cd4:	e59f0094 	ldr	r0, [pc, #148]	; 1d70 <do_load_serial_bin+0x158>
    1cd8:	e1a01004 	mov	r1, r4
    1cdc:	e1a0e00f 	mov	lr, pc
    1ce0:	e595f000 	ldr	pc, [r5]
    1ce4:	e1a00004 	mov	r0, r4
    1ce8:	e1a01006 	mov	r1, r6
    1cec:	eb00326e 	bl	e6ac <control_send>
    1cf0:	e3500000 	cmp	r0, #0	; 0x0
    1cf4:	aa000005 	bge	1d10 <do_load_serial_bin+0xf8>
    1cf8:	e59d0004 	ldr	r0, [sp, #4]
    1cfc:	e5954000 	ldr	r4, [r5]
    1d00:	eb00306d 	bl	debc <xyzModem_error>
    1d04:	e1a01000 	mov	r1, r0
    1d08:	e59f0064 	ldr	r0, [pc, #100]	; 1d74 <do_load_serial_bin+0x15c>
    1d0c:	e12fff34 	blx	r4
    1d10:	e28d0004 	add	r0, sp, #4	; 0x4
    1d14:	eb003050 	bl	de5c <xyzModem_stream_close>
    1d18:	e3a00000 	mov	r0, #0	; 0x0
    1d1c:	e59f1054 	ldr	r1, [pc, #84]	; 1d78 <do_load_serial_bin+0x160>
    1d20:	eb0032fe 	bl	e920 <xyzModem_stream_terminate>
    1d24:	e1a01006 	mov	r1, r6
    1d28:	e59f004c 	ldr	r0, [pc, #76]	; 1d7c <do_load_serial_bin+0x164>
    1d2c:	e1a02006 	mov	r2, r6
    1d30:	e59f3028 	ldr	r3, [pc, #40]	; 1d60 <do_load_serial_bin+0x148>
    1d34:	e1a0e00f 	mov	lr, pc
    1d38:	e593f000 	ldr	pc, [r3]
    1d3c:	ea000003 	b	1d50 <do_load_serial_bin+0x138>
    1d40:	e5971014 	ldr	r1, [r7, #20]
    1d44:	e59f0034 	ldr	r0, [pc, #52]	; 1d80 <do_load_serial_bin+0x168>
    1d48:	e1a0e00f 	mov	lr, pc
    1d4c:	e595f000 	ldr	pc, [r5]
    1d50:	e3a00000 	mov	r0, #0	; 0x0
    1d54:	e28dd00c 	add	sp, sp, #12	; 0xc
    1d58:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1d5c:	00010bcb 	.word	0x00010bcb
    1d60:	0800d704 	.word	0x0800d704
    1d64:	00010be5 	.word	0x00010be5
    1d68:	00010be8 	.word	0x00010be8
    1d6c:	00010c2b 	.word	0x00010c2b
    1d70:	00010c2e 	.word	0x00010c2e
    1d74:	00010c7b 	.word	0x00010c7b
    1d78:	00001bec 	.word	0x00001bec
    1d7c:	00010ba3 	.word	0x00010ba3
    1d80:	00010c7a 	.word	0x00010c7a

00001d84 <do_mem_cp>:
    1d84:	e3520004 	cmp	r2, #4	; 0x4
    1d88:	e92d4070 	push	{r4, r5, r6, lr}
    1d8c:	e1a04003 	mov	r4, r3
    1d90:	0a000002 	beq	1da0 <do_mem_cp+0x1c>
    1d94:	eb0003de 	bl	2d14 <cmd_usage>
    1d98:	e3a00001 	mov	r0, #1	; 0x1
    1d9c:	e8bd8070 	pop	{r4, r5, r6, pc}
    1da0:	e5930004 	ldr	r0, [r3, #4]
    1da4:	e3a01000 	mov	r1, #0	; 0x0
    1da8:	e3a02010 	mov	r2, #16	; 0x10
    1dac:	eb0028c5 	bl	c0c8 <simple_strtoul>
    1db0:	e3a01000 	mov	r1, #0	; 0x0
    1db4:	e3a02010 	mov	r2, #16	; 0x10
    1db8:	e1a06000 	mov	r6, r0
    1dbc:	e5940008 	ldr	r0, [r4, #8]
    1dc0:	eb0028c0 	bl	c0c8 <simple_strtoul>
    1dc4:	e3a01000 	mov	r1, #0	; 0x0
    1dc8:	e1a05000 	mov	r5, r0
    1dcc:	e3a02010 	mov	r2, #16	; 0x10
    1dd0:	e594000c 	ldr	r0, [r4, #12]
    1dd4:	eb0028bb 	bl	c0c8 <simple_strtoul>
    1dd8:	e2800003 	add	r0, r0, #3	; 0x3
    1ddc:	e1b00120 	lsrs	r0, r0, #2
    1de0:	1a000002 	bne	1df0 <do_mem_cp+0x6c>
    1de4:	ea000006 	b	1e04 <do_mem_cp+0x80>
    1de8:	e5163004 	ldr	r3, [r6, #-4]
    1dec:	e5053004 	str	r3, [r5, #-4]
    1df0:	e3500000 	cmp	r0, #0	; 0x0
    1df4:	e2866004 	add	r6, r6, #4	; 0x4
    1df8:	e2400001 	sub	r0, r0, #1	; 0x1
    1dfc:	e2855004 	add	r5, r5, #4	; 0x4
    1e00:	1afffff8 	bne	1de8 <do_mem_cp+0x64>
    1e04:	e3a00000 	mov	r0, #0	; 0x0
    1e08:	e8bd8070 	pop	{r4, r5, r6, pc}

00001e0c <do_mem_cmp>:
    1e0c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1e10:	e3520004 	cmp	r2, #4	; 0x4
    1e14:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1e18:	e1a04003 	mov	r4, r3
    1e1c:	0a000002 	beq	1e2c <do_mem_cmp+0x20>
    1e20:	eb0003bb 	bl	2d14 <cmd_usage>
    1e24:	e3a04001 	mov	r4, #1	; 0x1
    1e28:	ea000029 	b	1ed4 <do_mem_cmp+0xc8>
    1e2c:	e5930004 	ldr	r0, [r3, #4]
    1e30:	e3a01000 	mov	r1, #0	; 0x0
    1e34:	e3a02010 	mov	r2, #16	; 0x10
    1e38:	eb0028a2 	bl	c0c8 <simple_strtoul>
    1e3c:	e3a01000 	mov	r1, #0	; 0x0
    1e40:	e3a02010 	mov	r2, #16	; 0x10
    1e44:	e1a07000 	mov	r7, r0
    1e48:	e5940008 	ldr	r0, [r4, #8]
    1e4c:	eb00289d 	bl	c0c8 <simple_strtoul>
    1e50:	e3a01000 	mov	r1, #0	; 0x0
    1e54:	e1a06000 	mov	r6, r0
    1e58:	e3a02010 	mov	r2, #16	; 0x10
    1e5c:	e594000c 	ldr	r0, [r4, #12]
    1e60:	eb002898 	bl	c0c8 <simple_strtoul>
    1e64:	e2800003 	add	r0, r0, #3	; 0x3
    1e68:	e1a00120 	lsr	r0, r0, #2
    1e6c:	e3a05000 	mov	r5, #0	; 0x0
    1e70:	ea00000f 	b	1eb4 <do_mem_cmp+0xa8>
    1e74:	e5972000 	ldr	r2, [r7]
    1e78:	e5963000 	ldr	r3, [r6]
    1e7c:	e1520003 	cmp	r2, r3
    1e80:	0a000008 	beq	1ea8 <do_mem_cmp+0x9c>
    1e84:	e58d3000 	str	r3, [sp]
    1e88:	e1a01007 	mov	r1, r7
    1e8c:	e1a03006 	mov	r3, r6
    1e90:	e59f0048 	ldr	r0, [pc, #72]	; 1ee0 <do_mem_cmp+0xd4>
    1e94:	e59fc048 	ldr	ip, [pc, #72]	; 1ee4 <do_mem_cmp+0xd8>
    1e98:	e1a0e00f 	mov	lr, pc
    1e9c:	e59cf000 	ldr	pc, [ip]
    1ea0:	e3a04001 	mov	r4, #1	; 0x1
    1ea4:	ea000005 	b	1ec0 <do_mem_cmp+0xb4>
    1ea8:	e2855001 	add	r5, r5, #1	; 0x1
    1eac:	e2877004 	add	r7, r7, #4	; 0x4
    1eb0:	e2866004 	add	r6, r6, #4	; 0x4
    1eb4:	e1500005 	cmp	r0, r5
    1eb8:	1affffed 	bne	1e74 <do_mem_cmp+0x68>
    1ebc:	e3a04000 	mov	r4, #0	; 0x0
    1ec0:	e1a01005 	mov	r1, r5
    1ec4:	e59f001c 	ldr	r0, [pc, #28]	; 1ee8 <do_mem_cmp+0xdc>
    1ec8:	e59f3014 	ldr	r3, [pc, #20]	; 1ee4 <do_mem_cmp+0xd8>
    1ecc:	e1a0e00f 	mov	lr, pc
    1ed0:	e593f000 	ldr	pc, [r3]
    1ed4:	e1a00004 	mov	r0, r4
    1ed8:	e28dd00c 	add	sp, sp, #12	; 0xc
    1edc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1ee0:	00010d80 	.word	0x00010d80
    1ee4:	0800d704 	.word	0x0800d704
    1ee8:	00010db8 	.word	0x00010db8

00001eec <do_mem_mw>:
    1eec:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1ef0:	e1a06002 	mov	r6, r2
    1ef4:	e2422003 	sub	r2, r2, #3	; 0x3
    1ef8:	e3520001 	cmp	r2, #1	; 0x1
    1efc:	e24dd004 	sub	sp, sp, #4	; 0x4
    1f00:	e1a04003 	mov	r4, r3
    1f04:	9a000002 	bls	1f14 <do_mem_mw+0x28>
    1f08:	eb000381 	bl	2d14 <cmd_usage>
    1f0c:	e3a00001 	mov	r0, #1	; 0x1
    1f10:	ea000016 	b	1f70 <do_mem_mw+0x84>
    1f14:	e3a01000 	mov	r1, #0	; 0x0
    1f18:	e3a02010 	mov	r2, #16	; 0x10
    1f1c:	e5930004 	ldr	r0, [r3, #4]
    1f20:	eb002868 	bl	c0c8 <simple_strtoul>
    1f24:	e3a01000 	mov	r1, #0	; 0x0
    1f28:	e1a05000 	mov	r5, r0
    1f2c:	e3a02010 	mov	r2, #16	; 0x10
    1f30:	e5940008 	ldr	r0, [r4, #8]
    1f34:	eb002863 	bl	c0c8 <simple_strtoul>
    1f38:	e3560004 	cmp	r6, #4	; 0x4
    1f3c:	e1a07000 	mov	r7, r0
    1f40:	13a00001 	movne	r0, #1	; 0x1
    1f44:	1a000006 	bne	1f64 <do_mem_mw+0x78>
    1f48:	e594000c 	ldr	r0, [r4, #12]
    1f4c:	e3a01000 	mov	r1, #0	; 0x0
    1f50:	e3a02010 	mov	r2, #16	; 0x10
    1f54:	eb00285b 	bl	c0c8 <simple_strtoul>
    1f58:	ea000001 	b	1f64 <do_mem_mw+0x78>
    1f5c:	e5057004 	str	r7, [r5, #-4]
    1f60:	e2400001 	sub	r0, r0, #1	; 0x1
    1f64:	e3500000 	cmp	r0, #0	; 0x0
    1f68:	e2855004 	add	r5, r5, #4	; 0x4
    1f6c:	1afffffa 	bne	1f5c <do_mem_mw+0x70>
    1f70:	e28dd004 	add	sp, sp, #4	; 0x4
    1f74:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00001f78 <do_mem_md>:
    1f78:	e92d4070 	push	{r4, r5, r6, lr}
    1f7c:	e3520001 	cmp	r2, #1	; 0x1
    1f80:	e24dd008 	sub	sp, sp, #8	; 0x8
    1f84:	e1a04002 	mov	r4, r2
    1f88:	e1a05003 	mov	r5, r3
    1f8c:	ca000002 	bgt	1f9c <do_mem_md+0x24>
    1f90:	eb00035f 	bl	2d14 <cmd_usage>
    1f94:	e3a00001 	mov	r0, #1	; 0x1
    1f98:	ea000014 	b	1ff0 <do_mem_md+0x78>
    1f9c:	e5930004 	ldr	r0, [r3, #4]
    1fa0:	e3a01000 	mov	r1, #0	; 0x0
    1fa4:	e3a02010 	mov	r2, #16	; 0x10
    1fa8:	eb002846 	bl	c0c8 <simple_strtoul>
    1fac:	e3540002 	cmp	r4, #2	; 0x2
    1fb0:	e1a06000 	mov	r6, r0
    1fb4:	03a030a0 	moveq	r3, #160	; 0xa0
    1fb8:	0a000004 	beq	1fd0 <do_mem_md+0x58>
    1fbc:	e5950008 	ldr	r0, [r5, #8]
    1fc0:	e3a01000 	mov	r1, #0	; 0x0
    1fc4:	e3a02010 	mov	r2, #16	; 0x10
    1fc8:	eb00283e 	bl	c0c8 <simple_strtoul>
    1fcc:	e1a03000 	mov	r3, r0
    1fd0:	e3a0c004 	mov	ip, #4	; 0x4
    1fd4:	e1a00006 	mov	r0, r6
    1fd8:	e1a0200c 	mov	r2, ip
    1fdc:	e1a03123 	lsr	r3, r3, #2
    1fe0:	e1a01006 	mov	r1, r6
    1fe4:	e58dc000 	str	ip, [sp]
    1fe8:	eb00279e 	bl	be68 <print_buffer>
    1fec:	e3a00000 	mov	r0, #0	; 0x0
    1ff0:	e28dd008 	add	sp, sp, #8	; 0x8
    1ff4:	e8bd8070 	pop	{r4, r5, r6, pc}

00001ff8 <do_sc>:
    1ff8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1ffc:	e3520002 	cmp	r2, #2	; 0x2
    2000:	e24ddf47 	sub	sp, sp, #284	; 0x11c
    2004:	0a000002 	beq	2014 <do_sc+0x1c>
    2008:	eb000341 	bl	2d14 <cmd_usage>
    200c:	e3a00001 	mov	r0, #1	; 0x1
    2010:	ea000060 	b	2198 <do_sc+0x1a0>
    2014:	e5930004 	ldr	r0, [r3, #4]
    2018:	e3a01000 	mov	r1, #0	; 0x0
    201c:	e3a02010 	mov	r2, #16	; 0x10
    2020:	eb002828 	bl	c0c8 <simple_strtoul>
    2024:	e28d4f41 	add	r4, sp, #260	; 0x104
    2028:	e2844002 	add	r4, r4, #2	; 0x2
    202c:	e59f116c 	ldr	r1, [pc, #364]	; 21a0 <do_sc+0x1a8>
    2030:	e1a06000 	mov	r6, r0
    2034:	e3a02012 	mov	r2, #18	; 0x12
    2038:	e1a00004 	mov	r0, r4
    203c:	ebfffce5 	bl	13d8 <memcpy>
    2040:	e3a01000 	mov	r1, #0	; 0x0
    2044:	ea000004 	b	205c <do_sc+0x64>
    2048:	e7d12006 	ldrb	r2, [r1, r6]
    204c:	e7d43001 	ldrb	r3, [r4, r1]
    2050:	e2811001 	add	r1, r1, #1	; 0x1
    2054:	e1520003 	cmp	r2, r3
    2058:	1a000014 	bne	20b0 <do_sc+0xb8>
    205c:	e3510012 	cmp	r1, #18	; 0x12
    2060:	e0815006 	add	r5, r1, r6
    2064:	1afffff7 	bne	2048 <do_sc+0x50>
    2068:	e2851eff 	add	r1, r5, #4080	; 0xff0
    206c:	e281100f 	add	r1, r1, #15	; 0xf
    2070:	e1a02005 	mov	r2, r5
    2074:	e5d23000 	ldrb	r3, [r2]
    2078:	e353000a 	cmp	r3, #10	; 0xa
    207c:	1a000005 	bne	2098 <do_sc+0xa0>
    2080:	e5d23001 	ldrb	r3, [r2, #1]
    2084:	e353005d 	cmp	r3, #93	; 0x5d
    2088:	1a000002 	bne	2098 <do_sc+0xa0>
    208c:	e5d23002 	ldrb	r3, [r2, #2]
    2090:	e353005d 	cmp	r3, #93	; 0x5d
    2094:	0a000003 	beq	20a8 <do_sc+0xb0>
    2098:	e1520001 	cmp	r2, r1
    209c:	e2822001 	add	r2, r2, #1	; 0x1
    20a0:	1afffff3 	bne	2074 <do_sc+0x7c>
    20a4:	ea000001 	b	20b0 <do_sc+0xb8>
    20a8:	e3550000 	cmp	r5, #0	; 0x0
    20ac:	1a000006 	bne	20cc <do_sc+0xd4>
    20b0:	e59f00ec 	ldr	r0, [pc, #236]	; 21a4 <do_sc+0x1ac>
    20b4:	e1a01006 	mov	r1, r6
    20b8:	e59f30e8 	ldr	r3, [pc, #232]	; 21a8 <do_sc+0x1b0>
    20bc:	e1a0e00f 	mov	lr, pc
    20c0:	e593f000 	ldr	pc, [r3]
    20c4:	e3e00000 	mvn	r0, #0	; 0x0
    20c8:	ea000032 	b	2198 <do_sc+0x1a0>
    20cc:	e59f40d4 	ldr	r4, [pc, #212]	; 21a8 <do_sc+0x1b0>
    20d0:	e1a01006 	mov	r1, r6
    20d4:	e59f00d0 	ldr	r0, [pc, #208]	; 21ac <do_sc+0x1b4>
    20d8:	e28d6008 	add	r6, sp, #8	; 0x8
    20dc:	e1a0e00f 	mov	lr, pc
    20e0:	e594f000 	ldr	pc, [r4]
    20e4:	e2466002 	sub	r6, r6, #2	; 0x2
    20e8:	e1a07004 	mov	r7, r4
    20ec:	ea000000 	b	20f4 <do_sc+0xfc>
    20f0:	e2855001 	add	r5, r5, #1	; 0x1
    20f4:	e5d53000 	ldrb	r3, [r5]
    20f8:	e353002d 	cmp	r3, #45	; 0x2d
    20fc:	1353000a 	cmpne	r3, #10	; 0xa
    2100:	0afffffa 	beq	20f0 <do_sc+0xf8>
    2104:	e353005b 	cmp	r3, #91	; 0x5b
    2108:	0afffff8 	beq	20f0 <do_sc+0xf8>
    210c:	e353005d 	cmp	r3, #93	; 0x5d
    2110:	0a000001 	beq	211c <do_sc+0x124>
    2114:	e1a02006 	mov	r2, r6
    2118:	ea000005 	b	2134 <do_sc+0x13c>
    211c:	e5d53001 	ldrb	r3, [r5, #1]
    2120:	e353005d 	cmp	r3, #93	; 0x5d
    2124:	1afffffa 	bne	2114 <do_sc+0x11c>
    2128:	ea00000c 	b	2160 <do_sc+0x168>
    212c:	e5423001 	strb	r3, [r2, #-1]
    2130:	e2855001 	add	r5, r5, #1	; 0x1
    2134:	e5d53000 	ldrb	r3, [r5]
    2138:	e1a01002 	mov	r1, r2
    213c:	e353000a 	cmp	r3, #10	; 0xa
    2140:	1353005d 	cmpne	r3, #93	; 0x5d
    2144:	03a04000 	moveq	r4, #0	; 0x0
    2148:	13a04001 	movne	r4, #1	; 0x1
    214c:	e2822001 	add	r2, r2, #1	; 0x1
    2150:	1afffff5 	bne	212c <do_sc+0x134>
    2154:	e3550000 	cmp	r5, #0	; 0x0
    2158:	e5c14000 	strb	r4, [r1]
    215c:	1a000005 	bne	2178 <do_sc+0x180>
    2160:	e59f0048 	ldr	r0, [pc, #72]	; 21b0 <do_sc+0x1b8>
    2164:	e59f303c 	ldr	r3, [pc, #60]	; 21a8 <do_sc+0x1b0>
    2168:	e1a0e00f 	mov	lr, pc
    216c:	e593f000 	ldr	pc, [r3]
    2170:	e3a00000 	mov	r0, #0	; 0x0
    2174:	ea000007 	b	2198 <do_sc+0x1a0>
    2178:	e1a01006 	mov	r1, r6
    217c:	e59f0030 	ldr	r0, [pc, #48]	; 21b4 <do_sc+0x1bc>
    2180:	e1a0e00f 	mov	lr, pc
    2184:	e597f000 	ldr	pc, [r7]
    2188:	e1a00006 	mov	r0, r6
    218c:	e1a01004 	mov	r1, r4
    2190:	eb002c20 	bl	d218 <run_command>
    2194:	eaffffd6 	b	20f4 <do_sc+0xfc>
    2198:	e28ddf47 	add	sp, sp, #284	; 0x11c
    219c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    21a0:	000107dc 	.word	0x000107dc
    21a4:	00010e82 	.word	0x00010e82
    21a8:	0800d704 	.word	0x0800d704
    21ac:	00010e9f 	.word	0x00010e9f
    21b0:	00010eb8 	.word	0x00010eb8
    21b4:	00010ec5 	.word	0x00010ec5

000021b8 <do_ls>:
    21b8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    21bc:	e59f001c 	ldr	r0, [pc, #28]	; 21e0 <do_ls+0x28>
    21c0:	e24dd004 	sub	sp, sp, #4	; 0x4
    21c4:	e59f1018 	ldr	r1, [pc, #24]	; 21e4 <do_ls+0x2c>
    21c8:	e59f3018 	ldr	r3, [pc, #24]	; 21e8 <do_ls+0x30>
    21cc:	e1a0e00f 	mov	lr, pc
    21d0:	e593f000 	ldr	pc, [r3]
    21d4:	e3a00000 	mov	r0, #0	; 0x0
    21d8:	e28dd004 	add	sp, sp, #4	; 0x4
    21dc:	e8bd8000 	pop	{pc}
    21e0:	00010c7b 	.word	0x00010c7b
    21e4:	0800d0bc 	.word	0x0800d0bc
    21e8:	0800d704 	.word	0x0800d704

000021ec <do_linkpc>:
    21ec:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    21f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    21f4:	eb0026d1 	bl	bd40 <linkpc_loop>
    21f8:	e3a00000 	mov	r0, #0	; 0x0
    21fc:	e28dd004 	add	sp, sp, #4	; 0x4
    2200:	e8bd8000 	pop	{pc}

00002204 <do_pads>:
    2204:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    2208:	e3520002 	cmp	r2, #2	; 0x2
    220c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2210:	e1a04002 	mov	r4, r2
    2214:	e1a06003 	mov	r6, r3
    2218:	d59f01e4 	ldrle	r0, [pc, #484]	; 2404 <do_pads+0x200>
    221c:	da00006e 	ble	23dc <do_pads+0x1d8>
    2220:	e5930008 	ldr	r0, [r3, #8]
    2224:	e3a01000 	mov	r1, #0	; 0x0
    2228:	e3a0200a 	mov	r2, #10	; 0xa
    222c:	eb0027a5 	bl	c0c8 <simple_strtoul>
    2230:	e3540003 	cmp	r4, #3	; 0x3
    2234:	e1a05000 	mov	r5, r0
    2238:	01a08000 	moveq	r8, r0
    223c:	0a000004 	beq	2254 <do_pads+0x50>
    2240:	e596000c 	ldr	r0, [r6, #12]
    2244:	e3a01000 	mov	r1, #0	; 0x0
    2248:	e3a0200a 	mov	r2, #10	; 0xa
    224c:	eb00279d 	bl	c0c8 <simple_strtoul>
    2250:	e1a08000 	mov	r8, r0
    2254:	e1953008 	orrs	r3, r5, r8
    2258:	459f01a8 	ldrmi	r0, [pc, #424]	; 2408 <do_pads+0x204>
    225c:	4a00005e 	bmi	23dc <do_pads+0x1d8>
    2260:	e5964010 	ldr	r4, [r6, #16]
    2264:	e59f11a0 	ldr	r1, [pc, #416]	; 240c <do_pads+0x208>
    2268:	e1a00004 	mov	r0, r4
    226c:	ebfffc12 	bl	12bc <strcmp>
    2270:	e3500000 	cmp	r0, #0	; 0x0
    2274:	03a07001 	moveq	r7, #1	; 0x1
    2278:	0a00000b 	beq	22ac <do_pads+0xa8>
    227c:	e1a00004 	mov	r0, r4
    2280:	e59f1188 	ldr	r1, [pc, #392]	; 2410 <do_pads+0x20c>
    2284:	ebfffc0c 	bl	12bc <strcmp>
    2288:	e3500000 	cmp	r0, #0	; 0x0
    228c:	01a07000 	moveq	r7, r0
    2290:	0a000005 	beq	22ac <do_pads+0xa8>
    2294:	e59f0178 	ldr	r0, [pc, #376]	; 2414 <do_pads+0x210>
    2298:	e59f3178 	ldr	r3, [pc, #376]	; 2418 <do_pads+0x214>
    229c:	e1a0e00f 	mov	lr, pc
    22a0:	e593f000 	ldr	pc, [r3]
    22a4:	e3e00000 	mvn	r0, #0	; 0x0
    22a8:	ea000053 	b	23fc <do_pads+0x1f8>
    22ac:	e2866004 	add	r6, r6, #4	; 0x4
    22b0:	e59fb164 	ldr	fp, [pc, #356]	; 241c <do_pads+0x218>
    22b4:	e59fa15c 	ldr	sl, [pc, #348]	; 2418 <do_pads+0x214>
    22b8:	e59f9160 	ldr	r9, [pc, #352]	; 2420 <do_pads+0x21c>
    22bc:	ea00004b 	b	23f0 <do_pads+0x1ec>
    22c0:	e5960000 	ldr	r0, [r6]
    22c4:	e59f1150 	ldr	r1, [pc, #336]	; 241c <do_pads+0x218>
    22c8:	e3a02002 	mov	r2, #2	; 0x2
    22cc:	ebfffc08 	bl	12f4 <strncmp>
    22d0:	e3500000 	cmp	r0, #0	; 0x0
    22d4:	1a00000d 	bne	2310 <do_pads+0x10c>
    22d8:	e1a02000 	mov	r2, r0
    22dc:	e1a03007 	mov	r3, r7
    22e0:	e1a00005 	mov	r0, r5
    22e4:	e3a01001 	mov	r1, #1	; 0x1
    22e8:	ebfffa32 	bl	bb8 <pads_chmod>
    22ec:	e1a01007 	mov	r1, r7
    22f0:	e1a00005 	mov	r0, r5
    22f4:	e59a4000 	ldr	r4, [sl]
    22f8:	ebfffab7 	bl	ddc <pads_getpin>
    22fc:	e1a01005 	mov	r1, r5
    2300:	e1a02000 	mov	r2, r0
    2304:	e59f0118 	ldr	r0, [pc, #280]	; 2424 <do_pads+0x220>
    2308:	e12fff34 	blx	r4
    230c:	ea000036 	b	23ec <do_pads+0x1e8>
    2310:	e59f1110 	ldr	r1, [pc, #272]	; 2428 <do_pads+0x224>
    2314:	e3a02004 	mov	r2, #4	; 0x4
    2318:	e5960000 	ldr	r0, [r6]
    231c:	ebfffbf4 	bl	12f4 <strncmp>
    2320:	e3500000 	cmp	r0, #0	; 0x0
    2324:	01a00005 	moveq	r0, r5
    2328:	03a01002 	moveq	r1, #2	; 0x2
    232c:	03a02001 	moveq	r2, #1	; 0x1
    2330:	0a000011 	beq	237c <do_pads+0x178>
    2334:	e59f10f0 	ldr	r1, [pc, #240]	; 242c <do_pads+0x228>
    2338:	e3a02003 	mov	r2, #3	; 0x3
    233c:	e5960000 	ldr	r0, [r6]
    2340:	ebfffbeb 	bl	12f4 <strncmp>
    2344:	e3500000 	cmp	r0, #0	; 0x0
    2348:	01a02000 	moveq	r2, r0
    234c:	03a01002 	moveq	r1, #2	; 0x2
    2350:	01a00005 	moveq	r0, r5
    2354:	0a000008 	beq	237c <do_pads+0x178>
    2358:	e5960000 	ldr	r0, [r6]
    235c:	e59f10cc 	ldr	r1, [pc, #204]	; 2430 <do_pads+0x22c>
    2360:	e3a02004 	mov	r2, #4	; 0x4
    2364:	ebfffbe2 	bl	12f4 <strncmp>
    2368:	e3500000 	cmp	r0, #0	; 0x0
    236c:	1a000005 	bne	2388 <do_pads+0x184>
    2370:	e1a01000 	mov	r1, r0
    2374:	e1a02001 	mov	r2, r1
    2378:	e1a00005 	mov	r0, r5
    237c:	e1a03007 	mov	r3, r7
    2380:	ebfffa0c 	bl	bb8 <pads_chmod>
    2384:	ea000018 	b	23ec <do_pads+0x1e8>
    2388:	e5960000 	ldr	r0, [r6]
    238c:	e59f10a0 	ldr	r1, [pc, #160]	; 2434 <do_pads+0x230>
    2390:	e3a02002 	mov	r2, #2	; 0x2
    2394:	ebfffbd6 	bl	12f4 <strncmp>
    2398:	e3500000 	cmp	r0, #0	; 0x0
    239c:	1a00000d 	bne	23d8 <do_pads+0x1d4>
    23a0:	e1a00005 	mov	r0, r5
    23a4:	e1a01007 	mov	r1, r7
    23a8:	ebfff9e7 	bl	b4c <pads_states>
    23ac:	e3500000 	cmp	r0, #0	; 0x0
    23b0:	e59a3000 	ldr	r3, [sl]
    23b4:	059f207c 	ldreq	r2, [pc, #124]	; 2438 <do_pads+0x234>
    23b8:	0a000002 	beq	23c8 <do_pads+0x1c4>
    23bc:	e3500001 	cmp	r0, #1	; 0x1
    23c0:	01a0200b 	moveq	r2, fp
    23c4:	11a02009 	movne	r2, r9
    23c8:	e59f006c 	ldr	r0, [pc, #108]	; 243c <do_pads+0x238>
    23cc:	e1a01005 	mov	r1, r5
    23d0:	e12fff33 	blx	r3
    23d4:	ea000004 	b	23ec <do_pads+0x1e8>
    23d8:	e59f0060 	ldr	r0, [pc, #96]	; 2440 <do_pads+0x23c>
    23dc:	e59f3034 	ldr	r3, [pc, #52]	; 2418 <do_pads+0x214>
    23e0:	e1a0e00f 	mov	lr, pc
    23e4:	e593f000 	ldr	pc, [r3]
    23e8:	ea000002 	b	23f8 <do_pads+0x1f4>
    23ec:	e2855001 	add	r5, r5, #1	; 0x1
    23f0:	e1550008 	cmp	r5, r8
    23f4:	daffffb1 	ble	22c0 <do_pads+0xbc>
    23f8:	e3a00000 	mov	r0, #0	; 0x0
    23fc:	e28dd004 	add	sp, sp, #4	; 0x4
    2400:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    2404:	00010eec 	.word	0x00010eec
    2408:	00010f04 	.word	0x00010f04
    240c:	00010f29 	.word	0x00010f29
    2410:	00010f2e 	.word	0x00010f2e
    2414:	00010f32 	.word	0x00010f32
    2418:	0800d704 	.word	0x0800d704
    241c:	00010f4b 	.word	0x00010f4b
    2420:	000125c4 	.word	0x000125c4
    2424:	00010f4e 	.word	0x00010f4e
    2428:	00010f62 	.word	0x00010f62
    242c:	00010f67 	.word	0x00010f67
    2430:	00010f6b 	.word	0x00010f6b
    2434:	00010f70 	.word	0x00010f70
    2438:	00010f73 	.word	0x00010f73
    243c:	00010f7c 	.word	0x00010f7c
    2440:	00010f91 	.word	0x00010f91

00002444 <do_mux>:
    2444:	e92d4070 	push	{r4, r5, r6, lr}
    2448:	e5934008 	ldr	r4, [r3, #8]
    244c:	e59f108c 	ldr	r1, [pc, #140]	; 24e0 <do_mux+0x9c>
    2450:	e1a00004 	mov	r0, r4
    2454:	e1a05003 	mov	r5, r3
    2458:	e1a06002 	mov	r6, r2
    245c:	ebfffb96 	bl	12bc <strcmp>
    2460:	e3500000 	cmp	r0, #0	; 0x0
    2464:	03a04001 	moveq	r4, #1	; 0x1
    2468:	0a00000b 	beq	249c <do_mux+0x58>
    246c:	e1a00004 	mov	r0, r4
    2470:	e59f106c 	ldr	r1, [pc, #108]	; 24e4 <do_mux+0xa0>
    2474:	ebfffb90 	bl	12bc <strcmp>
    2478:	e3500000 	cmp	r0, #0	; 0x0
    247c:	01a04000 	moveq	r4, r0
    2480:	0a000005 	beq	249c <do_mux+0x58>
    2484:	e59f005c 	ldr	r0, [pc, #92]	; 24e8 <do_mux+0xa4>
    2488:	e59f305c 	ldr	r3, [pc, #92]	; 24ec <do_mux+0xa8>
    248c:	e1a0e00f 	mov	lr, pc
    2490:	e593f000 	ldr	pc, [r3]
    2494:	e3e00000 	mvn	r0, #0	; 0x0
    2498:	e8bd8070 	pop	{r4, r5, r6, pc}
    249c:	e3560001 	cmp	r6, #1	; 0x1
    24a0:	da000006 	ble	24c0 <do_mux+0x7c>
    24a4:	e3a01000 	mov	r1, #0	; 0x0
    24a8:	e5950004 	ldr	r0, [r5, #4]
    24ac:	e3a0200a 	mov	r2, #10	; 0xa
    24b0:	eb002704 	bl	c0c8 <simple_strtoul>
    24b4:	e1a01004 	mov	r1, r4
    24b8:	e6ff0070 	uxth	r0, r0
    24bc:	ebfffb1e 	bl	113c <set_mux>
    24c0:	e59f3028 	ldr	r3, [pc, #40]	; 24f0 <do_mux+0xac>
    24c4:	e59f0028 	ldr	r0, [pc, #40]	; 24f4 <do_mux+0xb0>
    24c8:	e5931000 	ldr	r1, [r3]
    24cc:	e59f3018 	ldr	r3, [pc, #24]	; 24ec <do_mux+0xa8>
    24d0:	e1a0e00f 	mov	lr, pc
    24d4:	e593f000 	ldr	pc, [r3]
    24d8:	e3a00000 	mov	r0, #0	; 0x0
    24dc:	e8bd8070 	pop	{r4, r5, r6, pc}
    24e0:	00010f29 	.word	0x00010f29
    24e4:	00010f2e 	.word	0x00010f2e
    24e8:	00010f32 	.word	0x00010f32
    24ec:	0800d704 	.word	0x0800d704
    24f0:	2d009000 	.word	0x2d009000
    24f4:	00010faa 	.word	0x00010faa

000024f8 <do_mac>:
    24f8:	e59f3020 	ldr	r3, [pc, #32]	; 2520 <do_mac+0x28>
    24fc:	e92d4010 	push	{r4, lr}
    2500:	e5934000 	ldr	r4, [r3]
    2504:	ebfff91e 	bl	984 <efuse_mac>
    2508:	e1a02000 	mov	r2, r0
    250c:	e1a03001 	mov	r3, r1
    2510:	e59f000c 	ldr	r0, [pc, #12]	; 2524 <do_mac+0x2c>
    2514:	e12fff34 	blx	r4
    2518:	e3a00000 	mov	r0, #0	; 0x0
    251c:	e8bd8010 	pop	{r4, pc}
    2520:	0800d704 	.word	0x0800d704
    2524:	00010fbc 	.word	0x00010fbc

00002528 <do_boot>:
    2528:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    252c:	e3a00000 	mov	r0, #0	; 0x0
    2530:	e24dd004 	sub	sp, sp, #4	; 0x4
    2534:	ebfff7b3 	bl	408 <try_boot>
    2538:	e3a00000 	mov	r0, #0	; 0x0
    253c:	e28dd004 	add	sp, sp, #4	; 0x4
    2540:	e8bd8000 	pop	{pc}

00002544 <do_efuse>:
    2544:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2548:	e3520001 	cmp	r2, #1	; 0x1
    254c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2550:	da000006 	ble	2570 <do_efuse+0x2c>
    2554:	e5930004 	ldr	r0, [r3, #4]
    2558:	e59f1024 	ldr	r1, [pc, #36]	; 2584 <do_efuse+0x40>
    255c:	e3a02002 	mov	r2, #2	; 0x2
    2560:	ebfffb63 	bl	12f4 <strncmp>
    2564:	e3500000 	cmp	r0, #0	; 0x0
    2568:	02800001 	addeq	r0, r0, #1	; 0x1
    256c:	0a000000 	beq	2574 <do_efuse+0x30>
    2570:	e3a00000 	mov	r0, #0	; 0x0
    2574:	ebfff905 	bl	990 <efuse_info>
    2578:	e3a00000 	mov	r0, #0	; 0x0
    257c:	e28dd004 	add	sp, sp, #4	; 0x4
    2580:	e8bd8000 	pop	{pc}
    2584:	00010fd4 	.word	0x00010fd4

00002588 <do_pi>:
    2588:	e92d4030 	push	{r4, r5, lr}
    258c:	e3520001 	cmp	r2, #1	; 0x1
    2590:	e24dd004 	sub	sp, sp, #4	; 0x4
    2594:	e1a04002 	mov	r4, r2
    2598:	e1a05003 	mov	r5, r3
    259c:	da000015 	ble	25f8 <do_pi+0x70>
    25a0:	e59f10ac 	ldr	r1, [pc, #172]	; 2654 <do_pi+0xcc>
    25a4:	e5930004 	ldr	r0, [r3, #4]
    25a8:	e3a02004 	mov	r2, #4	; 0x4
    25ac:	ebfffb50 	bl	12f4 <strncmp>
    25b0:	e2501000 	subs	r1, r0, #0	; 0x0
    25b4:	1a00000d 	bne	25f0 <do_pi+0x68>
    25b8:	e3540002 	cmp	r4, #2	; 0x2
    25bc:	01a04001 	moveq	r4, r1
    25c0:	0a000003 	beq	25d4 <do_pi+0x4c>
    25c4:	e5950008 	ldr	r0, [r5, #8]
    25c8:	e3a02010 	mov	r2, #16	; 0x10
    25cc:	eb0026bd 	bl	c0c8 <simple_strtoul>
    25d0:	e1a04000 	mov	r4, r0
    25d4:	e59f007c 	ldr	r0, [pc, #124]	; 2658 <do_pi+0xd0>
    25d8:	e1a01004 	mov	r1, r4
    25dc:	e59f3078 	ldr	r3, [pc, #120]	; 265c <do_pi+0xd4>
    25e0:	e1a0e00f 	mov	lr, pc
    25e4:	e593f000 	ldr	pc, [r3]
    25e8:	e1a00004 	mov	r0, r4
    25ec:	eb001097 	bl	6850 <burn>
    25f0:	e3a04000 	mov	r4, #0	; 0x0
    25f4:	ea000013 	b	2648 <do_pi+0xc0>
    25f8:	e3a00441 	mov	r0, #1090519040	; 0x41000000
    25fc:	eb002a45 	bl	cf18 <ius_check_header>
    2600:	e2504000 	subs	r4, r0, #0	; 0x0
    2604:	159f0054 	ldrne	r0, [pc, #84]	; 2660 <do_pi+0xd8>
    2608:	1a000005 	bne	2624 <do_pi+0x9c>
    260c:	e3a0000d 	mov	r0, #13	; 0xd
    2610:	e3a01001 	mov	r1, #1	; 0x1
    2614:	eb0009cc 	bl	4d4c <vs_assign_by_id>
    2618:	e2504000 	subs	r4, r0, #0	; 0x0
    261c:	0a000005 	beq	2638 <do_pi+0xb0>
    2620:	e59f003c 	ldr	r0, [pc, #60]	; 2664 <do_pi+0xdc>
    2624:	e1a01004 	mov	r1, r4
    2628:	e59f302c 	ldr	r3, [pc, #44]	; 265c <do_pi+0xd4>
    262c:	e1a0e00f 	mov	lr, pc
    2630:	e593f000 	ldr	pc, [r3]
    2634:	ea000003 	b	2648 <do_pi+0xc0>
    2638:	e3a00441 	mov	r0, #1090519040	; 0x41000000
    263c:	e3a0100d 	mov	r1, #13	; 0xd
    2640:	e1a02004 	mov	r2, r4
    2644:	eb001008 	bl	666c <burn_images>
    2648:	e1a00004 	mov	r0, r4
    264c:	e28dd004 	add	sp, sp, #4	; 0x4
    2650:	e8bd8030 	pop	{r4, r5, pc}
    2654:	00010fd7 	.word	0x00010fd7
    2658:	00010fdc 	.word	0x00010fdc
    265c:	0800d704 	.word	0x0800d704
    2660:	00011006 	.word	0x00011006
    2664:	00011026 	.word	0x00011026

00002668 <do_spbyte>:
    2668:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    266c:	e5930004 	ldr	r0, [r3, #4]
    2670:	e24dd004 	sub	sp, sp, #4	; 0x4
    2674:	e3a01000 	mov	r1, #0	; 0x0
    2678:	e3a0200a 	mov	r2, #10	; 0xa
    267c:	eb002691 	bl	c0c8 <simple_strtoul>
    2680:	eb001b10 	bl	92c8 <spi_set_bytes>
    2684:	e3a00000 	mov	r0, #0	; 0x0
    2688:	e28dd004 	add	sp, sp, #4	; 0x4
    268c:	e8bd8000 	pop	{pc}

00002690 <do_cfg>:
    2690:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2694:	e5930004 	ldr	r0, [r3, #4]
    2698:	e24dd004 	sub	sp, sp, #4	; 0x4
    269c:	e3a01000 	mov	r1, #0	; 0x0
    26a0:	e3a02010 	mov	r2, #16	; 0x10
    26a4:	eb002687 	bl	c0c8 <simple_strtoul>
    26a8:	eb000f2a 	bl	6358 <cdbg_config_isi>
    26ac:	e3a00000 	mov	r0, #0	; 0x0
    26b0:	e28dd004 	add	sp, sp, #4	; 0x4
    26b4:	e8bd8000 	pop	{pc}

000026b8 <do_sleep>:
    26b8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    26bc:	e3a01000 	mov	r1, #0	; 0x0
    26c0:	e24dd004 	sub	sp, sp, #4	; 0x4
    26c4:	e3a0200a 	mov	r2, #10	; 0xa
    26c8:	e5930004 	ldr	r0, [r3, #4]
    26cc:	eb00267d 	bl	c0c8 <simple_strtoul>
    26d0:	e59f3020 	ldr	r3, [pc, #32]	; 26f8 <do_sleep+0x40>
    26d4:	e0000093 	mul	r0, r3, r0
    26d8:	eb0018ce 	bl	8a18 <udelay>
    26dc:	e59f0018 	ldr	r0, [pc, #24]	; 26fc <do_sleep+0x44>
    26e0:	e59f3018 	ldr	r3, [pc, #24]	; 2700 <do_sleep+0x48>
    26e4:	e1a0e00f 	mov	lr, pc
    26e8:	e593f000 	ldr	pc, [r3]
    26ec:	e3a00000 	mov	r0, #0	; 0x0
    26f0:	e28dd004 	add	sp, sp, #4	; 0x4
    26f4:	e8bd8000 	pop	{pc}
    26f8:	000f4240 	.word	0x000f4240
    26fc:	00011041 	.word	0x00011041
    2700:	0800d704 	.word	0x0800d704

00002704 <do_go>:
    2704:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2708:	e5930004 	ldr	r0, [r3, #4]
    270c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2710:	e3a02010 	mov	r2, #16	; 0x10
    2714:	e3a01000 	mov	r1, #0	; 0x0
    2718:	eb00266a 	bl	c0c8 <simple_strtoul>
    271c:	e3a01000 	mov	r1, #0	; 0x0
    2720:	eb000e97 	bl	6184 <cdbg_jump>
    2724:	e3a00000 	mov	r0, #0	; 0x0
    2728:	e28dd004 	add	sp, sp, #4	; 0x4
    272c:	e8bd8000 	pop	{pc}

00002730 <do_ml>:
    2730:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2734:	e24dd004 	sub	sp, sp, #4	; 0x4
    2738:	ebfffc1c 	bl	17b0 <__sOINFo>
    273c:	e3a00000 	mov	r0, #0	; 0x0
    2740:	e28dd004 	add	sp, sp, #4	; 0x4
    2744:	e8bd8000 	pop	{pc}

00002748 <do_bp>:
    2748:	e92d4030 	push	{r4, r5, lr}
    274c:	e3520001 	cmp	r2, #1	; 0x1
    2750:	e24dd004 	sub	sp, sp, #4	; 0x4
    2754:	da000005 	ble	2770 <do_bp+0x28>
    2758:	e5930004 	ldr	r0, [r3, #4]
    275c:	e3a01000 	mov	r1, #0	; 0x0
    2760:	e3a02010 	mov	r2, #16	; 0x10
    2764:	eb002657 	bl	c0c8 <simple_strtoul>
    2768:	e6ff0070 	uxth	r0, r0
    276c:	ebfff78a 	bl	59c <set_xom>
    2770:	e59f5034 	ldr	r5, [pc, #52]	; 27ac <do_bp+0x64>
    2774:	e3a00001 	mov	r0, #1	; 0x1
    2778:	e5954000 	ldr	r4, [r5]
    277c:	ebfff782 	bl	58c <get_xom>
    2780:	e1a01000 	mov	r1, r0
    2784:	e59f0024 	ldr	r0, [pc, #36]	; 27b0 <do_bp+0x68>
    2788:	e12fff34 	blx	r4
    278c:	e5954000 	ldr	r4, [r5]
    2790:	ebfff788 	bl	5b8 <boot_info>
    2794:	e1a01000 	mov	r1, r0
    2798:	e59f0014 	ldr	r0, [pc, #20]	; 27b4 <do_bp+0x6c>
    279c:	e12fff34 	blx	r4
    27a0:	e3a00000 	mov	r0, #0	; 0x0
    27a4:	e28dd004 	add	sp, sp, #4	; 0x4
    27a8:	e8bd8030 	pop	{r4, r5, pc}
    27ac:	0800d704 	.word	0x0800d704
    27b0:	0001104c 	.word	0x0001104c
    27b4:	00011062 	.word	0x00011062

000027b8 <do_reset>:
    27b8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    27bc:	e24dd004 	sub	sp, sp, #4	; 0x4
    27c0:	ebfff81c 	bl	838 <system_reset>
    27c4:	e3a00000 	mov	r0, #0	; 0x0
    27c8:	e28dd004 	add	sp, sp, #4	; 0x4
    27cc:	e8bd8000 	pop	{pc}

000027d0 <output_debug>:
    27d0:	e24dd010 	sub	sp, sp, #16	; 0x10
    27d4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    27d8:	e24dd004 	sub	sp, sp, #4	; 0x4
    27dc:	e28dc008 	add	ip, sp, #8	; 0x8
    27e0:	e3700002 	cmn	r0, #2	; 0x2
    27e4:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    27e8:	e1a01000 	mov	r1, r0
    27ec:	0a000012 	beq	283c <output_debug+0x6c>
    27f0:	e3700001 	cmn	r0, #1	; 0x1
    27f4:	0a000008 	beq	281c <output_debug+0x4c>
    27f8:	e3700003 	cmn	r0, #3	; 0x3
    27fc:	1a000010 	bne	2844 <output_debug+0x74>
    2800:	e59f30a4 	ldr	r3, [pc, #164]	; 28ac <output_debug+0xdc>
    2804:	e59f00a4 	ldr	r0, [pc, #164]	; 28b0 <output_debug+0xe0>
    2808:	e5931000 	ldr	r1, [r3]
    280c:	e59f30a0 	ldr	r3, [pc, #160]	; 28b4 <output_debug+0xe4>
    2810:	e1a0e00f 	mov	lr, pc
    2814:	e593f000 	ldr	pc, [r3]
    2818:	ea00001e 	b	2898 <output_debug+0xc8>
    281c:	e59f3088 	ldr	r3, [pc, #136]	; 28ac <output_debug+0xdc>
    2820:	e59f0090 	ldr	r0, [pc, #144]	; 28b8 <output_debug+0xe8>
    2824:	e5932000 	ldr	r2, [r3]
    2828:	e28d1010 	add	r1, sp, #16	; 0x10
    282c:	e59f3080 	ldr	r3, [pc, #128]	; 28b4 <output_debug+0xe4>
    2830:	e1a0e00f 	mov	lr, pc
    2834:	e593f000 	ldr	pc, [r3]
    2838:	ea000016 	b	2898 <output_debug+0xc8>
    283c:	e59f0078 	ldr	r0, [pc, #120]	; 28bc <output_debug+0xec>
    2840:	ea000003 	b	2854 <output_debug+0x84>
    2844:	e59d300c 	ldr	r3, [sp, #12]
    2848:	e3530001 	cmp	r3, #1	; 0x1
    284c:	1a000004 	bne	2864 <output_debug+0x94>
    2850:	e59f0068 	ldr	r0, [pc, #104]	; 28c0 <output_debug+0xf0>
    2854:	e59f3058 	ldr	r3, [pc, #88]	; 28b4 <output_debug+0xe4>
    2858:	e1a0e00f 	mov	lr, pc
    285c:	e593f000 	ldr	pc, [r3]
    2860:	ea00000c 	b	2898 <output_debug+0xc8>
    2864:	e3530002 	cmp	r3, #2	; 0x2
    2868:	e59fc044 	ldr	ip, [pc, #68]	; 28b4 <output_debug+0xe4>
    286c:	1a000003 	bne	2880 <output_debug+0xb0>
    2870:	e59f004c 	ldr	r0, [pc, #76]	; 28c4 <output_debug+0xf4>
    2874:	e1a0e00f 	mov	lr, pc
    2878:	e59cf000 	ldr	pc, [ip]
    287c:	ea000005 	b	2898 <output_debug+0xc8>
    2880:	e59f3024 	ldr	r3, [pc, #36]	; 28ac <output_debug+0xdc>
    2884:	e59f003c 	ldr	r0, [pc, #60]	; 28c8 <output_debug+0xf8>
    2888:	e5933000 	ldr	r3, [r3]
    288c:	e28d2010 	add	r2, sp, #16	; 0x10
    2890:	e1a0e00f 	mov	lr, pc
    2894:	e59cf000 	ldr	pc, [ip]
    2898:	e3a00000 	mov	r0, #0	; 0x0
    289c:	e28dd004 	add	sp, sp, #4	; 0x4
    28a0:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    28a4:	e28dd010 	add	sp, sp, #16	; 0x10
    28a8:	e12fff1e 	bx	lr
    28ac:	0800d164 	.word	0x0800d164
    28b0:	000111ea 	.word	0x000111ea
    28b4:	0800d704 	.word	0x0800d704
    28b8:	00011212 	.word	0x00011212
    28bc:	00011221 	.word	0x00011221
    28c0:	0001123b 	.word	0x0001123b
    28c4:	00011246 	.word	0x00011246
    28c8:	00011254 	.word	0x00011254

000028cc <do_vs>:
    28cc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    28d0:	e3520006 	cmp	r2, #6	; 0x6
    28d4:	e24dd024 	sub	sp, sp, #36	; 0x24
    28d8:	e1a06000 	mov	r6, r0
    28dc:	e1a04003 	mov	r4, r3
    28e0:	1a000005 	bne	28fc <do_vs+0x30>
    28e4:	e5930014 	ldr	r0, [r3, #20]
    28e8:	e3a01000 	mov	r1, #0	; 0x0
    28ec:	e282200a 	add	r2, r2, #10	; 0xa
    28f0:	eb0025f4 	bl	c0c8 <simple_strtoul>
    28f4:	e1a05000 	mov	r5, r0
    28f8:	ea00008f 	b	2b3c <do_vs+0x270>
    28fc:	e2423001 	sub	r3, r2, #1	; 0x1
    2900:	e3530005 	cmp	r3, #5	; 0x5
    2904:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    2908:	ea0000c4 	b	2c20 <do_vs+0x354>
    290c:	00002c20 	.word	0x00002c20
    2910:	00002924 	.word	0x00002924
    2914:	00002a1c 	.word	0x00002a1c
    2918:	00002a8c 	.word	0x00002a8c
    291c:	00002b38 	.word	0x00002b38
    2920:	00002b38 	.word	0x00002b38
    2924:	e5940004 	ldr	r0, [r4, #4]
    2928:	e59f1310 	ldr	r1, [pc, #784]	; 2c40 <do_vs+0x374>
    292c:	ebfffa62 	bl	12bc <strcmp>
    2930:	e2505000 	subs	r5, r0, #0	; 0x0
    2934:	1a000025 	bne	29d0 <do_vs+0x104>
    2938:	e59f4304 	ldr	r4, [pc, #772]	; 2c44 <do_vs+0x378>
    293c:	e59f0304 	ldr	r0, [pc, #772]	; 2c48 <do_vs+0x37c>
    2940:	e1a0e00f 	mov	lr, pc
    2944:	e594f000 	ldr	pc, [r4]
    2948:	e3a0802c 	mov	r8, #44	; 0x2c
    294c:	e59f72f8 	ldr	r7, [pc, #760]	; 2c4c <do_vs+0x380>
    2950:	ea000003 	b	2964 <do_vs+0x98>
    2954:	e0227598 	mla	r2, r8, r5, r7
    2958:	e1a0e00f 	mov	lr, pc
    295c:	e594f000 	ldr	pc, [r4]
    2960:	e2855001 	add	r5, r5, #1	; 0x1
    2964:	eb00097e 	bl	4f64 <vs_device_count>
    2968:	e1550000 	cmp	r5, r0
    296c:	e1a01005 	mov	r1, r5
    2970:	e59f02d8 	ldr	r0, [pc, #728]	; 2c50 <do_vs+0x384>
    2974:	e59f62c8 	ldr	r6, [pc, #712]	; 2c44 <do_vs+0x378>
    2978:	bafffff5 	blt	2954 <do_vs+0x88>
    297c:	e59f02d0 	ldr	r0, [pc, #720]	; 2c54 <do_vs+0x388>
    2980:	e1a0e00f 	mov	lr, pc
    2984:	e596f000 	ldr	pc, [r6]
    2988:	eb00084e 	bl	4ac8 <vs_is_assigned>
    298c:	e2504000 	subs	r4, r0, #0	; 0x0
    2990:	0a000009 	beq	29bc <do_vs+0xf0>
    2994:	e59f32bc 	ldr	r3, [pc, #700]	; 2c58 <do_vs+0x38c>
    2998:	e5965000 	ldr	r5, [r6]
    299c:	e5934000 	ldr	r4, [r3]
    29a0:	e1a00004 	mov	r0, r4
    29a4:	eb0008d1 	bl	4cf0 <vs_align>
    29a8:	e1a01004 	mov	r1, r4
    29ac:	e1a02000 	mov	r2, r0
    29b0:	e59f02a4 	ldr	r0, [pc, #676]	; 2c5c <do_vs+0x390>
    29b4:	e12fff35 	blx	r5
    29b8:	ea00009d 	b	2c34 <do_vs+0x368>
    29bc:	e59f029c 	ldr	r0, [pc, #668]	; 2c60 <do_vs+0x394>
    29c0:	e1a0e00f 	mov	lr, pc
    29c4:	e596f000 	ldr	pc, [r6]
    29c8:	e1a00004 	mov	r0, r4
    29cc:	ea000099 	b	2c38 <do_vs+0x36c>
    29d0:	e5940004 	ldr	r0, [r4, #4]
    29d4:	e59f1288 	ldr	r1, [pc, #648]	; 2c64 <do_vs+0x398>
    29d8:	ebfffa37 	bl	12bc <strcmp>
    29dc:	e2505000 	subs	r5, r0, #0	; 0x0
    29e0:	1a000087 	bne	2c04 <do_vs+0x338>
    29e4:	eb000860 	bl	4b6c <vs_reset>
    29e8:	e28d4020 	add	r4, sp, #32	; 0x20
    29ec:	e5240018 	str	r0, [r4, #-24]!
    29f0:	e59f126c 	ldr	r1, [pc, #620]	; 2c64 <do_vs+0x398>
    29f4:	e2840008 	add	r0, r4, #8	; 0x8
    29f8:	ebfffa05 	bl	1214 <strcpy>
    29fc:	e3a03001 	mov	r3, #1	; 0x1
    2a00:	e58d300c 	str	r3, [sp, #12]
    2a04:	e28d3018 	add	r3, sp, #24	; 0x18
    2a08:	e8930003 	ldm	r3, {r0, r1}
    2a0c:	e88d0003 	stm	sp, {r0, r1}
    2a10:	e894000f 	ldm	r4, {r0, r1, r2, r3}
    2a14:	ebffff6d 	bl	27d0 <output_debug>
    2a18:	ea000019 	b	2a84 <do_vs+0x1b8>
    2a1c:	e5940004 	ldr	r0, [r4, #4]
    2a20:	e59f1240 	ldr	r1, [pc, #576]	; 2c68 <do_vs+0x39c>
    2a24:	ebfffa24 	bl	12bc <strcmp>
    2a28:	e2505000 	subs	r5, r0, #0	; 0x0
    2a2c:	1a000074 	bne	2c04 <do_vs+0x338>
    2a30:	e5940008 	ldr	r0, [r4, #8]
    2a34:	e3a01001 	mov	r1, #1	; 0x1
    2a38:	eb00095f 	bl	4fbc <vs_assign_by_name>
    2a3c:	e2507000 	subs	r7, r0, #0	; 0x0
    2a40:	e59f61fc 	ldr	r6, [pc, #508]	; 2c44 <do_vs+0x378>
    2a44:	1a00000b 	bne	2a78 <do_vs+0x1ac>
    2a48:	e5943008 	ldr	r3, [r4, #8]
    2a4c:	e59f2218 	ldr	r2, [pc, #536]	; 2c6c <do_vs+0x3a0>
    2a50:	e1a00003 	mov	r0, r3
    2a54:	e5823000 	str	r3, [r2]
    2a58:	eb000943 	bl	4f6c <vs_device_id>
    2a5c:	e59f31f4 	ldr	r3, [pc, #500]	; 2c58 <do_vs+0x38c>
    2a60:	e5830000 	str	r0, [r3]
    2a64:	e59f0204 	ldr	r0, [pc, #516]	; 2c70 <do_vs+0x3a4>
    2a68:	e1a0e00f 	mov	lr, pc
    2a6c:	e596f000 	ldr	pc, [r6]
    2a70:	e1a00007 	mov	r0, r7
    2a74:	ea00006f 	b	2c38 <do_vs+0x36c>
    2a78:	e59f01f4 	ldr	r0, [pc, #500]	; 2c74 <do_vs+0x3a8>
    2a7c:	e1a0e00f 	mov	lr, pc
    2a80:	e596f000 	ldr	pc, [r6]
    2a84:	e1a00005 	mov	r0, r5
    2a88:	ea00006a 	b	2c38 <do_vs+0x36c>
    2a8c:	e3a02010 	mov	r2, #16	; 0x10
    2a90:	e5940008 	ldr	r0, [r4, #8]
    2a94:	e3a01000 	mov	r1, #0	; 0x0
    2a98:	eb0025e2 	bl	c228 <simple_strtoull>
    2a9c:	e3a02010 	mov	r2, #16	; 0x10
    2aa0:	e1a07000 	mov	r7, r0
    2aa4:	e1a08001 	mov	r8, r1
    2aa8:	e594000c 	ldr	r0, [r4, #12]
    2aac:	e3a01000 	mov	r1, #0	; 0x0
    2ab0:	eb0025dc 	bl	c228 <simple_strtoull>
    2ab4:	e59f11bc 	ldr	r1, [pc, #444]	; 2c78 <do_vs+0x3ac>
    2ab8:	e1a05000 	mov	r5, r0
    2abc:	e28d0010 	add	r0, sp, #16	; 0x10
    2ac0:	ebfff9d3 	bl	1214 <strcpy>
    2ac4:	e3a03002 	mov	r3, #2	; 0x2
    2ac8:	e5940004 	ldr	r0, [r4, #4]
    2acc:	e59f11a4 	ldr	r1, [pc, #420]	; 2c78 <do_vs+0x3ac>
    2ad0:	e58d300c 	str	r3, [sp, #12]
    2ad4:	ebfff9f8 	bl	12bc <strcmp>
    2ad8:	e3500000 	cmp	r0, #0	; 0x0
    2adc:	1a000004 	bne	2af4 <do_vs+0x228>
    2ae0:	e1a00007 	mov	r0, r7
    2ae4:	e1a01008 	mov	r1, r8
    2ae8:	e1a02005 	mov	r2, r5
    2aec:	eb000843 	bl	4c00 <vs_erase>
    2af0:	ea000008 	b	2b18 <do_vs+0x24c>
    2af4:	e5940004 	ldr	r0, [r4, #4]
    2af8:	e59f117c 	ldr	r1, [pc, #380]	; 2c7c <do_vs+0x3b0>
    2afc:	ebfff9ee 	bl	12bc <strcmp>
    2b00:	e3500000 	cmp	r0, #0	; 0x0
    2b04:	1a00003e 	bne	2c04 <do_vs+0x338>
    2b08:	e1a00007 	mov	r0, r7
    2b0c:	e1a01008 	mov	r1, r8
    2b10:	e1a02005 	mov	r2, r5
    2b14:	eb000857 	bl	4c78 <vs_scrub>
    2b18:	e58d0008 	str	r0, [sp, #8]
    2b1c:	e28d2018 	add	r2, sp, #24	; 0x18
    2b20:	e8920003 	ldm	r2, {r0, r1}
    2b24:	e88d0003 	stm	sp, {r0, r1}
    2b28:	e28d3008 	add	r3, sp, #8	; 0x8
    2b2c:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    2b30:	ebffff26 	bl	27d0 <output_debug>
    2b34:	ea00003e 	b	2c34 <do_vs+0x368>
    2b38:	e3a05000 	mov	r5, #0	; 0x0
    2b3c:	e3a01000 	mov	r1, #0	; 0x0
    2b40:	e3a02010 	mov	r2, #16	; 0x10
    2b44:	e5940008 	ldr	r0, [r4, #8]
    2b48:	eb00255e 	bl	c0c8 <simple_strtoul>
    2b4c:	e3a02010 	mov	r2, #16	; 0x10
    2b50:	e1a09000 	mov	r9, r0
    2b54:	e3a01000 	mov	r1, #0	; 0x0
    2b58:	e594000c 	ldr	r0, [r4, #12]
    2b5c:	eb0025b1 	bl	c228 <simple_strtoull>
    2b60:	e3a02010 	mov	r2, #16	; 0x10
    2b64:	e1a08000 	mov	r8, r0
    2b68:	e1a0a001 	mov	sl, r1
    2b6c:	e5940010 	ldr	r0, [r4, #16]
    2b70:	e3a01000 	mov	r1, #0	; 0x0
    2b74:	e08db002 	add	fp, sp, r2
    2b78:	eb0025aa 	bl	c228 <simple_strtoull>
    2b7c:	e3a03000 	mov	r3, #0	; 0x0
    2b80:	e59f10f8 	ldr	r1, [pc, #248]	; 2c80 <do_vs+0x3b4>
    2b84:	e1a07000 	mov	r7, r0
    2b88:	e1a0000b 	mov	r0, fp
    2b8c:	e58d300c 	str	r3, [sp, #12]
    2b90:	ebfff99f 	bl	1214 <strcpy>
    2b94:	e5940004 	ldr	r0, [r4, #4]
    2b98:	e59f10e4 	ldr	r1, [pc, #228]	; 2c84 <do_vs+0x3b8>
    2b9c:	ebfff9c6 	bl	12bc <strcmp>
    2ba0:	e3500000 	cmp	r0, #0	; 0x0
    2ba4:	1a000006 	bne	2bc4 <do_vs+0x2f8>
    2ba8:	e1a00009 	mov	r0, r9
    2bac:	e1a02008 	mov	r2, r8
    2bb0:	e1a0300a 	mov	r3, sl
    2bb4:	e58d7000 	str	r7, [sp]
    2bb8:	e58d5004 	str	r5, [sp, #4]
    2bbc:	eb0007f9 	bl	4ba8 <vs_write>
    2bc0:	eaffffd4 	b	2b18 <do_vs+0x24c>
    2bc4:	e5940004 	ldr	r0, [r4, #4]
    2bc8:	e59f10b8 	ldr	r1, [pc, #184]	; 2c88 <do_vs+0x3bc>
    2bcc:	ebfff9ba 	bl	12bc <strcmp>
    2bd0:	e3500000 	cmp	r0, #0	; 0x0
    2bd4:	1a00000a 	bne	2c04 <do_vs+0x338>
    2bd8:	e1a00009 	mov	r0, r9
    2bdc:	e1a02008 	mov	r2, r8
    2be0:	e1a0300a 	mov	r3, sl
    2be4:	e58d7000 	str	r7, [sp]
    2be8:	e58d5004 	str	r5, [sp, #4]
    2bec:	eb000933 	bl	50c0 <vs_read>
    2bf0:	e59f1094 	ldr	r1, [pc, #148]	; 2c8c <do_vs+0x3c0>
    2bf4:	e58d0008 	str	r0, [sp, #8]
    2bf8:	e1a0000b 	mov	r0, fp
    2bfc:	ebfff984 	bl	1214 <strcpy>
    2c00:	eaffffc5 	b	2b1c <do_vs+0x250>
    2c04:	e59f0084 	ldr	r0, [pc, #132]	; 2c90 <do_vs+0x3c4>
    2c08:	e5961014 	ldr	r1, [r6, #20]
    2c0c:	e59f3030 	ldr	r3, [pc, #48]	; 2c44 <do_vs+0x378>
    2c10:	e1a0e00f 	mov	lr, pc
    2c14:	e593f000 	ldr	pc, [r3]
    2c18:	e3e00000 	mvn	r0, #0	; 0x0
    2c1c:	ea000005 	b	2c38 <do_vs+0x36c>
    2c20:	e5901014 	ldr	r1, [r0, #20]
    2c24:	e59f3018 	ldr	r3, [pc, #24]	; 2c44 <do_vs+0x378>
    2c28:	e59f0060 	ldr	r0, [pc, #96]	; 2c90 <do_vs+0x3c4>
    2c2c:	e1a0e00f 	mov	lr, pc
    2c30:	e593f000 	ldr	pc, [r3]
    2c34:	e3a00000 	mov	r0, #0	; 0x0
    2c38:	e28dd024 	add	sp, sp, #36	; 0x24
    2c3c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    2c40:	00011091 	.word	0x00011091
    2c44:	0800d704 	.word	0x0800d704
    2c48:	0001126b 	.word	0x0001126b
    2c4c:	0000febc 	.word	0x0000febc
    2c50:	00011279 	.word	0x00011279
    2c54:	00011281 	.word	0x00011281
    2c58:	0800d164 	.word	0x0800d164
    2c5c:	00011293 	.word	0x00011293
    2c60:	0001129d 	.word	0x0001129d
    2c64:	0001107f 	.word	0x0001107f
    2c68:	000112b0 	.word	0x000112b0
    2c6c:	0800d168 	.word	0x0800d168
    2c70:	000112b7 	.word	0x000112b7
    2c74:	000112c3 	.word	0x000112c3
    2c78:	000112f0 	.word	0x000112f0
    2c7c:	000112f6 	.word	0x000112f6
    2c80:	000112fc 	.word	0x000112fc
    2c84:	00011305 	.word	0x00011305
    2c88:	0001130b 	.word	0x0001130b
    2c8c:	00011310 	.word	0x00011310
    2c90:	00010c7a 	.word	0x00010c7a

00002c94 <cmd_get_data_size>:
    2c94:	e92d4030 	push	{r4, r5, lr}
    2c98:	e24dd004 	sub	sp, sp, #4	; 0x4
    2c9c:	e1a04001 	mov	r4, r1
    2ca0:	e1a05000 	mov	r5, r0
    2ca4:	ebfff9b1 	bl	1370 <strlen>
    2ca8:	e3500002 	cmp	r0, #2	; 0x2
    2cac:	da000015 	ble	2d08 <cmd_get_data_size+0x74>
    2cb0:	e0850000 	add	r0, r5, r0
    2cb4:	e5503002 	ldrb	r3, [r0, #-2]
    2cb8:	e353002e 	cmp	r3, #46	; 0x2e
    2cbc:	1a000011 	bne	2d08 <cmd_get_data_size+0x74>
    2cc0:	e5500001 	ldrb	r0, [r0, #-1]
    2cc4:	e350006c 	cmp	r0, #108	; 0x6c
    2cc8:	03a04004 	moveq	r4, #4	; 0x4
    2ccc:	0a00000d 	beq	2d08 <cmd_get_data_size+0x74>
    2cd0:	8a000002 	bhi	2ce0 <cmd_get_data_size+0x4c>
    2cd4:	e3500062 	cmp	r0, #98	; 0x62
    2cd8:	1a000009 	bne	2d04 <cmd_get_data_size+0x70>
    2cdc:	ea000006 	b	2cfc <cmd_get_data_size+0x68>
    2ce0:	e3500073 	cmp	r0, #115	; 0x73
    2ce4:	03e04001 	mvneq	r4, #1	; 0x1
    2ce8:	0a000006 	beq	2d08 <cmd_get_data_size+0x74>
    2cec:	e3500077 	cmp	r0, #119	; 0x77
    2cf0:	03a04002 	moveq	r4, #2	; 0x2
    2cf4:	1a000002 	bne	2d04 <cmd_get_data_size+0x70>
    2cf8:	ea000002 	b	2d08 <cmd_get_data_size+0x74>
    2cfc:	e3a04001 	mov	r4, #1	; 0x1
    2d00:	ea000000 	b	2d08 <cmd_get_data_size+0x74>
    2d04:	e3e04000 	mvn	r4, #0	; 0x0
    2d08:	e1a00004 	mov	r0, r4
    2d0c:	e28dd004 	add	sp, sp, #4	; 0x4
    2d10:	e8bd8030 	pop	{r4, r5, pc}

00002d14 <cmd_usage>:
    2d14:	e92d4030 	push	{r4, r5, lr}
    2d18:	e1a04000 	mov	r4, r0
    2d1c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2d20:	e59f5054 	ldr	r5, [pc, #84]	; 2d7c <cmd_usage+0x68>
    2d24:	e59f0054 	ldr	r0, [pc, #84]	; 2d80 <cmd_usage+0x6c>
    2d28:	e5941000 	ldr	r1, [r4]
    2d2c:	e5942010 	ldr	r2, [r4, #16]
    2d30:	e1a0e00f 	mov	lr, pc
    2d34:	e595f000 	ldr	pc, [r5]
    2d38:	e59f0044 	ldr	r0, [pc, #68]	; 2d84 <cmd_usage+0x70>
    2d3c:	e5941000 	ldr	r1, [r4]
    2d40:	e1a0e00f 	mov	lr, pc
    2d44:	e595f000 	ldr	pc, [r5]
    2d48:	e5940014 	ldr	r0, [r4, #20]
    2d4c:	e3500000 	cmp	r0, #0	; 0x0
    2d50:	1a000003 	bne	2d64 <cmd_usage+0x50>
    2d54:	e59f002c 	ldr	r0, [pc, #44]	; 2d88 <cmd_usage+0x74>
    2d58:	eb0028c8 	bl	d080 <puts>
    2d5c:	e3a00001 	mov	r0, #1	; 0x1
    2d60:	ea000003 	b	2d74 <cmd_usage+0x60>
    2d64:	eb0028c5 	bl	d080 <puts>
    2d68:	e3a0000a 	mov	r0, #10	; 0xa
    2d6c:	eb0028eb 	bl	d120 <putc>
    2d70:	e3a00000 	mov	r0, #0	; 0x0
    2d74:	e28dd004 	add	sp, sp, #4	; 0x4
    2d78:	e8bd8030 	pop	{r4, r5, pc}
    2d7c:	0800d704 	.word	0x0800d704
    2d80:	00011405 	.word	0x00011405
    2d84:	0001140f 	.word	0x0001140f
    2d88:	0001141a 	.word	0x0001141a

00002d8c <find_cmd_tbl>:
    2d8c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    2d90:	e1a05001 	mov	r5, r1
    2d94:	e24dd004 	sub	sp, sp, #4	; 0x4
    2d98:	e3a0102e 	mov	r1, #46	; 0x2e
    2d9c:	e1a07000 	mov	r7, r0
    2da0:	e1a08002 	mov	r8, r2
    2da4:	ebfff966 	bl	1344 <strchr>
    2da8:	e3500000 	cmp	r0, #0	; 0x0
    2dac:	10676000 	rsbne	r6, r7, r0
    2db0:	1a000002 	bne	2dc0 <find_cmd_tbl+0x34>
    2db4:	e1a00007 	mov	r0, r7
    2db8:	ebfff96c 	bl	1370 <strlen>
    2dbc:	e1a06000 	mov	r6, r0
    2dc0:	e3a03018 	mov	r3, #24	; 0x18
    2dc4:	e02a5893 	mla	sl, r3, r8, r5
    2dc8:	e1a04005 	mov	r4, r5
    2dcc:	e1a08005 	mov	r8, r5
    2dd0:	e3a05000 	mov	r5, #0	; 0x0
    2dd4:	ea00000a 	b	2e04 <find_cmd_tbl+0x78>
    2dd8:	e5941000 	ldr	r1, [r4]
    2ddc:	ebfff944 	bl	12f4 <strncmp>
    2de0:	e3500000 	cmp	r0, #0	; 0x0
    2de4:	1a000005 	bne	2e00 <find_cmd_tbl+0x74>
    2de8:	e5940000 	ldr	r0, [r4]
    2dec:	ebfff95f 	bl	1370 <strlen>
    2df0:	e1560000 	cmp	r6, r0
    2df4:	0a000009 	beq	2e20 <find_cmd_tbl+0x94>
    2df8:	e2855001 	add	r5, r5, #1	; 0x1
    2dfc:	e1a08004 	mov	r8, r4
    2e00:	e2844018 	add	r4, r4, #24	; 0x18
    2e04:	e154000a 	cmp	r4, sl
    2e08:	e1a02006 	mov	r2, r6
    2e0c:	e1a00007 	mov	r0, r7
    2e10:	1afffff0 	bne	2dd8 <find_cmd_tbl+0x4c>
    2e14:	e3550001 	cmp	r5, #1	; 0x1
    2e18:	01a04008 	moveq	r4, r8
    2e1c:	13a04000 	movne	r4, #0	; 0x0
    2e20:	e1a00004 	mov	r0, r4
    2e24:	e28dd004 	add	sp, sp, #4	; 0x4
    2e28:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00002e2c <find_cmd>:
    2e2c:	e59f1014 	ldr	r1, [pc, #20]	; 2e48 <find_cmd+0x1c>
    2e30:	e59f2014 	ldr	r2, [pc, #20]	; 2e4c <find_cmd+0x20>
    2e34:	e59f3014 	ldr	r3, [pc, #20]	; 2e50 <find_cmd+0x24>
    2e38:	e0612002 	rsb	r2, r1, r2
    2e3c:	e1a021c2 	asr	r2, r2, #3
    2e40:	e0020293 	mul	r2, r3, r2
    2e44:	eaffffd0 	b	2d8c <find_cmd_tbl>
    2e48:	00012708 	.word	0x00012708
    2e4c:	000129d8 	.word	0x000129d8
    2e50:	aaaaaaab 	.word	0xaaaaaaab

00002e54 <_do_help>:
    2e54:	e1a0c00d 	mov	ip, sp
    2e58:	e92ddff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}
    2e5c:	e24cb004 	sub	fp, ip, #4	; 0x4
    2e60:	e24dd004 	sub	sp, sp, #4	; 0x4
    2e64:	e59b8004 	ldr	r8, [fp, #4]
    2e68:	e1a07000 	mov	r7, r0
    2e6c:	e3580001 	cmp	r8, #1	; 0x1
    2e70:	e1a0a001 	mov	sl, r1
    2e74:	159b5008 	ldrne	r5, [fp, #8]
    2e78:	13a06001 	movne	r6, #1	; 0x1
    2e7c:	13a04000 	movne	r4, #0	; 0x0
    2e80:	159f9158 	ldrne	r9, [pc, #344]	; 2fe0 <_do_help+0x18c>
    2e84:	1a00004e 	bne	2fc4 <_do_help+0x170>
    2e88:	e1a03101 	lsl	r3, r1, #2
    2e8c:	e283300e 	add	r3, r3, #14	; 0xe
    2e90:	e3c33007 	bic	r3, r3, #7	; 0x7
    2e94:	e1a0900d 	mov	r9, sp
    2e98:	e063d00d 	rsb	sp, r3, sp
    2e9c:	e1a0800d 	mov	r8, sp
    2ea0:	e3a03000 	mov	r3, #0	; 0x0
    2ea4:	ea000002 	b	2eb4 <_do_help+0x60>
    2ea8:	e7880103 	str	r0, [r8, r3, lsl #2]
    2eac:	e2800018 	add	r0, r0, #24	; 0x18
    2eb0:	e2833001 	add	r3, r3, #1	; 0x1
    2eb4:	e153000a 	cmp	r3, sl
    2eb8:	bafffffa 	blt	2ea8 <_do_help+0x54>
    2ebc:	e24a7001 	sub	r7, sl, #1	; 0x1
    2ec0:	ea000015 	b	2f1c <_do_help+0xc8>
    2ec4:	e5943000 	ldr	r3, [r4]
    2ec8:	e5942004 	ldr	r2, [r4, #4]
    2ecc:	e5930000 	ldr	r0, [r3]
    2ed0:	e5921000 	ldr	r1, [r2]
    2ed4:	ebfff8f8 	bl	12bc <strcmp>
    2ed8:	e3500000 	cmp	r0, #0	; 0x0
    2edc:	c5942000 	ldrgt	r2, [r4]
    2ee0:	c5943004 	ldrgt	r3, [r4, #4]
    2ee4:	e2855001 	add	r5, r5, #1	; 0x1
    2ee8:	c5843000 	strgt	r3, [r4]
    2eec:	c5842004 	strgt	r2, [r4, #4]
    2ef0:	c2866001 	addgt	r6, r6, #1	; 0x1
    2ef4:	e1550007 	cmp	r5, r7
    2ef8:	e2844004 	add	r4, r4, #4	; 0x4
    2efc:	bafffff0 	blt	2ec4 <_do_help+0x70>
    2f00:	e3560000 	cmp	r6, #0	; 0x0
    2f04:	1a000003 	bne	2f18 <_do_help+0xc4>
    2f08:	e3a05000 	mov	r5, #0	; 0x0
    2f0c:	e1a04005 	mov	r4, r5
    2f10:	e59f70c8 	ldr	r7, [pc, #200]	; 2fe0 <_do_help+0x18c>
    2f14:	ea000016 	b	2f74 <_do_help+0x120>
    2f18:	e2477001 	sub	r7, r7, #1	; 0x1
    2f1c:	e3570000 	cmp	r7, #0	; 0x0
    2f20:	dafffff8 	ble	2f08 <_do_help+0xb4>
    2f24:	e3a05000 	mov	r5, #0	; 0x0
    2f28:	e1a04008 	mov	r4, r8
    2f2c:	e1a06005 	mov	r6, r5
    2f30:	eaffffe3 	b	2ec4 <_do_help+0x70>
    2f34:	e7983004 	ldr	r3, [r8, r4]
    2f38:	e5936010 	ldr	r6, [r3, #16]
    2f3c:	eb00287a 	bl	d12c <ctrlc>
    2f40:	e3500000 	cmp	r0, #0	; 0x0
    2f44:	e1a03006 	mov	r3, r6
    2f48:	e3a01008 	mov	r1, #8	; 0x8
    2f4c:	e59f0090 	ldr	r0, [pc, #144]	; 2fe4 <_do_help+0x190>
    2f50:	13a04001 	movne	r4, #1	; 0x1
    2f54:	1a00000a 	bne	2f84 <_do_help+0x130>
    2f58:	e3560000 	cmp	r6, #0	; 0x0
    2f5c:	0a000003 	beq	2f70 <_do_help+0x11c>
    2f60:	e7982004 	ldr	r2, [r8, r4]
    2f64:	e5922000 	ldr	r2, [r2]
    2f68:	e1a0e00f 	mov	lr, pc
    2f6c:	e597f000 	ldr	pc, [r7]
    2f70:	e2844004 	add	r4, r4, #4	; 0x4
    2f74:	e155000a 	cmp	r5, sl
    2f78:	e2855001 	add	r5, r5, #1	; 0x1
    2f7c:	baffffec 	blt	2f34 <_do_help+0xe0>
    2f80:	e3a04000 	mov	r4, #0	; 0x0
    2f84:	e1a0d009 	mov	sp, r9
    2f88:	ea000011 	b	2fd4 <_do_help+0x180>
    2f8c:	e5950004 	ldr	r0, [r5, #4]
    2f90:	ebffff7d 	bl	2d8c <find_cmd_tbl>
    2f94:	e3500000 	cmp	r0, #0	; 0x0
    2f98:	0a000002 	beq	2fa8 <_do_help+0x154>
    2f9c:	ebffff5c 	bl	2d14 <cmd_usage>
    2fa0:	e1844000 	orr	r4, r4, r0
    2fa4:	ea000004 	b	2fbc <_do_help+0x168>
    2fa8:	e5951004 	ldr	r1, [r5, #4]
    2fac:	e59f0034 	ldr	r0, [pc, #52]	; 2fe8 <_do_help+0x194>
    2fb0:	e1a0e00f 	mov	lr, pc
    2fb4:	e599f000 	ldr	pc, [r9]
    2fb8:	e3a04001 	mov	r4, #1	; 0x1
    2fbc:	e2866001 	add	r6, r6, #1	; 0x1
    2fc0:	e2855004 	add	r5, r5, #4	; 0x4
    2fc4:	e1560008 	cmp	r6, r8
    2fc8:	e1a01007 	mov	r1, r7
    2fcc:	e1a0200a 	mov	r2, sl
    2fd0:	baffffed 	blt	2f8c <_do_help+0x138>
    2fd4:	e1a00004 	mov	r0, r4
    2fd8:	e24bd028 	sub	sp, fp, #40	; 0x28
    2fdc:	e89daff0 	ldm	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
    2fe0:	0800d704 	.word	0x0800d704
    2fe4:	0001143b 	.word	0x0001143b
    2fe8:	00011445 	.word	0x00011445

00002fec <do_help>:
    2fec:	e92d4030 	push	{r4, r5, lr}
    2ff0:	e59fc038 	ldr	ip, [pc, #56]	; 3030 <do_help+0x44>
    2ff4:	e59fe038 	ldr	lr, [pc, #56]	; 3034 <do_help+0x48>
    2ff8:	e1a05001 	mov	r5, r1
    2ffc:	e06ec00c 	rsb	ip, lr, ip
    3000:	e59f1030 	ldr	r1, [pc, #48]	; 3038 <do_help+0x4c>
    3004:	e24dd00c 	sub	sp, sp, #12	; 0xc
    3008:	e1a04000 	mov	r4, r0
    300c:	e1a0c1cc 	asr	ip, ip, #3
    3010:	e88d000c 	stm	sp, {r2, r3}
    3014:	e0010c91 	mul	r1, r1, ip
    3018:	e1a0000e 	mov	r0, lr
    301c:	e1a02004 	mov	r2, r4
    3020:	e1a03005 	mov	r3, r5
    3024:	ebffff8a 	bl	2e54 <_do_help>
    3028:	e28dd00c 	add	sp, sp, #12	; 0xc
    302c:	e8bd8030 	pop	{r4, r5, pc}
    3030:	000129d8 	.word	0x000129d8
    3034:	00012708 	.word	0x00012708
    3038:	aaaaaaab 	.word	0xaaaaaaab

0000303c <tt_blk_trolling>:
    303c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3040:	e3a01001 	mov	r1, #1	; 0x1
    3044:	e24dd02c 	sub	sp, sp, #44	; 0x2c
    3048:	e1a04002 	mov	r4, r2
    304c:	e1a05003 	mov	r5, r3
    3050:	e58d0010 	str	r0, [sp, #16]
    3054:	e59da050 	ldr	sl, [sp, #80]
    3058:	eb0007d7 	bl	4fbc <vs_assign_by_name>
    305c:	e3500000 	cmp	r0, #0	; 0x0
    3060:	0a000005 	beq	307c <tt_blk_trolling+0x40>
    3064:	e59d1010 	ldr	r1, [sp, #16]
    3068:	e59f03d4 	ldr	r0, [pc, #980]	; 3444 <tt_blk_trolling+0x408>
    306c:	e59f33d4 	ldr	r3, [pc, #980]	; 3448 <tt_blk_trolling+0x40c>
    3070:	e1a0e00f 	mov	lr, pc
    3074:	e593f000 	ldr	pc, [r3]
    3078:	ea000026 	b	3118 <tt_blk_trolling+0xdc>
    307c:	e1940005 	orrs	r0, r4, r5
    3080:	03a04502 	moveq	r4, #8388608	; 0x800000
    3084:	03a06502 	moveq	r6, #8388608	; 0x800000
    3088:	03a07000 	moveq	r7, #0	; 0x0
    308c:	0a00000b 	beq	30c0 <tt_blk_trolling+0x84>
    3090:	e3550000 	cmp	r5, #0	; 0x0
    3094:	1a000006 	bne	30b4 <tt_blk_trolling+0x78>
    3098:	e3540502 	cmp	r4, #8388608	; 0x800000
    309c:	2a000004 	bcs	30b4 <tt_blk_trolling+0x78>
    30a0:	e3a0000e 	mov	r0, #14	; 0xe
    30a4:	eb000711 	bl	4cf0 <vs_align>
    30a8:	e1a06004 	mov	r6, r4
    30ac:	e1a07005 	mov	r7, r5
    30b0:	ea000002 	b	30c0 <tt_blk_trolling+0x84>
    30b4:	e1a06004 	mov	r6, r4
    30b8:	e1a07005 	mov	r7, r5
    30bc:	e3a04502 	mov	r4, #8388608	; 0x800000
    30c0:	e3a0000e 	mov	r0, #14	; 0xe
    30c4:	eb000709 	bl	4cf0 <vs_align>
    30c8:	e1a01000 	mov	r1, r0
    30cc:	e1a00004 	mov	r0, r4
    30d0:	eb003000 	bl	f0d8 <__aeabi_uidiv>
    30d4:	e1a01000 	mov	r1, r0
    30d8:	e3a00000 	mov	r0, #0	; 0x0
    30dc:	eb000514 	bl	4534 <tt_rnd_get_int>
    30e0:	e1a04000 	mov	r4, r0
    30e4:	e3a0000e 	mov	r0, #14	; 0xe
    30e8:	eb000700 	bl	4cf0 <vs_align>
    30ec:	e0040094 	mul	r4, r4, r0
    30f0:	e28d0024 	add	r0, sp, #36	; 0x24
    30f4:	e1a01004 	mov	r1, r4
    30f8:	e1a02004 	mov	r2, r4
    30fc:	eb000535 	bl	45d8 <tt_rnd_get_data>
    3100:	e250b000 	subs	fp, r0, #0	; 0x0
    3104:	1a000005 	bne	3120 <tt_blk_trolling+0xe4>
    3108:	e59f033c 	ldr	r0, [pc, #828]	; 344c <tt_blk_trolling+0x410>
    310c:	e59f3334 	ldr	r3, [pc, #820]	; 3448 <tt_blk_trolling+0x40c>
    3110:	e1a0e00f 	mov	lr, pc
    3114:	e593f000 	ldr	pc, [r3]
    3118:	e3e00000 	mvn	r0, #0	; 0x0
    311c:	ea0000c6 	b	343c <tt_blk_trolling+0x400>
    3120:	e59d3024 	ldr	r3, [sp, #36]
    3124:	e3a0000e 	mov	r0, #14	; 0xe
    3128:	e0564003 	subs	r4, r6, r3
    312c:	e0c75fc3 	sbc	r5, r7, r3, asr #31
    3130:	eb0006ee 	bl	4cf0 <vs_align>
    3134:	e3550000 	cmp	r5, #0	; 0x0
    3138:	e1cd41f8 	strd	r4, [sp, #24]
    313c:	1a000005 	bne	3158 <tt_blk_trolling+0x11c>
    3140:	e1a01000 	mov	r1, r0
    3144:	e1a00004 	mov	r0, r4
    3148:	eb002fe2 	bl	f0d8 <__aeabi_uidiv>
    314c:	e58d501c 	str	r5, [sp, #28]
    3150:	e58d0018 	str	r0, [sp, #24]
    3154:	ea000002 	b	3164 <tt_blk_trolling+0x128>
    3158:	e1a01000 	mov	r1, r0
    315c:	e28d0018 	add	r0, sp, #24	; 0x18
    3160:	eb00239b 	bl	bfd4 <__div64_32>
    3164:	e59d1018 	ldr	r1, [sp, #24]
    3168:	e3a00000 	mov	r0, #0	; 0x0
    316c:	eb0004f0 	bl	4534 <tt_rnd_get_int>
    3170:	e1a04000 	mov	r4, r0
    3174:	e3a0000e 	mov	r0, #14	; 0xe
    3178:	eb0006dc 	bl	4cf0 <vs_align>
    317c:	e0c98490 	smull	r8, r9, r0, r4
    3180:	e59d1010 	ldr	r1, [sp, #16]
    3184:	e1a0200b 	mov	r2, fp
    3188:	e59d3024 	ldr	r3, [sp, #36]
    318c:	e59f02bc 	ldr	r0, [pc, #700]	; 3450 <tt_blk_trolling+0x414>
    3190:	e88d0300 	stm	sp, {r8, r9}
    3194:	e59f42ac 	ldr	r4, [pc, #684]	; 3448 <tt_blk_trolling+0x40c>
    3198:	e1a0e00f 	mov	lr, pc
    319c:	e594f000 	ldr	pc, [r4]
    31a0:	e59d0010 	ldr	r0, [sp, #16]
    31a4:	eb000770 	bl	4f6c <vs_device_id>
    31a8:	eb00075b 	bl	4f1c <vs_device_erasable>
    31ac:	e3500000 	cmp	r0, #0	; 0x0
    31b0:	0a000006 	beq	31d0 <tt_blk_trolling+0x194>
    31b4:	e59f0298 	ldr	r0, [pc, #664]	; 3454 <tt_blk_trolling+0x418>
    31b8:	e1a0e00f 	mov	lr, pc
    31bc:	e594f000 	ldr	pc, [r4]
    31c0:	e1a00008 	mov	r0, r8
    31c4:	e1a01009 	mov	r1, r9
    31c8:	e59d2024 	ldr	r2, [sp, #36]
    31cc:	eb00068b 	bl	4c00 <vs_erase>
    31d0:	e59f0280 	ldr	r0, [pc, #640]	; 3458 <tt_blk_trolling+0x41c>
    31d4:	e59f326c 	ldr	r3, [pc, #620]	; 3448 <tt_blk_trolling+0x40c>
    31d8:	e1a0e00f 	mov	lr, pc
    31dc:	e593f000 	ldr	pc, [r3]
    31e0:	e35a0000 	cmp	sl, #0	; 0x0
    31e4:	13a07000 	movne	r7, #0	; 0x0
    31e8:	159d6024 	ldrne	r6, [sp, #36]
    31ec:	1a00001c 	bne	3264 <tt_blk_trolling+0x228>
    31f0:	ea00001f 	b	3274 <tt_blk_trolling+0x238>
    31f4:	e3560902 	cmp	r6, #32768	; 0x8000
    31f8:	b1a01006 	movlt	r1, r6
    31fc:	a3a01902 	movge	r1, #32768	; 0x8000
    3200:	e58d100c 	str	r1, [sp, #12]
    3204:	e59d1024 	ldr	r1, [sp, #36]
    3208:	e59d200c 	ldr	r2, [sp, #12]
    320c:	e0661001 	rsb	r1, r6, r1
    3210:	e08b1001 	add	r1, fp, r1
    3214:	ebfff86f 	bl	13d8 <memcpy>
    3218:	e3a00000 	mov	r0, #0	; 0x0
    321c:	eb001609 	bl	8a48 <get_timer>
    3220:	e59d3024 	ldr	r3, [sp, #36]
    3224:	e3a01000 	mov	r1, #0	; 0x0
    3228:	e0663003 	rsb	r3, r6, r3
    322c:	e0984003 	adds	r4, r8, r3
    3230:	e0a95fc3 	adc	r5, r9, r3, asr #31
    3234:	e1a03005 	mov	r3, r5
    3238:	e59d500c 	ldr	r5, [sp, #12]
    323c:	e58d0008 	str	r0, [sp, #8]
    3240:	e1a02004 	mov	r2, r4
    3244:	e1a0000a 	mov	r0, sl
    3248:	e58d1004 	str	r1, [sp, #4]
    324c:	e58d5000 	str	r5, [sp]
    3250:	eb000654 	bl	4ba8 <vs_write>
    3254:	e59d0008 	ldr	r0, [sp, #8]
    3258:	eb0015fa 	bl	8a48 <get_timer>
    325c:	e0656006 	rsb	r6, r5, r6
    3260:	e0877000 	add	r7, r7, r0
    3264:	e3560000 	cmp	r6, #0	; 0x0
    3268:	e1a0000a 	mov	r0, sl
    326c:	1affffe0 	bne	31f4 <tt_blk_trolling+0x1b8>
    3270:	ea00000b 	b	32a4 <tt_blk_trolling+0x268>
    3274:	e1a0000a 	mov	r0, sl
    3278:	eb0015f2 	bl	8a48 <get_timer>
    327c:	e59d1024 	ldr	r1, [sp, #36]
    3280:	e1a04000 	mov	r4, r0
    3284:	e1a02008 	mov	r2, r8
    3288:	e1a03009 	mov	r3, r9
    328c:	e1a0000b 	mov	r0, fp
    3290:	e88d0402 	stm	sp, {r1, sl}
    3294:	eb000643 	bl	4ba8 <vs_write>
    3298:	e1a00004 	mov	r0, r4
    329c:	eb0015e9 	bl	8a48 <get_timer>
    32a0:	e1a07000 	mov	r7, r0
    32a4:	e59d0024 	ldr	r0, [sp, #36]
    32a8:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    32ac:	e1a00540 	asr	r0, r0, #10
    32b0:	e1a01007 	mov	r1, r7
    32b4:	e0000093 	mul	r0, r3, r0
    32b8:	eb002f86 	bl	f0d8 <__aeabi_uidiv>
    32bc:	e59f3184 	ldr	r3, [pc, #388]	; 3448 <tt_blk_trolling+0x40c>
    32c0:	e58d0014 	str	r0, [sp, #20]
    32c4:	e59f0190 	ldr	r0, [pc, #400]	; 345c <tt_blk_trolling+0x420>
    32c8:	e1a0e00f 	mov	lr, pc
    32cc:	e593f000 	ldr	pc, [r3]
    32d0:	e35a0000 	cmp	sl, #0	; 0x0
    32d4:	13a07000 	movne	r7, #0	; 0x0
    32d8:	159d6024 	ldrne	r6, [sp, #36]
    32dc:	1a00001b 	bne	3350 <tt_blk_trolling+0x314>
    32e0:	ea00001e 	b	3360 <tt_blk_trolling+0x324>
    32e4:	eb0015d7 	bl	8a48 <get_timer>
    32e8:	e59d3024 	ldr	r3, [sp, #36]
    32ec:	e3560902 	cmp	r6, #32768	; 0x8000
    32f0:	b1a04006 	movlt	r4, r6
    32f4:	a3a04902 	movge	r4, #32768	; 0x8000
    32f8:	e0663003 	rsb	r3, r6, r3
    32fc:	e1a05000 	mov	r5, r0
    3300:	e0980003 	adds	r0, r8, r3
    3304:	e0a91fc3 	adc	r1, r9, r3, asr #31
    3308:	e1a02000 	mov	r2, r0
    330c:	e1a03001 	mov	r3, r1
    3310:	e1a0000a 	mov	r0, sl
    3314:	e3a01000 	mov	r1, #0	; 0x0
    3318:	e58d1004 	str	r1, [sp, #4]
    331c:	e58d4000 	str	r4, [sp]
    3320:	eb000766 	bl	50c0 <vs_read>
    3324:	e1a00005 	mov	r0, r5
    3328:	eb0015c6 	bl	8a48 <get_timer>
    332c:	e1a05000 	mov	r5, r0
    3330:	e59d0024 	ldr	r0, [sp, #36]
    3334:	e1a0100a 	mov	r1, sl
    3338:	e0660000 	rsb	r0, r6, r0
    333c:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    3340:	e1a02004 	mov	r2, r4
    3344:	ebfff823 	bl	13d8 <memcpy>
    3348:	e0877005 	add	r7, r7, r5
    334c:	e0646006 	rsb	r6, r4, r6
    3350:	e3a00000 	mov	r0, #0	; 0x0
    3354:	e1560000 	cmp	r6, r0
    3358:	1affffe1 	bne	32e4 <tt_blk_trolling+0x2a8>
    335c:	ea00000b 	b	3390 <tt_blk_trolling+0x354>
    3360:	e1a0000a 	mov	r0, sl
    3364:	eb0015b7 	bl	8a48 <get_timer>
    3368:	e59d1024 	ldr	r1, [sp, #36]
    336c:	e1a04000 	mov	r4, r0
    3370:	e1a02008 	mov	r2, r8
    3374:	e1a03009 	mov	r3, r9
    3378:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    337c:	e88d0402 	stm	sp, {r1, sl}
    3380:	eb00074e 	bl	50c0 <vs_read>
    3384:	e1a00004 	mov	r0, r4
    3388:	eb0015ae 	bl	8a48 <get_timer>
    338c:	e1a07000 	mov	r7, r0
    3390:	e59d0024 	ldr	r0, [sp, #36]
    3394:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    3398:	e1a00540 	asr	r0, r0, #10
    339c:	e0000093 	mul	r0, r3, r0
    33a0:	e1a01007 	mov	r1, r7
    33a4:	eb002f4b 	bl	f0d8 <__aeabi_uidiv>
    33a8:	e59f4098 	ldr	r4, [pc, #152]	; 3448 <tt_blk_trolling+0x40c>
    33ac:	e1a05000 	mov	r5, r0
    33b0:	e59f00a8 	ldr	r0, [pc, #168]	; 3460 <tt_blk_trolling+0x424>
    33b4:	e1a0e00f 	mov	lr, pc
    33b8:	e594f000 	ldr	pc, [r4]
    33bc:	e1a0000b 	mov	r0, fp
    33c0:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    33c4:	e59d2024 	ldr	r2, [sp, #36]
    33c8:	e3a0300c 	mov	r3, #12	; 0xc
    33cc:	eb000404 	bl	43e4 <tt_cmp_buffer>
    33d0:	e3500000 	cmp	r0, #0	; 0x0
    33d4:	0a000007 	beq	33f8 <tt_blk_trolling+0x3bc>
    33d8:	e58d0000 	str	r0, [sp]
    33dc:	e59d1010 	ldr	r1, [sp, #16]
    33e0:	e59d2014 	ldr	r2, [sp, #20]
    33e4:	e1a03005 	mov	r3, r5
    33e8:	e59f0074 	ldr	r0, [pc, #116]	; 3464 <tt_blk_trolling+0x428>
    33ec:	e1a0e00f 	mov	lr, pc
    33f0:	e594f000 	ldr	pc, [r4]
    33f4:	ea000005 	b	3410 <tt_blk_trolling+0x3d4>
    33f8:	e59d1010 	ldr	r1, [sp, #16]
    33fc:	e59d2014 	ldr	r2, [sp, #20]
    3400:	e1a03005 	mov	r3, r5
    3404:	e59f005c 	ldr	r0, [pc, #92]	; 3468 <tt_blk_trolling+0x42c>
    3408:	e1a0e00f 	mov	lr, pc
    340c:	e594f000 	ldr	pc, [r4]
    3410:	e59f0054 	ldr	r0, [pc, #84]	; 346c <tt_blk_trolling+0x430>
    3414:	e59d1024 	ldr	r1, [sp, #36]
    3418:	e890000c 	ldm	r0, {r2, r3}
    341c:	e0922001 	adds	r2, r2, r1
    3420:	e0a33fc1 	adc	r3, r3, r1, asr #31
    3424:	e880000c 	stm	r0, {r2, r3}
    3428:	e59f0040 	ldr	r0, [pc, #64]	; 3470 <tt_blk_trolling+0x434>
    342c:	e59f3014 	ldr	r3, [pc, #20]	; 3448 <tt_blk_trolling+0x40c>
    3430:	e1a0e00f 	mov	lr, pc
    3434:	e593f000 	ldr	pc, [r3]
    3438:	e3a00000 	mov	r0, #0	; 0x0
    343c:	e28dd02c 	add	sp, sp, #44	; 0x2c
    3440:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    3444:	000115ec 	.word	0x000115ec
    3448:	0800d704 	.word	0x0800d704
    344c:	0001160a 	.word	0x0001160a
    3450:	0001162a 	.word	0x0001162a
    3454:	000110ef 	.word	0x000110ef
    3458:	00010e00 	.word	0x00010e00
    345c:	00012614 	.word	0x00012614
    3460:	0001165e 	.word	0x0001165e
    3464:	00011661 	.word	0x00011661
    3468:	0001169b 	.word	0x0001169b
    346c:	0800d170 	.word	0x0800d170
    3470:	0001140d 	.word	0x0001140d

00003474 <do_tt_blk>:
    3474:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3478:	e3520001 	cmp	r2, #1	; 0x1
    347c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    3480:	e1a05002 	mov	r5, r2
    3484:	e1a07003 	mov	r7, r3
    3488:	ca000005 	bgt	34a4 <do_tt_blk+0x30>
    348c:	e59f01bc 	ldr	r0, [pc, #444]	; 3650 <do_tt_blk+0x1dc>
    3490:	e59f31bc 	ldr	r3, [pc, #444]	; 3654 <do_tt_blk+0x1e0>
    3494:	e1a0e00f 	mov	lr, pc
    3498:	e593f000 	ldr	pc, [r3]
    349c:	e3e00000 	mvn	r0, #0	; 0x0
    34a0:	ea000068 	b	3648 <do_tt_blk+0x1d4>
    34a4:	e3520002 	cmp	r2, #2	; 0x2
    34a8:	03a04000 	moveq	r4, #0	; 0x0
    34ac:	01a06004 	moveq	r6, r4
    34b0:	0a000025 	beq	354c <do_tt_blk+0xd8>
    34b4:	e3a01000 	mov	r1, #0	; 0x0
    34b8:	e3a0200a 	mov	r2, #10	; 0xa
    34bc:	e5930008 	ldr	r0, [r3, #8]
    34c0:	eb002300 	bl	c0c8 <simple_strtoul>
    34c4:	e59f118c 	ldr	r1, [pc, #396]	; 3658 <do_tt_blk+0x1e4>
    34c8:	e1a04000 	mov	r4, r0
    34cc:	e3a02005 	mov	r2, #5	; 0x5
    34d0:	e5970004 	ldr	r0, [r7, #4]
    34d4:	ebfff786 	bl	12f4 <strncmp>
    34d8:	e3500000 	cmp	r0, #0	; 0x0
    34dc:	059f0178 	ldreq	r0, [pc, #376]	; 365c <do_tt_blk+0x1e8>
    34e0:	01a01004 	moveq	r1, r4
    34e4:	059f3168 	ldreq	r3, [pc, #360]	; 3654 <do_tt_blk+0x1e0>
    34e8:	0a000008 	beq	3510 <do_tt_blk+0x9c>
    34ec:	e59f116c 	ldr	r1, [pc, #364]	; 3660 <do_tt_blk+0x1ec>
    34f0:	e5970004 	ldr	r0, [r7, #4]
    34f4:	e3a02003 	mov	r2, #3	; 0x3
    34f8:	ebfff77d 	bl	12f4 <strncmp>
    34fc:	e3500000 	cmp	r0, #0	; 0x0
    3500:	e59f314c 	ldr	r3, [pc, #332]	; 3654 <do_tt_blk+0x1e0>
    3504:	059f0150 	ldreq	r0, [pc, #336]	; 365c <do_tt_blk+0x1e8>
    3508:	159f0154 	ldrne	r0, [pc, #340]	; 3664 <do_tt_blk+0x1f0>
    350c:	e1a01004 	mov	r1, r4
    3510:	e1a0e00f 	mov	lr, pc
    3514:	e593f000 	ldr	pc, [r3]
    3518:	e3550003 	cmp	r5, #3	; 0x3
    351c:	03a06000 	moveq	r6, #0	; 0x0
    3520:	0a000009 	beq	354c <do_tt_blk+0xd8>
    3524:	e3a01000 	mov	r1, #0	; 0x0
    3528:	e597000c 	ldr	r0, [r7, #12]
    352c:	e3a02010 	mov	r2, #16	; 0x10
    3530:	eb0022e4 	bl	c0c8 <simple_strtoul>
    3534:	e1a06000 	mov	r6, r0
    3538:	e1a01006 	mov	r1, r6
    353c:	e59f0124 	ldr	r0, [pc, #292]	; 3668 <do_tt_blk+0x1f4>
    3540:	e59f310c 	ldr	r3, [pc, #268]	; 3654 <do_tt_blk+0x1e0>
    3544:	e1a0e00f 	mov	lr, pc
    3548:	e593f000 	ldr	pc, [r3]
    354c:	e3a00000 	mov	r0, #0	; 0x0
    3550:	eb00153c 	bl	8a48 <get_timer>
    3554:	e59f1110 	ldr	r1, [pc, #272]	; 366c <do_tt_blk+0x1f8>
    3558:	e3a02000 	mov	r2, #0	; 0x0
    355c:	e3a03000 	mov	r3, #0	; 0x0
    3560:	e881000c 	stm	r1, {r2, r3}
    3564:	e1a09a04 	lsl	r9, r4, #20
    3568:	e2875004 	add	r5, r7, #4	; 0x4
    356c:	e1a08b24 	lsr	r8, r4, #22
    3570:	e1a07504 	lsl	r7, r4, #10
    3574:	e1a0a624 	lsr	sl, r4, #12
    3578:	e1a0b000 	mov	fp, r0
    357c:	e3a04000 	mov	r4, #0	; 0x0
    3580:	e59f30cc 	ldr	r3, [pc, #204]	; 3654 <do_tt_blk+0x1e0>
    3584:	e1a01004 	mov	r1, r4
    3588:	e59f00e0 	ldr	r0, [pc, #224]	; 3670 <do_tt_blk+0x1fc>
    358c:	e1a0e00f 	mov	lr, pc
    3590:	e593f000 	ldr	pc, [r3]
    3594:	e59f10bc 	ldr	r1, [pc, #188]	; 3658 <do_tt_blk+0x1e4>
    3598:	e3a02005 	mov	r2, #5	; 0x5
    359c:	e5950000 	ldr	r0, [r5]
    35a0:	ebfff753 	bl	12f4 <strncmp>
    35a4:	e3500000 	cmp	r0, #0	; 0x0
    35a8:	e3a02003 	mov	r2, #3	; 0x3
    35ac:	e59f10ac 	ldr	r1, [pc, #172]	; 3660 <do_tt_blk+0x1ec>
    35b0:	e1a03008 	mov	r3, r8
    35b4:	05950000 	ldreq	r0, [r5]
    35b8:	01a02007 	moveq	r2, r7
    35bc:	0a000008 	beq	35e4 <do_tt_blk+0x170>
    35c0:	e5950000 	ldr	r0, [r5]
    35c4:	ebfff74a 	bl	12f4 <strncmp>
    35c8:	e3500000 	cmp	r0, #0	; 0x0
    35cc:	05950000 	ldreq	r0, [r5]
    35d0:	15950000 	ldrne	r0, [r5]
    35d4:	e1a02009 	mov	r2, r9
    35d8:	e1a0300a 	mov	r3, sl
    35dc:	01a02007 	moveq	r2, r7
    35e0:	01a03008 	moveq	r3, r8
    35e4:	e58d6000 	str	r6, [sp]
    35e8:	ebfffe93 	bl	303c <tt_blk_trolling>
    35ec:	eb0026cc 	bl	d124 <tstc>
    35f0:	e3500000 	cmp	r0, #0	; 0x0
    35f4:	e2844001 	add	r4, r4, #1	; 0x1
    35f8:	0affffe0 	beq	3580 <do_tt_blk+0x10c>
    35fc:	eb0026c9 	bl	d128 <getc>
    3600:	e3500003 	cmp	r0, #3	; 0x3
    3604:	1affffdd 	bne	3580 <do_tt_blk+0x10c>
    3608:	e1a0000b 	mov	r0, fp
    360c:	eb00150d 	bl	8a48 <get_timer>
    3610:	e59f3054 	ldr	r3, [pc, #84]	; 366c <do_tt_blk+0x1f8>
    3614:	e30e1a60 	movw	r1, #60000	; 0xea60
    3618:	e5934000 	ldr	r4, [r3]
    361c:	e5933004 	ldr	r3, [r3, #4]
    3620:	e1a04a24 	lsr	r4, r4, #20
    3624:	e1844603 	orr	r4, r4, r3, lsl #12
    3628:	eb002eaa 	bl	f0d8 <__aeabi_uidiv>
    362c:	e1a01004 	mov	r1, r4
    3630:	e1a02000 	mov	r2, r0
    3634:	e59f3018 	ldr	r3, [pc, #24]	; 3654 <do_tt_blk+0x1e0>
    3638:	e59f0034 	ldr	r0, [pc, #52]	; 3674 <do_tt_blk+0x200>
    363c:	e1a0e00f 	mov	lr, pc
    3640:	e593f000 	ldr	pc, [r3]
    3644:	e3a00000 	mov	r0, #0	; 0x0
    3648:	e28dd00c 	add	sp, sp, #12	; 0xc
    364c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    3650:	000116c8 	.word	0x000116c8
    3654:	0800d704 	.word	0x0800d704
    3658:	00010aca 	.word	0x00010aca
    365c:	0001170f 	.word	0x0001170f
    3660:	000110c6 	.word	0x000110c6
    3664:	00011733 	.word	0x00011733
    3668:	00011757 	.word	0x00011757
    366c:	0800d170 	.word	0x0800d170
    3670:	0001177a 	.word	0x0001177a
    3674:	00011784 	.word	0x00011784

00003678 <__show_strbuf>:
    3678:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    367c:	e59f4058 	ldr	r4, [pc, #88]	; 36dc <__show_strbuf+0x64>
    3680:	e24dd004 	sub	sp, sp, #4	; 0x4
    3684:	e1a06001 	mov	r6, r1
    3688:	e1a01000 	mov	r1, r0
    368c:	e59f004c 	ldr	r0, [pc, #76]	; 36e0 <__show_strbuf+0x68>
    3690:	e1a05002 	mov	r5, r2
    3694:	e1a07004 	mov	r7, r4
    3698:	e1a0e00f 	mov	lr, pc
    369c:	e594f000 	ldr	pc, [r4]
    36a0:	e3a04000 	mov	r4, #0	; 0x0
    36a4:	ea000003 	b	36b8 <__show_strbuf+0x40>
    36a8:	e7d61004 	ldrb	r1, [r6, r4]
    36ac:	e1a0e00f 	mov	lr, pc
    36b0:	e597f000 	ldr	pc, [r7]
    36b4:	e2844001 	add	r4, r4, #1	; 0x1
    36b8:	e1540005 	cmp	r4, r5
    36bc:	e59f0020 	ldr	r0, [pc, #32]	; 36e4 <__show_strbuf+0x6c>
    36c0:	bafffff8 	blt	36a8 <__show_strbuf+0x30>
    36c4:	e59f001c 	ldr	r0, [pc, #28]	; 36e8 <__show_strbuf+0x70>
    36c8:	e59f300c 	ldr	r3, [pc, #12]	; 36dc <__show_strbuf+0x64>
    36cc:	e1a0e00f 	mov	lr, pc
    36d0:	e593f000 	ldr	pc, [r3]
    36d4:	e28dd004 	add	sp, sp, #4	; 0x4
    36d8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    36dc:	0800d704 	.word	0x0800d704
    36e0:	000117d2 	.word	0x000117d2
    36e4:	00012446 	.word	0x00012446
    36e8:	0001140d 	.word	0x0001140d

000036ec <do_crypto_quick>:
    36ec:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    36f0:	e24dd0f0 	sub	sp, sp, #240	; 0xf0
    36f4:	e28da0e0 	add	sl, sp, #224	; 0xe0
    36f8:	e3a04010 	mov	r4, #16	; 0x10
    36fc:	e28d70c0 	add	r7, sp, #192	; 0xc0
    3700:	e1a02004 	mov	r2, r4
    3704:	e3a09020 	mov	r9, #32	; 0x20
    3708:	e59f11f0 	ldr	r1, [pc, #496]	; 3900 <do_crypto_quick+0x214>
    370c:	e1a0000a 	mov	r0, sl
    3710:	e3a05040 	mov	r5, #64	; 0x40
    3714:	e28d6080 	add	r6, sp, #128	; 0x80
    3718:	ebfff72e 	bl	13d8 <memcpy>
    371c:	e1a02009 	mov	r2, r9
    3720:	e59f11dc 	ldr	r1, [pc, #476]	; 3904 <do_crypto_quick+0x218>
    3724:	e1a00007 	mov	r0, r7
    3728:	ebfff72a 	bl	13d8 <memcpy>
    372c:	e08d8005 	add	r8, sp, r5
    3730:	e1a02005 	mov	r2, r5
    3734:	e59f11cc 	ldr	r1, [pc, #460]	; 3908 <do_crypto_quick+0x21c>
    3738:	e1a00006 	mov	r0, r6
    373c:	ebfff725 	bl	13d8 <memcpy>
    3740:	e1a02005 	mov	r2, r5
    3744:	e59f11c0 	ldr	r1, [pc, #448]	; 390c <do_crypto_quick+0x220>
    3748:	e1a00008 	mov	r0, r8
    374c:	ebfff721 	bl	13d8 <memcpy>
    3750:	e1a02004 	mov	r2, r4
    3754:	e1a0100a 	mov	r1, sl
    3758:	e59f01b0 	ldr	r0, [pc, #432]	; 3910 <do_crypto_quick+0x224>
    375c:	ebffffc5 	bl	3678 <__show_strbuf>
    3760:	e1a02009 	mov	r2, r9
    3764:	e1a01007 	mov	r1, r7
    3768:	e59f01a4 	ldr	r0, [pc, #420]	; 3914 <do_crypto_quick+0x228>
    376c:	ebffffc1 	bl	3678 <__show_strbuf>
    3770:	e1a01006 	mov	r1, r6
    3774:	e1a02005 	mov	r2, r5
    3778:	e59f0198 	ldr	r0, [pc, #408]	; 3918 <do_crypto_quick+0x22c>
    377c:	ebffffbd 	bl	3678 <__show_strbuf>
    3780:	e1a01008 	mov	r1, r8
    3784:	e1a02005 	mov	r2, r5
    3788:	e59f018c 	ldr	r0, [pc, #396]	; 391c <do_crypto_quick+0x230>
    378c:	ebffffb9 	bl	3678 <__show_strbuf>
    3790:	e1a01007 	mov	r1, r7
    3794:	e1a0200a 	mov	r2, sl
    3798:	e3a00001 	mov	r0, #1	; 0x1
    379c:	eb0006fa 	bl	538c <ss_aes_init>
    37a0:	e1a0100d 	mov	r1, sp
    37a4:	e1a02005 	mov	r2, r5
    37a8:	e1a00006 	mov	r0, r6
    37ac:	eb0006e5 	bl	5348 <ss_aes_data>
    37b0:	e1a0100d 	mov	r1, sp
    37b4:	e1a02005 	mov	r2, r5
    37b8:	e59f0160 	ldr	r0, [pc, #352]	; 3920 <do_crypto_quick+0x234>
    37bc:	ebffffad 	bl	3678 <__show_strbuf>
    37c0:	e1a01007 	mov	r1, r7
    37c4:	e1a0200a 	mov	r2, sl
    37c8:	e3a00000 	mov	r0, #0	; 0x0
    37cc:	eb0006ee 	bl	538c <ss_aes_init>
    37d0:	e59f714c 	ldr	r7, [pc, #332]	; 3924 <do_crypto_quick+0x238>
    37d4:	e1a0100d 	mov	r1, sp
    37d8:	e1a02005 	mov	r2, r5
    37dc:	e1a00008 	mov	r0, r8
    37e0:	eb0006d8 	bl	5348 <ss_aes_data>
    37e4:	e1a0100d 	mov	r1, sp
    37e8:	e1a02005 	mov	r2, r5
    37ec:	e59f0134 	ldr	r0, [pc, #308]	; 3928 <do_crypto_quick+0x23c>
    37f0:	ebffffa0 	bl	3678 <__show_strbuf>
    37f4:	e1a02007 	mov	r2, r7
    37f8:	e3a01000 	mov	r1, #0	; 0x0
    37fc:	e3a00001 	mov	r0, #1	; 0x1
    3800:	eb0006e1 	bl	538c <ss_aes_init>
    3804:	e1a0100d 	mov	r1, sp
    3808:	e1a02005 	mov	r2, r5
    380c:	e1a00006 	mov	r0, r6
    3810:	eb0006cc 	bl	5348 <ss_aes_data>
    3814:	e1a0100d 	mov	r1, sp
    3818:	e1a02005 	mov	r2, r5
    381c:	e59f0108 	ldr	r0, [pc, #264]	; 392c <do_crypto_quick+0x240>
    3820:	ebffff94 	bl	3678 <__show_strbuf>
    3824:	e3a00000 	mov	r0, #0	; 0x0
    3828:	e1a02007 	mov	r2, r7
    382c:	e1a01000 	mov	r1, r0
    3830:	eb0006d5 	bl	538c <ss_aes_init>
    3834:	e1a0100d 	mov	r1, sp
    3838:	e1a02005 	mov	r2, r5
    383c:	e1a0000d 	mov	r0, sp
    3840:	eb0006c0 	bl	5348 <ss_aes_data>
    3844:	e1a02005 	mov	r2, r5
    3848:	e1a0100d 	mov	r1, sp
    384c:	e59f00dc 	ldr	r0, [pc, #220]	; 3930 <do_crypto_quick+0x244>
    3850:	ebffff88 	bl	3678 <__show_strbuf>
    3854:	e1a01005 	mov	r1, r5
    3858:	e3a00001 	mov	r0, #1	; 0x1
    385c:	eb0007e8 	bl	5804 <hash_init>
    3860:	e1a01005 	mov	r1, r5
    3864:	e1a00006 	mov	r0, r6
    3868:	eb0007c0 	bl	5770 <hash_data>
    386c:	e1a0000d 	mov	r0, sp
    3870:	eb0007a2 	bl	5700 <hash_value>
    3874:	eb00079a 	bl	56e4 <hash_len>
    3878:	e1a0100d 	mov	r1, sp
    387c:	e1a02000 	mov	r2, r0
    3880:	e59f00ac 	ldr	r0, [pc, #172]	; 3934 <do_crypto_quick+0x248>
    3884:	ebffff7b 	bl	3678 <__show_strbuf>
    3888:	e1a01005 	mov	r1, r5
    388c:	e3a00002 	mov	r0, #2	; 0x2
    3890:	eb0007db 	bl	5804 <hash_init>
    3894:	e1a01005 	mov	r1, r5
    3898:	e1a00006 	mov	r0, r6
    389c:	eb0007b3 	bl	5770 <hash_data>
    38a0:	e1a0000d 	mov	r0, sp
    38a4:	eb000795 	bl	5700 <hash_value>
    38a8:	eb00078d 	bl	56e4 <hash_len>
    38ac:	e1a0100d 	mov	r1, sp
    38b0:	e1a02000 	mov	r2, r0
    38b4:	e59f007c 	ldr	r0, [pc, #124]	; 3938 <do_crypto_quick+0x24c>
    38b8:	ebffff6e 	bl	3678 <__show_strbuf>
    38bc:	e1a01005 	mov	r1, r5
    38c0:	e3a00003 	mov	r0, #3	; 0x3
    38c4:	eb0007ce 	bl	5804 <hash_init>
    38c8:	e1a01005 	mov	r1, r5
    38cc:	e1a00006 	mov	r0, r6
    38d0:	eb0007a6 	bl	5770 <hash_data>
    38d4:	e1a0000d 	mov	r0, sp
    38d8:	eb000788 	bl	5700 <hash_value>
    38dc:	eb000780 	bl	56e4 <hash_len>
    38e0:	e1a0100d 	mov	r1, sp
    38e4:	e1a02000 	mov	r2, r0
    38e8:	e59f004c 	ldr	r0, [pc, #76]	; 393c <do_crypto_quick+0x250>
    38ec:	ebffff61 	bl	3678 <__show_strbuf>
    38f0:	e1a0400d 	mov	r4, sp
    38f4:	e3a00000 	mov	r0, #0	; 0x0
    38f8:	e28dd0f0 	add	sp, sp, #240	; 0xf0
    38fc:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3900:	0000fe84 	.word	0x0000fe84
    3904:	0000fe64 	.word	0x0000fe64
    3908:	0000fe24 	.word	0x0000fe24
    390c:	0000fde4 	.word	0x0000fde4
    3910:	000117d7 	.word	0x000117d7
    3914:	000117da 	.word	0x000117da
    3918:	000117de 	.word	0x000117de
    391c:	000117e9 	.word	0x000117e9
    3920:	000117fa 	.word	0x000117fa
    3924:	0800d0bc 	.word	0x0800d0bc
    3928:	0001180d 	.word	0x0001180d
    392c:	000117f6 	.word	0x000117f6
    3930:	00011809 	.word	0x00011809
    3934:	0001181c 	.word	0x0001181c
    3938:	00011820 	.word	0x00011820
    393c:	00011825 	.word	0x00011825

00003940 <tt_crypto_hash>:
    3940:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    3944:	e3a0544e 	mov	r5, #1308622848	; 0x4e000000
    3948:	e5951000 	ldr	r1, [r5]
    394c:	e59f2220 	ldr	r2, [pc, #544]	; 3b74 <tt_crypto_hash+0x234>
    3950:	e24dd038 	sub	sp, sp, #56	; 0x38
    3954:	e59f321c 	ldr	r3, [pc, #540]	; 3b78 <tt_crypto_hash+0x238>
    3958:	e1510002 	cmp	r1, r2
    395c:	e28dc028 	add	ip, sp, #40	; 0x28
    3960:	e1a06000 	mov	r6, r0
    3964:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    3968:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    396c:	0a000006 	beq	398c <tt_crypto_hash+0x4c>
    3970:	e59f0204 	ldr	r0, [pc, #516]	; 3b7c <tt_crypto_hash+0x23c>
    3974:	e59f3204 	ldr	r3, [pc, #516]	; 3b80 <tt_crypto_hash+0x240>
    3978:	e1a0e00f 	mov	lr, pc
    397c:	e593f000 	ldr	pc, [r3]
    3980:	e59f01fc 	ldr	r0, [pc, #508]	; 3b84 <tt_crypto_hash+0x244>
    3984:	eb001423 	bl	8a18 <udelay>
    3988:	ea000008 	b	39b0 <tt_crypto_hash+0x70>
    398c:	e5951004 	ldr	r1, [r5, #4]
    3990:	e3a04000 	mov	r4, #0	; 0x0
    3994:	e3510003 	cmp	r1, #3	; 0x3
    3998:	e5854000 	str	r4, [r5]
    399c:	e59fc1dc 	ldr	ip, [pc, #476]	; 3b80 <tt_crypto_hash+0x240>
    39a0:	9a000004 	bls	39b8 <tt_crypto_hash+0x78>
    39a4:	e59f01dc 	ldr	r0, [pc, #476]	; 3b88 <tt_crypto_hash+0x248>
    39a8:	e1a0e00f 	mov	lr, pc
    39ac:	e59cf000 	ldr	pc, [ip]
    39b0:	e3e0c000 	mvn	ip, #0	; 0x0
    39b4:	ea00006b 	b	3b68 <tt_crypto_hash+0x228>
    39b8:	e28d2038 	add	r2, sp, #56	; 0x38
    39bc:	e0823101 	add	r3, r2, r1, lsl #2
    39c0:	e5131010 	ldr	r1, [r3, #-16]
    39c4:	e5952008 	ldr	r2, [r5, #8]
    39c8:	e59f01bc 	ldr	r0, [pc, #444]	; 3b8c <tt_crypto_hash+0x24c>
    39cc:	e1a0e00f 	mov	lr, pc
    39d0:	e59cf000 	ldr	pc, [ip]
    39d4:	e9950003 	ldmib	r5, {r0, r1}
    39d8:	eb000789 	bl	5804 <hash_init>
    39dc:	eb000740 	bl	56e4 <hash_len>
    39e0:	e1a03000 	mov	r3, r0
    39e4:	e59f01a4 	ldr	r0, [pc, #420]	; 3b90 <tt_crypto_hash+0x250>
    39e8:	e3a0c020 	mov	ip, #32	; 0x20
    39ec:	e1a01000 	mov	r1, r0
    39f0:	e3a02001 	mov	r2, #1	; 0x1
    39f4:	e58dc000 	str	ip, [sp]
    39f8:	eb00211a 	bl	be68 <print_buffer>
    39fc:	e3560000 	cmp	r6, #0	; 0x0
    3a00:	0a000022 	beq	3a90 <tt_crypto_hash+0x150>
    3a04:	e1a09005 	mov	r9, r5
    3a08:	e1a0a004 	mov	sl, r4
    3a0c:	e5955008 	ldr	r5, [r5, #8]
    3a10:	e2868901 	add	r8, r6, #16384	; 0x4000
    3a14:	ea000019 	b	3a80 <tt_crypto_hash+0x140>
    3a18:	e5991008 	ldr	r1, [r9, #8]
    3a1c:	e3550902 	cmp	r5, #32768	; 0x8000
    3a20:	b1a04005 	movlt	r4, r5
    3a24:	a3a04902 	movge	r4, #32768	; 0x8000
    3a28:	e0651001 	rsb	r1, r5, r1
    3a2c:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3a30:	e2811030 	add	r1, r1, #48	; 0x30
    3a34:	e1a02004 	mov	r2, r4
    3a38:	ebfff666 	bl	13d8 <memcpy>
    3a3c:	e3a00000 	mov	r0, #0	; 0x0
    3a40:	eb001400 	bl	8a48 <get_timer>
    3a44:	e3550901 	cmp	r5, #16384	; 0x4000
    3a48:	b1a01005 	movlt	r1, r5
    3a4c:	a3a01901 	movge	r1, #16384	; 0x4000
    3a50:	e1a07000 	mov	r7, r0
    3a54:	e1a00006 	mov	r0, r6
    3a58:	eb000744 	bl	5770 <hash_data>
    3a5c:	e3540902 	cmp	r4, #32768	; 0x8000
    3a60:	e0645005 	rsb	r5, r4, r5
    3a64:	1a000002 	bne	3a74 <tt_crypto_hash+0x134>
    3a68:	e3a01901 	mov	r1, #16384	; 0x4000
    3a6c:	e1a00008 	mov	r0, r8
    3a70:	eb00073e 	bl	5770 <hash_data>
    3a74:	e1a00007 	mov	r0, r7
    3a78:	eb0013f2 	bl	8a48 <get_timer>
    3a7c:	e08aa000 	add	sl, sl, r0
    3a80:	e3550000 	cmp	r5, #0	; 0x0
    3a84:	e1a00006 	mov	r0, r6
    3a88:	1affffe2 	bne	3a18 <tt_crypto_hash+0xd8>
    3a8c:	ea000008 	b	3ab4 <tt_crypto_hash+0x174>
    3a90:	e1a00006 	mov	r0, r6
    3a94:	eb0013eb 	bl	8a48 <get_timer>
    3a98:	e5951008 	ldr	r1, [r5, #8]
    3a9c:	e1a04000 	mov	r4, r0
    3aa0:	e59f00ec 	ldr	r0, [pc, #236]	; 3b94 <tt_crypto_hash+0x254>
    3aa4:	eb000731 	bl	5770 <hash_data>
    3aa8:	e1a00004 	mov	r0, r4
    3aac:	eb0013e5 	bl	8a48 <get_timer>
    3ab0:	e1a0a000 	mov	sl, r0
    3ab4:	e28d4008 	add	r4, sp, #8	; 0x8
    3ab8:	e3a0644e 	mov	r6, #1308622848	; 0x4e000000
    3abc:	e1a00004 	mov	r0, r4
    3ac0:	eb00070e 	bl	5700 <hash_value>
    3ac4:	e5965008 	ldr	r5, [r6, #8]
    3ac8:	eb000705 	bl	56e4 <hash_len>
    3acc:	e3a0c020 	mov	ip, #32	; 0x20
    3ad0:	e1a03000 	mov	r3, r0
    3ad4:	e1a01004 	mov	r1, r4
    3ad8:	e3a02001 	mov	r2, #1	; 0x1
    3adc:	e1a00004 	mov	r0, r4
    3ae0:	e58dc000 	str	ip, [sp]
    3ae4:	eb0020df 	bl	be68 <print_buffer>
    3ae8:	e5963004 	ldr	r3, [r6, #4]
    3aec:	e28d1038 	add	r1, sp, #56	; 0x38
    3af0:	e0812103 	add	r2, r1, r3, lsl #2
    3af4:	e59f3084 	ldr	r3, [pc, #132]	; 3b80 <tt_crypto_hash+0x240>
    3af8:	e5127010 	ldr	r7, [r2, #-16]
    3afc:	e5938000 	ldr	r8, [r3]
    3b00:	eb0006f7 	bl	56e4 <hash_len>
    3b04:	e1a01004 	mov	r1, r4
    3b08:	e1a02000 	mov	r2, r0
    3b0c:	e59f007c 	ldr	r0, [pc, #124]	; 3b90 <tt_crypto_hash+0x250>
    3b10:	ebfff651 	bl	145c <memcmp>
    3b14:	e1a05525 	lsr	r5, r5, #10
    3b18:	e3500000 	cmp	r0, #0	; 0x0
    3b1c:	e59f3074 	ldr	r3, [pc, #116]	; 3b98 <tt_crypto_hash+0x258>
    3b20:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    3b24:	e1a0100a 	mov	r1, sl
    3b28:	e0000590 	mul	r0, r0, r5
    3b2c:	e59f4068 	ldr	r4, [pc, #104]	; 3b9c <tt_crypto_hash+0x25c>
    3b30:	01a04003 	moveq	r4, r3
    3b34:	eb002d67 	bl	f0d8 <__aeabi_uidiv>
    3b38:	e1a01007 	mov	r1, r7
    3b3c:	e1a03000 	mov	r3, r0
    3b40:	e1a02004 	mov	r2, r4
    3b44:	e59f0054 	ldr	r0, [pc, #84]	; 3ba0 <tt_crypto_hash+0x260>
    3b48:	e12fff38 	blx	r8
    3b4c:	e59f1050 	ldr	r1, [pc, #80]	; 3ba4 <tt_crypto_hash+0x264>
    3b50:	e5960008 	ldr	r0, [r6, #8]
    3b54:	e891000c 	ldm	r1, {r2, r3}
    3b58:	e0922000 	adds	r2, r2, r0
    3b5c:	e2a33000 	adc	r3, r3, #0	; 0x0
    3b60:	e881000c 	stm	r1, {r2, r3}
    3b64:	e3a0c000 	mov	ip, #0	; 0x0
    3b68:	e1a0000c 	mov	r0, ip
    3b6c:	e28dd038 	add	sp, sp, #56	; 0x38
    3b70:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3b74:	99997777 	.word	0x99997777
    3b78:	0000fe94 	.word	0x0000fe94
    3b7c:	0001182c 	.word	0x0001182c
    3b80:	0800d704 	.word	0x0800d704
    3b84:	004c4b40 	.word	0x004c4b40
    3b88:	0001184b 	.word	0x0001184b
    3b8c:	00011866 	.word	0x00011866
    3b90:	4e000010 	.word	0x4e000010
    3b94:	4e000030 	.word	0x4e000030
    3b98:	00011881 	.word	0x00011881
    3b9c:	00011886 	.word	0x00011886
    3ba0:	0001188d 	.word	0x0001188d
    3ba4:	0800d180 	.word	0x0800d180

00003ba8 <tt_crypto_aes>:
    3ba8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3bac:	e24dd014 	sub	sp, sp, #20	; 0x14
    3bb0:	e1a06000 	mov	r6, r0
    3bb4:	e3a01000 	mov	r1, #0	; 0x0
    3bb8:	e28d000c 	add	r0, sp, #12	; 0xc
    3bbc:	e3a02502 	mov	r2, #8388608	; 0x800000
    3bc0:	eb000284 	bl	45d8 <tt_rnd_get_data>
    3bc4:	e250b000 	subs	fp, r0, #0	; 0x0
    3bc8:	e59f4340 	ldr	r4, [pc, #832]	; 3f10 <tt_crypto_aes+0x368>
    3bcc:	059f0340 	ldreq	r0, [pc, #832]	; 3f14 <tt_crypto_aes+0x36c>
    3bd0:	0a00005c 	beq	3d48 <tt_crypto_aes+0x1a0>
    3bd4:	e59d200c 	ldr	r2, [sp, #12]
    3bd8:	e1a0100b 	mov	r1, fp
    3bdc:	e3c2200f 	bic	r2, r2, #15	; 0xf
    3be0:	e1a03002 	mov	r3, r2
    3be4:	e58d200c 	str	r2, [sp, #12]
    3be8:	e59f0328 	ldr	r0, [pc, #808]	; 3f18 <tt_crypto_aes+0x370>
    3bec:	e1a02006 	mov	r2, r6
    3bf0:	e1a0e00f 	mov	lr, pc
    3bf4:	e594f000 	ldr	pc, [r4]
    3bf8:	e59f031c 	ldr	r0, [pc, #796]	; 3f1c <tt_crypto_aes+0x374>
    3bfc:	e1a0e00f 	mov	lr, pc
    3c00:	e594f000 	ldr	pc, [r4]
    3c04:	e1a0100b 	mov	r1, fp
    3c08:	e59d200c 	ldr	r2, [sp, #12]
    3c0c:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3c10:	ebfff5f0 	bl	13d8 <memcpy>
    3c14:	e59f0304 	ldr	r0, [pc, #772]	; 3f20 <tt_crypto_aes+0x378>
    3c18:	e1a0e00f 	mov	lr, pc
    3c1c:	e594f000 	ldr	pc, [r4]
    3c20:	e3a01000 	mov	r1, #0	; 0x0
    3c24:	e59f22f8 	ldr	r2, [pc, #760]	; 3f24 <tt_crypto_aes+0x37c>
    3c28:	e3a00001 	mov	r0, #1	; 0x1
    3c2c:	eb0005d6 	bl	538c <ss_aes_init>
    3c30:	e59f02f0 	ldr	r0, [pc, #752]	; 3f28 <tt_crypto_aes+0x380>
    3c34:	e1a0e00f 	mov	lr, pc
    3c38:	e594f000 	ldr	pc, [r4]
    3c3c:	e3560000 	cmp	r6, #0	; 0x0
    3c40:	159d500c 	ldrne	r5, [sp, #12]
    3c44:	13a0a000 	movne	sl, #0	; 0x0
    3c48:	12868901 	addne	r8, r6, #16384	; 0x4000
    3c4c:	1a000022 	bne	3cdc <tt_crypto_aes+0x134>
    3c50:	ea000025 	b	3cec <tt_crypto_aes+0x144>
    3c54:	e59d100c 	ldr	r1, [sp, #12]
    3c58:	e3550902 	cmp	r5, #32768	; 0x8000
    3c5c:	b1a07005 	movlt	r7, r5
    3c60:	a3a07902 	movge	r7, #32768	; 0x8000
    3c64:	e0651001 	rsb	r1, r5, r1
    3c68:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3c6c:	e1a02007 	mov	r2, r7
    3c70:	ebfff5d8 	bl	13d8 <memcpy>
    3c74:	e3a00000 	mov	r0, #0	; 0x0
    3c78:	eb001372 	bl	8a48 <get_timer>
    3c7c:	e3550901 	cmp	r5, #16384	; 0x4000
    3c80:	b1a02005 	movlt	r2, r5
    3c84:	a3a02901 	movge	r2, #16384	; 0x4000
    3c88:	e1a04000 	mov	r4, r0
    3c8c:	e1a01006 	mov	r1, r6
    3c90:	e1a00006 	mov	r0, r6
    3c94:	eb0005ab 	bl	5348 <ss_aes_data>
    3c98:	e3570902 	cmp	r7, #32768	; 0x8000
    3c9c:	1a000003 	bne	3cb0 <tt_crypto_aes+0x108>
    3ca0:	e1a01008 	mov	r1, r8
    3ca4:	e3a02901 	mov	r2, #16384	; 0x4000
    3ca8:	e1a00008 	mov	r0, r8
    3cac:	eb0005a5 	bl	5348 <ss_aes_data>
    3cb0:	e1a00004 	mov	r0, r4
    3cb4:	eb001363 	bl	8a48 <get_timer>
    3cb8:	e1a04000 	mov	r4, r0
    3cbc:	e59d000c 	ldr	r0, [sp, #12]
    3cc0:	e1a01006 	mov	r1, r6
    3cc4:	e0650000 	rsb	r0, r5, r0
    3cc8:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    3ccc:	e1a02007 	mov	r2, r7
    3cd0:	ebfff5c0 	bl	13d8 <memcpy>
    3cd4:	e08aa004 	add	sl, sl, r4
    3cd8:	e0675005 	rsb	r5, r7, r5
    3cdc:	e3550000 	cmp	r5, #0	; 0x0
    3ce0:	e1a00006 	mov	r0, r6
    3ce4:	1affffda 	bne	3c54 <tt_crypto_aes+0xac>
    3ce8:	ea000009 	b	3d14 <tt_crypto_aes+0x16c>
    3cec:	e1a00006 	mov	r0, r6
    3cf0:	eb001354 	bl	8a48 <get_timer>
    3cf4:	e1a04000 	mov	r4, r0
    3cf8:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3cfc:	e1a01000 	mov	r1, r0
    3d00:	e59d200c 	ldr	r2, [sp, #12]
    3d04:	eb00058f 	bl	5348 <ss_aes_data>
    3d08:	e1a00004 	mov	r0, r4
    3d0c:	eb00134d 	bl	8a48 <get_timer>
    3d10:	e1a0a000 	mov	sl, r0
    3d14:	e59d300c 	ldr	r3, [sp, #12]
    3d18:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3d1c:	e1a0100b 	mov	r1, fp
    3d20:	e3a02010 	mov	r2, #16	; 0x10
    3d24:	e58d3004 	str	r3, [sp, #4]
    3d28:	ebfff5cb 	bl	145c <memcmp>
    3d2c:	e3500000 	cmp	r0, #0	; 0x0
    3d30:	e59f41d8 	ldr	r4, [pc, #472]	; 3f10 <tt_crypto_aes+0x368>
    3d34:	1a000007 	bne	3d58 <tt_crypto_aes+0x1b0>
    3d38:	e59f01ec 	ldr	r0, [pc, #492]	; 3f2c <tt_crypto_aes+0x384>
    3d3c:	e1a0e00f 	mov	lr, pc
    3d40:	e594f000 	ldr	pc, [r4]
    3d44:	e59f01e4 	ldr	r0, [pc, #484]	; 3f30 <tt_crypto_aes+0x388>
    3d48:	e1a0e00f 	mov	lr, pc
    3d4c:	e594f000 	ldr	pc, [r4]
    3d50:	e3e00000 	mvn	r0, #0	; 0x0
    3d54:	ea00006b 	b	3f08 <tt_crypto_aes+0x360>
    3d58:	e3a00000 	mov	r0, #0	; 0x0
    3d5c:	e1a01000 	mov	r1, r0
    3d60:	e59f21bc 	ldr	r2, [pc, #444]	; 3f24 <tt_crypto_aes+0x37c>
    3d64:	eb000588 	bl	538c <ss_aes_init>
    3d68:	e59f01c4 	ldr	r0, [pc, #452]	; 3f34 <tt_crypto_aes+0x38c>
    3d6c:	e1a0e00f 	mov	lr, pc
    3d70:	e594f000 	ldr	pc, [r4]
    3d74:	e3560000 	cmp	r6, #0	; 0x0
    3d78:	159d500c 	ldrne	r5, [sp, #12]
    3d7c:	13a08000 	movne	r8, #0	; 0x0
    3d80:	12869901 	addne	r9, r6, #16384	; 0x4000
    3d84:	1a000022 	bne	3e14 <tt_crypto_aes+0x26c>
    3d88:	ea000025 	b	3e24 <tt_crypto_aes+0x27c>
    3d8c:	eb00132d 	bl	8a48 <get_timer>
    3d90:	e59d100c 	ldr	r1, [sp, #12]
    3d94:	e3550902 	cmp	r5, #32768	; 0x8000
    3d98:	b1a07005 	movlt	r7, r5
    3d9c:	a3a07902 	movge	r7, #32768	; 0x8000
    3da0:	e0651001 	rsb	r1, r5, r1
    3da4:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3da8:	e1a02007 	mov	r2, r7
    3dac:	e1a04000 	mov	r4, r0
    3db0:	e1a00006 	mov	r0, r6
    3db4:	ebfff587 	bl	13d8 <memcpy>
    3db8:	e3550901 	cmp	r5, #16384	; 0x4000
    3dbc:	b1a02005 	movlt	r2, r5
    3dc0:	a3a02901 	movge	r2, #16384	; 0x4000
    3dc4:	e1a00006 	mov	r0, r6
    3dc8:	e1a01006 	mov	r1, r6
    3dcc:	eb00055d 	bl	5348 <ss_aes_data>
    3dd0:	e3570902 	cmp	r7, #32768	; 0x8000
    3dd4:	1a000003 	bne	3de8 <tt_crypto_aes+0x240>
    3dd8:	e1a01009 	mov	r1, r9
    3ddc:	e3a02901 	mov	r2, #16384	; 0x4000
    3de0:	e1a00009 	mov	r0, r9
    3de4:	eb000557 	bl	5348 <ss_aes_data>
    3de8:	e1a00004 	mov	r0, r4
    3dec:	eb001315 	bl	8a48 <get_timer>
    3df0:	e1a04000 	mov	r4, r0
    3df4:	e59d000c 	ldr	r0, [sp, #12]
    3df8:	e1a01006 	mov	r1, r6
    3dfc:	e0650000 	rsb	r0, r5, r0
    3e00:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    3e04:	e1a02007 	mov	r2, r7
    3e08:	ebfff572 	bl	13d8 <memcpy>
    3e0c:	e0888004 	add	r8, r8, r4
    3e10:	e0675005 	rsb	r5, r7, r5
    3e14:	e3a00000 	mov	r0, #0	; 0x0
    3e18:	e1550000 	cmp	r5, r0
    3e1c:	1affffda 	bne	3d8c <tt_crypto_aes+0x1e4>
    3e20:	ea000009 	b	3e4c <tt_crypto_aes+0x2a4>
    3e24:	e1a00006 	mov	r0, r6
    3e28:	eb001306 	bl	8a48 <get_timer>
    3e2c:	e1a04000 	mov	r4, r0
    3e30:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3e34:	e1a01000 	mov	r1, r0
    3e38:	e59d200c 	ldr	r2, [sp, #12]
    3e3c:	eb000541 	bl	5348 <ss_aes_data>
    3e40:	e1a00004 	mov	r0, r4
    3e44:	eb0012ff 	bl	8a48 <get_timer>
    3e48:	e1a08000 	mov	r8, r0
    3e4c:	e59d3004 	ldr	r3, [sp, #4]
    3e50:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    3e54:	e1a00543 	asr	r0, r3, #10
    3e58:	e1a0100a 	mov	r1, sl
    3e5c:	e0000094 	mul	r0, r4, r0
    3e60:	eb002d1e 	bl	f2e0 <__aeabi_idiv>
    3e64:	e1a06000 	mov	r6, r0
    3e68:	e59d000c 	ldr	r0, [sp, #12]
    3e6c:	e1a01008 	mov	r1, r8
    3e70:	e1a00540 	asr	r0, r0, #10
    3e74:	e0000094 	mul	r0, r4, r0
    3e78:	eb002d18 	bl	f2e0 <__aeabi_idiv>
    3e7c:	e59f408c 	ldr	r4, [pc, #140]	; 3f10 <tt_crypto_aes+0x368>
    3e80:	e1a05000 	mov	r5, r0
    3e84:	e59f00ac 	ldr	r0, [pc, #172]	; 3f38 <tt_crypto_aes+0x390>
    3e88:	e1a0e00f 	mov	lr, pc
    3e8c:	e594f000 	ldr	pc, [r4]
    3e90:	e1a0000b 	mov	r0, fp
    3e94:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    3e98:	e59d200c 	ldr	r2, [sp, #12]
    3e9c:	e3a0300c 	mov	r3, #12	; 0xc
    3ea0:	eb00014f 	bl	43e4 <tt_cmp_buffer>
    3ea4:	e3500000 	cmp	r0, #0	; 0x0
    3ea8:	0a000006 	beq	3ec8 <tt_crypto_aes+0x320>
    3eac:	e1a03000 	mov	r3, r0
    3eb0:	e1a01006 	mov	r1, r6
    3eb4:	e1a02005 	mov	r2, r5
    3eb8:	e59f007c 	ldr	r0, [pc, #124]	; 3f3c <tt_crypto_aes+0x394>
    3ebc:	e1a0e00f 	mov	lr, pc
    3ec0:	e594f000 	ldr	pc, [r4]
    3ec4:	ea000004 	b	3edc <tt_crypto_aes+0x334>
    3ec8:	e1a01006 	mov	r1, r6
    3ecc:	e1a02005 	mov	r2, r5
    3ed0:	e59f0068 	ldr	r0, [pc, #104]	; 3f40 <tt_crypto_aes+0x398>
    3ed4:	e1a0e00f 	mov	lr, pc
    3ed8:	e594f000 	ldr	pc, [r4]
    3edc:	e59f0060 	ldr	r0, [pc, #96]	; 3f44 <tt_crypto_aes+0x39c>
    3ee0:	e59d100c 	ldr	r1, [sp, #12]
    3ee4:	e890000c 	ldm	r0, {r2, r3}
    3ee8:	e0922001 	adds	r2, r2, r1
    3eec:	e0a33fc1 	adc	r3, r3, r1, asr #31
    3ef0:	e880000c 	stm	r0, {r2, r3}
    3ef4:	e59f0030 	ldr	r0, [pc, #48]	; 3f2c <tt_crypto_aes+0x384>
    3ef8:	e59f3010 	ldr	r3, [pc, #16]	; 3f10 <tt_crypto_aes+0x368>
    3efc:	e1a0e00f 	mov	lr, pc
    3f00:	e593f000 	ldr	pc, [r3]
    3f04:	e3a00000 	mov	r0, #0	; 0x0
    3f08:	e28dd014 	add	sp, sp, #20	; 0x14
    3f0c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    3f10:	0800d704 	.word	0x0800d704
    3f14:	000118ad 	.word	0x000118ad
    3f18:	000118d0 	.word	0x000118d0
    3f1c:	00011318 	.word	0x00011318
    3f20:	000123d5 	.word	0x000123d5
    3f24:	0800d0bc 	.word	0x0800d0bc
    3f28:	000110ef 	.word	0x000110ef
    3f2c:	0001140d 	.word	0x0001140d
    3f30:	000118fc 	.word	0x000118fc
    3f34:	000121bf 	.word	0x000121bf
    3f38:	5e          	.byte	0x5e
    3f39:	16          	.byte	0x16
    3f3a:	0001      	.short	0x0001
    3f3c:	0001191d 	.word	0x0001191d
    3f40:	0001195b 	.word	0x0001195b
    3f44:	0800d178 	.word	0x0800d178

00003f48 <do_tt_crypto>:
    3f48:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    3f4c:	e3520001 	cmp	r2, #1	; 0x1
    3f50:	e24dd004 	sub	sp, sp, #4	; 0x4
    3f54:	d3a07000 	movle	r7, #0	; 0x0
    3f58:	da000009 	ble	3f84 <do_tt_crypto+0x3c>
    3f5c:	e5930004 	ldr	r0, [r3, #4]
    3f60:	e3a01000 	mov	r1, #0	; 0x0
    3f64:	e3a02010 	mov	r2, #16	; 0x10
    3f68:	eb002056 	bl	c0c8 <simple_strtoul>
    3f6c:	e1a07000 	mov	r7, r0
    3f70:	e1a01007 	mov	r1, r7
    3f74:	e59f0114 	ldr	r0, [pc, #276]	; 4090 <do_tt_crypto+0x148>
    3f78:	e59f3114 	ldr	r3, [pc, #276]	; 4094 <do_tt_crypto+0x14c>
    3f7c:	e1a0e00f 	mov	lr, pc
    3f80:	e593f000 	ldr	pc, [r3]
    3f84:	e3a00000 	mov	r0, #0	; 0x0
    3f88:	eb0012ae 	bl	8a48 <get_timer>
    3f8c:	e59f1104 	ldr	r1, [pc, #260]	; 4098 <do_tt_crypto+0x150>
    3f90:	e3a02000 	mov	r2, #0	; 0x0
    3f94:	e3a03000 	mov	r3, #0	; 0x0
    3f98:	e881000c 	stm	r1, {r2, r3}
    3f9c:	e59f10f8 	ldr	r1, [pc, #248]	; 409c <do_tt_crypto+0x154>
    3fa0:	e59fa0ec 	ldr	sl, [pc, #236]	; 4094 <do_tt_crypto+0x14c>
    3fa4:	e881000c 	stm	r1, {r2, r3}
    3fa8:	e1a08000 	mov	r8, r0
    3fac:	e3a06000 	mov	r6, #0	; 0x0
    3fb0:	e3a01006 	mov	r1, #6	; 0x6
    3fb4:	e3a00000 	mov	r0, #0	; 0x0
    3fb8:	eb00015d 	bl	4534 <tt_rnd_get_int>
    3fbc:	e3a01002 	mov	r1, #2	; 0x2
    3fc0:	e1a04000 	mov	r4, r0
    3fc4:	e3a00000 	mov	r0, #0	; 0x0
    3fc8:	eb000159 	bl	4534 <tt_rnd_get_int>
    3fcc:	e1a01006 	mov	r1, r6
    3fd0:	e1a03000 	mov	r3, r0
    3fd4:	e1a05000 	mov	r5, r0
    3fd8:	e1a02004 	mov	r2, r4
    3fdc:	e59f00bc 	ldr	r0, [pc, #188]	; 40a0 <do_tt_crypto+0x158>
    3fe0:	e1a0e00f 	mov	lr, pc
    3fe4:	e59af000 	ldr	pc, [sl]
    3fe8:	e3540003 	cmp	r4, #3	; 0x3
    3fec:	9a000004 	bls	4004 <do_tt_crypto+0xbc>
    3ff0:	e3550000 	cmp	r5, #0	; 0x0
    3ff4:	11a00007 	movne	r0, r7
    3ff8:	03a00000 	moveq	r0, #0	; 0x0
    3ffc:	ebfffee9 	bl	3ba8 <tt_crypto_aes>
    4000:	ea000003 	b	4014 <do_tt_crypto+0xcc>
    4004:	e3550000 	cmp	r5, #0	; 0x0
    4008:	11a00007 	movne	r0, r7
    400c:	03a00000 	moveq	r0, #0	; 0x0
    4010:	ebfffe4a 	bl	3940 <tt_crypto_hash>
    4014:	eb002442 	bl	d124 <tstc>
    4018:	e3500000 	cmp	r0, #0	; 0x0
    401c:	e2866001 	add	r6, r6, #1	; 0x1
    4020:	0affffe2 	beq	3fb0 <do_tt_crypto+0x68>
    4024:	eb00243f 	bl	d128 <getc>
    4028:	e3500003 	cmp	r0, #3	; 0x3
    402c:	1affffdf 	bne	3fb0 <do_tt_crypto+0x68>
    4030:	e1a00008 	mov	r0, r8
    4034:	eb001283 	bl	8a48 <get_timer>
    4038:	e59f305c 	ldr	r3, [pc, #92]	; 409c <do_tt_crypto+0x154>
    403c:	e59f2054 	ldr	r2, [pc, #84]	; 4098 <do_tt_crypto+0x150>
    4040:	e5934000 	ldr	r4, [r3]
    4044:	e5925000 	ldr	r5, [r2]
    4048:	e5933004 	ldr	r3, [r3, #4]
    404c:	e5922004 	ldr	r2, [r2, #4]
    4050:	e30e1a60 	movw	r1, #60000	; 0xea60
    4054:	e1a04a24 	lsr	r4, r4, #20
    4058:	e1a05a25 	lsr	r5, r5, #20
    405c:	e1844603 	orr	r4, r4, r3, lsl #12
    4060:	e1855602 	orr	r5, r5, r2, lsl #12
    4064:	eb002c1b 	bl	f0d8 <__aeabi_uidiv>
    4068:	e1a01004 	mov	r1, r4
    406c:	e1a03000 	mov	r3, r0
    4070:	e1a02005 	mov	r2, r5
    4074:	e59f0028 	ldr	r0, [pc, #40]	; 40a4 <do_tt_crypto+0x15c>
    4078:	e59fc014 	ldr	ip, [pc, #20]	; 4094 <do_tt_crypto+0x14c>
    407c:	e1a0e00f 	mov	lr, pc
    4080:	e59cf000 	ldr	pc, [ip]
    4084:	e3a00000 	mov	r0, #0	; 0x0
    4088:	e28dd004 	add	sp, sp, #4	; 0x4
    408c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    4090:	0001198c 	.word	0x0001198c
    4094:	0800d704 	.word	0x0800d704
    4098:	0800d180 	.word	0x0800d180
    409c:	0800d178 	.word	0x0800d178
    40a0:	000119b2 	.word	0x000119b2
    40a4:	000119d1 	.word	0x000119d1

000040a8 <tt_ff_trolling>:
    40a8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    40ac:	e3a01001 	mov	r1, #1	; 0x1
    40b0:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    40b4:	e58d200c 	str	r2, [sp, #12]
    40b8:	e58d3008 	str	r3, [sp, #8]
    40bc:	e1a05000 	mov	r5, r0
    40c0:	eb0003bd 	bl	4fbc <vs_assign_by_name>
    40c4:	e2506000 	subs	r6, r0, #0	; 0x0
    40c8:	0a000005 	beq	40e4 <tt_ff_trolling+0x3c>
    40cc:	e1a01005 	mov	r1, r5
    40d0:	e59f0258 	ldr	r0, [pc, #600]	; 4330 <tt_ff_trolling+0x288>
    40d4:	e59f3258 	ldr	r3, [pc, #600]	; 4334 <tt_ff_trolling+0x28c>
    40d8:	e1a0e00f 	mov	lr, pc
    40dc:	e593f000 	ldr	pc, [r3]
    40e0:	ea00000c 	b	4118 <tt_ff_trolling+0x70>
    40e4:	e3a0000e 	mov	r0, #14	; 0xe
    40e8:	eb000300 	bl	4cf0 <vs_align>
    40ec:	e1a01000 	mov	r1, r0
    40f0:	e28d0018 	add	r0, sp, #24	; 0x18
    40f4:	e5201004 	str	r1, [r0, #-4]!
    40f8:	e1a02001 	mov	r2, r1
    40fc:	eb000135 	bl	45d8 <tt_rnd_get_data>
    4100:	e250a000 	subs	sl, r0, #0	; 0x0
    4104:	e59f4228 	ldr	r4, [pc, #552]	; 4334 <tt_ff_trolling+0x28c>
    4108:	1a000004 	bne	4120 <tt_ff_trolling+0x78>
    410c:	e59f0224 	ldr	r0, [pc, #548]	; 4338 <tt_ff_trolling+0x290>
    4110:	e1a0e00f 	mov	lr, pc
    4114:	e594f000 	ldr	pc, [r4]
    4118:	e3e00000 	mvn	r0, #0	; 0x0
    411c:	ea000073 	b	42f0 <tt_ff_trolling+0x248>
    4120:	e59d2014 	ldr	r2, [sp, #20]
    4124:	e1a0100a 	mov	r1, sl
    4128:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    412c:	ebfff4a9 	bl	13d8 <memcpy>
    4130:	e3a01501 	mov	r1, #4194304	; 0x400000
    4134:	e59f0200 	ldr	r0, [pc, #512]	; 433c <tt_ff_trolling+0x294>
    4138:	e1a0e00f 	mov	lr, pc
    413c:	e594f000 	ldr	pc, [r4]
    4140:	e1a00005 	mov	r0, r5
    4144:	eb000388 	bl	4f6c <vs_device_id>
    4148:	e3a04000 	mov	r4, #0	; 0x0
    414c:	e3a05000 	mov	r5, #0	; 0x0
    4150:	e1a0b006 	mov	fp, r6
    4154:	e3a08501 	mov	r8, #4194304	; 0x400000
    4158:	e3a09000 	mov	r9, #0	; 0x0
    415c:	e1a07000 	mov	r7, r0
    4160:	ea000021 	b	41ec <tt_ff_trolling+0x144>
    4164:	eb00036c 	bl	4f1c <vs_device_erasable>
    4168:	e3500000 	cmp	r0, #0	; 0x0
    416c:	0a000003 	beq	4180 <tt_ff_trolling+0xd8>
    4170:	e1a00004 	mov	r0, r4
    4174:	e1a01005 	mov	r1, r5
    4178:	e3a02501 	mov	r2, #4194304	; 0x400000
    417c:	eb00029f 	bl	4c00 <vs_erase>
    4180:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    4184:	e5816000 	str	r6, [r1]
    4188:	e1a00001 	mov	r0, r1
    418c:	e59d1014 	ldr	r1, [sp, #20]
    4190:	e1a02004 	mov	r2, r4
    4194:	e1a03005 	mov	r3, r5
    4198:	e88d0802 	stm	sp, {r1, fp}
    419c:	eb000281 	bl	4ba8 <vs_write>
    41a0:	e3500000 	cmp	r0, #0	; 0x0
    41a4:	aa000006 	bge	41c4 <tt_ff_trolling+0x11c>
    41a8:	e1a02004 	mov	r2, r4
    41ac:	e1a03005 	mov	r3, r5
    41b0:	e59f0188 	ldr	r0, [pc, #392]	; 4340 <tt_ff_trolling+0x298>
    41b4:	e59f1178 	ldr	r1, [pc, #376]	; 4334 <tt_ff_trolling+0x28c>
    41b8:	e1a0e00f 	mov	lr, pc
    41bc:	e591f000 	ldr	pc, [r1]
    41c0:	ea000012 	b	4210 <tt_ff_trolling+0x168>
    41c4:	e316001f 	tst	r6, #31	; 0x1f
    41c8:	1a000004 	bne	41e0 <tt_ff_trolling+0x138>
    41cc:	e1a01106 	lsl	r1, r6, #2
    41d0:	e59f016c 	ldr	r0, [pc, #364]	; 4344 <tt_ff_trolling+0x29c>
    41d4:	e59f3158 	ldr	r3, [pc, #344]	; 4334 <tt_ff_trolling+0x28c>
    41d8:	e1a0e00f 	mov	lr, pc
    41dc:	e593f000 	ldr	pc, [r3]
    41e0:	e0944008 	adds	r4, r4, r8
    41e4:	e0a55009 	adc	r5, r5, r9
    41e8:	e2866001 	add	r6, r6, #1	; 0x1
    41ec:	e59d1008 	ldr	r1, [sp, #8]
    41f0:	e1a00007 	mov	r0, r7
    41f4:	e1510005 	cmp	r1, r5
    41f8:	8affffd9 	bhi	4164 <tt_ff_trolling+0xbc>
    41fc:	1a00003d 	bne	42f8 <tt_ff_trolling+0x250>
    4200:	e59d300c 	ldr	r3, [sp, #12]
    4204:	e1530004 	cmp	r3, r4
    4208:	8affffd5 	bhi	4164 <tt_ff_trolling+0xbc>
    420c:	ea000039 	b	42f8 <tt_ff_trolling+0x250>
    4210:	e59f411c 	ldr	r4, [pc, #284]	; 4334 <tt_ff_trolling+0x28c>
    4214:	e59f012c 	ldr	r0, [pc, #300]	; 4348 <tt_ff_trolling+0x2a0>
    4218:	e3a01501 	mov	r1, #4194304	; 0x400000
    421c:	e3a06000 	mov	r6, #0	; 0x0
    4220:	e1a0e00f 	mov	lr, pc
    4224:	e594f000 	ldr	pc, [r4]
    4228:	e28a7004 	add	r7, sl, #4	; 0x4
    422c:	e3a04000 	mov	r4, #0	; 0x0
    4230:	e3a05000 	mov	r5, #0	; 0x0
    4234:	e1a0b006 	mov	fp, r6
    4238:	e3a0a44e 	mov	sl, #1308622848	; 0x4e000000
    423c:	e3a08501 	mov	r8, #4194304	; 0x400000
    4240:	e3a09000 	mov	r9, #0	; 0x0
    4244:	ea00001e 	b	42c4 <tt_ff_trolling+0x21c>
    4248:	e59d1014 	ldr	r1, [sp, #20]
    424c:	e88d0802 	stm	sp, {r1, fp}
    4250:	eb00039a 	bl	50c0 <vs_read>
    4254:	e316001f 	tst	r6, #31	; 0x1f
    4258:	1a000004 	bne	4270 <tt_ff_trolling+0x1c8>
    425c:	e1a01106 	lsl	r1, r6, #2
    4260:	e59f00dc 	ldr	r0, [pc, #220]	; 4344 <tt_ff_trolling+0x29c>
    4264:	e59f30c8 	ldr	r3, [pc, #200]	; 4334 <tt_ff_trolling+0x28c>
    4268:	e1a0e00f 	mov	lr, pc
    426c:	e593f000 	ldr	pc, [r3]
    4270:	e59a2000 	ldr	r2, [sl]
    4274:	e1a00007 	mov	r0, r7
    4278:	e1520006 	cmp	r2, r6
    427c:	e59f10c8 	ldr	r1, [pc, #200]	; 434c <tt_ff_trolling+0x2a4>
    4280:	e3a0300a 	mov	r3, #10	; 0xa
    4284:	e2866001 	add	r6, r6, #1	; 0x1
    4288:	1a000004 	bne	42a0 <tt_ff_trolling+0x1f8>
    428c:	e59d2014 	ldr	r2, [sp, #20]
    4290:	e2422004 	sub	r2, r2, #4	; 0x4
    4294:	eb000052 	bl	43e4 <tt_cmp_buffer>
    4298:	e3500000 	cmp	r0, #0	; 0x0
    429c:	0a000006 	beq	42bc <tt_ff_trolling+0x214>
    42a0:	e1a02004 	mov	r2, r4
    42a4:	e1a03005 	mov	r3, r5
    42a8:	e59f00a0 	ldr	r0, [pc, #160]	; 4350 <tt_ff_trolling+0x2a8>
    42ac:	e59f1080 	ldr	r1, [pc, #128]	; 4334 <tt_ff_trolling+0x28c>
    42b0:	e1a0e00f 	mov	lr, pc
    42b4:	e591f000 	ldr	pc, [r1]
    42b8:	ea00001a 	b	4328 <tt_ff_trolling+0x280>
    42bc:	e0944008 	adds	r4, r4, r8
    42c0:	e0a55009 	adc	r5, r5, r9
    42c4:	e59d1008 	ldr	r1, [sp, #8]
    42c8:	e1a02004 	mov	r2, r4
    42cc:	e1510005 	cmp	r1, r5
    42d0:	e1a03005 	mov	r3, r5
    42d4:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    42d8:	8affffda 	bhi	4248 <tt_ff_trolling+0x1a0>
    42dc:	1a00000a 	bne	430c <tt_ff_trolling+0x264>
    42e0:	e59d100c 	ldr	r1, [sp, #12]
    42e4:	e1510004 	cmp	r1, r4
    42e8:	8affffd6 	bhi	4248 <tt_ff_trolling+0x1a0>
    42ec:	ea000006 	b	430c <tt_ff_trolling+0x264>
    42f0:	e28dd01c 	add	sp, sp, #28	; 0x1c
    42f4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    42f8:	e59f0054 	ldr	r0, [pc, #84]	; 4354 <tt_ff_trolling+0x2ac>
    42fc:	e59f3030 	ldr	r3, [pc, #48]	; 4334 <tt_ff_trolling+0x28c>
    4300:	e1a0e00f 	mov	lr, pc
    4304:	e593f000 	ldr	pc, [r3]
    4308:	eaffffc0 	b	4210 <tt_ff_trolling+0x168>
    430c:	e59f0040 	ldr	r0, [pc, #64]	; 4354 <tt_ff_trolling+0x2ac>
    4310:	e59f401c 	ldr	r4, [pc, #28]	; 4334 <tt_ff_trolling+0x28c>
    4314:	e1a0e00f 	mov	lr, pc
    4318:	e594f000 	ldr	pc, [r4]
    431c:	e59f0034 	ldr	r0, [pc, #52]	; 4358 <tt_ff_trolling+0x2b0>
    4320:	e1a0e00f 	mov	lr, pc
    4324:	e594f000 	ldr	pc, [r4]
    4328:	e3a00000 	mov	r0, #0	; 0x0
    432c:	eaffffef 	b	42f0 <tt_ff_trolling+0x248>
    4330:	00011a55 	.word	0x00011a55
    4334:	0800d704 	.word	0x0800d704
    4338:	00011a72 	.word	0x00011a72
    433c:	00011a91 	.word	0x00011a91
    4340:	00011aaf 	.word	0x00011aaf
    4344:	00011ac7 	.word	0x00011ac7
    4348:	00011ace 	.word	0x00011ace
    434c:	4e000004 	.word	0x4e000004
    4350:	00011aee 	.word	0x00011aee
    4354:	00011afe 	.word	0x00011afe
    4358:	00011b05 	.word	0x00011b05

0000435c <do_tt_ff>:
    435c:	e92d4030 	push	{r4, r5, lr}
    4360:	e3520001 	cmp	r2, #1	; 0x1
    4364:	e24dd004 	sub	sp, sp, #4	; 0x4
    4368:	e1a05003 	mov	r5, r3
    436c:	ca000005 	bgt	4388 <do_tt_ff+0x2c>
    4370:	e59f0060 	ldr	r0, [pc, #96]	; 43d8 <do_tt_ff+0x7c>
    4374:	e59f3060 	ldr	r3, [pc, #96]	; 43dc <do_tt_ff+0x80>
    4378:	e1a0e00f 	mov	lr, pc
    437c:	e593f000 	ldr	pc, [r3]
    4380:	e3e00000 	mvn	r0, #0	; 0x0
    4384:	ea000011 	b	43d0 <do_tt_ff+0x74>
    4388:	e3520002 	cmp	r2, #2	; 0x2
    438c:	03a04000 	moveq	r4, #0	; 0x0
    4390:	0a000009 	beq	43bc <do_tt_ff+0x60>
    4394:	e3a01000 	mov	r1, #0	; 0x0
    4398:	e5930008 	ldr	r0, [r3, #8]
    439c:	e3a0200a 	mov	r2, #10	; 0xa
    43a0:	eb001f48 	bl	c0c8 <simple_strtoul>
    43a4:	e1a04000 	mov	r4, r0
    43a8:	e1a01004 	mov	r1, r4
    43ac:	e59f002c 	ldr	r0, [pc, #44]	; 43e0 <do_tt_ff+0x84>
    43b0:	e59f3024 	ldr	r3, [pc, #36]	; 43dc <do_tt_ff+0x80>
    43b4:	e1a0e00f 	mov	lr, pc
    43b8:	e593f000 	ldr	pc, [r3]
    43bc:	e5950004 	ldr	r0, [r5, #4]
    43c0:	e1a03624 	lsr	r3, r4, #12
    43c4:	e1a02a04 	lsl	r2, r4, #20
    43c8:	ebffff36 	bl	40a8 <tt_ff_trolling>
    43cc:	e3a00000 	mov	r0, #0	; 0x0
    43d0:	e28dd004 	add	sp, sp, #4	; 0x4
    43d4:	e8bd8030 	pop	{r4, r5, pc}
    43d8:	00011b1d 	.word	0x00011b1d
    43dc:	0800d704 	.word	0x0800d704
    43e0:	00011b63 	.word	0x00011b63

000043e4 <tt_cmp_buffer>:
    43e4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    43e8:	e3a04000 	mov	r4, #0	; 0x0
    43ec:	e24dd004 	sub	sp, sp, #4	; 0x4
    43f0:	e1a0a002 	mov	sl, r2
    43f4:	e1a07000 	mov	r7, r0
    43f8:	e1a06001 	mov	r6, r1
    43fc:	e1a08003 	mov	r8, r3
    4400:	e1a09142 	asr	r9, r2, #2
    4404:	e1a05004 	mov	r5, r4
    4408:	e59fb0e4 	ldr	fp, [pc, #228]	; 44f4 <tt_cmp_buffer+0x110>
    440c:	ea00001f 	b	4490 <tt_cmp_buffer+0xac>
    4410:	e5970000 	ldr	r0, [r7]
    4414:	e5961000 	ldr	r1, [r6]
    4418:	e1500001 	cmp	r0, r1
    441c:	0a000018 	beq	4484 <tt_cmp_buffer+0xa0>
    4420:	e5d72000 	ldrb	r2, [r7]
    4424:	e5d63000 	ldrb	r3, [r6]
    4428:	e1520003 	cmp	r2, r3
    442c:	e5d72001 	ldrb	r2, [r7, #1]
    4430:	e5d63001 	ldrb	r3, [r6, #1]
    4434:	12855001 	addne	r5, r5, #1	; 0x1
    4438:	e1520003 	cmp	r2, r3
    443c:	e5d72002 	ldrb	r2, [r7, #2]
    4440:	e5d63002 	ldrb	r3, [r6, #2]
    4444:	12855001 	addne	r5, r5, #1	; 0x1
    4448:	e1520003 	cmp	r2, r3
    444c:	e5d72003 	ldrb	r2, [r7, #3]
    4450:	e5d63003 	ldrb	r3, [r6, #3]
    4454:	12855001 	addne	r5, r5, #1	; 0x1
    4458:	e1520003 	cmp	r2, r3
    445c:	12855001 	addne	r5, r5, #1	; 0x1
    4460:	e3580000 	cmp	r8, #0	; 0x0
    4464:	0a000006 	beq	4484 <tt_cmp_buffer+0xa0>
    4468:	e1a02000 	mov	r2, r0
    446c:	e1a03001 	mov	r3, r1
    4470:	e59f0080 	ldr	r0, [pc, #128]	; 44f8 <tt_cmp_buffer+0x114>
    4474:	e1a01104 	lsl	r1, r4, #2
    4478:	e1a0e00f 	mov	lr, pc
    447c:	e59bf000 	ldr	pc, [fp]
    4480:	e2488001 	sub	r8, r8, #1	; 0x1
    4484:	e2844001 	add	r4, r4, #1	; 0x1
    4488:	e2877004 	add	r7, r7, #4	; 0x4
    448c:	e2866004 	add	r6, r6, #4	; 0x4
    4490:	e1540009 	cmp	r4, r9
    4494:	baffffdd 	blt	4410 <tt_cmp_buffer+0x2c>
    4498:	e3ca9003 	bic	r9, sl, #3	; 0x3
    449c:	e3a04000 	mov	r4, #0	; 0x0
    44a0:	e20aa003 	and	sl, sl, #3	; 0x3
    44a4:	e59fb048 	ldr	fp, [pc, #72]	; 44f4 <tt_cmp_buffer+0x110>
    44a8:	ea00000c 	b	44e0 <tt_cmp_buffer+0xfc>
    44ac:	e7d72004 	ldrb	r2, [r7, r4]
    44b0:	e7d63004 	ldrb	r3, [r6, r4]
    44b4:	e1520003 	cmp	r2, r3
    44b8:	0a000007 	beq	44dc <tt_cmp_buffer+0xf8>
    44bc:	e3580000 	cmp	r8, #0	; 0x0
    44c0:	e2855001 	add	r5, r5, #1	; 0x1
    44c4:	0a000004 	beq	44dc <tt_cmp_buffer+0xf8>
    44c8:	e0841009 	add	r1, r4, r9
    44cc:	e59f0028 	ldr	r0, [pc, #40]	; 44fc <tt_cmp_buffer+0x118>
    44d0:	e1a0e00f 	mov	lr, pc
    44d4:	e59bf000 	ldr	pc, [fp]
    44d8:	e2488001 	sub	r8, r8, #1	; 0x1
    44dc:	e2844001 	add	r4, r4, #1	; 0x1
    44e0:	e154000a 	cmp	r4, sl
    44e4:	bafffff0 	blt	44ac <tt_cmp_buffer+0xc8>
    44e8:	e1a00005 	mov	r0, r5
    44ec:	e28dd004 	add	sp, sp, #4	; 0x4
    44f0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    44f4:	0800d704 	.word	0x0800d704
    44f8:	00011b8a 	.word	0x00011b8a
    44fc:	00011bb5 	.word	0x00011bb5

00004500 <tt_rnd_seed>:
    4500:	e92d4030 	push	{r4, r5, lr}
    4504:	e59f5024 	ldr	r5, [pc, #36]	; 4530 <tt_rnd_seed+0x30>
    4508:	e24dd004 	sub	sp, sp, #4	; 0x4
    450c:	e3a00000 	mov	r0, #0	; 0x0
    4510:	e5954000 	ldr	r4, [r5]
    4514:	eb00114b 	bl	8a48 <get_timer>
    4518:	e30f3ffc 	movw	r3, #65532	; 0xfffc
    451c:	e6e40070 	uxtab	r0, r4, r0
    4520:	e0003003 	and	r3, r0, r3
    4524:	e5853000 	str	r3, [r5]
    4528:	e28dd004 	add	sp, sp, #4	; 0x4
    452c:	e8bd8030 	pop	{r4, r5, pc}
    4530:	0800d188 	.word	0x0800d188

00004534 <tt_rnd_get_int>:
    4534:	e59fc094 	ldr	ip, [pc, #148]	; 45d0 <tt_rnd_get_int+0x9c>
    4538:	e3a02101 	mov	r2, #1073741824	; 0x40000000
    453c:	e59c3000 	ldr	r3, [ip]
    4540:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4544:	e19330b2 	ldrh	r3, [r3, r2]
    4548:	e24dd004 	sub	sp, sp, #4	; 0x4
    454c:	e3c33003 	bic	r3, r3, #3	; 0x3
    4550:	e6ff3073 	uxth	r3, r3
    4554:	e19340b2 	ldrh	r4, [r3, r2]
    4558:	e58c3000 	str	r3, [ip]
    455c:	e1a07000 	mov	r7, r0
    4560:	e1a06001 	mov	r6, r1
    4564:	ebffffe5 	bl	4500 <tt_rnd_seed>
    4568:	e1a00004 	mov	r0, r4
    456c:	eb002ca5 	bl	f808 <__aeabi_ui2d>
    4570:	e3a02000 	mov	r2, #0	; 0x0
    4574:	e59f3058 	ldr	r3, [pc, #88]	; 45d4 <tt_rnd_get_int+0xa0>
    4578:	eb002d7d 	bl	fb74 <__aeabi_ddiv>
    457c:	e1a04000 	mov	r4, r0
    4580:	e0670006 	rsb	r0, r7, r6
    4584:	e1a05001 	mov	r5, r1
    4588:	eb002ca7 	bl	f82c <__aeabi_i2d>
    458c:	e1a02000 	mov	r2, r0
    4590:	e1a03001 	mov	r3, r1
    4594:	e1a00004 	mov	r0, r4
    4598:	e1a01005 	mov	r1, r5
    459c:	eb002cd9 	bl	f908 <__aeabi_dmul>
    45a0:	e1a04000 	mov	r4, r0
    45a4:	e1a00007 	mov	r0, r7
    45a8:	e1a05001 	mov	r5, r1
    45ac:	eb002c9e 	bl	f82c <__aeabi_i2d>
    45b0:	e1a02000 	mov	r2, r0
    45b4:	e1a03001 	mov	r3, r1
    45b8:	e1a00004 	mov	r0, r4
    45bc:	e1a01005 	mov	r1, r5
    45c0:	eb002be5 	bl	f55c <__adddf3>
    45c4:	eb002deb 	bl	fd78 <__aeabi_d2iz>
    45c8:	e28dd004 	add	sp, sp, #4	; 0x4
    45cc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    45d0:	0800d188 	.word	0x0800d188
    45d4:	40efffe0 	.word	0x40efffe0

000045d8 <tt_rnd_get_data>:
    45d8:	e92d4010 	push	{r4, lr}
    45dc:	e1a04000 	mov	r4, r0
    45e0:	e1a00001 	mov	r0, r1
    45e4:	e1a01002 	mov	r1, r2
    45e8:	ebffffd1 	bl	4534 <tt_rnd_get_int>
    45ec:	e59f3010 	ldr	r3, [pc, #16]	; 4604 <tt_rnd_get_data+0x2c>
    45f0:	e5840000 	str	r0, [r4]
    45f4:	e5930000 	ldr	r0, [r3]
    45f8:	e3c00003 	bic	r0, r0, #3	; 0x3
    45fc:	e2800101 	add	r0, r0, #1073741824	; 0x40000000
    4600:	e8bd8010 	pop	{r4, pc}
    4604:	0800d188 	.word	0x0800d188

00004608 <tt_wire_get_opt>:
    4608:	e5900008 	ldr	r0, [r0, #8]
    460c:	e12fff1e 	bx	lr

00004610 <tt_wire_get_len>:
    4610:	e590000c 	ldr	r0, [r0, #12]
    4614:	e12fff1e 	bx	lr

00004618 <tt_wire_make_data>:
    4618:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    461c:	e24dd004 	sub	sp, sp, #4	; 0x4
    4620:	e1a04000 	mov	r4, r0
    4624:	e1a07002 	mov	r7, r2
    4628:	e1a06003 	mov	r6, r3
    462c:	ebfff369 	bl	13d8 <memcpy>
    4630:	e59f3040 	ldr	r3, [pc, #64]	; 4678 <tt_wire_make_data+0x60>
    4634:	e5846008 	str	r6, [r4, #8]
    4638:	e5843000 	str	r3, [r4]
    463c:	e59d3018 	ldr	r3, [sp, #24]
    4640:	e3a05000 	mov	r5, #0	; 0x0
    4644:	e584300c 	str	r3, [r4, #12]
    4648:	e1a01007 	mov	r1, r7
    464c:	e5845004 	str	r5, [r4, #4]
    4650:	e1a00005 	mov	r0, r5
    4654:	eb00046a 	bl	5804 <hash_init>
    4658:	e1a01007 	mov	r1, r7
    465c:	e1a00004 	mov	r0, r4
    4660:	eb000442 	bl	5770 <hash_data>
    4664:	e2840004 	add	r0, r4, #4	; 0x4
    4668:	eb000424 	bl	5700 <hash_value>
    466c:	e1a00005 	mov	r0, r5
    4670:	e28dd004 	add	sp, sp, #4	; 0x4
    4674:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4678:	7766dd80 	.word	0x7766dd80

0000467c <tt_wire_check_valid>:
    467c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4680:	e5903000 	ldr	r3, [r0]
    4684:	e59f2084 	ldr	r2, [pc, #132]	; 4710 <tt_wire_check_valid+0x94>
    4688:	e24dd00c 	sub	sp, sp, #12	; 0xc
    468c:	e1530002 	cmp	r3, r2
    4690:	e1a04000 	mov	r4, r0
    4694:	e1a06001 	mov	r6, r1
    4698:	e5907004 	ldr	r7, [r0, #4]
    469c:	0a000006 	beq	46bc <tt_wire_check_valid+0x40>
    46a0:	e1a01003 	mov	r1, r3
    46a4:	e59f0068 	ldr	r0, [pc, #104]	; 4714 <tt_wire_check_valid+0x98>
    46a8:	e59f3068 	ldr	r3, [pc, #104]	; 4718 <tt_wire_check_valid+0x9c>
    46ac:	e1a0e00f 	mov	lr, pc
    46b0:	e593f000 	ldr	pc, [r3]
    46b4:	e3a00000 	mov	r0, #0	; 0x0
    46b8:	ea000012 	b	4708 <tt_wire_check_valid+0x8c>
    46bc:	e3a05000 	mov	r5, #0	; 0x0
    46c0:	e5805004 	str	r5, [r0, #4]
    46c4:	e1a00005 	mov	r0, r5
    46c8:	eb00044d 	bl	5804 <hash_init>
    46cc:	e1a01006 	mov	r1, r6
    46d0:	e1a00004 	mov	r0, r4
    46d4:	eb000425 	bl	5770 <hash_data>
    46d8:	e28d0004 	add	r0, sp, #4	; 0x4
    46dc:	eb000407 	bl	5700 <hash_value>
    46e0:	e59d1004 	ldr	r1, [sp, #4]
    46e4:	e1510007 	cmp	r1, r7
    46e8:	03a00001 	moveq	r0, #1	; 0x1
    46ec:	0a000005 	beq	4708 <tt_wire_check_valid+0x8c>
    46f0:	e59f0024 	ldr	r0, [pc, #36]	; 471c <tt_wire_check_valid+0xa0>
    46f4:	e1a02007 	mov	r2, r7
    46f8:	e59f3018 	ldr	r3, [pc, #24]	; 4718 <tt_wire_check_valid+0x9c>
    46fc:	e1a0e00f 	mov	lr, pc
    4700:	e593f000 	ldr	pc, [r3]
    4704:	e1a00005 	mov	r0, r5
    4708:	e28dd00c 	add	sp, sp, #12	; 0xc
    470c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4710:	7766dd80 	.word	0x7766dd80
    4714:	00011be0 	.word	0x00011be0
    4718:	0800d704 	.word	0x0800d704
    471c:	00011c09 	.word	0x00011c09

00004720 <do_tt_wire>:
    4720:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4724:	e3520001 	cmp	r2, #1	; 0x1
    4728:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    472c:	e1a04003 	mov	r4, r3
    4730:	d59f032c 	ldrle	r0, [pc, #812]	; 4a64 <do_tt_wire+0x344>
    4734:	da00002b 	ble	47e8 <do_tt_wire+0xc8>
    4738:	e3a02003 	mov	r2, #3	; 0x3
    473c:	e5930004 	ldr	r0, [r3, #4]
    4740:	e59f1320 	ldr	r1, [pc, #800]	; 4a68 <do_tt_wire+0x348>
    4744:	ebfff2ea 	bl	12f4 <strncmp>
    4748:	e3500000 	cmp	r0, #0	; 0x0
    474c:	059f2318 	ldreq	r2, [pc, #792]	; 4a6c <do_tt_wire+0x34c>
    4750:	0a000006 	beq	4770 <do_tt_wire+0x50>
    4754:	e5940004 	ldr	r0, [r4, #4]
    4758:	e59f1310 	ldr	r1, [pc, #784]	; 4a70 <do_tt_wire+0x350>
    475c:	e3a02003 	mov	r2, #3	; 0x3
    4760:	ebfff2e3 	bl	12f4 <strncmp>
    4764:	e3500000 	cmp	r0, #0	; 0x0
    4768:	1a000009 	bne	4794 <do_tt_wire+0x74>
    476c:	e59f2300 	ldr	r2, [pc, #768]	; 4a74 <do_tt_wire+0x354>
    4770:	e59f3300 	ldr	r3, [pc, #768]	; 4a78 <do_tt_wire+0x358>
    4774:	e3a00000 	mov	r0, #0	; 0x0
    4778:	e5832000 	str	r2, [r3]
    477c:	eb0010b1 	bl	8a48 <get_timer>
    4780:	e59f82f4 	ldr	r8, [pc, #756]	; 4a7c <do_tt_wire+0x35c>
    4784:	e59f92ec 	ldr	r9, [pc, #748]	; 4a78 <do_tt_wire+0x358>
    4788:	e1a0a008 	mov	sl, r8
    478c:	e58d000c 	str	r0, [sp, #12]
    4790:	ea000006 	b	47b0 <do_tt_wire+0x90>
    4794:	e5941004 	ldr	r1, [r4, #4]
    4798:	e59f02e0 	ldr	r0, [pc, #736]	; 4a80 <do_tt_wire+0x360>
    479c:	e59f32d8 	ldr	r3, [pc, #728]	; 4a7c <do_tt_wire+0x35c>
    47a0:	e1a0e00f 	mov	lr, pc
    47a4:	e593f000 	ldr	pc, [r3]
    47a8:	e3e00000 	mvn	r0, #0	; 0x0
    47ac:	ea0000aa 	b	4a5c <do_tt_wire+0x33c>
    47b0:	e59f02cc 	ldr	r0, [pc, #716]	; 4a84 <do_tt_wire+0x364>
    47b4:	eb001097 	bl	8a18 <udelay>
    47b8:	e59f02c8 	ldr	r0, [pc, #712]	; 4a88 <do_tt_wire+0x368>
    47bc:	e1a0e00f 	mov	lr, pc
    47c0:	e598f000 	ldr	pc, [r8]
    47c4:	e5993000 	ldr	r3, [r9]
    47c8:	e1a0e00f 	mov	lr, pc
    47cc:	e593f000 	ldr	pc, [r3]
    47d0:	e3500000 	cmp	r0, #0	; 0x0
    47d4:	a3a07000 	movge	r7, #0	; 0x0
    47d8:	a3a06020 	movge	r6, #32	; 0x20
    47dc:	a59fb2a8 	ldrge	fp, [pc, #680]	; 4a8c <do_tt_wire+0x36c>
    47e0:	aa000004 	bge	47f8 <do_tt_wire+0xd8>
    47e4:	e59f02a4 	ldr	r0, [pc, #676]	; 4a90 <do_tt_wire+0x370>
    47e8:	e59f328c 	ldr	r3, [pc, #652]	; 4a7c <do_tt_wire+0x35c>
    47ec:	e1a0e00f 	mov	lr, pc
    47f0:	e593f000 	ldr	pc, [r3]
    47f4:	eaffffeb 	b	47a8 <do_tt_wire+0x88>
    47f8:	e3570000 	cmp	r7, #0	; 0x0
    47fc:	e1a01006 	mov	r1, r6
    4800:	e59f028c 	ldr	r0, [pc, #652]	; 4a94 <do_tt_wire+0x374>
    4804:	1a00002f 	bne	48c8 <do_tt_wire+0x1a8>
    4808:	e59f0288 	ldr	r0, [pc, #648]	; 4a98 <do_tt_wire+0x378>
    480c:	e1a0e00f 	mov	lr, pc
    4810:	e598f000 	ldr	pc, [r8]
    4814:	e1a00007 	mov	r0, r7
    4818:	eb00108a 	bl	8a48 <get_timer>
    481c:	e1a01006 	mov	r1, r6
    4820:	e1a04000 	mov	r4, r0
    4824:	e5993000 	ldr	r3, [r9]
    4828:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    482c:	e1a0e00f 	mov	lr, pc
    4830:	e593f004 	ldr	pc, [r3, #4]
    4834:	e1a05000 	mov	r5, r0
    4838:	e1a00004 	mov	r0, r4
    483c:	eb001081 	bl	8a48 <get_timer>
    4840:	e3550000 	cmp	r5, #0	; 0x0
    4844:	e1a04000 	mov	r4, r0
    4848:	e1a01006 	mov	r1, r6
    484c:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4850:	ba00003d 	blt	494c <do_tt_wire+0x22c>
    4854:	ebffff88 	bl	467c <tt_wire_check_valid>
    4858:	e3500000 	cmp	r0, #0	; 0x0
    485c:	e1a01004 	mov	r1, r4
    4860:	e1a03546 	asr	r3, r6, #10
    4864:	1a000007 	bne	4888 <do_tt_wire+0x168>
    4868:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    486c:	e0000390 	mul	r0, r0, r3
    4870:	eb002a9a 	bl	f2e0 <__aeabi_idiv>
    4874:	e1a01000 	mov	r1, r0
    4878:	e59f021c 	ldr	r0, [pc, #540]	; 4a9c <do_tt_wire+0x37c>
    487c:	e1a0e00f 	mov	lr, pc
    4880:	e59af000 	ldr	pc, [sl]
    4884:	eaffffc9 	b	47b0 <do_tt_wire+0x90>
    4888:	e59fc210 	ldr	ip, [pc, #528]	; 4aa0 <do_tt_wire+0x380>
    488c:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    4890:	e0000392 	mul	r0, r2, r3
    4894:	e89c000c 	ldm	ip, {r2, r3}
    4898:	e0922006 	adds	r2, r2, r6
    489c:	e0a33fc6 	adc	r3, r3, r6, asr #31
    48a0:	e88c000c 	stm	ip, {r2, r3}
    48a4:	eb002a8d 	bl	f2e0 <__aeabi_idiv>
    48a8:	e1a01000 	mov	r1, r0
    48ac:	e59f01f0 	ldr	r0, [pc, #496]	; 4aa4 <do_tt_wire+0x384>
    48b0:	e1a0e00f 	mov	lr, pc
    48b4:	e598f000 	ldr	pc, [r8]
    48b8:	e3a0344e 	mov	r3, #1308622848	; 0x4e000000
    48bc:	e5937008 	ldr	r7, [r3, #8]
    48c0:	e593600c 	ldr	r6, [r3, #12]
    48c4:	ea000044 	b	49dc <do_tt_wire+0x2bc>
    48c8:	e3570001 	cmp	r7, #1	; 0x1
    48cc:	1a000030 	bne	4994 <do_tt_wire+0x274>
    48d0:	e1a0e00f 	mov	lr, pc
    48d4:	e598f000 	ldr	pc, [r8]
    48d8:	e1a01007 	mov	r1, r7
    48dc:	e3a00000 	mov	r0, #0	; 0x0
    48e0:	ebffff13 	bl	4534 <tt_rnd_get_int>
    48e4:	e3a01020 	mov	r1, #32	; 0x20
    48e8:	e1a07000 	mov	r7, r0
    48ec:	e3a02502 	mov	r2, #8388608	; 0x800000
    48f0:	e28d0014 	add	r0, sp, #20	; 0x14
    48f4:	ebffff37 	bl	45d8 <tt_rnd_get_data>
    48f8:	e59dc014 	ldr	ip, [sp, #20]
    48fc:	e1a01000 	mov	r1, r0
    4900:	e1a02006 	mov	r2, r6
    4904:	e1a03007 	mov	r3, r7
    4908:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    490c:	e58dc000 	str	ip, [sp]
    4910:	ebffff40 	bl	4618 <tt_wire_make_data>
    4914:	e3a00000 	mov	r0, #0	; 0x0
    4918:	eb00104a 	bl	8a48 <get_timer>
    491c:	e1a01006 	mov	r1, r6
    4920:	e1a05000 	mov	r5, r0
    4924:	e5993000 	ldr	r3, [r9]
    4928:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    492c:	e1a0e00f 	mov	lr, pc
    4930:	e593f008 	ldr	pc, [r3, #8]
    4934:	e1a04000 	mov	r4, r0
    4938:	e1a00005 	mov	r0, r5
    493c:	eb001041 	bl	8a48 <get_timer>
    4940:	e3540000 	cmp	r4, #0	; 0x0
    4944:	e1a01000 	mov	r1, r0
    4948:	aa000003 	bge	495c <do_tt_wire+0x23c>
    494c:	e59f0154 	ldr	r0, [pc, #340]	; 4aa8 <do_tt_wire+0x388>
    4950:	e1a0e00f 	mov	lr, pc
    4954:	e59af000 	ldr	pc, [sl]
    4958:	eaffff94 	b	47b0 <do_tt_wire+0x90>
    495c:	e89b000c 	ldm	fp, {r2, r3}
    4960:	e0922006 	adds	r2, r2, r6
    4964:	e0a33fc6 	adc	r3, r3, r6, asr #31
    4968:	e59d6014 	ldr	r6, [sp, #20]
    496c:	e88b000c 	stm	fp, {r2, r3}
    4970:	e1a00546 	asr	r0, r6, #10
    4974:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    4978:	e0000092 	mul	r0, r2, r0
    497c:	eb002a57 	bl	f2e0 <__aeabi_idiv>
    4980:	e1a01000 	mov	r1, r0
    4984:	e59f0120 	ldr	r0, [pc, #288]	; 4aac <do_tt_wire+0x38c>
    4988:	e1a0e00f 	mov	lr, pc
    498c:	e598f000 	ldr	pc, [r8]
    4990:	ea000011 	b	49dc <do_tt_wire+0x2bc>
    4994:	e3570003 	cmp	r7, #3	; 0x3
    4998:	1a000004 	bne	49b0 <do_tt_wire+0x290>
    499c:	e59f010c 	ldr	r0, [pc, #268]	; 4ab0 <do_tt_wire+0x390>
    49a0:	e59f30d4 	ldr	r3, [pc, #212]	; 4a7c <do_tt_wire+0x35c>
    49a4:	e1a0e00f 	mov	lr, pc
    49a8:	e593f000 	ldr	pc, [r3]
    49ac:	ea000010 	b	49f4 <do_tt_wire+0x2d4>
    49b0:	e3570002 	cmp	r7, #2	; 0x2
    49b4:	1a000003 	bne	49c8 <do_tt_wire+0x2a8>
    49b8:	e59f00f4 	ldr	r0, [pc, #244]	; 4ab4 <do_tt_wire+0x394>
    49bc:	e1a0e00f 	mov	lr, pc
    49c0:	e598f000 	ldr	pc, [r8]
    49c4:	eaffff79 	b	47b0 <do_tt_wire+0x90>
    49c8:	e59f00e8 	ldr	r0, [pc, #232]	; 4ab8 <do_tt_wire+0x398>
    49cc:	e59f30a8 	ldr	r3, [pc, #168]	; 4a7c <do_tt_wire+0x35c>
    49d0:	e1a0e00f 	mov	lr, pc
    49d4:	e593f000 	ldr	pc, [r3]
    49d8:	eafffffe 	b	49d8 <do_tt_wire+0x2b8>
    49dc:	eb0021d0 	bl	d124 <tstc>
    49e0:	e3500000 	cmp	r0, #0	; 0x0
    49e4:	0affff83 	beq	47f8 <do_tt_wire+0xd8>
    49e8:	eb0021ce 	bl	d128 <getc>
    49ec:	e3500003 	cmp	r0, #3	; 0x3
    49f0:	1affff80 	bne	47f8 <do_tt_wire+0xd8>
    49f4:	e59d000c 	ldr	r0, [sp, #12]
    49f8:	eb001012 	bl	8a48 <get_timer>
    49fc:	e59f3088 	ldr	r3, [pc, #136]	; 4a8c <do_tt_wire+0x36c>
    4a00:	e59f4074 	ldr	r4, [pc, #116]	; 4a7c <do_tt_wire+0x35c>
    4a04:	e893000a 	ldm	r3, {r1, r3}
    4a08:	e1a01a21 	lsr	r1, r1, #20
    4a0c:	e1811603 	orr	r1, r1, r3, lsl #12
    4a10:	e1a05000 	mov	r5, r0
    4a14:	e59f00a0 	ldr	r0, [pc, #160]	; 4abc <do_tt_wire+0x39c>
    4a18:	e1a0e00f 	mov	lr, pc
    4a1c:	e594f000 	ldr	pc, [r4]
    4a20:	e59f3078 	ldr	r3, [pc, #120]	; 4aa0 <do_tt_wire+0x380>
    4a24:	e59f0094 	ldr	r0, [pc, #148]	; 4ac0 <do_tt_wire+0x3a0>
    4a28:	e893000a 	ldm	r3, {r1, r3}
    4a2c:	e1a01a21 	lsr	r1, r1, #20
    4a30:	e1811603 	orr	r1, r1, r3, lsl #12
    4a34:	e1a0e00f 	mov	lr, pc
    4a38:	e594f000 	ldr	pc, [r4]
    4a3c:	e30e1a60 	movw	r1, #60000	; 0xea60
    4a40:	e1a00005 	mov	r0, r5
    4a44:	eb002a25 	bl	f2e0 <__aeabi_idiv>
    4a48:	e1a01000 	mov	r1, r0
    4a4c:	e59f0070 	ldr	r0, [pc, #112]	; 4ac4 <do_tt_wire+0x3a4>
    4a50:	e1a0e00f 	mov	lr, pc
    4a54:	e594f000 	ldr	pc, [r4]
    4a58:	e3a00000 	mov	r0, #0	; 0x0
    4a5c:	e28dd01c 	add	sp, sp, #28	; 0x1c
    4a60:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4a64:	00011c32 	.word	0x00011c32
    4a68:	00011c7b 	.word	0x00011c7b
    4a6c:	0000fea4 	.word	0x0000fea4
    4a70:	00011c7f 	.word	0x00011c7f
    4a74:	0000febc 	.word	0x0000febc
    4a78:	0800d1a0 	.word	0x0800d1a0
    4a7c:	0800d704 	.word	0x0800d704
    4a80:	00011c83 	.word	0x00011c83
    4a84:	001e8480 	.word	0x001e8480
    4a88:	00011ca3 	.word	0x00011ca3
    4a8c:	0800d198 	.word	0x0800d198
    4a90:	00011cc5 	.word	0x00011cc5
    4a94:	00011d32 	.word	0x00011d32
    4a98:	00011ce8 	.word	0x00011ce8
    4a9c:	00011d14 	.word	0x00011d14
    4aa0:	0800d190 	.word	0x0800d190
    4aa4:	00011d23 	.word	0x00011d23
    4aa8:	00011d04 	.word	0x00011d04
    4aac:	00011d4e 	.word	0x00011d4e
    4ab0:	00011d5d 	.word	0x00011d5d
    4ab4:	00011d6f 	.word	0x00011d6f
    4ab8:	00011d88 	.word	0x00011d88
    4abc:	00011db5 	.word	0x00011db5
    4ac0:	00011dcf 	.word	0x00011dcf
    4ac4:	00011ded 	.word	0x00011ded

00004ac8 <vs_is_assigned>:
    4ac8:	e59f300c 	ldr	r3, [pc, #12]	; 4adc <vs_is_assigned+0x14>
    4acc:	e5930000 	ldr	r0, [r3]
    4ad0:	e2500000 	subs	r0, r0, #0	; 0x0
    4ad4:	13a00001 	movne	r0, #1	; 0x1
    4ad8:	e12fff1e 	bx	lr
    4adc:	0800d1a8 	.word	0x0800d1a8

00004ae0 <vs_is_opt_available>:
    4ae0:	e350000d 	cmp	r0, #13	; 0xd
    4ae4:	959f2078 	ldrls	r2, [pc, #120]	; 4b64 <vs_is_opt_available+0x84>
    4ae8:	93a0302c 	movls	r3, #44	; 0x2c
    4aec:	859f3074 	ldrhi	r3, [pc, #116]	; 4b68 <vs_is_opt_available+0x88>
    4af0:	90202093 	mlals	r0, r3, r0, r2
    4af4:	85930000 	ldrhi	r0, [r3]
    4af8:	e3500000 	cmp	r0, #0	; 0x0
    4afc:	0a000016 	beq	4b5c <vs_is_opt_available+0x7c>
    4b00:	e3510005 	cmp	r1, #5	; 0x5
    4b04:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    4b08:	ea000013 	b	4b5c <vs_is_opt_available+0x7c>
    4b0c:	00004b24 	.word	0x00004b24
    4b10:	00004b2c 	.word	0x00004b2c
    4b14:	00004b34 	.word	0x00004b34
    4b18:	00004b3c 	.word	0x00004b3c
    4b1c:	00004b44 	.word	0x00004b44
    4b20:	00004b4c 	.word	0x00004b4c
    4b24:	e590300c 	ldr	r3, [r0, #12]
    4b28:	ea000008 	b	4b50 <vs_is_opt_available+0x70>
    4b2c:	e5903010 	ldr	r3, [r0, #16]
    4b30:	ea000006 	b	4b50 <vs_is_opt_available+0x70>
    4b34:	e5903014 	ldr	r3, [r0, #20]
    4b38:	ea000004 	b	4b50 <vs_is_opt_available+0x70>
    4b3c:	e5903018 	ldr	r3, [r0, #24]
    4b40:	ea000002 	b	4b50 <vs_is_opt_available+0x70>
    4b44:	e590301c 	ldr	r3, [r0, #28]
    4b48:	ea000000 	b	4b50 <vs_is_opt_available+0x70>
    4b4c:	e5903020 	ldr	r3, [r0, #32]
    4b50:	e2530000 	subs	r0, r3, #0	; 0x0
    4b54:	13a00001 	movne	r0, #1	; 0x1
    4b58:	e12fff1e 	bx	lr
    4b5c:	e3a00000 	mov	r0, #0	; 0x0
    4b60:	e12fff1e 	bx	lr
    4b64:	0000febc 	.word	0x0000febc
    4b68:	0800d1a8 	.word	0x0800d1a8

00004b6c <vs_reset>:
    4b6c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4b70:	e59f302c 	ldr	r3, [pc, #44]	; 4ba4 <vs_reset+0x38>
    4b74:	e24dd004 	sub	sp, sp, #4	; 0x4
    4b78:	e5933000 	ldr	r3, [r3]
    4b7c:	e3530000 	cmp	r3, #0	; 0x0
    4b80:	03e00001 	mvneq	r0, #1	; 0x1
    4b84:	0a000004 	beq	4b9c <vs_reset+0x30>
    4b88:	e5933018 	ldr	r3, [r3, #24]
    4b8c:	e3530000 	cmp	r3, #0	; 0x0
    4b90:	03e00002 	mvneq	r0, #2	; 0x2
    4b94:	0a000000 	beq	4b9c <vs_reset+0x30>
    4b98:	e12fff33 	blx	r3
    4b9c:	e28dd004 	add	sp, sp, #4	; 0x4
    4ba0:	e8bd8000 	pop	{pc}
    4ba4:	0800d1a8 	.word	0x0800d1a8

00004ba8 <vs_write>:
    4ba8:	e92d4010 	push	{r4, lr}
    4bac:	e24dd008 	sub	sp, sp, #8	; 0x8
    4bb0:	e59d4010 	ldr	r4, [sp, #16]
    4bb4:	e3540000 	cmp	r4, #0	; 0x0
    4bb8:	01a00004 	moveq	r0, r4
    4bbc:	0a00000c 	beq	4bf4 <vs_write+0x4c>
    4bc0:	e59f1034 	ldr	r1, [pc, #52]	; 4bfc <vs_write+0x54>
    4bc4:	e5911000 	ldr	r1, [r1]
    4bc8:	e3510000 	cmp	r1, #0	; 0x0
    4bcc:	03e00001 	mvneq	r0, #1	; 0x1
    4bd0:	0a000007 	beq	4bf4 <vs_write+0x4c>
    4bd4:	e591c014 	ldr	ip, [r1, #20]
    4bd8:	e35c0000 	cmp	ip, #0	; 0x0
    4bdc:	03e00002 	mvneq	r0, #2	; 0x2
    4be0:	0a000003 	beq	4bf4 <vs_write+0x4c>
    4be4:	e59d1014 	ldr	r1, [sp, #20]
    4be8:	e58d4000 	str	r4, [sp]
    4bec:	e58d1004 	str	r1, [sp, #4]
    4bf0:	e12fff3c 	blx	ip
    4bf4:	e28dd008 	add	sp, sp, #8	; 0x8
    4bf8:	e8bd8010 	pop	{r4, pc}
    4bfc:	0800d1a8 	.word	0x0800d1a8

00004c00 <vs_erase>:
    4c00:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4c04:	e59f302c 	ldr	r3, [pc, #44]	; 4c38 <vs_erase+0x38>
    4c08:	e24dd004 	sub	sp, sp, #4	; 0x4
    4c0c:	e5933000 	ldr	r3, [r3]
    4c10:	e3530000 	cmp	r3, #0	; 0x0
    4c14:	03e00001 	mvneq	r0, #1	; 0x1
    4c18:	0a000004 	beq	4c30 <vs_erase+0x30>
    4c1c:	e593301c 	ldr	r3, [r3, #28]
    4c20:	e3530000 	cmp	r3, #0	; 0x0
    4c24:	03e00002 	mvneq	r0, #2	; 0x2
    4c28:	0a000000 	beq	4c30 <vs_erase+0x30>
    4c2c:	e12fff33 	blx	r3
    4c30:	e28dd004 	add	sp, sp, #4	; 0x4
    4c34:	e8bd8000 	pop	{pc}
    4c38:	0800d1a8 	.word	0x0800d1a8

00004c3c <vs_isbad>:
    4c3c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4c40:	e59f302c 	ldr	r3, [pc, #44]	; 4c74 <vs_isbad+0x38>
    4c44:	e24dd004 	sub	sp, sp, #4	; 0x4
    4c48:	e5933000 	ldr	r3, [r3]
    4c4c:	e3530000 	cmp	r3, #0	; 0x0
    4c50:	03e00001 	mvneq	r0, #1	; 0x1
    4c54:	0a000004 	beq	4c6c <vs_isbad+0x30>
    4c58:	e5933024 	ldr	r3, [r3, #36]
    4c5c:	e3530000 	cmp	r3, #0	; 0x0
    4c60:	03e00002 	mvneq	r0, #2	; 0x2
    4c64:	0a000000 	beq	4c6c <vs_isbad+0x30>
    4c68:	e12fff33 	blx	r3
    4c6c:	e28dd004 	add	sp, sp, #4	; 0x4
    4c70:	e8bd8000 	pop	{pc}
    4c74:	0800d1a8 	.word	0x0800d1a8

00004c78 <vs_scrub>:
    4c78:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4c7c:	e59f302c 	ldr	r3, [pc, #44]	; 4cb0 <vs_scrub+0x38>
    4c80:	e24dd004 	sub	sp, sp, #4	; 0x4
    4c84:	e5933000 	ldr	r3, [r3]
    4c88:	e3530000 	cmp	r3, #0	; 0x0
    4c8c:	03e00001 	mvneq	r0, #1	; 0x1
    4c90:	0a000004 	beq	4ca8 <vs_scrub+0x30>
    4c94:	e5933020 	ldr	r3, [r3, #32]
    4c98:	e3530000 	cmp	r3, #0	; 0x0
    4c9c:	03e00002 	mvneq	r0, #2	; 0x2
    4ca0:	0a000000 	beq	4ca8 <vs_scrub+0x30>
    4ca4:	e12fff33 	blx	r3
    4ca8:	e28dd004 	add	sp, sp, #4	; 0x4
    4cac:	e8bd8000 	pop	{pc}
    4cb0:	0800d1a8 	.word	0x0800d1a8

00004cb4 <vs_special>:
    4cb4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4cb8:	e59f302c 	ldr	r3, [pc, #44]	; 4cec <vs_special+0x38>
    4cbc:	e24dd004 	sub	sp, sp, #4	; 0x4
    4cc0:	e5933000 	ldr	r3, [r3]
    4cc4:	e3530000 	cmp	r3, #0	; 0x0
    4cc8:	03e00001 	mvneq	r0, #1	; 0x1
    4ccc:	0a000004 	beq	4ce4 <vs_special+0x30>
    4cd0:	e5933028 	ldr	r3, [r3, #40]
    4cd4:	e3530000 	cmp	r3, #0	; 0x0
    4cd8:	03e00002 	mvneq	r0, #2	; 0x2
    4cdc:	0a000000 	beq	4ce4 <vs_special+0x30>
    4ce0:	e12fff33 	blx	r3
    4ce4:	e28dd004 	add	sp, sp, #4	; 0x4
    4ce8:	e8bd8000 	pop	{pc}
    4cec:	0800d1a8 	.word	0x0800d1a8

00004cf0 <vs_align>:
    4cf0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4cf4:	e350000e 	cmp	r0, #14	; 0xe
    4cf8:	059f3044 	ldreq	r3, [pc, #68]	; 4d44 <vs_align+0x54>
    4cfc:	e24dd004 	sub	sp, sp, #4	; 0x4
    4d00:	05930000 	ldreq	r0, [r3]
    4d04:	0a000004 	beq	4d1c <vs_align+0x2c>
    4d08:	e350000d 	cmp	r0, #13	; 0xd
    4d0c:	8a000009 	bhi	4d38 <vs_align+0x48>
    4d10:	e59f2030 	ldr	r2, [pc, #48]	; 4d48 <vs_align+0x58>
    4d14:	e3a0302c 	mov	r3, #44	; 0x2c
    4d18:	e0202093 	mla	r0, r3, r0, r2
    4d1c:	e3500000 	cmp	r0, #0	; 0x0
    4d20:	0a000004 	beq	4d38 <vs_align+0x48>
    4d24:	e590000c 	ldr	r0, [r0, #12]
    4d28:	e3500000 	cmp	r0, #0	; 0x0
    4d2c:	0a000001 	beq	4d38 <vs_align+0x48>
    4d30:	e12fff30 	blx	r0
    4d34:	ea000000 	b	4d3c <vs_align+0x4c>
    4d38:	e3a00000 	mov	r0, #0	; 0x0
    4d3c:	e28dd004 	add	sp, sp, #4	; 0x4
    4d40:	e8bd8000 	pop	{pc}
    4d44:	0800d1a8 	.word	0x0800d1a8
    4d48:	0000febc 	.word	0x0000febc

00004d4c <vs_assign_by_id>:
    4d4c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4d50:	e59fc05c 	ldr	ip, [pc, #92]	; 4db4 <vs_assign_by_id+0x68>
    4d54:	e3a03000 	mov	r3, #0	; 0x0
    4d58:	e350000d 	cmp	r0, #13	; 0xd
    4d5c:	e24dd004 	sub	sp, sp, #4	; 0x4
    4d60:	e58c3000 	str	r3, [ip]
    4d64:	8a00000b 	bhi	4d98 <vs_assign_by_id+0x4c>
    4d68:	e59f2048 	ldr	r2, [pc, #72]	; 4db8 <vs_assign_by_id+0x6c>
    4d6c:	e283302c 	add	r3, r3, #44	; 0x2c
    4d70:	e0232093 	mla	r3, r3, r0, r2
    4d74:	e3510000 	cmp	r1, #0	; 0x0
    4d78:	e58c3000 	str	r3, [ip]
    4d7c:	01a00001 	moveq	r0, r1
    4d80:	0a000002 	beq	4d90 <vs_assign_by_id+0x44>
    4d84:	e28dd004 	add	sp, sp, #4	; 0x4
    4d88:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    4d8c:	eaffff76 	b	4b6c <vs_reset>
    4d90:	e28dd004 	add	sp, sp, #4	; 0x4
    4d94:	e8bd8000 	pop	{pc}
    4d98:	e1a01000 	mov	r1, r0
    4d9c:	e59f3018 	ldr	r3, [pc, #24]	; 4dbc <vs_assign_by_id+0x70>
    4da0:	e59f0018 	ldr	r0, [pc, #24]	; 4dc0 <vs_assign_by_id+0x74>
    4da4:	e1a0e00f 	mov	lr, pc
    4da8:	e593f000 	ldr	pc, [r3]
    4dac:	e3e00000 	mvn	r0, #0	; 0x0
    4db0:	eafffff6 	b	4d90 <vs_assign_by_id+0x44>
    4db4:	0800d1a8 	.word	0x0800d1a8
    4db8:	0000febc 	.word	0x0000febc
    4dbc:	0800d704 	.word	0x0800d704
    4dc0:	00011e16 	.word	0x00011e16

00004dc4 <vs_device_bootable>:
    4dc4:	e350000e 	cmp	r0, #14	; 0xe
    4dc8:	059f3030 	ldreq	r3, [pc, #48]	; 4e00 <vs_device_bootable+0x3c>
    4dcc:	05930000 	ldreq	r0, [r3]
    4dd0:	0a000004 	beq	4de8 <vs_device_bootable+0x24>
    4dd4:	e350000d 	cmp	r0, #13	; 0xd
    4dd8:	8a000006 	bhi	4df8 <vs_device_bootable+0x34>
    4ddc:	e59f2020 	ldr	r2, [pc, #32]	; 4e04 <vs_device_bootable+0x40>
    4de0:	e3a0302c 	mov	r3, #44	; 0x2c
    4de4:	e0202093 	mla	r0, r3, r0, r2
    4de8:	e3500000 	cmp	r0, #0	; 0x0
    4dec:	15903008 	ldrne	r3, [r0, #8]
    4df0:	12030001 	andne	r0, r3, #1	; 0x1
    4df4:	112fff1e 	bxne	lr
    4df8:	e3a00000 	mov	r0, #0	; 0x0
    4dfc:	e12fff1e 	bx	lr
    4e00:	0800d1a8 	.word	0x0800d1a8
    4e04:	0000febc 	.word	0x0000febc

00004e08 <vs_device_burnsrc>:
    4e08:	e350000e 	cmp	r0, #14	; 0xe
    4e0c:	059f3034 	ldreq	r3, [pc, #52]	; 4e48 <vs_device_burnsrc+0x40>
    4e10:	05930000 	ldreq	r0, [r3]
    4e14:	0a000004 	beq	4e2c <vs_device_burnsrc+0x24>
    4e18:	e350000d 	cmp	r0, #13	; 0xd
    4e1c:	8a000007 	bhi	4e40 <vs_device_burnsrc+0x38>
    4e20:	e59f2024 	ldr	r2, [pc, #36]	; 4e4c <vs_device_burnsrc+0x44>
    4e24:	e3a0302c 	mov	r3, #44	; 0x2c
    4e28:	e0202093 	mla	r0, r3, r0, r2
    4e2c:	e3500000 	cmp	r0, #0	; 0x0
    4e30:	15903008 	ldrne	r3, [r0, #8]
    4e34:	11a030a3 	lsrne	r3, r3, #1
    4e38:	12030001 	andne	r0, r3, #1	; 0x1
    4e3c:	112fff1e 	bxne	lr
    4e40:	e3a00000 	mov	r0, #0	; 0x0
    4e44:	e12fff1e 	bx	lr
    4e48:	0800d1a8 	.word	0x0800d1a8
    4e4c:	0000febc 	.word	0x0000febc

00004e50 <vs_device_burndst>:
    4e50:	e350000e 	cmp	r0, #14	; 0xe
    4e54:	059f3034 	ldreq	r3, [pc, #52]	; 4e90 <vs_device_burndst+0x40>
    4e58:	05930000 	ldreq	r0, [r3]
    4e5c:	0a000004 	beq	4e74 <vs_device_burndst+0x24>
    4e60:	e350000d 	cmp	r0, #13	; 0xd
    4e64:	8a000007 	bhi	4e88 <vs_device_burndst+0x38>
    4e68:	e59f2024 	ldr	r2, [pc, #36]	; 4e94 <vs_device_burndst+0x44>
    4e6c:	e3a0302c 	mov	r3, #44	; 0x2c
    4e70:	e0202093 	mla	r0, r3, r0, r2
    4e74:	e3500000 	cmp	r0, #0	; 0x0
    4e78:	15903008 	ldrne	r3, [r0, #8]
    4e7c:	11a03123 	lsrne	r3, r3, #2
    4e80:	12030001 	andne	r0, r3, #1	; 0x1
    4e84:	112fff1e 	bxne	lr
    4e88:	e3a00000 	mov	r0, #0	; 0x0
    4e8c:	e12fff1e 	bx	lr
    4e90:	0800d1a8 	.word	0x0800d1a8
    4e94:	0000febc 	.word	0x0000febc

00004e98 <vs_device_boot0>:
    4e98:	e350000e 	cmp	r0, #14	; 0xe
    4e9c:	059f3034 	ldreq	r3, [pc, #52]	; 4ed8 <vs_device_boot0+0x40>
    4ea0:	05930000 	ldreq	r0, [r3]
    4ea4:	0a000004 	beq	4ebc <vs_device_boot0+0x24>
    4ea8:	e350000d 	cmp	r0, #13	; 0xd
    4eac:	8a000007 	bhi	4ed0 <vs_device_boot0+0x38>
    4eb0:	e59f2024 	ldr	r2, [pc, #36]	; 4edc <vs_device_boot0+0x44>
    4eb4:	e3a0302c 	mov	r3, #44	; 0x2c
    4eb8:	e0202093 	mla	r0, r3, r0, r2
    4ebc:	e3500000 	cmp	r0, #0	; 0x0
    4ec0:	15903008 	ldrne	r3, [r0, #8]
    4ec4:	11a031a3 	lsrne	r3, r3, #3
    4ec8:	12030001 	andne	r0, r3, #1	; 0x1
    4ecc:	112fff1e 	bxne	lr
    4ed0:	e3a00000 	mov	r0, #0	; 0x0
    4ed4:	e12fff1e 	bx	lr
    4ed8:	0800d1a8 	.word	0x0800d1a8
    4edc:	0000febc 	.word	0x0000febc

00004ee0 <vs_device_name>:
    4ee0:	e350000e 	cmp	r0, #14	; 0xe
    4ee4:	059f3028 	ldreq	r3, [pc, #40]	; 4f14 <vs_device_name+0x34>
    4ee8:	05930000 	ldreq	r0, [r3]
    4eec:	0a000004 	beq	4f04 <vs_device_name+0x24>
    4ef0:	e350000d 	cmp	r0, #13	; 0xd
    4ef4:	8a000004 	bhi	4f0c <vs_device_name+0x2c>
    4ef8:	e59f2018 	ldr	r2, [pc, #24]	; 4f18 <vs_device_name+0x38>
    4efc:	e3a0302c 	mov	r3, #44	; 0x2c
    4f00:	e0202093 	mla	r0, r3, r0, r2
    4f04:	e3500000 	cmp	r0, #0	; 0x0
    4f08:	112fff1e 	bxne	lr
    4f0c:	e3a00000 	mov	r0, #0	; 0x0
    4f10:	e12fff1e 	bx	lr
    4f14:	0800d1a8 	.word	0x0800d1a8
    4f18:	0000febc 	.word	0x0000febc

00004f1c <vs_device_erasable>:
    4f1c:	e350000e 	cmp	r0, #14	; 0xe
    4f20:	059f3034 	ldreq	r3, [pc, #52]	; 4f5c <vs_device_erasable+0x40>
    4f24:	05930000 	ldreq	r0, [r3]
    4f28:	0a000004 	beq	4f40 <vs_device_erasable+0x24>
    4f2c:	e350000d 	cmp	r0, #13	; 0xd
    4f30:	8a000007 	bhi	4f54 <vs_device_erasable+0x38>
    4f34:	e59f2024 	ldr	r2, [pc, #36]	; 4f60 <vs_device_erasable+0x44>
    4f38:	e3a0302c 	mov	r3, #44	; 0x2c
    4f3c:	e0202093 	mla	r0, r3, r0, r2
    4f40:	e3500000 	cmp	r0, #0	; 0x0
    4f44:	15903008 	ldrne	r3, [r0, #8]
    4f48:	11a03223 	lsrne	r3, r3, #4
    4f4c:	12030001 	andne	r0, r3, #1	; 0x1
    4f50:	112fff1e 	bxne	lr
    4f54:	e3a00000 	mov	r0, #0	; 0x0
    4f58:	e12fff1e 	bx	lr
    4f5c:	0800d1a8 	.word	0x0800d1a8
    4f60:	0000febc 	.word	0x0000febc

00004f64 <vs_device_count>:
    4f64:	e3a0000e 	mov	r0, #14	; 0xe
    4f68:	e12fff1e 	bx	lr

00004f6c <vs_device_id>:
    4f6c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4f70:	e59f7040 	ldr	r7, [pc, #64]	; 4fb8 <vs_device_id+0x4c>
    4f74:	e24dd004 	sub	sp, sp, #4	; 0x4
    4f78:	e1a05000 	mov	r5, r0
    4f7c:	e3a04000 	mov	r4, #0	; 0x0
    4f80:	e3a0602c 	mov	r6, #44	; 0x2c
    4f84:	e0217496 	mla	r1, r6, r4, r7
    4f88:	e1a00005 	mov	r0, r5
    4f8c:	e3a02010 	mov	r2, #16	; 0x10
    4f90:	ebfff0d7 	bl	12f4 <strncmp>
    4f94:	e3500000 	cmp	r0, #0	; 0x0
    4f98:	0a000003 	beq	4fac <vs_device_id+0x40>
    4f9c:	e2844001 	add	r4, r4, #1	; 0x1
    4fa0:	e354000e 	cmp	r4, #14	; 0xe
    4fa4:	1afffff6 	bne	4f84 <vs_device_id+0x18>
    4fa8:	e2844001 	add	r4, r4, #1	; 0x1
    4fac:	e1a00004 	mov	r0, r4
    4fb0:	e28dd004 	add	sp, sp, #4	; 0x4
    4fb4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4fb8:	0000febc 	.word	0x0000febc

00004fbc <vs_assign_by_name>:
    4fbc:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    4fc0:	e59f3084 	ldr	r3, [pc, #132]	; 504c <vs_assign_by_name+0x90>
    4fc4:	e3a02000 	mov	r2, #0	; 0x0
    4fc8:	e24dd004 	sub	sp, sp, #4	; 0x4
    4fcc:	e5832000 	str	r2, [r3]
    4fd0:	e1a06000 	mov	r6, r0
    4fd4:	e1a07001 	mov	r7, r1
    4fd8:	e1a04002 	mov	r4, r2
    4fdc:	e3a0a02c 	mov	sl, #44	; 0x2c
    4fe0:	e59f8068 	ldr	r8, [pc, #104]	; 5050 <vs_assign_by_name+0x94>
    4fe4:	ea00000d 	b	5020 <vs_assign_by_name+0x64>
    4fe8:	e025849a 	mla	r5, sl, r4, r8
    4fec:	e1a00005 	mov	r0, r5
    4ff0:	ebfff0bf 	bl	12f4 <strncmp>
    4ff4:	e3500000 	cmp	r0, #0	; 0x0
    4ff8:	1a000007 	bne	501c <vs_assign_by_name+0x60>
    4ffc:	e59f3048 	ldr	r3, [pc, #72]	; 504c <vs_assign_by_name+0x90>
    5000:	e3570000 	cmp	r7, #0	; 0x0
    5004:	e5835000 	str	r5, [r3]
    5008:	01a00007 	moveq	r0, r7
    500c:	0a00000c 	beq	5044 <vs_assign_by_name+0x88>
    5010:	e28dd004 	add	sp, sp, #4	; 0x4
    5014:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    5018:	eafffed3 	b	4b6c <vs_reset>
    501c:	e2844001 	add	r4, r4, #1	; 0x1
    5020:	e354000e 	cmp	r4, #14	; 0xe
    5024:	e1a01006 	mov	r1, r6
    5028:	e3a02010 	mov	r2, #16	; 0x10
    502c:	1affffed 	bne	4fe8 <vs_assign_by_name+0x2c>
    5030:	e59f001c 	ldr	r0, [pc, #28]	; 5054 <vs_assign_by_name+0x98>
    5034:	e59f301c 	ldr	r3, [pc, #28]	; 5058 <vs_assign_by_name+0x9c>
    5038:	e1a0e00f 	mov	lr, pc
    503c:	e593f000 	ldr	pc, [r3]
    5040:	e3e00000 	mvn	r0, #0	; 0x0
    5044:	e28dd004 	add	sp, sp, #4	; 0x4
    5048:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    504c:	0800d1a8 	.word	0x0800d1a8
    5050:	0000febc 	.word	0x0000febc
    5054:	00011e2f 	.word	0x00011e2f
    5058:	0800d704 	.word	0x0800d704

0000505c <vs_is_device>:
    505c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5060:	e350000e 	cmp	r0, #14	; 0xe
    5064:	059f304c 	ldreq	r3, [pc, #76]	; 50b8 <vs_is_device+0x5c>
    5068:	e1a0c001 	mov	ip, r1
    506c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5070:	05931000 	ldreq	r1, [r3]
    5074:	0a000004 	beq	508c <vs_is_device+0x30>
    5078:	e350000d 	cmp	r0, #13	; 0xd
    507c:	8a00000a 	bhi	50ac <vs_is_device+0x50>
    5080:	e59f2034 	ldr	r2, [pc, #52]	; 50bc <vs_is_device+0x60>
    5084:	e3a0302c 	mov	r3, #44	; 0x2c
    5088:	e0212093 	mla	r1, r3, r0, r2
    508c:	e3510000 	cmp	r1, #0	; 0x0
    5090:	0a000005 	beq	50ac <vs_is_device+0x50>
    5094:	e1a0000c 	mov	r0, ip
    5098:	e3a02010 	mov	r2, #16	; 0x10
    509c:	ebfff094 	bl	12f4 <strncmp>
    50a0:	e2700001 	rsbs	r0, r0, #1	; 0x1
    50a4:	33a00000 	movcc	r0, #0	; 0x0
    50a8:	ea000000 	b	50b0 <vs_is_device+0x54>
    50ac:	e3a00000 	mov	r0, #0	; 0x0
    50b0:	e28dd004 	add	sp, sp, #4	; 0x4
    50b4:	e8bd8000 	pop	{pc}
    50b8:	0800d1a8 	.word	0x0800d1a8
    50bc:	0000febc 	.word	0x0000febc

000050c0 <vs_read>:
    50c0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    50c4:	e59f4238 	ldr	r4, [pc, #568]	; 5304 <vs_read+0x244>
    50c8:	e1a0b000 	mov	fp, r0
    50cc:	e5940000 	ldr	r0, [r4]
    50d0:	e24dd024 	sub	sp, sp, #36	; 0x24
    50d4:	e3500000 	cmp	r0, #0	; 0x0
    50d8:	e1a06002 	mov	r6, r2
    50dc:	e1a07003 	mov	r7, r3
    50e0:	e59d8048 	ldr	r8, [sp, #72]
    50e4:	03e05001 	mvneq	r5, #1	; 0x1
    50e8:	0a000082 	beq	52f8 <vs_read+0x238>
    50ec:	e5903010 	ldr	r3, [r0, #16]
    50f0:	e3530000 	cmp	r3, #0	; 0x0
    50f4:	03e05002 	mvneq	r5, #2	; 0x2
    50f8:	0a00007e 	beq	52f8 <vs_read+0x238>
    50fc:	e3580000 	cmp	r8, #0	; 0x0
    5100:	01a05008 	moveq	r5, r8
    5104:	0a00007b 	beq	52f8 <vs_read+0x238>
    5108:	e59f11f8 	ldr	r1, [pc, #504]	; 5308 <vs_read+0x248>
    510c:	e3a02003 	mov	r2, #3	; 0x3
    5110:	ebfff077 	bl	12f4 <strncmp>
    5114:	e3500000 	cmp	r0, #0	; 0x0
    5118:	1a00000a 	bne	5148 <vs_read+0x88>
    511c:	e59d204c 	ldr	r2, [sp, #76]
    5120:	e58d8000 	str	r8, [sp]
    5124:	e58d2004 	str	r2, [sp, #4]
    5128:	e5941000 	ldr	r1, [r4]
    512c:	e1a0000b 	mov	r0, fp
    5130:	e1a02006 	mov	r2, r6
    5134:	e1a03007 	mov	r3, r7
    5138:	e1a0e00f 	mov	lr, pc
    513c:	e591f010 	ldr	pc, [r1, #16]
    5140:	e1a05000 	mov	r5, r0
    5144:	ea00006b 	b	52f8 <vs_read+0x238>
    5148:	e3a0000e 	mov	r0, #14	; 0xe
    514c:	ebfffee7 	bl	4cf0 <vs_align>
    5150:	e2402001 	sub	r2, r0, #1	; 0x1
    5154:	e0124006 	ands	r4, r2, r6
    5158:	10644000 	rsbne	r4, r4, r0
    515c:	e0643008 	rsb	r3, r4, r8
    5160:	e0032002 	and	r2, r3, r2
    5164:	e1929004 	orrs	r9, r2, r4
    5168:	e1a0a000 	mov	sl, r0
    516c:	e58d3014 	str	r3, [sp, #20]
    5170:	e58d200c 	str	r2, [sp, #12]
    5174:	e58d9018 	str	r9, [sp, #24]
    5178:	01a05009 	moveq	r5, r9
    517c:	058d901c 	streq	r9, [sp, #28]
    5180:	0a00002d 	beq	523c <vs_read+0x17c>
    5184:	ebfff0f8 	bl	156c <malloc>
    5188:	e2502000 	subs	r2, r0, #0	; 0x0
    518c:	e58d201c 	str	r2, [sp, #28]
    5190:	1a00000c 	bne	51c8 <vs_read+0x108>
    5194:	e59f4170 	ldr	r4, [pc, #368]	; 530c <vs_read+0x24c>
    5198:	e59f0170 	ldr	r0, [pc, #368]	; 5310 <vs_read+0x250>
    519c:	e3001112 	movw	r1, #274	; 0x112
    51a0:	e1a0e00f 	mov	lr, pc
    51a4:	e594f000 	ldr	pc, [r4]
    51a8:	e59f0164 	ldr	r0, [pc, #356]	; 5314 <vs_read+0x254>
    51ac:	e59f1164 	ldr	r1, [pc, #356]	; 5318 <vs_read+0x258>
    51b0:	e1a0e00f 	mov	lr, pc
    51b4:	e594f000 	ldr	pc, [r4]
    51b8:	e59f015c 	ldr	r0, [pc, #348]	; 531c <vs_read+0x25c>
    51bc:	e1a0e00f 	mov	lr, pc
    51c0:	e594f000 	ldr	pc, [r4]
    51c4:	eafffffe 	b	51c4 <vs_read+0x104>
    51c8:	e3540000 	cmp	r4, #0	; 0x0
    51cc:	01a05004 	moveq	r5, r4
    51d0:	0a000019 	beq	523c <vs_read+0x17c>
    51d4:	e59d804c 	ldr	r8, [sp, #76]
    51d8:	e59f2124 	ldr	r2, [pc, #292]	; 5304 <vs_read+0x244>
    51dc:	e26a3000 	rsb	r3, sl, #0	; 0x0
    51e0:	e5921000 	ldr	r1, [r2]
    51e4:	e3a09000 	mov	r9, #0	; 0x0
    51e8:	e58d8004 	str	r8, [sp, #4]
    51ec:	e0068003 	and	r8, r6, r3
    51f0:	e58da000 	str	sl, [sp]
    51f4:	e1a02008 	mov	r2, r8
    51f8:	e1a03009 	mov	r3, r9
    51fc:	e1a0e00f 	mov	lr, pc
    5200:	e591f010 	ldr	pc, [r1, #16]
    5204:	e2505000 	subs	r5, r0, #0	; 0x0
    5208:	ba000038 	blt	52f0 <vs_read+0x230>
    520c:	e59d901c 	ldr	r9, [sp, #28]
    5210:	e064100a 	rsb	r1, r4, sl
    5214:	e1a0000b 	mov	r0, fp
    5218:	e0891001 	add	r1, r9, r1
    521c:	e1a02004 	mov	r2, r4
    5220:	ebfff06c 	bl	13d8 <memcpy>
    5224:	e1550004 	cmp	r5, r4
    5228:	21a05004 	movcs	r5, r4
    522c:	e59d8014 	ldr	r8, [sp, #20]
    5230:	e0966004 	adds	r6, r6, r4
    5234:	e2a77000 	adc	r7, r7, #0	; 0x0
    5238:	e08bb004 	add	fp, fp, r4
    523c:	e59d200c 	ldr	r2, [sp, #12]
    5240:	e0584002 	subs	r4, r8, r2
    5244:	0a00000f 	beq	5288 <vs_read+0x1c8>
    5248:	e59f30b4 	ldr	r3, [pc, #180]	; 5304 <vs_read+0x244>
    524c:	e58d4000 	str	r4, [sp]
    5250:	e5931000 	ldr	r1, [r3]
    5254:	e59d304c 	ldr	r3, [sp, #76]
    5258:	e1a0000b 	mov	r0, fp
    525c:	e58d3004 	str	r3, [sp, #4]
    5260:	e1a02006 	mov	r2, r6
    5264:	e1a03007 	mov	r3, r7
    5268:	e1a0e00f 	mov	lr, pc
    526c:	e591f010 	ldr	pc, [r1, #16]
    5270:	e3500000 	cmp	r0, #0	; 0x0
    5274:	ba000012 	blt	52c4 <vs_read+0x204>
    5278:	e0966004 	adds	r6, r6, r4
    527c:	e0a77fc4 	adc	r7, r7, r4, asr #31
    5280:	e0855000 	add	r5, r5, r0
    5284:	e08bb004 	add	fp, fp, r4
    5288:	e59d800c 	ldr	r8, [sp, #12]
    528c:	e3580000 	cmp	r8, #0	; 0x0
    5290:	0a000013 	beq	52e4 <vs_read+0x224>
    5294:	e59d904c 	ldr	r9, [sp, #76]
    5298:	e59f3064 	ldr	r3, [pc, #100]	; 5304 <vs_read+0x244>
    529c:	e58da000 	str	sl, [sp]
    52a0:	e58d9004 	str	r9, [sp, #4]
    52a4:	e5931000 	ldr	r1, [r3]
    52a8:	e1a02006 	mov	r2, r6
    52ac:	e1a03007 	mov	r3, r7
    52b0:	e59d001c 	ldr	r0, [sp, #28]
    52b4:	e1a0e00f 	mov	lr, pc
    52b8:	e591f010 	ldr	pc, [r1, #16]
    52bc:	e3500000 	cmp	r0, #0	; 0x0
    52c0:	aa000001 	bge	52cc <vs_read+0x20c>
    52c4:	e1a05000 	mov	r5, r0
    52c8:	ea000005 	b	52e4 <vs_read+0x224>
    52cc:	e59d200c 	ldr	r2, [sp, #12]
    52d0:	e1a0000b 	mov	r0, fp
    52d4:	e59d101c 	ldr	r1, [sp, #28]
    52d8:	ebfff03e 	bl	13d8 <memcpy>
    52dc:	e59d200c 	ldr	r2, [sp, #12]
    52e0:	e0855002 	add	r5, r5, r2
    52e4:	e59d3018 	ldr	r3, [sp, #24]
    52e8:	e3530000 	cmp	r3, #0	; 0x0
    52ec:	0a000001 	beq	52f8 <vs_read+0x238>
    52f0:	e59d001c 	ldr	r0, [sp, #28]
    52f4:	ebfff104 	bl	170c <free>
    52f8:	e1a00005 	mov	r0, r5
    52fc:	e28dd024 	add	sp, sp, #36	; 0x24
    5300:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5304:	0800d1a8 	.word	0x0800d1a8
    5308:	00011e4a 	.word	0x00011e4a
    530c:	0800d704 	.word	0x0800d704
    5310:	00011e4e 	.word	0x00011e4e
    5314:	00010c7b 	.word	0x00010c7b
    5318:	00011e60 	.word	0x00011e60
    531c:	00011e6b 	.word	0x00011e6b

00005320 <ss_lock_isk>:
    5320:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5324:	e24dd004 	sub	sp, sp, #4	; 0x4
    5328:	eb0001d5 	bl	5a84 <security_enabled>
    532c:	e3500000 	cmp	r0, #0	; 0x0
    5330:	0a000002 	beq	5340 <ss_lock_isk+0x20>
    5334:	e28dd004 	add	sp, sp, #4	; 0x4
    5338:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    533c:	ea00002e 	b	53fc <block_cipher_lock_isk>
    5340:	e28dd004 	add	sp, sp, #4	; 0x4
    5344:	e8bd8000 	pop	{pc}

00005348 <ss_aes_data>:
    5348:	e92d4070 	push	{r4, r5, r6, lr}
    534c:	e1a05000 	mov	r5, r0
    5350:	e1a06001 	mov	r6, r1
    5354:	e1a04002 	mov	r4, r2
    5358:	eb0001c9 	bl	5a84 <security_enabled>
    535c:	e3500000 	cmp	r0, #0	; 0x0
    5360:	0a000007 	beq	5384 <ss_aes_data+0x3c>
    5364:	e284200f 	add	r2, r4, #15	; 0xf
    5368:	e1a00005 	mov	r0, r5
    536c:	e1a01006 	mov	r1, r6
    5370:	e3c2200f 	bic	r2, r2, #15	; 0xf
    5374:	eb0000a0 	bl	55fc <block_cipher_operation>
    5378:	e3500000 	cmp	r0, #0	; 0x0
    537c:	13e00000 	mvnne	r0, #0	; 0x0
    5380:	18bd8070 	popne	{r4, r5, r6, pc}
    5384:	e3a00000 	mov	r0, #0	; 0x0
    5388:	e8bd8070 	pop	{r4, r5, r6, pc}

0000538c <ss_aes_init>:
    538c:	e92d4070 	push	{r4, r5, r6, lr}
    5390:	e1a05000 	mov	r5, r0
    5394:	e1a04001 	mov	r4, r1
    5398:	e1a06002 	mov	r6, r2
    539c:	eb0001b8 	bl	5a84 <security_enabled>
    53a0:	e3500000 	cmp	r0, #0	; 0x0
    53a4:	0a00000b 	beq	53d8 <ss_aes_init+0x4c>
    53a8:	e3540000 	cmp	r4, #0	; 0x0
    53ac:	01a00005 	moveq	r0, r5
    53b0:	01a02004 	moveq	r2, r4
    53b4:	01a03006 	moveq	r3, r6
    53b8:	03a01001 	moveq	r1, #1	; 0x1
    53bc:	11a00005 	movne	r0, r5
    53c0:	11a02004 	movne	r2, r4
    53c4:	11a03006 	movne	r3, r6
    53c8:	13a01000 	movne	r1, #0	; 0x0
    53cc:	eb00002c 	bl	5484 <block_cipher_aes_init>
    53d0:	e3500000 	cmp	r0, #0	; 0x0
    53d4:	1a000001 	bne	53e0 <ss_aes_init+0x54>
    53d8:	e3a00000 	mov	r0, #0	; 0x0
    53dc:	e8bd8070 	pop	{r4, r5, r6, pc}
    53e0:	e3e00000 	mvn	r0, #0	; 0x0
    53e4:	e8bd8070 	pop	{r4, r5, r6, pc}

000053e8 <check_register_bit>:
    53e8:	e3a03001 	mov	r3, #1	; 0x1
    53ec:	e0103113 	ands	r3, r0, r3, lsl r1
    53f0:	03a00000 	moveq	r0, #0	; 0x0
    53f4:	13a00001 	movne	r0, #1	; 0x1
    53f8:	e12fff1e 	bx	lr

000053fc <block_cipher_lock_isk>:
    53fc:	e59f200c 	ldr	r2, [pc, #12]	; 5410 <block_cipher_lock_isk+0x14>
    5400:	e5923400 	ldr	r3, [r2, #1024]
    5404:	e3833706 	orr	r3, r3, #1572864	; 0x180000
    5408:	e5823400 	str	r3, [r2, #1024]
    540c:	e12fff1e 	bx	lr
    5410:	27700000 	.word	0x27700000

00005414 <block_cipher_reset>:
    5414:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5418:	e59f4060 	ldr	r4, [pc, #96]	; 5480 <block_cipher_reset+0x6c>
    541c:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    5420:	e5843400 	str	r3, [r4, #1024]
    5424:	e24dd004 	sub	sp, sp, #4	; 0x4
    5428:	eb000d67 	bl	89cc <get_ticks>
    542c:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    5430:	e3a07000 	mov	r7, #0	; 0x0
    5434:	e0966000 	adds	r6, r6, r0
    5438:	e1a05004 	mov	r5, r4
    543c:	e0a77001 	adc	r7, r7, r1
    5440:	e5944400 	ldr	r4, [r4, #1024]
    5444:	ea000008 	b	546c <block_cipher_reset+0x58>
    5448:	e5954400 	ldr	r4, [r5, #1024]
    544c:	eb000d5e 	bl	89cc <get_ticks>
    5450:	e1510007 	cmp	r1, r7
    5454:	8a000002 	bhi	5464 <block_cipher_reset+0x50>
    5458:	1a000003 	bne	546c <block_cipher_reset+0x58>
    545c:	e1500006 	cmp	r0, r6
    5460:	9a000001 	bls	546c <block_cipher_reset+0x58>
    5464:	e3a00001 	mov	r0, #1	; 0x1
    5468:	ea000002 	b	5478 <block_cipher_reset+0x64>
    546c:	e3540000 	cmp	r4, #0	; 0x0
    5470:	bafffff4 	blt	5448 <block_cipher_reset+0x34>
    5474:	e3a00000 	mov	r0, #0	; 0x0
    5478:	e28dd004 	add	sp, sp, #4	; 0x4
    547c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5480:	27700000 	.word	0x27700000

00005484 <block_cipher_aes_init>:
    5484:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5488:	e24dd004 	sub	sp, sp, #4	; 0x4
    548c:	e1a05000 	mov	r5, r0
    5490:	e1a04001 	mov	r4, r1
    5494:	e1a06002 	mov	r6, r2
    5498:	e1a07003 	mov	r7, r3
    549c:	ebffffdc 	bl	5414 <block_cipher_reset>
    54a0:	e3500000 	cmp	r0, #0	; 0x0
    54a4:	1a00003d 	bne	55a0 <block_cipher_aes_init+0x11c>
    54a8:	e59f3134 	ldr	r3, [pc, #308]	; 55e4 <block_cipher_aes_init+0x160>
    54ac:	e3a02001 	mov	r2, #1	; 0x1
    54b0:	e3550001 	cmp	r5, #1	; 0x1
    54b4:	e5832404 	str	r2, [r3, #1028]
    54b8:	8a000038 	bhi	55a0 <block_cipher_aes_init+0x11c>
    54bc:	e3540003 	cmp	r4, #3	; 0x3
    54c0:	13540000 	cmpne	r4, #0	; 0x0
    54c4:	0a000001 	beq	54d0 <block_cipher_aes_init+0x4c>
    54c8:	e1540002 	cmp	r4, r2
    54cc:	1a000033 	bne	55a0 <block_cipher_aes_init+0x11c>
    54d0:	e1a03305 	lsl	r3, r5, #6
    54d4:	e3833901 	orr	r3, r3, #16384	; 0x4000
    54d8:	e3833004 	orr	r3, r3, #4	; 0x4
    54dc:	e59f2100 	ldr	r2, [pc, #256]	; 55e4 <block_cipher_aes_init+0x160>
    54e0:	e1833984 	orr	r3, r3, r4, lsl #19
    54e4:	e3540000 	cmp	r4, #0	; 0x0
    54e8:	e5823400 	str	r3, [r2, #1024]
    54ec:	1a000008 	bne	5514 <block_cipher_aes_init+0x90>
    54f0:	e59f10f0 	ldr	r1, [pc, #240]	; 55e8 <block_cipher_aes_init+0x164>
    54f4:	e59f00f0 	ldr	r0, [pc, #240]	; 55ec <block_cipher_aes_init+0x168>
    54f8:	e59f30f0 	ldr	r3, [pc, #240]	; 55f0 <block_cipher_aes_init+0x16c>
    54fc:	e0862001 	add	r2, r6, r1
    5500:	e0823003 	add	r3, r2, r3
    5504:	e5933000 	ldr	r3, [r3]
    5508:	e4813004 	str	r3, [r1], #4
    550c:	e1510000 	cmp	r1, r0
    5510:	1afffff8 	bne	54f8 <block_cipher_aes_init+0x74>
    5514:	e59f10d0 	ldr	r1, [pc, #208]	; 55ec <block_cipher_aes_init+0x168>
    5518:	e59f00d4 	ldr	r0, [pc, #212]	; 55f4 <block_cipher_aes_init+0x170>
    551c:	e59f30d4 	ldr	r3, [pc, #212]	; 55f8 <block_cipher_aes_init+0x174>
    5520:	e0872001 	add	r2, r7, r1
    5524:	e0823003 	add	r3, r2, r3
    5528:	e5933000 	ldr	r3, [r3]
    552c:	e4813004 	str	r3, [r1], #4
    5530:	e1510000 	cmp	r1, r0
    5534:	1afffff8 	bne	551c <block_cipher_aes_init+0x98>
    5538:	e3540000 	cmp	r4, #0	; 0x0
    553c:	1a000008 	bne	5564 <block_cipher_aes_init+0xe0>
    5540:	eb000d21 	bl	89cc <get_ticks>
    5544:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    5548:	e3a05000 	mov	r5, #0	; 0x0
    554c:	e0944000 	adds	r4, r4, r0
    5550:	e59f608c 	ldr	r6, [pc, #140]	; 55e4 <block_cipher_aes_init+0x160>
    5554:	e0a55001 	adc	r5, r5, r1
    5558:	e596340c 	ldr	r3, [r6, #1036]
    555c:	e3130008 	tst	r3, #8	; 0x8
    5560:	0a000011 	beq	55ac <block_cipher_aes_init+0x128>
    5564:	eb000d18 	bl	89cc <get_ticks>
    5568:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    556c:	e3a05000 	mov	r5, #0	; 0x0
    5570:	e0944000 	adds	r4, r4, r0
    5574:	e59f6068 	ldr	r6, [pc, #104]	; 55e4 <block_cipher_aes_init+0x160>
    5578:	e0a55001 	adc	r5, r5, r1
    557c:	e596340c 	ldr	r3, [r6, #1036]
    5580:	e59f205c 	ldr	r2, [pc, #92]	; 55e4 <block_cipher_aes_init+0x160>
    5584:	e3130010 	tst	r3, #16	; 0x10
    5588:	0a00000e 	beq	55c8 <block_cipher_aes_init+0x144>
    558c:	e5923400 	ldr	r3, [r2, #1024]
    5590:	e3a00000 	mov	r0, #0	; 0x0
    5594:	e3833001 	orr	r3, r3, #1	; 0x1
    5598:	e5823400 	str	r3, [r2, #1024]
    559c:	ea000000 	b	55a4 <block_cipher_aes_init+0x120>
    55a0:	e3a00001 	mov	r0, #1	; 0x1
    55a4:	e28dd004 	add	sp, sp, #4	; 0x4
    55a8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    55ac:	eb000d06 	bl	89cc <get_ticks>
    55b0:	e1510005 	cmp	r1, r5
    55b4:	8afffff9 	bhi	55a0 <block_cipher_aes_init+0x11c>
    55b8:	1affffe6 	bne	5558 <block_cipher_aes_init+0xd4>
    55bc:	e1500004 	cmp	r0, r4
    55c0:	8afffff6 	bhi	55a0 <block_cipher_aes_init+0x11c>
    55c4:	eaffffe3 	b	5558 <block_cipher_aes_init+0xd4>
    55c8:	eb000cff 	bl	89cc <get_ticks>
    55cc:	e1510005 	cmp	r1, r5
    55d0:	8afffff2 	bhi	55a0 <block_cipher_aes_init+0x11c>
    55d4:	1affffe8 	bne	557c <block_cipher_aes_init+0xf8>
    55d8:	e1500004 	cmp	r0, r4
    55dc:	8affffef 	bhi	55a0 <block_cipher_aes_init+0x11c>
    55e0:	eaffffe5 	b	557c <block_cipher_aes_init+0xf8>
    55e4:	27700000 	.word	0x27700000
    55e8:	27700420 	.word	0x27700420
    55ec:	27700440 	.word	0x27700440
    55f0:	d88ffbe0 	.word	0xd88ffbe0
    55f4:	27700450 	.word	0x27700450
    55f8:	d88ffbc0 	.word	0xd88ffbc0

000055fc <block_cipher_operation>:
    55fc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    5600:	e24dd00c 	sub	sp, sp, #12	; 0xc
    5604:	e1a08002 	mov	r8, r2
    5608:	e58d0004 	str	r0, [sp, #4]
    560c:	e1a09001 	mov	r9, r1
    5610:	e1a06002 	mov	r6, r2
    5614:	e59fa0c4 	ldr	sl, [pc, #196]	; 56e0 <block_cipher_operation+0xe4>
    5618:	e3a0b000 	mov	fp, #0	; 0x0
    561c:	ea000028 	b	56c4 <block_cipher_operation+0xc8>
    5620:	e59d3004 	ldr	r3, [sp, #4]
    5624:	e3560902 	cmp	r6, #32768	; 0x8000
    5628:	b1a07006 	movlt	r7, r6
    562c:	a3a07902 	movge	r7, #32768	; 0x8000
    5630:	e0664008 	rsb	r4, r6, r8
    5634:	e3a00000 	mov	r0, #0	; 0x0
    5638:	e0831004 	add	r1, r3, r4
    563c:	e1a02007 	mov	r2, r7
    5640:	eb0000ed 	bl	59fc <dma_sdma_init>
    5644:	e3500000 	cmp	r0, #0	; 0x0
    5648:	1a000021 	bne	56d4 <block_cipher_operation+0xd8>
    564c:	e0891004 	add	r1, r9, r4
    5650:	e2800001 	add	r0, r0, #1	; 0x1
    5654:	e1a02007 	mov	r2, r7
    5658:	eb0000e7 	bl	59fc <dma_sdma_init>
    565c:	e3500000 	cmp	r0, #0	; 0x0
    5660:	1a00001b 	bne	56d4 <block_cipher_operation+0xd8>
    5664:	eb0000d4 	bl	59bc <dma_start>
    5668:	e3a00001 	mov	r0, #1	; 0x1
    566c:	eb0000d2 	bl	59bc <dma_start>
    5670:	eb000cd5 	bl	89cc <get_ticks>
    5674:	e090400a 	adds	r4, r0, sl
    5678:	e0a1500b 	adc	r5, r1, fp
    567c:	ea000005 	b	5698 <block_cipher_operation+0x9c>
    5680:	eb000cd1 	bl	89cc <get_ticks>
    5684:	e1510005 	cmp	r1, r5
    5688:	8a000011 	bhi	56d4 <block_cipher_operation+0xd8>
    568c:	1a000001 	bne	5698 <block_cipher_operation+0x9c>
    5690:	e1500004 	cmp	r0, r4
    5694:	8a00000e 	bhi	56d4 <block_cipher_operation+0xd8>
    5698:	e3a01003 	mov	r1, #3	; 0x3
    569c:	e3a00001 	mov	r0, #1	; 0x1
    56a0:	eb0000cb 	bl	59d4 <dma_get_status>
    56a4:	e3500000 	cmp	r0, #0	; 0x0
    56a8:	0afffff4 	beq	5680 <block_cipher_operation+0x84>
    56ac:	e3a00000 	mov	r0, #0	; 0x0
    56b0:	e3a01003 	mov	r1, #3	; 0x3
    56b4:	eb0000c6 	bl	59d4 <dma_get_status>
    56b8:	e3500000 	cmp	r0, #0	; 0x0
    56bc:	0a000004 	beq	56d4 <block_cipher_operation+0xd8>
    56c0:	e0676006 	rsb	r6, r7, r6
    56c4:	e3560000 	cmp	r6, #0	; 0x0
    56c8:	1affffd4 	bne	5620 <block_cipher_operation+0x24>
    56cc:	e1a00006 	mov	r0, r6
    56d0:	ea000000 	b	56d8 <block_cipher_operation+0xdc>
    56d4:	e3a00001 	mov	r0, #1	; 0x1
    56d8:	e28dd00c 	add	sp, sp, #12	; 0xc
    56dc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    56e0:	000f4240 	.word	0x000f4240

000056e4 <hash_len>:
    56e4:	e59f300c 	ldr	r3, [pc, #12]	; 56f8 <hash_len+0x14>
    56e8:	e5932000 	ldr	r2, [r3]
    56ec:	e59f3008 	ldr	r3, [pc, #8]	; 56fc <hash_len+0x18>
    56f0:	e7930202 	ldr	r0, [r3, r2, lsl #4]
    56f4:	e12fff1e 	bx	lr
    56f8:	0800d1ac 	.word	0x0800d1ac
    56fc:	00010124 	.word	0x00010124

00005700 <hash_value>:
    5700:	e92d4070 	push	{r4, r5, r6, lr}
    5704:	e59f6054 	ldr	r6, [pc, #84]	; 5760 <hash_value+0x60>
    5708:	e1a05000 	mov	r5, r0
    570c:	e5963000 	ldr	r3, [r6]
    5710:	e3530000 	cmp	r3, #0	; 0x0
    5714:	0a000009 	beq	5740 <hash_value+0x40>
    5718:	eb0000d9 	bl	5a84 <security_enabled>
    571c:	e2504000 	subs	r4, r0, #0	; 0x0
    5720:	1a000006 	bne	5740 <hash_value+0x40>
    5724:	e59f0038 	ldr	r0, [pc, #56]	; 5764 <hash_value+0x64>
    5728:	e5961000 	ldr	r1, [r6]
    572c:	e59f3034 	ldr	r3, [pc, #52]	; 5768 <hash_value+0x68>
    5730:	e1a0e00f 	mov	lr, pc
    5734:	e593f000 	ldr	pc, [r3]
    5738:	e1a00004 	mov	r0, r4
    573c:	e8bd8070 	pop	{r4, r5, r6, pc}
    5740:	e59f3018 	ldr	r3, [pc, #24]	; 5760 <hash_value+0x60>
    5744:	e1a00005 	mov	r0, r5
    5748:	e5932000 	ldr	r2, [r3]
    574c:	e59f3018 	ldr	r3, [pc, #24]	; 576c <hash_value+0x6c>
    5750:	e0833202 	add	r3, r3, r2, lsl #4
    5754:	e1a0e00f 	mov	lr, pc
    5758:	e593f00c 	ldr	pc, [r3, #12]
    575c:	e8bd8070 	pop	{r4, r5, r6, pc}
    5760:	0800d1ac 	.word	0x0800d1ac
    5764:	00011e9c 	.word	0x00011e9c
    5768:	0800d704 	.word	0x0800d704
    576c:	00010124 	.word	0x00010124

00005770 <hash_data>:
    5770:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5774:	e59f5074 	ldr	r5, [pc, #116]	; 57f0 <hash_data+0x80>
    5778:	e24dd004 	sub	sp, sp, #4	; 0x4
    577c:	e5953000 	ldr	r3, [r5]
    5780:	e1a06000 	mov	r6, r0
    5784:	e3530000 	cmp	r3, #0	; 0x0
    5788:	e1a07001 	mov	r7, r1
    578c:	0a000009 	beq	57b8 <hash_data+0x48>
    5790:	eb0000bb 	bl	5a84 <security_enabled>
    5794:	e2504000 	subs	r4, r0, #0	; 0x0
    5798:	1a000006 	bne	57b8 <hash_data+0x48>
    579c:	e59f0050 	ldr	r0, [pc, #80]	; 57f4 <hash_data+0x84>
    57a0:	e5951000 	ldr	r1, [r5]
    57a4:	e59f304c 	ldr	r3, [pc, #76]	; 57f8 <hash_data+0x88>
    57a8:	e1a0e00f 	mov	lr, pc
    57ac:	e593f000 	ldr	pc, [r3]
    57b0:	e1a00004 	mov	r0, r4
    57b4:	ea00000b 	b	57e8 <hash_data+0x78>
    57b8:	e59f003c 	ldr	r0, [pc, #60]	; 57fc <hash_data+0x8c>
    57bc:	e59f3034 	ldr	r3, [pc, #52]	; 57f8 <hash_data+0x88>
    57c0:	e1a0e00f 	mov	lr, pc
    57c4:	e593f000 	ldr	pc, [r3]
    57c8:	e59f3020 	ldr	r3, [pc, #32]	; 57f0 <hash_data+0x80>
    57cc:	e1a00006 	mov	r0, r6
    57d0:	e5932000 	ldr	r2, [r3]
    57d4:	e59f3024 	ldr	r3, [pc, #36]	; 5800 <hash_data+0x90>
    57d8:	e1a01007 	mov	r1, r7
    57dc:	e0833202 	add	r3, r3, r2, lsl #4
    57e0:	e1a0e00f 	mov	lr, pc
    57e4:	e593f008 	ldr	pc, [r3, #8]
    57e8:	e28dd004 	add	sp, sp, #4	; 0x4
    57ec:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    57f0:	0800d1ac 	.word	0x0800d1ac
    57f4:	00011e9c 	.word	0x00011e9c
    57f8:	0800d704 	.word	0x0800d704
    57fc:	00011ebd 	.word	0x00011ebd
    5800:	00010124 	.word	0x00010124

00005804 <hash_init>:
    5804:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5808:	e3500003 	cmp	r0, #3	; 0x3
    580c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5810:	e1a04000 	mov	r4, r0
    5814:	e1a07001 	mov	r7, r1
    5818:	9a000005 	bls	5834 <hash_init+0x30>
    581c:	e59f0064 	ldr	r0, [pc, #100]	; 5888 <hash_init+0x84>
    5820:	e59f3064 	ldr	r3, [pc, #100]	; 588c <hash_init+0x88>
    5824:	e1a0e00f 	mov	lr, pc
    5828:	e593f000 	ldr	pc, [r3]
    582c:	e3e00000 	mvn	r0, #0	; 0x0
    5830:	ea000012 	b	5880 <hash_init+0x7c>
    5834:	e59f6054 	ldr	r6, [pc, #84]	; 5890 <hash_init+0x8c>
    5838:	e3500000 	cmp	r0, #0	; 0x0
    583c:	e5860000 	str	r0, [r6]
    5840:	0a000009 	beq	586c <hash_init+0x68>
    5844:	eb00008e 	bl	5a84 <security_enabled>
    5848:	e2505000 	subs	r5, r0, #0	; 0x0
    584c:	1a000006 	bne	586c <hash_init+0x68>
    5850:	e59f003c 	ldr	r0, [pc, #60]	; 5894 <hash_init+0x90>
    5854:	e5961000 	ldr	r1, [r6]
    5858:	e59f302c 	ldr	r3, [pc, #44]	; 588c <hash_init+0x88>
    585c:	e1a0e00f 	mov	lr, pc
    5860:	e593f000 	ldr	pc, [r3]
    5864:	e1a00005 	mov	r0, r5
    5868:	ea000004 	b	5880 <hash_init+0x7c>
    586c:	e59f3024 	ldr	r3, [pc, #36]	; 5898 <hash_init+0x94>
    5870:	e1a00007 	mov	r0, r7
    5874:	e0833204 	add	r3, r3, r4, lsl #4
    5878:	e1a0e00f 	mov	lr, pc
    587c:	e593f004 	ldr	pc, [r3, #4]
    5880:	e28dd004 	add	sp, sp, #4	; 0x4
    5884:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5888:	00011ec8 	.word	0x00011ec8
    588c:	0800d704 	.word	0x0800d704
    5890:	0800d1ac 	.word	0x0800d1ac
    5894:	00011e9c 	.word	0x00011e9c
    5898:	00010124 	.word	0x00010124

0000589c <md5_value>:
    589c:	e92d4030 	push	{r4, r5, lr}
    58a0:	e59f3054 	ldr	r3, [pc, #84]	; 58fc <md5_value+0x60>
    58a4:	e1a05000 	mov	r5, r0
    58a8:	e893000c 	ldm	r3, {r2, r3}
    58ac:	e1823003 	orr	r3, r2, r3
    58b0:	e24dd024 	sub	sp, sp, #36	; 0x24
    58b4:	e3530000 	cmp	r3, #0	; 0x0
    58b8:	13e00000 	mvnne	r0, #0	; 0x0
    58bc:	1a00000c 	bne	58f4 <md5_value+0x58>
    58c0:	e1a0000d 	mov	r0, sp
    58c4:	eb000178 	bl	5eac <hash_module_get_result>
    58c8:	e3500000 	cmp	r0, #0	; 0x0
    58cc:	e1a0400d 	mov	r4, sp
    58d0:	13e00001 	mvnne	r0, #1	; 0x1
    58d4:	1a000006 	bne	58f4 <md5_value+0x58>
    58d8:	e1a0200d 	mov	r2, sp
    58dc:	e7d23000 	ldrb	r3, [r2, r0]
    58e0:	e7c53000 	strb	r3, [r5, r0]
    58e4:	e2800001 	add	r0, r0, #1	; 0x1
    58e8:	e3500010 	cmp	r0, #16	; 0x10
    58ec:	1afffffa 	bne	58dc <md5_value+0x40>
    58f0:	e3a00000 	mov	r0, #0	; 0x0
    58f4:	e28dd024 	add	sp, sp, #36	; 0x24
    58f8:	e8bd8030 	pop	{r4, r5, pc}
    58fc:	0800d1b0 	.word	0x0800d1b0

00005900 <md5_data>:
    5900:	e59f305c 	ldr	r3, [pc, #92]	; 5964 <md5_data+0x64>
    5904:	e92d4070 	push	{r4, r5, r6, lr}
    5908:	e1a06001 	mov	r6, r1
    590c:	e5931004 	ldr	r1, [r3, #4]
    5910:	e1a04006 	mov	r4, r6
    5914:	e3a05000 	mov	r5, #0	; 0x0
    5918:	e1550001 	cmp	r5, r1
    591c:	e593c000 	ldr	ip, [r3]
    5920:	8a000002 	bhi	5930 <md5_data+0x30>
    5924:	1a000003 	bne	5938 <md5_data+0x38>
    5928:	e154000c 	cmp	r4, ip
    592c:	9a000001 	bls	5938 <md5_data+0x38>
    5930:	e3e00000 	mvn	r0, #0	; 0x0
    5934:	e8bd8070 	pop	{r4, r5, r6, pc}
    5938:	e1a0200c 	mov	r2, ip
    593c:	e1a03001 	mov	r3, r1
    5940:	e59fc01c 	ldr	ip, [pc, #28]	; 5964 <md5_data+0x64>
    5944:	e0522004 	subs	r2, r2, r4
    5948:	e0c33005 	sbc	r3, r3, r5
    594c:	e1a01006 	mov	r1, r6
    5950:	e88c000c 	stm	ip, {r2, r3}
    5954:	eb0001b7 	bl	6038 <hash_dma_transfer>
    5958:	e3500000 	cmp	r0, #0	; 0x0
    595c:	13e00001 	mvnne	r0, #1	; 0x1
    5960:	e8bd8070 	pop	{r4, r5, r6, pc}
    5964:	0800d1b0 	.word	0x0800d1b0

00005968 <md5_init>:
    5968:	e1a03ea0 	lsr	r3, r0, #29
    596c:	e1a0c180 	lsl	ip, r0, #3
    5970:	e92d4010 	push	{r4, lr}
    5974:	e59f403c 	ldr	r4, [pc, #60]	; 59b8 <md5_init+0x50>
    5978:	e1a0200c 	mov	r2, ip
    597c:	e1a01003 	mov	r1, r3
    5980:	e3a00000 	mov	r0, #0	; 0x0
    5984:	e5843004 	str	r3, [r4, #4]
    5988:	e584c000 	str	ip, [r4]
    598c:	eb00018d 	bl	5fc8 <hash_module_init>
    5990:	e3500000 	cmp	r0, #0	; 0x0
    5994:	05942000 	ldreq	r2, [r4]
    5998:	05943004 	ldreq	r3, [r4, #4]
    599c:	01a021a2 	lsreq	r2, r2, #3
    59a0:	01822e83 	orreq	r2, r2, r3, lsl #29
    59a4:	01a031a3 	lsreq	r3, r3, #3
    59a8:	13e00000 	mvnne	r0, #0	; 0x0
    59ac:	05843004 	streq	r3, [r4, #4]
    59b0:	05842000 	streq	r2, [r4]
    59b4:	e8bd8010 	pop	{r4, pc}
    59b8:	0800d1b0 	.word	0x0800d1b0

000059bc <dma_start>:
    59bc:	e280099d 	add	r0, r0, #2572288	; 0x274000
    59c0:	e2800a03 	add	r0, r0, #12288	; 0x3000
    59c4:	e1a00400 	lsl	r0, r0, #8
    59c8:	e3a03001 	mov	r3, #1	; 0x1
    59cc:	e5803000 	str	r3, [r0]
    59d0:	e12fff1e 	bx	lr

000059d4 <dma_get_status>:
    59d4:	e3a0c001 	mov	ip, #1	; 0x1
    59d8:	e1a0211c 	lsl	r2, ip, r1
    59dc:	e280099d 	add	r0, r0, #2572288	; 0x274000
    59e0:	e2800a03 	add	r0, r0, #12288	; 0x3000
    59e4:	e1a01400 	lsl	r1, r0, #8
    59e8:	e5913024 	ldr	r3, [r1, #36]
    59ec:	e0130002 	ands	r0, r3, r2
    59f0:	15812024 	strne	r2, [r1, #36]
    59f4:	11a0000c 	movne	r0, ip
    59f8:	e12fff1e 	bx	lr

000059fc <dma_sdma_init>:
    59fc:	e280c99d 	add	ip, r0, #2572288	; 0x274000
    5a00:	e28cca03 	add	ip, ip, #12288	; 0x3000
    5a04:	e1a0040c 	lsl	r0, ip, #8
    5a08:	e2800004 	add	r0, r0, #4	; 0x4
    5a0c:	e5903000 	ldr	r3, [r0]
    5a10:	e3520801 	cmp	r2, #65536	; 0x10000
    5a14:	e3833001 	orr	r3, r3, #1	; 0x1
    5a18:	e5803000 	str	r3, [r0]
    5a1c:	e92d4010 	push	{r4, lr}
    5a20:	e3a03341 	mov	r3, #67108865	; 0x4000001
    5a24:	e1a0440c 	lsl	r4, ip, #8
    5a28:	e584300c 	str	r3, [r4, #12]
    5a2c:	31a02802 	lslcc	r2, r2, #16
    5a30:	31a0340c 	lslcc	r3, ip, #8
    5a34:	31a02822 	lsrcc	r2, r2, #16
    5a38:	32833010 	addcc	r3, r3, #16	; 0x10
    5a3c:	35832000 	strcc	r2, [r3]
    5a40:	23a00001 	movcs	r0, #1	; 0x1
    5a44:	35841014 	strcc	r1, [r4, #20]
    5a48:	33a00000 	movcc	r0, #0	; 0x0
    5a4c:	e8bd8010 	pop	{r4, pc}

00005a50 <security_cke_control>:
    5a50:	e59fc014 	ldr	ip, [pc, #20]	; 5a6c <security_cke_control+0x1c>
    5a54:	e3a02001 	mov	r2, #1	; 0x1
    5a58:	e59c3000 	ldr	r3, [ip]
    5a5c:	e1c33012 	bic	r3, r3, r2, lsl r0
    5a60:	e1833011 	orr	r3, r3, r1, lsl r0
    5a64:	e58c3000 	str	r3, [ip]
    5a68:	e12fff1e 	bx	lr
    5a6c:	27702000 	.word	0x27702000

00005a70 <security_enable>:
    5a70:	e59f3008 	ldr	r3, [pc, #8]	; 5a80 <security_enable+0x10>
    5a74:	e3a02001 	mov	r2, #1	; 0x1
    5a78:	e5832000 	str	r2, [r3]
    5a7c:	e12fff1e 	bx	lr
    5a80:	0800d1b8 	.word	0x0800d1b8

00005a84 <security_enabled>:
    5a84:	e59f300c 	ldr	r3, [pc, #12]	; 5a98 <security_enabled+0x14>
    5a88:	e5930000 	ldr	r0, [r3]
    5a8c:	e2500000 	subs	r0, r0, #0	; 0x0
    5a90:	13a00001 	movne	r0, #1	; 0x1
    5a94:	e12fff1e 	bx	lr
    5a98:	0800d1b8 	.word	0x0800d1b8

00005a9c <dma_reset>:
    5a9c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5aa0:	e280099d 	add	r0, r0, #2572288	; 0x274000
    5aa4:	e2800a03 	add	r0, r0, #12288	; 0x3000
    5aa8:	e1a07400 	lsl	r7, r0, #8
    5aac:	e3a03002 	mov	r3, #2	; 0x2
    5ab0:	e24dd004 	sub	sp, sp, #4	; 0x4
    5ab4:	e5873000 	str	r3, [r7]
    5ab8:	eb000bc3 	bl	89cc <get_ticks>
    5abc:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    5ac0:	e3a05000 	mov	r5, #0	; 0x0
    5ac4:	e0944000 	adds	r4, r4, r0
    5ac8:	e0a55001 	adc	r5, r5, r1
    5acc:	e5976000 	ldr	r6, [r7]
    5ad0:	ea000008 	b	5af8 <dma_reset+0x5c>
    5ad4:	e5976000 	ldr	r6, [r7]
    5ad8:	eb000bbb 	bl	89cc <get_ticks>
    5adc:	e1510005 	cmp	r1, r5
    5ae0:	8a000002 	bhi	5af0 <dma_reset+0x54>
    5ae4:	1a000003 	bne	5af8 <dma_reset+0x5c>
    5ae8:	e1500004 	cmp	r0, r4
    5aec:	9a000001 	bls	5af8 <dma_reset+0x5c>
    5af0:	e3a00001 	mov	r0, #1	; 0x1
    5af4:	ea000002 	b	5b04 <dma_reset+0x68>
    5af8:	e3560000 	cmp	r6, #0	; 0x0
    5afc:	1afffff4 	bne	5ad4 <dma_reset+0x38>
    5b00:	e1a00006 	mov	r0, r6
    5b04:	e28dd004 	add	sp, sp, #4	; 0x4
    5b08:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00005b0c <sha1_value>:
    5b0c:	e92d4030 	push	{r4, r5, lr}
    5b10:	e59f3054 	ldr	r3, [pc, #84]	; 5b6c <sha1_value+0x60>
    5b14:	e1a05000 	mov	r5, r0
    5b18:	e893000c 	ldm	r3, {r2, r3}
    5b1c:	e1823003 	orr	r3, r2, r3
    5b20:	e24dd024 	sub	sp, sp, #36	; 0x24
    5b24:	e3530000 	cmp	r3, #0	; 0x0
    5b28:	13e00000 	mvnne	r0, #0	; 0x0
    5b2c:	1a00000c 	bne	5b64 <sha1_value+0x58>
    5b30:	e1a0000d 	mov	r0, sp
    5b34:	eb0000dc 	bl	5eac <hash_module_get_result>
    5b38:	e3500000 	cmp	r0, #0	; 0x0
    5b3c:	e1a0400d 	mov	r4, sp
    5b40:	13e00001 	mvnne	r0, #1	; 0x1
    5b44:	1a000006 	bne	5b64 <sha1_value+0x58>
    5b48:	e1a0200d 	mov	r2, sp
    5b4c:	e7d23000 	ldrb	r3, [r2, r0]
    5b50:	e7c53000 	strb	r3, [r5, r0]
    5b54:	e2800001 	add	r0, r0, #1	; 0x1
    5b58:	e3500014 	cmp	r0, #20	; 0x14
    5b5c:	1afffffa 	bne	5b4c <sha1_value+0x40>
    5b60:	e3a00000 	mov	r0, #0	; 0x0
    5b64:	e28dd024 	add	sp, sp, #36	; 0x24
    5b68:	e8bd8030 	pop	{r4, r5, pc}
    5b6c:	0800d1c0 	.word	0x0800d1c0

00005b70 <sha1_data>:
    5b70:	e59f305c 	ldr	r3, [pc, #92]	; 5bd4 <sha1_data+0x64>
    5b74:	e92d4070 	push	{r4, r5, r6, lr}
    5b78:	e1a06001 	mov	r6, r1
    5b7c:	e5931004 	ldr	r1, [r3, #4]
    5b80:	e1a04006 	mov	r4, r6
    5b84:	e3a05000 	mov	r5, #0	; 0x0
    5b88:	e1550001 	cmp	r5, r1
    5b8c:	e593c000 	ldr	ip, [r3]
    5b90:	8a000002 	bhi	5ba0 <sha1_data+0x30>
    5b94:	1a000003 	bne	5ba8 <sha1_data+0x38>
    5b98:	e154000c 	cmp	r4, ip
    5b9c:	9a000001 	bls	5ba8 <sha1_data+0x38>
    5ba0:	e3e00000 	mvn	r0, #0	; 0x0
    5ba4:	e8bd8070 	pop	{r4, r5, r6, pc}
    5ba8:	e1a0200c 	mov	r2, ip
    5bac:	e1a03001 	mov	r3, r1
    5bb0:	e59fc01c 	ldr	ip, [pc, #28]	; 5bd4 <sha1_data+0x64>
    5bb4:	e0522004 	subs	r2, r2, r4
    5bb8:	e0c33005 	sbc	r3, r3, r5
    5bbc:	e1a01006 	mov	r1, r6
    5bc0:	e88c000c 	stm	ip, {r2, r3}
    5bc4:	eb00011b 	bl	6038 <hash_dma_transfer>
    5bc8:	e3500000 	cmp	r0, #0	; 0x0
    5bcc:	13e00001 	mvnne	r0, #1	; 0x1
    5bd0:	e8bd8070 	pop	{r4, r5, r6, pc}
    5bd4:	0800d1c0 	.word	0x0800d1c0

00005bd8 <sha1_init>:
    5bd8:	e1a03ea0 	lsr	r3, r0, #29
    5bdc:	e1a0c180 	lsl	ip, r0, #3
    5be0:	e92d4010 	push	{r4, lr}
    5be4:	e59f403c 	ldr	r4, [pc, #60]	; 5c28 <sha1_init+0x50>
    5be8:	e1a0200c 	mov	r2, ip
    5bec:	e1a01003 	mov	r1, r3
    5bf0:	e3a00001 	mov	r0, #1	; 0x1
    5bf4:	e5843004 	str	r3, [r4, #4]
    5bf8:	e584c000 	str	ip, [r4]
    5bfc:	eb0000f1 	bl	5fc8 <hash_module_init>
    5c00:	e3500000 	cmp	r0, #0	; 0x0
    5c04:	05942000 	ldreq	r2, [r4]
    5c08:	05943004 	ldreq	r3, [r4, #4]
    5c0c:	01a021a2 	lsreq	r2, r2, #3
    5c10:	01822e83 	orreq	r2, r2, r3, lsl #29
    5c14:	01a031a3 	lsreq	r3, r3, #3
    5c18:	13e00000 	mvnne	r0, #0	; 0x0
    5c1c:	05843004 	streq	r3, [r4, #4]
    5c20:	05842000 	streq	r2, [r4]
    5c24:	e8bd8010 	pop	{r4, pc}
    5c28:	0800d1c0 	.word	0x0800d1c0

00005c2c <sha256_value>:
    5c2c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5c30:	e59f3028 	ldr	r3, [pc, #40]	; 5c60 <sha256_value+0x34>
    5c34:	e24dd004 	sub	sp, sp, #4	; 0x4
    5c38:	e893000c 	ldm	r3, {r2, r3}
    5c3c:	e1823003 	orr	r3, r2, r3
    5c40:	e3530000 	cmp	r3, #0	; 0x0
    5c44:	13e00000 	mvnne	r0, #0	; 0x0
    5c48:	1a000002 	bne	5c58 <sha256_value+0x2c>
    5c4c:	eb000096 	bl	5eac <hash_module_get_result>
    5c50:	e3500000 	cmp	r0, #0	; 0x0
    5c54:	13e00001 	mvnne	r0, #1	; 0x1
    5c58:	e28dd004 	add	sp, sp, #4	; 0x4
    5c5c:	e8bd8000 	pop	{pc}
    5c60:	0800d1c8 	.word	0x0800d1c8

00005c64 <sha256_data>:
    5c64:	e59f305c 	ldr	r3, [pc, #92]	; 5cc8 <sha256_data+0x64>
    5c68:	e92d4070 	push	{r4, r5, r6, lr}
    5c6c:	e1a06001 	mov	r6, r1
    5c70:	e5931004 	ldr	r1, [r3, #4]
    5c74:	e1a04006 	mov	r4, r6
    5c78:	e3a05000 	mov	r5, #0	; 0x0
    5c7c:	e1550001 	cmp	r5, r1
    5c80:	e593c000 	ldr	ip, [r3]
    5c84:	8a000002 	bhi	5c94 <sha256_data+0x30>
    5c88:	1a000003 	bne	5c9c <sha256_data+0x38>
    5c8c:	e154000c 	cmp	r4, ip
    5c90:	9a000001 	bls	5c9c <sha256_data+0x38>
    5c94:	e3e00000 	mvn	r0, #0	; 0x0
    5c98:	e8bd8070 	pop	{r4, r5, r6, pc}
    5c9c:	e1a0200c 	mov	r2, ip
    5ca0:	e1a03001 	mov	r3, r1
    5ca4:	e59fc01c 	ldr	ip, [pc, #28]	; 5cc8 <sha256_data+0x64>
    5ca8:	e0522004 	subs	r2, r2, r4
    5cac:	e0c33005 	sbc	r3, r3, r5
    5cb0:	e1a01006 	mov	r1, r6
    5cb4:	e88c000c 	stm	ip, {r2, r3}
    5cb8:	eb0000de 	bl	6038 <hash_dma_transfer>
    5cbc:	e3500000 	cmp	r0, #0	; 0x0
    5cc0:	13e00001 	mvnne	r0, #1	; 0x1
    5cc4:	e8bd8070 	pop	{r4, r5, r6, pc}
    5cc8:	0800d1c8 	.word	0x0800d1c8

00005ccc <sha256_init>:
    5ccc:	e1a03ea0 	lsr	r3, r0, #29
    5cd0:	e1a0c180 	lsl	ip, r0, #3
    5cd4:	e92d4010 	push	{r4, lr}
    5cd8:	e59f403c 	ldr	r4, [pc, #60]	; 5d1c <sha256_init+0x50>
    5cdc:	e1a0200c 	mov	r2, ip
    5ce0:	e1a01003 	mov	r1, r3
    5ce4:	e3a00003 	mov	r0, #3	; 0x3
    5ce8:	e5843004 	str	r3, [r4, #4]
    5cec:	e584c000 	str	ip, [r4]
    5cf0:	eb0000b4 	bl	5fc8 <hash_module_init>
    5cf4:	e3500000 	cmp	r0, #0	; 0x0
    5cf8:	05942000 	ldreq	r2, [r4]
    5cfc:	05943004 	ldreq	r3, [r4, #4]
    5d00:	01a021a2 	lsreq	r2, r2, #3
    5d04:	01822e83 	orreq	r2, r2, r3, lsl #29
    5d08:	01a031a3 	lsreq	r3, r3, #3
    5d0c:	13e00000 	mvnne	r0, #0	; 0x0
    5d10:	05843004 	streq	r3, [r4, #4]
    5d14:	05842000 	streq	r2, [r4]
    5d18:	e8bd8010 	pop	{r4, pc}
    5d1c:	0800d1c8 	.word	0x0800d1c8

00005d20 <crc32_init>:
    5d20:	e59f300c 	ldr	r3, [pc, #12]	; 5d34 <crc32_init+0x14>
    5d24:	e3e02000 	mvn	r2, #0	; 0x0
    5d28:	e5832000 	str	r2, [r3]
    5d2c:	e3a00000 	mov	r0, #0	; 0x0
    5d30:	e12fff1e 	bx	lr
    5d34:	0800d1d0 	.word	0x0800d1d0

00005d38 <crc32_data>:
    5d38:	e59f311c 	ldr	r3, [pc, #284]	; 5e5c <crc32_data+0x124>
    5d3c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    5d40:	e593c000 	ldr	ip, [r3]
    5d44:	e1a08000 	mov	r8, r0
    5d48:	e1a06001 	mov	r6, r1
    5d4c:	e1a04000 	mov	r4, r0
    5d50:	e1a07001 	mov	r7, r1
    5d54:	e59f5104 	ldr	r5, [pc, #260]	; 5e60 <crc32_data+0x128>
    5d58:	ea000027 	b	5dfc <crc32_data+0xc4>
    5d5c:	e5543008 	ldrb	r3, [r4, #-8]
    5d60:	e5542007 	ldrb	r2, [r4, #-7]
    5d64:	e02c3003 	eor	r3, ip, r3
    5d68:	e20330ff 	and	r3, r3, #255	; 0xff
    5d6c:	e7953103 	ldr	r3, [r5, r3, lsl #2]
    5d70:	e5541006 	ldrb	r1, [r4, #-6]
    5d74:	e023342c 	eor	r3, r3, ip, lsr #8
    5d78:	e0232002 	eor	r2, r3, r2
    5d7c:	e20220ff 	and	r2, r2, #255	; 0xff
    5d80:	e7952102 	ldr	r2, [r5, r2, lsl #2]
    5d84:	e5540005 	ldrb	r0, [r4, #-5]
    5d88:	e0222423 	eor	r2, r2, r3, lsr #8
    5d8c:	e0221001 	eor	r1, r2, r1
    5d90:	e20110ff 	and	r1, r1, #255	; 0xff
    5d94:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    5d98:	e5541004 	ldrb	r1, [r4, #-4]
    5d9c:	e0233422 	eor	r3, r3, r2, lsr #8
    5da0:	e0230000 	eor	r0, r3, r0
    5da4:	e20000ff 	and	r0, r0, #255	; 0xff
    5da8:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    5dac:	e5540003 	ldrb	r0, [r4, #-3]
    5db0:	e0222423 	eor	r2, r2, r3, lsr #8
    5db4:	e0221001 	eor	r1, r2, r1
    5db8:	e20110ff 	and	r1, r1, #255	; 0xff
    5dbc:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    5dc0:	e554c002 	ldrb	ip, [r4, #-2]
    5dc4:	e0233422 	eor	r3, r3, r2, lsr #8
    5dc8:	e0230000 	eor	r0, r3, r0
    5dcc:	e20000ff 	and	r0, r0, #255	; 0xff
    5dd0:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    5dd4:	e5541001 	ldrb	r1, [r4, #-1]
    5dd8:	e0222423 	eor	r2, r2, r3, lsr #8
    5ddc:	e022c00c 	eor	ip, r2, ip
    5de0:	e20cc0ff 	and	ip, ip, #255	; 0xff
    5de4:	e795310c 	ldr	r3, [r5, ip, lsl #2]
    5de8:	e0233422 	eor	r3, r3, r2, lsr #8
    5dec:	e0231001 	eor	r1, r3, r1
    5df0:	e20110ff 	and	r1, r1, #255	; 0xff
    5df4:	e7952101 	ldr	r2, [r5, r1, lsl #2]
    5df8:	e022c423 	eor	ip, r2, r3, lsr #8
    5dfc:	e3570007 	cmp	r7, #7	; 0x7
    5e00:	e2844008 	add	r4, r4, #8	; 0x8
    5e04:	e2477008 	sub	r7, r7, #8	; 0x8
    5e08:	8affffd3 	bhi	5d5c <crc32_data+0x24>
    5e0c:	e3c63007 	bic	r3, r6, #7	; 0x7
    5e10:	e0880003 	add	r0, r8, r3
    5e14:	e59f3040 	ldr	r3, [pc, #64]	; 5e5c <crc32_data+0x124>
    5e18:	e2161007 	ands	r1, r6, #7	; 0x7
    5e1c:	e583c000 	str	ip, [r3]
    5e20:	0a00000b 	beq	5e54 <crc32_data+0x11c>
    5e24:	e59f4034 	ldr	r4, [pc, #52]	; 5e60 <crc32_data+0x128>
    5e28:	e3a02000 	mov	r2, #0	; 0x0
    5e2c:	e7d03002 	ldrb	r3, [r0, r2]
    5e30:	e2822001 	add	r2, r2, #1	; 0x1
    5e34:	e02c3003 	eor	r3, ip, r3
    5e38:	e20330ff 	and	r3, r3, #255	; 0xff
    5e3c:	e7943103 	ldr	r3, [r4, r3, lsl #2]
    5e40:	e1510002 	cmp	r1, r2
    5e44:	e023c42c 	eor	ip, r3, ip, lsr #8
    5e48:	1afffff7 	bne	5e2c <crc32_data+0xf4>
    5e4c:	e59f3008 	ldr	r3, [pc, #8]	; 5e5c <crc32_data+0x124>
    5e50:	e583c000 	str	ip, [r3]
    5e54:	e3a00000 	mov	r0, #0	; 0x0
    5e58:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5e5c:	0800d1d0 	.word	0x0800d1d0
    5e60:	00010164 	.word	0x00010164

00005e64 <crc32_value>:
    5e64:	e59f3010 	ldr	r3, [pc, #16]	; 5e7c <crc32_value+0x18>
    5e68:	e5933000 	ldr	r3, [r3]
    5e6c:	e1e03003 	mvn	r3, r3
    5e70:	e5803000 	str	r3, [r0]
    5e74:	e3a00000 	mov	r0, #0	; 0x0
    5e78:	e12fff1e 	bx	lr
    5e7c:	0800d1d0 	.word	0x0800d1d0

00005e80 <crc32_le>:
    5e80:	e92d4010 	push	{r4, lr}
    5e84:	e59f401c 	ldr	r4, [pc, #28]	; 5ea8 <crc32_le+0x28>
    5e88:	e3e03000 	mvn	r3, #0	; 0x0
    5e8c:	e1a00001 	mov	r0, r1
    5e90:	e1a01002 	mov	r1, r2
    5e94:	e5843000 	str	r3, [r4]
    5e98:	ebffffa6 	bl	5d38 <crc32_data>
    5e9c:	e5940000 	ldr	r0, [r4]
    5ea0:	e1e00000 	mvn	r0, r0
    5ea4:	e8bd8010 	pop	{r4, pc}
    5ea8:	0800d1d0 	.word	0x0800d1d0

00005eac <hash_module_get_result>:
    5eac:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    5eb0:	e1a08000 	mov	r8, r0
    5eb4:	eb000ac4 	bl	89cc <get_ticks>
    5eb8:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    5ebc:	e59f3088 	ldr	r3, [pc, #136]	; 5f4c <hash_module_get_result+0xa0>
    5ec0:	e3a05000 	mov	r5, #0	; 0x0
    5ec4:	e0944000 	adds	r4, r4, r0
    5ec8:	e0a55001 	adc	r5, r5, r1
    5ecc:	e1a07003 	mov	r7, r3
    5ed0:	e593680c 	ldr	r6, [r3, #2060]
    5ed4:	ea000008 	b	5efc <hash_module_get_result+0x50>
    5ed8:	e597680c 	ldr	r6, [r7, #2060]
    5edc:	eb000aba 	bl	89cc <get_ticks>
    5ee0:	e1510005 	cmp	r1, r5
    5ee4:	8a000002 	bhi	5ef4 <hash_module_get_result+0x48>
    5ee8:	1a000003 	bne	5efc <hash_module_get_result+0x50>
    5eec:	e1500004 	cmp	r0, r4
    5ef0:	9a000001 	bls	5efc <hash_module_get_result+0x50>
    5ef4:	e3a00001 	mov	r0, #1	; 0x1
    5ef8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5efc:	e3160001 	tst	r6, #1	; 0x1
    5f00:	0afffff4 	beq	5ed8 <hash_module_get_result+0x2c>
    5f04:	e59f0044 	ldr	r0, [pc, #68]	; 5f50 <hash_module_get_result+0xa4>
    5f08:	e59fc044 	ldr	ip, [pc, #68]	; 5f54 <hash_module_get_result+0xa8>
    5f0c:	e1a03000 	mov	r3, r0
    5f10:	e6931008 	ldr	r1, [r3], r8
    5f14:	e59f203c 	ldr	r2, [pc, #60]	; 5f58 <hash_module_get_result+0xac>
    5f18:	e2800004 	add	r0, r0, #4	; 0x4
    5f1c:	e0832002 	add	r2, r3, r2
    5f20:	e150000c 	cmp	r0, ip
    5f24:	e5821000 	str	r1, [r2]
    5f28:	1afffff7 	bne	5f0c <hash_module_get_result+0x60>
    5f2c:	e59f2018 	ldr	r2, [pc, #24]	; 5f4c <hash_module_get_result+0xa0>
    5f30:	e3a03001 	mov	r3, #1	; 0x1
    5f34:	e582380c 	str	r3, [r2, #2060]
    5f38:	e5923800 	ldr	r3, [r2, #2048]
    5f3c:	e3a00000 	mov	r0, #0	; 0x0
    5f40:	e3c33080 	bic	r3, r3, #128	; 0x80
    5f44:	e5823800 	str	r3, [r2, #2048]
    5f48:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5f4c:	27700000 	.word	0x27700000
    5f50:	27700858 	.word	0x27700858
    5f54:	27700878 	.word	0x27700878
    5f58:	d88ff7a8 	.word	0xd88ff7a8

00005f5c <hash_module_clear>:
    5f5c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5f60:	e59f405c 	ldr	r4, [pc, #92]	; 5fc4 <hash_module_clear+0x68>
    5f64:	e3a03002 	mov	r3, #2	; 0x2
    5f68:	e5843800 	str	r3, [r4, #2048]
    5f6c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5f70:	eb000a95 	bl	89cc <get_ticks>
    5f74:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    5f78:	e3a07000 	mov	r7, #0	; 0x0
    5f7c:	e0966000 	adds	r6, r6, r0
    5f80:	e1a05004 	mov	r5, r4
    5f84:	e0a77001 	adc	r7, r7, r1
    5f88:	e5944800 	ldr	r4, [r4, #2048]
    5f8c:	ea000008 	b	5fb4 <hash_module_clear+0x58>
    5f90:	e5954800 	ldr	r4, [r5, #2048]
    5f94:	eb000a8c 	bl	89cc <get_ticks>
    5f98:	e1510007 	cmp	r1, r7
    5f9c:	8a000002 	bhi	5fac <hash_module_clear+0x50>
    5fa0:	1a000003 	bne	5fb4 <hash_module_clear+0x58>
    5fa4:	e1500006 	cmp	r0, r6
    5fa8:	9a000001 	bls	5fb4 <hash_module_clear+0x58>
    5fac:	e3a00001 	mov	r0, #1	; 0x1
    5fb0:	ea000001 	b	5fbc <hash_module_clear+0x60>
    5fb4:	e2140002 	ands	r0, r4, #2	; 0x2
    5fb8:	1afffff4 	bne	5f90 <hash_module_clear+0x34>
    5fbc:	e28dd004 	add	sp, sp, #4	; 0x4
    5fc0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5fc4:	27700000 	.word	0x27700000

00005fc8 <hash_module_init>:
    5fc8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5fcc:	e59f4060 	ldr	r4, [pc, #96]	; 6034 <hash_module_init+0x6c>
    5fd0:	e24dd004 	sub	sp, sp, #4	; 0x4
    5fd4:	e5943800 	ldr	r3, [r4, #2048]
    5fd8:	e1a05000 	mov	r5, r0
    5fdc:	e3c33080 	bic	r3, r3, #128	; 0x80
    5fe0:	e5843800 	str	r3, [r4, #2048]
    5fe4:	e1a07001 	mov	r7, r1
    5fe8:	e1a06002 	mov	r6, r2
    5fec:	ebffffda 	bl	5f5c <hash_module_clear>
    5ff0:	e3500000 	cmp	r0, #0	; 0x0
    5ff4:	13a00001 	movne	r0, #1	; 0x1
    5ff8:	1a00000b 	bne	602c <hash_module_init+0x64>
    5ffc:	e3a03001 	mov	r3, #1	; 0x1
    6000:	e1a02185 	lsl	r2, r5, #3
    6004:	e5843804 	str	r3, [r4, #2052]
    6008:	e5842800 	str	r2, [r4, #2048]
    600c:	e5847814 	str	r7, [r4, #2068]
    6010:	e5846810 	str	r6, [r4, #2064]
    6014:	e5943800 	ldr	r3, [r4, #2048]
    6018:	e3833080 	orr	r3, r3, #128	; 0x80
    601c:	e5843800 	str	r3, [r4, #2048]
    6020:	e5943800 	ldr	r3, [r4, #2048]
    6024:	e3833004 	orr	r3, r3, #4	; 0x4
    6028:	e5843800 	str	r3, [r4, #2048]
    602c:	e28dd004 	add	sp, sp, #4	; 0x4
    6030:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6034:	27700000 	.word	0x27700000

00006038 <hash_dma_transfer>:
    6038:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    603c:	e1a08001 	mov	r8, r1
    6040:	e24dd004 	sub	sp, sp, #4	; 0x4
    6044:	e1a09000 	mov	r9, r0
    6048:	e1a06001 	mov	r6, r1
    604c:	e59fa090 	ldr	sl, [pc, #144]	; 60e4 <hash_dma_transfer+0xac>
    6050:	e3a0b000 	mov	fp, #0	; 0x0
    6054:	ea00001b 	b	60c8 <hash_dma_transfer+0x90>
    6058:	e3560902 	cmp	r6, #32768	; 0x8000
    605c:	b1a07006 	movlt	r7, r6
    6060:	a3a07902 	movge	r7, #32768	; 0x8000
    6064:	e0661008 	rsb	r1, r6, r8
    6068:	e0891001 	add	r1, r9, r1
    606c:	e3a00002 	mov	r0, #2	; 0x2
    6070:	e1a02007 	mov	r2, r7
    6074:	ebfffe60 	bl	59fc <dma_sdma_init>
    6078:	e3500000 	cmp	r0, #0	; 0x0
    607c:	1a000015 	bne	60d8 <hash_dma_transfer+0xa0>
    6080:	e2800002 	add	r0, r0, #2	; 0x2
    6084:	ebfffe4c 	bl	59bc <dma_start>
    6088:	eb000a4f 	bl	89cc <get_ticks>
    608c:	e090400a 	adds	r4, r0, sl
    6090:	e0a1500b 	adc	r5, r1, fp
    6094:	ea000005 	b	60b0 <hash_dma_transfer+0x78>
    6098:	eb000a4b 	bl	89cc <get_ticks>
    609c:	e1510005 	cmp	r1, r5
    60a0:	8a00000c 	bhi	60d8 <hash_dma_transfer+0xa0>
    60a4:	1a000001 	bne	60b0 <hash_dma_transfer+0x78>
    60a8:	e1500004 	cmp	r0, r4
    60ac:	8a000009 	bhi	60d8 <hash_dma_transfer+0xa0>
    60b0:	e3a01003 	mov	r1, #3	; 0x3
    60b4:	e3a00002 	mov	r0, #2	; 0x2
    60b8:	ebfffe45 	bl	59d4 <dma_get_status>
    60bc:	e3500000 	cmp	r0, #0	; 0x0
    60c0:	0afffff4 	beq	6098 <hash_dma_transfer+0x60>
    60c4:	e0676006 	rsb	r6, r7, r6
    60c8:	e3560000 	cmp	r6, #0	; 0x0
    60cc:	1affffe1 	bne	6058 <hash_dma_transfer+0x20>
    60d0:	e1a00006 	mov	r0, r6
    60d4:	ea000000 	b	60dc <hash_dma_transfer+0xa4>
    60d8:	e3a00001 	mov	r0, #1	; 0x1
    60dc:	e28dd004 	add	sp, sp, #4	; 0x4
    60e0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    60e4:	000f4240 	.word	0x000f4240

000060e8 <cdbg_log_toggle>:
    60e8:	e59f3014 	ldr	r3, [pc, #20]	; 6104 <cdbg_log_toggle+0x1c>
    60ec:	e3500000 	cmp	r0, #0	; 0x0
    60f0:	e59f2010 	ldr	r2, [pc, #16]	; 6108 <cdbg_log_toggle+0x20>
    60f4:	01a02003 	moveq	r2, r3
    60f8:	e59f300c 	ldr	r3, [pc, #12]	; 610c <cdbg_log_toggle+0x24>
    60fc:	e5832000 	str	r2, [r3]
    6100:	e12fff1e 	bx	lr
    6104:	0000d02c 	.word	0x0000d02c
    6108:	0000d0b0 	.word	0x0000d0b0
    610c:	0800d704 	.word	0x0800d704

00006110 <cdbg_wipe_bootarea>:
    6110:	e59f3018 	ldr	r3, [pc, #24]	; 6130 <cdbg_wipe_bootarea+0x20>
    6114:	e59f1018 	ldr	r1, [pc, #24]	; 6134 <cdbg_wipe_bootarea+0x24>
    6118:	e59f2018 	ldr	r2, [pc, #24]	; 6138 <cdbg_wipe_bootarea+0x28>
    611c:	e5031004 	str	r1, [r3, #-4]
    6120:	e2833004 	add	r3, r3, #4	; 0x4
    6124:	e1530002 	cmp	r3, r2
    6128:	1afffffb 	bne	611c <cdbg_wipe_bootarea+0xc>
    612c:	e12fff1e 	bx	lr
    6130:	08000004 	.word	0x08000004
    6134:	eafffffe 	.word	0xeafffffe
    6138:	08008004 	.word	0x08008004

0000613c <cdbg_verify_burn>:
    613c:	e59f3004 	ldr	r3, [pc, #4]	; 6148 <cdbg_verify_burn+0xc>
    6140:	e5930000 	ldr	r0, [r3]
    6144:	e12fff1e 	bx	lr
    6148:	0800d1d4 	.word	0x0800d1d4

0000614c <cdbg_verify_burn_key>:
    614c:	e59f0000 	ldr	r0, [pc, #0]	; 6154 <cdbg_verify_burn_key+0x8>
    6150:	e12fff1e 	bx	lr
    6154:	0800d1d8 	.word	0x0800d1d8

00006158 <cdbg_verify_burn_enable>:
    6158:	e3500000 	cmp	r0, #0	; 0x0
    615c:	e59f3018 	ldr	r3, [pc, #24]	; 617c <cdbg_verify_burn_enable+0x24>
    6160:	03a02000 	moveq	r2, #0	; 0x0
    6164:	13a02001 	movne	r2, #1	; 0x1
    6168:	e5832000 	str	r2, [r3]
    616c:	012fff1e 	bxeq	lr
    6170:	e59f0008 	ldr	r0, [pc, #8]	; 6180 <cdbg_verify_burn_enable+0x28>
    6174:	e3a02020 	mov	r2, #32	; 0x20
    6178:	eaffec96 	b	13d8 <memcpy>
    617c:	0800d1d4 	.word	0x0800d1d4
    6180:	0800d1d8 	.word	0x0800d1d8

00006184 <cdbg_jump>:
    6184:	e92d4030 	push	{r4, r5, lr}
    6188:	e1a04000 	mov	r4, r0
    618c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6190:	e1a05001 	mov	r5, r1
    6194:	ebfffc61 	bl	5320 <ss_lock_isk>
    6198:	e59f001c 	ldr	r0, [pc, #28]	; 61bc <cdbg_jump+0x38>
    619c:	e1a01004 	mov	r1, r4
    61a0:	e59f3018 	ldr	r3, [pc, #24]	; 61c0 <cdbg_jump+0x3c>
    61a4:	e1a0e00f 	mov	lr, pc
    61a8:	e593f000 	ldr	pc, [r3]
    61ac:	e3000bb8 	movw	r0, #3000	; 0xbb8
    61b0:	eb000a18 	bl	8a18 <udelay>
    61b4:	e1a00005 	mov	r0, r5
    61b8:	e12fff34 	blx	r4
    61bc:	00011f09 	.word	0x00011f09
    61c0:	0800d704 	.word	0x0800d704

000061c4 <cdbg_restore_os>:
    61c4:	e92d4010 	push	{r4, lr}
    61c8:	ebffe990 	bl	810 <get_wakeup_entry>
    61cc:	e59f301c 	ldr	r3, [pc, #28]	; 61f0 <cdbg_restore_os+0x2c>
    61d0:	e1a04000 	mov	r4, r0
    61d4:	e59f0018 	ldr	r0, [pc, #24]	; 61f4 <cdbg_restore_os+0x30>
    61d8:	e1a0e00f 	mov	lr, pc
    61dc:	e593f000 	ldr	pc, [r3]
    61e0:	e1a00004 	mov	r0, r4
    61e4:	e3a01000 	mov	r1, #0	; 0x0
    61e8:	e8bd4010 	pop	{r4, lr}
    61ec:	eaffffe4 	b	6184 <cdbg_jump>
    61f0:	0800d704 	.word	0x0800d704
    61f4:	00011f19 	.word	0x00011f19

000061f8 <cdbg_get_pp>:
    61f8:	e92d4070 	push	{r4, r5, r6, lr}
    61fc:	e24dd008 	sub	sp, sp, #8	; 0x8
    6200:	e1a0000d 	mov	r0, sp
    6204:	e59f103c 	ldr	r1, [pc, #60]	; 6248 <cdbg_get_pp+0x50>
    6208:	e3a02008 	mov	r2, #8	; 0x8
    620c:	ebffec71 	bl	13d8 <memcpy>
    6210:	e1a0400d 	mov	r4, sp
    6214:	e1a0600d 	mov	r6, sp
    6218:	ea000004 	b	6230 <cdbg_get_pp+0x38>
    621c:	eb001bc1 	bl	d128 <getc>
    6220:	e1500004 	cmp	r0, r4
    6224:	e2854001 	add	r4, r5, #1	; 0x1
    6228:	e1a05006 	mov	r5, r6
    622c:	1a000000 	bne	6234 <cdbg_get_pp+0x3c>
    6230:	e1a05004 	mov	r5, r4
    6234:	e5d54000 	ldrb	r4, [r5]
    6238:	e3540000 	cmp	r4, #0	; 0x0
    623c:	1afffff6 	bne	621c <cdbg_get_pp+0x24>
    6240:	e28dd008 	add	sp, sp, #8	; 0x8
    6244:	e8bd8070 	pop	{r4, r5, r6, pc}
    6248:	000107ee 	.word	0x000107ee

0000624c <cdbg_shell>:
    624c:	e92d4010 	push	{r4, lr}
    6250:	ebfffc32 	bl	5320 <ss_lock_isk>
    6254:	e59f3048 	ldr	r3, [pc, #72]	; 62a4 <cdbg_shell+0x58>
    6258:	e59f1048 	ldr	r1, [pc, #72]	; 62a8 <cdbg_shell+0x5c>
    625c:	e59f2048 	ldr	r2, [pc, #72]	; 62ac <cdbg_shell+0x60>
    6260:	e5031004 	str	r1, [r3, #-4]
    6264:	e2833004 	add	r3, r3, #4	; 0x4
    6268:	e1530002 	cmp	r3, r2
    626c:	1afffffb 	bne	6260 <cdbg_shell+0x14>
    6270:	e3a00001 	mov	r0, #1	; 0x1
    6274:	eb002385 	bl	f090 <ss_jtag_en>
    6278:	e59f4030 	ldr	r4, [pc, #48]	; 62b0 <cdbg_shell+0x64>
    627c:	e59f3030 	ldr	r3, [pc, #48]	; 62b4 <cdbg_shell+0x68>
    6280:	e59f0030 	ldr	r0, [pc, #48]	; 62b8 <cdbg_shell+0x6c>
    6284:	e5843000 	str	r3, [r4]
    6288:	eb001b88 	bl	d0b0 <printf_on>
    628c:	ebffffd9 	bl	61f8 <cdbg_get_pp>
    6290:	e59f0024 	ldr	r0, [pc, #36]	; 62bc <cdbg_shell+0x70>
    6294:	e1a0e00f 	mov	lr, pc
    6298:	e594f000 	ldr	pc, [r4]
    629c:	eb001e8c 	bl	dcd4 <main_loop>
    62a0:	eafffffd 	b	629c <cdbg_shell+0x50>
    62a4:	08000004 	.word	0x08000004
    62a8:	eafffffe 	.word	0xeafffffe
    62ac:	08008004 	.word	0x08008004
    62b0:	0800d704 	.word	0x0800d704
    62b4:	0000d0b0 	.word	0x0000d0b0
    62b8:	00011f28 	.word	0x00011f28
    62bc:	00011f2d 	.word	0x00011f2d

000062c0 <cdbg_do_config>:
    62c0:	e92d4030 	push	{r4, r5, lr}
    62c4:	e59f307c 	ldr	r3, [pc, #124]	; 6348 <cdbg_do_config+0x88>
    62c8:	e24dd004 	sub	sp, sp, #4	; 0x4
    62cc:	e1a04000 	mov	r4, r0
    62d0:	e1a05001 	mov	r5, r1
    62d4:	e59f0070 	ldr	r0, [pc, #112]	; 634c <cdbg_do_config+0x8c>
    62d8:	e59f1070 	ldr	r1, [pc, #112]	; 6350 <cdbg_do_config+0x90>
    62dc:	e1a0e00f 	mov	lr, pc
    62e0:	e593f000 	ldr	pc, [r3]
    62e4:	e3a02000 	mov	r2, #0	; 0x0
    62e8:	e59f0060 	ldr	r0, [pc, #96]	; 6350 <cdbg_do_config+0x90>
    62ec:	ea000006 	b	630c <cdbg_do_config+0x4c>
    62f0:	e7943002 	ldr	r3, [r4, r2]
    62f4:	e2822004 	add	r2, r2, #4	; 0x4
    62f8:	e1530000 	cmp	r3, r0
    62fc:	1a000002 	bne	630c <cdbg_do_config+0x4c>
    6300:	e3510000 	cmp	r1, #0	; 0x0
    6304:	1a000004 	bne	631c <cdbg_do_config+0x5c>
    6308:	ea000007 	b	632c <cdbg_do_config+0x6c>
    630c:	e1520005 	cmp	r2, r5
    6310:	e0841002 	add	r1, r4, r2
    6314:	bafffff5 	blt	62f0 <cdbg_do_config+0x30>
    6318:	ea000003 	b	632c <cdbg_do_config+0x6c>
    631c:	e5910004 	ldr	r0, [r1, #4]
    6320:	eb000be8 	bl	92c8 <spi_set_bytes>
    6324:	e3a00000 	mov	r0, #0	; 0x0
    6328:	ea000004 	b	6340 <cdbg_do_config+0x80>
    632c:	e59f0020 	ldr	r0, [pc, #32]	; 6354 <cdbg_do_config+0x94>
    6330:	e59f3010 	ldr	r3, [pc, #16]	; 6348 <cdbg_do_config+0x88>
    6334:	e1a0e00f 	mov	lr, pc
    6338:	e593f000 	ldr	pc, [r3]
    633c:	e3e00000 	mvn	r0, #0	; 0x0
    6340:	e28dd004 	add	sp, sp, #4	; 0x4
    6344:	e8bd8030 	pop	{r4, r5, pc}
    6348:	0800d704 	.word	0x0800d704
    634c:	00011f3e 	.word	0x00011f3e
    6350:	4a82467c 	.word	0x4a82467c
    6354:	00011f5b 	.word	0x00011f5b

00006358 <cdbg_config_isi>:
    6358:	e92d4030 	push	{r4, r5, lr}
    635c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6360:	e1a04000 	mov	r4, r0
    6364:	ebffedde 	bl	1ae4 <isi_check>
    6368:	e2505000 	subs	r5, r0, #0	; 0x0
    636c:	0a000007 	beq	6390 <cdbg_config_isi+0x38>
    6370:	e59f002c 	ldr	r0, [pc, #44]	; 63a4 <cdbg_config_isi+0x4c>
    6374:	e1a01005 	mov	r1, r5
    6378:	e59f3028 	ldr	r3, [pc, #40]	; 63a8 <cdbg_config_isi+0x50>
    637c:	e1a0e00f 	mov	lr, pc
    6380:	e593f000 	ldr	pc, [r3]
    6384:	e1a00005 	mov	r0, r5
    6388:	e28dd004 	add	sp, sp, #4	; 0x4
    638c:	e8bd8030 	pop	{r4, r5, pc}
    6390:	e594100c 	ldr	r1, [r4, #12]
    6394:	e2840c02 	add	r0, r4, #512	; 0x200
    6398:	e28dd004 	add	sp, sp, #4	; 0x4
    639c:	e8bd4030 	pop	{r4, r5, lr}
    63a0:	eaffffc6 	b	62c0 <cdbg_do_config>
    63a4:	00011f7a 	.word	0x00011f7a
    63a8:	0800d704 	.word	0x0800d704

000063ac <burn_image_loop>:
    63ac:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    63b0:	e24dd024 	sub	sp, sp, #36	; 0x24
    63b4:	e59d704c 	ldr	r7, [sp, #76]
    63b8:	e1a06000 	mov	r6, r0
    63bc:	e3570000 	cmp	r7, #0	; 0x0
    63c0:	e58d1014 	str	r1, [sp, #20]
    63c4:	e58d2010 	str	r2, [sp, #16]
    63c8:	e58d300c 	str	r3, [sp, #12]
    63cc:	1a00000c 	bne	6404 <burn_image_loop+0x58>
    63d0:	e59f426c 	ldr	r4, [pc, #620]	; 6644 <burn_image_loop+0x298>
    63d4:	e59f026c 	ldr	r0, [pc, #620]	; 6648 <burn_image_loop+0x29c>
    63d8:	e3a0102e 	mov	r1, #46	; 0x2e
    63dc:	e1a0e00f 	mov	lr, pc
    63e0:	e594f000 	ldr	pc, [r4]
    63e4:	e59f0260 	ldr	r0, [pc, #608]	; 664c <burn_image_loop+0x2a0>
    63e8:	e59f1260 	ldr	r1, [pc, #608]	; 6650 <burn_image_loop+0x2a4>
    63ec:	e1a0e00f 	mov	lr, pc
    63f0:	e594f000 	ldr	pc, [r4]
    63f4:	e59f0258 	ldr	r0, [pc, #600]	; 6654 <burn_image_loop+0x2a8>
    63f8:	e1a0e00f 	mov	lr, pc
    63fc:	e594f000 	ldr	pc, [r4]
    6400:	eafffffe 	b	6400 <burn_image_loop+0x54>
    6404:	e59d0014 	ldr	r0, [sp, #20]
    6408:	e59d100c 	ldr	r1, [sp, #12]
    640c:	e1500001 	cmp	r0, r1
    6410:	1a000005 	bne	642c <burn_image_loop+0x80>
    6414:	e59f023c 	ldr	r0, [pc, #572]	; 6658 <burn_image_loop+0x2ac>
    6418:	e59f3224 	ldr	r3, [pc, #548]	; 6644 <burn_image_loop+0x298>
    641c:	e1a0e00f 	mov	lr, pc
    6420:	e593f000 	ldr	pc, [r3]
    6424:	e3e00001 	mvn	r0, #1	; 0x1
    6428:	ea000083 	b	663c <burn_image_loop+0x290>
    642c:	e596c000 	ldr	ip, [r6]
    6430:	e3a03000 	mov	r3, #0	; 0x0
    6434:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    6438:	e002200c 	and	r2, r2, ip
    643c:	e1a01483 	lsl	r1, r3, #9
    6440:	e1d600b8 	ldrh	r0, [r6, #8]
    6444:	e1811ba2 	orr	r1, r1, r2, lsr #23
    6448:	e1a0e482 	lsl	lr, r2, #9
    644c:	e1a03001 	mov	r3, r1
    6450:	e3a02401 	mov	r2, #16777216	; 0x1000000
    6454:	e5961004 	ldr	r1, [r6, #4]
    6458:	e1a0cc2c 	lsr	ip, ip, #24
    645c:	e1a05000 	mov	r5, r0
    6460:	e09e8002 	adds	r8, lr, r2
    6464:	e1a09003 	mov	r9, r3
    6468:	e3a04000 	mov	r4, #0	; 0x0
    646c:	e3a03000 	mov	r3, #0	; 0x0
    6470:	e20cc03f 	and	ip, ip, #63	; 0x3f
    6474:	e59d000c 	ldr	r0, [sp, #12]
    6478:	e0a99003 	adc	r9, r9, r3
    647c:	e184a001 	orr	sl, r4, r1
    6480:	e1a0b005 	mov	fp, r5
    6484:	e58dc018 	str	ip, [sp, #24]
    6488:	e596500c 	ldr	r5, [r6, #12]
    648c:	ebfffaa2 	bl	4f1c <vs_device_erasable>
    6490:	e1500004 	cmp	r0, r4
    6494:	0a000006 	beq	64b4 <burn_image_loop+0x108>
    6498:	e1a01004 	mov	r1, r4
    649c:	e59d000c 	ldr	r0, [sp, #12]
    64a0:	ebfffa29 	bl	4d4c <vs_assign_by_id>
    64a4:	e1a0000a 	mov	r0, sl
    64a8:	e1a0100b 	mov	r1, fp
    64ac:	e59d2048 	ldr	r2, [sp, #72]
    64b0:	ebfff9d2 	bl	4c00 <vs_erase>
    64b4:	e59d0014 	ldr	r0, [sp, #20]
    64b8:	e3a01001 	mov	r1, #1	; 0x1
    64bc:	ebfffa22 	bl	4d4c <vs_assign_by_id>
    64c0:	e59d2018 	ldr	r2, [sp, #24]
    64c4:	e3520002 	cmp	r2, #2	; 0x2
    64c8:	1a000019 	bne	6534 <burn_image_loop+0x188>
    64cc:	e3a01c02 	mov	r1, #512	; 0x200
    64d0:	e58d1000 	str	r1, [sp]
    64d4:	e59d1010 	ldr	r1, [sp, #16]
    64d8:	e1a02008 	mov	r2, r8
    64dc:	e1a03009 	mov	r3, r9
    64e0:	e1a00007 	mov	r0, r7
    64e4:	e58d1004 	str	r1, [sp, #4]
    64e8:	ebfffaf4 	bl	50c0 <vs_read>
    64ec:	e1a00007 	mov	r0, r7
    64f0:	ebffed20 	bl	1978 <isi_check_header>
    64f4:	e2504000 	subs	r4, r0, #0	; 0x0
    64f8:	1a00000d 	bne	6534 <burn_image_loop+0x188>
    64fc:	e5973004 	ldr	r3, [r7, #4]
    6500:	e1b03e23 	lsrs	r3, r3, #28
    6504:	0a00000a 	beq	6534 <burn_image_loop+0x188>
    6508:	e59f014c 	ldr	r0, [pc, #332]	; 665c <burn_image_loop+0x2b0>
    650c:	e59f3130 	ldr	r3, [pc, #304]	; 6644 <burn_image_loop+0x298>
    6510:	e1a0e00f 	mov	lr, pc
    6514:	e593f000 	ldr	pc, [r3]
    6518:	e59f2140 	ldr	r2, [pc, #320]	; 6660 <burn_image_loop+0x2b4>
    651c:	e1a01004 	mov	r1, r4
    6520:	e3a00001 	mov	r0, #1	; 0x1
    6524:	ebfffb98 	bl	538c <ss_aes_init>
    6528:	e3a02001 	mov	r2, #1	; 0x1
    652c:	e58d201c 	str	r2, [sp, #28]
    6530:	ea000001 	b	653c <burn_image_loop+0x190>
    6534:	e3a03000 	mov	r3, #0	; 0x0
    6538:	e58d301c 	str	r3, [sp, #28]
    653c:	e59f4100 	ldr	r4, [pc, #256]	; 6644 <burn_image_loop+0x298>
    6540:	e59d1048 	ldr	r1, [sp, #72]
    6544:	e59f0118 	ldr	r0, [pc, #280]	; 6664 <burn_image_loop+0x2b8>
    6548:	e3a02901 	mov	r2, #16384	; 0x4000
    654c:	e1a0e00f 	mov	lr, pc
    6550:	e594f000 	ldr	pc, [r4]
    6554:	e3a06901 	mov	r6, #16384	; 0x4000
    6558:	e1a04005 	mov	r4, r5
    655c:	ea000032 	b	662c <burn_image_loop+0x280>
    6560:	e1b03a23 	lsrs	r3, r3, #20
    6564:	0a000005 	beq	6580 <burn_image_loop+0x1d4>
    6568:	e1a01a25 	lsr	r1, r5, #20
    656c:	e59f00f4 	ldr	r0, [pc, #244]	; 6668 <burn_image_loop+0x2bc>
    6570:	e59f20cc 	ldr	r2, [pc, #204]	; 6644 <burn_image_loop+0x298>
    6574:	e1a0e00f 	mov	lr, pc
    6578:	e592f000 	ldr	pc, [r2]
    657c:	e1a04005 	mov	r4, r5
    6580:	e3a01000 	mov	r1, #0	; 0x0
    6584:	e59d0014 	ldr	r0, [sp, #20]
    6588:	ebfff9ef 	bl	4d4c <vs_assign_by_id>
    658c:	e59d1010 	ldr	r1, [sp, #16]
    6590:	e1a02008 	mov	r2, r8
    6594:	e1a03009 	mov	r3, r9
    6598:	e1a00007 	mov	r0, r7
    659c:	e58d6000 	str	r6, [sp]
    65a0:	e58d1004 	str	r1, [sp, #4]
    65a4:	ebfffac5 	bl	50c0 <vs_read>
    65a8:	e59d201c 	ldr	r2, [sp, #28]
    65ac:	e3520000 	cmp	r2, #0	; 0x0
    65b0:	0a000003 	beq	65c4 <burn_image_loop+0x218>
    65b4:	e1a01007 	mov	r1, r7
    65b8:	e1a02006 	mov	r2, r6
    65bc:	e1a00007 	mov	r0, r7
    65c0:	ebfffb60 	bl	5348 <ss_aes_data>
    65c4:	e3a01000 	mov	r1, #0	; 0x0
    65c8:	e59d000c 	ldr	r0, [sp, #12]
    65cc:	ebfff9de 	bl	4d4c <vs_assign_by_id>
    65d0:	e2501000 	subs	r1, r0, #0	; 0x0
    65d4:	1a000005 	bne	65f0 <burn_image_loop+0x244>
    65d8:	e1a00007 	mov	r0, r7
    65dc:	e1a0200a 	mov	r2, sl
    65e0:	e1a0300b 	mov	r3, fp
    65e4:	e58d1004 	str	r1, [sp, #4]
    65e8:	e58d6000 	str	r6, [sp]
    65ec:	ebfff96d 	bl	4ba8 <vs_write>
    65f0:	e59d0018 	ldr	r0, [sp, #24]
    65f4:	e3a01000 	mov	r1, #0	; 0x0
    65f8:	e3500001 	cmp	r0, #1	; 0x1
    65fc:	e3a00901 	mov	r0, #16384	; 0x4000
    6600:	93a03901 	movls	r3, #16384	; 0x4000
    6604:	83a03000 	movhi	r3, #0	; 0x0
    6608:	e3550901 	cmp	r5, #16384	; 0x4000
    660c:	31a02005 	movcc	r2, r5
    6610:	23a02901 	movcs	r2, #16384	; 0x4000
    6614:	e09aa000 	adds	sl, sl, r0
    6618:	e0abb001 	adc	fp, fp, r1
    661c:	e0988000 	adds	r8, r8, r0
    6620:	e0877003 	add	r7, r7, r3
    6624:	e0a99001 	adc	r9, r9, r1
    6628:	e0625005 	rsb	r5, r2, r5
    662c:	e3550000 	cmp	r5, #0	; 0x0
    6630:	e0653004 	rsb	r3, r5, r4
    6634:	1affffc9 	bne	6560 <burn_image_loop+0x1b4>
    6638:	e1a00005 	mov	r0, r5
    663c:	e28dd024 	add	sp, sp, #36	; 0x24
    6640:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6644:	0800d704 	.word	0x0800d704
    6648:	00011e4e 	.word	0x00011e4e
    664c:	00010c7b 	.word	0x00010c7b
    6650:	00011f95 	.word	0x00011f95
    6654:	00011f9c 	.word	0x00011f9c
    6658:	00011fb2 	.word	0x00011fb2
    665c:	00011fdf 	.word	0x00011fdf
    6660:	0800d0bc 	.word	0x0800d0bc
    6664:	00011ff8 	.word	0x00011ff8
    6668:	00012007 	.word	0x00012007

0000666c <burn_images>:
    666c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6670:	e590901c 	ldr	r9, [r0, #28]
    6674:	e24dd014 	sub	sp, sp, #20	; 0x14
    6678:	e59f41ac 	ldr	r4, [pc, #428]	; 682c <burn_images+0x1c0>
    667c:	e1a05000 	mov	r5, r0
    6680:	e58d100c 	str	r1, [sp, #12]
    6684:	e59f01a4 	ldr	r0, [pc, #420]	; 6830 <burn_images+0x1c4>
    6688:	e1a01009 	mov	r1, r9
    668c:	e1a0b002 	mov	fp, r2
    6690:	e2857020 	add	r7, r5, #32	; 0x20
    6694:	e1a0e00f 	mov	lr, pc
    6698:	e594f000 	ldr	pc, [r4]
    669c:	e1a0a004 	mov	sl, r4
    66a0:	e3a08000 	mov	r8, #0	; 0x0
    66a4:	ea000055 	b	6800 <burn_images+0x194>
    66a8:	e59f0184 	ldr	r0, [pc, #388]	; 6834 <burn_images+0x1c8>
    66ac:	e1a01008 	mov	r1, r8
    66b0:	e1a0e00f 	mov	lr, pc
    66b4:	e59af000 	ldr	pc, [sl]
    66b8:	e5d73003 	ldrb	r3, [r7, #3]
    66bc:	e5976008 	ldr	r6, [r7, #8]
    66c0:	e203503f 	and	r5, r3, #63	; 0x3f
    66c4:	e3550002 	cmp	r5, #2	; 0x2
    66c8:	0a00000a 	beq	66f8 <burn_images+0x8c>
    66cc:	ca000004 	bgt	66e4 <burn_images+0x78>
    66d0:	e3550000 	cmp	r5, #0	; 0x0
    66d4:	0a000051 	beq	6820 <burn_images+0x1b4>
    66d8:	e3550001 	cmp	r5, #1	; 0x1
    66dc:	1a00001a 	bne	674c <burn_images+0xe0>
    66e0:	ea000008 	b	6708 <burn_images+0x9c>
    66e4:	e3550030 	cmp	r5, #48	; 0x30
    66e8:	0a00000c 	beq	6720 <burn_images+0xb4>
    66ec:	e3550031 	cmp	r5, #49	; 0x31
    66f0:	1a000015 	bne	674c <burn_images+0xe0>
    66f4:	ea00000e 	b	6734 <burn_images+0xc8>
    66f8:	e1a04826 	lsr	r4, r6, #16
    66fc:	e354000f 	cmp	r4, #15	; 0xf
    6700:	1a000015 	bne	675c <burn_images+0xf0>
    6704:	ea00001f 	b	6788 <burn_images+0x11c>
    6708:	e59f0128 	ldr	r0, [pc, #296]	; 6838 <burn_images+0x1cc>
    670c:	e1a01006 	mov	r1, r6
    6710:	e1a0e00f 	mov	lr, pc
    6714:	e59af000 	ldr	pc, [sl]
    6718:	e1a0e006 	mov	lr, r6
    671c:	ea000040 	b	6824 <burn_images+0x1b8>
    6720:	e59f0114 	ldr	r0, [pc, #276]	; 683c <burn_images+0x1d0>
    6724:	e1a0e00f 	mov	lr, pc
    6728:	e59af000 	ldr	pc, [sl]
    672c:	ebffe841 	bl	838 <system_reset>
    6730:	ea00003a 	b	6820 <burn_images+0x1b4>
    6734:	e59f0104 	ldr	r0, [pc, #260]	; 6840 <burn_images+0x1d4>
    6738:	e1a0e00f 	mov	lr, pc
    673c:	e59af000 	ldr	pc, [sl]
    6740:	e3a00001 	mov	r0, #1	; 0x1
    6744:	ebffe72f 	bl	408 <try_boot>
    6748:	ea000034 	b	6820 <burn_images+0x1b4>
    674c:	e59f00f0 	ldr	r0, [pc, #240]	; 6844 <burn_images+0x1d8>
    6750:	e1a0e00f 	mov	lr, pc
    6754:	e59af000 	ldr	pc, [sl]
    6758:	ea000026 	b	67f8 <burn_images+0x18c>
    675c:	e1a00004 	mov	r0, r4
    6760:	e3a01001 	mov	r1, #1	; 0x1
    6764:	ebfff978 	bl	4d4c <vs_assign_by_id>
    6768:	e3500000 	cmp	r0, #0	; 0x0
    676c:	0a000005 	beq	6788 <burn_images+0x11c>
    6770:	e59f00d0 	ldr	r0, [pc, #208]	; 6848 <burn_images+0x1dc>
    6774:	e59f30b0 	ldr	r3, [pc, #176]	; 682c <burn_images+0x1c0>
    6778:	e1a0e00f 	mov	lr, pc
    677c:	e593f000 	ldr	pc, [r3]
    6780:	e3e04002 	mvn	r4, #2	; 0x2
    6784:	ea000020 	b	680c <burn_images+0x1a0>
    6788:	e3a0e302 	mov	lr, #134217728	; 0x8000000
    678c:	e1a0c826 	lsr	ip, r6, #16
    6790:	e1a0c80c 	lsl	ip, ip, #16
    6794:	e1a03004 	mov	r3, r4
    6798:	e1a00007 	mov	r0, r7
    679c:	e59d100c 	ldr	r1, [sp, #12]
    67a0:	e1a0200b 	mov	r2, fp
    67a4:	e88d5000 	stm	sp, {ip, lr}
    67a8:	ebfffeff 	bl	63ac <burn_image_loop>
    67ac:	e3500000 	cmp	r0, #0	; 0x0
    67b0:	aa000006 	bge	67d0 <burn_images+0x164>
    67b4:	e1a01008 	mov	r1, r8
    67b8:	e59f008c 	ldr	r0, [pc, #140]	; 684c <burn_images+0x1e0>
    67bc:	e59f3068 	ldr	r3, [pc, #104]	; 682c <burn_images+0x1c0>
    67c0:	e1a0e00f 	mov	lr, pc
    67c4:	e593f000 	ldr	pc, [r3]
    67c8:	e3e04000 	mvn	r4, #0	; 0x0
    67cc:	ea00000e 	b	680c <burn_images+0x1a0>
    67d0:	e3550001 	cmp	r5, #1	; 0x1
    67d4:	1a000003 	bne	67e8 <burn_images+0x17c>
    67d8:	e5970004 	ldr	r0, [r7, #4]
    67dc:	e3a01000 	mov	r1, #0	; 0x0
    67e0:	ebfffe67 	bl	6184 <cdbg_jump>
    67e4:	ea000003 	b	67f8 <burn_images+0x18c>
    67e8:	e3550000 	cmp	r5, #0	; 0x0
    67ec:	1a000001 	bne	67f8 <burn_images+0x18c>
    67f0:	e3a00302 	mov	r0, #134217728	; 0x8000000
    67f4:	ebfffed7 	bl	6358 <cdbg_config_isi>
    67f8:	e2888001 	add	r8, r8, #1	; 0x1
    67fc:	e2877010 	add	r7, r7, #16	; 0x10
    6800:	e1580009 	cmp	r8, r9
    6804:	baffffa7 	blt	66a8 <burn_images+0x3c>
    6808:	e3a04000 	mov	r4, #0	; 0x0
    680c:	e3a00302 	mov	r0, #134217728	; 0x8000000
    6810:	ebffebbd 	bl	170c <free>
    6814:	e1a00004 	mov	r0, r4
    6818:	e28dd014 	add	sp, sp, #20	; 0x14
    681c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6820:	e3a0e302 	mov	lr, #134217728	; 0x8000000
    6824:	e3a0400f 	mov	r4, #15	; 0xf
    6828:	eaffffd7 	b	678c <burn_images+0x120>
    682c:	0800d704 	.word	0x0800d704
    6830:	00012014 	.word	0x00012014
    6834:	00012031 	.word	0x00012031
    6838:	00012047 	.word	0x00012047
    683c:	00012064 	.word	0x00012064
    6840:	00012079 	.word	0x00012079
    6844:	0001208c 	.word	0x0001208c
    6848:	000120a0 	.word	0x000120a0
    684c:	000120bd 	.word	0x000120bd

00006850 <burn>:
    6850:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6854:	e1a09000 	mov	r9, r0
    6858:	e24dd00c 	sub	sp, sp, #12	; 0xc
    685c:	e3a00c02 	mov	r0, #512	; 0x200
    6860:	ebffeb41 	bl	156c <malloc>
    6864:	e2507000 	subs	r7, r0, #0	; 0x0
    6868:	1a000004 	bne	6880 <burn+0x30>
    686c:	e59f019c 	ldr	r0, [pc, #412]	; 6a10 <burn+0x1c0>
    6870:	e59f319c 	ldr	r3, [pc, #412]	; 6a14 <burn+0x1c4>
    6874:	e1a0e00f 	mov	lr, pc
    6878:	e593f000 	ldr	pc, [r3]
    687c:	ea000060 	b	6a04 <burn+0x1b4>
    6880:	ebffe75f 	bl	604 <boot_device>
    6884:	e3a05000 	mov	r5, #0	; 0x0
    6888:	e59f8184 	ldr	r8, [pc, #388]	; 6a14 <burn+0x1c4>
    688c:	e3a0bc02 	mov	fp, #512	; 0x200
    6890:	e1a0a000 	mov	sl, r0
    6894:	ea00004c 	b	69cc <burn+0x17c>
    6898:	e1a00005 	mov	r0, r5
    689c:	ebfff959 	bl	4e08 <vs_device_burnsrc>
    68a0:	e3500000 	cmp	r0, #0	; 0x0
    68a4:	0a000047 	beq	69c8 <burn+0x178>
    68a8:	e155000a 	cmp	r5, sl
    68ac:	0a000045 	beq	69c8 <burn+0x178>
    68b0:	e3a03001 	mov	r3, #1	; 0x1
    68b4:	e0193513 	ands	r3, r9, r3, lsl r5
    68b8:	1a000042 	bne	69c8 <burn+0x178>
    68bc:	ebffe80e 	bl	8fc <gtc_time>
    68c0:	e1a00005 	mov	r0, r5
    68c4:	e59f114c 	ldr	r1, [pc, #332]	; 6a18 <burn+0x1c8>
    68c8:	ebfff9e3 	bl	505c <vs_is_device>
    68cc:	e3500000 	cmp	r0, #0	; 0x0
    68d0:	1a000004 	bne	68e8 <burn+0x98>
    68d4:	e1a00005 	mov	r0, r5
    68d8:	e59f113c 	ldr	r1, [pc, #316]	; 6a1c <burn+0x1cc>
    68dc:	ebfff9de 	bl	505c <vs_is_device>
    68e0:	e3500000 	cmp	r0, #0	; 0x0
    68e4:	0a00000c 	beq	691c <burn+0xcc>
    68e8:	ebffe895 	bl	b44 <assert_linkpc>
    68ec:	e3500000 	cmp	r0, #0	; 0x0
    68f0:	0a000009 	beq	691c <burn+0xcc>
    68f4:	eb001511 	bl	bd40 <linkpc_loop>
    68f8:	e2506000 	subs	r6, r0, #0	; 0x0
    68fc:	0a000038 	beq	69e4 <burn+0x194>
    6900:	e1a00005 	mov	r0, r5
    6904:	e5984000 	ldr	r4, [r8]
    6908:	ebfff974 	bl	4ee0 <vs_device_name>
    690c:	e1a02006 	mov	r2, r6
    6910:	e1a01000 	mov	r1, r0
    6914:	e59f0104 	ldr	r0, [pc, #260]	; 6a20 <burn+0x1d0>
    6918:	ea00000a 	b	6948 <burn+0xf8>
    691c:	e1a00005 	mov	r0, r5
    6920:	e3a01001 	mov	r1, #1	; 0x1
    6924:	ebfff908 	bl	4d4c <vs_assign_by_id>
    6928:	e2506000 	subs	r6, r0, #0	; 0x0
    692c:	0a000007 	beq	6950 <burn+0x100>
    6930:	e1a00005 	mov	r0, r5
    6934:	e5984000 	ldr	r4, [r8]
    6938:	ebfff968 	bl	4ee0 <vs_device_name>
    693c:	e1a01000 	mov	r1, r0
    6940:	e59f00dc 	ldr	r0, [pc, #220]	; 6a24 <burn+0x1d4>
    6944:	e1a02006 	mov	r2, r6
    6948:	e12fff34 	blx	r4
    694c:	ea00001d 	b	69c8 <burn+0x178>
    6950:	e1a01007 	mov	r1, r7
    6954:	e59f00cc 	ldr	r0, [pc, #204]	; 6a28 <burn+0x1d8>
    6958:	e3a02401 	mov	r2, #16777216	; 0x1000000
    695c:	e3a03c02 	mov	r3, #512	; 0x200
    6960:	e58d6000 	str	r6, [sp]
    6964:	e1a0e00f 	mov	lr, pc
    6968:	e598f000 	ldr	pc, [r8]
    696c:	e3a02401 	mov	r2, #16777216	; 0x1000000
    6970:	e3a03000 	mov	r3, #0	; 0x0
    6974:	e1a00007 	mov	r0, r7
    6978:	e58d6004 	str	r6, [sp, #4]
    697c:	e58db000 	str	fp, [sp]
    6980:	ebfff9ce 	bl	50c0 <vs_read>
    6984:	e1a00007 	mov	r0, r7
    6988:	eb001962 	bl	cf18 <ius_check_header>
    698c:	e2504000 	subs	r4, r0, #0	; 0x0
    6990:	1a00000c 	bne	69c8 <burn+0x178>
    6994:	e1a01005 	mov	r1, r5
    6998:	e59f3074 	ldr	r3, [pc, #116]	; 6a14 <burn+0x1c4>
    699c:	e59f0088 	ldr	r0, [pc, #136]	; 6a2c <burn+0x1dc>
    69a0:	e1a0e00f 	mov	lr, pc
    69a4:	e593f000 	ldr	pc, [r3]
    69a8:	e1a00004 	mov	r0, r4
    69ac:	e1a01004 	mov	r1, r4
    69b0:	ebfffde8 	bl	6158 <cdbg_verify_burn_enable>
    69b4:	e1a00007 	mov	r0, r7
    69b8:	e1a01005 	mov	r1, r5
    69bc:	e1a02004 	mov	r2, r4
    69c0:	ebffff29 	bl	666c <burn_images>
    69c4:	ea000006 	b	69e4 <burn+0x194>
    69c8:	e2855001 	add	r5, r5, #1	; 0x1
    69cc:	ebfff964 	bl	4f64 <vs_device_count>
    69d0:	e1550000 	cmp	r5, r0
    69d4:	baffffaf 	blt	6898 <burn+0x48>
    69d8:	e1a00007 	mov	r0, r7
    69dc:	ebffeb4a 	bl	170c <free>
    69e0:	ea000007 	b	6a04 <burn+0x1b4>
    69e4:	e59f0044 	ldr	r0, [pc, #68]	; 6a30 <burn+0x1e0>
    69e8:	e59f3024 	ldr	r3, [pc, #36]	; 6a14 <burn+0x1c4>
    69ec:	e1a0e00f 	mov	lr, pc
    69f0:	e593f000 	ldr	pc, [r3]
    69f4:	e1a00007 	mov	r0, r7
    69f8:	ebffeb43 	bl	170c <free>
    69fc:	ebfffe12 	bl	624c <cdbg_shell>
    6a00:	eafffffe 	b	6a00 <burn+0x1b0>
    6a04:	e3e00000 	mvn	r0, #0	; 0x0
    6a08:	e28dd00c 	add	sp, sp, #12	; 0xc
    6a0c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6a10:	000120d5 	.word	0x000120d5
    6a14:	0800d704 	.word	0x0800d704
    6a18:	00011c7f 	.word	0x00011c7f
    6a1c:	00011c7b 	.word	0x00011c7b
    6a20:	000120f5 	.word	0x000120f5
    6a24:	00012117 	.word	0x00012117
    6a28:	00012136 	.word	0x00012136
    6a2c:	00012166 	.word	0x00012166
    6a30:	0001217e 	.word	0x0001217e

00006a34 <mmc_send_cmd>:
    6a34:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6a38:	e24dd004 	sub	sp, sp, #4	; 0x4
    6a3c:	e1a0e00f 	mov	lr, pc
    6a40:	e590f104 	ldr	pc, [r0, #260]
    6a44:	e28dd004 	add	sp, sp, #4	; 0x4
    6a48:	e8bd8000 	pop	{pc}

00006a4c <mmc_set_blocklen>:
    6a4c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6a50:	e3a03015 	mov	r3, #21	; 0x15
    6a54:	e24dd024 	sub	sp, sp, #36	; 0x24
    6a58:	e3a02000 	mov	r2, #0	; 0x0
    6a5c:	e58d3004 	str	r3, [sp, #4]
    6a60:	e3a03010 	mov	r3, #16	; 0x10
    6a64:	e58d1008 	str	r1, [sp, #8]
    6a68:	e1cd30b0 	strh	r3, [sp]
    6a6c:	e58d201c 	str	r2, [sp, #28]
    6a70:	e1a0100d 	mov	r1, sp
    6a74:	e1a0e00f 	mov	lr, pc
    6a78:	e590f104 	ldr	pc, [r0, #260]
    6a7c:	e28dd024 	add	sp, sp, #36	; 0x24
    6a80:	e8bd8000 	pop	{pc}

00006a84 <find_mmc_device>:
    6a84:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6a88:	e59f3048 	ldr	r3, [pc, #72]	; 6ad8 <find_mmc_device+0x54>
    6a8c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6a90:	e1a01000 	mov	r1, r0
    6a94:	e1a0c003 	mov	ip, r3
    6a98:	e5932000 	ldr	r2, [r3]
    6a9c:	ea000004 	b	6ab4 <find_mmc_device+0x30>
    6aa0:	e592309c 	ldr	r3, [r2, #156]
    6aa4:	e1a00002 	mov	r0, r2
    6aa8:	e1530001 	cmp	r3, r1
    6aac:	0a000007 	beq	6ad0 <find_mmc_device+0x4c>
    6ab0:	e5922000 	ldr	r2, [r2]
    6ab4:	e152000c 	cmp	r2, ip
    6ab8:	1afffff8 	bne	6aa0 <find_mmc_device+0x1c>
    6abc:	e59f0018 	ldr	r0, [pc, #24]	; 6adc <find_mmc_device+0x58>
    6ac0:	e59f3018 	ldr	r3, [pc, #24]	; 6ae0 <find_mmc_device+0x5c>
    6ac4:	e1a0e00f 	mov	lr, pc
    6ac8:	e593f000 	ldr	pc, [r3]
    6acc:	e3a00000 	mov	r0, #0	; 0x0
    6ad0:	e28dd004 	add	sp, sp, #4	; 0x4
    6ad4:	e8bd8000 	pop	{pc}
    6ad8:	0800d1f8 	.word	0x0800d1f8
    6adc:	000121a1 	.word	0x000121a1
    6ae0:	0800d704 	.word	0x0800d704

00006ae4 <mmc_send_ext_csd>:
    6ae4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6ae8:	e3a03015 	mov	r3, #21	; 0x15
    6aec:	e24dd034 	sub	sp, sp, #52	; 0x34
    6af0:	e58d3004 	str	r3, [sp, #4]
    6af4:	e3a03c02 	mov	r3, #512	; 0x200
    6af8:	e3a02000 	mov	r2, #0	; 0x0
    6afc:	e3a0c001 	mov	ip, #1	; 0x1
    6b00:	e58d302c 	str	r3, [sp, #44]
    6b04:	e3a03008 	mov	r3, #8	; 0x8
    6b08:	e58d1020 	str	r1, [sp, #32]
    6b0c:	e58d201c 	str	r2, [sp, #28]
    6b10:	e58d2008 	str	r2, [sp, #8]
    6b14:	e58dc024 	str	ip, [sp, #36]
    6b18:	e1cd30b0 	strh	r3, [sp]
    6b1c:	e58dc028 	str	ip, [sp, #40]
    6b20:	e1a0100d 	mov	r1, sp
    6b24:	e28d2020 	add	r2, sp, #32	; 0x20
    6b28:	e1a0e00f 	mov	lr, pc
    6b2c:	e590f104 	ldr	pc, [r0, #260]
    6b30:	e28dd034 	add	sp, sp, #52	; 0x34
    6b34:	e8bd8000 	pop	{pc}

00006b38 <sd_switch>:
    6b38:	e3e0c4ff 	mvn	ip, #-16777216	; 0xff000000
    6b3c:	e1a02102 	lsl	r2, r2, #2
    6b40:	e18ccf81 	orr	ip, ip, r1, lsl #31
    6b44:	e3a0100f 	mov	r1, #15	; 0xf
    6b48:	e1ccc211 	bic	ip, ip, r1, lsl r2
    6b4c:	e18cc213 	orr	ip, ip, r3, lsl r2
    6b50:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6b54:	e3a03015 	mov	r3, #21	; 0x15
    6b58:	e24dd034 	sub	sp, sp, #52	; 0x34
    6b5c:	e58d3004 	str	r3, [sp, #4]
    6b60:	e3a03000 	mov	r3, #0	; 0x0
    6b64:	e58d301c 	str	r3, [sp, #28]
    6b68:	e59d3038 	ldr	r3, [sp, #56]
    6b6c:	e3a02001 	mov	r2, #1	; 0x1
    6b70:	e58d3020 	str	r3, [sp, #32]
    6b74:	e3a03040 	mov	r3, #64	; 0x40
    6b78:	e58d302c 	str	r3, [sp, #44]
    6b7c:	e3a03006 	mov	r3, #6	; 0x6
    6b80:	e58d2024 	str	r2, [sp, #36]
    6b84:	e58d2028 	str	r2, [sp, #40]
    6b88:	e58dc008 	str	ip, [sp, #8]
    6b8c:	e1cd30b0 	strh	r3, [sp]
    6b90:	e1a0100d 	mov	r1, sp
    6b94:	e28d2020 	add	r2, sp, #32	; 0x20
    6b98:	e1a0e00f 	mov	lr, pc
    6b9c:	e590f104 	ldr	pc, [r0, #260]
    6ba0:	e28dd034 	add	sp, sp, #52	; 0x34
    6ba4:	e8bd8000 	pop	{pc}

00006ba8 <mmc_set_ios>:
    6ba8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6bac:	e24dd004 	sub	sp, sp, #4	; 0x4
    6bb0:	e1a0e00f 	mov	lr, pc
    6bb4:	e590f108 	ldr	pc, [r0, #264]
    6bb8:	e28dd004 	add	sp, sp, #4	; 0x4
    6bbc:	e8bd8000 	pop	{pc}

00006bc0 <mmc_set_clock>:
    6bc0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6bc4:	e590203c 	ldr	r2, [r0, #60]
    6bc8:	e590c038 	ldr	ip, [r0, #56]
    6bcc:	e1510002 	cmp	r1, r2
    6bd0:	21a01002 	movcs	r1, r2
    6bd4:	e24dd004 	sub	sp, sp, #4	; 0x4
    6bd8:	e151000c 	cmp	r1, ip
    6bdc:	25801048 	strcs	r1, [r0, #72]
    6be0:	3580c048 	strcc	ip, [r0, #72]
    6be4:	e1a0e00f 	mov	lr, pc
    6be8:	e590f108 	ldr	pc, [r0, #264]
    6bec:	e28dd004 	add	sp, sp, #4	; 0x4
    6bf0:	e8bd8000 	pop	{pc}

00006bf4 <mmc_set_bus_width>:
    6bf4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6bf8:	e5801044 	str	r1, [r0, #68]
    6bfc:	e24dd004 	sub	sp, sp, #4	; 0x4
    6c00:	e1a0e00f 	mov	lr, pc
    6c04:	e590f108 	ldr	pc, [r0, #264]
    6c08:	e28dd004 	add	sp, sp, #4	; 0x4
    6c0c:	e8bd8000 	pop	{pc}

00006c10 <mmc_send_if_cond>:
    6c10:	e92d4010 	push	{r4, lr}
    6c14:	e590202c 	ldr	r2, [r0, #44]
    6c18:	e59f3060 	ldr	r3, [pc, #96]	; 6c80 <mmc_send_if_cond+0x70>
    6c1c:	e24dd020 	sub	sp, sp, #32	; 0x20
    6c20:	e0023003 	and	r3, r2, r3
    6c24:	e3530000 	cmp	r3, #0	; 0x0
    6c28:	e28d1020 	add	r1, sp, #32	; 0x20
    6c2c:	e3a03008 	mov	r3, #8	; 0x8
    6c30:	e300c1aa 	movw	ip, #426	; 0x1aa
    6c34:	e16132b0 	strh	r3, [r1, #-32]!
    6c38:	03a0c0aa 	moveq	ip, #170	; 0xaa
    6c3c:	e3a02000 	mov	r2, #0	; 0x0
    6c40:	e3a03015 	mov	r3, #21	; 0x15
    6c44:	e98d1008 	stmib	sp, {r3, ip}
    6c48:	e58d201c 	str	r2, [sp, #28]
    6c4c:	e1a0100d 	mov	r1, sp
    6c50:	e5903104 	ldr	r3, [r0, #260]
    6c54:	e1a04000 	mov	r4, r0
    6c58:	e12fff33 	blx	r3
    6c5c:	e3500000 	cmp	r0, #0	; 0x0
    6c60:	1a000004 	bne	6c78 <mmc_send_if_cond+0x68>
    6c64:	e5dd300c 	ldrb	r3, [sp, #12]
    6c68:	e35300aa 	cmp	r3, #170	; 0xaa
    6c6c:	059f3010 	ldreq	r3, [pc, #16]	; 6c84 <mmc_send_if_cond+0x74>
    6c70:	13e00010 	mvnne	r0, #16	; 0x10
    6c74:	05843030 	streq	r3, [r4, #48]
    6c78:	e28dd020 	add	sp, sp, #32	; 0x20
    6c7c:	e8bd8010 	pop	{r4, pc}
    6c80:	00ff8000 	.word	0x00ff8000
    6c84:	00020020 	.word	0x00020020

00006c88 <mmc_register>:
    6c88:	e5903110 	ldr	r3, [r0, #272]
    6c8c:	e59fc05c 	ldr	ip, [pc, #92]	; 6cf0 <mmc_register+0x68>
    6c90:	e3530000 	cmp	r3, #0	; 0x0
    6c94:	e3a03006 	mov	r3, #6	; 0x6
    6c98:	e59c2000 	ldr	r2, [ip]
    6c9c:	e5803098 	str	r3, [r0, #152]
    6ca0:	e59f304c 	ldr	r3, [pc, #76]	; 6cf4 <mmc_register+0x6c>
    6ca4:	e2821001 	add	r1, r2, #1	; 0x1
    6ca8:	e58030f8 	str	r3, [r0, #248]
    6cac:	e59f3044 	ldr	r3, [pc, #68]	; 6cf8 <mmc_register+0x70>
    6cb0:	e580209c 	str	r2, [r0, #156]
    6cb4:	e58030fc 	str	r3, [r0, #252]
    6cb8:	e3a03001 	mov	r3, #1	; 0x1
    6cbc:	e58c1000 	str	r1, [ip]
    6cc0:	e5c030a4 	strb	r3, [r0, #164]
    6cc4:	028330ff 	addeq	r3, r3, #255	; 0xff
    6cc8:	05803110 	streq	r3, [r0, #272]
    6ccc:	e59f3028 	ldr	r3, [pc, #40]	; 6cfc <mmc_register+0x74>
    6cd0:	e5800004 	str	r0, [r0, #4]
    6cd4:	e5932004 	ldr	r2, [r3, #4]
    6cd8:	e5803000 	str	r3, [r0]
    6cdc:	e5830004 	str	r0, [r3, #4]
    6ce0:	e5820000 	str	r0, [r2]
    6ce4:	e5802004 	str	r2, [r0, #4]
    6ce8:	e3a00000 	mov	r0, #0	; 0x0
    6cec:	e12fff1e 	bx	lr
    6cf0:	0800d0dc 	.word	0x0800d0dc
    6cf4:	00007ec4 	.word	0x00007ec4
    6cf8:	000073e0 	.word	0x000073e0
    6cfc:	0800d1f8 	.word	0x0800d1f8

00006d00 <mmc_get_dev>:
    6d00:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6d04:	e24dd004 	sub	sp, sp, #4	; 0x4
    6d08:	ebffff5d 	bl	6a84 <find_mmc_device>
    6d0c:	e3500000 	cmp	r0, #0	; 0x0
    6d10:	12800098 	addne	r0, r0, #152	; 0x98
    6d14:	e28dd004 	add	sp, sp, #4	; 0x4
    6d18:	e8bd8000 	pop	{pc}

00006d1c <print_mmc_devices>:
    6d1c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6d20:	e59f3068 	ldr	r3, [pc, #104]	; 6d90 <print_mmc_devices+0x74>
    6d24:	e24dd004 	sub	sp, sp, #4	; 0x4
    6d28:	e1a05000 	mov	r5, r0
    6d2c:	e1a07003 	mov	r7, r3
    6d30:	e5934000 	ldr	r4, [r3]
    6d34:	e59f6058 	ldr	r6, [pc, #88]	; 6d94 <print_mmc_devices+0x78>
    6d38:	ea00000a 	b	6d68 <print_mmc_devices+0x4c>
    6d3c:	e594209c 	ldr	r2, [r4, #156]
    6d40:	e1a0e00f 	mov	lr, pc
    6d44:	e596f000 	ldr	pc, [r6]
    6d48:	e5943000 	ldr	r3, [r4]
    6d4c:	e59f0044 	ldr	r0, [pc, #68]	; 6d98 <print_mmc_devices+0x7c>
    6d50:	e1530007 	cmp	r3, r7
    6d54:	e1a01005 	mov	r1, r5
    6d58:	0a000001 	beq	6d64 <print_mmc_devices+0x48>
    6d5c:	e1a0e00f 	mov	lr, pc
    6d60:	e596f000 	ldr	pc, [r6]
    6d64:	e5944000 	ldr	r4, [r4]
    6d68:	e1540007 	cmp	r4, r7
    6d6c:	e2841008 	add	r1, r4, #8	; 0x8
    6d70:	e59f0024 	ldr	r0, [pc, #36]	; 6d9c <print_mmc_devices+0x80>
    6d74:	1afffff0 	bne	6d3c <print_mmc_devices+0x20>
    6d78:	e59f0020 	ldr	r0, [pc, #32]	; 6da0 <print_mmc_devices+0x84>
    6d7c:	e59f3010 	ldr	r3, [pc, #16]	; 6d94 <print_mmc_devices+0x78>
    6d80:	e1a0e00f 	mov	lr, pc
    6d84:	e593f000 	ldr	pc, [r3]
    6d88:	e28dd004 	add	sp, sp, #4	; 0x4
    6d8c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6d90:	0800d1f8 	.word	0x0800d1f8
    6d94:	0800d704 	.word	0x0800d704
    6d98:	000121c1 	.word	0x000121c1
    6d9c:	000121ba 	.word	0x000121ba
    6da0:	0001140d 	.word	0x0001140d

00006da4 <get_mmc_num>:
    6da4:	e59f3004 	ldr	r3, [pc, #4]	; 6db0 <get_mmc_num+0xc>
    6da8:	e5930000 	ldr	r0, [r3]
    6dac:	e12fff1e 	bx	lr
    6db0:	0800d0dc 	.word	0x0800d0dc

00006db4 <mmc_vs_align>:
    6db4:	e3a00c02 	mov	r0, #512	; 0x200
    6db8:	e12fff1e 	bx	lr

00006dbc <init_mmc>:
    6dbc:	e59f3060 	ldr	r3, [pc, #96]	; 6e24 <init_mmc+0x68>
    6dc0:	e59f2060 	ldr	r2, [pc, #96]	; 6e28 <init_mmc+0x6c>
    6dc4:	e92d4070 	push	{r4, r5, r6, lr}
    6dc8:	e3a04000 	mov	r4, #0	; 0x0
    6dcc:	e3a00b02 	mov	r0, #2048	; 0x800
    6dd0:	e5833004 	str	r3, [r3, #4]
    6dd4:	e5824000 	str	r4, [r2]
    6dd8:	e5833000 	str	r3, [r3]
    6ddc:	ebffe9e2 	bl	156c <malloc>
    6de0:	e1a05000 	mov	r5, r0
    6de4:	e2806b01 	add	r6, r0, #1024	; 0x400
    6de8:	e1a00005 	mov	r0, r5
    6dec:	e3a01000 	mov	r1, #0	; 0x0
    6df0:	e3a02f4e 	mov	r2, #312	; 0x138
    6df4:	ebffe96f 	bl	13b8 <memset>
    6df8:	e5856130 	str	r6, [r5, #304]
    6dfc:	e1a00004 	mov	r0, r4
    6e00:	e1a01005 	mov	r1, r5
    6e04:	eb000490 	bl	804c <imap_sdhc_initialize>
    6e08:	e3500000 	cmp	r0, #0	; 0x0
    6e0c:	e2844001 	add	r4, r4, #1	; 0x1
    6e10:	e2855f4e 	add	r5, r5, #312	; 0x138
    6e14:	18bd8070 	popne	{r4, r5, r6, pc}
    6e18:	e3540003 	cmp	r4, #3	; 0x3
    6e1c:	1afffff1 	bne	6de8 <init_mmc+0x2c>
    6e20:	e8bd8070 	pop	{r4, r5, r6, pc}
    6e24:	0800d1f8 	.word	0x0800d1f8
    6e28:	0800d0dc 	.word	0x0800d0dc

00006e2c <sd_change_freq>:
    6e2c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6e30:	e5903050 	ldr	r3, [r0, #80]
    6e34:	e3a01000 	mov	r1, #0	; 0x0
    6e38:	e2132b01 	ands	r2, r3, #1024	; 0x400
    6e3c:	e1a04000 	mov	r4, r0
    6e40:	e580104c 	str	r1, [r0, #76]
    6e44:	e24dd084 	sub	sp, sp, #132	; 0x84
    6e48:	11a00001 	movne	r0, r1
    6e4c:	1a000070 	bne	7014 <sd_change_freq+0x1e8>
    6e50:	e1d438b0 	ldrh	r3, [r4, #128]
    6e54:	e3a05015 	mov	r5, #21	; 0x15
    6e58:	e1a03803 	lsl	r3, r3, #16
    6e5c:	e58d3050 	str	r3, [sp, #80]
    6e60:	e28d6048 	add	r6, sp, #72	; 0x48
    6e64:	e3a03037 	mov	r3, #55	; 0x37
    6e68:	e1cd34b8 	strh	r3, [sp, #72]
    6e6c:	e58d504c 	str	r5, [sp, #76]
    6e70:	e58d2064 	str	r2, [sp, #100]
    6e74:	e1a01006 	mov	r1, r6
    6e78:	e1a0e00f 	mov	lr, pc
    6e7c:	e594f104 	ldr	pc, [r4, #260]
    6e80:	e3500000 	cmp	r0, #0	; 0x0
    6e84:	1a000062 	bne	7014 <sd_change_freq+0x1e8>
    6e88:	e3a03033 	mov	r3, #51	; 0x33
    6e8c:	e58d504c 	str	r5, [sp, #76]
    6e90:	e1a09006 	mov	r9, r6
    6e94:	e58d0064 	str	r0, [sp, #100]
    6e98:	e1cd34b8 	strh	r3, [sp, #72]
    6e9c:	e58d0050 	str	r0, [sp, #80]
    6ea0:	e3a05003 	mov	r5, #3	; 0x3
    6ea4:	e28da078 	add	sl, sp, #120	; 0x78
    6ea8:	e3a08008 	mov	r8, #8	; 0x8
    6eac:	e3a06001 	mov	r6, #1	; 0x1
    6eb0:	e28d7068 	add	r7, sp, #104	; 0x68
    6eb4:	e58da068 	str	sl, [sp, #104]
    6eb8:	e58d8074 	str	r8, [sp, #116]
    6ebc:	e58d6070 	str	r6, [sp, #112]
    6ec0:	e58d606c 	str	r6, [sp, #108]
    6ec4:	e1a00004 	mov	r0, r4
    6ec8:	e1a01009 	mov	r1, r9
    6ecc:	e1a02007 	mov	r2, r7
    6ed0:	e1a0e00f 	mov	lr, pc
    6ed4:	e594f104 	ldr	pc, [r4, #260]
    6ed8:	e250b000 	subs	fp, r0, #0	; 0x0
    6edc:	0a000003 	beq	6ef0 <sd_change_freq+0xc4>
    6ee0:	e3550000 	cmp	r5, #0	; 0x0
    6ee4:	e2455001 	sub	r5, r5, #1	; 0x1
    6ee8:	1afffff1 	bne	6eb4 <sd_change_freq+0x88>
    6eec:	ea000047 	b	7010 <sd_change_freq+0x1e4>
    6ef0:	e59d3078 	ldr	r3, [sp, #120]
    6ef4:	e59d007c 	ldr	r0, [sp, #124]
    6ef8:	e1a01c03 	lsl	r1, r3, #24
    6efc:	e2032cff 	and	r2, r3, #65280	; 0xff00
    6f00:	e1811c23 	orr	r1, r1, r3, lsr #24
    6f04:	e1811402 	orr	r1, r1, r2, lsl #8
    6f08:	e20338ff 	and	r3, r3, #16711680	; 0xff0000
    6f0c:	e1811423 	orr	r1, r1, r3, lsr #8
    6f10:	e1a03c00 	lsl	r3, r0, #24
    6f14:	e1833c20 	orr	r3, r3, r0, lsr #24
    6f18:	e2002cff 	and	r2, r0, #65280	; 0xff00
    6f1c:	e1a0cc21 	lsr	ip, r1, #24
    6f20:	e1833402 	orr	r3, r3, r2, lsl #8
    6f24:	e20008ff 	and	r0, r0, #16711680	; 0xff0000
    6f28:	e20cc00f 	and	ip, ip, #15	; 0xf
    6f2c:	e1833420 	orr	r3, r3, r0, lsr #8
    6f30:	e35c0001 	cmp	ip, #1	; 0x1
    6f34:	e584305c 	str	r3, [r4, #92]
    6f38:	e5841058 	str	r1, [r4, #88]
    6f3c:	059f30d8 	ldreq	r3, [pc, #216]	; 701c <sd_change_freq+0x1f0>
    6f40:	0a000004 	beq	6f58 <sd_change_freq+0x12c>
    6f44:	3a000002 	bcc	6f54 <sd_change_freq+0x128>
    6f48:	e35c0002 	cmp	ip, #2	; 0x2
    6f4c:	059f30cc 	ldreq	r3, [pc, #204]	; 7020 <sd_change_freq+0x1f4>
    6f50:	0a000000 	beq	6f58 <sd_change_freq+0x12c>
    6f54:	e59f30c8 	ldr	r3, [pc, #200]	; 7024 <sd_change_freq+0x1f8>
    6f58:	e5843030 	str	r3, [r4, #48]
    6f5c:	e5943058 	ldr	r3, [r4, #88]
    6f60:	e5942030 	ldr	r2, [r4, #48]
    6f64:	e3130701 	tst	r3, #262144	; 0x40000
    6f68:	1594304c 	ldrne	r3, [r4, #76]
    6f6c:	13833c01 	orrne	r3, r3, #256	; 0x100
    6f70:	1584304c 	strne	r3, [r4, #76]
    6f74:	e59f30a8 	ldr	r3, [pc, #168]	; 7024 <sd_change_freq+0x1f8>
    6f78:	e1520003 	cmp	r2, r3
    6f7c:	13a05004 	movne	r5, #4	; 0x4
    6f80:	128d6008 	addne	r6, sp, #8	; 0x8
    6f84:	1a000007 	bne	6fa8 <sd_change_freq+0x17c>
    6f88:	ea000020 	b	7010 <sd_change_freq+0x1e4>
    6f8c:	e58d6000 	str	r6, [sp]
    6f90:	ebfffee8 	bl	6b38 <sd_switch>
    6f94:	e3500000 	cmp	r0, #0	; 0x0
    6f98:	1a00001d 	bne	7014 <sd_change_freq+0x1e8>
    6f9c:	e59d3024 	ldr	r3, [sp, #36]
    6fa0:	e3130c02 	tst	r3, #512	; 0x200
    6fa4:	0a000005 	beq	6fc0 <sd_change_freq+0x194>
    6fa8:	e3a01000 	mov	r1, #0	; 0x0
    6fac:	e2555001 	subs	r5, r5, #1	; 0x1
    6fb0:	e1a00004 	mov	r0, r4
    6fb4:	e1a02001 	mov	r2, r1
    6fb8:	e3a03001 	mov	r3, #1	; 0x1
    6fbc:	2afffff2 	bcs	6f8c <sd_change_freq+0x160>
    6fc0:	e59d3014 	ldr	r3, [sp, #20]
    6fc4:	e3130c02 	tst	r3, #512	; 0x200
    6fc8:	0a000010 	beq	7010 <sd_change_freq+0x1e4>
    6fcc:	e3a01001 	mov	r1, #1	; 0x1
    6fd0:	e28dc008 	add	ip, sp, #8	; 0x8
    6fd4:	e1a00004 	mov	r0, r4
    6fd8:	e3a02000 	mov	r2, #0	; 0x0
    6fdc:	e1a03001 	mov	r3, r1
    6fe0:	e58dc000 	str	ip, [sp]
    6fe4:	ebfffed3 	bl	6b38 <sd_switch>
    6fe8:	e3500000 	cmp	r0, #0	; 0x0
    6fec:	1a000008 	bne	7014 <sd_change_freq+0x1e8>
    6ff0:	e59d3018 	ldr	r3, [sp, #24]
    6ff4:	e1a03c03 	lsl	r3, r3, #24
    6ff8:	e203340f 	and	r3, r3, #251658240	; 0xf000000
    6ffc:	e3530401 	cmp	r3, #16777216	; 0x1000000
    7000:	0594304c 	ldreq	r3, [r4, #76]
    7004:	03833001 	orreq	r3, r3, #1	; 0x1
    7008:	0584304c 	streq	r3, [r4, #76]
    700c:	ea000000 	b	7014 <sd_change_freq+0x1e8>
    7010:	e1a0000b 	mov	r0, fp
    7014:	e28dd084 	add	sp, sp, #132	; 0x84
    7018:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    701c:	0002001a 	.word	0x0002001a
    7020:	00020020 	.word	0x00020020
    7024:	00020010 	.word	0x00020010

00007028 <sd_send_op_cond>:
    7028:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    702c:	e59f9140 	ldr	r9, [pc, #320]	; 7174 <sd_send_op_cond+0x14c>
    7030:	e24dd020 	sub	sp, sp, #32	; 0x20
    7034:	e1a04000 	mov	r4, r0
    7038:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    703c:	e3a0a015 	mov	sl, #21	; 0x15
    7040:	e3a06000 	mov	r6, #0	; 0x0
    7044:	e1a0700d 	mov	r7, sp
    7048:	e3a08001 	mov	r8, #1	; 0x1
    704c:	e3a03037 	mov	r3, #55	; 0x37
    7050:	e1cd30b0 	strh	r3, [sp]
    7054:	e1a0100d 	mov	r1, sp
    7058:	e1a02006 	mov	r2, r6
    705c:	e58da004 	str	sl, [sp, #4]
    7060:	e58d6008 	str	r6, [sp, #8]
    7064:	e58d601c 	str	r6, [sp, #28]
    7068:	e1a00004 	mov	r0, r4
    706c:	e1a0e00f 	mov	lr, pc
    7070:	e594f104 	ldr	pc, [r4, #260]
    7074:	e2503000 	subs	r3, r0, #0	; 0x0
    7078:	e1a0100d 	mov	r1, sp
    707c:	e3a02000 	mov	r2, #0	; 0x0
    7080:	e59fe0f0 	ldr	lr, [pc, #240]	; 7178 <sd_send_op_cond+0x150>
    7084:	e1a00004 	mov	r0, r4
    7088:	1a000036 	bne	7168 <sd_send_op_cond+0x140>
    708c:	e594302c 	ldr	r3, [r4, #44]
    7090:	e594c030 	ldr	ip, [r4, #48]
    7094:	e003e00e 	and	lr, r3, lr
    7098:	e15c0009 	cmp	ip, r9
    709c:	038e3101 	orreq	r3, lr, #1073741824	; 0x40000000
    70a0:	e58de008 	str	lr, [sp, #8]
    70a4:	058d3008 	streq	r3, [sp, #8]
    70a8:	e3a03029 	mov	r3, #41	; 0x29
    70ac:	e1cd30b0 	strh	r3, [sp]
    70b0:	e58d8004 	str	r8, [sp, #4]
    70b4:	e5943104 	ldr	r3, [r4, #260]
    70b8:	e12fff33 	blx	r3
    70bc:	e2503000 	subs	r3, r0, #0	; 0x0
    70c0:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    70c4:	1a000027 	bne	7168 <sd_send_op_cond+0x140>
    70c8:	eb000652 	bl	8a18 <udelay>
    70cc:	e59d300c 	ldr	r3, [sp, #12]
    70d0:	e3530000 	cmp	r3, #0	; 0x0
    70d4:	ba000002 	blt	70e4 <sd_send_op_cond+0xbc>
    70d8:	e2555001 	subs	r5, r5, #1	; 0x1
    70dc:	2affffda 	bcs	704c <sd_send_op_cond+0x24>
    70e0:	ea00001f 	b	7164 <sd_send_op_cond+0x13c>
    70e4:	e3550000 	cmp	r5, #0	; 0x0
    70e8:	da00001d 	ble	7164 <sd_send_op_cond+0x13c>
    70ec:	e59f3080 	ldr	r3, [pc, #128]	; 7174 <sd_send_op_cond+0x14c>
    70f0:	e5942030 	ldr	r2, [r4, #48]
    70f4:	e1520003 	cmp	r2, r3
    70f8:	12433010 	subne	r3, r3, #16	; 0x10
    70fc:	15843030 	strne	r3, [r4, #48]
    7100:	e5943050 	ldr	r3, [r4, #80]
    7104:	e3130b01 	tst	r3, #1024	; 0x400
    7108:	0a00000c 	beq	7140 <sd_send_op_cond+0x118>
    710c:	e3a03001 	mov	r3, #1	; 0x1
    7110:	e3a02000 	mov	r2, #0	; 0x0
    7114:	e58d3004 	str	r3, [sp, #4]
    7118:	e3a0303a 	mov	r3, #58	; 0x3a
    711c:	e1cd30b0 	strh	r3, [sp]
    7120:	e58d2008 	str	r2, [sp, #8]
    7124:	e58d201c 	str	r2, [sp, #28]
    7128:	e1a00004 	mov	r0, r4
    712c:	e1a0100d 	mov	r1, sp
    7130:	e1a0e00f 	mov	lr, pc
    7134:	e594f104 	ldr	pc, [r4, #260]
    7138:	e2503000 	subs	r3, r0, #0	; 0x0
    713c:	1a000009 	bne	7168 <sd_send_op_cond+0x140>
    7140:	e59d200c 	ldr	r2, [sp, #12]
    7144:	e3a03000 	mov	r3, #0	; 0x0
    7148:	e1c438b0 	strh	r3, [r4, #128]
    714c:	e1a03f22 	lsr	r3, r2, #30
    7150:	e2033001 	and	r3, r3, #1	; 0x1
    7154:	e5843040 	str	r3, [r4, #64]
    7158:	e5842054 	str	r2, [r4, #84]
    715c:	e3a03000 	mov	r3, #0	; 0x0
    7160:	ea000000 	b	7168 <sd_send_op_cond+0x140>
    7164:	e3e03010 	mvn	r3, #16	; 0x10
    7168:	e1a00003 	mov	r0, r3
    716c:	e28dd020 	add	sp, sp, #32	; 0x20
    7170:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    7174:	00020020 	.word	0x00020020
    7178:	00ff8000 	.word	0x00ff8000

0000717c <mmc_go_idle>:
    717c:	e92d4010 	push	{r4, lr}
    7180:	e1a04000 	mov	r4, r0
    7184:	e24dd020 	sub	sp, sp, #32	; 0x20
    7188:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    718c:	eb000621 	bl	8a18 <udelay>
    7190:	e3a02000 	mov	r2, #0	; 0x0
    7194:	e1a00004 	mov	r0, r4
    7198:	e1cd20b0 	strh	r2, [sp]
    719c:	e58d2008 	str	r2, [sp, #8]
    71a0:	e58d2004 	str	r2, [sp, #4]
    71a4:	e58d201c 	str	r2, [sp, #28]
    71a8:	e1a0100d 	mov	r1, sp
    71ac:	e1a0e00f 	mov	lr, pc
    71b0:	e594f104 	ldr	pc, [r4, #260]
    71b4:	e2504000 	subs	r4, r0, #0	; 0x0
    71b8:	1a000001 	bne	71c4 <mmc_go_idle+0x48>
    71bc:	e2800e7d 	add	r0, r0, #2000	; 0x7d0
    71c0:	eb000614 	bl	8a18 <udelay>
    71c4:	e1a00004 	mov	r0, r4
    71c8:	e28dd020 	add	sp, sp, #32	; 0x20
    71cc:	e8bd8010 	pop	{r4, pc}

000071d0 <mmc_send_op_cond>:
    71d0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    71d4:	e24dd020 	sub	sp, sp, #32	; 0x20
    71d8:	e1a05000 	mov	r5, r0
    71dc:	e3a06001 	mov	r6, #1	; 0x1
    71e0:	ebffffe5 	bl	717c <mmc_go_idle>
    71e4:	e3a02000 	mov	r2, #0	; 0x0
    71e8:	e1cd60b0 	strh	r6, [sp]
    71ec:	e58d6004 	str	r6, [sp, #4]
    71f0:	e58d2008 	str	r2, [sp, #8]
    71f4:	e58d201c 	str	r2, [sp, #28]
    71f8:	e1a00005 	mov	r0, r5
    71fc:	e1a0100d 	mov	r1, sp
    7200:	e1a0e00f 	mov	lr, pc
    7204:	e595f104 	ldr	pc, [r5, #260]
    7208:	e2504000 	subs	r4, r0, #0	; 0x0
    720c:	e1a0700d 	mov	r7, sp
    7210:	1a000039 	bne	72fc <mmc_send_op_cond+0x12c>
    7214:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    7218:	eb0005fe 	bl	8a18 <udelay>
    721c:	e1a08006 	mov	r8, r6
    7220:	e1a0700d 	mov	r7, sp
    7224:	e1a06004 	mov	r6, r4
    7228:	e3024710 	movw	r4, #10000	; 0x2710
    722c:	e595302c 	ldr	r3, [r5, #44]
    7230:	e3a02001 	mov	r2, #1	; 0x1
    7234:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    7238:	e1cd20b0 	strh	r2, [sp]
    723c:	e58d3008 	str	r3, [sp, #8]
    7240:	e1a02006 	mov	r2, r6
    7244:	e58d8004 	str	r8, [sp, #4]
    7248:	e58d601c 	str	r6, [sp, #28]
    724c:	e1a0100d 	mov	r1, sp
    7250:	e1a00005 	mov	r0, r5
    7254:	e1a0e00f 	mov	lr, pc
    7258:	e595f104 	ldr	pc, [r5, #260]
    725c:	e2502000 	subs	r2, r0, #0	; 0x0
    7260:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    7264:	11a04002 	movne	r4, r2
    7268:	1a000023 	bne	72fc <mmc_send_op_cond+0x12c>
    726c:	eb0005e9 	bl	8a18 <udelay>
    7270:	e59d300c 	ldr	r3, [sp, #12]
    7274:	e3530000 	cmp	r3, #0	; 0x0
    7278:	ba000002 	blt	7288 <mmc_send_op_cond+0xb8>
    727c:	e2544001 	subs	r4, r4, #1	; 0x1
    7280:	2affffe9 	bcs	722c <mmc_send_op_cond+0x5c>
    7284:	ea00001b 	b	72f8 <mmc_send_op_cond+0x128>
    7288:	e3540000 	cmp	r4, #0	; 0x0
    728c:	da000019 	ble	72f8 <mmc_send_op_cond+0x128>
    7290:	e5953050 	ldr	r3, [r5, #80]
    7294:	e3130b01 	tst	r3, #1024	; 0x400
    7298:	0a00000b 	beq	72cc <mmc_send_op_cond+0xfc>
    729c:	e3a0303a 	mov	r3, #58	; 0x3a
    72a0:	e58d8004 	str	r8, [sp, #4]
    72a4:	e1cd30b0 	strh	r3, [sp]
    72a8:	e58d6008 	str	r6, [sp, #8]
    72ac:	e58d601c 	str	r6, [sp, #28]
    72b0:	e1a0100d 	mov	r1, sp
    72b4:	e1a02006 	mov	r2, r6
    72b8:	e1a00005 	mov	r0, r5
    72bc:	e1a0e00f 	mov	lr, pc
    72c0:	e595f104 	ldr	pc, [r5, #260]
    72c4:	e2504000 	subs	r4, r0, #0	; 0x0
    72c8:	1a00000b 	bne	72fc <mmc_send_op_cond+0x12c>
    72cc:	e59d300c 	ldr	r3, [sp, #12]
    72d0:	e3a01801 	mov	r1, #65536	; 0x10000
    72d4:	e1a02f23 	lsr	r2, r3, #30
    72d8:	e2022001 	and	r2, r2, #1	; 0x1
    72dc:	e5852040 	str	r2, [r5, #64]
    72e0:	e3a02000 	mov	r2, #0	; 0x0
    72e4:	e5851030 	str	r1, [r5, #48]
    72e8:	e1c528b0 	strh	r2, [r5, #128]
    72ec:	e5853054 	str	r3, [r5, #84]
    72f0:	e3a04000 	mov	r4, #0	; 0x0
    72f4:	ea000000 	b	72fc <mmc_send_op_cond+0x12c>
    72f8:	e3e04010 	mvn	r4, #16	; 0x10
    72fc:	e1a00004 	mov	r0, r4
    7300:	e28dd020 	add	sp, sp, #32	; 0x20
    7304:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

00007308 <mmc_send_status>:
    7308:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    730c:	e1d038b0 	ldrh	r3, [r0, #128]
    7310:	e24dd024 	sub	sp, sp, #36	; 0x24
    7314:	e1a03803 	lsl	r3, r3, #16
    7318:	e58d3008 	str	r3, [sp, #8]
    731c:	e3a03000 	mov	r3, #0	; 0x0
    7320:	e3a02015 	mov	r2, #21	; 0x15
    7324:	e58d301c 	str	r3, [sp, #28]
    7328:	e3a0300d 	mov	r3, #13	; 0xd
    732c:	e1a06000 	mov	r6, r0
    7330:	e58d2004 	str	r2, [sp, #4]
    7334:	e1a04001 	mov	r4, r1
    7338:	e1cd30b0 	strh	r3, [sp]
    733c:	e1a0700d 	mov	r7, sp
    7340:	e1a0100d 	mov	r1, sp
    7344:	e3a02000 	mov	r2, #0	; 0x0
    7348:	e1a00006 	mov	r0, r6
    734c:	e1a0e00f 	mov	lr, pc
    7350:	e596f104 	ldr	pc, [r6, #260]
    7354:	e2505000 	subs	r5, r0, #0	; 0x0
    7358:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    735c:	1a000018 	bne	73c4 <mmc_send_status+0xbc>
    7360:	e59d300c 	ldr	r3, [sp, #12]
    7364:	e3130c01 	tst	r3, #256	; 0x100
    7368:	1a00000e 	bne	73a8 <mmc_send_status+0xa0>
    736c:	eb0005a9 	bl	8a18 <udelay>
    7370:	e59d100c 	ldr	r1, [sp, #12]
    7374:	e59f3054 	ldr	r3, [pc, #84]	; 73d0 <mmc_send_status+0xc8>
    7378:	e2444001 	sub	r4, r4, #1	; 0x1
    737c:	e0013003 	and	r3, r1, r3
    7380:	e3530000 	cmp	r3, #0	; 0x0
    7384:	0a000005 	beq	73a0 <mmc_send_status+0x98>
    7388:	e59f0044 	ldr	r0, [pc, #68]	; 73d4 <mmc_send_status+0xcc>
    738c:	e59f3044 	ldr	r3, [pc, #68]	; 73d8 <mmc_send_status+0xd0>
    7390:	e1a0e00f 	mov	lr, pc
    7394:	e593f000 	ldr	pc, [r3]
    7398:	e3e05011 	mvn	r5, #17	; 0x11
    739c:	ea000008 	b	73c4 <mmc_send_status+0xbc>
    73a0:	e3740001 	cmn	r4, #1	; 0x1
    73a4:	1affffe5 	bne	7340 <mmc_send_status+0x38>
    73a8:	e3540000 	cmp	r4, #0	; 0x0
    73ac:	1a000004 	bne	73c4 <mmc_send_status+0xbc>
    73b0:	e59f0024 	ldr	r0, [pc, #36]	; 73dc <mmc_send_status+0xd4>
    73b4:	e59f301c 	ldr	r3, [pc, #28]	; 73d8 <mmc_send_status+0xd0>
    73b8:	e1a0e00f 	mov	lr, pc
    73bc:	e593f000 	ldr	pc, [r3]
    73c0:	e3e05012 	mvn	r5, #18	; 0x12
    73c4:	e1a00005 	mov	r0, r5
    73c8:	e28dd024 	add	sp, sp, #36	; 0x24
    73cc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    73d0:	fdf94080 	.word	0xfdf94080
    73d4:	000121c5 	.word	0x000121c5
    73d8:	0800d704 	.word	0x0800d704
    73dc:	000121db 	.word	0x000121db

000073e0 <mmc_bwrite>:
    73e0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    73e4:	e24dd034 	sub	sp, sp, #52	; 0x34
    73e8:	e1a06001 	mov	r6, r1
    73ec:	e1a0b002 	mov	fp, r2
    73f0:	e1a0a003 	mov	sl, r3
    73f4:	ebfffda2 	bl	6a84 <find_mmc_device>
    73f8:	e2504000 	subs	r4, r0, #0	; 0x0
    73fc:	0a000060 	beq	7584 <mmc_bwrite+0x1a4>
    7400:	e594108c 	ldr	r1, [r4, #140]
    7404:	ebfffd90 	bl	6a4c <mmc_set_blocklen>
    7408:	e3500000 	cmp	r0, #0	; 0x0
    740c:	01a09000 	moveq	r9, r0
    7410:	01a0700b 	moveq	r7, fp
    7414:	0a000005 	beq	7430 <mmc_bwrite+0x50>
    7418:	e594108c 	ldr	r1, [r4, #140]
    741c:	e59f016c 	ldr	r0, [pc, #364]	; 7590 <mmc_bwrite+0x1b0>
    7420:	e59f316c 	ldr	r3, [pc, #364]	; 7594 <mmc_bwrite+0x1b4>
    7424:	e1a0e00f 	mov	lr, pc
    7428:	e593f000 	ldr	pc, [r3]
    742c:	ea000054 	b	7584 <mmc_bwrite+0x1a4>
    7430:	e5943110 	ldr	r3, [r4, #272]
    7434:	e59420a8 	ldr	r2, [r4, #168]
    7438:	e1570003 	cmp	r7, r3
    743c:	31a05007 	movcc	r5, r7
    7440:	21a05003 	movcs	r5, r3
    7444:	e0858006 	add	r8, r5, r6
    7448:	e1580002 	cmp	r8, r2
    744c:	9a000005 	bls	7468 <mmc_bwrite+0x88>
    7450:	e59f0140 	ldr	r0, [pc, #320]	; 7598 <mmc_bwrite+0x1b8>
    7454:	e1a01008 	mov	r1, r8
    7458:	e59f3134 	ldr	r3, [pc, #308]	; 7594 <mmc_bwrite+0x1b4>
    745c:	e1a0e00f 	mov	lr, pc
    7460:	e593f000 	ldr	pc, [r3]
    7464:	ea00003d 	b	7560 <mmc_bwrite+0x180>
    7468:	e3550001 	cmp	r5, #1	; 0x1
    746c:	93a03018 	movls	r3, #24	; 0x18
    7470:	91cd30b0 	strhls	r3, [sp]
    7474:	e5943040 	ldr	r3, [r4, #64]
    7478:	83a02019 	movhi	r2, #25	; 0x19
    747c:	81cd20b0 	strhhi	r2, [sp]
    7480:	e3530000 	cmp	r3, #0	; 0x0
    7484:	0594308c 	ldreq	r3, [r4, #140]
    7488:	e3a02015 	mov	r2, #21	; 0x15
    748c:	00030693 	muleq	r3, r3, r6
    7490:	158d6008 	strne	r6, [sp, #8]
    7494:	058d3008 	streq	r3, [sp, #8]
    7498:	e58d2004 	str	r2, [sp, #4]
    749c:	e58d901c 	str	r9, [sp, #28]
    74a0:	e58da020 	str	sl, [sp, #32]
    74a4:	e58d5028 	str	r5, [sp, #40]
    74a8:	e594308c 	ldr	r3, [r4, #140]
    74ac:	e3a02002 	mov	r2, #2	; 0x2
    74b0:	e58d2024 	str	r2, [sp, #36]
    74b4:	e58d302c 	str	r3, [sp, #44]
    74b8:	e1a00004 	mov	r0, r4
    74bc:	e1a0100d 	mov	r1, sp
    74c0:	e28d2020 	add	r2, sp, #32	; 0x20
    74c4:	e1a0e00f 	mov	lr, pc
    74c8:	e594f104 	ldr	pc, [r4, #260]
    74cc:	e3500000 	cmp	r0, #0	; 0x0
    74d0:	0a000004 	beq	74e8 <mmc_bwrite+0x108>
    74d4:	e59f00c0 	ldr	r0, [pc, #192]	; 759c <mmc_bwrite+0x1bc>
    74d8:	e59f30b4 	ldr	r3, [pc, #180]	; 7594 <mmc_bwrite+0x1b4>
    74dc:	e1a0e00f 	mov	lr, pc
    74e0:	e593f000 	ldr	pc, [r3]
    74e4:	ea00001d 	b	7560 <mmc_bwrite+0x180>
    74e8:	e5943050 	ldr	r3, [r4, #80]
    74ec:	e1a03523 	lsr	r3, r3, #10
    74f0:	e2233001 	eor	r3, r3, #1	; 0x1
    74f4:	e3550001 	cmp	r5, #1	; 0x1
    74f8:	93a03000 	movls	r3, #0	; 0x0
    74fc:	82033001 	andhi	r3, r3, #1	; 0x1
    7500:	e3530000 	cmp	r3, #0	; 0x0
    7504:	0a000011 	beq	7550 <mmc_bwrite+0x170>
    7508:	e3a0200c 	mov	r2, #12	; 0xc
    750c:	e3a0301d 	mov	r3, #29	; 0x1d
    7510:	e1cd20b0 	strh	r2, [sp]
    7514:	e98d0208 	stmib	sp, {r3, r9}
    7518:	e58d901c 	str	r9, [sp, #28]
    751c:	e1a00004 	mov	r0, r4
    7520:	e1a0100d 	mov	r1, sp
    7524:	e1a02009 	mov	r2, r9
    7528:	e1a0e00f 	mov	lr, pc
    752c:	e594f104 	ldr	pc, [r4, #260]
    7530:	e3500000 	cmp	r0, #0	; 0x0
    7534:	0a000005 	beq	7550 <mmc_bwrite+0x170>
    7538:	e59f0060 	ldr	r0, [pc, #96]	; 75a0 <mmc_bwrite+0x1c0>
    753c:	e59f3050 	ldr	r3, [pc, #80]	; 7594 <mmc_bwrite+0x1b4>
    7540:	e1a0e00f 	mov	lr, pc
    7544:	e593f000 	ldr	pc, [r3]
    7548:	e1a00009 	mov	r0, r9
    754c:	ea00000d 	b	7588 <mmc_bwrite+0x1a8>
    7550:	e1a00004 	mov	r0, r4
    7554:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    7558:	ebffff6a 	bl	7308 <mmc_send_status>
    755c:	ea000001 	b	7568 <mmc_bwrite+0x188>
    7560:	e3550000 	cmp	r5, #0	; 0x0
    7564:	1a000006 	bne	7584 <mmc_bwrite+0x1a4>
    7568:	e0577005 	subs	r7, r7, r5
    756c:	e594008c 	ldr	r0, [r4, #140]
    7570:	102aa590 	mlane	sl, r0, r5, sl
    7574:	11a06008 	movne	r6, r8
    7578:	1affffac 	bne	7430 <mmc_bwrite+0x50>
    757c:	e000009b 	mul	r0, fp, r0
    7580:	ea000000 	b	7588 <mmc_bwrite+0x1a8>
    7584:	e3a00000 	mov	r0, #0	; 0x0
    7588:	e28dd034 	add	sp, sp, #52	; 0x34
    758c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7590:	000121f7 	.word	0x000121f7
    7594:	0800d704 	.word	0x0800d704
    7598:	00012201 	.word	0x00012201
    759c:	0001222d 	.word	0x0001222d
    75a0:	0001223f 	.word	0x0001223f

000075a4 <mmc2_vs_write>:
    75a4:	e1a024a2 	lsr	r2, r2, #9
    75a8:	e1821b83 	orr	r1, r2, r3, lsl #23
    75ac:	e59d2000 	ldr	r2, [sp]
    75b0:	e1a03000 	mov	r3, r0
    75b4:	e1a024c2 	asr	r2, r2, #9
    75b8:	e3a00002 	mov	r0, #2	; 0x2
    75bc:	eaffff87 	b	73e0 <mmc_bwrite>

000075c0 <mmc1_vs_write>:
    75c0:	e1a024a2 	lsr	r2, r2, #9
    75c4:	e1821b83 	orr	r1, r2, r3, lsl #23
    75c8:	e59d2000 	ldr	r2, [sp]
    75cc:	e1a03000 	mov	r3, r0
    75d0:	e1a024c2 	asr	r2, r2, #9
    75d4:	e3a00001 	mov	r0, #1	; 0x1
    75d8:	eaffff80 	b	73e0 <mmc_bwrite>

000075dc <mmc0_vs_write>:
    75dc:	e1a024a2 	lsr	r2, r2, #9
    75e0:	e1821b83 	orr	r1, r2, r3, lsl #23
    75e4:	e59d2000 	ldr	r2, [sp]
    75e8:	e1a03000 	mov	r3, r0
    75ec:	e1a024c2 	asr	r2, r2, #9
    75f0:	e3a00000 	mov	r0, #0	; 0x0
    75f4:	eaffff79 	b	73e0 <mmc_bwrite>

000075f8 <mmc_switch>:
    75f8:	e92d4030 	push	{r4, r5, lr}
    75fc:	e1a02802 	lsl	r2, r2, #16
    7600:	e3822403 	orr	r2, r2, #50331648	; 0x3000000
    7604:	e24dd024 	sub	sp, sp, #36	; 0x24
    7608:	e1822403 	orr	r2, r2, r3, lsl #8
    760c:	e3a0301d 	mov	r3, #29	; 0x1d
    7610:	e3a01000 	mov	r1, #0	; 0x0
    7614:	e58d3004 	str	r3, [sp, #4]
    7618:	e3a03006 	mov	r3, #6	; 0x6
    761c:	e58d2008 	str	r2, [sp, #8]
    7620:	e1cd30b0 	strh	r3, [sp]
    7624:	e1a02001 	mov	r2, r1
    7628:	e58d101c 	str	r1, [sp, #28]
    762c:	e1a0100d 	mov	r1, sp
    7630:	e1a04000 	mov	r4, r0
    7634:	e1a0e00f 	mov	lr, pc
    7638:	e590f104 	ldr	pc, [r0, #260]
    763c:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    7640:	e1a05000 	mov	r5, r0
    7644:	e1a00004 	mov	r0, r4
    7648:	ebffff2e 	bl	7308 <mmc_send_status>
    764c:	e1a00005 	mov	r0, r5
    7650:	e28dd024 	add	sp, sp, #36	; 0x24
    7654:	e8bd8030 	pop	{r4, r5, pc}

00007658 <mmc_change_freq>:
    7658:	e92d4070 	push	{r4, r5, r6, lr}
    765c:	e5903050 	ldr	r3, [r0, #80]
    7660:	e24ddc02 	sub	sp, sp, #512	; 0x200
    7664:	e3130b01 	tst	r3, #1024	; 0x400
    7668:	e3a03000 	mov	r3, #0	; 0x0
    766c:	e1a04000 	mov	r4, r0
    7670:	e580304c 	str	r3, [r0, #76]
    7674:	1a000020 	bne	76fc <mmc_change_freq+0xa4>
    7678:	e5902030 	ldr	r2, [r0, #48]
    767c:	e59f3084 	ldr	r3, [pc, #132]	; 7708 <mmc_change_freq+0xb0>
    7680:	e1520003 	cmp	r2, r3
    7684:	9a00001c 	bls	76fc <mmc_change_freq+0xa4>
    7688:	e3a03c01 	mov	r3, #256	; 0x100
    768c:	e580304c 	str	r3, [r0, #76]
    7690:	e1a0100d 	mov	r1, sp
    7694:	ebfffd12 	bl	6ae4 <mmc_send_ext_csd>
    7698:	e3500000 	cmp	r0, #0	; 0x0
    769c:	e1a0600d 	mov	r6, sp
    76a0:	1a000016 	bne	7700 <mmc_change_freq+0xa8>
    76a4:	e3a01001 	mov	r1, #1	; 0x1
    76a8:	e1a00004 	mov	r0, r4
    76ac:	e3a020b9 	mov	r2, #185	; 0xb9
    76b0:	e1a03001 	mov	r3, r1
    76b4:	e5dd50c4 	ldrb	r5, [sp, #196]
    76b8:	ebffffce 	bl	75f8 <mmc_switch>
    76bc:	e3500000 	cmp	r0, #0	; 0x0
    76c0:	1a00000e 	bne	7700 <mmc_change_freq+0xa8>
    76c4:	e1a0100d 	mov	r1, sp
    76c8:	e1a00004 	mov	r0, r4
    76cc:	ebfffd04 	bl	6ae4 <mmc_send_ext_csd>
    76d0:	e3500000 	cmp	r0, #0	; 0x0
    76d4:	1a000009 	bne	7700 <mmc_change_freq+0xa8>
    76d8:	e5dd30b9 	ldrb	r3, [sp, #185]
    76dc:	e3530000 	cmp	r3, #0	; 0x0
    76e0:	0a000006 	beq	7700 <mmc_change_freq+0xa8>
    76e4:	e594304c 	ldr	r3, [r4, #76]
    76e8:	e3150002 	tst	r5, #2	; 0x2
    76ec:	13833011 	orrne	r3, r3, #17	; 0x11
    76f0:	03833001 	orreq	r3, r3, #1	; 0x1
    76f4:	e584304c 	str	r3, [r4, #76]
    76f8:	ea000000 	b	7700 <mmc_change_freq+0xa8>
    76fc:	e3a00000 	mov	r0, #0	; 0x0
    7700:	e28ddc02 	add	sp, sp, #512	; 0x200
    7704:	e8bd8070 	pop	{r4, r5, r6, pc}
    7708:	0001003f 	.word	0x0001003f

0000770c <mmc_startup>:
    770c:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    7710:	e5903050 	ldr	r3, [r0, #80]
    7714:	e24ddf8e 	sub	sp, sp, #568	; 0x238
    7718:	e3130b01 	tst	r3, #1024	; 0x400
    771c:	13a0300a 	movne	r3, #10	; 0xa
    7720:	03a03002 	moveq	r3, #2	; 0x2
    7724:	e28d1c02 	add	r1, sp, #512	; 0x200
    7728:	e3a02000 	mov	r2, #0	; 0x0
    772c:	e1c131b0 	strh	r3, [r1, #16]
    7730:	e28d5e21 	add	r5, sp, #528	; 0x210
    7734:	e3a03007 	mov	r3, #7	; 0x7
    7738:	e58d3214 	str	r3, [sp, #532]
    773c:	e58d2218 	str	r2, [sp, #536]
    7740:	e58d222c 	str	r2, [sp, #556]
    7744:	e1a01005 	mov	r1, r5
    7748:	e1a06000 	mov	r6, r0
    774c:	e1a0e00f 	mov	lr, pc
    7750:	e590f104 	ldr	pc, [r0, #260]
    7754:	e2504000 	subs	r4, r0, #0	; 0x0
    7758:	1a000159 	bne	7cc4 <mmc_startup+0x5b8>
    775c:	e3a02010 	mov	r2, #16	; 0x10
    7760:	e2860070 	add	r0, r6, #112	; 0x70
    7764:	e285100c 	add	r1, r5, #12	; 0xc
    7768:	ebffe71a 	bl	13d8 <memcpy>
    776c:	e5963050 	ldr	r3, [r6, #80]
    7770:	e2132b01 	ands	r2, r3, #1024	; 0x400
    7774:	1a000013 	bne	77c8 <mmc_startup+0xbc>
    7778:	e1d638b0 	ldrh	r3, [r6, #128]
    777c:	e3a00003 	mov	r0, #3	; 0x3
    7780:	e1a03803 	lsl	r3, r3, #16
    7784:	e58d3218 	str	r3, [sp, #536]
    7788:	e3a03015 	mov	r3, #21	; 0x15
    778c:	e58d3214 	str	r3, [sp, #532]
    7790:	e28d3c02 	add	r3, sp, #512	; 0x200
    7794:	e1c301b0 	strh	r0, [r3, #16]
    7798:	e58d222c 	str	r2, [sp, #556]
    779c:	e1a01005 	mov	r1, r5
    77a0:	e1a00006 	mov	r0, r6
    77a4:	e1a0e00f 	mov	lr, pc
    77a8:	e596f104 	ldr	pc, [r6, #260]
    77ac:	e2504000 	subs	r4, r0, #0	; 0x0
    77b0:	1a000143 	bne	7cc4 <mmc_startup+0x5b8>
    77b4:	e5963030 	ldr	r3, [r6, #48]
    77b8:	e3130802 	tst	r3, #131072	; 0x20000
    77bc:	128d1c02 	addne	r1, sp, #512	; 0x200
    77c0:	11d111be 	ldrhne	r1, [r1, #30]
    77c4:	11c618b0 	strhne	r1, [r6, #128]
    77c8:	e1d638b0 	ldrh	r3, [r6, #128]
    77cc:	e3a01000 	mov	r1, #0	; 0x0
    77d0:	e1a03803 	lsl	r3, r3, #16
    77d4:	e3a02007 	mov	r2, #7	; 0x7
    77d8:	e58d3218 	str	r3, [sp, #536]
    77dc:	e3a00009 	mov	r0, #9	; 0x9
    77e0:	e28d3c02 	add	r3, sp, #512	; 0x200
    77e4:	e1c301b0 	strh	r0, [r3, #16]
    77e8:	e58d2214 	str	r2, [sp, #532]
    77ec:	e58d122c 	str	r1, [sp, #556]
    77f0:	e1a02001 	mov	r2, r1
    77f4:	e1a00006 	mov	r0, r6
    77f8:	e28d1e21 	add	r1, sp, #528	; 0x210
    77fc:	e1a0e00f 	mov	lr, pc
    7800:	e596f104 	ldr	pc, [r6, #260]
    7804:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    7808:	e1a04000 	mov	r4, r0
    780c:	e1a00006 	mov	r0, r6
    7810:	ebfffebc 	bl	7308 <mmc_send_status>
    7814:	e3540000 	cmp	r4, #0	; 0x0
    7818:	1a000129 	bne	7cc4 <mmc_startup+0x5b8>
    781c:	e5963030 	ldr	r3, [r6, #48]
    7820:	e59d021c 	ldr	r0, [sp, #540]
    7824:	e59d2224 	ldr	r2, [sp, #548]
    7828:	e59d1228 	ldr	r1, [sp, #552]
    782c:	e3530801 	cmp	r3, #65536	; 0x10000
    7830:	e59d3220 	ldr	r3, [sp, #544]
    7834:	e5862068 	str	r2, [r6, #104]
    7838:	e5863064 	str	r3, [r6, #100]
    783c:	e586106c 	str	r1, [r6, #108]
    7840:	e5860060 	str	r0, [r6, #96]
    7844:	1a000013 	bne	7898 <mmc_startup+0x18c>
    7848:	e1a03d20 	lsr	r3, r0, #26
    784c:	e203300f 	and	r3, r3, #15	; 0xf
    7850:	e3530004 	cmp	r3, #4	; 0x4
    7854:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    7858:	ea00000c 	b	7890 <mmc_startup+0x184>
    785c:	00007890 	.word	0x00007890
    7860:	00007870 	.word	0x00007870
    7864:	00007878 	.word	0x00007878
    7868:	00007880 	.word	0x00007880
    786c:	00007888 	.word	0x00007888
    7870:	e59f3458 	ldr	r3, [pc, #1112]	; 7cd0 <mmc_startup+0x5c4>
    7874:	ea000006 	b	7894 <mmc_startup+0x188>
    7878:	e59f3454 	ldr	r3, [pc, #1108]	; 7cd4 <mmc_startup+0x5c8>
    787c:	ea000004 	b	7894 <mmc_startup+0x188>
    7880:	e59f3450 	ldr	r3, [pc, #1104]	; 7cd8 <mmc_startup+0x5cc>
    7884:	ea000002 	b	7894 <mmc_startup+0x188>
    7888:	e59f344c 	ldr	r3, [pc, #1100]	; 7cdc <mmc_startup+0x5d0>
    788c:	ea000000 	b	7894 <mmc_startup+0x188>
    7890:	e59f3448 	ldr	r3, [pc, #1096]	; 7ce0 <mmc_startup+0x5d4>
    7894:	e5863030 	str	r3, [r6, #48]
    7898:	e28d1c02 	add	r1, sp, #512	; 0x200
    789c:	e1d132b2 	ldrh	r3, [r1, #34]
    78a0:	e3a0e001 	mov	lr, #1	; 0x1
    78a4:	e203300f 	and	r3, r3, #15	; 0xf
    78a8:	e1a0c31e 	lsl	ip, lr, r3
    78ac:	e59d121c 	ldr	r1, [sp, #540]
    78b0:	e59f242c 	ldr	r2, [pc, #1068]	; 7ce4 <mmc_startup+0x5d8>
    78b4:	e1a031a1 	lsr	r3, r1, #3
    78b8:	e203300f 	and	r3, r3, #15	; 0xf
    78bc:	e7920103 	ldr	r0, [r2, r3, lsl #2]
    78c0:	e59f3420 	ldr	r3, [pc, #1056]	; 7ce8 <mmc_startup+0x5dc>
    78c4:	e2011007 	and	r1, r1, #7	; 0x7
    78c8:	e7933101 	ldr	r3, [r3, r1, lsl #2]
    78cc:	e5962030 	ldr	r2, [r6, #48]
    78d0:	e0030093 	mul	r3, r3, r0
    78d4:	e3120802 	tst	r2, #131072	; 0x20000
    78d8:	e5863084 	str	r3, [r6, #132]
    78dc:	e586c088 	str	ip, [r6, #136]
    78e0:	059d3228 	ldreq	r3, [sp, #552]
    78e4:	1586c08c 	strne	ip, [r6, #140]
    78e8:	01a03b23 	lsreq	r3, r3, #22
    78ec:	0203300f 	andeq	r3, r3, #15	; 0xf
    78f0:	01a0331e 	lsleq	r3, lr, r3
    78f4:	0586308c 	streq	r3, [r6, #140]
    78f8:	e5963040 	ldr	r3, [r6, #64]
    78fc:	e5962064 	ldr	r2, [r6, #100]
    7900:	e3530000 	cmp	r3, #0	; 0x0
    7904:	e5960068 	ldr	r0, [r6, #104]
    7908:	0a000007 	beq	792c <mmc_startup+0x220>
    790c:	e202203f 	and	r2, r2, #63	; 0x3f
    7910:	e1a03820 	lsr	r3, r0, #16
    7914:	e1833802 	orr	r3, r3, r2, lsl #16
    7918:	e1a08003 	mov	r8, r3
    791c:	e3a09000 	mov	r9, #0	; 0x0
    7920:	e3a04008 	mov	r4, #8	; 0x8
    7924:	e3a05000 	mov	r5, #0	; 0x0
    7928:	ea000009 	b	7954 <mmc_startup+0x248>
    792c:	e1a03b02 	lsl	r3, r2, #22
    7930:	e1a03b23 	lsr	r3, r3, #22
    7934:	e200190e 	and	r1, r0, #229376	; 0x38000
    7938:	e1a02f20 	lsr	r2, r0, #30
    793c:	e1822103 	orr	r2, r2, r3, lsl #2
    7940:	e1a017a1 	lsr	r1, r1, #15
    7944:	e1a08002 	mov	r8, r2
    7948:	e3a09000 	mov	r9, #0	; 0x0
    794c:	e1a04001 	mov	r4, r1
    7950:	e3a05000 	mov	r5, #0	; 0x0
    7954:	e3a02001 	mov	r2, #1	; 0x1
    7958:	e0980002 	adds	r0, r8, r2
    795c:	e3a03000 	mov	r3, #0	; 0x0
    7960:	e0a91003 	adc	r1, r9, r3
    7964:	e2842002 	add	r2, r4, #2	; 0x2
    7968:	eb001ef1 	bl	f534 <__aeabi_llsl>
    796c:	e596c088 	ldr	ip, [r6, #136]
    7970:	e0832c90 	umull	r2, r3, r0, ip
    7974:	e023319c 	mla	r3, ip, r1, r3
    7978:	e35c0c02 	cmp	ip, #512	; 0x200
    797c:	e1c629f0 	strd	r2, [r6, #144]
    7980:	83a03c02 	movhi	r3, #512	; 0x200
    7984:	85863088 	strhi	r3, [r6, #136]
    7988:	e596308c 	ldr	r3, [r6, #140]
    798c:	e3530c02 	cmp	r3, #512	; 0x200
    7990:	83a03c02 	movhi	r3, #512	; 0x200
    7994:	8586308c 	strhi	r3, [r6, #140]
    7998:	e5963050 	ldr	r3, [r6, #80]
    799c:	e2131b01 	ands	r1, r3, #1024	; 0x400
    79a0:	1a00000f 	bne	79e4 <mmc_startup+0x2d8>
    79a4:	e1d638b0 	ldrh	r3, [r6, #128]
    79a8:	e3a0201d 	mov	r2, #29	; 0x1d
    79ac:	e1a03803 	lsl	r3, r3, #16
    79b0:	e58d2214 	str	r2, [sp, #532]
    79b4:	e58d3218 	str	r3, [sp, #536]
    79b8:	e28d2c02 	add	r2, sp, #512	; 0x200
    79bc:	e3a03007 	mov	r3, #7	; 0x7
    79c0:	e1c231b0 	strh	r3, [r2, #16]
    79c4:	e58d122c 	str	r1, [sp, #556]
    79c8:	e1a02001 	mov	r2, r1
    79cc:	e1a00006 	mov	r0, r6
    79d0:	e28d1e21 	add	r1, sp, #528	; 0x210
    79d4:	e1a0e00f 	mov	lr, pc
    79d8:	e596f104 	ldr	pc, [r6, #260]
    79dc:	e2504000 	subs	r4, r0, #0	; 0x0
    79e0:	1a0000b7 	bne	7cc4 <mmc_startup+0x5b8>
    79e4:	e5962030 	ldr	r2, [r6, #48]
    79e8:	e3e03000 	mvn	r3, #0	; 0x0
    79ec:	e3120802 	tst	r2, #131072	; 0x20000
    79f0:	e5c63082 	strb	r3, [r6, #130]
    79f4:	1a00001e 	bne	7a74 <mmc_startup+0x368>
    79f8:	e59f32ec 	ldr	r3, [pc, #748]	; 7cec <mmc_startup+0x5e0>
    79fc:	e1520003 	cmp	r2, r3
    7a00:	9a00001b 	bls	7a74 <mmc_startup+0x368>
    7a04:	e28d1010 	add	r1, sp, #16	; 0x10
    7a08:	e1a00006 	mov	r0, r6
    7a0c:	ebfffc34 	bl	6ae4 <mmc_send_ext_csd>
    7a10:	e5dd30d0 	ldrb	r3, [sp, #208]
    7a14:	e2701001 	rsbs	r1, r0, #1	; 0x1
    7a18:	33a01000 	movcc	r1, #0	; 0x0
    7a1c:	e3530001 	cmp	r3, #1	; 0x1
    7a20:	93a01000 	movls	r1, #0	; 0x0
    7a24:	e3510000 	cmp	r1, #0	; 0x0
    7a28:	0a00000d 	beq	7a64 <mmc_startup+0x358>
    7a2c:	e5dd20e6 	ldrb	r2, [sp, #230]
    7a30:	e5dd10e5 	ldrb	r1, [sp, #229]
    7a34:	e5dd00e4 	ldrb	r0, [sp, #228]
    7a38:	e1a02802 	lsl	r2, r2, #16
    7a3c:	e1822401 	orr	r2, r2, r1, lsl #8
    7a40:	e5dd10e7 	ldrb	r1, [sp, #231]
    7a44:	e1822000 	orr	r2, r2, r0
    7a48:	e1822c01 	orr	r2, r2, r1, lsl #24
    7a4c:	e1a03fc2 	asr	r3, r2, #31
    7a50:	e1a01483 	lsl	r1, r3, #9
    7a54:	e1811ba2 	orr	r1, r1, r2, lsr #23
    7a58:	e1a02482 	lsl	r2, r2, #9
    7a5c:	e5861094 	str	r1, [r6, #148]
    7a60:	e5862090 	str	r2, [r6, #144]
    7a64:	e5dd30b0 	ldrb	r3, [sp, #176]
    7a68:	e3130001 	tst	r3, #1	; 0x1
    7a6c:	15dd30c3 	ldrbne	r3, [sp, #195]
    7a70:	15c63082 	strbne	r3, [r6, #130]
    7a74:	e5963030 	ldr	r3, [r6, #48]
    7a78:	e3130802 	tst	r3, #131072	; 0x20000
    7a7c:	0a000002 	beq	7a8c <mmc_startup+0x380>
    7a80:	e1a00006 	mov	r0, r6
    7a84:	ebfffce8 	bl	6e2c <sd_change_freq>
    7a88:	ea000001 	b	7a94 <mmc_startup+0x388>
    7a8c:	e1a00006 	mov	r0, r6
    7a90:	ebfffef0 	bl	7658 <mmc_change_freq>
    7a94:	e3500000 	cmp	r0, #0	; 0x0
    7a98:	e1a05000 	mov	r5, r0
    7a9c:	11a04000 	movne	r4, r0
    7aa0:	1a000087 	bne	7cc4 <mmc_startup+0x5b8>
    7aa4:	e596304c 	ldr	r3, [r6, #76]
    7aa8:	e5962050 	ldr	r2, [r6, #80]
    7aac:	e5961030 	ldr	r1, [r6, #48]
    7ab0:	e0022003 	and	r2, r2, r3
    7ab4:	e3110802 	tst	r1, #131072	; 0x20000
    7ab8:	e586204c 	str	r2, [r6, #76]
    7abc:	e2023c01 	and	r3, r2, #256	; 0x100
    7ac0:	0a00002c 	beq	7b78 <mmc_startup+0x46c>
    7ac4:	e3530000 	cmp	r3, #0	; 0x0
    7ac8:	0a000023 	beq	7b5c <mmc_startup+0x450>
    7acc:	e1d638b0 	ldrh	r3, [r6, #128]
    7ad0:	e28d2c02 	add	r2, sp, #512	; 0x200
    7ad4:	e1a03803 	lsl	r3, r3, #16
    7ad8:	e58d3218 	str	r3, [sp, #536]
    7adc:	e3a08015 	mov	r8, #21	; 0x15
    7ae0:	e3a03037 	mov	r3, #55	; 0x37
    7ae4:	e28d9e21 	add	r9, sp, #528	; 0x210
    7ae8:	e1c231b0 	strh	r3, [r2, #16]
    7aec:	e58d022c 	str	r0, [sp, #556]
    7af0:	e58d8214 	str	r8, [sp, #532]
    7af4:	e1a00006 	mov	r0, r6
    7af8:	e1a01009 	mov	r1, r9
    7afc:	e1a02005 	mov	r2, r5
    7b00:	e1a0e00f 	mov	lr, pc
    7b04:	e596f104 	ldr	pc, [r6, #260]
    7b08:	e2504000 	subs	r4, r0, #0	; 0x0
    7b0c:	1a00006c 	bne	7cc4 <mmc_startup+0x5b8>
    7b10:	e3a03002 	mov	r3, #2	; 0x2
    7b14:	e3a01006 	mov	r1, #6	; 0x6
    7b18:	e28d0c02 	add	r0, sp, #512	; 0x200
    7b1c:	e1c011b0 	strh	r1, [r0, #16]
    7b20:	e58d8214 	str	r8, [sp, #532]
    7b24:	e58d3218 	str	r3, [sp, #536]
    7b28:	e58d522c 	str	r5, [sp, #556]
    7b2c:	e1a01009 	mov	r1, r9
    7b30:	e1a02005 	mov	r2, r5
    7b34:	e1a00006 	mov	r0, r6
    7b38:	e1a0e00f 	mov	lr, pc
    7b3c:	e596f104 	ldr	pc, [r6, #260]
    7b40:	e2504000 	subs	r4, r0, #0	; 0x0
    7b44:	1a00005e 	bne	7cc4 <mmc_startup+0x5b8>
    7b48:	e3a03004 	mov	r3, #4	; 0x4
    7b4c:	e5863044 	str	r3, [r6, #68]
    7b50:	e1a00006 	mov	r0, r6
    7b54:	e1a0e00f 	mov	lr, pc
    7b58:	e596f108 	ldr	pc, [r6, #264]
    7b5c:	e596304c 	ldr	r3, [r6, #76]
    7b60:	e3130001 	tst	r3, #1	; 0x1
    7b64:	11a00006 	movne	r0, r6
    7b68:	159f1180 	ldrne	r1, [pc, #384]	; 7cf0 <mmc_startup+0x5e4>
    7b6c:	01a00006 	moveq	r0, r6
    7b70:	059f117c 	ldreq	r1, [pc, #380]	; 7cf4 <mmc_startup+0x5e8>
    7b74:	ea000022 	b	7c04 <mmc_startup+0x4f8>
    7b78:	e3530000 	cmp	r3, #0	; 0x0
    7b7c:	0a000008 	beq	7ba4 <mmc_startup+0x498>
    7b80:	e3a01001 	mov	r1, #1	; 0x1
    7b84:	e1a03001 	mov	r3, r1
    7b88:	e1a00006 	mov	r0, r6
    7b8c:	e3a020b7 	mov	r2, #183	; 0xb7
    7b90:	ebfffe98 	bl	75f8 <mmc_switch>
    7b94:	e2504000 	subs	r4, r0, #0	; 0x0
    7b98:	03a03004 	moveq	r3, #4	; 0x4
    7b9c:	1a000048 	bne	7cc4 <mmc_startup+0x5b8>
    7ba0:	ea000009 	b	7bcc <mmc_startup+0x4c0>
    7ba4:	e3120c02 	tst	r2, #512	; 0x200
    7ba8:	0a00000b 	beq	7bdc <mmc_startup+0x4d0>
    7bac:	e1a00006 	mov	r0, r6
    7bb0:	e3a01001 	mov	r1, #1	; 0x1
    7bb4:	e3a020b7 	mov	r2, #183	; 0xb7
    7bb8:	e3a03002 	mov	r3, #2	; 0x2
    7bbc:	ebfffe8d 	bl	75f8 <mmc_switch>
    7bc0:	e2504000 	subs	r4, r0, #0	; 0x0
    7bc4:	1a00003e 	bne	7cc4 <mmc_startup+0x5b8>
    7bc8:	e3a03008 	mov	r3, #8	; 0x8
    7bcc:	e5863044 	str	r3, [r6, #68]
    7bd0:	e1a00006 	mov	r0, r6
    7bd4:	e1a0e00f 	mov	lr, pc
    7bd8:	e596f108 	ldr	pc, [r6, #264]
    7bdc:	e596304c 	ldr	r3, [r6, #76]
    7be0:	e3130001 	tst	r3, #1	; 0x1
    7be4:	01a00006 	moveq	r0, r6
    7be8:	059f1108 	ldreq	r1, [pc, #264]	; 7cf8 <mmc_startup+0x5ec>
    7bec:	0a000004 	beq	7c04 <mmc_startup+0x4f8>
    7bf0:	e3130010 	tst	r3, #16	; 0x10
    7bf4:	159f1100 	ldrne	r1, [pc, #256]	; 7cfc <mmc_startup+0x5f0>
    7bf8:	059f1100 	ldreq	r1, [pc, #256]	; 7d00 <mmc_startup+0x5f4>
    7bfc:	11a00006 	movne	r0, r6
    7c00:	01a00006 	moveq	r0, r6
    7c04:	ebfffbed 	bl	6bc0 <mmc_set_clock>
    7c08:	e5964094 	ldr	r4, [r6, #148]
    7c0c:	e5960090 	ldr	r0, [r6, #144]
    7c10:	e5961088 	ldr	r1, [r6, #136]
    7c14:	e3a03000 	mov	r3, #0	; 0x0
    7c18:	e3540000 	cmp	r4, #0	; 0x0
    7c1c:	e5c630a3 	strb	r3, [r6, #163]
    7c20:	e5c630a2 	strb	r3, [r6, #162]
    7c24:	e58610ac 	str	r1, [r6, #172]
    7c28:	e58d0230 	str	r0, [sp, #560]
    7c2c:	e58d4234 	str	r4, [sp, #564]
    7c30:	1a000003 	bne	7c44 <mmc_startup+0x538>
    7c34:	eb001d27 	bl	f0d8 <__aeabi_uidiv>
    7c38:	e58d4234 	str	r4, [sp, #564]
    7c3c:	e58d0230 	str	r0, [sp, #560]
    7c40:	ea000001 	b	7c4c <mmc_startup+0x540>
    7c44:	e28d0e23 	add	r0, sp, #560	; 0x230
    7c48:	eb0010e1 	bl	bfd4 <__div64_32>
    7c4c:	e5d6007f 	ldrb	r0, [r6, #127]
    7c50:	e59d1230 	ldr	r1, [sp, #560]
    7c54:	e5962070 	ldr	r2, [r6, #112]
    7c58:	e5963078 	ldr	r3, [r6, #120]
    7c5c:	e58610a8 	str	r1, [r6, #168]
    7c60:	e1803403 	orr	r3, r0, r3, lsl #8
    7c64:	e1a02422 	lsr	r2, r2, #8
    7c68:	e59f1094 	ldr	r1, [pc, #148]	; 7d04 <mmc_startup+0x5f8>
    7c6c:	e28600b0 	add	r0, r6, #176	; 0xb0
    7c70:	eb0014a0 	bl	cef8 <sprintf>
    7c74:	e596c074 	ldr	ip, [r6, #116]
    7c78:	e5d62070 	ldrb	r2, [r6, #112]
    7c7c:	e1a0482c 	lsr	r4, ip, #16
    7c80:	e1a0542c 	lsr	r5, ip, #8
    7c84:	e1a03c2c 	lsr	r3, ip, #24
    7c88:	e20440ff 	and	r4, r4, #255	; 0xff
    7c8c:	e20cc0ff 	and	ip, ip, #255	; 0xff
    7c90:	e59f1070 	ldr	r1, [pc, #112]	; 7d08 <mmc_startup+0x5fc>
    7c94:	e20550ff 	and	r5, r5, #255	; 0xff
    7c98:	e28600d9 	add	r0, r6, #217	; 0xd9
    7c9c:	e88d1030 	stm	sp, {r4, r5, ip}
    7ca0:	eb001494 	bl	cef8 <sprintf>
    7ca4:	e5962078 	ldr	r2, [r6, #120]
    7ca8:	e28600ee 	add	r0, r6, #238	; 0xee
    7cac:	e1a03c22 	lsr	r3, r2, #24
    7cb0:	e203300f 	and	r3, r3, #15	; 0xf
    7cb4:	e1a02e22 	lsr	r2, r2, #28
    7cb8:	e59f104c 	ldr	r1, [pc, #76]	; 7d0c <mmc_startup+0x600>
    7cbc:	eb00148d 	bl	cef8 <sprintf>
    7cc0:	e3a04000 	mov	r4, #0	; 0x0
    7cc4:	e1a00004 	mov	r0, r4
    7cc8:	e28ddf8e 	add	sp, sp, #568	; 0x238
    7ccc:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    7cd0:	00010014 	.word	0x00010014
    7cd4:	00010022 	.word	0x00010022
    7cd8:	00010030 	.word	0x00010030
    7cdc:	00010040 	.word	0x00010040
    7ce0:	00010012 	.word	0x00010012
    7ce4:	00010574 	.word	0x00010574
    7ce8:	00010564 	.word	0x00010564
    7cec:	0001003f 	.word	0x0001003f
    7cf0:	02faf080 	.word	0x02faf080
    7cf4:	017d7840 	.word	0x017d7840
    7cf8:	01312d00 	.word	0x01312d00
    7cfc:	03197500 	.word	0x03197500
    7d00:	018cba80 	.word	0x018cba80
    7d04:	0001225a 	.word	0x0001225a
    7d08:	0001226c 	.word	0x0001226c
    7d0c:	00012277 	.word	0x00012277

00007d10 <mmc_init>:
    7d10:	e92d4010 	push	{r4, lr}
    7d14:	ebfffb5a 	bl	6a84 <find_mmc_device>
    7d18:	e1a04000 	mov	r4, r0
    7d1c:	e1a0e00f 	mov	lr, pc
    7d20:	e590f10c 	ldr	pc, [r0, #268]
    7d24:	e3500000 	cmp	r0, #0	; 0x0
    7d28:	18bd8010 	popne	{r4, pc}
    7d2c:	e3a03001 	mov	r3, #1	; 0x1
    7d30:	e5843044 	str	r3, [r4, #68]
    7d34:	e1a00004 	mov	r0, r4
    7d38:	e1a0e00f 	mov	lr, pc
    7d3c:	e594f108 	ldr	pc, [r4, #264]
    7d40:	e1a00004 	mov	r0, r4
    7d44:	e59f1060 	ldr	r1, [pc, #96]	; 7dac <mmc_init+0x9c>
    7d48:	ebfffb9c 	bl	6bc0 <mmc_set_clock>
    7d4c:	e1a00004 	mov	r0, r4
    7d50:	ebfffd09 	bl	717c <mmc_go_idle>
    7d54:	e3500000 	cmp	r0, #0	; 0x0
    7d58:	18bd8010 	popne	{r4, pc}
    7d5c:	e5c40083 	strb	r0, [r4, #131]
    7d60:	e1a00004 	mov	r0, r4
    7d64:	ebfffba9 	bl	6c10 <mmc_send_if_cond>
    7d68:	e1a00004 	mov	r0, r4
    7d6c:	ebfffcad 	bl	7028 <sd_send_op_cond>
    7d70:	e3700013 	cmn	r0, #19	; 0x13
    7d74:	1a000009 	bne	7da0 <mmc_init+0x90>
    7d78:	e1a00004 	mov	r0, r4
    7d7c:	ebfffd13 	bl	71d0 <mmc_send_op_cond>
    7d80:	e3500000 	cmp	r0, #0	; 0x0
    7d84:	0a000005 	beq	7da0 <mmc_init+0x90>
    7d88:	e59f0020 	ldr	r0, [pc, #32]	; 7db0 <mmc_init+0xa0>
    7d8c:	e59f3020 	ldr	r3, [pc, #32]	; 7db4 <mmc_init+0xa4>
    7d90:	e1a0e00f 	mov	lr, pc
    7d94:	e593f000 	ldr	pc, [r3]
    7d98:	e3e00010 	mvn	r0, #16	; 0x10
    7d9c:	e8bd8010 	pop	{r4, pc}
    7da0:	e1a00004 	mov	r0, r4
    7da4:	e8bd4010 	pop	{r4, lr}
    7da8:	eafffe57 	b	770c <mmc_startup>
    7dac:	00061a80 	.word	0x00061a80
    7db0:	0001227d 	.word	0x0001227d
    7db4:	0800d704 	.word	0x0800d704

00007db8 <mmc2_vs_reset>:
    7db8:	e3a00002 	mov	r0, #2	; 0x2
    7dbc:	eaffffd3 	b	7d10 <mmc_init>

00007dc0 <mmc1_vs_reset>:
    7dc0:	e3a00001 	mov	r0, #1	; 0x1
    7dc4:	eaffffd1 	b	7d10 <mmc_init>

00007dc8 <mmc0_vs_reset>:
    7dc8:	e3a00000 	mov	r0, #0	; 0x0
    7dcc:	eaffffcf 	b	7d10 <mmc_init>

00007dd0 <mmc_read_blocks>:
    7dd0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7dd4:	e3530001 	cmp	r3, #1	; 0x1
    7dd8:	e24dd030 	sub	sp, sp, #48	; 0x30
    7ddc:	e1a06003 	mov	r6, r3
    7de0:	83a03012 	movhi	r3, #18	; 0x12
    7de4:	93a03011 	movls	r3, #17	; 0x11
    7de8:	e1cd30b0 	strh	r3, [sp]
    7dec:	e5903040 	ldr	r3, [r0, #64]
    7df0:	e3a07000 	mov	r7, #0	; 0x0
    7df4:	e3530000 	cmp	r3, #0	; 0x0
    7df8:	05903088 	ldreq	r3, [r0, #136]
    7dfc:	158d2008 	strne	r2, [sp, #8]
    7e00:	00030293 	muleq	r3, r3, r2
    7e04:	058d3008 	streq	r3, [sp, #8]
    7e08:	e5903088 	ldr	r3, [r0, #136]
    7e0c:	e3a02015 	mov	r2, #21	; 0x15
    7e10:	e58d302c 	str	r3, [sp, #44]
    7e14:	e3a03001 	mov	r3, #1	; 0x1
    7e18:	e58d2004 	str	r2, [sp, #4]
    7e1c:	e58d1020 	str	r1, [sp, #32]
    7e20:	e1a04000 	mov	r4, r0
    7e24:	e58d3024 	str	r3, [sp, #36]
    7e28:	e58d701c 	str	r7, [sp, #28]
    7e2c:	e58d6028 	str	r6, [sp, #40]
    7e30:	e1a0100d 	mov	r1, sp
    7e34:	e28d2020 	add	r2, sp, #32	; 0x20
    7e38:	e1a0e00f 	mov	lr, pc
    7e3c:	e594f104 	ldr	pc, [r4, #260]
    7e40:	e2505000 	subs	r5, r0, #0	; 0x0
    7e44:	e1a0800d 	mov	r8, sp
    7e48:	11a00007 	movne	r0, r7
    7e4c:	1a000018 	bne	7eb4 <mmc_read_blocks+0xe4>
    7e50:	e3560001 	cmp	r6, #1	; 0x1
    7e54:	9a000012 	bls	7ea4 <mmc_read_blocks+0xd4>
    7e58:	e3a0301d 	mov	r3, #29	; 0x1d
    7e5c:	e58d3004 	str	r3, [sp, #4]
    7e60:	e3a0300c 	mov	r3, #12	; 0xc
    7e64:	e1cd30b0 	strh	r3, [sp]
    7e68:	e58d5008 	str	r5, [sp, #8]
    7e6c:	e58d501c 	str	r5, [sp, #28]
    7e70:	e1a0100d 	mov	r1, sp
    7e74:	e1a00004 	mov	r0, r4
    7e78:	e1a02005 	mov	r2, r5
    7e7c:	e1a0e00f 	mov	lr, pc
    7e80:	e594f104 	ldr	pc, [r4, #260]
    7e84:	e3500000 	cmp	r0, #0	; 0x0
    7e88:	0a000005 	beq	7ea4 <mmc_read_blocks+0xd4>
    7e8c:	e59f0028 	ldr	r0, [pc, #40]	; 7ebc <mmc_read_blocks+0xec>
    7e90:	e59f3028 	ldr	r3, [pc, #40]	; 7ec0 <mmc_read_blocks+0xf0>
    7e94:	e1a0e00f 	mov	lr, pc
    7e98:	e593f000 	ldr	pc, [r3]
    7e9c:	e1a00005 	mov	r0, r5
    7ea0:	ea000003 	b	7eb4 <mmc_read_blocks+0xe4>
    7ea4:	e1a00004 	mov	r0, r4
    7ea8:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    7eac:	ebfffd15 	bl	7308 <mmc_send_status>
    7eb0:	e1a00006 	mov	r0, r6
    7eb4:	e28dd030 	add	sp, sp, #48	; 0x30
    7eb8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    7ebc:	0001223f 	.word	0x0001223f
    7ec0:	0800d704 	.word	0x0800d704

00007ec4 <mmc_bread>:
    7ec4:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    7ec8:	e252a000 	subs	sl, r2, #0	; 0x0
    7ecc:	e24dd004 	sub	sp, sp, #4	; 0x4
    7ed0:	e1a07001 	mov	r7, r1
    7ed4:	e1a08003 	mov	r8, r3
    7ed8:	0a000027 	beq	7f7c <mmc_bread+0xb8>
    7edc:	ebfffae8 	bl	6a84 <find_mmc_device>
    7ee0:	e2506000 	subs	r6, r0, #0	; 0x0
    7ee4:	0a000024 	beq	7f7c <mmc_bread+0xb8>
    7ee8:	e59620a8 	ldr	r2, [r6, #168]
    7eec:	e08a1007 	add	r1, sl, r7
    7ef0:	e1510002 	cmp	r1, r2
    7ef4:	9a000004 	bls	7f0c <mmc_bread+0x48>
    7ef8:	e59f0088 	ldr	r0, [pc, #136]	; 7f88 <mmc_bread+0xc4>
    7efc:	e59f3088 	ldr	r3, [pc, #136]	; 7f8c <mmc_bread+0xc8>
    7f00:	e1a0e00f 	mov	lr, pc
    7f04:	e593f000 	ldr	pc, [r3]
    7f08:	ea00001b 	b	7f7c <mmc_bread+0xb8>
    7f0c:	e5961088 	ldr	r1, [r6, #136]
    7f10:	ebfffacd 	bl	6a4c <mmc_set_blocklen>
    7f14:	e3500000 	cmp	r0, #0	; 0x0
    7f18:	01a0500a 	moveq	r5, sl
    7f1c:	0a000005 	beq	7f38 <mmc_bread+0x74>
    7f20:	e5961088 	ldr	r1, [r6, #136]
    7f24:	e59f0064 	ldr	r0, [pc, #100]	; 7f90 <mmc_bread+0xcc>
    7f28:	e59f305c 	ldr	r3, [pc, #92]	; 7f8c <mmc_bread+0xc8>
    7f2c:	e1a0e00f 	mov	lr, pc
    7f30:	e593f000 	ldr	pc, [r3]
    7f34:	ea000010 	b	7f7c <mmc_bread+0xb8>
    7f38:	e5964110 	ldr	r4, [r6, #272]
    7f3c:	e1a02007 	mov	r2, r7
    7f40:	e1550004 	cmp	r5, r4
    7f44:	31a04005 	movcc	r4, r5
    7f48:	e1a01008 	mov	r1, r8
    7f4c:	e1a00006 	mov	r0, r6
    7f50:	e1a03004 	mov	r3, r4
    7f54:	ebffff9d 	bl	7dd0 <mmc_read_blocks>
    7f58:	e1500004 	cmp	r0, r4
    7f5c:	e0877000 	add	r7, r7, r0
    7f60:	1a000005 	bne	7f7c <mmc_bread+0xb8>
    7f64:	e0555000 	subs	r5, r5, r0
    7f68:	e5962088 	ldr	r2, [r6, #136]
    7f6c:	10288092 	mlane	r8, r2, r0, r8
    7f70:	1afffff0 	bne	7f38 <mmc_bread+0x74>
    7f74:	e000029a 	mul	r0, sl, r2
    7f78:	ea000000 	b	7f80 <mmc_bread+0xbc>
    7f7c:	e3a00000 	mov	r0, #0	; 0x0
    7f80:	e28dd004 	add	sp, sp, #4	; 0x4
    7f84:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    7f88:	00012201 	.word	0x00012201
    7f8c:	0800d704 	.word	0x0800d704
    7f90:	000122a6 	.word	0x000122a6

00007f94 <mmc2_vs_read>:
    7f94:	e1a024a2 	lsr	r2, r2, #9
    7f98:	e1821b83 	orr	r1, r2, r3, lsl #23
    7f9c:	e59d2000 	ldr	r2, [sp]
    7fa0:	e1a03000 	mov	r3, r0
    7fa4:	e1a024c2 	asr	r2, r2, #9
    7fa8:	e3a00002 	mov	r0, #2	; 0x2
    7fac:	eaffffc4 	b	7ec4 <mmc_bread>

00007fb0 <mmc1_vs_read>:
    7fb0:	e1a024a2 	lsr	r2, r2, #9
    7fb4:	e1821b83 	orr	r1, r2, r3, lsl #23
    7fb8:	e59d2000 	ldr	r2, [sp]
    7fbc:	e1a03000 	mov	r3, r0
    7fc0:	e1a024c2 	asr	r2, r2, #9
    7fc4:	e3a00001 	mov	r0, #1	; 0x1
    7fc8:	eaffffbd 	b	7ec4 <mmc_bread>

00007fcc <mmc0_vs_read>:
    7fcc:	e1a024a2 	lsr	r2, r2, #9
    7fd0:	e1821b83 	orr	r1, r2, r3, lsl #23
    7fd4:	e59d2000 	ldr	r2, [sp]
    7fd8:	e1a03000 	mov	r3, r0
    7fdc:	e1a024c2 	asr	r2, r2, #9
    7fe0:	e3a00000 	mov	r0, #0	; 0x0
    7fe4:	eaffffb6 	b	7ec4 <mmc_bread>

00007fe8 <mci_pull_data32>:
    7fe8:	e1a02142 	asr	r2, r2, #2
    7fec:	e3a0c000 	mov	ip, #0	; 0x0
    7ff0:	ea000003 	b	8004 <mci_pull_data32+0x1c>
    7ff4:	e5903118 	ldr	r3, [r0, #280]
    7ff8:	e5933200 	ldr	r3, [r3, #512]
    7ffc:	e781300c 	str	r3, [r1, ip]
    8000:	e28cc004 	add	ip, ip, #4	; 0x4
    8004:	e3520000 	cmp	r2, #0	; 0x0
    8008:	e2422001 	sub	r2, r2, #1	; 0x1
    800c:	cafffff8 	bgt	7ff4 <mci_pull_data32+0xc>
    8010:	e12fff1e 	bx	lr

00008014 <mci_push_data32>:
    8014:	e92d4010 	push	{r4, lr}
    8018:	e3a0c000 	mov	ip, #0	; 0x0
    801c:	e1a04000 	mov	r4, r0
    8020:	e1a00001 	mov	r0, r1
    8024:	e1a01142 	asr	r1, r2, #2
    8028:	ea000003 	b	803c <mci_push_data32+0x28>
    802c:	e790200c 	ldr	r2, [r0, ip]
    8030:	e5943118 	ldr	r3, [r4, #280]
    8034:	e28cc004 	add	ip, ip, #4	; 0x4
    8038:	e5832200 	str	r2, [r3, #512]
    803c:	e3510000 	cmp	r1, #0	; 0x0
    8040:	e2411001 	sub	r1, r1, #1	; 0x1
    8044:	cafffff8 	bgt	802c <mci_push_data32+0x18>
    8048:	e8bd8010 	pop	{r4, pc}

0000804c <imap_sdhc_initialize>:
    804c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8050:	e3510000 	cmp	r1, #0	; 0x0
    8054:	e24dd004 	sub	sp, sp, #4	; 0x4
    8058:	1a000005 	bne	8074 <imap_sdhc_initialize+0x28>
    805c:	e59f00dc 	ldr	r0, [pc, #220]	; 8140 <imap_sdhc_initialize+0xf4>
    8060:	e59f30dc 	ldr	r3, [pc, #220]	; 8144 <imap_sdhc_initialize+0xf8>
    8064:	e1a0e00f 	mov	lr, pc
    8068:	e593f000 	ldr	pc, [r3]
    806c:	e3e00004 	mvn	r0, #4	; 0x4
    8070:	ea000030 	b	8138 <imap_sdhc_initialize+0xec>
    8074:	e3500000 	cmp	r0, #0	; 0x0
    8078:	03a0359d 	moveq	r3, #658505728	; 0x27400000
    807c:	0a000005 	beq	8098 <imap_sdhc_initialize+0x4c>
    8080:	e3500001 	cmp	r0, #1	; 0x1
    8084:	059f30bc 	ldreq	r3, [pc, #188]	; 8148 <imap_sdhc_initialize+0xfc>
    8088:	0a000002 	beq	8098 <imap_sdhc_initialize+0x4c>
    808c:	e3500002 	cmp	r0, #2	; 0x2
    8090:	1a000018 	bne	80f8 <imap_sdhc_initialize+0xac>
    8094:	e59f30b0 	ldr	r3, [pc, #176]	; 814c <imap_sdhc_initialize+0x100>
    8098:	e5813118 	str	r3, [r1, #280]
    809c:	e59f30ac 	ldr	r3, [pc, #172]	; 8150 <imap_sdhc_initialize+0x104>
    80a0:	e591c130 	ldr	ip, [r1, #304]
    80a4:	e5813104 	str	r3, [r1, #260]
    80a8:	e59f30a4 	ldr	r3, [pc, #164]	; 8154 <imap_sdhc_initialize+0x108>
    80ac:	e3a02000 	mov	r2, #0	; 0x0
    80b0:	e5813108 	str	r3, [r1, #264]
    80b4:	e59f309c 	ldr	r3, [pc, #156]	; 8158 <imap_sdhc_initialize+0x10c>
    80b8:	e5812034 	str	r2, [r1, #52]
    80bc:	e581310c 	str	r3, [r1, #268]
    80c0:	e59f3094 	ldr	r3, [pc, #148]	; 815c <imap_sdhc_initialize+0x110>
    80c4:	e5812028 	str	r2, [r1, #40]
    80c8:	e581302c 	str	r3, [r1, #44]
    80cc:	e3003101 	movw	r3, #257	; 0x101
    80d0:	e5813050 	str	r3, [r1, #80]
    80d4:	e59f3084 	ldr	r3, [pc, #132]	; 8160 <imap_sdhc_initialize+0x114>
    80d8:	e5812110 	str	r2, [r1, #272]
    80dc:	e5813038 	str	r3, [r1, #56]
    80e0:	e59f307c 	ldr	r3, [pc, #124]	; 8164 <imap_sdhc_initialize+0x118>
    80e4:	e5810114 	str	r0, [r1, #276]
    80e8:	e581303c 	str	r3, [r1, #60]
    80ec:	e28c2b01 	add	r2, ip, #1024	; 0x400
    80f0:	e28c3010 	add	r3, ip, #16	; 0x10
    80f4:	ea000005 	b	8110 <imap_sdhc_initialize+0xc4>
    80f8:	e59f0068 	ldr	r0, [pc, #104]	; 8168 <imap_sdhc_initialize+0x11c>
    80fc:	e59f3040 	ldr	r3, [pc, #64]	; 8144 <imap_sdhc_initialize+0xf8>
    8100:	e1a0e00f 	mov	lr, pc
    8104:	e593f000 	ldr	pc, [r3]
    8108:	e3e00000 	mvn	r0, #0	; 0x0
    810c:	ea000009 	b	8138 <imap_sdhc_initialize+0xec>
    8110:	e5033004 	str	r3, [r3, #-4]
    8114:	e2833010 	add	r3, r3, #16	; 0x10
    8118:	e1530002 	cmp	r3, r2
    811c:	1afffffb 	bne	8110 <imap_sdhc_initialize+0xc4>
    8120:	e3a03020 	mov	r3, #32	; 0x20
    8124:	e58c33f0 	str	r3, [ip, #1008]
    8128:	e58cc3fc 	str	ip, [ip, #1020]
    812c:	e1a00001 	mov	r0, r1
    8130:	ebfffad4 	bl	6c88 <mmc_register>
    8134:	e3a00000 	mov	r0, #0	; 0x0
    8138:	e28dd004 	add	sp, sp, #4	; 0x4
    813c:	e8bd8000 	pop	{pc}
    8140:	000122b0 	.word	0x000122b0
    8144:	0800d704 	.word	0x0800d704
    8148:	2a200000 	.word	0x2a200000
    814c:	2a300000 	.word	0x2a300000
    8150:	000081dc 	.word	0x000081dc
    8154:	000086c4 	.word	0x000086c4
    8158:	00008500 	.word	0x00008500
    815c:	00ff8000 	.word	0x00ff8000
    8160:	00061a80 	.word	0x00061a80
    8164:	018cba80 	.word	0x018cba80
    8168:	000122c6 	.word	0x000122c6

0000816c <mci_send_cmd>:
    816c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8170:	e5903118 	ldr	r3, [r0, #280]
    8174:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    8178:	e5832028 	str	r2, [r3, #40]
    817c:	e5903118 	ldr	r3, [r0, #280]
    8180:	e24dd004 	sub	sp, sp, #4	; 0x4
    8184:	e583102c 	str	r1, [r3, #44]
    8188:	e1a04000 	mov	r4, r0
    818c:	eb00020e 	bl	89cc <get_ticks>
    8190:	e59f5040 	ldr	r5, [pc, #64]	; 81d8 <mci_send_cmd+0x6c>
    8194:	e1a06000 	mov	r6, r0
    8198:	e1a07001 	mov	r7, r1
    819c:	e5943118 	ldr	r3, [r4, #280]
    81a0:	e593302c 	ldr	r3, [r3, #44]
    81a4:	e3530000 	cmp	r3, #0	; 0x0
    81a8:	aa000008 	bge	81d0 <mci_send_cmd+0x64>
    81ac:	eb000206 	bl	89cc <get_ticks>
    81b0:	e1a02000 	mov	r2, r0
    81b4:	e1a03001 	mov	r3, r1
    81b8:	e0522006 	subs	r2, r2, r6
    81bc:	e0c33007 	sbc	r3, r3, r7
    81c0:	e3530000 	cmp	r3, #0	; 0x0
    81c4:	1a000001 	bne	81d0 <mci_send_cmd+0x64>
    81c8:	e1520005 	cmp	r2, r5
    81cc:	9afffff2 	bls	819c <mci_send_cmd+0x30>
    81d0:	e28dd004 	add	sp, sp, #4	; 0x4
    81d4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    81d8:	0007a11f 	.word	0x0007a11f

000081dc <imap_sdhc_send_cmd>:
    81dc:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    81e0:	e252a000 	subs	sl, r2, #0	; 0x0
    81e4:	e24dd004 	sub	sp, sp, #4	; 0x4
    81e8:	e1a04000 	mov	r4, r0
    81ec:	e1a08001 	mov	r8, r1
    81f0:	0a000038 	beq	82d8 <imap_sdhc_send_cmd+0xfc>
    81f4:	e99a000c 	ldmib	sl, {r2, r3}
    81f8:	e59a100c 	ldr	r1, [sl, #12]
    81fc:	e3120001 	tst	r2, #1	; 0x1
    8200:	e0000193 	mul	r0, r3, r1
    8204:	15942118 	ldrne	r2, [r4, #280]
    8208:	11a03801 	lslne	r3, r1, #16
    820c:	13833001 	orrne	r3, r3, #1	; 0x1
    8210:	15823100 	strne	r3, [r2, #256]
    8214:	e59a200c 	ldr	r2, [sl, #12]
    8218:	e5943118 	ldr	r3, [r4, #280]
    821c:	e1a0e000 	mov	lr, r0
    8220:	e583201c 	str	r2, [r3, #28]
    8224:	e5943118 	ldr	r3, [r4, #280]
    8228:	e3a05000 	mov	r5, #0	; 0x0
    822c:	e5830020 	str	r0, [r3, #32]
    8230:	e5942118 	ldr	r2, [r4, #280]
    8234:	e3a0714a 	mov	r7, #-2147483630	; 0x80000012
    8238:	e5923000 	ldr	r3, [r2]
    823c:	e3833020 	orr	r3, r3, #32	; 0x20
    8240:	e5823000 	str	r3, [r2]
    8244:	e59a6000 	ldr	r6, [sl]
    8248:	e594c130 	ldr	ip, [r4, #304]
    824c:	e35e0eff 	cmp	lr, #4080	; 0xff0
    8250:	31a0100e 	movcc	r1, lr
    8254:	23a01eff 	movcs	r1, #4080	; 0xff0
    8258:	e59c0004 	ldr	r0, [ip, #4]
    825c:	e59f328c 	ldr	r3, [pc, #652]	; 84f0 <imap_sdhc_send_cmd+0x314>
    8260:	e1a02a01 	lsl	r2, r1, #20
    8264:	e0003003 	and	r3, r0, r3
    8268:	e1a02a22 	lsr	r2, r2, #20
    826c:	e1822003 	orr	r2, r2, r3
    8270:	e05ee001 	subs	lr, lr, r1
    8274:	e0853006 	add	r3, r5, r6
    8278:	e98c000c 	stmib	ip, {r2, r3}
    827c:	e58c7000 	str	r7, [ip]
    8280:	e1a0200c 	mov	r2, ip
    8284:	e0855001 	add	r5, r5, r1
    8288:	e28cc010 	add	ip, ip, #16	; 0x10
    828c:	1affffee 	bne	824c <imap_sdhc_send_cmd+0x70>
    8290:	e3c73002 	bic	r3, r7, #2	; 0x2
    8294:	e3833004 	orr	r3, r3, #4	; 0x4
    8298:	e5823000 	str	r3, [r2]
    829c:	e5942130 	ldr	r2, [r4, #304]
    82a0:	e5923000 	ldr	r3, [r2]
    82a4:	e3833008 	orr	r3, r3, #8	; 0x8
    82a8:	e5823000 	str	r3, [r2]
    82ac:	e5942118 	ldr	r2, [r4, #280]
    82b0:	e5923000 	ldr	r3, [r2]
    82b4:	e3833402 	orr	r3, r3, #33554432	; 0x2000000
    82b8:	e5823000 	str	r3, [r2]
    82bc:	e5942118 	ldr	r2, [r4, #280]
    82c0:	e5923080 	ldr	r3, [r2, #128]
    82c4:	e3833080 	orr	r3, r3, #128	; 0x80
    82c8:	e5823080 	str	r3, [r2, #128]
    82cc:	e5942118 	ldr	r2, [r4, #280]
    82d0:	e3a03001 	mov	r3, #1	; 0x1
    82d4:	e5823084 	str	r3, [r2, #132]
    82d8:	e1d800b0 	ldrh	r0, [r8]
    82dc:	e5982004 	ldr	r2, [r8, #4]
    82e0:	e350000c 	cmp	r0, #12	; 0xc
    82e4:	03a03901 	moveq	r3, #16384	; 0x4000
    82e8:	13a03a02 	movne	r3, #8192	; 0x2000
    82ec:	e3120004 	tst	r2, #4	; 0x4
    82f0:	13833c01 	orrne	r3, r3, #256	; 0x100
    82f4:	e3120001 	tst	r2, #1	; 0x1
    82f8:	e3831202 	orr	r1, r3, #536870912	; 0x20000000
    82fc:	0a000002 	beq	830c <imap_sdhc_send_cmd+0x130>
    8300:	e3120002 	tst	r2, #2	; 0x2
    8304:	e3811040 	orr	r1, r1, #64	; 0x40
    8308:	13811080 	orrne	r1, r1, #128	; 0x80
    830c:	e35a0000 	cmp	sl, #0	; 0x0
    8310:	0a000003 	beq	8324 <imap_sdhc_send_cmd+0x148>
    8314:	e59a3004 	ldr	r3, [sl, #4]
    8318:	e3811c02 	orr	r1, r1, #512	; 0x200
    831c:	e3130002 	tst	r3, #2	; 0x2
    8320:	13811b01 	orrne	r1, r1, #1024	; 0x400
    8324:	e594312c 	ldr	r3, [r4, #300]
    8328:	e1811000 	orr	r1, r1, r0
    832c:	e3530000 	cmp	r3, #0	; 0x0
    8330:	13a03000 	movne	r3, #0	; 0x0
    8334:	1584312c 	strne	r3, [r4, #300]
    8338:	e5982008 	ldr	r2, [r8, #8]
    833c:	e5943118 	ldr	r3, [r4, #280]
    8340:	13811902 	orrne	r1, r1, #32768	; 0x8000
    8344:	e5832028 	str	r2, [r3, #40]
    8348:	e5943118 	ldr	r3, [r4, #280]
    834c:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    8350:	e583102c 	str	r1, [r3, #44]
    8354:	eb00019c 	bl	89cc <get_ticks>
    8358:	e3005bb8 	movw	r5, #3000	; 0xbb8
    835c:	e1a06000 	mov	r6, r0
    8360:	e1a07001 	mov	r7, r1
    8364:	eb000198 	bl	89cc <get_ticks>
    8368:	e1a02000 	mov	r2, r0
    836c:	e1a03001 	mov	r3, r1
    8370:	e0522006 	subs	r2, r2, r6
    8374:	e0c33007 	sbc	r3, r3, r7
    8378:	e3530000 	cmp	r3, #0	; 0x0
    837c:	1a000007 	bne	83a0 <imap_sdhc_send_cmd+0x1c4>
    8380:	e1520005 	cmp	r2, r5
    8384:	8a000005 	bhi	83a0 <imap_sdhc_send_cmd+0x1c4>
    8388:	e5943118 	ldr	r3, [r4, #280]
    838c:	e5932044 	ldr	r2, [r3, #68]
    8390:	e2831044 	add	r1, r3, #68	; 0x44
    8394:	e3120004 	tst	r2, #4	; 0x4
    8398:	0afffff1 	beq	8364 <imap_sdhc_send_cmd+0x188>
    839c:	ea000005 	b	83b8 <imap_sdhc_send_cmd+0x1dc>
    83a0:	e59f014c 	ldr	r0, [pc, #332]	; 84f4 <imap_sdhc_send_cmd+0x318>
    83a4:	e59f314c 	ldr	r3, [pc, #332]	; 84f8 <imap_sdhc_send_cmd+0x31c>
    83a8:	e1a0e00f 	mov	lr, pc
    83ac:	e593f000 	ldr	pc, [r3]
    83b0:	e3e00011 	mvn	r0, #17	; 0x11
    83b4:	ea00004b 	b	84e8 <imap_sdhc_send_cmd+0x30c>
    83b8:	e5915000 	ldr	r5, [r1]
    83bc:	e3003142 	movw	r3, #322	; 0x142
    83c0:	e0053003 	and	r3, r5, r3
    83c4:	e3530000 	cmp	r3, #0	; 0x0
    83c8:	e3a03004 	mov	r3, #4	; 0x4
    83cc:	e5813000 	str	r3, [r1]
    83d0:	0a00000e 	beq	8410 <imap_sdhc_send_cmd+0x234>
    83d4:	e1d810b0 	ldrh	r1, [r8]
    83d8:	e5943118 	ldr	r3, [r4, #280]
    83dc:	e3510037 	cmp	r1, #55	; 0x37
    83e0:	13510008 	cmpne	r1, #8	; 0x8
    83e4:	e5835044 	str	r5, [r3, #68]
    83e8:	0a000004 	beq	8400 <imap_sdhc_send_cmd+0x224>
    83ec:	e59f0108 	ldr	r0, [pc, #264]	; 84fc <imap_sdhc_send_cmd+0x320>
    83f0:	e1a02005 	mov	r2, r5
    83f4:	e59f30fc 	ldr	r3, [pc, #252]	; 84f8 <imap_sdhc_send_cmd+0x31c>
    83f8:	e1a0e00f 	mov	lr, pc
    83fc:	e593f000 	ldr	pc, [r3]
    8400:	e3150c01 	tst	r5, #256	; 0x100
    8404:	13e00012 	mvnne	r0, #18	; 0x12
    8408:	03e00011 	mvneq	r0, #17	; 0x11
    840c:	ea000035 	b	84e8 <imap_sdhc_send_cmd+0x30c>
    8410:	e5983004 	ldr	r3, [r8, #4]
    8414:	e5942118 	ldr	r2, [r4, #280]
    8418:	e3130002 	tst	r3, #2	; 0x2
    841c:	05923030 	ldreq	r3, [r2, #48]
    8420:	0588300c 	streq	r3, [r8, #12]
    8424:	0a00000a 	beq	8454 <imap_sdhc_send_cmd+0x278>
    8428:	e592303c 	ldr	r3, [r2, #60]
    842c:	e588300c 	str	r3, [r8, #12]
    8430:	e5943118 	ldr	r3, [r4, #280]
    8434:	e5933038 	ldr	r3, [r3, #56]
    8438:	e5883010 	str	r3, [r8, #16]
    843c:	e5943118 	ldr	r3, [r4, #280]
    8440:	e5933034 	ldr	r3, [r3, #52]
    8444:	e5883014 	str	r3, [r8, #20]
    8448:	e5943118 	ldr	r3, [r4, #280]
    844c:	e5933030 	ldr	r3, [r3, #48]
    8450:	e5883018 	str	r3, [r8, #24]
    8454:	e35a0000 	cmp	sl, #0	; 0x0
    8458:	01a0000a 	moveq	r0, sl
    845c:	0a000021 	beq	84e8 <imap_sdhc_send_cmd+0x30c>
    8460:	e594c118 	ldr	ip, [r4, #280]
    8464:	e59a3004 	ldr	r3, [sl, #4]
    8468:	e28c7044 	add	r7, ip, #68	; 0x44
    846c:	e2030001 	and	r0, r3, #1	; 0x1
    8470:	e28c608c 	add	r6, ip, #140	; 0x8c
    8474:	e5975000 	ldr	r5, [r7]
    8478:	e3500000 	cmp	r0, #0	; 0x0
    847c:	e596e000 	ldr	lr, [r6]
    8480:	e2051008 	and	r1, r5, #8	; 0x8
    8484:	0a000005 	beq	84a0 <imap_sdhc_send_cmd+0x2c4>
    8488:	e3510000 	cmp	r1, #0	; 0x0
    848c:	0afffff8 	beq	8474 <imap_sdhc_send_cmd+0x298>
    8490:	e31e0002 	tst	lr, #2	; 0x2
    8494:	0afffff6 	beq	8474 <imap_sdhc_send_cmd+0x298>
    8498:	e3a03002 	mov	r3, #2	; 0x2
    849c:	ea000004 	b	84b4 <imap_sdhc_send_cmd+0x2d8>
    84a0:	e3510000 	cmp	r1, #0	; 0x0
    84a4:	0afffff2 	beq	8474 <imap_sdhc_send_cmd+0x298>
    84a8:	e31e0001 	tst	lr, #1	; 0x1
    84ac:	0afffff0 	beq	8474 <imap_sdhc_send_cmd+0x298>
    84b0:	e3a03001 	mov	r3, #1	; 0x1
    84b4:	e58c308c 	str	r3, [ip, #140]
    84b8:	e5943118 	ldr	r3, [r4, #280]
    84bc:	e3a00000 	mov	r0, #0	; 0x0
    84c0:	e5835044 	str	r5, [r3, #68]
    84c4:	e5942118 	ldr	r2, [r4, #280]
    84c8:	e5923000 	ldr	r3, [r2]
    84cc:	e3c33402 	bic	r3, r3, #33554432	; 0x2000000
    84d0:	e3833004 	orr	r3, r3, #4	; 0x4
    84d4:	e5823000 	str	r3, [r2]
    84d8:	e5942118 	ldr	r2, [r4, #280]
    84dc:	e5923080 	ldr	r3, [r2, #128]
    84e0:	e3c33080 	bic	r3, r3, #128	; 0x80
    84e4:	e5823080 	str	r3, [r2, #128]
    84e8:	e28dd004 	add	sp, sp, #4	; 0x4
    84ec:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    84f0:	03ffe000 	.word	0x03ffe000
    84f4:	000122d8 	.word	0x000122d8
    84f8:	0800d704 	.word	0x0800d704
    84fc:	000122e6 	.word	0x000122e6

00008500 <imap_sdhc_init>:
    8500:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8504:	e5903114 	ldr	r3, [r0, #276]
    8508:	e3a01000 	mov	r1, #0	; 0x0
    850c:	e3a02001 	mov	r2, #1	; 0x1
    8510:	e3530001 	cmp	r3, #1	; 0x1
    8514:	e24dd004 	sub	sp, sp, #4	; 0x4
    8518:	e1a04000 	mov	r4, r0
    851c:	e580111c 	str	r1, [r0, #284]
    8520:	e5801048 	str	r1, [r0, #72]
    8524:	e580212c 	str	r2, [r0, #300]
    8528:	0a000008 	beq	8550 <imap_sdhc_init+0x50>
    852c:	3a000002 	bcc	853c <imap_sdhc_init+0x3c>
    8530:	e3530002 	cmp	r3, #2	; 0x2
    8534:	1a00000d 	bne	8570 <imap_sdhc_init+0x70>
    8538:	ea000008 	b	8560 <imap_sdhc_init+0x60>
    853c:	e1a01002 	mov	r1, r2
    8540:	e3a0000c 	mov	r0, #12	; 0xc
    8544:	ebffe0c2 	bl	854 <pads_init_module>
    8548:	e59f0150 	ldr	r0, [pc, #336]	; 86a0 <imap_sdhc_init+0x1a0>
    854c:	ea000006 	b	856c <imap_sdhc_init+0x6c>
    8550:	e3a0000d 	mov	r0, #13	; 0xd
    8554:	ebffe0be 	bl	854 <pads_init_module>
    8558:	e59f0144 	ldr	r0, [pc, #324]	; 86a4 <imap_sdhc_init+0x1a4>
    855c:	ea000002 	b	856c <imap_sdhc_init+0x6c>
    8560:	e3a0000e 	mov	r0, #14	; 0xe
    8564:	ebffe0ba 	bl	854 <pads_init_module>
    8568:	e59f0138 	ldr	r0, [pc, #312]	; 86a8 <imap_sdhc_init+0x1a8>
    856c:	ebffe072 	bl	73c <module_enable>
    8570:	e5942118 	ldr	r2, [r4, #280]
    8574:	e59f5130 	ldr	r5, [pc, #304]	; 86ac <imap_sdhc_init+0x1ac>
    8578:	e5923070 	ldr	r3, [r2, #112]
    857c:	e1a03943 	asr	r3, r3, #18
    8580:	e2033007 	and	r3, r3, #7	; 0x7
    8584:	e3530002 	cmp	r3, #2	; 0x2
    8588:	03a03003 	moveq	r3, #3	; 0x3
    858c:	13a03002 	movne	r3, #2	; 0x2
    8590:	e5843120 	str	r3, [r4, #288]
    8594:	e3a03007 	mov	r3, #7	; 0x7
    8598:	e5823000 	str	r3, [r2]
    859c:	e59f310c 	ldr	r3, [pc, #268]	; 86b0 <imap_sdhc_init+0x1b0>
    85a0:	e5843124 	str	r3, [r4, #292]
    85a4:	e59f3108 	ldr	r3, [pc, #264]	; 86b4 <imap_sdhc_init+0x1b4>
    85a8:	e5843128 	str	r3, [r4, #296]
    85ac:	eb000106 	bl	89cc <get_ticks>
    85b0:	e1a06000 	mov	r6, r0
    85b4:	e1a07001 	mov	r7, r1
    85b8:	e5943118 	ldr	r3, [r4, #280]
    85bc:	e3a00001 	mov	r0, #1	; 0x1
    85c0:	e5933000 	ldr	r3, [r3]
    85c4:	e3130007 	tst	r3, #7	; 0x7
    85c8:	0a00000d 	beq	8604 <imap_sdhc_init+0x104>
    85cc:	eb000111 	bl	8a18 <udelay>
    85d0:	eb0000fd 	bl	89cc <get_ticks>
    85d4:	e1a02000 	mov	r2, r0
    85d8:	e1a03001 	mov	r3, r1
    85dc:	e0522006 	subs	r2, r2, r6
    85e0:	e0c33007 	sbc	r3, r3, r7
    85e4:	e3530000 	cmp	r3, #0	; 0x0
    85e8:	1a000001 	bne	85f4 <imap_sdhc_init+0xf4>
    85ec:	e1520005 	cmp	r2, r5
    85f0:	9afffff0 	bls	85b8 <imap_sdhc_init+0xb8>
    85f4:	e59f00bc 	ldr	r0, [pc, #188]	; 86b8 <imap_sdhc_init+0x1b8>
    85f8:	e59f30bc 	ldr	r3, [pc, #188]	; 86bc <imap_sdhc_init+0x1bc>
    85fc:	e1a0e00f 	mov	lr, pc
    8600:	e593f000 	ldr	pc, [r3]
    8604:	e5943118 	ldr	r3, [r4, #280]
    8608:	e3a01000 	mov	r1, #0	; 0x0
    860c:	e5831024 	str	r1, [r3, #36]
    8610:	e5943118 	ldr	r3, [r4, #280]
    8614:	e3e02000 	mvn	r2, #0	; 0x0
    8618:	e5832044 	str	r2, [r3, #68]
    861c:	e5943118 	ldr	r3, [r4, #280]
    8620:	e1a00001 	mov	r0, r1
    8624:	e5832014 	str	r2, [r3, #20]
    8628:	e5942118 	ldr	r2, [r4, #280]
    862c:	e592304c 	ldr	r3, [r2, #76]
    8630:	e1a03283 	lsl	r3, r3, #5
    8634:	e1a03b23 	lsr	r3, r3, #22
    8638:	e1833803 	orr	r3, r3, r3, lsl #16
    863c:	e582304c 	str	r3, [r2, #76]
    8640:	e5943118 	ldr	r3, [r4, #280]
    8644:	e5831010 	str	r1, [r3, #16]
    8648:	e5942118 	ldr	r2, [r4, #280]
    864c:	e5923004 	ldr	r3, [r2, #4]
    8650:	e3833001 	orr	r3, r3, #1	; 0x1
    8654:	e5823004 	str	r3, [r2, #4]
    8658:	e5943118 	ldr	r3, [r4, #280]
    865c:	e5831058 	str	r1, [r3, #88]
    8660:	e5942118 	ldr	r2, [r4, #280]
    8664:	e59f3054 	ldr	r3, [pc, #84]	; 86c0 <imap_sdhc_init+0x1c0>
    8668:	e5823108 	str	r3, [r2, #264]
    866c:	e5942118 	ldr	r2, [r4, #280]
    8670:	e3003103 	movw	r3, #259	; 0x103
    8674:	e5823090 	str	r3, [r2, #144]
    8678:	e5942130 	ldr	r2, [r4, #304]
    867c:	e5943118 	ldr	r3, [r4, #280]
    8680:	e5832088 	str	r2, [r3, #136]
    8684:	e5942118 	ldr	r2, [r4, #280]
    8688:	e5923080 	ldr	r3, [r2, #128]
    868c:	e3c3307c 	bic	r3, r3, #124	; 0x7c
    8690:	e3833008 	orr	r3, r3, #8	; 0x8
    8694:	e5823080 	str	r3, [r2, #128]
    8698:	e28dd004 	add	sp, sp, #4	; 0x4
    869c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    86a0:	2d039000 	.word	0x2d039000
    86a4:	2b038800 	.word	0x2b038800
    86a8:	2b038c00 	.word	0x2b038c00
    86ac:	0007a11f 	.word	0x0007a11f
    86b0:	00007fe8 	.word	0x00007fe8
    86b4:	00008014 	.word	0x00008014
    86b8:	000122fe 	.word	0x000122fe
    86bc:	0800d704 	.word	0x0800d704
    86c0:	3fa80000 	.word	0x3fa80000

000086c4 <imap_sdhc_set_ios>:
    86c4:	e5902048 	ldr	r2, [r0, #72]
    86c8:	e590311c 	ldr	r3, [r0, #284]
    86cc:	e92d4070 	push	{r4, r5, r6, lr}
    86d0:	e1520003 	cmp	r2, r3
    86d4:	e1a05000 	mov	r5, r0
    86d8:	0a00002a 	beq	8788 <imap_sdhc_set_ios+0xc4>
    86dc:	e5903114 	ldr	r3, [r0, #276]
    86e0:	e59f00d0 	ldr	r0, [pc, #208]	; 87b8 <imap_sdhc_set_ios+0xf4>
    86e4:	e0830000 	add	r0, r3, r0
    86e8:	e1a00200 	lsl	r0, r0, #4
    86ec:	ebffe261 	bl	1078 <module_get_clock>
    86f0:	e5954048 	ldr	r4, [r5, #72]
    86f4:	e1a06000 	mov	r6, r0
    86f8:	e1a01004 	mov	r1, r4
    86fc:	eb001af1 	bl	f2c8 <__aeabi_uidivmod>
    8700:	e3510000 	cmp	r1, #0	; 0x0
    8704:	0a000005 	beq	8720 <imap_sdhc_set_ios+0x5c>
    8708:	e1a00006 	mov	r0, r6
    870c:	e1a01004 	mov	r1, r4
    8710:	eb001a70 	bl	f0d8 <__aeabi_uidiv>
    8714:	e1a000a0 	lsr	r0, r0, #1
    8718:	e2806001 	add	r6, r0, #1	; 0x1
    871c:	ea000003 	b	8730 <imap_sdhc_set_ios+0x6c>
    8720:	e1a00006 	mov	r0, r6
    8724:	e1a01004 	mov	r1, r4
    8728:	eb001a6a 	bl	f0d8 <__aeabi_uidiv>
    872c:	e1a060a0 	lsr	r6, r0, #1
    8730:	e5953118 	ldr	r3, [r5, #280]
    8734:	e3a04000 	mov	r4, #0	; 0x0
    8738:	e5834010 	str	r4, [r3, #16]
    873c:	e5953118 	ldr	r3, [r5, #280]
    8740:	e1a00005 	mov	r0, r5
    8744:	e583400c 	str	r4, [r3, #12]
    8748:	e59f106c 	ldr	r1, [pc, #108]	; 87bc <imap_sdhc_set_ios+0xf8>
    874c:	e1a02004 	mov	r2, r4
    8750:	ebfffe85 	bl	816c <mci_send_cmd>
    8754:	e5953118 	ldr	r3, [r5, #280]
    8758:	e1a00005 	mov	r0, r5
    875c:	e5836008 	str	r6, [r3, #8]
    8760:	e59f1054 	ldr	r1, [pc, #84]	; 87bc <imap_sdhc_set_ios+0xf8>
    8764:	e1a02004 	mov	r2, r4
    8768:	ebfffe7f 	bl	816c <mci_send_cmd>
    876c:	e5952118 	ldr	r2, [r5, #280]
    8770:	e3a03001 	mov	r3, #1	; 0x1
    8774:	e5823010 	str	r3, [r2, #16]
    8778:	e1a00005 	mov	r0, r5
    877c:	e1a02004 	mov	r2, r4
    8780:	e59f1034 	ldr	r1, [pc, #52]	; 87bc <imap_sdhc_set_ios+0xf8>
    8784:	ebfffe78 	bl	816c <mci_send_cmd>
    8788:	e5953044 	ldr	r3, [r5, #68]
    878c:	e3530004 	cmp	r3, #4	; 0x4
    8790:	05952118 	ldreq	r2, [r5, #280]
    8794:	03a03001 	moveq	r3, #1	; 0x1
    8798:	05823018 	streq	r3, [r2, #24]
    879c:	08bd8070 	popeq	{r4, r5, r6, pc}
    87a0:	e3530008 	cmp	r3, #8	; 0x8
    87a4:	e5950118 	ldr	r0, [r5, #280]
    87a8:	059f3010 	ldreq	r3, [pc, #16]	; 87c0 <imap_sdhc_set_ios+0xfc>
    87ac:	13a03000 	movne	r3, #0	; 0x0
    87b0:	e5803018 	str	r3, [r0, #24]
    87b4:	e8bd8070 	pop	{r4, r5, r6, pc}
    87b8:	02d00a24 	.word	0x02d00a24
    87bc:	00202000 	.word	0x00202000
    87c0:	00010001 	.word	0x00010001

000087c4 <serial_tstc>:
    87c4:	e59f3010 	ldr	r3, [pc, #16]	; 87dc <serial_tstc+0x18>
    87c8:	e5930018 	ldr	r0, [r3, #24]
    87cc:	e1a00220 	lsr	r0, r0, #4
    87d0:	e2200001 	eor	r0, r0, #1	; 0x1
    87d4:	e2000001 	and	r0, r0, #1	; 0x1
    87d8:	e12fff1e 	bx	lr
    87dc:	20803000 	.word	0x20803000

000087e0 <serial_putc>:
    87e0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    87e4:	e24dd004 	sub	sp, sp, #4	; 0x4
    87e8:	e1a06000 	mov	r6, r0
    87ec:	eb000076 	bl	89cc <get_ticks>
    87f0:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    87f4:	e3a05000 	mov	r5, #0	; 0x0
    87f8:	e0944000 	adds	r4, r4, r0
    87fc:	e59f7034 	ldr	r7, [pc, #52]	; 8838 <serial_putc+0x58>
    8800:	e0a55001 	adc	r5, r5, r1
    8804:	e5973018 	ldr	r3, [r7, #24]
    8808:	e3130020 	tst	r3, #32	; 0x20
    880c:	0a000005 	beq	8828 <serial_putc+0x48>
    8810:	eb00006d 	bl	89cc <get_ticks>
    8814:	e1510005 	cmp	r1, r5
    8818:	8a000002 	bhi	8828 <serial_putc+0x48>
    881c:	1afffff8 	bne	8804 <serial_putc+0x24>
    8820:	e1500004 	cmp	r0, r4
    8824:	9afffff6 	bls	8804 <serial_putc+0x24>
    8828:	e59f3008 	ldr	r3, [pc, #8]	; 8838 <serial_putc+0x58>
    882c:	e5836000 	str	r6, [r3]
    8830:	e28dd004 	add	sp, sp, #4	; 0x4
    8834:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8838:	20803000 	.word	0x20803000

0000883c <serial_puts>:
    883c:	e92d4030 	push	{r4, r5, lr}
    8840:	e1a05000 	mov	r5, r0
    8844:	e24dd004 	sub	sp, sp, #4	; 0x4
    8848:	e3a04000 	mov	r4, #0	; 0x0
    884c:	ea000006 	b	886c <serial_puts+0x30>
    8850:	e353000a 	cmp	r3, #10	; 0xa
    8854:	1a000001 	bne	8860 <serial_puts+0x24>
    8858:	e3a0000d 	mov	r0, #13	; 0xd
    885c:	ebffffdf 	bl	87e0 <serial_putc>
    8860:	e7d50004 	ldrb	r0, [r5, r4]
    8864:	ebffffdd 	bl	87e0 <serial_putc>
    8868:	e2844001 	add	r4, r4, #1	; 0x1
    886c:	e7d53004 	ldrb	r3, [r5, r4]
    8870:	e3530000 	cmp	r3, #0	; 0x0
    8874:	1afffff5 	bne	8850 <serial_puts+0x14>
    8878:	e28dd004 	add	sp, sp, #4	; 0x4
    887c:	e8bd8030 	pop	{r4, r5, pc}

00008880 <serial_getc>:
    8880:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8884:	e59f2024 	ldr	r2, [pc, #36]	; 88b0 <serial_getc+0x30>
    8888:	e24dd004 	sub	sp, sp, #4	; 0x4
    888c:	e5923018 	ldr	r3, [r2, #24]
    8890:	e3130010 	tst	r3, #16	; 0x10
    8894:	1afffffc 	bne	888c <serial_getc+0xc>
    8898:	ebffef18 	bl	4500 <tt_rnd_seed>
    889c:	e59f300c 	ldr	r3, [pc, #12]	; 88b0 <serial_getc+0x30>
    88a0:	e5930000 	ldr	r0, [r3]
    88a4:	e6ef0070 	uxtb	r0, r0
    88a8:	e28dd004 	add	sp, sp, #4	; 0x4
    88ac:	e8bd8000 	pop	{pc}
    88b0:	20803000 	.word	0x20803000

000088b4 <serial_setbrg>:
    88b4:	e92d4010 	push	{r4, lr}
    88b8:	e59f0038 	ldr	r0, [pc, #56]	; 88f8 <serial_setbrg+0x44>
    88bc:	ebffe1ed 	bl	1078 <module_get_clock>
    88c0:	e59f1034 	ldr	r1, [pc, #52]	; 88fc <serial_setbrg+0x48>
    88c4:	e1a04000 	mov	r4, r0
    88c8:	eb001a84 	bl	f2e0 <__aeabi_idiv>
    88cc:	e6bf3070 	sxth	r3, r0
    88d0:	e59f0028 	ldr	r0, [pc, #40]	; 8900 <serial_setbrg+0x4c>
    88d4:	e59f1020 	ldr	r1, [pc, #32]	; 88fc <serial_setbrg+0x48>
    88d8:	e0204390 	mla	r0, r0, r3, r4
    88dc:	e59f4020 	ldr	r4, [pc, #32]	; 8904 <serial_setbrg+0x50>
    88e0:	e1a00300 	lsl	r0, r0, #6
    88e4:	e5843024 	str	r3, [r4, #36]
    88e8:	eb001a7c 	bl	f2e0 <__aeabi_idiv>
    88ec:	e200003f 	and	r0, r0, #63	; 0x3f
    88f0:	e5840028 	str	r0, [r4, #40]
    88f4:	e8bd8010 	pop	{r4, pc}
    88f8:	2d00a290 	.word	0x2d00a290
    88fc:	001c2000 	.word	0x001c2000
    8900:	ffe3e000 	.word	0xffe3e000
    8904:	20803000 	.word	0x20803000

00008908 <init_serial>:
    8908:	e92d4010 	push	{r4, lr}
    890c:	e59f0050 	ldr	r0, [pc, #80]	; 8964 <init_serial+0x5c>
    8910:	e59f4050 	ldr	r4, [pc, #80]	; 8968 <init_serial+0x60>
    8914:	ebffdf88 	bl	73c <module_enable>
    8918:	e3a01000 	mov	r1, #0	; 0x0
    891c:	e3a0000f 	mov	r0, #15	; 0xf
    8920:	ebffdfcb 	bl	854 <pads_init_module>
    8924:	e594302c 	ldr	r3, [r4, #44]
    8928:	e3c330ff 	bic	r3, r3, #255	; 0xff
    892c:	e3833060 	orr	r3, r3, #96	; 0x60
    8930:	e584302c 	str	r3, [r4, #44]
    8934:	ebffffde 	bl	88b4 <serial_setbrg>
    8938:	e5943030 	ldr	r3, [r4, #48]
    893c:	e3a00000 	mov	r0, #0	; 0x0
    8940:	e3c33cff 	bic	r3, r3, #65280	; 0xff00
    8944:	e3c33087 	bic	r3, r3, #135	; 0x87
    8948:	e3833c03 	orr	r3, r3, #768	; 0x300
    894c:	e3833001 	orr	r3, r3, #1	; 0x1
    8950:	e5843030 	str	r3, [r4, #48]
    8954:	e594302c 	ldr	r3, [r4, #44]
    8958:	e3833010 	orr	r3, r3, #16	; 0x10
    895c:	e584302c 	str	r3, [r4, #44]
    8960:	e8bd8010 	pop	{r4, pc}
    8964:	2d012000 	.word	0x2d012000
    8968:	20803000 	.word	0x20803000

0000896c <init_timer>:
    896c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8970:	e3a02001 	mov	r2, #1	; 0x1
    8974:	e3a03412 	mov	r3, #301989888	; 0x12000000
    8978:	e5832000 	str	r2, [r3]
    897c:	e24dd004 	sub	sp, sp, #4	; 0x4
    8980:	e28334ee 	add	r3, r3, #-301989888	; 0xee000000
    8984:	ee0e3f32 	mcr	15, 0, r3, cr14, cr2, {1}
    8988:	f57ff06f 	isb	sy
    898c:	e59f302c 	ldr	r3, [pc, #44]	; 89c0 <init_timer+0x54>
    8990:	ee0e3f10 	mcr	15, 0, r3, cr14, cr0, {0}
    8994:	ee1e0f10 	mrc	15, 0, r0, cr14, cr0, {0}
    8998:	e59f1024 	ldr	r1, [pc, #36]	; 89c4 <init_timer+0x58>
    899c:	eb001a4f 	bl	f2e0 <__aeabi_idiv>
    89a0:	e59f3020 	ldr	r3, [pc, #32]	; 89c8 <init_timer+0x5c>
    89a4:	e5830000 	str	r0, [r3]
    89a8:	e3a03003 	mov	r3, #3	; 0x3
    89ac:	ee0e3f32 	mcr	15, 0, r3, cr14, cr2, {1}
    89b0:	f57ff06f 	isb	sy
    89b4:	e3a00000 	mov	r0, #0	; 0x0
    89b8:	e28dd004 	add	sp, sp, #4	; 0x4
    89bc:	e8bd8000 	pop	{pc}
    89c0:	07de2900 	.word	0x07de2900
    89c4:	000f4240 	.word	0x000f4240
    89c8:	0800d76c 	.word	0x0800d76c

000089cc <get_ticks>:
    89cc:	e92d4030 	push	{r4, r5, lr}
    89d0:	e24dd00c 	sub	sp, sp, #12	; 0xc
    89d4:	f57ff06f 	isb	sy
    89d8:	ec554f0e 	mrrc	15, 0, r4, r5, cr14
    89dc:	e59f3030 	ldr	r3, [pc, #48]	; 8a14 <get_ticks+0x48>
    89e0:	e3550000 	cmp	r5, #0	; 0x0
    89e4:	e5931000 	ldr	r1, [r3]
    89e8:	e88d0030 	stm	sp, {r4, r5}
    89ec:	1a000003 	bne	8a00 <get_ticks+0x34>
    89f0:	e1a00004 	mov	r0, r4
    89f4:	eb0019b7 	bl	f0d8 <__aeabi_uidiv>
    89f8:	e88d0021 	stm	sp, {r0, r5}
    89fc:	ea000001 	b	8a08 <get_ticks+0x3c>
    8a00:	e1a0000d 	mov	r0, sp
    8a04:	eb000d72 	bl	bfd4 <__div64_32>
    8a08:	e89d0003 	ldm	sp, {r0, r1}
    8a0c:	e28dd00c 	add	sp, sp, #12	; 0xc
    8a10:	e8bd8030 	pop	{r4, r5, pc}
    8a14:	0800d76c 	.word	0x0800d76c

00008a18 <udelay>:
    8a18:	e92d40d0 	push	{r4, r6, r7, lr}
    8a1c:	e1a04000 	mov	r4, r0
    8a20:	ebffffe9 	bl	89cc <get_ticks>
    8a24:	e0906004 	adds	r6, r0, r4
    8a28:	e2a17000 	adc	r7, r1, #0	; 0x0
    8a2c:	ebffffe6 	bl	89cc <get_ticks>
    8a30:	e1570001 	cmp	r7, r1
    8a34:	8afffffc 	bhi	8a2c <udelay+0x14>
    8a38:	18bd80d0 	popne	{r4, r6, r7, pc}
    8a3c:	e1560000 	cmp	r6, r0
    8a40:	8afffff9 	bhi	8a2c <udelay+0x14>
    8a44:	e8bd80d0 	pop	{r4, r6, r7, pc}

00008a48 <get_timer>:
    8a48:	e92d4010 	push	{r4, lr}
    8a4c:	e1a04000 	mov	r4, r0
    8a50:	ebffffdd 	bl	89cc <get_ticks>
    8a54:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8a58:	eb00199e 	bl	f0d8 <__aeabi_uidiv>
    8a5c:	e0640000 	rsb	r0, r4, r0
    8a60:	e8bd8010 	pop	{r4, pc}

00008a64 <e2p_vs_align>:
    8a64:	e3a00010 	mov	r0, #16	; 0x10
    8a68:	e12fff1e 	bx	lr

00008a6c <e2p_vs_reset>:
    8a6c:	e3a00000 	mov	r0, #0	; 0x0
    8a70:	e12fff1e 	bx	lr

00008a74 <iic_address_num>:
    8a74:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8a78:	e24dd004 	sub	sp, sp, #4	; 0x4
    8a7c:	ebffdecd 	bl	5b8 <boot_info>
    8a80:	e59f3014 	ldr	r3, [pc, #20]	; 8a9c <iic_address_num+0x28>
    8a84:	e1a00080 	lsl	r0, r0, #1
    8a88:	e20000ff 	and	r0, r0, #255	; 0xff
    8a8c:	e5830000 	str	r0, [r3]
    8a90:	e3a00001 	mov	r0, #1	; 0x1
    8a94:	e28dd004 	add	sp, sp, #4	; 0x4
    8a98:	e8bd8000 	pop	{pc}
    8a9c:	0800d200 	.word	0x0800d200

00008aa0 <iic_read_get_address>:
    8aa0:	e1d020b2 	ldrh	r2, [r0, #2]
    8aa4:	e590300c 	ldr	r3, [r0, #12]
    8aa8:	e92d4010 	push	{r4, lr}
    8aac:	e0833002 	add	r3, r3, r2
    8ab0:	e1c030b2 	strh	r3, [r0, #2]
    8ab4:	e1a04000 	mov	r4, r0
    8ab8:	ebffffed 	bl	8a74 <iic_address_num>
    8abc:	e3500000 	cmp	r0, #0	; 0x0
    8ac0:	1a000008 	bne	8ae8 <iic_read_get_address+0x48>
    8ac4:	e1d430b2 	ldrh	r3, [r4, #2]
    8ac8:	e3530c01 	cmp	r3, #256	; 0x100
    8acc:	1a000005 	bne	8ae8 <iic_read_get_address+0x48>
    8ad0:	e1d430b0 	ldrh	r3, [r4]
    8ad4:	e1c400b2 	strh	r0, [r4, #2]
    8ad8:	e2833002 	add	r3, r3, #2	; 0x2
    8adc:	e1c430b0 	strh	r3, [r4]
    8ae0:	e3a03001 	mov	r3, #1	; 0x1
    8ae4:	e1c431b0 	strh	r3, [r4, #16]
    8ae8:	ebffffe1 	bl	8a74 <iic_address_num>
    8aec:	e3500000 	cmp	r0, #0	; 0x0
    8af0:	e5941008 	ldr	r1, [r4, #8]
    8af4:	0a000004 	beq	8b0c <iic_read_get_address+0x6c>
    8af8:	e3510000 	cmp	r1, #0	; 0x0
    8afc:	13a03000 	movne	r3, #0	; 0x0
    8b00:	15843008 	strne	r3, [r4, #8]
    8b04:	1a00000e 	bne	8b44 <iic_read_get_address+0xa4>
    8b08:	ea000010 	b	8b50 <iic_read_get_address+0xb0>
    8b0c:	e1d430b2 	ldrh	r3, [r4, #2]
    8b10:	e2632c01 	rsb	r2, r3, #256	; 0x100
    8b14:	e1520001 	cmp	r2, r1
    8b18:	e5842004 	str	r2, [r4, #4]
    8b1c:	2a000005 	bcs	8b38 <iic_read_get_address+0x98>
    8b20:	e0623001 	rsb	r3, r2, r1
    8b24:	e5843008 	str	r3, [r4, #8]
    8b28:	e3a03001 	mov	r3, #1	; 0x1
    8b2c:	e1c431b0 	strh	r3, [r4, #16]
    8b30:	e584200c 	str	r2, [r4, #12]
    8b34:	ea000003 	b	8b48 <iic_read_get_address+0xa8>
    8b38:	e3510000 	cmp	r1, #0	; 0x0
    8b3c:	0a000003 	beq	8b50 <iic_read_get_address+0xb0>
    8b40:	e5840008 	str	r0, [r4, #8]
    8b44:	e584100c 	str	r1, [r4, #12]
    8b48:	e3a00001 	mov	r0, #1	; 0x1
    8b4c:	e8bd8010 	pop	{r4, pc}
    8b50:	e3a00000 	mov	r0, #0	; 0x0
    8b54:	e8bd8010 	pop	{r4, pc}

00008b58 <iic_write_get_address>:
    8b58:	e1d020b2 	ldrh	r2, [r0, #2]
    8b5c:	e590300c 	ldr	r3, [r0, #12]
    8b60:	e92d4010 	push	{r4, lr}
    8b64:	e0833002 	add	r3, r3, r2
    8b68:	e1c030b2 	strh	r3, [r0, #2]
    8b6c:	e1a04000 	mov	r4, r0
    8b70:	ebffffbf 	bl	8a74 <iic_address_num>
    8b74:	e3500000 	cmp	r0, #0	; 0x0
    8b78:	1a000008 	bne	8ba0 <iic_write_get_address+0x48>
    8b7c:	e1d430b2 	ldrh	r3, [r4, #2]
    8b80:	e3530c01 	cmp	r3, #256	; 0x100
    8b84:	1a000005 	bne	8ba0 <iic_write_get_address+0x48>
    8b88:	e1d430b0 	ldrh	r3, [r4]
    8b8c:	e1c400b2 	strh	r0, [r4, #2]
    8b90:	e2833002 	add	r3, r3, #2	; 0x2
    8b94:	e1c430b0 	strh	r3, [r4]
    8b98:	e3a03001 	mov	r3, #1	; 0x1
    8b9c:	e1c431b0 	strh	r3, [r4, #16]
    8ba0:	e1d430b2 	ldrh	r3, [r4, #2]
    8ba4:	e5940008 	ldr	r0, [r4, #8]
    8ba8:	e203300f 	and	r3, r3, #15	; 0xf
    8bac:	e2632010 	rsb	r2, r3, #16	; 0x10
    8bb0:	e1520000 	cmp	r2, r0
    8bb4:	e5842004 	str	r2, [r4, #4]
    8bb8:	2a000004 	bcs	8bd0 <iic_write_get_address+0x78>
    8bbc:	e0623000 	rsb	r3, r2, r0
    8bc0:	e5843008 	str	r3, [r4, #8]
    8bc4:	e584200c 	str	r2, [r4, #12]
    8bc8:	e3a00001 	mov	r0, #1	; 0x1
    8bcc:	e8bd8010 	pop	{r4, pc}
    8bd0:	e3500000 	cmp	r0, #0	; 0x0
    8bd4:	13a03000 	movne	r3, #0	; 0x0
    8bd8:	1584000c 	strne	r0, [r4, #12]
    8bdc:	15843008 	strne	r3, [r4, #8]
    8be0:	13a00001 	movne	r0, #1	; 0x1
    8be4:	e8bd8010 	pop	{r4, pc}

00008be8 <iic_word_addr_func>:
    8be8:	e92d4030 	push	{r4, r5, lr}
    8bec:	e24dd004 	sub	sp, sp, #4	; 0x4
    8bf0:	e1a04000 	mov	r4, r0
    8bf4:	e1a05001 	mov	r5, r1
    8bf8:	ebffff9d 	bl	8a74 <iic_address_num>
    8bfc:	e3500000 	cmp	r0, #0	; 0x0
    8c00:	16ff0074 	uxthne	r0, r4
    8c04:	1a00000c 	bne	8c3c <iic_word_addr_func+0x54>
    8c08:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    8c0c:	e0051003 	and	r1, r5, r3
    8c10:	e3a020ff 	mov	r2, #255	; 0xff
    8c14:	e3510000 	cmp	r1, #0	; 0x0
    8c18:	e0040002 	and	r0, r4, r2
    8c1c:	aa000005 	bge	8c38 <iic_word_addr_func+0x50>
    8c20:	e3e02000 	mvn	r2, #0	; 0x0
    8c24:	e0900002 	adds	r0, r0, r2
    8c28:	e3e020ff 	mvn	r2, #255	; 0xff
    8c2c:	e1800002 	orr	r0, r0, r2
    8c30:	e3a02001 	mov	r2, #1	; 0x1
    8c34:	e0900002 	adds	r0, r0, r2
    8c38:	e6ff0070 	uxth	r0, r0
    8c3c:	e28dd004 	add	sp, sp, #4	; 0x4
    8c40:	e8bd8030 	pop	{r4, r5, pc}

00008c44 <iic_addr_func>:
    8c44:	e92d4030 	push	{r4, r5, lr}
    8c48:	e24dd004 	sub	sp, sp, #4	; 0x4
    8c4c:	e1a04000 	mov	r4, r0
    8c50:	e1a05001 	mov	r5, r1
    8c54:	ebffff86 	bl	8a74 <iic_address_num>
    8c58:	e3500000 	cmp	r0, #0	; 0x0
    8c5c:	e59f003c 	ldr	r0, [pc, #60]	; 8ca0 <iic_addr_func+0x5c>
    8c60:	11d000b0 	ldrhne	r0, [r0]
    8c64:	1a00000b 	bne	8c98 <iic_addr_func+0x54>
    8c68:	e1a02005 	mov	r2, r5
    8c6c:	e1a03fc2 	asr	r3, r2, #31
    8c70:	e1a02fc3 	asr	r2, r3, #31
    8c74:	e20220ff 	and	r2, r2, #255	; 0xff
    8c78:	e3a03000 	mov	r3, #0	; 0x0
    8c7c:	e0922004 	adds	r2, r2, r4
    8c80:	e0a33005 	adc	r3, r3, r5
    8c84:	e5900000 	ldr	r0, [r0]
    8c88:	e1a01422 	lsr	r1, r2, #8
    8c8c:	e1811c03 	orr	r1, r1, r3, lsl #24
    8c90:	e1800081 	orr	r0, r0, r1, lsl #1
    8c94:	e6ff0070 	uxth	r0, r0
    8c98:	e28dd004 	add	sp, sp, #4	; 0x4
    8c9c:	e8bd8030 	pop	{r4, r5, pc}
    8ca0:	0800d200 	.word	0x0800d200

00008ca4 <e2p_vs_write>:
    8ca4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8ca8:	e1a01003 	mov	r1, r3
    8cac:	e24dd024 	sub	sp, sp, #36	; 0x24
    8cb0:	e1a06000 	mov	r6, r0
    8cb4:	e1a00002 	mov	r0, r2
    8cb8:	e1a05003 	mov	r5, r3
    8cbc:	e1a04002 	mov	r4, r2
    8cc0:	ebffffdf 	bl	8c44 <iic_addr_func>
    8cc4:	e1a01005 	mov	r1, r5
    8cc8:	e1cd00bc 	strh	r0, [sp, #12]
    8ccc:	e1a00004 	mov	r0, r4
    8cd0:	ebffffc4 	bl	8be8 <iic_word_addr_func>
    8cd4:	e3a03000 	mov	r3, #0	; 0x0
    8cd8:	e59d2048 	ldr	r2, [sp, #72]
    8cdc:	e1a05003 	mov	r5, r3
    8ce0:	e58d3018 	str	r3, [sp, #24]
    8ce4:	e28dc00c 	add	ip, sp, #12	; 0xc
    8ce8:	e3a03001 	mov	r3, #1	; 0x1
    8cec:	e1cd00be 	strh	r0, [sp, #14]
    8cf0:	e58d2014 	str	r2, [sp, #20]
    8cf4:	e1cd31bc 	strh	r3, [sp, #28]
    8cf8:	e59fb10c 	ldr	fp, [pc, #268]	; 8e0c <e2p_vs_write+0x168>
    8cfc:	e3a0a001 	mov	sl, #1	; 0x1
    8d00:	e1a08005 	mov	r8, r5
    8d04:	e28c9012 	add	r9, ip, #18	; 0x12
    8d08:	ea000037 	b	8dec <e2p_vs_write+0x148>
    8d0c:	ebffff58 	bl	8a74 <iic_address_num>
    8d10:	e3500000 	cmp	r0, #0	; 0x0
    8d14:	e1dd10be 	ldrh	r1, [sp, #14]
    8d18:	11a03421 	lsrne	r3, r1, #8
    8d1c:	15cd301e 	strbne	r3, [sp, #30]
    8d20:	15cd101f 	strbne	r1, [sp, #31]
    8d24:	13a07002 	movne	r7, #2	; 0x2
    8d28:	1a000005 	bne	8d44 <e2p_vs_write+0xa0>
    8d2c:	e6ef1071 	uxtb	r1, r1
    8d30:	e5cd101e 	strb	r1, [sp, #30]
    8d34:	e59f00d4 	ldr	r0, [pc, #212]	; 8e10 <e2p_vs_write+0x16c>
    8d38:	e1a0e00f 	mov	lr, pc
    8d3c:	e59bf000 	ldr	pc, [fp]
    8d40:	e3a07001 	mov	r7, #1	; 0x1
    8d44:	ebffff20 	bl	89cc <get_ticks>
    8d48:	e1dd31bc 	ldrh	r3, [sp, #28]
    8d4c:	e1a04000 	mov	r4, r0
    8d50:	e3530000 	cmp	r3, #0	; 0x0
    8d54:	0a000016 	beq	8db4 <e2p_vs_write+0x110>
    8d58:	e3a0c000 	mov	ip, #0	; 0x0
    8d5c:	e5dd200c 	ldrb	r2, [sp, #12]
    8d60:	e1a00008 	mov	r0, r8
    8d64:	e1a0100a 	mov	r1, sl
    8d68:	e1a03008 	mov	r3, r8
    8d6c:	e1cdc1bc 	strh	ip, [sp, #28]
    8d70:	e58da000 	str	sl, [sp]
    8d74:	e58d8004 	str	r8, [sp, #4]
    8d78:	eb000137 	bl	925c <iic_init>
    8d7c:	e3500000 	cmp	r0, #0	; 0x0
    8d80:	1a00000b 	bne	8db4 <e2p_vs_write+0x110>
    8d84:	e1a00008 	mov	r0, r8
    8d88:	ea00001d 	b	8e04 <e2p_vs_write+0x160>
    8d8c:	ebffff0e 	bl	89cc <get_ticks>
    8d90:	e1540000 	cmp	r4, r0
    8d94:	2a000008 	bcs	8dbc <e2p_vs_write+0x118>
    8d98:	e59f0074 	ldr	r0, [pc, #116]	; 8e14 <e2p_vs_write+0x170>
    8d9c:	e1a0e00f 	mov	lr, pc
    8da0:	e59bf000 	ldr	pc, [fp]
    8da4:	e59d3018 	ldr	r3, [sp, #24]
    8da8:	e0853003 	add	r3, r5, r3
    8dac:	e6ff5073 	uxth	r5, r3
    8db0:	ea00000b 	b	8de4 <e2p_vs_write+0x140>
    8db4:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    8db8:	e2844e12 	add	r4, r4, #288	; 0x120
    8dbc:	e59dc018 	ldr	ip, [sp, #24]
    8dc0:	e1a01009 	mov	r1, r9
    8dc4:	e1a02007 	mov	r2, r7
    8dc8:	e1a03006 	mov	r3, r6
    8dcc:	e3a00000 	mov	r0, #0	; 0x0
    8dd0:	e58dc000 	str	ip, [sp]
    8dd4:	e58da004 	str	sl, [sp, #4]
    8dd8:	eb000078 	bl	8fc0 <iicreg_write>
    8ddc:	e3500000 	cmp	r0, #0	; 0x0
    8de0:	0affffe9 	beq	8d8c <e2p_vs_write+0xe8>
    8de4:	e59d3018 	ldr	r3, [sp, #24]
    8de8:	e0866003 	add	r6, r6, r3
    8dec:	e28d000c 	add	r0, sp, #12	; 0xc
    8df0:	ebffff58 	bl	8b58 <iic_write_get_address>
    8df4:	e3500000 	cmp	r0, #0	; 0x0
    8df8:	1affffc3 	bne	8d0c <e2p_vs_write+0x68>
    8dfc:	e59d2048 	ldr	r2, [sp, #72]
    8e00:	e0650002 	rsb	r0, r5, r2
    8e04:	e28dd024 	add	sp, sp, #36	; 0x24
    8e08:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    8e0c:	0800d704 	.word	0x0800d704
    8e10:	00012311 	.word	0x00012311
    8e14:	0001231b 	.word	0x0001231b

00008e18 <e2p_vs_read>:
    8e18:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8e1c:	e1a01003 	mov	r1, r3
    8e20:	e24dd024 	sub	sp, sp, #36	; 0x24
    8e24:	e1a06000 	mov	r6, r0
    8e28:	e1a00002 	mov	r0, r2
    8e2c:	e1a05003 	mov	r5, r3
    8e30:	e1a04002 	mov	r4, r2
    8e34:	e59d7048 	ldr	r7, [sp, #72]
    8e38:	ebffff81 	bl	8c44 <iic_addr_func>
    8e3c:	e1a01005 	mov	r1, r5
    8e40:	e1cd00bc 	strh	r0, [sp, #12]
    8e44:	e1a00004 	mov	r0, r4
    8e48:	ebffff66 	bl	8be8 <iic_word_addr_func>
    8e4c:	e28db00c 	add	fp, sp, #12	; 0xc
    8e50:	e3a03000 	mov	r3, #0	; 0x0
    8e54:	e3a02001 	mov	r2, #1	; 0x1
    8e58:	e1cd00be 	strh	r0, [sp, #14]
    8e5c:	e1cd21bc 	strh	r2, [sp, #28]
    8e60:	e58d7014 	str	r7, [sp, #20]
    8e64:	e58d3018 	str	r3, [sp, #24]
    8e68:	e1a05003 	mov	r5, r3
    8e6c:	e3a09001 	mov	r9, #1	; 0x1
    8e70:	e28ba012 	add	sl, fp, #18	; 0x12
    8e74:	ea00002b 	b	8f28 <e2p_vs_read+0x110>
    8e78:	ebfffefd 	bl	8a74 <iic_address_num>
    8e7c:	e1dd20be 	ldrh	r2, [sp, #14]
    8e80:	e3500000 	cmp	r0, #0	; 0x0
    8e84:	11a03422 	lsrne	r3, r2, #8
    8e88:	15cd301e 	strbne	r3, [sp, #30]
    8e8c:	15cd201f 	strbne	r2, [sp, #31]
    8e90:	13a08002 	movne	r8, #2	; 0x2
    8e94:	05cd201e 	strbeq	r2, [sp, #30]
    8e98:	03a08001 	moveq	r8, #1	; 0x1
    8e9c:	ebfffeca 	bl	89cc <get_ticks>
    8ea0:	e1dd31bc 	ldrh	r3, [sp, #28]
    8ea4:	e1a04000 	mov	r4, r0
    8ea8:	e3530000 	cmp	r3, #0	; 0x0
    8eac:	0a000010 	beq	8ef4 <e2p_vs_read+0xdc>
    8eb0:	e3a0c000 	mov	ip, #0	; 0x0
    8eb4:	e5dd200c 	ldrb	r2, [sp, #12]
    8eb8:	e1a00005 	mov	r0, r5
    8ebc:	e1a01009 	mov	r1, r9
    8ec0:	e1a03005 	mov	r3, r5
    8ec4:	e1cdc1bc 	strh	ip, [sp, #28]
    8ec8:	e58d9000 	str	r9, [sp]
    8ecc:	e58d5004 	str	r5, [sp, #4]
    8ed0:	eb0000e1 	bl	925c <iic_init>
    8ed4:	e3500000 	cmp	r0, #0	; 0x0
    8ed8:	1a000005 	bne	8ef4 <e2p_vs_read+0xdc>
    8edc:	e1a07005 	mov	r7, r5
    8ee0:	ea000014 	b	8f38 <e2p_vs_read+0x120>
    8ee4:	ebfffeb8 	bl	89cc <get_ticks>
    8ee8:	e1540000 	cmp	r4, r0
    8eec:	2a000002 	bcs	8efc <e2p_vs_read+0xe4>
    8ef0:	ea00000a 	b	8f20 <e2p_vs_read+0x108>
    8ef4:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    8ef8:	e2844e12 	add	r4, r4, #288	; 0x120
    8efc:	e59dc018 	ldr	ip, [sp, #24]
    8f00:	e1a0100a 	mov	r1, sl
    8f04:	e1a02008 	mov	r2, r8
    8f08:	e1a03006 	mov	r3, r6
    8f0c:	e3a00000 	mov	r0, #0	; 0x0
    8f10:	e58dc000 	str	ip, [sp]
    8f14:	eb000061 	bl	90a0 <iic_read_func>
    8f18:	e3500000 	cmp	r0, #0	; 0x0
    8f1c:	0afffff0 	beq	8ee4 <e2p_vs_read+0xcc>
    8f20:	e59d3018 	ldr	r3, [sp, #24]
    8f24:	e0866003 	add	r6, r6, r3
    8f28:	e1a0000b 	mov	r0, fp
    8f2c:	ebfffedb 	bl	8aa0 <iic_read_get_address>
    8f30:	e3500000 	cmp	r0, #0	; 0x0
    8f34:	1affffcf 	bne	8e78 <e2p_vs_read+0x60>
    8f38:	e1a00007 	mov	r0, r7
    8f3c:	e28dd024 	add	sp, sp, #36	; 0x24
    8f40:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

00008f44 <iicreg_read>:
    8f44:	e3500001 	cmp	r0, #1	; 0x1
    8f48:	e92d4070 	push	{r4, r5, r6, lr}
    8f4c:	83a00000 	movhi	r0, #0	; 0x0
    8f50:	e1a05001 	mov	r5, r1
    8f54:	e1a04002 	mov	r4, r2
    8f58:	88bd8070 	pophi	{r4, r5, r6, pc}
    8f5c:	e59f3058 	ldr	r3, [pc, #88]	; 8fbc <iicreg_read+0x78>
    8f60:	e5932070 	ldr	r2, [r3, #112]
    8f64:	e2022008 	and	r2, r2, #8	; 0x8
    8f68:	ea000000 	b	8f70 <iicreg_read+0x2c>
    8f6c:	e5931010 	ldr	r1, [r3, #16]
    8f70:	e3520000 	cmp	r2, #0	; 0x0
    8f74:	1afffffc 	bne	8f6c <iicreg_read+0x28>
    8f78:	e59f103c 	ldr	r1, [pc, #60]	; 8fbc <iicreg_read+0x78>
    8f7c:	e1a00004 	mov	r0, r4
    8f80:	e3a06c01 	mov	r6, #256	; 0x100
    8f84:	e591c070 	ldr	ip, [r1, #112]
    8f88:	e2503000 	subs	r3, r0, #0	; 0x0
    8f8c:	13a03001 	movne	r3, #1	; 0x1
    8f90:	e01330ac 	ands	r3, r3, ip, lsr #1
    8f94:	15816010 	strne	r6, [r1, #16]
    8f98:	12400001 	subne	r0, r0, #1	; 0x1
    8f9c:	e31c0008 	tst	ip, #8	; 0x8
    8fa0:	15913010 	ldrne	r3, [r1, #16]
    8fa4:	12822001 	addne	r2, r2, #1	; 0x1
    8fa8:	14c53001 	strbne	r3, [r5], #1
    8fac:	e1520004 	cmp	r2, r4
    8fb0:	1afffff3 	bne	8f84 <iicreg_read+0x40>
    8fb4:	e3a00001 	mov	r0, #1	; 0x1
    8fb8:	e8bd8070 	pop	{r4, r5, r6, pc}
    8fbc:	20300000 	.word	0x20300000

00008fc0 <iicreg_write>:
    8fc0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    8fc4:	e59d4018 	ldr	r4, [sp, #24]
    8fc8:	e3500001 	cmp	r0, #1	; 0x1
    8fcc:	e1a05001 	mov	r5, r1
    8fd0:	e5dd801c 	ldrb	r8, [sp, #28]
    8fd4:	e1a01003 	mov	r1, r3
    8fd8:	e0847002 	add	r7, r4, r2
    8fdc:	8a00002c 	bhi	9094 <iicreg_write+0xd4>
    8fe0:	e59f00b4 	ldr	r0, [pc, #180]	; 909c <iicreg_write+0xdc>
    8fe4:	e5903034 	ldr	r3, [r0, #52]
    8fe8:	e213c040 	ands	ip, r3, #64	; 0x40
    8fec:	15903080 	ldrne	r3, [r0, #128]
    8ff0:	15903054 	ldrne	r3, [r0, #84]
    8ff4:	13a00000 	movne	r0, #0	; 0x0
    8ff8:	18bd81f0 	popne	{r4, r5, r6, r7, r8, pc}
    8ffc:	e5903070 	ldr	r3, [r0, #112]
    9000:	e3130004 	tst	r3, #4	; 0x4
    9004:	11a06000 	movne	r6, r0
    9008:	11a0000c 	movne	r0, ip
    900c:	11a0c006 	movne	ip, r6
    9010:	1a00000d 	bne	904c <iicreg_write+0x8c>
    9014:	ea00001e 	b	9094 <iicreg_write+0xd4>
    9018:	e5963070 	ldr	r3, [r6, #112]
    901c:	e3130002 	tst	r3, #2	; 0x2
    9020:	0afffffc 	beq	9018 <iicreg_write+0x58>
    9024:	e3520000 	cmp	r2, #0	; 0x0
    9028:	14d53001 	ldrbne	r3, [r5], #1
    902c:	12422001 	subne	r2, r2, #1	; 0x1
    9030:	1a000003 	bne	9044 <iicreg_write+0x84>
    9034:	e3540000 	cmp	r4, #0	; 0x0
    9038:	0a000002 	beq	9048 <iicreg_write+0x88>
    903c:	e4d13001 	ldrb	r3, [r1], #1
    9040:	e2444001 	sub	r4, r4, #1	; 0x1
    9044:	e58c3010 	str	r3, [ip, #16]
    9048:	e2800001 	add	r0, r0, #1	; 0x1
    904c:	e1500007 	cmp	r0, r7
    9050:	3afffff0 	bcc	9018 <iicreg_write+0x58>
    9054:	e59f2040 	ldr	r2, [pc, #64]	; 909c <iicreg_write+0xdc>
    9058:	e5923070 	ldr	r3, [r2, #112]
    905c:	e3130004 	tst	r3, #4	; 0x4
    9060:	0afffffc 	beq	9058 <iicreg_write+0x98>
    9064:	e3580000 	cmp	r8, #0	; 0x0
    9068:	159f202c 	ldrne	r2, [pc, #44]	; 909c <iicreg_write+0xdc>
    906c:	0a000006 	beq	908c <iicreg_write+0xcc>
    9070:	e5923034 	ldr	r3, [r2, #52]
    9074:	e3130c02 	tst	r3, #512	; 0x200
    9078:	0afffffc 	beq	9070 <iicreg_write+0xb0>
    907c:	e59f3018 	ldr	r3, [pc, #24]	; 909c <iicreg_write+0xdc>
    9080:	e3010964 	movw	r0, #6500	; 0x1964
    9084:	e5933060 	ldr	r3, [r3, #96]
    9088:	ebfffe62 	bl	8a18 <udelay>
    908c:	e3a00001 	mov	r0, #1	; 0x1
    9090:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    9094:	e3a00000 	mov	r0, #0	; 0x0
    9098:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    909c:	20300000 	.word	0x20300000

000090a0 <iic_read_func>:
    90a0:	e92d4070 	push	{r4, r5, r6, lr}
    90a4:	e59f4080 	ldr	r4, [pc, #128]	; 912c <iic_read_func+0x8c>
    90a8:	e1a06003 	mov	r6, r3
    90ac:	e5d43000 	ldrb	r3, [r4]
    90b0:	e24dd008 	sub	sp, sp, #8	; 0x8
    90b4:	e243300a 	sub	r3, r3, #10	; 0xa
    90b8:	e6ef3073 	uxtb	r3, r3
    90bc:	e3530001 	cmp	r3, #1	; 0x1
    90c0:	e1a05000 	mov	r5, r0
    90c4:	9a00000b 	bls	90f8 <iic_read_func+0x58>
    90c8:	e3a0e000 	mov	lr, #0	; 0x0
    90cc:	e1a0300e 	mov	r3, lr
    90d0:	e3a0c001 	mov	ip, #1	; 0x1
    90d4:	e58dc004 	str	ip, [sp, #4]
    90d8:	e58de000 	str	lr, [sp]
    90dc:	ebffffb7 	bl	8fc0 <iicreg_write>
    90e0:	e3500000 	cmp	r0, #0	; 0x0
    90e4:	03a0300c 	moveq	r3, #12	; 0xc
    90e8:	05c43000 	strbeq	r3, [r4]
    90ec:	0a00000c 	beq	9124 <iic_read_func+0x84>
    90f0:	e3a0300a 	mov	r3, #10	; 0xa
    90f4:	e5c43000 	strb	r3, [r4]
    90f8:	e59d2018 	ldr	r2, [sp, #24]
    90fc:	e1a00005 	mov	r0, r5
    9100:	e1a01006 	mov	r1, r6
    9104:	ebffff8e 	bl	8f44 <iicreg_read>
    9108:	e59f201c 	ldr	r2, [pc, #28]	; 912c <iic_read_func+0x8c>
    910c:	e3500000 	cmp	r0, #0	; 0x0
    9110:	13a0300d 	movne	r3, #13	; 0xd
    9114:	03a0300b 	moveq	r3, #11	; 0xb
    9118:	15c23000 	strbne	r3, [r2]
    911c:	13a00001 	movne	r0, #1	; 0x1
    9120:	05c23000 	strbeq	r3, [r2]
    9124:	e28dd008 	add	sp, sp, #8	; 0x8
    9128:	e8bd8070 	pop	{r4, r5, r6, pc}
    912c:	0800d204 	.word	0x0800d204

00009130 <iic_writeread>:
    9130:	e92d4070 	push	{r4, r5, r6, lr}
    9134:	e59f4088 	ldr	r4, [pc, #136]	; 91c4 <iic_writeread+0x94>
    9138:	e24dd010 	sub	sp, sp, #16	; 0x10
    913c:	e5d43000 	ldrb	r3, [r4]
    9140:	e5cd100f 	strb	r1, [sp, #15]
    9144:	e243300a 	sub	r3, r3, #10	; 0xa
    9148:	e6ef3073 	uxtb	r3, r3
    914c:	e3530001 	cmp	r3, #1	; 0x1
    9150:	e1a06002 	mov	r6, r2
    9154:	e1a05000 	mov	r5, r0
    9158:	9a00000c 	bls	9190 <iic_writeread+0x60>
    915c:	e3a0c000 	mov	ip, #0	; 0x0
    9160:	e1a0300c 	mov	r3, ip
    9164:	e28d100f 	add	r1, sp, #15	; 0xf
    9168:	e3a02001 	mov	r2, #1	; 0x1
    916c:	e58dc000 	str	ip, [sp]
    9170:	e58dc004 	str	ip, [sp, #4]
    9174:	ebffff91 	bl	8fc0 <iicreg_write>
    9178:	e3500000 	cmp	r0, #0	; 0x0
    917c:	03a0300c 	moveq	r3, #12	; 0xc
    9180:	05c43000 	strbeq	r3, [r4]
    9184:	0a00000c 	beq	91bc <iic_writeread+0x8c>
    9188:	e3a0300a 	mov	r3, #10	; 0xa
    918c:	e5c43000 	strb	r3, [r4]
    9190:	e3a02001 	mov	r2, #1	; 0x1
    9194:	e1a00005 	mov	r0, r5
    9198:	e1a01006 	mov	r1, r6
    919c:	ebffff68 	bl	8f44 <iicreg_read>
    91a0:	e59f201c 	ldr	r2, [pc, #28]	; 91c4 <iic_writeread+0x94>
    91a4:	e3500000 	cmp	r0, #0	; 0x0
    91a8:	13a0300d 	movne	r3, #13	; 0xd
    91ac:	03a0300b 	moveq	r3, #11	; 0xb
    91b0:	15c23000 	strbne	r3, [r2]
    91b4:	13a00001 	movne	r0, #1	; 0x1
    91b8:	05c23000 	strbeq	r3, [r2]
    91bc:	e28dd010 	add	sp, sp, #16	; 0x10
    91c0:	e8bd8070 	pop	{r4, r5, r6, pc}
    91c4:	0800d204 	.word	0x0800d204

000091c8 <iicreg_init>:
    91c8:	e92d4030 	push	{r4, r5, lr}
    91cc:	e1a04000 	mov	r4, r0
    91d0:	e24dd004 	sub	sp, sp, #4	; 0x4
    91d4:	e59f0074 	ldr	r0, [pc, #116]	; 9250 <iicreg_init+0x88>
    91d8:	e1a05002 	mov	r5, r2
    91dc:	ebffdfa5 	bl	1078 <module_get_clock>
    91e0:	e3540001 	cmp	r4, #1	; 0x1
    91e4:	83a00000 	movhi	r0, #0	; 0x0
    91e8:	8a000016 	bhi	9248 <iicreg_init+0x80>
    91ec:	e59f1060 	ldr	r1, [pc, #96]	; 9254 <iicreg_init+0x8c>
    91f0:	eb0017b8 	bl	f0d8 <__aeabi_uidiv>
    91f4:	e59f205c 	ldr	r2, [pc, #92]	; 9258 <iicreg_init+0x90>
    91f8:	e1a010a0 	lsr	r1, r0, #1
    91fc:	e3a0c000 	mov	ip, #0	; 0x0
    9200:	e3a03043 	mov	r3, #67	; 0x43
    9204:	e582c06c 	str	ip, [r2, #108]
    9208:	e5823000 	str	r3, [r2]
    920c:	e5820014 	str	r0, [r2, #20]
    9210:	e5820018 	str	r0, [r2, #24]
    9214:	e58210a0 	str	r1, [r2, #160]
    9218:	e59230a0 	ldr	r3, [r2, #160]
    921c:	e1a000a5 	lsr	r0, r5, #1
    9220:	e3c33c01 	bic	r3, r3, #256	; 0x100
    9224:	e58230a0 	str	r3, [r2, #160]
    9228:	e5923000 	ldr	r3, [r2]
    922c:	e3a01001 	mov	r1, #1	; 0x1
    9230:	e3833020 	orr	r3, r3, #32	; 0x20
    9234:	e5823000 	str	r3, [r2]
    9238:	e582c030 	str	ip, [r2, #48]
    923c:	e5820004 	str	r0, [r2, #4]
    9240:	e582106c 	str	r1, [r2, #108]
    9244:	e1a00001 	mov	r0, r1
    9248:	e28dd004 	add	sp, sp, #4	; 0x4
    924c:	e8bd8030 	pop	{r4, r5, pc}
    9250:	2d00a360 	.word	0x2d00a360
    9254:	0009c400 	.word	0x0009c400
    9258:	20300000 	.word	0x20300000

0000925c <iic_init>:
    925c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    9260:	e59fc058 	ldr	ip, [pc, #88]	; 92c0 <iic_init+0x64>
    9264:	e24dd004 	sub	sp, sp, #4	; 0x4
    9268:	e3a0e00d 	mov	lr, #13	; 0xd
    926c:	e1a0a000 	mov	sl, r0
    9270:	e59f004c 	ldr	r0, [pc, #76]	; 92c4 <iic_init+0x68>
    9274:	e1a04001 	mov	r4, r1
    9278:	e1a05002 	mov	r5, r2
    927c:	e1a06003 	mov	r6, r3
    9280:	e5cce000 	strb	lr, [ip]
    9284:	e5dd7020 	ldrb	r7, [sp, #32]
    9288:	e5dd8024 	ldrb	r8, [sp, #36]
    928c:	ebffdd2a 	bl	73c <module_enable>
    9290:	e3a01000 	mov	r1, #0	; 0x0
    9294:	e3a0000a 	mov	r0, #10	; 0xa
    9298:	ebffdd6d 	bl	854 <pads_init_module>
    929c:	e1a0000a 	mov	r0, sl
    92a0:	e1a01004 	mov	r1, r4
    92a4:	e1a02005 	mov	r2, r5
    92a8:	e1a03006 	mov	r3, r6
    92ac:	e58d7020 	str	r7, [sp, #32]
    92b0:	e58d8024 	str	r8, [sp, #36]
    92b4:	e28dd004 	add	sp, sp, #4	; 0x4
    92b8:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    92bc:	eaffffc1 	b	91c8 <iicreg_init>
    92c0:	0800d204 	.word	0x0800d204
    92c4:	2d010c00 	.word	0x2d010c00

000092c8 <spi_set_bytes>:
    92c8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    92cc:	e59f3020 	ldr	r3, [pc, #32]	; 92f4 <spi_set_bytes+0x2c>
    92d0:	e1a01000 	mov	r1, r0
    92d4:	e5830000 	str	r0, [r3]
    92d8:	e24dd004 	sub	sp, sp, #4	; 0x4
    92dc:	e59f0014 	ldr	r0, [pc, #20]	; 92f8 <spi_set_bytes+0x30>
    92e0:	e59f3014 	ldr	r3, [pc, #20]	; 92fc <spi_set_bytes+0x34>
    92e4:	e1a0e00f 	mov	lr, pc
    92e8:	e593f000 	ldr	pc, [r3]
    92ec:	e28dd004 	add	sp, sp, #4	; 0x4
    92f0:	e8bd8000 	pop	{pc}
    92f4:	0800d0e0 	.word	0x0800d0e0
    92f8:	00012322 	.word	0x00012322
    92fc:	0800d704 	.word	0x0800d704

00009300 <spi_fifo_clear>:
    9300:	e59f2028 	ldr	r2, [pc, #40]	; 9330 <spi_fifo_clear+0x30>
    9304:	e592300c 	ldr	r3, [r2, #12]
    9308:	e3130010 	tst	r3, #16	; 0x10
    930c:	1afffffc 	bne	9304 <spi_fifo_clear+0x4>
    9310:	ea000001 	b	931c <spi_fifo_clear+0x1c>
    9314:	e5923008 	ldr	r3, [r2, #8]
    9318:	ea000000 	b	9320 <spi_fifo_clear+0x20>
    931c:	e59f200c 	ldr	r2, [pc, #12]	; 9330 <spi_fifo_clear+0x30>
    9320:	e592300c 	ldr	r3, [r2, #12]
    9324:	e3130004 	tst	r3, #4	; 0x4
    9328:	1afffff9 	bne	9314 <spi_fifo_clear+0x14>
    932c:	e12fff1e 	bx	lr
    9330:	28500000 	.word	0x28500000

00009334 <flash_vs_align>:
    9334:	e3a00c01 	mov	r0, #256	; 0x100
    9338:	e12fff1e 	bx	lr

0000933c <spi_exchange>:
    933c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    9340:	e5905008 	ldr	r5, [r0, #8]
    9344:	e3510001 	cmp	r1, #1	; 0x1
    9348:	e24dd00c 	sub	sp, sp, #12	; 0xc
    934c:	e1a0a000 	mov	sl, r0
    9350:	e5908004 	ldr	r8, [r0, #4]
    9354:	d1a04005 	movle	r4, r5
    9358:	d3a07000 	movle	r7, #0	; 0x0
    935c:	da000020 	ble	93e4 <spi_exchange+0xa8>
    9360:	e280a00c 	add	sl, r0, #12	; 0xc
    9364:	e1a01008 	mov	r1, r8
    9368:	e1a0000d 	mov	r0, sp
    936c:	e1a02005 	mov	r2, r5
    9370:	ebffe018 	bl	13d8 <memcpy>
    9374:	e99a0012 	ldmib	sl, {r1, r4}
    9378:	e0843005 	add	r3, r4, r5
    937c:	e3530008 	cmp	r3, #8	; 0x8
    9380:	a3a03008 	movge	r3, #8	; 0x8
    9384:	e6ef6073 	uxtb	r6, r3
    9388:	e3510000 	cmp	r1, #0	; 0x0
    938c:	e1a0700d 	mov	r7, sp
    9390:	059a8000 	ldreq	r8, [sl]
    9394:	00667084 	rsbeq	r7, r6, r4, lsl #1
    9398:	0a000008 	beq	93c0 <spi_exchange+0x84>
    939c:	e0654006 	rsb	r4, r5, r6
    93a0:	e0870005 	add	r0, r7, r5
    93a4:	e1a02004 	mov	r2, r4
    93a8:	ebffe00a 	bl	13d8 <memcpy>
    93ac:	e99a000c 	ldmib	sl, {r2, r3}
    93b0:	e0855003 	add	r5, r5, r3
    93b4:	e0828004 	add	r8, r2, r4
    93b8:	e3a07000 	mov	r7, #0	; 0x0
    93bc:	e0664005 	rsb	r4, r6, r5
    93c0:	e3a02000 	mov	r2, #0	; 0x0
    93c4:	e1a0000d 	mov	r0, sp
    93c8:	e59f126c 	ldr	r1, [pc, #620]	; 963c <spi_exchange+0x300>
    93cc:	ea000002 	b	93dc <spi_exchange+0xa0>
    93d0:	e7d03002 	ldrb	r3, [r0, r2]
    93d4:	e2822001 	add	r2, r2, #1	; 0x1
    93d8:	e5813008 	str	r3, [r1, #8]
    93dc:	e1520006 	cmp	r2, r6
    93e0:	bafffffa 	blt	93d0 <spi_exchange+0x94>
    93e4:	e59f6250 	ldr	r6, [pc, #592]	; 963c <spi_exchange+0x300>
    93e8:	e3a0b005 	mov	fp, #5	; 0x5
    93ec:	e5969018 	ldr	r9, [r6, #24]
    93f0:	e3190004 	tst	r9, #4	; 0x4
    93f4:	0a000030 	beq	94bc <spi_exchange+0x180>
    93f8:	e3550003 	cmp	r5, #3	; 0x3
    93fc:	da000005 	ble	9418 <spi_exchange+0xdc>
    9400:	e5963008 	ldr	r3, [r6, #8]
    9404:	e2455004 	sub	r5, r5, #4	; 0x4
    9408:	e5963008 	ldr	r3, [r6, #8]
    940c:	e5963008 	ldr	r3, [r6, #8]
    9410:	e5963008 	ldr	r3, [r6, #8]
    9414:	ea000028 	b	94bc <spi_exchange+0x180>
    9418:	e3550000 	cmp	r5, #0	; 0x0
    941c:	0a000003 	beq	9430 <spi_exchange+0xf4>
    9420:	e2555001 	subs	r5, r5, #1	; 0x1
    9424:	e5963008 	ldr	r3, [r6, #8]
    9428:	1afffffc 	bne	9420 <spi_exchange+0xe4>
    942c:	ea000022 	b	94bc <spi_exchange+0x180>
    9430:	e59a3000 	ldr	r3, [sl]
    9434:	e3530000 	cmp	r3, #0	; 0x0
    9438:	1a000004 	bne	9450 <spi_exchange+0x114>
    943c:	e59f01fc 	ldr	r0, [pc, #508]	; 9640 <spi_exchange+0x304>
    9440:	e59f31fc 	ldr	r3, [pc, #508]	; 9644 <spi_exchange+0x308>
    9444:	e1a0e00f 	mov	lr, pc
    9448:	e593f000 	ldr	pc, [r3]
    944c:	ea00001a 	b	94bc <spi_exchange+0x180>
    9450:	e3540003 	cmp	r4, #3	; 0x3
    9454:	da00000e 	ble	9494 <spi_exchange+0x158>
    9458:	e5962008 	ldr	r2, [r6, #8]
    945c:	e1a03008 	mov	r3, r8
    9460:	e4c32001 	strb	r2, [r3], #1
    9464:	e5962008 	ldr	r2, [r6, #8]
    9468:	e2831001 	add	r1, r3, #1	; 0x1
    946c:	e5c82001 	strb	r2, [r8, #1]
    9470:	e5962008 	ldr	r2, [r6, #8]
    9474:	e2544004 	subs	r4, r4, #4	; 0x4
    9478:	e5c32001 	strb	r2, [r3, #1]
    947c:	e5963008 	ldr	r3, [r6, #8]
    9480:	e2812001 	add	r2, r1, #1	; 0x1
    9484:	e5c13001 	strb	r3, [r1, #1]
    9488:	0a00005e 	beq	9608 <spi_exchange+0x2cc>
    948c:	e2828001 	add	r8, r2, #1	; 0x1
    9490:	ea000009 	b	94bc <spi_exchange+0x180>
    9494:	e3540000 	cmp	r4, #0	; 0x0
    9498:	0a00005a 	beq	9608 <spi_exchange+0x2cc>
    949c:	e59f2198 	ldr	r2, [pc, #408]	; 963c <spi_exchange+0x300>
    94a0:	e1a01005 	mov	r1, r5
    94a4:	e5923008 	ldr	r3, [r2, #8]
    94a8:	e7c83001 	strb	r3, [r8, r1]
    94ac:	e2811001 	add	r1, r1, #1	; 0x1
    94b0:	e1510004 	cmp	r1, r4
    94b4:	1afffffa 	bne	94a4 <spi_exchange+0x168>
    94b8:	ea000052 	b	9608 <spi_exchange+0x2cc>
    94bc:	e3190008 	tst	r9, #8	; 0x8
    94c0:	0a00002a 	beq	9570 <spi_exchange+0x234>
    94c4:	e59a3004 	ldr	r3, [sl, #4]
    94c8:	e3530000 	cmp	r3, #0	; 0x0
    94cc:	0a000019 	beq	9538 <spi_exchange+0x1fc>
    94d0:	e3540003 	cmp	r4, #3	; 0x3
    94d4:	da00000d 	ble	9510 <spi_exchange+0x1d4>
    94d8:	e1a03008 	mov	r3, r8
    94dc:	e4d32001 	ldrb	r2, [r3], #1
    94e0:	e2544004 	subs	r4, r4, #4	; 0x4
    94e4:	e5862008 	str	r2, [r6, #8]
    94e8:	e5d82001 	ldrb	r2, [r8, #1]
    94ec:	e2831001 	add	r1, r3, #1	; 0x1
    94f0:	e5862008 	str	r2, [r6, #8]
    94f4:	e5d33001 	ldrb	r3, [r3, #1]
    94f8:	e2818002 	add	r8, r1, #2	; 0x2
    94fc:	e5863008 	str	r3, [r6, #8]
    9500:	e5d13001 	ldrb	r3, [r1, #1]
    9504:	e5863008 	str	r3, [r6, #8]
    9508:	1a000018 	bne	9570 <spi_exchange+0x234>
    950c:	ea00003d 	b	9608 <spi_exchange+0x2cc>
    9510:	e3540000 	cmp	r4, #0	; 0x0
    9514:	0a00003b 	beq	9608 <spi_exchange+0x2cc>
    9518:	e59f111c 	ldr	r1, [pc, #284]	; 963c <spi_exchange+0x300>
    951c:	e3a02000 	mov	r2, #0	; 0x0
    9520:	e7d83002 	ldrb	r3, [r8, r2]
    9524:	e2822001 	add	r2, r2, #1	; 0x1
    9528:	e1520004 	cmp	r2, r4
    952c:	e5813008 	str	r3, [r1, #8]
    9530:	1afffffa 	bne	9520 <spi_exchange+0x1e4>
    9534:	ea000033 	b	9608 <spi_exchange+0x2cc>
    9538:	e3570003 	cmp	r7, #3	; 0x3
    953c:	da000005 	ble	9558 <spi_exchange+0x21c>
    9540:	e586b008 	str	fp, [r6, #8]
    9544:	e2477004 	sub	r7, r7, #4	; 0x4
    9548:	e586b008 	str	fp, [r6, #8]
    954c:	e586b008 	str	fp, [r6, #8]
    9550:	e586b008 	str	fp, [r6, #8]
    9554:	ea000005 	b	9570 <spi_exchange+0x234>
    9558:	e3570000 	cmp	r7, #0	; 0x0
    955c:	0a000003 	beq	9570 <spi_exchange+0x234>
    9560:	e3a03005 	mov	r3, #5	; 0x5
    9564:	e2577001 	subs	r7, r7, #1	; 0x1
    9568:	e5863008 	str	r3, [r6, #8]
    956c:	1afffffb 	bne	9560 <spi_exchange+0x224>
    9570:	e3190002 	tst	r9, #2	; 0x2
    9574:	0affff9c 	beq	93ec <spi_exchange+0xb0>
    9578:	ea000009 	b	95a4 <spi_exchange+0x268>
    957c:	e596300c 	ldr	r3, [r6, #12]
    9580:	e2455001 	sub	r5, r5, #1	; 0x1
    9584:	e3130004 	tst	r3, #4	; 0x4
    9588:	15963008 	ldrne	r3, [r6, #8]
    958c:	1a000006 	bne	95ac <spi_exchange+0x270>
    9590:	e59f00b0 	ldr	r0, [pc, #176]	; 9648 <spi_exchange+0x30c>
    9594:	e1a01005 	mov	r1, r5
    9598:	e1a0e00f 	mov	lr, pc
    959c:	e597f000 	ldr	pc, [r7]
    95a0:	ea000001 	b	95ac <spi_exchange+0x270>
    95a4:	e59f6090 	ldr	r6, [pc, #144]	; 963c <spi_exchange+0x300>
    95a8:	e59f7094 	ldr	r7, [pc, #148]	; 9644 <spi_exchange+0x308>
    95ac:	e3550000 	cmp	r5, #0	; 0x0
    95b0:	1afffff1 	bne	957c <spi_exchange+0x240>
    95b4:	ea00000a 	b	95e4 <spi_exchange+0x2a8>
    95b8:	e595300c 	ldr	r3, [r5, #12]
    95bc:	e2444001 	sub	r4, r4, #1	; 0x1
    95c0:	e3130004 	tst	r3, #4	; 0x4
    95c4:	15953008 	ldrne	r3, [r5, #8]
    95c8:	14c83001 	strbne	r3, [r8], #1
    95cc:	1a000006 	bne	95ec <spi_exchange+0x2b0>
    95d0:	e59f0074 	ldr	r0, [pc, #116]	; 964c <spi_exchange+0x310>
    95d4:	e1a01004 	mov	r1, r4
    95d8:	e1a0e00f 	mov	lr, pc
    95dc:	e596f000 	ldr	pc, [r6]
    95e0:	ea000001 	b	95ec <spi_exchange+0x2b0>
    95e4:	e59f5050 	ldr	r5, [pc, #80]	; 963c <spi_exchange+0x300>
    95e8:	e59f6054 	ldr	r6, [pc, #84]	; 9644 <spi_exchange+0x308>
    95ec:	e59a3000 	ldr	r3, [sl]
    95f0:	e3530000 	cmp	r3, #0	; 0x0
    95f4:	13540000 	cmpne	r4, #0	; 0x0
    95f8:	1affffee 	bne	95b8 <spi_exchange+0x27c>
    95fc:	e59f3038 	ldr	r3, [pc, #56]	; 963c <spi_exchange+0x300>
    9600:	e3a02003 	mov	r2, #3	; 0x3
    9604:	e5832020 	str	r2, [r3, #32]
    9608:	e59f202c 	ldr	r2, [pc, #44]	; 963c <spi_exchange+0x300>
    960c:	e592300c 	ldr	r3, [r2, #12]
    9610:	e3130010 	tst	r3, #16	; 0x10
    9614:	1afffffc 	bne	960c <spi_exchange+0x2d0>
    9618:	ea000001 	b	9624 <spi_exchange+0x2e8>
    961c:	e5923008 	ldr	r3, [r2, #8]
    9620:	ea000000 	b	9628 <spi_exchange+0x2ec>
    9624:	e59f2010 	ldr	r2, [pc, #16]	; 963c <spi_exchange+0x300>
    9628:	e592300c 	ldr	r3, [r2, #12]
    962c:	e3130004 	tst	r3, #4	; 0x4
    9630:	1afffff9 	bne	961c <spi_exchange+0x2e0>
    9634:	e28dd00c 	add	sp, sp, #12	; 0xc
    9638:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    963c:	28500000 	.word	0x28500000
    9640:	00012339 	.word	0x00012339
    9644:	0800d704 	.word	0x0800d704
    9648:	00012350 	.word	0x00012350
    964c:	00012368 	.word	0x00012368

00009650 <flash_vs_read>:
    9650:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    9654:	e24dd024 	sub	sp, sp, #36	; 0x24
    9658:	e28d4018 	add	r4, sp, #24	; 0x18
    965c:	e2841003 	add	r1, r4, #3	; 0x3
    9660:	e59da040 	ldr	sl, [sp, #64]
    9664:	e58d1004 	str	r1, [sp, #4]
    9668:	e3a01005 	mov	r1, #5	; 0x5
    966c:	e3a0c000 	mov	ip, #0	; 0x0
    9670:	e58d1008 	str	r1, [sp, #8]
    9674:	e2811006 	add	r1, r1, #6	; 0x6
    9678:	e58dc018 	str	ip, [sp, #24]
    967c:	e2845004 	add	r5, r4, #4	; 0x4
    9680:	e58d000c 	str	r0, [sp, #12]
    9684:	e58dc010 	str	ip, [sp, #16]
    9688:	e5cd101b 	strb	r1, [sp, #27]
    968c:	e1a06002 	mov	r6, r2
    9690:	e1a07003 	mov	r7, r3
    9694:	e58dc000 	str	ip, [sp]
    9698:	e1a0400a 	mov	r4, sl
    969c:	e1a0800d 	mov	r8, sp
    96a0:	ea00000c 	b	96d8 <flash_vs_read+0x88>
    96a4:	e3540a01 	cmp	r4, #4096	; 0x1000
    96a8:	b1a03004 	movlt	r3, r4
    96ac:	a3a03a01 	movge	r3, #4096	; 0x1000
    96b0:	e5852000 	str	r2, [r5]
    96b4:	e58d3014 	str	r3, [sp, #20]
    96b8:	ebffff1f 	bl	933c <spi_exchange>
    96bc:	e59d2014 	ldr	r2, [sp, #20]
    96c0:	e59d300c 	ldr	r3, [sp, #12]
    96c4:	e0966002 	adds	r6, r6, r2
    96c8:	e0833002 	add	r3, r3, r2
    96cc:	e0a77fc2 	adc	r7, r7, r2, asr #31
    96d0:	e58d300c 	str	r3, [sp, #12]
    96d4:	e0624004 	rsb	r4, r2, r4
    96d8:	e1a02c06 	lsl	r2, r6, #24
    96dc:	e2061cff 	and	r1, r6, #65280	; 0xff00
    96e0:	e1822c26 	orr	r2, r2, r6, lsr #24
    96e4:	e1822401 	orr	r2, r2, r1, lsl #8
    96e8:	e20638ff 	and	r3, r6, #16711680	; 0xff0000
    96ec:	e1822423 	orr	r2, r2, r3, lsr #8
    96f0:	e3540000 	cmp	r4, #0	; 0x0
    96f4:	e1a02442 	asr	r2, r2, #8
    96f8:	e1a0000d 	mov	r0, sp
    96fc:	e3a01002 	mov	r1, #2	; 0x2
    9700:	1affffe7 	bne	96a4 <flash_vs_read+0x54>
    9704:	e1a0000a 	mov	r0, sl
    9708:	e28dd024 	add	sp, sp, #36	; 0x24
    970c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00009710 <flash_reset>:
    9710:	e92d4030 	push	{r4, r5, lr}
    9714:	e24dd024 	sub	sp, sp, #36	; 0x24
    9718:	e3a04001 	mov	r4, #1	; 0x1
    971c:	e28dc018 	add	ip, sp, #24	; 0x18
    9720:	e08c3004 	add	r3, ip, r4
    9724:	e3a02000 	mov	r2, #0	; 0x0
    9728:	e58d3010 	str	r3, [sp, #16]
    972c:	e3a03066 	mov	r3, #102	; 0x66
    9730:	e58d2018 	str	r2, [sp, #24]
    9734:	e1a0000d 	mov	r0, sp
    9738:	e1a01004 	mov	r1, r4
    973c:	e5cd3018 	strb	r3, [sp, #24]
    9740:	e3e03066 	mvn	r3, #102	; 0x66
    9744:	e58d200c 	str	r2, [sp, #12]
    9748:	e5cd3019 	strb	r3, [sp, #25]
    974c:	e58d201c 	str	r2, [sp, #28]
    9750:	e88d1004 	stm	sp, {r2, ip}
    9754:	e58d4008 	str	r4, [sp, #8]
    9758:	e58d4014 	str	r4, [sp, #20]
    975c:	ebfffef6 	bl	933c <spi_exchange>
    9760:	e28d000c 	add	r0, sp, #12	; 0xc
    9764:	e1a01004 	mov	r1, r4
    9768:	ebfffef3 	bl	933c <spi_exchange>
    976c:	e3a0003c 	mov	r0, #60	; 0x3c
    9770:	e1a0500d 	mov	r5, sp
    9774:	ebfffca7 	bl	8a18 <udelay>
    9778:	e28dd024 	add	sp, sp, #36	; 0x24
    977c:	e8bd8030 	pop	{r4, r5, pc}

00009780 <flash_poll>:
    9780:	e92d4030 	push	{r4, r5, lr}
    9784:	e24dd024 	sub	sp, sp, #36	; 0x24
    9788:	e28dc018 	add	ip, sp, #24	; 0x18
    978c:	e28c3001 	add	r3, ip, #1	; 0x1
    9790:	e3a02000 	mov	r2, #0	; 0x0
    9794:	e3a01001 	mov	r1, #1	; 0x1
    9798:	e58d300c 	str	r3, [sp, #12]
    979c:	e3a03005 	mov	r3, #5	; 0x5
    97a0:	e58d2018 	str	r2, [sp, #24]
    97a4:	e58d2010 	str	r2, [sp, #16]
    97a8:	e58d1014 	str	r1, [sp, #20]
    97ac:	e5cd3018 	strb	r3, [sp, #24]
    97b0:	e1a04000 	mov	r4, r0
    97b4:	e58d201c 	str	r2, [sp, #28]
    97b8:	e88d1004 	stm	sp, {r2, ip}
    97bc:	e58d1008 	str	r1, [sp, #8]
    97c0:	e1a0500d 	mov	r5, sp
    97c4:	e1a0000d 	mov	r0, sp
    97c8:	e3a01002 	mov	r1, #2	; 0x2
    97cc:	ebfffeda 	bl	933c <spi_exchange>
    97d0:	e5dd3019 	ldrb	r3, [sp, #25]
    97d4:	e3540000 	cmp	r4, #0	; 0x0
    97d8:	11a030a3 	lsrne	r3, r3, #1
    97dc:	12233001 	eorne	r3, r3, #1	; 0x1
    97e0:	e2033001 	and	r3, r3, #1	; 0x1
    97e4:	e3530000 	cmp	r3, #0	; 0x0
    97e8:	1afffff5 	bne	97c4 <flash_poll+0x44>
    97ec:	e28dd024 	add	sp, sp, #36	; 0x24
    97f0:	e8bd8030 	pop	{r4, r5, pc}

000097f4 <flash_wren>:
    97f4:	e92d4010 	push	{r4, lr}
    97f8:	e24dd020 	sub	sp, sp, #32	; 0x20
    97fc:	e3a04001 	mov	r4, #1	; 0x1
    9800:	e28dc018 	add	ip, sp, #24	; 0x18
    9804:	e08c3004 	add	r3, ip, r4
    9808:	e3a02000 	mov	r2, #0	; 0x0
    980c:	e58d3010 	str	r3, [sp, #16]
    9810:	e3a03006 	mov	r3, #6	; 0x6
    9814:	e58d2018 	str	r2, [sp, #24]
    9818:	e1a0000d 	mov	r0, sp
    981c:	e5cd3018 	strb	r3, [sp, #24]
    9820:	e1a01004 	mov	r1, r4
    9824:	e3a03004 	mov	r3, #4	; 0x4
    9828:	e58d200c 	str	r2, [sp, #12]
    982c:	e5cd3019 	strb	r3, [sp, #25]
    9830:	e58d201c 	str	r2, [sp, #28]
    9834:	e88d1004 	stm	sp, {r2, ip}
    9838:	e58d4008 	str	r4, [sp, #8]
    983c:	e58d4014 	str	r4, [sp, #20]
    9840:	ebfffebd 	bl	933c <spi_exchange>
    9844:	e1a00004 	mov	r0, r4
    9848:	ebffffcc 	bl	9780 <flash_poll>
    984c:	e28dd020 	add	sp, sp, #32	; 0x20
    9850:	e8bd8010 	pop	{r4, pc}

00009854 <flash_vs_erase>:
    9854:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    9858:	e24dd034 	sub	sp, sp, #52	; 0x34
    985c:	e28d4028 	add	r4, sp, #40	; 0x28
    9860:	e3a06001 	mov	r6, #1	; 0x1
    9864:	e59f109c 	ldr	r1, [pc, #156]	; 9908 <flash_vs_erase+0xb4>
    9868:	e3a02008 	mov	r2, #8	; 0x8
    986c:	e1a00004 	mov	r0, r4
    9870:	ebffded8 	bl	13d8 <memcpy>
    9874:	e0843006 	add	r3, r4, r6
    9878:	e2842003 	add	r2, r4, #3	; 0x3
    987c:	e3a05000 	mov	r5, #0	; 0x0
    9880:	e28d7004 	add	r7, sp, #4	; 0x4
    9884:	e58d3008 	str	r3, [sp, #8]
    9888:	e3a03002 	mov	r3, #2	; 0x2
    988c:	e58d300c 	str	r3, [sp, #12]
    9890:	e58d2014 	str	r2, [sp, #20]
    9894:	e58d4020 	str	r4, [sp, #32]
    9898:	e58d5004 	str	r5, [sp, #4]
    989c:	e58d5010 	str	r5, [sp, #16]
    98a0:	e58d6018 	str	r6, [sp, #24]
    98a4:	e58d501c 	str	r5, [sp, #28]
    98a8:	e58d6024 	str	r6, [sp, #36]
    98ac:	ebffffd0 	bl	97f4 <flash_wren>
    98b0:	e1a01006 	mov	r1, r6
    98b4:	e1a00007 	mov	r0, r7
    98b8:	ebfffe9f 	bl	933c <spi_exchange>
    98bc:	e1a00005 	mov	r0, r5
    98c0:	ebffffae 	bl	9780 <flash_poll>
    98c4:	e2870018 	add	r0, r7, #24	; 0x18
    98c8:	e1a01006 	mov	r1, r6
    98cc:	ebfffe9a 	bl	933c <spi_exchange>
    98d0:	e1a01006 	mov	r1, r6
    98d4:	e1a00007 	mov	r0, r7
    98d8:	ebfffe97 	bl	933c <spi_exchange>
    98dc:	e1a00005 	mov	r0, r5
    98e0:	ebffffa6 	bl	9780 <flash_poll>
    98e4:	ebffffc2 	bl	97f4 <flash_wren>
    98e8:	e1a01006 	mov	r1, r6
    98ec:	e287000c 	add	r0, r7, #12	; 0xc
    98f0:	ebfffe91 	bl	933c <spi_exchange>
    98f4:	e1a00005 	mov	r0, r5
    98f8:	ebffffa0 	bl	9780 <flash_poll>
    98fc:	e1a00005 	mov	r0, r5
    9900:	e28dd034 	add	sp, sp, #52	; 0x34
    9904:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    9908:	000107f6 	.word	0x000107f6

0000990c <flash_vs_write>:
    990c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    9910:	e3a01004 	mov	r1, #4	; 0x4
    9914:	e24dd020 	sub	sp, sp, #32	; 0x20
    9918:	e3a0c000 	mov	ip, #0	; 0x0
    991c:	e59d9040 	ldr	r9, [sp, #64]
    9920:	e58d1008 	str	r1, [sp, #8]
    9924:	e2811002 	add	r1, r1, #2	; 0x2
    9928:	e58dc018 	str	ip, [sp, #24]
    992c:	e28d701c 	add	r7, sp, #28	; 0x1c
    9930:	e5cd1018 	strb	r1, [sp, #24]
    9934:	e3a01002 	mov	r1, #2	; 0x2
    9938:	e58dc00c 	str	ip, [sp, #12]
    993c:	e58d0010 	str	r0, [sp, #16]
    9940:	e5cd101b 	strb	r1, [sp, #27]
    9944:	e1a04002 	mov	r4, r2
    9948:	e1a05003 	mov	r5, r3
    994c:	e58dc000 	str	ip, [sp]
    9950:	e58d7004 	str	r7, [sp, #4]
    9954:	e1a06009 	mov	r6, r9
    9958:	e59fa080 	ldr	sl, [pc, #128]	; 99e0 <flash_vs_write+0xd4>
    995c:	e1a0800d 	mov	r8, sp
    9960:	ea000011 	b	99ac <flash_vs_write+0xa0>
    9964:	e59a3000 	ldr	r3, [sl]
    9968:	e5872000 	str	r2, [r7]
    996c:	e1560003 	cmp	r6, r3
    9970:	d58d6014 	strle	r6, [sp, #20]
    9974:	c58d3014 	strgt	r3, [sp, #20]
    9978:	ebffff9d 	bl	97f4 <flash_wren>
    997c:	e1a0000d 	mov	r0, sp
    9980:	e3a01002 	mov	r1, #2	; 0x2
    9984:	ebfffe6c 	bl	933c <spi_exchange>
    9988:	e3a00000 	mov	r0, #0	; 0x0
    998c:	ebffff7b 	bl	9780 <flash_poll>
    9990:	e59d2014 	ldr	r2, [sp, #20]
    9994:	e59d3010 	ldr	r3, [sp, #16]
    9998:	e0944002 	adds	r4, r4, r2
    999c:	e0833002 	add	r3, r3, r2
    99a0:	e0a55fc2 	adc	r5, r5, r2, asr #31
    99a4:	e58d3010 	str	r3, [sp, #16]
    99a8:	e0626006 	rsb	r6, r2, r6
    99ac:	e1a02c04 	lsl	r2, r4, #24
    99b0:	e1822c24 	orr	r2, r2, r4, lsr #24
    99b4:	e2041cff 	and	r1, r4, #65280	; 0xff00
    99b8:	e1822401 	orr	r2, r2, r1, lsl #8
    99bc:	e20438ff 	and	r3, r4, #16711680	; 0xff0000
    99c0:	e1822423 	orr	r2, r2, r3, lsr #8
    99c4:	e3c220ff 	bic	r2, r2, #255	; 0xff
    99c8:	e3560000 	cmp	r6, #0	; 0x0
    99cc:	e3822002 	orr	r2, r2, #2	; 0x2
    99d0:	1affffe3 	bne	9964 <flash_vs_write+0x58>
    99d4:	e1a00009 	mov	r0, r9
    99d8:	e28dd020 	add	sp, sp, #32	; 0x20
    99dc:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    99e0:	0800d0e0 	.word	0x0800d0e0

000099e4 <spi_init_pads>:
    99e4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    99e8:	e3a01000 	mov	r1, #0	; 0x0
    99ec:	e24dd004 	sub	sp, sp, #4	; 0x4
    99f0:	e3a0000b 	mov	r0, #11	; 0xb
    99f4:	ebffdb96 	bl	854 <pads_init_module>
    99f8:	e3a00000 	mov	r0, #0	; 0x0
    99fc:	e28dd004 	add	sp, sp, #4	; 0x4
    9a00:	e8bd8000 	pop	{pc}

00009a04 <spi_init_clk>:
    9a04:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    9a08:	e59f00f8 	ldr	r0, [pc, #248]	; 9b08 <spi_init_clk+0x104>
    9a0c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    9a10:	ebffdd98 	bl	1078 <module_get_clock>
    9a14:	e59f20f0 	ldr	r2, [pc, #240]	; 9b0c <spi_init_clk+0x108>
    9a18:	e1a08000 	mov	r8, r0
    9a1c:	e5923404 	ldr	r3, [r2, #1028]
    9a20:	e3a05002 	mov	r5, #2	; 0x2
    9a24:	e3833008 	orr	r3, r3, #8	; 0x8
    9a28:	e59fa0e0 	ldr	sl, [pc, #224]	; 9b10 <spi_init_clk+0x10c>
    9a2c:	e5823404 	str	r3, [r2, #1028]
    9a30:	ea00000c 	b	9a68 <spi_init_clk+0x64>
    9a34:	e1a06004 	mov	r6, r4
    9a38:	e1a01007 	mov	r1, r7
    9a3c:	e1a00008 	mov	r0, r8
    9a40:	eb001626 	bl	f2e0 <__aeabi_idiv>
    9a44:	e150000a 	cmp	r0, sl
    9a48:	e0877005 	add	r7, r7, r5
    9a4c:	e2864001 	add	r4, r6, #1	; 0x1
    9a50:	da000007 	ble	9a74 <spi_init_clk+0x70>
    9a54:	e3540c01 	cmp	r4, #256	; 0x100
    9a58:	1afffff5 	bne	9a34 <spi_init_clk+0x30>
    9a5c:	e2855002 	add	r5, r5, #2	; 0x2
    9a60:	e3550c01 	cmp	r5, #256	; 0x100
    9a64:	0a000003 	beq	9a78 <spi_init_clk+0x74>
    9a68:	e1a07005 	mov	r7, r5
    9a6c:	e3a06000 	mov	r6, #0	; 0x0
    9a70:	eafffff0 	b	9a38 <spi_init_clk+0x34>
    9a74:	e1a04006 	mov	r4, r6
    9a78:	e35400ff 	cmp	r4, #255	; 0xff
    9a7c:	d35500fe 	cmple	r5, #254	; 0xfe
    9a80:	da000007 	ble	9aa4 <spi_init_clk+0xa0>
    9a84:	e1a01005 	mov	r1, r5
    9a88:	e1a02004 	mov	r2, r4
    9a8c:	e59f0080 	ldr	r0, [pc, #128]	; 9b14 <spi_init_clk+0x110>
    9a90:	e59f3080 	ldr	r3, [pc, #128]	; 9b18 <spi_init_clk+0x114>
    9a94:	e1a0e00f 	mov	lr, pc
    9a98:	e593f000 	ldr	pc, [r3]
    9a9c:	e3a05002 	mov	r5, #2	; 0x2
    9aa0:	e3a04027 	mov	r4, #39	; 0x27
    9aa4:	e2841001 	add	r1, r4, #1	; 0x1
    9aa8:	e0010195 	mul	r1, r5, r1
    9aac:	e1a00008 	mov	r0, r8
    9ab0:	eb00160a 	bl	f2e0 <__aeabi_idiv>
    9ab4:	e1a01008 	mov	r1, r8
    9ab8:	e58d0000 	str	r0, [sp]
    9abc:	e1a02005 	mov	r2, r5
    9ac0:	e1a03004 	mov	r3, r4
    9ac4:	e59f0050 	ldr	r0, [pc, #80]	; 9b1c <spi_init_clk+0x118>
    9ac8:	e59fc048 	ldr	ip, [pc, #72]	; 9b18 <spi_init_clk+0x114>
    9acc:	e1a0e00f 	mov	lr, pc
    9ad0:	e59cf000 	ldr	pc, [ip]
    9ad4:	e59f2044 	ldr	r2, [pc, #68]	; 9b20 <spi_init_clk+0x11c>
    9ad8:	e3a03000 	mov	r3, #0	; 0x0
    9adc:	e1a01404 	lsl	r1, r4, #8
    9ae0:	e5823004 	str	r3, [r2, #4]
    9ae4:	e38110c7 	orr	r1, r1, #199	; 0xc7
    9ae8:	e5823014 	str	r3, [r2, #20]
    9aec:	e1a00003 	mov	r0, r3
    9af0:	e283300a 	add	r3, r3, #10	; 0xa
    9af4:	e5825010 	str	r5, [r2, #16]
    9af8:	e5821000 	str	r1, [r2]
    9afc:	e5823004 	str	r3, [r2, #4]
    9b00:	e28dd00c 	add	sp, sp, #12	; 0xc
    9b04:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    9b08:	2d00a2a0 	.word	0x2d00a2a0
    9b0c:	2b011000 	.word	0x2b011000
    9b10:	00061a80 	.word	0x00061a80
    9b14:	00012380 	.word	0x00012380
    9b18:	0800d704 	.word	0x0800d704
    9b1c:	000123ae 	.word	0x000123ae
    9b20:	28500000 	.word	0x28500000

00009b24 <spi_sequence_brd_reboot>:
    9b24:	e92d4010 	push	{r4, lr}
    9b28:	ebffdb22 	bl	7b8 <get_reset_stats>
    9b2c:	e3a01002 	mov	r1, #2	; 0x2
    9b30:	e1a04000 	mov	r4, r0
    9b34:	e3a02001 	mov	r2, #1	; 0x1
    9b38:	e3a00004 	mov	r0, #4	; 0x4
    9b3c:	e3a03000 	mov	r3, #0	; 0x0
    9b40:	ebffdc1c 	bl	bb8 <pads_chmod>
    9b44:	e3540000 	cmp	r4, #0	; 0x0
    9b48:	08bd8010 	popeq	{r4, pc}
    9b4c:	e3a01002 	mov	r1, #2	; 0x2
    9b50:	e3a02000 	mov	r2, #0	; 0x0
    9b54:	e3a03001 	mov	r3, #1	; 0x1
    9b58:	e3a0005f 	mov	r0, #95	; 0x5f
    9b5c:	ebffdc15 	bl	bb8 <pads_chmod>
    9b60:	e3020710 	movw	r0, #10000	; 0x2710
    9b64:	ebfffbab 	bl	8a18 <udelay>
    9b68:	e3a01001 	mov	r1, #1	; 0x1
    9b6c:	e3a0005f 	mov	r0, #95	; 0x5f
    9b70:	e3a02000 	mov	r2, #0	; 0x0
    9b74:	e1a03001 	mov	r3, r1
    9b78:	ebffdc0e 	bl	bb8 <pads_chmod>
    9b7c:	e3020710 	movw	r0, #10000	; 0x2710
    9b80:	e8bd4010 	pop	{r4, lr}
    9b84:	eafffba3 	b	8a18 <udelay>

00009b88 <flash_vs_reset>:
    9b88:	e92d4010 	push	{r4, lr}
    9b8c:	ebffffe4 	bl	9b24 <spi_sequence_brd_reboot>
    9b90:	e59f0048 	ldr	r0, [pc, #72]	; 9be0 <flash_vs_reset+0x58>
    9b94:	ebffdae8 	bl	73c <module_enable>
    9b98:	ebffff99 	bl	9a04 <spi_init_clk>
    9b9c:	e3a0000b 	mov	r0, #11	; 0xb
    9ba0:	e3a01000 	mov	r1, #0	; 0x0
    9ba4:	ebffdb2a 	bl	854 <pads_init_module>
    9ba8:	e59f2034 	ldr	r2, [pc, #52]	; 9be4 <flash_vs_reset+0x5c>
    9bac:	e592300c 	ldr	r3, [r2, #12]
    9bb0:	e3130010 	tst	r3, #16	; 0x10
    9bb4:	1afffffc 	bne	9bac <flash_vs_reset+0x24>
    9bb8:	ea000001 	b	9bc4 <flash_vs_reset+0x3c>
    9bbc:	e5923008 	ldr	r3, [r2, #8]
    9bc0:	ea000000 	b	9bc8 <flash_vs_reset+0x40>
    9bc4:	e59f2018 	ldr	r2, [pc, #24]	; 9be4 <flash_vs_reset+0x5c>
    9bc8:	e592300c 	ldr	r3, [r2, #12]
    9bcc:	e2134004 	ands	r4, r3, #4	; 0x4
    9bd0:	1afffff9 	bne	9bbc <flash_vs_reset+0x34>
    9bd4:	ebfffecd 	bl	9710 <flash_reset>
    9bd8:	e1a00004 	mov	r0, r4
    9bdc:	e8bd8010 	pop	{r4, pc}
    9be0:	2b011400 	.word	0x2b011400
    9be4:	28500000 	.word	0x28500000

00009be8 <ssp_log_open>:
    9be8:	e59f3008 	ldr	r3, [pc, #8]	; 9bf8 <ssp_log_open+0x10>
    9bec:	e3a02001 	mov	r2, #1	; 0x1
    9bf0:	e5832000 	str	r2, [r3]
    9bf4:	e12fff1e 	bx	lr
    9bf8:	0800d268 	.word	0x0800d268

00009bfc <ssp_log_close>:
    9bfc:	e59f3008 	ldr	r3, [pc, #8]	; 9c0c <ssp_log_close+0x10>
    9c00:	e3a02000 	mov	r2, #0	; 0x0
    9c04:	e5832000 	str	r2, [r3]
    9c08:	e12fff1e 	bx	lr
    9c0c:	0800d268 	.word	0x0800d268

00009c10 <ssp_manager_set_wire>:
    9c10:	e59f3004 	ldr	r3, [pc, #4]	; 9c1c <ssp_manager_set_wire+0xc>
    9c14:	e5830000 	str	r0, [r3]
    9c18:	e12fff1e 	bx	lr
    9c1c:	0800d208 	.word	0x0800d208

00009c20 <ssp_manager_set_rwmode>:
    9c20:	e2403064 	sub	r3, r0, #100	; 0x64
    9c24:	e59f2020 	ldr	r2, [pc, #32]	; 9c4c <ssp_manager_set_rwmode+0x2c>
    9c28:	e3530002 	cmp	r3, #2	; 0x2
    9c2c:	83a03064 	movhi	r3, #100	; 0x64
    9c30:	e5820050 	str	r0, [r2, #80]
    9c34:	85823050 	strhi	r3, [r2, #80]
    9c38:	e3500066 	cmp	r0, #102	; 0x66
    9c3c:	059f3008 	ldreq	r3, [pc, #8]	; 9c4c <ssp_manager_set_rwmode+0x2c>
    9c40:	03a02064 	moveq	r2, #100	; 0x64
    9c44:	05832050 	streq	r2, [r3, #80]
    9c48:	e12fff1e 	bx	lr
    9c4c:	0800d208 	.word	0x0800d208

00009c50 <ssp_manager_init>:
    9c50:	e59f2058 	ldr	r2, [pc, #88]	; 9cb0 <ssp_manager_init+0x60>
    9c54:	e30033ff 	movw	r3, #1023	; 0x3ff
    9c58:	e5823038 	str	r3, [r2, #56]
    9c5c:	e2433e3f 	sub	r3, r3, #1008	; 0x3f0
    9c60:	e5823000 	str	r3, [r2]
    9c64:	e2833011 	add	r3, r3, #17	; 0x11
    9c68:	e5823058 	str	r3, [r2, #88]
    9c6c:	e2833044 	add	r3, r3, #68	; 0x44
    9c70:	e5823050 	str	r3, [r2, #80]
    9c74:	e3a03040 	mov	r3, #64	; 0x40
    9c78:	e3a01000 	mov	r1, #0	; 0x0
    9c7c:	e3a00001 	mov	r0, #1	; 0x1
    9c80:	e3a0cb02 	mov	ip, #2048	; 0x800
    9c84:	e5823030 	str	r3, [r2, #48]
    9c88:	e0833003 	add	r3, r3, r3
    9c8c:	e582000c 	str	r0, [r2, #12]
    9c90:	e582c028 	str	ip, [r2, #40]
    9c94:	e582302c 	str	r3, [r2, #44]
    9c98:	e5821034 	str	r1, [r2, #52]
    9c9c:	e5821004 	str	r1, [r2, #4]
    9ca0:	e5821008 	str	r1, [r2, #8]
    9ca4:	e582005c 	str	r0, [r2, #92]
    9ca8:	e582c044 	str	ip, [r2, #68]
    9cac:	e12fff1e 	bx	lr
    9cb0:	0800d208 	.word	0x0800d208

00009cb4 <spi_status_is_busy>:
    9cb4:	e59f300c 	ldr	r3, [pc, #12]	; 9cc8 <spi_status_is_busy+0x14>
    9cb8:	e593000c 	ldr	r0, [r3, #12]
    9cbc:	e1a00220 	lsr	r0, r0, #4
    9cc0:	e2000001 	and	r0, r0, #1	; 0x1
    9cc4:	e12fff1e 	bx	lr
    9cc8:	28500000 	.word	0x28500000

00009ccc <spi_status_recv_fifo_not_empty>:
    9ccc:	e59f300c 	ldr	r3, [pc, #12]	; 9ce0 <spi_status_recv_fifo_not_empty+0x14>
    9cd0:	e593000c 	ldr	r0, [r3, #12]
    9cd4:	e1a00120 	lsr	r0, r0, #2
    9cd8:	e2000001 	and	r0, r0, #1	; 0x1
    9cdc:	e12fff1e 	bx	lr
    9ce0:	28500000 	.word	0x28500000

00009ce4 <spi_status_trans_fifo_empty>:
    9ce4:	e59f3008 	ldr	r3, [pc, #8]	; 9cf4 <spi_status_trans_fifo_empty+0x10>
    9ce8:	e593000c 	ldr	r0, [r3, #12]
    9cec:	e2000001 	and	r0, r0, #1	; 0x1
    9cf0:	e12fff1e 	bx	lr
    9cf4:	28500000 	.word	0x28500000

00009cf8 <spi_set_intr_recv_overrun>:
    9cf8:	e59f302c 	ldr	r3, [pc, #44]	; 9d2c <spi_set_intr_recv_overrun+0x34>
    9cfc:	e3500000 	cmp	r0, #0	; 0x0
    9d00:	e5933014 	ldr	r3, [r3, #20]
    9d04:	e3c32001 	bic	r2, r3, #1	; 0x1
    9d08:	0a000003 	beq	9d1c <spi_set_intr_recv_overrun+0x24>
    9d0c:	e3500001 	cmp	r0, #1	; 0x1
    9d10:	13e00000 	mvnne	r0, #0	; 0x0
    9d14:	112fff1e 	bxne	lr
    9d18:	e3822001 	orr	r2, r2, #1	; 0x1
    9d1c:	e59f3008 	ldr	r3, [pc, #8]	; 9d2c <spi_set_intr_recv_overrun+0x34>
    9d20:	e3a00000 	mov	r0, #0	; 0x0
    9d24:	e5832014 	str	r2, [r3, #20]
    9d28:	e12fff1e 	bx	lr
    9d2c:	28500000 	.word	0x28500000

00009d30 <spi_set_intr_recv_timeout>:
    9d30:	e59f302c 	ldr	r3, [pc, #44]	; 9d64 <spi_set_intr_recv_timeout+0x34>
    9d34:	e3500000 	cmp	r0, #0	; 0x0
    9d38:	e5933014 	ldr	r3, [r3, #20]
    9d3c:	e3c32002 	bic	r2, r3, #2	; 0x2
    9d40:	0a000003 	beq	9d54 <spi_set_intr_recv_timeout+0x24>
    9d44:	e3500002 	cmp	r0, #2	; 0x2
    9d48:	13e00000 	mvnne	r0, #0	; 0x0
    9d4c:	112fff1e 	bxne	lr
    9d50:	e3822002 	orr	r2, r2, #2	; 0x2
    9d54:	e59f3008 	ldr	r3, [pc, #8]	; 9d64 <spi_set_intr_recv_timeout+0x34>
    9d58:	e3a00000 	mov	r0, #0	; 0x0
    9d5c:	e5832014 	str	r2, [r3, #20]
    9d60:	e12fff1e 	bx	lr
    9d64:	28500000 	.word	0x28500000

00009d68 <spi_set_intr_recv_fifo>:
    9d68:	e59f302c 	ldr	r3, [pc, #44]	; 9d9c <spi_set_intr_recv_fifo+0x34>
    9d6c:	e3500000 	cmp	r0, #0	; 0x0
    9d70:	e5933014 	ldr	r3, [r3, #20]
    9d74:	e3c32004 	bic	r2, r3, #4	; 0x4
    9d78:	0a000003 	beq	9d8c <spi_set_intr_recv_fifo+0x24>
    9d7c:	e3500004 	cmp	r0, #4	; 0x4
    9d80:	13e00000 	mvnne	r0, #0	; 0x0
    9d84:	112fff1e 	bxne	lr
    9d88:	e3822004 	orr	r2, r2, #4	; 0x4
    9d8c:	e59f3008 	ldr	r3, [pc, #8]	; 9d9c <spi_set_intr_recv_fifo+0x34>
    9d90:	e3a00000 	mov	r0, #0	; 0x0
    9d94:	e5832014 	str	r2, [r3, #20]
    9d98:	e12fff1e 	bx	lr
    9d9c:	28500000 	.word	0x28500000

00009da0 <spi_set_intr_transmit_fifo>:
    9da0:	e59f302c 	ldr	r3, [pc, #44]	; 9dd4 <spi_set_intr_transmit_fifo+0x34>
    9da4:	e3500000 	cmp	r0, #0	; 0x0
    9da8:	e5933014 	ldr	r3, [r3, #20]
    9dac:	e3c32008 	bic	r2, r3, #8	; 0x8
    9db0:	0a000003 	beq	9dc4 <spi_set_intr_transmit_fifo+0x24>
    9db4:	e3500008 	cmp	r0, #8	; 0x8
    9db8:	13e00000 	mvnne	r0, #0	; 0x0
    9dbc:	112fff1e 	bxne	lr
    9dc0:	e3822008 	orr	r2, r2, #8	; 0x8
    9dc4:	e59f3008 	ldr	r3, [pc, #8]	; 9dd4 <spi_set_intr_transmit_fifo+0x34>
    9dc8:	e3a00000 	mov	r0, #0	; 0x0
    9dcc:	e5832014 	str	r2, [r3, #20]
    9dd0:	e12fff1e 	bx	lr
    9dd4:	28500000 	.word	0x28500000

00009dd8 <spi_get_raw_intr_recv_timeout>:
    9dd8:	e59f300c 	ldr	r3, [pc, #12]	; 9dec <spi_get_raw_intr_recv_timeout+0x14>
    9ddc:	e5930018 	ldr	r0, [r3, #24]
    9de0:	e1a000a0 	lsr	r0, r0, #1
    9de4:	e2000001 	and	r0, r0, #1	; 0x1
    9de8:	e12fff1e 	bx	lr
    9dec:	28500000 	.word	0x28500000

00009df0 <spi_get_raw_intr_recv_fifo>:
    9df0:	e59f300c 	ldr	r3, [pc, #12]	; 9e04 <spi_get_raw_intr_recv_fifo+0x14>
    9df4:	e5930018 	ldr	r0, [r3, #24]
    9df8:	e1a00120 	lsr	r0, r0, #2
    9dfc:	e2000001 	and	r0, r0, #1	; 0x1
    9e00:	e12fff1e 	bx	lr
    9e04:	28500000 	.word	0x28500000

00009e08 <spi_get_raw_intr_trans_fifo>:
    9e08:	e59f300c 	ldr	r3, [pc, #12]	; 9e1c <spi_get_raw_intr_trans_fifo+0x14>
    9e0c:	e5930018 	ldr	r0, [r3, #24]
    9e10:	e1a001a0 	lsr	r0, r0, #3
    9e14:	e2000001 	and	r0, r0, #1	; 0x1
    9e18:	e12fff1e 	bx	lr
    9e1c:	28500000 	.word	0x28500000

00009e20 <spi_clear_intr_recv_overun>:
    9e20:	e59f3008 	ldr	r3, [pc, #8]	; 9e30 <spi_clear_intr_recv_overun+0x10>
    9e24:	e3a02001 	mov	r2, #1	; 0x1
    9e28:	e5832020 	str	r2, [r3, #32]
    9e2c:	e12fff1e 	bx	lr
    9e30:	28500000 	.word	0x28500000

00009e34 <spi_clear_intr_recv_timeout>:
    9e34:	e59f3008 	ldr	r3, [pc, #8]	; 9e44 <spi_clear_intr_recv_timeout+0x10>
    9e38:	e3a02002 	mov	r2, #2	; 0x2
    9e3c:	e5832020 	str	r2, [r3, #32]
    9e40:	e12fff1e 	bx	lr
    9e44:	28500000 	.word	0x28500000

00009e48 <spi_clear_recv_fifo>:
    9e48:	e59f2014 	ldr	r2, [pc, #20]	; 9e64 <spi_clear_recv_fifo+0x1c>
    9e4c:	ea000000 	b	9e54 <spi_clear_recv_fifo+0xc>
    9e50:	e5923008 	ldr	r3, [r2, #8]
    9e54:	e592300c 	ldr	r3, [r2, #12]
    9e58:	e3130004 	tst	r3, #4	; 0x4
    9e5c:	1afffffb 	bne	9e50 <spi_clear_recv_fifo+0x8>
    9e60:	e12fff1e 	bx	lr
    9e64:	28500000 	.word	0x28500000

00009e68 <flash_nand_vs_align>:
    9e68:	e3a00b02 	mov	r0, #2048	; 0x800
    9e6c:	e12fff1e 	bx	lr

00009e70 <spi_transfer_init>:
    9e70:	e92d4010 	push	{r4, lr}
    9e74:	e2504000 	subs	r4, r0, #0	; 0x0
    9e78:	03e00001 	mvneq	r0, #1	; 0x1
    9e7c:	08bd8010 	popeq	{r4, pc}
    9e80:	e3a01000 	mov	r1, #0	; 0x0
    9e84:	e3a0205c 	mov	r2, #92	; 0x5c
    9e88:	ebffdd4a 	bl	13b8 <memset>
    9e8c:	e3a0200f 	mov	r2, #15	; 0xf
    9e90:	e5c42013 	strb	r2, [r4, #19]
    9e94:	e3a02002 	mov	r2, #2	; 0x2
    9e98:	e3a03000 	mov	r3, #0	; 0x0
    9e9c:	e5842030 	str	r2, [r4, #48]
    9ea0:	e3a02001 	mov	r2, #1	; 0x1
    9ea4:	e5842034 	str	r2, [r4, #52]
    9ea8:	e5c43058 	strb	r3, [r4, #88]
    9eac:	e5c43000 	strb	r3, [r4]
    9eb0:	e5c43001 	strb	r3, [r4, #1]
    9eb4:	e5c43002 	strb	r3, [r4, #2]
    9eb8:	e5c43003 	strb	r3, [r4, #3]
    9ebc:	e5c43004 	strb	r3, [r4, #4]
    9ec0:	e5843008 	str	r3, [r4, #8]
    9ec4:	e5c4300c 	strb	r3, [r4, #12]
    9ec8:	e5c4300d 	strb	r3, [r4, #13]
    9ecc:	e5c4300e 	strb	r3, [r4, #14]
    9ed0:	e5c4300f 	strb	r3, [r4, #15]
    9ed4:	e5c43010 	strb	r3, [r4, #16]
    9ed8:	e5c43011 	strb	r3, [r4, #17]
    9edc:	e5c43012 	strb	r3, [r4, #18]
    9ee0:	e5843014 	str	r3, [r4, #20]
    9ee4:	e5843018 	str	r3, [r4, #24]
    9ee8:	e584301c 	str	r3, [r4, #28]
    9eec:	e5843020 	str	r3, [r4, #32]
    9ef0:	e5843024 	str	r3, [r4, #36]
    9ef4:	e5843028 	str	r3, [r4, #40]
    9ef8:	e584302c 	str	r3, [r4, #44]
    9efc:	e5843038 	str	r3, [r4, #56]
    9f00:	e584303c 	str	r3, [r4, #60]
    9f04:	e5843040 	str	r3, [r4, #64]
    9f08:	e5843044 	str	r3, [r4, #68]
    9f0c:	e5843048 	str	r3, [r4, #72]
    9f10:	e584304c 	str	r3, [r4, #76]
    9f14:	e5843050 	str	r3, [r4, #80]
    9f18:	e5843054 	str	r3, [r4, #84]
    9f1c:	e1a00003 	mov	r0, r3
    9f20:	e8bd8010 	pop	{r4, pc}

00009f24 <spi_read_write_data>:
    9f24:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    9f28:	e2505000 	subs	r5, r0, #0	; 0x0
    9f2c:	e24dd014 	sub	sp, sp, #20	; 0x14
    9f30:	03e00001 	mvneq	r0, #1	; 0x1
    9f34:	0a0000f8 	beq	a31c <spi_read_write_data+0x3f8>
    9f38:	e1a0000d 	mov	r0, sp
    9f3c:	e3a01000 	mov	r1, #0	; 0x0
    9f40:	e3a02010 	mov	r2, #16	; 0x10
    9f44:	ebffdd1b 	bl	13b8 <memset>
    9f48:	e1d531d3 	ldrsb	r3, [r5, #19]
    9f4c:	e353000f 	cmp	r3, #15	; 0xf
    9f50:	1a0000ec 	bne	a308 <spi_read_write_data+0x3e4>
    9f54:	e59f23c8 	ldr	r2, [pc, #968]	; a324 <spi_read_write_data+0x400>
    9f58:	e592300c 	ldr	r3, [r2, #12]
    9f5c:	e3130001 	tst	r3, #1	; 0x1
    9f60:	0afffffc 	beq	9f58 <spi_read_write_data+0x34>
    9f64:	ea000001 	b	9f70 <spi_read_write_data+0x4c>
    9f68:	e5923008 	ldr	r3, [r2, #8]
    9f6c:	ea000000 	b	9f74 <spi_read_write_data+0x50>
    9f70:	e59f23ac 	ldr	r2, [pc, #940]	; a324 <spi_read_write_data+0x400>
    9f74:	e592300c 	ldr	r3, [r2, #12]
    9f78:	e2136004 	ands	r6, r3, #4	; 0x4
    9f7c:	1afffff9 	bne	9f68 <spi_read_write_data+0x44>
    9f80:	e1d531d0 	ldrsb	r3, [r5, #16]
    9f84:	e1d520d4 	ldrsb	r2, [r5, #4]
    9f88:	e5954008 	ldr	r4, [r5, #8]
    9f8c:	e1d511d2 	ldrsb	r1, [r5, #18]
    9f90:	e0833002 	add	r3, r3, r2
    9f94:	e3540000 	cmp	r4, #0	; 0x0
    9f98:	e0837001 	add	r7, r3, r1
    9f9c:	1a000021 	bne	a028 <spi_read_write_data+0x104>
    9fa0:	e3a00004 	mov	r0, #4	; 0x4
    9fa4:	e1a01004 	mov	r1, r4
    9fa8:	e1a02004 	mov	r2, r4
    9fac:	ebffdb6d 	bl	d68 <pads_setpin>
    9fb0:	e1a01004 	mov	r1, r4
    9fb4:	e1d520d4 	ldrsb	r2, [r5, #4]
    9fb8:	e59f0364 	ldr	r0, [pc, #868]	; a324 <spi_read_write_data+0x400>
    9fbc:	ea000002 	b	9fcc <spi_read_write_data+0xa8>
    9fc0:	e7d53001 	ldrb	r3, [r5, r1]
    9fc4:	e2811001 	add	r1, r1, #1	; 0x1
    9fc8:	e5803008 	str	r3, [r0, #8]
    9fcc:	e1510002 	cmp	r1, r2
    9fd0:	bafffffa 	blt	9fc0 <spi_read_write_data+0x9c>
    9fd4:	e59f2348 	ldr	r2, [pc, #840]	; a324 <spi_read_write_data+0x400>
    9fd8:	e592300c 	ldr	r3, [r2, #12]
    9fdc:	e3130010 	tst	r3, #16	; 0x10
    9fe0:	1afffffc 	bne	9fd8 <spi_read_write_data+0xb4>
    9fe4:	e59f2338 	ldr	r2, [pc, #824]	; a324 <spi_read_write_data+0x400>
    9fe8:	e592300c 	ldr	r3, [r2, #12]
    9fec:	e3130001 	tst	r3, #1	; 0x1
    9ff0:	0afffffc 	beq	9fe8 <spi_read_write_data+0xc4>
    9ff4:	ea000001 	b	a000 <spi_read_write_data+0xdc>
    9ff8:	e5923008 	ldr	r3, [r2, #8]
    9ffc:	ea000000 	b	a004 <spi_read_write_data+0xe0>
    a000:	e59f231c 	ldr	r2, [pc, #796]	; a324 <spi_read_write_data+0x400>
    a004:	e592300c 	ldr	r3, [r2, #12]
    a008:	e2134004 	ands	r4, r3, #4	; 0x4
    a00c:	1afffff9 	bne	9ff8 <spi_read_write_data+0xd4>
    a010:	e3a00004 	mov	r0, #4	; 0x4
    a014:	e3a01001 	mov	r1, #1	; 0x1
    a018:	e1a02004 	mov	r2, r4
    a01c:	ebffdb51 	bl	d68 <pads_setpin>
    a020:	e1a00004 	mov	r0, r4
    a024:	ea0000bc 	b	a31c <spi_read_write_data+0x3f8>
    a028:	e3a02010 	mov	r2, #16	; 0x10
    a02c:	e1a01006 	mov	r1, r6
    a030:	e1a0000d 	mov	r0, sp
    a034:	ebffdcdf 	bl	13b8 <memset>
    a038:	e1a01005 	mov	r1, r5
    a03c:	e1d520d4 	ldrsb	r2, [r5, #4]
    a040:	e1a0000d 	mov	r0, sp
    a044:	ebffdce3 	bl	13d8 <memcpy>
    a048:	e1d500d4 	ldrsb	r0, [r5, #4]
    a04c:	e285100c 	add	r1, r5, #12	; 0xc
    a050:	e1d521d0 	ldrsb	r2, [r5, #16]
    a054:	e08d0000 	add	r0, sp, r0
    a058:	ebffdcde 	bl	13d8 <memcpy>
    a05c:	e1d530d4 	ldrsb	r3, [r5, #4]
    a060:	e1d501d0 	ldrsb	r0, [r5, #16]
    a064:	e2851011 	add	r1, r5, #17	; 0x11
    a068:	e0800003 	add	r0, r0, r3
    a06c:	e1d521d2 	ldrsb	r2, [r5, #18]
    a070:	e08d0000 	add	r0, sp, r0
    a074:	ebffdcd7 	bl	13d8 <memcpy>
    a078:	e1a01006 	mov	r1, r6
    a07c:	e1a02006 	mov	r2, r6
    a080:	e3a00004 	mov	r0, #4	; 0x4
    a084:	ebffdb37 	bl	d68 <pads_setpin>
    a088:	e1a0400d 	mov	r4, sp
    a08c:	e1a01006 	mov	r1, r6
    a090:	e59f228c 	ldr	r2, [pc, #652]	; a324 <spi_read_write_data+0x400>
    a094:	ea000002 	b	a0a4 <spi_read_write_data+0x180>
    a098:	e19430d1 	ldrsb	r3, [r4, r1]
    a09c:	e2811001 	add	r1, r1, #1	; 0x1
    a0a0:	e5823008 	str	r3, [r2, #8]
    a0a4:	e1510007 	cmp	r1, r7
    a0a8:	bafffffa 	blt	a098 <spi_read_write_data+0x174>
    a0ac:	e3a08000 	mov	r8, #0	; 0x0
    a0b0:	e595e018 	ldr	lr, [r5, #24]
    a0b4:	e5956024 	ldr	r6, [r5, #36]
    a0b8:	e595401c 	ldr	r4, [r5, #28]
    a0bc:	e59fc260 	ldr	ip, [pc, #608]	; a324 <spi_read_write_data+0x400>
    a0c0:	e1a0b008 	mov	fp, r8
    a0c4:	e3a0a002 	mov	sl, #2	; 0x2
    a0c8:	e3a09001 	mov	r9, #1	; 0x1
    a0cc:	e59c3018 	ldr	r3, [ip, #24]
    a0d0:	e3130004 	tst	r3, #4	; 0x4
    a0d4:	0a000036 	beq	a1b4 <spi_read_write_data+0x290>
    a0d8:	e3570003 	cmp	r7, #3	; 0x3
    a0dc:	da000005 	ble	a0f8 <spi_read_write_data+0x1d4>
    a0e0:	e59c3008 	ldr	r3, [ip, #8]
    a0e4:	e2477004 	sub	r7, r7, #4	; 0x4
    a0e8:	e59c3008 	ldr	r3, [ip, #8]
    a0ec:	e59c3008 	ldr	r3, [ip, #8]
    a0f0:	e59c3008 	ldr	r3, [ip, #8]
    a0f4:	ea00002e 	b	a1b4 <spi_read_write_data+0x290>
    a0f8:	e3570000 	cmp	r7, #0	; 0x0
    a0fc:	da000003 	ble	a110 <spi_read_write_data+0x1ec>
    a100:	e2577001 	subs	r7, r7, #1	; 0x1
    a104:	e59c3008 	ldr	r3, [ip, #8]
    a108:	1afffffc 	bne	a100 <spi_read_write_data+0x1dc>
    a10c:	ea000028 	b	a1b4 <spi_read_write_data+0x290>
    a110:	e35e0003 	cmp	lr, #3	; 0x3
    a114:	da000018 	ble	a17c <spi_read_write_data+0x258>
    a118:	e5951014 	ldr	r1, [r5, #20]
    a11c:	e3510000 	cmp	r1, #0	; 0x0
    a120:	0a00000f 	beq	a164 <spi_read_write_data+0x240>
    a124:	e59c2008 	ldr	r2, [ip, #8]
    a128:	e1a03001 	mov	r3, r1
    a12c:	e4c32001 	strb	r2, [r3], #1
    a130:	e59c2008 	ldr	r2, [ip, #8]
    a134:	e5853014 	str	r3, [r5, #20]
    a138:	e5c12001 	strb	r2, [r1, #1]
    a13c:	e59c1008 	ldr	r1, [ip, #8]
    a140:	e2832001 	add	r2, r3, #1	; 0x1
    a144:	e5852014 	str	r2, [r5, #20]
    a148:	e5c31001 	strb	r1, [r3, #1]
    a14c:	e59c1008 	ldr	r1, [ip, #8]
    a150:	e2823001 	add	r3, r2, #1	; 0x1
    a154:	e2830001 	add	r0, r3, #1	; 0x1
    a158:	e5c21001 	strb	r1, [r2, #1]
    a15c:	e5850014 	str	r0, [r5, #20]
    a160:	ea000003 	b	a174 <spi_read_write_data+0x250>
    a164:	e59c3008 	ldr	r3, [ip, #8]
    a168:	e59c3008 	ldr	r3, [ip, #8]
    a16c:	e59c3008 	ldr	r3, [ip, #8]
    a170:	e59c3008 	ldr	r3, [ip, #8]
    a174:	e24ee004 	sub	lr, lr, #4	; 0x4
    a178:	ea00000b 	b	a1ac <spi_read_write_data+0x288>
    a17c:	e35e0000 	cmp	lr, #0	; 0x0
    a180:	da000009 	ble	a1ac <spi_read_write_data+0x288>
    a184:	e59f1198 	ldr	r1, [pc, #408]	; a324 <spi_read_write_data+0x400>
    a188:	e5953014 	ldr	r3, [r5, #20]
    a18c:	e2532000 	subs	r2, r3, #0	; 0x0
    a190:	15913008 	ldrne	r3, [r1, #8]
    a194:	05913008 	ldreq	r3, [r1, #8]
    a198:	14c23001 	strbne	r3, [r2], #1
    a19c:	15852014 	strne	r2, [r5, #20]
    a1a0:	e25ee001 	subs	lr, lr, #1	; 0x1
    a1a4:	1afffff7 	bne	a188 <spi_read_write_data+0x264>
    a1a8:	ea00004e 	b	a2e8 <spi_read_write_data+0x3c4>
    a1ac:	e35e0000 	cmp	lr, #0	; 0x0
    a1b0:	0a00004c 	beq	a2e8 <spi_read_write_data+0x3c4>
    a1b4:	e59c3018 	ldr	r3, [ip, #24]
    a1b8:	e3130008 	tst	r3, #8	; 0x8
    a1bc:	0a00002a 	beq	a26c <spi_read_write_data+0x348>
    a1c0:	e3560003 	cmp	r6, #3	; 0x3
    a1c4:	da000012 	ble	a214 <spi_read_write_data+0x2f0>
    a1c8:	e5951020 	ldr	r1, [r5, #32]
    a1cc:	e2466004 	sub	r6, r6, #4	; 0x4
    a1d0:	e1a02001 	mov	r2, r1
    a1d4:	e4d23001 	ldrb	r3, [r2], #1
    a1d8:	e58c3008 	str	r3, [ip, #8]
    a1dc:	e5852020 	str	r2, [r5, #32]
    a1e0:	e5d13001 	ldrb	r3, [r1, #1]
    a1e4:	e2821001 	add	r1, r2, #1	; 0x1
    a1e8:	e58c3008 	str	r3, [ip, #8]
    a1ec:	e5851020 	str	r1, [r5, #32]
    a1f0:	e5d22001 	ldrb	r2, [r2, #1]
    a1f4:	e2813001 	add	r3, r1, #1	; 0x1
    a1f8:	e5853020 	str	r3, [r5, #32]
    a1fc:	e58c2008 	str	r2, [ip, #8]
    a200:	e5d12001 	ldrb	r2, [r1, #1]
    a204:	e2833001 	add	r3, r3, #1	; 0x1
    a208:	e58c2008 	str	r2, [ip, #8]
    a20c:	e5853020 	str	r3, [r5, #32]
    a210:	ea000015 	b	a26c <spi_read_write_data+0x348>
    a214:	e3560000 	cmp	r6, #0	; 0x0
    a218:	da000006 	ble	a238 <spi_read_write_data+0x314>
    a21c:	e5953020 	ldr	r3, [r5, #32]
    a220:	e2566001 	subs	r6, r6, #1	; 0x1
    a224:	e4d32001 	ldrb	r2, [r3], #1
    a228:	e58c2008 	str	r2, [ip, #8]
    a22c:	e5853020 	str	r3, [r5, #32]
    a230:	1afffff9 	bne	a21c <spi_read_write_data+0x2f8>
    a234:	ea00000c 	b	a26c <spi_read_write_data+0x348>
    a238:	e3540003 	cmp	r4, #3	; 0x3
    a23c:	da000005 	ble	a258 <spi_read_write_data+0x334>
    a240:	e58c8008 	str	r8, [ip, #8]
    a244:	e2444004 	sub	r4, r4, #4	; 0x4
    a248:	e58c8008 	str	r8, [ip, #8]
    a24c:	e58c8008 	str	r8, [ip, #8]
    a250:	e58c8008 	str	r8, [ip, #8]
    a254:	ea000004 	b	a26c <spi_read_write_data+0x348>
    a258:	e3540000 	cmp	r4, #0	; 0x0
    a25c:	da000002 	ble	a26c <spi_read_write_data+0x348>
    a260:	e2544001 	subs	r4, r4, #1	; 0x1
    a264:	e58cb008 	str	fp, [ip, #8]
    a268:	1afffffc 	bne	a260 <spi_read_write_data+0x33c>
    a26c:	e59c3018 	ldr	r3, [ip, #24]
    a270:	e3130002 	tst	r3, #2	; 0x2
    a274:	0a000019 	beq	a2e0 <spi_read_write_data+0x3bc>
    a278:	e59c300c 	ldr	r3, [ip, #12]
    a27c:	e3130004 	tst	r3, #4	; 0x4
    a280:	0a000016 	beq	a2e0 <spi_read_write_data+0x3bc>
    a284:	ea000004 	b	a29c <spi_read_write_data+0x378>
    a288:	e59c300c 	ldr	r3, [ip, #12]
    a28c:	e3130004 	tst	r3, #4	; 0x4
    a290:	0a000012 	beq	a2e0 <spi_read_write_data+0x3bc>
    a294:	e59c3008 	ldr	r3, [ip, #8]
    a298:	e2477001 	sub	r7, r7, #1	; 0x1
    a29c:	e3570000 	cmp	r7, #0	; 0x0
    a2a0:	cafffff8 	bgt	a288 <spi_read_write_data+0x364>
    a2a4:	ea000009 	b	a2d0 <spi_read_write_data+0x3ac>
    a2a8:	e59c300c 	ldr	r3, [ip, #12]
    a2ac:	e3130004 	tst	r3, #4	; 0x4
    a2b0:	0a00000a 	beq	a2e0 <spi_read_write_data+0x3bc>
    a2b4:	e5953014 	ldr	r3, [r5, #20]
    a2b8:	e24ee001 	sub	lr, lr, #1	; 0x1
    a2bc:	e2532000 	subs	r2, r3, #0	; 0x0
    a2c0:	159c3008 	ldrne	r3, [ip, #8]
    a2c4:	059c3008 	ldreq	r3, [ip, #8]
    a2c8:	14c23001 	strbne	r3, [r2], #1
    a2cc:	15852014 	strne	r2, [r5, #20]
    a2d0:	e35e0000 	cmp	lr, #0	; 0x0
    a2d4:	cafffff3 	bgt	a2a8 <spi_read_write_data+0x384>
    a2d8:	e58ca020 	str	sl, [ip, #32]
    a2dc:	0a000001 	beq	a2e8 <spi_read_write_data+0x3c4>
    a2e0:	e58c9020 	str	r9, [ip, #32]
    a2e4:	eaffff78 	b	a0cc <spi_read_write_data+0x1a8>
    a2e8:	e59f2034 	ldr	r2, [pc, #52]	; a324 <spi_read_write_data+0x400>
    a2ec:	e592300c 	ldr	r3, [r2, #12]
    a2f0:	e3130010 	tst	r3, #16	; 0x10
    a2f4:	1afffffc 	bne	a2ec <spi_read_write_data+0x3c8>
    a2f8:	e59f2024 	ldr	r2, [pc, #36]	; a324 <spi_read_write_data+0x400>
    a2fc:	e592300c 	ldr	r3, [r2, #12]
    a300:	e3130001 	tst	r3, #1	; 0x1
    a304:	0afffffc 	beq	a2fc <spi_read_write_data+0x3d8>
    a308:	e3a00004 	mov	r0, #4	; 0x4
    a30c:	e3a01001 	mov	r1, #1	; 0x1
    a310:	e3a02000 	mov	r2, #0	; 0x0
    a314:	ebffda93 	bl	d68 <pads_setpin>
    a318:	e3a00000 	mov	r0, #0	; 0x0
    a31c:	e28dd014 	add	sp, sp, #20	; 0x14
    a320:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a324:	28500000 	.word	0x28500000

0000a328 <flash_set_device_status>:
    a328:	e92d4070 	push	{r4, r5, r6, lr}
    a32c:	e24dd060 	sub	sp, sp, #96	; 0x60
    a330:	e1a04000 	mov	r4, r0
    a334:	e28d0004 	add	r0, sp, #4	; 0x4
    a338:	e1a06001 	mov	r6, r1
    a33c:	e1a05002 	mov	r5, r2
    a340:	ebfffeca 	bl	9e70 <spi_transfer_init>
    a344:	e59f303c 	ldr	r3, [pc, #60]	; a388 <flash_set_device_status+0x60>
    a348:	e28d0060 	add	r0, sp, #96	; 0x60
    a34c:	e5933000 	ldr	r3, [r3]
    a350:	e560405c 	strb	r4, [r0, #-92]!
    a354:	e353000f 	cmp	r3, #15	; 0xf
    a358:	13a03001 	movne	r3, #1	; 0x1
    a35c:	e5cd3017 	strb	r3, [sp, #23]
    a360:	e3a04000 	mov	r4, #0	; 0x0
    a364:	e3a03003 	mov	r3, #3	; 0x3
    a368:	e5cd3008 	strb	r3, [sp, #8]
    a36c:	e5cd5006 	strb	r5, [sp, #6]
    a370:	e5cd6005 	strb	r6, [sp, #5]
    a374:	e58d400c 	str	r4, [sp, #12]
    a378:	ebfffee9 	bl	9f24 <spi_read_write_data>
    a37c:	e1a00004 	mov	r0, r4
    a380:	e28dd060 	add	sp, sp, #96	; 0x60
    a384:	e8bd8070 	pop	{r4, r5, r6, pc}
    a388:	0800d208 	.word	0x0800d208

0000a38c <flash_write_enable>:
    a38c:	e92d4010 	push	{r4, lr}
    a390:	e24dd060 	sub	sp, sp, #96	; 0x60
    a394:	e28d0004 	add	r0, sp, #4	; 0x4
    a398:	ebfffeb4 	bl	9e70 <spi_transfer_init>
    a39c:	e59f3030 	ldr	r3, [pc, #48]	; a3d4 <flash_write_enable+0x48>
    a3a0:	e28d0060 	add	r0, sp, #96	; 0x60
    a3a4:	e5933000 	ldr	r3, [r3]
    a3a8:	e3a04000 	mov	r4, #0	; 0x0
    a3ac:	e5cd3017 	strb	r3, [sp, #23]
    a3b0:	e3a03006 	mov	r3, #6	; 0x6
    a3b4:	e560305c 	strb	r3, [r0, #-92]!
    a3b8:	e3a03001 	mov	r3, #1	; 0x1
    a3bc:	e5cd3008 	strb	r3, [sp, #8]
    a3c0:	e58d400c 	str	r4, [sp, #12]
    a3c4:	ebfffed6 	bl	9f24 <spi_read_write_data>
    a3c8:	e1a00004 	mov	r0, r4
    a3cc:	e28dd060 	add	sp, sp, #96	; 0x60
    a3d0:	e8bd8010 	pop	{r4, pc}
    a3d4:	0800d208 	.word	0x0800d208

0000a3d8 <nand_flash_reset>:
    a3d8:	e92d4010 	push	{r4, lr}
    a3dc:	e24dd060 	sub	sp, sp, #96	; 0x60
    a3e0:	e28d4004 	add	r4, sp, #4	; 0x4
    a3e4:	e1a00004 	mov	r0, r4
    a3e8:	ebfffea0 	bl	9e70 <spi_transfer_init>
    a3ec:	e3a03001 	mov	r3, #1	; 0x1
    a3f0:	e5cd3008 	strb	r3, [sp, #8]
    a3f4:	e3a03000 	mov	r3, #0	; 0x0
    a3f8:	e58d300c 	str	r3, [sp, #12]
    a3fc:	e1a00004 	mov	r0, r4
    a400:	e283300f 	add	r3, r3, #15	; 0xf
    a404:	e3e04000 	mvn	r4, #0	; 0x0
    a408:	e5cd3017 	strb	r3, [sp, #23]
    a40c:	e5cd4004 	strb	r4, [sp, #4]
    a410:	ebfffec3 	bl	9f24 <spi_read_write_data>
    a414:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    a418:	ebfff97e 	bl	8a18 <udelay>
    a41c:	e1a00004 	mov	r0, r4
    a420:	e28dd060 	add	sp, sp, #96	; 0x60
    a424:	e8bd8010 	pop	{r4, pc}

0000a428 <flash_wait_device_status>:
    a428:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    a42c:	e2514000 	subs	r4, r1, #0	; 0x0
    a430:	e1a03000 	mov	r3, r0
    a434:	e24dd064 	sub	sp, sp, #100	; 0x64
    a438:	b3e00001 	mvnlt	r0, #1	; 0x1
    a43c:	ba00008d 	blt	a678 <flash_wait_device_status+0x250>
    a440:	e1a0000d 	mov	r0, sp
    a444:	e6ef5073 	uxtb	r5, r3
    a448:	ebfffe88 	bl	9e70 <spi_transfer_init>
    a44c:	e3a03002 	mov	r3, #2	; 0x2
    a450:	e58d3008 	str	r3, [sp, #8]
    a454:	e28d305c 	add	r3, sp, #92	; 0x5c
    a458:	e3a02001 	mov	r2, #1	; 0x1
    a45c:	e58d3014 	str	r3, [sp, #20]
    a460:	e2441001 	sub	r1, r4, #1	; 0x1
    a464:	e3a0300f 	mov	r3, #15	; 0xf
    a468:	e58d201c 	str	r2, [sp, #28]
    a46c:	e5cd3013 	strb	r3, [sp, #19]
    a470:	e5cd5000 	strb	r5, [sp]
    a474:	e5cd2004 	strb	r2, [sp, #4]
    a478:	e58d2018 	str	r2, [sp, #24]
    a47c:	e1510003 	cmp	r1, r3
    a480:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    a484:	ea00007a 	b	a674 <flash_wait_device_status+0x24c>
    a488:	0000a4c8 	.word	0x0000a4c8
    a48c:	0000a674 	.word	0x0000a674
    a490:	0000a674 	.word	0x0000a674
    a494:	0000a674 	.word	0x0000a674
    a498:	0000a674 	.word	0x0000a674
    a49c:	0000a5bc 	.word	0x0000a5bc
    a4a0:	0000a624 	.word	0x0000a624
    a4a4:	0000a510 	.word	0x0000a510
    a4a8:	0000a554 	.word	0x0000a554
    a4ac:	0000a674 	.word	0x0000a674
    a4b0:	0000a674 	.word	0x0000a674
    a4b4:	0000a674 	.word	0x0000a674
    a4b8:	0000a674 	.word	0x0000a674
    a4bc:	0000a674 	.word	0x0000a674
    a4c0:	0000a674 	.word	0x0000a674
    a4c4:	0000a574 	.word	0x0000a574
    a4c8:	e28d405c 	add	r4, sp, #92	; 0x5c
    a4cc:	e1a0500d 	mov	r5, sp
    a4d0:	e3a01000 	mov	r1, #0	; 0x0
    a4d4:	e3a02002 	mov	r2, #2	; 0x2
    a4d8:	e1a00004 	mov	r0, r4
    a4dc:	ebffdbb5 	bl	13b8 <memset>
    a4e0:	e1a0000d 	mov	r0, sp
    a4e4:	e58d4014 	str	r4, [sp, #20]
    a4e8:	ebfffe8d 	bl	9f24 <spi_read_write_data>
    a4ec:	e5dd305c 	ldrb	r3, [sp, #92]
    a4f0:	e3130002 	tst	r3, #2	; 0x2
    a4f4:	0a000001 	beq	a500 <flash_wait_device_status+0xd8>
    a4f8:	e3130001 	tst	r3, #1	; 0x1
    a4fc:	0a00005c 	beq	a674 <flash_wait_device_status+0x24c>
    a500:	ebffffa1 	bl	a38c <flash_write_enable>
    a504:	e3a00032 	mov	r0, #50	; 0x32
    a508:	ebfff942 	bl	8a18 <udelay>
    a50c:	eaffffef 	b	a4d0 <flash_wait_device_status+0xa8>
    a510:	e3a0c002 	mov	ip, #2	; 0x2
    a514:	e28d405c 	add	r4, sp, #92	; 0x5c
    a518:	e3e0305f 	mvn	r3, #95	; 0x5f
    a51c:	e1a0200c 	mov	r2, ip
    a520:	e3a01000 	mov	r1, #0	; 0x0
    a524:	e1a00004 	mov	r0, r4
    a528:	e5cd3001 	strb	r3, [sp, #1]
    a52c:	e5cd5000 	strb	r5, [sp]
    a530:	e5cdc004 	strb	ip, [sp, #4]
    a534:	e58dc008 	str	ip, [sp, #8]
    a538:	ebffdb9e 	bl	13b8 <memset>
    a53c:	e3a03001 	mov	r3, #1	; 0x1
    a540:	e1a0000d 	mov	r0, sp
    a544:	e58d4014 	str	r4, [sp, #20]
    a548:	e58d3018 	str	r3, [sp, #24]
    a54c:	ebfffe74 	bl	9f24 <spi_read_write_data>
    a550:	ea000047 	b	a674 <flash_wait_device_status+0x24c>
    a554:	e3a0c002 	mov	ip, #2	; 0x2
    a558:	e28d405c 	add	r4, sp, #92	; 0x5c
    a55c:	e1a0200c 	mov	r2, ip
    a560:	e3a01000 	mov	r1, #0	; 0x0
    a564:	e1a00004 	mov	r0, r4
    a568:	e5cd5000 	strb	r5, [sp]
    a56c:	e3e0304f 	mvn	r3, #79	; 0x4f
    a570:	ea000006 	b	a590 <flash_wait_device_status+0x168>
    a574:	e3a0c002 	mov	ip, #2	; 0x2
    a578:	e28d405c 	add	r4, sp, #92	; 0x5c
    a57c:	e1a0200c 	mov	r2, ip
    a580:	e3a01000 	mov	r1, #0	; 0x0
    a584:	e1a00004 	mov	r0, r4
    a588:	e5cd5000 	strb	r5, [sp]
    a58c:	e3e0303f 	mvn	r3, #63	; 0x3f
    a590:	e5cd3001 	strb	r3, [sp, #1]
    a594:	e5cdc004 	strb	ip, [sp, #4]
    a598:	e58dc008 	str	ip, [sp, #8]
    a59c:	ebffdb85 	bl	13b8 <memset>
    a5a0:	e3a03001 	mov	r3, #1	; 0x1
    a5a4:	e1a0000d 	mov	r0, sp
    a5a8:	e58d4014 	str	r4, [sp, #20]
    a5ac:	e58d3018 	str	r3, [sp, #24]
    a5b0:	ebfffe5b 	bl	9f24 <spi_read_write_data>
    a5b4:	e5dd005c 	ldrb	r0, [sp, #92]
    a5b8:	ea00002e 	b	a678 <flash_wait_device_status+0x250>
    a5bc:	e3a02002 	mov	r2, #2	; 0x2
    a5c0:	e3e0303f 	mvn	r3, #63	; 0x3f
    a5c4:	e5cd3001 	strb	r3, [sp, #1]
    a5c8:	e58d2008 	str	r2, [sp, #8]
    a5cc:	e5cd5000 	strb	r5, [sp]
    a5d0:	e5cd2004 	strb	r2, [sp, #4]
    a5d4:	e3a04000 	mov	r4, #0	; 0x0
    a5d8:	e28d605c 	add	r6, sp, #92	; 0x5c
    a5dc:	e1a0700d 	mov	r7, sp
    a5e0:	e3a01000 	mov	r1, #0	; 0x0
    a5e4:	e3a02002 	mov	r2, #2	; 0x2
    a5e8:	e1a00006 	mov	r0, r6
    a5ec:	ebffdb71 	bl	13b8 <memset>
    a5f0:	e1a0000d 	mov	r0, sp
    a5f4:	e58d6014 	str	r6, [sp, #20]
    a5f8:	ebfffe49 	bl	9f24 <spi_read_write_data>
    a5fc:	e5dd305c 	ldrb	r3, [sp, #92]
    a600:	e2844001 	add	r4, r4, #1	; 0x1
    a604:	e3130001 	tst	r3, #1	; 0x1
    a608:	e3a0001e 	mov	r0, #30	; 0x1e
    a60c:	0a000004 	beq	a624 <flash_wait_device_status+0x1fc>
    a610:	ebfff900 	bl	8a18 <udelay>
    a614:	e3540064 	cmp	r4, #100	; 0x64
    a618:	1afffff0 	bne	a5e0 <flash_wait_device_status+0x1b8>
    a61c:	e3e00000 	mvn	r0, #0	; 0x0
    a620:	ea000014 	b	a678 <flash_wait_device_status+0x250>
    a624:	e3a02002 	mov	r2, #2	; 0x2
    a628:	e3e0303f 	mvn	r3, #63	; 0x3f
    a62c:	e5cd5000 	strb	r5, [sp]
    a630:	e5cd3001 	strb	r3, [sp, #1]
    a634:	e58d2008 	str	r2, [sp, #8]
    a638:	e5cd2004 	strb	r2, [sp, #4]
    a63c:	e28d405c 	add	r4, sp, #92	; 0x5c
    a640:	e1a0500d 	mov	r5, sp
    a644:	e3a01000 	mov	r1, #0	; 0x0
    a648:	e3a02002 	mov	r2, #2	; 0x2
    a64c:	e1a00004 	mov	r0, r4
    a650:	ebffdb58 	bl	13b8 <memset>
    a654:	e1a0000d 	mov	r0, sp
    a658:	e58d4014 	str	r4, [sp, #20]
    a65c:	ebfffe30 	bl	9f24 <spi_read_write_data>
    a660:	e5dd305c 	ldrb	r3, [sp, #92]
    a664:	e3130002 	tst	r3, #2	; 0x2
    a668:	1a000001 	bne	a674 <flash_wait_device_status+0x24c>
    a66c:	ebffff46 	bl	a38c <flash_write_enable>
    a670:	eafffff3 	b	a644 <flash_wait_device_status+0x21c>
    a674:	e3a00000 	mov	r0, #0	; 0x0
    a678:	e28dd064 	add	sp, sp, #100	; 0x64
    a67c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

0000a680 <nand_write_cache_to_memory>:
    a680:	e92d4030 	push	{r4, r5, lr}
    a684:	e24dd064 	sub	sp, sp, #100	; 0x64
    a688:	e1a05000 	mov	r5, r0
    a68c:	e28d0004 	add	r0, sp, #4	; 0x4
    a690:	e1a04001 	mov	r4, r1
    a694:	ebfffdf5 	bl	9e70 <spi_transfer_init>
    a698:	e354000f 	cmp	r4, #15	; 0xf
    a69c:	13a03001 	movne	r3, #1	; 0x1
    a6a0:	15cd3017 	strbne	r3, [sp, #23]
    a6a4:	e3a03010 	mov	r3, #16	; 0x10
    a6a8:	e1a01335 	lsr	r1, r5, r3
    a6ac:	e28d0060 	add	r0, sp, #96	; 0x60
    a6b0:	05cd4017 	strbeq	r4, [sp, #23]
    a6b4:	e1a02425 	lsr	r2, r5, #8
    a6b8:	e3a04000 	mov	r4, #0	; 0x0
    a6bc:	e560305c 	strb	r3, [r0, #-92]!
    a6c0:	e3a03004 	mov	r3, #4	; 0x4
    a6c4:	e5cd3008 	strb	r3, [sp, #8]
    a6c8:	e5cd2006 	strb	r2, [sp, #6]
    a6cc:	e5cd1005 	strb	r1, [sp, #5]
    a6d0:	e58d400c 	str	r4, [sp, #12]
    a6d4:	e5cd5007 	strb	r5, [sp, #7]
    a6d8:	ebfffe11 	bl	9f24 <spi_read_write_data>
    a6dc:	e3a01006 	mov	r1, #6	; 0x6
    a6e0:	e3a0000f 	mov	r0, #15	; 0xf
    a6e4:	ebffff4f 	bl	a428 <flash_wait_device_status>
    a6e8:	e1a00004 	mov	r0, r4
    a6ec:	e28dd064 	add	sp, sp, #100	; 0x64
    a6f0:	e8bd8030 	pop	{r4, r5, pc}

0000a6f4 <nand_read_page_to_cache>:
    a6f4:	e92d4030 	push	{r4, r5, lr}
    a6f8:	e24dd064 	sub	sp, sp, #100	; 0x64
    a6fc:	e1a04001 	mov	r4, r1
    a700:	e1a05000 	mov	r5, r0
    a704:	e28d0004 	add	r0, sp, #4	; 0x4
    a708:	ebfffdd8 	bl	9e70 <spi_transfer_init>
    a70c:	e354000f 	cmp	r4, #15	; 0xf
    a710:	e28d0060 	add	r0, sp, #96	; 0x60
    a714:	e3a03013 	mov	r3, #19	; 0x13
    a718:	05cd4017 	strbeq	r4, [sp, #23]
    a71c:	e1a01825 	lsr	r1, r5, #16
    a720:	e1a02425 	lsr	r2, r5, #8
    a724:	e3a04000 	mov	r4, #0	; 0x0
    a728:	e560305c 	strb	r3, [r0, #-92]!
    a72c:	e3a03004 	mov	r3, #4	; 0x4
    a730:	e5cd3008 	strb	r3, [sp, #8]
    a734:	e5cd2006 	strb	r2, [sp, #6]
    a738:	e5cd1005 	strb	r1, [sp, #5]
    a73c:	e58d400c 	str	r4, [sp, #12]
    a740:	e5cd5007 	strb	r5, [sp, #7]
    a744:	ebfffdf6 	bl	9f24 <spi_read_write_data>
    a748:	e3a01006 	mov	r1, #6	; 0x6
    a74c:	e3a0000f 	mov	r0, #15	; 0xf
    a750:	ebffff34 	bl	a428 <flash_wait_device_status>
    a754:	e1a00004 	mov	r0, r4
    a758:	e28dd064 	add	sp, sp, #100	; 0x64
    a75c:	e8bd8030 	pop	{r4, r5, pc}

0000a760 <nand_blk_prg_era_fail>:
    a760:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    a764:	e3a01010 	mov	r1, #16	; 0x10
    a768:	e24dd004 	sub	sp, sp, #4	; 0x4
    a76c:	e3a0000f 	mov	r0, #15	; 0xf
    a770:	ebffff2c 	bl	a428 <flash_wait_device_status>
    a774:	e3100008 	tst	r0, #8	; 0x8
    a778:	01a03120 	lsreq	r3, r0, #2
    a77c:	13a00001 	movne	r0, #1	; 0x1
    a780:	02030001 	andeq	r0, r3, #1	; 0x1
    a784:	e28dd004 	add	sp, sp, #4	; 0x4
    a788:	e8bd8000 	pop	{pc}

0000a78c <nand_ecc_bits_check>:
    a78c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    a790:	e3a01010 	mov	r1, #16	; 0x10
    a794:	e24dd004 	sub	sp, sp, #4	; 0x4
    a798:	e3a0000f 	mov	r0, #15	; 0xf
    a79c:	ebffff21 	bl	a428 <flash_wait_device_status>
    a7a0:	e2000030 	and	r0, r0, #48	; 0x30
    a7a4:	e1a00220 	lsr	r0, r0, #4
    a7a8:	e28dd004 	add	sp, sp, #4	; 0x4
    a7ac:	e8bd8000 	pop	{pc}

0000a7b0 <nand_ecc_enable>:
    a7b0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    a7b4:	e59f409c 	ldr	r4, [pc, #156]	; a858 <nand_ecc_enable+0xa8>
    a7b8:	e24dd064 	sub	sp, sp, #100	; 0x64
    a7bc:	e594600c 	ldr	r6, [r4, #12]
    a7c0:	e3560001 	cmp	r6, #1	; 0x1
    a7c4:	1a00001e 	bne	a844 <nand_ecc_enable+0x94>
    a7c8:	e5943040 	ldr	r3, [r4, #64]
    a7cc:	e3530001 	cmp	r3, #1	; 0x1
    a7d0:	0a00001d 	beq	a84c <nand_ecc_enable+0x9c>
    a7d4:	e3a0000f 	mov	r0, #15	; 0xf
    a7d8:	e3a01009 	mov	r1, #9	; 0x9
    a7dc:	ebffff11 	bl	a428 <flash_wait_device_status>
    a7e0:	e2107010 	ands	r7, r0, #16	; 0x10
    a7e4:	e1a05000 	mov	r5, r0
    a7e8:	15846040 	strne	r6, [r4, #64]
    a7ec:	1a000016 	bne	a84c <nand_ecc_enable+0x9c>
    a7f0:	e28d4004 	add	r4, sp, #4	; 0x4
    a7f4:	e1a00004 	mov	r0, r4
    a7f8:	ebfffd9c 	bl	9e70 <spi_transfer_init>
    a7fc:	e3a0301f 	mov	r3, #31	; 0x1f
    a800:	e5cd3004 	strb	r3, [sp, #4]
    a804:	e3e0304f 	mvn	r3, #79	; 0x4f
    a808:	e3852010 	orr	r2, r5, #16	; 0x10
    a80c:	e1a00004 	mov	r0, r4
    a810:	e5cd3005 	strb	r3, [sp, #5]
    a814:	e3a0400f 	mov	r4, #15	; 0xf
    a818:	e2833053 	add	r3, r3, #83	; 0x53
    a81c:	e5cd2006 	strb	r2, [sp, #6]
    a820:	e5cd3008 	strb	r3, [sp, #8]
    a824:	e58d700c 	str	r7, [sp, #12]
    a828:	e5cd4017 	strb	r4, [sp, #23]
    a82c:	ebfffdbc 	bl	9f24 <spi_read_write_data>
    a830:	e1a00004 	mov	r0, r4
    a834:	e3a01009 	mov	r1, #9	; 0x9
    a838:	ebfffefa 	bl	a428 <flash_wait_device_status>
    a83c:	e3100010 	tst	r0, #16	; 0x10
    a840:	1a000001 	bne	a84c <nand_ecc_enable+0x9c>
    a844:	e3e00000 	mvn	r0, #0	; 0x0
    a848:	ea000000 	b	a850 <nand_ecc_enable+0xa0>
    a84c:	e3a00000 	mov	r0, #0	; 0x0
    a850:	e28dd064 	add	sp, sp, #100	; 0x64
    a854:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    a858:	0800d208 	.word	0x0800d208

0000a85c <flash_nand_vs_read>:
    a85c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a860:	e59f4284 	ldr	r4, [pc, #644]	; aaec <flash_nand_vs_read+0x290>
    a864:	e24dd06c 	sub	sp, sp, #108	; 0x6c
    a868:	e594305c 	ldr	r3, [r4, #92]
    a86c:	e58d0004 	str	r0, [sp, #4]
    a870:	e3530001 	cmp	r3, #1	; 0x1
    a874:	e1a08002 	mov	r8, r2
    a878:	e59db090 	ldr	fp, [sp, #144]
    a87c:	e5947028 	ldr	r7, [r4, #40]
    a880:	0a000004 	beq	a898 <flash_nand_vs_read+0x3c>
    a884:	ebfffcf1 	bl	9c50 <ssp_manager_init>
    a888:	e3a03064 	mov	r3, #100	; 0x64
    a88c:	e5843050 	str	r3, [r4, #80]
    a890:	e3a0300f 	mov	r3, #15	; 0xf
    a894:	e5843000 	str	r3, [r4]
    a898:	e59f624c 	ldr	r6, [pc, #588]	; aaec <flash_nand_vs_read+0x290>
    a89c:	e59fa24c 	ldr	sl, [pc, #588]	; aaf0 <flash_nand_vs_read+0x294>
    a8a0:	e5963044 	ldr	r3, [r6, #68]
    a8a4:	e5965000 	ldr	r5, [r6]
    a8a8:	e28d000c 	add	r0, sp, #12	; 0xc
    a8ac:	e58a3000 	str	r3, [sl]
    a8b0:	ebfffd6e 	bl	9e70 <spi_transfer_init>
    a8b4:	e355000f 	cmp	r5, #15	; 0xf
    a8b8:	1a000087 	bne	aadc <flash_nand_vs_read+0x280>
    a8bc:	e1a01007 	mov	r1, r7
    a8c0:	e1a00008 	mov	r0, r8
    a8c4:	eb001285 	bl	f2e0 <__aeabi_idiv>
    a8c8:	e59a7000 	ldr	r7, [sl]
    a8cc:	e3a03003 	mov	r3, #3	; 0x3
    a8d0:	e5cd300c 	strb	r3, [sp, #12]
    a8d4:	e59d3004 	ldr	r3, [sp, #4]
    a8d8:	e3a0c000 	mov	ip, #0	; 0x0
    a8dc:	e3a02001 	mov	r2, #1	; 0x1
    a8e0:	e1a09000 	mov	r9, r0
    a8e4:	e3a04002 	mov	r4, #2	; 0x2
    a8e8:	e1a0000b 	mov	r0, fp
    a8ec:	e1a01007 	mov	r1, r7
    a8f0:	e1a0800c 	mov	r8, ip
    a8f4:	e5cd501f 	strb	r5, [sp, #31]
    a8f8:	e58d3020 	str	r3, [sp, #32]
    a8fc:	e5cd2010 	strb	r2, [sp, #16]
    a900:	e58d4014 	str	r4, [sp, #20]
    a904:	e5cdc01d 	strb	ip, [sp, #29]
    a908:	e5cd201e 	strb	r2, [sp, #30]
    a90c:	e58d7024 	str	r7, [sp, #36]
    a910:	e58d7028 	str	r7, [sp, #40]
    a914:	e58d9034 	str	r9, [sp, #52]
    a918:	eb001270 	bl	f2e0 <__aeabi_idiv>
    a91c:	e1a05006 	mov	r5, r6
    a920:	e1a0a000 	mov	sl, r0
    a924:	e1a06008 	mov	r6, r8
    a928:	ea000034 	b	aa00 <flash_nand_vs_read+0x1a4>
    a92c:	e595200c 	ldr	r2, [r5, #12]
    a930:	e59d3034 	ldr	r3, [sp, #52]
    a934:	e3520001 	cmp	r2, #1	; 0x1
    a938:	e1a01823 	lsr	r1, r3, #16
    a93c:	e1a02423 	lsr	r2, r3, #8
    a940:	e5cd301a 	strb	r3, [sp, #26]
    a944:	13a03004 	movne	r3, #4	; 0x4
    a948:	e5cd1018 	strb	r1, [sp, #24]
    a94c:	e5cd2019 	strb	r2, [sp, #25]
    a950:	e5cd601b 	strb	r6, [sp, #27]
    a954:	15cd301c 	strbne	r3, [sp, #28]
    a958:	15cd601e 	strbne	r6, [sp, #30]
    a95c:	1a000014 	bne	a9b4 <flash_nand_vs_read+0x158>
    a960:	e3a04000 	mov	r4, #0	; 0x0
    a964:	ebffff91 	bl	a7b0 <nand_ecc_enable>
    a968:	e3500000 	cmp	r0, #0	; 0x0
    a96c:	e2844001 	add	r4, r4, #1	; 0x1
    a970:	0a000001 	beq	a97c <flash_nand_vs_read+0x120>
    a974:	e354000a 	cmp	r4, #10	; 0xa
    a978:	1afffff9 	bne	a964 <flash_nand_vs_read+0x108>
    a97c:	e595303c 	ldr	r3, [r5, #60]
    a980:	e5cd6018 	strb	r6, [sp, #24]
    a984:	e3530001 	cmp	r3, #1	; 0x1
    a988:	03a03008 	moveq	r3, #8	; 0x8
    a98c:	05cd3018 	strbeq	r3, [sp, #24]
    a990:	e3a03002 	mov	r3, #2	; 0x2
    a994:	e3a02000 	mov	r2, #0	; 0x0
    a998:	e5cd301c 	strb	r3, [sp, #28]
    a99c:	e0880009 	add	r0, r8, r9
    a9a0:	e3a03001 	mov	r3, #1	; 0x1
    a9a4:	e3a0100f 	mov	r1, #15	; 0xf
    a9a8:	e5cd2019 	strb	r2, [sp, #25]
    a9ac:	e5cd301e 	strb	r3, [sp, #30]
    a9b0:	ebffff4f 	bl	a6f4 <nand_read_page_to_cache>
    a9b4:	e5953028 	ldr	r3, [r5, #40]
    a9b8:	e28d000c 	add	r0, sp, #12	; 0xc
    a9bc:	e58d3028 	str	r3, [sp, #40]
    a9c0:	e58d3024 	str	r3, [sp, #36]
    a9c4:	ebfffd56 	bl	9f24 <spi_read_write_data>
    a9c8:	e595300c 	ldr	r3, [r5, #12]
    a9cc:	e3530001 	cmp	r3, #1	; 0x1
    a9d0:	1a000002 	bne	a9e0 <flash_nand_vs_read+0x184>
    a9d4:	ebffff6c 	bl	a78c <nand_ecc_bits_check>
    a9d8:	e3500002 	cmp	r0, #2	; 0x2
    a9dc:	0a00003e 	beq	aadc <flash_nand_vs_read+0x280>
    a9e0:	e59f3108 	ldr	r3, [pc, #264]	; aaf0 <flash_nand_vs_read+0x294>
    a9e4:	e2888001 	add	r8, r8, #1	; 0x1
    a9e8:	e5932000 	ldr	r2, [r3]
    a9ec:	e59d3034 	ldr	r3, [sp, #52]
    a9f0:	e58d2028 	str	r2, [sp, #40]
    a9f4:	e0833002 	add	r3, r3, r2
    a9f8:	e58d3034 	str	r3, [sp, #52]
    a9fc:	e58d2024 	str	r2, [sp, #36]
    aa00:	e158000a 	cmp	r8, sl
    aa04:	3affffc8 	bcc	a92c <flash_nand_vs_read+0xd0>
    aa08:	e1a01007 	mov	r1, r7
    aa0c:	e1a0000b 	mov	r0, fp
    aa10:	eb0012ba 	bl	f500 <__aeabi_idivmod>
    aa14:	e2515000 	subs	r5, r1, #0	; 0x0
    aa18:	0a000030 	beq	aae0 <flash_nand_vs_read+0x284>
    aa1c:	e59f30c8 	ldr	r3, [pc, #200]	; aaec <flash_nand_vs_read+0x290>
    aa20:	e59d2034 	ldr	r2, [sp, #52]
    aa24:	e593300c 	ldr	r3, [r3, #12]
    aa28:	e3a00000 	mov	r0, #0	; 0x0
    aa2c:	e3530001 	cmp	r3, #1	; 0x1
    aa30:	e1a03422 	lsr	r3, r2, #8
    aa34:	e1a01822 	lsr	r1, r2, #16
    aa38:	e5cd3019 	strb	r3, [sp, #25]
    aa3c:	13a03004 	movne	r3, #4	; 0x4
    aa40:	e5cd1018 	strb	r1, [sp, #24]
    aa44:	e5cd201a 	strb	r2, [sp, #26]
    aa48:	e5cd001b 	strb	r0, [sp, #27]
    aa4c:	15cd301c 	strbne	r3, [sp, #28]
    aa50:	15cd001e 	strbne	r0, [sp, #30]
    aa54:	1a000015 	bne	aab0 <flash_nand_vs_read+0x254>
    aa58:	e1a04000 	mov	r4, r0
    aa5c:	ebffff53 	bl	a7b0 <nand_ecc_enable>
    aa60:	e3500000 	cmp	r0, #0	; 0x0
    aa64:	e2844001 	add	r4, r4, #1	; 0x1
    aa68:	0a000001 	beq	aa74 <flash_nand_vs_read+0x218>
    aa6c:	e354000a 	cmp	r4, #10	; 0xa
    aa70:	1afffff9 	bne	aa5c <flash_nand_vs_read+0x200>
    aa74:	e59f2070 	ldr	r2, [pc, #112]	; aaec <flash_nand_vs_read+0x290>
    aa78:	e3a03000 	mov	r3, #0	; 0x0
    aa7c:	e592203c 	ldr	r2, [r2, #60]
    aa80:	e5cd3018 	strb	r3, [sp, #24]
    aa84:	e1520003 	cmp	r2, r3
    aa88:	e5cd3019 	strb	r3, [sp, #25]
    aa8c:	12833008 	addne	r3, r3, #8	; 0x8
    aa90:	15cd3018 	strbne	r3, [sp, #24]
    aa94:	e3a03002 	mov	r3, #2	; 0x2
    aa98:	e5cd301c 	strb	r3, [sp, #28]
    aa9c:	e0880009 	add	r0, r8, r9
    aaa0:	e3a03001 	mov	r3, #1	; 0x1
    aaa4:	e3a0100f 	mov	r1, #15	; 0xf
    aaa8:	e5cd301e 	strb	r3, [sp, #30]
    aaac:	ebffff10 	bl	a6f4 <nand_read_page_to_cache>
    aab0:	e28d000c 	add	r0, sp, #12	; 0xc
    aab4:	e58d5028 	str	r5, [sp, #40]
    aab8:	e58d5024 	str	r5, [sp, #36]
    aabc:	ebfffd18 	bl	9f24 <spi_read_write_data>
    aac0:	e59f3024 	ldr	r3, [pc, #36]	; aaec <flash_nand_vs_read+0x290>
    aac4:	e593300c 	ldr	r3, [r3, #12]
    aac8:	e3530001 	cmp	r3, #1	; 0x1
    aacc:	1a000003 	bne	aae0 <flash_nand_vs_read+0x284>
    aad0:	ebffff2d 	bl	a78c <nand_ecc_bits_check>
    aad4:	e3500002 	cmp	r0, #2	; 0x2
    aad8:	1a000000 	bne	aae0 <flash_nand_vs_read+0x284>
    aadc:	e3e0b000 	mvn	fp, #0	; 0x0
    aae0:	e1a0000b 	mov	r0, fp
    aae4:	e28dd06c 	add	sp, sp, #108	; 0x6c
    aae8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    aaec:	0800d208 	.word	0x0800d208
    aaf0:	0800d0e4 	.word	0x0800d0e4

0000aaf4 <flash_nand_vs_write>:
    aaf4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    aaf8:	e59f42a4 	ldr	r4, [pc, #676]	; ada4 <flash_nand_vs_write+0x2b0>
    aafc:	e24dd074 	sub	sp, sp, #116	; 0x74
    ab00:	e594305c 	ldr	r3, [r4, #92]
    ab04:	e58d0004 	str	r0, [sp, #4]
    ab08:	e3530001 	cmp	r3, #1	; 0x1
    ab0c:	e1a05002 	mov	r5, r2
    ab10:	e59db098 	ldr	fp, [sp, #152]
    ab14:	0a000004 	beq	ab2c <flash_nand_vs_write+0x38>
    ab18:	ebfffc4c 	bl	9c50 <ssp_manager_init>
    ab1c:	e3a03064 	mov	r3, #100	; 0x64
    ab20:	e5843050 	str	r3, [r4, #80]
    ab24:	e3a0300f 	mov	r3, #15	; 0xf
    ab28:	e5843000 	str	r3, [r4]
    ab2c:	e59f8270 	ldr	r8, [pc, #624]	; ada4 <flash_nand_vs_write+0x2b0>
    ab30:	e59f9270 	ldr	r9, [pc, #624]	; ada8 <flash_nand_vs_write+0x2b4>
    ab34:	e5983044 	ldr	r3, [r8, #68]
    ab38:	e28d0014 	add	r0, sp, #20	; 0x14
    ab3c:	e5893000 	str	r3, [r9]
    ab40:	e5984028 	ldr	r4, [r8, #40]
    ab44:	ebfffcc9 	bl	9e70 <spi_transfer_init>
    ab48:	e5987000 	ldr	r7, [r8]
    ab4c:	e357000f 	cmp	r7, #15	; 0xf
    ab50:	13e0b000 	mvnne	fp, #0	; 0x0
    ab54:	1a00008f 	bne	ad98 <flash_nand_vs_write+0x2a4>
    ab58:	e1a01004 	mov	r1, r4
    ab5c:	e1a00005 	mov	r0, r5
    ab60:	eb0011de 	bl	f2e0 <__aeabi_idiv>
    ab64:	e599a000 	ldr	sl, [r9]
    ab68:	e3a04000 	mov	r4, #0	; 0x0
    ab6c:	e3a05003 	mov	r5, #3	; 0x3
    ab70:	e3a03001 	mov	r3, #1	; 0x1
    ab74:	e58d000c 	str	r0, [sp, #12]
    ab78:	e3a06002 	mov	r6, #2	; 0x2
    ab7c:	e1a0000b 	mov	r0, fp
    ab80:	e1a0100a 	mov	r1, sl
    ab84:	e5cd3018 	strb	r3, [sp, #24]
    ab88:	e5cd7027 	strb	r7, [sp, #39]
    ab8c:	e5cd6014 	strb	r6, [sp, #20]
    ab90:	e58d501c 	str	r5, [sp, #28]
    ab94:	e5cd5024 	strb	r5, [sp, #36]
    ab98:	e5cd4026 	strb	r4, [sp, #38]
    ab9c:	eb0011cf 	bl	f2e0 <__aeabi_idiv>
    aba0:	e59d300c 	ldr	r3, [sp, #12]
    aba4:	e15b000a 	cmp	fp, sl
    aba8:	b1a0200b 	movlt	r2, fp
    abac:	a1a0200a 	movge	r2, sl
    abb0:	e58d303c 	str	r3, [sp, #60]
    abb4:	e59d3004 	ldr	r3, [sp, #4]
    abb8:	e1a05008 	mov	r5, r8
    abbc:	e1a07004 	mov	r7, r4
    abc0:	e58d0008 	str	r0, [sp, #8]
    abc4:	e58d3034 	str	r3, [sp, #52]
    abc8:	e58d202c 	str	r2, [sp, #44]
    abcc:	e58d4028 	str	r4, [sp, #40]
    abd0:	e58d2038 	str	r2, [sp, #56]
    abd4:	e58d4040 	str	r4, [sp, #64]
    abd8:	e28d6014 	add	r6, sp, #20	; 0x14
    abdc:	e1a08004 	mov	r8, r4
    abe0:	ea000032 	b	acb0 <flash_nand_vs_write+0x1bc>
    abe4:	ebfffde8 	bl	a38c <flash_write_enable>
    abe8:	e3a01007 	mov	r1, #7	; 0x7
    abec:	e3a0000f 	mov	r0, #15	; 0xf
    abf0:	ebfffe0c 	bl	a428 <flash_wait_device_status>
    abf4:	e3a01006 	mov	r1, #6	; 0x6
    abf8:	e3a0000f 	mov	r0, #15	; 0xf
    abfc:	ebfffe09 	bl	a428 <flash_wait_device_status>
    ac00:	e59d303c 	ldr	r3, [sp, #60]
    ac04:	e595200c 	ldr	r2, [r5, #12]
    ac08:	e1a01823 	lsr	r1, r3, #16
    ac0c:	e3520001 	cmp	r2, #1	; 0x1
    ac10:	e5cd3022 	strb	r3, [sp, #34]
    ac14:	e1a02423 	lsr	r2, r3, #8
    ac18:	e3a03003 	mov	r3, #3	; 0x3
    ac1c:	e5cd1020 	strb	r1, [sp, #32]
    ac20:	e5cd2021 	strb	r2, [sp, #33]
    ac24:	e5cd8023 	strb	r8, [sp, #35]
    ac28:	e5cd3024 	strb	r3, [sp, #36]
    ac2c:	1a00000b 	bne	ac60 <flash_nand_vs_write+0x16c>
    ac30:	e3a04000 	mov	r4, #0	; 0x0
    ac34:	ebfffedd 	bl	a7b0 <nand_ecc_enable>
    ac38:	e3500000 	cmp	r0, #0	; 0x0
    ac3c:	e2844001 	add	r4, r4, #1	; 0x1
    ac40:	0a000001 	beq	ac4c <flash_nand_vs_write+0x158>
    ac44:	e354000a 	cmp	r4, #10	; 0xa
    ac48:	1afffff9 	bne	ac34 <flash_nand_vs_write+0x140>
    ac4c:	e3a03002 	mov	r3, #2	; 0x2
    ac50:	e5cd8020 	strb	r8, [sp, #32]
    ac54:	e5cd8021 	strb	r8, [sp, #33]
    ac58:	e5cd3024 	strb	r3, [sp, #36]
    ac5c:	e5cd8026 	strb	r8, [sp, #38]
    ac60:	e5993000 	ldr	r3, [r9]
    ac64:	e1a00006 	mov	r0, r6
    ac68:	e58d302c 	str	r3, [sp, #44]
    ac6c:	e58d3038 	str	r3, [sp, #56]
    ac70:	ebfffcab 	bl	9f24 <spi_read_write_data>
    ac74:	e59d303c 	ldr	r3, [sp, #60]
    ac78:	e5992000 	ldr	r2, [r9]
    ac7c:	e595100c 	ldr	r1, [r5, #12]
    ac80:	e0833002 	add	r3, r3, r2
    ac84:	e3510001 	cmp	r1, #1	; 0x1
    ac88:	e58d303c 	str	r3, [sp, #60]
    ac8c:	1a000006 	bne	acac <flash_nand_vs_write+0x1b8>
    ac90:	e59d300c 	ldr	r3, [sp, #12]
    ac94:	e281100e 	add	r1, r1, #14	; 0xe
    ac98:	e0870003 	add	r0, r7, r3
    ac9c:	ebfffe77 	bl	a680 <nand_write_cache_to_memory>
    aca0:	ebfffeae 	bl	a760 <nand_blk_prg_era_fail>
    aca4:	e3500001 	cmp	r0, #1	; 0x1
    aca8:	0a000039 	beq	ad94 <flash_nand_vs_write+0x2a0>
    acac:	e2877001 	add	r7, r7, #1	; 0x1
    acb0:	e59d3008 	ldr	r3, [sp, #8]
    acb4:	e1570003 	cmp	r7, r3
    acb8:	3affffc9 	bcc	abe4 <flash_nand_vs_write+0xf0>
    acbc:	e1a0100a 	mov	r1, sl
    acc0:	e1a0000b 	mov	r0, fp
    acc4:	eb00120d 	bl	f500 <__aeabi_idivmod>
    acc8:	e2515000 	subs	r5, r1, #0	; 0x0
    accc:	0a000031 	beq	ad98 <flash_nand_vs_write+0x2a4>
    acd0:	ebfffdad 	bl	a38c <flash_write_enable>
    acd4:	e3a01007 	mov	r1, #7	; 0x7
    acd8:	e3a0000f 	mov	r0, #15	; 0xf
    acdc:	ebfffdd1 	bl	a428 <flash_wait_device_status>
    ace0:	e3a01006 	mov	r1, #6	; 0x6
    ace4:	e3a0000f 	mov	r0, #15	; 0xf
    ace8:	ebfffdce 	bl	a428 <flash_wait_device_status>
    acec:	e59f30b0 	ldr	r3, [pc, #176]	; ada4 <flash_nand_vs_write+0x2b0>
    acf0:	e59d203c 	ldr	r2, [sp, #60]
    acf4:	e593300c 	ldr	r3, [r3, #12]
    acf8:	e1a01822 	lsr	r1, r2, #16
    acfc:	e3530001 	cmp	r3, #1	; 0x1
    ad00:	e1a03422 	lsr	r3, r2, #8
    ad04:	e3a00000 	mov	r0, #0	; 0x0
    ad08:	e5cd3021 	strb	r3, [sp, #33]
    ad0c:	e3a03003 	mov	r3, #3	; 0x3
    ad10:	e5cd1020 	strb	r1, [sp, #32]
    ad14:	e5cd2022 	strb	r2, [sp, #34]
    ad18:	e5cd3024 	strb	r3, [sp, #36]
    ad1c:	e5cd0023 	strb	r0, [sp, #35]
    ad20:	1a00000c 	bne	ad58 <flash_nand_vs_write+0x264>
    ad24:	e1a04000 	mov	r4, r0
    ad28:	ebfffea0 	bl	a7b0 <nand_ecc_enable>
    ad2c:	e3500000 	cmp	r0, #0	; 0x0
    ad30:	e2844001 	add	r4, r4, #1	; 0x1
    ad34:	0a000001 	beq	ad40 <flash_nand_vs_write+0x24c>
    ad38:	e354000a 	cmp	r4, #10	; 0xa
    ad3c:	1afffff9 	bne	ad28 <flash_nand_vs_write+0x234>
    ad40:	e3a02000 	mov	r2, #0	; 0x0
    ad44:	e3a03002 	mov	r3, #2	; 0x2
    ad48:	e5cd3024 	strb	r3, [sp, #36]
    ad4c:	e5cd2026 	strb	r2, [sp, #38]
    ad50:	e5cd2020 	strb	r2, [sp, #32]
    ad54:	e5cd2021 	strb	r2, [sp, #33]
    ad58:	e28d0014 	add	r0, sp, #20	; 0x14
    ad5c:	e58d502c 	str	r5, [sp, #44]
    ad60:	e58d5038 	str	r5, [sp, #56]
    ad64:	ebfffc6e 	bl	9f24 <spi_read_write_data>
    ad68:	e59f3034 	ldr	r3, [pc, #52]	; ada4 <flash_nand_vs_write+0x2b0>
    ad6c:	e593300c 	ldr	r3, [r3, #12]
    ad70:	e3530001 	cmp	r3, #1	; 0x1
    ad74:	1a000007 	bne	ad98 <flash_nand_vs_write+0x2a4>
    ad78:	e59d300c 	ldr	r3, [sp, #12]
    ad7c:	e3a0100f 	mov	r1, #15	; 0xf
    ad80:	e0870003 	add	r0, r7, r3
    ad84:	ebfffe3d 	bl	a680 <nand_write_cache_to_memory>
    ad88:	ebfffe74 	bl	a760 <nand_blk_prg_era_fail>
    ad8c:	e3500001 	cmp	r0, #1	; 0x1
    ad90:	1a000000 	bne	ad98 <flash_nand_vs_write+0x2a4>
    ad94:	e3e0b002 	mvn	fp, #2	; 0x2
    ad98:	e1a0000b 	mov	r0, fp
    ad9c:	e28dd074 	add	sp, sp, #116	; 0x74
    ada0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ada4:	0800d208 	.word	0x0800d208
    ada8:	0800d0e4 	.word	0x0800d0e4

0000adac <flash_nand_vs_erase>:
    adac:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    adb0:	e59f4138 	ldr	r4, [pc, #312]	; aef0 <flash_nand_vs_erase+0x144>
    adb4:	e24dd060 	sub	sp, sp, #96	; 0x60
    adb8:	e594305c 	ldr	r3, [r4, #92]
    adbc:	e1a06000 	mov	r6, r0
    adc0:	e3530001 	cmp	r3, #1	; 0x1
    adc4:	e1a07001 	mov	r7, r1
    adc8:	e1a05002 	mov	r5, r2
    adcc:	0a000004 	beq	ade4 <flash_nand_vs_erase+0x38>
    add0:	ebfffb9e 	bl	9c50 <ssp_manager_init>
    add4:	e3a03064 	mov	r3, #100	; 0x64
    add8:	e5843050 	str	r3, [r4, #80]
    addc:	e3a0300f 	mov	r3, #15	; 0xf
    ade0:	e5843000 	str	r3, [r4]
    ade4:	e59f8104 	ldr	r8, [pc, #260]	; aef0 <flash_nand_vs_erase+0x144>
    ade8:	e5984000 	ldr	r4, [r8]
    adec:	e354000f 	cmp	r4, #15	; 0xf
    adf0:	13e00000 	mvnne	r0, #0	; 0x0
    adf4:	1a00003b 	bne	aee8 <flash_nand_vs_erase+0x13c>
    adf8:	e59f20f4 	ldr	r2, [pc, #244]	; aef4 <flash_nand_vs_erase+0x148>
    adfc:	e3a03000 	mov	r3, #0	; 0x0
    ae00:	e0922006 	adds	r2, r2, r6
    ae04:	e0a33007 	adc	r3, r3, r7
    ae08:	e0922005 	adds	r2, r2, r5
    ae0c:	e2a33000 	adc	r3, r3, #0	; 0x0
    ae10:	e1a018a2 	lsr	r1, r2, #17
    ae14:	e181a783 	orr	sl, r1, r3, lsl #15
    ae18:	ebfffd5b 	bl	a38c <flash_write_enable>
    ae1c:	e1a00004 	mov	r0, r4
    ae20:	e3a01007 	mov	r1, #7	; 0x7
    ae24:	ebfffd7f 	bl	a428 <flash_wait_device_status>
    ae28:	e3a01006 	mov	r1, #6	; 0x6
    ae2c:	e1a00004 	mov	r0, r4
    ae30:	ebfffd7c 	bl	a428 <flash_wait_device_status>
    ae34:	e28d0004 	add	r0, sp, #4	; 0x4
    ae38:	ebfffc0c 	bl	9e70 <spi_transfer_init>
    ae3c:	e5983000 	ldr	r3, [r8]
    ae40:	e3a04000 	mov	r4, #0	; 0x0
    ae44:	e353000f 	cmp	r3, #15	; 0xf
    ae48:	05cd3017 	strbeq	r3, [sp, #23]
    ae4c:	e1a058a6 	lsr	r5, r6, #17
    ae50:	e3e03027 	mvn	r3, #39	; 0x27
    ae54:	e3a0001f 	mov	r0, #31	; 0x1f
    ae58:	e3a010a0 	mov	r1, #160	; 0xa0
    ae5c:	e1a02004 	mov	r2, r4
    ae60:	e5cd3004 	strb	r3, [sp, #4]
    ae64:	e1855787 	orr	r5, r5, r7, lsl #15
    ae68:	e58d400c 	str	r4, [sp, #12]
    ae6c:	e5cd4005 	strb	r4, [sp, #5]
    ae70:	e59f9078 	ldr	r9, [pc, #120]	; aef0 <flash_nand_vs_erase+0x144>
    ae74:	ebfffd2b 	bl	a328 <flash_set_device_status>
    ae78:	e28d7004 	add	r7, sp, #4	; 0x4
    ae7c:	e3a08004 	mov	r8, #4	; 0x4
    ae80:	ea000013 	b	aed4 <flash_nand_vs_erase+0x128>
    ae84:	ebfffd40 	bl	a38c <flash_write_enable>
    ae88:	e3a01007 	mov	r1, #7	; 0x7
    ae8c:	e3a0000f 	mov	r0, #15	; 0xf
    ae90:	ebfffd64 	bl	a428 <flash_wait_device_status>
    ae94:	e3a01006 	mov	r1, #6	; 0x6
    ae98:	e3a0000f 	mov	r0, #15	; 0xf
    ae9c:	ebfffd61 	bl	a428 <flash_wait_device_status>
    aea0:	e5993030 	ldr	r3, [r9, #48]
    aea4:	e1a00007 	mov	r0, r7
    aea8:	e0030693 	mul	r3, r3, r6
    aeac:	e1a02403 	lsl	r2, r3, #8
    aeb0:	e2033cff 	and	r3, r3, #65280	; 0xff00
    aeb4:	e1822423 	orr	r2, r2, r3, lsr #8
    aeb8:	e1c720b2 	strh	r2, [r7, #2]
    aebc:	e5cd8008 	strb	r8, [sp, #8]
    aec0:	ebfffc17 	bl	9f24 <spi_read_write_data>
    aec4:	e3a01006 	mov	r1, #6	; 0x6
    aec8:	e3a0000f 	mov	r0, #15	; 0xf
    aecc:	ebfffd55 	bl	a428 <flash_wait_device_status>
    aed0:	ebfffe22 	bl	a760 <nand_blk_prg_era_fail>
    aed4:	e0846005 	add	r6, r4, r5
    aed8:	e156000a 	cmp	r6, sl
    aedc:	e2844001 	add	r4, r4, #1	; 0x1
    aee0:	baffffe7 	blt	ae84 <flash_nand_vs_erase+0xd8>
    aee4:	e3a00000 	mov	r0, #0	; 0x0
    aee8:	e28dd060 	add	sp, sp, #96	; 0x60
    aeec:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    aef0:	0800d208 	.word	0x0800d208
    aef4:	0001ffff 	.word	0x0001ffff

0000aef8 <spi_module_init>:
    aef8:	e3500000 	cmp	r0, #0	; 0x0
    aefc:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    af00:	03e00001 	mvneq	r0, #1	; 0x1
    af04:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    af08:	e59f00ac 	ldr	r0, [pc, #172]	; afbc <spi_module_init+0xc4>
    af0c:	ebffd859 	bl	1078 <module_get_clock>
    af10:	e2507000 	subs	r7, r0, #0	; 0x0
    af14:	b3e00000 	mvnlt	r0, #0	; 0x0
    af18:	a3a04002 	movge	r4, #2	; 0x2
    af1c:	a59f809c 	ldrge	r8, [pc, #156]	; afc0 <spi_module_init+0xc8>
    af20:	aa00000d 	bge	af5c <spi_module_init+0x64>
    af24:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    af28:	e1a05001 	mov	r5, r1
    af2c:	e1a01006 	mov	r1, r6
    af30:	e1a00007 	mov	r0, r7
    af34:	eb0010e9 	bl	f2e0 <__aeabi_idiv>
    af38:	e1500008 	cmp	r0, r8
    af3c:	e0866004 	add	r6, r6, r4
    af40:	e2851001 	add	r1, r5, #1	; 0x1
    af44:	da000007 	ble	af68 <spi_module_init+0x70>
    af48:	e3510c01 	cmp	r1, #256	; 0x100
    af4c:	1afffff5 	bne	af28 <spi_module_init+0x30>
    af50:	e2844002 	add	r4, r4, #2	; 0x2
    af54:	e3540c01 	cmp	r4, #256	; 0x100
    af58:	0a000003 	beq	af6c <spi_module_init+0x74>
    af5c:	e1a06004 	mov	r6, r4
    af60:	e3a05000 	mov	r5, #0	; 0x0
    af64:	eafffff0 	b	af2c <spi_module_init+0x34>
    af68:	e1a01005 	mov	r1, r5
    af6c:	e35400fe 	cmp	r4, #254	; 0xfe
    af70:	c35100ff 	cmpgt	r1, #255	; 0xff
    af74:	d3a03000 	movle	r3, #0	; 0x0
    af78:	c3a03001 	movgt	r3, #1	; 0x1
    af7c:	e3530000 	cmp	r3, #0	; 0x0
    af80:	e59f203c 	ldr	r2, [pc, #60]	; afc4 <spi_module_init+0xcc>
    af84:	13a01027 	movne	r1, #39	; 0x27
    af88:	e3a03000 	mov	r3, #0	; 0x0
    af8c:	13a04002 	movne	r4, #2	; 0x2
    af90:	e1a01401 	lsl	r1, r1, #8
    af94:	e5824010 	str	r4, [r2, #16]
    af98:	e38110c7 	orr	r1, r1, #199	; 0xc7
    af9c:	e5823014 	str	r3, [r2, #20]
    afa0:	e1a00003 	mov	r0, r3
    afa4:	e5823000 	str	r3, [r2]
    afa8:	e5823004 	str	r3, [r2, #4]
    afac:	e283300a 	add	r3, r3, #10	; 0xa
    afb0:	e5821000 	str	r1, [r2]
    afb4:	e5823004 	str	r3, [r2, #4]
    afb8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    afbc:	2d00a2a0 	.word	0x2d00a2a0
    afc0:	004c4b40 	.word	0x004c4b40
    afc4:	28500000 	.word	0x28500000

0000afc8 <flash_nand_vs_reset>:
    afc8:	e92d4010 	push	{r4, lr}
    afcc:	e59f40d4 	ldr	r4, [pc, #212]	; b0a8 <flash_nand_vs_reset+0xe0>
    afd0:	e594305c 	ldr	r3, [r4, #92]
    afd4:	e3530001 	cmp	r3, #1	; 0x1
    afd8:	0a000004 	beq	aff0 <flash_nand_vs_reset+0x28>
    afdc:	ebfffb1b 	bl	9c50 <ssp_manager_init>
    afe0:	e3a03064 	mov	r3, #100	; 0x64
    afe4:	e5843050 	str	r3, [r4, #80]
    afe8:	e3a0300f 	mov	r3, #15	; 0xf
    afec:	e5843000 	str	r3, [r4]
    aff0:	e59f30b0 	ldr	r3, [pc, #176]	; b0a8 <flash_nand_vs_reset+0xe0>
    aff4:	e5933000 	ldr	r3, [r3]
    aff8:	e353000f 	cmp	r3, #15	; 0xf
    affc:	1a000026 	bne	b09c <flash_nand_vs_reset+0xd4>
    b000:	e59f00a4 	ldr	r0, [pc, #164]	; b0ac <flash_nand_vs_reset+0xe4>
    b004:	ebffd5cc 	bl	73c <module_enable>
    b008:	e59f20a0 	ldr	r2, [pc, #160]	; b0b0 <flash_nand_vs_reset+0xe8>
    b00c:	e59f00a0 	ldr	r0, [pc, #160]	; b0b4 <flash_nand_vs_reset+0xec>
    b010:	e5923404 	ldr	r3, [r2, #1028]
    b014:	e3833008 	orr	r3, r3, #8	; 0x8
    b018:	e5823404 	str	r3, [r2, #1028]
    b01c:	ebffffb5 	bl	aef8 <spi_module_init>
    b020:	e59f2090 	ldr	r2, [pc, #144]	; b0b8 <flash_nand_vs_reset+0xf0>
    b024:	e3a01000 	mov	r1, #0	; 0x0
    b028:	e5923000 	ldr	r3, [r2]
    b02c:	e3a0000b 	mov	r0, #11	; 0xb
    b030:	e3833004 	orr	r3, r3, #4	; 0x4
    b034:	e5823000 	str	r3, [r2]
    b038:	ebffd605 	bl	854 <pads_init_module>
    b03c:	e3a01002 	mov	r1, #2	; 0x2
    b040:	e3a00004 	mov	r0, #4	; 0x4
    b044:	e3a02001 	mov	r2, #1	; 0x1
    b048:	e3a03000 	mov	r3, #0	; 0x0
    b04c:	ebffd6d9 	bl	bb8 <pads_chmod>
    b050:	e59f1064 	ldr	r1, [pc, #100]	; b0bc <flash_nand_vs_reset+0xf4>
    b054:	ea000000 	b	b05c <flash_nand_vs_reset+0x94>
    b058:	e5913008 	ldr	r3, [r1, #8]
    b05c:	e591300c 	ldr	r3, [r1, #12]
    b060:	e59f2054 	ldr	r2, [pc, #84]	; b0bc <flash_nand_vs_reset+0xf4>
    b064:	e3130004 	tst	r3, #4	; 0x4
    b068:	1afffffa 	bne	b058 <flash_nand_vs_reset+0x90>
    b06c:	e5923014 	ldr	r3, [r2, #20]
    b070:	e3833002 	orr	r3, r3, #2	; 0x2
    b074:	e5823014 	str	r3, [r2, #20]
    b078:	e5923014 	ldr	r3, [r2, #20]
    b07c:	e3833008 	orr	r3, r3, #8	; 0x8
    b080:	e5823014 	str	r3, [r2, #20]
    b084:	e5923014 	ldr	r3, [r2, #20]
    b088:	e3833004 	orr	r3, r3, #4	; 0x4
    b08c:	e5823014 	str	r3, [r2, #20]
    b090:	e5923014 	ldr	r3, [r2, #20]
    b094:	e3833001 	orr	r3, r3, #1	; 0x1
    b098:	e5823014 	str	r3, [r2, #20]
    b09c:	ebfffccd 	bl	a3d8 <nand_flash_reset>
    b0a0:	e3a00000 	mov	r0, #0	; 0x0
    b0a4:	e8bd8010 	pop	{r4, pc}
    b0a8:	0800d208 	.word	0x0800d208
    b0ac:	2b011400 	.word	0x2b011400
    b0b0:	2b011000 	.word	0x2b011000
    b0b4:	000123d3 	.word	0x000123d3
    b0b8:	2d009000 	.word	0x2d009000
    b0bc:	28500000 	.word	0x28500000

0000b0c0 <ram_addr2offs>:
    b0c0:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    b0c4:	93a02101 	movls	r2, #1073741824	; 0x40000000
    b0c8:	e3a01000 	mov	r1, #0	; 0x0
    b0cc:	83a02103 	movhi	r2, #-1073741824	; 0xc0000000
    b0d0:	93a03000 	movls	r3, #0	; 0x0
    b0d4:	83e03000 	mvnhi	r3, #0	; 0x0
    b0d8:	e0922000 	adds	r2, r2, r0
    b0dc:	e0a33001 	adc	r3, r3, r1
    b0e0:	e1a01003 	mov	r1, r3
    b0e4:	e1a00002 	mov	r0, r2
    b0e8:	e12fff1e 	bx	lr

0000b0ec <ram_offs2addr>:
    b0ec:	e3510000 	cmp	r1, #0	; 0x0
    b0f0:	ca000002 	bgt	b100 <ram_offs2addr+0x14>
    b0f4:	1a000003 	bne	b108 <ram_offs2addr+0x1c>
    b0f8:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    b0fc:	9a000001 	bls	b108 <ram_offs2addr+0x1c>
    b100:	e2800103 	add	r0, r0, #-1073741824	; 0xc0000000
    b104:	e12fff1e 	bx	lr
    b108:	e2800101 	add	r0, r0, #1073741824	; 0x40000000
    b10c:	e12fff1e 	bx	lr

0000b110 <ram_vs_align>:
    b110:	e3a00c02 	mov	r0, #512	; 0x200
    b114:	e12fff1e 	bx	lr

0000b118 <ram_vs_reset>:
    b118:	e3a00000 	mov	r0, #0	; 0x0
    b11c:	e12fff1e 	bx	lr

0000b120 <ram_vs_write>:
    b120:	e92d4010 	push	{r4, lr}
    b124:	e3530000 	cmp	r3, #0	; 0x0
    b128:	e1a01000 	mov	r1, r0
    b12c:	e59d4008 	ldr	r4, [sp, #8]
    b130:	ca000002 	bgt	b140 <ram_vs_write+0x20>
    b134:	1a000003 	bne	b148 <ram_vs_write+0x28>
    b138:	e3720107 	cmn	r2, #-1073741823	; 0xc0000001
    b13c:	9a000001 	bls	b148 <ram_vs_write+0x28>
    b140:	e2820103 	add	r0, r2, #-1073741824	; 0xc0000000
    b144:	ea000000 	b	b14c <ram_vs_write+0x2c>
    b148:	e2820101 	add	r0, r2, #1073741824	; 0x40000000
    b14c:	e1a02004 	mov	r2, r4
    b150:	ebffd8a0 	bl	13d8 <memcpy>
    b154:	e1a00004 	mov	r0, r4
    b158:	e8bd8010 	pop	{r4, pc}

0000b15c <ram_vs_read>:
    b15c:	e92d4010 	push	{r4, lr}
    b160:	e3530000 	cmp	r3, #0	; 0x0
    b164:	e59d4008 	ldr	r4, [sp, #8]
    b168:	ca000002 	bgt	b178 <ram_vs_read+0x1c>
    b16c:	1a000003 	bne	b180 <ram_vs_read+0x24>
    b170:	e3720107 	cmn	r2, #-1073741823	; 0xc0000001
    b174:	9a000001 	bls	b180 <ram_vs_read+0x24>
    b178:	e2821103 	add	r1, r2, #-1073741824	; 0xc0000000
    b17c:	ea000000 	b	b184 <ram_vs_read+0x28>
    b180:	e2821101 	add	r1, r2, #1073741824	; 0x40000000
    b184:	e1a02004 	mov	r2, r4
    b188:	ebffd892 	bl	13d8 <memcpy>
    b18c:	e1a00004 	mov	r0, r4
    b190:	e8bd8010 	pop	{r4, pc}

0000b194 <udc_get_cable_state>:
    b194:	e3a00001 	mov	r0, #1	; 0x1
    b198:	e12fff1e 	bx	lr

0000b19c <udc_disconnect>:
    b19c:	e59f103c 	ldr	r1, [pc, #60]	; b1e0 <udc_disconnect+0x44>
    b1a0:	e59f303c 	ldr	r3, [pc, #60]	; b1e4 <udc_disconnect+0x48>
    b1a4:	e5912010 	ldr	r2, [r1, #16]
    b1a8:	e3a00000 	mov	r0, #0	; 0x0
    b1ac:	e3822001 	orr	r2, r2, #1	; 0x1
    b1b0:	e92d4010 	push	{r4, lr}
    b1b4:	e5c30000 	strb	r0, [r3]
    b1b8:	e1a04001 	mov	r4, r1
    b1bc:	e5812010 	str	r2, [r1, #16]
    b1c0:	ea000000 	b	b1c8 <udc_disconnect+0x2c>
    b1c4:	ebfff613 	bl	8a18 <udelay>
    b1c8:	e5943010 	ldr	r3, [r4, #16]
    b1cc:	e3a0000a 	mov	r0, #10	; 0xa
    b1d0:	e2133001 	ands	r3, r3, #1	; 0x1
    b1d4:	1afffffa 	bne	b1c4 <udc_disconnect+0x28>
    b1d8:	e1a00003 	mov	r0, r3
    b1dc:	e8bd8010 	pop	{r4, pc}
    b1e0:	27100000 	.word	0x27100000
    b1e4:	0800d274 	.word	0x0800d274

0000b1e8 <udc_event_handle>:
    b1e8:	e59f2768 	ldr	r2, [pc, #1896]	; b958 <udc_event_handle+0x770>
    b1ec:	e59f3768 	ldr	r3, [pc, #1896]	; b95c <udc_event_handle+0x774>
    b1f0:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    b1f4:	e5934014 	ldr	r4, [r3, #20]
    b1f8:	e5923000 	ldr	r3, [r2]
    b1fc:	e59f0758 	ldr	r0, [pc, #1880]	; b95c <udc_event_handle+0x774>
    b200:	e1530004 	cmp	r3, r4
    b204:	e204ca03 	and	ip, r4, #12288	; 0x3000
    b208:	15824000 	strne	r4, [r2]
    b20c:	e2043b03 	and	r3, r4, #3072	; 0xc00
    b210:	e35c0a01 	cmp	ip, #4096	; 0x1000
    b214:	e24dd008 	sub	sp, sp, #8	; 0x8
    b218:	e5803014 	str	r3, [r0, #20]
    b21c:	0a000002 	beq	b22c <udc_event_handle+0x44>
    b220:	e35c0a02 	cmp	ip, #8192	; 0x2000
    b224:	1a000034 	bne	b2fc <udc_event_handle+0x114>
    b228:	ea000025 	b	b2c4 <udc_event_handle+0xdc>
    b22c:	e3a03302 	mov	r3, #134217728	; 0x8000000
    b230:	e5803b00 	str	r3, [r0, #2816]
    b234:	e5803b40 	str	r3, [r0, #2880]
    b238:	e59f3720 	ldr	r3, [pc, #1824]	; b960 <udc_event_handle+0x778>
    b23c:	e3a02009 	mov	r2, #9	; 0x9
    b240:	e580381c 	str	r3, [r0, #2076]
    b244:	e3a03b02 	mov	r3, #2048	; 0x800
    b248:	e5802814 	str	r2, [r0, #2068]
    b24c:	e5802810 	str	r2, [r0, #2064]
    b250:	e5803024 	str	r3, [r0, #36]
    b254:	e2833301 	add	r3, r3, #67108864	; 0x4000000
    b258:	e5803028 	str	r3, [r0, #40]
    b25c:	e2833b01 	add	r3, r3, #1024	; 0x400
    b260:	e5803104 	str	r3, [r0, #260]
    b264:	e59f36f8 	ldr	r3, [pc, #1784]	; b964 <udc_event_handle+0x77c>
    b268:	e1a05000 	mov	r5, r0
    b26c:	e580305c 	str	r3, [r0, #92]
    b270:	e5903010 	ldr	r3, [r0, #16]
    b274:	e3c33d1f 	bic	r3, r3, #1984	; 0x7c0
    b278:	e3833e43 	orr	r3, r3, #1072	; 0x430
    b27c:	e5803010 	str	r3, [r0, #16]
    b280:	ea000000 	b	b288 <udc_event_handle+0xa0>
    b284:	ebfff5e3 	bl	8a18 <udelay>
    b288:	e5953010 	ldr	r3, [r5, #16]
    b28c:	e3a00001 	mov	r0, #1	; 0x1
    b290:	e3130030 	tst	r3, #48	; 0x30
    b294:	e59f16c0 	ldr	r1, [pc, #1728]	; b95c <udc_event_handle+0x774>
    b298:	1afffff9 	bne	b284 <udc_event_handle+0x9c>
    b29c:	e59f36c4 	ldr	r3, [pc, #1732]	; b968 <udc_event_handle+0x780>
    b2a0:	e2800009 	add	r0, r0, #9	; 0x9
    b2a4:	e5932024 	ldr	r2, [r3, #36]
    b2a8:	e59f36bc 	ldr	r3, [pc, #1724]	; b96c <udc_event_handle+0x784>
    b2ac:	e5813b10 	str	r3, [r1, #2832]
    b2b0:	e3a03a01 	mov	r3, #4096	; 0x1000
    b2b4:	e5812b14 	str	r2, [r1, #2836]
    b2b8:	e5813014 	str	r3, [r1, #20]
    b2bc:	ebfff5d5 	bl	8a18 <udelay>
    b2c0:	ea00000d 	b	b2fc <udc_event_handle+0x114>
    b2c4:	e5902808 	ldr	r2, [r0, #2056]
    b2c8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    b2cc:	e5803b00 	str	r3, [r0, #2816]
    b2d0:	e1a020a2 	lsr	r2, r2, #1
    b2d4:	e59f368c 	ldr	r3, [pc, #1676]	; b968 <udc_event_handle+0x780>
    b2d8:	e5901018 	ldr	r1, [r0, #24]
    b2dc:	e2022003 	and	r2, r2, #3	; 0x3
    b2e0:	e5c3201c 	strb	r2, [r3, #28]
    b2e4:	e59f3684 	ldr	r3, [pc, #1668]	; b970 <udc_event_handle+0x788>
    b2e8:	e3811008 	orr	r1, r1, #8	; 0x8
    b2ec:	e3a02001 	mov	r2, #1	; 0x1
    b2f0:	e5801018 	str	r1, [r0, #24]
    b2f4:	e5832000 	str	r2, [r3]
    b2f8:	e580c014 	str	ip, [r0, #20]
    b2fc:	e59f3658 	ldr	r3, [pc, #1624]	; b95c <udc_event_handle+0x774>
    b300:	e3140701 	tst	r4, #262144	; 0x40000
    b304:	e5939818 	ldr	r9, [r3, #2072]
    b308:	0a000051 	beq	b454 <udc_event_handle+0x26c>
    b30c:	e3190001 	tst	r9, #1	; 0x1
    b310:	0a000042 	beq	b420 <udc_event_handle+0x238>
    b314:	e5935908 	ldr	r5, [r3, #2312]
    b318:	e3150001 	tst	r5, #1	; 0x1
    b31c:	0a00003b 	beq	b410 <udc_event_handle+0x228>
    b320:	e59f3640 	ldr	r3, [pc, #1600]	; b968 <udc_event_handle+0x780>
    b324:	e5933020 	ldr	r3, [r3, #32]
    b328:	e2433002 	sub	r3, r3, #2	; 0x2
    b32c:	e3530003 	cmp	r3, #3	; 0x3
    b330:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    b334:	ea000035 	b	b410 <udc_event_handle+0x228>
    b338:	0000b378 	.word	0x0000b378
    b33c:	0000b348 	.word	0x0000b348
    b340:	0000b3ac 	.word	0x0000b3ac
    b344:	0000b3ac 	.word	0x0000b3ac
    b348:	e59f160c 	ldr	r1, [pc, #1548]	; b95c <udc_event_handle+0x774>
    b34c:	e59f0614 	ldr	r0, [pc, #1556]	; b968 <udc_event_handle+0x780>
    b350:	e5912910 	ldr	r2, [r1, #2320]
    b354:	e590c00c 	ldr	ip, [r0, #12]
    b358:	e3a03004 	mov	r3, #4	; 0x4
    b35c:	e3822702 	orr	r2, r2, #524288	; 0x80000
    b360:	e5803020 	str	r3, [r0, #32]
    b364:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    b368:	e5812910 	str	r2, [r1, #2320]
    b36c:	e581c914 	str	ip, [r1, #2324]
    b370:	e5813900 	str	r3, [r1, #2304]
    b374:	ea000025 	b	b410 <udc_event_handle+0x228>
    b378:	e59f15dc 	ldr	r1, [pc, #1500]	; b95c <udc_event_handle+0x774>
    b37c:	e59f05e4 	ldr	r0, [pc, #1508]	; b968 <udc_event_handle+0x780>
    b380:	e5913b10 	ldr	r3, [r1, #2832]
    b384:	e590c024 	ldr	ip, [r0, #36]
    b388:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    b38c:	e3833702 	orr	r3, r3, #524288	; 0x80000
    b390:	e5813b10 	str	r3, [r1, #2832]
    b394:	e3a02005 	mov	r2, #5	; 0x5
    b398:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    b39c:	e581cb14 	str	ip, [r1, #2836]
    b3a0:	e5802020 	str	r2, [r0, #32]
    b3a4:	e5813b00 	str	r3, [r1, #2816]
    b3a8:	ea000018 	b	b410 <udc_event_handle+0x228>
    b3ac:	e59f05b4 	ldr	r0, [pc, #1460]	; b968 <udc_event_handle+0x780>
    b3b0:	e5d03015 	ldrb	r3, [r0, #21]
    b3b4:	e3530009 	cmp	r3, #9	; 0x9
    b3b8:	1a00000a 	bne	b3e8 <udc_event_handle+0x200>
    b3bc:	e59f1598 	ldr	r1, [pc, #1432]	; b95c <udc_event_handle+0x774>
    b3c0:	e59f35ac 	ldr	r3, [pc, #1452]	; b974 <udc_event_handle+0x78c>
    b3c4:	e3a02001 	mov	r2, #1	; 0x1
    b3c8:	e5813920 	str	r3, [r1, #2336]
    b3cc:	e2433d07 	sub	r3, r3, #448	; 0x1c0
    b3d0:	e5813b40 	str	r3, [r1, #2880]
    b3d4:	e591381c 	ldr	r3, [r1, #2076]
    b3d8:	e5c02000 	strb	r2, [r0]
    b3dc:	e3833701 	orr	r3, r3, #262144	; 0x40000
    b3e0:	e3833002 	orr	r3, r3, #2	; 0x2
    b3e4:	e581381c 	str	r3, [r1, #2076]
    b3e8:	e59f256c 	ldr	r2, [pc, #1388]	; b95c <udc_event_handle+0x774>
    b3ec:	e59f1574 	ldr	r1, [pc, #1396]	; b968 <udc_event_handle+0x780>
    b3f0:	e59f3574 	ldr	r3, [pc, #1396]	; b96c <udc_event_handle+0x784>
    b3f4:	e5910024 	ldr	r0, [r1, #36]
    b3f8:	e5823b10 	str	r3, [r2, #2832]
    b3fc:	e3a03001 	mov	r3, #1	; 0x1
    b400:	e5813020 	str	r3, [r1, #32]
    b404:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    b408:	e5820b14 	str	r0, [r2, #2836]
    b40c:	e5823b00 	str	r3, [r2, #2816]
    b410:	e300320d 	movw	r3, #525	; 0x20d
    b414:	e59f2540 	ldr	r2, [pc, #1344]	; b95c <udc_event_handle+0x774>
    b418:	e0053003 	and	r3, r5, r3
    b41c:	e5823908 	str	r3, [r2, #2312]
    b420:	e3190002 	tst	r9, #2	; 0x2
    b424:	0a00000a 	beq	b454 <udc_event_handle+0x26c>
    b428:	e59f352c 	ldr	r3, [pc, #1324]	; b95c <udc_event_handle+0x774>
    b42c:	e5932928 	ldr	r2, [r3, #2344]
    b430:	e3120001 	tst	r2, #1	; 0x1
    b434:	0a000003 	beq	b448 <udc_event_handle+0x260>
    b438:	e5933930 	ldr	r3, [r3, #2352]
    b43c:	e3d3120f 	bics	r1, r3, #-268435456	; 0xf0000000
    b440:	059f3520 	ldreq	r3, [pc, #1312]	; b968 <udc_event_handle+0x780>
    b444:	05c31002 	strbeq	r1, [r3, #2]
    b448:	e59f350c 	ldr	r3, [pc, #1292]	; b95c <udc_event_handle+0x774>
    b44c:	e2022005 	and	r2, r2, #5	; 0x5
    b450:	e5832928 	str	r2, [r3, #2344]
    b454:	e3140702 	tst	r4, #524288	; 0x80000
    b458:	0a00013c 	beq	b950 <udc_event_handle+0x768>
    b45c:	e3190801 	tst	r9, #65536	; 0x10000
    b460:	0a000124 	beq	b8f8 <udc_event_handle+0x710>
    b464:	e59f34f0 	ldr	r3, [pc, #1264]	; b95c <udc_event_handle+0x774>
    b468:	e593ab08 	ldr	sl, [r3, #2824]
    b46c:	e31a0001 	tst	sl, #1	; 0x1
    b470:	0a00002b 	beq	b524 <udc_event_handle+0x33c>
    b474:	e59f34ec 	ldr	r3, [pc, #1260]	; b968 <udc_event_handle+0x780>
    b478:	e5933020 	ldr	r3, [r3, #32]
    b47c:	e2433002 	sub	r3, r3, #2	; 0x2
    b480:	e3530003 	cmp	r3, #3	; 0x3
    b484:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    b488:	ea000025 	b	b524 <udc_event_handle+0x33c>
    b48c:	0000b4c8 	.word	0x0000b4c8
    b490:	0000b49c 	.word	0x0000b49c
    b494:	0000b4fc 	.word	0x0000b4fc
    b498:	0000b4fc 	.word	0x0000b4fc
    b49c:	e59f14c4 	ldr	r1, [pc, #1220]	; b968 <udc_event_handle+0x780>
    b4a0:	e59f24b4 	ldr	r2, [pc, #1204]	; b95c <udc_event_handle+0x774>
    b4a4:	e3a03004 	mov	r3, #4	; 0x4
    b4a8:	e591000c 	ldr	r0, [r1, #12]
    b4ac:	e5813020 	str	r3, [r1, #32]
    b4b0:	e3a03702 	mov	r3, #524288	; 0x80000
    b4b4:	e5823910 	str	r3, [r2, #2320]
    b4b8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    b4bc:	e5820914 	str	r0, [r2, #2324]
    b4c0:	e5823900 	str	r3, [r2, #2304]
    b4c4:	ea000016 	b	b524 <udc_event_handle+0x33c>
    b4c8:	e59f148c 	ldr	r1, [pc, #1164]	; b95c <udc_event_handle+0x774>
    b4cc:	e59f0494 	ldr	r0, [pc, #1172]	; b968 <udc_event_handle+0x780>
    b4d0:	e5913b10 	ldr	r3, [r1, #2832]
    b4d4:	e590c00c 	ldr	ip, [r0, #12]
    b4d8:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    b4dc:	e3833702 	orr	r3, r3, #524288	; 0x80000
    b4e0:	e5813b10 	str	r3, [r1, #2832]
    b4e4:	e3a02005 	mov	r2, #5	; 0x5
    b4e8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    b4ec:	e581cb14 	str	ip, [r1, #2836]
    b4f0:	e5802020 	str	r2, [r0, #32]
    b4f4:	e5813b00 	str	r3, [r1, #2816]
    b4f8:	ea000009 	b	b524 <udc_event_handle+0x33c>
    b4fc:	e59f2458 	ldr	r2, [pc, #1112]	; b95c <udc_event_handle+0x774>
    b500:	e59f1460 	ldr	r1, [pc, #1120]	; b968 <udc_event_handle+0x780>
    b504:	e59f3460 	ldr	r3, [pc, #1120]	; b96c <udc_event_handle+0x784>
    b508:	e5910024 	ldr	r0, [r1, #36]
    b50c:	e5823b10 	str	r3, [r2, #2832]
    b510:	e3a03001 	mov	r3, #1	; 0x1
    b514:	e5813020 	str	r3, [r1, #32]
    b518:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    b51c:	e5820b14 	str	r0, [r2, #2836]
    b520:	e5823b00 	str	r3, [r2, #2816]
    b524:	e31a0004 	tst	sl, #4	; 0x4
    b528:	159f342c 	ldrne	r3, [pc, #1068]	; b95c <udc_event_handle+0x774>
    b52c:	13a02004 	movne	r2, #4	; 0x4
    b530:	15832b08 	strne	r2, [r3, #2824]
    b534:	e31a0008 	tst	sl, #8	; 0x8
    b538:	0a0000ea 	beq	b8e8 <udc_event_handle+0x700>
    b53c:	e31a0040 	tst	sl, #64	; 0x40
    b540:	e59f1430 	ldr	r1, [pc, #1072]	; b978 <udc_event_handle+0x790>
    b544:	0a000006 	beq	b564 <udc_event_handle+0x37c>
    b548:	e59f340c 	ldr	r3, [pc, #1036]	; b95c <udc_event_handle+0x774>
    b54c:	e5933b14 	ldr	r3, [r3, #2836]
    b550:	e2432004 	sub	r2, r3, #4	; 0x4
    b554:	e2433008 	sub	r3, r3, #8	; 0x8
    b558:	e5812004 	str	r2, [r1, #4]
    b55c:	e5813000 	str	r3, [r1]
    b560:	ea000005 	b	b57c <udc_event_handle+0x394>
    b564:	e59f33fc 	ldr	r3, [pc, #1020]	; b968 <udc_event_handle+0x780>
    b568:	e5932024 	ldr	r2, [r3, #36]
    b56c:	e5923000 	ldr	r3, [r2]
    b570:	e5813000 	str	r3, [r1]
    b574:	e5923004 	ldr	r3, [r2, #4]
    b578:	e5813004 	str	r3, [r1, #4]
    b57c:	e59f23e4 	ldr	r2, [pc, #996]	; b968 <udc_event_handle+0x780>
    b580:	e5d20014 	ldrb	r0, [r2, #20]
    b584:	e3100080 	tst	r0, #128	; 0x80
    b588:	13a03002 	movne	r3, #2	; 0x2
    b58c:	03a03003 	moveq	r3, #3	; 0x3
    b590:	e5823020 	str	r3, [r2, #32]
    b594:	e59f23cc 	ldr	r2, [pc, #972]	; b968 <udc_event_handle+0x780>
    b598:	e1d211ba 	ldrh	r1, [r2, #26]
    b59c:	e3510000 	cmp	r1, #0	; 0x0
    b5a0:	03a03004 	moveq	r3, #4	; 0x4
    b5a4:	05823020 	streq	r3, [r2, #32]
    b5a8:	e59f33b8 	ldr	r3, [pc, #952]	; b968 <udc_event_handle+0x780>
    b5ac:	e5d33015 	ldrb	r3, [r3, #21]
    b5b0:	e3530009 	cmp	r3, #9	; 0x9
    b5b4:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    b5b8:	ea0000ca 	b	b8e8 <udc_event_handle+0x700>
    b5bc:	0000b880 	.word	0x0000b880
    b5c0:	0000b8e8 	.word	0x0000b8e8
    b5c4:	0000b8e8 	.word	0x0000b8e8
    b5c8:	0000b8e8 	.word	0x0000b8e8
    b5cc:	0000b8e8 	.word	0x0000b8e8
    b5d0:	0000b7cc 	.word	0x0000b7cc
    b5d4:	0000b5e4 	.word	0x0000b5e4
    b5d8:	0000b8e8 	.word	0x0000b8e8
    b5dc:	0000b8e8 	.word	0x0000b8e8
    b5e0:	0000b81c 	.word	0x0000b81c
    b5e4:	e3a00040 	mov	r0, #64	; 0x40
    b5e8:	ebffd7df 	bl	156c <malloc>
    b5ec:	e2506000 	subs	r6, r0, #0	; 0x0
    b5f0:	0a0000bc 	beq	b8e8 <udc_event_handle+0x700>
    b5f4:	e59f536c 	ldr	r5, [pc, #876]	; b968 <udc_event_handle+0x780>
    b5f8:	e1d521b6 	ldrh	r2, [r5, #22]
    b5fc:	e1a03422 	lsr	r3, r2, #8
    b600:	e2433001 	sub	r3, r3, #1	; 0x1
    b604:	e3530005 	cmp	r3, #5	; 0x5
    b608:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    b60c:	ea000054 	b	b764 <udc_event_handle+0x57c>
    b610:	0000b628 	.word	0x0000b628
    b614:	0000b640 	.word	0x0000b640
    b618:	0000b698 	.word	0x0000b698
    b61c:	0000b764 	.word	0x0000b764
    b620:	0000b764 	.word	0x0000b764
    b624:	0000b66c 	.word	0x0000b66c
    b628:	e1a00006 	mov	r0, r6
    b62c:	e59f1348 	ldr	r1, [pc, #840]	; b97c <udc_event_handle+0x794>
    b630:	e3a02012 	mov	r2, #18	; 0x12
    b634:	ebffd767 	bl	13d8 <memcpy>
    b638:	e3a03012 	mov	r3, #18	; 0x12
    b63c:	ea000028 	b	b6e4 <udc_event_handle+0x4fc>
    b640:	e59f3338 	ldr	r3, [pc, #824]	; b980 <udc_event_handle+0x798>
    b644:	e3a04020 	mov	r4, #32	; 0x20
    b648:	e3a0c000 	mov	ip, #0	; 0x0
    b64c:	e1a01003 	mov	r1, r3
    b650:	e1a00006 	mov	r0, r6
    b654:	e1a02004 	mov	r2, r4
    b658:	e5c3c003 	strb	ip, [r3, #3]
    b65c:	e5c34002 	strb	r4, [r3, #2]
    b660:	ebffd75c 	bl	13d8 <memcpy>
    b664:	e5854004 	str	r4, [r5, #4]
    b668:	ea000045 	b	b784 <udc_event_handle+0x59c>
    b66c:	e1d531ba 	ldrh	r3, [r5, #26]
    b670:	e59fc30c 	ldr	ip, [pc, #780]	; b984 <udc_event_handle+0x79c>
    b674:	e353000a 	cmp	r3, #10	; 0xa
    b678:	23a0300a 	movcs	r3, #10	; 0xa
    b67c:	e5cc3000 	strb	r3, [ip]
    b680:	e1a0100c 	mov	r1, ip
    b684:	e1a00006 	mov	r0, r6
    b688:	e3a0200a 	mov	r2, #10	; 0xa
    b68c:	ebffd751 	bl	13d8 <memcpy>
    b690:	e3a0300a 	mov	r3, #10	; 0xa
    b694:	ea000012 	b	b6e4 <udc_event_handle+0x4fc>
    b698:	e21270ff 	ands	r7, r2, #255	; 0xff
    b69c:	13a03000 	movne	r3, #0	; 0x0
    b6a0:	03a03001 	moveq	r3, #1	; 0x1
    b6a4:	e3570005 	cmp	r7, #5	; 0x5
    b6a8:	d1a08003 	movle	r8, r3
    b6ac:	c3838001 	orrgt	r8, r3, #1	; 0x1
    b6b0:	e3580000 	cmp	r8, #0	; 0x0
    b6b4:	e59f52ac 	ldr	r5, [pc, #684]	; b968 <udc_event_handle+0x780>
    b6b8:	0a00000b 	beq	b6ec <udc_event_handle+0x504>
    b6bc:	e1d531ba 	ldrh	r3, [r5, #26]
    b6c0:	e59fc2c0 	ldr	ip, [pc, #704]	; b988 <udc_event_handle+0x7a0>
    b6c4:	e3530004 	cmp	r3, #4	; 0x4
    b6c8:	23a03004 	movcs	r3, #4	; 0x4
    b6cc:	e5cc3000 	strb	r3, [ip]
    b6d0:	e1a0100c 	mov	r1, ip
    b6d4:	e1a00006 	mov	r0, r6
    b6d8:	e3a02004 	mov	r2, #4	; 0x4
    b6dc:	ebffd73d 	bl	13d8 <memcpy>
    b6e0:	e3a03004 	mov	r3, #4	; 0x4
    b6e4:	e5853004 	str	r3, [r5, #4]
    b6e8:	ea000025 	b	b784 <udc_event_handle+0x59c>
    b6ec:	e59f4298 	ldr	r4, [pc, #664]	; b98c <udc_event_handle+0x7a4>
    b6f0:	e7940107 	ldr	r0, [r4, r7, lsl #2]
    b6f4:	ebffd71d 	bl	1370 <strlen>
    b6f8:	e6ef0070 	uxtb	r0, r0
    b6fc:	e2803001 	add	r3, r0, #1	; 0x1
    b700:	e5d5201a 	ldrb	r2, [r5, #26]
    b704:	e1a03083 	lsl	r3, r3, #1
    b708:	e6ef3073 	uxtb	r3, r3
    b70c:	e1530002 	cmp	r3, r2
    b710:	31a0e003 	movcc	lr, r3
    b714:	21a0e002 	movcs	lr, r2
    b718:	e3a03003 	mov	r3, #3	; 0x3
    b71c:	e5c63001 	strb	r3, [r6, #1]
    b720:	e5c6e000 	strb	lr, [r6]
    b724:	e1a05008 	mov	r5, r8
    b728:	e3a02001 	mov	r2, #1	; 0x1
    b72c:	ea000005 	b	b748 <udc_event_handle+0x560>
    b730:	e7943107 	ldr	r3, [r4, r7, lsl #2]
    b734:	e0833002 	add	r3, r3, r2
    b738:	e5533001 	ldrb	r3, [r3, #-1]
    b73c:	e7c63082 	strb	r3, [r6, r2, lsl #1]
    b740:	e5c15001 	strb	r5, [r1, #1]
    b744:	e6ef207c 	uxtb	r2, ip
    b748:	e1500002 	cmp	r0, r2
    b74c:	e282c001 	add	ip, r2, #1	; 0x1
    b750:	e0861082 	add	r1, r6, r2, lsl #1
    b754:	aafffff5 	bge	b730 <udc_event_handle+0x548>
    b758:	e59f3208 	ldr	r3, [pc, #520]	; b968 <udc_event_handle+0x780>
    b75c:	e583e004 	str	lr, [r3, #4]
    b760:	ea000007 	b	b784 <udc_event_handle+0x59c>
    b764:	e59f21f0 	ldr	r2, [pc, #496]	; b95c <udc_event_handle+0x774>
    b768:	e3a03702 	mov	r3, #524288	; 0x80000
    b76c:	e5951024 	ldr	r1, [r5, #36]
    b770:	e5823910 	str	r3, [r2, #2320]
    b774:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    b778:	e5821914 	str	r1, [r2, #2324]
    b77c:	e5823900 	str	r3, [r2, #2304]
    b780:	ea00000e 	b	b7c0 <udc_event_handle+0x5d8>
    b784:	e59f11dc 	ldr	r1, [pc, #476]	; b968 <udc_event_handle+0x780>
    b788:	e59f01cc 	ldr	r0, [pc, #460]	; b95c <udc_event_handle+0x774>
    b78c:	e1d120b4 	ldrh	r2, [r1, #4]
    b790:	e1d131ba 	ldrh	r3, [r1, #26]
    b794:	e1520003 	cmp	r2, r3
    b798:	21a02003 	movcs	r2, r3
    b79c:	e3823702 	orr	r3, r2, #524288	; 0x80000
    b7a0:	e5803910 	str	r3, [r0, #2320]
    b7a4:	e3a03000 	mov	r3, #0	; 0x0
    b7a8:	e5c13001 	strb	r3, [r1, #1]
    b7ac:	e2833321 	add	r3, r3, #-2080374784	; 0x84000000
    b7b0:	e5812004 	str	r2, [r1, #4]
    b7b4:	e5806914 	str	r6, [r0, #2324]
    b7b8:	e5803900 	str	r3, [r0, #2304]
    b7bc:	e581600c 	str	r6, [r1, #12]
    b7c0:	e1a00006 	mov	r0, r6
    b7c4:	ebffd7d0 	bl	170c <free>
    b7c8:	ea000046 	b	b8e8 <udc_event_handle+0x700>
    b7cc:	e59fc1bc 	ldr	ip, [pc, #444]	; b990 <udc_event_handle+0x7a8>
    b7d0:	e5dc3000 	ldrb	r3, [ip]
    b7d4:	e35300ff 	cmp	r3, #255	; 0xff
    b7d8:	1a000042 	bne	b8e8 <udc_event_handle+0x700>
    b7dc:	e59f1178 	ldr	r1, [pc, #376]	; b95c <udc_event_handle+0x774>
    b7e0:	e59f0180 	ldr	r0, [pc, #384]	; b968 <udc_event_handle+0x780>
    b7e4:	e5913800 	ldr	r3, [r1, #2048]
    b7e8:	e1d021b6 	ldrh	r2, [r0, #22]
    b7ec:	e3c33e7f 	bic	r3, r3, #2032	; 0x7f0
    b7f0:	e202207f 	and	r2, r2, #127	; 0x7f
    b7f4:	e1833202 	orr	r3, r3, r2, lsl #4
    b7f8:	e5813800 	str	r3, [r1, #2048]
    b7fc:	e5900024 	ldr	r0, [r0, #36]
    b800:	e3a03702 	mov	r3, #524288	; 0x80000
    b804:	e5cc2000 	strb	r2, [ip]
    b808:	e5813910 	str	r3, [r1, #2320]
    b80c:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    b810:	e5810914 	str	r0, [r1, #2324]
    b814:	e5813900 	str	r3, [r1, #2304]
    b818:	ea000032 	b	b8e8 <udc_event_handle+0x700>
    b81c:	e59f016c 	ldr	r0, [pc, #364]	; b990 <udc_event_handle+0x7a8>
    b820:	e5d03000 	ldrb	r3, [r0]
    b824:	e35300ff 	cmp	r3, #255	; 0xff
    b828:	0a00002e 	beq	b8e8 <udc_event_handle+0x700>
    b82c:	e59f1134 	ldr	r1, [pc, #308]	; b968 <udc_event_handle+0x780>
    b830:	e1d131b6 	ldrh	r3, [r1, #22]
    b834:	e3530000 	cmp	r3, #0	; 0x0
    b838:	03e03000 	mvneq	r3, #0	; 0x0
    b83c:	05911024 	ldreq	r1, [r1, #36]
    b840:	059f2114 	ldreq	r2, [pc, #276]	; b95c <udc_event_handle+0x774>
    b844:	05c03000 	strbeq	r3, [r0]
    b848:	0a000013 	beq	b89c <udc_event_handle+0x6b4>
    b84c:	e3530001 	cmp	r3, #1	; 0x1
    b850:	1a000024 	bne	b8e8 <udc_event_handle+0x700>
    b854:	e59f2100 	ldr	r2, [pc, #256]	; b95c <udc_event_handle+0x774>
    b858:	e59f3134 	ldr	r3, [pc, #308]	; b994 <udc_event_handle+0x7ac>
    b85c:	e5911024 	ldr	r1, [r1, #36]
    b860:	e5823920 	str	r3, [r2, #2336]
    b864:	e2833d07 	add	r3, r3, #448	; 0x1c0
    b868:	e5823b40 	str	r3, [r2, #2880]
    b86c:	e592381c 	ldr	r3, [r2, #2076]
    b870:	e3833701 	orr	r3, r3, #262144	; 0x40000
    b874:	e3833002 	orr	r3, r3, #2	; 0x2
    b878:	e582381c 	str	r3, [r2, #2076]
    b87c:	ea000006 	b	b89c <udc_event_handle+0x6b4>
    b880:	e210c01f 	ands	ip, r0, #31	; 0x1f
    b884:	1a000017 	bne	b8e8 <udc_event_handle+0x700>
    b888:	e3510000 	cmp	r1, #0	; 0x0
    b88c:	e59f50d4 	ldr	r5, [pc, #212]	; b968 <udc_event_handle+0x780>
    b890:	1a000004 	bne	b8a8 <udc_event_handle+0x6c0>
    b894:	e5951024 	ldr	r1, [r5, #36]
    b898:	e59f20bc 	ldr	r2, [pc, #188]	; b95c <udc_event_handle+0x774>
    b89c:	e3a03702 	mov	r3, #524288	; 0x80000
    b8a0:	e5823910 	str	r3, [r2, #2320]
    b8a4:	ea00000c 	b	b8dc <udc_event_handle+0x6f4>
    b8a8:	e1d531b6 	ldrh	r3, [r5, #22]
    b8ac:	e59f20a8 	ldr	r2, [pc, #168]	; b95c <udc_event_handle+0x774>
    b8b0:	e3530002 	cmp	r3, #2	; 0x2
    b8b4:	23a03002 	movcs	r3, #2	; 0x2
    b8b8:	e6ef3073 	uxtb	r3, r3
    b8bc:	e1530001 	cmp	r3, r1
    b8c0:	21a03001 	movcs	r3, r1
    b8c4:	e3830702 	orr	r0, r3, #524288	; 0x80000
    b8c8:	e28d1006 	add	r1, sp, #6	; 0x6
    b8cc:	e5c5c001 	strb	ip, [r5, #1]
    b8d0:	e5853004 	str	r3, [r5, #4]
    b8d4:	e585100c 	str	r1, [r5, #12]
    b8d8:	e5820910 	str	r0, [r2, #2320]
    b8dc:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    b8e0:	e5821914 	str	r1, [r2, #2324]
    b8e4:	e5823900 	str	r3, [r2, #2304]
    b8e8:	e3003209 	movw	r3, #521	; 0x209
    b8ec:	e59f2068 	ldr	r2, [pc, #104]	; b95c <udc_event_handle+0x774>
    b8f0:	e00a3003 	and	r3, sl, r3
    b8f4:	e5823b08 	str	r3, [r2, #2824]
    b8f8:	e3190701 	tst	r9, #262144	; 0x40000
    b8fc:	0a000013 	beq	b950 <udc_event_handle+0x768>
    b900:	e59f1054 	ldr	r1, [pc, #84]	; b95c <udc_event_handle+0x774>
    b904:	e5912b48 	ldr	r2, [r1, #2888]
    b908:	e3120001 	tst	r2, #1	; 0x1
    b90c:	0a00000c 	beq	b944 <udc_event_handle+0x75c>
    b910:	e5913b50 	ldr	r3, [r1, #2896]
    b914:	e59f504c 	ldr	r5, [pc, #76]	; b968 <udc_event_handle+0x780>
    b918:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
    b91c:	e3c3373e 	bic	r3, r3, #16252928	; 0xf80000
    b920:	e3530c02 	cmp	r3, #512	; 0x200
    b924:	05953028 	ldreq	r3, [r5, #40]
    b928:	13a03000 	movne	r3, #0	; 0x0
    b92c:	03833702 	orreq	r3, r3, #524288	; 0x80000
    b930:	05813b50 	streq	r3, [r1, #2896]
    b934:	05913b40 	ldreq	r3, [r1, #2880]
    b938:	15c53003 	strbne	r3, [r5, #3]
    b93c:	03833321 	orreq	r3, r3, #-2080374784	; 0x84000000
    b940:	05813b40 	streq	r3, [r1, #2880]
    b944:	e59f3010 	ldr	r3, [pc, #16]	; b95c <udc_event_handle+0x774>
    b948:	e2022005 	and	r2, r2, #5	; 0x5
    b94c:	e5832b48 	str	r2, [r3, #2888]
    b950:	e28dd008 	add	sp, sp, #8	; 0x8
    b954:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    b958:	0800d270 	.word	0x0800d270
    b95c:	27100000 	.word	0x27100000
    b960:	00010001 	.word	0x00010001
    b964:	0c001780 	.word	0x0c001780
    b968:	0800d274 	.word	0x0800d274
    b96c:	60080040 	.word	0x60080040
    b970:	0800d26c 	.word	0x0800d26c
    b974:	18088200 	.word	0x18088200
    b978:	0800d288 	.word	0x0800d288
    b97c:	000107fe 	.word	0x000107fe
    b980:	0800d111 	.word	0x0800d111
    b984:	0800d131 	.word	0x0800d131
    b988:	0800d0e8 	.word	0x0800d0e8
    b98c:	0800d770 	.word	0x0800d770
    b990:	0800d110 	.word	0x0800d110
    b994:	18088040 	.word	0x18088040

0000b998 <udc_write>:
    b998:	e59f30a4 	ldr	r3, [pc, #164]	; ba44 <udc_write+0xac>
    b99c:	e3a02001 	mov	r2, #1	; 0x1
    b9a0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    b9a4:	e583000c 	str	r0, [r3, #12]
    b9a8:	e1a05001 	mov	r5, r1
    b9ac:	e5c32001 	strb	r2, [r3, #1]
    b9b0:	e1a04003 	mov	r4, r3
    b9b4:	e1a08002 	mov	r8, r2
    b9b8:	e59f6088 	ldr	r6, [pc, #136]	; ba48 <udc_write+0xb0>
    b9bc:	e59f7088 	ldr	r7, [pc, #136]	; ba4c <udc_write+0xb4>
    b9c0:	ea00001b 	b	ba34 <udc_write+0x9c>
    b9c4:	e3550701 	cmp	r5, #262144	; 0x40000
    b9c8:	b1a02005 	movlt	r2, r5
    b9cc:	a3a02701 	movge	r2, #262144	; 0x40000
    b9d0:	e292303f 	adds	r3, r2, #63	; 0x3f
    b9d4:	4282307e 	addmi	r3, r2, #126	; 0x7e
    b9d8:	e594100c 	ldr	r1, [r4, #12]
    b9dc:	e1a03343 	asr	r3, r3, #6
    b9e0:	e1823983 	orr	r3, r2, r3, lsl #19
    b9e4:	e5863930 	str	r3, [r6, #2352]
    b9e8:	e5861934 	str	r1, [r6, #2356]
    b9ec:	e5963920 	ldr	r3, [r6, #2336]
    b9f0:	e5842004 	str	r2, [r4, #4]
    b9f4:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    b9f8:	e5863920 	str	r3, [r6, #2336]
    b9fc:	e5c48002 	strb	r8, [r4, #2]
    ba00:	ea000000 	b	ba08 <udc_write+0x70>
    ba04:	ebfffdf7 	bl	b1e8 <udc_event_handle>
    ba08:	e5d43002 	ldrb	r3, [r4, #2]
    ba0c:	e3530000 	cmp	r3, #0	; 0x0
    ba10:	0a000002 	beq	ba20 <udc_write+0x88>
    ba14:	e5973000 	ldr	r3, [r7]
    ba18:	e3530000 	cmp	r3, #0	; 0x0
    ba1c:	1afffff8 	bne	ba04 <udc_write+0x6c>
    ba20:	e5943004 	ldr	r3, [r4, #4]
    ba24:	e594200c 	ldr	r2, [r4, #12]
    ba28:	e0635005 	rsb	r5, r3, r5
    ba2c:	e0833002 	add	r3, r3, r2
    ba30:	e584300c 	str	r3, [r4, #12]
    ba34:	e3550000 	cmp	r5, #0	; 0x0
    ba38:	1affffe1 	bne	b9c4 <udc_write+0x2c>
    ba3c:	e1a00005 	mov	r0, r5
    ba40:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    ba44:	0800d274 	.word	0x0800d274
    ba48:	27100000 	.word	0x27100000
    ba4c:	0800d10c 	.word	0x0800d10c

0000ba50 <udc_read>:
    ba50:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    ba54:	e59f30b4 	ldr	r3, [pc, #180]	; bb10 <udc_read+0xc0>
    ba58:	e24dd004 	sub	sp, sp, #4	; 0x4
    ba5c:	e1a04001 	mov	r4, r1
    ba60:	e1a05003 	mov	r5, r3
    ba64:	e5830010 	str	r0, [r3, #16]
    ba68:	e3a0a001 	mov	sl, #1	; 0x1
    ba6c:	e59f60a0 	ldr	r6, [pc, #160]	; bb14 <udc_read+0xc4>
    ba70:	e59f80a0 	ldr	r8, [pc, #160]	; bb18 <udc_read+0xc8>
    ba74:	e1a07003 	mov	r7, r3
    ba78:	ea00001f 	b	bafc <udc_read+0xac>
    ba7c:	e3540701 	cmp	r4, #262144	; 0x40000
    ba80:	b1a02004 	movlt	r2, r4
    ba84:	a3a02701 	movge	r2, #262144	; 0x40000
    ba88:	e2823f7f 	add	r3, r2, #508	; 0x1fc
    ba8c:	e2833003 	add	r3, r3, #3	; 0x3
    ba90:	e5951010 	ldr	r1, [r5, #16]
    ba94:	e1a034c3 	asr	r3, r3, #9
    ba98:	e1823983 	orr	r3, r2, r3, lsl #19
    ba9c:	e5863b50 	str	r3, [r6, #2896]
    baa0:	e5861b54 	str	r1, [r6, #2900]
    baa4:	e5963b40 	ldr	r3, [r6, #2880]
    baa8:	e5852008 	str	r2, [r5, #8]
    baac:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    bab0:	e5863b40 	str	r3, [r6, #2880]
    bab4:	e5c5a003 	strb	sl, [r5, #3]
    bab8:	ea000002 	b	bac8 <udc_read+0x78>
    babc:	ebfffdc9 	bl	b1e8 <udc_event_handle>
    bac0:	e3a00001 	mov	r0, #1	; 0x1
    bac4:	ebfff3d3 	bl	8a18 <udelay>
    bac8:	e5d53003 	ldrb	r3, [r5, #3]
    bacc:	e3530000 	cmp	r3, #0	; 0x0
    bad0:	0a000004 	beq	bae8 <udc_read+0x98>
    bad4:	e5983000 	ldr	r3, [r8]
    bad8:	e3530000 	cmp	r3, #0	; 0x0
    badc:	1afffff6 	bne	babc <udc_read+0x6c>
    bae0:	e3e04000 	mvn	r4, #0	; 0x0
    bae4:	ea000006 	b	bb04 <udc_read+0xb4>
    bae8:	e5973008 	ldr	r3, [r7, #8]
    baec:	e5972010 	ldr	r2, [r7, #16]
    baf0:	e0634004 	rsb	r4, r3, r4
    baf4:	e0833002 	add	r3, r3, r2
    baf8:	e5873010 	str	r3, [r7, #16]
    bafc:	e3540000 	cmp	r4, #0	; 0x0
    bb00:	1affffdd 	bne	ba7c <udc_read+0x2c>
    bb04:	e1a00004 	mov	r0, r4
    bb08:	e28dd004 	add	sp, sp, #4	; 0x4
    bb0c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    bb10:	0800d274 	.word	0x0800d274
    bb14:	27100000 	.word	0x27100000
    bb18:	0800d10c 	.word	0x0800d10c

0000bb1c <udc_connect>:
    bb1c:	e92d4070 	push	{r4, r5, r6, lr}
    bb20:	e59f41e4 	ldr	r4, [pc, #484]	; bd0c <udc_connect+0x1f0>
    bb24:	e3a02000 	mov	r2, #0	; 0x0
    bb28:	e5943024 	ldr	r3, [r4, #36]
    bb2c:	e3e01000 	mvn	r1, #0	; 0x0
    bb30:	e1530002 	cmp	r3, r2
    bb34:	e59f31d4 	ldr	r3, [pc, #468]	; bd10 <udc_connect+0x1f4>
    bb38:	e5c42003 	strb	r2, [r4, #3]
    bb3c:	e5c31000 	strb	r1, [r3]
    bb40:	e5c42000 	strb	r2, [r4]
    bb44:	e5c42002 	strb	r2, [r4, #2]
    bb48:	1a000002 	bne	bb58 <udc_connect+0x3c>
    bb4c:	e3a00028 	mov	r0, #40	; 0x28
    bb50:	ebffd685 	bl	156c <malloc>
    bb54:	e5840024 	str	r0, [r4, #36]
    bb58:	e59f31ac 	ldr	r3, [pc, #428]	; bd0c <udc_connect+0x1f0>
    bb5c:	e3a06001 	mov	r6, #1	; 0x1
    bb60:	e59f51ac 	ldr	r5, [pc, #428]	; bd14 <udc_connect+0x1f8>
    bb64:	e59f01ac 	ldr	r0, [pc, #428]	; bd18 <udc_connect+0x1fc>
    bb68:	e5836020 	str	r6, [r3, #32]
    bb6c:	ebffd2f2 	bl	73c <module_enable>
    bb70:	e3a02004 	mov	r2, #4	; 0x4
    bb74:	e3a03003 	mov	r3, #3	; 0x3
    bb78:	e3a01000 	mov	r1, #0	; 0x0
    bb7c:	e585243c 	str	r2, [r5, #1084]
    bb80:	e5853440 	str	r3, [r5, #1088]
    bb84:	e5852444 	str	r2, [r5, #1092]
    bb88:	e5853448 	str	r3, [r5, #1096]
    bb8c:	e585344c 	str	r3, [r5, #1100]
    bb90:	e5853450 	str	r3, [r5, #1104]
    bb94:	e5856454 	str	r6, [r5, #1108]
    bb98:	e5856458 	str	r6, [r5, #1112]
    bb9c:	e585145c 	str	r1, [r5, #1116]
    bba0:	e5851460 	str	r1, [r5, #1120]
    bba4:	e5953420 	ldr	r3, [r5, #1056]
    bba8:	e1a00006 	mov	r0, r6
    bbac:	e3c33007 	bic	r3, r3, #7	; 0x7
    bbb0:	e3833005 	orr	r3, r3, #5	; 0x5
    bbb4:	e5853420 	str	r3, [r5, #1056]
    bbb8:	e5953424 	ldr	r3, [r5, #1060]
    bbbc:	e3833003 	orr	r3, r3, #3	; 0x3
    bbc0:	e5853424 	str	r3, [r5, #1060]
    bbc4:	e595342c 	ldr	r3, [r5, #1068]
    bbc8:	e1833006 	orr	r3, r3, r6
    bbcc:	e585342c 	str	r3, [r5, #1068]
    bbd0:	e5954400 	ldr	r4, [r5, #1024]
    bbd4:	e384400f 	orr	r4, r4, #15	; 0xf
    bbd8:	e5854400 	str	r4, [r5, #1024]
    bbdc:	ebfff38d 	bl	8a18 <udelay>
    bbe0:	e3c4300e 	bic	r3, r4, #14	; 0xe
    bbe4:	e5853400 	str	r3, [r5, #1024]
    bbe8:	e1a00006 	mov	r0, r6
    bbec:	e3c4400f 	bic	r4, r4, #15	; 0xf
    bbf0:	ebfff388 	bl	8a18 <udelay>
    bbf4:	e5854400 	str	r4, [r5, #1024]
    bbf8:	e1a00006 	mov	r0, r6
    bbfc:	ebfff385 	bl	8a18 <udelay>
    bc00:	e59f2114 	ldr	r2, [pc, #276]	; bd1c <udc_connect+0x200>
    bc04:	e59f3114 	ldr	r3, [pc, #276]	; bd20 <udc_connect+0x204>
    bc08:	e1a05002 	mov	r5, r2
    bc0c:	e582300c 	str	r3, [r2, #12]
    bc10:	e5954010 	ldr	r4, [r5, #16]
    bc14:	e3a00028 	mov	r0, #40	; 0x28
    bc18:	ebfff37e 	bl	8a18 <udelay>
    bc1c:	e3540000 	cmp	r4, #0	; 0x0
    bc20:	e59f40f4 	ldr	r4, [pc, #244]	; bd1c <udc_connect+0x200>
    bc24:	aafffff9 	bge	bc10 <udc_connect+0xf4>
    bc28:	e5943010 	ldr	r3, [r4, #16]
    bc2c:	e3a0000a 	mov	r0, #10	; 0xa
    bc30:	e3833001 	orr	r3, r3, #1	; 0x1
    bc34:	e5843010 	str	r3, [r4, #16]
    bc38:	ebfff376 	bl	8a18 <udelay>
    bc3c:	e1a05004 	mov	r5, r4
    bc40:	e5954010 	ldr	r4, [r5, #16]
    bc44:	e3a0000a 	mov	r0, #10	; 0xa
    bc48:	ebfff372 	bl	8a18 <udelay>
    bc4c:	e3140001 	tst	r4, #1	; 0x1
    bc50:	e59fc0c4 	ldr	ip, [pc, #196]	; bd1c <udc_connect+0x200>
    bc54:	1afffff9 	bne	bc40 <udc_connect+0x124>
    bc58:	e3a03027 	mov	r3, #39	; 0x27
    bc5c:	e58c3008 	str	r3, [ip, #8]
    bc60:	e59c303c 	ldr	r3, [ip, #60]
    bc64:	e59c3014 	ldr	r3, [ip, #20]
    bc68:	e2130001 	ands	r0, r3, #1	; 0x1
    bc6c:	1a000024 	bne	bd04 <udc_connect+0x1e8>
    bc70:	e3a03301 	mov	r3, #67108864	; 0x4000000
    bc74:	e58c3800 	str	r3, [ip, #2048]
    bc78:	e59f30a4 	ldr	r3, [pc, #164]	; bd24 <udc_connect+0x208>
    bc7c:	e59f20a4 	ldr	r2, [pc, #164]	; bd28 <udc_connect+0x20c>
    bc80:	e58c3018 	str	r3, [ip, #24]
    bc84:	e59f30a0 	ldr	r3, [pc, #160]	; bd2c <udc_connect+0x210>
    bc88:	e59f10a0 	ldr	r1, [pc, #160]	; bd30 <udc_connect+0x214>
    bc8c:	e5823014 	str	r3, [r2, #20]
    bc90:	e59f309c 	ldr	r3, [pc, #156]	; bd34 <udc_connect+0x218>
    bc94:	e5821010 	str	r1, [r2, #16]
    bc98:	e5823004 	str	r3, [r2, #4]
    bc9c:	e59f3094 	ldr	r3, [pc, #148]	; bd38 <udc_connect+0x21c>
    bca0:	e582100c 	str	r1, [r2, #12]
    bca4:	e5823008 	str	r3, [r2, #8]
    bca8:	ebfff366 	bl	8a48 <get_timer>
    bcac:	e59f5058 	ldr	r5, [pc, #88]	; bd0c <udc_connect+0x1f0>
    bcb0:	e1a04000 	mov	r4, r0
    bcb4:	ebfffd4b 	bl	b1e8 <udc_event_handle>
    bcb8:	e5d53000 	ldrb	r3, [r5]
    bcbc:	e1a00004 	mov	r0, r4
    bcc0:	e3530000 	cmp	r3, #0	; 0x0
    bcc4:	1a000003 	bne	bcd8 <udc_connect+0x1bc>
    bcc8:	ebfff35e 	bl	8a48 <get_timer>
    bccc:	e3500ffa 	cmp	r0, #1000	; 0x3e8
    bcd0:	9afffff7 	bls	bcb4 <udc_connect+0x198>
    bcd4:	ea00000a 	b	bd04 <udc_connect+0x1e8>
    bcd8:	e59f203c 	ldr	r2, [pc, #60]	; bd1c <udc_connect+0x200>
    bcdc:	e59f3058 	ldr	r3, [pc, #88]	; bd3c <udc_connect+0x220>
    bce0:	e3a00000 	mov	r0, #0	; 0x0
    bce4:	e5823920 	str	r3, [r2, #2336]
    bce8:	e2833d07 	add	r3, r3, #448	; 0x1c0
    bcec:	e5823b40 	str	r3, [r2, #2880]
    bcf0:	e592381c 	ldr	r3, [r2, #2076]
    bcf4:	e3833701 	orr	r3, r3, #262144	; 0x40000
    bcf8:	e3833002 	orr	r3, r3, #2	; 0x2
    bcfc:	e582381c 	str	r3, [r2, #2076]
    bd00:	e8bd8070 	pop	{r4, r5, r6, pc}
    bd04:	e3e00000 	mvn	r0, #0	; 0x0
    bd08:	e8bd8070 	pop	{r4, r5, r6, pc}
    bd0c:	0800d274 	.word	0x0800d274
    bd10:	0800d110 	.word	0x0800d110
    bd14:	2d038000 	.word	0x2d038000
    bd18:	2d038400 	.word	0x2d038400
    bd1c:	27100000 	.word	0x27100000
    bd20:	4c001408 	.word	0x4c001408
    bd24:	000c3c00 	.word	0x000c3c00
    bd28:	0800d770 	.word	0x0800d770
    bd2c:	000123d7 	.word	0x000123d7
    bd30:	000123ec 	.word	0x000123ec
    bd34:	000123e0 	.word	0x000123e0
    bd38:	0800d0ec 	.word	0x0800d0ec
    bd3c:	18088040 	.word	0x18088040

0000bd40 <linkpc_loop>:
    bd40:	e92d4010 	push	{r4, lr}
    bd44:	e59f001c 	ldr	r0, [pc, #28]	; bd68 <linkpc_loop+0x28>
    bd48:	e59f401c 	ldr	r4, [pc, #28]	; bd6c <linkpc_loop+0x2c>
    bd4c:	e1a0e00f 	mov	lr, pc
    bd50:	e594f000 	ldr	pc, [r4]
    bd54:	e59f0014 	ldr	r0, [pc, #20]	; bd70 <linkpc_loop+0x30>
    bd58:	e1a0e00f 	mov	lr, pc
    bd5c:	e594f000 	ldr	pc, [r4]
    bd60:	e3a00000 	mov	r0, #0	; 0x0
    bd64:	e8bd8010 	pop	{r4, pc}
    bd68:	000123fd 	.word	0x000123fd
    bd6c:	0800d704 	.word	0x0800d704
    bd70:	0001240c 	.word	0x0001240c

0000bd74 <linkpc_set_stage>:
    bd74:	e1a01000 	mov	r1, r0
    bd78:	e59f0004 	ldr	r0, [pc, #4]	; bd84 <linkpc_set_stage+0x10>
    bd7c:	e3a02010 	mov	r2, #16	; 0x10
    bd80:	eaffd52a 	b	1230 <strncpy>
    bd84:	0800d13c 	.word	0x0800d13c

0000bd88 <print_size>:
    bd88:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    bd8c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    bd90:	e1a05000 	mov	r5, r0
    bd94:	e1a08001 	mov	r8, r1
    bd98:	83a04101 	movhi	r4, #1073741824	; 0x40000000
    bd9c:	83a07047 	movhi	r7, #71	; 0x47
    bda0:	8a000005 	bhi	bdbc <print_size+0x34>
    bda4:	e59f30a8 	ldr	r3, [pc, #168]	; be54 <print_size+0xcc>
    bda8:	e1500003 	cmp	r0, r3
    bdac:	83a0704d 	movhi	r7, #77	; 0x4d
    bdb0:	93a0706b 	movls	r7, #107	; 0x6b
    bdb4:	83a04601 	movhi	r4, #1048576	; 0x100000
    bdb8:	93a04b01 	movls	r4, #1024	; 0x400
    bdbc:	e1a01004 	mov	r1, r4
    bdc0:	e1a00005 	mov	r0, r5
    bdc4:	eb000cc3 	bl	f0d8 <__aeabi_uidiv>
    bdc8:	e1a01004 	mov	r1, r4
    bdcc:	e1a06000 	mov	r6, r0
    bdd0:	e1a00005 	mov	r0, r5
    bdd4:	eb000d3b 	bl	f2c8 <__aeabi_uidivmod>
    bdd8:	e2510000 	subs	r0, r1, #0	; 0x0
    bddc:	01a04000 	moveq	r4, r0
    bde0:	0a000009 	beq	be0c <print_size+0x84>
    bde4:	e0625694 	mls	r2, r4, r6, r5
    bde8:	e1a030a4 	lsr	r3, r4, #1
    bdec:	e3a0000a 	mov	r0, #10	; 0xa
    bdf0:	e1a01004 	mov	r1, r4
    bdf4:	e0203290 	mla	r0, r0, r2, r3
    bdf8:	eb000cb6 	bl	f0d8 <__aeabi_uidiv>
    bdfc:	e3500009 	cmp	r0, #9	; 0x9
    be00:	e1a04000 	mov	r4, r0
    be04:	82866001 	addhi	r6, r6, #1	; 0x1
    be08:	8240400a 	subhi	r4, r0, #10	; 0xa
    be0c:	e1a01006 	mov	r1, r6
    be10:	e59f5040 	ldr	r5, [pc, #64]	; be58 <print_size+0xd0>
    be14:	e59f0040 	ldr	r0, [pc, #64]	; be5c <print_size+0xd4>
    be18:	e1a0e00f 	mov	lr, pc
    be1c:	e595f000 	ldr	pc, [r5]
    be20:	e3540000 	cmp	r4, #0	; 0x0
    be24:	0a000003 	beq	be38 <print_size+0xb0>
    be28:	e1a01004 	mov	r1, r4
    be2c:	e59f002c 	ldr	r0, [pc, #44]	; be60 <print_size+0xd8>
    be30:	e1a0e00f 	mov	lr, pc
    be34:	e595f000 	ldr	pc, [r5]
    be38:	e1a01007 	mov	r1, r7
    be3c:	e1a02008 	mov	r2, r8
    be40:	e59f001c 	ldr	r0, [pc, #28]	; be64 <print_size+0xdc>
    be44:	e59f300c 	ldr	r3, [pc, #12]	; be58 <print_size+0xd0>
    be48:	e1a0e00f 	mov	lr, pc
    be4c:	e593f000 	ldr	pc, [r3]
    be50:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    be54:	000fffff 	.word	0x000fffff
    be58:	0800d704 	.word	0x0800d704
    be5c:	00012427 	.word	0x00012427
    be60:	0001242c 	.word	0x0001242c
    be64:	00012431 	.word	0x00012431

0000be68 <print_buffer>:
    be68:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    be6c:	e24dd044 	sub	sp, sp, #68	; 0x44
    be70:	e59d6068 	ldr	r6, [sp, #104]
    be74:	e1a07002 	mov	r7, r2
    be78:	e0020692 	mul	r2, r2, r6
    be7c:	e3520040 	cmp	r2, #64	; 0x40
    be80:	e1a09000 	mov	r9, r0
    be84:	e1a0a001 	mov	sl, r1
    be88:	e1a08003 	mov	r8, r3
    be8c:	9a000003 	bls	bea0 <print_buffer+0x38>
    be90:	e3a00040 	mov	r0, #64	; 0x40
    be94:	e1a01007 	mov	r1, r7
    be98:	eb000c8e 	bl	f0d8 <__aeabi_uidiv>
    be9c:	e1a06000 	mov	r6, r0
    bea0:	e3560000 	cmp	r6, #0	; 0x0
    bea4:	1a00003c 	bne	bf9c <print_buffer+0x134>
    bea8:	e3a00010 	mov	r0, #16	; 0x10
    beac:	e1a01007 	mov	r1, r7
    beb0:	eb000c88 	bl	f0d8 <__aeabi_uidiv>
    beb4:	e1a06000 	mov	r6, r0
    beb8:	ea000037 	b	bf9c <print_buffer+0x134>
    bebc:	e59f00f0 	ldr	r0, [pc, #240]	; bfb4 <print_buffer+0x14c>
    bec0:	e1a01009 	mov	r1, r9
    bec4:	e1a0e00f 	mov	lr, pc
    bec8:	e59bf000 	ldr	pc, [fp]
    becc:	e1560008 	cmp	r6, r8
    bed0:	21a06008 	movcs	r6, r8
    bed4:	e1a0500a 	mov	r5, sl
    bed8:	e3a04000 	mov	r4, #0	; 0x0
    bedc:	ea00000f 	b	bf20 <print_buffer+0xb8>
    bee0:	e3570004 	cmp	r7, #4	; 0x4
    bee4:	05951000 	ldreq	r1, [r5]
    bee8:	059f00c8 	ldreq	r0, [pc, #200]	; bfb8 <print_buffer+0x150>
    beec:	078d1104 	streq	r1, [sp, r4, lsl #2]
    bef0:	0a000006 	beq	bf10 <print_buffer+0xa8>
    bef4:	e3570002 	cmp	r7, #2	; 0x2
    bef8:	01d510b0 	ldrheq	r1, [r5]
    befc:	15d51000 	ldrbne	r1, [r5]
    bf00:	01a03084 	lsleq	r3, r4, #1
    bf04:	059f00b0 	ldreq	r0, [pc, #176]	; bfbc <print_buffer+0x154>
    bf08:	018d10b3 	strheq	r1, [sp, r3]
    bf0c:	17cd1004 	strbne	r1, [sp, r4]
    bf10:	e1a0e00f 	mov	lr, pc
    bf14:	e59bf000 	ldr	pc, [fp]
    bf18:	e0855007 	add	r5, r5, r7
    bf1c:	e2844001 	add	r4, r4, #1	; 0x1
    bf20:	e1540006 	cmp	r4, r6
    bf24:	e59f0094 	ldr	r0, [pc, #148]	; bfc0 <print_buffer+0x158>
    bf28:	3affffec 	bcc	bee0 <print_buffer+0x78>
    bf2c:	e0050697 	mul	r5, r7, r6
    bf30:	e59f008c 	ldr	r0, [pc, #140]	; bfc4 <print_buffer+0x15c>
    bf34:	e08aa005 	add	sl, sl, r5
    bf38:	eb000450 	bl	d080 <puts>
    bf3c:	e3a04000 	mov	r4, #0	; 0x0
    bf40:	ea000009 	b	bf6c <print_buffer+0x104>
    bf44:	e7dd0004 	ldrb	r0, [sp, r4]
    bf48:	e59f2078 	ldr	r2, [pc, #120]	; bfc8 <print_buffer+0x160>
    bf4c:	e2844001 	add	r4, r4, #1	; 0x1
    bf50:	e7d23000 	ldrb	r3, [r2, r0]
    bf54:	e3130097 	tst	r3, #151	; 0x97
    bf58:	0a000001 	beq	bf64 <print_buffer+0xfc>
    bf5c:	e3100080 	tst	r0, #128	; 0x80
    bf60:	0a000000 	beq	bf68 <print_buffer+0x100>
    bf64:	e3a0002e 	mov	r0, #46	; 0x2e
    bf68:	eb00046c 	bl	d120 <putc>
    bf6c:	e1540005 	cmp	r4, r5
    bf70:	3afffff3 	bcc	bf44 <print_buffer+0xdc>
    bf74:	e59f0050 	ldr	r0, [pc, #80]	; bfcc <print_buffer+0x164>
    bf78:	e1a0e00f 	mov	lr, pc
    bf7c:	e59bf000 	ldr	pc, [fp]
    bf80:	eb000469 	bl	d12c <ctrlc>
    bf84:	e3500000 	cmp	r0, #0	; 0x0
    bf88:	13e00000 	mvnne	r0, #0	; 0x0
    bf8c:	1a000006 	bne	bfac <print_buffer+0x144>
    bf90:	e0899005 	add	r9, r9, r5
    bf94:	e0668008 	rsb	r8, r6, r8
    bf98:	ea000000 	b	bfa0 <print_buffer+0x138>
    bf9c:	e59fb02c 	ldr	fp, [pc, #44]	; bfd0 <print_buffer+0x168>
    bfa0:	e3580000 	cmp	r8, #0	; 0x0
    bfa4:	1affffc4 	bne	bebc <print_buffer+0x54>
    bfa8:	e1a00008 	mov	r0, r8
    bfac:	e28dd044 	add	sp, sp, #68	; 0x44
    bfb0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    bfb4:	00012438 	.word	0x00012438
    bfb8:	00012266 	.word	0x00012266
    bfbc:	0001243f 	.word	0x0001243f
    bfc0:	00012445 	.word	0x00012445
    bfc4:	0001244b 	.word	0x0001244b
    bfc8:	00010831 	.word	0x00010831
    bfcc:	0001140d 	.word	0x0001140d
    bfd0:	0800d704 	.word	0x0800d704

0000bfd4 <__div64_32>:
    bfd4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    bfd8:	e8900300 	ldm	r0, {r8, r9}
    bfdc:	e1510009 	cmp	r1, r9
    bfe0:	e1a0a000 	mov	sl, r0
    bfe4:	e1a04001 	mov	r4, r1
    bfe8:	83a06000 	movhi	r6, #0	; 0x0
    bfec:	83a07000 	movhi	r7, #0	; 0x0
    bff0:	8a000007 	bhi	c014 <__div64_32+0x40>
    bff4:	e1a00009 	mov	r0, r9
    bff8:	eb000c36 	bl	f0d8 <__aeabi_uidiv>
    bffc:	e0030094 	mul	r3, r4, r0
    c000:	e3a06000 	mov	r6, #0	; 0x0
    c004:	e1a02006 	mov	r2, r6
    c008:	e1a07000 	mov	r7, r0
    c00c:	e0588002 	subs	r8, r8, r2
    c010:	e0c99003 	sbc	r9, r9, r3
    c014:	e1a00004 	mov	r0, r4
    c018:	e3a01000 	mov	r1, #0	; 0x0
    c01c:	e3a05000 	mov	r5, #0	; 0x0
    c020:	e3a04001 	mov	r4, #1	; 0x1
    c024:	ea000003 	b	c038 <__div64_32+0x64>
    c028:	e0900000 	adds	r0, r0, r0
    c02c:	e0a11001 	adc	r1, r1, r1
    c030:	e0944004 	adds	r4, r4, r4
    c034:	e0a55005 	adc	r5, r5, r5
    c038:	e3510000 	cmp	r1, #0	; 0x0
    c03c:	e3a03000 	mov	r3, #0	; 0x0
    c040:	ca000002 	bgt	c050 <__div64_32+0x7c>
    c044:	1a000002 	bne	c054 <__div64_32+0x80>
    c048:	e3500000 	cmp	r0, #0	; 0x0
    c04c:	0a000000 	beq	c054 <__div64_32+0x80>
    c050:	e3a03001 	mov	r3, #1	; 0x1
    c054:	e1590001 	cmp	r9, r1
    c058:	e3a02000 	mov	r2, #0	; 0x0
    c05c:	8a000002 	bhi	c06c <__div64_32+0x98>
    c060:	1a000002 	bne	c070 <__div64_32+0x9c>
    c064:	e1580000 	cmp	r8, r0
    c068:	9a000000 	bls	c070 <__div64_32+0x9c>
    c06c:	e3a02001 	mov	r2, #1	; 0x1
    c070:	e0033002 	and	r3, r3, r2
    c074:	e31300ff 	tst	r3, #255	; 0xff
    c078:	1affffea 	bne	c028 <__div64_32+0x54>
    c07c:	e1510009 	cmp	r1, r9
    c080:	8a000006 	bhi	c0a0 <__div64_32+0xcc>
    c084:	1a000001 	bne	c090 <__div64_32+0xbc>
    c088:	e1500008 	cmp	r0, r8
    c08c:	8a000003 	bhi	c0a0 <__div64_32+0xcc>
    c090:	e0588000 	subs	r8, r8, r0
    c094:	e0c99001 	sbc	r9, r9, r1
    c098:	e0966004 	adds	r6, r6, r4
    c09c:	e0a77005 	adc	r7, r7, r5
    c0a0:	e1b050a5 	lsrs	r5, r5, #1
    c0a4:	e1a04064 	rrx	r4, r4
    c0a8:	e1943005 	orrs	r3, r4, r5
    c0ac:	0a000002 	beq	c0bc <__div64_32+0xe8>
    c0b0:	e1b010a1 	lsrs	r1, r1, #1
    c0b4:	e1a00060 	rrx	r0, r0
    c0b8:	eaffffef 	b	c07c <__div64_32+0xa8>
    c0bc:	e88a00c0 	stm	sl, {r6, r7}
    c0c0:	e1a00008 	mov	r0, r8
    c0c4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

0000c0c8 <simple_strtoul>:
    c0c8:	e92d4070 	push	{r4, r5, r6, lr}
    c0cc:	e5d03000 	ldrb	r3, [r0]
    c0d0:	e1a05001 	mov	r5, r1
    c0d4:	e3530030 	cmp	r3, #48	; 0x30
    c0d8:	e1a01002 	mov	r1, r2
    c0dc:	1a00000d 	bne	c118 <simple_strtoul+0x50>
    c0e0:	e5f03001 	ldrb	r3, [r0, #1]!
    c0e4:	e3530078 	cmp	r3, #120	; 0x78
    c0e8:	1a000007 	bne	c10c <simple_strtoul+0x44>
    c0ec:	e5d03001 	ldrb	r3, [r0, #1]
    c0f0:	e59f2084 	ldr	r2, [pc, #132]	; c17c <simple_strtoul+0xb4>
    c0f4:	e280c001 	add	ip, r0, #1	; 0x1
    c0f8:	e7d23003 	ldrb	r3, [r2, r3]
    c0fc:	e3130044 	tst	r3, #68	; 0x44
    c100:	11a0000c 	movne	r0, ip
    c104:	13a01010 	movne	r1, #16	; 0x10
    c108:	1a000004 	bne	c120 <simple_strtoul+0x58>
    c10c:	e3510000 	cmp	r1, #0	; 0x0
    c110:	03a01008 	moveq	r1, #8	; 0x8
    c114:	ea000001 	b	c120 <simple_strtoul+0x58>
    c118:	e3520000 	cmp	r2, #0	; 0x0
    c11c:	03a0100a 	moveq	r1, #10	; 0xa
    c120:	e3a04000 	mov	r4, #0	; 0x0
    c124:	e59f6050 	ldr	r6, [pc, #80]	; c17c <simple_strtoul+0xb4>
    c128:	ea000001 	b	c134 <simple_strtoul+0x6c>
    c12c:	e0243491 	mla	r4, r1, r4, r3
    c130:	e2800001 	add	r0, r0, #1	; 0x1
    c134:	e5d0c000 	ldrb	ip, [r0]
    c138:	e7d6200c 	ldrb	r2, [r6, ip]
    c13c:	e24c3030 	sub	r3, ip, #48	; 0x30
    c140:	e3120044 	tst	r2, #68	; 0x44
    c144:	0a000008 	beq	c16c <simple_strtoul+0xa4>
    c148:	e3120004 	tst	r2, #4	; 0x4
    c14c:	1a000004 	bne	c164 <simple_strtoul+0x9c>
    c150:	e24c3020 	sub	r3, ip, #32	; 0x20
    c154:	e3120002 	tst	r2, #2	; 0x2
    c158:	e6ef3073 	uxtb	r3, r3
    c15c:	12433037 	subne	r3, r3, #55	; 0x37
    c160:	024c3037 	subeq	r3, ip, #55	; 0x37
    c164:	e1530001 	cmp	r3, r1
    c168:	3affffef 	bcc	c12c <simple_strtoul+0x64>
    c16c:	e3550000 	cmp	r5, #0	; 0x0
    c170:	15850000 	strne	r0, [r5]
    c174:	e1a00004 	mov	r0, r4
    c178:	e8bd8070 	pop	{r4, r5, r6, pc}
    c17c:	00010831 	.word	0x00010831

0000c180 <simple_strtol>:
    c180:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    c184:	e5d03000 	ldrb	r3, [r0]
    c188:	e24dd004 	sub	sp, sp, #4	; 0x4
    c18c:	e353002d 	cmp	r3, #45	; 0x2d
    c190:	1a000004 	bne	c1a8 <simple_strtol+0x28>
    c194:	e2800001 	add	r0, r0, #1	; 0x1
    c198:	ebffffca 	bl	c0c8 <simple_strtoul>
    c19c:	e2600000 	rsb	r0, r0, #0	; 0x0
    c1a0:	e28dd004 	add	sp, sp, #4	; 0x4
    c1a4:	e8bd8000 	pop	{pc}
    c1a8:	e28dd004 	add	sp, sp, #4	; 0x4
    c1ac:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    c1b0:	eaffffc4 	b	c0c8 <simple_strtoul>

0000c1b4 <ustrtoul>:
    c1b4:	e92d4010 	push	{r4, lr}
    c1b8:	e1a04001 	mov	r4, r1
    c1bc:	ebffffc1 	bl	c0c8 <simple_strtoul>
    c1c0:	e5941000 	ldr	r1, [r4]
    c1c4:	e5d13000 	ldrb	r3, [r1]
    c1c8:	e353004b 	cmp	r3, #75	; 0x4b
    c1cc:	0a00000a 	beq	c1fc <ustrtoul+0x48>
    c1d0:	8a000002 	bhi	c1e0 <ustrtoul+0x2c>
    c1d4:	e3530047 	cmp	r3, #71	; 0x47
    c1d8:	18bd8010 	popne	{r4, pc}
    c1dc:	ea000004 	b	c1f4 <ustrtoul+0x40>
    c1e0:	e353004d 	cmp	r3, #77	; 0x4d
    c1e4:	0a000003 	beq	c1f8 <ustrtoul+0x44>
    c1e8:	e353006b 	cmp	r3, #107	; 0x6b
    c1ec:	18bd8010 	popne	{r4, pc}
    c1f0:	ea000001 	b	c1fc <ustrtoul+0x48>
    c1f4:	e1a00500 	lsl	r0, r0, #10
    c1f8:	e1a00500 	lsl	r0, r0, #10
    c1fc:	e5d13001 	ldrb	r3, [r1, #1]
    c200:	e1a00500 	lsl	r0, r0, #10
    c204:	e3530069 	cmp	r3, #105	; 0x69
    c208:	18bd8010 	popne	{r4, pc}
    c20c:	e5d13002 	ldrb	r3, [r1, #2]
    c210:	e2812002 	add	r2, r1, #2	; 0x2
    c214:	e3530042 	cmp	r3, #66	; 0x42
    c218:	02813003 	addeq	r3, r1, #3	; 0x3
    c21c:	05843000 	streq	r3, [r4]
    c220:	15842000 	strne	r2, [r4]
    c224:	e8bd8010 	pop	{r4, pc}

0000c228 <simple_strtoull>:
    c228:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    c22c:	e5d03000 	ldrb	r3, [r0]
    c230:	e1a07000 	mov	r7, r0
    c234:	e3530030 	cmp	r3, #48	; 0x30
    c238:	e1a08001 	mov	r8, r1
    c23c:	e1a06002 	mov	r6, r2
    c240:	1a00000d 	bne	c27c <simple_strtoull+0x54>
    c244:	e5f73001 	ldrb	r3, [r7, #1]!
    c248:	e3530078 	cmp	r3, #120	; 0x78
    c24c:	1a000007 	bne	c270 <simple_strtoull+0x48>
    c250:	e5d73001 	ldrb	r3, [r7, #1]
    c254:	e59f20ac 	ldr	r2, [pc, #172]	; c308 <simple_strtoull+0xe0>
    c258:	e2870001 	add	r0, r7, #1	; 0x1
    c25c:	e7d23003 	ldrb	r3, [r2, r3]
    c260:	e3130044 	tst	r3, #68	; 0x44
    c264:	11a07000 	movne	r7, r0
    c268:	13a06010 	movne	r6, #16	; 0x10
    c26c:	1a000004 	bne	c284 <simple_strtoull+0x5c>
    c270:	e3560000 	cmp	r6, #0	; 0x0
    c274:	03a06008 	moveq	r6, #8	; 0x8
    c278:	ea000001 	b	c284 <simple_strtoull+0x5c>
    c27c:	e3520000 	cmp	r2, #0	; 0x0
    c280:	03a0600a 	moveq	r6, #10	; 0xa
    c284:	e3a04000 	mov	r4, #0	; 0x0
    c288:	e3a05000 	mov	r5, #0	; 0x0
    c28c:	e59f9074 	ldr	r9, [pc, #116]	; c308 <simple_strtoull+0xe0>
    c290:	e3a0a000 	mov	sl, #0	; 0x0
    c294:	ea000004 	b	c2ac <simple_strtoull+0x84>
    c298:	e0832694 	umull	r2, r3, r4, r6
    c29c:	e0233596 	mla	r3, r6, r5, r3
    c2a0:	e0904002 	adds	r4, r0, r2
    c2a4:	e0a15003 	adc	r5, r1, r3
    c2a8:	e2877001 	add	r7, r7, #1	; 0x1
    c2ac:	e5d7c000 	ldrb	ip, [r7]
    c2b0:	e7d9200c 	ldrb	r2, [r9, ip]
    c2b4:	e24c3030 	sub	r3, ip, #48	; 0x30
    c2b8:	e3120044 	tst	r2, #68	; 0x44
    c2bc:	0a00000c 	beq	c2f4 <simple_strtoull+0xcc>
    c2c0:	e3120004 	tst	r2, #4	; 0x4
    c2c4:	1a000004 	bne	c2dc <simple_strtoull+0xb4>
    c2c8:	e24c3020 	sub	r3, ip, #32	; 0x20
    c2cc:	e3120002 	tst	r2, #2	; 0x2
    c2d0:	e6ef3073 	uxtb	r3, r3
    c2d4:	12433037 	subne	r3, r3, #55	; 0x37
    c2d8:	024c3037 	subeq	r3, ip, #55	; 0x37
    c2dc:	e1a00003 	mov	r0, r3
    c2e0:	e1a01fc0 	asr	r1, r0, #31
    c2e4:	e15a0001 	cmp	sl, r1
    c2e8:	1a000001 	bne	c2f4 <simple_strtoull+0xcc>
    c2ec:	e1560000 	cmp	r6, r0
    c2f0:	8affffe8 	bhi	c298 <simple_strtoull+0x70>
    c2f4:	e3580000 	cmp	r8, #0	; 0x0
    c2f8:	15887000 	strne	r7, [r8]
    c2fc:	e1a01005 	mov	r1, r5
    c300:	e1a00004 	mov	r0, r4
    c304:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    c308:	00010831 	.word	0x00010831

0000c30c <put_dec_trunc>:
    c30c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    c310:	e1a03421 	lsr	r3, r1, #8
    c314:	e1a04221 	lsr	r4, r1, #4
    c318:	e204400f 	and	r4, r4, #15	; 0xf
    c31c:	e203600f 	and	r6, r3, #15	; 0xf
    c320:	e1a05621 	lsr	r5, r1, #12
    c324:	e0842006 	add	r2, r4, r6
    c328:	e201100f 	and	r1, r1, #15	; 0xf
    c32c:	e3a03006 	mov	r3, #6	; 0x6
    c330:	e0822005 	add	r2, r2, r5
    c334:	e0221293 	mla	r2, r3, r2, r1
    c338:	e3a080cd 	mov	r8, #205	; 0xcd
    c33c:	e00c0298 	mul	ip, r8, r2
    c340:	e3a01009 	mov	r1, #9	; 0x9
    c344:	e0214591 	mla	r1, r1, r5, r4
    c348:	e1a0c5ac 	lsr	ip, ip, #11
    c34c:	e3a0700a 	mov	r7, #10	; 0xa
    c350:	e3a03005 	mov	r3, #5	; 0x5
    c354:	e0231693 	mla	r3, r3, r6, r1
    c358:	e0622c97 	mls	r2, r7, ip, r2
    c35c:	e093100c 	adds	r1, r3, ip
    c360:	e2822030 	add	r2, r2, #48	; 0x30
    c364:	e1a04000 	mov	r4, r0
    c368:	e4c02001 	strb	r2, [r0], #1
    c36c:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    c370:	e0030198 	mul	r3, r8, r1
    c374:	e1a035a3 	lsr	r3, r3, #11
    c378:	e0621397 	mls	r2, r7, r3, r1
    c37c:	e0831086 	add	r1, r3, r6, lsl #1
    c380:	e2822030 	add	r2, r2, #48	; 0x30
    c384:	e1913005 	orrs	r3, r1, r5
    c388:	e5c42001 	strb	r2, [r4, #1]
    c38c:	e2800001 	add	r0, r0, #1	; 0x1
    c390:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    c394:	e3a0300d 	mov	r3, #13	; 0xd
    c398:	e0030193 	mul	r3, r3, r1
    c39c:	e1a033a3 	lsr	r3, r3, #7
    c3a0:	e0621397 	mls	r2, r7, r3, r1
    c3a4:	e0931105 	adds	r1, r3, r5, lsl #2
    c3a8:	e2822030 	add	r2, r2, #48	; 0x30
    c3ac:	e4c02001 	strb	r2, [r0], #1
    c3b0:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    c3b4:	e0030198 	mul	r3, r8, r1
    c3b8:	e1a0c5a3 	lsr	ip, r3, #11
    c3bc:	e0631c97 	mls	r3, r7, ip, r1
    c3c0:	e35c0000 	cmp	ip, #0	; 0x0
    c3c4:	e2833030 	add	r3, r3, #48	; 0x30
    c3c8:	e4c03001 	strb	r3, [r0], #1
    c3cc:	128c3030 	addne	r3, ip, #48	; 0x30
    c3d0:	14c03001 	strbne	r3, [r0], #1
    c3d4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000c3d8 <string>:
    c3d8:	e59fc0a4 	ldr	ip, [pc, #164]	; c484 <string+0xac>
    c3dc:	e3510000 	cmp	r1, #0	; 0x0
    c3e0:	e92d4070 	push	{r4, r5, r6, lr}
    c3e4:	11a06001 	movne	r6, r1
    c3e8:	01a0600c 	moveq	r6, ip
    c3ec:	e1a01003 	mov	r1, r3
    c3f0:	e1a04000 	mov	r4, r0
    c3f4:	e1a00006 	mov	r0, r6
    c3f8:	e1a05002 	mov	r5, r2
    c3fc:	ebffd3e3 	bl	1390 <strnlen>
    c400:	e59d3010 	ldr	r3, [sp, #16]
    c404:	e1a0c000 	mov	ip, r0
    c408:	e3130010 	tst	r3, #16	; 0x10
    c40c:	01a02004 	moveq	r2, r4
    c410:	03a01020 	moveq	r1, #32	; 0x20
    c414:	0a000001 	beq	c420 <string+0x48>
    c418:	ea000006 	b	c438 <string+0x60>
    c41c:	e5421001 	strb	r1, [r2, #-1]
    c420:	e2455001 	sub	r5, r5, #1	; 0x1
    c424:	e2853001 	add	r3, r5, #1	; 0x1
    c428:	e15c0003 	cmp	ip, r3
    c42c:	e1a04002 	mov	r4, r2
    c430:	e2822001 	add	r2, r2, #1	; 0x1
    c434:	bafffff8 	blt	c41c <string+0x44>
    c438:	e3a02000 	mov	r2, #0	; 0x0
    c43c:	ea000002 	b	c44c <string+0x74>
    c440:	e7d63002 	ldrb	r3, [r6, r2]
    c444:	e7c43002 	strb	r3, [r4, r2]
    c448:	e2822001 	add	r2, r2, #1	; 0x1
    c44c:	e152000c 	cmp	r2, ip
    c450:	e0843002 	add	r3, r4, r2
    c454:	bafffff9 	blt	c440 <string+0x68>
    c458:	e1a01003 	mov	r1, r3
    c45c:	e1a02005 	mov	r2, r5
    c460:	e3a03020 	mov	r3, #32	; 0x20
    c464:	ea000000 	b	c46c <string+0x94>
    c468:	e5413001 	strb	r3, [r1, #-1]
    c46c:	e15c0002 	cmp	ip, r2
    c470:	e1a00001 	mov	r0, r1
    c474:	e2422001 	sub	r2, r2, #1	; 0x1
    c478:	e2811001 	add	r1, r1, #1	; 0x1
    c47c:	bafffff9 	blt	c468 <string+0x90>
    c480:	e8bd8070 	pop	{r4, r5, r6, pc}
    c484:	00012450 	.word	0x00012450

0000c488 <put_dec>:
    c488:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    c48c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    c490:	e28d8008 	add	r8, sp, #8	; 0x8
    c494:	e16820f8 	strd	r2, [r8, #-8]!
    c498:	e1a07000 	mov	r7, r0
    c49c:	e3a090cd 	mov	r9, #205	; 0xcd
    c4a0:	e3a0a00a 	mov	sl, #10	; 0xa
    c4a4:	e3a0b00d 	mov	fp, #13	; 0xd
    c4a8:	e59d5004 	ldr	r5, [sp, #4]
    c4ac:	e1a0000d 	mov	r0, sp
    c4b0:	e3550000 	cmp	r5, #0	; 0x0
    c4b4:	e59f10f0 	ldr	r1, [pc, #240]	; c5ac <put_dec+0x124>
    c4b8:	e59d4000 	ldr	r4, [sp]
    c4bc:	1a00000f 	bne	c500 <put_dec+0x78>
    c4c0:	e59f30e8 	ldr	r3, [pc, #232]	; c5b0 <put_dec+0x128>
    c4c4:	e1a00004 	mov	r0, r4
    c4c8:	e1540003 	cmp	r4, r3
    c4cc:	8a000004 	bhi	c4e4 <put_dec+0x5c>
    c4d0:	e1a00007 	mov	r0, r7
    c4d4:	e1a01004 	mov	r1, r4
    c4d8:	ebffff8b 	bl	c30c <put_dec_trunc>
    c4dc:	e28dd00c 	add	sp, sp, #12	; 0xc
    c4e0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    c4e4:	eb000b77 	bl	f2c8 <__aeabi_uidivmod>
    c4e8:	e1a00004 	mov	r0, r4
    c4ec:	e1a06001 	mov	r6, r1
    c4f0:	e59f10b4 	ldr	r1, [pc, #180]	; c5ac <put_dec+0x124>
    c4f4:	eb000af7 	bl	f0d8 <__aeabi_uidiv>
    c4f8:	e88d0021 	stm	sp, {r0, r5}
    c4fc:	ea000001 	b	c508 <put_dec+0x80>
    c500:	ebfffeb3 	bl	bfd4 <__div64_32>
    c504:	e1a06000 	mov	r6, r0
    c508:	e1a02226 	lsr	r2, r6, #4
    c50c:	e1a0c426 	lsr	ip, r6, #8
    c510:	e20cc00f 	and	ip, ip, #15	; 0xf
    c514:	e202200f 	and	r2, r2, #15	; 0xf
    c518:	e1a0e626 	lsr	lr, r6, #12
    c51c:	e082500c 	add	r5, r2, ip
    c520:	e085500e 	add	r5, r5, lr
    c524:	e206300f 	and	r3, r6, #15	; 0xf
    c528:	e3a01006 	mov	r1, #6	; 0x6
    c52c:	e0253591 	mla	r5, r1, r5, r3
    c530:	e3a03009 	mov	r3, #9	; 0x9
    c534:	e0222e93 	mla	r2, r3, lr, r2
    c538:	e3a01005 	mov	r1, #5	; 0x5
    c53c:	e0000599 	mul	r0, r9, r5
    c540:	e0222c91 	mla	r2, r1, ip, r2
    c544:	e1a005a0 	lsr	r0, r0, #11
    c548:	e0822000 	add	r2, r2, r0
    c54c:	e0010299 	mul	r1, r9, r2
    c550:	e1a015a1 	lsr	r1, r1, #11
    c554:	e081c08c 	add	ip, r1, ip, lsl #1
    c558:	e0030c9b 	mul	r3, fp, ip
    c55c:	e1a033a3 	lsr	r3, r3, #7
    c560:	e083e10e 	add	lr, r3, lr, lsl #2
    c564:	e0040e99 	mul	r4, r9, lr
    c568:	e1a045a4 	lsr	r4, r4, #11
    c56c:	e06ee49a 	mls	lr, sl, r4, lr
    c570:	e060509a 	mls	r0, sl, r0, r5
    c574:	e061219a 	mls	r1, sl, r1, r2
    c578:	e063c39a 	mls	r3, sl, r3, ip
    c57c:	e2800030 	add	r0, r0, #48	; 0x30
    c580:	e2811030 	add	r1, r1, #48	; 0x30
    c584:	e2833030 	add	r3, r3, #48	; 0x30
    c588:	e28ee030 	add	lr, lr, #48	; 0x30
    c58c:	e2844030 	add	r4, r4, #48	; 0x30
    c590:	e5c70000 	strb	r0, [r7]
    c594:	e5c71001 	strb	r1, [r7, #1]
    c598:	e5c73002 	strb	r3, [r7, #2]
    c59c:	e5c7e003 	strb	lr, [r7, #3]
    c5a0:	e5c74004 	strb	r4, [r7, #4]
    c5a4:	e2877005 	add	r7, r7, #5	; 0x5
    c5a8:	eaffffbe 	b	c4a8 <put_dec+0x20>
    c5ac:	000186a0 	.word	0x000186a0
    c5b0:	0001869f 	.word	0x0001869f

0000c5b4 <number>:
    c5b4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    c5b8:	e24dd05c 	sub	sp, sp, #92	; 0x5c
    c5bc:	e1a0a000 	mov	sl, r0
    c5c0:	e59d0080 	ldr	r0, [sp, #128]
    c5c4:	e59d808c 	ldr	r8, [sp, #140]
    c5c8:	e250100a 	subs	r1, r0, #10	; 0xa
    c5cc:	13a01001 	movne	r1, #1	; 0x1
    c5d0:	e0011328 	and	r1, r1, r8, lsr #6
    c5d4:	e3180010 	tst	r8, #16	; 0x10
    c5d8:	e58d1008 	str	r1, [sp, #8]
    c5dc:	e2081020 	and	r1, r8, #32	; 0x20
    c5e0:	13c88001 	bicne	r8, r8, #1	; 0x1
    c5e4:	e3180002 	tst	r8, #2	; 0x2
    c5e8:	e1a06002 	mov	r6, r2
    c5ec:	e1a07003 	mov	r7, r3
    c5f0:	e59d5084 	ldr	r5, [sp, #132]
    c5f4:	e58d1004 	str	r1, [sp, #4]
    c5f8:	0a000010 	beq	c640 <number+0x8c>
    c5fc:	e3530000 	cmp	r3, #0	; 0x0
    c600:	ba000002 	blt	c610 <number+0x5c>
    c604:	e3180004 	tst	r8, #4	; 0x4
    c608:	0a000008 	beq	c630 <number+0x7c>
    c60c:	ea000004 	b	c624 <number+0x70>
    c610:	e2766000 	rsbs	r6, r6, #0	; 0x0
    c614:	e2e77000 	rsc	r7, r7, #0	; 0x0
    c618:	e2455001 	sub	r5, r5, #1	; 0x1
    c61c:	e3a0902d 	mov	r9, #45	; 0x2d
    c620:	ea000007 	b	c644 <number+0x90>
    c624:	e2455001 	sub	r5, r5, #1	; 0x1
    c628:	e3a0902b 	mov	r9, #43	; 0x2b
    c62c:	ea000004 	b	c644 <number+0x90>
    c630:	e3180008 	tst	r8, #8	; 0x8
    c634:	12455001 	subne	r5, r5, #1	; 0x1
    c638:	13a09020 	movne	r9, #32	; 0x20
    c63c:	1a000000 	bne	c644 <number+0x90>
    c640:	e3a09000 	mov	r9, #0	; 0x0
    c644:	e59d2008 	ldr	r2, [sp, #8]
    c648:	e3520000 	cmp	r2, #0	; 0x0
    c64c:	0a000003 	beq	c660 <number+0xac>
    c650:	e59d3080 	ldr	r3, [sp, #128]
    c654:	e2455001 	sub	r5, r5, #1	; 0x1
    c658:	e3530010 	cmp	r3, #16	; 0x10
    c65c:	02455001 	subeq	r5, r5, #1	; 0x1
    c660:	e196c007 	orrs	ip, r6, r7
    c664:	03a03030 	moveq	r3, #48	; 0x30
    c668:	05cd3016 	strbeq	r3, [sp, #22]
    c66c:	03a04001 	moveq	r4, #1	; 0x1
    c670:	0a000020 	beq	c6f8 <number+0x144>
    c674:	e59d0080 	ldr	r0, [sp, #128]
    c678:	e350000a 	cmp	r0, #10	; 0xa
    c67c:	0a000017 	beq	c6e0 <number+0x12c>
    c680:	e3500010 	cmp	r0, #16	; 0x10
    c684:	03a01004 	moveq	r1, #4	; 0x4
    c688:	13a01003 	movne	r1, #3	; 0x3
    c68c:	e58d100c 	str	r1, [sp, #12]
    c690:	e240b001 	sub	fp, r0, #1	; 0x1
    c694:	e3a04000 	mov	r4, #0	; 0x0
    c698:	e6ef3076 	uxtb	r3, r6
    c69c:	e59f2164 	ldr	r2, [pc, #356]	; c808 <number+0x254>
    c6a0:	e003300b 	and	r3, r3, fp
    c6a4:	e59dc004 	ldr	ip, [sp, #4]
    c6a8:	e7d23003 	ldrb	r3, [r2, r3]
    c6ac:	e1a00006 	mov	r0, r6
    c6b0:	e18c3003 	orr	r3, ip, r3
    c6b4:	e28dc016 	add	ip, sp, #22	; 0x16
    c6b8:	e1a01007 	mov	r1, r7
    c6bc:	e59d200c 	ldr	r2, [sp, #12]
    c6c0:	e7cc3004 	strb	r3, [ip, r4]
    c6c4:	eb000b93 	bl	f518 <__aeabi_llsr>
    c6c8:	e1a06000 	mov	r6, r0
    c6cc:	e1960001 	orrs	r0, r6, r1
    c6d0:	e1a07001 	mov	r7, r1
    c6d4:	e2844001 	add	r4, r4, #1	; 0x1
    c6d8:	1affffee 	bne	c698 <number+0xe4>
    c6dc:	ea000005 	b	c6f8 <number+0x144>
    c6e0:	e28d4016 	add	r4, sp, #22	; 0x16
    c6e4:	e1a00004 	mov	r0, r4
    c6e8:	e1a02006 	mov	r2, r6
    c6ec:	e1a03007 	mov	r3, r7
    c6f0:	ebffff64 	bl	c488 <put_dec>
    c6f4:	e0644000 	rsb	r4, r4, r0
    c6f8:	e59d3088 	ldr	r3, [sp, #136]
    c6fc:	e1540003 	cmp	r4, r3
    c700:	a1a01004 	movge	r1, r4
    c704:	b1a01003 	movlt	r1, r3
    c708:	e3180011 	tst	r8, #17	; 0x11
    c70c:	e061c005 	rsb	ip, r1, r5
    c710:	01a0300a 	moveq	r3, sl
    c714:	03a02020 	moveq	r2, #32	; 0x20
    c718:	0a000001 	beq	c724 <number+0x170>
    c71c:	ea000004 	b	c734 <number+0x180>
    c720:	e5432001 	strb	r2, [r3, #-1]
    c724:	e25cc001 	subs	ip, ip, #1	; 0x1
    c728:	e1a0a003 	mov	sl, r3
    c72c:	e2833001 	add	r3, r3, #1	; 0x1
    c730:	5afffffa 	bpl	c720 <number+0x16c>
    c734:	e3590000 	cmp	r9, #0	; 0x0
    c738:	14ca9001 	strbne	r9, [sl], #1
    c73c:	e59d2008 	ldr	r2, [sp, #8]
    c740:	e3520000 	cmp	r2, #0	; 0x0
    c744:	0a000006 	beq	c764 <number+0x1b0>
    c748:	e59d3080 	ldr	r3, [sp, #128]
    c74c:	e3530010 	cmp	r3, #16	; 0x10
    c750:	e3a03030 	mov	r3, #48	; 0x30
    c754:	e4ca3001 	strb	r3, [sl], #1
    c758:	059d0004 	ldreq	r0, [sp, #4]
    c75c:	03803058 	orreq	r3, r0, #88	; 0x58
    c760:	04ca3001 	strbeq	r3, [sl], #1
    c764:	e3180010 	tst	r8, #16	; 0x10
    c768:	1a000009 	bne	c794 <number+0x1e0>
    c76c:	e3180001 	tst	r8, #1	; 0x1
    c770:	13a02030 	movne	r2, #48	; 0x30
    c774:	03a02020 	moveq	r2, #32	; 0x20
    c778:	e1a0300a 	mov	r3, sl
    c77c:	ea000000 	b	c784 <number+0x1d0>
    c780:	e5432001 	strb	r2, [r3, #-1]
    c784:	e25cc001 	subs	ip, ip, #1	; 0x1
    c788:	e1a0a003 	mov	sl, r3
    c78c:	e2833001 	add	r3, r3, #1	; 0x1
    c790:	5afffffa 	bpl	c780 <number+0x1cc>
    c794:	e1a0000a 	mov	r0, sl
    c798:	e3a03030 	mov	r3, #48	; 0x30
    c79c:	ea000000 	b	c7a4 <number+0x1f0>
    c7a0:	e5403001 	strb	r3, [r0, #-1]
    c7a4:	e2411001 	sub	r1, r1, #1	; 0x1
    c7a8:	e1540001 	cmp	r4, r1
    c7ac:	e1a02000 	mov	r2, r0
    c7b0:	e2800001 	add	r0, r0, #1	; 0x1
    c7b4:	dafffff9 	ble	c7a0 <number+0x1ec>
    c7b8:	e28d1058 	add	r1, sp, #88	; 0x58
    c7bc:	e0813004 	add	r3, r1, r4
    c7c0:	e2431043 	sub	r1, r3, #67	; 0x43
    c7c4:	ea000001 	b	c7d0 <number+0x21c>
    c7c8:	e5d13001 	ldrb	r3, [r1, #1]
    c7cc:	e5423001 	strb	r3, [r2, #-1]
    c7d0:	e2544001 	subs	r4, r4, #1	; 0x1
    c7d4:	e1a03002 	mov	r3, r2
    c7d8:	e2411001 	sub	r1, r1, #1	; 0x1
    c7dc:	e2822001 	add	r2, r2, #1	; 0x1
    c7e0:	5afffff8 	bpl	c7c8 <number+0x214>
    c7e4:	e3a02020 	mov	r2, #32	; 0x20
    c7e8:	ea000000 	b	c7f0 <number+0x23c>
    c7ec:	e5432001 	strb	r2, [r3, #-1]
    c7f0:	e25cc001 	subs	ip, ip, #1	; 0x1
    c7f4:	e1a00003 	mov	r0, r3
    c7f8:	e2833001 	add	r3, r3, #1	; 0x1
    c7fc:	5afffffa 	bpl	c7ec <number+0x238>
    c800:	e28dd05c 	add	sp, sp, #92	; 0x5c
    c804:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    c808:	00010821 	.word	0x00010821

0000c80c <vsprintf>:
    c80c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    c810:	e24dd084 	sub	sp, sp, #132	; 0x84
    c814:	e58d0028 	str	r0, [sp, #40]
    c818:	e1a08001 	mov	r8, r1
    c81c:	e1a0a000 	mov	sl, r0
    c820:	e28d107b 	add	r1, sp, #123	; 0x7b
    c824:	e28d007d 	add	r0, sp, #125	; 0x7d
    c828:	e58d2024 	str	r2, [sp, #36]
    c82c:	e1a06002 	mov	r6, r2
    c830:	e58d001c 	str	r0, [sp, #28]
    c834:	e58d1018 	str	r1, [sp, #24]
    c838:	ea0001a3 	b	cecc <vsprintf+0x6c0>
    c83c:	e3530025 	cmp	r3, #37	; 0x25
    c840:	1a00015f 	bne	cdc4 <vsprintf+0x5b8>
    c844:	e3a07000 	mov	r7, #0	; 0x0
    c848:	e5f82001 	ldrb	r2, [r8, #1]!
    c84c:	e352002b 	cmp	r2, #43	; 0x2b
    c850:	0a00000c 	beq	c888 <vsprintf+0x7c>
    c854:	8a000004 	bhi	c86c <vsprintf+0x60>
    c858:	e3520020 	cmp	r2, #32	; 0x20
    c85c:	0a00000b 	beq	c890 <vsprintf+0x84>
    c860:	e3520023 	cmp	r2, #35	; 0x23
    c864:	1a00000f 	bne	c8a8 <vsprintf+0x9c>
    c868:	ea00000a 	b	c898 <vsprintf+0x8c>
    c86c:	e352002d 	cmp	r2, #45	; 0x2d
    c870:	0a000002 	beq	c880 <vsprintf+0x74>
    c874:	e3520030 	cmp	r2, #48	; 0x30
    c878:	1a00000a 	bne	c8a8 <vsprintf+0x9c>
    c87c:	ea000007 	b	c8a0 <vsprintf+0x94>
    c880:	e3877010 	orr	r7, r7, #16	; 0x10
    c884:	eaffffef 	b	c848 <vsprintf+0x3c>
    c888:	e3877004 	orr	r7, r7, #4	; 0x4
    c88c:	eaffffed 	b	c848 <vsprintf+0x3c>
    c890:	e3877008 	orr	r7, r7, #8	; 0x8
    c894:	eaffffeb 	b	c848 <vsprintf+0x3c>
    c898:	e3877040 	orr	r7, r7, #64	; 0x40
    c89c:	eaffffe9 	b	c848 <vsprintf+0x3c>
    c8a0:	e3877001 	orr	r7, r7, #1	; 0x1
    c8a4:	eaffffe7 	b	c848 <vsprintf+0x3c>
    c8a8:	e2423030 	sub	r3, r2, #48	; 0x30
    c8ac:	e6ef3073 	uxtb	r3, r3
    c8b0:	e3530009 	cmp	r3, #9	; 0x9
    c8b4:	93a09000 	movls	r9, #0	; 0x0
    c8b8:	93a0100a 	movls	r1, #10	; 0xa
    c8bc:	9a000002 	bls	c8cc <vsprintf+0xc0>
    c8c0:	ea000007 	b	c8e4 <vsprintf+0xd8>
    c8c4:	e0292991 	mla	r9, r1, r9, r2
    c8c8:	e2888001 	add	r8, r8, #1	; 0x1
    c8cc:	e5d83000 	ldrb	r3, [r8]
    c8d0:	e2432030 	sub	r2, r3, #48	; 0x30
    c8d4:	e6ef3072 	uxtb	r3, r2
    c8d8:	e3530009 	cmp	r3, #9	; 0x9
    c8dc:	9afffff8 	bls	c8c4 <vsprintf+0xb8>
    c8e0:	ea000008 	b	c908 <vsprintf+0xfc>
    c8e4:	e352002a 	cmp	r2, #42	; 0x2a
    c8e8:	13e09000 	mvnne	r9, #0	; 0x0
    c8ec:	1a000005 	bne	c908 <vsprintf+0xfc>
    c8f0:	e5969000 	ldr	r9, [r6]
    c8f4:	e2888001 	add	r8, r8, #1	; 0x1
    c8f8:	e3590000 	cmp	r9, #0	; 0x0
    c8fc:	e2866004 	add	r6, r6, #4	; 0x4
    c900:	b2699000 	rsblt	r9, r9, #0	; 0x0
    c904:	b3877010 	orrlt	r7, r7, #16	; 0x10
    c908:	e5d83000 	ldrb	r3, [r8]
    c90c:	e353002e 	cmp	r3, #46	; 0x2e
    c910:	13e02000 	mvnne	r2, #0	; 0x0
    c914:	158d202c 	strne	r2, [sp, #44]
    c918:	1a00001d 	bne	c994 <vsprintf+0x188>
    c91c:	e5f82001 	ldrb	r2, [r8, #1]!
    c920:	e2423030 	sub	r3, r2, #48	; 0x30
    c924:	e6ef3073 	uxtb	r3, r3
    c928:	e3530009 	cmp	r3, #9	; 0x9
    c92c:	93a03000 	movls	r3, #0	; 0x0
    c930:	958d302c 	strls	r3, [sp, #44]
    c934:	93a0100a 	movls	r1, #10	; 0xa
    c938:	9a000004 	bls	c950 <vsprintf+0x144>
    c93c:	ea000009 	b	c968 <vsprintf+0x15c>
    c940:	e59d002c 	ldr	r0, [sp, #44]
    c944:	e2888001 	add	r8, r8, #1	; 0x1
    c948:	e0202091 	mla	r0, r1, r0, r2
    c94c:	e58d002c 	str	r0, [sp, #44]
    c950:	e5d83000 	ldrb	r3, [r8]
    c954:	e2432030 	sub	r2, r3, #48	; 0x30
    c958:	e6ef3072 	uxtb	r3, r2
    c95c:	e3530009 	cmp	r3, #9	; 0x9
    c960:	9afffff6 	bls	c940 <vsprintf+0x134>
    c964:	ea000005 	b	c980 <vsprintf+0x174>
    c968:	e352002a 	cmp	r2, #42	; 0x2a
    c96c:	1a000006 	bne	c98c <vsprintf+0x180>
    c970:	e5961000 	ldr	r1, [r6]
    c974:	e2888001 	add	r8, r8, #1	; 0x1
    c978:	e58d102c 	str	r1, [sp, #44]
    c97c:	e2866004 	add	r6, r6, #4	; 0x4
    c980:	e59d202c 	ldr	r2, [sp, #44]
    c984:	e3520000 	cmp	r2, #0	; 0x0
    c988:	aa000001 	bge	c994 <vsprintf+0x188>
    c98c:	e3a03000 	mov	r3, #0	; 0x0
    c990:	e58d302c 	str	r3, [sp, #44]
    c994:	e5d83000 	ldrb	r3, [r8]
    c998:	e353006c 	cmp	r3, #108	; 0x6c
    c99c:	13530068 	cmpne	r3, #104	; 0x68
    c9a0:	0a000008 	beq	c9c8 <vsprintf+0x1bc>
    c9a4:	e353004c 	cmp	r3, #76	; 0x4c
    c9a8:	0a000006 	beq	c9c8 <vsprintf+0x1bc>
    c9ac:	e353005a 	cmp	r3, #90	; 0x5a
    c9b0:	0a000004 	beq	c9c8 <vsprintf+0x1bc>
    c9b4:	e353007a 	cmp	r3, #122	; 0x7a
    c9b8:	0a000002 	beq	c9c8 <vsprintf+0x1bc>
    c9bc:	e3530074 	cmp	r3, #116	; 0x74
    c9c0:	13e02000 	mvnne	r2, #0	; 0x0
    c9c4:	1a000007 	bne	c9e8 <vsprintf+0x1dc>
    c9c8:	e353006c 	cmp	r3, #108	; 0x6c
    c9cc:	e1a02003 	mov	r2, r3
    c9d0:	e2888001 	add	r8, r8, #1	; 0x1
    c9d4:	1a000003 	bne	c9e8 <vsprintf+0x1dc>
    c9d8:	e5d83000 	ldrb	r3, [r8]
    c9dc:	e353006c 	cmp	r3, #108	; 0x6c
    c9e0:	02888001 	addeq	r8, r8, #1	; 0x1
    c9e4:	03a0204c 	moveq	r2, #76	; 0x4c
    c9e8:	e5d83000 	ldrb	r3, [r8]
    c9ec:	e353006e 	cmp	r3, #110	; 0x6e
    c9f0:	0a0000df 	beq	cd74 <vsprintf+0x568>
    c9f4:	8a00000c 	bhi	ca2c <vsprintf+0x220>
    c9f8:	e3530063 	cmp	r3, #99	; 0x63
    c9fc:	0a000017 	beq	ca60 <vsprintf+0x254>
    ca00:	8a000004 	bhi	ca18 <vsprintf+0x20c>
    ca04:	e3530025 	cmp	r3, #37	; 0x25
    ca08:	0a0000ed 	beq	cdc4 <vsprintf+0x5b8>
    ca0c:	e3530058 	cmp	r3, #88	; 0x58
    ca10:	1a0000e5 	bne	cdac <vsprintf+0x5a0>
    ca14:	ea0000e0 	b	cd9c <vsprintf+0x590>
    ca18:	e3530064 	cmp	r3, #100	; 0x64
    ca1c:	0a0000e0 	beq	cda4 <vsprintf+0x598>
    ca20:	e3530069 	cmp	r3, #105	; 0x69
    ca24:	1a0000e0 	bne	cdac <vsprintf+0x5a0>
    ca28:	ea0000dd 	b	cda4 <vsprintf+0x598>
    ca2c:	e3530073 	cmp	r3, #115	; 0x73
    ca30:	0a000021 	beq	cabc <vsprintf+0x2b0>
    ca34:	8a000004 	bhi	ca4c <vsprintf+0x240>
    ca38:	e353006f 	cmp	r3, #111	; 0x6f
    ca3c:	0a0000d3 	beq	cd90 <vsprintf+0x584>
    ca40:	e3530070 	cmp	r3, #112	; 0x70
    ca44:	1a0000d8 	bne	cdac <vsprintf+0x5a0>
    ca48:	ea000023 	b	cadc <vsprintf+0x2d0>
    ca4c:	e3530075 	cmp	r3, #117	; 0x75
    ca50:	0a0000dd 	beq	cdcc <vsprintf+0x5c0>
    ca54:	e3530078 	cmp	r3, #120	; 0x78
    ca58:	1a0000d3 	bne	cdac <vsprintf+0x5a0>
    ca5c:	ea0000cd 	b	cd98 <vsprintf+0x58c>
    ca60:	e3170010 	tst	r7, #16	; 0x10
    ca64:	01a0300a 	moveq	r3, sl
    ca68:	03a02020 	moveq	r2, #32	; 0x20
    ca6c:	0a000001 	beq	ca78 <vsprintf+0x26c>
    ca70:	ea000005 	b	ca8c <vsprintf+0x280>
    ca74:	e5432001 	strb	r2, [r3, #-1]
    ca78:	e2499001 	sub	r9, r9, #1	; 0x1
    ca7c:	e3590000 	cmp	r9, #0	; 0x0
    ca80:	e1a0a003 	mov	sl, r3
    ca84:	e2833001 	add	r3, r3, #1	; 0x1
    ca88:	cafffff9 	bgt	ca74 <vsprintf+0x268>
    ca8c:	e5963000 	ldr	r3, [r6]
    ca90:	e1a0200a 	mov	r2, sl
    ca94:	e4c23001 	strb	r3, [r2], #1
    ca98:	e3a03020 	mov	r3, #32	; 0x20
    ca9c:	ea000000 	b	caa4 <vsprintf+0x298>
    caa0:	e5423001 	strb	r3, [r2, #-1]
    caa4:	e2499001 	sub	r9, r9, #1	; 0x1
    caa8:	e3590000 	cmp	r9, #0	; 0x0
    caac:	e1a0a002 	mov	sl, r2
    cab0:	e2822001 	add	r2, r2, #1	; 0x1
    cab4:	cafffff9 	bgt	caa0 <vsprintf+0x294>
    cab8:	ea0000ab 	b	cd6c <vsprintf+0x560>
    cabc:	e58d7000 	str	r7, [sp]
    cac0:	e1a0000a 	mov	r0, sl
    cac4:	e5961000 	ldr	r1, [r6]
    cac8:	e1a02009 	mov	r2, r9
    cacc:	e59d302c 	ldr	r3, [sp, #44]
    cad0:	ebfffe40 	bl	c3d8 <string>
    cad4:	e1a0a000 	mov	sl, r0
    cad8:	ea0000a3 	b	cd6c <vsprintf+0x560>
    cadc:	e596b000 	ldr	fp, [r6]
    cae0:	e35b0000 	cmp	fp, #0	; 0x0
    cae4:	1a000005 	bne	cb00 <vsprintf+0x2f4>
    cae8:	e1a0000a 	mov	r0, sl
    caec:	e1a02009 	mov	r2, r9
    caf0:	e59d302c 	ldr	r3, [sp, #44]
    caf4:	e59f13f0 	ldr	r1, [pc, #1008]	; ceec <vsprintf+0x6e0>
    caf8:	e58d7000 	str	r7, [sp]
    cafc:	ea000081 	b	cd08 <vsprintf+0x4fc>
    cb00:	e5d83001 	ldrb	r3, [r8, #1]
    cb04:	e2882001 	add	r2, r8, #1	; 0x1
    cb08:	e353004d 	cmp	r3, #77	; 0x4d
    cb0c:	0a000008 	beq	cb34 <vsprintf+0x328>
    cb10:	8a000002 	bhi	cb20 <vsprintf+0x314>
    cb14:	e3530049 	cmp	r3, #73	; 0x49
    cb18:	1a00007c 	bne	cd10 <vsprintf+0x504>
    cb1c:	ea000025 	b	cbb8 <vsprintf+0x3ac>
    cb20:	e3530069 	cmp	r3, #105	; 0x69
    cb24:	0a000022 	beq	cbb4 <vsprintf+0x3a8>
    cb28:	e353006d 	cmp	r3, #109	; 0x6d
    cb2c:	1a000077 	bne	cd10 <vsprintf+0x504>
    cb30:	e3877040 	orr	r7, r7, #64	; 0x40
    cb34:	e1a03327 	lsr	r3, r7, #6
    cb38:	e2233001 	eor	r3, r3, #1	; 0x1
    cb3c:	e2034001 	and	r4, r3, #1	; 0x1
    cb40:	e3a0e000 	mov	lr, #0	; 0x0
    cb44:	e28dc05b 	add	ip, sp, #91	; 0x5b
    cb48:	e3a0503a 	mov	r5, #58	; 0x3a
    cb4c:	e7db200e 	ldrb	r2, [fp, lr]
    cb50:	e59f0398 	ldr	r0, [pc, #920]	; cef0 <vsprintf+0x6e4>
    cb54:	e202300f 	and	r3, r2, #15	; 0xf
    cb58:	e7d02222 	ldrb	r2, [r0, r2, lsr #4]
    cb5c:	e7d03003 	ldrb	r3, [r0, r3]
    cb60:	e1a0000c 	mov	r0, ip
    cb64:	e4c02001 	strb	r2, [r0], #1
    cb68:	e35e0005 	cmp	lr, #5	; 0x5
    cb6c:	03a01000 	moveq	r1, #0	; 0x0
    cb70:	12041001 	andne	r1, r4, #1	; 0x1
    cb74:	e3510000 	cmp	r1, #0	; 0x0
    cb78:	e5cc3001 	strb	r3, [ip, #1]
    cb7c:	e28ee001 	add	lr, lr, #1	; 0x1
    cb80:	e280c001 	add	ip, r0, #1	; 0x1
    cb84:	128cc001 	addne	ip, ip, #1	; 0x1
    cb88:	15c05001 	strbne	r5, [r0, #1]
    cb8c:	e35e0006 	cmp	lr, #6	; 0x6
    cb90:	1affffed 	bne	cb4c <vsprintf+0x340>
    cb94:	e3a03000 	mov	r3, #0	; 0x0
    cb98:	e5cc3000 	strb	r3, [ip]
    cb9c:	e1a0000a 	mov	r0, sl
    cba0:	e3c7c040 	bic	ip, r7, #64	; 0x40
    cba4:	e1a02009 	mov	r2, r9
    cba8:	e59d302c 	ldr	r3, [sp, #44]
    cbac:	e28d105b 	add	r1, sp, #91	; 0x5b
    cbb0:	ea000053 	b	cd04 <vsprintf+0x4f8>
    cbb4:	e3877040 	orr	r7, r7, #64	; 0x40
    cbb8:	e5d23001 	ldrb	r3, [r2, #1]
    cbbc:	e3530036 	cmp	r3, #54	; 0x36
    cbc0:	1a00002d 	bne	cc7c <vsprintf+0x470>
    cbc4:	e1a03327 	lsr	r3, r7, #6
    cbc8:	e2233001 	eor	r3, r3, #1	; 0x1
    cbcc:	e1a0500b 	mov	r5, fp
    cbd0:	e3a04000 	mov	r4, #0	; 0x0
    cbd4:	e203b001 	and	fp, r3, #1	; 0x1
    cbd8:	e28dc033 	add	ip, sp, #51	; 0x33
    cbdc:	e5d52000 	ldrb	r2, [r5]
    cbe0:	e59fe308 	ldr	lr, [pc, #776]	; cef0 <vsprintf+0x6e4>
    cbe4:	e202300f 	and	r3, r2, #15	; 0xf
    cbe8:	e7de0003 	ldrb	r0, [lr, r3]
    cbec:	e7de3222 	ldrb	r3, [lr, r2, lsr #4]
    cbf0:	e1a0100c 	mov	r1, ip
    cbf4:	e4c13001 	strb	r3, [r1], #1
    cbf8:	e5cc0001 	strb	r0, [ip, #1]
    cbfc:	e5d52001 	ldrb	r2, [r5, #1]
    cc00:	e59f02e8 	ldr	r0, [pc, #744]	; cef0 <vsprintf+0x6e4>
    cc04:	e202300f 	and	r3, r2, #15	; 0xf
    cc08:	e7d0c003 	ldrb	ip, [r0, r3]
    cc0c:	e59f32dc 	ldr	r3, [pc, #732]	; cef0 <vsprintf+0x6e4>
    cc10:	e3540007 	cmp	r4, #7	; 0x7
    cc14:	03a0e000 	moveq	lr, #0	; 0x0
    cc18:	120be001 	andne	lr, fp, #1	; 0x1
    cc1c:	e7d32222 	ldrb	r2, [r3, r2, lsr #4]
    cc20:	e35e0000 	cmp	lr, #0	; 0x0
    cc24:	e58d2014 	str	r2, [sp, #20]
    cc28:	e2810001 	add	r0, r1, #1	; 0x1
    cc2c:	e59de014 	ldr	lr, [sp, #20]
    cc30:	e2802001 	add	r2, r0, #1	; 0x1
    cc34:	e5c1e001 	strb	lr, [r1, #1]
    cc38:	e2844001 	add	r4, r4, #1	; 0x1
    cc3c:	e5c0c001 	strb	ip, [r0, #1]
    cc40:	13a0003a 	movne	r0, #58	; 0x3a
    cc44:	e282c001 	add	ip, r2, #1	; 0x1
    cc48:	128cc001 	addne	ip, ip, #1	; 0x1
    cc4c:	15c20001 	strbne	r0, [r2, #1]
    cc50:	e3540008 	cmp	r4, #8	; 0x8
    cc54:	e2855002 	add	r5, r5, #2	; 0x2
    cc58:	1affffdf 	bne	cbdc <vsprintf+0x3d0>
    cc5c:	e3a03000 	mov	r3, #0	; 0x0
    cc60:	e5cc3000 	strb	r3, [ip]
    cc64:	e1a0000a 	mov	r0, sl
    cc68:	e3c7c040 	bic	ip, r7, #64	; 0x40
    cc6c:	e1a02009 	mov	r2, r9
    cc70:	e59d302c 	ldr	r3, [sp, #44]
    cc74:	e28d1033 	add	r1, sp, #51	; 0x33
    cc78:	ea000021 	b	cd04 <vsprintf+0x4f8>
    cc7c:	e3530034 	cmp	r3, #52	; 0x34
    cc80:	13c77040 	bicne	r7, r7, #64	; 0x40
    cc84:	1a000021 	bne	cd10 <vsprintf+0x504>
    cc88:	e3a05000 	mov	r5, #0	; 0x0
    cc8c:	e28d406d 	add	r4, sp, #109	; 0x6d
    cc90:	e7db1005 	ldrb	r1, [fp, r5]
    cc94:	e28d007d 	add	r0, sp, #125	; 0x7d
    cc98:	ebfffd9b 	bl	c30c <put_dec_trunc>
    cc9c:	e59d101c 	ldr	r1, [sp, #28]
    cca0:	e2402001 	sub	r2, r0, #1	; 0x1
    cca4:	e0610000 	rsb	r0, r1, r0
    cca8:	e3a01000 	mov	r1, #0	; 0x0
    ccac:	ea000002 	b	ccbc <vsprintf+0x4b0>
    ccb0:	e5d23001 	ldrb	r3, [r2, #1]
    ccb4:	e7c43001 	strb	r3, [r4, r1]
    ccb8:	e2811001 	add	r1, r1, #1	; 0x1
    ccbc:	e59d3018 	ldr	r3, [sp, #24]
    ccc0:	e2422001 	sub	r2, r2, #1	; 0x1
    ccc4:	e1520003 	cmp	r2, r3
    ccc8:	1afffff8 	bne	ccb0 <vsprintf+0x4a4>
    cccc:	e3550003 	cmp	r5, #3	; 0x3
    ccd0:	13a0e02e 	movne	lr, #46	; 0x2e
    ccd4:	e0844000 	add	r4, r4, r0
    ccd8:	e2855001 	add	r5, r5, #1	; 0x1
    ccdc:	14c4e001 	strbne	lr, [r4], #1
    cce0:	e3550004 	cmp	r5, #4	; 0x4
    cce4:	1affffe9 	bne	cc90 <vsprintf+0x484>
    cce8:	e3a03000 	mov	r3, #0	; 0x0
    ccec:	e5c43000 	strb	r3, [r4]
    ccf0:	e59d302c 	ldr	r3, [sp, #44]
    ccf4:	e3c7c040 	bic	ip, r7, #64	; 0x40
    ccf8:	e1a0000a 	mov	r0, sl
    ccfc:	e1a02009 	mov	r2, r9
    cd00:	e28d106d 	add	r1, sp, #109	; 0x6d
    cd04:	e58dc000 	str	ip, [sp]
    cd08:	ebfffdb2 	bl	c3d8 <string>
    cd0c:	ea00000d 	b	cd48 <vsprintf+0x53c>
    cd10:	e3a01010 	mov	r1, #16	; 0x10
    cd14:	e3790001 	cmn	r9, #1	; 0x1
    cd18:	e58d1000 	str	r1, [sp]
    cd1c:	e59d102c 	ldr	r1, [sp, #44]
    cd20:	e387c020 	orr	ip, r7, #32	; 0x20
    cd24:	02899009 	addeq	r9, r9, #9	; 0x9
    cd28:	0387c021 	orreq	ip, r7, #33	; 0x21
    cd2c:	e1a0000a 	mov	r0, sl
    cd30:	e1a0200b 	mov	r2, fp
    cd34:	e3a03000 	mov	r3, #0	; 0x0
    cd38:	e58d9004 	str	r9, [sp, #4]
    cd3c:	e58d1008 	str	r1, [sp, #8]
    cd40:	e58dc00c 	str	ip, [sp, #12]
    cd44:	ebfffe1a 	bl	c5b4 <number>
    cd48:	e1a0a000 	mov	sl, r0
    cd4c:	e1a02008 	mov	r2, r8
    cd50:	e5d23001 	ldrb	r3, [r2, #1]
    cd54:	e59fe198 	ldr	lr, [pc, #408]	; cef4 <vsprintf+0x6e8>
    cd58:	e1a08002 	mov	r8, r2
    cd5c:	e7de3003 	ldrb	r3, [lr, r3]
    cd60:	e2822001 	add	r2, r2, #1	; 0x1
    cd64:	e3130007 	tst	r3, #7	; 0x7
    cd68:	1afffff8 	bne	cd50 <vsprintf+0x544>
    cd6c:	e2866004 	add	r6, r6, #4	; 0x4
    cd70:	ea000054 	b	cec8 <vsprintf+0x6bc>
    cd74:	e59d0028 	ldr	r0, [sp, #40]
    cd78:	e5963000 	ldr	r3, [r6]
    cd7c:	e2861004 	add	r1, r6, #4	; 0x4
    cd80:	e060200a 	rsb	r2, r0, sl
    cd84:	e1a06001 	mov	r6, r1
    cd88:	e5832000 	str	r2, [r3]
    cd8c:	ea00004d 	b	cec8 <vsprintf+0x6bc>
    cd90:	e3a0c008 	mov	ip, #8	; 0x8
    cd94:	ea00000d 	b	cdd0 <vsprintf+0x5c4>
    cd98:	e3877020 	orr	r7, r7, #32	; 0x20
    cd9c:	e3a0c010 	mov	ip, #16	; 0x10
    cda0:	ea00000a 	b	cdd0 <vsprintf+0x5c4>
    cda4:	e3877002 	orr	r7, r7, #2	; 0x2
    cda8:	ea000007 	b	cdcc <vsprintf+0x5c0>
    cdac:	e3a03025 	mov	r3, #37	; 0x25
    cdb0:	e4ca3001 	strb	r3, [sl], #1
    cdb4:	e5d83000 	ldrb	r3, [r8]
    cdb8:	e3530000 	cmp	r3, #0	; 0x0
    cdbc:	02488001 	subeq	r8, r8, #1	; 0x1
    cdc0:	0a000040 	beq	cec8 <vsprintf+0x6bc>
    cdc4:	e4ca3001 	strb	r3, [sl], #1
    cdc8:	ea00003e 	b	cec8 <vsprintf+0x6bc>
    cdcc:	e3a0c00a 	mov	ip, #10	; 0xa
    cdd0:	e352004c 	cmp	r2, #76	; 0x4c
    cdd4:	1a000009 	bne	ce00 <vsprintf+0x5f4>
    cdd8:	e59d1024 	ldr	r1, [sp, #36]
    cddc:	e3a02000 	mov	r2, #0	; 0x0
    cde0:	e0613006 	rsb	r3, r1, r6
    cde4:	e3130007 	tst	r3, #7	; 0x7
    cde8:	02866004 	addeq	r6, r6, #4	; 0x4
    cdec:	e896000a 	ldm	r6, {r1, r3}
    cdf0:	e1a05003 	mov	r5, r3
    cdf4:	e1824001 	orr	r4, r2, r1
    cdf8:	e2866008 	add	r6, r6, #8	; 0x8
    cdfc:	ea000028 	b	cea4 <vsprintf+0x698>
    ce00:	e352006c 	cmp	r2, #108	; 0x6c
    ce04:	1a000006 	bne	ce24 <vsprintf+0x618>
    ce08:	e5963000 	ldr	r3, [r6]
    ce0c:	e3170002 	tst	r7, #2	; 0x2
    ce10:	e1a04003 	mov	r4, r3
    ce14:	e3a05000 	mov	r5, #0	; 0x0
    ce18:	e2866004 	add	r6, r6, #4	; 0x4
    ce1c:	0a000020 	beq	cea4 <vsprintf+0x698>
    ce20:	ea00001e 	b	cea0 <vsprintf+0x694>
    ce24:	e352007a 	cmp	r2, #122	; 0x7a
    ce28:	1352005a 	cmpne	r2, #90	; 0x5a
    ce2c:	05963000 	ldreq	r3, [r6]
    ce30:	02866004 	addeq	r6, r6, #4	; 0x4
    ce34:	01a04003 	moveq	r4, r3
    ce38:	03a05000 	moveq	r5, #0	; 0x0
    ce3c:	0a000018 	beq	cea4 <vsprintf+0x698>
    ce40:	e3520074 	cmp	r2, #116	; 0x74
    ce44:	05963000 	ldreq	r3, [r6]
    ce48:	02866004 	addeq	r6, r6, #4	; 0x4
    ce4c:	0a00000b 	beq	ce80 <vsprintf+0x674>
    ce50:	e3520068 	cmp	r2, #104	; 0x68
    ce54:	e2861004 	add	r1, r6, #4	; 0x4
    ce58:	e5962000 	ldr	r2, [r6]
    ce5c:	e2070002 	and	r0, r7, #2	; 0x2
    ce60:	1a000009 	bne	ce8c <vsprintf+0x680>
    ce64:	e6ff3072 	uxth	r3, r2
    ce68:	e3500000 	cmp	r0, #0	; 0x0
    ce6c:	e1a06001 	mov	r6, r1
    ce70:	e1a04003 	mov	r4, r3
    ce74:	e3a05000 	mov	r5, #0	; 0x0
    ce78:	0a000009 	beq	cea4 <vsprintf+0x698>
    ce7c:	e6bf3074 	sxth	r3, r4
    ce80:	e1a04003 	mov	r4, r3
    ce84:	e1a05fc4 	asr	r5, r4, #31
    ce88:	ea000005 	b	cea4 <vsprintf+0x698>
    ce8c:	e3500000 	cmp	r0, #0	; 0x0
    ce90:	e1a06001 	mov	r6, r1
    ce94:	e1a04002 	mov	r4, r2
    ce98:	e3a05000 	mov	r5, #0	; 0x0
    ce9c:	0a000000 	beq	cea4 <vsprintf+0x698>
    cea0:	e1a05fc4 	asr	r5, r4, #31
    cea4:	e59de02c 	ldr	lr, [sp, #44]
    cea8:	e1a0000a 	mov	r0, sl
    ceac:	e1a02004 	mov	r2, r4
    ceb0:	e1a03005 	mov	r3, r5
    ceb4:	e58dc000 	str	ip, [sp]
    ceb8:	e98d4200 	stmib	sp, {r9, lr}
    cebc:	e58d700c 	str	r7, [sp, #12]
    cec0:	ebfffdbb 	bl	c5b4 <number>
    cec4:	e1a0a000 	mov	sl, r0
    cec8:	e2888001 	add	r8, r8, #1	; 0x1
    cecc:	e5d83000 	ldrb	r3, [r8]
    ced0:	e3530000 	cmp	r3, #0	; 0x0
    ced4:	1afffe58 	bne	c83c <vsprintf+0x30>
    ced8:	e5ca3000 	strb	r3, [sl]
    cedc:	e59d1028 	ldr	r1, [sp, #40]
    cee0:	e061000a 	rsb	r0, r1, sl
    cee4:	e28dd084 	add	sp, sp, #132	; 0x84
    cee8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ceec:	00012457 	.word	0x00012457
    cef0:	00010810 	.word	0x00010810
    cef4:	00010831 	.word	0x00010831

0000cef8 <sprintf>:
    cef8:	e92d000e 	push	{r1, r2, r3}
    cefc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    cf00:	e59d1004 	ldr	r1, [sp, #4]
    cf04:	e28d2008 	add	r2, sp, #8	; 0x8
    cf08:	ebfffe3f 	bl	c80c <vsprintf>
    cf0c:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    cf10:	e28dd00c 	add	sp, sp, #12	; 0xc
    cf14:	e12fff1e 	bx	lr

0000cf18 <ius_check_header>:
    cf18:	e92d4070 	push	{r4, r5, r6, lr}
    cf1c:	e5901000 	ldr	r1, [r0]
    cf20:	e59f30e8 	ldr	r3, [pc, #232]	; d010 <ius_check_header+0xf8>
    cf24:	e24dd008 	sub	sp, sp, #8	; 0x8
    cf28:	e1510003 	cmp	r1, r3
    cf2c:	e1a04000 	mov	r4, r0
    cf30:	0a000005 	beq	cf4c <ius_check_header+0x34>
    cf34:	e59f00d8 	ldr	r0, [pc, #216]	; d014 <ius_check_header+0xfc>
    cf38:	e59f30d8 	ldr	r3, [pc, #216]	; d018 <ius_check_header+0x100>
    cf3c:	e1a0e00f 	mov	lr, pc
    cf40:	e593f000 	ldr	pc, [r3]
    cf44:	e3e00000 	mvn	r0, #0	; 0x0
    cf48:	ea00002e 	b	d008 <ius_check_header+0xf0>
    cf4c:	ebffe47a 	bl	613c <cdbg_verify_burn>
    cf50:	e3500000 	cmp	r0, #0	; 0x0
    cf54:	0a000011 	beq	cfa0 <ius_check_header+0x88>
    cf58:	e5943004 	ldr	r3, [r4, #4]
    cf5c:	e3130002 	tst	r3, #2	; 0x2
    cf60:	1a000005 	bne	cf7c <ius_check_header+0x64>
    cf64:	e59f00b0 	ldr	r0, [pc, #176]	; d01c <ius_check_header+0x104>
    cf68:	e59f30a8 	ldr	r3, [pc, #168]	; d018 <ius_check_header+0x100>
    cf6c:	e1a0e00f 	mov	lr, pc
    cf70:	e593f000 	ldr	pc, [r3]
    cf74:	e3e00021 	mvn	r0, #33	; 0x21
    cf78:	ea000022 	b	d008 <ius_check_header+0xf0>
    cf7c:	ebffe472 	bl	614c <cdbg_verify_burn_key>
    cf80:	e59f2098 	ldr	r2, [pc, #152]	; d020 <ius_check_header+0x108>
    cf84:	e1a01000 	mov	r1, r0
    cf88:	e3a00000 	mov	r0, #0	; 0x0
    cf8c:	ebffe0fe 	bl	538c <ss_aes_init>
    cf90:	e2840010 	add	r0, r4, #16	; 0x10
    cf94:	e1a01000 	mov	r1, r0
    cf98:	e3a02e1f 	mov	r2, #496	; 0x1f0
    cf9c:	ebffe0e9 	bl	5348 <ss_aes_data>
    cfa0:	e3a06000 	mov	r6, #0	; 0x0
    cfa4:	e5945018 	ldr	r5, [r4, #24]
    cfa8:	e3a01c02 	mov	r1, #512	; 0x200
    cfac:	e5846018 	str	r6, [r4, #24]
    cfb0:	e1a00006 	mov	r0, r6
    cfb4:	ebffe212 	bl	5804 <hash_init>
    cfb8:	e3a01c02 	mov	r1, #512	; 0x200
    cfbc:	e1a00004 	mov	r0, r4
    cfc0:	ebffe1ea 	bl	5770 <hash_data>
    cfc4:	e28d0004 	add	r0, sp, #4	; 0x4
    cfc8:	ebffe1cc 	bl	5700 <hash_value>
    cfcc:	e59d2004 	ldr	r2, [sp, #4]
    cfd0:	e59f3040 	ldr	r3, [pc, #64]	; d018 <ius_check_header+0x100>
    cfd4:	e1550002 	cmp	r5, r2
    cfd8:	e5845018 	str	r5, [r4, #24]
    cfdc:	0a000005 	beq	cff8 <ius_check_header+0xe0>
    cfe0:	e59f003c 	ldr	r0, [pc, #60]	; d024 <ius_check_header+0x10c>
    cfe4:	e1a01005 	mov	r1, r5
    cfe8:	e1a0e00f 	mov	lr, pc
    cfec:	e593f000 	ldr	pc, [r3]
    cff0:	e3e0001f 	mvn	r0, #31	; 0x1f
    cff4:	ea000003 	b	d008 <ius_check_header+0xf0>
    cff8:	e59f0028 	ldr	r0, [pc, #40]	; d028 <ius_check_header+0x110>
    cffc:	e1a0e00f 	mov	lr, pc
    d000:	e593f000 	ldr	pc, [r3]
    d004:	e1a00006 	mov	r0, r6
    d008:	e28dd008 	add	sp, sp, #8	; 0x8
    d00c:	e8bd8070 	pop	{r4, r5, r6, pc}
    d010:	7375692b 	.word	0x7375692b
    d014:	0001245e 	.word	0x0001245e
    d018:	0800d704 	.word	0x0800d704
    d01c:	00012478 	.word	0x00012478
    d020:	0800d0bc 	.word	0x0800d0bc
    d024:	00012487 	.word	0x00012487
    d028:	000124af 	.word	0x000124af

0000d02c <printf_off>:
    d02c:	e92d000f 	push	{r0, r1, r2, r3}
    d030:	e28dd010 	add	sp, sp, #16	; 0x10
    d034:	e12fff1e 	bx	lr

0000d038 <disable_ctrlc>:
    d038:	e59f300c 	ldr	r3, [pc, #12]	; d04c <disable_ctrlc+0x14>
    d03c:	e5932000 	ldr	r2, [r3]
    d040:	e5830000 	str	r0, [r3]
    d044:	e1a00002 	mov	r0, r2
    d048:	e12fff1e 	bx	lr
    d04c:	0800d2a4 	.word	0x0800d2a4

0000d050 <had_ctrlc>:
    d050:	e59f3004 	ldr	r3, [pc, #4]	; d05c <had_ctrlc+0xc>
    d054:	e5930000 	ldr	r0, [r3]
    d058:	e12fff1e 	bx	lr
    d05c:	0800d2a0 	.word	0x0800d2a0

0000d060 <clear_ctrlc>:
    d060:	e59f3008 	ldr	r3, [pc, #8]	; d070 <clear_ctrlc+0x10>
    d064:	e3a02000 	mov	r2, #0	; 0x0
    d068:	e5832000 	str	r2, [r3]
    d06c:	e12fff1e 	bx	lr
    d070:	0800d2a0 	.word	0x0800d2a0

0000d074 <dbg>:
    d074:	e92d000f 	push	{r0, r1, r2, r3}
    d078:	e28dd010 	add	sp, sp, #16	; 0x10
    d07c:	e12fff1e 	bx	lr

0000d080 <puts>:
    d080:	eaffeded 	b	883c <serial_puts>

0000d084 <vprintf>:
    d084:	e92d4010 	push	{r4, lr}
    d088:	e24ddd06 	sub	sp, sp, #384	; 0x180
    d08c:	e1a02001 	mov	r2, r1
    d090:	e1a01000 	mov	r1, r0
    d094:	e1a0000d 	mov	r0, sp
    d098:	ebfffddb 	bl	c80c <vsprintf>
    d09c:	e1a0000d 	mov	r0, sp
    d0a0:	e1a0400d 	mov	r4, sp
    d0a4:	ebffede4 	bl	883c <serial_puts>
    d0a8:	e28ddd06 	add	sp, sp, #384	; 0x180
    d0ac:	e8bd8010 	pop	{r4, pc}

0000d0b0 <printf_on>:
    d0b0:	e92d000f 	push	{r0, r1, r2, r3}
    d0b4:	e92d4010 	push	{r4, lr}
    d0b8:	e24ddd06 	sub	sp, sp, #384	; 0x180
    d0bc:	e1a0000d 	mov	r0, sp
    d0c0:	e59d1188 	ldr	r1, [sp, #392]
    d0c4:	e28d2f63 	add	r2, sp, #396	; 0x18c
    d0c8:	ebfffdcf 	bl	c80c <vsprintf>
    d0cc:	e1a0000d 	mov	r0, sp
    d0d0:	e1a0400d 	mov	r4, sp
    d0d4:	ebffedd8 	bl	883c <serial_puts>
    d0d8:	e28ddd06 	add	sp, sp, #384	; 0x180
    d0dc:	e8bd4010 	pop	{r4, lr}
    d0e0:	e28dd010 	add	sp, sp, #16	; 0x10
    d0e4:	e12fff1e 	bx	lr

0000d0e8 <serial_printf>:
    d0e8:	e92d000f 	push	{r0, r1, r2, r3}
    d0ec:	e92d4010 	push	{r4, lr}
    d0f0:	e24ddd06 	sub	sp, sp, #384	; 0x180
    d0f4:	e1a0000d 	mov	r0, sp
    d0f8:	e59d1188 	ldr	r1, [sp, #392]
    d0fc:	e28d2f63 	add	r2, sp, #396	; 0x18c
    d100:	ebfffdc1 	bl	c80c <vsprintf>
    d104:	e1a0000d 	mov	r0, sp
    d108:	e1a0400d 	mov	r4, sp
    d10c:	ebffedca 	bl	883c <serial_puts>
    d110:	e28ddd06 	add	sp, sp, #384	; 0x180
    d114:	e8bd4010 	pop	{r4, lr}
    d118:	e28dd010 	add	sp, sp, #16	; 0x10
    d11c:	e12fff1e 	bx	lr

0000d120 <putc>:
    d120:	eaffedae 	b	87e0 <serial_putc>

0000d124 <tstc>:
    d124:	eaffeda6 	b	87c4 <serial_tstc>

0000d128 <getc>:
    d128:	eaffedd4 	b	8880 <serial_getc>

0000d12c <ctrlc>:
    d12c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    d130:	e59f3040 	ldr	r3, [pc, #64]	; d178 <ctrlc+0x4c>
    d134:	e24dd004 	sub	sp, sp, #4	; 0x4
    d138:	e5933000 	ldr	r3, [r3]
    d13c:	e3530000 	cmp	r3, #0	; 0x0
    d140:	1a000009 	bne	d16c <ctrlc+0x40>
    d144:	ebffed9e 	bl	87c4 <serial_tstc>
    d148:	e3500000 	cmp	r0, #0	; 0x0
    d14c:	0a000006 	beq	d16c <ctrlc+0x40>
    d150:	ebffedca 	bl	8880 <serial_getc>
    d154:	e3500003 	cmp	r0, #3	; 0x3
    d158:	059f301c 	ldreq	r3, [pc, #28]	; d17c <ctrlc+0x50>
    d15c:	03a02001 	moveq	r2, #1	; 0x1
    d160:	05832000 	streq	r2, [r3]
    d164:	01a00002 	moveq	r0, r2
    d168:	0a000000 	beq	d170 <ctrlc+0x44>
    d16c:	e3a00000 	mov	r0, #0	; 0x0
    d170:	e28dd004 	add	sp, sp, #4	; 0x4
    d174:	e8bd8000 	pop	{pc}
    d178:	0800d2a4 	.word	0x0800d2a4
    d17c:	0800d2a0 	.word	0x0800d2a0

0000d180 <parse_line>:
    d180:	e92d4010 	push	{r4, lr}
    d184:	e3a04000 	mov	r4, #0	; 0x0
    d188:	e1a02004 	mov	r2, r4
    d18c:	ea000000 	b	d194 <parse_line+0x14>
    d190:	e2800001 	add	r0, r0, #1	; 0x1
    d194:	e5d03000 	ldrb	r3, [r0]
    d198:	e3530009 	cmp	r3, #9	; 0x9
    d19c:	13530020 	cmpne	r3, #32	; 0x20
    d1a0:	0afffffa 	beq	d190 <parse_line+0x10>
    d1a4:	e3530000 	cmp	r3, #0	; 0x0
    d1a8:	0a00000c 	beq	d1e0 <parse_line+0x60>
    d1ac:	e7810104 	str	r0, [r1, r4, lsl #2]
    d1b0:	e2844001 	add	r4, r4, #1	; 0x1
    d1b4:	ea000000 	b	d1bc <parse_line+0x3c>
    d1b8:	e2800001 	add	r0, r0, #1	; 0x1
    d1bc:	e5d03000 	ldrb	r3, [r0]
    d1c0:	e3530000 	cmp	r3, #0	; 0x0
    d1c4:	13530020 	cmpne	r3, #32	; 0x20
    d1c8:	0a000002 	beq	d1d8 <parse_line+0x58>
    d1cc:	e3530009 	cmp	r3, #9	; 0x9
    d1d0:	1afffff8 	bne	d1b8 <parse_line+0x38>
    d1d4:	ea000003 	b	d1e8 <parse_line+0x68>
    d1d8:	e3530000 	cmp	r3, #0	; 0x0
    d1dc:	1a000001 	bne	d1e8 <parse_line+0x68>
    d1e0:	e7813104 	str	r3, [r1, r4, lsl #2]
    d1e4:	ea000007 	b	d208 <parse_line+0x88>
    d1e8:	e3540010 	cmp	r4, #16	; 0x10
    d1ec:	e5c02000 	strb	r2, [r0]
    d1f0:	1affffe6 	bne	d190 <parse_line+0x10>
    d1f4:	e59f0014 	ldr	r0, [pc, #20]	; d210 <parse_line+0x90>
    d1f8:	e1a01004 	mov	r1, r4
    d1fc:	e59f3010 	ldr	r3, [pc, #16]	; d214 <parse_line+0x94>
    d200:	e1a0e00f 	mov	lr, pc
    d204:	e593f000 	ldr	pc, [r3]
    d208:	e1a00004 	mov	r0, r4
    d20c:	e8bd8010 	pop	{r4, pc}
    d210:	000124c5 	.word	0x000124c5
    d214:	0800d704 	.word	0x0800d704

0000d218 <run_command>:
    d218:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d21c:	e1a05000 	mov	r5, r0
    d220:	e24ddf93 	sub	sp, sp, #588	; 0x24c
    d224:	e1a0a001 	mov	sl, r1
    d228:	ebffff8c 	bl	d060 <clear_ctrlc>
    d22c:	e3550000 	cmp	r5, #0	; 0x0
    d230:	0a0000ac 	beq	d4e8 <run_command+0x2d0>
    d234:	e5d53000 	ldrb	r3, [r5]
    d238:	e3530000 	cmp	r3, #0	; 0x0
    d23c:	0a0000a9 	beq	d4e8 <run_command+0x2d0>
    d240:	e1a00005 	mov	r0, r5
    d244:	ebffd049 	bl	1370 <strlen>
    d248:	e35000ff 	cmp	r0, #255	; 0xff
    d24c:	9a000002 	bls	d25c <run_command+0x44>
    d250:	e59f02a0 	ldr	r0, [pc, #672]	; d4f8 <run_command+0x2e0>
    d254:	ebffff89 	bl	d080 <puts>
    d258:	ea0000a2 	b	d4e8 <run_command+0x2d0>
    d25c:	e28d4f41 	add	r4, sp, #260	; 0x104
    d260:	e1a01005 	mov	r1, r5
    d264:	e1a00004 	mov	r0, r4
    d268:	e3a06000 	mov	r6, #0	; 0x0
    d26c:	ebffcfe8 	bl	1214 <strcpy>
    d270:	e3a07001 	mov	r7, #1	; 0x1
    d274:	e1a0b006 	mov	fp, r6
    d278:	e28d9f81 	add	r9, sp, #516	; 0x204
    d27c:	ea000091 	b	d4c8 <run_command+0x2b0>
    d280:	e3510027 	cmp	r1, #39	; 0x27
    d284:	1a000002 	bne	d294 <run_command+0x7c>
    d288:	e5523001 	ldrb	r3, [r2, #-1]
    d28c:	e353005c 	cmp	r3, #92	; 0x5c
    d290:	12200001 	eorne	r0, r0, #1	; 0x1
    d294:	e3500000 	cmp	r0, #0	; 0x0
    d298:	1a000009 	bne	d2c4 <run_command+0xac>
    d29c:	e351003b 	cmp	r1, #59	; 0x3b
    d2a0:	13a03000 	movne	r3, #0	; 0x0
    d2a4:	03a03001 	moveq	r3, #1	; 0x1
    d2a8:	e1520004 	cmp	r2, r4
    d2ac:	03a03000 	moveq	r3, #0	; 0x0
    d2b0:	e3530000 	cmp	r3, #0	; 0x0
    d2b4:	0a000002 	beq	d2c4 <run_command+0xac>
    d2b8:	e5523001 	ldrb	r3, [r2, #-1]
    d2bc:	e353005c 	cmp	r3, #92	; 0x5c
    d2c0:	1a000005 	bne	d2dc <run_command+0xc4>
    d2c4:	e2822001 	add	r2, r2, #1	; 0x1
    d2c8:	e5d21000 	ldrb	r1, [r2]
    d2cc:	e3510000 	cmp	r1, #0	; 0x0
    d2d0:	1affffea 	bne	d280 <run_command+0x68>
    d2d4:	e1a08002 	mov	r8, r2
    d2d8:	ea000001 	b	d2e4 <run_command+0xcc>
    d2dc:	e5c20000 	strb	r0, [r2]
    d2e0:	e2828001 	add	r8, r2, #1	; 0x1
    d2e4:	e1a00004 	mov	r0, r4
    d2e8:	ebffd020 	bl	1370 <strlen>
    d2ec:	e3a0c000 	mov	ip, #0	; 0x0
    d2f0:	e3a0ec01 	mov	lr, #256	; 0x100
    d2f4:	e1a0100c 	mov	r1, ip
    d2f8:	e28d5004 	add	r5, sp, #4	; 0x4
    d2fc:	ea000049 	b	d428 <run_command+0x210>
    d300:	e35c0003 	cmp	ip, #3	; 0x3
    d304:	e2400001 	sub	r0, r0, #1	; 0x1
    d308:	e4d42001 	ldrb	r2, [r4], #1
    d30c:	0a000035 	beq	d3e8 <run_command+0x1d0>
    d310:	e352005c 	cmp	r2, #92	; 0x5c
    d314:	13a03000 	movne	r3, #0	; 0x0
    d318:	03a03001 	moveq	r3, #1	; 0x1
    d31c:	e351005c 	cmp	r1, #92	; 0x5c
    d320:	03a03000 	moveq	r3, #0	; 0x0
    d324:	e3530000 	cmp	r3, #0	; 0x0
    d328:	0a000004 	beq	d340 <run_command+0x128>
    d32c:	e3500000 	cmp	r0, #0	; 0x0
    d330:	0a00003f 	beq	d434 <run_command+0x21c>
    d334:	e4d42001 	ldrb	r2, [r4], #1
    d338:	e2400001 	sub	r0, r0, #1	; 0x1
    d33c:	e3a0105c 	mov	r1, #92	; 0x5c
    d340:	e35c0003 	cmp	ip, #3	; 0x3
    d344:	979ff10c 	ldrls	pc, [pc, ip, lsl #2]
    d348:	ea000035 	b	d424 <run_command+0x20c>
    d34c:	0000d35c 	.word	0x0000d35c
    d350:	0000d3a0 	.word	0x0000d3a0
    d354:	0000d3d8 	.word	0x0000d3d8
    d358:	0000d3e8 	.word	0x0000d3e8
    d35c:	e251105c 	subs	r1, r1, #92	; 0x5c
    d360:	13a01001 	movne	r1, #1	; 0x1
    d364:	e3520027 	cmp	r2, #39	; 0x27
    d368:	13a03000 	movne	r3, #0	; 0x0
    d36c:	02013001 	andeq	r3, r1, #1	; 0x1
    d370:	e3530000 	cmp	r3, #0	; 0x0
    d374:	13a0c003 	movne	ip, #3	; 0x3
    d378:	1a000022 	bne	d408 <run_command+0x1f0>
    d37c:	e3520024 	cmp	r2, #36	; 0x24
    d380:	13a03000 	movne	r3, #0	; 0x0
    d384:	02013001 	andeq	r3, r1, #1	; 0x1
    d388:	e3530000 	cmp	r3, #0	; 0x0
    d38c:	13a0c001 	movne	ip, #1	; 0x1
    d390:	13a02024 	movne	r2, #36	; 0x24
    d394:	04c52001 	strbeq	r2, [r5], #1
    d398:	024ee001 	subeq	lr, lr, #1	; 0x1
    d39c:	ea000020 	b	d424 <run_command+0x20c>
    d3a0:	e352007b 	cmp	r2, #123	; 0x7b
    d3a4:	13520028 	cmpne	r2, #40	; 0x28
    d3a8:	13a03000 	movne	r3, #0	; 0x0
    d3ac:	03a03001 	moveq	r3, #1	; 0x1
    d3b0:	03a0c002 	moveq	ip, #2	; 0x2
    d3b4:	0a00001a 	beq	d424 <run_command+0x20c>
    d3b8:	e3a01024 	mov	r1, #36	; 0x24
    d3bc:	e25ee001 	subs	lr, lr, #1	; 0x1
    d3c0:	e4c51001 	strb	r1, [r5], #1
    d3c4:	0a000015 	beq	d420 <run_command+0x208>
    d3c8:	e4c52001 	strb	r2, [r5], #1
    d3cc:	e24ee001 	sub	lr, lr, #1	; 0x1
    d3d0:	e1a0c003 	mov	ip, r3
    d3d4:	ea000012 	b	d424 <run_command+0x20c>
    d3d8:	e352007d 	cmp	r2, #125	; 0x7d
    d3dc:	13520029 	cmpne	r2, #41	; 0x29
    d3e0:	0a00000e 	beq	d420 <run_command+0x208>
    d3e4:	ea00000e 	b	d424 <run_command+0x20c>
    d3e8:	e3520027 	cmp	r2, #39	; 0x27
    d3ec:	13a03000 	movne	r3, #0	; 0x0
    d3f0:	03a03001 	moveq	r3, #1	; 0x1
    d3f4:	e351005c 	cmp	r1, #92	; 0x5c
    d3f8:	03a03000 	moveq	r3, #0	; 0x0
    d3fc:	e3530000 	cmp	r3, #0	; 0x0
    d400:	0a000002 	beq	d410 <run_command+0x1f8>
    d404:	e3a0c000 	mov	ip, #0	; 0x0
    d408:	e3a02027 	mov	r2, #39	; 0x27
    d40c:	ea000004 	b	d424 <run_command+0x20c>
    d410:	e4c52001 	strb	r2, [r5], #1
    d414:	e24ee001 	sub	lr, lr, #1	; 0x1
    d418:	e3a0c003 	mov	ip, #3	; 0x3
    d41c:	ea000000 	b	d424 <run_command+0x20c>
    d420:	e3a0c000 	mov	ip, #0	; 0x0
    d424:	e1a01002 	mov	r1, r2
    d428:	e3500000 	cmp	r0, #0	; 0x0
    d42c:	135e0000 	cmpne	lr, #0	; 0x0
    d430:	1affffb2 	bne	d300 <run_command+0xe8>
    d434:	e35e0000 	cmp	lr, #0	; 0x0
    d438:	15c5b000 	strbne	fp, [r5]
    d43c:	0545e001 	strbeq	lr, [r5, #-1]
    d440:	e28d0004 	add	r0, sp, #4	; 0x4
    d444:	e1a01009 	mov	r1, r9
    d448:	ebffff4c 	bl	d180 <parse_line>
    d44c:	e2505000 	subs	r5, r0, #0	; 0x0
    d450:	0a00000d 	beq	d48c <run_command+0x274>
    d454:	e59d0204 	ldr	r0, [sp, #516]
    d458:	ebffd673 	bl	2e2c <find_cmd>
    d45c:	e2504000 	subs	r4, r0, #0	; 0x0
    d460:	1a000005 	bne	d47c <run_command+0x264>
    d464:	e59f0090 	ldr	r0, [pc, #144]	; d4fc <run_command+0x2e4>
    d468:	e59d1204 	ldr	r1, [sp, #516]
    d46c:	e59f308c 	ldr	r3, [pc, #140]	; d500 <run_command+0x2e8>
    d470:	e1a0e00f 	mov	lr, pc
    d474:	e593f000 	ldr	pc, [r3]
    d478:	ea000003 	b	d48c <run_command+0x274>
    d47c:	e5943004 	ldr	r3, [r4, #4]
    d480:	e1550003 	cmp	r5, r3
    d484:	da000002 	ble	d494 <run_command+0x27c>
    d488:	ebffd621 	bl	2d14 <cmd_usage>
    d48c:	e3e06000 	mvn	r6, #0	; 0x0
    d490:	ea00000b 	b	d4c4 <run_command+0x2ac>
    d494:	e1a02005 	mov	r2, r5
    d498:	e1a0100a 	mov	r1, sl
    d49c:	e1a03009 	mov	r3, r9
    d4a0:	e1a0e00f 	mov	lr, pc
    d4a4:	e594f00c 	ldr	pc, [r4, #12]
    d4a8:	e3500000 	cmp	r0, #0	; 0x0
    d4ac:	13e06000 	mvnne	r6, #0	; 0x0
    d4b0:	e5944008 	ldr	r4, [r4, #8]
    d4b4:	ebfffee5 	bl	d050 <had_ctrlc>
    d4b8:	e3500000 	cmp	r0, #0	; 0x0
    d4bc:	1a000009 	bne	d4e8 <run_command+0x2d0>
    d4c0:	e0077004 	and	r7, r7, r4
    d4c4:	e1a04008 	mov	r4, r8
    d4c8:	e5d43000 	ldrb	r3, [r4]
    d4cc:	e3530000 	cmp	r3, #0	; 0x0
    d4d0:	11a02004 	movne	r2, r4
    d4d4:	13a00000 	movne	r0, #0	; 0x0
    d4d8:	1affff7a 	bne	d2c8 <run_command+0xb0>
    d4dc:	e3560000 	cmp	r6, #0	; 0x0
    d4e0:	01a06007 	moveq	r6, r7
    d4e4:	ea000000 	b	d4ec <run_command+0x2d4>
    d4e8:	e3e06000 	mvn	r6, #0	; 0x0
    d4ec:	e1a00006 	mov	r0, r6
    d4f0:	e28ddf93 	add	sp, sp, #588	; 0x24c
    d4f4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d4f8:	000124e4 	.word	0x000124e4
    d4fc:	000124fa 	.word	0x000124fa
    d500:	0800d704 	.word	0x0800d704

0000d504 <cread_add_char>:
    d504:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    d508:	e3510000 	cmp	r1, #0	; 0x0
    d50c:	e24dd004 	sub	sp, sp, #4	; 0x4
    d510:	e1a07000 	mov	r7, r0
    d514:	e1a04002 	mov	r4, r2
    d518:	e1a00003 	mov	r0, r3
    d51c:	e59d6018 	ldr	r6, [sp, #24]
    d520:	e59dc01c 	ldr	ip, [sp, #28]
    d524:	1a000003 	bne	d538 <cread_add_char+0x34>
    d528:	e5922000 	ldr	r2, [r2]
    d52c:	e5933000 	ldr	r3, [r3]
    d530:	e1520003 	cmp	r2, r3
    d534:	1a000026 	bne	d5d4 <cread_add_char+0xd0>
    d538:	e5902000 	ldr	r2, [r0]
    d53c:	e24c3001 	sub	r3, ip, #1	; 0x1
    d540:	e1520003 	cmp	r2, r3
    d544:	9a000003 	bls	d558 <cread_add_char+0x54>
    d548:	e3a00007 	mov	r0, #7	; 0x7
    d54c:	e28dd004 	add	sp, sp, #4	; 0x4
    d550:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
    d554:	eafffef1 	b	d120 <putc>
    d558:	e3510000 	cmp	r1, #0	; 0x0
    d55c:	e2821001 	add	r1, r2, #1	; 0x1
    d560:	e5801000 	str	r1, [r0]
    d564:	0a00001a 	beq	d5d4 <cread_add_char+0xd0>
    d568:	e5943000 	ldr	r3, [r4]
    d56c:	e0635001 	rsb	r5, r3, r1
    d570:	e3550001 	cmp	r5, #1	; 0x1
    d574:	9a000004 	bls	d58c <cread_add_char+0x88>
    d578:	e2830001 	add	r0, r3, #1	; 0x1
    d57c:	e0860000 	add	r0, r6, r0
    d580:	e0861003 	add	r1, r6, r3
    d584:	e2452001 	sub	r2, r5, #1	; 0x1
    d588:	ebffcf9b 	bl	13fc <memmove>
    d58c:	e5943000 	ldr	r3, [r4]
    d590:	e59f0074 	ldr	r0, [pc, #116]	; d60c <cread_add_char+0x108>
    d594:	e7c67003 	strb	r7, [r6, r3]
    d598:	e5942000 	ldr	r2, [r4]
    d59c:	e59f306c 	ldr	r3, [pc, #108]	; d610 <cread_add_char+0x10c>
    d5a0:	e0862002 	add	r2, r6, r2
    d5a4:	e1a01005 	mov	r1, r5
    d5a8:	e1a0e00f 	mov	lr, pc
    d5ac:	e593f000 	ldr	pc, [r3]
    d5b0:	e5943000 	ldr	r3, [r4]
    d5b4:	e2833001 	add	r3, r3, #1	; 0x1
    d5b8:	e5843000 	str	r3, [r4]
    d5bc:	ea000000 	b	d5c4 <cread_add_char+0xc0>
    d5c0:	ebfffed6 	bl	d120 <putc>
    d5c4:	e2555001 	subs	r5, r5, #1	; 0x1
    d5c8:	e3a00008 	mov	r0, #8	; 0x8
    d5cc:	1afffffb 	bne	d5c0 <cread_add_char+0xbc>
    d5d0:	ea00000b 	b	d604 <cread_add_char+0x100>
    d5d4:	e5943000 	ldr	r3, [r4]
    d5d8:	e59f002c 	ldr	r0, [pc, #44]	; d60c <cread_add_char+0x108>
    d5dc:	e7c67003 	strb	r7, [r6, r3]
    d5e0:	e5942000 	ldr	r2, [r4]
    d5e4:	e59f3024 	ldr	r3, [pc, #36]	; d610 <cread_add_char+0x10c>
    d5e8:	e0862002 	add	r2, r6, r2
    d5ec:	e3a01001 	mov	r1, #1	; 0x1
    d5f0:	e1a0e00f 	mov	lr, pc
    d5f4:	e593f000 	ldr	pc, [r3]
    d5f8:	e5943000 	ldr	r3, [r4]
    d5fc:	e2833001 	add	r3, r3, #1	; 0x1
    d600:	e5843000 	str	r3, [r4]
    d604:	e28dd004 	add	sp, sp, #4	; 0x4
    d608:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    d60c:	0001251d 	.word	0x0001251d
    d610:	0800d704 	.word	0x0800d704

0000d614 <readline_into_buffer>:
    d614:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d618:	e59f367c 	ldr	r3, [pc, #1660]	; dc9c <readline_into_buffer+0x688>
    d61c:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    d620:	e593e000 	ldr	lr, [r3]
    d624:	e1a06001 	mov	r6, r1
    d628:	e35e0000 	cmp	lr, #0	; 0x0
    d62c:	1a000014 	bne	d684 <readline_into_buffer+0x70>
    d630:	e59f3668 	ldr	r3, [pc, #1640]	; dca0 <readline_into_buffer+0x68c>
    d634:	e3e02000 	mvn	r2, #0	; 0x0
    d638:	e5832000 	str	r2, [r3]
    d63c:	e59f3660 	ldr	r3, [pc, #1632]	; dca4 <readline_into_buffer+0x690>
    d640:	e59fc660 	ldr	ip, [pc, #1632]	; dca8 <readline_into_buffer+0x694>
    d644:	e583e000 	str	lr, [r3]
    d648:	e59f365c 	ldr	r3, [pc, #1628]	; dcac <readline_into_buffer+0x698>
    d64c:	e59f465c 	ldr	r4, [pc, #1628]	; dcb0 <readline_into_buffer+0x69c>
    d650:	e583e000 	str	lr, [r3]
    d654:	e59f3658 	ldr	r3, [pc, #1624]	; dcb4 <readline_into_buffer+0x6a0>
    d658:	e1a0100e 	mov	r1, lr
    d65c:	e583e000 	str	lr, [r3]
    d660:	e1a0300e 	mov	r3, lr
    d664:	e784c001 	str	ip, [r4, r1]
    d668:	e2811004 	add	r1, r1, #4	; 0x4
    d66c:	e3510010 	cmp	r1, #16	; 0x10
    d670:	e4cc3100 	strb	r3, [ip], #256
    d674:	1afffffa 	bne	d664 <readline_into_buffer+0x50>
    d678:	e59f361c 	ldr	r3, [pc, #1564]	; dc9c <readline_into_buffer+0x688>
    d67c:	e3a02001 	mov	r2, #1	; 0x1
    d680:	e5832000 	str	r2, [r3]
    d684:	ebfffe7d 	bl	d080 <puts>
    d688:	e3a03000 	mov	r3, #0	; 0x0
    d68c:	e58d3014 	str	r3, [sp, #20]
    d690:	e58d3010 	str	r3, [sp, #16]
    d694:	e1a05003 	mov	r5, r3
    d698:	e3a09001 	mov	r9, #1	; 0x1
    d69c:	e59fb614 	ldr	fp, [pc, #1556]	; dcb8 <readline_into_buffer+0x6a4>
    d6a0:	e59fa5f8 	ldr	sl, [pc, #1528]	; dca0 <readline_into_buffer+0x68c>
    d6a4:	ea000000 	b	d6ac <readline_into_buffer+0x98>
    d6a8:	e3a05000 	mov	r5, #0	; 0x0
    d6ac:	ebfffe9d 	bl	d128 <getc>
    d6b0:	e6ef0070 	uxtb	r0, r0
    d6b4:	e350000d 	cmp	r0, #13	; 0xd
    d6b8:	1350000a 	cmpne	r0, #10	; 0xa
    d6bc:	13a04000 	movne	r4, #0	; 0x0
    d6c0:	03a04001 	moveq	r4, #1	; 0x1
    d6c4:	1a00000b 	bne	d6f8 <readline_into_buffer+0xe4>
    d6c8:	e59f35e8 	ldr	r3, [pc, #1512]	; dcb8 <readline_into_buffer+0x6a4>
    d6cc:	e59f05e8 	ldr	r0, [pc, #1512]	; dcbc <readline_into_buffer+0x6a8>
    d6d0:	e1a0e00f 	mov	lr, pc
    d6d4:	e593f000 	ldr	pc, [r3]
    d6d8:	e59d5010 	ldr	r5, [sp, #16]
    d6dc:	e3a03000 	mov	r3, #0	; 0x0
    d6e0:	e7c63005 	strb	r3, [r6, r5]
    d6e4:	e5d63000 	ldrb	r3, [r6]
    d6e8:	e3530000 	cmp	r3, #0	; 0x0
    d6ec:	13530021 	cmpne	r3, #33	; 0x21
    d6f0:	0a000162 	beq	dc80 <readline_into_buffer+0x66c>
    d6f4:	ea00014e 	b	dc34 <readline_into_buffer+0x620>
    d6f8:	e3550000 	cmp	r5, #0	; 0x0
    d6fc:	0a000033 	beq	d7d0 <readline_into_buffer+0x1bc>
    d700:	e3550001 	cmp	r5, #1	; 0x1
    d704:	1a00000c 	bne	d73c <readline_into_buffer+0x128>
    d708:	e350005b 	cmp	r0, #91	; 0x5b
    d70c:	05cd0009 	strbeq	r0, [sp, #9]
    d710:	03a05002 	moveq	r5, #2	; 0x2
    d714:	0affffe4 	beq	d6ac <readline_into_buffer+0x98>
    d718:	e3a0cc01 	mov	ip, #256	; 0x100
    d71c:	e5dd0008 	ldrb	r0, [sp, #8]
    d720:	e1a01009 	mov	r1, r9
    d724:	e28d2014 	add	r2, sp, #20	; 0x14
    d728:	e28d3010 	add	r3, sp, #16	; 0x10
    d72c:	e88d1040 	stm	sp, {r6, ip}
    d730:	e1a05004 	mov	r5, r4
    d734:	ebffff72 	bl	d504 <cread_add_char>
    d738:	eaffffdb 	b	d6ac <readline_into_buffer+0x98>
    d73c:	e2403041 	sub	r3, r0, #65	; 0x41
    d740:	e3530007 	cmp	r3, #7	; 0x7
    d744:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    d748:	ea000009 	b	d774 <readline_into_buffer+0x160>
    d74c:	0000dab8 	.word	0x0000dab8
    d750:	0000daf8 	.word	0x0000daf8
    d754:	0000d7c0 	.word	0x0000d7c0
    d758:	0000d8cc 	.word	0x0000d8cc
    d75c:	0000d774 	.word	0x0000d774
    d760:	0000d774 	.word	0x0000d774
    d764:	0000d774 	.word	0x0000d774
    d768:	0000d76c 	.word	0x0000d76c
    d76c:	e3a05000 	mov	r5, #0	; 0x0
    d770:	ea000043 	b	d884 <readline_into_buffer+0x270>
    d774:	e28d1018 	add	r1, sp, #24	; 0x18
    d778:	e0813005 	add	r3, r1, r5
    d77c:	e5430010 	strb	r0, [r3, #-16]
    d780:	e2855001 	add	r5, r5, #1	; 0x1
    d784:	e3a04000 	mov	r4, #0	; 0x0
    d788:	e28d8010 	add	r8, sp, #16	; 0x10
    d78c:	e3a07c01 	mov	r7, #256	; 0x100
    d790:	ea000004 	b	d7a8 <readline_into_buffer+0x194>
    d794:	e28dc008 	add	ip, sp, #8	; 0x8
    d798:	e7dc0004 	ldrb	r0, [ip, r4]
    d79c:	e88d00c0 	stm	sp, {r6, r7}
    d7a0:	ebffff57 	bl	d504 <cread_add_char>
    d7a4:	e2844001 	add	r4, r4, #1	; 0x1
    d7a8:	e1540005 	cmp	r4, r5
    d7ac:	e1a01009 	mov	r1, r9
    d7b0:	e28d2014 	add	r2, sp, #20	; 0x14
    d7b4:	e1a03008 	mov	r3, r8
    d7b8:	1afffff5 	bne	d794 <readline_into_buffer+0x180>
    d7bc:	eaffffb9 	b	d6a8 <readline_into_buffer+0x94>
    d7c0:	e3a00006 	mov	r0, #6	; 0x6
    d7c4:	e350000b 	cmp	r0, #11	; 0xb
    d7c8:	e3a05000 	mov	r5, #0	; 0x0
    d7cc:	ea000001 	b	d7d8 <readline_into_buffer+0x1c4>
    d7d0:	e350000b 	cmp	r0, #11	; 0xb
    d7d4:	0a00005f 	beq	d958 <readline_into_buffer+0x344>
    d7d8:	8a00000d 	bhi	d814 <readline_into_buffer+0x200>
    d7dc:	e3500004 	cmp	r0, #4	; 0x4
    d7e0:	0a000042 	beq	d8f0 <readline_into_buffer+0x2dc>
    d7e4:	8a000005 	bhi	d800 <readline_into_buffer+0x1ec>
    d7e8:	e3500002 	cmp	r0, #2	; 0x2
    d7ec:	0a000037 	beq	d8d0 <readline_into_buffer+0x2bc>
    d7f0:	8a000028 	bhi	d898 <readline_into_buffer+0x284>
    d7f4:	e3500001 	cmp	r0, #1	; 0x1
    d7f8:	1a000106 	bne	dc18 <readline_into_buffer+0x604>
    d7fc:	ea000020 	b	d884 <readline_into_buffer+0x270>
    d800:	e3500006 	cmp	r0, #6	; 0x6
    d804:	0a000027 	beq	d8a8 <readline_into_buffer+0x294>
    d808:	3a000066 	bcc	d9a8 <readline_into_buffer+0x394>
    d80c:	e3500008 	cmp	r0, #8	; 0x8
    d810:	ea000015 	b	d86c <readline_into_buffer+0x258>
    d814:	e3500015 	cmp	r0, #21	; 0x15
    d818:	0a000070 	beq	d9e0 <readline_into_buffer+0x3cc>
    d81c:	8a000007 	bhi	d840 <readline_into_buffer+0x22c>
    d820:	e350000f 	cmp	r0, #15	; 0xf
    d824:	02299001 	eoreq	r9, r9, #1	; 0x1
    d828:	0affff9f 	beq	d6ac <readline_into_buffer+0x98>
    d82c:	e3500010 	cmp	r0, #16	; 0x10
    d830:	0a0000a0 	beq	dab8 <readline_into_buffer+0x4a4>
    d834:	e350000e 	cmp	r0, #14	; 0xe
    d838:	1a0000f6 	bne	dc18 <readline_into_buffer+0x604>
    d83c:	ea0000ad 	b	daf8 <readline_into_buffer+0x4e4>
    d840:	e350001b 	cmp	r0, #27	; 0x1b
    d844:	05cd0008 	strbeq	r0, [sp, #8]
    d848:	03a05001 	moveq	r5, #1	; 0x1
    d84c:	0affff96 	beq	d6ac <readline_into_buffer+0x98>
    d850:	8a000002 	bhi	d860 <readline_into_buffer+0x24c>
    d854:	e3500018 	cmp	r0, #24	; 0x18
    d858:	1a0000ee 	bne	dc18 <readline_into_buffer+0x604>
    d85c:	ea00005f 	b	d9e0 <readline_into_buffer+0x3cc>
    d860:	e350007f 	cmp	r0, #127	; 0x7f
    d864:	0a000075 	beq	da40 <readline_into_buffer+0x42c>
    d868:	e35000ff 	cmp	r0, #255	; 0xff
    d86c:	1a0000e9 	bne	dc18 <readline_into_buffer+0x604>
    d870:	ea000072 	b	da40 <readline_into_buffer+0x42c>
    d874:	ebfffe29 	bl	d120 <putc>
    d878:	e59d3014 	ldr	r3, [sp, #20]
    d87c:	e2433001 	sub	r3, r3, #1	; 0x1
    d880:	e58d3014 	str	r3, [sp, #20]
    d884:	e59d3014 	ldr	r3, [sp, #20]
    d888:	e3a00008 	mov	r0, #8	; 0x8
    d88c:	e3530000 	cmp	r3, #0	; 0x0
    d890:	1afffff7 	bne	d874 <readline_into_buffer+0x260>
    d894:	eaffff84 	b	d6ac <readline_into_buffer+0x98>
    d898:	e3a03000 	mov	r3, #0	; 0x0
    d89c:	e5c63000 	strb	r3, [r6]
    d8a0:	e3e00000 	mvn	r0, #0	; 0x0
    d8a4:	ea0000fa 	b	dc94 <readline_into_buffer+0x680>
    d8a8:	e59d2014 	ldr	r2, [sp, #20]
    d8ac:	e59d3010 	ldr	r3, [sp, #16]
    d8b0:	e1520003 	cmp	r2, r3
    d8b4:	2affff7c 	bcs	d6ac <readline_into_buffer+0x98>
    d8b8:	e7d60002 	ldrb	r0, [r6, r2]
    d8bc:	ebfffe17 	bl	d120 <putc>
    d8c0:	e59d3014 	ldr	r3, [sp, #20]
    d8c4:	e2833001 	add	r3, r3, #1	; 0x1
    d8c8:	ea0000d0 	b	dc10 <readline_into_buffer+0x5fc>
    d8cc:	e3a05000 	mov	r5, #0	; 0x0
    d8d0:	e59d3014 	ldr	r3, [sp, #20]
    d8d4:	e3530000 	cmp	r3, #0	; 0x0
    d8d8:	0affff73 	beq	d6ac <readline_into_buffer+0x98>
    d8dc:	e3a00008 	mov	r0, #8	; 0x8
    d8e0:	ebfffe0e 	bl	d120 <putc>
    d8e4:	e59d3014 	ldr	r3, [sp, #20]
    d8e8:	e2433001 	sub	r3, r3, #1	; 0x1
    d8ec:	ea0000c7 	b	dc10 <readline_into_buffer+0x5fc>
    d8f0:	e59d2014 	ldr	r2, [sp, #20]
    d8f4:	e59d3010 	ldr	r3, [sp, #16]
    d8f8:	e1520003 	cmp	r2, r3
    d8fc:	2affff6a 	bcs	d6ac <readline_into_buffer+0x98>
    d900:	e2433001 	sub	r3, r3, #1	; 0x1
    d904:	e0534002 	subs	r4, r3, r2
    d908:	0a00000a 	beq	d938 <readline_into_buffer+0x324>
    d90c:	e2821001 	add	r1, r2, #1	; 0x1
    d910:	e0861001 	add	r1, r6, r1
    d914:	e0860002 	add	r0, r6, r2
    d918:	e1a02004 	mov	r2, r4
    d91c:	ebffceb6 	bl	13fc <memmove>
    d920:	e59d2014 	ldr	r2, [sp, #20]
    d924:	e59f0394 	ldr	r0, [pc, #916]	; dcc0 <readline_into_buffer+0x6ac>
    d928:	e0862002 	add	r2, r6, r2
    d92c:	e1a01004 	mov	r1, r4
    d930:	e1a0e00f 	mov	lr, pc
    d934:	e59bf000 	ldr	pc, [fp]
    d938:	e3a00020 	mov	r0, #32	; 0x20
    d93c:	ebfffdf7 	bl	d120 <putc>
    d940:	e2444001 	sub	r4, r4, #1	; 0x1
    d944:	e3a00008 	mov	r0, #8	; 0x8
    d948:	ebfffdf4 	bl	d120 <putc>
    d94c:	e3740001 	cmn	r4, #1	; 0x1
    d950:	1afffffa 	bne	d940 <readline_into_buffer+0x32c>
    d954:	ea000053 	b	daa8 <readline_into_buffer+0x494>
    d958:	e59d2014 	ldr	r2, [sp, #20]
    d95c:	e59d3010 	ldr	r3, [sp, #16]
    d960:	e1520003 	cmp	r2, r3
    d964:	31a04002 	movcc	r4, r2
    d968:	3a000002 	bcc	d978 <readline_into_buffer+0x364>
    d96c:	eaffff4e 	b	d6ac <readline_into_buffer+0x98>
    d970:	ebfffdea 	bl	d120 <putc>
    d974:	e2844001 	add	r4, r4, #1	; 0x1
    d978:	e59d3010 	ldr	r3, [sp, #16]
    d97c:	e3a00020 	mov	r0, #32	; 0x20
    d980:	e1540003 	cmp	r4, r3
    d984:	3afffff9 	bcc	d970 <readline_into_buffer+0x35c>
    d988:	ea000000 	b	d990 <readline_into_buffer+0x37c>
    d98c:	ebfffde3 	bl	d120 <putc>
    d990:	e59d2014 	ldr	r2, [sp, #20]
    d994:	e3a00008 	mov	r0, #8	; 0x8
    d998:	e1540002 	cmp	r4, r2
    d99c:	e2444001 	sub	r4, r4, #1	; 0x1
    d9a0:	8afffff9 	bhi	d98c <readline_into_buffer+0x378>
    d9a4:	ea000023 	b	da38 <readline_into_buffer+0x424>
    d9a8:	e59d3014 	ldr	r3, [sp, #20]
    d9ac:	e59d1010 	ldr	r1, [sp, #16]
    d9b0:	e1530001 	cmp	r3, r1
    d9b4:	2affff3c 	bcs	d6ac <readline_into_buffer+0x98>
    d9b8:	e0631001 	rsb	r1, r3, r1
    d9bc:	e0862003 	add	r2, r6, r3
    d9c0:	e59f02f8 	ldr	r0, [pc, #760]	; dcc0 <readline_into_buffer+0x6ac>
    d9c4:	e1a0e00f 	mov	lr, pc
    d9c8:	e59bf000 	ldr	pc, [fp]
    d9cc:	ea00008e 	b	dc0c <readline_into_buffer+0x5f8>
    d9d0:	ebfffdd2 	bl	d120 <putc>
    d9d4:	e59d3014 	ldr	r3, [sp, #20]
    d9d8:	e2433001 	sub	r3, r3, #1	; 0x1
    d9dc:	e58d3014 	str	r3, [sp, #20]
    d9e0:	e59d2014 	ldr	r2, [sp, #20]
    d9e4:	e3a00008 	mov	r0, #8	; 0x8
    d9e8:	e3520000 	cmp	r2, #0	; 0x0
    d9ec:	1afffff7 	bne	d9d0 <readline_into_buffer+0x3bc>
    d9f0:	e59d3010 	ldr	r3, [sp, #16]
    d9f4:	e3530000 	cmp	r3, #0	; 0x0
    d9f8:	11a04002 	movne	r4, r2
    d9fc:	1a000002 	bne	da0c <readline_into_buffer+0x3f8>
    da00:	eaffff29 	b	d6ac <readline_into_buffer+0x98>
    da04:	ebfffdc5 	bl	d120 <putc>
    da08:	e2844001 	add	r4, r4, #1	; 0x1
    da0c:	e59d3010 	ldr	r3, [sp, #16]
    da10:	e3a00020 	mov	r0, #32	; 0x20
    da14:	e1540003 	cmp	r4, r3
    da18:	3afffff9 	bcc	da04 <readline_into_buffer+0x3f0>
    da1c:	ea000000 	b	da24 <readline_into_buffer+0x410>
    da20:	ebfffdbe 	bl	d120 <putc>
    da24:	e59d2014 	ldr	r2, [sp, #20]
    da28:	e3a00008 	mov	r0, #8	; 0x8
    da2c:	e1540002 	cmp	r4, r2
    da30:	e2444001 	sub	r4, r4, #1	; 0x1
    da34:	8afffff9 	bhi	da20 <readline_into_buffer+0x40c>
    da38:	e58d2010 	str	r2, [sp, #16]
    da3c:	eaffff1a 	b	d6ac <readline_into_buffer+0x98>
    da40:	e59d0014 	ldr	r0, [sp, #20]
    da44:	e3500000 	cmp	r0, #0	; 0x0
    da48:	0affff17 	beq	d6ac <readline_into_buffer+0x98>
    da4c:	e59d2010 	ldr	r2, [sp, #16]
    da50:	e2403001 	sub	r3, r0, #1	; 0x1
    da54:	e0604002 	rsb	r4, r0, r2
    da58:	e0861000 	add	r1, r6, r0
    da5c:	e1a02004 	mov	r2, r4
    da60:	e0860003 	add	r0, r6, r3
    da64:	e58d3014 	str	r3, [sp, #20]
    da68:	ebffce63 	bl	13fc <memmove>
    da6c:	e3a00008 	mov	r0, #8	; 0x8
    da70:	ebfffdaa 	bl	d120 <putc>
    da74:	e59d2014 	ldr	r2, [sp, #20]
    da78:	e59f0240 	ldr	r0, [pc, #576]	; dcc0 <readline_into_buffer+0x6ac>
    da7c:	e0862002 	add	r2, r6, r2
    da80:	e1a01004 	mov	r1, r4
    da84:	e1a0e00f 	mov	lr, pc
    da88:	e59bf000 	ldr	pc, [fp]
    da8c:	e3a00020 	mov	r0, #32	; 0x20
    da90:	ebfffda2 	bl	d120 <putc>
    da94:	e2444001 	sub	r4, r4, #1	; 0x1
    da98:	e3a00008 	mov	r0, #8	; 0x8
    da9c:	ebfffd9f 	bl	d120 <putc>
    daa0:	e3740001 	cmn	r4, #1	; 0x1
    daa4:	1afffffa 	bne	da94 <readline_into_buffer+0x480>
    daa8:	e59d3010 	ldr	r3, [sp, #16]
    daac:	e2433001 	sub	r3, r3, #1	; 0x1
    dab0:	e58d3010 	str	r3, [sp, #16]
    dab4:	eafffefc 	b	d6ac <readline_into_buffer+0x98>
    dab8:	e59a2000 	ldr	r2, [sl]
    dabc:	e3520000 	cmp	r2, #0	; 0x0
    dac0:	ba000024 	blt	db58 <readline_into_buffer+0x544>
    dac4:	e2423001 	sub	r3, r2, #1	; 0x1
    dac8:	e3730001 	cmn	r3, #1	; 0x1
    dacc:	059f01d0 	ldreq	r0, [pc, #464]	; dca4 <readline_into_buffer+0x690>
    dad0:	e58a3000 	str	r3, [sl]
    dad4:	05903000 	ldreq	r3, [r0]
    dad8:	e59fc1cc 	ldr	ip, [pc, #460]	; dcac <readline_into_buffer+0x698>
    dadc:	058a3000 	streq	r3, [sl]
    dae0:	e59a1000 	ldr	r1, [sl]
    dae4:	e59c3000 	ldr	r3, [ip]
    dae8:	e1510003 	cmp	r1, r3
    daec:	058a2000 	streq	r2, [sl]
    daf0:	1a000014 	bne	db48 <readline_into_buffer+0x534>
    daf4:	ea000017 	b	db58 <readline_into_buffer+0x544>
    daf8:	e59a1000 	ldr	r1, [sl]
    dafc:	e3510000 	cmp	r1, #0	; 0x0
    db00:	ba000014 	blt	db58 <readline_into_buffer+0x544>
    db04:	e59f21a0 	ldr	r2, [pc, #416]	; dcac <readline_into_buffer+0x698>
    db08:	e5923000 	ldr	r3, [r2]
    db0c:	e1510003 	cmp	r1, r3
    db10:	0a000010 	beq	db58 <readline_into_buffer+0x544>
    db14:	e59f3188 	ldr	r3, [pc, #392]	; dca4 <readline_into_buffer+0x690>
    db18:	e59fc18c 	ldr	ip, [pc, #396]	; dcac <readline_into_buffer+0x698>
    db1c:	e5932000 	ldr	r2, [r3]
    db20:	e2813001 	add	r3, r1, #1	; 0x1
    db24:	e1530002 	cmp	r3, r2
    db28:	e58a3000 	str	r3, [sl]
    db2c:	c3a03000 	movgt	r3, #0	; 0x0
    db30:	c58a3000 	strgt	r3, [sl]
    db34:	e59a1000 	ldr	r1, [sl]
    db38:	e59c3000 	ldr	r3, [ip]
    db3c:	e1510003 	cmp	r1, r3
    db40:	059f517c 	ldreq	r5, [pc, #380]	; dcc4 <readline_into_buffer+0x6b0>
    db44:	0a00000a 	beq	db74 <readline_into_buffer+0x560>
    db48:	e59f0160 	ldr	r0, [pc, #352]	; dcb0 <readline_into_buffer+0x69c>
    db4c:	e7905101 	ldr	r5, [r0, r1, lsl #2]
    db50:	e3550000 	cmp	r5, #0	; 0x0
    db54:	1a000006 	bne	db74 <readline_into_buffer+0x560>
    db58:	e3a00007 	mov	r0, #7	; 0x7
    db5c:	ebfffd6f 	bl	d120 <putc>
    db60:	eafffed0 	b	d6a8 <readline_into_buffer+0x94>
    db64:	ebfffd6d 	bl	d120 <putc>
    db68:	e59d3014 	ldr	r3, [sp, #20]
    db6c:	e2433001 	sub	r3, r3, #1	; 0x1
    db70:	e58d3014 	str	r3, [sp, #20]
    db74:	e59d2014 	ldr	r2, [sp, #20]
    db78:	e3a00008 	mov	r0, #8	; 0x8
    db7c:	e3520000 	cmp	r2, #0	; 0x0
    db80:	1afffff7 	bne	db64 <readline_into_buffer+0x550>
    db84:	e59d3010 	ldr	r3, [sp, #16]
    db88:	e3530000 	cmp	r3, #0	; 0x0
    db8c:	11a04002 	movne	r4, r2
    db90:	1a000002 	bne	dba0 <readline_into_buffer+0x58c>
    db94:	ea00000d 	b	dbd0 <readline_into_buffer+0x5bc>
    db98:	ebfffd60 	bl	d120 <putc>
    db9c:	e2844001 	add	r4, r4, #1	; 0x1
    dba0:	e59d3010 	ldr	r3, [sp, #16]
    dba4:	e3a00020 	mov	r0, #32	; 0x20
    dba8:	e1540003 	cmp	r4, r3
    dbac:	3afffff9 	bcc	db98 <readline_into_buffer+0x584>
    dbb0:	ea000000 	b	dbb8 <readline_into_buffer+0x5a4>
    dbb4:	ebfffd59 	bl	d120 <putc>
    dbb8:	e59d2014 	ldr	r2, [sp, #20]
    dbbc:	e3a00008 	mov	r0, #8	; 0x8
    dbc0:	e1540002 	cmp	r4, r2
    dbc4:	e2444001 	sub	r4, r4, #1	; 0x1
    dbc8:	8afffff9 	bhi	dbb4 <readline_into_buffer+0x5a0>
    dbcc:	e58d2010 	str	r2, [sp, #16]
    dbd0:	e1a01005 	mov	r1, r5
    dbd4:	e1a00006 	mov	r0, r6
    dbd8:	ebffcd8d 	bl	1214 <strcpy>
    dbdc:	e1a00006 	mov	r0, r6
    dbe0:	ebffcde2 	bl	1370 <strlen>
    dbe4:	e59d3014 	ldr	r3, [sp, #20]
    dbe8:	e58d0010 	str	r0, [sp, #16]
    dbec:	e1530000 	cmp	r3, r0
    dbf0:	2afffeac 	bcs	d6a8 <readline_into_buffer+0x94>
    dbf4:	e0631000 	rsb	r1, r3, r0
    dbf8:	e0862003 	add	r2, r6, r3
    dbfc:	e59f00bc 	ldr	r0, [pc, #188]	; dcc0 <readline_into_buffer+0x6ac>
    dc00:	e1a0e00f 	mov	lr, pc
    dc04:	e59bf000 	ldr	pc, [fp]
    dc08:	e3a05000 	mov	r5, #0	; 0x0
    dc0c:	e59d3010 	ldr	r3, [sp, #16]
    dc10:	e58d3014 	str	r3, [sp, #20]
    dc14:	eafffea4 	b	d6ac <readline_into_buffer+0x98>
    dc18:	e3a0cc01 	mov	ip, #256	; 0x100
    dc1c:	e1a01009 	mov	r1, r9
    dc20:	e28d2014 	add	r2, sp, #20	; 0x14
    dc24:	e28d3010 	add	r3, sp, #16	; 0x10
    dc28:	e88d1040 	stm	sp, {r6, ip}
    dc2c:	ebfffe34 	bl	d504 <cread_add_char>
    dc30:	eafffe9d 	b	d6ac <readline_into_buffer+0x98>
    dc34:	e59f4070 	ldr	r4, [pc, #112]	; dcac <readline_into_buffer+0x698>
    dc38:	e59f3070 	ldr	r3, [pc, #112]	; dcb0 <readline_into_buffer+0x69c>
    dc3c:	e5942000 	ldr	r2, [r4]
    dc40:	e1a01006 	mov	r1, r6
    dc44:	e7930102 	ldr	r0, [r3, r2, lsl #2]
    dc48:	ebffcd71 	bl	1214 <strcpy>
    dc4c:	e5943000 	ldr	r3, [r4]
    dc50:	e59f104c 	ldr	r1, [pc, #76]	; dca4 <readline_into_buffer+0x690>
    dc54:	e2833001 	add	r3, r3, #1	; 0x1
    dc58:	e5912000 	ldr	r2, [r1]
    dc5c:	e3530003 	cmp	r3, #3	; 0x3
    dc60:	c3a03000 	movgt	r3, #0	; 0x0
    dc64:	e1530002 	cmp	r3, r2
    dc68:	e59f2044 	ldr	r2, [pc, #68]	; dcb4 <readline_into_buffer+0x6a0>
    dc6c:	e5843000 	str	r3, [r4]
    dc70:	c5813000 	strgt	r3, [r1]
    dc74:	e5923000 	ldr	r3, [r2]
    dc78:	e2833001 	add	r3, r3, #1	; 0x1
    dc7c:	e5823000 	str	r3, [r2]
    dc80:	e59f3024 	ldr	r3, [pc, #36]	; dcac <readline_into_buffer+0x698>
    dc84:	e1a00005 	mov	r0, r5
    dc88:	e5932000 	ldr	r2, [r3]
    dc8c:	e59f300c 	ldr	r3, [pc, #12]	; dca0 <readline_into_buffer+0x68c>
    dc90:	e5832000 	str	r2, [r3]
    dc94:	e28dd01c 	add	sp, sp, #28	; 0x1c
    dc98:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    dc9c:	0800d2ac 	.word	0x0800d2ac
    dca0:	0800d14c 	.word	0x0800d14c
    dca4:	0800d2b0 	.word	0x0800d2b0
    dca8:	0800d898 	.word	0x0800d898
    dcac:	0800d2b4 	.word	0x0800d2b4
    dcb0:	0800d888 	.word	0x0800d888
    dcb4:	0800d2a8 	.word	0x0800d2a8
    dcb8:	0800d704 	.word	0x0800d704
    dcbc:	0001140d 	.word	0x0001140d
    dcc0:	0001251d 	.word	0x0001251d
    dcc4:	0001140e 	.word	0x0001140e

0000dcc8 <readline>:
    dcc8:	e59f1000 	ldr	r1, [pc, #0]	; dcd0 <readline+0x8>
    dccc:	eafffe50 	b	d614 <readline_into_buffer>
    dcd0:	0800d788 	.word	0x0800d788

0000dcd4 <main_loop>:
    dcd4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    dcd8:	e24dd004 	sub	sp, sp, #4	; 0x4
    dcdc:	e59f0030 	ldr	r0, [pc, #48]	; dd14 <main_loop+0x40>
    dce0:	ebfffff8 	bl	dcc8 <readline>
    dce4:	e1a03000 	mov	r3, r0
    dce8:	e3730001 	cmn	r3, #1	; 0x1
    dcec:	e3a01000 	mov	r1, #0	; 0x0
    dcf0:	e59f0020 	ldr	r0, [pc, #32]	; dd18 <main_loop+0x44>
    dcf4:	1a000002 	bne	dd04 <main_loop+0x30>
    dcf8:	e59f001c 	ldr	r0, [pc, #28]	; dd1c <main_loop+0x48>
    dcfc:	ebfffcdf 	bl	d080 <puts>
    dd00:	eafffff5 	b	dcdc <main_loop+0x8>
    dd04:	e3530000 	cmp	r3, #0	; 0x0
    dd08:	dafffff3 	ble	dcdc <main_loop+0x8>
    dd0c:	ebfffd41 	bl	d218 <run_command>
    dd10:	eafffff1 	b	dcdc <main_loop+0x8>
    dd14:	0001253d 	.word	0x0001253d
    dd18:	0800d788 	.word	0x0800d788
    dd1c:	0001140d 	.word	0x0001140d

0000dd20 <parse_num>:
    dd20:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    dd24:	e1a08001 	mov	r8, r1
    dd28:	e1a07002 	mov	r7, r2
    dd2c:	e1a06003 	mov	r6, r3
    dd30:	ea000000 	b	dd38 <parse_num+0x18>
    dd34:	e2800001 	add	r0, r0, #1	; 0x1
    dd38:	e5d03000 	ldrb	r3, [r0]
    dd3c:	e3530020 	cmp	r3, #32	; 0x20
    dd40:	0afffffb 	beq	dd34 <parse_num+0x14>
    dd44:	ea000037 	b	de28 <parse_num+0x108>
    dd48:	e3520000 	cmp	r2, #0	; 0x0
    dd4c:	0a00000a 	beq	dd7c <parse_num+0x5c>
    dd50:	e3530030 	cmp	r3, #48	; 0x30
    dd54:	1a000008 	bne	dd7c <parse_num+0x5c>
    dd58:	e5d02001 	ldrb	r2, [r0, #1]
    dd5c:	e2423041 	sub	r3, r2, #65	; 0x41
    dd60:	e6ef3073 	uxtb	r3, r3
    dd64:	e3530019 	cmp	r3, #25	; 0x19
    dd68:	92823020 	addls	r3, r2, #32	; 0x20
    dd6c:	96ef2073 	uxtbls	r2, r3
    dd70:	e3520078 	cmp	r2, #120	; 0x78
    dd74:	02800002 	addeq	r0, r0, #2	; 0x2
    dd78:	03a04010 	moveq	r4, #16	; 0x10
    dd7c:	e4d0c001 	ldrb	ip, [r0], #1
    dd80:	e24c3030 	sub	r3, ip, #48	; 0x30
    dd84:	e24c2041 	sub	r2, ip, #65	; 0x41
    dd88:	e6ef3073 	uxtb	r3, r3
    dd8c:	e6ef2072 	uxtb	r2, r2
    dd90:	e3530009 	cmp	r3, #9	; 0x9
    dd94:	83a01000 	movhi	r1, #0	; 0x0
    dd98:	93a01001 	movls	r1, #1	; 0x1
    dd9c:	e3520005 	cmp	r2, #5	; 0x5
    dda0:	83a02000 	movhi	r2, #0	; 0x0
    dda4:	93a02001 	movls	r2, #1	; 0x1
    dda8:	e1913002 	orrs	r3, r1, r2
    ddac:	1a000003 	bne	ddc0 <parse_num+0xa0>
    ddb0:	e24c3061 	sub	r3, ip, #97	; 0x61
    ddb4:	e6ef3073 	uxtb	r3, r3
    ddb8:	e3530005 	cmp	r3, #5	; 0x5
    ddbc:	8a00000f 	bhi	de00 <parse_num+0xe0>
    ddc0:	e3510000 	cmp	r1, #0	; 0x0
    ddc4:	124c2030 	subne	r2, ip, #48	; 0x30
    ddc8:	1a000007 	bne	ddec <parse_num+0xcc>
    ddcc:	e24c3061 	sub	r3, ip, #97	; 0x61
    ddd0:	e6ef3073 	uxtb	r3, r3
    ddd4:	e3530005 	cmp	r3, #5	; 0x5
    ddd8:	924c2057 	subls	r2, ip, #87	; 0x57
    dddc:	9a000002 	bls	ddec <parse_num+0xcc>
    dde0:	e3520000 	cmp	r2, #0	; 0x0
    dde4:	0a000002 	beq	ddf4 <parse_num+0xd4>
    dde8:	e24c2037 	sub	r2, ip, #55	; 0x37
    ddec:	e1520004 	cmp	r2, r4
    ddf0:	aa000002 	bge	de00 <parse_num+0xe0>
    ddf4:	e0252495 	mla	r5, r5, r4, r2
    ddf8:	e3a02000 	mov	r2, #0	; 0x0
    ddfc:	ea00000c 	b	de34 <parse_num+0x114>
    de00:	e3560000 	cmp	r6, #0	; 0x0
    de04:	0a000012 	beq	de54 <parse_num+0x134>
    de08:	e3a03000 	mov	r3, #0	; 0x0
    de0c:	e7d62003 	ldrb	r2, [r6, r3]
    de10:	e2833001 	add	r3, r3, #1	; 0x1
    de14:	e3520000 	cmp	r2, #0	; 0x0
    de18:	0a00000d 	beq	de54 <parse_num+0x134>
    de1c:	e15c0002 	cmp	ip, r2
    de20:	1afffff9 	bne	de0c <parse_num+0xec>
    de24:	ea000005 	b	de40 <parse_num+0x120>
    de28:	e3a02001 	mov	r2, #1	; 0x1
    de2c:	e3a0400a 	mov	r4, #10	; 0xa
    de30:	e3a05000 	mov	r5, #0	; 0x0
    de34:	e5d03000 	ldrb	r3, [r0]
    de38:	e3530000 	cmp	r3, #0	; 0x0
    de3c:	1affffc1 	bne	dd48 <parse_num+0x28>
    de40:	e3570000 	cmp	r7, #0	; 0x0
    de44:	15870000 	strne	r0, [r7]
    de48:	e5885000 	str	r5, [r8]
    de4c:	e3a00001 	mov	r0, #1	; 0x1
    de50:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    de54:	e3a00000 	mov	r0, #0	; 0x0
    de58:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000de5c <xyzModem_stream_close>:
    de5c:	e59f2044 	ldr	r2, [pc, #68]	; dea8 <xyzModem_stream_close+0x4c>
    de60:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    de64:	e5920428 	ldr	r0, [r2, #1064]
    de68:	e59f303c 	ldr	r3, [pc, #60]	; deac <xyzModem_stream_close+0x50>
    de6c:	e592c424 	ldr	ip, [r2, #1060]
    de70:	e592e418 	ldr	lr, [r2, #1048]
    de74:	e3500000 	cmp	r0, #0	; 0x0
    de78:	e24dd00c 	sub	sp, sp, #12	; 0xc
    de7c:	e59f102c 	ldr	r1, [pc, #44]	; deb0 <xyzModem_stream_close+0x54>
    de80:	01a01003 	moveq	r1, r3
    de84:	e59f3028 	ldr	r3, [pc, #40]	; deb4 <xyzModem_stream_close+0x58>
    de88:	e88d5000 	stm	sp, {ip, lr}
    de8c:	e593c000 	ldr	ip, [r3]
    de90:	e59f0020 	ldr	r0, [pc, #32]	; deb8 <xyzModem_stream_close+0x5c>
    de94:	e5923420 	ldr	r3, [r2, #1056]
    de98:	e592241c 	ldr	r2, [r2, #1052]
    de9c:	e12fff3c 	blx	ip
    dea0:	e28dd00c 	add	sp, sp, #12	; 0xc
    dea4:	e8bd8000 	pop	{pc}
    dea8:	0800d2c8 	.word	0x0800d2c8
    deac:	00012541 	.word	0x00012541
    deb0:	00012547 	.word	0x00012547
    deb4:	0800d704 	.word	0x0800d704
    deb8:	0001254b 	.word	0x0001254b

0000debc <xyzModem_error>:
    debc:	e2800008 	add	r0, r0, #8	; 0x8
    dec0:	e3500007 	cmp	r0, #7	; 0x7
    dec4:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    dec8:	ea000017 	b	df2c <xyzModem_error+0x70>
    decc:	0000df24 	.word	0x0000df24
    ded0:	0000df1c 	.word	0x0000df1c
    ded4:	0000df14 	.word	0x0000df14
    ded8:	0000df0c 	.word	0x0000df0c
    dedc:	0000df04 	.word	0x0000df04
    dee0:	0000defc 	.word	0x0000defc
    dee4:	0000deec 	.word	0x0000deec
    dee8:	0000def4 	.word	0x0000def4
    deec:	e59f0040 	ldr	r0, [pc, #64]	; df34 <xyzModem_error+0x78>
    def0:	e12fff1e 	bx	lr
    def4:	e59f003c 	ldr	r0, [pc, #60]	; df38 <xyzModem_error+0x7c>
    def8:	e12fff1e 	bx	lr
    defc:	e59f0038 	ldr	r0, [pc, #56]	; df3c <xyzModem_error+0x80>
    df00:	e12fff1e 	bx	lr
    df04:	e59f0034 	ldr	r0, [pc, #52]	; df40 <xyzModem_error+0x84>
    df08:	e12fff1e 	bx	lr
    df0c:	e59f0030 	ldr	r0, [pc, #48]	; df44 <xyzModem_error+0x88>
    df10:	e12fff1e 	bx	lr
    df14:	e59f002c 	ldr	r0, [pc, #44]	; df48 <xyzModem_error+0x8c>
    df18:	e12fff1e 	bx	lr
    df1c:	e59f0028 	ldr	r0, [pc, #40]	; df4c <xyzModem_error+0x90>
    df20:	e12fff1e 	bx	lr
    df24:	e59f0024 	ldr	r0, [pc, #36]	; df50 <xyzModem_error+0x94>
    df28:	e12fff1e 	bx	lr
    df2c:	e59f0020 	ldr	r0, [pc, #32]	; df54 <xyzModem_error+0x98>
    df30:	e12fff1e 	bx	lr
    df34:	0001258c 	.word	0x0001258c
    df38:	000125ac 	.word	0x000125ac
    df3c:	000125be 	.word	0x000125be
    df40:	000125c8 	.word	0x000125c8
    df44:	000125d4 	.word	0x000125d4
    df48:	000125de 	.word	0x000125de
    df4c:	000125ee 	.word	0x000125ee
    df50:	00012601 	.word	0x00012601
    df54:	00012616 	.word	0x00012616

0000df58 <delay>:
    df58:	e12fff1e 	bx	lr

0000df5c <init_num>:
    df5c:	e59f305c 	ldr	r3, [pc, #92]	; dfc0 <init_num+0x64>
    df60:	e3a01001 	mov	r1, #1	; 0x1
    df64:	e5c31000 	strb	r1, [r3]
    df68:	e59f3054 	ldr	r3, [pc, #84]	; dfc4 <init_num+0x68>
    df6c:	e3a02000 	mov	r2, #0	; 0x0
    df70:	e5c32000 	strb	r2, [r3]
    df74:	e59f304c 	ldr	r3, [pc, #76]	; dfc8 <init_num+0x6c>
    df78:	e5c32000 	strb	r2, [r3]
    df7c:	e59f3048 	ldr	r3, [pc, #72]	; dfcc <init_num+0x70>
    df80:	e5c32000 	strb	r2, [r3]
    df84:	e59f3044 	ldr	r3, [pc, #68]	; dfd0 <init_num+0x74>
    df88:	e5c32000 	strb	r2, [r3]
    df8c:	e59f3040 	ldr	r3, [pc, #64]	; dfd4 <init_num+0x78>
    df90:	e5832000 	str	r2, [r3]
    df94:	e59f303c 	ldr	r3, [pc, #60]	; dfd8 <init_num+0x7c>
    df98:	e5c32000 	strb	r2, [r3]
    df9c:	e59f3038 	ldr	r3, [pc, #56]	; dfdc <init_num+0x80>
    dfa0:	e5c32000 	strb	r2, [r3]
    dfa4:	e59f3034 	ldr	r3, [pc, #52]	; dfe0 <init_num+0x84>
    dfa8:	e5c32000 	strb	r2, [r3]
    dfac:	e59f3030 	ldr	r3, [pc, #48]	; dfe4 <init_num+0x88>
    dfb0:	e5c32000 	strb	r2, [r3]
    dfb4:	e59f302c 	ldr	r3, [pc, #44]	; dfe8 <init_num+0x8c>
    dfb8:	e5c32000 	strb	r2, [r3]
    dfbc:	e12fff1e 	bx	lr
    dfc0:	0800d150 	.word	0x0800d150
    dfc4:	0800d2c5 	.word	0x0800d2c5
    dfc8:	0800d2b8 	.word	0x0800d2b8
    dfcc:	0800d2b9 	.word	0x0800d2b9
    dfd0:	0800d2ba 	.word	0x0800d2ba
    dfd4:	0800d2bc 	.word	0x0800d2bc
    dfd8:	0800d2c0 	.word	0x0800d2c0
    dfdc:	0800d2c1 	.word	0x0800d2c1
    dfe0:	0800d2c2 	.word	0x0800d2c2
    dfe4:	0800d2c3 	.word	0x0800d2c3
    dfe8:	0800d2c4 	.word	0x0800d2c4

0000dfec <out_message>:
    dfec:	e92d4010 	push	{r4, lr}
    dff0:	e59f30c8 	ldr	r3, [pc, #200]	; e0c0 <out_message+0xd4>
    dff4:	e59f40c8 	ldr	r4, [pc, #200]	; e0c4 <out_message+0xd8>
    dff8:	e5d31000 	ldrb	r1, [r3]
    dffc:	e59f00c4 	ldr	r0, [pc, #196]	; e0c8 <out_message+0xdc>
    e000:	e1a0e00f 	mov	lr, pc
    e004:	e594f000 	ldr	pc, [r4]
    e008:	e59f30bc 	ldr	r3, [pc, #188]	; e0cc <out_message+0xe0>
    e00c:	e59f00bc 	ldr	r0, [pc, #188]	; e0d0 <out_message+0xe4>
    e010:	e5d31000 	ldrb	r1, [r3]
    e014:	e1a0e00f 	mov	lr, pc
    e018:	e594f000 	ldr	pc, [r4]
    e01c:	e59f30b0 	ldr	r3, [pc, #176]	; e0d4 <out_message+0xe8>
    e020:	e59f00b0 	ldr	r0, [pc, #176]	; e0d8 <out_message+0xec>
    e024:	e5931000 	ldr	r1, [r3]
    e028:	e1a0e00f 	mov	lr, pc
    e02c:	e594f000 	ldr	pc, [r4]
    e030:	e59f30a4 	ldr	r3, [pc, #164]	; e0dc <out_message+0xf0>
    e034:	e59f00a4 	ldr	r0, [pc, #164]	; e0e0 <out_message+0xf4>
    e038:	e5d31000 	ldrb	r1, [r3]
    e03c:	e1a0e00f 	mov	lr, pc
    e040:	e594f000 	ldr	pc, [r4]
    e044:	e59f3098 	ldr	r3, [pc, #152]	; e0e4 <out_message+0xf8>
    e048:	e59f0098 	ldr	r0, [pc, #152]	; e0e8 <out_message+0xfc>
    e04c:	e5d31000 	ldrb	r1, [r3]
    e050:	e1a0e00f 	mov	lr, pc
    e054:	e594f000 	ldr	pc, [r4]
    e058:	e59f308c 	ldr	r3, [pc, #140]	; e0ec <out_message+0x100>
    e05c:	e59f008c 	ldr	r0, [pc, #140]	; e0f0 <out_message+0x104>
    e060:	e5d31000 	ldrb	r1, [r3]
    e064:	e1a0e00f 	mov	lr, pc
    e068:	e594f000 	ldr	pc, [r4]
    e06c:	e59f3080 	ldr	r3, [pc, #128]	; e0f4 <out_message+0x108>
    e070:	e59f0080 	ldr	r0, [pc, #128]	; e0f8 <out_message+0x10c>
    e074:	e5d31000 	ldrb	r1, [r3]
    e078:	e1a0e00f 	mov	lr, pc
    e07c:	e594f000 	ldr	pc, [r4]
    e080:	e59f3074 	ldr	r3, [pc, #116]	; e0fc <out_message+0x110>
    e084:	e59f0074 	ldr	r0, [pc, #116]	; e100 <out_message+0x114>
    e088:	e5d31000 	ldrb	r1, [r3]
    e08c:	e1a0e00f 	mov	lr, pc
    e090:	e594f000 	ldr	pc, [r4]
    e094:	e59f3068 	ldr	r3, [pc, #104]	; e104 <out_message+0x118>
    e098:	e59f0068 	ldr	r0, [pc, #104]	; e108 <out_message+0x11c>
    e09c:	e5d31000 	ldrb	r1, [r3]
    e0a0:	e1a0e00f 	mov	lr, pc
    e0a4:	e594f000 	ldr	pc, [r4]
    e0a8:	e59f305c 	ldr	r3, [pc, #92]	; e10c <out_message+0x120>
    e0ac:	e59f005c 	ldr	r0, [pc, #92]	; e110 <out_message+0x124>
    e0b0:	e5d31000 	ldrb	r1, [r3]
    e0b4:	e1a0e00f 	mov	lr, pc
    e0b8:	e594f000 	ldr	pc, [r4]
    e0bc:	e8bd8010 	pop	{r4, pc}
    e0c0:	0800d2b8 	.word	0x0800d2b8
    e0c4:	0800d704 	.word	0x0800d704
    e0c8:	00012624 	.word	0x00012624
    e0cc:	0800d2b9 	.word	0x0800d2b9
    e0d0:	00012632 	.word	0x00012632
    e0d4:	0800d2bc 	.word	0x0800d2bc
    e0d8:	00012645 	.word	0x00012645
    e0dc:	0800d2c0 	.word	0x0800d2c0
    e0e0:	00012657 	.word	0x00012657
    e0e4:	0800d2c1 	.word	0x0800d2c1
    e0e8:	0001266a 	.word	0x0001266a
    e0ec:	0800d2c2 	.word	0x0800d2c2
    e0f0:	00012681 	.word	0x00012681
    e0f4:	0800d150 	.word	0x0800d150
    e0f8:	00012699 	.word	0x00012699
    e0fc:	0800d2c3 	.word	0x0800d2c3
    e100:	000126a8 	.word	0x000126a8
    e104:	0800d2c4 	.word	0x0800d2c4
    e108:	000126c1 	.word	0x000126c1
    e10c:	0800d2c5 	.word	0x0800d2c5
    e110:	000126d3 	.word	0x000126d3

0000e114 <Procpacket>:
    e114:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    e118:	e59fc1ac 	ldr	ip, [pc, #428]	; e2cc <Procpacket+0x1b8>
    e11c:	e59f21ac 	ldr	r2, [pc, #428]	; e2d0 <Procpacket+0x1bc>
    e120:	e5dc3000 	ldrb	r3, [ip]
    e124:	e1a06000 	mov	r6, r0
    e128:	e5c03000 	strb	r3, [r0]
    e12c:	e5923000 	ldr	r3, [r2]
    e130:	e1a05001 	mov	r5, r1
    e134:	e5c03001 	strb	r3, [r0, #1]
    e138:	e5923000 	ldr	r3, [r2]
    e13c:	e1e03003 	mvn	r3, r3
    e140:	e5c03002 	strb	r3, [r0, #2]
    e144:	e59f3188 	ldr	r3, [pc, #392]	; e2d4 <Procpacket+0x1c0>
    e148:	e5922000 	ldr	r2, [r2]
    e14c:	e5933000 	ldr	r3, [r3]
    e150:	e1520003 	cmp	r2, r3
    e154:	1a000008 	bne	e17c <Procpacket+0x68>
    e158:	e59f3178 	ldr	r3, [pc, #376]	; e2d8 <Procpacket+0x1c4>
    e15c:	e5d32000 	ldrb	r2, [r3]
    e160:	e59f3174 	ldr	r3, [pc, #372]	; e2dc <Procpacket+0x1c8>
    e164:	e3520001 	cmp	r2, #1	; 0x1
    e168:	e5934000 	ldr	r4, [r3]
    e16c:	e5cc2000 	strb	r2, [ip]
    e170:	03a07080 	moveq	r7, #128	; 0x80
    e174:	13a07b01 	movne	r7, #1024	; 0x400
    e178:	ea000004 	b	e190 <Procpacket+0x7c>
    e17c:	e5dc3000 	ldrb	r3, [ip]
    e180:	e3530001 	cmp	r3, #1	; 0x1
    e184:	13a04b01 	movne	r4, #1024	; 0x400
    e188:	03a04080 	moveq	r4, #128	; 0x80
    e18c:	e1a07004 	mov	r7, r4
    e190:	e59f3148 	ldr	r3, [pc, #328]	; e2e0 <Procpacket+0x1cc>
    e194:	e5d33000 	ldrb	r3, [r3]
    e198:	e3530000 	cmp	r3, #0	; 0x0
    e19c:	1a000008 	bne	e1c4 <Procpacket+0xb0>
    e1a0:	e59f313c 	ldr	r3, [pc, #316]	; e2e4 <Procpacket+0x1d0>
    e1a4:	e2860003 	add	r0, r6, #3	; 0x3
    e1a8:	e5931000 	ldr	r1, [r3]
    e1ac:	e59f3134 	ldr	r3, [pc, #308]	; e2e8 <Procpacket+0x1d4>
    e1b0:	e1a02004 	mov	r2, r4
    e1b4:	e5933000 	ldr	r3, [r3]
    e1b8:	e0631001 	rsb	r1, r3, r1
    e1bc:	e0851001 	add	r1, r5, r1
    e1c0:	ebffcc84 	bl	13d8 <memcpy>
    e1c4:	e59f111c 	ldr	r1, [pc, #284]	; e2e8 <Procpacket+0x1d4>
    e1c8:	e59f311c 	ldr	r3, [pc, #284]	; e2ec <Procpacket+0x1d8>
    e1cc:	e5912000 	ldr	r2, [r1]
    e1d0:	e5d33000 	ldrb	r3, [r3]
    e1d4:	e0642002 	rsb	r2, r4, r2
    e1d8:	e3530000 	cmp	r3, #0	; 0x0
    e1dc:	e5812000 	str	r2, [r1]
    e1e0:	0a000006 	beq	e200 <Procpacket+0xec>
    e1e4:	e1a02007 	mov	r2, r7
    e1e8:	e2860003 	add	r0, r6, #3	; 0x3
    e1ec:	e3a01000 	mov	r1, #0	; 0x0
    e1f0:	ebffcc70 	bl	13b8 <memset>
    e1f4:	e59f30f4 	ldr	r3, [pc, #244]	; e2f0 <Procpacket+0x1dc>
    e1f8:	e3a02001 	mov	r2, #1	; 0x1
    e1fc:	e5c32000 	strb	r2, [r3]
    e200:	e59f30c8 	ldr	r3, [pc, #200]	; e2d0 <Procpacket+0x1bc>
    e204:	e5932000 	ldr	r2, [r3]
    e208:	e59f30c4 	ldr	r3, [pc, #196]	; e2d4 <Procpacket+0x1c0>
    e20c:	e5933000 	ldr	r3, [r3]
    e210:	e1520003 	cmp	r2, r3
    e214:	1a000004 	bne	e22c <Procpacket+0x118>
    e218:	e2840003 	add	r0, r4, #3	; 0x3
    e21c:	e0860000 	add	r0, r6, r0
    e220:	e0642007 	rsb	r2, r4, r7
    e224:	e3a0101a 	mov	r1, #26	; 0x1a
    e228:	ebffcc62 	bl	13b8 <memset>
    e22c:	e3a00000 	mov	r0, #0	; 0x0
    e230:	e59f80bc 	ldr	r8, [pc, #188]	; e2f4 <Procpacket+0x1e0>
    e234:	e1a04006 	mov	r4, r6
    e238:	e1a0c000 	mov	ip, r0
    e23c:	e1a05000 	mov	r5, r0
    e240:	e1a0342c 	lsr	r3, ip, #8
    e244:	e5d42003 	ldrb	r2, [r4, #3]
    e248:	e1a03083 	lsl	r3, r3, #1
    e24c:	e19810b3 	ldrh	r1, [r8, r3]
    e250:	e2800001 	add	r0, r0, #1	; 0x1
    e254:	e182340c 	orr	r3, r2, ip, lsl #8
    e258:	e0233001 	eor	r3, r3, r1
    e25c:	e0852002 	add	r2, r5, r2
    e260:	e1500007 	cmp	r0, r7
    e264:	e6ffc073 	uxth	ip, r3
    e268:	e6ef5072 	uxtb	r5, r2
    e26c:	e2844001 	add	r4, r4, #1	; 0x1
    e270:	e59f107c 	ldr	r1, [pc, #124]	; e2f4 <Procpacket+0x1e0>
    e274:	3afffff1 	bcc	e240 <Procpacket+0x12c>
    e278:	e59f3078 	ldr	r3, [pc, #120]	; e2f8 <Procpacket+0x1e4>
    e27c:	e0860007 	add	r0, r6, r7
    e280:	e5d33000 	ldrb	r3, [r3]
    e284:	e3530000 	cmp	r3, #0	; 0x0
    e288:	05c05003 	strbeq	r5, [r0, #3]
    e28c:	0a00000c 	beq	e2c4 <Procpacket+0x1b0>
    e290:	e1a0342c 	lsr	r3, ip, #8
    e294:	e1a03083 	lsl	r3, r3, #1
    e298:	e19120b3 	ldrh	r2, [r1, r3]
    e29c:	e022240c 	eor	r2, r2, ip, lsl #8
    e2a0:	e1a03442 	asr	r3, r2, #8
    e2a4:	e20330ff 	and	r3, r3, #255	; 0xff
    e2a8:	e1a03083 	lsl	r3, r3, #1
    e2ac:	e19130b3 	ldrh	r3, [r1, r3]
    e2b0:	e0233402 	eor	r3, r3, r2, lsl #8
    e2b4:	e6ff3073 	uxth	r3, r3
    e2b8:	e1a02423 	lsr	r2, r3, #8
    e2bc:	e5c03004 	strb	r3, [r0, #4]
    e2c0:	e5c02003 	strb	r2, [r0, #3]
    e2c4:	e3a00000 	mov	r0, #0	; 0x0
    e2c8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    e2cc:	0800d2ba 	.word	0x0800d2ba
    e2d0:	0800d2bc 	.word	0x0800d2bc
    e2d4:	0800dc98 	.word	0x0800dc98
    e2d8:	0800dca0 	.word	0x0800dca0
    e2dc:	0800dc9c 	.word	0x0800dc9c
    e2e0:	0800d2c0 	.word	0x0800d2c0
    e2e4:	0800d764 	.word	0x0800d764
    e2e8:	0800d768 	.word	0x0800d768
    e2ec:	0800d2c5 	.word	0x0800d2c5
    e2f0:	0800d2c4 	.word	0x0800d2c4
    e2f4:	000105bc 	.word	0x000105bc
    e2f8:	0800d760 	.word	0x0800d760

0000e2fc <procsend>:
    e2fc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    e300:	e59f31f8 	ldr	r3, [pc, #504]	; e500 <procsend+0x204>
    e304:	e1a06000 	mov	r6, r0
    e308:	e5d30000 	ldrb	r0, [r3]
    e30c:	e24dd004 	sub	sp, sp, #4	; 0x4
    e310:	e3500000 	cmp	r0, #0	; 0x0
    e314:	e1a0c002 	mov	ip, r2
    e318:	e59f41e4 	ldr	r4, [pc, #484]	; e504 <procsend+0x208>
    e31c:	0a000065 	beq	e4b8 <procsend+0x1bc>
    e320:	e59f31e0 	ldr	r3, [pc, #480]	; e508 <procsend+0x20c>
    e324:	e1a02b02 	lsl	r2, r2, #22
    e328:	e1a02b22 	lsr	r2, r2, #22
    e32c:	e583c000 	str	ip, [r3]
    e330:	e59f31d4 	ldr	r3, [pc, #468]	; e50c <procsend+0x210>
    e334:	e59f01d4 	ldr	r0, [pc, #468]	; e510 <procsend+0x214>
    e338:	e3520000 	cmp	r2, #0	; 0x0
    e33c:	e1a0152c 	lsr	r1, ip, #10
    e340:	e583c000 	str	ip, [r3]
    e344:	03a03b01 	moveq	r3, #1024	; 0x400
    e348:	e5802000 	str	r2, [r0]
    e34c:	e5841000 	str	r1, [r4]
    e350:	05803000 	streq	r3, [r0]
    e354:	0a000006 	beq	e374 <procsend+0x78>
    e358:	e2813001 	add	r3, r1, #1	; 0x1
    e35c:	e352007f 	cmp	r2, #127	; 0x7f
    e360:	e59f21ac 	ldr	r2, [pc, #428]	; e514 <procsend+0x218>
    e364:	e5843000 	str	r3, [r4]
    e368:	93a03001 	movls	r3, #1	; 0x1
    e36c:	83a03002 	movhi	r3, #2	; 0x2
    e370:	e5c23000 	strb	r3, [r2]
    e374:	e59f019c 	ldr	r0, [pc, #412]	; e518 <procsend+0x21c>
    e378:	ebffcbfc 	bl	1370 <strlen>
    e37c:	e59f2198 	ldr	r2, [pc, #408]	; e51c <procsend+0x220>
    e380:	e3500080 	cmp	r0, #128	; 0x80
    e384:	83a03002 	movhi	r3, #2	; 0x2
    e388:	93a03001 	movls	r3, #1	; 0x1
    e38c:	85c23000 	strbhi	r3, [r2]
    e390:	95c23000 	strbls	r3, [r2]
    e394:	e59f3184 	ldr	r3, [pc, #388]	; e520 <procsend+0x224>
    e398:	83a07b01 	movhi	r7, #1024	; 0x400
    e39c:	e5d31000 	ldrb	r1, [r3]
    e3a0:	93a07080 	movls	r7, #128	; 0x80
    e3a4:	e3510000 	cmp	r1, #0	; 0x0
    e3a8:	13a01000 	movne	r1, #0	; 0x0
    e3ac:	12872005 	addne	r2, r7, #5	; 0x5
    e3b0:	02872004 	addeq	r2, r7, #4	; 0x4
    e3b4:	11a00006 	movne	r0, r6
    e3b8:	01a00006 	moveq	r0, r6
    e3bc:	ebffcbfd 	bl	13b8 <memset>
    e3c0:	e59f3154 	ldr	r3, [pc, #340]	; e51c <procsend+0x220>
    e3c4:	e3a05000 	mov	r5, #0	; 0x0
    e3c8:	e5d33000 	ldrb	r3, [r3]
    e3cc:	e59f0144 	ldr	r0, [pc, #324]	; e518 <procsend+0x21c>
    e3d0:	e5c63000 	strb	r3, [r6]
    e3d4:	e3e03000 	mvn	r3, #0	; 0x0
    e3d8:	e5c63002 	strb	r3, [r6, #2]
    e3dc:	e5c65001 	strb	r5, [r6, #1]
    e3e0:	ebffcbe2 	bl	1370 <strlen>
    e3e4:	e59f112c 	ldr	r1, [pc, #300]	; e518 <procsend+0x21c>
    e3e8:	e1a02000 	mov	r2, r0
    e3ec:	e1a04000 	mov	r4, r0
    e3f0:	e2860003 	add	r0, r6, #3	; 0x3
    e3f4:	ebffcb8d 	bl	1230 <strncpy>
    e3f8:	e59f3108 	ldr	r3, [pc, #264]	; e508 <procsend+0x20c>
    e3fc:	e2844004 	add	r4, r4, #4	; 0x4
    e400:	e0860004 	add	r0, r6, r4
    e404:	e5932000 	ldr	r2, [r3]
    e408:	e59f1114 	ldr	r1, [pc, #276]	; e524 <procsend+0x228>
    e40c:	e59f4114 	ldr	r4, [pc, #276]	; e528 <procsend+0x22c>
    e410:	ebfffab8 	bl	cef8 <sprintf>
    e414:	e1a00006 	mov	r0, r6
    e418:	e1a0c005 	mov	ip, r5
    e41c:	e1a0e005 	mov	lr, r5
    e420:	e1a0342c 	lsr	r3, ip, #8
    e424:	e5d02003 	ldrb	r2, [r0, #3]
    e428:	e1a03083 	lsl	r3, r3, #1
    e42c:	e19410b3 	ldrh	r1, [r4, r3]
    e430:	e2855001 	add	r5, r5, #1	; 0x1
    e434:	e182340c 	orr	r3, r2, ip, lsl #8
    e438:	e0233001 	eor	r3, r3, r1
    e43c:	e08e2002 	add	r2, lr, r2
    e440:	e1550007 	cmp	r5, r7
    e444:	e6ffc073 	uxth	ip, r3
    e448:	e6efe072 	uxtb	lr, r2
    e44c:	e2800001 	add	r0, r0, #1	; 0x1
    e450:	e59f10d0 	ldr	r1, [pc, #208]	; e528 <procsend+0x22c>
    e454:	3afffff1 	bcc	e420 <procsend+0x124>
    e458:	e59f30c0 	ldr	r3, [pc, #192]	; e520 <procsend+0x224>
    e45c:	e0860007 	add	r0, r6, r7
    e460:	e5d33000 	ldrb	r3, [r3]
    e464:	e3530000 	cmp	r3, #0	; 0x0
    e468:	05c0e003 	strbeq	lr, [r0, #3]
    e46c:	0a00000c 	beq	e4a4 <procsend+0x1a8>
    e470:	e1a0342c 	lsr	r3, ip, #8
    e474:	e1a03083 	lsl	r3, r3, #1
    e478:	e19120b3 	ldrh	r2, [r1, r3]
    e47c:	e022240c 	eor	r2, r2, ip, lsl #8
    e480:	e1a03442 	asr	r3, r2, #8
    e484:	e20330ff 	and	r3, r3, #255	; 0xff
    e488:	e1a03083 	lsl	r3, r3, #1
    e48c:	e19130b3 	ldrh	r3, [r1, r3]
    e490:	e0233402 	eor	r3, r3, r2, lsl #8
    e494:	e6ff3073 	uxth	r3, r3
    e498:	e1a02423 	lsr	r2, r3, #8
    e49c:	e5c03004 	strb	r3, [r0, #4]
    e4a0:	e5c02003 	strb	r2, [r0, #3]
    e4a4:	e59f3080 	ldr	r3, [pc, #128]	; e52c <procsend+0x230>
    e4a8:	e3a02001 	mov	r2, #1	; 0x1
    e4ac:	e5c32000 	strb	r2, [r3]
    e4b0:	e3a00000 	mov	r0, #0	; 0x0
    e4b4:	ea00000f 	b	e4f8 <procsend+0x1fc>
    e4b8:	e59f2070 	ldr	r2, [pc, #112]	; e530 <procsend+0x234>
    e4bc:	e5943000 	ldr	r3, [r4]
    e4c0:	e5922000 	ldr	r2, [r2]
    e4c4:	e1520003 	cmp	r2, r3
    e4c8:	8a00000a 	bhi	e4f8 <procsend+0x1fc>
    e4cc:	e59f3060 	ldr	r3, [pc, #96]	; e534 <procsend+0x238>
    e4d0:	e59f2044 	ldr	r2, [pc, #68]	; e51c <procsend+0x220>
    e4d4:	e5d33000 	ldrb	r3, [r3]
    e4d8:	e1a00006 	mov	r0, r6
    e4dc:	e3530000 	cmp	r3, #0	; 0x0
    e4e0:	13a03001 	movne	r3, #1	; 0x1
    e4e4:	03a03002 	moveq	r3, #2	; 0x2
    e4e8:	e5c23000 	strb	r3, [r2]
    e4ec:	e1a0200c 	mov	r2, ip
    e4f0:	ebffff07 	bl	e114 <Procpacket>
    e4f4:	e6ef0070 	uxtb	r0, r0
    e4f8:	e28dd004 	add	sp, sp, #4	; 0x4
    e4fc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    e500:	0800d150 	.word	0x0800d150
    e504:	0800dc98 	.word	0x0800dc98
    e508:	0800d768 	.word	0x0800d768
    e50c:	0800d764 	.word	0x0800d764
    e510:	0800dc9c 	.word	0x0800dc9c
    e514:	0800dca0 	.word	0x0800dca0
    e518:	000126e2 	.word	0x000126e2
    e51c:	0800d2ba 	.word	0x0800d2ba
    e520:	0800d760 	.word	0x0800d760
    e524:	000121be 	.word	0x000121be
    e528:	000105bc 	.word	0x000105bc
    e52c:	0800d2b9 	.word	0x0800d2b9
    e530:	0800d2bc 	.word	0x0800d2bc
    e534:	0800d2c5 	.word	0x0800d2c5

0000e538 <CYGACC_COMM_IF_PUTC>:
    e538:	e1a00001 	mov	r0, r1
    e53c:	eafffaf7 	b	d120 <putc>

0000e540 <real_send>:
    e540:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    e544:	e59f30ec 	ldr	r3, [pc, #236]	; e638 <real_send+0xf8>
    e548:	e24ddb01 	sub	sp, sp, #1024	; 0x400
    e54c:	e5d33000 	ldrb	r3, [r3]
    e550:	e24dd00c 	sub	sp, sp, #12	; 0xc
    e554:	e3530000 	cmp	r3, #0	; 0x0
    e558:	e1a0e000 	mov	lr, r0
    e55c:	e1a0c001 	mov	ip, r1
    e560:	159f30d4 	ldrne	r3, [pc, #212]	; e63c <real_send+0xfc>
    e564:	13a02000 	movne	r2, #0	; 0x0
    e568:	15832000 	strne	r2, [r3]
    e56c:	1a000006 	bne	e58c <real_send+0x4c>
    e570:	e59f30c8 	ldr	r3, [pc, #200]	; e640 <real_send+0x100>
    e574:	e5d33000 	ldrb	r3, [r3]
    e578:	e3530000 	cmp	r3, #0	; 0x0
    e57c:	059f20b8 	ldreq	r2, [pc, #184]	; e63c <real_send+0xfc>
    e580:	05923000 	ldreq	r3, [r2]
    e584:	02833001 	addeq	r3, r3, #1	; 0x1
    e588:	05823000 	streq	r3, [r2]
    e58c:	e28d0008 	add	r0, sp, #8	; 0x8
    e590:	e2400005 	sub	r0, r0, #5	; 0x5
    e594:	e1a0100e 	mov	r1, lr
    e598:	e1a0200c 	mov	r2, ip
    e59c:	ebffff56 	bl	e2fc <procsend>
    e5a0:	e2507000 	subs	r7, r0, #0	; 0x0
    e5a4:	1a00001f 	bne	e628 <real_send+0xe8>
    e5a8:	e59f3094 	ldr	r3, [pc, #148]	; e644 <real_send+0x104>
    e5ac:	e28d5008 	add	r5, sp, #8	; 0x8
    e5b0:	e5d31000 	ldrb	r1, [r3]
    e5b4:	e59f308c 	ldr	r3, [pc, #140]	; e648 <real_send+0x108>
    e5b8:	e3510001 	cmp	r1, #1	; 0x1
    e5bc:	e5d32000 	ldrb	r2, [r3]
    e5c0:	e3003404 	movw	r3, #1028	; 0x404
    e5c4:	11a06003 	movne	r6, r3
    e5c8:	03a06084 	moveq	r6, #132	; 0x84
    e5cc:	e3520000 	cmp	r2, #0	; 0x0
    e5d0:	12866001 	addne	r6, r6, #1	; 0x1
    e5d4:	e2455005 	sub	r5, r5, #5	; 0x5
    e5d8:	e3a04000 	mov	r4, #0	; 0x0
    e5dc:	e7d50004 	ldrb	r0, [r5, r4]
    e5e0:	e2844001 	add	r4, r4, #1	; 0x1
    e5e4:	ebfffacd 	bl	d120 <putc>
    e5e8:	e1540006 	cmp	r4, r6
    e5ec:	3afffffa 	bcc	e5dc <real_send+0x9c>
    e5f0:	e59f3054 	ldr	r3, [pc, #84]	; e64c <real_send+0x10c>
    e5f4:	e5d33000 	ldrb	r3, [r3]
    e5f8:	e3530000 	cmp	r3, #0	; 0x0
    e5fc:	13e07000 	mvnne	r7, #0	; 0x0
    e600:	1a000008 	bne	e628 <real_send+0xe8>
    e604:	e59f3030 	ldr	r3, [pc, #48]	; e63c <real_send+0xfc>
    e608:	e5932000 	ldr	r2, [r3]
    e60c:	e59f303c 	ldr	r3, [pc, #60]	; e650 <real_send+0x110>
    e610:	e5933000 	ldr	r3, [r3]
    e614:	e1520003 	cmp	r2, r3
    e618:	059f3034 	ldreq	r3, [pc, #52]	; e654 <real_send+0x114>
    e61c:	03a02001 	moveq	r2, #1	; 0x1
    e620:	05c32000 	strbeq	r2, [r3]
    e624:	03a07003 	moveq	r7, #3	; 0x3
    e628:	e1a00007 	mov	r0, r7
    e62c:	e28dd00c 	add	sp, sp, #12	; 0xc
    e630:	e28ddb01 	add	sp, sp, #1024	; 0x400
    e634:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    e638:	0800d2c5 	.word	0x0800d2c5
    e63c:	0800d2bc 	.word	0x0800d2bc
    e640:	0800d150 	.word	0x0800d150
    e644:	0800d2ba 	.word	0x0800d2ba
    e648:	0800d760 	.word	0x0800d760
    e64c:	0800d2c4 	.word	0x0800d2c4
    e650:	0800dc98 	.word	0x0800dc98
    e654:	0800d2c0 	.word	0x0800d2c0

0000e658 <CYGACC_COMM_IF_GETC_TIMEOUT>:
    e658:	e92d4070 	push	{r4, r5, r6, lr}
    e65c:	e1a05001 	mov	r5, r1
    e660:	e3a04000 	mov	r4, #0	; 0x0
    e664:	e59f603c 	ldr	r6, [pc, #60]	; e6a8 <CYGACC_COMM_IF_GETC_TIMEOUT+0x50>
    e668:	ea000000 	b	e670 <CYGACC_COMM_IF_GETC_TIMEOUT+0x18>
    e66c:	ebffe8e9 	bl	8a18 <udelay>
    e670:	ebfffaab 	bl	d124 <tstc>
    e674:	e3500000 	cmp	r0, #0	; 0x0
    e678:	e3a00014 	mov	r0, #20	; 0x14
    e67c:	1a000002 	bne	e68c <CYGACC_COMM_IF_GETC_TIMEOUT+0x34>
    e680:	e1540006 	cmp	r4, r6
    e684:	e2844001 	add	r4, r4, #1	; 0x1
    e688:	1afffff7 	bne	e66c <CYGACC_COMM_IF_GETC_TIMEOUT+0x14>
    e68c:	ebfffaa4 	bl	d124 <tstc>
    e690:	e3500000 	cmp	r0, #0	; 0x0
    e694:	08bd8070 	popeq	{r4, r5, r6, pc}
    e698:	ebfffaa2 	bl	d128 <getc>
    e69c:	e5c50000 	strb	r0, [r5]
    e6a0:	e3a00001 	mov	r0, #1	; 0x1
    e6a4:	e8bd8070 	pop	{r4, r5, r6, pc}
    e6a8:	000186a0 	.word	0x000186a0

0000e6ac <control_send>:
    e6ac:	e59f323c 	ldr	r3, [pc, #572]	; e8f0 <control_send+0x244>
    e6b0:	e3a02000 	mov	r2, #0	; 0x0
    e6b4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    e6b8:	e5c32000 	strb	r2, [r3]
    e6bc:	e59f3230 	ldr	r3, [pc, #560]	; e8f4 <control_send+0x248>
    e6c0:	e59f4230 	ldr	r4, [pc, #560]	; e8f8 <control_send+0x24c>
    e6c4:	e5c32000 	strb	r2, [r3]
    e6c8:	e59f322c 	ldr	r3, [pc, #556]	; e8fc <control_send+0x250>
    e6cc:	e59f522c 	ldr	r5, [pc, #556]	; e900 <control_send+0x254>
    e6d0:	e5832000 	str	r2, [r3]
    e6d4:	e3a03001 	mov	r3, #1	; 0x1
    e6d8:	e59fe224 	ldr	lr, [pc, #548]	; e904 <control_send+0x258>
    e6dc:	e59f8224 	ldr	r8, [pc, #548]	; e908 <control_send+0x25c>
    e6e0:	e59f6224 	ldr	r6, [pc, #548]	; e90c <control_send+0x260>
    e6e4:	e59f7224 	ldr	r7, [pc, #548]	; e910 <control_send+0x264>
    e6e8:	e59fc224 	ldr	ip, [pc, #548]	; e914 <control_send+0x268>
    e6ec:	e5c43000 	strb	r3, [r4]
    e6f0:	e59f3220 	ldr	r3, [pc, #544]	; e918 <control_send+0x26c>
    e6f4:	e24dd014 	sub	sp, sp, #20	; 0x14
    e6f8:	e5c52000 	strb	r2, [r5]
    e6fc:	e5c32000 	strb	r2, [r3]
    e700:	e58d0004 	str	r0, [sp, #4]
    e704:	e1a0b001 	mov	fp, r1
    e708:	e5c82000 	strb	r2, [r8]
    e70c:	e5c62000 	strb	r2, [r6]
    e710:	e5c72000 	strb	r2, [r7]
    e714:	e5ce2000 	strb	r2, [lr]
    e718:	e5cc2000 	strb	r2, [ip]
    e71c:	e1a0a002 	mov	sl, r2
    e720:	e1a0900e 	mov	r9, lr
    e724:	e59f51f0 	ldr	r5, [pc, #496]	; e91c <control_send+0x270>
    e728:	ea000000 	b	e730 <control_send+0x84>
    e72c:	e3a0a000 	mov	sl, #0	; 0x0
    e730:	e3a00000 	mov	r0, #0	; 0x0
    e734:	e28d100f 	add	r1, sp, #15	; 0xf
    e738:	ebffffc6 	bl	e658 <CYGACC_COMM_IF_GETC_TIMEOUT>
    e73c:	e3500000 	cmp	r0, #0	; 0x0
    e740:	0afffffa 	beq	e730 <control_send+0x84>
    e744:	e5dd300f 	ldrb	r3, [sp, #15]
    e748:	e3530018 	cmp	r3, #24	; 0x18
    e74c:	0a000042 	beq	e85c <control_send+0x1b0>
    e750:	8a000004 	bhi	e768 <control_send+0xbc>
    e754:	e3530006 	cmp	r3, #6	; 0x6
    e758:	0a00002c 	beq	e810 <control_send+0x164>
    e75c:	e3530015 	cmp	r3, #21	; 0x15
    e760:	1afffff1 	bne	e72c <control_send+0x80>
    e764:	ea000010 	b	e7ac <control_send+0x100>
    e768:	e3530043 	cmp	r3, #67	; 0x43
    e76c:	0a000001 	beq	e778 <control_send+0xcc>
    e770:	e3530063 	cmp	r3, #99	; 0x63
    e774:	1affffec 	bne	e72c <control_send+0x80>
    e778:	e5d73000 	ldrb	r3, [r7]
    e77c:	e5d42000 	ldrb	r2, [r4]
    e780:	e5d61000 	ldrb	r1, [r6]
    e784:	e1833002 	orr	r3, r3, r2
    e788:	e1913003 	orrs	r3, r1, r3
    e78c:	0affffe6 	beq	e72c <control_send+0x80>
    e790:	e3510000 	cmp	r1, #0	; 0x0
    e794:	159f2178 	ldrne	r2, [pc, #376]	; e914 <control_send+0x268>
    e798:	13a03001 	movne	r3, #1	; 0x1
    e79c:	15c23000 	strbne	r3, [r2]
    e7a0:	e3a03001 	mov	r3, #1	; 0x1
    e7a4:	e5c53000 	strb	r3, [r5]
    e7a8:	ea00002f 	b	e86c <control_send+0x1c0>
    e7ac:	e59f313c 	ldr	r3, [pc, #316]	; e8f0 <control_send+0x244>
    e7b0:	e5d33000 	ldrb	r3, [r3]
    e7b4:	e3530000 	cmp	r3, #0	; 0x0
    e7b8:	0a000008 	beq	e7e0 <control_send+0x134>
    e7bc:	e59f2138 	ldr	r2, [pc, #312]	; e8fc <control_send+0x250>
    e7c0:	e5923000 	ldr	r3, [r2]
    e7c4:	e2433001 	sub	r3, r3, #1	; 0x1
    e7c8:	e5823000 	str	r3, [r2]
    e7cc:	e3a03001 	mov	r3, #1	; 0x1
    e7d0:	e5c43000 	strb	r3, [r4]
    e7d4:	e3a03000 	mov	r3, #0	; 0x0
    e7d8:	e5c93000 	strb	r3, [r9]
    e7dc:	ea000022 	b	e86c <control_send+0x1c0>
    e7e0:	e5d42000 	ldrb	r2, [r4]
    e7e4:	e3520000 	cmp	r2, #0	; 0x0
    e7e8:	15c53000 	strbne	r3, [r5]
    e7ec:	1a000007 	bne	e810 <control_send+0x164>
    e7f0:	e5d93000 	ldrb	r3, [r9]
    e7f4:	e3530000 	cmp	r3, #0	; 0x0
    e7f8:	15c92000 	strbne	r2, [r9]
    e7fc:	e59f20f8 	ldr	r2, [pc, #248]	; e8fc <control_send+0x250>
    e800:	e5923000 	ldr	r3, [r2]
    e804:	e2433001 	sub	r3, r3, #1	; 0x1
    e808:	e5823000 	str	r3, [r2]
    e80c:	ea000016 	b	e86c <control_send+0x1c0>
    e810:	e5d42000 	ldrb	r2, [r4]
    e814:	e3520000 	cmp	r2, #0	; 0x0
    e818:	1affffc3 	bne	e72c <control_send+0x80>
    e81c:	e5d91000 	ldrb	r1, [r9]
    e820:	e3510000 	cmp	r1, #0	; 0x0
    e824:	0a000005 	beq	e840 <control_send+0x194>
    e828:	e59f30c0 	ldr	r3, [pc, #192]	; e8f0 <control_send+0x244>
    e82c:	e1a0a002 	mov	sl, r2
    e830:	e5c32000 	strb	r2, [r3]
    e834:	e3a03001 	mov	r3, #1	; 0x1
    e838:	e5c73000 	strb	r3, [r7]
    e83c:	eaffffbb 	b	e730 <control_send+0x84>
    e840:	e5d83000 	ldrb	r3, [r8]
    e844:	e3530000 	cmp	r3, #0	; 0x0
    e848:	13a03001 	movne	r3, #1	; 0x1
    e84c:	15c63000 	strbne	r3, [r6]
    e850:	11a0a001 	movne	sl, r1
    e854:	1affffb5 	bne	e730 <control_send+0x84>
    e858:	ea000003 	b	e86c <control_send+0x1c0>
    e85c:	e28aa001 	add	sl, sl, #1	; 0x1
    e860:	e35a0002 	cmp	sl, #2	; 0x2
    e864:	1affffb1 	bne	e730 <control_send+0x84>
    e868:	ea00001d 	b	e8e4 <control_send+0x238>
    e86c:	e59f20a0 	ldr	r2, [pc, #160]	; e914 <control_send+0x268>
    e870:	e5d23000 	ldrb	r3, [r2]
    e874:	e3530000 	cmp	r3, #0	; 0x0
    e878:	1a000008 	bne	e8a0 <control_send+0x1f4>
    e87c:	e59f207c 	ldr	r2, [pc, #124]	; e900 <control_send+0x254>
    e880:	e5d23000 	ldrb	r3, [r2]
    e884:	e3530000 	cmp	r3, #0	; 0x0
    e888:	0a000004 	beq	e8a0 <control_send+0x1f4>
    e88c:	e3a00004 	mov	r0, #4	; 0x4
    e890:	ebfffa22 	bl	d120 <putc>
    e894:	e3a03001 	mov	r3, #1	; 0x1
    e898:	e5c83000 	strb	r3, [r8]
    e89c:	eaffffa2 	b	e72c <control_send+0x80>
    e8a0:	e59d0004 	ldr	r0, [sp, #4]
    e8a4:	e1a0100b 	mov	r1, fp
    e8a8:	ebffff24 	bl	e540 <real_send>
    e8ac:	e3500003 	cmp	r0, #3	; 0x3
    e8b0:	0affff9d 	beq	e72c <control_send+0x80>
    e8b4:	e3500000 	cmp	r0, #0	; 0x0
    e8b8:	1a00000a 	bne	e8e8 <control_send+0x23c>
    e8bc:	e5d93000 	ldrb	r3, [r9]
    e8c0:	e3530000 	cmp	r3, #0	; 0x0
    e8c4:	e5d43000 	ldrb	r3, [r4]
    e8c8:	15c90000 	strbne	r0, [r9]
    e8cc:	e3530000 	cmp	r3, #0	; 0x0
    e8d0:	13a03000 	movne	r3, #0	; 0x0
    e8d4:	15c43000 	strbne	r3, [r4]
    e8d8:	12833001 	addne	r3, r3, #1	; 0x1
    e8dc:	15c93000 	strbne	r3, [r9]
    e8e0:	eaffff91 	b	e72c <control_send+0x80>
    e8e4:	e3a00000 	mov	r0, #0	; 0x0
    e8e8:	e28dd014 	add	sp, sp, #20	; 0x14
    e8ec:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    e8f0:	0800d2b9 	.word	0x0800d2b9
    e8f4:	0800d2ba 	.word	0x0800d2ba
    e8f8:	0800d150 	.word	0x0800d150
    e8fc:	0800d2bc 	.word	0x0800d2bc
    e900:	0800d2c0 	.word	0x0800d2c0
    e904:	0800d2c3 	.word	0x0800d2c3
    e908:	0800d2b8 	.word	0x0800d2b8
    e90c:	0800d2c1 	.word	0x0800d2c1
    e910:	0800d2c2 	.word	0x0800d2c2
    e914:	0800d2c5 	.word	0x0800d2c5
    e918:	0800d2c4 	.word	0x0800d2c4
    e91c:	0800d760 	.word	0x0800d760

0000e920 <xyzModem_stream_terminate>:
    e920:	e92d4030 	push	{r4, r5, lr}
    e924:	e3500000 	cmp	r0, #0	; 0x0
    e928:	e24dd00c 	sub	sp, sp, #12	; 0xc
    e92c:	e1a04001 	mov	r4, r1
    e930:	0a000020 	beq	e9b8 <xyzModem_stream_terminate+0x98>
    e934:	e59f4098 	ldr	r4, [pc, #152]	; e9d4 <xyzModem_stream_terminate+0xb4>
    e938:	e5943414 	ldr	r3, [r4, #1044]
    e93c:	e2433001 	sub	r3, r3, #1	; 0x1
    e940:	e3530001 	cmp	r3, #1	; 0x1
    e944:	8a000020 	bhi	e9cc <xyzModem_stream_terminate+0xac>
    e948:	e3a00018 	mov	r0, #24	; 0x18
    e94c:	ebfff9f3 	bl	d120 <putc>
    e950:	e3a00018 	mov	r0, #24	; 0x18
    e954:	ebfff9f1 	bl	d120 <putc>
    e958:	e3a00018 	mov	r0, #24	; 0x18
    e95c:	ebfff9ef 	bl	d120 <putc>
    e960:	e3a00018 	mov	r0, #24	; 0x18
    e964:	ebfff9ed 	bl	d120 <putc>
    e968:	e3a00008 	mov	r0, #8	; 0x8
    e96c:	ebfff9eb 	bl	d120 <putc>
    e970:	e3a00008 	mov	r0, #8	; 0x8
    e974:	ebfff9e9 	bl	d120 <putc>
    e978:	e3a00008 	mov	r0, #8	; 0x8
    e97c:	ebfff9e7 	bl	d120 <putc>
    e980:	e3a00008 	mov	r0, #8	; 0x8
    e984:	ebfff9e5 	bl	d120 <putc>
    e988:	e1a05004 	mov	r5, r4
    e98c:	e28d4007 	add	r4, sp, #7	; 0x7
    e990:	e5953000 	ldr	r3, [r5]
    e994:	e1a01004 	mov	r1, r4
    e998:	e5d30000 	ldrb	r0, [r3]
    e99c:	ebffff2d 	bl	e658 <CYGACC_COMM_IF_GETC_TIMEOUT>
    e9a0:	e3500000 	cmp	r0, #0	; 0x0
    e9a4:	1afffff9 	bne	e990 <xyzModem_stream_terminate+0x70>
    e9a8:	e59f3024 	ldr	r3, [pc, #36]	; e9d4 <xyzModem_stream_terminate+0xb4>
    e9ac:	e3a02001 	mov	r2, #1	; 0x1
    e9b0:	e583242c 	str	r2, [r3, #1068]
    e9b4:	ea000004 	b	e9cc <xyzModem_stream_terminate+0xac>
    e9b8:	e12fff34 	blx	r4
    e9bc:	e3500000 	cmp	r0, #0	; 0x0
    e9c0:	aafffffc 	bge	e9b8 <xyzModem_stream_terminate+0x98>
    e9c4:	e59f000c 	ldr	r0, [pc, #12]	; e9d8 <xyzModem_stream_terminate+0xb8>
    e9c8:	ebffe812 	bl	8a18 <udelay>
    e9cc:	e28dd00c 	add	sp, sp, #12	; 0xc
    e9d0:	e8bd8030 	pop	{r4, r5, pc}
    e9d4:	0800d2c8 	.word	0x0800d2c8
    e9d8:	0003d090 	.word	0x0003d090

0000e9dc <xyzModem_get_hdr>:
    e9dc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    e9e0:	e59f42a0 	ldr	r4, [pc, #672]	; ec88 <xyzModem_get_hdr+0x2ac>
    e9e4:	e24dd00c 	sub	sp, sp, #12	; 0xc
    e9e8:	e5943430 	ldr	r3, [r4, #1072]
    e9ec:	e3530000 	cmp	r3, #0	; 0x0
    e9f0:	0a000003 	beq	ea04 <xyzModem_get_hdr+0x28>
    e9f4:	e3a00006 	mov	r0, #6	; 0x6
    e9f8:	ebfff9c8 	bl	d120 <putc>
    e9fc:	e3a03000 	mov	r3, #0	; 0x0
    ea00:	e5843430 	str	r3, [r4, #1072]
    ea04:	e3a04000 	mov	r4, #0	; 0x0
    ea08:	e59f6278 	ldr	r6, [pc, #632]	; ec88 <xyzModem_get_hdr+0x2ac>
    ea0c:	e1a05004 	mov	r5, r4
    ea10:	e28d7007 	add	r7, sp, #7	; 0x7
    ea14:	e5963000 	ldr	r3, [r6]
    ea18:	e1a01007 	mov	r1, r7
    ea1c:	e5d30000 	ldrb	r0, [r3]
    ea20:	ebffff0c 	bl	e658 <CYGACC_COMM_IF_GETC_TIMEOUT>
    ea24:	e3500000 	cmp	r0, #0	; 0x0
    ea28:	e2855001 	add	r5, r5, #1	; 0x1
    ea2c:	e59f2254 	ldr	r2, [pc, #596]	; ec88 <xyzModem_get_hdr+0x2ac>
    ea30:	0a000021 	beq	eabc <xyzModem_get_hdr+0xe0>
    ea34:	e5dd3007 	ldrb	r3, [sp, #7]
    ea38:	e3530002 	cmp	r3, #2	; 0x2
    ea3c:	0a00000c 	beq	ea74 <xyzModem_get_hdr+0x98>
    ea40:	8a000002 	bhi	ea50 <xyzModem_get_hdr+0x74>
    ea44:	e3530001 	cmp	r3, #1	; 0x1
    ea48:	1afffff1 	bne	ea14 <xyzModem_get_hdr+0x38>
    ea4c:	ea000004 	b	ea64 <xyzModem_get_hdr+0x88>
    ea50:	e3530004 	cmp	r3, #4	; 0x4
    ea54:	0a000012 	beq	eaa4 <xyzModem_get_hdr+0xc8>
    ea58:	e3530018 	cmp	r3, #24	; 0x18
    ea5c:	1affffec 	bne	ea14 <xyzModem_get_hdr+0x38>
    ea60:	ea000007 	b	ea84 <xyzModem_get_hdr+0xa8>
    ea64:	e592341c 	ldr	r3, [r2, #1052]
    ea68:	e2833001 	add	r3, r3, #1	; 0x1
    ea6c:	e582341c 	str	r3, [r2, #1052]
    ea70:	ea00007c 	b	ec68 <xyzModem_get_hdr+0x28c>
    ea74:	e5923420 	ldr	r3, [r2, #1056]
    ea78:	e2833001 	add	r3, r3, #1	; 0x1
    ea7c:	e5823420 	str	r3, [r2, #1056]
    ea80:	ea000078 	b	ec68 <xyzModem_get_hdr+0x28c>
    ea84:	e5963424 	ldr	r3, [r6, #1060]
    ea88:	e2844001 	add	r4, r4, #1	; 0x1
    ea8c:	e2833001 	add	r3, r3, #1	; 0x1
    ea90:	e3540003 	cmp	r4, #3	; 0x3
    ea94:	e5863424 	str	r3, [r6, #1060]
    ea98:	1affffdd 	bne	ea14 <xyzModem_get_hdr+0x38>
    ea9c:	e3e00004 	mvn	r0, #4	; 0x4
    eaa0:	ea00006e 	b	ec60 <xyzModem_get_hdr+0x284>
    eaa4:	e3550001 	cmp	r5, #1	; 0x1
    eaa8:	1affffd9 	bne	ea14 <xyzModem_get_hdr+0x38>
    eaac:	e3a00006 	mov	r0, #6	; 0x6
    eab0:	ebfff99a 	bl	d120 <putc>
    eab4:	e3e00003 	mvn	r0, #3	; 0x3
    eab8:	ea000068 	b	ec60 <xyzModem_get_hdr+0x284>
    eabc:	e1a05002 	mov	r5, r2
    eac0:	e28d4006 	add	r4, sp, #6	; 0x6
    eac4:	e5953000 	ldr	r3, [r5]
    eac8:	e1a01004 	mov	r1, r4
    eacc:	e5d30000 	ldrb	r0, [r3]
    ead0:	ebfffee0 	bl	e658 <CYGACC_COMM_IF_GETC_TIMEOUT>
    ead4:	e3500000 	cmp	r0, #0	; 0x0
    ead8:	1afffff9 	bne	eac4 <xyzModem_get_hdr+0xe8>
    eadc:	e59f01a8 	ldr	r0, [pc, #424]	; ec8c <xyzModem_get_hdr+0x2b0>
    eae0:	ebffe7cc 	bl	8a18 <udelay>
    eae4:	ea00005a 	b	ec54 <xyzModem_get_hdr+0x278>
    eae8:	e5943000 	ldr	r3, [r4]
    eaec:	e59f119c 	ldr	r1, [pc, #412]	; ec90 <xyzModem_get_hdr+0x2b4>
    eaf0:	e5d30000 	ldrb	r0, [r3]
    eaf4:	ebfffed7 	bl	e658 <CYGACC_COMM_IF_GETC_TIMEOUT>
    eaf8:	e3500000 	cmp	r0, #0	; 0x0
    eafc:	0a000054 	beq	ec54 <xyzModem_get_hdr+0x278>
    eb00:	e28d5008 	add	r5, sp, #8	; 0x8
    eb04:	e5753001 	ldrb	r3, [r5, #-1]!
    eb08:	e2842004 	add	r2, r4, #4	; 0x4
    eb0c:	e3530001 	cmp	r3, #1	; 0x1
    eb10:	03a03080 	moveq	r3, #128	; 0x80
    eb14:	13a03b01 	movne	r3, #1024	; 0x400
    eb18:	e5843410 	str	r3, [r4, #1040]
    eb1c:	e5842404 	str	r2, [r4, #1028]
    eb20:	e1a07004 	mov	r7, r4
    eb24:	e3a04000 	mov	r4, #0	; 0x0
    eb28:	ea000008 	b	eb50 <xyzModem_get_hdr+0x174>
    eb2c:	e5973000 	ldr	r3, [r7]
    eb30:	e5d30000 	ldrb	r0, [r3]
    eb34:	ebfffec7 	bl	e658 <CYGACC_COMM_IF_GETC_TIMEOUT>
    eb38:	e3500000 	cmp	r0, #0	; 0x0
    eb3c:	e0872004 	add	r2, r7, r4
    eb40:	e2844001 	add	r4, r4, #1	; 0x1
    eb44:	0a000042 	beq	ec54 <xyzModem_get_hdr+0x278>
    eb48:	e5dd3007 	ldrb	r3, [sp, #7]
    eb4c:	e5c23004 	strb	r3, [r2, #4]
    eb50:	e5973410 	ldr	r3, [r7, #1040]
    eb54:	e1a01005 	mov	r1, r5
    eb58:	e1540003 	cmp	r4, r3
    eb5c:	e59f6124 	ldr	r6, [pc, #292]	; ec88 <xyzModem_get_hdr+0x2ac>
    eb60:	bafffff1 	blt	eb2c <xyzModem_get_hdr+0x150>
    eb64:	e5963000 	ldr	r3, [r6]
    eb68:	e59f1124 	ldr	r1, [pc, #292]	; ec94 <xyzModem_get_hdr+0x2b8>
    eb6c:	e5d30000 	ldrb	r0, [r3]
    eb70:	ebfffeb8 	bl	e658 <CYGACC_COMM_IF_GETC_TIMEOUT>
    eb74:	e3500000 	cmp	r0, #0	; 0x0
    eb78:	0a000035 	beq	ec54 <xyzModem_get_hdr+0x278>
    eb7c:	e5963428 	ldr	r3, [r6, #1064]
    eb80:	e3530000 	cmp	r3, #0	; 0x0
    eb84:	0a000005 	beq	eba0 <xyzModem_get_hdr+0x1c4>
    eb88:	e5963000 	ldr	r3, [r6]
    eb8c:	e59f1104 	ldr	r1, [pc, #260]	; ec98 <xyzModem_get_hdr+0x2bc>
    eb90:	e5d30000 	ldrb	r0, [r3]
    eb94:	ebfffeaf 	bl	e658 <CYGACC_COMM_IF_GETC_TIMEOUT>
    eb98:	e3500000 	cmp	r0, #0	; 0x0
    eb9c:	0a00002c 	beq	ec54 <xyzModem_get_hdr+0x278>
    eba0:	e59f40e0 	ldr	r4, [pc, #224]	; ec88 <xyzModem_get_hdr+0x2ac>
    eba4:	e5d43409 	ldrb	r3, [r4, #1033]
    eba8:	e5d42408 	ldrb	r2, [r4, #1032]
    ebac:	e0233002 	eor	r3, r3, r2
    ebb0:	e35300ff 	cmp	r3, #255	; 0xff
    ebb4:	0a000009 	beq	ebe0 <xyzModem_get_hdr+0x204>
    ebb8:	e1a05004 	mov	r5, r4
    ebbc:	e28d4006 	add	r4, sp, #6	; 0x6
    ebc0:	e5953000 	ldr	r3, [r5]
    ebc4:	e1a01004 	mov	r1, r4
    ebc8:	e5d30000 	ldrb	r0, [r3]
    ebcc:	ebfffea1 	bl	e658 <CYGACC_COMM_IF_GETC_TIMEOUT>
    ebd0:	e3500000 	cmp	r0, #0	; 0x0
    ebd4:	1afffff9 	bne	ebc0 <xyzModem_get_hdr+0x1e4>
    ebd8:	e3e00005 	mvn	r0, #5	; 0x5
    ebdc:	ea00001f 	b	ec60 <xyzModem_get_hdr+0x284>
    ebe0:	e5943428 	ldr	r3, [r4, #1064]
    ebe4:	e3530000 	cmp	r3, #0	; 0x0
    ebe8:	01a02003 	moveq	r2, r3
    ebec:	01a0c004 	moveq	ip, r4
    ebf0:	05940410 	ldreq	r0, [r4, #1040]
    ebf4:	01a01003 	moveq	r1, r3
    ebf8:	0a00000a 	beq	ec28 <xyzModem_get_hdr+0x24c>
    ebfc:	e2840004 	add	r0, r4, #4	; 0x4
    ec00:	e5941410 	ldr	r1, [r4, #1040]
    ec04:	eb000110 	bl	f04c <cyg_crc16>
    ec08:	e5d4340b 	ldrb	r3, [r4, #1035]
    ec0c:	e5d4240a 	ldrb	r2, [r4, #1034]
    ec10:	e1833402 	orr	r3, r3, r2, lsl #8
    ec14:	e1500003 	cmp	r0, r3
    ec18:	ea00000a 	b	ec48 <xyzModem_get_hdr+0x26c>
    ec1c:	e5d33004 	ldrb	r3, [r3, #4]
    ec20:	e0813003 	add	r3, r1, r3
    ec24:	e6ff1073 	uxth	r1, r3
    ec28:	e1520000 	cmp	r2, r0
    ec2c:	e08c3002 	add	r3, ip, r2
    ec30:	e2822001 	add	r2, r2, #1	; 0x1
    ec34:	bafffff8 	blt	ec1c <xyzModem_get_hdr+0x240>
    ec38:	e59f3048 	ldr	r3, [pc, #72]	; ec88 <xyzModem_get_hdr+0x2ac>
    ec3c:	e5d3240a 	ldrb	r2, [r3, #1034]
    ec40:	e20130ff 	and	r3, r1, #255	; 0xff
    ec44:	e1520003 	cmp	r2, r3
    ec48:	13e00006 	mvnne	r0, #6	; 0x6
    ec4c:	1a000003 	bne	ec60 <xyzModem_get_hdr+0x284>
    ec50:	ea000001 	b	ec5c <xyzModem_get_hdr+0x280>
    ec54:	e3e00002 	mvn	r0, #2	; 0x2
    ec58:	ea000000 	b	ec60 <xyzModem_get_hdr+0x284>
    ec5c:	e3a00000 	mov	r0, #0	; 0x0
    ec60:	e28dd00c 	add	sp, sp, #12	; 0xc
    ec64:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    ec68:	e59f4018 	ldr	r4, [pc, #24]	; ec88 <xyzModem_get_hdr+0x2ac>
    ec6c:	e59f1028 	ldr	r1, [pc, #40]	; ec9c <xyzModem_get_hdr+0x2c0>
    ec70:	e5943000 	ldr	r3, [r4]
    ec74:	e5d30000 	ldrb	r0, [r3]
    ec78:	ebfffe76 	bl	e658 <CYGACC_COMM_IF_GETC_TIMEOUT>
    ec7c:	e3500000 	cmp	r0, #0	; 0x0
    ec80:	0afffff3 	beq	ec54 <xyzModem_get_hdr+0x278>
    ec84:	eaffff97 	b	eae8 <xyzModem_get_hdr+0x10c>
    ec88:	0800d2c8 	.word	0x0800d2c8
    ec8c:	0003d090 	.word	0x0003d090
    ec90:	0800d6d1 	.word	0x0800d6d1
    ec94:	0800d6d2 	.word	0x0800d6d2
    ec98:	0800d6d3 	.word	0x0800d6d3
    ec9c:	0800d6d0 	.word	0x0800d6d0

0000eca0 <xyzModem_stream_read>:
    eca0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    eca4:	e59f5240 	ldr	r5, [pc, #576]	; eeec <xyzModem_stream_read+0x24c>
    eca8:	e24dd004 	sub	sp, sp, #4	; 0x4
    ecac:	e1a08000 	mov	r8, r0
    ecb0:	e1a07001 	mov	r7, r1
    ecb4:	e1a0b002 	mov	fp, r2
    ecb8:	e3e06004 	mvn	r6, #4	; 0x4
    ecbc:	e3a0a000 	mov	sl, #0	; 0x0
    ecc0:	e1a09005 	mov	r9, r5
    ecc4:	ea00007e 	b	eec4 <xyzModem_stream_read+0x224>
    ecc8:	e5953410 	ldr	r3, [r5, #1040]
    eccc:	e3530000 	cmp	r3, #0	; 0x0
    ecd0:	03a04014 	moveq	r4, #20	; 0x14
    ecd4:	0a00005d 	beq	ee50 <xyzModem_stream_read+0x1b0>
    ecd8:	ea000065 	b	ee74 <xyzModem_stream_read+0x1d4>
    ecdc:	ebffff3e 	bl	e9dc <xyzModem_get_hdr>
    ece0:	e2506000 	subs	r6, r0, #0	; 0x0
    ece4:	1a000039 	bne	edd0 <xyzModem_stream_read+0x130>
    ece8:	e5d5240c 	ldrb	r2, [r5, #1036]
    ecec:	e5d51408 	ldrb	r1, [r5, #1032]
    ecf0:	e2423001 	sub	r3, r2, #1	; 0x1
    ecf4:	e1510002 	cmp	r1, r2
    ecf8:	e20330ff 	and	r3, r3, #255	; 0xff
    ecfc:	1a00002d 	bne	edb8 <xyzModem_stream_read+0x118>
    ed00:	e599c414 	ldr	ip, [r9, #1044]
    ed04:	e2813001 	add	r3, r1, #1	; 0x1
    ed08:	e5c9340c 	strb	r3, [r9, #1036]
    ed0c:	e35c0001 	cmp	ip, #1	; 0x1
    ed10:	e3a03001 	mov	r3, #1	; 0x1
    ed14:	e5893430 	str	r3, [r9, #1072]
    ed18:	0a000002 	beq	ed28 <xyzModem_stream_read+0x88>
    ed1c:	e5993434 	ldr	r3, [r9, #1076]
    ed20:	e3530000 	cmp	r3, #0	; 0x0
    ed24:	1a000017 	bne	ed88 <xyzModem_stream_read+0xe8>
    ed28:	e5953410 	ldr	r3, [r5, #1040]
    ed2c:	e5950404 	ldr	r0, [r5, #1028]
    ed30:	e0802003 	add	r2, r0, r3
    ed34:	e5523001 	ldrb	r3, [r2, #-1]
    ed38:	e353001a 	cmp	r3, #26	; 0x1a
    ed3c:	1a00000f 	bne	ed80 <xyzModem_stream_read+0xe0>
    ed40:	e5523002 	ldrb	r3, [r2, #-2]
    ed44:	e353001a 	cmp	r3, #26	; 0x1a
    ed48:	1a00000c 	bne	ed80 <xyzModem_stream_read+0xe0>
    ed4c:	e5523003 	ldrb	r3, [r2, #-3]
    ed50:	e353001a 	cmp	r3, #26	; 0x1a
    ed54:	1a000009 	bne	ed80 <xyzModem_stream_read+0xe0>
    ed58:	ea000000 	b	ed60 <xyzModem_stream_read+0xc0>
    ed5c:	e5851410 	str	r1, [r5, #1040]
    ed60:	e5953410 	ldr	r3, [r5, #1040]
    ed64:	e3530000 	cmp	r3, #0	; 0x0
    ed68:	e0802003 	add	r2, r0, r3
    ed6c:	e2431001 	sub	r1, r3, #1	; 0x1
    ed70:	0a000002 	beq	ed80 <xyzModem_stream_read+0xe0>
    ed74:	e5523001 	ldrb	r3, [r2, #-1]
    ed78:	e353001a 	cmp	r3, #26	; 0x1a
    ed7c:	0afffff6 	beq	ed5c <xyzModem_stream_read+0xbc>
    ed80:	e35c0001 	cmp	ip, #1	; 0x1
    ed84:	0a00003a 	beq	ee74 <xyzModem_stream_read+0x1d4>
    ed88:	e5951434 	ldr	r1, [r5, #1076]
    ed8c:	e3510000 	cmp	r1, #0	; 0x0
    ed90:	0a000037 	beq	ee74 <xyzModem_stream_read+0x1d4>
    ed94:	e5953438 	ldr	r3, [r5, #1080]
    ed98:	e5950410 	ldr	r0, [r5, #1040]
    ed9c:	e0802003 	add	r2, r0, r3
    eda0:	e1520001 	cmp	r2, r1
    eda4:	80803001 	addhi	r3, r0, r1
    eda8:	80623003 	rsbhi	r3, r2, r3
    edac:	e5852438 	str	r2, [r5, #1080]
    edb0:	85853410 	strhi	r3, [r5, #1040]
    edb4:	ea00002e 	b	ee74 <xyzModem_stream_read+0x1d4>
    edb8:	e1510003 	cmp	r1, r3
    edbc:	13e06007 	mvnne	r6, #7	; 0x7
    edc0:	1a00001a 	bne	ee30 <xyzModem_stream_read+0x190>
    edc4:	e3a00006 	mov	r0, #6	; 0x6
    edc8:	ebfff8d4 	bl	d120 <putc>
    edcc:	ea00001f 	b	ee50 <xyzModem_stream_read+0x1b0>
    edd0:	e3760005 	cmn	r6, #5	; 0x5
    edd4:	0a000021 	beq	ee60 <xyzModem_stream_read+0x1c0>
    edd8:	e3760004 	cmn	r6, #4	; 0x4
    eddc:	1a000013 	bne	ee30 <xyzModem_stream_read+0x190>
    ede0:	e3a00006 	mov	r0, #6	; 0x6
    ede4:	ebfff8cd 	bl	d120 <putc>
    ede8:	e5953414 	ldr	r3, [r5, #1044]
    edec:	e3530002 	cmp	r3, #2	; 0x2
    edf0:	1a00000b 	bne	ee24 <xyzModem_stream_read+0x184>
    edf4:	e5950428 	ldr	r0, [r5, #1064]
    edf8:	e3500000 	cmp	r0, #0	; 0x0
    edfc:	13a00043 	movne	r0, #67	; 0x43
    ee00:	03a00015 	moveq	r0, #21	; 0x15
    ee04:	ebfff8c5 	bl	d120 <putc>
    ee08:	e5953418 	ldr	r3, [r5, #1048]
    ee0c:	e2833001 	add	r3, r3, #1	; 0x1
    ee10:	e5853418 	str	r3, [r5, #1048]
    ee14:	ebfffef0 	bl	e9dc <xyzModem_get_hdr>
    ee18:	e1a06000 	mov	r6, r0
    ee1c:	e3a00006 	mov	r0, #6	; 0x6
    ee20:	ebfff8be 	bl	d120 <putc>
    ee24:	e3a03001 	mov	r3, #1	; 0x1
    ee28:	e585342c 	str	r3, [r5, #1068]
    ee2c:	ea000009 	b	ee58 <xyzModem_stream_read+0x1b8>
    ee30:	e5950428 	ldr	r0, [r5, #1064]
    ee34:	e3500000 	cmp	r0, #0	; 0x0
    ee38:	13a00043 	movne	r0, #67	; 0x43
    ee3c:	03a00015 	moveq	r0, #21	; 0x15
    ee40:	ebfff8b6 	bl	d120 <putc>
    ee44:	e5953418 	ldr	r3, [r5, #1048]
    ee48:	e2833001 	add	r3, r3, #1	; 0x1
    ee4c:	e5853418 	str	r3, [r5, #1048]
    ee50:	e2544001 	subs	r4, r4, #1	; 0x1
    ee54:	2affffa0 	bcs	ecdc <xyzModem_stream_read+0x3c>
    ee58:	e3560000 	cmp	r6, #0	; 0x0
    ee5c:	aa000004 	bge	ee74 <xyzModem_stream_read+0x1d4>
    ee60:	e59f3084 	ldr	r3, [pc, #132]	; eeec <xyzModem_stream_read+0x24c>
    ee64:	e3e02000 	mvn	r2, #0	; 0x0
    ee68:	e58b6000 	str	r6, [fp]
    ee6c:	e5832410 	str	r2, [r3, #1040]
    ee70:	ea00001a 	b	eee0 <xyzModem_stream_read+0x240>
    ee74:	e595342c 	ldr	r3, [r5, #1068]
    ee78:	e3530000 	cmp	r3, #0	; 0x0
    ee7c:	1a000010 	bne	eec4 <xyzModem_stream_read+0x224>
    ee80:	e5954410 	ldr	r4, [r5, #1040]
    ee84:	e1a00008 	mov	r0, r8
    ee88:	e1570004 	cmp	r7, r4
    ee8c:	b1a04007 	movlt	r4, r7
    ee90:	a1a04004 	movge	r4, r4
    ee94:	e1a02004 	mov	r2, r4
    ee98:	e5951404 	ldr	r1, [r5, #1028]
    ee9c:	ebffc94d 	bl	13d8 <memcpy>
    eea0:	e5953410 	ldr	r3, [r5, #1040]
    eea4:	e5952404 	ldr	r2, [r5, #1028]
    eea8:	e0643003 	rsb	r3, r4, r3
    eeac:	e0822004 	add	r2, r2, r4
    eeb0:	e5853410 	str	r3, [r5, #1040]
    eeb4:	e5852404 	str	r2, [r5, #1028]
    eeb8:	e0647007 	rsb	r7, r4, r7
    eebc:	e0888004 	add	r8, r8, r4
    eec0:	e08aa004 	add	sl, sl, r4
    eec4:	e595342c 	ldr	r3, [r5, #1068]
    eec8:	e2733001 	rsbs	r3, r3, #1	; 0x1
    eecc:	33a03000 	movcc	r3, #0	; 0x0
    eed0:	e3570000 	cmp	r7, #0	; 0x0
    eed4:	d3a03000 	movle	r3, #0	; 0x0
    eed8:	e3530000 	cmp	r3, #0	; 0x0
    eedc:	1affff79 	bne	ecc8 <xyzModem_stream_read+0x28>
    eee0:	e1a0000a 	mov	r0, sl
    eee4:	e28dd004 	add	sp, sp, #4	; 0x4
    eee8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    eeec:	0800d2c8 	.word	0x0800d2c8

0000eef0 <xyzModem_stream_open>:
    eef0:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    eef4:	e59f4140 	ldr	r4, [pc, #320]	; f03c <xyzModem_stream_open+0x14c>
    eef8:	e24dd00c 	sub	sp, sp, #12	; 0xc
    eefc:	e3a05000 	mov	r5, #0	; 0x0
    ef00:	e28d3008 	add	r3, sp, #8	; 0x8
    ef04:	e3a02001 	mov	r2, #1	; 0x1
    ef08:	e5235004 	str	r5, [r3, #-4]!
    ef0c:	e5842428 	str	r2, [r4, #1064]
    ef10:	e5845410 	str	r5, [r4, #1040]
    ef14:	e584542c 	str	r5, [r4, #1068]
    ef18:	e5845430 	str	r5, [r4, #1072]
    ef1c:	e5902004 	ldr	r2, [r0, #4]
    ef20:	e5843000 	str	r3, [r4]
    ef24:	e3a00043 	mov	r0, #67	; 0x43
    ef28:	e5842414 	str	r2, [r4, #1044]
    ef2c:	e5845418 	str	r5, [r4, #1048]
    ef30:	e584541c 	str	r5, [r4, #1052]
    ef34:	e5845420 	str	r5, [r4, #1056]
    ef38:	e5845424 	str	r5, [r4, #1060]
    ef3c:	e5845438 	str	r5, [r4, #1080]
    ef40:	e5845434 	str	r5, [r4, #1076]
    ef44:	e1a08001 	mov	r8, r1
    ef48:	ebfff874 	bl	d120 <putc>
    ef4c:	e5943414 	ldr	r3, [r4, #1044]
    ef50:	e3530001 	cmp	r3, #1	; 0x1
    ef54:	05c4340c 	strbeq	r3, [r4, #1036]
    ef58:	01a00005 	moveq	r0, r5
    ef5c:	0a000034 	beq	f034 <xyzModem_stream_open+0x144>
    ef60:	e1a07004 	mov	r7, r4
    ef64:	e3a06014 	mov	r6, #20	; 0x14
    ef68:	e3a0400a 	mov	r4, #10	; 0xa
    ef6c:	e1a0a005 	mov	sl, r5
    ef70:	ea00002b 	b	f024 <xyzModem_stream_open+0x134>
    ef74:	ebfffe98 	bl	e9dc <xyzModem_get_hdr>
    ef78:	e2505000 	subs	r5, r0, #0	; 0x0
    ef7c:	1a000016 	bne	efdc <xyzModem_stream_open+0xec>
    ef80:	e59f20b4 	ldr	r2, [pc, #180]	; f03c <xyzModem_stream_open+0x14c>
    ef84:	e5d23408 	ldrb	r3, [r2, #1032]
    ef88:	e3530000 	cmp	r3, #0	; 0x0
    ef8c:	1a00000b 	bne	efc0 <xyzModem_stream_open+0xd0>
    ef90:	e1a03002 	mov	r3, r2
    ef94:	e5930404 	ldr	r0, [r3, #1028]
    ef98:	e4d02001 	ldrb	r2, [r0], #1
    ef9c:	e3520000 	cmp	r2, #0	; 0x0
    efa0:	e5830404 	str	r0, [r3, #1028]
    efa4:	1afffffa 	bne	ef94 <xyzModem_stream_open+0xa4>
    efa8:	e59f3090 	ldr	r3, [pc, #144]	; f040 <xyzModem_stream_open+0x150>
    efac:	e59f1090 	ldr	r1, [pc, #144]	; f044 <xyzModem_stream_open+0x154>
    efb0:	ebfffb5a 	bl	dd20 <parse_num>
    efb4:	e59f3080 	ldr	r3, [pc, #128]	; f03c <xyzModem_stream_open+0x14c>
    efb8:	e3a02001 	mov	r2, #1	; 0x1
    efbc:	e5832430 	str	r2, [r3, #1072]
    efc0:	e59f3074 	ldr	r3, [pc, #116]	; f03c <xyzModem_stream_open+0x14c>
    efc4:	e3a01000 	mov	r1, #0	; 0x0
    efc8:	e3a02001 	mov	r2, #1	; 0x1
    efcc:	e5c3240c 	strb	r2, [r3, #1036]
    efd0:	e5831410 	str	r1, [r3, #1040]
    efd4:	e1a00001 	mov	r0, r1
    efd8:	ea000015 	b	f034 <xyzModem_stream_open+0x144>
    efdc:	e3750003 	cmn	r5, #3	; 0x3
    efe0:	1a00000d 	bne	f01c <xyzModem_stream_open+0x12c>
    efe4:	e2444001 	sub	r4, r4, #1	; 0x1
    efe8:	e3540000 	cmp	r4, #0	; 0x0
    efec:	e59f0054 	ldr	r0, [pc, #84]	; f048 <xyzModem_stream_open+0x158>
    eff0:	d587a428 	strle	sl, [r7, #1064]
    eff4:	ebffe687 	bl	8a18 <udelay>
    eff8:	e5970428 	ldr	r0, [r7, #1064]
    effc:	e3500000 	cmp	r0, #0	; 0x0
    f000:	13a00043 	movne	r0, #67	; 0x43
    f004:	03a00015 	moveq	r0, #21	; 0x15
    f008:	ebfff844 	bl	d120 <putc>
    f00c:	e5973418 	ldr	r3, [r7, #1048]
    f010:	e2833001 	add	r3, r3, #1	; 0x1
    f014:	e5873418 	str	r3, [r7, #1048]
    f018:	ea000001 	b	f024 <xyzModem_stream_open+0x134>
    f01c:	e3750005 	cmn	r5, #5	; 0x5
    f020:	0a000001 	beq	f02c <xyzModem_stream_open+0x13c>
    f024:	e2566001 	subs	r6, r6, #1	; 0x1
    f028:	2affffd1 	bcs	ef74 <xyzModem_stream_open+0x84>
    f02c:	e5885000 	str	r5, [r8]
    f030:	e3e00000 	mvn	r0, #0	; 0x0
    f034:	e28dd00c 	add	sp, sp, #12	; 0xc
    f038:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    f03c:	0800d2c8 	.word	0x0800d2c8
    f040:	0001244e 	.word	0x0001244e
    f044:	0800d6fc 	.word	0x0800d6fc
    f048:	0007a120 	.word	0x0007a120

0000f04c <cyg_crc16>:
    f04c:	e3a02000 	mov	r2, #0	; 0x0
    f050:	e92d4010 	push	{r4, lr}
    f054:	e1a0c000 	mov	ip, r0
    f058:	e59f402c 	ldr	r4, [pc, #44]	; f08c <cyg_crc16+0x40>
    f05c:	e1a00002 	mov	r0, r2
    f060:	ea000006 	b	f080 <cyg_crc16+0x34>
    f064:	e7dc3002 	ldrb	r3, [ip, r2]
    f068:	e2822001 	add	r2, r2, #1	; 0x1
    f06c:	e0233420 	eor	r3, r3, r0, lsr #8
    f070:	e1a03083 	lsl	r3, r3, #1
    f074:	e19430b3 	ldrh	r3, [r4, r3]
    f078:	e0233400 	eor	r3, r3, r0, lsl #8
    f07c:	e6ff0073 	uxth	r0, r3
    f080:	e1520001 	cmp	r2, r1
    f084:	bafffff6 	blt	f064 <cyg_crc16+0x18>
    f088:	e8bd8010 	pop	{r4, pc}
    f08c:	000105bc 	.word	0x000105bc

0000f090 <ss_jtag_en>:
    f090:	e59f300c 	ldr	r3, [pc, #12]	; f0a4 <ss_jtag_en+0x14>
    f094:	e2500000 	subs	r0, r0, #0	; 0x0
    f098:	13a00001 	movne	r0, #1	; 0x1
    f09c:	e58308b8 	str	r0, [r3, #2232]
    f0a0:	e12fff1e 	bx	lr
    f0a4:	2d008000 	.word	0x2d008000

0000f0a8 <raise>:
    f0a8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    f0ac:	e1a01000 	mov	r1, r0
    f0b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    f0b4:	e59f0014 	ldr	r0, [pc, #20]	; f0d0 <raise+0x28>
    f0b8:	e59f3014 	ldr	r3, [pc, #20]	; f0d4 <raise+0x2c>
    f0bc:	e1a0e00f 	mov	lr, pc
    f0c0:	e593f000 	ldr	pc, [r3]
    f0c4:	e3a00000 	mov	r0, #0	; 0x0
    f0c8:	e28dd004 	add	sp, sp, #4	; 0x4
    f0cc:	e8bd8000 	pop	{pc}
    f0d0:	000126ea 	.word	0x000126ea
    f0d4:	0800d704 	.word	0x0800d704

0000f0d8 <__aeabi_uidiv>:
    f0d8:	e2512001 	subs	r2, r1, #1	; 0x1
    f0dc:	012fff1e 	bxeq	lr
    f0e0:	3a000074 	bcc	f2b8 <__aeabi_uidiv+0x1e0>
    f0e4:	e1500001 	cmp	r0, r1
    f0e8:	9a00006b 	bls	f29c <__aeabi_uidiv+0x1c4>
    f0ec:	e1110002 	tst	r1, r2
    f0f0:	0a00006c 	beq	f2a8 <__aeabi_uidiv+0x1d0>
    f0f4:	e16f3f10 	clz	r3, r0
    f0f8:	e16f2f11 	clz	r2, r1
    f0fc:	e0423003 	sub	r3, r2, r3
    f100:	e273301f 	rsbs	r3, r3, #31	; 0x1f
    f104:	10833083 	addne	r3, r3, r3, lsl #1
    f108:	e3a02000 	mov	r2, #0	; 0x0
    f10c:	108ff103 	addne	pc, pc, r3, lsl #2
    f110:	e1a00000 	nop			(mov r0,r0)
    f114:	e1500f81 	cmp	r0, r1, lsl #31
    f118:	e0a22002 	adc	r2, r2, r2
    f11c:	20400f81 	subcs	r0, r0, r1, lsl #31
    f120:	e1500f01 	cmp	r0, r1, lsl #30
    f124:	e0a22002 	adc	r2, r2, r2
    f128:	20400f01 	subcs	r0, r0, r1, lsl #30
    f12c:	e1500e81 	cmp	r0, r1, lsl #29
    f130:	e0a22002 	adc	r2, r2, r2
    f134:	20400e81 	subcs	r0, r0, r1, lsl #29
    f138:	e1500e01 	cmp	r0, r1, lsl #28
    f13c:	e0a22002 	adc	r2, r2, r2
    f140:	20400e01 	subcs	r0, r0, r1, lsl #28
    f144:	e1500d81 	cmp	r0, r1, lsl #27
    f148:	e0a22002 	adc	r2, r2, r2
    f14c:	20400d81 	subcs	r0, r0, r1, lsl #27
    f150:	e1500d01 	cmp	r0, r1, lsl #26
    f154:	e0a22002 	adc	r2, r2, r2
    f158:	20400d01 	subcs	r0, r0, r1, lsl #26
    f15c:	e1500c81 	cmp	r0, r1, lsl #25
    f160:	e0a22002 	adc	r2, r2, r2
    f164:	20400c81 	subcs	r0, r0, r1, lsl #25
    f168:	e1500c01 	cmp	r0, r1, lsl #24
    f16c:	e0a22002 	adc	r2, r2, r2
    f170:	20400c01 	subcs	r0, r0, r1, lsl #24
    f174:	e1500b81 	cmp	r0, r1, lsl #23
    f178:	e0a22002 	adc	r2, r2, r2
    f17c:	20400b81 	subcs	r0, r0, r1, lsl #23
    f180:	e1500b01 	cmp	r0, r1, lsl #22
    f184:	e0a22002 	adc	r2, r2, r2
    f188:	20400b01 	subcs	r0, r0, r1, lsl #22
    f18c:	e1500a81 	cmp	r0, r1, lsl #21
    f190:	e0a22002 	adc	r2, r2, r2
    f194:	20400a81 	subcs	r0, r0, r1, lsl #21
    f198:	e1500a01 	cmp	r0, r1, lsl #20
    f19c:	e0a22002 	adc	r2, r2, r2
    f1a0:	20400a01 	subcs	r0, r0, r1, lsl #20
    f1a4:	e1500981 	cmp	r0, r1, lsl #19
    f1a8:	e0a22002 	adc	r2, r2, r2
    f1ac:	20400981 	subcs	r0, r0, r1, lsl #19
    f1b0:	e1500901 	cmp	r0, r1, lsl #18
    f1b4:	e0a22002 	adc	r2, r2, r2
    f1b8:	20400901 	subcs	r0, r0, r1, lsl #18
    f1bc:	e1500881 	cmp	r0, r1, lsl #17
    f1c0:	e0a22002 	adc	r2, r2, r2
    f1c4:	20400881 	subcs	r0, r0, r1, lsl #17
    f1c8:	e1500801 	cmp	r0, r1, lsl #16
    f1cc:	e0a22002 	adc	r2, r2, r2
    f1d0:	20400801 	subcs	r0, r0, r1, lsl #16
    f1d4:	e1500781 	cmp	r0, r1, lsl #15
    f1d8:	e0a22002 	adc	r2, r2, r2
    f1dc:	20400781 	subcs	r0, r0, r1, lsl #15
    f1e0:	e1500701 	cmp	r0, r1, lsl #14
    f1e4:	e0a22002 	adc	r2, r2, r2
    f1e8:	20400701 	subcs	r0, r0, r1, lsl #14
    f1ec:	e1500681 	cmp	r0, r1, lsl #13
    f1f0:	e0a22002 	adc	r2, r2, r2
    f1f4:	20400681 	subcs	r0, r0, r1, lsl #13
    f1f8:	e1500601 	cmp	r0, r1, lsl #12
    f1fc:	e0a22002 	adc	r2, r2, r2
    f200:	20400601 	subcs	r0, r0, r1, lsl #12
    f204:	e1500581 	cmp	r0, r1, lsl #11
    f208:	e0a22002 	adc	r2, r2, r2
    f20c:	20400581 	subcs	r0, r0, r1, lsl #11
    f210:	e1500501 	cmp	r0, r1, lsl #10
    f214:	e0a22002 	adc	r2, r2, r2
    f218:	20400501 	subcs	r0, r0, r1, lsl #10
    f21c:	e1500481 	cmp	r0, r1, lsl #9
    f220:	e0a22002 	adc	r2, r2, r2
    f224:	20400481 	subcs	r0, r0, r1, lsl #9
    f228:	e1500401 	cmp	r0, r1, lsl #8
    f22c:	e0a22002 	adc	r2, r2, r2
    f230:	20400401 	subcs	r0, r0, r1, lsl #8
    f234:	e1500381 	cmp	r0, r1, lsl #7
    f238:	e0a22002 	adc	r2, r2, r2
    f23c:	20400381 	subcs	r0, r0, r1, lsl #7
    f240:	e1500301 	cmp	r0, r1, lsl #6
    f244:	e0a22002 	adc	r2, r2, r2
    f248:	20400301 	subcs	r0, r0, r1, lsl #6
    f24c:	e1500281 	cmp	r0, r1, lsl #5
    f250:	e0a22002 	adc	r2, r2, r2
    f254:	20400281 	subcs	r0, r0, r1, lsl #5
    f258:	e1500201 	cmp	r0, r1, lsl #4
    f25c:	e0a22002 	adc	r2, r2, r2
    f260:	20400201 	subcs	r0, r0, r1, lsl #4
    f264:	e1500181 	cmp	r0, r1, lsl #3
    f268:	e0a22002 	adc	r2, r2, r2
    f26c:	20400181 	subcs	r0, r0, r1, lsl #3
    f270:	e1500101 	cmp	r0, r1, lsl #2
    f274:	e0a22002 	adc	r2, r2, r2
    f278:	20400101 	subcs	r0, r0, r1, lsl #2
    f27c:	e1500081 	cmp	r0, r1, lsl #1
    f280:	e0a22002 	adc	r2, r2, r2
    f284:	20400081 	subcs	r0, r0, r1, lsl #1
    f288:	e1500001 	cmp	r0, r1
    f28c:	e0a22002 	adc	r2, r2, r2
    f290:	20400001 	subcs	r0, r0, r1
    f294:	e1a00002 	mov	r0, r2
    f298:	e12fff1e 	bx	lr
    f29c:	03a00001 	moveq	r0, #1	; 0x1
    f2a0:	13a00000 	movne	r0, #0	; 0x0
    f2a4:	e12fff1e 	bx	lr
    f2a8:	e16f2f11 	clz	r2, r1
    f2ac:	e262201f 	rsb	r2, r2, #31	; 0x1f
    f2b0:	e1a00230 	lsr	r0, r0, r2
    f2b4:	e12fff1e 	bx	lr
    f2b8:	e52de008 	str	lr, [sp, #-8]!
    f2bc:	eb0002c4 	bl	fdd4 <__div0>
    f2c0:	e3a00000 	mov	r0, #0	; 0x0
    f2c4:	e49df008 	ldr	pc, [sp], #8

0000f2c8 <__aeabi_uidivmod>:
    f2c8:	e92d4003 	push	{r0, r1, lr}
    f2cc:	ebffff81 	bl	f0d8 <__aeabi_uidiv>
    f2d0:	e8bd4006 	pop	{r1, r2, lr}
    f2d4:	e0030092 	mul	r3, r2, r0
    f2d8:	e0411003 	sub	r1, r1, r3
    f2dc:	e12fff1e 	bx	lr

0000f2e0 <__aeabi_idiv>:
    f2e0:	e3510000 	cmp	r1, #0	; 0x0
    f2e4:	e020c001 	eor	ip, r0, r1
    f2e8:	0a000080 	beq	f4f0 <__aeabi_idiv+0x210>
    f2ec:	42611000 	rsbmi	r1, r1, #0	; 0x0
    f2f0:	e2512001 	subs	r2, r1, #1	; 0x1
    f2f4:	0a000070 	beq	f4bc <__aeabi_idiv+0x1dc>
    f2f8:	e1b03000 	movs	r3, r0
    f2fc:	42603000 	rsbmi	r3, r0, #0	; 0x0
    f300:	e1530001 	cmp	r3, r1
    f304:	9a00006f 	bls	f4c8 <__aeabi_idiv+0x1e8>
    f308:	e1110002 	tst	r1, r2
    f30c:	0a000071 	beq	f4d8 <__aeabi_idiv+0x1f8>
    f310:	e16f2f13 	clz	r2, r3
    f314:	e16f0f11 	clz	r0, r1
    f318:	e0402002 	sub	r2, r0, r2
    f31c:	e272201f 	rsbs	r2, r2, #31	; 0x1f
    f320:	10822082 	addne	r2, r2, r2, lsl #1
    f324:	e3a00000 	mov	r0, #0	; 0x0
    f328:	108ff102 	addne	pc, pc, r2, lsl #2
    f32c:	e1a00000 	nop			(mov r0,r0)
    f330:	e1530f81 	cmp	r3, r1, lsl #31
    f334:	e0a00000 	adc	r0, r0, r0
    f338:	20433f81 	subcs	r3, r3, r1, lsl #31
    f33c:	e1530f01 	cmp	r3, r1, lsl #30
    f340:	e0a00000 	adc	r0, r0, r0
    f344:	20433f01 	subcs	r3, r3, r1, lsl #30
    f348:	e1530e81 	cmp	r3, r1, lsl #29
    f34c:	e0a00000 	adc	r0, r0, r0
    f350:	20433e81 	subcs	r3, r3, r1, lsl #29
    f354:	e1530e01 	cmp	r3, r1, lsl #28
    f358:	e0a00000 	adc	r0, r0, r0
    f35c:	20433e01 	subcs	r3, r3, r1, lsl #28
    f360:	e1530d81 	cmp	r3, r1, lsl #27
    f364:	e0a00000 	adc	r0, r0, r0
    f368:	20433d81 	subcs	r3, r3, r1, lsl #27
    f36c:	e1530d01 	cmp	r3, r1, lsl #26
    f370:	e0a00000 	adc	r0, r0, r0
    f374:	20433d01 	subcs	r3, r3, r1, lsl #26
    f378:	e1530c81 	cmp	r3, r1, lsl #25
    f37c:	e0a00000 	adc	r0, r0, r0
    f380:	20433c81 	subcs	r3, r3, r1, lsl #25
    f384:	e1530c01 	cmp	r3, r1, lsl #24
    f388:	e0a00000 	adc	r0, r0, r0
    f38c:	20433c01 	subcs	r3, r3, r1, lsl #24
    f390:	e1530b81 	cmp	r3, r1, lsl #23
    f394:	e0a00000 	adc	r0, r0, r0
    f398:	20433b81 	subcs	r3, r3, r1, lsl #23
    f39c:	e1530b01 	cmp	r3, r1, lsl #22
    f3a0:	e0a00000 	adc	r0, r0, r0
    f3a4:	20433b01 	subcs	r3, r3, r1, lsl #22
    f3a8:	e1530a81 	cmp	r3, r1, lsl #21
    f3ac:	e0a00000 	adc	r0, r0, r0
    f3b0:	20433a81 	subcs	r3, r3, r1, lsl #21
    f3b4:	e1530a01 	cmp	r3, r1, lsl #20
    f3b8:	e0a00000 	adc	r0, r0, r0
    f3bc:	20433a01 	subcs	r3, r3, r1, lsl #20
    f3c0:	e1530981 	cmp	r3, r1, lsl #19
    f3c4:	e0a00000 	adc	r0, r0, r0
    f3c8:	20433981 	subcs	r3, r3, r1, lsl #19
    f3cc:	e1530901 	cmp	r3, r1, lsl #18
    f3d0:	e0a00000 	adc	r0, r0, r0
    f3d4:	20433901 	subcs	r3, r3, r1, lsl #18
    f3d8:	e1530881 	cmp	r3, r1, lsl #17
    f3dc:	e0a00000 	adc	r0, r0, r0
    f3e0:	20433881 	subcs	r3, r3, r1, lsl #17
    f3e4:	e1530801 	cmp	r3, r1, lsl #16
    f3e8:	e0a00000 	adc	r0, r0, r0
    f3ec:	20433801 	subcs	r3, r3, r1, lsl #16
    f3f0:	e1530781 	cmp	r3, r1, lsl #15
    f3f4:	e0a00000 	adc	r0, r0, r0
    f3f8:	20433781 	subcs	r3, r3, r1, lsl #15
    f3fc:	e1530701 	cmp	r3, r1, lsl #14
    f400:	e0a00000 	adc	r0, r0, r0
    f404:	20433701 	subcs	r3, r3, r1, lsl #14
    f408:	e1530681 	cmp	r3, r1, lsl #13
    f40c:	e0a00000 	adc	r0, r0, r0
    f410:	20433681 	subcs	r3, r3, r1, lsl #13
    f414:	e1530601 	cmp	r3, r1, lsl #12
    f418:	e0a00000 	adc	r0, r0, r0
    f41c:	20433601 	subcs	r3, r3, r1, lsl #12
    f420:	e1530581 	cmp	r3, r1, lsl #11
    f424:	e0a00000 	adc	r0, r0, r0
    f428:	20433581 	subcs	r3, r3, r1, lsl #11
    f42c:	e1530501 	cmp	r3, r1, lsl #10
    f430:	e0a00000 	adc	r0, r0, r0
    f434:	20433501 	subcs	r3, r3, r1, lsl #10
    f438:	e1530481 	cmp	r3, r1, lsl #9
    f43c:	e0a00000 	adc	r0, r0, r0
    f440:	20433481 	subcs	r3, r3, r1, lsl #9
    f444:	e1530401 	cmp	r3, r1, lsl #8
    f448:	e0a00000 	adc	r0, r0, r0
    f44c:	20433401 	subcs	r3, r3, r1, lsl #8
    f450:	e1530381 	cmp	r3, r1, lsl #7
    f454:	e0a00000 	adc	r0, r0, r0
    f458:	20433381 	subcs	r3, r3, r1, lsl #7
    f45c:	e1530301 	cmp	r3, r1, lsl #6
    f460:	e0a00000 	adc	r0, r0, r0
    f464:	20433301 	subcs	r3, r3, r1, lsl #6
    f468:	e1530281 	cmp	r3, r1, lsl #5
    f46c:	e0a00000 	adc	r0, r0, r0
    f470:	20433281 	subcs	r3, r3, r1, lsl #5
    f474:	e1530201 	cmp	r3, r1, lsl #4
    f478:	e0a00000 	adc	r0, r0, r0
    f47c:	20433201 	subcs	r3, r3, r1, lsl #4
    f480:	e1530181 	cmp	r3, r1, lsl #3
    f484:	e0a00000 	adc	r0, r0, r0
    f488:	20433181 	subcs	r3, r3, r1, lsl #3
    f48c:	e1530101 	cmp	r3, r1, lsl #2
    f490:	e0a00000 	adc	r0, r0, r0
    f494:	20433101 	subcs	r3, r3, r1, lsl #2
    f498:	e1530081 	cmp	r3, r1, lsl #1
    f49c:	e0a00000 	adc	r0, r0, r0
    f4a0:	20433081 	subcs	r3, r3, r1, lsl #1
    f4a4:	e1530001 	cmp	r3, r1
    f4a8:	e0a00000 	adc	r0, r0, r0
    f4ac:	20433001 	subcs	r3, r3, r1
    f4b0:	e35c0000 	cmp	ip, #0	; 0x0
    f4b4:	42600000 	rsbmi	r0, r0, #0	; 0x0
    f4b8:	e12fff1e 	bx	lr
    f4bc:	e13c0000 	teq	ip, r0
    f4c0:	42600000 	rsbmi	r0, r0, #0	; 0x0
    f4c4:	e12fff1e 	bx	lr
    f4c8:	33a00000 	movcc	r0, #0	; 0x0
    f4cc:	01a00fcc 	asreq	r0, ip, #31
    f4d0:	03800001 	orreq	r0, r0, #1	; 0x1
    f4d4:	e12fff1e 	bx	lr
    f4d8:	e16f2f11 	clz	r2, r1
    f4dc:	e262201f 	rsb	r2, r2, #31	; 0x1f
    f4e0:	e35c0000 	cmp	ip, #0	; 0x0
    f4e4:	e1a00233 	lsr	r0, r3, r2
    f4e8:	42600000 	rsbmi	r0, r0, #0	; 0x0
    f4ec:	e12fff1e 	bx	lr
    f4f0:	e52de008 	str	lr, [sp, #-8]!
    f4f4:	eb000236 	bl	fdd4 <__div0>
    f4f8:	e3a00000 	mov	r0, #0	; 0x0
    f4fc:	e49df008 	ldr	pc, [sp], #8

0000f500 <__aeabi_idivmod>:
    f500:	e92d4003 	push	{r0, r1, lr}
    f504:	ebffff75 	bl	f2e0 <__aeabi_idiv>
    f508:	e8bd4006 	pop	{r1, r2, lr}
    f50c:	e0030092 	mul	r3, r2, r0
    f510:	e0411003 	sub	r1, r1, r3
    f514:	e12fff1e 	bx	lr

0000f518 <__aeabi_llsr>:
    f518:	e2523020 	subs	r3, r2, #32	; 0x20
    f51c:	e262c020 	rsb	ip, r2, #32	; 0x20
    f520:	41a00230 	lsrmi	r0, r0, r2
    f524:	51a00331 	lsrpl	r0, r1, r3
    f528:	41800c11 	orrmi	r0, r0, r1, lsl ip
    f52c:	e1a01231 	lsr	r1, r1, r2
    f530:	e12fff1e 	bx	lr

0000f534 <__aeabi_llsl>:
    f534:	e2523020 	subs	r3, r2, #32	; 0x20
    f538:	e262c020 	rsb	ip, r2, #32	; 0x20
    f53c:	41a01211 	lslmi	r1, r1, r2
    f540:	51a01310 	lslpl	r1, r0, r3
    f544:	41811c30 	orrmi	r1, r1, r0, lsr ip
    f548:	e1a00210 	lsl	r0, r0, r2
    f54c:	e12fff1e 	bx	lr

0000f550 <__aeabi_drsub>:
    f550:	e2211102 	eor	r1, r1, #-2147483648	; 0x80000000
    f554:	ea000000 	b	f55c <__adddf3>

0000f558 <__aeabi_dsub>:
    f558:	e2233102 	eor	r3, r3, #-2147483648	; 0x80000000

0000f55c <__adddf3>:
    f55c:	e92d4030 	push	{r4, r5, lr}
    f560:	e1a04081 	lsl	r4, r1, #1
    f564:	e1a05083 	lsl	r5, r3, #1
    f568:	e1340005 	teq	r4, r5
    f56c:	01300002 	teqeq	r0, r2
    f570:	1194c000 	orrsne	ip, r4, r0
    f574:	1195c002 	orrsne	ip, r5, r2
    f578:	11f0cac4 	mvnsne	ip, r4, asr #21
    f57c:	11f0cac5 	mvnsne	ip, r5, asr #21
    f580:	0a000079 	beq	f76c <__adddf3+0x210>
    f584:	e1a04aa4 	lsr	r4, r4, #21
    f588:	e0745aa5 	rsbs	r5, r4, r5, lsr #21
    f58c:	b2655000 	rsblt	r5, r5, #0	; 0x0
    f590:	da000006 	ble	f5b0 <__adddf3+0x54>
    f594:	e0844005 	add	r4, r4, r5
    f598:	e0202002 	eor	r2, r0, r2
    f59c:	e0213003 	eor	r3, r1, r3
    f5a0:	e0220000 	eor	r0, r2, r0
    f5a4:	e0231001 	eor	r1, r3, r1
    f5a8:	e0202002 	eor	r2, r0, r2
    f5ac:	e0213003 	eor	r3, r1, r3
    f5b0:	e3550036 	cmp	r5, #54	; 0x36
    f5b4:	88bd8030 	pophi	{r4, r5, pc}
    f5b8:	e3110102 	tst	r1, #-2147483648	; 0x80000000
    f5bc:	e1a01601 	lsl	r1, r1, #12
    f5c0:	e3a0c601 	mov	ip, #1048576	; 0x100000
    f5c4:	e18c1621 	orr	r1, ip, r1, lsr #12
    f5c8:	0a000001 	beq	f5d4 <__adddf3+0x78>
    f5cc:	e2700000 	rsbs	r0, r0, #0	; 0x0
    f5d0:	e2e11000 	rsc	r1, r1, #0	; 0x0
    f5d4:	e3130102 	tst	r3, #-2147483648	; 0x80000000
    f5d8:	e1a03603 	lsl	r3, r3, #12
    f5dc:	e18c3623 	orr	r3, ip, r3, lsr #12
    f5e0:	0a000001 	beq	f5ec <__adddf3+0x90>
    f5e4:	e2722000 	rsbs	r2, r2, #0	; 0x0
    f5e8:	e2e33000 	rsc	r3, r3, #0	; 0x0
    f5ec:	e1340005 	teq	r4, r5
    f5f0:	0a000057 	beq	f754 <__adddf3+0x1f8>
    f5f4:	e2444001 	sub	r4, r4, #1	; 0x1
    f5f8:	e275e020 	rsbs	lr, r5, #32	; 0x20
    f5fc:	ba000005 	blt	f618 <__adddf3+0xbc>
    f600:	e1a0ce12 	lsl	ip, r2, lr
    f604:	e0900532 	adds	r0, r0, r2, lsr r5
    f608:	e2a11000 	adc	r1, r1, #0	; 0x0
    f60c:	e0900e13 	adds	r0, r0, r3, lsl lr
    f610:	e0b11553 	adcs	r1, r1, r3, asr r5
    f614:	ea000006 	b	f634 <__adddf3+0xd8>
    f618:	e2455020 	sub	r5, r5, #32	; 0x20
    f61c:	e28ee020 	add	lr, lr, #32	; 0x20
    f620:	e3520001 	cmp	r2, #1	; 0x1
    f624:	e1a0ce13 	lsl	ip, r3, lr
    f628:	238cc002 	orrcs	ip, ip, #2	; 0x2
    f62c:	e0900553 	adds	r0, r0, r3, asr r5
    f630:	e0b11fc3 	adcs	r1, r1, r3, asr #31
    f634:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
    f638:	5a000002 	bpl	f648 <__adddf3+0xec>
    f63c:	e27cc000 	rsbs	ip, ip, #0	; 0x0
    f640:	e2f00000 	rscs	r0, r0, #0	; 0x0
    f644:	e2e11000 	rsc	r1, r1, #0	; 0x0
    f648:	e3510601 	cmp	r1, #1048576	; 0x100000
    f64c:	3a00000e 	bcc	f68c <__adddf3+0x130>
    f650:	e3510602 	cmp	r1, #2097152	; 0x200000
    f654:	3a000006 	bcc	f674 <__adddf3+0x118>
    f658:	e1b010a1 	lsrs	r1, r1, #1
    f65c:	e1b00060 	rrxs	r0, r0
    f660:	e1a0c06c 	rrx	ip, ip
    f664:	e2844001 	add	r4, r4, #1	; 0x1
    f668:	e1a02a84 	lsl	r2, r4, #21
    f66c:	e3720501 	cmn	r2, #4194304	; 0x400000
    f670:	2a000055 	bcs	f7cc <__adddf3+0x270>
    f674:	e35c0102 	cmp	ip, #-2147483648	; 0x80000000
    f678:	01b0c0a0 	lsrseq	ip, r0, #1
    f67c:	e2b00000 	adcs	r0, r0, #0	; 0x0
    f680:	e0a11a04 	adc	r1, r1, r4, lsl #20
    f684:	e1811005 	orr	r1, r1, r5
    f688:	e8bd8030 	pop	{r4, r5, pc}
    f68c:	e1b0c08c 	lsls	ip, ip, #1
    f690:	e0b00000 	adcs	r0, r0, r0
    f694:	e0a11001 	adc	r1, r1, r1
    f698:	e3110601 	tst	r1, #1048576	; 0x100000
    f69c:	e2444001 	sub	r4, r4, #1	; 0x1
    f6a0:	1afffff3 	bne	f674 <__adddf3+0x118>
    f6a4:	e3310000 	teq	r1, #0	; 0x0
    f6a8:	01a01000 	moveq	r1, r0
    f6ac:	03a00000 	moveq	r0, #0	; 0x0
    f6b0:	e16f3f11 	clz	r3, r1
    f6b4:	02833020 	addeq	r3, r3, #32	; 0x20
    f6b8:	e243300b 	sub	r3, r3, #11	; 0xb
    f6bc:	e2532020 	subs	r2, r3, #32	; 0x20
    f6c0:	aa000007 	bge	f6e4 <__adddf3+0x188>
    f6c4:	e292200c 	adds	r2, r2, #12	; 0xc
    f6c8:	da000004 	ble	f6e0 <__adddf3+0x184>
    f6cc:	e282c014 	add	ip, r2, #20	; 0x14
    f6d0:	e262200c 	rsb	r2, r2, #12	; 0xc
    f6d4:	e1a00c11 	lsl	r0, r1, ip
    f6d8:	e1a01231 	lsr	r1, r1, r2
    f6dc:	ea000004 	b	f6f4 <__adddf3+0x198>
    f6e0:	e2822014 	add	r2, r2, #20	; 0x14
    f6e4:	d262c020 	rsble	ip, r2, #32	; 0x20
    f6e8:	e1a01211 	lsl	r1, r1, r2
    f6ec:	d1811c30 	orrle	r1, r1, r0, lsr ip
    f6f0:	d1a00210 	lslle	r0, r0, r2
    f6f4:	e0544003 	subs	r4, r4, r3
    f6f8:	a0811a04 	addge	r1, r1, r4, lsl #20
    f6fc:	a1811005 	orrge	r1, r1, r5
    f700:	a8bd8030 	popge	{r4, r5, pc}
    f704:	e1e04004 	mvn	r4, r4
    f708:	e254401f 	subs	r4, r4, #31	; 0x1f
    f70c:	aa00000d 	bge	f748 <__adddf3+0x1ec>
    f710:	e294400c 	adds	r4, r4, #12	; 0xc
    f714:	ca000005 	bgt	f730 <__adddf3+0x1d4>
    f718:	e2844014 	add	r4, r4, #20	; 0x14
    f71c:	e2642020 	rsb	r2, r4, #32	; 0x20
    f720:	e1a00430 	lsr	r0, r0, r4
    f724:	e1800211 	orr	r0, r0, r1, lsl r2
    f728:	e1851431 	orr	r1, r5, r1, lsr r4
    f72c:	e8bd8030 	pop	{r4, r5, pc}
    f730:	e264400c 	rsb	r4, r4, #12	; 0xc
    f734:	e2642020 	rsb	r2, r4, #32	; 0x20
    f738:	e1a00230 	lsr	r0, r0, r2
    f73c:	e1800411 	orr	r0, r0, r1, lsl r4
    f740:	e1a01005 	mov	r1, r5
    f744:	e8bd8030 	pop	{r4, r5, pc}
    f748:	e1a00431 	lsr	r0, r1, r4
    f74c:	e1a01005 	mov	r1, r5
    f750:	e8bd8030 	pop	{r4, r5, pc}
    f754:	e3340000 	teq	r4, #0	; 0x0
    f758:	e2233601 	eor	r3, r3, #1048576	; 0x100000
    f75c:	02211601 	eoreq	r1, r1, #1048576	; 0x100000
    f760:	02844001 	addeq	r4, r4, #1	; 0x1
    f764:	12455001 	subne	r5, r5, #1	; 0x1
    f768:	eaffffa1 	b	f5f4 <__adddf3+0x98>
    f76c:	e1f0cac4 	mvns	ip, r4, asr #21
    f770:	11f0cac5 	mvnsne	ip, r5, asr #21
    f774:	0a000018 	beq	f7dc <__adddf3+0x280>
    f778:	e1340005 	teq	r4, r5
    f77c:	01300002 	teqeq	r0, r2
    f780:	0a000003 	beq	f794 <__adddf3+0x238>
    f784:	e194c000 	orrs	ip, r4, r0
    f788:	01a01003 	moveq	r1, r3
    f78c:	01a00002 	moveq	r0, r2
    f790:	e8bd8030 	pop	{r4, r5, pc}
    f794:	e1310003 	teq	r1, r3
    f798:	13a01000 	movne	r1, #0	; 0x0
    f79c:	13a00000 	movne	r0, #0	; 0x0
    f7a0:	18bd8030 	popne	{r4, r5, pc}
    f7a4:	e1b0caa4 	lsrs	ip, r4, #21
    f7a8:	1a000003 	bne	f7bc <__adddf3+0x260>
    f7ac:	e1b00080 	lsls	r0, r0, #1
    f7b0:	e0b11001 	adcs	r1, r1, r1
    f7b4:	23811102 	orrcs	r1, r1, #-2147483648	; 0x80000000
    f7b8:	e8bd8030 	pop	{r4, r5, pc}
    f7bc:	e2944501 	adds	r4, r4, #4194304	; 0x400000
    f7c0:	32811601 	addcc	r1, r1, #1048576	; 0x100000
    f7c4:	38bd8030 	popcc	{r4, r5, pc}
    f7c8:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
    f7cc:	e385147f 	orr	r1, r5, #2130706432	; 0x7f000000
    f7d0:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
    f7d4:	e3a00000 	mov	r0, #0	; 0x0
    f7d8:	e8bd8030 	pop	{r4, r5, pc}
    f7dc:	e1f0cac4 	mvns	ip, r4, asr #21
    f7e0:	11a01003 	movne	r1, r3
    f7e4:	11a00002 	movne	r0, r2
    f7e8:	01f0cac5 	mvnseq	ip, r5, asr #21
    f7ec:	11a03001 	movne	r3, r1
    f7f0:	11a02000 	movne	r2, r0
    f7f4:	e1904601 	orrs	r4, r0, r1, lsl #12
    f7f8:	01925603 	orrseq	r5, r2, r3, lsl #12
    f7fc:	01310003 	teqeq	r1, r3
    f800:	13811702 	orrne	r1, r1, #524288	; 0x80000
    f804:	e8bd8030 	pop	{r4, r5, pc}

0000f808 <__aeabi_ui2d>:
    f808:	e3300000 	teq	r0, #0	; 0x0
    f80c:	03a01000 	moveq	r1, #0	; 0x0
    f810:	012fff1e 	bxeq	lr
    f814:	e92d4030 	push	{r4, r5, lr}
    f818:	e3a04b01 	mov	r4, #1024	; 0x400
    f81c:	e2844032 	add	r4, r4, #50	; 0x32
    f820:	e3a05000 	mov	r5, #0	; 0x0
    f824:	e3a01000 	mov	r1, #0	; 0x0
    f828:	eaffff9d 	b	f6a4 <__adddf3+0x148>

0000f82c <__aeabi_i2d>:
    f82c:	e3300000 	teq	r0, #0	; 0x0
    f830:	03a01000 	moveq	r1, #0	; 0x0
    f834:	012fff1e 	bxeq	lr
    f838:	e92d4030 	push	{r4, r5, lr}
    f83c:	e3a04b01 	mov	r4, #1024	; 0x400
    f840:	e2844032 	add	r4, r4, #50	; 0x32
    f844:	e2105102 	ands	r5, r0, #-2147483648	; 0x80000000
    f848:	42600000 	rsbmi	r0, r0, #0	; 0x0
    f84c:	e3a01000 	mov	r1, #0	; 0x0
    f850:	eaffff93 	b	f6a4 <__adddf3+0x148>

0000f854 <__aeabi_f2d>:
    f854:	e1b02080 	lsls	r2, r0, #1
    f858:	e1a011c2 	asr	r1, r2, #3
    f85c:	e1a01061 	rrx	r1, r1
    f860:	e1a00e02 	lsl	r0, r2, #28
    f864:	121234ff 	andsne	r3, r2, #-16777216	; 0xff000000
    f868:	133304ff 	teqne	r3, #-16777216	; 0xff000000
    f86c:	1221130e 	eorne	r1, r1, #939524096	; 0x38000000
    f870:	112fff1e 	bxne	lr
    f874:	e3320000 	teq	r2, #0	; 0x0
    f878:	133304ff 	teqne	r3, #-16777216	; 0xff000000
    f87c:	012fff1e 	bxeq	lr
    f880:	e92d4030 	push	{r4, r5, lr}
    f884:	e3a04d0e 	mov	r4, #896	; 0x380
    f888:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
    f88c:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
    f890:	eaffff83 	b	f6a4 <__adddf3+0x148>

0000f894 <__aeabi_ul2d>:
    f894:	e1902001 	orrs	r2, r0, r1
    f898:	012fff1e 	bxeq	lr
    f89c:	e92d4030 	push	{r4, r5, lr}
    f8a0:	e3a05000 	mov	r5, #0	; 0x0
    f8a4:	ea000006 	b	f8c4 <__aeabi_l2d+0x1c>

0000f8a8 <__aeabi_l2d>:
    f8a8:	e1902001 	orrs	r2, r0, r1
    f8ac:	012fff1e 	bxeq	lr
    f8b0:	e92d4030 	push	{r4, r5, lr}
    f8b4:	e2115102 	ands	r5, r1, #-2147483648	; 0x80000000
    f8b8:	5a000001 	bpl	f8c4 <__aeabi_l2d+0x1c>
    f8bc:	e2700000 	rsbs	r0, r0, #0	; 0x0
    f8c0:	e2e11000 	rsc	r1, r1, #0	; 0x0
    f8c4:	e3a04b01 	mov	r4, #1024	; 0x400
    f8c8:	e2844032 	add	r4, r4, #50	; 0x32
    f8cc:	e1b0cb21 	lsrs	ip, r1, #22
    f8d0:	0affff5c 	beq	f648 <__adddf3+0xec>
    f8d4:	e3a02003 	mov	r2, #3	; 0x3
    f8d8:	e1b0c1ac 	lsrs	ip, ip, #3
    f8dc:	12822003 	addne	r2, r2, #3	; 0x3
    f8e0:	e1b0c1ac 	lsrs	ip, ip, #3
    f8e4:	12822003 	addne	r2, r2, #3	; 0x3
    f8e8:	e08221ac 	add	r2, r2, ip, lsr #3
    f8ec:	e2623020 	rsb	r3, r2, #32	; 0x20
    f8f0:	e1a0c310 	lsl	ip, r0, r3
    f8f4:	e1a00230 	lsr	r0, r0, r2
    f8f8:	e1800311 	orr	r0, r0, r1, lsl r3
    f8fc:	e1a01231 	lsr	r1, r1, r2
    f900:	e0844002 	add	r4, r4, r2
    f904:	eaffff4f 	b	f648 <__adddf3+0xec>

0000f908 <__aeabi_dmul>:
    f908:	e92d4070 	push	{r4, r5, r6, lr}
    f90c:	e3a0c0ff 	mov	ip, #255	; 0xff
    f910:	e38ccc07 	orr	ip, ip, #1792	; 0x700
    f914:	e01c4a21 	ands	r4, ip, r1, lsr #20
    f918:	101c5a23 	andsne	r5, ip, r3, lsr #20
    f91c:	1134000c 	teqne	r4, ip
    f920:	1135000c 	teqne	r5, ip
    f924:	0b00006f 	bleq	fae8 <__aeabi_dmul+0x1e0>
    f928:	e0844005 	add	r4, r4, r5
    f92c:	e0216003 	eor	r6, r1, r3
    f930:	e1c11a8c 	bic	r1, r1, ip, lsl #21
    f934:	e1c33a8c 	bic	r3, r3, ip, lsl #21
    f938:	e1905601 	orrs	r5, r0, r1, lsl #12
    f93c:	11925603 	orrsne	r5, r2, r3, lsl #12
    f940:	e3811601 	orr	r1, r1, #1048576	; 0x100000
    f944:	e3833601 	orr	r3, r3, #1048576	; 0x100000
    f948:	0a00001c 	beq	f9c0 <__aeabi_dmul+0xb8>
    f94c:	e08ec290 	umull	ip, lr, r0, r2
    f950:	e3a05000 	mov	r5, #0	; 0x0
    f954:	e0a5e291 	umlal	lr, r5, r1, r2
    f958:	e2062102 	and	r2, r6, #-2147483648	; 0x80000000
    f95c:	e0a5e390 	umlal	lr, r5, r0, r3
    f960:	e3a06000 	mov	r6, #0	; 0x0
    f964:	e0a65391 	umlal	r5, r6, r1, r3
    f968:	e33c0000 	teq	ip, #0	; 0x0
    f96c:	138ee001 	orrne	lr, lr, #1	; 0x1
    f970:	e24440ff 	sub	r4, r4, #255	; 0xff
    f974:	e3560c02 	cmp	r6, #512	; 0x200
    f978:	e2c44c03 	sbc	r4, r4, #768	; 0x300
    f97c:	2a000002 	bcs	f98c <__aeabi_dmul+0x84>
    f980:	e1b0e08e 	lsls	lr, lr, #1
    f984:	e0b55005 	adcs	r5, r5, r5
    f988:	e0a66006 	adc	r6, r6, r6
    f98c:	e1821586 	orr	r1, r2, r6, lsl #11
    f990:	e1811aa5 	orr	r1, r1, r5, lsr #21
    f994:	e1a00585 	lsl	r0, r5, #11
    f998:	e1800aae 	orr	r0, r0, lr, lsr #21
    f99c:	e1a0e58e 	lsl	lr, lr, #11
    f9a0:	e254c0fd 	subs	ip, r4, #253	; 0xfd
    f9a4:	835c0c07 	cmphi	ip, #1792	; 0x700
    f9a8:	8a00000f 	bhi	f9ec <__aeabi_dmul+0xe4>
    f9ac:	e35e0102 	cmp	lr, #-2147483648	; 0x80000000
    f9b0:	01b0e0a0 	lsrseq	lr, r0, #1
    f9b4:	e2b00000 	adcs	r0, r0, #0	; 0x0
    f9b8:	e0a11a04 	adc	r1, r1, r4, lsl #20
    f9bc:	e8bd8070 	pop	{r4, r5, r6, pc}
    f9c0:	e2066102 	and	r6, r6, #-2147483648	; 0x80000000
    f9c4:	e1861001 	orr	r1, r6, r1
    f9c8:	e1800002 	orr	r0, r0, r2
    f9cc:	e0211003 	eor	r1, r1, r3
    f9d0:	e05440ac 	subs	r4, r4, ip, lsr #1
    f9d4:	c074500c 	rsbsgt	r5, r4, ip
    f9d8:	c1811a04 	orrgt	r1, r1, r4, lsl #20
    f9dc:	c8bd8070 	popgt	{r4, r5, r6, pc}
    f9e0:	e3811601 	orr	r1, r1, #1048576	; 0x100000
    f9e4:	e3a0e000 	mov	lr, #0	; 0x0
    f9e8:	e2544001 	subs	r4, r4, #1	; 0x1
    f9ec:	ca000058 	bgt	fb54 <__aeabi_dmul+0x24c>
    f9f0:	e3740036 	cmn	r4, #54	; 0x36
    f9f4:	d3a00000 	movle	r0, #0	; 0x0
    f9f8:	d2011102 	andle	r1, r1, #-2147483648	; 0x80000000
    f9fc:	d8bd8070 	pople	{r4, r5, r6, pc}
    fa00:	e2644000 	rsb	r4, r4, #0	; 0x0
    fa04:	e2544020 	subs	r4, r4, #32	; 0x20
    fa08:	aa000018 	bge	fa70 <__aeabi_dmul+0x168>
    fa0c:	e294400c 	adds	r4, r4, #12	; 0xc
    fa10:	ca00000b 	bgt	fa44 <__aeabi_dmul+0x13c>
    fa14:	e2844014 	add	r4, r4, #20	; 0x14
    fa18:	e2645020 	rsb	r5, r4, #32	; 0x20
    fa1c:	e1a03510 	lsl	r3, r0, r5
    fa20:	e1a00430 	lsr	r0, r0, r4
    fa24:	e1800511 	orr	r0, r0, r1, lsl r5
    fa28:	e2012102 	and	r2, r1, #-2147483648	; 0x80000000
    fa2c:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
    fa30:	e0900fa3 	adds	r0, r0, r3, lsr #31
    fa34:	e0a21431 	adc	r1, r2, r1, lsr r4
    fa38:	e19ee083 	orrs	lr, lr, r3, lsl #1
    fa3c:	01c00fa3 	biceq	r0, r0, r3, lsr #31
    fa40:	e8bd8070 	pop	{r4, r5, r6, pc}
    fa44:	e264400c 	rsb	r4, r4, #12	; 0xc
    fa48:	e2645020 	rsb	r5, r4, #32	; 0x20
    fa4c:	e1a03410 	lsl	r3, r0, r4
    fa50:	e1a00530 	lsr	r0, r0, r5
    fa54:	e1800411 	orr	r0, r0, r1, lsl r4
    fa58:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
    fa5c:	e0900fa3 	adds	r0, r0, r3, lsr #31
    fa60:	e2a11000 	adc	r1, r1, #0	; 0x0
    fa64:	e19ee083 	orrs	lr, lr, r3, lsl #1
    fa68:	01c00fa3 	biceq	r0, r0, r3, lsr #31
    fa6c:	e8bd8070 	pop	{r4, r5, r6, pc}
    fa70:	e2645020 	rsb	r5, r4, #32	; 0x20
    fa74:	e18ee510 	orr	lr, lr, r0, lsl r5
    fa78:	e1a03430 	lsr	r3, r0, r4
    fa7c:	e1833511 	orr	r3, r3, r1, lsl r5
    fa80:	e1a00431 	lsr	r0, r1, r4
    fa84:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
    fa88:	e1c00431 	bic	r0, r0, r1, lsr r4
    fa8c:	e0800fa3 	add	r0, r0, r3, lsr #31
    fa90:	e19ee083 	orrs	lr, lr, r3, lsl #1
    fa94:	01c00fa3 	biceq	r0, r0, r3, lsr #31
    fa98:	e8bd8070 	pop	{r4, r5, r6, pc}
    fa9c:	e3340000 	teq	r4, #0	; 0x0
    faa0:	1a000008 	bne	fac8 <__aeabi_dmul+0x1c0>
    faa4:	e2016102 	and	r6, r1, #-2147483648	; 0x80000000
    faa8:	e1b00080 	lsls	r0, r0, #1
    faac:	e0a11001 	adc	r1, r1, r1
    fab0:	e3110601 	tst	r1, #1048576	; 0x100000
    fab4:	02444001 	subeq	r4, r4, #1	; 0x1
    fab8:	0afffffa 	beq	faa8 <__aeabi_dmul+0x1a0>
    fabc:	e1811006 	orr	r1, r1, r6
    fac0:	e3350000 	teq	r5, #0	; 0x0
    fac4:	11a0f00e 	movne	pc, lr
    fac8:	e2036102 	and	r6, r3, #-2147483648	; 0x80000000
    facc:	e1b02082 	lsls	r2, r2, #1
    fad0:	e0a33003 	adc	r3, r3, r3
    fad4:	e3130601 	tst	r3, #1048576	; 0x100000
    fad8:	02455001 	subeq	r5, r5, #1	; 0x1
    fadc:	0afffffa 	beq	facc <__aeabi_dmul+0x1c4>
    fae0:	e1833006 	orr	r3, r3, r6
    fae4:	e1a0f00e 	mov	pc, lr
    fae8:	e134000c 	teq	r4, ip
    faec:	e00c5a23 	and	r5, ip, r3, lsr #20
    faf0:	1135000c 	teqne	r5, ip
    faf4:	0a000006 	beq	fb14 <__aeabi_dmul+0x20c>
    faf8:	e1906081 	orrs	r6, r0, r1, lsl #1
    fafc:	11926083 	orrsne	r6, r2, r3, lsl #1
    fb00:	1affffe5 	bne	fa9c <__aeabi_dmul+0x194>
    fb04:	e0211003 	eor	r1, r1, r3
    fb08:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
    fb0c:	e3a00000 	mov	r0, #0	; 0x0
    fb10:	e8bd8070 	pop	{r4, r5, r6, pc}
    fb14:	e1906081 	orrs	r6, r0, r1, lsl #1
    fb18:	01a00002 	moveq	r0, r2
    fb1c:	01a01003 	moveq	r1, r3
    fb20:	11926083 	orrsne	r6, r2, r3, lsl #1
    fb24:	0a00000f 	beq	fb68 <__aeabi_dmul+0x260>
    fb28:	e134000c 	teq	r4, ip
    fb2c:	1a000001 	bne	fb38 <__aeabi_dmul+0x230>
    fb30:	e1906601 	orrs	r6, r0, r1, lsl #12
    fb34:	1a00000b 	bne	fb68 <__aeabi_dmul+0x260>
    fb38:	e135000c 	teq	r5, ip
    fb3c:	1a000003 	bne	fb50 <__aeabi_dmul+0x248>
    fb40:	e1926603 	orrs	r6, r2, r3, lsl #12
    fb44:	11a00002 	movne	r0, r2
    fb48:	11a01003 	movne	r1, r3
    fb4c:	1a000005 	bne	fb68 <__aeabi_dmul+0x260>
    fb50:	e0211003 	eor	r1, r1, r3
    fb54:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
    fb58:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
    fb5c:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
    fb60:	e3a00000 	mov	r0, #0	; 0x0
    fb64:	e8bd8070 	pop	{r4, r5, r6, pc}
    fb68:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
    fb6c:	e381173e 	orr	r1, r1, #16252928	; 0xf80000
    fb70:	e8bd8070 	pop	{r4, r5, r6, pc}

0000fb74 <__aeabi_ddiv>:
    fb74:	e92d4070 	push	{r4, r5, r6, lr}
    fb78:	e3a0c0ff 	mov	ip, #255	; 0xff
    fb7c:	e38ccc07 	orr	ip, ip, #1792	; 0x700
    fb80:	e01c4a21 	ands	r4, ip, r1, lsr #20
    fb84:	101c5a23 	andsne	r5, ip, r3, lsr #20
    fb88:	1134000c 	teqne	r4, ip
    fb8c:	1135000c 	teqne	r5, ip
    fb90:	0b00005c 	bleq	fd08 <__aeabi_ddiv+0x194>
    fb94:	e0444005 	sub	r4, r4, r5
    fb98:	e021e003 	eor	lr, r1, r3
    fb9c:	e1925603 	orrs	r5, r2, r3, lsl #12
    fba0:	e1a01601 	lsl	r1, r1, #12
    fba4:	0a00004b 	beq	fcd8 <__aeabi_ddiv+0x164>
    fba8:	e1a03603 	lsl	r3, r3, #12
    fbac:	e3a05201 	mov	r5, #268435456	; 0x10000000
    fbb0:	e1853223 	orr	r3, r5, r3, lsr #4
    fbb4:	e1833c22 	orr	r3, r3, r2, lsr #24
    fbb8:	e1a02402 	lsl	r2, r2, #8
    fbbc:	e1855221 	orr	r5, r5, r1, lsr #4
    fbc0:	e1855c20 	orr	r5, r5, r0, lsr #24
    fbc4:	e1a06400 	lsl	r6, r0, #8
    fbc8:	e20e1102 	and	r1, lr, #-2147483648	; 0x80000000
    fbcc:	e1550003 	cmp	r5, r3
    fbd0:	01560002 	cmpeq	r6, r2
    fbd4:	e2a440fd 	adc	r4, r4, #253	; 0xfd
    fbd8:	e2844c03 	add	r4, r4, #768	; 0x300
    fbdc:	2a000001 	bcs	fbe8 <__aeabi_ddiv+0x74>
    fbe0:	e1b030a3 	lsrs	r3, r3, #1
    fbe4:	e1a02062 	rrx	r2, r2
    fbe8:	e0566002 	subs	r6, r6, r2
    fbec:	e0c55003 	sbc	r5, r5, r3
    fbf0:	e1b030a3 	lsrs	r3, r3, #1
    fbf4:	e1a02062 	rrx	r2, r2
    fbf8:	e3a00601 	mov	r0, #1048576	; 0x100000
    fbfc:	e3a0c702 	mov	ip, #524288	; 0x80000
    fc00:	e056e002 	subs	lr, r6, r2
    fc04:	e0d5e003 	sbcs	lr, r5, r3
    fc08:	20466002 	subcs	r6, r6, r2
    fc0c:	21a0500e 	movcs	r5, lr
    fc10:	2180000c 	orrcs	r0, r0, ip
    fc14:	e1b030a3 	lsrs	r3, r3, #1
    fc18:	e1a02062 	rrx	r2, r2
    fc1c:	e056e002 	subs	lr, r6, r2
    fc20:	e0d5e003 	sbcs	lr, r5, r3
    fc24:	20466002 	subcs	r6, r6, r2
    fc28:	21a0500e 	movcs	r5, lr
    fc2c:	218000ac 	orrcs	r0, r0, ip, lsr #1
    fc30:	e1b030a3 	lsrs	r3, r3, #1
    fc34:	e1a02062 	rrx	r2, r2
    fc38:	e056e002 	subs	lr, r6, r2
    fc3c:	e0d5e003 	sbcs	lr, r5, r3
    fc40:	20466002 	subcs	r6, r6, r2
    fc44:	21a0500e 	movcs	r5, lr
    fc48:	2180012c 	orrcs	r0, r0, ip, lsr #2
    fc4c:	e1b030a3 	lsrs	r3, r3, #1
    fc50:	e1a02062 	rrx	r2, r2
    fc54:	e056e002 	subs	lr, r6, r2
    fc58:	e0d5e003 	sbcs	lr, r5, r3
    fc5c:	20466002 	subcs	r6, r6, r2
    fc60:	21a0500e 	movcs	r5, lr
    fc64:	218001ac 	orrcs	r0, r0, ip, lsr #3
    fc68:	e195e006 	orrs	lr, r5, r6
    fc6c:	0a00000d 	beq	fca8 <__aeabi_ddiv+0x134>
    fc70:	e1a05205 	lsl	r5, r5, #4
    fc74:	e1855e26 	orr	r5, r5, r6, lsr #28
    fc78:	e1a06206 	lsl	r6, r6, #4
    fc7c:	e1a03183 	lsl	r3, r3, #3
    fc80:	e1833ea2 	orr	r3, r3, r2, lsr #29
    fc84:	e1a02182 	lsl	r2, r2, #3
    fc88:	e1b0c22c 	lsrs	ip, ip, #4
    fc8c:	1affffdb 	bne	fc00 <__aeabi_ddiv+0x8c>
    fc90:	e3110601 	tst	r1, #1048576	; 0x100000
    fc94:	1a000006 	bne	fcb4 <__aeabi_ddiv+0x140>
    fc98:	e1811000 	orr	r1, r1, r0
    fc9c:	e3a00000 	mov	r0, #0	; 0x0
    fca0:	e3a0c102 	mov	ip, #-2147483648	; 0x80000000
    fca4:	eaffffd5 	b	fc00 <__aeabi_ddiv+0x8c>
    fca8:	e3110601 	tst	r1, #1048576	; 0x100000
    fcac:	01811000 	orreq	r1, r1, r0
    fcb0:	03a00000 	moveq	r0, #0	; 0x0
    fcb4:	e254c0fd 	subs	ip, r4, #253	; 0xfd
    fcb8:	835c0c07 	cmphi	ip, #1792	; 0x700
    fcbc:	8affff4a 	bhi	f9ec <__aeabi_dmul+0xe4>
    fcc0:	e055c003 	subs	ip, r5, r3
    fcc4:	0056c002 	subseq	ip, r6, r2
    fcc8:	01b0c0a0 	lsrseq	ip, r0, #1
    fccc:	e2b00000 	adcs	r0, r0, #0	; 0x0
    fcd0:	e0a11a04 	adc	r1, r1, r4, lsl #20
    fcd4:	e8bd8070 	pop	{r4, r5, r6, pc}
    fcd8:	e20ee102 	and	lr, lr, #-2147483648	; 0x80000000
    fcdc:	e18e1621 	orr	r1, lr, r1, lsr #12
    fce0:	e09440ac 	adds	r4, r4, ip, lsr #1
    fce4:	c074500c 	rsbsgt	r5, r4, ip
    fce8:	c1811a04 	orrgt	r1, r1, r4, lsl #20
    fcec:	c8bd8070 	popgt	{r4, r5, r6, pc}
    fcf0:	e3811601 	orr	r1, r1, #1048576	; 0x100000
    fcf4:	e3a0e000 	mov	lr, #0	; 0x0
    fcf8:	e2544001 	subs	r4, r4, #1	; 0x1
    fcfc:	eaffff3a 	b	f9ec <__aeabi_dmul+0xe4>
    fd00:	e185e006 	orr	lr, r5, r6
    fd04:	eaffff38 	b	f9ec <__aeabi_dmul+0xe4>
    fd08:	e00c5a23 	and	r5, ip, r3, lsr #20
    fd0c:	e134000c 	teq	r4, ip
    fd10:	0135000c 	teqeq	r5, ip
    fd14:	0affff93 	beq	fb68 <__aeabi_dmul+0x260>
    fd18:	e134000c 	teq	r4, ip
    fd1c:	1a000006 	bne	fd3c <__aeabi_ddiv+0x1c8>
    fd20:	e1904601 	orrs	r4, r0, r1, lsl #12
    fd24:	1affff8f 	bne	fb68 <__aeabi_dmul+0x260>
    fd28:	e135000c 	teq	r5, ip
    fd2c:	1affff87 	bne	fb50 <__aeabi_dmul+0x248>
    fd30:	e1a00002 	mov	r0, r2
    fd34:	e1a01003 	mov	r1, r3
    fd38:	eaffff8a 	b	fb68 <__aeabi_dmul+0x260>
    fd3c:	e135000c 	teq	r5, ip
    fd40:	1a000004 	bne	fd58 <__aeabi_ddiv+0x1e4>
    fd44:	e1925603 	orrs	r5, r2, r3, lsl #12
    fd48:	0affff6d 	beq	fb04 <__aeabi_dmul+0x1fc>
    fd4c:	e1a00002 	mov	r0, r2
    fd50:	e1a01003 	mov	r1, r3
    fd54:	eaffff83 	b	fb68 <__aeabi_dmul+0x260>
    fd58:	e1906081 	orrs	r6, r0, r1, lsl #1
    fd5c:	11926083 	orrsne	r6, r2, r3, lsl #1
    fd60:	1affff4d 	bne	fa9c <__aeabi_dmul+0x194>
    fd64:	e1904081 	orrs	r4, r0, r1, lsl #1
    fd68:	1affff78 	bne	fb50 <__aeabi_dmul+0x248>
    fd6c:	e1925083 	orrs	r5, r2, r3, lsl #1
    fd70:	1affff63 	bne	fb04 <__aeabi_dmul+0x1fc>
    fd74:	eaffff7b 	b	fb68 <__aeabi_dmul+0x260>

0000fd78 <__aeabi_d2iz>:
    fd78:	e1a02081 	lsl	r2, r1, #1
    fd7c:	e2922602 	adds	r2, r2, #2097152	; 0x200000
    fd80:	2a00000c 	bcs	fdb8 <__aeabi_d2iz+0x40>
    fd84:	5a000009 	bpl	fdb0 <__aeabi_d2iz+0x38>
    fd88:	e3e03e3e 	mvn	r3, #992	; 0x3e0
    fd8c:	e0532ac2 	subs	r2, r3, r2, asr #21
    fd90:	9a00000a 	bls	fdc0 <__aeabi_d2iz+0x48>
    fd94:	e1a03581 	lsl	r3, r1, #11
    fd98:	e3833102 	orr	r3, r3, #-2147483648	; 0x80000000
    fd9c:	e1833aa0 	orr	r3, r3, r0, lsr #21
    fda0:	e3110102 	tst	r1, #-2147483648	; 0x80000000
    fda4:	e1a00233 	lsr	r0, r3, r2
    fda8:	12600000 	rsbne	r0, r0, #0	; 0x0
    fdac:	e12fff1e 	bx	lr
    fdb0:	e3a00000 	mov	r0, #0	; 0x0
    fdb4:	e12fff1e 	bx	lr
    fdb8:	e1900601 	orrs	r0, r0, r1, lsl #12
    fdbc:	1a000002 	bne	fdcc <__aeabi_d2iz+0x54>
    fdc0:	e2110102 	ands	r0, r1, #-2147483648	; 0x80000000
    fdc4:	03e00102 	mvneq	r0, #-2147483648	; 0x80000000
    fdc8:	e12fff1e 	bx	lr
    fdcc:	e3a00000 	mov	r0, #0	; 0x0
    fdd0:	e12fff1e 	bx	lr

0000fdd4 <__div0>:
    fdd4:	e92d4002 	push	{r1, lr}
    fdd8:	e3a00008 	mov	r0, #8	; 0x8
    fddc:	ebfffcb1 	bl	f0a8 <raise>
    fde0:	e8bd8002 	pop	{r1, pc}
