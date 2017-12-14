
/home/neville/Work/q3f_irom/irom/out/vmirom:     file format elf32-littlearm


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
      44:	0800d1a8 	.word	0x0800d1a8

00000048 <_bss_end>:
      48:	0800dd08 	.word	0x0800dd08

0000004c <__stack>:
      4c:	0800cfe0 	.word	0x0800cfe0

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
      78:	eb0003bf 	bl	f7c <chip_test>
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
      9c:	00000520 	.word	0x00000520

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
     148:	2d009c2c 	.word	0x2d009c2c
     14c:	e1a00000 	.word	0xe1a00000

00000150 <bl_load_isi>:
     150:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     154:	e1a04001 	mov	r4, r1
     158:	e24dd02c 	sub	sp, sp, #44	; 0x2c
     15c:	e58d0024 	str	r0, [sp, #36]
     160:	e3a01001 	mov	r1, #1	; 0x1
     164:	e1a00004 	mov	r0, r4
     168:	e1a09002 	mov	r9, r2
     16c:	eb0013d1 	bl	50b8 <vs_assign_by_id>
     170:	e2506000 	subs	r6, r0, #0	; 0x0
     174:	0a000008 	beq	19c <bl_load_isi+0x4c>
     178:	e59f3178 	ldr	r3, [pc, #376]	; 2f8 <_data_len+0x154>
     17c:	e1a00004 	mov	r0, r4
     180:	e5934000 	ldr	r4, [r3]
     184:	eb0012d4 	bl	4cdc <vs_device_name>
     188:	e1a02006 	mov	r2, r6
     18c:	e1a01000 	mov	r1, r0
     190:	e59f0164 	ldr	r0, [pc, #356]	; 2fc <_data_len+0x158>
     194:	e12fff34 	blx	r4
     198:	ea000053 	b	2ec <_data_len+0x148>
     19c:	e1a00004 	mov	r0, r4
     1a0:	e59f1158 	ldr	r1, [pc, #344]	; 300 <_data_len+0x15c>
     1a4:	eb001303 	bl	4db8 <vs_is_device>
     1a8:	e8990030 	ldm	r9, {r4, r5}
     1ac:	e59f2150 	ldr	r2, [pc, #336]	; 304 <_data_len+0x160>
     1b0:	e3a03000 	mov	r3, #0	; 0x0
     1b4:	e0922004 	adds	r2, r2, r4
     1b8:	e59f4148 	ldr	r4, [pc, #328]	; 308 <_data_len+0x164>
     1bc:	e0a33005 	adc	r3, r3, r5
     1c0:	e3e05000 	mvn	r5, #0	; 0x0
     1c4:	e3500000 	cmp	r0, #0	; 0x0
     1c8:	e0033005 	and	r3, r3, r5
     1cc:	e0022004 	and	r2, r2, r4
     1d0:	03a08b01 	moveq	r8, #1024	; 0x400
     1d4:	13a08b02 	movne	r8, #2048	; 0x800
     1d8:	e889000c 	stm	r9, {r2, r3}
     1dc:	e1a02008 	mov	r2, r8
     1e0:	e1a03fc2 	asr	r3, r2, #31
     1e4:	e1cd21f8 	strd	r2, [sp, #24]
     1e8:	e1a07006 	mov	r7, r6
     1ec:	e58d6008 	str	r6, [sp, #8]
     1f0:	e1a05006 	mov	r5, r6
     1f4:	e58d600c 	str	r6, [sp, #12]
     1f8:	e1570008 	cmp	r7, r8
     1fc:	1a00000e 	bne	23c <_data_len+0x98>
     200:	e59d0024 	ldr	r0, [sp, #36]
     204:	eb0005b8 	bl	18ec <isi_check_header>
     208:	e2502000 	subs	r2, r0, #0	; 0x0
     20c:	e59f00f8 	ldr	r0, [pc, #248]	; 30c <_data_len+0x168>
     210:	1a000007 	bne	234 <_data_len+0x90>
     214:	e59da024 	ldr	sl, [sp, #36]
     218:	e59fb0d8 	ldr	fp, [pc, #216]	; 2f8 <_data_len+0x154>
     21c:	e59a300c 	ldr	r3, [sl, #12]
     220:	e2835c02 	add	r5, r3, #512	; 0x200
     224:	e1a01005 	mov	r1, r5
     228:	e1a0e00f 	mov	lr, pc
     22c:	e59bf000 	ldr	pc, [fp]
     230:	ea000001 	b	23c <_data_len+0x98>
     234:	e59f00d4 	ldr	r0, [pc, #212]	; 310 <_data_len+0x16c>
     238:	ea00000c 	b	270 <_data_len+0xcc>
     23c:	e2554000 	subs	r4, r5, #0	; 0x0
     240:	13a04001 	movne	r4, #1	; 0x1
     244:	e1570005 	cmp	r7, r5
     248:	e59d1024 	ldr	r1, [sp, #36]
     24c:	b3a04000 	movlt	r4, #0	; 0x0
     250:	e3540000 	cmp	r4, #0	; 0x0
     254:	e0810007 	add	r0, r1, r7
     258:	0a000006 	beq	278 <_data_len+0xd4>
     25c:	e1a00001 	mov	r0, r1
     260:	eb0005fa 	bl	1a50 <isi_check_data>
     264:	e3500000 	cmp	r0, #0	; 0x0
     268:	0a00001f 	beq	2ec <_data_len+0x148>
     26c:	e59f00a0 	ldr	r0, [pc, #160]	; 314 <_data_len+0x170>
     270:	e3a01000 	mov	r1, #0	; 0x0
     274:	ea000006 	b	294 <_data_len+0xf0>
     278:	e59d200c 	ldr	r2, [sp, #12]
     27c:	e3520004 	cmp	r2, #4	; 0x4
     280:	e2822001 	add	r2, r2, #1	; 0x1
     284:	e58d200c 	str	r2, [sp, #12]
     288:	da000005 	ble	2a4 <_data_len+0x100>
     28c:	e59f0080 	ldr	r0, [pc, #128]	; 314 <_data_len+0x170>
     290:	e1a01004 	mov	r1, r4
     294:	e59f305c 	ldr	r3, [pc, #92]	; 2f8 <_data_len+0x154>
     298:	e1a0e00f 	mov	lr, pc
     29c:	e593f000 	ldr	pc, [r3]
     2a0:	ea000010 	b	2e8 <_data_len+0x144>
     2a4:	e899000c 	ldm	r9, {r2, r3}
     2a8:	e58d8000 	str	r8, [sp]
     2ac:	e58d4004 	str	r4, [sp, #4]
     2b0:	eb0012d9 	bl	4e1c <vs_read>
     2b4:	e8990c00 	ldm	r9, {sl, fp}
     2b8:	e1cd21d8 	ldrd	r2, [sp, #24]
     2bc:	e09aa002 	adds	sl, sl, r2
     2c0:	e0abb003 	adc	fp, fp, r3
     2c4:	e59d3008 	ldr	r3, [sp, #8]
     2c8:	e1500008 	cmp	r0, r8
     2cc:	e2833001 	add	r3, r3, #1	; 0x1
     2d0:	00877008 	addeq	r7, r7, r8
     2d4:	058d400c 	streq	r4, [sp, #12]
     2d8:	e3530080 	cmp	r3, #128	; 0x80
     2dc:	e8890c00 	stm	r9, {sl, fp}
     2e0:	e58d3008 	str	r3, [sp, #8]
     2e4:	1affffc3 	bne	1f8 <_data_len+0x54>
     2e8:	e3e06000 	mvn	r6, #0	; 0x0
     2ec:	e1a00006 	mov	r0, r6
     2f0:	e28dd02c 	add	sp, sp, #44	; 0x2c
     2f4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     2f8:	0800d76c 	.word	0x0800d76c
     2fc:	000119ca 	.word	0x000119ca
     300:	000119ee 	.word	0x000119ee
     304:	0001ffff 	.word	0x0001ffff
     308:	fffe0000 	.word	0xfffe0000
     30c:	000119f3 	.word	0x000119f3
     310:	00011a0a 	.word	0x00011a0a
     314:	00011a1c 	.word	0x00011a1c

00000318 <try_get_bl>:
     318:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     31c:	e3a02602 	mov	r2, #2097152	; 0x200000
     320:	e24dd00c 	sub	sp, sp, #12	; 0xc
     324:	e3a03000 	mov	r3, #0	; 0x0
     328:	e88d000c 	stm	sp, {r2, r3}
     32c:	e1a07001 	mov	r7, r1
     330:	e1a06000 	mov	r6, r0
     334:	eb0000aa 	bl	5e4 <boot_device>
     338:	e1a04000 	mov	r4, r0
     33c:	eb00121f 	bl	4bc0 <vs_device_bootable>
     340:	e2505000 	subs	r5, r0, #0	; 0x0
     344:	1a000008 	bne	36c <try_get_bl+0x54>
     348:	e59f308c 	ldr	r3, [pc, #140]	; 3dc <try_get_bl+0xc4>
     34c:	e1a00004 	mov	r0, r4
     350:	e5934000 	ldr	r4, [r3]
     354:	eb001260 	bl	4cdc <vs_device_name>
     358:	e1a01000 	mov	r1, r0
     35c:	e59f007c 	ldr	r0, [pc, #124]	; 3e0 <try_get_bl+0xc8>
     360:	e12fff34 	blx	r4
     364:	e1a00005 	mov	r0, r5
     368:	ea000019 	b	3d4 <try_get_bl+0xbc>
     36c:	e1a00004 	mov	r0, r4
     370:	eb001247 	bl	4c94 <vs_device_boot0>
     374:	e3500000 	cmp	r0, #0	; 0x0
     378:	13a02000 	movne	r2, #0	; 0x0
     37c:	13a03000 	movne	r3, #0	; 0x0
     380:	188d000c 	stmne	sp, {r2, r3}
     384:	e3a00302 	mov	r0, #134217728	; 0x8000000
     388:	e1a01004 	mov	r1, r4
     38c:	e1a0200d 	mov	r2, sp
     390:	e3a03000 	mov	r3, #0	; 0x0
     394:	ebffff6d 	bl	150 <bl_load_isi>
     398:	e3500000 	cmp	r0, #0	; 0x0
     39c:	13a00000 	movne	r0, #0	; 0x0
     3a0:	1a00000b 	bne	3d4 <try_get_bl+0xbc>
     3a4:	e3a03302 	mov	r3, #134217728	; 0x8000000
     3a8:	e5932008 	ldr	r2, [r3, #8]
     3ac:	e3c22102 	bic	r2, r2, #-2147483648	; 0x80000000
     3b0:	e5862000 	str	r2, [r6]
     3b4:	e5874000 	str	r4, [r7]
     3b8:	e5933008 	ldr	r3, [r3, #8]
     3bc:	e3530000 	cmp	r3, #0	; 0x0
     3c0:	aa000002 	bge	3d0 <try_get_bl+0xb8>
     3c4:	e3a00001 	mov	r0, #1	; 0x1
     3c8:	e59f1014 	ldr	r1, [pc, #20]	; 3e4 <try_get_bl+0xcc>
     3cc:	eb00170d 	bl	6008 <cdbg_verify_burn_enable>
     3d0:	e3a00001 	mov	r0, #1	; 0x1
     3d4:	e28dd00c 	add	sp, sp, #12	; 0xc
     3d8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     3dc:	0800d76c 	.word	0x0800d76c
     3e0:	00011a2e 	.word	0x00011a2e
     3e4:	08000100 	.word	0x08000100

000003e8 <try_boot>:
     3e8:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
     3ec:	e3a03000 	mov	r3, #0	; 0x0
     3f0:	e24dd008 	sub	sp, sp, #8	; 0x8
     3f4:	e1a07000 	mov	r7, r0
     3f8:	e1a00003 	mov	r0, r3
     3fc:	e58d3004 	str	r3, [sp, #4]
     400:	e58d3000 	str	r3, [sp]
     404:	e1a04003 	mov	r4, r3
     408:	eb00005b 	bl	57c <set_xom>
     40c:	eb000061 	bl	598 <boot_info>
     410:	e28d8004 	add	r8, sp, #4	; 0x4
     414:	e1a05000 	mov	r5, r0
     418:	ea000008 	b	440 <try_boot+0x58>
     41c:	eb00012e 	bl	8dc <gtc_time>
     420:	e1a00004 	mov	r0, r4
     424:	eb000054 	bl	57c <set_xom>
     428:	e1a00008 	mov	r0, r8
     42c:	e1a0100d 	mov	r1, sp
     430:	ebffffb8 	bl	318 <try_get_bl>
     434:	e3500000 	cmp	r0, #0	; 0x0
     438:	13a04001 	movne	r4, #1	; 0x1
     43c:	1a000003 	bne	450 <try_boot+0x68>
     440:	e1540005 	cmp	r4, r5
     444:	e2844001 	add	r4, r4, #1	; 0x1
     448:	bafffff3 	blt	41c <try_boot+0x34>
     44c:	e3a04000 	mov	r4, #0	; 0x0
     450:	eb000121 	bl	8dc <gtc_time>
     454:	e3a00001 	mov	r0, #1	; 0x1
     458:	eb000135 	bl	934 <ecfg_check_flag>
     45c:	e3500000 	cmp	r0, #0	; 0x0
     460:	1a00000c 	bne	498 <try_boot+0xb0>
     464:	e3570000 	cmp	r7, #0	; 0x0
     468:	1a00000a 	bne	498 <try_boot+0xb0>
     46c:	eb0000cb 	bl	7a0 <get_reset_stats>
     470:	e3500000 	cmp	r0, #0	; 0x0
     474:	13500002 	cmpne	r0, #2	; 0x2
     478:	1a000006 	bne	498 <try_boot+0xb0>
     47c:	e3500002 	cmp	r0, #2	; 0x2
     480:	1a000001 	bne	48c <try_boot+0xa4>
     484:	e28d0004 	add	r0, sp, #4	; 0x4
     488:	eb0000d1 	bl	7d4 <update_mask_on_reset>
     48c:	e59d0004 	ldr	r0, [sp, #4]
     490:	eb00189a 	bl	6700 <burn>
     494:	eb000110 	bl	8dc <gtc_time>
     498:	e3540000 	cmp	r4, #0	; 0x0
     49c:	0a000004 	beq	4b4 <try_boot+0xcc>
     4a0:	e3a03302 	mov	r3, #134217728	; 0x8000000
     4a4:	e5930014 	ldr	r0, [r3, #20]
     4a8:	e59d1000 	ldr	r1, [sp]
     4ac:	eb0016e0 	bl	6034 <cdbg_jump>
     4b0:	eafffffe 	b	4b0 <try_boot+0xc8>
     4b4:	eb001710 	bl	60fc <cdbg_shell>
     4b8:	eafffffe 	b	4b8 <try_boot+0xd0>

000004bc <init_data_section>:
     4bc:	e59f1010 	ldr	r1, [pc, #16]	; 4d4 <init_data_section+0x18>
     4c0:	e59f0010 	ldr	r0, [pc, #16]	; 4d8 <init_data_section+0x1c>
     4c4:	e1510000 	cmp	r1, r0
     4c8:	012fff1e 	bxeq	lr
     4cc:	e59f2008 	ldr	r2, [pc, #8]	; 4dc <init_data_section+0x20>
     4d0:	ea00039d 	b	134c <memcpy>
     4d4:	0001474c 	.word	0x0001474c
     4d8:	0800d000 	.word	0x0800d000
     4dc:	000001a4 	.word	0x000001a4

000004e0 <initialization>:
     4e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     4e4:	e24dd004 	sub	sp, sp, #4	; 0x4
     4e8:	ebfffff3 	bl	4bc <init_data_section>
     4ec:	eb000027 	bl	590 <init_xom>
     4f0:	eb000099 	bl	75c <init_lowlevel>
     4f4:	eb0004be 	bl	17f4 <init_malloc>
     4f8:	eb00211a 	bl	8968 <init_timer>
     4fc:	eb000103 	bl	910 <init_gtc>
     500:	eb0020ab 	bl	87b4 <init_serial>
     504:	eb0019d8 	bl	6c6c <init_mmc>
     508:	eb0000a3 	bl	79c <init_crypto>
     50c:	eb000220 	bl	d94 <init_alternate>
     510:	eb00001f 	bl	594 <init_boot_sequence>
     514:	e3a00000 	mov	r0, #0	; 0x0
     518:	e28dd004 	add	sp, sp, #4	; 0x4
     51c:	e8bd8000 	pop	{pc}

00000520 <main>:
     520:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     524:	e3a00000 	mov	r0, #0	; 0x0
     528:	e24dd004 	sub	sp, sp, #4	; 0x4
     52c:	eb001699 	bl	5f98 <cdbg_log_toggle>
     530:	ebffffea 	bl	4e0 <initialization>
     534:	eb0000e8 	bl	8dc <gtc_time>
     538:	e3a00001 	mov	r0, #1	; 0x1
     53c:	eb001695 	bl	5f98 <cdbg_log_toggle>
     540:	e59f001c 	ldr	r0, [pc, #28]	; 564 <main+0x44>
     544:	e59f301c 	ldr	r3, [pc, #28]	; 568 <main+0x48>
     548:	e1a0e00f 	mov	lr, pc
     54c:	e593f000 	ldr	pc, [r3]
     550:	e3a00000 	mov	r0, #0	; 0x0
     554:	eb00168f 	bl	5f98 <cdbg_log_toggle>
     558:	e3a00000 	mov	r0, #0	; 0x0
     55c:	ebffffa1 	bl	3e8 <try_boot>
     560:	eafffffe 	b	560 <main+0x40>
     564:	00011a48 	.word	0x00011a48
     568:	0800d76c 	.word	0x0800d76c

0000056c <get_xom>:
     56c:	e59f3004 	ldr	r3, [pc, #4]	; 578 <get_xom+0xc>
     570:	e1d300b0 	ldrh	r0, [r3]
     574:	e12fff1e 	bx	lr
     578:	0800d1a8 	.word	0x0800d1a8

0000057c <set_xom>:
     57c:	e59f3008 	ldr	r3, [pc, #8]	; 58c <set_xom+0x10>
     580:	e200001f 	and	r0, r0, #31	; 0x1f
     584:	e1c300b0 	strh	r0, [r3]
     588:	e12fff1e 	bx	lr
     58c:	0800d1a8 	.word	0x0800d1a8

00000590 <init_xom>:
     590:	e12fff1e 	bx	lr

00000594 <init_boot_sequence>:
     594:	e12fff1e 	bx	lr

00000598 <boot_info>:
     598:	e92d4010 	push	{r4, lr}
     59c:	eb0000ed 	bl	958 <ecfg_paras>
     5a0:	e1a04000 	mov	r4, r0
     5a4:	e3a00b01 	mov	r0, #1024	; 0x400
     5a8:	eb0000e1 	bl	934 <ecfg_check_flag>
     5ac:	e59f3028 	ldr	r3, [pc, #40]	; 5dc <boot_info+0x44>
     5b0:	e3500000 	cmp	r0, #0	; 0x0
     5b4:	11d330b0 	ldrhne	r3, [r3]
     5b8:	01d330b0 	ldrheq	r3, [r3]
     5bc:	1203301f 	andne	r3, r3, #31	; 0x1f
     5c0:	0203301f 	andeq	r3, r3, #31	; 0x1f
     5c4:	159f2014 	ldrne	r2, [pc, #20]	; 5e0 <boot_info+0x48>
     5c8:	11a03083 	lslne	r3, r3, #1
     5cc:	00843083 	addeq	r3, r4, r3, lsl #1
     5d0:	119200b3 	ldrhne	r0, [r2, r3]
     5d4:	01d302b0 	ldrheq	r0, [r3, #32]
     5d8:	e8bd8010 	pop	{r4, pc}
     5dc:	0800d1a8 	.word	0x0800d1a8
     5e0:	000115e4 	.word	0x000115e4

000005e4 <boot_device>:
     5e4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     5e8:	e24dd004 	sub	sp, sp, #4	; 0x4
     5ec:	ebffffe9 	bl	598 <boot_info>
     5f0:	e1a00620 	lsr	r0, r0, #12
     5f4:	e6ff0070 	uxth	r0, r0
     5f8:	e28dd004 	add	sp, sp, #4	; 0x4
     5fc:	e8bd8000 	pop	{pc}

00000600 <set_pll>:
     600:	e3500003 	cmp	r0, #3	; 0x3
     604:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
     608:	e1a02001 	mov	r2, r1
     60c:	88bd85f0 	pophi	{r4, r5, r6, r7, r8, sl, pc}
     610:	e3a03018 	mov	r3, #24	; 0x18
     614:	e0030093 	mul	r3, r3, r0
     618:	e59f808c 	ldr	r8, [pc, #140]	; 6ac <set_pll+0xac>
     61c:	e59f008c 	ldr	r0, [pc, #140]	; 6b0 <set_pll+0xb0>
     620:	e59fc08c 	ldr	ip, [pc, #140]	; 6b4 <set_pll+0xb4>
     624:	e59f508c 	ldr	r5, [pc, #140]	; 6b8 <set_pll+0xb8>
     628:	e59fa08c 	ldr	sl, [pc, #140]	; 6bc <set_pll+0xbc>
     62c:	e283142d 	add	r1, r3, #754974720	; 0x2d000000
     630:	e0838008 	add	r8, r3, r8
     634:	e083c00c 	add	ip, r3, ip
     638:	e3a07001 	mov	r7, #1	; 0x1
     63c:	e2811a0a 	add	r1, r1, #40960	; 0xa000
     640:	e0830000 	add	r0, r3, r0
     644:	e1a06422 	lsr	r6, r2, #8
     648:	e0835005 	add	r5, r3, r5
     64c:	e3a04000 	mov	r4, #0	; 0x0
     650:	e6ef2072 	uxtb	r2, r2
     654:	e083a00a 	add	sl, r3, sl
     658:	e3a03002 	mov	r3, #2	; 0x2
     65c:	e5c87000 	strb	r7, [r8]
     660:	e5c12000 	strb	r2, [r1]
     664:	e5c06000 	strb	r6, [r0]
     668:	e5cc4000 	strb	r4, [ip]
     66c:	e5c53000 	strb	r3, [r5]
     670:	e0833007 	add	r3, r3, r7
     674:	e5c83000 	strb	r3, [r8]
     678:	e1a02004 	mov	r2, r4
     67c:	e5cc7000 	strb	r7, [ip]
     680:	e3011a0a 	movw	r1, #6666	; 0x1a0a
     684:	ea000001 	b	690 <set_pll+0x90>
     688:	e1520001 	cmp	r2, r1
     68c:	0a000003 	beq	6a0 <set_pll+0xa0>
     690:	e5da3000 	ldrb	r3, [sl]
     694:	e2822001 	add	r2, r2, #1	; 0x1
     698:	e3130001 	tst	r3, #1	; 0x1
     69c:	0afffff9 	beq	688 <set_pll+0x88>
     6a0:	e3a03002 	mov	r3, #2	; 0x2
     6a4:	e5883000 	str	r3, [r8]
     6a8:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
     6ac:	2d00a00c 	.word	0x2d00a00c
     6b0:	2d00a004 	.word	0x2d00a004
     6b4:	2d00a010 	.word	0x2d00a010
     6b8:	2d00a008 	.word	0x2d00a008
     6bc:	2d00a014 	.word	0x2d00a014

000006c0 <set_clk_divider>:
     6c0:	e3500005 	cmp	r0, #5	; 0x5
     6c4:	812fff1e 	bxhi	lr
     6c8:	e59fc020 	ldr	ip, [pc, #32]	; 6f0 <set_clk_divider+0x30>
     6cc:	e1a03280 	lsl	r3, r0, #5
     6d0:	e083c00c 	add	ip, r3, ip
     6d4:	e3a02000 	mov	r2, #0	; 0x0
     6d8:	e7d13002 	ldrb	r3, [r1, r2]
     6dc:	e2822001 	add	r2, r2, #1	; 0x1
     6e0:	e3520008 	cmp	r2, #8	; 0x8
     6e4:	e4cc3004 	strb	r3, [ip], #4
     6e8:	1afffffa 	bne	6d8 <set_clk_divider+0x18>
     6ec:	e12fff1e 	bx	lr
     6f0:	2d00a070 	.word	0x2d00a070

000006f4 <module_set_clock>:
     6f4:	e59f3020 	ldr	r3, [pc, #32]	; 71c <module_set_clock+0x28>
     6f8:	e1500003 	cmp	r0, r3
     6fc:	13a03001 	movne	r3, #1	; 0x1
     700:	1580300c 	strne	r3, [r0, #12]
     704:	e2013007 	and	r3, r1, #7	; 0x7
     708:	e5803000 	str	r3, [r0]
     70c:	e3a03000 	mov	r3, #0	; 0x0
     710:	e5802004 	str	r2, [r0, #4]
     714:	e580300c 	str	r3, [r0, #12]
     718:	e12fff1e 	bx	lr
     71c:	2d00a360 	.word	0x2d00a360

00000720 <module_enable>:
     720:	e3a020ff 	mov	r2, #255	; 0xff
     724:	e3a01000 	mov	r1, #0	; 0x0
     728:	e3a03011 	mov	r3, #17	; 0x11
     72c:	e580100c 	str	r1, [r0, #12]
     730:	e5802000 	str	r2, [r0]
     734:	e5802004 	str	r2, [r0, #4]
     738:	e5803008 	str	r3, [r0, #8]
     73c:	e5801014 	str	r1, [r0, #20]
     740:	e5903008 	ldr	r3, [r0, #8]
     744:	e3c33010 	bic	r3, r3, #16	; 0x10
     748:	e5803008 	str	r3, [r0, #8]
     74c:	e5802018 	str	r2, [r0, #24]
     750:	e5801000 	str	r1, [r0]
     754:	e580200c 	str	r2, [r0, #12]
     758:	e12fff1e 	bx	lr

0000075c <init_lowlevel>:
     75c:	e3a01000 	mov	r1, #0	; 0x0
     760:	e3e00000 	mvn	r0, #0	; 0x0
     764:	e59f3018 	ldr	r3, [pc, #24]	; 784 <init_lowlevel+0x28>
     768:	e1a02101 	lsl	r2, r1, #2
     76c:	e2811001 	add	r1, r1, #1	; 0x1
     770:	e0823003 	add	r3, r2, r3
     774:	e3510008 	cmp	r1, #8	; 0x8
     778:	e5830000 	str	r0, [r3]
     77c:	1afffff8 	bne	764 <init_lowlevel+0x8>
     780:	e12fff1e 	bx	lr
     784:	21b01180 	.word	0x21b01180

00000788 <init_igps_ram>:
     788:	e59f3008 	ldr	r3, [pc, #8]	; 798 <init_igps_ram+0x10>
     78c:	e3a02001 	mov	r2, #1	; 0x1
     790:	e5832410 	str	r2, [r3, #1040]
     794:	e12fff1e 	bx	lr
     798:	2d00c000 	.word	0x2d00c000

0000079c <init_crypto>:
     79c:	e12fff1e 	bx	lr

000007a0 <get_reset_stats>:
     7a0:	e59f2028 	ldr	r2, [pc, #40]	; 7d0 <get_reset_stats+0x30>
     7a4:	e5923c10 	ldr	r3, [r2, #3088]
     7a8:	e2130002 	ands	r0, r3, #2	; 0x2
     7ac:	012fff1e 	bxeq	lr
     7b0:	e5920c2c 	ldr	r0, [r2, #3116]
     7b4:	e3100001 	tst	r0, #1	; 0x1
     7b8:	03a00001 	moveq	r0, #1	; 0x1
     7bc:	012fff1e 	bxeq	lr
     7c0:	e3500037 	cmp	r0, #55	; 0x37
     7c4:	03a00002 	moveq	r0, #2	; 0x2
     7c8:	13a00003 	movne	r0, #3	; 0x3
     7cc:	e12fff1e 	bx	lr
     7d0:	2d009000 	.word	0x2d009000

000007d4 <update_mask_on_reset>:
     7d4:	e59f3018 	ldr	r3, [pc, #24]	; 7f4 <update_mask_on_reset+0x20>
     7d8:	e5932c30 	ldr	r2, [r3, #3120]
     7dc:	e3120080 	tst	r2, #128	; 0x80
     7e0:	13a03001 	movne	r3, #1	; 0x1
     7e4:	1202200f 	andne	r2, r2, #15	; 0xf
     7e8:	11e03213 	mvnne	r3, r3, lsl r2
     7ec:	15803000 	strne	r3, [r0]
     7f0:	e12fff1e 	bx	lr
     7f4:	2d009000 	.word	0x2d009000

000007f8 <get_wakeup_entry>:
     7f8:	e59f301c 	ldr	r3, [pc, #28]	; 81c <get_wakeup_entry+0x24>
     7fc:	e5931c38 	ldr	r1, [r3, #3128]
     800:	e5932c34 	ldr	r2, [r3, #3124]
     804:	e5930c30 	ldr	r0, [r3, #3120]
     808:	e1822401 	orr	r2, r2, r1, lsl #8
     80c:	e5933c2c 	ldr	r3, [r3, #3116]
     810:	e1800402 	orr	r0, r0, r2, lsl #8
     814:	e1830400 	orr	r0, r3, r0, lsl #8
     818:	e12fff1e 	bx	lr
     81c:	2d009000 	.word	0x2d009000

00000820 <system_reset>:
     820:	e59f3010 	ldr	r3, [pc, #16]	; 838 <system_reset+0x18>
     824:	e3a02001 	mov	r2, #1	; 0x1
     828:	e5832c2c 	str	r2, [r3, #3116]
     82c:	e2822002 	add	r2, r2, #2	; 0x2
     830:	e5832c00 	str	r2, [r3, #3072]
     834:	e12fff1e 	bx	lr
     838:	2d009000 	.word	0x2d009000

0000083c <pads_init_module>:
     83c:	e92d4030 	push	{r4, r5, lr}
     840:	e1a05000 	mov	r5, r0
     844:	e24dd004 	sub	sp, sp, #4	; 0x4
     848:	e59f4064 	ldr	r4, [pc, #100]	; 8b4 <pads_init_module+0x78>
     84c:	ea000013 	b	8a0 <pads_init_module+0x64>
     850:	e1500005 	cmp	r0, r5
     854:	1a000010 	bne	89c <pads_init_module+0x60>
     858:	e3a01000 	mov	r1, #0	; 0x0
     85c:	e1a02001 	mov	r2, r1
     860:	e514000c 	ldr	r0, [r4, #-12]
     864:	eb0000c6 	bl	b84 <pads_chmod>
     868:	e5140004 	ldr	r0, [r4, #-4]
     86c:	e3a01000 	mov	r1, #0	; 0x0
     870:	e1500001 	cmp	r0, r1
     874:	e1a02001 	mov	r2, r1
     878:	da000000 	ble	880 <pads_init_module+0x44>
     87c:	eb0000c0 	bl	b84 <pads_chmod>
     880:	e5943000 	ldr	r3, [r4]
     884:	e3a01000 	mov	r1, #0	; 0x0
     888:	e1530001 	cmp	r3, r1
     88c:	e1a02001 	mov	r2, r1
     890:	e1a00003 	mov	r0, r3
     894:	da000000 	ble	89c <pads_init_module+0x60>
     898:	eb0000b9 	bl	b84 <pads_chmod>
     89c:	e2844014 	add	r4, r4, #20	; 0x14
     8a0:	e5140010 	ldr	r0, [r4, #-16]
     8a4:	e3500000 	cmp	r0, #0	; 0x0
     8a8:	1affffe8 	bne	850 <pads_init_module+0x14>
     8ac:	e28dd004 	add	sp, sp, #4	; 0x4
     8b0:	e8bd8030 	pop	{r4, r5, pc}
     8b4:	0800d010 	.word	0x0800d010

000008b8 <module_reset>:
     8b8:	e3a030ff 	mov	r3, #255	; 0xff
     8bc:	e5803000 	str	r3, [r0]
     8c0:	e92d4010 	push	{r4, lr}
     8c4:	e1a04000 	mov	r4, r0
     8c8:	e3a0000a 	mov	r0, #10	; 0xa
     8cc:	eb001fe1 	bl	8858 <udelay>
     8d0:	e3a03000 	mov	r3, #0	; 0x0
     8d4:	e5843000 	str	r3, [r4]
     8d8:	e8bd8010 	pop	{r4, pc}

000008dc <gtc_time>:
     8dc:	e59f3020 	ldr	r3, [pc, #32]	; 904 <gtc_time+0x28>
     8e0:	e92d4010 	push	{r4, lr}
     8e4:	e5930000 	ldr	r0, [r3]
     8e8:	e59f3018 	ldr	r3, [pc, #24]	; 908 <gtc_time+0x2c>
     8ec:	e5934000 	ldr	r4, [r3]
     8f0:	eb001ffc 	bl	88e8 <get_timer>
     8f4:	e1a01000 	mov	r1, r0
     8f8:	e59f000c 	ldr	r0, [pc, #12]	; 90c <gtc_time+0x30>
     8fc:	e12fff34 	blx	r4
     900:	e8bd8010 	pop	{r4, pc}
     904:	0800d1ac 	.word	0x0800d1ac
     908:	0800d76c 	.word	0x0800d76c
     90c:	00011a4d 	.word	0x00011a4d

00000910 <init_gtc>:
     910:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     914:	e3a00000 	mov	r0, #0	; 0x0
     918:	e24dd004 	sub	sp, sp, #4	; 0x4
     91c:	eb001ff1 	bl	88e8 <get_timer>
     920:	e59f3008 	ldr	r3, [pc, #8]	; 930 <init_gtc+0x20>
     924:	e5830000 	str	r0, [r3]
     928:	e28dd004 	add	sp, sp, #4	; 0x4
     92c:	e8bd8000 	pop	{pc}
     930:	0800d1ac 	.word	0x0800d1ac

00000934 <ecfg_check_flag>:
     934:	e3500801 	cmp	r0, #65536	; 0x10000
     938:	0a000004 	beq	950 <ecfg_check_flag+0x1c>
     93c:	e3500101 	cmp	r0, #1073741824	; 0x40000000
     940:	0a000002 	beq	950 <ecfg_check_flag+0x1c>
     944:	e3500001 	cmp	r0, #1	; 0x1
     948:	13a00001 	movne	r0, #1	; 0x1
     94c:	112fff1e 	bxne	lr
     950:	e3a00000 	mov	r0, #0	; 0x0
     954:	e12fff1e 	bx	lr

00000958 <ecfg_paras>:
     958:	e59f0000 	ldr	r0, [pc, #0]	; 960 <ecfg_paras+0x8>
     95c:	e12fff1e 	bx	lr
     960:	0800d770 	.word	0x0800d770

00000964 <efuse_mac>:
     964:	e3a00000 	mov	r0, #0	; 0x0
     968:	e3a01000 	mov	r1, #0	; 0x0
     96c:	e12fff1e 	bx	lr

00000970 <efuse_info>:
     970:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     974:	e1a04000 	mov	r4, r0
     978:	e24dd004 	sub	sp, sp, #4	; 0x4
     97c:	e59f6144 	ldr	r6, [pc, #324]	; ac8 <efuse_info+0x158>
     980:	e59f0144 	ldr	r0, [pc, #324]	; acc <efuse_info+0x15c>
     984:	e1a0e00f 	mov	lr, pc
     988:	e596f000 	ldr	pc, [r6]
     98c:	e59f013c 	ldr	r0, [pc, #316]	; ad0 <efuse_info+0x160>
     990:	e1a0e00f 	mov	lr, pc
     994:	e596f000 	ldr	pc, [r6]
     998:	e3540000 	cmp	r4, #0	; 0x0
     99c:	0a00003c 	beq	a94 <efuse_info+0x124>
     9a0:	e59f412c 	ldr	r4, [pc, #300]	; ad4 <efuse_info+0x164>
     9a4:	e59f012c 	ldr	r0, [pc, #300]	; ad8 <efuse_info+0x168>
     9a8:	e5941000 	ldr	r1, [r4]
     9ac:	e1a0e00f 	mov	lr, pc
     9b0:	e596f000 	ldr	pc, [r6]
     9b4:	e5d41004 	ldrb	r1, [r4, #4]
     9b8:	e59f011c 	ldr	r0, [pc, #284]	; adc <efuse_info+0x16c>
     9bc:	e1a0e00f 	mov	lr, pc
     9c0:	e596f000 	ldr	pc, [r6]
     9c4:	e5d41005 	ldrb	r1, [r4, #5]
     9c8:	e59f0110 	ldr	r0, [pc, #272]	; ae0 <efuse_info+0x170>
     9cc:	e1a0e00f 	mov	lr, pc
     9d0:	e596f000 	ldr	pc, [r6]
     9d4:	e5d41006 	ldrb	r1, [r4, #6]
     9d8:	e59f0104 	ldr	r0, [pc, #260]	; ae4 <efuse_info+0x174>
     9dc:	e1a0e00f 	mov	lr, pc
     9e0:	e596f000 	ldr	pc, [r6]
     9e4:	e5d41007 	ldrb	r1, [r4, #7]
     9e8:	e59f00f8 	ldr	r0, [pc, #248]	; ae8 <efuse_info+0x178>
     9ec:	e1a0e00f 	mov	lr, pc
     9f0:	e596f000 	ldr	pc, [r6]
     9f4:	e1d410b8 	ldrh	r1, [r4, #8]
     9f8:	e59f00ec 	ldr	r0, [pc, #236]	; aec <efuse_info+0x17c>
     9fc:	e1a0e00f 	mov	lr, pc
     a00:	e596f000 	ldr	pc, [r6]
     a04:	e1d410ba 	ldrh	r1, [r4, #10]
     a08:	e59f00e0 	ldr	r0, [pc, #224]	; af0 <efuse_info+0x180>
     a0c:	e1a0e00f 	mov	lr, pc
     a10:	e596f000 	ldr	pc, [r6]
     a14:	e1d410bc 	ldrh	r1, [r4, #12]
     a18:	e59f00d4 	ldr	r0, [pc, #212]	; af4 <efuse_info+0x184>
     a1c:	e1a0e00f 	mov	lr, pc
     a20:	e596f000 	ldr	pc, [r6]
     a24:	e1d410be 	ldrh	r1, [r4, #14]
     a28:	e59f00c8 	ldr	r0, [pc, #200]	; af8 <efuse_info+0x188>
     a2c:	e1a0e00f 	mov	lr, pc
     a30:	e596f000 	ldr	pc, [r6]
     a34:	e1a05004 	mov	r5, r4
     a38:	e3a04000 	mov	r4, #0	; 0x0
     a3c:	e1a01004 	mov	r1, r4
     a40:	e5952010 	ldr	r2, [r5, #16]
     a44:	e2844001 	add	r4, r4, #1	; 0x1
     a48:	e59f00ac 	ldr	r0, [pc, #172]	; afc <efuse_info+0x18c>
     a4c:	e1a0e00f 	mov	lr, pc
     a50:	e596f000 	ldr	pc, [r6]
     a54:	e3540004 	cmp	r4, #4	; 0x4
     a58:	e2855004 	add	r5, r5, #4	; 0x4
     a5c:	1afffff6 	bne	a3c <efuse_info+0xcc>
     a60:	e59f6060 	ldr	r6, [pc, #96]	; ac8 <efuse_info+0x158>
     a64:	e59f5068 	ldr	r5, [pc, #104]	; ad4 <efuse_info+0x164>
     a68:	e284400c 	add	r4, r4, #12	; 0xc
     a6c:	e1a03084 	lsl	r3, r4, #1
     a70:	e1a01004 	mov	r1, r4
     a74:	e19520b3 	ldrh	r2, [r5, r3]
     a78:	e2844001 	add	r4, r4, #1	; 0x1
     a7c:	e59f007c 	ldr	r0, [pc, #124]	; b00 <efuse_info+0x190>
     a80:	e1a0e00f 	mov	lr, pc
     a84:	e596f000 	ldr	pc, [r6]
     a88:	e354001c 	cmp	r4, #28	; 0x1c
     a8c:	1afffff6 	bne	a6c <efuse_info+0xfc>
     a90:	ea00000a 	b	ac0 <efuse_info+0x150>
     a94:	e59f4068 	ldr	r4, [pc, #104]	; b04 <efuse_info+0x194>
     a98:	e59f7068 	ldr	r7, [pc, #104]	; b08 <efuse_info+0x198>
     a9c:	e1a05006 	mov	r5, r6
     aa0:	e5942000 	ldr	r2, [r4]
     aa4:	e1a01004 	mov	r1, r4
     aa8:	e59f005c 	ldr	r0, [pc, #92]	; b0c <efuse_info+0x19c>
     aac:	e2844004 	add	r4, r4, #4	; 0x4
     ab0:	e1a0e00f 	mov	lr, pc
     ab4:	e595f000 	ldr	pc, [r5]
     ab8:	e1540007 	cmp	r4, r7
     abc:	1afffff7 	bne	aa0 <efuse_info+0x130>
     ac0:	e28dd004 	add	sp, sp, #4	; 0x4
     ac4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     ac8:	0800d76c 	.word	0x0800d76c
     acc:	00011a55 	.word	0x00011a55
     ad0:	00011a6a 	.word	0x00011a6a
     ad4:	0800d770 	.word	0x0800d770
     ad8:	00011a81 	.word	0x00011a81
     adc:	00011a90 	.word	0x00011a90
     ae0:	00011a9f 	.word	0x00011a9f
     ae4:	00011ab2 	.word	0x00011ab2
     ae8:	00011ac1 	.word	0x00011ac1
     aec:	00011ad6 	.word	0x00011ad6
     af0:	00011ae3 	.word	0x00011ae3
     af4:	00011af3 	.word	0x00011af3
     af8:	00011b04 	.word	0x00011b04
     afc:	00011b16 	.word	0x00011b16
     b00:	00011b26 	.word	0x00011b26
     b04:	2d00d9b4 	.word	0x2d00d9b4
     b08:	2d00da94 	.word	0x2d00da94
     b0c:	00011b37 	.word	0x00011b37

00000b10 <init_efuse>:
     b10:	e59f0008 	ldr	r0, [pc, #8]	; b20 <init_efuse+0x10>
     b14:	e3a01000 	mov	r1, #0	; 0x0
     b18:	e3a02038 	mov	r2, #56	; 0x38
     b1c:	ea000202 	b	132c <memset>
     b20:	0800d770 	.word	0x0800d770

00000b24 <assert_linkpc>:
     b24:	e3a00001 	mov	r0, #1	; 0x1
     b28:	e12fff1e 	bx	lr

00000b2c <pads_states>:
     b2c:	e3500000 	cmp	r0, #0	; 0x0
     b30:	e2803007 	add	r3, r0, #7	; 0x7
     b34:	a1a03000 	movge	r3, r0
     b38:	e1a031c3 	asr	r3, r3, #3
     b3c:	e59f203c 	ldr	r2, [pc, #60]	; b80 <pads_states+0x54>
     b40:	e1a03103 	lsl	r3, r3, #2
     b44:	e0832002 	add	r2, r3, r2
     b48:	e5921000 	ldr	r1, [r2]
     b4c:	e3a03001 	mov	r3, #1	; 0x1
     b50:	e2002007 	and	r2, r0, #7	; 0x7
     b54:	e0113213 	ands	r3, r1, r3, lsl r2
     b58:	01a00003 	moveq	r0, r3
     b5c:	012fff1e 	bxeq	lr
     b60:	e1a03300 	lsl	r3, r0, #6
     b64:	e2833282 	add	r3, r3, #536870920	; 0x20000008
     b68:	e283360f 	add	r3, r3, #15728640	; 0xf00000
     b6c:	e5933000 	ldr	r3, [r3]
     b70:	e3530000 	cmp	r3, #0	; 0x0
     b74:	13a00002 	movne	r0, #2	; 0x2
     b78:	03a00001 	moveq	r0, #1	; 0x1
     b7c:	e12fff1e 	bx	lr
     b80:	2d009004 	.word	0x2d009004

00000b84 <pads_chmod>:
     b84:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     b88:	e1b07840 	asrs	r7, r0, #16
     b8c:	e1a00800 	lsl	r0, r0, #16
     b90:	03a03000 	moveq	r3, #0	; 0x0
     b94:	13a03001 	movne	r3, #1	; 0x1
     b98:	e1a00820 	lsr	r0, r0, #16
     b9c:	e1570000 	cmp	r7, r0
     ba0:	a3a0c000 	movge	ip, #0	; 0x0
     ba4:	b203c001 	andlt	ip, r3, #1	; 0x1
     ba8:	e35c0000 	cmp	ip, #0	; 0x0
     bac:	e24dd004 	sub	sp, sp, #4	; 0x4
     bb0:	e1a05001 	mov	r5, r1
     bb4:	e1a09002 	mov	r9, r2
     bb8:	1a000038 	bne	ca0 <pads_chmod+0x11c>
     bbc:	e2803883 	add	r3, r0, #8585216	; 0x830000
     bc0:	e2833903 	add	r3, r3, #49152	; 0xc000
     bc4:	e1a02303 	lsl	r2, r3, #6
     bc8:	e3570000 	cmp	r7, #0	; 0x0
     bcc:	01a07000 	moveq	r7, r0
     bd0:	e2826008 	add	r6, r2, #8	; 0x8
     bd4:	e2828004 	add	r8, r2, #4	; 0x4
     bd8:	e1a04000 	mov	r4, r0
     bdc:	e3a0a001 	mov	sl, #1	; 0x1
     be0:	e59fb0c0 	ldr	fp, [pc, #192]	; ca8 <pads_chmod+0x124>
     be4:	ea00002a 	b	c94 <pads_chmod+0x110>
     be8:	e3540000 	cmp	r4, #0	; 0x0
     bec:	a1a03004 	movge	r3, r4
     bf0:	e3550001 	cmp	r5, #1	; 0x1
     bf4:	e1a011c3 	asr	r1, r3, #3
     bf8:	e204c007 	and	ip, r4, #7	; 0x7
     bfc:	0a00000b 	beq	c30 <pads_chmod+0xac>
     c00:	e3550002 	cmp	r5, #2	; 0x2
     c04:	0a000012 	beq	c54 <pads_chmod+0xd0>
     c08:	e59f209c 	ldr	r2, [pc, #156]	; cac <pads_chmod+0x128>
     c0c:	e1a03101 	lsl	r3, r1, #2
     c10:	e2551000 	subs	r1, r5, #0	; 0x0
     c14:	e0832002 	add	r2, r3, r2
     c18:	e59f0090 	ldr	r0, [pc, #144]	; cb0 <pads_chmod+0x12c>
     c1c:	1a000017 	bne	c80 <pads_chmod+0xfc>
     c20:	e5923000 	ldr	r3, [r2]
     c24:	e1c33c1a 	bic	r3, r3, sl, lsl ip
     c28:	e5823000 	str	r3, [r2]
     c2c:	ea000015 	b	c88 <pads_chmod+0x104>
     c30:	e59f3074 	ldr	r3, [pc, #116]	; cac <pads_chmod+0x128>
     c34:	e1a02101 	lsl	r2, r1, #2
     c38:	e0823003 	add	r3, r2, r3
     c3c:	e5932000 	ldr	r2, [r3]
     c40:	e1822c15 	orr	r2, r2, r5, lsl ip
     c44:	e5832000 	str	r2, [r3]
     c48:	e3a03000 	mov	r3, #0	; 0x0
     c4c:	e5863000 	str	r3, [r6]
     c50:	ea00000c 	b	c88 <pads_chmod+0x104>
     c54:	e59f2050 	ldr	r2, [pc, #80]	; cac <pads_chmod+0x128>
     c58:	e1a03101 	lsl	r3, r1, #2
     c5c:	e0832002 	add	r2, r3, r2
     c60:	e5923000 	ldr	r3, [r2]
     c64:	e2591000 	subs	r1, r9, #0	; 0x0
     c68:	13a01001 	movne	r1, #1	; 0x1
     c6c:	e1833c1a 	orr	r3, r3, sl, lsl ip
     c70:	e5823000 	str	r3, [r2]
     c74:	e586a000 	str	sl, [r6]
     c78:	e5881000 	str	r1, [r8]
     c7c:	ea000001 	b	c88 <pads_chmod+0x104>
     c80:	e1a0e00f 	mov	lr, pc
     c84:	e59bf000 	ldr	pc, [fp]
     c88:	e2844001 	add	r4, r4, #1	; 0x1
     c8c:	e2866040 	add	r6, r6, #64	; 0x40
     c90:	e2888040 	add	r8, r8, #64	; 0x40
     c94:	e1540007 	cmp	r4, r7
     c98:	e2843007 	add	r3, r4, #7	; 0x7
     c9c:	daffffd1 	ble	be8 <pads_chmod+0x64>
     ca0:	e28dd004 	add	sp, sp, #4	; 0x4
     ca4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     ca8:	0800d76c 	.word	0x0800d76c
     cac:	2d009004 	.word	0x2d009004
     cb0:	00011b47 	.word	0x00011b47

00000cb4 <pads_pull>:
     cb4:	e1b02840 	asrs	r2, r0, #16
     cb8:	e1a00800 	lsl	r0, r0, #16
     cbc:	03a03000 	moveq	r3, #0	; 0x0
     cc0:	13a03001 	movne	r3, #1	; 0x1
     cc4:	e1a00820 	lsr	r0, r0, #16
     cc8:	e1520000 	cmp	r2, r0
     ccc:	a3a03000 	movge	r3, #0	; 0x0
     cd0:	b2033001 	andlt	r3, r3, #1	; 0x1
     cd4:	e3530000 	cmp	r3, #0	; 0x0
     cd8:	112fff1e 	bxne	lr
     cdc:	e1a03300 	lsl	r3, r0, #6
     ce0:	e28332c2 	add	r3, r3, #536870924	; 0x2000000c
     ce4:	e3520000 	cmp	r2, #0	; 0x0
     ce8:	01a02000 	moveq	r2, r0
     cec:	e283360f 	add	r3, r3, #15728640	; 0xf00000
     cf0:	e3a0c001 	mov	ip, #1	; 0x1
     cf4:	ea000004 	b	d0c <pads_pull+0x58>
     cf8:	e3510000 	cmp	r1, #0	; 0x0
     cfc:	05831000 	streq	r1, [r3]
     d00:	1583c000 	strne	ip, [r3]
     d04:	e2800001 	add	r0, r0, #1	; 0x1
     d08:	e2833040 	add	r3, r3, #64	; 0x40
     d0c:	e1500002 	cmp	r0, r2
     d10:	dafffff8 	ble	cf8 <pads_pull+0x44>
     d14:	e12fff1e 	bx	lr

00000d18 <pads_setpin>:
     d18:	e1b0c840 	asrs	ip, r0, #16
     d1c:	e1a00800 	lsl	r0, r0, #16
     d20:	03a03000 	moveq	r3, #0	; 0x0
     d24:	13a03001 	movne	r3, #1	; 0x1
     d28:	e1a00820 	lsr	r0, r0, #16
     d2c:	e15c0000 	cmp	ip, r0
     d30:	a3a03000 	movge	r3, #0	; 0x0
     d34:	b2033001 	andlt	r3, r3, #1	; 0x1
     d38:	e3530000 	cmp	r3, #0	; 0x0
     d3c:	112fff1e 	bxne	lr
     d40:	e1a03300 	lsl	r3, r0, #6
     d44:	e2832242 	add	r2, r3, #536870916	; 0x20000004
     d48:	e35c0000 	cmp	ip, #0	; 0x0
     d4c:	01a0c000 	moveq	ip, r0
     d50:	e282260f 	add	r2, r2, #15728640	; 0xf00000
     d54:	ea000002 	b	d64 <pads_setpin+0x4c>
     d58:	e2513000 	subs	r3, r1, #0	; 0x0
     d5c:	13a03001 	movne	r3, #1	; 0x1
     d60:	e4823040 	str	r3, [r2], #64
     d64:	e150000c 	cmp	r0, ip
     d68:	e2800001 	add	r0, r0, #1	; 0x1
     d6c:	dafffff9 	ble	d58 <pads_setpin+0x40>
     d70:	e12fff1e 	bx	lr

00000d74 <pads_getpin>:
     d74:	e59f3014 	ldr	r3, [pc, #20]	; d90 <pads_getpin+0x1c>
     d78:	e6f30070 	uxtah	r0, r3, r0
     d7c:	e1a00300 	lsl	r0, r0, #6
     d80:	e5900000 	ldr	r0, [r0]
     d84:	e2500000 	subs	r0, r0, #0	; 0x0
     d88:	13a00001 	movne	r0, #1	; 0x1
     d8c:	e12fff1e 	bx	lr
     d90:	0083c000 	.word	0x0083c000

00000d94 <init_alternate>:
     d94:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     d98:	e59f0010 	ldr	r0, [pc, #16]	; db0 <init_alternate+0x1c>
     d9c:	e24dd004 	sub	sp, sp, #4	; 0x4
     da0:	ebfffe5e 	bl	720 <module_enable>
     da4:	e3a00000 	mov	r0, #0	; 0x0
     da8:	e28dd004 	add	sp, sp, #4	; 0x4
     dac:	e8bd8000 	pop	{pc}
     db0:	2d013c00 	.word	0x2d013c00

00000db4 <cpu_recase>:
     db4:	e92d4010 	push	{r4, lr}
     db8:	e59f1098 	ldr	r1, [pc, #152]	; e58 <cpu_recase+0xa4>
     dbc:	e3a02004 	mov	r2, #4	; 0x4
     dc0:	e1a04000 	mov	r4, r0
     dc4:	eb000127 	bl	1268 <strncmp>
     dc8:	e3500000 	cmp	r0, #0	; 0x0
     dcc:	1a000002 	bne	ddc <cpu_recase+0x28>
     dd0:	eb0000ba 	bl	10c0 <get_mux>
     dd4:	e3c00004 	bic	r0, r0, #4	; 0x4
     dd8:	ea00000d 	b	e14 <cpu_recase+0x60>
     ddc:	e1a00004 	mov	r0, r4
     de0:	e59f1074 	ldr	r1, [pc, #116]	; e5c <cpu_recase+0xa8>
     de4:	e3a02005 	mov	r2, #5	; 0x5
     de8:	eb00011e 	bl	1268 <strncmp>
     dec:	e3500000 	cmp	r0, #0	; 0x0
     df0:	0a000005 	beq	e0c <cpu_recase+0x58>
     df4:	e1a00004 	mov	r0, r4
     df8:	e59f1060 	ldr	r1, [pc, #96]	; e60 <cpu_recase+0xac>
     dfc:	e3a02004 	mov	r2, #4	; 0x4
     e00:	eb000118 	bl	1268 <strncmp>
     e04:	e3500000 	cmp	r0, #0	; 0x0
     e08:	18bd8010 	popne	{r4, pc}
     e0c:	eb0000ab 	bl	10c0 <get_mux>
     e10:	e3800004 	orr	r0, r0, #4	; 0x4
     e14:	e59f3048 	ldr	r3, [pc, #72]	; e64 <cpu_recase+0xb0>
     e18:	e59f4044 	ldr	r4, [pc, #68]	; e64 <cpu_recase+0xb0>
     e1c:	e59f1044 	ldr	r1, [pc, #68]	; e68 <cpu_recase+0xb4>
     e20:	e5830000 	str	r0, [r3]
     e24:	e5942000 	ldr	r2, [r4]
     e28:	e5913000 	ldr	r3, [r1]
     e2c:	e1520003 	cmp	r2, r3
     e30:	08bd8010 	popeq	{r4, pc}
     e34:	e1a00002 	mov	r0, r2
     e38:	e5812000 	str	r2, [r1]
     e3c:	eb00009b 	bl	10b0 <set_mux>
     e40:	e5941000 	ldr	r1, [r4]
     e44:	e59f0020 	ldr	r0, [pc, #32]	; e6c <cpu_recase+0xb8>
     e48:	e59f3020 	ldr	r3, [pc, #32]	; e70 <cpu_recase+0xbc>
     e4c:	e1a0e00f 	mov	lr, pc
     e50:	e593f000 	ldr	pc, [r3]
     e54:	e8bd8010 	pop	{r4, pc}
     e58:	00011b5e 	.word	0x00011b5e
     e5c:	00011b63 	.word	0x00011b63
     e60:	000119ee 	.word	0x000119ee
     e64:	0800d1b0 	.word	0x0800d1b0
     e68:	0800d104 	.word	0x0800d104
     e6c:	00011b69 	.word	0x00011b69
     e70:	0800d76c 	.word	0x0800d76c

00000e74 <set_clk>:
     e74:	e92d4030 	push	{r4, r5, lr}
     e78:	e1d010b2 	ldrh	r1, [r0, #2]
     e7c:	e24dd004 	sub	sp, sp, #4	; 0x4
     e80:	e3510000 	cmp	r1, #0	; 0x0
     e84:	e1a04000 	mov	r4, r0
     e88:	0a000001 	beq	e94 <set_clk+0x20>
     e8c:	e3a00001 	mov	r0, #1	; 0x1
     e90:	ebfffdda 	bl	600 <set_pll>
     e94:	e1d410b4 	ldrh	r1, [r4, #4]
     e98:	e3510000 	cmp	r1, #0	; 0x0
     e9c:	0a000001 	beq	ea8 <set_clk+0x34>
     ea0:	e3a00002 	mov	r0, #2	; 0x2
     ea4:	ebfffdd5 	bl	600 <set_pll>
     ea8:	e1d410b6 	ldrh	r1, [r4, #6]
     eac:	e3510000 	cmp	r1, #0	; 0x0
     eb0:	0a000001 	beq	ebc <set_clk+0x48>
     eb4:	e3a00003 	mov	r0, #3	; 0x3
     eb8:	ebfffdd0 	bl	600 <set_pll>
     ebc:	e59f0094 	ldr	r0, [pc, #148]	; f58 <set_clk+0xe4>
     ec0:	e3a01001 	mov	r1, #1	; 0x1
     ec4:	e3a0200b 	mov	r2, #11	; 0xb
     ec8:	ebfffe09 	bl	6f4 <module_set_clock>
     ecc:	e59f0088 	ldr	r0, [pc, #136]	; f5c <set_clk+0xe8>
     ed0:	e3a01001 	mov	r1, #1	; 0x1
     ed4:	e3a02013 	mov	r2, #19	; 0x13
     ed8:	ebfffe05 	bl	6f4 <module_set_clock>
     edc:	e59f007c 	ldr	r0, [pc, #124]	; f60 <set_clk+0xec>
     ee0:	e3a01002 	mov	r1, #2	; 0x2
     ee4:	e3a0200f 	mov	r2, #15	; 0xf
     ee8:	ebfffe01 	bl	6f4 <module_set_clock>
     eec:	e59f0070 	ldr	r0, [pc, #112]	; f64 <set_clk+0xf0>
     ef0:	e3a01002 	mov	r1, #2	; 0x2
     ef4:	e3a02019 	mov	r2, #25	; 0x19
     ef8:	ebfffdfd 	bl	6f4 <module_set_clock>
     efc:	e59f4064 	ldr	r4, [pc, #100]	; f68 <set_clk+0xf4>
     f00:	e59f5064 	ldr	r5, [pc, #100]	; f6c <set_clk+0xf8>
     f04:	e1a00004 	mov	r0, r4
     f08:	e3a01002 	mov	r1, #2	; 0x2
     f0c:	e2844010 	add	r4, r4, #16	; 0x10
     f10:	e3a02003 	mov	r2, #3	; 0x3
     f14:	ebfffdf6 	bl	6f4 <module_set_clock>
     f18:	e1540005 	cmp	r4, r5
     f1c:	1afffff8 	bne	f04 <set_clk+0x90>
     f20:	e59f0048 	ldr	r0, [pc, #72]	; f70 <set_clk+0xfc>
     f24:	e3a01003 	mov	r1, #3	; 0x3
     f28:	e3a02013 	mov	r2, #19	; 0x13
     f2c:	ebfffdf0 	bl	6f4 <module_set_clock>
     f30:	e59f003c 	ldr	r0, [pc, #60]	; f74 <set_clk+0x100>
     f34:	e3a01001 	mov	r1, #1	; 0x1
     f38:	e3a02004 	mov	r2, #4	; 0x4
     f3c:	ebfffdec 	bl	6f4 <module_set_clock>
     f40:	e59f0030 	ldr	r0, [pc, #48]	; f78 <set_clk+0x104>
     f44:	e3a01002 	mov	r1, #2	; 0x2
     f48:	e3a02007 	mov	r2, #7	; 0x7
     f4c:	e28dd004 	add	sp, sp, #4	; 0x4
     f50:	e8bd4030 	pop	{r4, r5, lr}
     f54:	eafffde6 	b	6f4 <module_set_clock>
     f58:	2d00a250 	.word	0x2d00a250
     f5c:	2d00a2c0 	.word	0x2d00a2c0
     f60:	2d00a240 	.word	0x2d00a240
     f64:	2d00a290 	.word	0x2d00a290
     f68:	2d00a130 	.word	0x2d00a130
     f6c:	2d00a190 	.word	0x2d00a190
     f70:	2d00a260 	.word	0x2d00a260
     f74:	2d00a2a0 	.word	0x2d00a2a0
     f78:	2d00a360 	.word	0x2d00a360

00000f7c <chip_test>:
     f7c:	e59f0044 	ldr	r0, [pc, #68]	; fc8 <end+0x4>
     f80:	e5901000 	ldr	r1, [r0]
     f84:	e59f2040 	ldr	r2, [pc, #64]	; fcc <end+0x8>
     f88:	e1520001 	cmp	r2, r1
     f8c:	1a00000c 	bne	fc4 <end>
     f90:	e5901004 	ldr	r1, [r0, #4]
     f94:	e59f2034 	ldr	r2, [pc, #52]	; fd0 <end+0xc>
     f98:	e1520001 	cmp	r2, r1
     f9c:	1a000008 	bne	fc4 <end>
     fa0:	e5901008 	ldr	r1, [r0, #8]
     fa4:	e59f2028 	ldr	r2, [pc, #40]	; fd4 <end+0x10>
     fa8:	e1520001 	cmp	r2, r1
     fac:	1a000004 	bne	fc4 <end>
     fb0:	e590100c 	ldr	r1, [r0, #12]
     fb4:	e59f201c 	ldr	r2, [pc, #28]	; fd8 <end+0x14>
     fb8:	e1520001 	cmp	r2, r1
     fbc:	1a000000 	bne	fc4 <end>
     fc0:	e59ff014 	ldr	pc, [pc, #20]	; fdc <end+0x18>

00000fc4 <end>:
     fc4:	e1a0f00e 	mov	pc, lr
     fc8:	08000040 	.word	0x08000040
     fcc:	6f666e69 	.word	0x6f666e69
     fd0:	612d6d74 	.word	0x612d6d74
     fd4:	2d6e696c 	.word	0x2d6e696c
     fd8:	796c6e6f 	.word	0x796c6e6f
     fdc:	08000200 	.word	0x08000200

00000fe0 <module_get_clock>:
     fe0:	e59f30a4 	ldr	r3, [pc, #164]	; 108c <module_get_clock+0xac>
     fe4:	e1500003 	cmp	r0, r3
     fe8:	0a00001d 	beq	1064 <module_get_clock+0x84>
     fec:	8a00000c 	bhi	1024 <module_get_clock+0x44>
     ff0:	e3500003 	cmp	r0, #3	; 0x3
     ff4:	0a000022 	beq	1084 <module_get_clock+0xa4>
     ff8:	8a000002 	bhi	1008 <module_get_clock+0x28>
     ffc:	e3500001 	cmp	r0, #1	; 0x1
    1000:	1a00001d 	bne	107c <module_get_clock+0x9c>
    1004:	ea00001e 	b	1084 <module_get_clock+0xa4>
    1008:	e59f3080 	ldr	r3, [pc, #128]	; 1090 <module_get_clock+0xb0>
    100c:	e1500003 	cmp	r0, r3
    1010:	0a000013 	beq	1064 <module_get_clock+0x84>
    1014:	e2833010 	add	r3, r3, #16	; 0x10
    1018:	e1500003 	cmp	r0, r3
    101c:	1a000016 	bne	107c <module_get_clock+0x9c>
    1020:	ea00000f 	b	1064 <module_get_clock+0x84>
    1024:	e59f3068 	ldr	r3, [pc, #104]	; 1094 <module_get_clock+0xb4>
    1028:	e1500003 	cmp	r0, r3
    102c:	0a00000c 	beq	1064 <module_get_clock+0x84>
    1030:	8a000003 	bhi	1044 <module_get_clock+0x64>
    1034:	e2433010 	sub	r3, r3, #16	; 0x10
    1038:	e1500003 	cmp	r0, r3
    103c:	1a00000e 	bne	107c <module_get_clock+0x9c>
    1040:	ea000009 	b	106c <module_get_clock+0x8c>
    1044:	e59f304c 	ldr	r3, [pc, #76]	; 1098 <module_get_clock+0xb8>
    1048:	e1500003 	cmp	r0, r3
    104c:	059f0048 	ldreq	r0, [pc, #72]	; 109c <module_get_clock+0xbc>
    1050:	012fff1e 	bxeq	lr
    1054:	e28330a0 	add	r3, r3, #160	; 0xa0
    1058:	e1500003 	cmp	r0, r3
    105c:	1a000006 	bne	107c <module_get_clock+0x9c>
    1060:	ea000003 	b	1074 <module_get_clock+0x94>
    1064:	e59f0034 	ldr	r0, [pc, #52]	; 10a0 <module_get_clock+0xc0>
    1068:	e12fff1e 	bx	lr
    106c:	e59f0030 	ldr	r0, [pc, #48]	; 10a4 <module_get_clock+0xc4>
    1070:	e12fff1e 	bx	lr
    1074:	e59f002c 	ldr	r0, [pc, #44]	; 10a8 <module_get_clock+0xc8>
    1078:	e12fff1e 	bx	lr
    107c:	e3a00000 	mov	r0, #0	; 0x0
    1080:	e12fff1e 	bx	lr
    1084:	e59f0020 	ldr	r0, [pc, #32]	; 10ac <module_get_clock+0xcc>
    1088:	e12fff1e 	bx	lr
    108c:	2d00a260 	.word	0x2d00a260
    1090:	2d00a240 	.word	0x2d00a240
    1094:	2d00a2a0 	.word	0x2d00a2a0
    1098:	2d00a2c0 	.word	0x2d00a2c0
    109c:	02dc6c00 	.word	0x02dc6c00
    10a0:	016e3600 	.word	0x016e3600
    10a4:	02625a00 	.word	0x02625a00
    10a8:	00e4e1c0 	.word	0x00e4e1c0
    10ac:	01312d00 	.word	0x01312d00

000010b0 <set_mux>:
    10b0:	e59f3004 	ldr	r3, [pc, #4]	; 10bc <set_mux+0xc>
    10b4:	e5830000 	str	r0, [r3]
    10b8:	e12fff1e 	bx	lr
    10bc:	2d009000 	.word	0x2d009000

000010c0 <get_mux>:
    10c0:	e59f3004 	ldr	r3, [pc, #4]	; 10cc <get_mux+0xc>
    10c4:	e5930000 	ldr	r0, [r3]
    10c8:	e12fff1e 	bx	lr
    10cc:	2d009000 	.word	0x2d009000

000010d0 <efuse_inject>:
    10d0:	e92d4010 	push	{r4, lr}
    10d4:	e24dd030 	sub	sp, sp, #48	; 0x30
    10d8:	e59f109c 	ldr	r1, [pc, #156]	; 117c <efuse_inject+0xac>
    10dc:	e3a02020 	mov	r2, #32	; 0x20
    10e0:	e1a0000d 	mov	r0, sp
    10e4:	eb000098 	bl	134c <memcpy>
    10e8:	e3a01000 	mov	r1, #0	; 0x0
    10ec:	e3a02010 	mov	r2, #16	; 0x10
    10f0:	e28d0020 	add	r0, sp, #32	; 0x20
    10f4:	eb00008c 	bl	132c <memset>
    10f8:	e3a0302e 	mov	r3, #46	; 0x2e
    10fc:	e5cd3020 	strb	r3, [sp, #32]
    1100:	e59f1078 	ldr	r1, [pc, #120]	; 1180 <efuse_inject+0xb0>
    1104:	e3a03016 	mov	r3, #22	; 0x16
    1108:	e5cd3021 	strb	r3, [sp, #33]
    110c:	e1a0400d 	mov	r4, sp
    1110:	e3a02000 	mov	r2, #0	; 0x0
    1114:	e7d43002 	ldrb	r3, [r4, r2]
    1118:	e2822001 	add	r2, r2, #1	; 0x1
    111c:	e3520020 	cmp	r2, #32	; 0x20
    1120:	e4813004 	str	r3, [r1], #4
    1124:	1afffffa 	bne	1114 <efuse_inject+0x44>
    1128:	e59f1054 	ldr	r1, [pc, #84]	; 1184 <efuse_inject+0xb4>
    112c:	e3a02000 	mov	r2, #0	; 0x0
    1130:	e28d0020 	add	r0, sp, #32	; 0x20
    1134:	e7d03002 	ldrb	r3, [r0, r2]
    1138:	e2822001 	add	r2, r2, #1	; 0x1
    113c:	e3520010 	cmp	r2, #16	; 0x10
    1140:	e4813004 	str	r3, [r1], #4
    1144:	1afffffa 	bne	1134 <efuse_inject+0x64>
    1148:	e59f3030 	ldr	r3, [pc, #48]	; 1180 <efuse_inject+0xb0>
    114c:	e3a02018 	mov	r2, #24	; 0x18
    1150:	e5832100 	str	r2, [r3, #256]
    1154:	e3a02001 	mov	r2, #1	; 0x1
    1158:	e2433a01 	sub	r3, r3, #4096	; 0x1000
    115c:	e5832000 	str	r2, [r3]
    1160:	e3a04000 	mov	r4, #0	; 0x0
    1164:	e2433a03 	sub	r3, r3, #12288	; 0x3000
    1168:	e5834c34 	str	r4, [r3, #3124]
    116c:	ebfffe67 	bl	b10 <init_efuse>
    1170:	e1a00004 	mov	r0, r4
    1174:	e28dd030 	add	sp, sp, #48	; 0x30
    1178:	e8bd8010 	pop	{r4, pc}
    117c:	000117ec 	.word	0x000117ec
    1180:	2d00e000 	.word	0x2d00e000
    1184:	2d00e080 	.word	0x2d00e080

00001188 <strcpy>:
    1188:	e3a02000 	mov	r2, #0	; 0x0
    118c:	e7d13002 	ldrb	r3, [r1, r2]
    1190:	e3530000 	cmp	r3, #0	; 0x0
    1194:	e7c03002 	strb	r3, [r0, r2]
    1198:	e2822001 	add	r2, r2, #1	; 0x1
    119c:	1afffffa 	bne	118c <strcpy+0x4>
    11a0:	e12fff1e 	bx	lr

000011a4 <strncpy>:
    11a4:	e3a0c000 	mov	ip, #0	; 0x0
    11a8:	e3520000 	cmp	r2, #0	; 0x0
    11ac:	e2422001 	sub	r2, r2, #1	; 0x1
    11b0:	012fff1e 	bxeq	lr
    11b4:	e7d1300c 	ldrb	r3, [r1, ip]
    11b8:	e3530000 	cmp	r3, #0	; 0x0
    11bc:	e7c0300c 	strb	r3, [r0, ip]
    11c0:	e28cc001 	add	ip, ip, #1	; 0x1
    11c4:	1afffff7 	bne	11a8 <strncpy+0x4>
    11c8:	e12fff1e 	bx	lr

000011cc <strncat>:
    11cc:	e92d4030 	push	{r4, r5, lr}
    11d0:	e2525000 	subs	r5, r2, #0	; 0x0
    11d4:	e1a04001 	mov	r4, r1
    11d8:	11a0c000 	movne	ip, r0
    11dc:	1a000001 	bne	11e8 <strncat+0x1c>
    11e0:	e8bd8030 	pop	{r4, r5, pc}
    11e4:	e28cc001 	add	ip, ip, #1	; 0x1
    11e8:	e5dc3000 	ldrb	r3, [ip]
    11ec:	e3530000 	cmp	r3, #0	; 0x0
    11f0:	1afffffb 	bne	11e4 <strncat+0x18>
    11f4:	ea000004 	b	120c <strncat+0x40>
    11f8:	e1550002 	cmp	r5, r2
    11fc:	1a000004 	bne	1214 <strncat+0x48>
    1200:	e3a03000 	mov	r3, #0	; 0x0
    1204:	e5cc3000 	strb	r3, [ip]
    1208:	e8bd8030 	pop	{r4, r5, pc}
    120c:	e1a02003 	mov	r2, r3
    1210:	e1a0100c 	mov	r1, ip
    1214:	e7d43002 	ldrb	r3, [r4, r2]
    1218:	e28cc001 	add	ip, ip, #1	; 0x1
    121c:	e3530000 	cmp	r3, #0	; 0x0
    1220:	e7c13002 	strb	r3, [r1, r2]
    1224:	e2822001 	add	r2, r2, #1	; 0x1
    1228:	1afffff2 	bne	11f8 <strncat+0x2c>
    122c:	e8bd8030 	pop	{r4, r5, pc}

00001230 <strcmp>:
    1230:	e92d4010 	push	{r4, lr}
    1234:	e3a02000 	mov	r2, #0	; 0x0
    1238:	e1a04000 	mov	r4, r0
    123c:	e7d4c002 	ldrb	ip, [r4, r2]
    1240:	e7d13002 	ldrb	r3, [r1, r2]
    1244:	e2822001 	add	r2, r2, #1	; 0x1
    1248:	e063300c 	rsb	r3, r3, ip
    124c:	e6ef0073 	uxtb	r0, r3
    1250:	e3500000 	cmp	r0, #0	; 0x0
    1254:	1a000001 	bne	1260 <strcmp+0x30>
    1258:	e35c0000 	cmp	ip, #0	; 0x0
    125c:	1afffff6 	bne	123c <strcmp+0xc>
    1260:	e6af0070 	sxtb	r0, r0
    1264:	e8bd8010 	pop	{r4, pc}

00001268 <strncmp>:
    1268:	e92d4030 	push	{r4, r5, lr}
    126c:	e3a0c000 	mov	ip, #0	; 0x0
    1270:	e1a05000 	mov	r5, r0
    1274:	e1a04001 	mov	r4, r1
    1278:	ea000009 	b	12a4 <strncmp+0x3c>
    127c:	e7d5100c 	ldrb	r1, [r5, ip]
    1280:	e7d4300c 	ldrb	r3, [r4, ip]
    1284:	e2422001 	sub	r2, r2, #1	; 0x1
    1288:	e0633001 	rsb	r3, r3, r1
    128c:	e6ef0073 	uxtb	r0, r3
    1290:	e3500000 	cmp	r0, #0	; 0x0
    1294:	e28cc001 	add	ip, ip, #1	; 0x1
    1298:	1a000004 	bne	12b0 <strncmp+0x48>
    129c:	e3510000 	cmp	r1, #0	; 0x0
    12a0:	0a000002 	beq	12b0 <strncmp+0x48>
    12a4:	e3520000 	cmp	r2, #0	; 0x0
    12a8:	1afffff3 	bne	127c <strncmp+0x14>
    12ac:	e1a00002 	mov	r0, r2
    12b0:	e6af0070 	sxtb	r0, r0
    12b4:	e8bd8030 	pop	{r4, r5, pc}

000012b8 <strchr>:
    12b8:	e1a03000 	mov	r3, r0
    12bc:	e6ef1071 	uxtb	r1, r1
    12c0:	ea000002 	b	12d0 <strchr+0x18>
    12c4:	e3500000 	cmp	r0, #0	; 0x0
    12c8:	012fff1e 	bxeq	lr
    12cc:	e2833001 	add	r3, r3, #1	; 0x1
    12d0:	e5d30000 	ldrb	r0, [r3]
    12d4:	e1500001 	cmp	r0, r1
    12d8:	1afffff9 	bne	12c4 <strchr+0xc>
    12dc:	e1a00003 	mov	r0, r3
    12e0:	e12fff1e 	bx	lr

000012e4 <strlen>:
    12e4:	e1a02000 	mov	r2, r0
    12e8:	ea000000 	b	12f0 <strlen+0xc>
    12ec:	e2800001 	add	r0, r0, #1	; 0x1
    12f0:	e5d03000 	ldrb	r3, [r0]
    12f4:	e3530000 	cmp	r3, #0	; 0x0
    12f8:	1afffffb 	bne	12ec <strlen+0x8>
    12fc:	e0620000 	rsb	r0, r2, r0
    1300:	e12fff1e 	bx	lr

00001304 <strnlen>:
    1304:	e3a02000 	mov	r2, #0	; 0x0
    1308:	e1510002 	cmp	r1, r2
    130c:	e080c002 	add	ip, r0, r2
    1310:	0a000003 	beq	1324 <strnlen+0x20>
    1314:	e7d03002 	ldrb	r3, [r0, r2]
    1318:	e2822001 	add	r2, r2, #1	; 0x1
    131c:	e3530000 	cmp	r3, #0	; 0x0
    1320:	1afffff8 	bne	1308 <strnlen+0x4>
    1324:	e060000c 	rsb	r0, r0, ip
    1328:	e12fff1e 	bx	lr

0000132c <memset>:
    132c:	e3a03000 	mov	r3, #0	; 0x0
    1330:	ea000001 	b	133c <memset+0x10>
    1334:	e7c01003 	strb	r1, [r0, r3]
    1338:	e2833001 	add	r3, r3, #1	; 0x1
    133c:	e3520000 	cmp	r2, #0	; 0x0
    1340:	e2422001 	sub	r2, r2, #1	; 0x1
    1344:	1afffffa 	bne	1334 <memset+0x8>
    1348:	e12fff1e 	bx	lr

0000134c <memcpy>:
    134c:	e3a0c000 	mov	ip, #0	; 0x0
    1350:	ea000002 	b	1360 <memcpy+0x14>
    1354:	e7d1300c 	ldrb	r3, [r1, ip]
    1358:	e7c0300c 	strb	r3, [r0, ip]
    135c:	e28cc001 	add	ip, ip, #1	; 0x1
    1360:	e3520000 	cmp	r2, #0	; 0x0
    1364:	e2422001 	sub	r2, r2, #1	; 0x1
    1368:	1afffff9 	bne	1354 <memcpy+0x8>
    136c:	e12fff1e 	bx	lr

00001370 <memmove>:
    1370:	e1500001 	cmp	r0, r1
    1374:	e92d4010 	push	{r4, lr}
    1378:	08bd8010 	popeq	{r4, pc}
    137c:	93a0c000 	movls	ip, #0	; 0x0
    1380:	9a000003 	bls	1394 <memmove+0x24>
    1384:	ea000006 	b	13a4 <memmove+0x34>
    1388:	e7d1300c 	ldrb	r3, [r1, ip]
    138c:	e7c0300c 	strb	r3, [r0, ip]
    1390:	e28cc001 	add	ip, ip, #1	; 0x1
    1394:	e3520000 	cmp	r2, #0	; 0x0
    1398:	e2422001 	sub	r2, r2, #1	; 0x1
    139c:	1afffff9 	bne	1388 <memmove+0x18>
    13a0:	e8bd8010 	pop	{r4, pc}
    13a4:	e0814002 	add	r4, r1, r2
    13a8:	e3a0c000 	mov	ip, #0	; 0x0
    13ac:	e0801002 	add	r1, r0, r2
    13b0:	ea000001 	b	13bc <memmove+0x4c>
    13b4:	e7d4300c 	ldrb	r3, [r4, ip]
    13b8:	e7c1300c 	strb	r3, [r1, ip]
    13bc:	e3520000 	cmp	r2, #0	; 0x0
    13c0:	e24cc001 	sub	ip, ip, #1	; 0x1
    13c4:	e2422001 	sub	r2, r2, #1	; 0x1
    13c8:	1afffff9 	bne	13b4 <memmove+0x44>
    13cc:	e8bd8010 	pop	{r4, pc}

000013d0 <memcmp>:
    13d0:	e92d4030 	push	{r4, r5, lr}
    13d4:	e3a0c000 	mov	ip, #0	; 0x0
    13d8:	e1a04001 	mov	r4, r1
    13dc:	e1a05000 	mov	r5, r0
    13e0:	e1a01002 	mov	r1, r2
    13e4:	ea000005 	b	1400 <memcmp+0x30>
    13e8:	e7d5200c 	ldrb	r2, [r5, ip]
    13ec:	e7d4300c 	ldrb	r3, [r4, ip]
    13f0:	e28cc001 	add	ip, ip, #1	; 0x1
    13f4:	e0520003 	subs	r0, r2, r3
    13f8:	18bd8030 	popne	{r4, r5, pc}
    13fc:	e2411001 	sub	r1, r1, #1	; 0x1
    1400:	e3510000 	cmp	r1, #0	; 0x0
    1404:	1afffff7 	bne	13e8 <memcmp+0x18>
    1408:	e1a00001 	mov	r0, r1
    140c:	e8bd8030 	pop	{r4, r5, pc}

00001410 <so_m_isused>:
    1410:	e5903004 	ldr	r3, [r0, #4]
    1414:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    1418:	03a00000 	moveq	r0, #0	; 0x0
    141c:	13a00001 	movne	r0, #1	; 0x1
    1420:	e12fff1e 	bx	lr

00001424 <so_m_set_used>:
    1424:	e5903004 	ldr	r3, [r0, #4]
    1428:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    142c:	e5803004 	str	r3, [r0, #4]
    1430:	e12fff1e 	bx	lr

00001434 <so_m_unset_used>:
    1434:	e5903004 	ldr	r3, [r0, #4]
    1438:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    143c:	e5803004 	str	r3, [r0, #4]
    1440:	e12fff1e 	bx	lr

00001444 <so_m_get_size>:
    1444:	e5900004 	ldr	r0, [r0, #4]
    1448:	e3c00103 	bic	r0, r0, #-1073741824	; 0xc0000000
    144c:	e12fff1e 	bx	lr

00001450 <so_m_set_size>:
    1450:	e5903004 	ldr	r3, [r0, #4]
    1454:	e3c11103 	bic	r1, r1, #-1073741824	; 0xc0000000
    1458:	e2033103 	and	r3, r3, #-1073741824	; 0xc0000000
    145c:	e1811003 	orr	r1, r1, r3
    1460:	e5801004 	str	r1, [r0, #4]
    1464:	e12fff1e 	bx	lr

00001468 <so_m_alloc>:
    1468:	e59f3048 	ldr	r3, [pc, #72]	; 14b8 <so_m_alloc+0x50>
    146c:	e3a02000 	mov	r2, #0	; 0x0
    1470:	e593c000 	ldr	ip, [r3]
    1474:	e5931008 	ldr	r1, [r3, #8]
    1478:	ea000007 	b	149c <so_m_alloc+0x34>
    147c:	e5903004 	ldr	r3, [r0, #4]
    1480:	e3530000 	cmp	r3, #0	; 0x0
    1484:	1a000004 	bne	149c <so_m_alloc+0x34>
    1488:	e59f2028 	ldr	r2, [pc, #40]	; 14b8 <so_m_alloc+0x50>
    148c:	e5923004 	ldr	r3, [r2, #4]
    1490:	e2433001 	sub	r3, r3, #1	; 0x1
    1494:	e5823004 	str	r3, [r2, #4]
    1498:	e12fff1e 	bx	lr
    149c:	e152000c 	cmp	r2, ip
    14a0:	e1a00001 	mov	r0, r1
    14a4:	e2822001 	add	r2, r2, #1	; 0x1
    14a8:	e2811010 	add	r1, r1, #16	; 0x10
    14ac:	3afffff2 	bcc	147c <so_m_alloc+0x14>
    14b0:	e3a00000 	mov	r0, #0	; 0x0
    14b4:	e12fff1e 	bx	lr
    14b8:	0800d7ac 	.word	0x0800d7ac

000014bc <so_m_remove>:
    14bc:	e3a03000 	mov	r3, #0	; 0x0
    14c0:	e59f2014 	ldr	r2, [pc, #20]	; 14dc <so_m_remove+0x20>
    14c4:	e5803000 	str	r3, [r0]
    14c8:	e5803004 	str	r3, [r0, #4]
    14cc:	e5923004 	ldr	r3, [r2, #4]
    14d0:	e2833001 	add	r3, r3, #1	; 0x1
    14d4:	e5823004 	str	r3, [r2, #4]
    14d8:	e12fff1e 	bx	lr
    14dc:	0800d7ac 	.word	0x0800d7ac

000014e0 <malloc>:
    14e0:	e3500000 	cmp	r0, #0	; 0x0
    14e4:	e92d4070 	push	{r4, r5, r6, lr}
    14e8:	0a000047 	beq	160c <malloc+0x12c>
    14ec:	e59f1120 	ldr	r1, [pc, #288]	; 1614 <malloc+0x134>
    14f0:	e280301f 	add	r3, r0, #31	; 0x1f
    14f4:	e5b12014 	ldr	r2, [r1, #20]!
    14f8:	e3c3401f 	bic	r4, r3, #31	; 0x1f
    14fc:	e2425008 	sub	r5, r2, #8	; 0x8
    1500:	ea000009 	b	152c <malloc+0x4c>
    1504:	e5953004 	ldr	r3, [r5, #4]
    1508:	e2130103 	ands	r0, r3, #-1073741824	; 0xc0000000
    150c:	e3c32103 	bic	r2, r3, #-1073741824	; 0xc0000000
    1510:	1a000003 	bne	1524 <malloc+0x44>
    1514:	e1520004 	cmp	r2, r4
    1518:	3a000001 	bcc	1524 <malloc+0x44>
    151c:	9a000030 	bls	15e4 <malloc+0x104>
    1520:	ea000005 	b	153c <malloc+0x5c>
    1524:	e5953008 	ldr	r3, [r5, #8]
    1528:	e2435008 	sub	r5, r3, #8	; 0x8
    152c:	e2853008 	add	r3, r5, #8	; 0x8
    1530:	e1530001 	cmp	r3, r1
    1534:	1afffff2 	bne	1504 <malloc+0x24>
    1538:	ea000033 	b	160c <malloc+0x12c>
    153c:	e59f30d0 	ldr	r3, [pc, #208]	; 1614 <malloc+0x134>
    1540:	e1a01000 	mov	r1, r0
    1544:	e5932008 	ldr	r2, [r3, #8]
    1548:	e5930000 	ldr	r0, [r3]
    154c:	ea00000a 	b	157c <malloc+0x9c>
    1550:	e5923004 	ldr	r3, [r2, #4]
    1554:	e2822010 	add	r2, r2, #16	; 0x10
    1558:	e3530000 	cmp	r3, #0	; 0x0
    155c:	1a000006 	bne	157c <malloc+0x9c>
    1560:	e59f20ac 	ldr	r2, [pc, #172]	; 1614 <malloc+0x134>
    1564:	e3560000 	cmp	r6, #0	; 0x0
    1568:	e5923004 	ldr	r3, [r2, #4]
    156c:	e2433001 	sub	r3, r3, #1	; 0x1
    1570:	e5823004 	str	r3, [r2, #4]
    1574:	1a000005 	bne	1590 <malloc+0xb0>
    1578:	ea000023 	b	160c <malloc+0x12c>
    157c:	e1510000 	cmp	r1, r0
    1580:	e1a06002 	mov	r6, r2
    1584:	e2811001 	add	r1, r1, #1	; 0x1
    1588:	3afffff0 	bcc	1550 <malloc+0x70>
    158c:	ea00001e 	b	160c <malloc+0x12c>
    1590:	e5953004 	ldr	r3, [r5, #4]
    1594:	e5962004 	ldr	r2, [r6, #4]
    1598:	e0643003 	rsb	r3, r4, r3
    159c:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    15a0:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    15a4:	e1833002 	orr	r3, r3, r2
    15a8:	e5863004 	str	r3, [r6, #4]
    15ac:	e5951000 	ldr	r1, [r5]
    15b0:	e5950004 	ldr	r0, [r5, #4]
    15b4:	e1a0c005 	mov	ip, r5
    15b8:	e1a03006 	mov	r3, r6
    15bc:	e0841001 	add	r1, r4, r1
    15c0:	e3c42103 	bic	r2, r4, #-1073741824	; 0xc0000000
    15c4:	e2000103 	and	r0, r0, #-1073741824	; 0xc0000000
    15c8:	e5bc4008 	ldr	r4, [ip, #8]!
    15cc:	e4831008 	str	r1, [r3], #8
    15d0:	e1822000 	orr	r2, r2, r0
    15d4:	e5864008 	str	r4, [r6, #8]
    15d8:	e5843004 	str	r3, [r4, #4]
    15dc:	e985000c 	stmib	r5, {r2, r3}
    15e0:	e586c00c 	str	ip, [r6, #12]
    15e4:	e5953004 	ldr	r3, [r5, #4]
    15e8:	e59f1024 	ldr	r1, [pc, #36]	; 1614 <malloc+0x134>
    15ec:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    15f0:	e5853004 	str	r3, [r5, #4]
    15f4:	e5912010 	ldr	r2, [r1, #16]
    15f8:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    15fc:	e0833002 	add	r3, r3, r2
    1600:	e5813010 	str	r3, [r1, #16]
    1604:	e5950000 	ldr	r0, [r5]
    1608:	e8bd8070 	pop	{r4, r5, r6, pc}
    160c:	e3a00000 	mov	r0, #0	; 0x0
    1610:	e8bd8070 	pop	{r4, r5, r6, pc}
    1614:	0800d7ac 	.word	0x0800d7ac

00001618 <__sOMERGe>:
    1618:	e5902004 	ldr	r2, [r0, #4]
    161c:	e92d4030 	push	{r4, r5, lr}
    1620:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    1624:	e1a05000 	mov	r5, r0
    1628:	e1a0c001 	mov	ip, r1
    162c:	18bd8030 	popne	{r4, r5, pc}
    1630:	e5913004 	ldr	r3, [r1, #4]
    1634:	e2134103 	ands	r4, r3, #-1073741824	; 0xc0000000
    1638:	18bd8030 	popne	{r4, r5, pc}
    163c:	e0823003 	add	r3, r2, r3
    1640:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1644:	e59f0030 	ldr	r0, [pc, #48]	; 167c <__sOMERGe+0x64>
    1648:	e5853004 	str	r3, [r5, #4]
    164c:	e5814000 	str	r4, [r1]
    1650:	e5814004 	str	r4, [r1, #4]
    1654:	e5903004 	ldr	r3, [r0, #4]
    1658:	e591100c 	ldr	r1, [r1, #12]
    165c:	e59c2008 	ldr	r2, [ip, #8]
    1660:	e2833001 	add	r3, r3, #1	; 0x1
    1664:	e5812000 	str	r2, [r1]
    1668:	e5821004 	str	r1, [r2, #4]
    166c:	e5803004 	str	r3, [r0, #4]
    1670:	e58c4008 	str	r4, [ip, #8]
    1674:	e58c400c 	str	r4, [ip, #12]
    1678:	e8bd8030 	pop	{r4, r5, pc}
    167c:	0800d7ac 	.word	0x0800d7ac

00001680 <free>:
    1680:	e3500000 	cmp	r0, #0	; 0x0
    1684:	159f2090 	ldrne	r2, [pc, #144]	; 171c <free+0x9c>
    1688:	e92d4010 	push	{r4, lr}
    168c:	15b23014 	ldrne	r3, [r2, #20]!
    1690:	1a000004 	bne	16a8 <free+0x28>
    1694:	e8bd8010 	pop	{r4, pc}
    1698:	e5943000 	ldr	r3, [r4]
    169c:	e1500003 	cmp	r0, r3
    16a0:	0a000005 	beq	16bc <free+0x3c>
    16a4:	e5943008 	ldr	r3, [r4, #8]
    16a8:	e2434008 	sub	r4, r3, #8	; 0x8
    16ac:	e2843008 	add	r3, r4, #8	; 0x8
    16b0:	e1530002 	cmp	r3, r2
    16b4:	1afffff7 	bne	1698 <free+0x18>
    16b8:	e8bd8010 	pop	{r4, pc}
    16bc:	e5943004 	ldr	r3, [r4, #4]
    16c0:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    16c4:	08bd8010 	popeq	{r4, pc}
    16c8:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    16cc:	e59f1048 	ldr	r1, [pc, #72]	; 171c <free+0x9c>
    16d0:	e5843004 	str	r3, [r4, #4]
    16d4:	e5912010 	ldr	r2, [r1, #16]
    16d8:	e5940008 	ldr	r0, [r4, #8]
    16dc:	e0632002 	rsb	r2, r3, r2
    16e0:	e59f3038 	ldr	r3, [pc, #56]	; 1720 <free+0xa0>
    16e4:	e5812010 	str	r2, [r1, #16]
    16e8:	e1500003 	cmp	r0, r3
    16ec:	0a000002 	beq	16fc <free+0x7c>
    16f0:	e2401008 	sub	r1, r0, #8	; 0x8
    16f4:	e1a00004 	mov	r0, r4
    16f8:	ebffffc6 	bl	1618 <__sOMERGe>
    16fc:	e594000c 	ldr	r0, [r4, #12]
    1700:	e59f3018 	ldr	r3, [pc, #24]	; 1720 <free+0xa0>
    1704:	e1500003 	cmp	r0, r3
    1708:	08bd8010 	popeq	{r4, pc}
    170c:	e2400008 	sub	r0, r0, #8	; 0x8
    1710:	e1a01004 	mov	r1, r4
    1714:	e8bd4010 	pop	{r4, lr}
    1718:	eaffffbe 	b	1618 <__sOMERGe>
    171c:	0800d7ac 	.word	0x0800d7ac
    1720:	0800d7c0 	.word	0x0800d7c0

00001724 <__sOINFo>:
    1724:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1728:	e59f50a8 	ldr	r5, [pc, #168]	; 17d8 <__sOINFo+0xb4>
    172c:	e24dd004 	sub	sp, sp, #4	; 0x4
    1730:	e59f60a4 	ldr	r6, [pc, #164]	; 17dc <__sOINFo+0xb8>
    1734:	e8950006 	ldm	r5, {r1, r2}
    1738:	e59f00a0 	ldr	r0, [pc, #160]	; 17e0 <__sOINFo+0xbc>
    173c:	e1a0e00f 	mov	lr, pc
    1740:	e596f000 	ldr	pc, [r6]
    1744:	e2853008 	add	r3, r5, #8	; 0x8
    1748:	e8930098 	ldm	r3, {r3, r4, r7}
    174c:	e2444b01 	sub	r4, r4, #1024	; 0x400
    1750:	e0634004 	rsb	r4, r3, r4
    1754:	e3a00064 	mov	r0, #100	; 0x64
    1758:	e1a01004 	mov	r1, r4
    175c:	e0000790 	mul	r0, r0, r7
    1760:	eb003a68 	bl	10108 <__aeabi_uidiv>
    1764:	e1a01004 	mov	r1, r4
    1768:	e1a03000 	mov	r3, r0
    176c:	e1a02007 	mov	r2, r7
    1770:	e59f006c 	ldr	r0, [pc, #108]	; 17e4 <__sOINFo+0xc0>
    1774:	e1a0e00f 	mov	lr, pc
    1778:	e596f000 	ldr	pc, [r6]
    177c:	e59f0064 	ldr	r0, [pc, #100]	; 17e8 <__sOINFo+0xc4>
    1780:	e1a0e00f 	mov	lr, pc
    1784:	e596f000 	ldr	pc, [r6]
    1788:	e59f005c 	ldr	r0, [pc, #92]	; 17ec <__sOINFo+0xc8>
    178c:	e1a0e00f 	mov	lr, pc
    1790:	e596f000 	ldr	pc, [r6]
    1794:	e5b53014 	ldr	r3, [r5, #20]!
    1798:	ea000007 	b	17bc <__sOINFo+0x98>
    179c:	e8940006 	ldm	r4, {r1, r2}
    17a0:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    17a4:	03a03000 	moveq	r3, #0	; 0x0
    17a8:	13a03001 	movne	r3, #1	; 0x1
    17ac:	e3c22103 	bic	r2, r2, #-1073741824	; 0xc0000000
    17b0:	e1a0e00f 	mov	lr, pc
    17b4:	e596f000 	ldr	pc, [r6]
    17b8:	e5943008 	ldr	r3, [r4, #8]
    17bc:	e2434008 	sub	r4, r3, #8	; 0x8
    17c0:	e2843008 	add	r3, r4, #8	; 0x8
    17c4:	e1530005 	cmp	r3, r5
    17c8:	e59f0020 	ldr	r0, [pc, #32]	; 17f0 <__sOINFo+0xcc>
    17cc:	1afffff2 	bne	179c <__sOINFo+0x78>
    17d0:	e28dd004 	add	sp, sp, #4	; 0x4
    17d4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    17d8:	0800d7ac 	.word	0x0800d7ac
    17dc:	0800d76c 	.word	0x0800d76c
    17e0:	00011b7f 	.word	0x00011b7f
    17e4:	00011b99 	.word	0x00011b99
    17e8:	00011bbe 	.word	0x00011bbe
    17ec:	00011a6c 	.word	0x00011a6c
    17f0:	00011bcf 	.word	0x00011bcf

000017f4 <init_malloc>:
    17f4:	e59f20e4 	ldr	r2, [pc, #228]	; 18e0 <init_malloc+0xec>
    17f8:	e59f30e4 	ldr	r3, [pc, #228]	; 18e4 <init_malloc+0xf0>
    17fc:	e3c2001f 	bic	r0, r2, #31	; 0x1f
    1800:	e3c3301f 	bic	r3, r3, #31	; 0x1f
    1804:	e0602003 	rsb	r2, r0, r3
    1808:	e92d4010 	push	{r4, lr}
    180c:	e59f40d4 	ldr	r4, [pc, #212]	; 18e8 <init_malloc+0xf4>
    1810:	e3520b01 	cmp	r2, #1024	; 0x400
    1814:	e5840008 	str	r0, [r4, #8]
    1818:	e584300c 	str	r3, [r4, #12]
    181c:	93e00000 	mvnls	r0, #0	; 0x0
    1820:	98bd8010 	popls	{r4, pc}
    1824:	e284c014 	add	ip, r4, #20	; 0x14
    1828:	e3a03040 	mov	r3, #64	; 0x40
    182c:	e3a01000 	mov	r1, #0	; 0x0
    1830:	e3a02b01 	mov	r2, #1024	; 0x400
    1834:	e584c018 	str	ip, [r4, #24]
    1838:	e584c014 	str	ip, [r4, #20]
    183c:	e5843004 	str	r3, [r4, #4]
    1840:	e5843000 	str	r3, [r4]
    1844:	ebfffeb8 	bl	132c <memset>
    1848:	e5940008 	ldr	r0, [r4, #8]
    184c:	e594c000 	ldr	ip, [r4]
    1850:	e1a01000 	mov	r1, r0
    1854:	e3a02000 	mov	r2, #0	; 0x0
    1858:	ea000006 	b	1878 <init_malloc+0x84>
    185c:	e59e3004 	ldr	r3, [lr, #4]
    1860:	e3530000 	cmp	r3, #0	; 0x0
    1864:	059f207c 	ldreq	r2, [pc, #124]	; 18e8 <init_malloc+0xf4>
    1868:	05923004 	ldreq	r3, [r2, #4]
    186c:	02433001 	subeq	r3, r3, #1	; 0x1
    1870:	05823004 	streq	r3, [r2, #4]
    1874:	0a000005 	beq	1890 <init_malloc+0x9c>
    1878:	e152000c 	cmp	r2, ip
    187c:	e1a0e001 	mov	lr, r1
    1880:	e2822001 	add	r2, r2, #1	; 0x1
    1884:	e2811010 	add	r1, r1, #16	; 0x10
    1888:	3afffff3 	bcc	185c <init_malloc+0x68>
    188c:	e3a0e000 	mov	lr, #0	; 0x0
    1890:	e59fc050 	ldr	ip, [pc, #80]	; 18e8 <init_malloc+0xf4>
    1894:	e59e2004 	ldr	r2, [lr, #4]
    1898:	e59c300c 	ldr	r3, [ip, #12]
    189c:	e1a0100e 	mov	r1, lr
    18a0:	e2433b01 	sub	r3, r3, #1024	; 0x400
    18a4:	e0603003 	rsb	r3, r0, r3
    18a8:	e59c4018 	ldr	r4, [ip, #24]
    18ac:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    18b0:	e2800b01 	add	r0, r0, #1024	; 0x400
    18b4:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    18b8:	e4810008 	str	r0, [r1], #8
    18bc:	e1833002 	orr	r3, r3, r2
    18c0:	e28c2014 	add	r2, ip, #20	; 0x14
    18c4:	e58e2008 	str	r2, [lr, #8]
    18c8:	e58c1018 	str	r1, [ip, #24]
    18cc:	e5841000 	str	r1, [r4]
    18d0:	e58e3004 	str	r3, [lr, #4]
    18d4:	e58e400c 	str	r4, [lr, #12]
    18d8:	e3a00000 	mov	r0, #0	; 0x0
    18dc:	e8bd8010 	pop	{r4, pc}
    18e0:	0800f01f 	.word	0x0800f01f
    18e4:	0801a000 	.word	0x0801a000
    18e8:	0800d7ac 	.word	0x0800d7ac

000018ec <isi_check_header>:
    18ec:	e92d4070 	push	{r4, r5, r6, lr}
    18f0:	e5902000 	ldr	r2, [r0]
    18f4:	e59f3088 	ldr	r3, [pc, #136]	; 1984 <isi_check_header+0x98>
    18f8:	e1a05000 	mov	r5, r0
    18fc:	e1520003 	cmp	r2, r3
    1900:	e24dd008 	sub	sp, sp, #8	; 0x8
    1904:	13e0001e 	mvnne	r0, #30	; 0x1e
    1908:	1a00001b 	bne	197c <isi_check_header+0x90>
    190c:	e3a06000 	mov	r6, #0	; 0x0
    1910:	e595401c 	ldr	r4, [r5, #28]
    1914:	e3a01c02 	mov	r1, #512	; 0x200
    1918:	e585601c 	str	r6, [r5, #28]
    191c:	e1a00006 	mov	r0, r6
    1920:	eb000f63 	bl	56b4 <hash_init>
    1924:	e3a01c02 	mov	r1, #512	; 0x200
    1928:	e1a00005 	mov	r0, r5
    192c:	eb000f3b 	bl	5620 <hash_data>
    1930:	e28d0004 	add	r0, sp, #4	; 0x4
    1934:	eb000f1d 	bl	55b0 <hash_value>
    1938:	e585401c 	str	r4, [r5, #28]
    193c:	e59d3004 	ldr	r3, [sp, #4]
    1940:	e1540003 	cmp	r4, r3
    1944:	13e0001f 	mvnne	r0, #31	; 0x1f
    1948:	1a00000b 	bne	197c <isi_check_header+0x90>
    194c:	e5951004 	ldr	r1, [r5, #4]
    1950:	e59f4030 	ldr	r4, [pc, #48]	; 1988 <isi_check_header+0x9c>
    1954:	e3c114ff 	bic	r1, r1, #-16777216	; 0xff000000
    1958:	e59f002c 	ldr	r0, [pc, #44]	; 198c <isi_check_header+0xa0>
    195c:	e1a0e00f 	mov	lr, pc
    1960:	e594f000 	ldr	pc, [r4]
    1964:	e5d51007 	ldrb	r1, [r5, #7]
    1968:	e59f0020 	ldr	r0, [pc, #32]	; 1990 <isi_check_header+0xa4>
    196c:	e201100f 	and	r1, r1, #15	; 0xf
    1970:	e1a0e00f 	mov	lr, pc
    1974:	e594f000 	ldr	pc, [r4]
    1978:	e1a00006 	mov	r0, r6
    197c:	e28dd008 	add	sp, sp, #8	; 0x8
    1980:	e8bd8070 	pop	{r4, r5, r6, pc}
    1984:	6973692b 	.word	0x6973692b
    1988:	0800d76c 	.word	0x0800d76c
    198c:	00011bf7 	.word	0x00011bf7
    1990:	00011c05 	.word	0x00011c05

00001994 <isi_check_data_separate>:
    1994:	e92d4070 	push	{r4, r5, r6, lr}
    1998:	e1a05000 	mov	r5, r0
    199c:	e5d00007 	ldrb	r0, [r0, #7]
    19a0:	e24dd028 	sub	sp, sp, #40	; 0x28
    19a4:	e1a04001 	mov	r4, r1
    19a8:	e200000f 	and	r0, r0, #15	; 0xf
    19ac:	e595100c 	ldr	r1, [r5, #12]
    19b0:	eb000f3f 	bl	56b4 <hash_init>
    19b4:	e28d6008 	add	r6, sp, #8	; 0x8
    19b8:	e595100c 	ldr	r1, [r5, #12]
    19bc:	e1a00004 	mov	r0, r4
    19c0:	eb000f16 	bl	5620 <hash_data>
    19c4:	e1a00006 	mov	r0, r6
    19c8:	eb000ef8 	bl	55b0 <hash_value>
    19cc:	eb000ef0 	bl	5594 <hash_len>
    19d0:	e2854020 	add	r4, r5, #32	; 0x20
    19d4:	e1a02000 	mov	r2, r0
    19d8:	e1a01004 	mov	r1, r4
    19dc:	e1a00006 	mov	r0, r6
    19e0:	ebfffe7a 	bl	13d0 <memcmp>
    19e4:	e3500000 	cmp	r0, #0	; 0x0
    19e8:	0a000014 	beq	1a40 <isi_check_data_separate+0xac>
    19ec:	e595100c 	ldr	r1, [r5, #12]
    19f0:	e59f3050 	ldr	r3, [pc, #80]	; 1a48 <isi_check_data_separate+0xb4>
    19f4:	e59f0050 	ldr	r0, [pc, #80]	; 1a4c <isi_check_data_separate+0xb8>
    19f8:	e1a0e00f 	mov	lr, pc
    19fc:	e593f000 	ldr	pc, [r3]
    1a00:	eb000ee3 	bl	5594 <hash_len>
    1a04:	e1a01004 	mov	r1, r4
    1a08:	e1a03000 	mov	r3, r0
    1a0c:	e3a02001 	mov	r2, #1	; 0x1
    1a10:	e1a00004 	mov	r0, r4
    1a14:	e3a04020 	mov	r4, #32	; 0x20
    1a18:	e58d4000 	str	r4, [sp]
    1a1c:	eb002d1d 	bl	ce98 <print_buffer>
    1a20:	eb000edb 	bl	5594 <hash_len>
    1a24:	e1a01006 	mov	r1, r6
    1a28:	e1a03000 	mov	r3, r0
    1a2c:	e3a02001 	mov	r2, #1	; 0x1
    1a30:	e1a00006 	mov	r0, r6
    1a34:	e58d4000 	str	r4, [sp]
    1a38:	eb002d16 	bl	ce98 <print_buffer>
    1a3c:	e3e00020 	mvn	r0, #32	; 0x20
    1a40:	e28dd028 	add	sp, sp, #40	; 0x28
    1a44:	e8bd8070 	pop	{r4, r5, r6, pc}
    1a48:	0800d76c 	.word	0x0800d76c
    1a4c:	00011c18 	.word	0x00011c18

00001a50 <isi_check_data>:
    1a50:	e2801c02 	add	r1, r0, #512	; 0x200
    1a54:	eaffffce 	b	1994 <isi_check_data_separate>

00001a58 <isi_check>:
    1a58:	e92d4010 	push	{r4, lr}
    1a5c:	e1a04000 	mov	r4, r0
    1a60:	ebffffa1 	bl	18ec <isi_check_header>
    1a64:	e3500000 	cmp	r0, #0	; 0x0
    1a68:	18bd8010 	popne	{r4, pc}
    1a6c:	e1a00004 	mov	r0, r4
    1a70:	e8bd4010 	pop	{r4, lr}
    1a74:	eafffff5 	b	1a50 <isi_check_data>

00001a78 <load_serial_ymodem>:
    1a78:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    1a7c:	e24dde41 	sub	sp, sp, #1040	; 0x410
    1a80:	e28d4b01 	add	r4, sp, #1024	; 0x400
    1a84:	e284400c 	add	r4, r4, #12	; 0xc
    1a88:	e3a03002 	mov	r3, #2	; 0x2
    1a8c:	e1a07000 	mov	r7, r0
    1a90:	e1a01004 	mov	r1, r4
    1a94:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1a98:	e58d3404 	str	r3, [sp, #1028]
    1a9c:	eb00391f 	bl	ff20 <xyzModem_stream_open>
    1aa0:	e3500000 	cmp	r0, #0	; 0x0
    1aa4:	1a000012 	bne	1af4 <load_serial_ymodem+0x7c>
    1aa8:	e1a08004 	mov	r8, r4
    1aac:	e1a05000 	mov	r5, r0
    1ab0:	e1a04000 	mov	r4, r0
    1ab4:	e1a0600d 	mov	r6, sp
    1ab8:	ea000001 	b	1ac4 <load_serial_ymodem+0x4c>
    1abc:	e085500c 	add	r5, r5, ip
    1ac0:	ebfffe21 	bl	134c <memcpy>
    1ac4:	e3a01b01 	mov	r1, #1024	; 0x400
    1ac8:	e1a02008 	mov	r2, r8
    1acc:	e1a0000d 	mov	r0, sp
    1ad0:	eb00387e 	bl	fcd0 <xyzModem_stream_read>
    1ad4:	e0843007 	add	r3, r4, r7
    1ad8:	e250c000 	subs	ip, r0, #0	; 0x0
    1adc:	e1a0100d 	mov	r1, sp
    1ae0:	e1a0200c 	mov	r2, ip
    1ae4:	e1a00003 	mov	r0, r3
    1ae8:	e084400c 	add	r4, r4, ip
    1aec:	cafffff2 	bgt	1abc <load_serial_ymodem+0x44>
    1af0:	ea000007 	b	1b14 <load_serial_ymodem+0x9c>
    1af4:	e59f3054 	ldr	r3, [pc, #84]	; 1b50 <load_serial_ymodem+0xd8>
    1af8:	e59d040c 	ldr	r0, [sp, #1036]
    1afc:	e5934000 	ldr	r4, [r3]
    1b00:	eb0034f9 	bl	eeec <xyzModem_error>
    1b04:	e1a01000 	mov	r1, r0
    1b08:	e59f0044 	ldr	r0, [pc, #68]	; 1b54 <load_serial_ymodem+0xdc>
    1b0c:	e12fff34 	blx	r4
    1b10:	e3a05000 	mov	r5, #0	; 0x0
    1b14:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1b18:	e280000c 	add	r0, r0, #12	; 0xc
    1b1c:	eb0034da 	bl	ee8c <xyzModem_stream_close>
    1b20:	e3a00000 	mov	r0, #0	; 0x0
    1b24:	e59f102c 	ldr	r1, [pc, #44]	; 1b58 <load_serial_ymodem+0xe0>
    1b28:	eb003788 	bl	f950 <xyzModem_stream_terminate>
    1b2c:	e1a01005 	mov	r1, r5
    1b30:	e59f0024 	ldr	r0, [pc, #36]	; 1b5c <load_serial_ymodem+0xe4>
    1b34:	e1a02005 	mov	r2, r5
    1b38:	e59f3010 	ldr	r3, [pc, #16]	; 1b50 <load_serial_ymodem+0xd8>
    1b3c:	e1a0e00f 	mov	lr, pc
    1b40:	e593f000 	ldr	pc, [r3]
    1b44:	e1a00007 	mov	r0, r7
    1b48:	e28dde41 	add	sp, sp, #1040	; 0x410
    1b4c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    1b50:	0800d76c 	.word	0x0800d76c
    1b54:	00011d14 	.word	0x00011d14
    1b58:	00001b60 	.word	0x00001b60
    1b5c:	00011c3c 	.word	0x00011c3c

00001b60 <getcxmodem>:
    1b60:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    1b64:	e24dd004 	sub	sp, sp, #4	; 0x4
    1b68:	eb003179 	bl	e154 <tstc>
    1b6c:	e3500000 	cmp	r0, #0	; 0x0
    1b70:	1a000002 	bne	1b80 <getcxmodem+0x20>
    1b74:	e3e00000 	mvn	r0, #0	; 0x0
    1b78:	e28dd004 	add	sp, sp, #4	; 0x4
    1b7c:	e8bd8000 	pop	{pc}
    1b80:	e28dd004 	add	sp, sp, #4	; 0x4
    1b84:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    1b88:	ea003172 	b	e158 <getc>

00001b8c <do_load_serial_bin>:
    1b8c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1b90:	e3520001 	cmp	r2, #1	; 0x1
    1b94:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1b98:	e1a06002 	mov	r6, r2
    1b9c:	e1a07000 	mov	r7, r0
    1ba0:	e1a05003 	mov	r5, r3
    1ba4:	ca000006 	bgt	1bc4 <do_load_serial_bin+0x38>
    1ba8:	e59f0120 	ldr	r0, [pc, #288]	; 1cd0 <do_load_serial_bin+0x144>
    1bac:	e3a06000 	mov	r6, #0	; 0x0
    1bb0:	e59f311c 	ldr	r3, [pc, #284]	; 1cd4 <do_load_serial_bin+0x148>
    1bb4:	e1a0e00f 	mov	lr, pc
    1bb8:	e593f000 	ldr	pc, [r3]
    1bbc:	e1a04006 	mov	r4, r6
    1bc0:	ea00000c 	b	1bf8 <do_load_serial_bin+0x6c>
    1bc4:	e5930004 	ldr	r0, [r3, #4]
    1bc8:	e3a01000 	mov	r1, #0	; 0x0
    1bcc:	e3a02010 	mov	r2, #16	; 0x10
    1bd0:	eb002d48 	bl	d0f8 <simple_strtoul>
    1bd4:	e3560003 	cmp	r6, #3	; 0x3
    1bd8:	e1a04000 	mov	r4, r0
    1bdc:	13a06000 	movne	r6, #0	; 0x0
    1be0:	1a000004 	bne	1bf8 <do_load_serial_bin+0x6c>
    1be4:	e5950008 	ldr	r0, [r5, #8]
    1be8:	e3a01000 	mov	r1, #0	; 0x0
    1bec:	e3a02010 	mov	r2, #16	; 0x10
    1bf0:	eb002d40 	bl	d0f8 <simple_strtoul>
    1bf4:	e1a06000 	mov	r6, r0
    1bf8:	e5950000 	ldr	r0, [r5]
    1bfc:	e59f10d4 	ldr	r1, [pc, #212]	; 1cd8 <do_load_serial_bin+0x14c>
    1c00:	ebfffd8a 	bl	1230 <strcmp>
    1c04:	e3500000 	cmp	r0, #0	; 0x0
    1c08:	1a000007 	bne	1c2c <do_load_serial_bin+0xa0>
    1c0c:	e1a01004 	mov	r1, r4
    1c10:	e59f00c4 	ldr	r0, [pc, #196]	; 1cdc <do_load_serial_bin+0x150>
    1c14:	e59f30b8 	ldr	r3, [pc, #184]	; 1cd4 <do_load_serial_bin+0x148>
    1c18:	e1a0e00f 	mov	lr, pc
    1c1c:	e593f000 	ldr	pc, [r3]
    1c20:	e1a00004 	mov	r0, r4
    1c24:	ebffff93 	bl	1a78 <load_serial_ymodem>
    1c28:	ea000025 	b	1cc4 <do_load_serial_bin+0x138>
    1c2c:	e5950000 	ldr	r0, [r5]
    1c30:	e59f10a8 	ldr	r1, [pc, #168]	; 1ce0 <do_load_serial_bin+0x154>
    1c34:	ebfffd7d 	bl	1230 <strcmp>
    1c38:	e3500000 	cmp	r0, #0	; 0x0
    1c3c:	e59f5090 	ldr	r5, [pc, #144]	; 1cd4 <do_load_serial_bin+0x148>
    1c40:	1a00001b 	bne	1cb4 <do_load_serial_bin+0x128>
    1c44:	e1a02006 	mov	r2, r6
    1c48:	e59f0094 	ldr	r0, [pc, #148]	; 1ce4 <do_load_serial_bin+0x158>
    1c4c:	e1a01004 	mov	r1, r4
    1c50:	e1a0e00f 	mov	lr, pc
    1c54:	e595f000 	ldr	pc, [r5]
    1c58:	e1a00004 	mov	r0, r4
    1c5c:	e1a01006 	mov	r1, r6
    1c60:	eb00369d 	bl	f6dc <control_send>
    1c64:	e3500000 	cmp	r0, #0	; 0x0
    1c68:	aa000005 	bge	1c84 <do_load_serial_bin+0xf8>
    1c6c:	e59d0004 	ldr	r0, [sp, #4]
    1c70:	e5954000 	ldr	r4, [r5]
    1c74:	eb00349c 	bl	eeec <xyzModem_error>
    1c78:	e1a01000 	mov	r1, r0
    1c7c:	e59f0064 	ldr	r0, [pc, #100]	; 1ce8 <do_load_serial_bin+0x15c>
    1c80:	e12fff34 	blx	r4
    1c84:	e28d0004 	add	r0, sp, #4	; 0x4
    1c88:	eb00347f 	bl	ee8c <xyzModem_stream_close>
    1c8c:	e3a00000 	mov	r0, #0	; 0x0
    1c90:	e59f1054 	ldr	r1, [pc, #84]	; 1cec <do_load_serial_bin+0x160>
    1c94:	eb00372d 	bl	f950 <xyzModem_stream_terminate>
    1c98:	e1a01006 	mov	r1, r6
    1c9c:	e59f004c 	ldr	r0, [pc, #76]	; 1cf0 <do_load_serial_bin+0x164>
    1ca0:	e1a02006 	mov	r2, r6
    1ca4:	e59f3028 	ldr	r3, [pc, #40]	; 1cd4 <do_load_serial_bin+0x148>
    1ca8:	e1a0e00f 	mov	lr, pc
    1cac:	e593f000 	ldr	pc, [r3]
    1cb0:	ea000003 	b	1cc4 <do_load_serial_bin+0x138>
    1cb4:	e5971014 	ldr	r1, [r7, #20]
    1cb8:	e59f0034 	ldr	r0, [pc, #52]	; 1cf4 <do_load_serial_bin+0x168>
    1cbc:	e1a0e00f 	mov	lr, pc
    1cc0:	e595f000 	ldr	pc, [r5]
    1cc4:	e3a00000 	mov	r0, #0	; 0x0
    1cc8:	e28dd00c 	add	sp, sp, #12	; 0xc
    1ccc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1cd0:	00011c64 	.word	0x00011c64
    1cd4:	0800d76c 	.word	0x0800d76c
    1cd8:	00011c7e 	.word	0x00011c7e
    1cdc:	00011c81 	.word	0x00011c81
    1ce0:	00011cc4 	.word	0x00011cc4
    1ce4:	00011cc7 	.word	0x00011cc7
    1ce8:	00011d14 	.word	0x00011d14
    1cec:	00001b60 	.word	0x00001b60
    1cf0:	00011c3c 	.word	0x00011c3c
    1cf4:	00011d13 	.word	0x00011d13

00001cf8 <do_mem_cp>:
    1cf8:	e3520004 	cmp	r2, #4	; 0x4
    1cfc:	e92d4070 	push	{r4, r5, r6, lr}
    1d00:	e1a04003 	mov	r4, r3
    1d04:	0a000002 	beq	1d14 <do_mem_cp+0x1c>
    1d08:	eb0003ad 	bl	2bc4 <cmd_usage>
    1d0c:	e3a00001 	mov	r0, #1	; 0x1
    1d10:	e8bd8070 	pop	{r4, r5, r6, pc}
    1d14:	e5930004 	ldr	r0, [r3, #4]
    1d18:	e3a01000 	mov	r1, #0	; 0x0
    1d1c:	e3a02010 	mov	r2, #16	; 0x10
    1d20:	eb002cf4 	bl	d0f8 <simple_strtoul>
    1d24:	e3a01000 	mov	r1, #0	; 0x0
    1d28:	e3a02010 	mov	r2, #16	; 0x10
    1d2c:	e1a06000 	mov	r6, r0
    1d30:	e5940008 	ldr	r0, [r4, #8]
    1d34:	eb002cef 	bl	d0f8 <simple_strtoul>
    1d38:	e3a01000 	mov	r1, #0	; 0x0
    1d3c:	e1a05000 	mov	r5, r0
    1d40:	e3a02010 	mov	r2, #16	; 0x10
    1d44:	e594000c 	ldr	r0, [r4, #12]
    1d48:	eb002cea 	bl	d0f8 <simple_strtoul>
    1d4c:	e2800003 	add	r0, r0, #3	; 0x3
    1d50:	e1b00120 	lsrs	r0, r0, #2
    1d54:	1a000002 	bne	1d64 <do_mem_cp+0x6c>
    1d58:	ea000006 	b	1d78 <do_mem_cp+0x80>
    1d5c:	e5163004 	ldr	r3, [r6, #-4]
    1d60:	e5053004 	str	r3, [r5, #-4]
    1d64:	e3500000 	cmp	r0, #0	; 0x0
    1d68:	e2866004 	add	r6, r6, #4	; 0x4
    1d6c:	e2400001 	sub	r0, r0, #1	; 0x1
    1d70:	e2855004 	add	r5, r5, #4	; 0x4
    1d74:	1afffff8 	bne	1d5c <do_mem_cp+0x64>
    1d78:	e3a00000 	mov	r0, #0	; 0x0
    1d7c:	e8bd8070 	pop	{r4, r5, r6, pc}

00001d80 <do_mem_cmp>:
    1d80:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1d84:	e3520004 	cmp	r2, #4	; 0x4
    1d88:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1d8c:	e1a04003 	mov	r4, r3
    1d90:	0a000002 	beq	1da0 <do_mem_cmp+0x20>
    1d94:	eb00038a 	bl	2bc4 <cmd_usage>
    1d98:	e3a04001 	mov	r4, #1	; 0x1
    1d9c:	ea000029 	b	1e48 <do_mem_cmp+0xc8>
    1da0:	e5930004 	ldr	r0, [r3, #4]
    1da4:	e3a01000 	mov	r1, #0	; 0x0
    1da8:	e3a02010 	mov	r2, #16	; 0x10
    1dac:	eb002cd1 	bl	d0f8 <simple_strtoul>
    1db0:	e3a01000 	mov	r1, #0	; 0x0
    1db4:	e3a02010 	mov	r2, #16	; 0x10
    1db8:	e1a07000 	mov	r7, r0
    1dbc:	e5940008 	ldr	r0, [r4, #8]
    1dc0:	eb002ccc 	bl	d0f8 <simple_strtoul>
    1dc4:	e3a01000 	mov	r1, #0	; 0x0
    1dc8:	e1a06000 	mov	r6, r0
    1dcc:	e3a02010 	mov	r2, #16	; 0x10
    1dd0:	e594000c 	ldr	r0, [r4, #12]
    1dd4:	eb002cc7 	bl	d0f8 <simple_strtoul>
    1dd8:	e2800003 	add	r0, r0, #3	; 0x3
    1ddc:	e1a00120 	lsr	r0, r0, #2
    1de0:	e3a05000 	mov	r5, #0	; 0x0
    1de4:	ea00000f 	b	1e28 <do_mem_cmp+0xa8>
    1de8:	e5972000 	ldr	r2, [r7]
    1dec:	e5963000 	ldr	r3, [r6]
    1df0:	e1520003 	cmp	r2, r3
    1df4:	0a000008 	beq	1e1c <do_mem_cmp+0x9c>
    1df8:	e58d3000 	str	r3, [sp]
    1dfc:	e1a01007 	mov	r1, r7
    1e00:	e1a03006 	mov	r3, r6
    1e04:	e59f0048 	ldr	r0, [pc, #72]	; 1e54 <do_mem_cmp+0xd4>
    1e08:	e59fc048 	ldr	ip, [pc, #72]	; 1e58 <do_mem_cmp+0xd8>
    1e0c:	e1a0e00f 	mov	lr, pc
    1e10:	e59cf000 	ldr	pc, [ip]
    1e14:	e3a04001 	mov	r4, #1	; 0x1
    1e18:	ea000005 	b	1e34 <do_mem_cmp+0xb4>
    1e1c:	e2855001 	add	r5, r5, #1	; 0x1
    1e20:	e2877004 	add	r7, r7, #4	; 0x4
    1e24:	e2866004 	add	r6, r6, #4	; 0x4
    1e28:	e1500005 	cmp	r0, r5
    1e2c:	1affffed 	bne	1de8 <do_mem_cmp+0x68>
    1e30:	e3a04000 	mov	r4, #0	; 0x0
    1e34:	e1a01005 	mov	r1, r5
    1e38:	e59f001c 	ldr	r0, [pc, #28]	; 1e5c <do_mem_cmp+0xdc>
    1e3c:	e59f3014 	ldr	r3, [pc, #20]	; 1e58 <do_mem_cmp+0xd8>
    1e40:	e1a0e00f 	mov	lr, pc
    1e44:	e593f000 	ldr	pc, [r3]
    1e48:	e1a00004 	mov	r0, r4
    1e4c:	e28dd00c 	add	sp, sp, #12	; 0xc
    1e50:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1e54:	00011e19 	.word	0x00011e19
    1e58:	0800d76c 	.word	0x0800d76c
    1e5c:	00011e51 	.word	0x00011e51

00001e60 <do_mem_mw>:
    1e60:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1e64:	e1a06002 	mov	r6, r2
    1e68:	e2422003 	sub	r2, r2, #3	; 0x3
    1e6c:	e3520001 	cmp	r2, #1	; 0x1
    1e70:	e24dd004 	sub	sp, sp, #4	; 0x4
    1e74:	e1a04003 	mov	r4, r3
    1e78:	9a000002 	bls	1e88 <do_mem_mw+0x28>
    1e7c:	eb000350 	bl	2bc4 <cmd_usage>
    1e80:	e3a00001 	mov	r0, #1	; 0x1
    1e84:	ea000016 	b	1ee4 <do_mem_mw+0x84>
    1e88:	e3a01000 	mov	r1, #0	; 0x0
    1e8c:	e3a02010 	mov	r2, #16	; 0x10
    1e90:	e5930004 	ldr	r0, [r3, #4]
    1e94:	eb002c97 	bl	d0f8 <simple_strtoul>
    1e98:	e3a01000 	mov	r1, #0	; 0x0
    1e9c:	e1a05000 	mov	r5, r0
    1ea0:	e3a02010 	mov	r2, #16	; 0x10
    1ea4:	e5940008 	ldr	r0, [r4, #8]
    1ea8:	eb002c92 	bl	d0f8 <simple_strtoul>
    1eac:	e3560004 	cmp	r6, #4	; 0x4
    1eb0:	e1a07000 	mov	r7, r0
    1eb4:	13a00001 	movne	r0, #1	; 0x1
    1eb8:	1a000006 	bne	1ed8 <do_mem_mw+0x78>
    1ebc:	e594000c 	ldr	r0, [r4, #12]
    1ec0:	e3a01000 	mov	r1, #0	; 0x0
    1ec4:	e3a02010 	mov	r2, #16	; 0x10
    1ec8:	eb002c8a 	bl	d0f8 <simple_strtoul>
    1ecc:	ea000001 	b	1ed8 <do_mem_mw+0x78>
    1ed0:	e5057004 	str	r7, [r5, #-4]
    1ed4:	e2400001 	sub	r0, r0, #1	; 0x1
    1ed8:	e3500000 	cmp	r0, #0	; 0x0
    1edc:	e2855004 	add	r5, r5, #4	; 0x4
    1ee0:	1afffffa 	bne	1ed0 <do_mem_mw+0x70>
    1ee4:	e28dd004 	add	sp, sp, #4	; 0x4
    1ee8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00001eec <do_mem_md>:
    1eec:	e92d4070 	push	{r4, r5, r6, lr}
    1ef0:	e3520001 	cmp	r2, #1	; 0x1
    1ef4:	e24dd008 	sub	sp, sp, #8	; 0x8
    1ef8:	e1a04002 	mov	r4, r2
    1efc:	e1a05003 	mov	r5, r3
    1f00:	ca000002 	bgt	1f10 <do_mem_md+0x24>
    1f04:	eb00032e 	bl	2bc4 <cmd_usage>
    1f08:	e3a00001 	mov	r0, #1	; 0x1
    1f0c:	ea000014 	b	1f64 <do_mem_md+0x78>
    1f10:	e5930004 	ldr	r0, [r3, #4]
    1f14:	e3a01000 	mov	r1, #0	; 0x0
    1f18:	e3a02010 	mov	r2, #16	; 0x10
    1f1c:	eb002c75 	bl	d0f8 <simple_strtoul>
    1f20:	e3540002 	cmp	r4, #2	; 0x2
    1f24:	e1a06000 	mov	r6, r0
    1f28:	03a030a0 	moveq	r3, #160	; 0xa0
    1f2c:	0a000004 	beq	1f44 <do_mem_md+0x58>
    1f30:	e5950008 	ldr	r0, [r5, #8]
    1f34:	e3a01000 	mov	r1, #0	; 0x0
    1f38:	e3a02010 	mov	r2, #16	; 0x10
    1f3c:	eb002c6d 	bl	d0f8 <simple_strtoul>
    1f40:	e1a03000 	mov	r3, r0
    1f44:	e3a0c004 	mov	ip, #4	; 0x4
    1f48:	e1a00006 	mov	r0, r6
    1f4c:	e1a0200c 	mov	r2, ip
    1f50:	e1a03123 	lsr	r3, r3, #2
    1f54:	e1a01006 	mov	r1, r6
    1f58:	e58dc000 	str	ip, [sp]
    1f5c:	eb002bcd 	bl	ce98 <print_buffer>
    1f60:	e3a00000 	mov	r0, #0	; 0x0
    1f64:	e28dd008 	add	sp, sp, #8	; 0x8
    1f68:	e8bd8070 	pop	{r4, r5, r6, pc}

00001f6c <do_sc>:
    1f6c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1f70:	e3520002 	cmp	r2, #2	; 0x2
    1f74:	e24ddf47 	sub	sp, sp, #284	; 0x11c
    1f78:	0a000002 	beq	1f88 <do_sc+0x1c>
    1f7c:	eb000310 	bl	2bc4 <cmd_usage>
    1f80:	e3a00001 	mov	r0, #1	; 0x1
    1f84:	ea000060 	b	210c <do_sc+0x1a0>
    1f88:	e5930004 	ldr	r0, [r3, #4]
    1f8c:	e3a01000 	mov	r1, #0	; 0x0
    1f90:	e3a02010 	mov	r2, #16	; 0x10
    1f94:	eb002c57 	bl	d0f8 <simple_strtoul>
    1f98:	e28d4f41 	add	r4, sp, #260	; 0x104
    1f9c:	e2844002 	add	r4, r4, #2	; 0x2
    1fa0:	e59f116c 	ldr	r1, [pc, #364]	; 2114 <do_sc+0x1a8>
    1fa4:	e1a06000 	mov	r6, r0
    1fa8:	e3a02012 	mov	r2, #18	; 0x12
    1fac:	e1a00004 	mov	r0, r4
    1fb0:	ebfffce5 	bl	134c <memcpy>
    1fb4:	e3a01000 	mov	r1, #0	; 0x0
    1fb8:	ea000004 	b	1fd0 <do_sc+0x64>
    1fbc:	e7d12006 	ldrb	r2, [r1, r6]
    1fc0:	e7d43001 	ldrb	r3, [r4, r1]
    1fc4:	e2811001 	add	r1, r1, #1	; 0x1
    1fc8:	e1520003 	cmp	r2, r3
    1fcc:	1a000014 	bne	2024 <do_sc+0xb8>
    1fd0:	e3510012 	cmp	r1, #18	; 0x12
    1fd4:	e0815006 	add	r5, r1, r6
    1fd8:	1afffff7 	bne	1fbc <do_sc+0x50>
    1fdc:	e2851eff 	add	r1, r5, #4080	; 0xff0
    1fe0:	e281100f 	add	r1, r1, #15	; 0xf
    1fe4:	e1a02005 	mov	r2, r5
    1fe8:	e5d23000 	ldrb	r3, [r2]
    1fec:	e353000a 	cmp	r3, #10	; 0xa
    1ff0:	1a000005 	bne	200c <do_sc+0xa0>
    1ff4:	e5d23001 	ldrb	r3, [r2, #1]
    1ff8:	e353005d 	cmp	r3, #93	; 0x5d
    1ffc:	1a000002 	bne	200c <do_sc+0xa0>
    2000:	e5d23002 	ldrb	r3, [r2, #2]
    2004:	e353005d 	cmp	r3, #93	; 0x5d
    2008:	0a000003 	beq	201c <do_sc+0xb0>
    200c:	e1520001 	cmp	r2, r1
    2010:	e2822001 	add	r2, r2, #1	; 0x1
    2014:	1afffff3 	bne	1fe8 <do_sc+0x7c>
    2018:	ea000001 	b	2024 <do_sc+0xb8>
    201c:	e3550000 	cmp	r5, #0	; 0x0
    2020:	1a000006 	bne	2040 <do_sc+0xd4>
    2024:	e59f00ec 	ldr	r0, [pc, #236]	; 2118 <do_sc+0x1ac>
    2028:	e1a01006 	mov	r1, r6
    202c:	e59f30e8 	ldr	r3, [pc, #232]	; 211c <do_sc+0x1b0>
    2030:	e1a0e00f 	mov	lr, pc
    2034:	e593f000 	ldr	pc, [r3]
    2038:	e3e00000 	mvn	r0, #0	; 0x0
    203c:	ea000032 	b	210c <do_sc+0x1a0>
    2040:	e59f40d4 	ldr	r4, [pc, #212]	; 211c <do_sc+0x1b0>
    2044:	e1a01006 	mov	r1, r6
    2048:	e59f00d0 	ldr	r0, [pc, #208]	; 2120 <do_sc+0x1b4>
    204c:	e28d6008 	add	r6, sp, #8	; 0x8
    2050:	e1a0e00f 	mov	lr, pc
    2054:	e594f000 	ldr	pc, [r4]
    2058:	e2466002 	sub	r6, r6, #2	; 0x2
    205c:	e1a07004 	mov	r7, r4
    2060:	ea000000 	b	2068 <do_sc+0xfc>
    2064:	e2855001 	add	r5, r5, #1	; 0x1
    2068:	e5d53000 	ldrb	r3, [r5]
    206c:	e353002d 	cmp	r3, #45	; 0x2d
    2070:	1353000a 	cmpne	r3, #10	; 0xa
    2074:	0afffffa 	beq	2064 <do_sc+0xf8>
    2078:	e353005b 	cmp	r3, #91	; 0x5b
    207c:	0afffff8 	beq	2064 <do_sc+0xf8>
    2080:	e353005d 	cmp	r3, #93	; 0x5d
    2084:	0a000001 	beq	2090 <do_sc+0x124>
    2088:	e1a02006 	mov	r2, r6
    208c:	ea000005 	b	20a8 <do_sc+0x13c>
    2090:	e5d53001 	ldrb	r3, [r5, #1]
    2094:	e353005d 	cmp	r3, #93	; 0x5d
    2098:	1afffffa 	bne	2088 <do_sc+0x11c>
    209c:	ea00000c 	b	20d4 <do_sc+0x168>
    20a0:	e5423001 	strb	r3, [r2, #-1]
    20a4:	e2855001 	add	r5, r5, #1	; 0x1
    20a8:	e5d53000 	ldrb	r3, [r5]
    20ac:	e1a01002 	mov	r1, r2
    20b0:	e353000a 	cmp	r3, #10	; 0xa
    20b4:	1353005d 	cmpne	r3, #93	; 0x5d
    20b8:	03a04000 	moveq	r4, #0	; 0x0
    20bc:	13a04001 	movne	r4, #1	; 0x1
    20c0:	e2822001 	add	r2, r2, #1	; 0x1
    20c4:	1afffff5 	bne	20a0 <do_sc+0x134>
    20c8:	e3550000 	cmp	r5, #0	; 0x0
    20cc:	e5c14000 	strb	r4, [r1]
    20d0:	1a000005 	bne	20ec <do_sc+0x180>
    20d4:	e59f0048 	ldr	r0, [pc, #72]	; 2124 <do_sc+0x1b8>
    20d8:	e59f303c 	ldr	r3, [pc, #60]	; 211c <do_sc+0x1b0>
    20dc:	e1a0e00f 	mov	lr, pc
    20e0:	e593f000 	ldr	pc, [r3]
    20e4:	e3a00000 	mov	r0, #0	; 0x0
    20e8:	ea000007 	b	210c <do_sc+0x1a0>
    20ec:	e1a01006 	mov	r1, r6
    20f0:	e59f0030 	ldr	r0, [pc, #48]	; 2128 <do_sc+0x1bc>
    20f4:	e1a0e00f 	mov	lr, pc
    20f8:	e597f000 	ldr	pc, [r7]
    20fc:	e1a00006 	mov	r0, r6
    2100:	e1a01004 	mov	r1, r4
    2104:	eb00304f 	bl	e248 <run_command>
    2108:	eaffffd6 	b	2068 <do_sc+0xfc>
    210c:	e28ddf47 	add	sp, sp, #284	; 0x11c
    2110:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    2114:	0001180c 	.word	0x0001180c
    2118:	00011f1b 	.word	0x00011f1b
    211c:	0800d76c 	.word	0x0800d76c
    2120:	00011f38 	.word	0x00011f38
    2124:	00011f51 	.word	0x00011f51
    2128:	00011f5e 	.word	0x00011f5e

0000212c <do_ls>:
    212c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2130:	e59f001c 	ldr	r0, [pc, #28]	; 2154 <do_ls+0x28>
    2134:	e24dd004 	sub	sp, sp, #4	; 0x4
    2138:	e59f1018 	ldr	r1, [pc, #24]	; 2158 <do_ls+0x2c>
    213c:	e59f3018 	ldr	r3, [pc, #24]	; 215c <do_ls+0x30>
    2140:	e1a0e00f 	mov	lr, pc
    2144:	e593f000 	ldr	pc, [r3]
    2148:	e3a00000 	mov	r0, #0	; 0x0
    214c:	e28dd004 	add	sp, sp, #4	; 0x4
    2150:	e8bd8000 	pop	{pc}
    2154:	00011d14 	.word	0x00011d14
    2158:	0800d10c 	.word	0x0800d10c
    215c:	0800d76c 	.word	0x0800d76c

00002160 <do_linkpc>:
    2160:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2164:	e24dd004 	sub	sp, sp, #4	; 0x4
    2168:	eb002964 	bl	c700 <linkpc_loop>
    216c:	e3a00000 	mov	r0, #0	; 0x0
    2170:	e28dd004 	add	sp, sp, #4	; 0x4
    2174:	e8bd8000 	pop	{pc}

00002178 <do_pads>:
    2178:	e3520002 	cmp	r2, #2	; 0x2
    217c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    2180:	d59f0194 	ldrle	r0, [pc, #404]	; 231c <do_pads+0x1a4>
    2184:	e1a05002 	mov	r5, r2
    2188:	e1a06003 	mov	r6, r3
    218c:	da000015 	ble	21e8 <do_pads+0x70>
    2190:	e5930008 	ldr	r0, [r3, #8]
    2194:	e3a01000 	mov	r1, #0	; 0x0
    2198:	e3a0200a 	mov	r2, #10	; 0xa
    219c:	eb002bd5 	bl	d0f8 <simple_strtoul>
    21a0:	e3550003 	cmp	r5, #3	; 0x3
    21a4:	e1a04000 	mov	r4, r0
    21a8:	01a07000 	moveq	r7, r0
    21ac:	0a000004 	beq	21c4 <do_pads+0x4c>
    21b0:	e596000c 	ldr	r0, [r6, #12]
    21b4:	e3a01000 	mov	r1, #0	; 0x0
    21b8:	e3a0200a 	mov	r2, #10	; 0xa
    21bc:	eb002bcd 	bl	d0f8 <simple_strtoul>
    21c0:	e1a07000 	mov	r7, r0
    21c4:	e1943007 	orrs	r3, r4, r7
    21c8:	4a000005 	bmi	21e4 <do_pads+0x6c>
    21cc:	e1a05004 	mov	r5, r4
    21d0:	e2866004 	add	r6, r6, #4	; 0x4
    21d4:	e59f9144 	ldr	r9, [pc, #324]	; 2320 <do_pads+0x1a8>
    21d8:	e59f8144 	ldr	r8, [pc, #324]	; 2324 <do_pads+0x1ac>
    21dc:	e59fa144 	ldr	sl, [pc, #324]	; 2328 <do_pads+0x1b0>
    21e0:	ea000049 	b	230c <do_pads+0x194>
    21e4:	e59f0140 	ldr	r0, [pc, #320]	; 232c <do_pads+0x1b4>
    21e8:	e59f3134 	ldr	r3, [pc, #308]	; 2324 <do_pads+0x1ac>
    21ec:	e1a0e00f 	mov	lr, pc
    21f0:	e593f000 	ldr	pc, [r3]
    21f4:	ea000046 	b	2314 <do_pads+0x19c>
    21f8:	e5960000 	ldr	r0, [r6]
    21fc:	e59f111c 	ldr	r1, [pc, #284]	; 2320 <do_pads+0x1a8>
    2200:	e3a02002 	mov	r2, #2	; 0x2
    2204:	ebfffc17 	bl	1268 <strncmp>
    2208:	e3500000 	cmp	r0, #0	; 0x0
    220c:	1a00000b 	bne	2240 <do_pads+0xc8>
    2210:	e1a02000 	mov	r2, r0
    2214:	e3a01001 	mov	r1, #1	; 0x1
    2218:	e1a00005 	mov	r0, r5
    221c:	ebfffa58 	bl	b84 <pads_chmod>
    2220:	e1a00005 	mov	r0, r5
    2224:	e5984000 	ldr	r4, [r8]
    2228:	ebfffad1 	bl	d74 <pads_getpin>
    222c:	e1a01005 	mov	r1, r5
    2230:	e1a02000 	mov	r2, r0
    2234:	e59f00f4 	ldr	r0, [pc, #244]	; 2330 <do_pads+0x1b8>
    2238:	e12fff34 	blx	r4
    223c:	ea000031 	b	2308 <do_pads+0x190>
    2240:	e59f10ec 	ldr	r1, [pc, #236]	; 2334 <do_pads+0x1bc>
    2244:	e3a02004 	mov	r2, #4	; 0x4
    2248:	e5960000 	ldr	r0, [r6]
    224c:	ebfffc05 	bl	1268 <strncmp>
    2250:	e3500000 	cmp	r0, #0	; 0x0
    2254:	01a00005 	moveq	r0, r5
    2258:	03a01002 	moveq	r1, #2	; 0x2
    225c:	03a02001 	moveq	r2, #1	; 0x1
    2260:	0a000011 	beq	22ac <do_pads+0x134>
    2264:	e59f10cc 	ldr	r1, [pc, #204]	; 2338 <do_pads+0x1c0>
    2268:	e3a02003 	mov	r2, #3	; 0x3
    226c:	e5960000 	ldr	r0, [r6]
    2270:	ebfffbfc 	bl	1268 <strncmp>
    2274:	e3500000 	cmp	r0, #0	; 0x0
    2278:	01a02000 	moveq	r2, r0
    227c:	03a01002 	moveq	r1, #2	; 0x2
    2280:	01a00005 	moveq	r0, r5
    2284:	0a000008 	beq	22ac <do_pads+0x134>
    2288:	e5960000 	ldr	r0, [r6]
    228c:	e59f10a8 	ldr	r1, [pc, #168]	; 233c <do_pads+0x1c4>
    2290:	e3a02004 	mov	r2, #4	; 0x4
    2294:	ebfffbf3 	bl	1268 <strncmp>
    2298:	e3500000 	cmp	r0, #0	; 0x0
    229c:	1a000004 	bne	22b4 <do_pads+0x13c>
    22a0:	e1a01000 	mov	r1, r0
    22a4:	e1a02001 	mov	r2, r1
    22a8:	e1a00005 	mov	r0, r5
    22ac:	ebfffa34 	bl	b84 <pads_chmod>
    22b0:	ea000014 	b	2308 <do_pads+0x190>
    22b4:	e5960000 	ldr	r0, [r6]
    22b8:	e59f1080 	ldr	r1, [pc, #128]	; 2340 <do_pads+0x1c8>
    22bc:	e3a02002 	mov	r2, #2	; 0x2
    22c0:	ebfffbe8 	bl	1268 <strncmp>
    22c4:	e3500000 	cmp	r0, #0	; 0x0
    22c8:	1a00000c 	bne	2300 <do_pads+0x188>
    22cc:	e1a00005 	mov	r0, r5
    22d0:	ebfffa15 	bl	b2c <pads_states>
    22d4:	e3500000 	cmp	r0, #0	; 0x0
    22d8:	e5983000 	ldr	r3, [r8]
    22dc:	059f2060 	ldreq	r2, [pc, #96]	; 2344 <do_pads+0x1cc>
    22e0:	0a000002 	beq	22f0 <do_pads+0x178>
    22e4:	e3500001 	cmp	r0, #1	; 0x1
    22e8:	01a02009 	moveq	r2, r9
    22ec:	11a0200a 	movne	r2, sl
    22f0:	e59f0050 	ldr	r0, [pc, #80]	; 2348 <do_pads+0x1d0>
    22f4:	e1a01005 	mov	r1, r5
    22f8:	e12fff33 	blx	r3
    22fc:	ea000001 	b	2308 <do_pads+0x190>
    2300:	e59f0044 	ldr	r0, [pc, #68]	; 234c <do_pads+0x1d4>
    2304:	eaffffb7 	b	21e8 <do_pads+0x70>
    2308:	e2855001 	add	r5, r5, #1	; 0x1
    230c:	e1550007 	cmp	r5, r7
    2310:	daffffb8 	ble	21f8 <do_pads+0x80>
    2314:	e3a00000 	mov	r0, #0	; 0x0
    2318:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    231c:	00011f82 	.word	0x00011f82
    2320:	00011fbf 	.word	0x00011fbf
    2324:	0800d76c 	.word	0x0800d76c
    2328:	00014338 	.word	0x00014338
    232c:	00011f9a 	.word	0x00011f9a
    2330:	00011fc2 	.word	0x00011fc2
    2334:	00011fd6 	.word	0x00011fd6
    2338:	00011fdb 	.word	0x00011fdb
    233c:	00011fdf 	.word	0x00011fdf
    2340:	00011fe4 	.word	0x00011fe4
    2344:	00011fe7 	.word	0x00011fe7
    2348:	00011ff0 	.word	0x00011ff0
    234c:	00012005 	.word	0x00012005

00002350 <do_mux>:
    2350:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2354:	e3520001 	cmp	r2, #1	; 0x1
    2358:	e24dd004 	sub	sp, sp, #4	; 0x4
    235c:	da000005 	ble	2378 <do_mux+0x28>
    2360:	e5930004 	ldr	r0, [r3, #4]
    2364:	e3a01000 	mov	r1, #0	; 0x0
    2368:	e3a0200a 	mov	r2, #10	; 0xa
    236c:	eb002b61 	bl	d0f8 <simple_strtoul>
    2370:	e6ff0070 	uxth	r0, r0
    2374:	ebfffb4d 	bl	10b0 <set_mux>
    2378:	e59f301c 	ldr	r3, [pc, #28]	; 239c <do_mux+0x4c>
    237c:	e59f001c 	ldr	r0, [pc, #28]	; 23a0 <do_mux+0x50>
    2380:	e5931000 	ldr	r1, [r3]
    2384:	e59f3018 	ldr	r3, [pc, #24]	; 23a4 <do_mux+0x54>
    2388:	e1a0e00f 	mov	lr, pc
    238c:	e593f000 	ldr	pc, [r3]
    2390:	e3a00000 	mov	r0, #0	; 0x0
    2394:	e28dd004 	add	sp, sp, #4	; 0x4
    2398:	e8bd8000 	pop	{pc}
    239c:	2d009000 	.word	0x2d009000
    23a0:	0001201e 	.word	0x0001201e
    23a4:	0800d76c 	.word	0x0800d76c

000023a8 <do_mac>:
    23a8:	e59f3020 	ldr	r3, [pc, #32]	; 23d0 <do_mac+0x28>
    23ac:	e92d4010 	push	{r4, lr}
    23b0:	e5934000 	ldr	r4, [r3]
    23b4:	ebfff96a 	bl	964 <efuse_mac>
    23b8:	e1a02000 	mov	r2, r0
    23bc:	e1a03001 	mov	r3, r1
    23c0:	e59f000c 	ldr	r0, [pc, #12]	; 23d4 <do_mac+0x2c>
    23c4:	e12fff34 	blx	r4
    23c8:	e3a00000 	mov	r0, #0	; 0x0
    23cc:	e8bd8010 	pop	{r4, pc}
    23d0:	0800d76c 	.word	0x0800d76c
    23d4:	00012030 	.word	0x00012030

000023d8 <do_boot>:
    23d8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    23dc:	e3a00000 	mov	r0, #0	; 0x0
    23e0:	e24dd004 	sub	sp, sp, #4	; 0x4
    23e4:	ebfff7ff 	bl	3e8 <try_boot>
    23e8:	e3a00000 	mov	r0, #0	; 0x0
    23ec:	e28dd004 	add	sp, sp, #4	; 0x4
    23f0:	e8bd8000 	pop	{pc}

000023f4 <do_efuse>:
    23f4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    23f8:	e3520001 	cmp	r2, #1	; 0x1
    23fc:	e24dd004 	sub	sp, sp, #4	; 0x4
    2400:	da000006 	ble	2420 <do_efuse+0x2c>
    2404:	e5930004 	ldr	r0, [r3, #4]
    2408:	e59f1024 	ldr	r1, [pc, #36]	; 2434 <do_efuse+0x40>
    240c:	e3a02002 	mov	r2, #2	; 0x2
    2410:	ebfffb94 	bl	1268 <strncmp>
    2414:	e3500000 	cmp	r0, #0	; 0x0
    2418:	02800001 	addeq	r0, r0, #1	; 0x1
    241c:	0a000000 	beq	2424 <do_efuse+0x30>
    2420:	e3a00000 	mov	r0, #0	; 0x0
    2424:	ebfff951 	bl	970 <efuse_info>
    2428:	e3a00000 	mov	r0, #0	; 0x0
    242c:	e28dd004 	add	sp, sp, #4	; 0x4
    2430:	e8bd8000 	pop	{pc}
    2434:	00012048 	.word	0x00012048

00002438 <do_pi>:
    2438:	e92d4030 	push	{r4, r5, lr}
    243c:	e3520001 	cmp	r2, #1	; 0x1
    2440:	e24dd004 	sub	sp, sp, #4	; 0x4
    2444:	e1a04002 	mov	r4, r2
    2448:	e1a05003 	mov	r5, r3
    244c:	da000015 	ble	24a8 <do_pi+0x70>
    2450:	e59f10ac 	ldr	r1, [pc, #172]	; 2504 <do_pi+0xcc>
    2454:	e5930004 	ldr	r0, [r3, #4]
    2458:	e3a02004 	mov	r2, #4	; 0x4
    245c:	ebfffb81 	bl	1268 <strncmp>
    2460:	e2501000 	subs	r1, r0, #0	; 0x0
    2464:	1a00000d 	bne	24a0 <do_pi+0x68>
    2468:	e3540002 	cmp	r4, #2	; 0x2
    246c:	01a04001 	moveq	r4, r1
    2470:	0a000003 	beq	2484 <do_pi+0x4c>
    2474:	e5950008 	ldr	r0, [r5, #8]
    2478:	e3a02010 	mov	r2, #16	; 0x10
    247c:	eb002b1d 	bl	d0f8 <simple_strtoul>
    2480:	e1a04000 	mov	r4, r0
    2484:	e59f007c 	ldr	r0, [pc, #124]	; 2508 <do_pi+0xd0>
    2488:	e1a01004 	mov	r1, r4
    248c:	e59f3078 	ldr	r3, [pc, #120]	; 250c <do_pi+0xd4>
    2490:	e1a0e00f 	mov	lr, pc
    2494:	e593f000 	ldr	pc, [r3]
    2498:	e1a00004 	mov	r0, r4
    249c:	eb001097 	bl	6700 <burn>
    24a0:	e3a04000 	mov	r4, #0	; 0x0
    24a4:	ea000013 	b	24f8 <do_pi+0xc0>
    24a8:	e3a00441 	mov	r0, #1090519040	; 0x41000000
    24ac:	eb002ea5 	bl	df48 <ius_check_header>
    24b0:	e2504000 	subs	r4, r0, #0	; 0x0
    24b4:	159f0054 	ldrne	r0, [pc, #84]	; 2510 <do_pi+0xd8>
    24b8:	1a000005 	bne	24d4 <do_pi+0x9c>
    24bc:	e3a0000d 	mov	r0, #13	; 0xd
    24c0:	e3a01001 	mov	r1, #1	; 0x1
    24c4:	eb000afb 	bl	50b8 <vs_assign_by_id>
    24c8:	e2504000 	subs	r4, r0, #0	; 0x0
    24cc:	0a000005 	beq	24e8 <do_pi+0xb0>
    24d0:	e59f003c 	ldr	r0, [pc, #60]	; 2514 <do_pi+0xdc>
    24d4:	e1a01004 	mov	r1, r4
    24d8:	e59f302c 	ldr	r3, [pc, #44]	; 250c <do_pi+0xd4>
    24dc:	e1a0e00f 	mov	lr, pc
    24e0:	e593f000 	ldr	pc, [r3]
    24e4:	ea000003 	b	24f8 <do_pi+0xc0>
    24e8:	e3a00441 	mov	r0, #1090519040	; 0x41000000
    24ec:	e3a0100d 	mov	r1, #13	; 0xd
    24f0:	e1a02004 	mov	r2, r4
    24f4:	eb001008 	bl	651c <burn_images>
    24f8:	e1a00004 	mov	r0, r4
    24fc:	e28dd004 	add	sp, sp, #4	; 0x4
    2500:	e8bd8030 	pop	{r4, r5, pc}
    2504:	0001204b 	.word	0x0001204b
    2508:	00012050 	.word	0x00012050
    250c:	0800d76c 	.word	0x0800d76c
    2510:	0001207a 	.word	0x0001207a
    2514:	0001209a 	.word	0x0001209a

00002518 <do_spbyte>:
    2518:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    251c:	e5930004 	ldr	r0, [r3, #4]
    2520:	e24dd004 	sub	sp, sp, #4	; 0x4
    2524:	e3a01000 	mov	r1, #0	; 0x0
    2528:	e3a0200a 	mov	r2, #10	; 0xa
    252c:	eb002af1 	bl	d0f8 <simple_strtoul>
    2530:	eb001b38 	bl	9218 <spi_set_bytes>
    2534:	e3a00000 	mov	r0, #0	; 0x0
    2538:	e28dd004 	add	sp, sp, #4	; 0x4
    253c:	e8bd8000 	pop	{pc}

00002540 <do_cfg>:
    2540:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2544:	e5930004 	ldr	r0, [r3, #4]
    2548:	e24dd004 	sub	sp, sp, #4	; 0x4
    254c:	e3a01000 	mov	r1, #0	; 0x0
    2550:	e3a02010 	mov	r2, #16	; 0x10
    2554:	eb002ae7 	bl	d0f8 <simple_strtoul>
    2558:	eb000f2a 	bl	6208 <cdbg_config_isi>
    255c:	e3a00000 	mov	r0, #0	; 0x0
    2560:	e28dd004 	add	sp, sp, #4	; 0x4
    2564:	e8bd8000 	pop	{pc}

00002568 <do_sleep>:
    2568:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    256c:	e3a01000 	mov	r1, #0	; 0x0
    2570:	e24dd004 	sub	sp, sp, #4	; 0x4
    2574:	e3a0200a 	mov	r2, #10	; 0xa
    2578:	e5930004 	ldr	r0, [r3, #4]
    257c:	eb002add 	bl	d0f8 <simple_strtoul>
    2580:	e59f3020 	ldr	r3, [pc, #32]	; 25a8 <do_sleep+0x40>
    2584:	e0000093 	mul	r0, r3, r0
    2588:	eb0018b2 	bl	8858 <udelay>
    258c:	e59f0018 	ldr	r0, [pc, #24]	; 25ac <do_sleep+0x44>
    2590:	e59f3018 	ldr	r3, [pc, #24]	; 25b0 <do_sleep+0x48>
    2594:	e1a0e00f 	mov	lr, pc
    2598:	e593f000 	ldr	pc, [r3]
    259c:	e3a00000 	mov	r0, #0	; 0x0
    25a0:	e28dd004 	add	sp, sp, #4	; 0x4
    25a4:	e8bd8000 	pop	{pc}
    25a8:	000f4240 	.word	0x000f4240
    25ac:	000120b5 	.word	0x000120b5
    25b0:	0800d76c 	.word	0x0800d76c

000025b4 <do_go>:
    25b4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    25b8:	e5930004 	ldr	r0, [r3, #4]
    25bc:	e24dd004 	sub	sp, sp, #4	; 0x4
    25c0:	e3a02010 	mov	r2, #16	; 0x10
    25c4:	e3a01000 	mov	r1, #0	; 0x0
    25c8:	eb002aca 	bl	d0f8 <simple_strtoul>
    25cc:	e3a01000 	mov	r1, #0	; 0x0
    25d0:	eb000e97 	bl	6034 <cdbg_jump>
    25d4:	e3a00000 	mov	r0, #0	; 0x0
    25d8:	e28dd004 	add	sp, sp, #4	; 0x4
    25dc:	e8bd8000 	pop	{pc}

000025e0 <do_ml>:
    25e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    25e4:	e24dd004 	sub	sp, sp, #4	; 0x4
    25e8:	ebfffc4d 	bl	1724 <__sOINFo>
    25ec:	e3a00000 	mov	r0, #0	; 0x0
    25f0:	e28dd004 	add	sp, sp, #4	; 0x4
    25f4:	e8bd8000 	pop	{pc}

000025f8 <do_bp>:
    25f8:	e92d4030 	push	{r4, r5, lr}
    25fc:	e3520001 	cmp	r2, #1	; 0x1
    2600:	e24dd004 	sub	sp, sp, #4	; 0x4
    2604:	da000005 	ble	2620 <do_bp+0x28>
    2608:	e5930004 	ldr	r0, [r3, #4]
    260c:	e3a01000 	mov	r1, #0	; 0x0
    2610:	e3a02010 	mov	r2, #16	; 0x10
    2614:	eb002ab7 	bl	d0f8 <simple_strtoul>
    2618:	e6ff0070 	uxth	r0, r0
    261c:	ebfff7d6 	bl	57c <set_xom>
    2620:	e59f5034 	ldr	r5, [pc, #52]	; 265c <do_bp+0x64>
    2624:	e3a00001 	mov	r0, #1	; 0x1
    2628:	e5954000 	ldr	r4, [r5]
    262c:	ebfff7ce 	bl	56c <get_xom>
    2630:	e1a01000 	mov	r1, r0
    2634:	e59f0024 	ldr	r0, [pc, #36]	; 2660 <do_bp+0x68>
    2638:	e12fff34 	blx	r4
    263c:	e5954000 	ldr	r4, [r5]
    2640:	ebfff7d4 	bl	598 <boot_info>
    2644:	e1a01000 	mov	r1, r0
    2648:	e59f0014 	ldr	r0, [pc, #20]	; 2664 <do_bp+0x6c>
    264c:	e12fff34 	blx	r4
    2650:	e3a00000 	mov	r0, #0	; 0x0
    2654:	e28dd004 	add	sp, sp, #4	; 0x4
    2658:	e8bd8030 	pop	{r4, r5, pc}
    265c:	0800d76c 	.word	0x0800d76c
    2660:	000120c0 	.word	0x000120c0
    2664:	000120d6 	.word	0x000120d6

00002668 <do_reset>:
    2668:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    266c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2670:	ebfff86a 	bl	820 <system_reset>
    2674:	e3a00000 	mov	r0, #0	; 0x0
    2678:	e28dd004 	add	sp, sp, #4	; 0x4
    267c:	e8bd8000 	pop	{pc}

00002680 <output_debug>:
    2680:	e24dd010 	sub	sp, sp, #16	; 0x10
    2684:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2688:	e24dd004 	sub	sp, sp, #4	; 0x4
    268c:	e28dc008 	add	ip, sp, #8	; 0x8
    2690:	e3700002 	cmn	r0, #2	; 0x2
    2694:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    2698:	e1a01000 	mov	r1, r0
    269c:	0a000012 	beq	26ec <output_debug+0x6c>
    26a0:	e3700001 	cmn	r0, #1	; 0x1
    26a4:	0a000008 	beq	26cc <output_debug+0x4c>
    26a8:	e3700003 	cmn	r0, #3	; 0x3
    26ac:	1a000010 	bne	26f4 <output_debug+0x74>
    26b0:	e59f30a4 	ldr	r3, [pc, #164]	; 275c <output_debug+0xdc>
    26b4:	e59f00a4 	ldr	r0, [pc, #164]	; 2760 <output_debug+0xe0>
    26b8:	e5931000 	ldr	r1, [r3]
    26bc:	e59f30a0 	ldr	r3, [pc, #160]	; 2764 <output_debug+0xe4>
    26c0:	e1a0e00f 	mov	lr, pc
    26c4:	e593f000 	ldr	pc, [r3]
    26c8:	ea00001e 	b	2748 <output_debug+0xc8>
    26cc:	e59f3088 	ldr	r3, [pc, #136]	; 275c <output_debug+0xdc>
    26d0:	e59f0090 	ldr	r0, [pc, #144]	; 2768 <output_debug+0xe8>
    26d4:	e5932000 	ldr	r2, [r3]
    26d8:	e28d1010 	add	r1, sp, #16	; 0x10
    26dc:	e59f3080 	ldr	r3, [pc, #128]	; 2764 <output_debug+0xe4>
    26e0:	e1a0e00f 	mov	lr, pc
    26e4:	e593f000 	ldr	pc, [r3]
    26e8:	ea000016 	b	2748 <output_debug+0xc8>
    26ec:	e59f0078 	ldr	r0, [pc, #120]	; 276c <output_debug+0xec>
    26f0:	ea000003 	b	2704 <output_debug+0x84>
    26f4:	e59d300c 	ldr	r3, [sp, #12]
    26f8:	e3530001 	cmp	r3, #1	; 0x1
    26fc:	1a000004 	bne	2714 <output_debug+0x94>
    2700:	e59f0068 	ldr	r0, [pc, #104]	; 2770 <output_debug+0xf0>
    2704:	e59f3058 	ldr	r3, [pc, #88]	; 2764 <output_debug+0xe4>
    2708:	e1a0e00f 	mov	lr, pc
    270c:	e593f000 	ldr	pc, [r3]
    2710:	ea00000c 	b	2748 <output_debug+0xc8>
    2714:	e3530002 	cmp	r3, #2	; 0x2
    2718:	e59fc044 	ldr	ip, [pc, #68]	; 2764 <output_debug+0xe4>
    271c:	1a000003 	bne	2730 <output_debug+0xb0>
    2720:	e59f004c 	ldr	r0, [pc, #76]	; 2774 <output_debug+0xf4>
    2724:	e1a0e00f 	mov	lr, pc
    2728:	e59cf000 	ldr	pc, [ip]
    272c:	ea000005 	b	2748 <output_debug+0xc8>
    2730:	e59f3024 	ldr	r3, [pc, #36]	; 275c <output_debug+0xdc>
    2734:	e59f003c 	ldr	r0, [pc, #60]	; 2778 <output_debug+0xf8>
    2738:	e5933000 	ldr	r3, [r3]
    273c:	e28d2010 	add	r2, sp, #16	; 0x10
    2740:	e1a0e00f 	mov	lr, pc
    2744:	e59cf000 	ldr	pc, [ip]
    2748:	e3a00000 	mov	r0, #0	; 0x0
    274c:	e28dd004 	add	sp, sp, #4	; 0x4
    2750:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    2754:	e28dd010 	add	sp, sp, #16	; 0x10
    2758:	e12fff1e 	bx	lr
    275c:	0800d1b4 	.word	0x0800d1b4
    2760:	0001225e 	.word	0x0001225e
    2764:	0800d76c 	.word	0x0800d76c
    2768:	00012286 	.word	0x00012286
    276c:	00012295 	.word	0x00012295
    2770:	000122af 	.word	0x000122af
    2774:	000122ba 	.word	0x000122ba
    2778:	000122c8 	.word	0x000122c8

0000277c <do_vs>:
    277c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    2780:	e3520006 	cmp	r2, #6	; 0x6
    2784:	e24dd024 	sub	sp, sp, #36	; 0x24
    2788:	e1a06000 	mov	r6, r0
    278c:	e1a04003 	mov	r4, r3
    2790:	1a000005 	bne	27ac <do_vs+0x30>
    2794:	e5930014 	ldr	r0, [r3, #20]
    2798:	e3a01000 	mov	r1, #0	; 0x0
    279c:	e282200a 	add	r2, r2, #10	; 0xa
    27a0:	eb002a54 	bl	d0f8 <simple_strtoul>
    27a4:	e1a05000 	mov	r5, r0
    27a8:	ea00008f 	b	29ec <do_vs+0x270>
    27ac:	e2423001 	sub	r3, r2, #1	; 0x1
    27b0:	e3530005 	cmp	r3, #5	; 0x5
    27b4:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    27b8:	ea0000c4 	b	2ad0 <do_vs+0x354>
    27bc:	00002ad0 	.word	0x00002ad0
    27c0:	000027d4 	.word	0x000027d4
    27c4:	000028cc 	.word	0x000028cc
    27c8:	0000293c 	.word	0x0000293c
    27cc:	000029e8 	.word	0x000029e8
    27d0:	000029e8 	.word	0x000029e8
    27d4:	e5940004 	ldr	r0, [r4, #4]
    27d8:	e59f1310 	ldr	r1, [pc, #784]	; 2af0 <do_vs+0x374>
    27dc:	ebfffa93 	bl	1230 <strcmp>
    27e0:	e2505000 	subs	r5, r0, #0	; 0x0
    27e4:	1a000025 	bne	2880 <do_vs+0x104>
    27e8:	e59f4304 	ldr	r4, [pc, #772]	; 2af4 <do_vs+0x378>
    27ec:	e59f0304 	ldr	r0, [pc, #772]	; 2af8 <do_vs+0x37c>
    27f0:	e1a0e00f 	mov	lr, pc
    27f4:	e594f000 	ldr	pc, [r4]
    27f8:	e3a0802c 	mov	r8, #44	; 0x2c
    27fc:	e59f72f8 	ldr	r7, [pc, #760]	; 2afc <do_vs+0x380>
    2800:	ea000003 	b	2814 <do_vs+0x98>
    2804:	e0227598 	mla	r2, r8, r5, r7
    2808:	e1a0e00f 	mov	lr, pc
    280c:	e594f000 	ldr	pc, [r4]
    2810:	e2855001 	add	r5, r5, #1	; 0x1
    2814:	eb000951 	bl	4d60 <vs_device_count>
    2818:	e1550000 	cmp	r5, r0
    281c:	e1a01005 	mov	r1, r5
    2820:	e59f02d8 	ldr	r0, [pc, #728]	; 2b00 <do_vs+0x384>
    2824:	e59f62c8 	ldr	r6, [pc, #712]	; 2af4 <do_vs+0x378>
    2828:	bafffff5 	blt	2804 <do_vs+0x88>
    282c:	e59f02d0 	ldr	r0, [pc, #720]	; 2b04 <do_vs+0x388>
    2830:	e1a0e00f 	mov	lr, pc
    2834:	e596f000 	ldr	pc, [r6]
    2838:	eb00084e 	bl	4978 <vs_is_assigned>
    283c:	e2504000 	subs	r4, r0, #0	; 0x0
    2840:	0a000009 	beq	286c <do_vs+0xf0>
    2844:	e59f32bc 	ldr	r3, [pc, #700]	; 2b08 <do_vs+0x38c>
    2848:	e5965000 	ldr	r5, [r6]
    284c:	e5934000 	ldr	r4, [r3]
    2850:	e1a00004 	mov	r0, r4
    2854:	eb0008c2 	bl	4b64 <vs_align>
    2858:	e1a01004 	mov	r1, r4
    285c:	e1a02000 	mov	r2, r0
    2860:	e59f02a4 	ldr	r0, [pc, #676]	; 2b0c <do_vs+0x390>
    2864:	e12fff35 	blx	r5
    2868:	ea00009d 	b	2ae4 <do_vs+0x368>
    286c:	e59f029c 	ldr	r0, [pc, #668]	; 2b10 <do_vs+0x394>
    2870:	e1a0e00f 	mov	lr, pc
    2874:	e596f000 	ldr	pc, [r6]
    2878:	e1a00004 	mov	r0, r4
    287c:	ea000099 	b	2ae8 <do_vs+0x36c>
    2880:	e5940004 	ldr	r0, [r4, #4]
    2884:	e59f1288 	ldr	r1, [pc, #648]	; 2b14 <do_vs+0x398>
    2888:	ebfffa68 	bl	1230 <strcmp>
    288c:	e2505000 	subs	r5, r0, #0	; 0x0
    2890:	1a000087 	bne	2ab4 <do_vs+0x338>
    2894:	eb0009f8 	bl	507c <vs_reset>
    2898:	e28d4020 	add	r4, sp, #32	; 0x20
    289c:	e5240018 	str	r0, [r4, #-24]!
    28a0:	e59f126c 	ldr	r1, [pc, #620]	; 2b14 <do_vs+0x398>
    28a4:	e2840008 	add	r0, r4, #8	; 0x8
    28a8:	ebfffa36 	bl	1188 <strcpy>
    28ac:	e3a03001 	mov	r3, #1	; 0x1
    28b0:	e58d300c 	str	r3, [sp, #12]
    28b4:	e28d3018 	add	r3, sp, #24	; 0x18
    28b8:	e8930003 	ldm	r3, {r0, r1}
    28bc:	e88d0003 	stm	sp, {r0, r1}
    28c0:	e894000f 	ldm	r4, {r0, r1, r2, r3}
    28c4:	ebffff6d 	bl	2680 <output_debug>
    28c8:	ea000019 	b	2934 <do_vs+0x1b8>
    28cc:	e5940004 	ldr	r0, [r4, #4]
    28d0:	e59f1240 	ldr	r1, [pc, #576]	; 2b18 <do_vs+0x39c>
    28d4:	ebfffa55 	bl	1230 <strcmp>
    28d8:	e2505000 	subs	r5, r0, #0	; 0x0
    28dc:	1a000074 	bne	2ab4 <do_vs+0x338>
    28e0:	e5940008 	ldr	r0, [r4, #8]
    28e4:	e3a01001 	mov	r1, #1	; 0x1
    28e8:	eb000a10 	bl	5130 <vs_assign_by_name>
    28ec:	e2507000 	subs	r7, r0, #0	; 0x0
    28f0:	e59f61fc 	ldr	r6, [pc, #508]	; 2af4 <do_vs+0x378>
    28f4:	1a00000b 	bne	2928 <do_vs+0x1ac>
    28f8:	e5943008 	ldr	r3, [r4, #8]
    28fc:	e59f2218 	ldr	r2, [pc, #536]	; 2b1c <do_vs+0x3a0>
    2900:	e1a00003 	mov	r0, r3
    2904:	e5823000 	str	r3, [r2]
    2908:	eb000916 	bl	4d68 <vs_device_id>
    290c:	e59f31f4 	ldr	r3, [pc, #500]	; 2b08 <do_vs+0x38c>
    2910:	e5830000 	str	r0, [r3]
    2914:	e59f0204 	ldr	r0, [pc, #516]	; 2b20 <do_vs+0x3a4>
    2918:	e1a0e00f 	mov	lr, pc
    291c:	e596f000 	ldr	pc, [r6]
    2920:	e1a00007 	mov	r0, r7
    2924:	ea00006f 	b	2ae8 <do_vs+0x36c>
    2928:	e59f01f4 	ldr	r0, [pc, #500]	; 2b24 <do_vs+0x3a8>
    292c:	e1a0e00f 	mov	lr, pc
    2930:	e596f000 	ldr	pc, [r6]
    2934:	e1a00005 	mov	r0, r5
    2938:	ea00006a 	b	2ae8 <do_vs+0x36c>
    293c:	e3a02010 	mov	r2, #16	; 0x10
    2940:	e5940008 	ldr	r0, [r4, #8]
    2944:	e3a01000 	mov	r1, #0	; 0x0
    2948:	eb002a42 	bl	d258 <simple_strtoull>
    294c:	e3a02010 	mov	r2, #16	; 0x10
    2950:	e1a07000 	mov	r7, r0
    2954:	e1a08001 	mov	r8, r1
    2958:	e594000c 	ldr	r0, [r4, #12]
    295c:	e3a01000 	mov	r1, #0	; 0x0
    2960:	eb002a3c 	bl	d258 <simple_strtoull>
    2964:	e59f11bc 	ldr	r1, [pc, #444]	; 2b28 <do_vs+0x3ac>
    2968:	e1a05000 	mov	r5, r0
    296c:	e28d0010 	add	r0, sp, #16	; 0x10
    2970:	ebfffa04 	bl	1188 <strcpy>
    2974:	e3a03002 	mov	r3, #2	; 0x2
    2978:	e5940004 	ldr	r0, [r4, #4]
    297c:	e59f11a4 	ldr	r1, [pc, #420]	; 2b28 <do_vs+0x3ac>
    2980:	e58d300c 	str	r3, [sp, #12]
    2984:	ebfffa29 	bl	1230 <strcmp>
    2988:	e3500000 	cmp	r0, #0	; 0x0
    298c:	1a000004 	bne	29a4 <do_vs+0x228>
    2990:	e1a00007 	mov	r0, r7
    2994:	e1a01008 	mov	r1, r8
    2998:	e1a02005 	mov	r2, r5
    299c:	eb000834 	bl	4a74 <vs_erase>
    29a0:	ea000008 	b	29c8 <do_vs+0x24c>
    29a4:	e5940004 	ldr	r0, [r4, #4]
    29a8:	e59f117c 	ldr	r1, [pc, #380]	; 2b2c <do_vs+0x3b0>
    29ac:	ebfffa1f 	bl	1230 <strcmp>
    29b0:	e3500000 	cmp	r0, #0	; 0x0
    29b4:	1a00003e 	bne	2ab4 <do_vs+0x338>
    29b8:	e1a00007 	mov	r0, r7
    29bc:	e1a01008 	mov	r1, r8
    29c0:	e1a02005 	mov	r2, r5
    29c4:	eb000848 	bl	4aec <vs_scrub>
    29c8:	e58d0008 	str	r0, [sp, #8]
    29cc:	e28d2018 	add	r2, sp, #24	; 0x18
    29d0:	e8920003 	ldm	r2, {r0, r1}
    29d4:	e88d0003 	stm	sp, {r0, r1}
    29d8:	e28d3008 	add	r3, sp, #8	; 0x8
    29dc:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    29e0:	ebffff26 	bl	2680 <output_debug>
    29e4:	ea00003e 	b	2ae4 <do_vs+0x368>
    29e8:	e3a05000 	mov	r5, #0	; 0x0
    29ec:	e3a01000 	mov	r1, #0	; 0x0
    29f0:	e3a02010 	mov	r2, #16	; 0x10
    29f4:	e5940008 	ldr	r0, [r4, #8]
    29f8:	eb0029be 	bl	d0f8 <simple_strtoul>
    29fc:	e3a02010 	mov	r2, #16	; 0x10
    2a00:	e1a09000 	mov	r9, r0
    2a04:	e3a01000 	mov	r1, #0	; 0x0
    2a08:	e594000c 	ldr	r0, [r4, #12]
    2a0c:	eb002a11 	bl	d258 <simple_strtoull>
    2a10:	e3a02010 	mov	r2, #16	; 0x10
    2a14:	e1a08000 	mov	r8, r0
    2a18:	e1a0a001 	mov	sl, r1
    2a1c:	e5940010 	ldr	r0, [r4, #16]
    2a20:	e3a01000 	mov	r1, #0	; 0x0
    2a24:	e08db002 	add	fp, sp, r2
    2a28:	eb002a0a 	bl	d258 <simple_strtoull>
    2a2c:	e3a03000 	mov	r3, #0	; 0x0
    2a30:	e59f10f8 	ldr	r1, [pc, #248]	; 2b30 <do_vs+0x3b4>
    2a34:	e1a07000 	mov	r7, r0
    2a38:	e1a0000b 	mov	r0, fp
    2a3c:	e58d300c 	str	r3, [sp, #12]
    2a40:	ebfff9d0 	bl	1188 <strcpy>
    2a44:	e5940004 	ldr	r0, [r4, #4]
    2a48:	e59f10e4 	ldr	r1, [pc, #228]	; 2b34 <do_vs+0x3b8>
    2a4c:	ebfff9f7 	bl	1230 <strcmp>
    2a50:	e3500000 	cmp	r0, #0	; 0x0
    2a54:	1a000006 	bne	2a74 <do_vs+0x2f8>
    2a58:	e1a00009 	mov	r0, r9
    2a5c:	e1a02008 	mov	r2, r8
    2a60:	e1a0300a 	mov	r3, sl
    2a64:	e58d7000 	str	r7, [sp]
    2a68:	e58d5004 	str	r5, [sp, #4]
    2a6c:	eb0007ea 	bl	4a1c <vs_write>
    2a70:	eaffffd4 	b	29c8 <do_vs+0x24c>
    2a74:	e5940004 	ldr	r0, [r4, #4]
    2a78:	e59f10b8 	ldr	r1, [pc, #184]	; 2b38 <do_vs+0x3bc>
    2a7c:	ebfff9eb 	bl	1230 <strcmp>
    2a80:	e3500000 	cmp	r0, #0	; 0x0
    2a84:	1a00000a 	bne	2ab4 <do_vs+0x338>
    2a88:	e1a00009 	mov	r0, r9
    2a8c:	e1a02008 	mov	r2, r8
    2a90:	e1a0300a 	mov	r3, sl
    2a94:	e58d7000 	str	r7, [sp]
    2a98:	e58d5004 	str	r5, [sp, #4]
    2a9c:	eb0008de 	bl	4e1c <vs_read>
    2aa0:	e59f1094 	ldr	r1, [pc, #148]	; 2b3c <do_vs+0x3c0>
    2aa4:	e58d0008 	str	r0, [sp, #8]
    2aa8:	e1a0000b 	mov	r0, fp
    2aac:	ebfff9b5 	bl	1188 <strcpy>
    2ab0:	eaffffc5 	b	29cc <do_vs+0x250>
    2ab4:	e59f0084 	ldr	r0, [pc, #132]	; 2b40 <do_vs+0x3c4>
    2ab8:	e5961014 	ldr	r1, [r6, #20]
    2abc:	e59f3030 	ldr	r3, [pc, #48]	; 2af4 <do_vs+0x378>
    2ac0:	e1a0e00f 	mov	lr, pc
    2ac4:	e593f000 	ldr	pc, [r3]
    2ac8:	e3e00000 	mvn	r0, #0	; 0x0
    2acc:	ea000005 	b	2ae8 <do_vs+0x36c>
    2ad0:	e5901014 	ldr	r1, [r0, #20]
    2ad4:	e59f3018 	ldr	r3, [pc, #24]	; 2af4 <do_vs+0x378>
    2ad8:	e59f0060 	ldr	r0, [pc, #96]	; 2b40 <do_vs+0x3c4>
    2adc:	e1a0e00f 	mov	lr, pc
    2ae0:	e593f000 	ldr	pc, [r3]
    2ae4:	e3a00000 	mov	r0, #0	; 0x0
    2ae8:	e28dd024 	add	sp, sp, #36	; 0x24
    2aec:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    2af0:	00012105 	.word	0x00012105
    2af4:	0800d76c 	.word	0x0800d76c
    2af8:	000122df 	.word	0x000122df
    2afc:	00010eec 	.word	0x00010eec
    2b00:	000122ed 	.word	0x000122ed
    2b04:	000122f5 	.word	0x000122f5
    2b08:	0800d1b4 	.word	0x0800d1b4
    2b0c:	00012307 	.word	0x00012307
    2b10:	00012311 	.word	0x00012311
    2b14:	000120f3 	.word	0x000120f3
    2b18:	0001410c 	.word	0x0001410c
    2b1c:	0800d1b8 	.word	0x0800d1b8
    2b20:	00012324 	.word	0x00012324
    2b24:	00012330 	.word	0x00012330
    2b28:	0001235d 	.word	0x0001235d
    2b2c:	00012363 	.word	0x00012363
    2b30:	00012369 	.word	0x00012369
    2b34:	00012372 	.word	0x00012372
    2b38:	00012378 	.word	0x00012378
    2b3c:	0001237d 	.word	0x0001237d
    2b40:	00011d13 	.word	0x00011d13

00002b44 <cmd_get_data_size>:
    2b44:	e92d4030 	push	{r4, r5, lr}
    2b48:	e24dd004 	sub	sp, sp, #4	; 0x4
    2b4c:	e1a04001 	mov	r4, r1
    2b50:	e1a05000 	mov	r5, r0
    2b54:	ebfff9e2 	bl	12e4 <strlen>
    2b58:	e3500002 	cmp	r0, #2	; 0x2
    2b5c:	da000015 	ble	2bb8 <cmd_get_data_size+0x74>
    2b60:	e0850000 	add	r0, r5, r0
    2b64:	e5503002 	ldrb	r3, [r0, #-2]
    2b68:	e353002e 	cmp	r3, #46	; 0x2e
    2b6c:	1a000011 	bne	2bb8 <cmd_get_data_size+0x74>
    2b70:	e5500001 	ldrb	r0, [r0, #-1]
    2b74:	e350006c 	cmp	r0, #108	; 0x6c
    2b78:	03a04004 	moveq	r4, #4	; 0x4
    2b7c:	0a00000d 	beq	2bb8 <cmd_get_data_size+0x74>
    2b80:	8a000002 	bhi	2b90 <cmd_get_data_size+0x4c>
    2b84:	e3500062 	cmp	r0, #98	; 0x62
    2b88:	1a000009 	bne	2bb4 <cmd_get_data_size+0x70>
    2b8c:	ea000006 	b	2bac <cmd_get_data_size+0x68>
    2b90:	e3500073 	cmp	r0, #115	; 0x73
    2b94:	03e04001 	mvneq	r4, #1	; 0x1
    2b98:	0a000006 	beq	2bb8 <cmd_get_data_size+0x74>
    2b9c:	e3500077 	cmp	r0, #119	; 0x77
    2ba0:	03a04002 	moveq	r4, #2	; 0x2
    2ba4:	1a000002 	bne	2bb4 <cmd_get_data_size+0x70>
    2ba8:	ea000002 	b	2bb8 <cmd_get_data_size+0x74>
    2bac:	e3a04001 	mov	r4, #1	; 0x1
    2bb0:	ea000000 	b	2bb8 <cmd_get_data_size+0x74>
    2bb4:	e3e04000 	mvn	r4, #0	; 0x0
    2bb8:	e1a00004 	mov	r0, r4
    2bbc:	e28dd004 	add	sp, sp, #4	; 0x4
    2bc0:	e8bd8030 	pop	{r4, r5, pc}

00002bc4 <cmd_usage>:
    2bc4:	e92d4030 	push	{r4, r5, lr}
    2bc8:	e1a04000 	mov	r4, r0
    2bcc:	e24dd004 	sub	sp, sp, #4	; 0x4
    2bd0:	e59f5054 	ldr	r5, [pc, #84]	; 2c2c <cmd_usage+0x68>
    2bd4:	e59f0054 	ldr	r0, [pc, #84]	; 2c30 <cmd_usage+0x6c>
    2bd8:	e5941000 	ldr	r1, [r4]
    2bdc:	e5942010 	ldr	r2, [r4, #16]
    2be0:	e1a0e00f 	mov	lr, pc
    2be4:	e595f000 	ldr	pc, [r5]
    2be8:	e59f0044 	ldr	r0, [pc, #68]	; 2c34 <cmd_usage+0x70>
    2bec:	e5941000 	ldr	r1, [r4]
    2bf0:	e1a0e00f 	mov	lr, pc
    2bf4:	e595f000 	ldr	pc, [r5]
    2bf8:	e5940014 	ldr	r0, [r4, #20]
    2bfc:	e3500000 	cmp	r0, #0	; 0x0
    2c00:	1a000003 	bne	2c14 <cmd_usage+0x50>
    2c04:	e59f002c 	ldr	r0, [pc, #44]	; 2c38 <cmd_usage+0x74>
    2c08:	eb002d28 	bl	e0b0 <puts>
    2c0c:	e3a00001 	mov	r0, #1	; 0x1
    2c10:	ea000003 	b	2c24 <cmd_usage+0x60>
    2c14:	eb002d25 	bl	e0b0 <puts>
    2c18:	e3a0000a 	mov	r0, #10	; 0xa
    2c1c:	eb002d4b 	bl	e150 <putc>
    2c20:	e3a00000 	mov	r0, #0	; 0x0
    2c24:	e28dd004 	add	sp, sp, #4	; 0x4
    2c28:	e8bd8030 	pop	{r4, r5, pc}
    2c2c:	0800d76c 	.word	0x0800d76c
    2c30:	00012472 	.word	0x00012472
    2c34:	0001247c 	.word	0x0001247c
    2c38:	00012487 	.word	0x00012487

00002c3c <find_cmd_tbl>:
    2c3c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    2c40:	e1a05001 	mov	r5, r1
    2c44:	e24dd004 	sub	sp, sp, #4	; 0x4
    2c48:	e3a0102e 	mov	r1, #46	; 0x2e
    2c4c:	e1a07000 	mov	r7, r0
    2c50:	e1a08002 	mov	r8, r2
    2c54:	ebfff997 	bl	12b8 <strchr>
    2c58:	e3500000 	cmp	r0, #0	; 0x0
    2c5c:	10676000 	rsbne	r6, r7, r0
    2c60:	1a000002 	bne	2c70 <find_cmd_tbl+0x34>
    2c64:	e1a00007 	mov	r0, r7
    2c68:	ebfff99d 	bl	12e4 <strlen>
    2c6c:	e1a06000 	mov	r6, r0
    2c70:	e3a03018 	mov	r3, #24	; 0x18
    2c74:	e02a5893 	mla	sl, r3, r8, r5
    2c78:	e1a04005 	mov	r4, r5
    2c7c:	e1a08005 	mov	r8, r5
    2c80:	e3a05000 	mov	r5, #0	; 0x0
    2c84:	ea00000a 	b	2cb4 <find_cmd_tbl+0x78>
    2c88:	e5941000 	ldr	r1, [r4]
    2c8c:	ebfff975 	bl	1268 <strncmp>
    2c90:	e3500000 	cmp	r0, #0	; 0x0
    2c94:	1a000005 	bne	2cb0 <find_cmd_tbl+0x74>
    2c98:	e5940000 	ldr	r0, [r4]
    2c9c:	ebfff990 	bl	12e4 <strlen>
    2ca0:	e1560000 	cmp	r6, r0
    2ca4:	0a000009 	beq	2cd0 <find_cmd_tbl+0x94>
    2ca8:	e2855001 	add	r5, r5, #1	; 0x1
    2cac:	e1a08004 	mov	r8, r4
    2cb0:	e2844018 	add	r4, r4, #24	; 0x18
    2cb4:	e154000a 	cmp	r4, sl
    2cb8:	e1a02006 	mov	r2, r6
    2cbc:	e1a00007 	mov	r0, r7
    2cc0:	1afffff0 	bne	2c88 <find_cmd_tbl+0x4c>
    2cc4:	e3550001 	cmp	r5, #1	; 0x1
    2cc8:	01a04008 	moveq	r4, r8
    2ccc:	13a04000 	movne	r4, #0	; 0x0
    2cd0:	e1a00004 	mov	r0, r4
    2cd4:	e28dd004 	add	sp, sp, #4	; 0x4
    2cd8:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00002cdc <find_cmd>:
    2cdc:	e59f1014 	ldr	r1, [pc, #20]	; 2cf8 <find_cmd+0x1c>
    2ce0:	e59f2014 	ldr	r2, [pc, #20]	; 2cfc <find_cmd+0x20>
    2ce4:	e59f3014 	ldr	r3, [pc, #20]	; 2d00 <find_cmd+0x24>
    2ce8:	e0612002 	rsb	r2, r1, r2
    2cec:	e1a021c2 	asr	r2, r2, #3
    2cf0:	e0020293 	mul	r2, r3, r2
    2cf4:	eaffffd0 	b	2c3c <find_cmd_tbl>
    2cf8:	0001447c 	.word	0x0001447c
    2cfc:	0001474c 	.word	0x0001474c
    2d00:	aaaaaaab 	.word	0xaaaaaaab

00002d04 <_do_help>:
    2d04:	e1a0c00d 	mov	ip, sp
    2d08:	e92ddff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}
    2d0c:	e24cb004 	sub	fp, ip, #4	; 0x4
    2d10:	e24dd004 	sub	sp, sp, #4	; 0x4
    2d14:	e59b8004 	ldr	r8, [fp, #4]
    2d18:	e1a07000 	mov	r7, r0
    2d1c:	e3580001 	cmp	r8, #1	; 0x1
    2d20:	e1a0a001 	mov	sl, r1
    2d24:	159b5008 	ldrne	r5, [fp, #8]
    2d28:	13a06001 	movne	r6, #1	; 0x1
    2d2c:	13a04000 	movne	r4, #0	; 0x0
    2d30:	159f9158 	ldrne	r9, [pc, #344]	; 2e90 <_do_help+0x18c>
    2d34:	1a00004e 	bne	2e74 <_do_help+0x170>
    2d38:	e1a03101 	lsl	r3, r1, #2
    2d3c:	e283300e 	add	r3, r3, #14	; 0xe
    2d40:	e3c33007 	bic	r3, r3, #7	; 0x7
    2d44:	e1a0900d 	mov	r9, sp
    2d48:	e063d00d 	rsb	sp, r3, sp
    2d4c:	e1a0800d 	mov	r8, sp
    2d50:	e3a03000 	mov	r3, #0	; 0x0
    2d54:	ea000002 	b	2d64 <_do_help+0x60>
    2d58:	e7880103 	str	r0, [r8, r3, lsl #2]
    2d5c:	e2800018 	add	r0, r0, #24	; 0x18
    2d60:	e2833001 	add	r3, r3, #1	; 0x1
    2d64:	e153000a 	cmp	r3, sl
    2d68:	bafffffa 	blt	2d58 <_do_help+0x54>
    2d6c:	e24a7001 	sub	r7, sl, #1	; 0x1
    2d70:	ea000015 	b	2dcc <_do_help+0xc8>
    2d74:	e5943000 	ldr	r3, [r4]
    2d78:	e5942004 	ldr	r2, [r4, #4]
    2d7c:	e5930000 	ldr	r0, [r3]
    2d80:	e5921000 	ldr	r1, [r2]
    2d84:	ebfff929 	bl	1230 <strcmp>
    2d88:	e3500000 	cmp	r0, #0	; 0x0
    2d8c:	c5942000 	ldrgt	r2, [r4]
    2d90:	c5943004 	ldrgt	r3, [r4, #4]
    2d94:	e2855001 	add	r5, r5, #1	; 0x1
    2d98:	c5843000 	strgt	r3, [r4]
    2d9c:	c5842004 	strgt	r2, [r4, #4]
    2da0:	c2866001 	addgt	r6, r6, #1	; 0x1
    2da4:	e1550007 	cmp	r5, r7
    2da8:	e2844004 	add	r4, r4, #4	; 0x4
    2dac:	bafffff0 	blt	2d74 <_do_help+0x70>
    2db0:	e3560000 	cmp	r6, #0	; 0x0
    2db4:	1a000003 	bne	2dc8 <_do_help+0xc4>
    2db8:	e3a05000 	mov	r5, #0	; 0x0
    2dbc:	e1a04005 	mov	r4, r5
    2dc0:	e59f70c8 	ldr	r7, [pc, #200]	; 2e90 <_do_help+0x18c>
    2dc4:	ea000016 	b	2e24 <_do_help+0x120>
    2dc8:	e2477001 	sub	r7, r7, #1	; 0x1
    2dcc:	e3570000 	cmp	r7, #0	; 0x0
    2dd0:	dafffff8 	ble	2db8 <_do_help+0xb4>
    2dd4:	e3a05000 	mov	r5, #0	; 0x0
    2dd8:	e1a04008 	mov	r4, r8
    2ddc:	e1a06005 	mov	r6, r5
    2de0:	eaffffe3 	b	2d74 <_do_help+0x70>
    2de4:	e7983004 	ldr	r3, [r8, r4]
    2de8:	e5936010 	ldr	r6, [r3, #16]
    2dec:	eb002cda 	bl	e15c <ctrlc>
    2df0:	e3500000 	cmp	r0, #0	; 0x0
    2df4:	e1a03006 	mov	r3, r6
    2df8:	e3a01008 	mov	r1, #8	; 0x8
    2dfc:	e59f0090 	ldr	r0, [pc, #144]	; 2e94 <_do_help+0x190>
    2e00:	13a04001 	movne	r4, #1	; 0x1
    2e04:	1a00000a 	bne	2e34 <_do_help+0x130>
    2e08:	e3560000 	cmp	r6, #0	; 0x0
    2e0c:	0a000003 	beq	2e20 <_do_help+0x11c>
    2e10:	e7982004 	ldr	r2, [r8, r4]
    2e14:	e5922000 	ldr	r2, [r2]
    2e18:	e1a0e00f 	mov	lr, pc
    2e1c:	e597f000 	ldr	pc, [r7]
    2e20:	e2844004 	add	r4, r4, #4	; 0x4
    2e24:	e155000a 	cmp	r5, sl
    2e28:	e2855001 	add	r5, r5, #1	; 0x1
    2e2c:	baffffec 	blt	2de4 <_do_help+0xe0>
    2e30:	e3a04000 	mov	r4, #0	; 0x0
    2e34:	e1a0d009 	mov	sp, r9
    2e38:	ea000011 	b	2e84 <_do_help+0x180>
    2e3c:	e5950004 	ldr	r0, [r5, #4]
    2e40:	ebffff7d 	bl	2c3c <find_cmd_tbl>
    2e44:	e3500000 	cmp	r0, #0	; 0x0
    2e48:	0a000002 	beq	2e58 <_do_help+0x154>
    2e4c:	ebffff5c 	bl	2bc4 <cmd_usage>
    2e50:	e1844000 	orr	r4, r4, r0
    2e54:	ea000004 	b	2e6c <_do_help+0x168>
    2e58:	e5951004 	ldr	r1, [r5, #4]
    2e5c:	e59f0034 	ldr	r0, [pc, #52]	; 2e98 <_do_help+0x194>
    2e60:	e1a0e00f 	mov	lr, pc
    2e64:	e599f000 	ldr	pc, [r9]
    2e68:	e3a04001 	mov	r4, #1	; 0x1
    2e6c:	e2866001 	add	r6, r6, #1	; 0x1
    2e70:	e2855004 	add	r5, r5, #4	; 0x4
    2e74:	e1560008 	cmp	r6, r8
    2e78:	e1a01007 	mov	r1, r7
    2e7c:	e1a0200a 	mov	r2, sl
    2e80:	baffffed 	blt	2e3c <_do_help+0x138>
    2e84:	e1a00004 	mov	r0, r4
    2e88:	e24bd028 	sub	sp, fp, #40	; 0x28
    2e8c:	e89daff0 	ldm	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
    2e90:	0800d76c 	.word	0x0800d76c
    2e94:	000124a8 	.word	0x000124a8
    2e98:	000124b2 	.word	0x000124b2

00002e9c <do_help>:
    2e9c:	e92d4030 	push	{r4, r5, lr}
    2ea0:	e59fc038 	ldr	ip, [pc, #56]	; 2ee0 <do_help+0x44>
    2ea4:	e59fe038 	ldr	lr, [pc, #56]	; 2ee4 <do_help+0x48>
    2ea8:	e1a05001 	mov	r5, r1
    2eac:	e06ec00c 	rsb	ip, lr, ip
    2eb0:	e59f1030 	ldr	r1, [pc, #48]	; 2ee8 <do_help+0x4c>
    2eb4:	e24dd00c 	sub	sp, sp, #12	; 0xc
    2eb8:	e1a04000 	mov	r4, r0
    2ebc:	e1a0c1cc 	asr	ip, ip, #3
    2ec0:	e88d000c 	stm	sp, {r2, r3}
    2ec4:	e0010c91 	mul	r1, r1, ip
    2ec8:	e1a0000e 	mov	r0, lr
    2ecc:	e1a02004 	mov	r2, r4
    2ed0:	e1a03005 	mov	r3, r5
    2ed4:	ebffff8a 	bl	2d04 <_do_help>
    2ed8:	e28dd00c 	add	sp, sp, #12	; 0xc
    2edc:	e8bd8030 	pop	{r4, r5, pc}
    2ee0:	0001474c 	.word	0x0001474c
    2ee4:	0001447c 	.word	0x0001447c
    2ee8:	aaaaaaab 	.word	0xaaaaaaab

00002eec <tt_blk_trolling>:
    2eec:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    2ef0:	e3a01001 	mov	r1, #1	; 0x1
    2ef4:	e24dd02c 	sub	sp, sp, #44	; 0x2c
    2ef8:	e1a04002 	mov	r4, r2
    2efc:	e1a05003 	mov	r5, r3
    2f00:	e58d0010 	str	r0, [sp, #16]
    2f04:	e59da050 	ldr	sl, [sp, #80]
    2f08:	eb000888 	bl	5130 <vs_assign_by_name>
    2f0c:	e3500000 	cmp	r0, #0	; 0x0
    2f10:	0a000005 	beq	2f2c <tt_blk_trolling+0x40>
    2f14:	e59d1010 	ldr	r1, [sp, #16]
    2f18:	e59f03d4 	ldr	r0, [pc, #980]	; 32f4 <tt_blk_trolling+0x408>
    2f1c:	e59f33d4 	ldr	r3, [pc, #980]	; 32f8 <tt_blk_trolling+0x40c>
    2f20:	e1a0e00f 	mov	lr, pc
    2f24:	e593f000 	ldr	pc, [r3]
    2f28:	ea000026 	b	2fc8 <tt_blk_trolling+0xdc>
    2f2c:	e1940005 	orrs	r0, r4, r5
    2f30:	03a04502 	moveq	r4, #8388608	; 0x800000
    2f34:	03a06502 	moveq	r6, #8388608	; 0x800000
    2f38:	03a07000 	moveq	r7, #0	; 0x0
    2f3c:	0a00000b 	beq	2f70 <tt_blk_trolling+0x84>
    2f40:	e3550000 	cmp	r5, #0	; 0x0
    2f44:	1a000006 	bne	2f64 <tt_blk_trolling+0x78>
    2f48:	e3540502 	cmp	r4, #8388608	; 0x800000
    2f4c:	2a000004 	bcs	2f64 <tt_blk_trolling+0x78>
    2f50:	e3a0000e 	mov	r0, #14	; 0xe
    2f54:	eb000702 	bl	4b64 <vs_align>
    2f58:	e1a06004 	mov	r6, r4
    2f5c:	e1a07005 	mov	r7, r5
    2f60:	ea000002 	b	2f70 <tt_blk_trolling+0x84>
    2f64:	e1a06004 	mov	r6, r4
    2f68:	e1a07005 	mov	r7, r5
    2f6c:	e3a04502 	mov	r4, #8388608	; 0x800000
    2f70:	e3a0000e 	mov	r0, #14	; 0xe
    2f74:	eb0006fa 	bl	4b64 <vs_align>
    2f78:	e1a01000 	mov	r1, r0
    2f7c:	e1a00004 	mov	r0, r4
    2f80:	eb003460 	bl	10108 <__aeabi_uidiv>
    2f84:	e1a01000 	mov	r1, r0
    2f88:	e3a00000 	mov	r0, #0	; 0x0
    2f8c:	eb000514 	bl	43e4 <tt_rnd_get_int>
    2f90:	e1a04000 	mov	r4, r0
    2f94:	e3a0000e 	mov	r0, #14	; 0xe
    2f98:	eb0006f1 	bl	4b64 <vs_align>
    2f9c:	e0040094 	mul	r4, r4, r0
    2fa0:	e28d0024 	add	r0, sp, #36	; 0x24
    2fa4:	e1a01004 	mov	r1, r4
    2fa8:	e1a02004 	mov	r2, r4
    2fac:	eb000535 	bl	4488 <tt_rnd_get_data>
    2fb0:	e250b000 	subs	fp, r0, #0	; 0x0
    2fb4:	1a000005 	bne	2fd0 <tt_blk_trolling+0xe4>
    2fb8:	e59f033c 	ldr	r0, [pc, #828]	; 32fc <tt_blk_trolling+0x410>
    2fbc:	e59f3334 	ldr	r3, [pc, #820]	; 32f8 <tt_blk_trolling+0x40c>
    2fc0:	e1a0e00f 	mov	lr, pc
    2fc4:	e593f000 	ldr	pc, [r3]
    2fc8:	e3e00000 	mvn	r0, #0	; 0x0
    2fcc:	ea0000c6 	b	32ec <tt_blk_trolling+0x400>
    2fd0:	e59d3024 	ldr	r3, [sp, #36]
    2fd4:	e3a0000e 	mov	r0, #14	; 0xe
    2fd8:	e0564003 	subs	r4, r6, r3
    2fdc:	e0c75fc3 	sbc	r5, r7, r3, asr #31
    2fe0:	eb0006df 	bl	4b64 <vs_align>
    2fe4:	e3550000 	cmp	r5, #0	; 0x0
    2fe8:	e1cd41f8 	strd	r4, [sp, #24]
    2fec:	1a000005 	bne	3008 <tt_blk_trolling+0x11c>
    2ff0:	e1a01000 	mov	r1, r0
    2ff4:	e1a00004 	mov	r0, r4
    2ff8:	eb003442 	bl	10108 <__aeabi_uidiv>
    2ffc:	e58d501c 	str	r5, [sp, #28]
    3000:	e58d0018 	str	r0, [sp, #24]
    3004:	ea000002 	b	3014 <tt_blk_trolling+0x128>
    3008:	e1a01000 	mov	r1, r0
    300c:	e28d0018 	add	r0, sp, #24	; 0x18
    3010:	eb0027fb 	bl	d004 <__div64_32>
    3014:	e59d1018 	ldr	r1, [sp, #24]
    3018:	e3a00000 	mov	r0, #0	; 0x0
    301c:	eb0004f0 	bl	43e4 <tt_rnd_get_int>
    3020:	e1a04000 	mov	r4, r0
    3024:	e3a0000e 	mov	r0, #14	; 0xe
    3028:	eb0006cd 	bl	4b64 <vs_align>
    302c:	e0c98490 	smull	r8, r9, r0, r4
    3030:	e59d1010 	ldr	r1, [sp, #16]
    3034:	e1a0200b 	mov	r2, fp
    3038:	e59d3024 	ldr	r3, [sp, #36]
    303c:	e59f02bc 	ldr	r0, [pc, #700]	; 3300 <tt_blk_trolling+0x414>
    3040:	e88d0300 	stm	sp, {r8, r9}
    3044:	e59f42ac 	ldr	r4, [pc, #684]	; 32f8 <tt_blk_trolling+0x40c>
    3048:	e1a0e00f 	mov	lr, pc
    304c:	e594f000 	ldr	pc, [r4]
    3050:	e59d0010 	ldr	r0, [sp, #16]
    3054:	eb000743 	bl	4d68 <vs_device_id>
    3058:	eb00072e 	bl	4d18 <vs_device_erasable>
    305c:	e3500000 	cmp	r0, #0	; 0x0
    3060:	0a000006 	beq	3080 <tt_blk_trolling+0x194>
    3064:	e59f0298 	ldr	r0, [pc, #664]	; 3304 <tt_blk_trolling+0x418>
    3068:	e1a0e00f 	mov	lr, pc
    306c:	e594f000 	ldr	pc, [r4]
    3070:	e1a00008 	mov	r0, r8
    3074:	e1a01009 	mov	r1, r9
    3078:	e59d2024 	ldr	r2, [sp, #36]
    307c:	eb00067c 	bl	4a74 <vs_erase>
    3080:	e59f0280 	ldr	r0, [pc, #640]	; 3308 <tt_blk_trolling+0x41c>
    3084:	e59f326c 	ldr	r3, [pc, #620]	; 32f8 <tt_blk_trolling+0x40c>
    3088:	e1a0e00f 	mov	lr, pc
    308c:	e593f000 	ldr	pc, [r3]
    3090:	e35a0000 	cmp	sl, #0	; 0x0
    3094:	13a07000 	movne	r7, #0	; 0x0
    3098:	159d6024 	ldrne	r6, [sp, #36]
    309c:	1a00001c 	bne	3114 <tt_blk_trolling+0x228>
    30a0:	ea00001f 	b	3124 <tt_blk_trolling+0x238>
    30a4:	e3560902 	cmp	r6, #32768	; 0x8000
    30a8:	b1a01006 	movlt	r1, r6
    30ac:	a3a01902 	movge	r1, #32768	; 0x8000
    30b0:	e58d100c 	str	r1, [sp, #12]
    30b4:	e59d1024 	ldr	r1, [sp, #36]
    30b8:	e59d200c 	ldr	r2, [sp, #12]
    30bc:	e0661001 	rsb	r1, r6, r1
    30c0:	e08b1001 	add	r1, fp, r1
    30c4:	ebfff8a0 	bl	134c <memcpy>
    30c8:	e3a00000 	mov	r0, #0	; 0x0
    30cc:	eb001605 	bl	88e8 <get_timer>
    30d0:	e59d3024 	ldr	r3, [sp, #36]
    30d4:	e3a01000 	mov	r1, #0	; 0x0
    30d8:	e0663003 	rsb	r3, r6, r3
    30dc:	e0984003 	adds	r4, r8, r3
    30e0:	e0a95fc3 	adc	r5, r9, r3, asr #31
    30e4:	e1a03005 	mov	r3, r5
    30e8:	e59d500c 	ldr	r5, [sp, #12]
    30ec:	e58d0008 	str	r0, [sp, #8]
    30f0:	e1a02004 	mov	r2, r4
    30f4:	e1a0000a 	mov	r0, sl
    30f8:	e58d1004 	str	r1, [sp, #4]
    30fc:	e58d5000 	str	r5, [sp]
    3100:	eb000645 	bl	4a1c <vs_write>
    3104:	e59d0008 	ldr	r0, [sp, #8]
    3108:	eb0015f6 	bl	88e8 <get_timer>
    310c:	e0656006 	rsb	r6, r5, r6
    3110:	e0877000 	add	r7, r7, r0
    3114:	e3560000 	cmp	r6, #0	; 0x0
    3118:	e1a0000a 	mov	r0, sl
    311c:	1affffe0 	bne	30a4 <tt_blk_trolling+0x1b8>
    3120:	ea00000b 	b	3154 <tt_blk_trolling+0x268>
    3124:	e1a0000a 	mov	r0, sl
    3128:	eb0015ee 	bl	88e8 <get_timer>
    312c:	e59d1024 	ldr	r1, [sp, #36]
    3130:	e1a04000 	mov	r4, r0
    3134:	e1a02008 	mov	r2, r8
    3138:	e1a03009 	mov	r3, r9
    313c:	e1a0000b 	mov	r0, fp
    3140:	e88d0402 	stm	sp, {r1, sl}
    3144:	eb000634 	bl	4a1c <vs_write>
    3148:	e1a00004 	mov	r0, r4
    314c:	eb0015e5 	bl	88e8 <get_timer>
    3150:	e1a07000 	mov	r7, r0
    3154:	e59d0024 	ldr	r0, [sp, #36]
    3158:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    315c:	e1a00540 	asr	r0, r0, #10
    3160:	e1a01007 	mov	r1, r7
    3164:	e0000093 	mul	r0, r3, r0
    3168:	eb0033e6 	bl	10108 <__aeabi_uidiv>
    316c:	e59f3184 	ldr	r3, [pc, #388]	; 32f8 <tt_blk_trolling+0x40c>
    3170:	e58d0014 	str	r0, [sp, #20]
    3174:	e59f0190 	ldr	r0, [pc, #400]	; 330c <tt_blk_trolling+0x420>
    3178:	e1a0e00f 	mov	lr, pc
    317c:	e593f000 	ldr	pc, [r3]
    3180:	e35a0000 	cmp	sl, #0	; 0x0
    3184:	13a07000 	movne	r7, #0	; 0x0
    3188:	159d6024 	ldrne	r6, [sp, #36]
    318c:	1a00001b 	bne	3200 <tt_blk_trolling+0x314>
    3190:	ea00001e 	b	3210 <tt_blk_trolling+0x324>
    3194:	eb0015d3 	bl	88e8 <get_timer>
    3198:	e59d3024 	ldr	r3, [sp, #36]
    319c:	e3560902 	cmp	r6, #32768	; 0x8000
    31a0:	b1a04006 	movlt	r4, r6
    31a4:	a3a04902 	movge	r4, #32768	; 0x8000
    31a8:	e0663003 	rsb	r3, r6, r3
    31ac:	e1a05000 	mov	r5, r0
    31b0:	e0980003 	adds	r0, r8, r3
    31b4:	e0a91fc3 	adc	r1, r9, r3, asr #31
    31b8:	e1a02000 	mov	r2, r0
    31bc:	e1a03001 	mov	r3, r1
    31c0:	e1a0000a 	mov	r0, sl
    31c4:	e3a01000 	mov	r1, #0	; 0x0
    31c8:	e58d1004 	str	r1, [sp, #4]
    31cc:	e58d4000 	str	r4, [sp]
    31d0:	eb000711 	bl	4e1c <vs_read>
    31d4:	e1a00005 	mov	r0, r5
    31d8:	eb0015c2 	bl	88e8 <get_timer>
    31dc:	e1a05000 	mov	r5, r0
    31e0:	e59d0024 	ldr	r0, [sp, #36]
    31e4:	e1a0100a 	mov	r1, sl
    31e8:	e0660000 	rsb	r0, r6, r0
    31ec:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    31f0:	e1a02004 	mov	r2, r4
    31f4:	ebfff854 	bl	134c <memcpy>
    31f8:	e0877005 	add	r7, r7, r5
    31fc:	e0646006 	rsb	r6, r4, r6
    3200:	e3a00000 	mov	r0, #0	; 0x0
    3204:	e1560000 	cmp	r6, r0
    3208:	1affffe1 	bne	3194 <tt_blk_trolling+0x2a8>
    320c:	ea00000b 	b	3240 <tt_blk_trolling+0x354>
    3210:	e1a0000a 	mov	r0, sl
    3214:	eb0015b3 	bl	88e8 <get_timer>
    3218:	e59d1024 	ldr	r1, [sp, #36]
    321c:	e1a04000 	mov	r4, r0
    3220:	e1a02008 	mov	r2, r8
    3224:	e1a03009 	mov	r3, r9
    3228:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    322c:	e88d0402 	stm	sp, {r1, sl}
    3230:	eb0006f9 	bl	4e1c <vs_read>
    3234:	e1a00004 	mov	r0, r4
    3238:	eb0015aa 	bl	88e8 <get_timer>
    323c:	e1a07000 	mov	r7, r0
    3240:	e59d0024 	ldr	r0, [sp, #36]
    3244:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    3248:	e1a00540 	asr	r0, r0, #10
    324c:	e0000093 	mul	r0, r3, r0
    3250:	e1a01007 	mov	r1, r7
    3254:	eb0033ab 	bl	10108 <__aeabi_uidiv>
    3258:	e59f4098 	ldr	r4, [pc, #152]	; 32f8 <tt_blk_trolling+0x40c>
    325c:	e1a05000 	mov	r5, r0
    3260:	e59f00a8 	ldr	r0, [pc, #168]	; 3310 <tt_blk_trolling+0x424>
    3264:	e1a0e00f 	mov	lr, pc
    3268:	e594f000 	ldr	pc, [r4]
    326c:	e1a0000b 	mov	r0, fp
    3270:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    3274:	e59d2024 	ldr	r2, [sp, #36]
    3278:	e3a0300c 	mov	r3, #12	; 0xc
    327c:	eb000404 	bl	4294 <tt_cmp_buffer>
    3280:	e3500000 	cmp	r0, #0	; 0x0
    3284:	0a000007 	beq	32a8 <tt_blk_trolling+0x3bc>
    3288:	e58d0000 	str	r0, [sp]
    328c:	e59d1010 	ldr	r1, [sp, #16]
    3290:	e59d2014 	ldr	r2, [sp, #20]
    3294:	e1a03005 	mov	r3, r5
    3298:	e59f0074 	ldr	r0, [pc, #116]	; 3314 <tt_blk_trolling+0x428>
    329c:	e1a0e00f 	mov	lr, pc
    32a0:	e594f000 	ldr	pc, [r4]
    32a4:	ea000005 	b	32c0 <tt_blk_trolling+0x3d4>
    32a8:	e59d1010 	ldr	r1, [sp, #16]
    32ac:	e59d2014 	ldr	r2, [sp, #20]
    32b0:	e1a03005 	mov	r3, r5
    32b4:	e59f005c 	ldr	r0, [pc, #92]	; 3318 <tt_blk_trolling+0x42c>
    32b8:	e1a0e00f 	mov	lr, pc
    32bc:	e594f000 	ldr	pc, [r4]
    32c0:	e59f0054 	ldr	r0, [pc, #84]	; 331c <tt_blk_trolling+0x430>
    32c4:	e59d1024 	ldr	r1, [sp, #36]
    32c8:	e890000c 	ldm	r0, {r2, r3}
    32cc:	e0922001 	adds	r2, r2, r1
    32d0:	e0a33fc1 	adc	r3, r3, r1, asr #31
    32d4:	e880000c 	stm	r0, {r2, r3}
    32d8:	e59f0040 	ldr	r0, [pc, #64]	; 3320 <tt_blk_trolling+0x434>
    32dc:	e59f3014 	ldr	r3, [pc, #20]	; 32f8 <tt_blk_trolling+0x40c>
    32e0:	e1a0e00f 	mov	lr, pc
    32e4:	e593f000 	ldr	pc, [r3]
    32e8:	e3a00000 	mov	r0, #0	; 0x0
    32ec:	e28dd02c 	add	sp, sp, #44	; 0x2c
    32f0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    32f4:	00012659 	.word	0x00012659
    32f8:	0800d76c 	.word	0x0800d76c
    32fc:	00012677 	.word	0x00012677
    3300:	00012697 	.word	0x00012697
    3304:	00014001 	.word	0x00014001
    3308:	00011e99 	.word	0x00011e99
    330c:	00014388 	.word	0x00014388
    3310:	000126cb 	.word	0x000126cb
    3314:	000126ce 	.word	0x000126ce
    3318:	00012708 	.word	0x00012708
    331c:	0800d1c0 	.word	0x0800d1c0
    3320:	0001247a 	.word	0x0001247a

00003324 <do_tt_blk>:
    3324:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3328:	e3520001 	cmp	r2, #1	; 0x1
    332c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    3330:	e1a05002 	mov	r5, r2
    3334:	e1a07003 	mov	r7, r3
    3338:	ca000005 	bgt	3354 <do_tt_blk+0x30>
    333c:	e59f01bc 	ldr	r0, [pc, #444]	; 3500 <do_tt_blk+0x1dc>
    3340:	e59f31bc 	ldr	r3, [pc, #444]	; 3504 <do_tt_blk+0x1e0>
    3344:	e1a0e00f 	mov	lr, pc
    3348:	e593f000 	ldr	pc, [r3]
    334c:	e3e00000 	mvn	r0, #0	; 0x0
    3350:	ea000068 	b	34f8 <do_tt_blk+0x1d4>
    3354:	e3520002 	cmp	r2, #2	; 0x2
    3358:	03a04000 	moveq	r4, #0	; 0x0
    335c:	01a06004 	moveq	r6, r4
    3360:	0a000025 	beq	33fc <do_tt_blk+0xd8>
    3364:	e3a01000 	mov	r1, #0	; 0x0
    3368:	e3a0200a 	mov	r2, #10	; 0xa
    336c:	e5930008 	ldr	r0, [r3, #8]
    3370:	eb002760 	bl	d0f8 <simple_strtoul>
    3374:	e59f118c 	ldr	r1, [pc, #396]	; 3508 <do_tt_blk+0x1e4>
    3378:	e1a04000 	mov	r4, r0
    337c:	e3a02005 	mov	r2, #5	; 0x5
    3380:	e5970004 	ldr	r0, [r7, #4]
    3384:	ebfff7b7 	bl	1268 <strncmp>
    3388:	e3500000 	cmp	r0, #0	; 0x0
    338c:	059f0178 	ldreq	r0, [pc, #376]	; 350c <do_tt_blk+0x1e8>
    3390:	01a01004 	moveq	r1, r4
    3394:	059f3168 	ldreq	r3, [pc, #360]	; 3504 <do_tt_blk+0x1e0>
    3398:	0a000008 	beq	33c0 <do_tt_blk+0x9c>
    339c:	e59f116c 	ldr	r1, [pc, #364]	; 3510 <do_tt_blk+0x1ec>
    33a0:	e5970004 	ldr	r0, [r7, #4]
    33a4:	e3a02003 	mov	r2, #3	; 0x3
    33a8:	ebfff7ae 	bl	1268 <strncmp>
    33ac:	e3500000 	cmp	r0, #0	; 0x0
    33b0:	e59f314c 	ldr	r3, [pc, #332]	; 3504 <do_tt_blk+0x1e0>
    33b4:	059f0150 	ldreq	r0, [pc, #336]	; 350c <do_tt_blk+0x1e8>
    33b8:	159f0154 	ldrne	r0, [pc, #340]	; 3514 <do_tt_blk+0x1f0>
    33bc:	e1a01004 	mov	r1, r4
    33c0:	e1a0e00f 	mov	lr, pc
    33c4:	e593f000 	ldr	pc, [r3]
    33c8:	e3550003 	cmp	r5, #3	; 0x3
    33cc:	03a06000 	moveq	r6, #0	; 0x0
    33d0:	0a000009 	beq	33fc <do_tt_blk+0xd8>
    33d4:	e3a01000 	mov	r1, #0	; 0x0
    33d8:	e597000c 	ldr	r0, [r7, #12]
    33dc:	e3a02010 	mov	r2, #16	; 0x10
    33e0:	eb002744 	bl	d0f8 <simple_strtoul>
    33e4:	e1a06000 	mov	r6, r0
    33e8:	e1a01006 	mov	r1, r6
    33ec:	e59f0124 	ldr	r0, [pc, #292]	; 3518 <do_tt_blk+0x1f4>
    33f0:	e59f310c 	ldr	r3, [pc, #268]	; 3504 <do_tt_blk+0x1e0>
    33f4:	e1a0e00f 	mov	lr, pc
    33f8:	e593f000 	ldr	pc, [r3]
    33fc:	e3a00000 	mov	r0, #0	; 0x0
    3400:	eb001538 	bl	88e8 <get_timer>
    3404:	e59f1110 	ldr	r1, [pc, #272]	; 351c <do_tt_blk+0x1f8>
    3408:	e3a02000 	mov	r2, #0	; 0x0
    340c:	e3a03000 	mov	r3, #0	; 0x0
    3410:	e881000c 	stm	r1, {r2, r3}
    3414:	e1a09a04 	lsl	r9, r4, #20
    3418:	e2875004 	add	r5, r7, #4	; 0x4
    341c:	e1a08b24 	lsr	r8, r4, #22
    3420:	e1a07504 	lsl	r7, r4, #10
    3424:	e1a0a624 	lsr	sl, r4, #12
    3428:	e1a0b000 	mov	fp, r0
    342c:	e3a04000 	mov	r4, #0	; 0x0
    3430:	e59f30cc 	ldr	r3, [pc, #204]	; 3504 <do_tt_blk+0x1e0>
    3434:	e1a01004 	mov	r1, r4
    3438:	e59f00e0 	ldr	r0, [pc, #224]	; 3520 <do_tt_blk+0x1fc>
    343c:	e1a0e00f 	mov	lr, pc
    3440:	e593f000 	ldr	pc, [r3]
    3444:	e59f10bc 	ldr	r1, [pc, #188]	; 3508 <do_tt_blk+0x1e4>
    3448:	e3a02005 	mov	r2, #5	; 0x5
    344c:	e5950000 	ldr	r0, [r5]
    3450:	ebfff784 	bl	1268 <strncmp>
    3454:	e3500000 	cmp	r0, #0	; 0x0
    3458:	e3a02003 	mov	r2, #3	; 0x3
    345c:	e59f10ac 	ldr	r1, [pc, #172]	; 3510 <do_tt_blk+0x1ec>
    3460:	e1a03008 	mov	r3, r8
    3464:	05950000 	ldreq	r0, [r5]
    3468:	01a02007 	moveq	r2, r7
    346c:	0a000008 	beq	3494 <do_tt_blk+0x170>
    3470:	e5950000 	ldr	r0, [r5]
    3474:	ebfff77b 	bl	1268 <strncmp>
    3478:	e3500000 	cmp	r0, #0	; 0x0
    347c:	05950000 	ldreq	r0, [r5]
    3480:	15950000 	ldrne	r0, [r5]
    3484:	e1a02009 	mov	r2, r9
    3488:	e1a0300a 	mov	r3, sl
    348c:	01a02007 	moveq	r2, r7
    3490:	01a03008 	moveq	r3, r8
    3494:	e58d6000 	str	r6, [sp]
    3498:	ebfffe93 	bl	2eec <tt_blk_trolling>
    349c:	eb002b2c 	bl	e154 <tstc>
    34a0:	e3500000 	cmp	r0, #0	; 0x0
    34a4:	e2844001 	add	r4, r4, #1	; 0x1
    34a8:	0affffe0 	beq	3430 <do_tt_blk+0x10c>
    34ac:	eb002b29 	bl	e158 <getc>
    34b0:	e3500003 	cmp	r0, #3	; 0x3
    34b4:	1affffdd 	bne	3430 <do_tt_blk+0x10c>
    34b8:	e1a0000b 	mov	r0, fp
    34bc:	eb001509 	bl	88e8 <get_timer>
    34c0:	e59f3054 	ldr	r3, [pc, #84]	; 351c <do_tt_blk+0x1f8>
    34c4:	e30e1a60 	movw	r1, #60000	; 0xea60
    34c8:	e5934000 	ldr	r4, [r3]
    34cc:	e5933004 	ldr	r3, [r3, #4]
    34d0:	e1a04a24 	lsr	r4, r4, #20
    34d4:	e1844603 	orr	r4, r4, r3, lsl #12
    34d8:	eb00330a 	bl	10108 <__aeabi_uidiv>
    34dc:	e1a01004 	mov	r1, r4
    34e0:	e1a02000 	mov	r2, r0
    34e4:	e59f3018 	ldr	r3, [pc, #24]	; 3504 <do_tt_blk+0x1e0>
    34e8:	e59f0034 	ldr	r0, [pc, #52]	; 3524 <do_tt_blk+0x200>
    34ec:	e1a0e00f 	mov	lr, pc
    34f0:	e593f000 	ldr	pc, [r3]
    34f4:	e3a00000 	mov	r0, #0	; 0x0
    34f8:	e28dd00c 	add	sp, sp, #12	; 0xc
    34fc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    3500:	00012735 	.word	0x00012735
    3504:	0800d76c 	.word	0x0800d76c
    3508:	00011b63 	.word	0x00011b63
    350c:	0001277c 	.word	0x0001277c
    3510:	0001213a 	.word	0x0001213a
    3514:	000127a0 	.word	0x000127a0
    3518:	000127c4 	.word	0x000127c4
    351c:	0800d1c0 	.word	0x0800d1c0
    3520:	000127e7 	.word	0x000127e7
    3524:	000127f1 	.word	0x000127f1

00003528 <__show_strbuf>:
    3528:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    352c:	e59f4058 	ldr	r4, [pc, #88]	; 358c <__show_strbuf+0x64>
    3530:	e24dd004 	sub	sp, sp, #4	; 0x4
    3534:	e1a06001 	mov	r6, r1
    3538:	e1a01000 	mov	r1, r0
    353c:	e59f004c 	ldr	r0, [pc, #76]	; 3590 <__show_strbuf+0x68>
    3540:	e1a05002 	mov	r5, r2
    3544:	e1a07004 	mov	r7, r4
    3548:	e1a0e00f 	mov	lr, pc
    354c:	e594f000 	ldr	pc, [r4]
    3550:	e3a04000 	mov	r4, #0	; 0x0
    3554:	ea000003 	b	3568 <__show_strbuf+0x40>
    3558:	e7d61004 	ldrb	r1, [r6, r4]
    355c:	e1a0e00f 	mov	lr, pc
    3560:	e597f000 	ldr	pc, [r7]
    3564:	e2844001 	add	r4, r4, #1	; 0x1
    3568:	e1540005 	cmp	r4, r5
    356c:	e59f0020 	ldr	r0, [pc, #32]	; 3594 <__show_strbuf+0x6c>
    3570:	bafffff8 	blt	3558 <__show_strbuf+0x30>
    3574:	e59f001c 	ldr	r0, [pc, #28]	; 3598 <__show_strbuf+0x70>
    3578:	e59f300c 	ldr	r3, [pc, #12]	; 358c <__show_strbuf+0x64>
    357c:	e1a0e00f 	mov	lr, pc
    3580:	e593f000 	ldr	pc, [r3]
    3584:	e28dd004 	add	sp, sp, #4	; 0x4
    3588:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    358c:	0800d76c 	.word	0x0800d76c
    3590:	0001283f 	.word	0x0001283f
    3594:	000141ba 	.word	0x000141ba
    3598:	0001247a 	.word	0x0001247a

0000359c <do_crypto_quick>:
    359c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    35a0:	e24dd0f0 	sub	sp, sp, #240	; 0xf0
    35a4:	e28da0e0 	add	sl, sp, #224	; 0xe0
    35a8:	e3a04010 	mov	r4, #16	; 0x10
    35ac:	e28d70c0 	add	r7, sp, #192	; 0xc0
    35b0:	e1a02004 	mov	r2, r4
    35b4:	e3a09020 	mov	r9, #32	; 0x20
    35b8:	e59f11f0 	ldr	r1, [pc, #496]	; 37b0 <do_crypto_quick+0x214>
    35bc:	e1a0000a 	mov	r0, sl
    35c0:	e3a05040 	mov	r5, #64	; 0x40
    35c4:	e28d6080 	add	r6, sp, #128	; 0x80
    35c8:	ebfff75f 	bl	134c <memcpy>
    35cc:	e1a02009 	mov	r2, r9
    35d0:	e59f11dc 	ldr	r1, [pc, #476]	; 37b4 <do_crypto_quick+0x218>
    35d4:	e1a00007 	mov	r0, r7
    35d8:	ebfff75b 	bl	134c <memcpy>
    35dc:	e08d8005 	add	r8, sp, r5
    35e0:	e1a02005 	mov	r2, r5
    35e4:	e59f11cc 	ldr	r1, [pc, #460]	; 37b8 <do_crypto_quick+0x21c>
    35e8:	e1a00006 	mov	r0, r6
    35ec:	ebfff756 	bl	134c <memcpy>
    35f0:	e1a02005 	mov	r2, r5
    35f4:	e59f11c0 	ldr	r1, [pc, #448]	; 37bc <do_crypto_quick+0x220>
    35f8:	e1a00008 	mov	r0, r8
    35fc:	ebfff752 	bl	134c <memcpy>
    3600:	e1a02004 	mov	r2, r4
    3604:	e1a0100a 	mov	r1, sl
    3608:	e59f01b0 	ldr	r0, [pc, #432]	; 37c0 <do_crypto_quick+0x224>
    360c:	ebffffc5 	bl	3528 <__show_strbuf>
    3610:	e1a02009 	mov	r2, r9
    3614:	e1a01007 	mov	r1, r7
    3618:	e59f01a4 	ldr	r0, [pc, #420]	; 37c4 <do_crypto_quick+0x228>
    361c:	ebffffc1 	bl	3528 <__show_strbuf>
    3620:	e1a01006 	mov	r1, r6
    3624:	e1a02005 	mov	r2, r5
    3628:	e59f0198 	ldr	r0, [pc, #408]	; 37c8 <do_crypto_quick+0x22c>
    362c:	ebffffbd 	bl	3528 <__show_strbuf>
    3630:	e1a01008 	mov	r1, r8
    3634:	e1a02005 	mov	r2, r5
    3638:	e59f018c 	ldr	r0, [pc, #396]	; 37cc <do_crypto_quick+0x230>
    363c:	ebffffb9 	bl	3528 <__show_strbuf>
    3640:	e1a01007 	mov	r1, r7
    3644:	e1a0200a 	mov	r2, sl
    3648:	e3a00001 	mov	r0, #1	; 0x1
    364c:	eb0006fa 	bl	523c <ss_aes_init>
    3650:	e1a0100d 	mov	r1, sp
    3654:	e1a02005 	mov	r2, r5
    3658:	e1a00006 	mov	r0, r6
    365c:	eb0006e5 	bl	51f8 <ss_aes_data>
    3660:	e1a0100d 	mov	r1, sp
    3664:	e1a02005 	mov	r2, r5
    3668:	e59f0160 	ldr	r0, [pc, #352]	; 37d0 <do_crypto_quick+0x234>
    366c:	ebffffad 	bl	3528 <__show_strbuf>
    3670:	e1a01007 	mov	r1, r7
    3674:	e1a0200a 	mov	r2, sl
    3678:	e3a00000 	mov	r0, #0	; 0x0
    367c:	eb0006ee 	bl	523c <ss_aes_init>
    3680:	e59f714c 	ldr	r7, [pc, #332]	; 37d4 <do_crypto_quick+0x238>
    3684:	e1a0100d 	mov	r1, sp
    3688:	e1a02005 	mov	r2, r5
    368c:	e1a00008 	mov	r0, r8
    3690:	eb0006d8 	bl	51f8 <ss_aes_data>
    3694:	e1a0100d 	mov	r1, sp
    3698:	e1a02005 	mov	r2, r5
    369c:	e59f0134 	ldr	r0, [pc, #308]	; 37d8 <do_crypto_quick+0x23c>
    36a0:	ebffffa0 	bl	3528 <__show_strbuf>
    36a4:	e1a02007 	mov	r2, r7
    36a8:	e3a01000 	mov	r1, #0	; 0x0
    36ac:	e3a00001 	mov	r0, #1	; 0x1
    36b0:	eb0006e1 	bl	523c <ss_aes_init>
    36b4:	e1a0100d 	mov	r1, sp
    36b8:	e1a02005 	mov	r2, r5
    36bc:	e1a00006 	mov	r0, r6
    36c0:	eb0006cc 	bl	51f8 <ss_aes_data>
    36c4:	e1a0100d 	mov	r1, sp
    36c8:	e1a02005 	mov	r2, r5
    36cc:	e59f0108 	ldr	r0, [pc, #264]	; 37dc <do_crypto_quick+0x240>
    36d0:	ebffff94 	bl	3528 <__show_strbuf>
    36d4:	e3a00000 	mov	r0, #0	; 0x0
    36d8:	e1a02007 	mov	r2, r7
    36dc:	e1a01000 	mov	r1, r0
    36e0:	eb0006d5 	bl	523c <ss_aes_init>
    36e4:	e1a0100d 	mov	r1, sp
    36e8:	e1a02005 	mov	r2, r5
    36ec:	e1a0000d 	mov	r0, sp
    36f0:	eb0006c0 	bl	51f8 <ss_aes_data>
    36f4:	e1a02005 	mov	r2, r5
    36f8:	e1a0100d 	mov	r1, sp
    36fc:	e59f00dc 	ldr	r0, [pc, #220]	; 37e0 <do_crypto_quick+0x244>
    3700:	ebffff88 	bl	3528 <__show_strbuf>
    3704:	e1a01005 	mov	r1, r5
    3708:	e3a00001 	mov	r0, #1	; 0x1
    370c:	eb0007e8 	bl	56b4 <hash_init>
    3710:	e1a01005 	mov	r1, r5
    3714:	e1a00006 	mov	r0, r6
    3718:	eb0007c0 	bl	5620 <hash_data>
    371c:	e1a0000d 	mov	r0, sp
    3720:	eb0007a2 	bl	55b0 <hash_value>
    3724:	eb00079a 	bl	5594 <hash_len>
    3728:	e1a0100d 	mov	r1, sp
    372c:	e1a02000 	mov	r2, r0
    3730:	e59f00ac 	ldr	r0, [pc, #172]	; 37e4 <do_crypto_quick+0x248>
    3734:	ebffff7b 	bl	3528 <__show_strbuf>
    3738:	e1a01005 	mov	r1, r5
    373c:	e3a00002 	mov	r0, #2	; 0x2
    3740:	eb0007db 	bl	56b4 <hash_init>
    3744:	e1a01005 	mov	r1, r5
    3748:	e1a00006 	mov	r0, r6
    374c:	eb0007b3 	bl	5620 <hash_data>
    3750:	e1a0000d 	mov	r0, sp
    3754:	eb000795 	bl	55b0 <hash_value>
    3758:	eb00078d 	bl	5594 <hash_len>
    375c:	e1a0100d 	mov	r1, sp
    3760:	e1a02000 	mov	r2, r0
    3764:	e59f007c 	ldr	r0, [pc, #124]	; 37e8 <do_crypto_quick+0x24c>
    3768:	ebffff6e 	bl	3528 <__show_strbuf>
    376c:	e1a01005 	mov	r1, r5
    3770:	e3a00003 	mov	r0, #3	; 0x3
    3774:	eb0007ce 	bl	56b4 <hash_init>
    3778:	e1a01005 	mov	r1, r5
    377c:	e1a00006 	mov	r0, r6
    3780:	eb0007a6 	bl	5620 <hash_data>
    3784:	e1a0000d 	mov	r0, sp
    3788:	eb000788 	bl	55b0 <hash_value>
    378c:	eb000780 	bl	5594 <hash_len>
    3790:	e1a0100d 	mov	r1, sp
    3794:	e1a02000 	mov	r2, r0
    3798:	e59f004c 	ldr	r0, [pc, #76]	; 37ec <do_crypto_quick+0x250>
    379c:	ebffff61 	bl	3528 <__show_strbuf>
    37a0:	e1a0400d 	mov	r4, sp
    37a4:	e3a00000 	mov	r0, #0	; 0x0
    37a8:	e28dd0f0 	add	sp, sp, #240	; 0xf0
    37ac:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    37b0:	00010eb4 	.word	0x00010eb4
    37b4:	00010e94 	.word	0x00010e94
    37b8:	00010e54 	.word	0x00010e54
    37bc:	00010e14 	.word	0x00010e14
    37c0:	00012844 	.word	0x00012844
    37c4:	00012847 	.word	0x00012847
    37c8:	0001284b 	.word	0x0001284b
    37cc:	00012856 	.word	0x00012856
    37d0:	00012867 	.word	0x00012867
    37d4:	0800d10c 	.word	0x0800d10c
    37d8:	0001287a 	.word	0x0001287a
    37dc:	00012863 	.word	0x00012863
    37e0:	00012876 	.word	0x00012876
    37e4:	00012889 	.word	0x00012889
    37e8:	0001288d 	.word	0x0001288d
    37ec:	00012892 	.word	0x00012892

000037f0 <tt_crypto_hash>:
    37f0:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    37f4:	e3a0544e 	mov	r5, #1308622848	; 0x4e000000
    37f8:	e5951000 	ldr	r1, [r5]
    37fc:	e59f2220 	ldr	r2, [pc, #544]	; 3a24 <tt_crypto_hash+0x234>
    3800:	e24dd038 	sub	sp, sp, #56	; 0x38
    3804:	e59f321c 	ldr	r3, [pc, #540]	; 3a28 <tt_crypto_hash+0x238>
    3808:	e1510002 	cmp	r1, r2
    380c:	e28dc028 	add	ip, sp, #40	; 0x28
    3810:	e1a06000 	mov	r6, r0
    3814:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    3818:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    381c:	0a000006 	beq	383c <tt_crypto_hash+0x4c>
    3820:	e59f0204 	ldr	r0, [pc, #516]	; 3a2c <tt_crypto_hash+0x23c>
    3824:	e59f3204 	ldr	r3, [pc, #516]	; 3a30 <tt_crypto_hash+0x240>
    3828:	e1a0e00f 	mov	lr, pc
    382c:	e593f000 	ldr	pc, [r3]
    3830:	e59f01fc 	ldr	r0, [pc, #508]	; 3a34 <tt_crypto_hash+0x244>
    3834:	eb001407 	bl	8858 <udelay>
    3838:	ea000008 	b	3860 <tt_crypto_hash+0x70>
    383c:	e5951004 	ldr	r1, [r5, #4]
    3840:	e3a04000 	mov	r4, #0	; 0x0
    3844:	e3510003 	cmp	r1, #3	; 0x3
    3848:	e5854000 	str	r4, [r5]
    384c:	e59fc1dc 	ldr	ip, [pc, #476]	; 3a30 <tt_crypto_hash+0x240>
    3850:	9a000004 	bls	3868 <tt_crypto_hash+0x78>
    3854:	e59f01dc 	ldr	r0, [pc, #476]	; 3a38 <tt_crypto_hash+0x248>
    3858:	e1a0e00f 	mov	lr, pc
    385c:	e59cf000 	ldr	pc, [ip]
    3860:	e3e0c000 	mvn	ip, #0	; 0x0
    3864:	ea00006b 	b	3a18 <tt_crypto_hash+0x228>
    3868:	e28d2038 	add	r2, sp, #56	; 0x38
    386c:	e0823101 	add	r3, r2, r1, lsl #2
    3870:	e5131010 	ldr	r1, [r3, #-16]
    3874:	e5952008 	ldr	r2, [r5, #8]
    3878:	e59f01bc 	ldr	r0, [pc, #444]	; 3a3c <tt_crypto_hash+0x24c>
    387c:	e1a0e00f 	mov	lr, pc
    3880:	e59cf000 	ldr	pc, [ip]
    3884:	e9950003 	ldmib	r5, {r0, r1}
    3888:	eb000789 	bl	56b4 <hash_init>
    388c:	eb000740 	bl	5594 <hash_len>
    3890:	e1a03000 	mov	r3, r0
    3894:	e59f01a4 	ldr	r0, [pc, #420]	; 3a40 <tt_crypto_hash+0x250>
    3898:	e3a0c020 	mov	ip, #32	; 0x20
    389c:	e1a01000 	mov	r1, r0
    38a0:	e3a02001 	mov	r2, #1	; 0x1
    38a4:	e58dc000 	str	ip, [sp]
    38a8:	eb00257a 	bl	ce98 <print_buffer>
    38ac:	e3560000 	cmp	r6, #0	; 0x0
    38b0:	0a000022 	beq	3940 <tt_crypto_hash+0x150>
    38b4:	e1a09005 	mov	r9, r5
    38b8:	e1a0a004 	mov	sl, r4
    38bc:	e5955008 	ldr	r5, [r5, #8]
    38c0:	e2868901 	add	r8, r6, #16384	; 0x4000
    38c4:	ea000019 	b	3930 <tt_crypto_hash+0x140>
    38c8:	e5991008 	ldr	r1, [r9, #8]
    38cc:	e3550902 	cmp	r5, #32768	; 0x8000
    38d0:	b1a04005 	movlt	r4, r5
    38d4:	a3a04902 	movge	r4, #32768	; 0x8000
    38d8:	e0651001 	rsb	r1, r5, r1
    38dc:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    38e0:	e2811030 	add	r1, r1, #48	; 0x30
    38e4:	e1a02004 	mov	r2, r4
    38e8:	ebfff697 	bl	134c <memcpy>
    38ec:	e3a00000 	mov	r0, #0	; 0x0
    38f0:	eb0013fc 	bl	88e8 <get_timer>
    38f4:	e3550901 	cmp	r5, #16384	; 0x4000
    38f8:	b1a01005 	movlt	r1, r5
    38fc:	a3a01901 	movge	r1, #16384	; 0x4000
    3900:	e1a07000 	mov	r7, r0
    3904:	e1a00006 	mov	r0, r6
    3908:	eb000744 	bl	5620 <hash_data>
    390c:	e3540902 	cmp	r4, #32768	; 0x8000
    3910:	e0645005 	rsb	r5, r4, r5
    3914:	1a000002 	bne	3924 <tt_crypto_hash+0x134>
    3918:	e3a01901 	mov	r1, #16384	; 0x4000
    391c:	e1a00008 	mov	r0, r8
    3920:	eb00073e 	bl	5620 <hash_data>
    3924:	e1a00007 	mov	r0, r7
    3928:	eb0013ee 	bl	88e8 <get_timer>
    392c:	e08aa000 	add	sl, sl, r0
    3930:	e3550000 	cmp	r5, #0	; 0x0
    3934:	e1a00006 	mov	r0, r6
    3938:	1affffe2 	bne	38c8 <tt_crypto_hash+0xd8>
    393c:	ea000008 	b	3964 <tt_crypto_hash+0x174>
    3940:	e1a00006 	mov	r0, r6
    3944:	eb0013e7 	bl	88e8 <get_timer>
    3948:	e5951008 	ldr	r1, [r5, #8]
    394c:	e1a04000 	mov	r4, r0
    3950:	e59f00ec 	ldr	r0, [pc, #236]	; 3a44 <tt_crypto_hash+0x254>
    3954:	eb000731 	bl	5620 <hash_data>
    3958:	e1a00004 	mov	r0, r4
    395c:	eb0013e1 	bl	88e8 <get_timer>
    3960:	e1a0a000 	mov	sl, r0
    3964:	e28d4008 	add	r4, sp, #8	; 0x8
    3968:	e3a0644e 	mov	r6, #1308622848	; 0x4e000000
    396c:	e1a00004 	mov	r0, r4
    3970:	eb00070e 	bl	55b0 <hash_value>
    3974:	e5965008 	ldr	r5, [r6, #8]
    3978:	eb000705 	bl	5594 <hash_len>
    397c:	e3a0c020 	mov	ip, #32	; 0x20
    3980:	e1a03000 	mov	r3, r0
    3984:	e1a01004 	mov	r1, r4
    3988:	e3a02001 	mov	r2, #1	; 0x1
    398c:	e1a00004 	mov	r0, r4
    3990:	e58dc000 	str	ip, [sp]
    3994:	eb00253f 	bl	ce98 <print_buffer>
    3998:	e5963004 	ldr	r3, [r6, #4]
    399c:	e28d1038 	add	r1, sp, #56	; 0x38
    39a0:	e0812103 	add	r2, r1, r3, lsl #2
    39a4:	e59f3084 	ldr	r3, [pc, #132]	; 3a30 <tt_crypto_hash+0x240>
    39a8:	e5127010 	ldr	r7, [r2, #-16]
    39ac:	e5938000 	ldr	r8, [r3]
    39b0:	eb0006f7 	bl	5594 <hash_len>
    39b4:	e1a01004 	mov	r1, r4
    39b8:	e1a02000 	mov	r2, r0
    39bc:	e59f007c 	ldr	r0, [pc, #124]	; 3a40 <tt_crypto_hash+0x250>
    39c0:	ebfff682 	bl	13d0 <memcmp>
    39c4:	e1a05525 	lsr	r5, r5, #10
    39c8:	e3500000 	cmp	r0, #0	; 0x0
    39cc:	e59f3074 	ldr	r3, [pc, #116]	; 3a48 <tt_crypto_hash+0x258>
    39d0:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    39d4:	e1a0100a 	mov	r1, sl
    39d8:	e0000590 	mul	r0, r0, r5
    39dc:	e59f4068 	ldr	r4, [pc, #104]	; 3a4c <tt_crypto_hash+0x25c>
    39e0:	01a04003 	moveq	r4, r3
    39e4:	eb0031c7 	bl	10108 <__aeabi_uidiv>
    39e8:	e1a01007 	mov	r1, r7
    39ec:	e1a03000 	mov	r3, r0
    39f0:	e1a02004 	mov	r2, r4
    39f4:	e59f0054 	ldr	r0, [pc, #84]	; 3a50 <tt_crypto_hash+0x260>
    39f8:	e12fff38 	blx	r8
    39fc:	e59f1050 	ldr	r1, [pc, #80]	; 3a54 <tt_crypto_hash+0x264>
    3a00:	e5960008 	ldr	r0, [r6, #8]
    3a04:	e891000c 	ldm	r1, {r2, r3}
    3a08:	e0922000 	adds	r2, r2, r0
    3a0c:	e2a33000 	adc	r3, r3, #0	; 0x0
    3a10:	e881000c 	stm	r1, {r2, r3}
    3a14:	e3a0c000 	mov	ip, #0	; 0x0
    3a18:	e1a0000c 	mov	r0, ip
    3a1c:	e28dd038 	add	sp, sp, #56	; 0x38
    3a20:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3a24:	99997777 	.word	0x99997777
    3a28:	00010ec4 	.word	0x00010ec4
    3a2c:	00012899 	.word	0x00012899
    3a30:	0800d76c 	.word	0x0800d76c
    3a34:	004c4b40 	.word	0x004c4b40
    3a38:	000128b8 	.word	0x000128b8
    3a3c:	000128d3 	.word	0x000128d3
    3a40:	4e000010 	.word	0x4e000010
    3a44:	4e000030 	.word	0x4e000030
    3a48:	000128ee 	.word	0x000128ee
    3a4c:	000128f3 	.word	0x000128f3
    3a50:	000128fa 	.word	0x000128fa
    3a54:	0800d1d0 	.word	0x0800d1d0

00003a58 <tt_crypto_aes>:
    3a58:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3a5c:	e24dd014 	sub	sp, sp, #20	; 0x14
    3a60:	e1a06000 	mov	r6, r0
    3a64:	e3a01000 	mov	r1, #0	; 0x0
    3a68:	e28d000c 	add	r0, sp, #12	; 0xc
    3a6c:	e3a02502 	mov	r2, #8388608	; 0x800000
    3a70:	eb000284 	bl	4488 <tt_rnd_get_data>
    3a74:	e250b000 	subs	fp, r0, #0	; 0x0
    3a78:	e59f4340 	ldr	r4, [pc, #832]	; 3dc0 <tt_crypto_aes+0x368>
    3a7c:	059f0340 	ldreq	r0, [pc, #832]	; 3dc4 <tt_crypto_aes+0x36c>
    3a80:	0a00005c 	beq	3bf8 <tt_crypto_aes+0x1a0>
    3a84:	e59d200c 	ldr	r2, [sp, #12]
    3a88:	e1a0100b 	mov	r1, fp
    3a8c:	e3c2200f 	bic	r2, r2, #15	; 0xf
    3a90:	e1a03002 	mov	r3, r2
    3a94:	e58d200c 	str	r2, [sp, #12]
    3a98:	e59f0328 	ldr	r0, [pc, #808]	; 3dc8 <tt_crypto_aes+0x370>
    3a9c:	e1a02006 	mov	r2, r6
    3aa0:	e1a0e00f 	mov	lr, pc
    3aa4:	e594f000 	ldr	pc, [r4]
    3aa8:	e59f031c 	ldr	r0, [pc, #796]	; 3dcc <tt_crypto_aes+0x374>
    3aac:	e1a0e00f 	mov	lr, pc
    3ab0:	e594f000 	ldr	pc, [r4]
    3ab4:	e1a0100b 	mov	r1, fp
    3ab8:	e59d200c 	ldr	r2, [sp, #12]
    3abc:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3ac0:	ebfff621 	bl	134c <memcpy>
    3ac4:	e59f0304 	ldr	r0, [pc, #772]	; 3dd0 <tt_crypto_aes+0x378>
    3ac8:	e1a0e00f 	mov	lr, pc
    3acc:	e594f000 	ldr	pc, [r4]
    3ad0:	e3a01000 	mov	r1, #0	; 0x0
    3ad4:	e59f22f8 	ldr	r2, [pc, #760]	; 3dd4 <tt_crypto_aes+0x37c>
    3ad8:	e3a00001 	mov	r0, #1	; 0x1
    3adc:	eb0005d6 	bl	523c <ss_aes_init>
    3ae0:	e59f02f0 	ldr	r0, [pc, #752]	; 3dd8 <tt_crypto_aes+0x380>
    3ae4:	e1a0e00f 	mov	lr, pc
    3ae8:	e594f000 	ldr	pc, [r4]
    3aec:	e3560000 	cmp	r6, #0	; 0x0
    3af0:	159d500c 	ldrne	r5, [sp, #12]
    3af4:	13a0a000 	movne	sl, #0	; 0x0
    3af8:	12868901 	addne	r8, r6, #16384	; 0x4000
    3afc:	1a000022 	bne	3b8c <tt_crypto_aes+0x134>
    3b00:	ea000025 	b	3b9c <tt_crypto_aes+0x144>
    3b04:	e59d100c 	ldr	r1, [sp, #12]
    3b08:	e3550902 	cmp	r5, #32768	; 0x8000
    3b0c:	b1a07005 	movlt	r7, r5
    3b10:	a3a07902 	movge	r7, #32768	; 0x8000
    3b14:	e0651001 	rsb	r1, r5, r1
    3b18:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3b1c:	e1a02007 	mov	r2, r7
    3b20:	ebfff609 	bl	134c <memcpy>
    3b24:	e3a00000 	mov	r0, #0	; 0x0
    3b28:	eb00136e 	bl	88e8 <get_timer>
    3b2c:	e3550901 	cmp	r5, #16384	; 0x4000
    3b30:	b1a02005 	movlt	r2, r5
    3b34:	a3a02901 	movge	r2, #16384	; 0x4000
    3b38:	e1a04000 	mov	r4, r0
    3b3c:	e1a01006 	mov	r1, r6
    3b40:	e1a00006 	mov	r0, r6
    3b44:	eb0005ab 	bl	51f8 <ss_aes_data>
    3b48:	e3570902 	cmp	r7, #32768	; 0x8000
    3b4c:	1a000003 	bne	3b60 <tt_crypto_aes+0x108>
    3b50:	e1a01008 	mov	r1, r8
    3b54:	e3a02901 	mov	r2, #16384	; 0x4000
    3b58:	e1a00008 	mov	r0, r8
    3b5c:	eb0005a5 	bl	51f8 <ss_aes_data>
    3b60:	e1a00004 	mov	r0, r4
    3b64:	eb00135f 	bl	88e8 <get_timer>
    3b68:	e1a04000 	mov	r4, r0
    3b6c:	e59d000c 	ldr	r0, [sp, #12]
    3b70:	e1a01006 	mov	r1, r6
    3b74:	e0650000 	rsb	r0, r5, r0
    3b78:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    3b7c:	e1a02007 	mov	r2, r7
    3b80:	ebfff5f1 	bl	134c <memcpy>
    3b84:	e08aa004 	add	sl, sl, r4
    3b88:	e0675005 	rsb	r5, r7, r5
    3b8c:	e3550000 	cmp	r5, #0	; 0x0
    3b90:	e1a00006 	mov	r0, r6
    3b94:	1affffda 	bne	3b04 <tt_crypto_aes+0xac>
    3b98:	ea000009 	b	3bc4 <tt_crypto_aes+0x16c>
    3b9c:	e1a00006 	mov	r0, r6
    3ba0:	eb001350 	bl	88e8 <get_timer>
    3ba4:	e1a04000 	mov	r4, r0
    3ba8:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3bac:	e1a01000 	mov	r1, r0
    3bb0:	e59d200c 	ldr	r2, [sp, #12]
    3bb4:	eb00058f 	bl	51f8 <ss_aes_data>
    3bb8:	e1a00004 	mov	r0, r4
    3bbc:	eb001349 	bl	88e8 <get_timer>
    3bc0:	e1a0a000 	mov	sl, r0
    3bc4:	e59d300c 	ldr	r3, [sp, #12]
    3bc8:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3bcc:	e1a0100b 	mov	r1, fp
    3bd0:	e3a02010 	mov	r2, #16	; 0x10
    3bd4:	e58d3004 	str	r3, [sp, #4]
    3bd8:	ebfff5fc 	bl	13d0 <memcmp>
    3bdc:	e3500000 	cmp	r0, #0	; 0x0
    3be0:	e59f41d8 	ldr	r4, [pc, #472]	; 3dc0 <tt_crypto_aes+0x368>
    3be4:	1a000007 	bne	3c08 <tt_crypto_aes+0x1b0>
    3be8:	e59f01ec 	ldr	r0, [pc, #492]	; 3ddc <tt_crypto_aes+0x384>
    3bec:	e1a0e00f 	mov	lr, pc
    3bf0:	e594f000 	ldr	pc, [r4]
    3bf4:	e59f01e4 	ldr	r0, [pc, #484]	; 3de0 <tt_crypto_aes+0x388>
    3bf8:	e1a0e00f 	mov	lr, pc
    3bfc:	e594f000 	ldr	pc, [r4]
    3c00:	e3e00000 	mvn	r0, #0	; 0x0
    3c04:	ea00006b 	b	3db8 <tt_crypto_aes+0x360>
    3c08:	e3a00000 	mov	r0, #0	; 0x0
    3c0c:	e1a01000 	mov	r1, r0
    3c10:	e59f21bc 	ldr	r2, [pc, #444]	; 3dd4 <tt_crypto_aes+0x37c>
    3c14:	eb000588 	bl	523c <ss_aes_init>
    3c18:	e59f01c4 	ldr	r0, [pc, #452]	; 3de4 <tt_crypto_aes+0x38c>
    3c1c:	e1a0e00f 	mov	lr, pc
    3c20:	e594f000 	ldr	pc, [r4]
    3c24:	e3560000 	cmp	r6, #0	; 0x0
    3c28:	159d500c 	ldrne	r5, [sp, #12]
    3c2c:	13a08000 	movne	r8, #0	; 0x0
    3c30:	12869901 	addne	r9, r6, #16384	; 0x4000
    3c34:	1a000022 	bne	3cc4 <tt_crypto_aes+0x26c>
    3c38:	ea000025 	b	3cd4 <tt_crypto_aes+0x27c>
    3c3c:	eb001329 	bl	88e8 <get_timer>
    3c40:	e59d100c 	ldr	r1, [sp, #12]
    3c44:	e3550902 	cmp	r5, #32768	; 0x8000
    3c48:	b1a07005 	movlt	r7, r5
    3c4c:	a3a07902 	movge	r7, #32768	; 0x8000
    3c50:	e0651001 	rsb	r1, r5, r1
    3c54:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3c58:	e1a02007 	mov	r2, r7
    3c5c:	e1a04000 	mov	r4, r0
    3c60:	e1a00006 	mov	r0, r6
    3c64:	ebfff5b8 	bl	134c <memcpy>
    3c68:	e3550901 	cmp	r5, #16384	; 0x4000
    3c6c:	b1a02005 	movlt	r2, r5
    3c70:	a3a02901 	movge	r2, #16384	; 0x4000
    3c74:	e1a00006 	mov	r0, r6
    3c78:	e1a01006 	mov	r1, r6
    3c7c:	eb00055d 	bl	51f8 <ss_aes_data>
    3c80:	e3570902 	cmp	r7, #32768	; 0x8000
    3c84:	1a000003 	bne	3c98 <tt_crypto_aes+0x240>
    3c88:	e1a01009 	mov	r1, r9
    3c8c:	e3a02901 	mov	r2, #16384	; 0x4000
    3c90:	e1a00009 	mov	r0, r9
    3c94:	eb000557 	bl	51f8 <ss_aes_data>
    3c98:	e1a00004 	mov	r0, r4
    3c9c:	eb001311 	bl	88e8 <get_timer>
    3ca0:	e1a04000 	mov	r4, r0
    3ca4:	e59d000c 	ldr	r0, [sp, #12]
    3ca8:	e1a01006 	mov	r1, r6
    3cac:	e0650000 	rsb	r0, r5, r0
    3cb0:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    3cb4:	e1a02007 	mov	r2, r7
    3cb8:	ebfff5a3 	bl	134c <memcpy>
    3cbc:	e0888004 	add	r8, r8, r4
    3cc0:	e0675005 	rsb	r5, r7, r5
    3cc4:	e3a00000 	mov	r0, #0	; 0x0
    3cc8:	e1550000 	cmp	r5, r0
    3ccc:	1affffda 	bne	3c3c <tt_crypto_aes+0x1e4>
    3cd0:	ea000009 	b	3cfc <tt_crypto_aes+0x2a4>
    3cd4:	e1a00006 	mov	r0, r6
    3cd8:	eb001302 	bl	88e8 <get_timer>
    3cdc:	e1a04000 	mov	r4, r0
    3ce0:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3ce4:	e1a01000 	mov	r1, r0
    3ce8:	e59d200c 	ldr	r2, [sp, #12]
    3cec:	eb000541 	bl	51f8 <ss_aes_data>
    3cf0:	e1a00004 	mov	r0, r4
    3cf4:	eb0012fb 	bl	88e8 <get_timer>
    3cf8:	e1a08000 	mov	r8, r0
    3cfc:	e59d3004 	ldr	r3, [sp, #4]
    3d00:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    3d04:	e1a00543 	asr	r0, r3, #10
    3d08:	e1a0100a 	mov	r1, sl
    3d0c:	e0000094 	mul	r0, r4, r0
    3d10:	eb00317e 	bl	10310 <__aeabi_idiv>
    3d14:	e1a06000 	mov	r6, r0
    3d18:	e59d000c 	ldr	r0, [sp, #12]
    3d1c:	e1a01008 	mov	r1, r8
    3d20:	e1a00540 	asr	r0, r0, #10
    3d24:	e0000094 	mul	r0, r4, r0
    3d28:	eb003178 	bl	10310 <__aeabi_idiv>
    3d2c:	e59f408c 	ldr	r4, [pc, #140]	; 3dc0 <tt_crypto_aes+0x368>
    3d30:	e1a05000 	mov	r5, r0
    3d34:	e59f00ac 	ldr	r0, [pc, #172]	; 3de8 <tt_crypto_aes+0x390>
    3d38:	e1a0e00f 	mov	lr, pc
    3d3c:	e594f000 	ldr	pc, [r4]
    3d40:	e1a0000b 	mov	r0, fp
    3d44:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    3d48:	e59d200c 	ldr	r2, [sp, #12]
    3d4c:	e3a0300c 	mov	r3, #12	; 0xc
    3d50:	eb00014f 	bl	4294 <tt_cmp_buffer>
    3d54:	e3500000 	cmp	r0, #0	; 0x0
    3d58:	0a000006 	beq	3d78 <tt_crypto_aes+0x320>
    3d5c:	e1a03000 	mov	r3, r0
    3d60:	e1a01006 	mov	r1, r6
    3d64:	e1a02005 	mov	r2, r5
    3d68:	e59f007c 	ldr	r0, [pc, #124]	; 3dec <tt_crypto_aes+0x394>
    3d6c:	e1a0e00f 	mov	lr, pc
    3d70:	e594f000 	ldr	pc, [r4]
    3d74:	ea000004 	b	3d8c <tt_crypto_aes+0x334>
    3d78:	e1a01006 	mov	r1, r6
    3d7c:	e1a02005 	mov	r2, r5
    3d80:	e59f0068 	ldr	r0, [pc, #104]	; 3df0 <tt_crypto_aes+0x398>
    3d84:	e1a0e00f 	mov	lr, pc
    3d88:	e594f000 	ldr	pc, [r4]
    3d8c:	e59f0060 	ldr	r0, [pc, #96]	; 3df4 <tt_crypto_aes+0x39c>
    3d90:	e59d100c 	ldr	r1, [sp, #12]
    3d94:	e890000c 	ldm	r0, {r2, r3}
    3d98:	e0922001 	adds	r2, r2, r1
    3d9c:	e0a33fc1 	adc	r3, r3, r1, asr #31
    3da0:	e880000c 	stm	r0, {r2, r3}
    3da4:	e59f0030 	ldr	r0, [pc, #48]	; 3ddc <tt_crypto_aes+0x384>
    3da8:	e59f3010 	ldr	r3, [pc, #16]	; 3dc0 <tt_crypto_aes+0x368>
    3dac:	e1a0e00f 	mov	lr, pc
    3db0:	e593f000 	ldr	pc, [r3]
    3db4:	e3a00000 	mov	r0, #0	; 0x0
    3db8:	e28dd014 	add	sp, sp, #20	; 0x14
    3dbc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    3dc0:	0800d76c 	.word	0x0800d76c
    3dc4:	0001291a 	.word	0x0001291a
    3dc8:	0001293d 	.word	0x0001293d
    3dcc:	00012385 	.word	0x00012385
    3dd0:	00013f66 	.word	0x00013f66
    3dd4:	0800d10c 	.word	0x0800d10c
    3dd8:	00014001 	.word	0x00014001
    3ddc:	0001247a 	.word	0x0001247a
    3de0:	00012969 	.word	0x00012969
    3de4:	0001322c 	.word	0x0001322c
    3de8:	000126cb 	.word	0x000126cb
    3dec:	0001298a 	.word	0x0001298a
    3df0:	000129c8 	.word	0x000129c8
    3df4:	0800d1c8 	.word	0x0800d1c8

00003df8 <do_tt_crypto>:
    3df8:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    3dfc:	e3520001 	cmp	r2, #1	; 0x1
    3e00:	e24dd004 	sub	sp, sp, #4	; 0x4
    3e04:	d3a07000 	movle	r7, #0	; 0x0
    3e08:	da000009 	ble	3e34 <do_tt_crypto+0x3c>
    3e0c:	e5930004 	ldr	r0, [r3, #4]
    3e10:	e3a01000 	mov	r1, #0	; 0x0
    3e14:	e3a02010 	mov	r2, #16	; 0x10
    3e18:	eb0024b6 	bl	d0f8 <simple_strtoul>
    3e1c:	e1a07000 	mov	r7, r0
    3e20:	e1a01007 	mov	r1, r7
    3e24:	e59f0114 	ldr	r0, [pc, #276]	; 3f40 <do_tt_crypto+0x148>
    3e28:	e59f3114 	ldr	r3, [pc, #276]	; 3f44 <do_tt_crypto+0x14c>
    3e2c:	e1a0e00f 	mov	lr, pc
    3e30:	e593f000 	ldr	pc, [r3]
    3e34:	e3a00000 	mov	r0, #0	; 0x0
    3e38:	eb0012aa 	bl	88e8 <get_timer>
    3e3c:	e59f1104 	ldr	r1, [pc, #260]	; 3f48 <do_tt_crypto+0x150>
    3e40:	e3a02000 	mov	r2, #0	; 0x0
    3e44:	e3a03000 	mov	r3, #0	; 0x0
    3e48:	e881000c 	stm	r1, {r2, r3}
    3e4c:	e59f10f8 	ldr	r1, [pc, #248]	; 3f4c <do_tt_crypto+0x154>
    3e50:	e59fa0ec 	ldr	sl, [pc, #236]	; 3f44 <do_tt_crypto+0x14c>
    3e54:	e881000c 	stm	r1, {r2, r3}
    3e58:	e1a08000 	mov	r8, r0
    3e5c:	e3a06000 	mov	r6, #0	; 0x0
    3e60:	e3a01006 	mov	r1, #6	; 0x6
    3e64:	e3a00000 	mov	r0, #0	; 0x0
    3e68:	eb00015d 	bl	43e4 <tt_rnd_get_int>
    3e6c:	e3a01002 	mov	r1, #2	; 0x2
    3e70:	e1a04000 	mov	r4, r0
    3e74:	e3a00000 	mov	r0, #0	; 0x0
    3e78:	eb000159 	bl	43e4 <tt_rnd_get_int>
    3e7c:	e1a01006 	mov	r1, r6
    3e80:	e1a03000 	mov	r3, r0
    3e84:	e1a05000 	mov	r5, r0
    3e88:	e1a02004 	mov	r2, r4
    3e8c:	e59f00bc 	ldr	r0, [pc, #188]	; 3f50 <do_tt_crypto+0x158>
    3e90:	e1a0e00f 	mov	lr, pc
    3e94:	e59af000 	ldr	pc, [sl]
    3e98:	e3540003 	cmp	r4, #3	; 0x3
    3e9c:	9a000004 	bls	3eb4 <do_tt_crypto+0xbc>
    3ea0:	e3550000 	cmp	r5, #0	; 0x0
    3ea4:	11a00007 	movne	r0, r7
    3ea8:	03a00000 	moveq	r0, #0	; 0x0
    3eac:	ebfffee9 	bl	3a58 <tt_crypto_aes>
    3eb0:	ea000003 	b	3ec4 <do_tt_crypto+0xcc>
    3eb4:	e3550000 	cmp	r5, #0	; 0x0
    3eb8:	11a00007 	movne	r0, r7
    3ebc:	03a00000 	moveq	r0, #0	; 0x0
    3ec0:	ebfffe4a 	bl	37f0 <tt_crypto_hash>
    3ec4:	eb0028a2 	bl	e154 <tstc>
    3ec8:	e3500000 	cmp	r0, #0	; 0x0
    3ecc:	e2866001 	add	r6, r6, #1	; 0x1
    3ed0:	0affffe2 	beq	3e60 <do_tt_crypto+0x68>
    3ed4:	eb00289f 	bl	e158 <getc>
    3ed8:	e3500003 	cmp	r0, #3	; 0x3
    3edc:	1affffdf 	bne	3e60 <do_tt_crypto+0x68>
    3ee0:	e1a00008 	mov	r0, r8
    3ee4:	eb00127f 	bl	88e8 <get_timer>
    3ee8:	e59f305c 	ldr	r3, [pc, #92]	; 3f4c <do_tt_crypto+0x154>
    3eec:	e59f2054 	ldr	r2, [pc, #84]	; 3f48 <do_tt_crypto+0x150>
    3ef0:	e5934000 	ldr	r4, [r3]
    3ef4:	e5925000 	ldr	r5, [r2]
    3ef8:	e5933004 	ldr	r3, [r3, #4]
    3efc:	e5922004 	ldr	r2, [r2, #4]
    3f00:	e30e1a60 	movw	r1, #60000	; 0xea60
    3f04:	e1a04a24 	lsr	r4, r4, #20
    3f08:	e1a05a25 	lsr	r5, r5, #20
    3f0c:	e1844603 	orr	r4, r4, r3, lsl #12
    3f10:	e1855602 	orr	r5, r5, r2, lsl #12
    3f14:	eb00307b 	bl	10108 <__aeabi_uidiv>
    3f18:	e1a01004 	mov	r1, r4
    3f1c:	e1a03000 	mov	r3, r0
    3f20:	e1a02005 	mov	r2, r5
    3f24:	e59f0028 	ldr	r0, [pc, #40]	; 3f54 <do_tt_crypto+0x15c>
    3f28:	e59fc014 	ldr	ip, [pc, #20]	; 3f44 <do_tt_crypto+0x14c>
    3f2c:	e1a0e00f 	mov	lr, pc
    3f30:	e59cf000 	ldr	pc, [ip]
    3f34:	e3a00000 	mov	r0, #0	; 0x0
    3f38:	e28dd004 	add	sp, sp, #4	; 0x4
    3f3c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    3f40:	000129f9 	.word	0x000129f9
    3f44:	0800d76c 	.word	0x0800d76c
    3f48:	0800d1d0 	.word	0x0800d1d0
    3f4c:	0800d1c8 	.word	0x0800d1c8
    3f50:	00012a1f 	.word	0x00012a1f
    3f54:	00012a3e 	.word	0x00012a3e

00003f58 <tt_ff_trolling>:
    3f58:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3f5c:	e3a01001 	mov	r1, #1	; 0x1
    3f60:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    3f64:	e58d200c 	str	r2, [sp, #12]
    3f68:	e58d3008 	str	r3, [sp, #8]
    3f6c:	e1a05000 	mov	r5, r0
    3f70:	eb00046e 	bl	5130 <vs_assign_by_name>
    3f74:	e2506000 	subs	r6, r0, #0	; 0x0
    3f78:	0a000005 	beq	3f94 <tt_ff_trolling+0x3c>
    3f7c:	e1a01005 	mov	r1, r5
    3f80:	e59f0258 	ldr	r0, [pc, #600]	; 41e0 <tt_ff_trolling+0x288>
    3f84:	e59f3258 	ldr	r3, [pc, #600]	; 41e4 <tt_ff_trolling+0x28c>
    3f88:	e1a0e00f 	mov	lr, pc
    3f8c:	e593f000 	ldr	pc, [r3]
    3f90:	ea00000c 	b	3fc8 <tt_ff_trolling+0x70>
    3f94:	e3a0000e 	mov	r0, #14	; 0xe
    3f98:	eb0002f1 	bl	4b64 <vs_align>
    3f9c:	e1a01000 	mov	r1, r0
    3fa0:	e28d0018 	add	r0, sp, #24	; 0x18
    3fa4:	e5201004 	str	r1, [r0, #-4]!
    3fa8:	e1a02001 	mov	r2, r1
    3fac:	eb000135 	bl	4488 <tt_rnd_get_data>
    3fb0:	e250a000 	subs	sl, r0, #0	; 0x0
    3fb4:	e59f4228 	ldr	r4, [pc, #552]	; 41e4 <tt_ff_trolling+0x28c>
    3fb8:	1a000004 	bne	3fd0 <tt_ff_trolling+0x78>
    3fbc:	e59f0224 	ldr	r0, [pc, #548]	; 41e8 <tt_ff_trolling+0x290>
    3fc0:	e1a0e00f 	mov	lr, pc
    3fc4:	e594f000 	ldr	pc, [r4]
    3fc8:	e3e00000 	mvn	r0, #0	; 0x0
    3fcc:	ea000073 	b	41a0 <tt_ff_trolling+0x248>
    3fd0:	e59d2014 	ldr	r2, [sp, #20]
    3fd4:	e1a0100a 	mov	r1, sl
    3fd8:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3fdc:	ebfff4da 	bl	134c <memcpy>
    3fe0:	e3a01501 	mov	r1, #4194304	; 0x400000
    3fe4:	e59f0200 	ldr	r0, [pc, #512]	; 41ec <tt_ff_trolling+0x294>
    3fe8:	e1a0e00f 	mov	lr, pc
    3fec:	e594f000 	ldr	pc, [r4]
    3ff0:	e1a00005 	mov	r0, r5
    3ff4:	eb00035b 	bl	4d68 <vs_device_id>
    3ff8:	e3a04000 	mov	r4, #0	; 0x0
    3ffc:	e3a05000 	mov	r5, #0	; 0x0
    4000:	e1a0b006 	mov	fp, r6
    4004:	e3a08501 	mov	r8, #4194304	; 0x400000
    4008:	e3a09000 	mov	r9, #0	; 0x0
    400c:	e1a07000 	mov	r7, r0
    4010:	ea000021 	b	409c <tt_ff_trolling+0x144>
    4014:	eb00033f 	bl	4d18 <vs_device_erasable>
    4018:	e3500000 	cmp	r0, #0	; 0x0
    401c:	0a000003 	beq	4030 <tt_ff_trolling+0xd8>
    4020:	e1a00004 	mov	r0, r4
    4024:	e1a01005 	mov	r1, r5
    4028:	e3a02501 	mov	r2, #4194304	; 0x400000
    402c:	eb000290 	bl	4a74 <vs_erase>
    4030:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    4034:	e5816000 	str	r6, [r1]
    4038:	e1a00001 	mov	r0, r1
    403c:	e59d1014 	ldr	r1, [sp, #20]
    4040:	e1a02004 	mov	r2, r4
    4044:	e1a03005 	mov	r3, r5
    4048:	e88d0802 	stm	sp, {r1, fp}
    404c:	eb000272 	bl	4a1c <vs_write>
    4050:	e3500000 	cmp	r0, #0	; 0x0
    4054:	aa000006 	bge	4074 <tt_ff_trolling+0x11c>
    4058:	e1a02004 	mov	r2, r4
    405c:	e1a03005 	mov	r3, r5
    4060:	e59f0188 	ldr	r0, [pc, #392]	; 41f0 <tt_ff_trolling+0x298>
    4064:	e59f1178 	ldr	r1, [pc, #376]	; 41e4 <tt_ff_trolling+0x28c>
    4068:	e1a0e00f 	mov	lr, pc
    406c:	e591f000 	ldr	pc, [r1]
    4070:	ea000012 	b	40c0 <tt_ff_trolling+0x168>
    4074:	e316001f 	tst	r6, #31	; 0x1f
    4078:	1a000004 	bne	4090 <tt_ff_trolling+0x138>
    407c:	e1a01106 	lsl	r1, r6, #2
    4080:	e59f016c 	ldr	r0, [pc, #364]	; 41f4 <tt_ff_trolling+0x29c>
    4084:	e59f3158 	ldr	r3, [pc, #344]	; 41e4 <tt_ff_trolling+0x28c>
    4088:	e1a0e00f 	mov	lr, pc
    408c:	e593f000 	ldr	pc, [r3]
    4090:	e0944008 	adds	r4, r4, r8
    4094:	e0a55009 	adc	r5, r5, r9
    4098:	e2866001 	add	r6, r6, #1	; 0x1
    409c:	e59d1008 	ldr	r1, [sp, #8]
    40a0:	e1a00007 	mov	r0, r7
    40a4:	e1510005 	cmp	r1, r5
    40a8:	8affffd9 	bhi	4014 <tt_ff_trolling+0xbc>
    40ac:	1a00003d 	bne	41a8 <tt_ff_trolling+0x250>
    40b0:	e59d300c 	ldr	r3, [sp, #12]
    40b4:	e1530004 	cmp	r3, r4
    40b8:	8affffd5 	bhi	4014 <tt_ff_trolling+0xbc>
    40bc:	ea000039 	b	41a8 <tt_ff_trolling+0x250>
    40c0:	e59f411c 	ldr	r4, [pc, #284]	; 41e4 <tt_ff_trolling+0x28c>
    40c4:	e59f012c 	ldr	r0, [pc, #300]	; 41f8 <tt_ff_trolling+0x2a0>
    40c8:	e3a01501 	mov	r1, #4194304	; 0x400000
    40cc:	e3a06000 	mov	r6, #0	; 0x0
    40d0:	e1a0e00f 	mov	lr, pc
    40d4:	e594f000 	ldr	pc, [r4]
    40d8:	e28a7004 	add	r7, sl, #4	; 0x4
    40dc:	e3a04000 	mov	r4, #0	; 0x0
    40e0:	e3a05000 	mov	r5, #0	; 0x0
    40e4:	e1a0b006 	mov	fp, r6
    40e8:	e3a0a44e 	mov	sl, #1308622848	; 0x4e000000
    40ec:	e3a08501 	mov	r8, #4194304	; 0x400000
    40f0:	e3a09000 	mov	r9, #0	; 0x0
    40f4:	ea00001e 	b	4174 <tt_ff_trolling+0x21c>
    40f8:	e59d1014 	ldr	r1, [sp, #20]
    40fc:	e88d0802 	stm	sp, {r1, fp}
    4100:	eb000345 	bl	4e1c <vs_read>
    4104:	e316001f 	tst	r6, #31	; 0x1f
    4108:	1a000004 	bne	4120 <tt_ff_trolling+0x1c8>
    410c:	e1a01106 	lsl	r1, r6, #2
    4110:	e59f00dc 	ldr	r0, [pc, #220]	; 41f4 <tt_ff_trolling+0x29c>
    4114:	e59f30c8 	ldr	r3, [pc, #200]	; 41e4 <tt_ff_trolling+0x28c>
    4118:	e1a0e00f 	mov	lr, pc
    411c:	e593f000 	ldr	pc, [r3]
    4120:	e59a2000 	ldr	r2, [sl]
    4124:	e1a00007 	mov	r0, r7
    4128:	e1520006 	cmp	r2, r6
    412c:	e59f10c8 	ldr	r1, [pc, #200]	; 41fc <tt_ff_trolling+0x2a4>
    4130:	e3a0300a 	mov	r3, #10	; 0xa
    4134:	e2866001 	add	r6, r6, #1	; 0x1
    4138:	1a000004 	bne	4150 <tt_ff_trolling+0x1f8>
    413c:	e59d2014 	ldr	r2, [sp, #20]
    4140:	e2422004 	sub	r2, r2, #4	; 0x4
    4144:	eb000052 	bl	4294 <tt_cmp_buffer>
    4148:	e3500000 	cmp	r0, #0	; 0x0
    414c:	0a000006 	beq	416c <tt_ff_trolling+0x214>
    4150:	e1a02004 	mov	r2, r4
    4154:	e1a03005 	mov	r3, r5
    4158:	e59f00a0 	ldr	r0, [pc, #160]	; 4200 <tt_ff_trolling+0x2a8>
    415c:	e59f1080 	ldr	r1, [pc, #128]	; 41e4 <tt_ff_trolling+0x28c>
    4160:	e1a0e00f 	mov	lr, pc
    4164:	e591f000 	ldr	pc, [r1]
    4168:	ea00001a 	b	41d8 <tt_ff_trolling+0x280>
    416c:	e0944008 	adds	r4, r4, r8
    4170:	e0a55009 	adc	r5, r5, r9
    4174:	e59d1008 	ldr	r1, [sp, #8]
    4178:	e1a02004 	mov	r2, r4
    417c:	e1510005 	cmp	r1, r5
    4180:	e1a03005 	mov	r3, r5
    4184:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4188:	8affffda 	bhi	40f8 <tt_ff_trolling+0x1a0>
    418c:	1a00000a 	bne	41bc <tt_ff_trolling+0x264>
    4190:	e59d100c 	ldr	r1, [sp, #12]
    4194:	e1510004 	cmp	r1, r4
    4198:	8affffd6 	bhi	40f8 <tt_ff_trolling+0x1a0>
    419c:	ea000006 	b	41bc <tt_ff_trolling+0x264>
    41a0:	e28dd01c 	add	sp, sp, #28	; 0x1c
    41a4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    41a8:	e59f0054 	ldr	r0, [pc, #84]	; 4204 <tt_ff_trolling+0x2ac>
    41ac:	e59f3030 	ldr	r3, [pc, #48]	; 41e4 <tt_ff_trolling+0x28c>
    41b0:	e1a0e00f 	mov	lr, pc
    41b4:	e593f000 	ldr	pc, [r3]
    41b8:	eaffffc0 	b	40c0 <tt_ff_trolling+0x168>
    41bc:	e59f0040 	ldr	r0, [pc, #64]	; 4204 <tt_ff_trolling+0x2ac>
    41c0:	e59f401c 	ldr	r4, [pc, #28]	; 41e4 <tt_ff_trolling+0x28c>
    41c4:	e1a0e00f 	mov	lr, pc
    41c8:	e594f000 	ldr	pc, [r4]
    41cc:	e59f0034 	ldr	r0, [pc, #52]	; 4208 <tt_ff_trolling+0x2b0>
    41d0:	e1a0e00f 	mov	lr, pc
    41d4:	e594f000 	ldr	pc, [r4]
    41d8:	e3a00000 	mov	r0, #0	; 0x0
    41dc:	eaffffef 	b	41a0 <tt_ff_trolling+0x248>
    41e0:	00012ac2 	.word	0x00012ac2
    41e4:	0800d76c 	.word	0x0800d76c
    41e8:	00012adf 	.word	0x00012adf
    41ec:	00012afe 	.word	0x00012afe
    41f0:	00012b1c 	.word	0x00012b1c
    41f4:	00012b34 	.word	0x00012b34
    41f8:	00012b3b 	.word	0x00012b3b
    41fc:	4e000004 	.word	0x4e000004
    4200:	00012b5b 	.word	0x00012b5b
    4204:	00012b6b 	.word	0x00012b6b
    4208:	00012b72 	.word	0x00012b72

0000420c <do_tt_ff>:
    420c:	e92d4030 	push	{r4, r5, lr}
    4210:	e3520001 	cmp	r2, #1	; 0x1
    4214:	e24dd004 	sub	sp, sp, #4	; 0x4
    4218:	e1a05003 	mov	r5, r3
    421c:	ca000005 	bgt	4238 <do_tt_ff+0x2c>
    4220:	e59f0060 	ldr	r0, [pc, #96]	; 4288 <do_tt_ff+0x7c>
    4224:	e59f3060 	ldr	r3, [pc, #96]	; 428c <do_tt_ff+0x80>
    4228:	e1a0e00f 	mov	lr, pc
    422c:	e593f000 	ldr	pc, [r3]
    4230:	e3e00000 	mvn	r0, #0	; 0x0
    4234:	ea000011 	b	4280 <do_tt_ff+0x74>
    4238:	e3520002 	cmp	r2, #2	; 0x2
    423c:	03a04000 	moveq	r4, #0	; 0x0
    4240:	0a000009 	beq	426c <do_tt_ff+0x60>
    4244:	e3a01000 	mov	r1, #0	; 0x0
    4248:	e5930008 	ldr	r0, [r3, #8]
    424c:	e3a0200a 	mov	r2, #10	; 0xa
    4250:	eb0023a8 	bl	d0f8 <simple_strtoul>
    4254:	e1a04000 	mov	r4, r0
    4258:	e1a01004 	mov	r1, r4
    425c:	e59f002c 	ldr	r0, [pc, #44]	; 4290 <do_tt_ff+0x84>
    4260:	e59f3024 	ldr	r3, [pc, #36]	; 428c <do_tt_ff+0x80>
    4264:	e1a0e00f 	mov	lr, pc
    4268:	e593f000 	ldr	pc, [r3]
    426c:	e5950004 	ldr	r0, [r5, #4]
    4270:	e1a03624 	lsr	r3, r4, #12
    4274:	e1a02a04 	lsl	r2, r4, #20
    4278:	ebffff36 	bl	3f58 <tt_ff_trolling>
    427c:	e3a00000 	mov	r0, #0	; 0x0
    4280:	e28dd004 	add	sp, sp, #4	; 0x4
    4284:	e8bd8030 	pop	{r4, r5, pc}
    4288:	00012b8a 	.word	0x00012b8a
    428c:	0800d76c 	.word	0x0800d76c
    4290:	00012bd0 	.word	0x00012bd0

00004294 <tt_cmp_buffer>:
    4294:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4298:	e3a04000 	mov	r4, #0	; 0x0
    429c:	e24dd004 	sub	sp, sp, #4	; 0x4
    42a0:	e1a0a002 	mov	sl, r2
    42a4:	e1a07000 	mov	r7, r0
    42a8:	e1a06001 	mov	r6, r1
    42ac:	e1a08003 	mov	r8, r3
    42b0:	e1a09142 	asr	r9, r2, #2
    42b4:	e1a05004 	mov	r5, r4
    42b8:	e59fb0e4 	ldr	fp, [pc, #228]	; 43a4 <tt_cmp_buffer+0x110>
    42bc:	ea00001f 	b	4340 <tt_cmp_buffer+0xac>
    42c0:	e5970000 	ldr	r0, [r7]
    42c4:	e5961000 	ldr	r1, [r6]
    42c8:	e1500001 	cmp	r0, r1
    42cc:	0a000018 	beq	4334 <tt_cmp_buffer+0xa0>
    42d0:	e5d72000 	ldrb	r2, [r7]
    42d4:	e5d63000 	ldrb	r3, [r6]
    42d8:	e1520003 	cmp	r2, r3
    42dc:	e5d72001 	ldrb	r2, [r7, #1]
    42e0:	e5d63001 	ldrb	r3, [r6, #1]
    42e4:	12855001 	addne	r5, r5, #1	; 0x1
    42e8:	e1520003 	cmp	r2, r3
    42ec:	e5d72002 	ldrb	r2, [r7, #2]
    42f0:	e5d63002 	ldrb	r3, [r6, #2]
    42f4:	12855001 	addne	r5, r5, #1	; 0x1
    42f8:	e1520003 	cmp	r2, r3
    42fc:	e5d72003 	ldrb	r2, [r7, #3]
    4300:	e5d63003 	ldrb	r3, [r6, #3]
    4304:	12855001 	addne	r5, r5, #1	; 0x1
    4308:	e1520003 	cmp	r2, r3
    430c:	12855001 	addne	r5, r5, #1	; 0x1
    4310:	e3580000 	cmp	r8, #0	; 0x0
    4314:	0a000006 	beq	4334 <tt_cmp_buffer+0xa0>
    4318:	e1a02000 	mov	r2, r0
    431c:	e1a03001 	mov	r3, r1
    4320:	e59f0080 	ldr	r0, [pc, #128]	; 43a8 <tt_cmp_buffer+0x114>
    4324:	e1a01104 	lsl	r1, r4, #2
    4328:	e1a0e00f 	mov	lr, pc
    432c:	e59bf000 	ldr	pc, [fp]
    4330:	e2488001 	sub	r8, r8, #1	; 0x1
    4334:	e2844001 	add	r4, r4, #1	; 0x1
    4338:	e2877004 	add	r7, r7, #4	; 0x4
    433c:	e2866004 	add	r6, r6, #4	; 0x4
    4340:	e1540009 	cmp	r4, r9
    4344:	baffffdd 	blt	42c0 <tt_cmp_buffer+0x2c>
    4348:	e3ca9003 	bic	r9, sl, #3	; 0x3
    434c:	e3a04000 	mov	r4, #0	; 0x0
    4350:	e20aa003 	and	sl, sl, #3	; 0x3
    4354:	e59fb048 	ldr	fp, [pc, #72]	; 43a4 <tt_cmp_buffer+0x110>
    4358:	ea00000c 	b	4390 <tt_cmp_buffer+0xfc>
    435c:	e7d72004 	ldrb	r2, [r7, r4]
    4360:	e7d63004 	ldrb	r3, [r6, r4]
    4364:	e1520003 	cmp	r2, r3
    4368:	0a000007 	beq	438c <tt_cmp_buffer+0xf8>
    436c:	e3580000 	cmp	r8, #0	; 0x0
    4370:	e2855001 	add	r5, r5, #1	; 0x1
    4374:	0a000004 	beq	438c <tt_cmp_buffer+0xf8>
    4378:	e0841009 	add	r1, r4, r9
    437c:	e59f0028 	ldr	r0, [pc, #40]	; 43ac <tt_cmp_buffer+0x118>
    4380:	e1a0e00f 	mov	lr, pc
    4384:	e59bf000 	ldr	pc, [fp]
    4388:	e2488001 	sub	r8, r8, #1	; 0x1
    438c:	e2844001 	add	r4, r4, #1	; 0x1
    4390:	e154000a 	cmp	r4, sl
    4394:	bafffff0 	blt	435c <tt_cmp_buffer+0xc8>
    4398:	e1a00005 	mov	r0, r5
    439c:	e28dd004 	add	sp, sp, #4	; 0x4
    43a0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    43a4:	0800d76c 	.word	0x0800d76c
    43a8:	00012bf7 	.word	0x00012bf7
    43ac:	00012c22 	.word	0x00012c22

000043b0 <tt_rnd_seed>:
    43b0:	e92d4030 	push	{r4, r5, lr}
    43b4:	e59f5024 	ldr	r5, [pc, #36]	; 43e0 <tt_rnd_seed+0x30>
    43b8:	e24dd004 	sub	sp, sp, #4	; 0x4
    43bc:	e3a00000 	mov	r0, #0	; 0x0
    43c0:	e5954000 	ldr	r4, [r5]
    43c4:	eb001147 	bl	88e8 <get_timer>
    43c8:	e30f3ffc 	movw	r3, #65532	; 0xfffc
    43cc:	e6e40070 	uxtab	r0, r4, r0
    43d0:	e0003003 	and	r3, r0, r3
    43d4:	e5853000 	str	r3, [r5]
    43d8:	e28dd004 	add	sp, sp, #4	; 0x4
    43dc:	e8bd8030 	pop	{r4, r5, pc}
    43e0:	0800d1d8 	.word	0x0800d1d8

000043e4 <tt_rnd_get_int>:
    43e4:	e59fc094 	ldr	ip, [pc, #148]	; 4480 <tt_rnd_get_int+0x9c>
    43e8:	e3a02101 	mov	r2, #1073741824	; 0x40000000
    43ec:	e59c3000 	ldr	r3, [ip]
    43f0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    43f4:	e19330b2 	ldrh	r3, [r3, r2]
    43f8:	e24dd004 	sub	sp, sp, #4	; 0x4
    43fc:	e3c33003 	bic	r3, r3, #3	; 0x3
    4400:	e6ff3073 	uxth	r3, r3
    4404:	e19340b2 	ldrh	r4, [r3, r2]
    4408:	e58c3000 	str	r3, [ip]
    440c:	e1a07000 	mov	r7, r0
    4410:	e1a06001 	mov	r6, r1
    4414:	ebffffe5 	bl	43b0 <tt_rnd_seed>
    4418:	e1a00004 	mov	r0, r4
    441c:	eb003105 	bl	10838 <__aeabi_ui2d>
    4420:	e3a02000 	mov	r2, #0	; 0x0
    4424:	e59f3058 	ldr	r3, [pc, #88]	; 4484 <tt_rnd_get_int+0xa0>
    4428:	eb0031dd 	bl	10ba4 <__aeabi_ddiv>
    442c:	e1a04000 	mov	r4, r0
    4430:	e0670006 	rsb	r0, r7, r6
    4434:	e1a05001 	mov	r5, r1
    4438:	eb003107 	bl	1085c <__aeabi_i2d>
    443c:	e1a02000 	mov	r2, r0
    4440:	e1a03001 	mov	r3, r1
    4444:	e1a00004 	mov	r0, r4
    4448:	e1a01005 	mov	r1, r5
    444c:	eb003139 	bl	10938 <__aeabi_dmul>
    4450:	e1a04000 	mov	r4, r0
    4454:	e1a00007 	mov	r0, r7
    4458:	e1a05001 	mov	r5, r1
    445c:	eb0030fe 	bl	1085c <__aeabi_i2d>
    4460:	e1a02000 	mov	r2, r0
    4464:	e1a03001 	mov	r3, r1
    4468:	e1a00004 	mov	r0, r4
    446c:	e1a01005 	mov	r1, r5
    4470:	eb003045 	bl	1058c <__adddf3>
    4474:	eb00324b 	bl	10da8 <__aeabi_d2iz>
    4478:	e28dd004 	add	sp, sp, #4	; 0x4
    447c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4480:	0800d1d8 	.word	0x0800d1d8
    4484:	40efffe0 	.word	0x40efffe0

00004488 <tt_rnd_get_data>:
    4488:	e92d4010 	push	{r4, lr}
    448c:	e1a04000 	mov	r4, r0
    4490:	e1a00001 	mov	r0, r1
    4494:	e1a01002 	mov	r1, r2
    4498:	ebffffd1 	bl	43e4 <tt_rnd_get_int>
    449c:	e59f3010 	ldr	r3, [pc, #16]	; 44b4 <tt_rnd_get_data+0x2c>
    44a0:	e5840000 	str	r0, [r4]
    44a4:	e5930000 	ldr	r0, [r3]
    44a8:	e3c00003 	bic	r0, r0, #3	; 0x3
    44ac:	e2800101 	add	r0, r0, #1073741824	; 0x40000000
    44b0:	e8bd8010 	pop	{r4, pc}
    44b4:	0800d1d8 	.word	0x0800d1d8

000044b8 <tt_wire_get_opt>:
    44b8:	e5900008 	ldr	r0, [r0, #8]
    44bc:	e12fff1e 	bx	lr

000044c0 <tt_wire_get_len>:
    44c0:	e590000c 	ldr	r0, [r0, #12]
    44c4:	e12fff1e 	bx	lr

000044c8 <tt_wire_make_data>:
    44c8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    44cc:	e24dd004 	sub	sp, sp, #4	; 0x4
    44d0:	e1a04000 	mov	r4, r0
    44d4:	e1a07002 	mov	r7, r2
    44d8:	e1a06003 	mov	r6, r3
    44dc:	ebfff39a 	bl	134c <memcpy>
    44e0:	e59f3040 	ldr	r3, [pc, #64]	; 4528 <tt_wire_make_data+0x60>
    44e4:	e5846008 	str	r6, [r4, #8]
    44e8:	e5843000 	str	r3, [r4]
    44ec:	e59d3018 	ldr	r3, [sp, #24]
    44f0:	e3a05000 	mov	r5, #0	; 0x0
    44f4:	e584300c 	str	r3, [r4, #12]
    44f8:	e1a01007 	mov	r1, r7
    44fc:	e5845004 	str	r5, [r4, #4]
    4500:	e1a00005 	mov	r0, r5
    4504:	eb00046a 	bl	56b4 <hash_init>
    4508:	e1a01007 	mov	r1, r7
    450c:	e1a00004 	mov	r0, r4
    4510:	eb000442 	bl	5620 <hash_data>
    4514:	e2840004 	add	r0, r4, #4	; 0x4
    4518:	eb000424 	bl	55b0 <hash_value>
    451c:	e1a00005 	mov	r0, r5
    4520:	e28dd004 	add	sp, sp, #4	; 0x4
    4524:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4528:	7766dd80 	.word	0x7766dd80

0000452c <tt_wire_check_valid>:
    452c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4530:	e5903000 	ldr	r3, [r0]
    4534:	e59f2084 	ldr	r2, [pc, #132]	; 45c0 <tt_wire_check_valid+0x94>
    4538:	e24dd00c 	sub	sp, sp, #12	; 0xc
    453c:	e1530002 	cmp	r3, r2
    4540:	e1a04000 	mov	r4, r0
    4544:	e1a06001 	mov	r6, r1
    4548:	e5907004 	ldr	r7, [r0, #4]
    454c:	0a000006 	beq	456c <tt_wire_check_valid+0x40>
    4550:	e1a01003 	mov	r1, r3
    4554:	e59f0068 	ldr	r0, [pc, #104]	; 45c4 <tt_wire_check_valid+0x98>
    4558:	e59f3068 	ldr	r3, [pc, #104]	; 45c8 <tt_wire_check_valid+0x9c>
    455c:	e1a0e00f 	mov	lr, pc
    4560:	e593f000 	ldr	pc, [r3]
    4564:	e3a00000 	mov	r0, #0	; 0x0
    4568:	ea000012 	b	45b8 <tt_wire_check_valid+0x8c>
    456c:	e3a05000 	mov	r5, #0	; 0x0
    4570:	e5805004 	str	r5, [r0, #4]
    4574:	e1a00005 	mov	r0, r5
    4578:	eb00044d 	bl	56b4 <hash_init>
    457c:	e1a01006 	mov	r1, r6
    4580:	e1a00004 	mov	r0, r4
    4584:	eb000425 	bl	5620 <hash_data>
    4588:	e28d0004 	add	r0, sp, #4	; 0x4
    458c:	eb000407 	bl	55b0 <hash_value>
    4590:	e59d1004 	ldr	r1, [sp, #4]
    4594:	e1510007 	cmp	r1, r7
    4598:	03a00001 	moveq	r0, #1	; 0x1
    459c:	0a000005 	beq	45b8 <tt_wire_check_valid+0x8c>
    45a0:	e59f0024 	ldr	r0, [pc, #36]	; 45cc <tt_wire_check_valid+0xa0>
    45a4:	e1a02007 	mov	r2, r7
    45a8:	e59f3018 	ldr	r3, [pc, #24]	; 45c8 <tt_wire_check_valid+0x9c>
    45ac:	e1a0e00f 	mov	lr, pc
    45b0:	e593f000 	ldr	pc, [r3]
    45b4:	e1a00005 	mov	r0, r5
    45b8:	e28dd00c 	add	sp, sp, #12	; 0xc
    45bc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    45c0:	7766dd80 	.word	0x7766dd80
    45c4:	00012c4d 	.word	0x00012c4d
    45c8:	0800d76c 	.word	0x0800d76c
    45cc:	00012c76 	.word	0x00012c76

000045d0 <do_tt_wire>:
    45d0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    45d4:	e3520001 	cmp	r2, #1	; 0x1
    45d8:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    45dc:	e1a04003 	mov	r4, r3
    45e0:	d59f032c 	ldrle	r0, [pc, #812]	; 4914 <do_tt_wire+0x344>
    45e4:	da00002b 	ble	4698 <do_tt_wire+0xc8>
    45e8:	e3a02003 	mov	r2, #3	; 0x3
    45ec:	e5930004 	ldr	r0, [r3, #4]
    45f0:	e59f1320 	ldr	r1, [pc, #800]	; 4918 <do_tt_wire+0x348>
    45f4:	ebfff31b 	bl	1268 <strncmp>
    45f8:	e3500000 	cmp	r0, #0	; 0x0
    45fc:	059f2318 	ldreq	r2, [pc, #792]	; 491c <do_tt_wire+0x34c>
    4600:	0a000006 	beq	4620 <do_tt_wire+0x50>
    4604:	e5940004 	ldr	r0, [r4, #4]
    4608:	e59f1310 	ldr	r1, [pc, #784]	; 4920 <do_tt_wire+0x350>
    460c:	e3a02003 	mov	r2, #3	; 0x3
    4610:	ebfff314 	bl	1268 <strncmp>
    4614:	e3500000 	cmp	r0, #0	; 0x0
    4618:	1a000009 	bne	4644 <do_tt_wire+0x74>
    461c:	e59f2300 	ldr	r2, [pc, #768]	; 4924 <do_tt_wire+0x354>
    4620:	e59f3300 	ldr	r3, [pc, #768]	; 4928 <do_tt_wire+0x358>
    4624:	e3a00000 	mov	r0, #0	; 0x0
    4628:	e5832000 	str	r2, [r3]
    462c:	eb0010ad 	bl	88e8 <get_timer>
    4630:	e59f82f4 	ldr	r8, [pc, #756]	; 492c <do_tt_wire+0x35c>
    4634:	e59f92ec 	ldr	r9, [pc, #748]	; 4928 <do_tt_wire+0x358>
    4638:	e1a0a008 	mov	sl, r8
    463c:	e58d000c 	str	r0, [sp, #12]
    4640:	ea000006 	b	4660 <do_tt_wire+0x90>
    4644:	e5941004 	ldr	r1, [r4, #4]
    4648:	e59f02e0 	ldr	r0, [pc, #736]	; 4930 <do_tt_wire+0x360>
    464c:	e59f32d8 	ldr	r3, [pc, #728]	; 492c <do_tt_wire+0x35c>
    4650:	e1a0e00f 	mov	lr, pc
    4654:	e593f000 	ldr	pc, [r3]
    4658:	e3e00000 	mvn	r0, #0	; 0x0
    465c:	ea0000aa 	b	490c <do_tt_wire+0x33c>
    4660:	e59f02cc 	ldr	r0, [pc, #716]	; 4934 <do_tt_wire+0x364>
    4664:	eb00107b 	bl	8858 <udelay>
    4668:	e59f02c8 	ldr	r0, [pc, #712]	; 4938 <do_tt_wire+0x368>
    466c:	e1a0e00f 	mov	lr, pc
    4670:	e598f000 	ldr	pc, [r8]
    4674:	e5993000 	ldr	r3, [r9]
    4678:	e1a0e00f 	mov	lr, pc
    467c:	e593f000 	ldr	pc, [r3]
    4680:	e3500000 	cmp	r0, #0	; 0x0
    4684:	a3a07000 	movge	r7, #0	; 0x0
    4688:	a3a06020 	movge	r6, #32	; 0x20
    468c:	a59fb2a8 	ldrge	fp, [pc, #680]	; 493c <do_tt_wire+0x36c>
    4690:	aa000004 	bge	46a8 <do_tt_wire+0xd8>
    4694:	e59f02a4 	ldr	r0, [pc, #676]	; 4940 <do_tt_wire+0x370>
    4698:	e59f328c 	ldr	r3, [pc, #652]	; 492c <do_tt_wire+0x35c>
    469c:	e1a0e00f 	mov	lr, pc
    46a0:	e593f000 	ldr	pc, [r3]
    46a4:	eaffffeb 	b	4658 <do_tt_wire+0x88>
    46a8:	e3570000 	cmp	r7, #0	; 0x0
    46ac:	e1a01006 	mov	r1, r6
    46b0:	e59f028c 	ldr	r0, [pc, #652]	; 4944 <do_tt_wire+0x374>
    46b4:	1a00002f 	bne	4778 <do_tt_wire+0x1a8>
    46b8:	e59f0288 	ldr	r0, [pc, #648]	; 4948 <do_tt_wire+0x378>
    46bc:	e1a0e00f 	mov	lr, pc
    46c0:	e598f000 	ldr	pc, [r8]
    46c4:	e1a00007 	mov	r0, r7
    46c8:	eb001086 	bl	88e8 <get_timer>
    46cc:	e1a01006 	mov	r1, r6
    46d0:	e1a04000 	mov	r4, r0
    46d4:	e5993000 	ldr	r3, [r9]
    46d8:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    46dc:	e1a0e00f 	mov	lr, pc
    46e0:	e593f004 	ldr	pc, [r3, #4]
    46e4:	e1a05000 	mov	r5, r0
    46e8:	e1a00004 	mov	r0, r4
    46ec:	eb00107d 	bl	88e8 <get_timer>
    46f0:	e3550000 	cmp	r5, #0	; 0x0
    46f4:	e1a04000 	mov	r4, r0
    46f8:	e1a01006 	mov	r1, r6
    46fc:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4700:	ba00003d 	blt	47fc <do_tt_wire+0x22c>
    4704:	ebffff88 	bl	452c <tt_wire_check_valid>
    4708:	e3500000 	cmp	r0, #0	; 0x0
    470c:	e1a01004 	mov	r1, r4
    4710:	e1a03546 	asr	r3, r6, #10
    4714:	1a000007 	bne	4738 <do_tt_wire+0x168>
    4718:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    471c:	e0000390 	mul	r0, r0, r3
    4720:	eb002efa 	bl	10310 <__aeabi_idiv>
    4724:	e1a01000 	mov	r1, r0
    4728:	e59f021c 	ldr	r0, [pc, #540]	; 494c <do_tt_wire+0x37c>
    472c:	e1a0e00f 	mov	lr, pc
    4730:	e59af000 	ldr	pc, [sl]
    4734:	eaffffc9 	b	4660 <do_tt_wire+0x90>
    4738:	e59fc210 	ldr	ip, [pc, #528]	; 4950 <do_tt_wire+0x380>
    473c:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    4740:	e0000392 	mul	r0, r2, r3
    4744:	e89c000c 	ldm	ip, {r2, r3}
    4748:	e0922006 	adds	r2, r2, r6
    474c:	e0a33fc6 	adc	r3, r3, r6, asr #31
    4750:	e88c000c 	stm	ip, {r2, r3}
    4754:	eb002eed 	bl	10310 <__aeabi_idiv>
    4758:	e1a01000 	mov	r1, r0
    475c:	e59f01f0 	ldr	r0, [pc, #496]	; 4954 <do_tt_wire+0x384>
    4760:	e1a0e00f 	mov	lr, pc
    4764:	e598f000 	ldr	pc, [r8]
    4768:	e3a0344e 	mov	r3, #1308622848	; 0x4e000000
    476c:	e5937008 	ldr	r7, [r3, #8]
    4770:	e593600c 	ldr	r6, [r3, #12]
    4774:	ea000044 	b	488c <do_tt_wire+0x2bc>
    4778:	e3570001 	cmp	r7, #1	; 0x1
    477c:	1a000030 	bne	4844 <do_tt_wire+0x274>
    4780:	e1a0e00f 	mov	lr, pc
    4784:	e598f000 	ldr	pc, [r8]
    4788:	e1a01007 	mov	r1, r7
    478c:	e3a00000 	mov	r0, #0	; 0x0
    4790:	ebffff13 	bl	43e4 <tt_rnd_get_int>
    4794:	e3a01020 	mov	r1, #32	; 0x20
    4798:	e1a07000 	mov	r7, r0
    479c:	e3a02502 	mov	r2, #8388608	; 0x800000
    47a0:	e28d0014 	add	r0, sp, #20	; 0x14
    47a4:	ebffff37 	bl	4488 <tt_rnd_get_data>
    47a8:	e59dc014 	ldr	ip, [sp, #20]
    47ac:	e1a01000 	mov	r1, r0
    47b0:	e1a02006 	mov	r2, r6
    47b4:	e1a03007 	mov	r3, r7
    47b8:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    47bc:	e58dc000 	str	ip, [sp]
    47c0:	ebffff40 	bl	44c8 <tt_wire_make_data>
    47c4:	e3a00000 	mov	r0, #0	; 0x0
    47c8:	eb001046 	bl	88e8 <get_timer>
    47cc:	e1a01006 	mov	r1, r6
    47d0:	e1a05000 	mov	r5, r0
    47d4:	e5993000 	ldr	r3, [r9]
    47d8:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    47dc:	e1a0e00f 	mov	lr, pc
    47e0:	e593f008 	ldr	pc, [r3, #8]
    47e4:	e1a04000 	mov	r4, r0
    47e8:	e1a00005 	mov	r0, r5
    47ec:	eb00103d 	bl	88e8 <get_timer>
    47f0:	e3540000 	cmp	r4, #0	; 0x0
    47f4:	e1a01000 	mov	r1, r0
    47f8:	aa000003 	bge	480c <do_tt_wire+0x23c>
    47fc:	e59f0154 	ldr	r0, [pc, #340]	; 4958 <do_tt_wire+0x388>
    4800:	e1a0e00f 	mov	lr, pc
    4804:	e59af000 	ldr	pc, [sl]
    4808:	eaffff94 	b	4660 <do_tt_wire+0x90>
    480c:	e89b000c 	ldm	fp, {r2, r3}
    4810:	e0922006 	adds	r2, r2, r6
    4814:	e0a33fc6 	adc	r3, r3, r6, asr #31
    4818:	e59d6014 	ldr	r6, [sp, #20]
    481c:	e88b000c 	stm	fp, {r2, r3}
    4820:	e1a00546 	asr	r0, r6, #10
    4824:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    4828:	e0000092 	mul	r0, r2, r0
    482c:	eb002eb7 	bl	10310 <__aeabi_idiv>
    4830:	e1a01000 	mov	r1, r0
    4834:	e59f0120 	ldr	r0, [pc, #288]	; 495c <do_tt_wire+0x38c>
    4838:	e1a0e00f 	mov	lr, pc
    483c:	e598f000 	ldr	pc, [r8]
    4840:	ea000011 	b	488c <do_tt_wire+0x2bc>
    4844:	e3570003 	cmp	r7, #3	; 0x3
    4848:	1a000004 	bne	4860 <do_tt_wire+0x290>
    484c:	e59f010c 	ldr	r0, [pc, #268]	; 4960 <do_tt_wire+0x390>
    4850:	e59f30d4 	ldr	r3, [pc, #212]	; 492c <do_tt_wire+0x35c>
    4854:	e1a0e00f 	mov	lr, pc
    4858:	e593f000 	ldr	pc, [r3]
    485c:	ea000010 	b	48a4 <do_tt_wire+0x2d4>
    4860:	e3570002 	cmp	r7, #2	; 0x2
    4864:	1a000003 	bne	4878 <do_tt_wire+0x2a8>
    4868:	e59f00f4 	ldr	r0, [pc, #244]	; 4964 <do_tt_wire+0x394>
    486c:	e1a0e00f 	mov	lr, pc
    4870:	e598f000 	ldr	pc, [r8]
    4874:	eaffff79 	b	4660 <do_tt_wire+0x90>
    4878:	e59f00e8 	ldr	r0, [pc, #232]	; 4968 <do_tt_wire+0x398>
    487c:	e59f30a8 	ldr	r3, [pc, #168]	; 492c <do_tt_wire+0x35c>
    4880:	e1a0e00f 	mov	lr, pc
    4884:	e593f000 	ldr	pc, [r3]
    4888:	eafffffe 	b	4888 <do_tt_wire+0x2b8>
    488c:	eb002630 	bl	e154 <tstc>
    4890:	e3500000 	cmp	r0, #0	; 0x0
    4894:	0affff83 	beq	46a8 <do_tt_wire+0xd8>
    4898:	eb00262e 	bl	e158 <getc>
    489c:	e3500003 	cmp	r0, #3	; 0x3
    48a0:	1affff80 	bne	46a8 <do_tt_wire+0xd8>
    48a4:	e59d000c 	ldr	r0, [sp, #12]
    48a8:	eb00100e 	bl	88e8 <get_timer>
    48ac:	e59f3088 	ldr	r3, [pc, #136]	; 493c <do_tt_wire+0x36c>
    48b0:	e59f4074 	ldr	r4, [pc, #116]	; 492c <do_tt_wire+0x35c>
    48b4:	e893000a 	ldm	r3, {r1, r3}
    48b8:	e1a01a21 	lsr	r1, r1, #20
    48bc:	e1811603 	orr	r1, r1, r3, lsl #12
    48c0:	e1a05000 	mov	r5, r0
    48c4:	e59f00a0 	ldr	r0, [pc, #160]	; 496c <do_tt_wire+0x39c>
    48c8:	e1a0e00f 	mov	lr, pc
    48cc:	e594f000 	ldr	pc, [r4]
    48d0:	e59f3078 	ldr	r3, [pc, #120]	; 4950 <do_tt_wire+0x380>
    48d4:	e59f0094 	ldr	r0, [pc, #148]	; 4970 <do_tt_wire+0x3a0>
    48d8:	e893000a 	ldm	r3, {r1, r3}
    48dc:	e1a01a21 	lsr	r1, r1, #20
    48e0:	e1811603 	orr	r1, r1, r3, lsl #12
    48e4:	e1a0e00f 	mov	lr, pc
    48e8:	e594f000 	ldr	pc, [r4]
    48ec:	e30e1a60 	movw	r1, #60000	; 0xea60
    48f0:	e1a00005 	mov	r0, r5
    48f4:	eb002e85 	bl	10310 <__aeabi_idiv>
    48f8:	e1a01000 	mov	r1, r0
    48fc:	e59f0070 	ldr	r0, [pc, #112]	; 4974 <do_tt_wire+0x3a4>
    4900:	e1a0e00f 	mov	lr, pc
    4904:	e594f000 	ldr	pc, [r4]
    4908:	e3a00000 	mov	r0, #0	; 0x0
    490c:	e28dd01c 	add	sp, sp, #28	; 0x1c
    4910:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4914:	00012c9f 	.word	0x00012c9f
    4918:	00012ce8 	.word	0x00012ce8
    491c:	00010ed4 	.word	0x00010ed4
    4920:	00012cec 	.word	0x00012cec
    4924:	00010eec 	.word	0x00010eec
    4928:	0800d1f0 	.word	0x0800d1f0
    492c:	0800d76c 	.word	0x0800d76c
    4930:	00012cf0 	.word	0x00012cf0
    4934:	001e8480 	.word	0x001e8480
    4938:	00012d10 	.word	0x00012d10
    493c:	0800d1e8 	.word	0x0800d1e8
    4940:	00012d32 	.word	0x00012d32
    4944:	00012d9f 	.word	0x00012d9f
    4948:	00012d55 	.word	0x00012d55
    494c:	00012d81 	.word	0x00012d81
    4950:	0800d1e0 	.word	0x0800d1e0
    4954:	00012d90 	.word	0x00012d90
    4958:	00012d71 	.word	0x00012d71
    495c:	00012dbb 	.word	0x00012dbb
    4960:	00012dca 	.word	0x00012dca
    4964:	00012ddc 	.word	0x00012ddc
    4968:	00012df5 	.word	0x00012df5
    496c:	00012e22 	.word	0x00012e22
    4970:	00012e3c 	.word	0x00012e3c
    4974:	00012e5a 	.word	0x00012e5a

00004978 <vs_is_assigned>:
    4978:	e59f300c 	ldr	r3, [pc, #12]	; 498c <vs_is_assigned+0x14>
    497c:	e5930000 	ldr	r0, [r3]
    4980:	e2500000 	subs	r0, r0, #0	; 0x0
    4984:	13a00001 	movne	r0, #1	; 0x1
    4988:	e12fff1e 	bx	lr
    498c:	0800d1f8 	.word	0x0800d1f8

00004990 <vs_is_opt_available>:
    4990:	e350000d 	cmp	r0, #13	; 0xd
    4994:	959f2078 	ldrls	r2, [pc, #120]	; 4a14 <vs_is_opt_available+0x84>
    4998:	93a0302c 	movls	r3, #44	; 0x2c
    499c:	859f3074 	ldrhi	r3, [pc, #116]	; 4a18 <vs_is_opt_available+0x88>
    49a0:	90202093 	mlals	r0, r3, r0, r2
    49a4:	85930000 	ldrhi	r0, [r3]
    49a8:	e3500000 	cmp	r0, #0	; 0x0
    49ac:	0a000016 	beq	4a0c <vs_is_opt_available+0x7c>
    49b0:	e3510005 	cmp	r1, #5	; 0x5
    49b4:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    49b8:	ea000013 	b	4a0c <vs_is_opt_available+0x7c>
    49bc:	000049d4 	.word	0x000049d4
    49c0:	000049dc 	.word	0x000049dc
    49c4:	000049e4 	.word	0x000049e4
    49c8:	000049ec 	.word	0x000049ec
    49cc:	000049f4 	.word	0x000049f4
    49d0:	000049fc 	.word	0x000049fc
    49d4:	e590300c 	ldr	r3, [r0, #12]
    49d8:	ea000008 	b	4a00 <vs_is_opt_available+0x70>
    49dc:	e5903010 	ldr	r3, [r0, #16]
    49e0:	ea000006 	b	4a00 <vs_is_opt_available+0x70>
    49e4:	e5903014 	ldr	r3, [r0, #20]
    49e8:	ea000004 	b	4a00 <vs_is_opt_available+0x70>
    49ec:	e5903018 	ldr	r3, [r0, #24]
    49f0:	ea000002 	b	4a00 <vs_is_opt_available+0x70>
    49f4:	e590301c 	ldr	r3, [r0, #28]
    49f8:	ea000000 	b	4a00 <vs_is_opt_available+0x70>
    49fc:	e5903020 	ldr	r3, [r0, #32]
    4a00:	e2530000 	subs	r0, r3, #0	; 0x0
    4a04:	13a00001 	movne	r0, #1	; 0x1
    4a08:	e12fff1e 	bx	lr
    4a0c:	e3a00000 	mov	r0, #0	; 0x0
    4a10:	e12fff1e 	bx	lr
    4a14:	00010eec 	.word	0x00010eec
    4a18:	0800d1f8 	.word	0x0800d1f8

00004a1c <vs_write>:
    4a1c:	e92d4010 	push	{r4, lr}
    4a20:	e24dd008 	sub	sp, sp, #8	; 0x8
    4a24:	e59d4010 	ldr	r4, [sp, #16]
    4a28:	e3540000 	cmp	r4, #0	; 0x0
    4a2c:	01a00004 	moveq	r0, r4
    4a30:	0a00000c 	beq	4a68 <vs_write+0x4c>
    4a34:	e59f1034 	ldr	r1, [pc, #52]	; 4a70 <vs_write+0x54>
    4a38:	e5911000 	ldr	r1, [r1]
    4a3c:	e3510000 	cmp	r1, #0	; 0x0
    4a40:	03e00001 	mvneq	r0, #1	; 0x1
    4a44:	0a000007 	beq	4a68 <vs_write+0x4c>
    4a48:	e591c014 	ldr	ip, [r1, #20]
    4a4c:	e35c0000 	cmp	ip, #0	; 0x0
    4a50:	03e00002 	mvneq	r0, #2	; 0x2
    4a54:	0a000003 	beq	4a68 <vs_write+0x4c>
    4a58:	e59d1014 	ldr	r1, [sp, #20]
    4a5c:	e58d4000 	str	r4, [sp]
    4a60:	e58d1004 	str	r1, [sp, #4]
    4a64:	e12fff3c 	blx	ip
    4a68:	e28dd008 	add	sp, sp, #8	; 0x8
    4a6c:	e8bd8010 	pop	{r4, pc}
    4a70:	0800d1f8 	.word	0x0800d1f8

00004a74 <vs_erase>:
    4a74:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4a78:	e59f302c 	ldr	r3, [pc, #44]	; 4aac <vs_erase+0x38>
    4a7c:	e24dd004 	sub	sp, sp, #4	; 0x4
    4a80:	e5933000 	ldr	r3, [r3]
    4a84:	e3530000 	cmp	r3, #0	; 0x0
    4a88:	03e00001 	mvneq	r0, #1	; 0x1
    4a8c:	0a000004 	beq	4aa4 <vs_erase+0x30>
    4a90:	e593301c 	ldr	r3, [r3, #28]
    4a94:	e3530000 	cmp	r3, #0	; 0x0
    4a98:	03e00002 	mvneq	r0, #2	; 0x2
    4a9c:	0a000000 	beq	4aa4 <vs_erase+0x30>
    4aa0:	e12fff33 	blx	r3
    4aa4:	e28dd004 	add	sp, sp, #4	; 0x4
    4aa8:	e8bd8000 	pop	{pc}
    4aac:	0800d1f8 	.word	0x0800d1f8

00004ab0 <vs_isbad>:
    4ab0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4ab4:	e59f302c 	ldr	r3, [pc, #44]	; 4ae8 <vs_isbad+0x38>
    4ab8:	e24dd004 	sub	sp, sp, #4	; 0x4
    4abc:	e5933000 	ldr	r3, [r3]
    4ac0:	e3530000 	cmp	r3, #0	; 0x0
    4ac4:	03e00001 	mvneq	r0, #1	; 0x1
    4ac8:	0a000004 	beq	4ae0 <vs_isbad+0x30>
    4acc:	e5933024 	ldr	r3, [r3, #36]
    4ad0:	e3530000 	cmp	r3, #0	; 0x0
    4ad4:	03e00002 	mvneq	r0, #2	; 0x2
    4ad8:	0a000000 	beq	4ae0 <vs_isbad+0x30>
    4adc:	e12fff33 	blx	r3
    4ae0:	e28dd004 	add	sp, sp, #4	; 0x4
    4ae4:	e8bd8000 	pop	{pc}
    4ae8:	0800d1f8 	.word	0x0800d1f8

00004aec <vs_scrub>:
    4aec:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4af0:	e59f302c 	ldr	r3, [pc, #44]	; 4b24 <vs_scrub+0x38>
    4af4:	e24dd004 	sub	sp, sp, #4	; 0x4
    4af8:	e5933000 	ldr	r3, [r3]
    4afc:	e3530000 	cmp	r3, #0	; 0x0
    4b00:	03e00001 	mvneq	r0, #1	; 0x1
    4b04:	0a000004 	beq	4b1c <vs_scrub+0x30>
    4b08:	e5933020 	ldr	r3, [r3, #32]
    4b0c:	e3530000 	cmp	r3, #0	; 0x0
    4b10:	03e00002 	mvneq	r0, #2	; 0x2
    4b14:	0a000000 	beq	4b1c <vs_scrub+0x30>
    4b18:	e12fff33 	blx	r3
    4b1c:	e28dd004 	add	sp, sp, #4	; 0x4
    4b20:	e8bd8000 	pop	{pc}
    4b24:	0800d1f8 	.word	0x0800d1f8

00004b28 <vs_special>:
    4b28:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4b2c:	e59f302c 	ldr	r3, [pc, #44]	; 4b60 <vs_special+0x38>
    4b30:	e24dd004 	sub	sp, sp, #4	; 0x4
    4b34:	e5933000 	ldr	r3, [r3]
    4b38:	e3530000 	cmp	r3, #0	; 0x0
    4b3c:	03e00001 	mvneq	r0, #1	; 0x1
    4b40:	0a000004 	beq	4b58 <vs_special+0x30>
    4b44:	e5933028 	ldr	r3, [r3, #40]
    4b48:	e3530000 	cmp	r3, #0	; 0x0
    4b4c:	03e00002 	mvneq	r0, #2	; 0x2
    4b50:	0a000000 	beq	4b58 <vs_special+0x30>
    4b54:	e12fff33 	blx	r3
    4b58:	e28dd004 	add	sp, sp, #4	; 0x4
    4b5c:	e8bd8000 	pop	{pc}
    4b60:	0800d1f8 	.word	0x0800d1f8

00004b64 <vs_align>:
    4b64:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4b68:	e350000e 	cmp	r0, #14	; 0xe
    4b6c:	059f3044 	ldreq	r3, [pc, #68]	; 4bb8 <vs_align+0x54>
    4b70:	e24dd004 	sub	sp, sp, #4	; 0x4
    4b74:	05930000 	ldreq	r0, [r3]
    4b78:	0a000004 	beq	4b90 <vs_align+0x2c>
    4b7c:	e350000d 	cmp	r0, #13	; 0xd
    4b80:	8a000009 	bhi	4bac <vs_align+0x48>
    4b84:	e59f2030 	ldr	r2, [pc, #48]	; 4bbc <vs_align+0x58>
    4b88:	e3a0302c 	mov	r3, #44	; 0x2c
    4b8c:	e0202093 	mla	r0, r3, r0, r2
    4b90:	e3500000 	cmp	r0, #0	; 0x0
    4b94:	0a000004 	beq	4bac <vs_align+0x48>
    4b98:	e590000c 	ldr	r0, [r0, #12]
    4b9c:	e3500000 	cmp	r0, #0	; 0x0
    4ba0:	0a000001 	beq	4bac <vs_align+0x48>
    4ba4:	e12fff30 	blx	r0
    4ba8:	ea000000 	b	4bb0 <vs_align+0x4c>
    4bac:	e3a00000 	mov	r0, #0	; 0x0
    4bb0:	e28dd004 	add	sp, sp, #4	; 0x4
    4bb4:	e8bd8000 	pop	{pc}
    4bb8:	0800d1f8 	.word	0x0800d1f8
    4bbc:	00010eec 	.word	0x00010eec

00004bc0 <vs_device_bootable>:
    4bc0:	e350000e 	cmp	r0, #14	; 0xe
    4bc4:	059f3030 	ldreq	r3, [pc, #48]	; 4bfc <vs_device_bootable+0x3c>
    4bc8:	05930000 	ldreq	r0, [r3]
    4bcc:	0a000004 	beq	4be4 <vs_device_bootable+0x24>
    4bd0:	e350000d 	cmp	r0, #13	; 0xd
    4bd4:	8a000006 	bhi	4bf4 <vs_device_bootable+0x34>
    4bd8:	e59f2020 	ldr	r2, [pc, #32]	; 4c00 <vs_device_bootable+0x40>
    4bdc:	e3a0302c 	mov	r3, #44	; 0x2c
    4be0:	e0202093 	mla	r0, r3, r0, r2
    4be4:	e3500000 	cmp	r0, #0	; 0x0
    4be8:	15903008 	ldrne	r3, [r0, #8]
    4bec:	12030001 	andne	r0, r3, #1	; 0x1
    4bf0:	112fff1e 	bxne	lr
    4bf4:	e3a00000 	mov	r0, #0	; 0x0
    4bf8:	e12fff1e 	bx	lr
    4bfc:	0800d1f8 	.word	0x0800d1f8
    4c00:	00010eec 	.word	0x00010eec

00004c04 <vs_device_burnsrc>:
    4c04:	e350000e 	cmp	r0, #14	; 0xe
    4c08:	059f3034 	ldreq	r3, [pc, #52]	; 4c44 <vs_device_burnsrc+0x40>
    4c0c:	05930000 	ldreq	r0, [r3]
    4c10:	0a000004 	beq	4c28 <vs_device_burnsrc+0x24>
    4c14:	e350000d 	cmp	r0, #13	; 0xd
    4c18:	8a000007 	bhi	4c3c <vs_device_burnsrc+0x38>
    4c1c:	e59f2024 	ldr	r2, [pc, #36]	; 4c48 <vs_device_burnsrc+0x44>
    4c20:	e3a0302c 	mov	r3, #44	; 0x2c
    4c24:	e0202093 	mla	r0, r3, r0, r2
    4c28:	e3500000 	cmp	r0, #0	; 0x0
    4c2c:	15903008 	ldrne	r3, [r0, #8]
    4c30:	11a030a3 	lsrne	r3, r3, #1
    4c34:	12030001 	andne	r0, r3, #1	; 0x1
    4c38:	112fff1e 	bxne	lr
    4c3c:	e3a00000 	mov	r0, #0	; 0x0
    4c40:	e12fff1e 	bx	lr
    4c44:	0800d1f8 	.word	0x0800d1f8
    4c48:	00010eec 	.word	0x00010eec

00004c4c <vs_device_burndst>:
    4c4c:	e350000e 	cmp	r0, #14	; 0xe
    4c50:	059f3034 	ldreq	r3, [pc, #52]	; 4c8c <vs_device_burndst+0x40>
    4c54:	05930000 	ldreq	r0, [r3]
    4c58:	0a000004 	beq	4c70 <vs_device_burndst+0x24>
    4c5c:	e350000d 	cmp	r0, #13	; 0xd
    4c60:	8a000007 	bhi	4c84 <vs_device_burndst+0x38>
    4c64:	e59f2024 	ldr	r2, [pc, #36]	; 4c90 <vs_device_burndst+0x44>
    4c68:	e3a0302c 	mov	r3, #44	; 0x2c
    4c6c:	e0202093 	mla	r0, r3, r0, r2
    4c70:	e3500000 	cmp	r0, #0	; 0x0
    4c74:	15903008 	ldrne	r3, [r0, #8]
    4c78:	11a03123 	lsrne	r3, r3, #2
    4c7c:	12030001 	andne	r0, r3, #1	; 0x1
    4c80:	112fff1e 	bxne	lr
    4c84:	e3a00000 	mov	r0, #0	; 0x0
    4c88:	e12fff1e 	bx	lr
    4c8c:	0800d1f8 	.word	0x0800d1f8
    4c90:	00010eec 	.word	0x00010eec

00004c94 <vs_device_boot0>:
    4c94:	e350000e 	cmp	r0, #14	; 0xe
    4c98:	059f3034 	ldreq	r3, [pc, #52]	; 4cd4 <vs_device_boot0+0x40>
    4c9c:	05930000 	ldreq	r0, [r3]
    4ca0:	0a000004 	beq	4cb8 <vs_device_boot0+0x24>
    4ca4:	e350000d 	cmp	r0, #13	; 0xd
    4ca8:	8a000007 	bhi	4ccc <vs_device_boot0+0x38>
    4cac:	e59f2024 	ldr	r2, [pc, #36]	; 4cd8 <vs_device_boot0+0x44>
    4cb0:	e3a0302c 	mov	r3, #44	; 0x2c
    4cb4:	e0202093 	mla	r0, r3, r0, r2
    4cb8:	e3500000 	cmp	r0, #0	; 0x0
    4cbc:	15903008 	ldrne	r3, [r0, #8]
    4cc0:	11a031a3 	lsrne	r3, r3, #3
    4cc4:	12030001 	andne	r0, r3, #1	; 0x1
    4cc8:	112fff1e 	bxne	lr
    4ccc:	e3a00000 	mov	r0, #0	; 0x0
    4cd0:	e12fff1e 	bx	lr
    4cd4:	0800d1f8 	.word	0x0800d1f8
    4cd8:	00010eec 	.word	0x00010eec

00004cdc <vs_device_name>:
    4cdc:	e350000e 	cmp	r0, #14	; 0xe
    4ce0:	059f3028 	ldreq	r3, [pc, #40]	; 4d10 <vs_device_name+0x34>
    4ce4:	05930000 	ldreq	r0, [r3]
    4ce8:	0a000004 	beq	4d00 <vs_device_name+0x24>
    4cec:	e350000d 	cmp	r0, #13	; 0xd
    4cf0:	8a000004 	bhi	4d08 <vs_device_name+0x2c>
    4cf4:	e59f2018 	ldr	r2, [pc, #24]	; 4d14 <vs_device_name+0x38>
    4cf8:	e3a0302c 	mov	r3, #44	; 0x2c
    4cfc:	e0202093 	mla	r0, r3, r0, r2
    4d00:	e3500000 	cmp	r0, #0	; 0x0
    4d04:	112fff1e 	bxne	lr
    4d08:	e3a00000 	mov	r0, #0	; 0x0
    4d0c:	e12fff1e 	bx	lr
    4d10:	0800d1f8 	.word	0x0800d1f8
    4d14:	00010eec 	.word	0x00010eec

00004d18 <vs_device_erasable>:
    4d18:	e350000e 	cmp	r0, #14	; 0xe
    4d1c:	059f3034 	ldreq	r3, [pc, #52]	; 4d58 <vs_device_erasable+0x40>
    4d20:	05930000 	ldreq	r0, [r3]
    4d24:	0a000004 	beq	4d3c <vs_device_erasable+0x24>
    4d28:	e350000d 	cmp	r0, #13	; 0xd
    4d2c:	8a000007 	bhi	4d50 <vs_device_erasable+0x38>
    4d30:	e59f2024 	ldr	r2, [pc, #36]	; 4d5c <vs_device_erasable+0x44>
    4d34:	e3a0302c 	mov	r3, #44	; 0x2c
    4d38:	e0202093 	mla	r0, r3, r0, r2
    4d3c:	e3500000 	cmp	r0, #0	; 0x0
    4d40:	15903008 	ldrne	r3, [r0, #8]
    4d44:	11a03223 	lsrne	r3, r3, #4
    4d48:	12030001 	andne	r0, r3, #1	; 0x1
    4d4c:	112fff1e 	bxne	lr
    4d50:	e3a00000 	mov	r0, #0	; 0x0
    4d54:	e12fff1e 	bx	lr
    4d58:	0800d1f8 	.word	0x0800d1f8
    4d5c:	00010eec 	.word	0x00010eec

00004d60 <vs_device_count>:
    4d60:	e3a0000e 	mov	r0, #14	; 0xe
    4d64:	e12fff1e 	bx	lr

00004d68 <vs_device_id>:
    4d68:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4d6c:	e59f7040 	ldr	r7, [pc, #64]	; 4db4 <vs_device_id+0x4c>
    4d70:	e24dd004 	sub	sp, sp, #4	; 0x4
    4d74:	e1a05000 	mov	r5, r0
    4d78:	e3a04000 	mov	r4, #0	; 0x0
    4d7c:	e3a0602c 	mov	r6, #44	; 0x2c
    4d80:	e0217496 	mla	r1, r6, r4, r7
    4d84:	e1a00005 	mov	r0, r5
    4d88:	e3a02010 	mov	r2, #16	; 0x10
    4d8c:	ebfff135 	bl	1268 <strncmp>
    4d90:	e3500000 	cmp	r0, #0	; 0x0
    4d94:	0a000003 	beq	4da8 <vs_device_id+0x40>
    4d98:	e2844001 	add	r4, r4, #1	; 0x1
    4d9c:	e354000e 	cmp	r4, #14	; 0xe
    4da0:	1afffff6 	bne	4d80 <vs_device_id+0x18>
    4da4:	e2844001 	add	r4, r4, #1	; 0x1
    4da8:	e1a00004 	mov	r0, r4
    4dac:	e28dd004 	add	sp, sp, #4	; 0x4
    4db0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4db4:	00010eec 	.word	0x00010eec

00004db8 <vs_is_device>:
    4db8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4dbc:	e350000e 	cmp	r0, #14	; 0xe
    4dc0:	059f304c 	ldreq	r3, [pc, #76]	; 4e14 <vs_is_device+0x5c>
    4dc4:	e1a0c001 	mov	ip, r1
    4dc8:	e24dd004 	sub	sp, sp, #4	; 0x4
    4dcc:	05931000 	ldreq	r1, [r3]
    4dd0:	0a000004 	beq	4de8 <vs_is_device+0x30>
    4dd4:	e350000d 	cmp	r0, #13	; 0xd
    4dd8:	8a00000a 	bhi	4e08 <vs_is_device+0x50>
    4ddc:	e59f2034 	ldr	r2, [pc, #52]	; 4e18 <vs_is_device+0x60>
    4de0:	e3a0302c 	mov	r3, #44	; 0x2c
    4de4:	e0212093 	mla	r1, r3, r0, r2
    4de8:	e3510000 	cmp	r1, #0	; 0x0
    4dec:	0a000005 	beq	4e08 <vs_is_device+0x50>
    4df0:	e1a0000c 	mov	r0, ip
    4df4:	e3a02010 	mov	r2, #16	; 0x10
    4df8:	ebfff11a 	bl	1268 <strncmp>
    4dfc:	e2700001 	rsbs	r0, r0, #1	; 0x1
    4e00:	33a00000 	movcc	r0, #0	; 0x0
    4e04:	ea000000 	b	4e0c <vs_is_device+0x54>
    4e08:	e3a00000 	mov	r0, #0	; 0x0
    4e0c:	e28dd004 	add	sp, sp, #4	; 0x4
    4e10:	e8bd8000 	pop	{pc}
    4e14:	0800d1f8 	.word	0x0800d1f8
    4e18:	00010eec 	.word	0x00010eec

00004e1c <vs_read>:
    4e1c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4e20:	e59f4238 	ldr	r4, [pc, #568]	; 5060 <vs_read+0x244>
    4e24:	e1a0b000 	mov	fp, r0
    4e28:	e5940000 	ldr	r0, [r4]
    4e2c:	e24dd024 	sub	sp, sp, #36	; 0x24
    4e30:	e3500000 	cmp	r0, #0	; 0x0
    4e34:	e1a06002 	mov	r6, r2
    4e38:	e1a07003 	mov	r7, r3
    4e3c:	e59d8048 	ldr	r8, [sp, #72]
    4e40:	03e05001 	mvneq	r5, #1	; 0x1
    4e44:	0a000082 	beq	5054 <vs_read+0x238>
    4e48:	e5903010 	ldr	r3, [r0, #16]
    4e4c:	e3530000 	cmp	r3, #0	; 0x0
    4e50:	03e05002 	mvneq	r5, #2	; 0x2
    4e54:	0a00007e 	beq	5054 <vs_read+0x238>
    4e58:	e3580000 	cmp	r8, #0	; 0x0
    4e5c:	01a05008 	moveq	r5, r8
    4e60:	0a00007b 	beq	5054 <vs_read+0x238>
    4e64:	e59f11f8 	ldr	r1, [pc, #504]	; 5064 <vs_read+0x248>
    4e68:	e3a02003 	mov	r2, #3	; 0x3
    4e6c:	ebfff0fd 	bl	1268 <strncmp>
    4e70:	e3500000 	cmp	r0, #0	; 0x0
    4e74:	1a00000a 	bne	4ea4 <vs_read+0x88>
    4e78:	e59d204c 	ldr	r2, [sp, #76]
    4e7c:	e58d8000 	str	r8, [sp]
    4e80:	e58d2004 	str	r2, [sp, #4]
    4e84:	e5941000 	ldr	r1, [r4]
    4e88:	e1a0000b 	mov	r0, fp
    4e8c:	e1a02006 	mov	r2, r6
    4e90:	e1a03007 	mov	r3, r7
    4e94:	e1a0e00f 	mov	lr, pc
    4e98:	e591f010 	ldr	pc, [r1, #16]
    4e9c:	e1a05000 	mov	r5, r0
    4ea0:	ea00006b 	b	5054 <vs_read+0x238>
    4ea4:	e3a0000e 	mov	r0, #14	; 0xe
    4ea8:	ebffff2d 	bl	4b64 <vs_align>
    4eac:	e2402001 	sub	r2, r0, #1	; 0x1
    4eb0:	e0124006 	ands	r4, r2, r6
    4eb4:	10644000 	rsbne	r4, r4, r0
    4eb8:	e0643008 	rsb	r3, r4, r8
    4ebc:	e0032002 	and	r2, r3, r2
    4ec0:	e1929004 	orrs	r9, r2, r4
    4ec4:	e1a0a000 	mov	sl, r0
    4ec8:	e58d3014 	str	r3, [sp, #20]
    4ecc:	e58d200c 	str	r2, [sp, #12]
    4ed0:	e58d9018 	str	r9, [sp, #24]
    4ed4:	01a05009 	moveq	r5, r9
    4ed8:	058d901c 	streq	r9, [sp, #28]
    4edc:	0a00002d 	beq	4f98 <vs_read+0x17c>
    4ee0:	ebfff17e 	bl	14e0 <malloc>
    4ee4:	e2502000 	subs	r2, r0, #0	; 0x0
    4ee8:	e58d201c 	str	r2, [sp, #28]
    4eec:	1a00000c 	bne	4f24 <vs_read+0x108>
    4ef0:	e59f4170 	ldr	r4, [pc, #368]	; 5068 <vs_read+0x24c>
    4ef4:	e59f0170 	ldr	r0, [pc, #368]	; 506c <vs_read+0x250>
    4ef8:	e3001112 	movw	r1, #274	; 0x112
    4efc:	e1a0e00f 	mov	lr, pc
    4f00:	e594f000 	ldr	pc, [r4]
    4f04:	e59f0164 	ldr	r0, [pc, #356]	; 5070 <vs_read+0x254>
    4f08:	e59f1164 	ldr	r1, [pc, #356]	; 5074 <vs_read+0x258>
    4f0c:	e1a0e00f 	mov	lr, pc
    4f10:	e594f000 	ldr	pc, [r4]
    4f14:	e59f015c 	ldr	r0, [pc, #348]	; 5078 <vs_read+0x25c>
    4f18:	e1a0e00f 	mov	lr, pc
    4f1c:	e594f000 	ldr	pc, [r4]
    4f20:	eafffffe 	b	4f20 <vs_read+0x104>
    4f24:	e3540000 	cmp	r4, #0	; 0x0
    4f28:	01a05004 	moveq	r5, r4
    4f2c:	0a000019 	beq	4f98 <vs_read+0x17c>
    4f30:	e59d804c 	ldr	r8, [sp, #76]
    4f34:	e59f2124 	ldr	r2, [pc, #292]	; 5060 <vs_read+0x244>
    4f38:	e26a3000 	rsb	r3, sl, #0	; 0x0
    4f3c:	e5921000 	ldr	r1, [r2]
    4f40:	e3a09000 	mov	r9, #0	; 0x0
    4f44:	e58d8004 	str	r8, [sp, #4]
    4f48:	e0068003 	and	r8, r6, r3
    4f4c:	e58da000 	str	sl, [sp]
    4f50:	e1a02008 	mov	r2, r8
    4f54:	e1a03009 	mov	r3, r9
    4f58:	e1a0e00f 	mov	lr, pc
    4f5c:	e591f010 	ldr	pc, [r1, #16]
    4f60:	e2505000 	subs	r5, r0, #0	; 0x0
    4f64:	ba000038 	blt	504c <vs_read+0x230>
    4f68:	e59d901c 	ldr	r9, [sp, #28]
    4f6c:	e064100a 	rsb	r1, r4, sl
    4f70:	e1a0000b 	mov	r0, fp
    4f74:	e0891001 	add	r1, r9, r1
    4f78:	e1a02004 	mov	r2, r4
    4f7c:	ebfff0f2 	bl	134c <memcpy>
    4f80:	e1550004 	cmp	r5, r4
    4f84:	21a05004 	movcs	r5, r4
    4f88:	e59d8014 	ldr	r8, [sp, #20]
    4f8c:	e0966004 	adds	r6, r6, r4
    4f90:	e2a77000 	adc	r7, r7, #0	; 0x0
    4f94:	e08bb004 	add	fp, fp, r4
    4f98:	e59d200c 	ldr	r2, [sp, #12]
    4f9c:	e0584002 	subs	r4, r8, r2
    4fa0:	0a00000f 	beq	4fe4 <vs_read+0x1c8>
    4fa4:	e59f30b4 	ldr	r3, [pc, #180]	; 5060 <vs_read+0x244>
    4fa8:	e58d4000 	str	r4, [sp]
    4fac:	e5931000 	ldr	r1, [r3]
    4fb0:	e59d304c 	ldr	r3, [sp, #76]
    4fb4:	e1a0000b 	mov	r0, fp
    4fb8:	e58d3004 	str	r3, [sp, #4]
    4fbc:	e1a02006 	mov	r2, r6
    4fc0:	e1a03007 	mov	r3, r7
    4fc4:	e1a0e00f 	mov	lr, pc
    4fc8:	e591f010 	ldr	pc, [r1, #16]
    4fcc:	e3500000 	cmp	r0, #0	; 0x0
    4fd0:	ba000012 	blt	5020 <vs_read+0x204>
    4fd4:	e0966004 	adds	r6, r6, r4
    4fd8:	e0a77fc4 	adc	r7, r7, r4, asr #31
    4fdc:	e0855000 	add	r5, r5, r0
    4fe0:	e08bb004 	add	fp, fp, r4
    4fe4:	e59d800c 	ldr	r8, [sp, #12]
    4fe8:	e3580000 	cmp	r8, #0	; 0x0
    4fec:	0a000013 	beq	5040 <vs_read+0x224>
    4ff0:	e59d904c 	ldr	r9, [sp, #76]
    4ff4:	e59f3064 	ldr	r3, [pc, #100]	; 5060 <vs_read+0x244>
    4ff8:	e58da000 	str	sl, [sp]
    4ffc:	e58d9004 	str	r9, [sp, #4]
    5000:	e5931000 	ldr	r1, [r3]
    5004:	e1a02006 	mov	r2, r6
    5008:	e1a03007 	mov	r3, r7
    500c:	e59d001c 	ldr	r0, [sp, #28]
    5010:	e1a0e00f 	mov	lr, pc
    5014:	e591f010 	ldr	pc, [r1, #16]
    5018:	e3500000 	cmp	r0, #0	; 0x0
    501c:	aa000001 	bge	5028 <vs_read+0x20c>
    5020:	e1a05000 	mov	r5, r0
    5024:	ea000005 	b	5040 <vs_read+0x224>
    5028:	e59d200c 	ldr	r2, [sp, #12]
    502c:	e1a0000b 	mov	r0, fp
    5030:	e59d101c 	ldr	r1, [sp, #28]
    5034:	ebfff0c4 	bl	134c <memcpy>
    5038:	e59d200c 	ldr	r2, [sp, #12]
    503c:	e0855002 	add	r5, r5, r2
    5040:	e59d3018 	ldr	r3, [sp, #24]
    5044:	e3530000 	cmp	r3, #0	; 0x0
    5048:	0a000001 	beq	5054 <vs_read+0x238>
    504c:	e59d001c 	ldr	r0, [sp, #28]
    5050:	ebfff18a 	bl	1680 <free>
    5054:	e1a00005 	mov	r0, r5
    5058:	e28dd024 	add	sp, sp, #36	; 0x24
    505c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5060:	0800d1f8 	.word	0x0800d1f8
    5064:	00012e83 	.word	0x00012e83
    5068:	0800d76c 	.word	0x0800d76c
    506c:	00012e87 	.word	0x00012e87
    5070:	00011d14 	.word	0x00011d14
    5074:	00012e99 	.word	0x00012e99
    5078:	00012ea4 	.word	0x00012ea4

0000507c <vs_reset>:
    507c:	e92d4010 	push	{r4, lr}
    5080:	e59f402c 	ldr	r4, [pc, #44]	; 50b4 <vs_reset+0x38>
    5084:	e5940000 	ldr	r0, [r4]
    5088:	ebffef49 	bl	db4 <cpu_recase>
    508c:	e5944000 	ldr	r4, [r4]
    5090:	e3540000 	cmp	r4, #0	; 0x0
    5094:	03e00001 	mvneq	r0, #1	; 0x1
    5098:	08bd8010 	popeq	{r4, pc}
    509c:	e5944018 	ldr	r4, [r4, #24]
    50a0:	e3540000 	cmp	r4, #0	; 0x0
    50a4:	03e00002 	mvneq	r0, #2	; 0x2
    50a8:	08bd8010 	popeq	{r4, pc}
    50ac:	e12fff34 	blx	r4
    50b0:	e8bd8010 	pop	{r4, pc}
    50b4:	0800d1f8 	.word	0x0800d1f8

000050b8 <vs_assign_by_id>:
    50b8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    50bc:	e59fc05c 	ldr	ip, [pc, #92]	; 5120 <vs_assign_by_id+0x68>
    50c0:	e3a03000 	mov	r3, #0	; 0x0
    50c4:	e350000d 	cmp	r0, #13	; 0xd
    50c8:	e24dd004 	sub	sp, sp, #4	; 0x4
    50cc:	e58c3000 	str	r3, [ip]
    50d0:	8a00000b 	bhi	5104 <vs_assign_by_id+0x4c>
    50d4:	e59f2048 	ldr	r2, [pc, #72]	; 5124 <vs_assign_by_id+0x6c>
    50d8:	e283302c 	add	r3, r3, #44	; 0x2c
    50dc:	e0232093 	mla	r3, r3, r0, r2
    50e0:	e3510000 	cmp	r1, #0	; 0x0
    50e4:	e58c3000 	str	r3, [ip]
    50e8:	01a00001 	moveq	r0, r1
    50ec:	0a000002 	beq	50fc <vs_assign_by_id+0x44>
    50f0:	e28dd004 	add	sp, sp, #4	; 0x4
    50f4:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    50f8:	eaffffdf 	b	507c <vs_reset>
    50fc:	e28dd004 	add	sp, sp, #4	; 0x4
    5100:	e8bd8000 	pop	{pc}
    5104:	e1a01000 	mov	r1, r0
    5108:	e59f3018 	ldr	r3, [pc, #24]	; 5128 <vs_assign_by_id+0x70>
    510c:	e59f0018 	ldr	r0, [pc, #24]	; 512c <vs_assign_by_id+0x74>
    5110:	e1a0e00f 	mov	lr, pc
    5114:	e593f000 	ldr	pc, [r3]
    5118:	e3e00000 	mvn	r0, #0	; 0x0
    511c:	eafffff6 	b	50fc <vs_assign_by_id+0x44>
    5120:	0800d1f8 	.word	0x0800d1f8
    5124:	00010eec 	.word	0x00010eec
    5128:	0800d76c 	.word	0x0800d76c
    512c:	00012ed5 	.word	0x00012ed5

00005130 <vs_assign_by_name>:
    5130:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    5134:	e59f3084 	ldr	r3, [pc, #132]	; 51c0 <vs_assign_by_name+0x90>
    5138:	e3a02000 	mov	r2, #0	; 0x0
    513c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5140:	e5832000 	str	r2, [r3]
    5144:	e1a06000 	mov	r6, r0
    5148:	e1a07001 	mov	r7, r1
    514c:	e1a04002 	mov	r4, r2
    5150:	e3a0a02c 	mov	sl, #44	; 0x2c
    5154:	e59f8068 	ldr	r8, [pc, #104]	; 51c4 <vs_assign_by_name+0x94>
    5158:	ea00000d 	b	5194 <vs_assign_by_name+0x64>
    515c:	e025849a 	mla	r5, sl, r4, r8
    5160:	e1a00005 	mov	r0, r5
    5164:	ebfff03f 	bl	1268 <strncmp>
    5168:	e3500000 	cmp	r0, #0	; 0x0
    516c:	1a000007 	bne	5190 <vs_assign_by_name+0x60>
    5170:	e59f3048 	ldr	r3, [pc, #72]	; 51c0 <vs_assign_by_name+0x90>
    5174:	e3570000 	cmp	r7, #0	; 0x0
    5178:	e5835000 	str	r5, [r3]
    517c:	01a00007 	moveq	r0, r7
    5180:	0a00000c 	beq	51b8 <vs_assign_by_name+0x88>
    5184:	e28dd004 	add	sp, sp, #4	; 0x4
    5188:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    518c:	eaffffba 	b	507c <vs_reset>
    5190:	e2844001 	add	r4, r4, #1	; 0x1
    5194:	e354000e 	cmp	r4, #14	; 0xe
    5198:	e1a01006 	mov	r1, r6
    519c:	e3a02010 	mov	r2, #16	; 0x10
    51a0:	1affffed 	bne	515c <vs_assign_by_name+0x2c>
    51a4:	e59f001c 	ldr	r0, [pc, #28]	; 51c8 <vs_assign_by_name+0x98>
    51a8:	e59f301c 	ldr	r3, [pc, #28]	; 51cc <vs_assign_by_name+0x9c>
    51ac:	e1a0e00f 	mov	lr, pc
    51b0:	e593f000 	ldr	pc, [r3]
    51b4:	e3e00000 	mvn	r0, #0	; 0x0
    51b8:	e28dd004 	add	sp, sp, #4	; 0x4
    51bc:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    51c0:	0800d1f8 	.word	0x0800d1f8
    51c4:	00010eec 	.word	0x00010eec
    51c8:	00012eee 	.word	0x00012eee
    51cc:	0800d76c 	.word	0x0800d76c

000051d0 <ss_lock_isk>:
    51d0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    51d4:	e24dd004 	sub	sp, sp, #4	; 0x4
    51d8:	eb0001d5 	bl	5934 <security_enabled>
    51dc:	e3500000 	cmp	r0, #0	; 0x0
    51e0:	0a000002 	beq	51f0 <ss_lock_isk+0x20>
    51e4:	e28dd004 	add	sp, sp, #4	; 0x4
    51e8:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    51ec:	ea00002e 	b	52ac <block_cipher_lock_isk>
    51f0:	e28dd004 	add	sp, sp, #4	; 0x4
    51f4:	e8bd8000 	pop	{pc}

000051f8 <ss_aes_data>:
    51f8:	e92d4070 	push	{r4, r5, r6, lr}
    51fc:	e1a05000 	mov	r5, r0
    5200:	e1a06001 	mov	r6, r1
    5204:	e1a04002 	mov	r4, r2
    5208:	eb0001c9 	bl	5934 <security_enabled>
    520c:	e3500000 	cmp	r0, #0	; 0x0
    5210:	0a000007 	beq	5234 <ss_aes_data+0x3c>
    5214:	e284200f 	add	r2, r4, #15	; 0xf
    5218:	e1a00005 	mov	r0, r5
    521c:	e1a01006 	mov	r1, r6
    5220:	e3c2200f 	bic	r2, r2, #15	; 0xf
    5224:	eb0000a0 	bl	54ac <block_cipher_operation>
    5228:	e3500000 	cmp	r0, #0	; 0x0
    522c:	13e00000 	mvnne	r0, #0	; 0x0
    5230:	18bd8070 	popne	{r4, r5, r6, pc}
    5234:	e3a00000 	mov	r0, #0	; 0x0
    5238:	e8bd8070 	pop	{r4, r5, r6, pc}

0000523c <ss_aes_init>:
    523c:	e92d4070 	push	{r4, r5, r6, lr}
    5240:	e1a05000 	mov	r5, r0
    5244:	e1a04001 	mov	r4, r1
    5248:	e1a06002 	mov	r6, r2
    524c:	eb0001b8 	bl	5934 <security_enabled>
    5250:	e3500000 	cmp	r0, #0	; 0x0
    5254:	0a00000b 	beq	5288 <ss_aes_init+0x4c>
    5258:	e3540000 	cmp	r4, #0	; 0x0
    525c:	01a00005 	moveq	r0, r5
    5260:	01a02004 	moveq	r2, r4
    5264:	01a03006 	moveq	r3, r6
    5268:	03a01001 	moveq	r1, #1	; 0x1
    526c:	11a00005 	movne	r0, r5
    5270:	11a02004 	movne	r2, r4
    5274:	11a03006 	movne	r3, r6
    5278:	13a01000 	movne	r1, #0	; 0x0
    527c:	eb00002c 	bl	5334 <block_cipher_aes_init>
    5280:	e3500000 	cmp	r0, #0	; 0x0
    5284:	1a000001 	bne	5290 <ss_aes_init+0x54>
    5288:	e3a00000 	mov	r0, #0	; 0x0
    528c:	e8bd8070 	pop	{r4, r5, r6, pc}
    5290:	e3e00000 	mvn	r0, #0	; 0x0
    5294:	e8bd8070 	pop	{r4, r5, r6, pc}

00005298 <check_register_bit>:
    5298:	e3a03001 	mov	r3, #1	; 0x1
    529c:	e0103113 	ands	r3, r0, r3, lsl r1
    52a0:	03a00000 	moveq	r0, #0	; 0x0
    52a4:	13a00001 	movne	r0, #1	; 0x1
    52a8:	e12fff1e 	bx	lr

000052ac <block_cipher_lock_isk>:
    52ac:	e59f200c 	ldr	r2, [pc, #12]	; 52c0 <block_cipher_lock_isk+0x14>
    52b0:	e5923400 	ldr	r3, [r2, #1024]
    52b4:	e3833706 	orr	r3, r3, #1572864	; 0x180000
    52b8:	e5823400 	str	r3, [r2, #1024]
    52bc:	e12fff1e 	bx	lr
    52c0:	27700000 	.word	0x27700000

000052c4 <block_cipher_reset>:
    52c4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    52c8:	e59f4060 	ldr	r4, [pc, #96]	; 5330 <block_cipher_reset+0x6c>
    52cc:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    52d0:	e5843400 	str	r3, [r4, #1024]
    52d4:	e24dd004 	sub	sp, sp, #4	; 0x4
    52d8:	eb000d4d 	bl	8814 <get_ticks>
    52dc:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    52e0:	e3a07000 	mov	r7, #0	; 0x0
    52e4:	e0966000 	adds	r6, r6, r0
    52e8:	e1a05004 	mov	r5, r4
    52ec:	e0a77001 	adc	r7, r7, r1
    52f0:	e5944400 	ldr	r4, [r4, #1024]
    52f4:	ea000008 	b	531c <block_cipher_reset+0x58>
    52f8:	e5954400 	ldr	r4, [r5, #1024]
    52fc:	eb000d44 	bl	8814 <get_ticks>
    5300:	e1510007 	cmp	r1, r7
    5304:	8a000002 	bhi	5314 <block_cipher_reset+0x50>
    5308:	1a000003 	bne	531c <block_cipher_reset+0x58>
    530c:	e1500006 	cmp	r0, r6
    5310:	9a000001 	bls	531c <block_cipher_reset+0x58>
    5314:	e3a00001 	mov	r0, #1	; 0x1
    5318:	ea000002 	b	5328 <block_cipher_reset+0x64>
    531c:	e3540000 	cmp	r4, #0	; 0x0
    5320:	bafffff4 	blt	52f8 <block_cipher_reset+0x34>
    5324:	e3a00000 	mov	r0, #0	; 0x0
    5328:	e28dd004 	add	sp, sp, #4	; 0x4
    532c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5330:	27700000 	.word	0x27700000

00005334 <block_cipher_aes_init>:
    5334:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5338:	e24dd004 	sub	sp, sp, #4	; 0x4
    533c:	e1a05000 	mov	r5, r0
    5340:	e1a04001 	mov	r4, r1
    5344:	e1a06002 	mov	r6, r2
    5348:	e1a07003 	mov	r7, r3
    534c:	ebffffdc 	bl	52c4 <block_cipher_reset>
    5350:	e3500000 	cmp	r0, #0	; 0x0
    5354:	1a00003d 	bne	5450 <block_cipher_aes_init+0x11c>
    5358:	e59f3134 	ldr	r3, [pc, #308]	; 5494 <block_cipher_aes_init+0x160>
    535c:	e3a02001 	mov	r2, #1	; 0x1
    5360:	e3550001 	cmp	r5, #1	; 0x1
    5364:	e5832404 	str	r2, [r3, #1028]
    5368:	8a000038 	bhi	5450 <block_cipher_aes_init+0x11c>
    536c:	e3540003 	cmp	r4, #3	; 0x3
    5370:	13540000 	cmpne	r4, #0	; 0x0
    5374:	0a000001 	beq	5380 <block_cipher_aes_init+0x4c>
    5378:	e1540002 	cmp	r4, r2
    537c:	1a000033 	bne	5450 <block_cipher_aes_init+0x11c>
    5380:	e1a03305 	lsl	r3, r5, #6
    5384:	e3833901 	orr	r3, r3, #16384	; 0x4000
    5388:	e3833004 	orr	r3, r3, #4	; 0x4
    538c:	e59f2100 	ldr	r2, [pc, #256]	; 5494 <block_cipher_aes_init+0x160>
    5390:	e1833984 	orr	r3, r3, r4, lsl #19
    5394:	e3540000 	cmp	r4, #0	; 0x0
    5398:	e5823400 	str	r3, [r2, #1024]
    539c:	1a000008 	bne	53c4 <block_cipher_aes_init+0x90>
    53a0:	e59f10f0 	ldr	r1, [pc, #240]	; 5498 <block_cipher_aes_init+0x164>
    53a4:	e59f00f0 	ldr	r0, [pc, #240]	; 549c <block_cipher_aes_init+0x168>
    53a8:	e59f30f0 	ldr	r3, [pc, #240]	; 54a0 <block_cipher_aes_init+0x16c>
    53ac:	e0862001 	add	r2, r6, r1
    53b0:	e0823003 	add	r3, r2, r3
    53b4:	e5933000 	ldr	r3, [r3]
    53b8:	e4813004 	str	r3, [r1], #4
    53bc:	e1510000 	cmp	r1, r0
    53c0:	1afffff8 	bne	53a8 <block_cipher_aes_init+0x74>
    53c4:	e59f10d0 	ldr	r1, [pc, #208]	; 549c <block_cipher_aes_init+0x168>
    53c8:	e59f00d4 	ldr	r0, [pc, #212]	; 54a4 <block_cipher_aes_init+0x170>
    53cc:	e59f30d4 	ldr	r3, [pc, #212]	; 54a8 <block_cipher_aes_init+0x174>
    53d0:	e0872001 	add	r2, r7, r1
    53d4:	e0823003 	add	r3, r2, r3
    53d8:	e5933000 	ldr	r3, [r3]
    53dc:	e4813004 	str	r3, [r1], #4
    53e0:	e1510000 	cmp	r1, r0
    53e4:	1afffff8 	bne	53cc <block_cipher_aes_init+0x98>
    53e8:	e3540000 	cmp	r4, #0	; 0x0
    53ec:	1a000008 	bne	5414 <block_cipher_aes_init+0xe0>
    53f0:	eb000d07 	bl	8814 <get_ticks>
    53f4:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    53f8:	e3a05000 	mov	r5, #0	; 0x0
    53fc:	e0944000 	adds	r4, r4, r0
    5400:	e59f608c 	ldr	r6, [pc, #140]	; 5494 <block_cipher_aes_init+0x160>
    5404:	e0a55001 	adc	r5, r5, r1
    5408:	e596340c 	ldr	r3, [r6, #1036]
    540c:	e3130008 	tst	r3, #8	; 0x8
    5410:	0a000011 	beq	545c <block_cipher_aes_init+0x128>
    5414:	eb000cfe 	bl	8814 <get_ticks>
    5418:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    541c:	e3a05000 	mov	r5, #0	; 0x0
    5420:	e0944000 	adds	r4, r4, r0
    5424:	e59f6068 	ldr	r6, [pc, #104]	; 5494 <block_cipher_aes_init+0x160>
    5428:	e0a55001 	adc	r5, r5, r1
    542c:	e596340c 	ldr	r3, [r6, #1036]
    5430:	e59f205c 	ldr	r2, [pc, #92]	; 5494 <block_cipher_aes_init+0x160>
    5434:	e3130010 	tst	r3, #16	; 0x10
    5438:	0a00000e 	beq	5478 <block_cipher_aes_init+0x144>
    543c:	e5923400 	ldr	r3, [r2, #1024]
    5440:	e3a00000 	mov	r0, #0	; 0x0
    5444:	e3833001 	orr	r3, r3, #1	; 0x1
    5448:	e5823400 	str	r3, [r2, #1024]
    544c:	ea000000 	b	5454 <block_cipher_aes_init+0x120>
    5450:	e3a00001 	mov	r0, #1	; 0x1
    5454:	e28dd004 	add	sp, sp, #4	; 0x4
    5458:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    545c:	eb000cec 	bl	8814 <get_ticks>
    5460:	e1510005 	cmp	r1, r5
    5464:	8afffff9 	bhi	5450 <block_cipher_aes_init+0x11c>
    5468:	1affffe6 	bne	5408 <block_cipher_aes_init+0xd4>
    546c:	e1500004 	cmp	r0, r4
    5470:	8afffff6 	bhi	5450 <block_cipher_aes_init+0x11c>
    5474:	eaffffe3 	b	5408 <block_cipher_aes_init+0xd4>
    5478:	eb000ce5 	bl	8814 <get_ticks>
    547c:	e1510005 	cmp	r1, r5
    5480:	8afffff2 	bhi	5450 <block_cipher_aes_init+0x11c>
    5484:	1affffe8 	bne	542c <block_cipher_aes_init+0xf8>
    5488:	e1500004 	cmp	r0, r4
    548c:	8affffef 	bhi	5450 <block_cipher_aes_init+0x11c>
    5490:	eaffffe5 	b	542c <block_cipher_aes_init+0xf8>
    5494:	27700000 	.word	0x27700000
    5498:	27700420 	.word	0x27700420
    549c:	27700440 	.word	0x27700440
    54a0:	d88ffbe0 	.word	0xd88ffbe0
    54a4:	27700450 	.word	0x27700450
    54a8:	d88ffbc0 	.word	0xd88ffbc0

000054ac <block_cipher_operation>:
    54ac:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    54b0:	e24dd00c 	sub	sp, sp, #12	; 0xc
    54b4:	e1a08002 	mov	r8, r2
    54b8:	e58d0004 	str	r0, [sp, #4]
    54bc:	e1a09001 	mov	r9, r1
    54c0:	e1a06002 	mov	r6, r2
    54c4:	e59fa0c4 	ldr	sl, [pc, #196]	; 5590 <block_cipher_operation+0xe4>
    54c8:	e3a0b000 	mov	fp, #0	; 0x0
    54cc:	ea000028 	b	5574 <block_cipher_operation+0xc8>
    54d0:	e59d3004 	ldr	r3, [sp, #4]
    54d4:	e3560902 	cmp	r6, #32768	; 0x8000
    54d8:	b1a07006 	movlt	r7, r6
    54dc:	a3a07902 	movge	r7, #32768	; 0x8000
    54e0:	e0664008 	rsb	r4, r6, r8
    54e4:	e3a00000 	mov	r0, #0	; 0x0
    54e8:	e0831004 	add	r1, r3, r4
    54ec:	e1a02007 	mov	r2, r7
    54f0:	eb0000ed 	bl	58ac <dma_sdma_init>
    54f4:	e3500000 	cmp	r0, #0	; 0x0
    54f8:	1a000021 	bne	5584 <block_cipher_operation+0xd8>
    54fc:	e0891004 	add	r1, r9, r4
    5500:	e2800001 	add	r0, r0, #1	; 0x1
    5504:	e1a02007 	mov	r2, r7
    5508:	eb0000e7 	bl	58ac <dma_sdma_init>
    550c:	e3500000 	cmp	r0, #0	; 0x0
    5510:	1a00001b 	bne	5584 <block_cipher_operation+0xd8>
    5514:	eb0000d4 	bl	586c <dma_start>
    5518:	e3a00001 	mov	r0, #1	; 0x1
    551c:	eb0000d2 	bl	586c <dma_start>
    5520:	eb000cbb 	bl	8814 <get_ticks>
    5524:	e090400a 	adds	r4, r0, sl
    5528:	e0a1500b 	adc	r5, r1, fp
    552c:	ea000005 	b	5548 <block_cipher_operation+0x9c>
    5530:	eb000cb7 	bl	8814 <get_ticks>
    5534:	e1510005 	cmp	r1, r5
    5538:	8a000011 	bhi	5584 <block_cipher_operation+0xd8>
    553c:	1a000001 	bne	5548 <block_cipher_operation+0x9c>
    5540:	e1500004 	cmp	r0, r4
    5544:	8a00000e 	bhi	5584 <block_cipher_operation+0xd8>
    5548:	e3a01003 	mov	r1, #3	; 0x3
    554c:	e3a00001 	mov	r0, #1	; 0x1
    5550:	eb0000cb 	bl	5884 <dma_get_status>
    5554:	e3500000 	cmp	r0, #0	; 0x0
    5558:	0afffff4 	beq	5530 <block_cipher_operation+0x84>
    555c:	e3a00000 	mov	r0, #0	; 0x0
    5560:	e3a01003 	mov	r1, #3	; 0x3
    5564:	eb0000c6 	bl	5884 <dma_get_status>
    5568:	e3500000 	cmp	r0, #0	; 0x0
    556c:	0a000004 	beq	5584 <block_cipher_operation+0xd8>
    5570:	e0676006 	rsb	r6, r7, r6
    5574:	e3560000 	cmp	r6, #0	; 0x0
    5578:	1affffd4 	bne	54d0 <block_cipher_operation+0x24>
    557c:	e1a00006 	mov	r0, r6
    5580:	ea000000 	b	5588 <block_cipher_operation+0xdc>
    5584:	e3a00001 	mov	r0, #1	; 0x1
    5588:	e28dd00c 	add	sp, sp, #12	; 0xc
    558c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5590:	000f4240 	.word	0x000f4240

00005594 <hash_len>:
    5594:	e59f300c 	ldr	r3, [pc, #12]	; 55a8 <hash_len+0x14>
    5598:	e5932000 	ldr	r2, [r3]
    559c:	e59f3008 	ldr	r3, [pc, #8]	; 55ac <hash_len+0x18>
    55a0:	e7930202 	ldr	r0, [r3, r2, lsl #4]
    55a4:	e12fff1e 	bx	lr
    55a8:	0800d1fc 	.word	0x0800d1fc
    55ac:	00011154 	.word	0x00011154

000055b0 <hash_value>:
    55b0:	e92d4070 	push	{r4, r5, r6, lr}
    55b4:	e59f6054 	ldr	r6, [pc, #84]	; 5610 <hash_value+0x60>
    55b8:	e1a05000 	mov	r5, r0
    55bc:	e5963000 	ldr	r3, [r6]
    55c0:	e3530000 	cmp	r3, #0	; 0x0
    55c4:	0a000009 	beq	55f0 <hash_value+0x40>
    55c8:	eb0000d9 	bl	5934 <security_enabled>
    55cc:	e2504000 	subs	r4, r0, #0	; 0x0
    55d0:	1a000006 	bne	55f0 <hash_value+0x40>
    55d4:	e59f0038 	ldr	r0, [pc, #56]	; 5614 <hash_value+0x64>
    55d8:	e5961000 	ldr	r1, [r6]
    55dc:	e59f3034 	ldr	r3, [pc, #52]	; 5618 <hash_value+0x68>
    55e0:	e1a0e00f 	mov	lr, pc
    55e4:	e593f000 	ldr	pc, [r3]
    55e8:	e1a00004 	mov	r0, r4
    55ec:	e8bd8070 	pop	{r4, r5, r6, pc}
    55f0:	e59f3018 	ldr	r3, [pc, #24]	; 5610 <hash_value+0x60>
    55f4:	e1a00005 	mov	r0, r5
    55f8:	e5932000 	ldr	r2, [r3]
    55fc:	e59f3018 	ldr	r3, [pc, #24]	; 561c <hash_value+0x6c>
    5600:	e0833202 	add	r3, r3, r2, lsl #4
    5604:	e1a0e00f 	mov	lr, pc
    5608:	e593f00c 	ldr	pc, [r3, #12]
    560c:	e8bd8070 	pop	{r4, r5, r6, pc}
    5610:	0800d1fc 	.word	0x0800d1fc
    5614:	00012f09 	.word	0x00012f09
    5618:	0800d76c 	.word	0x0800d76c
    561c:	00011154 	.word	0x00011154

00005620 <hash_data>:
    5620:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5624:	e59f5074 	ldr	r5, [pc, #116]	; 56a0 <hash_data+0x80>
    5628:	e24dd004 	sub	sp, sp, #4	; 0x4
    562c:	e5953000 	ldr	r3, [r5]
    5630:	e1a06000 	mov	r6, r0
    5634:	e3530000 	cmp	r3, #0	; 0x0
    5638:	e1a07001 	mov	r7, r1
    563c:	0a000009 	beq	5668 <hash_data+0x48>
    5640:	eb0000bb 	bl	5934 <security_enabled>
    5644:	e2504000 	subs	r4, r0, #0	; 0x0
    5648:	1a000006 	bne	5668 <hash_data+0x48>
    564c:	e59f0050 	ldr	r0, [pc, #80]	; 56a4 <hash_data+0x84>
    5650:	e5951000 	ldr	r1, [r5]
    5654:	e59f304c 	ldr	r3, [pc, #76]	; 56a8 <hash_data+0x88>
    5658:	e1a0e00f 	mov	lr, pc
    565c:	e593f000 	ldr	pc, [r3]
    5660:	e1a00004 	mov	r0, r4
    5664:	ea00000b 	b	5698 <hash_data+0x78>
    5668:	e59f003c 	ldr	r0, [pc, #60]	; 56ac <hash_data+0x8c>
    566c:	e59f3034 	ldr	r3, [pc, #52]	; 56a8 <hash_data+0x88>
    5670:	e1a0e00f 	mov	lr, pc
    5674:	e593f000 	ldr	pc, [r3]
    5678:	e59f3020 	ldr	r3, [pc, #32]	; 56a0 <hash_data+0x80>
    567c:	e1a00006 	mov	r0, r6
    5680:	e5932000 	ldr	r2, [r3]
    5684:	e59f3024 	ldr	r3, [pc, #36]	; 56b0 <hash_data+0x90>
    5688:	e1a01007 	mov	r1, r7
    568c:	e0833202 	add	r3, r3, r2, lsl #4
    5690:	e1a0e00f 	mov	lr, pc
    5694:	e593f008 	ldr	pc, [r3, #8]
    5698:	e28dd004 	add	sp, sp, #4	; 0x4
    569c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    56a0:	0800d1fc 	.word	0x0800d1fc
    56a4:	00012f09 	.word	0x00012f09
    56a8:	0800d76c 	.word	0x0800d76c
    56ac:	00012f2a 	.word	0x00012f2a
    56b0:	00011154 	.word	0x00011154

000056b4 <hash_init>:
    56b4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    56b8:	e3500003 	cmp	r0, #3	; 0x3
    56bc:	e24dd004 	sub	sp, sp, #4	; 0x4
    56c0:	e1a04000 	mov	r4, r0
    56c4:	e1a07001 	mov	r7, r1
    56c8:	9a000005 	bls	56e4 <hash_init+0x30>
    56cc:	e59f0064 	ldr	r0, [pc, #100]	; 5738 <hash_init+0x84>
    56d0:	e59f3064 	ldr	r3, [pc, #100]	; 573c <hash_init+0x88>
    56d4:	e1a0e00f 	mov	lr, pc
    56d8:	e593f000 	ldr	pc, [r3]
    56dc:	e3e00000 	mvn	r0, #0	; 0x0
    56e0:	ea000012 	b	5730 <hash_init+0x7c>
    56e4:	e59f6054 	ldr	r6, [pc, #84]	; 5740 <hash_init+0x8c>
    56e8:	e3500000 	cmp	r0, #0	; 0x0
    56ec:	e5860000 	str	r0, [r6]
    56f0:	0a000009 	beq	571c <hash_init+0x68>
    56f4:	eb00008e 	bl	5934 <security_enabled>
    56f8:	e2505000 	subs	r5, r0, #0	; 0x0
    56fc:	1a000006 	bne	571c <hash_init+0x68>
    5700:	e59f003c 	ldr	r0, [pc, #60]	; 5744 <hash_init+0x90>
    5704:	e5961000 	ldr	r1, [r6]
    5708:	e59f302c 	ldr	r3, [pc, #44]	; 573c <hash_init+0x88>
    570c:	e1a0e00f 	mov	lr, pc
    5710:	e593f000 	ldr	pc, [r3]
    5714:	e1a00005 	mov	r0, r5
    5718:	ea000004 	b	5730 <hash_init+0x7c>
    571c:	e59f3024 	ldr	r3, [pc, #36]	; 5748 <hash_init+0x94>
    5720:	e1a00007 	mov	r0, r7
    5724:	e0833204 	add	r3, r3, r4, lsl #4
    5728:	e1a0e00f 	mov	lr, pc
    572c:	e593f004 	ldr	pc, [r3, #4]
    5730:	e28dd004 	add	sp, sp, #4	; 0x4
    5734:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5738:	00012f35 	.word	0x00012f35
    573c:	0800d76c 	.word	0x0800d76c
    5740:	0800d1fc 	.word	0x0800d1fc
    5744:	00012f09 	.word	0x00012f09
    5748:	00011154 	.word	0x00011154

0000574c <md5_value>:
    574c:	e92d4030 	push	{r4, r5, lr}
    5750:	e59f3054 	ldr	r3, [pc, #84]	; 57ac <md5_value+0x60>
    5754:	e1a05000 	mov	r5, r0
    5758:	e893000c 	ldm	r3, {r2, r3}
    575c:	e1823003 	orr	r3, r2, r3
    5760:	e24dd024 	sub	sp, sp, #36	; 0x24
    5764:	e3530000 	cmp	r3, #0	; 0x0
    5768:	13e00000 	mvnne	r0, #0	; 0x0
    576c:	1a00000c 	bne	57a4 <md5_value+0x58>
    5770:	e1a0000d 	mov	r0, sp
    5774:	eb000178 	bl	5d5c <hash_module_get_result>
    5778:	e3500000 	cmp	r0, #0	; 0x0
    577c:	e1a0400d 	mov	r4, sp
    5780:	13e00001 	mvnne	r0, #1	; 0x1
    5784:	1a000006 	bne	57a4 <md5_value+0x58>
    5788:	e1a0200d 	mov	r2, sp
    578c:	e7d23000 	ldrb	r3, [r2, r0]
    5790:	e7c53000 	strb	r3, [r5, r0]
    5794:	e2800001 	add	r0, r0, #1	; 0x1
    5798:	e3500010 	cmp	r0, #16	; 0x10
    579c:	1afffffa 	bne	578c <md5_value+0x40>
    57a0:	e3a00000 	mov	r0, #0	; 0x0
    57a4:	e28dd024 	add	sp, sp, #36	; 0x24
    57a8:	e8bd8030 	pop	{r4, r5, pc}
    57ac:	0800d200 	.word	0x0800d200

000057b0 <md5_data>:
    57b0:	e59f305c 	ldr	r3, [pc, #92]	; 5814 <md5_data+0x64>
    57b4:	e92d4070 	push	{r4, r5, r6, lr}
    57b8:	e1a06001 	mov	r6, r1
    57bc:	e5931004 	ldr	r1, [r3, #4]
    57c0:	e1a04006 	mov	r4, r6
    57c4:	e3a05000 	mov	r5, #0	; 0x0
    57c8:	e1550001 	cmp	r5, r1
    57cc:	e593c000 	ldr	ip, [r3]
    57d0:	8a000002 	bhi	57e0 <md5_data+0x30>
    57d4:	1a000003 	bne	57e8 <md5_data+0x38>
    57d8:	e154000c 	cmp	r4, ip
    57dc:	9a000001 	bls	57e8 <md5_data+0x38>
    57e0:	e3e00000 	mvn	r0, #0	; 0x0
    57e4:	e8bd8070 	pop	{r4, r5, r6, pc}
    57e8:	e1a0200c 	mov	r2, ip
    57ec:	e1a03001 	mov	r3, r1
    57f0:	e59fc01c 	ldr	ip, [pc, #28]	; 5814 <md5_data+0x64>
    57f4:	e0522004 	subs	r2, r2, r4
    57f8:	e0c33005 	sbc	r3, r3, r5
    57fc:	e1a01006 	mov	r1, r6
    5800:	e88c000c 	stm	ip, {r2, r3}
    5804:	eb0001b7 	bl	5ee8 <hash_dma_transfer>
    5808:	e3500000 	cmp	r0, #0	; 0x0
    580c:	13e00001 	mvnne	r0, #1	; 0x1
    5810:	e8bd8070 	pop	{r4, r5, r6, pc}
    5814:	0800d200 	.word	0x0800d200

00005818 <md5_init>:
    5818:	e1a03ea0 	lsr	r3, r0, #29
    581c:	e1a0c180 	lsl	ip, r0, #3
    5820:	e92d4010 	push	{r4, lr}
    5824:	e59f403c 	ldr	r4, [pc, #60]	; 5868 <md5_init+0x50>
    5828:	e1a0200c 	mov	r2, ip
    582c:	e1a01003 	mov	r1, r3
    5830:	e3a00000 	mov	r0, #0	; 0x0
    5834:	e5843004 	str	r3, [r4, #4]
    5838:	e584c000 	str	ip, [r4]
    583c:	eb00018d 	bl	5e78 <hash_module_init>
    5840:	e3500000 	cmp	r0, #0	; 0x0
    5844:	05942000 	ldreq	r2, [r4]
    5848:	05943004 	ldreq	r3, [r4, #4]
    584c:	01a021a2 	lsreq	r2, r2, #3
    5850:	01822e83 	orreq	r2, r2, r3, lsl #29
    5854:	01a031a3 	lsreq	r3, r3, #3
    5858:	13e00000 	mvnne	r0, #0	; 0x0
    585c:	05843004 	streq	r3, [r4, #4]
    5860:	05842000 	streq	r2, [r4]
    5864:	e8bd8010 	pop	{r4, pc}
    5868:	0800d200 	.word	0x0800d200

0000586c <dma_start>:
    586c:	e280099d 	add	r0, r0, #2572288	; 0x274000
    5870:	e2800a03 	add	r0, r0, #12288	; 0x3000
    5874:	e1a00400 	lsl	r0, r0, #8
    5878:	e3a03001 	mov	r3, #1	; 0x1
    587c:	e5803000 	str	r3, [r0]
    5880:	e12fff1e 	bx	lr

00005884 <dma_get_status>:
    5884:	e3a0c001 	mov	ip, #1	; 0x1
    5888:	e1a0211c 	lsl	r2, ip, r1
    588c:	e280099d 	add	r0, r0, #2572288	; 0x274000
    5890:	e2800a03 	add	r0, r0, #12288	; 0x3000
    5894:	e1a01400 	lsl	r1, r0, #8
    5898:	e5913024 	ldr	r3, [r1, #36]
    589c:	e0130002 	ands	r0, r3, r2
    58a0:	15812024 	strne	r2, [r1, #36]
    58a4:	11a0000c 	movne	r0, ip
    58a8:	e12fff1e 	bx	lr

000058ac <dma_sdma_init>:
    58ac:	e280c99d 	add	ip, r0, #2572288	; 0x274000
    58b0:	e28cca03 	add	ip, ip, #12288	; 0x3000
    58b4:	e1a0040c 	lsl	r0, ip, #8
    58b8:	e2800004 	add	r0, r0, #4	; 0x4
    58bc:	e5903000 	ldr	r3, [r0]
    58c0:	e3520801 	cmp	r2, #65536	; 0x10000
    58c4:	e3833001 	orr	r3, r3, #1	; 0x1
    58c8:	e5803000 	str	r3, [r0]
    58cc:	e92d4010 	push	{r4, lr}
    58d0:	e3a03341 	mov	r3, #67108865	; 0x4000001
    58d4:	e1a0440c 	lsl	r4, ip, #8
    58d8:	e584300c 	str	r3, [r4, #12]
    58dc:	31a02802 	lslcc	r2, r2, #16
    58e0:	31a0340c 	lslcc	r3, ip, #8
    58e4:	31a02822 	lsrcc	r2, r2, #16
    58e8:	32833010 	addcc	r3, r3, #16	; 0x10
    58ec:	35832000 	strcc	r2, [r3]
    58f0:	23a00001 	movcs	r0, #1	; 0x1
    58f4:	35841014 	strcc	r1, [r4, #20]
    58f8:	33a00000 	movcc	r0, #0	; 0x0
    58fc:	e8bd8010 	pop	{r4, pc}

00005900 <security_cke_control>:
    5900:	e59fc014 	ldr	ip, [pc, #20]	; 591c <security_cke_control+0x1c>
    5904:	e3a02001 	mov	r2, #1	; 0x1
    5908:	e59c3000 	ldr	r3, [ip]
    590c:	e1c33012 	bic	r3, r3, r2, lsl r0
    5910:	e1833011 	orr	r3, r3, r1, lsl r0
    5914:	e58c3000 	str	r3, [ip]
    5918:	e12fff1e 	bx	lr
    591c:	27702000 	.word	0x27702000

00005920 <security_enable>:
    5920:	e59f3008 	ldr	r3, [pc, #8]	; 5930 <security_enable+0x10>
    5924:	e3a02001 	mov	r2, #1	; 0x1
    5928:	e5832000 	str	r2, [r3]
    592c:	e12fff1e 	bx	lr
    5930:	0800d208 	.word	0x0800d208

00005934 <security_enabled>:
    5934:	e59f300c 	ldr	r3, [pc, #12]	; 5948 <security_enabled+0x14>
    5938:	e5930000 	ldr	r0, [r3]
    593c:	e2500000 	subs	r0, r0, #0	; 0x0
    5940:	13a00001 	movne	r0, #1	; 0x1
    5944:	e12fff1e 	bx	lr
    5948:	0800d208 	.word	0x0800d208

0000594c <dma_reset>:
    594c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5950:	e280099d 	add	r0, r0, #2572288	; 0x274000
    5954:	e2800a03 	add	r0, r0, #12288	; 0x3000
    5958:	e1a07400 	lsl	r7, r0, #8
    595c:	e3a03002 	mov	r3, #2	; 0x2
    5960:	e24dd004 	sub	sp, sp, #4	; 0x4
    5964:	e5873000 	str	r3, [r7]
    5968:	eb000ba9 	bl	8814 <get_ticks>
    596c:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    5970:	e3a05000 	mov	r5, #0	; 0x0
    5974:	e0944000 	adds	r4, r4, r0
    5978:	e0a55001 	adc	r5, r5, r1
    597c:	e5976000 	ldr	r6, [r7]
    5980:	ea000008 	b	59a8 <dma_reset+0x5c>
    5984:	e5976000 	ldr	r6, [r7]
    5988:	eb000ba1 	bl	8814 <get_ticks>
    598c:	e1510005 	cmp	r1, r5
    5990:	8a000002 	bhi	59a0 <dma_reset+0x54>
    5994:	1a000003 	bne	59a8 <dma_reset+0x5c>
    5998:	e1500004 	cmp	r0, r4
    599c:	9a000001 	bls	59a8 <dma_reset+0x5c>
    59a0:	e3a00001 	mov	r0, #1	; 0x1
    59a4:	ea000002 	b	59b4 <dma_reset+0x68>
    59a8:	e3560000 	cmp	r6, #0	; 0x0
    59ac:	1afffff4 	bne	5984 <dma_reset+0x38>
    59b0:	e1a00006 	mov	r0, r6
    59b4:	e28dd004 	add	sp, sp, #4	; 0x4
    59b8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

000059bc <sha1_value>:
    59bc:	e92d4030 	push	{r4, r5, lr}
    59c0:	e59f3054 	ldr	r3, [pc, #84]	; 5a1c <sha1_value+0x60>
    59c4:	e1a05000 	mov	r5, r0
    59c8:	e893000c 	ldm	r3, {r2, r3}
    59cc:	e1823003 	orr	r3, r2, r3
    59d0:	e24dd024 	sub	sp, sp, #36	; 0x24
    59d4:	e3530000 	cmp	r3, #0	; 0x0
    59d8:	13e00000 	mvnne	r0, #0	; 0x0
    59dc:	1a00000c 	bne	5a14 <sha1_value+0x58>
    59e0:	e1a0000d 	mov	r0, sp
    59e4:	eb0000dc 	bl	5d5c <hash_module_get_result>
    59e8:	e3500000 	cmp	r0, #0	; 0x0
    59ec:	e1a0400d 	mov	r4, sp
    59f0:	13e00001 	mvnne	r0, #1	; 0x1
    59f4:	1a000006 	bne	5a14 <sha1_value+0x58>
    59f8:	e1a0200d 	mov	r2, sp
    59fc:	e7d23000 	ldrb	r3, [r2, r0]
    5a00:	e7c53000 	strb	r3, [r5, r0]
    5a04:	e2800001 	add	r0, r0, #1	; 0x1
    5a08:	e3500014 	cmp	r0, #20	; 0x14
    5a0c:	1afffffa 	bne	59fc <sha1_value+0x40>
    5a10:	e3a00000 	mov	r0, #0	; 0x0
    5a14:	e28dd024 	add	sp, sp, #36	; 0x24
    5a18:	e8bd8030 	pop	{r4, r5, pc}
    5a1c:	0800d210 	.word	0x0800d210

00005a20 <sha1_data>:
    5a20:	e59f305c 	ldr	r3, [pc, #92]	; 5a84 <sha1_data+0x64>
    5a24:	e92d4070 	push	{r4, r5, r6, lr}
    5a28:	e1a06001 	mov	r6, r1
    5a2c:	e5931004 	ldr	r1, [r3, #4]
    5a30:	e1a04006 	mov	r4, r6
    5a34:	e3a05000 	mov	r5, #0	; 0x0
    5a38:	e1550001 	cmp	r5, r1
    5a3c:	e593c000 	ldr	ip, [r3]
    5a40:	8a000002 	bhi	5a50 <sha1_data+0x30>
    5a44:	1a000003 	bne	5a58 <sha1_data+0x38>
    5a48:	e154000c 	cmp	r4, ip
    5a4c:	9a000001 	bls	5a58 <sha1_data+0x38>
    5a50:	e3e00000 	mvn	r0, #0	; 0x0
    5a54:	e8bd8070 	pop	{r4, r5, r6, pc}
    5a58:	e1a0200c 	mov	r2, ip
    5a5c:	e1a03001 	mov	r3, r1
    5a60:	e59fc01c 	ldr	ip, [pc, #28]	; 5a84 <sha1_data+0x64>
    5a64:	e0522004 	subs	r2, r2, r4
    5a68:	e0c33005 	sbc	r3, r3, r5
    5a6c:	e1a01006 	mov	r1, r6
    5a70:	e88c000c 	stm	ip, {r2, r3}
    5a74:	eb00011b 	bl	5ee8 <hash_dma_transfer>
    5a78:	e3500000 	cmp	r0, #0	; 0x0
    5a7c:	13e00001 	mvnne	r0, #1	; 0x1
    5a80:	e8bd8070 	pop	{r4, r5, r6, pc}
    5a84:	0800d210 	.word	0x0800d210

00005a88 <sha1_init>:
    5a88:	e1a03ea0 	lsr	r3, r0, #29
    5a8c:	e1a0c180 	lsl	ip, r0, #3
    5a90:	e92d4010 	push	{r4, lr}
    5a94:	e59f403c 	ldr	r4, [pc, #60]	; 5ad8 <sha1_init+0x50>
    5a98:	e1a0200c 	mov	r2, ip
    5a9c:	e1a01003 	mov	r1, r3
    5aa0:	e3a00001 	mov	r0, #1	; 0x1
    5aa4:	e5843004 	str	r3, [r4, #4]
    5aa8:	e584c000 	str	ip, [r4]
    5aac:	eb0000f1 	bl	5e78 <hash_module_init>
    5ab0:	e3500000 	cmp	r0, #0	; 0x0
    5ab4:	05942000 	ldreq	r2, [r4]
    5ab8:	05943004 	ldreq	r3, [r4, #4]
    5abc:	01a021a2 	lsreq	r2, r2, #3
    5ac0:	01822e83 	orreq	r2, r2, r3, lsl #29
    5ac4:	01a031a3 	lsreq	r3, r3, #3
    5ac8:	13e00000 	mvnne	r0, #0	; 0x0
    5acc:	05843004 	streq	r3, [r4, #4]
    5ad0:	05842000 	streq	r2, [r4]
    5ad4:	e8bd8010 	pop	{r4, pc}
    5ad8:	0800d210 	.word	0x0800d210

00005adc <sha256_value>:
    5adc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5ae0:	e59f3028 	ldr	r3, [pc, #40]	; 5b10 <sha256_value+0x34>
    5ae4:	e24dd004 	sub	sp, sp, #4	; 0x4
    5ae8:	e893000c 	ldm	r3, {r2, r3}
    5aec:	e1823003 	orr	r3, r2, r3
    5af0:	e3530000 	cmp	r3, #0	; 0x0
    5af4:	13e00000 	mvnne	r0, #0	; 0x0
    5af8:	1a000002 	bne	5b08 <sha256_value+0x2c>
    5afc:	eb000096 	bl	5d5c <hash_module_get_result>
    5b00:	e3500000 	cmp	r0, #0	; 0x0
    5b04:	13e00001 	mvnne	r0, #1	; 0x1
    5b08:	e28dd004 	add	sp, sp, #4	; 0x4
    5b0c:	e8bd8000 	pop	{pc}
    5b10:	0800d218 	.word	0x0800d218

00005b14 <sha256_data>:
    5b14:	e59f305c 	ldr	r3, [pc, #92]	; 5b78 <sha256_data+0x64>
    5b18:	e92d4070 	push	{r4, r5, r6, lr}
    5b1c:	e1a06001 	mov	r6, r1
    5b20:	e5931004 	ldr	r1, [r3, #4]
    5b24:	e1a04006 	mov	r4, r6
    5b28:	e3a05000 	mov	r5, #0	; 0x0
    5b2c:	e1550001 	cmp	r5, r1
    5b30:	e593c000 	ldr	ip, [r3]
    5b34:	8a000002 	bhi	5b44 <sha256_data+0x30>
    5b38:	1a000003 	bne	5b4c <sha256_data+0x38>
    5b3c:	e154000c 	cmp	r4, ip
    5b40:	9a000001 	bls	5b4c <sha256_data+0x38>
    5b44:	e3e00000 	mvn	r0, #0	; 0x0
    5b48:	e8bd8070 	pop	{r4, r5, r6, pc}
    5b4c:	e1a0200c 	mov	r2, ip
    5b50:	e1a03001 	mov	r3, r1
    5b54:	e59fc01c 	ldr	ip, [pc, #28]	; 5b78 <sha256_data+0x64>
    5b58:	e0522004 	subs	r2, r2, r4
    5b5c:	e0c33005 	sbc	r3, r3, r5
    5b60:	e1a01006 	mov	r1, r6
    5b64:	e88c000c 	stm	ip, {r2, r3}
    5b68:	eb0000de 	bl	5ee8 <hash_dma_transfer>
    5b6c:	e3500000 	cmp	r0, #0	; 0x0
    5b70:	13e00001 	mvnne	r0, #1	; 0x1
    5b74:	e8bd8070 	pop	{r4, r5, r6, pc}
    5b78:	0800d218 	.word	0x0800d218

00005b7c <sha256_init>:
    5b7c:	e1a03ea0 	lsr	r3, r0, #29
    5b80:	e1a0c180 	lsl	ip, r0, #3
    5b84:	e92d4010 	push	{r4, lr}
    5b88:	e59f403c 	ldr	r4, [pc, #60]	; 5bcc <sha256_init+0x50>
    5b8c:	e1a0200c 	mov	r2, ip
    5b90:	e1a01003 	mov	r1, r3
    5b94:	e3a00003 	mov	r0, #3	; 0x3
    5b98:	e5843004 	str	r3, [r4, #4]
    5b9c:	e584c000 	str	ip, [r4]
    5ba0:	eb0000b4 	bl	5e78 <hash_module_init>
    5ba4:	e3500000 	cmp	r0, #0	; 0x0
    5ba8:	05942000 	ldreq	r2, [r4]
    5bac:	05943004 	ldreq	r3, [r4, #4]
    5bb0:	01a021a2 	lsreq	r2, r2, #3
    5bb4:	01822e83 	orreq	r2, r2, r3, lsl #29
    5bb8:	01a031a3 	lsreq	r3, r3, #3
    5bbc:	13e00000 	mvnne	r0, #0	; 0x0
    5bc0:	05843004 	streq	r3, [r4, #4]
    5bc4:	05842000 	streq	r2, [r4]
    5bc8:	e8bd8010 	pop	{r4, pc}
    5bcc:	0800d218 	.word	0x0800d218

00005bd0 <crc32_init>:
    5bd0:	e59f300c 	ldr	r3, [pc, #12]	; 5be4 <crc32_init+0x14>
    5bd4:	e3e02000 	mvn	r2, #0	; 0x0
    5bd8:	e5832000 	str	r2, [r3]
    5bdc:	e3a00000 	mov	r0, #0	; 0x0
    5be0:	e12fff1e 	bx	lr
    5be4:	0800d220 	.word	0x0800d220

00005be8 <crc32_data>:
    5be8:	e59f311c 	ldr	r3, [pc, #284]	; 5d0c <crc32_data+0x124>
    5bec:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    5bf0:	e593c000 	ldr	ip, [r3]
    5bf4:	e1a08000 	mov	r8, r0
    5bf8:	e1a06001 	mov	r6, r1
    5bfc:	e1a04000 	mov	r4, r0
    5c00:	e1a07001 	mov	r7, r1
    5c04:	e59f5104 	ldr	r5, [pc, #260]	; 5d10 <crc32_data+0x128>
    5c08:	ea000027 	b	5cac <crc32_data+0xc4>
    5c0c:	e5543008 	ldrb	r3, [r4, #-8]
    5c10:	e5542007 	ldrb	r2, [r4, #-7]
    5c14:	e02c3003 	eor	r3, ip, r3
    5c18:	e20330ff 	and	r3, r3, #255	; 0xff
    5c1c:	e7953103 	ldr	r3, [r5, r3, lsl #2]
    5c20:	e5541006 	ldrb	r1, [r4, #-6]
    5c24:	e023342c 	eor	r3, r3, ip, lsr #8
    5c28:	e0232002 	eor	r2, r3, r2
    5c2c:	e20220ff 	and	r2, r2, #255	; 0xff
    5c30:	e7952102 	ldr	r2, [r5, r2, lsl #2]
    5c34:	e5540005 	ldrb	r0, [r4, #-5]
    5c38:	e0222423 	eor	r2, r2, r3, lsr #8
    5c3c:	e0221001 	eor	r1, r2, r1
    5c40:	e20110ff 	and	r1, r1, #255	; 0xff
    5c44:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    5c48:	e5541004 	ldrb	r1, [r4, #-4]
    5c4c:	e0233422 	eor	r3, r3, r2, lsr #8
    5c50:	e0230000 	eor	r0, r3, r0
    5c54:	e20000ff 	and	r0, r0, #255	; 0xff
    5c58:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    5c5c:	e5540003 	ldrb	r0, [r4, #-3]
    5c60:	e0222423 	eor	r2, r2, r3, lsr #8
    5c64:	e0221001 	eor	r1, r2, r1
    5c68:	e20110ff 	and	r1, r1, #255	; 0xff
    5c6c:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    5c70:	e554c002 	ldrb	ip, [r4, #-2]
    5c74:	e0233422 	eor	r3, r3, r2, lsr #8
    5c78:	e0230000 	eor	r0, r3, r0
    5c7c:	e20000ff 	and	r0, r0, #255	; 0xff
    5c80:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    5c84:	e5541001 	ldrb	r1, [r4, #-1]
    5c88:	e0222423 	eor	r2, r2, r3, lsr #8
    5c8c:	e022c00c 	eor	ip, r2, ip
    5c90:	e20cc0ff 	and	ip, ip, #255	; 0xff
    5c94:	e795310c 	ldr	r3, [r5, ip, lsl #2]
    5c98:	e0233422 	eor	r3, r3, r2, lsr #8
    5c9c:	e0231001 	eor	r1, r3, r1
    5ca0:	e20110ff 	and	r1, r1, #255	; 0xff
    5ca4:	e7952101 	ldr	r2, [r5, r1, lsl #2]
    5ca8:	e022c423 	eor	ip, r2, r3, lsr #8
    5cac:	e3570007 	cmp	r7, #7	; 0x7
    5cb0:	e2844008 	add	r4, r4, #8	; 0x8
    5cb4:	e2477008 	sub	r7, r7, #8	; 0x8
    5cb8:	8affffd3 	bhi	5c0c <crc32_data+0x24>
    5cbc:	e3c63007 	bic	r3, r6, #7	; 0x7
    5cc0:	e0880003 	add	r0, r8, r3
    5cc4:	e59f3040 	ldr	r3, [pc, #64]	; 5d0c <crc32_data+0x124>
    5cc8:	e2161007 	ands	r1, r6, #7	; 0x7
    5ccc:	e583c000 	str	ip, [r3]
    5cd0:	0a00000b 	beq	5d04 <crc32_data+0x11c>
    5cd4:	e59f4034 	ldr	r4, [pc, #52]	; 5d10 <crc32_data+0x128>
    5cd8:	e3a02000 	mov	r2, #0	; 0x0
    5cdc:	e7d03002 	ldrb	r3, [r0, r2]
    5ce0:	e2822001 	add	r2, r2, #1	; 0x1
    5ce4:	e02c3003 	eor	r3, ip, r3
    5ce8:	e20330ff 	and	r3, r3, #255	; 0xff
    5cec:	e7943103 	ldr	r3, [r4, r3, lsl #2]
    5cf0:	e1510002 	cmp	r1, r2
    5cf4:	e023c42c 	eor	ip, r3, ip, lsr #8
    5cf8:	1afffff7 	bne	5cdc <crc32_data+0xf4>
    5cfc:	e59f3008 	ldr	r3, [pc, #8]	; 5d0c <crc32_data+0x124>
    5d00:	e583c000 	str	ip, [r3]
    5d04:	e3a00000 	mov	r0, #0	; 0x0
    5d08:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5d0c:	0800d220 	.word	0x0800d220
    5d10:	00011194 	.word	0x00011194

00005d14 <crc32_value>:
    5d14:	e59f3010 	ldr	r3, [pc, #16]	; 5d2c <crc32_value+0x18>
    5d18:	e5933000 	ldr	r3, [r3]
    5d1c:	e1e03003 	mvn	r3, r3
    5d20:	e5803000 	str	r3, [r0]
    5d24:	e3a00000 	mov	r0, #0	; 0x0
    5d28:	e12fff1e 	bx	lr
    5d2c:	0800d220 	.word	0x0800d220

00005d30 <crc32_le>:
    5d30:	e92d4010 	push	{r4, lr}
    5d34:	e59f401c 	ldr	r4, [pc, #28]	; 5d58 <crc32_le+0x28>
    5d38:	e3e03000 	mvn	r3, #0	; 0x0
    5d3c:	e1a00001 	mov	r0, r1
    5d40:	e1a01002 	mov	r1, r2
    5d44:	e5843000 	str	r3, [r4]
    5d48:	ebffffa6 	bl	5be8 <crc32_data>
    5d4c:	e5940000 	ldr	r0, [r4]
    5d50:	e1e00000 	mvn	r0, r0
    5d54:	e8bd8010 	pop	{r4, pc}
    5d58:	0800d220 	.word	0x0800d220

00005d5c <hash_module_get_result>:
    5d5c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    5d60:	e1a08000 	mov	r8, r0
    5d64:	eb000aaa 	bl	8814 <get_ticks>
    5d68:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    5d6c:	e59f3088 	ldr	r3, [pc, #136]	; 5dfc <hash_module_get_result+0xa0>
    5d70:	e3a05000 	mov	r5, #0	; 0x0
    5d74:	e0944000 	adds	r4, r4, r0
    5d78:	e0a55001 	adc	r5, r5, r1
    5d7c:	e1a07003 	mov	r7, r3
    5d80:	e593680c 	ldr	r6, [r3, #2060]
    5d84:	ea000008 	b	5dac <hash_module_get_result+0x50>
    5d88:	e597680c 	ldr	r6, [r7, #2060]
    5d8c:	eb000aa0 	bl	8814 <get_ticks>
    5d90:	e1510005 	cmp	r1, r5
    5d94:	8a000002 	bhi	5da4 <hash_module_get_result+0x48>
    5d98:	1a000003 	bne	5dac <hash_module_get_result+0x50>
    5d9c:	e1500004 	cmp	r0, r4
    5da0:	9a000001 	bls	5dac <hash_module_get_result+0x50>
    5da4:	e3a00001 	mov	r0, #1	; 0x1
    5da8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5dac:	e3160001 	tst	r6, #1	; 0x1
    5db0:	0afffff4 	beq	5d88 <hash_module_get_result+0x2c>
    5db4:	e59f0044 	ldr	r0, [pc, #68]	; 5e00 <hash_module_get_result+0xa4>
    5db8:	e59fc044 	ldr	ip, [pc, #68]	; 5e04 <hash_module_get_result+0xa8>
    5dbc:	e1a03000 	mov	r3, r0
    5dc0:	e6931008 	ldr	r1, [r3], r8
    5dc4:	e59f203c 	ldr	r2, [pc, #60]	; 5e08 <hash_module_get_result+0xac>
    5dc8:	e2800004 	add	r0, r0, #4	; 0x4
    5dcc:	e0832002 	add	r2, r3, r2
    5dd0:	e150000c 	cmp	r0, ip
    5dd4:	e5821000 	str	r1, [r2]
    5dd8:	1afffff7 	bne	5dbc <hash_module_get_result+0x60>
    5ddc:	e59f2018 	ldr	r2, [pc, #24]	; 5dfc <hash_module_get_result+0xa0>
    5de0:	e3a03001 	mov	r3, #1	; 0x1
    5de4:	e582380c 	str	r3, [r2, #2060]
    5de8:	e5923800 	ldr	r3, [r2, #2048]
    5dec:	e3a00000 	mov	r0, #0	; 0x0
    5df0:	e3c33080 	bic	r3, r3, #128	; 0x80
    5df4:	e5823800 	str	r3, [r2, #2048]
    5df8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5dfc:	27700000 	.word	0x27700000
    5e00:	27700858 	.word	0x27700858
    5e04:	27700878 	.word	0x27700878
    5e08:	d88ff7a8 	.word	0xd88ff7a8

00005e0c <hash_module_clear>:
    5e0c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5e10:	e59f405c 	ldr	r4, [pc, #92]	; 5e74 <hash_module_clear+0x68>
    5e14:	e3a03002 	mov	r3, #2	; 0x2
    5e18:	e5843800 	str	r3, [r4, #2048]
    5e1c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5e20:	eb000a7b 	bl	8814 <get_ticks>
    5e24:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    5e28:	e3a07000 	mov	r7, #0	; 0x0
    5e2c:	e0966000 	adds	r6, r6, r0
    5e30:	e1a05004 	mov	r5, r4
    5e34:	e0a77001 	adc	r7, r7, r1
    5e38:	e5944800 	ldr	r4, [r4, #2048]
    5e3c:	ea000008 	b	5e64 <hash_module_clear+0x58>
    5e40:	e5954800 	ldr	r4, [r5, #2048]
    5e44:	eb000a72 	bl	8814 <get_ticks>
    5e48:	e1510007 	cmp	r1, r7
    5e4c:	8a000002 	bhi	5e5c <hash_module_clear+0x50>
    5e50:	1a000003 	bne	5e64 <hash_module_clear+0x58>
    5e54:	e1500006 	cmp	r0, r6
    5e58:	9a000001 	bls	5e64 <hash_module_clear+0x58>
    5e5c:	e3a00001 	mov	r0, #1	; 0x1
    5e60:	ea000001 	b	5e6c <hash_module_clear+0x60>
    5e64:	e2140002 	ands	r0, r4, #2	; 0x2
    5e68:	1afffff4 	bne	5e40 <hash_module_clear+0x34>
    5e6c:	e28dd004 	add	sp, sp, #4	; 0x4
    5e70:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5e74:	27700000 	.word	0x27700000

00005e78 <hash_module_init>:
    5e78:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5e7c:	e59f4060 	ldr	r4, [pc, #96]	; 5ee4 <hash_module_init+0x6c>
    5e80:	e24dd004 	sub	sp, sp, #4	; 0x4
    5e84:	e5943800 	ldr	r3, [r4, #2048]
    5e88:	e1a05000 	mov	r5, r0
    5e8c:	e3c33080 	bic	r3, r3, #128	; 0x80
    5e90:	e5843800 	str	r3, [r4, #2048]
    5e94:	e1a07001 	mov	r7, r1
    5e98:	e1a06002 	mov	r6, r2
    5e9c:	ebffffda 	bl	5e0c <hash_module_clear>
    5ea0:	e3500000 	cmp	r0, #0	; 0x0
    5ea4:	13a00001 	movne	r0, #1	; 0x1
    5ea8:	1a00000b 	bne	5edc <hash_module_init+0x64>
    5eac:	e3a03001 	mov	r3, #1	; 0x1
    5eb0:	e1a02185 	lsl	r2, r5, #3
    5eb4:	e5843804 	str	r3, [r4, #2052]
    5eb8:	e5842800 	str	r2, [r4, #2048]
    5ebc:	e5847814 	str	r7, [r4, #2068]
    5ec0:	e5846810 	str	r6, [r4, #2064]
    5ec4:	e5943800 	ldr	r3, [r4, #2048]
    5ec8:	e3833080 	orr	r3, r3, #128	; 0x80
    5ecc:	e5843800 	str	r3, [r4, #2048]
    5ed0:	e5943800 	ldr	r3, [r4, #2048]
    5ed4:	e3833004 	orr	r3, r3, #4	; 0x4
    5ed8:	e5843800 	str	r3, [r4, #2048]
    5edc:	e28dd004 	add	sp, sp, #4	; 0x4
    5ee0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5ee4:	27700000 	.word	0x27700000

00005ee8 <hash_dma_transfer>:
    5ee8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    5eec:	e1a08001 	mov	r8, r1
    5ef0:	e24dd004 	sub	sp, sp, #4	; 0x4
    5ef4:	e1a09000 	mov	r9, r0
    5ef8:	e1a06001 	mov	r6, r1
    5efc:	e59fa090 	ldr	sl, [pc, #144]	; 5f94 <hash_dma_transfer+0xac>
    5f00:	e3a0b000 	mov	fp, #0	; 0x0
    5f04:	ea00001b 	b	5f78 <hash_dma_transfer+0x90>
    5f08:	e3560902 	cmp	r6, #32768	; 0x8000
    5f0c:	b1a07006 	movlt	r7, r6
    5f10:	a3a07902 	movge	r7, #32768	; 0x8000
    5f14:	e0661008 	rsb	r1, r6, r8
    5f18:	e0891001 	add	r1, r9, r1
    5f1c:	e3a00002 	mov	r0, #2	; 0x2
    5f20:	e1a02007 	mov	r2, r7
    5f24:	ebfffe60 	bl	58ac <dma_sdma_init>
    5f28:	e3500000 	cmp	r0, #0	; 0x0
    5f2c:	1a000015 	bne	5f88 <hash_dma_transfer+0xa0>
    5f30:	e2800002 	add	r0, r0, #2	; 0x2
    5f34:	ebfffe4c 	bl	586c <dma_start>
    5f38:	eb000a35 	bl	8814 <get_ticks>
    5f3c:	e090400a 	adds	r4, r0, sl
    5f40:	e0a1500b 	adc	r5, r1, fp
    5f44:	ea000005 	b	5f60 <hash_dma_transfer+0x78>
    5f48:	eb000a31 	bl	8814 <get_ticks>
    5f4c:	e1510005 	cmp	r1, r5
    5f50:	8a00000c 	bhi	5f88 <hash_dma_transfer+0xa0>
    5f54:	1a000001 	bne	5f60 <hash_dma_transfer+0x78>
    5f58:	e1500004 	cmp	r0, r4
    5f5c:	8a000009 	bhi	5f88 <hash_dma_transfer+0xa0>
    5f60:	e3a01003 	mov	r1, #3	; 0x3
    5f64:	e3a00002 	mov	r0, #2	; 0x2
    5f68:	ebfffe45 	bl	5884 <dma_get_status>
    5f6c:	e3500000 	cmp	r0, #0	; 0x0
    5f70:	0afffff4 	beq	5f48 <hash_dma_transfer+0x60>
    5f74:	e0676006 	rsb	r6, r7, r6
    5f78:	e3560000 	cmp	r6, #0	; 0x0
    5f7c:	1affffe1 	bne	5f08 <hash_dma_transfer+0x20>
    5f80:	e1a00006 	mov	r0, r6
    5f84:	ea000000 	b	5f8c <hash_dma_transfer+0xa4>
    5f88:	e3a00001 	mov	r0, #1	; 0x1
    5f8c:	e28dd004 	add	sp, sp, #4	; 0x4
    5f90:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5f94:	000f4240 	.word	0x000f4240

00005f98 <cdbg_log_toggle>:
    5f98:	e59f3014 	ldr	r3, [pc, #20]	; 5fb4 <cdbg_log_toggle+0x1c>
    5f9c:	e3500000 	cmp	r0, #0	; 0x0
    5fa0:	e59f2010 	ldr	r2, [pc, #16]	; 5fb8 <cdbg_log_toggle+0x20>
    5fa4:	01a02003 	moveq	r2, r3
    5fa8:	e59f300c 	ldr	r3, [pc, #12]	; 5fbc <cdbg_log_toggle+0x24>
    5fac:	e5832000 	str	r2, [r3]
    5fb0:	e12fff1e 	bx	lr
    5fb4:	0000e05c 	.word	0x0000e05c
    5fb8:	0000e0e0 	.word	0x0000e0e0
    5fbc:	0800d76c 	.word	0x0800d76c

00005fc0 <cdbg_wipe_bootarea>:
    5fc0:	e59f3018 	ldr	r3, [pc, #24]	; 5fe0 <cdbg_wipe_bootarea+0x20>
    5fc4:	e59f1018 	ldr	r1, [pc, #24]	; 5fe4 <cdbg_wipe_bootarea+0x24>
    5fc8:	e59f2018 	ldr	r2, [pc, #24]	; 5fe8 <cdbg_wipe_bootarea+0x28>
    5fcc:	e5031004 	str	r1, [r3, #-4]
    5fd0:	e2833004 	add	r3, r3, #4	; 0x4
    5fd4:	e1530002 	cmp	r3, r2
    5fd8:	1afffffb 	bne	5fcc <cdbg_wipe_bootarea+0xc>
    5fdc:	e12fff1e 	bx	lr
    5fe0:	08000004 	.word	0x08000004
    5fe4:	eafffffe 	.word	0xeafffffe
    5fe8:	08008004 	.word	0x08008004

00005fec <cdbg_verify_burn>:
    5fec:	e59f3004 	ldr	r3, [pc, #4]	; 5ff8 <cdbg_verify_burn+0xc>
    5ff0:	e5930000 	ldr	r0, [r3]
    5ff4:	e12fff1e 	bx	lr
    5ff8:	0800d224 	.word	0x0800d224

00005ffc <cdbg_verify_burn_key>:
    5ffc:	e59f0000 	ldr	r0, [pc, #0]	; 6004 <cdbg_verify_burn_key+0x8>
    6000:	e12fff1e 	bx	lr
    6004:	0800d228 	.word	0x0800d228

00006008 <cdbg_verify_burn_enable>:
    6008:	e3500000 	cmp	r0, #0	; 0x0
    600c:	e59f3018 	ldr	r3, [pc, #24]	; 602c <cdbg_verify_burn_enable+0x24>
    6010:	03a02000 	moveq	r2, #0	; 0x0
    6014:	13a02001 	movne	r2, #1	; 0x1
    6018:	e5832000 	str	r2, [r3]
    601c:	012fff1e 	bxeq	lr
    6020:	e59f0008 	ldr	r0, [pc, #8]	; 6030 <cdbg_verify_burn_enable+0x28>
    6024:	e3a02020 	mov	r2, #32	; 0x20
    6028:	eaffecc7 	b	134c <memcpy>
    602c:	0800d224 	.word	0x0800d224
    6030:	0800d228 	.word	0x0800d228

00006034 <cdbg_jump>:
    6034:	e92d4030 	push	{r4, r5, lr}
    6038:	e1a04000 	mov	r4, r0
    603c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6040:	e1a05001 	mov	r5, r1
    6044:	ebfffc61 	bl	51d0 <ss_lock_isk>
    6048:	e59f001c 	ldr	r0, [pc, #28]	; 606c <cdbg_jump+0x38>
    604c:	e1a01004 	mov	r1, r4
    6050:	e59f3018 	ldr	r3, [pc, #24]	; 6070 <cdbg_jump+0x3c>
    6054:	e1a0e00f 	mov	lr, pc
    6058:	e593f000 	ldr	pc, [r3]
    605c:	e3000bb8 	movw	r0, #3000	; 0xbb8
    6060:	eb0009fc 	bl	8858 <udelay>
    6064:	e1a00005 	mov	r0, r5
    6068:	e12fff34 	blx	r4
    606c:	00012f76 	.word	0x00012f76
    6070:	0800d76c 	.word	0x0800d76c

00006074 <cdbg_restore_os>:
    6074:	e92d4010 	push	{r4, lr}
    6078:	ebffe9de 	bl	7f8 <get_wakeup_entry>
    607c:	e59f301c 	ldr	r3, [pc, #28]	; 60a0 <cdbg_restore_os+0x2c>
    6080:	e1a04000 	mov	r4, r0
    6084:	e59f0018 	ldr	r0, [pc, #24]	; 60a4 <cdbg_restore_os+0x30>
    6088:	e1a0e00f 	mov	lr, pc
    608c:	e593f000 	ldr	pc, [r3]
    6090:	e1a00004 	mov	r0, r4
    6094:	e3a01000 	mov	r1, #0	; 0x0
    6098:	e8bd4010 	pop	{r4, lr}
    609c:	eaffffe4 	b	6034 <cdbg_jump>
    60a0:	0800d76c 	.word	0x0800d76c
    60a4:	00012f86 	.word	0x00012f86

000060a8 <cdbg_get_pp>:
    60a8:	e92d4070 	push	{r4, r5, r6, lr}
    60ac:	e24dd008 	sub	sp, sp, #8	; 0x8
    60b0:	e1a0000d 	mov	r0, sp
    60b4:	e59f103c 	ldr	r1, [pc, #60]	; 60f8 <cdbg_get_pp+0x50>
    60b8:	e3a02008 	mov	r2, #8	; 0x8
    60bc:	ebffeca2 	bl	134c <memcpy>
    60c0:	e1a0400d 	mov	r4, sp
    60c4:	e1a0600d 	mov	r6, sp
    60c8:	ea000004 	b	60e0 <cdbg_get_pp+0x38>
    60cc:	eb002021 	bl	e158 <getc>
    60d0:	e1500004 	cmp	r0, r4
    60d4:	e2854001 	add	r4, r5, #1	; 0x1
    60d8:	e1a05006 	mov	r5, r6
    60dc:	1a000000 	bne	60e4 <cdbg_get_pp+0x3c>
    60e0:	e1a05004 	mov	r5, r4
    60e4:	e5d54000 	ldrb	r4, [r5]
    60e8:	e3540000 	cmp	r4, #0	; 0x0
    60ec:	1afffff6 	bne	60cc <cdbg_get_pp+0x24>
    60f0:	e28dd008 	add	sp, sp, #8	; 0x8
    60f4:	e8bd8070 	pop	{r4, r5, r6, pc}
    60f8:	0001181e 	.word	0x0001181e

000060fc <cdbg_shell>:
    60fc:	e92d4010 	push	{r4, lr}
    6100:	ebfffc32 	bl	51d0 <ss_lock_isk>
    6104:	e59f3048 	ldr	r3, [pc, #72]	; 6154 <cdbg_shell+0x58>
    6108:	e59f1048 	ldr	r1, [pc, #72]	; 6158 <cdbg_shell+0x5c>
    610c:	e59f2048 	ldr	r2, [pc, #72]	; 615c <cdbg_shell+0x60>
    6110:	e5031004 	str	r1, [r3, #-4]
    6114:	e2833004 	add	r3, r3, #4	; 0x4
    6118:	e1530002 	cmp	r3, r2
    611c:	1afffffb 	bne	6110 <cdbg_shell+0x14>
    6120:	e3a00001 	mov	r0, #1	; 0x1
    6124:	eb0027e5 	bl	100c0 <ss_jtag_en>
    6128:	e59f4030 	ldr	r4, [pc, #48]	; 6160 <cdbg_shell+0x64>
    612c:	e59f3030 	ldr	r3, [pc, #48]	; 6164 <cdbg_shell+0x68>
    6130:	e59f0030 	ldr	r0, [pc, #48]	; 6168 <cdbg_shell+0x6c>
    6134:	e5843000 	str	r3, [r4]
    6138:	eb001fe8 	bl	e0e0 <printf_on>
    613c:	ebffffd9 	bl	60a8 <cdbg_get_pp>
    6140:	e59f0024 	ldr	r0, [pc, #36]	; 616c <cdbg_shell+0x70>
    6144:	e1a0e00f 	mov	lr, pc
    6148:	e594f000 	ldr	pc, [r4]
    614c:	eb0022ec 	bl	ed04 <main_loop>
    6150:	eafffffd 	b	614c <cdbg_shell+0x50>
    6154:	08000004 	.word	0x08000004
    6158:	eafffffe 	.word	0xeafffffe
    615c:	08008004 	.word	0x08008004
    6160:	0800d76c 	.word	0x0800d76c
    6164:	0000e0e0 	.word	0x0000e0e0
    6168:	00012f95 	.word	0x00012f95
    616c:	00012f9a 	.word	0x00012f9a

00006170 <cdbg_do_config>:
    6170:	e92d4030 	push	{r4, r5, lr}
    6174:	e59f307c 	ldr	r3, [pc, #124]	; 61f8 <cdbg_do_config+0x88>
    6178:	e24dd004 	sub	sp, sp, #4	; 0x4
    617c:	e1a04000 	mov	r4, r0
    6180:	e1a05001 	mov	r5, r1
    6184:	e59f0070 	ldr	r0, [pc, #112]	; 61fc <cdbg_do_config+0x8c>
    6188:	e59f1070 	ldr	r1, [pc, #112]	; 6200 <cdbg_do_config+0x90>
    618c:	e1a0e00f 	mov	lr, pc
    6190:	e593f000 	ldr	pc, [r3]
    6194:	e3a02000 	mov	r2, #0	; 0x0
    6198:	e59f0060 	ldr	r0, [pc, #96]	; 6200 <cdbg_do_config+0x90>
    619c:	ea000006 	b	61bc <cdbg_do_config+0x4c>
    61a0:	e7943002 	ldr	r3, [r4, r2]
    61a4:	e2822004 	add	r2, r2, #4	; 0x4
    61a8:	e1530000 	cmp	r3, r0
    61ac:	1a000002 	bne	61bc <cdbg_do_config+0x4c>
    61b0:	e3510000 	cmp	r1, #0	; 0x0
    61b4:	1a000004 	bne	61cc <cdbg_do_config+0x5c>
    61b8:	ea000007 	b	61dc <cdbg_do_config+0x6c>
    61bc:	e1520005 	cmp	r2, r5
    61c0:	e0841002 	add	r1, r4, r2
    61c4:	bafffff5 	blt	61a0 <cdbg_do_config+0x30>
    61c8:	ea000003 	b	61dc <cdbg_do_config+0x6c>
    61cc:	e5910004 	ldr	r0, [r1, #4]
    61d0:	eb000c10 	bl	9218 <spi_set_bytes>
    61d4:	e3a00000 	mov	r0, #0	; 0x0
    61d8:	ea000004 	b	61f0 <cdbg_do_config+0x80>
    61dc:	e59f0020 	ldr	r0, [pc, #32]	; 6204 <cdbg_do_config+0x94>
    61e0:	e59f3010 	ldr	r3, [pc, #16]	; 61f8 <cdbg_do_config+0x88>
    61e4:	e1a0e00f 	mov	lr, pc
    61e8:	e593f000 	ldr	pc, [r3]
    61ec:	e3e00000 	mvn	r0, #0	; 0x0
    61f0:	e28dd004 	add	sp, sp, #4	; 0x4
    61f4:	e8bd8030 	pop	{r4, r5, pc}
    61f8:	0800d76c 	.word	0x0800d76c
    61fc:	00012fab 	.word	0x00012fab
    6200:	4a82467c 	.word	0x4a82467c
    6204:	00012fc8 	.word	0x00012fc8

00006208 <cdbg_config_isi>:
    6208:	e92d4030 	push	{r4, r5, lr}
    620c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6210:	e1a04000 	mov	r4, r0
    6214:	ebffee0f 	bl	1a58 <isi_check>
    6218:	e2505000 	subs	r5, r0, #0	; 0x0
    621c:	0a000007 	beq	6240 <cdbg_config_isi+0x38>
    6220:	e59f002c 	ldr	r0, [pc, #44]	; 6254 <cdbg_config_isi+0x4c>
    6224:	e1a01005 	mov	r1, r5
    6228:	e59f3028 	ldr	r3, [pc, #40]	; 6258 <cdbg_config_isi+0x50>
    622c:	e1a0e00f 	mov	lr, pc
    6230:	e593f000 	ldr	pc, [r3]
    6234:	e1a00005 	mov	r0, r5
    6238:	e28dd004 	add	sp, sp, #4	; 0x4
    623c:	e8bd8030 	pop	{r4, r5, pc}
    6240:	e594100c 	ldr	r1, [r4, #12]
    6244:	e2840c02 	add	r0, r4, #512	; 0x200
    6248:	e28dd004 	add	sp, sp, #4	; 0x4
    624c:	e8bd4030 	pop	{r4, r5, lr}
    6250:	eaffffc6 	b	6170 <cdbg_do_config>
    6254:	00012fe7 	.word	0x00012fe7
    6258:	0800d76c 	.word	0x0800d76c

0000625c <burn_image_loop>:
    625c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6260:	e24dd024 	sub	sp, sp, #36	; 0x24
    6264:	e59d704c 	ldr	r7, [sp, #76]
    6268:	e1a06000 	mov	r6, r0
    626c:	e3570000 	cmp	r7, #0	; 0x0
    6270:	e58d1014 	str	r1, [sp, #20]
    6274:	e58d2010 	str	r2, [sp, #16]
    6278:	e58d300c 	str	r3, [sp, #12]
    627c:	1a00000c 	bne	62b4 <burn_image_loop+0x58>
    6280:	e59f426c 	ldr	r4, [pc, #620]	; 64f4 <burn_image_loop+0x298>
    6284:	e59f026c 	ldr	r0, [pc, #620]	; 64f8 <burn_image_loop+0x29c>
    6288:	e3a0102e 	mov	r1, #46	; 0x2e
    628c:	e1a0e00f 	mov	lr, pc
    6290:	e594f000 	ldr	pc, [r4]
    6294:	e59f0260 	ldr	r0, [pc, #608]	; 64fc <burn_image_loop+0x2a0>
    6298:	e59f1260 	ldr	r1, [pc, #608]	; 6500 <burn_image_loop+0x2a4>
    629c:	e1a0e00f 	mov	lr, pc
    62a0:	e594f000 	ldr	pc, [r4]
    62a4:	e59f0258 	ldr	r0, [pc, #600]	; 6504 <burn_image_loop+0x2a8>
    62a8:	e1a0e00f 	mov	lr, pc
    62ac:	e594f000 	ldr	pc, [r4]
    62b0:	eafffffe 	b	62b0 <burn_image_loop+0x54>
    62b4:	e59d0014 	ldr	r0, [sp, #20]
    62b8:	e59d100c 	ldr	r1, [sp, #12]
    62bc:	e1500001 	cmp	r0, r1
    62c0:	1a000005 	bne	62dc <burn_image_loop+0x80>
    62c4:	e59f023c 	ldr	r0, [pc, #572]	; 6508 <burn_image_loop+0x2ac>
    62c8:	e59f3224 	ldr	r3, [pc, #548]	; 64f4 <burn_image_loop+0x298>
    62cc:	e1a0e00f 	mov	lr, pc
    62d0:	e593f000 	ldr	pc, [r3]
    62d4:	e3e00001 	mvn	r0, #1	; 0x1
    62d8:	ea000083 	b	64ec <burn_image_loop+0x290>
    62dc:	e596c000 	ldr	ip, [r6]
    62e0:	e3a03000 	mov	r3, #0	; 0x0
    62e4:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    62e8:	e002200c 	and	r2, r2, ip
    62ec:	e1a01483 	lsl	r1, r3, #9
    62f0:	e1d600b8 	ldrh	r0, [r6, #8]
    62f4:	e1811ba2 	orr	r1, r1, r2, lsr #23
    62f8:	e1a0e482 	lsl	lr, r2, #9
    62fc:	e1a03001 	mov	r3, r1
    6300:	e3a02401 	mov	r2, #16777216	; 0x1000000
    6304:	e5961004 	ldr	r1, [r6, #4]
    6308:	e1a0cc2c 	lsr	ip, ip, #24
    630c:	e1a05000 	mov	r5, r0
    6310:	e09e8002 	adds	r8, lr, r2
    6314:	e1a09003 	mov	r9, r3
    6318:	e3a04000 	mov	r4, #0	; 0x0
    631c:	e3a03000 	mov	r3, #0	; 0x0
    6320:	e20cc03f 	and	ip, ip, #63	; 0x3f
    6324:	e59d000c 	ldr	r0, [sp, #12]
    6328:	e0a99003 	adc	r9, r9, r3
    632c:	e184a001 	orr	sl, r4, r1
    6330:	e1a0b005 	mov	fp, r5
    6334:	e58dc018 	str	ip, [sp, #24]
    6338:	e596500c 	ldr	r5, [r6, #12]
    633c:	ebfffa75 	bl	4d18 <vs_device_erasable>
    6340:	e1500004 	cmp	r0, r4
    6344:	0a000006 	beq	6364 <burn_image_loop+0x108>
    6348:	e1a01004 	mov	r1, r4
    634c:	e59d000c 	ldr	r0, [sp, #12]
    6350:	ebfffb58 	bl	50b8 <vs_assign_by_id>
    6354:	e1a0000a 	mov	r0, sl
    6358:	e1a0100b 	mov	r1, fp
    635c:	e59d2048 	ldr	r2, [sp, #72]
    6360:	ebfff9c3 	bl	4a74 <vs_erase>
    6364:	e59d0014 	ldr	r0, [sp, #20]
    6368:	e3a01001 	mov	r1, #1	; 0x1
    636c:	ebfffb51 	bl	50b8 <vs_assign_by_id>
    6370:	e59d2018 	ldr	r2, [sp, #24]
    6374:	e3520002 	cmp	r2, #2	; 0x2
    6378:	1a000019 	bne	63e4 <burn_image_loop+0x188>
    637c:	e3a01c02 	mov	r1, #512	; 0x200
    6380:	e58d1000 	str	r1, [sp]
    6384:	e59d1010 	ldr	r1, [sp, #16]
    6388:	e1a02008 	mov	r2, r8
    638c:	e1a03009 	mov	r3, r9
    6390:	e1a00007 	mov	r0, r7
    6394:	e58d1004 	str	r1, [sp, #4]
    6398:	ebfffa9f 	bl	4e1c <vs_read>
    639c:	e1a00007 	mov	r0, r7
    63a0:	ebffed51 	bl	18ec <isi_check_header>
    63a4:	e2504000 	subs	r4, r0, #0	; 0x0
    63a8:	1a00000d 	bne	63e4 <burn_image_loop+0x188>
    63ac:	e5973004 	ldr	r3, [r7, #4]
    63b0:	e1b03e23 	lsrs	r3, r3, #28
    63b4:	0a00000a 	beq	63e4 <burn_image_loop+0x188>
    63b8:	e59f014c 	ldr	r0, [pc, #332]	; 650c <burn_image_loop+0x2b0>
    63bc:	e59f3130 	ldr	r3, [pc, #304]	; 64f4 <burn_image_loop+0x298>
    63c0:	e1a0e00f 	mov	lr, pc
    63c4:	e593f000 	ldr	pc, [r3]
    63c8:	e59f2140 	ldr	r2, [pc, #320]	; 6510 <burn_image_loop+0x2b4>
    63cc:	e1a01004 	mov	r1, r4
    63d0:	e3a00001 	mov	r0, #1	; 0x1
    63d4:	ebfffb98 	bl	523c <ss_aes_init>
    63d8:	e3a02001 	mov	r2, #1	; 0x1
    63dc:	e58d201c 	str	r2, [sp, #28]
    63e0:	ea000001 	b	63ec <burn_image_loop+0x190>
    63e4:	e3a03000 	mov	r3, #0	; 0x0
    63e8:	e58d301c 	str	r3, [sp, #28]
    63ec:	e59f4100 	ldr	r4, [pc, #256]	; 64f4 <burn_image_loop+0x298>
    63f0:	e59d1048 	ldr	r1, [sp, #72]
    63f4:	e59f0118 	ldr	r0, [pc, #280]	; 6514 <burn_image_loop+0x2b8>
    63f8:	e3a02901 	mov	r2, #16384	; 0x4000
    63fc:	e1a0e00f 	mov	lr, pc
    6400:	e594f000 	ldr	pc, [r4]
    6404:	e3a06901 	mov	r6, #16384	; 0x4000
    6408:	e1a04005 	mov	r4, r5
    640c:	ea000032 	b	64dc <burn_image_loop+0x280>
    6410:	e1b03a23 	lsrs	r3, r3, #20
    6414:	0a000005 	beq	6430 <burn_image_loop+0x1d4>
    6418:	e1a01a25 	lsr	r1, r5, #20
    641c:	e59f00f4 	ldr	r0, [pc, #244]	; 6518 <burn_image_loop+0x2bc>
    6420:	e59f20cc 	ldr	r2, [pc, #204]	; 64f4 <burn_image_loop+0x298>
    6424:	e1a0e00f 	mov	lr, pc
    6428:	e592f000 	ldr	pc, [r2]
    642c:	e1a04005 	mov	r4, r5
    6430:	e3a01000 	mov	r1, #0	; 0x0
    6434:	e59d0014 	ldr	r0, [sp, #20]
    6438:	ebfffb1e 	bl	50b8 <vs_assign_by_id>
    643c:	e59d1010 	ldr	r1, [sp, #16]
    6440:	e1a02008 	mov	r2, r8
    6444:	e1a03009 	mov	r3, r9
    6448:	e1a00007 	mov	r0, r7
    644c:	e58d6000 	str	r6, [sp]
    6450:	e58d1004 	str	r1, [sp, #4]
    6454:	ebfffa70 	bl	4e1c <vs_read>
    6458:	e59d201c 	ldr	r2, [sp, #28]
    645c:	e3520000 	cmp	r2, #0	; 0x0
    6460:	0a000003 	beq	6474 <burn_image_loop+0x218>
    6464:	e1a01007 	mov	r1, r7
    6468:	e1a02006 	mov	r2, r6
    646c:	e1a00007 	mov	r0, r7
    6470:	ebfffb60 	bl	51f8 <ss_aes_data>
    6474:	e3a01000 	mov	r1, #0	; 0x0
    6478:	e59d000c 	ldr	r0, [sp, #12]
    647c:	ebfffb0d 	bl	50b8 <vs_assign_by_id>
    6480:	e2501000 	subs	r1, r0, #0	; 0x0
    6484:	1a000005 	bne	64a0 <burn_image_loop+0x244>
    6488:	e1a00007 	mov	r0, r7
    648c:	e1a0200a 	mov	r2, sl
    6490:	e1a0300b 	mov	r3, fp
    6494:	e58d1004 	str	r1, [sp, #4]
    6498:	e58d6000 	str	r6, [sp]
    649c:	ebfff95e 	bl	4a1c <vs_write>
    64a0:	e59d0018 	ldr	r0, [sp, #24]
    64a4:	e3a01000 	mov	r1, #0	; 0x0
    64a8:	e3500001 	cmp	r0, #1	; 0x1
    64ac:	e3a00901 	mov	r0, #16384	; 0x4000
    64b0:	93a03901 	movls	r3, #16384	; 0x4000
    64b4:	83a03000 	movhi	r3, #0	; 0x0
    64b8:	e3550901 	cmp	r5, #16384	; 0x4000
    64bc:	31a02005 	movcc	r2, r5
    64c0:	23a02901 	movcs	r2, #16384	; 0x4000
    64c4:	e09aa000 	adds	sl, sl, r0
    64c8:	e0abb001 	adc	fp, fp, r1
    64cc:	e0988000 	adds	r8, r8, r0
    64d0:	e0877003 	add	r7, r7, r3
    64d4:	e0a99001 	adc	r9, r9, r1
    64d8:	e0625005 	rsb	r5, r2, r5
    64dc:	e3550000 	cmp	r5, #0	; 0x0
    64e0:	e0653004 	rsb	r3, r5, r4
    64e4:	1affffc9 	bne	6410 <burn_image_loop+0x1b4>
    64e8:	e1a00005 	mov	r0, r5
    64ec:	e28dd024 	add	sp, sp, #36	; 0x24
    64f0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    64f4:	0800d76c 	.word	0x0800d76c
    64f8:	00012e87 	.word	0x00012e87
    64fc:	00011d14 	.word	0x00011d14
    6500:	00013002 	.word	0x00013002
    6504:	00013009 	.word	0x00013009
    6508:	0001301f 	.word	0x0001301f
    650c:	0001304c 	.word	0x0001304c
    6510:	0800d10c 	.word	0x0800d10c
    6514:	00013065 	.word	0x00013065
    6518:	00013074 	.word	0x00013074

0000651c <burn_images>:
    651c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6520:	e590901c 	ldr	r9, [r0, #28]
    6524:	e24dd014 	sub	sp, sp, #20	; 0x14
    6528:	e59f41ac 	ldr	r4, [pc, #428]	; 66dc <burn_images+0x1c0>
    652c:	e1a05000 	mov	r5, r0
    6530:	e58d100c 	str	r1, [sp, #12]
    6534:	e59f01a4 	ldr	r0, [pc, #420]	; 66e0 <burn_images+0x1c4>
    6538:	e1a01009 	mov	r1, r9
    653c:	e1a0b002 	mov	fp, r2
    6540:	e2857020 	add	r7, r5, #32	; 0x20
    6544:	e1a0e00f 	mov	lr, pc
    6548:	e594f000 	ldr	pc, [r4]
    654c:	e1a0a004 	mov	sl, r4
    6550:	e3a08000 	mov	r8, #0	; 0x0
    6554:	ea000055 	b	66b0 <burn_images+0x194>
    6558:	e59f0184 	ldr	r0, [pc, #388]	; 66e4 <burn_images+0x1c8>
    655c:	e1a01008 	mov	r1, r8
    6560:	e1a0e00f 	mov	lr, pc
    6564:	e59af000 	ldr	pc, [sl]
    6568:	e5d73003 	ldrb	r3, [r7, #3]
    656c:	e5976008 	ldr	r6, [r7, #8]
    6570:	e203503f 	and	r5, r3, #63	; 0x3f
    6574:	e3550002 	cmp	r5, #2	; 0x2
    6578:	0a00000a 	beq	65a8 <burn_images+0x8c>
    657c:	ca000004 	bgt	6594 <burn_images+0x78>
    6580:	e3550000 	cmp	r5, #0	; 0x0
    6584:	0a000051 	beq	66d0 <burn_images+0x1b4>
    6588:	e3550001 	cmp	r5, #1	; 0x1
    658c:	1a00001a 	bne	65fc <burn_images+0xe0>
    6590:	ea000008 	b	65b8 <burn_images+0x9c>
    6594:	e3550030 	cmp	r5, #48	; 0x30
    6598:	0a00000c 	beq	65d0 <burn_images+0xb4>
    659c:	e3550031 	cmp	r5, #49	; 0x31
    65a0:	1a000015 	bne	65fc <burn_images+0xe0>
    65a4:	ea00000e 	b	65e4 <burn_images+0xc8>
    65a8:	e1a04826 	lsr	r4, r6, #16
    65ac:	e354000f 	cmp	r4, #15	; 0xf
    65b0:	1a000015 	bne	660c <burn_images+0xf0>
    65b4:	ea00001f 	b	6638 <burn_images+0x11c>
    65b8:	e59f0128 	ldr	r0, [pc, #296]	; 66e8 <burn_images+0x1cc>
    65bc:	e1a01006 	mov	r1, r6
    65c0:	e1a0e00f 	mov	lr, pc
    65c4:	e59af000 	ldr	pc, [sl]
    65c8:	e1a0e006 	mov	lr, r6
    65cc:	ea000040 	b	66d4 <burn_images+0x1b8>
    65d0:	e59f0114 	ldr	r0, [pc, #276]	; 66ec <burn_images+0x1d0>
    65d4:	e1a0e00f 	mov	lr, pc
    65d8:	e59af000 	ldr	pc, [sl]
    65dc:	ebffe88f 	bl	820 <system_reset>
    65e0:	ea00003a 	b	66d0 <burn_images+0x1b4>
    65e4:	e59f0104 	ldr	r0, [pc, #260]	; 66f0 <burn_images+0x1d4>
    65e8:	e1a0e00f 	mov	lr, pc
    65ec:	e59af000 	ldr	pc, [sl]
    65f0:	e3a00001 	mov	r0, #1	; 0x1
    65f4:	ebffe77b 	bl	3e8 <try_boot>
    65f8:	ea000034 	b	66d0 <burn_images+0x1b4>
    65fc:	e59f00f0 	ldr	r0, [pc, #240]	; 66f4 <burn_images+0x1d8>
    6600:	e1a0e00f 	mov	lr, pc
    6604:	e59af000 	ldr	pc, [sl]
    6608:	ea000026 	b	66a8 <burn_images+0x18c>
    660c:	e1a00004 	mov	r0, r4
    6610:	e3a01001 	mov	r1, #1	; 0x1
    6614:	ebfffaa7 	bl	50b8 <vs_assign_by_id>
    6618:	e3500000 	cmp	r0, #0	; 0x0
    661c:	0a000005 	beq	6638 <burn_images+0x11c>
    6620:	e59f00d0 	ldr	r0, [pc, #208]	; 66f8 <burn_images+0x1dc>
    6624:	e59f30b0 	ldr	r3, [pc, #176]	; 66dc <burn_images+0x1c0>
    6628:	e1a0e00f 	mov	lr, pc
    662c:	e593f000 	ldr	pc, [r3]
    6630:	e3e04002 	mvn	r4, #2	; 0x2
    6634:	ea000020 	b	66bc <burn_images+0x1a0>
    6638:	e3a0e302 	mov	lr, #134217728	; 0x8000000
    663c:	e1a0c826 	lsr	ip, r6, #16
    6640:	e1a0c80c 	lsl	ip, ip, #16
    6644:	e1a03004 	mov	r3, r4
    6648:	e1a00007 	mov	r0, r7
    664c:	e59d100c 	ldr	r1, [sp, #12]
    6650:	e1a0200b 	mov	r2, fp
    6654:	e88d5000 	stm	sp, {ip, lr}
    6658:	ebfffeff 	bl	625c <burn_image_loop>
    665c:	e3500000 	cmp	r0, #0	; 0x0
    6660:	aa000006 	bge	6680 <burn_images+0x164>
    6664:	e1a01008 	mov	r1, r8
    6668:	e59f008c 	ldr	r0, [pc, #140]	; 66fc <burn_images+0x1e0>
    666c:	e59f3068 	ldr	r3, [pc, #104]	; 66dc <burn_images+0x1c0>
    6670:	e1a0e00f 	mov	lr, pc
    6674:	e593f000 	ldr	pc, [r3]
    6678:	e3e04000 	mvn	r4, #0	; 0x0
    667c:	ea00000e 	b	66bc <burn_images+0x1a0>
    6680:	e3550001 	cmp	r5, #1	; 0x1
    6684:	1a000003 	bne	6698 <burn_images+0x17c>
    6688:	e5970004 	ldr	r0, [r7, #4]
    668c:	e3a01000 	mov	r1, #0	; 0x0
    6690:	ebfffe67 	bl	6034 <cdbg_jump>
    6694:	ea000003 	b	66a8 <burn_images+0x18c>
    6698:	e3550000 	cmp	r5, #0	; 0x0
    669c:	1a000001 	bne	66a8 <burn_images+0x18c>
    66a0:	e3a00302 	mov	r0, #134217728	; 0x8000000
    66a4:	ebfffed7 	bl	6208 <cdbg_config_isi>
    66a8:	e2888001 	add	r8, r8, #1	; 0x1
    66ac:	e2877010 	add	r7, r7, #16	; 0x10
    66b0:	e1580009 	cmp	r8, r9
    66b4:	baffffa7 	blt	6558 <burn_images+0x3c>
    66b8:	e3a04000 	mov	r4, #0	; 0x0
    66bc:	e3a00302 	mov	r0, #134217728	; 0x8000000
    66c0:	ebffebee 	bl	1680 <free>
    66c4:	e1a00004 	mov	r0, r4
    66c8:	e28dd014 	add	sp, sp, #20	; 0x14
    66cc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    66d0:	e3a0e302 	mov	lr, #134217728	; 0x8000000
    66d4:	e3a0400f 	mov	r4, #15	; 0xf
    66d8:	eaffffd7 	b	663c <burn_images+0x120>
    66dc:	0800d76c 	.word	0x0800d76c
    66e0:	00013081 	.word	0x00013081
    66e4:	0001309e 	.word	0x0001309e
    66e8:	000130b4 	.word	0x000130b4
    66ec:	000130d1 	.word	0x000130d1
    66f0:	000130e6 	.word	0x000130e6
    66f4:	000130f9 	.word	0x000130f9
    66f8:	0001310d 	.word	0x0001310d
    66fc:	0001312a 	.word	0x0001312a

00006700 <burn>:
    6700:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6704:	e1a09000 	mov	r9, r0
    6708:	e24dd00c 	sub	sp, sp, #12	; 0xc
    670c:	e3a00c02 	mov	r0, #512	; 0x200
    6710:	ebffeb72 	bl	14e0 <malloc>
    6714:	e2507000 	subs	r7, r0, #0	; 0x0
    6718:	1a000004 	bne	6730 <burn+0x30>
    671c:	e59f019c 	ldr	r0, [pc, #412]	; 68c0 <burn+0x1c0>
    6720:	e59f319c 	ldr	r3, [pc, #412]	; 68c4 <burn+0x1c4>
    6724:	e1a0e00f 	mov	lr, pc
    6728:	e593f000 	ldr	pc, [r3]
    672c:	ea000060 	b	68b4 <burn+0x1b4>
    6730:	ebffe7ab 	bl	5e4 <boot_device>
    6734:	e3a05000 	mov	r5, #0	; 0x0
    6738:	e59f8184 	ldr	r8, [pc, #388]	; 68c4 <burn+0x1c4>
    673c:	e3a0bc02 	mov	fp, #512	; 0x200
    6740:	e1a0a000 	mov	sl, r0
    6744:	ea00004c 	b	687c <burn+0x17c>
    6748:	e1a00005 	mov	r0, r5
    674c:	ebfff92c 	bl	4c04 <vs_device_burnsrc>
    6750:	e3500000 	cmp	r0, #0	; 0x0
    6754:	0a000047 	beq	6878 <burn+0x178>
    6758:	e155000a 	cmp	r5, sl
    675c:	0a000045 	beq	6878 <burn+0x178>
    6760:	e3a03001 	mov	r3, #1	; 0x1
    6764:	e0193513 	ands	r3, r9, r3, lsl r5
    6768:	1a000042 	bne	6878 <burn+0x178>
    676c:	ebffe85a 	bl	8dc <gtc_time>
    6770:	e1a00005 	mov	r0, r5
    6774:	e59f114c 	ldr	r1, [pc, #332]	; 68c8 <burn+0x1c8>
    6778:	ebfff98e 	bl	4db8 <vs_is_device>
    677c:	e3500000 	cmp	r0, #0	; 0x0
    6780:	1a000004 	bne	6798 <burn+0x98>
    6784:	e1a00005 	mov	r0, r5
    6788:	e59f113c 	ldr	r1, [pc, #316]	; 68cc <burn+0x1cc>
    678c:	ebfff989 	bl	4db8 <vs_is_device>
    6790:	e3500000 	cmp	r0, #0	; 0x0
    6794:	0a00000c 	beq	67cc <burn+0xcc>
    6798:	ebffe8e1 	bl	b24 <assert_linkpc>
    679c:	e3500000 	cmp	r0, #0	; 0x0
    67a0:	0a000009 	beq	67cc <burn+0xcc>
    67a4:	eb0017d5 	bl	c700 <linkpc_loop>
    67a8:	e2506000 	subs	r6, r0, #0	; 0x0
    67ac:	0a000038 	beq	6894 <burn+0x194>
    67b0:	e1a00005 	mov	r0, r5
    67b4:	e5984000 	ldr	r4, [r8]
    67b8:	ebfff947 	bl	4cdc <vs_device_name>
    67bc:	e1a02006 	mov	r2, r6
    67c0:	e1a01000 	mov	r1, r0
    67c4:	e59f0104 	ldr	r0, [pc, #260]	; 68d0 <burn+0x1d0>
    67c8:	ea00000a 	b	67f8 <burn+0xf8>
    67cc:	e1a00005 	mov	r0, r5
    67d0:	e3a01001 	mov	r1, #1	; 0x1
    67d4:	ebfffa37 	bl	50b8 <vs_assign_by_id>
    67d8:	e2506000 	subs	r6, r0, #0	; 0x0
    67dc:	0a000007 	beq	6800 <burn+0x100>
    67e0:	e1a00005 	mov	r0, r5
    67e4:	e5984000 	ldr	r4, [r8]
    67e8:	ebfff93b 	bl	4cdc <vs_device_name>
    67ec:	e1a01000 	mov	r1, r0
    67f0:	e59f00dc 	ldr	r0, [pc, #220]	; 68d4 <burn+0x1d4>
    67f4:	e1a02006 	mov	r2, r6
    67f8:	e12fff34 	blx	r4
    67fc:	ea00001d 	b	6878 <burn+0x178>
    6800:	e1a01007 	mov	r1, r7
    6804:	e59f00cc 	ldr	r0, [pc, #204]	; 68d8 <burn+0x1d8>
    6808:	e3a02401 	mov	r2, #16777216	; 0x1000000
    680c:	e3a03c02 	mov	r3, #512	; 0x200
    6810:	e58d6000 	str	r6, [sp]
    6814:	e1a0e00f 	mov	lr, pc
    6818:	e598f000 	ldr	pc, [r8]
    681c:	e3a02401 	mov	r2, #16777216	; 0x1000000
    6820:	e3a03000 	mov	r3, #0	; 0x0
    6824:	e1a00007 	mov	r0, r7
    6828:	e58d6004 	str	r6, [sp, #4]
    682c:	e58db000 	str	fp, [sp]
    6830:	ebfff979 	bl	4e1c <vs_read>
    6834:	e1a00007 	mov	r0, r7
    6838:	eb001dc2 	bl	df48 <ius_check_header>
    683c:	e2504000 	subs	r4, r0, #0	; 0x0
    6840:	1a00000c 	bne	6878 <burn+0x178>
    6844:	e1a01005 	mov	r1, r5
    6848:	e59f3074 	ldr	r3, [pc, #116]	; 68c4 <burn+0x1c4>
    684c:	e59f0088 	ldr	r0, [pc, #136]	; 68dc <burn+0x1dc>
    6850:	e1a0e00f 	mov	lr, pc
    6854:	e593f000 	ldr	pc, [r3]
    6858:	e1a00004 	mov	r0, r4
    685c:	e1a01004 	mov	r1, r4
    6860:	ebfffde8 	bl	6008 <cdbg_verify_burn_enable>
    6864:	e1a00007 	mov	r0, r7
    6868:	e1a01005 	mov	r1, r5
    686c:	e1a02004 	mov	r2, r4
    6870:	ebffff29 	bl	651c <burn_images>
    6874:	ea000006 	b	6894 <burn+0x194>
    6878:	e2855001 	add	r5, r5, #1	; 0x1
    687c:	ebfff937 	bl	4d60 <vs_device_count>
    6880:	e1550000 	cmp	r5, r0
    6884:	baffffaf 	blt	6748 <burn+0x48>
    6888:	e1a00007 	mov	r0, r7
    688c:	ebffeb7b 	bl	1680 <free>
    6890:	ea000007 	b	68b4 <burn+0x1b4>
    6894:	e59f0044 	ldr	r0, [pc, #68]	; 68e0 <burn+0x1e0>
    6898:	e59f3024 	ldr	r3, [pc, #36]	; 68c4 <burn+0x1c4>
    689c:	e1a0e00f 	mov	lr, pc
    68a0:	e593f000 	ldr	pc, [r3]
    68a4:	e1a00007 	mov	r0, r7
    68a8:	ebffeb74 	bl	1680 <free>
    68ac:	ebfffe12 	bl	60fc <cdbg_shell>
    68b0:	eafffffe 	b	68b0 <burn+0x1b0>
    68b4:	e3e00000 	mvn	r0, #0	; 0x0
    68b8:	e28dd00c 	add	sp, sp, #12	; 0xc
    68bc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    68c0:	00013142 	.word	0x00013142
    68c4:	0800d76c 	.word	0x0800d76c
    68c8:	00012cec 	.word	0x00012cec
    68cc:	00012ce8 	.word	0x00012ce8
    68d0:	00013162 	.word	0x00013162
    68d4:	00013184 	.word	0x00013184
    68d8:	000131a3 	.word	0x000131a3
    68dc:	000131d3 	.word	0x000131d3
    68e0:	000131eb 	.word	0x000131eb

000068e4 <mmc_send_cmd>:
    68e4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    68e8:	e24dd004 	sub	sp, sp, #4	; 0x4
    68ec:	e1a0e00f 	mov	lr, pc
    68f0:	e590f104 	ldr	pc, [r0, #260]
    68f4:	e28dd004 	add	sp, sp, #4	; 0x4
    68f8:	e8bd8000 	pop	{pc}

000068fc <mmc_set_blocklen>:
    68fc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6900:	e3a03015 	mov	r3, #21	; 0x15
    6904:	e24dd024 	sub	sp, sp, #36	; 0x24
    6908:	e3a02000 	mov	r2, #0	; 0x0
    690c:	e58d3004 	str	r3, [sp, #4]
    6910:	e3a03010 	mov	r3, #16	; 0x10
    6914:	e58d1008 	str	r1, [sp, #8]
    6918:	e1cd30b0 	strh	r3, [sp]
    691c:	e58d201c 	str	r2, [sp, #28]
    6920:	e1a0100d 	mov	r1, sp
    6924:	e1a0e00f 	mov	lr, pc
    6928:	e590f104 	ldr	pc, [r0, #260]
    692c:	e28dd024 	add	sp, sp, #36	; 0x24
    6930:	e8bd8000 	pop	{pc}

00006934 <find_mmc_device>:
    6934:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6938:	e59f3048 	ldr	r3, [pc, #72]	; 6988 <find_mmc_device+0x54>
    693c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6940:	e1a01000 	mov	r1, r0
    6944:	e1a0c003 	mov	ip, r3
    6948:	e5932000 	ldr	r2, [r3]
    694c:	ea000004 	b	6964 <find_mmc_device+0x30>
    6950:	e592309c 	ldr	r3, [r2, #156]
    6954:	e1a00002 	mov	r0, r2
    6958:	e1530001 	cmp	r3, r1
    695c:	0a000007 	beq	6980 <find_mmc_device+0x4c>
    6960:	e5922000 	ldr	r2, [r2]
    6964:	e152000c 	cmp	r2, ip
    6968:	1afffff8 	bne	6950 <find_mmc_device+0x1c>
    696c:	e59f0018 	ldr	r0, [pc, #24]	; 698c <find_mmc_device+0x58>
    6970:	e59f3018 	ldr	r3, [pc, #24]	; 6990 <find_mmc_device+0x5c>
    6974:	e1a0e00f 	mov	lr, pc
    6978:	e593f000 	ldr	pc, [r3]
    697c:	e3a00000 	mov	r0, #0	; 0x0
    6980:	e28dd004 	add	sp, sp, #4	; 0x4
    6984:	e8bd8000 	pop	{pc}
    6988:	0800d248 	.word	0x0800d248
    698c:	0001320e 	.word	0x0001320e
    6990:	0800d76c 	.word	0x0800d76c

00006994 <mmc_send_ext_csd>:
    6994:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6998:	e3a03015 	mov	r3, #21	; 0x15
    699c:	e24dd034 	sub	sp, sp, #52	; 0x34
    69a0:	e58d3004 	str	r3, [sp, #4]
    69a4:	e3a03c02 	mov	r3, #512	; 0x200
    69a8:	e3a02000 	mov	r2, #0	; 0x0
    69ac:	e3a0c001 	mov	ip, #1	; 0x1
    69b0:	e58d302c 	str	r3, [sp, #44]
    69b4:	e3a03008 	mov	r3, #8	; 0x8
    69b8:	e58d1020 	str	r1, [sp, #32]
    69bc:	e58d201c 	str	r2, [sp, #28]
    69c0:	e58d2008 	str	r2, [sp, #8]
    69c4:	e58dc024 	str	ip, [sp, #36]
    69c8:	e1cd30b0 	strh	r3, [sp]
    69cc:	e58dc028 	str	ip, [sp, #40]
    69d0:	e1a0100d 	mov	r1, sp
    69d4:	e28d2020 	add	r2, sp, #32	; 0x20
    69d8:	e1a0e00f 	mov	lr, pc
    69dc:	e590f104 	ldr	pc, [r0, #260]
    69e0:	e28dd034 	add	sp, sp, #52	; 0x34
    69e4:	e8bd8000 	pop	{pc}

000069e8 <sd_switch>:
    69e8:	e3e0c4ff 	mvn	ip, #-16777216	; 0xff000000
    69ec:	e1a02102 	lsl	r2, r2, #2
    69f0:	e18ccf81 	orr	ip, ip, r1, lsl #31
    69f4:	e3a0100f 	mov	r1, #15	; 0xf
    69f8:	e1ccc211 	bic	ip, ip, r1, lsl r2
    69fc:	e18cc213 	orr	ip, ip, r3, lsl r2
    6a00:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6a04:	e3a03015 	mov	r3, #21	; 0x15
    6a08:	e24dd034 	sub	sp, sp, #52	; 0x34
    6a0c:	e58d3004 	str	r3, [sp, #4]
    6a10:	e3a03000 	mov	r3, #0	; 0x0
    6a14:	e58d301c 	str	r3, [sp, #28]
    6a18:	e59d3038 	ldr	r3, [sp, #56]
    6a1c:	e3a02001 	mov	r2, #1	; 0x1
    6a20:	e58d3020 	str	r3, [sp, #32]
    6a24:	e3a03040 	mov	r3, #64	; 0x40
    6a28:	e58d302c 	str	r3, [sp, #44]
    6a2c:	e3a03006 	mov	r3, #6	; 0x6
    6a30:	e58d2024 	str	r2, [sp, #36]
    6a34:	e58d2028 	str	r2, [sp, #40]
    6a38:	e58dc008 	str	ip, [sp, #8]
    6a3c:	e1cd30b0 	strh	r3, [sp]
    6a40:	e1a0100d 	mov	r1, sp
    6a44:	e28d2020 	add	r2, sp, #32	; 0x20
    6a48:	e1a0e00f 	mov	lr, pc
    6a4c:	e590f104 	ldr	pc, [r0, #260]
    6a50:	e28dd034 	add	sp, sp, #52	; 0x34
    6a54:	e8bd8000 	pop	{pc}

00006a58 <mmc_set_ios>:
    6a58:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6a5c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6a60:	e1a0e00f 	mov	lr, pc
    6a64:	e590f108 	ldr	pc, [r0, #264]
    6a68:	e28dd004 	add	sp, sp, #4	; 0x4
    6a6c:	e8bd8000 	pop	{pc}

00006a70 <mmc_set_clock>:
    6a70:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6a74:	e590203c 	ldr	r2, [r0, #60]
    6a78:	e590c038 	ldr	ip, [r0, #56]
    6a7c:	e1510002 	cmp	r1, r2
    6a80:	21a01002 	movcs	r1, r2
    6a84:	e24dd004 	sub	sp, sp, #4	; 0x4
    6a88:	e151000c 	cmp	r1, ip
    6a8c:	25801048 	strcs	r1, [r0, #72]
    6a90:	3580c048 	strcc	ip, [r0, #72]
    6a94:	e1a0e00f 	mov	lr, pc
    6a98:	e590f108 	ldr	pc, [r0, #264]
    6a9c:	e28dd004 	add	sp, sp, #4	; 0x4
    6aa0:	e8bd8000 	pop	{pc}

00006aa4 <mmc_set_bus_width>:
    6aa4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6aa8:	e5801044 	str	r1, [r0, #68]
    6aac:	e24dd004 	sub	sp, sp, #4	; 0x4
    6ab0:	e1a0e00f 	mov	lr, pc
    6ab4:	e590f108 	ldr	pc, [r0, #264]
    6ab8:	e28dd004 	add	sp, sp, #4	; 0x4
    6abc:	e8bd8000 	pop	{pc}

00006ac0 <mmc_send_if_cond>:
    6ac0:	e92d4010 	push	{r4, lr}
    6ac4:	e590202c 	ldr	r2, [r0, #44]
    6ac8:	e59f3060 	ldr	r3, [pc, #96]	; 6b30 <mmc_send_if_cond+0x70>
    6acc:	e24dd020 	sub	sp, sp, #32	; 0x20
    6ad0:	e0023003 	and	r3, r2, r3
    6ad4:	e3530000 	cmp	r3, #0	; 0x0
    6ad8:	e28d1020 	add	r1, sp, #32	; 0x20
    6adc:	e3a03008 	mov	r3, #8	; 0x8
    6ae0:	e300c1aa 	movw	ip, #426	; 0x1aa
    6ae4:	e16132b0 	strh	r3, [r1, #-32]!
    6ae8:	03a0c0aa 	moveq	ip, #170	; 0xaa
    6aec:	e3a02000 	mov	r2, #0	; 0x0
    6af0:	e3a03015 	mov	r3, #21	; 0x15
    6af4:	e98d1008 	stmib	sp, {r3, ip}
    6af8:	e58d201c 	str	r2, [sp, #28]
    6afc:	e1a0100d 	mov	r1, sp
    6b00:	e5903104 	ldr	r3, [r0, #260]
    6b04:	e1a04000 	mov	r4, r0
    6b08:	e12fff33 	blx	r3
    6b0c:	e3500000 	cmp	r0, #0	; 0x0
    6b10:	1a000004 	bne	6b28 <mmc_send_if_cond+0x68>
    6b14:	e5dd300c 	ldrb	r3, [sp, #12]
    6b18:	e35300aa 	cmp	r3, #170	; 0xaa
    6b1c:	059f3010 	ldreq	r3, [pc, #16]	; 6b34 <mmc_send_if_cond+0x74>
    6b20:	13e00010 	mvnne	r0, #16	; 0x10
    6b24:	05843030 	streq	r3, [r4, #48]
    6b28:	e28dd020 	add	sp, sp, #32	; 0x20
    6b2c:	e8bd8010 	pop	{r4, pc}
    6b30:	00ff8000 	.word	0x00ff8000
    6b34:	00020020 	.word	0x00020020

00006b38 <mmc_register>:
    6b38:	e5903110 	ldr	r3, [r0, #272]
    6b3c:	e59fc05c 	ldr	ip, [pc, #92]	; 6ba0 <mmc_register+0x68>
    6b40:	e3530000 	cmp	r3, #0	; 0x0
    6b44:	e3a03006 	mov	r3, #6	; 0x6
    6b48:	e59c2000 	ldr	r2, [ip]
    6b4c:	e5803098 	str	r3, [r0, #152]
    6b50:	e59f304c 	ldr	r3, [pc, #76]	; 6ba4 <mmc_register+0x6c>
    6b54:	e2821001 	add	r1, r2, #1	; 0x1
    6b58:	e58030f8 	str	r3, [r0, #248]
    6b5c:	e59f3044 	ldr	r3, [pc, #68]	; 6ba8 <mmc_register+0x70>
    6b60:	e580209c 	str	r2, [r0, #156]
    6b64:	e58030fc 	str	r3, [r0, #252]
    6b68:	e3a03001 	mov	r3, #1	; 0x1
    6b6c:	e58c1000 	str	r1, [ip]
    6b70:	e5c030a4 	strb	r3, [r0, #164]
    6b74:	028330ff 	addeq	r3, r3, #255	; 0xff
    6b78:	05803110 	streq	r3, [r0, #272]
    6b7c:	e59f3028 	ldr	r3, [pc, #40]	; 6bac <mmc_register+0x74>
    6b80:	e5800004 	str	r0, [r0, #4]
    6b84:	e5932004 	ldr	r2, [r3, #4]
    6b88:	e5803000 	str	r3, [r0]
    6b8c:	e5830004 	str	r0, [r3, #4]
    6b90:	e5820000 	str	r0, [r2]
    6b94:	e5802004 	str	r2, [r0, #4]
    6b98:	e3a00000 	mov	r0, #0	; 0x0
    6b9c:	e12fff1e 	bx	lr
    6ba0:	0800d12c 	.word	0x0800d12c
    6ba4:	00007d74 	.word	0x00007d74
    6ba8:	00007290 	.word	0x00007290
    6bac:	0800d248 	.word	0x0800d248

00006bb0 <mmc_get_dev>:
    6bb0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6bb4:	e24dd004 	sub	sp, sp, #4	; 0x4
    6bb8:	ebffff5d 	bl	6934 <find_mmc_device>
    6bbc:	e3500000 	cmp	r0, #0	; 0x0
    6bc0:	12800098 	addne	r0, r0, #152	; 0x98
    6bc4:	e28dd004 	add	sp, sp, #4	; 0x4
    6bc8:	e8bd8000 	pop	{pc}

00006bcc <print_mmc_devices>:
    6bcc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6bd0:	e59f3068 	ldr	r3, [pc, #104]	; 6c40 <print_mmc_devices+0x74>
    6bd4:	e24dd004 	sub	sp, sp, #4	; 0x4
    6bd8:	e1a05000 	mov	r5, r0
    6bdc:	e1a07003 	mov	r7, r3
    6be0:	e5934000 	ldr	r4, [r3]
    6be4:	e59f6058 	ldr	r6, [pc, #88]	; 6c44 <print_mmc_devices+0x78>
    6be8:	ea00000a 	b	6c18 <print_mmc_devices+0x4c>
    6bec:	e594209c 	ldr	r2, [r4, #156]
    6bf0:	e1a0e00f 	mov	lr, pc
    6bf4:	e596f000 	ldr	pc, [r6]
    6bf8:	e5943000 	ldr	r3, [r4]
    6bfc:	e59f0044 	ldr	r0, [pc, #68]	; 6c48 <print_mmc_devices+0x7c>
    6c00:	e1530007 	cmp	r3, r7
    6c04:	e1a01005 	mov	r1, r5
    6c08:	0a000001 	beq	6c14 <print_mmc_devices+0x48>
    6c0c:	e1a0e00f 	mov	lr, pc
    6c10:	e596f000 	ldr	pc, [r6]
    6c14:	e5944000 	ldr	r4, [r4]
    6c18:	e1540007 	cmp	r4, r7
    6c1c:	e2841008 	add	r1, r4, #8	; 0x8
    6c20:	e59f0024 	ldr	r0, [pc, #36]	; 6c4c <print_mmc_devices+0x80>
    6c24:	1afffff0 	bne	6bec <print_mmc_devices+0x20>
    6c28:	e59f0020 	ldr	r0, [pc, #32]	; 6c50 <print_mmc_devices+0x84>
    6c2c:	e59f3010 	ldr	r3, [pc, #16]	; 6c44 <print_mmc_devices+0x78>
    6c30:	e1a0e00f 	mov	lr, pc
    6c34:	e593f000 	ldr	pc, [r3]
    6c38:	e28dd004 	add	sp, sp, #4	; 0x4
    6c3c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6c40:	0800d248 	.word	0x0800d248
    6c44:	0800d76c 	.word	0x0800d76c
    6c48:	0001322e 	.word	0x0001322e
    6c4c:	00013227 	.word	0x00013227
    6c50:	0001247a 	.word	0x0001247a

00006c54 <get_mmc_num>:
    6c54:	e59f3004 	ldr	r3, [pc, #4]	; 6c60 <get_mmc_num+0xc>
    6c58:	e5930000 	ldr	r0, [r3]
    6c5c:	e12fff1e 	bx	lr
    6c60:	0800d12c 	.word	0x0800d12c

00006c64 <mmc_vs_align>:
    6c64:	e3a00c02 	mov	r0, #512	; 0x200
    6c68:	e12fff1e 	bx	lr

00006c6c <init_mmc>:
    6c6c:	e59f3060 	ldr	r3, [pc, #96]	; 6cd4 <init_mmc+0x68>
    6c70:	e59f2060 	ldr	r2, [pc, #96]	; 6cd8 <init_mmc+0x6c>
    6c74:	e92d4070 	push	{r4, r5, r6, lr}
    6c78:	e3a04000 	mov	r4, #0	; 0x0
    6c7c:	e3a00b02 	mov	r0, #2048	; 0x800
    6c80:	e5833004 	str	r3, [r3, #4]
    6c84:	e5824000 	str	r4, [r2]
    6c88:	e5833000 	str	r3, [r3]
    6c8c:	ebffea13 	bl	14e0 <malloc>
    6c90:	e1a05000 	mov	r5, r0
    6c94:	e2806b01 	add	r6, r0, #1024	; 0x400
    6c98:	e1a00005 	mov	r0, r5
    6c9c:	e3a01000 	mov	r1, #0	; 0x0
    6ca0:	e3a02f4e 	mov	r2, #312	; 0x138
    6ca4:	ebffe9a0 	bl	132c <memset>
    6ca8:	e5856130 	str	r6, [r5, #304]
    6cac:	e1a00004 	mov	r0, r4
    6cb0:	e1a01005 	mov	r1, r5
    6cb4:	eb000490 	bl	7efc <imap_sdhc_initialize>
    6cb8:	e3500000 	cmp	r0, #0	; 0x0
    6cbc:	e2844001 	add	r4, r4, #1	; 0x1
    6cc0:	e2855f4e 	add	r5, r5, #312	; 0x138
    6cc4:	18bd8070 	popne	{r4, r5, r6, pc}
    6cc8:	e3540003 	cmp	r4, #3	; 0x3
    6ccc:	1afffff1 	bne	6c98 <init_mmc+0x2c>
    6cd0:	e8bd8070 	pop	{r4, r5, r6, pc}
    6cd4:	0800d248 	.word	0x0800d248
    6cd8:	0800d12c 	.word	0x0800d12c

00006cdc <sd_change_freq>:
    6cdc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6ce0:	e5903050 	ldr	r3, [r0, #80]
    6ce4:	e3a01000 	mov	r1, #0	; 0x0
    6ce8:	e2132b01 	ands	r2, r3, #1024	; 0x400
    6cec:	e1a04000 	mov	r4, r0
    6cf0:	e580104c 	str	r1, [r0, #76]
    6cf4:	e24dd084 	sub	sp, sp, #132	; 0x84
    6cf8:	11a00001 	movne	r0, r1
    6cfc:	1a000070 	bne	6ec4 <sd_change_freq+0x1e8>
    6d00:	e1d438b0 	ldrh	r3, [r4, #128]
    6d04:	e3a05015 	mov	r5, #21	; 0x15
    6d08:	e1a03803 	lsl	r3, r3, #16
    6d0c:	e58d3050 	str	r3, [sp, #80]
    6d10:	e28d6048 	add	r6, sp, #72	; 0x48
    6d14:	e3a03037 	mov	r3, #55	; 0x37
    6d18:	e1cd34b8 	strh	r3, [sp, #72]
    6d1c:	e58d504c 	str	r5, [sp, #76]
    6d20:	e58d2064 	str	r2, [sp, #100]
    6d24:	e1a01006 	mov	r1, r6
    6d28:	e1a0e00f 	mov	lr, pc
    6d2c:	e594f104 	ldr	pc, [r4, #260]
    6d30:	e3500000 	cmp	r0, #0	; 0x0
    6d34:	1a000062 	bne	6ec4 <sd_change_freq+0x1e8>
    6d38:	e3a03033 	mov	r3, #51	; 0x33
    6d3c:	e58d504c 	str	r5, [sp, #76]
    6d40:	e1a09006 	mov	r9, r6
    6d44:	e58d0064 	str	r0, [sp, #100]
    6d48:	e1cd34b8 	strh	r3, [sp, #72]
    6d4c:	e58d0050 	str	r0, [sp, #80]
    6d50:	e3a05003 	mov	r5, #3	; 0x3
    6d54:	e28da078 	add	sl, sp, #120	; 0x78
    6d58:	e3a08008 	mov	r8, #8	; 0x8
    6d5c:	e3a06001 	mov	r6, #1	; 0x1
    6d60:	e28d7068 	add	r7, sp, #104	; 0x68
    6d64:	e58da068 	str	sl, [sp, #104]
    6d68:	e58d8074 	str	r8, [sp, #116]
    6d6c:	e58d6070 	str	r6, [sp, #112]
    6d70:	e58d606c 	str	r6, [sp, #108]
    6d74:	e1a00004 	mov	r0, r4
    6d78:	e1a01009 	mov	r1, r9
    6d7c:	e1a02007 	mov	r2, r7
    6d80:	e1a0e00f 	mov	lr, pc
    6d84:	e594f104 	ldr	pc, [r4, #260]
    6d88:	e250b000 	subs	fp, r0, #0	; 0x0
    6d8c:	0a000003 	beq	6da0 <sd_change_freq+0xc4>
    6d90:	e3550000 	cmp	r5, #0	; 0x0
    6d94:	e2455001 	sub	r5, r5, #1	; 0x1
    6d98:	1afffff1 	bne	6d64 <sd_change_freq+0x88>
    6d9c:	ea000047 	b	6ec0 <sd_change_freq+0x1e4>
    6da0:	e59d3078 	ldr	r3, [sp, #120]
    6da4:	e59d007c 	ldr	r0, [sp, #124]
    6da8:	e1a01c03 	lsl	r1, r3, #24
    6dac:	e2032cff 	and	r2, r3, #65280	; 0xff00
    6db0:	e1811c23 	orr	r1, r1, r3, lsr #24
    6db4:	e1811402 	orr	r1, r1, r2, lsl #8
    6db8:	e20338ff 	and	r3, r3, #16711680	; 0xff0000
    6dbc:	e1811423 	orr	r1, r1, r3, lsr #8
    6dc0:	e1a03c00 	lsl	r3, r0, #24
    6dc4:	e1833c20 	orr	r3, r3, r0, lsr #24
    6dc8:	e2002cff 	and	r2, r0, #65280	; 0xff00
    6dcc:	e1a0cc21 	lsr	ip, r1, #24
    6dd0:	e1833402 	orr	r3, r3, r2, lsl #8
    6dd4:	e20008ff 	and	r0, r0, #16711680	; 0xff0000
    6dd8:	e20cc00f 	and	ip, ip, #15	; 0xf
    6ddc:	e1833420 	orr	r3, r3, r0, lsr #8
    6de0:	e35c0001 	cmp	ip, #1	; 0x1
    6de4:	e584305c 	str	r3, [r4, #92]
    6de8:	e5841058 	str	r1, [r4, #88]
    6dec:	059f30d8 	ldreq	r3, [pc, #216]	; 6ecc <sd_change_freq+0x1f0>
    6df0:	0a000004 	beq	6e08 <sd_change_freq+0x12c>
    6df4:	3a000002 	bcc	6e04 <sd_change_freq+0x128>
    6df8:	e35c0002 	cmp	ip, #2	; 0x2
    6dfc:	059f30cc 	ldreq	r3, [pc, #204]	; 6ed0 <sd_change_freq+0x1f4>
    6e00:	0a000000 	beq	6e08 <sd_change_freq+0x12c>
    6e04:	e59f30c8 	ldr	r3, [pc, #200]	; 6ed4 <sd_change_freq+0x1f8>
    6e08:	e5843030 	str	r3, [r4, #48]
    6e0c:	e5943058 	ldr	r3, [r4, #88]
    6e10:	e5942030 	ldr	r2, [r4, #48]
    6e14:	e3130701 	tst	r3, #262144	; 0x40000
    6e18:	1594304c 	ldrne	r3, [r4, #76]
    6e1c:	13833c01 	orrne	r3, r3, #256	; 0x100
    6e20:	1584304c 	strne	r3, [r4, #76]
    6e24:	e59f30a8 	ldr	r3, [pc, #168]	; 6ed4 <sd_change_freq+0x1f8>
    6e28:	e1520003 	cmp	r2, r3
    6e2c:	13a05004 	movne	r5, #4	; 0x4
    6e30:	128d6008 	addne	r6, sp, #8	; 0x8
    6e34:	1a000007 	bne	6e58 <sd_change_freq+0x17c>
    6e38:	ea000020 	b	6ec0 <sd_change_freq+0x1e4>
    6e3c:	e58d6000 	str	r6, [sp]
    6e40:	ebfffee8 	bl	69e8 <sd_switch>
    6e44:	e3500000 	cmp	r0, #0	; 0x0
    6e48:	1a00001d 	bne	6ec4 <sd_change_freq+0x1e8>
    6e4c:	e59d3024 	ldr	r3, [sp, #36]
    6e50:	e3130c02 	tst	r3, #512	; 0x200
    6e54:	0a000005 	beq	6e70 <sd_change_freq+0x194>
    6e58:	e3a01000 	mov	r1, #0	; 0x0
    6e5c:	e2555001 	subs	r5, r5, #1	; 0x1
    6e60:	e1a00004 	mov	r0, r4
    6e64:	e1a02001 	mov	r2, r1
    6e68:	e3a03001 	mov	r3, #1	; 0x1
    6e6c:	2afffff2 	bcs	6e3c <sd_change_freq+0x160>
    6e70:	e59d3014 	ldr	r3, [sp, #20]
    6e74:	e3130c02 	tst	r3, #512	; 0x200
    6e78:	0a000010 	beq	6ec0 <sd_change_freq+0x1e4>
    6e7c:	e3a01001 	mov	r1, #1	; 0x1
    6e80:	e28dc008 	add	ip, sp, #8	; 0x8
    6e84:	e1a00004 	mov	r0, r4
    6e88:	e3a02000 	mov	r2, #0	; 0x0
    6e8c:	e1a03001 	mov	r3, r1
    6e90:	e58dc000 	str	ip, [sp]
    6e94:	ebfffed3 	bl	69e8 <sd_switch>
    6e98:	e3500000 	cmp	r0, #0	; 0x0
    6e9c:	1a000008 	bne	6ec4 <sd_change_freq+0x1e8>
    6ea0:	e59d3018 	ldr	r3, [sp, #24]
    6ea4:	e1a03c03 	lsl	r3, r3, #24
    6ea8:	e203340f 	and	r3, r3, #251658240	; 0xf000000
    6eac:	e3530401 	cmp	r3, #16777216	; 0x1000000
    6eb0:	0594304c 	ldreq	r3, [r4, #76]
    6eb4:	03833001 	orreq	r3, r3, #1	; 0x1
    6eb8:	0584304c 	streq	r3, [r4, #76]
    6ebc:	ea000000 	b	6ec4 <sd_change_freq+0x1e8>
    6ec0:	e1a0000b 	mov	r0, fp
    6ec4:	e28dd084 	add	sp, sp, #132	; 0x84
    6ec8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6ecc:	0002001a 	.word	0x0002001a
    6ed0:	00020020 	.word	0x00020020
    6ed4:	00020010 	.word	0x00020010

00006ed8 <sd_send_op_cond>:
    6ed8:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    6edc:	e59f9140 	ldr	r9, [pc, #320]	; 7024 <sd_send_op_cond+0x14c>
    6ee0:	e24dd020 	sub	sp, sp, #32	; 0x20
    6ee4:	e1a04000 	mov	r4, r0
    6ee8:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    6eec:	e3a0a015 	mov	sl, #21	; 0x15
    6ef0:	e3a06000 	mov	r6, #0	; 0x0
    6ef4:	e1a0700d 	mov	r7, sp
    6ef8:	e3a08001 	mov	r8, #1	; 0x1
    6efc:	e3a03037 	mov	r3, #55	; 0x37
    6f00:	e1cd30b0 	strh	r3, [sp]
    6f04:	e1a0100d 	mov	r1, sp
    6f08:	e1a02006 	mov	r2, r6
    6f0c:	e58da004 	str	sl, [sp, #4]
    6f10:	e58d6008 	str	r6, [sp, #8]
    6f14:	e58d601c 	str	r6, [sp, #28]
    6f18:	e1a00004 	mov	r0, r4
    6f1c:	e1a0e00f 	mov	lr, pc
    6f20:	e594f104 	ldr	pc, [r4, #260]
    6f24:	e2503000 	subs	r3, r0, #0	; 0x0
    6f28:	e1a0100d 	mov	r1, sp
    6f2c:	e3a02000 	mov	r2, #0	; 0x0
    6f30:	e59fe0f0 	ldr	lr, [pc, #240]	; 7028 <sd_send_op_cond+0x150>
    6f34:	e1a00004 	mov	r0, r4
    6f38:	1a000036 	bne	7018 <sd_send_op_cond+0x140>
    6f3c:	e594302c 	ldr	r3, [r4, #44]
    6f40:	e594c030 	ldr	ip, [r4, #48]
    6f44:	e003e00e 	and	lr, r3, lr
    6f48:	e15c0009 	cmp	ip, r9
    6f4c:	038e3101 	orreq	r3, lr, #1073741824	; 0x40000000
    6f50:	e58de008 	str	lr, [sp, #8]
    6f54:	058d3008 	streq	r3, [sp, #8]
    6f58:	e3a03029 	mov	r3, #41	; 0x29
    6f5c:	e1cd30b0 	strh	r3, [sp]
    6f60:	e58d8004 	str	r8, [sp, #4]
    6f64:	e5943104 	ldr	r3, [r4, #260]
    6f68:	e12fff33 	blx	r3
    6f6c:	e2503000 	subs	r3, r0, #0	; 0x0
    6f70:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    6f74:	1a000027 	bne	7018 <sd_send_op_cond+0x140>
    6f78:	eb000636 	bl	8858 <udelay>
    6f7c:	e59d300c 	ldr	r3, [sp, #12]
    6f80:	e3530000 	cmp	r3, #0	; 0x0
    6f84:	ba000002 	blt	6f94 <sd_send_op_cond+0xbc>
    6f88:	e2555001 	subs	r5, r5, #1	; 0x1
    6f8c:	2affffda 	bcs	6efc <sd_send_op_cond+0x24>
    6f90:	ea00001f 	b	7014 <sd_send_op_cond+0x13c>
    6f94:	e3550000 	cmp	r5, #0	; 0x0
    6f98:	da00001d 	ble	7014 <sd_send_op_cond+0x13c>
    6f9c:	e59f3080 	ldr	r3, [pc, #128]	; 7024 <sd_send_op_cond+0x14c>
    6fa0:	e5942030 	ldr	r2, [r4, #48]
    6fa4:	e1520003 	cmp	r2, r3
    6fa8:	12433010 	subne	r3, r3, #16	; 0x10
    6fac:	15843030 	strne	r3, [r4, #48]
    6fb0:	e5943050 	ldr	r3, [r4, #80]
    6fb4:	e3130b01 	tst	r3, #1024	; 0x400
    6fb8:	0a00000c 	beq	6ff0 <sd_send_op_cond+0x118>
    6fbc:	e3a03001 	mov	r3, #1	; 0x1
    6fc0:	e3a02000 	mov	r2, #0	; 0x0
    6fc4:	e58d3004 	str	r3, [sp, #4]
    6fc8:	e3a0303a 	mov	r3, #58	; 0x3a
    6fcc:	e1cd30b0 	strh	r3, [sp]
    6fd0:	e58d2008 	str	r2, [sp, #8]
    6fd4:	e58d201c 	str	r2, [sp, #28]
    6fd8:	e1a00004 	mov	r0, r4
    6fdc:	e1a0100d 	mov	r1, sp
    6fe0:	e1a0e00f 	mov	lr, pc
    6fe4:	e594f104 	ldr	pc, [r4, #260]
    6fe8:	e2503000 	subs	r3, r0, #0	; 0x0
    6fec:	1a000009 	bne	7018 <sd_send_op_cond+0x140>
    6ff0:	e59d200c 	ldr	r2, [sp, #12]
    6ff4:	e3a03000 	mov	r3, #0	; 0x0
    6ff8:	e1c438b0 	strh	r3, [r4, #128]
    6ffc:	e1a03f22 	lsr	r3, r2, #30
    7000:	e2033001 	and	r3, r3, #1	; 0x1
    7004:	e5843040 	str	r3, [r4, #64]
    7008:	e5842054 	str	r2, [r4, #84]
    700c:	e3a03000 	mov	r3, #0	; 0x0
    7010:	ea000000 	b	7018 <sd_send_op_cond+0x140>
    7014:	e3e03010 	mvn	r3, #16	; 0x10
    7018:	e1a00003 	mov	r0, r3
    701c:	e28dd020 	add	sp, sp, #32	; 0x20
    7020:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    7024:	00020020 	.word	0x00020020
    7028:	00ff8000 	.word	0x00ff8000

0000702c <mmc_go_idle>:
    702c:	e92d4010 	push	{r4, lr}
    7030:	e1a04000 	mov	r4, r0
    7034:	e24dd020 	sub	sp, sp, #32	; 0x20
    7038:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    703c:	eb000605 	bl	8858 <udelay>
    7040:	e3a02000 	mov	r2, #0	; 0x0
    7044:	e1a00004 	mov	r0, r4
    7048:	e1cd20b0 	strh	r2, [sp]
    704c:	e58d2008 	str	r2, [sp, #8]
    7050:	e58d2004 	str	r2, [sp, #4]
    7054:	e58d201c 	str	r2, [sp, #28]
    7058:	e1a0100d 	mov	r1, sp
    705c:	e1a0e00f 	mov	lr, pc
    7060:	e594f104 	ldr	pc, [r4, #260]
    7064:	e2504000 	subs	r4, r0, #0	; 0x0
    7068:	1a000001 	bne	7074 <mmc_go_idle+0x48>
    706c:	e2800e7d 	add	r0, r0, #2000	; 0x7d0
    7070:	eb0005f8 	bl	8858 <udelay>
    7074:	e1a00004 	mov	r0, r4
    7078:	e28dd020 	add	sp, sp, #32	; 0x20
    707c:	e8bd8010 	pop	{r4, pc}

00007080 <mmc_send_op_cond>:
    7080:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7084:	e24dd020 	sub	sp, sp, #32	; 0x20
    7088:	e1a05000 	mov	r5, r0
    708c:	e3a06001 	mov	r6, #1	; 0x1
    7090:	ebffffe5 	bl	702c <mmc_go_idle>
    7094:	e3a02000 	mov	r2, #0	; 0x0
    7098:	e1cd60b0 	strh	r6, [sp]
    709c:	e58d6004 	str	r6, [sp, #4]
    70a0:	e58d2008 	str	r2, [sp, #8]
    70a4:	e58d201c 	str	r2, [sp, #28]
    70a8:	e1a00005 	mov	r0, r5
    70ac:	e1a0100d 	mov	r1, sp
    70b0:	e1a0e00f 	mov	lr, pc
    70b4:	e595f104 	ldr	pc, [r5, #260]
    70b8:	e2504000 	subs	r4, r0, #0	; 0x0
    70bc:	e1a0700d 	mov	r7, sp
    70c0:	1a000039 	bne	71ac <mmc_send_op_cond+0x12c>
    70c4:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    70c8:	eb0005e2 	bl	8858 <udelay>
    70cc:	e1a08006 	mov	r8, r6
    70d0:	e1a0700d 	mov	r7, sp
    70d4:	e1a06004 	mov	r6, r4
    70d8:	e3024710 	movw	r4, #10000	; 0x2710
    70dc:	e595302c 	ldr	r3, [r5, #44]
    70e0:	e3a02001 	mov	r2, #1	; 0x1
    70e4:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    70e8:	e1cd20b0 	strh	r2, [sp]
    70ec:	e58d3008 	str	r3, [sp, #8]
    70f0:	e1a02006 	mov	r2, r6
    70f4:	e58d8004 	str	r8, [sp, #4]
    70f8:	e58d601c 	str	r6, [sp, #28]
    70fc:	e1a0100d 	mov	r1, sp
    7100:	e1a00005 	mov	r0, r5
    7104:	e1a0e00f 	mov	lr, pc
    7108:	e595f104 	ldr	pc, [r5, #260]
    710c:	e2502000 	subs	r2, r0, #0	; 0x0
    7110:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    7114:	11a04002 	movne	r4, r2
    7118:	1a000023 	bne	71ac <mmc_send_op_cond+0x12c>
    711c:	eb0005cd 	bl	8858 <udelay>
    7120:	e59d300c 	ldr	r3, [sp, #12]
    7124:	e3530000 	cmp	r3, #0	; 0x0
    7128:	ba000002 	blt	7138 <mmc_send_op_cond+0xb8>
    712c:	e2544001 	subs	r4, r4, #1	; 0x1
    7130:	2affffe9 	bcs	70dc <mmc_send_op_cond+0x5c>
    7134:	ea00001b 	b	71a8 <mmc_send_op_cond+0x128>
    7138:	e3540000 	cmp	r4, #0	; 0x0
    713c:	da000019 	ble	71a8 <mmc_send_op_cond+0x128>
    7140:	e5953050 	ldr	r3, [r5, #80]
    7144:	e3130b01 	tst	r3, #1024	; 0x400
    7148:	0a00000b 	beq	717c <mmc_send_op_cond+0xfc>
    714c:	e3a0303a 	mov	r3, #58	; 0x3a
    7150:	e58d8004 	str	r8, [sp, #4]
    7154:	e1cd30b0 	strh	r3, [sp]
    7158:	e58d6008 	str	r6, [sp, #8]
    715c:	e58d601c 	str	r6, [sp, #28]
    7160:	e1a0100d 	mov	r1, sp
    7164:	e1a02006 	mov	r2, r6
    7168:	e1a00005 	mov	r0, r5
    716c:	e1a0e00f 	mov	lr, pc
    7170:	e595f104 	ldr	pc, [r5, #260]
    7174:	e2504000 	subs	r4, r0, #0	; 0x0
    7178:	1a00000b 	bne	71ac <mmc_send_op_cond+0x12c>
    717c:	e59d300c 	ldr	r3, [sp, #12]
    7180:	e3a01801 	mov	r1, #65536	; 0x10000
    7184:	e1a02f23 	lsr	r2, r3, #30
    7188:	e2022001 	and	r2, r2, #1	; 0x1
    718c:	e5852040 	str	r2, [r5, #64]
    7190:	e3a02000 	mov	r2, #0	; 0x0
    7194:	e5851030 	str	r1, [r5, #48]
    7198:	e1c528b0 	strh	r2, [r5, #128]
    719c:	e5853054 	str	r3, [r5, #84]
    71a0:	e3a04000 	mov	r4, #0	; 0x0
    71a4:	ea000000 	b	71ac <mmc_send_op_cond+0x12c>
    71a8:	e3e04010 	mvn	r4, #16	; 0x10
    71ac:	e1a00004 	mov	r0, r4
    71b0:	e28dd020 	add	sp, sp, #32	; 0x20
    71b4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

000071b8 <mmc_send_status>:
    71b8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    71bc:	e1d038b0 	ldrh	r3, [r0, #128]
    71c0:	e24dd024 	sub	sp, sp, #36	; 0x24
    71c4:	e1a03803 	lsl	r3, r3, #16
    71c8:	e58d3008 	str	r3, [sp, #8]
    71cc:	e3a03000 	mov	r3, #0	; 0x0
    71d0:	e3a02015 	mov	r2, #21	; 0x15
    71d4:	e58d301c 	str	r3, [sp, #28]
    71d8:	e3a0300d 	mov	r3, #13	; 0xd
    71dc:	e1a06000 	mov	r6, r0
    71e0:	e58d2004 	str	r2, [sp, #4]
    71e4:	e1a04001 	mov	r4, r1
    71e8:	e1cd30b0 	strh	r3, [sp]
    71ec:	e1a0700d 	mov	r7, sp
    71f0:	e1a0100d 	mov	r1, sp
    71f4:	e3a02000 	mov	r2, #0	; 0x0
    71f8:	e1a00006 	mov	r0, r6
    71fc:	e1a0e00f 	mov	lr, pc
    7200:	e596f104 	ldr	pc, [r6, #260]
    7204:	e2505000 	subs	r5, r0, #0	; 0x0
    7208:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    720c:	1a000018 	bne	7274 <mmc_send_status+0xbc>
    7210:	e59d300c 	ldr	r3, [sp, #12]
    7214:	e3130c01 	tst	r3, #256	; 0x100
    7218:	1a00000e 	bne	7258 <mmc_send_status+0xa0>
    721c:	eb00058d 	bl	8858 <udelay>
    7220:	e59d100c 	ldr	r1, [sp, #12]
    7224:	e59f3054 	ldr	r3, [pc, #84]	; 7280 <mmc_send_status+0xc8>
    7228:	e2444001 	sub	r4, r4, #1	; 0x1
    722c:	e0013003 	and	r3, r1, r3
    7230:	e3530000 	cmp	r3, #0	; 0x0
    7234:	0a000005 	beq	7250 <mmc_send_status+0x98>
    7238:	e59f0044 	ldr	r0, [pc, #68]	; 7284 <mmc_send_status+0xcc>
    723c:	e59f3044 	ldr	r3, [pc, #68]	; 7288 <mmc_send_status+0xd0>
    7240:	e1a0e00f 	mov	lr, pc
    7244:	e593f000 	ldr	pc, [r3]
    7248:	e3e05011 	mvn	r5, #17	; 0x11
    724c:	ea000008 	b	7274 <mmc_send_status+0xbc>
    7250:	e3740001 	cmn	r4, #1	; 0x1
    7254:	1affffe5 	bne	71f0 <mmc_send_status+0x38>
    7258:	e3540000 	cmp	r4, #0	; 0x0
    725c:	1a000004 	bne	7274 <mmc_send_status+0xbc>
    7260:	e59f0024 	ldr	r0, [pc, #36]	; 728c <mmc_send_status+0xd4>
    7264:	e59f301c 	ldr	r3, [pc, #28]	; 7288 <mmc_send_status+0xd0>
    7268:	e1a0e00f 	mov	lr, pc
    726c:	e593f000 	ldr	pc, [r3]
    7270:	e3e05012 	mvn	r5, #18	; 0x12
    7274:	e1a00005 	mov	r0, r5
    7278:	e28dd024 	add	sp, sp, #36	; 0x24
    727c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    7280:	fdf94080 	.word	0xfdf94080
    7284:	00013232 	.word	0x00013232
    7288:	0800d76c 	.word	0x0800d76c
    728c:	00013248 	.word	0x00013248

00007290 <mmc_bwrite>:
    7290:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7294:	e24dd034 	sub	sp, sp, #52	; 0x34
    7298:	e1a06001 	mov	r6, r1
    729c:	e1a0b002 	mov	fp, r2
    72a0:	e1a0a003 	mov	sl, r3
    72a4:	ebfffda2 	bl	6934 <find_mmc_device>
    72a8:	e2504000 	subs	r4, r0, #0	; 0x0
    72ac:	0a000060 	beq	7434 <mmc_bwrite+0x1a4>
    72b0:	e594108c 	ldr	r1, [r4, #140]
    72b4:	ebfffd90 	bl	68fc <mmc_set_blocklen>
    72b8:	e3500000 	cmp	r0, #0	; 0x0
    72bc:	01a09000 	moveq	r9, r0
    72c0:	01a0700b 	moveq	r7, fp
    72c4:	0a000005 	beq	72e0 <mmc_bwrite+0x50>
    72c8:	e594108c 	ldr	r1, [r4, #140]
    72cc:	e59f016c 	ldr	r0, [pc, #364]	; 7440 <mmc_bwrite+0x1b0>
    72d0:	e59f316c 	ldr	r3, [pc, #364]	; 7444 <mmc_bwrite+0x1b4>
    72d4:	e1a0e00f 	mov	lr, pc
    72d8:	e593f000 	ldr	pc, [r3]
    72dc:	ea000054 	b	7434 <mmc_bwrite+0x1a4>
    72e0:	e5943110 	ldr	r3, [r4, #272]
    72e4:	e59420a8 	ldr	r2, [r4, #168]
    72e8:	e1570003 	cmp	r7, r3
    72ec:	31a05007 	movcc	r5, r7
    72f0:	21a05003 	movcs	r5, r3
    72f4:	e0858006 	add	r8, r5, r6
    72f8:	e1580002 	cmp	r8, r2
    72fc:	9a000005 	bls	7318 <mmc_bwrite+0x88>
    7300:	e59f0140 	ldr	r0, [pc, #320]	; 7448 <mmc_bwrite+0x1b8>
    7304:	e1a01008 	mov	r1, r8
    7308:	e59f3134 	ldr	r3, [pc, #308]	; 7444 <mmc_bwrite+0x1b4>
    730c:	e1a0e00f 	mov	lr, pc
    7310:	e593f000 	ldr	pc, [r3]
    7314:	ea00003d 	b	7410 <mmc_bwrite+0x180>
    7318:	e3550001 	cmp	r5, #1	; 0x1
    731c:	93a03018 	movls	r3, #24	; 0x18
    7320:	91cd30b0 	strhls	r3, [sp]
    7324:	e5943040 	ldr	r3, [r4, #64]
    7328:	83a02019 	movhi	r2, #25	; 0x19
    732c:	81cd20b0 	strhhi	r2, [sp]
    7330:	e3530000 	cmp	r3, #0	; 0x0
    7334:	0594308c 	ldreq	r3, [r4, #140]
    7338:	e3a02015 	mov	r2, #21	; 0x15
    733c:	00030693 	muleq	r3, r3, r6
    7340:	158d6008 	strne	r6, [sp, #8]
    7344:	058d3008 	streq	r3, [sp, #8]
    7348:	e58d2004 	str	r2, [sp, #4]
    734c:	e58d901c 	str	r9, [sp, #28]
    7350:	e58da020 	str	sl, [sp, #32]
    7354:	e58d5028 	str	r5, [sp, #40]
    7358:	e594308c 	ldr	r3, [r4, #140]
    735c:	e3a02002 	mov	r2, #2	; 0x2
    7360:	e58d2024 	str	r2, [sp, #36]
    7364:	e58d302c 	str	r3, [sp, #44]
    7368:	e1a00004 	mov	r0, r4
    736c:	e1a0100d 	mov	r1, sp
    7370:	e28d2020 	add	r2, sp, #32	; 0x20
    7374:	e1a0e00f 	mov	lr, pc
    7378:	e594f104 	ldr	pc, [r4, #260]
    737c:	e3500000 	cmp	r0, #0	; 0x0
    7380:	0a000004 	beq	7398 <mmc_bwrite+0x108>
    7384:	e59f00c0 	ldr	r0, [pc, #192]	; 744c <mmc_bwrite+0x1bc>
    7388:	e59f30b4 	ldr	r3, [pc, #180]	; 7444 <mmc_bwrite+0x1b4>
    738c:	e1a0e00f 	mov	lr, pc
    7390:	e593f000 	ldr	pc, [r3]
    7394:	ea00001d 	b	7410 <mmc_bwrite+0x180>
    7398:	e5943050 	ldr	r3, [r4, #80]
    739c:	e1a03523 	lsr	r3, r3, #10
    73a0:	e2233001 	eor	r3, r3, #1	; 0x1
    73a4:	e3550001 	cmp	r5, #1	; 0x1
    73a8:	93a03000 	movls	r3, #0	; 0x0
    73ac:	82033001 	andhi	r3, r3, #1	; 0x1
    73b0:	e3530000 	cmp	r3, #0	; 0x0
    73b4:	0a000011 	beq	7400 <mmc_bwrite+0x170>
    73b8:	e3a0200c 	mov	r2, #12	; 0xc
    73bc:	e3a0301d 	mov	r3, #29	; 0x1d
    73c0:	e1cd20b0 	strh	r2, [sp]
    73c4:	e98d0208 	stmib	sp, {r3, r9}
    73c8:	e58d901c 	str	r9, [sp, #28]
    73cc:	e1a00004 	mov	r0, r4
    73d0:	e1a0100d 	mov	r1, sp
    73d4:	e1a02009 	mov	r2, r9
    73d8:	e1a0e00f 	mov	lr, pc
    73dc:	e594f104 	ldr	pc, [r4, #260]
    73e0:	e3500000 	cmp	r0, #0	; 0x0
    73e4:	0a000005 	beq	7400 <mmc_bwrite+0x170>
    73e8:	e59f0060 	ldr	r0, [pc, #96]	; 7450 <mmc_bwrite+0x1c0>
    73ec:	e59f3050 	ldr	r3, [pc, #80]	; 7444 <mmc_bwrite+0x1b4>
    73f0:	e1a0e00f 	mov	lr, pc
    73f4:	e593f000 	ldr	pc, [r3]
    73f8:	e1a00009 	mov	r0, r9
    73fc:	ea00000d 	b	7438 <mmc_bwrite+0x1a8>
    7400:	e1a00004 	mov	r0, r4
    7404:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    7408:	ebffff6a 	bl	71b8 <mmc_send_status>
    740c:	ea000001 	b	7418 <mmc_bwrite+0x188>
    7410:	e3550000 	cmp	r5, #0	; 0x0
    7414:	1a000006 	bne	7434 <mmc_bwrite+0x1a4>
    7418:	e0577005 	subs	r7, r7, r5
    741c:	e594008c 	ldr	r0, [r4, #140]
    7420:	102aa590 	mlane	sl, r0, r5, sl
    7424:	11a06008 	movne	r6, r8
    7428:	1affffac 	bne	72e0 <mmc_bwrite+0x50>
    742c:	e000009b 	mul	r0, fp, r0
    7430:	ea000000 	b	7438 <mmc_bwrite+0x1a8>
    7434:	e3a00000 	mov	r0, #0	; 0x0
    7438:	e28dd034 	add	sp, sp, #52	; 0x34
    743c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7440:	00013264 	.word	0x00013264
    7444:	0800d76c 	.word	0x0800d76c
    7448:	0001326e 	.word	0x0001326e
    744c:	0001329a 	.word	0x0001329a
    7450:	000132ac 	.word	0x000132ac

00007454 <mmc2_vs_write>:
    7454:	e1a024a2 	lsr	r2, r2, #9
    7458:	e1821b83 	orr	r1, r2, r3, lsl #23
    745c:	e59d2000 	ldr	r2, [sp]
    7460:	e1a03000 	mov	r3, r0
    7464:	e1a024c2 	asr	r2, r2, #9
    7468:	e3a00002 	mov	r0, #2	; 0x2
    746c:	eaffff87 	b	7290 <mmc_bwrite>

00007470 <mmc1_vs_write>:
    7470:	e1a024a2 	lsr	r2, r2, #9
    7474:	e1821b83 	orr	r1, r2, r3, lsl #23
    7478:	e59d2000 	ldr	r2, [sp]
    747c:	e1a03000 	mov	r3, r0
    7480:	e1a024c2 	asr	r2, r2, #9
    7484:	e3a00001 	mov	r0, #1	; 0x1
    7488:	eaffff80 	b	7290 <mmc_bwrite>

0000748c <mmc0_vs_write>:
    748c:	e1a024a2 	lsr	r2, r2, #9
    7490:	e1821b83 	orr	r1, r2, r3, lsl #23
    7494:	e59d2000 	ldr	r2, [sp]
    7498:	e1a03000 	mov	r3, r0
    749c:	e1a024c2 	asr	r2, r2, #9
    74a0:	e3a00000 	mov	r0, #0	; 0x0
    74a4:	eaffff79 	b	7290 <mmc_bwrite>

000074a8 <mmc_switch>:
    74a8:	e92d4030 	push	{r4, r5, lr}
    74ac:	e1a02802 	lsl	r2, r2, #16
    74b0:	e3822403 	orr	r2, r2, #50331648	; 0x3000000
    74b4:	e24dd024 	sub	sp, sp, #36	; 0x24
    74b8:	e1822403 	orr	r2, r2, r3, lsl #8
    74bc:	e3a0301d 	mov	r3, #29	; 0x1d
    74c0:	e3a01000 	mov	r1, #0	; 0x0
    74c4:	e58d3004 	str	r3, [sp, #4]
    74c8:	e3a03006 	mov	r3, #6	; 0x6
    74cc:	e58d2008 	str	r2, [sp, #8]
    74d0:	e1cd30b0 	strh	r3, [sp]
    74d4:	e1a02001 	mov	r2, r1
    74d8:	e58d101c 	str	r1, [sp, #28]
    74dc:	e1a0100d 	mov	r1, sp
    74e0:	e1a04000 	mov	r4, r0
    74e4:	e1a0e00f 	mov	lr, pc
    74e8:	e590f104 	ldr	pc, [r0, #260]
    74ec:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    74f0:	e1a05000 	mov	r5, r0
    74f4:	e1a00004 	mov	r0, r4
    74f8:	ebffff2e 	bl	71b8 <mmc_send_status>
    74fc:	e1a00005 	mov	r0, r5
    7500:	e28dd024 	add	sp, sp, #36	; 0x24
    7504:	e8bd8030 	pop	{r4, r5, pc}

00007508 <mmc_change_freq>:
    7508:	e92d4070 	push	{r4, r5, r6, lr}
    750c:	e5903050 	ldr	r3, [r0, #80]
    7510:	e24ddc02 	sub	sp, sp, #512	; 0x200
    7514:	e3130b01 	tst	r3, #1024	; 0x400
    7518:	e3a03000 	mov	r3, #0	; 0x0
    751c:	e1a04000 	mov	r4, r0
    7520:	e580304c 	str	r3, [r0, #76]
    7524:	1a000020 	bne	75ac <mmc_change_freq+0xa4>
    7528:	e5902030 	ldr	r2, [r0, #48]
    752c:	e59f3084 	ldr	r3, [pc, #132]	; 75b8 <mmc_change_freq+0xb0>
    7530:	e1520003 	cmp	r2, r3
    7534:	9a00001c 	bls	75ac <mmc_change_freq+0xa4>
    7538:	e3a03c01 	mov	r3, #256	; 0x100
    753c:	e580304c 	str	r3, [r0, #76]
    7540:	e1a0100d 	mov	r1, sp
    7544:	ebfffd12 	bl	6994 <mmc_send_ext_csd>
    7548:	e3500000 	cmp	r0, #0	; 0x0
    754c:	e1a0600d 	mov	r6, sp
    7550:	1a000016 	bne	75b0 <mmc_change_freq+0xa8>
    7554:	e3a01001 	mov	r1, #1	; 0x1
    7558:	e1a00004 	mov	r0, r4
    755c:	e3a020b9 	mov	r2, #185	; 0xb9
    7560:	e1a03001 	mov	r3, r1
    7564:	e5dd50c4 	ldrb	r5, [sp, #196]
    7568:	ebffffce 	bl	74a8 <mmc_switch>
    756c:	e3500000 	cmp	r0, #0	; 0x0
    7570:	1a00000e 	bne	75b0 <mmc_change_freq+0xa8>
    7574:	e1a0100d 	mov	r1, sp
    7578:	e1a00004 	mov	r0, r4
    757c:	ebfffd04 	bl	6994 <mmc_send_ext_csd>
    7580:	e3500000 	cmp	r0, #0	; 0x0
    7584:	1a000009 	bne	75b0 <mmc_change_freq+0xa8>
    7588:	e5dd30b9 	ldrb	r3, [sp, #185]
    758c:	e3530000 	cmp	r3, #0	; 0x0
    7590:	0a000006 	beq	75b0 <mmc_change_freq+0xa8>
    7594:	e594304c 	ldr	r3, [r4, #76]
    7598:	e3150002 	tst	r5, #2	; 0x2
    759c:	13833011 	orrne	r3, r3, #17	; 0x11
    75a0:	03833001 	orreq	r3, r3, #1	; 0x1
    75a4:	e584304c 	str	r3, [r4, #76]
    75a8:	ea000000 	b	75b0 <mmc_change_freq+0xa8>
    75ac:	e3a00000 	mov	r0, #0	; 0x0
    75b0:	e28ddc02 	add	sp, sp, #512	; 0x200
    75b4:	e8bd8070 	pop	{r4, r5, r6, pc}
    75b8:	0001003f 	.word	0x0001003f

000075bc <mmc_startup>:
    75bc:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    75c0:	e5903050 	ldr	r3, [r0, #80]
    75c4:	e24ddf8e 	sub	sp, sp, #568	; 0x238
    75c8:	e3130b01 	tst	r3, #1024	; 0x400
    75cc:	13a0300a 	movne	r3, #10	; 0xa
    75d0:	03a03002 	moveq	r3, #2	; 0x2
    75d4:	e28d1c02 	add	r1, sp, #512	; 0x200
    75d8:	e3a02000 	mov	r2, #0	; 0x0
    75dc:	e1c131b0 	strh	r3, [r1, #16]
    75e0:	e28d5e21 	add	r5, sp, #528	; 0x210
    75e4:	e3a03007 	mov	r3, #7	; 0x7
    75e8:	e58d3214 	str	r3, [sp, #532]
    75ec:	e58d2218 	str	r2, [sp, #536]
    75f0:	e58d222c 	str	r2, [sp, #556]
    75f4:	e1a01005 	mov	r1, r5
    75f8:	e1a06000 	mov	r6, r0
    75fc:	e1a0e00f 	mov	lr, pc
    7600:	e590f104 	ldr	pc, [r0, #260]
    7604:	e2504000 	subs	r4, r0, #0	; 0x0
    7608:	1a000159 	bne	7b74 <mmc_startup+0x5b8>
    760c:	e3a02010 	mov	r2, #16	; 0x10
    7610:	e2860070 	add	r0, r6, #112	; 0x70
    7614:	e285100c 	add	r1, r5, #12	; 0xc
    7618:	ebffe74b 	bl	134c <memcpy>
    761c:	e5963050 	ldr	r3, [r6, #80]
    7620:	e2132b01 	ands	r2, r3, #1024	; 0x400
    7624:	1a000013 	bne	7678 <mmc_startup+0xbc>
    7628:	e1d638b0 	ldrh	r3, [r6, #128]
    762c:	e3a00003 	mov	r0, #3	; 0x3
    7630:	e1a03803 	lsl	r3, r3, #16
    7634:	e58d3218 	str	r3, [sp, #536]
    7638:	e3a03015 	mov	r3, #21	; 0x15
    763c:	e58d3214 	str	r3, [sp, #532]
    7640:	e28d3c02 	add	r3, sp, #512	; 0x200
    7644:	e1c301b0 	strh	r0, [r3, #16]
    7648:	e58d222c 	str	r2, [sp, #556]
    764c:	e1a01005 	mov	r1, r5
    7650:	e1a00006 	mov	r0, r6
    7654:	e1a0e00f 	mov	lr, pc
    7658:	e596f104 	ldr	pc, [r6, #260]
    765c:	e2504000 	subs	r4, r0, #0	; 0x0
    7660:	1a000143 	bne	7b74 <mmc_startup+0x5b8>
    7664:	e5963030 	ldr	r3, [r6, #48]
    7668:	e3130802 	tst	r3, #131072	; 0x20000
    766c:	128d1c02 	addne	r1, sp, #512	; 0x200
    7670:	11d111be 	ldrhne	r1, [r1, #30]
    7674:	11c618b0 	strhne	r1, [r6, #128]
    7678:	e1d638b0 	ldrh	r3, [r6, #128]
    767c:	e3a01000 	mov	r1, #0	; 0x0
    7680:	e1a03803 	lsl	r3, r3, #16
    7684:	e3a02007 	mov	r2, #7	; 0x7
    7688:	e58d3218 	str	r3, [sp, #536]
    768c:	e3a00009 	mov	r0, #9	; 0x9
    7690:	e28d3c02 	add	r3, sp, #512	; 0x200
    7694:	e1c301b0 	strh	r0, [r3, #16]
    7698:	e58d2214 	str	r2, [sp, #532]
    769c:	e58d122c 	str	r1, [sp, #556]
    76a0:	e1a02001 	mov	r2, r1
    76a4:	e1a00006 	mov	r0, r6
    76a8:	e28d1e21 	add	r1, sp, #528	; 0x210
    76ac:	e1a0e00f 	mov	lr, pc
    76b0:	e596f104 	ldr	pc, [r6, #260]
    76b4:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    76b8:	e1a04000 	mov	r4, r0
    76bc:	e1a00006 	mov	r0, r6
    76c0:	ebfffebc 	bl	71b8 <mmc_send_status>
    76c4:	e3540000 	cmp	r4, #0	; 0x0
    76c8:	1a000129 	bne	7b74 <mmc_startup+0x5b8>
    76cc:	e5963030 	ldr	r3, [r6, #48]
    76d0:	e59d021c 	ldr	r0, [sp, #540]
    76d4:	e59d2224 	ldr	r2, [sp, #548]
    76d8:	e59d1228 	ldr	r1, [sp, #552]
    76dc:	e3530801 	cmp	r3, #65536	; 0x10000
    76e0:	e59d3220 	ldr	r3, [sp, #544]
    76e4:	e5862068 	str	r2, [r6, #104]
    76e8:	e5863064 	str	r3, [r6, #100]
    76ec:	e586106c 	str	r1, [r6, #108]
    76f0:	e5860060 	str	r0, [r6, #96]
    76f4:	1a000013 	bne	7748 <mmc_startup+0x18c>
    76f8:	e1a03d20 	lsr	r3, r0, #26
    76fc:	e203300f 	and	r3, r3, #15	; 0xf
    7700:	e3530004 	cmp	r3, #4	; 0x4
    7704:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    7708:	ea00000c 	b	7740 <mmc_startup+0x184>
    770c:	00007740 	.word	0x00007740
    7710:	00007720 	.word	0x00007720
    7714:	00007728 	.word	0x00007728
    7718:	00007730 	.word	0x00007730
    771c:	00007738 	.word	0x00007738
    7720:	e59f3458 	ldr	r3, [pc, #1112]	; 7b80 <mmc_startup+0x5c4>
    7724:	ea000006 	b	7744 <mmc_startup+0x188>
    7728:	e59f3454 	ldr	r3, [pc, #1108]	; 7b84 <mmc_startup+0x5c8>
    772c:	ea000004 	b	7744 <mmc_startup+0x188>
    7730:	e59f3450 	ldr	r3, [pc, #1104]	; 7b88 <mmc_startup+0x5cc>
    7734:	ea000002 	b	7744 <mmc_startup+0x188>
    7738:	e59f344c 	ldr	r3, [pc, #1100]	; 7b8c <mmc_startup+0x5d0>
    773c:	ea000000 	b	7744 <mmc_startup+0x188>
    7740:	e59f3448 	ldr	r3, [pc, #1096]	; 7b90 <mmc_startup+0x5d4>
    7744:	e5863030 	str	r3, [r6, #48]
    7748:	e28d1c02 	add	r1, sp, #512	; 0x200
    774c:	e1d132b2 	ldrh	r3, [r1, #34]
    7750:	e3a0e001 	mov	lr, #1	; 0x1
    7754:	e203300f 	and	r3, r3, #15	; 0xf
    7758:	e1a0c31e 	lsl	ip, lr, r3
    775c:	e59d121c 	ldr	r1, [sp, #540]
    7760:	e59f242c 	ldr	r2, [pc, #1068]	; 7b94 <mmc_startup+0x5d8>
    7764:	e1a031a1 	lsr	r3, r1, #3
    7768:	e203300f 	and	r3, r3, #15	; 0xf
    776c:	e7920103 	ldr	r0, [r2, r3, lsl #2]
    7770:	e59f3420 	ldr	r3, [pc, #1056]	; 7b98 <mmc_startup+0x5dc>
    7774:	e2011007 	and	r1, r1, #7	; 0x7
    7778:	e7933101 	ldr	r3, [r3, r1, lsl #2]
    777c:	e5962030 	ldr	r2, [r6, #48]
    7780:	e0030093 	mul	r3, r3, r0
    7784:	e3120802 	tst	r2, #131072	; 0x20000
    7788:	e5863084 	str	r3, [r6, #132]
    778c:	e586c088 	str	ip, [r6, #136]
    7790:	059d3228 	ldreq	r3, [sp, #552]
    7794:	1586c08c 	strne	ip, [r6, #140]
    7798:	01a03b23 	lsreq	r3, r3, #22
    779c:	0203300f 	andeq	r3, r3, #15	; 0xf
    77a0:	01a0331e 	lsleq	r3, lr, r3
    77a4:	0586308c 	streq	r3, [r6, #140]
    77a8:	e5963040 	ldr	r3, [r6, #64]
    77ac:	e5962064 	ldr	r2, [r6, #100]
    77b0:	e3530000 	cmp	r3, #0	; 0x0
    77b4:	e5960068 	ldr	r0, [r6, #104]
    77b8:	0a000007 	beq	77dc <mmc_startup+0x220>
    77bc:	e202203f 	and	r2, r2, #63	; 0x3f
    77c0:	e1a03820 	lsr	r3, r0, #16
    77c4:	e1833802 	orr	r3, r3, r2, lsl #16
    77c8:	e1a08003 	mov	r8, r3
    77cc:	e3a09000 	mov	r9, #0	; 0x0
    77d0:	e3a04008 	mov	r4, #8	; 0x8
    77d4:	e3a05000 	mov	r5, #0	; 0x0
    77d8:	ea000009 	b	7804 <mmc_startup+0x248>
    77dc:	e1a03b02 	lsl	r3, r2, #22
    77e0:	e1a03b23 	lsr	r3, r3, #22
    77e4:	e200190e 	and	r1, r0, #229376	; 0x38000
    77e8:	e1a02f20 	lsr	r2, r0, #30
    77ec:	e1822103 	orr	r2, r2, r3, lsl #2
    77f0:	e1a017a1 	lsr	r1, r1, #15
    77f4:	e1a08002 	mov	r8, r2
    77f8:	e3a09000 	mov	r9, #0	; 0x0
    77fc:	e1a04001 	mov	r4, r1
    7800:	e3a05000 	mov	r5, #0	; 0x0
    7804:	e3a02001 	mov	r2, #1	; 0x1
    7808:	e0980002 	adds	r0, r8, r2
    780c:	e3a03000 	mov	r3, #0	; 0x0
    7810:	e0a91003 	adc	r1, r9, r3
    7814:	e2842002 	add	r2, r4, #2	; 0x2
    7818:	eb002351 	bl	10564 <__aeabi_llsl>
    781c:	e596c088 	ldr	ip, [r6, #136]
    7820:	e0832c90 	umull	r2, r3, r0, ip
    7824:	e023319c 	mla	r3, ip, r1, r3
    7828:	e35c0c02 	cmp	ip, #512	; 0x200
    782c:	e1c629f0 	strd	r2, [r6, #144]
    7830:	83a03c02 	movhi	r3, #512	; 0x200
    7834:	85863088 	strhi	r3, [r6, #136]
    7838:	e596308c 	ldr	r3, [r6, #140]
    783c:	e3530c02 	cmp	r3, #512	; 0x200
    7840:	83a03c02 	movhi	r3, #512	; 0x200
    7844:	8586308c 	strhi	r3, [r6, #140]
    7848:	e5963050 	ldr	r3, [r6, #80]
    784c:	e2131b01 	ands	r1, r3, #1024	; 0x400
    7850:	1a00000f 	bne	7894 <mmc_startup+0x2d8>
    7854:	e1d638b0 	ldrh	r3, [r6, #128]
    7858:	e3a0201d 	mov	r2, #29	; 0x1d
    785c:	e1a03803 	lsl	r3, r3, #16
    7860:	e58d2214 	str	r2, [sp, #532]
    7864:	e58d3218 	str	r3, [sp, #536]
    7868:	e28d2c02 	add	r2, sp, #512	; 0x200
    786c:	e3a03007 	mov	r3, #7	; 0x7
    7870:	e1c231b0 	strh	r3, [r2, #16]
    7874:	e58d122c 	str	r1, [sp, #556]
    7878:	e1a02001 	mov	r2, r1
    787c:	e1a00006 	mov	r0, r6
    7880:	e28d1e21 	add	r1, sp, #528	; 0x210
    7884:	e1a0e00f 	mov	lr, pc
    7888:	e596f104 	ldr	pc, [r6, #260]
    788c:	e2504000 	subs	r4, r0, #0	; 0x0
    7890:	1a0000b7 	bne	7b74 <mmc_startup+0x5b8>
    7894:	e5962030 	ldr	r2, [r6, #48]
    7898:	e3e03000 	mvn	r3, #0	; 0x0
    789c:	e3120802 	tst	r2, #131072	; 0x20000
    78a0:	e5c63082 	strb	r3, [r6, #130]
    78a4:	1a00001e 	bne	7924 <mmc_startup+0x368>
    78a8:	e59f32ec 	ldr	r3, [pc, #748]	; 7b9c <mmc_startup+0x5e0>
    78ac:	e1520003 	cmp	r2, r3
    78b0:	9a00001b 	bls	7924 <mmc_startup+0x368>
    78b4:	e28d1010 	add	r1, sp, #16	; 0x10
    78b8:	e1a00006 	mov	r0, r6
    78bc:	ebfffc34 	bl	6994 <mmc_send_ext_csd>
    78c0:	e5dd30d0 	ldrb	r3, [sp, #208]
    78c4:	e2701001 	rsbs	r1, r0, #1	; 0x1
    78c8:	33a01000 	movcc	r1, #0	; 0x0
    78cc:	e3530001 	cmp	r3, #1	; 0x1
    78d0:	93a01000 	movls	r1, #0	; 0x0
    78d4:	e3510000 	cmp	r1, #0	; 0x0
    78d8:	0a00000d 	beq	7914 <mmc_startup+0x358>
    78dc:	e5dd20e6 	ldrb	r2, [sp, #230]
    78e0:	e5dd10e5 	ldrb	r1, [sp, #229]
    78e4:	e5dd00e4 	ldrb	r0, [sp, #228]
    78e8:	e1a02802 	lsl	r2, r2, #16
    78ec:	e1822401 	orr	r2, r2, r1, lsl #8
    78f0:	e5dd10e7 	ldrb	r1, [sp, #231]
    78f4:	e1822000 	orr	r2, r2, r0
    78f8:	e1822c01 	orr	r2, r2, r1, lsl #24
    78fc:	e1a03fc2 	asr	r3, r2, #31
    7900:	e1a01483 	lsl	r1, r3, #9
    7904:	e1811ba2 	orr	r1, r1, r2, lsr #23
    7908:	e1a02482 	lsl	r2, r2, #9
    790c:	e5861094 	str	r1, [r6, #148]
    7910:	e5862090 	str	r2, [r6, #144]
    7914:	e5dd30b0 	ldrb	r3, [sp, #176]
    7918:	e3130001 	tst	r3, #1	; 0x1
    791c:	15dd30c3 	ldrbne	r3, [sp, #195]
    7920:	15c63082 	strbne	r3, [r6, #130]
    7924:	e5963030 	ldr	r3, [r6, #48]
    7928:	e3130802 	tst	r3, #131072	; 0x20000
    792c:	0a000002 	beq	793c <mmc_startup+0x380>
    7930:	e1a00006 	mov	r0, r6
    7934:	ebfffce8 	bl	6cdc <sd_change_freq>
    7938:	ea000001 	b	7944 <mmc_startup+0x388>
    793c:	e1a00006 	mov	r0, r6
    7940:	ebfffef0 	bl	7508 <mmc_change_freq>
    7944:	e3500000 	cmp	r0, #0	; 0x0
    7948:	e1a05000 	mov	r5, r0
    794c:	11a04000 	movne	r4, r0
    7950:	1a000087 	bne	7b74 <mmc_startup+0x5b8>
    7954:	e596304c 	ldr	r3, [r6, #76]
    7958:	e5962050 	ldr	r2, [r6, #80]
    795c:	e5961030 	ldr	r1, [r6, #48]
    7960:	e0022003 	and	r2, r2, r3
    7964:	e3110802 	tst	r1, #131072	; 0x20000
    7968:	e586204c 	str	r2, [r6, #76]
    796c:	e2023c01 	and	r3, r2, #256	; 0x100
    7970:	0a00002c 	beq	7a28 <mmc_startup+0x46c>
    7974:	e3530000 	cmp	r3, #0	; 0x0
    7978:	0a000023 	beq	7a0c <mmc_startup+0x450>
    797c:	e1d638b0 	ldrh	r3, [r6, #128]
    7980:	e28d2c02 	add	r2, sp, #512	; 0x200
    7984:	e1a03803 	lsl	r3, r3, #16
    7988:	e58d3218 	str	r3, [sp, #536]
    798c:	e3a08015 	mov	r8, #21	; 0x15
    7990:	e3a03037 	mov	r3, #55	; 0x37
    7994:	e28d9e21 	add	r9, sp, #528	; 0x210
    7998:	e1c231b0 	strh	r3, [r2, #16]
    799c:	e58d022c 	str	r0, [sp, #556]
    79a0:	e58d8214 	str	r8, [sp, #532]
    79a4:	e1a00006 	mov	r0, r6
    79a8:	e1a01009 	mov	r1, r9
    79ac:	e1a02005 	mov	r2, r5
    79b0:	e1a0e00f 	mov	lr, pc
    79b4:	e596f104 	ldr	pc, [r6, #260]
    79b8:	e2504000 	subs	r4, r0, #0	; 0x0
    79bc:	1a00006c 	bne	7b74 <mmc_startup+0x5b8>
    79c0:	e3a03002 	mov	r3, #2	; 0x2
    79c4:	e3a01006 	mov	r1, #6	; 0x6
    79c8:	e28d0c02 	add	r0, sp, #512	; 0x200
    79cc:	e1c011b0 	strh	r1, [r0, #16]
    79d0:	e58d8214 	str	r8, [sp, #532]
    79d4:	e58d3218 	str	r3, [sp, #536]
    79d8:	e58d522c 	str	r5, [sp, #556]
    79dc:	e1a01009 	mov	r1, r9
    79e0:	e1a02005 	mov	r2, r5
    79e4:	e1a00006 	mov	r0, r6
    79e8:	e1a0e00f 	mov	lr, pc
    79ec:	e596f104 	ldr	pc, [r6, #260]
    79f0:	e2504000 	subs	r4, r0, #0	; 0x0
    79f4:	1a00005e 	bne	7b74 <mmc_startup+0x5b8>
    79f8:	e3a03004 	mov	r3, #4	; 0x4
    79fc:	e5863044 	str	r3, [r6, #68]
    7a00:	e1a00006 	mov	r0, r6
    7a04:	e1a0e00f 	mov	lr, pc
    7a08:	e596f108 	ldr	pc, [r6, #264]
    7a0c:	e596304c 	ldr	r3, [r6, #76]
    7a10:	e3130001 	tst	r3, #1	; 0x1
    7a14:	11a00006 	movne	r0, r6
    7a18:	159f1180 	ldrne	r1, [pc, #384]	; 7ba0 <mmc_startup+0x5e4>
    7a1c:	01a00006 	moveq	r0, r6
    7a20:	059f117c 	ldreq	r1, [pc, #380]	; 7ba4 <mmc_startup+0x5e8>
    7a24:	ea000022 	b	7ab4 <mmc_startup+0x4f8>
    7a28:	e3530000 	cmp	r3, #0	; 0x0
    7a2c:	0a000008 	beq	7a54 <mmc_startup+0x498>
    7a30:	e3a01001 	mov	r1, #1	; 0x1
    7a34:	e1a03001 	mov	r3, r1
    7a38:	e1a00006 	mov	r0, r6
    7a3c:	e3a020b7 	mov	r2, #183	; 0xb7
    7a40:	ebfffe98 	bl	74a8 <mmc_switch>
    7a44:	e2504000 	subs	r4, r0, #0	; 0x0
    7a48:	03a03004 	moveq	r3, #4	; 0x4
    7a4c:	1a000048 	bne	7b74 <mmc_startup+0x5b8>
    7a50:	ea000009 	b	7a7c <mmc_startup+0x4c0>
    7a54:	e3120c02 	tst	r2, #512	; 0x200
    7a58:	0a00000b 	beq	7a8c <mmc_startup+0x4d0>
    7a5c:	e1a00006 	mov	r0, r6
    7a60:	e3a01001 	mov	r1, #1	; 0x1
    7a64:	e3a020b7 	mov	r2, #183	; 0xb7
    7a68:	e3a03002 	mov	r3, #2	; 0x2
    7a6c:	ebfffe8d 	bl	74a8 <mmc_switch>
    7a70:	e2504000 	subs	r4, r0, #0	; 0x0
    7a74:	1a00003e 	bne	7b74 <mmc_startup+0x5b8>
    7a78:	e3a03008 	mov	r3, #8	; 0x8
    7a7c:	e5863044 	str	r3, [r6, #68]
    7a80:	e1a00006 	mov	r0, r6
    7a84:	e1a0e00f 	mov	lr, pc
    7a88:	e596f108 	ldr	pc, [r6, #264]
    7a8c:	e596304c 	ldr	r3, [r6, #76]
    7a90:	e3130001 	tst	r3, #1	; 0x1
    7a94:	01a00006 	moveq	r0, r6
    7a98:	059f1108 	ldreq	r1, [pc, #264]	; 7ba8 <mmc_startup+0x5ec>
    7a9c:	0a000004 	beq	7ab4 <mmc_startup+0x4f8>
    7aa0:	e3130010 	tst	r3, #16	; 0x10
    7aa4:	159f1100 	ldrne	r1, [pc, #256]	; 7bac <mmc_startup+0x5f0>
    7aa8:	059f1100 	ldreq	r1, [pc, #256]	; 7bb0 <mmc_startup+0x5f4>
    7aac:	11a00006 	movne	r0, r6
    7ab0:	01a00006 	moveq	r0, r6
    7ab4:	ebfffbed 	bl	6a70 <mmc_set_clock>
    7ab8:	e5964094 	ldr	r4, [r6, #148]
    7abc:	e5960090 	ldr	r0, [r6, #144]
    7ac0:	e5961088 	ldr	r1, [r6, #136]
    7ac4:	e3a03000 	mov	r3, #0	; 0x0
    7ac8:	e3540000 	cmp	r4, #0	; 0x0
    7acc:	e5c630a3 	strb	r3, [r6, #163]
    7ad0:	e5c630a2 	strb	r3, [r6, #162]
    7ad4:	e58610ac 	str	r1, [r6, #172]
    7ad8:	e58d0230 	str	r0, [sp, #560]
    7adc:	e58d4234 	str	r4, [sp, #564]
    7ae0:	1a000003 	bne	7af4 <mmc_startup+0x538>
    7ae4:	eb002187 	bl	10108 <__aeabi_uidiv>
    7ae8:	e58d4234 	str	r4, [sp, #564]
    7aec:	e58d0230 	str	r0, [sp, #560]
    7af0:	ea000001 	b	7afc <mmc_startup+0x540>
    7af4:	e28d0e23 	add	r0, sp, #560	; 0x230
    7af8:	eb001541 	bl	d004 <__div64_32>
    7afc:	e5d6007f 	ldrb	r0, [r6, #127]
    7b00:	e59d1230 	ldr	r1, [sp, #560]
    7b04:	e5962070 	ldr	r2, [r6, #112]
    7b08:	e5963078 	ldr	r3, [r6, #120]
    7b0c:	e58610a8 	str	r1, [r6, #168]
    7b10:	e1803403 	orr	r3, r0, r3, lsl #8
    7b14:	e1a02422 	lsr	r2, r2, #8
    7b18:	e59f1094 	ldr	r1, [pc, #148]	; 7bb4 <mmc_startup+0x5f8>
    7b1c:	e28600b0 	add	r0, r6, #176	; 0xb0
    7b20:	eb001900 	bl	df28 <sprintf>
    7b24:	e596c074 	ldr	ip, [r6, #116]
    7b28:	e5d62070 	ldrb	r2, [r6, #112]
    7b2c:	e1a0482c 	lsr	r4, ip, #16
    7b30:	e1a0542c 	lsr	r5, ip, #8
    7b34:	e1a03c2c 	lsr	r3, ip, #24
    7b38:	e20440ff 	and	r4, r4, #255	; 0xff
    7b3c:	e20cc0ff 	and	ip, ip, #255	; 0xff
    7b40:	e59f1070 	ldr	r1, [pc, #112]	; 7bb8 <mmc_startup+0x5fc>
    7b44:	e20550ff 	and	r5, r5, #255	; 0xff
    7b48:	e28600d9 	add	r0, r6, #217	; 0xd9
    7b4c:	e88d1030 	stm	sp, {r4, r5, ip}
    7b50:	eb0018f4 	bl	df28 <sprintf>
    7b54:	e5962078 	ldr	r2, [r6, #120]
    7b58:	e28600ee 	add	r0, r6, #238	; 0xee
    7b5c:	e1a03c22 	lsr	r3, r2, #24
    7b60:	e203300f 	and	r3, r3, #15	; 0xf
    7b64:	e1a02e22 	lsr	r2, r2, #28
    7b68:	e59f104c 	ldr	r1, [pc, #76]	; 7bbc <mmc_startup+0x600>
    7b6c:	eb0018ed 	bl	df28 <sprintf>
    7b70:	e3a04000 	mov	r4, #0	; 0x0
    7b74:	e1a00004 	mov	r0, r4
    7b78:	e28ddf8e 	add	sp, sp, #568	; 0x238
    7b7c:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    7b80:	00010014 	.word	0x00010014
    7b84:	00010022 	.word	0x00010022
    7b88:	00010030 	.word	0x00010030
    7b8c:	00010040 	.word	0x00010040
    7b90:	00010012 	.word	0x00010012
    7b94:	000115a4 	.word	0x000115a4
    7b98:	00011594 	.word	0x00011594
    7b9c:	0001003f 	.word	0x0001003f
    7ba0:	02faf080 	.word	0x02faf080
    7ba4:	017d7840 	.word	0x017d7840
    7ba8:	01312d00 	.word	0x01312d00
    7bac:	03197500 	.word	0x03197500
    7bb0:	018cba80 	.word	0x018cba80
    7bb4:	000132c7 	.word	0x000132c7
    7bb8:	000132d9 	.word	0x000132d9
    7bbc:	000132e4 	.word	0x000132e4

00007bc0 <mmc_init>:
    7bc0:	e92d4010 	push	{r4, lr}
    7bc4:	ebfffb5a 	bl	6934 <find_mmc_device>
    7bc8:	e1a04000 	mov	r4, r0
    7bcc:	e1a0e00f 	mov	lr, pc
    7bd0:	e590f10c 	ldr	pc, [r0, #268]
    7bd4:	e3500000 	cmp	r0, #0	; 0x0
    7bd8:	18bd8010 	popne	{r4, pc}
    7bdc:	e3a03001 	mov	r3, #1	; 0x1
    7be0:	e5843044 	str	r3, [r4, #68]
    7be4:	e1a00004 	mov	r0, r4
    7be8:	e1a0e00f 	mov	lr, pc
    7bec:	e594f108 	ldr	pc, [r4, #264]
    7bf0:	e1a00004 	mov	r0, r4
    7bf4:	e59f1060 	ldr	r1, [pc, #96]	; 7c5c <mmc_init+0x9c>
    7bf8:	ebfffb9c 	bl	6a70 <mmc_set_clock>
    7bfc:	e1a00004 	mov	r0, r4
    7c00:	ebfffd09 	bl	702c <mmc_go_idle>
    7c04:	e3500000 	cmp	r0, #0	; 0x0
    7c08:	18bd8010 	popne	{r4, pc}
    7c0c:	e5c40083 	strb	r0, [r4, #131]
    7c10:	e1a00004 	mov	r0, r4
    7c14:	ebfffba9 	bl	6ac0 <mmc_send_if_cond>
    7c18:	e1a00004 	mov	r0, r4
    7c1c:	ebfffcad 	bl	6ed8 <sd_send_op_cond>
    7c20:	e3700013 	cmn	r0, #19	; 0x13
    7c24:	1a000009 	bne	7c50 <mmc_init+0x90>
    7c28:	e1a00004 	mov	r0, r4
    7c2c:	ebfffd13 	bl	7080 <mmc_send_op_cond>
    7c30:	e3500000 	cmp	r0, #0	; 0x0
    7c34:	0a000005 	beq	7c50 <mmc_init+0x90>
    7c38:	e59f0020 	ldr	r0, [pc, #32]	; 7c60 <mmc_init+0xa0>
    7c3c:	e59f3020 	ldr	r3, [pc, #32]	; 7c64 <mmc_init+0xa4>
    7c40:	e1a0e00f 	mov	lr, pc
    7c44:	e593f000 	ldr	pc, [r3]
    7c48:	e3e00010 	mvn	r0, #16	; 0x10
    7c4c:	e8bd8010 	pop	{r4, pc}
    7c50:	e1a00004 	mov	r0, r4
    7c54:	e8bd4010 	pop	{r4, lr}
    7c58:	eafffe57 	b	75bc <mmc_startup>
    7c5c:	00061a80 	.word	0x00061a80
    7c60:	000132ea 	.word	0x000132ea
    7c64:	0800d76c 	.word	0x0800d76c

00007c68 <mmc2_vs_reset>:
    7c68:	e3a00002 	mov	r0, #2	; 0x2
    7c6c:	eaffffd3 	b	7bc0 <mmc_init>

00007c70 <mmc1_vs_reset>:
    7c70:	e3a00001 	mov	r0, #1	; 0x1
    7c74:	eaffffd1 	b	7bc0 <mmc_init>

00007c78 <mmc0_vs_reset>:
    7c78:	e3a00000 	mov	r0, #0	; 0x0
    7c7c:	eaffffcf 	b	7bc0 <mmc_init>

00007c80 <mmc_read_blocks>:
    7c80:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7c84:	e3530001 	cmp	r3, #1	; 0x1
    7c88:	e24dd030 	sub	sp, sp, #48	; 0x30
    7c8c:	e1a06003 	mov	r6, r3
    7c90:	83a03012 	movhi	r3, #18	; 0x12
    7c94:	93a03011 	movls	r3, #17	; 0x11
    7c98:	e1cd30b0 	strh	r3, [sp]
    7c9c:	e5903040 	ldr	r3, [r0, #64]
    7ca0:	e3a07000 	mov	r7, #0	; 0x0
    7ca4:	e3530000 	cmp	r3, #0	; 0x0
    7ca8:	05903088 	ldreq	r3, [r0, #136]
    7cac:	158d2008 	strne	r2, [sp, #8]
    7cb0:	00030293 	muleq	r3, r3, r2
    7cb4:	058d3008 	streq	r3, [sp, #8]
    7cb8:	e5903088 	ldr	r3, [r0, #136]
    7cbc:	e3a02015 	mov	r2, #21	; 0x15
    7cc0:	e58d302c 	str	r3, [sp, #44]
    7cc4:	e3a03001 	mov	r3, #1	; 0x1
    7cc8:	e58d2004 	str	r2, [sp, #4]
    7ccc:	e58d1020 	str	r1, [sp, #32]
    7cd0:	e1a04000 	mov	r4, r0
    7cd4:	e58d3024 	str	r3, [sp, #36]
    7cd8:	e58d701c 	str	r7, [sp, #28]
    7cdc:	e58d6028 	str	r6, [sp, #40]
    7ce0:	e1a0100d 	mov	r1, sp
    7ce4:	e28d2020 	add	r2, sp, #32	; 0x20
    7ce8:	e1a0e00f 	mov	lr, pc
    7cec:	e594f104 	ldr	pc, [r4, #260]
    7cf0:	e2505000 	subs	r5, r0, #0	; 0x0
    7cf4:	e1a0800d 	mov	r8, sp
    7cf8:	11a00007 	movne	r0, r7
    7cfc:	1a000018 	bne	7d64 <mmc_read_blocks+0xe4>
    7d00:	e3560001 	cmp	r6, #1	; 0x1
    7d04:	9a000012 	bls	7d54 <mmc_read_blocks+0xd4>
    7d08:	e3a0301d 	mov	r3, #29	; 0x1d
    7d0c:	e58d3004 	str	r3, [sp, #4]
    7d10:	e3a0300c 	mov	r3, #12	; 0xc
    7d14:	e1cd30b0 	strh	r3, [sp]
    7d18:	e58d5008 	str	r5, [sp, #8]
    7d1c:	e58d501c 	str	r5, [sp, #28]
    7d20:	e1a0100d 	mov	r1, sp
    7d24:	e1a00004 	mov	r0, r4
    7d28:	e1a02005 	mov	r2, r5
    7d2c:	e1a0e00f 	mov	lr, pc
    7d30:	e594f104 	ldr	pc, [r4, #260]
    7d34:	e3500000 	cmp	r0, #0	; 0x0
    7d38:	0a000005 	beq	7d54 <mmc_read_blocks+0xd4>
    7d3c:	e59f0028 	ldr	r0, [pc, #40]	; 7d6c <mmc_read_blocks+0xec>
    7d40:	e59f3028 	ldr	r3, [pc, #40]	; 7d70 <mmc_read_blocks+0xf0>
    7d44:	e1a0e00f 	mov	lr, pc
    7d48:	e593f000 	ldr	pc, [r3]
    7d4c:	e1a00005 	mov	r0, r5
    7d50:	ea000003 	b	7d64 <mmc_read_blocks+0xe4>
    7d54:	e1a00004 	mov	r0, r4
    7d58:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    7d5c:	ebfffd15 	bl	71b8 <mmc_send_status>
    7d60:	e1a00006 	mov	r0, r6
    7d64:	e28dd030 	add	sp, sp, #48	; 0x30
    7d68:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    7d6c:	000132ac 	.word	0x000132ac
    7d70:	0800d76c 	.word	0x0800d76c

00007d74 <mmc_bread>:
    7d74:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    7d78:	e252a000 	subs	sl, r2, #0	; 0x0
    7d7c:	e24dd004 	sub	sp, sp, #4	; 0x4
    7d80:	e1a07001 	mov	r7, r1
    7d84:	e1a08003 	mov	r8, r3
    7d88:	0a000027 	beq	7e2c <mmc_bread+0xb8>
    7d8c:	ebfffae8 	bl	6934 <find_mmc_device>
    7d90:	e2506000 	subs	r6, r0, #0	; 0x0
    7d94:	0a000024 	beq	7e2c <mmc_bread+0xb8>
    7d98:	e59620a8 	ldr	r2, [r6, #168]
    7d9c:	e08a1007 	add	r1, sl, r7
    7da0:	e1510002 	cmp	r1, r2
    7da4:	9a000004 	bls	7dbc <mmc_bread+0x48>
    7da8:	e59f0088 	ldr	r0, [pc, #136]	; 7e38 <mmc_bread+0xc4>
    7dac:	e59f3088 	ldr	r3, [pc, #136]	; 7e3c <mmc_bread+0xc8>
    7db0:	e1a0e00f 	mov	lr, pc
    7db4:	e593f000 	ldr	pc, [r3]
    7db8:	ea00001b 	b	7e2c <mmc_bread+0xb8>
    7dbc:	e5961088 	ldr	r1, [r6, #136]
    7dc0:	ebfffacd 	bl	68fc <mmc_set_blocklen>
    7dc4:	e3500000 	cmp	r0, #0	; 0x0
    7dc8:	01a0500a 	moveq	r5, sl
    7dcc:	0a000005 	beq	7de8 <mmc_bread+0x74>
    7dd0:	e5961088 	ldr	r1, [r6, #136]
    7dd4:	e59f0064 	ldr	r0, [pc, #100]	; 7e40 <mmc_bread+0xcc>
    7dd8:	e59f305c 	ldr	r3, [pc, #92]	; 7e3c <mmc_bread+0xc8>
    7ddc:	e1a0e00f 	mov	lr, pc
    7de0:	e593f000 	ldr	pc, [r3]
    7de4:	ea000010 	b	7e2c <mmc_bread+0xb8>
    7de8:	e5964110 	ldr	r4, [r6, #272]
    7dec:	e1a02007 	mov	r2, r7
    7df0:	e1550004 	cmp	r5, r4
    7df4:	31a04005 	movcc	r4, r5
    7df8:	e1a01008 	mov	r1, r8
    7dfc:	e1a00006 	mov	r0, r6
    7e00:	e1a03004 	mov	r3, r4
    7e04:	ebffff9d 	bl	7c80 <mmc_read_blocks>
    7e08:	e1500004 	cmp	r0, r4
    7e0c:	e0877000 	add	r7, r7, r0
    7e10:	1a000005 	bne	7e2c <mmc_bread+0xb8>
    7e14:	e0555000 	subs	r5, r5, r0
    7e18:	e5962088 	ldr	r2, [r6, #136]
    7e1c:	10288092 	mlane	r8, r2, r0, r8
    7e20:	1afffff0 	bne	7de8 <mmc_bread+0x74>
    7e24:	e000029a 	mul	r0, sl, r2
    7e28:	ea000000 	b	7e30 <mmc_bread+0xbc>
    7e2c:	e3a00000 	mov	r0, #0	; 0x0
    7e30:	e28dd004 	add	sp, sp, #4	; 0x4
    7e34:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    7e38:	0001326e 	.word	0x0001326e
    7e3c:	0800d76c 	.word	0x0800d76c
    7e40:	00013313 	.word	0x00013313

00007e44 <mmc2_vs_read>:
    7e44:	e1a024a2 	lsr	r2, r2, #9
    7e48:	e1821b83 	orr	r1, r2, r3, lsl #23
    7e4c:	e59d2000 	ldr	r2, [sp]
    7e50:	e1a03000 	mov	r3, r0
    7e54:	e1a024c2 	asr	r2, r2, #9
    7e58:	e3a00002 	mov	r0, #2	; 0x2
    7e5c:	eaffffc4 	b	7d74 <mmc_bread>

00007e60 <mmc1_vs_read>:
    7e60:	e1a024a2 	lsr	r2, r2, #9
    7e64:	e1821b83 	orr	r1, r2, r3, lsl #23
    7e68:	e59d2000 	ldr	r2, [sp]
    7e6c:	e1a03000 	mov	r3, r0
    7e70:	e1a024c2 	asr	r2, r2, #9
    7e74:	e3a00001 	mov	r0, #1	; 0x1
    7e78:	eaffffbd 	b	7d74 <mmc_bread>

00007e7c <mmc0_vs_read>:
    7e7c:	e1a024a2 	lsr	r2, r2, #9
    7e80:	e1821b83 	orr	r1, r2, r3, lsl #23
    7e84:	e59d2000 	ldr	r2, [sp]
    7e88:	e1a03000 	mov	r3, r0
    7e8c:	e1a024c2 	asr	r2, r2, #9
    7e90:	e3a00000 	mov	r0, #0	; 0x0
    7e94:	eaffffb6 	b	7d74 <mmc_bread>

00007e98 <mci_pull_data32>:
    7e98:	e1a02142 	asr	r2, r2, #2
    7e9c:	e3a0c000 	mov	ip, #0	; 0x0
    7ea0:	ea000003 	b	7eb4 <mci_pull_data32+0x1c>
    7ea4:	e5903118 	ldr	r3, [r0, #280]
    7ea8:	e5933200 	ldr	r3, [r3, #512]
    7eac:	e781300c 	str	r3, [r1, ip]
    7eb0:	e28cc004 	add	ip, ip, #4	; 0x4
    7eb4:	e3520000 	cmp	r2, #0	; 0x0
    7eb8:	e2422001 	sub	r2, r2, #1	; 0x1
    7ebc:	cafffff8 	bgt	7ea4 <mci_pull_data32+0xc>
    7ec0:	e12fff1e 	bx	lr

00007ec4 <mci_push_data32>:
    7ec4:	e92d4010 	push	{r4, lr}
    7ec8:	e3a0c000 	mov	ip, #0	; 0x0
    7ecc:	e1a04000 	mov	r4, r0
    7ed0:	e1a00001 	mov	r0, r1
    7ed4:	e1a01142 	asr	r1, r2, #2
    7ed8:	ea000003 	b	7eec <mci_push_data32+0x28>
    7edc:	e790200c 	ldr	r2, [r0, ip]
    7ee0:	e5943118 	ldr	r3, [r4, #280]
    7ee4:	e28cc004 	add	ip, ip, #4	; 0x4
    7ee8:	e5832200 	str	r2, [r3, #512]
    7eec:	e3510000 	cmp	r1, #0	; 0x0
    7ef0:	e2411001 	sub	r1, r1, #1	; 0x1
    7ef4:	cafffff8 	bgt	7edc <mci_push_data32+0x18>
    7ef8:	e8bd8010 	pop	{r4, pc}

00007efc <imap_sdhc_initialize>:
    7efc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    7f00:	e3510000 	cmp	r1, #0	; 0x0
    7f04:	e24dd004 	sub	sp, sp, #4	; 0x4
    7f08:	1a000005 	bne	7f24 <imap_sdhc_initialize+0x28>
    7f0c:	e59f00dc 	ldr	r0, [pc, #220]	; 7ff0 <imap_sdhc_initialize+0xf4>
    7f10:	e59f30dc 	ldr	r3, [pc, #220]	; 7ff4 <imap_sdhc_initialize+0xf8>
    7f14:	e1a0e00f 	mov	lr, pc
    7f18:	e593f000 	ldr	pc, [r3]
    7f1c:	e3e00004 	mvn	r0, #4	; 0x4
    7f20:	ea000030 	b	7fe8 <imap_sdhc_initialize+0xec>
    7f24:	e3500000 	cmp	r0, #0	; 0x0
    7f28:	03a0359d 	moveq	r3, #658505728	; 0x27400000
    7f2c:	0a000005 	beq	7f48 <imap_sdhc_initialize+0x4c>
    7f30:	e3500001 	cmp	r0, #1	; 0x1
    7f34:	059f30bc 	ldreq	r3, [pc, #188]	; 7ff8 <imap_sdhc_initialize+0xfc>
    7f38:	0a000002 	beq	7f48 <imap_sdhc_initialize+0x4c>
    7f3c:	e3500002 	cmp	r0, #2	; 0x2
    7f40:	1a000018 	bne	7fa8 <imap_sdhc_initialize+0xac>
    7f44:	e59f30b0 	ldr	r3, [pc, #176]	; 7ffc <imap_sdhc_initialize+0x100>
    7f48:	e5813118 	str	r3, [r1, #280]
    7f4c:	e59f30ac 	ldr	r3, [pc, #172]	; 8000 <imap_sdhc_initialize+0x104>
    7f50:	e591c130 	ldr	ip, [r1, #304]
    7f54:	e5813104 	str	r3, [r1, #260]
    7f58:	e59f30a4 	ldr	r3, [pc, #164]	; 8004 <imap_sdhc_initialize+0x108>
    7f5c:	e3a02000 	mov	r2, #0	; 0x0
    7f60:	e5813108 	str	r3, [r1, #264]
    7f64:	e59f309c 	ldr	r3, [pc, #156]	; 8008 <imap_sdhc_initialize+0x10c>
    7f68:	e5812034 	str	r2, [r1, #52]
    7f6c:	e581310c 	str	r3, [r1, #268]
    7f70:	e59f3094 	ldr	r3, [pc, #148]	; 800c <imap_sdhc_initialize+0x110>
    7f74:	e5812028 	str	r2, [r1, #40]
    7f78:	e581302c 	str	r3, [r1, #44]
    7f7c:	e3003101 	movw	r3, #257	; 0x101
    7f80:	e5813050 	str	r3, [r1, #80]
    7f84:	e59f3084 	ldr	r3, [pc, #132]	; 8010 <imap_sdhc_initialize+0x114>
    7f88:	e5812110 	str	r2, [r1, #272]
    7f8c:	e5813038 	str	r3, [r1, #56]
    7f90:	e59f307c 	ldr	r3, [pc, #124]	; 8014 <imap_sdhc_initialize+0x118>
    7f94:	e5810114 	str	r0, [r1, #276]
    7f98:	e581303c 	str	r3, [r1, #60]
    7f9c:	e28c2b01 	add	r2, ip, #1024	; 0x400
    7fa0:	e28c3010 	add	r3, ip, #16	; 0x10
    7fa4:	ea000005 	b	7fc0 <imap_sdhc_initialize+0xc4>
    7fa8:	e59f0068 	ldr	r0, [pc, #104]	; 8018 <imap_sdhc_initialize+0x11c>
    7fac:	e59f3040 	ldr	r3, [pc, #64]	; 7ff4 <imap_sdhc_initialize+0xf8>
    7fb0:	e1a0e00f 	mov	lr, pc
    7fb4:	e593f000 	ldr	pc, [r3]
    7fb8:	e3e00000 	mvn	r0, #0	; 0x0
    7fbc:	ea000009 	b	7fe8 <imap_sdhc_initialize+0xec>
    7fc0:	e5033004 	str	r3, [r3, #-4]
    7fc4:	e2833010 	add	r3, r3, #16	; 0x10
    7fc8:	e1530002 	cmp	r3, r2
    7fcc:	1afffffb 	bne	7fc0 <imap_sdhc_initialize+0xc4>
    7fd0:	e3a03020 	mov	r3, #32	; 0x20
    7fd4:	e58c33f0 	str	r3, [ip, #1008]
    7fd8:	e58cc3fc 	str	ip, [ip, #1020]
    7fdc:	e1a00001 	mov	r0, r1
    7fe0:	ebfffad4 	bl	6b38 <mmc_register>
    7fe4:	e3a00000 	mov	r0, #0	; 0x0
    7fe8:	e28dd004 	add	sp, sp, #4	; 0x4
    7fec:	e8bd8000 	pop	{pc}
    7ff0:	0001331d 	.word	0x0001331d
    7ff4:	0800d76c 	.word	0x0800d76c
    7ff8:	27200000 	.word	0x27200000
    7ffc:	27300000 	.word	0x27300000
    8000:	0000808c 	.word	0x0000808c
    8004:	00008570 	.word	0x00008570
    8008:	000083b0 	.word	0x000083b0
    800c:	00ff8000 	.word	0x00ff8000
    8010:	00061a80 	.word	0x00061a80
    8014:	018cba80 	.word	0x018cba80
    8018:	00013333 	.word	0x00013333

0000801c <mci_send_cmd>:
    801c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8020:	e5903118 	ldr	r3, [r0, #280]
    8024:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    8028:	e5832028 	str	r2, [r3, #40]
    802c:	e5903118 	ldr	r3, [r0, #280]
    8030:	e24dd004 	sub	sp, sp, #4	; 0x4
    8034:	e583102c 	str	r1, [r3, #44]
    8038:	e1a04000 	mov	r4, r0
    803c:	eb0001f4 	bl	8814 <get_ticks>
    8040:	e59f5040 	ldr	r5, [pc, #64]	; 8088 <mci_send_cmd+0x6c>
    8044:	e1a06000 	mov	r6, r0
    8048:	e1a07001 	mov	r7, r1
    804c:	e5943118 	ldr	r3, [r4, #280]
    8050:	e593302c 	ldr	r3, [r3, #44]
    8054:	e3530000 	cmp	r3, #0	; 0x0
    8058:	aa000008 	bge	8080 <mci_send_cmd+0x64>
    805c:	eb0001ec 	bl	8814 <get_ticks>
    8060:	e1a02000 	mov	r2, r0
    8064:	e1a03001 	mov	r3, r1
    8068:	e0522006 	subs	r2, r2, r6
    806c:	e0c33007 	sbc	r3, r3, r7
    8070:	e3530000 	cmp	r3, #0	; 0x0
    8074:	1a000001 	bne	8080 <mci_send_cmd+0x64>
    8078:	e1520005 	cmp	r2, r5
    807c:	9afffff2 	bls	804c <mci_send_cmd+0x30>
    8080:	e28dd004 	add	sp, sp, #4	; 0x4
    8084:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8088:	0007a11f 	.word	0x0007a11f

0000808c <imap_sdhc_send_cmd>:
    808c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    8090:	e252a000 	subs	sl, r2, #0	; 0x0
    8094:	e24dd004 	sub	sp, sp, #4	; 0x4
    8098:	e1a04000 	mov	r4, r0
    809c:	e1a08001 	mov	r8, r1
    80a0:	0a000038 	beq	8188 <imap_sdhc_send_cmd+0xfc>
    80a4:	e99a000c 	ldmib	sl, {r2, r3}
    80a8:	e59a100c 	ldr	r1, [sl, #12]
    80ac:	e3120001 	tst	r2, #1	; 0x1
    80b0:	e0000193 	mul	r0, r3, r1
    80b4:	15942118 	ldrne	r2, [r4, #280]
    80b8:	11a03801 	lslne	r3, r1, #16
    80bc:	13833001 	orrne	r3, r3, #1	; 0x1
    80c0:	15823100 	strne	r3, [r2, #256]
    80c4:	e59a200c 	ldr	r2, [sl, #12]
    80c8:	e5943118 	ldr	r3, [r4, #280]
    80cc:	e1a0e000 	mov	lr, r0
    80d0:	e583201c 	str	r2, [r3, #28]
    80d4:	e5943118 	ldr	r3, [r4, #280]
    80d8:	e3a05000 	mov	r5, #0	; 0x0
    80dc:	e5830020 	str	r0, [r3, #32]
    80e0:	e5942118 	ldr	r2, [r4, #280]
    80e4:	e3a0714a 	mov	r7, #-2147483630	; 0x80000012
    80e8:	e5923000 	ldr	r3, [r2]
    80ec:	e3833020 	orr	r3, r3, #32	; 0x20
    80f0:	e5823000 	str	r3, [r2]
    80f4:	e59a6000 	ldr	r6, [sl]
    80f8:	e594c130 	ldr	ip, [r4, #304]
    80fc:	e35e0eff 	cmp	lr, #4080	; 0xff0
    8100:	31a0100e 	movcc	r1, lr
    8104:	23a01eff 	movcs	r1, #4080	; 0xff0
    8108:	e59c0004 	ldr	r0, [ip, #4]
    810c:	e59f328c 	ldr	r3, [pc, #652]	; 83a0 <imap_sdhc_send_cmd+0x314>
    8110:	e1a02a01 	lsl	r2, r1, #20
    8114:	e0003003 	and	r3, r0, r3
    8118:	e1a02a22 	lsr	r2, r2, #20
    811c:	e1822003 	orr	r2, r2, r3
    8120:	e05ee001 	subs	lr, lr, r1
    8124:	e0853006 	add	r3, r5, r6
    8128:	e98c000c 	stmib	ip, {r2, r3}
    812c:	e58c7000 	str	r7, [ip]
    8130:	e1a0200c 	mov	r2, ip
    8134:	e0855001 	add	r5, r5, r1
    8138:	e28cc010 	add	ip, ip, #16	; 0x10
    813c:	1affffee 	bne	80fc <imap_sdhc_send_cmd+0x70>
    8140:	e3c73002 	bic	r3, r7, #2	; 0x2
    8144:	e3833004 	orr	r3, r3, #4	; 0x4
    8148:	e5823000 	str	r3, [r2]
    814c:	e5942130 	ldr	r2, [r4, #304]
    8150:	e5923000 	ldr	r3, [r2]
    8154:	e3833008 	orr	r3, r3, #8	; 0x8
    8158:	e5823000 	str	r3, [r2]
    815c:	e5942118 	ldr	r2, [r4, #280]
    8160:	e5923000 	ldr	r3, [r2]
    8164:	e3833402 	orr	r3, r3, #33554432	; 0x2000000
    8168:	e5823000 	str	r3, [r2]
    816c:	e5942118 	ldr	r2, [r4, #280]
    8170:	e5923080 	ldr	r3, [r2, #128]
    8174:	e3833080 	orr	r3, r3, #128	; 0x80
    8178:	e5823080 	str	r3, [r2, #128]
    817c:	e5942118 	ldr	r2, [r4, #280]
    8180:	e3a03001 	mov	r3, #1	; 0x1
    8184:	e5823084 	str	r3, [r2, #132]
    8188:	e1d800b0 	ldrh	r0, [r8]
    818c:	e5982004 	ldr	r2, [r8, #4]
    8190:	e350000c 	cmp	r0, #12	; 0xc
    8194:	03a03901 	moveq	r3, #16384	; 0x4000
    8198:	13a03a02 	movne	r3, #8192	; 0x2000
    819c:	e3120004 	tst	r2, #4	; 0x4
    81a0:	13833c01 	orrne	r3, r3, #256	; 0x100
    81a4:	e3120001 	tst	r2, #1	; 0x1
    81a8:	e3831202 	orr	r1, r3, #536870912	; 0x20000000
    81ac:	0a000002 	beq	81bc <imap_sdhc_send_cmd+0x130>
    81b0:	e3120002 	tst	r2, #2	; 0x2
    81b4:	e3811040 	orr	r1, r1, #64	; 0x40
    81b8:	13811080 	orrne	r1, r1, #128	; 0x80
    81bc:	e35a0000 	cmp	sl, #0	; 0x0
    81c0:	0a000003 	beq	81d4 <imap_sdhc_send_cmd+0x148>
    81c4:	e59a3004 	ldr	r3, [sl, #4]
    81c8:	e3811c02 	orr	r1, r1, #512	; 0x200
    81cc:	e3130002 	tst	r3, #2	; 0x2
    81d0:	13811b01 	orrne	r1, r1, #1024	; 0x400
    81d4:	e594312c 	ldr	r3, [r4, #300]
    81d8:	e1811000 	orr	r1, r1, r0
    81dc:	e3530000 	cmp	r3, #0	; 0x0
    81e0:	13a03000 	movne	r3, #0	; 0x0
    81e4:	1584312c 	strne	r3, [r4, #300]
    81e8:	e5982008 	ldr	r2, [r8, #8]
    81ec:	e5943118 	ldr	r3, [r4, #280]
    81f0:	13811902 	orrne	r1, r1, #32768	; 0x8000
    81f4:	e5832028 	str	r2, [r3, #40]
    81f8:	e5943118 	ldr	r3, [r4, #280]
    81fc:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    8200:	e583102c 	str	r1, [r3, #44]
    8204:	eb000182 	bl	8814 <get_ticks>
    8208:	e3005bb8 	movw	r5, #3000	; 0xbb8
    820c:	e1a06000 	mov	r6, r0
    8210:	e1a07001 	mov	r7, r1
    8214:	eb00017e 	bl	8814 <get_ticks>
    8218:	e1a02000 	mov	r2, r0
    821c:	e1a03001 	mov	r3, r1
    8220:	e0522006 	subs	r2, r2, r6
    8224:	e0c33007 	sbc	r3, r3, r7
    8228:	e3530000 	cmp	r3, #0	; 0x0
    822c:	1a000007 	bne	8250 <imap_sdhc_send_cmd+0x1c4>
    8230:	e1520005 	cmp	r2, r5
    8234:	8a000005 	bhi	8250 <imap_sdhc_send_cmd+0x1c4>
    8238:	e5943118 	ldr	r3, [r4, #280]
    823c:	e5932044 	ldr	r2, [r3, #68]
    8240:	e2831044 	add	r1, r3, #68	; 0x44
    8244:	e3120004 	tst	r2, #4	; 0x4
    8248:	0afffff1 	beq	8214 <imap_sdhc_send_cmd+0x188>
    824c:	ea000005 	b	8268 <imap_sdhc_send_cmd+0x1dc>
    8250:	e59f014c 	ldr	r0, [pc, #332]	; 83a4 <imap_sdhc_send_cmd+0x318>
    8254:	e59f314c 	ldr	r3, [pc, #332]	; 83a8 <imap_sdhc_send_cmd+0x31c>
    8258:	e1a0e00f 	mov	lr, pc
    825c:	e593f000 	ldr	pc, [r3]
    8260:	e3e00011 	mvn	r0, #17	; 0x11
    8264:	ea00004b 	b	8398 <imap_sdhc_send_cmd+0x30c>
    8268:	e5915000 	ldr	r5, [r1]
    826c:	e3003142 	movw	r3, #322	; 0x142
    8270:	e0053003 	and	r3, r5, r3
    8274:	e3530000 	cmp	r3, #0	; 0x0
    8278:	e3a03004 	mov	r3, #4	; 0x4
    827c:	e5813000 	str	r3, [r1]
    8280:	0a00000e 	beq	82c0 <imap_sdhc_send_cmd+0x234>
    8284:	e1d810b0 	ldrh	r1, [r8]
    8288:	e5943118 	ldr	r3, [r4, #280]
    828c:	e3510037 	cmp	r1, #55	; 0x37
    8290:	13510008 	cmpne	r1, #8	; 0x8
    8294:	e5835044 	str	r5, [r3, #68]
    8298:	0a000004 	beq	82b0 <imap_sdhc_send_cmd+0x224>
    829c:	e59f0108 	ldr	r0, [pc, #264]	; 83ac <imap_sdhc_send_cmd+0x320>
    82a0:	e1a02005 	mov	r2, r5
    82a4:	e59f30fc 	ldr	r3, [pc, #252]	; 83a8 <imap_sdhc_send_cmd+0x31c>
    82a8:	e1a0e00f 	mov	lr, pc
    82ac:	e593f000 	ldr	pc, [r3]
    82b0:	e3150c01 	tst	r5, #256	; 0x100
    82b4:	13e00012 	mvnne	r0, #18	; 0x12
    82b8:	03e00011 	mvneq	r0, #17	; 0x11
    82bc:	ea000035 	b	8398 <imap_sdhc_send_cmd+0x30c>
    82c0:	e5983004 	ldr	r3, [r8, #4]
    82c4:	e5942118 	ldr	r2, [r4, #280]
    82c8:	e3130002 	tst	r3, #2	; 0x2
    82cc:	05923030 	ldreq	r3, [r2, #48]
    82d0:	0588300c 	streq	r3, [r8, #12]
    82d4:	0a00000a 	beq	8304 <imap_sdhc_send_cmd+0x278>
    82d8:	e592303c 	ldr	r3, [r2, #60]
    82dc:	e588300c 	str	r3, [r8, #12]
    82e0:	e5943118 	ldr	r3, [r4, #280]
    82e4:	e5933038 	ldr	r3, [r3, #56]
    82e8:	e5883010 	str	r3, [r8, #16]
    82ec:	e5943118 	ldr	r3, [r4, #280]
    82f0:	e5933034 	ldr	r3, [r3, #52]
    82f4:	e5883014 	str	r3, [r8, #20]
    82f8:	e5943118 	ldr	r3, [r4, #280]
    82fc:	e5933030 	ldr	r3, [r3, #48]
    8300:	e5883018 	str	r3, [r8, #24]
    8304:	e35a0000 	cmp	sl, #0	; 0x0
    8308:	01a0000a 	moveq	r0, sl
    830c:	0a000021 	beq	8398 <imap_sdhc_send_cmd+0x30c>
    8310:	e594c118 	ldr	ip, [r4, #280]
    8314:	e59a3004 	ldr	r3, [sl, #4]
    8318:	e28c7044 	add	r7, ip, #68	; 0x44
    831c:	e2030001 	and	r0, r3, #1	; 0x1
    8320:	e28c608c 	add	r6, ip, #140	; 0x8c
    8324:	e5975000 	ldr	r5, [r7]
    8328:	e3500000 	cmp	r0, #0	; 0x0
    832c:	e596e000 	ldr	lr, [r6]
    8330:	e2051008 	and	r1, r5, #8	; 0x8
    8334:	0a000005 	beq	8350 <imap_sdhc_send_cmd+0x2c4>
    8338:	e3510000 	cmp	r1, #0	; 0x0
    833c:	0afffff8 	beq	8324 <imap_sdhc_send_cmd+0x298>
    8340:	e31e0002 	tst	lr, #2	; 0x2
    8344:	0afffff6 	beq	8324 <imap_sdhc_send_cmd+0x298>
    8348:	e3a03002 	mov	r3, #2	; 0x2
    834c:	ea000004 	b	8364 <imap_sdhc_send_cmd+0x2d8>
    8350:	e3510000 	cmp	r1, #0	; 0x0
    8354:	0afffff2 	beq	8324 <imap_sdhc_send_cmd+0x298>
    8358:	e31e0001 	tst	lr, #1	; 0x1
    835c:	0afffff0 	beq	8324 <imap_sdhc_send_cmd+0x298>
    8360:	e3a03001 	mov	r3, #1	; 0x1
    8364:	e58c308c 	str	r3, [ip, #140]
    8368:	e5943118 	ldr	r3, [r4, #280]
    836c:	e3a00000 	mov	r0, #0	; 0x0
    8370:	e5835044 	str	r5, [r3, #68]
    8374:	e5942118 	ldr	r2, [r4, #280]
    8378:	e5923000 	ldr	r3, [r2]
    837c:	e3c33402 	bic	r3, r3, #33554432	; 0x2000000
    8380:	e3833004 	orr	r3, r3, #4	; 0x4
    8384:	e5823000 	str	r3, [r2]
    8388:	e5942118 	ldr	r2, [r4, #280]
    838c:	e5923080 	ldr	r3, [r2, #128]
    8390:	e3c33080 	bic	r3, r3, #128	; 0x80
    8394:	e5823080 	str	r3, [r2, #128]
    8398:	e28dd004 	add	sp, sp, #4	; 0x4
    839c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    83a0:	03ffe000 	.word	0x03ffe000
    83a4:	00013345 	.word	0x00013345
    83a8:	0800d76c 	.word	0x0800d76c
    83ac:	00013353 	.word	0x00013353

000083b0 <imap_sdhc_init>:
    83b0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    83b4:	e5901114 	ldr	r1, [r0, #276]
    83b8:	e3a02000 	mov	r2, #0	; 0x0
    83bc:	e3a03001 	mov	r3, #1	; 0x1
    83c0:	e3510001 	cmp	r1, #1	; 0x1
    83c4:	e24dd004 	sub	sp, sp, #4	; 0x4
    83c8:	e1a04000 	mov	r4, r0
    83cc:	e5802048 	str	r2, [r0, #72]
    83d0:	e580312c 	str	r3, [r0, #300]
    83d4:	e580211c 	str	r2, [r0, #284]
    83d8:	0a000007 	beq	83fc <imap_sdhc_init+0x4c>
    83dc:	3a000002 	bcc	83ec <imap_sdhc_init+0x3c>
    83e0:	e3510002 	cmp	r1, #2	; 0x2
    83e4:	1a00000c 	bne	841c <imap_sdhc_init+0x6c>
    83e8:	ea000007 	b	840c <imap_sdhc_init+0x5c>
    83ec:	e3a0000c 	mov	r0, #12	; 0xc
    83f0:	ebffe111 	bl	83c <pads_init_module>
    83f4:	e59f0150 	ldr	r0, [pc, #336]	; 854c <imap_sdhc_init+0x19c>
    83f8:	ea000006 	b	8418 <imap_sdhc_init+0x68>
    83fc:	e3a0000d 	mov	r0, #13	; 0xd
    8400:	ebffe10d 	bl	83c <pads_init_module>
    8404:	e59f0144 	ldr	r0, [pc, #324]	; 8550 <imap_sdhc_init+0x1a0>
    8408:	ea000002 	b	8418 <imap_sdhc_init+0x68>
    840c:	e3a0000e 	mov	r0, #14	; 0xe
    8410:	ebffe109 	bl	83c <pads_init_module>
    8414:	e59f0138 	ldr	r0, [pc, #312]	; 8554 <imap_sdhc_init+0x1a4>
    8418:	ebffe0c0 	bl	720 <module_enable>
    841c:	e5942118 	ldr	r2, [r4, #280]
    8420:	e59f5130 	ldr	r5, [pc, #304]	; 8558 <imap_sdhc_init+0x1a8>
    8424:	e5923070 	ldr	r3, [r2, #112]
    8428:	e1a03943 	asr	r3, r3, #18
    842c:	e2033007 	and	r3, r3, #7	; 0x7
    8430:	e3530002 	cmp	r3, #2	; 0x2
    8434:	03a03003 	moveq	r3, #3	; 0x3
    8438:	13a03002 	movne	r3, #2	; 0x2
    843c:	e5843120 	str	r3, [r4, #288]
    8440:	e3a03007 	mov	r3, #7	; 0x7
    8444:	e5823000 	str	r3, [r2]
    8448:	e59f310c 	ldr	r3, [pc, #268]	; 855c <imap_sdhc_init+0x1ac>
    844c:	e5843124 	str	r3, [r4, #292]
    8450:	e59f3108 	ldr	r3, [pc, #264]	; 8560 <imap_sdhc_init+0x1b0>
    8454:	e5843128 	str	r3, [r4, #296]
    8458:	eb0000ed 	bl	8814 <get_ticks>
    845c:	e1a06000 	mov	r6, r0
    8460:	e1a07001 	mov	r7, r1
    8464:	e5943118 	ldr	r3, [r4, #280]
    8468:	e3a00001 	mov	r0, #1	; 0x1
    846c:	e5933000 	ldr	r3, [r3]
    8470:	e3130007 	tst	r3, #7	; 0x7
    8474:	0a00000d 	beq	84b0 <imap_sdhc_init+0x100>
    8478:	eb0000f6 	bl	8858 <udelay>
    847c:	eb0000e4 	bl	8814 <get_ticks>
    8480:	e1a02000 	mov	r2, r0
    8484:	e1a03001 	mov	r3, r1
    8488:	e0522006 	subs	r2, r2, r6
    848c:	e0c33007 	sbc	r3, r3, r7
    8490:	e3530000 	cmp	r3, #0	; 0x0
    8494:	1a000001 	bne	84a0 <imap_sdhc_init+0xf0>
    8498:	e1520005 	cmp	r2, r5
    849c:	9afffff0 	bls	8464 <imap_sdhc_init+0xb4>
    84a0:	e59f00bc 	ldr	r0, [pc, #188]	; 8564 <imap_sdhc_init+0x1b4>
    84a4:	e59f30bc 	ldr	r3, [pc, #188]	; 8568 <imap_sdhc_init+0x1b8>
    84a8:	e1a0e00f 	mov	lr, pc
    84ac:	e593f000 	ldr	pc, [r3]
    84b0:	e5943118 	ldr	r3, [r4, #280]
    84b4:	e3a01000 	mov	r1, #0	; 0x0
    84b8:	e5831024 	str	r1, [r3, #36]
    84bc:	e5943118 	ldr	r3, [r4, #280]
    84c0:	e3e02000 	mvn	r2, #0	; 0x0
    84c4:	e5832044 	str	r2, [r3, #68]
    84c8:	e5943118 	ldr	r3, [r4, #280]
    84cc:	e1a00001 	mov	r0, r1
    84d0:	e5832014 	str	r2, [r3, #20]
    84d4:	e5942118 	ldr	r2, [r4, #280]
    84d8:	e592304c 	ldr	r3, [r2, #76]
    84dc:	e1a03283 	lsl	r3, r3, #5
    84e0:	e1a03b23 	lsr	r3, r3, #22
    84e4:	e1833803 	orr	r3, r3, r3, lsl #16
    84e8:	e582304c 	str	r3, [r2, #76]
    84ec:	e5943118 	ldr	r3, [r4, #280]
    84f0:	e5831010 	str	r1, [r3, #16]
    84f4:	e5942118 	ldr	r2, [r4, #280]
    84f8:	e5923004 	ldr	r3, [r2, #4]
    84fc:	e3833001 	orr	r3, r3, #1	; 0x1
    8500:	e5823004 	str	r3, [r2, #4]
    8504:	e5943118 	ldr	r3, [r4, #280]
    8508:	e5831058 	str	r1, [r3, #88]
    850c:	e5942118 	ldr	r2, [r4, #280]
    8510:	e59f3054 	ldr	r3, [pc, #84]	; 856c <imap_sdhc_init+0x1bc>
    8514:	e5823108 	str	r3, [r2, #264]
    8518:	e5942118 	ldr	r2, [r4, #280]
    851c:	e3003103 	movw	r3, #259	; 0x103
    8520:	e5823090 	str	r3, [r2, #144]
    8524:	e5942130 	ldr	r2, [r4, #304]
    8528:	e5943118 	ldr	r3, [r4, #280]
    852c:	e5832088 	str	r2, [r3, #136]
    8530:	e5942118 	ldr	r2, [r4, #280]
    8534:	e5923080 	ldr	r3, [r2, #128]
    8538:	e3c3307c 	bic	r3, r3, #124	; 0x7c
    853c:	e3833008 	orr	r3, r3, #8	; 0x8
    8540:	e5823080 	str	r3, [r2, #128]
    8544:	e28dd004 	add	sp, sp, #4	; 0x4
    8548:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    854c:	2d039000 	.word	0x2d039000
    8550:	2d038800 	.word	0x2d038800
    8554:	2d038c00 	.word	0x2d038c00
    8558:	0007a11f 	.word	0x0007a11f
    855c:	00007e98 	.word	0x00007e98
    8560:	00007ec4 	.word	0x00007ec4
    8564:	0001336b 	.word	0x0001336b
    8568:	0800d76c 	.word	0x0800d76c
    856c:	3fa80000 	.word	0x3fa80000

00008570 <imap_sdhc_set_ios>:
    8570:	e5902048 	ldr	r2, [r0, #72]
    8574:	e590311c 	ldr	r3, [r0, #284]
    8578:	e92d4070 	push	{r4, r5, r6, lr}
    857c:	e1520003 	cmp	r2, r3
    8580:	e1a05000 	mov	r5, r0
    8584:	0a00002a 	beq	8634 <imap_sdhc_set_ios+0xc4>
    8588:	e5903114 	ldr	r3, [r0, #276]
    858c:	e59f00d0 	ldr	r0, [pc, #208]	; 8664 <imap_sdhc_set_ios+0xf4>
    8590:	e0830000 	add	r0, r3, r0
    8594:	e1a00200 	lsl	r0, r0, #4
    8598:	ebffe290 	bl	fe0 <module_get_clock>
    859c:	e5954048 	ldr	r4, [r5, #72]
    85a0:	e1a06000 	mov	r6, r0
    85a4:	e1a01004 	mov	r1, r4
    85a8:	eb001f52 	bl	102f8 <__aeabi_uidivmod>
    85ac:	e3510000 	cmp	r1, #0	; 0x0
    85b0:	0a000005 	beq	85cc <imap_sdhc_set_ios+0x5c>
    85b4:	e1a00006 	mov	r0, r6
    85b8:	e1a01004 	mov	r1, r4
    85bc:	eb001ed1 	bl	10108 <__aeabi_uidiv>
    85c0:	e1a000a0 	lsr	r0, r0, #1
    85c4:	e2806001 	add	r6, r0, #1	; 0x1
    85c8:	ea000003 	b	85dc <imap_sdhc_set_ios+0x6c>
    85cc:	e1a00006 	mov	r0, r6
    85d0:	e1a01004 	mov	r1, r4
    85d4:	eb001ecb 	bl	10108 <__aeabi_uidiv>
    85d8:	e1a060a0 	lsr	r6, r0, #1
    85dc:	e5953118 	ldr	r3, [r5, #280]
    85e0:	e3a04000 	mov	r4, #0	; 0x0
    85e4:	e5834010 	str	r4, [r3, #16]
    85e8:	e5953118 	ldr	r3, [r5, #280]
    85ec:	e1a00005 	mov	r0, r5
    85f0:	e583400c 	str	r4, [r3, #12]
    85f4:	e59f106c 	ldr	r1, [pc, #108]	; 8668 <imap_sdhc_set_ios+0xf8>
    85f8:	e1a02004 	mov	r2, r4
    85fc:	ebfffe86 	bl	801c <mci_send_cmd>
    8600:	e5953118 	ldr	r3, [r5, #280]
    8604:	e1a00005 	mov	r0, r5
    8608:	e5836008 	str	r6, [r3, #8]
    860c:	e59f1054 	ldr	r1, [pc, #84]	; 8668 <imap_sdhc_set_ios+0xf8>
    8610:	e1a02004 	mov	r2, r4
    8614:	ebfffe80 	bl	801c <mci_send_cmd>
    8618:	e5952118 	ldr	r2, [r5, #280]
    861c:	e3a03001 	mov	r3, #1	; 0x1
    8620:	e5823010 	str	r3, [r2, #16]
    8624:	e1a00005 	mov	r0, r5
    8628:	e1a02004 	mov	r2, r4
    862c:	e59f1034 	ldr	r1, [pc, #52]	; 8668 <imap_sdhc_set_ios+0xf8>
    8630:	ebfffe79 	bl	801c <mci_send_cmd>
    8634:	e5953044 	ldr	r3, [r5, #68]
    8638:	e3530004 	cmp	r3, #4	; 0x4
    863c:	05952118 	ldreq	r2, [r5, #280]
    8640:	03a03001 	moveq	r3, #1	; 0x1
    8644:	05823018 	streq	r3, [r2, #24]
    8648:	08bd8070 	popeq	{r4, r5, r6, pc}
    864c:	e3530008 	cmp	r3, #8	; 0x8
    8650:	e5950118 	ldr	r0, [r5, #280]
    8654:	059f3010 	ldreq	r3, [pc, #16]	; 866c <imap_sdhc_set_ios+0xfc>
    8658:	13a03000 	movne	r3, #0	; 0x0
    865c:	e5803018 	str	r3, [r0, #24]
    8660:	e8bd8070 	pop	{r4, r5, r6, pc}
    8664:	02d00a24 	.word	0x02d00a24
    8668:	00202000 	.word	0x00202000
    866c:	00010001 	.word	0x00010001

00008670 <serial_tstc>:
    8670:	e59f3010 	ldr	r3, [pc, #16]	; 8688 <serial_tstc+0x18>
    8674:	e5930018 	ldr	r0, [r3, #24]
    8678:	e1a00220 	lsr	r0, r0, #4
    867c:	e2200001 	eor	r0, r0, #1	; 0x1
    8680:	e2000001 	and	r0, r0, #1	; 0x1
    8684:	e12fff1e 	bx	lr
    8688:	20803000 	.word	0x20803000

0000868c <serial_putc>:
    868c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8690:	e24dd004 	sub	sp, sp, #4	; 0x4
    8694:	e1a06000 	mov	r6, r0
    8698:	eb00005d 	bl	8814 <get_ticks>
    869c:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    86a0:	e3a05000 	mov	r5, #0	; 0x0
    86a4:	e0944000 	adds	r4, r4, r0
    86a8:	e59f7034 	ldr	r7, [pc, #52]	; 86e4 <serial_putc+0x58>
    86ac:	e0a55001 	adc	r5, r5, r1
    86b0:	e5973018 	ldr	r3, [r7, #24]
    86b4:	e3130020 	tst	r3, #32	; 0x20
    86b8:	0a000005 	beq	86d4 <serial_putc+0x48>
    86bc:	eb000054 	bl	8814 <get_ticks>
    86c0:	e1510005 	cmp	r1, r5
    86c4:	8a000002 	bhi	86d4 <serial_putc+0x48>
    86c8:	1afffff8 	bne	86b0 <serial_putc+0x24>
    86cc:	e1500004 	cmp	r0, r4
    86d0:	9afffff6 	bls	86b0 <serial_putc+0x24>
    86d4:	e59f3008 	ldr	r3, [pc, #8]	; 86e4 <serial_putc+0x58>
    86d8:	e5836000 	str	r6, [r3]
    86dc:	e28dd004 	add	sp, sp, #4	; 0x4
    86e0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    86e4:	20803000 	.word	0x20803000

000086e8 <serial_puts>:
    86e8:	e92d4030 	push	{r4, r5, lr}
    86ec:	e1a05000 	mov	r5, r0
    86f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    86f4:	e3a04000 	mov	r4, #0	; 0x0
    86f8:	ea000006 	b	8718 <serial_puts+0x30>
    86fc:	e353000a 	cmp	r3, #10	; 0xa
    8700:	1a000001 	bne	870c <serial_puts+0x24>
    8704:	e3a0000d 	mov	r0, #13	; 0xd
    8708:	ebffffdf 	bl	868c <serial_putc>
    870c:	e7d50004 	ldrb	r0, [r5, r4]
    8710:	ebffffdd 	bl	868c <serial_putc>
    8714:	e2844001 	add	r4, r4, #1	; 0x1
    8718:	e7d53004 	ldrb	r3, [r5, r4]
    871c:	e3530000 	cmp	r3, #0	; 0x0
    8720:	1afffff5 	bne	86fc <serial_puts+0x14>
    8724:	e28dd004 	add	sp, sp, #4	; 0x4
    8728:	e8bd8030 	pop	{r4, r5, pc}

0000872c <serial_getc>:
    872c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8730:	e59f2024 	ldr	r2, [pc, #36]	; 875c <serial_getc+0x30>
    8734:	e24dd004 	sub	sp, sp, #4	; 0x4
    8738:	e5923018 	ldr	r3, [r2, #24]
    873c:	e3130010 	tst	r3, #16	; 0x10
    8740:	1afffffc 	bne	8738 <serial_getc+0xc>
    8744:	ebffef19 	bl	43b0 <tt_rnd_seed>
    8748:	e59f300c 	ldr	r3, [pc, #12]	; 875c <serial_getc+0x30>
    874c:	e5930000 	ldr	r0, [r3]
    8750:	e6ef0070 	uxtb	r0, r0
    8754:	e28dd004 	add	sp, sp, #4	; 0x4
    8758:	e8bd8000 	pop	{pc}
    875c:	20803000 	.word	0x20803000

00008760 <serial_setbrg>:
    8760:	e92d4010 	push	{r4, lr}
    8764:	e59f0038 	ldr	r0, [pc, #56]	; 87a4 <serial_setbrg+0x44>
    8768:	ebffe21c 	bl	fe0 <module_get_clock>
    876c:	e59f1034 	ldr	r1, [pc, #52]	; 87a8 <serial_setbrg+0x48>
    8770:	e1a04000 	mov	r4, r0
    8774:	eb001ee5 	bl	10310 <__aeabi_idiv>
    8778:	e6bf3070 	sxth	r3, r0
    877c:	e59f0028 	ldr	r0, [pc, #40]	; 87ac <serial_setbrg+0x4c>
    8780:	e59f1020 	ldr	r1, [pc, #32]	; 87a8 <serial_setbrg+0x48>
    8784:	e0204390 	mla	r0, r0, r3, r4
    8788:	e59f4020 	ldr	r4, [pc, #32]	; 87b0 <serial_setbrg+0x50>
    878c:	e1a00300 	lsl	r0, r0, #6
    8790:	e5843024 	str	r3, [r4, #36]
    8794:	eb001edd 	bl	10310 <__aeabi_idiv>
    8798:	e200003f 	and	r0, r0, #63	; 0x3f
    879c:	e5840028 	str	r0, [r4, #40]
    87a0:	e8bd8010 	pop	{r4, pc}
    87a4:	2d00a290 	.word	0x2d00a290
    87a8:	001c2000 	.word	0x001c2000
    87ac:	ffe3e000 	.word	0xffe3e000
    87b0:	20803000 	.word	0x20803000

000087b4 <init_serial>:
    87b4:	e92d4010 	push	{r4, lr}
    87b8:	e59f004c 	ldr	r0, [pc, #76]	; 880c <init_serial+0x58>
    87bc:	e59f404c 	ldr	r4, [pc, #76]	; 8810 <init_serial+0x5c>
    87c0:	ebffdfd6 	bl	720 <module_enable>
    87c4:	e3a0000f 	mov	r0, #15	; 0xf
    87c8:	ebffe01b 	bl	83c <pads_init_module>
    87cc:	e594302c 	ldr	r3, [r4, #44]
    87d0:	e3c330ff 	bic	r3, r3, #255	; 0xff
    87d4:	e3833060 	orr	r3, r3, #96	; 0x60
    87d8:	e584302c 	str	r3, [r4, #44]
    87dc:	ebffffdf 	bl	8760 <serial_setbrg>
    87e0:	e5943030 	ldr	r3, [r4, #48]
    87e4:	e3a00000 	mov	r0, #0	; 0x0
    87e8:	e3c33cff 	bic	r3, r3, #65280	; 0xff00
    87ec:	e3c33087 	bic	r3, r3, #135	; 0x87
    87f0:	e3833c03 	orr	r3, r3, #768	; 0x300
    87f4:	e3833001 	orr	r3, r3, #1	; 0x1
    87f8:	e5843030 	str	r3, [r4, #48]
    87fc:	e594302c 	ldr	r3, [r4, #44]
    8800:	e3833010 	orr	r3, r3, #16	; 0x10
    8804:	e584302c 	str	r3, [r4, #44]
    8808:	e8bd8010 	pop	{r4, pc}
    880c:	2d012000 	.word	0x2d012000
    8810:	20803000 	.word	0x20803000

00008814 <get_ticks>:
    8814:	e59f1038 	ldr	r1, [pc, #56]	; 8854 <get_ticks+0x40>
    8818:	e5913204 	ldr	r3, [r1, #516]
    881c:	e5912200 	ldr	r2, [r1, #512]
    8820:	e591c204 	ldr	ip, [r1, #516]
    8824:	e15c0003 	cmp	ip, r3
    8828:	15913200 	ldrne	r3, [r1, #512]
    882c:	01a00002 	moveq	r0, r2
    8830:	03a01000 	moveq	r1, #0	; 0x0
    8834:	11a00003 	movne	r0, r3
    8838:	13a01000 	movne	r1, #0	; 0x0
    883c:	e3a02000 	mov	r2, #0	; 0x0
    8840:	e1822000 	orr	r2, r2, r0
    8844:	e18c3001 	orr	r3, ip, r1
    8848:	e1a01003 	mov	r1, r3
    884c:	e1a00002 	mov	r0, r2
    8850:	e12fff1e 	bx	lr
    8854:	21b00000 	.word	0x21b00000

00008858 <udelay>:
    8858:	e59f1084 	ldr	r1, [pc, #132]	; 88e4 <udelay+0x8c>
    885c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8860:	e5913204 	ldr	r3, [r1, #516]
    8864:	e5912200 	ldr	r2, [r1, #512]
    8868:	e591c204 	ldr	ip, [r1, #516]
    886c:	e15c0003 	cmp	ip, r3
    8870:	15913200 	ldrne	r3, [r1, #512]
    8874:	01a04002 	moveq	r4, r2
    8878:	03a05000 	moveq	r5, #0	; 0x0
    887c:	11a04003 	movne	r4, r3
    8880:	13a05000 	movne	r5, #0	; 0x0
    8884:	e3a02000 	mov	r2, #0	; 0x0
    8888:	e1822004 	orr	r2, r2, r4
    888c:	e18c3005 	orr	r3, ip, r5
    8890:	e0926000 	adds	r6, r2, r0
    8894:	e59fc048 	ldr	ip, [pc, #72]	; 88e4 <udelay+0x8c>
    8898:	e2a37000 	adc	r7, r3, #0	; 0x0
    889c:	e59c3204 	ldr	r3, [ip, #516]
    88a0:	e59c2200 	ldr	r2, [ip, #512]
    88a4:	e59c1204 	ldr	r1, [ip, #516]
    88a8:	e1510003 	cmp	r1, r3
    88ac:	159c3200 	ldrne	r3, [ip, #512]
    88b0:	01a04002 	moveq	r4, r2
    88b4:	03a05000 	moveq	r5, #0	; 0x0
    88b8:	11a04003 	movne	r4, r3
    88bc:	13a05000 	movne	r5, #0	; 0x0
    88c0:	e1851001 	orr	r1, r5, r1
    88c4:	e3a02000 	mov	r2, #0	; 0x0
    88c8:	e1570001 	cmp	r7, r1
    88cc:	e1840002 	orr	r0, r4, r2
    88d0:	8afffff1 	bhi	889c <udelay+0x44>
    88d4:	18bd80f0 	popne	{r4, r5, r6, r7, pc}
    88d8:	e1560000 	cmp	r6, r0
    88dc:	8affffee 	bhi	889c <udelay+0x44>
    88e0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    88e4:	21b00000 	.word	0x21b00000

000088e8 <get_timer>:
    88e8:	e59f2074 	ldr	r2, [pc, #116]	; 8964 <get_timer+0x7c>
    88ec:	e92d4070 	push	{r4, r5, r6, lr}
    88f0:	e5923204 	ldr	r3, [r2, #516]
    88f4:	e1a06000 	mov	r6, r0
    88f8:	e5921200 	ldr	r1, [r2, #512]
    88fc:	e5920204 	ldr	r0, [r2, #516]
    8900:	e24dd008 	sub	sp, sp, #8	; 0x8
    8904:	e1500003 	cmp	r0, r3
    8908:	15923200 	ldrne	r3, [r2, #512]
    890c:	01a04001 	moveq	r4, r1
    8910:	03a05000 	moveq	r5, #0	; 0x0
    8914:	11a04003 	movne	r4, r3
    8918:	13a05000 	movne	r5, #0	; 0x0
    891c:	e1a03000 	mov	r3, r0
    8920:	e3a02000 	mov	r2, #0	; 0x0
    8924:	e1851003 	orr	r1, r5, r3
    8928:	e1840002 	orr	r0, r4, r2
    892c:	e1510002 	cmp	r1, r2
    8930:	e88d0003 	stm	sp, {r0, r1}
    8934:	1a000003 	bne	8948 <get_timer+0x60>
    8938:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    893c:	eb001df1 	bl	10108 <__aeabi_uidiv>
    8940:	e58d0000 	str	r0, [sp]
    8944:	ea000002 	b	8954 <get_timer+0x6c>
    8948:	e1a0000d 	mov	r0, sp
    894c:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8950:	eb0011ab 	bl	d004 <__div64_32>
    8954:	e59d0000 	ldr	r0, [sp]
    8958:	e0660000 	rsb	r0, r6, r0
    895c:	e28dd008 	add	sp, sp, #8	; 0x8
    8960:	e8bd8070 	pop	{r4, r5, r6, pc}
    8964:	21b00000 	.word	0x21b00000

00008968 <init_timer>:
    8968:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    896c:	e3a00003 	mov	r0, #3	; 0x3
    8970:	e24dd004 	sub	sp, sp, #4	; 0x4
    8974:	ebffe199 	bl	fe0 <module_get_clock>
    8978:	e59f1030 	ldr	r1, [pc, #48]	; 89b0 <init_timer+0x48>
    897c:	eb001e63 	bl	10310 <__aeabi_idiv>
    8980:	e2401001 	sub	r1, r0, #1	; 0x1
    8984:	e59f2028 	ldr	r2, [pc, #40]	; 89b4 <init_timer+0x4c>
    8988:	e1a01401 	lsl	r1, r1, #8
    898c:	e3a03000 	mov	r3, #0	; 0x0
    8990:	e3811001 	orr	r1, r1, #1	; 0x1
    8994:	e5823208 	str	r3, [r2, #520]
    8998:	e1a00003 	mov	r0, r3
    899c:	e5823200 	str	r3, [r2, #512]
    89a0:	e5823204 	str	r3, [r2, #516]
    89a4:	e5821208 	str	r1, [r2, #520]
    89a8:	e28dd004 	add	sp, sp, #4	; 0x4
    89ac:	e8bd8000 	pop	{pc}
    89b0:	000f4240 	.word	0x000f4240
    89b4:	21b00000 	.word	0x21b00000

000089b8 <e2p_vs_align>:
    89b8:	e3a00010 	mov	r0, #16	; 0x10
    89bc:	e12fff1e 	bx	lr

000089c0 <e2p_vs_reset>:
    89c0:	e3a00000 	mov	r0, #0	; 0x0
    89c4:	e12fff1e 	bx	lr

000089c8 <iic_address_num>:
    89c8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    89cc:	e24dd004 	sub	sp, sp, #4	; 0x4
    89d0:	ebffdef0 	bl	598 <boot_info>
    89d4:	e59f3014 	ldr	r3, [pc, #20]	; 89f0 <iic_address_num+0x28>
    89d8:	e1a00080 	lsl	r0, r0, #1
    89dc:	e20000ff 	and	r0, r0, #255	; 0xff
    89e0:	e5830000 	str	r0, [r3]
    89e4:	e3a00001 	mov	r0, #1	; 0x1
    89e8:	e28dd004 	add	sp, sp, #4	; 0x4
    89ec:	e8bd8000 	pop	{pc}
    89f0:	0800d250 	.word	0x0800d250

000089f4 <iic_read_get_address>:
    89f4:	e1d020b2 	ldrh	r2, [r0, #2]
    89f8:	e590300c 	ldr	r3, [r0, #12]
    89fc:	e92d4010 	push	{r4, lr}
    8a00:	e0833002 	add	r3, r3, r2
    8a04:	e1c030b2 	strh	r3, [r0, #2]
    8a08:	e1a04000 	mov	r4, r0
    8a0c:	ebffffed 	bl	89c8 <iic_address_num>
    8a10:	e3500000 	cmp	r0, #0	; 0x0
    8a14:	1a000008 	bne	8a3c <iic_read_get_address+0x48>
    8a18:	e1d430b2 	ldrh	r3, [r4, #2]
    8a1c:	e3530c01 	cmp	r3, #256	; 0x100
    8a20:	1a000005 	bne	8a3c <iic_read_get_address+0x48>
    8a24:	e1d430b0 	ldrh	r3, [r4]
    8a28:	e1c400b2 	strh	r0, [r4, #2]
    8a2c:	e2833002 	add	r3, r3, #2	; 0x2
    8a30:	e1c430b0 	strh	r3, [r4]
    8a34:	e3a03001 	mov	r3, #1	; 0x1
    8a38:	e1c431b0 	strh	r3, [r4, #16]
    8a3c:	ebffffe1 	bl	89c8 <iic_address_num>
    8a40:	e3500000 	cmp	r0, #0	; 0x0
    8a44:	e5941008 	ldr	r1, [r4, #8]
    8a48:	0a000004 	beq	8a60 <iic_read_get_address+0x6c>
    8a4c:	e3510000 	cmp	r1, #0	; 0x0
    8a50:	13a03000 	movne	r3, #0	; 0x0
    8a54:	15843008 	strne	r3, [r4, #8]
    8a58:	1a00000e 	bne	8a98 <iic_read_get_address+0xa4>
    8a5c:	ea000010 	b	8aa4 <iic_read_get_address+0xb0>
    8a60:	e1d430b2 	ldrh	r3, [r4, #2]
    8a64:	e2632c01 	rsb	r2, r3, #256	; 0x100
    8a68:	e1520001 	cmp	r2, r1
    8a6c:	e5842004 	str	r2, [r4, #4]
    8a70:	2a000005 	bcs	8a8c <iic_read_get_address+0x98>
    8a74:	e0623001 	rsb	r3, r2, r1
    8a78:	e5843008 	str	r3, [r4, #8]
    8a7c:	e3a03001 	mov	r3, #1	; 0x1
    8a80:	e1c431b0 	strh	r3, [r4, #16]
    8a84:	e584200c 	str	r2, [r4, #12]
    8a88:	ea000003 	b	8a9c <iic_read_get_address+0xa8>
    8a8c:	e3510000 	cmp	r1, #0	; 0x0
    8a90:	0a000003 	beq	8aa4 <iic_read_get_address+0xb0>
    8a94:	e5840008 	str	r0, [r4, #8]
    8a98:	e584100c 	str	r1, [r4, #12]
    8a9c:	e3a00001 	mov	r0, #1	; 0x1
    8aa0:	e8bd8010 	pop	{r4, pc}
    8aa4:	e3a00000 	mov	r0, #0	; 0x0
    8aa8:	e8bd8010 	pop	{r4, pc}

00008aac <iic_write_get_address>:
    8aac:	e1d020b2 	ldrh	r2, [r0, #2]
    8ab0:	e590300c 	ldr	r3, [r0, #12]
    8ab4:	e92d4010 	push	{r4, lr}
    8ab8:	e0833002 	add	r3, r3, r2
    8abc:	e1c030b2 	strh	r3, [r0, #2]
    8ac0:	e1a04000 	mov	r4, r0
    8ac4:	ebffffbf 	bl	89c8 <iic_address_num>
    8ac8:	e3500000 	cmp	r0, #0	; 0x0
    8acc:	1a000008 	bne	8af4 <iic_write_get_address+0x48>
    8ad0:	e1d430b2 	ldrh	r3, [r4, #2]
    8ad4:	e3530c01 	cmp	r3, #256	; 0x100
    8ad8:	1a000005 	bne	8af4 <iic_write_get_address+0x48>
    8adc:	e1d430b0 	ldrh	r3, [r4]
    8ae0:	e1c400b2 	strh	r0, [r4, #2]
    8ae4:	e2833002 	add	r3, r3, #2	; 0x2
    8ae8:	e1c430b0 	strh	r3, [r4]
    8aec:	e3a03001 	mov	r3, #1	; 0x1
    8af0:	e1c431b0 	strh	r3, [r4, #16]
    8af4:	e1d430b2 	ldrh	r3, [r4, #2]
    8af8:	e5940008 	ldr	r0, [r4, #8]
    8afc:	e203300f 	and	r3, r3, #15	; 0xf
    8b00:	e2632010 	rsb	r2, r3, #16	; 0x10
    8b04:	e1520000 	cmp	r2, r0
    8b08:	e5842004 	str	r2, [r4, #4]
    8b0c:	2a000004 	bcs	8b24 <iic_write_get_address+0x78>
    8b10:	e0623000 	rsb	r3, r2, r0
    8b14:	e5843008 	str	r3, [r4, #8]
    8b18:	e584200c 	str	r2, [r4, #12]
    8b1c:	e3a00001 	mov	r0, #1	; 0x1
    8b20:	e8bd8010 	pop	{r4, pc}
    8b24:	e3500000 	cmp	r0, #0	; 0x0
    8b28:	13a03000 	movne	r3, #0	; 0x0
    8b2c:	1584000c 	strne	r0, [r4, #12]
    8b30:	15843008 	strne	r3, [r4, #8]
    8b34:	13a00001 	movne	r0, #1	; 0x1
    8b38:	e8bd8010 	pop	{r4, pc}

00008b3c <iic_word_addr_func>:
    8b3c:	e92d4030 	push	{r4, r5, lr}
    8b40:	e24dd004 	sub	sp, sp, #4	; 0x4
    8b44:	e1a04000 	mov	r4, r0
    8b48:	e1a05001 	mov	r5, r1
    8b4c:	ebffff9d 	bl	89c8 <iic_address_num>
    8b50:	e3500000 	cmp	r0, #0	; 0x0
    8b54:	16ff0074 	uxthne	r0, r4
    8b58:	1a00000c 	bne	8b90 <iic_word_addr_func+0x54>
    8b5c:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    8b60:	e0051003 	and	r1, r5, r3
    8b64:	e3a020ff 	mov	r2, #255	; 0xff
    8b68:	e3510000 	cmp	r1, #0	; 0x0
    8b6c:	e0040002 	and	r0, r4, r2
    8b70:	aa000005 	bge	8b8c <iic_word_addr_func+0x50>
    8b74:	e3e02000 	mvn	r2, #0	; 0x0
    8b78:	e0900002 	adds	r0, r0, r2
    8b7c:	e3e020ff 	mvn	r2, #255	; 0xff
    8b80:	e1800002 	orr	r0, r0, r2
    8b84:	e3a02001 	mov	r2, #1	; 0x1
    8b88:	e0900002 	adds	r0, r0, r2
    8b8c:	e6ff0070 	uxth	r0, r0
    8b90:	e28dd004 	add	sp, sp, #4	; 0x4
    8b94:	e8bd8030 	pop	{r4, r5, pc}

00008b98 <iic_addr_func>:
    8b98:	e92d4030 	push	{r4, r5, lr}
    8b9c:	e24dd004 	sub	sp, sp, #4	; 0x4
    8ba0:	e1a04000 	mov	r4, r0
    8ba4:	e1a05001 	mov	r5, r1
    8ba8:	ebffff86 	bl	89c8 <iic_address_num>
    8bac:	e3500000 	cmp	r0, #0	; 0x0
    8bb0:	e59f003c 	ldr	r0, [pc, #60]	; 8bf4 <iic_addr_func+0x5c>
    8bb4:	11d000b0 	ldrhne	r0, [r0]
    8bb8:	1a00000b 	bne	8bec <iic_addr_func+0x54>
    8bbc:	e1a02005 	mov	r2, r5
    8bc0:	e1a03fc2 	asr	r3, r2, #31
    8bc4:	e1a02fc3 	asr	r2, r3, #31
    8bc8:	e20220ff 	and	r2, r2, #255	; 0xff
    8bcc:	e3a03000 	mov	r3, #0	; 0x0
    8bd0:	e0922004 	adds	r2, r2, r4
    8bd4:	e0a33005 	adc	r3, r3, r5
    8bd8:	e5900000 	ldr	r0, [r0]
    8bdc:	e1a01422 	lsr	r1, r2, #8
    8be0:	e1811c03 	orr	r1, r1, r3, lsl #24
    8be4:	e1800081 	orr	r0, r0, r1, lsl #1
    8be8:	e6ff0070 	uxth	r0, r0
    8bec:	e28dd004 	add	sp, sp, #4	; 0x4
    8bf0:	e8bd8030 	pop	{r4, r5, pc}
    8bf4:	0800d250 	.word	0x0800d250

00008bf8 <e2p_vs_write>:
    8bf8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8bfc:	e1a01003 	mov	r1, r3
    8c00:	e24dd024 	sub	sp, sp, #36	; 0x24
    8c04:	e1a06000 	mov	r6, r0
    8c08:	e1a00002 	mov	r0, r2
    8c0c:	e1a05003 	mov	r5, r3
    8c10:	e1a04002 	mov	r4, r2
    8c14:	ebffffdf 	bl	8b98 <iic_addr_func>
    8c18:	e1a01005 	mov	r1, r5
    8c1c:	e1cd00bc 	strh	r0, [sp, #12]
    8c20:	e1a00004 	mov	r0, r4
    8c24:	ebffffc4 	bl	8b3c <iic_word_addr_func>
    8c28:	e3a03000 	mov	r3, #0	; 0x0
    8c2c:	e59d2048 	ldr	r2, [sp, #72]
    8c30:	e1a05003 	mov	r5, r3
    8c34:	e58d3018 	str	r3, [sp, #24]
    8c38:	e28dc00c 	add	ip, sp, #12	; 0xc
    8c3c:	e3a03001 	mov	r3, #1	; 0x1
    8c40:	e1cd00be 	strh	r0, [sp, #14]
    8c44:	e58d2014 	str	r2, [sp, #20]
    8c48:	e1cd31bc 	strh	r3, [sp, #28]
    8c4c:	e59fb10c 	ldr	fp, [pc, #268]	; 8d60 <e2p_vs_write+0x168>
    8c50:	e3a0a001 	mov	sl, #1	; 0x1
    8c54:	e1a08005 	mov	r8, r5
    8c58:	e28c9012 	add	r9, ip, #18	; 0x12
    8c5c:	ea000037 	b	8d40 <e2p_vs_write+0x148>
    8c60:	ebffff58 	bl	89c8 <iic_address_num>
    8c64:	e3500000 	cmp	r0, #0	; 0x0
    8c68:	e1dd10be 	ldrh	r1, [sp, #14]
    8c6c:	11a03421 	lsrne	r3, r1, #8
    8c70:	15cd301e 	strbne	r3, [sp, #30]
    8c74:	15cd101f 	strbne	r1, [sp, #31]
    8c78:	13a07002 	movne	r7, #2	; 0x2
    8c7c:	1a000005 	bne	8c98 <e2p_vs_write+0xa0>
    8c80:	e6ef1071 	uxtb	r1, r1
    8c84:	e5cd101e 	strb	r1, [sp, #30]
    8c88:	e59f00d4 	ldr	r0, [pc, #212]	; 8d64 <e2p_vs_write+0x16c>
    8c8c:	e1a0e00f 	mov	lr, pc
    8c90:	e59bf000 	ldr	pc, [fp]
    8c94:	e3a07001 	mov	r7, #1	; 0x1
    8c98:	ebfffedd 	bl	8814 <get_ticks>
    8c9c:	e1dd31bc 	ldrh	r3, [sp, #28]
    8ca0:	e1a04000 	mov	r4, r0
    8ca4:	e3530000 	cmp	r3, #0	; 0x0
    8ca8:	0a000016 	beq	8d08 <e2p_vs_write+0x110>
    8cac:	e3a0c000 	mov	ip, #0	; 0x0
    8cb0:	e5dd200c 	ldrb	r2, [sp, #12]
    8cb4:	e1a00008 	mov	r0, r8
    8cb8:	e1a0100a 	mov	r1, sl
    8cbc:	e1a03008 	mov	r3, r8
    8cc0:	e1cdc1bc 	strh	ip, [sp, #28]
    8cc4:	e58da000 	str	sl, [sp]
    8cc8:	e58d8004 	str	r8, [sp, #4]
    8ccc:	eb000137 	bl	91b0 <iic_init>
    8cd0:	e3500000 	cmp	r0, #0	; 0x0
    8cd4:	1a00000b 	bne	8d08 <e2p_vs_write+0x110>
    8cd8:	e1a00008 	mov	r0, r8
    8cdc:	ea00001d 	b	8d58 <e2p_vs_write+0x160>
    8ce0:	ebfffecb 	bl	8814 <get_ticks>
    8ce4:	e1540000 	cmp	r4, r0
    8ce8:	2a000008 	bcs	8d10 <e2p_vs_write+0x118>
    8cec:	e59f0074 	ldr	r0, [pc, #116]	; 8d68 <e2p_vs_write+0x170>
    8cf0:	e1a0e00f 	mov	lr, pc
    8cf4:	e59bf000 	ldr	pc, [fp]
    8cf8:	e59d3018 	ldr	r3, [sp, #24]
    8cfc:	e0853003 	add	r3, r5, r3
    8d00:	e6ff5073 	uxth	r5, r3
    8d04:	ea00000b 	b	8d38 <e2p_vs_write+0x140>
    8d08:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    8d0c:	e2844e12 	add	r4, r4, #288	; 0x120
    8d10:	e59dc018 	ldr	ip, [sp, #24]
    8d14:	e1a01009 	mov	r1, r9
    8d18:	e1a02007 	mov	r2, r7
    8d1c:	e1a03006 	mov	r3, r6
    8d20:	e3a00000 	mov	r0, #0	; 0x0
    8d24:	e58dc000 	str	ip, [sp]
    8d28:	e58da004 	str	sl, [sp, #4]
    8d2c:	eb000078 	bl	8f14 <iicreg_write>
    8d30:	e3500000 	cmp	r0, #0	; 0x0
    8d34:	0affffe9 	beq	8ce0 <e2p_vs_write+0xe8>
    8d38:	e59d3018 	ldr	r3, [sp, #24]
    8d3c:	e0866003 	add	r6, r6, r3
    8d40:	e28d000c 	add	r0, sp, #12	; 0xc
    8d44:	ebffff58 	bl	8aac <iic_write_get_address>
    8d48:	e3500000 	cmp	r0, #0	; 0x0
    8d4c:	1affffc3 	bne	8c60 <e2p_vs_write+0x68>
    8d50:	e59d2048 	ldr	r2, [sp, #72]
    8d54:	e0650002 	rsb	r0, r5, r2
    8d58:	e28dd024 	add	sp, sp, #36	; 0x24
    8d5c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    8d60:	0800d76c 	.word	0x0800d76c
    8d64:	0001337e 	.word	0x0001337e
    8d68:	00013b60 	.word	0x00013b60

00008d6c <e2p_vs_read>:
    8d6c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8d70:	e1a01003 	mov	r1, r3
    8d74:	e24dd024 	sub	sp, sp, #36	; 0x24
    8d78:	e1a06000 	mov	r6, r0
    8d7c:	e1a00002 	mov	r0, r2
    8d80:	e1a05003 	mov	r5, r3
    8d84:	e1a04002 	mov	r4, r2
    8d88:	e59d7048 	ldr	r7, [sp, #72]
    8d8c:	ebffff81 	bl	8b98 <iic_addr_func>
    8d90:	e1a01005 	mov	r1, r5
    8d94:	e1cd00bc 	strh	r0, [sp, #12]
    8d98:	e1a00004 	mov	r0, r4
    8d9c:	ebffff66 	bl	8b3c <iic_word_addr_func>
    8da0:	e28db00c 	add	fp, sp, #12	; 0xc
    8da4:	e3a03000 	mov	r3, #0	; 0x0
    8da8:	e3a02001 	mov	r2, #1	; 0x1
    8dac:	e1cd00be 	strh	r0, [sp, #14]
    8db0:	e1cd21bc 	strh	r2, [sp, #28]
    8db4:	e58d7014 	str	r7, [sp, #20]
    8db8:	e58d3018 	str	r3, [sp, #24]
    8dbc:	e1a05003 	mov	r5, r3
    8dc0:	e3a09001 	mov	r9, #1	; 0x1
    8dc4:	e28ba012 	add	sl, fp, #18	; 0x12
    8dc8:	ea00002b 	b	8e7c <e2p_vs_read+0x110>
    8dcc:	ebfffefd 	bl	89c8 <iic_address_num>
    8dd0:	e1dd20be 	ldrh	r2, [sp, #14]
    8dd4:	e3500000 	cmp	r0, #0	; 0x0
    8dd8:	11a03422 	lsrne	r3, r2, #8
    8ddc:	15cd301e 	strbne	r3, [sp, #30]
    8de0:	15cd201f 	strbne	r2, [sp, #31]
    8de4:	13a08002 	movne	r8, #2	; 0x2
    8de8:	05cd201e 	strbeq	r2, [sp, #30]
    8dec:	03a08001 	moveq	r8, #1	; 0x1
    8df0:	ebfffe87 	bl	8814 <get_ticks>
    8df4:	e1dd31bc 	ldrh	r3, [sp, #28]
    8df8:	e1a04000 	mov	r4, r0
    8dfc:	e3530000 	cmp	r3, #0	; 0x0
    8e00:	0a000010 	beq	8e48 <e2p_vs_read+0xdc>
    8e04:	e3a0c000 	mov	ip, #0	; 0x0
    8e08:	e5dd200c 	ldrb	r2, [sp, #12]
    8e0c:	e1a00005 	mov	r0, r5
    8e10:	e1a01009 	mov	r1, r9
    8e14:	e1a03005 	mov	r3, r5
    8e18:	e1cdc1bc 	strh	ip, [sp, #28]
    8e1c:	e58d9000 	str	r9, [sp]
    8e20:	e58d5004 	str	r5, [sp, #4]
    8e24:	eb0000e1 	bl	91b0 <iic_init>
    8e28:	e3500000 	cmp	r0, #0	; 0x0
    8e2c:	1a000005 	bne	8e48 <e2p_vs_read+0xdc>
    8e30:	e1a07005 	mov	r7, r5
    8e34:	ea000014 	b	8e8c <e2p_vs_read+0x120>
    8e38:	ebfffe75 	bl	8814 <get_ticks>
    8e3c:	e1540000 	cmp	r4, r0
    8e40:	2a000002 	bcs	8e50 <e2p_vs_read+0xe4>
    8e44:	ea00000a 	b	8e74 <e2p_vs_read+0x108>
    8e48:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    8e4c:	e2844e12 	add	r4, r4, #288	; 0x120
    8e50:	e59dc018 	ldr	ip, [sp, #24]
    8e54:	e1a0100a 	mov	r1, sl
    8e58:	e1a02008 	mov	r2, r8
    8e5c:	e1a03006 	mov	r3, r6
    8e60:	e3a00000 	mov	r0, #0	; 0x0
    8e64:	e58dc000 	str	ip, [sp]
    8e68:	eb000061 	bl	8ff4 <iic_read_func>
    8e6c:	e3500000 	cmp	r0, #0	; 0x0
    8e70:	0afffff0 	beq	8e38 <e2p_vs_read+0xcc>
    8e74:	e59d3018 	ldr	r3, [sp, #24]
    8e78:	e0866003 	add	r6, r6, r3
    8e7c:	e1a0000b 	mov	r0, fp
    8e80:	ebfffedb 	bl	89f4 <iic_read_get_address>
    8e84:	e3500000 	cmp	r0, #0	; 0x0
    8e88:	1affffcf 	bne	8dcc <e2p_vs_read+0x60>
    8e8c:	e1a00007 	mov	r0, r7
    8e90:	e28dd024 	add	sp, sp, #36	; 0x24
    8e94:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

00008e98 <iicreg_read>:
    8e98:	e3500001 	cmp	r0, #1	; 0x1
    8e9c:	e92d4070 	push	{r4, r5, r6, lr}
    8ea0:	83a00000 	movhi	r0, #0	; 0x0
    8ea4:	e1a05001 	mov	r5, r1
    8ea8:	e1a04002 	mov	r4, r2
    8eac:	88bd8070 	pophi	{r4, r5, r6, pc}
    8eb0:	e59f3058 	ldr	r3, [pc, #88]	; 8f10 <iicreg_read+0x78>
    8eb4:	e5932070 	ldr	r2, [r3, #112]
    8eb8:	e2022008 	and	r2, r2, #8	; 0x8
    8ebc:	ea000000 	b	8ec4 <iicreg_read+0x2c>
    8ec0:	e5931010 	ldr	r1, [r3, #16]
    8ec4:	e3520000 	cmp	r2, #0	; 0x0
    8ec8:	1afffffc 	bne	8ec0 <iicreg_read+0x28>
    8ecc:	e59f103c 	ldr	r1, [pc, #60]	; 8f10 <iicreg_read+0x78>
    8ed0:	e1a00004 	mov	r0, r4
    8ed4:	e3a06c01 	mov	r6, #256	; 0x100
    8ed8:	e591c070 	ldr	ip, [r1, #112]
    8edc:	e2503000 	subs	r3, r0, #0	; 0x0
    8ee0:	13a03001 	movne	r3, #1	; 0x1
    8ee4:	e01330ac 	ands	r3, r3, ip, lsr #1
    8ee8:	15816010 	strne	r6, [r1, #16]
    8eec:	12400001 	subne	r0, r0, #1	; 0x1
    8ef0:	e31c0008 	tst	ip, #8	; 0x8
    8ef4:	15913010 	ldrne	r3, [r1, #16]
    8ef8:	12822001 	addne	r2, r2, #1	; 0x1
    8efc:	14c53001 	strbne	r3, [r5], #1
    8f00:	e1520004 	cmp	r2, r4
    8f04:	1afffff3 	bne	8ed8 <iicreg_read+0x40>
    8f08:	e3a00001 	mov	r0, #1	; 0x1
    8f0c:	e8bd8070 	pop	{r4, r5, r6, pc}
    8f10:	20300000 	.word	0x20300000

00008f14 <iicreg_write>:
    8f14:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    8f18:	e59d4018 	ldr	r4, [sp, #24]
    8f1c:	e3500001 	cmp	r0, #1	; 0x1
    8f20:	e1a05001 	mov	r5, r1
    8f24:	e5dd801c 	ldrb	r8, [sp, #28]
    8f28:	e1a01003 	mov	r1, r3
    8f2c:	e0847002 	add	r7, r4, r2
    8f30:	8a00002c 	bhi	8fe8 <iicreg_write+0xd4>
    8f34:	e59f00b4 	ldr	r0, [pc, #180]	; 8ff0 <iicreg_write+0xdc>
    8f38:	e5903034 	ldr	r3, [r0, #52]
    8f3c:	e213c040 	ands	ip, r3, #64	; 0x40
    8f40:	15903080 	ldrne	r3, [r0, #128]
    8f44:	15903054 	ldrne	r3, [r0, #84]
    8f48:	13a00000 	movne	r0, #0	; 0x0
    8f4c:	18bd81f0 	popne	{r4, r5, r6, r7, r8, pc}
    8f50:	e5903070 	ldr	r3, [r0, #112]
    8f54:	e3130004 	tst	r3, #4	; 0x4
    8f58:	11a06000 	movne	r6, r0
    8f5c:	11a0000c 	movne	r0, ip
    8f60:	11a0c006 	movne	ip, r6
    8f64:	1a00000d 	bne	8fa0 <iicreg_write+0x8c>
    8f68:	ea00001e 	b	8fe8 <iicreg_write+0xd4>
    8f6c:	e5963070 	ldr	r3, [r6, #112]
    8f70:	e3130002 	tst	r3, #2	; 0x2
    8f74:	0afffffc 	beq	8f6c <iicreg_write+0x58>
    8f78:	e3520000 	cmp	r2, #0	; 0x0
    8f7c:	14d53001 	ldrbne	r3, [r5], #1
    8f80:	12422001 	subne	r2, r2, #1	; 0x1
    8f84:	1a000003 	bne	8f98 <iicreg_write+0x84>
    8f88:	e3540000 	cmp	r4, #0	; 0x0
    8f8c:	0a000002 	beq	8f9c <iicreg_write+0x88>
    8f90:	e4d13001 	ldrb	r3, [r1], #1
    8f94:	e2444001 	sub	r4, r4, #1	; 0x1
    8f98:	e58c3010 	str	r3, [ip, #16]
    8f9c:	e2800001 	add	r0, r0, #1	; 0x1
    8fa0:	e1500007 	cmp	r0, r7
    8fa4:	3afffff0 	bcc	8f6c <iicreg_write+0x58>
    8fa8:	e59f2040 	ldr	r2, [pc, #64]	; 8ff0 <iicreg_write+0xdc>
    8fac:	e5923070 	ldr	r3, [r2, #112]
    8fb0:	e3130004 	tst	r3, #4	; 0x4
    8fb4:	0afffffc 	beq	8fac <iicreg_write+0x98>
    8fb8:	e3580000 	cmp	r8, #0	; 0x0
    8fbc:	159f202c 	ldrne	r2, [pc, #44]	; 8ff0 <iicreg_write+0xdc>
    8fc0:	0a000006 	beq	8fe0 <iicreg_write+0xcc>
    8fc4:	e5923034 	ldr	r3, [r2, #52]
    8fc8:	e3130c02 	tst	r3, #512	; 0x200
    8fcc:	0afffffc 	beq	8fc4 <iicreg_write+0xb0>
    8fd0:	e59f3018 	ldr	r3, [pc, #24]	; 8ff0 <iicreg_write+0xdc>
    8fd4:	e3010964 	movw	r0, #6500	; 0x1964
    8fd8:	e5933060 	ldr	r3, [r3, #96]
    8fdc:	ebfffe1d 	bl	8858 <udelay>
    8fe0:	e3a00001 	mov	r0, #1	; 0x1
    8fe4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    8fe8:	e3a00000 	mov	r0, #0	; 0x0
    8fec:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    8ff0:	20300000 	.word	0x20300000

00008ff4 <iic_read_func>:
    8ff4:	e92d4070 	push	{r4, r5, r6, lr}
    8ff8:	e59f4080 	ldr	r4, [pc, #128]	; 9080 <iic_read_func+0x8c>
    8ffc:	e1a06003 	mov	r6, r3
    9000:	e5d43000 	ldrb	r3, [r4]
    9004:	e24dd008 	sub	sp, sp, #8	; 0x8
    9008:	e243300a 	sub	r3, r3, #10	; 0xa
    900c:	e6ef3073 	uxtb	r3, r3
    9010:	e3530001 	cmp	r3, #1	; 0x1
    9014:	e1a05000 	mov	r5, r0
    9018:	9a00000b 	bls	904c <iic_read_func+0x58>
    901c:	e3a0e000 	mov	lr, #0	; 0x0
    9020:	e1a0300e 	mov	r3, lr
    9024:	e3a0c001 	mov	ip, #1	; 0x1
    9028:	e58dc004 	str	ip, [sp, #4]
    902c:	e58de000 	str	lr, [sp]
    9030:	ebffffb7 	bl	8f14 <iicreg_write>
    9034:	e3500000 	cmp	r0, #0	; 0x0
    9038:	03a0300c 	moveq	r3, #12	; 0xc
    903c:	05c43000 	strbeq	r3, [r4]
    9040:	0a00000c 	beq	9078 <iic_read_func+0x84>
    9044:	e3a0300a 	mov	r3, #10	; 0xa
    9048:	e5c43000 	strb	r3, [r4]
    904c:	e59d2018 	ldr	r2, [sp, #24]
    9050:	e1a00005 	mov	r0, r5
    9054:	e1a01006 	mov	r1, r6
    9058:	ebffff8e 	bl	8e98 <iicreg_read>
    905c:	e59f201c 	ldr	r2, [pc, #28]	; 9080 <iic_read_func+0x8c>
    9060:	e3500000 	cmp	r0, #0	; 0x0
    9064:	13a0300d 	movne	r3, #13	; 0xd
    9068:	03a0300b 	moveq	r3, #11	; 0xb
    906c:	15c23000 	strbne	r3, [r2]
    9070:	13a00001 	movne	r0, #1	; 0x1
    9074:	05c23000 	strbeq	r3, [r2]
    9078:	e28dd008 	add	sp, sp, #8	; 0x8
    907c:	e8bd8070 	pop	{r4, r5, r6, pc}
    9080:	0800d254 	.word	0x0800d254

00009084 <iic_writeread>:
    9084:	e92d4070 	push	{r4, r5, r6, lr}
    9088:	e59f4088 	ldr	r4, [pc, #136]	; 9118 <iic_writeread+0x94>
    908c:	e24dd010 	sub	sp, sp, #16	; 0x10
    9090:	e5d43000 	ldrb	r3, [r4]
    9094:	e5cd100f 	strb	r1, [sp, #15]
    9098:	e243300a 	sub	r3, r3, #10	; 0xa
    909c:	e6ef3073 	uxtb	r3, r3
    90a0:	e3530001 	cmp	r3, #1	; 0x1
    90a4:	e1a06002 	mov	r6, r2
    90a8:	e1a05000 	mov	r5, r0
    90ac:	9a00000c 	bls	90e4 <iic_writeread+0x60>
    90b0:	e3a0c000 	mov	ip, #0	; 0x0
    90b4:	e1a0300c 	mov	r3, ip
    90b8:	e28d100f 	add	r1, sp, #15	; 0xf
    90bc:	e3a02001 	mov	r2, #1	; 0x1
    90c0:	e58dc000 	str	ip, [sp]
    90c4:	e58dc004 	str	ip, [sp, #4]
    90c8:	ebffff91 	bl	8f14 <iicreg_write>
    90cc:	e3500000 	cmp	r0, #0	; 0x0
    90d0:	03a0300c 	moveq	r3, #12	; 0xc
    90d4:	05c43000 	strbeq	r3, [r4]
    90d8:	0a00000c 	beq	9110 <iic_writeread+0x8c>
    90dc:	e3a0300a 	mov	r3, #10	; 0xa
    90e0:	e5c43000 	strb	r3, [r4]
    90e4:	e3a02001 	mov	r2, #1	; 0x1
    90e8:	e1a00005 	mov	r0, r5
    90ec:	e1a01006 	mov	r1, r6
    90f0:	ebffff68 	bl	8e98 <iicreg_read>
    90f4:	e59f201c 	ldr	r2, [pc, #28]	; 9118 <iic_writeread+0x94>
    90f8:	e3500000 	cmp	r0, #0	; 0x0
    90fc:	13a0300d 	movne	r3, #13	; 0xd
    9100:	03a0300b 	moveq	r3, #11	; 0xb
    9104:	15c23000 	strbne	r3, [r2]
    9108:	13a00001 	movne	r0, #1	; 0x1
    910c:	05c23000 	strbeq	r3, [r2]
    9110:	e28dd010 	add	sp, sp, #16	; 0x10
    9114:	e8bd8070 	pop	{r4, r5, r6, pc}
    9118:	0800d254 	.word	0x0800d254

0000911c <iicreg_init>:
    911c:	e92d4030 	push	{r4, r5, lr}
    9120:	e1a04000 	mov	r4, r0
    9124:	e24dd004 	sub	sp, sp, #4	; 0x4
    9128:	e59f0074 	ldr	r0, [pc, #116]	; 91a4 <iicreg_init+0x88>
    912c:	e1a05002 	mov	r5, r2
    9130:	ebffdfaa 	bl	fe0 <module_get_clock>
    9134:	e3540001 	cmp	r4, #1	; 0x1
    9138:	83a00000 	movhi	r0, #0	; 0x0
    913c:	8a000016 	bhi	919c <iicreg_init+0x80>
    9140:	e59f1060 	ldr	r1, [pc, #96]	; 91a8 <iicreg_init+0x8c>
    9144:	eb001bef 	bl	10108 <__aeabi_uidiv>
    9148:	e59f205c 	ldr	r2, [pc, #92]	; 91ac <iicreg_init+0x90>
    914c:	e1a010a0 	lsr	r1, r0, #1
    9150:	e3a0c000 	mov	ip, #0	; 0x0
    9154:	e3a03043 	mov	r3, #67	; 0x43
    9158:	e582c06c 	str	ip, [r2, #108]
    915c:	e5823000 	str	r3, [r2]
    9160:	e5820014 	str	r0, [r2, #20]
    9164:	e5820018 	str	r0, [r2, #24]
    9168:	e58210a0 	str	r1, [r2, #160]
    916c:	e59230a0 	ldr	r3, [r2, #160]
    9170:	e1a000a5 	lsr	r0, r5, #1
    9174:	e3c33c01 	bic	r3, r3, #256	; 0x100
    9178:	e58230a0 	str	r3, [r2, #160]
    917c:	e5923000 	ldr	r3, [r2]
    9180:	e3a01001 	mov	r1, #1	; 0x1
    9184:	e3833020 	orr	r3, r3, #32	; 0x20
    9188:	e5823000 	str	r3, [r2]
    918c:	e582c030 	str	ip, [r2, #48]
    9190:	e5820004 	str	r0, [r2, #4]
    9194:	e582106c 	str	r1, [r2, #108]
    9198:	e1a00001 	mov	r0, r1
    919c:	e28dd004 	add	sp, sp, #4	; 0x4
    91a0:	e8bd8030 	pop	{r4, r5, pc}
    91a4:	2d00a360 	.word	0x2d00a360
    91a8:	0009c400 	.word	0x0009c400
    91ac:	20300000 	.word	0x20300000

000091b0 <iic_init>:
    91b0:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    91b4:	e59fc054 	ldr	ip, [pc, #84]	; 9210 <iic_init+0x60>
    91b8:	e24dd004 	sub	sp, sp, #4	; 0x4
    91bc:	e3a0e00d 	mov	lr, #13	; 0xd
    91c0:	e1a0a000 	mov	sl, r0
    91c4:	e59f0048 	ldr	r0, [pc, #72]	; 9214 <iic_init+0x64>
    91c8:	e1a04001 	mov	r4, r1
    91cc:	e1a05002 	mov	r5, r2
    91d0:	e1a06003 	mov	r6, r3
    91d4:	e5cce000 	strb	lr, [ip]
    91d8:	e5dd7020 	ldrb	r7, [sp, #32]
    91dc:	e5dd8024 	ldrb	r8, [sp, #36]
    91e0:	ebffdd4e 	bl	720 <module_enable>
    91e4:	e3a0000a 	mov	r0, #10	; 0xa
    91e8:	ebffdd93 	bl	83c <pads_init_module>
    91ec:	e1a0000a 	mov	r0, sl
    91f0:	e1a01004 	mov	r1, r4
    91f4:	e1a02005 	mov	r2, r5
    91f8:	e1a03006 	mov	r3, r6
    91fc:	e58d7020 	str	r7, [sp, #32]
    9200:	e58d8024 	str	r8, [sp, #36]
    9204:	e28dd004 	add	sp, sp, #4	; 0x4
    9208:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    920c:	eaffffc2 	b	911c <iicreg_init>
    9210:	0800d254 	.word	0x0800d254
    9214:	2d010c00 	.word	0x2d010c00

00009218 <spi_set_bytes>:
    9218:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    921c:	e59f3020 	ldr	r3, [pc, #32]	; 9244 <spi_set_bytes+0x2c>
    9220:	e1a01000 	mov	r1, r0
    9224:	e5830000 	str	r0, [r3]
    9228:	e24dd004 	sub	sp, sp, #4	; 0x4
    922c:	e59f0014 	ldr	r0, [pc, #20]	; 9248 <spi_set_bytes+0x30>
    9230:	e59f3014 	ldr	r3, [pc, #20]	; 924c <spi_set_bytes+0x34>
    9234:	e1a0e00f 	mov	lr, pc
    9238:	e593f000 	ldr	pc, [r3]
    923c:	e28dd004 	add	sp, sp, #4	; 0x4
    9240:	e8bd8000 	pop	{pc}
    9244:	0800d130 	.word	0x0800d130
    9248:	00013388 	.word	0x00013388
    924c:	0800d76c 	.word	0x0800d76c

00009250 <spi_fifo_clear>:
    9250:	e59f2028 	ldr	r2, [pc, #40]	; 9280 <spi_fifo_clear+0x30>
    9254:	e592300c 	ldr	r3, [r2, #12]
    9258:	e3130010 	tst	r3, #16	; 0x10
    925c:	1afffffc 	bne	9254 <spi_fifo_clear+0x4>
    9260:	ea000001 	b	926c <spi_fifo_clear+0x1c>
    9264:	e5923008 	ldr	r3, [r2, #8]
    9268:	ea000000 	b	9270 <spi_fifo_clear+0x20>
    926c:	e59f200c 	ldr	r2, [pc, #12]	; 9280 <spi_fifo_clear+0x30>
    9270:	e592300c 	ldr	r3, [r2, #12]
    9274:	e3130004 	tst	r3, #4	; 0x4
    9278:	1afffff9 	bne	9264 <spi_fifo_clear+0x14>
    927c:	e12fff1e 	bx	lr
    9280:	20500000 	.word	0x20500000

00009284 <flash_vs_align>:
    9284:	e3a00c01 	mov	r0, #256	; 0x100
    9288:	e12fff1e 	bx	lr

0000928c <spi_exchange>:
    928c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    9290:	e5905008 	ldr	r5, [r0, #8]
    9294:	e3510001 	cmp	r1, #1	; 0x1
    9298:	e24dd00c 	sub	sp, sp, #12	; 0xc
    929c:	e1a0a000 	mov	sl, r0
    92a0:	e5908004 	ldr	r8, [r0, #4]
    92a4:	d1a04005 	movle	r4, r5
    92a8:	d3a07000 	movle	r7, #0	; 0x0
    92ac:	da000020 	ble	9334 <spi_exchange+0xa8>
    92b0:	e280a00c 	add	sl, r0, #12	; 0xc
    92b4:	e1a01008 	mov	r1, r8
    92b8:	e1a0000d 	mov	r0, sp
    92bc:	e1a02005 	mov	r2, r5
    92c0:	ebffe021 	bl	134c <memcpy>
    92c4:	e99a0012 	ldmib	sl, {r1, r4}
    92c8:	e0843005 	add	r3, r4, r5
    92cc:	e3530008 	cmp	r3, #8	; 0x8
    92d0:	a3a03008 	movge	r3, #8	; 0x8
    92d4:	e6ef6073 	uxtb	r6, r3
    92d8:	e3510000 	cmp	r1, #0	; 0x0
    92dc:	e1a0700d 	mov	r7, sp
    92e0:	059a8000 	ldreq	r8, [sl]
    92e4:	00667084 	rsbeq	r7, r6, r4, lsl #1
    92e8:	0a000008 	beq	9310 <spi_exchange+0x84>
    92ec:	e0654006 	rsb	r4, r5, r6
    92f0:	e0870005 	add	r0, r7, r5
    92f4:	e1a02004 	mov	r2, r4
    92f8:	ebffe013 	bl	134c <memcpy>
    92fc:	e99a000c 	ldmib	sl, {r2, r3}
    9300:	e0855003 	add	r5, r5, r3
    9304:	e0828004 	add	r8, r2, r4
    9308:	e3a07000 	mov	r7, #0	; 0x0
    930c:	e0664005 	rsb	r4, r6, r5
    9310:	e3a02000 	mov	r2, #0	; 0x0
    9314:	e1a0000d 	mov	r0, sp
    9318:	e59f126c 	ldr	r1, [pc, #620]	; 958c <spi_exchange+0x300>
    931c:	ea000002 	b	932c <spi_exchange+0xa0>
    9320:	e7d03002 	ldrb	r3, [r0, r2]
    9324:	e2822001 	add	r2, r2, #1	; 0x1
    9328:	e5813008 	str	r3, [r1, #8]
    932c:	e1520006 	cmp	r2, r6
    9330:	bafffffa 	blt	9320 <spi_exchange+0x94>
    9334:	e59f6250 	ldr	r6, [pc, #592]	; 958c <spi_exchange+0x300>
    9338:	e3a0b005 	mov	fp, #5	; 0x5
    933c:	e5969018 	ldr	r9, [r6, #24]
    9340:	e3190004 	tst	r9, #4	; 0x4
    9344:	0a000030 	beq	940c <spi_exchange+0x180>
    9348:	e3550003 	cmp	r5, #3	; 0x3
    934c:	da000005 	ble	9368 <spi_exchange+0xdc>
    9350:	e5963008 	ldr	r3, [r6, #8]
    9354:	e2455004 	sub	r5, r5, #4	; 0x4
    9358:	e5963008 	ldr	r3, [r6, #8]
    935c:	e5963008 	ldr	r3, [r6, #8]
    9360:	e5963008 	ldr	r3, [r6, #8]
    9364:	ea000028 	b	940c <spi_exchange+0x180>
    9368:	e3550000 	cmp	r5, #0	; 0x0
    936c:	0a000003 	beq	9380 <spi_exchange+0xf4>
    9370:	e2555001 	subs	r5, r5, #1	; 0x1
    9374:	e5963008 	ldr	r3, [r6, #8]
    9378:	1afffffc 	bne	9370 <spi_exchange+0xe4>
    937c:	ea000022 	b	940c <spi_exchange+0x180>
    9380:	e59a3000 	ldr	r3, [sl]
    9384:	e3530000 	cmp	r3, #0	; 0x0
    9388:	1a000004 	bne	93a0 <spi_exchange+0x114>
    938c:	e59f01fc 	ldr	r0, [pc, #508]	; 9590 <spi_exchange+0x304>
    9390:	e59f31fc 	ldr	r3, [pc, #508]	; 9594 <spi_exchange+0x308>
    9394:	e1a0e00f 	mov	lr, pc
    9398:	e593f000 	ldr	pc, [r3]
    939c:	ea00001a 	b	940c <spi_exchange+0x180>
    93a0:	e3540003 	cmp	r4, #3	; 0x3
    93a4:	da00000e 	ble	93e4 <spi_exchange+0x158>
    93a8:	e5962008 	ldr	r2, [r6, #8]
    93ac:	e1a03008 	mov	r3, r8
    93b0:	e4c32001 	strb	r2, [r3], #1
    93b4:	e5962008 	ldr	r2, [r6, #8]
    93b8:	e2831001 	add	r1, r3, #1	; 0x1
    93bc:	e5c82001 	strb	r2, [r8, #1]
    93c0:	e5962008 	ldr	r2, [r6, #8]
    93c4:	e2544004 	subs	r4, r4, #4	; 0x4
    93c8:	e5c32001 	strb	r2, [r3, #1]
    93cc:	e5963008 	ldr	r3, [r6, #8]
    93d0:	e2812001 	add	r2, r1, #1	; 0x1
    93d4:	e5c13001 	strb	r3, [r1, #1]
    93d8:	0a00005e 	beq	9558 <spi_exchange+0x2cc>
    93dc:	e2828001 	add	r8, r2, #1	; 0x1
    93e0:	ea000009 	b	940c <spi_exchange+0x180>
    93e4:	e3540000 	cmp	r4, #0	; 0x0
    93e8:	0a00005a 	beq	9558 <spi_exchange+0x2cc>
    93ec:	e59f2198 	ldr	r2, [pc, #408]	; 958c <spi_exchange+0x300>
    93f0:	e1a01005 	mov	r1, r5
    93f4:	e5923008 	ldr	r3, [r2, #8]
    93f8:	e7c83001 	strb	r3, [r8, r1]
    93fc:	e2811001 	add	r1, r1, #1	; 0x1
    9400:	e1510004 	cmp	r1, r4
    9404:	1afffffa 	bne	93f4 <spi_exchange+0x168>
    9408:	ea000052 	b	9558 <spi_exchange+0x2cc>
    940c:	e3190008 	tst	r9, #8	; 0x8
    9410:	0a00002a 	beq	94c0 <spi_exchange+0x234>
    9414:	e59a3004 	ldr	r3, [sl, #4]
    9418:	e3530000 	cmp	r3, #0	; 0x0
    941c:	0a000019 	beq	9488 <spi_exchange+0x1fc>
    9420:	e3540003 	cmp	r4, #3	; 0x3
    9424:	da00000d 	ble	9460 <spi_exchange+0x1d4>
    9428:	e1a03008 	mov	r3, r8
    942c:	e4d32001 	ldrb	r2, [r3], #1
    9430:	e2544004 	subs	r4, r4, #4	; 0x4
    9434:	e5862008 	str	r2, [r6, #8]
    9438:	e5d82001 	ldrb	r2, [r8, #1]
    943c:	e2831001 	add	r1, r3, #1	; 0x1
    9440:	e5862008 	str	r2, [r6, #8]
    9444:	e5d33001 	ldrb	r3, [r3, #1]
    9448:	e2818002 	add	r8, r1, #2	; 0x2
    944c:	e5863008 	str	r3, [r6, #8]
    9450:	e5d13001 	ldrb	r3, [r1, #1]
    9454:	e5863008 	str	r3, [r6, #8]
    9458:	1a000018 	bne	94c0 <spi_exchange+0x234>
    945c:	ea00003d 	b	9558 <spi_exchange+0x2cc>
    9460:	e3540000 	cmp	r4, #0	; 0x0
    9464:	0a00003b 	beq	9558 <spi_exchange+0x2cc>
    9468:	e59f111c 	ldr	r1, [pc, #284]	; 958c <spi_exchange+0x300>
    946c:	e3a02000 	mov	r2, #0	; 0x0
    9470:	e7d83002 	ldrb	r3, [r8, r2]
    9474:	e2822001 	add	r2, r2, #1	; 0x1
    9478:	e1520004 	cmp	r2, r4
    947c:	e5813008 	str	r3, [r1, #8]
    9480:	1afffffa 	bne	9470 <spi_exchange+0x1e4>
    9484:	ea000033 	b	9558 <spi_exchange+0x2cc>
    9488:	e3570003 	cmp	r7, #3	; 0x3
    948c:	da000005 	ble	94a8 <spi_exchange+0x21c>
    9490:	e586b008 	str	fp, [r6, #8]
    9494:	e2477004 	sub	r7, r7, #4	; 0x4
    9498:	e586b008 	str	fp, [r6, #8]
    949c:	e586b008 	str	fp, [r6, #8]
    94a0:	e586b008 	str	fp, [r6, #8]
    94a4:	ea000005 	b	94c0 <spi_exchange+0x234>
    94a8:	e3570000 	cmp	r7, #0	; 0x0
    94ac:	0a000003 	beq	94c0 <spi_exchange+0x234>
    94b0:	e3a03005 	mov	r3, #5	; 0x5
    94b4:	e2577001 	subs	r7, r7, #1	; 0x1
    94b8:	e5863008 	str	r3, [r6, #8]
    94bc:	1afffffb 	bne	94b0 <spi_exchange+0x224>
    94c0:	e3190002 	tst	r9, #2	; 0x2
    94c4:	0affff9c 	beq	933c <spi_exchange+0xb0>
    94c8:	ea000009 	b	94f4 <spi_exchange+0x268>
    94cc:	e596300c 	ldr	r3, [r6, #12]
    94d0:	e2455001 	sub	r5, r5, #1	; 0x1
    94d4:	e3130004 	tst	r3, #4	; 0x4
    94d8:	15963008 	ldrne	r3, [r6, #8]
    94dc:	1a000006 	bne	94fc <spi_exchange+0x270>
    94e0:	e59f00b0 	ldr	r0, [pc, #176]	; 9598 <spi_exchange+0x30c>
    94e4:	e1a01005 	mov	r1, r5
    94e8:	e1a0e00f 	mov	lr, pc
    94ec:	e597f000 	ldr	pc, [r7]
    94f0:	ea000001 	b	94fc <spi_exchange+0x270>
    94f4:	e59f6090 	ldr	r6, [pc, #144]	; 958c <spi_exchange+0x300>
    94f8:	e59f7094 	ldr	r7, [pc, #148]	; 9594 <spi_exchange+0x308>
    94fc:	e3550000 	cmp	r5, #0	; 0x0
    9500:	1afffff1 	bne	94cc <spi_exchange+0x240>
    9504:	ea00000a 	b	9534 <spi_exchange+0x2a8>
    9508:	e595300c 	ldr	r3, [r5, #12]
    950c:	e2444001 	sub	r4, r4, #1	; 0x1
    9510:	e3130004 	tst	r3, #4	; 0x4
    9514:	15953008 	ldrne	r3, [r5, #8]
    9518:	14c83001 	strbne	r3, [r8], #1
    951c:	1a000006 	bne	953c <spi_exchange+0x2b0>
    9520:	e59f0074 	ldr	r0, [pc, #116]	; 959c <spi_exchange+0x310>
    9524:	e1a01004 	mov	r1, r4
    9528:	e1a0e00f 	mov	lr, pc
    952c:	e596f000 	ldr	pc, [r6]
    9530:	ea000001 	b	953c <spi_exchange+0x2b0>
    9534:	e59f5050 	ldr	r5, [pc, #80]	; 958c <spi_exchange+0x300>
    9538:	e59f6054 	ldr	r6, [pc, #84]	; 9594 <spi_exchange+0x308>
    953c:	e59a3000 	ldr	r3, [sl]
    9540:	e3530000 	cmp	r3, #0	; 0x0
    9544:	13540000 	cmpne	r4, #0	; 0x0
    9548:	1affffee 	bne	9508 <spi_exchange+0x27c>
    954c:	e59f3038 	ldr	r3, [pc, #56]	; 958c <spi_exchange+0x300>
    9550:	e3a02003 	mov	r2, #3	; 0x3
    9554:	e5832020 	str	r2, [r3, #32]
    9558:	e59f202c 	ldr	r2, [pc, #44]	; 958c <spi_exchange+0x300>
    955c:	e592300c 	ldr	r3, [r2, #12]
    9560:	e3130010 	tst	r3, #16	; 0x10
    9564:	1afffffc 	bne	955c <spi_exchange+0x2d0>
    9568:	ea000001 	b	9574 <spi_exchange+0x2e8>
    956c:	e5923008 	ldr	r3, [r2, #8]
    9570:	ea000000 	b	9578 <spi_exchange+0x2ec>
    9574:	e59f2010 	ldr	r2, [pc, #16]	; 958c <spi_exchange+0x300>
    9578:	e592300c 	ldr	r3, [r2, #12]
    957c:	e3130004 	tst	r3, #4	; 0x4
    9580:	1afffff9 	bne	956c <spi_exchange+0x2e0>
    9584:	e28dd00c 	add	sp, sp, #12	; 0xc
    9588:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    958c:	20500000 	.word	0x20500000
    9590:	0001339f 	.word	0x0001339f
    9594:	0800d76c 	.word	0x0800d76c
    9598:	000133b6 	.word	0x000133b6
    959c:	000133ce 	.word	0x000133ce

000095a0 <flash_vs_read>:
    95a0:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    95a4:	e24dd024 	sub	sp, sp, #36	; 0x24
    95a8:	e28d4018 	add	r4, sp, #24	; 0x18
    95ac:	e2841003 	add	r1, r4, #3	; 0x3
    95b0:	e59da040 	ldr	sl, [sp, #64]
    95b4:	e58d1004 	str	r1, [sp, #4]
    95b8:	e3a01005 	mov	r1, #5	; 0x5
    95bc:	e3a0c000 	mov	ip, #0	; 0x0
    95c0:	e58d1008 	str	r1, [sp, #8]
    95c4:	e2811006 	add	r1, r1, #6	; 0x6
    95c8:	e58dc018 	str	ip, [sp, #24]
    95cc:	e2845004 	add	r5, r4, #4	; 0x4
    95d0:	e58d000c 	str	r0, [sp, #12]
    95d4:	e58dc010 	str	ip, [sp, #16]
    95d8:	e5cd101b 	strb	r1, [sp, #27]
    95dc:	e1a06002 	mov	r6, r2
    95e0:	e1a07003 	mov	r7, r3
    95e4:	e58dc000 	str	ip, [sp]
    95e8:	e1a0400a 	mov	r4, sl
    95ec:	e1a0800d 	mov	r8, sp
    95f0:	ea00000c 	b	9628 <flash_vs_read+0x88>
    95f4:	e3540a01 	cmp	r4, #4096	; 0x1000
    95f8:	b1a03004 	movlt	r3, r4
    95fc:	a3a03a01 	movge	r3, #4096	; 0x1000
    9600:	e5852000 	str	r2, [r5]
    9604:	e58d3014 	str	r3, [sp, #20]
    9608:	ebffff1f 	bl	928c <spi_exchange>
    960c:	e59d2014 	ldr	r2, [sp, #20]
    9610:	e59d300c 	ldr	r3, [sp, #12]
    9614:	e0966002 	adds	r6, r6, r2
    9618:	e0833002 	add	r3, r3, r2
    961c:	e0a77fc2 	adc	r7, r7, r2, asr #31
    9620:	e58d300c 	str	r3, [sp, #12]
    9624:	e0624004 	rsb	r4, r2, r4
    9628:	e1a02c06 	lsl	r2, r6, #24
    962c:	e2061cff 	and	r1, r6, #65280	; 0xff00
    9630:	e1822c26 	orr	r2, r2, r6, lsr #24
    9634:	e1822401 	orr	r2, r2, r1, lsl #8
    9638:	e20638ff 	and	r3, r6, #16711680	; 0xff0000
    963c:	e1822423 	orr	r2, r2, r3, lsr #8
    9640:	e3540000 	cmp	r4, #0	; 0x0
    9644:	e1a02442 	asr	r2, r2, #8
    9648:	e1a0000d 	mov	r0, sp
    964c:	e3a01002 	mov	r1, #2	; 0x2
    9650:	1affffe7 	bne	95f4 <flash_vs_read+0x54>
    9654:	e1a0000a 	mov	r0, sl
    9658:	e28dd024 	add	sp, sp, #36	; 0x24
    965c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00009660 <flash_reset>:
    9660:	e92d4030 	push	{r4, r5, lr}
    9664:	e24dd024 	sub	sp, sp, #36	; 0x24
    9668:	e3a04001 	mov	r4, #1	; 0x1
    966c:	e28dc018 	add	ip, sp, #24	; 0x18
    9670:	e08c3004 	add	r3, ip, r4
    9674:	e3a02000 	mov	r2, #0	; 0x0
    9678:	e58d3010 	str	r3, [sp, #16]
    967c:	e3a03066 	mov	r3, #102	; 0x66
    9680:	e58d2018 	str	r2, [sp, #24]
    9684:	e1a0000d 	mov	r0, sp
    9688:	e1a01004 	mov	r1, r4
    968c:	e5cd3018 	strb	r3, [sp, #24]
    9690:	e3e03066 	mvn	r3, #102	; 0x66
    9694:	e58d200c 	str	r2, [sp, #12]
    9698:	e5cd3019 	strb	r3, [sp, #25]
    969c:	e58d201c 	str	r2, [sp, #28]
    96a0:	e88d1004 	stm	sp, {r2, ip}
    96a4:	e58d4008 	str	r4, [sp, #8]
    96a8:	e58d4014 	str	r4, [sp, #20]
    96ac:	ebfffef6 	bl	928c <spi_exchange>
    96b0:	e28d000c 	add	r0, sp, #12	; 0xc
    96b4:	e1a01004 	mov	r1, r4
    96b8:	ebfffef3 	bl	928c <spi_exchange>
    96bc:	e3a0003c 	mov	r0, #60	; 0x3c
    96c0:	e1a0500d 	mov	r5, sp
    96c4:	ebfffc63 	bl	8858 <udelay>
    96c8:	e28dd024 	add	sp, sp, #36	; 0x24
    96cc:	e8bd8030 	pop	{r4, r5, pc}

000096d0 <flash_poll>:
    96d0:	e92d4030 	push	{r4, r5, lr}
    96d4:	e24dd024 	sub	sp, sp, #36	; 0x24
    96d8:	e28dc018 	add	ip, sp, #24	; 0x18
    96dc:	e28c3001 	add	r3, ip, #1	; 0x1
    96e0:	e3a02000 	mov	r2, #0	; 0x0
    96e4:	e3a01001 	mov	r1, #1	; 0x1
    96e8:	e58d300c 	str	r3, [sp, #12]
    96ec:	e3a03005 	mov	r3, #5	; 0x5
    96f0:	e58d2018 	str	r2, [sp, #24]
    96f4:	e58d2010 	str	r2, [sp, #16]
    96f8:	e58d1014 	str	r1, [sp, #20]
    96fc:	e5cd3018 	strb	r3, [sp, #24]
    9700:	e1a04000 	mov	r4, r0
    9704:	e58d201c 	str	r2, [sp, #28]
    9708:	e88d1004 	stm	sp, {r2, ip}
    970c:	e58d1008 	str	r1, [sp, #8]
    9710:	e1a0500d 	mov	r5, sp
    9714:	e1a0000d 	mov	r0, sp
    9718:	e3a01002 	mov	r1, #2	; 0x2
    971c:	ebfffeda 	bl	928c <spi_exchange>
    9720:	e5dd3019 	ldrb	r3, [sp, #25]
    9724:	e3540000 	cmp	r4, #0	; 0x0
    9728:	11a030a3 	lsrne	r3, r3, #1
    972c:	12233001 	eorne	r3, r3, #1	; 0x1
    9730:	e2033001 	and	r3, r3, #1	; 0x1
    9734:	e3530000 	cmp	r3, #0	; 0x0
    9738:	1afffff5 	bne	9714 <flash_poll+0x44>
    973c:	e28dd024 	add	sp, sp, #36	; 0x24
    9740:	e8bd8030 	pop	{r4, r5, pc}

00009744 <flash_wren>:
    9744:	e92d4010 	push	{r4, lr}
    9748:	e24dd020 	sub	sp, sp, #32	; 0x20
    974c:	e3a04001 	mov	r4, #1	; 0x1
    9750:	e28dc018 	add	ip, sp, #24	; 0x18
    9754:	e08c3004 	add	r3, ip, r4
    9758:	e3a02000 	mov	r2, #0	; 0x0
    975c:	e58d3010 	str	r3, [sp, #16]
    9760:	e3a03006 	mov	r3, #6	; 0x6
    9764:	e58d2018 	str	r2, [sp, #24]
    9768:	e1a0000d 	mov	r0, sp
    976c:	e5cd3018 	strb	r3, [sp, #24]
    9770:	e1a01004 	mov	r1, r4
    9774:	e3a03004 	mov	r3, #4	; 0x4
    9778:	e58d200c 	str	r2, [sp, #12]
    977c:	e5cd3019 	strb	r3, [sp, #25]
    9780:	e58d201c 	str	r2, [sp, #28]
    9784:	e88d1004 	stm	sp, {r2, ip}
    9788:	e58d4008 	str	r4, [sp, #8]
    978c:	e58d4014 	str	r4, [sp, #20]
    9790:	ebfffebd 	bl	928c <spi_exchange>
    9794:	e1a00004 	mov	r0, r4
    9798:	ebffffcc 	bl	96d0 <flash_poll>
    979c:	e28dd020 	add	sp, sp, #32	; 0x20
    97a0:	e8bd8010 	pop	{r4, pc}

000097a4 <flash_vs_erase>:
    97a4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    97a8:	e24dd034 	sub	sp, sp, #52	; 0x34
    97ac:	e28d4028 	add	r4, sp, #40	; 0x28
    97b0:	e3a06001 	mov	r6, #1	; 0x1
    97b4:	e59f109c 	ldr	r1, [pc, #156]	; 9858 <flash_vs_erase+0xb4>
    97b8:	e3a02008 	mov	r2, #8	; 0x8
    97bc:	e1a00004 	mov	r0, r4
    97c0:	ebffdee1 	bl	134c <memcpy>
    97c4:	e0843006 	add	r3, r4, r6
    97c8:	e2842003 	add	r2, r4, #3	; 0x3
    97cc:	e3a05000 	mov	r5, #0	; 0x0
    97d0:	e28d7004 	add	r7, sp, #4	; 0x4
    97d4:	e58d3008 	str	r3, [sp, #8]
    97d8:	e3a03002 	mov	r3, #2	; 0x2
    97dc:	e58d300c 	str	r3, [sp, #12]
    97e0:	e58d2014 	str	r2, [sp, #20]
    97e4:	e58d4020 	str	r4, [sp, #32]
    97e8:	e58d5004 	str	r5, [sp, #4]
    97ec:	e58d5010 	str	r5, [sp, #16]
    97f0:	e58d6018 	str	r6, [sp, #24]
    97f4:	e58d501c 	str	r5, [sp, #28]
    97f8:	e58d6024 	str	r6, [sp, #36]
    97fc:	ebffffd0 	bl	9744 <flash_wren>
    9800:	e1a01006 	mov	r1, r6
    9804:	e1a00007 	mov	r0, r7
    9808:	ebfffe9f 	bl	928c <spi_exchange>
    980c:	e1a00005 	mov	r0, r5
    9810:	ebffffae 	bl	96d0 <flash_poll>
    9814:	e2870018 	add	r0, r7, #24	; 0x18
    9818:	e1a01006 	mov	r1, r6
    981c:	ebfffe9a 	bl	928c <spi_exchange>
    9820:	e1a01006 	mov	r1, r6
    9824:	e1a00007 	mov	r0, r7
    9828:	ebfffe97 	bl	928c <spi_exchange>
    982c:	e1a00005 	mov	r0, r5
    9830:	ebffffa6 	bl	96d0 <flash_poll>
    9834:	ebffffc2 	bl	9744 <flash_wren>
    9838:	e1a01006 	mov	r1, r6
    983c:	e287000c 	add	r0, r7, #12	; 0xc
    9840:	ebfffe91 	bl	928c <spi_exchange>
    9844:	e1a00005 	mov	r0, r5
    9848:	ebffffa0 	bl	96d0 <flash_poll>
    984c:	e1a00005 	mov	r0, r5
    9850:	e28dd034 	add	sp, sp, #52	; 0x34
    9854:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    9858:	00011826 	.word	0x00011826

0000985c <flash_vs_write>:
    985c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    9860:	e3a01004 	mov	r1, #4	; 0x4
    9864:	e24dd020 	sub	sp, sp, #32	; 0x20
    9868:	e3a0c000 	mov	ip, #0	; 0x0
    986c:	e59d9040 	ldr	r9, [sp, #64]
    9870:	e58d1008 	str	r1, [sp, #8]
    9874:	e2811002 	add	r1, r1, #2	; 0x2
    9878:	e58dc018 	str	ip, [sp, #24]
    987c:	e28d701c 	add	r7, sp, #28	; 0x1c
    9880:	e5cd1018 	strb	r1, [sp, #24]
    9884:	e3a01002 	mov	r1, #2	; 0x2
    9888:	e58dc00c 	str	ip, [sp, #12]
    988c:	e58d0010 	str	r0, [sp, #16]
    9890:	e5cd101b 	strb	r1, [sp, #27]
    9894:	e1a04002 	mov	r4, r2
    9898:	e1a05003 	mov	r5, r3
    989c:	e58dc000 	str	ip, [sp]
    98a0:	e58d7004 	str	r7, [sp, #4]
    98a4:	e1a06009 	mov	r6, r9
    98a8:	e59fa080 	ldr	sl, [pc, #128]	; 9930 <flash_vs_write+0xd4>
    98ac:	e1a0800d 	mov	r8, sp
    98b0:	ea000011 	b	98fc <flash_vs_write+0xa0>
    98b4:	e59a3000 	ldr	r3, [sl]
    98b8:	e5872000 	str	r2, [r7]
    98bc:	e1560003 	cmp	r6, r3
    98c0:	d58d6014 	strle	r6, [sp, #20]
    98c4:	c58d3014 	strgt	r3, [sp, #20]
    98c8:	ebffff9d 	bl	9744 <flash_wren>
    98cc:	e1a0000d 	mov	r0, sp
    98d0:	e3a01002 	mov	r1, #2	; 0x2
    98d4:	ebfffe6c 	bl	928c <spi_exchange>
    98d8:	e3a00000 	mov	r0, #0	; 0x0
    98dc:	ebffff7b 	bl	96d0 <flash_poll>
    98e0:	e59d2014 	ldr	r2, [sp, #20]
    98e4:	e59d3010 	ldr	r3, [sp, #16]
    98e8:	e0944002 	adds	r4, r4, r2
    98ec:	e0833002 	add	r3, r3, r2
    98f0:	e0a55fc2 	adc	r5, r5, r2, asr #31
    98f4:	e58d3010 	str	r3, [sp, #16]
    98f8:	e0626006 	rsb	r6, r2, r6
    98fc:	e1a02c04 	lsl	r2, r4, #24
    9900:	e1822c24 	orr	r2, r2, r4, lsr #24
    9904:	e2041cff 	and	r1, r4, #65280	; 0xff00
    9908:	e1822401 	orr	r2, r2, r1, lsl #8
    990c:	e20438ff 	and	r3, r4, #16711680	; 0xff0000
    9910:	e1822423 	orr	r2, r2, r3, lsr #8
    9914:	e3c220ff 	bic	r2, r2, #255	; 0xff
    9918:	e3560000 	cmp	r6, #0	; 0x0
    991c:	e3822002 	orr	r2, r2, #2	; 0x2
    9920:	1affffe3 	bne	98b4 <flash_vs_write+0x58>
    9924:	e1a00009 	mov	r0, r9
    9928:	e28dd020 	add	sp, sp, #32	; 0x20
    992c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    9930:	0800d130 	.word	0x0800d130

00009934 <spi_init_pads>:
    9934:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9938:	e3a0000b 	mov	r0, #11	; 0xb
    993c:	e24dd004 	sub	sp, sp, #4	; 0x4
    9940:	ebffdbbd 	bl	83c <pads_init_module>
    9944:	e3a00000 	mov	r0, #0	; 0x0
    9948:	e28dd004 	add	sp, sp, #4	; 0x4
    994c:	e8bd8000 	pop	{pc}

00009950 <spi_init_clk>:
    9950:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    9954:	e59f00f8 	ldr	r0, [pc, #248]	; 9a54 <spi_init_clk+0x104>
    9958:	e24dd00c 	sub	sp, sp, #12	; 0xc
    995c:	ebffdd9f 	bl	fe0 <module_get_clock>
    9960:	e59f20f0 	ldr	r2, [pc, #240]	; 9a58 <spi_init_clk+0x108>
    9964:	e1a08000 	mov	r8, r0
    9968:	e5923404 	ldr	r3, [r2, #1028]
    996c:	e3a05002 	mov	r5, #2	; 0x2
    9970:	e3833008 	orr	r3, r3, #8	; 0x8
    9974:	e59fa0e0 	ldr	sl, [pc, #224]	; 9a5c <spi_init_clk+0x10c>
    9978:	e5823404 	str	r3, [r2, #1028]
    997c:	ea00000c 	b	99b4 <spi_init_clk+0x64>
    9980:	e1a06004 	mov	r6, r4
    9984:	e1a01007 	mov	r1, r7
    9988:	e1a00008 	mov	r0, r8
    998c:	eb001a5f 	bl	10310 <__aeabi_idiv>
    9990:	e150000a 	cmp	r0, sl
    9994:	e0877005 	add	r7, r7, r5
    9998:	e2864001 	add	r4, r6, #1	; 0x1
    999c:	da000007 	ble	99c0 <spi_init_clk+0x70>
    99a0:	e3540c01 	cmp	r4, #256	; 0x100
    99a4:	1afffff5 	bne	9980 <spi_init_clk+0x30>
    99a8:	e2855002 	add	r5, r5, #2	; 0x2
    99ac:	e3550c01 	cmp	r5, #256	; 0x100
    99b0:	0a000003 	beq	99c4 <spi_init_clk+0x74>
    99b4:	e1a07005 	mov	r7, r5
    99b8:	e3a06000 	mov	r6, #0	; 0x0
    99bc:	eafffff0 	b	9984 <spi_init_clk+0x34>
    99c0:	e1a04006 	mov	r4, r6
    99c4:	e35400ff 	cmp	r4, #255	; 0xff
    99c8:	d35500fe 	cmple	r5, #254	; 0xfe
    99cc:	da000007 	ble	99f0 <spi_init_clk+0xa0>
    99d0:	e1a01005 	mov	r1, r5
    99d4:	e1a02004 	mov	r2, r4
    99d8:	e59f0080 	ldr	r0, [pc, #128]	; 9a60 <spi_init_clk+0x110>
    99dc:	e59f3080 	ldr	r3, [pc, #128]	; 9a64 <spi_init_clk+0x114>
    99e0:	e1a0e00f 	mov	lr, pc
    99e4:	e593f000 	ldr	pc, [r3]
    99e8:	e3a05002 	mov	r5, #2	; 0x2
    99ec:	e3a04027 	mov	r4, #39	; 0x27
    99f0:	e2841001 	add	r1, r4, #1	; 0x1
    99f4:	e0010195 	mul	r1, r5, r1
    99f8:	e1a00008 	mov	r0, r8
    99fc:	eb001a43 	bl	10310 <__aeabi_idiv>
    9a00:	e1a01008 	mov	r1, r8
    9a04:	e58d0000 	str	r0, [sp]
    9a08:	e1a02005 	mov	r2, r5
    9a0c:	e1a03004 	mov	r3, r4
    9a10:	e59f0050 	ldr	r0, [pc, #80]	; 9a68 <spi_init_clk+0x118>
    9a14:	e59fc048 	ldr	ip, [pc, #72]	; 9a64 <spi_init_clk+0x114>
    9a18:	e1a0e00f 	mov	lr, pc
    9a1c:	e59cf000 	ldr	pc, [ip]
    9a20:	e59f2044 	ldr	r2, [pc, #68]	; 9a6c <spi_init_clk+0x11c>
    9a24:	e3a03000 	mov	r3, #0	; 0x0
    9a28:	e1a01404 	lsl	r1, r4, #8
    9a2c:	e5823004 	str	r3, [r2, #4]
    9a30:	e38110c7 	orr	r1, r1, #199	; 0xc7
    9a34:	e5823014 	str	r3, [r2, #20]
    9a38:	e1a00003 	mov	r0, r3
    9a3c:	e283300a 	add	r3, r3, #10	; 0xa
    9a40:	e5825010 	str	r5, [r2, #16]
    9a44:	e5821000 	str	r1, [r2]
    9a48:	e5823004 	str	r3, [r2, #4]
    9a4c:	e28dd00c 	add	sp, sp, #12	; 0xc
    9a50:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    9a54:	2d00a2a0 	.word	0x2d00a2a0
    9a58:	2d011000 	.word	0x2d011000
    9a5c:	00061a80 	.word	0x00061a80
    9a60:	000133e6 	.word	0x000133e6
    9a64:	0800d76c 	.word	0x0800d76c
    9a68:	00013414 	.word	0x00013414
    9a6c:	20500000 	.word	0x20500000

00009a70 <spi_sequence_brd_reboot>:
    9a70:	e92d4010 	push	{r4, lr}
    9a74:	ebffdb49 	bl	7a0 <get_reset_stats>
    9a78:	e3a01002 	mov	r1, #2	; 0x2
    9a7c:	e1a04000 	mov	r4, r0
    9a80:	e3a02001 	mov	r2, #1	; 0x1
    9a84:	e3a0004a 	mov	r0, #74	; 0x4a
    9a88:	ebffdc3d 	bl	b84 <pads_chmod>
    9a8c:	e3540000 	cmp	r4, #0	; 0x0
    9a90:	08bd8010 	popeq	{r4, pc}
    9a94:	e3a01002 	mov	r1, #2	; 0x2
    9a98:	e3a02000 	mov	r2, #0	; 0x0
    9a9c:	e3a00077 	mov	r0, #119	; 0x77
    9aa0:	ebffdc37 	bl	b84 <pads_chmod>
    9aa4:	e3020710 	movw	r0, #10000	; 0x2710
    9aa8:	ebfffb6a 	bl	8858 <udelay>
    9aac:	e3a00077 	mov	r0, #119	; 0x77
    9ab0:	e3a01001 	mov	r1, #1	; 0x1
    9ab4:	e3a02000 	mov	r2, #0	; 0x0
    9ab8:	ebffdc31 	bl	b84 <pads_chmod>
    9abc:	e3020710 	movw	r0, #10000	; 0x2710
    9ac0:	e8bd4010 	pop	{r4, lr}
    9ac4:	eafffb63 	b	8858 <udelay>

00009ac8 <flash_vs_reset>:
    9ac8:	e92d4010 	push	{r4, lr}
    9acc:	ebffffe7 	bl	9a70 <spi_sequence_brd_reboot>
    9ad0:	e59f0044 	ldr	r0, [pc, #68]	; 9b1c <flash_vs_reset+0x54>
    9ad4:	ebffdb11 	bl	720 <module_enable>
    9ad8:	ebffff9c 	bl	9950 <spi_init_clk>
    9adc:	e3a0000b 	mov	r0, #11	; 0xb
    9ae0:	ebffdb55 	bl	83c <pads_init_module>
    9ae4:	e59f2034 	ldr	r2, [pc, #52]	; 9b20 <flash_vs_reset+0x58>
    9ae8:	e592300c 	ldr	r3, [r2, #12]
    9aec:	e3130010 	tst	r3, #16	; 0x10
    9af0:	1afffffc 	bne	9ae8 <flash_vs_reset+0x20>
    9af4:	ea000001 	b	9b00 <flash_vs_reset+0x38>
    9af8:	e5923008 	ldr	r3, [r2, #8]
    9afc:	ea000000 	b	9b04 <flash_vs_reset+0x3c>
    9b00:	e59f2018 	ldr	r2, [pc, #24]	; 9b20 <flash_vs_reset+0x58>
    9b04:	e592300c 	ldr	r3, [r2, #12]
    9b08:	e2134004 	ands	r4, r3, #4	; 0x4
    9b0c:	1afffff9 	bne	9af8 <flash_vs_reset+0x30>
    9b10:	ebfffed2 	bl	9660 <flash_reset>
    9b14:	e1a00004 	mov	r0, r4
    9b18:	e8bd8010 	pop	{r4, pc}
    9b1c:	2d011400 	.word	0x2d011400
    9b20:	20500000 	.word	0x20500000

00009b24 <ssp_log_open>:
    9b24:	e59f3008 	ldr	r3, [pc, #8]	; 9b34 <ssp_log_open+0x10>
    9b28:	e3a02001 	mov	r2, #1	; 0x1
    9b2c:	e5832000 	str	r2, [r3]
    9b30:	e12fff1e 	bx	lr
    9b34:	0800d2b8 	.word	0x0800d2b8

00009b38 <ssp_log_close>:
    9b38:	e59f3008 	ldr	r3, [pc, #8]	; 9b48 <ssp_log_close+0x10>
    9b3c:	e3a02000 	mov	r2, #0	; 0x0
    9b40:	e5832000 	str	r2, [r3]
    9b44:	e12fff1e 	bx	lr
    9b48:	0800d2b8 	.word	0x0800d2b8

00009b4c <ssp_manager_set_wire>:
    9b4c:	e59f3004 	ldr	r3, [pc, #4]	; 9b58 <ssp_manager_set_wire+0xc>
    9b50:	e5830000 	str	r0, [r3]
    9b54:	e12fff1e 	bx	lr
    9b58:	0800d258 	.word	0x0800d258

00009b5c <ssp_manager_set_rwmode>:
    9b5c:	e2403064 	sub	r3, r0, #100	; 0x64
    9b60:	e59f2020 	ldr	r2, [pc, #32]	; 9b88 <ssp_manager_set_rwmode+0x2c>
    9b64:	e3530002 	cmp	r3, #2	; 0x2
    9b68:	83a03064 	movhi	r3, #100	; 0x64
    9b6c:	e5820050 	str	r0, [r2, #80]
    9b70:	85823050 	strhi	r3, [r2, #80]
    9b74:	e3500066 	cmp	r0, #102	; 0x66
    9b78:	059f3008 	ldreq	r3, [pc, #8]	; 9b88 <ssp_manager_set_rwmode+0x2c>
    9b7c:	03a02064 	moveq	r2, #100	; 0x64
    9b80:	05832050 	streq	r2, [r3, #80]
    9b84:	e12fff1e 	bx	lr
    9b88:	0800d258 	.word	0x0800d258

00009b8c <ssp_manager_init>:
    9b8c:	e59f2058 	ldr	r2, [pc, #88]	; 9bec <ssp_manager_init+0x60>
    9b90:	e30033ff 	movw	r3, #1023	; 0x3ff
    9b94:	e5823038 	str	r3, [r2, #56]
    9b98:	e2433e3f 	sub	r3, r3, #1008	; 0x3f0
    9b9c:	e5823000 	str	r3, [r2]
    9ba0:	e2833011 	add	r3, r3, #17	; 0x11
    9ba4:	e5823058 	str	r3, [r2, #88]
    9ba8:	e2833044 	add	r3, r3, #68	; 0x44
    9bac:	e5823050 	str	r3, [r2, #80]
    9bb0:	e3a03040 	mov	r3, #64	; 0x40
    9bb4:	e3a01000 	mov	r1, #0	; 0x0
    9bb8:	e3a00001 	mov	r0, #1	; 0x1
    9bbc:	e3a0cb02 	mov	ip, #2048	; 0x800
    9bc0:	e5823030 	str	r3, [r2, #48]
    9bc4:	e0833003 	add	r3, r3, r3
    9bc8:	e582000c 	str	r0, [r2, #12]
    9bcc:	e582c028 	str	ip, [r2, #40]
    9bd0:	e582302c 	str	r3, [r2, #44]
    9bd4:	e5821034 	str	r1, [r2, #52]
    9bd8:	e5821004 	str	r1, [r2, #4]
    9bdc:	e5821008 	str	r1, [r2, #8]
    9be0:	e582005c 	str	r0, [r2, #92]
    9be4:	e582c044 	str	ip, [r2, #68]
    9be8:	e12fff1e 	bx	lr
    9bec:	0800d258 	.word	0x0800d258

00009bf0 <spi_status_is_busy>:
    9bf0:	e59f300c 	ldr	r3, [pc, #12]	; 9c04 <spi_status_is_busy+0x14>
    9bf4:	e593000c 	ldr	r0, [r3, #12]
    9bf8:	e1a00220 	lsr	r0, r0, #4
    9bfc:	e2000001 	and	r0, r0, #1	; 0x1
    9c00:	e12fff1e 	bx	lr
    9c04:	20500000 	.word	0x20500000

00009c08 <spi_status_recv_fifo_not_empty>:
    9c08:	e59f300c 	ldr	r3, [pc, #12]	; 9c1c <spi_status_recv_fifo_not_empty+0x14>
    9c0c:	e593000c 	ldr	r0, [r3, #12]
    9c10:	e1a00120 	lsr	r0, r0, #2
    9c14:	e2000001 	and	r0, r0, #1	; 0x1
    9c18:	e12fff1e 	bx	lr
    9c1c:	20500000 	.word	0x20500000

00009c20 <spi_status_trans_fifo_empty>:
    9c20:	e59f3008 	ldr	r3, [pc, #8]	; 9c30 <spi_status_trans_fifo_empty+0x10>
    9c24:	e593000c 	ldr	r0, [r3, #12]
    9c28:	e2000001 	and	r0, r0, #1	; 0x1
    9c2c:	e12fff1e 	bx	lr
    9c30:	20500000 	.word	0x20500000

00009c34 <spi_set_intr_recv_overrun>:
    9c34:	e59f30b4 	ldr	r3, [pc, #180]	; 9cf0 <spi_set_intr_recv_overrun+0xbc>
    9c38:	e92d4030 	push	{r4, r5, lr}
    9c3c:	e5932000 	ldr	r2, [r3]
    9c40:	e59f30ac 	ldr	r3, [pc, #172]	; 9cf4 <spi_set_intr_recv_overrun+0xc0>
    9c44:	e3520000 	cmp	r2, #0	; 0x0
    9c48:	e5933014 	ldr	r3, [r3, #20]
    9c4c:	e24dd004 	sub	sp, sp, #4	; 0x4
    9c50:	e1a05000 	mov	r5, r0
    9c54:	e3c34001 	bic	r4, r3, #1	; 0x1
    9c58:	0a000003 	beq	9c6c <spi_set_intr_recv_overrun+0x38>
    9c5c:	e59f0094 	ldr	r0, [pc, #148]	; 9cf8 <spi_set_intr_recv_overrun+0xc4>
    9c60:	e59f3094 	ldr	r3, [pc, #148]	; 9cfc <spi_set_intr_recv_overrun+0xc8>
    9c64:	e1a0e00f 	mov	lr, pc
    9c68:	e593f000 	ldr	pc, [r3]
    9c6c:	e3550000 	cmp	r5, #0	; 0x0
    9c70:	0a000002 	beq	9c80 <spi_set_intr_recv_overrun+0x4c>
    9c74:	e3550001 	cmp	r5, #1	; 0x1
    9c78:	1a000011 	bne	9cc4 <spi_set_intr_recv_overrun+0x90>
    9c7c:	ea000005 	b	9c98 <spi_set_intr_recv_overrun+0x64>
    9c80:	e59f3068 	ldr	r3, [pc, #104]	; 9cf0 <spi_set_intr_recv_overrun+0xbc>
    9c84:	e5933000 	ldr	r3, [r3]
    9c88:	e3530000 	cmp	r3, #0	; 0x0
    9c8c:	159f006c 	ldrne	r0, [pc, #108]	; 9d00 <spi_set_intr_recv_overrun+0xcc>
    9c90:	0a000011 	beq	9cdc <spi_set_intr_recv_overrun+0xa8>
    9c94:	ea000005 	b	9cb0 <spi_set_intr_recv_overrun+0x7c>
    9c98:	e59f3050 	ldr	r3, [pc, #80]	; 9cf0 <spi_set_intr_recv_overrun+0xbc>
    9c9c:	e3844001 	orr	r4, r4, #1	; 0x1
    9ca0:	e5933000 	ldr	r3, [r3]
    9ca4:	e3530000 	cmp	r3, #0	; 0x0
    9ca8:	0a00000b 	beq	9cdc <spi_set_intr_recv_overrun+0xa8>
    9cac:	e59f0050 	ldr	r0, [pc, #80]	; 9d04 <spi_set_intr_recv_overrun+0xd0>
    9cb0:	e1a01004 	mov	r1, r4
    9cb4:	e59f3040 	ldr	r3, [pc, #64]	; 9cfc <spi_set_intr_recv_overrun+0xc8>
    9cb8:	e1a0e00f 	mov	lr, pc
    9cbc:	e593f000 	ldr	pc, [r3]
    9cc0:	ea000005 	b	9cdc <spi_set_intr_recv_overrun+0xa8>
    9cc4:	e59f003c 	ldr	r0, [pc, #60]	; 9d08 <spi_set_intr_recv_overrun+0xd4>
    9cc8:	e59f302c 	ldr	r3, [pc, #44]	; 9cfc <spi_set_intr_recv_overrun+0xc8>
    9ccc:	e1a0e00f 	mov	lr, pc
    9cd0:	e593f000 	ldr	pc, [r3]
    9cd4:	e3e00000 	mvn	r0, #0	; 0x0
    9cd8:	ea000002 	b	9ce8 <spi_set_intr_recv_overrun+0xb4>
    9cdc:	e59f3010 	ldr	r3, [pc, #16]	; 9cf4 <spi_set_intr_recv_overrun+0xc0>
    9ce0:	e3a00000 	mov	r0, #0	; 0x0
    9ce4:	e5834014 	str	r4, [r3, #20]
    9ce8:	e28dd004 	add	sp, sp, #4	; 0x4
    9cec:	e8bd8030 	pop	{r4, r5, pc}
    9cf0:	0800d2b8 	.word	0x0800d2b8
    9cf4:	20500000 	.word	0x20500000
    9cf8:	00013439 	.word	0x00013439
    9cfc:	0800d76c 	.word	0x0800d76c
    9d00:	00013478 	.word	0x00013478
    9d04:	000134b5 	.word	0x000134b5
    9d08:	000134f4 	.word	0x000134f4

00009d0c <spi_set_intr_recv_timeout>:
    9d0c:	e59f30b4 	ldr	r3, [pc, #180]	; 9dc8 <spi_set_intr_recv_timeout+0xbc>
    9d10:	e92d4030 	push	{r4, r5, lr}
    9d14:	e5932000 	ldr	r2, [r3]
    9d18:	e59f30ac 	ldr	r3, [pc, #172]	; 9dcc <spi_set_intr_recv_timeout+0xc0>
    9d1c:	e3520000 	cmp	r2, #0	; 0x0
    9d20:	e5933014 	ldr	r3, [r3, #20]
    9d24:	e24dd004 	sub	sp, sp, #4	; 0x4
    9d28:	e1a05000 	mov	r5, r0
    9d2c:	e3c34002 	bic	r4, r3, #2	; 0x2
    9d30:	0a000003 	beq	9d44 <spi_set_intr_recv_timeout+0x38>
    9d34:	e59f0094 	ldr	r0, [pc, #148]	; 9dd0 <spi_set_intr_recv_timeout+0xc4>
    9d38:	e59f3094 	ldr	r3, [pc, #148]	; 9dd4 <spi_set_intr_recv_timeout+0xc8>
    9d3c:	e1a0e00f 	mov	lr, pc
    9d40:	e593f000 	ldr	pc, [r3]
    9d44:	e3550000 	cmp	r5, #0	; 0x0
    9d48:	0a000002 	beq	9d58 <spi_set_intr_recv_timeout+0x4c>
    9d4c:	e3550002 	cmp	r5, #2	; 0x2
    9d50:	1a000011 	bne	9d9c <spi_set_intr_recv_timeout+0x90>
    9d54:	ea000005 	b	9d70 <spi_set_intr_recv_timeout+0x64>
    9d58:	e59f3068 	ldr	r3, [pc, #104]	; 9dc8 <spi_set_intr_recv_timeout+0xbc>
    9d5c:	e5933000 	ldr	r3, [r3]
    9d60:	e3530000 	cmp	r3, #0	; 0x0
    9d64:	159f006c 	ldrne	r0, [pc, #108]	; 9dd8 <spi_set_intr_recv_timeout+0xcc>
    9d68:	0a000011 	beq	9db4 <spi_set_intr_recv_timeout+0xa8>
    9d6c:	ea000005 	b	9d88 <spi_set_intr_recv_timeout+0x7c>
    9d70:	e59f3050 	ldr	r3, [pc, #80]	; 9dc8 <spi_set_intr_recv_timeout+0xbc>
    9d74:	e3844002 	orr	r4, r4, #2	; 0x2
    9d78:	e5933000 	ldr	r3, [r3]
    9d7c:	e3530000 	cmp	r3, #0	; 0x0
    9d80:	0a00000b 	beq	9db4 <spi_set_intr_recv_timeout+0xa8>
    9d84:	e59f0050 	ldr	r0, [pc, #80]	; 9ddc <spi_set_intr_recv_timeout+0xd0>
    9d88:	e1a01004 	mov	r1, r4
    9d8c:	e59f3040 	ldr	r3, [pc, #64]	; 9dd4 <spi_set_intr_recv_timeout+0xc8>
    9d90:	e1a0e00f 	mov	lr, pc
    9d94:	e593f000 	ldr	pc, [r3]
    9d98:	ea000005 	b	9db4 <spi_set_intr_recv_timeout+0xa8>
    9d9c:	e59f003c 	ldr	r0, [pc, #60]	; 9de0 <spi_set_intr_recv_timeout+0xd4>
    9da0:	e59f302c 	ldr	r3, [pc, #44]	; 9dd4 <spi_set_intr_recv_timeout+0xc8>
    9da4:	e1a0e00f 	mov	lr, pc
    9da8:	e593f000 	ldr	pc, [r3]
    9dac:	e3e00000 	mvn	r0, #0	; 0x0
    9db0:	ea000002 	b	9dc0 <spi_set_intr_recv_timeout+0xb4>
    9db4:	e59f3010 	ldr	r3, [pc, #16]	; 9dcc <spi_set_intr_recv_timeout+0xc0>
    9db8:	e3a00000 	mov	r0, #0	; 0x0
    9dbc:	e5834014 	str	r4, [r3, #20]
    9dc0:	e28dd004 	add	sp, sp, #4	; 0x4
    9dc4:	e8bd8030 	pop	{r4, r5, pc}
    9dc8:	0800d2b8 	.word	0x0800d2b8
    9dcc:	20500000 	.word	0x20500000
    9dd0:	00013515 	.word	0x00013515
    9dd4:	0800d76c 	.word	0x0800d76c
    9dd8:	00013554 	.word	0x00013554
    9ddc:	00013599 	.word	0x00013599
    9de0:	000134f4 	.word	0x000134f4

00009de4 <spi_set_intr_recv_fifo>:
    9de4:	e59f30b4 	ldr	r3, [pc, #180]	; 9ea0 <spi_set_intr_recv_fifo+0xbc>
    9de8:	e92d4030 	push	{r4, r5, lr}
    9dec:	e5932000 	ldr	r2, [r3]
    9df0:	e59f30ac 	ldr	r3, [pc, #172]	; 9ea4 <spi_set_intr_recv_fifo+0xc0>
    9df4:	e3520000 	cmp	r2, #0	; 0x0
    9df8:	e5933014 	ldr	r3, [r3, #20]
    9dfc:	e24dd004 	sub	sp, sp, #4	; 0x4
    9e00:	e1a05000 	mov	r5, r0
    9e04:	e3c34004 	bic	r4, r3, #4	; 0x4
    9e08:	0a000003 	beq	9e1c <spi_set_intr_recv_fifo+0x38>
    9e0c:	e59f0094 	ldr	r0, [pc, #148]	; 9ea8 <spi_set_intr_recv_fifo+0xc4>
    9e10:	e59f3094 	ldr	r3, [pc, #148]	; 9eac <spi_set_intr_recv_fifo+0xc8>
    9e14:	e1a0e00f 	mov	lr, pc
    9e18:	e593f000 	ldr	pc, [r3]
    9e1c:	e3550000 	cmp	r5, #0	; 0x0
    9e20:	0a000002 	beq	9e30 <spi_set_intr_recv_fifo+0x4c>
    9e24:	e3550004 	cmp	r5, #4	; 0x4
    9e28:	1a000011 	bne	9e74 <spi_set_intr_recv_fifo+0x90>
    9e2c:	ea000005 	b	9e48 <spi_set_intr_recv_fifo+0x64>
    9e30:	e59f3068 	ldr	r3, [pc, #104]	; 9ea0 <spi_set_intr_recv_fifo+0xbc>
    9e34:	e5933000 	ldr	r3, [r3]
    9e38:	e3530000 	cmp	r3, #0	; 0x0
    9e3c:	159f006c 	ldrne	r0, [pc, #108]	; 9eb0 <spi_set_intr_recv_fifo+0xcc>
    9e40:	0a000011 	beq	9e8c <spi_set_intr_recv_fifo+0xa8>
    9e44:	ea000005 	b	9e60 <spi_set_intr_recv_fifo+0x7c>
    9e48:	e59f3050 	ldr	r3, [pc, #80]	; 9ea0 <spi_set_intr_recv_fifo+0xbc>
    9e4c:	e3844004 	orr	r4, r4, #4	; 0x4
    9e50:	e5933000 	ldr	r3, [r3]
    9e54:	e3530000 	cmp	r3, #0	; 0x0
    9e58:	0a00000b 	beq	9e8c <spi_set_intr_recv_fifo+0xa8>
    9e5c:	e59f0050 	ldr	r0, [pc, #80]	; 9eb4 <spi_set_intr_recv_fifo+0xd0>
    9e60:	e1a01004 	mov	r1, r4
    9e64:	e59f3040 	ldr	r3, [pc, #64]	; 9eac <spi_set_intr_recv_fifo+0xc8>
    9e68:	e1a0e00f 	mov	lr, pc
    9e6c:	e593f000 	ldr	pc, [r3]
    9e70:	ea000005 	b	9e8c <spi_set_intr_recv_fifo+0xa8>
    9e74:	e59f003c 	ldr	r0, [pc, #60]	; 9eb8 <spi_set_intr_recv_fifo+0xd4>
    9e78:	e59f302c 	ldr	r3, [pc, #44]	; 9eac <spi_set_intr_recv_fifo+0xc8>
    9e7c:	e1a0e00f 	mov	lr, pc
    9e80:	e593f000 	ldr	pc, [r3]
    9e84:	e3e00000 	mvn	r0, #0	; 0x0
    9e88:	ea000002 	b	9e98 <spi_set_intr_recv_fifo+0xb4>
    9e8c:	e59f3010 	ldr	r3, [pc, #16]	; 9ea4 <spi_set_intr_recv_fifo+0xc0>
    9e90:	e3a00000 	mov	r0, #0	; 0x0
    9e94:	e5834014 	str	r4, [r3, #20]
    9e98:	e28dd004 	add	sp, sp, #4	; 0x4
    9e9c:	e8bd8030 	pop	{r4, r5, pc}
    9ea0:	0800d2b8 	.word	0x0800d2b8
    9ea4:	20500000 	.word	0x20500000
    9ea8:	000135e0 	.word	0x000135e0
    9eac:	0800d76c 	.word	0x0800d76c
    9eb0:	0001361c 	.word	0x0001361c
    9eb4:	0001365e 	.word	0x0001365e
    9eb8:	000134f4 	.word	0x000134f4

00009ebc <spi_set_intr_transmit_fifo>:
    9ebc:	e59f30b4 	ldr	r3, [pc, #180]	; 9f78 <spi_set_intr_transmit_fifo+0xbc>
    9ec0:	e92d4030 	push	{r4, r5, lr}
    9ec4:	e5932000 	ldr	r2, [r3]
    9ec8:	e59f30ac 	ldr	r3, [pc, #172]	; 9f7c <spi_set_intr_transmit_fifo+0xc0>
    9ecc:	e3520000 	cmp	r2, #0	; 0x0
    9ed0:	e5933014 	ldr	r3, [r3, #20]
    9ed4:	e24dd004 	sub	sp, sp, #4	; 0x4
    9ed8:	e1a05000 	mov	r5, r0
    9edc:	e3c34008 	bic	r4, r3, #8	; 0x8
    9ee0:	0a000003 	beq	9ef4 <spi_set_intr_transmit_fifo+0x38>
    9ee4:	e59f0094 	ldr	r0, [pc, #148]	; 9f80 <spi_set_intr_transmit_fifo+0xc4>
    9ee8:	e59f3094 	ldr	r3, [pc, #148]	; 9f84 <spi_set_intr_transmit_fifo+0xc8>
    9eec:	e1a0e00f 	mov	lr, pc
    9ef0:	e593f000 	ldr	pc, [r3]
    9ef4:	e3550000 	cmp	r5, #0	; 0x0
    9ef8:	0a000002 	beq	9f08 <spi_set_intr_transmit_fifo+0x4c>
    9efc:	e3550008 	cmp	r5, #8	; 0x8
    9f00:	1a000011 	bne	9f4c <spi_set_intr_transmit_fifo+0x90>
    9f04:	ea000005 	b	9f20 <spi_set_intr_transmit_fifo+0x64>
    9f08:	e59f3068 	ldr	r3, [pc, #104]	; 9f78 <spi_set_intr_transmit_fifo+0xbc>
    9f0c:	e5933000 	ldr	r3, [r3]
    9f10:	e3530000 	cmp	r3, #0	; 0x0
    9f14:	159f006c 	ldrne	r0, [pc, #108]	; 9f88 <spi_set_intr_transmit_fifo+0xcc>
    9f18:	0a000011 	beq	9f64 <spi_set_intr_transmit_fifo+0xa8>
    9f1c:	ea000005 	b	9f38 <spi_set_intr_transmit_fifo+0x7c>
    9f20:	e59f3050 	ldr	r3, [pc, #80]	; 9f78 <spi_set_intr_transmit_fifo+0xbc>
    9f24:	e3844008 	orr	r4, r4, #8	; 0x8
    9f28:	e5933000 	ldr	r3, [r3]
    9f2c:	e3530000 	cmp	r3, #0	; 0x0
    9f30:	0a00000b 	beq	9f64 <spi_set_intr_transmit_fifo+0xa8>
    9f34:	e59f0050 	ldr	r0, [pc, #80]	; 9f8c <spi_set_intr_transmit_fifo+0xd0>
    9f38:	e1a01004 	mov	r1, r4
    9f3c:	e59f3040 	ldr	r3, [pc, #64]	; 9f84 <spi_set_intr_transmit_fifo+0xc8>
    9f40:	e1a0e00f 	mov	lr, pc
    9f44:	e593f000 	ldr	pc, [r3]
    9f48:	ea000005 	b	9f64 <spi_set_intr_transmit_fifo+0xa8>
    9f4c:	e59f003c 	ldr	r0, [pc, #60]	; 9f90 <spi_set_intr_transmit_fifo+0xd4>
    9f50:	e59f302c 	ldr	r3, [pc, #44]	; 9f84 <spi_set_intr_transmit_fifo+0xc8>
    9f54:	e1a0e00f 	mov	lr, pc
    9f58:	e593f000 	ldr	pc, [r3]
    9f5c:	e3e00000 	mvn	r0, #0	; 0x0
    9f60:	ea000002 	b	9f70 <spi_set_intr_transmit_fifo+0xb4>
    9f64:	e59f3010 	ldr	r3, [pc, #16]	; 9f7c <spi_set_intr_transmit_fifo+0xc0>
    9f68:	e3a00000 	mov	r0, #0	; 0x0
    9f6c:	e5834014 	str	r4, [r3, #20]
    9f70:	e28dd004 	add	sp, sp, #4	; 0x4
    9f74:	e8bd8030 	pop	{r4, r5, pc}
    9f78:	0800d2b8 	.word	0x0800d2b8
    9f7c:	20500000 	.word	0x20500000
    9f80:	000136a2 	.word	0x000136a2
    9f84:	0800d76c 	.word	0x0800d76c
    9f88:	000136df 	.word	0x000136df
    9f8c:	00013722 	.word	0x00013722
    9f90:	000134f4 	.word	0x000134f4

00009f94 <spi_get_raw_intr_recv_timeout>:
    9f94:	e59f300c 	ldr	r3, [pc, #12]	; 9fa8 <spi_get_raw_intr_recv_timeout+0x14>
    9f98:	e5930018 	ldr	r0, [r3, #24]
    9f9c:	e1a000a0 	lsr	r0, r0, #1
    9fa0:	e2000001 	and	r0, r0, #1	; 0x1
    9fa4:	e12fff1e 	bx	lr
    9fa8:	20500000 	.word	0x20500000

00009fac <spi_get_raw_intr_recv_fifo>:
    9fac:	e59f300c 	ldr	r3, [pc, #12]	; 9fc0 <spi_get_raw_intr_recv_fifo+0x14>
    9fb0:	e5930018 	ldr	r0, [r3, #24]
    9fb4:	e1a00120 	lsr	r0, r0, #2
    9fb8:	e2000001 	and	r0, r0, #1	; 0x1
    9fbc:	e12fff1e 	bx	lr
    9fc0:	20500000 	.word	0x20500000

00009fc4 <spi_get_raw_intr_trans_fifo>:
    9fc4:	e59f300c 	ldr	r3, [pc, #12]	; 9fd8 <spi_get_raw_intr_trans_fifo+0x14>
    9fc8:	e5930018 	ldr	r0, [r3, #24]
    9fcc:	e1a001a0 	lsr	r0, r0, #3
    9fd0:	e2000001 	and	r0, r0, #1	; 0x1
    9fd4:	e12fff1e 	bx	lr
    9fd8:	20500000 	.word	0x20500000

00009fdc <spi_clear_intr_recv_overun>:
    9fdc:	e59f3008 	ldr	r3, [pc, #8]	; 9fec <spi_clear_intr_recv_overun+0x10>
    9fe0:	e3a02001 	mov	r2, #1	; 0x1
    9fe4:	e5832020 	str	r2, [r3, #32]
    9fe8:	e12fff1e 	bx	lr
    9fec:	20500000 	.word	0x20500000

00009ff0 <spi_clear_intr_recv_timeout>:
    9ff0:	e59f3008 	ldr	r3, [pc, #8]	; a000 <spi_clear_intr_recv_timeout+0x10>
    9ff4:	e3a02002 	mov	r2, #2	; 0x2
    9ff8:	e5832020 	str	r2, [r3, #32]
    9ffc:	e12fff1e 	bx	lr
    a000:	20500000 	.word	0x20500000

0000a004 <spi_clear_recv_fifo>:
    a004:	e59f2014 	ldr	r2, [pc, #20]	; a020 <spi_clear_recv_fifo+0x1c>
    a008:	ea000000 	b	a010 <spi_clear_recv_fifo+0xc>
    a00c:	e5923008 	ldr	r3, [r2, #8]
    a010:	e592300c 	ldr	r3, [r2, #12]
    a014:	e3130004 	tst	r3, #4	; 0x4
    a018:	1afffffb 	bne	a00c <spi_clear_recv_fifo+0x8>
    a01c:	e12fff1e 	bx	lr
    a020:	20500000 	.word	0x20500000

0000a024 <flash_nand_vs_align>:
    a024:	e3a00b02 	mov	r0, #2048	; 0x800
    a028:	e12fff1e 	bx	lr

0000a02c <spi_transfer_init>:
    a02c:	e92d4010 	push	{r4, lr}
    a030:	e2504000 	subs	r4, r0, #0	; 0x0
    a034:	1a000005 	bne	a050 <spi_transfer_init+0x24>
    a038:	e59f00b4 	ldr	r0, [pc, #180]	; a0f4 <spi_transfer_init+0xc8>
    a03c:	e59f30b4 	ldr	r3, [pc, #180]	; a0f8 <spi_transfer_init+0xcc>
    a040:	e1a0e00f 	mov	lr, pc
    a044:	e593f000 	ldr	pc, [r3]
    a048:	e3e00001 	mvn	r0, #1	; 0x1
    a04c:	e8bd8010 	pop	{r4, pc}
    a050:	e3a01000 	mov	r1, #0	; 0x0
    a054:	e3a0205c 	mov	r2, #92	; 0x5c
    a058:	ebffdcb3 	bl	132c <memset>
    a05c:	e3a0200f 	mov	r2, #15	; 0xf
    a060:	e5c42013 	strb	r2, [r4, #19]
    a064:	e3a02002 	mov	r2, #2	; 0x2
    a068:	e3a03000 	mov	r3, #0	; 0x0
    a06c:	e5842030 	str	r2, [r4, #48]
    a070:	e3a02001 	mov	r2, #1	; 0x1
    a074:	e5842034 	str	r2, [r4, #52]
    a078:	e5c43058 	strb	r3, [r4, #88]
    a07c:	e5c43000 	strb	r3, [r4]
    a080:	e5c43001 	strb	r3, [r4, #1]
    a084:	e5c43002 	strb	r3, [r4, #2]
    a088:	e5c43003 	strb	r3, [r4, #3]
    a08c:	e5c43004 	strb	r3, [r4, #4]
    a090:	e5843008 	str	r3, [r4, #8]
    a094:	e5c4300c 	strb	r3, [r4, #12]
    a098:	e5c4300d 	strb	r3, [r4, #13]
    a09c:	e5c4300e 	strb	r3, [r4, #14]
    a0a0:	e5c4300f 	strb	r3, [r4, #15]
    a0a4:	e5c43010 	strb	r3, [r4, #16]
    a0a8:	e5c43011 	strb	r3, [r4, #17]
    a0ac:	e5c43012 	strb	r3, [r4, #18]
    a0b0:	e5843014 	str	r3, [r4, #20]
    a0b4:	e5843018 	str	r3, [r4, #24]
    a0b8:	e584301c 	str	r3, [r4, #28]
    a0bc:	e5843020 	str	r3, [r4, #32]
    a0c0:	e5843024 	str	r3, [r4, #36]
    a0c4:	e5843028 	str	r3, [r4, #40]
    a0c8:	e584302c 	str	r3, [r4, #44]
    a0cc:	e5843038 	str	r3, [r4, #56]
    a0d0:	e584303c 	str	r3, [r4, #60]
    a0d4:	e5843040 	str	r3, [r4, #64]
    a0d8:	e5843044 	str	r3, [r4, #68]
    a0dc:	e5843048 	str	r3, [r4, #72]
    a0e0:	e584304c 	str	r3, [r4, #76]
    a0e4:	e5843050 	str	r3, [r4, #80]
    a0e8:	e5843054 	str	r3, [r4, #84]
    a0ec:	e1a00003 	mov	r0, r3
    a0f0:	e8bd8010 	pop	{r4, pc}
    a0f4:	00013767 	.word	0x00013767
    a0f8:	0800d76c 	.word	0x0800d76c

0000a0fc <spi_read_write_data>:
    a0fc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a100:	e2505000 	subs	r5, r0, #0	; 0x0
    a104:	e24dd014 	sub	sp, sp, #20	; 0x14
    a108:	03e00001 	mvneq	r0, #1	; 0x1
    a10c:	0a00010a 	beq	a53c <spi_read_write_data+0x440>
    a110:	e1a0000d 	mov	r0, sp
    a114:	e3a01000 	mov	r1, #0	; 0x0
    a118:	e3a02010 	mov	r2, #16	; 0x10
    a11c:	ebffdc82 	bl	132c <memset>
    a120:	e1d531d3 	ldrsb	r3, [r5, #19]
    a124:	e353000f 	cmp	r3, #15	; 0xf
    a128:	0a000006 	beq	a148 <spi_read_write_data+0x4c>
    a12c:	ea0000fe 	b	a52c <spi_read_write_data+0x430>
    a130:	e3510000 	cmp	r1, #0	; 0x0
    a134:	0a000007 	beq	a158 <spi_read_write_data+0x5c>
    a138:	e59f0404 	ldr	r0, [pc, #1028]	; a544 <spi_read_write_data+0x448>
    a13c:	e1a0e00f 	mov	lr, pc
    a140:	e594f000 	ldr	pc, [r4]
    a144:	ea000002 	b	a154 <spi_read_write_data+0x58>
    a148:	e59f73f8 	ldr	r7, [pc, #1016]	; a548 <spi_read_write_data+0x44c>
    a14c:	e59f63f8 	ldr	r6, [pc, #1016]	; a54c <spi_read_write_data+0x450>
    a150:	e59f43f8 	ldr	r4, [pc, #1016]	; a550 <spi_read_write_data+0x454>
    a154:	e5971000 	ldr	r1, [r7]
    a158:	e596300c 	ldr	r3, [r6, #12]
    a15c:	e3130001 	tst	r3, #1	; 0x1
    a160:	0afffff2 	beq	a130 <spi_read_write_data+0x34>
    a164:	ea000001 	b	a170 <spi_read_write_data+0x74>
    a168:	e5923008 	ldr	r3, [r2, #8]
    a16c:	ea000000 	b	a174 <spi_read_write_data+0x78>
    a170:	e59f23d4 	ldr	r2, [pc, #980]	; a54c <spi_read_write_data+0x450>
    a174:	e592300c 	ldr	r3, [r2, #12]
    a178:	e3130004 	tst	r3, #4	; 0x4
    a17c:	1afffff9 	bne	a168 <spi_read_write_data+0x6c>
    a180:	e3510000 	cmp	r1, #0	; 0x0
    a184:	0a000003 	beq	a198 <spi_read_write_data+0x9c>
    a188:	e59f03c4 	ldr	r0, [pc, #964]	; a554 <spi_read_write_data+0x458>
    a18c:	e59f33bc 	ldr	r3, [pc, #956]	; a550 <spi_read_write_data+0x454>
    a190:	e1a0e00f 	mov	lr, pc
    a194:	e593f000 	ldr	pc, [r3]
    a198:	e1d531d0 	ldrsb	r3, [r5, #16]
    a19c:	e1d520d4 	ldrsb	r2, [r5, #4]
    a1a0:	e5954008 	ldr	r4, [r5, #8]
    a1a4:	e1d511d2 	ldrsb	r1, [r5, #18]
    a1a8:	e0833002 	add	r3, r3, r2
    a1ac:	e3540000 	cmp	r4, #0	; 0x0
    a1b0:	e0837001 	add	r7, r3, r1
    a1b4:	1a00001f 	bne	a238 <spi_read_write_data+0x13c>
    a1b8:	e3a0004a 	mov	r0, #74	; 0x4a
    a1bc:	e1a01004 	mov	r1, r4
    a1c0:	ebffdad4 	bl	d18 <pads_setpin>
    a1c4:	e1a01004 	mov	r1, r4
    a1c8:	e1d520d4 	ldrsb	r2, [r5, #4]
    a1cc:	e59f0378 	ldr	r0, [pc, #888]	; a54c <spi_read_write_data+0x450>
    a1d0:	ea000002 	b	a1e0 <spi_read_write_data+0xe4>
    a1d4:	e7d53001 	ldrb	r3, [r5, r1]
    a1d8:	e2811001 	add	r1, r1, #1	; 0x1
    a1dc:	e5803008 	str	r3, [r0, #8]
    a1e0:	e1510002 	cmp	r1, r2
    a1e4:	bafffffa 	blt	a1d4 <spi_read_write_data+0xd8>
    a1e8:	e59f235c 	ldr	r2, [pc, #860]	; a54c <spi_read_write_data+0x450>
    a1ec:	e592300c 	ldr	r3, [r2, #12]
    a1f0:	e3130010 	tst	r3, #16	; 0x10
    a1f4:	1afffffc 	bne	a1ec <spi_read_write_data+0xf0>
    a1f8:	e59f234c 	ldr	r2, [pc, #844]	; a54c <spi_read_write_data+0x450>
    a1fc:	e592300c 	ldr	r3, [r2, #12]
    a200:	e3130001 	tst	r3, #1	; 0x1
    a204:	0afffffc 	beq	a1fc <spi_read_write_data+0x100>
    a208:	ea000001 	b	a214 <spi_read_write_data+0x118>
    a20c:	e5923008 	ldr	r3, [r2, #8]
    a210:	ea000000 	b	a218 <spi_read_write_data+0x11c>
    a214:	e59f2330 	ldr	r2, [pc, #816]	; a54c <spi_read_write_data+0x450>
    a218:	e592300c 	ldr	r3, [r2, #12]
    a21c:	e2134004 	ands	r4, r3, #4	; 0x4
    a220:	1afffff9 	bne	a20c <spi_read_write_data+0x110>
    a224:	e3a0004a 	mov	r0, #74	; 0x4a
    a228:	e3a01001 	mov	r1, #1	; 0x1
    a22c:	ebffdab9 	bl	d18 <pads_setpin>
    a230:	e1a00004 	mov	r0, r4
    a234:	ea0000c0 	b	a53c <spi_read_write_data+0x440>
    a238:	e3a02010 	mov	r2, #16	; 0x10
    a23c:	e3a01000 	mov	r1, #0	; 0x0
    a240:	e1a0000d 	mov	r0, sp
    a244:	ebffdc38 	bl	132c <memset>
    a248:	e1a01005 	mov	r1, r5
    a24c:	e1d520d4 	ldrsb	r2, [r5, #4]
    a250:	e1a0000d 	mov	r0, sp
    a254:	ebffdc3c 	bl	134c <memcpy>
    a258:	e1d500d4 	ldrsb	r0, [r5, #4]
    a25c:	e285100c 	add	r1, r5, #12	; 0xc
    a260:	e1d521d0 	ldrsb	r2, [r5, #16]
    a264:	e08d0000 	add	r0, sp, r0
    a268:	ebffdc37 	bl	134c <memcpy>
    a26c:	e1d530d4 	ldrsb	r3, [r5, #4]
    a270:	e1d501d0 	ldrsb	r0, [r5, #16]
    a274:	e1d521d2 	ldrsb	r2, [r5, #18]
    a278:	e0800003 	add	r0, r0, r3
    a27c:	e2851011 	add	r1, r5, #17	; 0x11
    a280:	e08d0000 	add	r0, sp, r0
    a284:	ebffdc30 	bl	134c <memcpy>
    a288:	e3a01000 	mov	r1, #0	; 0x0
    a28c:	e3a0004a 	mov	r0, #74	; 0x4a
    a290:	ebffdaa0 	bl	d18 <pads_setpin>
    a294:	e1a0400d 	mov	r4, sp
    a298:	e3a02000 	mov	r2, #0	; 0x0
    a29c:	e59f12a8 	ldr	r1, [pc, #680]	; a54c <spi_read_write_data+0x450>
    a2a0:	ea000002 	b	a2b0 <spi_read_write_data+0x1b4>
    a2a4:	e19430d2 	ldrsb	r3, [r4, r2]
    a2a8:	e2822001 	add	r2, r2, #1	; 0x1
    a2ac:	e5813008 	str	r3, [r1, #8]
    a2b0:	e1520007 	cmp	r2, r7
    a2b4:	bafffffa 	blt	a2a4 <spi_read_write_data+0x1a8>
    a2b8:	e5956018 	ldr	r6, [r5, #24]
    a2bc:	e595a024 	ldr	sl, [r5, #36]
    a2c0:	e595801c 	ldr	r8, [r5, #28]
    a2c4:	e59f4280 	ldr	r4, [pc, #640]	; a54c <spi_read_write_data+0x450>
    a2c8:	e3a09000 	mov	r9, #0	; 0x0
    a2cc:	e3a0b001 	mov	fp, #1	; 0x1
    a2d0:	e5943018 	ldr	r3, [r4, #24]
    a2d4:	e3130004 	tst	r3, #4	; 0x4
    a2d8:	0a000036 	beq	a3b8 <spi_read_write_data+0x2bc>
    a2dc:	e3570003 	cmp	r7, #3	; 0x3
    a2e0:	da000005 	ble	a2fc <spi_read_write_data+0x200>
    a2e4:	e5943008 	ldr	r3, [r4, #8]
    a2e8:	e2477004 	sub	r7, r7, #4	; 0x4
    a2ec:	e5943008 	ldr	r3, [r4, #8]
    a2f0:	e5943008 	ldr	r3, [r4, #8]
    a2f4:	e5943008 	ldr	r3, [r4, #8]
    a2f8:	ea00002e 	b	a3b8 <spi_read_write_data+0x2bc>
    a2fc:	e3570000 	cmp	r7, #0	; 0x0
    a300:	da000003 	ble	a314 <spi_read_write_data+0x218>
    a304:	e2577001 	subs	r7, r7, #1	; 0x1
    a308:	e5943008 	ldr	r3, [r4, #8]
    a30c:	1afffffc 	bne	a304 <spi_read_write_data+0x208>
    a310:	ea000028 	b	a3b8 <spi_read_write_data+0x2bc>
    a314:	e3560003 	cmp	r6, #3	; 0x3
    a318:	da000018 	ble	a380 <spi_read_write_data+0x284>
    a31c:	e5951014 	ldr	r1, [r5, #20]
    a320:	e3510000 	cmp	r1, #0	; 0x0
    a324:	0a00000f 	beq	a368 <spi_read_write_data+0x26c>
    a328:	e5942008 	ldr	r2, [r4, #8]
    a32c:	e1a03001 	mov	r3, r1
    a330:	e4c32001 	strb	r2, [r3], #1
    a334:	e5942008 	ldr	r2, [r4, #8]
    a338:	e5853014 	str	r3, [r5, #20]
    a33c:	e5c12001 	strb	r2, [r1, #1]
    a340:	e5941008 	ldr	r1, [r4, #8]
    a344:	e2832001 	add	r2, r3, #1	; 0x1
    a348:	e5852014 	str	r2, [r5, #20]
    a34c:	e5c31001 	strb	r1, [r3, #1]
    a350:	e5941008 	ldr	r1, [r4, #8]
    a354:	e2823001 	add	r3, r2, #1	; 0x1
    a358:	e2830001 	add	r0, r3, #1	; 0x1
    a35c:	e5c21001 	strb	r1, [r2, #1]
    a360:	e5850014 	str	r0, [r5, #20]
    a364:	ea000003 	b	a378 <spi_read_write_data+0x27c>
    a368:	e5943008 	ldr	r3, [r4, #8]
    a36c:	e5943008 	ldr	r3, [r4, #8]
    a370:	e5943008 	ldr	r3, [r4, #8]
    a374:	e5943008 	ldr	r3, [r4, #8]
    a378:	e2466004 	sub	r6, r6, #4	; 0x4
    a37c:	ea00000b 	b	a3b0 <spi_read_write_data+0x2b4>
    a380:	e3560000 	cmp	r6, #0	; 0x0
    a384:	da000009 	ble	a3b0 <spi_read_write_data+0x2b4>
    a388:	e59f11bc 	ldr	r1, [pc, #444]	; a54c <spi_read_write_data+0x450>
    a38c:	e5953014 	ldr	r3, [r5, #20]
    a390:	e2532000 	subs	r2, r3, #0	; 0x0
    a394:	15913008 	ldrne	r3, [r1, #8]
    a398:	05913008 	ldreq	r3, [r1, #8]
    a39c:	14c23001 	strbne	r3, [r2], #1
    a3a0:	15852014 	strne	r2, [r5, #20]
    a3a4:	e2566001 	subs	r6, r6, #1	; 0x1
    a3a8:	1afffff7 	bne	a38c <spi_read_write_data+0x290>
    a3ac:	ea000056 	b	a50c <spi_read_write_data+0x410>
    a3b0:	e3560000 	cmp	r6, #0	; 0x0
    a3b4:	0a000054 	beq	a50c <spi_read_write_data+0x410>
    a3b8:	e5943018 	ldr	r3, [r4, #24]
    a3bc:	e3130008 	tst	r3, #8	; 0x8
    a3c0:	0a00002b 	beq	a474 <spi_read_write_data+0x378>
    a3c4:	e35a0003 	cmp	sl, #3	; 0x3
    a3c8:	da000012 	ble	a418 <spi_read_write_data+0x31c>
    a3cc:	e5951020 	ldr	r1, [r5, #32]
    a3d0:	e24aa004 	sub	sl, sl, #4	; 0x4
    a3d4:	e1a02001 	mov	r2, r1
    a3d8:	e4d23001 	ldrb	r3, [r2], #1
    a3dc:	e5843008 	str	r3, [r4, #8]
    a3e0:	e5852020 	str	r2, [r5, #32]
    a3e4:	e5d13001 	ldrb	r3, [r1, #1]
    a3e8:	e2821001 	add	r1, r2, #1	; 0x1
    a3ec:	e5843008 	str	r3, [r4, #8]
    a3f0:	e5851020 	str	r1, [r5, #32]
    a3f4:	e5d22001 	ldrb	r2, [r2, #1]
    a3f8:	e2813001 	add	r3, r1, #1	; 0x1
    a3fc:	e5853020 	str	r3, [r5, #32]
    a400:	e5842008 	str	r2, [r4, #8]
    a404:	e5d12001 	ldrb	r2, [r1, #1]
    a408:	e2833001 	add	r3, r3, #1	; 0x1
    a40c:	e5842008 	str	r2, [r4, #8]
    a410:	e5853020 	str	r3, [r5, #32]
    a414:	ea000016 	b	a474 <spi_read_write_data+0x378>
    a418:	e35a0000 	cmp	sl, #0	; 0x0
    a41c:	da000006 	ble	a43c <spi_read_write_data+0x340>
    a420:	e5953020 	ldr	r3, [r5, #32]
    a424:	e25aa001 	subs	sl, sl, #1	; 0x1
    a428:	e4d32001 	ldrb	r2, [r3], #1
    a42c:	e5842008 	str	r2, [r4, #8]
    a430:	e5853020 	str	r3, [r5, #32]
    a434:	1afffff9 	bne	a420 <spi_read_write_data+0x324>
    a438:	ea00000d 	b	a474 <spi_read_write_data+0x378>
    a43c:	e3580003 	cmp	r8, #3	; 0x3
    a440:	da000005 	ble	a45c <spi_read_write_data+0x360>
    a444:	e5849008 	str	r9, [r4, #8]
    a448:	e2488004 	sub	r8, r8, #4	; 0x4
    a44c:	e5849008 	str	r9, [r4, #8]
    a450:	e5849008 	str	r9, [r4, #8]
    a454:	e5849008 	str	r9, [r4, #8]
    a458:	ea000005 	b	a474 <spi_read_write_data+0x378>
    a45c:	e3580000 	cmp	r8, #0	; 0x0
    a460:	da000003 	ble	a474 <spi_read_write_data+0x378>
    a464:	e3a03000 	mov	r3, #0	; 0x0
    a468:	e2588001 	subs	r8, r8, #1	; 0x1
    a46c:	e5843008 	str	r3, [r4, #8]
    a470:	1afffffb 	bne	a464 <spi_read_write_data+0x368>
    a474:	e5943018 	ldr	r3, [r4, #24]
    a478:	e3130002 	tst	r3, #2	; 0x2
    a47c:	0a000020 	beq	a504 <spi_read_write_data+0x408>
    a480:	e594300c 	ldr	r3, [r4, #12]
    a484:	e3130004 	tst	r3, #4	; 0x4
    a488:	0a00001d 	beq	a504 <spi_read_write_data+0x408>
    a48c:	ea00000a 	b	a4bc <spi_read_write_data+0x3c0>
    a490:	e594300c 	ldr	r3, [r4, #12]
    a494:	e3130004 	tst	r3, #4	; 0x4
    a498:	15943008 	ldrne	r3, [r4, #8]
    a49c:	12477001 	subne	r7, r7, #1	; 0x1
    a4a0:	1a000005 	bne	a4bc <spi_read_write_data+0x3c0>
    a4a4:	e59f00ac 	ldr	r0, [pc, #172]	; a558 <spi_read_write_data+0x45c>
    a4a8:	e1a01007 	mov	r1, r7
    a4ac:	e59f309c 	ldr	r3, [pc, #156]	; a550 <spi_read_write_data+0x454>
    a4b0:	e1a0e00f 	mov	lr, pc
    a4b4:	e593f000 	ldr	pc, [r3]
    a4b8:	ea000011 	b	a504 <spi_read_write_data+0x408>
    a4bc:	e3570000 	cmp	r7, #0	; 0x0
    a4c0:	cafffff2 	bgt	a490 <spi_read_write_data+0x394>
    a4c4:	ea000009 	b	a4f0 <spi_read_write_data+0x3f4>
    a4c8:	e594300c 	ldr	r3, [r4, #12]
    a4cc:	e3130004 	tst	r3, #4	; 0x4
    a4d0:	0a00000b 	beq	a504 <spi_read_write_data+0x408>
    a4d4:	e5953014 	ldr	r3, [r5, #20]
    a4d8:	e2466001 	sub	r6, r6, #1	; 0x1
    a4dc:	e2532000 	subs	r2, r3, #0	; 0x0
    a4e0:	15943008 	ldrne	r3, [r4, #8]
    a4e4:	05943008 	ldreq	r3, [r4, #8]
    a4e8:	14c23001 	strbne	r3, [r2], #1
    a4ec:	15852014 	strne	r2, [r5, #20]
    a4f0:	e3560000 	cmp	r6, #0	; 0x0
    a4f4:	cafffff3 	bgt	a4c8 <spi_read_write_data+0x3cc>
    a4f8:	e3a03002 	mov	r3, #2	; 0x2
    a4fc:	e5843020 	str	r3, [r4, #32]
    a500:	0a000001 	beq	a50c <spi_read_write_data+0x410>
    a504:	e584b020 	str	fp, [r4, #32]
    a508:	eaffff70 	b	a2d0 <spi_read_write_data+0x1d4>
    a50c:	e59f2038 	ldr	r2, [pc, #56]	; a54c <spi_read_write_data+0x450>
    a510:	e592300c 	ldr	r3, [r2, #12]
    a514:	e3130010 	tst	r3, #16	; 0x10
    a518:	1afffffc 	bne	a510 <spi_read_write_data+0x414>
    a51c:	e59f2028 	ldr	r2, [pc, #40]	; a54c <spi_read_write_data+0x450>
    a520:	e592300c 	ldr	r3, [r2, #12]
    a524:	e3130001 	tst	r3, #1	; 0x1
    a528:	0afffffc 	beq	a520 <spi_read_write_data+0x424>
    a52c:	e3a0004a 	mov	r0, #74	; 0x4a
    a530:	e3a01001 	mov	r1, #1	; 0x1
    a534:	ebffd9f7 	bl	d18 <pads_setpin>
    a538:	e3a00000 	mov	r0, #0	; 0x0
    a53c:	e28dd014 	add	sp, sp, #20	; 0x14
    a540:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a544:	0001378a 	.word	0x0001378a
    a548:	0800d2b8 	.word	0x0800d2b8
    a54c:	20500000 	.word	0x20500000
    a550:	0800d76c 	.word	0x0800d76c
    a554:	000137b2 	.word	0x000137b2
    a558:	000137d3 	.word	0x000137d3

0000a55c <flash_set_device_status>:
    a55c:	e92d4070 	push	{r4, r5, r6, lr}
    a560:	e24dd060 	sub	sp, sp, #96	; 0x60
    a564:	e1a04000 	mov	r4, r0
    a568:	e28d0004 	add	r0, sp, #4	; 0x4
    a56c:	e1a06001 	mov	r6, r1
    a570:	e1a05002 	mov	r5, r2
    a574:	ebfffeac 	bl	a02c <spi_transfer_init>
    a578:	e59f303c 	ldr	r3, [pc, #60]	; a5bc <flash_set_device_status+0x60>
    a57c:	e28d0060 	add	r0, sp, #96	; 0x60
    a580:	e5933000 	ldr	r3, [r3]
    a584:	e560405c 	strb	r4, [r0, #-92]!
    a588:	e353000f 	cmp	r3, #15	; 0xf
    a58c:	13a03001 	movne	r3, #1	; 0x1
    a590:	e5cd3017 	strb	r3, [sp, #23]
    a594:	e3a04000 	mov	r4, #0	; 0x0
    a598:	e3a03003 	mov	r3, #3	; 0x3
    a59c:	e5cd3008 	strb	r3, [sp, #8]
    a5a0:	e5cd5006 	strb	r5, [sp, #6]
    a5a4:	e5cd6005 	strb	r6, [sp, #5]
    a5a8:	e58d400c 	str	r4, [sp, #12]
    a5ac:	ebfffed2 	bl	a0fc <spi_read_write_data>
    a5b0:	e1a00004 	mov	r0, r4
    a5b4:	e28dd060 	add	sp, sp, #96	; 0x60
    a5b8:	e8bd8070 	pop	{r4, r5, r6, pc}
    a5bc:	0800d258 	.word	0x0800d258

0000a5c0 <flash_write_enable>:
    a5c0:	e92d4010 	push	{r4, lr}
    a5c4:	e24dd060 	sub	sp, sp, #96	; 0x60
    a5c8:	e28d0004 	add	r0, sp, #4	; 0x4
    a5cc:	ebfffe96 	bl	a02c <spi_transfer_init>
    a5d0:	e59f3030 	ldr	r3, [pc, #48]	; a608 <flash_write_enable+0x48>
    a5d4:	e28d0060 	add	r0, sp, #96	; 0x60
    a5d8:	e5933000 	ldr	r3, [r3]
    a5dc:	e3a04000 	mov	r4, #0	; 0x0
    a5e0:	e5cd3017 	strb	r3, [sp, #23]
    a5e4:	e3a03006 	mov	r3, #6	; 0x6
    a5e8:	e560305c 	strb	r3, [r0, #-92]!
    a5ec:	e3a03001 	mov	r3, #1	; 0x1
    a5f0:	e5cd3008 	strb	r3, [sp, #8]
    a5f4:	e58d400c 	str	r4, [sp, #12]
    a5f8:	ebfffebf 	bl	a0fc <spi_read_write_data>
    a5fc:	e1a00004 	mov	r0, r4
    a600:	e28dd060 	add	sp, sp, #96	; 0x60
    a604:	e8bd8010 	pop	{r4, pc}
    a608:	0800d258 	.word	0x0800d258

0000a60c <nand_flash_reset>:
    a60c:	e92d4010 	push	{r4, lr}
    a610:	e24dd060 	sub	sp, sp, #96	; 0x60
    a614:	e28d4004 	add	r4, sp, #4	; 0x4
    a618:	e1a00004 	mov	r0, r4
    a61c:	ebfffe82 	bl	a02c <spi_transfer_init>
    a620:	e3a03001 	mov	r3, #1	; 0x1
    a624:	e5cd3008 	strb	r3, [sp, #8]
    a628:	e3a03000 	mov	r3, #0	; 0x0
    a62c:	e58d300c 	str	r3, [sp, #12]
    a630:	e1a00004 	mov	r0, r4
    a634:	e283300f 	add	r3, r3, #15	; 0xf
    a638:	e3e04000 	mvn	r4, #0	; 0x0
    a63c:	e5cd3017 	strb	r3, [sp, #23]
    a640:	e5cd4004 	strb	r4, [sp, #4]
    a644:	ebfffeac 	bl	a0fc <spi_read_write_data>
    a648:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    a64c:	ebfff881 	bl	8858 <udelay>
    a650:	e1a00004 	mov	r0, r4
    a654:	e28dd060 	add	sp, sp, #96	; 0x60
    a658:	e8bd8010 	pop	{r4, pc}

0000a65c <flash_wait_device_status>:
    a65c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    a660:	e2514000 	subs	r4, r1, #0	; 0x0
    a664:	e24dd064 	sub	sp, sp, #100	; 0x64
    a668:	e1a03000 	mov	r3, r0
    a66c:	aa000005 	bge	a688 <flash_wait_device_status+0x2c>
    a670:	e59f0364 	ldr	r0, [pc, #868]	; a9dc <flash_wait_device_status+0x380>
    a674:	e59f3364 	ldr	r3, [pc, #868]	; a9e0 <flash_wait_device_status+0x384>
    a678:	e1a0e00f 	mov	lr, pc
    a67c:	e593f000 	ldr	pc, [r3]
    a680:	e3e00001 	mvn	r0, #1	; 0x1
    a684:	ea0000d2 	b	a9d4 <flash_wait_device_status+0x378>
    a688:	e1a0000d 	mov	r0, sp
    a68c:	e6ef5073 	uxtb	r5, r3
    a690:	ebfffe65 	bl	a02c <spi_transfer_init>
    a694:	e3a03002 	mov	r3, #2	; 0x2
    a698:	e58d3008 	str	r3, [sp, #8]
    a69c:	e28d305c 	add	r3, sp, #92	; 0x5c
    a6a0:	e3a02001 	mov	r2, #1	; 0x1
    a6a4:	e58d3014 	str	r3, [sp, #20]
    a6a8:	e2441001 	sub	r1, r4, #1	; 0x1
    a6ac:	e3a0300f 	mov	r3, #15	; 0xf
    a6b0:	e58d201c 	str	r2, [sp, #28]
    a6b4:	e5cd3013 	strb	r3, [sp, #19]
    a6b8:	e5cd5000 	strb	r5, [sp]
    a6bc:	e5cd2004 	strb	r2, [sp, #4]
    a6c0:	e58d2018 	str	r2, [sp, #24]
    a6c4:	e1510003 	cmp	r1, r3
    a6c8:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    a6cc:	ea0000b6 	b	a9ac <flash_wait_device_status+0x350>
    a6d0:	0000a710 	.word	0x0000a710
    a6d4:	0000a9ac 	.word	0x0000a9ac
    a6d8:	0000a9ac 	.word	0x0000a9ac
    a6dc:	0000a9ac 	.word	0x0000a9ac
    a6e0:	0000a9ac 	.word	0x0000a9ac
    a6e4:	0000a8c0 	.word	0x0000a8c0
    a6e8:	0000a93c 	.word	0x0000a93c
    a6ec:	0000a794 	.word	0x0000a794
    a6f0:	0000a7fc 	.word	0x0000a7fc
    a6f4:	0000a9ac 	.word	0x0000a9ac
    a6f8:	0000a9ac 	.word	0x0000a9ac
    a6fc:	0000a9ac 	.word	0x0000a9ac
    a700:	0000a9ac 	.word	0x0000a9ac
    a704:	0000a9ac 	.word	0x0000a9ac
    a708:	0000a9ac 	.word	0x0000a9ac
    a70c:	0000a854 	.word	0x0000a854
    a710:	e59f72cc 	ldr	r7, [pc, #716]	; a9e4 <flash_wait_device_status+0x388>
    a714:	e59f62c4 	ldr	r6, [pc, #708]	; a9e0 <flash_wait_device_status+0x384>
    a718:	e28d405c 	add	r4, sp, #92	; 0x5c
    a71c:	e1a0500d 	mov	r5, sp
    a720:	e3a01000 	mov	r1, #0	; 0x0
    a724:	e3a02002 	mov	r2, #2	; 0x2
    a728:	e1a00004 	mov	r0, r4
    a72c:	ebffdafe 	bl	132c <memset>
    a730:	e1a0000d 	mov	r0, sp
    a734:	e58d4014 	str	r4, [sp, #20]
    a738:	ebfffe6f 	bl	a0fc <spi_read_write_data>
    a73c:	e5dd105c 	ldrb	r1, [sp, #92]
    a740:	e3110002 	tst	r1, #2	; 0x2
    a744:	0a000007 	beq	a768 <flash_wait_device_status+0x10c>
    a748:	e3110001 	tst	r1, #1	; 0x1
    a74c:	1a000005 	bne	a768 <flash_wait_device_status+0x10c>
    a750:	e59f328c 	ldr	r3, [pc, #652]	; a9e4 <flash_wait_device_status+0x388>
    a754:	e5933000 	ldr	r3, [r3]
    a758:	e3530000 	cmp	r3, #0	; 0x0
    a75c:	159f0284 	ldrne	r0, [pc, #644]	; a9e8 <flash_wait_device_status+0x38c>
    a760:	0a000091 	beq	a9ac <flash_wait_device_status+0x350>
    a764:	ea000020 	b	a7ec <flash_wait_device_status+0x190>
    a768:	ebffff94 	bl	a5c0 <flash_write_enable>
    a76c:	e5973000 	ldr	r3, [r7]
    a770:	e3530000 	cmp	r3, #0	; 0x0
    a774:	0a000003 	beq	a788 <flash_wait_device_status+0x12c>
    a778:	e5dd105c 	ldrb	r1, [sp, #92]
    a77c:	e59f0268 	ldr	r0, [pc, #616]	; a9ec <flash_wait_device_status+0x390>
    a780:	e1a0e00f 	mov	lr, pc
    a784:	e596f000 	ldr	pc, [r6]
    a788:	e3a00032 	mov	r0, #50	; 0x32
    a78c:	ebfff831 	bl	8858 <udelay>
    a790:	eaffffe2 	b	a720 <flash_wait_device_status+0xc4>
    a794:	e3a0c002 	mov	ip, #2	; 0x2
    a798:	e28d405c 	add	r4, sp, #92	; 0x5c
    a79c:	e3e0305f 	mvn	r3, #95	; 0x5f
    a7a0:	e1a0200c 	mov	r2, ip
    a7a4:	e3a01000 	mov	r1, #0	; 0x0
    a7a8:	e1a00004 	mov	r0, r4
    a7ac:	e5cd3001 	strb	r3, [sp, #1]
    a7b0:	e5cdc004 	strb	ip, [sp, #4]
    a7b4:	e58dc008 	str	ip, [sp, #8]
    a7b8:	e5cd5000 	strb	r5, [sp]
    a7bc:	ebffdada 	bl	132c <memset>
    a7c0:	e3a03001 	mov	r3, #1	; 0x1
    a7c4:	e1a0000d 	mov	r0, sp
    a7c8:	e58d3018 	str	r3, [sp, #24]
    a7cc:	e58d4014 	str	r4, [sp, #20]
    a7d0:	ebfffe49 	bl	a0fc <spi_read_write_data>
    a7d4:	e59f3208 	ldr	r3, [pc, #520]	; a9e4 <flash_wait_device_status+0x388>
    a7d8:	e5933000 	ldr	r3, [r3]
    a7dc:	e3530000 	cmp	r3, #0	; 0x0
    a7e0:	0a000071 	beq	a9ac <flash_wait_device_status+0x350>
    a7e4:	e59f0204 	ldr	r0, [pc, #516]	; a9f0 <flash_wait_device_status+0x394>
    a7e8:	e5dd105c 	ldrb	r1, [sp, #92]
    a7ec:	e59f31ec 	ldr	r3, [pc, #492]	; a9e0 <flash_wait_device_status+0x384>
    a7f0:	e1a0e00f 	mov	lr, pc
    a7f4:	e593f000 	ldr	pc, [r3]
    a7f8:	ea00006b 	b	a9ac <flash_wait_device_status+0x350>
    a7fc:	e3a0c002 	mov	ip, #2	; 0x2
    a800:	e28d405c 	add	r4, sp, #92	; 0x5c
    a804:	e3e0304f 	mvn	r3, #79	; 0x4f
    a808:	e1a0200c 	mov	r2, ip
    a80c:	e3a01000 	mov	r1, #0	; 0x0
    a810:	e1a00004 	mov	r0, r4
    a814:	e5cd3001 	strb	r3, [sp, #1]
    a818:	e5cdc004 	strb	ip, [sp, #4]
    a81c:	e58dc008 	str	ip, [sp, #8]
    a820:	e5cd5000 	strb	r5, [sp]
    a824:	ebffdac0 	bl	132c <memset>
    a828:	e3a03001 	mov	r3, #1	; 0x1
    a82c:	e1a0000d 	mov	r0, sp
    a830:	e58d3018 	str	r3, [sp, #24]
    a834:	e58d4014 	str	r4, [sp, #20]
    a838:	ebfffe2f 	bl	a0fc <spi_read_write_data>
    a83c:	e59f31a0 	ldr	r3, [pc, #416]	; a9e4 <flash_wait_device_status+0x388>
    a840:	e5933000 	ldr	r3, [r3]
    a844:	e3530000 	cmp	r3, #0	; 0x0
    a848:	159f01a4 	ldrne	r0, [pc, #420]	; a9f4 <flash_wait_device_status+0x398>
    a84c:	0a000019 	beq	a8b8 <flash_wait_device_status+0x25c>
    a850:	ea000014 	b	a8a8 <flash_wait_device_status+0x24c>
    a854:	e3a0c002 	mov	ip, #2	; 0x2
    a858:	e28d405c 	add	r4, sp, #92	; 0x5c
    a85c:	e3e0303f 	mvn	r3, #63	; 0x3f
    a860:	e1a0200c 	mov	r2, ip
    a864:	e3a01000 	mov	r1, #0	; 0x0
    a868:	e1a00004 	mov	r0, r4
    a86c:	e5cd3001 	strb	r3, [sp, #1]
    a870:	e5cdc004 	strb	ip, [sp, #4]
    a874:	e58dc008 	str	ip, [sp, #8]
    a878:	e5cd5000 	strb	r5, [sp]
    a87c:	ebffdaaa 	bl	132c <memset>
    a880:	e3a03001 	mov	r3, #1	; 0x1
    a884:	e1a0000d 	mov	r0, sp
    a888:	e58d3018 	str	r3, [sp, #24]
    a88c:	e58d4014 	str	r4, [sp, #20]
    a890:	ebfffe19 	bl	a0fc <spi_read_write_data>
    a894:	e59f3148 	ldr	r3, [pc, #328]	; a9e4 <flash_wait_device_status+0x388>
    a898:	e5933000 	ldr	r3, [r3]
    a89c:	e3530000 	cmp	r3, #0	; 0x0
    a8a0:	0a000004 	beq	a8b8 <flash_wait_device_status+0x25c>
    a8a4:	e59f014c 	ldr	r0, [pc, #332]	; a9f8 <flash_wait_device_status+0x39c>
    a8a8:	e5dd105c 	ldrb	r1, [sp, #92]
    a8ac:	e59f312c 	ldr	r3, [pc, #300]	; a9e0 <flash_wait_device_status+0x384>
    a8b0:	e1a0e00f 	mov	lr, pc
    a8b4:	e593f000 	ldr	pc, [r3]
    a8b8:	e5dd005c 	ldrb	r0, [sp, #92]
    a8bc:	ea000044 	b	a9d4 <flash_wait_device_status+0x378>
    a8c0:	e3a02002 	mov	r2, #2	; 0x2
    a8c4:	e3e0303f 	mvn	r3, #63	; 0x3f
    a8c8:	e5cd3001 	strb	r3, [sp, #1]
    a8cc:	e58d2008 	str	r2, [sp, #8]
    a8d0:	e5cd5000 	strb	r5, [sp]
    a8d4:	e5cd2004 	strb	r2, [sp, #4]
    a8d8:	e3a04000 	mov	r4, #0	; 0x0
    a8dc:	e28d605c 	add	r6, sp, #92	; 0x5c
    a8e0:	e1a0700d 	mov	r7, sp
    a8e4:	e3a01000 	mov	r1, #0	; 0x0
    a8e8:	e3a02002 	mov	r2, #2	; 0x2
    a8ec:	e1a00006 	mov	r0, r6
    a8f0:	ebffda8d 	bl	132c <memset>
    a8f4:	e1a0000d 	mov	r0, sp
    a8f8:	e58d6014 	str	r6, [sp, #20]
    a8fc:	ebfffdfe 	bl	a0fc <spi_read_write_data>
    a900:	e5dd305c 	ldrb	r3, [sp, #92]
    a904:	e2844001 	add	r4, r4, #1	; 0x1
    a908:	e3130001 	tst	r3, #1	; 0x1
    a90c:	e3a0001e 	mov	r0, #30	; 0x1e
    a910:	0a000009 	beq	a93c <flash_wait_device_status+0x2e0>
    a914:	ebfff7cf 	bl	8858 <udelay>
    a918:	e3540064 	cmp	r4, #100	; 0x64
    a91c:	1afffff0 	bne	a8e4 <flash_wait_device_status+0x288>
    a920:	e59f00d4 	ldr	r0, [pc, #212]	; a9fc <flash_wait_device_status+0x3a0>
    a924:	e5dd105c 	ldrb	r1, [sp, #92]
    a928:	e59f30b0 	ldr	r3, [pc, #176]	; a9e0 <flash_wait_device_status+0x384>
    a92c:	e1a0e00f 	mov	lr, pc
    a930:	e593f000 	ldr	pc, [r3]
    a934:	e3e00000 	mvn	r0, #0	; 0x0
    a938:	ea000025 	b	a9d4 <flash_wait_device_status+0x378>
    a93c:	e3a02002 	mov	r2, #2	; 0x2
    a940:	e3e0303f 	mvn	r3, #63	; 0x3f
    a944:	e59f7098 	ldr	r7, [pc, #152]	; a9e4 <flash_wait_device_status+0x388>
    a948:	e59f6090 	ldr	r6, [pc, #144]	; a9e0 <flash_wait_device_status+0x384>
    a94c:	e5cd5000 	strb	r5, [sp]
    a950:	e5cd3001 	strb	r3, [sp, #1]
    a954:	e58d2008 	str	r2, [sp, #8]
    a958:	e5cd2004 	strb	r2, [sp, #4]
    a95c:	e28d405c 	add	r4, sp, #92	; 0x5c
    a960:	e1a0500d 	mov	r5, sp
    a964:	e3a01000 	mov	r1, #0	; 0x0
    a968:	e3a02002 	mov	r2, #2	; 0x2
    a96c:	e1a00004 	mov	r0, r4
    a970:	ebffda6d 	bl	132c <memset>
    a974:	e1a0000d 	mov	r0, sp
    a978:	e58d4014 	str	r4, [sp, #20]
    a97c:	ebfffdde 	bl	a0fc <spi_read_write_data>
    a980:	e5dd105c 	ldrb	r1, [sp, #92]
    a984:	e3110002 	tst	r1, #2	; 0x2
    a988:	1a000007 	bne	a9ac <flash_wait_device_status+0x350>
    a98c:	e5973000 	ldr	r3, [r7]
    a990:	e3530000 	cmp	r3, #0	; 0x0
    a994:	0a000002 	beq	a9a4 <flash_wait_device_status+0x348>
    a998:	e59f0060 	ldr	r0, [pc, #96]	; aa00 <flash_wait_device_status+0x3a4>
    a99c:	e1a0e00f 	mov	lr, pc
    a9a0:	e596f000 	ldr	pc, [r6]
    a9a4:	ebffff05 	bl	a5c0 <flash_write_enable>
    a9a8:	eaffffed 	b	a964 <flash_wait_device_status+0x308>
    a9ac:	e59f3030 	ldr	r3, [pc, #48]	; a9e4 <flash_wait_device_status+0x388>
    a9b0:	e5930000 	ldr	r0, [r3]
    a9b4:	e3500000 	cmp	r0, #0	; 0x0
    a9b8:	0a000005 	beq	a9d4 <flash_wait_device_status+0x378>
    a9bc:	e59f0040 	ldr	r0, [pc, #64]	; aa04 <flash_wait_device_status+0x3a8>
    a9c0:	e5dd105c 	ldrb	r1, [sp, #92]
    a9c4:	e59f3014 	ldr	r3, [pc, #20]	; a9e0 <flash_wait_device_status+0x384>
    a9c8:	e1a0e00f 	mov	lr, pc
    a9cc:	e593f000 	ldr	pc, [r3]
    a9d0:	e3a00000 	mov	r0, #0	; 0x0
    a9d4:	e28dd064 	add	sp, sp, #100	; 0x64
    a9d8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    a9dc:	000137ff 	.word	0x000137ff
    a9e0:	0800d76c 	.word	0x0800d76c
    a9e4:	0800d2b8 	.word	0x0800d2b8
    a9e8:	0001382b 	.word	0x0001382b
    a9ec:	00013855 	.word	0x00013855
    a9f0:	00013878 	.word	0x00013878
    a9f4:	000138a3 	.word	0x000138a3
    a9f8:	000138ce 	.word	0x000138ce
    a9fc:	000138f9 	.word	0x000138f9
    aa00:	00013938 	.word	0x00013938
    aa04:	00013979 	.word	0x00013979

0000aa08 <nand_write_cache_to_memory>:
    aa08:	e92d4030 	push	{r4, r5, lr}
    aa0c:	e24dd064 	sub	sp, sp, #100	; 0x64
    aa10:	e1a05000 	mov	r5, r0
    aa14:	e28d0004 	add	r0, sp, #4	; 0x4
    aa18:	e1a04001 	mov	r4, r1
    aa1c:	ebfffd82 	bl	a02c <spi_transfer_init>
    aa20:	e354000f 	cmp	r4, #15	; 0xf
    aa24:	13a03001 	movne	r3, #1	; 0x1
    aa28:	15cd3017 	strbne	r3, [sp, #23]
    aa2c:	e3a03010 	mov	r3, #16	; 0x10
    aa30:	e1a01335 	lsr	r1, r5, r3
    aa34:	e28d0060 	add	r0, sp, #96	; 0x60
    aa38:	05cd4017 	strbeq	r4, [sp, #23]
    aa3c:	e1a02425 	lsr	r2, r5, #8
    aa40:	e3a04000 	mov	r4, #0	; 0x0
    aa44:	e560305c 	strb	r3, [r0, #-92]!
    aa48:	e3a03004 	mov	r3, #4	; 0x4
    aa4c:	e5cd3008 	strb	r3, [sp, #8]
    aa50:	e5cd2006 	strb	r2, [sp, #6]
    aa54:	e5cd1005 	strb	r1, [sp, #5]
    aa58:	e58d400c 	str	r4, [sp, #12]
    aa5c:	e5cd5007 	strb	r5, [sp, #7]
    aa60:	ebfffda5 	bl	a0fc <spi_read_write_data>
    aa64:	e3a01006 	mov	r1, #6	; 0x6
    aa68:	e3a0000f 	mov	r0, #15	; 0xf
    aa6c:	ebfffefa 	bl	a65c <flash_wait_device_status>
    aa70:	e1a00004 	mov	r0, r4
    aa74:	e28dd064 	add	sp, sp, #100	; 0x64
    aa78:	e8bd8030 	pop	{r4, r5, pc}

0000aa7c <nand_read_page_to_cache>:
    aa7c:	e92d4030 	push	{r4, r5, lr}
    aa80:	e24dd064 	sub	sp, sp, #100	; 0x64
    aa84:	e1a04001 	mov	r4, r1
    aa88:	e1a05000 	mov	r5, r0
    aa8c:	e28d0004 	add	r0, sp, #4	; 0x4
    aa90:	ebfffd65 	bl	a02c <spi_transfer_init>
    aa94:	e354000f 	cmp	r4, #15	; 0xf
    aa98:	e28d0060 	add	r0, sp, #96	; 0x60
    aa9c:	e3a03013 	mov	r3, #19	; 0x13
    aaa0:	05cd4017 	strbeq	r4, [sp, #23]
    aaa4:	e1a01825 	lsr	r1, r5, #16
    aaa8:	e1a02425 	lsr	r2, r5, #8
    aaac:	e3a04000 	mov	r4, #0	; 0x0
    aab0:	e560305c 	strb	r3, [r0, #-92]!
    aab4:	e3a03004 	mov	r3, #4	; 0x4
    aab8:	e5cd3008 	strb	r3, [sp, #8]
    aabc:	e5cd2006 	strb	r2, [sp, #6]
    aac0:	e5cd1005 	strb	r1, [sp, #5]
    aac4:	e58d400c 	str	r4, [sp, #12]
    aac8:	e5cd5007 	strb	r5, [sp, #7]
    aacc:	ebfffd8a 	bl	a0fc <spi_read_write_data>
    aad0:	e3a01006 	mov	r1, #6	; 0x6
    aad4:	e3a0000f 	mov	r0, #15	; 0xf
    aad8:	ebfffedf 	bl	a65c <flash_wait_device_status>
    aadc:	e1a00004 	mov	r0, r4
    aae0:	e28dd064 	add	sp, sp, #100	; 0x64
    aae4:	e8bd8030 	pop	{r4, r5, pc}

0000aae8 <nand_blk_prg_era_fail>:
    aae8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    aaec:	e3a0000f 	mov	r0, #15	; 0xf
    aaf0:	e24dd004 	sub	sp, sp, #4	; 0x4
    aaf4:	e3a01010 	mov	r1, #16	; 0x10
    aaf8:	ebfffed7 	bl	a65c <flash_wait_device_status>
    aafc:	e3100008 	tst	r0, #8	; 0x8
    ab00:	0a000004 	beq	ab18 <nand_blk_prg_era_fail+0x30>
    ab04:	e59f0038 	ldr	r0, [pc, #56]	; ab44 <nand_blk_prg_era_fail+0x5c>
    ab08:	e59f3038 	ldr	r3, [pc, #56]	; ab48 <nand_blk_prg_era_fail+0x60>
    ab0c:	e1a0e00f 	mov	lr, pc
    ab10:	e593f000 	ldr	pc, [r3]
    ab14:	ea000007 	b	ab38 <nand_blk_prg_era_fail+0x50>
    ab18:	e2103004 	ands	r3, r0, #4	; 0x4
    ab1c:	01a00003 	moveq	r0, r3
    ab20:	0a000005 	beq	ab3c <nand_blk_prg_era_fail+0x54>
    ab24:	e1a01000 	mov	r1, r0
    ab28:	e59f3018 	ldr	r3, [pc, #24]	; ab48 <nand_blk_prg_era_fail+0x60>
    ab2c:	e59f0018 	ldr	r0, [pc, #24]	; ab4c <nand_blk_prg_era_fail+0x64>
    ab30:	e1a0e00f 	mov	lr, pc
    ab34:	e593f000 	ldr	pc, [r3]
    ab38:	e3a00001 	mov	r0, #1	; 0x1
    ab3c:	e28dd004 	add	sp, sp, #4	; 0x4
    ab40:	e8bd8000 	pop	{pc}
    ab44:	000139b8 	.word	0x000139b8
    ab48:	0800d76c 	.word	0x0800d76c
    ab4c:	000139dc 	.word	0x000139dc

0000ab50 <nand_ecc_bits_check>:
    ab50:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ab54:	e3a01010 	mov	r1, #16	; 0x10
    ab58:	e24dd004 	sub	sp, sp, #4	; 0x4
    ab5c:	e3a0000f 	mov	r0, #15	; 0xf
    ab60:	ebfffebd 	bl	a65c <flash_wait_device_status>
    ab64:	e2000030 	and	r0, r0, #48	; 0x30
    ab68:	e1a00220 	lsr	r0, r0, #4
    ab6c:	e28dd004 	add	sp, sp, #4	; 0x4
    ab70:	e8bd8000 	pop	{pc}

0000ab74 <nand_ecc_enable>:
    ab74:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    ab78:	e59f40b4 	ldr	r4, [pc, #180]	; ac34 <nand_ecc_enable+0xc0>
    ab7c:	e24dd064 	sub	sp, sp, #100	; 0x64
    ab80:	e594600c 	ldr	r6, [r4, #12]
    ab84:	e3560001 	cmp	r6, #1	; 0x1
    ab88:	1a000026 	bne	ac28 <nand_ecc_enable+0xb4>
    ab8c:	e5943040 	ldr	r3, [r4, #64]
    ab90:	e3530001 	cmp	r3, #1	; 0x1
    ab94:	0a000006 	beq	abb4 <nand_ecc_enable+0x40>
    ab98:	e3a0000f 	mov	r0, #15	; 0xf
    ab9c:	e3a01009 	mov	r1, #9	; 0x9
    aba0:	ebfffead 	bl	a65c <flash_wait_device_status>
    aba4:	e2107010 	ands	r7, r0, #16	; 0x10
    aba8:	e1a05000 	mov	r5, r0
    abac:	0a000002 	beq	abbc <nand_ecc_enable+0x48>
    abb0:	e5846040 	str	r6, [r4, #64]
    abb4:	e3a00000 	mov	r0, #0	; 0x0
    abb8:	ea00001b 	b	ac2c <nand_ecc_enable+0xb8>
    abbc:	e28d4004 	add	r4, sp, #4	; 0x4
    abc0:	e1a00004 	mov	r0, r4
    abc4:	ebfffd18 	bl	a02c <spi_transfer_init>
    abc8:	e3a0301f 	mov	r3, #31	; 0x1f
    abcc:	e5cd3004 	strb	r3, [sp, #4]
    abd0:	e3e0304f 	mvn	r3, #79	; 0x4f
    abd4:	e3852010 	orr	r2, r5, #16	; 0x10
    abd8:	e1a00004 	mov	r0, r4
    abdc:	e5cd3005 	strb	r3, [sp, #5]
    abe0:	e3a0400f 	mov	r4, #15	; 0xf
    abe4:	e2833053 	add	r3, r3, #83	; 0x53
    abe8:	e5cd2006 	strb	r2, [sp, #6]
    abec:	e5cd3008 	strb	r3, [sp, #8]
    abf0:	e58d700c 	str	r7, [sp, #12]
    abf4:	e5cd4017 	strb	r4, [sp, #23]
    abf8:	ebfffd3f 	bl	a0fc <spi_read_write_data>
    abfc:	e1a00004 	mov	r0, r4
    ac00:	e3a01009 	mov	r1, #9	; 0x9
    ac04:	ebfffe94 	bl	a65c <flash_wait_device_status>
    ac08:	e3100010 	tst	r0, #16	; 0x10
    ac0c:	0a000005 	beq	ac28 <nand_ecc_enable+0xb4>
    ac10:	e59f0020 	ldr	r0, [pc, #32]	; ac38 <nand_ecc_enable+0xc4>
    ac14:	e59f3020 	ldr	r3, [pc, #32]	; ac3c <nand_ecc_enable+0xc8>
    ac18:	e1a0e00f 	mov	lr, pc
    ac1c:	e593f000 	ldr	pc, [r3]
    ac20:	e1a00007 	mov	r0, r7
    ac24:	ea000000 	b	ac2c <nand_ecc_enable+0xb8>
    ac28:	e3e00000 	mvn	r0, #0	; 0x0
    ac2c:	e28dd064 	add	sp, sp, #100	; 0x64
    ac30:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    ac34:	0800d258 	.word	0x0800d258
    ac38:	00013a12 	.word	0x00013a12
    ac3c:	0800d76c 	.word	0x0800d76c

0000ac40 <flash_nand_vs_read>:
    ac40:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ac44:	e59f4438 	ldr	r4, [pc, #1080]	; b084 <flash_nand_vs_read+0x444>
    ac48:	e24dd074 	sub	sp, sp, #116	; 0x74
    ac4c:	e1a05000 	mov	r5, r0
    ac50:	e5941028 	ldr	r1, [r4, #40]
    ac54:	e1a00002 	mov	r0, r2
    ac58:	eb0015ac 	bl	10310 <__aeabi_idiv>
    ac5c:	e594305c 	ldr	r3, [r4, #92]
    ac60:	e1a07000 	mov	r7, r0
    ac64:	e3530001 	cmp	r3, #1	; 0x1
    ac68:	e59d6098 	ldr	r6, [sp, #152]
    ac6c:	e1a08000 	mov	r8, r0
    ac70:	e1a09fc8 	asr	r9, r8, #31
    ac74:	0a000004 	beq	ac8c <flash_nand_vs_read+0x4c>
    ac78:	ebfffbc3 	bl	9b8c <ssp_manager_init>
    ac7c:	e3a03064 	mov	r3, #100	; 0x64
    ac80:	e5843050 	str	r3, [r4, #80]
    ac84:	e3a0300f 	mov	r3, #15	; 0xf
    ac88:	e5843000 	str	r3, [r4]
    ac8c:	e59f33f4 	ldr	r3, [pc, #1012]	; b088 <flash_nand_vs_read+0x448>
    ac90:	e5933000 	ldr	r3, [r3]
    ac94:	e3530000 	cmp	r3, #0	; 0x0
    ac98:	0a000007 	beq	acbc <flash_nand_vs_read+0x7c>
    ac9c:	e59f23e0 	ldr	r2, [pc, #992]	; b084 <flash_nand_vs_read+0x444>
    aca0:	e59f03e4 	ldr	r0, [pc, #996]	; b08c <flash_nand_vs_read+0x44c>
    aca4:	e5923050 	ldr	r3, [r2, #80]
    aca8:	e5921000 	ldr	r1, [r2]
    acac:	e59fc3dc 	ldr	ip, [pc, #988]	; b090 <flash_nand_vs_read+0x450>
    acb0:	e5922004 	ldr	r2, [r2, #4]
    acb4:	e1a0e00f 	mov	lr, pc
    acb8:	e59cf000 	ldr	pc, [ip]
    acbc:	e59f33c4 	ldr	r3, [pc, #964]	; b088 <flash_nand_vs_read+0x448>
    acc0:	e59f23bc 	ldr	r2, [pc, #956]	; b084 <flash_nand_vs_read+0x444>
    acc4:	e5933000 	ldr	r3, [r3]
    acc8:	e5921044 	ldr	r1, [r2, #68]
    accc:	e3530000 	cmp	r3, #0	; 0x0
    acd0:	e59f33bc 	ldr	r3, [pc, #956]	; b094 <flash_nand_vs_read+0x454>
    acd4:	e5924000 	ldr	r4, [r2]
    acd8:	e5831000 	str	r1, [r3]
    acdc:	e5921004 	ldr	r1, [r2, #4]
    ace0:	0a00000e 	beq	ad20 <flash_nand_vs_read+0xe0>
    ace4:	e59f23ac 	ldr	r2, [pc, #940]	; b098 <flash_nand_vs_read+0x458>
    ace8:	e59f33ac 	ldr	r3, [pc, #940]	; b09c <flash_nand_vs_read+0x45c>
    acec:	e3510000 	cmp	r1, #0	; 0x0
    acf0:	01a03002 	moveq	r3, r2
    acf4:	e58d300c 	str	r3, [sp, #12]
    acf8:	e59f3390 	ldr	r3, [pc, #912]	; b090 <flash_nand_vs_read+0x450>
    acfc:	e58d1008 	str	r1, [sp, #8]
    ad00:	e593c000 	ldr	ip, [r3]
    ad04:	e59f0394 	ldr	r0, [pc, #916]	; b0a0 <flash_nand_vs_read+0x460>
    ad08:	e58d6000 	str	r6, [sp]
    ad0c:	e58d4004 	str	r4, [sp, #4]
    ad10:	e59f138c 	ldr	r1, [pc, #908]	; b0a4 <flash_nand_vs_read+0x464>
    ad14:	e1a02008 	mov	r2, r8
    ad18:	e1a03009 	mov	r3, r9
    ad1c:	e12fff3c 	blx	ip
    ad20:	e28d0014 	add	r0, sp, #20	; 0x14
    ad24:	ebfffcc0 	bl	a02c <spi_transfer_init>
    ad28:	e354000f 	cmp	r4, #15	; 0xf
    ad2c:	159f0374 	ldrne	r0, [pc, #884]	; b0a8 <flash_nand_vs_read+0x468>
    ad30:	1a0000cc 	bne	b068 <flash_nand_vs_read+0x428>
    ad34:	e59f334c 	ldr	r3, [pc, #844]	; b088 <flash_nand_vs_read+0x448>
    ad38:	e5933000 	ldr	r3, [r3]
    ad3c:	e3530000 	cmp	r3, #0	; 0x0
    ad40:	0a000005 	beq	ad5c <flash_nand_vs_read+0x11c>
    ad44:	e59f0360 	ldr	r0, [pc, #864]	; b0ac <flash_nand_vs_read+0x46c>
    ad48:	e59f1354 	ldr	r1, [pc, #852]	; b0a4 <flash_nand_vs_read+0x464>
    ad4c:	e3002381 	movw	r2, #897	; 0x381
    ad50:	e59f3338 	ldr	r3, [pc, #824]	; b090 <flash_nand_vs_read+0x450>
    ad54:	e1a0e00f 	mov	lr, pc
    ad58:	e593f000 	ldr	pc, [r3]
    ad5c:	e59f3324 	ldr	r3, [pc, #804]	; b088 <flash_nand_vs_read+0x448>
    ad60:	e3a02001 	mov	r2, #1	; 0x1
    ad64:	e5933000 	ldr	r3, [r3]
    ad68:	e5cd2026 	strb	r2, [sp, #38]
    ad6c:	e3530000 	cmp	r3, #0	; 0x0
    ad70:	e3a03003 	mov	r3, #3	; 0x3
    ad74:	e5cd3014 	strb	r3, [sp, #20]
    ad78:	e3a03002 	mov	r3, #2	; 0x2
    ad7c:	e58d301c 	str	r3, [sp, #28]
    ad80:	e3a03000 	mov	r3, #0	; 0x0
    ad84:	e5cd3025 	strb	r3, [sp, #37]
    ad88:	e283300f 	add	r3, r3, #15	; 0xf
    ad8c:	e5cd3027 	strb	r3, [sp, #39]
    ad90:	e5cd2018 	strb	r2, [sp, #24]
    ad94:	0a000005 	beq	adb0 <flash_nand_vs_read+0x170>
    ad98:	e59f030c 	ldr	r0, [pc, #780]	; b0ac <flash_nand_vs_read+0x46c>
    ad9c:	e59f1300 	ldr	r1, [pc, #768]	; b0a4 <flash_nand_vs_read+0x464>
    ada0:	e3a02fe2 	mov	r2, #904	; 0x388
    ada4:	e59f32e4 	ldr	r3, [pc, #740]	; b090 <flash_nand_vs_read+0x450>
    ada8:	e1a0e00f 	mov	lr, pc
    adac:	e593f000 	ldr	pc, [r3]
    adb0:	e59f32d0 	ldr	r3, [pc, #720]	; b088 <flash_nand_vs_read+0x448>
    adb4:	e58d5028 	str	r5, [sp, #40]
    adb8:	e5932000 	ldr	r2, [r3]
    adbc:	e59f32d0 	ldr	r3, [pc, #720]	; b094 <flash_nand_vs_read+0x454>
    adc0:	e3520000 	cmp	r2, #0	; 0x0
    adc4:	e5933000 	ldr	r3, [r3]
    adc8:	e58d803c 	str	r8, [sp, #60]
    adcc:	e58d3030 	str	r3, [sp, #48]
    add0:	e58d302c 	str	r3, [sp, #44]
    add4:	0a000005 	beq	adf0 <flash_nand_vs_read+0x1b0>
    add8:	e59f02cc 	ldr	r0, [pc, #716]	; b0ac <flash_nand_vs_read+0x46c>
    addc:	e59f12c0 	ldr	r1, [pc, #704]	; b0a4 <flash_nand_vs_read+0x464>
    ade0:	e3002395 	movw	r2, #917	; 0x395
    ade4:	e59f32a4 	ldr	r3, [pc, #676]	; b090 <flash_nand_vs_read+0x450>
    ade8:	e1a0e00f 	mov	lr, pc
    adec:	e593f000 	ldr	pc, [r3]
    adf0:	e59f329c 	ldr	r3, [pc, #668]	; b094 <flash_nand_vs_read+0x454>
    adf4:	e1a00006 	mov	r0, r6
    adf8:	e593b000 	ldr	fp, [r3]
    adfc:	e1a0100b 	mov	r1, fp
    ae00:	eb001542 	bl	10310 <__aeabi_idiv>
    ae04:	e59f327c 	ldr	r3, [pc, #636]	; b088 <flash_nand_vs_read+0x448>
    ae08:	e1a0a000 	mov	sl, r0
    ae0c:	e5933000 	ldr	r3, [r3]
    ae10:	e3530000 	cmp	r3, #0	; 0x0
    ae14:	0a000004 	beq	ae2c <flash_nand_vs_read+0x1ec>
    ae18:	e59f0290 	ldr	r0, [pc, #656]	; b0b0 <flash_nand_vs_read+0x470>
    ae1c:	e1a0100a 	mov	r1, sl
    ae20:	e59f3268 	ldr	r3, [pc, #616]	; b090 <flash_nand_vs_read+0x450>
    ae24:	e1a0e00f 	mov	lr, pc
    ae28:	e593f000 	ldr	pc, [r3]
    ae2c:	e3a05000 	mov	r5, #0	; 0x0
    ae30:	e1a09005 	mov	r9, r5
    ae34:	e59f8248 	ldr	r8, [pc, #584]	; b084 <flash_nand_vs_read+0x444>
    ae38:	ea000047 	b	af5c <flash_nand_vs_read+0x31c>
    ae3c:	e59d303c 	ldr	r3, [sp, #60]
    ae40:	e598200c 	ldr	r2, [r8, #12]
    ae44:	e1a01823 	lsr	r1, r3, #16
    ae48:	e3520001 	cmp	r2, #1	; 0x1
    ae4c:	e1a02423 	lsr	r2, r3, #8
    ae50:	e5cd2021 	strb	r2, [sp, #33]
    ae54:	13a02004 	movne	r2, #4	; 0x4
    ae58:	e5cd1020 	strb	r1, [sp, #32]
    ae5c:	e5cd3022 	strb	r3, [sp, #34]
    ae60:	e5cd9023 	strb	r9, [sp, #35]
    ae64:	15cd2024 	strbne	r2, [sp, #36]
    ae68:	15cd9026 	strbne	r9, [sp, #38]
    ae6c:	1a000014 	bne	aec4 <flash_nand_vs_read+0x284>
    ae70:	e3a04000 	mov	r4, #0	; 0x0
    ae74:	ebffff3e 	bl	ab74 <nand_ecc_enable>
    ae78:	e3500000 	cmp	r0, #0	; 0x0
    ae7c:	e2844001 	add	r4, r4, #1	; 0x1
    ae80:	0a000001 	beq	ae8c <flash_nand_vs_read+0x24c>
    ae84:	e354000a 	cmp	r4, #10	; 0xa
    ae88:	1afffff9 	bne	ae74 <flash_nand_vs_read+0x234>
    ae8c:	e598303c 	ldr	r3, [r8, #60]
    ae90:	e3a02000 	mov	r2, #0	; 0x0
    ae94:	e3530001 	cmp	r3, #1	; 0x1
    ae98:	02833007 	addeq	r3, r3, #7	; 0x7
    ae9c:	e5cd9020 	strb	r9, [sp, #32]
    aea0:	e5cd2021 	strb	r2, [sp, #33]
    aea4:	05cd3020 	strbeq	r3, [sp, #32]
    aea8:	e3a02002 	mov	r2, #2	; 0x2
    aeac:	e3a03001 	mov	r3, #1	; 0x1
    aeb0:	e0850007 	add	r0, r5, r7
    aeb4:	e3a0100f 	mov	r1, #15	; 0xf
    aeb8:	e5cd2024 	strb	r2, [sp, #36]
    aebc:	e5cd3026 	strb	r3, [sp, #38]
    aec0:	ebfffeed 	bl	aa7c <nand_read_page_to_cache>
    aec4:	e59f21bc 	ldr	r2, [pc, #444]	; b088 <flash_nand_vs_read+0x448>
    aec8:	e5923000 	ldr	r3, [r2]
    aecc:	e3530000 	cmp	r3, #0	; 0x0
    aed0:	0a000005 	beq	aeec <flash_nand_vs_read+0x2ac>
    aed4:	e59f01d8 	ldr	r0, [pc, #472]	; b0b4 <flash_nand_vs_read+0x474>
    aed8:	e59d102c 	ldr	r1, [sp, #44]
    aedc:	e59d203c 	ldr	r2, [sp, #60]
    aee0:	e59f31a8 	ldr	r3, [pc, #424]	; b090 <flash_nand_vs_read+0x450>
    aee4:	e1a0e00f 	mov	lr, pc
    aee8:	e593f000 	ldr	pc, [r3]
    aeec:	e59f2194 	ldr	r2, [pc, #404]	; b088 <flash_nand_vs_read+0x448>
    aef0:	e5923000 	ldr	r3, [r2]
    aef4:	e3530000 	cmp	r3, #0	; 0x0
    aef8:	0a000004 	beq	af10 <flash_nand_vs_read+0x2d0>
    aefc:	e59f01b4 	ldr	r0, [pc, #436]	; b0b8 <flash_nand_vs_read+0x478>
    af00:	e1a01005 	mov	r1, r5
    af04:	e59f3184 	ldr	r3, [pc, #388]	; b090 <flash_nand_vs_read+0x450>
    af08:	e1a0e00f 	mov	lr, pc
    af0c:	e593f000 	ldr	pc, [r3]
    af10:	e5983028 	ldr	r3, [r8, #40]
    af14:	e28d0014 	add	r0, sp, #20	; 0x14
    af18:	e58d3030 	str	r3, [sp, #48]
    af1c:	e58d302c 	str	r3, [sp, #44]
    af20:	ebfffc75 	bl	a0fc <spi_read_write_data>
    af24:	e598300c 	ldr	r3, [r8, #12]
    af28:	e3530001 	cmp	r3, #1	; 0x1
    af2c:	1a000002 	bne	af3c <flash_nand_vs_read+0x2fc>
    af30:	ebffff06 	bl	ab50 <nand_ecc_bits_check>
    af34:	e3500002 	cmp	r0, #2	; 0x2
    af38:	0a000049 	beq	b064 <flash_nand_vs_read+0x424>
    af3c:	e59f3150 	ldr	r3, [pc, #336]	; b094 <flash_nand_vs_read+0x454>
    af40:	e2855001 	add	r5, r5, #1	; 0x1
    af44:	e5932000 	ldr	r2, [r3]
    af48:	e59d303c 	ldr	r3, [sp, #60]
    af4c:	e58d2030 	str	r2, [sp, #48]
    af50:	e0833002 	add	r3, r3, r2
    af54:	e58d303c 	str	r3, [sp, #60]
    af58:	e58d202c 	str	r2, [sp, #44]
    af5c:	e155000a 	cmp	r5, sl
    af60:	3affffb5 	bcc	ae3c <flash_nand_vs_read+0x1fc>
    af64:	e1a0100b 	mov	r1, fp
    af68:	e1a00006 	mov	r0, r6
    af6c:	eb00156f 	bl	10530 <__aeabi_idivmod>
    af70:	e2518000 	subs	r8, r1, #0	; 0x0
    af74:	0a00003f 	beq	b078 <flash_nand_vs_read+0x438>
    af78:	e59f3104 	ldr	r3, [pc, #260]	; b084 <flash_nand_vs_read+0x444>
    af7c:	e59d203c 	ldr	r2, [sp, #60]
    af80:	e593300c 	ldr	r3, [r3, #12]
    af84:	e3a00000 	mov	r0, #0	; 0x0
    af88:	e3530001 	cmp	r3, #1	; 0x1
    af8c:	e1a03422 	lsr	r3, r2, #8
    af90:	e1a01822 	lsr	r1, r2, #16
    af94:	e5cd3021 	strb	r3, [sp, #33]
    af98:	13a03004 	movne	r3, #4	; 0x4
    af9c:	e5cd1020 	strb	r1, [sp, #32]
    afa0:	e5cd2022 	strb	r2, [sp, #34]
    afa4:	e5cd0023 	strb	r0, [sp, #35]
    afa8:	15cd3024 	strbne	r3, [sp, #36]
    afac:	15cd0026 	strbne	r0, [sp, #38]
    afb0:	1a000015 	bne	b00c <flash_nand_vs_read+0x3cc>
    afb4:	e1a04000 	mov	r4, r0
    afb8:	ebfffeed 	bl	ab74 <nand_ecc_enable>
    afbc:	e3500000 	cmp	r0, #0	; 0x0
    afc0:	e2844001 	add	r4, r4, #1	; 0x1
    afc4:	0a000001 	beq	afd0 <flash_nand_vs_read+0x390>
    afc8:	e354000a 	cmp	r4, #10	; 0xa
    afcc:	1afffff9 	bne	afb8 <flash_nand_vs_read+0x378>
    afd0:	e59f20ac 	ldr	r2, [pc, #172]	; b084 <flash_nand_vs_read+0x444>
    afd4:	e3a03000 	mov	r3, #0	; 0x0
    afd8:	e592203c 	ldr	r2, [r2, #60]
    afdc:	e5cd3020 	strb	r3, [sp, #32]
    afe0:	e1520003 	cmp	r2, r3
    afe4:	e5cd3021 	strb	r3, [sp, #33]
    afe8:	12833008 	addne	r3, r3, #8	; 0x8
    afec:	15cd3020 	strbne	r3, [sp, #32]
    aff0:	e3a03002 	mov	r3, #2	; 0x2
    aff4:	e5cd3024 	strb	r3, [sp, #36]
    aff8:	e0850007 	add	r0, r5, r7
    affc:	e3a03001 	mov	r3, #1	; 0x1
    b000:	e3a0100f 	mov	r1, #15	; 0xf
    b004:	e5cd3026 	strb	r3, [sp, #38]
    b008:	ebfffe9b 	bl	aa7c <nand_read_page_to_cache>
    b00c:	e59f3074 	ldr	r3, [pc, #116]	; b088 <flash_nand_vs_read+0x448>
    b010:	e5933000 	ldr	r3, [r3]
    b014:	e3530000 	cmp	r3, #0	; 0x0
    b018:	0a000006 	beq	b038 <flash_nand_vs_read+0x3f8>
    b01c:	e59f0098 	ldr	r0, [pc, #152]	; b0bc <flash_nand_vs_read+0x47c>
    b020:	e59d102c 	ldr	r1, [sp, #44]
    b024:	e59d203c 	ldr	r2, [sp, #60]
    b028:	e1dd32d7 	ldrsb	r3, [sp, #39]
    b02c:	e59fc05c 	ldr	ip, [pc, #92]	; b090 <flash_nand_vs_read+0x450>
    b030:	e1a0e00f 	mov	lr, pc
    b034:	e59cf000 	ldr	pc, [ip]
    b038:	e28d0014 	add	r0, sp, #20	; 0x14
    b03c:	e58d8030 	str	r8, [sp, #48]
    b040:	e58d802c 	str	r8, [sp, #44]
    b044:	ebfffc2c 	bl	a0fc <spi_read_write_data>
    b048:	e59f3034 	ldr	r3, [pc, #52]	; b084 <flash_nand_vs_read+0x444>
    b04c:	e593300c 	ldr	r3, [r3, #12]
    b050:	e3530001 	cmp	r3, #1	; 0x1
    b054:	1a000007 	bne	b078 <flash_nand_vs_read+0x438>
    b058:	ebfffebc 	bl	ab50 <nand_ecc_bits_check>
    b05c:	e3500002 	cmp	r0, #2	; 0x2
    b060:	1a000004 	bne	b078 <flash_nand_vs_read+0x438>
    b064:	e59f0054 	ldr	r0, [pc, #84]	; b0c0 <flash_nand_vs_read+0x480>
    b068:	e59f3020 	ldr	r3, [pc, #32]	; b090 <flash_nand_vs_read+0x450>
    b06c:	e1a0e00f 	mov	lr, pc
    b070:	e593f000 	ldr	pc, [r3]
    b074:	e3e06000 	mvn	r6, #0	; 0x0
    b078:	e1a00006 	mov	r0, r6
    b07c:	e28dd074 	add	sp, sp, #116	; 0x74
    b080:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b084:	0800d258 	.word	0x0800d258
    b088:	0800d2b8 	.word	0x0800d2b8
    b08c:	00013a45 	.word	0x00013a45
    b090:	0800d76c 	.word	0x0800d76c
    b094:	0800d134 	.word	0x0800d134
    b098:	00014009 	.word	0x00014009
    b09c:	00013a73 	.word	0x00013a73
    b0a0:	00013a77 	.word	0x00013a77
    b0a4:	00011842 	.word	0x00011842
    b0a8:	000134f4 	.word	0x000134f4
    b0ac:	00013ac0 	.word	0x00013ac0
    b0b0:	00013ad4 	.word	0x00013ad4
    b0b4:	00013aef 	.word	0x00013aef
    b0b8:	00013b1f 	.word	0x00013b1f
    b0bc:	00013b67 	.word	0x00013b67
    b0c0:	00013b49 	.word	0x00013b49

0000b0c4 <flash_nand_vs_write>:
    b0c4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b0c8:	e59f444c 	ldr	r4, [pc, #1100]	; b51c <flash_nand_vs_write+0x458>
    b0cc:	e24dd074 	sub	sp, sp, #116	; 0x74
    b0d0:	e594305c 	ldr	r3, [r4, #92]
    b0d4:	e1a09000 	mov	r9, r0
    b0d8:	e3530001 	cmp	r3, #1	; 0x1
    b0dc:	e1a05002 	mov	r5, r2
    b0e0:	e59d8098 	ldr	r8, [sp, #152]
    b0e4:	0a000004 	beq	b0fc <flash_nand_vs_write+0x38>
    b0e8:	ebfffaa7 	bl	9b8c <ssp_manager_init>
    b0ec:	e3a03064 	mov	r3, #100	; 0x64
    b0f0:	e5843050 	str	r3, [r4, #80]
    b0f4:	e3a0300f 	mov	r3, #15	; 0xf
    b0f8:	e5843000 	str	r3, [r4]
    b0fc:	e59f4418 	ldr	r4, [pc, #1048]	; b51c <flash_nand_vs_write+0x458>
    b100:	e1a00005 	mov	r0, r5
    b104:	e5941028 	ldr	r1, [r4, #40]
    b108:	eb001480 	bl	10310 <__aeabi_idiv>
    b10c:	e59f340c 	ldr	r3, [pc, #1036]	; b520 <flash_nand_vs_write+0x45c>
    b110:	e5942044 	ldr	r2, [r4, #68]
    b114:	e5941030 	ldr	r1, [r4, #48]
    b118:	e1a0b000 	mov	fp, r0
    b11c:	e5832000 	str	r2, [r3]
    b120:	e28d0014 	add	r0, sp, #20	; 0x14
    b124:	e58d100c 	str	r1, [sp, #12]
    b128:	ebfffbbf 	bl	a02c <spi_transfer_init>
    b12c:	e59f33f0 	ldr	r3, [pc, #1008]	; b524 <flash_nand_vs_write+0x460>
    b130:	e1a0600b 	mov	r6, fp
    b134:	e1a07fc6 	asr	r7, r6, #31
    b138:	e5933000 	ldr	r3, [r3]
    b13c:	e58d603c 	str	r6, [sp, #60]
    b140:	e3530000 	cmp	r3, #0	; 0x0
    b144:	e5945000 	ldr	r5, [r4]
    b148:	e1a0a006 	mov	sl, r6
    b14c:	0a000003 	beq	b160 <flash_nand_vs_write+0x9c>
    b150:	e59f03d0 	ldr	r0, [pc, #976]	; b528 <flash_nand_vs_write+0x464>
    b154:	e59f33d0 	ldr	r3, [pc, #976]	; b52c <flash_nand_vs_write+0x468>
    b158:	e1a0e00f 	mov	lr, pc
    b15c:	e593f000 	ldr	pc, [r3]
    b160:	e59f33bc 	ldr	r3, [pc, #956]	; b524 <flash_nand_vs_write+0x460>
    b164:	e5933000 	ldr	r3, [r3]
    b168:	e3530000 	cmp	r3, #0	; 0x0
    b16c:	0a000007 	beq	b190 <flash_nand_vs_write+0xcc>
    b170:	e58d8000 	str	r8, [sp]
    b174:	e1a02006 	mov	r2, r6
    b178:	e1a03007 	mov	r3, r7
    b17c:	e59f03ac 	ldr	r0, [pc, #940]	; b530 <flash_nand_vs_write+0x46c>
    b180:	e59f13ac 	ldr	r1, [pc, #940]	; b534 <flash_nand_vs_write+0x470>
    b184:	e59fc3a0 	ldr	ip, [pc, #928]	; b52c <flash_nand_vs_write+0x468>
    b188:	e1a0e00f 	mov	lr, pc
    b18c:	e59cf000 	ldr	pc, [ip]
    b190:	e59f338c 	ldr	r3, [pc, #908]	; b524 <flash_nand_vs_write+0x460>
    b194:	e5933000 	ldr	r3, [r3]
    b198:	e3530000 	cmp	r3, #0	; 0x0
    b19c:	0a000006 	beq	b1bc <flash_nand_vs_write+0xf8>
    b1a0:	e59f3374 	ldr	r3, [pc, #884]	; b51c <flash_nand_vs_write+0x458>
    b1a4:	e59f038c 	ldr	r0, [pc, #908]	; b538 <flash_nand_vs_write+0x474>
    b1a8:	e593104c 	ldr	r1, [r3, #76]
    b1ac:	e1a02005 	mov	r2, r5
    b1b0:	e59f3374 	ldr	r3, [pc, #884]	; b52c <flash_nand_vs_write+0x468>
    b1b4:	e1a0e00f 	mov	lr, pc
    b1b8:	e593f000 	ldr	pc, [r3]
    b1bc:	e355000f 	cmp	r5, #15	; 0xf
    b1c0:	1a00001d 	bne	b23c <flash_nand_vs_write+0x178>
    b1c4:	e59f3354 	ldr	r3, [pc, #852]	; b520 <flash_nand_vs_write+0x45c>
    b1c8:	e3a04000 	mov	r4, #0	; 0x0
    b1cc:	e5936000 	ldr	r6, [r3]
    b1d0:	e3a03002 	mov	r3, #2	; 0x2
    b1d4:	e3a02003 	mov	r2, #3	; 0x3
    b1d8:	e5cd3014 	strb	r3, [sp, #20]
    b1dc:	e1a00008 	mov	r0, r8
    b1e0:	e3a03001 	mov	r3, #1	; 0x1
    b1e4:	e1a01006 	mov	r1, r6
    b1e8:	e5cd3018 	strb	r3, [sp, #24]
    b1ec:	e5cd2024 	strb	r2, [sp, #36]
    b1f0:	e58d201c 	str	r2, [sp, #28]
    b1f4:	e5cd5027 	strb	r5, [sp, #39]
    b1f8:	e5cd4026 	strb	r4, [sp, #38]
    b1fc:	eb001443 	bl	10310 <__aeabi_idiv>
    b200:	e59f331c 	ldr	r3, [pc, #796]	; b524 <flash_nand_vs_write+0x460>
    b204:	e1580006 	cmp	r8, r6
    b208:	b1a02008 	movlt	r2, r8
    b20c:	a1a02006 	movge	r2, r6
    b210:	e5933000 	ldr	r3, [r3]
    b214:	e58d0008 	str	r0, [sp, #8]
    b218:	e1530004 	cmp	r3, r4
    b21c:	e58da03c 	str	sl, [sp, #60]
    b220:	e58d9034 	str	r9, [sp, #52]
    b224:	e58d202c 	str	r2, [sp, #44]
    b228:	e58d4040 	str	r4, [sp, #64]
    b22c:	e58d4028 	str	r4, [sp, #40]
    b230:	e58d2038 	str	r2, [sp, #56]
    b234:	0a00000b 	beq	b268 <flash_nand_vs_write+0x1a4>
    b238:	ea000005 	b	b254 <flash_nand_vs_write+0x190>
    b23c:	e59f02f8 	ldr	r0, [pc, #760]	; b53c <flash_nand_vs_write+0x478>
    b240:	e59f32e4 	ldr	r3, [pc, #740]	; b52c <flash_nand_vs_write+0x468>
    b244:	e1a0e00f 	mov	lr, pc
    b248:	e593f000 	ldr	pc, [r3]
    b24c:	e3e08000 	mvn	r8, #0	; 0x0
    b250:	ea0000ae 	b	b510 <flash_nand_vs_write+0x44c>
    b254:	e59f02e4 	ldr	r0, [pc, #740]	; b540 <flash_nand_vs_write+0x47c>
    b258:	e59d1008 	ldr	r1, [sp, #8]
    b25c:	e59f32c8 	ldr	r3, [pc, #712]	; b52c <flash_nand_vs_write+0x468>
    b260:	e1a0e00f 	mov	lr, pc
    b264:	e593f000 	ldr	pc, [r3]
    b268:	e3a05000 	mov	r5, #0	; 0x0
    b26c:	e59f92b0 	ldr	r9, [pc, #688]	; b524 <flash_nand_vs_write+0x460>
    b270:	e59fa2b4 	ldr	sl, [pc, #692]	; b52c <flash_nand_vs_write+0x468>
    b274:	e1a07005 	mov	r7, r5
    b278:	ea000049 	b	b3a4 <flash_nand_vs_write+0x2e0>
    b27c:	e5993000 	ldr	r3, [r9]
    b280:	e3530000 	cmp	r3, #0	; 0x0
    b284:	0a000003 	beq	b298 <flash_nand_vs_write+0x1d4>
    b288:	e59f02b4 	ldr	r0, [pc, #692]	; b544 <flash_nand_vs_write+0x480>
    b28c:	e1a01005 	mov	r1, r5
    b290:	e1a0e00f 	mov	lr, pc
    b294:	e59af000 	ldr	pc, [sl]
    b298:	ebfffcc8 	bl	a5c0 <flash_write_enable>
    b29c:	e3a01007 	mov	r1, #7	; 0x7
    b2a0:	e3a0000f 	mov	r0, #15	; 0xf
    b2a4:	ebfffcec 	bl	a65c <flash_wait_device_status>
    b2a8:	e3a01006 	mov	r1, #6	; 0x6
    b2ac:	e3a0000f 	mov	r0, #15	; 0xf
    b2b0:	ebfffce9 	bl	a65c <flash_wait_device_status>
    b2b4:	e59f1260 	ldr	r1, [pc, #608]	; b51c <flash_nand_vs_write+0x458>
    b2b8:	e59d303c 	ldr	r3, [sp, #60]
    b2bc:	e591200c 	ldr	r2, [r1, #12]
    b2c0:	e1a01823 	lsr	r1, r3, #16
    b2c4:	e3520001 	cmp	r2, #1	; 0x1
    b2c8:	e1a02423 	lsr	r2, r3, #8
    b2cc:	e5cd2021 	strb	r2, [sp, #33]
    b2d0:	e3a02003 	mov	r2, #3	; 0x3
    b2d4:	e5cd1020 	strb	r1, [sp, #32]
    b2d8:	e5cd3022 	strb	r3, [sp, #34]
    b2dc:	e5cd7023 	strb	r7, [sp, #35]
    b2e0:	e5cd2024 	strb	r2, [sp, #36]
    b2e4:	1a000017 	bne	b348 <flash_nand_vs_write+0x284>
    b2e8:	e5993000 	ldr	r3, [r9]
    b2ec:	e3530000 	cmp	r3, #0	; 0x0
    b2f0:	0a000002 	beq	b300 <flash_nand_vs_write+0x23c>
    b2f4:	e59f024c 	ldr	r0, [pc, #588]	; b548 <flash_nand_vs_write+0x484>
    b2f8:	e1a0e00f 	mov	lr, pc
    b2fc:	e59af000 	ldr	pc, [sl]
    b300:	e3a04000 	mov	r4, #0	; 0x0
    b304:	ebfffe1a 	bl	ab74 <nand_ecc_enable>
    b308:	e3500000 	cmp	r0, #0	; 0x0
    b30c:	e2844001 	add	r4, r4, #1	; 0x1
    b310:	0a000001 	beq	b31c <flash_nand_vs_write+0x258>
    b314:	e354000a 	cmp	r4, #10	; 0xa
    b318:	1afffff9 	bne	b304 <flash_nand_vs_write+0x240>
    b31c:	e5993000 	ldr	r3, [r9]
    b320:	e3530000 	cmp	r3, #0	; 0x0
    b324:	0a000002 	beq	b334 <flash_nand_vs_write+0x270>
    b328:	e59f021c 	ldr	r0, [pc, #540]	; b54c <flash_nand_vs_write+0x488>
    b32c:	e1a0e00f 	mov	lr, pc
    b330:	e59af000 	ldr	pc, [sl]
    b334:	e3a03002 	mov	r3, #2	; 0x2
    b338:	e5cd7020 	strb	r7, [sp, #32]
    b33c:	e5cd7021 	strb	r7, [sp, #33]
    b340:	e5cd3024 	strb	r3, [sp, #36]
    b344:	e5cd7026 	strb	r7, [sp, #38]
    b348:	e59f11d0 	ldr	r1, [pc, #464]	; b520 <flash_nand_vs_write+0x45c>
    b34c:	e28d0014 	add	r0, sp, #20	; 0x14
    b350:	e5913000 	ldr	r3, [r1]
    b354:	e58d302c 	str	r3, [sp, #44]
    b358:	e58d3038 	str	r3, [sp, #56]
    b35c:	ebfffb66 	bl	a0fc <spi_read_write_data>
    b360:	e59f31b8 	ldr	r3, [pc, #440]	; b520 <flash_nand_vs_write+0x45c>
    b364:	e5932000 	ldr	r2, [r3]
    b368:	e59f31ac 	ldr	r3, [pc, #428]	; b51c <flash_nand_vs_write+0x458>
    b36c:	e593100c 	ldr	r1, [r3, #12]
    b370:	e59d303c 	ldr	r3, [sp, #60]
    b374:	e3510001 	cmp	r1, #1	; 0x1
    b378:	e0833002 	add	r3, r3, r2
    b37c:	e58d303c 	str	r3, [sp, #60]
    b380:	1a000006 	bne	b3a0 <flash_nand_vs_write+0x2dc>
    b384:	e085400b 	add	r4, r5, fp
    b388:	e281100e 	add	r1, r1, #14	; 0xe
    b38c:	e1a00004 	mov	r0, r4
    b390:	ebfffd9c 	bl	aa08 <nand_write_cache_to_memory>
    b394:	ebfffdd3 	bl	aae8 <nand_blk_prg_era_fail>
    b398:	e3500001 	cmp	r0, #1	; 0x1
    b39c:	0a000051 	beq	b4e8 <flash_nand_vs_write+0x424>
    b3a0:	e2855001 	add	r5, r5, #1	; 0x1
    b3a4:	e59d1008 	ldr	r1, [sp, #8]
    b3a8:	e1550001 	cmp	r5, r1
    b3ac:	3affffb2 	bcc	b27c <flash_nand_vs_write+0x1b8>
    b3b0:	e1a01006 	mov	r1, r6
    b3b4:	e1a00008 	mov	r0, r8
    b3b8:	eb00145c 	bl	10530 <__aeabi_idivmod>
    b3bc:	e2516000 	subs	r6, r1, #0	; 0x0
    b3c0:	0a000052 	beq	b510 <flash_nand_vs_write+0x44c>
    b3c4:	e59f3158 	ldr	r3, [pc, #344]	; b524 <flash_nand_vs_write+0x460>
    b3c8:	e5933000 	ldr	r3, [r3]
    b3cc:	e3530000 	cmp	r3, #0	; 0x0
    b3d0:	0a000003 	beq	b3e4 <flash_nand_vs_write+0x320>
    b3d4:	e59f0174 	ldr	r0, [pc, #372]	; b550 <flash_nand_vs_write+0x48c>
    b3d8:	e59f314c 	ldr	r3, [pc, #332]	; b52c <flash_nand_vs_write+0x468>
    b3dc:	e1a0e00f 	mov	lr, pc
    b3e0:	e593f000 	ldr	pc, [r3]
    b3e4:	ebfffc75 	bl	a5c0 <flash_write_enable>
    b3e8:	e3a01007 	mov	r1, #7	; 0x7
    b3ec:	e3a0000f 	mov	r0, #15	; 0xf
    b3f0:	ebfffc99 	bl	a65c <flash_wait_device_status>
    b3f4:	e3a01006 	mov	r1, #6	; 0x6
    b3f8:	e3a0000f 	mov	r0, #15	; 0xf
    b3fc:	ebfffc96 	bl	a65c <flash_wait_device_status>
    b400:	e59f3114 	ldr	r3, [pc, #276]	; b51c <flash_nand_vs_write+0x458>
    b404:	e59d203c 	ldr	r2, [sp, #60]
    b408:	e593300c 	ldr	r3, [r3, #12]
    b40c:	e1a01822 	lsr	r1, r2, #16
    b410:	e3530001 	cmp	r3, #1	; 0x1
    b414:	e1a03422 	lsr	r3, r2, #8
    b418:	e5cd3021 	strb	r3, [sp, #33]
    b41c:	e3a03000 	mov	r3, #0	; 0x0
    b420:	e5cd3023 	strb	r3, [sp, #35]
    b424:	e2833003 	add	r3, r3, #3	; 0x3
    b428:	e5cd1020 	strb	r1, [sp, #32]
    b42c:	e5cd2022 	strb	r2, [sp, #34]
    b430:	e5cd3024 	strb	r3, [sp, #36]
    b434:	1a00001c 	bne	b4ac <flash_nand_vs_write+0x3e8>
    b438:	e59f30e4 	ldr	r3, [pc, #228]	; b524 <flash_nand_vs_write+0x460>
    b43c:	e5933000 	ldr	r3, [r3]
    b440:	e3530000 	cmp	r3, #0	; 0x0
    b444:	0a000003 	beq	b458 <flash_nand_vs_write+0x394>
    b448:	e59f00f8 	ldr	r0, [pc, #248]	; b548 <flash_nand_vs_write+0x484>
    b44c:	e59f30d8 	ldr	r3, [pc, #216]	; b52c <flash_nand_vs_write+0x468>
    b450:	e1a0e00f 	mov	lr, pc
    b454:	e593f000 	ldr	pc, [r3]
    b458:	e3a04000 	mov	r4, #0	; 0x0
    b45c:	ebfffdc4 	bl	ab74 <nand_ecc_enable>
    b460:	e3500000 	cmp	r0, #0	; 0x0
    b464:	e2844001 	add	r4, r4, #1	; 0x1
    b468:	0a000001 	beq	b474 <flash_nand_vs_write+0x3b0>
    b46c:	e354000a 	cmp	r4, #10	; 0xa
    b470:	1afffff9 	bne	b45c <flash_nand_vs_write+0x398>
    b474:	e59f30a8 	ldr	r3, [pc, #168]	; b524 <flash_nand_vs_write+0x460>
    b478:	e5933000 	ldr	r3, [r3]
    b47c:	e3530000 	cmp	r3, #0	; 0x0
    b480:	0a000003 	beq	b494 <flash_nand_vs_write+0x3d0>
    b484:	e59f00c0 	ldr	r0, [pc, #192]	; b54c <flash_nand_vs_write+0x488>
    b488:	e59f309c 	ldr	r3, [pc, #156]	; b52c <flash_nand_vs_write+0x468>
    b48c:	e1a0e00f 	mov	lr, pc
    b490:	e593f000 	ldr	pc, [r3]
    b494:	e3a02000 	mov	r2, #0	; 0x0
    b498:	e3a03002 	mov	r3, #2	; 0x2
    b49c:	e5cd3024 	strb	r3, [sp, #36]
    b4a0:	e5cd2026 	strb	r2, [sp, #38]
    b4a4:	e5cd2020 	strb	r2, [sp, #32]
    b4a8:	e5cd2021 	strb	r2, [sp, #33]
    b4ac:	e28d0014 	add	r0, sp, #20	; 0x14
    b4b0:	e58d602c 	str	r6, [sp, #44]
    b4b4:	e58d6038 	str	r6, [sp, #56]
    b4b8:	ebfffb0f 	bl	a0fc <spi_read_write_data>
    b4bc:	e59f3058 	ldr	r3, [pc, #88]	; b51c <flash_nand_vs_write+0x458>
    b4c0:	e593300c 	ldr	r3, [r3, #12]
    b4c4:	e3530001 	cmp	r3, #1	; 0x1
    b4c8:	1a000010 	bne	b510 <flash_nand_vs_write+0x44c>
    b4cc:	e085400b 	add	r4, r5, fp
    b4d0:	e3a0100f 	mov	r1, #15	; 0xf
    b4d4:	e1a00004 	mov	r0, r4
    b4d8:	ebfffd4a 	bl	aa08 <nand_write_cache_to_memory>
    b4dc:	ebfffd81 	bl	aae8 <nand_blk_prg_era_fail>
    b4e0:	e3500001 	cmp	r0, #1	; 0x1
    b4e4:	1a000009 	bne	b510 <flash_nand_vs_write+0x44c>
    b4e8:	e59d100c 	ldr	r1, [sp, #12]
    b4ec:	e1a00004 	mov	r0, r4
    b4f0:	eb001386 	bl	10310 <__aeabi_idiv>
    b4f4:	e1a02005 	mov	r2, r5
    b4f8:	e1a01000 	mov	r1, r0
    b4fc:	e59f3028 	ldr	r3, [pc, #40]	; b52c <flash_nand_vs_write+0x468>
    b500:	e59f004c 	ldr	r0, [pc, #76]	; b554 <flash_nand_vs_write+0x490>
    b504:	e1a0e00f 	mov	lr, pc
    b508:	e593f000 	ldr	pc, [r3]
    b50c:	e3e08002 	mvn	r8, #2	; 0x2
    b510:	e1a00008 	mov	r0, r8
    b514:	e28dd074 	add	sp, sp, #116	; 0x74
    b518:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b51c:	0800d258 	.word	0x0800d258
    b520:	0800d134 	.word	0x0800d134
    b524:	0800d2b8 	.word	0x0800d2b8
    b528:	0001247a 	.word	0x0001247a
    b52c:	0800d76c 	.word	0x0800d76c
    b530:	00013ba6 	.word	0x00013ba6
    b534:	0001182e 	.word	0x0001182e
    b538:	00013bd5 	.word	0x00013bd5
    b53c:	00013bf8 	.word	0x00013bf8
    b540:	00013c1b 	.word	0x00013c1b
    b544:	00013c37 	.word	0x00013c37
    b548:	00013c59 	.word	0x00013c59
    b54c:	00013c87 	.word	0x00013c87
    b550:	00013ce1 	.word	0x00013ce1
    b554:	00013cb4 	.word	0x00013cb4

0000b558 <flash_nand_vs_erase>:
    b558:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b55c:	e59f418c 	ldr	r4, [pc, #396]	; b6f0 <flash_nand_vs_erase+0x198>
    b560:	e24dd064 	sub	sp, sp, #100	; 0x64
    b564:	e594305c 	ldr	r3, [r4, #92]
    b568:	e1a06000 	mov	r6, r0
    b56c:	e3530001 	cmp	r3, #1	; 0x1
    b570:	e1a07001 	mov	r7, r1
    b574:	e1a05002 	mov	r5, r2
    b578:	0a000004 	beq	b590 <flash_nand_vs_erase+0x38>
    b57c:	ebfff982 	bl	9b8c <ssp_manager_init>
    b580:	e3a03064 	mov	r3, #100	; 0x64
    b584:	e5843050 	str	r3, [r4, #80]
    b588:	e3a0300f 	mov	r3, #15	; 0xf
    b58c:	e5843000 	str	r3, [r4]
    b590:	e59f8158 	ldr	r8, [pc, #344]	; b6f0 <flash_nand_vs_erase+0x198>
    b594:	e5984000 	ldr	r4, [r8]
    b598:	e354000f 	cmp	r4, #15	; 0xf
    b59c:	0a000005 	beq	b5b8 <flash_nand_vs_erase+0x60>
    b5a0:	e59f014c 	ldr	r0, [pc, #332]	; b6f4 <flash_nand_vs_erase+0x19c>
    b5a4:	e59f314c 	ldr	r3, [pc, #332]	; b6f8 <flash_nand_vs_erase+0x1a0>
    b5a8:	e1a0e00f 	mov	lr, pc
    b5ac:	e593f000 	ldr	pc, [r3]
    b5b0:	e3e00000 	mvn	r0, #0	; 0x0
    b5b4:	ea00004b 	b	b6e8 <flash_nand_vs_erase+0x190>
    b5b8:	e59f213c 	ldr	r2, [pc, #316]	; b6fc <flash_nand_vs_erase+0x1a4>
    b5bc:	e3a03000 	mov	r3, #0	; 0x0
    b5c0:	e0922006 	adds	r2, r2, r6
    b5c4:	e0a33007 	adc	r3, r3, r7
    b5c8:	e0922005 	adds	r2, r2, r5
    b5cc:	e2a33000 	adc	r3, r3, #0	; 0x0
    b5d0:	e1a018a2 	lsr	r1, r2, #17
    b5d4:	e181a783 	orr	sl, r1, r3, lsl #15
    b5d8:	ebfffbf8 	bl	a5c0 <flash_write_enable>
    b5dc:	e1a00004 	mov	r0, r4
    b5e0:	e3a01007 	mov	r1, #7	; 0x7
    b5e4:	ebfffc1c 	bl	a65c <flash_wait_device_status>
    b5e8:	e3a01006 	mov	r1, #6	; 0x6
    b5ec:	e1a00004 	mov	r0, r4
    b5f0:	ebfffc19 	bl	a65c <flash_wait_device_status>
    b5f4:	e28d0004 	add	r0, sp, #4	; 0x4
    b5f8:	ebfffa8b 	bl	a02c <spi_transfer_init>
    b5fc:	e5983000 	ldr	r3, [r8]
    b600:	e3a04000 	mov	r4, #0	; 0x0
    b604:	e353000f 	cmp	r3, #15	; 0xf
    b608:	e1a058a6 	lsr	r5, r6, #17
    b60c:	05cd3017 	strbeq	r3, [sp, #23]
    b610:	e1855787 	orr	r5, r5, r7, lsl #15
    b614:	e3e03027 	mvn	r3, #39	; 0x27
    b618:	e3a0001f 	mov	r0, #31	; 0x1f
    b61c:	e3a010a0 	mov	r1, #160	; 0xa0
    b620:	e1a02004 	mov	r2, r4
    b624:	e5cd3004 	strb	r3, [sp, #4]
    b628:	e58d400c 	str	r4, [sp, #12]
    b62c:	e5cd4005 	strb	r4, [sp, #5]
    b630:	e1a06005 	mov	r6, r5
    b634:	ebfffbc8 	bl	a55c <flash_set_device_status>
    b638:	e59fb0b0 	ldr	fp, [pc, #176]	; b6f0 <flash_nand_vs_erase+0x198>
    b63c:	e28d7004 	add	r7, sp, #4	; 0x4
    b640:	e3a09004 	mov	r9, #4	; 0x4
    b644:	e3a08019 	mov	r8, #25	; 0x19
    b648:	ea00001d 	b	b6c4 <flash_nand_vs_erase+0x16c>
    b64c:	ebfffbdb 	bl	a5c0 <flash_write_enable>
    b650:	e3a01007 	mov	r1, #7	; 0x7
    b654:	e3a0000f 	mov	r0, #15	; 0xf
    b658:	ebfffbff 	bl	a65c <flash_wait_device_status>
    b65c:	e3a01006 	mov	r1, #6	; 0x6
    b660:	e3a0000f 	mov	r0, #15	; 0xf
    b664:	ebfffbfc 	bl	a65c <flash_wait_device_status>
    b668:	e59b3030 	ldr	r3, [fp, #48]
    b66c:	e1a00007 	mov	r0, r7
    b670:	e0030693 	mul	r3, r3, r6
    b674:	e1a02403 	lsl	r2, r3, #8
    b678:	e2033cff 	and	r3, r3, #65280	; 0xff00
    b67c:	e1822423 	orr	r2, r2, r3, lsr #8
    b680:	e1c720b2 	strh	r2, [r7, #2]
    b684:	e2866001 	add	r6, r6, #1	; 0x1
    b688:	e5cd9008 	strb	r9, [sp, #8]
    b68c:	ebfffa9a 	bl	a0fc <spi_read_write_data>
    b690:	e3a01006 	mov	r1, #6	; 0x6
    b694:	e3a0000f 	mov	r0, #15	; 0xf
    b698:	ebfffbef 	bl	a65c <flash_wait_device_status>
    b69c:	ebfffd11 	bl	aae8 <nand_blk_prg_era_fail>
    b6a0:	e3560000 	cmp	r6, #0	; 0x0
    b6a4:	e28610ff 	add	r1, r6, #255	; 0xff
    b6a8:	a1a01006 	movge	r1, r6
    b6ac:	e1a01441 	asr	r1, r1, #8
    b6b0:	e0010198 	mul	r1, r8, r1
    b6b4:	e59f0044 	ldr	r0, [pc, #68]	; b700 <flash_nand_vs_erase+0x1a8>
    b6b8:	e59f3038 	ldr	r3, [pc, #56]	; b6f8 <flash_nand_vs_erase+0x1a0>
    b6bc:	e1a0e00f 	mov	lr, pc
    b6c0:	e593f000 	ldr	pc, [r3]
    b6c4:	e0843005 	add	r3, r4, r5
    b6c8:	e153000a 	cmp	r3, sl
    b6cc:	e2844001 	add	r4, r4, #1	; 0x1
    b6d0:	baffffdd 	blt	b64c <flash_nand_vs_erase+0xf4>
    b6d4:	e59f0028 	ldr	r0, [pc, #40]	; b704 <flash_nand_vs_erase+0x1ac>
    b6d8:	e59f3018 	ldr	r3, [pc, #24]	; b6f8 <flash_nand_vs_erase+0x1a0>
    b6dc:	e1a0e00f 	mov	lr, pc
    b6e0:	e593f000 	ldr	pc, [r3]
    b6e4:	e3a00000 	mov	r0, #0	; 0x0
    b6e8:	e28dd064 	add	sp, sp, #100	; 0x64
    b6ec:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b6f0:	0800d258 	.word	0x0800d258
    b6f4:	00013cf5 	.word	0x00013cf5
    b6f8:	0800d76c 	.word	0x0800d76c
    b6fc:	0001ffff 	.word	0x0001ffff
    b700:	00013d1d 	.word	0x00013d1d
    b704:	00013d31 	.word	0x00013d31

0000b708 <spi_module_init>:
    b708:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    b70c:	e2504000 	subs	r4, r0, #0	; 0x0
    b710:	e24dd00c 	sub	sp, sp, #12	; 0xc
    b714:	1a000005 	bne	b730 <spi_module_init+0x28>
    b718:	e59f0168 	ldr	r0, [pc, #360]	; b888 <spi_module_init+0x180>
    b71c:	e59f3168 	ldr	r3, [pc, #360]	; b88c <spi_module_init+0x184>
    b720:	e1a0e00f 	mov	lr, pc
    b724:	e593f000 	ldr	pc, [r3]
    b728:	e3e00001 	mvn	r0, #1	; 0x1
    b72c:	ea000053 	b	b880 <spi_module_init+0x178>
    b730:	e59f0158 	ldr	r0, [pc, #344]	; b890 <spi_module_init+0x188>
    b734:	ebffd629 	bl	fe0 <module_get_clock>
    b738:	e2508000 	subs	r8, r0, #0	; 0x0
    b73c:	a3a07002 	movge	r7, #2	; 0x2
    b740:	a59fa14c 	ldrge	sl, [pc, #332]	; b894 <spi_module_init+0x18c>
    b744:	aa000013 	bge	b798 <spi_module_init+0x90>
    b748:	e59f0148 	ldr	r0, [pc, #328]	; b898 <spi_module_init+0x190>
    b74c:	e1a01004 	mov	r1, r4
    b750:	e59f3134 	ldr	r3, [pc, #308]	; b88c <spi_module_init+0x184>
    b754:	e1a0e00f 	mov	lr, pc
    b758:	e593f000 	ldr	pc, [r3]
    b75c:	e3e00000 	mvn	r0, #0	; 0x0
    b760:	ea000046 	b	b880 <spi_module_init+0x178>
    b764:	e1a05004 	mov	r5, r4
    b768:	e1a01006 	mov	r1, r6
    b76c:	e1a00008 	mov	r0, r8
    b770:	eb0012e6 	bl	10310 <__aeabi_idiv>
    b774:	e150000a 	cmp	r0, sl
    b778:	e0866007 	add	r6, r6, r7
    b77c:	e2854001 	add	r4, r5, #1	; 0x1
    b780:	da000007 	ble	b7a4 <spi_module_init+0x9c>
    b784:	e3540c01 	cmp	r4, #256	; 0x100
    b788:	1afffff5 	bne	b764 <spi_module_init+0x5c>
    b78c:	e2877002 	add	r7, r7, #2	; 0x2
    b790:	e3570c01 	cmp	r7, #256	; 0x100
    b794:	0a000003 	beq	b7a8 <spi_module_init+0xa0>
    b798:	e1a06007 	mov	r6, r7
    b79c:	e3a05000 	mov	r5, #0	; 0x0
    b7a0:	eafffff0 	b	b768 <spi_module_init+0x60>
    b7a4:	e1a04005 	mov	r4, r5
    b7a8:	e35700fe 	cmp	r7, #254	; 0xfe
    b7ac:	c35400ff 	cmpgt	r4, #255	; 0xff
    b7b0:	da00000a 	ble	b7e0 <spi_module_init+0xd8>
    b7b4:	e3a01002 	mov	r1, #2	; 0x2
    b7b8:	e3a02027 	mov	r2, #39	; 0x27
    b7bc:	e59f00d8 	ldr	r0, [pc, #216]	; b89c <spi_module_init+0x194>
    b7c0:	e59f30c4 	ldr	r3, [pc, #196]	; b88c <spi_module_init+0x184>
    b7c4:	e1a0e00f 	mov	lr, pc
    b7c8:	e593f000 	ldr	pc, [r3]
    b7cc:	e1a00008 	mov	r0, r8
    b7d0:	e3a01050 	mov	r1, #80	; 0x50
    b7d4:	eb0012cd 	bl	10310 <__aeabi_idiv>
    b7d8:	e3a07002 	mov	r7, #2	; 0x2
    b7dc:	e3a04027 	mov	r4, #39	; 0x27
    b7e0:	e59f50a4 	ldr	r5, [pc, #164]	; b88c <spi_module_init+0x184>
    b7e4:	e1a02007 	mov	r2, r7
    b7e8:	e1a03004 	mov	r3, r4
    b7ec:	e58d0000 	str	r0, [sp]
    b7f0:	e1a01008 	mov	r1, r8
    b7f4:	e59f00a4 	ldr	r0, [pc, #164]	; b8a0 <spi_module_init+0x198>
    b7f8:	e1a0e00f 	mov	lr, pc
    b7fc:	e595f000 	ldr	pc, [r5]
    b800:	e1a03404 	lsl	r3, r4, #8
    b804:	e59f4098 	ldr	r4, [pc, #152]	; b8a4 <spi_module_init+0x19c>
    b808:	e3a06000 	mov	r6, #0	; 0x0
    b80c:	e38330c7 	orr	r3, r3, #199	; 0xc7
    b810:	e5847010 	str	r7, [r4, #16]
    b814:	e5846014 	str	r6, [r4, #20]
    b818:	e5846000 	str	r6, [r4]
    b81c:	e5846004 	str	r6, [r4, #4]
    b820:	e5843000 	str	r3, [r4]
    b824:	e3a0300a 	mov	r3, #10	; 0xa
    b828:	e5843004 	str	r3, [r4, #4]
    b82c:	e5941000 	ldr	r1, [r4]
    b830:	e59f0070 	ldr	r0, [pc, #112]	; b8a8 <spi_module_init+0x1a0>
    b834:	e1a0e00f 	mov	lr, pc
    b838:	e595f000 	ldr	pc, [r5]
    b83c:	e5941004 	ldr	r1, [r4, #4]
    b840:	e59f0064 	ldr	r0, [pc, #100]	; b8ac <spi_module_init+0x1a4>
    b844:	e1a0e00f 	mov	lr, pc
    b848:	e595f000 	ldr	pc, [r5]
    b84c:	e59f005c 	ldr	r0, [pc, #92]	; b8b0 <spi_module_init+0x1a8>
    b850:	e1a0e00f 	mov	lr, pc
    b854:	e595f000 	ldr	pc, [r5]
    b858:	e59f0054 	ldr	r0, [pc, #84]	; b8b4 <spi_module_init+0x1ac>
    b85c:	e1a0e00f 	mov	lr, pc
    b860:	e595f000 	ldr	pc, [r5]
    b864:	e59f004c 	ldr	r0, [pc, #76]	; b8b8 <spi_module_init+0x1b0>
    b868:	e1a0e00f 	mov	lr, pc
    b86c:	e595f000 	ldr	pc, [r5]
    b870:	e59f0044 	ldr	r0, [pc, #68]	; b8bc <spi_module_init+0x1b4>
    b874:	e1a0e00f 	mov	lr, pc
    b878:	e595f000 	ldr	pc, [r5]
    b87c:	e1a00006 	mov	r0, r6
    b880:	e28dd00c 	add	sp, sp, #12	; 0xc
    b884:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    b888:	00013d39 	.word	0x00013d39
    b88c:	0800d76c 	.word	0x0800d76c
    b890:	2d00a2a0 	.word	0x2d00a2a0
    b894:	004c4b40 	.word	0x004c4b40
    b898:	00013d4f 	.word	0x00013d4f
    b89c:	00013d83 	.word	0x00013d83
    b8a0:	00013ddd 	.word	0x00013ddd
    b8a4:	20500000 	.word	0x20500000
    b8a8:	00013e1e 	.word	0x00013e1e
    b8ac:	00013e42 	.word	0x00013e42
    b8b0:	00013e66 	.word	0x00013e66
    b8b4:	00013e8b 	.word	0x00013e8b
    b8b8:	00013eab 	.word	0x00013eab
    b8bc:	00013ed9 	.word	0x00013ed9

0000b8c0 <flash_nand_vs_reset>:
    b8c0:	e92d4030 	push	{r4, r5, lr}
    b8c4:	e59f4120 	ldr	r4, [pc, #288]	; b9ec <flash_nand_vs_reset+0x12c>
    b8c8:	e24dd004 	sub	sp, sp, #4	; 0x4
    b8cc:	e59f511c 	ldr	r5, [pc, #284]	; b9f0 <flash_nand_vs_reset+0x130>
    b8d0:	e59f011c 	ldr	r0, [pc, #284]	; b9f4 <flash_nand_vs_reset+0x134>
    b8d4:	e1a0e00f 	mov	lr, pc
    b8d8:	e595f000 	ldr	pc, [r5]
    b8dc:	e594305c 	ldr	r3, [r4, #92]
    b8e0:	e3530001 	cmp	r3, #1	; 0x1
    b8e4:	0a000007 	beq	b908 <flash_nand_vs_reset+0x48>
    b8e8:	ebfff8a7 	bl	9b8c <ssp_manager_init>
    b8ec:	e3a03064 	mov	r3, #100	; 0x64
    b8f0:	e5843050 	str	r3, [r4, #80]
    b8f4:	e3a0300f 	mov	r3, #15	; 0xf
    b8f8:	e5843000 	str	r3, [r4]
    b8fc:	e59f00f4 	ldr	r0, [pc, #244]	; b9f8 <flash_nand_vs_reset+0x138>
    b900:	e1a0e00f 	mov	lr, pc
    b904:	e595f000 	ldr	pc, [r5]
    b908:	e59f30ec 	ldr	r3, [pc, #236]	; b9fc <flash_nand_vs_reset+0x13c>
    b90c:	e5933000 	ldr	r3, [r3]
    b910:	e3530000 	cmp	r3, #0	; 0x0
    b914:	0a000007 	beq	b938 <flash_nand_vs_reset+0x78>
    b918:	e59f20cc 	ldr	r2, [pc, #204]	; b9ec <flash_nand_vs_reset+0x12c>
    b91c:	e59f00dc 	ldr	r0, [pc, #220]	; ba00 <flash_nand_vs_reset+0x140>
    b920:	e5923050 	ldr	r3, [r2, #80]
    b924:	e5921000 	ldr	r1, [r2]
    b928:	e59fc0c0 	ldr	ip, [pc, #192]	; b9f0 <flash_nand_vs_reset+0x130>
    b92c:	e5922004 	ldr	r2, [r2, #4]
    b930:	e1a0e00f 	mov	lr, pc
    b934:	e59cf000 	ldr	pc, [ip]
    b938:	e59f30ac 	ldr	r3, [pc, #172]	; b9ec <flash_nand_vs_reset+0x12c>
    b93c:	e5933000 	ldr	r3, [r3]
    b940:	e353000f 	cmp	r3, #15	; 0xf
    b944:	1a000020 	bne	b9cc <flash_nand_vs_reset+0x10c>
    b948:	e59f00b4 	ldr	r0, [pc, #180]	; ba04 <flash_nand_vs_reset+0x144>
    b94c:	ebffd373 	bl	720 <module_enable>
    b950:	e59f20b0 	ldr	r2, [pc, #176]	; ba08 <flash_nand_vs_reset+0x148>
    b954:	e59f00b0 	ldr	r0, [pc, #176]	; ba0c <flash_nand_vs_reset+0x14c>
    b958:	e5923404 	ldr	r3, [r2, #1028]
    b95c:	e3833008 	orr	r3, r3, #8	; 0x8
    b960:	e5823404 	str	r3, [r2, #1028]
    b964:	ebffff67 	bl	b708 <spi_module_init>
    b968:	e59f20a0 	ldr	r2, [pc, #160]	; ba10 <flash_nand_vs_reset+0x150>
    b96c:	e3a0000b 	mov	r0, #11	; 0xb
    b970:	e5923000 	ldr	r3, [r2]
    b974:	e3833004 	orr	r3, r3, #4	; 0x4
    b978:	e5823000 	str	r3, [r2]
    b97c:	ebffd3ae 	bl	83c <pads_init_module>
    b980:	e3a02001 	mov	r2, #1	; 0x1
    b984:	e3a0004a 	mov	r0, #74	; 0x4a
    b988:	e3a01002 	mov	r1, #2	; 0x2
    b98c:	ebffd47c 	bl	b84 <pads_chmod>
    b990:	e59f207c 	ldr	r2, [pc, #124]	; ba14 <flash_nand_vs_reset+0x154>
    b994:	ea000000 	b	b99c <flash_nand_vs_reset+0xdc>
    b998:	e5923008 	ldr	r3, [r2, #8]
    b99c:	e592300c 	ldr	r3, [r2, #12]
    b9a0:	e3130004 	tst	r3, #4	; 0x4
    b9a4:	1afffffb 	bne	b998 <flash_nand_vs_reset+0xd8>
    b9a8:	e3a00002 	mov	r0, #2	; 0x2
    b9ac:	ebfff8d6 	bl	9d0c <spi_set_intr_recv_timeout>
    b9b0:	e3a00008 	mov	r0, #8	; 0x8
    b9b4:	ebfff940 	bl	9ebc <spi_set_intr_transmit_fifo>
    b9b8:	e3a00004 	mov	r0, #4	; 0x4
    b9bc:	ebfff908 	bl	9de4 <spi_set_intr_recv_fifo>
    b9c0:	e3a00001 	mov	r0, #1	; 0x1
    b9c4:	ebfff89a 	bl	9c34 <spi_set_intr_recv_overrun>
    b9c8:	ea000003 	b	b9dc <flash_nand_vs_reset+0x11c>
    b9cc:	e59f0044 	ldr	r0, [pc, #68]	; ba18 <flash_nand_vs_reset+0x158>
    b9d0:	e59f3018 	ldr	r3, [pc, #24]	; b9f0 <flash_nand_vs_reset+0x130>
    b9d4:	e1a0e00f 	mov	lr, pc
    b9d8:	e593f000 	ldr	pc, [r3]
    b9dc:	ebfffb0a 	bl	a60c <nand_flash_reset>
    b9e0:	e3a00000 	mov	r0, #0	; 0x0
    b9e4:	e28dd004 	add	sp, sp, #4	; 0x4
    b9e8:	e8bd8030 	pop	{r4, r5, pc}
    b9ec:	0800d258 	.word	0x0800d258
    b9f0:	0800d76c 	.word	0x0800d76c
    b9f4:	00013f04 	.word	0x00013f04
    b9f8:	00013f2f 	.word	0x00013f2f
    b9fc:	0800d2b8 	.word	0x0800d2b8
    ba00:	00013a45 	.word	0x00013a45
    ba04:	2d011400 	.word	0x2d011400
    ba08:	2d011000 	.word	0x2d011000
    ba0c:	00013f64 	.word	0x00013f64
    ba10:	2d009000 	.word	0x2d009000
    ba14:	20500000 	.word	0x20500000
    ba18:	00013bf8 	.word	0x00013bf8

0000ba1c <ram_addr2offs>:
    ba1c:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    ba20:	93a02101 	movls	r2, #1073741824	; 0x40000000
    ba24:	e3a01000 	mov	r1, #0	; 0x0
    ba28:	83a02103 	movhi	r2, #-1073741824	; 0xc0000000
    ba2c:	93a03000 	movls	r3, #0	; 0x0
    ba30:	83e03000 	mvnhi	r3, #0	; 0x0
    ba34:	e0922000 	adds	r2, r2, r0
    ba38:	e0a33001 	adc	r3, r3, r1
    ba3c:	e1a01003 	mov	r1, r3
    ba40:	e1a00002 	mov	r0, r2
    ba44:	e12fff1e 	bx	lr

0000ba48 <ram_offs2addr>:
    ba48:	e3510000 	cmp	r1, #0	; 0x0
    ba4c:	ca000002 	bgt	ba5c <ram_offs2addr+0x14>
    ba50:	1a000003 	bne	ba64 <ram_offs2addr+0x1c>
    ba54:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    ba58:	9a000001 	bls	ba64 <ram_offs2addr+0x1c>
    ba5c:	e2800103 	add	r0, r0, #-1073741824	; 0xc0000000
    ba60:	e12fff1e 	bx	lr
    ba64:	e2800101 	add	r0, r0, #1073741824	; 0x40000000
    ba68:	e12fff1e 	bx	lr

0000ba6c <ram_vs_align>:
    ba6c:	e3a00c02 	mov	r0, #512	; 0x200
    ba70:	e12fff1e 	bx	lr

0000ba74 <ram_vs_reset>:
    ba74:	e3a00000 	mov	r0, #0	; 0x0
    ba78:	e12fff1e 	bx	lr

0000ba7c <ram_vs_write>:
    ba7c:	e92d4010 	push	{r4, lr}
    ba80:	e3530000 	cmp	r3, #0	; 0x0
    ba84:	e1a01000 	mov	r1, r0
    ba88:	e59d4008 	ldr	r4, [sp, #8]
    ba8c:	ca000002 	bgt	ba9c <ram_vs_write+0x20>
    ba90:	1a000003 	bne	baa4 <ram_vs_write+0x28>
    ba94:	e3720107 	cmn	r2, #-1073741823	; 0xc0000001
    ba98:	9a000001 	bls	baa4 <ram_vs_write+0x28>
    ba9c:	e2820103 	add	r0, r2, #-1073741824	; 0xc0000000
    baa0:	ea000000 	b	baa8 <ram_vs_write+0x2c>
    baa4:	e2820101 	add	r0, r2, #1073741824	; 0x40000000
    baa8:	e1a02004 	mov	r2, r4
    baac:	ebffd626 	bl	134c <memcpy>
    bab0:	e1a00004 	mov	r0, r4
    bab4:	e8bd8010 	pop	{r4, pc}

0000bab8 <ram_vs_read>:
    bab8:	e92d4010 	push	{r4, lr}
    babc:	e3530000 	cmp	r3, #0	; 0x0
    bac0:	e59d4008 	ldr	r4, [sp, #8]
    bac4:	ca000002 	bgt	bad4 <ram_vs_read+0x1c>
    bac8:	1a000003 	bne	badc <ram_vs_read+0x24>
    bacc:	e3720107 	cmn	r2, #-1073741823	; 0xc0000001
    bad0:	9a000001 	bls	badc <ram_vs_read+0x24>
    bad4:	e2821103 	add	r1, r2, #-1073741824	; 0xc0000000
    bad8:	ea000000 	b	bae0 <ram_vs_read+0x28>
    badc:	e2821101 	add	r1, r2, #1073741824	; 0x40000000
    bae0:	e1a02004 	mov	r2, r4
    bae4:	ebffd618 	bl	134c <memcpy>
    bae8:	e1a00004 	mov	r0, r4
    baec:	e8bd8010 	pop	{r4, pc}

0000baf0 <udc_get_cable_state>:
    baf0:	e3a00001 	mov	r0, #1	; 0x1
    baf4:	e12fff1e 	bx	lr

0000baf8 <udc_disconnect>:
    baf8:	e59f103c 	ldr	r1, [pc, #60]	; bb3c <udc_disconnect+0x44>
    bafc:	e59f303c 	ldr	r3, [pc, #60]	; bb40 <udc_disconnect+0x48>
    bb00:	e5912010 	ldr	r2, [r1, #16]
    bb04:	e3a00000 	mov	r0, #0	; 0x0
    bb08:	e3822001 	orr	r2, r2, #1	; 0x1
    bb0c:	e92d4010 	push	{r4, lr}
    bb10:	e5c30000 	strb	r0, [r3]
    bb14:	e1a04001 	mov	r4, r1
    bb18:	e5812010 	str	r2, [r1, #16]
    bb1c:	ea000000 	b	bb24 <udc_disconnect+0x2c>
    bb20:	ebfff34c 	bl	8858 <udelay>
    bb24:	e5943010 	ldr	r3, [r4, #16]
    bb28:	e3a0000a 	mov	r0, #10	; 0xa
    bb2c:	e2133001 	ands	r3, r3, #1	; 0x1
    bb30:	1afffffa 	bne	bb20 <udc_disconnect+0x28>
    bb34:	e1a00003 	mov	r0, r3
    bb38:	e8bd8010 	pop	{r4, pc}
    bb3c:	27100000 	.word	0x27100000
    bb40:	0800d2c4 	.word	0x0800d2c4

0000bb44 <udc_event_handle>:
    bb44:	e59f2768 	ldr	r2, [pc, #1896]	; c2b4 <udc_event_handle+0x770>
    bb48:	e59f3768 	ldr	r3, [pc, #1896]	; c2b8 <udc_event_handle+0x774>
    bb4c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    bb50:	e5934014 	ldr	r4, [r3, #20]
    bb54:	e5923000 	ldr	r3, [r2]
    bb58:	e59f0758 	ldr	r0, [pc, #1880]	; c2b8 <udc_event_handle+0x774>
    bb5c:	e1530004 	cmp	r3, r4
    bb60:	e204ca03 	and	ip, r4, #12288	; 0x3000
    bb64:	15824000 	strne	r4, [r2]
    bb68:	e2043b03 	and	r3, r4, #3072	; 0xc00
    bb6c:	e35c0a01 	cmp	ip, #4096	; 0x1000
    bb70:	e24dd008 	sub	sp, sp, #8	; 0x8
    bb74:	e5803014 	str	r3, [r0, #20]
    bb78:	0a000002 	beq	bb88 <udc_event_handle+0x44>
    bb7c:	e35c0a02 	cmp	ip, #8192	; 0x2000
    bb80:	1a000034 	bne	bc58 <udc_event_handle+0x114>
    bb84:	ea000025 	b	bc20 <udc_event_handle+0xdc>
    bb88:	e3a03302 	mov	r3, #134217728	; 0x8000000
    bb8c:	e5803b00 	str	r3, [r0, #2816]
    bb90:	e5803b40 	str	r3, [r0, #2880]
    bb94:	e59f3720 	ldr	r3, [pc, #1824]	; c2bc <udc_event_handle+0x778>
    bb98:	e3a02009 	mov	r2, #9	; 0x9
    bb9c:	e580381c 	str	r3, [r0, #2076]
    bba0:	e3a03b02 	mov	r3, #2048	; 0x800
    bba4:	e5802814 	str	r2, [r0, #2068]
    bba8:	e5802810 	str	r2, [r0, #2064]
    bbac:	e5803024 	str	r3, [r0, #36]
    bbb0:	e2833301 	add	r3, r3, #67108864	; 0x4000000
    bbb4:	e5803028 	str	r3, [r0, #40]
    bbb8:	e2833b01 	add	r3, r3, #1024	; 0x400
    bbbc:	e5803104 	str	r3, [r0, #260]
    bbc0:	e59f36f8 	ldr	r3, [pc, #1784]	; c2c0 <udc_event_handle+0x77c>
    bbc4:	e1a05000 	mov	r5, r0
    bbc8:	e580305c 	str	r3, [r0, #92]
    bbcc:	e5903010 	ldr	r3, [r0, #16]
    bbd0:	e3c33d1f 	bic	r3, r3, #1984	; 0x7c0
    bbd4:	e3833e43 	orr	r3, r3, #1072	; 0x430
    bbd8:	e5803010 	str	r3, [r0, #16]
    bbdc:	ea000000 	b	bbe4 <udc_event_handle+0xa0>
    bbe0:	ebfff31c 	bl	8858 <udelay>
    bbe4:	e5953010 	ldr	r3, [r5, #16]
    bbe8:	e3a00001 	mov	r0, #1	; 0x1
    bbec:	e3130030 	tst	r3, #48	; 0x30
    bbf0:	e59f16c0 	ldr	r1, [pc, #1728]	; c2b8 <udc_event_handle+0x774>
    bbf4:	1afffff9 	bne	bbe0 <udc_event_handle+0x9c>
    bbf8:	e59f36c4 	ldr	r3, [pc, #1732]	; c2c4 <udc_event_handle+0x780>
    bbfc:	e2800009 	add	r0, r0, #9	; 0x9
    bc00:	e5932024 	ldr	r2, [r3, #36]
    bc04:	e59f36bc 	ldr	r3, [pc, #1724]	; c2c8 <udc_event_handle+0x784>
    bc08:	e5813b10 	str	r3, [r1, #2832]
    bc0c:	e3a03a01 	mov	r3, #4096	; 0x1000
    bc10:	e5812b14 	str	r2, [r1, #2836]
    bc14:	e5813014 	str	r3, [r1, #20]
    bc18:	ebfff30e 	bl	8858 <udelay>
    bc1c:	ea00000d 	b	bc58 <udc_event_handle+0x114>
    bc20:	e5902808 	ldr	r2, [r0, #2056]
    bc24:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bc28:	e5803b00 	str	r3, [r0, #2816]
    bc2c:	e1a020a2 	lsr	r2, r2, #1
    bc30:	e59f368c 	ldr	r3, [pc, #1676]	; c2c4 <udc_event_handle+0x780>
    bc34:	e5901018 	ldr	r1, [r0, #24]
    bc38:	e2022003 	and	r2, r2, #3	; 0x3
    bc3c:	e5c3201c 	strb	r2, [r3, #28]
    bc40:	e59f3684 	ldr	r3, [pc, #1668]	; c2cc <udc_event_handle+0x788>
    bc44:	e3811008 	orr	r1, r1, #8	; 0x8
    bc48:	e3a02001 	mov	r2, #1	; 0x1
    bc4c:	e5801018 	str	r1, [r0, #24]
    bc50:	e5832000 	str	r2, [r3]
    bc54:	e580c014 	str	ip, [r0, #20]
    bc58:	e59f3658 	ldr	r3, [pc, #1624]	; c2b8 <udc_event_handle+0x774>
    bc5c:	e3140701 	tst	r4, #262144	; 0x40000
    bc60:	e5939818 	ldr	r9, [r3, #2072]
    bc64:	0a000051 	beq	bdb0 <udc_event_handle+0x26c>
    bc68:	e3190001 	tst	r9, #1	; 0x1
    bc6c:	0a000042 	beq	bd7c <udc_event_handle+0x238>
    bc70:	e5935908 	ldr	r5, [r3, #2312]
    bc74:	e3150001 	tst	r5, #1	; 0x1
    bc78:	0a00003b 	beq	bd6c <udc_event_handle+0x228>
    bc7c:	e59f3640 	ldr	r3, [pc, #1600]	; c2c4 <udc_event_handle+0x780>
    bc80:	e5933020 	ldr	r3, [r3, #32]
    bc84:	e2433002 	sub	r3, r3, #2	; 0x2
    bc88:	e3530003 	cmp	r3, #3	; 0x3
    bc8c:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    bc90:	ea000035 	b	bd6c <udc_event_handle+0x228>
    bc94:	0000bcd4 	.word	0x0000bcd4
    bc98:	0000bca4 	.word	0x0000bca4
    bc9c:	0000bd08 	.word	0x0000bd08
    bca0:	0000bd08 	.word	0x0000bd08
    bca4:	e59f160c 	ldr	r1, [pc, #1548]	; c2b8 <udc_event_handle+0x774>
    bca8:	e59f0614 	ldr	r0, [pc, #1556]	; c2c4 <udc_event_handle+0x780>
    bcac:	e5912910 	ldr	r2, [r1, #2320]
    bcb0:	e590c00c 	ldr	ip, [r0, #12]
    bcb4:	e3a03004 	mov	r3, #4	; 0x4
    bcb8:	e3822702 	orr	r2, r2, #524288	; 0x80000
    bcbc:	e5803020 	str	r3, [r0, #32]
    bcc0:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bcc4:	e5812910 	str	r2, [r1, #2320]
    bcc8:	e581c914 	str	ip, [r1, #2324]
    bccc:	e5813900 	str	r3, [r1, #2304]
    bcd0:	ea000025 	b	bd6c <udc_event_handle+0x228>
    bcd4:	e59f15dc 	ldr	r1, [pc, #1500]	; c2b8 <udc_event_handle+0x774>
    bcd8:	e59f05e4 	ldr	r0, [pc, #1508]	; c2c4 <udc_event_handle+0x780>
    bcdc:	e5913b10 	ldr	r3, [r1, #2832]
    bce0:	e590c024 	ldr	ip, [r0, #36]
    bce4:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    bce8:	e3833702 	orr	r3, r3, #524288	; 0x80000
    bcec:	e5813b10 	str	r3, [r1, #2832]
    bcf0:	e3a02005 	mov	r2, #5	; 0x5
    bcf4:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bcf8:	e581cb14 	str	ip, [r1, #2836]
    bcfc:	e5802020 	str	r2, [r0, #32]
    bd00:	e5813b00 	str	r3, [r1, #2816]
    bd04:	ea000018 	b	bd6c <udc_event_handle+0x228>
    bd08:	e59f05b4 	ldr	r0, [pc, #1460]	; c2c4 <udc_event_handle+0x780>
    bd0c:	e5d03015 	ldrb	r3, [r0, #21]
    bd10:	e3530009 	cmp	r3, #9	; 0x9
    bd14:	1a00000a 	bne	bd44 <udc_event_handle+0x200>
    bd18:	e59f1598 	ldr	r1, [pc, #1432]	; c2b8 <udc_event_handle+0x774>
    bd1c:	e59f35ac 	ldr	r3, [pc, #1452]	; c2d0 <udc_event_handle+0x78c>
    bd20:	e3a02001 	mov	r2, #1	; 0x1
    bd24:	e5813920 	str	r3, [r1, #2336]
    bd28:	e2433d07 	sub	r3, r3, #448	; 0x1c0
    bd2c:	e5813b40 	str	r3, [r1, #2880]
    bd30:	e591381c 	ldr	r3, [r1, #2076]
    bd34:	e5c02000 	strb	r2, [r0]
    bd38:	e3833701 	orr	r3, r3, #262144	; 0x40000
    bd3c:	e3833002 	orr	r3, r3, #2	; 0x2
    bd40:	e581381c 	str	r3, [r1, #2076]
    bd44:	e59f256c 	ldr	r2, [pc, #1388]	; c2b8 <udc_event_handle+0x774>
    bd48:	e59f1574 	ldr	r1, [pc, #1396]	; c2c4 <udc_event_handle+0x780>
    bd4c:	e59f3574 	ldr	r3, [pc, #1396]	; c2c8 <udc_event_handle+0x784>
    bd50:	e5910024 	ldr	r0, [r1, #36]
    bd54:	e5823b10 	str	r3, [r2, #2832]
    bd58:	e3a03001 	mov	r3, #1	; 0x1
    bd5c:	e5813020 	str	r3, [r1, #32]
    bd60:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bd64:	e5820b14 	str	r0, [r2, #2836]
    bd68:	e5823b00 	str	r3, [r2, #2816]
    bd6c:	e300320d 	movw	r3, #525	; 0x20d
    bd70:	e59f2540 	ldr	r2, [pc, #1344]	; c2b8 <udc_event_handle+0x774>
    bd74:	e0053003 	and	r3, r5, r3
    bd78:	e5823908 	str	r3, [r2, #2312]
    bd7c:	e3190002 	tst	r9, #2	; 0x2
    bd80:	0a00000a 	beq	bdb0 <udc_event_handle+0x26c>
    bd84:	e59f352c 	ldr	r3, [pc, #1324]	; c2b8 <udc_event_handle+0x774>
    bd88:	e5932928 	ldr	r2, [r3, #2344]
    bd8c:	e3120001 	tst	r2, #1	; 0x1
    bd90:	0a000003 	beq	bda4 <udc_event_handle+0x260>
    bd94:	e5933930 	ldr	r3, [r3, #2352]
    bd98:	e3d3120f 	bics	r1, r3, #-268435456	; 0xf0000000
    bd9c:	059f3520 	ldreq	r3, [pc, #1312]	; c2c4 <udc_event_handle+0x780>
    bda0:	05c31002 	strbeq	r1, [r3, #2]
    bda4:	e59f350c 	ldr	r3, [pc, #1292]	; c2b8 <udc_event_handle+0x774>
    bda8:	e2022005 	and	r2, r2, #5	; 0x5
    bdac:	e5832928 	str	r2, [r3, #2344]
    bdb0:	e3140702 	tst	r4, #524288	; 0x80000
    bdb4:	0a00013c 	beq	c2ac <udc_event_handle+0x768>
    bdb8:	e3190801 	tst	r9, #65536	; 0x10000
    bdbc:	0a000124 	beq	c254 <udc_event_handle+0x710>
    bdc0:	e59f34f0 	ldr	r3, [pc, #1264]	; c2b8 <udc_event_handle+0x774>
    bdc4:	e593ab08 	ldr	sl, [r3, #2824]
    bdc8:	e31a0001 	tst	sl, #1	; 0x1
    bdcc:	0a00002b 	beq	be80 <udc_event_handle+0x33c>
    bdd0:	e59f34ec 	ldr	r3, [pc, #1260]	; c2c4 <udc_event_handle+0x780>
    bdd4:	e5933020 	ldr	r3, [r3, #32]
    bdd8:	e2433002 	sub	r3, r3, #2	; 0x2
    bddc:	e3530003 	cmp	r3, #3	; 0x3
    bde0:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    bde4:	ea000025 	b	be80 <udc_event_handle+0x33c>
    bde8:	0000be24 	.word	0x0000be24
    bdec:	0000bdf8 	.word	0x0000bdf8
    bdf0:	0000be58 	.word	0x0000be58
    bdf4:	0000be58 	.word	0x0000be58
    bdf8:	e59f14c4 	ldr	r1, [pc, #1220]	; c2c4 <udc_event_handle+0x780>
    bdfc:	e59f24b4 	ldr	r2, [pc, #1204]	; c2b8 <udc_event_handle+0x774>
    be00:	e3a03004 	mov	r3, #4	; 0x4
    be04:	e591000c 	ldr	r0, [r1, #12]
    be08:	e5813020 	str	r3, [r1, #32]
    be0c:	e3a03702 	mov	r3, #524288	; 0x80000
    be10:	e5823910 	str	r3, [r2, #2320]
    be14:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    be18:	e5820914 	str	r0, [r2, #2324]
    be1c:	e5823900 	str	r3, [r2, #2304]
    be20:	ea000016 	b	be80 <udc_event_handle+0x33c>
    be24:	e59f148c 	ldr	r1, [pc, #1164]	; c2b8 <udc_event_handle+0x774>
    be28:	e59f0494 	ldr	r0, [pc, #1172]	; c2c4 <udc_event_handle+0x780>
    be2c:	e5913b10 	ldr	r3, [r1, #2832]
    be30:	e590c00c 	ldr	ip, [r0, #12]
    be34:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    be38:	e3833702 	orr	r3, r3, #524288	; 0x80000
    be3c:	e5813b10 	str	r3, [r1, #2832]
    be40:	e3a02005 	mov	r2, #5	; 0x5
    be44:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    be48:	e581cb14 	str	ip, [r1, #2836]
    be4c:	e5802020 	str	r2, [r0, #32]
    be50:	e5813b00 	str	r3, [r1, #2816]
    be54:	ea000009 	b	be80 <udc_event_handle+0x33c>
    be58:	e59f2458 	ldr	r2, [pc, #1112]	; c2b8 <udc_event_handle+0x774>
    be5c:	e59f1460 	ldr	r1, [pc, #1120]	; c2c4 <udc_event_handle+0x780>
    be60:	e59f3460 	ldr	r3, [pc, #1120]	; c2c8 <udc_event_handle+0x784>
    be64:	e5910024 	ldr	r0, [r1, #36]
    be68:	e5823b10 	str	r3, [r2, #2832]
    be6c:	e3a03001 	mov	r3, #1	; 0x1
    be70:	e5813020 	str	r3, [r1, #32]
    be74:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    be78:	e5820b14 	str	r0, [r2, #2836]
    be7c:	e5823b00 	str	r3, [r2, #2816]
    be80:	e31a0004 	tst	sl, #4	; 0x4
    be84:	159f342c 	ldrne	r3, [pc, #1068]	; c2b8 <udc_event_handle+0x774>
    be88:	13a02004 	movne	r2, #4	; 0x4
    be8c:	15832b08 	strne	r2, [r3, #2824]
    be90:	e31a0008 	tst	sl, #8	; 0x8
    be94:	0a0000ea 	beq	c244 <udc_event_handle+0x700>
    be98:	e31a0040 	tst	sl, #64	; 0x40
    be9c:	e59f1430 	ldr	r1, [pc, #1072]	; c2d4 <udc_event_handle+0x790>
    bea0:	0a000006 	beq	bec0 <udc_event_handle+0x37c>
    bea4:	e59f340c 	ldr	r3, [pc, #1036]	; c2b8 <udc_event_handle+0x774>
    bea8:	e5933b14 	ldr	r3, [r3, #2836]
    beac:	e2432004 	sub	r2, r3, #4	; 0x4
    beb0:	e2433008 	sub	r3, r3, #8	; 0x8
    beb4:	e5812004 	str	r2, [r1, #4]
    beb8:	e5813000 	str	r3, [r1]
    bebc:	ea000005 	b	bed8 <udc_event_handle+0x394>
    bec0:	e59f33fc 	ldr	r3, [pc, #1020]	; c2c4 <udc_event_handle+0x780>
    bec4:	e5932024 	ldr	r2, [r3, #36]
    bec8:	e5923000 	ldr	r3, [r2]
    becc:	e5813000 	str	r3, [r1]
    bed0:	e5923004 	ldr	r3, [r2, #4]
    bed4:	e5813004 	str	r3, [r1, #4]
    bed8:	e59f23e4 	ldr	r2, [pc, #996]	; c2c4 <udc_event_handle+0x780>
    bedc:	e5d20014 	ldrb	r0, [r2, #20]
    bee0:	e3100080 	tst	r0, #128	; 0x80
    bee4:	13a03002 	movne	r3, #2	; 0x2
    bee8:	03a03003 	moveq	r3, #3	; 0x3
    beec:	e5823020 	str	r3, [r2, #32]
    bef0:	e59f23cc 	ldr	r2, [pc, #972]	; c2c4 <udc_event_handle+0x780>
    bef4:	e1d211ba 	ldrh	r1, [r2, #26]
    bef8:	e3510000 	cmp	r1, #0	; 0x0
    befc:	03a03004 	moveq	r3, #4	; 0x4
    bf00:	05823020 	streq	r3, [r2, #32]
    bf04:	e59f33b8 	ldr	r3, [pc, #952]	; c2c4 <udc_event_handle+0x780>
    bf08:	e5d33015 	ldrb	r3, [r3, #21]
    bf0c:	e3530009 	cmp	r3, #9	; 0x9
    bf10:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    bf14:	ea0000ca 	b	c244 <udc_event_handle+0x700>
    bf18:	0000c1dc 	.word	0x0000c1dc
    bf1c:	0000c244 	.word	0x0000c244
    bf20:	0000c244 	.word	0x0000c244
    bf24:	0000c244 	.word	0x0000c244
    bf28:	0000c244 	.word	0x0000c244
    bf2c:	0000c128 	.word	0x0000c128
    bf30:	0000bf40 	.word	0x0000bf40
    bf34:	0000c244 	.word	0x0000c244
    bf38:	0000c244 	.word	0x0000c244
    bf3c:	0000c178 	.word	0x0000c178
    bf40:	e3a00040 	mov	r0, #64	; 0x40
    bf44:	ebffd565 	bl	14e0 <malloc>
    bf48:	e2506000 	subs	r6, r0, #0	; 0x0
    bf4c:	0a0000bc 	beq	c244 <udc_event_handle+0x700>
    bf50:	e59f536c 	ldr	r5, [pc, #876]	; c2c4 <udc_event_handle+0x780>
    bf54:	e1d521b6 	ldrh	r2, [r5, #22]
    bf58:	e1a03422 	lsr	r3, r2, #8
    bf5c:	e2433001 	sub	r3, r3, #1	; 0x1
    bf60:	e3530005 	cmp	r3, #5	; 0x5
    bf64:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    bf68:	ea000054 	b	c0c0 <udc_event_handle+0x57c>
    bf6c:	0000bf84 	.word	0x0000bf84
    bf70:	0000bf9c 	.word	0x0000bf9c
    bf74:	0000bff4 	.word	0x0000bff4
    bf78:	0000c0c0 	.word	0x0000c0c0
    bf7c:	0000c0c0 	.word	0x0000c0c0
    bf80:	0000bfc8 	.word	0x0000bfc8
    bf84:	e1a00006 	mov	r0, r6
    bf88:	e59f1348 	ldr	r1, [pc, #840]	; c2d8 <udc_event_handle+0x794>
    bf8c:	e3a02012 	mov	r2, #18	; 0x12
    bf90:	ebffd4ed 	bl	134c <memcpy>
    bf94:	e3a03012 	mov	r3, #18	; 0x12
    bf98:	ea000028 	b	c040 <udc_event_handle+0x4fc>
    bf9c:	e59f3338 	ldr	r3, [pc, #824]	; c2dc <udc_event_handle+0x798>
    bfa0:	e3a04020 	mov	r4, #32	; 0x20
    bfa4:	e3a0c000 	mov	ip, #0	; 0x0
    bfa8:	e1a01003 	mov	r1, r3
    bfac:	e1a00006 	mov	r0, r6
    bfb0:	e1a02004 	mov	r2, r4
    bfb4:	e5c3c003 	strb	ip, [r3, #3]
    bfb8:	e5c34002 	strb	r4, [r3, #2]
    bfbc:	ebffd4e2 	bl	134c <memcpy>
    bfc0:	e5854004 	str	r4, [r5, #4]
    bfc4:	ea000045 	b	c0e0 <udc_event_handle+0x59c>
    bfc8:	e1d531ba 	ldrh	r3, [r5, #26]
    bfcc:	e59fc30c 	ldr	ip, [pc, #780]	; c2e0 <udc_event_handle+0x79c>
    bfd0:	e353000a 	cmp	r3, #10	; 0xa
    bfd4:	23a0300a 	movcs	r3, #10	; 0xa
    bfd8:	e5cc3000 	strb	r3, [ip]
    bfdc:	e1a0100c 	mov	r1, ip
    bfe0:	e1a00006 	mov	r0, r6
    bfe4:	e3a0200a 	mov	r2, #10	; 0xa
    bfe8:	ebffd4d7 	bl	134c <memcpy>
    bfec:	e3a0300a 	mov	r3, #10	; 0xa
    bff0:	ea000012 	b	c040 <udc_event_handle+0x4fc>
    bff4:	e21270ff 	ands	r7, r2, #255	; 0xff
    bff8:	13a03000 	movne	r3, #0	; 0x0
    bffc:	03a03001 	moveq	r3, #1	; 0x1
    c000:	e3570005 	cmp	r7, #5	; 0x5
    c004:	d1a08003 	movle	r8, r3
    c008:	c3838001 	orrgt	r8, r3, #1	; 0x1
    c00c:	e3580000 	cmp	r8, #0	; 0x0
    c010:	e59f52ac 	ldr	r5, [pc, #684]	; c2c4 <udc_event_handle+0x780>
    c014:	0a00000b 	beq	c048 <udc_event_handle+0x504>
    c018:	e1d531ba 	ldrh	r3, [r5, #26]
    c01c:	e59fc2c0 	ldr	ip, [pc, #704]	; c2e4 <udc_event_handle+0x7a0>
    c020:	e3530004 	cmp	r3, #4	; 0x4
    c024:	23a03004 	movcs	r3, #4	; 0x4
    c028:	e5cc3000 	strb	r3, [ip]
    c02c:	e1a0100c 	mov	r1, ip
    c030:	e1a00006 	mov	r0, r6
    c034:	e3a02004 	mov	r2, #4	; 0x4
    c038:	ebffd4c3 	bl	134c <memcpy>
    c03c:	e3a03004 	mov	r3, #4	; 0x4
    c040:	e5853004 	str	r3, [r5, #4]
    c044:	ea000025 	b	c0e0 <udc_event_handle+0x59c>
    c048:	e59f4298 	ldr	r4, [pc, #664]	; c2e8 <udc_event_handle+0x7a4>
    c04c:	e7940107 	ldr	r0, [r4, r7, lsl #2]
    c050:	ebffd4a3 	bl	12e4 <strlen>
    c054:	e6ef0070 	uxtb	r0, r0
    c058:	e2803001 	add	r3, r0, #1	; 0x1
    c05c:	e5d5201a 	ldrb	r2, [r5, #26]
    c060:	e1a03083 	lsl	r3, r3, #1
    c064:	e6ef3073 	uxtb	r3, r3
    c068:	e1530002 	cmp	r3, r2
    c06c:	31a0e003 	movcc	lr, r3
    c070:	21a0e002 	movcs	lr, r2
    c074:	e3a03003 	mov	r3, #3	; 0x3
    c078:	e5c63001 	strb	r3, [r6, #1]
    c07c:	e5c6e000 	strb	lr, [r6]
    c080:	e1a05008 	mov	r5, r8
    c084:	e3a02001 	mov	r2, #1	; 0x1
    c088:	ea000005 	b	c0a4 <udc_event_handle+0x560>
    c08c:	e7943107 	ldr	r3, [r4, r7, lsl #2]
    c090:	e0833002 	add	r3, r3, r2
    c094:	e5533001 	ldrb	r3, [r3, #-1]
    c098:	e7c63082 	strb	r3, [r6, r2, lsl #1]
    c09c:	e5c15001 	strb	r5, [r1, #1]
    c0a0:	e6ef207c 	uxtb	r2, ip
    c0a4:	e1500002 	cmp	r0, r2
    c0a8:	e282c001 	add	ip, r2, #1	; 0x1
    c0ac:	e0861082 	add	r1, r6, r2, lsl #1
    c0b0:	aafffff5 	bge	c08c <udc_event_handle+0x548>
    c0b4:	e59f3208 	ldr	r3, [pc, #520]	; c2c4 <udc_event_handle+0x780>
    c0b8:	e583e004 	str	lr, [r3, #4]
    c0bc:	ea000007 	b	c0e0 <udc_event_handle+0x59c>
    c0c0:	e59f21f0 	ldr	r2, [pc, #496]	; c2b8 <udc_event_handle+0x774>
    c0c4:	e3a03702 	mov	r3, #524288	; 0x80000
    c0c8:	e5951024 	ldr	r1, [r5, #36]
    c0cc:	e5823910 	str	r3, [r2, #2320]
    c0d0:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    c0d4:	e5821914 	str	r1, [r2, #2324]
    c0d8:	e5823900 	str	r3, [r2, #2304]
    c0dc:	ea00000e 	b	c11c <udc_event_handle+0x5d8>
    c0e0:	e59f11dc 	ldr	r1, [pc, #476]	; c2c4 <udc_event_handle+0x780>
    c0e4:	e59f01cc 	ldr	r0, [pc, #460]	; c2b8 <udc_event_handle+0x774>
    c0e8:	e1d120b4 	ldrh	r2, [r1, #4]
    c0ec:	e1d131ba 	ldrh	r3, [r1, #26]
    c0f0:	e1520003 	cmp	r2, r3
    c0f4:	21a02003 	movcs	r2, r3
    c0f8:	e3823702 	orr	r3, r2, #524288	; 0x80000
    c0fc:	e5803910 	str	r3, [r0, #2320]
    c100:	e3a03000 	mov	r3, #0	; 0x0
    c104:	e5c13001 	strb	r3, [r1, #1]
    c108:	e2833321 	add	r3, r3, #-2080374784	; 0x84000000
    c10c:	e5812004 	str	r2, [r1, #4]
    c110:	e5806914 	str	r6, [r0, #2324]
    c114:	e5803900 	str	r3, [r0, #2304]
    c118:	e581600c 	str	r6, [r1, #12]
    c11c:	e1a00006 	mov	r0, r6
    c120:	ebffd556 	bl	1680 <free>
    c124:	ea000046 	b	c244 <udc_event_handle+0x700>
    c128:	e59fc1bc 	ldr	ip, [pc, #444]	; c2ec <udc_event_handle+0x7a8>
    c12c:	e5dc3000 	ldrb	r3, [ip]
    c130:	e35300ff 	cmp	r3, #255	; 0xff
    c134:	1a000042 	bne	c244 <udc_event_handle+0x700>
    c138:	e59f1178 	ldr	r1, [pc, #376]	; c2b8 <udc_event_handle+0x774>
    c13c:	e59f0180 	ldr	r0, [pc, #384]	; c2c4 <udc_event_handle+0x780>
    c140:	e5913800 	ldr	r3, [r1, #2048]
    c144:	e1d021b6 	ldrh	r2, [r0, #22]
    c148:	e3c33e7f 	bic	r3, r3, #2032	; 0x7f0
    c14c:	e202207f 	and	r2, r2, #127	; 0x7f
    c150:	e1833202 	orr	r3, r3, r2, lsl #4
    c154:	e5813800 	str	r3, [r1, #2048]
    c158:	e5900024 	ldr	r0, [r0, #36]
    c15c:	e3a03702 	mov	r3, #524288	; 0x80000
    c160:	e5cc2000 	strb	r2, [ip]
    c164:	e5813910 	str	r3, [r1, #2320]
    c168:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    c16c:	e5810914 	str	r0, [r1, #2324]
    c170:	e5813900 	str	r3, [r1, #2304]
    c174:	ea000032 	b	c244 <udc_event_handle+0x700>
    c178:	e59f016c 	ldr	r0, [pc, #364]	; c2ec <udc_event_handle+0x7a8>
    c17c:	e5d03000 	ldrb	r3, [r0]
    c180:	e35300ff 	cmp	r3, #255	; 0xff
    c184:	0a00002e 	beq	c244 <udc_event_handle+0x700>
    c188:	e59f1134 	ldr	r1, [pc, #308]	; c2c4 <udc_event_handle+0x780>
    c18c:	e1d131b6 	ldrh	r3, [r1, #22]
    c190:	e3530000 	cmp	r3, #0	; 0x0
    c194:	03e03000 	mvneq	r3, #0	; 0x0
    c198:	05911024 	ldreq	r1, [r1, #36]
    c19c:	059f2114 	ldreq	r2, [pc, #276]	; c2b8 <udc_event_handle+0x774>
    c1a0:	05c03000 	strbeq	r3, [r0]
    c1a4:	0a000013 	beq	c1f8 <udc_event_handle+0x6b4>
    c1a8:	e3530001 	cmp	r3, #1	; 0x1
    c1ac:	1a000024 	bne	c244 <udc_event_handle+0x700>
    c1b0:	e59f2100 	ldr	r2, [pc, #256]	; c2b8 <udc_event_handle+0x774>
    c1b4:	e59f3134 	ldr	r3, [pc, #308]	; c2f0 <udc_event_handle+0x7ac>
    c1b8:	e5911024 	ldr	r1, [r1, #36]
    c1bc:	e5823920 	str	r3, [r2, #2336]
    c1c0:	e2833d07 	add	r3, r3, #448	; 0x1c0
    c1c4:	e5823b40 	str	r3, [r2, #2880]
    c1c8:	e592381c 	ldr	r3, [r2, #2076]
    c1cc:	e3833701 	orr	r3, r3, #262144	; 0x40000
    c1d0:	e3833002 	orr	r3, r3, #2	; 0x2
    c1d4:	e582381c 	str	r3, [r2, #2076]
    c1d8:	ea000006 	b	c1f8 <udc_event_handle+0x6b4>
    c1dc:	e210c01f 	ands	ip, r0, #31	; 0x1f
    c1e0:	1a000017 	bne	c244 <udc_event_handle+0x700>
    c1e4:	e3510000 	cmp	r1, #0	; 0x0
    c1e8:	e59f50d4 	ldr	r5, [pc, #212]	; c2c4 <udc_event_handle+0x780>
    c1ec:	1a000004 	bne	c204 <udc_event_handle+0x6c0>
    c1f0:	e5951024 	ldr	r1, [r5, #36]
    c1f4:	e59f20bc 	ldr	r2, [pc, #188]	; c2b8 <udc_event_handle+0x774>
    c1f8:	e3a03702 	mov	r3, #524288	; 0x80000
    c1fc:	e5823910 	str	r3, [r2, #2320]
    c200:	ea00000c 	b	c238 <udc_event_handle+0x6f4>
    c204:	e1d531b6 	ldrh	r3, [r5, #22]
    c208:	e59f20a8 	ldr	r2, [pc, #168]	; c2b8 <udc_event_handle+0x774>
    c20c:	e3530002 	cmp	r3, #2	; 0x2
    c210:	23a03002 	movcs	r3, #2	; 0x2
    c214:	e6ef3073 	uxtb	r3, r3
    c218:	e1530001 	cmp	r3, r1
    c21c:	21a03001 	movcs	r3, r1
    c220:	e3830702 	orr	r0, r3, #524288	; 0x80000
    c224:	e28d1006 	add	r1, sp, #6	; 0x6
    c228:	e5c5c001 	strb	ip, [r5, #1]
    c22c:	e5853004 	str	r3, [r5, #4]
    c230:	e585100c 	str	r1, [r5, #12]
    c234:	e5820910 	str	r0, [r2, #2320]
    c238:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    c23c:	e5821914 	str	r1, [r2, #2324]
    c240:	e5823900 	str	r3, [r2, #2304]
    c244:	e3003209 	movw	r3, #521	; 0x209
    c248:	e59f2068 	ldr	r2, [pc, #104]	; c2b8 <udc_event_handle+0x774>
    c24c:	e00a3003 	and	r3, sl, r3
    c250:	e5823b08 	str	r3, [r2, #2824]
    c254:	e3190701 	tst	r9, #262144	; 0x40000
    c258:	0a000013 	beq	c2ac <udc_event_handle+0x768>
    c25c:	e59f1054 	ldr	r1, [pc, #84]	; c2b8 <udc_event_handle+0x774>
    c260:	e5912b48 	ldr	r2, [r1, #2888]
    c264:	e3120001 	tst	r2, #1	; 0x1
    c268:	0a00000c 	beq	c2a0 <udc_event_handle+0x75c>
    c26c:	e5913b50 	ldr	r3, [r1, #2896]
    c270:	e59f504c 	ldr	r5, [pc, #76]	; c2c4 <udc_event_handle+0x780>
    c274:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
    c278:	e3c3373e 	bic	r3, r3, #16252928	; 0xf80000
    c27c:	e3530c02 	cmp	r3, #512	; 0x200
    c280:	05953028 	ldreq	r3, [r5, #40]
    c284:	13a03000 	movne	r3, #0	; 0x0
    c288:	03833702 	orreq	r3, r3, #524288	; 0x80000
    c28c:	05813b50 	streq	r3, [r1, #2896]
    c290:	05913b40 	ldreq	r3, [r1, #2880]
    c294:	15c53003 	strbne	r3, [r5, #3]
    c298:	03833321 	orreq	r3, r3, #-2080374784	; 0x84000000
    c29c:	05813b40 	streq	r3, [r1, #2880]
    c2a0:	e59f3010 	ldr	r3, [pc, #16]	; c2b8 <udc_event_handle+0x774>
    c2a4:	e2022005 	and	r2, r2, #5	; 0x5
    c2a8:	e5832b48 	str	r2, [r3, #2888]
    c2ac:	e28dd008 	add	sp, sp, #8	; 0x8
    c2b0:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    c2b4:	0800d2c0 	.word	0x0800d2c0
    c2b8:	27100000 	.word	0x27100000
    c2bc:	00010001 	.word	0x00010001
    c2c0:	0c001780 	.word	0x0c001780
    c2c4:	0800d2c4 	.word	0x0800d2c4
    c2c8:	60080040 	.word	0x60080040
    c2cc:	0800d2bc 	.word	0x0800d2bc
    c2d0:	18088200 	.word	0x18088200
    c2d4:	0800d2d8 	.word	0x0800d2d8
    c2d8:	00011855 	.word	0x00011855
    c2dc:	0800d161 	.word	0x0800d161
    c2e0:	0800d181 	.word	0x0800d181
    c2e4:	0800d138 	.word	0x0800d138
    c2e8:	0800d7d4 	.word	0x0800d7d4
    c2ec:	0800d160 	.word	0x0800d160
    c2f0:	18088040 	.word	0x18088040

0000c2f4 <udc_write>:
    c2f4:	e59f30a4 	ldr	r3, [pc, #164]	; c3a0 <udc_write+0xac>
    c2f8:	e3a02001 	mov	r2, #1	; 0x1
    c2fc:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    c300:	e583000c 	str	r0, [r3, #12]
    c304:	e1a05001 	mov	r5, r1
    c308:	e5c32001 	strb	r2, [r3, #1]
    c30c:	e1a04003 	mov	r4, r3
    c310:	e1a08002 	mov	r8, r2
    c314:	e59f6088 	ldr	r6, [pc, #136]	; c3a4 <udc_write+0xb0>
    c318:	e59f7088 	ldr	r7, [pc, #136]	; c3a8 <udc_write+0xb4>
    c31c:	ea00001b 	b	c390 <udc_write+0x9c>
    c320:	e3550701 	cmp	r5, #262144	; 0x40000
    c324:	b1a02005 	movlt	r2, r5
    c328:	a3a02701 	movge	r2, #262144	; 0x40000
    c32c:	e292303f 	adds	r3, r2, #63	; 0x3f
    c330:	4282307e 	addmi	r3, r2, #126	; 0x7e
    c334:	e594100c 	ldr	r1, [r4, #12]
    c338:	e1a03343 	asr	r3, r3, #6
    c33c:	e1823983 	orr	r3, r2, r3, lsl #19
    c340:	e5863930 	str	r3, [r6, #2352]
    c344:	e5861934 	str	r1, [r6, #2356]
    c348:	e5963920 	ldr	r3, [r6, #2336]
    c34c:	e5842004 	str	r2, [r4, #4]
    c350:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    c354:	e5863920 	str	r3, [r6, #2336]
    c358:	e5c48002 	strb	r8, [r4, #2]
    c35c:	ea000000 	b	c364 <udc_write+0x70>
    c360:	ebfffdf7 	bl	bb44 <udc_event_handle>
    c364:	e5d43002 	ldrb	r3, [r4, #2]
    c368:	e3530000 	cmp	r3, #0	; 0x0
    c36c:	0a000002 	beq	c37c <udc_write+0x88>
    c370:	e5973000 	ldr	r3, [r7]
    c374:	e3530000 	cmp	r3, #0	; 0x0
    c378:	1afffff8 	bne	c360 <udc_write+0x6c>
    c37c:	e5943004 	ldr	r3, [r4, #4]
    c380:	e594200c 	ldr	r2, [r4, #12]
    c384:	e0635005 	rsb	r5, r3, r5
    c388:	e0833002 	add	r3, r3, r2
    c38c:	e584300c 	str	r3, [r4, #12]
    c390:	e3550000 	cmp	r5, #0	; 0x0
    c394:	1affffe1 	bne	c320 <udc_write+0x2c>
    c398:	e1a00005 	mov	r0, r5
    c39c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    c3a0:	0800d2c4 	.word	0x0800d2c4
    c3a4:	27100000 	.word	0x27100000
    c3a8:	0800d15c 	.word	0x0800d15c

0000c3ac <udc_read>:
    c3ac:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    c3b0:	e59f30b4 	ldr	r3, [pc, #180]	; c46c <udc_read+0xc0>
    c3b4:	e24dd004 	sub	sp, sp, #4	; 0x4
    c3b8:	e1a04001 	mov	r4, r1
    c3bc:	e1a05003 	mov	r5, r3
    c3c0:	e5830010 	str	r0, [r3, #16]
    c3c4:	e3a0a001 	mov	sl, #1	; 0x1
    c3c8:	e59f60a0 	ldr	r6, [pc, #160]	; c470 <udc_read+0xc4>
    c3cc:	e59f80a0 	ldr	r8, [pc, #160]	; c474 <udc_read+0xc8>
    c3d0:	e1a07003 	mov	r7, r3
    c3d4:	ea00001f 	b	c458 <udc_read+0xac>
    c3d8:	e3540701 	cmp	r4, #262144	; 0x40000
    c3dc:	b1a02004 	movlt	r2, r4
    c3e0:	a3a02701 	movge	r2, #262144	; 0x40000
    c3e4:	e2823f7f 	add	r3, r2, #508	; 0x1fc
    c3e8:	e2833003 	add	r3, r3, #3	; 0x3
    c3ec:	e5951010 	ldr	r1, [r5, #16]
    c3f0:	e1a034c3 	asr	r3, r3, #9
    c3f4:	e1823983 	orr	r3, r2, r3, lsl #19
    c3f8:	e5863b50 	str	r3, [r6, #2896]
    c3fc:	e5861b54 	str	r1, [r6, #2900]
    c400:	e5963b40 	ldr	r3, [r6, #2880]
    c404:	e5852008 	str	r2, [r5, #8]
    c408:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    c40c:	e5863b40 	str	r3, [r6, #2880]
    c410:	e5c5a003 	strb	sl, [r5, #3]
    c414:	ea000002 	b	c424 <udc_read+0x78>
    c418:	ebfffdc9 	bl	bb44 <udc_event_handle>
    c41c:	e3a00001 	mov	r0, #1	; 0x1
    c420:	ebfff10c 	bl	8858 <udelay>
    c424:	e5d53003 	ldrb	r3, [r5, #3]
    c428:	e3530000 	cmp	r3, #0	; 0x0
    c42c:	0a000004 	beq	c444 <udc_read+0x98>
    c430:	e5983000 	ldr	r3, [r8]
    c434:	e3530000 	cmp	r3, #0	; 0x0
    c438:	1afffff6 	bne	c418 <udc_read+0x6c>
    c43c:	e3e04000 	mvn	r4, #0	; 0x0
    c440:	ea000006 	b	c460 <udc_read+0xb4>
    c444:	e5973008 	ldr	r3, [r7, #8]
    c448:	e5972010 	ldr	r2, [r7, #16]
    c44c:	e0634004 	rsb	r4, r3, r4
    c450:	e0833002 	add	r3, r3, r2
    c454:	e5873010 	str	r3, [r7, #16]
    c458:	e3540000 	cmp	r4, #0	; 0x0
    c45c:	1affffdd 	bne	c3d8 <udc_read+0x2c>
    c460:	e1a00004 	mov	r0, r4
    c464:	e28dd004 	add	sp, sp, #4	; 0x4
    c468:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    c46c:	0800d2c4 	.word	0x0800d2c4
    c470:	27100000 	.word	0x27100000
    c474:	0800d15c 	.word	0x0800d15c

0000c478 <udc_connect>:
    c478:	e92d4070 	push	{r4, r5, r6, lr}
    c47c:	e59f41e4 	ldr	r4, [pc, #484]	; c668 <udc_connect+0x1f0>
    c480:	e3a02000 	mov	r2, #0	; 0x0
    c484:	e5943024 	ldr	r3, [r4, #36]
    c488:	e3e01000 	mvn	r1, #0	; 0x0
    c48c:	e1530002 	cmp	r3, r2
    c490:	e59f31d4 	ldr	r3, [pc, #468]	; c66c <udc_connect+0x1f4>
    c494:	e5c42003 	strb	r2, [r4, #3]
    c498:	e5c31000 	strb	r1, [r3]
    c49c:	e5c42000 	strb	r2, [r4]
    c4a0:	e5c42002 	strb	r2, [r4, #2]
    c4a4:	1a000002 	bne	c4b4 <udc_connect+0x3c>
    c4a8:	e3a00028 	mov	r0, #40	; 0x28
    c4ac:	ebffd40b 	bl	14e0 <malloc>
    c4b0:	e5840024 	str	r0, [r4, #36]
    c4b4:	e59f31ac 	ldr	r3, [pc, #428]	; c668 <udc_connect+0x1f0>
    c4b8:	e3a06001 	mov	r6, #1	; 0x1
    c4bc:	e59f51ac 	ldr	r5, [pc, #428]	; c670 <udc_connect+0x1f8>
    c4c0:	e59f01ac 	ldr	r0, [pc, #428]	; c674 <udc_connect+0x1fc>
    c4c4:	e5836020 	str	r6, [r3, #32]
    c4c8:	ebffd094 	bl	720 <module_enable>
    c4cc:	e3a02004 	mov	r2, #4	; 0x4
    c4d0:	e3a03003 	mov	r3, #3	; 0x3
    c4d4:	e3a01000 	mov	r1, #0	; 0x0
    c4d8:	e585243c 	str	r2, [r5, #1084]
    c4dc:	e5853440 	str	r3, [r5, #1088]
    c4e0:	e5852444 	str	r2, [r5, #1092]
    c4e4:	e5853448 	str	r3, [r5, #1096]
    c4e8:	e585344c 	str	r3, [r5, #1100]
    c4ec:	e5853450 	str	r3, [r5, #1104]
    c4f0:	e5856454 	str	r6, [r5, #1108]
    c4f4:	e5856458 	str	r6, [r5, #1112]
    c4f8:	e585145c 	str	r1, [r5, #1116]
    c4fc:	e5851460 	str	r1, [r5, #1120]
    c500:	e5953420 	ldr	r3, [r5, #1056]
    c504:	e1a00006 	mov	r0, r6
    c508:	e3c33007 	bic	r3, r3, #7	; 0x7
    c50c:	e3833005 	orr	r3, r3, #5	; 0x5
    c510:	e5853420 	str	r3, [r5, #1056]
    c514:	e5953424 	ldr	r3, [r5, #1060]
    c518:	e3833003 	orr	r3, r3, #3	; 0x3
    c51c:	e5853424 	str	r3, [r5, #1060]
    c520:	e595342c 	ldr	r3, [r5, #1068]
    c524:	e1833006 	orr	r3, r3, r6
    c528:	e585342c 	str	r3, [r5, #1068]
    c52c:	e5954400 	ldr	r4, [r5, #1024]
    c530:	e384400f 	orr	r4, r4, #15	; 0xf
    c534:	e5854400 	str	r4, [r5, #1024]
    c538:	ebfff0c6 	bl	8858 <udelay>
    c53c:	e3c4300e 	bic	r3, r4, #14	; 0xe
    c540:	e5853400 	str	r3, [r5, #1024]
    c544:	e1a00006 	mov	r0, r6
    c548:	e3c4400f 	bic	r4, r4, #15	; 0xf
    c54c:	ebfff0c1 	bl	8858 <udelay>
    c550:	e5854400 	str	r4, [r5, #1024]
    c554:	e1a00006 	mov	r0, r6
    c558:	ebfff0be 	bl	8858 <udelay>
    c55c:	e59f2114 	ldr	r2, [pc, #276]	; c678 <udc_connect+0x200>
    c560:	e59f3114 	ldr	r3, [pc, #276]	; c67c <udc_connect+0x204>
    c564:	e1a05002 	mov	r5, r2
    c568:	e582300c 	str	r3, [r2, #12]
    c56c:	e5954010 	ldr	r4, [r5, #16]
    c570:	e3a00028 	mov	r0, #40	; 0x28
    c574:	ebfff0b7 	bl	8858 <udelay>
    c578:	e3540000 	cmp	r4, #0	; 0x0
    c57c:	e59f40f4 	ldr	r4, [pc, #244]	; c678 <udc_connect+0x200>
    c580:	aafffff9 	bge	c56c <udc_connect+0xf4>
    c584:	e5943010 	ldr	r3, [r4, #16]
    c588:	e3a0000a 	mov	r0, #10	; 0xa
    c58c:	e3833001 	orr	r3, r3, #1	; 0x1
    c590:	e5843010 	str	r3, [r4, #16]
    c594:	ebfff0af 	bl	8858 <udelay>
    c598:	e1a05004 	mov	r5, r4
    c59c:	e5954010 	ldr	r4, [r5, #16]
    c5a0:	e3a0000a 	mov	r0, #10	; 0xa
    c5a4:	ebfff0ab 	bl	8858 <udelay>
    c5a8:	e3140001 	tst	r4, #1	; 0x1
    c5ac:	e59fc0c4 	ldr	ip, [pc, #196]	; c678 <udc_connect+0x200>
    c5b0:	1afffff9 	bne	c59c <udc_connect+0x124>
    c5b4:	e3a03027 	mov	r3, #39	; 0x27
    c5b8:	e58c3008 	str	r3, [ip, #8]
    c5bc:	e59c303c 	ldr	r3, [ip, #60]
    c5c0:	e59c3014 	ldr	r3, [ip, #20]
    c5c4:	e2130001 	ands	r0, r3, #1	; 0x1
    c5c8:	1a000024 	bne	c660 <udc_connect+0x1e8>
    c5cc:	e3a03301 	mov	r3, #67108864	; 0x4000000
    c5d0:	e58c3800 	str	r3, [ip, #2048]
    c5d4:	e59f30a4 	ldr	r3, [pc, #164]	; c680 <udc_connect+0x208>
    c5d8:	e59f20a4 	ldr	r2, [pc, #164]	; c684 <udc_connect+0x20c>
    c5dc:	e58c3018 	str	r3, [ip, #24]
    c5e0:	e59f30a0 	ldr	r3, [pc, #160]	; c688 <udc_connect+0x210>
    c5e4:	e59f10a0 	ldr	r1, [pc, #160]	; c68c <udc_connect+0x214>
    c5e8:	e5823014 	str	r3, [r2, #20]
    c5ec:	e59f309c 	ldr	r3, [pc, #156]	; c690 <udc_connect+0x218>
    c5f0:	e5821010 	str	r1, [r2, #16]
    c5f4:	e5823004 	str	r3, [r2, #4]
    c5f8:	e59f3094 	ldr	r3, [pc, #148]	; c694 <udc_connect+0x21c>
    c5fc:	e582100c 	str	r1, [r2, #12]
    c600:	e5823008 	str	r3, [r2, #8]
    c604:	ebfff0b7 	bl	88e8 <get_timer>
    c608:	e59f5058 	ldr	r5, [pc, #88]	; c668 <udc_connect+0x1f0>
    c60c:	e1a04000 	mov	r4, r0
    c610:	ebfffd4b 	bl	bb44 <udc_event_handle>
    c614:	e5d53000 	ldrb	r3, [r5]
    c618:	e1a00004 	mov	r0, r4
    c61c:	e3530000 	cmp	r3, #0	; 0x0
    c620:	1a000003 	bne	c634 <udc_connect+0x1bc>
    c624:	ebfff0af 	bl	88e8 <get_timer>
    c628:	e3500ffa 	cmp	r0, #1000	; 0x3e8
    c62c:	9afffff7 	bls	c610 <udc_connect+0x198>
    c630:	ea00000a 	b	c660 <udc_connect+0x1e8>
    c634:	e59f203c 	ldr	r2, [pc, #60]	; c678 <udc_connect+0x200>
    c638:	e59f3058 	ldr	r3, [pc, #88]	; c698 <udc_connect+0x220>
    c63c:	e3a00000 	mov	r0, #0	; 0x0
    c640:	e5823920 	str	r3, [r2, #2336]
    c644:	e2833d07 	add	r3, r3, #448	; 0x1c0
    c648:	e5823b40 	str	r3, [r2, #2880]
    c64c:	e592381c 	ldr	r3, [r2, #2076]
    c650:	e3833701 	orr	r3, r3, #262144	; 0x40000
    c654:	e3833002 	orr	r3, r3, #2	; 0x2
    c658:	e582381c 	str	r3, [r2, #2076]
    c65c:	e8bd8070 	pop	{r4, r5, r6, pc}
    c660:	e3e00000 	mvn	r0, #0	; 0x0
    c664:	e8bd8070 	pop	{r4, r5, r6, pc}
    c668:	0800d2c4 	.word	0x0800d2c4
    c66c:	0800d160 	.word	0x0800d160
    c670:	2d038000 	.word	0x2d038000
    c674:	2d038400 	.word	0x2d038400
    c678:	27100000 	.word	0x27100000
    c67c:	4c001408 	.word	0x4c001408
    c680:	000c3c00 	.word	0x000c3c00
    c684:	0800d7d4 	.word	0x0800d7d4
    c688:	00013f68 	.word	0x00013f68
    c68c:	00013f7d 	.word	0x00013f7d
    c690:	00013f71 	.word	0x00013f71
    c694:	0800d13c 	.word	0x0800d13c
    c698:	18088040 	.word	0x18088040

0000c69c <linkpc_set_stage>:
    c69c:	e1a01000 	mov	r1, r0
    c6a0:	e59f0004 	ldr	r0, [pc, #4]	; c6ac <linkpc_set_stage+0x10>
    c6a4:	e3a02010 	mov	r2, #16	; 0x10
    c6a8:	eaffd2bd 	b	11a4 <strncpy>
    c6ac:	0800d18c 	.word	0x0800d18c

0000c6b0 <__response_ok>:
    c6b0:	e92d4010 	push	{r4, lr}
    c6b4:	e24dd020 	sub	sp, sp, #32	; 0x20
    c6b8:	e59f1038 	ldr	r1, [pc, #56]	; c6f8 <__response_ok+0x48>
    c6bc:	e3a02005 	mov	r2, #5	; 0x5
    c6c0:	e1a0000d 	mov	r0, sp
    c6c4:	ebffd320 	bl	134c <memcpy>
    c6c8:	e3a0201b 	mov	r2, #27	; 0x1b
    c6cc:	e3a01000 	mov	r1, #0	; 0x0
    c6d0:	e28d0005 	add	r0, sp, #5	; 0x5
    c6d4:	ebffd314 	bl	132c <memset>
    c6d8:	e1a0000d 	mov	r0, sp
    c6dc:	e3a01004 	mov	r1, #4	; 0x4
    c6e0:	ebffff03 	bl	c2f4 <udc_write>
    c6e4:	e59f0010 	ldr	r0, [pc, #16]	; c6fc <__response_ok+0x4c>
    c6e8:	e1a0400d 	mov	r4, sp
    c6ec:	ebfff059 	bl	8858 <udelay>
    c6f0:	e28dd020 	add	sp, sp, #32	; 0x20
    c6f4:	e8bd8010 	pop	{r4, pc}
    c6f8:	00011867 	.word	0x00011867
    c6fc:	0007a120 	.word	0x0007a120

0000c700 <linkpc_loop>:
    c700:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    c704:	e59f05e0 	ldr	r0, [pc, #1504]	; ccec <linkpc_loop+0x5ec>
    c708:	e24ddf59 	sub	sp, sp, #356	; 0x164
    c70c:	e59f45dc 	ldr	r4, [pc, #1500]	; ccf0 <linkpc_loop+0x5f0>
    c710:	e1a0e00f 	mov	lr, pc
    c714:	e594f000 	ldr	pc, [r4]
    c718:	ebffff56 	bl	c478 <udc_connect>
    c71c:	e3500000 	cmp	r0, #0	; 0x0
    c720:	13a04001 	movne	r4, #1	; 0x1
    c724:	1a000169 	bne	ccd0 <linkpc_loop+0x5d0>
    c728:	e59f05c4 	ldr	r0, [pc, #1476]	; ccf4 <linkpc_loop+0x5f4>
    c72c:	e1a0e00f 	mov	lr, pc
    c730:	e594f000 	ldr	pc, [r4]
    c734:	e59f05bc 	ldr	r0, [pc, #1468]	; ccf8 <linkpc_loop+0x5f8>
    c738:	e1a0e00f 	mov	lr, pc
    c73c:	e594f000 	ldr	pc, [r4]
    c740:	e28d708c 	add	r7, sp, #140	; 0x8c
    c744:	e59f95b0 	ldr	r9, [pc, #1456]	; ccfc <linkpc_loop+0x5fc>
    c748:	e1a0a004 	mov	sl, r4
    c74c:	e287b004 	add	fp, r7, #4	; 0x4
    c750:	e5991000 	ldr	r1, [r9]
    c754:	e3510000 	cmp	r1, #0	; 0x0
    c758:	159f25a0 	ldrne	r2, [pc, #1440]	; cd00 <linkpc_loop+0x600>
    c75c:	028d4f43 	addeq	r4, sp, #268	; 0x10c
    c760:	15923000 	ldrne	r3, [r2]
    c764:	03a05040 	moveq	r5, #64	; 0x40
    c768:	12834302 	addne	r4, r3, #134217728	; 0x8000000
    c76c:	11a05001 	movne	r5, r1
    c770:	e1a01005 	mov	r1, r5
    c774:	e1a00004 	mov	r0, r4
    c778:	ebffff0b 	bl	c3ac <udc_read>
    c77c:	e1a00007 	mov	r0, r7
    c780:	e3a01000 	mov	r1, #0	; 0x0
    c784:	e3a02080 	mov	r2, #128	; 0x80
    c788:	ebffd2e7 	bl	132c <memset>
    c78c:	e5993000 	ldr	r3, [r9]
    c790:	e3530000 	cmp	r3, #0	; 0x0
    c794:	0a00002a 	beq	c844 <linkpc_loop+0x144>
    c798:	e28d400c 	add	r4, sp, #12	; 0xc
    c79c:	e3a02080 	mov	r2, #128	; 0x80
    c7a0:	e3a01000 	mov	r1, #0	; 0x0
    c7a4:	e1a00004 	mov	r0, r4
    c7a8:	ebffd2df 	bl	132c <memset>
    c7ac:	e59f0550 	ldr	r0, [pc, #1360]	; cd04 <linkpc_loop+0x604>
    c7b0:	e59f1550 	ldr	r1, [pc, #1360]	; cd08 <linkpc_loop+0x608>
    c7b4:	e1a0e00f 	mov	lr, pc
    c7b8:	e59af000 	ldr	pc, [sl]
    c7bc:	e59fc53c 	ldr	ip, [pc, #1340]	; cd00 <linkpc_loop+0x600>
    c7c0:	e5990000 	ldr	r0, [r9]
    c7c4:	e59c3000 	ldr	r3, [ip]
    c7c8:	e0632000 	rsb	r2, r3, r0
    c7cc:	e1550002 	cmp	r5, r2
    c7d0:	90833005 	addls	r3, r3, r5
    c7d4:	80833002 	addhi	r3, r3, r2
    c7d8:	e1530000 	cmp	r3, r0
    c7dc:	e58c3000 	str	r3, [ip]
    c7e0:	3affffda 	bcc	c750 <linkpc_loop+0x50>
    c7e4:	e59f3520 	ldr	r3, [pc, #1312]	; cd0c <linkpc_loop+0x60c>
    c7e8:	e5933000 	ldr	r3, [r3]
    c7ec:	e3530000 	cmp	r3, #0	; 0x0
    c7f0:	e59f3518 	ldr	r3, [pc, #1304]	; cd10 <linkpc_loop+0x610>
    c7f4:	159f1518 	ldrne	r1, [pc, #1304]	; cd14 <linkpc_loop+0x614>
    c7f8:	059f1518 	ldreq	r1, [pc, #1304]	; cd18 <linkpc_loop+0x618>
    c7fc:	e5830000 	str	r0, [r3]
    c800:	e3a03000 	mov	r3, #0	; 0x0
    c804:	11a00004 	movne	r0, r4
    c808:	01a00004 	moveq	r0, r4
    c80c:	e28d400c 	add	r4, sp, #12	; 0xc
    c810:	e5893000 	str	r3, [r9]
    c814:	eb0005c3 	bl	df28 <sprintf>
    c818:	e1a00004 	mov	r0, r4
    c81c:	ebffd2b0 	bl	12e4 <strlen>
    c820:	e1a01000 	mov	r1, r0
    c824:	e1a00004 	mov	r0, r4
    c828:	ebfffeb1 	bl	c2f4 <udc_write>
    c82c:	e59f24cc 	ldr	r2, [pc, #1228]	; cd00 <linkpc_loop+0x600>
    c830:	e59f04e4 	ldr	r0, [pc, #1252]	; cd1c <linkpc_loop+0x61c>
    c834:	e5921000 	ldr	r1, [r2]
    c838:	e1a0e00f 	mov	lr, pc
    c83c:	e59af000 	ldr	pc, [sl]
    c840:	eaffffc2 	b	c750 <linkpc_loop+0x50>
    c844:	e1a01004 	mov	r1, r4
    c848:	e59f04b4 	ldr	r0, [pc, #1204]	; cd04 <linkpc_loop+0x604>
    c84c:	e1a0e00f 	mov	lr, pc
    c850:	e59af000 	ldr	pc, [sl]
    c854:	e59f14c4 	ldr	r1, [pc, #1220]	; cd20 <linkpc_loop+0x620>
    c858:	e1a00007 	mov	r0, r7
    c85c:	eb0005b1 	bl	df28 <sprintf>
    c860:	e1a00004 	mov	r0, r4
    c864:	e59f14b8 	ldr	r1, [pc, #1208]	; cd24 <linkpc_loop+0x624>
    c868:	e3a02006 	mov	r2, #6	; 0x6
    c86c:	ebffd27d 	bl	1268 <strncmp>
    c870:	e3500000 	cmp	r0, #0	; 0x0
    c874:	1a000002 	bne	c884 <linkpc_loop+0x184>
    c878:	ebffff8c 	bl	c6b0 <__response_ok>
    c87c:	ebffcfe7 	bl	820 <system_reset>
    c880:	ea000105 	b	cc9c <linkpc_loop+0x59c>
    c884:	e1a00004 	mov	r0, r4
    c888:	e59f1498 	ldr	r1, [pc, #1176]	; cd28 <linkpc_loop+0x628>
    c88c:	e3a02007 	mov	r2, #7	; 0x7
    c890:	ebffd274 	bl	1268 <strncmp>
    c894:	e3500000 	cmp	r0, #0	; 0x0
    c898:	1a00002d 	bne	c954 <linkpc_loop+0x254>
    c89c:	e2844007 	add	r4, r4, #7	; 0x7
    c8a0:	e1a02004 	mov	r2, r4
    c8a4:	e59f1480 	ldr	r1, [pc, #1152]	; cd2c <linkpc_loop+0x62c>
    c8a8:	e59f0480 	ldr	r0, [pc, #1152]	; cd30 <linkpc_loop+0x630>
    c8ac:	e1a0e00f 	mov	lr, pc
    c8b0:	e59af000 	ldr	pc, [sl]
    c8b4:	e59f145c 	ldr	r1, [pc, #1116]	; cd18 <linkpc_loop+0x618>
    c8b8:	e1a00007 	mov	r0, r7
    c8bc:	ebffd231 	bl	1188 <strcpy>
    c8c0:	e59f146c 	ldr	r1, [pc, #1132]	; cd34 <linkpc_loop+0x634>
    c8c4:	e1a00004 	mov	r0, r4
    c8c8:	e3a02007 	mov	r2, #7	; 0x7
    c8cc:	ebffd265 	bl	1268 <strncmp>
    c8d0:	e3500000 	cmp	r0, #0	; 0x0
    c8d4:	01a0000b 	moveq	r0, fp
    c8d8:	059f1458 	ldreq	r1, [pc, #1112]	; cd38 <linkpc_loop+0x638>
    c8dc:	0a000010 	beq	c924 <linkpc_loop+0x224>
    c8e0:	e59f1454 	ldr	r1, [pc, #1108]	; cd3c <linkpc_loop+0x63c>
    c8e4:	e1a00004 	mov	r0, r4
    c8e8:	e3a02005 	mov	r2, #5	; 0x5
    c8ec:	ebffd25d 	bl	1268 <strncmp>
    c8f0:	e3500000 	cmp	r0, #0	; 0x0
    c8f4:	01a0000b 	moveq	r0, fp
    c8f8:	059f1440 	ldreq	r1, [pc, #1088]	; cd40 <linkpc_loop+0x640>
    c8fc:	0a000008 	beq	c924 <linkpc_loop+0x224>
    c900:	e1a00004 	mov	r0, r4
    c904:	e59f1438 	ldr	r1, [pc, #1080]	; cd44 <linkpc_loop+0x644>
    c908:	e3a02008 	mov	r2, #8	; 0x8
    c90c:	ebffd255 	bl	1268 <strncmp>
    c910:	e3500000 	cmp	r0, #0	; 0x0
    c914:	1a000004 	bne	c92c <linkpc_loop+0x22c>
    c918:	e59f3428 	ldr	r3, [pc, #1064]	; cd48 <linkpc_loop+0x648>
    c91c:	e1a0000b 	mov	r0, fp
    c920:	e5931008 	ldr	r1, [r3, #8]
    c924:	ebffd217 	bl	1188 <strcpy>
    c928:	ea0000db 	b	cc9c <linkpc_loop+0x59c>
    c92c:	e1a00004 	mov	r0, r4
    c930:	e59f1414 	ldr	r1, [pc, #1044]	; cd4c <linkpc_loop+0x64c>
    c934:	e3a0200c 	mov	r2, #12	; 0xc
    c938:	ebffd24a 	bl	1268 <strncmp>
    c93c:	e3500000 	cmp	r0, #0	; 0x0
    c940:	1a0000d5 	bne	cc9c <linkpc_loop+0x59c>
    c944:	e1a0000b 	mov	r0, fp
    c948:	e59f1400 	ldr	r1, [pc, #1024]	; cd50 <linkpc_loop+0x650>
    c94c:	e3a02903 	mov	r2, #49152	; 0xc000
    c950:	ea000020 	b	c9d8 <linkpc_loop+0x2d8>
    c954:	e1a00004 	mov	r0, r4
    c958:	e59f13f4 	ldr	r1, [pc, #1012]	; cd54 <linkpc_loop+0x654>
    c95c:	e3a02009 	mov	r2, #9	; 0x9
    c960:	ebffd240 	bl	1268 <strncmp>
    c964:	e2505000 	subs	r5, r0, #0	; 0x0
    c968:	1a00001c 	bne	c9e0 <linkpc_loop+0x2e0>
    c96c:	e59f3398 	ldr	r3, [pc, #920]	; cd0c <linkpc_loop+0x60c>
    c970:	e59fc388 	ldr	ip, [pc, #904]	; cd00 <linkpc_loop+0x600>
    c974:	e3a02010 	mov	r2, #16	; 0x10
    c978:	e1a01005 	mov	r1, r5
    c97c:	e5835000 	str	r5, [r3]
    c980:	e58c5000 	str	r5, [ip]
    c984:	e2840009 	add	r0, r4, #9	; 0x9
    c988:	eb0001da 	bl	d0f8 <simple_strtoul>
    c98c:	e1a01000 	mov	r1, r0
    c990:	e5890000 	str	r0, [r9]
    c994:	e59f03bc 	ldr	r0, [pc, #956]	; cd58 <linkpc_loop+0x658>
    c998:	e1a0e00f 	mov	lr, pc
    c99c:	e59af000 	ldr	pc, [sl]
    c9a0:	e5992000 	ldr	r2, [r9]
    c9a4:	e3520000 	cmp	r2, #0	; 0x0
    c9a8:	01a00007 	moveq	r0, r7
    c9ac:	059f13a8 	ldreq	r1, [pc, #936]	; cd5c <linkpc_loop+0x65c>
    c9b0:	0a0000b8 	beq	cc98 <linkpc_loop+0x598>
    c9b4:	e3520903 	cmp	r2, #49152	; 0xc000
    c9b8:	91a00007 	movls	r0, r7
    c9bc:	959f139c 	ldrls	r1, [pc, #924]	; cd60 <linkpc_loop+0x660>
    c9c0:	9a000004 	bls	c9d8 <linkpc_loop+0x2d8>
    c9c4:	e1a00007 	mov	r0, r7
    c9c8:	e59f1394 	ldr	r1, [pc, #916]	; cd64 <linkpc_loop+0x664>
    c9cc:	eb000555 	bl	df28 <sprintf>
    c9d0:	e5895000 	str	r5, [r9]
    c9d4:	ea0000b0 	b	cc9c <linkpc_loop+0x59c>
    c9d8:	eb000552 	bl	df28 <sprintf>
    c9dc:	ea0000ae 	b	cc9c <linkpc_loop+0x59c>
    c9e0:	e1a00004 	mov	r0, r4
    c9e4:	e59f137c 	ldr	r1, [pc, #892]	; cd68 <linkpc_loop+0x668>
    c9e8:	e3a02006 	mov	r2, #6	; 0x6
    c9ec:	ebffd21d 	bl	1268 <strncmp>
    c9f0:	e3500000 	cmp	r0, #0	; 0x0
    c9f4:	1a000065 	bne	cb90 <linkpc_loop+0x490>
    c9f8:	e2844006 	add	r4, r4, #6	; 0x6
    c9fc:	e1a00004 	mov	r0, r4
    ca00:	e59f1364 	ldr	r1, [pc, #868]	; cd6c <linkpc_loop+0x66c>
    ca04:	e3a02003 	mov	r2, #3	; 0x3
    ca08:	ebffd216 	bl	1268 <strncmp>
    ca0c:	e3500000 	cmp	r0, #0	; 0x0
    ca10:	1a000007 	bne	ca34 <linkpc_loop+0x334>
    ca14:	e2800302 	add	r0, r0, #134217728	; 0x8000000
    ca18:	ebffe5fa 	bl	6208 <cdbg_config_isi>
    ca1c:	e3500000 	cmp	r0, #0	; 0x0
    ca20:	11a00007 	movne	r0, r7
    ca24:	159f1344 	ldrne	r1, [pc, #836]	; cd70 <linkpc_loop+0x670>
    ca28:	01a00007 	moveq	r0, r7
    ca2c:	059f12e4 	ldreq	r1, [pc, #740]	; cd18 <linkpc_loop+0x618>
    ca30:	ea000098 	b	cc98 <linkpc_loop+0x598>
    ca34:	e1a00004 	mov	r0, r4
    ca38:	ebffe0ca 	bl	4d68 <vs_device_id>
    ca3c:	e59f6330 	ldr	r6, [pc, #816]	; cd74 <linkpc_loop+0x674>
    ca40:	e59fc330 	ldr	ip, [pc, #816]	; cd78 <linkpc_loop+0x678>
    ca44:	e5963000 	ldr	r3, [r6]
    ca48:	e1a05000 	mov	r5, r0
    ca4c:	e58d3000 	str	r3, [sp]
    ca50:	e59f0324 	ldr	r0, [pc, #804]	; cd7c <linkpc_loop+0x67c>
    ca54:	e59f1324 	ldr	r1, [pc, #804]	; cd80 <linkpc_loop+0x680>
    ca58:	e1a02005 	mov	r2, r5
    ca5c:	e59c3000 	ldr	r3, [ip]
    ca60:	e1a0e00f 	mov	lr, pc
    ca64:	e59af000 	ldr	pc, [sl]
    ca68:	e355000e 	cmp	r5, #14	; 0xe
    ca6c:	ca000006 	bgt	ca8c <linkpc_loop+0x38c>
    ca70:	e1a00005 	mov	r0, r5
    ca74:	ebffe0a7 	bl	4d18 <vs_device_erasable>
    ca78:	e3500000 	cmp	r0, #0	; 0x0
    ca7c:	0a000005 	beq	ca98 <linkpc_loop+0x398>
    ca80:	e5963000 	ldr	r3, [r6]
    ca84:	e3530000 	cmp	r3, #0	; 0x0
    ca88:	1a000002 	bne	ca98 <linkpc_loop+0x398>
    ca8c:	e1a00007 	mov	r0, r7
    ca90:	e59f12ec 	ldr	r1, [pc, #748]	; cd84 <linkpc_loop+0x684>
    ca94:	ea00007f 	b	cc98 <linkpc_loop+0x598>
    ca98:	e3a00901 	mov	r0, #16384	; 0x4000
    ca9c:	ebffd28f 	bl	14e0 <malloc>
    caa0:	e2508000 	subs	r8, r0, #0	; 0x0
    caa4:	01a00007 	moveq	r0, r7
    caa8:	059f12d8 	ldreq	r1, [pc, #728]	; cd88 <linkpc_loop+0x688>
    caac:	0a000079 	beq	cc98 <linkpc_loop+0x598>
    cab0:	e3a01001 	mov	r1, #1	; 0x1
    cab4:	e1a00005 	mov	r0, r5
    cab8:	ebffe17e 	bl	50b8 <vs_assign_by_id>
    cabc:	e3500000 	cmp	r0, #0	; 0x0
    cac0:	b1a00007 	movlt	r0, r7
    cac4:	b59f12c0 	ldrlt	r1, [pc, #704]	; cd8c <linkpc_loop+0x68c>
    cac8:	ba00002c 	blt	cb80 <linkpc_loop+0x480>
    cacc:	e355000d 	cmp	r5, #13	; 0xd
    cad0:	13a06002 	movne	r6, #2	; 0x2
    cad4:	03a06001 	moveq	r6, #1	; 0x1
    cad8:	e1a04c06 	lsl	r4, r6, #24
    cadc:	e3a00302 	mov	r0, #134217728	; 0x8000000
    cae0:	03a0500f 	moveq	r5, #15	; 0xf
    cae4:	e58d414c 	str	r4, [sp, #332]
    cae8:	ebfffbcb 	bl	ba1c <ram_addr2offs>
    caec:	e3a024ff 	mov	r2, #-16777216	; 0xff000000
    caf0:	e0922000 	adds	r2, r2, r0
    caf4:	e3e03000 	mvn	r3, #0	; 0x0
    caf8:	e0a33001 	adc	r3, r3, r1
    cafc:	e59f1270 	ldr	r1, [pc, #624]	; cd74 <linkpc_loop+0x674>
    cb00:	e59fc270 	ldr	ip, [pc, #624]	; cd78 <linkpc_loop+0x678>
    cb04:	e1d100b2 	ldrh	r0, [r1, #2]
    cb08:	e1a014a2 	lsr	r1, r2, #9
    cb0c:	e1811b83 	orr	r1, r1, r3, lsl #23
    cb10:	e59f31f8 	ldr	r3, [pc, #504]	; cd10 <linkpc_loop+0x610>
    cb14:	e1844001 	orr	r4, r4, r1
    cb18:	e5932000 	ldr	r2, [r3]
    cb1c:	e59c3000 	ldr	r3, [ip]
    cb20:	e3560001 	cmp	r6, #1	; 0x1
    cb24:	e1a0c800 	lsl	ip, r0, #16
    cb28:	e58d414c 	str	r4, [sp, #332]
    cb2c:	e58d3150 	str	r3, [sp, #336]
    cb30:	e58d2158 	str	r2, [sp, #344]
    cb34:	e58dc154 	str	ip, [sp, #340]
    cb38:	1a000004 	bne	cb50 <linkpc_loop+0x450>
    cb3c:	ebfffedb 	bl	c6b0 <__response_ok>
    cb40:	e59d0150 	ldr	r0, [sp, #336]
    cb44:	e3a01000 	mov	r1, #0	; 0x0
    cb48:	ebffe539 	bl	6034 <cdbg_jump>
    cb4c:	eafffffe 	b	cb4c <linkpc_loop+0x44c>
    cb50:	e3a0100d 	mov	r1, #13	; 0xd
    cb54:	e1a03005 	mov	r3, r5
    cb58:	e28d0f53 	add	r0, sp, #332	; 0x14c
    cb5c:	e3a02000 	mov	r2, #0	; 0x0
    cb60:	e58dc000 	str	ip, [sp]
    cb64:	e58d8004 	str	r8, [sp, #4]
    cb68:	ebffe5bb 	bl	625c <burn_image_loop>
    cb6c:	e3500000 	cmp	r0, #0	; 0x0
    cb70:	159f1218 	ldrne	r1, [pc, #536]	; cd90 <linkpc_loop+0x690>
    cb74:	059f119c 	ldreq	r1, [pc, #412]	; cd18 <linkpc_loop+0x618>
    cb78:	11a00007 	movne	r0, r7
    cb7c:	01a00007 	moveq	r0, r7
    cb80:	eb0004e8 	bl	df28 <sprintf>
    cb84:	e1a00008 	mov	r0, r8
    cb88:	ebffd2bc 	bl	1680 <free>
    cb8c:	ea000042 	b	cc9c <linkpc_loop+0x59c>
    cb90:	e1a00004 	mov	r0, r4
    cb94:	e59f11f8 	ldr	r1, [pc, #504]	; cd94 <linkpc_loop+0x694>
    cb98:	e3a02008 	mov	r2, #8	; 0x8
    cb9c:	ebffd1b1 	bl	1268 <strncmp>
    cba0:	e3500000 	cmp	r0, #0	; 0x0
    cba4:	1a000003 	bne	cbb8 <linkpc_loop+0x4b8>
    cba8:	ebfffec0 	bl	c6b0 <__response_ok>
    cbac:	e3a00001 	mov	r0, #1	; 0x1
    cbb0:	ebffce0c 	bl	3e8 <try_boot>
    cbb4:	ea000038 	b	cc9c <linkpc_loop+0x59c>
    cbb8:	e1a00004 	mov	r0, r4
    cbbc:	e59f11d4 	ldr	r1, [pc, #468]	; cd98 <linkpc_loop+0x698>
    cbc0:	e3a0200a 	mov	r2, #10	; 0xa
    cbc4:	ebffd1a7 	bl	1268 <strncmp>
    cbc8:	e2505000 	subs	r5, r0, #0	; 0x0
    cbcc:	1a000019 	bne	cc38 <linkpc_loop+0x538>
    cbd0:	e28d1f57 	add	r1, sp, #348	; 0x15c
    cbd4:	e284000b 	add	r0, r4, #11	; 0xb
    cbd8:	e3a02010 	mov	r2, #16	; 0x10
    cbdc:	eb000145 	bl	d0f8 <simple_strtoul>
    cbe0:	e59f2190 	ldr	r2, [pc, #400]	; cd78 <linkpc_loop+0x678>
    cbe4:	e1a01005 	mov	r1, r5
    cbe8:	e5820000 	str	r0, [r2]
    cbec:	e59d015c 	ldr	r0, [sp, #348]
    cbf0:	e3a02010 	mov	r2, #16	; 0x10
    cbf4:	e2800001 	add	r0, r0, #1	; 0x1
    cbf8:	eb00013e 	bl	d0f8 <simple_strtoul>
    cbfc:	e59f4170 	ldr	r4, [pc, #368]	; cd74 <linkpc_loop+0x674>
    cc00:	e59f3170 	ldr	r3, [pc, #368]	; cd78 <linkpc_loop+0x678>
    cc04:	e1a02000 	mov	r2, r0
    cc08:	e5840000 	str	r0, [r4]
    cc0c:	e5931000 	ldr	r1, [r3]
    cc10:	e59f0184 	ldr	r0, [pc, #388]	; cd9c <linkpc_loop+0x69c>
    cc14:	e1a0e00f 	mov	lr, pc
    cc18:	e59af000 	ldr	pc, [sl]
    cc1c:	e59fc154 	ldr	ip, [pc, #340]	; cd78 <linkpc_loop+0x678>
    cc20:	e1a00007 	mov	r0, r7
    cc24:	e5943000 	ldr	r3, [r4]
    cc28:	e59f1170 	ldr	r1, [pc, #368]	; cda0 <linkpc_loop+0x6a0>
    cc2c:	e59c2000 	ldr	r2, [ip]
    cc30:	eb0004bc 	bl	df28 <sprintf>
    cc34:	ea000018 	b	cc9c <linkpc_loop+0x59c>
    cc38:	e1a00004 	mov	r0, r4
    cc3c:	e59f1160 	ldr	r1, [pc, #352]	; cda4 <linkpc_loop+0x6a4>
    cc40:	e3a0200d 	mov	r2, #13	; 0xd
    cc44:	ebffd187 	bl	1268 <strncmp>
    cc48:	e3500000 	cmp	r0, #0	; 0x0
    cc4c:	1a000006 	bne	cc6c <linkpc_loop+0x56c>
    cc50:	e59f20f0 	ldr	r2, [pc, #240]	; cd48 <linkpc_loop+0x648>
    cc54:	e284100e 	add	r1, r4, #14	; 0xe
    cc58:	e5920008 	ldr	r0, [r2, #8]
    cc5c:	ebffd149 	bl	1188 <strcpy>
    cc60:	ebfffe92 	bl	c6b0 <__response_ok>
    cc64:	ebfffea5 	bl	c700 <linkpc_loop>
    cc68:	ea00000b 	b	cc9c <linkpc_loop+0x59c>
    cc6c:	e1a00004 	mov	r0, r4
    cc70:	e59f1130 	ldr	r1, [pc, #304]	; cda8 <linkpc_loop+0x6a8>
    cc74:	e3a02007 	mov	r2, #7	; 0x7
    cc78:	ebffd17a 	bl	1268 <strncmp>
    cc7c:	e3500000 	cmp	r0, #0	; 0x0
    cc80:	1a000002 	bne	cc90 <linkpc_loop+0x590>
    cc84:	ebfffe89 	bl	c6b0 <__response_ok>
    cc88:	ebffe51b 	bl	60fc <cdbg_shell>
    cc8c:	eafffffe 	b	cc8c <linkpc_loop+0x58c>
    cc90:	e59f1114 	ldr	r1, [pc, #276]	; cdac <linkpc_loop+0x6ac>
    cc94:	e1a00007 	mov	r0, r7
    cc98:	eb0004a2 	bl	df28 <sprintf>
    cc9c:	e3a01c01 	mov	r1, #256	; 0x100
    cca0:	e1a00007 	mov	r0, r7
    cca4:	ebffd196 	bl	1304 <strnlen>
    cca8:	e1a01000 	mov	r1, r0
    ccac:	e1a00007 	mov	r0, r7
    ccb0:	ebfffd8f 	bl	c2f4 <udc_write>
    ccb4:	e3500000 	cmp	r0, #0	; 0x0
    ccb8:	0afffea4 	beq	c750 <linkpc_loop+0x50>
    ccbc:	e59f00ec 	ldr	r0, [pc, #236]	; cdb0 <linkpc_loop+0x6b0>
    ccc0:	e59f3028 	ldr	r3, [pc, #40]	; ccf0 <linkpc_loop+0x5f0>
    ccc4:	e1a0e00f 	mov	lr, pc
    ccc8:	e593f000 	ldr	pc, [r3]
    cccc:	e3a04000 	mov	r4, #0	; 0x0
    ccd0:	e59f00dc 	ldr	r0, [pc, #220]	; cdb4 <linkpc_loop+0x6b4>
    ccd4:	e59f3014 	ldr	r3, [pc, #20]	; ccf0 <linkpc_loop+0x5f0>
    ccd8:	e1a0e00f 	mov	lr, pc
    ccdc:	e593f000 	ldr	pc, [r3]
    cce0:	e1a00004 	mov	r0, r4
    cce4:	e28ddf59 	add	sp, sp, #356	; 0x164
    cce8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ccec:	00013f8e 	.word	0x00013f8e
    ccf0:	0800d76c 	.word	0x0800d76c
    ccf4:	000122b6 	.word	0x000122b6
    ccf8:	00013f9d 	.word	0x00013f9d
    ccfc:	0800d2f0 	.word	0x0800d2f0
    cd00:	0800d2f4 	.word	0x0800d2f4
    cd04:	00011d14 	.word	0x00011d14
    cd08:	00011887 	.word	0x00011887
    cd0c:	0800d2f8 	.word	0x0800d2f8
    cd10:	0800d2fc 	.word	0x0800d2fc
    cd14:	00013fb0 	.word	0x00013fb0
    cd18:	00011867 	.word	0x00011867
    cd1c:	00013fb6 	.word	0x00013fb6
    cd20:	00013fd9 	.word	0x00013fd9
    cd24:	00013fde 	.word	0x00013fde
    cd28:	00013fe5 	.word	0x00013fe5
    cd2c:	00011893 	.word	0x00011893
    cd30:	00013fed 	.word	0x00013fed
    cd34:	00013ff5 	.word	0x00013ff5
    cd38:	0800d10c 	.word	0x0800d10c
    cd3c:	00013ffd 	.word	0x00013ffd
    cd40:	0800d18c 	.word	0x0800d18c
    cd44:	00014003 	.word	0x00014003
    cd48:	0800d7d4 	.word	0x0800d7d4
    cd4c:	0001400c 	.word	0x0001400c
    cd50:	0001406f 	.word	0x0001406f
    cd54:	00014019 	.word	0x00014019
    cd58:	00014023 	.word	0x00014023
    cd5c:	00014042 	.word	0x00014042
    cd60:	00014074 	.word	0x00014074
    cd64:	0001405b 	.word	0x0001405b
    cd68:	0001407d 	.word	0x0001407d
    cd6c:	0001213e 	.word	0x0001213e
    cd70:	00014084 	.word	0x00014084
    cd74:	0800d304 	.word	0x0800d304
    cd78:	0800d300 	.word	0x0800d300
    cd7c:	0001409c 	.word	0x0001409c
    cd80:	000118a0 	.word	0x000118a0
    cd84:	000140ac 	.word	0x000140ac
    cd88:	000140c3 	.word	0x000140c3
    cd8c:	000140d1 	.word	0x000140d1
    cd90:	000140ea 	.word	0x000140ea
    cd94:	000140ff 	.word	0x000140ff
    cd98:	00014108 	.word	0x00014108
    cd9c:	00014113 	.word	0x00014113
    cda0:	0001412b 	.word	0x0001412b
    cda4:	00014146 	.word	0x00014146
    cda8:	00014154 	.word	0x00014154
    cdac:	0001415c 	.word	0x0001415c
    cdb0:	00014170 	.word	0x00014170
    cdb4:	00014193 	.word	0x00014193

0000cdb8 <print_size>:
    cdb8:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    cdbc:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    cdc0:	e1a05000 	mov	r5, r0
    cdc4:	e1a08001 	mov	r8, r1
    cdc8:	83a04101 	movhi	r4, #1073741824	; 0x40000000
    cdcc:	83a07047 	movhi	r7, #71	; 0x47
    cdd0:	8a000005 	bhi	cdec <print_size+0x34>
    cdd4:	e59f30a8 	ldr	r3, [pc, #168]	; ce84 <print_size+0xcc>
    cdd8:	e1500003 	cmp	r0, r3
    cddc:	83a0704d 	movhi	r7, #77	; 0x4d
    cde0:	93a0706b 	movls	r7, #107	; 0x6b
    cde4:	83a04601 	movhi	r4, #1048576	; 0x100000
    cde8:	93a04b01 	movls	r4, #1024	; 0x400
    cdec:	e1a01004 	mov	r1, r4
    cdf0:	e1a00005 	mov	r0, r5
    cdf4:	eb000cc3 	bl	10108 <__aeabi_uidiv>
    cdf8:	e1a01004 	mov	r1, r4
    cdfc:	e1a06000 	mov	r6, r0
    ce00:	e1a00005 	mov	r0, r5
    ce04:	eb000d3b 	bl	102f8 <__aeabi_uidivmod>
    ce08:	e2510000 	subs	r0, r1, #0	; 0x0
    ce0c:	01a04000 	moveq	r4, r0
    ce10:	0a000009 	beq	ce3c <print_size+0x84>
    ce14:	e0625694 	mls	r2, r4, r6, r5
    ce18:	e1a030a4 	lsr	r3, r4, #1
    ce1c:	e3a0000a 	mov	r0, #10	; 0xa
    ce20:	e1a01004 	mov	r1, r4
    ce24:	e0203290 	mla	r0, r0, r2, r3
    ce28:	eb000cb6 	bl	10108 <__aeabi_uidiv>
    ce2c:	e3500009 	cmp	r0, #9	; 0x9
    ce30:	e1a04000 	mov	r4, r0
    ce34:	82866001 	addhi	r6, r6, #1	; 0x1
    ce38:	8240400a 	subhi	r4, r0, #10	; 0xa
    ce3c:	e1a01006 	mov	r1, r6
    ce40:	e59f5040 	ldr	r5, [pc, #64]	; ce88 <print_size+0xd0>
    ce44:	e59f0040 	ldr	r0, [pc, #64]	; ce8c <print_size+0xd4>
    ce48:	e1a0e00f 	mov	lr, pc
    ce4c:	e595f000 	ldr	pc, [r5]
    ce50:	e3540000 	cmp	r4, #0	; 0x0
    ce54:	0a000003 	beq	ce68 <print_size+0xb0>
    ce58:	e1a01004 	mov	r1, r4
    ce5c:	e59f002c 	ldr	r0, [pc, #44]	; ce90 <print_size+0xd8>
    ce60:	e1a0e00f 	mov	lr, pc
    ce64:	e595f000 	ldr	pc, [r5]
    ce68:	e1a01007 	mov	r1, r7
    ce6c:	e1a02008 	mov	r2, r8
    ce70:	e59f001c 	ldr	r0, [pc, #28]	; ce94 <print_size+0xdc>
    ce74:	e59f300c 	ldr	r3, [pc, #12]	; ce88 <print_size+0xd0>
    ce78:	e1a0e00f 	mov	lr, pc
    ce7c:	e593f000 	ldr	pc, [r3]
    ce80:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    ce84:	000fffff 	.word	0x000fffff
    ce88:	0800d76c 	.word	0x0800d76c
    ce8c:	0001419b 	.word	0x0001419b
    ce90:	000141a0 	.word	0x000141a0
    ce94:	000141a5 	.word	0x000141a5

0000ce98 <print_buffer>:
    ce98:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ce9c:	e24dd044 	sub	sp, sp, #68	; 0x44
    cea0:	e59d6068 	ldr	r6, [sp, #104]
    cea4:	e1a07002 	mov	r7, r2
    cea8:	e0020692 	mul	r2, r2, r6
    ceac:	e3520040 	cmp	r2, #64	; 0x40
    ceb0:	e1a09000 	mov	r9, r0
    ceb4:	e1a0a001 	mov	sl, r1
    ceb8:	e1a08003 	mov	r8, r3
    cebc:	9a000003 	bls	ced0 <print_buffer+0x38>
    cec0:	e3a00040 	mov	r0, #64	; 0x40
    cec4:	e1a01007 	mov	r1, r7
    cec8:	eb000c8e 	bl	10108 <__aeabi_uidiv>
    cecc:	e1a06000 	mov	r6, r0
    ced0:	e3560000 	cmp	r6, #0	; 0x0
    ced4:	1a00003c 	bne	cfcc <print_buffer+0x134>
    ced8:	e3a00010 	mov	r0, #16	; 0x10
    cedc:	e1a01007 	mov	r1, r7
    cee0:	eb000c88 	bl	10108 <__aeabi_uidiv>
    cee4:	e1a06000 	mov	r6, r0
    cee8:	ea000037 	b	cfcc <print_buffer+0x134>
    ceec:	e59f00f0 	ldr	r0, [pc, #240]	; cfe4 <print_buffer+0x14c>
    cef0:	e1a01009 	mov	r1, r9
    cef4:	e1a0e00f 	mov	lr, pc
    cef8:	e59bf000 	ldr	pc, [fp]
    cefc:	e1560008 	cmp	r6, r8
    cf00:	21a06008 	movcs	r6, r8
    cf04:	e1a0500a 	mov	r5, sl
    cf08:	e3a04000 	mov	r4, #0	; 0x0
    cf0c:	ea00000f 	b	cf50 <print_buffer+0xb8>
    cf10:	e3570004 	cmp	r7, #4	; 0x4
    cf14:	05951000 	ldreq	r1, [r5]
    cf18:	059f00c8 	ldreq	r0, [pc, #200]	; cfe8 <print_buffer+0x150>
    cf1c:	078d1104 	streq	r1, [sp, r4, lsl #2]
    cf20:	0a000006 	beq	cf40 <print_buffer+0xa8>
    cf24:	e3570002 	cmp	r7, #2	; 0x2
    cf28:	01d510b0 	ldrheq	r1, [r5]
    cf2c:	15d51000 	ldrbne	r1, [r5]
    cf30:	01a03084 	lsleq	r3, r4, #1
    cf34:	059f00b0 	ldreq	r0, [pc, #176]	; cfec <print_buffer+0x154>
    cf38:	018d10b3 	strheq	r1, [sp, r3]
    cf3c:	17cd1004 	strbne	r1, [sp, r4]
    cf40:	e1a0e00f 	mov	lr, pc
    cf44:	e59bf000 	ldr	pc, [fp]
    cf48:	e0855007 	add	r5, r5, r7
    cf4c:	e2844001 	add	r4, r4, #1	; 0x1
    cf50:	e1540006 	cmp	r4, r6
    cf54:	e59f0094 	ldr	r0, [pc, #148]	; cff0 <print_buffer+0x158>
    cf58:	3affffec 	bcc	cf10 <print_buffer+0x78>
    cf5c:	e0050697 	mul	r5, r7, r6
    cf60:	e59f008c 	ldr	r0, [pc, #140]	; cff4 <print_buffer+0x15c>
    cf64:	e08aa005 	add	sl, sl, r5
    cf68:	eb000450 	bl	e0b0 <puts>
    cf6c:	e3a04000 	mov	r4, #0	; 0x0
    cf70:	ea000009 	b	cf9c <print_buffer+0x104>
    cf74:	e7dd0004 	ldrb	r0, [sp, r4]
    cf78:	e59f2078 	ldr	r2, [pc, #120]	; cff8 <print_buffer+0x160>
    cf7c:	e2844001 	add	r4, r4, #1	; 0x1
    cf80:	e7d23000 	ldrb	r3, [r2, r0]
    cf84:	e3130097 	tst	r3, #151	; 0x97
    cf88:	0a000001 	beq	cf94 <print_buffer+0xfc>
    cf8c:	e3100080 	tst	r0, #128	; 0x80
    cf90:	0a000000 	beq	cf98 <print_buffer+0x100>
    cf94:	e3a0002e 	mov	r0, #46	; 0x2e
    cf98:	eb00046c 	bl	e150 <putc>
    cf9c:	e1540005 	cmp	r4, r5
    cfa0:	3afffff3 	bcc	cf74 <print_buffer+0xdc>
    cfa4:	e59f0050 	ldr	r0, [pc, #80]	; cffc <print_buffer+0x164>
    cfa8:	e1a0e00f 	mov	lr, pc
    cfac:	e59bf000 	ldr	pc, [fp]
    cfb0:	eb000469 	bl	e15c <ctrlc>
    cfb4:	e3500000 	cmp	r0, #0	; 0x0
    cfb8:	13e00000 	mvnne	r0, #0	; 0x0
    cfbc:	1a000006 	bne	cfdc <print_buffer+0x144>
    cfc0:	e0899005 	add	r9, r9, r5
    cfc4:	e0668008 	rsb	r8, r6, r8
    cfc8:	ea000000 	b	cfd0 <print_buffer+0x138>
    cfcc:	e59fb02c 	ldr	fp, [pc, #44]	; d000 <print_buffer+0x168>
    cfd0:	e3580000 	cmp	r8, #0	; 0x0
    cfd4:	1affffc4 	bne	ceec <print_buffer+0x54>
    cfd8:	e1a00008 	mov	r0, r8
    cfdc:	e28dd044 	add	sp, sp, #68	; 0x44
    cfe0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    cfe4:	000141ac 	.word	0x000141ac
    cfe8:	000132d3 	.word	0x000132d3
    cfec:	000141b3 	.word	0x000141b3
    cff0:	000141b9 	.word	0x000141b9
    cff4:	000141bf 	.word	0x000141bf
    cff8:	000118ca 	.word	0x000118ca
    cffc:	0001247a 	.word	0x0001247a
    d000:	0800d76c 	.word	0x0800d76c

0000d004 <__div64_32>:
    d004:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    d008:	e8900300 	ldm	r0, {r8, r9}
    d00c:	e1510009 	cmp	r1, r9
    d010:	e1a0a000 	mov	sl, r0
    d014:	e1a04001 	mov	r4, r1
    d018:	83a06000 	movhi	r6, #0	; 0x0
    d01c:	83a07000 	movhi	r7, #0	; 0x0
    d020:	8a000007 	bhi	d044 <__div64_32+0x40>
    d024:	e1a00009 	mov	r0, r9
    d028:	eb000c36 	bl	10108 <__aeabi_uidiv>
    d02c:	e0030094 	mul	r3, r4, r0
    d030:	e3a06000 	mov	r6, #0	; 0x0
    d034:	e1a02006 	mov	r2, r6
    d038:	e1a07000 	mov	r7, r0
    d03c:	e0588002 	subs	r8, r8, r2
    d040:	e0c99003 	sbc	r9, r9, r3
    d044:	e1a00004 	mov	r0, r4
    d048:	e3a01000 	mov	r1, #0	; 0x0
    d04c:	e3a05000 	mov	r5, #0	; 0x0
    d050:	e3a04001 	mov	r4, #1	; 0x1
    d054:	ea000003 	b	d068 <__div64_32+0x64>
    d058:	e0900000 	adds	r0, r0, r0
    d05c:	e0a11001 	adc	r1, r1, r1
    d060:	e0944004 	adds	r4, r4, r4
    d064:	e0a55005 	adc	r5, r5, r5
    d068:	e3510000 	cmp	r1, #0	; 0x0
    d06c:	e3a03000 	mov	r3, #0	; 0x0
    d070:	ca000002 	bgt	d080 <__div64_32+0x7c>
    d074:	1a000002 	bne	d084 <__div64_32+0x80>
    d078:	e3500000 	cmp	r0, #0	; 0x0
    d07c:	0a000000 	beq	d084 <__div64_32+0x80>
    d080:	e3a03001 	mov	r3, #1	; 0x1
    d084:	e1590001 	cmp	r9, r1
    d088:	e3a02000 	mov	r2, #0	; 0x0
    d08c:	8a000002 	bhi	d09c <__div64_32+0x98>
    d090:	1a000002 	bne	d0a0 <__div64_32+0x9c>
    d094:	e1580000 	cmp	r8, r0
    d098:	9a000000 	bls	d0a0 <__div64_32+0x9c>
    d09c:	e3a02001 	mov	r2, #1	; 0x1
    d0a0:	e0033002 	and	r3, r3, r2
    d0a4:	e31300ff 	tst	r3, #255	; 0xff
    d0a8:	1affffea 	bne	d058 <__div64_32+0x54>
    d0ac:	e1510009 	cmp	r1, r9
    d0b0:	8a000006 	bhi	d0d0 <__div64_32+0xcc>
    d0b4:	1a000001 	bne	d0c0 <__div64_32+0xbc>
    d0b8:	e1500008 	cmp	r0, r8
    d0bc:	8a000003 	bhi	d0d0 <__div64_32+0xcc>
    d0c0:	e0588000 	subs	r8, r8, r0
    d0c4:	e0c99001 	sbc	r9, r9, r1
    d0c8:	e0966004 	adds	r6, r6, r4
    d0cc:	e0a77005 	adc	r7, r7, r5
    d0d0:	e1b050a5 	lsrs	r5, r5, #1
    d0d4:	e1a04064 	rrx	r4, r4
    d0d8:	e1943005 	orrs	r3, r4, r5
    d0dc:	0a000002 	beq	d0ec <__div64_32+0xe8>
    d0e0:	e1b010a1 	lsrs	r1, r1, #1
    d0e4:	e1a00060 	rrx	r0, r0
    d0e8:	eaffffef 	b	d0ac <__div64_32+0xa8>
    d0ec:	e88a00c0 	stm	sl, {r6, r7}
    d0f0:	e1a00008 	mov	r0, r8
    d0f4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

0000d0f8 <simple_strtoul>:
    d0f8:	e92d4070 	push	{r4, r5, r6, lr}
    d0fc:	e5d03000 	ldrb	r3, [r0]
    d100:	e1a05001 	mov	r5, r1
    d104:	e3530030 	cmp	r3, #48	; 0x30
    d108:	e1a01002 	mov	r1, r2
    d10c:	1a00000d 	bne	d148 <simple_strtoul+0x50>
    d110:	e5f03001 	ldrb	r3, [r0, #1]!
    d114:	e3530078 	cmp	r3, #120	; 0x78
    d118:	1a000007 	bne	d13c <simple_strtoul+0x44>
    d11c:	e5d03001 	ldrb	r3, [r0, #1]
    d120:	e59f2084 	ldr	r2, [pc, #132]	; d1ac <simple_strtoul+0xb4>
    d124:	e280c001 	add	ip, r0, #1	; 0x1
    d128:	e7d23003 	ldrb	r3, [r2, r3]
    d12c:	e3130044 	tst	r3, #68	; 0x44
    d130:	11a0000c 	movne	r0, ip
    d134:	13a01010 	movne	r1, #16	; 0x10
    d138:	1a000004 	bne	d150 <simple_strtoul+0x58>
    d13c:	e3510000 	cmp	r1, #0	; 0x0
    d140:	03a01008 	moveq	r1, #8	; 0x8
    d144:	ea000001 	b	d150 <simple_strtoul+0x58>
    d148:	e3520000 	cmp	r2, #0	; 0x0
    d14c:	03a0100a 	moveq	r1, #10	; 0xa
    d150:	e3a04000 	mov	r4, #0	; 0x0
    d154:	e59f6050 	ldr	r6, [pc, #80]	; d1ac <simple_strtoul+0xb4>
    d158:	ea000001 	b	d164 <simple_strtoul+0x6c>
    d15c:	e0243491 	mla	r4, r1, r4, r3
    d160:	e2800001 	add	r0, r0, #1	; 0x1
    d164:	e5d0c000 	ldrb	ip, [r0]
    d168:	e7d6200c 	ldrb	r2, [r6, ip]
    d16c:	e24c3030 	sub	r3, ip, #48	; 0x30
    d170:	e3120044 	tst	r2, #68	; 0x44
    d174:	0a000008 	beq	d19c <simple_strtoul+0xa4>
    d178:	e3120004 	tst	r2, #4	; 0x4
    d17c:	1a000004 	bne	d194 <simple_strtoul+0x9c>
    d180:	e24c3020 	sub	r3, ip, #32	; 0x20
    d184:	e3120002 	tst	r2, #2	; 0x2
    d188:	e6ef3073 	uxtb	r3, r3
    d18c:	12433037 	subne	r3, r3, #55	; 0x37
    d190:	024c3037 	subeq	r3, ip, #55	; 0x37
    d194:	e1530001 	cmp	r3, r1
    d198:	3affffef 	bcc	d15c <simple_strtoul+0x64>
    d19c:	e3550000 	cmp	r5, #0	; 0x0
    d1a0:	15850000 	strne	r0, [r5]
    d1a4:	e1a00004 	mov	r0, r4
    d1a8:	e8bd8070 	pop	{r4, r5, r6, pc}
    d1ac:	000118ca 	.word	0x000118ca

0000d1b0 <simple_strtol>:
    d1b0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    d1b4:	e5d03000 	ldrb	r3, [r0]
    d1b8:	e24dd004 	sub	sp, sp, #4	; 0x4
    d1bc:	e353002d 	cmp	r3, #45	; 0x2d
    d1c0:	1a000004 	bne	d1d8 <simple_strtol+0x28>
    d1c4:	e2800001 	add	r0, r0, #1	; 0x1
    d1c8:	ebffffca 	bl	d0f8 <simple_strtoul>
    d1cc:	e2600000 	rsb	r0, r0, #0	; 0x0
    d1d0:	e28dd004 	add	sp, sp, #4	; 0x4
    d1d4:	e8bd8000 	pop	{pc}
    d1d8:	e28dd004 	add	sp, sp, #4	; 0x4
    d1dc:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    d1e0:	eaffffc4 	b	d0f8 <simple_strtoul>

0000d1e4 <ustrtoul>:
    d1e4:	e92d4010 	push	{r4, lr}
    d1e8:	e1a04001 	mov	r4, r1
    d1ec:	ebffffc1 	bl	d0f8 <simple_strtoul>
    d1f0:	e5941000 	ldr	r1, [r4]
    d1f4:	e5d13000 	ldrb	r3, [r1]
    d1f8:	e353004b 	cmp	r3, #75	; 0x4b
    d1fc:	0a00000a 	beq	d22c <ustrtoul+0x48>
    d200:	8a000002 	bhi	d210 <ustrtoul+0x2c>
    d204:	e3530047 	cmp	r3, #71	; 0x47
    d208:	18bd8010 	popne	{r4, pc}
    d20c:	ea000004 	b	d224 <ustrtoul+0x40>
    d210:	e353004d 	cmp	r3, #77	; 0x4d
    d214:	0a000003 	beq	d228 <ustrtoul+0x44>
    d218:	e353006b 	cmp	r3, #107	; 0x6b
    d21c:	18bd8010 	popne	{r4, pc}
    d220:	ea000001 	b	d22c <ustrtoul+0x48>
    d224:	e1a00500 	lsl	r0, r0, #10
    d228:	e1a00500 	lsl	r0, r0, #10
    d22c:	e5d13001 	ldrb	r3, [r1, #1]
    d230:	e1a00500 	lsl	r0, r0, #10
    d234:	e3530069 	cmp	r3, #105	; 0x69
    d238:	18bd8010 	popne	{r4, pc}
    d23c:	e5d13002 	ldrb	r3, [r1, #2]
    d240:	e2812002 	add	r2, r1, #2	; 0x2
    d244:	e3530042 	cmp	r3, #66	; 0x42
    d248:	02813003 	addeq	r3, r1, #3	; 0x3
    d24c:	05843000 	streq	r3, [r4]
    d250:	15842000 	strne	r2, [r4]
    d254:	e8bd8010 	pop	{r4, pc}

0000d258 <simple_strtoull>:
    d258:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    d25c:	e5d03000 	ldrb	r3, [r0]
    d260:	e1a07000 	mov	r7, r0
    d264:	e3530030 	cmp	r3, #48	; 0x30
    d268:	e1a08001 	mov	r8, r1
    d26c:	e1a06002 	mov	r6, r2
    d270:	1a00000d 	bne	d2ac <simple_strtoull+0x54>
    d274:	e5f73001 	ldrb	r3, [r7, #1]!
    d278:	e3530078 	cmp	r3, #120	; 0x78
    d27c:	1a000007 	bne	d2a0 <simple_strtoull+0x48>
    d280:	e5d73001 	ldrb	r3, [r7, #1]
    d284:	e59f20ac 	ldr	r2, [pc, #172]	; d338 <simple_strtoull+0xe0>
    d288:	e2870001 	add	r0, r7, #1	; 0x1
    d28c:	e7d23003 	ldrb	r3, [r2, r3]
    d290:	e3130044 	tst	r3, #68	; 0x44
    d294:	11a07000 	movne	r7, r0
    d298:	13a06010 	movne	r6, #16	; 0x10
    d29c:	1a000004 	bne	d2b4 <simple_strtoull+0x5c>
    d2a0:	e3560000 	cmp	r6, #0	; 0x0
    d2a4:	03a06008 	moveq	r6, #8	; 0x8
    d2a8:	ea000001 	b	d2b4 <simple_strtoull+0x5c>
    d2ac:	e3520000 	cmp	r2, #0	; 0x0
    d2b0:	03a0600a 	moveq	r6, #10	; 0xa
    d2b4:	e3a04000 	mov	r4, #0	; 0x0
    d2b8:	e3a05000 	mov	r5, #0	; 0x0
    d2bc:	e59f9074 	ldr	r9, [pc, #116]	; d338 <simple_strtoull+0xe0>
    d2c0:	e3a0a000 	mov	sl, #0	; 0x0
    d2c4:	ea000004 	b	d2dc <simple_strtoull+0x84>
    d2c8:	e0832694 	umull	r2, r3, r4, r6
    d2cc:	e0233596 	mla	r3, r6, r5, r3
    d2d0:	e0904002 	adds	r4, r0, r2
    d2d4:	e0a15003 	adc	r5, r1, r3
    d2d8:	e2877001 	add	r7, r7, #1	; 0x1
    d2dc:	e5d7c000 	ldrb	ip, [r7]
    d2e0:	e7d9200c 	ldrb	r2, [r9, ip]
    d2e4:	e24c3030 	sub	r3, ip, #48	; 0x30
    d2e8:	e3120044 	tst	r2, #68	; 0x44
    d2ec:	0a00000c 	beq	d324 <simple_strtoull+0xcc>
    d2f0:	e3120004 	tst	r2, #4	; 0x4
    d2f4:	1a000004 	bne	d30c <simple_strtoull+0xb4>
    d2f8:	e24c3020 	sub	r3, ip, #32	; 0x20
    d2fc:	e3120002 	tst	r2, #2	; 0x2
    d300:	e6ef3073 	uxtb	r3, r3
    d304:	12433037 	subne	r3, r3, #55	; 0x37
    d308:	024c3037 	subeq	r3, ip, #55	; 0x37
    d30c:	e1a00003 	mov	r0, r3
    d310:	e1a01fc0 	asr	r1, r0, #31
    d314:	e15a0001 	cmp	sl, r1
    d318:	1a000001 	bne	d324 <simple_strtoull+0xcc>
    d31c:	e1560000 	cmp	r6, r0
    d320:	8affffe8 	bhi	d2c8 <simple_strtoull+0x70>
    d324:	e3580000 	cmp	r8, #0	; 0x0
    d328:	15887000 	strne	r7, [r8]
    d32c:	e1a01005 	mov	r1, r5
    d330:	e1a00004 	mov	r0, r4
    d334:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    d338:	000118ca 	.word	0x000118ca

0000d33c <put_dec_trunc>:
    d33c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    d340:	e1a03421 	lsr	r3, r1, #8
    d344:	e1a04221 	lsr	r4, r1, #4
    d348:	e204400f 	and	r4, r4, #15	; 0xf
    d34c:	e203600f 	and	r6, r3, #15	; 0xf
    d350:	e1a05621 	lsr	r5, r1, #12
    d354:	e0842006 	add	r2, r4, r6
    d358:	e201100f 	and	r1, r1, #15	; 0xf
    d35c:	e3a03006 	mov	r3, #6	; 0x6
    d360:	e0822005 	add	r2, r2, r5
    d364:	e0221293 	mla	r2, r3, r2, r1
    d368:	e3a080cd 	mov	r8, #205	; 0xcd
    d36c:	e00c0298 	mul	ip, r8, r2
    d370:	e3a01009 	mov	r1, #9	; 0x9
    d374:	e0214591 	mla	r1, r1, r5, r4
    d378:	e1a0c5ac 	lsr	ip, ip, #11
    d37c:	e3a0700a 	mov	r7, #10	; 0xa
    d380:	e3a03005 	mov	r3, #5	; 0x5
    d384:	e0231693 	mla	r3, r3, r6, r1
    d388:	e0622c97 	mls	r2, r7, ip, r2
    d38c:	e093100c 	adds	r1, r3, ip
    d390:	e2822030 	add	r2, r2, #48	; 0x30
    d394:	e1a04000 	mov	r4, r0
    d398:	e4c02001 	strb	r2, [r0], #1
    d39c:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    d3a0:	e0030198 	mul	r3, r8, r1
    d3a4:	e1a035a3 	lsr	r3, r3, #11
    d3a8:	e0621397 	mls	r2, r7, r3, r1
    d3ac:	e0831086 	add	r1, r3, r6, lsl #1
    d3b0:	e2822030 	add	r2, r2, #48	; 0x30
    d3b4:	e1913005 	orrs	r3, r1, r5
    d3b8:	e5c42001 	strb	r2, [r4, #1]
    d3bc:	e2800001 	add	r0, r0, #1	; 0x1
    d3c0:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    d3c4:	e3a0300d 	mov	r3, #13	; 0xd
    d3c8:	e0030193 	mul	r3, r3, r1
    d3cc:	e1a033a3 	lsr	r3, r3, #7
    d3d0:	e0621397 	mls	r2, r7, r3, r1
    d3d4:	e0931105 	adds	r1, r3, r5, lsl #2
    d3d8:	e2822030 	add	r2, r2, #48	; 0x30
    d3dc:	e4c02001 	strb	r2, [r0], #1
    d3e0:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    d3e4:	e0030198 	mul	r3, r8, r1
    d3e8:	e1a0c5a3 	lsr	ip, r3, #11
    d3ec:	e0631c97 	mls	r3, r7, ip, r1
    d3f0:	e35c0000 	cmp	ip, #0	; 0x0
    d3f4:	e2833030 	add	r3, r3, #48	; 0x30
    d3f8:	e4c03001 	strb	r3, [r0], #1
    d3fc:	128c3030 	addne	r3, ip, #48	; 0x30
    d400:	14c03001 	strbne	r3, [r0], #1
    d404:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000d408 <string>:
    d408:	e59fc0a4 	ldr	ip, [pc, #164]	; d4b4 <string+0xac>
    d40c:	e3510000 	cmp	r1, #0	; 0x0
    d410:	e92d4070 	push	{r4, r5, r6, lr}
    d414:	11a06001 	movne	r6, r1
    d418:	01a0600c 	moveq	r6, ip
    d41c:	e1a01003 	mov	r1, r3
    d420:	e1a04000 	mov	r4, r0
    d424:	e1a00006 	mov	r0, r6
    d428:	e1a05002 	mov	r5, r2
    d42c:	ebffcfb4 	bl	1304 <strnlen>
    d430:	e59d3010 	ldr	r3, [sp, #16]
    d434:	e1a0c000 	mov	ip, r0
    d438:	e3130010 	tst	r3, #16	; 0x10
    d43c:	01a02004 	moveq	r2, r4
    d440:	03a01020 	moveq	r1, #32	; 0x20
    d444:	0a000001 	beq	d450 <string+0x48>
    d448:	ea000006 	b	d468 <string+0x60>
    d44c:	e5421001 	strb	r1, [r2, #-1]
    d450:	e2455001 	sub	r5, r5, #1	; 0x1
    d454:	e2853001 	add	r3, r5, #1	; 0x1
    d458:	e15c0003 	cmp	ip, r3
    d45c:	e1a04002 	mov	r4, r2
    d460:	e2822001 	add	r2, r2, #1	; 0x1
    d464:	bafffff8 	blt	d44c <string+0x44>
    d468:	e3a02000 	mov	r2, #0	; 0x0
    d46c:	ea000002 	b	d47c <string+0x74>
    d470:	e7d63002 	ldrb	r3, [r6, r2]
    d474:	e7c43002 	strb	r3, [r4, r2]
    d478:	e2822001 	add	r2, r2, #1	; 0x1
    d47c:	e152000c 	cmp	r2, ip
    d480:	e0843002 	add	r3, r4, r2
    d484:	bafffff9 	blt	d470 <string+0x68>
    d488:	e1a01003 	mov	r1, r3
    d48c:	e1a02005 	mov	r2, r5
    d490:	e3a03020 	mov	r3, #32	; 0x20
    d494:	ea000000 	b	d49c <string+0x94>
    d498:	e5413001 	strb	r3, [r1, #-1]
    d49c:	e15c0002 	cmp	ip, r2
    d4a0:	e1a00001 	mov	r0, r1
    d4a4:	e2422001 	sub	r2, r2, #1	; 0x1
    d4a8:	e2811001 	add	r1, r1, #1	; 0x1
    d4ac:	bafffff9 	blt	d498 <string+0x90>
    d4b0:	e8bd8070 	pop	{r4, r5, r6, pc}
    d4b4:	000141c4 	.word	0x000141c4

0000d4b8 <put_dec>:
    d4b8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d4bc:	e24dd00c 	sub	sp, sp, #12	; 0xc
    d4c0:	e28d8008 	add	r8, sp, #8	; 0x8
    d4c4:	e16820f8 	strd	r2, [r8, #-8]!
    d4c8:	e1a07000 	mov	r7, r0
    d4cc:	e3a090cd 	mov	r9, #205	; 0xcd
    d4d0:	e3a0a00a 	mov	sl, #10	; 0xa
    d4d4:	e3a0b00d 	mov	fp, #13	; 0xd
    d4d8:	e59d5004 	ldr	r5, [sp, #4]
    d4dc:	e1a0000d 	mov	r0, sp
    d4e0:	e3550000 	cmp	r5, #0	; 0x0
    d4e4:	e59f10f0 	ldr	r1, [pc, #240]	; d5dc <put_dec+0x124>
    d4e8:	e59d4000 	ldr	r4, [sp]
    d4ec:	1a00000f 	bne	d530 <put_dec+0x78>
    d4f0:	e59f30e8 	ldr	r3, [pc, #232]	; d5e0 <put_dec+0x128>
    d4f4:	e1a00004 	mov	r0, r4
    d4f8:	e1540003 	cmp	r4, r3
    d4fc:	8a000004 	bhi	d514 <put_dec+0x5c>
    d500:	e1a00007 	mov	r0, r7
    d504:	e1a01004 	mov	r1, r4
    d508:	ebffff8b 	bl	d33c <put_dec_trunc>
    d50c:	e28dd00c 	add	sp, sp, #12	; 0xc
    d510:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d514:	eb000b77 	bl	102f8 <__aeabi_uidivmod>
    d518:	e1a00004 	mov	r0, r4
    d51c:	e1a06001 	mov	r6, r1
    d520:	e59f10b4 	ldr	r1, [pc, #180]	; d5dc <put_dec+0x124>
    d524:	eb000af7 	bl	10108 <__aeabi_uidiv>
    d528:	e88d0021 	stm	sp, {r0, r5}
    d52c:	ea000001 	b	d538 <put_dec+0x80>
    d530:	ebfffeb3 	bl	d004 <__div64_32>
    d534:	e1a06000 	mov	r6, r0
    d538:	e1a02226 	lsr	r2, r6, #4
    d53c:	e1a0c426 	lsr	ip, r6, #8
    d540:	e20cc00f 	and	ip, ip, #15	; 0xf
    d544:	e202200f 	and	r2, r2, #15	; 0xf
    d548:	e1a0e626 	lsr	lr, r6, #12
    d54c:	e082500c 	add	r5, r2, ip
    d550:	e085500e 	add	r5, r5, lr
    d554:	e206300f 	and	r3, r6, #15	; 0xf
    d558:	e3a01006 	mov	r1, #6	; 0x6
    d55c:	e0253591 	mla	r5, r1, r5, r3
    d560:	e3a03009 	mov	r3, #9	; 0x9
    d564:	e0222e93 	mla	r2, r3, lr, r2
    d568:	e3a01005 	mov	r1, #5	; 0x5
    d56c:	e0000599 	mul	r0, r9, r5
    d570:	e0222c91 	mla	r2, r1, ip, r2
    d574:	e1a005a0 	lsr	r0, r0, #11
    d578:	e0822000 	add	r2, r2, r0
    d57c:	e0010299 	mul	r1, r9, r2
    d580:	e1a015a1 	lsr	r1, r1, #11
    d584:	e081c08c 	add	ip, r1, ip, lsl #1
    d588:	e0030c9b 	mul	r3, fp, ip
    d58c:	e1a033a3 	lsr	r3, r3, #7
    d590:	e083e10e 	add	lr, r3, lr, lsl #2
    d594:	e0040e99 	mul	r4, r9, lr
    d598:	e1a045a4 	lsr	r4, r4, #11
    d59c:	e06ee49a 	mls	lr, sl, r4, lr
    d5a0:	e060509a 	mls	r0, sl, r0, r5
    d5a4:	e061219a 	mls	r1, sl, r1, r2
    d5a8:	e063c39a 	mls	r3, sl, r3, ip
    d5ac:	e2800030 	add	r0, r0, #48	; 0x30
    d5b0:	e2811030 	add	r1, r1, #48	; 0x30
    d5b4:	e2833030 	add	r3, r3, #48	; 0x30
    d5b8:	e28ee030 	add	lr, lr, #48	; 0x30
    d5bc:	e2844030 	add	r4, r4, #48	; 0x30
    d5c0:	e5c70000 	strb	r0, [r7]
    d5c4:	e5c71001 	strb	r1, [r7, #1]
    d5c8:	e5c73002 	strb	r3, [r7, #2]
    d5cc:	e5c7e003 	strb	lr, [r7, #3]
    d5d0:	e5c74004 	strb	r4, [r7, #4]
    d5d4:	e2877005 	add	r7, r7, #5	; 0x5
    d5d8:	eaffffbe 	b	d4d8 <put_dec+0x20>
    d5dc:	000186a0 	.word	0x000186a0
    d5e0:	0001869f 	.word	0x0001869f

0000d5e4 <number>:
    d5e4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d5e8:	e24dd05c 	sub	sp, sp, #92	; 0x5c
    d5ec:	e1a0a000 	mov	sl, r0
    d5f0:	e59d0080 	ldr	r0, [sp, #128]
    d5f4:	e59d808c 	ldr	r8, [sp, #140]
    d5f8:	e250100a 	subs	r1, r0, #10	; 0xa
    d5fc:	13a01001 	movne	r1, #1	; 0x1
    d600:	e0011328 	and	r1, r1, r8, lsr #6
    d604:	e3180010 	tst	r8, #16	; 0x10
    d608:	e58d1008 	str	r1, [sp, #8]
    d60c:	e2081020 	and	r1, r8, #32	; 0x20
    d610:	13c88001 	bicne	r8, r8, #1	; 0x1
    d614:	e3180002 	tst	r8, #2	; 0x2
    d618:	e1a06002 	mov	r6, r2
    d61c:	e1a07003 	mov	r7, r3
    d620:	e59d5084 	ldr	r5, [sp, #132]
    d624:	e58d1004 	str	r1, [sp, #4]
    d628:	0a000010 	beq	d670 <number+0x8c>
    d62c:	e3530000 	cmp	r3, #0	; 0x0
    d630:	ba000002 	blt	d640 <number+0x5c>
    d634:	e3180004 	tst	r8, #4	; 0x4
    d638:	0a000008 	beq	d660 <number+0x7c>
    d63c:	ea000004 	b	d654 <number+0x70>
    d640:	e2766000 	rsbs	r6, r6, #0	; 0x0
    d644:	e2e77000 	rsc	r7, r7, #0	; 0x0
    d648:	e2455001 	sub	r5, r5, #1	; 0x1
    d64c:	e3a0902d 	mov	r9, #45	; 0x2d
    d650:	ea000007 	b	d674 <number+0x90>
    d654:	e2455001 	sub	r5, r5, #1	; 0x1
    d658:	e3a0902b 	mov	r9, #43	; 0x2b
    d65c:	ea000004 	b	d674 <number+0x90>
    d660:	e3180008 	tst	r8, #8	; 0x8
    d664:	12455001 	subne	r5, r5, #1	; 0x1
    d668:	13a09020 	movne	r9, #32	; 0x20
    d66c:	1a000000 	bne	d674 <number+0x90>
    d670:	e3a09000 	mov	r9, #0	; 0x0
    d674:	e59d2008 	ldr	r2, [sp, #8]
    d678:	e3520000 	cmp	r2, #0	; 0x0
    d67c:	0a000003 	beq	d690 <number+0xac>
    d680:	e59d3080 	ldr	r3, [sp, #128]
    d684:	e2455001 	sub	r5, r5, #1	; 0x1
    d688:	e3530010 	cmp	r3, #16	; 0x10
    d68c:	02455001 	subeq	r5, r5, #1	; 0x1
    d690:	e196c007 	orrs	ip, r6, r7
    d694:	03a03030 	moveq	r3, #48	; 0x30
    d698:	05cd3016 	strbeq	r3, [sp, #22]
    d69c:	03a04001 	moveq	r4, #1	; 0x1
    d6a0:	0a000020 	beq	d728 <number+0x144>
    d6a4:	e59d0080 	ldr	r0, [sp, #128]
    d6a8:	e350000a 	cmp	r0, #10	; 0xa
    d6ac:	0a000017 	beq	d710 <number+0x12c>
    d6b0:	e3500010 	cmp	r0, #16	; 0x10
    d6b4:	03a01004 	moveq	r1, #4	; 0x4
    d6b8:	13a01003 	movne	r1, #3	; 0x3
    d6bc:	e58d100c 	str	r1, [sp, #12]
    d6c0:	e240b001 	sub	fp, r0, #1	; 0x1
    d6c4:	e3a04000 	mov	r4, #0	; 0x0
    d6c8:	e6ef3076 	uxtb	r3, r6
    d6cc:	e59f2164 	ldr	r2, [pc, #356]	; d838 <number+0x254>
    d6d0:	e003300b 	and	r3, r3, fp
    d6d4:	e59dc004 	ldr	ip, [sp, #4]
    d6d8:	e7d23003 	ldrb	r3, [r2, r3]
    d6dc:	e1a00006 	mov	r0, r6
    d6e0:	e18c3003 	orr	r3, ip, r3
    d6e4:	e28dc016 	add	ip, sp, #22	; 0x16
    d6e8:	e1a01007 	mov	r1, r7
    d6ec:	e59d200c 	ldr	r2, [sp, #12]
    d6f0:	e7cc3004 	strb	r3, [ip, r4]
    d6f4:	eb000b93 	bl	10548 <__aeabi_llsr>
    d6f8:	e1a06000 	mov	r6, r0
    d6fc:	e1960001 	orrs	r0, r6, r1
    d700:	e1a07001 	mov	r7, r1
    d704:	e2844001 	add	r4, r4, #1	; 0x1
    d708:	1affffee 	bne	d6c8 <number+0xe4>
    d70c:	ea000005 	b	d728 <number+0x144>
    d710:	e28d4016 	add	r4, sp, #22	; 0x16
    d714:	e1a00004 	mov	r0, r4
    d718:	e1a02006 	mov	r2, r6
    d71c:	e1a03007 	mov	r3, r7
    d720:	ebffff64 	bl	d4b8 <put_dec>
    d724:	e0644000 	rsb	r4, r4, r0
    d728:	e59d3088 	ldr	r3, [sp, #136]
    d72c:	e1540003 	cmp	r4, r3
    d730:	a1a01004 	movge	r1, r4
    d734:	b1a01003 	movlt	r1, r3
    d738:	e3180011 	tst	r8, #17	; 0x11
    d73c:	e061c005 	rsb	ip, r1, r5
    d740:	01a0300a 	moveq	r3, sl
    d744:	03a02020 	moveq	r2, #32	; 0x20
    d748:	0a000001 	beq	d754 <number+0x170>
    d74c:	ea000004 	b	d764 <number+0x180>
    d750:	e5432001 	strb	r2, [r3, #-1]
    d754:	e25cc001 	subs	ip, ip, #1	; 0x1
    d758:	e1a0a003 	mov	sl, r3
    d75c:	e2833001 	add	r3, r3, #1	; 0x1
    d760:	5afffffa 	bpl	d750 <number+0x16c>
    d764:	e3590000 	cmp	r9, #0	; 0x0
    d768:	14ca9001 	strbne	r9, [sl], #1
    d76c:	e59d2008 	ldr	r2, [sp, #8]
    d770:	e3520000 	cmp	r2, #0	; 0x0
    d774:	0a000006 	beq	d794 <number+0x1b0>
    d778:	e59d3080 	ldr	r3, [sp, #128]
    d77c:	e3530010 	cmp	r3, #16	; 0x10
    d780:	e3a03030 	mov	r3, #48	; 0x30
    d784:	e4ca3001 	strb	r3, [sl], #1
    d788:	059d0004 	ldreq	r0, [sp, #4]
    d78c:	03803058 	orreq	r3, r0, #88	; 0x58
    d790:	04ca3001 	strbeq	r3, [sl], #1
    d794:	e3180010 	tst	r8, #16	; 0x10
    d798:	1a000009 	bne	d7c4 <number+0x1e0>
    d79c:	e3180001 	tst	r8, #1	; 0x1
    d7a0:	13a02030 	movne	r2, #48	; 0x30
    d7a4:	03a02020 	moveq	r2, #32	; 0x20
    d7a8:	e1a0300a 	mov	r3, sl
    d7ac:	ea000000 	b	d7b4 <number+0x1d0>
    d7b0:	e5432001 	strb	r2, [r3, #-1]
    d7b4:	e25cc001 	subs	ip, ip, #1	; 0x1
    d7b8:	e1a0a003 	mov	sl, r3
    d7bc:	e2833001 	add	r3, r3, #1	; 0x1
    d7c0:	5afffffa 	bpl	d7b0 <number+0x1cc>
    d7c4:	e1a0000a 	mov	r0, sl
    d7c8:	e3a03030 	mov	r3, #48	; 0x30
    d7cc:	ea000000 	b	d7d4 <number+0x1f0>
    d7d0:	e5403001 	strb	r3, [r0, #-1]
    d7d4:	e2411001 	sub	r1, r1, #1	; 0x1
    d7d8:	e1540001 	cmp	r4, r1
    d7dc:	e1a02000 	mov	r2, r0
    d7e0:	e2800001 	add	r0, r0, #1	; 0x1
    d7e4:	dafffff9 	ble	d7d0 <number+0x1ec>
    d7e8:	e28d1058 	add	r1, sp, #88	; 0x58
    d7ec:	e0813004 	add	r3, r1, r4
    d7f0:	e2431043 	sub	r1, r3, #67	; 0x43
    d7f4:	ea000001 	b	d800 <number+0x21c>
    d7f8:	e5d13001 	ldrb	r3, [r1, #1]
    d7fc:	e5423001 	strb	r3, [r2, #-1]
    d800:	e2544001 	subs	r4, r4, #1	; 0x1
    d804:	e1a03002 	mov	r3, r2
    d808:	e2411001 	sub	r1, r1, #1	; 0x1
    d80c:	e2822001 	add	r2, r2, #1	; 0x1
    d810:	5afffff8 	bpl	d7f8 <number+0x214>
    d814:	e3a02020 	mov	r2, #32	; 0x20
    d818:	ea000000 	b	d820 <number+0x23c>
    d81c:	e5432001 	strb	r2, [r3, #-1]
    d820:	e25cc001 	subs	ip, ip, #1	; 0x1
    d824:	e1a00003 	mov	r0, r3
    d828:	e2833001 	add	r3, r3, #1	; 0x1
    d82c:	5afffffa 	bpl	d81c <number+0x238>
    d830:	e28dd05c 	add	sp, sp, #92	; 0x5c
    d834:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d838:	000118ba 	.word	0x000118ba

0000d83c <vsprintf>:
    d83c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d840:	e24dd084 	sub	sp, sp, #132	; 0x84
    d844:	e58d0028 	str	r0, [sp, #40]
    d848:	e1a08001 	mov	r8, r1
    d84c:	e1a0a000 	mov	sl, r0
    d850:	e28d107b 	add	r1, sp, #123	; 0x7b
    d854:	e28d007d 	add	r0, sp, #125	; 0x7d
    d858:	e58d2024 	str	r2, [sp, #36]
    d85c:	e1a06002 	mov	r6, r2
    d860:	e58d001c 	str	r0, [sp, #28]
    d864:	e58d1018 	str	r1, [sp, #24]
    d868:	ea0001a3 	b	defc <vsprintf+0x6c0>
    d86c:	e3530025 	cmp	r3, #37	; 0x25
    d870:	1a00015f 	bne	ddf4 <vsprintf+0x5b8>
    d874:	e3a07000 	mov	r7, #0	; 0x0
    d878:	e5f82001 	ldrb	r2, [r8, #1]!
    d87c:	e352002b 	cmp	r2, #43	; 0x2b
    d880:	0a00000c 	beq	d8b8 <vsprintf+0x7c>
    d884:	8a000004 	bhi	d89c <vsprintf+0x60>
    d888:	e3520020 	cmp	r2, #32	; 0x20
    d88c:	0a00000b 	beq	d8c0 <vsprintf+0x84>
    d890:	e3520023 	cmp	r2, #35	; 0x23
    d894:	1a00000f 	bne	d8d8 <vsprintf+0x9c>
    d898:	ea00000a 	b	d8c8 <vsprintf+0x8c>
    d89c:	e352002d 	cmp	r2, #45	; 0x2d
    d8a0:	0a000002 	beq	d8b0 <vsprintf+0x74>
    d8a4:	e3520030 	cmp	r2, #48	; 0x30
    d8a8:	1a00000a 	bne	d8d8 <vsprintf+0x9c>
    d8ac:	ea000007 	b	d8d0 <vsprintf+0x94>
    d8b0:	e3877010 	orr	r7, r7, #16	; 0x10
    d8b4:	eaffffef 	b	d878 <vsprintf+0x3c>
    d8b8:	e3877004 	orr	r7, r7, #4	; 0x4
    d8bc:	eaffffed 	b	d878 <vsprintf+0x3c>
    d8c0:	e3877008 	orr	r7, r7, #8	; 0x8
    d8c4:	eaffffeb 	b	d878 <vsprintf+0x3c>
    d8c8:	e3877040 	orr	r7, r7, #64	; 0x40
    d8cc:	eaffffe9 	b	d878 <vsprintf+0x3c>
    d8d0:	e3877001 	orr	r7, r7, #1	; 0x1
    d8d4:	eaffffe7 	b	d878 <vsprintf+0x3c>
    d8d8:	e2423030 	sub	r3, r2, #48	; 0x30
    d8dc:	e6ef3073 	uxtb	r3, r3
    d8e0:	e3530009 	cmp	r3, #9	; 0x9
    d8e4:	93a09000 	movls	r9, #0	; 0x0
    d8e8:	93a0100a 	movls	r1, #10	; 0xa
    d8ec:	9a000002 	bls	d8fc <vsprintf+0xc0>
    d8f0:	ea000007 	b	d914 <vsprintf+0xd8>
    d8f4:	e0292991 	mla	r9, r1, r9, r2
    d8f8:	e2888001 	add	r8, r8, #1	; 0x1
    d8fc:	e5d83000 	ldrb	r3, [r8]
    d900:	e2432030 	sub	r2, r3, #48	; 0x30
    d904:	e6ef3072 	uxtb	r3, r2
    d908:	e3530009 	cmp	r3, #9	; 0x9
    d90c:	9afffff8 	bls	d8f4 <vsprintf+0xb8>
    d910:	ea000008 	b	d938 <vsprintf+0xfc>
    d914:	e352002a 	cmp	r2, #42	; 0x2a
    d918:	13e09000 	mvnne	r9, #0	; 0x0
    d91c:	1a000005 	bne	d938 <vsprintf+0xfc>
    d920:	e5969000 	ldr	r9, [r6]
    d924:	e2888001 	add	r8, r8, #1	; 0x1
    d928:	e3590000 	cmp	r9, #0	; 0x0
    d92c:	e2866004 	add	r6, r6, #4	; 0x4
    d930:	b2699000 	rsblt	r9, r9, #0	; 0x0
    d934:	b3877010 	orrlt	r7, r7, #16	; 0x10
    d938:	e5d83000 	ldrb	r3, [r8]
    d93c:	e353002e 	cmp	r3, #46	; 0x2e
    d940:	13e02000 	mvnne	r2, #0	; 0x0
    d944:	158d202c 	strne	r2, [sp, #44]
    d948:	1a00001d 	bne	d9c4 <vsprintf+0x188>
    d94c:	e5f82001 	ldrb	r2, [r8, #1]!
    d950:	e2423030 	sub	r3, r2, #48	; 0x30
    d954:	e6ef3073 	uxtb	r3, r3
    d958:	e3530009 	cmp	r3, #9	; 0x9
    d95c:	93a03000 	movls	r3, #0	; 0x0
    d960:	958d302c 	strls	r3, [sp, #44]
    d964:	93a0100a 	movls	r1, #10	; 0xa
    d968:	9a000004 	bls	d980 <vsprintf+0x144>
    d96c:	ea000009 	b	d998 <vsprintf+0x15c>
    d970:	e59d002c 	ldr	r0, [sp, #44]
    d974:	e2888001 	add	r8, r8, #1	; 0x1
    d978:	e0202091 	mla	r0, r1, r0, r2
    d97c:	e58d002c 	str	r0, [sp, #44]
    d980:	e5d83000 	ldrb	r3, [r8]
    d984:	e2432030 	sub	r2, r3, #48	; 0x30
    d988:	e6ef3072 	uxtb	r3, r2
    d98c:	e3530009 	cmp	r3, #9	; 0x9
    d990:	9afffff6 	bls	d970 <vsprintf+0x134>
    d994:	ea000005 	b	d9b0 <vsprintf+0x174>
    d998:	e352002a 	cmp	r2, #42	; 0x2a
    d99c:	1a000006 	bne	d9bc <vsprintf+0x180>
    d9a0:	e5961000 	ldr	r1, [r6]
    d9a4:	e2888001 	add	r8, r8, #1	; 0x1
    d9a8:	e58d102c 	str	r1, [sp, #44]
    d9ac:	e2866004 	add	r6, r6, #4	; 0x4
    d9b0:	e59d202c 	ldr	r2, [sp, #44]
    d9b4:	e3520000 	cmp	r2, #0	; 0x0
    d9b8:	aa000001 	bge	d9c4 <vsprintf+0x188>
    d9bc:	e3a03000 	mov	r3, #0	; 0x0
    d9c0:	e58d302c 	str	r3, [sp, #44]
    d9c4:	e5d83000 	ldrb	r3, [r8]
    d9c8:	e353006c 	cmp	r3, #108	; 0x6c
    d9cc:	13530068 	cmpne	r3, #104	; 0x68
    d9d0:	0a000008 	beq	d9f8 <vsprintf+0x1bc>
    d9d4:	e353004c 	cmp	r3, #76	; 0x4c
    d9d8:	0a000006 	beq	d9f8 <vsprintf+0x1bc>
    d9dc:	e353005a 	cmp	r3, #90	; 0x5a
    d9e0:	0a000004 	beq	d9f8 <vsprintf+0x1bc>
    d9e4:	e353007a 	cmp	r3, #122	; 0x7a
    d9e8:	0a000002 	beq	d9f8 <vsprintf+0x1bc>
    d9ec:	e3530074 	cmp	r3, #116	; 0x74
    d9f0:	13e02000 	mvnne	r2, #0	; 0x0
    d9f4:	1a000007 	bne	da18 <vsprintf+0x1dc>
    d9f8:	e353006c 	cmp	r3, #108	; 0x6c
    d9fc:	e1a02003 	mov	r2, r3
    da00:	e2888001 	add	r8, r8, #1	; 0x1
    da04:	1a000003 	bne	da18 <vsprintf+0x1dc>
    da08:	e5d83000 	ldrb	r3, [r8]
    da0c:	e353006c 	cmp	r3, #108	; 0x6c
    da10:	02888001 	addeq	r8, r8, #1	; 0x1
    da14:	03a0204c 	moveq	r2, #76	; 0x4c
    da18:	e5d83000 	ldrb	r3, [r8]
    da1c:	e353006e 	cmp	r3, #110	; 0x6e
    da20:	0a0000df 	beq	dda4 <vsprintf+0x568>
    da24:	8a00000c 	bhi	da5c <vsprintf+0x220>
    da28:	e3530063 	cmp	r3, #99	; 0x63
    da2c:	0a000017 	beq	da90 <vsprintf+0x254>
    da30:	8a000004 	bhi	da48 <vsprintf+0x20c>
    da34:	e3530025 	cmp	r3, #37	; 0x25
    da38:	0a0000ed 	beq	ddf4 <vsprintf+0x5b8>
    da3c:	e3530058 	cmp	r3, #88	; 0x58
    da40:	1a0000e5 	bne	dddc <vsprintf+0x5a0>
    da44:	ea0000e0 	b	ddcc <vsprintf+0x590>
    da48:	e3530064 	cmp	r3, #100	; 0x64
    da4c:	0a0000e0 	beq	ddd4 <vsprintf+0x598>
    da50:	e3530069 	cmp	r3, #105	; 0x69
    da54:	1a0000e0 	bne	dddc <vsprintf+0x5a0>
    da58:	ea0000dd 	b	ddd4 <vsprintf+0x598>
    da5c:	e3530073 	cmp	r3, #115	; 0x73
    da60:	0a000021 	beq	daec <vsprintf+0x2b0>
    da64:	8a000004 	bhi	da7c <vsprintf+0x240>
    da68:	e353006f 	cmp	r3, #111	; 0x6f
    da6c:	0a0000d3 	beq	ddc0 <vsprintf+0x584>
    da70:	e3530070 	cmp	r3, #112	; 0x70
    da74:	1a0000d8 	bne	dddc <vsprintf+0x5a0>
    da78:	ea000023 	b	db0c <vsprintf+0x2d0>
    da7c:	e3530075 	cmp	r3, #117	; 0x75
    da80:	0a0000dd 	beq	ddfc <vsprintf+0x5c0>
    da84:	e3530078 	cmp	r3, #120	; 0x78
    da88:	1a0000d3 	bne	dddc <vsprintf+0x5a0>
    da8c:	ea0000cd 	b	ddc8 <vsprintf+0x58c>
    da90:	e3170010 	tst	r7, #16	; 0x10
    da94:	01a0300a 	moveq	r3, sl
    da98:	03a02020 	moveq	r2, #32	; 0x20
    da9c:	0a000001 	beq	daa8 <vsprintf+0x26c>
    daa0:	ea000005 	b	dabc <vsprintf+0x280>
    daa4:	e5432001 	strb	r2, [r3, #-1]
    daa8:	e2499001 	sub	r9, r9, #1	; 0x1
    daac:	e3590000 	cmp	r9, #0	; 0x0
    dab0:	e1a0a003 	mov	sl, r3
    dab4:	e2833001 	add	r3, r3, #1	; 0x1
    dab8:	cafffff9 	bgt	daa4 <vsprintf+0x268>
    dabc:	e5963000 	ldr	r3, [r6]
    dac0:	e1a0200a 	mov	r2, sl
    dac4:	e4c23001 	strb	r3, [r2], #1
    dac8:	e3a03020 	mov	r3, #32	; 0x20
    dacc:	ea000000 	b	dad4 <vsprintf+0x298>
    dad0:	e5423001 	strb	r3, [r2, #-1]
    dad4:	e2499001 	sub	r9, r9, #1	; 0x1
    dad8:	e3590000 	cmp	r9, #0	; 0x0
    dadc:	e1a0a002 	mov	sl, r2
    dae0:	e2822001 	add	r2, r2, #1	; 0x1
    dae4:	cafffff9 	bgt	dad0 <vsprintf+0x294>
    dae8:	ea0000ab 	b	dd9c <vsprintf+0x560>
    daec:	e58d7000 	str	r7, [sp]
    daf0:	e1a0000a 	mov	r0, sl
    daf4:	e5961000 	ldr	r1, [r6]
    daf8:	e1a02009 	mov	r2, r9
    dafc:	e59d302c 	ldr	r3, [sp, #44]
    db00:	ebfffe40 	bl	d408 <string>
    db04:	e1a0a000 	mov	sl, r0
    db08:	ea0000a3 	b	dd9c <vsprintf+0x560>
    db0c:	e596b000 	ldr	fp, [r6]
    db10:	e35b0000 	cmp	fp, #0	; 0x0
    db14:	1a000005 	bne	db30 <vsprintf+0x2f4>
    db18:	e1a0000a 	mov	r0, sl
    db1c:	e1a02009 	mov	r2, r9
    db20:	e59d302c 	ldr	r3, [sp, #44]
    db24:	e59f13f0 	ldr	r1, [pc, #1008]	; df1c <vsprintf+0x6e0>
    db28:	e58d7000 	str	r7, [sp]
    db2c:	ea000081 	b	dd38 <vsprintf+0x4fc>
    db30:	e5d83001 	ldrb	r3, [r8, #1]
    db34:	e2882001 	add	r2, r8, #1	; 0x1
    db38:	e353004d 	cmp	r3, #77	; 0x4d
    db3c:	0a000008 	beq	db64 <vsprintf+0x328>
    db40:	8a000002 	bhi	db50 <vsprintf+0x314>
    db44:	e3530049 	cmp	r3, #73	; 0x49
    db48:	1a00007c 	bne	dd40 <vsprintf+0x504>
    db4c:	ea000025 	b	dbe8 <vsprintf+0x3ac>
    db50:	e3530069 	cmp	r3, #105	; 0x69
    db54:	0a000022 	beq	dbe4 <vsprintf+0x3a8>
    db58:	e353006d 	cmp	r3, #109	; 0x6d
    db5c:	1a000077 	bne	dd40 <vsprintf+0x504>
    db60:	e3877040 	orr	r7, r7, #64	; 0x40
    db64:	e1a03327 	lsr	r3, r7, #6
    db68:	e2233001 	eor	r3, r3, #1	; 0x1
    db6c:	e2034001 	and	r4, r3, #1	; 0x1
    db70:	e3a0e000 	mov	lr, #0	; 0x0
    db74:	e28dc05b 	add	ip, sp, #91	; 0x5b
    db78:	e3a0503a 	mov	r5, #58	; 0x3a
    db7c:	e7db200e 	ldrb	r2, [fp, lr]
    db80:	e59f0398 	ldr	r0, [pc, #920]	; df20 <vsprintf+0x6e4>
    db84:	e202300f 	and	r3, r2, #15	; 0xf
    db88:	e7d02222 	ldrb	r2, [r0, r2, lsr #4]
    db8c:	e7d03003 	ldrb	r3, [r0, r3]
    db90:	e1a0000c 	mov	r0, ip
    db94:	e4c02001 	strb	r2, [r0], #1
    db98:	e35e0005 	cmp	lr, #5	; 0x5
    db9c:	03a01000 	moveq	r1, #0	; 0x0
    dba0:	12041001 	andne	r1, r4, #1	; 0x1
    dba4:	e3510000 	cmp	r1, #0	; 0x0
    dba8:	e5cc3001 	strb	r3, [ip, #1]
    dbac:	e28ee001 	add	lr, lr, #1	; 0x1
    dbb0:	e280c001 	add	ip, r0, #1	; 0x1
    dbb4:	128cc001 	addne	ip, ip, #1	; 0x1
    dbb8:	15c05001 	strbne	r5, [r0, #1]
    dbbc:	e35e0006 	cmp	lr, #6	; 0x6
    dbc0:	1affffed 	bne	db7c <vsprintf+0x340>
    dbc4:	e3a03000 	mov	r3, #0	; 0x0
    dbc8:	e5cc3000 	strb	r3, [ip]
    dbcc:	e1a0000a 	mov	r0, sl
    dbd0:	e3c7c040 	bic	ip, r7, #64	; 0x40
    dbd4:	e1a02009 	mov	r2, r9
    dbd8:	e59d302c 	ldr	r3, [sp, #44]
    dbdc:	e28d105b 	add	r1, sp, #91	; 0x5b
    dbe0:	ea000053 	b	dd34 <vsprintf+0x4f8>
    dbe4:	e3877040 	orr	r7, r7, #64	; 0x40
    dbe8:	e5d23001 	ldrb	r3, [r2, #1]
    dbec:	e3530036 	cmp	r3, #54	; 0x36
    dbf0:	1a00002d 	bne	dcac <vsprintf+0x470>
    dbf4:	e1a03327 	lsr	r3, r7, #6
    dbf8:	e2233001 	eor	r3, r3, #1	; 0x1
    dbfc:	e1a0500b 	mov	r5, fp
    dc00:	e3a04000 	mov	r4, #0	; 0x0
    dc04:	e203b001 	and	fp, r3, #1	; 0x1
    dc08:	e28dc033 	add	ip, sp, #51	; 0x33
    dc0c:	e5d52000 	ldrb	r2, [r5]
    dc10:	e59fe308 	ldr	lr, [pc, #776]	; df20 <vsprintf+0x6e4>
    dc14:	e202300f 	and	r3, r2, #15	; 0xf
    dc18:	e7de0003 	ldrb	r0, [lr, r3]
    dc1c:	e7de3222 	ldrb	r3, [lr, r2, lsr #4]
    dc20:	e1a0100c 	mov	r1, ip
    dc24:	e4c13001 	strb	r3, [r1], #1
    dc28:	e5cc0001 	strb	r0, [ip, #1]
    dc2c:	e5d52001 	ldrb	r2, [r5, #1]
    dc30:	e59f02e8 	ldr	r0, [pc, #744]	; df20 <vsprintf+0x6e4>
    dc34:	e202300f 	and	r3, r2, #15	; 0xf
    dc38:	e7d0c003 	ldrb	ip, [r0, r3]
    dc3c:	e59f32dc 	ldr	r3, [pc, #732]	; df20 <vsprintf+0x6e4>
    dc40:	e3540007 	cmp	r4, #7	; 0x7
    dc44:	03a0e000 	moveq	lr, #0	; 0x0
    dc48:	120be001 	andne	lr, fp, #1	; 0x1
    dc4c:	e7d32222 	ldrb	r2, [r3, r2, lsr #4]
    dc50:	e35e0000 	cmp	lr, #0	; 0x0
    dc54:	e58d2014 	str	r2, [sp, #20]
    dc58:	e2810001 	add	r0, r1, #1	; 0x1
    dc5c:	e59de014 	ldr	lr, [sp, #20]
    dc60:	e2802001 	add	r2, r0, #1	; 0x1
    dc64:	e5c1e001 	strb	lr, [r1, #1]
    dc68:	e2844001 	add	r4, r4, #1	; 0x1
    dc6c:	e5c0c001 	strb	ip, [r0, #1]
    dc70:	13a0003a 	movne	r0, #58	; 0x3a
    dc74:	e282c001 	add	ip, r2, #1	; 0x1
    dc78:	128cc001 	addne	ip, ip, #1	; 0x1
    dc7c:	15c20001 	strbne	r0, [r2, #1]
    dc80:	e3540008 	cmp	r4, #8	; 0x8
    dc84:	e2855002 	add	r5, r5, #2	; 0x2
    dc88:	1affffdf 	bne	dc0c <vsprintf+0x3d0>
    dc8c:	e3a03000 	mov	r3, #0	; 0x0
    dc90:	e5cc3000 	strb	r3, [ip]
    dc94:	e1a0000a 	mov	r0, sl
    dc98:	e3c7c040 	bic	ip, r7, #64	; 0x40
    dc9c:	e1a02009 	mov	r2, r9
    dca0:	e59d302c 	ldr	r3, [sp, #44]
    dca4:	e28d1033 	add	r1, sp, #51	; 0x33
    dca8:	ea000021 	b	dd34 <vsprintf+0x4f8>
    dcac:	e3530034 	cmp	r3, #52	; 0x34
    dcb0:	13c77040 	bicne	r7, r7, #64	; 0x40
    dcb4:	1a000021 	bne	dd40 <vsprintf+0x504>
    dcb8:	e3a05000 	mov	r5, #0	; 0x0
    dcbc:	e28d406d 	add	r4, sp, #109	; 0x6d
    dcc0:	e7db1005 	ldrb	r1, [fp, r5]
    dcc4:	e28d007d 	add	r0, sp, #125	; 0x7d
    dcc8:	ebfffd9b 	bl	d33c <put_dec_trunc>
    dccc:	e59d101c 	ldr	r1, [sp, #28]
    dcd0:	e2402001 	sub	r2, r0, #1	; 0x1
    dcd4:	e0610000 	rsb	r0, r1, r0
    dcd8:	e3a01000 	mov	r1, #0	; 0x0
    dcdc:	ea000002 	b	dcec <vsprintf+0x4b0>
    dce0:	e5d23001 	ldrb	r3, [r2, #1]
    dce4:	e7c43001 	strb	r3, [r4, r1]
    dce8:	e2811001 	add	r1, r1, #1	; 0x1
    dcec:	e59d3018 	ldr	r3, [sp, #24]
    dcf0:	e2422001 	sub	r2, r2, #1	; 0x1
    dcf4:	e1520003 	cmp	r2, r3
    dcf8:	1afffff8 	bne	dce0 <vsprintf+0x4a4>
    dcfc:	e3550003 	cmp	r5, #3	; 0x3
    dd00:	13a0e02e 	movne	lr, #46	; 0x2e
    dd04:	e0844000 	add	r4, r4, r0
    dd08:	e2855001 	add	r5, r5, #1	; 0x1
    dd0c:	14c4e001 	strbne	lr, [r4], #1
    dd10:	e3550004 	cmp	r5, #4	; 0x4
    dd14:	1affffe9 	bne	dcc0 <vsprintf+0x484>
    dd18:	e3a03000 	mov	r3, #0	; 0x0
    dd1c:	e5c43000 	strb	r3, [r4]
    dd20:	e59d302c 	ldr	r3, [sp, #44]
    dd24:	e3c7c040 	bic	ip, r7, #64	; 0x40
    dd28:	e1a0000a 	mov	r0, sl
    dd2c:	e1a02009 	mov	r2, r9
    dd30:	e28d106d 	add	r1, sp, #109	; 0x6d
    dd34:	e58dc000 	str	ip, [sp]
    dd38:	ebfffdb2 	bl	d408 <string>
    dd3c:	ea00000d 	b	dd78 <vsprintf+0x53c>
    dd40:	e3a01010 	mov	r1, #16	; 0x10
    dd44:	e3790001 	cmn	r9, #1	; 0x1
    dd48:	e58d1000 	str	r1, [sp]
    dd4c:	e59d102c 	ldr	r1, [sp, #44]
    dd50:	e387c020 	orr	ip, r7, #32	; 0x20
    dd54:	02899009 	addeq	r9, r9, #9	; 0x9
    dd58:	0387c021 	orreq	ip, r7, #33	; 0x21
    dd5c:	e1a0000a 	mov	r0, sl
    dd60:	e1a0200b 	mov	r2, fp
    dd64:	e3a03000 	mov	r3, #0	; 0x0
    dd68:	e58d9004 	str	r9, [sp, #4]
    dd6c:	e58d1008 	str	r1, [sp, #8]
    dd70:	e58dc00c 	str	ip, [sp, #12]
    dd74:	ebfffe1a 	bl	d5e4 <number>
    dd78:	e1a0a000 	mov	sl, r0
    dd7c:	e1a02008 	mov	r2, r8
    dd80:	e5d23001 	ldrb	r3, [r2, #1]
    dd84:	e59fe198 	ldr	lr, [pc, #408]	; df24 <vsprintf+0x6e8>
    dd88:	e1a08002 	mov	r8, r2
    dd8c:	e7de3003 	ldrb	r3, [lr, r3]
    dd90:	e2822001 	add	r2, r2, #1	; 0x1
    dd94:	e3130007 	tst	r3, #7	; 0x7
    dd98:	1afffff8 	bne	dd80 <vsprintf+0x544>
    dd9c:	e2866004 	add	r6, r6, #4	; 0x4
    dda0:	ea000054 	b	def8 <vsprintf+0x6bc>
    dda4:	e59d0028 	ldr	r0, [sp, #40]
    dda8:	e5963000 	ldr	r3, [r6]
    ddac:	e2861004 	add	r1, r6, #4	; 0x4
    ddb0:	e060200a 	rsb	r2, r0, sl
    ddb4:	e1a06001 	mov	r6, r1
    ddb8:	e5832000 	str	r2, [r3]
    ddbc:	ea00004d 	b	def8 <vsprintf+0x6bc>
    ddc0:	e3a0c008 	mov	ip, #8	; 0x8
    ddc4:	ea00000d 	b	de00 <vsprintf+0x5c4>
    ddc8:	e3877020 	orr	r7, r7, #32	; 0x20
    ddcc:	e3a0c010 	mov	ip, #16	; 0x10
    ddd0:	ea00000a 	b	de00 <vsprintf+0x5c4>
    ddd4:	e3877002 	orr	r7, r7, #2	; 0x2
    ddd8:	ea000007 	b	ddfc <vsprintf+0x5c0>
    dddc:	e3a03025 	mov	r3, #37	; 0x25
    dde0:	e4ca3001 	strb	r3, [sl], #1
    dde4:	e5d83000 	ldrb	r3, [r8]
    dde8:	e3530000 	cmp	r3, #0	; 0x0
    ddec:	02488001 	subeq	r8, r8, #1	; 0x1
    ddf0:	0a000040 	beq	def8 <vsprintf+0x6bc>
    ddf4:	e4ca3001 	strb	r3, [sl], #1
    ddf8:	ea00003e 	b	def8 <vsprintf+0x6bc>
    ddfc:	e3a0c00a 	mov	ip, #10	; 0xa
    de00:	e352004c 	cmp	r2, #76	; 0x4c
    de04:	1a000009 	bne	de30 <vsprintf+0x5f4>
    de08:	e59d1024 	ldr	r1, [sp, #36]
    de0c:	e3a02000 	mov	r2, #0	; 0x0
    de10:	e0613006 	rsb	r3, r1, r6
    de14:	e3130007 	tst	r3, #7	; 0x7
    de18:	02866004 	addeq	r6, r6, #4	; 0x4
    de1c:	e896000a 	ldm	r6, {r1, r3}
    de20:	e1a05003 	mov	r5, r3
    de24:	e1824001 	orr	r4, r2, r1
    de28:	e2866008 	add	r6, r6, #8	; 0x8
    de2c:	ea000028 	b	ded4 <vsprintf+0x698>
    de30:	e352006c 	cmp	r2, #108	; 0x6c
    de34:	1a000006 	bne	de54 <vsprintf+0x618>
    de38:	e5963000 	ldr	r3, [r6]
    de3c:	e3170002 	tst	r7, #2	; 0x2
    de40:	e1a04003 	mov	r4, r3
    de44:	e3a05000 	mov	r5, #0	; 0x0
    de48:	e2866004 	add	r6, r6, #4	; 0x4
    de4c:	0a000020 	beq	ded4 <vsprintf+0x698>
    de50:	ea00001e 	b	ded0 <vsprintf+0x694>
    de54:	e352007a 	cmp	r2, #122	; 0x7a
    de58:	1352005a 	cmpne	r2, #90	; 0x5a
    de5c:	05963000 	ldreq	r3, [r6]
    de60:	02866004 	addeq	r6, r6, #4	; 0x4
    de64:	01a04003 	moveq	r4, r3
    de68:	03a05000 	moveq	r5, #0	; 0x0
    de6c:	0a000018 	beq	ded4 <vsprintf+0x698>
    de70:	e3520074 	cmp	r2, #116	; 0x74
    de74:	05963000 	ldreq	r3, [r6]
    de78:	02866004 	addeq	r6, r6, #4	; 0x4
    de7c:	0a00000b 	beq	deb0 <vsprintf+0x674>
    de80:	e3520068 	cmp	r2, #104	; 0x68
    de84:	e2861004 	add	r1, r6, #4	; 0x4
    de88:	e5962000 	ldr	r2, [r6]
    de8c:	e2070002 	and	r0, r7, #2	; 0x2
    de90:	1a000009 	bne	debc <vsprintf+0x680>
    de94:	e6ff3072 	uxth	r3, r2
    de98:	e3500000 	cmp	r0, #0	; 0x0
    de9c:	e1a06001 	mov	r6, r1
    dea0:	e1a04003 	mov	r4, r3
    dea4:	e3a05000 	mov	r5, #0	; 0x0
    dea8:	0a000009 	beq	ded4 <vsprintf+0x698>
    deac:	e6bf3074 	sxth	r3, r4
    deb0:	e1a04003 	mov	r4, r3
    deb4:	e1a05fc4 	asr	r5, r4, #31
    deb8:	ea000005 	b	ded4 <vsprintf+0x698>
    debc:	e3500000 	cmp	r0, #0	; 0x0
    dec0:	e1a06001 	mov	r6, r1
    dec4:	e1a04002 	mov	r4, r2
    dec8:	e3a05000 	mov	r5, #0	; 0x0
    decc:	0a000000 	beq	ded4 <vsprintf+0x698>
    ded0:	e1a05fc4 	asr	r5, r4, #31
    ded4:	e59de02c 	ldr	lr, [sp, #44]
    ded8:	e1a0000a 	mov	r0, sl
    dedc:	e1a02004 	mov	r2, r4
    dee0:	e1a03005 	mov	r3, r5
    dee4:	e58dc000 	str	ip, [sp]
    dee8:	e98d4200 	stmib	sp, {r9, lr}
    deec:	e58d700c 	str	r7, [sp, #12]
    def0:	ebfffdbb 	bl	d5e4 <number>
    def4:	e1a0a000 	mov	sl, r0
    def8:	e2888001 	add	r8, r8, #1	; 0x1
    defc:	e5d83000 	ldrb	r3, [r8]
    df00:	e3530000 	cmp	r3, #0	; 0x0
    df04:	1afffe58 	bne	d86c <vsprintf+0x30>
    df08:	e5ca3000 	strb	r3, [sl]
    df0c:	e59d1028 	ldr	r1, [sp, #40]
    df10:	e061000a 	rsb	r0, r1, sl
    df14:	e28dd084 	add	sp, sp, #132	; 0x84
    df18:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    df1c:	000141cb 	.word	0x000141cb
    df20:	000118a9 	.word	0x000118a9
    df24:	000118ca 	.word	0x000118ca

0000df28 <sprintf>:
    df28:	e92d000e 	push	{r1, r2, r3}
    df2c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    df30:	e59d1004 	ldr	r1, [sp, #4]
    df34:	e28d2008 	add	r2, sp, #8	; 0x8
    df38:	ebfffe3f 	bl	d83c <vsprintf>
    df3c:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    df40:	e28dd00c 	add	sp, sp, #12	; 0xc
    df44:	e12fff1e 	bx	lr

0000df48 <ius_check_header>:
    df48:	e92d4070 	push	{r4, r5, r6, lr}
    df4c:	e5901000 	ldr	r1, [r0]
    df50:	e59f30e8 	ldr	r3, [pc, #232]	; e040 <ius_check_header+0xf8>
    df54:	e24dd008 	sub	sp, sp, #8	; 0x8
    df58:	e1510003 	cmp	r1, r3
    df5c:	e1a04000 	mov	r4, r0
    df60:	0a000005 	beq	df7c <ius_check_header+0x34>
    df64:	e59f00d8 	ldr	r0, [pc, #216]	; e044 <ius_check_header+0xfc>
    df68:	e59f30d8 	ldr	r3, [pc, #216]	; e048 <ius_check_header+0x100>
    df6c:	e1a0e00f 	mov	lr, pc
    df70:	e593f000 	ldr	pc, [r3]
    df74:	e3e00000 	mvn	r0, #0	; 0x0
    df78:	ea00002e 	b	e038 <ius_check_header+0xf0>
    df7c:	ebffe01a 	bl	5fec <cdbg_verify_burn>
    df80:	e3500000 	cmp	r0, #0	; 0x0
    df84:	0a000011 	beq	dfd0 <ius_check_header+0x88>
    df88:	e5943004 	ldr	r3, [r4, #4]
    df8c:	e3130002 	tst	r3, #2	; 0x2
    df90:	1a000005 	bne	dfac <ius_check_header+0x64>
    df94:	e59f00b0 	ldr	r0, [pc, #176]	; e04c <ius_check_header+0x104>
    df98:	e59f30a8 	ldr	r3, [pc, #168]	; e048 <ius_check_header+0x100>
    df9c:	e1a0e00f 	mov	lr, pc
    dfa0:	e593f000 	ldr	pc, [r3]
    dfa4:	e3e00021 	mvn	r0, #33	; 0x21
    dfa8:	ea000022 	b	e038 <ius_check_header+0xf0>
    dfac:	ebffe012 	bl	5ffc <cdbg_verify_burn_key>
    dfb0:	e59f2098 	ldr	r2, [pc, #152]	; e050 <ius_check_header+0x108>
    dfb4:	e1a01000 	mov	r1, r0
    dfb8:	e3a00000 	mov	r0, #0	; 0x0
    dfbc:	ebffdc9e 	bl	523c <ss_aes_init>
    dfc0:	e2840010 	add	r0, r4, #16	; 0x10
    dfc4:	e1a01000 	mov	r1, r0
    dfc8:	e3a02e1f 	mov	r2, #496	; 0x1f0
    dfcc:	ebffdc89 	bl	51f8 <ss_aes_data>
    dfd0:	e3a06000 	mov	r6, #0	; 0x0
    dfd4:	e5945018 	ldr	r5, [r4, #24]
    dfd8:	e3a01c02 	mov	r1, #512	; 0x200
    dfdc:	e5846018 	str	r6, [r4, #24]
    dfe0:	e1a00006 	mov	r0, r6
    dfe4:	ebffddb2 	bl	56b4 <hash_init>
    dfe8:	e3a01c02 	mov	r1, #512	; 0x200
    dfec:	e1a00004 	mov	r0, r4
    dff0:	ebffdd8a 	bl	5620 <hash_data>
    dff4:	e28d0004 	add	r0, sp, #4	; 0x4
    dff8:	ebffdd6c 	bl	55b0 <hash_value>
    dffc:	e59d2004 	ldr	r2, [sp, #4]
    e000:	e59f3040 	ldr	r3, [pc, #64]	; e048 <ius_check_header+0x100>
    e004:	e1550002 	cmp	r5, r2
    e008:	e5845018 	str	r5, [r4, #24]
    e00c:	0a000005 	beq	e028 <ius_check_header+0xe0>
    e010:	e59f003c 	ldr	r0, [pc, #60]	; e054 <ius_check_header+0x10c>
    e014:	e1a01005 	mov	r1, r5
    e018:	e1a0e00f 	mov	lr, pc
    e01c:	e593f000 	ldr	pc, [r3]
    e020:	e3e0001f 	mvn	r0, #31	; 0x1f
    e024:	ea000003 	b	e038 <ius_check_header+0xf0>
    e028:	e59f0028 	ldr	r0, [pc, #40]	; e058 <ius_check_header+0x110>
    e02c:	e1a0e00f 	mov	lr, pc
    e030:	e593f000 	ldr	pc, [r3]
    e034:	e1a00006 	mov	r0, r6
    e038:	e28dd008 	add	sp, sp, #8	; 0x8
    e03c:	e8bd8070 	pop	{r4, r5, r6, pc}
    e040:	7375692b 	.word	0x7375692b
    e044:	000141d2 	.word	0x000141d2
    e048:	0800d76c 	.word	0x0800d76c
    e04c:	000141ec 	.word	0x000141ec
    e050:	0800d10c 	.word	0x0800d10c
    e054:	000141fb 	.word	0x000141fb
    e058:	00014223 	.word	0x00014223

0000e05c <printf_off>:
    e05c:	e92d000f 	push	{r0, r1, r2, r3}
    e060:	e28dd010 	add	sp, sp, #16	; 0x10
    e064:	e12fff1e 	bx	lr

0000e068 <disable_ctrlc>:
    e068:	e59f300c 	ldr	r3, [pc, #12]	; e07c <disable_ctrlc+0x14>
    e06c:	e5932000 	ldr	r2, [r3]
    e070:	e5830000 	str	r0, [r3]
    e074:	e1a00002 	mov	r0, r2
    e078:	e12fff1e 	bx	lr
    e07c:	0800d30c 	.word	0x0800d30c

0000e080 <had_ctrlc>:
    e080:	e59f3004 	ldr	r3, [pc, #4]	; e08c <had_ctrlc+0xc>
    e084:	e5930000 	ldr	r0, [r3]
    e088:	e12fff1e 	bx	lr
    e08c:	0800d308 	.word	0x0800d308

0000e090 <clear_ctrlc>:
    e090:	e59f3008 	ldr	r3, [pc, #8]	; e0a0 <clear_ctrlc+0x10>
    e094:	e3a02000 	mov	r2, #0	; 0x0
    e098:	e5832000 	str	r2, [r3]
    e09c:	e12fff1e 	bx	lr
    e0a0:	0800d308 	.word	0x0800d308

0000e0a4 <dbg>:
    e0a4:	e92d000f 	push	{r0, r1, r2, r3}
    e0a8:	e28dd010 	add	sp, sp, #16	; 0x10
    e0ac:	e12fff1e 	bx	lr

0000e0b0 <puts>:
    e0b0:	eaffe98c 	b	86e8 <serial_puts>

0000e0b4 <vprintf>:
    e0b4:	e92d4010 	push	{r4, lr}
    e0b8:	e24ddd06 	sub	sp, sp, #384	; 0x180
    e0bc:	e1a02001 	mov	r2, r1
    e0c0:	e1a01000 	mov	r1, r0
    e0c4:	e1a0000d 	mov	r0, sp
    e0c8:	ebfffddb 	bl	d83c <vsprintf>
    e0cc:	e1a0000d 	mov	r0, sp
    e0d0:	e1a0400d 	mov	r4, sp
    e0d4:	ebffe983 	bl	86e8 <serial_puts>
    e0d8:	e28ddd06 	add	sp, sp, #384	; 0x180
    e0dc:	e8bd8010 	pop	{r4, pc}

0000e0e0 <printf_on>:
    e0e0:	e92d000f 	push	{r0, r1, r2, r3}
    e0e4:	e92d4010 	push	{r4, lr}
    e0e8:	e24ddd06 	sub	sp, sp, #384	; 0x180
    e0ec:	e1a0000d 	mov	r0, sp
    e0f0:	e59d1188 	ldr	r1, [sp, #392]
    e0f4:	e28d2f63 	add	r2, sp, #396	; 0x18c
    e0f8:	ebfffdcf 	bl	d83c <vsprintf>
    e0fc:	e1a0000d 	mov	r0, sp
    e100:	e1a0400d 	mov	r4, sp
    e104:	ebffe977 	bl	86e8 <serial_puts>
    e108:	e28ddd06 	add	sp, sp, #384	; 0x180
    e10c:	e8bd4010 	pop	{r4, lr}
    e110:	e28dd010 	add	sp, sp, #16	; 0x10
    e114:	e12fff1e 	bx	lr

0000e118 <serial_printf>:
    e118:	e92d000f 	push	{r0, r1, r2, r3}
    e11c:	e92d4010 	push	{r4, lr}
    e120:	e24ddd06 	sub	sp, sp, #384	; 0x180
    e124:	e1a0000d 	mov	r0, sp
    e128:	e59d1188 	ldr	r1, [sp, #392]
    e12c:	e28d2f63 	add	r2, sp, #396	; 0x18c
    e130:	ebfffdc1 	bl	d83c <vsprintf>
    e134:	e1a0000d 	mov	r0, sp
    e138:	e1a0400d 	mov	r4, sp
    e13c:	ebffe969 	bl	86e8 <serial_puts>
    e140:	e28ddd06 	add	sp, sp, #384	; 0x180
    e144:	e8bd4010 	pop	{r4, lr}
    e148:	e28dd010 	add	sp, sp, #16	; 0x10
    e14c:	e12fff1e 	bx	lr

0000e150 <putc>:
    e150:	eaffe94d 	b	868c <serial_putc>

0000e154 <tstc>:
    e154:	eaffe945 	b	8670 <serial_tstc>

0000e158 <getc>:
    e158:	eaffe973 	b	872c <serial_getc>

0000e15c <ctrlc>:
    e15c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    e160:	e59f3040 	ldr	r3, [pc, #64]	; e1a8 <ctrlc+0x4c>
    e164:	e24dd004 	sub	sp, sp, #4	; 0x4
    e168:	e5933000 	ldr	r3, [r3]
    e16c:	e3530000 	cmp	r3, #0	; 0x0
    e170:	1a000009 	bne	e19c <ctrlc+0x40>
    e174:	ebffe93d 	bl	8670 <serial_tstc>
    e178:	e3500000 	cmp	r0, #0	; 0x0
    e17c:	0a000006 	beq	e19c <ctrlc+0x40>
    e180:	ebffe969 	bl	872c <serial_getc>
    e184:	e3500003 	cmp	r0, #3	; 0x3
    e188:	059f301c 	ldreq	r3, [pc, #28]	; e1ac <ctrlc+0x50>
    e18c:	03a02001 	moveq	r2, #1	; 0x1
    e190:	05832000 	streq	r2, [r3]
    e194:	01a00002 	moveq	r0, r2
    e198:	0a000000 	beq	e1a0 <ctrlc+0x44>
    e19c:	e3a00000 	mov	r0, #0	; 0x0
    e1a0:	e28dd004 	add	sp, sp, #4	; 0x4
    e1a4:	e8bd8000 	pop	{pc}
    e1a8:	0800d30c 	.word	0x0800d30c
    e1ac:	0800d308 	.word	0x0800d308

0000e1b0 <parse_line>:
    e1b0:	e92d4010 	push	{r4, lr}
    e1b4:	e3a04000 	mov	r4, #0	; 0x0
    e1b8:	e1a02004 	mov	r2, r4
    e1bc:	ea000000 	b	e1c4 <parse_line+0x14>
    e1c0:	e2800001 	add	r0, r0, #1	; 0x1
    e1c4:	e5d03000 	ldrb	r3, [r0]
    e1c8:	e3530009 	cmp	r3, #9	; 0x9
    e1cc:	13530020 	cmpne	r3, #32	; 0x20
    e1d0:	0afffffa 	beq	e1c0 <parse_line+0x10>
    e1d4:	e3530000 	cmp	r3, #0	; 0x0
    e1d8:	0a00000c 	beq	e210 <parse_line+0x60>
    e1dc:	e7810104 	str	r0, [r1, r4, lsl #2]
    e1e0:	e2844001 	add	r4, r4, #1	; 0x1
    e1e4:	ea000000 	b	e1ec <parse_line+0x3c>
    e1e8:	e2800001 	add	r0, r0, #1	; 0x1
    e1ec:	e5d03000 	ldrb	r3, [r0]
    e1f0:	e3530000 	cmp	r3, #0	; 0x0
    e1f4:	13530020 	cmpne	r3, #32	; 0x20
    e1f8:	0a000002 	beq	e208 <parse_line+0x58>
    e1fc:	e3530009 	cmp	r3, #9	; 0x9
    e200:	1afffff8 	bne	e1e8 <parse_line+0x38>
    e204:	ea000003 	b	e218 <parse_line+0x68>
    e208:	e3530000 	cmp	r3, #0	; 0x0
    e20c:	1a000001 	bne	e218 <parse_line+0x68>
    e210:	e7813104 	str	r3, [r1, r4, lsl #2]
    e214:	ea000007 	b	e238 <parse_line+0x88>
    e218:	e3540010 	cmp	r4, #16	; 0x10
    e21c:	e5c02000 	strb	r2, [r0]
    e220:	1affffe6 	bne	e1c0 <parse_line+0x10>
    e224:	e59f0014 	ldr	r0, [pc, #20]	; e240 <parse_line+0x90>
    e228:	e1a01004 	mov	r1, r4
    e22c:	e59f3010 	ldr	r3, [pc, #16]	; e244 <parse_line+0x94>
    e230:	e1a0e00f 	mov	lr, pc
    e234:	e593f000 	ldr	pc, [r3]
    e238:	e1a00004 	mov	r0, r4
    e23c:	e8bd8010 	pop	{r4, pc}
    e240:	00014239 	.word	0x00014239
    e244:	0800d76c 	.word	0x0800d76c

0000e248 <run_command>:
    e248:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    e24c:	e1a05000 	mov	r5, r0
    e250:	e24ddf93 	sub	sp, sp, #588	; 0x24c
    e254:	e1a0a001 	mov	sl, r1
    e258:	ebffff8c 	bl	e090 <clear_ctrlc>
    e25c:	e3550000 	cmp	r5, #0	; 0x0
    e260:	0a0000ac 	beq	e518 <run_command+0x2d0>
    e264:	e5d53000 	ldrb	r3, [r5]
    e268:	e3530000 	cmp	r3, #0	; 0x0
    e26c:	0a0000a9 	beq	e518 <run_command+0x2d0>
    e270:	e1a00005 	mov	r0, r5
    e274:	ebffcc1a 	bl	12e4 <strlen>
    e278:	e35000ff 	cmp	r0, #255	; 0xff
    e27c:	9a000002 	bls	e28c <run_command+0x44>
    e280:	e59f02a0 	ldr	r0, [pc, #672]	; e528 <run_command+0x2e0>
    e284:	ebffff89 	bl	e0b0 <puts>
    e288:	ea0000a2 	b	e518 <run_command+0x2d0>
    e28c:	e28d4f41 	add	r4, sp, #260	; 0x104
    e290:	e1a01005 	mov	r1, r5
    e294:	e1a00004 	mov	r0, r4
    e298:	e3a06000 	mov	r6, #0	; 0x0
    e29c:	ebffcbb9 	bl	1188 <strcpy>
    e2a0:	e3a07001 	mov	r7, #1	; 0x1
    e2a4:	e1a0b006 	mov	fp, r6
    e2a8:	e28d9f81 	add	r9, sp, #516	; 0x204
    e2ac:	ea000091 	b	e4f8 <run_command+0x2b0>
    e2b0:	e3510027 	cmp	r1, #39	; 0x27
    e2b4:	1a000002 	bne	e2c4 <run_command+0x7c>
    e2b8:	e5523001 	ldrb	r3, [r2, #-1]
    e2bc:	e353005c 	cmp	r3, #92	; 0x5c
    e2c0:	12200001 	eorne	r0, r0, #1	; 0x1
    e2c4:	e3500000 	cmp	r0, #0	; 0x0
    e2c8:	1a000009 	bne	e2f4 <run_command+0xac>
    e2cc:	e351003b 	cmp	r1, #59	; 0x3b
    e2d0:	13a03000 	movne	r3, #0	; 0x0
    e2d4:	03a03001 	moveq	r3, #1	; 0x1
    e2d8:	e1520004 	cmp	r2, r4
    e2dc:	03a03000 	moveq	r3, #0	; 0x0
    e2e0:	e3530000 	cmp	r3, #0	; 0x0
    e2e4:	0a000002 	beq	e2f4 <run_command+0xac>
    e2e8:	e5523001 	ldrb	r3, [r2, #-1]
    e2ec:	e353005c 	cmp	r3, #92	; 0x5c
    e2f0:	1a000005 	bne	e30c <run_command+0xc4>
    e2f4:	e2822001 	add	r2, r2, #1	; 0x1
    e2f8:	e5d21000 	ldrb	r1, [r2]
    e2fc:	e3510000 	cmp	r1, #0	; 0x0
    e300:	1affffea 	bne	e2b0 <run_command+0x68>
    e304:	e1a08002 	mov	r8, r2
    e308:	ea000001 	b	e314 <run_command+0xcc>
    e30c:	e5c20000 	strb	r0, [r2]
    e310:	e2828001 	add	r8, r2, #1	; 0x1
    e314:	e1a00004 	mov	r0, r4
    e318:	ebffcbf1 	bl	12e4 <strlen>
    e31c:	e3a0c000 	mov	ip, #0	; 0x0
    e320:	e3a0ec01 	mov	lr, #256	; 0x100
    e324:	e1a0100c 	mov	r1, ip
    e328:	e28d5004 	add	r5, sp, #4	; 0x4
    e32c:	ea000049 	b	e458 <run_command+0x210>
    e330:	e35c0003 	cmp	ip, #3	; 0x3
    e334:	e2400001 	sub	r0, r0, #1	; 0x1
    e338:	e4d42001 	ldrb	r2, [r4], #1
    e33c:	0a000035 	beq	e418 <run_command+0x1d0>
    e340:	e352005c 	cmp	r2, #92	; 0x5c
    e344:	13a03000 	movne	r3, #0	; 0x0
    e348:	03a03001 	moveq	r3, #1	; 0x1
    e34c:	e351005c 	cmp	r1, #92	; 0x5c
    e350:	03a03000 	moveq	r3, #0	; 0x0
    e354:	e3530000 	cmp	r3, #0	; 0x0
    e358:	0a000004 	beq	e370 <run_command+0x128>
    e35c:	e3500000 	cmp	r0, #0	; 0x0
    e360:	0a00003f 	beq	e464 <run_command+0x21c>
    e364:	e4d42001 	ldrb	r2, [r4], #1
    e368:	e2400001 	sub	r0, r0, #1	; 0x1
    e36c:	e3a0105c 	mov	r1, #92	; 0x5c
    e370:	e35c0003 	cmp	ip, #3	; 0x3
    e374:	979ff10c 	ldrls	pc, [pc, ip, lsl #2]
    e378:	ea000035 	b	e454 <run_command+0x20c>
    e37c:	0000e38c 	.word	0x0000e38c
    e380:	0000e3d0 	.word	0x0000e3d0
    e384:	0000e408 	.word	0x0000e408
    e388:	0000e418 	.word	0x0000e418
    e38c:	e251105c 	subs	r1, r1, #92	; 0x5c
    e390:	13a01001 	movne	r1, #1	; 0x1
    e394:	e3520027 	cmp	r2, #39	; 0x27
    e398:	13a03000 	movne	r3, #0	; 0x0
    e39c:	02013001 	andeq	r3, r1, #1	; 0x1
    e3a0:	e3530000 	cmp	r3, #0	; 0x0
    e3a4:	13a0c003 	movne	ip, #3	; 0x3
    e3a8:	1a000022 	bne	e438 <run_command+0x1f0>
    e3ac:	e3520024 	cmp	r2, #36	; 0x24
    e3b0:	13a03000 	movne	r3, #0	; 0x0
    e3b4:	02013001 	andeq	r3, r1, #1	; 0x1
    e3b8:	e3530000 	cmp	r3, #0	; 0x0
    e3bc:	13a0c001 	movne	ip, #1	; 0x1
    e3c0:	13a02024 	movne	r2, #36	; 0x24
    e3c4:	04c52001 	strbeq	r2, [r5], #1
    e3c8:	024ee001 	subeq	lr, lr, #1	; 0x1
    e3cc:	ea000020 	b	e454 <run_command+0x20c>
    e3d0:	e352007b 	cmp	r2, #123	; 0x7b
    e3d4:	13520028 	cmpne	r2, #40	; 0x28
    e3d8:	13a03000 	movne	r3, #0	; 0x0
    e3dc:	03a03001 	moveq	r3, #1	; 0x1
    e3e0:	03a0c002 	moveq	ip, #2	; 0x2
    e3e4:	0a00001a 	beq	e454 <run_command+0x20c>
    e3e8:	e3a01024 	mov	r1, #36	; 0x24
    e3ec:	e25ee001 	subs	lr, lr, #1	; 0x1
    e3f0:	e4c51001 	strb	r1, [r5], #1
    e3f4:	0a000015 	beq	e450 <run_command+0x208>
    e3f8:	e4c52001 	strb	r2, [r5], #1
    e3fc:	e24ee001 	sub	lr, lr, #1	; 0x1
    e400:	e1a0c003 	mov	ip, r3
    e404:	ea000012 	b	e454 <run_command+0x20c>
    e408:	e352007d 	cmp	r2, #125	; 0x7d
    e40c:	13520029 	cmpne	r2, #41	; 0x29
    e410:	0a00000e 	beq	e450 <run_command+0x208>
    e414:	ea00000e 	b	e454 <run_command+0x20c>
    e418:	e3520027 	cmp	r2, #39	; 0x27
    e41c:	13a03000 	movne	r3, #0	; 0x0
    e420:	03a03001 	moveq	r3, #1	; 0x1
    e424:	e351005c 	cmp	r1, #92	; 0x5c
    e428:	03a03000 	moveq	r3, #0	; 0x0
    e42c:	e3530000 	cmp	r3, #0	; 0x0
    e430:	0a000002 	beq	e440 <run_command+0x1f8>
    e434:	e3a0c000 	mov	ip, #0	; 0x0
    e438:	e3a02027 	mov	r2, #39	; 0x27
    e43c:	ea000004 	b	e454 <run_command+0x20c>
    e440:	e4c52001 	strb	r2, [r5], #1
    e444:	e24ee001 	sub	lr, lr, #1	; 0x1
    e448:	e3a0c003 	mov	ip, #3	; 0x3
    e44c:	ea000000 	b	e454 <run_command+0x20c>
    e450:	e3a0c000 	mov	ip, #0	; 0x0
    e454:	e1a01002 	mov	r1, r2
    e458:	e3500000 	cmp	r0, #0	; 0x0
    e45c:	135e0000 	cmpne	lr, #0	; 0x0
    e460:	1affffb2 	bne	e330 <run_command+0xe8>
    e464:	e35e0000 	cmp	lr, #0	; 0x0
    e468:	15c5b000 	strbne	fp, [r5]
    e46c:	0545e001 	strbeq	lr, [r5, #-1]
    e470:	e28d0004 	add	r0, sp, #4	; 0x4
    e474:	e1a01009 	mov	r1, r9
    e478:	ebffff4c 	bl	e1b0 <parse_line>
    e47c:	e2505000 	subs	r5, r0, #0	; 0x0
    e480:	0a00000d 	beq	e4bc <run_command+0x274>
    e484:	e59d0204 	ldr	r0, [sp, #516]
    e488:	ebffd213 	bl	2cdc <find_cmd>
    e48c:	e2504000 	subs	r4, r0, #0	; 0x0
    e490:	1a000005 	bne	e4ac <run_command+0x264>
    e494:	e59f0090 	ldr	r0, [pc, #144]	; e52c <run_command+0x2e4>
    e498:	e59d1204 	ldr	r1, [sp, #516]
    e49c:	e59f308c 	ldr	r3, [pc, #140]	; e530 <run_command+0x2e8>
    e4a0:	e1a0e00f 	mov	lr, pc
    e4a4:	e593f000 	ldr	pc, [r3]
    e4a8:	ea000003 	b	e4bc <run_command+0x274>
    e4ac:	e5943004 	ldr	r3, [r4, #4]
    e4b0:	e1550003 	cmp	r5, r3
    e4b4:	da000002 	ble	e4c4 <run_command+0x27c>
    e4b8:	ebffd1c1 	bl	2bc4 <cmd_usage>
    e4bc:	e3e06000 	mvn	r6, #0	; 0x0
    e4c0:	ea00000b 	b	e4f4 <run_command+0x2ac>
    e4c4:	e1a02005 	mov	r2, r5
    e4c8:	e1a0100a 	mov	r1, sl
    e4cc:	e1a03009 	mov	r3, r9
    e4d0:	e1a0e00f 	mov	lr, pc
    e4d4:	e594f00c 	ldr	pc, [r4, #12]
    e4d8:	e3500000 	cmp	r0, #0	; 0x0
    e4dc:	13e06000 	mvnne	r6, #0	; 0x0
    e4e0:	e5944008 	ldr	r4, [r4, #8]
    e4e4:	ebfffee5 	bl	e080 <had_ctrlc>
    e4e8:	e3500000 	cmp	r0, #0	; 0x0
    e4ec:	1a000009 	bne	e518 <run_command+0x2d0>
    e4f0:	e0077004 	and	r7, r7, r4
    e4f4:	e1a04008 	mov	r4, r8
    e4f8:	e5d43000 	ldrb	r3, [r4]
    e4fc:	e3530000 	cmp	r3, #0	; 0x0
    e500:	11a02004 	movne	r2, r4
    e504:	13a00000 	movne	r0, #0	; 0x0
    e508:	1affff7a 	bne	e2f8 <run_command+0xb0>
    e50c:	e3560000 	cmp	r6, #0	; 0x0
    e510:	01a06007 	moveq	r6, r7
    e514:	ea000000 	b	e51c <run_command+0x2d4>
    e518:	e3e06000 	mvn	r6, #0	; 0x0
    e51c:	e1a00006 	mov	r0, r6
    e520:	e28ddf93 	add	sp, sp, #588	; 0x24c
    e524:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    e528:	00014258 	.word	0x00014258
    e52c:	0001426e 	.word	0x0001426e
    e530:	0800d76c 	.word	0x0800d76c

0000e534 <cread_add_char>:
    e534:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    e538:	e3510000 	cmp	r1, #0	; 0x0
    e53c:	e24dd004 	sub	sp, sp, #4	; 0x4
    e540:	e1a07000 	mov	r7, r0
    e544:	e1a04002 	mov	r4, r2
    e548:	e1a00003 	mov	r0, r3
    e54c:	e59d6018 	ldr	r6, [sp, #24]
    e550:	e59dc01c 	ldr	ip, [sp, #28]
    e554:	1a000003 	bne	e568 <cread_add_char+0x34>
    e558:	e5922000 	ldr	r2, [r2]
    e55c:	e5933000 	ldr	r3, [r3]
    e560:	e1520003 	cmp	r2, r3
    e564:	1a000026 	bne	e604 <cread_add_char+0xd0>
    e568:	e5902000 	ldr	r2, [r0]
    e56c:	e24c3001 	sub	r3, ip, #1	; 0x1
    e570:	e1520003 	cmp	r2, r3
    e574:	9a000003 	bls	e588 <cread_add_char+0x54>
    e578:	e3a00007 	mov	r0, #7	; 0x7
    e57c:	e28dd004 	add	sp, sp, #4	; 0x4
    e580:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
    e584:	eafffef1 	b	e150 <putc>
    e588:	e3510000 	cmp	r1, #0	; 0x0
    e58c:	e2821001 	add	r1, r2, #1	; 0x1
    e590:	e5801000 	str	r1, [r0]
    e594:	0a00001a 	beq	e604 <cread_add_char+0xd0>
    e598:	e5943000 	ldr	r3, [r4]
    e59c:	e0635001 	rsb	r5, r3, r1
    e5a0:	e3550001 	cmp	r5, #1	; 0x1
    e5a4:	9a000004 	bls	e5bc <cread_add_char+0x88>
    e5a8:	e2830001 	add	r0, r3, #1	; 0x1
    e5ac:	e0860000 	add	r0, r6, r0
    e5b0:	e0861003 	add	r1, r6, r3
    e5b4:	e2452001 	sub	r2, r5, #1	; 0x1
    e5b8:	ebffcb6c 	bl	1370 <memmove>
    e5bc:	e5943000 	ldr	r3, [r4]
    e5c0:	e59f0074 	ldr	r0, [pc, #116]	; e63c <cread_add_char+0x108>
    e5c4:	e7c67003 	strb	r7, [r6, r3]
    e5c8:	e5942000 	ldr	r2, [r4]
    e5cc:	e59f306c 	ldr	r3, [pc, #108]	; e640 <cread_add_char+0x10c>
    e5d0:	e0862002 	add	r2, r6, r2
    e5d4:	e1a01005 	mov	r1, r5
    e5d8:	e1a0e00f 	mov	lr, pc
    e5dc:	e593f000 	ldr	pc, [r3]
    e5e0:	e5943000 	ldr	r3, [r4]
    e5e4:	e2833001 	add	r3, r3, #1	; 0x1
    e5e8:	e5843000 	str	r3, [r4]
    e5ec:	ea000000 	b	e5f4 <cread_add_char+0xc0>
    e5f0:	ebfffed6 	bl	e150 <putc>
    e5f4:	e2555001 	subs	r5, r5, #1	; 0x1
    e5f8:	e3a00008 	mov	r0, #8	; 0x8
    e5fc:	1afffffb 	bne	e5f0 <cread_add_char+0xbc>
    e600:	ea00000b 	b	e634 <cread_add_char+0x100>
    e604:	e5943000 	ldr	r3, [r4]
    e608:	e59f002c 	ldr	r0, [pc, #44]	; e63c <cread_add_char+0x108>
    e60c:	e7c67003 	strb	r7, [r6, r3]
    e610:	e5942000 	ldr	r2, [r4]
    e614:	e59f3024 	ldr	r3, [pc, #36]	; e640 <cread_add_char+0x10c>
    e618:	e0862002 	add	r2, r6, r2
    e61c:	e3a01001 	mov	r1, #1	; 0x1
    e620:	e1a0e00f 	mov	lr, pc
    e624:	e593f000 	ldr	pc, [r3]
    e628:	e5943000 	ldr	r3, [r4]
    e62c:	e2833001 	add	r3, r3, #1	; 0x1
    e630:	e5843000 	str	r3, [r4]
    e634:	e28dd004 	add	sp, sp, #4	; 0x4
    e638:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    e63c:	00014291 	.word	0x00014291
    e640:	0800d76c 	.word	0x0800d76c

0000e644 <readline_into_buffer>:
    e644:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    e648:	e59f367c 	ldr	r3, [pc, #1660]	; eccc <readline_into_buffer+0x688>
    e64c:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    e650:	e593e000 	ldr	lr, [r3]
    e654:	e1a06001 	mov	r6, r1
    e658:	e35e0000 	cmp	lr, #0	; 0x0
    e65c:	1a000014 	bne	e6b4 <readline_into_buffer+0x70>
    e660:	e59f3668 	ldr	r3, [pc, #1640]	; ecd0 <readline_into_buffer+0x68c>
    e664:	e3e02000 	mvn	r2, #0	; 0x0
    e668:	e5832000 	str	r2, [r3]
    e66c:	e59f3660 	ldr	r3, [pc, #1632]	; ecd4 <readline_into_buffer+0x690>
    e670:	e59fc660 	ldr	ip, [pc, #1632]	; ecd8 <readline_into_buffer+0x694>
    e674:	e583e000 	str	lr, [r3]
    e678:	e59f365c 	ldr	r3, [pc, #1628]	; ecdc <readline_into_buffer+0x698>
    e67c:	e59f465c 	ldr	r4, [pc, #1628]	; ece0 <readline_into_buffer+0x69c>
    e680:	e583e000 	str	lr, [r3]
    e684:	e59f3658 	ldr	r3, [pc, #1624]	; ece4 <readline_into_buffer+0x6a0>
    e688:	e1a0100e 	mov	r1, lr
    e68c:	e583e000 	str	lr, [r3]
    e690:	e1a0300e 	mov	r3, lr
    e694:	e784c001 	str	ip, [r4, r1]
    e698:	e2811004 	add	r1, r1, #4	; 0x4
    e69c:	e3510010 	cmp	r1, #16	; 0x10
    e6a0:	e4cc3100 	strb	r3, [ip], #256
    e6a4:	1afffffa 	bne	e694 <readline_into_buffer+0x50>
    e6a8:	e59f361c 	ldr	r3, [pc, #1564]	; eccc <readline_into_buffer+0x688>
    e6ac:	e3a02001 	mov	r2, #1	; 0x1
    e6b0:	e5832000 	str	r2, [r3]
    e6b4:	ebfffe7d 	bl	e0b0 <puts>
    e6b8:	e3a03000 	mov	r3, #0	; 0x0
    e6bc:	e58d3014 	str	r3, [sp, #20]
    e6c0:	e58d3010 	str	r3, [sp, #16]
    e6c4:	e1a05003 	mov	r5, r3
    e6c8:	e3a09001 	mov	r9, #1	; 0x1
    e6cc:	e59fb614 	ldr	fp, [pc, #1556]	; ece8 <readline_into_buffer+0x6a4>
    e6d0:	e59fa5f8 	ldr	sl, [pc, #1528]	; ecd0 <readline_into_buffer+0x68c>
    e6d4:	ea000000 	b	e6dc <readline_into_buffer+0x98>
    e6d8:	e3a05000 	mov	r5, #0	; 0x0
    e6dc:	ebfffe9d 	bl	e158 <getc>
    e6e0:	e6ef0070 	uxtb	r0, r0
    e6e4:	e350000d 	cmp	r0, #13	; 0xd
    e6e8:	1350000a 	cmpne	r0, #10	; 0xa
    e6ec:	13a04000 	movne	r4, #0	; 0x0
    e6f0:	03a04001 	moveq	r4, #1	; 0x1
    e6f4:	1a00000b 	bne	e728 <readline_into_buffer+0xe4>
    e6f8:	e59f35e8 	ldr	r3, [pc, #1512]	; ece8 <readline_into_buffer+0x6a4>
    e6fc:	e59f05e8 	ldr	r0, [pc, #1512]	; ecec <readline_into_buffer+0x6a8>
    e700:	e1a0e00f 	mov	lr, pc
    e704:	e593f000 	ldr	pc, [r3]
    e708:	e59d5010 	ldr	r5, [sp, #16]
    e70c:	e3a03000 	mov	r3, #0	; 0x0
    e710:	e7c63005 	strb	r3, [r6, r5]
    e714:	e5d63000 	ldrb	r3, [r6]
    e718:	e3530000 	cmp	r3, #0	; 0x0
    e71c:	13530021 	cmpne	r3, #33	; 0x21
    e720:	0a000162 	beq	ecb0 <readline_into_buffer+0x66c>
    e724:	ea00014e 	b	ec64 <readline_into_buffer+0x620>
    e728:	e3550000 	cmp	r5, #0	; 0x0
    e72c:	0a000033 	beq	e800 <readline_into_buffer+0x1bc>
    e730:	e3550001 	cmp	r5, #1	; 0x1
    e734:	1a00000c 	bne	e76c <readline_into_buffer+0x128>
    e738:	e350005b 	cmp	r0, #91	; 0x5b
    e73c:	05cd0009 	strbeq	r0, [sp, #9]
    e740:	03a05002 	moveq	r5, #2	; 0x2
    e744:	0affffe4 	beq	e6dc <readline_into_buffer+0x98>
    e748:	e3a0cc01 	mov	ip, #256	; 0x100
    e74c:	e5dd0008 	ldrb	r0, [sp, #8]
    e750:	e1a01009 	mov	r1, r9
    e754:	e28d2014 	add	r2, sp, #20	; 0x14
    e758:	e28d3010 	add	r3, sp, #16	; 0x10
    e75c:	e88d1040 	stm	sp, {r6, ip}
    e760:	e1a05004 	mov	r5, r4
    e764:	ebffff72 	bl	e534 <cread_add_char>
    e768:	eaffffdb 	b	e6dc <readline_into_buffer+0x98>
    e76c:	e2403041 	sub	r3, r0, #65	; 0x41
    e770:	e3530007 	cmp	r3, #7	; 0x7
    e774:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    e778:	ea000009 	b	e7a4 <readline_into_buffer+0x160>
    e77c:	0000eae8 	.word	0x0000eae8
    e780:	0000eb28 	.word	0x0000eb28
    e784:	0000e7f0 	.word	0x0000e7f0
    e788:	0000e8fc 	.word	0x0000e8fc
    e78c:	0000e7a4 	.word	0x0000e7a4
    e790:	0000e7a4 	.word	0x0000e7a4
    e794:	0000e7a4 	.word	0x0000e7a4
    e798:	0000e79c 	.word	0x0000e79c
    e79c:	e3a05000 	mov	r5, #0	; 0x0
    e7a0:	ea000043 	b	e8b4 <readline_into_buffer+0x270>
    e7a4:	e28d1018 	add	r1, sp, #24	; 0x18
    e7a8:	e0813005 	add	r3, r1, r5
    e7ac:	e5430010 	strb	r0, [r3, #-16]
    e7b0:	e2855001 	add	r5, r5, #1	; 0x1
    e7b4:	e3a04000 	mov	r4, #0	; 0x0
    e7b8:	e28d8010 	add	r8, sp, #16	; 0x10
    e7bc:	e3a07c01 	mov	r7, #256	; 0x100
    e7c0:	ea000004 	b	e7d8 <readline_into_buffer+0x194>
    e7c4:	e28dc008 	add	ip, sp, #8	; 0x8
    e7c8:	e7dc0004 	ldrb	r0, [ip, r4]
    e7cc:	e88d00c0 	stm	sp, {r6, r7}
    e7d0:	ebffff57 	bl	e534 <cread_add_char>
    e7d4:	e2844001 	add	r4, r4, #1	; 0x1
    e7d8:	e1540005 	cmp	r4, r5
    e7dc:	e1a01009 	mov	r1, r9
    e7e0:	e28d2014 	add	r2, sp, #20	; 0x14
    e7e4:	e1a03008 	mov	r3, r8
    e7e8:	1afffff5 	bne	e7c4 <readline_into_buffer+0x180>
    e7ec:	eaffffb9 	b	e6d8 <readline_into_buffer+0x94>
    e7f0:	e3a00006 	mov	r0, #6	; 0x6
    e7f4:	e350000b 	cmp	r0, #11	; 0xb
    e7f8:	e3a05000 	mov	r5, #0	; 0x0
    e7fc:	ea000001 	b	e808 <readline_into_buffer+0x1c4>
    e800:	e350000b 	cmp	r0, #11	; 0xb
    e804:	0a00005f 	beq	e988 <readline_into_buffer+0x344>
    e808:	8a00000d 	bhi	e844 <readline_into_buffer+0x200>
    e80c:	e3500004 	cmp	r0, #4	; 0x4
    e810:	0a000042 	beq	e920 <readline_into_buffer+0x2dc>
    e814:	8a000005 	bhi	e830 <readline_into_buffer+0x1ec>
    e818:	e3500002 	cmp	r0, #2	; 0x2
    e81c:	0a000037 	beq	e900 <readline_into_buffer+0x2bc>
    e820:	8a000028 	bhi	e8c8 <readline_into_buffer+0x284>
    e824:	e3500001 	cmp	r0, #1	; 0x1
    e828:	1a000106 	bne	ec48 <readline_into_buffer+0x604>
    e82c:	ea000020 	b	e8b4 <readline_into_buffer+0x270>
    e830:	e3500006 	cmp	r0, #6	; 0x6
    e834:	0a000027 	beq	e8d8 <readline_into_buffer+0x294>
    e838:	3a000066 	bcc	e9d8 <readline_into_buffer+0x394>
    e83c:	e3500008 	cmp	r0, #8	; 0x8
    e840:	ea000015 	b	e89c <readline_into_buffer+0x258>
    e844:	e3500015 	cmp	r0, #21	; 0x15
    e848:	0a000070 	beq	ea10 <readline_into_buffer+0x3cc>
    e84c:	8a000007 	bhi	e870 <readline_into_buffer+0x22c>
    e850:	e350000f 	cmp	r0, #15	; 0xf
    e854:	02299001 	eoreq	r9, r9, #1	; 0x1
    e858:	0affff9f 	beq	e6dc <readline_into_buffer+0x98>
    e85c:	e3500010 	cmp	r0, #16	; 0x10
    e860:	0a0000a0 	beq	eae8 <readline_into_buffer+0x4a4>
    e864:	e350000e 	cmp	r0, #14	; 0xe
    e868:	1a0000f6 	bne	ec48 <readline_into_buffer+0x604>
    e86c:	ea0000ad 	b	eb28 <readline_into_buffer+0x4e4>
    e870:	e350001b 	cmp	r0, #27	; 0x1b
    e874:	05cd0008 	strbeq	r0, [sp, #8]
    e878:	03a05001 	moveq	r5, #1	; 0x1
    e87c:	0affff96 	beq	e6dc <readline_into_buffer+0x98>
    e880:	8a000002 	bhi	e890 <readline_into_buffer+0x24c>
    e884:	e3500018 	cmp	r0, #24	; 0x18
    e888:	1a0000ee 	bne	ec48 <readline_into_buffer+0x604>
    e88c:	ea00005f 	b	ea10 <readline_into_buffer+0x3cc>
    e890:	e350007f 	cmp	r0, #127	; 0x7f
    e894:	0a000075 	beq	ea70 <readline_into_buffer+0x42c>
    e898:	e35000ff 	cmp	r0, #255	; 0xff
    e89c:	1a0000e9 	bne	ec48 <readline_into_buffer+0x604>
    e8a0:	ea000072 	b	ea70 <readline_into_buffer+0x42c>
    e8a4:	ebfffe29 	bl	e150 <putc>
    e8a8:	e59d3014 	ldr	r3, [sp, #20]
    e8ac:	e2433001 	sub	r3, r3, #1	; 0x1
    e8b0:	e58d3014 	str	r3, [sp, #20]
    e8b4:	e59d3014 	ldr	r3, [sp, #20]
    e8b8:	e3a00008 	mov	r0, #8	; 0x8
    e8bc:	e3530000 	cmp	r3, #0	; 0x0
    e8c0:	1afffff7 	bne	e8a4 <readline_into_buffer+0x260>
    e8c4:	eaffff84 	b	e6dc <readline_into_buffer+0x98>
    e8c8:	e3a03000 	mov	r3, #0	; 0x0
    e8cc:	e5c63000 	strb	r3, [r6]
    e8d0:	e3e00000 	mvn	r0, #0	; 0x0
    e8d4:	ea0000fa 	b	ecc4 <readline_into_buffer+0x680>
    e8d8:	e59d2014 	ldr	r2, [sp, #20]
    e8dc:	e59d3010 	ldr	r3, [sp, #16]
    e8e0:	e1520003 	cmp	r2, r3
    e8e4:	2affff7c 	bcs	e6dc <readline_into_buffer+0x98>
    e8e8:	e7d60002 	ldrb	r0, [r6, r2]
    e8ec:	ebfffe17 	bl	e150 <putc>
    e8f0:	e59d3014 	ldr	r3, [sp, #20]
    e8f4:	e2833001 	add	r3, r3, #1	; 0x1
    e8f8:	ea0000d0 	b	ec40 <readline_into_buffer+0x5fc>
    e8fc:	e3a05000 	mov	r5, #0	; 0x0
    e900:	e59d3014 	ldr	r3, [sp, #20]
    e904:	e3530000 	cmp	r3, #0	; 0x0
    e908:	0affff73 	beq	e6dc <readline_into_buffer+0x98>
    e90c:	e3a00008 	mov	r0, #8	; 0x8
    e910:	ebfffe0e 	bl	e150 <putc>
    e914:	e59d3014 	ldr	r3, [sp, #20]
    e918:	e2433001 	sub	r3, r3, #1	; 0x1
    e91c:	ea0000c7 	b	ec40 <readline_into_buffer+0x5fc>
    e920:	e59d2014 	ldr	r2, [sp, #20]
    e924:	e59d3010 	ldr	r3, [sp, #16]
    e928:	e1520003 	cmp	r2, r3
    e92c:	2affff6a 	bcs	e6dc <readline_into_buffer+0x98>
    e930:	e2433001 	sub	r3, r3, #1	; 0x1
    e934:	e0534002 	subs	r4, r3, r2
    e938:	0a00000a 	beq	e968 <readline_into_buffer+0x324>
    e93c:	e2821001 	add	r1, r2, #1	; 0x1
    e940:	e0861001 	add	r1, r6, r1
    e944:	e0860002 	add	r0, r6, r2
    e948:	e1a02004 	mov	r2, r4
    e94c:	ebffca87 	bl	1370 <memmove>
    e950:	e59d2014 	ldr	r2, [sp, #20]
    e954:	e59f0394 	ldr	r0, [pc, #916]	; ecf0 <readline_into_buffer+0x6ac>
    e958:	e0862002 	add	r2, r6, r2
    e95c:	e1a01004 	mov	r1, r4
    e960:	e1a0e00f 	mov	lr, pc
    e964:	e59bf000 	ldr	pc, [fp]
    e968:	e3a00020 	mov	r0, #32	; 0x20
    e96c:	ebfffdf7 	bl	e150 <putc>
    e970:	e2444001 	sub	r4, r4, #1	; 0x1
    e974:	e3a00008 	mov	r0, #8	; 0x8
    e978:	ebfffdf4 	bl	e150 <putc>
    e97c:	e3740001 	cmn	r4, #1	; 0x1
    e980:	1afffffa 	bne	e970 <readline_into_buffer+0x32c>
    e984:	ea000053 	b	ead8 <readline_into_buffer+0x494>
    e988:	e59d2014 	ldr	r2, [sp, #20]
    e98c:	e59d3010 	ldr	r3, [sp, #16]
    e990:	e1520003 	cmp	r2, r3
    e994:	31a04002 	movcc	r4, r2
    e998:	3a000002 	bcc	e9a8 <readline_into_buffer+0x364>
    e99c:	eaffff4e 	b	e6dc <readline_into_buffer+0x98>
    e9a0:	ebfffdea 	bl	e150 <putc>
    e9a4:	e2844001 	add	r4, r4, #1	; 0x1
    e9a8:	e59d3010 	ldr	r3, [sp, #16]
    e9ac:	e3a00020 	mov	r0, #32	; 0x20
    e9b0:	e1540003 	cmp	r4, r3
    e9b4:	3afffff9 	bcc	e9a0 <readline_into_buffer+0x35c>
    e9b8:	ea000000 	b	e9c0 <readline_into_buffer+0x37c>
    e9bc:	ebfffde3 	bl	e150 <putc>
    e9c0:	e59d2014 	ldr	r2, [sp, #20]
    e9c4:	e3a00008 	mov	r0, #8	; 0x8
    e9c8:	e1540002 	cmp	r4, r2
    e9cc:	e2444001 	sub	r4, r4, #1	; 0x1
    e9d0:	8afffff9 	bhi	e9bc <readline_into_buffer+0x378>
    e9d4:	ea000023 	b	ea68 <readline_into_buffer+0x424>
    e9d8:	e59d3014 	ldr	r3, [sp, #20]
    e9dc:	e59d1010 	ldr	r1, [sp, #16]
    e9e0:	e1530001 	cmp	r3, r1
    e9e4:	2affff3c 	bcs	e6dc <readline_into_buffer+0x98>
    e9e8:	e0631001 	rsb	r1, r3, r1
    e9ec:	e0862003 	add	r2, r6, r3
    e9f0:	e59f02f8 	ldr	r0, [pc, #760]	; ecf0 <readline_into_buffer+0x6ac>
    e9f4:	e1a0e00f 	mov	lr, pc
    e9f8:	e59bf000 	ldr	pc, [fp]
    e9fc:	ea00008e 	b	ec3c <readline_into_buffer+0x5f8>
    ea00:	ebfffdd2 	bl	e150 <putc>
    ea04:	e59d3014 	ldr	r3, [sp, #20]
    ea08:	e2433001 	sub	r3, r3, #1	; 0x1
    ea0c:	e58d3014 	str	r3, [sp, #20]
    ea10:	e59d2014 	ldr	r2, [sp, #20]
    ea14:	e3a00008 	mov	r0, #8	; 0x8
    ea18:	e3520000 	cmp	r2, #0	; 0x0
    ea1c:	1afffff7 	bne	ea00 <readline_into_buffer+0x3bc>
    ea20:	e59d3010 	ldr	r3, [sp, #16]
    ea24:	e3530000 	cmp	r3, #0	; 0x0
    ea28:	11a04002 	movne	r4, r2
    ea2c:	1a000002 	bne	ea3c <readline_into_buffer+0x3f8>
    ea30:	eaffff29 	b	e6dc <readline_into_buffer+0x98>
    ea34:	ebfffdc5 	bl	e150 <putc>
    ea38:	e2844001 	add	r4, r4, #1	; 0x1
    ea3c:	e59d3010 	ldr	r3, [sp, #16]
    ea40:	e3a00020 	mov	r0, #32	; 0x20
    ea44:	e1540003 	cmp	r4, r3
    ea48:	3afffff9 	bcc	ea34 <readline_into_buffer+0x3f0>
    ea4c:	ea000000 	b	ea54 <readline_into_buffer+0x410>
    ea50:	ebfffdbe 	bl	e150 <putc>
    ea54:	e59d2014 	ldr	r2, [sp, #20]
    ea58:	e3a00008 	mov	r0, #8	; 0x8
    ea5c:	e1540002 	cmp	r4, r2
    ea60:	e2444001 	sub	r4, r4, #1	; 0x1
    ea64:	8afffff9 	bhi	ea50 <readline_into_buffer+0x40c>
    ea68:	e58d2010 	str	r2, [sp, #16]
    ea6c:	eaffff1a 	b	e6dc <readline_into_buffer+0x98>
    ea70:	e59d0014 	ldr	r0, [sp, #20]
    ea74:	e3500000 	cmp	r0, #0	; 0x0
    ea78:	0affff17 	beq	e6dc <readline_into_buffer+0x98>
    ea7c:	e59d2010 	ldr	r2, [sp, #16]
    ea80:	e2403001 	sub	r3, r0, #1	; 0x1
    ea84:	e0604002 	rsb	r4, r0, r2
    ea88:	e0861000 	add	r1, r6, r0
    ea8c:	e1a02004 	mov	r2, r4
    ea90:	e0860003 	add	r0, r6, r3
    ea94:	e58d3014 	str	r3, [sp, #20]
    ea98:	ebffca34 	bl	1370 <memmove>
    ea9c:	e3a00008 	mov	r0, #8	; 0x8
    eaa0:	ebfffdaa 	bl	e150 <putc>
    eaa4:	e59d2014 	ldr	r2, [sp, #20]
    eaa8:	e59f0240 	ldr	r0, [pc, #576]	; ecf0 <readline_into_buffer+0x6ac>
    eaac:	e0862002 	add	r2, r6, r2
    eab0:	e1a01004 	mov	r1, r4
    eab4:	e1a0e00f 	mov	lr, pc
    eab8:	e59bf000 	ldr	pc, [fp]
    eabc:	e3a00020 	mov	r0, #32	; 0x20
    eac0:	ebfffda2 	bl	e150 <putc>
    eac4:	e2444001 	sub	r4, r4, #1	; 0x1
    eac8:	e3a00008 	mov	r0, #8	; 0x8
    eacc:	ebfffd9f 	bl	e150 <putc>
    ead0:	e3740001 	cmn	r4, #1	; 0x1
    ead4:	1afffffa 	bne	eac4 <readline_into_buffer+0x480>
    ead8:	e59d3010 	ldr	r3, [sp, #16]
    eadc:	e2433001 	sub	r3, r3, #1	; 0x1
    eae0:	e58d3010 	str	r3, [sp, #16]
    eae4:	eafffefc 	b	e6dc <readline_into_buffer+0x98>
    eae8:	e59a2000 	ldr	r2, [sl]
    eaec:	e3520000 	cmp	r2, #0	; 0x0
    eaf0:	ba000024 	blt	eb88 <readline_into_buffer+0x544>
    eaf4:	e2423001 	sub	r3, r2, #1	; 0x1
    eaf8:	e3730001 	cmn	r3, #1	; 0x1
    eafc:	059f01d0 	ldreq	r0, [pc, #464]	; ecd4 <readline_into_buffer+0x690>
    eb00:	e58a3000 	str	r3, [sl]
    eb04:	05903000 	ldreq	r3, [r0]
    eb08:	e59fc1cc 	ldr	ip, [pc, #460]	; ecdc <readline_into_buffer+0x698>
    eb0c:	058a3000 	streq	r3, [sl]
    eb10:	e59a1000 	ldr	r1, [sl]
    eb14:	e59c3000 	ldr	r3, [ip]
    eb18:	e1510003 	cmp	r1, r3
    eb1c:	058a2000 	streq	r2, [sl]
    eb20:	1a000014 	bne	eb78 <readline_into_buffer+0x534>
    eb24:	ea000017 	b	eb88 <readline_into_buffer+0x544>
    eb28:	e59a1000 	ldr	r1, [sl]
    eb2c:	e3510000 	cmp	r1, #0	; 0x0
    eb30:	ba000014 	blt	eb88 <readline_into_buffer+0x544>
    eb34:	e59f21a0 	ldr	r2, [pc, #416]	; ecdc <readline_into_buffer+0x698>
    eb38:	e5923000 	ldr	r3, [r2]
    eb3c:	e1510003 	cmp	r1, r3
    eb40:	0a000010 	beq	eb88 <readline_into_buffer+0x544>
    eb44:	e59f3188 	ldr	r3, [pc, #392]	; ecd4 <readline_into_buffer+0x690>
    eb48:	e59fc18c 	ldr	ip, [pc, #396]	; ecdc <readline_into_buffer+0x698>
    eb4c:	e5932000 	ldr	r2, [r3]
    eb50:	e2813001 	add	r3, r1, #1	; 0x1
    eb54:	e1530002 	cmp	r3, r2
    eb58:	e58a3000 	str	r3, [sl]
    eb5c:	c3a03000 	movgt	r3, #0	; 0x0
    eb60:	c58a3000 	strgt	r3, [sl]
    eb64:	e59a1000 	ldr	r1, [sl]
    eb68:	e59c3000 	ldr	r3, [ip]
    eb6c:	e1510003 	cmp	r1, r3
    eb70:	059f517c 	ldreq	r5, [pc, #380]	; ecf4 <readline_into_buffer+0x6b0>
    eb74:	0a00000a 	beq	eba4 <readline_into_buffer+0x560>
    eb78:	e59f0160 	ldr	r0, [pc, #352]	; ece0 <readline_into_buffer+0x69c>
    eb7c:	e7905101 	ldr	r5, [r0, r1, lsl #2]
    eb80:	e3550000 	cmp	r5, #0	; 0x0
    eb84:	1a000006 	bne	eba4 <readline_into_buffer+0x560>
    eb88:	e3a00007 	mov	r0, #7	; 0x7
    eb8c:	ebfffd6f 	bl	e150 <putc>
    eb90:	eafffed0 	b	e6d8 <readline_into_buffer+0x94>
    eb94:	ebfffd6d 	bl	e150 <putc>
    eb98:	e59d3014 	ldr	r3, [sp, #20]
    eb9c:	e2433001 	sub	r3, r3, #1	; 0x1
    eba0:	e58d3014 	str	r3, [sp, #20]
    eba4:	e59d2014 	ldr	r2, [sp, #20]
    eba8:	e3a00008 	mov	r0, #8	; 0x8
    ebac:	e3520000 	cmp	r2, #0	; 0x0
    ebb0:	1afffff7 	bne	eb94 <readline_into_buffer+0x550>
    ebb4:	e59d3010 	ldr	r3, [sp, #16]
    ebb8:	e3530000 	cmp	r3, #0	; 0x0
    ebbc:	11a04002 	movne	r4, r2
    ebc0:	1a000002 	bne	ebd0 <readline_into_buffer+0x58c>
    ebc4:	ea00000d 	b	ec00 <readline_into_buffer+0x5bc>
    ebc8:	ebfffd60 	bl	e150 <putc>
    ebcc:	e2844001 	add	r4, r4, #1	; 0x1
    ebd0:	e59d3010 	ldr	r3, [sp, #16]
    ebd4:	e3a00020 	mov	r0, #32	; 0x20
    ebd8:	e1540003 	cmp	r4, r3
    ebdc:	3afffff9 	bcc	ebc8 <readline_into_buffer+0x584>
    ebe0:	ea000000 	b	ebe8 <readline_into_buffer+0x5a4>
    ebe4:	ebfffd59 	bl	e150 <putc>
    ebe8:	e59d2014 	ldr	r2, [sp, #20]
    ebec:	e3a00008 	mov	r0, #8	; 0x8
    ebf0:	e1540002 	cmp	r4, r2
    ebf4:	e2444001 	sub	r4, r4, #1	; 0x1
    ebf8:	8afffff9 	bhi	ebe4 <readline_into_buffer+0x5a0>
    ebfc:	e58d2010 	str	r2, [sp, #16]
    ec00:	e1a01005 	mov	r1, r5
    ec04:	e1a00006 	mov	r0, r6
    ec08:	ebffc95e 	bl	1188 <strcpy>
    ec0c:	e1a00006 	mov	r0, r6
    ec10:	ebffc9b3 	bl	12e4 <strlen>
    ec14:	e59d3014 	ldr	r3, [sp, #20]
    ec18:	e58d0010 	str	r0, [sp, #16]
    ec1c:	e1530000 	cmp	r3, r0
    ec20:	2afffeac 	bcs	e6d8 <readline_into_buffer+0x94>
    ec24:	e0631000 	rsb	r1, r3, r0
    ec28:	e0862003 	add	r2, r6, r3
    ec2c:	e59f00bc 	ldr	r0, [pc, #188]	; ecf0 <readline_into_buffer+0x6ac>
    ec30:	e1a0e00f 	mov	lr, pc
    ec34:	e59bf000 	ldr	pc, [fp]
    ec38:	e3a05000 	mov	r5, #0	; 0x0
    ec3c:	e59d3010 	ldr	r3, [sp, #16]
    ec40:	e58d3014 	str	r3, [sp, #20]
    ec44:	eafffea4 	b	e6dc <readline_into_buffer+0x98>
    ec48:	e3a0cc01 	mov	ip, #256	; 0x100
    ec4c:	e1a01009 	mov	r1, r9
    ec50:	e28d2014 	add	r2, sp, #20	; 0x14
    ec54:	e28d3010 	add	r3, sp, #16	; 0x10
    ec58:	e88d1040 	stm	sp, {r6, ip}
    ec5c:	ebfffe34 	bl	e534 <cread_add_char>
    ec60:	eafffe9d 	b	e6dc <readline_into_buffer+0x98>
    ec64:	e59f4070 	ldr	r4, [pc, #112]	; ecdc <readline_into_buffer+0x698>
    ec68:	e59f3070 	ldr	r3, [pc, #112]	; ece0 <readline_into_buffer+0x69c>
    ec6c:	e5942000 	ldr	r2, [r4]
    ec70:	e1a01006 	mov	r1, r6
    ec74:	e7930102 	ldr	r0, [r3, r2, lsl #2]
    ec78:	ebffc942 	bl	1188 <strcpy>
    ec7c:	e5943000 	ldr	r3, [r4]
    ec80:	e59f104c 	ldr	r1, [pc, #76]	; ecd4 <readline_into_buffer+0x690>
    ec84:	e2833001 	add	r3, r3, #1	; 0x1
    ec88:	e5912000 	ldr	r2, [r1]
    ec8c:	e3530003 	cmp	r3, #3	; 0x3
    ec90:	c3a03000 	movgt	r3, #0	; 0x0
    ec94:	e1530002 	cmp	r3, r2
    ec98:	e59f2044 	ldr	r2, [pc, #68]	; ece4 <readline_into_buffer+0x6a0>
    ec9c:	e5843000 	str	r3, [r4]
    eca0:	c5813000 	strgt	r3, [r1]
    eca4:	e5923000 	ldr	r3, [r2]
    eca8:	e2833001 	add	r3, r3, #1	; 0x1
    ecac:	e5823000 	str	r3, [r2]
    ecb0:	e59f3024 	ldr	r3, [pc, #36]	; ecdc <readline_into_buffer+0x698>
    ecb4:	e1a00005 	mov	r0, r5
    ecb8:	e5932000 	ldr	r2, [r3]
    ecbc:	e59f300c 	ldr	r3, [pc, #12]	; ecd0 <readline_into_buffer+0x68c>
    ecc0:	e5832000 	str	r2, [r3]
    ecc4:	e28dd01c 	add	sp, sp, #28	; 0x1c
    ecc8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    eccc:	0800d314 	.word	0x0800d314
    ecd0:	0800d19c 	.word	0x0800d19c
    ecd4:	0800d318 	.word	0x0800d318
    ecd8:	0800d8fc 	.word	0x0800d8fc
    ecdc:	0800d31c 	.word	0x0800d31c
    ece0:	0800d8ec 	.word	0x0800d8ec
    ece4:	0800d310 	.word	0x0800d310
    ece8:	0800d76c 	.word	0x0800d76c
    ecec:	0001247a 	.word	0x0001247a
    ecf0:	00014291 	.word	0x00014291
    ecf4:	0001247b 	.word	0x0001247b

0000ecf8 <readline>:
    ecf8:	e59f1000 	ldr	r1, [pc, #0]	; ed00 <readline+0x8>
    ecfc:	eafffe50 	b	e644 <readline_into_buffer>
    ed00:	0800d7ec 	.word	0x0800d7ec

0000ed04 <main_loop>:
    ed04:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ed08:	e24dd004 	sub	sp, sp, #4	; 0x4
    ed0c:	e59f0030 	ldr	r0, [pc, #48]	; ed44 <main_loop+0x40>
    ed10:	ebfffff8 	bl	ecf8 <readline>
    ed14:	e1a03000 	mov	r3, r0
    ed18:	e3730001 	cmn	r3, #1	; 0x1
    ed1c:	e3a01000 	mov	r1, #0	; 0x0
    ed20:	e59f0020 	ldr	r0, [pc, #32]	; ed48 <main_loop+0x44>
    ed24:	1a000002 	bne	ed34 <main_loop+0x30>
    ed28:	e59f001c 	ldr	r0, [pc, #28]	; ed4c <main_loop+0x48>
    ed2c:	ebfffcdf 	bl	e0b0 <puts>
    ed30:	eafffff5 	b	ed0c <main_loop+0x8>
    ed34:	e3530000 	cmp	r3, #0	; 0x0
    ed38:	dafffff3 	ble	ed0c <main_loop+0x8>
    ed3c:	ebfffd41 	bl	e248 <run_command>
    ed40:	eafffff1 	b	ed0c <main_loop+0x8>
    ed44:	000142b1 	.word	0x000142b1
    ed48:	0800d7ec 	.word	0x0800d7ec
    ed4c:	0001247a 	.word	0x0001247a

0000ed50 <parse_num>:
    ed50:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    ed54:	e1a08001 	mov	r8, r1
    ed58:	e1a07002 	mov	r7, r2
    ed5c:	e1a06003 	mov	r6, r3
    ed60:	ea000000 	b	ed68 <parse_num+0x18>
    ed64:	e2800001 	add	r0, r0, #1	; 0x1
    ed68:	e5d03000 	ldrb	r3, [r0]
    ed6c:	e3530020 	cmp	r3, #32	; 0x20
    ed70:	0afffffb 	beq	ed64 <parse_num+0x14>
    ed74:	ea000037 	b	ee58 <parse_num+0x108>
    ed78:	e3520000 	cmp	r2, #0	; 0x0
    ed7c:	0a00000a 	beq	edac <parse_num+0x5c>
    ed80:	e3530030 	cmp	r3, #48	; 0x30
    ed84:	1a000008 	bne	edac <parse_num+0x5c>
    ed88:	e5d02001 	ldrb	r2, [r0, #1]
    ed8c:	e2423041 	sub	r3, r2, #65	; 0x41
    ed90:	e6ef3073 	uxtb	r3, r3
    ed94:	e3530019 	cmp	r3, #25	; 0x19
    ed98:	92823020 	addls	r3, r2, #32	; 0x20
    ed9c:	96ef2073 	uxtbls	r2, r3
    eda0:	e3520078 	cmp	r2, #120	; 0x78
    eda4:	02800002 	addeq	r0, r0, #2	; 0x2
    eda8:	03a04010 	moveq	r4, #16	; 0x10
    edac:	e4d0c001 	ldrb	ip, [r0], #1
    edb0:	e24c3030 	sub	r3, ip, #48	; 0x30
    edb4:	e24c2041 	sub	r2, ip, #65	; 0x41
    edb8:	e6ef3073 	uxtb	r3, r3
    edbc:	e6ef2072 	uxtb	r2, r2
    edc0:	e3530009 	cmp	r3, #9	; 0x9
    edc4:	83a01000 	movhi	r1, #0	; 0x0
    edc8:	93a01001 	movls	r1, #1	; 0x1
    edcc:	e3520005 	cmp	r2, #5	; 0x5
    edd0:	83a02000 	movhi	r2, #0	; 0x0
    edd4:	93a02001 	movls	r2, #1	; 0x1
    edd8:	e1913002 	orrs	r3, r1, r2
    eddc:	1a000003 	bne	edf0 <parse_num+0xa0>
    ede0:	e24c3061 	sub	r3, ip, #97	; 0x61
    ede4:	e6ef3073 	uxtb	r3, r3
    ede8:	e3530005 	cmp	r3, #5	; 0x5
    edec:	8a00000f 	bhi	ee30 <parse_num+0xe0>
    edf0:	e3510000 	cmp	r1, #0	; 0x0
    edf4:	124c2030 	subne	r2, ip, #48	; 0x30
    edf8:	1a000007 	bne	ee1c <parse_num+0xcc>
    edfc:	e24c3061 	sub	r3, ip, #97	; 0x61
    ee00:	e6ef3073 	uxtb	r3, r3
    ee04:	e3530005 	cmp	r3, #5	; 0x5
    ee08:	924c2057 	subls	r2, ip, #87	; 0x57
    ee0c:	9a000002 	bls	ee1c <parse_num+0xcc>
    ee10:	e3520000 	cmp	r2, #0	; 0x0
    ee14:	0a000002 	beq	ee24 <parse_num+0xd4>
    ee18:	e24c2037 	sub	r2, ip, #55	; 0x37
    ee1c:	e1520004 	cmp	r2, r4
    ee20:	aa000002 	bge	ee30 <parse_num+0xe0>
    ee24:	e0252495 	mla	r5, r5, r4, r2
    ee28:	e3a02000 	mov	r2, #0	; 0x0
    ee2c:	ea00000c 	b	ee64 <parse_num+0x114>
    ee30:	e3560000 	cmp	r6, #0	; 0x0
    ee34:	0a000012 	beq	ee84 <parse_num+0x134>
    ee38:	e3a03000 	mov	r3, #0	; 0x0
    ee3c:	e7d62003 	ldrb	r2, [r6, r3]
    ee40:	e2833001 	add	r3, r3, #1	; 0x1
    ee44:	e3520000 	cmp	r2, #0	; 0x0
    ee48:	0a00000d 	beq	ee84 <parse_num+0x134>
    ee4c:	e15c0002 	cmp	ip, r2
    ee50:	1afffff9 	bne	ee3c <parse_num+0xec>
    ee54:	ea000005 	b	ee70 <parse_num+0x120>
    ee58:	e3a02001 	mov	r2, #1	; 0x1
    ee5c:	e3a0400a 	mov	r4, #10	; 0xa
    ee60:	e3a05000 	mov	r5, #0	; 0x0
    ee64:	e5d03000 	ldrb	r3, [r0]
    ee68:	e3530000 	cmp	r3, #0	; 0x0
    ee6c:	1affffc1 	bne	ed78 <parse_num+0x28>
    ee70:	e3570000 	cmp	r7, #0	; 0x0
    ee74:	15870000 	strne	r0, [r7]
    ee78:	e5885000 	str	r5, [r8]
    ee7c:	e3a00001 	mov	r0, #1	; 0x1
    ee80:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    ee84:	e3a00000 	mov	r0, #0	; 0x0
    ee88:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000ee8c <xyzModem_stream_close>:
    ee8c:	e59f2044 	ldr	r2, [pc, #68]	; eed8 <xyzModem_stream_close+0x4c>
    ee90:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ee94:	e5920428 	ldr	r0, [r2, #1064]
    ee98:	e59f303c 	ldr	r3, [pc, #60]	; eedc <xyzModem_stream_close+0x50>
    ee9c:	e592c424 	ldr	ip, [r2, #1060]
    eea0:	e592e418 	ldr	lr, [r2, #1048]
    eea4:	e3500000 	cmp	r0, #0	; 0x0
    eea8:	e24dd00c 	sub	sp, sp, #12	; 0xc
    eeac:	e59f102c 	ldr	r1, [pc, #44]	; eee0 <xyzModem_stream_close+0x54>
    eeb0:	01a01003 	moveq	r1, r3
    eeb4:	e59f3028 	ldr	r3, [pc, #40]	; eee4 <xyzModem_stream_close+0x58>
    eeb8:	e88d5000 	stm	sp, {ip, lr}
    eebc:	e593c000 	ldr	ip, [r3]
    eec0:	e59f0020 	ldr	r0, [pc, #32]	; eee8 <xyzModem_stream_close+0x5c>
    eec4:	e5923420 	ldr	r3, [r2, #1056]
    eec8:	e592241c 	ldr	r2, [r2, #1052]
    eecc:	e12fff3c 	blx	ip
    eed0:	e28dd00c 	add	sp, sp, #12	; 0xc
    eed4:	e8bd8000 	pop	{pc}
    eed8:	0800d330 	.word	0x0800d330
    eedc:	000142b5 	.word	0x000142b5
    eee0:	000142bb 	.word	0x000142bb
    eee4:	0800d76c 	.word	0x0800d76c
    eee8:	000142bf 	.word	0x000142bf

0000eeec <xyzModem_error>:
    eeec:	e2800008 	add	r0, r0, #8	; 0x8
    eef0:	e3500007 	cmp	r0, #7	; 0x7
    eef4:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    eef8:	ea000017 	b	ef5c <xyzModem_error+0x70>
    eefc:	0000ef54 	.word	0x0000ef54
    ef00:	0000ef4c 	.word	0x0000ef4c
    ef04:	0000ef44 	.word	0x0000ef44
    ef08:	0000ef3c 	.word	0x0000ef3c
    ef0c:	0000ef34 	.word	0x0000ef34
    ef10:	0000ef2c 	.word	0x0000ef2c
    ef14:	0000ef1c 	.word	0x0000ef1c
    ef18:	0000ef24 	.word	0x0000ef24
    ef1c:	e59f0040 	ldr	r0, [pc, #64]	; ef64 <xyzModem_error+0x78>
    ef20:	e12fff1e 	bx	lr
    ef24:	e59f003c 	ldr	r0, [pc, #60]	; ef68 <xyzModem_error+0x7c>
    ef28:	e12fff1e 	bx	lr
    ef2c:	e59f0038 	ldr	r0, [pc, #56]	; ef6c <xyzModem_error+0x80>
    ef30:	e12fff1e 	bx	lr
    ef34:	e59f0034 	ldr	r0, [pc, #52]	; ef70 <xyzModem_error+0x84>
    ef38:	e12fff1e 	bx	lr
    ef3c:	e59f0030 	ldr	r0, [pc, #48]	; ef74 <xyzModem_error+0x88>
    ef40:	e12fff1e 	bx	lr
    ef44:	e59f002c 	ldr	r0, [pc, #44]	; ef78 <xyzModem_error+0x8c>
    ef48:	e12fff1e 	bx	lr
    ef4c:	e59f0028 	ldr	r0, [pc, #40]	; ef7c <xyzModem_error+0x90>
    ef50:	e12fff1e 	bx	lr
    ef54:	e59f0024 	ldr	r0, [pc, #36]	; ef80 <xyzModem_error+0x94>
    ef58:	e12fff1e 	bx	lr
    ef5c:	e59f0020 	ldr	r0, [pc, #32]	; ef84 <xyzModem_error+0x98>
    ef60:	e12fff1e 	bx	lr
    ef64:	00014300 	.word	0x00014300
    ef68:	00014320 	.word	0x00014320
    ef6c:	00014332 	.word	0x00014332
    ef70:	0001433c 	.word	0x0001433c
    ef74:	00014348 	.word	0x00014348
    ef78:	00014352 	.word	0x00014352
    ef7c:	00014362 	.word	0x00014362
    ef80:	00014375 	.word	0x00014375
    ef84:	0001438a 	.word	0x0001438a

0000ef88 <delay>:
    ef88:	e12fff1e 	bx	lr

0000ef8c <init_num>:
    ef8c:	e59f305c 	ldr	r3, [pc, #92]	; eff0 <init_num+0x64>
    ef90:	e3a01001 	mov	r1, #1	; 0x1
    ef94:	e5c31000 	strb	r1, [r3]
    ef98:	e59f3054 	ldr	r3, [pc, #84]	; eff4 <init_num+0x68>
    ef9c:	e3a02000 	mov	r2, #0	; 0x0
    efa0:	e5c32000 	strb	r2, [r3]
    efa4:	e59f304c 	ldr	r3, [pc, #76]	; eff8 <init_num+0x6c>
    efa8:	e5c32000 	strb	r2, [r3]
    efac:	e59f3048 	ldr	r3, [pc, #72]	; effc <init_num+0x70>
    efb0:	e5c32000 	strb	r2, [r3]
    efb4:	e59f3044 	ldr	r3, [pc, #68]	; f000 <init_num+0x74>
    efb8:	e5c32000 	strb	r2, [r3]
    efbc:	e59f3040 	ldr	r3, [pc, #64]	; f004 <init_num+0x78>
    efc0:	e5832000 	str	r2, [r3]
    efc4:	e59f303c 	ldr	r3, [pc, #60]	; f008 <init_num+0x7c>
    efc8:	e5c32000 	strb	r2, [r3]
    efcc:	e59f3038 	ldr	r3, [pc, #56]	; f00c <init_num+0x80>
    efd0:	e5c32000 	strb	r2, [r3]
    efd4:	e59f3034 	ldr	r3, [pc, #52]	; f010 <init_num+0x84>
    efd8:	e5c32000 	strb	r2, [r3]
    efdc:	e59f3030 	ldr	r3, [pc, #48]	; f014 <init_num+0x88>
    efe0:	e5c32000 	strb	r2, [r3]
    efe4:	e59f302c 	ldr	r3, [pc, #44]	; f018 <init_num+0x8c>
    efe8:	e5c32000 	strb	r2, [r3]
    efec:	e12fff1e 	bx	lr
    eff0:	0800d1a0 	.word	0x0800d1a0
    eff4:	0800d32d 	.word	0x0800d32d
    eff8:	0800d320 	.word	0x0800d320
    effc:	0800d321 	.word	0x0800d321
    f000:	0800d322 	.word	0x0800d322
    f004:	0800d324 	.word	0x0800d324
    f008:	0800d328 	.word	0x0800d328
    f00c:	0800d329 	.word	0x0800d329
    f010:	0800d32a 	.word	0x0800d32a
    f014:	0800d32b 	.word	0x0800d32b
    f018:	0800d32c 	.word	0x0800d32c

0000f01c <out_message>:
    f01c:	e92d4010 	push	{r4, lr}
    f020:	e59f30c8 	ldr	r3, [pc, #200]	; f0f0 <out_message+0xd4>
    f024:	e59f40c8 	ldr	r4, [pc, #200]	; f0f4 <out_message+0xd8>
    f028:	e5d31000 	ldrb	r1, [r3]
    f02c:	e59f00c4 	ldr	r0, [pc, #196]	; f0f8 <out_message+0xdc>
    f030:	e1a0e00f 	mov	lr, pc
    f034:	e594f000 	ldr	pc, [r4]
    f038:	e59f30bc 	ldr	r3, [pc, #188]	; f0fc <out_message+0xe0>
    f03c:	e59f00bc 	ldr	r0, [pc, #188]	; f100 <out_message+0xe4>
    f040:	e5d31000 	ldrb	r1, [r3]
    f044:	e1a0e00f 	mov	lr, pc
    f048:	e594f000 	ldr	pc, [r4]
    f04c:	e59f30b0 	ldr	r3, [pc, #176]	; f104 <out_message+0xe8>
    f050:	e59f00b0 	ldr	r0, [pc, #176]	; f108 <out_message+0xec>
    f054:	e5931000 	ldr	r1, [r3]
    f058:	e1a0e00f 	mov	lr, pc
    f05c:	e594f000 	ldr	pc, [r4]
    f060:	e59f30a4 	ldr	r3, [pc, #164]	; f10c <out_message+0xf0>
    f064:	e59f00a4 	ldr	r0, [pc, #164]	; f110 <out_message+0xf4>
    f068:	e5d31000 	ldrb	r1, [r3]
    f06c:	e1a0e00f 	mov	lr, pc
    f070:	e594f000 	ldr	pc, [r4]
    f074:	e59f3098 	ldr	r3, [pc, #152]	; f114 <out_message+0xf8>
    f078:	e59f0098 	ldr	r0, [pc, #152]	; f118 <out_message+0xfc>
    f07c:	e5d31000 	ldrb	r1, [r3]
    f080:	e1a0e00f 	mov	lr, pc
    f084:	e594f000 	ldr	pc, [r4]
    f088:	e59f308c 	ldr	r3, [pc, #140]	; f11c <out_message+0x100>
    f08c:	e59f008c 	ldr	r0, [pc, #140]	; f120 <out_message+0x104>
    f090:	e5d31000 	ldrb	r1, [r3]
    f094:	e1a0e00f 	mov	lr, pc
    f098:	e594f000 	ldr	pc, [r4]
    f09c:	e59f3080 	ldr	r3, [pc, #128]	; f124 <out_message+0x108>
    f0a0:	e59f0080 	ldr	r0, [pc, #128]	; f128 <out_message+0x10c>
    f0a4:	e5d31000 	ldrb	r1, [r3]
    f0a8:	e1a0e00f 	mov	lr, pc
    f0ac:	e594f000 	ldr	pc, [r4]
    f0b0:	e59f3074 	ldr	r3, [pc, #116]	; f12c <out_message+0x110>
    f0b4:	e59f0074 	ldr	r0, [pc, #116]	; f130 <out_message+0x114>
    f0b8:	e5d31000 	ldrb	r1, [r3]
    f0bc:	e1a0e00f 	mov	lr, pc
    f0c0:	e594f000 	ldr	pc, [r4]
    f0c4:	e59f3068 	ldr	r3, [pc, #104]	; f134 <out_message+0x118>
    f0c8:	e59f0068 	ldr	r0, [pc, #104]	; f138 <out_message+0x11c>
    f0cc:	e5d31000 	ldrb	r1, [r3]
    f0d0:	e1a0e00f 	mov	lr, pc
    f0d4:	e594f000 	ldr	pc, [r4]
    f0d8:	e59f305c 	ldr	r3, [pc, #92]	; f13c <out_message+0x120>
    f0dc:	e59f005c 	ldr	r0, [pc, #92]	; f140 <out_message+0x124>
    f0e0:	e5d31000 	ldrb	r1, [r3]
    f0e4:	e1a0e00f 	mov	lr, pc
    f0e8:	e594f000 	ldr	pc, [r4]
    f0ec:	e8bd8010 	pop	{r4, pc}
    f0f0:	0800d320 	.word	0x0800d320
    f0f4:	0800d76c 	.word	0x0800d76c
    f0f8:	00014398 	.word	0x00014398
    f0fc:	0800d321 	.word	0x0800d321
    f100:	000143a6 	.word	0x000143a6
    f104:	0800d324 	.word	0x0800d324
    f108:	000143b9 	.word	0x000143b9
    f10c:	0800d328 	.word	0x0800d328
    f110:	000143cb 	.word	0x000143cb
    f114:	0800d329 	.word	0x0800d329
    f118:	000143de 	.word	0x000143de
    f11c:	0800d32a 	.word	0x0800d32a
    f120:	000143f5 	.word	0x000143f5
    f124:	0800d1a0 	.word	0x0800d1a0
    f128:	0001440d 	.word	0x0001440d
    f12c:	0800d32b 	.word	0x0800d32b
    f130:	0001441c 	.word	0x0001441c
    f134:	0800d32c 	.word	0x0800d32c
    f138:	00014435 	.word	0x00014435
    f13c:	0800d32d 	.word	0x0800d32d
    f140:	00014447 	.word	0x00014447

0000f144 <Procpacket>:
    f144:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    f148:	e59fc1ac 	ldr	ip, [pc, #428]	; f2fc <Procpacket+0x1b8>
    f14c:	e59f21ac 	ldr	r2, [pc, #428]	; f300 <Procpacket+0x1bc>
    f150:	e5dc3000 	ldrb	r3, [ip]
    f154:	e1a06000 	mov	r6, r0
    f158:	e5c03000 	strb	r3, [r0]
    f15c:	e5923000 	ldr	r3, [r2]
    f160:	e1a05001 	mov	r5, r1
    f164:	e5c03001 	strb	r3, [r0, #1]
    f168:	e5923000 	ldr	r3, [r2]
    f16c:	e1e03003 	mvn	r3, r3
    f170:	e5c03002 	strb	r3, [r0, #2]
    f174:	e59f3188 	ldr	r3, [pc, #392]	; f304 <Procpacket+0x1c0>
    f178:	e5922000 	ldr	r2, [r2]
    f17c:	e5933000 	ldr	r3, [r3]
    f180:	e1520003 	cmp	r2, r3
    f184:	1a000008 	bne	f1ac <Procpacket+0x68>
    f188:	e59f3178 	ldr	r3, [pc, #376]	; f308 <Procpacket+0x1c4>
    f18c:	e5d32000 	ldrb	r2, [r3]
    f190:	e59f3174 	ldr	r3, [pc, #372]	; f30c <Procpacket+0x1c8>
    f194:	e3520001 	cmp	r2, #1	; 0x1
    f198:	e5934000 	ldr	r4, [r3]
    f19c:	e5cc2000 	strb	r2, [ip]
    f1a0:	03a07080 	moveq	r7, #128	; 0x80
    f1a4:	13a07b01 	movne	r7, #1024	; 0x400
    f1a8:	ea000004 	b	f1c0 <Procpacket+0x7c>
    f1ac:	e5dc3000 	ldrb	r3, [ip]
    f1b0:	e3530001 	cmp	r3, #1	; 0x1
    f1b4:	13a04b01 	movne	r4, #1024	; 0x400
    f1b8:	03a04080 	moveq	r4, #128	; 0x80
    f1bc:	e1a07004 	mov	r7, r4
    f1c0:	e59f3148 	ldr	r3, [pc, #328]	; f310 <Procpacket+0x1cc>
    f1c4:	e5d33000 	ldrb	r3, [r3]
    f1c8:	e3530000 	cmp	r3, #0	; 0x0
    f1cc:	1a000008 	bne	f1f4 <Procpacket+0xb0>
    f1d0:	e59f313c 	ldr	r3, [pc, #316]	; f314 <Procpacket+0x1d0>
    f1d4:	e2860003 	add	r0, r6, #3	; 0x3
    f1d8:	e5931000 	ldr	r1, [r3]
    f1dc:	e59f3134 	ldr	r3, [pc, #308]	; f318 <Procpacket+0x1d4>
    f1e0:	e1a02004 	mov	r2, r4
    f1e4:	e5933000 	ldr	r3, [r3]
    f1e8:	e0631001 	rsb	r1, r3, r1
    f1ec:	e0851001 	add	r1, r5, r1
    f1f0:	ebffc855 	bl	134c <memcpy>
    f1f4:	e59f111c 	ldr	r1, [pc, #284]	; f318 <Procpacket+0x1d4>
    f1f8:	e59f311c 	ldr	r3, [pc, #284]	; f31c <Procpacket+0x1d8>
    f1fc:	e5912000 	ldr	r2, [r1]
    f200:	e5d33000 	ldrb	r3, [r3]
    f204:	e0642002 	rsb	r2, r4, r2
    f208:	e3530000 	cmp	r3, #0	; 0x0
    f20c:	e5812000 	str	r2, [r1]
    f210:	0a000006 	beq	f230 <Procpacket+0xec>
    f214:	e1a02007 	mov	r2, r7
    f218:	e2860003 	add	r0, r6, #3	; 0x3
    f21c:	e3a01000 	mov	r1, #0	; 0x0
    f220:	ebffc841 	bl	132c <memset>
    f224:	e59f30f4 	ldr	r3, [pc, #244]	; f320 <Procpacket+0x1dc>
    f228:	e3a02001 	mov	r2, #1	; 0x1
    f22c:	e5c32000 	strb	r2, [r3]
    f230:	e59f30c8 	ldr	r3, [pc, #200]	; f300 <Procpacket+0x1bc>
    f234:	e5932000 	ldr	r2, [r3]
    f238:	e59f30c4 	ldr	r3, [pc, #196]	; f304 <Procpacket+0x1c0>
    f23c:	e5933000 	ldr	r3, [r3]
    f240:	e1520003 	cmp	r2, r3
    f244:	1a000004 	bne	f25c <Procpacket+0x118>
    f248:	e2840003 	add	r0, r4, #3	; 0x3
    f24c:	e0860000 	add	r0, r6, r0
    f250:	e0642007 	rsb	r2, r4, r7
    f254:	e3a0101a 	mov	r1, #26	; 0x1a
    f258:	ebffc833 	bl	132c <memset>
    f25c:	e3a00000 	mov	r0, #0	; 0x0
    f260:	e59f80bc 	ldr	r8, [pc, #188]	; f324 <Procpacket+0x1e0>
    f264:	e1a04006 	mov	r4, r6
    f268:	e1a0c000 	mov	ip, r0
    f26c:	e1a05000 	mov	r5, r0
    f270:	e1a0342c 	lsr	r3, ip, #8
    f274:	e5d42003 	ldrb	r2, [r4, #3]
    f278:	e1a03083 	lsl	r3, r3, #1
    f27c:	e19810b3 	ldrh	r1, [r8, r3]
    f280:	e2800001 	add	r0, r0, #1	; 0x1
    f284:	e182340c 	orr	r3, r2, ip, lsl #8
    f288:	e0233001 	eor	r3, r3, r1
    f28c:	e0852002 	add	r2, r5, r2
    f290:	e1500007 	cmp	r0, r7
    f294:	e6ffc073 	uxth	ip, r3
    f298:	e6ef5072 	uxtb	r5, r2
    f29c:	e2844001 	add	r4, r4, #1	; 0x1
    f2a0:	e59f107c 	ldr	r1, [pc, #124]	; f324 <Procpacket+0x1e0>
    f2a4:	3afffff1 	bcc	f270 <Procpacket+0x12c>
    f2a8:	e59f3078 	ldr	r3, [pc, #120]	; f328 <Procpacket+0x1e4>
    f2ac:	e0860007 	add	r0, r6, r7
    f2b0:	e5d33000 	ldrb	r3, [r3]
    f2b4:	e3530000 	cmp	r3, #0	; 0x0
    f2b8:	05c05003 	strbeq	r5, [r0, #3]
    f2bc:	0a00000c 	beq	f2f4 <Procpacket+0x1b0>
    f2c0:	e1a0342c 	lsr	r3, ip, #8
    f2c4:	e1a03083 	lsl	r3, r3, #1
    f2c8:	e19120b3 	ldrh	r2, [r1, r3]
    f2cc:	e022240c 	eor	r2, r2, ip, lsl #8
    f2d0:	e1a03442 	asr	r3, r2, #8
    f2d4:	e20330ff 	and	r3, r3, #255	; 0xff
    f2d8:	e1a03083 	lsl	r3, r3, #1
    f2dc:	e19130b3 	ldrh	r3, [r1, r3]
    f2e0:	e0233402 	eor	r3, r3, r2, lsl #8
    f2e4:	e6ff3073 	uxth	r3, r3
    f2e8:	e1a02423 	lsr	r2, r3, #8
    f2ec:	e5c03004 	strb	r3, [r0, #4]
    f2f0:	e5c02003 	strb	r2, [r0, #3]
    f2f4:	e3a00000 	mov	r0, #0	; 0x0
    f2f8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    f2fc:	0800d322 	.word	0x0800d322
    f300:	0800d324 	.word	0x0800d324
    f304:	0800dcfc 	.word	0x0800dcfc
    f308:	0800dd04 	.word	0x0800dd04
    f30c:	0800dd00 	.word	0x0800dd00
    f310:	0800d328 	.word	0x0800d328
    f314:	0800d7cc 	.word	0x0800d7cc
    f318:	0800d7d0 	.word	0x0800d7d0
    f31c:	0800d32d 	.word	0x0800d32d
    f320:	0800d32c 	.word	0x0800d32c
    f324:	000115ec 	.word	0x000115ec
    f328:	0800d7c8 	.word	0x0800d7c8

0000f32c <procsend>:
    f32c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    f330:	e59f31f8 	ldr	r3, [pc, #504]	; f530 <procsend+0x204>
    f334:	e1a06000 	mov	r6, r0
    f338:	e5d30000 	ldrb	r0, [r3]
    f33c:	e24dd004 	sub	sp, sp, #4	; 0x4
    f340:	e3500000 	cmp	r0, #0	; 0x0
    f344:	e1a0c002 	mov	ip, r2
    f348:	e59f41e4 	ldr	r4, [pc, #484]	; f534 <procsend+0x208>
    f34c:	0a000065 	beq	f4e8 <procsend+0x1bc>
    f350:	e59f31e0 	ldr	r3, [pc, #480]	; f538 <procsend+0x20c>
    f354:	e1a02b02 	lsl	r2, r2, #22
    f358:	e1a02b22 	lsr	r2, r2, #22
    f35c:	e583c000 	str	ip, [r3]
    f360:	e59f31d4 	ldr	r3, [pc, #468]	; f53c <procsend+0x210>
    f364:	e59f01d4 	ldr	r0, [pc, #468]	; f540 <procsend+0x214>
    f368:	e3520000 	cmp	r2, #0	; 0x0
    f36c:	e1a0152c 	lsr	r1, ip, #10
    f370:	e583c000 	str	ip, [r3]
    f374:	03a03b01 	moveq	r3, #1024	; 0x400
    f378:	e5802000 	str	r2, [r0]
    f37c:	e5841000 	str	r1, [r4]
    f380:	05803000 	streq	r3, [r0]
    f384:	0a000006 	beq	f3a4 <procsend+0x78>
    f388:	e2813001 	add	r3, r1, #1	; 0x1
    f38c:	e352007f 	cmp	r2, #127	; 0x7f
    f390:	e59f21ac 	ldr	r2, [pc, #428]	; f544 <procsend+0x218>
    f394:	e5843000 	str	r3, [r4]
    f398:	93a03001 	movls	r3, #1	; 0x1
    f39c:	83a03002 	movhi	r3, #2	; 0x2
    f3a0:	e5c23000 	strb	r3, [r2]
    f3a4:	e59f019c 	ldr	r0, [pc, #412]	; f548 <procsend+0x21c>
    f3a8:	ebffc7cd 	bl	12e4 <strlen>
    f3ac:	e59f2198 	ldr	r2, [pc, #408]	; f54c <procsend+0x220>
    f3b0:	e3500080 	cmp	r0, #128	; 0x80
    f3b4:	83a03002 	movhi	r3, #2	; 0x2
    f3b8:	93a03001 	movls	r3, #1	; 0x1
    f3bc:	85c23000 	strbhi	r3, [r2]
    f3c0:	95c23000 	strbls	r3, [r2]
    f3c4:	e59f3184 	ldr	r3, [pc, #388]	; f550 <procsend+0x224>
    f3c8:	83a07b01 	movhi	r7, #1024	; 0x400
    f3cc:	e5d31000 	ldrb	r1, [r3]
    f3d0:	93a07080 	movls	r7, #128	; 0x80
    f3d4:	e3510000 	cmp	r1, #0	; 0x0
    f3d8:	13a01000 	movne	r1, #0	; 0x0
    f3dc:	12872005 	addne	r2, r7, #5	; 0x5
    f3e0:	02872004 	addeq	r2, r7, #4	; 0x4
    f3e4:	11a00006 	movne	r0, r6
    f3e8:	01a00006 	moveq	r0, r6
    f3ec:	ebffc7ce 	bl	132c <memset>
    f3f0:	e59f3154 	ldr	r3, [pc, #340]	; f54c <procsend+0x220>
    f3f4:	e3a05000 	mov	r5, #0	; 0x0
    f3f8:	e5d33000 	ldrb	r3, [r3]
    f3fc:	e59f0144 	ldr	r0, [pc, #324]	; f548 <procsend+0x21c>
    f400:	e5c63000 	strb	r3, [r6]
    f404:	e3e03000 	mvn	r3, #0	; 0x0
    f408:	e5c63002 	strb	r3, [r6, #2]
    f40c:	e5c65001 	strb	r5, [r6, #1]
    f410:	ebffc7b3 	bl	12e4 <strlen>
    f414:	e59f112c 	ldr	r1, [pc, #300]	; f548 <procsend+0x21c>
    f418:	e1a02000 	mov	r2, r0
    f41c:	e1a04000 	mov	r4, r0
    f420:	e2860003 	add	r0, r6, #3	; 0x3
    f424:	ebffc75e 	bl	11a4 <strncpy>
    f428:	e59f3108 	ldr	r3, [pc, #264]	; f538 <procsend+0x20c>
    f42c:	e2844004 	add	r4, r4, #4	; 0x4
    f430:	e0860004 	add	r0, r6, r4
    f434:	e5932000 	ldr	r2, [r3]
    f438:	e59f1114 	ldr	r1, [pc, #276]	; f554 <procsend+0x228>
    f43c:	e59f4114 	ldr	r4, [pc, #276]	; f558 <procsend+0x22c>
    f440:	ebfffab8 	bl	df28 <sprintf>
    f444:	e1a00006 	mov	r0, r6
    f448:	e1a0c005 	mov	ip, r5
    f44c:	e1a0e005 	mov	lr, r5
    f450:	e1a0342c 	lsr	r3, ip, #8
    f454:	e5d02003 	ldrb	r2, [r0, #3]
    f458:	e1a03083 	lsl	r3, r3, #1
    f45c:	e19410b3 	ldrh	r1, [r4, r3]
    f460:	e2855001 	add	r5, r5, #1	; 0x1
    f464:	e182340c 	orr	r3, r2, ip, lsl #8
    f468:	e0233001 	eor	r3, r3, r1
    f46c:	e08e2002 	add	r2, lr, r2
    f470:	e1550007 	cmp	r5, r7
    f474:	e6ffc073 	uxth	ip, r3
    f478:	e6efe072 	uxtb	lr, r2
    f47c:	e2800001 	add	r0, r0, #1	; 0x1
    f480:	e59f10d0 	ldr	r1, [pc, #208]	; f558 <procsend+0x22c>
    f484:	3afffff1 	bcc	f450 <procsend+0x124>
    f488:	e59f30c0 	ldr	r3, [pc, #192]	; f550 <procsend+0x224>
    f48c:	e0860007 	add	r0, r6, r7
    f490:	e5d33000 	ldrb	r3, [r3]
    f494:	e3530000 	cmp	r3, #0	; 0x0
    f498:	05c0e003 	strbeq	lr, [r0, #3]
    f49c:	0a00000c 	beq	f4d4 <procsend+0x1a8>
    f4a0:	e1a0342c 	lsr	r3, ip, #8
    f4a4:	e1a03083 	lsl	r3, r3, #1
    f4a8:	e19120b3 	ldrh	r2, [r1, r3]
    f4ac:	e022240c 	eor	r2, r2, ip, lsl #8
    f4b0:	e1a03442 	asr	r3, r2, #8
    f4b4:	e20330ff 	and	r3, r3, #255	; 0xff
    f4b8:	e1a03083 	lsl	r3, r3, #1
    f4bc:	e19130b3 	ldrh	r3, [r1, r3]
    f4c0:	e0233402 	eor	r3, r3, r2, lsl #8
    f4c4:	e6ff3073 	uxth	r3, r3
    f4c8:	e1a02423 	lsr	r2, r3, #8
    f4cc:	e5c03004 	strb	r3, [r0, #4]
    f4d0:	e5c02003 	strb	r2, [r0, #3]
    f4d4:	e59f3080 	ldr	r3, [pc, #128]	; f55c <procsend+0x230>
    f4d8:	e3a02001 	mov	r2, #1	; 0x1
    f4dc:	e5c32000 	strb	r2, [r3]
    f4e0:	e3a00000 	mov	r0, #0	; 0x0
    f4e4:	ea00000f 	b	f528 <procsend+0x1fc>
    f4e8:	e59f2070 	ldr	r2, [pc, #112]	; f560 <procsend+0x234>
    f4ec:	e5943000 	ldr	r3, [r4]
    f4f0:	e5922000 	ldr	r2, [r2]
    f4f4:	e1520003 	cmp	r2, r3
    f4f8:	8a00000a 	bhi	f528 <procsend+0x1fc>
    f4fc:	e59f3060 	ldr	r3, [pc, #96]	; f564 <procsend+0x238>
    f500:	e59f2044 	ldr	r2, [pc, #68]	; f54c <procsend+0x220>
    f504:	e5d33000 	ldrb	r3, [r3]
    f508:	e1a00006 	mov	r0, r6
    f50c:	e3530000 	cmp	r3, #0	; 0x0
    f510:	13a03001 	movne	r3, #1	; 0x1
    f514:	03a03002 	moveq	r3, #2	; 0x2
    f518:	e5c23000 	strb	r3, [r2]
    f51c:	e1a0200c 	mov	r2, ip
    f520:	ebffff07 	bl	f144 <Procpacket>
    f524:	e6ef0070 	uxtb	r0, r0
    f528:	e28dd004 	add	sp, sp, #4	; 0x4
    f52c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    f530:	0800d1a0 	.word	0x0800d1a0
    f534:	0800dcfc 	.word	0x0800dcfc
    f538:	0800d7d0 	.word	0x0800d7d0
    f53c:	0800d7cc 	.word	0x0800d7cc
    f540:	0800dd00 	.word	0x0800dd00
    f544:	0800dd04 	.word	0x0800dd04
    f548:	00014456 	.word	0x00014456
    f54c:	0800d322 	.word	0x0800d322
    f550:	0800d7c8 	.word	0x0800d7c8
    f554:	0001322b 	.word	0x0001322b
    f558:	000115ec 	.word	0x000115ec
    f55c:	0800d321 	.word	0x0800d321
    f560:	0800d324 	.word	0x0800d324
    f564:	0800d32d 	.word	0x0800d32d

0000f568 <CYGACC_COMM_IF_PUTC>:
    f568:	e1a00001 	mov	r0, r1
    f56c:	eafffaf7 	b	e150 <putc>

0000f570 <real_send>:
    f570:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    f574:	e59f30ec 	ldr	r3, [pc, #236]	; f668 <real_send+0xf8>
    f578:	e24ddb01 	sub	sp, sp, #1024	; 0x400
    f57c:	e5d33000 	ldrb	r3, [r3]
    f580:	e24dd00c 	sub	sp, sp, #12	; 0xc
    f584:	e3530000 	cmp	r3, #0	; 0x0
    f588:	e1a0e000 	mov	lr, r0
    f58c:	e1a0c001 	mov	ip, r1
    f590:	159f30d4 	ldrne	r3, [pc, #212]	; f66c <real_send+0xfc>
    f594:	13a02000 	movne	r2, #0	; 0x0
    f598:	15832000 	strne	r2, [r3]
    f59c:	1a000006 	bne	f5bc <real_send+0x4c>
    f5a0:	e59f30c8 	ldr	r3, [pc, #200]	; f670 <real_send+0x100>
    f5a4:	e5d33000 	ldrb	r3, [r3]
    f5a8:	e3530000 	cmp	r3, #0	; 0x0
    f5ac:	059f20b8 	ldreq	r2, [pc, #184]	; f66c <real_send+0xfc>
    f5b0:	05923000 	ldreq	r3, [r2]
    f5b4:	02833001 	addeq	r3, r3, #1	; 0x1
    f5b8:	05823000 	streq	r3, [r2]
    f5bc:	e28d0008 	add	r0, sp, #8	; 0x8
    f5c0:	e2400005 	sub	r0, r0, #5	; 0x5
    f5c4:	e1a0100e 	mov	r1, lr
    f5c8:	e1a0200c 	mov	r2, ip
    f5cc:	ebffff56 	bl	f32c <procsend>
    f5d0:	e2507000 	subs	r7, r0, #0	; 0x0
    f5d4:	1a00001f 	bne	f658 <real_send+0xe8>
    f5d8:	e59f3094 	ldr	r3, [pc, #148]	; f674 <real_send+0x104>
    f5dc:	e28d5008 	add	r5, sp, #8	; 0x8
    f5e0:	e5d31000 	ldrb	r1, [r3]
    f5e4:	e59f308c 	ldr	r3, [pc, #140]	; f678 <real_send+0x108>
    f5e8:	e3510001 	cmp	r1, #1	; 0x1
    f5ec:	e5d32000 	ldrb	r2, [r3]
    f5f0:	e3003404 	movw	r3, #1028	; 0x404
    f5f4:	11a06003 	movne	r6, r3
    f5f8:	03a06084 	moveq	r6, #132	; 0x84
    f5fc:	e3520000 	cmp	r2, #0	; 0x0
    f600:	12866001 	addne	r6, r6, #1	; 0x1
    f604:	e2455005 	sub	r5, r5, #5	; 0x5
    f608:	e3a04000 	mov	r4, #0	; 0x0
    f60c:	e7d50004 	ldrb	r0, [r5, r4]
    f610:	e2844001 	add	r4, r4, #1	; 0x1
    f614:	ebfffacd 	bl	e150 <putc>
    f618:	e1540006 	cmp	r4, r6
    f61c:	3afffffa 	bcc	f60c <real_send+0x9c>
    f620:	e59f3054 	ldr	r3, [pc, #84]	; f67c <real_send+0x10c>
    f624:	e5d33000 	ldrb	r3, [r3]
    f628:	e3530000 	cmp	r3, #0	; 0x0
    f62c:	13e07000 	mvnne	r7, #0	; 0x0
    f630:	1a000008 	bne	f658 <real_send+0xe8>
    f634:	e59f3030 	ldr	r3, [pc, #48]	; f66c <real_send+0xfc>
    f638:	e5932000 	ldr	r2, [r3]
    f63c:	e59f303c 	ldr	r3, [pc, #60]	; f680 <real_send+0x110>
    f640:	e5933000 	ldr	r3, [r3]
    f644:	e1520003 	cmp	r2, r3
    f648:	059f3034 	ldreq	r3, [pc, #52]	; f684 <real_send+0x114>
    f64c:	03a02001 	moveq	r2, #1	; 0x1
    f650:	05c32000 	strbeq	r2, [r3]
    f654:	03a07003 	moveq	r7, #3	; 0x3
    f658:	e1a00007 	mov	r0, r7
    f65c:	e28dd00c 	add	sp, sp, #12	; 0xc
    f660:	e28ddb01 	add	sp, sp, #1024	; 0x400
    f664:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    f668:	0800d32d 	.word	0x0800d32d
    f66c:	0800d324 	.word	0x0800d324
    f670:	0800d1a0 	.word	0x0800d1a0
    f674:	0800d322 	.word	0x0800d322
    f678:	0800d7c8 	.word	0x0800d7c8
    f67c:	0800d32c 	.word	0x0800d32c
    f680:	0800dcfc 	.word	0x0800dcfc
    f684:	0800d328 	.word	0x0800d328

0000f688 <CYGACC_COMM_IF_GETC_TIMEOUT>:
    f688:	e92d4070 	push	{r4, r5, r6, lr}
    f68c:	e1a05001 	mov	r5, r1
    f690:	e3a04000 	mov	r4, #0	; 0x0
    f694:	e59f603c 	ldr	r6, [pc, #60]	; f6d8 <CYGACC_COMM_IF_GETC_TIMEOUT+0x50>
    f698:	ea000000 	b	f6a0 <CYGACC_COMM_IF_GETC_TIMEOUT+0x18>
    f69c:	ebffe46d 	bl	8858 <udelay>
    f6a0:	ebfffaab 	bl	e154 <tstc>
    f6a4:	e3500000 	cmp	r0, #0	; 0x0
    f6a8:	e3a00014 	mov	r0, #20	; 0x14
    f6ac:	1a000002 	bne	f6bc <CYGACC_COMM_IF_GETC_TIMEOUT+0x34>
    f6b0:	e1540006 	cmp	r4, r6
    f6b4:	e2844001 	add	r4, r4, #1	; 0x1
    f6b8:	1afffff7 	bne	f69c <CYGACC_COMM_IF_GETC_TIMEOUT+0x14>
    f6bc:	ebfffaa4 	bl	e154 <tstc>
    f6c0:	e3500000 	cmp	r0, #0	; 0x0
    f6c4:	08bd8070 	popeq	{r4, r5, r6, pc}
    f6c8:	ebfffaa2 	bl	e158 <getc>
    f6cc:	e5c50000 	strb	r0, [r5]
    f6d0:	e3a00001 	mov	r0, #1	; 0x1
    f6d4:	e8bd8070 	pop	{r4, r5, r6, pc}
    f6d8:	000186a0 	.word	0x000186a0

0000f6dc <control_send>:
    f6dc:	e59f323c 	ldr	r3, [pc, #572]	; f920 <control_send+0x244>
    f6e0:	e3a02000 	mov	r2, #0	; 0x0
    f6e4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    f6e8:	e5c32000 	strb	r2, [r3]
    f6ec:	e59f3230 	ldr	r3, [pc, #560]	; f924 <control_send+0x248>
    f6f0:	e59f4230 	ldr	r4, [pc, #560]	; f928 <control_send+0x24c>
    f6f4:	e5c32000 	strb	r2, [r3]
    f6f8:	e59f322c 	ldr	r3, [pc, #556]	; f92c <control_send+0x250>
    f6fc:	e59f522c 	ldr	r5, [pc, #556]	; f930 <control_send+0x254>
    f700:	e5832000 	str	r2, [r3]
    f704:	e3a03001 	mov	r3, #1	; 0x1
    f708:	e59fe224 	ldr	lr, [pc, #548]	; f934 <control_send+0x258>
    f70c:	e59f8224 	ldr	r8, [pc, #548]	; f938 <control_send+0x25c>
    f710:	e59f6224 	ldr	r6, [pc, #548]	; f93c <control_send+0x260>
    f714:	e59f7224 	ldr	r7, [pc, #548]	; f940 <control_send+0x264>
    f718:	e59fc224 	ldr	ip, [pc, #548]	; f944 <control_send+0x268>
    f71c:	e5c43000 	strb	r3, [r4]
    f720:	e59f3220 	ldr	r3, [pc, #544]	; f948 <control_send+0x26c>
    f724:	e24dd014 	sub	sp, sp, #20	; 0x14
    f728:	e5c52000 	strb	r2, [r5]
    f72c:	e5c32000 	strb	r2, [r3]
    f730:	e58d0004 	str	r0, [sp, #4]
    f734:	e1a0b001 	mov	fp, r1
    f738:	e5c82000 	strb	r2, [r8]
    f73c:	e5c62000 	strb	r2, [r6]
    f740:	e5c72000 	strb	r2, [r7]
    f744:	e5ce2000 	strb	r2, [lr]
    f748:	e5cc2000 	strb	r2, [ip]
    f74c:	e1a0a002 	mov	sl, r2
    f750:	e1a0900e 	mov	r9, lr
    f754:	e59f51f0 	ldr	r5, [pc, #496]	; f94c <control_send+0x270>
    f758:	ea000000 	b	f760 <control_send+0x84>
    f75c:	e3a0a000 	mov	sl, #0	; 0x0
    f760:	e3a00000 	mov	r0, #0	; 0x0
    f764:	e28d100f 	add	r1, sp, #15	; 0xf
    f768:	ebffffc6 	bl	f688 <CYGACC_COMM_IF_GETC_TIMEOUT>
    f76c:	e3500000 	cmp	r0, #0	; 0x0
    f770:	0afffffa 	beq	f760 <control_send+0x84>
    f774:	e5dd300f 	ldrb	r3, [sp, #15]
    f778:	e3530018 	cmp	r3, #24	; 0x18
    f77c:	0a000042 	beq	f88c <control_send+0x1b0>
    f780:	8a000004 	bhi	f798 <control_send+0xbc>
    f784:	e3530006 	cmp	r3, #6	; 0x6
    f788:	0a00002c 	beq	f840 <control_send+0x164>
    f78c:	e3530015 	cmp	r3, #21	; 0x15
    f790:	1afffff1 	bne	f75c <control_send+0x80>
    f794:	ea000010 	b	f7dc <control_send+0x100>
    f798:	e3530043 	cmp	r3, #67	; 0x43
    f79c:	0a000001 	beq	f7a8 <control_send+0xcc>
    f7a0:	e3530063 	cmp	r3, #99	; 0x63
    f7a4:	1affffec 	bne	f75c <control_send+0x80>
    f7a8:	e5d73000 	ldrb	r3, [r7]
    f7ac:	e5d42000 	ldrb	r2, [r4]
    f7b0:	e5d61000 	ldrb	r1, [r6]
    f7b4:	e1833002 	orr	r3, r3, r2
    f7b8:	e1913003 	orrs	r3, r1, r3
    f7bc:	0affffe6 	beq	f75c <control_send+0x80>
    f7c0:	e3510000 	cmp	r1, #0	; 0x0
    f7c4:	159f2178 	ldrne	r2, [pc, #376]	; f944 <control_send+0x268>
    f7c8:	13a03001 	movne	r3, #1	; 0x1
    f7cc:	15c23000 	strbne	r3, [r2]
    f7d0:	e3a03001 	mov	r3, #1	; 0x1
    f7d4:	e5c53000 	strb	r3, [r5]
    f7d8:	ea00002f 	b	f89c <control_send+0x1c0>
    f7dc:	e59f313c 	ldr	r3, [pc, #316]	; f920 <control_send+0x244>
    f7e0:	e5d33000 	ldrb	r3, [r3]
    f7e4:	e3530000 	cmp	r3, #0	; 0x0
    f7e8:	0a000008 	beq	f810 <control_send+0x134>
    f7ec:	e59f2138 	ldr	r2, [pc, #312]	; f92c <control_send+0x250>
    f7f0:	e5923000 	ldr	r3, [r2]
    f7f4:	e2433001 	sub	r3, r3, #1	; 0x1
    f7f8:	e5823000 	str	r3, [r2]
    f7fc:	e3a03001 	mov	r3, #1	; 0x1
    f800:	e5c43000 	strb	r3, [r4]
    f804:	e3a03000 	mov	r3, #0	; 0x0
    f808:	e5c93000 	strb	r3, [r9]
    f80c:	ea000022 	b	f89c <control_send+0x1c0>
    f810:	e5d42000 	ldrb	r2, [r4]
    f814:	e3520000 	cmp	r2, #0	; 0x0
    f818:	15c53000 	strbne	r3, [r5]
    f81c:	1a000007 	bne	f840 <control_send+0x164>
    f820:	e5d93000 	ldrb	r3, [r9]
    f824:	e3530000 	cmp	r3, #0	; 0x0
    f828:	15c92000 	strbne	r2, [r9]
    f82c:	e59f20f8 	ldr	r2, [pc, #248]	; f92c <control_send+0x250>
    f830:	e5923000 	ldr	r3, [r2]
    f834:	e2433001 	sub	r3, r3, #1	; 0x1
    f838:	e5823000 	str	r3, [r2]
    f83c:	ea000016 	b	f89c <control_send+0x1c0>
    f840:	e5d42000 	ldrb	r2, [r4]
    f844:	e3520000 	cmp	r2, #0	; 0x0
    f848:	1affffc3 	bne	f75c <control_send+0x80>
    f84c:	e5d91000 	ldrb	r1, [r9]
    f850:	e3510000 	cmp	r1, #0	; 0x0
    f854:	0a000005 	beq	f870 <control_send+0x194>
    f858:	e59f30c0 	ldr	r3, [pc, #192]	; f920 <control_send+0x244>
    f85c:	e1a0a002 	mov	sl, r2
    f860:	e5c32000 	strb	r2, [r3]
    f864:	e3a03001 	mov	r3, #1	; 0x1
    f868:	e5c73000 	strb	r3, [r7]
    f86c:	eaffffbb 	b	f760 <control_send+0x84>
    f870:	e5d83000 	ldrb	r3, [r8]
    f874:	e3530000 	cmp	r3, #0	; 0x0
    f878:	13a03001 	movne	r3, #1	; 0x1
    f87c:	15c63000 	strbne	r3, [r6]
    f880:	11a0a001 	movne	sl, r1
    f884:	1affffb5 	bne	f760 <control_send+0x84>
    f888:	ea000003 	b	f89c <control_send+0x1c0>
    f88c:	e28aa001 	add	sl, sl, #1	; 0x1
    f890:	e35a0002 	cmp	sl, #2	; 0x2
    f894:	1affffb1 	bne	f760 <control_send+0x84>
    f898:	ea00001d 	b	f914 <control_send+0x238>
    f89c:	e59f20a0 	ldr	r2, [pc, #160]	; f944 <control_send+0x268>
    f8a0:	e5d23000 	ldrb	r3, [r2]
    f8a4:	e3530000 	cmp	r3, #0	; 0x0
    f8a8:	1a000008 	bne	f8d0 <control_send+0x1f4>
    f8ac:	e59f207c 	ldr	r2, [pc, #124]	; f930 <control_send+0x254>
    f8b0:	e5d23000 	ldrb	r3, [r2]
    f8b4:	e3530000 	cmp	r3, #0	; 0x0
    f8b8:	0a000004 	beq	f8d0 <control_send+0x1f4>
    f8bc:	e3a00004 	mov	r0, #4	; 0x4
    f8c0:	ebfffa22 	bl	e150 <putc>
    f8c4:	e3a03001 	mov	r3, #1	; 0x1
    f8c8:	e5c83000 	strb	r3, [r8]
    f8cc:	eaffffa2 	b	f75c <control_send+0x80>
    f8d0:	e59d0004 	ldr	r0, [sp, #4]
    f8d4:	e1a0100b 	mov	r1, fp
    f8d8:	ebffff24 	bl	f570 <real_send>
    f8dc:	e3500003 	cmp	r0, #3	; 0x3
    f8e0:	0affff9d 	beq	f75c <control_send+0x80>
    f8e4:	e3500000 	cmp	r0, #0	; 0x0
    f8e8:	1a00000a 	bne	f918 <control_send+0x23c>
    f8ec:	e5d93000 	ldrb	r3, [r9]
    f8f0:	e3530000 	cmp	r3, #0	; 0x0
    f8f4:	e5d43000 	ldrb	r3, [r4]
    f8f8:	15c90000 	strbne	r0, [r9]
    f8fc:	e3530000 	cmp	r3, #0	; 0x0
    f900:	13a03000 	movne	r3, #0	; 0x0
    f904:	15c43000 	strbne	r3, [r4]
    f908:	12833001 	addne	r3, r3, #1	; 0x1
    f90c:	15c93000 	strbne	r3, [r9]
    f910:	eaffff91 	b	f75c <control_send+0x80>
    f914:	e3a00000 	mov	r0, #0	; 0x0
    f918:	e28dd014 	add	sp, sp, #20	; 0x14
    f91c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    f920:	0800d321 	.word	0x0800d321
    f924:	0800d322 	.word	0x0800d322
    f928:	0800d1a0 	.word	0x0800d1a0
    f92c:	0800d324 	.word	0x0800d324
    f930:	0800d328 	.word	0x0800d328
    f934:	0800d32b 	.word	0x0800d32b
    f938:	0800d320 	.word	0x0800d320
    f93c:	0800d329 	.word	0x0800d329
    f940:	0800d32a 	.word	0x0800d32a
    f944:	0800d32d 	.word	0x0800d32d
    f948:	0800d32c 	.word	0x0800d32c
    f94c:	0800d7c8 	.word	0x0800d7c8

0000f950 <xyzModem_stream_terminate>:
    f950:	e92d4030 	push	{r4, r5, lr}
    f954:	e3500000 	cmp	r0, #0	; 0x0
    f958:	e24dd00c 	sub	sp, sp, #12	; 0xc
    f95c:	e1a04001 	mov	r4, r1
    f960:	0a000020 	beq	f9e8 <xyzModem_stream_terminate+0x98>
    f964:	e59f4098 	ldr	r4, [pc, #152]	; fa04 <xyzModem_stream_terminate+0xb4>
    f968:	e5943414 	ldr	r3, [r4, #1044]
    f96c:	e2433001 	sub	r3, r3, #1	; 0x1
    f970:	e3530001 	cmp	r3, #1	; 0x1
    f974:	8a000020 	bhi	f9fc <xyzModem_stream_terminate+0xac>
    f978:	e3a00018 	mov	r0, #24	; 0x18
    f97c:	ebfff9f3 	bl	e150 <putc>
    f980:	e3a00018 	mov	r0, #24	; 0x18
    f984:	ebfff9f1 	bl	e150 <putc>
    f988:	e3a00018 	mov	r0, #24	; 0x18
    f98c:	ebfff9ef 	bl	e150 <putc>
    f990:	e3a00018 	mov	r0, #24	; 0x18
    f994:	ebfff9ed 	bl	e150 <putc>
    f998:	e3a00008 	mov	r0, #8	; 0x8
    f99c:	ebfff9eb 	bl	e150 <putc>
    f9a0:	e3a00008 	mov	r0, #8	; 0x8
    f9a4:	ebfff9e9 	bl	e150 <putc>
    f9a8:	e3a00008 	mov	r0, #8	; 0x8
    f9ac:	ebfff9e7 	bl	e150 <putc>
    f9b0:	e3a00008 	mov	r0, #8	; 0x8
    f9b4:	ebfff9e5 	bl	e150 <putc>
    f9b8:	e1a05004 	mov	r5, r4
    f9bc:	e28d4007 	add	r4, sp, #7	; 0x7
    f9c0:	e5953000 	ldr	r3, [r5]
    f9c4:	e1a01004 	mov	r1, r4
    f9c8:	e5d30000 	ldrb	r0, [r3]
    f9cc:	ebffff2d 	bl	f688 <CYGACC_COMM_IF_GETC_TIMEOUT>
    f9d0:	e3500000 	cmp	r0, #0	; 0x0
    f9d4:	1afffff9 	bne	f9c0 <xyzModem_stream_terminate+0x70>
    f9d8:	e59f3024 	ldr	r3, [pc, #36]	; fa04 <xyzModem_stream_terminate+0xb4>
    f9dc:	e3a02001 	mov	r2, #1	; 0x1
    f9e0:	e583242c 	str	r2, [r3, #1068]
    f9e4:	ea000004 	b	f9fc <xyzModem_stream_terminate+0xac>
    f9e8:	e12fff34 	blx	r4
    f9ec:	e3500000 	cmp	r0, #0	; 0x0
    f9f0:	aafffffc 	bge	f9e8 <xyzModem_stream_terminate+0x98>
    f9f4:	e59f000c 	ldr	r0, [pc, #12]	; fa08 <xyzModem_stream_terminate+0xb8>
    f9f8:	ebffe396 	bl	8858 <udelay>
    f9fc:	e28dd00c 	add	sp, sp, #12	; 0xc
    fa00:	e8bd8030 	pop	{r4, r5, pc}
    fa04:	0800d330 	.word	0x0800d330
    fa08:	0003d090 	.word	0x0003d090

0000fa0c <xyzModem_get_hdr>:
    fa0c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    fa10:	e59f42a0 	ldr	r4, [pc, #672]	; fcb8 <xyzModem_get_hdr+0x2ac>
    fa14:	e24dd00c 	sub	sp, sp, #12	; 0xc
    fa18:	e5943430 	ldr	r3, [r4, #1072]
    fa1c:	e3530000 	cmp	r3, #0	; 0x0
    fa20:	0a000003 	beq	fa34 <xyzModem_get_hdr+0x28>
    fa24:	e3a00006 	mov	r0, #6	; 0x6
    fa28:	ebfff9c8 	bl	e150 <putc>
    fa2c:	e3a03000 	mov	r3, #0	; 0x0
    fa30:	e5843430 	str	r3, [r4, #1072]
    fa34:	e3a04000 	mov	r4, #0	; 0x0
    fa38:	e59f6278 	ldr	r6, [pc, #632]	; fcb8 <xyzModem_get_hdr+0x2ac>
    fa3c:	e1a05004 	mov	r5, r4
    fa40:	e28d7007 	add	r7, sp, #7	; 0x7
    fa44:	e5963000 	ldr	r3, [r6]
    fa48:	e1a01007 	mov	r1, r7
    fa4c:	e5d30000 	ldrb	r0, [r3]
    fa50:	ebffff0c 	bl	f688 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fa54:	e3500000 	cmp	r0, #0	; 0x0
    fa58:	e2855001 	add	r5, r5, #1	; 0x1
    fa5c:	e59f2254 	ldr	r2, [pc, #596]	; fcb8 <xyzModem_get_hdr+0x2ac>
    fa60:	0a000021 	beq	faec <xyzModem_get_hdr+0xe0>
    fa64:	e5dd3007 	ldrb	r3, [sp, #7]
    fa68:	e3530002 	cmp	r3, #2	; 0x2
    fa6c:	0a00000c 	beq	faa4 <xyzModem_get_hdr+0x98>
    fa70:	8a000002 	bhi	fa80 <xyzModem_get_hdr+0x74>
    fa74:	e3530001 	cmp	r3, #1	; 0x1
    fa78:	1afffff1 	bne	fa44 <xyzModem_get_hdr+0x38>
    fa7c:	ea000004 	b	fa94 <xyzModem_get_hdr+0x88>
    fa80:	e3530004 	cmp	r3, #4	; 0x4
    fa84:	0a000012 	beq	fad4 <xyzModem_get_hdr+0xc8>
    fa88:	e3530018 	cmp	r3, #24	; 0x18
    fa8c:	1affffec 	bne	fa44 <xyzModem_get_hdr+0x38>
    fa90:	ea000007 	b	fab4 <xyzModem_get_hdr+0xa8>
    fa94:	e592341c 	ldr	r3, [r2, #1052]
    fa98:	e2833001 	add	r3, r3, #1	; 0x1
    fa9c:	e582341c 	str	r3, [r2, #1052]
    faa0:	ea00007c 	b	fc98 <xyzModem_get_hdr+0x28c>
    faa4:	e5923420 	ldr	r3, [r2, #1056]
    faa8:	e2833001 	add	r3, r3, #1	; 0x1
    faac:	e5823420 	str	r3, [r2, #1056]
    fab0:	ea000078 	b	fc98 <xyzModem_get_hdr+0x28c>
    fab4:	e5963424 	ldr	r3, [r6, #1060]
    fab8:	e2844001 	add	r4, r4, #1	; 0x1
    fabc:	e2833001 	add	r3, r3, #1	; 0x1
    fac0:	e3540003 	cmp	r4, #3	; 0x3
    fac4:	e5863424 	str	r3, [r6, #1060]
    fac8:	1affffdd 	bne	fa44 <xyzModem_get_hdr+0x38>
    facc:	e3e00004 	mvn	r0, #4	; 0x4
    fad0:	ea00006e 	b	fc90 <xyzModem_get_hdr+0x284>
    fad4:	e3550001 	cmp	r5, #1	; 0x1
    fad8:	1affffd9 	bne	fa44 <xyzModem_get_hdr+0x38>
    fadc:	e3a00006 	mov	r0, #6	; 0x6
    fae0:	ebfff99a 	bl	e150 <putc>
    fae4:	e3e00003 	mvn	r0, #3	; 0x3
    fae8:	ea000068 	b	fc90 <xyzModem_get_hdr+0x284>
    faec:	e1a05002 	mov	r5, r2
    faf0:	e28d4006 	add	r4, sp, #6	; 0x6
    faf4:	e5953000 	ldr	r3, [r5]
    faf8:	e1a01004 	mov	r1, r4
    fafc:	e5d30000 	ldrb	r0, [r3]
    fb00:	ebfffee0 	bl	f688 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fb04:	e3500000 	cmp	r0, #0	; 0x0
    fb08:	1afffff9 	bne	faf4 <xyzModem_get_hdr+0xe8>
    fb0c:	e59f01a8 	ldr	r0, [pc, #424]	; fcbc <xyzModem_get_hdr+0x2b0>
    fb10:	ebffe350 	bl	8858 <udelay>
    fb14:	ea00005a 	b	fc84 <xyzModem_get_hdr+0x278>
    fb18:	e5943000 	ldr	r3, [r4]
    fb1c:	e59f119c 	ldr	r1, [pc, #412]	; fcc0 <xyzModem_get_hdr+0x2b4>
    fb20:	e5d30000 	ldrb	r0, [r3]
    fb24:	ebfffed7 	bl	f688 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fb28:	e3500000 	cmp	r0, #0	; 0x0
    fb2c:	0a000054 	beq	fc84 <xyzModem_get_hdr+0x278>
    fb30:	e28d5008 	add	r5, sp, #8	; 0x8
    fb34:	e5753001 	ldrb	r3, [r5, #-1]!
    fb38:	e2842004 	add	r2, r4, #4	; 0x4
    fb3c:	e3530001 	cmp	r3, #1	; 0x1
    fb40:	03a03080 	moveq	r3, #128	; 0x80
    fb44:	13a03b01 	movne	r3, #1024	; 0x400
    fb48:	e5843410 	str	r3, [r4, #1040]
    fb4c:	e5842404 	str	r2, [r4, #1028]
    fb50:	e1a07004 	mov	r7, r4
    fb54:	e3a04000 	mov	r4, #0	; 0x0
    fb58:	ea000008 	b	fb80 <xyzModem_get_hdr+0x174>
    fb5c:	e5973000 	ldr	r3, [r7]
    fb60:	e5d30000 	ldrb	r0, [r3]
    fb64:	ebfffec7 	bl	f688 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fb68:	e3500000 	cmp	r0, #0	; 0x0
    fb6c:	e0872004 	add	r2, r7, r4
    fb70:	e2844001 	add	r4, r4, #1	; 0x1
    fb74:	0a000042 	beq	fc84 <xyzModem_get_hdr+0x278>
    fb78:	e5dd3007 	ldrb	r3, [sp, #7]
    fb7c:	e5c23004 	strb	r3, [r2, #4]
    fb80:	e5973410 	ldr	r3, [r7, #1040]
    fb84:	e1a01005 	mov	r1, r5
    fb88:	e1540003 	cmp	r4, r3
    fb8c:	e59f6124 	ldr	r6, [pc, #292]	; fcb8 <xyzModem_get_hdr+0x2ac>
    fb90:	bafffff1 	blt	fb5c <xyzModem_get_hdr+0x150>
    fb94:	e5963000 	ldr	r3, [r6]
    fb98:	e59f1124 	ldr	r1, [pc, #292]	; fcc4 <xyzModem_get_hdr+0x2b8>
    fb9c:	e5d30000 	ldrb	r0, [r3]
    fba0:	ebfffeb8 	bl	f688 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fba4:	e3500000 	cmp	r0, #0	; 0x0
    fba8:	0a000035 	beq	fc84 <xyzModem_get_hdr+0x278>
    fbac:	e5963428 	ldr	r3, [r6, #1064]
    fbb0:	e3530000 	cmp	r3, #0	; 0x0
    fbb4:	0a000005 	beq	fbd0 <xyzModem_get_hdr+0x1c4>
    fbb8:	e5963000 	ldr	r3, [r6]
    fbbc:	e59f1104 	ldr	r1, [pc, #260]	; fcc8 <xyzModem_get_hdr+0x2bc>
    fbc0:	e5d30000 	ldrb	r0, [r3]
    fbc4:	ebfffeaf 	bl	f688 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fbc8:	e3500000 	cmp	r0, #0	; 0x0
    fbcc:	0a00002c 	beq	fc84 <xyzModem_get_hdr+0x278>
    fbd0:	e59f40e0 	ldr	r4, [pc, #224]	; fcb8 <xyzModem_get_hdr+0x2ac>
    fbd4:	e5d43409 	ldrb	r3, [r4, #1033]
    fbd8:	e5d42408 	ldrb	r2, [r4, #1032]
    fbdc:	e0233002 	eor	r3, r3, r2
    fbe0:	e35300ff 	cmp	r3, #255	; 0xff
    fbe4:	0a000009 	beq	fc10 <xyzModem_get_hdr+0x204>
    fbe8:	e1a05004 	mov	r5, r4
    fbec:	e28d4006 	add	r4, sp, #6	; 0x6
    fbf0:	e5953000 	ldr	r3, [r5]
    fbf4:	e1a01004 	mov	r1, r4
    fbf8:	e5d30000 	ldrb	r0, [r3]
    fbfc:	ebfffea1 	bl	f688 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fc00:	e3500000 	cmp	r0, #0	; 0x0
    fc04:	1afffff9 	bne	fbf0 <xyzModem_get_hdr+0x1e4>
    fc08:	e3e00005 	mvn	r0, #5	; 0x5
    fc0c:	ea00001f 	b	fc90 <xyzModem_get_hdr+0x284>
    fc10:	e5943428 	ldr	r3, [r4, #1064]
    fc14:	e3530000 	cmp	r3, #0	; 0x0
    fc18:	01a02003 	moveq	r2, r3
    fc1c:	01a0c004 	moveq	ip, r4
    fc20:	05940410 	ldreq	r0, [r4, #1040]
    fc24:	01a01003 	moveq	r1, r3
    fc28:	0a00000a 	beq	fc58 <xyzModem_get_hdr+0x24c>
    fc2c:	e2840004 	add	r0, r4, #4	; 0x4
    fc30:	e5941410 	ldr	r1, [r4, #1040]
    fc34:	eb000110 	bl	1007c <cyg_crc16>
    fc38:	e5d4340b 	ldrb	r3, [r4, #1035]
    fc3c:	e5d4240a 	ldrb	r2, [r4, #1034]
    fc40:	e1833402 	orr	r3, r3, r2, lsl #8
    fc44:	e1500003 	cmp	r0, r3
    fc48:	ea00000a 	b	fc78 <xyzModem_get_hdr+0x26c>
    fc4c:	e5d33004 	ldrb	r3, [r3, #4]
    fc50:	e0813003 	add	r3, r1, r3
    fc54:	e6ff1073 	uxth	r1, r3
    fc58:	e1520000 	cmp	r2, r0
    fc5c:	e08c3002 	add	r3, ip, r2
    fc60:	e2822001 	add	r2, r2, #1	; 0x1
    fc64:	bafffff8 	blt	fc4c <xyzModem_get_hdr+0x240>
    fc68:	e59f3048 	ldr	r3, [pc, #72]	; fcb8 <xyzModem_get_hdr+0x2ac>
    fc6c:	e5d3240a 	ldrb	r2, [r3, #1034]
    fc70:	e20130ff 	and	r3, r1, #255	; 0xff
    fc74:	e1520003 	cmp	r2, r3
    fc78:	13e00006 	mvnne	r0, #6	; 0x6
    fc7c:	1a000003 	bne	fc90 <xyzModem_get_hdr+0x284>
    fc80:	ea000001 	b	fc8c <xyzModem_get_hdr+0x280>
    fc84:	e3e00002 	mvn	r0, #2	; 0x2
    fc88:	ea000000 	b	fc90 <xyzModem_get_hdr+0x284>
    fc8c:	e3a00000 	mov	r0, #0	; 0x0
    fc90:	e28dd00c 	add	sp, sp, #12	; 0xc
    fc94:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    fc98:	e59f4018 	ldr	r4, [pc, #24]	; fcb8 <xyzModem_get_hdr+0x2ac>
    fc9c:	e59f1028 	ldr	r1, [pc, #40]	; fccc <xyzModem_get_hdr+0x2c0>
    fca0:	e5943000 	ldr	r3, [r4]
    fca4:	e5d30000 	ldrb	r0, [r3]
    fca8:	ebfffe76 	bl	f688 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fcac:	e3500000 	cmp	r0, #0	; 0x0
    fcb0:	0afffff3 	beq	fc84 <xyzModem_get_hdr+0x278>
    fcb4:	eaffff97 	b	fb18 <xyzModem_get_hdr+0x10c>
    fcb8:	0800d330 	.word	0x0800d330
    fcbc:	0003d090 	.word	0x0003d090
    fcc0:	0800d739 	.word	0x0800d739
    fcc4:	0800d73a 	.word	0x0800d73a
    fcc8:	0800d73b 	.word	0x0800d73b
    fccc:	0800d738 	.word	0x0800d738

0000fcd0 <xyzModem_stream_read>:
    fcd0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    fcd4:	e59f5240 	ldr	r5, [pc, #576]	; ff1c <xyzModem_stream_read+0x24c>
    fcd8:	e24dd004 	sub	sp, sp, #4	; 0x4
    fcdc:	e1a08000 	mov	r8, r0
    fce0:	e1a07001 	mov	r7, r1
    fce4:	e1a0b002 	mov	fp, r2
    fce8:	e3e06004 	mvn	r6, #4	; 0x4
    fcec:	e3a0a000 	mov	sl, #0	; 0x0
    fcf0:	e1a09005 	mov	r9, r5
    fcf4:	ea00007e 	b	fef4 <xyzModem_stream_read+0x224>
    fcf8:	e5953410 	ldr	r3, [r5, #1040]
    fcfc:	e3530000 	cmp	r3, #0	; 0x0
    fd00:	03a04014 	moveq	r4, #20	; 0x14
    fd04:	0a00005d 	beq	fe80 <xyzModem_stream_read+0x1b0>
    fd08:	ea000065 	b	fea4 <xyzModem_stream_read+0x1d4>
    fd0c:	ebffff3e 	bl	fa0c <xyzModem_get_hdr>
    fd10:	e2506000 	subs	r6, r0, #0	; 0x0
    fd14:	1a000039 	bne	fe00 <xyzModem_stream_read+0x130>
    fd18:	e5d5240c 	ldrb	r2, [r5, #1036]
    fd1c:	e5d51408 	ldrb	r1, [r5, #1032]
    fd20:	e2423001 	sub	r3, r2, #1	; 0x1
    fd24:	e1510002 	cmp	r1, r2
    fd28:	e20330ff 	and	r3, r3, #255	; 0xff
    fd2c:	1a00002d 	bne	fde8 <xyzModem_stream_read+0x118>
    fd30:	e599c414 	ldr	ip, [r9, #1044]
    fd34:	e2813001 	add	r3, r1, #1	; 0x1
    fd38:	e5c9340c 	strb	r3, [r9, #1036]
    fd3c:	e35c0001 	cmp	ip, #1	; 0x1
    fd40:	e3a03001 	mov	r3, #1	; 0x1
    fd44:	e5893430 	str	r3, [r9, #1072]
    fd48:	0a000002 	beq	fd58 <xyzModem_stream_read+0x88>
    fd4c:	e5993434 	ldr	r3, [r9, #1076]
    fd50:	e3530000 	cmp	r3, #0	; 0x0
    fd54:	1a000017 	bne	fdb8 <xyzModem_stream_read+0xe8>
    fd58:	e5953410 	ldr	r3, [r5, #1040]
    fd5c:	e5950404 	ldr	r0, [r5, #1028]
    fd60:	e0802003 	add	r2, r0, r3
    fd64:	e5523001 	ldrb	r3, [r2, #-1]
    fd68:	e353001a 	cmp	r3, #26	; 0x1a
    fd6c:	1a00000f 	bne	fdb0 <xyzModem_stream_read+0xe0>
    fd70:	e5523002 	ldrb	r3, [r2, #-2]
    fd74:	e353001a 	cmp	r3, #26	; 0x1a
    fd78:	1a00000c 	bne	fdb0 <xyzModem_stream_read+0xe0>
    fd7c:	e5523003 	ldrb	r3, [r2, #-3]
    fd80:	e353001a 	cmp	r3, #26	; 0x1a
    fd84:	1a000009 	bne	fdb0 <xyzModem_stream_read+0xe0>
    fd88:	ea000000 	b	fd90 <xyzModem_stream_read+0xc0>
    fd8c:	e5851410 	str	r1, [r5, #1040]
    fd90:	e5953410 	ldr	r3, [r5, #1040]
    fd94:	e3530000 	cmp	r3, #0	; 0x0
    fd98:	e0802003 	add	r2, r0, r3
    fd9c:	e2431001 	sub	r1, r3, #1	; 0x1
    fda0:	0a000002 	beq	fdb0 <xyzModem_stream_read+0xe0>
    fda4:	e5523001 	ldrb	r3, [r2, #-1]
    fda8:	e353001a 	cmp	r3, #26	; 0x1a
    fdac:	0afffff6 	beq	fd8c <xyzModem_stream_read+0xbc>
    fdb0:	e35c0001 	cmp	ip, #1	; 0x1
    fdb4:	0a00003a 	beq	fea4 <xyzModem_stream_read+0x1d4>
    fdb8:	e5951434 	ldr	r1, [r5, #1076]
    fdbc:	e3510000 	cmp	r1, #0	; 0x0
    fdc0:	0a000037 	beq	fea4 <xyzModem_stream_read+0x1d4>
    fdc4:	e5953438 	ldr	r3, [r5, #1080]
    fdc8:	e5950410 	ldr	r0, [r5, #1040]
    fdcc:	e0802003 	add	r2, r0, r3
    fdd0:	e1520001 	cmp	r2, r1
    fdd4:	80803001 	addhi	r3, r0, r1
    fdd8:	80623003 	rsbhi	r3, r2, r3
    fddc:	e5852438 	str	r2, [r5, #1080]
    fde0:	85853410 	strhi	r3, [r5, #1040]
    fde4:	ea00002e 	b	fea4 <xyzModem_stream_read+0x1d4>
    fde8:	e1510003 	cmp	r1, r3
    fdec:	13e06007 	mvnne	r6, #7	; 0x7
    fdf0:	1a00001a 	bne	fe60 <xyzModem_stream_read+0x190>
    fdf4:	e3a00006 	mov	r0, #6	; 0x6
    fdf8:	ebfff8d4 	bl	e150 <putc>
    fdfc:	ea00001f 	b	fe80 <xyzModem_stream_read+0x1b0>
    fe00:	e3760005 	cmn	r6, #5	; 0x5
    fe04:	0a000021 	beq	fe90 <xyzModem_stream_read+0x1c0>
    fe08:	e3760004 	cmn	r6, #4	; 0x4
    fe0c:	1a000013 	bne	fe60 <xyzModem_stream_read+0x190>
    fe10:	e3a00006 	mov	r0, #6	; 0x6
    fe14:	ebfff8cd 	bl	e150 <putc>
    fe18:	e5953414 	ldr	r3, [r5, #1044]
    fe1c:	e3530002 	cmp	r3, #2	; 0x2
    fe20:	1a00000b 	bne	fe54 <xyzModem_stream_read+0x184>
    fe24:	e5950428 	ldr	r0, [r5, #1064]
    fe28:	e3500000 	cmp	r0, #0	; 0x0
    fe2c:	13a00043 	movne	r0, #67	; 0x43
    fe30:	03a00015 	moveq	r0, #21	; 0x15
    fe34:	ebfff8c5 	bl	e150 <putc>
    fe38:	e5953418 	ldr	r3, [r5, #1048]
    fe3c:	e2833001 	add	r3, r3, #1	; 0x1
    fe40:	e5853418 	str	r3, [r5, #1048]
    fe44:	ebfffef0 	bl	fa0c <xyzModem_get_hdr>
    fe48:	e1a06000 	mov	r6, r0
    fe4c:	e3a00006 	mov	r0, #6	; 0x6
    fe50:	ebfff8be 	bl	e150 <putc>
    fe54:	e3a03001 	mov	r3, #1	; 0x1
    fe58:	e585342c 	str	r3, [r5, #1068]
    fe5c:	ea000009 	b	fe88 <xyzModem_stream_read+0x1b8>
    fe60:	e5950428 	ldr	r0, [r5, #1064]
    fe64:	e3500000 	cmp	r0, #0	; 0x0
    fe68:	13a00043 	movne	r0, #67	; 0x43
    fe6c:	03a00015 	moveq	r0, #21	; 0x15
    fe70:	ebfff8b6 	bl	e150 <putc>
    fe74:	e5953418 	ldr	r3, [r5, #1048]
    fe78:	e2833001 	add	r3, r3, #1	; 0x1
    fe7c:	e5853418 	str	r3, [r5, #1048]
    fe80:	e2544001 	subs	r4, r4, #1	; 0x1
    fe84:	2affffa0 	bcs	fd0c <xyzModem_stream_read+0x3c>
    fe88:	e3560000 	cmp	r6, #0	; 0x0
    fe8c:	aa000004 	bge	fea4 <xyzModem_stream_read+0x1d4>
    fe90:	e59f3084 	ldr	r3, [pc, #132]	; ff1c <xyzModem_stream_read+0x24c>
    fe94:	e3e02000 	mvn	r2, #0	; 0x0
    fe98:	e58b6000 	str	r6, [fp]
    fe9c:	e5832410 	str	r2, [r3, #1040]
    fea0:	ea00001a 	b	ff10 <xyzModem_stream_read+0x240>
    fea4:	e595342c 	ldr	r3, [r5, #1068]
    fea8:	e3530000 	cmp	r3, #0	; 0x0
    feac:	1a000010 	bne	fef4 <xyzModem_stream_read+0x224>
    feb0:	e5954410 	ldr	r4, [r5, #1040]
    feb4:	e1a00008 	mov	r0, r8
    feb8:	e1570004 	cmp	r7, r4
    febc:	b1a04007 	movlt	r4, r7
    fec0:	a1a04004 	movge	r4, r4
    fec4:	e1a02004 	mov	r2, r4
    fec8:	e5951404 	ldr	r1, [r5, #1028]
    fecc:	ebffc51e 	bl	134c <memcpy>
    fed0:	e5953410 	ldr	r3, [r5, #1040]
    fed4:	e5952404 	ldr	r2, [r5, #1028]
    fed8:	e0643003 	rsb	r3, r4, r3
    fedc:	e0822004 	add	r2, r2, r4
    fee0:	e5853410 	str	r3, [r5, #1040]
    fee4:	e5852404 	str	r2, [r5, #1028]
    fee8:	e0647007 	rsb	r7, r4, r7
    feec:	e0888004 	add	r8, r8, r4
    fef0:	e08aa004 	add	sl, sl, r4
    fef4:	e595342c 	ldr	r3, [r5, #1068]
    fef8:	e2733001 	rsbs	r3, r3, #1	; 0x1
    fefc:	33a03000 	movcc	r3, #0	; 0x0
    ff00:	e3570000 	cmp	r7, #0	; 0x0
    ff04:	d3a03000 	movle	r3, #0	; 0x0
    ff08:	e3530000 	cmp	r3, #0	; 0x0
    ff0c:	1affff79 	bne	fcf8 <xyzModem_stream_read+0x28>
    ff10:	e1a0000a 	mov	r0, sl
    ff14:	e28dd004 	add	sp, sp, #4	; 0x4
    ff18:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ff1c:	0800d330 	.word	0x0800d330

0000ff20 <xyzModem_stream_open>:
    ff20:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    ff24:	e59f4140 	ldr	r4, [pc, #320]	; 1006c <xyzModem_stream_open+0x14c>
    ff28:	e24dd00c 	sub	sp, sp, #12	; 0xc
    ff2c:	e3a05000 	mov	r5, #0	; 0x0
    ff30:	e28d3008 	add	r3, sp, #8	; 0x8
    ff34:	e3a02001 	mov	r2, #1	; 0x1
    ff38:	e5235004 	str	r5, [r3, #-4]!
    ff3c:	e5842428 	str	r2, [r4, #1064]
    ff40:	e5845410 	str	r5, [r4, #1040]
    ff44:	e584542c 	str	r5, [r4, #1068]
    ff48:	e5845430 	str	r5, [r4, #1072]
    ff4c:	e5902004 	ldr	r2, [r0, #4]
    ff50:	e5843000 	str	r3, [r4]
    ff54:	e3a00043 	mov	r0, #67	; 0x43
    ff58:	e5842414 	str	r2, [r4, #1044]
    ff5c:	e5845418 	str	r5, [r4, #1048]
    ff60:	e584541c 	str	r5, [r4, #1052]
    ff64:	e5845420 	str	r5, [r4, #1056]
    ff68:	e5845424 	str	r5, [r4, #1060]
    ff6c:	e5845438 	str	r5, [r4, #1080]
    ff70:	e5845434 	str	r5, [r4, #1076]
    ff74:	e1a08001 	mov	r8, r1
    ff78:	ebfff874 	bl	e150 <putc>
    ff7c:	e5943414 	ldr	r3, [r4, #1044]
    ff80:	e3530001 	cmp	r3, #1	; 0x1
    ff84:	05c4340c 	strbeq	r3, [r4, #1036]
    ff88:	01a00005 	moveq	r0, r5
    ff8c:	0a000034 	beq	10064 <xyzModem_stream_open+0x144>
    ff90:	e1a07004 	mov	r7, r4
    ff94:	e3a06014 	mov	r6, #20	; 0x14
    ff98:	e3a0400a 	mov	r4, #10	; 0xa
    ff9c:	e1a0a005 	mov	sl, r5
    ffa0:	ea00002b 	b	10054 <xyzModem_stream_open+0x134>
    ffa4:	ebfffe98 	bl	fa0c <xyzModem_get_hdr>
    ffa8:	e2505000 	subs	r5, r0, #0	; 0x0
    ffac:	1a000016 	bne	1000c <xyzModem_stream_open+0xec>
    ffb0:	e59f20b4 	ldr	r2, [pc, #180]	; 1006c <xyzModem_stream_open+0x14c>
    ffb4:	e5d23408 	ldrb	r3, [r2, #1032]
    ffb8:	e3530000 	cmp	r3, #0	; 0x0
    ffbc:	1a00000b 	bne	fff0 <xyzModem_stream_open+0xd0>
    ffc0:	e1a03002 	mov	r3, r2
    ffc4:	e5930404 	ldr	r0, [r3, #1028]
    ffc8:	e4d02001 	ldrb	r2, [r0], #1
    ffcc:	e3520000 	cmp	r2, #0	; 0x0
    ffd0:	e5830404 	str	r0, [r3, #1028]
    ffd4:	1afffffa 	bne	ffc4 <xyzModem_stream_open+0xa4>
    ffd8:	e59f3090 	ldr	r3, [pc, #144]	; 10070 <xyzModem_stream_open+0x150>
    ffdc:	e59f1090 	ldr	r1, [pc, #144]	; 10074 <xyzModem_stream_open+0x154>
    ffe0:	ebfffb5a 	bl	ed50 <parse_num>
    ffe4:	e59f3080 	ldr	r3, [pc, #128]	; 1006c <xyzModem_stream_open+0x14c>
    ffe8:	e3a02001 	mov	r2, #1	; 0x1
    ffec:	e5832430 	str	r2, [r3, #1072]
    fff0:	e59f3074 	ldr	r3, [pc, #116]	; 1006c <xyzModem_stream_open+0x14c>
    fff4:	e3a01000 	mov	r1, #0	; 0x0
    fff8:	e3a02001 	mov	r2, #1	; 0x1
    fffc:	e5c3240c 	strb	r2, [r3, #1036]
   10000:	e5831410 	str	r1, [r3, #1040]
   10004:	e1a00001 	mov	r0, r1
   10008:	ea000015 	b	10064 <xyzModem_stream_open+0x144>
   1000c:	e3750003 	cmn	r5, #3	; 0x3
   10010:	1a00000d 	bne	1004c <xyzModem_stream_open+0x12c>
   10014:	e2444001 	sub	r4, r4, #1	; 0x1
   10018:	e3540000 	cmp	r4, #0	; 0x0
   1001c:	e59f0054 	ldr	r0, [pc, #84]	; 10078 <xyzModem_stream_open+0x158>
   10020:	d587a428 	strle	sl, [r7, #1064]
   10024:	ebffe20b 	bl	8858 <udelay>
   10028:	e5970428 	ldr	r0, [r7, #1064]
   1002c:	e3500000 	cmp	r0, #0	; 0x0
   10030:	13a00043 	movne	r0, #67	; 0x43
   10034:	03a00015 	moveq	r0, #21	; 0x15
   10038:	ebfff844 	bl	e150 <putc>
   1003c:	e5973418 	ldr	r3, [r7, #1048]
   10040:	e2833001 	add	r3, r3, #1	; 0x1
   10044:	e5873418 	str	r3, [r7, #1048]
   10048:	ea000001 	b	10054 <xyzModem_stream_open+0x134>
   1004c:	e3750005 	cmn	r5, #5	; 0x5
   10050:	0a000001 	beq	1005c <xyzModem_stream_open+0x13c>
   10054:	e2566001 	subs	r6, r6, #1	; 0x1
   10058:	2affffd1 	bcs	ffa4 <xyzModem_stream_open+0x84>
   1005c:	e5885000 	str	r5, [r8]
   10060:	e3e00000 	mvn	r0, #0	; 0x0
   10064:	e28dd00c 	add	sp, sp, #12	; 0xc
   10068:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
   1006c:	0800d330 	.word	0x0800d330
   10070:	000141c2 	.word	0x000141c2
   10074:	0800d764 	.word	0x0800d764
   10078:	0007a120 	.word	0x0007a120

0001007c <cyg_crc16>:
   1007c:	e3a02000 	mov	r2, #0	; 0x0
   10080:	e92d4010 	push	{r4, lr}
   10084:	e1a0c000 	mov	ip, r0
   10088:	e59f402c 	ldr	r4, [pc, #44]	; 100bc <cyg_crc16+0x40>
   1008c:	e1a00002 	mov	r0, r2
   10090:	ea000006 	b	100b0 <cyg_crc16+0x34>
   10094:	e7dc3002 	ldrb	r3, [ip, r2]
   10098:	e2822001 	add	r2, r2, #1	; 0x1
   1009c:	e0233420 	eor	r3, r3, r0, lsr #8
   100a0:	e1a03083 	lsl	r3, r3, #1
   100a4:	e19430b3 	ldrh	r3, [r4, r3]
   100a8:	e0233400 	eor	r3, r3, r0, lsl #8
   100ac:	e6ff0073 	uxth	r0, r3
   100b0:	e1520001 	cmp	r2, r1
   100b4:	bafffff6 	blt	10094 <cyg_crc16+0x18>
   100b8:	e8bd8010 	pop	{r4, pc}
   100bc:	000115ec 	.word	0x000115ec

000100c0 <ss_jtag_en>:
   100c0:	e59f300c 	ldr	r3, [pc, #12]	; 100d4 <ss_jtag_en+0x14>
   100c4:	e2500000 	subs	r0, r0, #0	; 0x0
   100c8:	13a00001 	movne	r0, #1	; 0x1
   100cc:	e58308b8 	str	r0, [r3, #2232]
   100d0:	e12fff1e 	bx	lr
   100d4:	2d008000 	.word	0x2d008000

000100d8 <raise>:
   100d8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   100dc:	e1a01000 	mov	r1, r0
   100e0:	e24dd004 	sub	sp, sp, #4	; 0x4
   100e4:	e59f0014 	ldr	r0, [pc, #20]	; 10100 <raise+0x28>
   100e8:	e59f3014 	ldr	r3, [pc, #20]	; 10104 <raise+0x2c>
   100ec:	e1a0e00f 	mov	lr, pc
   100f0:	e593f000 	ldr	pc, [r3]
   100f4:	e3a00000 	mov	r0, #0	; 0x0
   100f8:	e28dd004 	add	sp, sp, #4	; 0x4
   100fc:	e8bd8000 	pop	{pc}
   10100:	0001445e 	.word	0x0001445e
   10104:	0800d76c 	.word	0x0800d76c

00010108 <__aeabi_uidiv>:
   10108:	e2512001 	subs	r2, r1, #1	; 0x1
   1010c:	012fff1e 	bxeq	lr
   10110:	3a000074 	bcc	102e8 <__aeabi_uidiv+0x1e0>
   10114:	e1500001 	cmp	r0, r1
   10118:	9a00006b 	bls	102cc <__aeabi_uidiv+0x1c4>
   1011c:	e1110002 	tst	r1, r2
   10120:	0a00006c 	beq	102d8 <__aeabi_uidiv+0x1d0>
   10124:	e16f3f10 	clz	r3, r0
   10128:	e16f2f11 	clz	r2, r1
   1012c:	e0423003 	sub	r3, r2, r3
   10130:	e273301f 	rsbs	r3, r3, #31	; 0x1f
   10134:	10833083 	addne	r3, r3, r3, lsl #1
   10138:	e3a02000 	mov	r2, #0	; 0x0
   1013c:	108ff103 	addne	pc, pc, r3, lsl #2
   10140:	e1a00000 	nop			(mov r0,r0)
   10144:	e1500f81 	cmp	r0, r1, lsl #31
   10148:	e0a22002 	adc	r2, r2, r2
   1014c:	20400f81 	subcs	r0, r0, r1, lsl #31
   10150:	e1500f01 	cmp	r0, r1, lsl #30
   10154:	e0a22002 	adc	r2, r2, r2
   10158:	20400f01 	subcs	r0, r0, r1, lsl #30
   1015c:	e1500e81 	cmp	r0, r1, lsl #29
   10160:	e0a22002 	adc	r2, r2, r2
   10164:	20400e81 	subcs	r0, r0, r1, lsl #29
   10168:	e1500e01 	cmp	r0, r1, lsl #28
   1016c:	e0a22002 	adc	r2, r2, r2
   10170:	20400e01 	subcs	r0, r0, r1, lsl #28
   10174:	e1500d81 	cmp	r0, r1, lsl #27
   10178:	e0a22002 	adc	r2, r2, r2
   1017c:	20400d81 	subcs	r0, r0, r1, lsl #27
   10180:	e1500d01 	cmp	r0, r1, lsl #26
   10184:	e0a22002 	adc	r2, r2, r2
   10188:	20400d01 	subcs	r0, r0, r1, lsl #26
   1018c:	e1500c81 	cmp	r0, r1, lsl #25
   10190:	e0a22002 	adc	r2, r2, r2
   10194:	20400c81 	subcs	r0, r0, r1, lsl #25
   10198:	e1500c01 	cmp	r0, r1, lsl #24
   1019c:	e0a22002 	adc	r2, r2, r2
   101a0:	20400c01 	subcs	r0, r0, r1, lsl #24
   101a4:	e1500b81 	cmp	r0, r1, lsl #23
   101a8:	e0a22002 	adc	r2, r2, r2
   101ac:	20400b81 	subcs	r0, r0, r1, lsl #23
   101b0:	e1500b01 	cmp	r0, r1, lsl #22
   101b4:	e0a22002 	adc	r2, r2, r2
   101b8:	20400b01 	subcs	r0, r0, r1, lsl #22
   101bc:	e1500a81 	cmp	r0, r1, lsl #21
   101c0:	e0a22002 	adc	r2, r2, r2
   101c4:	20400a81 	subcs	r0, r0, r1, lsl #21
   101c8:	e1500a01 	cmp	r0, r1, lsl #20
   101cc:	e0a22002 	adc	r2, r2, r2
   101d0:	20400a01 	subcs	r0, r0, r1, lsl #20
   101d4:	e1500981 	cmp	r0, r1, lsl #19
   101d8:	e0a22002 	adc	r2, r2, r2
   101dc:	20400981 	subcs	r0, r0, r1, lsl #19
   101e0:	e1500901 	cmp	r0, r1, lsl #18
   101e4:	e0a22002 	adc	r2, r2, r2
   101e8:	20400901 	subcs	r0, r0, r1, lsl #18
   101ec:	e1500881 	cmp	r0, r1, lsl #17
   101f0:	e0a22002 	adc	r2, r2, r2
   101f4:	20400881 	subcs	r0, r0, r1, lsl #17
   101f8:	e1500801 	cmp	r0, r1, lsl #16
   101fc:	e0a22002 	adc	r2, r2, r2
   10200:	20400801 	subcs	r0, r0, r1, lsl #16
   10204:	e1500781 	cmp	r0, r1, lsl #15
   10208:	e0a22002 	adc	r2, r2, r2
   1020c:	20400781 	subcs	r0, r0, r1, lsl #15
   10210:	e1500701 	cmp	r0, r1, lsl #14
   10214:	e0a22002 	adc	r2, r2, r2
   10218:	20400701 	subcs	r0, r0, r1, lsl #14
   1021c:	e1500681 	cmp	r0, r1, lsl #13
   10220:	e0a22002 	adc	r2, r2, r2
   10224:	20400681 	subcs	r0, r0, r1, lsl #13
   10228:	e1500601 	cmp	r0, r1, lsl #12
   1022c:	e0a22002 	adc	r2, r2, r2
   10230:	20400601 	subcs	r0, r0, r1, lsl #12
   10234:	e1500581 	cmp	r0, r1, lsl #11
   10238:	e0a22002 	adc	r2, r2, r2
   1023c:	20400581 	subcs	r0, r0, r1, lsl #11
   10240:	e1500501 	cmp	r0, r1, lsl #10
   10244:	e0a22002 	adc	r2, r2, r2
   10248:	20400501 	subcs	r0, r0, r1, lsl #10
   1024c:	e1500481 	cmp	r0, r1, lsl #9
   10250:	e0a22002 	adc	r2, r2, r2
   10254:	20400481 	subcs	r0, r0, r1, lsl #9
   10258:	e1500401 	cmp	r0, r1, lsl #8
   1025c:	e0a22002 	adc	r2, r2, r2
   10260:	20400401 	subcs	r0, r0, r1, lsl #8
   10264:	e1500381 	cmp	r0, r1, lsl #7
   10268:	e0a22002 	adc	r2, r2, r2
   1026c:	20400381 	subcs	r0, r0, r1, lsl #7
   10270:	e1500301 	cmp	r0, r1, lsl #6
   10274:	e0a22002 	adc	r2, r2, r2
   10278:	20400301 	subcs	r0, r0, r1, lsl #6
   1027c:	e1500281 	cmp	r0, r1, lsl #5
   10280:	e0a22002 	adc	r2, r2, r2
   10284:	20400281 	subcs	r0, r0, r1, lsl #5
   10288:	e1500201 	cmp	r0, r1, lsl #4
   1028c:	e0a22002 	adc	r2, r2, r2
   10290:	20400201 	subcs	r0, r0, r1, lsl #4
   10294:	e1500181 	cmp	r0, r1, lsl #3
   10298:	e0a22002 	adc	r2, r2, r2
   1029c:	20400181 	subcs	r0, r0, r1, lsl #3
   102a0:	e1500101 	cmp	r0, r1, lsl #2
   102a4:	e0a22002 	adc	r2, r2, r2
   102a8:	20400101 	subcs	r0, r0, r1, lsl #2
   102ac:	e1500081 	cmp	r0, r1, lsl #1
   102b0:	e0a22002 	adc	r2, r2, r2
   102b4:	20400081 	subcs	r0, r0, r1, lsl #1
   102b8:	e1500001 	cmp	r0, r1
   102bc:	e0a22002 	adc	r2, r2, r2
   102c0:	20400001 	subcs	r0, r0, r1
   102c4:	e1a00002 	mov	r0, r2
   102c8:	e12fff1e 	bx	lr
   102cc:	03a00001 	moveq	r0, #1	; 0x1
   102d0:	13a00000 	movne	r0, #0	; 0x0
   102d4:	e12fff1e 	bx	lr
   102d8:	e16f2f11 	clz	r2, r1
   102dc:	e262201f 	rsb	r2, r2, #31	; 0x1f
   102e0:	e1a00230 	lsr	r0, r0, r2
   102e4:	e12fff1e 	bx	lr
   102e8:	e52de008 	str	lr, [sp, #-8]!
   102ec:	eb0002c4 	bl	10e04 <__div0>
   102f0:	e3a00000 	mov	r0, #0	; 0x0
   102f4:	e49df008 	ldr	pc, [sp], #8

000102f8 <__aeabi_uidivmod>:
   102f8:	e92d4003 	push	{r0, r1, lr}
   102fc:	ebffff81 	bl	10108 <__aeabi_uidiv>
   10300:	e8bd4006 	pop	{r1, r2, lr}
   10304:	e0030092 	mul	r3, r2, r0
   10308:	e0411003 	sub	r1, r1, r3
   1030c:	e12fff1e 	bx	lr

00010310 <__aeabi_idiv>:
   10310:	e3510000 	cmp	r1, #0	; 0x0
   10314:	e020c001 	eor	ip, r0, r1
   10318:	0a000080 	beq	10520 <__aeabi_idiv+0x210>
   1031c:	42611000 	rsbmi	r1, r1, #0	; 0x0
   10320:	e2512001 	subs	r2, r1, #1	; 0x1
   10324:	0a000070 	beq	104ec <__aeabi_idiv+0x1dc>
   10328:	e1b03000 	movs	r3, r0
   1032c:	42603000 	rsbmi	r3, r0, #0	; 0x0
   10330:	e1530001 	cmp	r3, r1
   10334:	9a00006f 	bls	104f8 <__aeabi_idiv+0x1e8>
   10338:	e1110002 	tst	r1, r2
   1033c:	0a000071 	beq	10508 <__aeabi_idiv+0x1f8>
   10340:	e16f2f13 	clz	r2, r3
   10344:	e16f0f11 	clz	r0, r1
   10348:	e0402002 	sub	r2, r0, r2
   1034c:	e272201f 	rsbs	r2, r2, #31	; 0x1f
   10350:	10822082 	addne	r2, r2, r2, lsl #1
   10354:	e3a00000 	mov	r0, #0	; 0x0
   10358:	108ff102 	addne	pc, pc, r2, lsl #2
   1035c:	e1a00000 	nop			(mov r0,r0)
   10360:	e1530f81 	cmp	r3, r1, lsl #31
   10364:	e0a00000 	adc	r0, r0, r0
   10368:	20433f81 	subcs	r3, r3, r1, lsl #31
   1036c:	e1530f01 	cmp	r3, r1, lsl #30
   10370:	e0a00000 	adc	r0, r0, r0
   10374:	20433f01 	subcs	r3, r3, r1, lsl #30
   10378:	e1530e81 	cmp	r3, r1, lsl #29
   1037c:	e0a00000 	adc	r0, r0, r0
   10380:	20433e81 	subcs	r3, r3, r1, lsl #29
   10384:	e1530e01 	cmp	r3, r1, lsl #28
   10388:	e0a00000 	adc	r0, r0, r0
   1038c:	20433e01 	subcs	r3, r3, r1, lsl #28
   10390:	e1530d81 	cmp	r3, r1, lsl #27
   10394:	e0a00000 	adc	r0, r0, r0
   10398:	20433d81 	subcs	r3, r3, r1, lsl #27
   1039c:	e1530d01 	cmp	r3, r1, lsl #26
   103a0:	e0a00000 	adc	r0, r0, r0
   103a4:	20433d01 	subcs	r3, r3, r1, lsl #26
   103a8:	e1530c81 	cmp	r3, r1, lsl #25
   103ac:	e0a00000 	adc	r0, r0, r0
   103b0:	20433c81 	subcs	r3, r3, r1, lsl #25
   103b4:	e1530c01 	cmp	r3, r1, lsl #24
   103b8:	e0a00000 	adc	r0, r0, r0
   103bc:	20433c01 	subcs	r3, r3, r1, lsl #24
   103c0:	e1530b81 	cmp	r3, r1, lsl #23
   103c4:	e0a00000 	adc	r0, r0, r0
   103c8:	20433b81 	subcs	r3, r3, r1, lsl #23
   103cc:	e1530b01 	cmp	r3, r1, lsl #22
   103d0:	e0a00000 	adc	r0, r0, r0
   103d4:	20433b01 	subcs	r3, r3, r1, lsl #22
   103d8:	e1530a81 	cmp	r3, r1, lsl #21
   103dc:	e0a00000 	adc	r0, r0, r0
   103e0:	20433a81 	subcs	r3, r3, r1, lsl #21
   103e4:	e1530a01 	cmp	r3, r1, lsl #20
   103e8:	e0a00000 	adc	r0, r0, r0
   103ec:	20433a01 	subcs	r3, r3, r1, lsl #20
   103f0:	e1530981 	cmp	r3, r1, lsl #19
   103f4:	e0a00000 	adc	r0, r0, r0
   103f8:	20433981 	subcs	r3, r3, r1, lsl #19
   103fc:	e1530901 	cmp	r3, r1, lsl #18
   10400:	e0a00000 	adc	r0, r0, r0
   10404:	20433901 	subcs	r3, r3, r1, lsl #18
   10408:	e1530881 	cmp	r3, r1, lsl #17
   1040c:	e0a00000 	adc	r0, r0, r0
   10410:	20433881 	subcs	r3, r3, r1, lsl #17
   10414:	e1530801 	cmp	r3, r1, lsl #16
   10418:	e0a00000 	adc	r0, r0, r0
   1041c:	20433801 	subcs	r3, r3, r1, lsl #16
   10420:	e1530781 	cmp	r3, r1, lsl #15
   10424:	e0a00000 	adc	r0, r0, r0
   10428:	20433781 	subcs	r3, r3, r1, lsl #15
   1042c:	e1530701 	cmp	r3, r1, lsl #14
   10430:	e0a00000 	adc	r0, r0, r0
   10434:	20433701 	subcs	r3, r3, r1, lsl #14
   10438:	e1530681 	cmp	r3, r1, lsl #13
   1043c:	e0a00000 	adc	r0, r0, r0
   10440:	20433681 	subcs	r3, r3, r1, lsl #13
   10444:	e1530601 	cmp	r3, r1, lsl #12
   10448:	e0a00000 	adc	r0, r0, r0
   1044c:	20433601 	subcs	r3, r3, r1, lsl #12
   10450:	e1530581 	cmp	r3, r1, lsl #11
   10454:	e0a00000 	adc	r0, r0, r0
   10458:	20433581 	subcs	r3, r3, r1, lsl #11
   1045c:	e1530501 	cmp	r3, r1, lsl #10
   10460:	e0a00000 	adc	r0, r0, r0
   10464:	20433501 	subcs	r3, r3, r1, lsl #10
   10468:	e1530481 	cmp	r3, r1, lsl #9
   1046c:	e0a00000 	adc	r0, r0, r0
   10470:	20433481 	subcs	r3, r3, r1, lsl #9
   10474:	e1530401 	cmp	r3, r1, lsl #8
   10478:	e0a00000 	adc	r0, r0, r0
   1047c:	20433401 	subcs	r3, r3, r1, lsl #8
   10480:	e1530381 	cmp	r3, r1, lsl #7
   10484:	e0a00000 	adc	r0, r0, r0
   10488:	20433381 	subcs	r3, r3, r1, lsl #7
   1048c:	e1530301 	cmp	r3, r1, lsl #6
   10490:	e0a00000 	adc	r0, r0, r0
   10494:	20433301 	subcs	r3, r3, r1, lsl #6
   10498:	e1530281 	cmp	r3, r1, lsl #5
   1049c:	e0a00000 	adc	r0, r0, r0
   104a0:	20433281 	subcs	r3, r3, r1, lsl #5
   104a4:	e1530201 	cmp	r3, r1, lsl #4
   104a8:	e0a00000 	adc	r0, r0, r0
   104ac:	20433201 	subcs	r3, r3, r1, lsl #4
   104b0:	e1530181 	cmp	r3, r1, lsl #3
   104b4:	e0a00000 	adc	r0, r0, r0
   104b8:	20433181 	subcs	r3, r3, r1, lsl #3
   104bc:	e1530101 	cmp	r3, r1, lsl #2
   104c0:	e0a00000 	adc	r0, r0, r0
   104c4:	20433101 	subcs	r3, r3, r1, lsl #2
   104c8:	e1530081 	cmp	r3, r1, lsl #1
   104cc:	e0a00000 	adc	r0, r0, r0
   104d0:	20433081 	subcs	r3, r3, r1, lsl #1
   104d4:	e1530001 	cmp	r3, r1
   104d8:	e0a00000 	adc	r0, r0, r0
   104dc:	20433001 	subcs	r3, r3, r1
   104e0:	e35c0000 	cmp	ip, #0	; 0x0
   104e4:	42600000 	rsbmi	r0, r0, #0	; 0x0
   104e8:	e12fff1e 	bx	lr
   104ec:	e13c0000 	teq	ip, r0
   104f0:	42600000 	rsbmi	r0, r0, #0	; 0x0
   104f4:	e12fff1e 	bx	lr
   104f8:	33a00000 	movcc	r0, #0	; 0x0
   104fc:	01a00fcc 	asreq	r0, ip, #31
   10500:	03800001 	orreq	r0, r0, #1	; 0x1
   10504:	e12fff1e 	bx	lr
   10508:	e16f2f11 	clz	r2, r1
   1050c:	e262201f 	rsb	r2, r2, #31	; 0x1f
   10510:	e35c0000 	cmp	ip, #0	; 0x0
   10514:	e1a00233 	lsr	r0, r3, r2
   10518:	42600000 	rsbmi	r0, r0, #0	; 0x0
   1051c:	e12fff1e 	bx	lr
   10520:	e52de008 	str	lr, [sp, #-8]!
   10524:	eb000236 	bl	10e04 <__div0>
   10528:	e3a00000 	mov	r0, #0	; 0x0
   1052c:	e49df008 	ldr	pc, [sp], #8

00010530 <__aeabi_idivmod>:
   10530:	e92d4003 	push	{r0, r1, lr}
   10534:	ebffff75 	bl	10310 <__aeabi_idiv>
   10538:	e8bd4006 	pop	{r1, r2, lr}
   1053c:	e0030092 	mul	r3, r2, r0
   10540:	e0411003 	sub	r1, r1, r3
   10544:	e12fff1e 	bx	lr

00010548 <__aeabi_llsr>:
   10548:	e2523020 	subs	r3, r2, #32	; 0x20
   1054c:	e262c020 	rsb	ip, r2, #32	; 0x20
   10550:	41a00230 	lsrmi	r0, r0, r2
   10554:	51a00331 	lsrpl	r0, r1, r3
   10558:	41800c11 	orrmi	r0, r0, r1, lsl ip
   1055c:	e1a01231 	lsr	r1, r1, r2
   10560:	e12fff1e 	bx	lr

00010564 <__aeabi_llsl>:
   10564:	e2523020 	subs	r3, r2, #32	; 0x20
   10568:	e262c020 	rsb	ip, r2, #32	; 0x20
   1056c:	41a01211 	lslmi	r1, r1, r2
   10570:	51a01310 	lslpl	r1, r0, r3
   10574:	41811c30 	orrmi	r1, r1, r0, lsr ip
   10578:	e1a00210 	lsl	r0, r0, r2
   1057c:	e12fff1e 	bx	lr

00010580 <__aeabi_drsub>:
   10580:	e2211102 	eor	r1, r1, #-2147483648	; 0x80000000
   10584:	ea000000 	b	1058c <__adddf3>

00010588 <__aeabi_dsub>:
   10588:	e2233102 	eor	r3, r3, #-2147483648	; 0x80000000

0001058c <__adddf3>:
   1058c:	e92d4030 	push	{r4, r5, lr}
   10590:	e1a04081 	lsl	r4, r1, #1
   10594:	e1a05083 	lsl	r5, r3, #1
   10598:	e1340005 	teq	r4, r5
   1059c:	01300002 	teqeq	r0, r2
   105a0:	1194c000 	orrsne	ip, r4, r0
   105a4:	1195c002 	orrsne	ip, r5, r2
   105a8:	11f0cac4 	mvnsne	ip, r4, asr #21
   105ac:	11f0cac5 	mvnsne	ip, r5, asr #21
   105b0:	0a000079 	beq	1079c <__adddf3+0x210>
   105b4:	e1a04aa4 	lsr	r4, r4, #21
   105b8:	e0745aa5 	rsbs	r5, r4, r5, lsr #21
   105bc:	b2655000 	rsblt	r5, r5, #0	; 0x0
   105c0:	da000006 	ble	105e0 <__adddf3+0x54>
   105c4:	e0844005 	add	r4, r4, r5
   105c8:	e0202002 	eor	r2, r0, r2
   105cc:	e0213003 	eor	r3, r1, r3
   105d0:	e0220000 	eor	r0, r2, r0
   105d4:	e0231001 	eor	r1, r3, r1
   105d8:	e0202002 	eor	r2, r0, r2
   105dc:	e0213003 	eor	r3, r1, r3
   105e0:	e3550036 	cmp	r5, #54	; 0x36
   105e4:	88bd8030 	pophi	{r4, r5, pc}
   105e8:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   105ec:	e1a01601 	lsl	r1, r1, #12
   105f0:	e3a0c601 	mov	ip, #1048576	; 0x100000
   105f4:	e18c1621 	orr	r1, ip, r1, lsr #12
   105f8:	0a000001 	beq	10604 <__adddf3+0x78>
   105fc:	e2700000 	rsbs	r0, r0, #0	; 0x0
   10600:	e2e11000 	rsc	r1, r1, #0	; 0x0
   10604:	e3130102 	tst	r3, #-2147483648	; 0x80000000
   10608:	e1a03603 	lsl	r3, r3, #12
   1060c:	e18c3623 	orr	r3, ip, r3, lsr #12
   10610:	0a000001 	beq	1061c <__adddf3+0x90>
   10614:	e2722000 	rsbs	r2, r2, #0	; 0x0
   10618:	e2e33000 	rsc	r3, r3, #0	; 0x0
   1061c:	e1340005 	teq	r4, r5
   10620:	0a000057 	beq	10784 <__adddf3+0x1f8>
   10624:	e2444001 	sub	r4, r4, #1	; 0x1
   10628:	e275e020 	rsbs	lr, r5, #32	; 0x20
   1062c:	ba000005 	blt	10648 <__adddf3+0xbc>
   10630:	e1a0ce12 	lsl	ip, r2, lr
   10634:	e0900532 	adds	r0, r0, r2, lsr r5
   10638:	e2a11000 	adc	r1, r1, #0	; 0x0
   1063c:	e0900e13 	adds	r0, r0, r3, lsl lr
   10640:	e0b11553 	adcs	r1, r1, r3, asr r5
   10644:	ea000006 	b	10664 <__adddf3+0xd8>
   10648:	e2455020 	sub	r5, r5, #32	; 0x20
   1064c:	e28ee020 	add	lr, lr, #32	; 0x20
   10650:	e3520001 	cmp	r2, #1	; 0x1
   10654:	e1a0ce13 	lsl	ip, r3, lr
   10658:	238cc002 	orrcs	ip, ip, #2	; 0x2
   1065c:	e0900553 	adds	r0, r0, r3, asr r5
   10660:	e0b11fc3 	adcs	r1, r1, r3, asr #31
   10664:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   10668:	5a000002 	bpl	10678 <__adddf3+0xec>
   1066c:	e27cc000 	rsbs	ip, ip, #0	; 0x0
   10670:	e2f00000 	rscs	r0, r0, #0	; 0x0
   10674:	e2e11000 	rsc	r1, r1, #0	; 0x0
   10678:	e3510601 	cmp	r1, #1048576	; 0x100000
   1067c:	3a00000e 	bcc	106bc <__adddf3+0x130>
   10680:	e3510602 	cmp	r1, #2097152	; 0x200000
   10684:	3a000006 	bcc	106a4 <__adddf3+0x118>
   10688:	e1b010a1 	lsrs	r1, r1, #1
   1068c:	e1b00060 	rrxs	r0, r0
   10690:	e1a0c06c 	rrx	ip, ip
   10694:	e2844001 	add	r4, r4, #1	; 0x1
   10698:	e1a02a84 	lsl	r2, r4, #21
   1069c:	e3720501 	cmn	r2, #4194304	; 0x400000
   106a0:	2a000055 	bcs	107fc <__adddf3+0x270>
   106a4:	e35c0102 	cmp	ip, #-2147483648	; 0x80000000
   106a8:	01b0c0a0 	lsrseq	ip, r0, #1
   106ac:	e2b00000 	adcs	r0, r0, #0	; 0x0
   106b0:	e0a11a04 	adc	r1, r1, r4, lsl #20
   106b4:	e1811005 	orr	r1, r1, r5
   106b8:	e8bd8030 	pop	{r4, r5, pc}
   106bc:	e1b0c08c 	lsls	ip, ip, #1
   106c0:	e0b00000 	adcs	r0, r0, r0
   106c4:	e0a11001 	adc	r1, r1, r1
   106c8:	e3110601 	tst	r1, #1048576	; 0x100000
   106cc:	e2444001 	sub	r4, r4, #1	; 0x1
   106d0:	1afffff3 	bne	106a4 <__adddf3+0x118>
   106d4:	e3310000 	teq	r1, #0	; 0x0
   106d8:	01a01000 	moveq	r1, r0
   106dc:	03a00000 	moveq	r0, #0	; 0x0
   106e0:	e16f3f11 	clz	r3, r1
   106e4:	02833020 	addeq	r3, r3, #32	; 0x20
   106e8:	e243300b 	sub	r3, r3, #11	; 0xb
   106ec:	e2532020 	subs	r2, r3, #32	; 0x20
   106f0:	aa000007 	bge	10714 <__adddf3+0x188>
   106f4:	e292200c 	adds	r2, r2, #12	; 0xc
   106f8:	da000004 	ble	10710 <__adddf3+0x184>
   106fc:	e282c014 	add	ip, r2, #20	; 0x14
   10700:	e262200c 	rsb	r2, r2, #12	; 0xc
   10704:	e1a00c11 	lsl	r0, r1, ip
   10708:	e1a01231 	lsr	r1, r1, r2
   1070c:	ea000004 	b	10724 <__adddf3+0x198>
   10710:	e2822014 	add	r2, r2, #20	; 0x14
   10714:	d262c020 	rsble	ip, r2, #32	; 0x20
   10718:	e1a01211 	lsl	r1, r1, r2
   1071c:	d1811c30 	orrle	r1, r1, r0, lsr ip
   10720:	d1a00210 	lslle	r0, r0, r2
   10724:	e0544003 	subs	r4, r4, r3
   10728:	a0811a04 	addge	r1, r1, r4, lsl #20
   1072c:	a1811005 	orrge	r1, r1, r5
   10730:	a8bd8030 	popge	{r4, r5, pc}
   10734:	e1e04004 	mvn	r4, r4
   10738:	e254401f 	subs	r4, r4, #31	; 0x1f
   1073c:	aa00000d 	bge	10778 <__adddf3+0x1ec>
   10740:	e294400c 	adds	r4, r4, #12	; 0xc
   10744:	ca000005 	bgt	10760 <__adddf3+0x1d4>
   10748:	e2844014 	add	r4, r4, #20	; 0x14
   1074c:	e2642020 	rsb	r2, r4, #32	; 0x20
   10750:	e1a00430 	lsr	r0, r0, r4
   10754:	e1800211 	orr	r0, r0, r1, lsl r2
   10758:	e1851431 	orr	r1, r5, r1, lsr r4
   1075c:	e8bd8030 	pop	{r4, r5, pc}
   10760:	e264400c 	rsb	r4, r4, #12	; 0xc
   10764:	e2642020 	rsb	r2, r4, #32	; 0x20
   10768:	e1a00230 	lsr	r0, r0, r2
   1076c:	e1800411 	orr	r0, r0, r1, lsl r4
   10770:	e1a01005 	mov	r1, r5
   10774:	e8bd8030 	pop	{r4, r5, pc}
   10778:	e1a00431 	lsr	r0, r1, r4
   1077c:	e1a01005 	mov	r1, r5
   10780:	e8bd8030 	pop	{r4, r5, pc}
   10784:	e3340000 	teq	r4, #0	; 0x0
   10788:	e2233601 	eor	r3, r3, #1048576	; 0x100000
   1078c:	02211601 	eoreq	r1, r1, #1048576	; 0x100000
   10790:	02844001 	addeq	r4, r4, #1	; 0x1
   10794:	12455001 	subne	r5, r5, #1	; 0x1
   10798:	eaffffa1 	b	10624 <__adddf3+0x98>
   1079c:	e1f0cac4 	mvns	ip, r4, asr #21
   107a0:	11f0cac5 	mvnsne	ip, r5, asr #21
   107a4:	0a000018 	beq	1080c <__adddf3+0x280>
   107a8:	e1340005 	teq	r4, r5
   107ac:	01300002 	teqeq	r0, r2
   107b0:	0a000003 	beq	107c4 <__adddf3+0x238>
   107b4:	e194c000 	orrs	ip, r4, r0
   107b8:	01a01003 	moveq	r1, r3
   107bc:	01a00002 	moveq	r0, r2
   107c0:	e8bd8030 	pop	{r4, r5, pc}
   107c4:	e1310003 	teq	r1, r3
   107c8:	13a01000 	movne	r1, #0	; 0x0
   107cc:	13a00000 	movne	r0, #0	; 0x0
   107d0:	18bd8030 	popne	{r4, r5, pc}
   107d4:	e1b0caa4 	lsrs	ip, r4, #21
   107d8:	1a000003 	bne	107ec <__adddf3+0x260>
   107dc:	e1b00080 	lsls	r0, r0, #1
   107e0:	e0b11001 	adcs	r1, r1, r1
   107e4:	23811102 	orrcs	r1, r1, #-2147483648	; 0x80000000
   107e8:	e8bd8030 	pop	{r4, r5, pc}
   107ec:	e2944501 	adds	r4, r4, #4194304	; 0x400000
   107f0:	32811601 	addcc	r1, r1, #1048576	; 0x100000
   107f4:	38bd8030 	popcc	{r4, r5, pc}
   107f8:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   107fc:	e385147f 	orr	r1, r5, #2130706432	; 0x7f000000
   10800:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   10804:	e3a00000 	mov	r0, #0	; 0x0
   10808:	e8bd8030 	pop	{r4, r5, pc}
   1080c:	e1f0cac4 	mvns	ip, r4, asr #21
   10810:	11a01003 	movne	r1, r3
   10814:	11a00002 	movne	r0, r2
   10818:	01f0cac5 	mvnseq	ip, r5, asr #21
   1081c:	11a03001 	movne	r3, r1
   10820:	11a02000 	movne	r2, r0
   10824:	e1904601 	orrs	r4, r0, r1, lsl #12
   10828:	01925603 	orrseq	r5, r2, r3, lsl #12
   1082c:	01310003 	teqeq	r1, r3
   10830:	13811702 	orrne	r1, r1, #524288	; 0x80000
   10834:	e8bd8030 	pop	{r4, r5, pc}

00010838 <__aeabi_ui2d>:
   10838:	e3300000 	teq	r0, #0	; 0x0
   1083c:	03a01000 	moveq	r1, #0	; 0x0
   10840:	012fff1e 	bxeq	lr
   10844:	e92d4030 	push	{r4, r5, lr}
   10848:	e3a04b01 	mov	r4, #1024	; 0x400
   1084c:	e2844032 	add	r4, r4, #50	; 0x32
   10850:	e3a05000 	mov	r5, #0	; 0x0
   10854:	e3a01000 	mov	r1, #0	; 0x0
   10858:	eaffff9d 	b	106d4 <__adddf3+0x148>

0001085c <__aeabi_i2d>:
   1085c:	e3300000 	teq	r0, #0	; 0x0
   10860:	03a01000 	moveq	r1, #0	; 0x0
   10864:	012fff1e 	bxeq	lr
   10868:	e92d4030 	push	{r4, r5, lr}
   1086c:	e3a04b01 	mov	r4, #1024	; 0x400
   10870:	e2844032 	add	r4, r4, #50	; 0x32
   10874:	e2105102 	ands	r5, r0, #-2147483648	; 0x80000000
   10878:	42600000 	rsbmi	r0, r0, #0	; 0x0
   1087c:	e3a01000 	mov	r1, #0	; 0x0
   10880:	eaffff93 	b	106d4 <__adddf3+0x148>

00010884 <__aeabi_f2d>:
   10884:	e1b02080 	lsls	r2, r0, #1
   10888:	e1a011c2 	asr	r1, r2, #3
   1088c:	e1a01061 	rrx	r1, r1
   10890:	e1a00e02 	lsl	r0, r2, #28
   10894:	121234ff 	andsne	r3, r2, #-16777216	; 0xff000000
   10898:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   1089c:	1221130e 	eorne	r1, r1, #939524096	; 0x38000000
   108a0:	112fff1e 	bxne	lr
   108a4:	e3320000 	teq	r2, #0	; 0x0
   108a8:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   108ac:	012fff1e 	bxeq	lr
   108b0:	e92d4030 	push	{r4, r5, lr}
   108b4:	e3a04d0e 	mov	r4, #896	; 0x380
   108b8:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   108bc:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   108c0:	eaffff83 	b	106d4 <__adddf3+0x148>

000108c4 <__aeabi_ul2d>:
   108c4:	e1902001 	orrs	r2, r0, r1
   108c8:	012fff1e 	bxeq	lr
   108cc:	e92d4030 	push	{r4, r5, lr}
   108d0:	e3a05000 	mov	r5, #0	; 0x0
   108d4:	ea000006 	b	108f4 <__aeabi_l2d+0x1c>

000108d8 <__aeabi_l2d>:
   108d8:	e1902001 	orrs	r2, r0, r1
   108dc:	012fff1e 	bxeq	lr
   108e0:	e92d4030 	push	{r4, r5, lr}
   108e4:	e2115102 	ands	r5, r1, #-2147483648	; 0x80000000
   108e8:	5a000001 	bpl	108f4 <__aeabi_l2d+0x1c>
   108ec:	e2700000 	rsbs	r0, r0, #0	; 0x0
   108f0:	e2e11000 	rsc	r1, r1, #0	; 0x0
   108f4:	e3a04b01 	mov	r4, #1024	; 0x400
   108f8:	e2844032 	add	r4, r4, #50	; 0x32
   108fc:	e1b0cb21 	lsrs	ip, r1, #22
   10900:	0affff5c 	beq	10678 <__adddf3+0xec>
   10904:	e3a02003 	mov	r2, #3	; 0x3
   10908:	e1b0c1ac 	lsrs	ip, ip, #3
   1090c:	12822003 	addne	r2, r2, #3	; 0x3
   10910:	e1b0c1ac 	lsrs	ip, ip, #3
   10914:	12822003 	addne	r2, r2, #3	; 0x3
   10918:	e08221ac 	add	r2, r2, ip, lsr #3
   1091c:	e2623020 	rsb	r3, r2, #32	; 0x20
   10920:	e1a0c310 	lsl	ip, r0, r3
   10924:	e1a00230 	lsr	r0, r0, r2
   10928:	e1800311 	orr	r0, r0, r1, lsl r3
   1092c:	e1a01231 	lsr	r1, r1, r2
   10930:	e0844002 	add	r4, r4, r2
   10934:	eaffff4f 	b	10678 <__adddf3+0xec>

00010938 <__aeabi_dmul>:
   10938:	e92d4070 	push	{r4, r5, r6, lr}
   1093c:	e3a0c0ff 	mov	ip, #255	; 0xff
   10940:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   10944:	e01c4a21 	ands	r4, ip, r1, lsr #20
   10948:	101c5a23 	andsne	r5, ip, r3, lsr #20
   1094c:	1134000c 	teqne	r4, ip
   10950:	1135000c 	teqne	r5, ip
   10954:	0b00006f 	bleq	10b18 <__aeabi_dmul+0x1e0>
   10958:	e0844005 	add	r4, r4, r5
   1095c:	e0216003 	eor	r6, r1, r3
   10960:	e1c11a8c 	bic	r1, r1, ip, lsl #21
   10964:	e1c33a8c 	bic	r3, r3, ip, lsl #21
   10968:	e1905601 	orrs	r5, r0, r1, lsl #12
   1096c:	11925603 	orrsne	r5, r2, r3, lsl #12
   10970:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   10974:	e3833601 	orr	r3, r3, #1048576	; 0x100000
   10978:	0a00001c 	beq	109f0 <__aeabi_dmul+0xb8>
   1097c:	e08ec290 	umull	ip, lr, r0, r2
   10980:	e3a05000 	mov	r5, #0	; 0x0
   10984:	e0a5e291 	umlal	lr, r5, r1, r2
   10988:	e2062102 	and	r2, r6, #-2147483648	; 0x80000000
   1098c:	e0a5e390 	umlal	lr, r5, r0, r3
   10990:	e3a06000 	mov	r6, #0	; 0x0
   10994:	e0a65391 	umlal	r5, r6, r1, r3
   10998:	e33c0000 	teq	ip, #0	; 0x0
   1099c:	138ee001 	orrne	lr, lr, #1	; 0x1
   109a0:	e24440ff 	sub	r4, r4, #255	; 0xff
   109a4:	e3560c02 	cmp	r6, #512	; 0x200
   109a8:	e2c44c03 	sbc	r4, r4, #768	; 0x300
   109ac:	2a000002 	bcs	109bc <__aeabi_dmul+0x84>
   109b0:	e1b0e08e 	lsls	lr, lr, #1
   109b4:	e0b55005 	adcs	r5, r5, r5
   109b8:	e0a66006 	adc	r6, r6, r6
   109bc:	e1821586 	orr	r1, r2, r6, lsl #11
   109c0:	e1811aa5 	orr	r1, r1, r5, lsr #21
   109c4:	e1a00585 	lsl	r0, r5, #11
   109c8:	e1800aae 	orr	r0, r0, lr, lsr #21
   109cc:	e1a0e58e 	lsl	lr, lr, #11
   109d0:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   109d4:	835c0c07 	cmphi	ip, #1792	; 0x700
   109d8:	8a00000f 	bhi	10a1c <__aeabi_dmul+0xe4>
   109dc:	e35e0102 	cmp	lr, #-2147483648	; 0x80000000
   109e0:	01b0e0a0 	lsrseq	lr, r0, #1
   109e4:	e2b00000 	adcs	r0, r0, #0	; 0x0
   109e8:	e0a11a04 	adc	r1, r1, r4, lsl #20
   109ec:	e8bd8070 	pop	{r4, r5, r6, pc}
   109f0:	e2066102 	and	r6, r6, #-2147483648	; 0x80000000
   109f4:	e1861001 	orr	r1, r6, r1
   109f8:	e1800002 	orr	r0, r0, r2
   109fc:	e0211003 	eor	r1, r1, r3
   10a00:	e05440ac 	subs	r4, r4, ip, lsr #1
   10a04:	c074500c 	rsbsgt	r5, r4, ip
   10a08:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   10a0c:	c8bd8070 	popgt	{r4, r5, r6, pc}
   10a10:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   10a14:	e3a0e000 	mov	lr, #0	; 0x0
   10a18:	e2544001 	subs	r4, r4, #1	; 0x1
   10a1c:	ca000058 	bgt	10b84 <__aeabi_dmul+0x24c>
   10a20:	e3740036 	cmn	r4, #54	; 0x36
   10a24:	d3a00000 	movle	r0, #0	; 0x0
   10a28:	d2011102 	andle	r1, r1, #-2147483648	; 0x80000000
   10a2c:	d8bd8070 	pople	{r4, r5, r6, pc}
   10a30:	e2644000 	rsb	r4, r4, #0	; 0x0
   10a34:	e2544020 	subs	r4, r4, #32	; 0x20
   10a38:	aa000018 	bge	10aa0 <__aeabi_dmul+0x168>
   10a3c:	e294400c 	adds	r4, r4, #12	; 0xc
   10a40:	ca00000b 	bgt	10a74 <__aeabi_dmul+0x13c>
   10a44:	e2844014 	add	r4, r4, #20	; 0x14
   10a48:	e2645020 	rsb	r5, r4, #32	; 0x20
   10a4c:	e1a03510 	lsl	r3, r0, r5
   10a50:	e1a00430 	lsr	r0, r0, r4
   10a54:	e1800511 	orr	r0, r0, r1, lsl r5
   10a58:	e2012102 	and	r2, r1, #-2147483648	; 0x80000000
   10a5c:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   10a60:	e0900fa3 	adds	r0, r0, r3, lsr #31
   10a64:	e0a21431 	adc	r1, r2, r1, lsr r4
   10a68:	e19ee083 	orrs	lr, lr, r3, lsl #1
   10a6c:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   10a70:	e8bd8070 	pop	{r4, r5, r6, pc}
   10a74:	e264400c 	rsb	r4, r4, #12	; 0xc
   10a78:	e2645020 	rsb	r5, r4, #32	; 0x20
   10a7c:	e1a03410 	lsl	r3, r0, r4
   10a80:	e1a00530 	lsr	r0, r0, r5
   10a84:	e1800411 	orr	r0, r0, r1, lsl r4
   10a88:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10a8c:	e0900fa3 	adds	r0, r0, r3, lsr #31
   10a90:	e2a11000 	adc	r1, r1, #0	; 0x0
   10a94:	e19ee083 	orrs	lr, lr, r3, lsl #1
   10a98:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   10a9c:	e8bd8070 	pop	{r4, r5, r6, pc}
   10aa0:	e2645020 	rsb	r5, r4, #32	; 0x20
   10aa4:	e18ee510 	orr	lr, lr, r0, lsl r5
   10aa8:	e1a03430 	lsr	r3, r0, r4
   10aac:	e1833511 	orr	r3, r3, r1, lsl r5
   10ab0:	e1a00431 	lsr	r0, r1, r4
   10ab4:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10ab8:	e1c00431 	bic	r0, r0, r1, lsr r4
   10abc:	e0800fa3 	add	r0, r0, r3, lsr #31
   10ac0:	e19ee083 	orrs	lr, lr, r3, lsl #1
   10ac4:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   10ac8:	e8bd8070 	pop	{r4, r5, r6, pc}
   10acc:	e3340000 	teq	r4, #0	; 0x0
   10ad0:	1a000008 	bne	10af8 <__aeabi_dmul+0x1c0>
   10ad4:	e2016102 	and	r6, r1, #-2147483648	; 0x80000000
   10ad8:	e1b00080 	lsls	r0, r0, #1
   10adc:	e0a11001 	adc	r1, r1, r1
   10ae0:	e3110601 	tst	r1, #1048576	; 0x100000
   10ae4:	02444001 	subeq	r4, r4, #1	; 0x1
   10ae8:	0afffffa 	beq	10ad8 <__aeabi_dmul+0x1a0>
   10aec:	e1811006 	orr	r1, r1, r6
   10af0:	e3350000 	teq	r5, #0	; 0x0
   10af4:	11a0f00e 	movne	pc, lr
   10af8:	e2036102 	and	r6, r3, #-2147483648	; 0x80000000
   10afc:	e1b02082 	lsls	r2, r2, #1
   10b00:	e0a33003 	adc	r3, r3, r3
   10b04:	e3130601 	tst	r3, #1048576	; 0x100000
   10b08:	02455001 	subeq	r5, r5, #1	; 0x1
   10b0c:	0afffffa 	beq	10afc <__aeabi_dmul+0x1c4>
   10b10:	e1833006 	orr	r3, r3, r6
   10b14:	e1a0f00e 	mov	pc, lr
   10b18:	e134000c 	teq	r4, ip
   10b1c:	e00c5a23 	and	r5, ip, r3, lsr #20
   10b20:	1135000c 	teqne	r5, ip
   10b24:	0a000006 	beq	10b44 <__aeabi_dmul+0x20c>
   10b28:	e1906081 	orrs	r6, r0, r1, lsl #1
   10b2c:	11926083 	orrsne	r6, r2, r3, lsl #1
   10b30:	1affffe5 	bne	10acc <__aeabi_dmul+0x194>
   10b34:	e0211003 	eor	r1, r1, r3
   10b38:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10b3c:	e3a00000 	mov	r0, #0	; 0x0
   10b40:	e8bd8070 	pop	{r4, r5, r6, pc}
   10b44:	e1906081 	orrs	r6, r0, r1, lsl #1
   10b48:	01a00002 	moveq	r0, r2
   10b4c:	01a01003 	moveq	r1, r3
   10b50:	11926083 	orrsne	r6, r2, r3, lsl #1
   10b54:	0a00000f 	beq	10b98 <__aeabi_dmul+0x260>
   10b58:	e134000c 	teq	r4, ip
   10b5c:	1a000001 	bne	10b68 <__aeabi_dmul+0x230>
   10b60:	e1906601 	orrs	r6, r0, r1, lsl #12
   10b64:	1a00000b 	bne	10b98 <__aeabi_dmul+0x260>
   10b68:	e135000c 	teq	r5, ip
   10b6c:	1a000003 	bne	10b80 <__aeabi_dmul+0x248>
   10b70:	e1926603 	orrs	r6, r2, r3, lsl #12
   10b74:	11a00002 	movne	r0, r2
   10b78:	11a01003 	movne	r1, r3
   10b7c:	1a000005 	bne	10b98 <__aeabi_dmul+0x260>
   10b80:	e0211003 	eor	r1, r1, r3
   10b84:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10b88:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   10b8c:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   10b90:	e3a00000 	mov	r0, #0	; 0x0
   10b94:	e8bd8070 	pop	{r4, r5, r6, pc}
   10b98:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   10b9c:	e381173e 	orr	r1, r1, #16252928	; 0xf80000
   10ba0:	e8bd8070 	pop	{r4, r5, r6, pc}

00010ba4 <__aeabi_ddiv>:
   10ba4:	e92d4070 	push	{r4, r5, r6, lr}
   10ba8:	e3a0c0ff 	mov	ip, #255	; 0xff
   10bac:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   10bb0:	e01c4a21 	ands	r4, ip, r1, lsr #20
   10bb4:	101c5a23 	andsne	r5, ip, r3, lsr #20
   10bb8:	1134000c 	teqne	r4, ip
   10bbc:	1135000c 	teqne	r5, ip
   10bc0:	0b00005c 	bleq	10d38 <__aeabi_ddiv+0x194>
   10bc4:	e0444005 	sub	r4, r4, r5
   10bc8:	e021e003 	eor	lr, r1, r3
   10bcc:	e1925603 	orrs	r5, r2, r3, lsl #12
   10bd0:	e1a01601 	lsl	r1, r1, #12
   10bd4:	0a00004b 	beq	10d08 <__aeabi_ddiv+0x164>
   10bd8:	e1a03603 	lsl	r3, r3, #12
   10bdc:	e3a05201 	mov	r5, #268435456	; 0x10000000
   10be0:	e1853223 	orr	r3, r5, r3, lsr #4
   10be4:	e1833c22 	orr	r3, r3, r2, lsr #24
   10be8:	e1a02402 	lsl	r2, r2, #8
   10bec:	e1855221 	orr	r5, r5, r1, lsr #4
   10bf0:	e1855c20 	orr	r5, r5, r0, lsr #24
   10bf4:	e1a06400 	lsl	r6, r0, #8
   10bf8:	e20e1102 	and	r1, lr, #-2147483648	; 0x80000000
   10bfc:	e1550003 	cmp	r5, r3
   10c00:	01560002 	cmpeq	r6, r2
   10c04:	e2a440fd 	adc	r4, r4, #253	; 0xfd
   10c08:	e2844c03 	add	r4, r4, #768	; 0x300
   10c0c:	2a000001 	bcs	10c18 <__aeabi_ddiv+0x74>
   10c10:	e1b030a3 	lsrs	r3, r3, #1
   10c14:	e1a02062 	rrx	r2, r2
   10c18:	e0566002 	subs	r6, r6, r2
   10c1c:	e0c55003 	sbc	r5, r5, r3
   10c20:	e1b030a3 	lsrs	r3, r3, #1
   10c24:	e1a02062 	rrx	r2, r2
   10c28:	e3a00601 	mov	r0, #1048576	; 0x100000
   10c2c:	e3a0c702 	mov	ip, #524288	; 0x80000
   10c30:	e056e002 	subs	lr, r6, r2
   10c34:	e0d5e003 	sbcs	lr, r5, r3
   10c38:	20466002 	subcs	r6, r6, r2
   10c3c:	21a0500e 	movcs	r5, lr
   10c40:	2180000c 	orrcs	r0, r0, ip
   10c44:	e1b030a3 	lsrs	r3, r3, #1
   10c48:	e1a02062 	rrx	r2, r2
   10c4c:	e056e002 	subs	lr, r6, r2
   10c50:	e0d5e003 	sbcs	lr, r5, r3
   10c54:	20466002 	subcs	r6, r6, r2
   10c58:	21a0500e 	movcs	r5, lr
   10c5c:	218000ac 	orrcs	r0, r0, ip, lsr #1
   10c60:	e1b030a3 	lsrs	r3, r3, #1
   10c64:	e1a02062 	rrx	r2, r2
   10c68:	e056e002 	subs	lr, r6, r2
   10c6c:	e0d5e003 	sbcs	lr, r5, r3
   10c70:	20466002 	subcs	r6, r6, r2
   10c74:	21a0500e 	movcs	r5, lr
   10c78:	2180012c 	orrcs	r0, r0, ip, lsr #2
   10c7c:	e1b030a3 	lsrs	r3, r3, #1
   10c80:	e1a02062 	rrx	r2, r2
   10c84:	e056e002 	subs	lr, r6, r2
   10c88:	e0d5e003 	sbcs	lr, r5, r3
   10c8c:	20466002 	subcs	r6, r6, r2
   10c90:	21a0500e 	movcs	r5, lr
   10c94:	218001ac 	orrcs	r0, r0, ip, lsr #3
   10c98:	e195e006 	orrs	lr, r5, r6
   10c9c:	0a00000d 	beq	10cd8 <__aeabi_ddiv+0x134>
   10ca0:	e1a05205 	lsl	r5, r5, #4
   10ca4:	e1855e26 	orr	r5, r5, r6, lsr #28
   10ca8:	e1a06206 	lsl	r6, r6, #4
   10cac:	e1a03183 	lsl	r3, r3, #3
   10cb0:	e1833ea2 	orr	r3, r3, r2, lsr #29
   10cb4:	e1a02182 	lsl	r2, r2, #3
   10cb8:	e1b0c22c 	lsrs	ip, ip, #4
   10cbc:	1affffdb 	bne	10c30 <__aeabi_ddiv+0x8c>
   10cc0:	e3110601 	tst	r1, #1048576	; 0x100000
   10cc4:	1a000006 	bne	10ce4 <__aeabi_ddiv+0x140>
   10cc8:	e1811000 	orr	r1, r1, r0
   10ccc:	e3a00000 	mov	r0, #0	; 0x0
   10cd0:	e3a0c102 	mov	ip, #-2147483648	; 0x80000000
   10cd4:	eaffffd5 	b	10c30 <__aeabi_ddiv+0x8c>
   10cd8:	e3110601 	tst	r1, #1048576	; 0x100000
   10cdc:	01811000 	orreq	r1, r1, r0
   10ce0:	03a00000 	moveq	r0, #0	; 0x0
   10ce4:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   10ce8:	835c0c07 	cmphi	ip, #1792	; 0x700
   10cec:	8affff4a 	bhi	10a1c <__aeabi_dmul+0xe4>
   10cf0:	e055c003 	subs	ip, r5, r3
   10cf4:	0056c002 	subseq	ip, r6, r2
   10cf8:	01b0c0a0 	lsrseq	ip, r0, #1
   10cfc:	e2b00000 	adcs	r0, r0, #0	; 0x0
   10d00:	e0a11a04 	adc	r1, r1, r4, lsl #20
   10d04:	e8bd8070 	pop	{r4, r5, r6, pc}
   10d08:	e20ee102 	and	lr, lr, #-2147483648	; 0x80000000
   10d0c:	e18e1621 	orr	r1, lr, r1, lsr #12
   10d10:	e09440ac 	adds	r4, r4, ip, lsr #1
   10d14:	c074500c 	rsbsgt	r5, r4, ip
   10d18:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   10d1c:	c8bd8070 	popgt	{r4, r5, r6, pc}
   10d20:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   10d24:	e3a0e000 	mov	lr, #0	; 0x0
   10d28:	e2544001 	subs	r4, r4, #1	; 0x1
   10d2c:	eaffff3a 	b	10a1c <__aeabi_dmul+0xe4>
   10d30:	e185e006 	orr	lr, r5, r6
   10d34:	eaffff38 	b	10a1c <__aeabi_dmul+0xe4>
   10d38:	e00c5a23 	and	r5, ip, r3, lsr #20
   10d3c:	e134000c 	teq	r4, ip
   10d40:	0135000c 	teqeq	r5, ip
   10d44:	0affff93 	beq	10b98 <__aeabi_dmul+0x260>
   10d48:	e134000c 	teq	r4, ip
   10d4c:	1a000006 	bne	10d6c <__aeabi_ddiv+0x1c8>
   10d50:	e1904601 	orrs	r4, r0, r1, lsl #12
   10d54:	1affff8f 	bne	10b98 <__aeabi_dmul+0x260>
   10d58:	e135000c 	teq	r5, ip
   10d5c:	1affff87 	bne	10b80 <__aeabi_dmul+0x248>
   10d60:	e1a00002 	mov	r0, r2
   10d64:	e1a01003 	mov	r1, r3
   10d68:	eaffff8a 	b	10b98 <__aeabi_dmul+0x260>
   10d6c:	e135000c 	teq	r5, ip
   10d70:	1a000004 	bne	10d88 <__aeabi_ddiv+0x1e4>
   10d74:	e1925603 	orrs	r5, r2, r3, lsl #12
   10d78:	0affff6d 	beq	10b34 <__aeabi_dmul+0x1fc>
   10d7c:	e1a00002 	mov	r0, r2
   10d80:	e1a01003 	mov	r1, r3
   10d84:	eaffff83 	b	10b98 <__aeabi_dmul+0x260>
   10d88:	e1906081 	orrs	r6, r0, r1, lsl #1
   10d8c:	11926083 	orrsne	r6, r2, r3, lsl #1
   10d90:	1affff4d 	bne	10acc <__aeabi_dmul+0x194>
   10d94:	e1904081 	orrs	r4, r0, r1, lsl #1
   10d98:	1affff78 	bne	10b80 <__aeabi_dmul+0x248>
   10d9c:	e1925083 	orrs	r5, r2, r3, lsl #1
   10da0:	1affff63 	bne	10b34 <__aeabi_dmul+0x1fc>
   10da4:	eaffff7b 	b	10b98 <__aeabi_dmul+0x260>

00010da8 <__aeabi_d2iz>:
   10da8:	e1a02081 	lsl	r2, r1, #1
   10dac:	e2922602 	adds	r2, r2, #2097152	; 0x200000
   10db0:	2a00000c 	bcs	10de8 <__aeabi_d2iz+0x40>
   10db4:	5a000009 	bpl	10de0 <__aeabi_d2iz+0x38>
   10db8:	e3e03e3e 	mvn	r3, #992	; 0x3e0
   10dbc:	e0532ac2 	subs	r2, r3, r2, asr #21
   10dc0:	9a00000a 	bls	10df0 <__aeabi_d2iz+0x48>
   10dc4:	e1a03581 	lsl	r3, r1, #11
   10dc8:	e3833102 	orr	r3, r3, #-2147483648	; 0x80000000
   10dcc:	e1833aa0 	orr	r3, r3, r0, lsr #21
   10dd0:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   10dd4:	e1a00233 	lsr	r0, r3, r2
   10dd8:	12600000 	rsbne	r0, r0, #0	; 0x0
   10ddc:	e12fff1e 	bx	lr
   10de0:	e3a00000 	mov	r0, #0	; 0x0
   10de4:	e12fff1e 	bx	lr
   10de8:	e1900601 	orrs	r0, r0, r1, lsl #12
   10dec:	1a000002 	bne	10dfc <__aeabi_d2iz+0x54>
   10df0:	e2110102 	ands	r0, r1, #-2147483648	; 0x80000000
   10df4:	03e00102 	mvneq	r0, #-2147483648	; 0x80000000
   10df8:	e12fff1e 	bx	lr
   10dfc:	e3a00000 	mov	r0, #0	; 0x0
   10e00:	e12fff1e 	bx	lr

00010e04 <__div0>:
   10e04:	e92d4002 	push	{r1, lr}
   10e08:	e3a00008 	mov	r0, #8	; 0x8
   10e0c:	ebfffcb1 	bl	100d8 <raise>
   10e10:	e8bd8002 	pop	{r1, pc}
