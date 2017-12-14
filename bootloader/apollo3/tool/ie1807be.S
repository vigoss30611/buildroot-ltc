
/home/chamber/ivan/projects/irom_gerrit/irom/out/vmirom:     file format elf32-littlearm


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
      44:	0800d1a0 	.word	0x0800d1a0

00000048 <_bss_end>:
      48:	0800dd00 	.word	0x0800dd00

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
      78:	eb0003ef 	bl	103c <chip_test>
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
      9c:	00000524 	.word	0x00000524

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
     16c:	eb0013fe 	bl	516c <vs_assign_by_id>
     170:	e2506000 	subs	r6, r0, #0	; 0x0
     174:	0a000008 	beq	19c <bl_load_isi+0x4c>
     178:	e59f3178 	ldr	r3, [pc, #376]	; 2f8 <_data_len+0x158>
     17c:	e1a00004 	mov	r0, r4
     180:	e5934000 	ldr	r4, [r3]
     184:	eb001301 	bl	4d90 <vs_device_name>
     188:	e1a02006 	mov	r2, r6
     18c:	e1a01000 	mov	r1, r0
     190:	e59f0164 	ldr	r0, [pc, #356]	; 2fc <_data_len+0x15c>
     194:	e12fff34 	blx	r4
     198:	ea000053 	b	2ec <_data_len+0x14c>
     19c:	e1a00004 	mov	r0, r4
     1a0:	e59f1158 	ldr	r1, [pc, #344]	; 300 <_data_len+0x160>
     1a4:	eb001330 	bl	4e6c <vs_is_device>
     1a8:	e8990030 	ldm	r9, {r4, r5}
     1ac:	e59f2150 	ldr	r2, [pc, #336]	; 304 <_data_len+0x164>
     1b0:	e3a03000 	mov	r3, #0	; 0x0
     1b4:	e0922004 	adds	r2, r2, r4
     1b8:	e59f4148 	ldr	r4, [pc, #328]	; 308 <_data_len+0x168>
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
     1fc:	1a00000e 	bne	23c <_data_len+0x9c>
     200:	e59d0024 	ldr	r0, [sp, #36]
     204:	eb0005e5 	bl	19a0 <isi_check_header>
     208:	e2502000 	subs	r2, r0, #0	; 0x0
     20c:	e59f00f8 	ldr	r0, [pc, #248]	; 30c <_data_len+0x16c>
     210:	1a000007 	bne	234 <_data_len+0x94>
     214:	e59da024 	ldr	sl, [sp, #36]
     218:	e59fb0d8 	ldr	fp, [pc, #216]	; 2f8 <_data_len+0x158>
     21c:	e59a300c 	ldr	r3, [sl, #12]
     220:	e2835c02 	add	r5, r3, #512	; 0x200
     224:	e1a01005 	mov	r1, r5
     228:	e1a0e00f 	mov	lr, pc
     22c:	e59bf000 	ldr	pc, [fp]
     230:	ea000001 	b	23c <_data_len+0x9c>
     234:	e59f00d4 	ldr	r0, [pc, #212]	; 310 <_data_len+0x170>
     238:	ea00000c 	b	270 <_data_len+0xd0>
     23c:	e2554000 	subs	r4, r5, #0	; 0x0
     240:	13a04001 	movne	r4, #1	; 0x1
     244:	e1570005 	cmp	r7, r5
     248:	e59d1024 	ldr	r1, [sp, #36]
     24c:	b3a04000 	movlt	r4, #0	; 0x0
     250:	e3540000 	cmp	r4, #0	; 0x0
     254:	e0810007 	add	r0, r1, r7
     258:	0a000006 	beq	278 <_data_len+0xd8>
     25c:	e1a00001 	mov	r0, r1
     260:	eb000627 	bl	1b04 <isi_check_data>
     264:	e3500000 	cmp	r0, #0	; 0x0
     268:	0a00001f 	beq	2ec <_data_len+0x14c>
     26c:	e59f00a0 	ldr	r0, [pc, #160]	; 314 <_data_len+0x174>
     270:	e3a01000 	mov	r1, #0	; 0x0
     274:	ea000006 	b	294 <_data_len+0xf4>
     278:	e59d200c 	ldr	r2, [sp, #12]
     27c:	e3520004 	cmp	r2, #4	; 0x4
     280:	e2822001 	add	r2, r2, #1	; 0x1
     284:	e58d200c 	str	r2, [sp, #12]
     288:	da000005 	ble	2a4 <_data_len+0x104>
     28c:	e59f0080 	ldr	r0, [pc, #128]	; 314 <_data_len+0x174>
     290:	e1a01004 	mov	r1, r4
     294:	e59f305c 	ldr	r3, [pc, #92]	; 2f8 <_data_len+0x158>
     298:	e1a0e00f 	mov	lr, pc
     29c:	e593f000 	ldr	pc, [r3]
     2a0:	ea000010 	b	2e8 <_data_len+0x148>
     2a4:	e899000c 	ldm	r9, {r2, r3}
     2a8:	e58d8000 	str	r8, [sp]
     2ac:	e58d4004 	str	r4, [sp, #4]
     2b0:	eb001306 	bl	4ed0 <vs_read>
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
     2e4:	1affffc3 	bne	1f8 <_data_len+0x58>
     2e8:	e3e06000 	mvn	r6, #0	; 0x0
     2ec:	e1a00006 	mov	r0, r6
     2f0:	e28dd02c 	add	sp, sp, #44	; 0x2c
     2f4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     2f8:	0800d764 	.word	0x0800d764
     2fc:	00011a5e 	.word	0x00011a5e
     300:	00011a82 	.word	0x00011a82
     304:	0001ffff 	.word	0x0001ffff
     308:	fffe0000 	.word	0xfffe0000
     30c:	00011a87 	.word	0x00011a87
     310:	00011a9e 	.word	0x00011a9e
     314:	00011ab0 	.word	0x00011ab0

00000318 <try_get_bl>:
     318:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     31c:	e3a02602 	mov	r2, #2097152	; 0x200000
     320:	e24dd00c 	sub	sp, sp, #12	; 0xc
     324:	e3a03000 	mov	r3, #0	; 0x0
     328:	e88d000c 	stm	sp, {r2, r3}
     32c:	e1a07001 	mov	r7, r1
     330:	e1a06000 	mov	r6, r0
     334:	eb0000ab 	bl	5e8 <boot_device>
     338:	e1a04000 	mov	r4, r0
     33c:	eb00124c 	bl	4c74 <vs_device_bootable>
     340:	e2505000 	subs	r5, r0, #0	; 0x0
     344:	1a000008 	bne	36c <try_get_bl+0x54>
     348:	e59f308c 	ldr	r3, [pc, #140]	; 3dc <try_get_bl+0xc4>
     34c:	e1a00004 	mov	r0, r4
     350:	e5934000 	ldr	r4, [r3]
     354:	eb00128d 	bl	4d90 <vs_device_name>
     358:	e1a01000 	mov	r1, r0
     35c:	e59f007c 	ldr	r0, [pc, #124]	; 3e0 <try_get_bl+0xc8>
     360:	e12fff34 	blx	r4
     364:	e1a00005 	mov	r0, r5
     368:	ea000019 	b	3d4 <try_get_bl+0xbc>
     36c:	e1a00004 	mov	r0, r4
     370:	eb001274 	bl	4d48 <vs_device_boot0>
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
     3cc:	eb00173a 	bl	60bc <cdbg_verify_burn_enable>
     3d0:	e3a00001 	mov	r0, #1	; 0x1
     3d4:	e28dd00c 	add	sp, sp, #12	; 0xc
     3d8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     3dc:	0800d764 	.word	0x0800d764
     3e0:	00011ac2 	.word	0x00011ac2
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
     408:	eb00005c 	bl	580 <set_xom>
     40c:	eb000062 	bl	59c <boot_info>
     410:	e28d8004 	add	r8, sp, #4	; 0x4
     414:	e1a05000 	mov	r5, r0
     418:	ea000008 	b	440 <try_boot+0x58>
     41c:	eb00015e 	bl	99c <gtc_time>
     420:	e1a00004 	mov	r0, r4
     424:	eb000055 	bl	580 <set_xom>
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
     450:	eb000151 	bl	99c <gtc_time>
     454:	e3a00001 	mov	r0, #1	; 0x1
     458:	eb000165 	bl	9f4 <ecfg_check_flag>
     45c:	e3500000 	cmp	r0, #0	; 0x0
     460:	1a00000c 	bne	498 <try_boot+0xb0>
     464:	e3570000 	cmp	r7, #0	; 0x0
     468:	1a00000a 	bne	498 <try_boot+0xb0>
     46c:	eb0000c1 	bl	778 <get_reset_stats>
     470:	e3500000 	cmp	r0, #0	; 0x0
     474:	13500002 	cmpne	r0, #2	; 0x2
     478:	1a000006 	bne	498 <try_boot+0xb0>
     47c:	e3500002 	cmp	r0, #2	; 0x2
     480:	1a000001 	bne	48c <try_boot+0xa4>
     484:	e28d0004 	add	r0, sp, #4	; 0x4
     488:	eb0000c7 	bl	7ac <update_mask_on_reset>
     48c:	e59d0004 	ldr	r0, [sp, #4]
     490:	eb0018c7 	bl	67b4 <burn>
     494:	eb000140 	bl	99c <gtc_time>
     498:	e3540000 	cmp	r4, #0	; 0x0
     49c:	0a000004 	beq	4b4 <try_boot+0xcc>
     4a0:	e3a03302 	mov	r3, #134217728	; 0x8000000
     4a4:	e5930014 	ldr	r0, [r3, #20]
     4a8:	e59d1000 	ldr	r1, [sp]
     4ac:	eb00170d 	bl	60e8 <cdbg_jump>
     4b0:	eafffffe 	b	4b0 <try_boot+0xc8>
     4b4:	eb00173d 	bl	61b0 <cdbg_shell>
     4b8:	eafffffe 	b	4b8 <try_boot+0xd0>

000004bc <init_data_section>:
     4bc:	e59f1010 	ldr	r1, [pc, #16]	; 4d4 <init_data_section+0x18>
     4c0:	e59f0010 	ldr	r0, [pc, #16]	; 4d8 <init_data_section+0x1c>
     4c4:	e1510000 	cmp	r1, r0
     4c8:	012fff1e 	bxeq	lr
     4cc:	e59f2008 	ldr	r2, [pc, #8]	; 4dc <init_data_section+0x20>
     4d0:	ea0003ca 	b	1400 <memcpy>
     4d4:	000147e0 	.word	0x000147e0
     4d8:	0800d000 	.word	0x0800d000
     4dc:	000001a0 	.word	0x000001a0

000004e0 <initialization>:
     4e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     4e4:	e24dd004 	sub	sp, sp, #4	; 0x4
     4e8:	ebfffff3 	bl	4bc <init_data_section>
     4ec:	eb0001b7 	bl	bd0 <init_efuse>
     4f0:	eb000027 	bl	594 <init_xom>
     4f4:	eb0000e5 	bl	890 <init_lowlevel>
     4f8:	eb0004ea 	bl	18a8 <init_malloc>
     4fc:	eb002146 	bl	8a1c <init_timer>
     500:	eb000132 	bl	9d0 <init_gtc>
     504:	eb0020d7 	bl	8868 <init_serial>
     508:	eb001a04 	bl	6d20 <init_mmc>
     50c:	eb000098 	bl	774 <init_crypto>
     510:	eb00024f 	bl	e54 <init_alternate>
     514:	eb00001f 	bl	598 <init_boot_sequence>
     518:	e3a00000 	mov	r0, #0	; 0x0
     51c:	e28dd004 	add	sp, sp, #4	; 0x4
     520:	e8bd8000 	pop	{pc}

00000524 <main>:
     524:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     528:	e3a00000 	mov	r0, #0	; 0x0
     52c:	e24dd004 	sub	sp, sp, #4	; 0x4
     530:	eb0016c5 	bl	604c <cdbg_log_toggle>
     534:	ebffffe9 	bl	4e0 <initialization>
     538:	eb000117 	bl	99c <gtc_time>
     53c:	e3a00001 	mov	r0, #1	; 0x1
     540:	eb0016c1 	bl	604c <cdbg_log_toggle>
     544:	e59f001c 	ldr	r0, [pc, #28]	; 568 <main+0x44>
     548:	e59f301c 	ldr	r3, [pc, #28]	; 56c <main+0x48>
     54c:	e1a0e00f 	mov	lr, pc
     550:	e593f000 	ldr	pc, [r3]
     554:	e3a00000 	mov	r0, #0	; 0x0
     558:	eb0016bb 	bl	604c <cdbg_log_toggle>
     55c:	e3a00000 	mov	r0, #0	; 0x0
     560:	ebffffa0 	bl	3e8 <try_boot>
     564:	eafffffe 	b	564 <main+0x40>
     568:	00011adc 	.word	0x00011adc
     56c:	0800d764 	.word	0x0800d764

00000570 <get_xom>:
     570:	e59f3004 	ldr	r3, [pc, #4]	; 57c <get_xom+0xc>
     574:	e1d300b0 	ldrh	r0, [r3]
     578:	e12fff1e 	bx	lr
     57c:	0800d1a0 	.word	0x0800d1a0

00000580 <set_xom>:
     580:	e59f3008 	ldr	r3, [pc, #8]	; 590 <set_xom+0x10>
     584:	e200001f 	and	r0, r0, #31	; 0x1f
     588:	e1c300b0 	strh	r0, [r3]
     58c:	e12fff1e 	bx	lr
     590:	0800d1a0 	.word	0x0800d1a0

00000594 <init_xom>:
     594:	e12fff1e 	bx	lr

00000598 <init_boot_sequence>:
     598:	e12fff1e 	bx	lr

0000059c <boot_info>:
     59c:	e92d4010 	push	{r4, lr}
     5a0:	eb00011c 	bl	a18 <ecfg_paras>
     5a4:	e1a04000 	mov	r4, r0
     5a8:	e3a00b01 	mov	r0, #1024	; 0x400
     5ac:	eb000110 	bl	9f4 <ecfg_check_flag>
     5b0:	e59f3028 	ldr	r3, [pc, #40]	; 5e0 <boot_info+0x44>
     5b4:	e3500000 	cmp	r0, #0	; 0x0
     5b8:	11d330b0 	ldrhne	r3, [r3]
     5bc:	01d330b0 	ldrheq	r3, [r3]
     5c0:	1203301f 	andne	r3, r3, #31	; 0x1f
     5c4:	0203301f 	andeq	r3, r3, #31	; 0x1f
     5c8:	159f2014 	ldrne	r2, [pc, #20]	; 5e4 <boot_info+0x48>
     5cc:	11a03083 	lslne	r3, r3, #1
     5d0:	00843083 	addeq	r3, r4, r3, lsl #1
     5d4:	119200b3 	ldrhne	r0, [r2, r3]
     5d8:	01d302b0 	ldrheq	r0, [r3, #32]
     5dc:	e8bd8010 	pop	{r4, pc}
     5e0:	0800d1a0 	.word	0x0800d1a0
     5e4:	00011698 	.word	0x00011698

000005e8 <boot_device>:
     5e8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     5ec:	e24dd004 	sub	sp, sp, #4	; 0x4
     5f0:	ebffffe9 	bl	59c <boot_info>
     5f4:	e1a00620 	lsr	r0, r0, #12
     5f8:	e6ff0070 	uxth	r0, r0
     5fc:	e28dd004 	add	sp, sp, #4	; 0x4
     600:	e8bd8000 	pop	{pc}

00000604 <set_pll>:
     604:	e3500003 	cmp	r0, #3	; 0x3
     608:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
     60c:	e1a02001 	mov	r2, r1
     610:	88bd85f0 	pophi	{r4, r5, r6, r7, r8, sl, pc}
     614:	e3a03018 	mov	r3, #24	; 0x18
     618:	e0030093 	mul	r3, r3, r0
     61c:	e59f808c 	ldr	r8, [pc, #140]	; 6b0 <set_pll+0xac>
     620:	e59f008c 	ldr	r0, [pc, #140]	; 6b4 <set_pll+0xb0>
     624:	e59fc08c 	ldr	ip, [pc, #140]	; 6b8 <set_pll+0xb4>
     628:	e59f508c 	ldr	r5, [pc, #140]	; 6bc <set_pll+0xb8>
     62c:	e59fa08c 	ldr	sl, [pc, #140]	; 6c0 <set_pll+0xbc>
     630:	e283142d 	add	r1, r3, #754974720	; 0x2d000000
     634:	e0838008 	add	r8, r3, r8
     638:	e083c00c 	add	ip, r3, ip
     63c:	e3a07001 	mov	r7, #1	; 0x1
     640:	e2811a0a 	add	r1, r1, #40960	; 0xa000
     644:	e0830000 	add	r0, r3, r0
     648:	e1a06422 	lsr	r6, r2, #8
     64c:	e0835005 	add	r5, r3, r5
     650:	e3a04000 	mov	r4, #0	; 0x0
     654:	e6ef2072 	uxtb	r2, r2
     658:	e083a00a 	add	sl, r3, sl
     65c:	e3a03002 	mov	r3, #2	; 0x2
     660:	e5c87000 	strb	r7, [r8]
     664:	e5c12000 	strb	r2, [r1]
     668:	e5c06000 	strb	r6, [r0]
     66c:	e5cc4000 	strb	r4, [ip]
     670:	e5c53000 	strb	r3, [r5]
     674:	e0833007 	add	r3, r3, r7
     678:	e5c83000 	strb	r3, [r8]
     67c:	e1a02004 	mov	r2, r4
     680:	e5cc7000 	strb	r7, [ip]
     684:	e3011a0a 	movw	r1, #6666	; 0x1a0a
     688:	ea000001 	b	694 <set_pll+0x90>
     68c:	e1520001 	cmp	r2, r1
     690:	0a000003 	beq	6a4 <set_pll+0xa0>
     694:	e5da3000 	ldrb	r3, [sl]
     698:	e2822001 	add	r2, r2, #1	; 0x1
     69c:	e3130001 	tst	r3, #1	; 0x1
     6a0:	0afffff9 	beq	68c <set_pll+0x88>
     6a4:	e3a03002 	mov	r3, #2	; 0x2
     6a8:	e5883000 	str	r3, [r8]
     6ac:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
     6b0:	2d00a00c 	.word	0x2d00a00c
     6b4:	2d00a004 	.word	0x2d00a004
     6b8:	2d00a010 	.word	0x2d00a010
     6bc:	2d00a008 	.word	0x2d00a008
     6c0:	2d00a014 	.word	0x2d00a014

000006c4 <set_clk_divider>:
     6c4:	e3500005 	cmp	r0, #5	; 0x5
     6c8:	812fff1e 	bxhi	lr
     6cc:	e59fc020 	ldr	ip, [pc, #32]	; 6f4 <set_clk_divider+0x30>
     6d0:	e1a03280 	lsl	r3, r0, #5
     6d4:	e083c00c 	add	ip, r3, ip
     6d8:	e3a02000 	mov	r2, #0	; 0x0
     6dc:	e7d13002 	ldrb	r3, [r1, r2]
     6e0:	e2822001 	add	r2, r2, #1	; 0x1
     6e4:	e3520008 	cmp	r2, #8	; 0x8
     6e8:	e4cc3004 	strb	r3, [ip], #4
     6ec:	1afffffa 	bne	6dc <set_clk_divider+0x18>
     6f0:	e12fff1e 	bx	lr
     6f4:	2d00a070 	.word	0x2d00a070

000006f8 <module_set_clock>:
     6f8:	e59f3020 	ldr	r3, [pc, #32]	; 720 <module_set_clock+0x28>
     6fc:	e1500003 	cmp	r0, r3
     700:	13a03001 	movne	r3, #1	; 0x1
     704:	1580300c 	strne	r3, [r0, #12]
     708:	e2013007 	and	r3, r1, #7	; 0x7
     70c:	e5803000 	str	r3, [r0]
     710:	e3a03000 	mov	r3, #0	; 0x0
     714:	e5802004 	str	r2, [r0, #4]
     718:	e580300c 	str	r3, [r0, #12]
     71c:	e12fff1e 	bx	lr
     720:	2d00a360 	.word	0x2d00a360

00000724 <module_enable>:
     724:	e3a020ff 	mov	r2, #255	; 0xff
     728:	e3a01000 	mov	r1, #0	; 0x0
     72c:	e3a03011 	mov	r3, #17	; 0x11
     730:	e580100c 	str	r1, [r0, #12]
     734:	e5802000 	str	r2, [r0]
     738:	e5802004 	str	r2, [r0, #4]
     73c:	e5803008 	str	r3, [r0, #8]
     740:	e5801014 	str	r1, [r0, #20]
     744:	e5903008 	ldr	r3, [r0, #8]
     748:	e3c33010 	bic	r3, r3, #16	; 0x10
     74c:	e5803008 	str	r3, [r0, #8]
     750:	e5802018 	str	r2, [r0, #24]
     754:	e5801000 	str	r1, [r0]
     758:	e580200c 	str	r2, [r0, #12]
     75c:	e12fff1e 	bx	lr

00000760 <init_igps_ram>:
     760:	e59f3008 	ldr	r3, [pc, #8]	; 770 <init_igps_ram+0x10>
     764:	e3a02001 	mov	r2, #1	; 0x1
     768:	e5832410 	str	r2, [r3, #1040]
     76c:	e12fff1e 	bx	lr
     770:	2d00c000 	.word	0x2d00c000

00000774 <init_crypto>:
     774:	e12fff1e 	bx	lr

00000778 <get_reset_stats>:
     778:	e59f2028 	ldr	r2, [pc, #40]	; 7a8 <get_reset_stats+0x30>
     77c:	e5923c10 	ldr	r3, [r2, #3088]
     780:	e2130002 	ands	r0, r3, #2	; 0x2
     784:	012fff1e 	bxeq	lr
     788:	e5920c2c 	ldr	r0, [r2, #3116]
     78c:	e3100001 	tst	r0, #1	; 0x1
     790:	03a00001 	moveq	r0, #1	; 0x1
     794:	012fff1e 	bxeq	lr
     798:	e3500037 	cmp	r0, #55	; 0x37
     79c:	03a00002 	moveq	r0, #2	; 0x2
     7a0:	13a00003 	movne	r0, #3	; 0x3
     7a4:	e12fff1e 	bx	lr
     7a8:	2d009000 	.word	0x2d009000

000007ac <update_mask_on_reset>:
     7ac:	e59f3018 	ldr	r3, [pc, #24]	; 7cc <update_mask_on_reset+0x20>
     7b0:	e5932c30 	ldr	r2, [r3, #3120]
     7b4:	e3120080 	tst	r2, #128	; 0x80
     7b8:	13a03001 	movne	r3, #1	; 0x1
     7bc:	1202200f 	andne	r2, r2, #15	; 0xf
     7c0:	11e03213 	mvnne	r3, r3, lsl r2
     7c4:	15803000 	strne	r3, [r0]
     7c8:	e12fff1e 	bx	lr
     7cc:	2d009000 	.word	0x2d009000

000007d0 <get_wakeup_entry>:
     7d0:	e59f301c 	ldr	r3, [pc, #28]	; 7f4 <get_wakeup_entry+0x24>
     7d4:	e5931c38 	ldr	r1, [r3, #3128]
     7d8:	e5932c34 	ldr	r2, [r3, #3124]
     7dc:	e5930c30 	ldr	r0, [r3, #3120]
     7e0:	e1822401 	orr	r2, r2, r1, lsl #8
     7e4:	e5933c2c 	ldr	r3, [r3, #3116]
     7e8:	e1800402 	orr	r0, r0, r2, lsl #8
     7ec:	e1830400 	orr	r0, r3, r0, lsl #8
     7f0:	e12fff1e 	bx	lr
     7f4:	2d009000 	.word	0x2d009000

000007f8 <system_reset>:
     7f8:	e59f3010 	ldr	r3, [pc, #16]	; 810 <system_reset+0x18>
     7fc:	e3a02001 	mov	r2, #1	; 0x1
     800:	e5832c2c 	str	r2, [r3, #3116]
     804:	e2822002 	add	r2, r2, #2	; 0x2
     808:	e5832c00 	str	r2, [r3, #3072]
     80c:	e12fff1e 	bx	lr
     810:	2d009000 	.word	0x2d009000

00000814 <pads_init_module>:
     814:	e92d4030 	push	{r4, r5, lr}
     818:	e1a05000 	mov	r5, r0
     81c:	e24dd004 	sub	sp, sp, #4	; 0x4
     820:	e59f4064 	ldr	r4, [pc, #100]	; 88c <pads_init_module+0x78>
     824:	ea000013 	b	878 <pads_init_module+0x64>
     828:	e1500005 	cmp	r0, r5
     82c:	1a000010 	bne	874 <pads_init_module+0x60>
     830:	e3a01000 	mov	r1, #0	; 0x0
     834:	e1a02001 	mov	r2, r1
     838:	e514000c 	ldr	r0, [r4, #-12]
     83c:	eb000100 	bl	c44 <pads_chmod>
     840:	e5140004 	ldr	r0, [r4, #-4]
     844:	e3a01000 	mov	r1, #0	; 0x0
     848:	e1500001 	cmp	r0, r1
     84c:	e1a02001 	mov	r2, r1
     850:	da000000 	ble	858 <pads_init_module+0x44>
     854:	eb0000fa 	bl	c44 <pads_chmod>
     858:	e5943000 	ldr	r3, [r4]
     85c:	e3a01000 	mov	r1, #0	; 0x0
     860:	e1530001 	cmp	r3, r1
     864:	e1a02001 	mov	r2, r1
     868:	e1a00003 	mov	r0, r3
     86c:	da000000 	ble	874 <pads_init_module+0x60>
     870:	eb0000f3 	bl	c44 <pads_chmod>
     874:	e2844014 	add	r4, r4, #20	; 0x14
     878:	e5140010 	ldr	r0, [r4, #-16]
     87c:	e3500000 	cmp	r0, #0	; 0x0
     880:	1affffe8 	bne	828 <pads_init_module+0x14>
     884:	e28dd004 	add	sp, sp, #4	; 0x4
     888:	e8bd8030 	pop	{r4, r5, pc}
     88c:	0800d010 	.word	0x0800d010

00000890 <init_lowlevel>:
     890:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     894:	e24dd044 	sub	sp, sp, #68	; 0x44
     898:	e3a01000 	mov	r1, #0	; 0x0
     89c:	e3a02040 	mov	r2, #64	; 0x40
     8a0:	e1a0000d 	mov	r0, sp
     8a4:	eb0002cd 	bl	13e0 <memset>
     8a8:	e3a03063 	mov	r3, #99	; 0x63
     8ac:	e5cd3010 	strb	r3, [sp, #16]
     8b0:	e3e03073 	mvn	r3, #115	; 0x73
     8b4:	e5cd3011 	strb	r3, [sp, #17]
     8b8:	e28330a5 	add	r3, r3, #165	; 0xa5
     8bc:	e5cd3012 	strb	r3, [sp, #18]
     8c0:	e3a0301f 	mov	r3, #31	; 0x1f
     8c4:	e5cd3018 	strb	r3, [sp, #24]
     8c8:	e283305d 	add	r3, r3, #93	; 0x5d
     8cc:	e5cd3019 	strb	r3, [sp, #25]
     8d0:	e3e0300f 	mvn	r3, #15	; 0xf
     8d4:	e5cd301a 	strb	r3, [sp, #26]
     8d8:	e2833020 	add	r3, r3, #32	; 0x20
     8dc:	e5cd3020 	strb	r3, [sp, #32]
     8e0:	e2833032 	add	r3, r3, #50	; 0x32
     8e4:	e5cd3021 	strb	r3, [sp, #33]
     8e8:	e3a03002 	mov	r3, #2	; 0x2
     8ec:	e5cd3029 	strb	r3, [sp, #41]
     8f0:	e3e0301f 	mvn	r3, #31	; 0x1f
     8f4:	e5cd3030 	strb	r3, [sp, #48]
     8f8:	e3e0307c 	mvn	r3, #124	; 0x7c
     8fc:	e5cd3031 	strb	r3, [sp, #49]
     900:	e283308c 	add	r3, r3, #140	; 0x8c
     904:	e3a02008 	mov	r2, #8	; 0x8
     908:	e5cd3032 	strb	r3, [sp, #50]
     90c:	e3a03001 	mov	r3, #1	; 0x1
     910:	e5cd202a 	strb	r2, [sp, #42]
     914:	e58d3038 	str	r3, [sp, #56]
     918:	e5cd2022 	strb	r2, [sp, #34]
     91c:	e3a01000 	mov	r1, #0	; 0x0
     920:	e3e00000 	mvn	r0, #0	; 0x0
     924:	e59f3048 	ldr	r3, [pc, #72]	; 974 <init_lowlevel+0xe4>
     928:	e1a02101 	lsl	r2, r1, #2
     92c:	e2811001 	add	r1, r1, #1	; 0x1
     930:	e0823003 	add	r3, r2, r3
     934:	e3510008 	cmp	r1, #8	; 0x8
     938:	e5830000 	str	r0, [r3]
     93c:	1afffff8 	bne	924 <init_lowlevel+0x94>
     940:	e28d0040 	add	r0, sp, #64	; 0x40
     944:	e301302a 	movw	r3, #4138	; 0x102a
     948:	e16034b0 	strh	r3, [r0, #-64]!
     94c:	e2833018 	add	r3, r3, #24	; 0x18
     950:	e1cd30b6 	strh	r3, [sp, #6]
     954:	e2433011 	sub	r3, r3, #17	; 0x11
     958:	e1cd30b4 	strh	r3, [sp, #4]
     95c:	e1a0000d 	mov	r0, sp
     960:	e302304f 	movw	r3, #8271	; 0x204f
     964:	e1cd30b2 	strh	r3, [sp, #2]
     968:	eb000171 	bl	f34 <set_clk>
     96c:	e28dd044 	add	sp, sp, #68	; 0x44
     970:	e8bd8000 	pop	{pc}
     974:	21b01180 	.word	0x21b01180

00000978 <module_reset>:
     978:	e3a030ff 	mov	r3, #255	; 0xff
     97c:	e5803000 	str	r3, [r0]
     980:	e92d4010 	push	{r4, lr}
     984:	e1a04000 	mov	r4, r0
     988:	e3a0000a 	mov	r0, #10	; 0xa
     98c:	eb001fde 	bl	890c <udelay>
     990:	e3a03000 	mov	r3, #0	; 0x0
     994:	e5843000 	str	r3, [r4]
     998:	e8bd8010 	pop	{r4, pc}

0000099c <gtc_time>:
     99c:	e59f3020 	ldr	r3, [pc, #32]	; 9c4 <gtc_time+0x28>
     9a0:	e92d4010 	push	{r4, lr}
     9a4:	e5930000 	ldr	r0, [r3]
     9a8:	e59f3018 	ldr	r3, [pc, #24]	; 9c8 <gtc_time+0x2c>
     9ac:	e5934000 	ldr	r4, [r3]
     9b0:	eb001ff9 	bl	899c <get_timer>
     9b4:	e1a01000 	mov	r1, r0
     9b8:	e59f000c 	ldr	r0, [pc, #12]	; 9cc <gtc_time+0x30>
     9bc:	e12fff34 	blx	r4
     9c0:	e8bd8010 	pop	{r4, pc}
     9c4:	0800d1a4 	.word	0x0800d1a4
     9c8:	0800d764 	.word	0x0800d764
     9cc:	00011ae1 	.word	0x00011ae1

000009d0 <init_gtc>:
     9d0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     9d4:	e3a00000 	mov	r0, #0	; 0x0
     9d8:	e24dd004 	sub	sp, sp, #4	; 0x4
     9dc:	eb001fee 	bl	899c <get_timer>
     9e0:	e59f3008 	ldr	r3, [pc, #8]	; 9f0 <init_gtc+0x20>
     9e4:	e5830000 	str	r0, [r3]
     9e8:	e28dd004 	add	sp, sp, #4	; 0x4
     9ec:	e8bd8000 	pop	{pc}
     9f0:	0800d1a4 	.word	0x0800d1a4

000009f4 <ecfg_check_flag>:
     9f4:	e3500801 	cmp	r0, #65536	; 0x10000
     9f8:	0a000004 	beq	a10 <ecfg_check_flag+0x1c>
     9fc:	e3500101 	cmp	r0, #1073741824	; 0x40000000
     a00:	0a000002 	beq	a10 <ecfg_check_flag+0x1c>
     a04:	e3500001 	cmp	r0, #1	; 0x1
     a08:	13a00001 	movne	r0, #1	; 0x1
     a0c:	112fff1e 	bxne	lr
     a10:	e3a00000 	mov	r0, #0	; 0x0
     a14:	e12fff1e 	bx	lr

00000a18 <ecfg_paras>:
     a18:	e59f0000 	ldr	r0, [pc, #0]	; a20 <ecfg_paras+0x8>
     a1c:	e12fff1e 	bx	lr
     a20:	0800d768 	.word	0x0800d768

00000a24 <efuse_mac>:
     a24:	e3a00000 	mov	r0, #0	; 0x0
     a28:	e3a01000 	mov	r1, #0	; 0x0
     a2c:	e12fff1e 	bx	lr

00000a30 <efuse_info>:
     a30:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     a34:	e1a04000 	mov	r4, r0
     a38:	e24dd004 	sub	sp, sp, #4	; 0x4
     a3c:	e59f6144 	ldr	r6, [pc, #324]	; b88 <efuse_info+0x158>
     a40:	e59f0144 	ldr	r0, [pc, #324]	; b8c <efuse_info+0x15c>
     a44:	e1a0e00f 	mov	lr, pc
     a48:	e596f000 	ldr	pc, [r6]
     a4c:	e59f013c 	ldr	r0, [pc, #316]	; b90 <efuse_info+0x160>
     a50:	e1a0e00f 	mov	lr, pc
     a54:	e596f000 	ldr	pc, [r6]
     a58:	e3540000 	cmp	r4, #0	; 0x0
     a5c:	0a00003c 	beq	b54 <efuse_info+0x124>
     a60:	e59f412c 	ldr	r4, [pc, #300]	; b94 <efuse_info+0x164>
     a64:	e59f012c 	ldr	r0, [pc, #300]	; b98 <efuse_info+0x168>
     a68:	e5941000 	ldr	r1, [r4]
     a6c:	e1a0e00f 	mov	lr, pc
     a70:	e596f000 	ldr	pc, [r6]
     a74:	e5d41004 	ldrb	r1, [r4, #4]
     a78:	e59f011c 	ldr	r0, [pc, #284]	; b9c <efuse_info+0x16c>
     a7c:	e1a0e00f 	mov	lr, pc
     a80:	e596f000 	ldr	pc, [r6]
     a84:	e5d41005 	ldrb	r1, [r4, #5]
     a88:	e59f0110 	ldr	r0, [pc, #272]	; ba0 <efuse_info+0x170>
     a8c:	e1a0e00f 	mov	lr, pc
     a90:	e596f000 	ldr	pc, [r6]
     a94:	e5d41006 	ldrb	r1, [r4, #6]
     a98:	e59f0104 	ldr	r0, [pc, #260]	; ba4 <efuse_info+0x174>
     a9c:	e1a0e00f 	mov	lr, pc
     aa0:	e596f000 	ldr	pc, [r6]
     aa4:	e5d41007 	ldrb	r1, [r4, #7]
     aa8:	e59f00f8 	ldr	r0, [pc, #248]	; ba8 <efuse_info+0x178>
     aac:	e1a0e00f 	mov	lr, pc
     ab0:	e596f000 	ldr	pc, [r6]
     ab4:	e1d410b8 	ldrh	r1, [r4, #8]
     ab8:	e59f00ec 	ldr	r0, [pc, #236]	; bac <efuse_info+0x17c>
     abc:	e1a0e00f 	mov	lr, pc
     ac0:	e596f000 	ldr	pc, [r6]
     ac4:	e1d410ba 	ldrh	r1, [r4, #10]
     ac8:	e59f00e0 	ldr	r0, [pc, #224]	; bb0 <efuse_info+0x180>
     acc:	e1a0e00f 	mov	lr, pc
     ad0:	e596f000 	ldr	pc, [r6]
     ad4:	e1d410bc 	ldrh	r1, [r4, #12]
     ad8:	e59f00d4 	ldr	r0, [pc, #212]	; bb4 <efuse_info+0x184>
     adc:	e1a0e00f 	mov	lr, pc
     ae0:	e596f000 	ldr	pc, [r6]
     ae4:	e1d410be 	ldrh	r1, [r4, #14]
     ae8:	e59f00c8 	ldr	r0, [pc, #200]	; bb8 <efuse_info+0x188>
     aec:	e1a0e00f 	mov	lr, pc
     af0:	e596f000 	ldr	pc, [r6]
     af4:	e1a05004 	mov	r5, r4
     af8:	e3a04000 	mov	r4, #0	; 0x0
     afc:	e1a01004 	mov	r1, r4
     b00:	e5952010 	ldr	r2, [r5, #16]
     b04:	e2844001 	add	r4, r4, #1	; 0x1
     b08:	e59f00ac 	ldr	r0, [pc, #172]	; bbc <efuse_info+0x18c>
     b0c:	e1a0e00f 	mov	lr, pc
     b10:	e596f000 	ldr	pc, [r6]
     b14:	e3540004 	cmp	r4, #4	; 0x4
     b18:	e2855004 	add	r5, r5, #4	; 0x4
     b1c:	1afffff6 	bne	afc <efuse_info+0xcc>
     b20:	e59f6060 	ldr	r6, [pc, #96]	; b88 <efuse_info+0x158>
     b24:	e59f5068 	ldr	r5, [pc, #104]	; b94 <efuse_info+0x164>
     b28:	e284400c 	add	r4, r4, #12	; 0xc
     b2c:	e1a03084 	lsl	r3, r4, #1
     b30:	e1a01004 	mov	r1, r4
     b34:	e19520b3 	ldrh	r2, [r5, r3]
     b38:	e2844001 	add	r4, r4, #1	; 0x1
     b3c:	e59f007c 	ldr	r0, [pc, #124]	; bc0 <efuse_info+0x190>
     b40:	e1a0e00f 	mov	lr, pc
     b44:	e596f000 	ldr	pc, [r6]
     b48:	e354001c 	cmp	r4, #28	; 0x1c
     b4c:	1afffff6 	bne	b2c <efuse_info+0xfc>
     b50:	ea00000a 	b	b80 <efuse_info+0x150>
     b54:	e59f4068 	ldr	r4, [pc, #104]	; bc4 <efuse_info+0x194>
     b58:	e59f7068 	ldr	r7, [pc, #104]	; bc8 <efuse_info+0x198>
     b5c:	e1a05006 	mov	r5, r6
     b60:	e5942000 	ldr	r2, [r4]
     b64:	e1a01004 	mov	r1, r4
     b68:	e59f005c 	ldr	r0, [pc, #92]	; bcc <efuse_info+0x19c>
     b6c:	e2844004 	add	r4, r4, #4	; 0x4
     b70:	e1a0e00f 	mov	lr, pc
     b74:	e595f000 	ldr	pc, [r5]
     b78:	e1540007 	cmp	r4, r7
     b7c:	1afffff7 	bne	b60 <efuse_info+0x130>
     b80:	e28dd004 	add	sp, sp, #4	; 0x4
     b84:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     b88:	0800d764 	.word	0x0800d764
     b8c:	00011ae9 	.word	0x00011ae9
     b90:	00011afe 	.word	0x00011afe
     b94:	0800d768 	.word	0x0800d768
     b98:	00011b15 	.word	0x00011b15
     b9c:	00011b24 	.word	0x00011b24
     ba0:	00011b33 	.word	0x00011b33
     ba4:	00011b46 	.word	0x00011b46
     ba8:	00011b55 	.word	0x00011b55
     bac:	00011b6a 	.word	0x00011b6a
     bb0:	00011b77 	.word	0x00011b77
     bb4:	00011b87 	.word	0x00011b87
     bb8:	00011b98 	.word	0x00011b98
     bbc:	00011baa 	.word	0x00011baa
     bc0:	00011bba 	.word	0x00011bba
     bc4:	2d00d9b4 	.word	0x2d00d9b4
     bc8:	2d00da94 	.word	0x2d00da94
     bcc:	00011bcb 	.word	0x00011bcb

00000bd0 <init_efuse>:
     bd0:	e59f0008 	ldr	r0, [pc, #8]	; be0 <init_efuse+0x10>
     bd4:	e3a01000 	mov	r1, #0	; 0x0
     bd8:	e3a02038 	mov	r2, #56	; 0x38
     bdc:	ea0001ff 	b	13e0 <memset>
     be0:	0800d768 	.word	0x0800d768

00000be4 <assert_linkpc>:
     be4:	e3a00001 	mov	r0, #1	; 0x1
     be8:	e12fff1e 	bx	lr

00000bec <pads_states>:
     bec:	e3500000 	cmp	r0, #0	; 0x0
     bf0:	e2803007 	add	r3, r0, #7	; 0x7
     bf4:	a1a03000 	movge	r3, r0
     bf8:	e1a031c3 	asr	r3, r3, #3
     bfc:	e59f203c 	ldr	r2, [pc, #60]	; c40 <pads_states+0x54>
     c00:	e1a03103 	lsl	r3, r3, #2
     c04:	e0832002 	add	r2, r3, r2
     c08:	e5921000 	ldr	r1, [r2]
     c0c:	e3a03001 	mov	r3, #1	; 0x1
     c10:	e2002007 	and	r2, r0, #7	; 0x7
     c14:	e0113213 	ands	r3, r1, r3, lsl r2
     c18:	01a00003 	moveq	r0, r3
     c1c:	012fff1e 	bxeq	lr
     c20:	e1a03300 	lsl	r3, r0, #6
     c24:	e2833282 	add	r3, r3, #536870920	; 0x20000008
     c28:	e283360f 	add	r3, r3, #15728640	; 0xf00000
     c2c:	e5933000 	ldr	r3, [r3]
     c30:	e3530000 	cmp	r3, #0	; 0x0
     c34:	13a00002 	movne	r0, #2	; 0x2
     c38:	03a00001 	moveq	r0, #1	; 0x1
     c3c:	e12fff1e 	bx	lr
     c40:	2d009004 	.word	0x2d009004

00000c44 <pads_chmod>:
     c44:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     c48:	e1b07840 	asrs	r7, r0, #16
     c4c:	e1a00800 	lsl	r0, r0, #16
     c50:	03a03000 	moveq	r3, #0	; 0x0
     c54:	13a03001 	movne	r3, #1	; 0x1
     c58:	e1a00820 	lsr	r0, r0, #16
     c5c:	e1570000 	cmp	r7, r0
     c60:	a3a0c000 	movge	ip, #0	; 0x0
     c64:	b203c001 	andlt	ip, r3, #1	; 0x1
     c68:	e35c0000 	cmp	ip, #0	; 0x0
     c6c:	e24dd004 	sub	sp, sp, #4	; 0x4
     c70:	e1a05001 	mov	r5, r1
     c74:	e1a09002 	mov	r9, r2
     c78:	1a000038 	bne	d60 <pads_chmod+0x11c>
     c7c:	e2803883 	add	r3, r0, #8585216	; 0x830000
     c80:	e2833903 	add	r3, r3, #49152	; 0xc000
     c84:	e1a02303 	lsl	r2, r3, #6
     c88:	e3570000 	cmp	r7, #0	; 0x0
     c8c:	01a07000 	moveq	r7, r0
     c90:	e2826008 	add	r6, r2, #8	; 0x8
     c94:	e2828004 	add	r8, r2, #4	; 0x4
     c98:	e1a04000 	mov	r4, r0
     c9c:	e3a0a001 	mov	sl, #1	; 0x1
     ca0:	e59fb0c0 	ldr	fp, [pc, #192]	; d68 <pads_chmod+0x124>
     ca4:	ea00002a 	b	d54 <pads_chmod+0x110>
     ca8:	e3540000 	cmp	r4, #0	; 0x0
     cac:	a1a03004 	movge	r3, r4
     cb0:	e3550001 	cmp	r5, #1	; 0x1
     cb4:	e1a011c3 	asr	r1, r3, #3
     cb8:	e204c007 	and	ip, r4, #7	; 0x7
     cbc:	0a00000b 	beq	cf0 <pads_chmod+0xac>
     cc0:	e3550002 	cmp	r5, #2	; 0x2
     cc4:	0a000012 	beq	d14 <pads_chmod+0xd0>
     cc8:	e59f209c 	ldr	r2, [pc, #156]	; d6c <pads_chmod+0x128>
     ccc:	e1a03101 	lsl	r3, r1, #2
     cd0:	e2551000 	subs	r1, r5, #0	; 0x0
     cd4:	e0832002 	add	r2, r3, r2
     cd8:	e59f0090 	ldr	r0, [pc, #144]	; d70 <pads_chmod+0x12c>
     cdc:	1a000017 	bne	d40 <pads_chmod+0xfc>
     ce0:	e5923000 	ldr	r3, [r2]
     ce4:	e1c33c1a 	bic	r3, r3, sl, lsl ip
     ce8:	e5823000 	str	r3, [r2]
     cec:	ea000015 	b	d48 <pads_chmod+0x104>
     cf0:	e59f3074 	ldr	r3, [pc, #116]	; d6c <pads_chmod+0x128>
     cf4:	e1a02101 	lsl	r2, r1, #2
     cf8:	e0823003 	add	r3, r2, r3
     cfc:	e5932000 	ldr	r2, [r3]
     d00:	e1822c15 	orr	r2, r2, r5, lsl ip
     d04:	e5832000 	str	r2, [r3]
     d08:	e3a03000 	mov	r3, #0	; 0x0
     d0c:	e5863000 	str	r3, [r6]
     d10:	ea00000c 	b	d48 <pads_chmod+0x104>
     d14:	e59f2050 	ldr	r2, [pc, #80]	; d6c <pads_chmod+0x128>
     d18:	e1a03101 	lsl	r3, r1, #2
     d1c:	e0832002 	add	r2, r3, r2
     d20:	e5923000 	ldr	r3, [r2]
     d24:	e2591000 	subs	r1, r9, #0	; 0x0
     d28:	13a01001 	movne	r1, #1	; 0x1
     d2c:	e1833c1a 	orr	r3, r3, sl, lsl ip
     d30:	e5823000 	str	r3, [r2]
     d34:	e586a000 	str	sl, [r6]
     d38:	e5881000 	str	r1, [r8]
     d3c:	ea000001 	b	d48 <pads_chmod+0x104>
     d40:	e1a0e00f 	mov	lr, pc
     d44:	e59bf000 	ldr	pc, [fp]
     d48:	e2844001 	add	r4, r4, #1	; 0x1
     d4c:	e2866040 	add	r6, r6, #64	; 0x40
     d50:	e2888040 	add	r8, r8, #64	; 0x40
     d54:	e1540007 	cmp	r4, r7
     d58:	e2843007 	add	r3, r4, #7	; 0x7
     d5c:	daffffd1 	ble	ca8 <pads_chmod+0x64>
     d60:	e28dd004 	add	sp, sp, #4	; 0x4
     d64:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     d68:	0800d764 	.word	0x0800d764
     d6c:	2d009004 	.word	0x2d009004
     d70:	00011bdb 	.word	0x00011bdb

00000d74 <pads_pull>:
     d74:	e1b02840 	asrs	r2, r0, #16
     d78:	e1a00800 	lsl	r0, r0, #16
     d7c:	03a03000 	moveq	r3, #0	; 0x0
     d80:	13a03001 	movne	r3, #1	; 0x1
     d84:	e1a00820 	lsr	r0, r0, #16
     d88:	e1520000 	cmp	r2, r0
     d8c:	a3a03000 	movge	r3, #0	; 0x0
     d90:	b2033001 	andlt	r3, r3, #1	; 0x1
     d94:	e3530000 	cmp	r3, #0	; 0x0
     d98:	112fff1e 	bxne	lr
     d9c:	e1a03300 	lsl	r3, r0, #6
     da0:	e28332c2 	add	r3, r3, #536870924	; 0x2000000c
     da4:	e3520000 	cmp	r2, #0	; 0x0
     da8:	01a02000 	moveq	r2, r0
     dac:	e283360f 	add	r3, r3, #15728640	; 0xf00000
     db0:	e3a0c001 	mov	ip, #1	; 0x1
     db4:	ea000004 	b	dcc <pads_pull+0x58>
     db8:	e3510000 	cmp	r1, #0	; 0x0
     dbc:	05831000 	streq	r1, [r3]
     dc0:	1583c000 	strne	ip, [r3]
     dc4:	e2800001 	add	r0, r0, #1	; 0x1
     dc8:	e2833040 	add	r3, r3, #64	; 0x40
     dcc:	e1500002 	cmp	r0, r2
     dd0:	dafffff8 	ble	db8 <pads_pull+0x44>
     dd4:	e12fff1e 	bx	lr

00000dd8 <pads_setpin>:
     dd8:	e1b0c840 	asrs	ip, r0, #16
     ddc:	e1a00800 	lsl	r0, r0, #16
     de0:	03a03000 	moveq	r3, #0	; 0x0
     de4:	13a03001 	movne	r3, #1	; 0x1
     de8:	e1a00820 	lsr	r0, r0, #16
     dec:	e15c0000 	cmp	ip, r0
     df0:	a3a03000 	movge	r3, #0	; 0x0
     df4:	b2033001 	andlt	r3, r3, #1	; 0x1
     df8:	e3530000 	cmp	r3, #0	; 0x0
     dfc:	112fff1e 	bxne	lr
     e00:	e1a03300 	lsl	r3, r0, #6
     e04:	e2832242 	add	r2, r3, #536870916	; 0x20000004
     e08:	e35c0000 	cmp	ip, #0	; 0x0
     e0c:	01a0c000 	moveq	ip, r0
     e10:	e282260f 	add	r2, r2, #15728640	; 0xf00000
     e14:	ea000002 	b	e24 <pads_setpin+0x4c>
     e18:	e2513000 	subs	r3, r1, #0	; 0x0
     e1c:	13a03001 	movne	r3, #1	; 0x1
     e20:	e4823040 	str	r3, [r2], #64
     e24:	e150000c 	cmp	r0, ip
     e28:	e2800001 	add	r0, r0, #1	; 0x1
     e2c:	dafffff9 	ble	e18 <pads_setpin+0x40>
     e30:	e12fff1e 	bx	lr

00000e34 <pads_getpin>:
     e34:	e59f3014 	ldr	r3, [pc, #20]	; e50 <pads_getpin+0x1c>
     e38:	e6f30070 	uxtah	r0, r3, r0
     e3c:	e1a00300 	lsl	r0, r0, #6
     e40:	e5900000 	ldr	r0, [r0]
     e44:	e2500000 	subs	r0, r0, #0	; 0x0
     e48:	13a00001 	movne	r0, #1	; 0x1
     e4c:	e12fff1e 	bx	lr
     e50:	0083c000 	.word	0x0083c000

00000e54 <init_alternate>:
     e54:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     e58:	e59f0010 	ldr	r0, [pc, #16]	; e70 <init_alternate+0x1c>
     e5c:	e24dd004 	sub	sp, sp, #4	; 0x4
     e60:	ebfffe2f 	bl	724 <module_enable>
     e64:	e3a00000 	mov	r0, #0	; 0x0
     e68:	e28dd004 	add	sp, sp, #4	; 0x4
     e6c:	e8bd8000 	pop	{pc}
     e70:	2d013c00 	.word	0x2d013c00

00000e74 <cpu_recase>:
     e74:	e92d4010 	push	{r4, lr}
     e78:	e59f1098 	ldr	r1, [pc, #152]	; f18 <cpu_recase+0xa4>
     e7c:	e3a02004 	mov	r2, #4	; 0x4
     e80:	e1a04000 	mov	r4, r0
     e84:	eb000124 	bl	131c <strncmp>
     e88:	e3500000 	cmp	r0, #0	; 0x0
     e8c:	1a000002 	bne	e9c <cpu_recase+0x28>
     e90:	eb0000e5 	bl	122c <get_mux>
     e94:	e3c00004 	bic	r0, r0, #4	; 0x4
     e98:	ea00000d 	b	ed4 <cpu_recase+0x60>
     e9c:	e1a00004 	mov	r0, r4
     ea0:	e59f1074 	ldr	r1, [pc, #116]	; f1c <cpu_recase+0xa8>
     ea4:	e3a02005 	mov	r2, #5	; 0x5
     ea8:	eb00011b 	bl	131c <strncmp>
     eac:	e3500000 	cmp	r0, #0	; 0x0
     eb0:	0a000005 	beq	ecc <cpu_recase+0x58>
     eb4:	e1a00004 	mov	r0, r4
     eb8:	e59f1060 	ldr	r1, [pc, #96]	; f20 <cpu_recase+0xac>
     ebc:	e3a02004 	mov	r2, #4	; 0x4
     ec0:	eb000115 	bl	131c <strncmp>
     ec4:	e3500000 	cmp	r0, #0	; 0x0
     ec8:	18bd8010 	popne	{r4, pc}
     ecc:	eb0000d6 	bl	122c <get_mux>
     ed0:	e3800004 	orr	r0, r0, #4	; 0x4
     ed4:	e59f3048 	ldr	r3, [pc, #72]	; f24 <cpu_recase+0xb0>
     ed8:	e59f4044 	ldr	r4, [pc, #68]	; f24 <cpu_recase+0xb0>
     edc:	e59f1044 	ldr	r1, [pc, #68]	; f28 <cpu_recase+0xb4>
     ee0:	e5830000 	str	r0, [r3]
     ee4:	e5942000 	ldr	r2, [r4]
     ee8:	e5913000 	ldr	r3, [r1]
     eec:	e1520003 	cmp	r2, r3
     ef0:	08bd8010 	popeq	{r4, pc}
     ef4:	e1a00002 	mov	r0, r2
     ef8:	e5812000 	str	r2, [r1]
     efc:	eb0000c6 	bl	121c <set_mux>
     f00:	e5941000 	ldr	r1, [r4]
     f04:	e59f0020 	ldr	r0, [pc, #32]	; f2c <cpu_recase+0xb8>
     f08:	e59f3020 	ldr	r3, [pc, #32]	; f30 <cpu_recase+0xbc>
     f0c:	e1a0e00f 	mov	lr, pc
     f10:	e593f000 	ldr	pc, [r3]
     f14:	e8bd8010 	pop	{r4, pc}
     f18:	00011bf2 	.word	0x00011bf2
     f1c:	00011bf7 	.word	0x00011bf7
     f20:	00011a82 	.word	0x00011a82
     f24:	0800d1a8 	.word	0x0800d1a8
     f28:	0800d104 	.word	0x0800d104
     f2c:	00011bfd 	.word	0x00011bfd
     f30:	0800d764 	.word	0x0800d764

00000f34 <set_clk>:
     f34:	e92d4030 	push	{r4, r5, lr}
     f38:	e1d010b2 	ldrh	r1, [r0, #2]
     f3c:	e24dd004 	sub	sp, sp, #4	; 0x4
     f40:	e3510000 	cmp	r1, #0	; 0x0
     f44:	e1a04000 	mov	r4, r0
     f48:	0a000001 	beq	f54 <set_clk+0x20>
     f4c:	e3a00001 	mov	r0, #1	; 0x1
     f50:	ebfffdab 	bl	604 <set_pll>
     f54:	e1d410b4 	ldrh	r1, [r4, #4]
     f58:	e3510000 	cmp	r1, #0	; 0x0
     f5c:	0a000001 	beq	f68 <set_clk+0x34>
     f60:	e3a00002 	mov	r0, #2	; 0x2
     f64:	ebfffda6 	bl	604 <set_pll>
     f68:	e1d410b6 	ldrh	r1, [r4, #6]
     f6c:	e3510000 	cmp	r1, #0	; 0x0
     f70:	0a000001 	beq	f7c <set_clk+0x48>
     f74:	e3a00003 	mov	r0, #3	; 0x3
     f78:	ebfffda1 	bl	604 <set_pll>
     f7c:	e59f0094 	ldr	r0, [pc, #148]	; 1018 <set_clk+0xe4>
     f80:	e3a01001 	mov	r1, #1	; 0x1
     f84:	e3a0200b 	mov	r2, #11	; 0xb
     f88:	ebfffdda 	bl	6f8 <module_set_clock>
     f8c:	e59f0088 	ldr	r0, [pc, #136]	; 101c <set_clk+0xe8>
     f90:	e3a01001 	mov	r1, #1	; 0x1
     f94:	e3a02013 	mov	r2, #19	; 0x13
     f98:	ebfffdd6 	bl	6f8 <module_set_clock>
     f9c:	e59f007c 	ldr	r0, [pc, #124]	; 1020 <set_clk+0xec>
     fa0:	e3a01002 	mov	r1, #2	; 0x2
     fa4:	e3a0200f 	mov	r2, #15	; 0xf
     fa8:	ebfffdd2 	bl	6f8 <module_set_clock>
     fac:	e59f0070 	ldr	r0, [pc, #112]	; 1024 <set_clk+0xf0>
     fb0:	e3a01002 	mov	r1, #2	; 0x2
     fb4:	e3a02019 	mov	r2, #25	; 0x19
     fb8:	ebfffdce 	bl	6f8 <module_set_clock>
     fbc:	e59f4064 	ldr	r4, [pc, #100]	; 1028 <set_clk+0xf4>
     fc0:	e59f5064 	ldr	r5, [pc, #100]	; 102c <set_clk+0xf8>
     fc4:	e1a00004 	mov	r0, r4
     fc8:	e3a01002 	mov	r1, #2	; 0x2
     fcc:	e2844010 	add	r4, r4, #16	; 0x10
     fd0:	e3a02003 	mov	r2, #3	; 0x3
     fd4:	ebfffdc7 	bl	6f8 <module_set_clock>
     fd8:	e1540005 	cmp	r4, r5
     fdc:	1afffff8 	bne	fc4 <set_clk+0x90>
     fe0:	e59f0048 	ldr	r0, [pc, #72]	; 1030 <set_clk+0xfc>
     fe4:	e3a01003 	mov	r1, #3	; 0x3
     fe8:	e3a02013 	mov	r2, #19	; 0x13
     fec:	ebfffdc1 	bl	6f8 <module_set_clock>
     ff0:	e59f003c 	ldr	r0, [pc, #60]	; 1034 <set_clk+0x100>
     ff4:	e3a01001 	mov	r1, #1	; 0x1
     ff8:	e3a02004 	mov	r2, #4	; 0x4
     ffc:	ebfffdbd 	bl	6f8 <module_set_clock>
    1000:	e59f0030 	ldr	r0, [pc, #48]	; 1038 <set_clk+0x104>
    1004:	e3a01002 	mov	r1, #2	; 0x2
    1008:	e3a02007 	mov	r2, #7	; 0x7
    100c:	e28dd004 	add	sp, sp, #4	; 0x4
    1010:	e8bd4030 	pop	{r4, r5, lr}
    1014:	eafffdb7 	b	6f8 <module_set_clock>
    1018:	2d00a250 	.word	0x2d00a250
    101c:	2d00a2c0 	.word	0x2d00a2c0
    1020:	2d00a240 	.word	0x2d00a240
    1024:	2d00a290 	.word	0x2d00a290
    1028:	2d00a130 	.word	0x2d00a130
    102c:	2d00a190 	.word	0x2d00a190
    1030:	2d00a260 	.word	0x2d00a260
    1034:	2d00a2a0 	.word	0x2d00a2a0
    1038:	2d00a360 	.word	0x2d00a360

0000103c <chip_test>:
    103c:	e59f0044 	ldr	r0, [pc, #68]	; 1088 <end+0x4>
    1040:	e5901000 	ldr	r1, [r0]
    1044:	e59f2040 	ldr	r2, [pc, #64]	; 108c <end+0x8>
    1048:	e1520001 	cmp	r2, r1
    104c:	1a00000c 	bne	1084 <end>
    1050:	e5901004 	ldr	r1, [r0, #4]
    1054:	e59f2034 	ldr	r2, [pc, #52]	; 1090 <end+0xc>
    1058:	e1520001 	cmp	r2, r1
    105c:	1a000008 	bne	1084 <end>
    1060:	e5901008 	ldr	r1, [r0, #8]
    1064:	e59f2028 	ldr	r2, [pc, #40]	; 1094 <end+0x10>
    1068:	e1520001 	cmp	r2, r1
    106c:	1a000004 	bne	1084 <end>
    1070:	e590100c 	ldr	r1, [r0, #12]
    1074:	e59f201c 	ldr	r2, [pc, #28]	; 1098 <end+0x14>
    1078:	e1520001 	cmp	r2, r1
    107c:	1a000000 	bne	1084 <end>
    1080:	e59ff014 	ldr	pc, [pc, #20]	; 109c <end+0x18>

00001084 <end>:
    1084:	e1a0f00e 	mov	pc, lr
    1088:	08000040 	.word	0x08000040
    108c:	6f666e69 	.word	0x6f666e69
    1090:	612d6d74 	.word	0x612d6d74
    1094:	696c      	.short	0x696c
    1096:	6e          	.byte	0x6e
    1097:	2d          	.byte	0x2d
    1098:	796c6e6f 	.word	0x796c6e6f
    109c:	08000200 	.word	0x08000200

000010a0 <module_get_clock>:
    10a0:	e59f3134 	ldr	r3, [pc, #308]	; 11dc <module_get_clock+0x13c>
    10a4:	e1500003 	cmp	r0, r3
    10a8:	0a00003d 	beq	11a4 <module_get_clock+0x104>
    10ac:	8a000018 	bhi	1114 <module_get_clock+0x74>
    10b0:	e2433050 	sub	r3, r3, #80	; 0x50
    10b4:	e1500003 	cmp	r0, r3
    10b8:	0a000039 	beq	11a4 <module_get_clock+0x104>
    10bc:	8a000008 	bhi	10e4 <module_get_clock+0x44>
    10c0:	e3500003 	cmp	r0, #3	; 0x3
    10c4:	059f0114 	ldreq	r0, [pc, #276]	; 11e0 <module_get_clock+0x140>
    10c8:	012fff1e 	bxeq	lr
    10cc:	e2433010 	sub	r3, r3, #16	; 0x10
    10d0:	e1500003 	cmp	r0, r3
    10d4:	0a000032 	beq	11a4 <module_get_clock+0x104>
    10d8:	e3500001 	cmp	r0, #1	; 0x1
    10dc:	1a000032 	bne	11ac <module_get_clock+0x10c>
    10e0:	ea00003b 	b	11d4 <module_get_clock+0x134>
    10e4:	e59f30f8 	ldr	r3, [pc, #248]	; 11e4 <module_get_clock+0x144>
    10e8:	e1500003 	cmp	r0, r3
    10ec:	0a00002c 	beq	11a4 <module_get_clock+0x104>
    10f0:	92433010 	subls	r3, r3, #16	; 0x10
    10f4:	9a000003 	bls	1108 <module_get_clock+0x68>
    10f8:	e59f30e8 	ldr	r3, [pc, #232]	; 11e8 <module_get_clock+0x148>
    10fc:	e1500003 	cmp	r0, r3
    1100:	0a000027 	beq	11a4 <module_get_clock+0x104>
    1104:	e2833010 	add	r3, r3, #16	; 0x10
    1108:	e1500003 	cmp	r0, r3
    110c:	1a000026 	bne	11ac <module_get_clock+0x10c>
    1110:	ea000023 	b	11a4 <module_get_clock+0x104>
    1114:	e59f30d0 	ldr	r3, [pc, #208]	; 11ec <module_get_clock+0x14c>
    1118:	e1500003 	cmp	r0, r3
    111c:	059f00cc 	ldreq	r0, [pc, #204]	; 11f0 <module_get_clock+0x150>
    1120:	012fff1e 	bxeq	lr
    1124:	8a00000e 	bhi	1164 <module_get_clock+0xc4>
    1128:	e2433050 	sub	r3, r3, #80	; 0x50
    112c:	e1500003 	cmp	r0, r3
    1130:	0a00001f 	beq	11b4 <module_get_clock+0x114>
    1134:	8a000003 	bhi	1148 <module_get_clock+0xa8>
    1138:	e2433010 	sub	r3, r3, #16	; 0x10
    113c:	e1500003 	cmp	r0, r3
    1140:	1a000019 	bne	11ac <module_get_clock+0x10c>
    1144:	ea00001a 	b	11b4 <module_get_clock+0x114>
    1148:	e59f30a4 	ldr	r3, [pc, #164]	; 11f4 <module_get_clock+0x154>
    114c:	e1500003 	cmp	r0, r3
    1150:	0a000017 	beq	11b4 <module_get_clock+0x114>
    1154:	e2833030 	add	r3, r3, #48	; 0x30
    1158:	e1500003 	cmp	r0, r3
    115c:	1a000012 	bne	11ac <module_get_clock+0x10c>
    1160:	ea000017 	b	11c4 <module_get_clock+0x124>
    1164:	e59f308c 	ldr	r3, [pc, #140]	; 11f8 <module_get_clock+0x158>
    1168:	e1500003 	cmp	r0, r3
    116c:	059f0088 	ldreq	r0, [pc, #136]	; 11fc <module_get_clock+0x15c>
    1170:	012fff1e 	bxeq	lr
    1174:	8a000003 	bhi	1188 <module_get_clock+0xe8>
    1178:	e2433010 	sub	r3, r3, #16	; 0x10
    117c:	e1500003 	cmp	r0, r3
    1180:	1a000009 	bne	11ac <module_get_clock+0x10c>
    1184:	ea00000c 	b	11bc <module_get_clock+0x11c>
    1188:	e59f3070 	ldr	r3, [pc, #112]	; 1200 <module_get_clock+0x160>
    118c:	e1500003 	cmp	r0, r3
    1190:	0a000003 	beq	11a4 <module_get_clock+0x104>
    1194:	e2833040 	add	r3, r3, #64	; 0x40
    1198:	e1500003 	cmp	r0, r3
    119c:	1a000002 	bne	11ac <module_get_clock+0x10c>
    11a0:	ea000009 	b	11cc <module_get_clock+0x12c>
    11a4:	e59f0058 	ldr	r0, [pc, #88]	; 1204 <module_get_clock+0x164>
    11a8:	e12fff1e 	bx	lr
    11ac:	e3a00000 	mov	r0, #0	; 0x0
    11b0:	e12fff1e 	bx	lr
    11b4:	e59f004c 	ldr	r0, [pc, #76]	; 1208 <module_get_clock+0x168>
    11b8:	e12fff1e 	bx	lr
    11bc:	e59f0048 	ldr	r0, [pc, #72]	; 120c <module_get_clock+0x16c>
    11c0:	e12fff1e 	bx	lr
    11c4:	e59f0044 	ldr	r0, [pc, #68]	; 1210 <module_get_clock+0x170>
    11c8:	e12fff1e 	bx	lr
    11cc:	e59f0040 	ldr	r0, [pc, #64]	; 1214 <module_get_clock+0x174>
    11d0:	e12fff1e 	bx	lr
    11d4:	e59f003c 	ldr	r0, [pc, #60]	; 1218 <module_get_clock+0x178>
    11d8:	e12fff1e 	bx	lr
    11dc:	2d00a190 	.word	0x2d00a190
    11e0:	03d83120 	.word	0x03d83120
    11e4:	2d00a160 	.word	0x2d00a160
    11e8:	2d00a170 	.word	0x2d00a170
    11ec:	2d00a2a0 	.word	0x2d00a2a0
    11f0:	05b8d800 	.word	0x05b8d800
    11f4:	2d00a260 	.word	0x2d00a260
    11f8:	2d00a2d0 	.word	0x2d00a2d0
    11fc:	02faf080 	.word	0x02faf080
    1200:	2d00a320 	.word	0x2d00a320
    1204:	08f0d180 	.word	0x08f0d180
    1208:	02625a00 	.word	0x02625a00
    120c:	016e3600 	.word	0x016e3600
    1210:	0160203b 	.word	0x0160203b
    1214:	047868c0 	.word	0x047868c0
    1218:	0ff95b00 	.word	0x0ff95b00

0000121c <set_mux>:
    121c:	e59f3004 	ldr	r3, [pc, #4]	; 1228 <set_mux+0xc>
    1220:	e5830000 	str	r0, [r3]
    1224:	e12fff1e 	bx	lr
    1228:	2d009000 	.word	0x2d009000

0000122c <get_mux>:
    122c:	e59f3004 	ldr	r3, [pc, #4]	; 1238 <get_mux+0xc>
    1230:	e5930000 	ldr	r0, [r3]
    1234:	e12fff1e 	bx	lr
    1238:	2d009000 	.word	0x2d009000

0000123c <strcpy>:
    123c:	e3a02000 	mov	r2, #0	; 0x0
    1240:	e7d13002 	ldrb	r3, [r1, r2]
    1244:	e3530000 	cmp	r3, #0	; 0x0
    1248:	e7c03002 	strb	r3, [r0, r2]
    124c:	e2822001 	add	r2, r2, #1	; 0x1
    1250:	1afffffa 	bne	1240 <strcpy+0x4>
    1254:	e12fff1e 	bx	lr

00001258 <strncpy>:
    1258:	e3a0c000 	mov	ip, #0	; 0x0
    125c:	e3520000 	cmp	r2, #0	; 0x0
    1260:	e2422001 	sub	r2, r2, #1	; 0x1
    1264:	012fff1e 	bxeq	lr
    1268:	e7d1300c 	ldrb	r3, [r1, ip]
    126c:	e3530000 	cmp	r3, #0	; 0x0
    1270:	e7c0300c 	strb	r3, [r0, ip]
    1274:	e28cc001 	add	ip, ip, #1	; 0x1
    1278:	1afffff7 	bne	125c <strncpy+0x4>
    127c:	e12fff1e 	bx	lr

00001280 <strncat>:
    1280:	e92d4030 	push	{r4, r5, lr}
    1284:	e2525000 	subs	r5, r2, #0	; 0x0
    1288:	e1a04001 	mov	r4, r1
    128c:	11a0c000 	movne	ip, r0
    1290:	1a000001 	bne	129c <strncat+0x1c>
    1294:	e8bd8030 	pop	{r4, r5, pc}
    1298:	e28cc001 	add	ip, ip, #1	; 0x1
    129c:	e5dc3000 	ldrb	r3, [ip]
    12a0:	e3530000 	cmp	r3, #0	; 0x0
    12a4:	1afffffb 	bne	1298 <strncat+0x18>
    12a8:	ea000004 	b	12c0 <strncat+0x40>
    12ac:	e1550002 	cmp	r5, r2
    12b0:	1a000004 	bne	12c8 <strncat+0x48>
    12b4:	e3a03000 	mov	r3, #0	; 0x0
    12b8:	e5cc3000 	strb	r3, [ip]
    12bc:	e8bd8030 	pop	{r4, r5, pc}
    12c0:	e1a02003 	mov	r2, r3
    12c4:	e1a0100c 	mov	r1, ip
    12c8:	e7d43002 	ldrb	r3, [r4, r2]
    12cc:	e28cc001 	add	ip, ip, #1	; 0x1
    12d0:	e3530000 	cmp	r3, #0	; 0x0
    12d4:	e7c13002 	strb	r3, [r1, r2]
    12d8:	e2822001 	add	r2, r2, #1	; 0x1
    12dc:	1afffff2 	bne	12ac <strncat+0x2c>
    12e0:	e8bd8030 	pop	{r4, r5, pc}

000012e4 <strcmp>:
    12e4:	e92d4010 	push	{r4, lr}
    12e8:	e3a02000 	mov	r2, #0	; 0x0
    12ec:	e1a04000 	mov	r4, r0
    12f0:	e7d4c002 	ldrb	ip, [r4, r2]
    12f4:	e7d13002 	ldrb	r3, [r1, r2]
    12f8:	e2822001 	add	r2, r2, #1	; 0x1
    12fc:	e063300c 	rsb	r3, r3, ip
    1300:	e6ef0073 	uxtb	r0, r3
    1304:	e3500000 	cmp	r0, #0	; 0x0
    1308:	1a000001 	bne	1314 <strcmp+0x30>
    130c:	e35c0000 	cmp	ip, #0	; 0x0
    1310:	1afffff6 	bne	12f0 <strcmp+0xc>
    1314:	e6af0070 	sxtb	r0, r0
    1318:	e8bd8010 	pop	{r4, pc}

0000131c <strncmp>:
    131c:	e92d4030 	push	{r4, r5, lr}
    1320:	e3a0c000 	mov	ip, #0	; 0x0
    1324:	e1a05000 	mov	r5, r0
    1328:	e1a04001 	mov	r4, r1
    132c:	ea000009 	b	1358 <strncmp+0x3c>
    1330:	e7d5100c 	ldrb	r1, [r5, ip]
    1334:	e7d4300c 	ldrb	r3, [r4, ip]
    1338:	e2422001 	sub	r2, r2, #1	; 0x1
    133c:	e0633001 	rsb	r3, r3, r1
    1340:	e6ef0073 	uxtb	r0, r3
    1344:	e3500000 	cmp	r0, #0	; 0x0
    1348:	e28cc001 	add	ip, ip, #1	; 0x1
    134c:	1a000004 	bne	1364 <strncmp+0x48>
    1350:	e3510000 	cmp	r1, #0	; 0x0
    1354:	0a000002 	beq	1364 <strncmp+0x48>
    1358:	e3520000 	cmp	r2, #0	; 0x0
    135c:	1afffff3 	bne	1330 <strncmp+0x14>
    1360:	e1a00002 	mov	r0, r2
    1364:	e6af0070 	sxtb	r0, r0
    1368:	e8bd8030 	pop	{r4, r5, pc}

0000136c <strchr>:
    136c:	e1a03000 	mov	r3, r0
    1370:	e6ef1071 	uxtb	r1, r1
    1374:	ea000002 	b	1384 <strchr+0x18>
    1378:	e3500000 	cmp	r0, #0	; 0x0
    137c:	012fff1e 	bxeq	lr
    1380:	e2833001 	add	r3, r3, #1	; 0x1
    1384:	e5d30000 	ldrb	r0, [r3]
    1388:	e1500001 	cmp	r0, r1
    138c:	1afffff9 	bne	1378 <strchr+0xc>
    1390:	e1a00003 	mov	r0, r3
    1394:	e12fff1e 	bx	lr

00001398 <strlen>:
    1398:	e1a02000 	mov	r2, r0
    139c:	ea000000 	b	13a4 <strlen+0xc>
    13a0:	e2800001 	add	r0, r0, #1	; 0x1
    13a4:	e5d03000 	ldrb	r3, [r0]
    13a8:	e3530000 	cmp	r3, #0	; 0x0
    13ac:	1afffffb 	bne	13a0 <strlen+0x8>
    13b0:	e0620000 	rsb	r0, r2, r0
    13b4:	e12fff1e 	bx	lr

000013b8 <strnlen>:
    13b8:	e3a02000 	mov	r2, #0	; 0x0
    13bc:	e1510002 	cmp	r1, r2
    13c0:	e080c002 	add	ip, r0, r2
    13c4:	0a000003 	beq	13d8 <strnlen+0x20>
    13c8:	e7d03002 	ldrb	r3, [r0, r2]
    13cc:	e2822001 	add	r2, r2, #1	; 0x1
    13d0:	e3530000 	cmp	r3, #0	; 0x0
    13d4:	1afffff8 	bne	13bc <strnlen+0x4>
    13d8:	e060000c 	rsb	r0, r0, ip
    13dc:	e12fff1e 	bx	lr

000013e0 <memset>:
    13e0:	e3a03000 	mov	r3, #0	; 0x0
    13e4:	ea000001 	b	13f0 <memset+0x10>
    13e8:	e7c01003 	strb	r1, [r0, r3]
    13ec:	e2833001 	add	r3, r3, #1	; 0x1
    13f0:	e3520000 	cmp	r2, #0	; 0x0
    13f4:	e2422001 	sub	r2, r2, #1	; 0x1
    13f8:	1afffffa 	bne	13e8 <memset+0x8>
    13fc:	e12fff1e 	bx	lr

00001400 <memcpy>:
    1400:	e3a0c000 	mov	ip, #0	; 0x0
    1404:	ea000002 	b	1414 <memcpy+0x14>
    1408:	e7d1300c 	ldrb	r3, [r1, ip]
    140c:	e7c0300c 	strb	r3, [r0, ip]
    1410:	e28cc001 	add	ip, ip, #1	; 0x1
    1414:	e3520000 	cmp	r2, #0	; 0x0
    1418:	e2422001 	sub	r2, r2, #1	; 0x1
    141c:	1afffff9 	bne	1408 <memcpy+0x8>
    1420:	e12fff1e 	bx	lr

00001424 <memmove>:
    1424:	e1500001 	cmp	r0, r1
    1428:	e92d4010 	push	{r4, lr}
    142c:	08bd8010 	popeq	{r4, pc}
    1430:	93a0c000 	movls	ip, #0	; 0x0
    1434:	9a000003 	bls	1448 <memmove+0x24>
    1438:	ea000006 	b	1458 <memmove+0x34>
    143c:	e7d1300c 	ldrb	r3, [r1, ip]
    1440:	e7c0300c 	strb	r3, [r0, ip]
    1444:	e28cc001 	add	ip, ip, #1	; 0x1
    1448:	e3520000 	cmp	r2, #0	; 0x0
    144c:	e2422001 	sub	r2, r2, #1	; 0x1
    1450:	1afffff9 	bne	143c <memmove+0x18>
    1454:	e8bd8010 	pop	{r4, pc}
    1458:	e0814002 	add	r4, r1, r2
    145c:	e3a0c000 	mov	ip, #0	; 0x0
    1460:	e0801002 	add	r1, r0, r2
    1464:	ea000001 	b	1470 <memmove+0x4c>
    1468:	e7d4300c 	ldrb	r3, [r4, ip]
    146c:	e7c1300c 	strb	r3, [r1, ip]
    1470:	e3520000 	cmp	r2, #0	; 0x0
    1474:	e24cc001 	sub	ip, ip, #1	; 0x1
    1478:	e2422001 	sub	r2, r2, #1	; 0x1
    147c:	1afffff9 	bne	1468 <memmove+0x44>
    1480:	e8bd8010 	pop	{r4, pc}

00001484 <memcmp>:
    1484:	e92d4030 	push	{r4, r5, lr}
    1488:	e3a0c000 	mov	ip, #0	; 0x0
    148c:	e1a04001 	mov	r4, r1
    1490:	e1a05000 	mov	r5, r0
    1494:	e1a01002 	mov	r1, r2
    1498:	ea000005 	b	14b4 <memcmp+0x30>
    149c:	e7d5200c 	ldrb	r2, [r5, ip]
    14a0:	e7d4300c 	ldrb	r3, [r4, ip]
    14a4:	e28cc001 	add	ip, ip, #1	; 0x1
    14a8:	e0520003 	subs	r0, r2, r3
    14ac:	18bd8030 	popne	{r4, r5, pc}
    14b0:	e2411001 	sub	r1, r1, #1	; 0x1
    14b4:	e3510000 	cmp	r1, #0	; 0x0
    14b8:	1afffff7 	bne	149c <memcmp+0x18>
    14bc:	e1a00001 	mov	r0, r1
    14c0:	e8bd8030 	pop	{r4, r5, pc}

000014c4 <so_m_isused>:
    14c4:	e5903004 	ldr	r3, [r0, #4]
    14c8:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    14cc:	03a00000 	moveq	r0, #0	; 0x0
    14d0:	13a00001 	movne	r0, #1	; 0x1
    14d4:	e12fff1e 	bx	lr

000014d8 <so_m_set_used>:
    14d8:	e5903004 	ldr	r3, [r0, #4]
    14dc:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    14e0:	e5803004 	str	r3, [r0, #4]
    14e4:	e12fff1e 	bx	lr

000014e8 <so_m_unset_used>:
    14e8:	e5903004 	ldr	r3, [r0, #4]
    14ec:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    14f0:	e5803004 	str	r3, [r0, #4]
    14f4:	e12fff1e 	bx	lr

000014f8 <so_m_get_size>:
    14f8:	e5900004 	ldr	r0, [r0, #4]
    14fc:	e3c00103 	bic	r0, r0, #-1073741824	; 0xc0000000
    1500:	e12fff1e 	bx	lr

00001504 <so_m_set_size>:
    1504:	e5903004 	ldr	r3, [r0, #4]
    1508:	e3c11103 	bic	r1, r1, #-1073741824	; 0xc0000000
    150c:	e2033103 	and	r3, r3, #-1073741824	; 0xc0000000
    1510:	e1811003 	orr	r1, r1, r3
    1514:	e5801004 	str	r1, [r0, #4]
    1518:	e12fff1e 	bx	lr

0000151c <so_m_alloc>:
    151c:	e59f3048 	ldr	r3, [pc, #72]	; 156c <so_m_alloc+0x50>
    1520:	e3a02000 	mov	r2, #0	; 0x0
    1524:	e593c000 	ldr	ip, [r3]
    1528:	e5931008 	ldr	r1, [r3, #8]
    152c:	ea000007 	b	1550 <so_m_alloc+0x34>
    1530:	e5903004 	ldr	r3, [r0, #4]
    1534:	e3530000 	cmp	r3, #0	; 0x0
    1538:	1a000004 	bne	1550 <so_m_alloc+0x34>
    153c:	e59f2028 	ldr	r2, [pc, #40]	; 156c <so_m_alloc+0x50>
    1540:	e5923004 	ldr	r3, [r2, #4]
    1544:	e2433001 	sub	r3, r3, #1	; 0x1
    1548:	e5823004 	str	r3, [r2, #4]
    154c:	e12fff1e 	bx	lr
    1550:	e152000c 	cmp	r2, ip
    1554:	e1a00001 	mov	r0, r1
    1558:	e2822001 	add	r2, r2, #1	; 0x1
    155c:	e2811010 	add	r1, r1, #16	; 0x10
    1560:	3afffff2 	bcc	1530 <so_m_alloc+0x14>
    1564:	e3a00000 	mov	r0, #0	; 0x0
    1568:	e12fff1e 	bx	lr
    156c:	0800d7a4 	.word	0x0800d7a4

00001570 <so_m_remove>:
    1570:	e3a03000 	mov	r3, #0	; 0x0
    1574:	e59f2014 	ldr	r2, [pc, #20]	; 1590 <so_m_remove+0x20>
    1578:	e5803000 	str	r3, [r0]
    157c:	e5803004 	str	r3, [r0, #4]
    1580:	e5923004 	ldr	r3, [r2, #4]
    1584:	e2833001 	add	r3, r3, #1	; 0x1
    1588:	e5823004 	str	r3, [r2, #4]
    158c:	e12fff1e 	bx	lr
    1590:	0800d7a4 	.word	0x0800d7a4

00001594 <malloc>:
    1594:	e3500000 	cmp	r0, #0	; 0x0
    1598:	e92d4070 	push	{r4, r5, r6, lr}
    159c:	0a000047 	beq	16c0 <malloc+0x12c>
    15a0:	e59f1120 	ldr	r1, [pc, #288]	; 16c8 <malloc+0x134>
    15a4:	e280301f 	add	r3, r0, #31	; 0x1f
    15a8:	e5b12014 	ldr	r2, [r1, #20]!
    15ac:	e3c3401f 	bic	r4, r3, #31	; 0x1f
    15b0:	e2425008 	sub	r5, r2, #8	; 0x8
    15b4:	ea000009 	b	15e0 <malloc+0x4c>
    15b8:	e5953004 	ldr	r3, [r5, #4]
    15bc:	e2130103 	ands	r0, r3, #-1073741824	; 0xc0000000
    15c0:	e3c32103 	bic	r2, r3, #-1073741824	; 0xc0000000
    15c4:	1a000003 	bne	15d8 <malloc+0x44>
    15c8:	e1520004 	cmp	r2, r4
    15cc:	3a000001 	bcc	15d8 <malloc+0x44>
    15d0:	9a000030 	bls	1698 <malloc+0x104>
    15d4:	ea000005 	b	15f0 <malloc+0x5c>
    15d8:	e5953008 	ldr	r3, [r5, #8]
    15dc:	e2435008 	sub	r5, r3, #8	; 0x8
    15e0:	e2853008 	add	r3, r5, #8	; 0x8
    15e4:	e1530001 	cmp	r3, r1
    15e8:	1afffff2 	bne	15b8 <malloc+0x24>
    15ec:	ea000033 	b	16c0 <malloc+0x12c>
    15f0:	e59f30d0 	ldr	r3, [pc, #208]	; 16c8 <malloc+0x134>
    15f4:	e1a01000 	mov	r1, r0
    15f8:	e5932008 	ldr	r2, [r3, #8]
    15fc:	e5930000 	ldr	r0, [r3]
    1600:	ea00000a 	b	1630 <malloc+0x9c>
    1604:	e5923004 	ldr	r3, [r2, #4]
    1608:	e2822010 	add	r2, r2, #16	; 0x10
    160c:	e3530000 	cmp	r3, #0	; 0x0
    1610:	1a000006 	bne	1630 <malloc+0x9c>
    1614:	e59f20ac 	ldr	r2, [pc, #172]	; 16c8 <malloc+0x134>
    1618:	e3560000 	cmp	r6, #0	; 0x0
    161c:	e5923004 	ldr	r3, [r2, #4]
    1620:	e2433001 	sub	r3, r3, #1	; 0x1
    1624:	e5823004 	str	r3, [r2, #4]
    1628:	1a000005 	bne	1644 <malloc+0xb0>
    162c:	ea000023 	b	16c0 <malloc+0x12c>
    1630:	e1510000 	cmp	r1, r0
    1634:	e1a06002 	mov	r6, r2
    1638:	e2811001 	add	r1, r1, #1	; 0x1
    163c:	3afffff0 	bcc	1604 <malloc+0x70>
    1640:	ea00001e 	b	16c0 <malloc+0x12c>
    1644:	e5953004 	ldr	r3, [r5, #4]
    1648:	e5962004 	ldr	r2, [r6, #4]
    164c:	e0643003 	rsb	r3, r4, r3
    1650:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    1654:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1658:	e1833002 	orr	r3, r3, r2
    165c:	e5863004 	str	r3, [r6, #4]
    1660:	e5951000 	ldr	r1, [r5]
    1664:	e5950004 	ldr	r0, [r5, #4]
    1668:	e1a0c005 	mov	ip, r5
    166c:	e1a03006 	mov	r3, r6
    1670:	e0841001 	add	r1, r4, r1
    1674:	e3c42103 	bic	r2, r4, #-1073741824	; 0xc0000000
    1678:	e2000103 	and	r0, r0, #-1073741824	; 0xc0000000
    167c:	e5bc4008 	ldr	r4, [ip, #8]!
    1680:	e4831008 	str	r1, [r3], #8
    1684:	e1822000 	orr	r2, r2, r0
    1688:	e5864008 	str	r4, [r6, #8]
    168c:	e5843004 	str	r3, [r4, #4]
    1690:	e985000c 	stmib	r5, {r2, r3}
    1694:	e586c00c 	str	ip, [r6, #12]
    1698:	e5953004 	ldr	r3, [r5, #4]
    169c:	e59f1024 	ldr	r1, [pc, #36]	; 16c8 <malloc+0x134>
    16a0:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    16a4:	e5853004 	str	r3, [r5, #4]
    16a8:	e5912010 	ldr	r2, [r1, #16]
    16ac:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    16b0:	e0833002 	add	r3, r3, r2
    16b4:	e5813010 	str	r3, [r1, #16]
    16b8:	e5950000 	ldr	r0, [r5]
    16bc:	e8bd8070 	pop	{r4, r5, r6, pc}
    16c0:	e3a00000 	mov	r0, #0	; 0x0
    16c4:	e8bd8070 	pop	{r4, r5, r6, pc}
    16c8:	0800d7a4 	.word	0x0800d7a4

000016cc <__sOMERGe>:
    16cc:	e5902004 	ldr	r2, [r0, #4]
    16d0:	e92d4030 	push	{r4, r5, lr}
    16d4:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    16d8:	e1a05000 	mov	r5, r0
    16dc:	e1a0c001 	mov	ip, r1
    16e0:	18bd8030 	popne	{r4, r5, pc}
    16e4:	e5913004 	ldr	r3, [r1, #4]
    16e8:	e2134103 	ands	r4, r3, #-1073741824	; 0xc0000000
    16ec:	18bd8030 	popne	{r4, r5, pc}
    16f0:	e0823003 	add	r3, r2, r3
    16f4:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    16f8:	e59f0030 	ldr	r0, [pc, #48]	; 1730 <__sOMERGe+0x64>
    16fc:	e5853004 	str	r3, [r5, #4]
    1700:	e5814000 	str	r4, [r1]
    1704:	e5814004 	str	r4, [r1, #4]
    1708:	e5903004 	ldr	r3, [r0, #4]
    170c:	e591100c 	ldr	r1, [r1, #12]
    1710:	e59c2008 	ldr	r2, [ip, #8]
    1714:	e2833001 	add	r3, r3, #1	; 0x1
    1718:	e5812000 	str	r2, [r1]
    171c:	e5821004 	str	r1, [r2, #4]
    1720:	e5803004 	str	r3, [r0, #4]
    1724:	e58c4008 	str	r4, [ip, #8]
    1728:	e58c400c 	str	r4, [ip, #12]
    172c:	e8bd8030 	pop	{r4, r5, pc}
    1730:	0800d7a4 	.word	0x0800d7a4

00001734 <free>:
    1734:	e3500000 	cmp	r0, #0	; 0x0
    1738:	159f2090 	ldrne	r2, [pc, #144]	; 17d0 <free+0x9c>
    173c:	e92d4010 	push	{r4, lr}
    1740:	15b23014 	ldrne	r3, [r2, #20]!
    1744:	1a000004 	bne	175c <free+0x28>
    1748:	e8bd8010 	pop	{r4, pc}
    174c:	e5943000 	ldr	r3, [r4]
    1750:	e1500003 	cmp	r0, r3
    1754:	0a000005 	beq	1770 <free+0x3c>
    1758:	e5943008 	ldr	r3, [r4, #8]
    175c:	e2434008 	sub	r4, r3, #8	; 0x8
    1760:	e2843008 	add	r3, r4, #8	; 0x8
    1764:	e1530002 	cmp	r3, r2
    1768:	1afffff7 	bne	174c <free+0x18>
    176c:	e8bd8010 	pop	{r4, pc}
    1770:	e5943004 	ldr	r3, [r4, #4]
    1774:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    1778:	08bd8010 	popeq	{r4, pc}
    177c:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1780:	e59f1048 	ldr	r1, [pc, #72]	; 17d0 <free+0x9c>
    1784:	e5843004 	str	r3, [r4, #4]
    1788:	e5912010 	ldr	r2, [r1, #16]
    178c:	e5940008 	ldr	r0, [r4, #8]
    1790:	e0632002 	rsb	r2, r3, r2
    1794:	e59f3038 	ldr	r3, [pc, #56]	; 17d4 <free+0xa0>
    1798:	e5812010 	str	r2, [r1, #16]
    179c:	e1500003 	cmp	r0, r3
    17a0:	0a000002 	beq	17b0 <free+0x7c>
    17a4:	e2401008 	sub	r1, r0, #8	; 0x8
    17a8:	e1a00004 	mov	r0, r4
    17ac:	ebffffc6 	bl	16cc <__sOMERGe>
    17b0:	e594000c 	ldr	r0, [r4, #12]
    17b4:	e59f3018 	ldr	r3, [pc, #24]	; 17d4 <free+0xa0>
    17b8:	e1500003 	cmp	r0, r3
    17bc:	08bd8010 	popeq	{r4, pc}
    17c0:	e2400008 	sub	r0, r0, #8	; 0x8
    17c4:	e1a01004 	mov	r1, r4
    17c8:	e8bd4010 	pop	{r4, lr}
    17cc:	eaffffbe 	b	16cc <__sOMERGe>
    17d0:	0800d7a4 	.word	0x0800d7a4
    17d4:	0800d7b8 	.word	0x0800d7b8

000017d8 <__sOINFo>:
    17d8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    17dc:	e59f50a8 	ldr	r5, [pc, #168]	; 188c <__sOINFo+0xb4>
    17e0:	e24dd004 	sub	sp, sp, #4	; 0x4
    17e4:	e59f60a4 	ldr	r6, [pc, #164]	; 1890 <__sOINFo+0xb8>
    17e8:	e8950006 	ldm	r5, {r1, r2}
    17ec:	e59f00a0 	ldr	r0, [pc, #160]	; 1894 <__sOINFo+0xbc>
    17f0:	e1a0e00f 	mov	lr, pc
    17f4:	e596f000 	ldr	pc, [r6]
    17f8:	e2853008 	add	r3, r5, #8	; 0x8
    17fc:	e8930098 	ldm	r3, {r3, r4, r7}
    1800:	e2444b01 	sub	r4, r4, #1024	; 0x400
    1804:	e0634004 	rsb	r4, r3, r4
    1808:	e3a00064 	mov	r0, #100	; 0x64
    180c:	e1a01004 	mov	r1, r4
    1810:	e0000790 	mul	r0, r0, r7
    1814:	eb003a68 	bl	101bc <__aeabi_uidiv>
    1818:	e1a01004 	mov	r1, r4
    181c:	e1a03000 	mov	r3, r0
    1820:	e1a02007 	mov	r2, r7
    1824:	e59f006c 	ldr	r0, [pc, #108]	; 1898 <__sOINFo+0xc0>
    1828:	e1a0e00f 	mov	lr, pc
    182c:	e596f000 	ldr	pc, [r6]
    1830:	e59f0064 	ldr	r0, [pc, #100]	; 189c <__sOINFo+0xc4>
    1834:	e1a0e00f 	mov	lr, pc
    1838:	e596f000 	ldr	pc, [r6]
    183c:	e59f005c 	ldr	r0, [pc, #92]	; 18a0 <__sOINFo+0xc8>
    1840:	e1a0e00f 	mov	lr, pc
    1844:	e596f000 	ldr	pc, [r6]
    1848:	e5b53014 	ldr	r3, [r5, #20]!
    184c:	ea000007 	b	1870 <__sOINFo+0x98>
    1850:	e8940006 	ldm	r4, {r1, r2}
    1854:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    1858:	03a03000 	moveq	r3, #0	; 0x0
    185c:	13a03001 	movne	r3, #1	; 0x1
    1860:	e3c22103 	bic	r2, r2, #-1073741824	; 0xc0000000
    1864:	e1a0e00f 	mov	lr, pc
    1868:	e596f000 	ldr	pc, [r6]
    186c:	e5943008 	ldr	r3, [r4, #8]
    1870:	e2434008 	sub	r4, r3, #8	; 0x8
    1874:	e2843008 	add	r3, r4, #8	; 0x8
    1878:	e1530005 	cmp	r3, r5
    187c:	e59f0020 	ldr	r0, [pc, #32]	; 18a4 <__sOINFo+0xcc>
    1880:	1afffff2 	bne	1850 <__sOINFo+0x78>
    1884:	e28dd004 	add	sp, sp, #4	; 0x4
    1888:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    188c:	0800d7a4 	.word	0x0800d7a4
    1890:	0800d764 	.word	0x0800d764
    1894:	00011c13 	.word	0x00011c13
    1898:	00011c2d 	.word	0x00011c2d
    189c:	00011c52 	.word	0x00011c52
    18a0:	00011b00 	.word	0x00011b00
    18a4:	00011c63 	.word	0x00011c63

000018a8 <init_malloc>:
    18a8:	e59f20e4 	ldr	r2, [pc, #228]	; 1994 <init_malloc+0xec>
    18ac:	e59f30e4 	ldr	r3, [pc, #228]	; 1998 <init_malloc+0xf0>
    18b0:	e3c2001f 	bic	r0, r2, #31	; 0x1f
    18b4:	e3c3301f 	bic	r3, r3, #31	; 0x1f
    18b8:	e0602003 	rsb	r2, r0, r3
    18bc:	e92d4010 	push	{r4, lr}
    18c0:	e59f40d4 	ldr	r4, [pc, #212]	; 199c <init_malloc+0xf4>
    18c4:	e3520b01 	cmp	r2, #1024	; 0x400
    18c8:	e5840008 	str	r0, [r4, #8]
    18cc:	e584300c 	str	r3, [r4, #12]
    18d0:	93e00000 	mvnls	r0, #0	; 0x0
    18d4:	98bd8010 	popls	{r4, pc}
    18d8:	e284c014 	add	ip, r4, #20	; 0x14
    18dc:	e3a03040 	mov	r3, #64	; 0x40
    18e0:	e3a01000 	mov	r1, #0	; 0x0
    18e4:	e3a02b01 	mov	r2, #1024	; 0x400
    18e8:	e584c018 	str	ip, [r4, #24]
    18ec:	e584c014 	str	ip, [r4, #20]
    18f0:	e5843004 	str	r3, [r4, #4]
    18f4:	e5843000 	str	r3, [r4]
    18f8:	ebfffeb8 	bl	13e0 <memset>
    18fc:	e5940008 	ldr	r0, [r4, #8]
    1900:	e594c000 	ldr	ip, [r4]
    1904:	e1a01000 	mov	r1, r0
    1908:	e3a02000 	mov	r2, #0	; 0x0
    190c:	ea000006 	b	192c <init_malloc+0x84>
    1910:	e59e3004 	ldr	r3, [lr, #4]
    1914:	e3530000 	cmp	r3, #0	; 0x0
    1918:	059f207c 	ldreq	r2, [pc, #124]	; 199c <init_malloc+0xf4>
    191c:	05923004 	ldreq	r3, [r2, #4]
    1920:	02433001 	subeq	r3, r3, #1	; 0x1
    1924:	05823004 	streq	r3, [r2, #4]
    1928:	0a000005 	beq	1944 <init_malloc+0x9c>
    192c:	e152000c 	cmp	r2, ip
    1930:	e1a0e001 	mov	lr, r1
    1934:	e2822001 	add	r2, r2, #1	; 0x1
    1938:	e2811010 	add	r1, r1, #16	; 0x10
    193c:	3afffff3 	bcc	1910 <init_malloc+0x68>
    1940:	e3a0e000 	mov	lr, #0	; 0x0
    1944:	e59fc050 	ldr	ip, [pc, #80]	; 199c <init_malloc+0xf4>
    1948:	e59e2004 	ldr	r2, [lr, #4]
    194c:	e59c300c 	ldr	r3, [ip, #12]
    1950:	e1a0100e 	mov	r1, lr
    1954:	e2433b01 	sub	r3, r3, #1024	; 0x400
    1958:	e0603003 	rsb	r3, r0, r3
    195c:	e59c4018 	ldr	r4, [ip, #24]
    1960:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    1964:	e2800b01 	add	r0, r0, #1024	; 0x400
    1968:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    196c:	e4810008 	str	r0, [r1], #8
    1970:	e1833002 	orr	r3, r3, r2
    1974:	e28c2014 	add	r2, ip, #20	; 0x14
    1978:	e58e2008 	str	r2, [lr, #8]
    197c:	e58c1018 	str	r1, [ip, #24]
    1980:	e5841000 	str	r1, [r4]
    1984:	e58e3004 	str	r3, [lr, #4]
    1988:	e58e400c 	str	r4, [lr, #12]
    198c:	e3a00000 	mov	r0, #0	; 0x0
    1990:	e8bd8010 	pop	{r4, pc}
    1994:	0800f01f 	.word	0x0800f01f
    1998:	0801a000 	.word	0x0801a000
    199c:	0800d7a4 	.word	0x0800d7a4

000019a0 <isi_check_header>:
    19a0:	e92d4070 	push	{r4, r5, r6, lr}
    19a4:	e5902000 	ldr	r2, [r0]
    19a8:	e59f3088 	ldr	r3, [pc, #136]	; 1a38 <isi_check_header+0x98>
    19ac:	e1a05000 	mov	r5, r0
    19b0:	e1520003 	cmp	r2, r3
    19b4:	e24dd008 	sub	sp, sp, #8	; 0x8
    19b8:	13e0001e 	mvnne	r0, #30	; 0x1e
    19bc:	1a00001b 	bne	1a30 <isi_check_header+0x90>
    19c0:	e3a06000 	mov	r6, #0	; 0x0
    19c4:	e595401c 	ldr	r4, [r5, #28]
    19c8:	e3a01c02 	mov	r1, #512	; 0x200
    19cc:	e585601c 	str	r6, [r5, #28]
    19d0:	e1a00006 	mov	r0, r6
    19d4:	eb000f63 	bl	5768 <hash_init>
    19d8:	e3a01c02 	mov	r1, #512	; 0x200
    19dc:	e1a00005 	mov	r0, r5
    19e0:	eb000f3b 	bl	56d4 <hash_data>
    19e4:	e28d0004 	add	r0, sp, #4	; 0x4
    19e8:	eb000f1d 	bl	5664 <hash_value>
    19ec:	e585401c 	str	r4, [r5, #28]
    19f0:	e59d3004 	ldr	r3, [sp, #4]
    19f4:	e1540003 	cmp	r4, r3
    19f8:	13e0001f 	mvnne	r0, #31	; 0x1f
    19fc:	1a00000b 	bne	1a30 <isi_check_header+0x90>
    1a00:	e5951004 	ldr	r1, [r5, #4]
    1a04:	e59f4030 	ldr	r4, [pc, #48]	; 1a3c <isi_check_header+0x9c>
    1a08:	e3c114ff 	bic	r1, r1, #-16777216	; 0xff000000
    1a0c:	e59f002c 	ldr	r0, [pc, #44]	; 1a40 <isi_check_header+0xa0>
    1a10:	e1a0e00f 	mov	lr, pc
    1a14:	e594f000 	ldr	pc, [r4]
    1a18:	e5d51007 	ldrb	r1, [r5, #7]
    1a1c:	e59f0020 	ldr	r0, [pc, #32]	; 1a44 <isi_check_header+0xa4>
    1a20:	e201100f 	and	r1, r1, #15	; 0xf
    1a24:	e1a0e00f 	mov	lr, pc
    1a28:	e594f000 	ldr	pc, [r4]
    1a2c:	e1a00006 	mov	r0, r6
    1a30:	e28dd008 	add	sp, sp, #8	; 0x8
    1a34:	e8bd8070 	pop	{r4, r5, r6, pc}
    1a38:	6973692b 	.word	0x6973692b
    1a3c:	0800d764 	.word	0x0800d764
    1a40:	00011c8b 	.word	0x00011c8b
    1a44:	00011c99 	.word	0x00011c99

00001a48 <isi_check_data_separate>:
    1a48:	e92d4070 	push	{r4, r5, r6, lr}
    1a4c:	e1a05000 	mov	r5, r0
    1a50:	e5d00007 	ldrb	r0, [r0, #7]
    1a54:	e24dd028 	sub	sp, sp, #40	; 0x28
    1a58:	e1a04001 	mov	r4, r1
    1a5c:	e200000f 	and	r0, r0, #15	; 0xf
    1a60:	e595100c 	ldr	r1, [r5, #12]
    1a64:	eb000f3f 	bl	5768 <hash_init>
    1a68:	e28d6008 	add	r6, sp, #8	; 0x8
    1a6c:	e595100c 	ldr	r1, [r5, #12]
    1a70:	e1a00004 	mov	r0, r4
    1a74:	eb000f16 	bl	56d4 <hash_data>
    1a78:	e1a00006 	mov	r0, r6
    1a7c:	eb000ef8 	bl	5664 <hash_value>
    1a80:	eb000ef0 	bl	5648 <hash_len>
    1a84:	e2854020 	add	r4, r5, #32	; 0x20
    1a88:	e1a02000 	mov	r2, r0
    1a8c:	e1a01004 	mov	r1, r4
    1a90:	e1a00006 	mov	r0, r6
    1a94:	ebfffe7a 	bl	1484 <memcmp>
    1a98:	e3500000 	cmp	r0, #0	; 0x0
    1a9c:	0a000014 	beq	1af4 <isi_check_data_separate+0xac>
    1aa0:	e595100c 	ldr	r1, [r5, #12]
    1aa4:	e59f3050 	ldr	r3, [pc, #80]	; 1afc <isi_check_data_separate+0xb4>
    1aa8:	e59f0050 	ldr	r0, [pc, #80]	; 1b00 <isi_check_data_separate+0xb8>
    1aac:	e1a0e00f 	mov	lr, pc
    1ab0:	e593f000 	ldr	pc, [r3]
    1ab4:	eb000ee3 	bl	5648 <hash_len>
    1ab8:	e1a01004 	mov	r1, r4
    1abc:	e1a03000 	mov	r3, r0
    1ac0:	e3a02001 	mov	r2, #1	; 0x1
    1ac4:	e1a00004 	mov	r0, r4
    1ac8:	e3a04020 	mov	r4, #32	; 0x20
    1acc:	e58d4000 	str	r4, [sp]
    1ad0:	eb002d1d 	bl	cf4c <print_buffer>
    1ad4:	eb000edb 	bl	5648 <hash_len>
    1ad8:	e1a01006 	mov	r1, r6
    1adc:	e1a03000 	mov	r3, r0
    1ae0:	e3a02001 	mov	r2, #1	; 0x1
    1ae4:	e1a00006 	mov	r0, r6
    1ae8:	e58d4000 	str	r4, [sp]
    1aec:	eb002d16 	bl	cf4c <print_buffer>
    1af0:	e3e00020 	mvn	r0, #32	; 0x20
    1af4:	e28dd028 	add	sp, sp, #40	; 0x28
    1af8:	e8bd8070 	pop	{r4, r5, r6, pc}
    1afc:	0800d764 	.word	0x0800d764
    1b00:	00011cac 	.word	0x00011cac

00001b04 <isi_check_data>:
    1b04:	e2801c02 	add	r1, r0, #512	; 0x200
    1b08:	eaffffce 	b	1a48 <isi_check_data_separate>

00001b0c <isi_check>:
    1b0c:	e92d4010 	push	{r4, lr}
    1b10:	e1a04000 	mov	r4, r0
    1b14:	ebffffa1 	bl	19a0 <isi_check_header>
    1b18:	e3500000 	cmp	r0, #0	; 0x0
    1b1c:	18bd8010 	popne	{r4, pc}
    1b20:	e1a00004 	mov	r0, r4
    1b24:	e8bd4010 	pop	{r4, lr}
    1b28:	eafffff5 	b	1b04 <isi_check_data>

00001b2c <load_serial_ymodem>:
    1b2c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    1b30:	e24dde41 	sub	sp, sp, #1040	; 0x410
    1b34:	e28d4b01 	add	r4, sp, #1024	; 0x400
    1b38:	e284400c 	add	r4, r4, #12	; 0xc
    1b3c:	e3a03002 	mov	r3, #2	; 0x2
    1b40:	e1a07000 	mov	r7, r0
    1b44:	e1a01004 	mov	r1, r4
    1b48:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1b4c:	e58d3404 	str	r3, [sp, #1028]
    1b50:	eb00391f 	bl	ffd4 <xyzModem_stream_open>
    1b54:	e3500000 	cmp	r0, #0	; 0x0
    1b58:	1a000012 	bne	1ba8 <load_serial_ymodem+0x7c>
    1b5c:	e1a08004 	mov	r8, r4
    1b60:	e1a05000 	mov	r5, r0
    1b64:	e1a04000 	mov	r4, r0
    1b68:	e1a0600d 	mov	r6, sp
    1b6c:	ea000001 	b	1b78 <load_serial_ymodem+0x4c>
    1b70:	e085500c 	add	r5, r5, ip
    1b74:	ebfffe21 	bl	1400 <memcpy>
    1b78:	e3a01b01 	mov	r1, #1024	; 0x400
    1b7c:	e1a02008 	mov	r2, r8
    1b80:	e1a0000d 	mov	r0, sp
    1b84:	eb00387e 	bl	fd84 <xyzModem_stream_read>
    1b88:	e0843007 	add	r3, r4, r7
    1b8c:	e250c000 	subs	ip, r0, #0	; 0x0
    1b90:	e1a0100d 	mov	r1, sp
    1b94:	e1a0200c 	mov	r2, ip
    1b98:	e1a00003 	mov	r0, r3
    1b9c:	e084400c 	add	r4, r4, ip
    1ba0:	cafffff2 	bgt	1b70 <load_serial_ymodem+0x44>
    1ba4:	ea000007 	b	1bc8 <load_serial_ymodem+0x9c>
    1ba8:	e59f3054 	ldr	r3, [pc, #84]	; 1c04 <load_serial_ymodem+0xd8>
    1bac:	e59d040c 	ldr	r0, [sp, #1036]
    1bb0:	e5934000 	ldr	r4, [r3]
    1bb4:	eb0034f9 	bl	efa0 <xyzModem_error>
    1bb8:	e1a01000 	mov	r1, r0
    1bbc:	e59f0044 	ldr	r0, [pc, #68]	; 1c08 <load_serial_ymodem+0xdc>
    1bc0:	e12fff34 	blx	r4
    1bc4:	e3a05000 	mov	r5, #0	; 0x0
    1bc8:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1bcc:	e280000c 	add	r0, r0, #12	; 0xc
    1bd0:	eb0034da 	bl	ef40 <xyzModem_stream_close>
    1bd4:	e3a00000 	mov	r0, #0	; 0x0
    1bd8:	e59f102c 	ldr	r1, [pc, #44]	; 1c0c <load_serial_ymodem+0xe0>
    1bdc:	eb003788 	bl	fa04 <xyzModem_stream_terminate>
    1be0:	e1a01005 	mov	r1, r5
    1be4:	e59f0024 	ldr	r0, [pc, #36]	; 1c10 <load_serial_ymodem+0xe4>
    1be8:	e1a02005 	mov	r2, r5
    1bec:	e59f3010 	ldr	r3, [pc, #16]	; 1c04 <load_serial_ymodem+0xd8>
    1bf0:	e1a0e00f 	mov	lr, pc
    1bf4:	e593f000 	ldr	pc, [r3]
    1bf8:	e1a00007 	mov	r0, r7
    1bfc:	e28dde41 	add	sp, sp, #1040	; 0x410
    1c00:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    1c04:	0800d764 	.word	0x0800d764
    1c08:	00011da8 	.word	0x00011da8
    1c0c:	00001c14 	.word	0x00001c14
    1c10:	00011cd0 	.word	0x00011cd0

00001c14 <getcxmodem>:
    1c14:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    1c18:	e24dd004 	sub	sp, sp, #4	; 0x4
    1c1c:	eb003179 	bl	e208 <tstc>
    1c20:	e3500000 	cmp	r0, #0	; 0x0
    1c24:	1a000002 	bne	1c34 <getcxmodem+0x20>
    1c28:	e3e00000 	mvn	r0, #0	; 0x0
    1c2c:	e28dd004 	add	sp, sp, #4	; 0x4
    1c30:	e8bd8000 	pop	{pc}
    1c34:	e28dd004 	add	sp, sp, #4	; 0x4
    1c38:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    1c3c:	ea003172 	b	e20c <getc>

00001c40 <do_load_serial_bin>:
    1c40:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1c44:	e3520001 	cmp	r2, #1	; 0x1
    1c48:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1c4c:	e1a06002 	mov	r6, r2
    1c50:	e1a07000 	mov	r7, r0
    1c54:	e1a05003 	mov	r5, r3
    1c58:	ca000006 	bgt	1c78 <do_load_serial_bin+0x38>
    1c5c:	e59f0120 	ldr	r0, [pc, #288]	; 1d84 <do_load_serial_bin+0x144>
    1c60:	e3a06000 	mov	r6, #0	; 0x0
    1c64:	e59f311c 	ldr	r3, [pc, #284]	; 1d88 <do_load_serial_bin+0x148>
    1c68:	e1a0e00f 	mov	lr, pc
    1c6c:	e593f000 	ldr	pc, [r3]
    1c70:	e1a04006 	mov	r4, r6
    1c74:	ea00000c 	b	1cac <do_load_serial_bin+0x6c>
    1c78:	e5930004 	ldr	r0, [r3, #4]
    1c7c:	e3a01000 	mov	r1, #0	; 0x0
    1c80:	e3a02010 	mov	r2, #16	; 0x10
    1c84:	eb002d48 	bl	d1ac <simple_strtoul>
    1c88:	e3560003 	cmp	r6, #3	; 0x3
    1c8c:	e1a04000 	mov	r4, r0
    1c90:	13a06000 	movne	r6, #0	; 0x0
    1c94:	1a000004 	bne	1cac <do_load_serial_bin+0x6c>
    1c98:	e5950008 	ldr	r0, [r5, #8]
    1c9c:	e3a01000 	mov	r1, #0	; 0x0
    1ca0:	e3a02010 	mov	r2, #16	; 0x10
    1ca4:	eb002d40 	bl	d1ac <simple_strtoul>
    1ca8:	e1a06000 	mov	r6, r0
    1cac:	e5950000 	ldr	r0, [r5]
    1cb0:	e59f10d4 	ldr	r1, [pc, #212]	; 1d8c <do_load_serial_bin+0x14c>
    1cb4:	ebfffd8a 	bl	12e4 <strcmp>
    1cb8:	e3500000 	cmp	r0, #0	; 0x0
    1cbc:	1a000007 	bne	1ce0 <do_load_serial_bin+0xa0>
    1cc0:	e1a01004 	mov	r1, r4
    1cc4:	e59f00c4 	ldr	r0, [pc, #196]	; 1d90 <do_load_serial_bin+0x150>
    1cc8:	e59f30b8 	ldr	r3, [pc, #184]	; 1d88 <do_load_serial_bin+0x148>
    1ccc:	e1a0e00f 	mov	lr, pc
    1cd0:	e593f000 	ldr	pc, [r3]
    1cd4:	e1a00004 	mov	r0, r4
    1cd8:	ebffff93 	bl	1b2c <load_serial_ymodem>
    1cdc:	ea000025 	b	1d78 <do_load_serial_bin+0x138>
    1ce0:	e5950000 	ldr	r0, [r5]
    1ce4:	e59f10a8 	ldr	r1, [pc, #168]	; 1d94 <do_load_serial_bin+0x154>
    1ce8:	ebfffd7d 	bl	12e4 <strcmp>
    1cec:	e3500000 	cmp	r0, #0	; 0x0
    1cf0:	e59f5090 	ldr	r5, [pc, #144]	; 1d88 <do_load_serial_bin+0x148>
    1cf4:	1a00001b 	bne	1d68 <do_load_serial_bin+0x128>
    1cf8:	e1a02006 	mov	r2, r6
    1cfc:	e59f0094 	ldr	r0, [pc, #148]	; 1d98 <do_load_serial_bin+0x158>
    1d00:	e1a01004 	mov	r1, r4
    1d04:	e1a0e00f 	mov	lr, pc
    1d08:	e595f000 	ldr	pc, [r5]
    1d0c:	e1a00004 	mov	r0, r4
    1d10:	e1a01006 	mov	r1, r6
    1d14:	eb00369d 	bl	f790 <control_send>
    1d18:	e3500000 	cmp	r0, #0	; 0x0
    1d1c:	aa000005 	bge	1d38 <do_load_serial_bin+0xf8>
    1d20:	e59d0004 	ldr	r0, [sp, #4]
    1d24:	e5954000 	ldr	r4, [r5]
    1d28:	eb00349c 	bl	efa0 <xyzModem_error>
    1d2c:	e1a01000 	mov	r1, r0
    1d30:	e59f0064 	ldr	r0, [pc, #100]	; 1d9c <do_load_serial_bin+0x15c>
    1d34:	e12fff34 	blx	r4
    1d38:	e28d0004 	add	r0, sp, #4	; 0x4
    1d3c:	eb00347f 	bl	ef40 <xyzModem_stream_close>
    1d40:	e3a00000 	mov	r0, #0	; 0x0
    1d44:	e59f1054 	ldr	r1, [pc, #84]	; 1da0 <do_load_serial_bin+0x160>
    1d48:	eb00372d 	bl	fa04 <xyzModem_stream_terminate>
    1d4c:	e1a01006 	mov	r1, r6
    1d50:	e59f004c 	ldr	r0, [pc, #76]	; 1da4 <do_load_serial_bin+0x164>
    1d54:	e1a02006 	mov	r2, r6
    1d58:	e59f3028 	ldr	r3, [pc, #40]	; 1d88 <do_load_serial_bin+0x148>
    1d5c:	e1a0e00f 	mov	lr, pc
    1d60:	e593f000 	ldr	pc, [r3]
    1d64:	ea000003 	b	1d78 <do_load_serial_bin+0x138>
    1d68:	e5971014 	ldr	r1, [r7, #20]
    1d6c:	e59f0034 	ldr	r0, [pc, #52]	; 1da8 <do_load_serial_bin+0x168>
    1d70:	e1a0e00f 	mov	lr, pc
    1d74:	e595f000 	ldr	pc, [r5]
    1d78:	e3a00000 	mov	r0, #0	; 0x0
    1d7c:	e28dd00c 	add	sp, sp, #12	; 0xc
    1d80:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1d84:	00011cf8 	.word	0x00011cf8
    1d88:	0800d764 	.word	0x0800d764
    1d8c:	00011d12 	.word	0x00011d12
    1d90:	00011d15 	.word	0x00011d15
    1d94:	00011d58 	.word	0x00011d58
    1d98:	00011d5b 	.word	0x00011d5b
    1d9c:	00011da8 	.word	0x00011da8
    1da0:	00001c14 	.word	0x00001c14
    1da4:	00011cd0 	.word	0x00011cd0
    1da8:	00011da7 	.word	0x00011da7

00001dac <do_mem_cp>:
    1dac:	e3520004 	cmp	r2, #4	; 0x4
    1db0:	e92d4070 	push	{r4, r5, r6, lr}
    1db4:	e1a04003 	mov	r4, r3
    1db8:	0a000002 	beq	1dc8 <do_mem_cp+0x1c>
    1dbc:	eb0003ad 	bl	2c78 <cmd_usage>
    1dc0:	e3a00001 	mov	r0, #1	; 0x1
    1dc4:	e8bd8070 	pop	{r4, r5, r6, pc}
    1dc8:	e5930004 	ldr	r0, [r3, #4]
    1dcc:	e3a01000 	mov	r1, #0	; 0x0
    1dd0:	e3a02010 	mov	r2, #16	; 0x10
    1dd4:	eb002cf4 	bl	d1ac <simple_strtoul>
    1dd8:	e3a01000 	mov	r1, #0	; 0x0
    1ddc:	e3a02010 	mov	r2, #16	; 0x10
    1de0:	e1a06000 	mov	r6, r0
    1de4:	e5940008 	ldr	r0, [r4, #8]
    1de8:	eb002cef 	bl	d1ac <simple_strtoul>
    1dec:	e3a01000 	mov	r1, #0	; 0x0
    1df0:	e1a05000 	mov	r5, r0
    1df4:	e3a02010 	mov	r2, #16	; 0x10
    1df8:	e594000c 	ldr	r0, [r4, #12]
    1dfc:	eb002cea 	bl	d1ac <simple_strtoul>
    1e00:	e2800003 	add	r0, r0, #3	; 0x3
    1e04:	e1b00120 	lsrs	r0, r0, #2
    1e08:	1a000002 	bne	1e18 <do_mem_cp+0x6c>
    1e0c:	ea000006 	b	1e2c <do_mem_cp+0x80>
    1e10:	e5163004 	ldr	r3, [r6, #-4]
    1e14:	e5053004 	str	r3, [r5, #-4]
    1e18:	e3500000 	cmp	r0, #0	; 0x0
    1e1c:	e2866004 	add	r6, r6, #4	; 0x4
    1e20:	e2400001 	sub	r0, r0, #1	; 0x1
    1e24:	e2855004 	add	r5, r5, #4	; 0x4
    1e28:	1afffff8 	bne	1e10 <do_mem_cp+0x64>
    1e2c:	e3a00000 	mov	r0, #0	; 0x0
    1e30:	e8bd8070 	pop	{r4, r5, r6, pc}

00001e34 <do_mem_cmp>:
    1e34:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1e38:	e3520004 	cmp	r2, #4	; 0x4
    1e3c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1e40:	e1a04003 	mov	r4, r3
    1e44:	0a000002 	beq	1e54 <do_mem_cmp+0x20>
    1e48:	eb00038a 	bl	2c78 <cmd_usage>
    1e4c:	e3a04001 	mov	r4, #1	; 0x1
    1e50:	ea000029 	b	1efc <do_mem_cmp+0xc8>
    1e54:	e5930004 	ldr	r0, [r3, #4]
    1e58:	e3a01000 	mov	r1, #0	; 0x0
    1e5c:	e3a02010 	mov	r2, #16	; 0x10
    1e60:	eb002cd1 	bl	d1ac <simple_strtoul>
    1e64:	e3a01000 	mov	r1, #0	; 0x0
    1e68:	e3a02010 	mov	r2, #16	; 0x10
    1e6c:	e1a07000 	mov	r7, r0
    1e70:	e5940008 	ldr	r0, [r4, #8]
    1e74:	eb002ccc 	bl	d1ac <simple_strtoul>
    1e78:	e3a01000 	mov	r1, #0	; 0x0
    1e7c:	e1a06000 	mov	r6, r0
    1e80:	e3a02010 	mov	r2, #16	; 0x10
    1e84:	e594000c 	ldr	r0, [r4, #12]
    1e88:	eb002cc7 	bl	d1ac <simple_strtoul>
    1e8c:	e2800003 	add	r0, r0, #3	; 0x3
    1e90:	e1a00120 	lsr	r0, r0, #2
    1e94:	e3a05000 	mov	r5, #0	; 0x0
    1e98:	ea00000f 	b	1edc <do_mem_cmp+0xa8>
    1e9c:	e5972000 	ldr	r2, [r7]
    1ea0:	e5963000 	ldr	r3, [r6]
    1ea4:	e1520003 	cmp	r2, r3
    1ea8:	0a000008 	beq	1ed0 <do_mem_cmp+0x9c>
    1eac:	e58d3000 	str	r3, [sp]
    1eb0:	e1a01007 	mov	r1, r7
    1eb4:	e1a03006 	mov	r3, r6
    1eb8:	e59f0048 	ldr	r0, [pc, #72]	; 1f08 <do_mem_cmp+0xd4>
    1ebc:	e59fc048 	ldr	ip, [pc, #72]	; 1f0c <do_mem_cmp+0xd8>
    1ec0:	e1a0e00f 	mov	lr, pc
    1ec4:	e59cf000 	ldr	pc, [ip]
    1ec8:	e3a04001 	mov	r4, #1	; 0x1
    1ecc:	ea000005 	b	1ee8 <do_mem_cmp+0xb4>
    1ed0:	e2855001 	add	r5, r5, #1	; 0x1
    1ed4:	e2877004 	add	r7, r7, #4	; 0x4
    1ed8:	e2866004 	add	r6, r6, #4	; 0x4
    1edc:	e1500005 	cmp	r0, r5
    1ee0:	1affffed 	bne	1e9c <do_mem_cmp+0x68>
    1ee4:	e3a04000 	mov	r4, #0	; 0x0
    1ee8:	e1a01005 	mov	r1, r5
    1eec:	e59f001c 	ldr	r0, [pc, #28]	; 1f10 <do_mem_cmp+0xdc>
    1ef0:	e59f3014 	ldr	r3, [pc, #20]	; 1f0c <do_mem_cmp+0xd8>
    1ef4:	e1a0e00f 	mov	lr, pc
    1ef8:	e593f000 	ldr	pc, [r3]
    1efc:	e1a00004 	mov	r0, r4
    1f00:	e28dd00c 	add	sp, sp, #12	; 0xc
    1f04:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1f08:	00011ead 	.word	0x00011ead
    1f0c:	0800d764 	.word	0x0800d764
    1f10:	00011ee5 	.word	0x00011ee5

00001f14 <do_mem_mw>:
    1f14:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1f18:	e1a06002 	mov	r6, r2
    1f1c:	e2422003 	sub	r2, r2, #3	; 0x3
    1f20:	e3520001 	cmp	r2, #1	; 0x1
    1f24:	e24dd004 	sub	sp, sp, #4	; 0x4
    1f28:	e1a04003 	mov	r4, r3
    1f2c:	9a000002 	bls	1f3c <do_mem_mw+0x28>
    1f30:	eb000350 	bl	2c78 <cmd_usage>
    1f34:	e3a00001 	mov	r0, #1	; 0x1
    1f38:	ea000016 	b	1f98 <do_mem_mw+0x84>
    1f3c:	e3a01000 	mov	r1, #0	; 0x0
    1f40:	e3a02010 	mov	r2, #16	; 0x10
    1f44:	e5930004 	ldr	r0, [r3, #4]
    1f48:	eb002c97 	bl	d1ac <simple_strtoul>
    1f4c:	e3a01000 	mov	r1, #0	; 0x0
    1f50:	e1a05000 	mov	r5, r0
    1f54:	e3a02010 	mov	r2, #16	; 0x10
    1f58:	e5940008 	ldr	r0, [r4, #8]
    1f5c:	eb002c92 	bl	d1ac <simple_strtoul>
    1f60:	e3560004 	cmp	r6, #4	; 0x4
    1f64:	e1a07000 	mov	r7, r0
    1f68:	13a00001 	movne	r0, #1	; 0x1
    1f6c:	1a000006 	bne	1f8c <do_mem_mw+0x78>
    1f70:	e594000c 	ldr	r0, [r4, #12]
    1f74:	e3a01000 	mov	r1, #0	; 0x0
    1f78:	e3a02010 	mov	r2, #16	; 0x10
    1f7c:	eb002c8a 	bl	d1ac <simple_strtoul>
    1f80:	ea000001 	b	1f8c <do_mem_mw+0x78>
    1f84:	e5057004 	str	r7, [r5, #-4]
    1f88:	e2400001 	sub	r0, r0, #1	; 0x1
    1f8c:	e3500000 	cmp	r0, #0	; 0x0
    1f90:	e2855004 	add	r5, r5, #4	; 0x4
    1f94:	1afffffa 	bne	1f84 <do_mem_mw+0x70>
    1f98:	e28dd004 	add	sp, sp, #4	; 0x4
    1f9c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00001fa0 <do_mem_md>:
    1fa0:	e92d4070 	push	{r4, r5, r6, lr}
    1fa4:	e3520001 	cmp	r2, #1	; 0x1
    1fa8:	e24dd008 	sub	sp, sp, #8	; 0x8
    1fac:	e1a04002 	mov	r4, r2
    1fb0:	e1a05003 	mov	r5, r3
    1fb4:	ca000002 	bgt	1fc4 <do_mem_md+0x24>
    1fb8:	eb00032e 	bl	2c78 <cmd_usage>
    1fbc:	e3a00001 	mov	r0, #1	; 0x1
    1fc0:	ea000014 	b	2018 <do_mem_md+0x78>
    1fc4:	e5930004 	ldr	r0, [r3, #4]
    1fc8:	e3a01000 	mov	r1, #0	; 0x0
    1fcc:	e3a02010 	mov	r2, #16	; 0x10
    1fd0:	eb002c75 	bl	d1ac <simple_strtoul>
    1fd4:	e3540002 	cmp	r4, #2	; 0x2
    1fd8:	e1a06000 	mov	r6, r0
    1fdc:	03a030a0 	moveq	r3, #160	; 0xa0
    1fe0:	0a000004 	beq	1ff8 <do_mem_md+0x58>
    1fe4:	e5950008 	ldr	r0, [r5, #8]
    1fe8:	e3a01000 	mov	r1, #0	; 0x0
    1fec:	e3a02010 	mov	r2, #16	; 0x10
    1ff0:	eb002c6d 	bl	d1ac <simple_strtoul>
    1ff4:	e1a03000 	mov	r3, r0
    1ff8:	e3a0c004 	mov	ip, #4	; 0x4
    1ffc:	e1a00006 	mov	r0, r6
    2000:	e1a0200c 	mov	r2, ip
    2004:	e1a03123 	lsr	r3, r3, #2
    2008:	e1a01006 	mov	r1, r6
    200c:	e58dc000 	str	ip, [sp]
    2010:	eb002bcd 	bl	cf4c <print_buffer>
    2014:	e3a00000 	mov	r0, #0	; 0x0
    2018:	e28dd008 	add	sp, sp, #8	; 0x8
    201c:	e8bd8070 	pop	{r4, r5, r6, pc}

00002020 <do_sc>:
    2020:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    2024:	e3520002 	cmp	r2, #2	; 0x2
    2028:	e24ddf47 	sub	sp, sp, #284	; 0x11c
    202c:	0a000002 	beq	203c <do_sc+0x1c>
    2030:	eb000310 	bl	2c78 <cmd_usage>
    2034:	e3a00001 	mov	r0, #1	; 0x1
    2038:	ea000060 	b	21c0 <do_sc+0x1a0>
    203c:	e5930004 	ldr	r0, [r3, #4]
    2040:	e3a01000 	mov	r1, #0	; 0x0
    2044:	e3a02010 	mov	r2, #16	; 0x10
    2048:	eb002c57 	bl	d1ac <simple_strtoul>
    204c:	e28d4f41 	add	r4, sp, #260	; 0x104
    2050:	e2844002 	add	r4, r4, #2	; 0x2
    2054:	e59f116c 	ldr	r1, [pc, #364]	; 21c8 <do_sc+0x1a8>
    2058:	e1a06000 	mov	r6, r0
    205c:	e3a02012 	mov	r2, #18	; 0x12
    2060:	e1a00004 	mov	r0, r4
    2064:	ebfffce5 	bl	1400 <memcpy>
    2068:	e3a01000 	mov	r1, #0	; 0x0
    206c:	ea000004 	b	2084 <do_sc+0x64>
    2070:	e7d12006 	ldrb	r2, [r1, r6]
    2074:	e7d43001 	ldrb	r3, [r4, r1]
    2078:	e2811001 	add	r1, r1, #1	; 0x1
    207c:	e1520003 	cmp	r2, r3
    2080:	1a000014 	bne	20d8 <do_sc+0xb8>
    2084:	e3510012 	cmp	r1, #18	; 0x12
    2088:	e0815006 	add	r5, r1, r6
    208c:	1afffff7 	bne	2070 <do_sc+0x50>
    2090:	e2851eff 	add	r1, r5, #4080	; 0xff0
    2094:	e281100f 	add	r1, r1, #15	; 0xf
    2098:	e1a02005 	mov	r2, r5
    209c:	e5d23000 	ldrb	r3, [r2]
    20a0:	e353000a 	cmp	r3, #10	; 0xa
    20a4:	1a000005 	bne	20c0 <do_sc+0xa0>
    20a8:	e5d23001 	ldrb	r3, [r2, #1]
    20ac:	e353005d 	cmp	r3, #93	; 0x5d
    20b0:	1a000002 	bne	20c0 <do_sc+0xa0>
    20b4:	e5d23002 	ldrb	r3, [r2, #2]
    20b8:	e353005d 	cmp	r3, #93	; 0x5d
    20bc:	0a000003 	beq	20d0 <do_sc+0xb0>
    20c0:	e1520001 	cmp	r2, r1
    20c4:	e2822001 	add	r2, r2, #1	; 0x1
    20c8:	1afffff3 	bne	209c <do_sc+0x7c>
    20cc:	ea000001 	b	20d8 <do_sc+0xb8>
    20d0:	e3550000 	cmp	r5, #0	; 0x0
    20d4:	1a000006 	bne	20f4 <do_sc+0xd4>
    20d8:	e59f00ec 	ldr	r0, [pc, #236]	; 21cc <do_sc+0x1ac>
    20dc:	e1a01006 	mov	r1, r6
    20e0:	e59f30e8 	ldr	r3, [pc, #232]	; 21d0 <do_sc+0x1b0>
    20e4:	e1a0e00f 	mov	lr, pc
    20e8:	e593f000 	ldr	pc, [r3]
    20ec:	e3e00000 	mvn	r0, #0	; 0x0
    20f0:	ea000032 	b	21c0 <do_sc+0x1a0>
    20f4:	e59f40d4 	ldr	r4, [pc, #212]	; 21d0 <do_sc+0x1b0>
    20f8:	e1a01006 	mov	r1, r6
    20fc:	e59f00d0 	ldr	r0, [pc, #208]	; 21d4 <do_sc+0x1b4>
    2100:	e28d6008 	add	r6, sp, #8	; 0x8
    2104:	e1a0e00f 	mov	lr, pc
    2108:	e594f000 	ldr	pc, [r4]
    210c:	e2466002 	sub	r6, r6, #2	; 0x2
    2110:	e1a07004 	mov	r7, r4
    2114:	ea000000 	b	211c <do_sc+0xfc>
    2118:	e2855001 	add	r5, r5, #1	; 0x1
    211c:	e5d53000 	ldrb	r3, [r5]
    2120:	e353002d 	cmp	r3, #45	; 0x2d
    2124:	1353000a 	cmpne	r3, #10	; 0xa
    2128:	0afffffa 	beq	2118 <do_sc+0xf8>
    212c:	e353005b 	cmp	r3, #91	; 0x5b
    2130:	0afffff8 	beq	2118 <do_sc+0xf8>
    2134:	e353005d 	cmp	r3, #93	; 0x5d
    2138:	0a000001 	beq	2144 <do_sc+0x124>
    213c:	e1a02006 	mov	r2, r6
    2140:	ea000005 	b	215c <do_sc+0x13c>
    2144:	e5d53001 	ldrb	r3, [r5, #1]
    2148:	e353005d 	cmp	r3, #93	; 0x5d
    214c:	1afffffa 	bne	213c <do_sc+0x11c>
    2150:	ea00000c 	b	2188 <do_sc+0x168>
    2154:	e5423001 	strb	r3, [r2, #-1]
    2158:	e2855001 	add	r5, r5, #1	; 0x1
    215c:	e5d53000 	ldrb	r3, [r5]
    2160:	e1a01002 	mov	r1, r2
    2164:	e353000a 	cmp	r3, #10	; 0xa
    2168:	1353005d 	cmpne	r3, #93	; 0x5d
    216c:	03a04000 	moveq	r4, #0	; 0x0
    2170:	13a04001 	movne	r4, #1	; 0x1
    2174:	e2822001 	add	r2, r2, #1	; 0x1
    2178:	1afffff5 	bne	2154 <do_sc+0x134>
    217c:	e3550000 	cmp	r5, #0	; 0x0
    2180:	e5c14000 	strb	r4, [r1]
    2184:	1a000005 	bne	21a0 <do_sc+0x180>
    2188:	e59f0048 	ldr	r0, [pc, #72]	; 21d8 <do_sc+0x1b8>
    218c:	e59f303c 	ldr	r3, [pc, #60]	; 21d0 <do_sc+0x1b0>
    2190:	e1a0e00f 	mov	lr, pc
    2194:	e593f000 	ldr	pc, [r3]
    2198:	e3a00000 	mov	r0, #0	; 0x0
    219c:	ea000007 	b	21c0 <do_sc+0x1a0>
    21a0:	e1a01006 	mov	r1, r6
    21a4:	e59f0030 	ldr	r0, [pc, #48]	; 21dc <do_sc+0x1bc>
    21a8:	e1a0e00f 	mov	lr, pc
    21ac:	e597f000 	ldr	pc, [r7]
    21b0:	e1a00006 	mov	r0, r6
    21b4:	e1a01004 	mov	r1, r4
    21b8:	eb00304f 	bl	e2fc <run_command>
    21bc:	eaffffd6 	b	211c <do_sc+0xfc>
    21c0:	e28ddf47 	add	sp, sp, #284	; 0x11c
    21c4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    21c8:	000118a0 	.word	0x000118a0
    21cc:	00011faf 	.word	0x00011faf
    21d0:	0800d764 	.word	0x0800d764
    21d4:	00011fcc 	.word	0x00011fcc
    21d8:	00011fe5 	.word	0x00011fe5
    21dc:	00011ff2 	.word	0x00011ff2

000021e0 <do_ls>:
    21e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    21e4:	e59f001c 	ldr	r0, [pc, #28]	; 2208 <do_ls+0x28>
    21e8:	e24dd004 	sub	sp, sp, #4	; 0x4
    21ec:	e59f1018 	ldr	r1, [pc, #24]	; 220c <do_ls+0x2c>
    21f0:	e59f3018 	ldr	r3, [pc, #24]	; 2210 <do_ls+0x30>
    21f4:	e1a0e00f 	mov	lr, pc
    21f8:	e593f000 	ldr	pc, [r3]
    21fc:	e3a00000 	mov	r0, #0	; 0x0
    2200:	e28dd004 	add	sp, sp, #4	; 0x4
    2204:	e8bd8000 	pop	{pc}
    2208:	00011da8 	.word	0x00011da8
    220c:	0800d108 	.word	0x0800d108
    2210:	0800d764 	.word	0x0800d764

00002214 <do_linkpc>:
    2214:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2218:	e24dd004 	sub	sp, sp, #4	; 0x4
    221c:	eb002964 	bl	c7b4 <linkpc_loop>
    2220:	e3a00000 	mov	r0, #0	; 0x0
    2224:	e28dd004 	add	sp, sp, #4	; 0x4
    2228:	e8bd8000 	pop	{pc}

0000222c <do_pads>:
    222c:	e3520002 	cmp	r2, #2	; 0x2
    2230:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    2234:	d59f0194 	ldrle	r0, [pc, #404]	; 23d0 <do_pads+0x1a4>
    2238:	e1a05002 	mov	r5, r2
    223c:	e1a06003 	mov	r6, r3
    2240:	da000015 	ble	229c <do_pads+0x70>
    2244:	e5930008 	ldr	r0, [r3, #8]
    2248:	e3a01000 	mov	r1, #0	; 0x0
    224c:	e3a0200a 	mov	r2, #10	; 0xa
    2250:	eb002bd5 	bl	d1ac <simple_strtoul>
    2254:	e3550003 	cmp	r5, #3	; 0x3
    2258:	e1a04000 	mov	r4, r0
    225c:	01a07000 	moveq	r7, r0
    2260:	0a000004 	beq	2278 <do_pads+0x4c>
    2264:	e596000c 	ldr	r0, [r6, #12]
    2268:	e3a01000 	mov	r1, #0	; 0x0
    226c:	e3a0200a 	mov	r2, #10	; 0xa
    2270:	eb002bcd 	bl	d1ac <simple_strtoul>
    2274:	e1a07000 	mov	r7, r0
    2278:	e1943007 	orrs	r3, r4, r7
    227c:	4a000005 	bmi	2298 <do_pads+0x6c>
    2280:	e1a05004 	mov	r5, r4
    2284:	e2866004 	add	r6, r6, #4	; 0x4
    2288:	e59f9144 	ldr	r9, [pc, #324]	; 23d4 <do_pads+0x1a8>
    228c:	e59f8144 	ldr	r8, [pc, #324]	; 23d8 <do_pads+0x1ac>
    2290:	e59fa144 	ldr	sl, [pc, #324]	; 23dc <do_pads+0x1b0>
    2294:	ea000049 	b	23c0 <do_pads+0x194>
    2298:	e59f0140 	ldr	r0, [pc, #320]	; 23e0 <do_pads+0x1b4>
    229c:	e59f3134 	ldr	r3, [pc, #308]	; 23d8 <do_pads+0x1ac>
    22a0:	e1a0e00f 	mov	lr, pc
    22a4:	e593f000 	ldr	pc, [r3]
    22a8:	ea000046 	b	23c8 <do_pads+0x19c>
    22ac:	e5960000 	ldr	r0, [r6]
    22b0:	e59f111c 	ldr	r1, [pc, #284]	; 23d4 <do_pads+0x1a8>
    22b4:	e3a02002 	mov	r2, #2	; 0x2
    22b8:	ebfffc17 	bl	131c <strncmp>
    22bc:	e3500000 	cmp	r0, #0	; 0x0
    22c0:	1a00000b 	bne	22f4 <do_pads+0xc8>
    22c4:	e1a02000 	mov	r2, r0
    22c8:	e3a01001 	mov	r1, #1	; 0x1
    22cc:	e1a00005 	mov	r0, r5
    22d0:	ebfffa5b 	bl	c44 <pads_chmod>
    22d4:	e1a00005 	mov	r0, r5
    22d8:	e5984000 	ldr	r4, [r8]
    22dc:	ebfffad4 	bl	e34 <pads_getpin>
    22e0:	e1a01005 	mov	r1, r5
    22e4:	e1a02000 	mov	r2, r0
    22e8:	e59f00f4 	ldr	r0, [pc, #244]	; 23e4 <do_pads+0x1b8>
    22ec:	e12fff34 	blx	r4
    22f0:	ea000031 	b	23bc <do_pads+0x190>
    22f4:	e59f10ec 	ldr	r1, [pc, #236]	; 23e8 <do_pads+0x1bc>
    22f8:	e3a02004 	mov	r2, #4	; 0x4
    22fc:	e5960000 	ldr	r0, [r6]
    2300:	ebfffc05 	bl	131c <strncmp>
    2304:	e3500000 	cmp	r0, #0	; 0x0
    2308:	01a00005 	moveq	r0, r5
    230c:	03a01002 	moveq	r1, #2	; 0x2
    2310:	03a02001 	moveq	r2, #1	; 0x1
    2314:	0a000011 	beq	2360 <do_pads+0x134>
    2318:	e59f10cc 	ldr	r1, [pc, #204]	; 23ec <do_pads+0x1c0>
    231c:	e3a02003 	mov	r2, #3	; 0x3
    2320:	e5960000 	ldr	r0, [r6]
    2324:	ebfffbfc 	bl	131c <strncmp>
    2328:	e3500000 	cmp	r0, #0	; 0x0
    232c:	01a02000 	moveq	r2, r0
    2330:	03a01002 	moveq	r1, #2	; 0x2
    2334:	01a00005 	moveq	r0, r5
    2338:	0a000008 	beq	2360 <do_pads+0x134>
    233c:	e5960000 	ldr	r0, [r6]
    2340:	e59f10a8 	ldr	r1, [pc, #168]	; 23f0 <do_pads+0x1c4>
    2344:	e3a02004 	mov	r2, #4	; 0x4
    2348:	ebfffbf3 	bl	131c <strncmp>
    234c:	e3500000 	cmp	r0, #0	; 0x0
    2350:	1a000004 	bne	2368 <do_pads+0x13c>
    2354:	e1a01000 	mov	r1, r0
    2358:	e1a02001 	mov	r2, r1
    235c:	e1a00005 	mov	r0, r5
    2360:	ebfffa37 	bl	c44 <pads_chmod>
    2364:	ea000014 	b	23bc <do_pads+0x190>
    2368:	e5960000 	ldr	r0, [r6]
    236c:	e59f1080 	ldr	r1, [pc, #128]	; 23f4 <do_pads+0x1c8>
    2370:	e3a02002 	mov	r2, #2	; 0x2
    2374:	ebfffbe8 	bl	131c <strncmp>
    2378:	e3500000 	cmp	r0, #0	; 0x0
    237c:	1a00000c 	bne	23b4 <do_pads+0x188>
    2380:	e1a00005 	mov	r0, r5
    2384:	ebfffa18 	bl	bec <pads_states>
    2388:	e3500000 	cmp	r0, #0	; 0x0
    238c:	e5983000 	ldr	r3, [r8]
    2390:	059f2060 	ldreq	r2, [pc, #96]	; 23f8 <do_pads+0x1cc>
    2394:	0a000002 	beq	23a4 <do_pads+0x178>
    2398:	e3500001 	cmp	r0, #1	; 0x1
    239c:	01a02009 	moveq	r2, r9
    23a0:	11a0200a 	movne	r2, sl
    23a4:	e59f0050 	ldr	r0, [pc, #80]	; 23fc <do_pads+0x1d0>
    23a8:	e1a01005 	mov	r1, r5
    23ac:	e12fff33 	blx	r3
    23b0:	ea000001 	b	23bc <do_pads+0x190>
    23b4:	e59f0044 	ldr	r0, [pc, #68]	; 2400 <do_pads+0x1d4>
    23b8:	eaffffb7 	b	229c <do_pads+0x70>
    23bc:	e2855001 	add	r5, r5, #1	; 0x1
    23c0:	e1550007 	cmp	r5, r7
    23c4:	daffffb8 	ble	22ac <do_pads+0x80>
    23c8:	e3a00000 	mov	r0, #0	; 0x0
    23cc:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    23d0:	00012016 	.word	0x00012016
    23d4:	00012053 	.word	0x00012053
    23d8:	0800d764 	.word	0x0800d764
    23dc:	000143cc 	.word	0x000143cc
    23e0:	0001202e 	.word	0x0001202e
    23e4:	00012056 	.word	0x00012056
    23e8:	0001206a 	.word	0x0001206a
    23ec:	0001206f 	.word	0x0001206f
    23f0:	00012073 	.word	0x00012073
    23f4:	00012078 	.word	0x00012078
    23f8:	0001207b 	.word	0x0001207b
    23fc:	00012084 	.word	0x00012084
    2400:	00012099 	.word	0x00012099

00002404 <do_mux>:
    2404:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2408:	e3520001 	cmp	r2, #1	; 0x1
    240c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2410:	da000005 	ble	242c <do_mux+0x28>
    2414:	e5930004 	ldr	r0, [r3, #4]
    2418:	e3a01000 	mov	r1, #0	; 0x0
    241c:	e3a0200a 	mov	r2, #10	; 0xa
    2420:	eb002b61 	bl	d1ac <simple_strtoul>
    2424:	e6ff0070 	uxth	r0, r0
    2428:	ebfffb7b 	bl	121c <set_mux>
    242c:	e59f301c 	ldr	r3, [pc, #28]	; 2450 <do_mux+0x4c>
    2430:	e59f001c 	ldr	r0, [pc, #28]	; 2454 <do_mux+0x50>
    2434:	e5931000 	ldr	r1, [r3]
    2438:	e59f3018 	ldr	r3, [pc, #24]	; 2458 <do_mux+0x54>
    243c:	e1a0e00f 	mov	lr, pc
    2440:	e593f000 	ldr	pc, [r3]
    2444:	e3a00000 	mov	r0, #0	; 0x0
    2448:	e28dd004 	add	sp, sp, #4	; 0x4
    244c:	e8bd8000 	pop	{pc}
    2450:	2d009000 	.word	0x2d009000
    2454:	000120b2 	.word	0x000120b2
    2458:	0800d764 	.word	0x0800d764

0000245c <do_mac>:
    245c:	e59f3020 	ldr	r3, [pc, #32]	; 2484 <do_mac+0x28>
    2460:	e92d4010 	push	{r4, lr}
    2464:	e5934000 	ldr	r4, [r3]
    2468:	ebfff96d 	bl	a24 <efuse_mac>
    246c:	e1a02000 	mov	r2, r0
    2470:	e1a03001 	mov	r3, r1
    2474:	e59f000c 	ldr	r0, [pc, #12]	; 2488 <do_mac+0x2c>
    2478:	e12fff34 	blx	r4
    247c:	e3a00000 	mov	r0, #0	; 0x0
    2480:	e8bd8010 	pop	{r4, pc}
    2484:	0800d764 	.word	0x0800d764
    2488:	000120c4 	.word	0x000120c4

0000248c <do_boot>:
    248c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2490:	e3a00000 	mov	r0, #0	; 0x0
    2494:	e24dd004 	sub	sp, sp, #4	; 0x4
    2498:	ebfff7d2 	bl	3e8 <try_boot>
    249c:	e3a00000 	mov	r0, #0	; 0x0
    24a0:	e28dd004 	add	sp, sp, #4	; 0x4
    24a4:	e8bd8000 	pop	{pc}

000024a8 <do_efuse>:
    24a8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    24ac:	e3520001 	cmp	r2, #1	; 0x1
    24b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    24b4:	da000006 	ble	24d4 <do_efuse+0x2c>
    24b8:	e5930004 	ldr	r0, [r3, #4]
    24bc:	e59f1024 	ldr	r1, [pc, #36]	; 24e8 <do_efuse+0x40>
    24c0:	e3a02002 	mov	r2, #2	; 0x2
    24c4:	ebfffb94 	bl	131c <strncmp>
    24c8:	e3500000 	cmp	r0, #0	; 0x0
    24cc:	02800001 	addeq	r0, r0, #1	; 0x1
    24d0:	0a000000 	beq	24d8 <do_efuse+0x30>
    24d4:	e3a00000 	mov	r0, #0	; 0x0
    24d8:	ebfff954 	bl	a30 <efuse_info>
    24dc:	e3a00000 	mov	r0, #0	; 0x0
    24e0:	e28dd004 	add	sp, sp, #4	; 0x4
    24e4:	e8bd8000 	pop	{pc}
    24e8:	000120dc 	.word	0x000120dc

000024ec <do_pi>:
    24ec:	e92d4030 	push	{r4, r5, lr}
    24f0:	e3520001 	cmp	r2, #1	; 0x1
    24f4:	e24dd004 	sub	sp, sp, #4	; 0x4
    24f8:	e1a04002 	mov	r4, r2
    24fc:	e1a05003 	mov	r5, r3
    2500:	da000015 	ble	255c <do_pi+0x70>
    2504:	e59f10ac 	ldr	r1, [pc, #172]	; 25b8 <do_pi+0xcc>
    2508:	e5930004 	ldr	r0, [r3, #4]
    250c:	e3a02004 	mov	r2, #4	; 0x4
    2510:	ebfffb81 	bl	131c <strncmp>
    2514:	e2501000 	subs	r1, r0, #0	; 0x0
    2518:	1a00000d 	bne	2554 <do_pi+0x68>
    251c:	e3540002 	cmp	r4, #2	; 0x2
    2520:	01a04001 	moveq	r4, r1
    2524:	0a000003 	beq	2538 <do_pi+0x4c>
    2528:	e5950008 	ldr	r0, [r5, #8]
    252c:	e3a02010 	mov	r2, #16	; 0x10
    2530:	eb002b1d 	bl	d1ac <simple_strtoul>
    2534:	e1a04000 	mov	r4, r0
    2538:	e59f007c 	ldr	r0, [pc, #124]	; 25bc <do_pi+0xd0>
    253c:	e1a01004 	mov	r1, r4
    2540:	e59f3078 	ldr	r3, [pc, #120]	; 25c0 <do_pi+0xd4>
    2544:	e1a0e00f 	mov	lr, pc
    2548:	e593f000 	ldr	pc, [r3]
    254c:	e1a00004 	mov	r0, r4
    2550:	eb001097 	bl	67b4 <burn>
    2554:	e3a04000 	mov	r4, #0	; 0x0
    2558:	ea000013 	b	25ac <do_pi+0xc0>
    255c:	e3a00441 	mov	r0, #1090519040	; 0x41000000
    2560:	eb002ea5 	bl	dffc <ius_check_header>
    2564:	e2504000 	subs	r4, r0, #0	; 0x0
    2568:	159f0054 	ldrne	r0, [pc, #84]	; 25c4 <do_pi+0xd8>
    256c:	1a000005 	bne	2588 <do_pi+0x9c>
    2570:	e3a0000d 	mov	r0, #13	; 0xd
    2574:	e3a01001 	mov	r1, #1	; 0x1
    2578:	eb000afb 	bl	516c <vs_assign_by_id>
    257c:	e2504000 	subs	r4, r0, #0	; 0x0
    2580:	0a000005 	beq	259c <do_pi+0xb0>
    2584:	e59f003c 	ldr	r0, [pc, #60]	; 25c8 <do_pi+0xdc>
    2588:	e1a01004 	mov	r1, r4
    258c:	e59f302c 	ldr	r3, [pc, #44]	; 25c0 <do_pi+0xd4>
    2590:	e1a0e00f 	mov	lr, pc
    2594:	e593f000 	ldr	pc, [r3]
    2598:	ea000003 	b	25ac <do_pi+0xc0>
    259c:	e3a00441 	mov	r0, #1090519040	; 0x41000000
    25a0:	e3a0100d 	mov	r1, #13	; 0xd
    25a4:	e1a02004 	mov	r2, r4
    25a8:	eb001008 	bl	65d0 <burn_images>
    25ac:	e1a00004 	mov	r0, r4
    25b0:	e28dd004 	add	sp, sp, #4	; 0x4
    25b4:	e8bd8030 	pop	{r4, r5, pc}
    25b8:	000120df 	.word	0x000120df
    25bc:	000120e4 	.word	0x000120e4
    25c0:	0800d764 	.word	0x0800d764
    25c4:	0001210e 	.word	0x0001210e
    25c8:	0001212e 	.word	0x0001212e

000025cc <do_spbyte>:
    25cc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    25d0:	e5930004 	ldr	r0, [r3, #4]
    25d4:	e24dd004 	sub	sp, sp, #4	; 0x4
    25d8:	e3a01000 	mov	r1, #0	; 0x0
    25dc:	e3a0200a 	mov	r2, #10	; 0xa
    25e0:	eb002af1 	bl	d1ac <simple_strtoul>
    25e4:	eb001b38 	bl	92cc <spi_set_bytes>
    25e8:	e3a00000 	mov	r0, #0	; 0x0
    25ec:	e28dd004 	add	sp, sp, #4	; 0x4
    25f0:	e8bd8000 	pop	{pc}

000025f4 <do_cfg>:
    25f4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    25f8:	e5930004 	ldr	r0, [r3, #4]
    25fc:	e24dd004 	sub	sp, sp, #4	; 0x4
    2600:	e3a01000 	mov	r1, #0	; 0x0
    2604:	e3a02010 	mov	r2, #16	; 0x10
    2608:	eb002ae7 	bl	d1ac <simple_strtoul>
    260c:	eb000f2a 	bl	62bc <cdbg_config_isi>
    2610:	e3a00000 	mov	r0, #0	; 0x0
    2614:	e28dd004 	add	sp, sp, #4	; 0x4
    2618:	e8bd8000 	pop	{pc}

0000261c <do_sleep>:
    261c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2620:	e3a01000 	mov	r1, #0	; 0x0
    2624:	e24dd004 	sub	sp, sp, #4	; 0x4
    2628:	e3a0200a 	mov	r2, #10	; 0xa
    262c:	e5930004 	ldr	r0, [r3, #4]
    2630:	eb002add 	bl	d1ac <simple_strtoul>
    2634:	e59f3020 	ldr	r3, [pc, #32]	; 265c <do_sleep+0x40>
    2638:	e0000093 	mul	r0, r3, r0
    263c:	eb0018b2 	bl	890c <udelay>
    2640:	e59f0018 	ldr	r0, [pc, #24]	; 2660 <do_sleep+0x44>
    2644:	e59f3018 	ldr	r3, [pc, #24]	; 2664 <do_sleep+0x48>
    2648:	e1a0e00f 	mov	lr, pc
    264c:	e593f000 	ldr	pc, [r3]
    2650:	e3a00000 	mov	r0, #0	; 0x0
    2654:	e28dd004 	add	sp, sp, #4	; 0x4
    2658:	e8bd8000 	pop	{pc}
    265c:	000f4240 	.word	0x000f4240
    2660:	00012149 	.word	0x00012149
    2664:	0800d764 	.word	0x0800d764

00002668 <do_go>:
    2668:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    266c:	e5930004 	ldr	r0, [r3, #4]
    2670:	e24dd004 	sub	sp, sp, #4	; 0x4
    2674:	e3a02010 	mov	r2, #16	; 0x10
    2678:	e3a01000 	mov	r1, #0	; 0x0
    267c:	eb002aca 	bl	d1ac <simple_strtoul>
    2680:	e3a01000 	mov	r1, #0	; 0x0
    2684:	eb000e97 	bl	60e8 <cdbg_jump>
    2688:	e3a00000 	mov	r0, #0	; 0x0
    268c:	e28dd004 	add	sp, sp, #4	; 0x4
    2690:	e8bd8000 	pop	{pc}

00002694 <do_ml>:
    2694:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2698:	e24dd004 	sub	sp, sp, #4	; 0x4
    269c:	ebfffc4d 	bl	17d8 <__sOINFo>
    26a0:	e3a00000 	mov	r0, #0	; 0x0
    26a4:	e28dd004 	add	sp, sp, #4	; 0x4
    26a8:	e8bd8000 	pop	{pc}

000026ac <do_bp>:
    26ac:	e92d4030 	push	{r4, r5, lr}
    26b0:	e3520001 	cmp	r2, #1	; 0x1
    26b4:	e24dd004 	sub	sp, sp, #4	; 0x4
    26b8:	da000005 	ble	26d4 <do_bp+0x28>
    26bc:	e5930004 	ldr	r0, [r3, #4]
    26c0:	e3a01000 	mov	r1, #0	; 0x0
    26c4:	e3a02010 	mov	r2, #16	; 0x10
    26c8:	eb002ab7 	bl	d1ac <simple_strtoul>
    26cc:	e6ff0070 	uxth	r0, r0
    26d0:	ebfff7aa 	bl	580 <set_xom>
    26d4:	e59f5034 	ldr	r5, [pc, #52]	; 2710 <do_bp+0x64>
    26d8:	e3a00001 	mov	r0, #1	; 0x1
    26dc:	e5954000 	ldr	r4, [r5]
    26e0:	ebfff7a2 	bl	570 <get_xom>
    26e4:	e1a01000 	mov	r1, r0
    26e8:	e59f0024 	ldr	r0, [pc, #36]	; 2714 <do_bp+0x68>
    26ec:	e12fff34 	blx	r4
    26f0:	e5954000 	ldr	r4, [r5]
    26f4:	ebfff7a8 	bl	59c <boot_info>
    26f8:	e1a01000 	mov	r1, r0
    26fc:	e59f0014 	ldr	r0, [pc, #20]	; 2718 <do_bp+0x6c>
    2700:	e12fff34 	blx	r4
    2704:	e3a00000 	mov	r0, #0	; 0x0
    2708:	e28dd004 	add	sp, sp, #4	; 0x4
    270c:	e8bd8030 	pop	{r4, r5, pc}
    2710:	0800d764 	.word	0x0800d764
    2714:	00012154 	.word	0x00012154
    2718:	0001216a 	.word	0x0001216a

0000271c <do_reset>:
    271c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2720:	e24dd004 	sub	sp, sp, #4	; 0x4
    2724:	ebfff833 	bl	7f8 <system_reset>
    2728:	e3a00000 	mov	r0, #0	; 0x0
    272c:	e28dd004 	add	sp, sp, #4	; 0x4
    2730:	e8bd8000 	pop	{pc}

00002734 <output_debug>:
    2734:	e24dd010 	sub	sp, sp, #16	; 0x10
    2738:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    273c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2740:	e28dc008 	add	ip, sp, #8	; 0x8
    2744:	e3700002 	cmn	r0, #2	; 0x2
    2748:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    274c:	e1a01000 	mov	r1, r0
    2750:	0a000012 	beq	27a0 <output_debug+0x6c>
    2754:	e3700001 	cmn	r0, #1	; 0x1
    2758:	0a000008 	beq	2780 <output_debug+0x4c>
    275c:	e3700003 	cmn	r0, #3	; 0x3
    2760:	1a000010 	bne	27a8 <output_debug+0x74>
    2764:	e59f30a4 	ldr	r3, [pc, #164]	; 2810 <output_debug+0xdc>
    2768:	e59f00a4 	ldr	r0, [pc, #164]	; 2814 <output_debug+0xe0>
    276c:	e5931000 	ldr	r1, [r3]
    2770:	e59f30a0 	ldr	r3, [pc, #160]	; 2818 <output_debug+0xe4>
    2774:	e1a0e00f 	mov	lr, pc
    2778:	e593f000 	ldr	pc, [r3]
    277c:	ea00001e 	b	27fc <output_debug+0xc8>
    2780:	e59f3088 	ldr	r3, [pc, #136]	; 2810 <output_debug+0xdc>
    2784:	e59f0090 	ldr	r0, [pc, #144]	; 281c <output_debug+0xe8>
    2788:	e5932000 	ldr	r2, [r3]
    278c:	e28d1010 	add	r1, sp, #16	; 0x10
    2790:	e59f3080 	ldr	r3, [pc, #128]	; 2818 <output_debug+0xe4>
    2794:	e1a0e00f 	mov	lr, pc
    2798:	e593f000 	ldr	pc, [r3]
    279c:	ea000016 	b	27fc <output_debug+0xc8>
    27a0:	e59f0078 	ldr	r0, [pc, #120]	; 2820 <output_debug+0xec>
    27a4:	ea000003 	b	27b8 <output_debug+0x84>
    27a8:	e59d300c 	ldr	r3, [sp, #12]
    27ac:	e3530001 	cmp	r3, #1	; 0x1
    27b0:	1a000004 	bne	27c8 <output_debug+0x94>
    27b4:	e59f0068 	ldr	r0, [pc, #104]	; 2824 <output_debug+0xf0>
    27b8:	e59f3058 	ldr	r3, [pc, #88]	; 2818 <output_debug+0xe4>
    27bc:	e1a0e00f 	mov	lr, pc
    27c0:	e593f000 	ldr	pc, [r3]
    27c4:	ea00000c 	b	27fc <output_debug+0xc8>
    27c8:	e3530002 	cmp	r3, #2	; 0x2
    27cc:	e59fc044 	ldr	ip, [pc, #68]	; 2818 <output_debug+0xe4>
    27d0:	1a000003 	bne	27e4 <output_debug+0xb0>
    27d4:	e59f004c 	ldr	r0, [pc, #76]	; 2828 <output_debug+0xf4>
    27d8:	e1a0e00f 	mov	lr, pc
    27dc:	e59cf000 	ldr	pc, [ip]
    27e0:	ea000005 	b	27fc <output_debug+0xc8>
    27e4:	e59f3024 	ldr	r3, [pc, #36]	; 2810 <output_debug+0xdc>
    27e8:	e59f003c 	ldr	r0, [pc, #60]	; 282c <output_debug+0xf8>
    27ec:	e5933000 	ldr	r3, [r3]
    27f0:	e28d2010 	add	r2, sp, #16	; 0x10
    27f4:	e1a0e00f 	mov	lr, pc
    27f8:	e59cf000 	ldr	pc, [ip]
    27fc:	e3a00000 	mov	r0, #0	; 0x0
    2800:	e28dd004 	add	sp, sp, #4	; 0x4
    2804:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    2808:	e28dd010 	add	sp, sp, #16	; 0x10
    280c:	e12fff1e 	bx	lr
    2810:	0800d1b0 	.word	0x0800d1b0
    2814:	000122f2 	.word	0x000122f2
    2818:	0800d764 	.word	0x0800d764
    281c:	0001231a 	.word	0x0001231a
    2820:	00012329 	.word	0x00012329
    2824:	00012343 	.word	0x00012343
    2828:	0001234e 	.word	0x0001234e
    282c:	0001235c 	.word	0x0001235c

00002830 <do_vs>:
    2830:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    2834:	e3520006 	cmp	r2, #6	; 0x6
    2838:	e24dd024 	sub	sp, sp, #36	; 0x24
    283c:	e1a06000 	mov	r6, r0
    2840:	e1a04003 	mov	r4, r3
    2844:	1a000005 	bne	2860 <do_vs+0x30>
    2848:	e5930014 	ldr	r0, [r3, #20]
    284c:	e3a01000 	mov	r1, #0	; 0x0
    2850:	e282200a 	add	r2, r2, #10	; 0xa
    2854:	eb002a54 	bl	d1ac <simple_strtoul>
    2858:	e1a05000 	mov	r5, r0
    285c:	ea00008f 	b	2aa0 <do_vs+0x270>
    2860:	e2423001 	sub	r3, r2, #1	; 0x1
    2864:	e3530005 	cmp	r3, #5	; 0x5
    2868:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    286c:	ea0000c4 	b	2b84 <do_vs+0x354>
    2870:	00002b84 	.word	0x00002b84
    2874:	00002888 	.word	0x00002888
    2878:	00002980 	.word	0x00002980
    287c:	000029f0 	.word	0x000029f0
    2880:	00002a9c 	.word	0x00002a9c
    2884:	00002a9c 	.word	0x00002a9c
    2888:	e5940004 	ldr	r0, [r4, #4]
    288c:	e59f1310 	ldr	r1, [pc, #784]	; 2ba4 <do_vs+0x374>
    2890:	ebfffa93 	bl	12e4 <strcmp>
    2894:	e2505000 	subs	r5, r0, #0	; 0x0
    2898:	1a000025 	bne	2934 <do_vs+0x104>
    289c:	e59f4304 	ldr	r4, [pc, #772]	; 2ba8 <do_vs+0x378>
    28a0:	e59f0304 	ldr	r0, [pc, #772]	; 2bac <do_vs+0x37c>
    28a4:	e1a0e00f 	mov	lr, pc
    28a8:	e594f000 	ldr	pc, [r4]
    28ac:	e3a0802c 	mov	r8, #44	; 0x2c
    28b0:	e59f72f8 	ldr	r7, [pc, #760]	; 2bb0 <do_vs+0x380>
    28b4:	ea000003 	b	28c8 <do_vs+0x98>
    28b8:	e0227598 	mla	r2, r8, r5, r7
    28bc:	e1a0e00f 	mov	lr, pc
    28c0:	e594f000 	ldr	pc, [r4]
    28c4:	e2855001 	add	r5, r5, #1	; 0x1
    28c8:	eb000951 	bl	4e14 <vs_device_count>
    28cc:	e1550000 	cmp	r5, r0
    28d0:	e1a01005 	mov	r1, r5
    28d4:	e59f02d8 	ldr	r0, [pc, #728]	; 2bb4 <do_vs+0x384>
    28d8:	e59f62c8 	ldr	r6, [pc, #712]	; 2ba8 <do_vs+0x378>
    28dc:	bafffff5 	blt	28b8 <do_vs+0x88>
    28e0:	e59f02d0 	ldr	r0, [pc, #720]	; 2bb8 <do_vs+0x388>
    28e4:	e1a0e00f 	mov	lr, pc
    28e8:	e596f000 	ldr	pc, [r6]
    28ec:	eb00084e 	bl	4a2c <vs_is_assigned>
    28f0:	e2504000 	subs	r4, r0, #0	; 0x0
    28f4:	0a000009 	beq	2920 <do_vs+0xf0>
    28f8:	e59f32bc 	ldr	r3, [pc, #700]	; 2bbc <do_vs+0x38c>
    28fc:	e5965000 	ldr	r5, [r6]
    2900:	e5934000 	ldr	r4, [r3]
    2904:	e1a00004 	mov	r0, r4
    2908:	eb0008c2 	bl	4c18 <vs_align>
    290c:	e1a01004 	mov	r1, r4
    2910:	e1a02000 	mov	r2, r0
    2914:	e59f02a4 	ldr	r0, [pc, #676]	; 2bc0 <do_vs+0x390>
    2918:	e12fff35 	blx	r5
    291c:	ea00009d 	b	2b98 <do_vs+0x368>
    2920:	e59f029c 	ldr	r0, [pc, #668]	; 2bc4 <do_vs+0x394>
    2924:	e1a0e00f 	mov	lr, pc
    2928:	e596f000 	ldr	pc, [r6]
    292c:	e1a00004 	mov	r0, r4
    2930:	ea000099 	b	2b9c <do_vs+0x36c>
    2934:	e5940004 	ldr	r0, [r4, #4]
    2938:	e59f1288 	ldr	r1, [pc, #648]	; 2bc8 <do_vs+0x398>
    293c:	ebfffa68 	bl	12e4 <strcmp>
    2940:	e2505000 	subs	r5, r0, #0	; 0x0
    2944:	1a000087 	bne	2b68 <do_vs+0x338>
    2948:	eb0009f8 	bl	5130 <vs_reset>
    294c:	e28d4020 	add	r4, sp, #32	; 0x20
    2950:	e5240018 	str	r0, [r4, #-24]!
    2954:	e59f126c 	ldr	r1, [pc, #620]	; 2bc8 <do_vs+0x398>
    2958:	e2840008 	add	r0, r4, #8	; 0x8
    295c:	ebfffa36 	bl	123c <strcpy>
    2960:	e3a03001 	mov	r3, #1	; 0x1
    2964:	e58d300c 	str	r3, [sp, #12]
    2968:	e28d3018 	add	r3, sp, #24	; 0x18
    296c:	e8930003 	ldm	r3, {r0, r1}
    2970:	e88d0003 	stm	sp, {r0, r1}
    2974:	e894000f 	ldm	r4, {r0, r1, r2, r3}
    2978:	ebffff6d 	bl	2734 <output_debug>
    297c:	ea000019 	b	29e8 <do_vs+0x1b8>
    2980:	e5940004 	ldr	r0, [r4, #4]
    2984:	e59f1240 	ldr	r1, [pc, #576]	; 2bcc <do_vs+0x39c>
    2988:	ebfffa55 	bl	12e4 <strcmp>
    298c:	e2505000 	subs	r5, r0, #0	; 0x0
    2990:	1a000074 	bne	2b68 <do_vs+0x338>
    2994:	e5940008 	ldr	r0, [r4, #8]
    2998:	e3a01001 	mov	r1, #1	; 0x1
    299c:	eb000a10 	bl	51e4 <vs_assign_by_name>
    29a0:	e2507000 	subs	r7, r0, #0	; 0x0
    29a4:	e59f61fc 	ldr	r6, [pc, #508]	; 2ba8 <do_vs+0x378>
    29a8:	1a00000b 	bne	29dc <do_vs+0x1ac>
    29ac:	e5943008 	ldr	r3, [r4, #8]
    29b0:	e59f2218 	ldr	r2, [pc, #536]	; 2bd0 <do_vs+0x3a0>
    29b4:	e1a00003 	mov	r0, r3
    29b8:	e5823000 	str	r3, [r2]
    29bc:	eb000916 	bl	4e1c <vs_device_id>
    29c0:	e59f31f4 	ldr	r3, [pc, #500]	; 2bbc <do_vs+0x38c>
    29c4:	e5830000 	str	r0, [r3]
    29c8:	e59f0204 	ldr	r0, [pc, #516]	; 2bd4 <do_vs+0x3a4>
    29cc:	e1a0e00f 	mov	lr, pc
    29d0:	e596f000 	ldr	pc, [r6]
    29d4:	e1a00007 	mov	r0, r7
    29d8:	ea00006f 	b	2b9c <do_vs+0x36c>
    29dc:	e59f01f4 	ldr	r0, [pc, #500]	; 2bd8 <do_vs+0x3a8>
    29e0:	e1a0e00f 	mov	lr, pc
    29e4:	e596f000 	ldr	pc, [r6]
    29e8:	e1a00005 	mov	r0, r5
    29ec:	ea00006a 	b	2b9c <do_vs+0x36c>
    29f0:	e3a02010 	mov	r2, #16	; 0x10
    29f4:	e5940008 	ldr	r0, [r4, #8]
    29f8:	e3a01000 	mov	r1, #0	; 0x0
    29fc:	eb002a42 	bl	d30c <simple_strtoull>
    2a00:	e3a02010 	mov	r2, #16	; 0x10
    2a04:	e1a07000 	mov	r7, r0
    2a08:	e1a08001 	mov	r8, r1
    2a0c:	e594000c 	ldr	r0, [r4, #12]
    2a10:	e3a01000 	mov	r1, #0	; 0x0
    2a14:	eb002a3c 	bl	d30c <simple_strtoull>
    2a18:	e59f11bc 	ldr	r1, [pc, #444]	; 2bdc <do_vs+0x3ac>
    2a1c:	e1a05000 	mov	r5, r0
    2a20:	e28d0010 	add	r0, sp, #16	; 0x10
    2a24:	ebfffa04 	bl	123c <strcpy>
    2a28:	e3a03002 	mov	r3, #2	; 0x2
    2a2c:	e5940004 	ldr	r0, [r4, #4]
    2a30:	e59f11a4 	ldr	r1, [pc, #420]	; 2bdc <do_vs+0x3ac>
    2a34:	e58d300c 	str	r3, [sp, #12]
    2a38:	ebfffa29 	bl	12e4 <strcmp>
    2a3c:	e3500000 	cmp	r0, #0	; 0x0
    2a40:	1a000004 	bne	2a58 <do_vs+0x228>
    2a44:	e1a00007 	mov	r0, r7
    2a48:	e1a01008 	mov	r1, r8
    2a4c:	e1a02005 	mov	r2, r5
    2a50:	eb000834 	bl	4b28 <vs_erase>
    2a54:	ea000008 	b	2a7c <do_vs+0x24c>
    2a58:	e5940004 	ldr	r0, [r4, #4]
    2a5c:	e59f117c 	ldr	r1, [pc, #380]	; 2be0 <do_vs+0x3b0>
    2a60:	ebfffa1f 	bl	12e4 <strcmp>
    2a64:	e3500000 	cmp	r0, #0	; 0x0
    2a68:	1a00003e 	bne	2b68 <do_vs+0x338>
    2a6c:	e1a00007 	mov	r0, r7
    2a70:	e1a01008 	mov	r1, r8
    2a74:	e1a02005 	mov	r2, r5
    2a78:	eb000848 	bl	4ba0 <vs_scrub>
    2a7c:	e58d0008 	str	r0, [sp, #8]
    2a80:	e28d2018 	add	r2, sp, #24	; 0x18
    2a84:	e8920003 	ldm	r2, {r0, r1}
    2a88:	e88d0003 	stm	sp, {r0, r1}
    2a8c:	e28d3008 	add	r3, sp, #8	; 0x8
    2a90:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    2a94:	ebffff26 	bl	2734 <output_debug>
    2a98:	ea00003e 	b	2b98 <do_vs+0x368>
    2a9c:	e3a05000 	mov	r5, #0	; 0x0
    2aa0:	e3a01000 	mov	r1, #0	; 0x0
    2aa4:	e3a02010 	mov	r2, #16	; 0x10
    2aa8:	e5940008 	ldr	r0, [r4, #8]
    2aac:	eb0029be 	bl	d1ac <simple_strtoul>
    2ab0:	e3a02010 	mov	r2, #16	; 0x10
    2ab4:	e1a09000 	mov	r9, r0
    2ab8:	e3a01000 	mov	r1, #0	; 0x0
    2abc:	e594000c 	ldr	r0, [r4, #12]
    2ac0:	eb002a11 	bl	d30c <simple_strtoull>
    2ac4:	e3a02010 	mov	r2, #16	; 0x10
    2ac8:	e1a08000 	mov	r8, r0
    2acc:	e1a0a001 	mov	sl, r1
    2ad0:	e5940010 	ldr	r0, [r4, #16]
    2ad4:	e3a01000 	mov	r1, #0	; 0x0
    2ad8:	e08db002 	add	fp, sp, r2
    2adc:	eb002a0a 	bl	d30c <simple_strtoull>
    2ae0:	e3a03000 	mov	r3, #0	; 0x0
    2ae4:	e59f10f8 	ldr	r1, [pc, #248]	; 2be4 <do_vs+0x3b4>
    2ae8:	e1a07000 	mov	r7, r0
    2aec:	e1a0000b 	mov	r0, fp
    2af0:	e58d300c 	str	r3, [sp, #12]
    2af4:	ebfff9d0 	bl	123c <strcpy>
    2af8:	e5940004 	ldr	r0, [r4, #4]
    2afc:	e59f10e4 	ldr	r1, [pc, #228]	; 2be8 <do_vs+0x3b8>
    2b00:	ebfff9f7 	bl	12e4 <strcmp>
    2b04:	e3500000 	cmp	r0, #0	; 0x0
    2b08:	1a000006 	bne	2b28 <do_vs+0x2f8>
    2b0c:	e1a00009 	mov	r0, r9
    2b10:	e1a02008 	mov	r2, r8
    2b14:	e1a0300a 	mov	r3, sl
    2b18:	e58d7000 	str	r7, [sp]
    2b1c:	e58d5004 	str	r5, [sp, #4]
    2b20:	eb0007ea 	bl	4ad0 <vs_write>
    2b24:	eaffffd4 	b	2a7c <do_vs+0x24c>
    2b28:	e5940004 	ldr	r0, [r4, #4]
    2b2c:	e59f10b8 	ldr	r1, [pc, #184]	; 2bec <do_vs+0x3bc>
    2b30:	ebfff9eb 	bl	12e4 <strcmp>
    2b34:	e3500000 	cmp	r0, #0	; 0x0
    2b38:	1a00000a 	bne	2b68 <do_vs+0x338>
    2b3c:	e1a00009 	mov	r0, r9
    2b40:	e1a02008 	mov	r2, r8
    2b44:	e1a0300a 	mov	r3, sl
    2b48:	e58d7000 	str	r7, [sp]
    2b4c:	e58d5004 	str	r5, [sp, #4]
    2b50:	eb0008de 	bl	4ed0 <vs_read>
    2b54:	e59f1094 	ldr	r1, [pc, #148]	; 2bf0 <do_vs+0x3c0>
    2b58:	e58d0008 	str	r0, [sp, #8]
    2b5c:	e1a0000b 	mov	r0, fp
    2b60:	ebfff9b5 	bl	123c <strcpy>
    2b64:	eaffffc5 	b	2a80 <do_vs+0x250>
    2b68:	e59f0084 	ldr	r0, [pc, #132]	; 2bf4 <do_vs+0x3c4>
    2b6c:	e5961014 	ldr	r1, [r6, #20]
    2b70:	e59f3030 	ldr	r3, [pc, #48]	; 2ba8 <do_vs+0x378>
    2b74:	e1a0e00f 	mov	lr, pc
    2b78:	e593f000 	ldr	pc, [r3]
    2b7c:	e3e00000 	mvn	r0, #0	; 0x0
    2b80:	ea000005 	b	2b9c <do_vs+0x36c>
    2b84:	e5901014 	ldr	r1, [r0, #20]
    2b88:	e59f3018 	ldr	r3, [pc, #24]	; 2ba8 <do_vs+0x378>
    2b8c:	e59f0060 	ldr	r0, [pc, #96]	; 2bf4 <do_vs+0x3c4>
    2b90:	e1a0e00f 	mov	lr, pc
    2b94:	e593f000 	ldr	pc, [r3]
    2b98:	e3a00000 	mov	r0, #0	; 0x0
    2b9c:	e28dd024 	add	sp, sp, #36	; 0x24
    2ba0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    2ba4:	00012199 	.word	0x00012199
    2ba8:	0800d764 	.word	0x0800d764
    2bac:	00012373 	.word	0x00012373
    2bb0:	00010fa0 	.word	0x00010fa0
    2bb4:	00012381 	.word	0x00012381
    2bb8:	00012389 	.word	0x00012389
    2bbc:	0800d1b0 	.word	0x0800d1b0
    2bc0:	0001239b 	.word	0x0001239b
    2bc4:	000123a5 	.word	0x000123a5
    2bc8:	00012187 	.word	0x00012187
    2bcc:	000141a0 	.word	0x000141a0
    2bd0:	0800d1b4 	.word	0x0800d1b4
    2bd4:	000123b8 	.word	0x000123b8
    2bd8:	000123c4 	.word	0x000123c4
    2bdc:	000123f1 	.word	0x000123f1
    2be0:	000123f7 	.word	0x000123f7
    2be4:	000123fd 	.word	0x000123fd
    2be8:	00012406 	.word	0x00012406
    2bec:	0001240c 	.word	0x0001240c
    2bf0:	00012411 	.word	0x00012411
    2bf4:	00011da7 	.word	0x00011da7

00002bf8 <cmd_get_data_size>:
    2bf8:	e92d4030 	push	{r4, r5, lr}
    2bfc:	e24dd004 	sub	sp, sp, #4	; 0x4
    2c00:	e1a04001 	mov	r4, r1
    2c04:	e1a05000 	mov	r5, r0
    2c08:	ebfff9e2 	bl	1398 <strlen>
    2c0c:	e3500002 	cmp	r0, #2	; 0x2
    2c10:	da000015 	ble	2c6c <cmd_get_data_size+0x74>
    2c14:	e0850000 	add	r0, r5, r0
    2c18:	e5503002 	ldrb	r3, [r0, #-2]
    2c1c:	e353002e 	cmp	r3, #46	; 0x2e
    2c20:	1a000011 	bne	2c6c <cmd_get_data_size+0x74>
    2c24:	e5500001 	ldrb	r0, [r0, #-1]
    2c28:	e350006c 	cmp	r0, #108	; 0x6c
    2c2c:	03a04004 	moveq	r4, #4	; 0x4
    2c30:	0a00000d 	beq	2c6c <cmd_get_data_size+0x74>
    2c34:	8a000002 	bhi	2c44 <cmd_get_data_size+0x4c>
    2c38:	e3500062 	cmp	r0, #98	; 0x62
    2c3c:	1a000009 	bne	2c68 <cmd_get_data_size+0x70>
    2c40:	ea000006 	b	2c60 <cmd_get_data_size+0x68>
    2c44:	e3500073 	cmp	r0, #115	; 0x73
    2c48:	03e04001 	mvneq	r4, #1	; 0x1
    2c4c:	0a000006 	beq	2c6c <cmd_get_data_size+0x74>
    2c50:	e3500077 	cmp	r0, #119	; 0x77
    2c54:	03a04002 	moveq	r4, #2	; 0x2
    2c58:	1a000002 	bne	2c68 <cmd_get_data_size+0x70>
    2c5c:	ea000002 	b	2c6c <cmd_get_data_size+0x74>
    2c60:	e3a04001 	mov	r4, #1	; 0x1
    2c64:	ea000000 	b	2c6c <cmd_get_data_size+0x74>
    2c68:	e3e04000 	mvn	r4, #0	; 0x0
    2c6c:	e1a00004 	mov	r0, r4
    2c70:	e28dd004 	add	sp, sp, #4	; 0x4
    2c74:	e8bd8030 	pop	{r4, r5, pc}

00002c78 <cmd_usage>:
    2c78:	e92d4030 	push	{r4, r5, lr}
    2c7c:	e1a04000 	mov	r4, r0
    2c80:	e24dd004 	sub	sp, sp, #4	; 0x4
    2c84:	e59f5054 	ldr	r5, [pc, #84]	; 2ce0 <cmd_usage+0x68>
    2c88:	e59f0054 	ldr	r0, [pc, #84]	; 2ce4 <cmd_usage+0x6c>
    2c8c:	e5941000 	ldr	r1, [r4]
    2c90:	e5942010 	ldr	r2, [r4, #16]
    2c94:	e1a0e00f 	mov	lr, pc
    2c98:	e595f000 	ldr	pc, [r5]
    2c9c:	e59f0044 	ldr	r0, [pc, #68]	; 2ce8 <cmd_usage+0x70>
    2ca0:	e5941000 	ldr	r1, [r4]
    2ca4:	e1a0e00f 	mov	lr, pc
    2ca8:	e595f000 	ldr	pc, [r5]
    2cac:	e5940014 	ldr	r0, [r4, #20]
    2cb0:	e3500000 	cmp	r0, #0	; 0x0
    2cb4:	1a000003 	bne	2cc8 <cmd_usage+0x50>
    2cb8:	e59f002c 	ldr	r0, [pc, #44]	; 2cec <cmd_usage+0x74>
    2cbc:	eb002d28 	bl	e164 <puts>
    2cc0:	e3a00001 	mov	r0, #1	; 0x1
    2cc4:	ea000003 	b	2cd8 <cmd_usage+0x60>
    2cc8:	eb002d25 	bl	e164 <puts>
    2ccc:	e3a0000a 	mov	r0, #10	; 0xa
    2cd0:	eb002d4b 	bl	e204 <putc>
    2cd4:	e3a00000 	mov	r0, #0	; 0x0
    2cd8:	e28dd004 	add	sp, sp, #4	; 0x4
    2cdc:	e8bd8030 	pop	{r4, r5, pc}
    2ce0:	0800d764 	.word	0x0800d764
    2ce4:	00012506 	.word	0x00012506
    2ce8:	00012510 	.word	0x00012510
    2cec:	0001251b 	.word	0x0001251b

00002cf0 <find_cmd_tbl>:
    2cf0:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    2cf4:	e1a05001 	mov	r5, r1
    2cf8:	e24dd004 	sub	sp, sp, #4	; 0x4
    2cfc:	e3a0102e 	mov	r1, #46	; 0x2e
    2d00:	e1a07000 	mov	r7, r0
    2d04:	e1a08002 	mov	r8, r2
    2d08:	ebfff997 	bl	136c <strchr>
    2d0c:	e3500000 	cmp	r0, #0	; 0x0
    2d10:	10676000 	rsbne	r6, r7, r0
    2d14:	1a000002 	bne	2d24 <find_cmd_tbl+0x34>
    2d18:	e1a00007 	mov	r0, r7
    2d1c:	ebfff99d 	bl	1398 <strlen>
    2d20:	e1a06000 	mov	r6, r0
    2d24:	e3a03018 	mov	r3, #24	; 0x18
    2d28:	e02a5893 	mla	sl, r3, r8, r5
    2d2c:	e1a04005 	mov	r4, r5
    2d30:	e1a08005 	mov	r8, r5
    2d34:	e3a05000 	mov	r5, #0	; 0x0
    2d38:	ea00000a 	b	2d68 <find_cmd_tbl+0x78>
    2d3c:	e5941000 	ldr	r1, [r4]
    2d40:	ebfff975 	bl	131c <strncmp>
    2d44:	e3500000 	cmp	r0, #0	; 0x0
    2d48:	1a000005 	bne	2d64 <find_cmd_tbl+0x74>
    2d4c:	e5940000 	ldr	r0, [r4]
    2d50:	ebfff990 	bl	1398 <strlen>
    2d54:	e1560000 	cmp	r6, r0
    2d58:	0a000009 	beq	2d84 <find_cmd_tbl+0x94>
    2d5c:	e2855001 	add	r5, r5, #1	; 0x1
    2d60:	e1a08004 	mov	r8, r4
    2d64:	e2844018 	add	r4, r4, #24	; 0x18
    2d68:	e154000a 	cmp	r4, sl
    2d6c:	e1a02006 	mov	r2, r6
    2d70:	e1a00007 	mov	r0, r7
    2d74:	1afffff0 	bne	2d3c <find_cmd_tbl+0x4c>
    2d78:	e3550001 	cmp	r5, #1	; 0x1
    2d7c:	01a04008 	moveq	r4, r8
    2d80:	13a04000 	movne	r4, #0	; 0x0
    2d84:	e1a00004 	mov	r0, r4
    2d88:	e28dd004 	add	sp, sp, #4	; 0x4
    2d8c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00002d90 <find_cmd>:
    2d90:	e59f1014 	ldr	r1, [pc, #20]	; 2dac <find_cmd+0x1c>
    2d94:	e59f2014 	ldr	r2, [pc, #20]	; 2db0 <find_cmd+0x20>
    2d98:	e59f3014 	ldr	r3, [pc, #20]	; 2db4 <find_cmd+0x24>
    2d9c:	e0612002 	rsb	r2, r1, r2
    2da0:	e1a021c2 	asr	r2, r2, #3
    2da4:	e0020293 	mul	r2, r3, r2
    2da8:	eaffffd0 	b	2cf0 <find_cmd_tbl>
    2dac:	00014510 	.word	0x00014510
    2db0:	000147e0 	.word	0x000147e0
    2db4:	aaaaaaab 	.word	0xaaaaaaab

00002db8 <_do_help>:
    2db8:	e1a0c00d 	mov	ip, sp
    2dbc:	e92ddff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}
    2dc0:	e24cb004 	sub	fp, ip, #4	; 0x4
    2dc4:	e24dd004 	sub	sp, sp, #4	; 0x4
    2dc8:	e59b8004 	ldr	r8, [fp, #4]
    2dcc:	e1a07000 	mov	r7, r0
    2dd0:	e3580001 	cmp	r8, #1	; 0x1
    2dd4:	e1a0a001 	mov	sl, r1
    2dd8:	159b5008 	ldrne	r5, [fp, #8]
    2ddc:	13a06001 	movne	r6, #1	; 0x1
    2de0:	13a04000 	movne	r4, #0	; 0x0
    2de4:	159f9158 	ldrne	r9, [pc, #344]	; 2f44 <_do_help+0x18c>
    2de8:	1a00004e 	bne	2f28 <_do_help+0x170>
    2dec:	e1a03101 	lsl	r3, r1, #2
    2df0:	e283300e 	add	r3, r3, #14	; 0xe
    2df4:	e3c33007 	bic	r3, r3, #7	; 0x7
    2df8:	e1a0900d 	mov	r9, sp
    2dfc:	e063d00d 	rsb	sp, r3, sp
    2e00:	e1a0800d 	mov	r8, sp
    2e04:	e3a03000 	mov	r3, #0	; 0x0
    2e08:	ea000002 	b	2e18 <_do_help+0x60>
    2e0c:	e7880103 	str	r0, [r8, r3, lsl #2]
    2e10:	e2800018 	add	r0, r0, #24	; 0x18
    2e14:	e2833001 	add	r3, r3, #1	; 0x1
    2e18:	e153000a 	cmp	r3, sl
    2e1c:	bafffffa 	blt	2e0c <_do_help+0x54>
    2e20:	e24a7001 	sub	r7, sl, #1	; 0x1
    2e24:	ea000015 	b	2e80 <_do_help+0xc8>
    2e28:	e5943000 	ldr	r3, [r4]
    2e2c:	e5942004 	ldr	r2, [r4, #4]
    2e30:	e5930000 	ldr	r0, [r3]
    2e34:	e5921000 	ldr	r1, [r2]
    2e38:	ebfff929 	bl	12e4 <strcmp>
    2e3c:	e3500000 	cmp	r0, #0	; 0x0
    2e40:	c5942000 	ldrgt	r2, [r4]
    2e44:	c5943004 	ldrgt	r3, [r4, #4]
    2e48:	e2855001 	add	r5, r5, #1	; 0x1
    2e4c:	c5843000 	strgt	r3, [r4]
    2e50:	c5842004 	strgt	r2, [r4, #4]
    2e54:	c2866001 	addgt	r6, r6, #1	; 0x1
    2e58:	e1550007 	cmp	r5, r7
    2e5c:	e2844004 	add	r4, r4, #4	; 0x4
    2e60:	bafffff0 	blt	2e28 <_do_help+0x70>
    2e64:	e3560000 	cmp	r6, #0	; 0x0
    2e68:	1a000003 	bne	2e7c <_do_help+0xc4>
    2e6c:	e3a05000 	mov	r5, #0	; 0x0
    2e70:	e1a04005 	mov	r4, r5
    2e74:	e59f70c8 	ldr	r7, [pc, #200]	; 2f44 <_do_help+0x18c>
    2e78:	ea000016 	b	2ed8 <_do_help+0x120>
    2e7c:	e2477001 	sub	r7, r7, #1	; 0x1
    2e80:	e3570000 	cmp	r7, #0	; 0x0
    2e84:	dafffff8 	ble	2e6c <_do_help+0xb4>
    2e88:	e3a05000 	mov	r5, #0	; 0x0
    2e8c:	e1a04008 	mov	r4, r8
    2e90:	e1a06005 	mov	r6, r5
    2e94:	eaffffe3 	b	2e28 <_do_help+0x70>
    2e98:	e7983004 	ldr	r3, [r8, r4]
    2e9c:	e5936010 	ldr	r6, [r3, #16]
    2ea0:	eb002cda 	bl	e210 <ctrlc>
    2ea4:	e3500000 	cmp	r0, #0	; 0x0
    2ea8:	e1a03006 	mov	r3, r6
    2eac:	e3a01008 	mov	r1, #8	; 0x8
    2eb0:	e59f0090 	ldr	r0, [pc, #144]	; 2f48 <_do_help+0x190>
    2eb4:	13a04001 	movne	r4, #1	; 0x1
    2eb8:	1a00000a 	bne	2ee8 <_do_help+0x130>
    2ebc:	e3560000 	cmp	r6, #0	; 0x0
    2ec0:	0a000003 	beq	2ed4 <_do_help+0x11c>
    2ec4:	e7982004 	ldr	r2, [r8, r4]
    2ec8:	e5922000 	ldr	r2, [r2]
    2ecc:	e1a0e00f 	mov	lr, pc
    2ed0:	e597f000 	ldr	pc, [r7]
    2ed4:	e2844004 	add	r4, r4, #4	; 0x4
    2ed8:	e155000a 	cmp	r5, sl
    2edc:	e2855001 	add	r5, r5, #1	; 0x1
    2ee0:	baffffec 	blt	2e98 <_do_help+0xe0>
    2ee4:	e3a04000 	mov	r4, #0	; 0x0
    2ee8:	e1a0d009 	mov	sp, r9
    2eec:	ea000011 	b	2f38 <_do_help+0x180>
    2ef0:	e5950004 	ldr	r0, [r5, #4]
    2ef4:	ebffff7d 	bl	2cf0 <find_cmd_tbl>
    2ef8:	e3500000 	cmp	r0, #0	; 0x0
    2efc:	0a000002 	beq	2f0c <_do_help+0x154>
    2f00:	ebffff5c 	bl	2c78 <cmd_usage>
    2f04:	e1844000 	orr	r4, r4, r0
    2f08:	ea000004 	b	2f20 <_do_help+0x168>
    2f0c:	e5951004 	ldr	r1, [r5, #4]
    2f10:	e59f0034 	ldr	r0, [pc, #52]	; 2f4c <_do_help+0x194>
    2f14:	e1a0e00f 	mov	lr, pc
    2f18:	e599f000 	ldr	pc, [r9]
    2f1c:	e3a04001 	mov	r4, #1	; 0x1
    2f20:	e2866001 	add	r6, r6, #1	; 0x1
    2f24:	e2855004 	add	r5, r5, #4	; 0x4
    2f28:	e1560008 	cmp	r6, r8
    2f2c:	e1a01007 	mov	r1, r7
    2f30:	e1a0200a 	mov	r2, sl
    2f34:	baffffed 	blt	2ef0 <_do_help+0x138>
    2f38:	e1a00004 	mov	r0, r4
    2f3c:	e24bd028 	sub	sp, fp, #40	; 0x28
    2f40:	e89daff0 	ldm	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
    2f44:	0800d764 	.word	0x0800d764
    2f48:	0001253c 	.word	0x0001253c
    2f4c:	00012546 	.word	0x00012546

00002f50 <do_help>:
    2f50:	e92d4030 	push	{r4, r5, lr}
    2f54:	e59fc038 	ldr	ip, [pc, #56]	; 2f94 <do_help+0x44>
    2f58:	e59fe038 	ldr	lr, [pc, #56]	; 2f98 <do_help+0x48>
    2f5c:	e1a05001 	mov	r5, r1
    2f60:	e06ec00c 	rsb	ip, lr, ip
    2f64:	e59f1030 	ldr	r1, [pc, #48]	; 2f9c <do_help+0x4c>
    2f68:	e24dd00c 	sub	sp, sp, #12	; 0xc
    2f6c:	e1a04000 	mov	r4, r0
    2f70:	e1a0c1cc 	asr	ip, ip, #3
    2f74:	e88d000c 	stm	sp, {r2, r3}
    2f78:	e0010c91 	mul	r1, r1, ip
    2f7c:	e1a0000e 	mov	r0, lr
    2f80:	e1a02004 	mov	r2, r4
    2f84:	e1a03005 	mov	r3, r5
    2f88:	ebffff8a 	bl	2db8 <_do_help>
    2f8c:	e28dd00c 	add	sp, sp, #12	; 0xc
    2f90:	e8bd8030 	pop	{r4, r5, pc}
    2f94:	000147e0 	.word	0x000147e0
    2f98:	00014510 	.word	0x00014510
    2f9c:	aaaaaaab 	.word	0xaaaaaaab

00002fa0 <tt_blk_trolling>:
    2fa0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    2fa4:	e3a01001 	mov	r1, #1	; 0x1
    2fa8:	e24dd02c 	sub	sp, sp, #44	; 0x2c
    2fac:	e1a04002 	mov	r4, r2
    2fb0:	e1a05003 	mov	r5, r3
    2fb4:	e58d0010 	str	r0, [sp, #16]
    2fb8:	e59da050 	ldr	sl, [sp, #80]
    2fbc:	eb000888 	bl	51e4 <vs_assign_by_name>
    2fc0:	e3500000 	cmp	r0, #0	; 0x0
    2fc4:	0a000005 	beq	2fe0 <tt_blk_trolling+0x40>
    2fc8:	e59d1010 	ldr	r1, [sp, #16]
    2fcc:	e59f03d4 	ldr	r0, [pc, #980]	; 33a8 <tt_blk_trolling+0x408>
    2fd0:	e59f33d4 	ldr	r3, [pc, #980]	; 33ac <tt_blk_trolling+0x40c>
    2fd4:	e1a0e00f 	mov	lr, pc
    2fd8:	e593f000 	ldr	pc, [r3]
    2fdc:	ea000026 	b	307c <tt_blk_trolling+0xdc>
    2fe0:	e1940005 	orrs	r0, r4, r5
    2fe4:	03a04502 	moveq	r4, #8388608	; 0x800000
    2fe8:	03a06502 	moveq	r6, #8388608	; 0x800000
    2fec:	03a07000 	moveq	r7, #0	; 0x0
    2ff0:	0a00000b 	beq	3024 <tt_blk_trolling+0x84>
    2ff4:	e3550000 	cmp	r5, #0	; 0x0
    2ff8:	1a000006 	bne	3018 <tt_blk_trolling+0x78>
    2ffc:	e3540502 	cmp	r4, #8388608	; 0x800000
    3000:	2a000004 	bcs	3018 <tt_blk_trolling+0x78>
    3004:	e3a0000e 	mov	r0, #14	; 0xe
    3008:	eb000702 	bl	4c18 <vs_align>
    300c:	e1a06004 	mov	r6, r4
    3010:	e1a07005 	mov	r7, r5
    3014:	ea000002 	b	3024 <tt_blk_trolling+0x84>
    3018:	e1a06004 	mov	r6, r4
    301c:	e1a07005 	mov	r7, r5
    3020:	e3a04502 	mov	r4, #8388608	; 0x800000
    3024:	e3a0000e 	mov	r0, #14	; 0xe
    3028:	eb0006fa 	bl	4c18 <vs_align>
    302c:	e1a01000 	mov	r1, r0
    3030:	e1a00004 	mov	r0, r4
    3034:	eb003460 	bl	101bc <__aeabi_uidiv>
    3038:	e1a01000 	mov	r1, r0
    303c:	e3a00000 	mov	r0, #0	; 0x0
    3040:	eb000514 	bl	4498 <tt_rnd_get_int>
    3044:	e1a04000 	mov	r4, r0
    3048:	e3a0000e 	mov	r0, #14	; 0xe
    304c:	eb0006f1 	bl	4c18 <vs_align>
    3050:	e0040094 	mul	r4, r4, r0
    3054:	e28d0024 	add	r0, sp, #36	; 0x24
    3058:	e1a01004 	mov	r1, r4
    305c:	e1a02004 	mov	r2, r4
    3060:	eb000535 	bl	453c <tt_rnd_get_data>
    3064:	e250b000 	subs	fp, r0, #0	; 0x0
    3068:	1a000005 	bne	3084 <tt_blk_trolling+0xe4>
    306c:	e59f033c 	ldr	r0, [pc, #828]	; 33b0 <tt_blk_trolling+0x410>
    3070:	e59f3334 	ldr	r3, [pc, #820]	; 33ac <tt_blk_trolling+0x40c>
    3074:	e1a0e00f 	mov	lr, pc
    3078:	e593f000 	ldr	pc, [r3]
    307c:	e3e00000 	mvn	r0, #0	; 0x0
    3080:	ea0000c6 	b	33a0 <tt_blk_trolling+0x400>
    3084:	e59d3024 	ldr	r3, [sp, #36]
    3088:	e3a0000e 	mov	r0, #14	; 0xe
    308c:	e0564003 	subs	r4, r6, r3
    3090:	e0c75fc3 	sbc	r5, r7, r3, asr #31
    3094:	eb0006df 	bl	4c18 <vs_align>
    3098:	e3550000 	cmp	r5, #0	; 0x0
    309c:	e1cd41f8 	strd	r4, [sp, #24]
    30a0:	1a000005 	bne	30bc <tt_blk_trolling+0x11c>
    30a4:	e1a01000 	mov	r1, r0
    30a8:	e1a00004 	mov	r0, r4
    30ac:	eb003442 	bl	101bc <__aeabi_uidiv>
    30b0:	e58d501c 	str	r5, [sp, #28]
    30b4:	e58d0018 	str	r0, [sp, #24]
    30b8:	ea000002 	b	30c8 <tt_blk_trolling+0x128>
    30bc:	e1a01000 	mov	r1, r0
    30c0:	e28d0018 	add	r0, sp, #24	; 0x18
    30c4:	eb0027fb 	bl	d0b8 <__div64_32>
    30c8:	e59d1018 	ldr	r1, [sp, #24]
    30cc:	e3a00000 	mov	r0, #0	; 0x0
    30d0:	eb0004f0 	bl	4498 <tt_rnd_get_int>
    30d4:	e1a04000 	mov	r4, r0
    30d8:	e3a0000e 	mov	r0, #14	; 0xe
    30dc:	eb0006cd 	bl	4c18 <vs_align>
    30e0:	e0c98490 	smull	r8, r9, r0, r4
    30e4:	e59d1010 	ldr	r1, [sp, #16]
    30e8:	e1a0200b 	mov	r2, fp
    30ec:	e59d3024 	ldr	r3, [sp, #36]
    30f0:	e59f02bc 	ldr	r0, [pc, #700]	; 33b4 <tt_blk_trolling+0x414>
    30f4:	e88d0300 	stm	sp, {r8, r9}
    30f8:	e59f42ac 	ldr	r4, [pc, #684]	; 33ac <tt_blk_trolling+0x40c>
    30fc:	e1a0e00f 	mov	lr, pc
    3100:	e594f000 	ldr	pc, [r4]
    3104:	e59d0010 	ldr	r0, [sp, #16]
    3108:	eb000743 	bl	4e1c <vs_device_id>
    310c:	eb00072e 	bl	4dcc <vs_device_erasable>
    3110:	e3500000 	cmp	r0, #0	; 0x0
    3114:	0a000006 	beq	3134 <tt_blk_trolling+0x194>
    3118:	e59f0298 	ldr	r0, [pc, #664]	; 33b8 <tt_blk_trolling+0x418>
    311c:	e1a0e00f 	mov	lr, pc
    3120:	e594f000 	ldr	pc, [r4]
    3124:	e1a00008 	mov	r0, r8
    3128:	e1a01009 	mov	r1, r9
    312c:	e59d2024 	ldr	r2, [sp, #36]
    3130:	eb00067c 	bl	4b28 <vs_erase>
    3134:	e59f0280 	ldr	r0, [pc, #640]	; 33bc <tt_blk_trolling+0x41c>
    3138:	e59f326c 	ldr	r3, [pc, #620]	; 33ac <tt_blk_trolling+0x40c>
    313c:	e1a0e00f 	mov	lr, pc
    3140:	e593f000 	ldr	pc, [r3]
    3144:	e35a0000 	cmp	sl, #0	; 0x0
    3148:	13a07000 	movne	r7, #0	; 0x0
    314c:	159d6024 	ldrne	r6, [sp, #36]
    3150:	1a00001c 	bne	31c8 <tt_blk_trolling+0x228>
    3154:	ea00001f 	b	31d8 <tt_blk_trolling+0x238>
    3158:	e3560902 	cmp	r6, #32768	; 0x8000
    315c:	b1a01006 	movlt	r1, r6
    3160:	a3a01902 	movge	r1, #32768	; 0x8000
    3164:	e58d100c 	str	r1, [sp, #12]
    3168:	e59d1024 	ldr	r1, [sp, #36]
    316c:	e59d200c 	ldr	r2, [sp, #12]
    3170:	e0661001 	rsb	r1, r6, r1
    3174:	e08b1001 	add	r1, fp, r1
    3178:	ebfff8a0 	bl	1400 <memcpy>
    317c:	e3a00000 	mov	r0, #0	; 0x0
    3180:	eb001605 	bl	899c <get_timer>
    3184:	e59d3024 	ldr	r3, [sp, #36]
    3188:	e3a01000 	mov	r1, #0	; 0x0
    318c:	e0663003 	rsb	r3, r6, r3
    3190:	e0984003 	adds	r4, r8, r3
    3194:	e0a95fc3 	adc	r5, r9, r3, asr #31
    3198:	e1a03005 	mov	r3, r5
    319c:	e59d500c 	ldr	r5, [sp, #12]
    31a0:	e58d0008 	str	r0, [sp, #8]
    31a4:	e1a02004 	mov	r2, r4
    31a8:	e1a0000a 	mov	r0, sl
    31ac:	e58d1004 	str	r1, [sp, #4]
    31b0:	e58d5000 	str	r5, [sp]
    31b4:	eb000645 	bl	4ad0 <vs_write>
    31b8:	e59d0008 	ldr	r0, [sp, #8]
    31bc:	eb0015f6 	bl	899c <get_timer>
    31c0:	e0656006 	rsb	r6, r5, r6
    31c4:	e0877000 	add	r7, r7, r0
    31c8:	e3560000 	cmp	r6, #0	; 0x0
    31cc:	e1a0000a 	mov	r0, sl
    31d0:	1affffe0 	bne	3158 <tt_blk_trolling+0x1b8>
    31d4:	ea00000b 	b	3208 <tt_blk_trolling+0x268>
    31d8:	e1a0000a 	mov	r0, sl
    31dc:	eb0015ee 	bl	899c <get_timer>
    31e0:	e59d1024 	ldr	r1, [sp, #36]
    31e4:	e1a04000 	mov	r4, r0
    31e8:	e1a02008 	mov	r2, r8
    31ec:	e1a03009 	mov	r3, r9
    31f0:	e1a0000b 	mov	r0, fp
    31f4:	e88d0402 	stm	sp, {r1, sl}
    31f8:	eb000634 	bl	4ad0 <vs_write>
    31fc:	e1a00004 	mov	r0, r4
    3200:	eb0015e5 	bl	899c <get_timer>
    3204:	e1a07000 	mov	r7, r0
    3208:	e59d0024 	ldr	r0, [sp, #36]
    320c:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    3210:	e1a00540 	asr	r0, r0, #10
    3214:	e1a01007 	mov	r1, r7
    3218:	e0000093 	mul	r0, r3, r0
    321c:	eb0033e6 	bl	101bc <__aeabi_uidiv>
    3220:	e59f3184 	ldr	r3, [pc, #388]	; 33ac <tt_blk_trolling+0x40c>
    3224:	e58d0014 	str	r0, [sp, #20]
    3228:	e59f0190 	ldr	r0, [pc, #400]	; 33c0 <tt_blk_trolling+0x420>
    322c:	e1a0e00f 	mov	lr, pc
    3230:	e593f000 	ldr	pc, [r3]
    3234:	e35a0000 	cmp	sl, #0	; 0x0
    3238:	13a07000 	movne	r7, #0	; 0x0
    323c:	159d6024 	ldrne	r6, [sp, #36]
    3240:	1a00001b 	bne	32b4 <tt_blk_trolling+0x314>
    3244:	ea00001e 	b	32c4 <tt_blk_trolling+0x324>
    3248:	eb0015d3 	bl	899c <get_timer>
    324c:	e59d3024 	ldr	r3, [sp, #36]
    3250:	e3560902 	cmp	r6, #32768	; 0x8000
    3254:	b1a04006 	movlt	r4, r6
    3258:	a3a04902 	movge	r4, #32768	; 0x8000
    325c:	e0663003 	rsb	r3, r6, r3
    3260:	e1a05000 	mov	r5, r0
    3264:	e0980003 	adds	r0, r8, r3
    3268:	e0a91fc3 	adc	r1, r9, r3, asr #31
    326c:	e1a02000 	mov	r2, r0
    3270:	e1a03001 	mov	r3, r1
    3274:	e1a0000a 	mov	r0, sl
    3278:	e3a01000 	mov	r1, #0	; 0x0
    327c:	e58d1004 	str	r1, [sp, #4]
    3280:	e58d4000 	str	r4, [sp]
    3284:	eb000711 	bl	4ed0 <vs_read>
    3288:	e1a00005 	mov	r0, r5
    328c:	eb0015c2 	bl	899c <get_timer>
    3290:	e1a05000 	mov	r5, r0
    3294:	e59d0024 	ldr	r0, [sp, #36]
    3298:	e1a0100a 	mov	r1, sl
    329c:	e0660000 	rsb	r0, r6, r0
    32a0:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    32a4:	e1a02004 	mov	r2, r4
    32a8:	ebfff854 	bl	1400 <memcpy>
    32ac:	e0877005 	add	r7, r7, r5
    32b0:	e0646006 	rsb	r6, r4, r6
    32b4:	e3a00000 	mov	r0, #0	; 0x0
    32b8:	e1560000 	cmp	r6, r0
    32bc:	1affffe1 	bne	3248 <tt_blk_trolling+0x2a8>
    32c0:	ea00000b 	b	32f4 <tt_blk_trolling+0x354>
    32c4:	e1a0000a 	mov	r0, sl
    32c8:	eb0015b3 	bl	899c <get_timer>
    32cc:	e59d1024 	ldr	r1, [sp, #36]
    32d0:	e1a04000 	mov	r4, r0
    32d4:	e1a02008 	mov	r2, r8
    32d8:	e1a03009 	mov	r3, r9
    32dc:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    32e0:	e88d0402 	stm	sp, {r1, sl}
    32e4:	eb0006f9 	bl	4ed0 <vs_read>
    32e8:	e1a00004 	mov	r0, r4
    32ec:	eb0015aa 	bl	899c <get_timer>
    32f0:	e1a07000 	mov	r7, r0
    32f4:	e59d0024 	ldr	r0, [sp, #36]
    32f8:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    32fc:	e1a00540 	asr	r0, r0, #10
    3300:	e0000093 	mul	r0, r3, r0
    3304:	e1a01007 	mov	r1, r7
    3308:	eb0033ab 	bl	101bc <__aeabi_uidiv>
    330c:	e59f4098 	ldr	r4, [pc, #152]	; 33ac <tt_blk_trolling+0x40c>
    3310:	e1a05000 	mov	r5, r0
    3314:	e59f00a8 	ldr	r0, [pc, #168]	; 33c4 <tt_blk_trolling+0x424>
    3318:	e1a0e00f 	mov	lr, pc
    331c:	e594f000 	ldr	pc, [r4]
    3320:	e1a0000b 	mov	r0, fp
    3324:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    3328:	e59d2024 	ldr	r2, [sp, #36]
    332c:	e3a0300c 	mov	r3, #12	; 0xc
    3330:	eb000404 	bl	4348 <tt_cmp_buffer>
    3334:	e3500000 	cmp	r0, #0	; 0x0
    3338:	0a000007 	beq	335c <tt_blk_trolling+0x3bc>
    333c:	e58d0000 	str	r0, [sp]
    3340:	e59d1010 	ldr	r1, [sp, #16]
    3344:	e59d2014 	ldr	r2, [sp, #20]
    3348:	e1a03005 	mov	r3, r5
    334c:	e59f0074 	ldr	r0, [pc, #116]	; 33c8 <tt_blk_trolling+0x428>
    3350:	e1a0e00f 	mov	lr, pc
    3354:	e594f000 	ldr	pc, [r4]
    3358:	ea000005 	b	3374 <tt_blk_trolling+0x3d4>
    335c:	e59d1010 	ldr	r1, [sp, #16]
    3360:	e59d2014 	ldr	r2, [sp, #20]
    3364:	e1a03005 	mov	r3, r5
    3368:	e59f005c 	ldr	r0, [pc, #92]	; 33cc <tt_blk_trolling+0x42c>
    336c:	e1a0e00f 	mov	lr, pc
    3370:	e594f000 	ldr	pc, [r4]
    3374:	e59f0054 	ldr	r0, [pc, #84]	; 33d0 <tt_blk_trolling+0x430>
    3378:	e59d1024 	ldr	r1, [sp, #36]
    337c:	e890000c 	ldm	r0, {r2, r3}
    3380:	e0922001 	adds	r2, r2, r1
    3384:	e0a33fc1 	adc	r3, r3, r1, asr #31
    3388:	e880000c 	stm	r0, {r2, r3}
    338c:	e59f0040 	ldr	r0, [pc, #64]	; 33d4 <tt_blk_trolling+0x434>
    3390:	e59f3014 	ldr	r3, [pc, #20]	; 33ac <tt_blk_trolling+0x40c>
    3394:	e1a0e00f 	mov	lr, pc
    3398:	e593f000 	ldr	pc, [r3]
    339c:	e3a00000 	mov	r0, #0	; 0x0
    33a0:	e28dd02c 	add	sp, sp, #44	; 0x2c
    33a4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    33a8:	000126ed 	.word	0x000126ed
    33ac:	0800d764 	.word	0x0800d764
    33b0:	0001270b 	.word	0x0001270b
    33b4:	0001272b 	.word	0x0001272b
    33b8:	00014095 	.word	0x00014095
    33bc:	00011f2d 	.word	0x00011f2d
    33c0:	0001441c 	.word	0x0001441c
    33c4:	0001275f 	.word	0x0001275f
    33c8:	00012762 	.word	0x00012762
    33cc:	0001279c 	.word	0x0001279c
    33d0:	0800d1b8 	.word	0x0800d1b8
    33d4:	0001250e 	.word	0x0001250e

000033d8 <do_tt_blk>:
    33d8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    33dc:	e3520001 	cmp	r2, #1	; 0x1
    33e0:	e24dd00c 	sub	sp, sp, #12	; 0xc
    33e4:	e1a05002 	mov	r5, r2
    33e8:	e1a07003 	mov	r7, r3
    33ec:	ca000005 	bgt	3408 <do_tt_blk+0x30>
    33f0:	e59f01bc 	ldr	r0, [pc, #444]	; 35b4 <do_tt_blk+0x1dc>
    33f4:	e59f31bc 	ldr	r3, [pc, #444]	; 35b8 <do_tt_blk+0x1e0>
    33f8:	e1a0e00f 	mov	lr, pc
    33fc:	e593f000 	ldr	pc, [r3]
    3400:	e3e00000 	mvn	r0, #0	; 0x0
    3404:	ea000068 	b	35ac <do_tt_blk+0x1d4>
    3408:	e3520002 	cmp	r2, #2	; 0x2
    340c:	03a04000 	moveq	r4, #0	; 0x0
    3410:	01a06004 	moveq	r6, r4
    3414:	0a000025 	beq	34b0 <do_tt_blk+0xd8>
    3418:	e3a01000 	mov	r1, #0	; 0x0
    341c:	e3a0200a 	mov	r2, #10	; 0xa
    3420:	e5930008 	ldr	r0, [r3, #8]
    3424:	eb002760 	bl	d1ac <simple_strtoul>
    3428:	e59f118c 	ldr	r1, [pc, #396]	; 35bc <do_tt_blk+0x1e4>
    342c:	e1a04000 	mov	r4, r0
    3430:	e3a02005 	mov	r2, #5	; 0x5
    3434:	e5970004 	ldr	r0, [r7, #4]
    3438:	ebfff7b7 	bl	131c <strncmp>
    343c:	e3500000 	cmp	r0, #0	; 0x0
    3440:	059f0178 	ldreq	r0, [pc, #376]	; 35c0 <do_tt_blk+0x1e8>
    3444:	01a01004 	moveq	r1, r4
    3448:	059f3168 	ldreq	r3, [pc, #360]	; 35b8 <do_tt_blk+0x1e0>
    344c:	0a000008 	beq	3474 <do_tt_blk+0x9c>
    3450:	e59f116c 	ldr	r1, [pc, #364]	; 35c4 <do_tt_blk+0x1ec>
    3454:	e5970004 	ldr	r0, [r7, #4]
    3458:	e3a02003 	mov	r2, #3	; 0x3
    345c:	ebfff7ae 	bl	131c <strncmp>
    3460:	e3500000 	cmp	r0, #0	; 0x0
    3464:	e59f314c 	ldr	r3, [pc, #332]	; 35b8 <do_tt_blk+0x1e0>
    3468:	059f0150 	ldreq	r0, [pc, #336]	; 35c0 <do_tt_blk+0x1e8>
    346c:	159f0154 	ldrne	r0, [pc, #340]	; 35c8 <do_tt_blk+0x1f0>
    3470:	e1a01004 	mov	r1, r4
    3474:	e1a0e00f 	mov	lr, pc
    3478:	e593f000 	ldr	pc, [r3]
    347c:	e3550003 	cmp	r5, #3	; 0x3
    3480:	03a06000 	moveq	r6, #0	; 0x0
    3484:	0a000009 	beq	34b0 <do_tt_blk+0xd8>
    3488:	e3a01000 	mov	r1, #0	; 0x0
    348c:	e597000c 	ldr	r0, [r7, #12]
    3490:	e3a02010 	mov	r2, #16	; 0x10
    3494:	eb002744 	bl	d1ac <simple_strtoul>
    3498:	e1a06000 	mov	r6, r0
    349c:	e1a01006 	mov	r1, r6
    34a0:	e59f0124 	ldr	r0, [pc, #292]	; 35cc <do_tt_blk+0x1f4>
    34a4:	e59f310c 	ldr	r3, [pc, #268]	; 35b8 <do_tt_blk+0x1e0>
    34a8:	e1a0e00f 	mov	lr, pc
    34ac:	e593f000 	ldr	pc, [r3]
    34b0:	e3a00000 	mov	r0, #0	; 0x0
    34b4:	eb001538 	bl	899c <get_timer>
    34b8:	e59f1110 	ldr	r1, [pc, #272]	; 35d0 <do_tt_blk+0x1f8>
    34bc:	e3a02000 	mov	r2, #0	; 0x0
    34c0:	e3a03000 	mov	r3, #0	; 0x0
    34c4:	e881000c 	stm	r1, {r2, r3}
    34c8:	e1a09a04 	lsl	r9, r4, #20
    34cc:	e2875004 	add	r5, r7, #4	; 0x4
    34d0:	e1a08b24 	lsr	r8, r4, #22
    34d4:	e1a07504 	lsl	r7, r4, #10
    34d8:	e1a0a624 	lsr	sl, r4, #12
    34dc:	e1a0b000 	mov	fp, r0
    34e0:	e3a04000 	mov	r4, #0	; 0x0
    34e4:	e59f30cc 	ldr	r3, [pc, #204]	; 35b8 <do_tt_blk+0x1e0>
    34e8:	e1a01004 	mov	r1, r4
    34ec:	e59f00e0 	ldr	r0, [pc, #224]	; 35d4 <do_tt_blk+0x1fc>
    34f0:	e1a0e00f 	mov	lr, pc
    34f4:	e593f000 	ldr	pc, [r3]
    34f8:	e59f10bc 	ldr	r1, [pc, #188]	; 35bc <do_tt_blk+0x1e4>
    34fc:	e3a02005 	mov	r2, #5	; 0x5
    3500:	e5950000 	ldr	r0, [r5]
    3504:	ebfff784 	bl	131c <strncmp>
    3508:	e3500000 	cmp	r0, #0	; 0x0
    350c:	e3a02003 	mov	r2, #3	; 0x3
    3510:	e59f10ac 	ldr	r1, [pc, #172]	; 35c4 <do_tt_blk+0x1ec>
    3514:	e1a03008 	mov	r3, r8
    3518:	05950000 	ldreq	r0, [r5]
    351c:	01a02007 	moveq	r2, r7
    3520:	0a000008 	beq	3548 <do_tt_blk+0x170>
    3524:	e5950000 	ldr	r0, [r5]
    3528:	ebfff77b 	bl	131c <strncmp>
    352c:	e3500000 	cmp	r0, #0	; 0x0
    3530:	05950000 	ldreq	r0, [r5]
    3534:	15950000 	ldrne	r0, [r5]
    3538:	e1a02009 	mov	r2, r9
    353c:	e1a0300a 	mov	r3, sl
    3540:	01a02007 	moveq	r2, r7
    3544:	01a03008 	moveq	r3, r8
    3548:	e58d6000 	str	r6, [sp]
    354c:	ebfffe93 	bl	2fa0 <tt_blk_trolling>
    3550:	eb002b2c 	bl	e208 <tstc>
    3554:	e3500000 	cmp	r0, #0	; 0x0
    3558:	e2844001 	add	r4, r4, #1	; 0x1
    355c:	0affffe0 	beq	34e4 <do_tt_blk+0x10c>
    3560:	eb002b29 	bl	e20c <getc>
    3564:	e3500003 	cmp	r0, #3	; 0x3
    3568:	1affffdd 	bne	34e4 <do_tt_blk+0x10c>
    356c:	e1a0000b 	mov	r0, fp
    3570:	eb001509 	bl	899c <get_timer>
    3574:	e59f3054 	ldr	r3, [pc, #84]	; 35d0 <do_tt_blk+0x1f8>
    3578:	e30e1a60 	movw	r1, #60000	; 0xea60
    357c:	e5934000 	ldr	r4, [r3]
    3580:	e5933004 	ldr	r3, [r3, #4]
    3584:	e1a04a24 	lsr	r4, r4, #20
    3588:	e1844603 	orr	r4, r4, r3, lsl #12
    358c:	eb00330a 	bl	101bc <__aeabi_uidiv>
    3590:	e1a01004 	mov	r1, r4
    3594:	e1a02000 	mov	r2, r0
    3598:	e59f3018 	ldr	r3, [pc, #24]	; 35b8 <do_tt_blk+0x1e0>
    359c:	e59f0034 	ldr	r0, [pc, #52]	; 35d8 <do_tt_blk+0x200>
    35a0:	e1a0e00f 	mov	lr, pc
    35a4:	e593f000 	ldr	pc, [r3]
    35a8:	e3a00000 	mov	r0, #0	; 0x0
    35ac:	e28dd00c 	add	sp, sp, #12	; 0xc
    35b0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    35b4:	000127c9 	.word	0x000127c9
    35b8:	0800d764 	.word	0x0800d764
    35bc:	00011bf7 	.word	0x00011bf7
    35c0:	00012810 	.word	0x00012810
    35c4:	000121ce 	.word	0x000121ce
    35c8:	00012834 	.word	0x00012834
    35cc:	00012858 	.word	0x00012858
    35d0:	0800d1b8 	.word	0x0800d1b8
    35d4:	0001287b 	.word	0x0001287b
    35d8:	00012885 	.word	0x00012885

000035dc <__show_strbuf>:
    35dc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    35e0:	e59f4058 	ldr	r4, [pc, #88]	; 3640 <__show_strbuf+0x64>
    35e4:	e24dd004 	sub	sp, sp, #4	; 0x4
    35e8:	e1a06001 	mov	r6, r1
    35ec:	e1a01000 	mov	r1, r0
    35f0:	e59f004c 	ldr	r0, [pc, #76]	; 3644 <__show_strbuf+0x68>
    35f4:	e1a05002 	mov	r5, r2
    35f8:	e1a07004 	mov	r7, r4
    35fc:	e1a0e00f 	mov	lr, pc
    3600:	e594f000 	ldr	pc, [r4]
    3604:	e3a04000 	mov	r4, #0	; 0x0
    3608:	ea000003 	b	361c <__show_strbuf+0x40>
    360c:	e7d61004 	ldrb	r1, [r6, r4]
    3610:	e1a0e00f 	mov	lr, pc
    3614:	e597f000 	ldr	pc, [r7]
    3618:	e2844001 	add	r4, r4, #1	; 0x1
    361c:	e1540005 	cmp	r4, r5
    3620:	e59f0020 	ldr	r0, [pc, #32]	; 3648 <__show_strbuf+0x6c>
    3624:	bafffff8 	blt	360c <__show_strbuf+0x30>
    3628:	e59f001c 	ldr	r0, [pc, #28]	; 364c <__show_strbuf+0x70>
    362c:	e59f300c 	ldr	r3, [pc, #12]	; 3640 <__show_strbuf+0x64>
    3630:	e1a0e00f 	mov	lr, pc
    3634:	e593f000 	ldr	pc, [r3]
    3638:	e28dd004 	add	sp, sp, #4	; 0x4
    363c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    3640:	0800d764 	.word	0x0800d764
    3644:	000128d3 	.word	0x000128d3
    3648:	0001424e 	.word	0x0001424e
    364c:	0001250e 	.word	0x0001250e

00003650 <do_crypto_quick>:
    3650:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    3654:	e24dd0f0 	sub	sp, sp, #240	; 0xf0
    3658:	e28da0e0 	add	sl, sp, #224	; 0xe0
    365c:	e3a04010 	mov	r4, #16	; 0x10
    3660:	e28d70c0 	add	r7, sp, #192	; 0xc0
    3664:	e1a02004 	mov	r2, r4
    3668:	e3a09020 	mov	r9, #32	; 0x20
    366c:	e59f11f0 	ldr	r1, [pc, #496]	; 3864 <do_crypto_quick+0x214>
    3670:	e1a0000a 	mov	r0, sl
    3674:	e3a05040 	mov	r5, #64	; 0x40
    3678:	e28d6080 	add	r6, sp, #128	; 0x80
    367c:	ebfff75f 	bl	1400 <memcpy>
    3680:	e1a02009 	mov	r2, r9
    3684:	e59f11dc 	ldr	r1, [pc, #476]	; 3868 <do_crypto_quick+0x218>
    3688:	e1a00007 	mov	r0, r7
    368c:	ebfff75b 	bl	1400 <memcpy>
    3690:	e08d8005 	add	r8, sp, r5
    3694:	e1a02005 	mov	r2, r5
    3698:	e59f11cc 	ldr	r1, [pc, #460]	; 386c <do_crypto_quick+0x21c>
    369c:	e1a00006 	mov	r0, r6
    36a0:	ebfff756 	bl	1400 <memcpy>
    36a4:	e1a02005 	mov	r2, r5
    36a8:	e59f11c0 	ldr	r1, [pc, #448]	; 3870 <do_crypto_quick+0x220>
    36ac:	e1a00008 	mov	r0, r8
    36b0:	ebfff752 	bl	1400 <memcpy>
    36b4:	e1a02004 	mov	r2, r4
    36b8:	e1a0100a 	mov	r1, sl
    36bc:	e59f01b0 	ldr	r0, [pc, #432]	; 3874 <do_crypto_quick+0x224>
    36c0:	ebffffc5 	bl	35dc <__show_strbuf>
    36c4:	e1a02009 	mov	r2, r9
    36c8:	e1a01007 	mov	r1, r7
    36cc:	e59f01a4 	ldr	r0, [pc, #420]	; 3878 <do_crypto_quick+0x228>
    36d0:	ebffffc1 	bl	35dc <__show_strbuf>
    36d4:	e1a01006 	mov	r1, r6
    36d8:	e1a02005 	mov	r2, r5
    36dc:	e59f0198 	ldr	r0, [pc, #408]	; 387c <do_crypto_quick+0x22c>
    36e0:	ebffffbd 	bl	35dc <__show_strbuf>
    36e4:	e1a01008 	mov	r1, r8
    36e8:	e1a02005 	mov	r2, r5
    36ec:	e59f018c 	ldr	r0, [pc, #396]	; 3880 <do_crypto_quick+0x230>
    36f0:	ebffffb9 	bl	35dc <__show_strbuf>
    36f4:	e1a01007 	mov	r1, r7
    36f8:	e1a0200a 	mov	r2, sl
    36fc:	e3a00001 	mov	r0, #1	; 0x1
    3700:	eb0006fa 	bl	52f0 <ss_aes_init>
    3704:	e1a0100d 	mov	r1, sp
    3708:	e1a02005 	mov	r2, r5
    370c:	e1a00006 	mov	r0, r6
    3710:	eb0006e5 	bl	52ac <ss_aes_data>
    3714:	e1a0100d 	mov	r1, sp
    3718:	e1a02005 	mov	r2, r5
    371c:	e59f0160 	ldr	r0, [pc, #352]	; 3884 <do_crypto_quick+0x234>
    3720:	ebffffad 	bl	35dc <__show_strbuf>
    3724:	e1a01007 	mov	r1, r7
    3728:	e1a0200a 	mov	r2, sl
    372c:	e3a00000 	mov	r0, #0	; 0x0
    3730:	eb0006ee 	bl	52f0 <ss_aes_init>
    3734:	e59f714c 	ldr	r7, [pc, #332]	; 3888 <do_crypto_quick+0x238>
    3738:	e1a0100d 	mov	r1, sp
    373c:	e1a02005 	mov	r2, r5
    3740:	e1a00008 	mov	r0, r8
    3744:	eb0006d8 	bl	52ac <ss_aes_data>
    3748:	e1a0100d 	mov	r1, sp
    374c:	e1a02005 	mov	r2, r5
    3750:	e59f0134 	ldr	r0, [pc, #308]	; 388c <do_crypto_quick+0x23c>
    3754:	ebffffa0 	bl	35dc <__show_strbuf>
    3758:	e1a02007 	mov	r2, r7
    375c:	e3a01000 	mov	r1, #0	; 0x0
    3760:	e3a00001 	mov	r0, #1	; 0x1
    3764:	eb0006e1 	bl	52f0 <ss_aes_init>
    3768:	e1a0100d 	mov	r1, sp
    376c:	e1a02005 	mov	r2, r5
    3770:	e1a00006 	mov	r0, r6
    3774:	eb0006cc 	bl	52ac <ss_aes_data>
    3778:	e1a0100d 	mov	r1, sp
    377c:	e1a02005 	mov	r2, r5
    3780:	e59f0108 	ldr	r0, [pc, #264]	; 3890 <do_crypto_quick+0x240>
    3784:	ebffff94 	bl	35dc <__show_strbuf>
    3788:	e3a00000 	mov	r0, #0	; 0x0
    378c:	e1a02007 	mov	r2, r7
    3790:	e1a01000 	mov	r1, r0
    3794:	eb0006d5 	bl	52f0 <ss_aes_init>
    3798:	e1a0100d 	mov	r1, sp
    379c:	e1a02005 	mov	r2, r5
    37a0:	e1a0000d 	mov	r0, sp
    37a4:	eb0006c0 	bl	52ac <ss_aes_data>
    37a8:	e1a02005 	mov	r2, r5
    37ac:	e1a0100d 	mov	r1, sp
    37b0:	e59f00dc 	ldr	r0, [pc, #220]	; 3894 <do_crypto_quick+0x244>
    37b4:	ebffff88 	bl	35dc <__show_strbuf>
    37b8:	e1a01005 	mov	r1, r5
    37bc:	e3a00001 	mov	r0, #1	; 0x1
    37c0:	eb0007e8 	bl	5768 <hash_init>
    37c4:	e1a01005 	mov	r1, r5
    37c8:	e1a00006 	mov	r0, r6
    37cc:	eb0007c0 	bl	56d4 <hash_data>
    37d0:	e1a0000d 	mov	r0, sp
    37d4:	eb0007a2 	bl	5664 <hash_value>
    37d8:	eb00079a 	bl	5648 <hash_len>
    37dc:	e1a0100d 	mov	r1, sp
    37e0:	e1a02000 	mov	r2, r0
    37e4:	e59f00ac 	ldr	r0, [pc, #172]	; 3898 <do_crypto_quick+0x248>
    37e8:	ebffff7b 	bl	35dc <__show_strbuf>
    37ec:	e1a01005 	mov	r1, r5
    37f0:	e3a00002 	mov	r0, #2	; 0x2
    37f4:	eb0007db 	bl	5768 <hash_init>
    37f8:	e1a01005 	mov	r1, r5
    37fc:	e1a00006 	mov	r0, r6
    3800:	eb0007b3 	bl	56d4 <hash_data>
    3804:	e1a0000d 	mov	r0, sp
    3808:	eb000795 	bl	5664 <hash_value>
    380c:	eb00078d 	bl	5648 <hash_len>
    3810:	e1a0100d 	mov	r1, sp
    3814:	e1a02000 	mov	r2, r0
    3818:	e59f007c 	ldr	r0, [pc, #124]	; 389c <do_crypto_quick+0x24c>
    381c:	ebffff6e 	bl	35dc <__show_strbuf>
    3820:	e1a01005 	mov	r1, r5
    3824:	e3a00003 	mov	r0, #3	; 0x3
    3828:	eb0007ce 	bl	5768 <hash_init>
    382c:	e1a01005 	mov	r1, r5
    3830:	e1a00006 	mov	r0, r6
    3834:	eb0007a6 	bl	56d4 <hash_data>
    3838:	e1a0000d 	mov	r0, sp
    383c:	eb000788 	bl	5664 <hash_value>
    3840:	eb000780 	bl	5648 <hash_len>
    3844:	e1a0100d 	mov	r1, sp
    3848:	e1a02000 	mov	r2, r0
    384c:	e59f004c 	ldr	r0, [pc, #76]	; 38a0 <do_crypto_quick+0x250>
    3850:	ebffff61 	bl	35dc <__show_strbuf>
    3854:	e1a0400d 	mov	r4, sp
    3858:	e3a00000 	mov	r0, #0	; 0x0
    385c:	e28dd0f0 	add	sp, sp, #240	; 0xf0
    3860:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3864:	00010f68 	.word	0x00010f68
    3868:	00010f48 	.word	0x00010f48
    386c:	00010f08 	.word	0x00010f08
    3870:	00010ec8 	.word	0x00010ec8
    3874:	000128d8 	.word	0x000128d8
    3878:	000128db 	.word	0x000128db
    387c:	000128df 	.word	0x000128df
    3880:	000128ea 	.word	0x000128ea
    3884:	000128fb 	.word	0x000128fb
    3888:	0800d108 	.word	0x0800d108
    388c:	0001290e 	.word	0x0001290e
    3890:	000128f7 	.word	0x000128f7
    3894:	0001290a 	.word	0x0001290a
    3898:	0001291d 	.word	0x0001291d
    389c:	00012921 	.word	0x00012921
    38a0:	00012926 	.word	0x00012926

000038a4 <tt_crypto_hash>:
    38a4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    38a8:	e3a0544e 	mov	r5, #1308622848	; 0x4e000000
    38ac:	e5951000 	ldr	r1, [r5]
    38b0:	e59f2220 	ldr	r2, [pc, #544]	; 3ad8 <tt_crypto_hash+0x234>
    38b4:	e24dd038 	sub	sp, sp, #56	; 0x38
    38b8:	e59f321c 	ldr	r3, [pc, #540]	; 3adc <tt_crypto_hash+0x238>
    38bc:	e1510002 	cmp	r1, r2
    38c0:	e28dc028 	add	ip, sp, #40	; 0x28
    38c4:	e1a06000 	mov	r6, r0
    38c8:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    38cc:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    38d0:	0a000006 	beq	38f0 <tt_crypto_hash+0x4c>
    38d4:	e59f0204 	ldr	r0, [pc, #516]	; 3ae0 <tt_crypto_hash+0x23c>
    38d8:	e59f3204 	ldr	r3, [pc, #516]	; 3ae4 <tt_crypto_hash+0x240>
    38dc:	e1a0e00f 	mov	lr, pc
    38e0:	e593f000 	ldr	pc, [r3]
    38e4:	e59f01fc 	ldr	r0, [pc, #508]	; 3ae8 <tt_crypto_hash+0x244>
    38e8:	eb001407 	bl	890c <udelay>
    38ec:	ea000008 	b	3914 <tt_crypto_hash+0x70>
    38f0:	e5951004 	ldr	r1, [r5, #4]
    38f4:	e3a04000 	mov	r4, #0	; 0x0
    38f8:	e3510003 	cmp	r1, #3	; 0x3
    38fc:	e5854000 	str	r4, [r5]
    3900:	e59fc1dc 	ldr	ip, [pc, #476]	; 3ae4 <tt_crypto_hash+0x240>
    3904:	9a000004 	bls	391c <tt_crypto_hash+0x78>
    3908:	e59f01dc 	ldr	r0, [pc, #476]	; 3aec <tt_crypto_hash+0x248>
    390c:	e1a0e00f 	mov	lr, pc
    3910:	e59cf000 	ldr	pc, [ip]
    3914:	e3e0c000 	mvn	ip, #0	; 0x0
    3918:	ea00006b 	b	3acc <tt_crypto_hash+0x228>
    391c:	e28d2038 	add	r2, sp, #56	; 0x38
    3920:	e0823101 	add	r3, r2, r1, lsl #2
    3924:	e5131010 	ldr	r1, [r3, #-16]
    3928:	e5952008 	ldr	r2, [r5, #8]
    392c:	e59f01bc 	ldr	r0, [pc, #444]	; 3af0 <tt_crypto_hash+0x24c>
    3930:	e1a0e00f 	mov	lr, pc
    3934:	e59cf000 	ldr	pc, [ip]
    3938:	e9950003 	ldmib	r5, {r0, r1}
    393c:	eb000789 	bl	5768 <hash_init>
    3940:	eb000740 	bl	5648 <hash_len>
    3944:	e1a03000 	mov	r3, r0
    3948:	e59f01a4 	ldr	r0, [pc, #420]	; 3af4 <tt_crypto_hash+0x250>
    394c:	e3a0c020 	mov	ip, #32	; 0x20
    3950:	e1a01000 	mov	r1, r0
    3954:	e3a02001 	mov	r2, #1	; 0x1
    3958:	e58dc000 	str	ip, [sp]
    395c:	eb00257a 	bl	cf4c <print_buffer>
    3960:	e3560000 	cmp	r6, #0	; 0x0
    3964:	0a000022 	beq	39f4 <tt_crypto_hash+0x150>
    3968:	e1a09005 	mov	r9, r5
    396c:	e1a0a004 	mov	sl, r4
    3970:	e5955008 	ldr	r5, [r5, #8]
    3974:	e2868901 	add	r8, r6, #16384	; 0x4000
    3978:	ea000019 	b	39e4 <tt_crypto_hash+0x140>
    397c:	e5991008 	ldr	r1, [r9, #8]
    3980:	e3550902 	cmp	r5, #32768	; 0x8000
    3984:	b1a04005 	movlt	r4, r5
    3988:	a3a04902 	movge	r4, #32768	; 0x8000
    398c:	e0651001 	rsb	r1, r5, r1
    3990:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3994:	e2811030 	add	r1, r1, #48	; 0x30
    3998:	e1a02004 	mov	r2, r4
    399c:	ebfff697 	bl	1400 <memcpy>
    39a0:	e3a00000 	mov	r0, #0	; 0x0
    39a4:	eb0013fc 	bl	899c <get_timer>
    39a8:	e3550901 	cmp	r5, #16384	; 0x4000
    39ac:	b1a01005 	movlt	r1, r5
    39b0:	a3a01901 	movge	r1, #16384	; 0x4000
    39b4:	e1a07000 	mov	r7, r0
    39b8:	e1a00006 	mov	r0, r6
    39bc:	eb000744 	bl	56d4 <hash_data>
    39c0:	e3540902 	cmp	r4, #32768	; 0x8000
    39c4:	e0645005 	rsb	r5, r4, r5
    39c8:	1a000002 	bne	39d8 <tt_crypto_hash+0x134>
    39cc:	e3a01901 	mov	r1, #16384	; 0x4000
    39d0:	e1a00008 	mov	r0, r8
    39d4:	eb00073e 	bl	56d4 <hash_data>
    39d8:	e1a00007 	mov	r0, r7
    39dc:	eb0013ee 	bl	899c <get_timer>
    39e0:	e08aa000 	add	sl, sl, r0
    39e4:	e3550000 	cmp	r5, #0	; 0x0
    39e8:	e1a00006 	mov	r0, r6
    39ec:	1affffe2 	bne	397c <tt_crypto_hash+0xd8>
    39f0:	ea000008 	b	3a18 <tt_crypto_hash+0x174>
    39f4:	e1a00006 	mov	r0, r6
    39f8:	eb0013e7 	bl	899c <get_timer>
    39fc:	e5951008 	ldr	r1, [r5, #8]
    3a00:	e1a04000 	mov	r4, r0
    3a04:	e59f00ec 	ldr	r0, [pc, #236]	; 3af8 <tt_crypto_hash+0x254>
    3a08:	eb000731 	bl	56d4 <hash_data>
    3a0c:	e1a00004 	mov	r0, r4
    3a10:	eb0013e1 	bl	899c <get_timer>
    3a14:	e1a0a000 	mov	sl, r0
    3a18:	e28d4008 	add	r4, sp, #8	; 0x8
    3a1c:	e3a0644e 	mov	r6, #1308622848	; 0x4e000000
    3a20:	e1a00004 	mov	r0, r4
    3a24:	eb00070e 	bl	5664 <hash_value>
    3a28:	e5965008 	ldr	r5, [r6, #8]
    3a2c:	eb000705 	bl	5648 <hash_len>
    3a30:	e3a0c020 	mov	ip, #32	; 0x20
    3a34:	e1a03000 	mov	r3, r0
    3a38:	e1a01004 	mov	r1, r4
    3a3c:	e3a02001 	mov	r2, #1	; 0x1
    3a40:	e1a00004 	mov	r0, r4
    3a44:	e58dc000 	str	ip, [sp]
    3a48:	eb00253f 	bl	cf4c <print_buffer>
    3a4c:	e5963004 	ldr	r3, [r6, #4]
    3a50:	e28d1038 	add	r1, sp, #56	; 0x38
    3a54:	e0812103 	add	r2, r1, r3, lsl #2
    3a58:	e59f3084 	ldr	r3, [pc, #132]	; 3ae4 <tt_crypto_hash+0x240>
    3a5c:	e5127010 	ldr	r7, [r2, #-16]
    3a60:	e5938000 	ldr	r8, [r3]
    3a64:	eb0006f7 	bl	5648 <hash_len>
    3a68:	e1a01004 	mov	r1, r4
    3a6c:	e1a02000 	mov	r2, r0
    3a70:	e59f007c 	ldr	r0, [pc, #124]	; 3af4 <tt_crypto_hash+0x250>
    3a74:	ebfff682 	bl	1484 <memcmp>
    3a78:	e1a05525 	lsr	r5, r5, #10
    3a7c:	e3500000 	cmp	r0, #0	; 0x0
    3a80:	e59f3074 	ldr	r3, [pc, #116]	; 3afc <tt_crypto_hash+0x258>
    3a84:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    3a88:	e1a0100a 	mov	r1, sl
    3a8c:	e0000590 	mul	r0, r0, r5
    3a90:	e59f4068 	ldr	r4, [pc, #104]	; 3b00 <tt_crypto_hash+0x25c>
    3a94:	01a04003 	moveq	r4, r3
    3a98:	eb0031c7 	bl	101bc <__aeabi_uidiv>
    3a9c:	e1a01007 	mov	r1, r7
    3aa0:	e1a03000 	mov	r3, r0
    3aa4:	e1a02004 	mov	r2, r4
    3aa8:	e59f0054 	ldr	r0, [pc, #84]	; 3b04 <tt_crypto_hash+0x260>
    3aac:	e12fff38 	blx	r8
    3ab0:	e59f1050 	ldr	r1, [pc, #80]	; 3b08 <tt_crypto_hash+0x264>
    3ab4:	e5960008 	ldr	r0, [r6, #8]
    3ab8:	e891000c 	ldm	r1, {r2, r3}
    3abc:	e0922000 	adds	r2, r2, r0
    3ac0:	e2a33000 	adc	r3, r3, #0	; 0x0
    3ac4:	e881000c 	stm	r1, {r2, r3}
    3ac8:	e3a0c000 	mov	ip, #0	; 0x0
    3acc:	e1a0000c 	mov	r0, ip
    3ad0:	e28dd038 	add	sp, sp, #56	; 0x38
    3ad4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3ad8:	99997777 	.word	0x99997777
    3adc:	00010f78 	.word	0x00010f78
    3ae0:	0001292d 	.word	0x0001292d
    3ae4:	0800d764 	.word	0x0800d764
    3ae8:	004c4b40 	.word	0x004c4b40
    3aec:	0001294c 	.word	0x0001294c
    3af0:	00012967 	.word	0x00012967
    3af4:	4e000010 	.word	0x4e000010
    3af8:	4e000030 	.word	0x4e000030
    3afc:	00012982 	.word	0x00012982
    3b00:	00012987 	.word	0x00012987
    3b04:	0001298e 	.word	0x0001298e
    3b08:	0800d1c8 	.word	0x0800d1c8

00003b0c <tt_crypto_aes>:
    3b0c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3b10:	e24dd014 	sub	sp, sp, #20	; 0x14
    3b14:	e1a06000 	mov	r6, r0
    3b18:	e3a01000 	mov	r1, #0	; 0x0
    3b1c:	e28d000c 	add	r0, sp, #12	; 0xc
    3b20:	e3a02502 	mov	r2, #8388608	; 0x800000
    3b24:	eb000284 	bl	453c <tt_rnd_get_data>
    3b28:	e250b000 	subs	fp, r0, #0	; 0x0
    3b2c:	e59f4340 	ldr	r4, [pc, #832]	; 3e74 <tt_crypto_aes+0x368>
    3b30:	059f0340 	ldreq	r0, [pc, #832]	; 3e78 <tt_crypto_aes+0x36c>
    3b34:	0a00005c 	beq	3cac <tt_crypto_aes+0x1a0>
    3b38:	e59d200c 	ldr	r2, [sp, #12]
    3b3c:	e1a0100b 	mov	r1, fp
    3b40:	e3c2200f 	bic	r2, r2, #15	; 0xf
    3b44:	e1a03002 	mov	r3, r2
    3b48:	e58d200c 	str	r2, [sp, #12]
    3b4c:	e59f0328 	ldr	r0, [pc, #808]	; 3e7c <tt_crypto_aes+0x370>
    3b50:	e1a02006 	mov	r2, r6
    3b54:	e1a0e00f 	mov	lr, pc
    3b58:	e594f000 	ldr	pc, [r4]
    3b5c:	e59f031c 	ldr	r0, [pc, #796]	; 3e80 <tt_crypto_aes+0x374>
    3b60:	e1a0e00f 	mov	lr, pc
    3b64:	e594f000 	ldr	pc, [r4]
    3b68:	e1a0100b 	mov	r1, fp
    3b6c:	e59d200c 	ldr	r2, [sp, #12]
    3b70:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3b74:	ebfff621 	bl	1400 <memcpy>
    3b78:	e59f0304 	ldr	r0, [pc, #772]	; 3e84 <tt_crypto_aes+0x378>
    3b7c:	e1a0e00f 	mov	lr, pc
    3b80:	e594f000 	ldr	pc, [r4]
    3b84:	e3a01000 	mov	r1, #0	; 0x0
    3b88:	e59f22f8 	ldr	r2, [pc, #760]	; 3e88 <tt_crypto_aes+0x37c>
    3b8c:	e3a00001 	mov	r0, #1	; 0x1
    3b90:	eb0005d6 	bl	52f0 <ss_aes_init>
    3b94:	e59f02f0 	ldr	r0, [pc, #752]	; 3e8c <tt_crypto_aes+0x380>
    3b98:	e1a0e00f 	mov	lr, pc
    3b9c:	e594f000 	ldr	pc, [r4]
    3ba0:	e3560000 	cmp	r6, #0	; 0x0
    3ba4:	159d500c 	ldrne	r5, [sp, #12]
    3ba8:	13a0a000 	movne	sl, #0	; 0x0
    3bac:	12868901 	addne	r8, r6, #16384	; 0x4000
    3bb0:	1a000022 	bne	3c40 <tt_crypto_aes+0x134>
    3bb4:	ea000025 	b	3c50 <tt_crypto_aes+0x144>
    3bb8:	e59d100c 	ldr	r1, [sp, #12]
    3bbc:	e3550902 	cmp	r5, #32768	; 0x8000
    3bc0:	b1a07005 	movlt	r7, r5
    3bc4:	a3a07902 	movge	r7, #32768	; 0x8000
    3bc8:	e0651001 	rsb	r1, r5, r1
    3bcc:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3bd0:	e1a02007 	mov	r2, r7
    3bd4:	ebfff609 	bl	1400 <memcpy>
    3bd8:	e3a00000 	mov	r0, #0	; 0x0
    3bdc:	eb00136e 	bl	899c <get_timer>
    3be0:	e3550901 	cmp	r5, #16384	; 0x4000
    3be4:	b1a02005 	movlt	r2, r5
    3be8:	a3a02901 	movge	r2, #16384	; 0x4000
    3bec:	e1a04000 	mov	r4, r0
    3bf0:	e1a01006 	mov	r1, r6
    3bf4:	e1a00006 	mov	r0, r6
    3bf8:	eb0005ab 	bl	52ac <ss_aes_data>
    3bfc:	e3570902 	cmp	r7, #32768	; 0x8000
    3c00:	1a000003 	bne	3c14 <tt_crypto_aes+0x108>
    3c04:	e1a01008 	mov	r1, r8
    3c08:	e3a02901 	mov	r2, #16384	; 0x4000
    3c0c:	e1a00008 	mov	r0, r8
    3c10:	eb0005a5 	bl	52ac <ss_aes_data>
    3c14:	e1a00004 	mov	r0, r4
    3c18:	eb00135f 	bl	899c <get_timer>
    3c1c:	e1a04000 	mov	r4, r0
    3c20:	e59d000c 	ldr	r0, [sp, #12]
    3c24:	e1a01006 	mov	r1, r6
    3c28:	e0650000 	rsb	r0, r5, r0
    3c2c:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    3c30:	e1a02007 	mov	r2, r7
    3c34:	ebfff5f1 	bl	1400 <memcpy>
    3c38:	e08aa004 	add	sl, sl, r4
    3c3c:	e0675005 	rsb	r5, r7, r5
    3c40:	e3550000 	cmp	r5, #0	; 0x0
    3c44:	e1a00006 	mov	r0, r6
    3c48:	1affffda 	bne	3bb8 <tt_crypto_aes+0xac>
    3c4c:	ea000009 	b	3c78 <tt_crypto_aes+0x16c>
    3c50:	e1a00006 	mov	r0, r6
    3c54:	eb001350 	bl	899c <get_timer>
    3c58:	e1a04000 	mov	r4, r0
    3c5c:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3c60:	e1a01000 	mov	r1, r0
    3c64:	e59d200c 	ldr	r2, [sp, #12]
    3c68:	eb00058f 	bl	52ac <ss_aes_data>
    3c6c:	e1a00004 	mov	r0, r4
    3c70:	eb001349 	bl	899c <get_timer>
    3c74:	e1a0a000 	mov	sl, r0
    3c78:	e59d300c 	ldr	r3, [sp, #12]
    3c7c:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3c80:	e1a0100b 	mov	r1, fp
    3c84:	e3a02010 	mov	r2, #16	; 0x10
    3c88:	e58d3004 	str	r3, [sp, #4]
    3c8c:	ebfff5fc 	bl	1484 <memcmp>
    3c90:	e3500000 	cmp	r0, #0	; 0x0
    3c94:	e59f41d8 	ldr	r4, [pc, #472]	; 3e74 <tt_crypto_aes+0x368>
    3c98:	1a000007 	bne	3cbc <tt_crypto_aes+0x1b0>
    3c9c:	e59f01ec 	ldr	r0, [pc, #492]	; 3e90 <tt_crypto_aes+0x384>
    3ca0:	e1a0e00f 	mov	lr, pc
    3ca4:	e594f000 	ldr	pc, [r4]
    3ca8:	e59f01e4 	ldr	r0, [pc, #484]	; 3e94 <tt_crypto_aes+0x388>
    3cac:	e1a0e00f 	mov	lr, pc
    3cb0:	e594f000 	ldr	pc, [r4]
    3cb4:	e3e00000 	mvn	r0, #0	; 0x0
    3cb8:	ea00006b 	b	3e6c <tt_crypto_aes+0x360>
    3cbc:	e3a00000 	mov	r0, #0	; 0x0
    3cc0:	e1a01000 	mov	r1, r0
    3cc4:	e59f21bc 	ldr	r2, [pc, #444]	; 3e88 <tt_crypto_aes+0x37c>
    3cc8:	eb000588 	bl	52f0 <ss_aes_init>
    3ccc:	e59f01c4 	ldr	r0, [pc, #452]	; 3e98 <tt_crypto_aes+0x38c>
    3cd0:	e1a0e00f 	mov	lr, pc
    3cd4:	e594f000 	ldr	pc, [r4]
    3cd8:	e3560000 	cmp	r6, #0	; 0x0
    3cdc:	159d500c 	ldrne	r5, [sp, #12]
    3ce0:	13a08000 	movne	r8, #0	; 0x0
    3ce4:	12869901 	addne	r9, r6, #16384	; 0x4000
    3ce8:	1a000022 	bne	3d78 <tt_crypto_aes+0x26c>
    3cec:	ea000025 	b	3d88 <tt_crypto_aes+0x27c>
    3cf0:	eb001329 	bl	899c <get_timer>
    3cf4:	e59d100c 	ldr	r1, [sp, #12]
    3cf8:	e3550902 	cmp	r5, #32768	; 0x8000
    3cfc:	b1a07005 	movlt	r7, r5
    3d00:	a3a07902 	movge	r7, #32768	; 0x8000
    3d04:	e0651001 	rsb	r1, r5, r1
    3d08:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3d0c:	e1a02007 	mov	r2, r7
    3d10:	e1a04000 	mov	r4, r0
    3d14:	e1a00006 	mov	r0, r6
    3d18:	ebfff5b8 	bl	1400 <memcpy>
    3d1c:	e3550901 	cmp	r5, #16384	; 0x4000
    3d20:	b1a02005 	movlt	r2, r5
    3d24:	a3a02901 	movge	r2, #16384	; 0x4000
    3d28:	e1a00006 	mov	r0, r6
    3d2c:	e1a01006 	mov	r1, r6
    3d30:	eb00055d 	bl	52ac <ss_aes_data>
    3d34:	e3570902 	cmp	r7, #32768	; 0x8000
    3d38:	1a000003 	bne	3d4c <tt_crypto_aes+0x240>
    3d3c:	e1a01009 	mov	r1, r9
    3d40:	e3a02901 	mov	r2, #16384	; 0x4000
    3d44:	e1a00009 	mov	r0, r9
    3d48:	eb000557 	bl	52ac <ss_aes_data>
    3d4c:	e1a00004 	mov	r0, r4
    3d50:	eb001311 	bl	899c <get_timer>
    3d54:	e1a04000 	mov	r4, r0
    3d58:	e59d000c 	ldr	r0, [sp, #12]
    3d5c:	e1a01006 	mov	r1, r6
    3d60:	e0650000 	rsb	r0, r5, r0
    3d64:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    3d68:	e1a02007 	mov	r2, r7
    3d6c:	ebfff5a3 	bl	1400 <memcpy>
    3d70:	e0888004 	add	r8, r8, r4
    3d74:	e0675005 	rsb	r5, r7, r5
    3d78:	e3a00000 	mov	r0, #0	; 0x0
    3d7c:	e1550000 	cmp	r5, r0
    3d80:	1affffda 	bne	3cf0 <tt_crypto_aes+0x1e4>
    3d84:	ea000009 	b	3db0 <tt_crypto_aes+0x2a4>
    3d88:	e1a00006 	mov	r0, r6
    3d8c:	eb001302 	bl	899c <get_timer>
    3d90:	e1a04000 	mov	r4, r0
    3d94:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3d98:	e1a01000 	mov	r1, r0
    3d9c:	e59d200c 	ldr	r2, [sp, #12]
    3da0:	eb000541 	bl	52ac <ss_aes_data>
    3da4:	e1a00004 	mov	r0, r4
    3da8:	eb0012fb 	bl	899c <get_timer>
    3dac:	e1a08000 	mov	r8, r0
    3db0:	e59d3004 	ldr	r3, [sp, #4]
    3db4:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    3db8:	e1a00543 	asr	r0, r3, #10
    3dbc:	e1a0100a 	mov	r1, sl
    3dc0:	e0000094 	mul	r0, r4, r0
    3dc4:	eb00317e 	bl	103c4 <__aeabi_idiv>
    3dc8:	e1a06000 	mov	r6, r0
    3dcc:	e59d000c 	ldr	r0, [sp, #12]
    3dd0:	e1a01008 	mov	r1, r8
    3dd4:	e1a00540 	asr	r0, r0, #10
    3dd8:	e0000094 	mul	r0, r4, r0
    3ddc:	eb003178 	bl	103c4 <__aeabi_idiv>
    3de0:	e59f408c 	ldr	r4, [pc, #140]	; 3e74 <tt_crypto_aes+0x368>
    3de4:	e1a05000 	mov	r5, r0
    3de8:	e59f00ac 	ldr	r0, [pc, #172]	; 3e9c <tt_crypto_aes+0x390>
    3dec:	e1a0e00f 	mov	lr, pc
    3df0:	e594f000 	ldr	pc, [r4]
    3df4:	e1a0000b 	mov	r0, fp
    3df8:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    3dfc:	e59d200c 	ldr	r2, [sp, #12]
    3e00:	e3a0300c 	mov	r3, #12	; 0xc
    3e04:	eb00014f 	bl	4348 <tt_cmp_buffer>
    3e08:	e3500000 	cmp	r0, #0	; 0x0
    3e0c:	0a000006 	beq	3e2c <tt_crypto_aes+0x320>
    3e10:	e1a03000 	mov	r3, r0
    3e14:	e1a01006 	mov	r1, r6
    3e18:	e1a02005 	mov	r2, r5
    3e1c:	e59f007c 	ldr	r0, [pc, #124]	; 3ea0 <tt_crypto_aes+0x394>
    3e20:	e1a0e00f 	mov	lr, pc
    3e24:	e594f000 	ldr	pc, [r4]
    3e28:	ea000004 	b	3e40 <tt_crypto_aes+0x334>
    3e2c:	e1a01006 	mov	r1, r6
    3e30:	e1a02005 	mov	r2, r5
    3e34:	e59f0068 	ldr	r0, [pc, #104]	; 3ea4 <tt_crypto_aes+0x398>
    3e38:	e1a0e00f 	mov	lr, pc
    3e3c:	e594f000 	ldr	pc, [r4]
    3e40:	e59f0060 	ldr	r0, [pc, #96]	; 3ea8 <tt_crypto_aes+0x39c>
    3e44:	e59d100c 	ldr	r1, [sp, #12]
    3e48:	e890000c 	ldm	r0, {r2, r3}
    3e4c:	e0922001 	adds	r2, r2, r1
    3e50:	e0a33fc1 	adc	r3, r3, r1, asr #31
    3e54:	e880000c 	stm	r0, {r2, r3}
    3e58:	e59f0030 	ldr	r0, [pc, #48]	; 3e90 <tt_crypto_aes+0x384>
    3e5c:	e59f3010 	ldr	r3, [pc, #16]	; 3e74 <tt_crypto_aes+0x368>
    3e60:	e1a0e00f 	mov	lr, pc
    3e64:	e593f000 	ldr	pc, [r3]
    3e68:	e3a00000 	mov	r0, #0	; 0x0
    3e6c:	e28dd014 	add	sp, sp, #20	; 0x14
    3e70:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    3e74:	0800d764 	.word	0x0800d764
    3e78:	000129ae 	.word	0x000129ae
    3e7c:	000129d1 	.word	0x000129d1
    3e80:	00012419 	.word	0x00012419
    3e84:	00013ffa 	.word	0x00013ffa
    3e88:	0800d108 	.word	0x0800d108
    3e8c:	00014095 	.word	0x00014095
    3e90:	0001250e 	.word	0x0001250e
    3e94:	000129fd 	.word	0x000129fd
    3e98:	000132c0 	.word	0x000132c0
    3e9c:	0001275f 	.word	0x0001275f
    3ea0:	00012a1e 	.word	0x00012a1e
    3ea4:	00012a5c 	.word	0x00012a5c
    3ea8:	0800d1c0 	.word	0x0800d1c0

00003eac <do_tt_crypto>:
    3eac:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    3eb0:	e3520001 	cmp	r2, #1	; 0x1
    3eb4:	e24dd004 	sub	sp, sp, #4	; 0x4
    3eb8:	d3a07000 	movle	r7, #0	; 0x0
    3ebc:	da000009 	ble	3ee8 <do_tt_crypto+0x3c>
    3ec0:	e5930004 	ldr	r0, [r3, #4]
    3ec4:	e3a01000 	mov	r1, #0	; 0x0
    3ec8:	e3a02010 	mov	r2, #16	; 0x10
    3ecc:	eb0024b6 	bl	d1ac <simple_strtoul>
    3ed0:	e1a07000 	mov	r7, r0
    3ed4:	e1a01007 	mov	r1, r7
    3ed8:	e59f0114 	ldr	r0, [pc, #276]	; 3ff4 <do_tt_crypto+0x148>
    3edc:	e59f3114 	ldr	r3, [pc, #276]	; 3ff8 <do_tt_crypto+0x14c>
    3ee0:	e1a0e00f 	mov	lr, pc
    3ee4:	e593f000 	ldr	pc, [r3]
    3ee8:	e3a00000 	mov	r0, #0	; 0x0
    3eec:	eb0012aa 	bl	899c <get_timer>
    3ef0:	e59f1104 	ldr	r1, [pc, #260]	; 3ffc <do_tt_crypto+0x150>
    3ef4:	e3a02000 	mov	r2, #0	; 0x0
    3ef8:	e3a03000 	mov	r3, #0	; 0x0
    3efc:	e881000c 	stm	r1, {r2, r3}
    3f00:	e59f10f8 	ldr	r1, [pc, #248]	; 4000 <do_tt_crypto+0x154>
    3f04:	e59fa0ec 	ldr	sl, [pc, #236]	; 3ff8 <do_tt_crypto+0x14c>
    3f08:	e881000c 	stm	r1, {r2, r3}
    3f0c:	e1a08000 	mov	r8, r0
    3f10:	e3a06000 	mov	r6, #0	; 0x0
    3f14:	e3a01006 	mov	r1, #6	; 0x6
    3f18:	e3a00000 	mov	r0, #0	; 0x0
    3f1c:	eb00015d 	bl	4498 <tt_rnd_get_int>
    3f20:	e3a01002 	mov	r1, #2	; 0x2
    3f24:	e1a04000 	mov	r4, r0
    3f28:	e3a00000 	mov	r0, #0	; 0x0
    3f2c:	eb000159 	bl	4498 <tt_rnd_get_int>
    3f30:	e1a01006 	mov	r1, r6
    3f34:	e1a03000 	mov	r3, r0
    3f38:	e1a05000 	mov	r5, r0
    3f3c:	e1a02004 	mov	r2, r4
    3f40:	e59f00bc 	ldr	r0, [pc, #188]	; 4004 <do_tt_crypto+0x158>
    3f44:	e1a0e00f 	mov	lr, pc
    3f48:	e59af000 	ldr	pc, [sl]
    3f4c:	e3540003 	cmp	r4, #3	; 0x3
    3f50:	9a000004 	bls	3f68 <do_tt_crypto+0xbc>
    3f54:	e3550000 	cmp	r5, #0	; 0x0
    3f58:	11a00007 	movne	r0, r7
    3f5c:	03a00000 	moveq	r0, #0	; 0x0
    3f60:	ebfffee9 	bl	3b0c <tt_crypto_aes>
    3f64:	ea000003 	b	3f78 <do_tt_crypto+0xcc>
    3f68:	e3550000 	cmp	r5, #0	; 0x0
    3f6c:	11a00007 	movne	r0, r7
    3f70:	03a00000 	moveq	r0, #0	; 0x0
    3f74:	ebfffe4a 	bl	38a4 <tt_crypto_hash>
    3f78:	eb0028a2 	bl	e208 <tstc>
    3f7c:	e3500000 	cmp	r0, #0	; 0x0
    3f80:	e2866001 	add	r6, r6, #1	; 0x1
    3f84:	0affffe2 	beq	3f14 <do_tt_crypto+0x68>
    3f88:	eb00289f 	bl	e20c <getc>
    3f8c:	e3500003 	cmp	r0, #3	; 0x3
    3f90:	1affffdf 	bne	3f14 <do_tt_crypto+0x68>
    3f94:	e1a00008 	mov	r0, r8
    3f98:	eb00127f 	bl	899c <get_timer>
    3f9c:	e59f305c 	ldr	r3, [pc, #92]	; 4000 <do_tt_crypto+0x154>
    3fa0:	e59f2054 	ldr	r2, [pc, #84]	; 3ffc <do_tt_crypto+0x150>
    3fa4:	e5934000 	ldr	r4, [r3]
    3fa8:	e5925000 	ldr	r5, [r2]
    3fac:	e5933004 	ldr	r3, [r3, #4]
    3fb0:	e5922004 	ldr	r2, [r2, #4]
    3fb4:	e30e1a60 	movw	r1, #60000	; 0xea60
    3fb8:	e1a04a24 	lsr	r4, r4, #20
    3fbc:	e1a05a25 	lsr	r5, r5, #20
    3fc0:	e1844603 	orr	r4, r4, r3, lsl #12
    3fc4:	e1855602 	orr	r5, r5, r2, lsl #12
    3fc8:	eb00307b 	bl	101bc <__aeabi_uidiv>
    3fcc:	e1a01004 	mov	r1, r4
    3fd0:	e1a03000 	mov	r3, r0
    3fd4:	e1a02005 	mov	r2, r5
    3fd8:	e59f0028 	ldr	r0, [pc, #40]	; 4008 <do_tt_crypto+0x15c>
    3fdc:	e59fc014 	ldr	ip, [pc, #20]	; 3ff8 <do_tt_crypto+0x14c>
    3fe0:	e1a0e00f 	mov	lr, pc
    3fe4:	e59cf000 	ldr	pc, [ip]
    3fe8:	e3a00000 	mov	r0, #0	; 0x0
    3fec:	e28dd004 	add	sp, sp, #4	; 0x4
    3ff0:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    3ff4:	00012a8d 	.word	0x00012a8d
    3ff8:	0800d764 	.word	0x0800d764
    3ffc:	0800d1c8 	.word	0x0800d1c8
    4000:	0800d1c0 	.word	0x0800d1c0
    4004:	00012ab3 	.word	0x00012ab3
    4008:	00012ad2 	.word	0x00012ad2

0000400c <tt_ff_trolling>:
    400c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4010:	e3a01001 	mov	r1, #1	; 0x1
    4014:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    4018:	e58d200c 	str	r2, [sp, #12]
    401c:	e58d3008 	str	r3, [sp, #8]
    4020:	e1a05000 	mov	r5, r0
    4024:	eb00046e 	bl	51e4 <vs_assign_by_name>
    4028:	e2506000 	subs	r6, r0, #0	; 0x0
    402c:	0a000005 	beq	4048 <tt_ff_trolling+0x3c>
    4030:	e1a01005 	mov	r1, r5
    4034:	e59f0258 	ldr	r0, [pc, #600]	; 4294 <tt_ff_trolling+0x288>
    4038:	e59f3258 	ldr	r3, [pc, #600]	; 4298 <tt_ff_trolling+0x28c>
    403c:	e1a0e00f 	mov	lr, pc
    4040:	e593f000 	ldr	pc, [r3]
    4044:	ea00000c 	b	407c <tt_ff_trolling+0x70>
    4048:	e3a0000e 	mov	r0, #14	; 0xe
    404c:	eb0002f1 	bl	4c18 <vs_align>
    4050:	e1a01000 	mov	r1, r0
    4054:	e28d0018 	add	r0, sp, #24	; 0x18
    4058:	e5201004 	str	r1, [r0, #-4]!
    405c:	e1a02001 	mov	r2, r1
    4060:	eb000135 	bl	453c <tt_rnd_get_data>
    4064:	e250a000 	subs	sl, r0, #0	; 0x0
    4068:	e59f4228 	ldr	r4, [pc, #552]	; 4298 <tt_ff_trolling+0x28c>
    406c:	1a000004 	bne	4084 <tt_ff_trolling+0x78>
    4070:	e59f0224 	ldr	r0, [pc, #548]	; 429c <tt_ff_trolling+0x290>
    4074:	e1a0e00f 	mov	lr, pc
    4078:	e594f000 	ldr	pc, [r4]
    407c:	e3e00000 	mvn	r0, #0	; 0x0
    4080:	ea000073 	b	4254 <tt_ff_trolling+0x248>
    4084:	e59d2014 	ldr	r2, [sp, #20]
    4088:	e1a0100a 	mov	r1, sl
    408c:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4090:	ebfff4da 	bl	1400 <memcpy>
    4094:	e3a01501 	mov	r1, #4194304	; 0x400000
    4098:	e59f0200 	ldr	r0, [pc, #512]	; 42a0 <tt_ff_trolling+0x294>
    409c:	e1a0e00f 	mov	lr, pc
    40a0:	e594f000 	ldr	pc, [r4]
    40a4:	e1a00005 	mov	r0, r5
    40a8:	eb00035b 	bl	4e1c <vs_device_id>
    40ac:	e3a04000 	mov	r4, #0	; 0x0
    40b0:	e3a05000 	mov	r5, #0	; 0x0
    40b4:	e1a0b006 	mov	fp, r6
    40b8:	e3a08501 	mov	r8, #4194304	; 0x400000
    40bc:	e3a09000 	mov	r9, #0	; 0x0
    40c0:	e1a07000 	mov	r7, r0
    40c4:	ea000021 	b	4150 <tt_ff_trolling+0x144>
    40c8:	eb00033f 	bl	4dcc <vs_device_erasable>
    40cc:	e3500000 	cmp	r0, #0	; 0x0
    40d0:	0a000003 	beq	40e4 <tt_ff_trolling+0xd8>
    40d4:	e1a00004 	mov	r0, r4
    40d8:	e1a01005 	mov	r1, r5
    40dc:	e3a02501 	mov	r2, #4194304	; 0x400000
    40e0:	eb000290 	bl	4b28 <vs_erase>
    40e4:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    40e8:	e5816000 	str	r6, [r1]
    40ec:	e1a00001 	mov	r0, r1
    40f0:	e59d1014 	ldr	r1, [sp, #20]
    40f4:	e1a02004 	mov	r2, r4
    40f8:	e1a03005 	mov	r3, r5
    40fc:	e88d0802 	stm	sp, {r1, fp}
    4100:	eb000272 	bl	4ad0 <vs_write>
    4104:	e3500000 	cmp	r0, #0	; 0x0
    4108:	aa000006 	bge	4128 <tt_ff_trolling+0x11c>
    410c:	e1a02004 	mov	r2, r4
    4110:	e1a03005 	mov	r3, r5
    4114:	e59f0188 	ldr	r0, [pc, #392]	; 42a4 <tt_ff_trolling+0x298>
    4118:	e59f1178 	ldr	r1, [pc, #376]	; 4298 <tt_ff_trolling+0x28c>
    411c:	e1a0e00f 	mov	lr, pc
    4120:	e591f000 	ldr	pc, [r1]
    4124:	ea000012 	b	4174 <tt_ff_trolling+0x168>
    4128:	e316001f 	tst	r6, #31	; 0x1f
    412c:	1a000004 	bne	4144 <tt_ff_trolling+0x138>
    4130:	e1a01106 	lsl	r1, r6, #2
    4134:	e59f016c 	ldr	r0, [pc, #364]	; 42a8 <tt_ff_trolling+0x29c>
    4138:	e59f3158 	ldr	r3, [pc, #344]	; 4298 <tt_ff_trolling+0x28c>
    413c:	e1a0e00f 	mov	lr, pc
    4140:	e593f000 	ldr	pc, [r3]
    4144:	e0944008 	adds	r4, r4, r8
    4148:	e0a55009 	adc	r5, r5, r9
    414c:	e2866001 	add	r6, r6, #1	; 0x1
    4150:	e59d1008 	ldr	r1, [sp, #8]
    4154:	e1a00007 	mov	r0, r7
    4158:	e1510005 	cmp	r1, r5
    415c:	8affffd9 	bhi	40c8 <tt_ff_trolling+0xbc>
    4160:	1a00003d 	bne	425c <tt_ff_trolling+0x250>
    4164:	e59d300c 	ldr	r3, [sp, #12]
    4168:	e1530004 	cmp	r3, r4
    416c:	8affffd5 	bhi	40c8 <tt_ff_trolling+0xbc>
    4170:	ea000039 	b	425c <tt_ff_trolling+0x250>
    4174:	e59f411c 	ldr	r4, [pc, #284]	; 4298 <tt_ff_trolling+0x28c>
    4178:	e59f012c 	ldr	r0, [pc, #300]	; 42ac <tt_ff_trolling+0x2a0>
    417c:	e3a01501 	mov	r1, #4194304	; 0x400000
    4180:	e3a06000 	mov	r6, #0	; 0x0
    4184:	e1a0e00f 	mov	lr, pc
    4188:	e594f000 	ldr	pc, [r4]
    418c:	e28a7004 	add	r7, sl, #4	; 0x4
    4190:	e3a04000 	mov	r4, #0	; 0x0
    4194:	e3a05000 	mov	r5, #0	; 0x0
    4198:	e1a0b006 	mov	fp, r6
    419c:	e3a0a44e 	mov	sl, #1308622848	; 0x4e000000
    41a0:	e3a08501 	mov	r8, #4194304	; 0x400000
    41a4:	e3a09000 	mov	r9, #0	; 0x0
    41a8:	ea00001e 	b	4228 <tt_ff_trolling+0x21c>
    41ac:	e59d1014 	ldr	r1, [sp, #20]
    41b0:	e88d0802 	stm	sp, {r1, fp}
    41b4:	eb000345 	bl	4ed0 <vs_read>
    41b8:	e316001f 	tst	r6, #31	; 0x1f
    41bc:	1a000004 	bne	41d4 <tt_ff_trolling+0x1c8>
    41c0:	e1a01106 	lsl	r1, r6, #2
    41c4:	e59f00dc 	ldr	r0, [pc, #220]	; 42a8 <tt_ff_trolling+0x29c>
    41c8:	e59f30c8 	ldr	r3, [pc, #200]	; 4298 <tt_ff_trolling+0x28c>
    41cc:	e1a0e00f 	mov	lr, pc
    41d0:	e593f000 	ldr	pc, [r3]
    41d4:	e59a2000 	ldr	r2, [sl]
    41d8:	e1a00007 	mov	r0, r7
    41dc:	e1520006 	cmp	r2, r6
    41e0:	e59f10c8 	ldr	r1, [pc, #200]	; 42b0 <tt_ff_trolling+0x2a4>
    41e4:	e3a0300a 	mov	r3, #10	; 0xa
    41e8:	e2866001 	add	r6, r6, #1	; 0x1
    41ec:	1a000004 	bne	4204 <tt_ff_trolling+0x1f8>
    41f0:	e59d2014 	ldr	r2, [sp, #20]
    41f4:	e2422004 	sub	r2, r2, #4	; 0x4
    41f8:	eb000052 	bl	4348 <tt_cmp_buffer>
    41fc:	e3500000 	cmp	r0, #0	; 0x0
    4200:	0a000006 	beq	4220 <tt_ff_trolling+0x214>
    4204:	e1a02004 	mov	r2, r4
    4208:	e1a03005 	mov	r3, r5
    420c:	e59f00a0 	ldr	r0, [pc, #160]	; 42b4 <tt_ff_trolling+0x2a8>
    4210:	e59f1080 	ldr	r1, [pc, #128]	; 4298 <tt_ff_trolling+0x28c>
    4214:	e1a0e00f 	mov	lr, pc
    4218:	e591f000 	ldr	pc, [r1]
    421c:	ea00001a 	b	428c <tt_ff_trolling+0x280>
    4220:	e0944008 	adds	r4, r4, r8
    4224:	e0a55009 	adc	r5, r5, r9
    4228:	e59d1008 	ldr	r1, [sp, #8]
    422c:	e1a02004 	mov	r2, r4
    4230:	e1510005 	cmp	r1, r5
    4234:	e1a03005 	mov	r3, r5
    4238:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    423c:	8affffda 	bhi	41ac <tt_ff_trolling+0x1a0>
    4240:	1a00000a 	bne	4270 <tt_ff_trolling+0x264>
    4244:	e59d100c 	ldr	r1, [sp, #12]
    4248:	e1510004 	cmp	r1, r4
    424c:	8affffd6 	bhi	41ac <tt_ff_trolling+0x1a0>
    4250:	ea000006 	b	4270 <tt_ff_trolling+0x264>
    4254:	e28dd01c 	add	sp, sp, #28	; 0x1c
    4258:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    425c:	e59f0054 	ldr	r0, [pc, #84]	; 42b8 <tt_ff_trolling+0x2ac>
    4260:	e59f3030 	ldr	r3, [pc, #48]	; 4298 <tt_ff_trolling+0x28c>
    4264:	e1a0e00f 	mov	lr, pc
    4268:	e593f000 	ldr	pc, [r3]
    426c:	eaffffc0 	b	4174 <tt_ff_trolling+0x168>
    4270:	e59f0040 	ldr	r0, [pc, #64]	; 42b8 <tt_ff_trolling+0x2ac>
    4274:	e59f401c 	ldr	r4, [pc, #28]	; 4298 <tt_ff_trolling+0x28c>
    4278:	e1a0e00f 	mov	lr, pc
    427c:	e594f000 	ldr	pc, [r4]
    4280:	e59f0034 	ldr	r0, [pc, #52]	; 42bc <tt_ff_trolling+0x2b0>
    4284:	e1a0e00f 	mov	lr, pc
    4288:	e594f000 	ldr	pc, [r4]
    428c:	e3a00000 	mov	r0, #0	; 0x0
    4290:	eaffffef 	b	4254 <tt_ff_trolling+0x248>
    4294:	00012b56 	.word	0x00012b56
    4298:	0800d764 	.word	0x0800d764
    429c:	00012b73 	.word	0x00012b73
    42a0:	00012b92 	.word	0x00012b92
    42a4:	00012bb0 	.word	0x00012bb0
    42a8:	00012bc8 	.word	0x00012bc8
    42ac:	00012bcf 	.word	0x00012bcf
    42b0:	4e000004 	.word	0x4e000004
    42b4:	00012bef 	.word	0x00012bef
    42b8:	00012bff 	.word	0x00012bff
    42bc:	00012c06 	.word	0x00012c06

000042c0 <do_tt_ff>:
    42c0:	e92d4030 	push	{r4, r5, lr}
    42c4:	e3520001 	cmp	r2, #1	; 0x1
    42c8:	e24dd004 	sub	sp, sp, #4	; 0x4
    42cc:	e1a05003 	mov	r5, r3
    42d0:	ca000005 	bgt	42ec <do_tt_ff+0x2c>
    42d4:	e59f0060 	ldr	r0, [pc, #96]	; 433c <do_tt_ff+0x7c>
    42d8:	e59f3060 	ldr	r3, [pc, #96]	; 4340 <do_tt_ff+0x80>
    42dc:	e1a0e00f 	mov	lr, pc
    42e0:	e593f000 	ldr	pc, [r3]
    42e4:	e3e00000 	mvn	r0, #0	; 0x0
    42e8:	ea000011 	b	4334 <do_tt_ff+0x74>
    42ec:	e3520002 	cmp	r2, #2	; 0x2
    42f0:	03a04000 	moveq	r4, #0	; 0x0
    42f4:	0a000009 	beq	4320 <do_tt_ff+0x60>
    42f8:	e3a01000 	mov	r1, #0	; 0x0
    42fc:	e5930008 	ldr	r0, [r3, #8]
    4300:	e3a0200a 	mov	r2, #10	; 0xa
    4304:	eb0023a8 	bl	d1ac <simple_strtoul>
    4308:	e1a04000 	mov	r4, r0
    430c:	e1a01004 	mov	r1, r4
    4310:	e59f002c 	ldr	r0, [pc, #44]	; 4344 <do_tt_ff+0x84>
    4314:	e59f3024 	ldr	r3, [pc, #36]	; 4340 <do_tt_ff+0x80>
    4318:	e1a0e00f 	mov	lr, pc
    431c:	e593f000 	ldr	pc, [r3]
    4320:	e5950004 	ldr	r0, [r5, #4]
    4324:	e1a03624 	lsr	r3, r4, #12
    4328:	e1a02a04 	lsl	r2, r4, #20
    432c:	ebffff36 	bl	400c <tt_ff_trolling>
    4330:	e3a00000 	mov	r0, #0	; 0x0
    4334:	e28dd004 	add	sp, sp, #4	; 0x4
    4338:	e8bd8030 	pop	{r4, r5, pc}
    433c:	00012c1e 	.word	0x00012c1e
    4340:	0800d764 	.word	0x0800d764
    4344:	00012c64 	.word	0x00012c64

00004348 <tt_cmp_buffer>:
    4348:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    434c:	e3a04000 	mov	r4, #0	; 0x0
    4350:	e24dd004 	sub	sp, sp, #4	; 0x4
    4354:	e1a0a002 	mov	sl, r2
    4358:	e1a07000 	mov	r7, r0
    435c:	e1a06001 	mov	r6, r1
    4360:	e1a08003 	mov	r8, r3
    4364:	e1a09142 	asr	r9, r2, #2
    4368:	e1a05004 	mov	r5, r4
    436c:	e59fb0e4 	ldr	fp, [pc, #228]	; 4458 <tt_cmp_buffer+0x110>
    4370:	ea00001f 	b	43f4 <tt_cmp_buffer+0xac>
    4374:	e5970000 	ldr	r0, [r7]
    4378:	e5961000 	ldr	r1, [r6]
    437c:	e1500001 	cmp	r0, r1
    4380:	0a000018 	beq	43e8 <tt_cmp_buffer+0xa0>
    4384:	e5d72000 	ldrb	r2, [r7]
    4388:	e5d63000 	ldrb	r3, [r6]
    438c:	e1520003 	cmp	r2, r3
    4390:	e5d72001 	ldrb	r2, [r7, #1]
    4394:	e5d63001 	ldrb	r3, [r6, #1]
    4398:	12855001 	addne	r5, r5, #1	; 0x1
    439c:	e1520003 	cmp	r2, r3
    43a0:	e5d72002 	ldrb	r2, [r7, #2]
    43a4:	e5d63002 	ldrb	r3, [r6, #2]
    43a8:	12855001 	addne	r5, r5, #1	; 0x1
    43ac:	e1520003 	cmp	r2, r3
    43b0:	e5d72003 	ldrb	r2, [r7, #3]
    43b4:	e5d63003 	ldrb	r3, [r6, #3]
    43b8:	12855001 	addne	r5, r5, #1	; 0x1
    43bc:	e1520003 	cmp	r2, r3
    43c0:	12855001 	addne	r5, r5, #1	; 0x1
    43c4:	e3580000 	cmp	r8, #0	; 0x0
    43c8:	0a000006 	beq	43e8 <tt_cmp_buffer+0xa0>
    43cc:	e1a02000 	mov	r2, r0
    43d0:	e1a03001 	mov	r3, r1
    43d4:	e59f0080 	ldr	r0, [pc, #128]	; 445c <tt_cmp_buffer+0x114>
    43d8:	e1a01104 	lsl	r1, r4, #2
    43dc:	e1a0e00f 	mov	lr, pc
    43e0:	e59bf000 	ldr	pc, [fp]
    43e4:	e2488001 	sub	r8, r8, #1	; 0x1
    43e8:	e2844001 	add	r4, r4, #1	; 0x1
    43ec:	e2877004 	add	r7, r7, #4	; 0x4
    43f0:	e2866004 	add	r6, r6, #4	; 0x4
    43f4:	e1540009 	cmp	r4, r9
    43f8:	baffffdd 	blt	4374 <tt_cmp_buffer+0x2c>
    43fc:	e3ca9003 	bic	r9, sl, #3	; 0x3
    4400:	e3a04000 	mov	r4, #0	; 0x0
    4404:	e20aa003 	and	sl, sl, #3	; 0x3
    4408:	e59fb048 	ldr	fp, [pc, #72]	; 4458 <tt_cmp_buffer+0x110>
    440c:	ea00000c 	b	4444 <tt_cmp_buffer+0xfc>
    4410:	e7d72004 	ldrb	r2, [r7, r4]
    4414:	e7d63004 	ldrb	r3, [r6, r4]
    4418:	e1520003 	cmp	r2, r3
    441c:	0a000007 	beq	4440 <tt_cmp_buffer+0xf8>
    4420:	e3580000 	cmp	r8, #0	; 0x0
    4424:	e2855001 	add	r5, r5, #1	; 0x1
    4428:	0a000004 	beq	4440 <tt_cmp_buffer+0xf8>
    442c:	e0841009 	add	r1, r4, r9
    4430:	e59f0028 	ldr	r0, [pc, #40]	; 4460 <tt_cmp_buffer+0x118>
    4434:	e1a0e00f 	mov	lr, pc
    4438:	e59bf000 	ldr	pc, [fp]
    443c:	e2488001 	sub	r8, r8, #1	; 0x1
    4440:	e2844001 	add	r4, r4, #1	; 0x1
    4444:	e154000a 	cmp	r4, sl
    4448:	bafffff0 	blt	4410 <tt_cmp_buffer+0xc8>
    444c:	e1a00005 	mov	r0, r5
    4450:	e28dd004 	add	sp, sp, #4	; 0x4
    4454:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4458:	0800d764 	.word	0x0800d764
    445c:	00012c8b 	.word	0x00012c8b
    4460:	00012cb6 	.word	0x00012cb6

00004464 <tt_rnd_seed>:
    4464:	e92d4030 	push	{r4, r5, lr}
    4468:	e59f5024 	ldr	r5, [pc, #36]	; 4494 <tt_rnd_seed+0x30>
    446c:	e24dd004 	sub	sp, sp, #4	; 0x4
    4470:	e3a00000 	mov	r0, #0	; 0x0
    4474:	e5954000 	ldr	r4, [r5]
    4478:	eb001147 	bl	899c <get_timer>
    447c:	e30f3ffc 	movw	r3, #65532	; 0xfffc
    4480:	e6e40070 	uxtab	r0, r4, r0
    4484:	e0003003 	and	r3, r0, r3
    4488:	e5853000 	str	r3, [r5]
    448c:	e28dd004 	add	sp, sp, #4	; 0x4
    4490:	e8bd8030 	pop	{r4, r5, pc}
    4494:	0800d1d0 	.word	0x0800d1d0

00004498 <tt_rnd_get_int>:
    4498:	e59fc094 	ldr	ip, [pc, #148]	; 4534 <tt_rnd_get_int+0x9c>
    449c:	e3a02101 	mov	r2, #1073741824	; 0x40000000
    44a0:	e59c3000 	ldr	r3, [ip]
    44a4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    44a8:	e19330b2 	ldrh	r3, [r3, r2]
    44ac:	e24dd004 	sub	sp, sp, #4	; 0x4
    44b0:	e3c33003 	bic	r3, r3, #3	; 0x3
    44b4:	e6ff3073 	uxth	r3, r3
    44b8:	e19340b2 	ldrh	r4, [r3, r2]
    44bc:	e58c3000 	str	r3, [ip]
    44c0:	e1a07000 	mov	r7, r0
    44c4:	e1a06001 	mov	r6, r1
    44c8:	ebffffe5 	bl	4464 <tt_rnd_seed>
    44cc:	e1a00004 	mov	r0, r4
    44d0:	eb003105 	bl	108ec <__aeabi_ui2d>
    44d4:	e3a02000 	mov	r2, #0	; 0x0
    44d8:	e59f3058 	ldr	r3, [pc, #88]	; 4538 <tt_rnd_get_int+0xa0>
    44dc:	eb0031dd 	bl	10c58 <__aeabi_ddiv>
    44e0:	e1a04000 	mov	r4, r0
    44e4:	e0670006 	rsb	r0, r7, r6
    44e8:	e1a05001 	mov	r5, r1
    44ec:	eb003107 	bl	10910 <__aeabi_i2d>
    44f0:	e1a02000 	mov	r2, r0
    44f4:	e1a03001 	mov	r3, r1
    44f8:	e1a00004 	mov	r0, r4
    44fc:	e1a01005 	mov	r1, r5
    4500:	eb003139 	bl	109ec <__aeabi_dmul>
    4504:	e1a04000 	mov	r4, r0
    4508:	e1a00007 	mov	r0, r7
    450c:	e1a05001 	mov	r5, r1
    4510:	eb0030fe 	bl	10910 <__aeabi_i2d>
    4514:	e1a02000 	mov	r2, r0
    4518:	e1a03001 	mov	r3, r1
    451c:	e1a00004 	mov	r0, r4
    4520:	e1a01005 	mov	r1, r5
    4524:	eb003045 	bl	10640 <__adddf3>
    4528:	eb00324b 	bl	10e5c <__aeabi_d2iz>
    452c:	e28dd004 	add	sp, sp, #4	; 0x4
    4530:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4534:	0800d1d0 	.word	0x0800d1d0
    4538:	40efffe0 	.word	0x40efffe0

0000453c <tt_rnd_get_data>:
    453c:	e92d4010 	push	{r4, lr}
    4540:	e1a04000 	mov	r4, r0
    4544:	e1a00001 	mov	r0, r1
    4548:	e1a01002 	mov	r1, r2
    454c:	ebffffd1 	bl	4498 <tt_rnd_get_int>
    4550:	e59f3010 	ldr	r3, [pc, #16]	; 4568 <tt_rnd_get_data+0x2c>
    4554:	e5840000 	str	r0, [r4]
    4558:	e5930000 	ldr	r0, [r3]
    455c:	e3c00003 	bic	r0, r0, #3	; 0x3
    4560:	e2800101 	add	r0, r0, #1073741824	; 0x40000000
    4564:	e8bd8010 	pop	{r4, pc}
    4568:	0800d1d0 	.word	0x0800d1d0

0000456c <tt_wire_get_opt>:
    456c:	e5900008 	ldr	r0, [r0, #8]
    4570:	e12fff1e 	bx	lr

00004574 <tt_wire_get_len>:
    4574:	e590000c 	ldr	r0, [r0, #12]
    4578:	e12fff1e 	bx	lr

0000457c <tt_wire_make_data>:
    457c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4580:	e24dd004 	sub	sp, sp, #4	; 0x4
    4584:	e1a04000 	mov	r4, r0
    4588:	e1a07002 	mov	r7, r2
    458c:	e1a06003 	mov	r6, r3
    4590:	ebfff39a 	bl	1400 <memcpy>
    4594:	e59f3040 	ldr	r3, [pc, #64]	; 45dc <tt_wire_make_data+0x60>
    4598:	e5846008 	str	r6, [r4, #8]
    459c:	e5843000 	str	r3, [r4]
    45a0:	e59d3018 	ldr	r3, [sp, #24]
    45a4:	e3a05000 	mov	r5, #0	; 0x0
    45a8:	e584300c 	str	r3, [r4, #12]
    45ac:	e1a01007 	mov	r1, r7
    45b0:	e5845004 	str	r5, [r4, #4]
    45b4:	e1a00005 	mov	r0, r5
    45b8:	eb00046a 	bl	5768 <hash_init>
    45bc:	e1a01007 	mov	r1, r7
    45c0:	e1a00004 	mov	r0, r4
    45c4:	eb000442 	bl	56d4 <hash_data>
    45c8:	e2840004 	add	r0, r4, #4	; 0x4
    45cc:	eb000424 	bl	5664 <hash_value>
    45d0:	e1a00005 	mov	r0, r5
    45d4:	e28dd004 	add	sp, sp, #4	; 0x4
    45d8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    45dc:	7766dd80 	.word	0x7766dd80

000045e0 <tt_wire_check_valid>:
    45e0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    45e4:	e5903000 	ldr	r3, [r0]
    45e8:	e59f2084 	ldr	r2, [pc, #132]	; 4674 <tt_wire_check_valid+0x94>
    45ec:	e24dd00c 	sub	sp, sp, #12	; 0xc
    45f0:	e1530002 	cmp	r3, r2
    45f4:	e1a04000 	mov	r4, r0
    45f8:	e1a06001 	mov	r6, r1
    45fc:	e5907004 	ldr	r7, [r0, #4]
    4600:	0a000006 	beq	4620 <tt_wire_check_valid+0x40>
    4604:	e1a01003 	mov	r1, r3
    4608:	e59f0068 	ldr	r0, [pc, #104]	; 4678 <tt_wire_check_valid+0x98>
    460c:	e59f3068 	ldr	r3, [pc, #104]	; 467c <tt_wire_check_valid+0x9c>
    4610:	e1a0e00f 	mov	lr, pc
    4614:	e593f000 	ldr	pc, [r3]
    4618:	e3a00000 	mov	r0, #0	; 0x0
    461c:	ea000012 	b	466c <tt_wire_check_valid+0x8c>
    4620:	e3a05000 	mov	r5, #0	; 0x0
    4624:	e5805004 	str	r5, [r0, #4]
    4628:	e1a00005 	mov	r0, r5
    462c:	eb00044d 	bl	5768 <hash_init>
    4630:	e1a01006 	mov	r1, r6
    4634:	e1a00004 	mov	r0, r4
    4638:	eb000425 	bl	56d4 <hash_data>
    463c:	e28d0004 	add	r0, sp, #4	; 0x4
    4640:	eb000407 	bl	5664 <hash_value>
    4644:	e59d1004 	ldr	r1, [sp, #4]
    4648:	e1510007 	cmp	r1, r7
    464c:	03a00001 	moveq	r0, #1	; 0x1
    4650:	0a000005 	beq	466c <tt_wire_check_valid+0x8c>
    4654:	e59f0024 	ldr	r0, [pc, #36]	; 4680 <tt_wire_check_valid+0xa0>
    4658:	e1a02007 	mov	r2, r7
    465c:	e59f3018 	ldr	r3, [pc, #24]	; 467c <tt_wire_check_valid+0x9c>
    4660:	e1a0e00f 	mov	lr, pc
    4664:	e593f000 	ldr	pc, [r3]
    4668:	e1a00005 	mov	r0, r5
    466c:	e28dd00c 	add	sp, sp, #12	; 0xc
    4670:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4674:	7766dd80 	.word	0x7766dd80
    4678:	00012ce1 	.word	0x00012ce1
    467c:	0800d764 	.word	0x0800d764
    4680:	00012d0a 	.word	0x00012d0a

00004684 <do_tt_wire>:
    4684:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4688:	e3520001 	cmp	r2, #1	; 0x1
    468c:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    4690:	e1a04003 	mov	r4, r3
    4694:	d59f032c 	ldrle	r0, [pc, #812]	; 49c8 <do_tt_wire+0x344>
    4698:	da00002b 	ble	474c <do_tt_wire+0xc8>
    469c:	e3a02003 	mov	r2, #3	; 0x3
    46a0:	e5930004 	ldr	r0, [r3, #4]
    46a4:	e59f1320 	ldr	r1, [pc, #800]	; 49cc <do_tt_wire+0x348>
    46a8:	ebfff31b 	bl	131c <strncmp>
    46ac:	e3500000 	cmp	r0, #0	; 0x0
    46b0:	059f2318 	ldreq	r2, [pc, #792]	; 49d0 <do_tt_wire+0x34c>
    46b4:	0a000006 	beq	46d4 <do_tt_wire+0x50>
    46b8:	e5940004 	ldr	r0, [r4, #4]
    46bc:	e59f1310 	ldr	r1, [pc, #784]	; 49d4 <do_tt_wire+0x350>
    46c0:	e3a02003 	mov	r2, #3	; 0x3
    46c4:	ebfff314 	bl	131c <strncmp>
    46c8:	e3500000 	cmp	r0, #0	; 0x0
    46cc:	1a000009 	bne	46f8 <do_tt_wire+0x74>
    46d0:	e59f2300 	ldr	r2, [pc, #768]	; 49d8 <do_tt_wire+0x354>
    46d4:	e59f3300 	ldr	r3, [pc, #768]	; 49dc <do_tt_wire+0x358>
    46d8:	e3a00000 	mov	r0, #0	; 0x0
    46dc:	e5832000 	str	r2, [r3]
    46e0:	eb0010ad 	bl	899c <get_timer>
    46e4:	e59f82f4 	ldr	r8, [pc, #756]	; 49e0 <do_tt_wire+0x35c>
    46e8:	e59f92ec 	ldr	r9, [pc, #748]	; 49dc <do_tt_wire+0x358>
    46ec:	e1a0a008 	mov	sl, r8
    46f0:	e58d000c 	str	r0, [sp, #12]
    46f4:	ea000006 	b	4714 <do_tt_wire+0x90>
    46f8:	e5941004 	ldr	r1, [r4, #4]
    46fc:	e59f02e0 	ldr	r0, [pc, #736]	; 49e4 <do_tt_wire+0x360>
    4700:	e59f32d8 	ldr	r3, [pc, #728]	; 49e0 <do_tt_wire+0x35c>
    4704:	e1a0e00f 	mov	lr, pc
    4708:	e593f000 	ldr	pc, [r3]
    470c:	e3e00000 	mvn	r0, #0	; 0x0
    4710:	ea0000aa 	b	49c0 <do_tt_wire+0x33c>
    4714:	e59f02cc 	ldr	r0, [pc, #716]	; 49e8 <do_tt_wire+0x364>
    4718:	eb00107b 	bl	890c <udelay>
    471c:	e59f02c8 	ldr	r0, [pc, #712]	; 49ec <do_tt_wire+0x368>
    4720:	e1a0e00f 	mov	lr, pc
    4724:	e598f000 	ldr	pc, [r8]
    4728:	e5993000 	ldr	r3, [r9]
    472c:	e1a0e00f 	mov	lr, pc
    4730:	e593f000 	ldr	pc, [r3]
    4734:	e3500000 	cmp	r0, #0	; 0x0
    4738:	a3a07000 	movge	r7, #0	; 0x0
    473c:	a3a06020 	movge	r6, #32	; 0x20
    4740:	a59fb2a8 	ldrge	fp, [pc, #680]	; 49f0 <do_tt_wire+0x36c>
    4744:	aa000004 	bge	475c <do_tt_wire+0xd8>
    4748:	e59f02a4 	ldr	r0, [pc, #676]	; 49f4 <do_tt_wire+0x370>
    474c:	e59f328c 	ldr	r3, [pc, #652]	; 49e0 <do_tt_wire+0x35c>
    4750:	e1a0e00f 	mov	lr, pc
    4754:	e593f000 	ldr	pc, [r3]
    4758:	eaffffeb 	b	470c <do_tt_wire+0x88>
    475c:	e3570000 	cmp	r7, #0	; 0x0
    4760:	e1a01006 	mov	r1, r6
    4764:	e59f028c 	ldr	r0, [pc, #652]	; 49f8 <do_tt_wire+0x374>
    4768:	1a00002f 	bne	482c <do_tt_wire+0x1a8>
    476c:	e59f0288 	ldr	r0, [pc, #648]	; 49fc <do_tt_wire+0x378>
    4770:	e1a0e00f 	mov	lr, pc
    4774:	e598f000 	ldr	pc, [r8]
    4778:	e1a00007 	mov	r0, r7
    477c:	eb001086 	bl	899c <get_timer>
    4780:	e1a01006 	mov	r1, r6
    4784:	e1a04000 	mov	r4, r0
    4788:	e5993000 	ldr	r3, [r9]
    478c:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4790:	e1a0e00f 	mov	lr, pc
    4794:	e593f004 	ldr	pc, [r3, #4]
    4798:	e1a05000 	mov	r5, r0
    479c:	e1a00004 	mov	r0, r4
    47a0:	eb00107d 	bl	899c <get_timer>
    47a4:	e3550000 	cmp	r5, #0	; 0x0
    47a8:	e1a04000 	mov	r4, r0
    47ac:	e1a01006 	mov	r1, r6
    47b0:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    47b4:	ba00003d 	blt	48b0 <do_tt_wire+0x22c>
    47b8:	ebffff88 	bl	45e0 <tt_wire_check_valid>
    47bc:	e3500000 	cmp	r0, #0	; 0x0
    47c0:	e1a01004 	mov	r1, r4
    47c4:	e1a03546 	asr	r3, r6, #10
    47c8:	1a000007 	bne	47ec <do_tt_wire+0x168>
    47cc:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    47d0:	e0000390 	mul	r0, r0, r3
    47d4:	eb002efa 	bl	103c4 <__aeabi_idiv>
    47d8:	e1a01000 	mov	r1, r0
    47dc:	e59f021c 	ldr	r0, [pc, #540]	; 4a00 <do_tt_wire+0x37c>
    47e0:	e1a0e00f 	mov	lr, pc
    47e4:	e59af000 	ldr	pc, [sl]
    47e8:	eaffffc9 	b	4714 <do_tt_wire+0x90>
    47ec:	e59fc210 	ldr	ip, [pc, #528]	; 4a04 <do_tt_wire+0x380>
    47f0:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    47f4:	e0000392 	mul	r0, r2, r3
    47f8:	e89c000c 	ldm	ip, {r2, r3}
    47fc:	e0922006 	adds	r2, r2, r6
    4800:	e0a33fc6 	adc	r3, r3, r6, asr #31
    4804:	e88c000c 	stm	ip, {r2, r3}
    4808:	eb002eed 	bl	103c4 <__aeabi_idiv>
    480c:	e1a01000 	mov	r1, r0
    4810:	e59f01f0 	ldr	r0, [pc, #496]	; 4a08 <do_tt_wire+0x384>
    4814:	e1a0e00f 	mov	lr, pc
    4818:	e598f000 	ldr	pc, [r8]
    481c:	e3a0344e 	mov	r3, #1308622848	; 0x4e000000
    4820:	e5937008 	ldr	r7, [r3, #8]
    4824:	e593600c 	ldr	r6, [r3, #12]
    4828:	ea000044 	b	4940 <do_tt_wire+0x2bc>
    482c:	e3570001 	cmp	r7, #1	; 0x1
    4830:	1a000030 	bne	48f8 <do_tt_wire+0x274>
    4834:	e1a0e00f 	mov	lr, pc
    4838:	e598f000 	ldr	pc, [r8]
    483c:	e1a01007 	mov	r1, r7
    4840:	e3a00000 	mov	r0, #0	; 0x0
    4844:	ebffff13 	bl	4498 <tt_rnd_get_int>
    4848:	e3a01020 	mov	r1, #32	; 0x20
    484c:	e1a07000 	mov	r7, r0
    4850:	e3a02502 	mov	r2, #8388608	; 0x800000
    4854:	e28d0014 	add	r0, sp, #20	; 0x14
    4858:	ebffff37 	bl	453c <tt_rnd_get_data>
    485c:	e59dc014 	ldr	ip, [sp, #20]
    4860:	e1a01000 	mov	r1, r0
    4864:	e1a02006 	mov	r2, r6
    4868:	e1a03007 	mov	r3, r7
    486c:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4870:	e58dc000 	str	ip, [sp]
    4874:	ebffff40 	bl	457c <tt_wire_make_data>
    4878:	e3a00000 	mov	r0, #0	; 0x0
    487c:	eb001046 	bl	899c <get_timer>
    4880:	e1a01006 	mov	r1, r6
    4884:	e1a05000 	mov	r5, r0
    4888:	e5993000 	ldr	r3, [r9]
    488c:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4890:	e1a0e00f 	mov	lr, pc
    4894:	e593f008 	ldr	pc, [r3, #8]
    4898:	e1a04000 	mov	r4, r0
    489c:	e1a00005 	mov	r0, r5
    48a0:	eb00103d 	bl	899c <get_timer>
    48a4:	e3540000 	cmp	r4, #0	; 0x0
    48a8:	e1a01000 	mov	r1, r0
    48ac:	aa000003 	bge	48c0 <do_tt_wire+0x23c>
    48b0:	e59f0154 	ldr	r0, [pc, #340]	; 4a0c <do_tt_wire+0x388>
    48b4:	e1a0e00f 	mov	lr, pc
    48b8:	e59af000 	ldr	pc, [sl]
    48bc:	eaffff94 	b	4714 <do_tt_wire+0x90>
    48c0:	e89b000c 	ldm	fp, {r2, r3}
    48c4:	e0922006 	adds	r2, r2, r6
    48c8:	e0a33fc6 	adc	r3, r3, r6, asr #31
    48cc:	e59d6014 	ldr	r6, [sp, #20]
    48d0:	e88b000c 	stm	fp, {r2, r3}
    48d4:	e1a00546 	asr	r0, r6, #10
    48d8:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    48dc:	e0000092 	mul	r0, r2, r0
    48e0:	eb002eb7 	bl	103c4 <__aeabi_idiv>
    48e4:	e1a01000 	mov	r1, r0
    48e8:	e59f0120 	ldr	r0, [pc, #288]	; 4a10 <do_tt_wire+0x38c>
    48ec:	e1a0e00f 	mov	lr, pc
    48f0:	e598f000 	ldr	pc, [r8]
    48f4:	ea000011 	b	4940 <do_tt_wire+0x2bc>
    48f8:	e3570003 	cmp	r7, #3	; 0x3
    48fc:	1a000004 	bne	4914 <do_tt_wire+0x290>
    4900:	e59f010c 	ldr	r0, [pc, #268]	; 4a14 <do_tt_wire+0x390>
    4904:	e59f30d4 	ldr	r3, [pc, #212]	; 49e0 <do_tt_wire+0x35c>
    4908:	e1a0e00f 	mov	lr, pc
    490c:	e593f000 	ldr	pc, [r3]
    4910:	ea000010 	b	4958 <do_tt_wire+0x2d4>
    4914:	e3570002 	cmp	r7, #2	; 0x2
    4918:	1a000003 	bne	492c <do_tt_wire+0x2a8>
    491c:	e59f00f4 	ldr	r0, [pc, #244]	; 4a18 <do_tt_wire+0x394>
    4920:	e1a0e00f 	mov	lr, pc
    4924:	e598f000 	ldr	pc, [r8]
    4928:	eaffff79 	b	4714 <do_tt_wire+0x90>
    492c:	e59f00e8 	ldr	r0, [pc, #232]	; 4a1c <do_tt_wire+0x398>
    4930:	e59f30a8 	ldr	r3, [pc, #168]	; 49e0 <do_tt_wire+0x35c>
    4934:	e1a0e00f 	mov	lr, pc
    4938:	e593f000 	ldr	pc, [r3]
    493c:	eafffffe 	b	493c <do_tt_wire+0x2b8>
    4940:	eb002630 	bl	e208 <tstc>
    4944:	e3500000 	cmp	r0, #0	; 0x0
    4948:	0affff83 	beq	475c <do_tt_wire+0xd8>
    494c:	eb00262e 	bl	e20c <getc>
    4950:	e3500003 	cmp	r0, #3	; 0x3
    4954:	1affff80 	bne	475c <do_tt_wire+0xd8>
    4958:	e59d000c 	ldr	r0, [sp, #12]
    495c:	eb00100e 	bl	899c <get_timer>
    4960:	e59f3088 	ldr	r3, [pc, #136]	; 49f0 <do_tt_wire+0x36c>
    4964:	e59f4074 	ldr	r4, [pc, #116]	; 49e0 <do_tt_wire+0x35c>
    4968:	e893000a 	ldm	r3, {r1, r3}
    496c:	e1a01a21 	lsr	r1, r1, #20
    4970:	e1811603 	orr	r1, r1, r3, lsl #12
    4974:	e1a05000 	mov	r5, r0
    4978:	e59f00a0 	ldr	r0, [pc, #160]	; 4a20 <do_tt_wire+0x39c>
    497c:	e1a0e00f 	mov	lr, pc
    4980:	e594f000 	ldr	pc, [r4]
    4984:	e59f3078 	ldr	r3, [pc, #120]	; 4a04 <do_tt_wire+0x380>
    4988:	e59f0094 	ldr	r0, [pc, #148]	; 4a24 <do_tt_wire+0x3a0>
    498c:	e893000a 	ldm	r3, {r1, r3}
    4990:	e1a01a21 	lsr	r1, r1, #20
    4994:	e1811603 	orr	r1, r1, r3, lsl #12
    4998:	e1a0e00f 	mov	lr, pc
    499c:	e594f000 	ldr	pc, [r4]
    49a0:	e30e1a60 	movw	r1, #60000	; 0xea60
    49a4:	e1a00005 	mov	r0, r5
    49a8:	eb002e85 	bl	103c4 <__aeabi_idiv>
    49ac:	e1a01000 	mov	r1, r0
    49b0:	e59f0070 	ldr	r0, [pc, #112]	; 4a28 <do_tt_wire+0x3a4>
    49b4:	e1a0e00f 	mov	lr, pc
    49b8:	e594f000 	ldr	pc, [r4]
    49bc:	e3a00000 	mov	r0, #0	; 0x0
    49c0:	e28dd01c 	add	sp, sp, #28	; 0x1c
    49c4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    49c8:	00012d33 	.word	0x00012d33
    49cc:	00012d7c 	.word	0x00012d7c
    49d0:	00010f88 	.word	0x00010f88
    49d4:	00012d80 	.word	0x00012d80
    49d8:	00010fa0 	.word	0x00010fa0
    49dc:	0800d1e8 	.word	0x0800d1e8
    49e0:	0800d764 	.word	0x0800d764
    49e4:	00012d84 	.word	0x00012d84
    49e8:	001e8480 	.word	0x001e8480
    49ec:	00012da4 	.word	0x00012da4
    49f0:	0800d1e0 	.word	0x0800d1e0
    49f4:	00012dc6 	.word	0x00012dc6
    49f8:	00012e33 	.word	0x00012e33
    49fc:	00012de9 	.word	0x00012de9
    4a00:	00012e15 	.word	0x00012e15
    4a04:	0800d1d8 	.word	0x0800d1d8
    4a08:	00012e24 	.word	0x00012e24
    4a0c:	00012e05 	.word	0x00012e05
    4a10:	00012e4f 	.word	0x00012e4f
    4a14:	00012e5e 	.word	0x00012e5e
    4a18:	00012e70 	.word	0x00012e70
    4a1c:	00012e89 	.word	0x00012e89
    4a20:	00012eb6 	.word	0x00012eb6
    4a24:	00012ed0 	.word	0x00012ed0
    4a28:	00012eee 	.word	0x00012eee

00004a2c <vs_is_assigned>:
    4a2c:	e59f300c 	ldr	r3, [pc, #12]	; 4a40 <vs_is_assigned+0x14>
    4a30:	e5930000 	ldr	r0, [r3]
    4a34:	e2500000 	subs	r0, r0, #0	; 0x0
    4a38:	13a00001 	movne	r0, #1	; 0x1
    4a3c:	e12fff1e 	bx	lr
    4a40:	0800d1f0 	.word	0x0800d1f0

00004a44 <vs_is_opt_available>:
    4a44:	e350000d 	cmp	r0, #13	; 0xd
    4a48:	959f2078 	ldrls	r2, [pc, #120]	; 4ac8 <vs_is_opt_available+0x84>
    4a4c:	93a0302c 	movls	r3, #44	; 0x2c
    4a50:	859f3074 	ldrhi	r3, [pc, #116]	; 4acc <vs_is_opt_available+0x88>
    4a54:	90202093 	mlals	r0, r3, r0, r2
    4a58:	85930000 	ldrhi	r0, [r3]
    4a5c:	e3500000 	cmp	r0, #0	; 0x0
    4a60:	0a000016 	beq	4ac0 <vs_is_opt_available+0x7c>
    4a64:	e3510005 	cmp	r1, #5	; 0x5
    4a68:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    4a6c:	ea000013 	b	4ac0 <vs_is_opt_available+0x7c>
    4a70:	00004a88 	.word	0x00004a88
    4a74:	00004a90 	.word	0x00004a90
    4a78:	00004a98 	.word	0x00004a98
    4a7c:	00004aa0 	.word	0x00004aa0
    4a80:	00004aa8 	.word	0x00004aa8
    4a84:	00004ab0 	.word	0x00004ab0
    4a88:	e590300c 	ldr	r3, [r0, #12]
    4a8c:	ea000008 	b	4ab4 <vs_is_opt_available+0x70>
    4a90:	e5903010 	ldr	r3, [r0, #16]
    4a94:	ea000006 	b	4ab4 <vs_is_opt_available+0x70>
    4a98:	e5903014 	ldr	r3, [r0, #20]
    4a9c:	ea000004 	b	4ab4 <vs_is_opt_available+0x70>
    4aa0:	e5903018 	ldr	r3, [r0, #24]
    4aa4:	ea000002 	b	4ab4 <vs_is_opt_available+0x70>
    4aa8:	e590301c 	ldr	r3, [r0, #28]
    4aac:	ea000000 	b	4ab4 <vs_is_opt_available+0x70>
    4ab0:	e5903020 	ldr	r3, [r0, #32]
    4ab4:	e2530000 	subs	r0, r3, #0	; 0x0
    4ab8:	13a00001 	movne	r0, #1	; 0x1
    4abc:	e12fff1e 	bx	lr
    4ac0:	e3a00000 	mov	r0, #0	; 0x0
    4ac4:	e12fff1e 	bx	lr
    4ac8:	00010fa0 	.word	0x00010fa0
    4acc:	0800d1f0 	.word	0x0800d1f0

00004ad0 <vs_write>:
    4ad0:	e92d4010 	push	{r4, lr}
    4ad4:	e24dd008 	sub	sp, sp, #8	; 0x8
    4ad8:	e59d4010 	ldr	r4, [sp, #16]
    4adc:	e3540000 	cmp	r4, #0	; 0x0
    4ae0:	01a00004 	moveq	r0, r4
    4ae4:	0a00000c 	beq	4b1c <vs_write+0x4c>
    4ae8:	e59f1034 	ldr	r1, [pc, #52]	; 4b24 <vs_write+0x54>
    4aec:	e5911000 	ldr	r1, [r1]
    4af0:	e3510000 	cmp	r1, #0	; 0x0
    4af4:	03e00001 	mvneq	r0, #1	; 0x1
    4af8:	0a000007 	beq	4b1c <vs_write+0x4c>
    4afc:	e591c014 	ldr	ip, [r1, #20]
    4b00:	e35c0000 	cmp	ip, #0	; 0x0
    4b04:	03e00002 	mvneq	r0, #2	; 0x2
    4b08:	0a000003 	beq	4b1c <vs_write+0x4c>
    4b0c:	e59d1014 	ldr	r1, [sp, #20]
    4b10:	e58d4000 	str	r4, [sp]
    4b14:	e58d1004 	str	r1, [sp, #4]
    4b18:	e12fff3c 	blx	ip
    4b1c:	e28dd008 	add	sp, sp, #8	; 0x8
    4b20:	e8bd8010 	pop	{r4, pc}
    4b24:	0800d1f0 	.word	0x0800d1f0

00004b28 <vs_erase>:
    4b28:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4b2c:	e59f302c 	ldr	r3, [pc, #44]	; 4b60 <vs_erase+0x38>
    4b30:	e24dd004 	sub	sp, sp, #4	; 0x4
    4b34:	e5933000 	ldr	r3, [r3]
    4b38:	e3530000 	cmp	r3, #0	; 0x0
    4b3c:	03e00001 	mvneq	r0, #1	; 0x1
    4b40:	0a000004 	beq	4b58 <vs_erase+0x30>
    4b44:	e593301c 	ldr	r3, [r3, #28]
    4b48:	e3530000 	cmp	r3, #0	; 0x0
    4b4c:	03e00002 	mvneq	r0, #2	; 0x2
    4b50:	0a000000 	beq	4b58 <vs_erase+0x30>
    4b54:	e12fff33 	blx	r3
    4b58:	e28dd004 	add	sp, sp, #4	; 0x4
    4b5c:	e8bd8000 	pop	{pc}
    4b60:	0800d1f0 	.word	0x0800d1f0

00004b64 <vs_isbad>:
    4b64:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4b68:	e59f302c 	ldr	r3, [pc, #44]	; 4b9c <vs_isbad+0x38>
    4b6c:	e24dd004 	sub	sp, sp, #4	; 0x4
    4b70:	e5933000 	ldr	r3, [r3]
    4b74:	e3530000 	cmp	r3, #0	; 0x0
    4b78:	03e00001 	mvneq	r0, #1	; 0x1
    4b7c:	0a000004 	beq	4b94 <vs_isbad+0x30>
    4b80:	e5933024 	ldr	r3, [r3, #36]
    4b84:	e3530000 	cmp	r3, #0	; 0x0
    4b88:	03e00002 	mvneq	r0, #2	; 0x2
    4b8c:	0a000000 	beq	4b94 <vs_isbad+0x30>
    4b90:	e12fff33 	blx	r3
    4b94:	e28dd004 	add	sp, sp, #4	; 0x4
    4b98:	e8bd8000 	pop	{pc}
    4b9c:	0800d1f0 	.word	0x0800d1f0

00004ba0 <vs_scrub>:
    4ba0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4ba4:	e59f302c 	ldr	r3, [pc, #44]	; 4bd8 <vs_scrub+0x38>
    4ba8:	e24dd004 	sub	sp, sp, #4	; 0x4
    4bac:	e5933000 	ldr	r3, [r3]
    4bb0:	e3530000 	cmp	r3, #0	; 0x0
    4bb4:	03e00001 	mvneq	r0, #1	; 0x1
    4bb8:	0a000004 	beq	4bd0 <vs_scrub+0x30>
    4bbc:	e5933020 	ldr	r3, [r3, #32]
    4bc0:	e3530000 	cmp	r3, #0	; 0x0
    4bc4:	03e00002 	mvneq	r0, #2	; 0x2
    4bc8:	0a000000 	beq	4bd0 <vs_scrub+0x30>
    4bcc:	e12fff33 	blx	r3
    4bd0:	e28dd004 	add	sp, sp, #4	; 0x4
    4bd4:	e8bd8000 	pop	{pc}
    4bd8:	0800d1f0 	.word	0x0800d1f0

00004bdc <vs_special>:
    4bdc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4be0:	e59f302c 	ldr	r3, [pc, #44]	; 4c14 <vs_special+0x38>
    4be4:	e24dd004 	sub	sp, sp, #4	; 0x4
    4be8:	e5933000 	ldr	r3, [r3]
    4bec:	e3530000 	cmp	r3, #0	; 0x0
    4bf0:	03e00001 	mvneq	r0, #1	; 0x1
    4bf4:	0a000004 	beq	4c0c <vs_special+0x30>
    4bf8:	e5933028 	ldr	r3, [r3, #40]
    4bfc:	e3530000 	cmp	r3, #0	; 0x0
    4c00:	03e00002 	mvneq	r0, #2	; 0x2
    4c04:	0a000000 	beq	4c0c <vs_special+0x30>
    4c08:	e12fff33 	blx	r3
    4c0c:	e28dd004 	add	sp, sp, #4	; 0x4
    4c10:	e8bd8000 	pop	{pc}
    4c14:	0800d1f0 	.word	0x0800d1f0

00004c18 <vs_align>:
    4c18:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4c1c:	e350000e 	cmp	r0, #14	; 0xe
    4c20:	059f3044 	ldreq	r3, [pc, #68]	; 4c6c <vs_align+0x54>
    4c24:	e24dd004 	sub	sp, sp, #4	; 0x4
    4c28:	05930000 	ldreq	r0, [r3]
    4c2c:	0a000004 	beq	4c44 <vs_align+0x2c>
    4c30:	e350000d 	cmp	r0, #13	; 0xd
    4c34:	8a000009 	bhi	4c60 <vs_align+0x48>
    4c38:	e59f2030 	ldr	r2, [pc, #48]	; 4c70 <vs_align+0x58>
    4c3c:	e3a0302c 	mov	r3, #44	; 0x2c
    4c40:	e0202093 	mla	r0, r3, r0, r2
    4c44:	e3500000 	cmp	r0, #0	; 0x0
    4c48:	0a000004 	beq	4c60 <vs_align+0x48>
    4c4c:	e590000c 	ldr	r0, [r0, #12]
    4c50:	e3500000 	cmp	r0, #0	; 0x0
    4c54:	0a000001 	beq	4c60 <vs_align+0x48>
    4c58:	e12fff30 	blx	r0
    4c5c:	ea000000 	b	4c64 <vs_align+0x4c>
    4c60:	e3a00000 	mov	r0, #0	; 0x0
    4c64:	e28dd004 	add	sp, sp, #4	; 0x4
    4c68:	e8bd8000 	pop	{pc}
    4c6c:	0800d1f0 	.word	0x0800d1f0
    4c70:	00010fa0 	.word	0x00010fa0

00004c74 <vs_device_bootable>:
    4c74:	e350000e 	cmp	r0, #14	; 0xe
    4c78:	059f3030 	ldreq	r3, [pc, #48]	; 4cb0 <vs_device_bootable+0x3c>
    4c7c:	05930000 	ldreq	r0, [r3]
    4c80:	0a000004 	beq	4c98 <vs_device_bootable+0x24>
    4c84:	e350000d 	cmp	r0, #13	; 0xd
    4c88:	8a000006 	bhi	4ca8 <vs_device_bootable+0x34>
    4c8c:	e59f2020 	ldr	r2, [pc, #32]	; 4cb4 <vs_device_bootable+0x40>
    4c90:	e3a0302c 	mov	r3, #44	; 0x2c
    4c94:	e0202093 	mla	r0, r3, r0, r2
    4c98:	e3500000 	cmp	r0, #0	; 0x0
    4c9c:	15903008 	ldrne	r3, [r0, #8]
    4ca0:	12030001 	andne	r0, r3, #1	; 0x1
    4ca4:	112fff1e 	bxne	lr
    4ca8:	e3a00000 	mov	r0, #0	; 0x0
    4cac:	e12fff1e 	bx	lr
    4cb0:	0800d1f0 	.word	0x0800d1f0
    4cb4:	00010fa0 	.word	0x00010fa0

00004cb8 <vs_device_burnsrc>:
    4cb8:	e350000e 	cmp	r0, #14	; 0xe
    4cbc:	059f3034 	ldreq	r3, [pc, #52]	; 4cf8 <vs_device_burnsrc+0x40>
    4cc0:	05930000 	ldreq	r0, [r3]
    4cc4:	0a000004 	beq	4cdc <vs_device_burnsrc+0x24>
    4cc8:	e350000d 	cmp	r0, #13	; 0xd
    4ccc:	8a000007 	bhi	4cf0 <vs_device_burnsrc+0x38>
    4cd0:	e59f2024 	ldr	r2, [pc, #36]	; 4cfc <vs_device_burnsrc+0x44>
    4cd4:	e3a0302c 	mov	r3, #44	; 0x2c
    4cd8:	e0202093 	mla	r0, r3, r0, r2
    4cdc:	e3500000 	cmp	r0, #0	; 0x0
    4ce0:	15903008 	ldrne	r3, [r0, #8]
    4ce4:	11a030a3 	lsrne	r3, r3, #1
    4ce8:	12030001 	andne	r0, r3, #1	; 0x1
    4cec:	112fff1e 	bxne	lr
    4cf0:	e3a00000 	mov	r0, #0	; 0x0
    4cf4:	e12fff1e 	bx	lr
    4cf8:	0800d1f0 	.word	0x0800d1f0
    4cfc:	00010fa0 	.word	0x00010fa0

00004d00 <vs_device_burndst>:
    4d00:	e350000e 	cmp	r0, #14	; 0xe
    4d04:	059f3034 	ldreq	r3, [pc, #52]	; 4d40 <vs_device_burndst+0x40>
    4d08:	05930000 	ldreq	r0, [r3]
    4d0c:	0a000004 	beq	4d24 <vs_device_burndst+0x24>
    4d10:	e350000d 	cmp	r0, #13	; 0xd
    4d14:	8a000007 	bhi	4d38 <vs_device_burndst+0x38>
    4d18:	e59f2024 	ldr	r2, [pc, #36]	; 4d44 <vs_device_burndst+0x44>
    4d1c:	e3a0302c 	mov	r3, #44	; 0x2c
    4d20:	e0202093 	mla	r0, r3, r0, r2
    4d24:	e3500000 	cmp	r0, #0	; 0x0
    4d28:	15903008 	ldrne	r3, [r0, #8]
    4d2c:	11a03123 	lsrne	r3, r3, #2
    4d30:	12030001 	andne	r0, r3, #1	; 0x1
    4d34:	112fff1e 	bxne	lr
    4d38:	e3a00000 	mov	r0, #0	; 0x0
    4d3c:	e12fff1e 	bx	lr
    4d40:	0800d1f0 	.word	0x0800d1f0
    4d44:	00010fa0 	.word	0x00010fa0

00004d48 <vs_device_boot0>:
    4d48:	e350000e 	cmp	r0, #14	; 0xe
    4d4c:	059f3034 	ldreq	r3, [pc, #52]	; 4d88 <vs_device_boot0+0x40>
    4d50:	05930000 	ldreq	r0, [r3]
    4d54:	0a000004 	beq	4d6c <vs_device_boot0+0x24>
    4d58:	e350000d 	cmp	r0, #13	; 0xd
    4d5c:	8a000007 	bhi	4d80 <vs_device_boot0+0x38>
    4d60:	e59f2024 	ldr	r2, [pc, #36]	; 4d8c <vs_device_boot0+0x44>
    4d64:	e3a0302c 	mov	r3, #44	; 0x2c
    4d68:	e0202093 	mla	r0, r3, r0, r2
    4d6c:	e3500000 	cmp	r0, #0	; 0x0
    4d70:	15903008 	ldrne	r3, [r0, #8]
    4d74:	11a031a3 	lsrne	r3, r3, #3
    4d78:	12030001 	andne	r0, r3, #1	; 0x1
    4d7c:	112fff1e 	bxne	lr
    4d80:	e3a00000 	mov	r0, #0	; 0x0
    4d84:	e12fff1e 	bx	lr
    4d88:	0800d1f0 	.word	0x0800d1f0
    4d8c:	00010fa0 	.word	0x00010fa0

00004d90 <vs_device_name>:
    4d90:	e350000e 	cmp	r0, #14	; 0xe
    4d94:	059f3028 	ldreq	r3, [pc, #40]	; 4dc4 <vs_device_name+0x34>
    4d98:	05930000 	ldreq	r0, [r3]
    4d9c:	0a000004 	beq	4db4 <vs_device_name+0x24>
    4da0:	e350000d 	cmp	r0, #13	; 0xd
    4da4:	8a000004 	bhi	4dbc <vs_device_name+0x2c>
    4da8:	e59f2018 	ldr	r2, [pc, #24]	; 4dc8 <vs_device_name+0x38>
    4dac:	e3a0302c 	mov	r3, #44	; 0x2c
    4db0:	e0202093 	mla	r0, r3, r0, r2
    4db4:	e3500000 	cmp	r0, #0	; 0x0
    4db8:	112fff1e 	bxne	lr
    4dbc:	e3a00000 	mov	r0, #0	; 0x0
    4dc0:	e12fff1e 	bx	lr
    4dc4:	0800d1f0 	.word	0x0800d1f0
    4dc8:	00010fa0 	.word	0x00010fa0

00004dcc <vs_device_erasable>:
    4dcc:	e350000e 	cmp	r0, #14	; 0xe
    4dd0:	059f3034 	ldreq	r3, [pc, #52]	; 4e0c <vs_device_erasable+0x40>
    4dd4:	05930000 	ldreq	r0, [r3]
    4dd8:	0a000004 	beq	4df0 <vs_device_erasable+0x24>
    4ddc:	e350000d 	cmp	r0, #13	; 0xd
    4de0:	8a000007 	bhi	4e04 <vs_device_erasable+0x38>
    4de4:	e59f2024 	ldr	r2, [pc, #36]	; 4e10 <vs_device_erasable+0x44>
    4de8:	e3a0302c 	mov	r3, #44	; 0x2c
    4dec:	e0202093 	mla	r0, r3, r0, r2
    4df0:	e3500000 	cmp	r0, #0	; 0x0
    4df4:	15903008 	ldrne	r3, [r0, #8]
    4df8:	11a03223 	lsrne	r3, r3, #4
    4dfc:	12030001 	andne	r0, r3, #1	; 0x1
    4e00:	112fff1e 	bxne	lr
    4e04:	e3a00000 	mov	r0, #0	; 0x0
    4e08:	e12fff1e 	bx	lr
    4e0c:	0800d1f0 	.word	0x0800d1f0
    4e10:	00010fa0 	.word	0x00010fa0

00004e14 <vs_device_count>:
    4e14:	e3a0000e 	mov	r0, #14	; 0xe
    4e18:	e12fff1e 	bx	lr

00004e1c <vs_device_id>:
    4e1c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4e20:	e59f7040 	ldr	r7, [pc, #64]	; 4e68 <vs_device_id+0x4c>
    4e24:	e24dd004 	sub	sp, sp, #4	; 0x4
    4e28:	e1a05000 	mov	r5, r0
    4e2c:	e3a04000 	mov	r4, #0	; 0x0
    4e30:	e3a0602c 	mov	r6, #44	; 0x2c
    4e34:	e0217496 	mla	r1, r6, r4, r7
    4e38:	e1a00005 	mov	r0, r5
    4e3c:	e3a02010 	mov	r2, #16	; 0x10
    4e40:	ebfff135 	bl	131c <strncmp>
    4e44:	e3500000 	cmp	r0, #0	; 0x0
    4e48:	0a000003 	beq	4e5c <vs_device_id+0x40>
    4e4c:	e2844001 	add	r4, r4, #1	; 0x1
    4e50:	e354000e 	cmp	r4, #14	; 0xe
    4e54:	1afffff6 	bne	4e34 <vs_device_id+0x18>
    4e58:	e2844001 	add	r4, r4, #1	; 0x1
    4e5c:	e1a00004 	mov	r0, r4
    4e60:	e28dd004 	add	sp, sp, #4	; 0x4
    4e64:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4e68:	00010fa0 	.word	0x00010fa0

00004e6c <vs_is_device>:
    4e6c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4e70:	e350000e 	cmp	r0, #14	; 0xe
    4e74:	059f304c 	ldreq	r3, [pc, #76]	; 4ec8 <vs_is_device+0x5c>
    4e78:	e1a0c001 	mov	ip, r1
    4e7c:	e24dd004 	sub	sp, sp, #4	; 0x4
    4e80:	05931000 	ldreq	r1, [r3]
    4e84:	0a000004 	beq	4e9c <vs_is_device+0x30>
    4e88:	e350000d 	cmp	r0, #13	; 0xd
    4e8c:	8a00000a 	bhi	4ebc <vs_is_device+0x50>
    4e90:	e59f2034 	ldr	r2, [pc, #52]	; 4ecc <vs_is_device+0x60>
    4e94:	e3a0302c 	mov	r3, #44	; 0x2c
    4e98:	e0212093 	mla	r1, r3, r0, r2
    4e9c:	e3510000 	cmp	r1, #0	; 0x0
    4ea0:	0a000005 	beq	4ebc <vs_is_device+0x50>
    4ea4:	e1a0000c 	mov	r0, ip
    4ea8:	e3a02010 	mov	r2, #16	; 0x10
    4eac:	ebfff11a 	bl	131c <strncmp>
    4eb0:	e2700001 	rsbs	r0, r0, #1	; 0x1
    4eb4:	33a00000 	movcc	r0, #0	; 0x0
    4eb8:	ea000000 	b	4ec0 <vs_is_device+0x54>
    4ebc:	e3a00000 	mov	r0, #0	; 0x0
    4ec0:	e28dd004 	add	sp, sp, #4	; 0x4
    4ec4:	e8bd8000 	pop	{pc}
    4ec8:	0800d1f0 	.word	0x0800d1f0
    4ecc:	00010fa0 	.word	0x00010fa0

00004ed0 <vs_read>:
    4ed0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4ed4:	e59f4238 	ldr	r4, [pc, #568]	; 5114 <vs_read+0x244>
    4ed8:	e1a0b000 	mov	fp, r0
    4edc:	e5940000 	ldr	r0, [r4]
    4ee0:	e24dd024 	sub	sp, sp, #36	; 0x24
    4ee4:	e3500000 	cmp	r0, #0	; 0x0
    4ee8:	e1a06002 	mov	r6, r2
    4eec:	e1a07003 	mov	r7, r3
    4ef0:	e59d8048 	ldr	r8, [sp, #72]
    4ef4:	03e05001 	mvneq	r5, #1	; 0x1
    4ef8:	0a000082 	beq	5108 <vs_read+0x238>
    4efc:	e5903010 	ldr	r3, [r0, #16]
    4f00:	e3530000 	cmp	r3, #0	; 0x0
    4f04:	03e05002 	mvneq	r5, #2	; 0x2
    4f08:	0a00007e 	beq	5108 <vs_read+0x238>
    4f0c:	e3580000 	cmp	r8, #0	; 0x0
    4f10:	01a05008 	moveq	r5, r8
    4f14:	0a00007b 	beq	5108 <vs_read+0x238>
    4f18:	e59f11f8 	ldr	r1, [pc, #504]	; 5118 <vs_read+0x248>
    4f1c:	e3a02003 	mov	r2, #3	; 0x3
    4f20:	ebfff0fd 	bl	131c <strncmp>
    4f24:	e3500000 	cmp	r0, #0	; 0x0
    4f28:	1a00000a 	bne	4f58 <vs_read+0x88>
    4f2c:	e59d204c 	ldr	r2, [sp, #76]
    4f30:	e58d8000 	str	r8, [sp]
    4f34:	e58d2004 	str	r2, [sp, #4]
    4f38:	e5941000 	ldr	r1, [r4]
    4f3c:	e1a0000b 	mov	r0, fp
    4f40:	e1a02006 	mov	r2, r6
    4f44:	e1a03007 	mov	r3, r7
    4f48:	e1a0e00f 	mov	lr, pc
    4f4c:	e591f010 	ldr	pc, [r1, #16]
    4f50:	e1a05000 	mov	r5, r0
    4f54:	ea00006b 	b	5108 <vs_read+0x238>
    4f58:	e3a0000e 	mov	r0, #14	; 0xe
    4f5c:	ebffff2d 	bl	4c18 <vs_align>
    4f60:	e2402001 	sub	r2, r0, #1	; 0x1
    4f64:	e0124006 	ands	r4, r2, r6
    4f68:	10644000 	rsbne	r4, r4, r0
    4f6c:	e0643008 	rsb	r3, r4, r8
    4f70:	e0032002 	and	r2, r3, r2
    4f74:	e1929004 	orrs	r9, r2, r4
    4f78:	e1a0a000 	mov	sl, r0
    4f7c:	e58d3014 	str	r3, [sp, #20]
    4f80:	e58d200c 	str	r2, [sp, #12]
    4f84:	e58d9018 	str	r9, [sp, #24]
    4f88:	01a05009 	moveq	r5, r9
    4f8c:	058d901c 	streq	r9, [sp, #28]
    4f90:	0a00002d 	beq	504c <vs_read+0x17c>
    4f94:	ebfff17e 	bl	1594 <malloc>
    4f98:	e2502000 	subs	r2, r0, #0	; 0x0
    4f9c:	e58d201c 	str	r2, [sp, #28]
    4fa0:	1a00000c 	bne	4fd8 <vs_read+0x108>
    4fa4:	e59f4170 	ldr	r4, [pc, #368]	; 511c <vs_read+0x24c>
    4fa8:	e59f0170 	ldr	r0, [pc, #368]	; 5120 <vs_read+0x250>
    4fac:	e3001112 	movw	r1, #274	; 0x112
    4fb0:	e1a0e00f 	mov	lr, pc
    4fb4:	e594f000 	ldr	pc, [r4]
    4fb8:	e59f0164 	ldr	r0, [pc, #356]	; 5124 <vs_read+0x254>
    4fbc:	e59f1164 	ldr	r1, [pc, #356]	; 5128 <vs_read+0x258>
    4fc0:	e1a0e00f 	mov	lr, pc
    4fc4:	e594f000 	ldr	pc, [r4]
    4fc8:	e59f015c 	ldr	r0, [pc, #348]	; 512c <vs_read+0x25c>
    4fcc:	e1a0e00f 	mov	lr, pc
    4fd0:	e594f000 	ldr	pc, [r4]
    4fd4:	eafffffe 	b	4fd4 <vs_read+0x104>
    4fd8:	e3540000 	cmp	r4, #0	; 0x0
    4fdc:	01a05004 	moveq	r5, r4
    4fe0:	0a000019 	beq	504c <vs_read+0x17c>
    4fe4:	e59d804c 	ldr	r8, [sp, #76]
    4fe8:	e59f2124 	ldr	r2, [pc, #292]	; 5114 <vs_read+0x244>
    4fec:	e26a3000 	rsb	r3, sl, #0	; 0x0
    4ff0:	e5921000 	ldr	r1, [r2]
    4ff4:	e3a09000 	mov	r9, #0	; 0x0
    4ff8:	e58d8004 	str	r8, [sp, #4]
    4ffc:	e0068003 	and	r8, r6, r3
    5000:	e58da000 	str	sl, [sp]
    5004:	e1a02008 	mov	r2, r8
    5008:	e1a03009 	mov	r3, r9
    500c:	e1a0e00f 	mov	lr, pc
    5010:	e591f010 	ldr	pc, [r1, #16]
    5014:	e2505000 	subs	r5, r0, #0	; 0x0
    5018:	ba000038 	blt	5100 <vs_read+0x230>
    501c:	e59d901c 	ldr	r9, [sp, #28]
    5020:	e064100a 	rsb	r1, r4, sl
    5024:	e1a0000b 	mov	r0, fp
    5028:	e0891001 	add	r1, r9, r1
    502c:	e1a02004 	mov	r2, r4
    5030:	ebfff0f2 	bl	1400 <memcpy>
    5034:	e1550004 	cmp	r5, r4
    5038:	21a05004 	movcs	r5, r4
    503c:	e59d8014 	ldr	r8, [sp, #20]
    5040:	e0966004 	adds	r6, r6, r4
    5044:	e2a77000 	adc	r7, r7, #0	; 0x0
    5048:	e08bb004 	add	fp, fp, r4
    504c:	e59d200c 	ldr	r2, [sp, #12]
    5050:	e0584002 	subs	r4, r8, r2
    5054:	0a00000f 	beq	5098 <vs_read+0x1c8>
    5058:	e59f30b4 	ldr	r3, [pc, #180]	; 5114 <vs_read+0x244>
    505c:	e58d4000 	str	r4, [sp]
    5060:	e5931000 	ldr	r1, [r3]
    5064:	e59d304c 	ldr	r3, [sp, #76]
    5068:	e1a0000b 	mov	r0, fp
    506c:	e58d3004 	str	r3, [sp, #4]
    5070:	e1a02006 	mov	r2, r6
    5074:	e1a03007 	mov	r3, r7
    5078:	e1a0e00f 	mov	lr, pc
    507c:	e591f010 	ldr	pc, [r1, #16]
    5080:	e3500000 	cmp	r0, #0	; 0x0
    5084:	ba000012 	blt	50d4 <vs_read+0x204>
    5088:	e0966004 	adds	r6, r6, r4
    508c:	e0a77fc4 	adc	r7, r7, r4, asr #31
    5090:	e0855000 	add	r5, r5, r0
    5094:	e08bb004 	add	fp, fp, r4
    5098:	e59d800c 	ldr	r8, [sp, #12]
    509c:	e3580000 	cmp	r8, #0	; 0x0
    50a0:	0a000013 	beq	50f4 <vs_read+0x224>
    50a4:	e59d904c 	ldr	r9, [sp, #76]
    50a8:	e59f3064 	ldr	r3, [pc, #100]	; 5114 <vs_read+0x244>
    50ac:	e58da000 	str	sl, [sp]
    50b0:	e58d9004 	str	r9, [sp, #4]
    50b4:	e5931000 	ldr	r1, [r3]
    50b8:	e1a02006 	mov	r2, r6
    50bc:	e1a03007 	mov	r3, r7
    50c0:	e59d001c 	ldr	r0, [sp, #28]
    50c4:	e1a0e00f 	mov	lr, pc
    50c8:	e591f010 	ldr	pc, [r1, #16]
    50cc:	e3500000 	cmp	r0, #0	; 0x0
    50d0:	aa000001 	bge	50dc <vs_read+0x20c>
    50d4:	e1a05000 	mov	r5, r0
    50d8:	ea000005 	b	50f4 <vs_read+0x224>
    50dc:	e59d200c 	ldr	r2, [sp, #12]
    50e0:	e1a0000b 	mov	r0, fp
    50e4:	e59d101c 	ldr	r1, [sp, #28]
    50e8:	ebfff0c4 	bl	1400 <memcpy>
    50ec:	e59d200c 	ldr	r2, [sp, #12]
    50f0:	e0855002 	add	r5, r5, r2
    50f4:	e59d3018 	ldr	r3, [sp, #24]
    50f8:	e3530000 	cmp	r3, #0	; 0x0
    50fc:	0a000001 	beq	5108 <vs_read+0x238>
    5100:	e59d001c 	ldr	r0, [sp, #28]
    5104:	ebfff18a 	bl	1734 <free>
    5108:	e1a00005 	mov	r0, r5
    510c:	e28dd024 	add	sp, sp, #36	; 0x24
    5110:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5114:	0800d1f0 	.word	0x0800d1f0
    5118:	00012f17 	.word	0x00012f17
    511c:	0800d764 	.word	0x0800d764
    5120:	00012f1b 	.word	0x00012f1b
    5124:	00011da8 	.word	0x00011da8
    5128:	00012f2d 	.word	0x00012f2d
    512c:	00012f38 	.word	0x00012f38

00005130 <vs_reset>:
    5130:	e92d4010 	push	{r4, lr}
    5134:	e59f402c 	ldr	r4, [pc, #44]	; 5168 <vs_reset+0x38>
    5138:	e5940000 	ldr	r0, [r4]
    513c:	ebffef4c 	bl	e74 <cpu_recase>
    5140:	e5944000 	ldr	r4, [r4]
    5144:	e3540000 	cmp	r4, #0	; 0x0
    5148:	03e00001 	mvneq	r0, #1	; 0x1
    514c:	08bd8010 	popeq	{r4, pc}
    5150:	e5944018 	ldr	r4, [r4, #24]
    5154:	e3540000 	cmp	r4, #0	; 0x0
    5158:	03e00002 	mvneq	r0, #2	; 0x2
    515c:	08bd8010 	popeq	{r4, pc}
    5160:	e12fff34 	blx	r4
    5164:	e8bd8010 	pop	{r4, pc}
    5168:	0800d1f0 	.word	0x0800d1f0

0000516c <vs_assign_by_id>:
    516c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5170:	e59fc05c 	ldr	ip, [pc, #92]	; 51d4 <vs_assign_by_id+0x68>
    5174:	e3a03000 	mov	r3, #0	; 0x0
    5178:	e350000d 	cmp	r0, #13	; 0xd
    517c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5180:	e58c3000 	str	r3, [ip]
    5184:	8a00000b 	bhi	51b8 <vs_assign_by_id+0x4c>
    5188:	e59f2048 	ldr	r2, [pc, #72]	; 51d8 <vs_assign_by_id+0x6c>
    518c:	e283302c 	add	r3, r3, #44	; 0x2c
    5190:	e0232093 	mla	r3, r3, r0, r2
    5194:	e3510000 	cmp	r1, #0	; 0x0
    5198:	e58c3000 	str	r3, [ip]
    519c:	01a00001 	moveq	r0, r1
    51a0:	0a000002 	beq	51b0 <vs_assign_by_id+0x44>
    51a4:	e28dd004 	add	sp, sp, #4	; 0x4
    51a8:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    51ac:	eaffffdf 	b	5130 <vs_reset>
    51b0:	e28dd004 	add	sp, sp, #4	; 0x4
    51b4:	e8bd8000 	pop	{pc}
    51b8:	e1a01000 	mov	r1, r0
    51bc:	e59f3018 	ldr	r3, [pc, #24]	; 51dc <vs_assign_by_id+0x70>
    51c0:	e59f0018 	ldr	r0, [pc, #24]	; 51e0 <vs_assign_by_id+0x74>
    51c4:	e1a0e00f 	mov	lr, pc
    51c8:	e593f000 	ldr	pc, [r3]
    51cc:	e3e00000 	mvn	r0, #0	; 0x0
    51d0:	eafffff6 	b	51b0 <vs_assign_by_id+0x44>
    51d4:	0800d1f0 	.word	0x0800d1f0
    51d8:	00010fa0 	.word	0x00010fa0
    51dc:	0800d764 	.word	0x0800d764
    51e0:	00012f69 	.word	0x00012f69

000051e4 <vs_assign_by_name>:
    51e4:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    51e8:	e59f3084 	ldr	r3, [pc, #132]	; 5274 <vs_assign_by_name+0x90>
    51ec:	e3a02000 	mov	r2, #0	; 0x0
    51f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    51f4:	e5832000 	str	r2, [r3]
    51f8:	e1a06000 	mov	r6, r0
    51fc:	e1a07001 	mov	r7, r1
    5200:	e1a04002 	mov	r4, r2
    5204:	e3a0a02c 	mov	sl, #44	; 0x2c
    5208:	e59f8068 	ldr	r8, [pc, #104]	; 5278 <vs_assign_by_name+0x94>
    520c:	ea00000d 	b	5248 <vs_assign_by_name+0x64>
    5210:	e025849a 	mla	r5, sl, r4, r8
    5214:	e1a00005 	mov	r0, r5
    5218:	ebfff03f 	bl	131c <strncmp>
    521c:	e3500000 	cmp	r0, #0	; 0x0
    5220:	1a000007 	bne	5244 <vs_assign_by_name+0x60>
    5224:	e59f3048 	ldr	r3, [pc, #72]	; 5274 <vs_assign_by_name+0x90>
    5228:	e3570000 	cmp	r7, #0	; 0x0
    522c:	e5835000 	str	r5, [r3]
    5230:	01a00007 	moveq	r0, r7
    5234:	0a00000c 	beq	526c <vs_assign_by_name+0x88>
    5238:	e28dd004 	add	sp, sp, #4	; 0x4
    523c:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    5240:	eaffffba 	b	5130 <vs_reset>
    5244:	e2844001 	add	r4, r4, #1	; 0x1
    5248:	e354000e 	cmp	r4, #14	; 0xe
    524c:	e1a01006 	mov	r1, r6
    5250:	e3a02010 	mov	r2, #16	; 0x10
    5254:	1affffed 	bne	5210 <vs_assign_by_name+0x2c>
    5258:	e59f001c 	ldr	r0, [pc, #28]	; 527c <vs_assign_by_name+0x98>
    525c:	e59f301c 	ldr	r3, [pc, #28]	; 5280 <vs_assign_by_name+0x9c>
    5260:	e1a0e00f 	mov	lr, pc
    5264:	e593f000 	ldr	pc, [r3]
    5268:	e3e00000 	mvn	r0, #0	; 0x0
    526c:	e28dd004 	add	sp, sp, #4	; 0x4
    5270:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    5274:	0800d1f0 	.word	0x0800d1f0
    5278:	00010fa0 	.word	0x00010fa0
    527c:	00012f82 	.word	0x00012f82
    5280:	0800d764 	.word	0x0800d764

00005284 <ss_lock_isk>:
    5284:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5288:	e24dd004 	sub	sp, sp, #4	; 0x4
    528c:	eb0001d5 	bl	59e8 <security_enabled>
    5290:	e3500000 	cmp	r0, #0	; 0x0
    5294:	0a000002 	beq	52a4 <ss_lock_isk+0x20>
    5298:	e28dd004 	add	sp, sp, #4	; 0x4
    529c:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    52a0:	ea00002e 	b	5360 <block_cipher_lock_isk>
    52a4:	e28dd004 	add	sp, sp, #4	; 0x4
    52a8:	e8bd8000 	pop	{pc}

000052ac <ss_aes_data>:
    52ac:	e92d4070 	push	{r4, r5, r6, lr}
    52b0:	e1a05000 	mov	r5, r0
    52b4:	e1a06001 	mov	r6, r1
    52b8:	e1a04002 	mov	r4, r2
    52bc:	eb0001c9 	bl	59e8 <security_enabled>
    52c0:	e3500000 	cmp	r0, #0	; 0x0
    52c4:	0a000007 	beq	52e8 <ss_aes_data+0x3c>
    52c8:	e284200f 	add	r2, r4, #15	; 0xf
    52cc:	e1a00005 	mov	r0, r5
    52d0:	e1a01006 	mov	r1, r6
    52d4:	e3c2200f 	bic	r2, r2, #15	; 0xf
    52d8:	eb0000a0 	bl	5560 <block_cipher_operation>
    52dc:	e3500000 	cmp	r0, #0	; 0x0
    52e0:	13e00000 	mvnne	r0, #0	; 0x0
    52e4:	18bd8070 	popne	{r4, r5, r6, pc}
    52e8:	e3a00000 	mov	r0, #0	; 0x0
    52ec:	e8bd8070 	pop	{r4, r5, r6, pc}

000052f0 <ss_aes_init>:
    52f0:	e92d4070 	push	{r4, r5, r6, lr}
    52f4:	e1a05000 	mov	r5, r0
    52f8:	e1a04001 	mov	r4, r1
    52fc:	e1a06002 	mov	r6, r2
    5300:	eb0001b8 	bl	59e8 <security_enabled>
    5304:	e3500000 	cmp	r0, #0	; 0x0
    5308:	0a00000b 	beq	533c <ss_aes_init+0x4c>
    530c:	e3540000 	cmp	r4, #0	; 0x0
    5310:	01a00005 	moveq	r0, r5
    5314:	01a02004 	moveq	r2, r4
    5318:	01a03006 	moveq	r3, r6
    531c:	03a01001 	moveq	r1, #1	; 0x1
    5320:	11a00005 	movne	r0, r5
    5324:	11a02004 	movne	r2, r4
    5328:	11a03006 	movne	r3, r6
    532c:	13a01000 	movne	r1, #0	; 0x0
    5330:	eb00002c 	bl	53e8 <block_cipher_aes_init>
    5334:	e3500000 	cmp	r0, #0	; 0x0
    5338:	1a000001 	bne	5344 <ss_aes_init+0x54>
    533c:	e3a00000 	mov	r0, #0	; 0x0
    5340:	e8bd8070 	pop	{r4, r5, r6, pc}
    5344:	e3e00000 	mvn	r0, #0	; 0x0
    5348:	e8bd8070 	pop	{r4, r5, r6, pc}

0000534c <check_register_bit>:
    534c:	e3a03001 	mov	r3, #1	; 0x1
    5350:	e0103113 	ands	r3, r0, r3, lsl r1
    5354:	03a00000 	moveq	r0, #0	; 0x0
    5358:	13a00001 	movne	r0, #1	; 0x1
    535c:	e12fff1e 	bx	lr

00005360 <block_cipher_lock_isk>:
    5360:	e59f200c 	ldr	r2, [pc, #12]	; 5374 <block_cipher_lock_isk+0x14>
    5364:	e5923400 	ldr	r3, [r2, #1024]
    5368:	e3833706 	orr	r3, r3, #1572864	; 0x180000
    536c:	e5823400 	str	r3, [r2, #1024]
    5370:	e12fff1e 	bx	lr
    5374:	27700000 	.word	0x27700000

00005378 <block_cipher_reset>:
    5378:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    537c:	e59f4060 	ldr	r4, [pc, #96]	; 53e4 <block_cipher_reset+0x6c>
    5380:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    5384:	e5843400 	str	r3, [r4, #1024]
    5388:	e24dd004 	sub	sp, sp, #4	; 0x4
    538c:	eb000d4d 	bl	88c8 <get_ticks>
    5390:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    5394:	e3a07000 	mov	r7, #0	; 0x0
    5398:	e0966000 	adds	r6, r6, r0
    539c:	e1a05004 	mov	r5, r4
    53a0:	e0a77001 	adc	r7, r7, r1
    53a4:	e5944400 	ldr	r4, [r4, #1024]
    53a8:	ea000008 	b	53d0 <block_cipher_reset+0x58>
    53ac:	e5954400 	ldr	r4, [r5, #1024]
    53b0:	eb000d44 	bl	88c8 <get_ticks>
    53b4:	e1510007 	cmp	r1, r7
    53b8:	8a000002 	bhi	53c8 <block_cipher_reset+0x50>
    53bc:	1a000003 	bne	53d0 <block_cipher_reset+0x58>
    53c0:	e1500006 	cmp	r0, r6
    53c4:	9a000001 	bls	53d0 <block_cipher_reset+0x58>
    53c8:	e3a00001 	mov	r0, #1	; 0x1
    53cc:	ea000002 	b	53dc <block_cipher_reset+0x64>
    53d0:	e3540000 	cmp	r4, #0	; 0x0
    53d4:	bafffff4 	blt	53ac <block_cipher_reset+0x34>
    53d8:	e3a00000 	mov	r0, #0	; 0x0
    53dc:	e28dd004 	add	sp, sp, #4	; 0x4
    53e0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    53e4:	27700000 	.word	0x27700000

000053e8 <block_cipher_aes_init>:
    53e8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    53ec:	e24dd004 	sub	sp, sp, #4	; 0x4
    53f0:	e1a05000 	mov	r5, r0
    53f4:	e1a04001 	mov	r4, r1
    53f8:	e1a06002 	mov	r6, r2
    53fc:	e1a07003 	mov	r7, r3
    5400:	ebffffdc 	bl	5378 <block_cipher_reset>
    5404:	e3500000 	cmp	r0, #0	; 0x0
    5408:	1a00003d 	bne	5504 <block_cipher_aes_init+0x11c>
    540c:	e59f3134 	ldr	r3, [pc, #308]	; 5548 <block_cipher_aes_init+0x160>
    5410:	e3a02001 	mov	r2, #1	; 0x1
    5414:	e3550001 	cmp	r5, #1	; 0x1
    5418:	e5832404 	str	r2, [r3, #1028]
    541c:	8a000038 	bhi	5504 <block_cipher_aes_init+0x11c>
    5420:	e3540003 	cmp	r4, #3	; 0x3
    5424:	13540000 	cmpne	r4, #0	; 0x0
    5428:	0a000001 	beq	5434 <block_cipher_aes_init+0x4c>
    542c:	e1540002 	cmp	r4, r2
    5430:	1a000033 	bne	5504 <block_cipher_aes_init+0x11c>
    5434:	e1a03305 	lsl	r3, r5, #6
    5438:	e3833901 	orr	r3, r3, #16384	; 0x4000
    543c:	e3833004 	orr	r3, r3, #4	; 0x4
    5440:	e59f2100 	ldr	r2, [pc, #256]	; 5548 <block_cipher_aes_init+0x160>
    5444:	e1833984 	orr	r3, r3, r4, lsl #19
    5448:	e3540000 	cmp	r4, #0	; 0x0
    544c:	e5823400 	str	r3, [r2, #1024]
    5450:	1a000008 	bne	5478 <block_cipher_aes_init+0x90>
    5454:	e59f10f0 	ldr	r1, [pc, #240]	; 554c <block_cipher_aes_init+0x164>
    5458:	e59f00f0 	ldr	r0, [pc, #240]	; 5550 <block_cipher_aes_init+0x168>
    545c:	e59f30f0 	ldr	r3, [pc, #240]	; 5554 <block_cipher_aes_init+0x16c>
    5460:	e0862001 	add	r2, r6, r1
    5464:	e0823003 	add	r3, r2, r3
    5468:	e5933000 	ldr	r3, [r3]
    546c:	e4813004 	str	r3, [r1], #4
    5470:	e1510000 	cmp	r1, r0
    5474:	1afffff8 	bne	545c <block_cipher_aes_init+0x74>
    5478:	e59f10d0 	ldr	r1, [pc, #208]	; 5550 <block_cipher_aes_init+0x168>
    547c:	e59f00d4 	ldr	r0, [pc, #212]	; 5558 <block_cipher_aes_init+0x170>
    5480:	e59f30d4 	ldr	r3, [pc, #212]	; 555c <block_cipher_aes_init+0x174>
    5484:	e0872001 	add	r2, r7, r1
    5488:	e0823003 	add	r3, r2, r3
    548c:	e5933000 	ldr	r3, [r3]
    5490:	e4813004 	str	r3, [r1], #4
    5494:	e1510000 	cmp	r1, r0
    5498:	1afffff8 	bne	5480 <block_cipher_aes_init+0x98>
    549c:	e3540000 	cmp	r4, #0	; 0x0
    54a0:	1a000008 	bne	54c8 <block_cipher_aes_init+0xe0>
    54a4:	eb000d07 	bl	88c8 <get_ticks>
    54a8:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    54ac:	e3a05000 	mov	r5, #0	; 0x0
    54b0:	e0944000 	adds	r4, r4, r0
    54b4:	e59f608c 	ldr	r6, [pc, #140]	; 5548 <block_cipher_aes_init+0x160>
    54b8:	e0a55001 	adc	r5, r5, r1
    54bc:	e596340c 	ldr	r3, [r6, #1036]
    54c0:	e3130008 	tst	r3, #8	; 0x8
    54c4:	0a000011 	beq	5510 <block_cipher_aes_init+0x128>
    54c8:	eb000cfe 	bl	88c8 <get_ticks>
    54cc:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    54d0:	e3a05000 	mov	r5, #0	; 0x0
    54d4:	e0944000 	adds	r4, r4, r0
    54d8:	e59f6068 	ldr	r6, [pc, #104]	; 5548 <block_cipher_aes_init+0x160>
    54dc:	e0a55001 	adc	r5, r5, r1
    54e0:	e596340c 	ldr	r3, [r6, #1036]
    54e4:	e59f205c 	ldr	r2, [pc, #92]	; 5548 <block_cipher_aes_init+0x160>
    54e8:	e3130010 	tst	r3, #16	; 0x10
    54ec:	0a00000e 	beq	552c <block_cipher_aes_init+0x144>
    54f0:	e5923400 	ldr	r3, [r2, #1024]
    54f4:	e3a00000 	mov	r0, #0	; 0x0
    54f8:	e3833001 	orr	r3, r3, #1	; 0x1
    54fc:	e5823400 	str	r3, [r2, #1024]
    5500:	ea000000 	b	5508 <block_cipher_aes_init+0x120>
    5504:	e3a00001 	mov	r0, #1	; 0x1
    5508:	e28dd004 	add	sp, sp, #4	; 0x4
    550c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5510:	eb000cec 	bl	88c8 <get_ticks>
    5514:	e1510005 	cmp	r1, r5
    5518:	8afffff9 	bhi	5504 <block_cipher_aes_init+0x11c>
    551c:	1affffe6 	bne	54bc <block_cipher_aes_init+0xd4>
    5520:	e1500004 	cmp	r0, r4
    5524:	8afffff6 	bhi	5504 <block_cipher_aes_init+0x11c>
    5528:	eaffffe3 	b	54bc <block_cipher_aes_init+0xd4>
    552c:	eb000ce5 	bl	88c8 <get_ticks>
    5530:	e1510005 	cmp	r1, r5
    5534:	8afffff2 	bhi	5504 <block_cipher_aes_init+0x11c>
    5538:	1affffe8 	bne	54e0 <block_cipher_aes_init+0xf8>
    553c:	e1500004 	cmp	r0, r4
    5540:	8affffef 	bhi	5504 <block_cipher_aes_init+0x11c>
    5544:	eaffffe5 	b	54e0 <block_cipher_aes_init+0xf8>
    5548:	27700000 	.word	0x27700000
    554c:	27700420 	.word	0x27700420
    5550:	27700440 	.word	0x27700440
    5554:	d88ffbe0 	.word	0xd88ffbe0
    5558:	27700450 	.word	0x27700450
    555c:	d88ffbc0 	.word	0xd88ffbc0

00005560 <block_cipher_operation>:
    5560:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    5564:	e24dd00c 	sub	sp, sp, #12	; 0xc
    5568:	e1a08002 	mov	r8, r2
    556c:	e58d0004 	str	r0, [sp, #4]
    5570:	e1a09001 	mov	r9, r1
    5574:	e1a06002 	mov	r6, r2
    5578:	e59fa0c4 	ldr	sl, [pc, #196]	; 5644 <block_cipher_operation+0xe4>
    557c:	e3a0b000 	mov	fp, #0	; 0x0
    5580:	ea000028 	b	5628 <block_cipher_operation+0xc8>
    5584:	e59d3004 	ldr	r3, [sp, #4]
    5588:	e3560902 	cmp	r6, #32768	; 0x8000
    558c:	b1a07006 	movlt	r7, r6
    5590:	a3a07902 	movge	r7, #32768	; 0x8000
    5594:	e0664008 	rsb	r4, r6, r8
    5598:	e3a00000 	mov	r0, #0	; 0x0
    559c:	e0831004 	add	r1, r3, r4
    55a0:	e1a02007 	mov	r2, r7
    55a4:	eb0000ed 	bl	5960 <dma_sdma_init>
    55a8:	e3500000 	cmp	r0, #0	; 0x0
    55ac:	1a000021 	bne	5638 <block_cipher_operation+0xd8>
    55b0:	e0891004 	add	r1, r9, r4
    55b4:	e2800001 	add	r0, r0, #1	; 0x1
    55b8:	e1a02007 	mov	r2, r7
    55bc:	eb0000e7 	bl	5960 <dma_sdma_init>
    55c0:	e3500000 	cmp	r0, #0	; 0x0
    55c4:	1a00001b 	bne	5638 <block_cipher_operation+0xd8>
    55c8:	eb0000d4 	bl	5920 <dma_start>
    55cc:	e3a00001 	mov	r0, #1	; 0x1
    55d0:	eb0000d2 	bl	5920 <dma_start>
    55d4:	eb000cbb 	bl	88c8 <get_ticks>
    55d8:	e090400a 	adds	r4, r0, sl
    55dc:	e0a1500b 	adc	r5, r1, fp
    55e0:	ea000005 	b	55fc <block_cipher_operation+0x9c>
    55e4:	eb000cb7 	bl	88c8 <get_ticks>
    55e8:	e1510005 	cmp	r1, r5
    55ec:	8a000011 	bhi	5638 <block_cipher_operation+0xd8>
    55f0:	1a000001 	bne	55fc <block_cipher_operation+0x9c>
    55f4:	e1500004 	cmp	r0, r4
    55f8:	8a00000e 	bhi	5638 <block_cipher_operation+0xd8>
    55fc:	e3a01003 	mov	r1, #3	; 0x3
    5600:	e3a00001 	mov	r0, #1	; 0x1
    5604:	eb0000cb 	bl	5938 <dma_get_status>
    5608:	e3500000 	cmp	r0, #0	; 0x0
    560c:	0afffff4 	beq	55e4 <block_cipher_operation+0x84>
    5610:	e3a00000 	mov	r0, #0	; 0x0
    5614:	e3a01003 	mov	r1, #3	; 0x3
    5618:	eb0000c6 	bl	5938 <dma_get_status>
    561c:	e3500000 	cmp	r0, #0	; 0x0
    5620:	0a000004 	beq	5638 <block_cipher_operation+0xd8>
    5624:	e0676006 	rsb	r6, r7, r6
    5628:	e3560000 	cmp	r6, #0	; 0x0
    562c:	1affffd4 	bne	5584 <block_cipher_operation+0x24>
    5630:	e1a00006 	mov	r0, r6
    5634:	ea000000 	b	563c <block_cipher_operation+0xdc>
    5638:	e3a00001 	mov	r0, #1	; 0x1
    563c:	e28dd00c 	add	sp, sp, #12	; 0xc
    5640:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5644:	000f4240 	.word	0x000f4240

00005648 <hash_len>:
    5648:	e59f300c 	ldr	r3, [pc, #12]	; 565c <hash_len+0x14>
    564c:	e5932000 	ldr	r2, [r3]
    5650:	e59f3008 	ldr	r3, [pc, #8]	; 5660 <hash_len+0x18>
    5654:	e7930202 	ldr	r0, [r3, r2, lsl #4]
    5658:	e12fff1e 	bx	lr
    565c:	0800d1f4 	.word	0x0800d1f4
    5660:	00011208 	.word	0x00011208

00005664 <hash_value>:
    5664:	e92d4070 	push	{r4, r5, r6, lr}
    5668:	e59f6054 	ldr	r6, [pc, #84]	; 56c4 <hash_value+0x60>
    566c:	e1a05000 	mov	r5, r0
    5670:	e5963000 	ldr	r3, [r6]
    5674:	e3530000 	cmp	r3, #0	; 0x0
    5678:	0a000009 	beq	56a4 <hash_value+0x40>
    567c:	eb0000d9 	bl	59e8 <security_enabled>
    5680:	e2504000 	subs	r4, r0, #0	; 0x0
    5684:	1a000006 	bne	56a4 <hash_value+0x40>
    5688:	e59f0038 	ldr	r0, [pc, #56]	; 56c8 <hash_value+0x64>
    568c:	e5961000 	ldr	r1, [r6]
    5690:	e59f3034 	ldr	r3, [pc, #52]	; 56cc <hash_value+0x68>
    5694:	e1a0e00f 	mov	lr, pc
    5698:	e593f000 	ldr	pc, [r3]
    569c:	e1a00004 	mov	r0, r4
    56a0:	e8bd8070 	pop	{r4, r5, r6, pc}
    56a4:	e59f3018 	ldr	r3, [pc, #24]	; 56c4 <hash_value+0x60>
    56a8:	e1a00005 	mov	r0, r5
    56ac:	e5932000 	ldr	r2, [r3]
    56b0:	e59f3018 	ldr	r3, [pc, #24]	; 56d0 <hash_value+0x6c>
    56b4:	e0833202 	add	r3, r3, r2, lsl #4
    56b8:	e1a0e00f 	mov	lr, pc
    56bc:	e593f00c 	ldr	pc, [r3, #12]
    56c0:	e8bd8070 	pop	{r4, r5, r6, pc}
    56c4:	0800d1f4 	.word	0x0800d1f4
    56c8:	00012f9d 	.word	0x00012f9d
    56cc:	0800d764 	.word	0x0800d764
    56d0:	00011208 	.word	0x00011208

000056d4 <hash_data>:
    56d4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    56d8:	e59f5074 	ldr	r5, [pc, #116]	; 5754 <hash_data+0x80>
    56dc:	e24dd004 	sub	sp, sp, #4	; 0x4
    56e0:	e5953000 	ldr	r3, [r5]
    56e4:	e1a06000 	mov	r6, r0
    56e8:	e3530000 	cmp	r3, #0	; 0x0
    56ec:	e1a07001 	mov	r7, r1
    56f0:	0a000009 	beq	571c <hash_data+0x48>
    56f4:	eb0000bb 	bl	59e8 <security_enabled>
    56f8:	e2504000 	subs	r4, r0, #0	; 0x0
    56fc:	1a000006 	bne	571c <hash_data+0x48>
    5700:	e59f0050 	ldr	r0, [pc, #80]	; 5758 <hash_data+0x84>
    5704:	e5951000 	ldr	r1, [r5]
    5708:	e59f304c 	ldr	r3, [pc, #76]	; 575c <hash_data+0x88>
    570c:	e1a0e00f 	mov	lr, pc
    5710:	e593f000 	ldr	pc, [r3]
    5714:	e1a00004 	mov	r0, r4
    5718:	ea00000b 	b	574c <hash_data+0x78>
    571c:	e59f003c 	ldr	r0, [pc, #60]	; 5760 <hash_data+0x8c>
    5720:	e59f3034 	ldr	r3, [pc, #52]	; 575c <hash_data+0x88>
    5724:	e1a0e00f 	mov	lr, pc
    5728:	e593f000 	ldr	pc, [r3]
    572c:	e59f3020 	ldr	r3, [pc, #32]	; 5754 <hash_data+0x80>
    5730:	e1a00006 	mov	r0, r6
    5734:	e5932000 	ldr	r2, [r3]
    5738:	e59f3024 	ldr	r3, [pc, #36]	; 5764 <hash_data+0x90>
    573c:	e1a01007 	mov	r1, r7
    5740:	e0833202 	add	r3, r3, r2, lsl #4
    5744:	e1a0e00f 	mov	lr, pc
    5748:	e593f008 	ldr	pc, [r3, #8]
    574c:	e28dd004 	add	sp, sp, #4	; 0x4
    5750:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5754:	0800d1f4 	.word	0x0800d1f4
    5758:	00012f9d 	.word	0x00012f9d
    575c:	0800d764 	.word	0x0800d764
    5760:	00012fbe 	.word	0x00012fbe
    5764:	00011208 	.word	0x00011208

00005768 <hash_init>:
    5768:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    576c:	e3500003 	cmp	r0, #3	; 0x3
    5770:	e24dd004 	sub	sp, sp, #4	; 0x4
    5774:	e1a04000 	mov	r4, r0
    5778:	e1a07001 	mov	r7, r1
    577c:	9a000005 	bls	5798 <hash_init+0x30>
    5780:	e59f0064 	ldr	r0, [pc, #100]	; 57ec <hash_init+0x84>
    5784:	e59f3064 	ldr	r3, [pc, #100]	; 57f0 <hash_init+0x88>
    5788:	e1a0e00f 	mov	lr, pc
    578c:	e593f000 	ldr	pc, [r3]
    5790:	e3e00000 	mvn	r0, #0	; 0x0
    5794:	ea000012 	b	57e4 <hash_init+0x7c>
    5798:	e59f6054 	ldr	r6, [pc, #84]	; 57f4 <hash_init+0x8c>
    579c:	e3500000 	cmp	r0, #0	; 0x0
    57a0:	e5860000 	str	r0, [r6]
    57a4:	0a000009 	beq	57d0 <hash_init+0x68>
    57a8:	eb00008e 	bl	59e8 <security_enabled>
    57ac:	e2505000 	subs	r5, r0, #0	; 0x0
    57b0:	1a000006 	bne	57d0 <hash_init+0x68>
    57b4:	e59f003c 	ldr	r0, [pc, #60]	; 57f8 <hash_init+0x90>
    57b8:	e5961000 	ldr	r1, [r6]
    57bc:	e59f302c 	ldr	r3, [pc, #44]	; 57f0 <hash_init+0x88>
    57c0:	e1a0e00f 	mov	lr, pc
    57c4:	e593f000 	ldr	pc, [r3]
    57c8:	e1a00005 	mov	r0, r5
    57cc:	ea000004 	b	57e4 <hash_init+0x7c>
    57d0:	e59f3024 	ldr	r3, [pc, #36]	; 57fc <hash_init+0x94>
    57d4:	e1a00007 	mov	r0, r7
    57d8:	e0833204 	add	r3, r3, r4, lsl #4
    57dc:	e1a0e00f 	mov	lr, pc
    57e0:	e593f004 	ldr	pc, [r3, #4]
    57e4:	e28dd004 	add	sp, sp, #4	; 0x4
    57e8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    57ec:	00012fc9 	.word	0x00012fc9
    57f0:	0800d764 	.word	0x0800d764
    57f4:	0800d1f4 	.word	0x0800d1f4
    57f8:	00012f9d 	.word	0x00012f9d
    57fc:	00011208 	.word	0x00011208

00005800 <md5_value>:
    5800:	e92d4030 	push	{r4, r5, lr}
    5804:	e59f3054 	ldr	r3, [pc, #84]	; 5860 <md5_value+0x60>
    5808:	e1a05000 	mov	r5, r0
    580c:	e893000c 	ldm	r3, {r2, r3}
    5810:	e1823003 	orr	r3, r2, r3
    5814:	e24dd024 	sub	sp, sp, #36	; 0x24
    5818:	e3530000 	cmp	r3, #0	; 0x0
    581c:	13e00000 	mvnne	r0, #0	; 0x0
    5820:	1a00000c 	bne	5858 <md5_value+0x58>
    5824:	e1a0000d 	mov	r0, sp
    5828:	eb000178 	bl	5e10 <hash_module_get_result>
    582c:	e3500000 	cmp	r0, #0	; 0x0
    5830:	e1a0400d 	mov	r4, sp
    5834:	13e00001 	mvnne	r0, #1	; 0x1
    5838:	1a000006 	bne	5858 <md5_value+0x58>
    583c:	e1a0200d 	mov	r2, sp
    5840:	e7d23000 	ldrb	r3, [r2, r0]
    5844:	e7c53000 	strb	r3, [r5, r0]
    5848:	e2800001 	add	r0, r0, #1	; 0x1
    584c:	e3500010 	cmp	r0, #16	; 0x10
    5850:	1afffffa 	bne	5840 <md5_value+0x40>
    5854:	e3a00000 	mov	r0, #0	; 0x0
    5858:	e28dd024 	add	sp, sp, #36	; 0x24
    585c:	e8bd8030 	pop	{r4, r5, pc}
    5860:	0800d1f8 	.word	0x0800d1f8

00005864 <md5_data>:
    5864:	e59f305c 	ldr	r3, [pc, #92]	; 58c8 <md5_data+0x64>
    5868:	e92d4070 	push	{r4, r5, r6, lr}
    586c:	e1a06001 	mov	r6, r1
    5870:	e5931004 	ldr	r1, [r3, #4]
    5874:	e1a04006 	mov	r4, r6
    5878:	e3a05000 	mov	r5, #0	; 0x0
    587c:	e1550001 	cmp	r5, r1
    5880:	e593c000 	ldr	ip, [r3]
    5884:	8a000002 	bhi	5894 <md5_data+0x30>
    5888:	1a000003 	bne	589c <md5_data+0x38>
    588c:	e154000c 	cmp	r4, ip
    5890:	9a000001 	bls	589c <md5_data+0x38>
    5894:	e3e00000 	mvn	r0, #0	; 0x0
    5898:	e8bd8070 	pop	{r4, r5, r6, pc}
    589c:	e1a0200c 	mov	r2, ip
    58a0:	e1a03001 	mov	r3, r1
    58a4:	e59fc01c 	ldr	ip, [pc, #28]	; 58c8 <md5_data+0x64>
    58a8:	e0522004 	subs	r2, r2, r4
    58ac:	e0c33005 	sbc	r3, r3, r5
    58b0:	e1a01006 	mov	r1, r6
    58b4:	e88c000c 	stm	ip, {r2, r3}
    58b8:	eb0001b7 	bl	5f9c <hash_dma_transfer>
    58bc:	e3500000 	cmp	r0, #0	; 0x0
    58c0:	13e00001 	mvnne	r0, #1	; 0x1
    58c4:	e8bd8070 	pop	{r4, r5, r6, pc}
    58c8:	0800d1f8 	.word	0x0800d1f8

000058cc <md5_init>:
    58cc:	e1a03ea0 	lsr	r3, r0, #29
    58d0:	e1a0c180 	lsl	ip, r0, #3
    58d4:	e92d4010 	push	{r4, lr}
    58d8:	e59f403c 	ldr	r4, [pc, #60]	; 591c <md5_init+0x50>
    58dc:	e1a0200c 	mov	r2, ip
    58e0:	e1a01003 	mov	r1, r3
    58e4:	e3a00000 	mov	r0, #0	; 0x0
    58e8:	e5843004 	str	r3, [r4, #4]
    58ec:	e584c000 	str	ip, [r4]
    58f0:	eb00018d 	bl	5f2c <hash_module_init>
    58f4:	e3500000 	cmp	r0, #0	; 0x0
    58f8:	05942000 	ldreq	r2, [r4]
    58fc:	05943004 	ldreq	r3, [r4, #4]
    5900:	01a021a2 	lsreq	r2, r2, #3
    5904:	01822e83 	orreq	r2, r2, r3, lsl #29
    5908:	01a031a3 	lsreq	r3, r3, #3
    590c:	13e00000 	mvnne	r0, #0	; 0x0
    5910:	05843004 	streq	r3, [r4, #4]
    5914:	05842000 	streq	r2, [r4]
    5918:	e8bd8010 	pop	{r4, pc}
    591c:	0800d1f8 	.word	0x0800d1f8

00005920 <dma_start>:
    5920:	e280099d 	add	r0, r0, #2572288	; 0x274000
    5924:	e2800a03 	add	r0, r0, #12288	; 0x3000
    5928:	e1a00400 	lsl	r0, r0, #8
    592c:	e3a03001 	mov	r3, #1	; 0x1
    5930:	e5803000 	str	r3, [r0]
    5934:	e12fff1e 	bx	lr

00005938 <dma_get_status>:
    5938:	e3a0c001 	mov	ip, #1	; 0x1
    593c:	e1a0211c 	lsl	r2, ip, r1
    5940:	e280099d 	add	r0, r0, #2572288	; 0x274000
    5944:	e2800a03 	add	r0, r0, #12288	; 0x3000
    5948:	e1a01400 	lsl	r1, r0, #8
    594c:	e5913024 	ldr	r3, [r1, #36]
    5950:	e0130002 	ands	r0, r3, r2
    5954:	15812024 	strne	r2, [r1, #36]
    5958:	11a0000c 	movne	r0, ip
    595c:	e12fff1e 	bx	lr

00005960 <dma_sdma_init>:
    5960:	e280c99d 	add	ip, r0, #2572288	; 0x274000
    5964:	e28cca03 	add	ip, ip, #12288	; 0x3000
    5968:	e1a0040c 	lsl	r0, ip, #8
    596c:	e2800004 	add	r0, r0, #4	; 0x4
    5970:	e5903000 	ldr	r3, [r0]
    5974:	e3520801 	cmp	r2, #65536	; 0x10000
    5978:	e3833001 	orr	r3, r3, #1	; 0x1
    597c:	e5803000 	str	r3, [r0]
    5980:	e92d4010 	push	{r4, lr}
    5984:	e3a03341 	mov	r3, #67108865	; 0x4000001
    5988:	e1a0440c 	lsl	r4, ip, #8
    598c:	e584300c 	str	r3, [r4, #12]
    5990:	31a02802 	lslcc	r2, r2, #16
    5994:	31a0340c 	lslcc	r3, ip, #8
    5998:	31a02822 	lsrcc	r2, r2, #16
    599c:	32833010 	addcc	r3, r3, #16	; 0x10
    59a0:	35832000 	strcc	r2, [r3]
    59a4:	23a00001 	movcs	r0, #1	; 0x1
    59a8:	35841014 	strcc	r1, [r4, #20]
    59ac:	33a00000 	movcc	r0, #0	; 0x0
    59b0:	e8bd8010 	pop	{r4, pc}

000059b4 <security_cke_control>:
    59b4:	e59fc014 	ldr	ip, [pc, #20]	; 59d0 <security_cke_control+0x1c>
    59b8:	e3a02001 	mov	r2, #1	; 0x1
    59bc:	e59c3000 	ldr	r3, [ip]
    59c0:	e1c33012 	bic	r3, r3, r2, lsl r0
    59c4:	e1833011 	orr	r3, r3, r1, lsl r0
    59c8:	e58c3000 	str	r3, [ip]
    59cc:	e12fff1e 	bx	lr
    59d0:	27702000 	.word	0x27702000

000059d4 <security_enable>:
    59d4:	e59f3008 	ldr	r3, [pc, #8]	; 59e4 <security_enable+0x10>
    59d8:	e3a02001 	mov	r2, #1	; 0x1
    59dc:	e5832000 	str	r2, [r3]
    59e0:	e12fff1e 	bx	lr
    59e4:	0800d200 	.word	0x0800d200

000059e8 <security_enabled>:
    59e8:	e59f300c 	ldr	r3, [pc, #12]	; 59fc <security_enabled+0x14>
    59ec:	e5930000 	ldr	r0, [r3]
    59f0:	e2500000 	subs	r0, r0, #0	; 0x0
    59f4:	13a00001 	movne	r0, #1	; 0x1
    59f8:	e12fff1e 	bx	lr
    59fc:	0800d200 	.word	0x0800d200

00005a00 <dma_reset>:
    5a00:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5a04:	e280099d 	add	r0, r0, #2572288	; 0x274000
    5a08:	e2800a03 	add	r0, r0, #12288	; 0x3000
    5a0c:	e1a07400 	lsl	r7, r0, #8
    5a10:	e3a03002 	mov	r3, #2	; 0x2
    5a14:	e24dd004 	sub	sp, sp, #4	; 0x4
    5a18:	e5873000 	str	r3, [r7]
    5a1c:	eb000ba9 	bl	88c8 <get_ticks>
    5a20:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    5a24:	e3a05000 	mov	r5, #0	; 0x0
    5a28:	e0944000 	adds	r4, r4, r0
    5a2c:	e0a55001 	adc	r5, r5, r1
    5a30:	e5976000 	ldr	r6, [r7]
    5a34:	ea000008 	b	5a5c <dma_reset+0x5c>
    5a38:	e5976000 	ldr	r6, [r7]
    5a3c:	eb000ba1 	bl	88c8 <get_ticks>
    5a40:	e1510005 	cmp	r1, r5
    5a44:	8a000002 	bhi	5a54 <dma_reset+0x54>
    5a48:	1a000003 	bne	5a5c <dma_reset+0x5c>
    5a4c:	e1500004 	cmp	r0, r4
    5a50:	9a000001 	bls	5a5c <dma_reset+0x5c>
    5a54:	e3a00001 	mov	r0, #1	; 0x1
    5a58:	ea000002 	b	5a68 <dma_reset+0x68>
    5a5c:	e3560000 	cmp	r6, #0	; 0x0
    5a60:	1afffff4 	bne	5a38 <dma_reset+0x38>
    5a64:	e1a00006 	mov	r0, r6
    5a68:	e28dd004 	add	sp, sp, #4	; 0x4
    5a6c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00005a70 <sha1_value>:
    5a70:	e92d4030 	push	{r4, r5, lr}
    5a74:	e59f3054 	ldr	r3, [pc, #84]	; 5ad0 <sha1_value+0x60>
    5a78:	e1a05000 	mov	r5, r0
    5a7c:	e893000c 	ldm	r3, {r2, r3}
    5a80:	e1823003 	orr	r3, r2, r3
    5a84:	e24dd024 	sub	sp, sp, #36	; 0x24
    5a88:	e3530000 	cmp	r3, #0	; 0x0
    5a8c:	13e00000 	mvnne	r0, #0	; 0x0
    5a90:	1a00000c 	bne	5ac8 <sha1_value+0x58>
    5a94:	e1a0000d 	mov	r0, sp
    5a98:	eb0000dc 	bl	5e10 <hash_module_get_result>
    5a9c:	e3500000 	cmp	r0, #0	; 0x0
    5aa0:	e1a0400d 	mov	r4, sp
    5aa4:	13e00001 	mvnne	r0, #1	; 0x1
    5aa8:	1a000006 	bne	5ac8 <sha1_value+0x58>
    5aac:	e1a0200d 	mov	r2, sp
    5ab0:	e7d23000 	ldrb	r3, [r2, r0]
    5ab4:	e7c53000 	strb	r3, [r5, r0]
    5ab8:	e2800001 	add	r0, r0, #1	; 0x1
    5abc:	e3500014 	cmp	r0, #20	; 0x14
    5ac0:	1afffffa 	bne	5ab0 <sha1_value+0x40>
    5ac4:	e3a00000 	mov	r0, #0	; 0x0
    5ac8:	e28dd024 	add	sp, sp, #36	; 0x24
    5acc:	e8bd8030 	pop	{r4, r5, pc}
    5ad0:	0800d208 	.word	0x0800d208

00005ad4 <sha1_data>:
    5ad4:	e59f305c 	ldr	r3, [pc, #92]	; 5b38 <sha1_data+0x64>
    5ad8:	e92d4070 	push	{r4, r5, r6, lr}
    5adc:	e1a06001 	mov	r6, r1
    5ae0:	e5931004 	ldr	r1, [r3, #4]
    5ae4:	e1a04006 	mov	r4, r6
    5ae8:	e3a05000 	mov	r5, #0	; 0x0
    5aec:	e1550001 	cmp	r5, r1
    5af0:	e593c000 	ldr	ip, [r3]
    5af4:	8a000002 	bhi	5b04 <sha1_data+0x30>
    5af8:	1a000003 	bne	5b0c <sha1_data+0x38>
    5afc:	e154000c 	cmp	r4, ip
    5b00:	9a000001 	bls	5b0c <sha1_data+0x38>
    5b04:	e3e00000 	mvn	r0, #0	; 0x0
    5b08:	e8bd8070 	pop	{r4, r5, r6, pc}
    5b0c:	e1a0200c 	mov	r2, ip
    5b10:	e1a03001 	mov	r3, r1
    5b14:	e59fc01c 	ldr	ip, [pc, #28]	; 5b38 <sha1_data+0x64>
    5b18:	e0522004 	subs	r2, r2, r4
    5b1c:	e0c33005 	sbc	r3, r3, r5
    5b20:	e1a01006 	mov	r1, r6
    5b24:	e88c000c 	stm	ip, {r2, r3}
    5b28:	eb00011b 	bl	5f9c <hash_dma_transfer>
    5b2c:	e3500000 	cmp	r0, #0	; 0x0
    5b30:	13e00001 	mvnne	r0, #1	; 0x1
    5b34:	e8bd8070 	pop	{r4, r5, r6, pc}
    5b38:	0800d208 	.word	0x0800d208

00005b3c <sha1_init>:
    5b3c:	e1a03ea0 	lsr	r3, r0, #29
    5b40:	e1a0c180 	lsl	ip, r0, #3
    5b44:	e92d4010 	push	{r4, lr}
    5b48:	e59f403c 	ldr	r4, [pc, #60]	; 5b8c <sha1_init+0x50>
    5b4c:	e1a0200c 	mov	r2, ip
    5b50:	e1a01003 	mov	r1, r3
    5b54:	e3a00001 	mov	r0, #1	; 0x1
    5b58:	e5843004 	str	r3, [r4, #4]
    5b5c:	e584c000 	str	ip, [r4]
    5b60:	eb0000f1 	bl	5f2c <hash_module_init>
    5b64:	e3500000 	cmp	r0, #0	; 0x0
    5b68:	05942000 	ldreq	r2, [r4]
    5b6c:	05943004 	ldreq	r3, [r4, #4]
    5b70:	01a021a2 	lsreq	r2, r2, #3
    5b74:	01822e83 	orreq	r2, r2, r3, lsl #29
    5b78:	01a031a3 	lsreq	r3, r3, #3
    5b7c:	13e00000 	mvnne	r0, #0	; 0x0
    5b80:	05843004 	streq	r3, [r4, #4]
    5b84:	05842000 	streq	r2, [r4]
    5b88:	e8bd8010 	pop	{r4, pc}
    5b8c:	0800d208 	.word	0x0800d208

00005b90 <sha256_value>:
    5b90:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5b94:	e59f3028 	ldr	r3, [pc, #40]	; 5bc4 <sha256_value+0x34>
    5b98:	e24dd004 	sub	sp, sp, #4	; 0x4
    5b9c:	e893000c 	ldm	r3, {r2, r3}
    5ba0:	e1823003 	orr	r3, r2, r3
    5ba4:	e3530000 	cmp	r3, #0	; 0x0
    5ba8:	13e00000 	mvnne	r0, #0	; 0x0
    5bac:	1a000002 	bne	5bbc <sha256_value+0x2c>
    5bb0:	eb000096 	bl	5e10 <hash_module_get_result>
    5bb4:	e3500000 	cmp	r0, #0	; 0x0
    5bb8:	13e00001 	mvnne	r0, #1	; 0x1
    5bbc:	e28dd004 	add	sp, sp, #4	; 0x4
    5bc0:	e8bd8000 	pop	{pc}
    5bc4:	0800d210 	.word	0x0800d210

00005bc8 <sha256_data>:
    5bc8:	e59f305c 	ldr	r3, [pc, #92]	; 5c2c <sha256_data+0x64>
    5bcc:	e92d4070 	push	{r4, r5, r6, lr}
    5bd0:	e1a06001 	mov	r6, r1
    5bd4:	e5931004 	ldr	r1, [r3, #4]
    5bd8:	e1a04006 	mov	r4, r6
    5bdc:	e3a05000 	mov	r5, #0	; 0x0
    5be0:	e1550001 	cmp	r5, r1
    5be4:	e593c000 	ldr	ip, [r3]
    5be8:	8a000002 	bhi	5bf8 <sha256_data+0x30>
    5bec:	1a000003 	bne	5c00 <sha256_data+0x38>
    5bf0:	e154000c 	cmp	r4, ip
    5bf4:	9a000001 	bls	5c00 <sha256_data+0x38>
    5bf8:	e3e00000 	mvn	r0, #0	; 0x0
    5bfc:	e8bd8070 	pop	{r4, r5, r6, pc}
    5c00:	e1a0200c 	mov	r2, ip
    5c04:	e1a03001 	mov	r3, r1
    5c08:	e59fc01c 	ldr	ip, [pc, #28]	; 5c2c <sha256_data+0x64>
    5c0c:	e0522004 	subs	r2, r2, r4
    5c10:	e0c33005 	sbc	r3, r3, r5
    5c14:	e1a01006 	mov	r1, r6
    5c18:	e88c000c 	stm	ip, {r2, r3}
    5c1c:	eb0000de 	bl	5f9c <hash_dma_transfer>
    5c20:	e3500000 	cmp	r0, #0	; 0x0
    5c24:	13e00001 	mvnne	r0, #1	; 0x1
    5c28:	e8bd8070 	pop	{r4, r5, r6, pc}
    5c2c:	0800d210 	.word	0x0800d210

00005c30 <sha256_init>:
    5c30:	e1a03ea0 	lsr	r3, r0, #29
    5c34:	e1a0c180 	lsl	ip, r0, #3
    5c38:	e92d4010 	push	{r4, lr}
    5c3c:	e59f403c 	ldr	r4, [pc, #60]	; 5c80 <sha256_init+0x50>
    5c40:	e1a0200c 	mov	r2, ip
    5c44:	e1a01003 	mov	r1, r3
    5c48:	e3a00003 	mov	r0, #3	; 0x3
    5c4c:	e5843004 	str	r3, [r4, #4]
    5c50:	e584c000 	str	ip, [r4]
    5c54:	eb0000b4 	bl	5f2c <hash_module_init>
    5c58:	e3500000 	cmp	r0, #0	; 0x0
    5c5c:	05942000 	ldreq	r2, [r4]
    5c60:	05943004 	ldreq	r3, [r4, #4]
    5c64:	01a021a2 	lsreq	r2, r2, #3
    5c68:	01822e83 	orreq	r2, r2, r3, lsl #29
    5c6c:	01a031a3 	lsreq	r3, r3, #3
    5c70:	13e00000 	mvnne	r0, #0	; 0x0
    5c74:	05843004 	streq	r3, [r4, #4]
    5c78:	05842000 	streq	r2, [r4]
    5c7c:	e8bd8010 	pop	{r4, pc}
    5c80:	0800d210 	.word	0x0800d210

00005c84 <crc32_init>:
    5c84:	e59f300c 	ldr	r3, [pc, #12]	; 5c98 <crc32_init+0x14>
    5c88:	e3e02000 	mvn	r2, #0	; 0x0
    5c8c:	e5832000 	str	r2, [r3]
    5c90:	e3a00000 	mov	r0, #0	; 0x0
    5c94:	e12fff1e 	bx	lr
    5c98:	0800d218 	.word	0x0800d218

00005c9c <crc32_data>:
    5c9c:	e59f311c 	ldr	r3, [pc, #284]	; 5dc0 <crc32_data+0x124>
    5ca0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    5ca4:	e593c000 	ldr	ip, [r3]
    5ca8:	e1a08000 	mov	r8, r0
    5cac:	e1a06001 	mov	r6, r1
    5cb0:	e1a04000 	mov	r4, r0
    5cb4:	e1a07001 	mov	r7, r1
    5cb8:	e59f5104 	ldr	r5, [pc, #260]	; 5dc4 <crc32_data+0x128>
    5cbc:	ea000027 	b	5d60 <crc32_data+0xc4>
    5cc0:	e5543008 	ldrb	r3, [r4, #-8]
    5cc4:	e5542007 	ldrb	r2, [r4, #-7]
    5cc8:	e02c3003 	eor	r3, ip, r3
    5ccc:	e20330ff 	and	r3, r3, #255	; 0xff
    5cd0:	e7953103 	ldr	r3, [r5, r3, lsl #2]
    5cd4:	e5541006 	ldrb	r1, [r4, #-6]
    5cd8:	e023342c 	eor	r3, r3, ip, lsr #8
    5cdc:	e0232002 	eor	r2, r3, r2
    5ce0:	e20220ff 	and	r2, r2, #255	; 0xff
    5ce4:	e7952102 	ldr	r2, [r5, r2, lsl #2]
    5ce8:	e5540005 	ldrb	r0, [r4, #-5]
    5cec:	e0222423 	eor	r2, r2, r3, lsr #8
    5cf0:	e0221001 	eor	r1, r2, r1
    5cf4:	e20110ff 	and	r1, r1, #255	; 0xff
    5cf8:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    5cfc:	e5541004 	ldrb	r1, [r4, #-4]
    5d00:	e0233422 	eor	r3, r3, r2, lsr #8
    5d04:	e0230000 	eor	r0, r3, r0
    5d08:	e20000ff 	and	r0, r0, #255	; 0xff
    5d0c:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    5d10:	e5540003 	ldrb	r0, [r4, #-3]
    5d14:	e0222423 	eor	r2, r2, r3, lsr #8
    5d18:	e0221001 	eor	r1, r2, r1
    5d1c:	e20110ff 	and	r1, r1, #255	; 0xff
    5d20:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    5d24:	e554c002 	ldrb	ip, [r4, #-2]
    5d28:	e0233422 	eor	r3, r3, r2, lsr #8
    5d2c:	e0230000 	eor	r0, r3, r0
    5d30:	e20000ff 	and	r0, r0, #255	; 0xff
    5d34:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    5d38:	e5541001 	ldrb	r1, [r4, #-1]
    5d3c:	e0222423 	eor	r2, r2, r3, lsr #8
    5d40:	e022c00c 	eor	ip, r2, ip
    5d44:	e20cc0ff 	and	ip, ip, #255	; 0xff
    5d48:	e795310c 	ldr	r3, [r5, ip, lsl #2]
    5d4c:	e0233422 	eor	r3, r3, r2, lsr #8
    5d50:	e0231001 	eor	r1, r3, r1
    5d54:	e20110ff 	and	r1, r1, #255	; 0xff
    5d58:	e7952101 	ldr	r2, [r5, r1, lsl #2]
    5d5c:	e022c423 	eor	ip, r2, r3, lsr #8
    5d60:	e3570007 	cmp	r7, #7	; 0x7
    5d64:	e2844008 	add	r4, r4, #8	; 0x8
    5d68:	e2477008 	sub	r7, r7, #8	; 0x8
    5d6c:	8affffd3 	bhi	5cc0 <crc32_data+0x24>
    5d70:	e3c63007 	bic	r3, r6, #7	; 0x7
    5d74:	e0880003 	add	r0, r8, r3
    5d78:	e59f3040 	ldr	r3, [pc, #64]	; 5dc0 <crc32_data+0x124>
    5d7c:	e2161007 	ands	r1, r6, #7	; 0x7
    5d80:	e583c000 	str	ip, [r3]
    5d84:	0a00000b 	beq	5db8 <crc32_data+0x11c>
    5d88:	e59f4034 	ldr	r4, [pc, #52]	; 5dc4 <crc32_data+0x128>
    5d8c:	e3a02000 	mov	r2, #0	; 0x0
    5d90:	e7d03002 	ldrb	r3, [r0, r2]
    5d94:	e2822001 	add	r2, r2, #1	; 0x1
    5d98:	e02c3003 	eor	r3, ip, r3
    5d9c:	e20330ff 	and	r3, r3, #255	; 0xff
    5da0:	e7943103 	ldr	r3, [r4, r3, lsl #2]
    5da4:	e1510002 	cmp	r1, r2
    5da8:	e023c42c 	eor	ip, r3, ip, lsr #8
    5dac:	1afffff7 	bne	5d90 <crc32_data+0xf4>
    5db0:	e59f3008 	ldr	r3, [pc, #8]	; 5dc0 <crc32_data+0x124>
    5db4:	e583c000 	str	ip, [r3]
    5db8:	e3a00000 	mov	r0, #0	; 0x0
    5dbc:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5dc0:	0800d218 	.word	0x0800d218
    5dc4:	00011248 	.word	0x00011248

00005dc8 <crc32_value>:
    5dc8:	e59f3010 	ldr	r3, [pc, #16]	; 5de0 <crc32_value+0x18>
    5dcc:	e5933000 	ldr	r3, [r3]
    5dd0:	e1e03003 	mvn	r3, r3
    5dd4:	e5803000 	str	r3, [r0]
    5dd8:	e3a00000 	mov	r0, #0	; 0x0
    5ddc:	e12fff1e 	bx	lr
    5de0:	0800d218 	.word	0x0800d218

00005de4 <crc32_le>:
    5de4:	e92d4010 	push	{r4, lr}
    5de8:	e59f401c 	ldr	r4, [pc, #28]	; 5e0c <crc32_le+0x28>
    5dec:	e3e03000 	mvn	r3, #0	; 0x0
    5df0:	e1a00001 	mov	r0, r1
    5df4:	e1a01002 	mov	r1, r2
    5df8:	e5843000 	str	r3, [r4]
    5dfc:	ebffffa6 	bl	5c9c <crc32_data>
    5e00:	e5940000 	ldr	r0, [r4]
    5e04:	e1e00000 	mvn	r0, r0
    5e08:	e8bd8010 	pop	{r4, pc}
    5e0c:	0800d218 	.word	0x0800d218

00005e10 <hash_module_get_result>:
    5e10:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    5e14:	e1a08000 	mov	r8, r0
    5e18:	eb000aaa 	bl	88c8 <get_ticks>
    5e1c:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    5e20:	e59f3088 	ldr	r3, [pc, #136]	; 5eb0 <hash_module_get_result+0xa0>
    5e24:	e3a05000 	mov	r5, #0	; 0x0
    5e28:	e0944000 	adds	r4, r4, r0
    5e2c:	e0a55001 	adc	r5, r5, r1
    5e30:	e1a07003 	mov	r7, r3
    5e34:	e593680c 	ldr	r6, [r3, #2060]
    5e38:	ea000008 	b	5e60 <hash_module_get_result+0x50>
    5e3c:	e597680c 	ldr	r6, [r7, #2060]
    5e40:	eb000aa0 	bl	88c8 <get_ticks>
    5e44:	e1510005 	cmp	r1, r5
    5e48:	8a000002 	bhi	5e58 <hash_module_get_result+0x48>
    5e4c:	1a000003 	bne	5e60 <hash_module_get_result+0x50>
    5e50:	e1500004 	cmp	r0, r4
    5e54:	9a000001 	bls	5e60 <hash_module_get_result+0x50>
    5e58:	e3a00001 	mov	r0, #1	; 0x1
    5e5c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5e60:	e3160001 	tst	r6, #1	; 0x1
    5e64:	0afffff4 	beq	5e3c <hash_module_get_result+0x2c>
    5e68:	e59f0044 	ldr	r0, [pc, #68]	; 5eb4 <hash_module_get_result+0xa4>
    5e6c:	e59fc044 	ldr	ip, [pc, #68]	; 5eb8 <hash_module_get_result+0xa8>
    5e70:	e1a03000 	mov	r3, r0
    5e74:	e6931008 	ldr	r1, [r3], r8
    5e78:	e59f203c 	ldr	r2, [pc, #60]	; 5ebc <hash_module_get_result+0xac>
    5e7c:	e2800004 	add	r0, r0, #4	; 0x4
    5e80:	e0832002 	add	r2, r3, r2
    5e84:	e150000c 	cmp	r0, ip
    5e88:	e5821000 	str	r1, [r2]
    5e8c:	1afffff7 	bne	5e70 <hash_module_get_result+0x60>
    5e90:	e59f2018 	ldr	r2, [pc, #24]	; 5eb0 <hash_module_get_result+0xa0>
    5e94:	e3a03001 	mov	r3, #1	; 0x1
    5e98:	e582380c 	str	r3, [r2, #2060]
    5e9c:	e5923800 	ldr	r3, [r2, #2048]
    5ea0:	e3a00000 	mov	r0, #0	; 0x0
    5ea4:	e3c33080 	bic	r3, r3, #128	; 0x80
    5ea8:	e5823800 	str	r3, [r2, #2048]
    5eac:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5eb0:	27700000 	.word	0x27700000
    5eb4:	27700858 	.word	0x27700858
    5eb8:	27700878 	.word	0x27700878
    5ebc:	d88ff7a8 	.word	0xd88ff7a8

00005ec0 <hash_module_clear>:
    5ec0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5ec4:	e59f405c 	ldr	r4, [pc, #92]	; 5f28 <hash_module_clear+0x68>
    5ec8:	e3a03002 	mov	r3, #2	; 0x2
    5ecc:	e5843800 	str	r3, [r4, #2048]
    5ed0:	e24dd004 	sub	sp, sp, #4	; 0x4
    5ed4:	eb000a7b 	bl	88c8 <get_ticks>
    5ed8:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    5edc:	e3a07000 	mov	r7, #0	; 0x0
    5ee0:	e0966000 	adds	r6, r6, r0
    5ee4:	e1a05004 	mov	r5, r4
    5ee8:	e0a77001 	adc	r7, r7, r1
    5eec:	e5944800 	ldr	r4, [r4, #2048]
    5ef0:	ea000008 	b	5f18 <hash_module_clear+0x58>
    5ef4:	e5954800 	ldr	r4, [r5, #2048]
    5ef8:	eb000a72 	bl	88c8 <get_ticks>
    5efc:	e1510007 	cmp	r1, r7
    5f00:	8a000002 	bhi	5f10 <hash_module_clear+0x50>
    5f04:	1a000003 	bne	5f18 <hash_module_clear+0x58>
    5f08:	e1500006 	cmp	r0, r6
    5f0c:	9a000001 	bls	5f18 <hash_module_clear+0x58>
    5f10:	e3a00001 	mov	r0, #1	; 0x1
    5f14:	ea000001 	b	5f20 <hash_module_clear+0x60>
    5f18:	e2140002 	ands	r0, r4, #2	; 0x2
    5f1c:	1afffff4 	bne	5ef4 <hash_module_clear+0x34>
    5f20:	e28dd004 	add	sp, sp, #4	; 0x4
    5f24:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5f28:	27700000 	.word	0x27700000

00005f2c <hash_module_init>:
    5f2c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5f30:	e59f4060 	ldr	r4, [pc, #96]	; 5f98 <hash_module_init+0x6c>
    5f34:	e24dd004 	sub	sp, sp, #4	; 0x4
    5f38:	e5943800 	ldr	r3, [r4, #2048]
    5f3c:	e1a05000 	mov	r5, r0
    5f40:	e3c33080 	bic	r3, r3, #128	; 0x80
    5f44:	e5843800 	str	r3, [r4, #2048]
    5f48:	e1a07001 	mov	r7, r1
    5f4c:	e1a06002 	mov	r6, r2
    5f50:	ebffffda 	bl	5ec0 <hash_module_clear>
    5f54:	e3500000 	cmp	r0, #0	; 0x0
    5f58:	13a00001 	movne	r0, #1	; 0x1
    5f5c:	1a00000b 	bne	5f90 <hash_module_init+0x64>
    5f60:	e3a03001 	mov	r3, #1	; 0x1
    5f64:	e1a02185 	lsl	r2, r5, #3
    5f68:	e5843804 	str	r3, [r4, #2052]
    5f6c:	e5842800 	str	r2, [r4, #2048]
    5f70:	e5847814 	str	r7, [r4, #2068]
    5f74:	e5846810 	str	r6, [r4, #2064]
    5f78:	e5943800 	ldr	r3, [r4, #2048]
    5f7c:	e3833080 	orr	r3, r3, #128	; 0x80
    5f80:	e5843800 	str	r3, [r4, #2048]
    5f84:	e5943800 	ldr	r3, [r4, #2048]
    5f88:	e3833004 	orr	r3, r3, #4	; 0x4
    5f8c:	e5843800 	str	r3, [r4, #2048]
    5f90:	e28dd004 	add	sp, sp, #4	; 0x4
    5f94:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5f98:	27700000 	.word	0x27700000

00005f9c <hash_dma_transfer>:
    5f9c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    5fa0:	e1a08001 	mov	r8, r1
    5fa4:	e24dd004 	sub	sp, sp, #4	; 0x4
    5fa8:	e1a09000 	mov	r9, r0
    5fac:	e1a06001 	mov	r6, r1
    5fb0:	e59fa090 	ldr	sl, [pc, #144]	; 6048 <hash_dma_transfer+0xac>
    5fb4:	e3a0b000 	mov	fp, #0	; 0x0
    5fb8:	ea00001b 	b	602c <hash_dma_transfer+0x90>
    5fbc:	e3560902 	cmp	r6, #32768	; 0x8000
    5fc0:	b1a07006 	movlt	r7, r6
    5fc4:	a3a07902 	movge	r7, #32768	; 0x8000
    5fc8:	e0661008 	rsb	r1, r6, r8
    5fcc:	e0891001 	add	r1, r9, r1
    5fd0:	e3a00002 	mov	r0, #2	; 0x2
    5fd4:	e1a02007 	mov	r2, r7
    5fd8:	ebfffe60 	bl	5960 <dma_sdma_init>
    5fdc:	e3500000 	cmp	r0, #0	; 0x0
    5fe0:	1a000015 	bne	603c <hash_dma_transfer+0xa0>
    5fe4:	e2800002 	add	r0, r0, #2	; 0x2
    5fe8:	ebfffe4c 	bl	5920 <dma_start>
    5fec:	eb000a35 	bl	88c8 <get_ticks>
    5ff0:	e090400a 	adds	r4, r0, sl
    5ff4:	e0a1500b 	adc	r5, r1, fp
    5ff8:	ea000005 	b	6014 <hash_dma_transfer+0x78>
    5ffc:	eb000a31 	bl	88c8 <get_ticks>
    6000:	e1510005 	cmp	r1, r5
    6004:	8a00000c 	bhi	603c <hash_dma_transfer+0xa0>
    6008:	1a000001 	bne	6014 <hash_dma_transfer+0x78>
    600c:	e1500004 	cmp	r0, r4
    6010:	8a000009 	bhi	603c <hash_dma_transfer+0xa0>
    6014:	e3a01003 	mov	r1, #3	; 0x3
    6018:	e3a00002 	mov	r0, #2	; 0x2
    601c:	ebfffe45 	bl	5938 <dma_get_status>
    6020:	e3500000 	cmp	r0, #0	; 0x0
    6024:	0afffff4 	beq	5ffc <hash_dma_transfer+0x60>
    6028:	e0676006 	rsb	r6, r7, r6
    602c:	e3560000 	cmp	r6, #0	; 0x0
    6030:	1affffe1 	bne	5fbc <hash_dma_transfer+0x20>
    6034:	e1a00006 	mov	r0, r6
    6038:	ea000000 	b	6040 <hash_dma_transfer+0xa4>
    603c:	e3a00001 	mov	r0, #1	; 0x1
    6040:	e28dd004 	add	sp, sp, #4	; 0x4
    6044:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6048:	000f4240 	.word	0x000f4240

0000604c <cdbg_log_toggle>:
    604c:	e59f3014 	ldr	r3, [pc, #20]	; 6068 <cdbg_log_toggle+0x1c>
    6050:	e3500000 	cmp	r0, #0	; 0x0
    6054:	e59f2010 	ldr	r2, [pc, #16]	; 606c <cdbg_log_toggle+0x20>
    6058:	01a02003 	moveq	r2, r3
    605c:	e59f300c 	ldr	r3, [pc, #12]	; 6070 <cdbg_log_toggle+0x24>
    6060:	e5832000 	str	r2, [r3]
    6064:	e12fff1e 	bx	lr
    6068:	0000e110 	.word	0x0000e110
    606c:	0000e194 	.word	0x0000e194
    6070:	0800d764 	.word	0x0800d764

00006074 <cdbg_wipe_bootarea>:
    6074:	e59f3018 	ldr	r3, [pc, #24]	; 6094 <cdbg_wipe_bootarea+0x20>
    6078:	e59f1018 	ldr	r1, [pc, #24]	; 6098 <cdbg_wipe_bootarea+0x24>
    607c:	e59f2018 	ldr	r2, [pc, #24]	; 609c <cdbg_wipe_bootarea+0x28>
    6080:	e5031004 	str	r1, [r3, #-4]
    6084:	e2833004 	add	r3, r3, #4	; 0x4
    6088:	e1530002 	cmp	r3, r2
    608c:	1afffffb 	bne	6080 <cdbg_wipe_bootarea+0xc>
    6090:	e12fff1e 	bx	lr
    6094:	08000004 	.word	0x08000004
    6098:	eafffffe 	.word	0xeafffffe
    609c:	08008004 	.word	0x08008004

000060a0 <cdbg_verify_burn>:
    60a0:	e59f3004 	ldr	r3, [pc, #4]	; 60ac <cdbg_verify_burn+0xc>
    60a4:	e5930000 	ldr	r0, [r3]
    60a8:	e12fff1e 	bx	lr
    60ac:	0800d21c 	.word	0x0800d21c

000060b0 <cdbg_verify_burn_key>:
    60b0:	e59f0000 	ldr	r0, [pc, #0]	; 60b8 <cdbg_verify_burn_key+0x8>
    60b4:	e12fff1e 	bx	lr
    60b8:	0800d220 	.word	0x0800d220

000060bc <cdbg_verify_burn_enable>:
    60bc:	e3500000 	cmp	r0, #0	; 0x0
    60c0:	e59f3018 	ldr	r3, [pc, #24]	; 60e0 <cdbg_verify_burn_enable+0x24>
    60c4:	03a02000 	moveq	r2, #0	; 0x0
    60c8:	13a02001 	movne	r2, #1	; 0x1
    60cc:	e5832000 	str	r2, [r3]
    60d0:	012fff1e 	bxeq	lr
    60d4:	e59f0008 	ldr	r0, [pc, #8]	; 60e4 <cdbg_verify_burn_enable+0x28>
    60d8:	e3a02020 	mov	r2, #32	; 0x20
    60dc:	eaffecc7 	b	1400 <memcpy>
    60e0:	0800d21c 	.word	0x0800d21c
    60e4:	0800d220 	.word	0x0800d220

000060e8 <cdbg_jump>:
    60e8:	e92d4030 	push	{r4, r5, lr}
    60ec:	e1a04000 	mov	r4, r0
    60f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    60f4:	e1a05001 	mov	r5, r1
    60f8:	ebfffc61 	bl	5284 <ss_lock_isk>
    60fc:	e59f001c 	ldr	r0, [pc, #28]	; 6120 <cdbg_jump+0x38>
    6100:	e1a01004 	mov	r1, r4
    6104:	e59f3018 	ldr	r3, [pc, #24]	; 6124 <cdbg_jump+0x3c>
    6108:	e1a0e00f 	mov	lr, pc
    610c:	e593f000 	ldr	pc, [r3]
    6110:	e3000bb8 	movw	r0, #3000	; 0xbb8
    6114:	eb0009fc 	bl	890c <udelay>
    6118:	e1a00005 	mov	r0, r5
    611c:	e12fff34 	blx	r4
    6120:	0001300a 	.word	0x0001300a
    6124:	0800d764 	.word	0x0800d764

00006128 <cdbg_restore_os>:
    6128:	e92d4010 	push	{r4, lr}
    612c:	ebffe9a7 	bl	7d0 <get_wakeup_entry>
    6130:	e59f301c 	ldr	r3, [pc, #28]	; 6154 <cdbg_restore_os+0x2c>
    6134:	e1a04000 	mov	r4, r0
    6138:	e59f0018 	ldr	r0, [pc, #24]	; 6158 <cdbg_restore_os+0x30>
    613c:	e1a0e00f 	mov	lr, pc
    6140:	e593f000 	ldr	pc, [r3]
    6144:	e1a00004 	mov	r0, r4
    6148:	e3a01000 	mov	r1, #0	; 0x0
    614c:	e8bd4010 	pop	{r4, lr}
    6150:	eaffffe4 	b	60e8 <cdbg_jump>
    6154:	0800d764 	.word	0x0800d764
    6158:	0001301a 	.word	0x0001301a

0000615c <cdbg_get_pp>:
    615c:	e92d4070 	push	{r4, r5, r6, lr}
    6160:	e24dd008 	sub	sp, sp, #8	; 0x8
    6164:	e1a0000d 	mov	r0, sp
    6168:	e59f103c 	ldr	r1, [pc, #60]	; 61ac <cdbg_get_pp+0x50>
    616c:	e3a02008 	mov	r2, #8	; 0x8
    6170:	ebffeca2 	bl	1400 <memcpy>
    6174:	e1a0400d 	mov	r4, sp
    6178:	e1a0600d 	mov	r6, sp
    617c:	ea000004 	b	6194 <cdbg_get_pp+0x38>
    6180:	eb002021 	bl	e20c <getc>
    6184:	e1500004 	cmp	r0, r4
    6188:	e2854001 	add	r4, r5, #1	; 0x1
    618c:	e1a05006 	mov	r5, r6
    6190:	1a000000 	bne	6198 <cdbg_get_pp+0x3c>
    6194:	e1a05004 	mov	r5, r4
    6198:	e5d54000 	ldrb	r4, [r5]
    619c:	e3540000 	cmp	r4, #0	; 0x0
    61a0:	1afffff6 	bne	6180 <cdbg_get_pp+0x24>
    61a4:	e28dd008 	add	sp, sp, #8	; 0x8
    61a8:	e8bd8070 	pop	{r4, r5, r6, pc}
    61ac:	000118b2 	.word	0x000118b2

000061b0 <cdbg_shell>:
    61b0:	e92d4010 	push	{r4, lr}
    61b4:	ebfffc32 	bl	5284 <ss_lock_isk>
    61b8:	e59f3048 	ldr	r3, [pc, #72]	; 6208 <cdbg_shell+0x58>
    61bc:	e59f1048 	ldr	r1, [pc, #72]	; 620c <cdbg_shell+0x5c>
    61c0:	e59f2048 	ldr	r2, [pc, #72]	; 6210 <cdbg_shell+0x60>
    61c4:	e5031004 	str	r1, [r3, #-4]
    61c8:	e2833004 	add	r3, r3, #4	; 0x4
    61cc:	e1530002 	cmp	r3, r2
    61d0:	1afffffb 	bne	61c4 <cdbg_shell+0x14>
    61d4:	e3a00001 	mov	r0, #1	; 0x1
    61d8:	eb0027e5 	bl	10174 <ss_jtag_en>
    61dc:	e59f4030 	ldr	r4, [pc, #48]	; 6214 <cdbg_shell+0x64>
    61e0:	e59f3030 	ldr	r3, [pc, #48]	; 6218 <cdbg_shell+0x68>
    61e4:	e59f0030 	ldr	r0, [pc, #48]	; 621c <cdbg_shell+0x6c>
    61e8:	e5843000 	str	r3, [r4]
    61ec:	eb001fe8 	bl	e194 <printf_on>
    61f0:	ebffffd9 	bl	615c <cdbg_get_pp>
    61f4:	e59f0024 	ldr	r0, [pc, #36]	; 6220 <cdbg_shell+0x70>
    61f8:	e1a0e00f 	mov	lr, pc
    61fc:	e594f000 	ldr	pc, [r4]
    6200:	eb0022ec 	bl	edb8 <main_loop>
    6204:	eafffffd 	b	6200 <cdbg_shell+0x50>
    6208:	08000004 	.word	0x08000004
    620c:	eafffffe 	.word	0xeafffffe
    6210:	08008004 	.word	0x08008004
    6214:	0800d764 	.word	0x0800d764
    6218:	0000e194 	.word	0x0000e194
    621c:	00013029 	.word	0x00013029
    6220:	0001302e 	.word	0x0001302e

00006224 <cdbg_do_config>:
    6224:	e92d4030 	push	{r4, r5, lr}
    6228:	e59f307c 	ldr	r3, [pc, #124]	; 62ac <cdbg_do_config+0x88>
    622c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6230:	e1a04000 	mov	r4, r0
    6234:	e1a05001 	mov	r5, r1
    6238:	e59f0070 	ldr	r0, [pc, #112]	; 62b0 <cdbg_do_config+0x8c>
    623c:	e59f1070 	ldr	r1, [pc, #112]	; 62b4 <cdbg_do_config+0x90>
    6240:	e1a0e00f 	mov	lr, pc
    6244:	e593f000 	ldr	pc, [r3]
    6248:	e3a02000 	mov	r2, #0	; 0x0
    624c:	e59f0060 	ldr	r0, [pc, #96]	; 62b4 <cdbg_do_config+0x90>
    6250:	ea000006 	b	6270 <cdbg_do_config+0x4c>
    6254:	e7943002 	ldr	r3, [r4, r2]
    6258:	e2822004 	add	r2, r2, #4	; 0x4
    625c:	e1530000 	cmp	r3, r0
    6260:	1a000002 	bne	6270 <cdbg_do_config+0x4c>
    6264:	e3510000 	cmp	r1, #0	; 0x0
    6268:	1a000004 	bne	6280 <cdbg_do_config+0x5c>
    626c:	ea000007 	b	6290 <cdbg_do_config+0x6c>
    6270:	e1520005 	cmp	r2, r5
    6274:	e0841002 	add	r1, r4, r2
    6278:	bafffff5 	blt	6254 <cdbg_do_config+0x30>
    627c:	ea000003 	b	6290 <cdbg_do_config+0x6c>
    6280:	e5910004 	ldr	r0, [r1, #4]
    6284:	eb000c10 	bl	92cc <spi_set_bytes>
    6288:	e3a00000 	mov	r0, #0	; 0x0
    628c:	ea000004 	b	62a4 <cdbg_do_config+0x80>
    6290:	e59f0020 	ldr	r0, [pc, #32]	; 62b8 <cdbg_do_config+0x94>
    6294:	e59f3010 	ldr	r3, [pc, #16]	; 62ac <cdbg_do_config+0x88>
    6298:	e1a0e00f 	mov	lr, pc
    629c:	e593f000 	ldr	pc, [r3]
    62a0:	e3e00000 	mvn	r0, #0	; 0x0
    62a4:	e28dd004 	add	sp, sp, #4	; 0x4
    62a8:	e8bd8030 	pop	{r4, r5, pc}
    62ac:	0800d764 	.word	0x0800d764
    62b0:	0001303f 	.word	0x0001303f
    62b4:	4a82467c 	.word	0x4a82467c
    62b8:	0001305c 	.word	0x0001305c

000062bc <cdbg_config_isi>:
    62bc:	e92d4030 	push	{r4, r5, lr}
    62c0:	e24dd004 	sub	sp, sp, #4	; 0x4
    62c4:	e1a04000 	mov	r4, r0
    62c8:	ebffee0f 	bl	1b0c <isi_check>
    62cc:	e2505000 	subs	r5, r0, #0	; 0x0
    62d0:	0a000007 	beq	62f4 <cdbg_config_isi+0x38>
    62d4:	e59f002c 	ldr	r0, [pc, #44]	; 6308 <cdbg_config_isi+0x4c>
    62d8:	e1a01005 	mov	r1, r5
    62dc:	e59f3028 	ldr	r3, [pc, #40]	; 630c <cdbg_config_isi+0x50>
    62e0:	e1a0e00f 	mov	lr, pc
    62e4:	e593f000 	ldr	pc, [r3]
    62e8:	e1a00005 	mov	r0, r5
    62ec:	e28dd004 	add	sp, sp, #4	; 0x4
    62f0:	e8bd8030 	pop	{r4, r5, pc}
    62f4:	e594100c 	ldr	r1, [r4, #12]
    62f8:	e2840c02 	add	r0, r4, #512	; 0x200
    62fc:	e28dd004 	add	sp, sp, #4	; 0x4
    6300:	e8bd4030 	pop	{r4, r5, lr}
    6304:	eaffffc6 	b	6224 <cdbg_do_config>
    6308:	0001307b 	.word	0x0001307b
    630c:	0800d764 	.word	0x0800d764

00006310 <burn_image_loop>:
    6310:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6314:	e24dd024 	sub	sp, sp, #36	; 0x24
    6318:	e59d704c 	ldr	r7, [sp, #76]
    631c:	e1a06000 	mov	r6, r0
    6320:	e3570000 	cmp	r7, #0	; 0x0
    6324:	e58d1014 	str	r1, [sp, #20]
    6328:	e58d2010 	str	r2, [sp, #16]
    632c:	e58d300c 	str	r3, [sp, #12]
    6330:	1a00000c 	bne	6368 <burn_image_loop+0x58>
    6334:	e59f426c 	ldr	r4, [pc, #620]	; 65a8 <burn_image_loop+0x298>
    6338:	e59f026c 	ldr	r0, [pc, #620]	; 65ac <burn_image_loop+0x29c>
    633c:	e3a0102e 	mov	r1, #46	; 0x2e
    6340:	e1a0e00f 	mov	lr, pc
    6344:	e594f000 	ldr	pc, [r4]
    6348:	e59f0260 	ldr	r0, [pc, #608]	; 65b0 <burn_image_loop+0x2a0>
    634c:	e59f1260 	ldr	r1, [pc, #608]	; 65b4 <burn_image_loop+0x2a4>
    6350:	e1a0e00f 	mov	lr, pc
    6354:	e594f000 	ldr	pc, [r4]
    6358:	e59f0258 	ldr	r0, [pc, #600]	; 65b8 <burn_image_loop+0x2a8>
    635c:	e1a0e00f 	mov	lr, pc
    6360:	e594f000 	ldr	pc, [r4]
    6364:	eafffffe 	b	6364 <burn_image_loop+0x54>
    6368:	e59d0014 	ldr	r0, [sp, #20]
    636c:	e59d100c 	ldr	r1, [sp, #12]
    6370:	e1500001 	cmp	r0, r1
    6374:	1a000005 	bne	6390 <burn_image_loop+0x80>
    6378:	e59f023c 	ldr	r0, [pc, #572]	; 65bc <burn_image_loop+0x2ac>
    637c:	e59f3224 	ldr	r3, [pc, #548]	; 65a8 <burn_image_loop+0x298>
    6380:	e1a0e00f 	mov	lr, pc
    6384:	e593f000 	ldr	pc, [r3]
    6388:	e3e00001 	mvn	r0, #1	; 0x1
    638c:	ea000083 	b	65a0 <burn_image_loop+0x290>
    6390:	e596c000 	ldr	ip, [r6]
    6394:	e3a03000 	mov	r3, #0	; 0x0
    6398:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    639c:	e002200c 	and	r2, r2, ip
    63a0:	e1a01483 	lsl	r1, r3, #9
    63a4:	e1d600b8 	ldrh	r0, [r6, #8]
    63a8:	e1811ba2 	orr	r1, r1, r2, lsr #23
    63ac:	e1a0e482 	lsl	lr, r2, #9
    63b0:	e1a03001 	mov	r3, r1
    63b4:	e3a02401 	mov	r2, #16777216	; 0x1000000
    63b8:	e5961004 	ldr	r1, [r6, #4]
    63bc:	e1a0cc2c 	lsr	ip, ip, #24
    63c0:	e1a05000 	mov	r5, r0
    63c4:	e09e8002 	adds	r8, lr, r2
    63c8:	e1a09003 	mov	r9, r3
    63cc:	e3a04000 	mov	r4, #0	; 0x0
    63d0:	e3a03000 	mov	r3, #0	; 0x0
    63d4:	e20cc03f 	and	ip, ip, #63	; 0x3f
    63d8:	e59d000c 	ldr	r0, [sp, #12]
    63dc:	e0a99003 	adc	r9, r9, r3
    63e0:	e184a001 	orr	sl, r4, r1
    63e4:	e1a0b005 	mov	fp, r5
    63e8:	e58dc018 	str	ip, [sp, #24]
    63ec:	e596500c 	ldr	r5, [r6, #12]
    63f0:	ebfffa75 	bl	4dcc <vs_device_erasable>
    63f4:	e1500004 	cmp	r0, r4
    63f8:	0a000006 	beq	6418 <burn_image_loop+0x108>
    63fc:	e1a01004 	mov	r1, r4
    6400:	e59d000c 	ldr	r0, [sp, #12]
    6404:	ebfffb58 	bl	516c <vs_assign_by_id>
    6408:	e1a0000a 	mov	r0, sl
    640c:	e1a0100b 	mov	r1, fp
    6410:	e59d2048 	ldr	r2, [sp, #72]
    6414:	ebfff9c3 	bl	4b28 <vs_erase>
    6418:	e59d0014 	ldr	r0, [sp, #20]
    641c:	e3a01001 	mov	r1, #1	; 0x1
    6420:	ebfffb51 	bl	516c <vs_assign_by_id>
    6424:	e59d2018 	ldr	r2, [sp, #24]
    6428:	e3520002 	cmp	r2, #2	; 0x2
    642c:	1a000019 	bne	6498 <burn_image_loop+0x188>
    6430:	e3a01c02 	mov	r1, #512	; 0x200
    6434:	e58d1000 	str	r1, [sp]
    6438:	e59d1010 	ldr	r1, [sp, #16]
    643c:	e1a02008 	mov	r2, r8
    6440:	e1a03009 	mov	r3, r9
    6444:	e1a00007 	mov	r0, r7
    6448:	e58d1004 	str	r1, [sp, #4]
    644c:	ebfffa9f 	bl	4ed0 <vs_read>
    6450:	e1a00007 	mov	r0, r7
    6454:	ebffed51 	bl	19a0 <isi_check_header>
    6458:	e2504000 	subs	r4, r0, #0	; 0x0
    645c:	1a00000d 	bne	6498 <burn_image_loop+0x188>
    6460:	e5973004 	ldr	r3, [r7, #4]
    6464:	e1b03e23 	lsrs	r3, r3, #28
    6468:	0a00000a 	beq	6498 <burn_image_loop+0x188>
    646c:	e59f014c 	ldr	r0, [pc, #332]	; 65c0 <burn_image_loop+0x2b0>
    6470:	e59f3130 	ldr	r3, [pc, #304]	; 65a8 <burn_image_loop+0x298>
    6474:	e1a0e00f 	mov	lr, pc
    6478:	e593f000 	ldr	pc, [r3]
    647c:	e59f2140 	ldr	r2, [pc, #320]	; 65c4 <burn_image_loop+0x2b4>
    6480:	e1a01004 	mov	r1, r4
    6484:	e3a00001 	mov	r0, #1	; 0x1
    6488:	ebfffb98 	bl	52f0 <ss_aes_init>
    648c:	e3a02001 	mov	r2, #1	; 0x1
    6490:	e58d201c 	str	r2, [sp, #28]
    6494:	ea000001 	b	64a0 <burn_image_loop+0x190>
    6498:	e3a03000 	mov	r3, #0	; 0x0
    649c:	e58d301c 	str	r3, [sp, #28]
    64a0:	e59f4100 	ldr	r4, [pc, #256]	; 65a8 <burn_image_loop+0x298>
    64a4:	e59d1048 	ldr	r1, [sp, #72]
    64a8:	e59f0118 	ldr	r0, [pc, #280]	; 65c8 <burn_image_loop+0x2b8>
    64ac:	e3a02901 	mov	r2, #16384	; 0x4000
    64b0:	e1a0e00f 	mov	lr, pc
    64b4:	e594f000 	ldr	pc, [r4]
    64b8:	e3a06901 	mov	r6, #16384	; 0x4000
    64bc:	e1a04005 	mov	r4, r5
    64c0:	ea000032 	b	6590 <burn_image_loop+0x280>
    64c4:	e1b03a23 	lsrs	r3, r3, #20
    64c8:	0a000005 	beq	64e4 <burn_image_loop+0x1d4>
    64cc:	e1a01a25 	lsr	r1, r5, #20
    64d0:	e59f00f4 	ldr	r0, [pc, #244]	; 65cc <burn_image_loop+0x2bc>
    64d4:	e59f20cc 	ldr	r2, [pc, #204]	; 65a8 <burn_image_loop+0x298>
    64d8:	e1a0e00f 	mov	lr, pc
    64dc:	e592f000 	ldr	pc, [r2]
    64e0:	e1a04005 	mov	r4, r5
    64e4:	e3a01000 	mov	r1, #0	; 0x0
    64e8:	e59d0014 	ldr	r0, [sp, #20]
    64ec:	ebfffb1e 	bl	516c <vs_assign_by_id>
    64f0:	e59d1010 	ldr	r1, [sp, #16]
    64f4:	e1a02008 	mov	r2, r8
    64f8:	e1a03009 	mov	r3, r9
    64fc:	e1a00007 	mov	r0, r7
    6500:	e58d6000 	str	r6, [sp]
    6504:	e58d1004 	str	r1, [sp, #4]
    6508:	ebfffa70 	bl	4ed0 <vs_read>
    650c:	e59d201c 	ldr	r2, [sp, #28]
    6510:	e3520000 	cmp	r2, #0	; 0x0
    6514:	0a000003 	beq	6528 <burn_image_loop+0x218>
    6518:	e1a01007 	mov	r1, r7
    651c:	e1a02006 	mov	r2, r6
    6520:	e1a00007 	mov	r0, r7
    6524:	ebfffb60 	bl	52ac <ss_aes_data>
    6528:	e3a01000 	mov	r1, #0	; 0x0
    652c:	e59d000c 	ldr	r0, [sp, #12]
    6530:	ebfffb0d 	bl	516c <vs_assign_by_id>
    6534:	e2501000 	subs	r1, r0, #0	; 0x0
    6538:	1a000005 	bne	6554 <burn_image_loop+0x244>
    653c:	e1a00007 	mov	r0, r7
    6540:	e1a0200a 	mov	r2, sl
    6544:	e1a0300b 	mov	r3, fp
    6548:	e58d1004 	str	r1, [sp, #4]
    654c:	e58d6000 	str	r6, [sp]
    6550:	ebfff95e 	bl	4ad0 <vs_write>
    6554:	e59d0018 	ldr	r0, [sp, #24]
    6558:	e3a01000 	mov	r1, #0	; 0x0
    655c:	e3500001 	cmp	r0, #1	; 0x1
    6560:	e3a00901 	mov	r0, #16384	; 0x4000
    6564:	93a03901 	movls	r3, #16384	; 0x4000
    6568:	83a03000 	movhi	r3, #0	; 0x0
    656c:	e3550901 	cmp	r5, #16384	; 0x4000
    6570:	31a02005 	movcc	r2, r5
    6574:	23a02901 	movcs	r2, #16384	; 0x4000
    6578:	e09aa000 	adds	sl, sl, r0
    657c:	e0abb001 	adc	fp, fp, r1
    6580:	e0988000 	adds	r8, r8, r0
    6584:	e0877003 	add	r7, r7, r3
    6588:	e0a99001 	adc	r9, r9, r1
    658c:	e0625005 	rsb	r5, r2, r5
    6590:	e3550000 	cmp	r5, #0	; 0x0
    6594:	e0653004 	rsb	r3, r5, r4
    6598:	1affffc9 	bne	64c4 <burn_image_loop+0x1b4>
    659c:	e1a00005 	mov	r0, r5
    65a0:	e28dd024 	add	sp, sp, #36	; 0x24
    65a4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    65a8:	0800d764 	.word	0x0800d764
    65ac:	00012f1b 	.word	0x00012f1b
    65b0:	00011da8 	.word	0x00011da8
    65b4:	00013096 	.word	0x00013096
    65b8:	0001309d 	.word	0x0001309d
    65bc:	000130b3 	.word	0x000130b3
    65c0:	000130e0 	.word	0x000130e0
    65c4:	0800d108 	.word	0x0800d108
    65c8:	000130f9 	.word	0x000130f9
    65cc:	00013108 	.word	0x00013108

000065d0 <burn_images>:
    65d0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    65d4:	e590901c 	ldr	r9, [r0, #28]
    65d8:	e24dd014 	sub	sp, sp, #20	; 0x14
    65dc:	e59f41ac 	ldr	r4, [pc, #428]	; 6790 <burn_images+0x1c0>
    65e0:	e1a05000 	mov	r5, r0
    65e4:	e58d100c 	str	r1, [sp, #12]
    65e8:	e59f01a4 	ldr	r0, [pc, #420]	; 6794 <burn_images+0x1c4>
    65ec:	e1a01009 	mov	r1, r9
    65f0:	e1a0b002 	mov	fp, r2
    65f4:	e2857020 	add	r7, r5, #32	; 0x20
    65f8:	e1a0e00f 	mov	lr, pc
    65fc:	e594f000 	ldr	pc, [r4]
    6600:	e1a0a004 	mov	sl, r4
    6604:	e3a08000 	mov	r8, #0	; 0x0
    6608:	ea000055 	b	6764 <burn_images+0x194>
    660c:	e59f0184 	ldr	r0, [pc, #388]	; 6798 <burn_images+0x1c8>
    6610:	e1a01008 	mov	r1, r8
    6614:	e1a0e00f 	mov	lr, pc
    6618:	e59af000 	ldr	pc, [sl]
    661c:	e5d73003 	ldrb	r3, [r7, #3]
    6620:	e5976008 	ldr	r6, [r7, #8]
    6624:	e203503f 	and	r5, r3, #63	; 0x3f
    6628:	e3550002 	cmp	r5, #2	; 0x2
    662c:	0a00000a 	beq	665c <burn_images+0x8c>
    6630:	ca000004 	bgt	6648 <burn_images+0x78>
    6634:	e3550000 	cmp	r5, #0	; 0x0
    6638:	0a000051 	beq	6784 <burn_images+0x1b4>
    663c:	e3550001 	cmp	r5, #1	; 0x1
    6640:	1a00001a 	bne	66b0 <burn_images+0xe0>
    6644:	ea000008 	b	666c <burn_images+0x9c>
    6648:	e3550030 	cmp	r5, #48	; 0x30
    664c:	0a00000c 	beq	6684 <burn_images+0xb4>
    6650:	e3550031 	cmp	r5, #49	; 0x31
    6654:	1a000015 	bne	66b0 <burn_images+0xe0>
    6658:	ea00000e 	b	6698 <burn_images+0xc8>
    665c:	e1a04826 	lsr	r4, r6, #16
    6660:	e354000f 	cmp	r4, #15	; 0xf
    6664:	1a000015 	bne	66c0 <burn_images+0xf0>
    6668:	ea00001f 	b	66ec <burn_images+0x11c>
    666c:	e59f0128 	ldr	r0, [pc, #296]	; 679c <burn_images+0x1cc>
    6670:	e1a01006 	mov	r1, r6
    6674:	e1a0e00f 	mov	lr, pc
    6678:	e59af000 	ldr	pc, [sl]
    667c:	e1a0e006 	mov	lr, r6
    6680:	ea000040 	b	6788 <burn_images+0x1b8>
    6684:	e59f0114 	ldr	r0, [pc, #276]	; 67a0 <burn_images+0x1d0>
    6688:	e1a0e00f 	mov	lr, pc
    668c:	e59af000 	ldr	pc, [sl]
    6690:	ebffe858 	bl	7f8 <system_reset>
    6694:	ea00003a 	b	6784 <burn_images+0x1b4>
    6698:	e59f0104 	ldr	r0, [pc, #260]	; 67a4 <burn_images+0x1d4>
    669c:	e1a0e00f 	mov	lr, pc
    66a0:	e59af000 	ldr	pc, [sl]
    66a4:	e3a00001 	mov	r0, #1	; 0x1
    66a8:	ebffe74e 	bl	3e8 <try_boot>
    66ac:	ea000034 	b	6784 <burn_images+0x1b4>
    66b0:	e59f00f0 	ldr	r0, [pc, #240]	; 67a8 <burn_images+0x1d8>
    66b4:	e1a0e00f 	mov	lr, pc
    66b8:	e59af000 	ldr	pc, [sl]
    66bc:	ea000026 	b	675c <burn_images+0x18c>
    66c0:	e1a00004 	mov	r0, r4
    66c4:	e3a01001 	mov	r1, #1	; 0x1
    66c8:	ebfffaa7 	bl	516c <vs_assign_by_id>
    66cc:	e3500000 	cmp	r0, #0	; 0x0
    66d0:	0a000005 	beq	66ec <burn_images+0x11c>
    66d4:	e59f00d0 	ldr	r0, [pc, #208]	; 67ac <burn_images+0x1dc>
    66d8:	e59f30b0 	ldr	r3, [pc, #176]	; 6790 <burn_images+0x1c0>
    66dc:	e1a0e00f 	mov	lr, pc
    66e0:	e593f000 	ldr	pc, [r3]
    66e4:	e3e04002 	mvn	r4, #2	; 0x2
    66e8:	ea000020 	b	6770 <burn_images+0x1a0>
    66ec:	e3a0e302 	mov	lr, #134217728	; 0x8000000
    66f0:	e1a0c826 	lsr	ip, r6, #16
    66f4:	e1a0c80c 	lsl	ip, ip, #16
    66f8:	e1a03004 	mov	r3, r4
    66fc:	e1a00007 	mov	r0, r7
    6700:	e59d100c 	ldr	r1, [sp, #12]
    6704:	e1a0200b 	mov	r2, fp
    6708:	e88d5000 	stm	sp, {ip, lr}
    670c:	ebfffeff 	bl	6310 <burn_image_loop>
    6710:	e3500000 	cmp	r0, #0	; 0x0
    6714:	aa000006 	bge	6734 <burn_images+0x164>
    6718:	e1a01008 	mov	r1, r8
    671c:	e59f008c 	ldr	r0, [pc, #140]	; 67b0 <burn_images+0x1e0>
    6720:	e59f3068 	ldr	r3, [pc, #104]	; 6790 <burn_images+0x1c0>
    6724:	e1a0e00f 	mov	lr, pc
    6728:	e593f000 	ldr	pc, [r3]
    672c:	e3e04000 	mvn	r4, #0	; 0x0
    6730:	ea00000e 	b	6770 <burn_images+0x1a0>
    6734:	e3550001 	cmp	r5, #1	; 0x1
    6738:	1a000003 	bne	674c <burn_images+0x17c>
    673c:	e5970004 	ldr	r0, [r7, #4]
    6740:	e3a01000 	mov	r1, #0	; 0x0
    6744:	ebfffe67 	bl	60e8 <cdbg_jump>
    6748:	ea000003 	b	675c <burn_images+0x18c>
    674c:	e3550000 	cmp	r5, #0	; 0x0
    6750:	1a000001 	bne	675c <burn_images+0x18c>
    6754:	e3a00302 	mov	r0, #134217728	; 0x8000000
    6758:	ebfffed7 	bl	62bc <cdbg_config_isi>
    675c:	e2888001 	add	r8, r8, #1	; 0x1
    6760:	e2877010 	add	r7, r7, #16	; 0x10
    6764:	e1580009 	cmp	r8, r9
    6768:	baffffa7 	blt	660c <burn_images+0x3c>
    676c:	e3a04000 	mov	r4, #0	; 0x0
    6770:	e3a00302 	mov	r0, #134217728	; 0x8000000
    6774:	ebffebee 	bl	1734 <free>
    6778:	e1a00004 	mov	r0, r4
    677c:	e28dd014 	add	sp, sp, #20	; 0x14
    6780:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6784:	e3a0e302 	mov	lr, #134217728	; 0x8000000
    6788:	e3a0400f 	mov	r4, #15	; 0xf
    678c:	eaffffd7 	b	66f0 <burn_images+0x120>
    6790:	0800d764 	.word	0x0800d764
    6794:	00013115 	.word	0x00013115
    6798:	00013132 	.word	0x00013132
    679c:	00013148 	.word	0x00013148
    67a0:	00013165 	.word	0x00013165
    67a4:	0001317a 	.word	0x0001317a
    67a8:	0001318d 	.word	0x0001318d
    67ac:	000131a1 	.word	0x000131a1
    67b0:	000131be 	.word	0x000131be

000067b4 <burn>:
    67b4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    67b8:	e1a09000 	mov	r9, r0
    67bc:	e24dd00c 	sub	sp, sp, #12	; 0xc
    67c0:	e3a00c02 	mov	r0, #512	; 0x200
    67c4:	ebffeb72 	bl	1594 <malloc>
    67c8:	e2507000 	subs	r7, r0, #0	; 0x0
    67cc:	1a000004 	bne	67e4 <burn+0x30>
    67d0:	e59f019c 	ldr	r0, [pc, #412]	; 6974 <burn+0x1c0>
    67d4:	e59f319c 	ldr	r3, [pc, #412]	; 6978 <burn+0x1c4>
    67d8:	e1a0e00f 	mov	lr, pc
    67dc:	e593f000 	ldr	pc, [r3]
    67e0:	ea000060 	b	6968 <burn+0x1b4>
    67e4:	ebffe77f 	bl	5e8 <boot_device>
    67e8:	e3a05000 	mov	r5, #0	; 0x0
    67ec:	e59f8184 	ldr	r8, [pc, #388]	; 6978 <burn+0x1c4>
    67f0:	e3a0bc02 	mov	fp, #512	; 0x200
    67f4:	e1a0a000 	mov	sl, r0
    67f8:	ea00004c 	b	6930 <burn+0x17c>
    67fc:	e1a00005 	mov	r0, r5
    6800:	ebfff92c 	bl	4cb8 <vs_device_burnsrc>
    6804:	e3500000 	cmp	r0, #0	; 0x0
    6808:	0a000047 	beq	692c <burn+0x178>
    680c:	e155000a 	cmp	r5, sl
    6810:	0a000045 	beq	692c <burn+0x178>
    6814:	e3a03001 	mov	r3, #1	; 0x1
    6818:	e0193513 	ands	r3, r9, r3, lsl r5
    681c:	1a000042 	bne	692c <burn+0x178>
    6820:	ebffe85d 	bl	99c <gtc_time>
    6824:	e1a00005 	mov	r0, r5
    6828:	e59f114c 	ldr	r1, [pc, #332]	; 697c <burn+0x1c8>
    682c:	ebfff98e 	bl	4e6c <vs_is_device>
    6830:	e3500000 	cmp	r0, #0	; 0x0
    6834:	1a000004 	bne	684c <burn+0x98>
    6838:	e1a00005 	mov	r0, r5
    683c:	e59f113c 	ldr	r1, [pc, #316]	; 6980 <burn+0x1cc>
    6840:	ebfff989 	bl	4e6c <vs_is_device>
    6844:	e3500000 	cmp	r0, #0	; 0x0
    6848:	0a00000c 	beq	6880 <burn+0xcc>
    684c:	ebffe8e4 	bl	be4 <assert_linkpc>
    6850:	e3500000 	cmp	r0, #0	; 0x0
    6854:	0a000009 	beq	6880 <burn+0xcc>
    6858:	eb0017d5 	bl	c7b4 <linkpc_loop>
    685c:	e2506000 	subs	r6, r0, #0	; 0x0
    6860:	0a000038 	beq	6948 <burn+0x194>
    6864:	e1a00005 	mov	r0, r5
    6868:	e5984000 	ldr	r4, [r8]
    686c:	ebfff947 	bl	4d90 <vs_device_name>
    6870:	e1a02006 	mov	r2, r6
    6874:	e1a01000 	mov	r1, r0
    6878:	e59f0104 	ldr	r0, [pc, #260]	; 6984 <burn+0x1d0>
    687c:	ea00000a 	b	68ac <burn+0xf8>
    6880:	e1a00005 	mov	r0, r5
    6884:	e3a01001 	mov	r1, #1	; 0x1
    6888:	ebfffa37 	bl	516c <vs_assign_by_id>
    688c:	e2506000 	subs	r6, r0, #0	; 0x0
    6890:	0a000007 	beq	68b4 <burn+0x100>
    6894:	e1a00005 	mov	r0, r5
    6898:	e5984000 	ldr	r4, [r8]
    689c:	ebfff93b 	bl	4d90 <vs_device_name>
    68a0:	e1a01000 	mov	r1, r0
    68a4:	e59f00dc 	ldr	r0, [pc, #220]	; 6988 <burn+0x1d4>
    68a8:	e1a02006 	mov	r2, r6
    68ac:	e12fff34 	blx	r4
    68b0:	ea00001d 	b	692c <burn+0x178>
    68b4:	e1a01007 	mov	r1, r7
    68b8:	e59f00cc 	ldr	r0, [pc, #204]	; 698c <burn+0x1d8>
    68bc:	e3a02401 	mov	r2, #16777216	; 0x1000000
    68c0:	e3a03c02 	mov	r3, #512	; 0x200
    68c4:	e58d6000 	str	r6, [sp]
    68c8:	e1a0e00f 	mov	lr, pc
    68cc:	e598f000 	ldr	pc, [r8]
    68d0:	e3a02401 	mov	r2, #16777216	; 0x1000000
    68d4:	e3a03000 	mov	r3, #0	; 0x0
    68d8:	e1a00007 	mov	r0, r7
    68dc:	e58d6004 	str	r6, [sp, #4]
    68e0:	e58db000 	str	fp, [sp]
    68e4:	ebfff979 	bl	4ed0 <vs_read>
    68e8:	e1a00007 	mov	r0, r7
    68ec:	eb001dc2 	bl	dffc <ius_check_header>
    68f0:	e2504000 	subs	r4, r0, #0	; 0x0
    68f4:	1a00000c 	bne	692c <burn+0x178>
    68f8:	e1a01005 	mov	r1, r5
    68fc:	e59f3074 	ldr	r3, [pc, #116]	; 6978 <burn+0x1c4>
    6900:	e59f0088 	ldr	r0, [pc, #136]	; 6990 <burn+0x1dc>
    6904:	e1a0e00f 	mov	lr, pc
    6908:	e593f000 	ldr	pc, [r3]
    690c:	e1a00004 	mov	r0, r4
    6910:	e1a01004 	mov	r1, r4
    6914:	ebfffde8 	bl	60bc <cdbg_verify_burn_enable>
    6918:	e1a00007 	mov	r0, r7
    691c:	e1a01005 	mov	r1, r5
    6920:	e1a02004 	mov	r2, r4
    6924:	ebffff29 	bl	65d0 <burn_images>
    6928:	ea000006 	b	6948 <burn+0x194>
    692c:	e2855001 	add	r5, r5, #1	; 0x1
    6930:	ebfff937 	bl	4e14 <vs_device_count>
    6934:	e1550000 	cmp	r5, r0
    6938:	baffffaf 	blt	67fc <burn+0x48>
    693c:	e1a00007 	mov	r0, r7
    6940:	ebffeb7b 	bl	1734 <free>
    6944:	ea000007 	b	6968 <burn+0x1b4>
    6948:	e59f0044 	ldr	r0, [pc, #68]	; 6994 <burn+0x1e0>
    694c:	e59f3024 	ldr	r3, [pc, #36]	; 6978 <burn+0x1c4>
    6950:	e1a0e00f 	mov	lr, pc
    6954:	e593f000 	ldr	pc, [r3]
    6958:	e1a00007 	mov	r0, r7
    695c:	ebffeb74 	bl	1734 <free>
    6960:	ebfffe12 	bl	61b0 <cdbg_shell>
    6964:	eafffffe 	b	6964 <burn+0x1b0>
    6968:	e3e00000 	mvn	r0, #0	; 0x0
    696c:	e28dd00c 	add	sp, sp, #12	; 0xc
    6970:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6974:	000131d6 	.word	0x000131d6
    6978:	0800d764 	.word	0x0800d764
    697c:	00012d80 	.word	0x00012d80
    6980:	00012d7c 	.word	0x00012d7c
    6984:	000131f6 	.word	0x000131f6
    6988:	00013218 	.word	0x00013218
    698c:	00013237 	.word	0x00013237
    6990:	00013267 	.word	0x00013267
    6994:	0001327f 	.word	0x0001327f

00006998 <mmc_send_cmd>:
    6998:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    699c:	e24dd004 	sub	sp, sp, #4	; 0x4
    69a0:	e1a0e00f 	mov	lr, pc
    69a4:	e590f104 	ldr	pc, [r0, #260]
    69a8:	e28dd004 	add	sp, sp, #4	; 0x4
    69ac:	e8bd8000 	pop	{pc}

000069b0 <mmc_set_blocklen>:
    69b0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    69b4:	e3a03015 	mov	r3, #21	; 0x15
    69b8:	e24dd024 	sub	sp, sp, #36	; 0x24
    69bc:	e3a02000 	mov	r2, #0	; 0x0
    69c0:	e58d3004 	str	r3, [sp, #4]
    69c4:	e3a03010 	mov	r3, #16	; 0x10
    69c8:	e58d1008 	str	r1, [sp, #8]
    69cc:	e1cd30b0 	strh	r3, [sp]
    69d0:	e58d201c 	str	r2, [sp, #28]
    69d4:	e1a0100d 	mov	r1, sp
    69d8:	e1a0e00f 	mov	lr, pc
    69dc:	e590f104 	ldr	pc, [r0, #260]
    69e0:	e28dd024 	add	sp, sp, #36	; 0x24
    69e4:	e8bd8000 	pop	{pc}

000069e8 <find_mmc_device>:
    69e8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    69ec:	e59f3048 	ldr	r3, [pc, #72]	; 6a3c <find_mmc_device+0x54>
    69f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    69f4:	e1a01000 	mov	r1, r0
    69f8:	e1a0c003 	mov	ip, r3
    69fc:	e5932000 	ldr	r2, [r3]
    6a00:	ea000004 	b	6a18 <find_mmc_device+0x30>
    6a04:	e592309c 	ldr	r3, [r2, #156]
    6a08:	e1a00002 	mov	r0, r2
    6a0c:	e1530001 	cmp	r3, r1
    6a10:	0a000007 	beq	6a34 <find_mmc_device+0x4c>
    6a14:	e5922000 	ldr	r2, [r2]
    6a18:	e152000c 	cmp	r2, ip
    6a1c:	1afffff8 	bne	6a04 <find_mmc_device+0x1c>
    6a20:	e59f0018 	ldr	r0, [pc, #24]	; 6a40 <find_mmc_device+0x58>
    6a24:	e59f3018 	ldr	r3, [pc, #24]	; 6a44 <find_mmc_device+0x5c>
    6a28:	e1a0e00f 	mov	lr, pc
    6a2c:	e593f000 	ldr	pc, [r3]
    6a30:	e3a00000 	mov	r0, #0	; 0x0
    6a34:	e28dd004 	add	sp, sp, #4	; 0x4
    6a38:	e8bd8000 	pop	{pc}
    6a3c:	0800d240 	.word	0x0800d240
    6a40:	000132a2 	.word	0x000132a2
    6a44:	0800d764 	.word	0x0800d764

00006a48 <mmc_send_ext_csd>:
    6a48:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6a4c:	e3a03015 	mov	r3, #21	; 0x15
    6a50:	e24dd034 	sub	sp, sp, #52	; 0x34
    6a54:	e58d3004 	str	r3, [sp, #4]
    6a58:	e3a03c02 	mov	r3, #512	; 0x200
    6a5c:	e3a02000 	mov	r2, #0	; 0x0
    6a60:	e3a0c001 	mov	ip, #1	; 0x1
    6a64:	e58d302c 	str	r3, [sp, #44]
    6a68:	e3a03008 	mov	r3, #8	; 0x8
    6a6c:	e58d1020 	str	r1, [sp, #32]
    6a70:	e58d201c 	str	r2, [sp, #28]
    6a74:	e58d2008 	str	r2, [sp, #8]
    6a78:	e58dc024 	str	ip, [sp, #36]
    6a7c:	e1cd30b0 	strh	r3, [sp]
    6a80:	e58dc028 	str	ip, [sp, #40]
    6a84:	e1a0100d 	mov	r1, sp
    6a88:	e28d2020 	add	r2, sp, #32	; 0x20
    6a8c:	e1a0e00f 	mov	lr, pc
    6a90:	e590f104 	ldr	pc, [r0, #260]
    6a94:	e28dd034 	add	sp, sp, #52	; 0x34
    6a98:	e8bd8000 	pop	{pc}

00006a9c <sd_switch>:
    6a9c:	e3e0c4ff 	mvn	ip, #-16777216	; 0xff000000
    6aa0:	e1a02102 	lsl	r2, r2, #2
    6aa4:	e18ccf81 	orr	ip, ip, r1, lsl #31
    6aa8:	e3a0100f 	mov	r1, #15	; 0xf
    6aac:	e1ccc211 	bic	ip, ip, r1, lsl r2
    6ab0:	e18cc213 	orr	ip, ip, r3, lsl r2
    6ab4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6ab8:	e3a03015 	mov	r3, #21	; 0x15
    6abc:	e24dd034 	sub	sp, sp, #52	; 0x34
    6ac0:	e58d3004 	str	r3, [sp, #4]
    6ac4:	e3a03000 	mov	r3, #0	; 0x0
    6ac8:	e58d301c 	str	r3, [sp, #28]
    6acc:	e59d3038 	ldr	r3, [sp, #56]
    6ad0:	e3a02001 	mov	r2, #1	; 0x1
    6ad4:	e58d3020 	str	r3, [sp, #32]
    6ad8:	e3a03040 	mov	r3, #64	; 0x40
    6adc:	e58d302c 	str	r3, [sp, #44]
    6ae0:	e3a03006 	mov	r3, #6	; 0x6
    6ae4:	e58d2024 	str	r2, [sp, #36]
    6ae8:	e58d2028 	str	r2, [sp, #40]
    6aec:	e58dc008 	str	ip, [sp, #8]
    6af0:	e1cd30b0 	strh	r3, [sp]
    6af4:	e1a0100d 	mov	r1, sp
    6af8:	e28d2020 	add	r2, sp, #32	; 0x20
    6afc:	e1a0e00f 	mov	lr, pc
    6b00:	e590f104 	ldr	pc, [r0, #260]
    6b04:	e28dd034 	add	sp, sp, #52	; 0x34
    6b08:	e8bd8000 	pop	{pc}

00006b0c <mmc_set_ios>:
    6b0c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6b10:	e24dd004 	sub	sp, sp, #4	; 0x4
    6b14:	e1a0e00f 	mov	lr, pc
    6b18:	e590f108 	ldr	pc, [r0, #264]
    6b1c:	e28dd004 	add	sp, sp, #4	; 0x4
    6b20:	e8bd8000 	pop	{pc}

00006b24 <mmc_set_clock>:
    6b24:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6b28:	e590203c 	ldr	r2, [r0, #60]
    6b2c:	e590c038 	ldr	ip, [r0, #56]
    6b30:	e1510002 	cmp	r1, r2
    6b34:	21a01002 	movcs	r1, r2
    6b38:	e24dd004 	sub	sp, sp, #4	; 0x4
    6b3c:	e151000c 	cmp	r1, ip
    6b40:	25801048 	strcs	r1, [r0, #72]
    6b44:	3580c048 	strcc	ip, [r0, #72]
    6b48:	e1a0e00f 	mov	lr, pc
    6b4c:	e590f108 	ldr	pc, [r0, #264]
    6b50:	e28dd004 	add	sp, sp, #4	; 0x4
    6b54:	e8bd8000 	pop	{pc}

00006b58 <mmc_set_bus_width>:
    6b58:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6b5c:	e5801044 	str	r1, [r0, #68]
    6b60:	e24dd004 	sub	sp, sp, #4	; 0x4
    6b64:	e1a0e00f 	mov	lr, pc
    6b68:	e590f108 	ldr	pc, [r0, #264]
    6b6c:	e28dd004 	add	sp, sp, #4	; 0x4
    6b70:	e8bd8000 	pop	{pc}

00006b74 <mmc_send_if_cond>:
    6b74:	e92d4010 	push	{r4, lr}
    6b78:	e590202c 	ldr	r2, [r0, #44]
    6b7c:	e59f3060 	ldr	r3, [pc, #96]	; 6be4 <mmc_send_if_cond+0x70>
    6b80:	e24dd020 	sub	sp, sp, #32	; 0x20
    6b84:	e0023003 	and	r3, r2, r3
    6b88:	e3530000 	cmp	r3, #0	; 0x0
    6b8c:	e28d1020 	add	r1, sp, #32	; 0x20
    6b90:	e3a03008 	mov	r3, #8	; 0x8
    6b94:	e300c1aa 	movw	ip, #426	; 0x1aa
    6b98:	e16132b0 	strh	r3, [r1, #-32]!
    6b9c:	03a0c0aa 	moveq	ip, #170	; 0xaa
    6ba0:	e3a02000 	mov	r2, #0	; 0x0
    6ba4:	e3a03015 	mov	r3, #21	; 0x15
    6ba8:	e98d1008 	stmib	sp, {r3, ip}
    6bac:	e58d201c 	str	r2, [sp, #28]
    6bb0:	e1a0100d 	mov	r1, sp
    6bb4:	e5903104 	ldr	r3, [r0, #260]
    6bb8:	e1a04000 	mov	r4, r0
    6bbc:	e12fff33 	blx	r3
    6bc0:	e3500000 	cmp	r0, #0	; 0x0
    6bc4:	1a000004 	bne	6bdc <mmc_send_if_cond+0x68>
    6bc8:	e5dd300c 	ldrb	r3, [sp, #12]
    6bcc:	e35300aa 	cmp	r3, #170	; 0xaa
    6bd0:	059f3010 	ldreq	r3, [pc, #16]	; 6be8 <mmc_send_if_cond+0x74>
    6bd4:	13e00010 	mvnne	r0, #16	; 0x10
    6bd8:	05843030 	streq	r3, [r4, #48]
    6bdc:	e28dd020 	add	sp, sp, #32	; 0x20
    6be0:	e8bd8010 	pop	{r4, pc}
    6be4:	00ff8000 	.word	0x00ff8000
    6be8:	00020020 	.word	0x00020020

00006bec <mmc_register>:
    6bec:	e5903110 	ldr	r3, [r0, #272]
    6bf0:	e59fc05c 	ldr	ip, [pc, #92]	; 6c54 <mmc_register+0x68>
    6bf4:	e3530000 	cmp	r3, #0	; 0x0
    6bf8:	e3a03006 	mov	r3, #6	; 0x6
    6bfc:	e59c2000 	ldr	r2, [ip]
    6c00:	e5803098 	str	r3, [r0, #152]
    6c04:	e59f304c 	ldr	r3, [pc, #76]	; 6c58 <mmc_register+0x6c>
    6c08:	e2821001 	add	r1, r2, #1	; 0x1
    6c0c:	e58030f8 	str	r3, [r0, #248]
    6c10:	e59f3044 	ldr	r3, [pc, #68]	; 6c5c <mmc_register+0x70>
    6c14:	e580209c 	str	r2, [r0, #156]
    6c18:	e58030fc 	str	r3, [r0, #252]
    6c1c:	e3a03001 	mov	r3, #1	; 0x1
    6c20:	e58c1000 	str	r1, [ip]
    6c24:	e5c030a4 	strb	r3, [r0, #164]
    6c28:	028330ff 	addeq	r3, r3, #255	; 0xff
    6c2c:	05803110 	streq	r3, [r0, #272]
    6c30:	e59f3028 	ldr	r3, [pc, #40]	; 6c60 <mmc_register+0x74>
    6c34:	e5800004 	str	r0, [r0, #4]
    6c38:	e5932004 	ldr	r2, [r3, #4]
    6c3c:	e5803000 	str	r3, [r0]
    6c40:	e5830004 	str	r0, [r3, #4]
    6c44:	e5820000 	str	r0, [r2]
    6c48:	e5802004 	str	r2, [r0, #4]
    6c4c:	e3a00000 	mov	r0, #0	; 0x0
    6c50:	e12fff1e 	bx	lr
    6c54:	0800d128 	.word	0x0800d128
    6c58:	00007e28 	.word	0x00007e28
    6c5c:	00007344 	.word	0x00007344
    6c60:	0800d240 	.word	0x0800d240

00006c64 <mmc_get_dev>:
    6c64:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6c68:	e24dd004 	sub	sp, sp, #4	; 0x4
    6c6c:	ebffff5d 	bl	69e8 <find_mmc_device>
    6c70:	e3500000 	cmp	r0, #0	; 0x0
    6c74:	12800098 	addne	r0, r0, #152	; 0x98
    6c78:	e28dd004 	add	sp, sp, #4	; 0x4
    6c7c:	e8bd8000 	pop	{pc}

00006c80 <print_mmc_devices>:
    6c80:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6c84:	e59f3068 	ldr	r3, [pc, #104]	; 6cf4 <print_mmc_devices+0x74>
    6c88:	e24dd004 	sub	sp, sp, #4	; 0x4
    6c8c:	e1a05000 	mov	r5, r0
    6c90:	e1a07003 	mov	r7, r3
    6c94:	e5934000 	ldr	r4, [r3]
    6c98:	e59f6058 	ldr	r6, [pc, #88]	; 6cf8 <print_mmc_devices+0x78>
    6c9c:	ea00000a 	b	6ccc <print_mmc_devices+0x4c>
    6ca0:	e594209c 	ldr	r2, [r4, #156]
    6ca4:	e1a0e00f 	mov	lr, pc
    6ca8:	e596f000 	ldr	pc, [r6]
    6cac:	e5943000 	ldr	r3, [r4]
    6cb0:	e59f0044 	ldr	r0, [pc, #68]	; 6cfc <print_mmc_devices+0x7c>
    6cb4:	e1530007 	cmp	r3, r7
    6cb8:	e1a01005 	mov	r1, r5
    6cbc:	0a000001 	beq	6cc8 <print_mmc_devices+0x48>
    6cc0:	e1a0e00f 	mov	lr, pc
    6cc4:	e596f000 	ldr	pc, [r6]
    6cc8:	e5944000 	ldr	r4, [r4]
    6ccc:	e1540007 	cmp	r4, r7
    6cd0:	e2841008 	add	r1, r4, #8	; 0x8
    6cd4:	e59f0024 	ldr	r0, [pc, #36]	; 6d00 <print_mmc_devices+0x80>
    6cd8:	1afffff0 	bne	6ca0 <print_mmc_devices+0x20>
    6cdc:	e59f0020 	ldr	r0, [pc, #32]	; 6d04 <print_mmc_devices+0x84>
    6ce0:	e59f3010 	ldr	r3, [pc, #16]	; 6cf8 <print_mmc_devices+0x78>
    6ce4:	e1a0e00f 	mov	lr, pc
    6ce8:	e593f000 	ldr	pc, [r3]
    6cec:	e28dd004 	add	sp, sp, #4	; 0x4
    6cf0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6cf4:	0800d240 	.word	0x0800d240
    6cf8:	0800d764 	.word	0x0800d764
    6cfc:	000132c2 	.word	0x000132c2
    6d00:	000132bb 	.word	0x000132bb
    6d04:	0001250e 	.word	0x0001250e

00006d08 <get_mmc_num>:
    6d08:	e59f3004 	ldr	r3, [pc, #4]	; 6d14 <get_mmc_num+0xc>
    6d0c:	e5930000 	ldr	r0, [r3]
    6d10:	e12fff1e 	bx	lr
    6d14:	0800d128 	.word	0x0800d128

00006d18 <mmc_vs_align>:
    6d18:	e3a00c02 	mov	r0, #512	; 0x200
    6d1c:	e12fff1e 	bx	lr

00006d20 <init_mmc>:
    6d20:	e59f3060 	ldr	r3, [pc, #96]	; 6d88 <init_mmc+0x68>
    6d24:	e59f2060 	ldr	r2, [pc, #96]	; 6d8c <init_mmc+0x6c>
    6d28:	e92d4070 	push	{r4, r5, r6, lr}
    6d2c:	e3a04000 	mov	r4, #0	; 0x0
    6d30:	e3a00b02 	mov	r0, #2048	; 0x800
    6d34:	e5833004 	str	r3, [r3, #4]
    6d38:	e5824000 	str	r4, [r2]
    6d3c:	e5833000 	str	r3, [r3]
    6d40:	ebffea13 	bl	1594 <malloc>
    6d44:	e1a05000 	mov	r5, r0
    6d48:	e2806b01 	add	r6, r0, #1024	; 0x400
    6d4c:	e1a00005 	mov	r0, r5
    6d50:	e3a01000 	mov	r1, #0	; 0x0
    6d54:	e3a02f4e 	mov	r2, #312	; 0x138
    6d58:	ebffe9a0 	bl	13e0 <memset>
    6d5c:	e5856130 	str	r6, [r5, #304]
    6d60:	e1a00004 	mov	r0, r4
    6d64:	e1a01005 	mov	r1, r5
    6d68:	eb000490 	bl	7fb0 <imap_sdhc_initialize>
    6d6c:	e3500000 	cmp	r0, #0	; 0x0
    6d70:	e2844001 	add	r4, r4, #1	; 0x1
    6d74:	e2855f4e 	add	r5, r5, #312	; 0x138
    6d78:	18bd8070 	popne	{r4, r5, r6, pc}
    6d7c:	e3540003 	cmp	r4, #3	; 0x3
    6d80:	1afffff1 	bne	6d4c <init_mmc+0x2c>
    6d84:	e8bd8070 	pop	{r4, r5, r6, pc}
    6d88:	0800d240 	.word	0x0800d240
    6d8c:	0800d128 	.word	0x0800d128

00006d90 <sd_change_freq>:
    6d90:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6d94:	e5903050 	ldr	r3, [r0, #80]
    6d98:	e3a01000 	mov	r1, #0	; 0x0
    6d9c:	e2132b01 	ands	r2, r3, #1024	; 0x400
    6da0:	e1a04000 	mov	r4, r0
    6da4:	e580104c 	str	r1, [r0, #76]
    6da8:	e24dd084 	sub	sp, sp, #132	; 0x84
    6dac:	11a00001 	movne	r0, r1
    6db0:	1a000070 	bne	6f78 <sd_change_freq+0x1e8>
    6db4:	e1d438b0 	ldrh	r3, [r4, #128]
    6db8:	e3a05015 	mov	r5, #21	; 0x15
    6dbc:	e1a03803 	lsl	r3, r3, #16
    6dc0:	e58d3050 	str	r3, [sp, #80]
    6dc4:	e28d6048 	add	r6, sp, #72	; 0x48
    6dc8:	e3a03037 	mov	r3, #55	; 0x37
    6dcc:	e1cd34b8 	strh	r3, [sp, #72]
    6dd0:	e58d504c 	str	r5, [sp, #76]
    6dd4:	e58d2064 	str	r2, [sp, #100]
    6dd8:	e1a01006 	mov	r1, r6
    6ddc:	e1a0e00f 	mov	lr, pc
    6de0:	e594f104 	ldr	pc, [r4, #260]
    6de4:	e3500000 	cmp	r0, #0	; 0x0
    6de8:	1a000062 	bne	6f78 <sd_change_freq+0x1e8>
    6dec:	e3a03033 	mov	r3, #51	; 0x33
    6df0:	e58d504c 	str	r5, [sp, #76]
    6df4:	e1a09006 	mov	r9, r6
    6df8:	e58d0064 	str	r0, [sp, #100]
    6dfc:	e1cd34b8 	strh	r3, [sp, #72]
    6e00:	e58d0050 	str	r0, [sp, #80]
    6e04:	e3a05003 	mov	r5, #3	; 0x3
    6e08:	e28da078 	add	sl, sp, #120	; 0x78
    6e0c:	e3a08008 	mov	r8, #8	; 0x8
    6e10:	e3a06001 	mov	r6, #1	; 0x1
    6e14:	e28d7068 	add	r7, sp, #104	; 0x68
    6e18:	e58da068 	str	sl, [sp, #104]
    6e1c:	e58d8074 	str	r8, [sp, #116]
    6e20:	e58d6070 	str	r6, [sp, #112]
    6e24:	e58d606c 	str	r6, [sp, #108]
    6e28:	e1a00004 	mov	r0, r4
    6e2c:	e1a01009 	mov	r1, r9
    6e30:	e1a02007 	mov	r2, r7
    6e34:	e1a0e00f 	mov	lr, pc
    6e38:	e594f104 	ldr	pc, [r4, #260]
    6e3c:	e250b000 	subs	fp, r0, #0	; 0x0
    6e40:	0a000003 	beq	6e54 <sd_change_freq+0xc4>
    6e44:	e3550000 	cmp	r5, #0	; 0x0
    6e48:	e2455001 	sub	r5, r5, #1	; 0x1
    6e4c:	1afffff1 	bne	6e18 <sd_change_freq+0x88>
    6e50:	ea000047 	b	6f74 <sd_change_freq+0x1e4>
    6e54:	e59d3078 	ldr	r3, [sp, #120]
    6e58:	e59d007c 	ldr	r0, [sp, #124]
    6e5c:	e1a01c03 	lsl	r1, r3, #24
    6e60:	e2032cff 	and	r2, r3, #65280	; 0xff00
    6e64:	e1811c23 	orr	r1, r1, r3, lsr #24
    6e68:	e1811402 	orr	r1, r1, r2, lsl #8
    6e6c:	e20338ff 	and	r3, r3, #16711680	; 0xff0000
    6e70:	e1811423 	orr	r1, r1, r3, lsr #8
    6e74:	e1a03c00 	lsl	r3, r0, #24
    6e78:	e1833c20 	orr	r3, r3, r0, lsr #24
    6e7c:	e2002cff 	and	r2, r0, #65280	; 0xff00
    6e80:	e1a0cc21 	lsr	ip, r1, #24
    6e84:	e1833402 	orr	r3, r3, r2, lsl #8
    6e88:	e20008ff 	and	r0, r0, #16711680	; 0xff0000
    6e8c:	e20cc00f 	and	ip, ip, #15	; 0xf
    6e90:	e1833420 	orr	r3, r3, r0, lsr #8
    6e94:	e35c0001 	cmp	ip, #1	; 0x1
    6e98:	e584305c 	str	r3, [r4, #92]
    6e9c:	e5841058 	str	r1, [r4, #88]
    6ea0:	059f30d8 	ldreq	r3, [pc, #216]	; 6f80 <sd_change_freq+0x1f0>
    6ea4:	0a000004 	beq	6ebc <sd_change_freq+0x12c>
    6ea8:	3a000002 	bcc	6eb8 <sd_change_freq+0x128>
    6eac:	e35c0002 	cmp	ip, #2	; 0x2
    6eb0:	059f30cc 	ldreq	r3, [pc, #204]	; 6f84 <sd_change_freq+0x1f4>
    6eb4:	0a000000 	beq	6ebc <sd_change_freq+0x12c>
    6eb8:	e59f30c8 	ldr	r3, [pc, #200]	; 6f88 <sd_change_freq+0x1f8>
    6ebc:	e5843030 	str	r3, [r4, #48]
    6ec0:	e5943058 	ldr	r3, [r4, #88]
    6ec4:	e5942030 	ldr	r2, [r4, #48]
    6ec8:	e3130701 	tst	r3, #262144	; 0x40000
    6ecc:	1594304c 	ldrne	r3, [r4, #76]
    6ed0:	13833c01 	orrne	r3, r3, #256	; 0x100
    6ed4:	1584304c 	strne	r3, [r4, #76]
    6ed8:	e59f30a8 	ldr	r3, [pc, #168]	; 6f88 <sd_change_freq+0x1f8>
    6edc:	e1520003 	cmp	r2, r3
    6ee0:	13a05004 	movne	r5, #4	; 0x4
    6ee4:	128d6008 	addne	r6, sp, #8	; 0x8
    6ee8:	1a000007 	bne	6f0c <sd_change_freq+0x17c>
    6eec:	ea000020 	b	6f74 <sd_change_freq+0x1e4>
    6ef0:	e58d6000 	str	r6, [sp]
    6ef4:	ebfffee8 	bl	6a9c <sd_switch>
    6ef8:	e3500000 	cmp	r0, #0	; 0x0
    6efc:	1a00001d 	bne	6f78 <sd_change_freq+0x1e8>
    6f00:	e59d3024 	ldr	r3, [sp, #36]
    6f04:	e3130c02 	tst	r3, #512	; 0x200
    6f08:	0a000005 	beq	6f24 <sd_change_freq+0x194>
    6f0c:	e3a01000 	mov	r1, #0	; 0x0
    6f10:	e2555001 	subs	r5, r5, #1	; 0x1
    6f14:	e1a00004 	mov	r0, r4
    6f18:	e1a02001 	mov	r2, r1
    6f1c:	e3a03001 	mov	r3, #1	; 0x1
    6f20:	2afffff2 	bcs	6ef0 <sd_change_freq+0x160>
    6f24:	e59d3014 	ldr	r3, [sp, #20]
    6f28:	e3130c02 	tst	r3, #512	; 0x200
    6f2c:	0a000010 	beq	6f74 <sd_change_freq+0x1e4>
    6f30:	e3a01001 	mov	r1, #1	; 0x1
    6f34:	e28dc008 	add	ip, sp, #8	; 0x8
    6f38:	e1a00004 	mov	r0, r4
    6f3c:	e3a02000 	mov	r2, #0	; 0x0
    6f40:	e1a03001 	mov	r3, r1
    6f44:	e58dc000 	str	ip, [sp]
    6f48:	ebfffed3 	bl	6a9c <sd_switch>
    6f4c:	e3500000 	cmp	r0, #0	; 0x0
    6f50:	1a000008 	bne	6f78 <sd_change_freq+0x1e8>
    6f54:	e59d3018 	ldr	r3, [sp, #24]
    6f58:	e1a03c03 	lsl	r3, r3, #24
    6f5c:	e203340f 	and	r3, r3, #251658240	; 0xf000000
    6f60:	e3530401 	cmp	r3, #16777216	; 0x1000000
    6f64:	0594304c 	ldreq	r3, [r4, #76]
    6f68:	03833001 	orreq	r3, r3, #1	; 0x1
    6f6c:	0584304c 	streq	r3, [r4, #76]
    6f70:	ea000000 	b	6f78 <sd_change_freq+0x1e8>
    6f74:	e1a0000b 	mov	r0, fp
    6f78:	e28dd084 	add	sp, sp, #132	; 0x84
    6f7c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6f80:	0002001a 	.word	0x0002001a
    6f84:	00020020 	.word	0x00020020
    6f88:	00020010 	.word	0x00020010

00006f8c <sd_send_op_cond>:
    6f8c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    6f90:	e59f9140 	ldr	r9, [pc, #320]	; 70d8 <sd_send_op_cond+0x14c>
    6f94:	e24dd020 	sub	sp, sp, #32	; 0x20
    6f98:	e1a04000 	mov	r4, r0
    6f9c:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    6fa0:	e3a0a015 	mov	sl, #21	; 0x15
    6fa4:	e3a06000 	mov	r6, #0	; 0x0
    6fa8:	e1a0700d 	mov	r7, sp
    6fac:	e3a08001 	mov	r8, #1	; 0x1
    6fb0:	e3a03037 	mov	r3, #55	; 0x37
    6fb4:	e1cd30b0 	strh	r3, [sp]
    6fb8:	e1a0100d 	mov	r1, sp
    6fbc:	e1a02006 	mov	r2, r6
    6fc0:	e58da004 	str	sl, [sp, #4]
    6fc4:	e58d6008 	str	r6, [sp, #8]
    6fc8:	e58d601c 	str	r6, [sp, #28]
    6fcc:	e1a00004 	mov	r0, r4
    6fd0:	e1a0e00f 	mov	lr, pc
    6fd4:	e594f104 	ldr	pc, [r4, #260]
    6fd8:	e2503000 	subs	r3, r0, #0	; 0x0
    6fdc:	e1a0100d 	mov	r1, sp
    6fe0:	e3a02000 	mov	r2, #0	; 0x0
    6fe4:	e59fe0f0 	ldr	lr, [pc, #240]	; 70dc <sd_send_op_cond+0x150>
    6fe8:	e1a00004 	mov	r0, r4
    6fec:	1a000036 	bne	70cc <sd_send_op_cond+0x140>
    6ff0:	e594302c 	ldr	r3, [r4, #44]
    6ff4:	e594c030 	ldr	ip, [r4, #48]
    6ff8:	e003e00e 	and	lr, r3, lr
    6ffc:	e15c0009 	cmp	ip, r9
    7000:	038e3101 	orreq	r3, lr, #1073741824	; 0x40000000
    7004:	e58de008 	str	lr, [sp, #8]
    7008:	058d3008 	streq	r3, [sp, #8]
    700c:	e3a03029 	mov	r3, #41	; 0x29
    7010:	e1cd30b0 	strh	r3, [sp]
    7014:	e58d8004 	str	r8, [sp, #4]
    7018:	e5943104 	ldr	r3, [r4, #260]
    701c:	e12fff33 	blx	r3
    7020:	e2503000 	subs	r3, r0, #0	; 0x0
    7024:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    7028:	1a000027 	bne	70cc <sd_send_op_cond+0x140>
    702c:	eb000636 	bl	890c <udelay>
    7030:	e59d300c 	ldr	r3, [sp, #12]
    7034:	e3530000 	cmp	r3, #0	; 0x0
    7038:	ba000002 	blt	7048 <sd_send_op_cond+0xbc>
    703c:	e2555001 	subs	r5, r5, #1	; 0x1
    7040:	2affffda 	bcs	6fb0 <sd_send_op_cond+0x24>
    7044:	ea00001f 	b	70c8 <sd_send_op_cond+0x13c>
    7048:	e3550000 	cmp	r5, #0	; 0x0
    704c:	da00001d 	ble	70c8 <sd_send_op_cond+0x13c>
    7050:	e59f3080 	ldr	r3, [pc, #128]	; 70d8 <sd_send_op_cond+0x14c>
    7054:	e5942030 	ldr	r2, [r4, #48]
    7058:	e1520003 	cmp	r2, r3
    705c:	12433010 	subne	r3, r3, #16	; 0x10
    7060:	15843030 	strne	r3, [r4, #48]
    7064:	e5943050 	ldr	r3, [r4, #80]
    7068:	e3130b01 	tst	r3, #1024	; 0x400
    706c:	0a00000c 	beq	70a4 <sd_send_op_cond+0x118>
    7070:	e3a03001 	mov	r3, #1	; 0x1
    7074:	e3a02000 	mov	r2, #0	; 0x0
    7078:	e58d3004 	str	r3, [sp, #4]
    707c:	e3a0303a 	mov	r3, #58	; 0x3a
    7080:	e1cd30b0 	strh	r3, [sp]
    7084:	e58d2008 	str	r2, [sp, #8]
    7088:	e58d201c 	str	r2, [sp, #28]
    708c:	e1a00004 	mov	r0, r4
    7090:	e1a0100d 	mov	r1, sp
    7094:	e1a0e00f 	mov	lr, pc
    7098:	e594f104 	ldr	pc, [r4, #260]
    709c:	e2503000 	subs	r3, r0, #0	; 0x0
    70a0:	1a000009 	bne	70cc <sd_send_op_cond+0x140>
    70a4:	e59d200c 	ldr	r2, [sp, #12]
    70a8:	e3a03000 	mov	r3, #0	; 0x0
    70ac:	e1c438b0 	strh	r3, [r4, #128]
    70b0:	e1a03f22 	lsr	r3, r2, #30
    70b4:	e2033001 	and	r3, r3, #1	; 0x1
    70b8:	e5843040 	str	r3, [r4, #64]
    70bc:	e5842054 	str	r2, [r4, #84]
    70c0:	e3a03000 	mov	r3, #0	; 0x0
    70c4:	ea000000 	b	70cc <sd_send_op_cond+0x140>
    70c8:	e3e03010 	mvn	r3, #16	; 0x10
    70cc:	e1a00003 	mov	r0, r3
    70d0:	e28dd020 	add	sp, sp, #32	; 0x20
    70d4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    70d8:	00020020 	.word	0x00020020
    70dc:	00ff8000 	.word	0x00ff8000

000070e0 <mmc_go_idle>:
    70e0:	e92d4010 	push	{r4, lr}
    70e4:	e1a04000 	mov	r4, r0
    70e8:	e24dd020 	sub	sp, sp, #32	; 0x20
    70ec:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    70f0:	eb000605 	bl	890c <udelay>
    70f4:	e3a02000 	mov	r2, #0	; 0x0
    70f8:	e1a00004 	mov	r0, r4
    70fc:	e1cd20b0 	strh	r2, [sp]
    7100:	e58d2008 	str	r2, [sp, #8]
    7104:	e58d2004 	str	r2, [sp, #4]
    7108:	e58d201c 	str	r2, [sp, #28]
    710c:	e1a0100d 	mov	r1, sp
    7110:	e1a0e00f 	mov	lr, pc
    7114:	e594f104 	ldr	pc, [r4, #260]
    7118:	e2504000 	subs	r4, r0, #0	; 0x0
    711c:	1a000001 	bne	7128 <mmc_go_idle+0x48>
    7120:	e2800e7d 	add	r0, r0, #2000	; 0x7d0
    7124:	eb0005f8 	bl	890c <udelay>
    7128:	e1a00004 	mov	r0, r4
    712c:	e28dd020 	add	sp, sp, #32	; 0x20
    7130:	e8bd8010 	pop	{r4, pc}

00007134 <mmc_send_op_cond>:
    7134:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7138:	e24dd020 	sub	sp, sp, #32	; 0x20
    713c:	e1a05000 	mov	r5, r0
    7140:	e3a06001 	mov	r6, #1	; 0x1
    7144:	ebffffe5 	bl	70e0 <mmc_go_idle>
    7148:	e3a02000 	mov	r2, #0	; 0x0
    714c:	e1cd60b0 	strh	r6, [sp]
    7150:	e58d6004 	str	r6, [sp, #4]
    7154:	e58d2008 	str	r2, [sp, #8]
    7158:	e58d201c 	str	r2, [sp, #28]
    715c:	e1a00005 	mov	r0, r5
    7160:	e1a0100d 	mov	r1, sp
    7164:	e1a0e00f 	mov	lr, pc
    7168:	e595f104 	ldr	pc, [r5, #260]
    716c:	e2504000 	subs	r4, r0, #0	; 0x0
    7170:	e1a0700d 	mov	r7, sp
    7174:	1a000039 	bne	7260 <mmc_send_op_cond+0x12c>
    7178:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    717c:	eb0005e2 	bl	890c <udelay>
    7180:	e1a08006 	mov	r8, r6
    7184:	e1a0700d 	mov	r7, sp
    7188:	e1a06004 	mov	r6, r4
    718c:	e3024710 	movw	r4, #10000	; 0x2710
    7190:	e595302c 	ldr	r3, [r5, #44]
    7194:	e3a02001 	mov	r2, #1	; 0x1
    7198:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    719c:	e1cd20b0 	strh	r2, [sp]
    71a0:	e58d3008 	str	r3, [sp, #8]
    71a4:	e1a02006 	mov	r2, r6
    71a8:	e58d8004 	str	r8, [sp, #4]
    71ac:	e58d601c 	str	r6, [sp, #28]
    71b0:	e1a0100d 	mov	r1, sp
    71b4:	e1a00005 	mov	r0, r5
    71b8:	e1a0e00f 	mov	lr, pc
    71bc:	e595f104 	ldr	pc, [r5, #260]
    71c0:	e2502000 	subs	r2, r0, #0	; 0x0
    71c4:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    71c8:	11a04002 	movne	r4, r2
    71cc:	1a000023 	bne	7260 <mmc_send_op_cond+0x12c>
    71d0:	eb0005cd 	bl	890c <udelay>
    71d4:	e59d300c 	ldr	r3, [sp, #12]
    71d8:	e3530000 	cmp	r3, #0	; 0x0
    71dc:	ba000002 	blt	71ec <mmc_send_op_cond+0xb8>
    71e0:	e2544001 	subs	r4, r4, #1	; 0x1
    71e4:	2affffe9 	bcs	7190 <mmc_send_op_cond+0x5c>
    71e8:	ea00001b 	b	725c <mmc_send_op_cond+0x128>
    71ec:	e3540000 	cmp	r4, #0	; 0x0
    71f0:	da000019 	ble	725c <mmc_send_op_cond+0x128>
    71f4:	e5953050 	ldr	r3, [r5, #80]
    71f8:	e3130b01 	tst	r3, #1024	; 0x400
    71fc:	0a00000b 	beq	7230 <mmc_send_op_cond+0xfc>
    7200:	e3a0303a 	mov	r3, #58	; 0x3a
    7204:	e58d8004 	str	r8, [sp, #4]
    7208:	e1cd30b0 	strh	r3, [sp]
    720c:	e58d6008 	str	r6, [sp, #8]
    7210:	e58d601c 	str	r6, [sp, #28]
    7214:	e1a0100d 	mov	r1, sp
    7218:	e1a02006 	mov	r2, r6
    721c:	e1a00005 	mov	r0, r5
    7220:	e1a0e00f 	mov	lr, pc
    7224:	e595f104 	ldr	pc, [r5, #260]
    7228:	e2504000 	subs	r4, r0, #0	; 0x0
    722c:	1a00000b 	bne	7260 <mmc_send_op_cond+0x12c>
    7230:	e59d300c 	ldr	r3, [sp, #12]
    7234:	e3a01801 	mov	r1, #65536	; 0x10000
    7238:	e1a02f23 	lsr	r2, r3, #30
    723c:	e2022001 	and	r2, r2, #1	; 0x1
    7240:	e5852040 	str	r2, [r5, #64]
    7244:	e3a02000 	mov	r2, #0	; 0x0
    7248:	e5851030 	str	r1, [r5, #48]
    724c:	e1c528b0 	strh	r2, [r5, #128]
    7250:	e5853054 	str	r3, [r5, #84]
    7254:	e3a04000 	mov	r4, #0	; 0x0
    7258:	ea000000 	b	7260 <mmc_send_op_cond+0x12c>
    725c:	e3e04010 	mvn	r4, #16	; 0x10
    7260:	e1a00004 	mov	r0, r4
    7264:	e28dd020 	add	sp, sp, #32	; 0x20
    7268:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000726c <mmc_send_status>:
    726c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    7270:	e1d038b0 	ldrh	r3, [r0, #128]
    7274:	e24dd024 	sub	sp, sp, #36	; 0x24
    7278:	e1a03803 	lsl	r3, r3, #16
    727c:	e58d3008 	str	r3, [sp, #8]
    7280:	e3a03000 	mov	r3, #0	; 0x0
    7284:	e3a02015 	mov	r2, #21	; 0x15
    7288:	e58d301c 	str	r3, [sp, #28]
    728c:	e3a0300d 	mov	r3, #13	; 0xd
    7290:	e1a06000 	mov	r6, r0
    7294:	e58d2004 	str	r2, [sp, #4]
    7298:	e1a04001 	mov	r4, r1
    729c:	e1cd30b0 	strh	r3, [sp]
    72a0:	e1a0700d 	mov	r7, sp
    72a4:	e1a0100d 	mov	r1, sp
    72a8:	e3a02000 	mov	r2, #0	; 0x0
    72ac:	e1a00006 	mov	r0, r6
    72b0:	e1a0e00f 	mov	lr, pc
    72b4:	e596f104 	ldr	pc, [r6, #260]
    72b8:	e2505000 	subs	r5, r0, #0	; 0x0
    72bc:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    72c0:	1a000018 	bne	7328 <mmc_send_status+0xbc>
    72c4:	e59d300c 	ldr	r3, [sp, #12]
    72c8:	e3130c01 	tst	r3, #256	; 0x100
    72cc:	1a00000e 	bne	730c <mmc_send_status+0xa0>
    72d0:	eb00058d 	bl	890c <udelay>
    72d4:	e59d100c 	ldr	r1, [sp, #12]
    72d8:	e59f3054 	ldr	r3, [pc, #84]	; 7334 <mmc_send_status+0xc8>
    72dc:	e2444001 	sub	r4, r4, #1	; 0x1
    72e0:	e0013003 	and	r3, r1, r3
    72e4:	e3530000 	cmp	r3, #0	; 0x0
    72e8:	0a000005 	beq	7304 <mmc_send_status+0x98>
    72ec:	e59f0044 	ldr	r0, [pc, #68]	; 7338 <mmc_send_status+0xcc>
    72f0:	e59f3044 	ldr	r3, [pc, #68]	; 733c <mmc_send_status+0xd0>
    72f4:	e1a0e00f 	mov	lr, pc
    72f8:	e593f000 	ldr	pc, [r3]
    72fc:	e3e05011 	mvn	r5, #17	; 0x11
    7300:	ea000008 	b	7328 <mmc_send_status+0xbc>
    7304:	e3740001 	cmn	r4, #1	; 0x1
    7308:	1affffe5 	bne	72a4 <mmc_send_status+0x38>
    730c:	e3540000 	cmp	r4, #0	; 0x0
    7310:	1a000004 	bne	7328 <mmc_send_status+0xbc>
    7314:	e59f0024 	ldr	r0, [pc, #36]	; 7340 <mmc_send_status+0xd4>
    7318:	e59f301c 	ldr	r3, [pc, #28]	; 733c <mmc_send_status+0xd0>
    731c:	e1a0e00f 	mov	lr, pc
    7320:	e593f000 	ldr	pc, [r3]
    7324:	e3e05012 	mvn	r5, #18	; 0x12
    7328:	e1a00005 	mov	r0, r5
    732c:	e28dd024 	add	sp, sp, #36	; 0x24
    7330:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    7334:	fdf94080 	.word	0xfdf94080
    7338:	000132c6 	.word	0x000132c6
    733c:	0800d764 	.word	0x0800d764
    7340:	000132dc 	.word	0x000132dc

00007344 <mmc_bwrite>:
    7344:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7348:	e24dd034 	sub	sp, sp, #52	; 0x34
    734c:	e1a06001 	mov	r6, r1
    7350:	e1a0b002 	mov	fp, r2
    7354:	e1a0a003 	mov	sl, r3
    7358:	ebfffda2 	bl	69e8 <find_mmc_device>
    735c:	e2504000 	subs	r4, r0, #0	; 0x0
    7360:	0a000060 	beq	74e8 <mmc_bwrite+0x1a4>
    7364:	e594108c 	ldr	r1, [r4, #140]
    7368:	ebfffd90 	bl	69b0 <mmc_set_blocklen>
    736c:	e3500000 	cmp	r0, #0	; 0x0
    7370:	01a09000 	moveq	r9, r0
    7374:	01a0700b 	moveq	r7, fp
    7378:	0a000005 	beq	7394 <mmc_bwrite+0x50>
    737c:	e594108c 	ldr	r1, [r4, #140]
    7380:	e59f016c 	ldr	r0, [pc, #364]	; 74f4 <mmc_bwrite+0x1b0>
    7384:	e59f316c 	ldr	r3, [pc, #364]	; 74f8 <mmc_bwrite+0x1b4>
    7388:	e1a0e00f 	mov	lr, pc
    738c:	e593f000 	ldr	pc, [r3]
    7390:	ea000054 	b	74e8 <mmc_bwrite+0x1a4>
    7394:	e5943110 	ldr	r3, [r4, #272]
    7398:	e59420a8 	ldr	r2, [r4, #168]
    739c:	e1570003 	cmp	r7, r3
    73a0:	31a05007 	movcc	r5, r7
    73a4:	21a05003 	movcs	r5, r3
    73a8:	e0858006 	add	r8, r5, r6
    73ac:	e1580002 	cmp	r8, r2
    73b0:	9a000005 	bls	73cc <mmc_bwrite+0x88>
    73b4:	e59f0140 	ldr	r0, [pc, #320]	; 74fc <mmc_bwrite+0x1b8>
    73b8:	e1a01008 	mov	r1, r8
    73bc:	e59f3134 	ldr	r3, [pc, #308]	; 74f8 <mmc_bwrite+0x1b4>
    73c0:	e1a0e00f 	mov	lr, pc
    73c4:	e593f000 	ldr	pc, [r3]
    73c8:	ea00003d 	b	74c4 <mmc_bwrite+0x180>
    73cc:	e3550001 	cmp	r5, #1	; 0x1
    73d0:	93a03018 	movls	r3, #24	; 0x18
    73d4:	91cd30b0 	strhls	r3, [sp]
    73d8:	e5943040 	ldr	r3, [r4, #64]
    73dc:	83a02019 	movhi	r2, #25	; 0x19
    73e0:	81cd20b0 	strhhi	r2, [sp]
    73e4:	e3530000 	cmp	r3, #0	; 0x0
    73e8:	0594308c 	ldreq	r3, [r4, #140]
    73ec:	e3a02015 	mov	r2, #21	; 0x15
    73f0:	00030693 	muleq	r3, r3, r6
    73f4:	158d6008 	strne	r6, [sp, #8]
    73f8:	058d3008 	streq	r3, [sp, #8]
    73fc:	e58d2004 	str	r2, [sp, #4]
    7400:	e58d901c 	str	r9, [sp, #28]
    7404:	e58da020 	str	sl, [sp, #32]
    7408:	e58d5028 	str	r5, [sp, #40]
    740c:	e594308c 	ldr	r3, [r4, #140]
    7410:	e3a02002 	mov	r2, #2	; 0x2
    7414:	e58d2024 	str	r2, [sp, #36]
    7418:	e58d302c 	str	r3, [sp, #44]
    741c:	e1a00004 	mov	r0, r4
    7420:	e1a0100d 	mov	r1, sp
    7424:	e28d2020 	add	r2, sp, #32	; 0x20
    7428:	e1a0e00f 	mov	lr, pc
    742c:	e594f104 	ldr	pc, [r4, #260]
    7430:	e3500000 	cmp	r0, #0	; 0x0
    7434:	0a000004 	beq	744c <mmc_bwrite+0x108>
    7438:	e59f00c0 	ldr	r0, [pc, #192]	; 7500 <mmc_bwrite+0x1bc>
    743c:	e59f30b4 	ldr	r3, [pc, #180]	; 74f8 <mmc_bwrite+0x1b4>
    7440:	e1a0e00f 	mov	lr, pc
    7444:	e593f000 	ldr	pc, [r3]
    7448:	ea00001d 	b	74c4 <mmc_bwrite+0x180>
    744c:	e5943050 	ldr	r3, [r4, #80]
    7450:	e1a03523 	lsr	r3, r3, #10
    7454:	e2233001 	eor	r3, r3, #1	; 0x1
    7458:	e3550001 	cmp	r5, #1	; 0x1
    745c:	93a03000 	movls	r3, #0	; 0x0
    7460:	82033001 	andhi	r3, r3, #1	; 0x1
    7464:	e3530000 	cmp	r3, #0	; 0x0
    7468:	0a000011 	beq	74b4 <mmc_bwrite+0x170>
    746c:	e3a0200c 	mov	r2, #12	; 0xc
    7470:	e3a0301d 	mov	r3, #29	; 0x1d
    7474:	e1cd20b0 	strh	r2, [sp]
    7478:	e98d0208 	stmib	sp, {r3, r9}
    747c:	e58d901c 	str	r9, [sp, #28]
    7480:	e1a00004 	mov	r0, r4
    7484:	e1a0100d 	mov	r1, sp
    7488:	e1a02009 	mov	r2, r9
    748c:	e1a0e00f 	mov	lr, pc
    7490:	e594f104 	ldr	pc, [r4, #260]
    7494:	e3500000 	cmp	r0, #0	; 0x0
    7498:	0a000005 	beq	74b4 <mmc_bwrite+0x170>
    749c:	e59f0060 	ldr	r0, [pc, #96]	; 7504 <mmc_bwrite+0x1c0>
    74a0:	e59f3050 	ldr	r3, [pc, #80]	; 74f8 <mmc_bwrite+0x1b4>
    74a4:	e1a0e00f 	mov	lr, pc
    74a8:	e593f000 	ldr	pc, [r3]
    74ac:	e1a00009 	mov	r0, r9
    74b0:	ea00000d 	b	74ec <mmc_bwrite+0x1a8>
    74b4:	e1a00004 	mov	r0, r4
    74b8:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    74bc:	ebffff6a 	bl	726c <mmc_send_status>
    74c0:	ea000001 	b	74cc <mmc_bwrite+0x188>
    74c4:	e3550000 	cmp	r5, #0	; 0x0
    74c8:	1a000006 	bne	74e8 <mmc_bwrite+0x1a4>
    74cc:	e0577005 	subs	r7, r7, r5
    74d0:	e594008c 	ldr	r0, [r4, #140]
    74d4:	102aa590 	mlane	sl, r0, r5, sl
    74d8:	11a06008 	movne	r6, r8
    74dc:	1affffac 	bne	7394 <mmc_bwrite+0x50>
    74e0:	e000009b 	mul	r0, fp, r0
    74e4:	ea000000 	b	74ec <mmc_bwrite+0x1a8>
    74e8:	e3a00000 	mov	r0, #0	; 0x0
    74ec:	e28dd034 	add	sp, sp, #52	; 0x34
    74f0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    74f4:	000132f8 	.word	0x000132f8
    74f8:	0800d764 	.word	0x0800d764
    74fc:	00013302 	.word	0x00013302
    7500:	0001332e 	.word	0x0001332e
    7504:	00013340 	.word	0x00013340

00007508 <mmc2_vs_write>:
    7508:	e1a024a2 	lsr	r2, r2, #9
    750c:	e1821b83 	orr	r1, r2, r3, lsl #23
    7510:	e59d2000 	ldr	r2, [sp]
    7514:	e1a03000 	mov	r3, r0
    7518:	e1a024c2 	asr	r2, r2, #9
    751c:	e3a00002 	mov	r0, #2	; 0x2
    7520:	eaffff87 	b	7344 <mmc_bwrite>

00007524 <mmc1_vs_write>:
    7524:	e1a024a2 	lsr	r2, r2, #9
    7528:	e1821b83 	orr	r1, r2, r3, lsl #23
    752c:	e59d2000 	ldr	r2, [sp]
    7530:	e1a03000 	mov	r3, r0
    7534:	e1a024c2 	asr	r2, r2, #9
    7538:	e3a00001 	mov	r0, #1	; 0x1
    753c:	eaffff80 	b	7344 <mmc_bwrite>

00007540 <mmc0_vs_write>:
    7540:	e1a024a2 	lsr	r2, r2, #9
    7544:	e1821b83 	orr	r1, r2, r3, lsl #23
    7548:	e59d2000 	ldr	r2, [sp]
    754c:	e1a03000 	mov	r3, r0
    7550:	e1a024c2 	asr	r2, r2, #9
    7554:	e3a00000 	mov	r0, #0	; 0x0
    7558:	eaffff79 	b	7344 <mmc_bwrite>

0000755c <mmc_switch>:
    755c:	e92d4030 	push	{r4, r5, lr}
    7560:	e1a02802 	lsl	r2, r2, #16
    7564:	e3822403 	orr	r2, r2, #50331648	; 0x3000000
    7568:	e24dd024 	sub	sp, sp, #36	; 0x24
    756c:	e1822403 	orr	r2, r2, r3, lsl #8
    7570:	e3a0301d 	mov	r3, #29	; 0x1d
    7574:	e3a01000 	mov	r1, #0	; 0x0
    7578:	e58d3004 	str	r3, [sp, #4]
    757c:	e3a03006 	mov	r3, #6	; 0x6
    7580:	e58d2008 	str	r2, [sp, #8]
    7584:	e1cd30b0 	strh	r3, [sp]
    7588:	e1a02001 	mov	r2, r1
    758c:	e58d101c 	str	r1, [sp, #28]
    7590:	e1a0100d 	mov	r1, sp
    7594:	e1a04000 	mov	r4, r0
    7598:	e1a0e00f 	mov	lr, pc
    759c:	e590f104 	ldr	pc, [r0, #260]
    75a0:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    75a4:	e1a05000 	mov	r5, r0
    75a8:	e1a00004 	mov	r0, r4
    75ac:	ebffff2e 	bl	726c <mmc_send_status>
    75b0:	e1a00005 	mov	r0, r5
    75b4:	e28dd024 	add	sp, sp, #36	; 0x24
    75b8:	e8bd8030 	pop	{r4, r5, pc}

000075bc <mmc_change_freq>:
    75bc:	e92d4070 	push	{r4, r5, r6, lr}
    75c0:	e5903050 	ldr	r3, [r0, #80]
    75c4:	e24ddc02 	sub	sp, sp, #512	; 0x200
    75c8:	e3130b01 	tst	r3, #1024	; 0x400
    75cc:	e3a03000 	mov	r3, #0	; 0x0
    75d0:	e1a04000 	mov	r4, r0
    75d4:	e580304c 	str	r3, [r0, #76]
    75d8:	1a000020 	bne	7660 <mmc_change_freq+0xa4>
    75dc:	e5902030 	ldr	r2, [r0, #48]
    75e0:	e59f3084 	ldr	r3, [pc, #132]	; 766c <mmc_change_freq+0xb0>
    75e4:	e1520003 	cmp	r2, r3
    75e8:	9a00001c 	bls	7660 <mmc_change_freq+0xa4>
    75ec:	e3a03c01 	mov	r3, #256	; 0x100
    75f0:	e580304c 	str	r3, [r0, #76]
    75f4:	e1a0100d 	mov	r1, sp
    75f8:	ebfffd12 	bl	6a48 <mmc_send_ext_csd>
    75fc:	e3500000 	cmp	r0, #0	; 0x0
    7600:	e1a0600d 	mov	r6, sp
    7604:	1a000016 	bne	7664 <mmc_change_freq+0xa8>
    7608:	e3a01001 	mov	r1, #1	; 0x1
    760c:	e1a00004 	mov	r0, r4
    7610:	e3a020b9 	mov	r2, #185	; 0xb9
    7614:	e1a03001 	mov	r3, r1
    7618:	e5dd50c4 	ldrb	r5, [sp, #196]
    761c:	ebffffce 	bl	755c <mmc_switch>
    7620:	e3500000 	cmp	r0, #0	; 0x0
    7624:	1a00000e 	bne	7664 <mmc_change_freq+0xa8>
    7628:	e1a0100d 	mov	r1, sp
    762c:	e1a00004 	mov	r0, r4
    7630:	ebfffd04 	bl	6a48 <mmc_send_ext_csd>
    7634:	e3500000 	cmp	r0, #0	; 0x0
    7638:	1a000009 	bne	7664 <mmc_change_freq+0xa8>
    763c:	e5dd30b9 	ldrb	r3, [sp, #185]
    7640:	e3530000 	cmp	r3, #0	; 0x0
    7644:	0a000006 	beq	7664 <mmc_change_freq+0xa8>
    7648:	e594304c 	ldr	r3, [r4, #76]
    764c:	e3150002 	tst	r5, #2	; 0x2
    7650:	13833011 	orrne	r3, r3, #17	; 0x11
    7654:	03833001 	orreq	r3, r3, #1	; 0x1
    7658:	e584304c 	str	r3, [r4, #76]
    765c:	ea000000 	b	7664 <mmc_change_freq+0xa8>
    7660:	e3a00000 	mov	r0, #0	; 0x0
    7664:	e28ddc02 	add	sp, sp, #512	; 0x200
    7668:	e8bd8070 	pop	{r4, r5, r6, pc}
    766c:	0001003f 	.word	0x0001003f

00007670 <mmc_startup>:
    7670:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    7674:	e5903050 	ldr	r3, [r0, #80]
    7678:	e24ddf8e 	sub	sp, sp, #568	; 0x238
    767c:	e3130b01 	tst	r3, #1024	; 0x400
    7680:	13a0300a 	movne	r3, #10	; 0xa
    7684:	03a03002 	moveq	r3, #2	; 0x2
    7688:	e28d1c02 	add	r1, sp, #512	; 0x200
    768c:	e3a02000 	mov	r2, #0	; 0x0
    7690:	e1c131b0 	strh	r3, [r1, #16]
    7694:	e28d5e21 	add	r5, sp, #528	; 0x210
    7698:	e3a03007 	mov	r3, #7	; 0x7
    769c:	e58d3214 	str	r3, [sp, #532]
    76a0:	e58d2218 	str	r2, [sp, #536]
    76a4:	e58d222c 	str	r2, [sp, #556]
    76a8:	e1a01005 	mov	r1, r5
    76ac:	e1a06000 	mov	r6, r0
    76b0:	e1a0e00f 	mov	lr, pc
    76b4:	e590f104 	ldr	pc, [r0, #260]
    76b8:	e2504000 	subs	r4, r0, #0	; 0x0
    76bc:	1a000159 	bne	7c28 <mmc_startup+0x5b8>
    76c0:	e3a02010 	mov	r2, #16	; 0x10
    76c4:	e2860070 	add	r0, r6, #112	; 0x70
    76c8:	e285100c 	add	r1, r5, #12	; 0xc
    76cc:	ebffe74b 	bl	1400 <memcpy>
    76d0:	e5963050 	ldr	r3, [r6, #80]
    76d4:	e2132b01 	ands	r2, r3, #1024	; 0x400
    76d8:	1a000013 	bne	772c <mmc_startup+0xbc>
    76dc:	e1d638b0 	ldrh	r3, [r6, #128]
    76e0:	e3a00003 	mov	r0, #3	; 0x3
    76e4:	e1a03803 	lsl	r3, r3, #16
    76e8:	e58d3218 	str	r3, [sp, #536]
    76ec:	e3a03015 	mov	r3, #21	; 0x15
    76f0:	e58d3214 	str	r3, [sp, #532]
    76f4:	e28d3c02 	add	r3, sp, #512	; 0x200
    76f8:	e1c301b0 	strh	r0, [r3, #16]
    76fc:	e58d222c 	str	r2, [sp, #556]
    7700:	e1a01005 	mov	r1, r5
    7704:	e1a00006 	mov	r0, r6
    7708:	e1a0e00f 	mov	lr, pc
    770c:	e596f104 	ldr	pc, [r6, #260]
    7710:	e2504000 	subs	r4, r0, #0	; 0x0
    7714:	1a000143 	bne	7c28 <mmc_startup+0x5b8>
    7718:	e5963030 	ldr	r3, [r6, #48]
    771c:	e3130802 	tst	r3, #131072	; 0x20000
    7720:	128d1c02 	addne	r1, sp, #512	; 0x200
    7724:	11d111be 	ldrhne	r1, [r1, #30]
    7728:	11c618b0 	strhne	r1, [r6, #128]
    772c:	e1d638b0 	ldrh	r3, [r6, #128]
    7730:	e3a01000 	mov	r1, #0	; 0x0
    7734:	e1a03803 	lsl	r3, r3, #16
    7738:	e3a02007 	mov	r2, #7	; 0x7
    773c:	e58d3218 	str	r3, [sp, #536]
    7740:	e3a00009 	mov	r0, #9	; 0x9
    7744:	e28d3c02 	add	r3, sp, #512	; 0x200
    7748:	e1c301b0 	strh	r0, [r3, #16]
    774c:	e58d2214 	str	r2, [sp, #532]
    7750:	e58d122c 	str	r1, [sp, #556]
    7754:	e1a02001 	mov	r2, r1
    7758:	e1a00006 	mov	r0, r6
    775c:	e28d1e21 	add	r1, sp, #528	; 0x210
    7760:	e1a0e00f 	mov	lr, pc
    7764:	e596f104 	ldr	pc, [r6, #260]
    7768:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    776c:	e1a04000 	mov	r4, r0
    7770:	e1a00006 	mov	r0, r6
    7774:	ebfffebc 	bl	726c <mmc_send_status>
    7778:	e3540000 	cmp	r4, #0	; 0x0
    777c:	1a000129 	bne	7c28 <mmc_startup+0x5b8>
    7780:	e5963030 	ldr	r3, [r6, #48]
    7784:	e59d021c 	ldr	r0, [sp, #540]
    7788:	e59d2224 	ldr	r2, [sp, #548]
    778c:	e59d1228 	ldr	r1, [sp, #552]
    7790:	e3530801 	cmp	r3, #65536	; 0x10000
    7794:	e59d3220 	ldr	r3, [sp, #544]
    7798:	e5862068 	str	r2, [r6, #104]
    779c:	e5863064 	str	r3, [r6, #100]
    77a0:	e586106c 	str	r1, [r6, #108]
    77a4:	e5860060 	str	r0, [r6, #96]
    77a8:	1a000013 	bne	77fc <mmc_startup+0x18c>
    77ac:	e1a03d20 	lsr	r3, r0, #26
    77b0:	e203300f 	and	r3, r3, #15	; 0xf
    77b4:	e3530004 	cmp	r3, #4	; 0x4
    77b8:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    77bc:	ea00000c 	b	77f4 <mmc_startup+0x184>
    77c0:	000077f4 	.word	0x000077f4
    77c4:	000077d4 	.word	0x000077d4
    77c8:	000077dc 	.word	0x000077dc
    77cc:	000077e4 	.word	0x000077e4
    77d0:	000077ec 	.word	0x000077ec
    77d4:	e59f3458 	ldr	r3, [pc, #1112]	; 7c34 <mmc_startup+0x5c4>
    77d8:	ea000006 	b	77f8 <mmc_startup+0x188>
    77dc:	e59f3454 	ldr	r3, [pc, #1108]	; 7c38 <mmc_startup+0x5c8>
    77e0:	ea000004 	b	77f8 <mmc_startup+0x188>
    77e4:	e59f3450 	ldr	r3, [pc, #1104]	; 7c3c <mmc_startup+0x5cc>
    77e8:	ea000002 	b	77f8 <mmc_startup+0x188>
    77ec:	e59f344c 	ldr	r3, [pc, #1100]	; 7c40 <mmc_startup+0x5d0>
    77f0:	ea000000 	b	77f8 <mmc_startup+0x188>
    77f4:	e59f3448 	ldr	r3, [pc, #1096]	; 7c44 <mmc_startup+0x5d4>
    77f8:	e5863030 	str	r3, [r6, #48]
    77fc:	e28d1c02 	add	r1, sp, #512	; 0x200
    7800:	e1d132b2 	ldrh	r3, [r1, #34]
    7804:	e3a0e001 	mov	lr, #1	; 0x1
    7808:	e203300f 	and	r3, r3, #15	; 0xf
    780c:	e1a0c31e 	lsl	ip, lr, r3
    7810:	e59d121c 	ldr	r1, [sp, #540]
    7814:	e59f242c 	ldr	r2, [pc, #1068]	; 7c48 <mmc_startup+0x5d8>
    7818:	e1a031a1 	lsr	r3, r1, #3
    781c:	e203300f 	and	r3, r3, #15	; 0xf
    7820:	e7920103 	ldr	r0, [r2, r3, lsl #2]
    7824:	e59f3420 	ldr	r3, [pc, #1056]	; 7c4c <mmc_startup+0x5dc>
    7828:	e2011007 	and	r1, r1, #7	; 0x7
    782c:	e7933101 	ldr	r3, [r3, r1, lsl #2]
    7830:	e5962030 	ldr	r2, [r6, #48]
    7834:	e0030093 	mul	r3, r3, r0
    7838:	e3120802 	tst	r2, #131072	; 0x20000
    783c:	e5863084 	str	r3, [r6, #132]
    7840:	e586c088 	str	ip, [r6, #136]
    7844:	059d3228 	ldreq	r3, [sp, #552]
    7848:	1586c08c 	strne	ip, [r6, #140]
    784c:	01a03b23 	lsreq	r3, r3, #22
    7850:	0203300f 	andeq	r3, r3, #15	; 0xf
    7854:	01a0331e 	lsleq	r3, lr, r3
    7858:	0586308c 	streq	r3, [r6, #140]
    785c:	e5963040 	ldr	r3, [r6, #64]
    7860:	e5962064 	ldr	r2, [r6, #100]
    7864:	e3530000 	cmp	r3, #0	; 0x0
    7868:	e5960068 	ldr	r0, [r6, #104]
    786c:	0a000007 	beq	7890 <mmc_startup+0x220>
    7870:	e202203f 	and	r2, r2, #63	; 0x3f
    7874:	e1a03820 	lsr	r3, r0, #16
    7878:	e1833802 	orr	r3, r3, r2, lsl #16
    787c:	e1a08003 	mov	r8, r3
    7880:	e3a09000 	mov	r9, #0	; 0x0
    7884:	e3a04008 	mov	r4, #8	; 0x8
    7888:	e3a05000 	mov	r5, #0	; 0x0
    788c:	ea000009 	b	78b8 <mmc_startup+0x248>
    7890:	e1a03b02 	lsl	r3, r2, #22
    7894:	e1a03b23 	lsr	r3, r3, #22
    7898:	e200190e 	and	r1, r0, #229376	; 0x38000
    789c:	e1a02f20 	lsr	r2, r0, #30
    78a0:	e1822103 	orr	r2, r2, r3, lsl #2
    78a4:	e1a017a1 	lsr	r1, r1, #15
    78a8:	e1a08002 	mov	r8, r2
    78ac:	e3a09000 	mov	r9, #0	; 0x0
    78b0:	e1a04001 	mov	r4, r1
    78b4:	e3a05000 	mov	r5, #0	; 0x0
    78b8:	e3a02001 	mov	r2, #1	; 0x1
    78bc:	e0980002 	adds	r0, r8, r2
    78c0:	e3a03000 	mov	r3, #0	; 0x0
    78c4:	e0a91003 	adc	r1, r9, r3
    78c8:	e2842002 	add	r2, r4, #2	; 0x2
    78cc:	eb002351 	bl	10618 <__aeabi_llsl>
    78d0:	e596c088 	ldr	ip, [r6, #136]
    78d4:	e0832c90 	umull	r2, r3, r0, ip
    78d8:	e023319c 	mla	r3, ip, r1, r3
    78dc:	e35c0c02 	cmp	ip, #512	; 0x200
    78e0:	e1c629f0 	strd	r2, [r6, #144]
    78e4:	83a03c02 	movhi	r3, #512	; 0x200
    78e8:	85863088 	strhi	r3, [r6, #136]
    78ec:	e596308c 	ldr	r3, [r6, #140]
    78f0:	e3530c02 	cmp	r3, #512	; 0x200
    78f4:	83a03c02 	movhi	r3, #512	; 0x200
    78f8:	8586308c 	strhi	r3, [r6, #140]
    78fc:	e5963050 	ldr	r3, [r6, #80]
    7900:	e2131b01 	ands	r1, r3, #1024	; 0x400
    7904:	1a00000f 	bne	7948 <mmc_startup+0x2d8>
    7908:	e1d638b0 	ldrh	r3, [r6, #128]
    790c:	e3a0201d 	mov	r2, #29	; 0x1d
    7910:	e1a03803 	lsl	r3, r3, #16
    7914:	e58d2214 	str	r2, [sp, #532]
    7918:	e58d3218 	str	r3, [sp, #536]
    791c:	e28d2c02 	add	r2, sp, #512	; 0x200
    7920:	e3a03007 	mov	r3, #7	; 0x7
    7924:	e1c231b0 	strh	r3, [r2, #16]
    7928:	e58d122c 	str	r1, [sp, #556]
    792c:	e1a02001 	mov	r2, r1
    7930:	e1a00006 	mov	r0, r6
    7934:	e28d1e21 	add	r1, sp, #528	; 0x210
    7938:	e1a0e00f 	mov	lr, pc
    793c:	e596f104 	ldr	pc, [r6, #260]
    7940:	e2504000 	subs	r4, r0, #0	; 0x0
    7944:	1a0000b7 	bne	7c28 <mmc_startup+0x5b8>
    7948:	e5962030 	ldr	r2, [r6, #48]
    794c:	e3e03000 	mvn	r3, #0	; 0x0
    7950:	e3120802 	tst	r2, #131072	; 0x20000
    7954:	e5c63082 	strb	r3, [r6, #130]
    7958:	1a00001e 	bne	79d8 <mmc_startup+0x368>
    795c:	e59f32ec 	ldr	r3, [pc, #748]	; 7c50 <mmc_startup+0x5e0>
    7960:	e1520003 	cmp	r2, r3
    7964:	9a00001b 	bls	79d8 <mmc_startup+0x368>
    7968:	e28d1010 	add	r1, sp, #16	; 0x10
    796c:	e1a00006 	mov	r0, r6
    7970:	ebfffc34 	bl	6a48 <mmc_send_ext_csd>
    7974:	e5dd30d0 	ldrb	r3, [sp, #208]
    7978:	e2701001 	rsbs	r1, r0, #1	; 0x1
    797c:	33a01000 	movcc	r1, #0	; 0x0
    7980:	e3530001 	cmp	r3, #1	; 0x1
    7984:	93a01000 	movls	r1, #0	; 0x0
    7988:	e3510000 	cmp	r1, #0	; 0x0
    798c:	0a00000d 	beq	79c8 <mmc_startup+0x358>
    7990:	e5dd20e6 	ldrb	r2, [sp, #230]
    7994:	e5dd10e5 	ldrb	r1, [sp, #229]
    7998:	e5dd00e4 	ldrb	r0, [sp, #228]
    799c:	e1a02802 	lsl	r2, r2, #16
    79a0:	e1822401 	orr	r2, r2, r1, lsl #8
    79a4:	e5dd10e7 	ldrb	r1, [sp, #231]
    79a8:	e1822000 	orr	r2, r2, r0
    79ac:	e1822c01 	orr	r2, r2, r1, lsl #24
    79b0:	e1a03fc2 	asr	r3, r2, #31
    79b4:	e1a01483 	lsl	r1, r3, #9
    79b8:	e1811ba2 	orr	r1, r1, r2, lsr #23
    79bc:	e1a02482 	lsl	r2, r2, #9
    79c0:	e5861094 	str	r1, [r6, #148]
    79c4:	e5862090 	str	r2, [r6, #144]
    79c8:	e5dd30b0 	ldrb	r3, [sp, #176]
    79cc:	e3130001 	tst	r3, #1	; 0x1
    79d0:	15dd30c3 	ldrbne	r3, [sp, #195]
    79d4:	15c63082 	strbne	r3, [r6, #130]
    79d8:	e5963030 	ldr	r3, [r6, #48]
    79dc:	e3130802 	tst	r3, #131072	; 0x20000
    79e0:	0a000002 	beq	79f0 <mmc_startup+0x380>
    79e4:	e1a00006 	mov	r0, r6
    79e8:	ebfffce8 	bl	6d90 <sd_change_freq>
    79ec:	ea000001 	b	79f8 <mmc_startup+0x388>
    79f0:	e1a00006 	mov	r0, r6
    79f4:	ebfffef0 	bl	75bc <mmc_change_freq>
    79f8:	e3500000 	cmp	r0, #0	; 0x0
    79fc:	e1a05000 	mov	r5, r0
    7a00:	11a04000 	movne	r4, r0
    7a04:	1a000087 	bne	7c28 <mmc_startup+0x5b8>
    7a08:	e596304c 	ldr	r3, [r6, #76]
    7a0c:	e5962050 	ldr	r2, [r6, #80]
    7a10:	e5961030 	ldr	r1, [r6, #48]
    7a14:	e0022003 	and	r2, r2, r3
    7a18:	e3110802 	tst	r1, #131072	; 0x20000
    7a1c:	e586204c 	str	r2, [r6, #76]
    7a20:	e2023c01 	and	r3, r2, #256	; 0x100
    7a24:	0a00002c 	beq	7adc <mmc_startup+0x46c>
    7a28:	e3530000 	cmp	r3, #0	; 0x0
    7a2c:	0a000023 	beq	7ac0 <mmc_startup+0x450>
    7a30:	e1d638b0 	ldrh	r3, [r6, #128]
    7a34:	e28d2c02 	add	r2, sp, #512	; 0x200
    7a38:	e1a03803 	lsl	r3, r3, #16
    7a3c:	e58d3218 	str	r3, [sp, #536]
    7a40:	e3a08015 	mov	r8, #21	; 0x15
    7a44:	e3a03037 	mov	r3, #55	; 0x37
    7a48:	e28d9e21 	add	r9, sp, #528	; 0x210
    7a4c:	e1c231b0 	strh	r3, [r2, #16]
    7a50:	e58d022c 	str	r0, [sp, #556]
    7a54:	e58d8214 	str	r8, [sp, #532]
    7a58:	e1a00006 	mov	r0, r6
    7a5c:	e1a01009 	mov	r1, r9
    7a60:	e1a02005 	mov	r2, r5
    7a64:	e1a0e00f 	mov	lr, pc
    7a68:	e596f104 	ldr	pc, [r6, #260]
    7a6c:	e2504000 	subs	r4, r0, #0	; 0x0
    7a70:	1a00006c 	bne	7c28 <mmc_startup+0x5b8>
    7a74:	e3a03002 	mov	r3, #2	; 0x2
    7a78:	e3a01006 	mov	r1, #6	; 0x6
    7a7c:	e28d0c02 	add	r0, sp, #512	; 0x200
    7a80:	e1c011b0 	strh	r1, [r0, #16]
    7a84:	e58d8214 	str	r8, [sp, #532]
    7a88:	e58d3218 	str	r3, [sp, #536]
    7a8c:	e58d522c 	str	r5, [sp, #556]
    7a90:	e1a01009 	mov	r1, r9
    7a94:	e1a02005 	mov	r2, r5
    7a98:	e1a00006 	mov	r0, r6
    7a9c:	e1a0e00f 	mov	lr, pc
    7aa0:	e596f104 	ldr	pc, [r6, #260]
    7aa4:	e2504000 	subs	r4, r0, #0	; 0x0
    7aa8:	1a00005e 	bne	7c28 <mmc_startup+0x5b8>
    7aac:	e3a03004 	mov	r3, #4	; 0x4
    7ab0:	e5863044 	str	r3, [r6, #68]
    7ab4:	e1a00006 	mov	r0, r6
    7ab8:	e1a0e00f 	mov	lr, pc
    7abc:	e596f108 	ldr	pc, [r6, #264]
    7ac0:	e596304c 	ldr	r3, [r6, #76]
    7ac4:	e3130001 	tst	r3, #1	; 0x1
    7ac8:	11a00006 	movne	r0, r6
    7acc:	159f1180 	ldrne	r1, [pc, #384]	; 7c54 <mmc_startup+0x5e4>
    7ad0:	01a00006 	moveq	r0, r6
    7ad4:	059f117c 	ldreq	r1, [pc, #380]	; 7c58 <mmc_startup+0x5e8>
    7ad8:	ea000022 	b	7b68 <mmc_startup+0x4f8>
    7adc:	e3530000 	cmp	r3, #0	; 0x0
    7ae0:	0a000008 	beq	7b08 <mmc_startup+0x498>
    7ae4:	e3a01001 	mov	r1, #1	; 0x1
    7ae8:	e1a03001 	mov	r3, r1
    7aec:	e1a00006 	mov	r0, r6
    7af0:	e3a020b7 	mov	r2, #183	; 0xb7
    7af4:	ebfffe98 	bl	755c <mmc_switch>
    7af8:	e2504000 	subs	r4, r0, #0	; 0x0
    7afc:	03a03004 	moveq	r3, #4	; 0x4
    7b00:	1a000048 	bne	7c28 <mmc_startup+0x5b8>
    7b04:	ea000009 	b	7b30 <mmc_startup+0x4c0>
    7b08:	e3120c02 	tst	r2, #512	; 0x200
    7b0c:	0a00000b 	beq	7b40 <mmc_startup+0x4d0>
    7b10:	e1a00006 	mov	r0, r6
    7b14:	e3a01001 	mov	r1, #1	; 0x1
    7b18:	e3a020b7 	mov	r2, #183	; 0xb7
    7b1c:	e3a03002 	mov	r3, #2	; 0x2
    7b20:	ebfffe8d 	bl	755c <mmc_switch>
    7b24:	e2504000 	subs	r4, r0, #0	; 0x0
    7b28:	1a00003e 	bne	7c28 <mmc_startup+0x5b8>
    7b2c:	e3a03008 	mov	r3, #8	; 0x8
    7b30:	e5863044 	str	r3, [r6, #68]
    7b34:	e1a00006 	mov	r0, r6
    7b38:	e1a0e00f 	mov	lr, pc
    7b3c:	e596f108 	ldr	pc, [r6, #264]
    7b40:	e596304c 	ldr	r3, [r6, #76]
    7b44:	e3130001 	tst	r3, #1	; 0x1
    7b48:	01a00006 	moveq	r0, r6
    7b4c:	059f1108 	ldreq	r1, [pc, #264]	; 7c5c <mmc_startup+0x5ec>
    7b50:	0a000004 	beq	7b68 <mmc_startup+0x4f8>
    7b54:	e3130010 	tst	r3, #16	; 0x10
    7b58:	159f1100 	ldrne	r1, [pc, #256]	; 7c60 <mmc_startup+0x5f0>
    7b5c:	059f1100 	ldreq	r1, [pc, #256]	; 7c64 <mmc_startup+0x5f4>
    7b60:	11a00006 	movne	r0, r6
    7b64:	01a00006 	moveq	r0, r6
    7b68:	ebfffbed 	bl	6b24 <mmc_set_clock>
    7b6c:	e5964094 	ldr	r4, [r6, #148]
    7b70:	e5960090 	ldr	r0, [r6, #144]
    7b74:	e5961088 	ldr	r1, [r6, #136]
    7b78:	e3a03000 	mov	r3, #0	; 0x0
    7b7c:	e3540000 	cmp	r4, #0	; 0x0
    7b80:	e5c630a3 	strb	r3, [r6, #163]
    7b84:	e5c630a2 	strb	r3, [r6, #162]
    7b88:	e58610ac 	str	r1, [r6, #172]
    7b8c:	e58d0230 	str	r0, [sp, #560]
    7b90:	e58d4234 	str	r4, [sp, #564]
    7b94:	1a000003 	bne	7ba8 <mmc_startup+0x538>
    7b98:	eb002187 	bl	101bc <__aeabi_uidiv>
    7b9c:	e58d4234 	str	r4, [sp, #564]
    7ba0:	e58d0230 	str	r0, [sp, #560]
    7ba4:	ea000001 	b	7bb0 <mmc_startup+0x540>
    7ba8:	e28d0e23 	add	r0, sp, #560	; 0x230
    7bac:	eb001541 	bl	d0b8 <__div64_32>
    7bb0:	e5d6007f 	ldrb	r0, [r6, #127]
    7bb4:	e59d1230 	ldr	r1, [sp, #560]
    7bb8:	e5962070 	ldr	r2, [r6, #112]
    7bbc:	e5963078 	ldr	r3, [r6, #120]
    7bc0:	e58610a8 	str	r1, [r6, #168]
    7bc4:	e1803403 	orr	r3, r0, r3, lsl #8
    7bc8:	e1a02422 	lsr	r2, r2, #8
    7bcc:	e59f1094 	ldr	r1, [pc, #148]	; 7c68 <mmc_startup+0x5f8>
    7bd0:	e28600b0 	add	r0, r6, #176	; 0xb0
    7bd4:	eb001900 	bl	dfdc <sprintf>
    7bd8:	e596c074 	ldr	ip, [r6, #116]
    7bdc:	e5d62070 	ldrb	r2, [r6, #112]
    7be0:	e1a0482c 	lsr	r4, ip, #16
    7be4:	e1a0542c 	lsr	r5, ip, #8
    7be8:	e1a03c2c 	lsr	r3, ip, #24
    7bec:	e20440ff 	and	r4, r4, #255	; 0xff
    7bf0:	e20cc0ff 	and	ip, ip, #255	; 0xff
    7bf4:	e59f1070 	ldr	r1, [pc, #112]	; 7c6c <mmc_startup+0x5fc>
    7bf8:	e20550ff 	and	r5, r5, #255	; 0xff
    7bfc:	e28600d9 	add	r0, r6, #217	; 0xd9
    7c00:	e88d1030 	stm	sp, {r4, r5, ip}
    7c04:	eb0018f4 	bl	dfdc <sprintf>
    7c08:	e5962078 	ldr	r2, [r6, #120]
    7c0c:	e28600ee 	add	r0, r6, #238	; 0xee
    7c10:	e1a03c22 	lsr	r3, r2, #24
    7c14:	e203300f 	and	r3, r3, #15	; 0xf
    7c18:	e1a02e22 	lsr	r2, r2, #28
    7c1c:	e59f104c 	ldr	r1, [pc, #76]	; 7c70 <mmc_startup+0x600>
    7c20:	eb0018ed 	bl	dfdc <sprintf>
    7c24:	e3a04000 	mov	r4, #0	; 0x0
    7c28:	e1a00004 	mov	r0, r4
    7c2c:	e28ddf8e 	add	sp, sp, #568	; 0x238
    7c30:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    7c34:	00010014 	.word	0x00010014
    7c38:	00010022 	.word	0x00010022
    7c3c:	00010030 	.word	0x00010030
    7c40:	00010040 	.word	0x00010040
    7c44:	00010012 	.word	0x00010012
    7c48:	00011658 	.word	0x00011658
    7c4c:	00011648 	.word	0x00011648
    7c50:	0001003f 	.word	0x0001003f
    7c54:	02faf080 	.word	0x02faf080
    7c58:	017d7840 	.word	0x017d7840
    7c5c:	01312d00 	.word	0x01312d00
    7c60:	03197500 	.word	0x03197500
    7c64:	018cba80 	.word	0x018cba80
    7c68:	0001335b 	.word	0x0001335b
    7c6c:	0001336d 	.word	0x0001336d
    7c70:	00013378 	.word	0x00013378

00007c74 <mmc_init>:
    7c74:	e92d4010 	push	{r4, lr}
    7c78:	ebfffb5a 	bl	69e8 <find_mmc_device>
    7c7c:	e1a04000 	mov	r4, r0
    7c80:	e1a0e00f 	mov	lr, pc
    7c84:	e590f10c 	ldr	pc, [r0, #268]
    7c88:	e3500000 	cmp	r0, #0	; 0x0
    7c8c:	18bd8010 	popne	{r4, pc}
    7c90:	e3a03001 	mov	r3, #1	; 0x1
    7c94:	e5843044 	str	r3, [r4, #68]
    7c98:	e1a00004 	mov	r0, r4
    7c9c:	e1a0e00f 	mov	lr, pc
    7ca0:	e594f108 	ldr	pc, [r4, #264]
    7ca4:	e1a00004 	mov	r0, r4
    7ca8:	e59f1060 	ldr	r1, [pc, #96]	; 7d10 <mmc_init+0x9c>
    7cac:	ebfffb9c 	bl	6b24 <mmc_set_clock>
    7cb0:	e1a00004 	mov	r0, r4
    7cb4:	ebfffd09 	bl	70e0 <mmc_go_idle>
    7cb8:	e3500000 	cmp	r0, #0	; 0x0
    7cbc:	18bd8010 	popne	{r4, pc}
    7cc0:	e5c40083 	strb	r0, [r4, #131]
    7cc4:	e1a00004 	mov	r0, r4
    7cc8:	ebfffba9 	bl	6b74 <mmc_send_if_cond>
    7ccc:	e1a00004 	mov	r0, r4
    7cd0:	ebfffcad 	bl	6f8c <sd_send_op_cond>
    7cd4:	e3700013 	cmn	r0, #19	; 0x13
    7cd8:	1a000009 	bne	7d04 <mmc_init+0x90>
    7cdc:	e1a00004 	mov	r0, r4
    7ce0:	ebfffd13 	bl	7134 <mmc_send_op_cond>
    7ce4:	e3500000 	cmp	r0, #0	; 0x0
    7ce8:	0a000005 	beq	7d04 <mmc_init+0x90>
    7cec:	e59f0020 	ldr	r0, [pc, #32]	; 7d14 <mmc_init+0xa0>
    7cf0:	e59f3020 	ldr	r3, [pc, #32]	; 7d18 <mmc_init+0xa4>
    7cf4:	e1a0e00f 	mov	lr, pc
    7cf8:	e593f000 	ldr	pc, [r3]
    7cfc:	e3e00010 	mvn	r0, #16	; 0x10
    7d00:	e8bd8010 	pop	{r4, pc}
    7d04:	e1a00004 	mov	r0, r4
    7d08:	e8bd4010 	pop	{r4, lr}
    7d0c:	eafffe57 	b	7670 <mmc_startup>
    7d10:	00061a80 	.word	0x00061a80
    7d14:	0001337e 	.word	0x0001337e
    7d18:	0800d764 	.word	0x0800d764

00007d1c <mmc2_vs_reset>:
    7d1c:	e3a00002 	mov	r0, #2	; 0x2
    7d20:	eaffffd3 	b	7c74 <mmc_init>

00007d24 <mmc1_vs_reset>:
    7d24:	e3a00001 	mov	r0, #1	; 0x1
    7d28:	eaffffd1 	b	7c74 <mmc_init>

00007d2c <mmc0_vs_reset>:
    7d2c:	e3a00000 	mov	r0, #0	; 0x0
    7d30:	eaffffcf 	b	7c74 <mmc_init>

00007d34 <mmc_read_blocks>:
    7d34:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7d38:	e3530001 	cmp	r3, #1	; 0x1
    7d3c:	e24dd030 	sub	sp, sp, #48	; 0x30
    7d40:	e1a06003 	mov	r6, r3
    7d44:	83a03012 	movhi	r3, #18	; 0x12
    7d48:	93a03011 	movls	r3, #17	; 0x11
    7d4c:	e1cd30b0 	strh	r3, [sp]
    7d50:	e5903040 	ldr	r3, [r0, #64]
    7d54:	e3a07000 	mov	r7, #0	; 0x0
    7d58:	e3530000 	cmp	r3, #0	; 0x0
    7d5c:	05903088 	ldreq	r3, [r0, #136]
    7d60:	158d2008 	strne	r2, [sp, #8]
    7d64:	00030293 	muleq	r3, r3, r2
    7d68:	058d3008 	streq	r3, [sp, #8]
    7d6c:	e5903088 	ldr	r3, [r0, #136]
    7d70:	e3a02015 	mov	r2, #21	; 0x15
    7d74:	e58d302c 	str	r3, [sp, #44]
    7d78:	e3a03001 	mov	r3, #1	; 0x1
    7d7c:	e58d2004 	str	r2, [sp, #4]
    7d80:	e58d1020 	str	r1, [sp, #32]
    7d84:	e1a04000 	mov	r4, r0
    7d88:	e58d3024 	str	r3, [sp, #36]
    7d8c:	e58d701c 	str	r7, [sp, #28]
    7d90:	e58d6028 	str	r6, [sp, #40]
    7d94:	e1a0100d 	mov	r1, sp
    7d98:	e28d2020 	add	r2, sp, #32	; 0x20
    7d9c:	e1a0e00f 	mov	lr, pc
    7da0:	e594f104 	ldr	pc, [r4, #260]
    7da4:	e2505000 	subs	r5, r0, #0	; 0x0
    7da8:	e1a0800d 	mov	r8, sp
    7dac:	11a00007 	movne	r0, r7
    7db0:	1a000018 	bne	7e18 <mmc_read_blocks+0xe4>
    7db4:	e3560001 	cmp	r6, #1	; 0x1
    7db8:	9a000012 	bls	7e08 <mmc_read_blocks+0xd4>
    7dbc:	e3a0301d 	mov	r3, #29	; 0x1d
    7dc0:	e58d3004 	str	r3, [sp, #4]
    7dc4:	e3a0300c 	mov	r3, #12	; 0xc
    7dc8:	e1cd30b0 	strh	r3, [sp]
    7dcc:	e58d5008 	str	r5, [sp, #8]
    7dd0:	e58d501c 	str	r5, [sp, #28]
    7dd4:	e1a0100d 	mov	r1, sp
    7dd8:	e1a00004 	mov	r0, r4
    7ddc:	e1a02005 	mov	r2, r5
    7de0:	e1a0e00f 	mov	lr, pc
    7de4:	e594f104 	ldr	pc, [r4, #260]
    7de8:	e3500000 	cmp	r0, #0	; 0x0
    7dec:	0a000005 	beq	7e08 <mmc_read_blocks+0xd4>
    7df0:	e59f0028 	ldr	r0, [pc, #40]	; 7e20 <mmc_read_blocks+0xec>
    7df4:	e59f3028 	ldr	r3, [pc, #40]	; 7e24 <mmc_read_blocks+0xf0>
    7df8:	e1a0e00f 	mov	lr, pc
    7dfc:	e593f000 	ldr	pc, [r3]
    7e00:	e1a00005 	mov	r0, r5
    7e04:	ea000003 	b	7e18 <mmc_read_blocks+0xe4>
    7e08:	e1a00004 	mov	r0, r4
    7e0c:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    7e10:	ebfffd15 	bl	726c <mmc_send_status>
    7e14:	e1a00006 	mov	r0, r6
    7e18:	e28dd030 	add	sp, sp, #48	; 0x30
    7e1c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    7e20:	00013340 	.word	0x00013340
    7e24:	0800d764 	.word	0x0800d764

00007e28 <mmc_bread>:
    7e28:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    7e2c:	e252a000 	subs	sl, r2, #0	; 0x0
    7e30:	e24dd004 	sub	sp, sp, #4	; 0x4
    7e34:	e1a07001 	mov	r7, r1
    7e38:	e1a08003 	mov	r8, r3
    7e3c:	0a000027 	beq	7ee0 <mmc_bread+0xb8>
    7e40:	ebfffae8 	bl	69e8 <find_mmc_device>
    7e44:	e2506000 	subs	r6, r0, #0	; 0x0
    7e48:	0a000024 	beq	7ee0 <mmc_bread+0xb8>
    7e4c:	e59620a8 	ldr	r2, [r6, #168]
    7e50:	e08a1007 	add	r1, sl, r7
    7e54:	e1510002 	cmp	r1, r2
    7e58:	9a000004 	bls	7e70 <mmc_bread+0x48>
    7e5c:	e59f0088 	ldr	r0, [pc, #136]	; 7eec <mmc_bread+0xc4>
    7e60:	e59f3088 	ldr	r3, [pc, #136]	; 7ef0 <mmc_bread+0xc8>
    7e64:	e1a0e00f 	mov	lr, pc
    7e68:	e593f000 	ldr	pc, [r3]
    7e6c:	ea00001b 	b	7ee0 <mmc_bread+0xb8>
    7e70:	e5961088 	ldr	r1, [r6, #136]
    7e74:	ebfffacd 	bl	69b0 <mmc_set_blocklen>
    7e78:	e3500000 	cmp	r0, #0	; 0x0
    7e7c:	01a0500a 	moveq	r5, sl
    7e80:	0a000005 	beq	7e9c <mmc_bread+0x74>
    7e84:	e5961088 	ldr	r1, [r6, #136]
    7e88:	e59f0064 	ldr	r0, [pc, #100]	; 7ef4 <mmc_bread+0xcc>
    7e8c:	e59f305c 	ldr	r3, [pc, #92]	; 7ef0 <mmc_bread+0xc8>
    7e90:	e1a0e00f 	mov	lr, pc
    7e94:	e593f000 	ldr	pc, [r3]
    7e98:	ea000010 	b	7ee0 <mmc_bread+0xb8>
    7e9c:	e5964110 	ldr	r4, [r6, #272]
    7ea0:	e1a02007 	mov	r2, r7
    7ea4:	e1550004 	cmp	r5, r4
    7ea8:	31a04005 	movcc	r4, r5
    7eac:	e1a01008 	mov	r1, r8
    7eb0:	e1a00006 	mov	r0, r6
    7eb4:	e1a03004 	mov	r3, r4
    7eb8:	ebffff9d 	bl	7d34 <mmc_read_blocks>
    7ebc:	e1500004 	cmp	r0, r4
    7ec0:	e0877000 	add	r7, r7, r0
    7ec4:	1a000005 	bne	7ee0 <mmc_bread+0xb8>
    7ec8:	e0555000 	subs	r5, r5, r0
    7ecc:	e5962088 	ldr	r2, [r6, #136]
    7ed0:	10288092 	mlane	r8, r2, r0, r8
    7ed4:	1afffff0 	bne	7e9c <mmc_bread+0x74>
    7ed8:	e000029a 	mul	r0, sl, r2
    7edc:	ea000000 	b	7ee4 <mmc_bread+0xbc>
    7ee0:	e3a00000 	mov	r0, #0	; 0x0
    7ee4:	e28dd004 	add	sp, sp, #4	; 0x4
    7ee8:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    7eec:	00013302 	.word	0x00013302
    7ef0:	0800d764 	.word	0x0800d764
    7ef4:	000133a7 	.word	0x000133a7

00007ef8 <mmc2_vs_read>:
    7ef8:	e1a024a2 	lsr	r2, r2, #9
    7efc:	e1821b83 	orr	r1, r2, r3, lsl #23
    7f00:	e59d2000 	ldr	r2, [sp]
    7f04:	e1a03000 	mov	r3, r0
    7f08:	e1a024c2 	asr	r2, r2, #9
    7f0c:	e3a00002 	mov	r0, #2	; 0x2
    7f10:	eaffffc4 	b	7e28 <mmc_bread>

00007f14 <mmc1_vs_read>:
    7f14:	e1a024a2 	lsr	r2, r2, #9
    7f18:	e1821b83 	orr	r1, r2, r3, lsl #23
    7f1c:	e59d2000 	ldr	r2, [sp]
    7f20:	e1a03000 	mov	r3, r0
    7f24:	e1a024c2 	asr	r2, r2, #9
    7f28:	e3a00001 	mov	r0, #1	; 0x1
    7f2c:	eaffffbd 	b	7e28 <mmc_bread>

00007f30 <mmc0_vs_read>:
    7f30:	e1a024a2 	lsr	r2, r2, #9
    7f34:	e1821b83 	orr	r1, r2, r3, lsl #23
    7f38:	e59d2000 	ldr	r2, [sp]
    7f3c:	e1a03000 	mov	r3, r0
    7f40:	e1a024c2 	asr	r2, r2, #9
    7f44:	e3a00000 	mov	r0, #0	; 0x0
    7f48:	eaffffb6 	b	7e28 <mmc_bread>

00007f4c <mci_pull_data32>:
    7f4c:	e1a02142 	asr	r2, r2, #2
    7f50:	e3a0c000 	mov	ip, #0	; 0x0
    7f54:	ea000003 	b	7f68 <mci_pull_data32+0x1c>
    7f58:	e5903118 	ldr	r3, [r0, #280]
    7f5c:	e5933200 	ldr	r3, [r3, #512]
    7f60:	e781300c 	str	r3, [r1, ip]
    7f64:	e28cc004 	add	ip, ip, #4	; 0x4
    7f68:	e3520000 	cmp	r2, #0	; 0x0
    7f6c:	e2422001 	sub	r2, r2, #1	; 0x1
    7f70:	cafffff8 	bgt	7f58 <mci_pull_data32+0xc>
    7f74:	e12fff1e 	bx	lr

00007f78 <mci_push_data32>:
    7f78:	e92d4010 	push	{r4, lr}
    7f7c:	e3a0c000 	mov	ip, #0	; 0x0
    7f80:	e1a04000 	mov	r4, r0
    7f84:	e1a00001 	mov	r0, r1
    7f88:	e1a01142 	asr	r1, r2, #2
    7f8c:	ea000003 	b	7fa0 <mci_push_data32+0x28>
    7f90:	e790200c 	ldr	r2, [r0, ip]
    7f94:	e5943118 	ldr	r3, [r4, #280]
    7f98:	e28cc004 	add	ip, ip, #4	; 0x4
    7f9c:	e5832200 	str	r2, [r3, #512]
    7fa0:	e3510000 	cmp	r1, #0	; 0x0
    7fa4:	e2411001 	sub	r1, r1, #1	; 0x1
    7fa8:	cafffff8 	bgt	7f90 <mci_push_data32+0x18>
    7fac:	e8bd8010 	pop	{r4, pc}

00007fb0 <imap_sdhc_initialize>:
    7fb0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    7fb4:	e3510000 	cmp	r1, #0	; 0x0
    7fb8:	e24dd004 	sub	sp, sp, #4	; 0x4
    7fbc:	1a000005 	bne	7fd8 <imap_sdhc_initialize+0x28>
    7fc0:	e59f00dc 	ldr	r0, [pc, #220]	; 80a4 <imap_sdhc_initialize+0xf4>
    7fc4:	e59f30dc 	ldr	r3, [pc, #220]	; 80a8 <imap_sdhc_initialize+0xf8>
    7fc8:	e1a0e00f 	mov	lr, pc
    7fcc:	e593f000 	ldr	pc, [r3]
    7fd0:	e3e00004 	mvn	r0, #4	; 0x4
    7fd4:	ea000030 	b	809c <imap_sdhc_initialize+0xec>
    7fd8:	e3500000 	cmp	r0, #0	; 0x0
    7fdc:	03a0359d 	moveq	r3, #658505728	; 0x27400000
    7fe0:	0a000005 	beq	7ffc <imap_sdhc_initialize+0x4c>
    7fe4:	e3500001 	cmp	r0, #1	; 0x1
    7fe8:	059f30bc 	ldreq	r3, [pc, #188]	; 80ac <imap_sdhc_initialize+0xfc>
    7fec:	0a000002 	beq	7ffc <imap_sdhc_initialize+0x4c>
    7ff0:	e3500002 	cmp	r0, #2	; 0x2
    7ff4:	1a000018 	bne	805c <imap_sdhc_initialize+0xac>
    7ff8:	e59f30b0 	ldr	r3, [pc, #176]	; 80b0 <imap_sdhc_initialize+0x100>
    7ffc:	e5813118 	str	r3, [r1, #280]
    8000:	e59f30ac 	ldr	r3, [pc, #172]	; 80b4 <imap_sdhc_initialize+0x104>
    8004:	e591c130 	ldr	ip, [r1, #304]
    8008:	e5813104 	str	r3, [r1, #260]
    800c:	e59f30a4 	ldr	r3, [pc, #164]	; 80b8 <imap_sdhc_initialize+0x108>
    8010:	e3a02000 	mov	r2, #0	; 0x0
    8014:	e5813108 	str	r3, [r1, #264]
    8018:	e59f309c 	ldr	r3, [pc, #156]	; 80bc <imap_sdhc_initialize+0x10c>
    801c:	e5812034 	str	r2, [r1, #52]
    8020:	e581310c 	str	r3, [r1, #268]
    8024:	e59f3094 	ldr	r3, [pc, #148]	; 80c0 <imap_sdhc_initialize+0x110>
    8028:	e5812028 	str	r2, [r1, #40]
    802c:	e581302c 	str	r3, [r1, #44]
    8030:	e3003101 	movw	r3, #257	; 0x101
    8034:	e5813050 	str	r3, [r1, #80]
    8038:	e59f3084 	ldr	r3, [pc, #132]	; 80c4 <imap_sdhc_initialize+0x114>
    803c:	e5812110 	str	r2, [r1, #272]
    8040:	e5813038 	str	r3, [r1, #56]
    8044:	e59f307c 	ldr	r3, [pc, #124]	; 80c8 <imap_sdhc_initialize+0x118>
    8048:	e5810114 	str	r0, [r1, #276]
    804c:	e581303c 	str	r3, [r1, #60]
    8050:	e28c2b01 	add	r2, ip, #1024	; 0x400
    8054:	e28c3010 	add	r3, ip, #16	; 0x10
    8058:	ea000005 	b	8074 <imap_sdhc_initialize+0xc4>
    805c:	e59f0068 	ldr	r0, [pc, #104]	; 80cc <imap_sdhc_initialize+0x11c>
    8060:	e59f3040 	ldr	r3, [pc, #64]	; 80a8 <imap_sdhc_initialize+0xf8>
    8064:	e1a0e00f 	mov	lr, pc
    8068:	e593f000 	ldr	pc, [r3]
    806c:	e3e00000 	mvn	r0, #0	; 0x0
    8070:	ea000009 	b	809c <imap_sdhc_initialize+0xec>
    8074:	e5033004 	str	r3, [r3, #-4]
    8078:	e2833010 	add	r3, r3, #16	; 0x10
    807c:	e1530002 	cmp	r3, r2
    8080:	1afffffb 	bne	8074 <imap_sdhc_initialize+0xc4>
    8084:	e3a03020 	mov	r3, #32	; 0x20
    8088:	e58c33f0 	str	r3, [ip, #1008]
    808c:	e58cc3fc 	str	ip, [ip, #1020]
    8090:	e1a00001 	mov	r0, r1
    8094:	ebfffad4 	bl	6bec <mmc_register>
    8098:	e3a00000 	mov	r0, #0	; 0x0
    809c:	e28dd004 	add	sp, sp, #4	; 0x4
    80a0:	e8bd8000 	pop	{pc}
    80a4:	000133b1 	.word	0x000133b1
    80a8:	0800d764 	.word	0x0800d764
    80ac:	27200000 	.word	0x27200000
    80b0:	27300000 	.word	0x27300000
    80b4:	00008140 	.word	0x00008140
    80b8:	00008624 	.word	0x00008624
    80bc:	00008464 	.word	0x00008464
    80c0:	00ff8000 	.word	0x00ff8000
    80c4:	00061a80 	.word	0x00061a80
    80c8:	018cba80 	.word	0x018cba80
    80cc:	000133c7 	.word	0x000133c7

000080d0 <mci_send_cmd>:
    80d0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    80d4:	e5903118 	ldr	r3, [r0, #280]
    80d8:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    80dc:	e5832028 	str	r2, [r3, #40]
    80e0:	e5903118 	ldr	r3, [r0, #280]
    80e4:	e24dd004 	sub	sp, sp, #4	; 0x4
    80e8:	e583102c 	str	r1, [r3, #44]
    80ec:	e1a04000 	mov	r4, r0
    80f0:	eb0001f4 	bl	88c8 <get_ticks>
    80f4:	e59f5040 	ldr	r5, [pc, #64]	; 813c <mci_send_cmd+0x6c>
    80f8:	e1a06000 	mov	r6, r0
    80fc:	e1a07001 	mov	r7, r1
    8100:	e5943118 	ldr	r3, [r4, #280]
    8104:	e593302c 	ldr	r3, [r3, #44]
    8108:	e3530000 	cmp	r3, #0	; 0x0
    810c:	aa000008 	bge	8134 <mci_send_cmd+0x64>
    8110:	eb0001ec 	bl	88c8 <get_ticks>
    8114:	e1a02000 	mov	r2, r0
    8118:	e1a03001 	mov	r3, r1
    811c:	e0522006 	subs	r2, r2, r6
    8120:	e0c33007 	sbc	r3, r3, r7
    8124:	e3530000 	cmp	r3, #0	; 0x0
    8128:	1a000001 	bne	8134 <mci_send_cmd+0x64>
    812c:	e1520005 	cmp	r2, r5
    8130:	9afffff2 	bls	8100 <mci_send_cmd+0x30>
    8134:	e28dd004 	add	sp, sp, #4	; 0x4
    8138:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    813c:	0007a11f 	.word	0x0007a11f

00008140 <imap_sdhc_send_cmd>:
    8140:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    8144:	e252a000 	subs	sl, r2, #0	; 0x0
    8148:	e24dd004 	sub	sp, sp, #4	; 0x4
    814c:	e1a04000 	mov	r4, r0
    8150:	e1a08001 	mov	r8, r1
    8154:	0a000038 	beq	823c <imap_sdhc_send_cmd+0xfc>
    8158:	e99a000c 	ldmib	sl, {r2, r3}
    815c:	e59a100c 	ldr	r1, [sl, #12]
    8160:	e3120001 	tst	r2, #1	; 0x1
    8164:	e0000193 	mul	r0, r3, r1
    8168:	15942118 	ldrne	r2, [r4, #280]
    816c:	11a03801 	lslne	r3, r1, #16
    8170:	13833001 	orrne	r3, r3, #1	; 0x1
    8174:	15823100 	strne	r3, [r2, #256]
    8178:	e59a200c 	ldr	r2, [sl, #12]
    817c:	e5943118 	ldr	r3, [r4, #280]
    8180:	e1a0e000 	mov	lr, r0
    8184:	e583201c 	str	r2, [r3, #28]
    8188:	e5943118 	ldr	r3, [r4, #280]
    818c:	e3a05000 	mov	r5, #0	; 0x0
    8190:	e5830020 	str	r0, [r3, #32]
    8194:	e5942118 	ldr	r2, [r4, #280]
    8198:	e3a0714a 	mov	r7, #-2147483630	; 0x80000012
    819c:	e5923000 	ldr	r3, [r2]
    81a0:	e3833020 	orr	r3, r3, #32	; 0x20
    81a4:	e5823000 	str	r3, [r2]
    81a8:	e59a6000 	ldr	r6, [sl]
    81ac:	e594c130 	ldr	ip, [r4, #304]
    81b0:	e35e0eff 	cmp	lr, #4080	; 0xff0
    81b4:	31a0100e 	movcc	r1, lr
    81b8:	23a01eff 	movcs	r1, #4080	; 0xff0
    81bc:	e59c0004 	ldr	r0, [ip, #4]
    81c0:	e59f328c 	ldr	r3, [pc, #652]	; 8454 <imap_sdhc_send_cmd+0x314>
    81c4:	e1a02a01 	lsl	r2, r1, #20
    81c8:	e0003003 	and	r3, r0, r3
    81cc:	e1a02a22 	lsr	r2, r2, #20
    81d0:	e1822003 	orr	r2, r2, r3
    81d4:	e05ee001 	subs	lr, lr, r1
    81d8:	e0853006 	add	r3, r5, r6
    81dc:	e98c000c 	stmib	ip, {r2, r3}
    81e0:	e58c7000 	str	r7, [ip]
    81e4:	e1a0200c 	mov	r2, ip
    81e8:	e0855001 	add	r5, r5, r1
    81ec:	e28cc010 	add	ip, ip, #16	; 0x10
    81f0:	1affffee 	bne	81b0 <imap_sdhc_send_cmd+0x70>
    81f4:	e3c73002 	bic	r3, r7, #2	; 0x2
    81f8:	e3833004 	orr	r3, r3, #4	; 0x4
    81fc:	e5823000 	str	r3, [r2]
    8200:	e5942130 	ldr	r2, [r4, #304]
    8204:	e5923000 	ldr	r3, [r2]
    8208:	e3833008 	orr	r3, r3, #8	; 0x8
    820c:	e5823000 	str	r3, [r2]
    8210:	e5942118 	ldr	r2, [r4, #280]
    8214:	e5923000 	ldr	r3, [r2]
    8218:	e3833402 	orr	r3, r3, #33554432	; 0x2000000
    821c:	e5823000 	str	r3, [r2]
    8220:	e5942118 	ldr	r2, [r4, #280]
    8224:	e5923080 	ldr	r3, [r2, #128]
    8228:	e3833080 	orr	r3, r3, #128	; 0x80
    822c:	e5823080 	str	r3, [r2, #128]
    8230:	e5942118 	ldr	r2, [r4, #280]
    8234:	e3a03001 	mov	r3, #1	; 0x1
    8238:	e5823084 	str	r3, [r2, #132]
    823c:	e1d800b0 	ldrh	r0, [r8]
    8240:	e5982004 	ldr	r2, [r8, #4]
    8244:	e350000c 	cmp	r0, #12	; 0xc
    8248:	03a03901 	moveq	r3, #16384	; 0x4000
    824c:	13a03a02 	movne	r3, #8192	; 0x2000
    8250:	e3120004 	tst	r2, #4	; 0x4
    8254:	13833c01 	orrne	r3, r3, #256	; 0x100
    8258:	e3120001 	tst	r2, #1	; 0x1
    825c:	e3831202 	orr	r1, r3, #536870912	; 0x20000000
    8260:	0a000002 	beq	8270 <imap_sdhc_send_cmd+0x130>
    8264:	e3120002 	tst	r2, #2	; 0x2
    8268:	e3811040 	orr	r1, r1, #64	; 0x40
    826c:	13811080 	orrne	r1, r1, #128	; 0x80
    8270:	e35a0000 	cmp	sl, #0	; 0x0
    8274:	0a000003 	beq	8288 <imap_sdhc_send_cmd+0x148>
    8278:	e59a3004 	ldr	r3, [sl, #4]
    827c:	e3811c02 	orr	r1, r1, #512	; 0x200
    8280:	e3130002 	tst	r3, #2	; 0x2
    8284:	13811b01 	orrne	r1, r1, #1024	; 0x400
    8288:	e594312c 	ldr	r3, [r4, #300]
    828c:	e1811000 	orr	r1, r1, r0
    8290:	e3530000 	cmp	r3, #0	; 0x0
    8294:	13a03000 	movne	r3, #0	; 0x0
    8298:	1584312c 	strne	r3, [r4, #300]
    829c:	e5982008 	ldr	r2, [r8, #8]
    82a0:	e5943118 	ldr	r3, [r4, #280]
    82a4:	13811902 	orrne	r1, r1, #32768	; 0x8000
    82a8:	e5832028 	str	r2, [r3, #40]
    82ac:	e5943118 	ldr	r3, [r4, #280]
    82b0:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    82b4:	e583102c 	str	r1, [r3, #44]
    82b8:	eb000182 	bl	88c8 <get_ticks>
    82bc:	e3005bb8 	movw	r5, #3000	; 0xbb8
    82c0:	e1a06000 	mov	r6, r0
    82c4:	e1a07001 	mov	r7, r1
    82c8:	eb00017e 	bl	88c8 <get_ticks>
    82cc:	e1a02000 	mov	r2, r0
    82d0:	e1a03001 	mov	r3, r1
    82d4:	e0522006 	subs	r2, r2, r6
    82d8:	e0c33007 	sbc	r3, r3, r7
    82dc:	e3530000 	cmp	r3, #0	; 0x0
    82e0:	1a000007 	bne	8304 <imap_sdhc_send_cmd+0x1c4>
    82e4:	e1520005 	cmp	r2, r5
    82e8:	8a000005 	bhi	8304 <imap_sdhc_send_cmd+0x1c4>
    82ec:	e5943118 	ldr	r3, [r4, #280]
    82f0:	e5932044 	ldr	r2, [r3, #68]
    82f4:	e2831044 	add	r1, r3, #68	; 0x44
    82f8:	e3120004 	tst	r2, #4	; 0x4
    82fc:	0afffff1 	beq	82c8 <imap_sdhc_send_cmd+0x188>
    8300:	ea000005 	b	831c <imap_sdhc_send_cmd+0x1dc>
    8304:	e59f014c 	ldr	r0, [pc, #332]	; 8458 <imap_sdhc_send_cmd+0x318>
    8308:	e59f314c 	ldr	r3, [pc, #332]	; 845c <imap_sdhc_send_cmd+0x31c>
    830c:	e1a0e00f 	mov	lr, pc
    8310:	e593f000 	ldr	pc, [r3]
    8314:	e3e00011 	mvn	r0, #17	; 0x11
    8318:	ea00004b 	b	844c <imap_sdhc_send_cmd+0x30c>
    831c:	e5915000 	ldr	r5, [r1]
    8320:	e3003142 	movw	r3, #322	; 0x142
    8324:	e0053003 	and	r3, r5, r3
    8328:	e3530000 	cmp	r3, #0	; 0x0
    832c:	e3a03004 	mov	r3, #4	; 0x4
    8330:	e5813000 	str	r3, [r1]
    8334:	0a00000e 	beq	8374 <imap_sdhc_send_cmd+0x234>
    8338:	e1d810b0 	ldrh	r1, [r8]
    833c:	e5943118 	ldr	r3, [r4, #280]
    8340:	e3510037 	cmp	r1, #55	; 0x37
    8344:	13510008 	cmpne	r1, #8	; 0x8
    8348:	e5835044 	str	r5, [r3, #68]
    834c:	0a000004 	beq	8364 <imap_sdhc_send_cmd+0x224>
    8350:	e59f0108 	ldr	r0, [pc, #264]	; 8460 <imap_sdhc_send_cmd+0x320>
    8354:	e1a02005 	mov	r2, r5
    8358:	e59f30fc 	ldr	r3, [pc, #252]	; 845c <imap_sdhc_send_cmd+0x31c>
    835c:	e1a0e00f 	mov	lr, pc
    8360:	e593f000 	ldr	pc, [r3]
    8364:	e3150c01 	tst	r5, #256	; 0x100
    8368:	13e00012 	mvnne	r0, #18	; 0x12
    836c:	03e00011 	mvneq	r0, #17	; 0x11
    8370:	ea000035 	b	844c <imap_sdhc_send_cmd+0x30c>
    8374:	e5983004 	ldr	r3, [r8, #4]
    8378:	e5942118 	ldr	r2, [r4, #280]
    837c:	e3130002 	tst	r3, #2	; 0x2
    8380:	05923030 	ldreq	r3, [r2, #48]
    8384:	0588300c 	streq	r3, [r8, #12]
    8388:	0a00000a 	beq	83b8 <imap_sdhc_send_cmd+0x278>
    838c:	e592303c 	ldr	r3, [r2, #60]
    8390:	e588300c 	str	r3, [r8, #12]
    8394:	e5943118 	ldr	r3, [r4, #280]
    8398:	e5933038 	ldr	r3, [r3, #56]
    839c:	e5883010 	str	r3, [r8, #16]
    83a0:	e5943118 	ldr	r3, [r4, #280]
    83a4:	e5933034 	ldr	r3, [r3, #52]
    83a8:	e5883014 	str	r3, [r8, #20]
    83ac:	e5943118 	ldr	r3, [r4, #280]
    83b0:	e5933030 	ldr	r3, [r3, #48]
    83b4:	e5883018 	str	r3, [r8, #24]
    83b8:	e35a0000 	cmp	sl, #0	; 0x0
    83bc:	01a0000a 	moveq	r0, sl
    83c0:	0a000021 	beq	844c <imap_sdhc_send_cmd+0x30c>
    83c4:	e594c118 	ldr	ip, [r4, #280]
    83c8:	e59a3004 	ldr	r3, [sl, #4]
    83cc:	e28c7044 	add	r7, ip, #68	; 0x44
    83d0:	e2030001 	and	r0, r3, #1	; 0x1
    83d4:	e28c608c 	add	r6, ip, #140	; 0x8c
    83d8:	e5975000 	ldr	r5, [r7]
    83dc:	e3500000 	cmp	r0, #0	; 0x0
    83e0:	e596e000 	ldr	lr, [r6]
    83e4:	e2051008 	and	r1, r5, #8	; 0x8
    83e8:	0a000005 	beq	8404 <imap_sdhc_send_cmd+0x2c4>
    83ec:	e3510000 	cmp	r1, #0	; 0x0
    83f0:	0afffff8 	beq	83d8 <imap_sdhc_send_cmd+0x298>
    83f4:	e31e0002 	tst	lr, #2	; 0x2
    83f8:	0afffff6 	beq	83d8 <imap_sdhc_send_cmd+0x298>
    83fc:	e3a03002 	mov	r3, #2	; 0x2
    8400:	ea000004 	b	8418 <imap_sdhc_send_cmd+0x2d8>
    8404:	e3510000 	cmp	r1, #0	; 0x0
    8408:	0afffff2 	beq	83d8 <imap_sdhc_send_cmd+0x298>
    840c:	e31e0001 	tst	lr, #1	; 0x1
    8410:	0afffff0 	beq	83d8 <imap_sdhc_send_cmd+0x298>
    8414:	e3a03001 	mov	r3, #1	; 0x1
    8418:	e58c308c 	str	r3, [ip, #140]
    841c:	e5943118 	ldr	r3, [r4, #280]
    8420:	e3a00000 	mov	r0, #0	; 0x0
    8424:	e5835044 	str	r5, [r3, #68]
    8428:	e5942118 	ldr	r2, [r4, #280]
    842c:	e5923000 	ldr	r3, [r2]
    8430:	e3c33402 	bic	r3, r3, #33554432	; 0x2000000
    8434:	e3833004 	orr	r3, r3, #4	; 0x4
    8438:	e5823000 	str	r3, [r2]
    843c:	e5942118 	ldr	r2, [r4, #280]
    8440:	e5923080 	ldr	r3, [r2, #128]
    8444:	e3c33080 	bic	r3, r3, #128	; 0x80
    8448:	e5823080 	str	r3, [r2, #128]
    844c:	e28dd004 	add	sp, sp, #4	; 0x4
    8450:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    8454:	03ffe000 	.word	0x03ffe000
    8458:	000133d9 	.word	0x000133d9
    845c:	0800d764 	.word	0x0800d764
    8460:	000133e7 	.word	0x000133e7

00008464 <imap_sdhc_init>:
    8464:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8468:	e5901114 	ldr	r1, [r0, #276]
    846c:	e3a02000 	mov	r2, #0	; 0x0
    8470:	e3a03001 	mov	r3, #1	; 0x1
    8474:	e3510001 	cmp	r1, #1	; 0x1
    8478:	e24dd004 	sub	sp, sp, #4	; 0x4
    847c:	e1a04000 	mov	r4, r0
    8480:	e5802048 	str	r2, [r0, #72]
    8484:	e580312c 	str	r3, [r0, #300]
    8488:	e580211c 	str	r2, [r0, #284]
    848c:	0a000007 	beq	84b0 <imap_sdhc_init+0x4c>
    8490:	3a000002 	bcc	84a0 <imap_sdhc_init+0x3c>
    8494:	e3510002 	cmp	r1, #2	; 0x2
    8498:	1a00000c 	bne	84d0 <imap_sdhc_init+0x6c>
    849c:	ea000007 	b	84c0 <imap_sdhc_init+0x5c>
    84a0:	e3a0000c 	mov	r0, #12	; 0xc
    84a4:	ebffe0da 	bl	814 <pads_init_module>
    84a8:	e59f0150 	ldr	r0, [pc, #336]	; 8600 <imap_sdhc_init+0x19c>
    84ac:	ea000006 	b	84cc <imap_sdhc_init+0x68>
    84b0:	e3a0000d 	mov	r0, #13	; 0xd
    84b4:	ebffe0d6 	bl	814 <pads_init_module>
    84b8:	e59f0144 	ldr	r0, [pc, #324]	; 8604 <imap_sdhc_init+0x1a0>
    84bc:	ea000002 	b	84cc <imap_sdhc_init+0x68>
    84c0:	e3a0000e 	mov	r0, #14	; 0xe
    84c4:	ebffe0d2 	bl	814 <pads_init_module>
    84c8:	e59f0138 	ldr	r0, [pc, #312]	; 8608 <imap_sdhc_init+0x1a4>
    84cc:	ebffe094 	bl	724 <module_enable>
    84d0:	e5942118 	ldr	r2, [r4, #280]
    84d4:	e59f5130 	ldr	r5, [pc, #304]	; 860c <imap_sdhc_init+0x1a8>
    84d8:	e5923070 	ldr	r3, [r2, #112]
    84dc:	e1a03943 	asr	r3, r3, #18
    84e0:	e2033007 	and	r3, r3, #7	; 0x7
    84e4:	e3530002 	cmp	r3, #2	; 0x2
    84e8:	03a03003 	moveq	r3, #3	; 0x3
    84ec:	13a03002 	movne	r3, #2	; 0x2
    84f0:	e5843120 	str	r3, [r4, #288]
    84f4:	e3a03007 	mov	r3, #7	; 0x7
    84f8:	e5823000 	str	r3, [r2]
    84fc:	e59f310c 	ldr	r3, [pc, #268]	; 8610 <imap_sdhc_init+0x1ac>
    8500:	e5843124 	str	r3, [r4, #292]
    8504:	e59f3108 	ldr	r3, [pc, #264]	; 8614 <imap_sdhc_init+0x1b0>
    8508:	e5843128 	str	r3, [r4, #296]
    850c:	eb0000ed 	bl	88c8 <get_ticks>
    8510:	e1a06000 	mov	r6, r0
    8514:	e1a07001 	mov	r7, r1
    8518:	e5943118 	ldr	r3, [r4, #280]
    851c:	e3a00001 	mov	r0, #1	; 0x1
    8520:	e5933000 	ldr	r3, [r3]
    8524:	e3130007 	tst	r3, #7	; 0x7
    8528:	0a00000d 	beq	8564 <imap_sdhc_init+0x100>
    852c:	eb0000f6 	bl	890c <udelay>
    8530:	eb0000e4 	bl	88c8 <get_ticks>
    8534:	e1a02000 	mov	r2, r0
    8538:	e1a03001 	mov	r3, r1
    853c:	e0522006 	subs	r2, r2, r6
    8540:	e0c33007 	sbc	r3, r3, r7
    8544:	e3530000 	cmp	r3, #0	; 0x0
    8548:	1a000001 	bne	8554 <imap_sdhc_init+0xf0>
    854c:	e1520005 	cmp	r2, r5
    8550:	9afffff0 	bls	8518 <imap_sdhc_init+0xb4>
    8554:	e59f00bc 	ldr	r0, [pc, #188]	; 8618 <imap_sdhc_init+0x1b4>
    8558:	e59f30bc 	ldr	r3, [pc, #188]	; 861c <imap_sdhc_init+0x1b8>
    855c:	e1a0e00f 	mov	lr, pc
    8560:	e593f000 	ldr	pc, [r3]
    8564:	e5943118 	ldr	r3, [r4, #280]
    8568:	e3a01000 	mov	r1, #0	; 0x0
    856c:	e5831024 	str	r1, [r3, #36]
    8570:	e5943118 	ldr	r3, [r4, #280]
    8574:	e3e02000 	mvn	r2, #0	; 0x0
    8578:	e5832044 	str	r2, [r3, #68]
    857c:	e5943118 	ldr	r3, [r4, #280]
    8580:	e1a00001 	mov	r0, r1
    8584:	e5832014 	str	r2, [r3, #20]
    8588:	e5942118 	ldr	r2, [r4, #280]
    858c:	e592304c 	ldr	r3, [r2, #76]
    8590:	e1a03283 	lsl	r3, r3, #5
    8594:	e1a03b23 	lsr	r3, r3, #22
    8598:	e1833803 	orr	r3, r3, r3, lsl #16
    859c:	e582304c 	str	r3, [r2, #76]
    85a0:	e5943118 	ldr	r3, [r4, #280]
    85a4:	e5831010 	str	r1, [r3, #16]
    85a8:	e5942118 	ldr	r2, [r4, #280]
    85ac:	e5923004 	ldr	r3, [r2, #4]
    85b0:	e3833001 	orr	r3, r3, #1	; 0x1
    85b4:	e5823004 	str	r3, [r2, #4]
    85b8:	e5943118 	ldr	r3, [r4, #280]
    85bc:	e5831058 	str	r1, [r3, #88]
    85c0:	e5942118 	ldr	r2, [r4, #280]
    85c4:	e59f3054 	ldr	r3, [pc, #84]	; 8620 <imap_sdhc_init+0x1bc>
    85c8:	e5823108 	str	r3, [r2, #264]
    85cc:	e5942118 	ldr	r2, [r4, #280]
    85d0:	e3003103 	movw	r3, #259	; 0x103
    85d4:	e5823090 	str	r3, [r2, #144]
    85d8:	e5942130 	ldr	r2, [r4, #304]
    85dc:	e5943118 	ldr	r3, [r4, #280]
    85e0:	e5832088 	str	r2, [r3, #136]
    85e4:	e5942118 	ldr	r2, [r4, #280]
    85e8:	e5923080 	ldr	r3, [r2, #128]
    85ec:	e3c3307c 	bic	r3, r3, #124	; 0x7c
    85f0:	e3833008 	orr	r3, r3, #8	; 0x8
    85f4:	e5823080 	str	r3, [r2, #128]
    85f8:	e28dd004 	add	sp, sp, #4	; 0x4
    85fc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8600:	2d039000 	.word	0x2d039000
    8604:	2d038800 	.word	0x2d038800
    8608:	2d038c00 	.word	0x2d038c00
    860c:	0007a11f 	.word	0x0007a11f
    8610:	00007f4c 	.word	0x00007f4c
    8614:	00007f78 	.word	0x00007f78
    8618:	000133ff 	.word	0x000133ff
    861c:	0800d764 	.word	0x0800d764
    8620:	3fa80000 	.word	0x3fa80000

00008624 <imap_sdhc_set_ios>:
    8624:	e5902048 	ldr	r2, [r0, #72]
    8628:	e590311c 	ldr	r3, [r0, #284]
    862c:	e92d4070 	push	{r4, r5, r6, lr}
    8630:	e1520003 	cmp	r2, r3
    8634:	e1a05000 	mov	r5, r0
    8638:	0a00002a 	beq	86e8 <imap_sdhc_set_ios+0xc4>
    863c:	e5903114 	ldr	r3, [r0, #276]
    8640:	e59f00d0 	ldr	r0, [pc, #208]	; 8718 <imap_sdhc_set_ios+0xf4>
    8644:	e0830000 	add	r0, r3, r0
    8648:	e1a00200 	lsl	r0, r0, #4
    864c:	ebffe293 	bl	10a0 <module_get_clock>
    8650:	e5954048 	ldr	r4, [r5, #72]
    8654:	e1a06000 	mov	r6, r0
    8658:	e1a01004 	mov	r1, r4
    865c:	eb001f52 	bl	103ac <__aeabi_uidivmod>
    8660:	e3510000 	cmp	r1, #0	; 0x0
    8664:	0a000005 	beq	8680 <imap_sdhc_set_ios+0x5c>
    8668:	e1a00006 	mov	r0, r6
    866c:	e1a01004 	mov	r1, r4
    8670:	eb001ed1 	bl	101bc <__aeabi_uidiv>
    8674:	e1a000a0 	lsr	r0, r0, #1
    8678:	e2806001 	add	r6, r0, #1	; 0x1
    867c:	ea000003 	b	8690 <imap_sdhc_set_ios+0x6c>
    8680:	e1a00006 	mov	r0, r6
    8684:	e1a01004 	mov	r1, r4
    8688:	eb001ecb 	bl	101bc <__aeabi_uidiv>
    868c:	e1a060a0 	lsr	r6, r0, #1
    8690:	e5953118 	ldr	r3, [r5, #280]
    8694:	e3a04000 	mov	r4, #0	; 0x0
    8698:	e5834010 	str	r4, [r3, #16]
    869c:	e5953118 	ldr	r3, [r5, #280]
    86a0:	e1a00005 	mov	r0, r5
    86a4:	e583400c 	str	r4, [r3, #12]
    86a8:	e59f106c 	ldr	r1, [pc, #108]	; 871c <imap_sdhc_set_ios+0xf8>
    86ac:	e1a02004 	mov	r2, r4
    86b0:	ebfffe86 	bl	80d0 <mci_send_cmd>
    86b4:	e5953118 	ldr	r3, [r5, #280]
    86b8:	e1a00005 	mov	r0, r5
    86bc:	e5836008 	str	r6, [r3, #8]
    86c0:	e59f1054 	ldr	r1, [pc, #84]	; 871c <imap_sdhc_set_ios+0xf8>
    86c4:	e1a02004 	mov	r2, r4
    86c8:	ebfffe80 	bl	80d0 <mci_send_cmd>
    86cc:	e5952118 	ldr	r2, [r5, #280]
    86d0:	e3a03001 	mov	r3, #1	; 0x1
    86d4:	e5823010 	str	r3, [r2, #16]
    86d8:	e1a00005 	mov	r0, r5
    86dc:	e1a02004 	mov	r2, r4
    86e0:	e59f1034 	ldr	r1, [pc, #52]	; 871c <imap_sdhc_set_ios+0xf8>
    86e4:	ebfffe79 	bl	80d0 <mci_send_cmd>
    86e8:	e5953044 	ldr	r3, [r5, #68]
    86ec:	e3530004 	cmp	r3, #4	; 0x4
    86f0:	05952118 	ldreq	r2, [r5, #280]
    86f4:	03a03001 	moveq	r3, #1	; 0x1
    86f8:	05823018 	streq	r3, [r2, #24]
    86fc:	08bd8070 	popeq	{r4, r5, r6, pc}
    8700:	e3530008 	cmp	r3, #8	; 0x8
    8704:	e5950118 	ldr	r0, [r5, #280]
    8708:	059f3010 	ldreq	r3, [pc, #16]	; 8720 <imap_sdhc_set_ios+0xfc>
    870c:	13a03000 	movne	r3, #0	; 0x0
    8710:	e5803018 	str	r3, [r0, #24]
    8714:	e8bd8070 	pop	{r4, r5, r6, pc}
    8718:	02d00a24 	.word	0x02d00a24
    871c:	00202000 	.word	0x00202000
    8720:	00010001 	.word	0x00010001

00008724 <serial_tstc>:
    8724:	e59f3010 	ldr	r3, [pc, #16]	; 873c <serial_tstc+0x18>
    8728:	e5930018 	ldr	r0, [r3, #24]
    872c:	e1a00220 	lsr	r0, r0, #4
    8730:	e2200001 	eor	r0, r0, #1	; 0x1
    8734:	e2000001 	and	r0, r0, #1	; 0x1
    8738:	e12fff1e 	bx	lr
    873c:	20803000 	.word	0x20803000

00008740 <serial_putc>:
    8740:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8744:	e24dd004 	sub	sp, sp, #4	; 0x4
    8748:	e1a06000 	mov	r6, r0
    874c:	eb00005d 	bl	88c8 <get_ticks>
    8750:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    8754:	e3a05000 	mov	r5, #0	; 0x0
    8758:	e0944000 	adds	r4, r4, r0
    875c:	e59f7034 	ldr	r7, [pc, #52]	; 8798 <serial_putc+0x58>
    8760:	e0a55001 	adc	r5, r5, r1
    8764:	e5973018 	ldr	r3, [r7, #24]
    8768:	e3130020 	tst	r3, #32	; 0x20
    876c:	0a000005 	beq	8788 <serial_putc+0x48>
    8770:	eb000054 	bl	88c8 <get_ticks>
    8774:	e1510005 	cmp	r1, r5
    8778:	8a000002 	bhi	8788 <serial_putc+0x48>
    877c:	1afffff8 	bne	8764 <serial_putc+0x24>
    8780:	e1500004 	cmp	r0, r4
    8784:	9afffff6 	bls	8764 <serial_putc+0x24>
    8788:	e59f3008 	ldr	r3, [pc, #8]	; 8798 <serial_putc+0x58>
    878c:	e5836000 	str	r6, [r3]
    8790:	e28dd004 	add	sp, sp, #4	; 0x4
    8794:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8798:	20803000 	.word	0x20803000

0000879c <serial_puts>:
    879c:	e92d4030 	push	{r4, r5, lr}
    87a0:	e1a05000 	mov	r5, r0
    87a4:	e24dd004 	sub	sp, sp, #4	; 0x4
    87a8:	e3a04000 	mov	r4, #0	; 0x0
    87ac:	ea000006 	b	87cc <serial_puts+0x30>
    87b0:	e353000a 	cmp	r3, #10	; 0xa
    87b4:	1a000001 	bne	87c0 <serial_puts+0x24>
    87b8:	e3a0000d 	mov	r0, #13	; 0xd
    87bc:	ebffffdf 	bl	8740 <serial_putc>
    87c0:	e7d50004 	ldrb	r0, [r5, r4]
    87c4:	ebffffdd 	bl	8740 <serial_putc>
    87c8:	e2844001 	add	r4, r4, #1	; 0x1
    87cc:	e7d53004 	ldrb	r3, [r5, r4]
    87d0:	e3530000 	cmp	r3, #0	; 0x0
    87d4:	1afffff5 	bne	87b0 <serial_puts+0x14>
    87d8:	e28dd004 	add	sp, sp, #4	; 0x4
    87dc:	e8bd8030 	pop	{r4, r5, pc}

000087e0 <serial_getc>:
    87e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    87e4:	e59f2024 	ldr	r2, [pc, #36]	; 8810 <serial_getc+0x30>
    87e8:	e24dd004 	sub	sp, sp, #4	; 0x4
    87ec:	e5923018 	ldr	r3, [r2, #24]
    87f0:	e3130010 	tst	r3, #16	; 0x10
    87f4:	1afffffc 	bne	87ec <serial_getc+0xc>
    87f8:	ebffef19 	bl	4464 <tt_rnd_seed>
    87fc:	e59f300c 	ldr	r3, [pc, #12]	; 8810 <serial_getc+0x30>
    8800:	e5930000 	ldr	r0, [r3]
    8804:	e6ef0070 	uxtb	r0, r0
    8808:	e28dd004 	add	sp, sp, #4	; 0x4
    880c:	e8bd8000 	pop	{pc}
    8810:	20803000 	.word	0x20803000

00008814 <serial_setbrg>:
    8814:	e92d4010 	push	{r4, lr}
    8818:	e59f0038 	ldr	r0, [pc, #56]	; 8858 <serial_setbrg+0x44>
    881c:	ebffe21f 	bl	10a0 <module_get_clock>
    8820:	e59f1034 	ldr	r1, [pc, #52]	; 885c <serial_setbrg+0x48>
    8824:	e1a04000 	mov	r4, r0
    8828:	eb001ee5 	bl	103c4 <__aeabi_idiv>
    882c:	e6bf3070 	sxth	r3, r0
    8830:	e59f0028 	ldr	r0, [pc, #40]	; 8860 <serial_setbrg+0x4c>
    8834:	e59f1020 	ldr	r1, [pc, #32]	; 885c <serial_setbrg+0x48>
    8838:	e0204390 	mla	r0, r0, r3, r4
    883c:	e59f4020 	ldr	r4, [pc, #32]	; 8864 <serial_setbrg+0x50>
    8840:	e1a00300 	lsl	r0, r0, #6
    8844:	e5843024 	str	r3, [r4, #36]
    8848:	eb001edd 	bl	103c4 <__aeabi_idiv>
    884c:	e200003f 	and	r0, r0, #63	; 0x3f
    8850:	e5840028 	str	r0, [r4, #40]
    8854:	e8bd8010 	pop	{r4, pc}
    8858:	2d00a290 	.word	0x2d00a290
    885c:	001c2000 	.word	0x001c2000
    8860:	ffe3e000 	.word	0xffe3e000
    8864:	20803000 	.word	0x20803000

00008868 <init_serial>:
    8868:	e92d4010 	push	{r4, lr}
    886c:	e59f004c 	ldr	r0, [pc, #76]	; 88c0 <init_serial+0x58>
    8870:	e59f404c 	ldr	r4, [pc, #76]	; 88c4 <init_serial+0x5c>
    8874:	ebffdfaa 	bl	724 <module_enable>
    8878:	e3a0000f 	mov	r0, #15	; 0xf
    887c:	ebffdfe4 	bl	814 <pads_init_module>
    8880:	e594302c 	ldr	r3, [r4, #44]
    8884:	e3c330ff 	bic	r3, r3, #255	; 0xff
    8888:	e3833060 	orr	r3, r3, #96	; 0x60
    888c:	e584302c 	str	r3, [r4, #44]
    8890:	ebffffdf 	bl	8814 <serial_setbrg>
    8894:	e5943030 	ldr	r3, [r4, #48]
    8898:	e3a00000 	mov	r0, #0	; 0x0
    889c:	e3c33cff 	bic	r3, r3, #65280	; 0xff00
    88a0:	e3c33087 	bic	r3, r3, #135	; 0x87
    88a4:	e3833c03 	orr	r3, r3, #768	; 0x300
    88a8:	e3833001 	orr	r3, r3, #1	; 0x1
    88ac:	e5843030 	str	r3, [r4, #48]
    88b0:	e594302c 	ldr	r3, [r4, #44]
    88b4:	e3833010 	orr	r3, r3, #16	; 0x10
    88b8:	e584302c 	str	r3, [r4, #44]
    88bc:	e8bd8010 	pop	{r4, pc}
    88c0:	2d012000 	.word	0x2d012000
    88c4:	20803000 	.word	0x20803000

000088c8 <get_ticks>:
    88c8:	e59f1038 	ldr	r1, [pc, #56]	; 8908 <get_ticks+0x40>
    88cc:	e5913204 	ldr	r3, [r1, #516]
    88d0:	e5912200 	ldr	r2, [r1, #512]
    88d4:	e591c204 	ldr	ip, [r1, #516]
    88d8:	e15c0003 	cmp	ip, r3
    88dc:	15913200 	ldrne	r3, [r1, #512]
    88e0:	01a00002 	moveq	r0, r2
    88e4:	03a01000 	moveq	r1, #0	; 0x0
    88e8:	11a00003 	movne	r0, r3
    88ec:	13a01000 	movne	r1, #0	; 0x0
    88f0:	e3a02000 	mov	r2, #0	; 0x0
    88f4:	e1822000 	orr	r2, r2, r0
    88f8:	e18c3001 	orr	r3, ip, r1
    88fc:	e1a01003 	mov	r1, r3
    8900:	e1a00002 	mov	r0, r2
    8904:	e12fff1e 	bx	lr
    8908:	21b00000 	.word	0x21b00000

0000890c <udelay>:
    890c:	e59f1084 	ldr	r1, [pc, #132]	; 8998 <udelay+0x8c>
    8910:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8914:	e5913204 	ldr	r3, [r1, #516]
    8918:	e5912200 	ldr	r2, [r1, #512]
    891c:	e591c204 	ldr	ip, [r1, #516]
    8920:	e15c0003 	cmp	ip, r3
    8924:	15913200 	ldrne	r3, [r1, #512]
    8928:	01a04002 	moveq	r4, r2
    892c:	03a05000 	moveq	r5, #0	; 0x0
    8930:	11a04003 	movne	r4, r3
    8934:	13a05000 	movne	r5, #0	; 0x0
    8938:	e3a02000 	mov	r2, #0	; 0x0
    893c:	e1822004 	orr	r2, r2, r4
    8940:	e18c3005 	orr	r3, ip, r5
    8944:	e0926000 	adds	r6, r2, r0
    8948:	e59fc048 	ldr	ip, [pc, #72]	; 8998 <udelay+0x8c>
    894c:	e2a37000 	adc	r7, r3, #0	; 0x0
    8950:	e59c3204 	ldr	r3, [ip, #516]
    8954:	e59c2200 	ldr	r2, [ip, #512]
    8958:	e59c1204 	ldr	r1, [ip, #516]
    895c:	e1510003 	cmp	r1, r3
    8960:	159c3200 	ldrne	r3, [ip, #512]
    8964:	01a04002 	moveq	r4, r2
    8968:	03a05000 	moveq	r5, #0	; 0x0
    896c:	11a04003 	movne	r4, r3
    8970:	13a05000 	movne	r5, #0	; 0x0
    8974:	e1851001 	orr	r1, r5, r1
    8978:	e3a02000 	mov	r2, #0	; 0x0
    897c:	e1570001 	cmp	r7, r1
    8980:	e1840002 	orr	r0, r4, r2
    8984:	8afffff1 	bhi	8950 <udelay+0x44>
    8988:	18bd80f0 	popne	{r4, r5, r6, r7, pc}
    898c:	e1560000 	cmp	r6, r0
    8990:	8affffee 	bhi	8950 <udelay+0x44>
    8994:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8998:	21b00000 	.word	0x21b00000

0000899c <get_timer>:
    899c:	e59f2074 	ldr	r2, [pc, #116]	; 8a18 <get_timer+0x7c>
    89a0:	e92d4070 	push	{r4, r5, r6, lr}
    89a4:	e5923204 	ldr	r3, [r2, #516]
    89a8:	e1a06000 	mov	r6, r0
    89ac:	e5921200 	ldr	r1, [r2, #512]
    89b0:	e5920204 	ldr	r0, [r2, #516]
    89b4:	e24dd008 	sub	sp, sp, #8	; 0x8
    89b8:	e1500003 	cmp	r0, r3
    89bc:	15923200 	ldrne	r3, [r2, #512]
    89c0:	01a04001 	moveq	r4, r1
    89c4:	03a05000 	moveq	r5, #0	; 0x0
    89c8:	11a04003 	movne	r4, r3
    89cc:	13a05000 	movne	r5, #0	; 0x0
    89d0:	e1a03000 	mov	r3, r0
    89d4:	e3a02000 	mov	r2, #0	; 0x0
    89d8:	e1851003 	orr	r1, r5, r3
    89dc:	e1840002 	orr	r0, r4, r2
    89e0:	e1510002 	cmp	r1, r2
    89e4:	e88d0003 	stm	sp, {r0, r1}
    89e8:	1a000003 	bne	89fc <get_timer+0x60>
    89ec:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    89f0:	eb001df1 	bl	101bc <__aeabi_uidiv>
    89f4:	e58d0000 	str	r0, [sp]
    89f8:	ea000002 	b	8a08 <get_timer+0x6c>
    89fc:	e1a0000d 	mov	r0, sp
    8a00:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8a04:	eb0011ab 	bl	d0b8 <__div64_32>
    8a08:	e59d0000 	ldr	r0, [sp]
    8a0c:	e0660000 	rsb	r0, r6, r0
    8a10:	e28dd008 	add	sp, sp, #8	; 0x8
    8a14:	e8bd8070 	pop	{r4, r5, r6, pc}
    8a18:	21b00000 	.word	0x21b00000

00008a1c <init_timer>:
    8a1c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8a20:	e3a00003 	mov	r0, #3	; 0x3
    8a24:	e24dd004 	sub	sp, sp, #4	; 0x4
    8a28:	ebffe19c 	bl	10a0 <module_get_clock>
    8a2c:	e59f1030 	ldr	r1, [pc, #48]	; 8a64 <init_timer+0x48>
    8a30:	eb001e63 	bl	103c4 <__aeabi_idiv>
    8a34:	e2401001 	sub	r1, r0, #1	; 0x1
    8a38:	e59f2028 	ldr	r2, [pc, #40]	; 8a68 <init_timer+0x4c>
    8a3c:	e1a01401 	lsl	r1, r1, #8
    8a40:	e3a03000 	mov	r3, #0	; 0x0
    8a44:	e3811001 	orr	r1, r1, #1	; 0x1
    8a48:	e5823208 	str	r3, [r2, #520]
    8a4c:	e1a00003 	mov	r0, r3
    8a50:	e5823200 	str	r3, [r2, #512]
    8a54:	e5823204 	str	r3, [r2, #516]
    8a58:	e5821208 	str	r1, [r2, #520]
    8a5c:	e28dd004 	add	sp, sp, #4	; 0x4
    8a60:	e8bd8000 	pop	{pc}
    8a64:	000f4240 	.word	0x000f4240
    8a68:	21b00000 	.word	0x21b00000

00008a6c <e2p_vs_align>:
    8a6c:	e3a00010 	mov	r0, #16	; 0x10
    8a70:	e12fff1e 	bx	lr

00008a74 <e2p_vs_reset>:
    8a74:	e3a00000 	mov	r0, #0	; 0x0
    8a78:	e12fff1e 	bx	lr

00008a7c <iic_address_num>:
    8a7c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8a80:	e24dd004 	sub	sp, sp, #4	; 0x4
    8a84:	ebffdec4 	bl	59c <boot_info>
    8a88:	e59f3014 	ldr	r3, [pc, #20]	; 8aa4 <iic_address_num+0x28>
    8a8c:	e1a00080 	lsl	r0, r0, #1
    8a90:	e20000ff 	and	r0, r0, #255	; 0xff
    8a94:	e5830000 	str	r0, [r3]
    8a98:	e3a00001 	mov	r0, #1	; 0x1
    8a9c:	e28dd004 	add	sp, sp, #4	; 0x4
    8aa0:	e8bd8000 	pop	{pc}
    8aa4:	0800d248 	.word	0x0800d248

00008aa8 <iic_read_get_address>:
    8aa8:	e1d020b2 	ldrh	r2, [r0, #2]
    8aac:	e590300c 	ldr	r3, [r0, #12]
    8ab0:	e92d4010 	push	{r4, lr}
    8ab4:	e0833002 	add	r3, r3, r2
    8ab8:	e1c030b2 	strh	r3, [r0, #2]
    8abc:	e1a04000 	mov	r4, r0
    8ac0:	ebffffed 	bl	8a7c <iic_address_num>
    8ac4:	e3500000 	cmp	r0, #0	; 0x0
    8ac8:	1a000008 	bne	8af0 <iic_read_get_address+0x48>
    8acc:	e1d430b2 	ldrh	r3, [r4, #2]
    8ad0:	e3530c01 	cmp	r3, #256	; 0x100
    8ad4:	1a000005 	bne	8af0 <iic_read_get_address+0x48>
    8ad8:	e1d430b0 	ldrh	r3, [r4]
    8adc:	e1c400b2 	strh	r0, [r4, #2]
    8ae0:	e2833002 	add	r3, r3, #2	; 0x2
    8ae4:	e1c430b0 	strh	r3, [r4]
    8ae8:	e3a03001 	mov	r3, #1	; 0x1
    8aec:	e1c431b0 	strh	r3, [r4, #16]
    8af0:	ebffffe1 	bl	8a7c <iic_address_num>
    8af4:	e3500000 	cmp	r0, #0	; 0x0
    8af8:	e5941008 	ldr	r1, [r4, #8]
    8afc:	0a000004 	beq	8b14 <iic_read_get_address+0x6c>
    8b00:	e3510000 	cmp	r1, #0	; 0x0
    8b04:	13a03000 	movne	r3, #0	; 0x0
    8b08:	15843008 	strne	r3, [r4, #8]
    8b0c:	1a00000e 	bne	8b4c <iic_read_get_address+0xa4>
    8b10:	ea000010 	b	8b58 <iic_read_get_address+0xb0>
    8b14:	e1d430b2 	ldrh	r3, [r4, #2]
    8b18:	e2632c01 	rsb	r2, r3, #256	; 0x100
    8b1c:	e1520001 	cmp	r2, r1
    8b20:	e5842004 	str	r2, [r4, #4]
    8b24:	2a000005 	bcs	8b40 <iic_read_get_address+0x98>
    8b28:	e0623001 	rsb	r3, r2, r1
    8b2c:	e5843008 	str	r3, [r4, #8]
    8b30:	e3a03001 	mov	r3, #1	; 0x1
    8b34:	e1c431b0 	strh	r3, [r4, #16]
    8b38:	e584200c 	str	r2, [r4, #12]
    8b3c:	ea000003 	b	8b50 <iic_read_get_address+0xa8>
    8b40:	e3510000 	cmp	r1, #0	; 0x0
    8b44:	0a000003 	beq	8b58 <iic_read_get_address+0xb0>
    8b48:	e5840008 	str	r0, [r4, #8]
    8b4c:	e584100c 	str	r1, [r4, #12]
    8b50:	e3a00001 	mov	r0, #1	; 0x1
    8b54:	e8bd8010 	pop	{r4, pc}
    8b58:	e3a00000 	mov	r0, #0	; 0x0
    8b5c:	e8bd8010 	pop	{r4, pc}

00008b60 <iic_write_get_address>:
    8b60:	e1d020b2 	ldrh	r2, [r0, #2]
    8b64:	e590300c 	ldr	r3, [r0, #12]
    8b68:	e92d4010 	push	{r4, lr}
    8b6c:	e0833002 	add	r3, r3, r2
    8b70:	e1c030b2 	strh	r3, [r0, #2]
    8b74:	e1a04000 	mov	r4, r0
    8b78:	ebffffbf 	bl	8a7c <iic_address_num>
    8b7c:	e3500000 	cmp	r0, #0	; 0x0
    8b80:	1a000008 	bne	8ba8 <iic_write_get_address+0x48>
    8b84:	e1d430b2 	ldrh	r3, [r4, #2]
    8b88:	e3530c01 	cmp	r3, #256	; 0x100
    8b8c:	1a000005 	bne	8ba8 <iic_write_get_address+0x48>
    8b90:	e1d430b0 	ldrh	r3, [r4]
    8b94:	e1c400b2 	strh	r0, [r4, #2]
    8b98:	e2833002 	add	r3, r3, #2	; 0x2
    8b9c:	e1c430b0 	strh	r3, [r4]
    8ba0:	e3a03001 	mov	r3, #1	; 0x1
    8ba4:	e1c431b0 	strh	r3, [r4, #16]
    8ba8:	e1d430b2 	ldrh	r3, [r4, #2]
    8bac:	e5940008 	ldr	r0, [r4, #8]
    8bb0:	e203300f 	and	r3, r3, #15	; 0xf
    8bb4:	e2632010 	rsb	r2, r3, #16	; 0x10
    8bb8:	e1520000 	cmp	r2, r0
    8bbc:	e5842004 	str	r2, [r4, #4]
    8bc0:	2a000004 	bcs	8bd8 <iic_write_get_address+0x78>
    8bc4:	e0623000 	rsb	r3, r2, r0
    8bc8:	e5843008 	str	r3, [r4, #8]
    8bcc:	e584200c 	str	r2, [r4, #12]
    8bd0:	e3a00001 	mov	r0, #1	; 0x1
    8bd4:	e8bd8010 	pop	{r4, pc}
    8bd8:	e3500000 	cmp	r0, #0	; 0x0
    8bdc:	13a03000 	movne	r3, #0	; 0x0
    8be0:	1584000c 	strne	r0, [r4, #12]
    8be4:	15843008 	strne	r3, [r4, #8]
    8be8:	13a00001 	movne	r0, #1	; 0x1
    8bec:	e8bd8010 	pop	{r4, pc}

00008bf0 <iic_word_addr_func>:
    8bf0:	e92d4030 	push	{r4, r5, lr}
    8bf4:	e24dd004 	sub	sp, sp, #4	; 0x4
    8bf8:	e1a04000 	mov	r4, r0
    8bfc:	e1a05001 	mov	r5, r1
    8c00:	ebffff9d 	bl	8a7c <iic_address_num>
    8c04:	e3500000 	cmp	r0, #0	; 0x0
    8c08:	16ff0074 	uxthne	r0, r4
    8c0c:	1a00000c 	bne	8c44 <iic_word_addr_func+0x54>
    8c10:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    8c14:	e0051003 	and	r1, r5, r3
    8c18:	e3a020ff 	mov	r2, #255	; 0xff
    8c1c:	e3510000 	cmp	r1, #0	; 0x0
    8c20:	e0040002 	and	r0, r4, r2
    8c24:	aa000005 	bge	8c40 <iic_word_addr_func+0x50>
    8c28:	e3e02000 	mvn	r2, #0	; 0x0
    8c2c:	e0900002 	adds	r0, r0, r2
    8c30:	e3e020ff 	mvn	r2, #255	; 0xff
    8c34:	e1800002 	orr	r0, r0, r2
    8c38:	e3a02001 	mov	r2, #1	; 0x1
    8c3c:	e0900002 	adds	r0, r0, r2
    8c40:	e6ff0070 	uxth	r0, r0
    8c44:	e28dd004 	add	sp, sp, #4	; 0x4
    8c48:	e8bd8030 	pop	{r4, r5, pc}

00008c4c <iic_addr_func>:
    8c4c:	e92d4030 	push	{r4, r5, lr}
    8c50:	e24dd004 	sub	sp, sp, #4	; 0x4
    8c54:	e1a04000 	mov	r4, r0
    8c58:	e1a05001 	mov	r5, r1
    8c5c:	ebffff86 	bl	8a7c <iic_address_num>
    8c60:	e3500000 	cmp	r0, #0	; 0x0
    8c64:	e59f003c 	ldr	r0, [pc, #60]	; 8ca8 <iic_addr_func+0x5c>
    8c68:	11d000b0 	ldrhne	r0, [r0]
    8c6c:	1a00000b 	bne	8ca0 <iic_addr_func+0x54>
    8c70:	e1a02005 	mov	r2, r5
    8c74:	e1a03fc2 	asr	r3, r2, #31
    8c78:	e1a02fc3 	asr	r2, r3, #31
    8c7c:	e20220ff 	and	r2, r2, #255	; 0xff
    8c80:	e3a03000 	mov	r3, #0	; 0x0
    8c84:	e0922004 	adds	r2, r2, r4
    8c88:	e0a33005 	adc	r3, r3, r5
    8c8c:	e5900000 	ldr	r0, [r0]
    8c90:	e1a01422 	lsr	r1, r2, #8
    8c94:	e1811c03 	orr	r1, r1, r3, lsl #24
    8c98:	e1800081 	orr	r0, r0, r1, lsl #1
    8c9c:	e6ff0070 	uxth	r0, r0
    8ca0:	e28dd004 	add	sp, sp, #4	; 0x4
    8ca4:	e8bd8030 	pop	{r4, r5, pc}
    8ca8:	0800d248 	.word	0x0800d248

00008cac <e2p_vs_write>:
    8cac:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8cb0:	e1a01003 	mov	r1, r3
    8cb4:	e24dd024 	sub	sp, sp, #36	; 0x24
    8cb8:	e1a06000 	mov	r6, r0
    8cbc:	e1a00002 	mov	r0, r2
    8cc0:	e1a05003 	mov	r5, r3
    8cc4:	e1a04002 	mov	r4, r2
    8cc8:	ebffffdf 	bl	8c4c <iic_addr_func>
    8ccc:	e1a01005 	mov	r1, r5
    8cd0:	e1cd00bc 	strh	r0, [sp, #12]
    8cd4:	e1a00004 	mov	r0, r4
    8cd8:	ebffffc4 	bl	8bf0 <iic_word_addr_func>
    8cdc:	e3a03000 	mov	r3, #0	; 0x0
    8ce0:	e59d2048 	ldr	r2, [sp, #72]
    8ce4:	e1a05003 	mov	r5, r3
    8ce8:	e58d3018 	str	r3, [sp, #24]
    8cec:	e28dc00c 	add	ip, sp, #12	; 0xc
    8cf0:	e3a03001 	mov	r3, #1	; 0x1
    8cf4:	e1cd00be 	strh	r0, [sp, #14]
    8cf8:	e58d2014 	str	r2, [sp, #20]
    8cfc:	e1cd31bc 	strh	r3, [sp, #28]
    8d00:	e59fb10c 	ldr	fp, [pc, #268]	; 8e14 <e2p_vs_write+0x168>
    8d04:	e3a0a001 	mov	sl, #1	; 0x1
    8d08:	e1a08005 	mov	r8, r5
    8d0c:	e28c9012 	add	r9, ip, #18	; 0x12
    8d10:	ea000037 	b	8df4 <e2p_vs_write+0x148>
    8d14:	ebffff58 	bl	8a7c <iic_address_num>
    8d18:	e3500000 	cmp	r0, #0	; 0x0
    8d1c:	e1dd10be 	ldrh	r1, [sp, #14]
    8d20:	11a03421 	lsrne	r3, r1, #8
    8d24:	15cd301e 	strbne	r3, [sp, #30]
    8d28:	15cd101f 	strbne	r1, [sp, #31]
    8d2c:	13a07002 	movne	r7, #2	; 0x2
    8d30:	1a000005 	bne	8d4c <e2p_vs_write+0xa0>
    8d34:	e6ef1071 	uxtb	r1, r1
    8d38:	e5cd101e 	strb	r1, [sp, #30]
    8d3c:	e59f00d4 	ldr	r0, [pc, #212]	; 8e18 <e2p_vs_write+0x16c>
    8d40:	e1a0e00f 	mov	lr, pc
    8d44:	e59bf000 	ldr	pc, [fp]
    8d48:	e3a07001 	mov	r7, #1	; 0x1
    8d4c:	ebfffedd 	bl	88c8 <get_ticks>
    8d50:	e1dd31bc 	ldrh	r3, [sp, #28]
    8d54:	e1a04000 	mov	r4, r0
    8d58:	e3530000 	cmp	r3, #0	; 0x0
    8d5c:	0a000016 	beq	8dbc <e2p_vs_write+0x110>
    8d60:	e3a0c000 	mov	ip, #0	; 0x0
    8d64:	e5dd200c 	ldrb	r2, [sp, #12]
    8d68:	e1a00008 	mov	r0, r8
    8d6c:	e1a0100a 	mov	r1, sl
    8d70:	e1a03008 	mov	r3, r8
    8d74:	e1cdc1bc 	strh	ip, [sp, #28]
    8d78:	e58da000 	str	sl, [sp]
    8d7c:	e58d8004 	str	r8, [sp, #4]
    8d80:	eb000137 	bl	9264 <iic_init>
    8d84:	e3500000 	cmp	r0, #0	; 0x0
    8d88:	1a00000b 	bne	8dbc <e2p_vs_write+0x110>
    8d8c:	e1a00008 	mov	r0, r8
    8d90:	ea00001d 	b	8e0c <e2p_vs_write+0x160>
    8d94:	ebfffecb 	bl	88c8 <get_ticks>
    8d98:	e1540000 	cmp	r4, r0
    8d9c:	2a000008 	bcs	8dc4 <e2p_vs_write+0x118>
    8da0:	e59f0074 	ldr	r0, [pc, #116]	; 8e1c <e2p_vs_write+0x170>
    8da4:	e1a0e00f 	mov	lr, pc
    8da8:	e59bf000 	ldr	pc, [fp]
    8dac:	e59d3018 	ldr	r3, [sp, #24]
    8db0:	e0853003 	add	r3, r5, r3
    8db4:	e6ff5073 	uxth	r5, r3
    8db8:	ea00000b 	b	8dec <e2p_vs_write+0x140>
    8dbc:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    8dc0:	e2844e12 	add	r4, r4, #288	; 0x120
    8dc4:	e59dc018 	ldr	ip, [sp, #24]
    8dc8:	e1a01009 	mov	r1, r9
    8dcc:	e1a02007 	mov	r2, r7
    8dd0:	e1a03006 	mov	r3, r6
    8dd4:	e3a00000 	mov	r0, #0	; 0x0
    8dd8:	e58dc000 	str	ip, [sp]
    8ddc:	e58da004 	str	sl, [sp, #4]
    8de0:	eb000078 	bl	8fc8 <iicreg_write>
    8de4:	e3500000 	cmp	r0, #0	; 0x0
    8de8:	0affffe9 	beq	8d94 <e2p_vs_write+0xe8>
    8dec:	e59d3018 	ldr	r3, [sp, #24]
    8df0:	e0866003 	add	r6, r6, r3
    8df4:	e28d000c 	add	r0, sp, #12	; 0xc
    8df8:	ebffff58 	bl	8b60 <iic_write_get_address>
    8dfc:	e3500000 	cmp	r0, #0	; 0x0
    8e00:	1affffc3 	bne	8d14 <e2p_vs_write+0x68>
    8e04:	e59d2048 	ldr	r2, [sp, #72]
    8e08:	e0650002 	rsb	r0, r5, r2
    8e0c:	e28dd024 	add	sp, sp, #36	; 0x24
    8e10:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    8e14:	0800d764 	.word	0x0800d764
    8e18:	00013412 	.word	0x00013412
    8e1c:	00013bf4 	.word	0x00013bf4

00008e20 <e2p_vs_read>:
    8e20:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8e24:	e1a01003 	mov	r1, r3
    8e28:	e24dd024 	sub	sp, sp, #36	; 0x24
    8e2c:	e1a06000 	mov	r6, r0
    8e30:	e1a00002 	mov	r0, r2
    8e34:	e1a05003 	mov	r5, r3
    8e38:	e1a04002 	mov	r4, r2
    8e3c:	e59d7048 	ldr	r7, [sp, #72]
    8e40:	ebffff81 	bl	8c4c <iic_addr_func>
    8e44:	e1a01005 	mov	r1, r5
    8e48:	e1cd00bc 	strh	r0, [sp, #12]
    8e4c:	e1a00004 	mov	r0, r4
    8e50:	ebffff66 	bl	8bf0 <iic_word_addr_func>
    8e54:	e28db00c 	add	fp, sp, #12	; 0xc
    8e58:	e3a03000 	mov	r3, #0	; 0x0
    8e5c:	e3a02001 	mov	r2, #1	; 0x1
    8e60:	e1cd00be 	strh	r0, [sp, #14]
    8e64:	e1cd21bc 	strh	r2, [sp, #28]
    8e68:	e58d7014 	str	r7, [sp, #20]
    8e6c:	e58d3018 	str	r3, [sp, #24]
    8e70:	e1a05003 	mov	r5, r3
    8e74:	e3a09001 	mov	r9, #1	; 0x1
    8e78:	e28ba012 	add	sl, fp, #18	; 0x12
    8e7c:	ea00002b 	b	8f30 <e2p_vs_read+0x110>
    8e80:	ebfffefd 	bl	8a7c <iic_address_num>
    8e84:	e1dd20be 	ldrh	r2, [sp, #14]
    8e88:	e3500000 	cmp	r0, #0	; 0x0
    8e8c:	11a03422 	lsrne	r3, r2, #8
    8e90:	15cd301e 	strbne	r3, [sp, #30]
    8e94:	15cd201f 	strbne	r2, [sp, #31]
    8e98:	13a08002 	movne	r8, #2	; 0x2
    8e9c:	05cd201e 	strbeq	r2, [sp, #30]
    8ea0:	03a08001 	moveq	r8, #1	; 0x1
    8ea4:	ebfffe87 	bl	88c8 <get_ticks>
    8ea8:	e1dd31bc 	ldrh	r3, [sp, #28]
    8eac:	e1a04000 	mov	r4, r0
    8eb0:	e3530000 	cmp	r3, #0	; 0x0
    8eb4:	0a000010 	beq	8efc <e2p_vs_read+0xdc>
    8eb8:	e3a0c000 	mov	ip, #0	; 0x0
    8ebc:	e5dd200c 	ldrb	r2, [sp, #12]
    8ec0:	e1a00005 	mov	r0, r5
    8ec4:	e1a01009 	mov	r1, r9
    8ec8:	e1a03005 	mov	r3, r5
    8ecc:	e1cdc1bc 	strh	ip, [sp, #28]
    8ed0:	e58d9000 	str	r9, [sp]
    8ed4:	e58d5004 	str	r5, [sp, #4]
    8ed8:	eb0000e1 	bl	9264 <iic_init>
    8edc:	e3500000 	cmp	r0, #0	; 0x0
    8ee0:	1a000005 	bne	8efc <e2p_vs_read+0xdc>
    8ee4:	e1a07005 	mov	r7, r5
    8ee8:	ea000014 	b	8f40 <e2p_vs_read+0x120>
    8eec:	ebfffe75 	bl	88c8 <get_ticks>
    8ef0:	e1540000 	cmp	r4, r0
    8ef4:	2a000002 	bcs	8f04 <e2p_vs_read+0xe4>
    8ef8:	ea00000a 	b	8f28 <e2p_vs_read+0x108>
    8efc:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    8f00:	e2844e12 	add	r4, r4, #288	; 0x120
    8f04:	e59dc018 	ldr	ip, [sp, #24]
    8f08:	e1a0100a 	mov	r1, sl
    8f0c:	e1a02008 	mov	r2, r8
    8f10:	e1a03006 	mov	r3, r6
    8f14:	e3a00000 	mov	r0, #0	; 0x0
    8f18:	e58dc000 	str	ip, [sp]
    8f1c:	eb000061 	bl	90a8 <iic_read_func>
    8f20:	e3500000 	cmp	r0, #0	; 0x0
    8f24:	0afffff0 	beq	8eec <e2p_vs_read+0xcc>
    8f28:	e59d3018 	ldr	r3, [sp, #24]
    8f2c:	e0866003 	add	r6, r6, r3
    8f30:	e1a0000b 	mov	r0, fp
    8f34:	ebfffedb 	bl	8aa8 <iic_read_get_address>
    8f38:	e3500000 	cmp	r0, #0	; 0x0
    8f3c:	1affffcf 	bne	8e80 <e2p_vs_read+0x60>
    8f40:	e1a00007 	mov	r0, r7
    8f44:	e28dd024 	add	sp, sp, #36	; 0x24
    8f48:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

00008f4c <iicreg_read>:
    8f4c:	e3500001 	cmp	r0, #1	; 0x1
    8f50:	e92d4070 	push	{r4, r5, r6, lr}
    8f54:	83a00000 	movhi	r0, #0	; 0x0
    8f58:	e1a05001 	mov	r5, r1
    8f5c:	e1a04002 	mov	r4, r2
    8f60:	88bd8070 	pophi	{r4, r5, r6, pc}
    8f64:	e59f3058 	ldr	r3, [pc, #88]	; 8fc4 <iicreg_read+0x78>
    8f68:	e5932070 	ldr	r2, [r3, #112]
    8f6c:	e2022008 	and	r2, r2, #8	; 0x8
    8f70:	ea000000 	b	8f78 <iicreg_read+0x2c>
    8f74:	e5931010 	ldr	r1, [r3, #16]
    8f78:	e3520000 	cmp	r2, #0	; 0x0
    8f7c:	1afffffc 	bne	8f74 <iicreg_read+0x28>
    8f80:	e59f103c 	ldr	r1, [pc, #60]	; 8fc4 <iicreg_read+0x78>
    8f84:	e1a00004 	mov	r0, r4
    8f88:	e3a06c01 	mov	r6, #256	; 0x100
    8f8c:	e591c070 	ldr	ip, [r1, #112]
    8f90:	e2503000 	subs	r3, r0, #0	; 0x0
    8f94:	13a03001 	movne	r3, #1	; 0x1
    8f98:	e01330ac 	ands	r3, r3, ip, lsr #1
    8f9c:	15816010 	strne	r6, [r1, #16]
    8fa0:	12400001 	subne	r0, r0, #1	; 0x1
    8fa4:	e31c0008 	tst	ip, #8	; 0x8
    8fa8:	15913010 	ldrne	r3, [r1, #16]
    8fac:	12822001 	addne	r2, r2, #1	; 0x1
    8fb0:	14c53001 	strbne	r3, [r5], #1
    8fb4:	e1520004 	cmp	r2, r4
    8fb8:	1afffff3 	bne	8f8c <iicreg_read+0x40>
    8fbc:	e3a00001 	mov	r0, #1	; 0x1
    8fc0:	e8bd8070 	pop	{r4, r5, r6, pc}
    8fc4:	20300000 	.word	0x20300000

00008fc8 <iicreg_write>:
    8fc8:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    8fcc:	e59d4018 	ldr	r4, [sp, #24]
    8fd0:	e3500001 	cmp	r0, #1	; 0x1
    8fd4:	e1a05001 	mov	r5, r1
    8fd8:	e5dd801c 	ldrb	r8, [sp, #28]
    8fdc:	e1a01003 	mov	r1, r3
    8fe0:	e0847002 	add	r7, r4, r2
    8fe4:	8a00002c 	bhi	909c <iicreg_write+0xd4>
    8fe8:	e59f00b4 	ldr	r0, [pc, #180]	; 90a4 <iicreg_write+0xdc>
    8fec:	e5903034 	ldr	r3, [r0, #52]
    8ff0:	e213c040 	ands	ip, r3, #64	; 0x40
    8ff4:	15903080 	ldrne	r3, [r0, #128]
    8ff8:	15903054 	ldrne	r3, [r0, #84]
    8ffc:	13a00000 	movne	r0, #0	; 0x0
    9000:	18bd81f0 	popne	{r4, r5, r6, r7, r8, pc}
    9004:	e5903070 	ldr	r3, [r0, #112]
    9008:	e3130004 	tst	r3, #4	; 0x4
    900c:	11a06000 	movne	r6, r0
    9010:	11a0000c 	movne	r0, ip
    9014:	11a0c006 	movne	ip, r6
    9018:	1a00000d 	bne	9054 <iicreg_write+0x8c>
    901c:	ea00001e 	b	909c <iicreg_write+0xd4>
    9020:	e5963070 	ldr	r3, [r6, #112]
    9024:	e3130002 	tst	r3, #2	; 0x2
    9028:	0afffffc 	beq	9020 <iicreg_write+0x58>
    902c:	e3520000 	cmp	r2, #0	; 0x0
    9030:	14d53001 	ldrbne	r3, [r5], #1
    9034:	12422001 	subne	r2, r2, #1	; 0x1
    9038:	1a000003 	bne	904c <iicreg_write+0x84>
    903c:	e3540000 	cmp	r4, #0	; 0x0
    9040:	0a000002 	beq	9050 <iicreg_write+0x88>
    9044:	e4d13001 	ldrb	r3, [r1], #1
    9048:	e2444001 	sub	r4, r4, #1	; 0x1
    904c:	e58c3010 	str	r3, [ip, #16]
    9050:	e2800001 	add	r0, r0, #1	; 0x1
    9054:	e1500007 	cmp	r0, r7
    9058:	3afffff0 	bcc	9020 <iicreg_write+0x58>
    905c:	e59f2040 	ldr	r2, [pc, #64]	; 90a4 <iicreg_write+0xdc>
    9060:	e5923070 	ldr	r3, [r2, #112]
    9064:	e3130004 	tst	r3, #4	; 0x4
    9068:	0afffffc 	beq	9060 <iicreg_write+0x98>
    906c:	e3580000 	cmp	r8, #0	; 0x0
    9070:	159f202c 	ldrne	r2, [pc, #44]	; 90a4 <iicreg_write+0xdc>
    9074:	0a000006 	beq	9094 <iicreg_write+0xcc>
    9078:	e5923034 	ldr	r3, [r2, #52]
    907c:	e3130c02 	tst	r3, #512	; 0x200
    9080:	0afffffc 	beq	9078 <iicreg_write+0xb0>
    9084:	e59f3018 	ldr	r3, [pc, #24]	; 90a4 <iicreg_write+0xdc>
    9088:	e3010964 	movw	r0, #6500	; 0x1964
    908c:	e5933060 	ldr	r3, [r3, #96]
    9090:	ebfffe1d 	bl	890c <udelay>
    9094:	e3a00001 	mov	r0, #1	; 0x1
    9098:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    909c:	e3a00000 	mov	r0, #0	; 0x0
    90a0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    90a4:	20300000 	.word	0x20300000

000090a8 <iic_read_func>:
    90a8:	e92d4070 	push	{r4, r5, r6, lr}
    90ac:	e59f4080 	ldr	r4, [pc, #128]	; 9134 <iic_read_func+0x8c>
    90b0:	e1a06003 	mov	r6, r3
    90b4:	e5d43000 	ldrb	r3, [r4]
    90b8:	e24dd008 	sub	sp, sp, #8	; 0x8
    90bc:	e243300a 	sub	r3, r3, #10	; 0xa
    90c0:	e6ef3073 	uxtb	r3, r3
    90c4:	e3530001 	cmp	r3, #1	; 0x1
    90c8:	e1a05000 	mov	r5, r0
    90cc:	9a00000b 	bls	9100 <iic_read_func+0x58>
    90d0:	e3a0e000 	mov	lr, #0	; 0x0
    90d4:	e1a0300e 	mov	r3, lr
    90d8:	e3a0c001 	mov	ip, #1	; 0x1
    90dc:	e58dc004 	str	ip, [sp, #4]
    90e0:	e58de000 	str	lr, [sp]
    90e4:	ebffffb7 	bl	8fc8 <iicreg_write>
    90e8:	e3500000 	cmp	r0, #0	; 0x0
    90ec:	03a0300c 	moveq	r3, #12	; 0xc
    90f0:	05c43000 	strbeq	r3, [r4]
    90f4:	0a00000c 	beq	912c <iic_read_func+0x84>
    90f8:	e3a0300a 	mov	r3, #10	; 0xa
    90fc:	e5c43000 	strb	r3, [r4]
    9100:	e59d2018 	ldr	r2, [sp, #24]
    9104:	e1a00005 	mov	r0, r5
    9108:	e1a01006 	mov	r1, r6
    910c:	ebffff8e 	bl	8f4c <iicreg_read>
    9110:	e59f201c 	ldr	r2, [pc, #28]	; 9134 <iic_read_func+0x8c>
    9114:	e3500000 	cmp	r0, #0	; 0x0
    9118:	13a0300d 	movne	r3, #13	; 0xd
    911c:	03a0300b 	moveq	r3, #11	; 0xb
    9120:	15c23000 	strbne	r3, [r2]
    9124:	13a00001 	movne	r0, #1	; 0x1
    9128:	05c23000 	strbeq	r3, [r2]
    912c:	e28dd008 	add	sp, sp, #8	; 0x8
    9130:	e8bd8070 	pop	{r4, r5, r6, pc}
    9134:	0800d24c 	.word	0x0800d24c

00009138 <iic_writeread>:
    9138:	e92d4070 	push	{r4, r5, r6, lr}
    913c:	e59f4088 	ldr	r4, [pc, #136]	; 91cc <iic_writeread+0x94>
    9140:	e24dd010 	sub	sp, sp, #16	; 0x10
    9144:	e5d43000 	ldrb	r3, [r4]
    9148:	e5cd100f 	strb	r1, [sp, #15]
    914c:	e243300a 	sub	r3, r3, #10	; 0xa
    9150:	e6ef3073 	uxtb	r3, r3
    9154:	e3530001 	cmp	r3, #1	; 0x1
    9158:	e1a06002 	mov	r6, r2
    915c:	e1a05000 	mov	r5, r0
    9160:	9a00000c 	bls	9198 <iic_writeread+0x60>
    9164:	e3a0c000 	mov	ip, #0	; 0x0
    9168:	e1a0300c 	mov	r3, ip
    916c:	e28d100f 	add	r1, sp, #15	; 0xf
    9170:	e3a02001 	mov	r2, #1	; 0x1
    9174:	e58dc000 	str	ip, [sp]
    9178:	e58dc004 	str	ip, [sp, #4]
    917c:	ebffff91 	bl	8fc8 <iicreg_write>
    9180:	e3500000 	cmp	r0, #0	; 0x0
    9184:	03a0300c 	moveq	r3, #12	; 0xc
    9188:	05c43000 	strbeq	r3, [r4]
    918c:	0a00000c 	beq	91c4 <iic_writeread+0x8c>
    9190:	e3a0300a 	mov	r3, #10	; 0xa
    9194:	e5c43000 	strb	r3, [r4]
    9198:	e3a02001 	mov	r2, #1	; 0x1
    919c:	e1a00005 	mov	r0, r5
    91a0:	e1a01006 	mov	r1, r6
    91a4:	ebffff68 	bl	8f4c <iicreg_read>
    91a8:	e59f201c 	ldr	r2, [pc, #28]	; 91cc <iic_writeread+0x94>
    91ac:	e3500000 	cmp	r0, #0	; 0x0
    91b0:	13a0300d 	movne	r3, #13	; 0xd
    91b4:	03a0300b 	moveq	r3, #11	; 0xb
    91b8:	15c23000 	strbne	r3, [r2]
    91bc:	13a00001 	movne	r0, #1	; 0x1
    91c0:	05c23000 	strbeq	r3, [r2]
    91c4:	e28dd010 	add	sp, sp, #16	; 0x10
    91c8:	e8bd8070 	pop	{r4, r5, r6, pc}
    91cc:	0800d24c 	.word	0x0800d24c

000091d0 <iicreg_init>:
    91d0:	e92d4030 	push	{r4, r5, lr}
    91d4:	e1a04000 	mov	r4, r0
    91d8:	e24dd004 	sub	sp, sp, #4	; 0x4
    91dc:	e59f0074 	ldr	r0, [pc, #116]	; 9258 <iicreg_init+0x88>
    91e0:	e1a05002 	mov	r5, r2
    91e4:	ebffdfad 	bl	10a0 <module_get_clock>
    91e8:	e3540001 	cmp	r4, #1	; 0x1
    91ec:	83a00000 	movhi	r0, #0	; 0x0
    91f0:	8a000016 	bhi	9250 <iicreg_init+0x80>
    91f4:	e59f1060 	ldr	r1, [pc, #96]	; 925c <iicreg_init+0x8c>
    91f8:	eb001bef 	bl	101bc <__aeabi_uidiv>
    91fc:	e59f205c 	ldr	r2, [pc, #92]	; 9260 <iicreg_init+0x90>
    9200:	e1a010a0 	lsr	r1, r0, #1
    9204:	e3a0c000 	mov	ip, #0	; 0x0
    9208:	e3a03043 	mov	r3, #67	; 0x43
    920c:	e582c06c 	str	ip, [r2, #108]
    9210:	e5823000 	str	r3, [r2]
    9214:	e5820014 	str	r0, [r2, #20]
    9218:	e5820018 	str	r0, [r2, #24]
    921c:	e58210a0 	str	r1, [r2, #160]
    9220:	e59230a0 	ldr	r3, [r2, #160]
    9224:	e1a000a5 	lsr	r0, r5, #1
    9228:	e3c33c01 	bic	r3, r3, #256	; 0x100
    922c:	e58230a0 	str	r3, [r2, #160]
    9230:	e5923000 	ldr	r3, [r2]
    9234:	e3a01001 	mov	r1, #1	; 0x1
    9238:	e3833020 	orr	r3, r3, #32	; 0x20
    923c:	e5823000 	str	r3, [r2]
    9240:	e582c030 	str	ip, [r2, #48]
    9244:	e5820004 	str	r0, [r2, #4]
    9248:	e582106c 	str	r1, [r2, #108]
    924c:	e1a00001 	mov	r0, r1
    9250:	e28dd004 	add	sp, sp, #4	; 0x4
    9254:	e8bd8030 	pop	{r4, r5, pc}
    9258:	2d00a360 	.word	0x2d00a360
    925c:	0009c400 	.word	0x0009c400
    9260:	20300000 	.word	0x20300000

00009264 <iic_init>:
    9264:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    9268:	e59fc054 	ldr	ip, [pc, #84]	; 92c4 <iic_init+0x60>
    926c:	e24dd004 	sub	sp, sp, #4	; 0x4
    9270:	e3a0e00d 	mov	lr, #13	; 0xd
    9274:	e1a0a000 	mov	sl, r0
    9278:	e59f0048 	ldr	r0, [pc, #72]	; 92c8 <iic_init+0x64>
    927c:	e1a04001 	mov	r4, r1
    9280:	e1a05002 	mov	r5, r2
    9284:	e1a06003 	mov	r6, r3
    9288:	e5cce000 	strb	lr, [ip]
    928c:	e5dd7020 	ldrb	r7, [sp, #32]
    9290:	e5dd8024 	ldrb	r8, [sp, #36]
    9294:	ebffdd22 	bl	724 <module_enable>
    9298:	e3a0000a 	mov	r0, #10	; 0xa
    929c:	ebffdd5c 	bl	814 <pads_init_module>
    92a0:	e1a0000a 	mov	r0, sl
    92a4:	e1a01004 	mov	r1, r4
    92a8:	e1a02005 	mov	r2, r5
    92ac:	e1a03006 	mov	r3, r6
    92b0:	e58d7020 	str	r7, [sp, #32]
    92b4:	e58d8024 	str	r8, [sp, #36]
    92b8:	e28dd004 	add	sp, sp, #4	; 0x4
    92bc:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    92c0:	eaffffc2 	b	91d0 <iicreg_init>
    92c4:	0800d24c 	.word	0x0800d24c
    92c8:	2d010c00 	.word	0x2d010c00

000092cc <spi_set_bytes>:
    92cc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    92d0:	e59f3020 	ldr	r3, [pc, #32]	; 92f8 <spi_set_bytes+0x2c>
    92d4:	e1a01000 	mov	r1, r0
    92d8:	e5830000 	str	r0, [r3]
    92dc:	e24dd004 	sub	sp, sp, #4	; 0x4
    92e0:	e59f0014 	ldr	r0, [pc, #20]	; 92fc <spi_set_bytes+0x30>
    92e4:	e59f3014 	ldr	r3, [pc, #20]	; 9300 <spi_set_bytes+0x34>
    92e8:	e1a0e00f 	mov	lr, pc
    92ec:	e593f000 	ldr	pc, [r3]
    92f0:	e28dd004 	add	sp, sp, #4	; 0x4
    92f4:	e8bd8000 	pop	{pc}
    92f8:	0800d12c 	.word	0x0800d12c
    92fc:	0001341c 	.word	0x0001341c
    9300:	0800d764 	.word	0x0800d764

00009304 <spi_fifo_clear>:
    9304:	e59f2028 	ldr	r2, [pc, #40]	; 9334 <spi_fifo_clear+0x30>
    9308:	e592300c 	ldr	r3, [r2, #12]
    930c:	e3130010 	tst	r3, #16	; 0x10
    9310:	1afffffc 	bne	9308 <spi_fifo_clear+0x4>
    9314:	ea000001 	b	9320 <spi_fifo_clear+0x1c>
    9318:	e5923008 	ldr	r3, [r2, #8]
    931c:	ea000000 	b	9324 <spi_fifo_clear+0x20>
    9320:	e59f200c 	ldr	r2, [pc, #12]	; 9334 <spi_fifo_clear+0x30>
    9324:	e592300c 	ldr	r3, [r2, #12]
    9328:	e3130004 	tst	r3, #4	; 0x4
    932c:	1afffff9 	bne	9318 <spi_fifo_clear+0x14>
    9330:	e12fff1e 	bx	lr
    9334:	20500000 	.word	0x20500000

00009338 <flash_vs_align>:
    9338:	e3a00c01 	mov	r0, #256	; 0x100
    933c:	e12fff1e 	bx	lr

00009340 <spi_exchange>:
    9340:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    9344:	e5905008 	ldr	r5, [r0, #8]
    9348:	e3510001 	cmp	r1, #1	; 0x1
    934c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    9350:	e1a0a000 	mov	sl, r0
    9354:	e5908004 	ldr	r8, [r0, #4]
    9358:	d1a04005 	movle	r4, r5
    935c:	d3a07000 	movle	r7, #0	; 0x0
    9360:	da000020 	ble	93e8 <spi_exchange+0xa8>
    9364:	e280a00c 	add	sl, r0, #12	; 0xc
    9368:	e1a01008 	mov	r1, r8
    936c:	e1a0000d 	mov	r0, sp
    9370:	e1a02005 	mov	r2, r5
    9374:	ebffe021 	bl	1400 <memcpy>
    9378:	e99a0012 	ldmib	sl, {r1, r4}
    937c:	e0843005 	add	r3, r4, r5
    9380:	e3530008 	cmp	r3, #8	; 0x8
    9384:	a3a03008 	movge	r3, #8	; 0x8
    9388:	e6ef6073 	uxtb	r6, r3
    938c:	e3510000 	cmp	r1, #0	; 0x0
    9390:	e1a0700d 	mov	r7, sp
    9394:	059a8000 	ldreq	r8, [sl]
    9398:	00667084 	rsbeq	r7, r6, r4, lsl #1
    939c:	0a000008 	beq	93c4 <spi_exchange+0x84>
    93a0:	e0654006 	rsb	r4, r5, r6
    93a4:	e0870005 	add	r0, r7, r5
    93a8:	e1a02004 	mov	r2, r4
    93ac:	ebffe013 	bl	1400 <memcpy>
    93b0:	e99a000c 	ldmib	sl, {r2, r3}
    93b4:	e0855003 	add	r5, r5, r3
    93b8:	e0828004 	add	r8, r2, r4
    93bc:	e3a07000 	mov	r7, #0	; 0x0
    93c0:	e0664005 	rsb	r4, r6, r5
    93c4:	e3a02000 	mov	r2, #0	; 0x0
    93c8:	e1a0000d 	mov	r0, sp
    93cc:	e59f126c 	ldr	r1, [pc, #620]	; 9640 <spi_exchange+0x300>
    93d0:	ea000002 	b	93e0 <spi_exchange+0xa0>
    93d4:	e7d03002 	ldrb	r3, [r0, r2]
    93d8:	e2822001 	add	r2, r2, #1	; 0x1
    93dc:	e5813008 	str	r3, [r1, #8]
    93e0:	e1520006 	cmp	r2, r6
    93e4:	bafffffa 	blt	93d4 <spi_exchange+0x94>
    93e8:	e59f6250 	ldr	r6, [pc, #592]	; 9640 <spi_exchange+0x300>
    93ec:	e3a0b005 	mov	fp, #5	; 0x5
    93f0:	e5969018 	ldr	r9, [r6, #24]
    93f4:	e3190004 	tst	r9, #4	; 0x4
    93f8:	0a000030 	beq	94c0 <spi_exchange+0x180>
    93fc:	e3550003 	cmp	r5, #3	; 0x3
    9400:	da000005 	ble	941c <spi_exchange+0xdc>
    9404:	e5963008 	ldr	r3, [r6, #8]
    9408:	e2455004 	sub	r5, r5, #4	; 0x4
    940c:	e5963008 	ldr	r3, [r6, #8]
    9410:	e5963008 	ldr	r3, [r6, #8]
    9414:	e5963008 	ldr	r3, [r6, #8]
    9418:	ea000028 	b	94c0 <spi_exchange+0x180>
    941c:	e3550000 	cmp	r5, #0	; 0x0
    9420:	0a000003 	beq	9434 <spi_exchange+0xf4>
    9424:	e2555001 	subs	r5, r5, #1	; 0x1
    9428:	e5963008 	ldr	r3, [r6, #8]
    942c:	1afffffc 	bne	9424 <spi_exchange+0xe4>
    9430:	ea000022 	b	94c0 <spi_exchange+0x180>
    9434:	e59a3000 	ldr	r3, [sl]
    9438:	e3530000 	cmp	r3, #0	; 0x0
    943c:	1a000004 	bne	9454 <spi_exchange+0x114>
    9440:	e59f01fc 	ldr	r0, [pc, #508]	; 9644 <spi_exchange+0x304>
    9444:	e59f31fc 	ldr	r3, [pc, #508]	; 9648 <spi_exchange+0x308>
    9448:	e1a0e00f 	mov	lr, pc
    944c:	e593f000 	ldr	pc, [r3]
    9450:	ea00001a 	b	94c0 <spi_exchange+0x180>
    9454:	e3540003 	cmp	r4, #3	; 0x3
    9458:	da00000e 	ble	9498 <spi_exchange+0x158>
    945c:	e5962008 	ldr	r2, [r6, #8]
    9460:	e1a03008 	mov	r3, r8
    9464:	e4c32001 	strb	r2, [r3], #1
    9468:	e5962008 	ldr	r2, [r6, #8]
    946c:	e2831001 	add	r1, r3, #1	; 0x1
    9470:	e5c82001 	strb	r2, [r8, #1]
    9474:	e5962008 	ldr	r2, [r6, #8]
    9478:	e2544004 	subs	r4, r4, #4	; 0x4
    947c:	e5c32001 	strb	r2, [r3, #1]
    9480:	e5963008 	ldr	r3, [r6, #8]
    9484:	e2812001 	add	r2, r1, #1	; 0x1
    9488:	e5c13001 	strb	r3, [r1, #1]
    948c:	0a00005e 	beq	960c <spi_exchange+0x2cc>
    9490:	e2828001 	add	r8, r2, #1	; 0x1
    9494:	ea000009 	b	94c0 <spi_exchange+0x180>
    9498:	e3540000 	cmp	r4, #0	; 0x0
    949c:	0a00005a 	beq	960c <spi_exchange+0x2cc>
    94a0:	e59f2198 	ldr	r2, [pc, #408]	; 9640 <spi_exchange+0x300>
    94a4:	e1a01005 	mov	r1, r5
    94a8:	e5923008 	ldr	r3, [r2, #8]
    94ac:	e7c83001 	strb	r3, [r8, r1]
    94b0:	e2811001 	add	r1, r1, #1	; 0x1
    94b4:	e1510004 	cmp	r1, r4
    94b8:	1afffffa 	bne	94a8 <spi_exchange+0x168>
    94bc:	ea000052 	b	960c <spi_exchange+0x2cc>
    94c0:	e3190008 	tst	r9, #8	; 0x8
    94c4:	0a00002a 	beq	9574 <spi_exchange+0x234>
    94c8:	e59a3004 	ldr	r3, [sl, #4]
    94cc:	e3530000 	cmp	r3, #0	; 0x0
    94d0:	0a000019 	beq	953c <spi_exchange+0x1fc>
    94d4:	e3540003 	cmp	r4, #3	; 0x3
    94d8:	da00000d 	ble	9514 <spi_exchange+0x1d4>
    94dc:	e1a03008 	mov	r3, r8
    94e0:	e4d32001 	ldrb	r2, [r3], #1
    94e4:	e2544004 	subs	r4, r4, #4	; 0x4
    94e8:	e5862008 	str	r2, [r6, #8]
    94ec:	e5d82001 	ldrb	r2, [r8, #1]
    94f0:	e2831001 	add	r1, r3, #1	; 0x1
    94f4:	e5862008 	str	r2, [r6, #8]
    94f8:	e5d33001 	ldrb	r3, [r3, #1]
    94fc:	e2818002 	add	r8, r1, #2	; 0x2
    9500:	e5863008 	str	r3, [r6, #8]
    9504:	e5d13001 	ldrb	r3, [r1, #1]
    9508:	e5863008 	str	r3, [r6, #8]
    950c:	1a000018 	bne	9574 <spi_exchange+0x234>
    9510:	ea00003d 	b	960c <spi_exchange+0x2cc>
    9514:	e3540000 	cmp	r4, #0	; 0x0
    9518:	0a00003b 	beq	960c <spi_exchange+0x2cc>
    951c:	e59f111c 	ldr	r1, [pc, #284]	; 9640 <spi_exchange+0x300>
    9520:	e3a02000 	mov	r2, #0	; 0x0
    9524:	e7d83002 	ldrb	r3, [r8, r2]
    9528:	e2822001 	add	r2, r2, #1	; 0x1
    952c:	e1520004 	cmp	r2, r4
    9530:	e5813008 	str	r3, [r1, #8]
    9534:	1afffffa 	bne	9524 <spi_exchange+0x1e4>
    9538:	ea000033 	b	960c <spi_exchange+0x2cc>
    953c:	e3570003 	cmp	r7, #3	; 0x3
    9540:	da000005 	ble	955c <spi_exchange+0x21c>
    9544:	e586b008 	str	fp, [r6, #8]
    9548:	e2477004 	sub	r7, r7, #4	; 0x4
    954c:	e586b008 	str	fp, [r6, #8]
    9550:	e586b008 	str	fp, [r6, #8]
    9554:	e586b008 	str	fp, [r6, #8]
    9558:	ea000005 	b	9574 <spi_exchange+0x234>
    955c:	e3570000 	cmp	r7, #0	; 0x0
    9560:	0a000003 	beq	9574 <spi_exchange+0x234>
    9564:	e3a03005 	mov	r3, #5	; 0x5
    9568:	e2577001 	subs	r7, r7, #1	; 0x1
    956c:	e5863008 	str	r3, [r6, #8]
    9570:	1afffffb 	bne	9564 <spi_exchange+0x224>
    9574:	e3190002 	tst	r9, #2	; 0x2
    9578:	0affff9c 	beq	93f0 <spi_exchange+0xb0>
    957c:	ea000009 	b	95a8 <spi_exchange+0x268>
    9580:	e596300c 	ldr	r3, [r6, #12]
    9584:	e2455001 	sub	r5, r5, #1	; 0x1
    9588:	e3130004 	tst	r3, #4	; 0x4
    958c:	15963008 	ldrne	r3, [r6, #8]
    9590:	1a000006 	bne	95b0 <spi_exchange+0x270>
    9594:	e59f00b0 	ldr	r0, [pc, #176]	; 964c <spi_exchange+0x30c>
    9598:	e1a01005 	mov	r1, r5
    959c:	e1a0e00f 	mov	lr, pc
    95a0:	e597f000 	ldr	pc, [r7]
    95a4:	ea000001 	b	95b0 <spi_exchange+0x270>
    95a8:	e59f6090 	ldr	r6, [pc, #144]	; 9640 <spi_exchange+0x300>
    95ac:	e59f7094 	ldr	r7, [pc, #148]	; 9648 <spi_exchange+0x308>
    95b0:	e3550000 	cmp	r5, #0	; 0x0
    95b4:	1afffff1 	bne	9580 <spi_exchange+0x240>
    95b8:	ea00000a 	b	95e8 <spi_exchange+0x2a8>
    95bc:	e595300c 	ldr	r3, [r5, #12]
    95c0:	e2444001 	sub	r4, r4, #1	; 0x1
    95c4:	e3130004 	tst	r3, #4	; 0x4
    95c8:	15953008 	ldrne	r3, [r5, #8]
    95cc:	14c83001 	strbne	r3, [r8], #1
    95d0:	1a000006 	bne	95f0 <spi_exchange+0x2b0>
    95d4:	e59f0074 	ldr	r0, [pc, #116]	; 9650 <spi_exchange+0x310>
    95d8:	e1a01004 	mov	r1, r4
    95dc:	e1a0e00f 	mov	lr, pc
    95e0:	e596f000 	ldr	pc, [r6]
    95e4:	ea000001 	b	95f0 <spi_exchange+0x2b0>
    95e8:	e59f5050 	ldr	r5, [pc, #80]	; 9640 <spi_exchange+0x300>
    95ec:	e59f6054 	ldr	r6, [pc, #84]	; 9648 <spi_exchange+0x308>
    95f0:	e59a3000 	ldr	r3, [sl]
    95f4:	e3530000 	cmp	r3, #0	; 0x0
    95f8:	13540000 	cmpne	r4, #0	; 0x0
    95fc:	1affffee 	bne	95bc <spi_exchange+0x27c>
    9600:	e59f3038 	ldr	r3, [pc, #56]	; 9640 <spi_exchange+0x300>
    9604:	e3a02003 	mov	r2, #3	; 0x3
    9608:	e5832020 	str	r2, [r3, #32]
    960c:	e59f202c 	ldr	r2, [pc, #44]	; 9640 <spi_exchange+0x300>
    9610:	e592300c 	ldr	r3, [r2, #12]
    9614:	e3130010 	tst	r3, #16	; 0x10
    9618:	1afffffc 	bne	9610 <spi_exchange+0x2d0>
    961c:	ea000001 	b	9628 <spi_exchange+0x2e8>
    9620:	e5923008 	ldr	r3, [r2, #8]
    9624:	ea000000 	b	962c <spi_exchange+0x2ec>
    9628:	e59f2010 	ldr	r2, [pc, #16]	; 9640 <spi_exchange+0x300>
    962c:	e592300c 	ldr	r3, [r2, #12]
    9630:	e3130004 	tst	r3, #4	; 0x4
    9634:	1afffff9 	bne	9620 <spi_exchange+0x2e0>
    9638:	e28dd00c 	add	sp, sp, #12	; 0xc
    963c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    9640:	20500000 	.word	0x20500000
    9644:	00013433 	.word	0x00013433
    9648:	0800d764 	.word	0x0800d764
    964c:	0001344a 	.word	0x0001344a
    9650:	00013462 	.word	0x00013462

00009654 <flash_vs_read>:
    9654:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    9658:	e24dd024 	sub	sp, sp, #36	; 0x24
    965c:	e28d4018 	add	r4, sp, #24	; 0x18
    9660:	e2841003 	add	r1, r4, #3	; 0x3
    9664:	e59da040 	ldr	sl, [sp, #64]
    9668:	e58d1004 	str	r1, [sp, #4]
    966c:	e3a01005 	mov	r1, #5	; 0x5
    9670:	e3a0c000 	mov	ip, #0	; 0x0
    9674:	e58d1008 	str	r1, [sp, #8]
    9678:	e2811006 	add	r1, r1, #6	; 0x6
    967c:	e58dc018 	str	ip, [sp, #24]
    9680:	e2845004 	add	r5, r4, #4	; 0x4
    9684:	e58d000c 	str	r0, [sp, #12]
    9688:	e58dc010 	str	ip, [sp, #16]
    968c:	e5cd101b 	strb	r1, [sp, #27]
    9690:	e1a06002 	mov	r6, r2
    9694:	e1a07003 	mov	r7, r3
    9698:	e58dc000 	str	ip, [sp]
    969c:	e1a0400a 	mov	r4, sl
    96a0:	e1a0800d 	mov	r8, sp
    96a4:	ea00000c 	b	96dc <flash_vs_read+0x88>
    96a8:	e3540a01 	cmp	r4, #4096	; 0x1000
    96ac:	b1a03004 	movlt	r3, r4
    96b0:	a3a03a01 	movge	r3, #4096	; 0x1000
    96b4:	e5852000 	str	r2, [r5]
    96b8:	e58d3014 	str	r3, [sp, #20]
    96bc:	ebffff1f 	bl	9340 <spi_exchange>
    96c0:	e59d2014 	ldr	r2, [sp, #20]
    96c4:	e59d300c 	ldr	r3, [sp, #12]
    96c8:	e0966002 	adds	r6, r6, r2
    96cc:	e0833002 	add	r3, r3, r2
    96d0:	e0a77fc2 	adc	r7, r7, r2, asr #31
    96d4:	e58d300c 	str	r3, [sp, #12]
    96d8:	e0624004 	rsb	r4, r2, r4
    96dc:	e1a02c06 	lsl	r2, r6, #24
    96e0:	e2061cff 	and	r1, r6, #65280	; 0xff00
    96e4:	e1822c26 	orr	r2, r2, r6, lsr #24
    96e8:	e1822401 	orr	r2, r2, r1, lsl #8
    96ec:	e20638ff 	and	r3, r6, #16711680	; 0xff0000
    96f0:	e1822423 	orr	r2, r2, r3, lsr #8
    96f4:	e3540000 	cmp	r4, #0	; 0x0
    96f8:	e1a02442 	asr	r2, r2, #8
    96fc:	e1a0000d 	mov	r0, sp
    9700:	e3a01002 	mov	r1, #2	; 0x2
    9704:	1affffe7 	bne	96a8 <flash_vs_read+0x54>
    9708:	e1a0000a 	mov	r0, sl
    970c:	e28dd024 	add	sp, sp, #36	; 0x24
    9710:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00009714 <flash_reset>:
    9714:	e92d4030 	push	{r4, r5, lr}
    9718:	e24dd024 	sub	sp, sp, #36	; 0x24
    971c:	e3a04001 	mov	r4, #1	; 0x1
    9720:	e28dc018 	add	ip, sp, #24	; 0x18
    9724:	e08c3004 	add	r3, ip, r4
    9728:	e3a02000 	mov	r2, #0	; 0x0
    972c:	e58d3010 	str	r3, [sp, #16]
    9730:	e3a03066 	mov	r3, #102	; 0x66
    9734:	e58d2018 	str	r2, [sp, #24]
    9738:	e1a0000d 	mov	r0, sp
    973c:	e1a01004 	mov	r1, r4
    9740:	e5cd3018 	strb	r3, [sp, #24]
    9744:	e3e03066 	mvn	r3, #102	; 0x66
    9748:	e58d200c 	str	r2, [sp, #12]
    974c:	e5cd3019 	strb	r3, [sp, #25]
    9750:	e58d201c 	str	r2, [sp, #28]
    9754:	e88d1004 	stm	sp, {r2, ip}
    9758:	e58d4008 	str	r4, [sp, #8]
    975c:	e58d4014 	str	r4, [sp, #20]
    9760:	ebfffef6 	bl	9340 <spi_exchange>
    9764:	e28d000c 	add	r0, sp, #12	; 0xc
    9768:	e1a01004 	mov	r1, r4
    976c:	ebfffef3 	bl	9340 <spi_exchange>
    9770:	e3a0003c 	mov	r0, #60	; 0x3c
    9774:	e1a0500d 	mov	r5, sp
    9778:	ebfffc63 	bl	890c <udelay>
    977c:	e28dd024 	add	sp, sp, #36	; 0x24
    9780:	e8bd8030 	pop	{r4, r5, pc}

00009784 <flash_poll>:
    9784:	e92d4030 	push	{r4, r5, lr}
    9788:	e24dd024 	sub	sp, sp, #36	; 0x24
    978c:	e28dc018 	add	ip, sp, #24	; 0x18
    9790:	e28c3001 	add	r3, ip, #1	; 0x1
    9794:	e3a02000 	mov	r2, #0	; 0x0
    9798:	e3a01001 	mov	r1, #1	; 0x1
    979c:	e58d300c 	str	r3, [sp, #12]
    97a0:	e3a03005 	mov	r3, #5	; 0x5
    97a4:	e58d2018 	str	r2, [sp, #24]
    97a8:	e58d2010 	str	r2, [sp, #16]
    97ac:	e58d1014 	str	r1, [sp, #20]
    97b0:	e5cd3018 	strb	r3, [sp, #24]
    97b4:	e1a04000 	mov	r4, r0
    97b8:	e58d201c 	str	r2, [sp, #28]
    97bc:	e88d1004 	stm	sp, {r2, ip}
    97c0:	e58d1008 	str	r1, [sp, #8]
    97c4:	e1a0500d 	mov	r5, sp
    97c8:	e1a0000d 	mov	r0, sp
    97cc:	e3a01002 	mov	r1, #2	; 0x2
    97d0:	ebfffeda 	bl	9340 <spi_exchange>
    97d4:	e5dd3019 	ldrb	r3, [sp, #25]
    97d8:	e3540000 	cmp	r4, #0	; 0x0
    97dc:	11a030a3 	lsrne	r3, r3, #1
    97e0:	12233001 	eorne	r3, r3, #1	; 0x1
    97e4:	e2033001 	and	r3, r3, #1	; 0x1
    97e8:	e3530000 	cmp	r3, #0	; 0x0
    97ec:	1afffff5 	bne	97c8 <flash_poll+0x44>
    97f0:	e28dd024 	add	sp, sp, #36	; 0x24
    97f4:	e8bd8030 	pop	{r4, r5, pc}

000097f8 <flash_wren>:
    97f8:	e92d4010 	push	{r4, lr}
    97fc:	e24dd020 	sub	sp, sp, #32	; 0x20
    9800:	e3a04001 	mov	r4, #1	; 0x1
    9804:	e28dc018 	add	ip, sp, #24	; 0x18
    9808:	e08c3004 	add	r3, ip, r4
    980c:	e3a02000 	mov	r2, #0	; 0x0
    9810:	e58d3010 	str	r3, [sp, #16]
    9814:	e3a03006 	mov	r3, #6	; 0x6
    9818:	e58d2018 	str	r2, [sp, #24]
    981c:	e1a0000d 	mov	r0, sp
    9820:	e5cd3018 	strb	r3, [sp, #24]
    9824:	e1a01004 	mov	r1, r4
    9828:	e3a03004 	mov	r3, #4	; 0x4
    982c:	e58d200c 	str	r2, [sp, #12]
    9830:	e5cd3019 	strb	r3, [sp, #25]
    9834:	e58d201c 	str	r2, [sp, #28]
    9838:	e88d1004 	stm	sp, {r2, ip}
    983c:	e58d4008 	str	r4, [sp, #8]
    9840:	e58d4014 	str	r4, [sp, #20]
    9844:	ebfffebd 	bl	9340 <spi_exchange>
    9848:	e1a00004 	mov	r0, r4
    984c:	ebffffcc 	bl	9784 <flash_poll>
    9850:	e28dd020 	add	sp, sp, #32	; 0x20
    9854:	e8bd8010 	pop	{r4, pc}

00009858 <flash_vs_erase>:
    9858:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    985c:	e24dd034 	sub	sp, sp, #52	; 0x34
    9860:	e28d4028 	add	r4, sp, #40	; 0x28
    9864:	e3a06001 	mov	r6, #1	; 0x1
    9868:	e59f109c 	ldr	r1, [pc, #156]	; 990c <flash_vs_erase+0xb4>
    986c:	e3a02008 	mov	r2, #8	; 0x8
    9870:	e1a00004 	mov	r0, r4
    9874:	ebffdee1 	bl	1400 <memcpy>
    9878:	e0843006 	add	r3, r4, r6
    987c:	e2842003 	add	r2, r4, #3	; 0x3
    9880:	e3a05000 	mov	r5, #0	; 0x0
    9884:	e28d7004 	add	r7, sp, #4	; 0x4
    9888:	e58d3008 	str	r3, [sp, #8]
    988c:	e3a03002 	mov	r3, #2	; 0x2
    9890:	e58d300c 	str	r3, [sp, #12]
    9894:	e58d2014 	str	r2, [sp, #20]
    9898:	e58d4020 	str	r4, [sp, #32]
    989c:	e58d5004 	str	r5, [sp, #4]
    98a0:	e58d5010 	str	r5, [sp, #16]
    98a4:	e58d6018 	str	r6, [sp, #24]
    98a8:	e58d501c 	str	r5, [sp, #28]
    98ac:	e58d6024 	str	r6, [sp, #36]
    98b0:	ebffffd0 	bl	97f8 <flash_wren>
    98b4:	e1a01006 	mov	r1, r6
    98b8:	e1a00007 	mov	r0, r7
    98bc:	ebfffe9f 	bl	9340 <spi_exchange>
    98c0:	e1a00005 	mov	r0, r5
    98c4:	ebffffae 	bl	9784 <flash_poll>
    98c8:	e2870018 	add	r0, r7, #24	; 0x18
    98cc:	e1a01006 	mov	r1, r6
    98d0:	ebfffe9a 	bl	9340 <spi_exchange>
    98d4:	e1a01006 	mov	r1, r6
    98d8:	e1a00007 	mov	r0, r7
    98dc:	ebfffe97 	bl	9340 <spi_exchange>
    98e0:	e1a00005 	mov	r0, r5
    98e4:	ebffffa6 	bl	9784 <flash_poll>
    98e8:	ebffffc2 	bl	97f8 <flash_wren>
    98ec:	e1a01006 	mov	r1, r6
    98f0:	e287000c 	add	r0, r7, #12	; 0xc
    98f4:	ebfffe91 	bl	9340 <spi_exchange>
    98f8:	e1a00005 	mov	r0, r5
    98fc:	ebffffa0 	bl	9784 <flash_poll>
    9900:	e1a00005 	mov	r0, r5
    9904:	e28dd034 	add	sp, sp, #52	; 0x34
    9908:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    990c:	000118ba 	.word	0x000118ba

00009910 <flash_vs_write>:
    9910:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    9914:	e3a01004 	mov	r1, #4	; 0x4
    9918:	e24dd020 	sub	sp, sp, #32	; 0x20
    991c:	e3a0c000 	mov	ip, #0	; 0x0
    9920:	e59d9040 	ldr	r9, [sp, #64]
    9924:	e58d1008 	str	r1, [sp, #8]
    9928:	e2811002 	add	r1, r1, #2	; 0x2
    992c:	e58dc018 	str	ip, [sp, #24]
    9930:	e28d701c 	add	r7, sp, #28	; 0x1c
    9934:	e5cd1018 	strb	r1, [sp, #24]
    9938:	e3a01002 	mov	r1, #2	; 0x2
    993c:	e58dc00c 	str	ip, [sp, #12]
    9940:	e58d0010 	str	r0, [sp, #16]
    9944:	e5cd101b 	strb	r1, [sp, #27]
    9948:	e1a04002 	mov	r4, r2
    994c:	e1a05003 	mov	r5, r3
    9950:	e58dc000 	str	ip, [sp]
    9954:	e58d7004 	str	r7, [sp, #4]
    9958:	e1a06009 	mov	r6, r9
    995c:	e59fa080 	ldr	sl, [pc, #128]	; 99e4 <flash_vs_write+0xd4>
    9960:	e1a0800d 	mov	r8, sp
    9964:	ea000011 	b	99b0 <flash_vs_write+0xa0>
    9968:	e59a3000 	ldr	r3, [sl]
    996c:	e5872000 	str	r2, [r7]
    9970:	e1560003 	cmp	r6, r3
    9974:	d58d6014 	strle	r6, [sp, #20]
    9978:	c58d3014 	strgt	r3, [sp, #20]
    997c:	ebffff9d 	bl	97f8 <flash_wren>
    9980:	e1a0000d 	mov	r0, sp
    9984:	e3a01002 	mov	r1, #2	; 0x2
    9988:	ebfffe6c 	bl	9340 <spi_exchange>
    998c:	e3a00000 	mov	r0, #0	; 0x0
    9990:	ebffff7b 	bl	9784 <flash_poll>
    9994:	e59d2014 	ldr	r2, [sp, #20]
    9998:	e59d3010 	ldr	r3, [sp, #16]
    999c:	e0944002 	adds	r4, r4, r2
    99a0:	e0833002 	add	r3, r3, r2
    99a4:	e0a55fc2 	adc	r5, r5, r2, asr #31
    99a8:	e58d3010 	str	r3, [sp, #16]
    99ac:	e0626006 	rsb	r6, r2, r6
    99b0:	e1a02c04 	lsl	r2, r4, #24
    99b4:	e1822c24 	orr	r2, r2, r4, lsr #24
    99b8:	e2041cff 	and	r1, r4, #65280	; 0xff00
    99bc:	e1822401 	orr	r2, r2, r1, lsl #8
    99c0:	e20438ff 	and	r3, r4, #16711680	; 0xff0000
    99c4:	e1822423 	orr	r2, r2, r3, lsr #8
    99c8:	e3c220ff 	bic	r2, r2, #255	; 0xff
    99cc:	e3560000 	cmp	r6, #0	; 0x0
    99d0:	e3822002 	orr	r2, r2, #2	; 0x2
    99d4:	1affffe3 	bne	9968 <flash_vs_write+0x58>
    99d8:	e1a00009 	mov	r0, r9
    99dc:	e28dd020 	add	sp, sp, #32	; 0x20
    99e0:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    99e4:	0800d12c 	.word	0x0800d12c

000099e8 <spi_init_pads>:
    99e8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    99ec:	e3a0000b 	mov	r0, #11	; 0xb
    99f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    99f4:	ebffdb86 	bl	814 <pads_init_module>
    99f8:	e3a00000 	mov	r0, #0	; 0x0
    99fc:	e28dd004 	add	sp, sp, #4	; 0x4
    9a00:	e8bd8000 	pop	{pc}

00009a04 <spi_init_clk>:
    9a04:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    9a08:	e59f00f8 	ldr	r0, [pc, #248]	; 9b08 <spi_init_clk+0x104>
    9a0c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    9a10:	ebffdda2 	bl	10a0 <module_get_clock>
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
    9a40:	eb001a5f 	bl	103c4 <__aeabi_idiv>
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
    9ab0:	eb001a43 	bl	103c4 <__aeabi_idiv>
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
    9b0c:	2d011000 	.word	0x2d011000
    9b10:	004c4b40 	.word	0x004c4b40
    9b14:	0001347a 	.word	0x0001347a
    9b18:	0800d764 	.word	0x0800d764
    9b1c:	000134a8 	.word	0x000134a8
    9b20:	20500000 	.word	0x20500000

00009b24 <spi_sequence_brd_reboot>:
    9b24:	e92d4010 	push	{r4, lr}
    9b28:	ebffdb12 	bl	778 <get_reset_stats>
    9b2c:	e3a01002 	mov	r1, #2	; 0x2
    9b30:	e1a04000 	mov	r4, r0
    9b34:	e3a02001 	mov	r2, #1	; 0x1
    9b38:	e3a0004a 	mov	r0, #74	; 0x4a
    9b3c:	ebffdc40 	bl	c44 <pads_chmod>
    9b40:	e3540000 	cmp	r4, #0	; 0x0
    9b44:	08bd8010 	popeq	{r4, pc}
    9b48:	e3a01002 	mov	r1, #2	; 0x2
    9b4c:	e3a02000 	mov	r2, #0	; 0x0
    9b50:	e3a00077 	mov	r0, #119	; 0x77
    9b54:	ebffdc3a 	bl	c44 <pads_chmod>
    9b58:	e3020710 	movw	r0, #10000	; 0x2710
    9b5c:	ebfffb6a 	bl	890c <udelay>
    9b60:	e3a00077 	mov	r0, #119	; 0x77
    9b64:	e3a01001 	mov	r1, #1	; 0x1
    9b68:	e3a02000 	mov	r2, #0	; 0x0
    9b6c:	ebffdc34 	bl	c44 <pads_chmod>
    9b70:	e3020710 	movw	r0, #10000	; 0x2710
    9b74:	e8bd4010 	pop	{r4, lr}
    9b78:	eafffb63 	b	890c <udelay>

00009b7c <flash_vs_reset>:
    9b7c:	e92d4010 	push	{r4, lr}
    9b80:	ebffffe7 	bl	9b24 <spi_sequence_brd_reboot>
    9b84:	e59f0044 	ldr	r0, [pc, #68]	; 9bd0 <flash_vs_reset+0x54>
    9b88:	ebffdae5 	bl	724 <module_enable>
    9b8c:	ebffff9c 	bl	9a04 <spi_init_clk>
    9b90:	e3a0000b 	mov	r0, #11	; 0xb
    9b94:	ebffdb1e 	bl	814 <pads_init_module>
    9b98:	e59f2034 	ldr	r2, [pc, #52]	; 9bd4 <flash_vs_reset+0x58>
    9b9c:	e592300c 	ldr	r3, [r2, #12]
    9ba0:	e3130010 	tst	r3, #16	; 0x10
    9ba4:	1afffffc 	bne	9b9c <flash_vs_reset+0x20>
    9ba8:	ea000001 	b	9bb4 <flash_vs_reset+0x38>
    9bac:	e5923008 	ldr	r3, [r2, #8]
    9bb0:	ea000000 	b	9bb8 <flash_vs_reset+0x3c>
    9bb4:	e59f2018 	ldr	r2, [pc, #24]	; 9bd4 <flash_vs_reset+0x58>
    9bb8:	e592300c 	ldr	r3, [r2, #12]
    9bbc:	e2134004 	ands	r4, r3, #4	; 0x4
    9bc0:	1afffff9 	bne	9bac <flash_vs_reset+0x30>
    9bc4:	ebfffed2 	bl	9714 <flash_reset>
    9bc8:	e1a00004 	mov	r0, r4
    9bcc:	e8bd8010 	pop	{r4, pc}
    9bd0:	2d011400 	.word	0x2d011400
    9bd4:	20500000 	.word	0x20500000

00009bd8 <ssp_log_open>:
    9bd8:	e59f3008 	ldr	r3, [pc, #8]	; 9be8 <ssp_log_open+0x10>
    9bdc:	e3a02001 	mov	r2, #1	; 0x1
    9be0:	e5832000 	str	r2, [r3]
    9be4:	e12fff1e 	bx	lr
    9be8:	0800d2b0 	.word	0x0800d2b0

00009bec <ssp_log_close>:
    9bec:	e59f3008 	ldr	r3, [pc, #8]	; 9bfc <ssp_log_close+0x10>
    9bf0:	e3a02000 	mov	r2, #0	; 0x0
    9bf4:	e5832000 	str	r2, [r3]
    9bf8:	e12fff1e 	bx	lr
    9bfc:	0800d2b0 	.word	0x0800d2b0

00009c00 <ssp_manager_set_wire>:
    9c00:	e59f3004 	ldr	r3, [pc, #4]	; 9c0c <ssp_manager_set_wire+0xc>
    9c04:	e5830000 	str	r0, [r3]
    9c08:	e12fff1e 	bx	lr
    9c0c:	0800d250 	.word	0x0800d250

00009c10 <ssp_manager_set_rwmode>:
    9c10:	e2403064 	sub	r3, r0, #100	; 0x64
    9c14:	e59f2020 	ldr	r2, [pc, #32]	; 9c3c <ssp_manager_set_rwmode+0x2c>
    9c18:	e3530002 	cmp	r3, #2	; 0x2
    9c1c:	83a03064 	movhi	r3, #100	; 0x64
    9c20:	e5820050 	str	r0, [r2, #80]
    9c24:	85823050 	strhi	r3, [r2, #80]
    9c28:	e3500066 	cmp	r0, #102	; 0x66
    9c2c:	059f3008 	ldreq	r3, [pc, #8]	; 9c3c <ssp_manager_set_rwmode+0x2c>
    9c30:	03a02064 	moveq	r2, #100	; 0x64
    9c34:	05832050 	streq	r2, [r3, #80]
    9c38:	e12fff1e 	bx	lr
    9c3c:	0800d250 	.word	0x0800d250

00009c40 <ssp_manager_init>:
    9c40:	e59f2058 	ldr	r2, [pc, #88]	; 9ca0 <ssp_manager_init+0x60>
    9c44:	e30033ff 	movw	r3, #1023	; 0x3ff
    9c48:	e5823038 	str	r3, [r2, #56]
    9c4c:	e2433e3f 	sub	r3, r3, #1008	; 0x3f0
    9c50:	e5823000 	str	r3, [r2]
    9c54:	e2833011 	add	r3, r3, #17	; 0x11
    9c58:	e5823058 	str	r3, [r2, #88]
    9c5c:	e2833044 	add	r3, r3, #68	; 0x44
    9c60:	e5823050 	str	r3, [r2, #80]
    9c64:	e3a03040 	mov	r3, #64	; 0x40
    9c68:	e3a01000 	mov	r1, #0	; 0x0
    9c6c:	e3a00001 	mov	r0, #1	; 0x1
    9c70:	e3a0cb02 	mov	ip, #2048	; 0x800
    9c74:	e5823030 	str	r3, [r2, #48]
    9c78:	e0833003 	add	r3, r3, r3
    9c7c:	e582000c 	str	r0, [r2, #12]
    9c80:	e582c028 	str	ip, [r2, #40]
    9c84:	e582302c 	str	r3, [r2, #44]
    9c88:	e5821034 	str	r1, [r2, #52]
    9c8c:	e5821004 	str	r1, [r2, #4]
    9c90:	e5821008 	str	r1, [r2, #8]
    9c94:	e582005c 	str	r0, [r2, #92]
    9c98:	e582c044 	str	ip, [r2, #68]
    9c9c:	e12fff1e 	bx	lr
    9ca0:	0800d250 	.word	0x0800d250

00009ca4 <spi_status_is_busy>:
    9ca4:	e59f300c 	ldr	r3, [pc, #12]	; 9cb8 <spi_status_is_busy+0x14>
    9ca8:	e593000c 	ldr	r0, [r3, #12]
    9cac:	e1a00220 	lsr	r0, r0, #4
    9cb0:	e2000001 	and	r0, r0, #1	; 0x1
    9cb4:	e12fff1e 	bx	lr
    9cb8:	20500000 	.word	0x20500000

00009cbc <spi_status_recv_fifo_not_empty>:
    9cbc:	e59f300c 	ldr	r3, [pc, #12]	; 9cd0 <spi_status_recv_fifo_not_empty+0x14>
    9cc0:	e593000c 	ldr	r0, [r3, #12]
    9cc4:	e1a00120 	lsr	r0, r0, #2
    9cc8:	e2000001 	and	r0, r0, #1	; 0x1
    9ccc:	e12fff1e 	bx	lr
    9cd0:	20500000 	.word	0x20500000

00009cd4 <spi_status_trans_fifo_empty>:
    9cd4:	e59f3008 	ldr	r3, [pc, #8]	; 9ce4 <spi_status_trans_fifo_empty+0x10>
    9cd8:	e593000c 	ldr	r0, [r3, #12]
    9cdc:	e2000001 	and	r0, r0, #1	; 0x1
    9ce0:	e12fff1e 	bx	lr
    9ce4:	20500000 	.word	0x20500000

00009ce8 <spi_set_intr_recv_overrun>:
    9ce8:	e59f30b4 	ldr	r3, [pc, #180]	; 9da4 <spi_set_intr_recv_overrun+0xbc>
    9cec:	e92d4030 	push	{r4, r5, lr}
    9cf0:	e5932000 	ldr	r2, [r3]
    9cf4:	e59f30ac 	ldr	r3, [pc, #172]	; 9da8 <spi_set_intr_recv_overrun+0xc0>
    9cf8:	e3520000 	cmp	r2, #0	; 0x0
    9cfc:	e5933014 	ldr	r3, [r3, #20]
    9d00:	e24dd004 	sub	sp, sp, #4	; 0x4
    9d04:	e1a05000 	mov	r5, r0
    9d08:	e3c34001 	bic	r4, r3, #1	; 0x1
    9d0c:	0a000003 	beq	9d20 <spi_set_intr_recv_overrun+0x38>
    9d10:	e59f0094 	ldr	r0, [pc, #148]	; 9dac <spi_set_intr_recv_overrun+0xc4>
    9d14:	e59f3094 	ldr	r3, [pc, #148]	; 9db0 <spi_set_intr_recv_overrun+0xc8>
    9d18:	e1a0e00f 	mov	lr, pc
    9d1c:	e593f000 	ldr	pc, [r3]
    9d20:	e3550000 	cmp	r5, #0	; 0x0
    9d24:	0a000002 	beq	9d34 <spi_set_intr_recv_overrun+0x4c>
    9d28:	e3550001 	cmp	r5, #1	; 0x1
    9d2c:	1a000011 	bne	9d78 <spi_set_intr_recv_overrun+0x90>
    9d30:	ea000005 	b	9d4c <spi_set_intr_recv_overrun+0x64>
    9d34:	e59f3068 	ldr	r3, [pc, #104]	; 9da4 <spi_set_intr_recv_overrun+0xbc>
    9d38:	e5933000 	ldr	r3, [r3]
    9d3c:	e3530000 	cmp	r3, #0	; 0x0
    9d40:	159f006c 	ldrne	r0, [pc, #108]	; 9db4 <spi_set_intr_recv_overrun+0xcc>
    9d44:	0a000011 	beq	9d90 <spi_set_intr_recv_overrun+0xa8>
    9d48:	ea000005 	b	9d64 <spi_set_intr_recv_overrun+0x7c>
    9d4c:	e59f3050 	ldr	r3, [pc, #80]	; 9da4 <spi_set_intr_recv_overrun+0xbc>
    9d50:	e3844001 	orr	r4, r4, #1	; 0x1
    9d54:	e5933000 	ldr	r3, [r3]
    9d58:	e3530000 	cmp	r3, #0	; 0x0
    9d5c:	0a00000b 	beq	9d90 <spi_set_intr_recv_overrun+0xa8>
    9d60:	e59f0050 	ldr	r0, [pc, #80]	; 9db8 <spi_set_intr_recv_overrun+0xd0>
    9d64:	e1a01004 	mov	r1, r4
    9d68:	e59f3040 	ldr	r3, [pc, #64]	; 9db0 <spi_set_intr_recv_overrun+0xc8>
    9d6c:	e1a0e00f 	mov	lr, pc
    9d70:	e593f000 	ldr	pc, [r3]
    9d74:	ea000005 	b	9d90 <spi_set_intr_recv_overrun+0xa8>
    9d78:	e59f003c 	ldr	r0, [pc, #60]	; 9dbc <spi_set_intr_recv_overrun+0xd4>
    9d7c:	e59f302c 	ldr	r3, [pc, #44]	; 9db0 <spi_set_intr_recv_overrun+0xc8>
    9d80:	e1a0e00f 	mov	lr, pc
    9d84:	e593f000 	ldr	pc, [r3]
    9d88:	e3e00000 	mvn	r0, #0	; 0x0
    9d8c:	ea000002 	b	9d9c <spi_set_intr_recv_overrun+0xb4>
    9d90:	e59f3010 	ldr	r3, [pc, #16]	; 9da8 <spi_set_intr_recv_overrun+0xc0>
    9d94:	e3a00000 	mov	r0, #0	; 0x0
    9d98:	e5834014 	str	r4, [r3, #20]
    9d9c:	e28dd004 	add	sp, sp, #4	; 0x4
    9da0:	e8bd8030 	pop	{r4, r5, pc}
    9da4:	0800d2b0 	.word	0x0800d2b0
    9da8:	20500000 	.word	0x20500000
    9dac:	000134cd 	.word	0x000134cd
    9db0:	0800d764 	.word	0x0800d764
    9db4:	0001350c 	.word	0x0001350c
    9db8:	00013549 	.word	0x00013549
    9dbc:	00013588 	.word	0x00013588

00009dc0 <spi_set_intr_recv_timeout>:
    9dc0:	e59f30b4 	ldr	r3, [pc, #180]	; 9e7c <spi_set_intr_recv_timeout+0xbc>
    9dc4:	e92d4030 	push	{r4, r5, lr}
    9dc8:	e5932000 	ldr	r2, [r3]
    9dcc:	e59f30ac 	ldr	r3, [pc, #172]	; 9e80 <spi_set_intr_recv_timeout+0xc0>
    9dd0:	e3520000 	cmp	r2, #0	; 0x0
    9dd4:	e5933014 	ldr	r3, [r3, #20]
    9dd8:	e24dd004 	sub	sp, sp, #4	; 0x4
    9ddc:	e1a05000 	mov	r5, r0
    9de0:	e3c34002 	bic	r4, r3, #2	; 0x2
    9de4:	0a000003 	beq	9df8 <spi_set_intr_recv_timeout+0x38>
    9de8:	e59f0094 	ldr	r0, [pc, #148]	; 9e84 <spi_set_intr_recv_timeout+0xc4>
    9dec:	e59f3094 	ldr	r3, [pc, #148]	; 9e88 <spi_set_intr_recv_timeout+0xc8>
    9df0:	e1a0e00f 	mov	lr, pc
    9df4:	e593f000 	ldr	pc, [r3]
    9df8:	e3550000 	cmp	r5, #0	; 0x0
    9dfc:	0a000002 	beq	9e0c <spi_set_intr_recv_timeout+0x4c>
    9e00:	e3550002 	cmp	r5, #2	; 0x2
    9e04:	1a000011 	bne	9e50 <spi_set_intr_recv_timeout+0x90>
    9e08:	ea000005 	b	9e24 <spi_set_intr_recv_timeout+0x64>
    9e0c:	e59f3068 	ldr	r3, [pc, #104]	; 9e7c <spi_set_intr_recv_timeout+0xbc>
    9e10:	e5933000 	ldr	r3, [r3]
    9e14:	e3530000 	cmp	r3, #0	; 0x0
    9e18:	159f006c 	ldrne	r0, [pc, #108]	; 9e8c <spi_set_intr_recv_timeout+0xcc>
    9e1c:	0a000011 	beq	9e68 <spi_set_intr_recv_timeout+0xa8>
    9e20:	ea000005 	b	9e3c <spi_set_intr_recv_timeout+0x7c>
    9e24:	e59f3050 	ldr	r3, [pc, #80]	; 9e7c <spi_set_intr_recv_timeout+0xbc>
    9e28:	e3844002 	orr	r4, r4, #2	; 0x2
    9e2c:	e5933000 	ldr	r3, [r3]
    9e30:	e3530000 	cmp	r3, #0	; 0x0
    9e34:	0a00000b 	beq	9e68 <spi_set_intr_recv_timeout+0xa8>
    9e38:	e59f0050 	ldr	r0, [pc, #80]	; 9e90 <spi_set_intr_recv_timeout+0xd0>
    9e3c:	e1a01004 	mov	r1, r4
    9e40:	e59f3040 	ldr	r3, [pc, #64]	; 9e88 <spi_set_intr_recv_timeout+0xc8>
    9e44:	e1a0e00f 	mov	lr, pc
    9e48:	e593f000 	ldr	pc, [r3]
    9e4c:	ea000005 	b	9e68 <spi_set_intr_recv_timeout+0xa8>
    9e50:	e59f003c 	ldr	r0, [pc, #60]	; 9e94 <spi_set_intr_recv_timeout+0xd4>
    9e54:	e59f302c 	ldr	r3, [pc, #44]	; 9e88 <spi_set_intr_recv_timeout+0xc8>
    9e58:	e1a0e00f 	mov	lr, pc
    9e5c:	e593f000 	ldr	pc, [r3]
    9e60:	e3e00000 	mvn	r0, #0	; 0x0
    9e64:	ea000002 	b	9e74 <spi_set_intr_recv_timeout+0xb4>
    9e68:	e59f3010 	ldr	r3, [pc, #16]	; 9e80 <spi_set_intr_recv_timeout+0xc0>
    9e6c:	e3a00000 	mov	r0, #0	; 0x0
    9e70:	e5834014 	str	r4, [r3, #20]
    9e74:	e28dd004 	add	sp, sp, #4	; 0x4
    9e78:	e8bd8030 	pop	{r4, r5, pc}
    9e7c:	0800d2b0 	.word	0x0800d2b0
    9e80:	20500000 	.word	0x20500000
    9e84:	000135a9 	.word	0x000135a9
    9e88:	0800d764 	.word	0x0800d764
    9e8c:	000135e8 	.word	0x000135e8
    9e90:	0001362d 	.word	0x0001362d
    9e94:	00013588 	.word	0x00013588

00009e98 <spi_set_intr_recv_fifo>:
    9e98:	e59f30b4 	ldr	r3, [pc, #180]	; 9f54 <spi_set_intr_recv_fifo+0xbc>
    9e9c:	e92d4030 	push	{r4, r5, lr}
    9ea0:	e5932000 	ldr	r2, [r3]
    9ea4:	e59f30ac 	ldr	r3, [pc, #172]	; 9f58 <spi_set_intr_recv_fifo+0xc0>
    9ea8:	e3520000 	cmp	r2, #0	; 0x0
    9eac:	e5933014 	ldr	r3, [r3, #20]
    9eb0:	e24dd004 	sub	sp, sp, #4	; 0x4
    9eb4:	e1a05000 	mov	r5, r0
    9eb8:	e3c34004 	bic	r4, r3, #4	; 0x4
    9ebc:	0a000003 	beq	9ed0 <spi_set_intr_recv_fifo+0x38>
    9ec0:	e59f0094 	ldr	r0, [pc, #148]	; 9f5c <spi_set_intr_recv_fifo+0xc4>
    9ec4:	e59f3094 	ldr	r3, [pc, #148]	; 9f60 <spi_set_intr_recv_fifo+0xc8>
    9ec8:	e1a0e00f 	mov	lr, pc
    9ecc:	e593f000 	ldr	pc, [r3]
    9ed0:	e3550000 	cmp	r5, #0	; 0x0
    9ed4:	0a000002 	beq	9ee4 <spi_set_intr_recv_fifo+0x4c>
    9ed8:	e3550004 	cmp	r5, #4	; 0x4
    9edc:	1a000011 	bne	9f28 <spi_set_intr_recv_fifo+0x90>
    9ee0:	ea000005 	b	9efc <spi_set_intr_recv_fifo+0x64>
    9ee4:	e59f3068 	ldr	r3, [pc, #104]	; 9f54 <spi_set_intr_recv_fifo+0xbc>
    9ee8:	e5933000 	ldr	r3, [r3]
    9eec:	e3530000 	cmp	r3, #0	; 0x0
    9ef0:	159f006c 	ldrne	r0, [pc, #108]	; 9f64 <spi_set_intr_recv_fifo+0xcc>
    9ef4:	0a000011 	beq	9f40 <spi_set_intr_recv_fifo+0xa8>
    9ef8:	ea000005 	b	9f14 <spi_set_intr_recv_fifo+0x7c>
    9efc:	e59f3050 	ldr	r3, [pc, #80]	; 9f54 <spi_set_intr_recv_fifo+0xbc>
    9f00:	e3844004 	orr	r4, r4, #4	; 0x4
    9f04:	e5933000 	ldr	r3, [r3]
    9f08:	e3530000 	cmp	r3, #0	; 0x0
    9f0c:	0a00000b 	beq	9f40 <spi_set_intr_recv_fifo+0xa8>
    9f10:	e59f0050 	ldr	r0, [pc, #80]	; 9f68 <spi_set_intr_recv_fifo+0xd0>
    9f14:	e1a01004 	mov	r1, r4
    9f18:	e59f3040 	ldr	r3, [pc, #64]	; 9f60 <spi_set_intr_recv_fifo+0xc8>
    9f1c:	e1a0e00f 	mov	lr, pc
    9f20:	e593f000 	ldr	pc, [r3]
    9f24:	ea000005 	b	9f40 <spi_set_intr_recv_fifo+0xa8>
    9f28:	e59f003c 	ldr	r0, [pc, #60]	; 9f6c <spi_set_intr_recv_fifo+0xd4>
    9f2c:	e59f302c 	ldr	r3, [pc, #44]	; 9f60 <spi_set_intr_recv_fifo+0xc8>
    9f30:	e1a0e00f 	mov	lr, pc
    9f34:	e593f000 	ldr	pc, [r3]
    9f38:	e3e00000 	mvn	r0, #0	; 0x0
    9f3c:	ea000002 	b	9f4c <spi_set_intr_recv_fifo+0xb4>
    9f40:	e59f3010 	ldr	r3, [pc, #16]	; 9f58 <spi_set_intr_recv_fifo+0xc0>
    9f44:	e3a00000 	mov	r0, #0	; 0x0
    9f48:	e5834014 	str	r4, [r3, #20]
    9f4c:	e28dd004 	add	sp, sp, #4	; 0x4
    9f50:	e8bd8030 	pop	{r4, r5, pc}
    9f54:	0800d2b0 	.word	0x0800d2b0
    9f58:	20500000 	.word	0x20500000
    9f5c:	00013674 	.word	0x00013674
    9f60:	0800d764 	.word	0x0800d764
    9f64:	000136b0 	.word	0x000136b0
    9f68:	000136f2 	.word	0x000136f2
    9f6c:	00013588 	.word	0x00013588

00009f70 <spi_set_intr_transmit_fifo>:
    9f70:	e59f30b4 	ldr	r3, [pc, #180]	; a02c <spi_set_intr_transmit_fifo+0xbc>
    9f74:	e92d4030 	push	{r4, r5, lr}
    9f78:	e5932000 	ldr	r2, [r3]
    9f7c:	e59f30ac 	ldr	r3, [pc, #172]	; a030 <spi_set_intr_transmit_fifo+0xc0>
    9f80:	e3520000 	cmp	r2, #0	; 0x0
    9f84:	e5933014 	ldr	r3, [r3, #20]
    9f88:	e24dd004 	sub	sp, sp, #4	; 0x4
    9f8c:	e1a05000 	mov	r5, r0
    9f90:	e3c34008 	bic	r4, r3, #8	; 0x8
    9f94:	0a000003 	beq	9fa8 <spi_set_intr_transmit_fifo+0x38>
    9f98:	e59f0094 	ldr	r0, [pc, #148]	; a034 <spi_set_intr_transmit_fifo+0xc4>
    9f9c:	e59f3094 	ldr	r3, [pc, #148]	; a038 <spi_set_intr_transmit_fifo+0xc8>
    9fa0:	e1a0e00f 	mov	lr, pc
    9fa4:	e593f000 	ldr	pc, [r3]
    9fa8:	e3550000 	cmp	r5, #0	; 0x0
    9fac:	0a000002 	beq	9fbc <spi_set_intr_transmit_fifo+0x4c>
    9fb0:	e3550008 	cmp	r5, #8	; 0x8
    9fb4:	1a000011 	bne	a000 <spi_set_intr_transmit_fifo+0x90>
    9fb8:	ea000005 	b	9fd4 <spi_set_intr_transmit_fifo+0x64>
    9fbc:	e59f3068 	ldr	r3, [pc, #104]	; a02c <spi_set_intr_transmit_fifo+0xbc>
    9fc0:	e5933000 	ldr	r3, [r3]
    9fc4:	e3530000 	cmp	r3, #0	; 0x0
    9fc8:	159f006c 	ldrne	r0, [pc, #108]	; a03c <spi_set_intr_transmit_fifo+0xcc>
    9fcc:	0a000011 	beq	a018 <spi_set_intr_transmit_fifo+0xa8>
    9fd0:	ea000005 	b	9fec <spi_set_intr_transmit_fifo+0x7c>
    9fd4:	e59f3050 	ldr	r3, [pc, #80]	; a02c <spi_set_intr_transmit_fifo+0xbc>
    9fd8:	e3844008 	orr	r4, r4, #8	; 0x8
    9fdc:	e5933000 	ldr	r3, [r3]
    9fe0:	e3530000 	cmp	r3, #0	; 0x0
    9fe4:	0a00000b 	beq	a018 <spi_set_intr_transmit_fifo+0xa8>
    9fe8:	e59f0050 	ldr	r0, [pc, #80]	; a040 <spi_set_intr_transmit_fifo+0xd0>
    9fec:	e1a01004 	mov	r1, r4
    9ff0:	e59f3040 	ldr	r3, [pc, #64]	; a038 <spi_set_intr_transmit_fifo+0xc8>
    9ff4:	e1a0e00f 	mov	lr, pc
    9ff8:	e593f000 	ldr	pc, [r3]
    9ffc:	ea000005 	b	a018 <spi_set_intr_transmit_fifo+0xa8>
    a000:	e59f003c 	ldr	r0, [pc, #60]	; a044 <spi_set_intr_transmit_fifo+0xd4>
    a004:	e59f302c 	ldr	r3, [pc, #44]	; a038 <spi_set_intr_transmit_fifo+0xc8>
    a008:	e1a0e00f 	mov	lr, pc
    a00c:	e593f000 	ldr	pc, [r3]
    a010:	e3e00000 	mvn	r0, #0	; 0x0
    a014:	ea000002 	b	a024 <spi_set_intr_transmit_fifo+0xb4>
    a018:	e59f3010 	ldr	r3, [pc, #16]	; a030 <spi_set_intr_transmit_fifo+0xc0>
    a01c:	e3a00000 	mov	r0, #0	; 0x0
    a020:	e5834014 	str	r4, [r3, #20]
    a024:	e28dd004 	add	sp, sp, #4	; 0x4
    a028:	e8bd8030 	pop	{r4, r5, pc}
    a02c:	0800d2b0 	.word	0x0800d2b0
    a030:	20500000 	.word	0x20500000
    a034:	00013736 	.word	0x00013736
    a038:	0800d764 	.word	0x0800d764
    a03c:	00013773 	.word	0x00013773
    a040:	000137b6 	.word	0x000137b6
    a044:	00013588 	.word	0x00013588

0000a048 <spi_get_raw_intr_recv_timeout>:
    a048:	e59f300c 	ldr	r3, [pc, #12]	; a05c <spi_get_raw_intr_recv_timeout+0x14>
    a04c:	e5930018 	ldr	r0, [r3, #24]
    a050:	e1a000a0 	lsr	r0, r0, #1
    a054:	e2000001 	and	r0, r0, #1	; 0x1
    a058:	e12fff1e 	bx	lr
    a05c:	20500000 	.word	0x20500000

0000a060 <spi_get_raw_intr_recv_fifo>:
    a060:	e59f300c 	ldr	r3, [pc, #12]	; a074 <spi_get_raw_intr_recv_fifo+0x14>
    a064:	e5930018 	ldr	r0, [r3, #24]
    a068:	e1a00120 	lsr	r0, r0, #2
    a06c:	e2000001 	and	r0, r0, #1	; 0x1
    a070:	e12fff1e 	bx	lr
    a074:	20500000 	.word	0x20500000

0000a078 <spi_get_raw_intr_trans_fifo>:
    a078:	e59f300c 	ldr	r3, [pc, #12]	; a08c <spi_get_raw_intr_trans_fifo+0x14>
    a07c:	e5930018 	ldr	r0, [r3, #24]
    a080:	e1a001a0 	lsr	r0, r0, #3
    a084:	e2000001 	and	r0, r0, #1	; 0x1
    a088:	e12fff1e 	bx	lr
    a08c:	20500000 	.word	0x20500000

0000a090 <spi_clear_intr_recv_overun>:
    a090:	e59f3008 	ldr	r3, [pc, #8]	; a0a0 <spi_clear_intr_recv_overun+0x10>
    a094:	e3a02001 	mov	r2, #1	; 0x1
    a098:	e5832020 	str	r2, [r3, #32]
    a09c:	e12fff1e 	bx	lr
    a0a0:	20500000 	.word	0x20500000

0000a0a4 <spi_clear_intr_recv_timeout>:
    a0a4:	e59f3008 	ldr	r3, [pc, #8]	; a0b4 <spi_clear_intr_recv_timeout+0x10>
    a0a8:	e3a02002 	mov	r2, #2	; 0x2
    a0ac:	e5832020 	str	r2, [r3, #32]
    a0b0:	e12fff1e 	bx	lr
    a0b4:	20500000 	.word	0x20500000

0000a0b8 <spi_clear_recv_fifo>:
    a0b8:	e59f2014 	ldr	r2, [pc, #20]	; a0d4 <spi_clear_recv_fifo+0x1c>
    a0bc:	ea000000 	b	a0c4 <spi_clear_recv_fifo+0xc>
    a0c0:	e5923008 	ldr	r3, [r2, #8]
    a0c4:	e592300c 	ldr	r3, [r2, #12]
    a0c8:	e3130004 	tst	r3, #4	; 0x4
    a0cc:	1afffffb 	bne	a0c0 <spi_clear_recv_fifo+0x8>
    a0d0:	e12fff1e 	bx	lr
    a0d4:	20500000 	.word	0x20500000

0000a0d8 <flash_nand_vs_align>:
    a0d8:	e3a00b02 	mov	r0, #2048	; 0x800
    a0dc:	e12fff1e 	bx	lr

0000a0e0 <spi_transfer_init>:
    a0e0:	e92d4010 	push	{r4, lr}
    a0e4:	e2504000 	subs	r4, r0, #0	; 0x0
    a0e8:	1a000005 	bne	a104 <spi_transfer_init+0x24>
    a0ec:	e59f00b4 	ldr	r0, [pc, #180]	; a1a8 <spi_transfer_init+0xc8>
    a0f0:	e59f30b4 	ldr	r3, [pc, #180]	; a1ac <spi_transfer_init+0xcc>
    a0f4:	e1a0e00f 	mov	lr, pc
    a0f8:	e593f000 	ldr	pc, [r3]
    a0fc:	e3e00001 	mvn	r0, #1	; 0x1
    a100:	e8bd8010 	pop	{r4, pc}
    a104:	e3a01000 	mov	r1, #0	; 0x0
    a108:	e3a0205c 	mov	r2, #92	; 0x5c
    a10c:	ebffdcb3 	bl	13e0 <memset>
    a110:	e3a0200f 	mov	r2, #15	; 0xf
    a114:	e5c42013 	strb	r2, [r4, #19]
    a118:	e3a02002 	mov	r2, #2	; 0x2
    a11c:	e3a03000 	mov	r3, #0	; 0x0
    a120:	e5842030 	str	r2, [r4, #48]
    a124:	e3a02001 	mov	r2, #1	; 0x1
    a128:	e5842034 	str	r2, [r4, #52]
    a12c:	e5c43058 	strb	r3, [r4, #88]
    a130:	e5c43000 	strb	r3, [r4]
    a134:	e5c43001 	strb	r3, [r4, #1]
    a138:	e5c43002 	strb	r3, [r4, #2]
    a13c:	e5c43003 	strb	r3, [r4, #3]
    a140:	e5c43004 	strb	r3, [r4, #4]
    a144:	e5843008 	str	r3, [r4, #8]
    a148:	e5c4300c 	strb	r3, [r4, #12]
    a14c:	e5c4300d 	strb	r3, [r4, #13]
    a150:	e5c4300e 	strb	r3, [r4, #14]
    a154:	e5c4300f 	strb	r3, [r4, #15]
    a158:	e5c43010 	strb	r3, [r4, #16]
    a15c:	e5c43011 	strb	r3, [r4, #17]
    a160:	e5c43012 	strb	r3, [r4, #18]
    a164:	e5843014 	str	r3, [r4, #20]
    a168:	e5843018 	str	r3, [r4, #24]
    a16c:	e584301c 	str	r3, [r4, #28]
    a170:	e5843020 	str	r3, [r4, #32]
    a174:	e5843024 	str	r3, [r4, #36]
    a178:	e5843028 	str	r3, [r4, #40]
    a17c:	e584302c 	str	r3, [r4, #44]
    a180:	e5843038 	str	r3, [r4, #56]
    a184:	e584303c 	str	r3, [r4, #60]
    a188:	e5843040 	str	r3, [r4, #64]
    a18c:	e5843044 	str	r3, [r4, #68]
    a190:	e5843048 	str	r3, [r4, #72]
    a194:	e584304c 	str	r3, [r4, #76]
    a198:	e5843050 	str	r3, [r4, #80]
    a19c:	e5843054 	str	r3, [r4, #84]
    a1a0:	e1a00003 	mov	r0, r3
    a1a4:	e8bd8010 	pop	{r4, pc}
    a1a8:	000137fb 	.word	0x000137fb
    a1ac:	0800d764 	.word	0x0800d764

0000a1b0 <spi_read_write_data>:
    a1b0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a1b4:	e2505000 	subs	r5, r0, #0	; 0x0
    a1b8:	e24dd014 	sub	sp, sp, #20	; 0x14
    a1bc:	03e00001 	mvneq	r0, #1	; 0x1
    a1c0:	0a00010a 	beq	a5f0 <spi_read_write_data+0x440>
    a1c4:	e1a0000d 	mov	r0, sp
    a1c8:	e3a01000 	mov	r1, #0	; 0x0
    a1cc:	e3a02010 	mov	r2, #16	; 0x10
    a1d0:	ebffdc82 	bl	13e0 <memset>
    a1d4:	e1d531d3 	ldrsb	r3, [r5, #19]
    a1d8:	e353000f 	cmp	r3, #15	; 0xf
    a1dc:	0a000006 	beq	a1fc <spi_read_write_data+0x4c>
    a1e0:	ea0000fe 	b	a5e0 <spi_read_write_data+0x430>
    a1e4:	e3510000 	cmp	r1, #0	; 0x0
    a1e8:	0a000007 	beq	a20c <spi_read_write_data+0x5c>
    a1ec:	e59f0404 	ldr	r0, [pc, #1028]	; a5f8 <spi_read_write_data+0x448>
    a1f0:	e1a0e00f 	mov	lr, pc
    a1f4:	e594f000 	ldr	pc, [r4]
    a1f8:	ea000002 	b	a208 <spi_read_write_data+0x58>
    a1fc:	e59f73f8 	ldr	r7, [pc, #1016]	; a5fc <spi_read_write_data+0x44c>
    a200:	e59f63f8 	ldr	r6, [pc, #1016]	; a600 <spi_read_write_data+0x450>
    a204:	e59f43f8 	ldr	r4, [pc, #1016]	; a604 <spi_read_write_data+0x454>
    a208:	e5971000 	ldr	r1, [r7]
    a20c:	e596300c 	ldr	r3, [r6, #12]
    a210:	e3130001 	tst	r3, #1	; 0x1
    a214:	0afffff2 	beq	a1e4 <spi_read_write_data+0x34>
    a218:	ea000001 	b	a224 <spi_read_write_data+0x74>
    a21c:	e5923008 	ldr	r3, [r2, #8]
    a220:	ea000000 	b	a228 <spi_read_write_data+0x78>
    a224:	e59f23d4 	ldr	r2, [pc, #980]	; a600 <spi_read_write_data+0x450>
    a228:	e592300c 	ldr	r3, [r2, #12]
    a22c:	e3130004 	tst	r3, #4	; 0x4
    a230:	1afffff9 	bne	a21c <spi_read_write_data+0x6c>
    a234:	e3510000 	cmp	r1, #0	; 0x0
    a238:	0a000003 	beq	a24c <spi_read_write_data+0x9c>
    a23c:	e59f03c4 	ldr	r0, [pc, #964]	; a608 <spi_read_write_data+0x458>
    a240:	e59f33bc 	ldr	r3, [pc, #956]	; a604 <spi_read_write_data+0x454>
    a244:	e1a0e00f 	mov	lr, pc
    a248:	e593f000 	ldr	pc, [r3]
    a24c:	e1d531d0 	ldrsb	r3, [r5, #16]
    a250:	e1d520d4 	ldrsb	r2, [r5, #4]
    a254:	e5954008 	ldr	r4, [r5, #8]
    a258:	e1d511d2 	ldrsb	r1, [r5, #18]
    a25c:	e0833002 	add	r3, r3, r2
    a260:	e3540000 	cmp	r4, #0	; 0x0
    a264:	e0837001 	add	r7, r3, r1
    a268:	1a00001f 	bne	a2ec <spi_read_write_data+0x13c>
    a26c:	e3a0004a 	mov	r0, #74	; 0x4a
    a270:	e1a01004 	mov	r1, r4
    a274:	ebffdad7 	bl	dd8 <pads_setpin>
    a278:	e1a01004 	mov	r1, r4
    a27c:	e1d520d4 	ldrsb	r2, [r5, #4]
    a280:	e59f0378 	ldr	r0, [pc, #888]	; a600 <spi_read_write_data+0x450>
    a284:	ea000002 	b	a294 <spi_read_write_data+0xe4>
    a288:	e7d53001 	ldrb	r3, [r5, r1]
    a28c:	e2811001 	add	r1, r1, #1	; 0x1
    a290:	e5803008 	str	r3, [r0, #8]
    a294:	e1510002 	cmp	r1, r2
    a298:	bafffffa 	blt	a288 <spi_read_write_data+0xd8>
    a29c:	e59f235c 	ldr	r2, [pc, #860]	; a600 <spi_read_write_data+0x450>
    a2a0:	e592300c 	ldr	r3, [r2, #12]
    a2a4:	e3130010 	tst	r3, #16	; 0x10
    a2a8:	1afffffc 	bne	a2a0 <spi_read_write_data+0xf0>
    a2ac:	e59f234c 	ldr	r2, [pc, #844]	; a600 <spi_read_write_data+0x450>
    a2b0:	e592300c 	ldr	r3, [r2, #12]
    a2b4:	e3130001 	tst	r3, #1	; 0x1
    a2b8:	0afffffc 	beq	a2b0 <spi_read_write_data+0x100>
    a2bc:	ea000001 	b	a2c8 <spi_read_write_data+0x118>
    a2c0:	e5923008 	ldr	r3, [r2, #8]
    a2c4:	ea000000 	b	a2cc <spi_read_write_data+0x11c>
    a2c8:	e59f2330 	ldr	r2, [pc, #816]	; a600 <spi_read_write_data+0x450>
    a2cc:	e592300c 	ldr	r3, [r2, #12]
    a2d0:	e2134004 	ands	r4, r3, #4	; 0x4
    a2d4:	1afffff9 	bne	a2c0 <spi_read_write_data+0x110>
    a2d8:	e3a0004a 	mov	r0, #74	; 0x4a
    a2dc:	e3a01001 	mov	r1, #1	; 0x1
    a2e0:	ebffdabc 	bl	dd8 <pads_setpin>
    a2e4:	e1a00004 	mov	r0, r4
    a2e8:	ea0000c0 	b	a5f0 <spi_read_write_data+0x440>
    a2ec:	e3a02010 	mov	r2, #16	; 0x10
    a2f0:	e3a01000 	mov	r1, #0	; 0x0
    a2f4:	e1a0000d 	mov	r0, sp
    a2f8:	ebffdc38 	bl	13e0 <memset>
    a2fc:	e1a01005 	mov	r1, r5
    a300:	e1d520d4 	ldrsb	r2, [r5, #4]
    a304:	e1a0000d 	mov	r0, sp
    a308:	ebffdc3c 	bl	1400 <memcpy>
    a30c:	e1d500d4 	ldrsb	r0, [r5, #4]
    a310:	e285100c 	add	r1, r5, #12	; 0xc
    a314:	e1d521d0 	ldrsb	r2, [r5, #16]
    a318:	e08d0000 	add	r0, sp, r0
    a31c:	ebffdc37 	bl	1400 <memcpy>
    a320:	e1d530d4 	ldrsb	r3, [r5, #4]
    a324:	e1d501d0 	ldrsb	r0, [r5, #16]
    a328:	e1d521d2 	ldrsb	r2, [r5, #18]
    a32c:	e0800003 	add	r0, r0, r3
    a330:	e2851011 	add	r1, r5, #17	; 0x11
    a334:	e08d0000 	add	r0, sp, r0
    a338:	ebffdc30 	bl	1400 <memcpy>
    a33c:	e3a01000 	mov	r1, #0	; 0x0
    a340:	e3a0004a 	mov	r0, #74	; 0x4a
    a344:	ebffdaa3 	bl	dd8 <pads_setpin>
    a348:	e1a0400d 	mov	r4, sp
    a34c:	e3a02000 	mov	r2, #0	; 0x0
    a350:	e59f12a8 	ldr	r1, [pc, #680]	; a600 <spi_read_write_data+0x450>
    a354:	ea000002 	b	a364 <spi_read_write_data+0x1b4>
    a358:	e19430d2 	ldrsb	r3, [r4, r2]
    a35c:	e2822001 	add	r2, r2, #1	; 0x1
    a360:	e5813008 	str	r3, [r1, #8]
    a364:	e1520007 	cmp	r2, r7
    a368:	bafffffa 	blt	a358 <spi_read_write_data+0x1a8>
    a36c:	e5956018 	ldr	r6, [r5, #24]
    a370:	e595a024 	ldr	sl, [r5, #36]
    a374:	e595801c 	ldr	r8, [r5, #28]
    a378:	e59f4280 	ldr	r4, [pc, #640]	; a600 <spi_read_write_data+0x450>
    a37c:	e3a09000 	mov	r9, #0	; 0x0
    a380:	e3a0b001 	mov	fp, #1	; 0x1
    a384:	e5943018 	ldr	r3, [r4, #24]
    a388:	e3130004 	tst	r3, #4	; 0x4
    a38c:	0a000036 	beq	a46c <spi_read_write_data+0x2bc>
    a390:	e3570003 	cmp	r7, #3	; 0x3
    a394:	da000005 	ble	a3b0 <spi_read_write_data+0x200>
    a398:	e5943008 	ldr	r3, [r4, #8]
    a39c:	e2477004 	sub	r7, r7, #4	; 0x4
    a3a0:	e5943008 	ldr	r3, [r4, #8]
    a3a4:	e5943008 	ldr	r3, [r4, #8]
    a3a8:	e5943008 	ldr	r3, [r4, #8]
    a3ac:	ea00002e 	b	a46c <spi_read_write_data+0x2bc>
    a3b0:	e3570000 	cmp	r7, #0	; 0x0
    a3b4:	da000003 	ble	a3c8 <spi_read_write_data+0x218>
    a3b8:	e2577001 	subs	r7, r7, #1	; 0x1
    a3bc:	e5943008 	ldr	r3, [r4, #8]
    a3c0:	1afffffc 	bne	a3b8 <spi_read_write_data+0x208>
    a3c4:	ea000028 	b	a46c <spi_read_write_data+0x2bc>
    a3c8:	e3560003 	cmp	r6, #3	; 0x3
    a3cc:	da000018 	ble	a434 <spi_read_write_data+0x284>
    a3d0:	e5951014 	ldr	r1, [r5, #20]
    a3d4:	e3510000 	cmp	r1, #0	; 0x0
    a3d8:	0a00000f 	beq	a41c <spi_read_write_data+0x26c>
    a3dc:	e5942008 	ldr	r2, [r4, #8]
    a3e0:	e1a03001 	mov	r3, r1
    a3e4:	e4c32001 	strb	r2, [r3], #1
    a3e8:	e5942008 	ldr	r2, [r4, #8]
    a3ec:	e5853014 	str	r3, [r5, #20]
    a3f0:	e5c12001 	strb	r2, [r1, #1]
    a3f4:	e5941008 	ldr	r1, [r4, #8]
    a3f8:	e2832001 	add	r2, r3, #1	; 0x1
    a3fc:	e5852014 	str	r2, [r5, #20]
    a400:	e5c31001 	strb	r1, [r3, #1]
    a404:	e5941008 	ldr	r1, [r4, #8]
    a408:	e2823001 	add	r3, r2, #1	; 0x1
    a40c:	e2830001 	add	r0, r3, #1	; 0x1
    a410:	e5c21001 	strb	r1, [r2, #1]
    a414:	e5850014 	str	r0, [r5, #20]
    a418:	ea000003 	b	a42c <spi_read_write_data+0x27c>
    a41c:	e5943008 	ldr	r3, [r4, #8]
    a420:	e5943008 	ldr	r3, [r4, #8]
    a424:	e5943008 	ldr	r3, [r4, #8]
    a428:	e5943008 	ldr	r3, [r4, #8]
    a42c:	e2466004 	sub	r6, r6, #4	; 0x4
    a430:	ea00000b 	b	a464 <spi_read_write_data+0x2b4>
    a434:	e3560000 	cmp	r6, #0	; 0x0
    a438:	da000009 	ble	a464 <spi_read_write_data+0x2b4>
    a43c:	e59f11bc 	ldr	r1, [pc, #444]	; a600 <spi_read_write_data+0x450>
    a440:	e5953014 	ldr	r3, [r5, #20]
    a444:	e2532000 	subs	r2, r3, #0	; 0x0
    a448:	15913008 	ldrne	r3, [r1, #8]
    a44c:	05913008 	ldreq	r3, [r1, #8]
    a450:	14c23001 	strbne	r3, [r2], #1
    a454:	15852014 	strne	r2, [r5, #20]
    a458:	e2566001 	subs	r6, r6, #1	; 0x1
    a45c:	1afffff7 	bne	a440 <spi_read_write_data+0x290>
    a460:	ea000056 	b	a5c0 <spi_read_write_data+0x410>
    a464:	e3560000 	cmp	r6, #0	; 0x0
    a468:	0a000054 	beq	a5c0 <spi_read_write_data+0x410>
    a46c:	e5943018 	ldr	r3, [r4, #24]
    a470:	e3130008 	tst	r3, #8	; 0x8
    a474:	0a00002b 	beq	a528 <spi_read_write_data+0x378>
    a478:	e35a0003 	cmp	sl, #3	; 0x3
    a47c:	da000012 	ble	a4cc <spi_read_write_data+0x31c>
    a480:	e5951020 	ldr	r1, [r5, #32]
    a484:	e24aa004 	sub	sl, sl, #4	; 0x4
    a488:	e1a02001 	mov	r2, r1
    a48c:	e4d23001 	ldrb	r3, [r2], #1
    a490:	e5843008 	str	r3, [r4, #8]
    a494:	e5852020 	str	r2, [r5, #32]
    a498:	e5d13001 	ldrb	r3, [r1, #1]
    a49c:	e2821001 	add	r1, r2, #1	; 0x1
    a4a0:	e5843008 	str	r3, [r4, #8]
    a4a4:	e5851020 	str	r1, [r5, #32]
    a4a8:	e5d22001 	ldrb	r2, [r2, #1]
    a4ac:	e2813001 	add	r3, r1, #1	; 0x1
    a4b0:	e5853020 	str	r3, [r5, #32]
    a4b4:	e5842008 	str	r2, [r4, #8]
    a4b8:	e5d12001 	ldrb	r2, [r1, #1]
    a4bc:	e2833001 	add	r3, r3, #1	; 0x1
    a4c0:	e5842008 	str	r2, [r4, #8]
    a4c4:	e5853020 	str	r3, [r5, #32]
    a4c8:	ea000016 	b	a528 <spi_read_write_data+0x378>
    a4cc:	e35a0000 	cmp	sl, #0	; 0x0
    a4d0:	da000006 	ble	a4f0 <spi_read_write_data+0x340>
    a4d4:	e5953020 	ldr	r3, [r5, #32]
    a4d8:	e25aa001 	subs	sl, sl, #1	; 0x1
    a4dc:	e4d32001 	ldrb	r2, [r3], #1
    a4e0:	e5842008 	str	r2, [r4, #8]
    a4e4:	e5853020 	str	r3, [r5, #32]
    a4e8:	1afffff9 	bne	a4d4 <spi_read_write_data+0x324>
    a4ec:	ea00000d 	b	a528 <spi_read_write_data+0x378>
    a4f0:	e3580003 	cmp	r8, #3	; 0x3
    a4f4:	da000005 	ble	a510 <spi_read_write_data+0x360>
    a4f8:	e5849008 	str	r9, [r4, #8]
    a4fc:	e2488004 	sub	r8, r8, #4	; 0x4
    a500:	e5849008 	str	r9, [r4, #8]
    a504:	e5849008 	str	r9, [r4, #8]
    a508:	e5849008 	str	r9, [r4, #8]
    a50c:	ea000005 	b	a528 <spi_read_write_data+0x378>
    a510:	e3580000 	cmp	r8, #0	; 0x0
    a514:	da000003 	ble	a528 <spi_read_write_data+0x378>
    a518:	e3a03000 	mov	r3, #0	; 0x0
    a51c:	e2588001 	subs	r8, r8, #1	; 0x1
    a520:	e5843008 	str	r3, [r4, #8]
    a524:	1afffffb 	bne	a518 <spi_read_write_data+0x368>
    a528:	e5943018 	ldr	r3, [r4, #24]
    a52c:	e3130002 	tst	r3, #2	; 0x2
    a530:	0a000020 	beq	a5b8 <spi_read_write_data+0x408>
    a534:	e594300c 	ldr	r3, [r4, #12]
    a538:	e3130004 	tst	r3, #4	; 0x4
    a53c:	0a00001d 	beq	a5b8 <spi_read_write_data+0x408>
    a540:	ea00000a 	b	a570 <spi_read_write_data+0x3c0>
    a544:	e594300c 	ldr	r3, [r4, #12]
    a548:	e3130004 	tst	r3, #4	; 0x4
    a54c:	15943008 	ldrne	r3, [r4, #8]
    a550:	12477001 	subne	r7, r7, #1	; 0x1
    a554:	1a000005 	bne	a570 <spi_read_write_data+0x3c0>
    a558:	e59f00ac 	ldr	r0, [pc, #172]	; a60c <spi_read_write_data+0x45c>
    a55c:	e1a01007 	mov	r1, r7
    a560:	e59f309c 	ldr	r3, [pc, #156]	; a604 <spi_read_write_data+0x454>
    a564:	e1a0e00f 	mov	lr, pc
    a568:	e593f000 	ldr	pc, [r3]
    a56c:	ea000011 	b	a5b8 <spi_read_write_data+0x408>
    a570:	e3570000 	cmp	r7, #0	; 0x0
    a574:	cafffff2 	bgt	a544 <spi_read_write_data+0x394>
    a578:	ea000009 	b	a5a4 <spi_read_write_data+0x3f4>
    a57c:	e594300c 	ldr	r3, [r4, #12]
    a580:	e3130004 	tst	r3, #4	; 0x4
    a584:	0a00000b 	beq	a5b8 <spi_read_write_data+0x408>
    a588:	e5953014 	ldr	r3, [r5, #20]
    a58c:	e2466001 	sub	r6, r6, #1	; 0x1
    a590:	e2532000 	subs	r2, r3, #0	; 0x0
    a594:	15943008 	ldrne	r3, [r4, #8]
    a598:	05943008 	ldreq	r3, [r4, #8]
    a59c:	14c23001 	strbne	r3, [r2], #1
    a5a0:	15852014 	strne	r2, [r5, #20]
    a5a4:	e3560000 	cmp	r6, #0	; 0x0
    a5a8:	cafffff3 	bgt	a57c <spi_read_write_data+0x3cc>
    a5ac:	e3a03002 	mov	r3, #2	; 0x2
    a5b0:	e5843020 	str	r3, [r4, #32]
    a5b4:	0a000001 	beq	a5c0 <spi_read_write_data+0x410>
    a5b8:	e584b020 	str	fp, [r4, #32]
    a5bc:	eaffff70 	b	a384 <spi_read_write_data+0x1d4>
    a5c0:	e59f2038 	ldr	r2, [pc, #56]	; a600 <spi_read_write_data+0x450>
    a5c4:	e592300c 	ldr	r3, [r2, #12]
    a5c8:	e3130010 	tst	r3, #16	; 0x10
    a5cc:	1afffffc 	bne	a5c4 <spi_read_write_data+0x414>
    a5d0:	e59f2028 	ldr	r2, [pc, #40]	; a600 <spi_read_write_data+0x450>
    a5d4:	e592300c 	ldr	r3, [r2, #12]
    a5d8:	e3130001 	tst	r3, #1	; 0x1
    a5dc:	0afffffc 	beq	a5d4 <spi_read_write_data+0x424>
    a5e0:	e3a0004a 	mov	r0, #74	; 0x4a
    a5e4:	e3a01001 	mov	r1, #1	; 0x1
    a5e8:	ebffd9fa 	bl	dd8 <pads_setpin>
    a5ec:	e3a00000 	mov	r0, #0	; 0x0
    a5f0:	e28dd014 	add	sp, sp, #20	; 0x14
    a5f4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a5f8:	0001381e 	.word	0x0001381e
    a5fc:	0800d2b0 	.word	0x0800d2b0
    a600:	20500000 	.word	0x20500000
    a604:	0800d764 	.word	0x0800d764
    a608:	00013846 	.word	0x00013846
    a60c:	00013867 	.word	0x00013867

0000a610 <flash_set_device_status>:
    a610:	e92d4070 	push	{r4, r5, r6, lr}
    a614:	e24dd060 	sub	sp, sp, #96	; 0x60
    a618:	e1a04000 	mov	r4, r0
    a61c:	e28d0004 	add	r0, sp, #4	; 0x4
    a620:	e1a06001 	mov	r6, r1
    a624:	e1a05002 	mov	r5, r2
    a628:	ebfffeac 	bl	a0e0 <spi_transfer_init>
    a62c:	e59f303c 	ldr	r3, [pc, #60]	; a670 <flash_set_device_status+0x60>
    a630:	e28d0060 	add	r0, sp, #96	; 0x60
    a634:	e5933000 	ldr	r3, [r3]
    a638:	e560405c 	strb	r4, [r0, #-92]!
    a63c:	e353000f 	cmp	r3, #15	; 0xf
    a640:	13a03001 	movne	r3, #1	; 0x1
    a644:	e5cd3017 	strb	r3, [sp, #23]
    a648:	e3a04000 	mov	r4, #0	; 0x0
    a64c:	e3a03003 	mov	r3, #3	; 0x3
    a650:	e5cd3008 	strb	r3, [sp, #8]
    a654:	e5cd5006 	strb	r5, [sp, #6]
    a658:	e5cd6005 	strb	r6, [sp, #5]
    a65c:	e58d400c 	str	r4, [sp, #12]
    a660:	ebfffed2 	bl	a1b0 <spi_read_write_data>
    a664:	e1a00004 	mov	r0, r4
    a668:	e28dd060 	add	sp, sp, #96	; 0x60
    a66c:	e8bd8070 	pop	{r4, r5, r6, pc}
    a670:	0800d250 	.word	0x0800d250

0000a674 <flash_write_enable>:
    a674:	e92d4010 	push	{r4, lr}
    a678:	e24dd060 	sub	sp, sp, #96	; 0x60
    a67c:	e28d0004 	add	r0, sp, #4	; 0x4
    a680:	ebfffe96 	bl	a0e0 <spi_transfer_init>
    a684:	e59f3030 	ldr	r3, [pc, #48]	; a6bc <flash_write_enable+0x48>
    a688:	e28d0060 	add	r0, sp, #96	; 0x60
    a68c:	e5933000 	ldr	r3, [r3]
    a690:	e3a04000 	mov	r4, #0	; 0x0
    a694:	e5cd3017 	strb	r3, [sp, #23]
    a698:	e3a03006 	mov	r3, #6	; 0x6
    a69c:	e560305c 	strb	r3, [r0, #-92]!
    a6a0:	e3a03001 	mov	r3, #1	; 0x1
    a6a4:	e5cd3008 	strb	r3, [sp, #8]
    a6a8:	e58d400c 	str	r4, [sp, #12]
    a6ac:	ebfffebf 	bl	a1b0 <spi_read_write_data>
    a6b0:	e1a00004 	mov	r0, r4
    a6b4:	e28dd060 	add	sp, sp, #96	; 0x60
    a6b8:	e8bd8010 	pop	{r4, pc}
    a6bc:	0800d250 	.word	0x0800d250

0000a6c0 <nand_flash_reset>:
    a6c0:	e92d4010 	push	{r4, lr}
    a6c4:	e24dd060 	sub	sp, sp, #96	; 0x60
    a6c8:	e28d4004 	add	r4, sp, #4	; 0x4
    a6cc:	e1a00004 	mov	r0, r4
    a6d0:	ebfffe82 	bl	a0e0 <spi_transfer_init>
    a6d4:	e3a03001 	mov	r3, #1	; 0x1
    a6d8:	e5cd3008 	strb	r3, [sp, #8]
    a6dc:	e3a03000 	mov	r3, #0	; 0x0
    a6e0:	e58d300c 	str	r3, [sp, #12]
    a6e4:	e1a00004 	mov	r0, r4
    a6e8:	e283300f 	add	r3, r3, #15	; 0xf
    a6ec:	e3e04000 	mvn	r4, #0	; 0x0
    a6f0:	e5cd3017 	strb	r3, [sp, #23]
    a6f4:	e5cd4004 	strb	r4, [sp, #4]
    a6f8:	ebfffeac 	bl	a1b0 <spi_read_write_data>
    a6fc:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    a700:	ebfff881 	bl	890c <udelay>
    a704:	e1a00004 	mov	r0, r4
    a708:	e28dd060 	add	sp, sp, #96	; 0x60
    a70c:	e8bd8010 	pop	{r4, pc}

0000a710 <flash_wait_device_status>:
    a710:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    a714:	e2514000 	subs	r4, r1, #0	; 0x0
    a718:	e24dd064 	sub	sp, sp, #100	; 0x64
    a71c:	e1a03000 	mov	r3, r0
    a720:	aa000005 	bge	a73c <flash_wait_device_status+0x2c>
    a724:	e59f0364 	ldr	r0, [pc, #868]	; aa90 <flash_wait_device_status+0x380>
    a728:	e59f3364 	ldr	r3, [pc, #868]	; aa94 <flash_wait_device_status+0x384>
    a72c:	e1a0e00f 	mov	lr, pc
    a730:	e593f000 	ldr	pc, [r3]
    a734:	e3e00001 	mvn	r0, #1	; 0x1
    a738:	ea0000d2 	b	aa88 <flash_wait_device_status+0x378>
    a73c:	e1a0000d 	mov	r0, sp
    a740:	e6ef5073 	uxtb	r5, r3
    a744:	ebfffe65 	bl	a0e0 <spi_transfer_init>
    a748:	e3a03002 	mov	r3, #2	; 0x2
    a74c:	e58d3008 	str	r3, [sp, #8]
    a750:	e28d305c 	add	r3, sp, #92	; 0x5c
    a754:	e3a02001 	mov	r2, #1	; 0x1
    a758:	e58d3014 	str	r3, [sp, #20]
    a75c:	e2441001 	sub	r1, r4, #1	; 0x1
    a760:	e3a0300f 	mov	r3, #15	; 0xf
    a764:	e58d201c 	str	r2, [sp, #28]
    a768:	e5cd3013 	strb	r3, [sp, #19]
    a76c:	e5cd5000 	strb	r5, [sp]
    a770:	e5cd2004 	strb	r2, [sp, #4]
    a774:	e58d2018 	str	r2, [sp, #24]
    a778:	e1510003 	cmp	r1, r3
    a77c:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    a780:	ea0000b6 	b	aa60 <flash_wait_device_status+0x350>
    a784:	0000a7c4 	.word	0x0000a7c4
    a788:	0000aa60 	.word	0x0000aa60
    a78c:	0000aa60 	.word	0x0000aa60
    a790:	0000aa60 	.word	0x0000aa60
    a794:	0000aa60 	.word	0x0000aa60
    a798:	0000a974 	.word	0x0000a974
    a79c:	0000a9f0 	.word	0x0000a9f0
    a7a0:	0000a848 	.word	0x0000a848
    a7a4:	0000a8b0 	.word	0x0000a8b0
    a7a8:	0000aa60 	.word	0x0000aa60
    a7ac:	0000aa60 	.word	0x0000aa60
    a7b0:	0000aa60 	.word	0x0000aa60
    a7b4:	0000aa60 	.word	0x0000aa60
    a7b8:	0000aa60 	.word	0x0000aa60
    a7bc:	0000aa60 	.word	0x0000aa60
    a7c0:	0000a908 	.word	0x0000a908
    a7c4:	e59f72cc 	ldr	r7, [pc, #716]	; aa98 <flash_wait_device_status+0x388>
    a7c8:	e59f62c4 	ldr	r6, [pc, #708]	; aa94 <flash_wait_device_status+0x384>
    a7cc:	e28d405c 	add	r4, sp, #92	; 0x5c
    a7d0:	e1a0500d 	mov	r5, sp
    a7d4:	e3a01000 	mov	r1, #0	; 0x0
    a7d8:	e3a02002 	mov	r2, #2	; 0x2
    a7dc:	e1a00004 	mov	r0, r4
    a7e0:	ebffdafe 	bl	13e0 <memset>
    a7e4:	e1a0000d 	mov	r0, sp
    a7e8:	e58d4014 	str	r4, [sp, #20]
    a7ec:	ebfffe6f 	bl	a1b0 <spi_read_write_data>
    a7f0:	e5dd105c 	ldrb	r1, [sp, #92]
    a7f4:	e3110002 	tst	r1, #2	; 0x2
    a7f8:	0a000007 	beq	a81c <flash_wait_device_status+0x10c>
    a7fc:	e3110001 	tst	r1, #1	; 0x1
    a800:	1a000005 	bne	a81c <flash_wait_device_status+0x10c>
    a804:	e59f328c 	ldr	r3, [pc, #652]	; aa98 <flash_wait_device_status+0x388>
    a808:	e5933000 	ldr	r3, [r3]
    a80c:	e3530000 	cmp	r3, #0	; 0x0
    a810:	159f0284 	ldrne	r0, [pc, #644]	; aa9c <flash_wait_device_status+0x38c>
    a814:	0a000091 	beq	aa60 <flash_wait_device_status+0x350>
    a818:	ea000020 	b	a8a0 <flash_wait_device_status+0x190>
    a81c:	ebffff94 	bl	a674 <flash_write_enable>
    a820:	e5973000 	ldr	r3, [r7]
    a824:	e3530000 	cmp	r3, #0	; 0x0
    a828:	0a000003 	beq	a83c <flash_wait_device_status+0x12c>
    a82c:	e5dd105c 	ldrb	r1, [sp, #92]
    a830:	e59f0268 	ldr	r0, [pc, #616]	; aaa0 <flash_wait_device_status+0x390>
    a834:	e1a0e00f 	mov	lr, pc
    a838:	e596f000 	ldr	pc, [r6]
    a83c:	e3a00032 	mov	r0, #50	; 0x32
    a840:	ebfff831 	bl	890c <udelay>
    a844:	eaffffe2 	b	a7d4 <flash_wait_device_status+0xc4>
    a848:	e3a0c002 	mov	ip, #2	; 0x2
    a84c:	e28d405c 	add	r4, sp, #92	; 0x5c
    a850:	e3e0305f 	mvn	r3, #95	; 0x5f
    a854:	e1a0200c 	mov	r2, ip
    a858:	e3a01000 	mov	r1, #0	; 0x0
    a85c:	e1a00004 	mov	r0, r4
    a860:	e5cd3001 	strb	r3, [sp, #1]
    a864:	e5cdc004 	strb	ip, [sp, #4]
    a868:	e58dc008 	str	ip, [sp, #8]
    a86c:	e5cd5000 	strb	r5, [sp]
    a870:	ebffdada 	bl	13e0 <memset>
    a874:	e3a03001 	mov	r3, #1	; 0x1
    a878:	e1a0000d 	mov	r0, sp
    a87c:	e58d3018 	str	r3, [sp, #24]
    a880:	e58d4014 	str	r4, [sp, #20]
    a884:	ebfffe49 	bl	a1b0 <spi_read_write_data>
    a888:	e59f3208 	ldr	r3, [pc, #520]	; aa98 <flash_wait_device_status+0x388>
    a88c:	e5933000 	ldr	r3, [r3]
    a890:	e3530000 	cmp	r3, #0	; 0x0
    a894:	0a000071 	beq	aa60 <flash_wait_device_status+0x350>
    a898:	e59f0204 	ldr	r0, [pc, #516]	; aaa4 <flash_wait_device_status+0x394>
    a89c:	e5dd105c 	ldrb	r1, [sp, #92]
    a8a0:	e59f31ec 	ldr	r3, [pc, #492]	; aa94 <flash_wait_device_status+0x384>
    a8a4:	e1a0e00f 	mov	lr, pc
    a8a8:	e593f000 	ldr	pc, [r3]
    a8ac:	ea00006b 	b	aa60 <flash_wait_device_status+0x350>
    a8b0:	e3a0c002 	mov	ip, #2	; 0x2
    a8b4:	e28d405c 	add	r4, sp, #92	; 0x5c
    a8b8:	e3e0304f 	mvn	r3, #79	; 0x4f
    a8bc:	e1a0200c 	mov	r2, ip
    a8c0:	e3a01000 	mov	r1, #0	; 0x0
    a8c4:	e1a00004 	mov	r0, r4
    a8c8:	e5cd3001 	strb	r3, [sp, #1]
    a8cc:	e5cdc004 	strb	ip, [sp, #4]
    a8d0:	e58dc008 	str	ip, [sp, #8]
    a8d4:	e5cd5000 	strb	r5, [sp]
    a8d8:	ebffdac0 	bl	13e0 <memset>
    a8dc:	e3a03001 	mov	r3, #1	; 0x1
    a8e0:	e1a0000d 	mov	r0, sp
    a8e4:	e58d3018 	str	r3, [sp, #24]
    a8e8:	e58d4014 	str	r4, [sp, #20]
    a8ec:	ebfffe2f 	bl	a1b0 <spi_read_write_data>
    a8f0:	e59f31a0 	ldr	r3, [pc, #416]	; aa98 <flash_wait_device_status+0x388>
    a8f4:	e5933000 	ldr	r3, [r3]
    a8f8:	e3530000 	cmp	r3, #0	; 0x0
    a8fc:	159f01a4 	ldrne	r0, [pc, #420]	; aaa8 <flash_wait_device_status+0x398>
    a900:	0a000019 	beq	a96c <flash_wait_device_status+0x25c>
    a904:	ea000014 	b	a95c <flash_wait_device_status+0x24c>
    a908:	e3a0c002 	mov	ip, #2	; 0x2
    a90c:	e28d405c 	add	r4, sp, #92	; 0x5c
    a910:	e3e0303f 	mvn	r3, #63	; 0x3f
    a914:	e1a0200c 	mov	r2, ip
    a918:	e3a01000 	mov	r1, #0	; 0x0
    a91c:	e1a00004 	mov	r0, r4
    a920:	e5cd3001 	strb	r3, [sp, #1]
    a924:	e5cdc004 	strb	ip, [sp, #4]
    a928:	e58dc008 	str	ip, [sp, #8]
    a92c:	e5cd5000 	strb	r5, [sp]
    a930:	ebffdaaa 	bl	13e0 <memset>
    a934:	e3a03001 	mov	r3, #1	; 0x1
    a938:	e1a0000d 	mov	r0, sp
    a93c:	e58d3018 	str	r3, [sp, #24]
    a940:	e58d4014 	str	r4, [sp, #20]
    a944:	ebfffe19 	bl	a1b0 <spi_read_write_data>
    a948:	e59f3148 	ldr	r3, [pc, #328]	; aa98 <flash_wait_device_status+0x388>
    a94c:	e5933000 	ldr	r3, [r3]
    a950:	e3530000 	cmp	r3, #0	; 0x0
    a954:	0a000004 	beq	a96c <flash_wait_device_status+0x25c>
    a958:	e59f014c 	ldr	r0, [pc, #332]	; aaac <flash_wait_device_status+0x39c>
    a95c:	e5dd105c 	ldrb	r1, [sp, #92]
    a960:	e59f312c 	ldr	r3, [pc, #300]	; aa94 <flash_wait_device_status+0x384>
    a964:	e1a0e00f 	mov	lr, pc
    a968:	e593f000 	ldr	pc, [r3]
    a96c:	e5dd005c 	ldrb	r0, [sp, #92]
    a970:	ea000044 	b	aa88 <flash_wait_device_status+0x378>
    a974:	e3a02002 	mov	r2, #2	; 0x2
    a978:	e3e0303f 	mvn	r3, #63	; 0x3f
    a97c:	e5cd3001 	strb	r3, [sp, #1]
    a980:	e58d2008 	str	r2, [sp, #8]
    a984:	e5cd5000 	strb	r5, [sp]
    a988:	e5cd2004 	strb	r2, [sp, #4]
    a98c:	e3a04000 	mov	r4, #0	; 0x0
    a990:	e28d605c 	add	r6, sp, #92	; 0x5c
    a994:	e1a0700d 	mov	r7, sp
    a998:	e3a01000 	mov	r1, #0	; 0x0
    a99c:	e3a02002 	mov	r2, #2	; 0x2
    a9a0:	e1a00006 	mov	r0, r6
    a9a4:	ebffda8d 	bl	13e0 <memset>
    a9a8:	e1a0000d 	mov	r0, sp
    a9ac:	e58d6014 	str	r6, [sp, #20]
    a9b0:	ebfffdfe 	bl	a1b0 <spi_read_write_data>
    a9b4:	e5dd305c 	ldrb	r3, [sp, #92]
    a9b8:	e2844001 	add	r4, r4, #1	; 0x1
    a9bc:	e3130001 	tst	r3, #1	; 0x1
    a9c0:	e3a0001e 	mov	r0, #30	; 0x1e
    a9c4:	0a000009 	beq	a9f0 <flash_wait_device_status+0x2e0>
    a9c8:	ebfff7cf 	bl	890c <udelay>
    a9cc:	e3540064 	cmp	r4, #100	; 0x64
    a9d0:	1afffff0 	bne	a998 <flash_wait_device_status+0x288>
    a9d4:	e59f00d4 	ldr	r0, [pc, #212]	; aab0 <flash_wait_device_status+0x3a0>
    a9d8:	e5dd105c 	ldrb	r1, [sp, #92]
    a9dc:	e59f30b0 	ldr	r3, [pc, #176]	; aa94 <flash_wait_device_status+0x384>
    a9e0:	e1a0e00f 	mov	lr, pc
    a9e4:	e593f000 	ldr	pc, [r3]
    a9e8:	e3e00000 	mvn	r0, #0	; 0x0
    a9ec:	ea000025 	b	aa88 <flash_wait_device_status+0x378>
    a9f0:	e3a02002 	mov	r2, #2	; 0x2
    a9f4:	e3e0303f 	mvn	r3, #63	; 0x3f
    a9f8:	e59f7098 	ldr	r7, [pc, #152]	; aa98 <flash_wait_device_status+0x388>
    a9fc:	e59f6090 	ldr	r6, [pc, #144]	; aa94 <flash_wait_device_status+0x384>
    aa00:	e5cd5000 	strb	r5, [sp]
    aa04:	e5cd3001 	strb	r3, [sp, #1]
    aa08:	e58d2008 	str	r2, [sp, #8]
    aa0c:	e5cd2004 	strb	r2, [sp, #4]
    aa10:	e28d405c 	add	r4, sp, #92	; 0x5c
    aa14:	e1a0500d 	mov	r5, sp
    aa18:	e3a01000 	mov	r1, #0	; 0x0
    aa1c:	e3a02002 	mov	r2, #2	; 0x2
    aa20:	e1a00004 	mov	r0, r4
    aa24:	ebffda6d 	bl	13e0 <memset>
    aa28:	e1a0000d 	mov	r0, sp
    aa2c:	e58d4014 	str	r4, [sp, #20]
    aa30:	ebfffdde 	bl	a1b0 <spi_read_write_data>
    aa34:	e5dd105c 	ldrb	r1, [sp, #92]
    aa38:	e3110002 	tst	r1, #2	; 0x2
    aa3c:	1a000007 	bne	aa60 <flash_wait_device_status+0x350>
    aa40:	e5973000 	ldr	r3, [r7]
    aa44:	e3530000 	cmp	r3, #0	; 0x0
    aa48:	0a000002 	beq	aa58 <flash_wait_device_status+0x348>
    aa4c:	e59f0060 	ldr	r0, [pc, #96]	; aab4 <flash_wait_device_status+0x3a4>
    aa50:	e1a0e00f 	mov	lr, pc
    aa54:	e596f000 	ldr	pc, [r6]
    aa58:	ebffff05 	bl	a674 <flash_write_enable>
    aa5c:	eaffffed 	b	aa18 <flash_wait_device_status+0x308>
    aa60:	e59f3030 	ldr	r3, [pc, #48]	; aa98 <flash_wait_device_status+0x388>
    aa64:	e5930000 	ldr	r0, [r3]
    aa68:	e3500000 	cmp	r0, #0	; 0x0
    aa6c:	0a000005 	beq	aa88 <flash_wait_device_status+0x378>
    aa70:	e59f0040 	ldr	r0, [pc, #64]	; aab8 <flash_wait_device_status+0x3a8>
    aa74:	e5dd105c 	ldrb	r1, [sp, #92]
    aa78:	e59f3014 	ldr	r3, [pc, #20]	; aa94 <flash_wait_device_status+0x384>
    aa7c:	e1a0e00f 	mov	lr, pc
    aa80:	e593f000 	ldr	pc, [r3]
    aa84:	e3a00000 	mov	r0, #0	; 0x0
    aa88:	e28dd064 	add	sp, sp, #100	; 0x64
    aa8c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    aa90:	00013893 	.word	0x00013893
    aa94:	0800d764 	.word	0x0800d764
    aa98:	0800d2b0 	.word	0x0800d2b0
    aa9c:	000138bf 	.word	0x000138bf
    aaa0:	000138e9 	.word	0x000138e9
    aaa4:	0001390c 	.word	0x0001390c
    aaa8:	00013937 	.word	0x00013937
    aaac:	00013962 	.word	0x00013962
    aab0:	0001398d 	.word	0x0001398d
    aab4:	000139cc 	.word	0x000139cc
    aab8:	00013a0d 	.word	0x00013a0d

0000aabc <nand_write_cache_to_memory>:
    aabc:	e92d4030 	push	{r4, r5, lr}
    aac0:	e24dd064 	sub	sp, sp, #100	; 0x64
    aac4:	e1a05000 	mov	r5, r0
    aac8:	e28d0004 	add	r0, sp, #4	; 0x4
    aacc:	e1a04001 	mov	r4, r1
    aad0:	ebfffd82 	bl	a0e0 <spi_transfer_init>
    aad4:	e354000f 	cmp	r4, #15	; 0xf
    aad8:	13a03001 	movne	r3, #1	; 0x1
    aadc:	15cd3017 	strbne	r3, [sp, #23]
    aae0:	e3a03010 	mov	r3, #16	; 0x10
    aae4:	e1a01335 	lsr	r1, r5, r3
    aae8:	e28d0060 	add	r0, sp, #96	; 0x60
    aaec:	05cd4017 	strbeq	r4, [sp, #23]
    aaf0:	e1a02425 	lsr	r2, r5, #8
    aaf4:	e3a04000 	mov	r4, #0	; 0x0
    aaf8:	e560305c 	strb	r3, [r0, #-92]!
    aafc:	e3a03004 	mov	r3, #4	; 0x4
    ab00:	e5cd3008 	strb	r3, [sp, #8]
    ab04:	e5cd2006 	strb	r2, [sp, #6]
    ab08:	e5cd1005 	strb	r1, [sp, #5]
    ab0c:	e58d400c 	str	r4, [sp, #12]
    ab10:	e5cd5007 	strb	r5, [sp, #7]
    ab14:	ebfffda5 	bl	a1b0 <spi_read_write_data>
    ab18:	e3a01006 	mov	r1, #6	; 0x6
    ab1c:	e3a0000f 	mov	r0, #15	; 0xf
    ab20:	ebfffefa 	bl	a710 <flash_wait_device_status>
    ab24:	e1a00004 	mov	r0, r4
    ab28:	e28dd064 	add	sp, sp, #100	; 0x64
    ab2c:	e8bd8030 	pop	{r4, r5, pc}

0000ab30 <nand_read_page_to_cache>:
    ab30:	e92d4030 	push	{r4, r5, lr}
    ab34:	e24dd064 	sub	sp, sp, #100	; 0x64
    ab38:	e1a04001 	mov	r4, r1
    ab3c:	e1a05000 	mov	r5, r0
    ab40:	e28d0004 	add	r0, sp, #4	; 0x4
    ab44:	ebfffd65 	bl	a0e0 <spi_transfer_init>
    ab48:	e354000f 	cmp	r4, #15	; 0xf
    ab4c:	e28d0060 	add	r0, sp, #96	; 0x60
    ab50:	e3a03013 	mov	r3, #19	; 0x13
    ab54:	05cd4017 	strbeq	r4, [sp, #23]
    ab58:	e1a01825 	lsr	r1, r5, #16
    ab5c:	e1a02425 	lsr	r2, r5, #8
    ab60:	e3a04000 	mov	r4, #0	; 0x0
    ab64:	e560305c 	strb	r3, [r0, #-92]!
    ab68:	e3a03004 	mov	r3, #4	; 0x4
    ab6c:	e5cd3008 	strb	r3, [sp, #8]
    ab70:	e5cd2006 	strb	r2, [sp, #6]
    ab74:	e5cd1005 	strb	r1, [sp, #5]
    ab78:	e58d400c 	str	r4, [sp, #12]
    ab7c:	e5cd5007 	strb	r5, [sp, #7]
    ab80:	ebfffd8a 	bl	a1b0 <spi_read_write_data>
    ab84:	e3a01006 	mov	r1, #6	; 0x6
    ab88:	e3a0000f 	mov	r0, #15	; 0xf
    ab8c:	ebfffedf 	bl	a710 <flash_wait_device_status>
    ab90:	e1a00004 	mov	r0, r4
    ab94:	e28dd064 	add	sp, sp, #100	; 0x64
    ab98:	e8bd8030 	pop	{r4, r5, pc}

0000ab9c <nand_blk_prg_era_fail>:
    ab9c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    aba0:	e3a0000f 	mov	r0, #15	; 0xf
    aba4:	e24dd004 	sub	sp, sp, #4	; 0x4
    aba8:	e3a01010 	mov	r1, #16	; 0x10
    abac:	ebfffed7 	bl	a710 <flash_wait_device_status>
    abb0:	e3100008 	tst	r0, #8	; 0x8
    abb4:	0a000004 	beq	abcc <nand_blk_prg_era_fail+0x30>
    abb8:	e59f0038 	ldr	r0, [pc, #56]	; abf8 <nand_blk_prg_era_fail+0x5c>
    abbc:	e59f3038 	ldr	r3, [pc, #56]	; abfc <nand_blk_prg_era_fail+0x60>
    abc0:	e1a0e00f 	mov	lr, pc
    abc4:	e593f000 	ldr	pc, [r3]
    abc8:	ea000007 	b	abec <nand_blk_prg_era_fail+0x50>
    abcc:	e2103004 	ands	r3, r0, #4	; 0x4
    abd0:	01a00003 	moveq	r0, r3
    abd4:	0a000005 	beq	abf0 <nand_blk_prg_era_fail+0x54>
    abd8:	e1a01000 	mov	r1, r0
    abdc:	e59f3018 	ldr	r3, [pc, #24]	; abfc <nand_blk_prg_era_fail+0x60>
    abe0:	e59f0018 	ldr	r0, [pc, #24]	; ac00 <nand_blk_prg_era_fail+0x64>
    abe4:	e1a0e00f 	mov	lr, pc
    abe8:	e593f000 	ldr	pc, [r3]
    abec:	e3a00001 	mov	r0, #1	; 0x1
    abf0:	e28dd004 	add	sp, sp, #4	; 0x4
    abf4:	e8bd8000 	pop	{pc}
    abf8:	00013a4c 	.word	0x00013a4c
    abfc:	0800d764 	.word	0x0800d764
    ac00:	00013a70 	.word	0x00013a70

0000ac04 <nand_ecc_bits_check>:
    ac04:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ac08:	e3a01010 	mov	r1, #16	; 0x10
    ac0c:	e24dd004 	sub	sp, sp, #4	; 0x4
    ac10:	e3a0000f 	mov	r0, #15	; 0xf
    ac14:	ebfffebd 	bl	a710 <flash_wait_device_status>
    ac18:	e2000030 	and	r0, r0, #48	; 0x30
    ac1c:	e1a00220 	lsr	r0, r0, #4
    ac20:	e28dd004 	add	sp, sp, #4	; 0x4
    ac24:	e8bd8000 	pop	{pc}

0000ac28 <nand_ecc_enable>:
    ac28:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    ac2c:	e59f40b4 	ldr	r4, [pc, #180]	; ace8 <nand_ecc_enable+0xc0>
    ac30:	e24dd064 	sub	sp, sp, #100	; 0x64
    ac34:	e594600c 	ldr	r6, [r4, #12]
    ac38:	e3560001 	cmp	r6, #1	; 0x1
    ac3c:	1a000026 	bne	acdc <nand_ecc_enable+0xb4>
    ac40:	e5943040 	ldr	r3, [r4, #64]
    ac44:	e3530001 	cmp	r3, #1	; 0x1
    ac48:	0a000006 	beq	ac68 <nand_ecc_enable+0x40>
    ac4c:	e3a0000f 	mov	r0, #15	; 0xf
    ac50:	e3a01009 	mov	r1, #9	; 0x9
    ac54:	ebfffead 	bl	a710 <flash_wait_device_status>
    ac58:	e2107010 	ands	r7, r0, #16	; 0x10
    ac5c:	e1a05000 	mov	r5, r0
    ac60:	0a000002 	beq	ac70 <nand_ecc_enable+0x48>
    ac64:	e5846040 	str	r6, [r4, #64]
    ac68:	e3a00000 	mov	r0, #0	; 0x0
    ac6c:	ea00001b 	b	ace0 <nand_ecc_enable+0xb8>
    ac70:	e28d4004 	add	r4, sp, #4	; 0x4
    ac74:	e1a00004 	mov	r0, r4
    ac78:	ebfffd18 	bl	a0e0 <spi_transfer_init>
    ac7c:	e3a0301f 	mov	r3, #31	; 0x1f
    ac80:	e5cd3004 	strb	r3, [sp, #4]
    ac84:	e3e0304f 	mvn	r3, #79	; 0x4f
    ac88:	e3852010 	orr	r2, r5, #16	; 0x10
    ac8c:	e1a00004 	mov	r0, r4
    ac90:	e5cd3005 	strb	r3, [sp, #5]
    ac94:	e3a0400f 	mov	r4, #15	; 0xf
    ac98:	e2833053 	add	r3, r3, #83	; 0x53
    ac9c:	e5cd2006 	strb	r2, [sp, #6]
    aca0:	e5cd3008 	strb	r3, [sp, #8]
    aca4:	e58d700c 	str	r7, [sp, #12]
    aca8:	e5cd4017 	strb	r4, [sp, #23]
    acac:	ebfffd3f 	bl	a1b0 <spi_read_write_data>
    acb0:	e1a00004 	mov	r0, r4
    acb4:	e3a01009 	mov	r1, #9	; 0x9
    acb8:	ebfffe94 	bl	a710 <flash_wait_device_status>
    acbc:	e3100010 	tst	r0, #16	; 0x10
    acc0:	0a000005 	beq	acdc <nand_ecc_enable+0xb4>
    acc4:	e59f0020 	ldr	r0, [pc, #32]	; acec <nand_ecc_enable+0xc4>
    acc8:	e59f3020 	ldr	r3, [pc, #32]	; acf0 <nand_ecc_enable+0xc8>
    accc:	e1a0e00f 	mov	lr, pc
    acd0:	e593f000 	ldr	pc, [r3]
    acd4:	e1a00007 	mov	r0, r7
    acd8:	ea000000 	b	ace0 <nand_ecc_enable+0xb8>
    acdc:	e3e00000 	mvn	r0, #0	; 0x0
    ace0:	e28dd064 	add	sp, sp, #100	; 0x64
    ace4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    ace8:	0800d250 	.word	0x0800d250
    acec:	00013aa6 	.word	0x00013aa6
    acf0:	0800d764 	.word	0x0800d764

0000acf4 <flash_nand_vs_read>:
    acf4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    acf8:	e59f4438 	ldr	r4, [pc, #1080]	; b138 <flash_nand_vs_read+0x444>
    acfc:	e24dd074 	sub	sp, sp, #116	; 0x74
    ad00:	e1a05000 	mov	r5, r0
    ad04:	e5941028 	ldr	r1, [r4, #40]
    ad08:	e1a00002 	mov	r0, r2
    ad0c:	eb0015ac 	bl	103c4 <__aeabi_idiv>
    ad10:	e594305c 	ldr	r3, [r4, #92]
    ad14:	e1a07000 	mov	r7, r0
    ad18:	e3530001 	cmp	r3, #1	; 0x1
    ad1c:	e59d6098 	ldr	r6, [sp, #152]
    ad20:	e1a08000 	mov	r8, r0
    ad24:	e1a09fc8 	asr	r9, r8, #31
    ad28:	0a000004 	beq	ad40 <flash_nand_vs_read+0x4c>
    ad2c:	ebfffbc3 	bl	9c40 <ssp_manager_init>
    ad30:	e3a03064 	mov	r3, #100	; 0x64
    ad34:	e5843050 	str	r3, [r4, #80]
    ad38:	e3a0300f 	mov	r3, #15	; 0xf
    ad3c:	e5843000 	str	r3, [r4]
    ad40:	e59f33f4 	ldr	r3, [pc, #1012]	; b13c <flash_nand_vs_read+0x448>
    ad44:	e5933000 	ldr	r3, [r3]
    ad48:	e3530000 	cmp	r3, #0	; 0x0
    ad4c:	0a000007 	beq	ad70 <flash_nand_vs_read+0x7c>
    ad50:	e59f23e0 	ldr	r2, [pc, #992]	; b138 <flash_nand_vs_read+0x444>
    ad54:	e59f03e4 	ldr	r0, [pc, #996]	; b140 <flash_nand_vs_read+0x44c>
    ad58:	e5923050 	ldr	r3, [r2, #80]
    ad5c:	e5921000 	ldr	r1, [r2]
    ad60:	e59fc3dc 	ldr	ip, [pc, #988]	; b144 <flash_nand_vs_read+0x450>
    ad64:	e5922004 	ldr	r2, [r2, #4]
    ad68:	e1a0e00f 	mov	lr, pc
    ad6c:	e59cf000 	ldr	pc, [ip]
    ad70:	e59f33c4 	ldr	r3, [pc, #964]	; b13c <flash_nand_vs_read+0x448>
    ad74:	e59f23bc 	ldr	r2, [pc, #956]	; b138 <flash_nand_vs_read+0x444>
    ad78:	e5933000 	ldr	r3, [r3]
    ad7c:	e5921044 	ldr	r1, [r2, #68]
    ad80:	e3530000 	cmp	r3, #0	; 0x0
    ad84:	e59f33bc 	ldr	r3, [pc, #956]	; b148 <flash_nand_vs_read+0x454>
    ad88:	e5924000 	ldr	r4, [r2]
    ad8c:	e5831000 	str	r1, [r3]
    ad90:	e5921004 	ldr	r1, [r2, #4]
    ad94:	0a00000e 	beq	add4 <flash_nand_vs_read+0xe0>
    ad98:	e59f23ac 	ldr	r2, [pc, #940]	; b14c <flash_nand_vs_read+0x458>
    ad9c:	e59f33ac 	ldr	r3, [pc, #940]	; b150 <flash_nand_vs_read+0x45c>
    ada0:	e3510000 	cmp	r1, #0	; 0x0
    ada4:	01a03002 	moveq	r3, r2
    ada8:	e58d300c 	str	r3, [sp, #12]
    adac:	e59f3390 	ldr	r3, [pc, #912]	; b144 <flash_nand_vs_read+0x450>
    adb0:	e58d1008 	str	r1, [sp, #8]
    adb4:	e593c000 	ldr	ip, [r3]
    adb8:	e59f0394 	ldr	r0, [pc, #916]	; b154 <flash_nand_vs_read+0x460>
    adbc:	e58d6000 	str	r6, [sp]
    adc0:	e58d4004 	str	r4, [sp, #4]
    adc4:	e59f138c 	ldr	r1, [pc, #908]	; b158 <flash_nand_vs_read+0x464>
    adc8:	e1a02008 	mov	r2, r8
    adcc:	e1a03009 	mov	r3, r9
    add0:	e12fff3c 	blx	ip
    add4:	e28d0014 	add	r0, sp, #20	; 0x14
    add8:	ebfffcc0 	bl	a0e0 <spi_transfer_init>
    addc:	e354000f 	cmp	r4, #15	; 0xf
    ade0:	159f0374 	ldrne	r0, [pc, #884]	; b15c <flash_nand_vs_read+0x468>
    ade4:	1a0000cc 	bne	b11c <flash_nand_vs_read+0x428>
    ade8:	e59f334c 	ldr	r3, [pc, #844]	; b13c <flash_nand_vs_read+0x448>
    adec:	e5933000 	ldr	r3, [r3]
    adf0:	e3530000 	cmp	r3, #0	; 0x0
    adf4:	0a000005 	beq	ae10 <flash_nand_vs_read+0x11c>
    adf8:	e59f0360 	ldr	r0, [pc, #864]	; b160 <flash_nand_vs_read+0x46c>
    adfc:	e59f1354 	ldr	r1, [pc, #852]	; b158 <flash_nand_vs_read+0x464>
    ae00:	e3002381 	movw	r2, #897	; 0x381
    ae04:	e59f3338 	ldr	r3, [pc, #824]	; b144 <flash_nand_vs_read+0x450>
    ae08:	e1a0e00f 	mov	lr, pc
    ae0c:	e593f000 	ldr	pc, [r3]
    ae10:	e59f3324 	ldr	r3, [pc, #804]	; b13c <flash_nand_vs_read+0x448>
    ae14:	e3a02001 	mov	r2, #1	; 0x1
    ae18:	e5933000 	ldr	r3, [r3]
    ae1c:	e5cd2026 	strb	r2, [sp, #38]
    ae20:	e3530000 	cmp	r3, #0	; 0x0
    ae24:	e3a03003 	mov	r3, #3	; 0x3
    ae28:	e5cd3014 	strb	r3, [sp, #20]
    ae2c:	e3a03002 	mov	r3, #2	; 0x2
    ae30:	e58d301c 	str	r3, [sp, #28]
    ae34:	e3a03000 	mov	r3, #0	; 0x0
    ae38:	e5cd3025 	strb	r3, [sp, #37]
    ae3c:	e283300f 	add	r3, r3, #15	; 0xf
    ae40:	e5cd3027 	strb	r3, [sp, #39]
    ae44:	e5cd2018 	strb	r2, [sp, #24]
    ae48:	0a000005 	beq	ae64 <flash_nand_vs_read+0x170>
    ae4c:	e59f030c 	ldr	r0, [pc, #780]	; b160 <flash_nand_vs_read+0x46c>
    ae50:	e59f1300 	ldr	r1, [pc, #768]	; b158 <flash_nand_vs_read+0x464>
    ae54:	e3a02fe2 	mov	r2, #904	; 0x388
    ae58:	e59f32e4 	ldr	r3, [pc, #740]	; b144 <flash_nand_vs_read+0x450>
    ae5c:	e1a0e00f 	mov	lr, pc
    ae60:	e593f000 	ldr	pc, [r3]
    ae64:	e59f32d0 	ldr	r3, [pc, #720]	; b13c <flash_nand_vs_read+0x448>
    ae68:	e58d5028 	str	r5, [sp, #40]
    ae6c:	e5932000 	ldr	r2, [r3]
    ae70:	e59f32d0 	ldr	r3, [pc, #720]	; b148 <flash_nand_vs_read+0x454>
    ae74:	e3520000 	cmp	r2, #0	; 0x0
    ae78:	e5933000 	ldr	r3, [r3]
    ae7c:	e58d803c 	str	r8, [sp, #60]
    ae80:	e58d3030 	str	r3, [sp, #48]
    ae84:	e58d302c 	str	r3, [sp, #44]
    ae88:	0a000005 	beq	aea4 <flash_nand_vs_read+0x1b0>
    ae8c:	e59f02cc 	ldr	r0, [pc, #716]	; b160 <flash_nand_vs_read+0x46c>
    ae90:	e59f12c0 	ldr	r1, [pc, #704]	; b158 <flash_nand_vs_read+0x464>
    ae94:	e3002395 	movw	r2, #917	; 0x395
    ae98:	e59f32a4 	ldr	r3, [pc, #676]	; b144 <flash_nand_vs_read+0x450>
    ae9c:	e1a0e00f 	mov	lr, pc
    aea0:	e593f000 	ldr	pc, [r3]
    aea4:	e59f329c 	ldr	r3, [pc, #668]	; b148 <flash_nand_vs_read+0x454>
    aea8:	e1a00006 	mov	r0, r6
    aeac:	e593b000 	ldr	fp, [r3]
    aeb0:	e1a0100b 	mov	r1, fp
    aeb4:	eb001542 	bl	103c4 <__aeabi_idiv>
    aeb8:	e59f327c 	ldr	r3, [pc, #636]	; b13c <flash_nand_vs_read+0x448>
    aebc:	e1a0a000 	mov	sl, r0
    aec0:	e5933000 	ldr	r3, [r3]
    aec4:	e3530000 	cmp	r3, #0	; 0x0
    aec8:	0a000004 	beq	aee0 <flash_nand_vs_read+0x1ec>
    aecc:	e59f0290 	ldr	r0, [pc, #656]	; b164 <flash_nand_vs_read+0x470>
    aed0:	e1a0100a 	mov	r1, sl
    aed4:	e59f3268 	ldr	r3, [pc, #616]	; b144 <flash_nand_vs_read+0x450>
    aed8:	e1a0e00f 	mov	lr, pc
    aedc:	e593f000 	ldr	pc, [r3]
    aee0:	e3a05000 	mov	r5, #0	; 0x0
    aee4:	e1a09005 	mov	r9, r5
    aee8:	e59f8248 	ldr	r8, [pc, #584]	; b138 <flash_nand_vs_read+0x444>
    aeec:	ea000047 	b	b010 <flash_nand_vs_read+0x31c>
    aef0:	e59d303c 	ldr	r3, [sp, #60]
    aef4:	e598200c 	ldr	r2, [r8, #12]
    aef8:	e1a01823 	lsr	r1, r3, #16
    aefc:	e3520001 	cmp	r2, #1	; 0x1
    af00:	e1a02423 	lsr	r2, r3, #8
    af04:	e5cd2021 	strb	r2, [sp, #33]
    af08:	13a02004 	movne	r2, #4	; 0x4
    af0c:	e5cd1020 	strb	r1, [sp, #32]
    af10:	e5cd3022 	strb	r3, [sp, #34]
    af14:	e5cd9023 	strb	r9, [sp, #35]
    af18:	15cd2024 	strbne	r2, [sp, #36]
    af1c:	15cd9026 	strbne	r9, [sp, #38]
    af20:	1a000014 	bne	af78 <flash_nand_vs_read+0x284>
    af24:	e3a04000 	mov	r4, #0	; 0x0
    af28:	ebffff3e 	bl	ac28 <nand_ecc_enable>
    af2c:	e3500000 	cmp	r0, #0	; 0x0
    af30:	e2844001 	add	r4, r4, #1	; 0x1
    af34:	0a000001 	beq	af40 <flash_nand_vs_read+0x24c>
    af38:	e354000a 	cmp	r4, #10	; 0xa
    af3c:	1afffff9 	bne	af28 <flash_nand_vs_read+0x234>
    af40:	e598303c 	ldr	r3, [r8, #60]
    af44:	e3a02000 	mov	r2, #0	; 0x0
    af48:	e3530001 	cmp	r3, #1	; 0x1
    af4c:	02833007 	addeq	r3, r3, #7	; 0x7
    af50:	e5cd9020 	strb	r9, [sp, #32]
    af54:	e5cd2021 	strb	r2, [sp, #33]
    af58:	05cd3020 	strbeq	r3, [sp, #32]
    af5c:	e3a02002 	mov	r2, #2	; 0x2
    af60:	e3a03001 	mov	r3, #1	; 0x1
    af64:	e0850007 	add	r0, r5, r7
    af68:	e3a0100f 	mov	r1, #15	; 0xf
    af6c:	e5cd2024 	strb	r2, [sp, #36]
    af70:	e5cd3026 	strb	r3, [sp, #38]
    af74:	ebfffeed 	bl	ab30 <nand_read_page_to_cache>
    af78:	e59f21bc 	ldr	r2, [pc, #444]	; b13c <flash_nand_vs_read+0x448>
    af7c:	e5923000 	ldr	r3, [r2]
    af80:	e3530000 	cmp	r3, #0	; 0x0
    af84:	0a000005 	beq	afa0 <flash_nand_vs_read+0x2ac>
    af88:	e59f01d8 	ldr	r0, [pc, #472]	; b168 <flash_nand_vs_read+0x474>
    af8c:	e59d102c 	ldr	r1, [sp, #44]
    af90:	e59d203c 	ldr	r2, [sp, #60]
    af94:	e59f31a8 	ldr	r3, [pc, #424]	; b144 <flash_nand_vs_read+0x450>
    af98:	e1a0e00f 	mov	lr, pc
    af9c:	e593f000 	ldr	pc, [r3]
    afa0:	e59f2194 	ldr	r2, [pc, #404]	; b13c <flash_nand_vs_read+0x448>
    afa4:	e5923000 	ldr	r3, [r2]
    afa8:	e3530000 	cmp	r3, #0	; 0x0
    afac:	0a000004 	beq	afc4 <flash_nand_vs_read+0x2d0>
    afb0:	e59f01b4 	ldr	r0, [pc, #436]	; b16c <flash_nand_vs_read+0x478>
    afb4:	e1a01005 	mov	r1, r5
    afb8:	e59f3184 	ldr	r3, [pc, #388]	; b144 <flash_nand_vs_read+0x450>
    afbc:	e1a0e00f 	mov	lr, pc
    afc0:	e593f000 	ldr	pc, [r3]
    afc4:	e5983028 	ldr	r3, [r8, #40]
    afc8:	e28d0014 	add	r0, sp, #20	; 0x14
    afcc:	e58d3030 	str	r3, [sp, #48]
    afd0:	e58d302c 	str	r3, [sp, #44]
    afd4:	ebfffc75 	bl	a1b0 <spi_read_write_data>
    afd8:	e598300c 	ldr	r3, [r8, #12]
    afdc:	e3530001 	cmp	r3, #1	; 0x1
    afe0:	1a000002 	bne	aff0 <flash_nand_vs_read+0x2fc>
    afe4:	ebffff06 	bl	ac04 <nand_ecc_bits_check>
    afe8:	e3500002 	cmp	r0, #2	; 0x2
    afec:	0a000049 	beq	b118 <flash_nand_vs_read+0x424>
    aff0:	e59f3150 	ldr	r3, [pc, #336]	; b148 <flash_nand_vs_read+0x454>
    aff4:	e2855001 	add	r5, r5, #1	; 0x1
    aff8:	e5932000 	ldr	r2, [r3]
    affc:	e59d303c 	ldr	r3, [sp, #60]
    b000:	e58d2030 	str	r2, [sp, #48]
    b004:	e0833002 	add	r3, r3, r2
    b008:	e58d303c 	str	r3, [sp, #60]
    b00c:	e58d202c 	str	r2, [sp, #44]
    b010:	e155000a 	cmp	r5, sl
    b014:	3affffb5 	bcc	aef0 <flash_nand_vs_read+0x1fc>
    b018:	e1a0100b 	mov	r1, fp
    b01c:	e1a00006 	mov	r0, r6
    b020:	eb00156f 	bl	105e4 <__aeabi_idivmod>
    b024:	e2518000 	subs	r8, r1, #0	; 0x0
    b028:	0a00003f 	beq	b12c <flash_nand_vs_read+0x438>
    b02c:	e59f3104 	ldr	r3, [pc, #260]	; b138 <flash_nand_vs_read+0x444>
    b030:	e59d203c 	ldr	r2, [sp, #60]
    b034:	e593300c 	ldr	r3, [r3, #12]
    b038:	e3a00000 	mov	r0, #0	; 0x0
    b03c:	e3530001 	cmp	r3, #1	; 0x1
    b040:	e1a03422 	lsr	r3, r2, #8
    b044:	e1a01822 	lsr	r1, r2, #16
    b048:	e5cd3021 	strb	r3, [sp, #33]
    b04c:	13a03004 	movne	r3, #4	; 0x4
    b050:	e5cd1020 	strb	r1, [sp, #32]
    b054:	e5cd2022 	strb	r2, [sp, #34]
    b058:	e5cd0023 	strb	r0, [sp, #35]
    b05c:	15cd3024 	strbne	r3, [sp, #36]
    b060:	15cd0026 	strbne	r0, [sp, #38]
    b064:	1a000015 	bne	b0c0 <flash_nand_vs_read+0x3cc>
    b068:	e1a04000 	mov	r4, r0
    b06c:	ebfffeed 	bl	ac28 <nand_ecc_enable>
    b070:	e3500000 	cmp	r0, #0	; 0x0
    b074:	e2844001 	add	r4, r4, #1	; 0x1
    b078:	0a000001 	beq	b084 <flash_nand_vs_read+0x390>
    b07c:	e354000a 	cmp	r4, #10	; 0xa
    b080:	1afffff9 	bne	b06c <flash_nand_vs_read+0x378>
    b084:	e59f20ac 	ldr	r2, [pc, #172]	; b138 <flash_nand_vs_read+0x444>
    b088:	e3a03000 	mov	r3, #0	; 0x0
    b08c:	e592203c 	ldr	r2, [r2, #60]
    b090:	e5cd3020 	strb	r3, [sp, #32]
    b094:	e1520003 	cmp	r2, r3
    b098:	e5cd3021 	strb	r3, [sp, #33]
    b09c:	12833008 	addne	r3, r3, #8	; 0x8
    b0a0:	15cd3020 	strbne	r3, [sp, #32]
    b0a4:	e3a03002 	mov	r3, #2	; 0x2
    b0a8:	e5cd3024 	strb	r3, [sp, #36]
    b0ac:	e0850007 	add	r0, r5, r7
    b0b0:	e3a03001 	mov	r3, #1	; 0x1
    b0b4:	e3a0100f 	mov	r1, #15	; 0xf
    b0b8:	e5cd3026 	strb	r3, [sp, #38]
    b0bc:	ebfffe9b 	bl	ab30 <nand_read_page_to_cache>
    b0c0:	e59f3074 	ldr	r3, [pc, #116]	; b13c <flash_nand_vs_read+0x448>
    b0c4:	e5933000 	ldr	r3, [r3]
    b0c8:	e3530000 	cmp	r3, #0	; 0x0
    b0cc:	0a000006 	beq	b0ec <flash_nand_vs_read+0x3f8>
    b0d0:	e59f0098 	ldr	r0, [pc, #152]	; b170 <flash_nand_vs_read+0x47c>
    b0d4:	e59d102c 	ldr	r1, [sp, #44]
    b0d8:	e59d203c 	ldr	r2, [sp, #60]
    b0dc:	e1dd32d7 	ldrsb	r3, [sp, #39]
    b0e0:	e59fc05c 	ldr	ip, [pc, #92]	; b144 <flash_nand_vs_read+0x450>
    b0e4:	e1a0e00f 	mov	lr, pc
    b0e8:	e59cf000 	ldr	pc, [ip]
    b0ec:	e28d0014 	add	r0, sp, #20	; 0x14
    b0f0:	e58d8030 	str	r8, [sp, #48]
    b0f4:	e58d802c 	str	r8, [sp, #44]
    b0f8:	ebfffc2c 	bl	a1b0 <spi_read_write_data>
    b0fc:	e59f3034 	ldr	r3, [pc, #52]	; b138 <flash_nand_vs_read+0x444>
    b100:	e593300c 	ldr	r3, [r3, #12]
    b104:	e3530001 	cmp	r3, #1	; 0x1
    b108:	1a000007 	bne	b12c <flash_nand_vs_read+0x438>
    b10c:	ebfffebc 	bl	ac04 <nand_ecc_bits_check>
    b110:	e3500002 	cmp	r0, #2	; 0x2
    b114:	1a000004 	bne	b12c <flash_nand_vs_read+0x438>
    b118:	e59f0054 	ldr	r0, [pc, #84]	; b174 <flash_nand_vs_read+0x480>
    b11c:	e59f3020 	ldr	r3, [pc, #32]	; b144 <flash_nand_vs_read+0x450>
    b120:	e1a0e00f 	mov	lr, pc
    b124:	e593f000 	ldr	pc, [r3]
    b128:	e3e06000 	mvn	r6, #0	; 0x0
    b12c:	e1a00006 	mov	r0, r6
    b130:	e28dd074 	add	sp, sp, #116	; 0x74
    b134:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b138:	0800d250 	.word	0x0800d250
    b13c:	0800d2b0 	.word	0x0800d2b0
    b140:	00013ad9 	.word	0x00013ad9
    b144:	0800d764 	.word	0x0800d764
    b148:	0800d130 	.word	0x0800d130
    b14c:	0001409d 	.word	0x0001409d
    b150:	00013b07 	.word	0x00013b07
    b154:	00013b0b 	.word	0x00013b0b
    b158:	000118d6 	.word	0x000118d6
    b15c:	00013588 	.word	0x00013588
    b160:	00013b54 	.word	0x00013b54
    b164:	00013b68 	.word	0x00013b68
    b168:	00013b83 	.word	0x00013b83
    b16c:	00013bb3 	.word	0x00013bb3
    b170:	00013bfb 	.word	0x00013bfb
    b174:	00013bdd 	.word	0x00013bdd

0000b178 <flash_nand_vs_write>:
    b178:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b17c:	e59f444c 	ldr	r4, [pc, #1100]	; b5d0 <flash_nand_vs_write+0x458>
    b180:	e24dd074 	sub	sp, sp, #116	; 0x74
    b184:	e594305c 	ldr	r3, [r4, #92]
    b188:	e1a09000 	mov	r9, r0
    b18c:	e3530001 	cmp	r3, #1	; 0x1
    b190:	e1a05002 	mov	r5, r2
    b194:	e59d8098 	ldr	r8, [sp, #152]
    b198:	0a000004 	beq	b1b0 <flash_nand_vs_write+0x38>
    b19c:	ebfffaa7 	bl	9c40 <ssp_manager_init>
    b1a0:	e3a03064 	mov	r3, #100	; 0x64
    b1a4:	e5843050 	str	r3, [r4, #80]
    b1a8:	e3a0300f 	mov	r3, #15	; 0xf
    b1ac:	e5843000 	str	r3, [r4]
    b1b0:	e59f4418 	ldr	r4, [pc, #1048]	; b5d0 <flash_nand_vs_write+0x458>
    b1b4:	e1a00005 	mov	r0, r5
    b1b8:	e5941028 	ldr	r1, [r4, #40]
    b1bc:	eb001480 	bl	103c4 <__aeabi_idiv>
    b1c0:	e59f340c 	ldr	r3, [pc, #1036]	; b5d4 <flash_nand_vs_write+0x45c>
    b1c4:	e5942044 	ldr	r2, [r4, #68]
    b1c8:	e5941030 	ldr	r1, [r4, #48]
    b1cc:	e1a0b000 	mov	fp, r0
    b1d0:	e5832000 	str	r2, [r3]
    b1d4:	e28d0014 	add	r0, sp, #20	; 0x14
    b1d8:	e58d100c 	str	r1, [sp, #12]
    b1dc:	ebfffbbf 	bl	a0e0 <spi_transfer_init>
    b1e0:	e59f33f0 	ldr	r3, [pc, #1008]	; b5d8 <flash_nand_vs_write+0x460>
    b1e4:	e1a0600b 	mov	r6, fp
    b1e8:	e1a07fc6 	asr	r7, r6, #31
    b1ec:	e5933000 	ldr	r3, [r3]
    b1f0:	e58d603c 	str	r6, [sp, #60]
    b1f4:	e3530000 	cmp	r3, #0	; 0x0
    b1f8:	e5945000 	ldr	r5, [r4]
    b1fc:	e1a0a006 	mov	sl, r6
    b200:	0a000003 	beq	b214 <flash_nand_vs_write+0x9c>
    b204:	e59f03d0 	ldr	r0, [pc, #976]	; b5dc <flash_nand_vs_write+0x464>
    b208:	e59f33d0 	ldr	r3, [pc, #976]	; b5e0 <flash_nand_vs_write+0x468>
    b20c:	e1a0e00f 	mov	lr, pc
    b210:	e593f000 	ldr	pc, [r3]
    b214:	e59f33bc 	ldr	r3, [pc, #956]	; b5d8 <flash_nand_vs_write+0x460>
    b218:	e5933000 	ldr	r3, [r3]
    b21c:	e3530000 	cmp	r3, #0	; 0x0
    b220:	0a000007 	beq	b244 <flash_nand_vs_write+0xcc>
    b224:	e58d8000 	str	r8, [sp]
    b228:	e1a02006 	mov	r2, r6
    b22c:	e1a03007 	mov	r3, r7
    b230:	e59f03ac 	ldr	r0, [pc, #940]	; b5e4 <flash_nand_vs_write+0x46c>
    b234:	e59f13ac 	ldr	r1, [pc, #940]	; b5e8 <flash_nand_vs_write+0x470>
    b238:	e59fc3a0 	ldr	ip, [pc, #928]	; b5e0 <flash_nand_vs_write+0x468>
    b23c:	e1a0e00f 	mov	lr, pc
    b240:	e59cf000 	ldr	pc, [ip]
    b244:	e59f338c 	ldr	r3, [pc, #908]	; b5d8 <flash_nand_vs_write+0x460>
    b248:	e5933000 	ldr	r3, [r3]
    b24c:	e3530000 	cmp	r3, #0	; 0x0
    b250:	0a000006 	beq	b270 <flash_nand_vs_write+0xf8>
    b254:	e59f3374 	ldr	r3, [pc, #884]	; b5d0 <flash_nand_vs_write+0x458>
    b258:	e59f038c 	ldr	r0, [pc, #908]	; b5ec <flash_nand_vs_write+0x474>
    b25c:	e593104c 	ldr	r1, [r3, #76]
    b260:	e1a02005 	mov	r2, r5
    b264:	e59f3374 	ldr	r3, [pc, #884]	; b5e0 <flash_nand_vs_write+0x468>
    b268:	e1a0e00f 	mov	lr, pc
    b26c:	e593f000 	ldr	pc, [r3]
    b270:	e355000f 	cmp	r5, #15	; 0xf
    b274:	1a00001d 	bne	b2f0 <flash_nand_vs_write+0x178>
    b278:	e59f3354 	ldr	r3, [pc, #852]	; b5d4 <flash_nand_vs_write+0x45c>
    b27c:	e3a04000 	mov	r4, #0	; 0x0
    b280:	e5936000 	ldr	r6, [r3]
    b284:	e3a03002 	mov	r3, #2	; 0x2
    b288:	e3a02003 	mov	r2, #3	; 0x3
    b28c:	e5cd3014 	strb	r3, [sp, #20]
    b290:	e1a00008 	mov	r0, r8
    b294:	e3a03001 	mov	r3, #1	; 0x1
    b298:	e1a01006 	mov	r1, r6
    b29c:	e5cd3018 	strb	r3, [sp, #24]
    b2a0:	e5cd2024 	strb	r2, [sp, #36]
    b2a4:	e58d201c 	str	r2, [sp, #28]
    b2a8:	e5cd5027 	strb	r5, [sp, #39]
    b2ac:	e5cd4026 	strb	r4, [sp, #38]
    b2b0:	eb001443 	bl	103c4 <__aeabi_idiv>
    b2b4:	e59f331c 	ldr	r3, [pc, #796]	; b5d8 <flash_nand_vs_write+0x460>
    b2b8:	e1580006 	cmp	r8, r6
    b2bc:	b1a02008 	movlt	r2, r8
    b2c0:	a1a02006 	movge	r2, r6
    b2c4:	e5933000 	ldr	r3, [r3]
    b2c8:	e58d0008 	str	r0, [sp, #8]
    b2cc:	e1530004 	cmp	r3, r4
    b2d0:	e58da03c 	str	sl, [sp, #60]
    b2d4:	e58d9034 	str	r9, [sp, #52]
    b2d8:	e58d202c 	str	r2, [sp, #44]
    b2dc:	e58d4040 	str	r4, [sp, #64]
    b2e0:	e58d4028 	str	r4, [sp, #40]
    b2e4:	e58d2038 	str	r2, [sp, #56]
    b2e8:	0a00000b 	beq	b31c <flash_nand_vs_write+0x1a4>
    b2ec:	ea000005 	b	b308 <flash_nand_vs_write+0x190>
    b2f0:	e59f02f8 	ldr	r0, [pc, #760]	; b5f0 <flash_nand_vs_write+0x478>
    b2f4:	e59f32e4 	ldr	r3, [pc, #740]	; b5e0 <flash_nand_vs_write+0x468>
    b2f8:	e1a0e00f 	mov	lr, pc
    b2fc:	e593f000 	ldr	pc, [r3]
    b300:	e3e08000 	mvn	r8, #0	; 0x0
    b304:	ea0000ae 	b	b5c4 <flash_nand_vs_write+0x44c>
    b308:	e59f02e4 	ldr	r0, [pc, #740]	; b5f4 <flash_nand_vs_write+0x47c>
    b30c:	e59d1008 	ldr	r1, [sp, #8]
    b310:	e59f32c8 	ldr	r3, [pc, #712]	; b5e0 <flash_nand_vs_write+0x468>
    b314:	e1a0e00f 	mov	lr, pc
    b318:	e593f000 	ldr	pc, [r3]
    b31c:	e3a05000 	mov	r5, #0	; 0x0
    b320:	e59f92b0 	ldr	r9, [pc, #688]	; b5d8 <flash_nand_vs_write+0x460>
    b324:	e59fa2b4 	ldr	sl, [pc, #692]	; b5e0 <flash_nand_vs_write+0x468>
    b328:	e1a07005 	mov	r7, r5
    b32c:	ea000049 	b	b458 <flash_nand_vs_write+0x2e0>
    b330:	e5993000 	ldr	r3, [r9]
    b334:	e3530000 	cmp	r3, #0	; 0x0
    b338:	0a000003 	beq	b34c <flash_nand_vs_write+0x1d4>
    b33c:	e59f02b4 	ldr	r0, [pc, #692]	; b5f8 <flash_nand_vs_write+0x480>
    b340:	e1a01005 	mov	r1, r5
    b344:	e1a0e00f 	mov	lr, pc
    b348:	e59af000 	ldr	pc, [sl]
    b34c:	ebfffcc8 	bl	a674 <flash_write_enable>
    b350:	e3a01007 	mov	r1, #7	; 0x7
    b354:	e3a0000f 	mov	r0, #15	; 0xf
    b358:	ebfffcec 	bl	a710 <flash_wait_device_status>
    b35c:	e3a01006 	mov	r1, #6	; 0x6
    b360:	e3a0000f 	mov	r0, #15	; 0xf
    b364:	ebfffce9 	bl	a710 <flash_wait_device_status>
    b368:	e59f1260 	ldr	r1, [pc, #608]	; b5d0 <flash_nand_vs_write+0x458>
    b36c:	e59d303c 	ldr	r3, [sp, #60]
    b370:	e591200c 	ldr	r2, [r1, #12]
    b374:	e1a01823 	lsr	r1, r3, #16
    b378:	e3520001 	cmp	r2, #1	; 0x1
    b37c:	e1a02423 	lsr	r2, r3, #8
    b380:	e5cd2021 	strb	r2, [sp, #33]
    b384:	e3a02003 	mov	r2, #3	; 0x3
    b388:	e5cd1020 	strb	r1, [sp, #32]
    b38c:	e5cd3022 	strb	r3, [sp, #34]
    b390:	e5cd7023 	strb	r7, [sp, #35]
    b394:	e5cd2024 	strb	r2, [sp, #36]
    b398:	1a000017 	bne	b3fc <flash_nand_vs_write+0x284>
    b39c:	e5993000 	ldr	r3, [r9]
    b3a0:	e3530000 	cmp	r3, #0	; 0x0
    b3a4:	0a000002 	beq	b3b4 <flash_nand_vs_write+0x23c>
    b3a8:	e59f024c 	ldr	r0, [pc, #588]	; b5fc <flash_nand_vs_write+0x484>
    b3ac:	e1a0e00f 	mov	lr, pc
    b3b0:	e59af000 	ldr	pc, [sl]
    b3b4:	e3a04000 	mov	r4, #0	; 0x0
    b3b8:	ebfffe1a 	bl	ac28 <nand_ecc_enable>
    b3bc:	e3500000 	cmp	r0, #0	; 0x0
    b3c0:	e2844001 	add	r4, r4, #1	; 0x1
    b3c4:	0a000001 	beq	b3d0 <flash_nand_vs_write+0x258>
    b3c8:	e354000a 	cmp	r4, #10	; 0xa
    b3cc:	1afffff9 	bne	b3b8 <flash_nand_vs_write+0x240>
    b3d0:	e5993000 	ldr	r3, [r9]
    b3d4:	e3530000 	cmp	r3, #0	; 0x0
    b3d8:	0a000002 	beq	b3e8 <flash_nand_vs_write+0x270>
    b3dc:	e59f021c 	ldr	r0, [pc, #540]	; b600 <flash_nand_vs_write+0x488>
    b3e0:	e1a0e00f 	mov	lr, pc
    b3e4:	e59af000 	ldr	pc, [sl]
    b3e8:	e3a03002 	mov	r3, #2	; 0x2
    b3ec:	e5cd7020 	strb	r7, [sp, #32]
    b3f0:	e5cd7021 	strb	r7, [sp, #33]
    b3f4:	e5cd3024 	strb	r3, [sp, #36]
    b3f8:	e5cd7026 	strb	r7, [sp, #38]
    b3fc:	e59f11d0 	ldr	r1, [pc, #464]	; b5d4 <flash_nand_vs_write+0x45c>
    b400:	e28d0014 	add	r0, sp, #20	; 0x14
    b404:	e5913000 	ldr	r3, [r1]
    b408:	e58d302c 	str	r3, [sp, #44]
    b40c:	e58d3038 	str	r3, [sp, #56]
    b410:	ebfffb66 	bl	a1b0 <spi_read_write_data>
    b414:	e59f31b8 	ldr	r3, [pc, #440]	; b5d4 <flash_nand_vs_write+0x45c>
    b418:	e5932000 	ldr	r2, [r3]
    b41c:	e59f31ac 	ldr	r3, [pc, #428]	; b5d0 <flash_nand_vs_write+0x458>
    b420:	e593100c 	ldr	r1, [r3, #12]
    b424:	e59d303c 	ldr	r3, [sp, #60]
    b428:	e3510001 	cmp	r1, #1	; 0x1
    b42c:	e0833002 	add	r3, r3, r2
    b430:	e58d303c 	str	r3, [sp, #60]
    b434:	1a000006 	bne	b454 <flash_nand_vs_write+0x2dc>
    b438:	e085400b 	add	r4, r5, fp
    b43c:	e281100e 	add	r1, r1, #14	; 0xe
    b440:	e1a00004 	mov	r0, r4
    b444:	ebfffd9c 	bl	aabc <nand_write_cache_to_memory>
    b448:	ebfffdd3 	bl	ab9c <nand_blk_prg_era_fail>
    b44c:	e3500001 	cmp	r0, #1	; 0x1
    b450:	0a000051 	beq	b59c <flash_nand_vs_write+0x424>
    b454:	e2855001 	add	r5, r5, #1	; 0x1
    b458:	e59d1008 	ldr	r1, [sp, #8]
    b45c:	e1550001 	cmp	r5, r1
    b460:	3affffb2 	bcc	b330 <flash_nand_vs_write+0x1b8>
    b464:	e1a01006 	mov	r1, r6
    b468:	e1a00008 	mov	r0, r8
    b46c:	eb00145c 	bl	105e4 <__aeabi_idivmod>
    b470:	e2516000 	subs	r6, r1, #0	; 0x0
    b474:	0a000052 	beq	b5c4 <flash_nand_vs_write+0x44c>
    b478:	e59f3158 	ldr	r3, [pc, #344]	; b5d8 <flash_nand_vs_write+0x460>
    b47c:	e5933000 	ldr	r3, [r3]
    b480:	e3530000 	cmp	r3, #0	; 0x0
    b484:	0a000003 	beq	b498 <flash_nand_vs_write+0x320>
    b488:	e59f0174 	ldr	r0, [pc, #372]	; b604 <flash_nand_vs_write+0x48c>
    b48c:	e59f314c 	ldr	r3, [pc, #332]	; b5e0 <flash_nand_vs_write+0x468>
    b490:	e1a0e00f 	mov	lr, pc
    b494:	e593f000 	ldr	pc, [r3]
    b498:	ebfffc75 	bl	a674 <flash_write_enable>
    b49c:	e3a01007 	mov	r1, #7	; 0x7
    b4a0:	e3a0000f 	mov	r0, #15	; 0xf
    b4a4:	ebfffc99 	bl	a710 <flash_wait_device_status>
    b4a8:	e3a01006 	mov	r1, #6	; 0x6
    b4ac:	e3a0000f 	mov	r0, #15	; 0xf
    b4b0:	ebfffc96 	bl	a710 <flash_wait_device_status>
    b4b4:	e59f3114 	ldr	r3, [pc, #276]	; b5d0 <flash_nand_vs_write+0x458>
    b4b8:	e59d203c 	ldr	r2, [sp, #60]
    b4bc:	e593300c 	ldr	r3, [r3, #12]
    b4c0:	e1a01822 	lsr	r1, r2, #16
    b4c4:	e3530001 	cmp	r3, #1	; 0x1
    b4c8:	e1a03422 	lsr	r3, r2, #8
    b4cc:	e5cd3021 	strb	r3, [sp, #33]
    b4d0:	e3a03000 	mov	r3, #0	; 0x0
    b4d4:	e5cd3023 	strb	r3, [sp, #35]
    b4d8:	e2833003 	add	r3, r3, #3	; 0x3
    b4dc:	e5cd1020 	strb	r1, [sp, #32]
    b4e0:	e5cd2022 	strb	r2, [sp, #34]
    b4e4:	e5cd3024 	strb	r3, [sp, #36]
    b4e8:	1a00001c 	bne	b560 <flash_nand_vs_write+0x3e8>
    b4ec:	e59f30e4 	ldr	r3, [pc, #228]	; b5d8 <flash_nand_vs_write+0x460>
    b4f0:	e5933000 	ldr	r3, [r3]
    b4f4:	e3530000 	cmp	r3, #0	; 0x0
    b4f8:	0a000003 	beq	b50c <flash_nand_vs_write+0x394>
    b4fc:	e59f00f8 	ldr	r0, [pc, #248]	; b5fc <flash_nand_vs_write+0x484>
    b500:	e59f30d8 	ldr	r3, [pc, #216]	; b5e0 <flash_nand_vs_write+0x468>
    b504:	e1a0e00f 	mov	lr, pc
    b508:	e593f000 	ldr	pc, [r3]
    b50c:	e3a04000 	mov	r4, #0	; 0x0
    b510:	ebfffdc4 	bl	ac28 <nand_ecc_enable>
    b514:	e3500000 	cmp	r0, #0	; 0x0
    b518:	e2844001 	add	r4, r4, #1	; 0x1
    b51c:	0a000001 	beq	b528 <flash_nand_vs_write+0x3b0>
    b520:	e354000a 	cmp	r4, #10	; 0xa
    b524:	1afffff9 	bne	b510 <flash_nand_vs_write+0x398>
    b528:	e59f30a8 	ldr	r3, [pc, #168]	; b5d8 <flash_nand_vs_write+0x460>
    b52c:	e5933000 	ldr	r3, [r3]
    b530:	e3530000 	cmp	r3, #0	; 0x0
    b534:	0a000003 	beq	b548 <flash_nand_vs_write+0x3d0>
    b538:	e59f00c0 	ldr	r0, [pc, #192]	; b600 <flash_nand_vs_write+0x488>
    b53c:	e59f309c 	ldr	r3, [pc, #156]	; b5e0 <flash_nand_vs_write+0x468>
    b540:	e1a0e00f 	mov	lr, pc
    b544:	e593f000 	ldr	pc, [r3]
    b548:	e3a02000 	mov	r2, #0	; 0x0
    b54c:	e3a03002 	mov	r3, #2	; 0x2
    b550:	e5cd3024 	strb	r3, [sp, #36]
    b554:	e5cd2026 	strb	r2, [sp, #38]
    b558:	e5cd2020 	strb	r2, [sp, #32]
    b55c:	e5cd2021 	strb	r2, [sp, #33]
    b560:	e28d0014 	add	r0, sp, #20	; 0x14
    b564:	e58d602c 	str	r6, [sp, #44]
    b568:	e58d6038 	str	r6, [sp, #56]
    b56c:	ebfffb0f 	bl	a1b0 <spi_read_write_data>
    b570:	e59f3058 	ldr	r3, [pc, #88]	; b5d0 <flash_nand_vs_write+0x458>
    b574:	e593300c 	ldr	r3, [r3, #12]
    b578:	e3530001 	cmp	r3, #1	; 0x1
    b57c:	1a000010 	bne	b5c4 <flash_nand_vs_write+0x44c>
    b580:	e085400b 	add	r4, r5, fp
    b584:	e3a0100f 	mov	r1, #15	; 0xf
    b588:	e1a00004 	mov	r0, r4
    b58c:	ebfffd4a 	bl	aabc <nand_write_cache_to_memory>
    b590:	ebfffd81 	bl	ab9c <nand_blk_prg_era_fail>
    b594:	e3500001 	cmp	r0, #1	; 0x1
    b598:	1a000009 	bne	b5c4 <flash_nand_vs_write+0x44c>
    b59c:	e59d100c 	ldr	r1, [sp, #12]
    b5a0:	e1a00004 	mov	r0, r4
    b5a4:	eb001386 	bl	103c4 <__aeabi_idiv>
    b5a8:	e1a02005 	mov	r2, r5
    b5ac:	e1a01000 	mov	r1, r0
    b5b0:	e59f3028 	ldr	r3, [pc, #40]	; b5e0 <flash_nand_vs_write+0x468>
    b5b4:	e59f004c 	ldr	r0, [pc, #76]	; b608 <flash_nand_vs_write+0x490>
    b5b8:	e1a0e00f 	mov	lr, pc
    b5bc:	e593f000 	ldr	pc, [r3]
    b5c0:	e3e08002 	mvn	r8, #2	; 0x2
    b5c4:	e1a00008 	mov	r0, r8
    b5c8:	e28dd074 	add	sp, sp, #116	; 0x74
    b5cc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b5d0:	0800d250 	.word	0x0800d250
    b5d4:	0800d130 	.word	0x0800d130
    b5d8:	0800d2b0 	.word	0x0800d2b0
    b5dc:	0001250e 	.word	0x0001250e
    b5e0:	0800d764 	.word	0x0800d764
    b5e4:	00013c3a 	.word	0x00013c3a
    b5e8:	000118c2 	.word	0x000118c2
    b5ec:	00013c69 	.word	0x00013c69
    b5f0:	00013c8c 	.word	0x00013c8c
    b5f4:	00013caf 	.word	0x00013caf
    b5f8:	00013ccb 	.word	0x00013ccb
    b5fc:	00013ced 	.word	0x00013ced
    b600:	00013d1b 	.word	0x00013d1b
    b604:	00013d75 	.word	0x00013d75
    b608:	00013d48 	.word	0x00013d48

0000b60c <flash_nand_vs_erase>:
    b60c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b610:	e59f418c 	ldr	r4, [pc, #396]	; b7a4 <flash_nand_vs_erase+0x198>
    b614:	e24dd064 	sub	sp, sp, #100	; 0x64
    b618:	e594305c 	ldr	r3, [r4, #92]
    b61c:	e1a06000 	mov	r6, r0
    b620:	e3530001 	cmp	r3, #1	; 0x1
    b624:	e1a07001 	mov	r7, r1
    b628:	e1a05002 	mov	r5, r2
    b62c:	0a000004 	beq	b644 <flash_nand_vs_erase+0x38>
    b630:	ebfff982 	bl	9c40 <ssp_manager_init>
    b634:	e3a03064 	mov	r3, #100	; 0x64
    b638:	e5843050 	str	r3, [r4, #80]
    b63c:	e3a0300f 	mov	r3, #15	; 0xf
    b640:	e5843000 	str	r3, [r4]
    b644:	e59f8158 	ldr	r8, [pc, #344]	; b7a4 <flash_nand_vs_erase+0x198>
    b648:	e5984000 	ldr	r4, [r8]
    b64c:	e354000f 	cmp	r4, #15	; 0xf
    b650:	0a000005 	beq	b66c <flash_nand_vs_erase+0x60>
    b654:	e59f014c 	ldr	r0, [pc, #332]	; b7a8 <flash_nand_vs_erase+0x19c>
    b658:	e59f314c 	ldr	r3, [pc, #332]	; b7ac <flash_nand_vs_erase+0x1a0>
    b65c:	e1a0e00f 	mov	lr, pc
    b660:	e593f000 	ldr	pc, [r3]
    b664:	e3e00000 	mvn	r0, #0	; 0x0
    b668:	ea00004b 	b	b79c <flash_nand_vs_erase+0x190>
    b66c:	e59f213c 	ldr	r2, [pc, #316]	; b7b0 <flash_nand_vs_erase+0x1a4>
    b670:	e3a03000 	mov	r3, #0	; 0x0
    b674:	e0922006 	adds	r2, r2, r6
    b678:	e0a33007 	adc	r3, r3, r7
    b67c:	e0922005 	adds	r2, r2, r5
    b680:	e2a33000 	adc	r3, r3, #0	; 0x0
    b684:	e1a018a2 	lsr	r1, r2, #17
    b688:	e181a783 	orr	sl, r1, r3, lsl #15
    b68c:	ebfffbf8 	bl	a674 <flash_write_enable>
    b690:	e1a00004 	mov	r0, r4
    b694:	e3a01007 	mov	r1, #7	; 0x7
    b698:	ebfffc1c 	bl	a710 <flash_wait_device_status>
    b69c:	e3a01006 	mov	r1, #6	; 0x6
    b6a0:	e1a00004 	mov	r0, r4
    b6a4:	ebfffc19 	bl	a710 <flash_wait_device_status>
    b6a8:	e28d0004 	add	r0, sp, #4	; 0x4
    b6ac:	ebfffa8b 	bl	a0e0 <spi_transfer_init>
    b6b0:	e5983000 	ldr	r3, [r8]
    b6b4:	e3a04000 	mov	r4, #0	; 0x0
    b6b8:	e353000f 	cmp	r3, #15	; 0xf
    b6bc:	e1a058a6 	lsr	r5, r6, #17
    b6c0:	05cd3017 	strbeq	r3, [sp, #23]
    b6c4:	e1855787 	orr	r5, r5, r7, lsl #15
    b6c8:	e3e03027 	mvn	r3, #39	; 0x27
    b6cc:	e3a0001f 	mov	r0, #31	; 0x1f
    b6d0:	e3a010a0 	mov	r1, #160	; 0xa0
    b6d4:	e1a02004 	mov	r2, r4
    b6d8:	e5cd3004 	strb	r3, [sp, #4]
    b6dc:	e58d400c 	str	r4, [sp, #12]
    b6e0:	e5cd4005 	strb	r4, [sp, #5]
    b6e4:	e1a06005 	mov	r6, r5
    b6e8:	ebfffbc8 	bl	a610 <flash_set_device_status>
    b6ec:	e59fb0b0 	ldr	fp, [pc, #176]	; b7a4 <flash_nand_vs_erase+0x198>
    b6f0:	e28d7004 	add	r7, sp, #4	; 0x4
    b6f4:	e3a09004 	mov	r9, #4	; 0x4
    b6f8:	e3a08019 	mov	r8, #25	; 0x19
    b6fc:	ea00001d 	b	b778 <flash_nand_vs_erase+0x16c>
    b700:	ebfffbdb 	bl	a674 <flash_write_enable>
    b704:	e3a01007 	mov	r1, #7	; 0x7
    b708:	e3a0000f 	mov	r0, #15	; 0xf
    b70c:	ebfffbff 	bl	a710 <flash_wait_device_status>
    b710:	e3a01006 	mov	r1, #6	; 0x6
    b714:	e3a0000f 	mov	r0, #15	; 0xf
    b718:	ebfffbfc 	bl	a710 <flash_wait_device_status>
    b71c:	e59b3030 	ldr	r3, [fp, #48]
    b720:	e1a00007 	mov	r0, r7
    b724:	e0030693 	mul	r3, r3, r6
    b728:	e1a02403 	lsl	r2, r3, #8
    b72c:	e2033cff 	and	r3, r3, #65280	; 0xff00
    b730:	e1822423 	orr	r2, r2, r3, lsr #8
    b734:	e1c720b2 	strh	r2, [r7, #2]
    b738:	e2866001 	add	r6, r6, #1	; 0x1
    b73c:	e5cd9008 	strb	r9, [sp, #8]
    b740:	ebfffa9a 	bl	a1b0 <spi_read_write_data>
    b744:	e3a01006 	mov	r1, #6	; 0x6
    b748:	e3a0000f 	mov	r0, #15	; 0xf
    b74c:	ebfffbef 	bl	a710 <flash_wait_device_status>
    b750:	ebfffd11 	bl	ab9c <nand_blk_prg_era_fail>
    b754:	e3560000 	cmp	r6, #0	; 0x0
    b758:	e28610ff 	add	r1, r6, #255	; 0xff
    b75c:	a1a01006 	movge	r1, r6
    b760:	e1a01441 	asr	r1, r1, #8
    b764:	e0010198 	mul	r1, r8, r1
    b768:	e59f0044 	ldr	r0, [pc, #68]	; b7b4 <flash_nand_vs_erase+0x1a8>
    b76c:	e59f3038 	ldr	r3, [pc, #56]	; b7ac <flash_nand_vs_erase+0x1a0>
    b770:	e1a0e00f 	mov	lr, pc
    b774:	e593f000 	ldr	pc, [r3]
    b778:	e0843005 	add	r3, r4, r5
    b77c:	e153000a 	cmp	r3, sl
    b780:	e2844001 	add	r4, r4, #1	; 0x1
    b784:	baffffdd 	blt	b700 <flash_nand_vs_erase+0xf4>
    b788:	e59f0028 	ldr	r0, [pc, #40]	; b7b8 <flash_nand_vs_erase+0x1ac>
    b78c:	e59f3018 	ldr	r3, [pc, #24]	; b7ac <flash_nand_vs_erase+0x1a0>
    b790:	e1a0e00f 	mov	lr, pc
    b794:	e593f000 	ldr	pc, [r3]
    b798:	e3a00000 	mov	r0, #0	; 0x0
    b79c:	e28dd064 	add	sp, sp, #100	; 0x64
    b7a0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b7a4:	0800d250 	.word	0x0800d250
    b7a8:	00013d89 	.word	0x00013d89
    b7ac:	0800d764 	.word	0x0800d764
    b7b0:	0001ffff 	.word	0x0001ffff
    b7b4:	00013db1 	.word	0x00013db1
    b7b8:	00013dc5 	.word	0x00013dc5

0000b7bc <spi_module_init>:
    b7bc:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    b7c0:	e2504000 	subs	r4, r0, #0	; 0x0
    b7c4:	e24dd00c 	sub	sp, sp, #12	; 0xc
    b7c8:	1a000005 	bne	b7e4 <spi_module_init+0x28>
    b7cc:	e59f0168 	ldr	r0, [pc, #360]	; b93c <spi_module_init+0x180>
    b7d0:	e59f3168 	ldr	r3, [pc, #360]	; b940 <spi_module_init+0x184>
    b7d4:	e1a0e00f 	mov	lr, pc
    b7d8:	e593f000 	ldr	pc, [r3]
    b7dc:	e3e00001 	mvn	r0, #1	; 0x1
    b7e0:	ea000053 	b	b934 <spi_module_init+0x178>
    b7e4:	e59f0158 	ldr	r0, [pc, #344]	; b944 <spi_module_init+0x188>
    b7e8:	ebffd62c 	bl	10a0 <module_get_clock>
    b7ec:	e2508000 	subs	r8, r0, #0	; 0x0
    b7f0:	a3a07002 	movge	r7, #2	; 0x2
    b7f4:	a59fa14c 	ldrge	sl, [pc, #332]	; b948 <spi_module_init+0x18c>
    b7f8:	aa000013 	bge	b84c <spi_module_init+0x90>
    b7fc:	e59f0148 	ldr	r0, [pc, #328]	; b94c <spi_module_init+0x190>
    b800:	e1a01004 	mov	r1, r4
    b804:	e59f3134 	ldr	r3, [pc, #308]	; b940 <spi_module_init+0x184>
    b808:	e1a0e00f 	mov	lr, pc
    b80c:	e593f000 	ldr	pc, [r3]
    b810:	e3e00000 	mvn	r0, #0	; 0x0
    b814:	ea000046 	b	b934 <spi_module_init+0x178>
    b818:	e1a05004 	mov	r5, r4
    b81c:	e1a01006 	mov	r1, r6
    b820:	e1a00008 	mov	r0, r8
    b824:	eb0012e6 	bl	103c4 <__aeabi_idiv>
    b828:	e150000a 	cmp	r0, sl
    b82c:	e0866007 	add	r6, r6, r7
    b830:	e2854001 	add	r4, r5, #1	; 0x1
    b834:	da000007 	ble	b858 <spi_module_init+0x9c>
    b838:	e3540c01 	cmp	r4, #256	; 0x100
    b83c:	1afffff5 	bne	b818 <spi_module_init+0x5c>
    b840:	e2877002 	add	r7, r7, #2	; 0x2
    b844:	e3570c01 	cmp	r7, #256	; 0x100
    b848:	0a000003 	beq	b85c <spi_module_init+0xa0>
    b84c:	e1a06007 	mov	r6, r7
    b850:	e3a05000 	mov	r5, #0	; 0x0
    b854:	eafffff0 	b	b81c <spi_module_init+0x60>
    b858:	e1a04005 	mov	r4, r5
    b85c:	e35700fe 	cmp	r7, #254	; 0xfe
    b860:	c35400ff 	cmpgt	r4, #255	; 0xff
    b864:	da00000a 	ble	b894 <spi_module_init+0xd8>
    b868:	e3a01002 	mov	r1, #2	; 0x2
    b86c:	e3a02027 	mov	r2, #39	; 0x27
    b870:	e59f00d8 	ldr	r0, [pc, #216]	; b950 <spi_module_init+0x194>
    b874:	e59f30c4 	ldr	r3, [pc, #196]	; b940 <spi_module_init+0x184>
    b878:	e1a0e00f 	mov	lr, pc
    b87c:	e593f000 	ldr	pc, [r3]
    b880:	e1a00008 	mov	r0, r8
    b884:	e3a01050 	mov	r1, #80	; 0x50
    b888:	eb0012cd 	bl	103c4 <__aeabi_idiv>
    b88c:	e3a07002 	mov	r7, #2	; 0x2
    b890:	e3a04027 	mov	r4, #39	; 0x27
    b894:	e59f50a4 	ldr	r5, [pc, #164]	; b940 <spi_module_init+0x184>
    b898:	e1a02007 	mov	r2, r7
    b89c:	e1a03004 	mov	r3, r4
    b8a0:	e58d0000 	str	r0, [sp]
    b8a4:	e1a01008 	mov	r1, r8
    b8a8:	e59f00a4 	ldr	r0, [pc, #164]	; b954 <spi_module_init+0x198>
    b8ac:	e1a0e00f 	mov	lr, pc
    b8b0:	e595f000 	ldr	pc, [r5]
    b8b4:	e1a03404 	lsl	r3, r4, #8
    b8b8:	e59f4098 	ldr	r4, [pc, #152]	; b958 <spi_module_init+0x19c>
    b8bc:	e3a06000 	mov	r6, #0	; 0x0
    b8c0:	e38330c7 	orr	r3, r3, #199	; 0xc7
    b8c4:	e5847010 	str	r7, [r4, #16]
    b8c8:	e5846014 	str	r6, [r4, #20]
    b8cc:	e5846000 	str	r6, [r4]
    b8d0:	e5846004 	str	r6, [r4, #4]
    b8d4:	e5843000 	str	r3, [r4]
    b8d8:	e3a0300a 	mov	r3, #10	; 0xa
    b8dc:	e5843004 	str	r3, [r4, #4]
    b8e0:	e5941000 	ldr	r1, [r4]
    b8e4:	e59f0070 	ldr	r0, [pc, #112]	; b95c <spi_module_init+0x1a0>
    b8e8:	e1a0e00f 	mov	lr, pc
    b8ec:	e595f000 	ldr	pc, [r5]
    b8f0:	e5941004 	ldr	r1, [r4, #4]
    b8f4:	e59f0064 	ldr	r0, [pc, #100]	; b960 <spi_module_init+0x1a4>
    b8f8:	e1a0e00f 	mov	lr, pc
    b8fc:	e595f000 	ldr	pc, [r5]
    b900:	e59f005c 	ldr	r0, [pc, #92]	; b964 <spi_module_init+0x1a8>
    b904:	e1a0e00f 	mov	lr, pc
    b908:	e595f000 	ldr	pc, [r5]
    b90c:	e59f0054 	ldr	r0, [pc, #84]	; b968 <spi_module_init+0x1ac>
    b910:	e1a0e00f 	mov	lr, pc
    b914:	e595f000 	ldr	pc, [r5]
    b918:	e59f004c 	ldr	r0, [pc, #76]	; b96c <spi_module_init+0x1b0>
    b91c:	e1a0e00f 	mov	lr, pc
    b920:	e595f000 	ldr	pc, [r5]
    b924:	e59f0044 	ldr	r0, [pc, #68]	; b970 <spi_module_init+0x1b4>
    b928:	e1a0e00f 	mov	lr, pc
    b92c:	e595f000 	ldr	pc, [r5]
    b930:	e1a00006 	mov	r0, r6
    b934:	e28dd00c 	add	sp, sp, #12	; 0xc
    b938:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    b93c:	00013dcd 	.word	0x00013dcd
    b940:	0800d764 	.word	0x0800d764
    b944:	2d00a2a0 	.word	0x2d00a2a0
    b948:	004c4b40 	.word	0x004c4b40
    b94c:	00013de3 	.word	0x00013de3
    b950:	00013e17 	.word	0x00013e17
    b954:	00013e71 	.word	0x00013e71
    b958:	20500000 	.word	0x20500000
    b95c:	00013eb2 	.word	0x00013eb2
    b960:	00013ed6 	.word	0x00013ed6
    b964:	00013efa 	.word	0x00013efa
    b968:	00013f1f 	.word	0x00013f1f
    b96c:	00013f3f 	.word	0x00013f3f
    b970:	00013f6d 	.word	0x00013f6d

0000b974 <flash_nand_vs_reset>:
    b974:	e92d4030 	push	{r4, r5, lr}
    b978:	e59f4120 	ldr	r4, [pc, #288]	; baa0 <flash_nand_vs_reset+0x12c>
    b97c:	e24dd004 	sub	sp, sp, #4	; 0x4
    b980:	e59f511c 	ldr	r5, [pc, #284]	; baa4 <flash_nand_vs_reset+0x130>
    b984:	e59f011c 	ldr	r0, [pc, #284]	; baa8 <flash_nand_vs_reset+0x134>
    b988:	e1a0e00f 	mov	lr, pc
    b98c:	e595f000 	ldr	pc, [r5]
    b990:	e594305c 	ldr	r3, [r4, #92]
    b994:	e3530001 	cmp	r3, #1	; 0x1
    b998:	0a000007 	beq	b9bc <flash_nand_vs_reset+0x48>
    b99c:	ebfff8a7 	bl	9c40 <ssp_manager_init>
    b9a0:	e3a03064 	mov	r3, #100	; 0x64
    b9a4:	e5843050 	str	r3, [r4, #80]
    b9a8:	e3a0300f 	mov	r3, #15	; 0xf
    b9ac:	e5843000 	str	r3, [r4]
    b9b0:	e59f00f4 	ldr	r0, [pc, #244]	; baac <flash_nand_vs_reset+0x138>
    b9b4:	e1a0e00f 	mov	lr, pc
    b9b8:	e595f000 	ldr	pc, [r5]
    b9bc:	e59f30ec 	ldr	r3, [pc, #236]	; bab0 <flash_nand_vs_reset+0x13c>
    b9c0:	e5933000 	ldr	r3, [r3]
    b9c4:	e3530000 	cmp	r3, #0	; 0x0
    b9c8:	0a000007 	beq	b9ec <flash_nand_vs_reset+0x78>
    b9cc:	e59f20cc 	ldr	r2, [pc, #204]	; baa0 <flash_nand_vs_reset+0x12c>
    b9d0:	e59f00dc 	ldr	r0, [pc, #220]	; bab4 <flash_nand_vs_reset+0x140>
    b9d4:	e5923050 	ldr	r3, [r2, #80]
    b9d8:	e5921000 	ldr	r1, [r2]
    b9dc:	e59fc0c0 	ldr	ip, [pc, #192]	; baa4 <flash_nand_vs_reset+0x130>
    b9e0:	e5922004 	ldr	r2, [r2, #4]
    b9e4:	e1a0e00f 	mov	lr, pc
    b9e8:	e59cf000 	ldr	pc, [ip]
    b9ec:	e59f30ac 	ldr	r3, [pc, #172]	; baa0 <flash_nand_vs_reset+0x12c>
    b9f0:	e5933000 	ldr	r3, [r3]
    b9f4:	e353000f 	cmp	r3, #15	; 0xf
    b9f8:	1a000020 	bne	ba80 <flash_nand_vs_reset+0x10c>
    b9fc:	e59f00b4 	ldr	r0, [pc, #180]	; bab8 <flash_nand_vs_reset+0x144>
    ba00:	ebffd347 	bl	724 <module_enable>
    ba04:	e59f20b0 	ldr	r2, [pc, #176]	; babc <flash_nand_vs_reset+0x148>
    ba08:	e59f00b0 	ldr	r0, [pc, #176]	; bac0 <flash_nand_vs_reset+0x14c>
    ba0c:	e5923404 	ldr	r3, [r2, #1028]
    ba10:	e3833008 	orr	r3, r3, #8	; 0x8
    ba14:	e5823404 	str	r3, [r2, #1028]
    ba18:	ebffff67 	bl	b7bc <spi_module_init>
    ba1c:	e59f20a0 	ldr	r2, [pc, #160]	; bac4 <flash_nand_vs_reset+0x150>
    ba20:	e3a0000b 	mov	r0, #11	; 0xb
    ba24:	e5923000 	ldr	r3, [r2]
    ba28:	e3833004 	orr	r3, r3, #4	; 0x4
    ba2c:	e5823000 	str	r3, [r2]
    ba30:	ebffd377 	bl	814 <pads_init_module>
    ba34:	e3a02001 	mov	r2, #1	; 0x1
    ba38:	e3a0004a 	mov	r0, #74	; 0x4a
    ba3c:	e3a01002 	mov	r1, #2	; 0x2
    ba40:	ebffd47f 	bl	c44 <pads_chmod>
    ba44:	e59f207c 	ldr	r2, [pc, #124]	; bac8 <flash_nand_vs_reset+0x154>
    ba48:	ea000000 	b	ba50 <flash_nand_vs_reset+0xdc>
    ba4c:	e5923008 	ldr	r3, [r2, #8]
    ba50:	e592300c 	ldr	r3, [r2, #12]
    ba54:	e3130004 	tst	r3, #4	; 0x4
    ba58:	1afffffb 	bne	ba4c <flash_nand_vs_reset+0xd8>
    ba5c:	e3a00002 	mov	r0, #2	; 0x2
    ba60:	ebfff8d6 	bl	9dc0 <spi_set_intr_recv_timeout>
    ba64:	e3a00008 	mov	r0, #8	; 0x8
    ba68:	ebfff940 	bl	9f70 <spi_set_intr_transmit_fifo>
    ba6c:	e3a00004 	mov	r0, #4	; 0x4
    ba70:	ebfff908 	bl	9e98 <spi_set_intr_recv_fifo>
    ba74:	e3a00001 	mov	r0, #1	; 0x1
    ba78:	ebfff89a 	bl	9ce8 <spi_set_intr_recv_overrun>
    ba7c:	ea000003 	b	ba90 <flash_nand_vs_reset+0x11c>
    ba80:	e59f0044 	ldr	r0, [pc, #68]	; bacc <flash_nand_vs_reset+0x158>
    ba84:	e59f3018 	ldr	r3, [pc, #24]	; baa4 <flash_nand_vs_reset+0x130>
    ba88:	e1a0e00f 	mov	lr, pc
    ba8c:	e593f000 	ldr	pc, [r3]
    ba90:	ebfffb0a 	bl	a6c0 <nand_flash_reset>
    ba94:	e3a00000 	mov	r0, #0	; 0x0
    ba98:	e28dd004 	add	sp, sp, #4	; 0x4
    ba9c:	e8bd8030 	pop	{r4, r5, pc}
    baa0:	0800d250 	.word	0x0800d250
    baa4:	0800d764 	.word	0x0800d764
    baa8:	00013f98 	.word	0x00013f98
    baac:	00013fc3 	.word	0x00013fc3
    bab0:	0800d2b0 	.word	0x0800d2b0
    bab4:	00013ad9 	.word	0x00013ad9
    bab8:	2d011400 	.word	0x2d011400
    babc:	2d011000 	.word	0x2d011000
    bac0:	00013ff8 	.word	0x00013ff8
    bac4:	2d009000 	.word	0x2d009000
    bac8:	20500000 	.word	0x20500000
    bacc:	00013c8c 	.word	0x00013c8c

0000bad0 <ram_addr2offs>:
    bad0:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    bad4:	93a02101 	movls	r2, #1073741824	; 0x40000000
    bad8:	e3a01000 	mov	r1, #0	; 0x0
    badc:	83a02103 	movhi	r2, #-1073741824	; 0xc0000000
    bae0:	93a03000 	movls	r3, #0	; 0x0
    bae4:	83e03000 	mvnhi	r3, #0	; 0x0
    bae8:	e0922000 	adds	r2, r2, r0
    baec:	e0a33001 	adc	r3, r3, r1
    baf0:	e1a01003 	mov	r1, r3
    baf4:	e1a00002 	mov	r0, r2
    baf8:	e12fff1e 	bx	lr

0000bafc <ram_offs2addr>:
    bafc:	e3510000 	cmp	r1, #0	; 0x0
    bb00:	ca000002 	bgt	bb10 <ram_offs2addr+0x14>
    bb04:	1a000003 	bne	bb18 <ram_offs2addr+0x1c>
    bb08:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    bb0c:	9a000001 	bls	bb18 <ram_offs2addr+0x1c>
    bb10:	e2800103 	add	r0, r0, #-1073741824	; 0xc0000000
    bb14:	e12fff1e 	bx	lr
    bb18:	e2800101 	add	r0, r0, #1073741824	; 0x40000000
    bb1c:	e12fff1e 	bx	lr

0000bb20 <ram_vs_align>:
    bb20:	e3a00c02 	mov	r0, #512	; 0x200
    bb24:	e12fff1e 	bx	lr

0000bb28 <ram_vs_reset>:
    bb28:	e3a00000 	mov	r0, #0	; 0x0
    bb2c:	e12fff1e 	bx	lr

0000bb30 <ram_vs_write>:
    bb30:	e92d4010 	push	{r4, lr}
    bb34:	e3530000 	cmp	r3, #0	; 0x0
    bb38:	e1a01000 	mov	r1, r0
    bb3c:	e59d4008 	ldr	r4, [sp, #8]
    bb40:	ca000002 	bgt	bb50 <ram_vs_write+0x20>
    bb44:	1a000003 	bne	bb58 <ram_vs_write+0x28>
    bb48:	e3720107 	cmn	r2, #-1073741823	; 0xc0000001
    bb4c:	9a000001 	bls	bb58 <ram_vs_write+0x28>
    bb50:	e2820103 	add	r0, r2, #-1073741824	; 0xc0000000
    bb54:	ea000000 	b	bb5c <ram_vs_write+0x2c>
    bb58:	e2820101 	add	r0, r2, #1073741824	; 0x40000000
    bb5c:	e1a02004 	mov	r2, r4
    bb60:	ebffd626 	bl	1400 <memcpy>
    bb64:	e1a00004 	mov	r0, r4
    bb68:	e8bd8010 	pop	{r4, pc}

0000bb6c <ram_vs_read>:
    bb6c:	e92d4010 	push	{r4, lr}
    bb70:	e3530000 	cmp	r3, #0	; 0x0
    bb74:	e59d4008 	ldr	r4, [sp, #8]
    bb78:	ca000002 	bgt	bb88 <ram_vs_read+0x1c>
    bb7c:	1a000003 	bne	bb90 <ram_vs_read+0x24>
    bb80:	e3720107 	cmn	r2, #-1073741823	; 0xc0000001
    bb84:	9a000001 	bls	bb90 <ram_vs_read+0x24>
    bb88:	e2821103 	add	r1, r2, #-1073741824	; 0xc0000000
    bb8c:	ea000000 	b	bb94 <ram_vs_read+0x28>
    bb90:	e2821101 	add	r1, r2, #1073741824	; 0x40000000
    bb94:	e1a02004 	mov	r2, r4
    bb98:	ebffd618 	bl	1400 <memcpy>
    bb9c:	e1a00004 	mov	r0, r4
    bba0:	e8bd8010 	pop	{r4, pc}

0000bba4 <udc_get_cable_state>:
    bba4:	e3a00001 	mov	r0, #1	; 0x1
    bba8:	e12fff1e 	bx	lr

0000bbac <udc_disconnect>:
    bbac:	e59f103c 	ldr	r1, [pc, #60]	; bbf0 <udc_disconnect+0x44>
    bbb0:	e59f303c 	ldr	r3, [pc, #60]	; bbf4 <udc_disconnect+0x48>
    bbb4:	e5912010 	ldr	r2, [r1, #16]
    bbb8:	e3a00000 	mov	r0, #0	; 0x0
    bbbc:	e3822001 	orr	r2, r2, #1	; 0x1
    bbc0:	e92d4010 	push	{r4, lr}
    bbc4:	e5c30000 	strb	r0, [r3]
    bbc8:	e1a04001 	mov	r4, r1
    bbcc:	e5812010 	str	r2, [r1, #16]
    bbd0:	ea000000 	b	bbd8 <udc_disconnect+0x2c>
    bbd4:	ebfff34c 	bl	890c <udelay>
    bbd8:	e5943010 	ldr	r3, [r4, #16]
    bbdc:	e3a0000a 	mov	r0, #10	; 0xa
    bbe0:	e2133001 	ands	r3, r3, #1	; 0x1
    bbe4:	1afffffa 	bne	bbd4 <udc_disconnect+0x28>
    bbe8:	e1a00003 	mov	r0, r3
    bbec:	e8bd8010 	pop	{r4, pc}
    bbf0:	27100000 	.word	0x27100000
    bbf4:	0800d2bc 	.word	0x0800d2bc

0000bbf8 <udc_event_handle>:
    bbf8:	e59f2768 	ldr	r2, [pc, #1896]	; c368 <udc_event_handle+0x770>
    bbfc:	e59f3768 	ldr	r3, [pc, #1896]	; c36c <udc_event_handle+0x774>
    bc00:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    bc04:	e5934014 	ldr	r4, [r3, #20]
    bc08:	e5923000 	ldr	r3, [r2]
    bc0c:	e59f0758 	ldr	r0, [pc, #1880]	; c36c <udc_event_handle+0x774>
    bc10:	e1530004 	cmp	r3, r4
    bc14:	e204ca03 	and	ip, r4, #12288	; 0x3000
    bc18:	15824000 	strne	r4, [r2]
    bc1c:	e2043b03 	and	r3, r4, #3072	; 0xc00
    bc20:	e35c0a01 	cmp	ip, #4096	; 0x1000
    bc24:	e24dd008 	sub	sp, sp, #8	; 0x8
    bc28:	e5803014 	str	r3, [r0, #20]
    bc2c:	0a000002 	beq	bc3c <udc_event_handle+0x44>
    bc30:	e35c0a02 	cmp	ip, #8192	; 0x2000
    bc34:	1a000034 	bne	bd0c <udc_event_handle+0x114>
    bc38:	ea000025 	b	bcd4 <udc_event_handle+0xdc>
    bc3c:	e3a03302 	mov	r3, #134217728	; 0x8000000
    bc40:	e5803b00 	str	r3, [r0, #2816]
    bc44:	e5803b40 	str	r3, [r0, #2880]
    bc48:	e59f3720 	ldr	r3, [pc, #1824]	; c370 <udc_event_handle+0x778>
    bc4c:	e3a02009 	mov	r2, #9	; 0x9
    bc50:	e580381c 	str	r3, [r0, #2076]
    bc54:	e3a03b02 	mov	r3, #2048	; 0x800
    bc58:	e5802814 	str	r2, [r0, #2068]
    bc5c:	e5802810 	str	r2, [r0, #2064]
    bc60:	e5803024 	str	r3, [r0, #36]
    bc64:	e2833301 	add	r3, r3, #67108864	; 0x4000000
    bc68:	e5803028 	str	r3, [r0, #40]
    bc6c:	e2833b01 	add	r3, r3, #1024	; 0x400
    bc70:	e5803104 	str	r3, [r0, #260]
    bc74:	e59f36f8 	ldr	r3, [pc, #1784]	; c374 <udc_event_handle+0x77c>
    bc78:	e1a05000 	mov	r5, r0
    bc7c:	e580305c 	str	r3, [r0, #92]
    bc80:	e5903010 	ldr	r3, [r0, #16]
    bc84:	e3c33d1f 	bic	r3, r3, #1984	; 0x7c0
    bc88:	e3833e43 	orr	r3, r3, #1072	; 0x430
    bc8c:	e5803010 	str	r3, [r0, #16]
    bc90:	ea000000 	b	bc98 <udc_event_handle+0xa0>
    bc94:	ebfff31c 	bl	890c <udelay>
    bc98:	e5953010 	ldr	r3, [r5, #16]
    bc9c:	e3a00001 	mov	r0, #1	; 0x1
    bca0:	e3130030 	tst	r3, #48	; 0x30
    bca4:	e59f16c0 	ldr	r1, [pc, #1728]	; c36c <udc_event_handle+0x774>
    bca8:	1afffff9 	bne	bc94 <udc_event_handle+0x9c>
    bcac:	e59f36c4 	ldr	r3, [pc, #1732]	; c378 <udc_event_handle+0x780>
    bcb0:	e2800009 	add	r0, r0, #9	; 0x9
    bcb4:	e5932024 	ldr	r2, [r3, #36]
    bcb8:	e59f36bc 	ldr	r3, [pc, #1724]	; c37c <udc_event_handle+0x784>
    bcbc:	e5813b10 	str	r3, [r1, #2832]
    bcc0:	e3a03a01 	mov	r3, #4096	; 0x1000
    bcc4:	e5812b14 	str	r2, [r1, #2836]
    bcc8:	e5813014 	str	r3, [r1, #20]
    bccc:	ebfff30e 	bl	890c <udelay>
    bcd0:	ea00000d 	b	bd0c <udc_event_handle+0x114>
    bcd4:	e5902808 	ldr	r2, [r0, #2056]
    bcd8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bcdc:	e5803b00 	str	r3, [r0, #2816]
    bce0:	e1a020a2 	lsr	r2, r2, #1
    bce4:	e59f368c 	ldr	r3, [pc, #1676]	; c378 <udc_event_handle+0x780>
    bce8:	e5901018 	ldr	r1, [r0, #24]
    bcec:	e2022003 	and	r2, r2, #3	; 0x3
    bcf0:	e5c3201c 	strb	r2, [r3, #28]
    bcf4:	e59f3684 	ldr	r3, [pc, #1668]	; c380 <udc_event_handle+0x788>
    bcf8:	e3811008 	orr	r1, r1, #8	; 0x8
    bcfc:	e3a02001 	mov	r2, #1	; 0x1
    bd00:	e5801018 	str	r1, [r0, #24]
    bd04:	e5832000 	str	r2, [r3]
    bd08:	e580c014 	str	ip, [r0, #20]
    bd0c:	e59f3658 	ldr	r3, [pc, #1624]	; c36c <udc_event_handle+0x774>
    bd10:	e3140701 	tst	r4, #262144	; 0x40000
    bd14:	e5939818 	ldr	r9, [r3, #2072]
    bd18:	0a000051 	beq	be64 <udc_event_handle+0x26c>
    bd1c:	e3190001 	tst	r9, #1	; 0x1
    bd20:	0a000042 	beq	be30 <udc_event_handle+0x238>
    bd24:	e5935908 	ldr	r5, [r3, #2312]
    bd28:	e3150001 	tst	r5, #1	; 0x1
    bd2c:	0a00003b 	beq	be20 <udc_event_handle+0x228>
    bd30:	e59f3640 	ldr	r3, [pc, #1600]	; c378 <udc_event_handle+0x780>
    bd34:	e5933020 	ldr	r3, [r3, #32]
    bd38:	e2433002 	sub	r3, r3, #2	; 0x2
    bd3c:	e3530003 	cmp	r3, #3	; 0x3
    bd40:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    bd44:	ea000035 	b	be20 <udc_event_handle+0x228>
    bd48:	0000bd88 	.word	0x0000bd88
    bd4c:	0000bd58 	.word	0x0000bd58
    bd50:	0000bdbc 	.word	0x0000bdbc
    bd54:	0000bdbc 	.word	0x0000bdbc
    bd58:	e59f160c 	ldr	r1, [pc, #1548]	; c36c <udc_event_handle+0x774>
    bd5c:	e59f0614 	ldr	r0, [pc, #1556]	; c378 <udc_event_handle+0x780>
    bd60:	e5912910 	ldr	r2, [r1, #2320]
    bd64:	e590c00c 	ldr	ip, [r0, #12]
    bd68:	e3a03004 	mov	r3, #4	; 0x4
    bd6c:	e3822702 	orr	r2, r2, #524288	; 0x80000
    bd70:	e5803020 	str	r3, [r0, #32]
    bd74:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bd78:	e5812910 	str	r2, [r1, #2320]
    bd7c:	e581c914 	str	ip, [r1, #2324]
    bd80:	e5813900 	str	r3, [r1, #2304]
    bd84:	ea000025 	b	be20 <udc_event_handle+0x228>
    bd88:	e59f15dc 	ldr	r1, [pc, #1500]	; c36c <udc_event_handle+0x774>
    bd8c:	e59f05e4 	ldr	r0, [pc, #1508]	; c378 <udc_event_handle+0x780>
    bd90:	e5913b10 	ldr	r3, [r1, #2832]
    bd94:	e590c024 	ldr	ip, [r0, #36]
    bd98:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    bd9c:	e3833702 	orr	r3, r3, #524288	; 0x80000
    bda0:	e5813b10 	str	r3, [r1, #2832]
    bda4:	e3a02005 	mov	r2, #5	; 0x5
    bda8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bdac:	e581cb14 	str	ip, [r1, #2836]
    bdb0:	e5802020 	str	r2, [r0, #32]
    bdb4:	e5813b00 	str	r3, [r1, #2816]
    bdb8:	ea000018 	b	be20 <udc_event_handle+0x228>
    bdbc:	e59f05b4 	ldr	r0, [pc, #1460]	; c378 <udc_event_handle+0x780>
    bdc0:	e5d03015 	ldrb	r3, [r0, #21]
    bdc4:	e3530009 	cmp	r3, #9	; 0x9
    bdc8:	1a00000a 	bne	bdf8 <udc_event_handle+0x200>
    bdcc:	e59f1598 	ldr	r1, [pc, #1432]	; c36c <udc_event_handle+0x774>
    bdd0:	e59f35ac 	ldr	r3, [pc, #1452]	; c384 <udc_event_handle+0x78c>
    bdd4:	e3a02001 	mov	r2, #1	; 0x1
    bdd8:	e5813920 	str	r3, [r1, #2336]
    bddc:	e2433d07 	sub	r3, r3, #448	; 0x1c0
    bde0:	e5813b40 	str	r3, [r1, #2880]
    bde4:	e591381c 	ldr	r3, [r1, #2076]
    bde8:	e5c02000 	strb	r2, [r0]
    bdec:	e3833701 	orr	r3, r3, #262144	; 0x40000
    bdf0:	e3833002 	orr	r3, r3, #2	; 0x2
    bdf4:	e581381c 	str	r3, [r1, #2076]
    bdf8:	e59f256c 	ldr	r2, [pc, #1388]	; c36c <udc_event_handle+0x774>
    bdfc:	e59f1574 	ldr	r1, [pc, #1396]	; c378 <udc_event_handle+0x780>
    be00:	e59f3574 	ldr	r3, [pc, #1396]	; c37c <udc_event_handle+0x784>
    be04:	e5910024 	ldr	r0, [r1, #36]
    be08:	e5823b10 	str	r3, [r2, #2832]
    be0c:	e3a03001 	mov	r3, #1	; 0x1
    be10:	e5813020 	str	r3, [r1, #32]
    be14:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    be18:	e5820b14 	str	r0, [r2, #2836]
    be1c:	e5823b00 	str	r3, [r2, #2816]
    be20:	e300320d 	movw	r3, #525	; 0x20d
    be24:	e59f2540 	ldr	r2, [pc, #1344]	; c36c <udc_event_handle+0x774>
    be28:	e0053003 	and	r3, r5, r3
    be2c:	e5823908 	str	r3, [r2, #2312]
    be30:	e3190002 	tst	r9, #2	; 0x2
    be34:	0a00000a 	beq	be64 <udc_event_handle+0x26c>
    be38:	e59f352c 	ldr	r3, [pc, #1324]	; c36c <udc_event_handle+0x774>
    be3c:	e5932928 	ldr	r2, [r3, #2344]
    be40:	e3120001 	tst	r2, #1	; 0x1
    be44:	0a000003 	beq	be58 <udc_event_handle+0x260>
    be48:	e5933930 	ldr	r3, [r3, #2352]
    be4c:	e3d3120f 	bics	r1, r3, #-268435456	; 0xf0000000
    be50:	059f3520 	ldreq	r3, [pc, #1312]	; c378 <udc_event_handle+0x780>
    be54:	05c31002 	strbeq	r1, [r3, #2]
    be58:	e59f350c 	ldr	r3, [pc, #1292]	; c36c <udc_event_handle+0x774>
    be5c:	e2022005 	and	r2, r2, #5	; 0x5
    be60:	e5832928 	str	r2, [r3, #2344]
    be64:	e3140702 	tst	r4, #524288	; 0x80000
    be68:	0a00013c 	beq	c360 <udc_event_handle+0x768>
    be6c:	e3190801 	tst	r9, #65536	; 0x10000
    be70:	0a000124 	beq	c308 <udc_event_handle+0x710>
    be74:	e59f34f0 	ldr	r3, [pc, #1264]	; c36c <udc_event_handle+0x774>
    be78:	e593ab08 	ldr	sl, [r3, #2824]
    be7c:	e31a0001 	tst	sl, #1	; 0x1
    be80:	0a00002b 	beq	bf34 <udc_event_handle+0x33c>
    be84:	e59f34ec 	ldr	r3, [pc, #1260]	; c378 <udc_event_handle+0x780>
    be88:	e5933020 	ldr	r3, [r3, #32]
    be8c:	e2433002 	sub	r3, r3, #2	; 0x2
    be90:	e3530003 	cmp	r3, #3	; 0x3
    be94:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    be98:	ea000025 	b	bf34 <udc_event_handle+0x33c>
    be9c:	0000bed8 	.word	0x0000bed8
    bea0:	0000beac 	.word	0x0000beac
    bea4:	0000bf0c 	.word	0x0000bf0c
    bea8:	0000bf0c 	.word	0x0000bf0c
    beac:	e59f14c4 	ldr	r1, [pc, #1220]	; c378 <udc_event_handle+0x780>
    beb0:	e59f24b4 	ldr	r2, [pc, #1204]	; c36c <udc_event_handle+0x774>
    beb4:	e3a03004 	mov	r3, #4	; 0x4
    beb8:	e591000c 	ldr	r0, [r1, #12]
    bebc:	e5813020 	str	r3, [r1, #32]
    bec0:	e3a03702 	mov	r3, #524288	; 0x80000
    bec4:	e5823910 	str	r3, [r2, #2320]
    bec8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    becc:	e5820914 	str	r0, [r2, #2324]
    bed0:	e5823900 	str	r3, [r2, #2304]
    bed4:	ea000016 	b	bf34 <udc_event_handle+0x33c>
    bed8:	e59f148c 	ldr	r1, [pc, #1164]	; c36c <udc_event_handle+0x774>
    bedc:	e59f0494 	ldr	r0, [pc, #1172]	; c378 <udc_event_handle+0x780>
    bee0:	e5913b10 	ldr	r3, [r1, #2832]
    bee4:	e590c00c 	ldr	ip, [r0, #12]
    bee8:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    beec:	e3833702 	orr	r3, r3, #524288	; 0x80000
    bef0:	e5813b10 	str	r3, [r1, #2832]
    bef4:	e3a02005 	mov	r2, #5	; 0x5
    bef8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    befc:	e581cb14 	str	ip, [r1, #2836]
    bf00:	e5802020 	str	r2, [r0, #32]
    bf04:	e5813b00 	str	r3, [r1, #2816]
    bf08:	ea000009 	b	bf34 <udc_event_handle+0x33c>
    bf0c:	e59f2458 	ldr	r2, [pc, #1112]	; c36c <udc_event_handle+0x774>
    bf10:	e59f1460 	ldr	r1, [pc, #1120]	; c378 <udc_event_handle+0x780>
    bf14:	e59f3460 	ldr	r3, [pc, #1120]	; c37c <udc_event_handle+0x784>
    bf18:	e5910024 	ldr	r0, [r1, #36]
    bf1c:	e5823b10 	str	r3, [r2, #2832]
    bf20:	e3a03001 	mov	r3, #1	; 0x1
    bf24:	e5813020 	str	r3, [r1, #32]
    bf28:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bf2c:	e5820b14 	str	r0, [r2, #2836]
    bf30:	e5823b00 	str	r3, [r2, #2816]
    bf34:	e31a0004 	tst	sl, #4	; 0x4
    bf38:	159f342c 	ldrne	r3, [pc, #1068]	; c36c <udc_event_handle+0x774>
    bf3c:	13a02004 	movne	r2, #4	; 0x4
    bf40:	15832b08 	strne	r2, [r3, #2824]
    bf44:	e31a0008 	tst	sl, #8	; 0x8
    bf48:	0a0000ea 	beq	c2f8 <udc_event_handle+0x700>
    bf4c:	e31a0040 	tst	sl, #64	; 0x40
    bf50:	e59f1430 	ldr	r1, [pc, #1072]	; c388 <udc_event_handle+0x790>
    bf54:	0a000006 	beq	bf74 <udc_event_handle+0x37c>
    bf58:	e59f340c 	ldr	r3, [pc, #1036]	; c36c <udc_event_handle+0x774>
    bf5c:	e5933b14 	ldr	r3, [r3, #2836]
    bf60:	e2432004 	sub	r2, r3, #4	; 0x4
    bf64:	e2433008 	sub	r3, r3, #8	; 0x8
    bf68:	e5812004 	str	r2, [r1, #4]
    bf6c:	e5813000 	str	r3, [r1]
    bf70:	ea000005 	b	bf8c <udc_event_handle+0x394>
    bf74:	e59f33fc 	ldr	r3, [pc, #1020]	; c378 <udc_event_handle+0x780>
    bf78:	e5932024 	ldr	r2, [r3, #36]
    bf7c:	e5923000 	ldr	r3, [r2]
    bf80:	e5813000 	str	r3, [r1]
    bf84:	e5923004 	ldr	r3, [r2, #4]
    bf88:	e5813004 	str	r3, [r1, #4]
    bf8c:	e59f23e4 	ldr	r2, [pc, #996]	; c378 <udc_event_handle+0x780>
    bf90:	e5d20014 	ldrb	r0, [r2, #20]
    bf94:	e3100080 	tst	r0, #128	; 0x80
    bf98:	13a03002 	movne	r3, #2	; 0x2
    bf9c:	03a03003 	moveq	r3, #3	; 0x3
    bfa0:	e5823020 	str	r3, [r2, #32]
    bfa4:	e59f23cc 	ldr	r2, [pc, #972]	; c378 <udc_event_handle+0x780>
    bfa8:	e1d211ba 	ldrh	r1, [r2, #26]
    bfac:	e3510000 	cmp	r1, #0	; 0x0
    bfb0:	03a03004 	moveq	r3, #4	; 0x4
    bfb4:	05823020 	streq	r3, [r2, #32]
    bfb8:	e59f33b8 	ldr	r3, [pc, #952]	; c378 <udc_event_handle+0x780>
    bfbc:	e5d33015 	ldrb	r3, [r3, #21]
    bfc0:	e3530009 	cmp	r3, #9	; 0x9
    bfc4:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    bfc8:	ea0000ca 	b	c2f8 <udc_event_handle+0x700>
    bfcc:	0000c290 	.word	0x0000c290
    bfd0:	0000c2f8 	.word	0x0000c2f8
    bfd4:	0000c2f8 	.word	0x0000c2f8
    bfd8:	0000c2f8 	.word	0x0000c2f8
    bfdc:	0000c2f8 	.word	0x0000c2f8
    bfe0:	0000c1dc 	.word	0x0000c1dc
    bfe4:	0000bff4 	.word	0x0000bff4
    bfe8:	0000c2f8 	.word	0x0000c2f8
    bfec:	0000c2f8 	.word	0x0000c2f8
    bff0:	0000c22c 	.word	0x0000c22c
    bff4:	e3a00040 	mov	r0, #64	; 0x40
    bff8:	ebffd565 	bl	1594 <malloc>
    bffc:	e2506000 	subs	r6, r0, #0	; 0x0
    c000:	0a0000bc 	beq	c2f8 <udc_event_handle+0x700>
    c004:	e59f536c 	ldr	r5, [pc, #876]	; c378 <udc_event_handle+0x780>
    c008:	e1d521b6 	ldrh	r2, [r5, #22]
    c00c:	e1a03422 	lsr	r3, r2, #8
    c010:	e2433001 	sub	r3, r3, #1	; 0x1
    c014:	e3530005 	cmp	r3, #5	; 0x5
    c018:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    c01c:	ea000054 	b	c174 <udc_event_handle+0x57c>
    c020:	0000c038 	.word	0x0000c038
    c024:	0000c050 	.word	0x0000c050
    c028:	0000c0a8 	.word	0x0000c0a8
    c02c:	0000c174 	.word	0x0000c174
    c030:	0000c174 	.word	0x0000c174
    c034:	0000c07c 	.word	0x0000c07c
    c038:	e1a00006 	mov	r0, r6
    c03c:	e59f1348 	ldr	r1, [pc, #840]	; c38c <udc_event_handle+0x794>
    c040:	e3a02012 	mov	r2, #18	; 0x12
    c044:	ebffd4ed 	bl	1400 <memcpy>
    c048:	e3a03012 	mov	r3, #18	; 0x12
    c04c:	ea000028 	b	c0f4 <udc_event_handle+0x4fc>
    c050:	e59f3338 	ldr	r3, [pc, #824]	; c390 <udc_event_handle+0x798>
    c054:	e3a04020 	mov	r4, #32	; 0x20
    c058:	e3a0c000 	mov	ip, #0	; 0x0
    c05c:	e1a01003 	mov	r1, r3
    c060:	e1a00006 	mov	r0, r6
    c064:	e1a02004 	mov	r2, r4
    c068:	e5c3c003 	strb	ip, [r3, #3]
    c06c:	e5c34002 	strb	r4, [r3, #2]
    c070:	ebffd4e2 	bl	1400 <memcpy>
    c074:	e5854004 	str	r4, [r5, #4]
    c078:	ea000045 	b	c194 <udc_event_handle+0x59c>
    c07c:	e1d531ba 	ldrh	r3, [r5, #26]
    c080:	e59fc30c 	ldr	ip, [pc, #780]	; c394 <udc_event_handle+0x79c>
    c084:	e353000a 	cmp	r3, #10	; 0xa
    c088:	23a0300a 	movcs	r3, #10	; 0xa
    c08c:	e5cc3000 	strb	r3, [ip]
    c090:	e1a0100c 	mov	r1, ip
    c094:	e1a00006 	mov	r0, r6
    c098:	e3a0200a 	mov	r2, #10	; 0xa
    c09c:	ebffd4d7 	bl	1400 <memcpy>
    c0a0:	e3a0300a 	mov	r3, #10	; 0xa
    c0a4:	ea000012 	b	c0f4 <udc_event_handle+0x4fc>
    c0a8:	e21270ff 	ands	r7, r2, #255	; 0xff
    c0ac:	13a03000 	movne	r3, #0	; 0x0
    c0b0:	03a03001 	moveq	r3, #1	; 0x1
    c0b4:	e3570005 	cmp	r7, #5	; 0x5
    c0b8:	d1a08003 	movle	r8, r3
    c0bc:	c3838001 	orrgt	r8, r3, #1	; 0x1
    c0c0:	e3580000 	cmp	r8, #0	; 0x0
    c0c4:	e59f52ac 	ldr	r5, [pc, #684]	; c378 <udc_event_handle+0x780>
    c0c8:	0a00000b 	beq	c0fc <udc_event_handle+0x504>
    c0cc:	e1d531ba 	ldrh	r3, [r5, #26]
    c0d0:	e59fc2c0 	ldr	ip, [pc, #704]	; c398 <udc_event_handle+0x7a0>
    c0d4:	e3530004 	cmp	r3, #4	; 0x4
    c0d8:	23a03004 	movcs	r3, #4	; 0x4
    c0dc:	e5cc3000 	strb	r3, [ip]
    c0e0:	e1a0100c 	mov	r1, ip
    c0e4:	e1a00006 	mov	r0, r6
    c0e8:	e3a02004 	mov	r2, #4	; 0x4
    c0ec:	ebffd4c3 	bl	1400 <memcpy>
    c0f0:	e3a03004 	mov	r3, #4	; 0x4
    c0f4:	e5853004 	str	r3, [r5, #4]
    c0f8:	ea000025 	b	c194 <udc_event_handle+0x59c>
    c0fc:	e59f4298 	ldr	r4, [pc, #664]	; c39c <udc_event_handle+0x7a4>
    c100:	e7940107 	ldr	r0, [r4, r7, lsl #2]
    c104:	ebffd4a3 	bl	1398 <strlen>
    c108:	e6ef0070 	uxtb	r0, r0
    c10c:	e2803001 	add	r3, r0, #1	; 0x1
    c110:	e5d5201a 	ldrb	r2, [r5, #26]
    c114:	e1a03083 	lsl	r3, r3, #1
    c118:	e6ef3073 	uxtb	r3, r3
    c11c:	e1530002 	cmp	r3, r2
    c120:	31a0e003 	movcc	lr, r3
    c124:	21a0e002 	movcs	lr, r2
    c128:	e3a03003 	mov	r3, #3	; 0x3
    c12c:	e5c63001 	strb	r3, [r6, #1]
    c130:	e5c6e000 	strb	lr, [r6]
    c134:	e1a05008 	mov	r5, r8
    c138:	e3a02001 	mov	r2, #1	; 0x1
    c13c:	ea000005 	b	c158 <udc_event_handle+0x560>
    c140:	e7943107 	ldr	r3, [r4, r7, lsl #2]
    c144:	e0833002 	add	r3, r3, r2
    c148:	e5533001 	ldrb	r3, [r3, #-1]
    c14c:	e7c63082 	strb	r3, [r6, r2, lsl #1]
    c150:	e5c15001 	strb	r5, [r1, #1]
    c154:	e6ef207c 	uxtb	r2, ip
    c158:	e1500002 	cmp	r0, r2
    c15c:	e282c001 	add	ip, r2, #1	; 0x1
    c160:	e0861082 	add	r1, r6, r2, lsl #1
    c164:	aafffff5 	bge	c140 <udc_event_handle+0x548>
    c168:	e59f3208 	ldr	r3, [pc, #520]	; c378 <udc_event_handle+0x780>
    c16c:	e583e004 	str	lr, [r3, #4]
    c170:	ea000007 	b	c194 <udc_event_handle+0x59c>
    c174:	e59f21f0 	ldr	r2, [pc, #496]	; c36c <udc_event_handle+0x774>
    c178:	e3a03702 	mov	r3, #524288	; 0x80000
    c17c:	e5951024 	ldr	r1, [r5, #36]
    c180:	e5823910 	str	r3, [r2, #2320]
    c184:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    c188:	e5821914 	str	r1, [r2, #2324]
    c18c:	e5823900 	str	r3, [r2, #2304]
    c190:	ea00000e 	b	c1d0 <udc_event_handle+0x5d8>
    c194:	e59f11dc 	ldr	r1, [pc, #476]	; c378 <udc_event_handle+0x780>
    c198:	e59f01cc 	ldr	r0, [pc, #460]	; c36c <udc_event_handle+0x774>
    c19c:	e1d120b4 	ldrh	r2, [r1, #4]
    c1a0:	e1d131ba 	ldrh	r3, [r1, #26]
    c1a4:	e1520003 	cmp	r2, r3
    c1a8:	21a02003 	movcs	r2, r3
    c1ac:	e3823702 	orr	r3, r2, #524288	; 0x80000
    c1b0:	e5803910 	str	r3, [r0, #2320]
    c1b4:	e3a03000 	mov	r3, #0	; 0x0
    c1b8:	e5c13001 	strb	r3, [r1, #1]
    c1bc:	e2833321 	add	r3, r3, #-2080374784	; 0x84000000
    c1c0:	e5812004 	str	r2, [r1, #4]
    c1c4:	e5806914 	str	r6, [r0, #2324]
    c1c8:	e5803900 	str	r3, [r0, #2304]
    c1cc:	e581600c 	str	r6, [r1, #12]
    c1d0:	e1a00006 	mov	r0, r6
    c1d4:	ebffd556 	bl	1734 <free>
    c1d8:	ea000046 	b	c2f8 <udc_event_handle+0x700>
    c1dc:	e59fc1bc 	ldr	ip, [pc, #444]	; c3a0 <udc_event_handle+0x7a8>
    c1e0:	e5dc3000 	ldrb	r3, [ip]
    c1e4:	e35300ff 	cmp	r3, #255	; 0xff
    c1e8:	1a000042 	bne	c2f8 <udc_event_handle+0x700>
    c1ec:	e59f1178 	ldr	r1, [pc, #376]	; c36c <udc_event_handle+0x774>
    c1f0:	e59f0180 	ldr	r0, [pc, #384]	; c378 <udc_event_handle+0x780>
    c1f4:	e5913800 	ldr	r3, [r1, #2048]
    c1f8:	e1d021b6 	ldrh	r2, [r0, #22]
    c1fc:	e3c33e7f 	bic	r3, r3, #2032	; 0x7f0
    c200:	e202207f 	and	r2, r2, #127	; 0x7f
    c204:	e1833202 	orr	r3, r3, r2, lsl #4
    c208:	e5813800 	str	r3, [r1, #2048]
    c20c:	e5900024 	ldr	r0, [r0, #36]
    c210:	e3a03702 	mov	r3, #524288	; 0x80000
    c214:	e5cc2000 	strb	r2, [ip]
    c218:	e5813910 	str	r3, [r1, #2320]
    c21c:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    c220:	e5810914 	str	r0, [r1, #2324]
    c224:	e5813900 	str	r3, [r1, #2304]
    c228:	ea000032 	b	c2f8 <udc_event_handle+0x700>
    c22c:	e59f016c 	ldr	r0, [pc, #364]	; c3a0 <udc_event_handle+0x7a8>
    c230:	e5d03000 	ldrb	r3, [r0]
    c234:	e35300ff 	cmp	r3, #255	; 0xff
    c238:	0a00002e 	beq	c2f8 <udc_event_handle+0x700>
    c23c:	e59f1134 	ldr	r1, [pc, #308]	; c378 <udc_event_handle+0x780>
    c240:	e1d131b6 	ldrh	r3, [r1, #22]
    c244:	e3530000 	cmp	r3, #0	; 0x0
    c248:	03e03000 	mvneq	r3, #0	; 0x0
    c24c:	05911024 	ldreq	r1, [r1, #36]
    c250:	059f2114 	ldreq	r2, [pc, #276]	; c36c <udc_event_handle+0x774>
    c254:	05c03000 	strbeq	r3, [r0]
    c258:	0a000013 	beq	c2ac <udc_event_handle+0x6b4>
    c25c:	e3530001 	cmp	r3, #1	; 0x1
    c260:	1a000024 	bne	c2f8 <udc_event_handle+0x700>
    c264:	e59f2100 	ldr	r2, [pc, #256]	; c36c <udc_event_handle+0x774>
    c268:	e59f3134 	ldr	r3, [pc, #308]	; c3a4 <udc_event_handle+0x7ac>
    c26c:	e5911024 	ldr	r1, [r1, #36]
    c270:	e5823920 	str	r3, [r2, #2336]
    c274:	e2833d07 	add	r3, r3, #448	; 0x1c0
    c278:	e5823b40 	str	r3, [r2, #2880]
    c27c:	e592381c 	ldr	r3, [r2, #2076]
    c280:	e3833701 	orr	r3, r3, #262144	; 0x40000
    c284:	e3833002 	orr	r3, r3, #2	; 0x2
    c288:	e582381c 	str	r3, [r2, #2076]
    c28c:	ea000006 	b	c2ac <udc_event_handle+0x6b4>
    c290:	e210c01f 	ands	ip, r0, #31	; 0x1f
    c294:	1a000017 	bne	c2f8 <udc_event_handle+0x700>
    c298:	e3510000 	cmp	r1, #0	; 0x0
    c29c:	e59f50d4 	ldr	r5, [pc, #212]	; c378 <udc_event_handle+0x780>
    c2a0:	1a000004 	bne	c2b8 <udc_event_handle+0x6c0>
    c2a4:	e5951024 	ldr	r1, [r5, #36]
    c2a8:	e59f20bc 	ldr	r2, [pc, #188]	; c36c <udc_event_handle+0x774>
    c2ac:	e3a03702 	mov	r3, #524288	; 0x80000
    c2b0:	e5823910 	str	r3, [r2, #2320]
    c2b4:	ea00000c 	b	c2ec <udc_event_handle+0x6f4>
    c2b8:	e1d531b6 	ldrh	r3, [r5, #22]
    c2bc:	e59f20a8 	ldr	r2, [pc, #168]	; c36c <udc_event_handle+0x774>
    c2c0:	e3530002 	cmp	r3, #2	; 0x2
    c2c4:	23a03002 	movcs	r3, #2	; 0x2
    c2c8:	e6ef3073 	uxtb	r3, r3
    c2cc:	e1530001 	cmp	r3, r1
    c2d0:	21a03001 	movcs	r3, r1
    c2d4:	e3830702 	orr	r0, r3, #524288	; 0x80000
    c2d8:	e28d1006 	add	r1, sp, #6	; 0x6
    c2dc:	e5c5c001 	strb	ip, [r5, #1]
    c2e0:	e5853004 	str	r3, [r5, #4]
    c2e4:	e585100c 	str	r1, [r5, #12]
    c2e8:	e5820910 	str	r0, [r2, #2320]
    c2ec:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    c2f0:	e5821914 	str	r1, [r2, #2324]
    c2f4:	e5823900 	str	r3, [r2, #2304]
    c2f8:	e3003209 	movw	r3, #521	; 0x209
    c2fc:	e59f2068 	ldr	r2, [pc, #104]	; c36c <udc_event_handle+0x774>
    c300:	e00a3003 	and	r3, sl, r3
    c304:	e5823b08 	str	r3, [r2, #2824]
    c308:	e3190701 	tst	r9, #262144	; 0x40000
    c30c:	0a000013 	beq	c360 <udc_event_handle+0x768>
    c310:	e59f1054 	ldr	r1, [pc, #84]	; c36c <udc_event_handle+0x774>
    c314:	e5912b48 	ldr	r2, [r1, #2888]
    c318:	e3120001 	tst	r2, #1	; 0x1
    c31c:	0a00000c 	beq	c354 <udc_event_handle+0x75c>
    c320:	e5913b50 	ldr	r3, [r1, #2896]
    c324:	e59f504c 	ldr	r5, [pc, #76]	; c378 <udc_event_handle+0x780>
    c328:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
    c32c:	e3c3373e 	bic	r3, r3, #16252928	; 0xf80000
    c330:	e3530c02 	cmp	r3, #512	; 0x200
    c334:	05953028 	ldreq	r3, [r5, #40]
    c338:	13a03000 	movne	r3, #0	; 0x0
    c33c:	03833702 	orreq	r3, r3, #524288	; 0x80000
    c340:	05813b50 	streq	r3, [r1, #2896]
    c344:	05913b40 	ldreq	r3, [r1, #2880]
    c348:	15c53003 	strbne	r3, [r5, #3]
    c34c:	03833321 	orreq	r3, r3, #-2080374784	; 0x84000000
    c350:	05813b40 	streq	r3, [r1, #2880]
    c354:	e59f3010 	ldr	r3, [pc, #16]	; c36c <udc_event_handle+0x774>
    c358:	e2022005 	and	r2, r2, #5	; 0x5
    c35c:	e5832b48 	str	r2, [r3, #2888]
    c360:	e28dd008 	add	sp, sp, #8	; 0x8
    c364:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    c368:	0800d2b8 	.word	0x0800d2b8
    c36c:	27100000 	.word	0x27100000
    c370:	00010001 	.word	0x00010001
    c374:	0c001780 	.word	0x0c001780
    c378:	0800d2bc 	.word	0x0800d2bc
    c37c:	60080040 	.word	0x60080040
    c380:	0800d2b4 	.word	0x0800d2b4
    c384:	18088200 	.word	0x18088200
    c388:	0800d2d0 	.word	0x0800d2d0
    c38c:	000118e9 	.word	0x000118e9
    c390:	0800d15d 	.word	0x0800d15d
    c394:	0800d17d 	.word	0x0800d17d
    c398:	0800d134 	.word	0x0800d134
    c39c:	0800d7cc 	.word	0x0800d7cc
    c3a0:	0800d15c 	.word	0x0800d15c
    c3a4:	18088040 	.word	0x18088040

0000c3a8 <udc_write>:
    c3a8:	e59f30a4 	ldr	r3, [pc, #164]	; c454 <udc_write+0xac>
    c3ac:	e3a02001 	mov	r2, #1	; 0x1
    c3b0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    c3b4:	e583000c 	str	r0, [r3, #12]
    c3b8:	e1a05001 	mov	r5, r1
    c3bc:	e5c32001 	strb	r2, [r3, #1]
    c3c0:	e1a04003 	mov	r4, r3
    c3c4:	e1a08002 	mov	r8, r2
    c3c8:	e59f6088 	ldr	r6, [pc, #136]	; c458 <udc_write+0xb0>
    c3cc:	e59f7088 	ldr	r7, [pc, #136]	; c45c <udc_write+0xb4>
    c3d0:	ea00001b 	b	c444 <udc_write+0x9c>
    c3d4:	e3550701 	cmp	r5, #262144	; 0x40000
    c3d8:	b1a02005 	movlt	r2, r5
    c3dc:	a3a02701 	movge	r2, #262144	; 0x40000
    c3e0:	e292303f 	adds	r3, r2, #63	; 0x3f
    c3e4:	4282307e 	addmi	r3, r2, #126	; 0x7e
    c3e8:	e594100c 	ldr	r1, [r4, #12]
    c3ec:	e1a03343 	asr	r3, r3, #6
    c3f0:	e1823983 	orr	r3, r2, r3, lsl #19
    c3f4:	e5863930 	str	r3, [r6, #2352]
    c3f8:	e5861934 	str	r1, [r6, #2356]
    c3fc:	e5963920 	ldr	r3, [r6, #2336]
    c400:	e5842004 	str	r2, [r4, #4]
    c404:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    c408:	e5863920 	str	r3, [r6, #2336]
    c40c:	e5c48002 	strb	r8, [r4, #2]
    c410:	ea000000 	b	c418 <udc_write+0x70>
    c414:	ebfffdf7 	bl	bbf8 <udc_event_handle>
    c418:	e5d43002 	ldrb	r3, [r4, #2]
    c41c:	e3530000 	cmp	r3, #0	; 0x0
    c420:	0a000002 	beq	c430 <udc_write+0x88>
    c424:	e5973000 	ldr	r3, [r7]
    c428:	e3530000 	cmp	r3, #0	; 0x0
    c42c:	1afffff8 	bne	c414 <udc_write+0x6c>
    c430:	e5943004 	ldr	r3, [r4, #4]
    c434:	e594200c 	ldr	r2, [r4, #12]
    c438:	e0635005 	rsb	r5, r3, r5
    c43c:	e0833002 	add	r3, r3, r2
    c440:	e584300c 	str	r3, [r4, #12]
    c444:	e3550000 	cmp	r5, #0	; 0x0
    c448:	1affffe1 	bne	c3d4 <udc_write+0x2c>
    c44c:	e1a00005 	mov	r0, r5
    c450:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    c454:	0800d2bc 	.word	0x0800d2bc
    c458:	27100000 	.word	0x27100000
    c45c:	0800d158 	.word	0x0800d158

0000c460 <udc_read>:
    c460:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    c464:	e59f30b4 	ldr	r3, [pc, #180]	; c520 <udc_read+0xc0>
    c468:	e24dd004 	sub	sp, sp, #4	; 0x4
    c46c:	e1a04001 	mov	r4, r1
    c470:	e1a05003 	mov	r5, r3
    c474:	e5830010 	str	r0, [r3, #16]
    c478:	e3a0a001 	mov	sl, #1	; 0x1
    c47c:	e59f60a0 	ldr	r6, [pc, #160]	; c524 <udc_read+0xc4>
    c480:	e59f80a0 	ldr	r8, [pc, #160]	; c528 <udc_read+0xc8>
    c484:	e1a07003 	mov	r7, r3
    c488:	ea00001f 	b	c50c <udc_read+0xac>
    c48c:	e3540701 	cmp	r4, #262144	; 0x40000
    c490:	b1a02004 	movlt	r2, r4
    c494:	a3a02701 	movge	r2, #262144	; 0x40000
    c498:	e2823f7f 	add	r3, r2, #508	; 0x1fc
    c49c:	e2833003 	add	r3, r3, #3	; 0x3
    c4a0:	e5951010 	ldr	r1, [r5, #16]
    c4a4:	e1a034c3 	asr	r3, r3, #9
    c4a8:	e1823983 	orr	r3, r2, r3, lsl #19
    c4ac:	e5863b50 	str	r3, [r6, #2896]
    c4b0:	e5861b54 	str	r1, [r6, #2900]
    c4b4:	e5963b40 	ldr	r3, [r6, #2880]
    c4b8:	e5852008 	str	r2, [r5, #8]
    c4bc:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    c4c0:	e5863b40 	str	r3, [r6, #2880]
    c4c4:	e5c5a003 	strb	sl, [r5, #3]
    c4c8:	ea000002 	b	c4d8 <udc_read+0x78>
    c4cc:	ebfffdc9 	bl	bbf8 <udc_event_handle>
    c4d0:	e3a00001 	mov	r0, #1	; 0x1
    c4d4:	ebfff10c 	bl	890c <udelay>
    c4d8:	e5d53003 	ldrb	r3, [r5, #3]
    c4dc:	e3530000 	cmp	r3, #0	; 0x0
    c4e0:	0a000004 	beq	c4f8 <udc_read+0x98>
    c4e4:	e5983000 	ldr	r3, [r8]
    c4e8:	e3530000 	cmp	r3, #0	; 0x0
    c4ec:	1afffff6 	bne	c4cc <udc_read+0x6c>
    c4f0:	e3e04000 	mvn	r4, #0	; 0x0
    c4f4:	ea000006 	b	c514 <udc_read+0xb4>
    c4f8:	e5973008 	ldr	r3, [r7, #8]
    c4fc:	e5972010 	ldr	r2, [r7, #16]
    c500:	e0634004 	rsb	r4, r3, r4
    c504:	e0833002 	add	r3, r3, r2
    c508:	e5873010 	str	r3, [r7, #16]
    c50c:	e3540000 	cmp	r4, #0	; 0x0
    c510:	1affffdd 	bne	c48c <udc_read+0x2c>
    c514:	e1a00004 	mov	r0, r4
    c518:	e28dd004 	add	sp, sp, #4	; 0x4
    c51c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    c520:	0800d2bc 	.word	0x0800d2bc
    c524:	27100000 	.word	0x27100000
    c528:	0800d158 	.word	0x0800d158

0000c52c <udc_connect>:
    c52c:	e92d4070 	push	{r4, r5, r6, lr}
    c530:	e59f41e4 	ldr	r4, [pc, #484]	; c71c <udc_connect+0x1f0>
    c534:	e3a02000 	mov	r2, #0	; 0x0
    c538:	e5943024 	ldr	r3, [r4, #36]
    c53c:	e3e01000 	mvn	r1, #0	; 0x0
    c540:	e1530002 	cmp	r3, r2
    c544:	e59f31d4 	ldr	r3, [pc, #468]	; c720 <udc_connect+0x1f4>
    c548:	e5c42003 	strb	r2, [r4, #3]
    c54c:	e5c31000 	strb	r1, [r3]
    c550:	e5c42000 	strb	r2, [r4]
    c554:	e5c42002 	strb	r2, [r4, #2]
    c558:	1a000002 	bne	c568 <udc_connect+0x3c>
    c55c:	e3a00028 	mov	r0, #40	; 0x28
    c560:	ebffd40b 	bl	1594 <malloc>
    c564:	e5840024 	str	r0, [r4, #36]
    c568:	e59f31ac 	ldr	r3, [pc, #428]	; c71c <udc_connect+0x1f0>
    c56c:	e3a06001 	mov	r6, #1	; 0x1
    c570:	e59f51ac 	ldr	r5, [pc, #428]	; c724 <udc_connect+0x1f8>
    c574:	e59f01ac 	ldr	r0, [pc, #428]	; c728 <udc_connect+0x1fc>
    c578:	e5836020 	str	r6, [r3, #32]
    c57c:	ebffd068 	bl	724 <module_enable>
    c580:	e3a02004 	mov	r2, #4	; 0x4
    c584:	e3a03003 	mov	r3, #3	; 0x3
    c588:	e3a01000 	mov	r1, #0	; 0x0
    c58c:	e585243c 	str	r2, [r5, #1084]
    c590:	e5853440 	str	r3, [r5, #1088]
    c594:	e5852444 	str	r2, [r5, #1092]
    c598:	e5853448 	str	r3, [r5, #1096]
    c59c:	e585344c 	str	r3, [r5, #1100]
    c5a0:	e5853450 	str	r3, [r5, #1104]
    c5a4:	e5856454 	str	r6, [r5, #1108]
    c5a8:	e5856458 	str	r6, [r5, #1112]
    c5ac:	e585145c 	str	r1, [r5, #1116]
    c5b0:	e5851460 	str	r1, [r5, #1120]
    c5b4:	e5953420 	ldr	r3, [r5, #1056]
    c5b8:	e1a00006 	mov	r0, r6
    c5bc:	e3c33007 	bic	r3, r3, #7	; 0x7
    c5c0:	e3833005 	orr	r3, r3, #5	; 0x5
    c5c4:	e5853420 	str	r3, [r5, #1056]
    c5c8:	e5953424 	ldr	r3, [r5, #1060]
    c5cc:	e3833003 	orr	r3, r3, #3	; 0x3
    c5d0:	e5853424 	str	r3, [r5, #1060]
    c5d4:	e595342c 	ldr	r3, [r5, #1068]
    c5d8:	e1833006 	orr	r3, r3, r6
    c5dc:	e585342c 	str	r3, [r5, #1068]
    c5e0:	e5954400 	ldr	r4, [r5, #1024]
    c5e4:	e384400f 	orr	r4, r4, #15	; 0xf
    c5e8:	e5854400 	str	r4, [r5, #1024]
    c5ec:	ebfff0c6 	bl	890c <udelay>
    c5f0:	e3c4300e 	bic	r3, r4, #14	; 0xe
    c5f4:	e5853400 	str	r3, [r5, #1024]
    c5f8:	e1a00006 	mov	r0, r6
    c5fc:	e3c4400f 	bic	r4, r4, #15	; 0xf
    c600:	ebfff0c1 	bl	890c <udelay>
    c604:	e5854400 	str	r4, [r5, #1024]
    c608:	e1a00006 	mov	r0, r6
    c60c:	ebfff0be 	bl	890c <udelay>
    c610:	e59f2114 	ldr	r2, [pc, #276]	; c72c <udc_connect+0x200>
    c614:	e59f3114 	ldr	r3, [pc, #276]	; c730 <udc_connect+0x204>
    c618:	e1a05002 	mov	r5, r2
    c61c:	e582300c 	str	r3, [r2, #12]
    c620:	e5954010 	ldr	r4, [r5, #16]
    c624:	e3a00028 	mov	r0, #40	; 0x28
    c628:	ebfff0b7 	bl	890c <udelay>
    c62c:	e3540000 	cmp	r4, #0	; 0x0
    c630:	e59f40f4 	ldr	r4, [pc, #244]	; c72c <udc_connect+0x200>
    c634:	aafffff9 	bge	c620 <udc_connect+0xf4>
    c638:	e5943010 	ldr	r3, [r4, #16]
    c63c:	e3a0000a 	mov	r0, #10	; 0xa
    c640:	e3833001 	orr	r3, r3, #1	; 0x1
    c644:	e5843010 	str	r3, [r4, #16]
    c648:	ebfff0af 	bl	890c <udelay>
    c64c:	e1a05004 	mov	r5, r4
    c650:	e5954010 	ldr	r4, [r5, #16]
    c654:	e3a0000a 	mov	r0, #10	; 0xa
    c658:	ebfff0ab 	bl	890c <udelay>
    c65c:	e3140001 	tst	r4, #1	; 0x1
    c660:	e59fc0c4 	ldr	ip, [pc, #196]	; c72c <udc_connect+0x200>
    c664:	1afffff9 	bne	c650 <udc_connect+0x124>
    c668:	e3a03027 	mov	r3, #39	; 0x27
    c66c:	e58c3008 	str	r3, [ip, #8]
    c670:	e59c303c 	ldr	r3, [ip, #60]
    c674:	e59c3014 	ldr	r3, [ip, #20]
    c678:	e2130001 	ands	r0, r3, #1	; 0x1
    c67c:	1a000024 	bne	c714 <udc_connect+0x1e8>
    c680:	e3a03301 	mov	r3, #67108864	; 0x4000000
    c684:	e58c3800 	str	r3, [ip, #2048]
    c688:	e59f30a4 	ldr	r3, [pc, #164]	; c734 <udc_connect+0x208>
    c68c:	e59f20a4 	ldr	r2, [pc, #164]	; c738 <udc_connect+0x20c>
    c690:	e58c3018 	str	r3, [ip, #24]
    c694:	e59f30a0 	ldr	r3, [pc, #160]	; c73c <udc_connect+0x210>
    c698:	e59f10a0 	ldr	r1, [pc, #160]	; c740 <udc_connect+0x214>
    c69c:	e5823014 	str	r3, [r2, #20]
    c6a0:	e59f309c 	ldr	r3, [pc, #156]	; c744 <udc_connect+0x218>
    c6a4:	e5821010 	str	r1, [r2, #16]
    c6a8:	e5823004 	str	r3, [r2, #4]
    c6ac:	e59f3094 	ldr	r3, [pc, #148]	; c748 <udc_connect+0x21c>
    c6b0:	e582100c 	str	r1, [r2, #12]
    c6b4:	e5823008 	str	r3, [r2, #8]
    c6b8:	ebfff0b7 	bl	899c <get_timer>
    c6bc:	e59f5058 	ldr	r5, [pc, #88]	; c71c <udc_connect+0x1f0>
    c6c0:	e1a04000 	mov	r4, r0
    c6c4:	ebfffd4b 	bl	bbf8 <udc_event_handle>
    c6c8:	e5d53000 	ldrb	r3, [r5]
    c6cc:	e1a00004 	mov	r0, r4
    c6d0:	e3530000 	cmp	r3, #0	; 0x0
    c6d4:	1a000003 	bne	c6e8 <udc_connect+0x1bc>
    c6d8:	ebfff0af 	bl	899c <get_timer>
    c6dc:	e3500ffa 	cmp	r0, #1000	; 0x3e8
    c6e0:	9afffff7 	bls	c6c4 <udc_connect+0x198>
    c6e4:	ea00000a 	b	c714 <udc_connect+0x1e8>
    c6e8:	e59f203c 	ldr	r2, [pc, #60]	; c72c <udc_connect+0x200>
    c6ec:	e59f3058 	ldr	r3, [pc, #88]	; c74c <udc_connect+0x220>
    c6f0:	e3a00000 	mov	r0, #0	; 0x0
    c6f4:	e5823920 	str	r3, [r2, #2336]
    c6f8:	e2833d07 	add	r3, r3, #448	; 0x1c0
    c6fc:	e5823b40 	str	r3, [r2, #2880]
    c700:	e592381c 	ldr	r3, [r2, #2076]
    c704:	e3833701 	orr	r3, r3, #262144	; 0x40000
    c708:	e3833002 	orr	r3, r3, #2	; 0x2
    c70c:	e582381c 	str	r3, [r2, #2076]
    c710:	e8bd8070 	pop	{r4, r5, r6, pc}
    c714:	e3e00000 	mvn	r0, #0	; 0x0
    c718:	e8bd8070 	pop	{r4, r5, r6, pc}
    c71c:	0800d2bc 	.word	0x0800d2bc
    c720:	0800d15c 	.word	0x0800d15c
    c724:	2d038000 	.word	0x2d038000
    c728:	2d038400 	.word	0x2d038400
    c72c:	27100000 	.word	0x27100000
    c730:	4c001408 	.word	0x4c001408
    c734:	000c3c00 	.word	0x000c3c00
    c738:	0800d7cc 	.word	0x0800d7cc
    c73c:	00013ffc 	.word	0x00013ffc
    c740:	00014011 	.word	0x00014011
    c744:	00014005 	.word	0x00014005
    c748:	0800d138 	.word	0x0800d138
    c74c:	18088040 	.word	0x18088040

0000c750 <linkpc_set_stage>:
    c750:	e1a01000 	mov	r1, r0
    c754:	e59f0004 	ldr	r0, [pc, #4]	; c760 <linkpc_set_stage+0x10>
    c758:	e3a02010 	mov	r2, #16	; 0x10
    c75c:	eaffd2bd 	b	1258 <strncpy>
    c760:	0800d188 	.word	0x0800d188

0000c764 <__response_ok>:
    c764:	e92d4010 	push	{r4, lr}
    c768:	e24dd020 	sub	sp, sp, #32	; 0x20
    c76c:	e59f1038 	ldr	r1, [pc, #56]	; c7ac <__response_ok+0x48>
    c770:	e3a02005 	mov	r2, #5	; 0x5
    c774:	e1a0000d 	mov	r0, sp
    c778:	ebffd320 	bl	1400 <memcpy>
    c77c:	e3a0201b 	mov	r2, #27	; 0x1b
    c780:	e3a01000 	mov	r1, #0	; 0x0
    c784:	e28d0005 	add	r0, sp, #5	; 0x5
    c788:	ebffd314 	bl	13e0 <memset>
    c78c:	e1a0000d 	mov	r0, sp
    c790:	e3a01004 	mov	r1, #4	; 0x4
    c794:	ebffff03 	bl	c3a8 <udc_write>
    c798:	e59f0010 	ldr	r0, [pc, #16]	; c7b0 <__response_ok+0x4c>
    c79c:	e1a0400d 	mov	r4, sp
    c7a0:	ebfff059 	bl	890c <udelay>
    c7a4:	e28dd020 	add	sp, sp, #32	; 0x20
    c7a8:	e8bd8010 	pop	{r4, pc}
    c7ac:	000118fb 	.word	0x000118fb
    c7b0:	0007a120 	.word	0x0007a120

0000c7b4 <linkpc_loop>:
    c7b4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    c7b8:	e59f05e0 	ldr	r0, [pc, #1504]	; cda0 <linkpc_loop+0x5ec>
    c7bc:	e24ddf59 	sub	sp, sp, #356	; 0x164
    c7c0:	e59f45dc 	ldr	r4, [pc, #1500]	; cda4 <linkpc_loop+0x5f0>
    c7c4:	e1a0e00f 	mov	lr, pc
    c7c8:	e594f000 	ldr	pc, [r4]
    c7cc:	ebffff56 	bl	c52c <udc_connect>
    c7d0:	e3500000 	cmp	r0, #0	; 0x0
    c7d4:	13a04001 	movne	r4, #1	; 0x1
    c7d8:	1a000169 	bne	cd84 <linkpc_loop+0x5d0>
    c7dc:	e59f05c4 	ldr	r0, [pc, #1476]	; cda8 <linkpc_loop+0x5f4>
    c7e0:	e1a0e00f 	mov	lr, pc
    c7e4:	e594f000 	ldr	pc, [r4]
    c7e8:	e59f05bc 	ldr	r0, [pc, #1468]	; cdac <linkpc_loop+0x5f8>
    c7ec:	e1a0e00f 	mov	lr, pc
    c7f0:	e594f000 	ldr	pc, [r4]
    c7f4:	e28d708c 	add	r7, sp, #140	; 0x8c
    c7f8:	e59f95b0 	ldr	r9, [pc, #1456]	; cdb0 <linkpc_loop+0x5fc>
    c7fc:	e1a0a004 	mov	sl, r4
    c800:	e287b004 	add	fp, r7, #4	; 0x4
    c804:	e5991000 	ldr	r1, [r9]
    c808:	e3510000 	cmp	r1, #0	; 0x0
    c80c:	159f25a0 	ldrne	r2, [pc, #1440]	; cdb4 <linkpc_loop+0x600>
    c810:	028d4f43 	addeq	r4, sp, #268	; 0x10c
    c814:	15923000 	ldrne	r3, [r2]
    c818:	03a05040 	moveq	r5, #64	; 0x40
    c81c:	12834302 	addne	r4, r3, #134217728	; 0x8000000
    c820:	11a05001 	movne	r5, r1
    c824:	e1a01005 	mov	r1, r5
    c828:	e1a00004 	mov	r0, r4
    c82c:	ebffff0b 	bl	c460 <udc_read>
    c830:	e1a00007 	mov	r0, r7
    c834:	e3a01000 	mov	r1, #0	; 0x0
    c838:	e3a02080 	mov	r2, #128	; 0x80
    c83c:	ebffd2e7 	bl	13e0 <memset>
    c840:	e5993000 	ldr	r3, [r9]
    c844:	e3530000 	cmp	r3, #0	; 0x0
    c848:	0a00002a 	beq	c8f8 <linkpc_loop+0x144>
    c84c:	e28d400c 	add	r4, sp, #12	; 0xc
    c850:	e3a02080 	mov	r2, #128	; 0x80
    c854:	e3a01000 	mov	r1, #0	; 0x0
    c858:	e1a00004 	mov	r0, r4
    c85c:	ebffd2df 	bl	13e0 <memset>
    c860:	e59f0550 	ldr	r0, [pc, #1360]	; cdb8 <linkpc_loop+0x604>
    c864:	e59f1550 	ldr	r1, [pc, #1360]	; cdbc <linkpc_loop+0x608>
    c868:	e1a0e00f 	mov	lr, pc
    c86c:	e59af000 	ldr	pc, [sl]
    c870:	e59fc53c 	ldr	ip, [pc, #1340]	; cdb4 <linkpc_loop+0x600>
    c874:	e5990000 	ldr	r0, [r9]
    c878:	e59c3000 	ldr	r3, [ip]
    c87c:	e0632000 	rsb	r2, r3, r0
    c880:	e1550002 	cmp	r5, r2
    c884:	90833005 	addls	r3, r3, r5
    c888:	80833002 	addhi	r3, r3, r2
    c88c:	e1530000 	cmp	r3, r0
    c890:	e58c3000 	str	r3, [ip]
    c894:	3affffda 	bcc	c804 <linkpc_loop+0x50>
    c898:	e59f3520 	ldr	r3, [pc, #1312]	; cdc0 <linkpc_loop+0x60c>
    c89c:	e5933000 	ldr	r3, [r3]
    c8a0:	e3530000 	cmp	r3, #0	; 0x0
    c8a4:	e59f3518 	ldr	r3, [pc, #1304]	; cdc4 <linkpc_loop+0x610>
    c8a8:	159f1518 	ldrne	r1, [pc, #1304]	; cdc8 <linkpc_loop+0x614>
    c8ac:	059f1518 	ldreq	r1, [pc, #1304]	; cdcc <linkpc_loop+0x618>
    c8b0:	e5830000 	str	r0, [r3]
    c8b4:	e3a03000 	mov	r3, #0	; 0x0
    c8b8:	11a00004 	movne	r0, r4
    c8bc:	01a00004 	moveq	r0, r4
    c8c0:	e28d400c 	add	r4, sp, #12	; 0xc
    c8c4:	e5893000 	str	r3, [r9]
    c8c8:	eb0005c3 	bl	dfdc <sprintf>
    c8cc:	e1a00004 	mov	r0, r4
    c8d0:	ebffd2b0 	bl	1398 <strlen>
    c8d4:	e1a01000 	mov	r1, r0
    c8d8:	e1a00004 	mov	r0, r4
    c8dc:	ebfffeb1 	bl	c3a8 <udc_write>
    c8e0:	e59f24cc 	ldr	r2, [pc, #1228]	; cdb4 <linkpc_loop+0x600>
    c8e4:	e59f04e4 	ldr	r0, [pc, #1252]	; cdd0 <linkpc_loop+0x61c>
    c8e8:	e5921000 	ldr	r1, [r2]
    c8ec:	e1a0e00f 	mov	lr, pc
    c8f0:	e59af000 	ldr	pc, [sl]
    c8f4:	eaffffc2 	b	c804 <linkpc_loop+0x50>
    c8f8:	e1a01004 	mov	r1, r4
    c8fc:	e59f04b4 	ldr	r0, [pc, #1204]	; cdb8 <linkpc_loop+0x604>
    c900:	e1a0e00f 	mov	lr, pc
    c904:	e59af000 	ldr	pc, [sl]
    c908:	e59f14c4 	ldr	r1, [pc, #1220]	; cdd4 <linkpc_loop+0x620>
    c90c:	e1a00007 	mov	r0, r7
    c910:	eb0005b1 	bl	dfdc <sprintf>
    c914:	e1a00004 	mov	r0, r4
    c918:	e59f14b8 	ldr	r1, [pc, #1208]	; cdd8 <linkpc_loop+0x624>
    c91c:	e3a02006 	mov	r2, #6	; 0x6
    c920:	ebffd27d 	bl	131c <strncmp>
    c924:	e3500000 	cmp	r0, #0	; 0x0
    c928:	1a000002 	bne	c938 <linkpc_loop+0x184>
    c92c:	ebffff8c 	bl	c764 <__response_ok>
    c930:	ebffcfb0 	bl	7f8 <system_reset>
    c934:	ea000105 	b	cd50 <linkpc_loop+0x59c>
    c938:	e1a00004 	mov	r0, r4
    c93c:	e59f1498 	ldr	r1, [pc, #1176]	; cddc <linkpc_loop+0x628>
    c940:	e3a02007 	mov	r2, #7	; 0x7
    c944:	ebffd274 	bl	131c <strncmp>
    c948:	e3500000 	cmp	r0, #0	; 0x0
    c94c:	1a00002d 	bne	ca08 <linkpc_loop+0x254>
    c950:	e2844007 	add	r4, r4, #7	; 0x7
    c954:	e1a02004 	mov	r2, r4
    c958:	e59f1480 	ldr	r1, [pc, #1152]	; cde0 <linkpc_loop+0x62c>
    c95c:	e59f0480 	ldr	r0, [pc, #1152]	; cde4 <linkpc_loop+0x630>
    c960:	e1a0e00f 	mov	lr, pc
    c964:	e59af000 	ldr	pc, [sl]
    c968:	e59f145c 	ldr	r1, [pc, #1116]	; cdcc <linkpc_loop+0x618>
    c96c:	e1a00007 	mov	r0, r7
    c970:	ebffd231 	bl	123c <strcpy>
    c974:	e59f146c 	ldr	r1, [pc, #1132]	; cde8 <linkpc_loop+0x634>
    c978:	e1a00004 	mov	r0, r4
    c97c:	e3a02007 	mov	r2, #7	; 0x7
    c980:	ebffd265 	bl	131c <strncmp>
    c984:	e3500000 	cmp	r0, #0	; 0x0
    c988:	01a0000b 	moveq	r0, fp
    c98c:	059f1458 	ldreq	r1, [pc, #1112]	; cdec <linkpc_loop+0x638>
    c990:	0a000010 	beq	c9d8 <linkpc_loop+0x224>
    c994:	e59f1454 	ldr	r1, [pc, #1108]	; cdf0 <linkpc_loop+0x63c>
    c998:	e1a00004 	mov	r0, r4
    c99c:	e3a02005 	mov	r2, #5	; 0x5
    c9a0:	ebffd25d 	bl	131c <strncmp>
    c9a4:	e3500000 	cmp	r0, #0	; 0x0
    c9a8:	01a0000b 	moveq	r0, fp
    c9ac:	059f1440 	ldreq	r1, [pc, #1088]	; cdf4 <linkpc_loop+0x640>
    c9b0:	0a000008 	beq	c9d8 <linkpc_loop+0x224>
    c9b4:	e1a00004 	mov	r0, r4
    c9b8:	e59f1438 	ldr	r1, [pc, #1080]	; cdf8 <linkpc_loop+0x644>
    c9bc:	e3a02008 	mov	r2, #8	; 0x8
    c9c0:	ebffd255 	bl	131c <strncmp>
    c9c4:	e3500000 	cmp	r0, #0	; 0x0
    c9c8:	1a000004 	bne	c9e0 <linkpc_loop+0x22c>
    c9cc:	e59f3428 	ldr	r3, [pc, #1064]	; cdfc <linkpc_loop+0x648>
    c9d0:	e1a0000b 	mov	r0, fp
    c9d4:	e5931008 	ldr	r1, [r3, #8]
    c9d8:	ebffd217 	bl	123c <strcpy>
    c9dc:	ea0000db 	b	cd50 <linkpc_loop+0x59c>
    c9e0:	e1a00004 	mov	r0, r4
    c9e4:	e59f1414 	ldr	r1, [pc, #1044]	; ce00 <linkpc_loop+0x64c>
    c9e8:	e3a0200c 	mov	r2, #12	; 0xc
    c9ec:	ebffd24a 	bl	131c <strncmp>
    c9f0:	e3500000 	cmp	r0, #0	; 0x0
    c9f4:	1a0000d5 	bne	cd50 <linkpc_loop+0x59c>
    c9f8:	e1a0000b 	mov	r0, fp
    c9fc:	e59f1400 	ldr	r1, [pc, #1024]	; ce04 <linkpc_loop+0x650>
    ca00:	e3a02903 	mov	r2, #49152	; 0xc000
    ca04:	ea000020 	b	ca8c <linkpc_loop+0x2d8>
    ca08:	e1a00004 	mov	r0, r4
    ca0c:	e59f13f4 	ldr	r1, [pc, #1012]	; ce08 <linkpc_loop+0x654>
    ca10:	e3a02009 	mov	r2, #9	; 0x9
    ca14:	ebffd240 	bl	131c <strncmp>
    ca18:	e2505000 	subs	r5, r0, #0	; 0x0
    ca1c:	1a00001c 	bne	ca94 <linkpc_loop+0x2e0>
    ca20:	e59f3398 	ldr	r3, [pc, #920]	; cdc0 <linkpc_loop+0x60c>
    ca24:	e59fc388 	ldr	ip, [pc, #904]	; cdb4 <linkpc_loop+0x600>
    ca28:	e3a02010 	mov	r2, #16	; 0x10
    ca2c:	e1a01005 	mov	r1, r5
    ca30:	e5835000 	str	r5, [r3]
    ca34:	e58c5000 	str	r5, [ip]
    ca38:	e2840009 	add	r0, r4, #9	; 0x9
    ca3c:	eb0001da 	bl	d1ac <simple_strtoul>
    ca40:	e1a01000 	mov	r1, r0
    ca44:	e5890000 	str	r0, [r9]
    ca48:	e59f03bc 	ldr	r0, [pc, #956]	; ce0c <linkpc_loop+0x658>
    ca4c:	e1a0e00f 	mov	lr, pc
    ca50:	e59af000 	ldr	pc, [sl]
    ca54:	e5992000 	ldr	r2, [r9]
    ca58:	e3520000 	cmp	r2, #0	; 0x0
    ca5c:	01a00007 	moveq	r0, r7
    ca60:	059f13a8 	ldreq	r1, [pc, #936]	; ce10 <linkpc_loop+0x65c>
    ca64:	0a0000b8 	beq	cd4c <linkpc_loop+0x598>
    ca68:	e3520903 	cmp	r2, #49152	; 0xc000
    ca6c:	91a00007 	movls	r0, r7
    ca70:	959f139c 	ldrls	r1, [pc, #924]	; ce14 <linkpc_loop+0x660>
    ca74:	9a000004 	bls	ca8c <linkpc_loop+0x2d8>
    ca78:	e1a00007 	mov	r0, r7
    ca7c:	e59f1394 	ldr	r1, [pc, #916]	; ce18 <linkpc_loop+0x664>
    ca80:	eb000555 	bl	dfdc <sprintf>
    ca84:	e5895000 	str	r5, [r9]
    ca88:	ea0000b0 	b	cd50 <linkpc_loop+0x59c>
    ca8c:	eb000552 	bl	dfdc <sprintf>
    ca90:	ea0000ae 	b	cd50 <linkpc_loop+0x59c>
    ca94:	e1a00004 	mov	r0, r4
    ca98:	e59f137c 	ldr	r1, [pc, #892]	; ce1c <linkpc_loop+0x668>
    ca9c:	e3a02006 	mov	r2, #6	; 0x6
    caa0:	ebffd21d 	bl	131c <strncmp>
    caa4:	e3500000 	cmp	r0, #0	; 0x0
    caa8:	1a000065 	bne	cc44 <linkpc_loop+0x490>
    caac:	e2844006 	add	r4, r4, #6	; 0x6
    cab0:	e1a00004 	mov	r0, r4
    cab4:	e59f1364 	ldr	r1, [pc, #868]	; ce20 <linkpc_loop+0x66c>
    cab8:	e3a02003 	mov	r2, #3	; 0x3
    cabc:	ebffd216 	bl	131c <strncmp>
    cac0:	e3500000 	cmp	r0, #0	; 0x0
    cac4:	1a000007 	bne	cae8 <linkpc_loop+0x334>
    cac8:	e2800302 	add	r0, r0, #134217728	; 0x8000000
    cacc:	ebffe5fa 	bl	62bc <cdbg_config_isi>
    cad0:	e3500000 	cmp	r0, #0	; 0x0
    cad4:	11a00007 	movne	r0, r7
    cad8:	159f1344 	ldrne	r1, [pc, #836]	; ce24 <linkpc_loop+0x670>
    cadc:	01a00007 	moveq	r0, r7
    cae0:	059f12e4 	ldreq	r1, [pc, #740]	; cdcc <linkpc_loop+0x618>
    cae4:	ea000098 	b	cd4c <linkpc_loop+0x598>
    cae8:	e1a00004 	mov	r0, r4
    caec:	ebffe0ca 	bl	4e1c <vs_device_id>
    caf0:	e59f6330 	ldr	r6, [pc, #816]	; ce28 <linkpc_loop+0x674>
    caf4:	e59fc330 	ldr	ip, [pc, #816]	; ce2c <linkpc_loop+0x678>
    caf8:	e5963000 	ldr	r3, [r6]
    cafc:	e1a05000 	mov	r5, r0
    cb00:	e58d3000 	str	r3, [sp]
    cb04:	e59f0324 	ldr	r0, [pc, #804]	; ce30 <linkpc_loop+0x67c>
    cb08:	e59f1324 	ldr	r1, [pc, #804]	; ce34 <linkpc_loop+0x680>
    cb0c:	e1a02005 	mov	r2, r5
    cb10:	e59c3000 	ldr	r3, [ip]
    cb14:	e1a0e00f 	mov	lr, pc
    cb18:	e59af000 	ldr	pc, [sl]
    cb1c:	e355000e 	cmp	r5, #14	; 0xe
    cb20:	ca000006 	bgt	cb40 <linkpc_loop+0x38c>
    cb24:	e1a00005 	mov	r0, r5
    cb28:	ebffe0a7 	bl	4dcc <vs_device_erasable>
    cb2c:	e3500000 	cmp	r0, #0	; 0x0
    cb30:	0a000005 	beq	cb4c <linkpc_loop+0x398>
    cb34:	e5963000 	ldr	r3, [r6]
    cb38:	e3530000 	cmp	r3, #0	; 0x0
    cb3c:	1a000002 	bne	cb4c <linkpc_loop+0x398>
    cb40:	e1a00007 	mov	r0, r7
    cb44:	e59f12ec 	ldr	r1, [pc, #748]	; ce38 <linkpc_loop+0x684>
    cb48:	ea00007f 	b	cd4c <linkpc_loop+0x598>
    cb4c:	e3a00901 	mov	r0, #16384	; 0x4000
    cb50:	ebffd28f 	bl	1594 <malloc>
    cb54:	e2508000 	subs	r8, r0, #0	; 0x0
    cb58:	01a00007 	moveq	r0, r7
    cb5c:	059f12d8 	ldreq	r1, [pc, #728]	; ce3c <linkpc_loop+0x688>
    cb60:	0a000079 	beq	cd4c <linkpc_loop+0x598>
    cb64:	e3a01001 	mov	r1, #1	; 0x1
    cb68:	e1a00005 	mov	r0, r5
    cb6c:	ebffe17e 	bl	516c <vs_assign_by_id>
    cb70:	e3500000 	cmp	r0, #0	; 0x0
    cb74:	b1a00007 	movlt	r0, r7
    cb78:	b59f12c0 	ldrlt	r1, [pc, #704]	; ce40 <linkpc_loop+0x68c>
    cb7c:	ba00002c 	blt	cc34 <linkpc_loop+0x480>
    cb80:	e355000d 	cmp	r5, #13	; 0xd
    cb84:	13a06002 	movne	r6, #2	; 0x2
    cb88:	03a06001 	moveq	r6, #1	; 0x1
    cb8c:	e1a04c06 	lsl	r4, r6, #24
    cb90:	e3a00302 	mov	r0, #134217728	; 0x8000000
    cb94:	03a0500f 	moveq	r5, #15	; 0xf
    cb98:	e58d414c 	str	r4, [sp, #332]
    cb9c:	ebfffbcb 	bl	bad0 <ram_addr2offs>
    cba0:	e3a024ff 	mov	r2, #-16777216	; 0xff000000
    cba4:	e0922000 	adds	r2, r2, r0
    cba8:	e3e03000 	mvn	r3, #0	; 0x0
    cbac:	e0a33001 	adc	r3, r3, r1
    cbb0:	e59f1270 	ldr	r1, [pc, #624]	; ce28 <linkpc_loop+0x674>
    cbb4:	e59fc270 	ldr	ip, [pc, #624]	; ce2c <linkpc_loop+0x678>
    cbb8:	e1d100b2 	ldrh	r0, [r1, #2]
    cbbc:	e1a014a2 	lsr	r1, r2, #9
    cbc0:	e1811b83 	orr	r1, r1, r3, lsl #23
    cbc4:	e59f31f8 	ldr	r3, [pc, #504]	; cdc4 <linkpc_loop+0x610>
    cbc8:	e1844001 	orr	r4, r4, r1
    cbcc:	e5932000 	ldr	r2, [r3]
    cbd0:	e59c3000 	ldr	r3, [ip]
    cbd4:	e3560001 	cmp	r6, #1	; 0x1
    cbd8:	e1a0c800 	lsl	ip, r0, #16
    cbdc:	e58d414c 	str	r4, [sp, #332]
    cbe0:	e58d3150 	str	r3, [sp, #336]
    cbe4:	e58d2158 	str	r2, [sp, #344]
    cbe8:	e58dc154 	str	ip, [sp, #340]
    cbec:	1a000004 	bne	cc04 <linkpc_loop+0x450>
    cbf0:	ebfffedb 	bl	c764 <__response_ok>
    cbf4:	e59d0150 	ldr	r0, [sp, #336]
    cbf8:	e3a01000 	mov	r1, #0	; 0x0
    cbfc:	ebffe539 	bl	60e8 <cdbg_jump>
    cc00:	eafffffe 	b	cc00 <linkpc_loop+0x44c>
    cc04:	e3a0100d 	mov	r1, #13	; 0xd
    cc08:	e1a03005 	mov	r3, r5
    cc0c:	e28d0f53 	add	r0, sp, #332	; 0x14c
    cc10:	e3a02000 	mov	r2, #0	; 0x0
    cc14:	e58dc000 	str	ip, [sp]
    cc18:	e58d8004 	str	r8, [sp, #4]
    cc1c:	ebffe5bb 	bl	6310 <burn_image_loop>
    cc20:	e3500000 	cmp	r0, #0	; 0x0
    cc24:	159f1218 	ldrne	r1, [pc, #536]	; ce44 <linkpc_loop+0x690>
    cc28:	059f119c 	ldreq	r1, [pc, #412]	; cdcc <linkpc_loop+0x618>
    cc2c:	11a00007 	movne	r0, r7
    cc30:	01a00007 	moveq	r0, r7
    cc34:	eb0004e8 	bl	dfdc <sprintf>
    cc38:	e1a00008 	mov	r0, r8
    cc3c:	ebffd2bc 	bl	1734 <free>
    cc40:	ea000042 	b	cd50 <linkpc_loop+0x59c>
    cc44:	e1a00004 	mov	r0, r4
    cc48:	e59f11f8 	ldr	r1, [pc, #504]	; ce48 <linkpc_loop+0x694>
    cc4c:	e3a02008 	mov	r2, #8	; 0x8
    cc50:	ebffd1b1 	bl	131c <strncmp>
    cc54:	e3500000 	cmp	r0, #0	; 0x0
    cc58:	1a000003 	bne	cc6c <linkpc_loop+0x4b8>
    cc5c:	ebfffec0 	bl	c764 <__response_ok>
    cc60:	e3a00001 	mov	r0, #1	; 0x1
    cc64:	ebffcddf 	bl	3e8 <try_boot>
    cc68:	ea000038 	b	cd50 <linkpc_loop+0x59c>
    cc6c:	e1a00004 	mov	r0, r4
    cc70:	e59f11d4 	ldr	r1, [pc, #468]	; ce4c <linkpc_loop+0x698>
    cc74:	e3a0200a 	mov	r2, #10	; 0xa
    cc78:	ebffd1a7 	bl	131c <strncmp>
    cc7c:	e2505000 	subs	r5, r0, #0	; 0x0
    cc80:	1a000019 	bne	ccec <linkpc_loop+0x538>
    cc84:	e28d1f57 	add	r1, sp, #348	; 0x15c
    cc88:	e284000b 	add	r0, r4, #11	; 0xb
    cc8c:	e3a02010 	mov	r2, #16	; 0x10
    cc90:	eb000145 	bl	d1ac <simple_strtoul>
    cc94:	e59f2190 	ldr	r2, [pc, #400]	; ce2c <linkpc_loop+0x678>
    cc98:	e1a01005 	mov	r1, r5
    cc9c:	e5820000 	str	r0, [r2]
    cca0:	e59d015c 	ldr	r0, [sp, #348]
    cca4:	e3a02010 	mov	r2, #16	; 0x10
    cca8:	e2800001 	add	r0, r0, #1	; 0x1
    ccac:	eb00013e 	bl	d1ac <simple_strtoul>
    ccb0:	e59f4170 	ldr	r4, [pc, #368]	; ce28 <linkpc_loop+0x674>
    ccb4:	e59f3170 	ldr	r3, [pc, #368]	; ce2c <linkpc_loop+0x678>
    ccb8:	e1a02000 	mov	r2, r0
    ccbc:	e5840000 	str	r0, [r4]
    ccc0:	e5931000 	ldr	r1, [r3]
    ccc4:	e59f0184 	ldr	r0, [pc, #388]	; ce50 <linkpc_loop+0x69c>
    ccc8:	e1a0e00f 	mov	lr, pc
    cccc:	e59af000 	ldr	pc, [sl]
    ccd0:	e59fc154 	ldr	ip, [pc, #340]	; ce2c <linkpc_loop+0x678>
    ccd4:	e1a00007 	mov	r0, r7
    ccd8:	e5943000 	ldr	r3, [r4]
    ccdc:	e59f1170 	ldr	r1, [pc, #368]	; ce54 <linkpc_loop+0x6a0>
    cce0:	e59c2000 	ldr	r2, [ip]
    cce4:	eb0004bc 	bl	dfdc <sprintf>
    cce8:	ea000018 	b	cd50 <linkpc_loop+0x59c>
    ccec:	e1a00004 	mov	r0, r4
    ccf0:	e59f1160 	ldr	r1, [pc, #352]	; ce58 <linkpc_loop+0x6a4>
    ccf4:	e3a0200d 	mov	r2, #13	; 0xd
    ccf8:	ebffd187 	bl	131c <strncmp>
    ccfc:	e3500000 	cmp	r0, #0	; 0x0
    cd00:	1a000006 	bne	cd20 <linkpc_loop+0x56c>
    cd04:	e59f20f0 	ldr	r2, [pc, #240]	; cdfc <linkpc_loop+0x648>
    cd08:	e284100e 	add	r1, r4, #14	; 0xe
    cd0c:	e5920008 	ldr	r0, [r2, #8]
    cd10:	ebffd149 	bl	123c <strcpy>
    cd14:	ebfffe92 	bl	c764 <__response_ok>
    cd18:	ebfffea5 	bl	c7b4 <linkpc_loop>
    cd1c:	ea00000b 	b	cd50 <linkpc_loop+0x59c>
    cd20:	e1a00004 	mov	r0, r4
    cd24:	e59f1130 	ldr	r1, [pc, #304]	; ce5c <linkpc_loop+0x6a8>
    cd28:	e3a02007 	mov	r2, #7	; 0x7
    cd2c:	ebffd17a 	bl	131c <strncmp>
    cd30:	e3500000 	cmp	r0, #0	; 0x0
    cd34:	1a000002 	bne	cd44 <linkpc_loop+0x590>
    cd38:	ebfffe89 	bl	c764 <__response_ok>
    cd3c:	ebffe51b 	bl	61b0 <cdbg_shell>
    cd40:	eafffffe 	b	cd40 <linkpc_loop+0x58c>
    cd44:	e59f1114 	ldr	r1, [pc, #276]	; ce60 <linkpc_loop+0x6ac>
    cd48:	e1a00007 	mov	r0, r7
    cd4c:	eb0004a2 	bl	dfdc <sprintf>
    cd50:	e3a01c01 	mov	r1, #256	; 0x100
    cd54:	e1a00007 	mov	r0, r7
    cd58:	ebffd196 	bl	13b8 <strnlen>
    cd5c:	e1a01000 	mov	r1, r0
    cd60:	e1a00007 	mov	r0, r7
    cd64:	ebfffd8f 	bl	c3a8 <udc_write>
    cd68:	e3500000 	cmp	r0, #0	; 0x0
    cd6c:	0afffea4 	beq	c804 <linkpc_loop+0x50>
    cd70:	e59f00ec 	ldr	r0, [pc, #236]	; ce64 <linkpc_loop+0x6b0>
    cd74:	e59f3028 	ldr	r3, [pc, #40]	; cda4 <linkpc_loop+0x5f0>
    cd78:	e1a0e00f 	mov	lr, pc
    cd7c:	e593f000 	ldr	pc, [r3]
    cd80:	e3a04000 	mov	r4, #0	; 0x0
    cd84:	e59f00dc 	ldr	r0, [pc, #220]	; ce68 <linkpc_loop+0x6b4>
    cd88:	e59f3014 	ldr	r3, [pc, #20]	; cda4 <linkpc_loop+0x5f0>
    cd8c:	e1a0e00f 	mov	lr, pc
    cd90:	e593f000 	ldr	pc, [r3]
    cd94:	e1a00004 	mov	r0, r4
    cd98:	e28ddf59 	add	sp, sp, #356	; 0x164
    cd9c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    cda0:	00014022 	.word	0x00014022
    cda4:	0800d764 	.word	0x0800d764
    cda8:	0001234a 	.word	0x0001234a
    cdac:	00014031 	.word	0x00014031
    cdb0:	0800d2e8 	.word	0x0800d2e8
    cdb4:	0800d2ec 	.word	0x0800d2ec
    cdb8:	00011da8 	.word	0x00011da8
    cdbc:	0001191b 	.word	0x0001191b
    cdc0:	0800d2f0 	.word	0x0800d2f0
    cdc4:	0800d2f4 	.word	0x0800d2f4
    cdc8:	00014044 	.word	0x00014044
    cdcc:	000118fb 	.word	0x000118fb
    cdd0:	0001404a 	.word	0x0001404a
    cdd4:	0001406d 	.word	0x0001406d
    cdd8:	00014072 	.word	0x00014072
    cddc:	00014079 	.word	0x00014079
    cde0:	00011927 	.word	0x00011927
    cde4:	00014081 	.word	0x00014081
    cde8:	00014089 	.word	0x00014089
    cdec:	0800d108 	.word	0x0800d108
    cdf0:	00014091 	.word	0x00014091
    cdf4:	0800d188 	.word	0x0800d188
    cdf8:	00014097 	.word	0x00014097
    cdfc:	0800d7cc 	.word	0x0800d7cc
    ce00:	000140a0 	.word	0x000140a0
    ce04:	00014103 	.word	0x00014103
    ce08:	000140ad 	.word	0x000140ad
    ce0c:	000140b7 	.word	0x000140b7
    ce10:	000140d6 	.word	0x000140d6
    ce14:	00014108 	.word	0x00014108
    ce18:	000140ef 	.word	0x000140ef
    ce1c:	00014111 	.word	0x00014111
    ce20:	000121d2 	.word	0x000121d2
    ce24:	00014118 	.word	0x00014118
    ce28:	0800d2fc 	.word	0x0800d2fc
    ce2c:	0800d2f8 	.word	0x0800d2f8
    ce30:	00014130 	.word	0x00014130
    ce34:	00011934 	.word	0x00011934
    ce38:	00014140 	.word	0x00014140
    ce3c:	00014157 	.word	0x00014157
    ce40:	00014165 	.word	0x00014165
    ce44:	0001417e 	.word	0x0001417e
    ce48:	00014193 	.word	0x00014193
    ce4c:	0001419c 	.word	0x0001419c
    ce50:	000141a7 	.word	0x000141a7
    ce54:	000141bf 	.word	0x000141bf
    ce58:	000141da 	.word	0x000141da
    ce5c:	000141e8 	.word	0x000141e8
    ce60:	000141f0 	.word	0x000141f0
    ce64:	00014204 	.word	0x00014204
    ce68:	00014227 	.word	0x00014227

0000ce6c <print_size>:
    ce6c:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    ce70:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    ce74:	e1a05000 	mov	r5, r0
    ce78:	e1a08001 	mov	r8, r1
    ce7c:	83a04101 	movhi	r4, #1073741824	; 0x40000000
    ce80:	83a07047 	movhi	r7, #71	; 0x47
    ce84:	8a000005 	bhi	cea0 <print_size+0x34>
    ce88:	e59f30a8 	ldr	r3, [pc, #168]	; cf38 <print_size+0xcc>
    ce8c:	e1500003 	cmp	r0, r3
    ce90:	83a0704d 	movhi	r7, #77	; 0x4d
    ce94:	93a0706b 	movls	r7, #107	; 0x6b
    ce98:	83a04601 	movhi	r4, #1048576	; 0x100000
    ce9c:	93a04b01 	movls	r4, #1024	; 0x400
    cea0:	e1a01004 	mov	r1, r4
    cea4:	e1a00005 	mov	r0, r5
    cea8:	eb000cc3 	bl	101bc <__aeabi_uidiv>
    ceac:	e1a01004 	mov	r1, r4
    ceb0:	e1a06000 	mov	r6, r0
    ceb4:	e1a00005 	mov	r0, r5
    ceb8:	eb000d3b 	bl	103ac <__aeabi_uidivmod>
    cebc:	e2510000 	subs	r0, r1, #0	; 0x0
    cec0:	01a04000 	moveq	r4, r0
    cec4:	0a000009 	beq	cef0 <print_size+0x84>
    cec8:	e0625694 	mls	r2, r4, r6, r5
    cecc:	e1a030a4 	lsr	r3, r4, #1
    ced0:	e3a0000a 	mov	r0, #10	; 0xa
    ced4:	e1a01004 	mov	r1, r4
    ced8:	e0203290 	mla	r0, r0, r2, r3
    cedc:	eb000cb6 	bl	101bc <__aeabi_uidiv>
    cee0:	e3500009 	cmp	r0, #9	; 0x9
    cee4:	e1a04000 	mov	r4, r0
    cee8:	82866001 	addhi	r6, r6, #1	; 0x1
    ceec:	8240400a 	subhi	r4, r0, #10	; 0xa
    cef0:	e1a01006 	mov	r1, r6
    cef4:	e59f5040 	ldr	r5, [pc, #64]	; cf3c <print_size+0xd0>
    cef8:	e59f0040 	ldr	r0, [pc, #64]	; cf40 <print_size+0xd4>
    cefc:	e1a0e00f 	mov	lr, pc
    cf00:	e595f000 	ldr	pc, [r5]
    cf04:	e3540000 	cmp	r4, #0	; 0x0
    cf08:	0a000003 	beq	cf1c <print_size+0xb0>
    cf0c:	e1a01004 	mov	r1, r4
    cf10:	e59f002c 	ldr	r0, [pc, #44]	; cf44 <print_size+0xd8>
    cf14:	e1a0e00f 	mov	lr, pc
    cf18:	e595f000 	ldr	pc, [r5]
    cf1c:	e1a01007 	mov	r1, r7
    cf20:	e1a02008 	mov	r2, r8
    cf24:	e59f001c 	ldr	r0, [pc, #28]	; cf48 <print_size+0xdc>
    cf28:	e59f300c 	ldr	r3, [pc, #12]	; cf3c <print_size+0xd0>
    cf2c:	e1a0e00f 	mov	lr, pc
    cf30:	e593f000 	ldr	pc, [r3]
    cf34:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    cf38:	000fffff 	.word	0x000fffff
    cf3c:	0800d764 	.word	0x0800d764
    cf40:	0001422f 	.word	0x0001422f
    cf44:	00014234 	.word	0x00014234
    cf48:	00014239 	.word	0x00014239

0000cf4c <print_buffer>:
    cf4c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    cf50:	e24dd044 	sub	sp, sp, #68	; 0x44
    cf54:	e59d6068 	ldr	r6, [sp, #104]
    cf58:	e1a07002 	mov	r7, r2
    cf5c:	e0020692 	mul	r2, r2, r6
    cf60:	e3520040 	cmp	r2, #64	; 0x40
    cf64:	e1a09000 	mov	r9, r0
    cf68:	e1a0a001 	mov	sl, r1
    cf6c:	e1a08003 	mov	r8, r3
    cf70:	9a000003 	bls	cf84 <print_buffer+0x38>
    cf74:	e3a00040 	mov	r0, #64	; 0x40
    cf78:	e1a01007 	mov	r1, r7
    cf7c:	eb000c8e 	bl	101bc <__aeabi_uidiv>
    cf80:	e1a06000 	mov	r6, r0
    cf84:	e3560000 	cmp	r6, #0	; 0x0
    cf88:	1a00003c 	bne	d080 <print_buffer+0x134>
    cf8c:	e3a00010 	mov	r0, #16	; 0x10
    cf90:	e1a01007 	mov	r1, r7
    cf94:	eb000c88 	bl	101bc <__aeabi_uidiv>
    cf98:	e1a06000 	mov	r6, r0
    cf9c:	ea000037 	b	d080 <print_buffer+0x134>
    cfa0:	e59f00f0 	ldr	r0, [pc, #240]	; d098 <print_buffer+0x14c>
    cfa4:	e1a01009 	mov	r1, r9
    cfa8:	e1a0e00f 	mov	lr, pc
    cfac:	e59bf000 	ldr	pc, [fp]
    cfb0:	e1560008 	cmp	r6, r8
    cfb4:	21a06008 	movcs	r6, r8
    cfb8:	e1a0500a 	mov	r5, sl
    cfbc:	e3a04000 	mov	r4, #0	; 0x0
    cfc0:	ea00000f 	b	d004 <print_buffer+0xb8>
    cfc4:	e3570004 	cmp	r7, #4	; 0x4
    cfc8:	05951000 	ldreq	r1, [r5]
    cfcc:	059f00c8 	ldreq	r0, [pc, #200]	; d09c <print_buffer+0x150>
    cfd0:	078d1104 	streq	r1, [sp, r4, lsl #2]
    cfd4:	0a000006 	beq	cff4 <print_buffer+0xa8>
    cfd8:	e3570002 	cmp	r7, #2	; 0x2
    cfdc:	01d510b0 	ldrheq	r1, [r5]
    cfe0:	15d51000 	ldrbne	r1, [r5]
    cfe4:	01a03084 	lsleq	r3, r4, #1
    cfe8:	059f00b0 	ldreq	r0, [pc, #176]	; d0a0 <print_buffer+0x154>
    cfec:	018d10b3 	strheq	r1, [sp, r3]
    cff0:	17cd1004 	strbne	r1, [sp, r4]
    cff4:	e1a0e00f 	mov	lr, pc
    cff8:	e59bf000 	ldr	pc, [fp]
    cffc:	e0855007 	add	r5, r5, r7
    d000:	e2844001 	add	r4, r4, #1	; 0x1
    d004:	e1540006 	cmp	r4, r6
    d008:	e59f0094 	ldr	r0, [pc, #148]	; d0a4 <print_buffer+0x158>
    d00c:	3affffec 	bcc	cfc4 <print_buffer+0x78>
    d010:	e0050697 	mul	r5, r7, r6
    d014:	e59f008c 	ldr	r0, [pc, #140]	; d0a8 <print_buffer+0x15c>
    d018:	e08aa005 	add	sl, sl, r5
    d01c:	eb000450 	bl	e164 <puts>
    d020:	e3a04000 	mov	r4, #0	; 0x0
    d024:	ea000009 	b	d050 <print_buffer+0x104>
    d028:	e7dd0004 	ldrb	r0, [sp, r4]
    d02c:	e59f2078 	ldr	r2, [pc, #120]	; d0ac <print_buffer+0x160>
    d030:	e2844001 	add	r4, r4, #1	; 0x1
    d034:	e7d23000 	ldrb	r3, [r2, r0]
    d038:	e3130097 	tst	r3, #151	; 0x97
    d03c:	0a000001 	beq	d048 <print_buffer+0xfc>
    d040:	e3100080 	tst	r0, #128	; 0x80
    d044:	0a000000 	beq	d04c <print_buffer+0x100>
    d048:	e3a0002e 	mov	r0, #46	; 0x2e
    d04c:	eb00046c 	bl	e204 <putc>
    d050:	e1540005 	cmp	r4, r5
    d054:	3afffff3 	bcc	d028 <print_buffer+0xdc>
    d058:	e59f0050 	ldr	r0, [pc, #80]	; d0b0 <print_buffer+0x164>
    d05c:	e1a0e00f 	mov	lr, pc
    d060:	e59bf000 	ldr	pc, [fp]
    d064:	eb000469 	bl	e210 <ctrlc>
    d068:	e3500000 	cmp	r0, #0	; 0x0
    d06c:	13e00000 	mvnne	r0, #0	; 0x0
    d070:	1a000006 	bne	d090 <print_buffer+0x144>
    d074:	e0899005 	add	r9, r9, r5
    d078:	e0668008 	rsb	r8, r6, r8
    d07c:	ea000000 	b	d084 <print_buffer+0x138>
    d080:	e59fb02c 	ldr	fp, [pc, #44]	; d0b4 <print_buffer+0x168>
    d084:	e3580000 	cmp	r8, #0	; 0x0
    d088:	1affffc4 	bne	cfa0 <print_buffer+0x54>
    d08c:	e1a00008 	mov	r0, r8
    d090:	e28dd044 	add	sp, sp, #68	; 0x44
    d094:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d098:	00014240 	.word	0x00014240
    d09c:	00013367 	.word	0x00013367
    d0a0:	00014247 	.word	0x00014247
    d0a4:	0001424d 	.word	0x0001424d
    d0a8:	00014253 	.word	0x00014253
    d0ac:	0001195e 	.word	0x0001195e
    d0b0:	0001250e 	.word	0x0001250e
    d0b4:	0800d764 	.word	0x0800d764

0000d0b8 <__div64_32>:
    d0b8:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    d0bc:	e8900300 	ldm	r0, {r8, r9}
    d0c0:	e1510009 	cmp	r1, r9
    d0c4:	e1a0a000 	mov	sl, r0
    d0c8:	e1a04001 	mov	r4, r1
    d0cc:	83a06000 	movhi	r6, #0	; 0x0
    d0d0:	83a07000 	movhi	r7, #0	; 0x0
    d0d4:	8a000007 	bhi	d0f8 <__div64_32+0x40>
    d0d8:	e1a00009 	mov	r0, r9
    d0dc:	eb000c36 	bl	101bc <__aeabi_uidiv>
    d0e0:	e0030094 	mul	r3, r4, r0
    d0e4:	e3a06000 	mov	r6, #0	; 0x0
    d0e8:	e1a02006 	mov	r2, r6
    d0ec:	e1a07000 	mov	r7, r0
    d0f0:	e0588002 	subs	r8, r8, r2
    d0f4:	e0c99003 	sbc	r9, r9, r3
    d0f8:	e1a00004 	mov	r0, r4
    d0fc:	e3a01000 	mov	r1, #0	; 0x0
    d100:	e3a05000 	mov	r5, #0	; 0x0
    d104:	e3a04001 	mov	r4, #1	; 0x1
    d108:	ea000003 	b	d11c <__div64_32+0x64>
    d10c:	e0900000 	adds	r0, r0, r0
    d110:	e0a11001 	adc	r1, r1, r1
    d114:	e0944004 	adds	r4, r4, r4
    d118:	e0a55005 	adc	r5, r5, r5
    d11c:	e3510000 	cmp	r1, #0	; 0x0
    d120:	e3a03000 	mov	r3, #0	; 0x0
    d124:	ca000002 	bgt	d134 <__div64_32+0x7c>
    d128:	1a000002 	bne	d138 <__div64_32+0x80>
    d12c:	e3500000 	cmp	r0, #0	; 0x0
    d130:	0a000000 	beq	d138 <__div64_32+0x80>
    d134:	e3a03001 	mov	r3, #1	; 0x1
    d138:	e1590001 	cmp	r9, r1
    d13c:	e3a02000 	mov	r2, #0	; 0x0
    d140:	8a000002 	bhi	d150 <__div64_32+0x98>
    d144:	1a000002 	bne	d154 <__div64_32+0x9c>
    d148:	e1580000 	cmp	r8, r0
    d14c:	9a000000 	bls	d154 <__div64_32+0x9c>
    d150:	e3a02001 	mov	r2, #1	; 0x1
    d154:	e0033002 	and	r3, r3, r2
    d158:	e31300ff 	tst	r3, #255	; 0xff
    d15c:	1affffea 	bne	d10c <__div64_32+0x54>
    d160:	e1510009 	cmp	r1, r9
    d164:	8a000006 	bhi	d184 <__div64_32+0xcc>
    d168:	1a000001 	bne	d174 <__div64_32+0xbc>
    d16c:	e1500008 	cmp	r0, r8
    d170:	8a000003 	bhi	d184 <__div64_32+0xcc>
    d174:	e0588000 	subs	r8, r8, r0
    d178:	e0c99001 	sbc	r9, r9, r1
    d17c:	e0966004 	adds	r6, r6, r4
    d180:	e0a77005 	adc	r7, r7, r5
    d184:	e1b050a5 	lsrs	r5, r5, #1
    d188:	e1a04064 	rrx	r4, r4
    d18c:	e1943005 	orrs	r3, r4, r5
    d190:	0a000002 	beq	d1a0 <__div64_32+0xe8>
    d194:	e1b010a1 	lsrs	r1, r1, #1
    d198:	e1a00060 	rrx	r0, r0
    d19c:	eaffffef 	b	d160 <__div64_32+0xa8>
    d1a0:	e88a00c0 	stm	sl, {r6, r7}
    d1a4:	e1a00008 	mov	r0, r8
    d1a8:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

0000d1ac <simple_strtoul>:
    d1ac:	e92d4070 	push	{r4, r5, r6, lr}
    d1b0:	e5d03000 	ldrb	r3, [r0]
    d1b4:	e1a05001 	mov	r5, r1
    d1b8:	e3530030 	cmp	r3, #48	; 0x30
    d1bc:	e1a01002 	mov	r1, r2
    d1c0:	1a00000d 	bne	d1fc <simple_strtoul+0x50>
    d1c4:	e5f03001 	ldrb	r3, [r0, #1]!
    d1c8:	e3530078 	cmp	r3, #120	; 0x78
    d1cc:	1a000007 	bne	d1f0 <simple_strtoul+0x44>
    d1d0:	e5d03001 	ldrb	r3, [r0, #1]
    d1d4:	e59f2084 	ldr	r2, [pc, #132]	; d260 <simple_strtoul+0xb4>
    d1d8:	e280c001 	add	ip, r0, #1	; 0x1
    d1dc:	e7d23003 	ldrb	r3, [r2, r3]
    d1e0:	e3130044 	tst	r3, #68	; 0x44
    d1e4:	11a0000c 	movne	r0, ip
    d1e8:	13a01010 	movne	r1, #16	; 0x10
    d1ec:	1a000004 	bne	d204 <simple_strtoul+0x58>
    d1f0:	e3510000 	cmp	r1, #0	; 0x0
    d1f4:	03a01008 	moveq	r1, #8	; 0x8
    d1f8:	ea000001 	b	d204 <simple_strtoul+0x58>
    d1fc:	e3520000 	cmp	r2, #0	; 0x0
    d200:	03a0100a 	moveq	r1, #10	; 0xa
    d204:	e3a04000 	mov	r4, #0	; 0x0
    d208:	e59f6050 	ldr	r6, [pc, #80]	; d260 <simple_strtoul+0xb4>
    d20c:	ea000001 	b	d218 <simple_strtoul+0x6c>
    d210:	e0243491 	mla	r4, r1, r4, r3
    d214:	e2800001 	add	r0, r0, #1	; 0x1
    d218:	e5d0c000 	ldrb	ip, [r0]
    d21c:	e7d6200c 	ldrb	r2, [r6, ip]
    d220:	e24c3030 	sub	r3, ip, #48	; 0x30
    d224:	e3120044 	tst	r2, #68	; 0x44
    d228:	0a000008 	beq	d250 <simple_strtoul+0xa4>
    d22c:	e3120004 	tst	r2, #4	; 0x4
    d230:	1a000004 	bne	d248 <simple_strtoul+0x9c>
    d234:	e24c3020 	sub	r3, ip, #32	; 0x20
    d238:	e3120002 	tst	r2, #2	; 0x2
    d23c:	e6ef3073 	uxtb	r3, r3
    d240:	12433037 	subne	r3, r3, #55	; 0x37
    d244:	024c3037 	subeq	r3, ip, #55	; 0x37
    d248:	e1530001 	cmp	r3, r1
    d24c:	3affffef 	bcc	d210 <simple_strtoul+0x64>
    d250:	e3550000 	cmp	r5, #0	; 0x0
    d254:	15850000 	strne	r0, [r5]
    d258:	e1a00004 	mov	r0, r4
    d25c:	e8bd8070 	pop	{r4, r5, r6, pc}
    d260:	0001195e 	.word	0x0001195e

0000d264 <simple_strtol>:
    d264:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    d268:	e5d03000 	ldrb	r3, [r0]
    d26c:	e24dd004 	sub	sp, sp, #4	; 0x4
    d270:	e353002d 	cmp	r3, #45	; 0x2d
    d274:	1a000004 	bne	d28c <simple_strtol+0x28>
    d278:	e2800001 	add	r0, r0, #1	; 0x1
    d27c:	ebffffca 	bl	d1ac <simple_strtoul>
    d280:	e2600000 	rsb	r0, r0, #0	; 0x0
    d284:	e28dd004 	add	sp, sp, #4	; 0x4
    d288:	e8bd8000 	pop	{pc}
    d28c:	e28dd004 	add	sp, sp, #4	; 0x4
    d290:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    d294:	eaffffc4 	b	d1ac <simple_strtoul>

0000d298 <ustrtoul>:
    d298:	e92d4010 	push	{r4, lr}
    d29c:	e1a04001 	mov	r4, r1
    d2a0:	ebffffc1 	bl	d1ac <simple_strtoul>
    d2a4:	e5941000 	ldr	r1, [r4]
    d2a8:	e5d13000 	ldrb	r3, [r1]
    d2ac:	e353004b 	cmp	r3, #75	; 0x4b
    d2b0:	0a00000a 	beq	d2e0 <ustrtoul+0x48>
    d2b4:	8a000002 	bhi	d2c4 <ustrtoul+0x2c>
    d2b8:	e3530047 	cmp	r3, #71	; 0x47
    d2bc:	18bd8010 	popne	{r4, pc}
    d2c0:	ea000004 	b	d2d8 <ustrtoul+0x40>
    d2c4:	e353004d 	cmp	r3, #77	; 0x4d
    d2c8:	0a000003 	beq	d2dc <ustrtoul+0x44>
    d2cc:	e353006b 	cmp	r3, #107	; 0x6b
    d2d0:	18bd8010 	popne	{r4, pc}
    d2d4:	ea000001 	b	d2e0 <ustrtoul+0x48>
    d2d8:	e1a00500 	lsl	r0, r0, #10
    d2dc:	e1a00500 	lsl	r0, r0, #10
    d2e0:	e5d13001 	ldrb	r3, [r1, #1]
    d2e4:	e1a00500 	lsl	r0, r0, #10
    d2e8:	e3530069 	cmp	r3, #105	; 0x69
    d2ec:	18bd8010 	popne	{r4, pc}
    d2f0:	e5d13002 	ldrb	r3, [r1, #2]
    d2f4:	e2812002 	add	r2, r1, #2	; 0x2
    d2f8:	e3530042 	cmp	r3, #66	; 0x42
    d2fc:	02813003 	addeq	r3, r1, #3	; 0x3
    d300:	05843000 	streq	r3, [r4]
    d304:	15842000 	strne	r2, [r4]
    d308:	e8bd8010 	pop	{r4, pc}

0000d30c <simple_strtoull>:
    d30c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    d310:	e5d03000 	ldrb	r3, [r0]
    d314:	e1a07000 	mov	r7, r0
    d318:	e3530030 	cmp	r3, #48	; 0x30
    d31c:	e1a08001 	mov	r8, r1
    d320:	e1a06002 	mov	r6, r2
    d324:	1a00000d 	bne	d360 <simple_strtoull+0x54>
    d328:	e5f73001 	ldrb	r3, [r7, #1]!
    d32c:	e3530078 	cmp	r3, #120	; 0x78
    d330:	1a000007 	bne	d354 <simple_strtoull+0x48>
    d334:	e5d73001 	ldrb	r3, [r7, #1]
    d338:	e59f20ac 	ldr	r2, [pc, #172]	; d3ec <simple_strtoull+0xe0>
    d33c:	e2870001 	add	r0, r7, #1	; 0x1
    d340:	e7d23003 	ldrb	r3, [r2, r3]
    d344:	e3130044 	tst	r3, #68	; 0x44
    d348:	11a07000 	movne	r7, r0
    d34c:	13a06010 	movne	r6, #16	; 0x10
    d350:	1a000004 	bne	d368 <simple_strtoull+0x5c>
    d354:	e3560000 	cmp	r6, #0	; 0x0
    d358:	03a06008 	moveq	r6, #8	; 0x8
    d35c:	ea000001 	b	d368 <simple_strtoull+0x5c>
    d360:	e3520000 	cmp	r2, #0	; 0x0
    d364:	03a0600a 	moveq	r6, #10	; 0xa
    d368:	e3a04000 	mov	r4, #0	; 0x0
    d36c:	e3a05000 	mov	r5, #0	; 0x0
    d370:	e59f9074 	ldr	r9, [pc, #116]	; d3ec <simple_strtoull+0xe0>
    d374:	e3a0a000 	mov	sl, #0	; 0x0
    d378:	ea000004 	b	d390 <simple_strtoull+0x84>
    d37c:	e0832694 	umull	r2, r3, r4, r6
    d380:	e0233596 	mla	r3, r6, r5, r3
    d384:	e0904002 	adds	r4, r0, r2
    d388:	e0a15003 	adc	r5, r1, r3
    d38c:	e2877001 	add	r7, r7, #1	; 0x1
    d390:	e5d7c000 	ldrb	ip, [r7]
    d394:	e7d9200c 	ldrb	r2, [r9, ip]
    d398:	e24c3030 	sub	r3, ip, #48	; 0x30
    d39c:	e3120044 	tst	r2, #68	; 0x44
    d3a0:	0a00000c 	beq	d3d8 <simple_strtoull+0xcc>
    d3a4:	e3120004 	tst	r2, #4	; 0x4
    d3a8:	1a000004 	bne	d3c0 <simple_strtoull+0xb4>
    d3ac:	e24c3020 	sub	r3, ip, #32	; 0x20
    d3b0:	e3120002 	tst	r2, #2	; 0x2
    d3b4:	e6ef3073 	uxtb	r3, r3
    d3b8:	12433037 	subne	r3, r3, #55	; 0x37
    d3bc:	024c3037 	subeq	r3, ip, #55	; 0x37
    d3c0:	e1a00003 	mov	r0, r3
    d3c4:	e1a01fc0 	asr	r1, r0, #31
    d3c8:	e15a0001 	cmp	sl, r1
    d3cc:	1a000001 	bne	d3d8 <simple_strtoull+0xcc>
    d3d0:	e1560000 	cmp	r6, r0
    d3d4:	8affffe8 	bhi	d37c <simple_strtoull+0x70>
    d3d8:	e3580000 	cmp	r8, #0	; 0x0
    d3dc:	15887000 	strne	r7, [r8]
    d3e0:	e1a01005 	mov	r1, r5
    d3e4:	e1a00004 	mov	r0, r4
    d3e8:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    d3ec:	0001195e 	.word	0x0001195e

0000d3f0 <put_dec_trunc>:
    d3f0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    d3f4:	e1a03421 	lsr	r3, r1, #8
    d3f8:	e1a04221 	lsr	r4, r1, #4
    d3fc:	e204400f 	and	r4, r4, #15	; 0xf
    d400:	e203600f 	and	r6, r3, #15	; 0xf
    d404:	e1a05621 	lsr	r5, r1, #12
    d408:	e0842006 	add	r2, r4, r6
    d40c:	e201100f 	and	r1, r1, #15	; 0xf
    d410:	e3a03006 	mov	r3, #6	; 0x6
    d414:	e0822005 	add	r2, r2, r5
    d418:	e0221293 	mla	r2, r3, r2, r1
    d41c:	e3a080cd 	mov	r8, #205	; 0xcd
    d420:	e00c0298 	mul	ip, r8, r2
    d424:	e3a01009 	mov	r1, #9	; 0x9
    d428:	e0214591 	mla	r1, r1, r5, r4
    d42c:	e1a0c5ac 	lsr	ip, ip, #11
    d430:	e3a0700a 	mov	r7, #10	; 0xa
    d434:	e3a03005 	mov	r3, #5	; 0x5
    d438:	e0231693 	mla	r3, r3, r6, r1
    d43c:	e0622c97 	mls	r2, r7, ip, r2
    d440:	e093100c 	adds	r1, r3, ip
    d444:	e2822030 	add	r2, r2, #48	; 0x30
    d448:	e1a04000 	mov	r4, r0
    d44c:	e4c02001 	strb	r2, [r0], #1
    d450:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    d454:	e0030198 	mul	r3, r8, r1
    d458:	e1a035a3 	lsr	r3, r3, #11
    d45c:	e0621397 	mls	r2, r7, r3, r1
    d460:	e0831086 	add	r1, r3, r6, lsl #1
    d464:	e2822030 	add	r2, r2, #48	; 0x30
    d468:	e1913005 	orrs	r3, r1, r5
    d46c:	e5c42001 	strb	r2, [r4, #1]
    d470:	e2800001 	add	r0, r0, #1	; 0x1
    d474:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    d478:	e3a0300d 	mov	r3, #13	; 0xd
    d47c:	e0030193 	mul	r3, r3, r1
    d480:	e1a033a3 	lsr	r3, r3, #7
    d484:	e0621397 	mls	r2, r7, r3, r1
    d488:	e0931105 	adds	r1, r3, r5, lsl #2
    d48c:	e2822030 	add	r2, r2, #48	; 0x30
    d490:	e4c02001 	strb	r2, [r0], #1
    d494:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    d498:	e0030198 	mul	r3, r8, r1
    d49c:	e1a0c5a3 	lsr	ip, r3, #11
    d4a0:	e0631c97 	mls	r3, r7, ip, r1
    d4a4:	e35c0000 	cmp	ip, #0	; 0x0
    d4a8:	e2833030 	add	r3, r3, #48	; 0x30
    d4ac:	e4c03001 	strb	r3, [r0], #1
    d4b0:	128c3030 	addne	r3, ip, #48	; 0x30
    d4b4:	14c03001 	strbne	r3, [r0], #1
    d4b8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000d4bc <string>:
    d4bc:	e59fc0a4 	ldr	ip, [pc, #164]	; d568 <string+0xac>
    d4c0:	e3510000 	cmp	r1, #0	; 0x0
    d4c4:	e92d4070 	push	{r4, r5, r6, lr}
    d4c8:	11a06001 	movne	r6, r1
    d4cc:	01a0600c 	moveq	r6, ip
    d4d0:	e1a01003 	mov	r1, r3
    d4d4:	e1a04000 	mov	r4, r0
    d4d8:	e1a00006 	mov	r0, r6
    d4dc:	e1a05002 	mov	r5, r2
    d4e0:	ebffcfb4 	bl	13b8 <strnlen>
    d4e4:	e59d3010 	ldr	r3, [sp, #16]
    d4e8:	e1a0c000 	mov	ip, r0
    d4ec:	e3130010 	tst	r3, #16	; 0x10
    d4f0:	01a02004 	moveq	r2, r4
    d4f4:	03a01020 	moveq	r1, #32	; 0x20
    d4f8:	0a000001 	beq	d504 <string+0x48>
    d4fc:	ea000006 	b	d51c <string+0x60>
    d500:	e5421001 	strb	r1, [r2, #-1]
    d504:	e2455001 	sub	r5, r5, #1	; 0x1
    d508:	e2853001 	add	r3, r5, #1	; 0x1
    d50c:	e15c0003 	cmp	ip, r3
    d510:	e1a04002 	mov	r4, r2
    d514:	e2822001 	add	r2, r2, #1	; 0x1
    d518:	bafffff8 	blt	d500 <string+0x44>
    d51c:	e3a02000 	mov	r2, #0	; 0x0
    d520:	ea000002 	b	d530 <string+0x74>
    d524:	e7d63002 	ldrb	r3, [r6, r2]
    d528:	e7c43002 	strb	r3, [r4, r2]
    d52c:	e2822001 	add	r2, r2, #1	; 0x1
    d530:	e152000c 	cmp	r2, ip
    d534:	e0843002 	add	r3, r4, r2
    d538:	bafffff9 	blt	d524 <string+0x68>
    d53c:	e1a01003 	mov	r1, r3
    d540:	e1a02005 	mov	r2, r5
    d544:	e3a03020 	mov	r3, #32	; 0x20
    d548:	ea000000 	b	d550 <string+0x94>
    d54c:	e5413001 	strb	r3, [r1, #-1]
    d550:	e15c0002 	cmp	ip, r2
    d554:	e1a00001 	mov	r0, r1
    d558:	e2422001 	sub	r2, r2, #1	; 0x1
    d55c:	e2811001 	add	r1, r1, #1	; 0x1
    d560:	bafffff9 	blt	d54c <string+0x90>
    d564:	e8bd8070 	pop	{r4, r5, r6, pc}
    d568:	00014258 	.word	0x00014258

0000d56c <put_dec>:
    d56c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d570:	e24dd00c 	sub	sp, sp, #12	; 0xc
    d574:	e28d8008 	add	r8, sp, #8	; 0x8
    d578:	e16820f8 	strd	r2, [r8, #-8]!
    d57c:	e1a07000 	mov	r7, r0
    d580:	e3a090cd 	mov	r9, #205	; 0xcd
    d584:	e3a0a00a 	mov	sl, #10	; 0xa
    d588:	e3a0b00d 	mov	fp, #13	; 0xd
    d58c:	e59d5004 	ldr	r5, [sp, #4]
    d590:	e1a0000d 	mov	r0, sp
    d594:	e3550000 	cmp	r5, #0	; 0x0
    d598:	e59f10f0 	ldr	r1, [pc, #240]	; d690 <put_dec+0x124>
    d59c:	e59d4000 	ldr	r4, [sp]
    d5a0:	1a00000f 	bne	d5e4 <put_dec+0x78>
    d5a4:	e59f30e8 	ldr	r3, [pc, #232]	; d694 <put_dec+0x128>
    d5a8:	e1a00004 	mov	r0, r4
    d5ac:	e1540003 	cmp	r4, r3
    d5b0:	8a000004 	bhi	d5c8 <put_dec+0x5c>
    d5b4:	e1a00007 	mov	r0, r7
    d5b8:	e1a01004 	mov	r1, r4
    d5bc:	ebffff8b 	bl	d3f0 <put_dec_trunc>
    d5c0:	e28dd00c 	add	sp, sp, #12	; 0xc
    d5c4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d5c8:	eb000b77 	bl	103ac <__aeabi_uidivmod>
    d5cc:	e1a00004 	mov	r0, r4
    d5d0:	e1a06001 	mov	r6, r1
    d5d4:	e59f10b4 	ldr	r1, [pc, #180]	; d690 <put_dec+0x124>
    d5d8:	eb000af7 	bl	101bc <__aeabi_uidiv>
    d5dc:	e88d0021 	stm	sp, {r0, r5}
    d5e0:	ea000001 	b	d5ec <put_dec+0x80>
    d5e4:	ebfffeb3 	bl	d0b8 <__div64_32>
    d5e8:	e1a06000 	mov	r6, r0
    d5ec:	e1a02226 	lsr	r2, r6, #4
    d5f0:	e1a0c426 	lsr	ip, r6, #8
    d5f4:	e20cc00f 	and	ip, ip, #15	; 0xf
    d5f8:	e202200f 	and	r2, r2, #15	; 0xf
    d5fc:	e1a0e626 	lsr	lr, r6, #12
    d600:	e082500c 	add	r5, r2, ip
    d604:	e085500e 	add	r5, r5, lr
    d608:	e206300f 	and	r3, r6, #15	; 0xf
    d60c:	e3a01006 	mov	r1, #6	; 0x6
    d610:	e0253591 	mla	r5, r1, r5, r3
    d614:	e3a03009 	mov	r3, #9	; 0x9
    d618:	e0222e93 	mla	r2, r3, lr, r2
    d61c:	e3a01005 	mov	r1, #5	; 0x5
    d620:	e0000599 	mul	r0, r9, r5
    d624:	e0222c91 	mla	r2, r1, ip, r2
    d628:	e1a005a0 	lsr	r0, r0, #11
    d62c:	e0822000 	add	r2, r2, r0
    d630:	e0010299 	mul	r1, r9, r2
    d634:	e1a015a1 	lsr	r1, r1, #11
    d638:	e081c08c 	add	ip, r1, ip, lsl #1
    d63c:	e0030c9b 	mul	r3, fp, ip
    d640:	e1a033a3 	lsr	r3, r3, #7
    d644:	e083e10e 	add	lr, r3, lr, lsl #2
    d648:	e0040e99 	mul	r4, r9, lr
    d64c:	e1a045a4 	lsr	r4, r4, #11
    d650:	e06ee49a 	mls	lr, sl, r4, lr
    d654:	e060509a 	mls	r0, sl, r0, r5
    d658:	e061219a 	mls	r1, sl, r1, r2
    d65c:	e063c39a 	mls	r3, sl, r3, ip
    d660:	e2800030 	add	r0, r0, #48	; 0x30
    d664:	e2811030 	add	r1, r1, #48	; 0x30
    d668:	e2833030 	add	r3, r3, #48	; 0x30
    d66c:	e28ee030 	add	lr, lr, #48	; 0x30
    d670:	e2844030 	add	r4, r4, #48	; 0x30
    d674:	e5c70000 	strb	r0, [r7]
    d678:	e5c71001 	strb	r1, [r7, #1]
    d67c:	e5c73002 	strb	r3, [r7, #2]
    d680:	e5c7e003 	strb	lr, [r7, #3]
    d684:	e5c74004 	strb	r4, [r7, #4]
    d688:	e2877005 	add	r7, r7, #5	; 0x5
    d68c:	eaffffbe 	b	d58c <put_dec+0x20>
    d690:	000186a0 	.word	0x000186a0
    d694:	0001869f 	.word	0x0001869f

0000d698 <number>:
    d698:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d69c:	e24dd05c 	sub	sp, sp, #92	; 0x5c
    d6a0:	e1a0a000 	mov	sl, r0
    d6a4:	e59d0080 	ldr	r0, [sp, #128]
    d6a8:	e59d808c 	ldr	r8, [sp, #140]
    d6ac:	e250100a 	subs	r1, r0, #10	; 0xa
    d6b0:	13a01001 	movne	r1, #1	; 0x1
    d6b4:	e0011328 	and	r1, r1, r8, lsr #6
    d6b8:	e3180010 	tst	r8, #16	; 0x10
    d6bc:	e58d1008 	str	r1, [sp, #8]
    d6c0:	e2081020 	and	r1, r8, #32	; 0x20
    d6c4:	13c88001 	bicne	r8, r8, #1	; 0x1
    d6c8:	e3180002 	tst	r8, #2	; 0x2
    d6cc:	e1a06002 	mov	r6, r2
    d6d0:	e1a07003 	mov	r7, r3
    d6d4:	e59d5084 	ldr	r5, [sp, #132]
    d6d8:	e58d1004 	str	r1, [sp, #4]
    d6dc:	0a000010 	beq	d724 <number+0x8c>
    d6e0:	e3530000 	cmp	r3, #0	; 0x0
    d6e4:	ba000002 	blt	d6f4 <number+0x5c>
    d6e8:	e3180004 	tst	r8, #4	; 0x4
    d6ec:	0a000008 	beq	d714 <number+0x7c>
    d6f0:	ea000004 	b	d708 <number+0x70>
    d6f4:	e2766000 	rsbs	r6, r6, #0	; 0x0
    d6f8:	e2e77000 	rsc	r7, r7, #0	; 0x0
    d6fc:	e2455001 	sub	r5, r5, #1	; 0x1
    d700:	e3a0902d 	mov	r9, #45	; 0x2d
    d704:	ea000007 	b	d728 <number+0x90>
    d708:	e2455001 	sub	r5, r5, #1	; 0x1
    d70c:	e3a0902b 	mov	r9, #43	; 0x2b
    d710:	ea000004 	b	d728 <number+0x90>
    d714:	e3180008 	tst	r8, #8	; 0x8
    d718:	12455001 	subne	r5, r5, #1	; 0x1
    d71c:	13a09020 	movne	r9, #32	; 0x20
    d720:	1a000000 	bne	d728 <number+0x90>
    d724:	e3a09000 	mov	r9, #0	; 0x0
    d728:	e59d2008 	ldr	r2, [sp, #8]
    d72c:	e3520000 	cmp	r2, #0	; 0x0
    d730:	0a000003 	beq	d744 <number+0xac>
    d734:	e59d3080 	ldr	r3, [sp, #128]
    d738:	e2455001 	sub	r5, r5, #1	; 0x1
    d73c:	e3530010 	cmp	r3, #16	; 0x10
    d740:	02455001 	subeq	r5, r5, #1	; 0x1
    d744:	e196c007 	orrs	ip, r6, r7
    d748:	03a03030 	moveq	r3, #48	; 0x30
    d74c:	05cd3016 	strbeq	r3, [sp, #22]
    d750:	03a04001 	moveq	r4, #1	; 0x1
    d754:	0a000020 	beq	d7dc <number+0x144>
    d758:	e59d0080 	ldr	r0, [sp, #128]
    d75c:	e350000a 	cmp	r0, #10	; 0xa
    d760:	0a000017 	beq	d7c4 <number+0x12c>
    d764:	e3500010 	cmp	r0, #16	; 0x10
    d768:	03a01004 	moveq	r1, #4	; 0x4
    d76c:	13a01003 	movne	r1, #3	; 0x3
    d770:	e58d100c 	str	r1, [sp, #12]
    d774:	e240b001 	sub	fp, r0, #1	; 0x1
    d778:	e3a04000 	mov	r4, #0	; 0x0
    d77c:	e6ef3076 	uxtb	r3, r6
    d780:	e59f2164 	ldr	r2, [pc, #356]	; d8ec <number+0x254>
    d784:	e003300b 	and	r3, r3, fp
    d788:	e59dc004 	ldr	ip, [sp, #4]
    d78c:	e7d23003 	ldrb	r3, [r2, r3]
    d790:	e1a00006 	mov	r0, r6
    d794:	e18c3003 	orr	r3, ip, r3
    d798:	e28dc016 	add	ip, sp, #22	; 0x16
    d79c:	e1a01007 	mov	r1, r7
    d7a0:	e59d200c 	ldr	r2, [sp, #12]
    d7a4:	e7cc3004 	strb	r3, [ip, r4]
    d7a8:	eb000b93 	bl	105fc <__aeabi_llsr>
    d7ac:	e1a06000 	mov	r6, r0
    d7b0:	e1960001 	orrs	r0, r6, r1
    d7b4:	e1a07001 	mov	r7, r1
    d7b8:	e2844001 	add	r4, r4, #1	; 0x1
    d7bc:	1affffee 	bne	d77c <number+0xe4>
    d7c0:	ea000005 	b	d7dc <number+0x144>
    d7c4:	e28d4016 	add	r4, sp, #22	; 0x16
    d7c8:	e1a00004 	mov	r0, r4
    d7cc:	e1a02006 	mov	r2, r6
    d7d0:	e1a03007 	mov	r3, r7
    d7d4:	ebffff64 	bl	d56c <put_dec>
    d7d8:	e0644000 	rsb	r4, r4, r0
    d7dc:	e59d3088 	ldr	r3, [sp, #136]
    d7e0:	e1540003 	cmp	r4, r3
    d7e4:	a1a01004 	movge	r1, r4
    d7e8:	b1a01003 	movlt	r1, r3
    d7ec:	e3180011 	tst	r8, #17	; 0x11
    d7f0:	e061c005 	rsb	ip, r1, r5
    d7f4:	01a0300a 	moveq	r3, sl
    d7f8:	03a02020 	moveq	r2, #32	; 0x20
    d7fc:	0a000001 	beq	d808 <number+0x170>
    d800:	ea000004 	b	d818 <number+0x180>
    d804:	e5432001 	strb	r2, [r3, #-1]
    d808:	e25cc001 	subs	ip, ip, #1	; 0x1
    d80c:	e1a0a003 	mov	sl, r3
    d810:	e2833001 	add	r3, r3, #1	; 0x1
    d814:	5afffffa 	bpl	d804 <number+0x16c>
    d818:	e3590000 	cmp	r9, #0	; 0x0
    d81c:	14ca9001 	strbne	r9, [sl], #1
    d820:	e59d2008 	ldr	r2, [sp, #8]
    d824:	e3520000 	cmp	r2, #0	; 0x0
    d828:	0a000006 	beq	d848 <number+0x1b0>
    d82c:	e59d3080 	ldr	r3, [sp, #128]
    d830:	e3530010 	cmp	r3, #16	; 0x10
    d834:	e3a03030 	mov	r3, #48	; 0x30
    d838:	e4ca3001 	strb	r3, [sl], #1
    d83c:	059d0004 	ldreq	r0, [sp, #4]
    d840:	03803058 	orreq	r3, r0, #88	; 0x58
    d844:	04ca3001 	strbeq	r3, [sl], #1
    d848:	e3180010 	tst	r8, #16	; 0x10
    d84c:	1a000009 	bne	d878 <number+0x1e0>
    d850:	e3180001 	tst	r8, #1	; 0x1
    d854:	13a02030 	movne	r2, #48	; 0x30
    d858:	03a02020 	moveq	r2, #32	; 0x20
    d85c:	e1a0300a 	mov	r3, sl
    d860:	ea000000 	b	d868 <number+0x1d0>
    d864:	e5432001 	strb	r2, [r3, #-1]
    d868:	e25cc001 	subs	ip, ip, #1	; 0x1
    d86c:	e1a0a003 	mov	sl, r3
    d870:	e2833001 	add	r3, r3, #1	; 0x1
    d874:	5afffffa 	bpl	d864 <number+0x1cc>
    d878:	e1a0000a 	mov	r0, sl
    d87c:	e3a03030 	mov	r3, #48	; 0x30
    d880:	ea000000 	b	d888 <number+0x1f0>
    d884:	e5403001 	strb	r3, [r0, #-1]
    d888:	e2411001 	sub	r1, r1, #1	; 0x1
    d88c:	e1540001 	cmp	r4, r1
    d890:	e1a02000 	mov	r2, r0
    d894:	e2800001 	add	r0, r0, #1	; 0x1
    d898:	dafffff9 	ble	d884 <number+0x1ec>
    d89c:	e28d1058 	add	r1, sp, #88	; 0x58
    d8a0:	e0813004 	add	r3, r1, r4
    d8a4:	e2431043 	sub	r1, r3, #67	; 0x43
    d8a8:	ea000001 	b	d8b4 <number+0x21c>
    d8ac:	e5d13001 	ldrb	r3, [r1, #1]
    d8b0:	e5423001 	strb	r3, [r2, #-1]
    d8b4:	e2544001 	subs	r4, r4, #1	; 0x1
    d8b8:	e1a03002 	mov	r3, r2
    d8bc:	e2411001 	sub	r1, r1, #1	; 0x1
    d8c0:	e2822001 	add	r2, r2, #1	; 0x1
    d8c4:	5afffff8 	bpl	d8ac <number+0x214>
    d8c8:	e3a02020 	mov	r2, #32	; 0x20
    d8cc:	ea000000 	b	d8d4 <number+0x23c>
    d8d0:	e5432001 	strb	r2, [r3, #-1]
    d8d4:	e25cc001 	subs	ip, ip, #1	; 0x1
    d8d8:	e1a00003 	mov	r0, r3
    d8dc:	e2833001 	add	r3, r3, #1	; 0x1
    d8e0:	5afffffa 	bpl	d8d0 <number+0x238>
    d8e4:	e28dd05c 	add	sp, sp, #92	; 0x5c
    d8e8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d8ec:	0001194e 	.word	0x0001194e

0000d8f0 <vsprintf>:
    d8f0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d8f4:	e24dd084 	sub	sp, sp, #132	; 0x84
    d8f8:	e58d0028 	str	r0, [sp, #40]
    d8fc:	e1a08001 	mov	r8, r1
    d900:	e1a0a000 	mov	sl, r0
    d904:	e28d107b 	add	r1, sp, #123	; 0x7b
    d908:	e28d007d 	add	r0, sp, #125	; 0x7d
    d90c:	e58d2024 	str	r2, [sp, #36]
    d910:	e1a06002 	mov	r6, r2
    d914:	e58d001c 	str	r0, [sp, #28]
    d918:	e58d1018 	str	r1, [sp, #24]
    d91c:	ea0001a3 	b	dfb0 <vsprintf+0x6c0>
    d920:	e3530025 	cmp	r3, #37	; 0x25
    d924:	1a00015f 	bne	dea8 <vsprintf+0x5b8>
    d928:	e3a07000 	mov	r7, #0	; 0x0
    d92c:	e5f82001 	ldrb	r2, [r8, #1]!
    d930:	e352002b 	cmp	r2, #43	; 0x2b
    d934:	0a00000c 	beq	d96c <vsprintf+0x7c>
    d938:	8a000004 	bhi	d950 <vsprintf+0x60>
    d93c:	e3520020 	cmp	r2, #32	; 0x20
    d940:	0a00000b 	beq	d974 <vsprintf+0x84>
    d944:	e3520023 	cmp	r2, #35	; 0x23
    d948:	1a00000f 	bne	d98c <vsprintf+0x9c>
    d94c:	ea00000a 	b	d97c <vsprintf+0x8c>
    d950:	e352002d 	cmp	r2, #45	; 0x2d
    d954:	0a000002 	beq	d964 <vsprintf+0x74>
    d958:	e3520030 	cmp	r2, #48	; 0x30
    d95c:	1a00000a 	bne	d98c <vsprintf+0x9c>
    d960:	ea000007 	b	d984 <vsprintf+0x94>
    d964:	e3877010 	orr	r7, r7, #16	; 0x10
    d968:	eaffffef 	b	d92c <vsprintf+0x3c>
    d96c:	e3877004 	orr	r7, r7, #4	; 0x4
    d970:	eaffffed 	b	d92c <vsprintf+0x3c>
    d974:	e3877008 	orr	r7, r7, #8	; 0x8
    d978:	eaffffeb 	b	d92c <vsprintf+0x3c>
    d97c:	e3877040 	orr	r7, r7, #64	; 0x40
    d980:	eaffffe9 	b	d92c <vsprintf+0x3c>
    d984:	e3877001 	orr	r7, r7, #1	; 0x1
    d988:	eaffffe7 	b	d92c <vsprintf+0x3c>
    d98c:	e2423030 	sub	r3, r2, #48	; 0x30
    d990:	e6ef3073 	uxtb	r3, r3
    d994:	e3530009 	cmp	r3, #9	; 0x9
    d998:	93a09000 	movls	r9, #0	; 0x0
    d99c:	93a0100a 	movls	r1, #10	; 0xa
    d9a0:	9a000002 	bls	d9b0 <vsprintf+0xc0>
    d9a4:	ea000007 	b	d9c8 <vsprintf+0xd8>
    d9a8:	e0292991 	mla	r9, r1, r9, r2
    d9ac:	e2888001 	add	r8, r8, #1	; 0x1
    d9b0:	e5d83000 	ldrb	r3, [r8]
    d9b4:	e2432030 	sub	r2, r3, #48	; 0x30
    d9b8:	e6ef3072 	uxtb	r3, r2
    d9bc:	e3530009 	cmp	r3, #9	; 0x9
    d9c0:	9afffff8 	bls	d9a8 <vsprintf+0xb8>
    d9c4:	ea000008 	b	d9ec <vsprintf+0xfc>
    d9c8:	e352002a 	cmp	r2, #42	; 0x2a
    d9cc:	13e09000 	mvnne	r9, #0	; 0x0
    d9d0:	1a000005 	bne	d9ec <vsprintf+0xfc>
    d9d4:	e5969000 	ldr	r9, [r6]
    d9d8:	e2888001 	add	r8, r8, #1	; 0x1
    d9dc:	e3590000 	cmp	r9, #0	; 0x0
    d9e0:	e2866004 	add	r6, r6, #4	; 0x4
    d9e4:	b2699000 	rsblt	r9, r9, #0	; 0x0
    d9e8:	b3877010 	orrlt	r7, r7, #16	; 0x10
    d9ec:	e5d83000 	ldrb	r3, [r8]
    d9f0:	e353002e 	cmp	r3, #46	; 0x2e
    d9f4:	13e02000 	mvnne	r2, #0	; 0x0
    d9f8:	158d202c 	strne	r2, [sp, #44]
    d9fc:	1a00001d 	bne	da78 <vsprintf+0x188>
    da00:	e5f82001 	ldrb	r2, [r8, #1]!
    da04:	e2423030 	sub	r3, r2, #48	; 0x30
    da08:	e6ef3073 	uxtb	r3, r3
    da0c:	e3530009 	cmp	r3, #9	; 0x9
    da10:	93a03000 	movls	r3, #0	; 0x0
    da14:	958d302c 	strls	r3, [sp, #44]
    da18:	93a0100a 	movls	r1, #10	; 0xa
    da1c:	9a000004 	bls	da34 <vsprintf+0x144>
    da20:	ea000009 	b	da4c <vsprintf+0x15c>
    da24:	e59d002c 	ldr	r0, [sp, #44]
    da28:	e2888001 	add	r8, r8, #1	; 0x1
    da2c:	e0202091 	mla	r0, r1, r0, r2
    da30:	e58d002c 	str	r0, [sp, #44]
    da34:	e5d83000 	ldrb	r3, [r8]
    da38:	e2432030 	sub	r2, r3, #48	; 0x30
    da3c:	e6ef3072 	uxtb	r3, r2
    da40:	e3530009 	cmp	r3, #9	; 0x9
    da44:	9afffff6 	bls	da24 <vsprintf+0x134>
    da48:	ea000005 	b	da64 <vsprintf+0x174>
    da4c:	e352002a 	cmp	r2, #42	; 0x2a
    da50:	1a000006 	bne	da70 <vsprintf+0x180>
    da54:	e5961000 	ldr	r1, [r6]
    da58:	e2888001 	add	r8, r8, #1	; 0x1
    da5c:	e58d102c 	str	r1, [sp, #44]
    da60:	e2866004 	add	r6, r6, #4	; 0x4
    da64:	e59d202c 	ldr	r2, [sp, #44]
    da68:	e3520000 	cmp	r2, #0	; 0x0
    da6c:	aa000001 	bge	da78 <vsprintf+0x188>
    da70:	e3a03000 	mov	r3, #0	; 0x0
    da74:	e58d302c 	str	r3, [sp, #44]
    da78:	e5d83000 	ldrb	r3, [r8]
    da7c:	e353006c 	cmp	r3, #108	; 0x6c
    da80:	13530068 	cmpne	r3, #104	; 0x68
    da84:	0a000008 	beq	daac <vsprintf+0x1bc>
    da88:	e353004c 	cmp	r3, #76	; 0x4c
    da8c:	0a000006 	beq	daac <vsprintf+0x1bc>
    da90:	e353005a 	cmp	r3, #90	; 0x5a
    da94:	0a000004 	beq	daac <vsprintf+0x1bc>
    da98:	e353007a 	cmp	r3, #122	; 0x7a
    da9c:	0a000002 	beq	daac <vsprintf+0x1bc>
    daa0:	e3530074 	cmp	r3, #116	; 0x74
    daa4:	13e02000 	mvnne	r2, #0	; 0x0
    daa8:	1a000007 	bne	dacc <vsprintf+0x1dc>
    daac:	e353006c 	cmp	r3, #108	; 0x6c
    dab0:	e1a02003 	mov	r2, r3
    dab4:	e2888001 	add	r8, r8, #1	; 0x1
    dab8:	1a000003 	bne	dacc <vsprintf+0x1dc>
    dabc:	e5d83000 	ldrb	r3, [r8]
    dac0:	e353006c 	cmp	r3, #108	; 0x6c
    dac4:	02888001 	addeq	r8, r8, #1	; 0x1
    dac8:	03a0204c 	moveq	r2, #76	; 0x4c
    dacc:	e5d83000 	ldrb	r3, [r8]
    dad0:	e353006e 	cmp	r3, #110	; 0x6e
    dad4:	0a0000df 	beq	de58 <vsprintf+0x568>
    dad8:	8a00000c 	bhi	db10 <vsprintf+0x220>
    dadc:	e3530063 	cmp	r3, #99	; 0x63
    dae0:	0a000017 	beq	db44 <vsprintf+0x254>
    dae4:	8a000004 	bhi	dafc <vsprintf+0x20c>
    dae8:	e3530025 	cmp	r3, #37	; 0x25
    daec:	0a0000ed 	beq	dea8 <vsprintf+0x5b8>
    daf0:	e3530058 	cmp	r3, #88	; 0x58
    daf4:	1a0000e5 	bne	de90 <vsprintf+0x5a0>
    daf8:	ea0000e0 	b	de80 <vsprintf+0x590>
    dafc:	e3530064 	cmp	r3, #100	; 0x64
    db00:	0a0000e0 	beq	de88 <vsprintf+0x598>
    db04:	e3530069 	cmp	r3, #105	; 0x69
    db08:	1a0000e0 	bne	de90 <vsprintf+0x5a0>
    db0c:	ea0000dd 	b	de88 <vsprintf+0x598>
    db10:	e3530073 	cmp	r3, #115	; 0x73
    db14:	0a000021 	beq	dba0 <vsprintf+0x2b0>
    db18:	8a000004 	bhi	db30 <vsprintf+0x240>
    db1c:	e353006f 	cmp	r3, #111	; 0x6f
    db20:	0a0000d3 	beq	de74 <vsprintf+0x584>
    db24:	e3530070 	cmp	r3, #112	; 0x70
    db28:	1a0000d8 	bne	de90 <vsprintf+0x5a0>
    db2c:	ea000023 	b	dbc0 <vsprintf+0x2d0>
    db30:	e3530075 	cmp	r3, #117	; 0x75
    db34:	0a0000dd 	beq	deb0 <vsprintf+0x5c0>
    db38:	e3530078 	cmp	r3, #120	; 0x78
    db3c:	1a0000d3 	bne	de90 <vsprintf+0x5a0>
    db40:	ea0000cd 	b	de7c <vsprintf+0x58c>
    db44:	e3170010 	tst	r7, #16	; 0x10
    db48:	01a0300a 	moveq	r3, sl
    db4c:	03a02020 	moveq	r2, #32	; 0x20
    db50:	0a000001 	beq	db5c <vsprintf+0x26c>
    db54:	ea000005 	b	db70 <vsprintf+0x280>
    db58:	e5432001 	strb	r2, [r3, #-1]
    db5c:	e2499001 	sub	r9, r9, #1	; 0x1
    db60:	e3590000 	cmp	r9, #0	; 0x0
    db64:	e1a0a003 	mov	sl, r3
    db68:	e2833001 	add	r3, r3, #1	; 0x1
    db6c:	cafffff9 	bgt	db58 <vsprintf+0x268>
    db70:	e5963000 	ldr	r3, [r6]
    db74:	e1a0200a 	mov	r2, sl
    db78:	e4c23001 	strb	r3, [r2], #1
    db7c:	e3a03020 	mov	r3, #32	; 0x20
    db80:	ea000000 	b	db88 <vsprintf+0x298>
    db84:	e5423001 	strb	r3, [r2, #-1]
    db88:	e2499001 	sub	r9, r9, #1	; 0x1
    db8c:	e3590000 	cmp	r9, #0	; 0x0
    db90:	e1a0a002 	mov	sl, r2
    db94:	e2822001 	add	r2, r2, #1	; 0x1
    db98:	cafffff9 	bgt	db84 <vsprintf+0x294>
    db9c:	ea0000ab 	b	de50 <vsprintf+0x560>
    dba0:	e58d7000 	str	r7, [sp]
    dba4:	e1a0000a 	mov	r0, sl
    dba8:	e5961000 	ldr	r1, [r6]
    dbac:	e1a02009 	mov	r2, r9
    dbb0:	e59d302c 	ldr	r3, [sp, #44]
    dbb4:	ebfffe40 	bl	d4bc <string>
    dbb8:	e1a0a000 	mov	sl, r0
    dbbc:	ea0000a3 	b	de50 <vsprintf+0x560>
    dbc0:	e596b000 	ldr	fp, [r6]
    dbc4:	e35b0000 	cmp	fp, #0	; 0x0
    dbc8:	1a000005 	bne	dbe4 <vsprintf+0x2f4>
    dbcc:	e1a0000a 	mov	r0, sl
    dbd0:	e1a02009 	mov	r2, r9
    dbd4:	e59d302c 	ldr	r3, [sp, #44]
    dbd8:	e59f13f0 	ldr	r1, [pc, #1008]	; dfd0 <vsprintf+0x6e0>
    dbdc:	e58d7000 	str	r7, [sp]
    dbe0:	ea000081 	b	ddec <vsprintf+0x4fc>
    dbe4:	e5d83001 	ldrb	r3, [r8, #1]
    dbe8:	e2882001 	add	r2, r8, #1	; 0x1
    dbec:	e353004d 	cmp	r3, #77	; 0x4d
    dbf0:	0a000008 	beq	dc18 <vsprintf+0x328>
    dbf4:	8a000002 	bhi	dc04 <vsprintf+0x314>
    dbf8:	e3530049 	cmp	r3, #73	; 0x49
    dbfc:	1a00007c 	bne	ddf4 <vsprintf+0x504>
    dc00:	ea000025 	b	dc9c <vsprintf+0x3ac>
    dc04:	e3530069 	cmp	r3, #105	; 0x69
    dc08:	0a000022 	beq	dc98 <vsprintf+0x3a8>
    dc0c:	e353006d 	cmp	r3, #109	; 0x6d
    dc10:	1a000077 	bne	ddf4 <vsprintf+0x504>
    dc14:	e3877040 	orr	r7, r7, #64	; 0x40
    dc18:	e1a03327 	lsr	r3, r7, #6
    dc1c:	e2233001 	eor	r3, r3, #1	; 0x1
    dc20:	e2034001 	and	r4, r3, #1	; 0x1
    dc24:	e3a0e000 	mov	lr, #0	; 0x0
    dc28:	e28dc05b 	add	ip, sp, #91	; 0x5b
    dc2c:	e3a0503a 	mov	r5, #58	; 0x3a
    dc30:	e7db200e 	ldrb	r2, [fp, lr]
    dc34:	e59f0398 	ldr	r0, [pc, #920]	; dfd4 <vsprintf+0x6e4>
    dc38:	e202300f 	and	r3, r2, #15	; 0xf
    dc3c:	e7d02222 	ldrb	r2, [r0, r2, lsr #4]
    dc40:	e7d03003 	ldrb	r3, [r0, r3]
    dc44:	e1a0000c 	mov	r0, ip
    dc48:	e4c02001 	strb	r2, [r0], #1
    dc4c:	e35e0005 	cmp	lr, #5	; 0x5
    dc50:	03a01000 	moveq	r1, #0	; 0x0
    dc54:	12041001 	andne	r1, r4, #1	; 0x1
    dc58:	e3510000 	cmp	r1, #0	; 0x0
    dc5c:	e5cc3001 	strb	r3, [ip, #1]
    dc60:	e28ee001 	add	lr, lr, #1	; 0x1
    dc64:	e280c001 	add	ip, r0, #1	; 0x1
    dc68:	128cc001 	addne	ip, ip, #1	; 0x1
    dc6c:	15c05001 	strbne	r5, [r0, #1]
    dc70:	e35e0006 	cmp	lr, #6	; 0x6
    dc74:	1affffed 	bne	dc30 <vsprintf+0x340>
    dc78:	e3a03000 	mov	r3, #0	; 0x0
    dc7c:	e5cc3000 	strb	r3, [ip]
    dc80:	e1a0000a 	mov	r0, sl
    dc84:	e3c7c040 	bic	ip, r7, #64	; 0x40
    dc88:	e1a02009 	mov	r2, r9
    dc8c:	e59d302c 	ldr	r3, [sp, #44]
    dc90:	e28d105b 	add	r1, sp, #91	; 0x5b
    dc94:	ea000053 	b	dde8 <vsprintf+0x4f8>
    dc98:	e3877040 	orr	r7, r7, #64	; 0x40
    dc9c:	e5d23001 	ldrb	r3, [r2, #1]
    dca0:	e3530036 	cmp	r3, #54	; 0x36
    dca4:	1a00002d 	bne	dd60 <vsprintf+0x470>
    dca8:	e1a03327 	lsr	r3, r7, #6
    dcac:	e2233001 	eor	r3, r3, #1	; 0x1
    dcb0:	e1a0500b 	mov	r5, fp
    dcb4:	e3a04000 	mov	r4, #0	; 0x0
    dcb8:	e203b001 	and	fp, r3, #1	; 0x1
    dcbc:	e28dc033 	add	ip, sp, #51	; 0x33
    dcc0:	e5d52000 	ldrb	r2, [r5]
    dcc4:	e59fe308 	ldr	lr, [pc, #776]	; dfd4 <vsprintf+0x6e4>
    dcc8:	e202300f 	and	r3, r2, #15	; 0xf
    dccc:	e7de0003 	ldrb	r0, [lr, r3]
    dcd0:	e7de3222 	ldrb	r3, [lr, r2, lsr #4]
    dcd4:	e1a0100c 	mov	r1, ip
    dcd8:	e4c13001 	strb	r3, [r1], #1
    dcdc:	e5cc0001 	strb	r0, [ip, #1]
    dce0:	e5d52001 	ldrb	r2, [r5, #1]
    dce4:	e59f02e8 	ldr	r0, [pc, #744]	; dfd4 <vsprintf+0x6e4>
    dce8:	e202300f 	and	r3, r2, #15	; 0xf
    dcec:	e7d0c003 	ldrb	ip, [r0, r3]
    dcf0:	e59f32dc 	ldr	r3, [pc, #732]	; dfd4 <vsprintf+0x6e4>
    dcf4:	e3540007 	cmp	r4, #7	; 0x7
    dcf8:	03a0e000 	moveq	lr, #0	; 0x0
    dcfc:	120be001 	andne	lr, fp, #1	; 0x1
    dd00:	e7d32222 	ldrb	r2, [r3, r2, lsr #4]
    dd04:	e35e0000 	cmp	lr, #0	; 0x0
    dd08:	e58d2014 	str	r2, [sp, #20]
    dd0c:	e2810001 	add	r0, r1, #1	; 0x1
    dd10:	e59de014 	ldr	lr, [sp, #20]
    dd14:	e2802001 	add	r2, r0, #1	; 0x1
    dd18:	e5c1e001 	strb	lr, [r1, #1]
    dd1c:	e2844001 	add	r4, r4, #1	; 0x1
    dd20:	e5c0c001 	strb	ip, [r0, #1]
    dd24:	13a0003a 	movne	r0, #58	; 0x3a
    dd28:	e282c001 	add	ip, r2, #1	; 0x1
    dd2c:	128cc001 	addne	ip, ip, #1	; 0x1
    dd30:	15c20001 	strbne	r0, [r2, #1]
    dd34:	e3540008 	cmp	r4, #8	; 0x8
    dd38:	e2855002 	add	r5, r5, #2	; 0x2
    dd3c:	1affffdf 	bne	dcc0 <vsprintf+0x3d0>
    dd40:	e3a03000 	mov	r3, #0	; 0x0
    dd44:	e5cc3000 	strb	r3, [ip]
    dd48:	e1a0000a 	mov	r0, sl
    dd4c:	e3c7c040 	bic	ip, r7, #64	; 0x40
    dd50:	e1a02009 	mov	r2, r9
    dd54:	e59d302c 	ldr	r3, [sp, #44]
    dd58:	e28d1033 	add	r1, sp, #51	; 0x33
    dd5c:	ea000021 	b	dde8 <vsprintf+0x4f8>
    dd60:	e3530034 	cmp	r3, #52	; 0x34
    dd64:	13c77040 	bicne	r7, r7, #64	; 0x40
    dd68:	1a000021 	bne	ddf4 <vsprintf+0x504>
    dd6c:	e3a05000 	mov	r5, #0	; 0x0
    dd70:	e28d406d 	add	r4, sp, #109	; 0x6d
    dd74:	e7db1005 	ldrb	r1, [fp, r5]
    dd78:	e28d007d 	add	r0, sp, #125	; 0x7d
    dd7c:	ebfffd9b 	bl	d3f0 <put_dec_trunc>
    dd80:	e59d101c 	ldr	r1, [sp, #28]
    dd84:	e2402001 	sub	r2, r0, #1	; 0x1
    dd88:	e0610000 	rsb	r0, r1, r0
    dd8c:	e3a01000 	mov	r1, #0	; 0x0
    dd90:	ea000002 	b	dda0 <vsprintf+0x4b0>
    dd94:	e5d23001 	ldrb	r3, [r2, #1]
    dd98:	e7c43001 	strb	r3, [r4, r1]
    dd9c:	e2811001 	add	r1, r1, #1	; 0x1
    dda0:	e59d3018 	ldr	r3, [sp, #24]
    dda4:	e2422001 	sub	r2, r2, #1	; 0x1
    dda8:	e1520003 	cmp	r2, r3
    ddac:	1afffff8 	bne	dd94 <vsprintf+0x4a4>
    ddb0:	e3550003 	cmp	r5, #3	; 0x3
    ddb4:	13a0e02e 	movne	lr, #46	; 0x2e
    ddb8:	e0844000 	add	r4, r4, r0
    ddbc:	e2855001 	add	r5, r5, #1	; 0x1
    ddc0:	14c4e001 	strbne	lr, [r4], #1
    ddc4:	e3550004 	cmp	r5, #4	; 0x4
    ddc8:	1affffe9 	bne	dd74 <vsprintf+0x484>
    ddcc:	e3a03000 	mov	r3, #0	; 0x0
    ddd0:	e5c43000 	strb	r3, [r4]
    ddd4:	e59d302c 	ldr	r3, [sp, #44]
    ddd8:	e3c7c040 	bic	ip, r7, #64	; 0x40
    dddc:	e1a0000a 	mov	r0, sl
    dde0:	e1a02009 	mov	r2, r9
    dde4:	e28d106d 	add	r1, sp, #109	; 0x6d
    dde8:	e58dc000 	str	ip, [sp]
    ddec:	ebfffdb2 	bl	d4bc <string>
    ddf0:	ea00000d 	b	de2c <vsprintf+0x53c>
    ddf4:	e3a01010 	mov	r1, #16	; 0x10
    ddf8:	e3790001 	cmn	r9, #1	; 0x1
    ddfc:	e58d1000 	str	r1, [sp]
    de00:	e59d102c 	ldr	r1, [sp, #44]
    de04:	e387c020 	orr	ip, r7, #32	; 0x20
    de08:	02899009 	addeq	r9, r9, #9	; 0x9
    de0c:	0387c021 	orreq	ip, r7, #33	; 0x21
    de10:	e1a0000a 	mov	r0, sl
    de14:	e1a0200b 	mov	r2, fp
    de18:	e3a03000 	mov	r3, #0	; 0x0
    de1c:	e58d9004 	str	r9, [sp, #4]
    de20:	e58d1008 	str	r1, [sp, #8]
    de24:	e58dc00c 	str	ip, [sp, #12]
    de28:	ebfffe1a 	bl	d698 <number>
    de2c:	e1a0a000 	mov	sl, r0
    de30:	e1a02008 	mov	r2, r8
    de34:	e5d23001 	ldrb	r3, [r2, #1]
    de38:	e59fe198 	ldr	lr, [pc, #408]	; dfd8 <vsprintf+0x6e8>
    de3c:	e1a08002 	mov	r8, r2
    de40:	e7de3003 	ldrb	r3, [lr, r3]
    de44:	e2822001 	add	r2, r2, #1	; 0x1
    de48:	e3130007 	tst	r3, #7	; 0x7
    de4c:	1afffff8 	bne	de34 <vsprintf+0x544>
    de50:	e2866004 	add	r6, r6, #4	; 0x4
    de54:	ea000054 	b	dfac <vsprintf+0x6bc>
    de58:	e59d0028 	ldr	r0, [sp, #40]
    de5c:	e5963000 	ldr	r3, [r6]
    de60:	e2861004 	add	r1, r6, #4	; 0x4
    de64:	e060200a 	rsb	r2, r0, sl
    de68:	e1a06001 	mov	r6, r1
    de6c:	e5832000 	str	r2, [r3]
    de70:	ea00004d 	b	dfac <vsprintf+0x6bc>
    de74:	e3a0c008 	mov	ip, #8	; 0x8
    de78:	ea00000d 	b	deb4 <vsprintf+0x5c4>
    de7c:	e3877020 	orr	r7, r7, #32	; 0x20
    de80:	e3a0c010 	mov	ip, #16	; 0x10
    de84:	ea00000a 	b	deb4 <vsprintf+0x5c4>
    de88:	e3877002 	orr	r7, r7, #2	; 0x2
    de8c:	ea000007 	b	deb0 <vsprintf+0x5c0>
    de90:	e3a03025 	mov	r3, #37	; 0x25
    de94:	e4ca3001 	strb	r3, [sl], #1
    de98:	e5d83000 	ldrb	r3, [r8]
    de9c:	e3530000 	cmp	r3, #0	; 0x0
    dea0:	02488001 	subeq	r8, r8, #1	; 0x1
    dea4:	0a000040 	beq	dfac <vsprintf+0x6bc>
    dea8:	e4ca3001 	strb	r3, [sl], #1
    deac:	ea00003e 	b	dfac <vsprintf+0x6bc>
    deb0:	e3a0c00a 	mov	ip, #10	; 0xa
    deb4:	e352004c 	cmp	r2, #76	; 0x4c
    deb8:	1a000009 	bne	dee4 <vsprintf+0x5f4>
    debc:	e59d1024 	ldr	r1, [sp, #36]
    dec0:	e3a02000 	mov	r2, #0	; 0x0
    dec4:	e0613006 	rsb	r3, r1, r6
    dec8:	e3130007 	tst	r3, #7	; 0x7
    decc:	02866004 	addeq	r6, r6, #4	; 0x4
    ded0:	e896000a 	ldm	r6, {r1, r3}
    ded4:	e1a05003 	mov	r5, r3
    ded8:	e1824001 	orr	r4, r2, r1
    dedc:	e2866008 	add	r6, r6, #8	; 0x8
    dee0:	ea000028 	b	df88 <vsprintf+0x698>
    dee4:	e352006c 	cmp	r2, #108	; 0x6c
    dee8:	1a000006 	bne	df08 <vsprintf+0x618>
    deec:	e5963000 	ldr	r3, [r6]
    def0:	e3170002 	tst	r7, #2	; 0x2
    def4:	e1a04003 	mov	r4, r3
    def8:	e3a05000 	mov	r5, #0	; 0x0
    defc:	e2866004 	add	r6, r6, #4	; 0x4
    df00:	0a000020 	beq	df88 <vsprintf+0x698>
    df04:	ea00001e 	b	df84 <vsprintf+0x694>
    df08:	e352007a 	cmp	r2, #122	; 0x7a
    df0c:	1352005a 	cmpne	r2, #90	; 0x5a
    df10:	05963000 	ldreq	r3, [r6]
    df14:	02866004 	addeq	r6, r6, #4	; 0x4
    df18:	01a04003 	moveq	r4, r3
    df1c:	03a05000 	moveq	r5, #0	; 0x0
    df20:	0a000018 	beq	df88 <vsprintf+0x698>
    df24:	e3520074 	cmp	r2, #116	; 0x74
    df28:	05963000 	ldreq	r3, [r6]
    df2c:	02866004 	addeq	r6, r6, #4	; 0x4
    df30:	0a00000b 	beq	df64 <vsprintf+0x674>
    df34:	e3520068 	cmp	r2, #104	; 0x68
    df38:	e2861004 	add	r1, r6, #4	; 0x4
    df3c:	e5962000 	ldr	r2, [r6]
    df40:	e2070002 	and	r0, r7, #2	; 0x2
    df44:	1a000009 	bne	df70 <vsprintf+0x680>
    df48:	e6ff3072 	uxth	r3, r2
    df4c:	e3500000 	cmp	r0, #0	; 0x0
    df50:	e1a06001 	mov	r6, r1
    df54:	e1a04003 	mov	r4, r3
    df58:	e3a05000 	mov	r5, #0	; 0x0
    df5c:	0a000009 	beq	df88 <vsprintf+0x698>
    df60:	e6bf3074 	sxth	r3, r4
    df64:	e1a04003 	mov	r4, r3
    df68:	e1a05fc4 	asr	r5, r4, #31
    df6c:	ea000005 	b	df88 <vsprintf+0x698>
    df70:	e3500000 	cmp	r0, #0	; 0x0
    df74:	e1a06001 	mov	r6, r1
    df78:	e1a04002 	mov	r4, r2
    df7c:	e3a05000 	mov	r5, #0	; 0x0
    df80:	0a000000 	beq	df88 <vsprintf+0x698>
    df84:	e1a05fc4 	asr	r5, r4, #31
    df88:	e59de02c 	ldr	lr, [sp, #44]
    df8c:	e1a0000a 	mov	r0, sl
    df90:	e1a02004 	mov	r2, r4
    df94:	e1a03005 	mov	r3, r5
    df98:	e58dc000 	str	ip, [sp]
    df9c:	e98d4200 	stmib	sp, {r9, lr}
    dfa0:	e58d700c 	str	r7, [sp, #12]
    dfa4:	ebfffdbb 	bl	d698 <number>
    dfa8:	e1a0a000 	mov	sl, r0
    dfac:	e2888001 	add	r8, r8, #1	; 0x1
    dfb0:	e5d83000 	ldrb	r3, [r8]
    dfb4:	e3530000 	cmp	r3, #0	; 0x0
    dfb8:	1afffe58 	bne	d920 <vsprintf+0x30>
    dfbc:	e5ca3000 	strb	r3, [sl]
    dfc0:	e59d1028 	ldr	r1, [sp, #40]
    dfc4:	e061000a 	rsb	r0, r1, sl
    dfc8:	e28dd084 	add	sp, sp, #132	; 0x84
    dfcc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    dfd0:	0001425f 	.word	0x0001425f
    dfd4:	0001193d 	.word	0x0001193d
    dfd8:	0001195e 	.word	0x0001195e

0000dfdc <sprintf>:
    dfdc:	e92d000e 	push	{r1, r2, r3}
    dfe0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    dfe4:	e59d1004 	ldr	r1, [sp, #4]
    dfe8:	e28d2008 	add	r2, sp, #8	; 0x8
    dfec:	ebfffe3f 	bl	d8f0 <vsprintf>
    dff0:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    dff4:	e28dd00c 	add	sp, sp, #12	; 0xc
    dff8:	e12fff1e 	bx	lr

0000dffc <ius_check_header>:
    dffc:	e92d4070 	push	{r4, r5, r6, lr}
    e000:	e5901000 	ldr	r1, [r0]
    e004:	e59f30e8 	ldr	r3, [pc, #232]	; e0f4 <ius_check_header+0xf8>
    e008:	e24dd008 	sub	sp, sp, #8	; 0x8
    e00c:	e1510003 	cmp	r1, r3
    e010:	e1a04000 	mov	r4, r0
    e014:	0a000005 	beq	e030 <ius_check_header+0x34>
    e018:	e59f00d8 	ldr	r0, [pc, #216]	; e0f8 <ius_check_header+0xfc>
    e01c:	e59f30d8 	ldr	r3, [pc, #216]	; e0fc <ius_check_header+0x100>
    e020:	e1a0e00f 	mov	lr, pc
    e024:	e593f000 	ldr	pc, [r3]
    e028:	e3e00000 	mvn	r0, #0	; 0x0
    e02c:	ea00002e 	b	e0ec <ius_check_header+0xf0>
    e030:	ebffe01a 	bl	60a0 <cdbg_verify_burn>
    e034:	e3500000 	cmp	r0, #0	; 0x0
    e038:	0a000011 	beq	e084 <ius_check_header+0x88>
    e03c:	e5943004 	ldr	r3, [r4, #4]
    e040:	e3130002 	tst	r3, #2	; 0x2
    e044:	1a000005 	bne	e060 <ius_check_header+0x64>
    e048:	e59f00b0 	ldr	r0, [pc, #176]	; e100 <ius_check_header+0x104>
    e04c:	e59f30a8 	ldr	r3, [pc, #168]	; e0fc <ius_check_header+0x100>
    e050:	e1a0e00f 	mov	lr, pc
    e054:	e593f000 	ldr	pc, [r3]
    e058:	e3e00021 	mvn	r0, #33	; 0x21
    e05c:	ea000022 	b	e0ec <ius_check_header+0xf0>
    e060:	ebffe012 	bl	60b0 <cdbg_verify_burn_key>
    e064:	e59f2098 	ldr	r2, [pc, #152]	; e104 <ius_check_header+0x108>
    e068:	e1a01000 	mov	r1, r0
    e06c:	e3a00000 	mov	r0, #0	; 0x0
    e070:	ebffdc9e 	bl	52f0 <ss_aes_init>
    e074:	e2840010 	add	r0, r4, #16	; 0x10
    e078:	e1a01000 	mov	r1, r0
    e07c:	e3a02e1f 	mov	r2, #496	; 0x1f0
    e080:	ebffdc89 	bl	52ac <ss_aes_data>
    e084:	e3a06000 	mov	r6, #0	; 0x0
    e088:	e5945018 	ldr	r5, [r4, #24]
    e08c:	e3a01c02 	mov	r1, #512	; 0x200
    e090:	e5846018 	str	r6, [r4, #24]
    e094:	e1a00006 	mov	r0, r6
    e098:	ebffddb2 	bl	5768 <hash_init>
    e09c:	e3a01c02 	mov	r1, #512	; 0x200
    e0a0:	e1a00004 	mov	r0, r4
    e0a4:	ebffdd8a 	bl	56d4 <hash_data>
    e0a8:	e28d0004 	add	r0, sp, #4	; 0x4
    e0ac:	ebffdd6c 	bl	5664 <hash_value>
    e0b0:	e59d2004 	ldr	r2, [sp, #4]
    e0b4:	e59f3040 	ldr	r3, [pc, #64]	; e0fc <ius_check_header+0x100>
    e0b8:	e1550002 	cmp	r5, r2
    e0bc:	e5845018 	str	r5, [r4, #24]
    e0c0:	0a000005 	beq	e0dc <ius_check_header+0xe0>
    e0c4:	e59f003c 	ldr	r0, [pc, #60]	; e108 <ius_check_header+0x10c>
    e0c8:	e1a01005 	mov	r1, r5
    e0cc:	e1a0e00f 	mov	lr, pc
    e0d0:	e593f000 	ldr	pc, [r3]
    e0d4:	e3e0001f 	mvn	r0, #31	; 0x1f
    e0d8:	ea000003 	b	e0ec <ius_check_header+0xf0>
    e0dc:	e59f0028 	ldr	r0, [pc, #40]	; e10c <ius_check_header+0x110>
    e0e0:	e1a0e00f 	mov	lr, pc
    e0e4:	e593f000 	ldr	pc, [r3]
    e0e8:	e1a00006 	mov	r0, r6
    e0ec:	e28dd008 	add	sp, sp, #8	; 0x8
    e0f0:	e8bd8070 	pop	{r4, r5, r6, pc}
    e0f4:	7375692b 	.word	0x7375692b
    e0f8:	00014266 	.word	0x00014266
    e0fc:	0800d764 	.word	0x0800d764
    e100:	00014280 	.word	0x00014280
    e104:	0800d108 	.word	0x0800d108
    e108:	0001428f 	.word	0x0001428f
    e10c:	000142b7 	.word	0x000142b7

0000e110 <printf_off>:
    e110:	e92d000f 	push	{r0, r1, r2, r3}
    e114:	e28dd010 	add	sp, sp, #16	; 0x10
    e118:	e12fff1e 	bx	lr

0000e11c <disable_ctrlc>:
    e11c:	e59f300c 	ldr	r3, [pc, #12]	; e130 <disable_ctrlc+0x14>
    e120:	e5932000 	ldr	r2, [r3]
    e124:	e5830000 	str	r0, [r3]
    e128:	e1a00002 	mov	r0, r2
    e12c:	e12fff1e 	bx	lr
    e130:	0800d304 	.word	0x0800d304

0000e134 <had_ctrlc>:
    e134:	e59f3004 	ldr	r3, [pc, #4]	; e140 <had_ctrlc+0xc>
    e138:	e5930000 	ldr	r0, [r3]
    e13c:	e12fff1e 	bx	lr
    e140:	0800d300 	.word	0x0800d300

0000e144 <clear_ctrlc>:
    e144:	e59f3008 	ldr	r3, [pc, #8]	; e154 <clear_ctrlc+0x10>
    e148:	e3a02000 	mov	r2, #0	; 0x0
    e14c:	e5832000 	str	r2, [r3]
    e150:	e12fff1e 	bx	lr
    e154:	0800d300 	.word	0x0800d300

0000e158 <dbg>:
    e158:	e92d000f 	push	{r0, r1, r2, r3}
    e15c:	e28dd010 	add	sp, sp, #16	; 0x10
    e160:	e12fff1e 	bx	lr

0000e164 <puts>:
    e164:	eaffe98c 	b	879c <serial_puts>

0000e168 <vprintf>:
    e168:	e92d4010 	push	{r4, lr}
    e16c:	e24ddd06 	sub	sp, sp, #384	; 0x180
    e170:	e1a02001 	mov	r2, r1
    e174:	e1a01000 	mov	r1, r0
    e178:	e1a0000d 	mov	r0, sp
    e17c:	ebfffddb 	bl	d8f0 <vsprintf>
    e180:	e1a0000d 	mov	r0, sp
    e184:	e1a0400d 	mov	r4, sp
    e188:	ebffe983 	bl	879c <serial_puts>
    e18c:	e28ddd06 	add	sp, sp, #384	; 0x180
    e190:	e8bd8010 	pop	{r4, pc}

0000e194 <printf_on>:
    e194:	e92d000f 	push	{r0, r1, r2, r3}
    e198:	e92d4010 	push	{r4, lr}
    e19c:	e24ddd06 	sub	sp, sp, #384	; 0x180
    e1a0:	e1a0000d 	mov	r0, sp
    e1a4:	e59d1188 	ldr	r1, [sp, #392]
    e1a8:	e28d2f63 	add	r2, sp, #396	; 0x18c
    e1ac:	ebfffdcf 	bl	d8f0 <vsprintf>
    e1b0:	e1a0000d 	mov	r0, sp
    e1b4:	e1a0400d 	mov	r4, sp
    e1b8:	ebffe977 	bl	879c <serial_puts>
    e1bc:	e28ddd06 	add	sp, sp, #384	; 0x180
    e1c0:	e8bd4010 	pop	{r4, lr}
    e1c4:	e28dd010 	add	sp, sp, #16	; 0x10
    e1c8:	e12fff1e 	bx	lr

0000e1cc <serial_printf>:
    e1cc:	e92d000f 	push	{r0, r1, r2, r3}
    e1d0:	e92d4010 	push	{r4, lr}
    e1d4:	e24ddd06 	sub	sp, sp, #384	; 0x180
    e1d8:	e1a0000d 	mov	r0, sp
    e1dc:	e59d1188 	ldr	r1, [sp, #392]
    e1e0:	e28d2f63 	add	r2, sp, #396	; 0x18c
    e1e4:	ebfffdc1 	bl	d8f0 <vsprintf>
    e1e8:	e1a0000d 	mov	r0, sp
    e1ec:	e1a0400d 	mov	r4, sp
    e1f0:	ebffe969 	bl	879c <serial_puts>
    e1f4:	e28ddd06 	add	sp, sp, #384	; 0x180
    e1f8:	e8bd4010 	pop	{r4, lr}
    e1fc:	e28dd010 	add	sp, sp, #16	; 0x10
    e200:	e12fff1e 	bx	lr

0000e204 <putc>:
    e204:	eaffe94d 	b	8740 <serial_putc>

0000e208 <tstc>:
    e208:	eaffe945 	b	8724 <serial_tstc>

0000e20c <getc>:
    e20c:	eaffe973 	b	87e0 <serial_getc>

0000e210 <ctrlc>:
    e210:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    e214:	e59f3040 	ldr	r3, [pc, #64]	; e25c <ctrlc+0x4c>
    e218:	e24dd004 	sub	sp, sp, #4	; 0x4
    e21c:	e5933000 	ldr	r3, [r3]
    e220:	e3530000 	cmp	r3, #0	; 0x0
    e224:	1a000009 	bne	e250 <ctrlc+0x40>
    e228:	ebffe93d 	bl	8724 <serial_tstc>
    e22c:	e3500000 	cmp	r0, #0	; 0x0
    e230:	0a000006 	beq	e250 <ctrlc+0x40>
    e234:	ebffe969 	bl	87e0 <serial_getc>
    e238:	e3500003 	cmp	r0, #3	; 0x3
    e23c:	059f301c 	ldreq	r3, [pc, #28]	; e260 <ctrlc+0x50>
    e240:	03a02001 	moveq	r2, #1	; 0x1
    e244:	05832000 	streq	r2, [r3]
    e248:	01a00002 	moveq	r0, r2
    e24c:	0a000000 	beq	e254 <ctrlc+0x44>
    e250:	e3a00000 	mov	r0, #0	; 0x0
    e254:	e28dd004 	add	sp, sp, #4	; 0x4
    e258:	e8bd8000 	pop	{pc}
    e25c:	0800d304 	.word	0x0800d304
    e260:	0800d300 	.word	0x0800d300

0000e264 <parse_line>:
    e264:	e92d4010 	push	{r4, lr}
    e268:	e3a04000 	mov	r4, #0	; 0x0
    e26c:	e1a02004 	mov	r2, r4
    e270:	ea000000 	b	e278 <parse_line+0x14>
    e274:	e2800001 	add	r0, r0, #1	; 0x1
    e278:	e5d03000 	ldrb	r3, [r0]
    e27c:	e3530009 	cmp	r3, #9	; 0x9
    e280:	13530020 	cmpne	r3, #32	; 0x20
    e284:	0afffffa 	beq	e274 <parse_line+0x10>
    e288:	e3530000 	cmp	r3, #0	; 0x0
    e28c:	0a00000c 	beq	e2c4 <parse_line+0x60>
    e290:	e7810104 	str	r0, [r1, r4, lsl #2]
    e294:	e2844001 	add	r4, r4, #1	; 0x1
    e298:	ea000000 	b	e2a0 <parse_line+0x3c>
    e29c:	e2800001 	add	r0, r0, #1	; 0x1
    e2a0:	e5d03000 	ldrb	r3, [r0]
    e2a4:	e3530000 	cmp	r3, #0	; 0x0
    e2a8:	13530020 	cmpne	r3, #32	; 0x20
    e2ac:	0a000002 	beq	e2bc <parse_line+0x58>
    e2b0:	e3530009 	cmp	r3, #9	; 0x9
    e2b4:	1afffff8 	bne	e29c <parse_line+0x38>
    e2b8:	ea000003 	b	e2cc <parse_line+0x68>
    e2bc:	e3530000 	cmp	r3, #0	; 0x0
    e2c0:	1a000001 	bne	e2cc <parse_line+0x68>
    e2c4:	e7813104 	str	r3, [r1, r4, lsl #2]
    e2c8:	ea000007 	b	e2ec <parse_line+0x88>
    e2cc:	e3540010 	cmp	r4, #16	; 0x10
    e2d0:	e5c02000 	strb	r2, [r0]
    e2d4:	1affffe6 	bne	e274 <parse_line+0x10>
    e2d8:	e59f0014 	ldr	r0, [pc, #20]	; e2f4 <parse_line+0x90>
    e2dc:	e1a01004 	mov	r1, r4
    e2e0:	e59f3010 	ldr	r3, [pc, #16]	; e2f8 <parse_line+0x94>
    e2e4:	e1a0e00f 	mov	lr, pc
    e2e8:	e593f000 	ldr	pc, [r3]
    e2ec:	e1a00004 	mov	r0, r4
    e2f0:	e8bd8010 	pop	{r4, pc}
    e2f4:	000142cd 	.word	0x000142cd
    e2f8:	0800d764 	.word	0x0800d764

0000e2fc <run_command>:
    e2fc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    e300:	e1a05000 	mov	r5, r0
    e304:	e24ddf93 	sub	sp, sp, #588	; 0x24c
    e308:	e1a0a001 	mov	sl, r1
    e30c:	ebffff8c 	bl	e144 <clear_ctrlc>
    e310:	e3550000 	cmp	r5, #0	; 0x0
    e314:	0a0000ac 	beq	e5cc <run_command+0x2d0>
    e318:	e5d53000 	ldrb	r3, [r5]
    e31c:	e3530000 	cmp	r3, #0	; 0x0
    e320:	0a0000a9 	beq	e5cc <run_command+0x2d0>
    e324:	e1a00005 	mov	r0, r5
    e328:	ebffcc1a 	bl	1398 <strlen>
    e32c:	e35000ff 	cmp	r0, #255	; 0xff
    e330:	9a000002 	bls	e340 <run_command+0x44>
    e334:	e59f02a0 	ldr	r0, [pc, #672]	; e5dc <run_command+0x2e0>
    e338:	ebffff89 	bl	e164 <puts>
    e33c:	ea0000a2 	b	e5cc <run_command+0x2d0>
    e340:	e28d4f41 	add	r4, sp, #260	; 0x104
    e344:	e1a01005 	mov	r1, r5
    e348:	e1a00004 	mov	r0, r4
    e34c:	e3a06000 	mov	r6, #0	; 0x0
    e350:	ebffcbb9 	bl	123c <strcpy>
    e354:	e3a07001 	mov	r7, #1	; 0x1
    e358:	e1a0b006 	mov	fp, r6
    e35c:	e28d9f81 	add	r9, sp, #516	; 0x204
    e360:	ea000091 	b	e5ac <run_command+0x2b0>
    e364:	e3510027 	cmp	r1, #39	; 0x27
    e368:	1a000002 	bne	e378 <run_command+0x7c>
    e36c:	e5523001 	ldrb	r3, [r2, #-1]
    e370:	e353005c 	cmp	r3, #92	; 0x5c
    e374:	12200001 	eorne	r0, r0, #1	; 0x1
    e378:	e3500000 	cmp	r0, #0	; 0x0
    e37c:	1a000009 	bne	e3a8 <run_command+0xac>
    e380:	e351003b 	cmp	r1, #59	; 0x3b
    e384:	13a03000 	movne	r3, #0	; 0x0
    e388:	03a03001 	moveq	r3, #1	; 0x1
    e38c:	e1520004 	cmp	r2, r4
    e390:	03a03000 	moveq	r3, #0	; 0x0
    e394:	e3530000 	cmp	r3, #0	; 0x0
    e398:	0a000002 	beq	e3a8 <run_command+0xac>
    e39c:	e5523001 	ldrb	r3, [r2, #-1]
    e3a0:	e353005c 	cmp	r3, #92	; 0x5c
    e3a4:	1a000005 	bne	e3c0 <run_command+0xc4>
    e3a8:	e2822001 	add	r2, r2, #1	; 0x1
    e3ac:	e5d21000 	ldrb	r1, [r2]
    e3b0:	e3510000 	cmp	r1, #0	; 0x0
    e3b4:	1affffea 	bne	e364 <run_command+0x68>
    e3b8:	e1a08002 	mov	r8, r2
    e3bc:	ea000001 	b	e3c8 <run_command+0xcc>
    e3c0:	e5c20000 	strb	r0, [r2]
    e3c4:	e2828001 	add	r8, r2, #1	; 0x1
    e3c8:	e1a00004 	mov	r0, r4
    e3cc:	ebffcbf1 	bl	1398 <strlen>
    e3d0:	e3a0c000 	mov	ip, #0	; 0x0
    e3d4:	e3a0ec01 	mov	lr, #256	; 0x100
    e3d8:	e1a0100c 	mov	r1, ip
    e3dc:	e28d5004 	add	r5, sp, #4	; 0x4
    e3e0:	ea000049 	b	e50c <run_command+0x210>
    e3e4:	e35c0003 	cmp	ip, #3	; 0x3
    e3e8:	e2400001 	sub	r0, r0, #1	; 0x1
    e3ec:	e4d42001 	ldrb	r2, [r4], #1
    e3f0:	0a000035 	beq	e4cc <run_command+0x1d0>
    e3f4:	e352005c 	cmp	r2, #92	; 0x5c
    e3f8:	13a03000 	movne	r3, #0	; 0x0
    e3fc:	03a03001 	moveq	r3, #1	; 0x1
    e400:	e351005c 	cmp	r1, #92	; 0x5c
    e404:	03a03000 	moveq	r3, #0	; 0x0
    e408:	e3530000 	cmp	r3, #0	; 0x0
    e40c:	0a000004 	beq	e424 <run_command+0x128>
    e410:	e3500000 	cmp	r0, #0	; 0x0
    e414:	0a00003f 	beq	e518 <run_command+0x21c>
    e418:	e4d42001 	ldrb	r2, [r4], #1
    e41c:	e2400001 	sub	r0, r0, #1	; 0x1
    e420:	e3a0105c 	mov	r1, #92	; 0x5c
    e424:	e35c0003 	cmp	ip, #3	; 0x3
    e428:	979ff10c 	ldrls	pc, [pc, ip, lsl #2]
    e42c:	ea000035 	b	e508 <run_command+0x20c>
    e430:	0000e440 	.word	0x0000e440
    e434:	0000e484 	.word	0x0000e484
    e438:	0000e4bc 	.word	0x0000e4bc
    e43c:	0000e4cc 	.word	0x0000e4cc
    e440:	e251105c 	subs	r1, r1, #92	; 0x5c
    e444:	13a01001 	movne	r1, #1	; 0x1
    e448:	e3520027 	cmp	r2, #39	; 0x27
    e44c:	13a03000 	movne	r3, #0	; 0x0
    e450:	02013001 	andeq	r3, r1, #1	; 0x1
    e454:	e3530000 	cmp	r3, #0	; 0x0
    e458:	13a0c003 	movne	ip, #3	; 0x3
    e45c:	1a000022 	bne	e4ec <run_command+0x1f0>
    e460:	e3520024 	cmp	r2, #36	; 0x24
    e464:	13a03000 	movne	r3, #0	; 0x0
    e468:	02013001 	andeq	r3, r1, #1	; 0x1
    e46c:	e3530000 	cmp	r3, #0	; 0x0
    e470:	13a0c001 	movne	ip, #1	; 0x1
    e474:	13a02024 	movne	r2, #36	; 0x24
    e478:	04c52001 	strbeq	r2, [r5], #1
    e47c:	024ee001 	subeq	lr, lr, #1	; 0x1
    e480:	ea000020 	b	e508 <run_command+0x20c>
    e484:	e352007b 	cmp	r2, #123	; 0x7b
    e488:	13520028 	cmpne	r2, #40	; 0x28
    e48c:	13a03000 	movne	r3, #0	; 0x0
    e490:	03a03001 	moveq	r3, #1	; 0x1
    e494:	03a0c002 	moveq	ip, #2	; 0x2
    e498:	0a00001a 	beq	e508 <run_command+0x20c>
    e49c:	e3a01024 	mov	r1, #36	; 0x24
    e4a0:	e25ee001 	subs	lr, lr, #1	; 0x1
    e4a4:	e4c51001 	strb	r1, [r5], #1
    e4a8:	0a000015 	beq	e504 <run_command+0x208>
    e4ac:	e4c52001 	strb	r2, [r5], #1
    e4b0:	e24ee001 	sub	lr, lr, #1	; 0x1
    e4b4:	e1a0c003 	mov	ip, r3
    e4b8:	ea000012 	b	e508 <run_command+0x20c>
    e4bc:	e352007d 	cmp	r2, #125	; 0x7d
    e4c0:	13520029 	cmpne	r2, #41	; 0x29
    e4c4:	0a00000e 	beq	e504 <run_command+0x208>
    e4c8:	ea00000e 	b	e508 <run_command+0x20c>
    e4cc:	e3520027 	cmp	r2, #39	; 0x27
    e4d0:	13a03000 	movne	r3, #0	; 0x0
    e4d4:	03a03001 	moveq	r3, #1	; 0x1
    e4d8:	e351005c 	cmp	r1, #92	; 0x5c
    e4dc:	03a03000 	moveq	r3, #0	; 0x0
    e4e0:	e3530000 	cmp	r3, #0	; 0x0
    e4e4:	0a000002 	beq	e4f4 <run_command+0x1f8>
    e4e8:	e3a0c000 	mov	ip, #0	; 0x0
    e4ec:	e3a02027 	mov	r2, #39	; 0x27
    e4f0:	ea000004 	b	e508 <run_command+0x20c>
    e4f4:	e4c52001 	strb	r2, [r5], #1
    e4f8:	e24ee001 	sub	lr, lr, #1	; 0x1
    e4fc:	e3a0c003 	mov	ip, #3	; 0x3
    e500:	ea000000 	b	e508 <run_command+0x20c>
    e504:	e3a0c000 	mov	ip, #0	; 0x0
    e508:	e1a01002 	mov	r1, r2
    e50c:	e3500000 	cmp	r0, #0	; 0x0
    e510:	135e0000 	cmpne	lr, #0	; 0x0
    e514:	1affffb2 	bne	e3e4 <run_command+0xe8>
    e518:	e35e0000 	cmp	lr, #0	; 0x0
    e51c:	15c5b000 	strbne	fp, [r5]
    e520:	0545e001 	strbeq	lr, [r5, #-1]
    e524:	e28d0004 	add	r0, sp, #4	; 0x4
    e528:	e1a01009 	mov	r1, r9
    e52c:	ebffff4c 	bl	e264 <parse_line>
    e530:	e2505000 	subs	r5, r0, #0	; 0x0
    e534:	0a00000d 	beq	e570 <run_command+0x274>
    e538:	e59d0204 	ldr	r0, [sp, #516]
    e53c:	ebffd213 	bl	2d90 <find_cmd>
    e540:	e2504000 	subs	r4, r0, #0	; 0x0
    e544:	1a000005 	bne	e560 <run_command+0x264>
    e548:	e59f0090 	ldr	r0, [pc, #144]	; e5e0 <run_command+0x2e4>
    e54c:	e59d1204 	ldr	r1, [sp, #516]
    e550:	e59f308c 	ldr	r3, [pc, #140]	; e5e4 <run_command+0x2e8>
    e554:	e1a0e00f 	mov	lr, pc
    e558:	e593f000 	ldr	pc, [r3]
    e55c:	ea000003 	b	e570 <run_command+0x274>
    e560:	e5943004 	ldr	r3, [r4, #4]
    e564:	e1550003 	cmp	r5, r3
    e568:	da000002 	ble	e578 <run_command+0x27c>
    e56c:	ebffd1c1 	bl	2c78 <cmd_usage>
    e570:	e3e06000 	mvn	r6, #0	; 0x0
    e574:	ea00000b 	b	e5a8 <run_command+0x2ac>
    e578:	e1a02005 	mov	r2, r5
    e57c:	e1a0100a 	mov	r1, sl
    e580:	e1a03009 	mov	r3, r9
    e584:	e1a0e00f 	mov	lr, pc
    e588:	e594f00c 	ldr	pc, [r4, #12]
    e58c:	e3500000 	cmp	r0, #0	; 0x0
    e590:	13e06000 	mvnne	r6, #0	; 0x0
    e594:	e5944008 	ldr	r4, [r4, #8]
    e598:	ebfffee5 	bl	e134 <had_ctrlc>
    e59c:	e3500000 	cmp	r0, #0	; 0x0
    e5a0:	1a000009 	bne	e5cc <run_command+0x2d0>
    e5a4:	e0077004 	and	r7, r7, r4
    e5a8:	e1a04008 	mov	r4, r8
    e5ac:	e5d43000 	ldrb	r3, [r4]
    e5b0:	e3530000 	cmp	r3, #0	; 0x0
    e5b4:	11a02004 	movne	r2, r4
    e5b8:	13a00000 	movne	r0, #0	; 0x0
    e5bc:	1affff7a 	bne	e3ac <run_command+0xb0>
    e5c0:	e3560000 	cmp	r6, #0	; 0x0
    e5c4:	01a06007 	moveq	r6, r7
    e5c8:	ea000000 	b	e5d0 <run_command+0x2d4>
    e5cc:	e3e06000 	mvn	r6, #0	; 0x0
    e5d0:	e1a00006 	mov	r0, r6
    e5d4:	e28ddf93 	add	sp, sp, #588	; 0x24c
    e5d8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    e5dc:	000142ec 	.word	0x000142ec
    e5e0:	00014302 	.word	0x00014302
    e5e4:	0800d764 	.word	0x0800d764

0000e5e8 <cread_add_char>:
    e5e8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    e5ec:	e3510000 	cmp	r1, #0	; 0x0
    e5f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    e5f4:	e1a07000 	mov	r7, r0
    e5f8:	e1a04002 	mov	r4, r2
    e5fc:	e1a00003 	mov	r0, r3
    e600:	e59d6018 	ldr	r6, [sp, #24]
    e604:	e59dc01c 	ldr	ip, [sp, #28]
    e608:	1a000003 	bne	e61c <cread_add_char+0x34>
    e60c:	e5922000 	ldr	r2, [r2]
    e610:	e5933000 	ldr	r3, [r3]
    e614:	e1520003 	cmp	r2, r3
    e618:	1a000026 	bne	e6b8 <cread_add_char+0xd0>
    e61c:	e5902000 	ldr	r2, [r0]
    e620:	e24c3001 	sub	r3, ip, #1	; 0x1
    e624:	e1520003 	cmp	r2, r3
    e628:	9a000003 	bls	e63c <cread_add_char+0x54>
    e62c:	e3a00007 	mov	r0, #7	; 0x7
    e630:	e28dd004 	add	sp, sp, #4	; 0x4
    e634:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
    e638:	eafffef1 	b	e204 <putc>
    e63c:	e3510000 	cmp	r1, #0	; 0x0
    e640:	e2821001 	add	r1, r2, #1	; 0x1
    e644:	e5801000 	str	r1, [r0]
    e648:	0a00001a 	beq	e6b8 <cread_add_char+0xd0>
    e64c:	e5943000 	ldr	r3, [r4]
    e650:	e0635001 	rsb	r5, r3, r1
    e654:	e3550001 	cmp	r5, #1	; 0x1
    e658:	9a000004 	bls	e670 <cread_add_char+0x88>
    e65c:	e2830001 	add	r0, r3, #1	; 0x1
    e660:	e0860000 	add	r0, r6, r0
    e664:	e0861003 	add	r1, r6, r3
    e668:	e2452001 	sub	r2, r5, #1	; 0x1
    e66c:	ebffcb6c 	bl	1424 <memmove>
    e670:	e5943000 	ldr	r3, [r4]
    e674:	e59f0074 	ldr	r0, [pc, #116]	; e6f0 <cread_add_char+0x108>
    e678:	e7c67003 	strb	r7, [r6, r3]
    e67c:	e5942000 	ldr	r2, [r4]
    e680:	e59f306c 	ldr	r3, [pc, #108]	; e6f4 <cread_add_char+0x10c>
    e684:	e0862002 	add	r2, r6, r2
    e688:	e1a01005 	mov	r1, r5
    e68c:	e1a0e00f 	mov	lr, pc
    e690:	e593f000 	ldr	pc, [r3]
    e694:	e5943000 	ldr	r3, [r4]
    e698:	e2833001 	add	r3, r3, #1	; 0x1
    e69c:	e5843000 	str	r3, [r4]
    e6a0:	ea000000 	b	e6a8 <cread_add_char+0xc0>
    e6a4:	ebfffed6 	bl	e204 <putc>
    e6a8:	e2555001 	subs	r5, r5, #1	; 0x1
    e6ac:	e3a00008 	mov	r0, #8	; 0x8
    e6b0:	1afffffb 	bne	e6a4 <cread_add_char+0xbc>
    e6b4:	ea00000b 	b	e6e8 <cread_add_char+0x100>
    e6b8:	e5943000 	ldr	r3, [r4]
    e6bc:	e59f002c 	ldr	r0, [pc, #44]	; e6f0 <cread_add_char+0x108>
    e6c0:	e7c67003 	strb	r7, [r6, r3]
    e6c4:	e5942000 	ldr	r2, [r4]
    e6c8:	e59f3024 	ldr	r3, [pc, #36]	; e6f4 <cread_add_char+0x10c>
    e6cc:	e0862002 	add	r2, r6, r2
    e6d0:	e3a01001 	mov	r1, #1	; 0x1
    e6d4:	e1a0e00f 	mov	lr, pc
    e6d8:	e593f000 	ldr	pc, [r3]
    e6dc:	e5943000 	ldr	r3, [r4]
    e6e0:	e2833001 	add	r3, r3, #1	; 0x1
    e6e4:	e5843000 	str	r3, [r4]
    e6e8:	e28dd004 	add	sp, sp, #4	; 0x4
    e6ec:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    e6f0:	00014325 	.word	0x00014325
    e6f4:	0800d764 	.word	0x0800d764

0000e6f8 <readline_into_buffer>:
    e6f8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    e6fc:	e59f367c 	ldr	r3, [pc, #1660]	; ed80 <readline_into_buffer+0x688>
    e700:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    e704:	e593e000 	ldr	lr, [r3]
    e708:	e1a06001 	mov	r6, r1
    e70c:	e35e0000 	cmp	lr, #0	; 0x0
    e710:	1a000014 	bne	e768 <readline_into_buffer+0x70>
    e714:	e59f3668 	ldr	r3, [pc, #1640]	; ed84 <readline_into_buffer+0x68c>
    e718:	e3e02000 	mvn	r2, #0	; 0x0
    e71c:	e5832000 	str	r2, [r3]
    e720:	e59f3660 	ldr	r3, [pc, #1632]	; ed88 <readline_into_buffer+0x690>
    e724:	e59fc660 	ldr	ip, [pc, #1632]	; ed8c <readline_into_buffer+0x694>
    e728:	e583e000 	str	lr, [r3]
    e72c:	e59f365c 	ldr	r3, [pc, #1628]	; ed90 <readline_into_buffer+0x698>
    e730:	e59f465c 	ldr	r4, [pc, #1628]	; ed94 <readline_into_buffer+0x69c>
    e734:	e583e000 	str	lr, [r3]
    e738:	e59f3658 	ldr	r3, [pc, #1624]	; ed98 <readline_into_buffer+0x6a0>
    e73c:	e1a0100e 	mov	r1, lr
    e740:	e583e000 	str	lr, [r3]
    e744:	e1a0300e 	mov	r3, lr
    e748:	e784c001 	str	ip, [r4, r1]
    e74c:	e2811004 	add	r1, r1, #4	; 0x4
    e750:	e3510010 	cmp	r1, #16	; 0x10
    e754:	e4cc3100 	strb	r3, [ip], #256
    e758:	1afffffa 	bne	e748 <readline_into_buffer+0x50>
    e75c:	e59f361c 	ldr	r3, [pc, #1564]	; ed80 <readline_into_buffer+0x688>
    e760:	e3a02001 	mov	r2, #1	; 0x1
    e764:	e5832000 	str	r2, [r3]
    e768:	ebfffe7d 	bl	e164 <puts>
    e76c:	e3a03000 	mov	r3, #0	; 0x0
    e770:	e58d3014 	str	r3, [sp, #20]
    e774:	e58d3010 	str	r3, [sp, #16]
    e778:	e1a05003 	mov	r5, r3
    e77c:	e3a09001 	mov	r9, #1	; 0x1
    e780:	e59fb614 	ldr	fp, [pc, #1556]	; ed9c <readline_into_buffer+0x6a4>
    e784:	e59fa5f8 	ldr	sl, [pc, #1528]	; ed84 <readline_into_buffer+0x68c>
    e788:	ea000000 	b	e790 <readline_into_buffer+0x98>
    e78c:	e3a05000 	mov	r5, #0	; 0x0
    e790:	ebfffe9d 	bl	e20c <getc>
    e794:	e6ef0070 	uxtb	r0, r0
    e798:	e350000d 	cmp	r0, #13	; 0xd
    e79c:	1350000a 	cmpne	r0, #10	; 0xa
    e7a0:	13a04000 	movne	r4, #0	; 0x0
    e7a4:	03a04001 	moveq	r4, #1	; 0x1
    e7a8:	1a00000b 	bne	e7dc <readline_into_buffer+0xe4>
    e7ac:	e59f35e8 	ldr	r3, [pc, #1512]	; ed9c <readline_into_buffer+0x6a4>
    e7b0:	e59f05e8 	ldr	r0, [pc, #1512]	; eda0 <readline_into_buffer+0x6a8>
    e7b4:	e1a0e00f 	mov	lr, pc
    e7b8:	e593f000 	ldr	pc, [r3]
    e7bc:	e59d5010 	ldr	r5, [sp, #16]
    e7c0:	e3a03000 	mov	r3, #0	; 0x0
    e7c4:	e7c63005 	strb	r3, [r6, r5]
    e7c8:	e5d63000 	ldrb	r3, [r6]
    e7cc:	e3530000 	cmp	r3, #0	; 0x0
    e7d0:	13530021 	cmpne	r3, #33	; 0x21
    e7d4:	0a000162 	beq	ed64 <readline_into_buffer+0x66c>
    e7d8:	ea00014e 	b	ed18 <readline_into_buffer+0x620>
    e7dc:	e3550000 	cmp	r5, #0	; 0x0
    e7e0:	0a000033 	beq	e8b4 <readline_into_buffer+0x1bc>
    e7e4:	e3550001 	cmp	r5, #1	; 0x1
    e7e8:	1a00000c 	bne	e820 <readline_into_buffer+0x128>
    e7ec:	e350005b 	cmp	r0, #91	; 0x5b
    e7f0:	05cd0009 	strbeq	r0, [sp, #9]
    e7f4:	03a05002 	moveq	r5, #2	; 0x2
    e7f8:	0affffe4 	beq	e790 <readline_into_buffer+0x98>
    e7fc:	e3a0cc01 	mov	ip, #256	; 0x100
    e800:	e5dd0008 	ldrb	r0, [sp, #8]
    e804:	e1a01009 	mov	r1, r9
    e808:	e28d2014 	add	r2, sp, #20	; 0x14
    e80c:	e28d3010 	add	r3, sp, #16	; 0x10
    e810:	e88d1040 	stm	sp, {r6, ip}
    e814:	e1a05004 	mov	r5, r4
    e818:	ebffff72 	bl	e5e8 <cread_add_char>
    e81c:	eaffffdb 	b	e790 <readline_into_buffer+0x98>
    e820:	e2403041 	sub	r3, r0, #65	; 0x41
    e824:	e3530007 	cmp	r3, #7	; 0x7
    e828:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    e82c:	ea000009 	b	e858 <readline_into_buffer+0x160>
    e830:	0000eb9c 	.word	0x0000eb9c
    e834:	0000ebdc 	.word	0x0000ebdc
    e838:	0000e8a4 	.word	0x0000e8a4
    e83c:	0000e9b0 	.word	0x0000e9b0
    e840:	0000e858 	.word	0x0000e858
    e844:	0000e858 	.word	0x0000e858
    e848:	0000e858 	.word	0x0000e858
    e84c:	0000e850 	.word	0x0000e850
    e850:	e3a05000 	mov	r5, #0	; 0x0
    e854:	ea000043 	b	e968 <readline_into_buffer+0x270>
    e858:	e28d1018 	add	r1, sp, #24	; 0x18
    e85c:	e0813005 	add	r3, r1, r5
    e860:	e5430010 	strb	r0, [r3, #-16]
    e864:	e2855001 	add	r5, r5, #1	; 0x1
    e868:	e3a04000 	mov	r4, #0	; 0x0
    e86c:	e28d8010 	add	r8, sp, #16	; 0x10
    e870:	e3a07c01 	mov	r7, #256	; 0x100
    e874:	ea000004 	b	e88c <readline_into_buffer+0x194>
    e878:	e28dc008 	add	ip, sp, #8	; 0x8
    e87c:	e7dc0004 	ldrb	r0, [ip, r4]
    e880:	e88d00c0 	stm	sp, {r6, r7}
    e884:	ebffff57 	bl	e5e8 <cread_add_char>
    e888:	e2844001 	add	r4, r4, #1	; 0x1
    e88c:	e1540005 	cmp	r4, r5
    e890:	e1a01009 	mov	r1, r9
    e894:	e28d2014 	add	r2, sp, #20	; 0x14
    e898:	e1a03008 	mov	r3, r8
    e89c:	1afffff5 	bne	e878 <readline_into_buffer+0x180>
    e8a0:	eaffffb9 	b	e78c <readline_into_buffer+0x94>
    e8a4:	e3a00006 	mov	r0, #6	; 0x6
    e8a8:	e350000b 	cmp	r0, #11	; 0xb
    e8ac:	e3a05000 	mov	r5, #0	; 0x0
    e8b0:	ea000001 	b	e8bc <readline_into_buffer+0x1c4>
    e8b4:	e350000b 	cmp	r0, #11	; 0xb
    e8b8:	0a00005f 	beq	ea3c <readline_into_buffer+0x344>
    e8bc:	8a00000d 	bhi	e8f8 <readline_into_buffer+0x200>
    e8c0:	e3500004 	cmp	r0, #4	; 0x4
    e8c4:	0a000042 	beq	e9d4 <readline_into_buffer+0x2dc>
    e8c8:	8a000005 	bhi	e8e4 <readline_into_buffer+0x1ec>
    e8cc:	e3500002 	cmp	r0, #2	; 0x2
    e8d0:	0a000037 	beq	e9b4 <readline_into_buffer+0x2bc>
    e8d4:	8a000028 	bhi	e97c <readline_into_buffer+0x284>
    e8d8:	e3500001 	cmp	r0, #1	; 0x1
    e8dc:	1a000106 	bne	ecfc <readline_into_buffer+0x604>
    e8e0:	ea000020 	b	e968 <readline_into_buffer+0x270>
    e8e4:	e3500006 	cmp	r0, #6	; 0x6
    e8e8:	0a000027 	beq	e98c <readline_into_buffer+0x294>
    e8ec:	3a000066 	bcc	ea8c <readline_into_buffer+0x394>
    e8f0:	e3500008 	cmp	r0, #8	; 0x8
    e8f4:	ea000015 	b	e950 <readline_into_buffer+0x258>
    e8f8:	e3500015 	cmp	r0, #21	; 0x15
    e8fc:	0a000070 	beq	eac4 <readline_into_buffer+0x3cc>
    e900:	8a000007 	bhi	e924 <readline_into_buffer+0x22c>
    e904:	e350000f 	cmp	r0, #15	; 0xf
    e908:	02299001 	eoreq	r9, r9, #1	; 0x1
    e90c:	0affff9f 	beq	e790 <readline_into_buffer+0x98>
    e910:	e3500010 	cmp	r0, #16	; 0x10
    e914:	0a0000a0 	beq	eb9c <readline_into_buffer+0x4a4>
    e918:	e350000e 	cmp	r0, #14	; 0xe
    e91c:	1a0000f6 	bne	ecfc <readline_into_buffer+0x604>
    e920:	ea0000ad 	b	ebdc <readline_into_buffer+0x4e4>
    e924:	e350001b 	cmp	r0, #27	; 0x1b
    e928:	05cd0008 	strbeq	r0, [sp, #8]
    e92c:	03a05001 	moveq	r5, #1	; 0x1
    e930:	0affff96 	beq	e790 <readline_into_buffer+0x98>
    e934:	8a000002 	bhi	e944 <readline_into_buffer+0x24c>
    e938:	e3500018 	cmp	r0, #24	; 0x18
    e93c:	1a0000ee 	bne	ecfc <readline_into_buffer+0x604>
    e940:	ea00005f 	b	eac4 <readline_into_buffer+0x3cc>
    e944:	e350007f 	cmp	r0, #127	; 0x7f
    e948:	0a000075 	beq	eb24 <readline_into_buffer+0x42c>
    e94c:	e35000ff 	cmp	r0, #255	; 0xff
    e950:	1a0000e9 	bne	ecfc <readline_into_buffer+0x604>
    e954:	ea000072 	b	eb24 <readline_into_buffer+0x42c>
    e958:	ebfffe29 	bl	e204 <putc>
    e95c:	e59d3014 	ldr	r3, [sp, #20]
    e960:	e2433001 	sub	r3, r3, #1	; 0x1
    e964:	e58d3014 	str	r3, [sp, #20]
    e968:	e59d3014 	ldr	r3, [sp, #20]
    e96c:	e3a00008 	mov	r0, #8	; 0x8
    e970:	e3530000 	cmp	r3, #0	; 0x0
    e974:	1afffff7 	bne	e958 <readline_into_buffer+0x260>
    e978:	eaffff84 	b	e790 <readline_into_buffer+0x98>
    e97c:	e3a03000 	mov	r3, #0	; 0x0
    e980:	e5c63000 	strb	r3, [r6]
    e984:	e3e00000 	mvn	r0, #0	; 0x0
    e988:	ea0000fa 	b	ed78 <readline_into_buffer+0x680>
    e98c:	e59d2014 	ldr	r2, [sp, #20]
    e990:	e59d3010 	ldr	r3, [sp, #16]
    e994:	e1520003 	cmp	r2, r3
    e998:	2affff7c 	bcs	e790 <readline_into_buffer+0x98>
    e99c:	e7d60002 	ldrb	r0, [r6, r2]
    e9a0:	ebfffe17 	bl	e204 <putc>
    e9a4:	e59d3014 	ldr	r3, [sp, #20]
    e9a8:	e2833001 	add	r3, r3, #1	; 0x1
    e9ac:	ea0000d0 	b	ecf4 <readline_into_buffer+0x5fc>
    e9b0:	e3a05000 	mov	r5, #0	; 0x0
    e9b4:	e59d3014 	ldr	r3, [sp, #20]
    e9b8:	e3530000 	cmp	r3, #0	; 0x0
    e9bc:	0affff73 	beq	e790 <readline_into_buffer+0x98>
    e9c0:	e3a00008 	mov	r0, #8	; 0x8
    e9c4:	ebfffe0e 	bl	e204 <putc>
    e9c8:	e59d3014 	ldr	r3, [sp, #20]
    e9cc:	e2433001 	sub	r3, r3, #1	; 0x1
    e9d0:	ea0000c7 	b	ecf4 <readline_into_buffer+0x5fc>
    e9d4:	e59d2014 	ldr	r2, [sp, #20]
    e9d8:	e59d3010 	ldr	r3, [sp, #16]
    e9dc:	e1520003 	cmp	r2, r3
    e9e0:	2affff6a 	bcs	e790 <readline_into_buffer+0x98>
    e9e4:	e2433001 	sub	r3, r3, #1	; 0x1
    e9e8:	e0534002 	subs	r4, r3, r2
    e9ec:	0a00000a 	beq	ea1c <readline_into_buffer+0x324>
    e9f0:	e2821001 	add	r1, r2, #1	; 0x1
    e9f4:	e0861001 	add	r1, r6, r1
    e9f8:	e0860002 	add	r0, r6, r2
    e9fc:	e1a02004 	mov	r2, r4
    ea00:	ebffca87 	bl	1424 <memmove>
    ea04:	e59d2014 	ldr	r2, [sp, #20]
    ea08:	e59f0394 	ldr	r0, [pc, #916]	; eda4 <readline_into_buffer+0x6ac>
    ea0c:	e0862002 	add	r2, r6, r2
    ea10:	e1a01004 	mov	r1, r4
    ea14:	e1a0e00f 	mov	lr, pc
    ea18:	e59bf000 	ldr	pc, [fp]
    ea1c:	e3a00020 	mov	r0, #32	; 0x20
    ea20:	ebfffdf7 	bl	e204 <putc>
    ea24:	e2444001 	sub	r4, r4, #1	; 0x1
    ea28:	e3a00008 	mov	r0, #8	; 0x8
    ea2c:	ebfffdf4 	bl	e204 <putc>
    ea30:	e3740001 	cmn	r4, #1	; 0x1
    ea34:	1afffffa 	bne	ea24 <readline_into_buffer+0x32c>
    ea38:	ea000053 	b	eb8c <readline_into_buffer+0x494>
    ea3c:	e59d2014 	ldr	r2, [sp, #20]
    ea40:	e59d3010 	ldr	r3, [sp, #16]
    ea44:	e1520003 	cmp	r2, r3
    ea48:	31a04002 	movcc	r4, r2
    ea4c:	3a000002 	bcc	ea5c <readline_into_buffer+0x364>
    ea50:	eaffff4e 	b	e790 <readline_into_buffer+0x98>
    ea54:	ebfffdea 	bl	e204 <putc>
    ea58:	e2844001 	add	r4, r4, #1	; 0x1
    ea5c:	e59d3010 	ldr	r3, [sp, #16]
    ea60:	e3a00020 	mov	r0, #32	; 0x20
    ea64:	e1540003 	cmp	r4, r3
    ea68:	3afffff9 	bcc	ea54 <readline_into_buffer+0x35c>
    ea6c:	ea000000 	b	ea74 <readline_into_buffer+0x37c>
    ea70:	ebfffde3 	bl	e204 <putc>
    ea74:	e59d2014 	ldr	r2, [sp, #20]
    ea78:	e3a00008 	mov	r0, #8	; 0x8
    ea7c:	e1540002 	cmp	r4, r2
    ea80:	e2444001 	sub	r4, r4, #1	; 0x1
    ea84:	8afffff9 	bhi	ea70 <readline_into_buffer+0x378>
    ea88:	ea000023 	b	eb1c <readline_into_buffer+0x424>
    ea8c:	e59d3014 	ldr	r3, [sp, #20]
    ea90:	e59d1010 	ldr	r1, [sp, #16]
    ea94:	e1530001 	cmp	r3, r1
    ea98:	2affff3c 	bcs	e790 <readline_into_buffer+0x98>
    ea9c:	e0631001 	rsb	r1, r3, r1
    eaa0:	e0862003 	add	r2, r6, r3
    eaa4:	e59f02f8 	ldr	r0, [pc, #760]	; eda4 <readline_into_buffer+0x6ac>
    eaa8:	e1a0e00f 	mov	lr, pc
    eaac:	e59bf000 	ldr	pc, [fp]
    eab0:	ea00008e 	b	ecf0 <readline_into_buffer+0x5f8>
    eab4:	ebfffdd2 	bl	e204 <putc>
    eab8:	e59d3014 	ldr	r3, [sp, #20]
    eabc:	e2433001 	sub	r3, r3, #1	; 0x1
    eac0:	e58d3014 	str	r3, [sp, #20]
    eac4:	e59d2014 	ldr	r2, [sp, #20]
    eac8:	e3a00008 	mov	r0, #8	; 0x8
    eacc:	e3520000 	cmp	r2, #0	; 0x0
    ead0:	1afffff7 	bne	eab4 <readline_into_buffer+0x3bc>
    ead4:	e59d3010 	ldr	r3, [sp, #16]
    ead8:	e3530000 	cmp	r3, #0	; 0x0
    eadc:	11a04002 	movne	r4, r2
    eae0:	1a000002 	bne	eaf0 <readline_into_buffer+0x3f8>
    eae4:	eaffff29 	b	e790 <readline_into_buffer+0x98>
    eae8:	ebfffdc5 	bl	e204 <putc>
    eaec:	e2844001 	add	r4, r4, #1	; 0x1
    eaf0:	e59d3010 	ldr	r3, [sp, #16]
    eaf4:	e3a00020 	mov	r0, #32	; 0x20
    eaf8:	e1540003 	cmp	r4, r3
    eafc:	3afffff9 	bcc	eae8 <readline_into_buffer+0x3f0>
    eb00:	ea000000 	b	eb08 <readline_into_buffer+0x410>
    eb04:	ebfffdbe 	bl	e204 <putc>
    eb08:	e59d2014 	ldr	r2, [sp, #20]
    eb0c:	e3a00008 	mov	r0, #8	; 0x8
    eb10:	e1540002 	cmp	r4, r2
    eb14:	e2444001 	sub	r4, r4, #1	; 0x1
    eb18:	8afffff9 	bhi	eb04 <readline_into_buffer+0x40c>
    eb1c:	e58d2010 	str	r2, [sp, #16]
    eb20:	eaffff1a 	b	e790 <readline_into_buffer+0x98>
    eb24:	e59d0014 	ldr	r0, [sp, #20]
    eb28:	e3500000 	cmp	r0, #0	; 0x0
    eb2c:	0affff17 	beq	e790 <readline_into_buffer+0x98>
    eb30:	e59d2010 	ldr	r2, [sp, #16]
    eb34:	e2403001 	sub	r3, r0, #1	; 0x1
    eb38:	e0604002 	rsb	r4, r0, r2
    eb3c:	e0861000 	add	r1, r6, r0
    eb40:	e1a02004 	mov	r2, r4
    eb44:	e0860003 	add	r0, r6, r3
    eb48:	e58d3014 	str	r3, [sp, #20]
    eb4c:	ebffca34 	bl	1424 <memmove>
    eb50:	e3a00008 	mov	r0, #8	; 0x8
    eb54:	ebfffdaa 	bl	e204 <putc>
    eb58:	e59d2014 	ldr	r2, [sp, #20]
    eb5c:	e59f0240 	ldr	r0, [pc, #576]	; eda4 <readline_into_buffer+0x6ac>
    eb60:	e0862002 	add	r2, r6, r2
    eb64:	e1a01004 	mov	r1, r4
    eb68:	e1a0e00f 	mov	lr, pc
    eb6c:	e59bf000 	ldr	pc, [fp]
    eb70:	e3a00020 	mov	r0, #32	; 0x20
    eb74:	ebfffda2 	bl	e204 <putc>
    eb78:	e2444001 	sub	r4, r4, #1	; 0x1
    eb7c:	e3a00008 	mov	r0, #8	; 0x8
    eb80:	ebfffd9f 	bl	e204 <putc>
    eb84:	e3740001 	cmn	r4, #1	; 0x1
    eb88:	1afffffa 	bne	eb78 <readline_into_buffer+0x480>
    eb8c:	e59d3010 	ldr	r3, [sp, #16]
    eb90:	e2433001 	sub	r3, r3, #1	; 0x1
    eb94:	e58d3010 	str	r3, [sp, #16]
    eb98:	eafffefc 	b	e790 <readline_into_buffer+0x98>
    eb9c:	e59a2000 	ldr	r2, [sl]
    eba0:	e3520000 	cmp	r2, #0	; 0x0
    eba4:	ba000024 	blt	ec3c <readline_into_buffer+0x544>
    eba8:	e2423001 	sub	r3, r2, #1	; 0x1
    ebac:	e3730001 	cmn	r3, #1	; 0x1
    ebb0:	059f01d0 	ldreq	r0, [pc, #464]	; ed88 <readline_into_buffer+0x690>
    ebb4:	e58a3000 	str	r3, [sl]
    ebb8:	05903000 	ldreq	r3, [r0]
    ebbc:	e59fc1cc 	ldr	ip, [pc, #460]	; ed90 <readline_into_buffer+0x698>
    ebc0:	058a3000 	streq	r3, [sl]
    ebc4:	e59a1000 	ldr	r1, [sl]
    ebc8:	e59c3000 	ldr	r3, [ip]
    ebcc:	e1510003 	cmp	r1, r3
    ebd0:	058a2000 	streq	r2, [sl]
    ebd4:	1a000014 	bne	ec2c <readline_into_buffer+0x534>
    ebd8:	ea000017 	b	ec3c <readline_into_buffer+0x544>
    ebdc:	e59a1000 	ldr	r1, [sl]
    ebe0:	e3510000 	cmp	r1, #0	; 0x0
    ebe4:	ba000014 	blt	ec3c <readline_into_buffer+0x544>
    ebe8:	e59f21a0 	ldr	r2, [pc, #416]	; ed90 <readline_into_buffer+0x698>
    ebec:	e5923000 	ldr	r3, [r2]
    ebf0:	e1510003 	cmp	r1, r3
    ebf4:	0a000010 	beq	ec3c <readline_into_buffer+0x544>
    ebf8:	e59f3188 	ldr	r3, [pc, #392]	; ed88 <readline_into_buffer+0x690>
    ebfc:	e59fc18c 	ldr	ip, [pc, #396]	; ed90 <readline_into_buffer+0x698>
    ec00:	e5932000 	ldr	r2, [r3]
    ec04:	e2813001 	add	r3, r1, #1	; 0x1
    ec08:	e1530002 	cmp	r3, r2
    ec0c:	e58a3000 	str	r3, [sl]
    ec10:	c3a03000 	movgt	r3, #0	; 0x0
    ec14:	c58a3000 	strgt	r3, [sl]
    ec18:	e59a1000 	ldr	r1, [sl]
    ec1c:	e59c3000 	ldr	r3, [ip]
    ec20:	e1510003 	cmp	r1, r3
    ec24:	059f517c 	ldreq	r5, [pc, #380]	; eda8 <readline_into_buffer+0x6b0>
    ec28:	0a00000a 	beq	ec58 <readline_into_buffer+0x560>
    ec2c:	e59f0160 	ldr	r0, [pc, #352]	; ed94 <readline_into_buffer+0x69c>
    ec30:	e7905101 	ldr	r5, [r0, r1, lsl #2]
    ec34:	e3550000 	cmp	r5, #0	; 0x0
    ec38:	1a000006 	bne	ec58 <readline_into_buffer+0x560>
    ec3c:	e3a00007 	mov	r0, #7	; 0x7
    ec40:	ebfffd6f 	bl	e204 <putc>
    ec44:	eafffed0 	b	e78c <readline_into_buffer+0x94>
    ec48:	ebfffd6d 	bl	e204 <putc>
    ec4c:	e59d3014 	ldr	r3, [sp, #20]
    ec50:	e2433001 	sub	r3, r3, #1	; 0x1
    ec54:	e58d3014 	str	r3, [sp, #20]
    ec58:	e59d2014 	ldr	r2, [sp, #20]
    ec5c:	e3a00008 	mov	r0, #8	; 0x8
    ec60:	e3520000 	cmp	r2, #0	; 0x0
    ec64:	1afffff7 	bne	ec48 <readline_into_buffer+0x550>
    ec68:	e59d3010 	ldr	r3, [sp, #16]
    ec6c:	e3530000 	cmp	r3, #0	; 0x0
    ec70:	11a04002 	movne	r4, r2
    ec74:	1a000002 	bne	ec84 <readline_into_buffer+0x58c>
    ec78:	ea00000d 	b	ecb4 <readline_into_buffer+0x5bc>
    ec7c:	ebfffd60 	bl	e204 <putc>
    ec80:	e2844001 	add	r4, r4, #1	; 0x1
    ec84:	e59d3010 	ldr	r3, [sp, #16]
    ec88:	e3a00020 	mov	r0, #32	; 0x20
    ec8c:	e1540003 	cmp	r4, r3
    ec90:	3afffff9 	bcc	ec7c <readline_into_buffer+0x584>
    ec94:	ea000000 	b	ec9c <readline_into_buffer+0x5a4>
    ec98:	ebfffd59 	bl	e204 <putc>
    ec9c:	e59d2014 	ldr	r2, [sp, #20]
    eca0:	e3a00008 	mov	r0, #8	; 0x8
    eca4:	e1540002 	cmp	r4, r2
    eca8:	e2444001 	sub	r4, r4, #1	; 0x1
    ecac:	8afffff9 	bhi	ec98 <readline_into_buffer+0x5a0>
    ecb0:	e58d2010 	str	r2, [sp, #16]
    ecb4:	e1a01005 	mov	r1, r5
    ecb8:	e1a00006 	mov	r0, r6
    ecbc:	ebffc95e 	bl	123c <strcpy>
    ecc0:	e1a00006 	mov	r0, r6
    ecc4:	ebffc9b3 	bl	1398 <strlen>
    ecc8:	e59d3014 	ldr	r3, [sp, #20]
    eccc:	e58d0010 	str	r0, [sp, #16]
    ecd0:	e1530000 	cmp	r3, r0
    ecd4:	2afffeac 	bcs	e78c <readline_into_buffer+0x94>
    ecd8:	e0631000 	rsb	r1, r3, r0
    ecdc:	e0862003 	add	r2, r6, r3
    ece0:	e59f00bc 	ldr	r0, [pc, #188]	; eda4 <readline_into_buffer+0x6ac>
    ece4:	e1a0e00f 	mov	lr, pc
    ece8:	e59bf000 	ldr	pc, [fp]
    ecec:	e3a05000 	mov	r5, #0	; 0x0
    ecf0:	e59d3010 	ldr	r3, [sp, #16]
    ecf4:	e58d3014 	str	r3, [sp, #20]
    ecf8:	eafffea4 	b	e790 <readline_into_buffer+0x98>
    ecfc:	e3a0cc01 	mov	ip, #256	; 0x100
    ed00:	e1a01009 	mov	r1, r9
    ed04:	e28d2014 	add	r2, sp, #20	; 0x14
    ed08:	e28d3010 	add	r3, sp, #16	; 0x10
    ed0c:	e88d1040 	stm	sp, {r6, ip}
    ed10:	ebfffe34 	bl	e5e8 <cread_add_char>
    ed14:	eafffe9d 	b	e790 <readline_into_buffer+0x98>
    ed18:	e59f4070 	ldr	r4, [pc, #112]	; ed90 <readline_into_buffer+0x698>
    ed1c:	e59f3070 	ldr	r3, [pc, #112]	; ed94 <readline_into_buffer+0x69c>
    ed20:	e5942000 	ldr	r2, [r4]
    ed24:	e1a01006 	mov	r1, r6
    ed28:	e7930102 	ldr	r0, [r3, r2, lsl #2]
    ed2c:	ebffc942 	bl	123c <strcpy>
    ed30:	e5943000 	ldr	r3, [r4]
    ed34:	e59f104c 	ldr	r1, [pc, #76]	; ed88 <readline_into_buffer+0x690>
    ed38:	e2833001 	add	r3, r3, #1	; 0x1
    ed3c:	e5912000 	ldr	r2, [r1]
    ed40:	e3530003 	cmp	r3, #3	; 0x3
    ed44:	c3a03000 	movgt	r3, #0	; 0x0
    ed48:	e1530002 	cmp	r3, r2
    ed4c:	e59f2044 	ldr	r2, [pc, #68]	; ed98 <readline_into_buffer+0x6a0>
    ed50:	e5843000 	str	r3, [r4]
    ed54:	c5813000 	strgt	r3, [r1]
    ed58:	e5923000 	ldr	r3, [r2]
    ed5c:	e2833001 	add	r3, r3, #1	; 0x1
    ed60:	e5823000 	str	r3, [r2]
    ed64:	e59f3024 	ldr	r3, [pc, #36]	; ed90 <readline_into_buffer+0x698>
    ed68:	e1a00005 	mov	r0, r5
    ed6c:	e5932000 	ldr	r2, [r3]
    ed70:	e59f300c 	ldr	r3, [pc, #12]	; ed84 <readline_into_buffer+0x68c>
    ed74:	e5832000 	str	r2, [r3]
    ed78:	e28dd01c 	add	sp, sp, #28	; 0x1c
    ed7c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ed80:	0800d30c 	.word	0x0800d30c
    ed84:	0800d198 	.word	0x0800d198
    ed88:	0800d310 	.word	0x0800d310
    ed8c:	0800d8f4 	.word	0x0800d8f4
    ed90:	0800d314 	.word	0x0800d314
    ed94:	0800d8e4 	.word	0x0800d8e4
    ed98:	0800d308 	.word	0x0800d308
    ed9c:	0800d764 	.word	0x0800d764
    eda0:	0001250e 	.word	0x0001250e
    eda4:	00014325 	.word	0x00014325
    eda8:	0001250f 	.word	0x0001250f

0000edac <readline>:
    edac:	e59f1000 	ldr	r1, [pc, #0]	; edb4 <readline+0x8>
    edb0:	eafffe50 	b	e6f8 <readline_into_buffer>
    edb4:	0800d7e4 	.word	0x0800d7e4

0000edb8 <main_loop>:
    edb8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    edbc:	e24dd004 	sub	sp, sp, #4	; 0x4
    edc0:	e59f0030 	ldr	r0, [pc, #48]	; edf8 <main_loop+0x40>
    edc4:	ebfffff8 	bl	edac <readline>
    edc8:	e1a03000 	mov	r3, r0
    edcc:	e3730001 	cmn	r3, #1	; 0x1
    edd0:	e3a01000 	mov	r1, #0	; 0x0
    edd4:	e59f0020 	ldr	r0, [pc, #32]	; edfc <main_loop+0x44>
    edd8:	1a000002 	bne	ede8 <main_loop+0x30>
    eddc:	e59f001c 	ldr	r0, [pc, #28]	; ee00 <main_loop+0x48>
    ede0:	ebfffcdf 	bl	e164 <puts>
    ede4:	eafffff5 	b	edc0 <main_loop+0x8>
    ede8:	e3530000 	cmp	r3, #0	; 0x0
    edec:	dafffff3 	ble	edc0 <main_loop+0x8>
    edf0:	ebfffd41 	bl	e2fc <run_command>
    edf4:	eafffff1 	b	edc0 <main_loop+0x8>
    edf8:	00014345 	.word	0x00014345
    edfc:	0800d7e4 	.word	0x0800d7e4
    ee00:	0001250e 	.word	0x0001250e

0000ee04 <parse_num>:
    ee04:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    ee08:	e1a08001 	mov	r8, r1
    ee0c:	e1a07002 	mov	r7, r2
    ee10:	e1a06003 	mov	r6, r3
    ee14:	ea000000 	b	ee1c <parse_num+0x18>
    ee18:	e2800001 	add	r0, r0, #1	; 0x1
    ee1c:	e5d03000 	ldrb	r3, [r0]
    ee20:	e3530020 	cmp	r3, #32	; 0x20
    ee24:	0afffffb 	beq	ee18 <parse_num+0x14>
    ee28:	ea000037 	b	ef0c <parse_num+0x108>
    ee2c:	e3520000 	cmp	r2, #0	; 0x0
    ee30:	0a00000a 	beq	ee60 <parse_num+0x5c>
    ee34:	e3530030 	cmp	r3, #48	; 0x30
    ee38:	1a000008 	bne	ee60 <parse_num+0x5c>
    ee3c:	e5d02001 	ldrb	r2, [r0, #1]
    ee40:	e2423041 	sub	r3, r2, #65	; 0x41
    ee44:	e6ef3073 	uxtb	r3, r3
    ee48:	e3530019 	cmp	r3, #25	; 0x19
    ee4c:	92823020 	addls	r3, r2, #32	; 0x20
    ee50:	96ef2073 	uxtbls	r2, r3
    ee54:	e3520078 	cmp	r2, #120	; 0x78
    ee58:	02800002 	addeq	r0, r0, #2	; 0x2
    ee5c:	03a04010 	moveq	r4, #16	; 0x10
    ee60:	e4d0c001 	ldrb	ip, [r0], #1
    ee64:	e24c3030 	sub	r3, ip, #48	; 0x30
    ee68:	e24c2041 	sub	r2, ip, #65	; 0x41
    ee6c:	e6ef3073 	uxtb	r3, r3
    ee70:	e6ef2072 	uxtb	r2, r2
    ee74:	e3530009 	cmp	r3, #9	; 0x9
    ee78:	83a01000 	movhi	r1, #0	; 0x0
    ee7c:	93a01001 	movls	r1, #1	; 0x1
    ee80:	e3520005 	cmp	r2, #5	; 0x5
    ee84:	83a02000 	movhi	r2, #0	; 0x0
    ee88:	93a02001 	movls	r2, #1	; 0x1
    ee8c:	e1913002 	orrs	r3, r1, r2
    ee90:	1a000003 	bne	eea4 <parse_num+0xa0>
    ee94:	e24c3061 	sub	r3, ip, #97	; 0x61
    ee98:	e6ef3073 	uxtb	r3, r3
    ee9c:	e3530005 	cmp	r3, #5	; 0x5
    eea0:	8a00000f 	bhi	eee4 <parse_num+0xe0>
    eea4:	e3510000 	cmp	r1, #0	; 0x0
    eea8:	124c2030 	subne	r2, ip, #48	; 0x30
    eeac:	1a000007 	bne	eed0 <parse_num+0xcc>
    eeb0:	e24c3061 	sub	r3, ip, #97	; 0x61
    eeb4:	e6ef3073 	uxtb	r3, r3
    eeb8:	e3530005 	cmp	r3, #5	; 0x5
    eebc:	924c2057 	subls	r2, ip, #87	; 0x57
    eec0:	9a000002 	bls	eed0 <parse_num+0xcc>
    eec4:	e3520000 	cmp	r2, #0	; 0x0
    eec8:	0a000002 	beq	eed8 <parse_num+0xd4>
    eecc:	e24c2037 	sub	r2, ip, #55	; 0x37
    eed0:	e1520004 	cmp	r2, r4
    eed4:	aa000002 	bge	eee4 <parse_num+0xe0>
    eed8:	e0252495 	mla	r5, r5, r4, r2
    eedc:	e3a02000 	mov	r2, #0	; 0x0
    eee0:	ea00000c 	b	ef18 <parse_num+0x114>
    eee4:	e3560000 	cmp	r6, #0	; 0x0
    eee8:	0a000012 	beq	ef38 <parse_num+0x134>
    eeec:	e3a03000 	mov	r3, #0	; 0x0
    eef0:	e7d62003 	ldrb	r2, [r6, r3]
    eef4:	e2833001 	add	r3, r3, #1	; 0x1
    eef8:	e3520000 	cmp	r2, #0	; 0x0
    eefc:	0a00000d 	beq	ef38 <parse_num+0x134>
    ef00:	e15c0002 	cmp	ip, r2
    ef04:	1afffff9 	bne	eef0 <parse_num+0xec>
    ef08:	ea000005 	b	ef24 <parse_num+0x120>
    ef0c:	e3a02001 	mov	r2, #1	; 0x1
    ef10:	e3a0400a 	mov	r4, #10	; 0xa
    ef14:	e3a05000 	mov	r5, #0	; 0x0
    ef18:	e5d03000 	ldrb	r3, [r0]
    ef1c:	e3530000 	cmp	r3, #0	; 0x0
    ef20:	1affffc1 	bne	ee2c <parse_num+0x28>
    ef24:	e3570000 	cmp	r7, #0	; 0x0
    ef28:	15870000 	strne	r0, [r7]
    ef2c:	e5885000 	str	r5, [r8]
    ef30:	e3a00001 	mov	r0, #1	; 0x1
    ef34:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    ef38:	e3a00000 	mov	r0, #0	; 0x0
    ef3c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000ef40 <xyzModem_stream_close>:
    ef40:	e59f2044 	ldr	r2, [pc, #68]	; ef8c <xyzModem_stream_close+0x4c>
    ef44:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ef48:	e5920428 	ldr	r0, [r2, #1064]
    ef4c:	e59f303c 	ldr	r3, [pc, #60]	; ef90 <xyzModem_stream_close+0x50>
    ef50:	e592c424 	ldr	ip, [r2, #1060]
    ef54:	e592e418 	ldr	lr, [r2, #1048]
    ef58:	e3500000 	cmp	r0, #0	; 0x0
    ef5c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    ef60:	e59f102c 	ldr	r1, [pc, #44]	; ef94 <xyzModem_stream_close+0x54>
    ef64:	01a01003 	moveq	r1, r3
    ef68:	e59f3028 	ldr	r3, [pc, #40]	; ef98 <xyzModem_stream_close+0x58>
    ef6c:	e88d5000 	stm	sp, {ip, lr}
    ef70:	e593c000 	ldr	ip, [r3]
    ef74:	e59f0020 	ldr	r0, [pc, #32]	; ef9c <xyzModem_stream_close+0x5c>
    ef78:	e5923420 	ldr	r3, [r2, #1056]
    ef7c:	e592241c 	ldr	r2, [r2, #1052]
    ef80:	e12fff3c 	blx	ip
    ef84:	e28dd00c 	add	sp, sp, #12	; 0xc
    ef88:	e8bd8000 	pop	{pc}
    ef8c:	0800d328 	.word	0x0800d328
    ef90:	00014349 	.word	0x00014349
    ef94:	0001434f 	.word	0x0001434f
    ef98:	0800d764 	.word	0x0800d764
    ef9c:	00014353 	.word	0x00014353

0000efa0 <xyzModem_error>:
    efa0:	e2800008 	add	r0, r0, #8	; 0x8
    efa4:	e3500007 	cmp	r0, #7	; 0x7
    efa8:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    efac:	ea000017 	b	f010 <xyzModem_error+0x70>
    efb0:	0000f008 	.word	0x0000f008
    efb4:	0000f000 	.word	0x0000f000
    efb8:	0000eff8 	.word	0x0000eff8
    efbc:	0000eff0 	.word	0x0000eff0
    efc0:	0000efe8 	.word	0x0000efe8
    efc4:	0000efe0 	.word	0x0000efe0
    efc8:	0000efd0 	.word	0x0000efd0
    efcc:	0000efd8 	.word	0x0000efd8
    efd0:	e59f0040 	ldr	r0, [pc, #64]	; f018 <xyzModem_error+0x78>
    efd4:	e12fff1e 	bx	lr
    efd8:	e59f003c 	ldr	r0, [pc, #60]	; f01c <xyzModem_error+0x7c>
    efdc:	e12fff1e 	bx	lr
    efe0:	e59f0038 	ldr	r0, [pc, #56]	; f020 <xyzModem_error+0x80>
    efe4:	e12fff1e 	bx	lr
    efe8:	e59f0034 	ldr	r0, [pc, #52]	; f024 <xyzModem_error+0x84>
    efec:	e12fff1e 	bx	lr
    eff0:	e59f0030 	ldr	r0, [pc, #48]	; f028 <xyzModem_error+0x88>
    eff4:	e12fff1e 	bx	lr
    eff8:	e59f002c 	ldr	r0, [pc, #44]	; f02c <xyzModem_error+0x8c>
    effc:	e12fff1e 	bx	lr
    f000:	e59f0028 	ldr	r0, [pc, #40]	; f030 <xyzModem_error+0x90>
    f004:	e12fff1e 	bx	lr
    f008:	e59f0024 	ldr	r0, [pc, #36]	; f034 <xyzModem_error+0x94>
    f00c:	e12fff1e 	bx	lr
    f010:	e59f0020 	ldr	r0, [pc, #32]	; f038 <xyzModem_error+0x98>
    f014:	e12fff1e 	bx	lr
    f018:	00014394 	.word	0x00014394
    f01c:	000143b4 	.word	0x000143b4
    f020:	000143c6 	.word	0x000143c6
    f024:	000143d0 	.word	0x000143d0
    f028:	000143dc 	.word	0x000143dc
    f02c:	000143e6 	.word	0x000143e6
    f030:	000143f6 	.word	0x000143f6
    f034:	00014409 	.word	0x00014409
    f038:	0001441e 	.word	0x0001441e

0000f03c <delay>:
    f03c:	e12fff1e 	bx	lr

0000f040 <init_num>:
    f040:	e59f305c 	ldr	r3, [pc, #92]	; f0a4 <init_num+0x64>
    f044:	e3a01001 	mov	r1, #1	; 0x1
    f048:	e5c31000 	strb	r1, [r3]
    f04c:	e59f3054 	ldr	r3, [pc, #84]	; f0a8 <init_num+0x68>
    f050:	e3a02000 	mov	r2, #0	; 0x0
    f054:	e5c32000 	strb	r2, [r3]
    f058:	e59f304c 	ldr	r3, [pc, #76]	; f0ac <init_num+0x6c>
    f05c:	e5c32000 	strb	r2, [r3]
    f060:	e59f3048 	ldr	r3, [pc, #72]	; f0b0 <init_num+0x70>
    f064:	e5c32000 	strb	r2, [r3]
    f068:	e59f3044 	ldr	r3, [pc, #68]	; f0b4 <init_num+0x74>
    f06c:	e5c32000 	strb	r2, [r3]
    f070:	e59f3040 	ldr	r3, [pc, #64]	; f0b8 <init_num+0x78>
    f074:	e5832000 	str	r2, [r3]
    f078:	e59f303c 	ldr	r3, [pc, #60]	; f0bc <init_num+0x7c>
    f07c:	e5c32000 	strb	r2, [r3]
    f080:	e59f3038 	ldr	r3, [pc, #56]	; f0c0 <init_num+0x80>
    f084:	e5c32000 	strb	r2, [r3]
    f088:	e59f3034 	ldr	r3, [pc, #52]	; f0c4 <init_num+0x84>
    f08c:	e5c32000 	strb	r2, [r3]
    f090:	e59f3030 	ldr	r3, [pc, #48]	; f0c8 <init_num+0x88>
    f094:	e5c32000 	strb	r2, [r3]
    f098:	e59f302c 	ldr	r3, [pc, #44]	; f0cc <init_num+0x8c>
    f09c:	e5c32000 	strb	r2, [r3]
    f0a0:	e12fff1e 	bx	lr
    f0a4:	0800d19c 	.word	0x0800d19c
    f0a8:	0800d325 	.word	0x0800d325
    f0ac:	0800d318 	.word	0x0800d318
    f0b0:	0800d319 	.word	0x0800d319
    f0b4:	0800d31a 	.word	0x0800d31a
    f0b8:	0800d31c 	.word	0x0800d31c
    f0bc:	0800d320 	.word	0x0800d320
    f0c0:	0800d321 	.word	0x0800d321
    f0c4:	0800d322 	.word	0x0800d322
    f0c8:	0800d323 	.word	0x0800d323
    f0cc:	0800d324 	.word	0x0800d324

0000f0d0 <out_message>:
    f0d0:	e92d4010 	push	{r4, lr}
    f0d4:	e59f30c8 	ldr	r3, [pc, #200]	; f1a4 <out_message+0xd4>
    f0d8:	e59f40c8 	ldr	r4, [pc, #200]	; f1a8 <out_message+0xd8>
    f0dc:	e5d31000 	ldrb	r1, [r3]
    f0e0:	e59f00c4 	ldr	r0, [pc, #196]	; f1ac <out_message+0xdc>
    f0e4:	e1a0e00f 	mov	lr, pc
    f0e8:	e594f000 	ldr	pc, [r4]
    f0ec:	e59f30bc 	ldr	r3, [pc, #188]	; f1b0 <out_message+0xe0>
    f0f0:	e59f00bc 	ldr	r0, [pc, #188]	; f1b4 <out_message+0xe4>
    f0f4:	e5d31000 	ldrb	r1, [r3]
    f0f8:	e1a0e00f 	mov	lr, pc
    f0fc:	e594f000 	ldr	pc, [r4]
    f100:	e59f30b0 	ldr	r3, [pc, #176]	; f1b8 <out_message+0xe8>
    f104:	e59f00b0 	ldr	r0, [pc, #176]	; f1bc <out_message+0xec>
    f108:	e5931000 	ldr	r1, [r3]
    f10c:	e1a0e00f 	mov	lr, pc
    f110:	e594f000 	ldr	pc, [r4]
    f114:	e59f30a4 	ldr	r3, [pc, #164]	; f1c0 <out_message+0xf0>
    f118:	e59f00a4 	ldr	r0, [pc, #164]	; f1c4 <out_message+0xf4>
    f11c:	e5d31000 	ldrb	r1, [r3]
    f120:	e1a0e00f 	mov	lr, pc
    f124:	e594f000 	ldr	pc, [r4]
    f128:	e59f3098 	ldr	r3, [pc, #152]	; f1c8 <out_message+0xf8>
    f12c:	e59f0098 	ldr	r0, [pc, #152]	; f1cc <out_message+0xfc>
    f130:	e5d31000 	ldrb	r1, [r3]
    f134:	e1a0e00f 	mov	lr, pc
    f138:	e594f000 	ldr	pc, [r4]
    f13c:	e59f308c 	ldr	r3, [pc, #140]	; f1d0 <out_message+0x100>
    f140:	e59f008c 	ldr	r0, [pc, #140]	; f1d4 <out_message+0x104>
    f144:	e5d31000 	ldrb	r1, [r3]
    f148:	e1a0e00f 	mov	lr, pc
    f14c:	e594f000 	ldr	pc, [r4]
    f150:	e59f3080 	ldr	r3, [pc, #128]	; f1d8 <out_message+0x108>
    f154:	e59f0080 	ldr	r0, [pc, #128]	; f1dc <out_message+0x10c>
    f158:	e5d31000 	ldrb	r1, [r3]
    f15c:	e1a0e00f 	mov	lr, pc
    f160:	e594f000 	ldr	pc, [r4]
    f164:	e59f3074 	ldr	r3, [pc, #116]	; f1e0 <out_message+0x110>
    f168:	e59f0074 	ldr	r0, [pc, #116]	; f1e4 <out_message+0x114>
    f16c:	e5d31000 	ldrb	r1, [r3]
    f170:	e1a0e00f 	mov	lr, pc
    f174:	e594f000 	ldr	pc, [r4]
    f178:	e59f3068 	ldr	r3, [pc, #104]	; f1e8 <out_message+0x118>
    f17c:	e59f0068 	ldr	r0, [pc, #104]	; f1ec <out_message+0x11c>
    f180:	e5d31000 	ldrb	r1, [r3]
    f184:	e1a0e00f 	mov	lr, pc
    f188:	e594f000 	ldr	pc, [r4]
    f18c:	e59f305c 	ldr	r3, [pc, #92]	; f1f0 <out_message+0x120>
    f190:	e59f005c 	ldr	r0, [pc, #92]	; f1f4 <out_message+0x124>
    f194:	e5d31000 	ldrb	r1, [r3]
    f198:	e1a0e00f 	mov	lr, pc
    f19c:	e594f000 	ldr	pc, [r4]
    f1a0:	e8bd8010 	pop	{r4, pc}
    f1a4:	0800d318 	.word	0x0800d318
    f1a8:	0800d764 	.word	0x0800d764
    f1ac:	0001442c 	.word	0x0001442c
    f1b0:	0800d319 	.word	0x0800d319
    f1b4:	0001443a 	.word	0x0001443a
    f1b8:	0800d31c 	.word	0x0800d31c
    f1bc:	0001444d 	.word	0x0001444d
    f1c0:	0800d320 	.word	0x0800d320
    f1c4:	0001445f 	.word	0x0001445f
    f1c8:	0800d321 	.word	0x0800d321
    f1cc:	00014472 	.word	0x00014472
    f1d0:	0800d322 	.word	0x0800d322
    f1d4:	00014489 	.word	0x00014489
    f1d8:	0800d19c 	.word	0x0800d19c
    f1dc:	000144a1 	.word	0x000144a1
    f1e0:	0800d323 	.word	0x0800d323
    f1e4:	000144b0 	.word	0x000144b0
    f1e8:	0800d324 	.word	0x0800d324
    f1ec:	000144c9 	.word	0x000144c9
    f1f0:	0800d325 	.word	0x0800d325
    f1f4:	000144db 	.word	0x000144db

0000f1f8 <Procpacket>:
    f1f8:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    f1fc:	e59fc1ac 	ldr	ip, [pc, #428]	; f3b0 <Procpacket+0x1b8>
    f200:	e59f21ac 	ldr	r2, [pc, #428]	; f3b4 <Procpacket+0x1bc>
    f204:	e5dc3000 	ldrb	r3, [ip]
    f208:	e1a06000 	mov	r6, r0
    f20c:	e5c03000 	strb	r3, [r0]
    f210:	e5923000 	ldr	r3, [r2]
    f214:	e1a05001 	mov	r5, r1
    f218:	e5c03001 	strb	r3, [r0, #1]
    f21c:	e5923000 	ldr	r3, [r2]
    f220:	e1e03003 	mvn	r3, r3
    f224:	e5c03002 	strb	r3, [r0, #2]
    f228:	e59f3188 	ldr	r3, [pc, #392]	; f3b8 <Procpacket+0x1c0>
    f22c:	e5922000 	ldr	r2, [r2]
    f230:	e5933000 	ldr	r3, [r3]
    f234:	e1520003 	cmp	r2, r3
    f238:	1a000008 	bne	f260 <Procpacket+0x68>
    f23c:	e59f3178 	ldr	r3, [pc, #376]	; f3bc <Procpacket+0x1c4>
    f240:	e5d32000 	ldrb	r2, [r3]
    f244:	e59f3174 	ldr	r3, [pc, #372]	; f3c0 <Procpacket+0x1c8>
    f248:	e3520001 	cmp	r2, #1	; 0x1
    f24c:	e5934000 	ldr	r4, [r3]
    f250:	e5cc2000 	strb	r2, [ip]
    f254:	03a07080 	moveq	r7, #128	; 0x80
    f258:	13a07b01 	movne	r7, #1024	; 0x400
    f25c:	ea000004 	b	f274 <Procpacket+0x7c>
    f260:	e5dc3000 	ldrb	r3, [ip]
    f264:	e3530001 	cmp	r3, #1	; 0x1
    f268:	13a04b01 	movne	r4, #1024	; 0x400
    f26c:	03a04080 	moveq	r4, #128	; 0x80
    f270:	e1a07004 	mov	r7, r4
    f274:	e59f3148 	ldr	r3, [pc, #328]	; f3c4 <Procpacket+0x1cc>
    f278:	e5d33000 	ldrb	r3, [r3]
    f27c:	e3530000 	cmp	r3, #0	; 0x0
    f280:	1a000008 	bne	f2a8 <Procpacket+0xb0>
    f284:	e59f313c 	ldr	r3, [pc, #316]	; f3c8 <Procpacket+0x1d0>
    f288:	e2860003 	add	r0, r6, #3	; 0x3
    f28c:	e5931000 	ldr	r1, [r3]
    f290:	e59f3134 	ldr	r3, [pc, #308]	; f3cc <Procpacket+0x1d4>
    f294:	e1a02004 	mov	r2, r4
    f298:	e5933000 	ldr	r3, [r3]
    f29c:	e0631001 	rsb	r1, r3, r1
    f2a0:	e0851001 	add	r1, r5, r1
    f2a4:	ebffc855 	bl	1400 <memcpy>
    f2a8:	e59f111c 	ldr	r1, [pc, #284]	; f3cc <Procpacket+0x1d4>
    f2ac:	e59f311c 	ldr	r3, [pc, #284]	; f3d0 <Procpacket+0x1d8>
    f2b0:	e5912000 	ldr	r2, [r1]
    f2b4:	e5d33000 	ldrb	r3, [r3]
    f2b8:	e0642002 	rsb	r2, r4, r2
    f2bc:	e3530000 	cmp	r3, #0	; 0x0
    f2c0:	e5812000 	str	r2, [r1]
    f2c4:	0a000006 	beq	f2e4 <Procpacket+0xec>
    f2c8:	e1a02007 	mov	r2, r7
    f2cc:	e2860003 	add	r0, r6, #3	; 0x3
    f2d0:	e3a01000 	mov	r1, #0	; 0x0
    f2d4:	ebffc841 	bl	13e0 <memset>
    f2d8:	e59f30f4 	ldr	r3, [pc, #244]	; f3d4 <Procpacket+0x1dc>
    f2dc:	e3a02001 	mov	r2, #1	; 0x1
    f2e0:	e5c32000 	strb	r2, [r3]
    f2e4:	e59f30c8 	ldr	r3, [pc, #200]	; f3b4 <Procpacket+0x1bc>
    f2e8:	e5932000 	ldr	r2, [r3]
    f2ec:	e59f30c4 	ldr	r3, [pc, #196]	; f3b8 <Procpacket+0x1c0>
    f2f0:	e5933000 	ldr	r3, [r3]
    f2f4:	e1520003 	cmp	r2, r3
    f2f8:	1a000004 	bne	f310 <Procpacket+0x118>
    f2fc:	e2840003 	add	r0, r4, #3	; 0x3
    f300:	e0860000 	add	r0, r6, r0
    f304:	e0642007 	rsb	r2, r4, r7
    f308:	e3a0101a 	mov	r1, #26	; 0x1a
    f30c:	ebffc833 	bl	13e0 <memset>
    f310:	e3a00000 	mov	r0, #0	; 0x0
    f314:	e59f80bc 	ldr	r8, [pc, #188]	; f3d8 <Procpacket+0x1e0>
    f318:	e1a04006 	mov	r4, r6
    f31c:	e1a0c000 	mov	ip, r0
    f320:	e1a05000 	mov	r5, r0
    f324:	e1a0342c 	lsr	r3, ip, #8
    f328:	e5d42003 	ldrb	r2, [r4, #3]
    f32c:	e1a03083 	lsl	r3, r3, #1
    f330:	e19810b3 	ldrh	r1, [r8, r3]
    f334:	e2800001 	add	r0, r0, #1	; 0x1
    f338:	e182340c 	orr	r3, r2, ip, lsl #8
    f33c:	e0233001 	eor	r3, r3, r1
    f340:	e0852002 	add	r2, r5, r2
    f344:	e1500007 	cmp	r0, r7
    f348:	e6ffc073 	uxth	ip, r3
    f34c:	e6ef5072 	uxtb	r5, r2
    f350:	e2844001 	add	r4, r4, #1	; 0x1
    f354:	e59f107c 	ldr	r1, [pc, #124]	; f3d8 <Procpacket+0x1e0>
    f358:	3afffff1 	bcc	f324 <Procpacket+0x12c>
    f35c:	e59f3078 	ldr	r3, [pc, #120]	; f3dc <Procpacket+0x1e4>
    f360:	e0860007 	add	r0, r6, r7
    f364:	e5d33000 	ldrb	r3, [r3]
    f368:	e3530000 	cmp	r3, #0	; 0x0
    f36c:	05c05003 	strbeq	r5, [r0, #3]
    f370:	0a00000c 	beq	f3a8 <Procpacket+0x1b0>
    f374:	e1a0342c 	lsr	r3, ip, #8
    f378:	e1a03083 	lsl	r3, r3, #1
    f37c:	e19120b3 	ldrh	r2, [r1, r3]
    f380:	e022240c 	eor	r2, r2, ip, lsl #8
    f384:	e1a03442 	asr	r3, r2, #8
    f388:	e20330ff 	and	r3, r3, #255	; 0xff
    f38c:	e1a03083 	lsl	r3, r3, #1
    f390:	e19130b3 	ldrh	r3, [r1, r3]
    f394:	e0233402 	eor	r3, r3, r2, lsl #8
    f398:	e6ff3073 	uxth	r3, r3
    f39c:	e1a02423 	lsr	r2, r3, #8
    f3a0:	e5c03004 	strb	r3, [r0, #4]
    f3a4:	e5c02003 	strb	r2, [r0, #3]
    f3a8:	e3a00000 	mov	r0, #0	; 0x0
    f3ac:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    f3b0:	0800d31a 	.word	0x0800d31a
    f3b4:	0800d31c 	.word	0x0800d31c
    f3b8:	0800dcf4 	.word	0x0800dcf4
    f3bc:	0800dcfc 	.word	0x0800dcfc
    f3c0:	0800dcf8 	.word	0x0800dcf8
    f3c4:	0800d320 	.word	0x0800d320
    f3c8:	0800d7c4 	.word	0x0800d7c4
    f3cc:	0800d7c8 	.word	0x0800d7c8
    f3d0:	0800d325 	.word	0x0800d325
    f3d4:	0800d324 	.word	0x0800d324
    f3d8:	000116a0 	.word	0x000116a0
    f3dc:	0800d7c0 	.word	0x0800d7c0

0000f3e0 <procsend>:
    f3e0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    f3e4:	e59f31f8 	ldr	r3, [pc, #504]	; f5e4 <procsend+0x204>
    f3e8:	e1a06000 	mov	r6, r0
    f3ec:	e5d30000 	ldrb	r0, [r3]
    f3f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    f3f4:	e3500000 	cmp	r0, #0	; 0x0
    f3f8:	e1a0c002 	mov	ip, r2
    f3fc:	e59f41e4 	ldr	r4, [pc, #484]	; f5e8 <procsend+0x208>
    f400:	0a000065 	beq	f59c <procsend+0x1bc>
    f404:	e59f31e0 	ldr	r3, [pc, #480]	; f5ec <procsend+0x20c>
    f408:	e1a02b02 	lsl	r2, r2, #22
    f40c:	e1a02b22 	lsr	r2, r2, #22
    f410:	e583c000 	str	ip, [r3]
    f414:	e59f31d4 	ldr	r3, [pc, #468]	; f5f0 <procsend+0x210>
    f418:	e59f01d4 	ldr	r0, [pc, #468]	; f5f4 <procsend+0x214>
    f41c:	e3520000 	cmp	r2, #0	; 0x0
    f420:	e1a0152c 	lsr	r1, ip, #10
    f424:	e583c000 	str	ip, [r3]
    f428:	03a03b01 	moveq	r3, #1024	; 0x400
    f42c:	e5802000 	str	r2, [r0]
    f430:	e5841000 	str	r1, [r4]
    f434:	05803000 	streq	r3, [r0]
    f438:	0a000006 	beq	f458 <procsend+0x78>
    f43c:	e2813001 	add	r3, r1, #1	; 0x1
    f440:	e352007f 	cmp	r2, #127	; 0x7f
    f444:	e59f21ac 	ldr	r2, [pc, #428]	; f5f8 <procsend+0x218>
    f448:	e5843000 	str	r3, [r4]
    f44c:	93a03001 	movls	r3, #1	; 0x1
    f450:	83a03002 	movhi	r3, #2	; 0x2
    f454:	e5c23000 	strb	r3, [r2]
    f458:	e59f019c 	ldr	r0, [pc, #412]	; f5fc <procsend+0x21c>
    f45c:	ebffc7cd 	bl	1398 <strlen>
    f460:	e59f2198 	ldr	r2, [pc, #408]	; f600 <procsend+0x220>
    f464:	e3500080 	cmp	r0, #128	; 0x80
    f468:	83a03002 	movhi	r3, #2	; 0x2
    f46c:	93a03001 	movls	r3, #1	; 0x1
    f470:	85c23000 	strbhi	r3, [r2]
    f474:	95c23000 	strbls	r3, [r2]
    f478:	e59f3184 	ldr	r3, [pc, #388]	; f604 <procsend+0x224>
    f47c:	83a07b01 	movhi	r7, #1024	; 0x400
    f480:	e5d31000 	ldrb	r1, [r3]
    f484:	93a07080 	movls	r7, #128	; 0x80
    f488:	e3510000 	cmp	r1, #0	; 0x0
    f48c:	13a01000 	movne	r1, #0	; 0x0
    f490:	12872005 	addne	r2, r7, #5	; 0x5
    f494:	02872004 	addeq	r2, r7, #4	; 0x4
    f498:	11a00006 	movne	r0, r6
    f49c:	01a00006 	moveq	r0, r6
    f4a0:	ebffc7ce 	bl	13e0 <memset>
    f4a4:	e59f3154 	ldr	r3, [pc, #340]	; f600 <procsend+0x220>
    f4a8:	e3a05000 	mov	r5, #0	; 0x0
    f4ac:	e5d33000 	ldrb	r3, [r3]
    f4b0:	e59f0144 	ldr	r0, [pc, #324]	; f5fc <procsend+0x21c>
    f4b4:	e5c63000 	strb	r3, [r6]
    f4b8:	e3e03000 	mvn	r3, #0	; 0x0
    f4bc:	e5c63002 	strb	r3, [r6, #2]
    f4c0:	e5c65001 	strb	r5, [r6, #1]
    f4c4:	ebffc7b3 	bl	1398 <strlen>
    f4c8:	e59f112c 	ldr	r1, [pc, #300]	; f5fc <procsend+0x21c>
    f4cc:	e1a02000 	mov	r2, r0
    f4d0:	e1a04000 	mov	r4, r0
    f4d4:	e2860003 	add	r0, r6, #3	; 0x3
    f4d8:	ebffc75e 	bl	1258 <strncpy>
    f4dc:	e59f3108 	ldr	r3, [pc, #264]	; f5ec <procsend+0x20c>
    f4e0:	e2844004 	add	r4, r4, #4	; 0x4
    f4e4:	e0860004 	add	r0, r6, r4
    f4e8:	e5932000 	ldr	r2, [r3]
    f4ec:	e59f1114 	ldr	r1, [pc, #276]	; f608 <procsend+0x228>
    f4f0:	e59f4114 	ldr	r4, [pc, #276]	; f60c <procsend+0x22c>
    f4f4:	ebfffab8 	bl	dfdc <sprintf>
    f4f8:	e1a00006 	mov	r0, r6
    f4fc:	e1a0c005 	mov	ip, r5
    f500:	e1a0e005 	mov	lr, r5
    f504:	e1a0342c 	lsr	r3, ip, #8
    f508:	e5d02003 	ldrb	r2, [r0, #3]
    f50c:	e1a03083 	lsl	r3, r3, #1
    f510:	e19410b3 	ldrh	r1, [r4, r3]
    f514:	e2855001 	add	r5, r5, #1	; 0x1
    f518:	e182340c 	orr	r3, r2, ip, lsl #8
    f51c:	e0233001 	eor	r3, r3, r1
    f520:	e08e2002 	add	r2, lr, r2
    f524:	e1550007 	cmp	r5, r7
    f528:	e6ffc073 	uxth	ip, r3
    f52c:	e6efe072 	uxtb	lr, r2
    f530:	e2800001 	add	r0, r0, #1	; 0x1
    f534:	e59f10d0 	ldr	r1, [pc, #208]	; f60c <procsend+0x22c>
    f538:	3afffff1 	bcc	f504 <procsend+0x124>
    f53c:	e59f30c0 	ldr	r3, [pc, #192]	; f604 <procsend+0x224>
    f540:	e0860007 	add	r0, r6, r7
    f544:	e5d33000 	ldrb	r3, [r3]
    f548:	e3530000 	cmp	r3, #0	; 0x0
    f54c:	05c0e003 	strbeq	lr, [r0, #3]
    f550:	0a00000c 	beq	f588 <procsend+0x1a8>
    f554:	e1a0342c 	lsr	r3, ip, #8
    f558:	e1a03083 	lsl	r3, r3, #1
    f55c:	e19120b3 	ldrh	r2, [r1, r3]
    f560:	e022240c 	eor	r2, r2, ip, lsl #8
    f564:	e1a03442 	asr	r3, r2, #8
    f568:	e20330ff 	and	r3, r3, #255	; 0xff
    f56c:	e1a03083 	lsl	r3, r3, #1
    f570:	e19130b3 	ldrh	r3, [r1, r3]
    f574:	e0233402 	eor	r3, r3, r2, lsl #8
    f578:	e6ff3073 	uxth	r3, r3
    f57c:	e1a02423 	lsr	r2, r3, #8
    f580:	e5c03004 	strb	r3, [r0, #4]
    f584:	e5c02003 	strb	r2, [r0, #3]
    f588:	e59f3080 	ldr	r3, [pc, #128]	; f610 <procsend+0x230>
    f58c:	e3a02001 	mov	r2, #1	; 0x1
    f590:	e5c32000 	strb	r2, [r3]
    f594:	e3a00000 	mov	r0, #0	; 0x0
    f598:	ea00000f 	b	f5dc <procsend+0x1fc>
    f59c:	e59f2070 	ldr	r2, [pc, #112]	; f614 <procsend+0x234>
    f5a0:	e5943000 	ldr	r3, [r4]
    f5a4:	e5922000 	ldr	r2, [r2]
    f5a8:	e1520003 	cmp	r2, r3
    f5ac:	8a00000a 	bhi	f5dc <procsend+0x1fc>
    f5b0:	e59f3060 	ldr	r3, [pc, #96]	; f618 <procsend+0x238>
    f5b4:	e59f2044 	ldr	r2, [pc, #68]	; f600 <procsend+0x220>
    f5b8:	e5d33000 	ldrb	r3, [r3]
    f5bc:	e1a00006 	mov	r0, r6
    f5c0:	e3530000 	cmp	r3, #0	; 0x0
    f5c4:	13a03001 	movne	r3, #1	; 0x1
    f5c8:	03a03002 	moveq	r3, #2	; 0x2
    f5cc:	e5c23000 	strb	r3, [r2]
    f5d0:	e1a0200c 	mov	r2, ip
    f5d4:	ebffff07 	bl	f1f8 <Procpacket>
    f5d8:	e6ef0070 	uxtb	r0, r0
    f5dc:	e28dd004 	add	sp, sp, #4	; 0x4
    f5e0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    f5e4:	0800d19c 	.word	0x0800d19c
    f5e8:	0800dcf4 	.word	0x0800dcf4
    f5ec:	0800d7c8 	.word	0x0800d7c8
    f5f0:	0800d7c4 	.word	0x0800d7c4
    f5f4:	0800dcf8 	.word	0x0800dcf8
    f5f8:	0800dcfc 	.word	0x0800dcfc
    f5fc:	000144ea 	.word	0x000144ea
    f600:	0800d31a 	.word	0x0800d31a
    f604:	0800d7c0 	.word	0x0800d7c0
    f608:	000132bf 	.word	0x000132bf
    f60c:	000116a0 	.word	0x000116a0
    f610:	0800d319 	.word	0x0800d319
    f614:	0800d31c 	.word	0x0800d31c
    f618:	0800d325 	.word	0x0800d325

0000f61c <CYGACC_COMM_IF_PUTC>:
    f61c:	e1a00001 	mov	r0, r1
    f620:	eafffaf7 	b	e204 <putc>

0000f624 <real_send>:
    f624:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    f628:	e59f30ec 	ldr	r3, [pc, #236]	; f71c <real_send+0xf8>
    f62c:	e24ddb01 	sub	sp, sp, #1024	; 0x400
    f630:	e5d33000 	ldrb	r3, [r3]
    f634:	e24dd00c 	sub	sp, sp, #12	; 0xc
    f638:	e3530000 	cmp	r3, #0	; 0x0
    f63c:	e1a0e000 	mov	lr, r0
    f640:	e1a0c001 	mov	ip, r1
    f644:	159f30d4 	ldrne	r3, [pc, #212]	; f720 <real_send+0xfc>
    f648:	13a02000 	movne	r2, #0	; 0x0
    f64c:	15832000 	strne	r2, [r3]
    f650:	1a000006 	bne	f670 <real_send+0x4c>
    f654:	e59f30c8 	ldr	r3, [pc, #200]	; f724 <real_send+0x100>
    f658:	e5d33000 	ldrb	r3, [r3]
    f65c:	e3530000 	cmp	r3, #0	; 0x0
    f660:	059f20b8 	ldreq	r2, [pc, #184]	; f720 <real_send+0xfc>
    f664:	05923000 	ldreq	r3, [r2]
    f668:	02833001 	addeq	r3, r3, #1	; 0x1
    f66c:	05823000 	streq	r3, [r2]
    f670:	e28d0008 	add	r0, sp, #8	; 0x8
    f674:	e2400005 	sub	r0, r0, #5	; 0x5
    f678:	e1a0100e 	mov	r1, lr
    f67c:	e1a0200c 	mov	r2, ip
    f680:	ebffff56 	bl	f3e0 <procsend>
    f684:	e2507000 	subs	r7, r0, #0	; 0x0
    f688:	1a00001f 	bne	f70c <real_send+0xe8>
    f68c:	e59f3094 	ldr	r3, [pc, #148]	; f728 <real_send+0x104>
    f690:	e28d5008 	add	r5, sp, #8	; 0x8
    f694:	e5d31000 	ldrb	r1, [r3]
    f698:	e59f308c 	ldr	r3, [pc, #140]	; f72c <real_send+0x108>
    f69c:	e3510001 	cmp	r1, #1	; 0x1
    f6a0:	e5d32000 	ldrb	r2, [r3]
    f6a4:	e3003404 	movw	r3, #1028	; 0x404
    f6a8:	11a06003 	movne	r6, r3
    f6ac:	03a06084 	moveq	r6, #132	; 0x84
    f6b0:	e3520000 	cmp	r2, #0	; 0x0
    f6b4:	12866001 	addne	r6, r6, #1	; 0x1
    f6b8:	e2455005 	sub	r5, r5, #5	; 0x5
    f6bc:	e3a04000 	mov	r4, #0	; 0x0
    f6c0:	e7d50004 	ldrb	r0, [r5, r4]
    f6c4:	e2844001 	add	r4, r4, #1	; 0x1
    f6c8:	ebfffacd 	bl	e204 <putc>
    f6cc:	e1540006 	cmp	r4, r6
    f6d0:	3afffffa 	bcc	f6c0 <real_send+0x9c>
    f6d4:	e59f3054 	ldr	r3, [pc, #84]	; f730 <real_send+0x10c>
    f6d8:	e5d33000 	ldrb	r3, [r3]
    f6dc:	e3530000 	cmp	r3, #0	; 0x0
    f6e0:	13e07000 	mvnne	r7, #0	; 0x0
    f6e4:	1a000008 	bne	f70c <real_send+0xe8>
    f6e8:	e59f3030 	ldr	r3, [pc, #48]	; f720 <real_send+0xfc>
    f6ec:	e5932000 	ldr	r2, [r3]
    f6f0:	e59f303c 	ldr	r3, [pc, #60]	; f734 <real_send+0x110>
    f6f4:	e5933000 	ldr	r3, [r3]
    f6f8:	e1520003 	cmp	r2, r3
    f6fc:	059f3034 	ldreq	r3, [pc, #52]	; f738 <real_send+0x114>
    f700:	03a02001 	moveq	r2, #1	; 0x1
    f704:	05c32000 	strbeq	r2, [r3]
    f708:	03a07003 	moveq	r7, #3	; 0x3
    f70c:	e1a00007 	mov	r0, r7
    f710:	e28dd00c 	add	sp, sp, #12	; 0xc
    f714:	e28ddb01 	add	sp, sp, #1024	; 0x400
    f718:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    f71c:	0800d325 	.word	0x0800d325
    f720:	0800d31c 	.word	0x0800d31c
    f724:	0800d19c 	.word	0x0800d19c
    f728:	0800d31a 	.word	0x0800d31a
    f72c:	0800d7c0 	.word	0x0800d7c0
    f730:	0800d324 	.word	0x0800d324
    f734:	0800dcf4 	.word	0x0800dcf4
    f738:	0800d320 	.word	0x0800d320

0000f73c <CYGACC_COMM_IF_GETC_TIMEOUT>:
    f73c:	e92d4070 	push	{r4, r5, r6, lr}
    f740:	e1a05001 	mov	r5, r1
    f744:	e3a04000 	mov	r4, #0	; 0x0
    f748:	e59f603c 	ldr	r6, [pc, #60]	; f78c <CYGACC_COMM_IF_GETC_TIMEOUT+0x50>
    f74c:	ea000000 	b	f754 <CYGACC_COMM_IF_GETC_TIMEOUT+0x18>
    f750:	ebffe46d 	bl	890c <udelay>
    f754:	ebfffaab 	bl	e208 <tstc>
    f758:	e3500000 	cmp	r0, #0	; 0x0
    f75c:	e3a00014 	mov	r0, #20	; 0x14
    f760:	1a000002 	bne	f770 <CYGACC_COMM_IF_GETC_TIMEOUT+0x34>
    f764:	e1540006 	cmp	r4, r6
    f768:	e2844001 	add	r4, r4, #1	; 0x1
    f76c:	1afffff7 	bne	f750 <CYGACC_COMM_IF_GETC_TIMEOUT+0x14>
    f770:	ebfffaa4 	bl	e208 <tstc>
    f774:	e3500000 	cmp	r0, #0	; 0x0
    f778:	08bd8070 	popeq	{r4, r5, r6, pc}
    f77c:	ebfffaa2 	bl	e20c <getc>
    f780:	e5c50000 	strb	r0, [r5]
    f784:	e3a00001 	mov	r0, #1	; 0x1
    f788:	e8bd8070 	pop	{r4, r5, r6, pc}
    f78c:	000186a0 	.word	0x000186a0

0000f790 <control_send>:
    f790:	e59f323c 	ldr	r3, [pc, #572]	; f9d4 <control_send+0x244>
    f794:	e3a02000 	mov	r2, #0	; 0x0
    f798:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    f79c:	e5c32000 	strb	r2, [r3]
    f7a0:	e59f3230 	ldr	r3, [pc, #560]	; f9d8 <control_send+0x248>
    f7a4:	e59f4230 	ldr	r4, [pc, #560]	; f9dc <control_send+0x24c>
    f7a8:	e5c32000 	strb	r2, [r3]
    f7ac:	e59f322c 	ldr	r3, [pc, #556]	; f9e0 <control_send+0x250>
    f7b0:	e59f522c 	ldr	r5, [pc, #556]	; f9e4 <control_send+0x254>
    f7b4:	e5832000 	str	r2, [r3]
    f7b8:	e3a03001 	mov	r3, #1	; 0x1
    f7bc:	e59fe224 	ldr	lr, [pc, #548]	; f9e8 <control_send+0x258>
    f7c0:	e59f8224 	ldr	r8, [pc, #548]	; f9ec <control_send+0x25c>
    f7c4:	e59f6224 	ldr	r6, [pc, #548]	; f9f0 <control_send+0x260>
    f7c8:	e59f7224 	ldr	r7, [pc, #548]	; f9f4 <control_send+0x264>
    f7cc:	e59fc224 	ldr	ip, [pc, #548]	; f9f8 <control_send+0x268>
    f7d0:	e5c43000 	strb	r3, [r4]
    f7d4:	e59f3220 	ldr	r3, [pc, #544]	; f9fc <control_send+0x26c>
    f7d8:	e24dd014 	sub	sp, sp, #20	; 0x14
    f7dc:	e5c52000 	strb	r2, [r5]
    f7e0:	e5c32000 	strb	r2, [r3]
    f7e4:	e58d0004 	str	r0, [sp, #4]
    f7e8:	e1a0b001 	mov	fp, r1
    f7ec:	e5c82000 	strb	r2, [r8]
    f7f0:	e5c62000 	strb	r2, [r6]
    f7f4:	e5c72000 	strb	r2, [r7]
    f7f8:	e5ce2000 	strb	r2, [lr]
    f7fc:	e5cc2000 	strb	r2, [ip]
    f800:	e1a0a002 	mov	sl, r2
    f804:	e1a0900e 	mov	r9, lr
    f808:	e59f51f0 	ldr	r5, [pc, #496]	; fa00 <control_send+0x270>
    f80c:	ea000000 	b	f814 <control_send+0x84>
    f810:	e3a0a000 	mov	sl, #0	; 0x0
    f814:	e3a00000 	mov	r0, #0	; 0x0
    f818:	e28d100f 	add	r1, sp, #15	; 0xf
    f81c:	ebffffc6 	bl	f73c <CYGACC_COMM_IF_GETC_TIMEOUT>
    f820:	e3500000 	cmp	r0, #0	; 0x0
    f824:	0afffffa 	beq	f814 <control_send+0x84>
    f828:	e5dd300f 	ldrb	r3, [sp, #15]
    f82c:	e3530018 	cmp	r3, #24	; 0x18
    f830:	0a000042 	beq	f940 <control_send+0x1b0>
    f834:	8a000004 	bhi	f84c <control_send+0xbc>
    f838:	e3530006 	cmp	r3, #6	; 0x6
    f83c:	0a00002c 	beq	f8f4 <control_send+0x164>
    f840:	e3530015 	cmp	r3, #21	; 0x15
    f844:	1afffff1 	bne	f810 <control_send+0x80>
    f848:	ea000010 	b	f890 <control_send+0x100>
    f84c:	e3530043 	cmp	r3, #67	; 0x43
    f850:	0a000001 	beq	f85c <control_send+0xcc>
    f854:	e3530063 	cmp	r3, #99	; 0x63
    f858:	1affffec 	bne	f810 <control_send+0x80>
    f85c:	e5d73000 	ldrb	r3, [r7]
    f860:	e5d42000 	ldrb	r2, [r4]
    f864:	e5d61000 	ldrb	r1, [r6]
    f868:	e1833002 	orr	r3, r3, r2
    f86c:	e1913003 	orrs	r3, r1, r3
    f870:	0affffe6 	beq	f810 <control_send+0x80>
    f874:	e3510000 	cmp	r1, #0	; 0x0
    f878:	159f2178 	ldrne	r2, [pc, #376]	; f9f8 <control_send+0x268>
    f87c:	13a03001 	movne	r3, #1	; 0x1
    f880:	15c23000 	strbne	r3, [r2]
    f884:	e3a03001 	mov	r3, #1	; 0x1
    f888:	e5c53000 	strb	r3, [r5]
    f88c:	ea00002f 	b	f950 <control_send+0x1c0>
    f890:	e59f313c 	ldr	r3, [pc, #316]	; f9d4 <control_send+0x244>
    f894:	e5d33000 	ldrb	r3, [r3]
    f898:	e3530000 	cmp	r3, #0	; 0x0
    f89c:	0a000008 	beq	f8c4 <control_send+0x134>
    f8a0:	e59f2138 	ldr	r2, [pc, #312]	; f9e0 <control_send+0x250>
    f8a4:	e5923000 	ldr	r3, [r2]
    f8a8:	e2433001 	sub	r3, r3, #1	; 0x1
    f8ac:	e5823000 	str	r3, [r2]
    f8b0:	e3a03001 	mov	r3, #1	; 0x1
    f8b4:	e5c43000 	strb	r3, [r4]
    f8b8:	e3a03000 	mov	r3, #0	; 0x0
    f8bc:	e5c93000 	strb	r3, [r9]
    f8c0:	ea000022 	b	f950 <control_send+0x1c0>
    f8c4:	e5d42000 	ldrb	r2, [r4]
    f8c8:	e3520000 	cmp	r2, #0	; 0x0
    f8cc:	15c53000 	strbne	r3, [r5]
    f8d0:	1a000007 	bne	f8f4 <control_send+0x164>
    f8d4:	e5d93000 	ldrb	r3, [r9]
    f8d8:	e3530000 	cmp	r3, #0	; 0x0
    f8dc:	15c92000 	strbne	r2, [r9]
    f8e0:	e59f20f8 	ldr	r2, [pc, #248]	; f9e0 <control_send+0x250>
    f8e4:	e5923000 	ldr	r3, [r2]
    f8e8:	e2433001 	sub	r3, r3, #1	; 0x1
    f8ec:	e5823000 	str	r3, [r2]
    f8f0:	ea000016 	b	f950 <control_send+0x1c0>
    f8f4:	e5d42000 	ldrb	r2, [r4]
    f8f8:	e3520000 	cmp	r2, #0	; 0x0
    f8fc:	1affffc3 	bne	f810 <control_send+0x80>
    f900:	e5d91000 	ldrb	r1, [r9]
    f904:	e3510000 	cmp	r1, #0	; 0x0
    f908:	0a000005 	beq	f924 <control_send+0x194>
    f90c:	e59f30c0 	ldr	r3, [pc, #192]	; f9d4 <control_send+0x244>
    f910:	e1a0a002 	mov	sl, r2
    f914:	e5c32000 	strb	r2, [r3]
    f918:	e3a03001 	mov	r3, #1	; 0x1
    f91c:	e5c73000 	strb	r3, [r7]
    f920:	eaffffbb 	b	f814 <control_send+0x84>
    f924:	e5d83000 	ldrb	r3, [r8]
    f928:	e3530000 	cmp	r3, #0	; 0x0
    f92c:	13a03001 	movne	r3, #1	; 0x1
    f930:	15c63000 	strbne	r3, [r6]
    f934:	11a0a001 	movne	sl, r1
    f938:	1affffb5 	bne	f814 <control_send+0x84>
    f93c:	ea000003 	b	f950 <control_send+0x1c0>
    f940:	e28aa001 	add	sl, sl, #1	; 0x1
    f944:	e35a0002 	cmp	sl, #2	; 0x2
    f948:	1affffb1 	bne	f814 <control_send+0x84>
    f94c:	ea00001d 	b	f9c8 <control_send+0x238>
    f950:	e59f20a0 	ldr	r2, [pc, #160]	; f9f8 <control_send+0x268>
    f954:	e5d23000 	ldrb	r3, [r2]
    f958:	e3530000 	cmp	r3, #0	; 0x0
    f95c:	1a000008 	bne	f984 <control_send+0x1f4>
    f960:	e59f207c 	ldr	r2, [pc, #124]	; f9e4 <control_send+0x254>
    f964:	e5d23000 	ldrb	r3, [r2]
    f968:	e3530000 	cmp	r3, #0	; 0x0
    f96c:	0a000004 	beq	f984 <control_send+0x1f4>
    f970:	e3a00004 	mov	r0, #4	; 0x4
    f974:	ebfffa22 	bl	e204 <putc>
    f978:	e3a03001 	mov	r3, #1	; 0x1
    f97c:	e5c83000 	strb	r3, [r8]
    f980:	eaffffa2 	b	f810 <control_send+0x80>
    f984:	e59d0004 	ldr	r0, [sp, #4]
    f988:	e1a0100b 	mov	r1, fp
    f98c:	ebffff24 	bl	f624 <real_send>
    f990:	e3500003 	cmp	r0, #3	; 0x3
    f994:	0affff9d 	beq	f810 <control_send+0x80>
    f998:	e3500000 	cmp	r0, #0	; 0x0
    f99c:	1a00000a 	bne	f9cc <control_send+0x23c>
    f9a0:	e5d93000 	ldrb	r3, [r9]
    f9a4:	e3530000 	cmp	r3, #0	; 0x0
    f9a8:	e5d43000 	ldrb	r3, [r4]
    f9ac:	15c90000 	strbne	r0, [r9]
    f9b0:	e3530000 	cmp	r3, #0	; 0x0
    f9b4:	13a03000 	movne	r3, #0	; 0x0
    f9b8:	15c43000 	strbne	r3, [r4]
    f9bc:	12833001 	addne	r3, r3, #1	; 0x1
    f9c0:	15c93000 	strbne	r3, [r9]
    f9c4:	eaffff91 	b	f810 <control_send+0x80>
    f9c8:	e3a00000 	mov	r0, #0	; 0x0
    f9cc:	e28dd014 	add	sp, sp, #20	; 0x14
    f9d0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    f9d4:	0800d319 	.word	0x0800d319
    f9d8:	0800d31a 	.word	0x0800d31a
    f9dc:	0800d19c 	.word	0x0800d19c
    f9e0:	0800d31c 	.word	0x0800d31c
    f9e4:	0800d320 	.word	0x0800d320
    f9e8:	0800d323 	.word	0x0800d323
    f9ec:	0800d318 	.word	0x0800d318
    f9f0:	0800d321 	.word	0x0800d321
    f9f4:	0800d322 	.word	0x0800d322
    f9f8:	0800d325 	.word	0x0800d325
    f9fc:	0800d324 	.word	0x0800d324
    fa00:	0800d7c0 	.word	0x0800d7c0

0000fa04 <xyzModem_stream_terminate>:
    fa04:	e92d4030 	push	{r4, r5, lr}
    fa08:	e3500000 	cmp	r0, #0	; 0x0
    fa0c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    fa10:	e1a04001 	mov	r4, r1
    fa14:	0a000020 	beq	fa9c <xyzModem_stream_terminate+0x98>
    fa18:	e59f4098 	ldr	r4, [pc, #152]	; fab8 <xyzModem_stream_terminate+0xb4>
    fa1c:	e5943414 	ldr	r3, [r4, #1044]
    fa20:	e2433001 	sub	r3, r3, #1	; 0x1
    fa24:	e3530001 	cmp	r3, #1	; 0x1
    fa28:	8a000020 	bhi	fab0 <xyzModem_stream_terminate+0xac>
    fa2c:	e3a00018 	mov	r0, #24	; 0x18
    fa30:	ebfff9f3 	bl	e204 <putc>
    fa34:	e3a00018 	mov	r0, #24	; 0x18
    fa38:	ebfff9f1 	bl	e204 <putc>
    fa3c:	e3a00018 	mov	r0, #24	; 0x18
    fa40:	ebfff9ef 	bl	e204 <putc>
    fa44:	e3a00018 	mov	r0, #24	; 0x18
    fa48:	ebfff9ed 	bl	e204 <putc>
    fa4c:	e3a00008 	mov	r0, #8	; 0x8
    fa50:	ebfff9eb 	bl	e204 <putc>
    fa54:	e3a00008 	mov	r0, #8	; 0x8
    fa58:	ebfff9e9 	bl	e204 <putc>
    fa5c:	e3a00008 	mov	r0, #8	; 0x8
    fa60:	ebfff9e7 	bl	e204 <putc>
    fa64:	e3a00008 	mov	r0, #8	; 0x8
    fa68:	ebfff9e5 	bl	e204 <putc>
    fa6c:	e1a05004 	mov	r5, r4
    fa70:	e28d4007 	add	r4, sp, #7	; 0x7
    fa74:	e5953000 	ldr	r3, [r5]
    fa78:	e1a01004 	mov	r1, r4
    fa7c:	e5d30000 	ldrb	r0, [r3]
    fa80:	ebffff2d 	bl	f73c <CYGACC_COMM_IF_GETC_TIMEOUT>
    fa84:	e3500000 	cmp	r0, #0	; 0x0
    fa88:	1afffff9 	bne	fa74 <xyzModem_stream_terminate+0x70>
    fa8c:	e59f3024 	ldr	r3, [pc, #36]	; fab8 <xyzModem_stream_terminate+0xb4>
    fa90:	e3a02001 	mov	r2, #1	; 0x1
    fa94:	e583242c 	str	r2, [r3, #1068]
    fa98:	ea000004 	b	fab0 <xyzModem_stream_terminate+0xac>
    fa9c:	e12fff34 	blx	r4
    faa0:	e3500000 	cmp	r0, #0	; 0x0
    faa4:	aafffffc 	bge	fa9c <xyzModem_stream_terminate+0x98>
    faa8:	e59f000c 	ldr	r0, [pc, #12]	; fabc <xyzModem_stream_terminate+0xb8>
    faac:	ebffe396 	bl	890c <udelay>
    fab0:	e28dd00c 	add	sp, sp, #12	; 0xc
    fab4:	e8bd8030 	pop	{r4, r5, pc}
    fab8:	0800d328 	.word	0x0800d328
    fabc:	0003d090 	.word	0x0003d090

0000fac0 <xyzModem_get_hdr>:
    fac0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    fac4:	e59f42a0 	ldr	r4, [pc, #672]	; fd6c <xyzModem_get_hdr+0x2ac>
    fac8:	e24dd00c 	sub	sp, sp, #12	; 0xc
    facc:	e5943430 	ldr	r3, [r4, #1072]
    fad0:	e3530000 	cmp	r3, #0	; 0x0
    fad4:	0a000003 	beq	fae8 <xyzModem_get_hdr+0x28>
    fad8:	e3a00006 	mov	r0, #6	; 0x6
    fadc:	ebfff9c8 	bl	e204 <putc>
    fae0:	e3a03000 	mov	r3, #0	; 0x0
    fae4:	e5843430 	str	r3, [r4, #1072]
    fae8:	e3a04000 	mov	r4, #0	; 0x0
    faec:	e59f6278 	ldr	r6, [pc, #632]	; fd6c <xyzModem_get_hdr+0x2ac>
    faf0:	e1a05004 	mov	r5, r4
    faf4:	e28d7007 	add	r7, sp, #7	; 0x7
    faf8:	e5963000 	ldr	r3, [r6]
    fafc:	e1a01007 	mov	r1, r7
    fb00:	e5d30000 	ldrb	r0, [r3]
    fb04:	ebffff0c 	bl	f73c <CYGACC_COMM_IF_GETC_TIMEOUT>
    fb08:	e3500000 	cmp	r0, #0	; 0x0
    fb0c:	e2855001 	add	r5, r5, #1	; 0x1
    fb10:	e59f2254 	ldr	r2, [pc, #596]	; fd6c <xyzModem_get_hdr+0x2ac>
    fb14:	0a000021 	beq	fba0 <xyzModem_get_hdr+0xe0>
    fb18:	e5dd3007 	ldrb	r3, [sp, #7]
    fb1c:	e3530002 	cmp	r3, #2	; 0x2
    fb20:	0a00000c 	beq	fb58 <xyzModem_get_hdr+0x98>
    fb24:	8a000002 	bhi	fb34 <xyzModem_get_hdr+0x74>
    fb28:	e3530001 	cmp	r3, #1	; 0x1
    fb2c:	1afffff1 	bne	faf8 <xyzModem_get_hdr+0x38>
    fb30:	ea000004 	b	fb48 <xyzModem_get_hdr+0x88>
    fb34:	e3530004 	cmp	r3, #4	; 0x4
    fb38:	0a000012 	beq	fb88 <xyzModem_get_hdr+0xc8>
    fb3c:	e3530018 	cmp	r3, #24	; 0x18
    fb40:	1affffec 	bne	faf8 <xyzModem_get_hdr+0x38>
    fb44:	ea000007 	b	fb68 <xyzModem_get_hdr+0xa8>
    fb48:	e592341c 	ldr	r3, [r2, #1052]
    fb4c:	e2833001 	add	r3, r3, #1	; 0x1
    fb50:	e582341c 	str	r3, [r2, #1052]
    fb54:	ea00007c 	b	fd4c <xyzModem_get_hdr+0x28c>
    fb58:	e5923420 	ldr	r3, [r2, #1056]
    fb5c:	e2833001 	add	r3, r3, #1	; 0x1
    fb60:	e5823420 	str	r3, [r2, #1056]
    fb64:	ea000078 	b	fd4c <xyzModem_get_hdr+0x28c>
    fb68:	e5963424 	ldr	r3, [r6, #1060]
    fb6c:	e2844001 	add	r4, r4, #1	; 0x1
    fb70:	e2833001 	add	r3, r3, #1	; 0x1
    fb74:	e3540003 	cmp	r4, #3	; 0x3
    fb78:	e5863424 	str	r3, [r6, #1060]
    fb7c:	1affffdd 	bne	faf8 <xyzModem_get_hdr+0x38>
    fb80:	e3e00004 	mvn	r0, #4	; 0x4
    fb84:	ea00006e 	b	fd44 <xyzModem_get_hdr+0x284>
    fb88:	e3550001 	cmp	r5, #1	; 0x1
    fb8c:	1affffd9 	bne	faf8 <xyzModem_get_hdr+0x38>
    fb90:	e3a00006 	mov	r0, #6	; 0x6
    fb94:	ebfff99a 	bl	e204 <putc>
    fb98:	e3e00003 	mvn	r0, #3	; 0x3
    fb9c:	ea000068 	b	fd44 <xyzModem_get_hdr+0x284>
    fba0:	e1a05002 	mov	r5, r2
    fba4:	e28d4006 	add	r4, sp, #6	; 0x6
    fba8:	e5953000 	ldr	r3, [r5]
    fbac:	e1a01004 	mov	r1, r4
    fbb0:	e5d30000 	ldrb	r0, [r3]
    fbb4:	ebfffee0 	bl	f73c <CYGACC_COMM_IF_GETC_TIMEOUT>
    fbb8:	e3500000 	cmp	r0, #0	; 0x0
    fbbc:	1afffff9 	bne	fba8 <xyzModem_get_hdr+0xe8>
    fbc0:	e59f01a8 	ldr	r0, [pc, #424]	; fd70 <xyzModem_get_hdr+0x2b0>
    fbc4:	ebffe350 	bl	890c <udelay>
    fbc8:	ea00005a 	b	fd38 <xyzModem_get_hdr+0x278>
    fbcc:	e5943000 	ldr	r3, [r4]
    fbd0:	e59f119c 	ldr	r1, [pc, #412]	; fd74 <xyzModem_get_hdr+0x2b4>
    fbd4:	e5d30000 	ldrb	r0, [r3]
    fbd8:	ebfffed7 	bl	f73c <CYGACC_COMM_IF_GETC_TIMEOUT>
    fbdc:	e3500000 	cmp	r0, #0	; 0x0
    fbe0:	0a000054 	beq	fd38 <xyzModem_get_hdr+0x278>
    fbe4:	e28d5008 	add	r5, sp, #8	; 0x8
    fbe8:	e5753001 	ldrb	r3, [r5, #-1]!
    fbec:	e2842004 	add	r2, r4, #4	; 0x4
    fbf0:	e3530001 	cmp	r3, #1	; 0x1
    fbf4:	03a03080 	moveq	r3, #128	; 0x80
    fbf8:	13a03b01 	movne	r3, #1024	; 0x400
    fbfc:	e5843410 	str	r3, [r4, #1040]
    fc00:	e5842404 	str	r2, [r4, #1028]
    fc04:	e1a07004 	mov	r7, r4
    fc08:	e3a04000 	mov	r4, #0	; 0x0
    fc0c:	ea000008 	b	fc34 <xyzModem_get_hdr+0x174>
    fc10:	e5973000 	ldr	r3, [r7]
    fc14:	e5d30000 	ldrb	r0, [r3]
    fc18:	ebfffec7 	bl	f73c <CYGACC_COMM_IF_GETC_TIMEOUT>
    fc1c:	e3500000 	cmp	r0, #0	; 0x0
    fc20:	e0872004 	add	r2, r7, r4
    fc24:	e2844001 	add	r4, r4, #1	; 0x1
    fc28:	0a000042 	beq	fd38 <xyzModem_get_hdr+0x278>
    fc2c:	e5dd3007 	ldrb	r3, [sp, #7]
    fc30:	e5c23004 	strb	r3, [r2, #4]
    fc34:	e5973410 	ldr	r3, [r7, #1040]
    fc38:	e1a01005 	mov	r1, r5
    fc3c:	e1540003 	cmp	r4, r3
    fc40:	e59f6124 	ldr	r6, [pc, #292]	; fd6c <xyzModem_get_hdr+0x2ac>
    fc44:	bafffff1 	blt	fc10 <xyzModem_get_hdr+0x150>
    fc48:	e5963000 	ldr	r3, [r6]
    fc4c:	e59f1124 	ldr	r1, [pc, #292]	; fd78 <xyzModem_get_hdr+0x2b8>
    fc50:	e5d30000 	ldrb	r0, [r3]
    fc54:	ebfffeb8 	bl	f73c <CYGACC_COMM_IF_GETC_TIMEOUT>
    fc58:	e3500000 	cmp	r0, #0	; 0x0
    fc5c:	0a000035 	beq	fd38 <xyzModem_get_hdr+0x278>
    fc60:	e5963428 	ldr	r3, [r6, #1064]
    fc64:	e3530000 	cmp	r3, #0	; 0x0
    fc68:	0a000005 	beq	fc84 <xyzModem_get_hdr+0x1c4>
    fc6c:	e5963000 	ldr	r3, [r6]
    fc70:	e59f1104 	ldr	r1, [pc, #260]	; fd7c <xyzModem_get_hdr+0x2bc>
    fc74:	e5d30000 	ldrb	r0, [r3]
    fc78:	ebfffeaf 	bl	f73c <CYGACC_COMM_IF_GETC_TIMEOUT>
    fc7c:	e3500000 	cmp	r0, #0	; 0x0
    fc80:	0a00002c 	beq	fd38 <xyzModem_get_hdr+0x278>
    fc84:	e59f40e0 	ldr	r4, [pc, #224]	; fd6c <xyzModem_get_hdr+0x2ac>
    fc88:	e5d43409 	ldrb	r3, [r4, #1033]
    fc8c:	e5d42408 	ldrb	r2, [r4, #1032]
    fc90:	e0233002 	eor	r3, r3, r2
    fc94:	e35300ff 	cmp	r3, #255	; 0xff
    fc98:	0a000009 	beq	fcc4 <xyzModem_get_hdr+0x204>
    fc9c:	e1a05004 	mov	r5, r4
    fca0:	e28d4006 	add	r4, sp, #6	; 0x6
    fca4:	e5953000 	ldr	r3, [r5]
    fca8:	e1a01004 	mov	r1, r4
    fcac:	e5d30000 	ldrb	r0, [r3]
    fcb0:	ebfffea1 	bl	f73c <CYGACC_COMM_IF_GETC_TIMEOUT>
    fcb4:	e3500000 	cmp	r0, #0	; 0x0
    fcb8:	1afffff9 	bne	fca4 <xyzModem_get_hdr+0x1e4>
    fcbc:	e3e00005 	mvn	r0, #5	; 0x5
    fcc0:	ea00001f 	b	fd44 <xyzModem_get_hdr+0x284>
    fcc4:	e5943428 	ldr	r3, [r4, #1064]
    fcc8:	e3530000 	cmp	r3, #0	; 0x0
    fccc:	01a02003 	moveq	r2, r3
    fcd0:	01a0c004 	moveq	ip, r4
    fcd4:	05940410 	ldreq	r0, [r4, #1040]
    fcd8:	01a01003 	moveq	r1, r3
    fcdc:	0a00000a 	beq	fd0c <xyzModem_get_hdr+0x24c>
    fce0:	e2840004 	add	r0, r4, #4	; 0x4
    fce4:	e5941410 	ldr	r1, [r4, #1040]
    fce8:	eb000110 	bl	10130 <cyg_crc16>
    fcec:	e5d4340b 	ldrb	r3, [r4, #1035]
    fcf0:	e5d4240a 	ldrb	r2, [r4, #1034]
    fcf4:	e1833402 	orr	r3, r3, r2, lsl #8
    fcf8:	e1500003 	cmp	r0, r3
    fcfc:	ea00000a 	b	fd2c <xyzModem_get_hdr+0x26c>
    fd00:	e5d33004 	ldrb	r3, [r3, #4]
    fd04:	e0813003 	add	r3, r1, r3
    fd08:	e6ff1073 	uxth	r1, r3
    fd0c:	e1520000 	cmp	r2, r0
    fd10:	e08c3002 	add	r3, ip, r2
    fd14:	e2822001 	add	r2, r2, #1	; 0x1
    fd18:	bafffff8 	blt	fd00 <xyzModem_get_hdr+0x240>
    fd1c:	e59f3048 	ldr	r3, [pc, #72]	; fd6c <xyzModem_get_hdr+0x2ac>
    fd20:	e5d3240a 	ldrb	r2, [r3, #1034]
    fd24:	e20130ff 	and	r3, r1, #255	; 0xff
    fd28:	e1520003 	cmp	r2, r3
    fd2c:	13e00006 	mvnne	r0, #6	; 0x6
    fd30:	1a000003 	bne	fd44 <xyzModem_get_hdr+0x284>
    fd34:	ea000001 	b	fd40 <xyzModem_get_hdr+0x280>
    fd38:	e3e00002 	mvn	r0, #2	; 0x2
    fd3c:	ea000000 	b	fd44 <xyzModem_get_hdr+0x284>
    fd40:	e3a00000 	mov	r0, #0	; 0x0
    fd44:	e28dd00c 	add	sp, sp, #12	; 0xc
    fd48:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    fd4c:	e59f4018 	ldr	r4, [pc, #24]	; fd6c <xyzModem_get_hdr+0x2ac>
    fd50:	e59f1028 	ldr	r1, [pc, #40]	; fd80 <xyzModem_get_hdr+0x2c0>
    fd54:	e5943000 	ldr	r3, [r4]
    fd58:	e5d30000 	ldrb	r0, [r3]
    fd5c:	ebfffe76 	bl	f73c <CYGACC_COMM_IF_GETC_TIMEOUT>
    fd60:	e3500000 	cmp	r0, #0	; 0x0
    fd64:	0afffff3 	beq	fd38 <xyzModem_get_hdr+0x278>
    fd68:	eaffff97 	b	fbcc <xyzModem_get_hdr+0x10c>
    fd6c:	0800d328 	.word	0x0800d328
    fd70:	0003d090 	.word	0x0003d090
    fd74:	0800d731 	.word	0x0800d731
    fd78:	0800d732 	.word	0x0800d732
    fd7c:	0800d733 	.word	0x0800d733
    fd80:	0800d730 	.word	0x0800d730

0000fd84 <xyzModem_stream_read>:
    fd84:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    fd88:	e59f5240 	ldr	r5, [pc, #576]	; ffd0 <xyzModem_stream_read+0x24c>
    fd8c:	e24dd004 	sub	sp, sp, #4	; 0x4
    fd90:	e1a08000 	mov	r8, r0
    fd94:	e1a07001 	mov	r7, r1
    fd98:	e1a0b002 	mov	fp, r2
    fd9c:	e3e06004 	mvn	r6, #4	; 0x4
    fda0:	e3a0a000 	mov	sl, #0	; 0x0
    fda4:	e1a09005 	mov	r9, r5
    fda8:	ea00007e 	b	ffa8 <xyzModem_stream_read+0x224>
    fdac:	e5953410 	ldr	r3, [r5, #1040]
    fdb0:	e3530000 	cmp	r3, #0	; 0x0
    fdb4:	03a04014 	moveq	r4, #20	; 0x14
    fdb8:	0a00005d 	beq	ff34 <xyzModem_stream_read+0x1b0>
    fdbc:	ea000065 	b	ff58 <xyzModem_stream_read+0x1d4>
    fdc0:	ebffff3e 	bl	fac0 <xyzModem_get_hdr>
    fdc4:	e2506000 	subs	r6, r0, #0	; 0x0
    fdc8:	1a000039 	bne	feb4 <xyzModem_stream_read+0x130>
    fdcc:	e5d5240c 	ldrb	r2, [r5, #1036]
    fdd0:	e5d51408 	ldrb	r1, [r5, #1032]
    fdd4:	e2423001 	sub	r3, r2, #1	; 0x1
    fdd8:	e1510002 	cmp	r1, r2
    fddc:	e20330ff 	and	r3, r3, #255	; 0xff
    fde0:	1a00002d 	bne	fe9c <xyzModem_stream_read+0x118>
    fde4:	e599c414 	ldr	ip, [r9, #1044]
    fde8:	e2813001 	add	r3, r1, #1	; 0x1
    fdec:	e5c9340c 	strb	r3, [r9, #1036]
    fdf0:	e35c0001 	cmp	ip, #1	; 0x1
    fdf4:	e3a03001 	mov	r3, #1	; 0x1
    fdf8:	e5893430 	str	r3, [r9, #1072]
    fdfc:	0a000002 	beq	fe0c <xyzModem_stream_read+0x88>
    fe00:	e5993434 	ldr	r3, [r9, #1076]
    fe04:	e3530000 	cmp	r3, #0	; 0x0
    fe08:	1a000017 	bne	fe6c <xyzModem_stream_read+0xe8>
    fe0c:	e5953410 	ldr	r3, [r5, #1040]
    fe10:	e5950404 	ldr	r0, [r5, #1028]
    fe14:	e0802003 	add	r2, r0, r3
    fe18:	e5523001 	ldrb	r3, [r2, #-1]
    fe1c:	e353001a 	cmp	r3, #26	; 0x1a
    fe20:	1a00000f 	bne	fe64 <xyzModem_stream_read+0xe0>
    fe24:	e5523002 	ldrb	r3, [r2, #-2]
    fe28:	e353001a 	cmp	r3, #26	; 0x1a
    fe2c:	1a00000c 	bne	fe64 <xyzModem_stream_read+0xe0>
    fe30:	e5523003 	ldrb	r3, [r2, #-3]
    fe34:	e353001a 	cmp	r3, #26	; 0x1a
    fe38:	1a000009 	bne	fe64 <xyzModem_stream_read+0xe0>
    fe3c:	ea000000 	b	fe44 <xyzModem_stream_read+0xc0>
    fe40:	e5851410 	str	r1, [r5, #1040]
    fe44:	e5953410 	ldr	r3, [r5, #1040]
    fe48:	e3530000 	cmp	r3, #0	; 0x0
    fe4c:	e0802003 	add	r2, r0, r3
    fe50:	e2431001 	sub	r1, r3, #1	; 0x1
    fe54:	0a000002 	beq	fe64 <xyzModem_stream_read+0xe0>
    fe58:	e5523001 	ldrb	r3, [r2, #-1]
    fe5c:	e353001a 	cmp	r3, #26	; 0x1a
    fe60:	0afffff6 	beq	fe40 <xyzModem_stream_read+0xbc>
    fe64:	e35c0001 	cmp	ip, #1	; 0x1
    fe68:	0a00003a 	beq	ff58 <xyzModem_stream_read+0x1d4>
    fe6c:	e5951434 	ldr	r1, [r5, #1076]
    fe70:	e3510000 	cmp	r1, #0	; 0x0
    fe74:	0a000037 	beq	ff58 <xyzModem_stream_read+0x1d4>
    fe78:	e5953438 	ldr	r3, [r5, #1080]
    fe7c:	e5950410 	ldr	r0, [r5, #1040]
    fe80:	e0802003 	add	r2, r0, r3
    fe84:	e1520001 	cmp	r2, r1
    fe88:	80803001 	addhi	r3, r0, r1
    fe8c:	80623003 	rsbhi	r3, r2, r3
    fe90:	e5852438 	str	r2, [r5, #1080]
    fe94:	85853410 	strhi	r3, [r5, #1040]
    fe98:	ea00002e 	b	ff58 <xyzModem_stream_read+0x1d4>
    fe9c:	e1510003 	cmp	r1, r3
    fea0:	13e06007 	mvnne	r6, #7	; 0x7
    fea4:	1a00001a 	bne	ff14 <xyzModem_stream_read+0x190>
    fea8:	e3a00006 	mov	r0, #6	; 0x6
    feac:	ebfff8d4 	bl	e204 <putc>
    feb0:	ea00001f 	b	ff34 <xyzModem_stream_read+0x1b0>
    feb4:	e3760005 	cmn	r6, #5	; 0x5
    feb8:	0a000021 	beq	ff44 <xyzModem_stream_read+0x1c0>
    febc:	e3760004 	cmn	r6, #4	; 0x4
    fec0:	1a000013 	bne	ff14 <xyzModem_stream_read+0x190>
    fec4:	e3a00006 	mov	r0, #6	; 0x6
    fec8:	ebfff8cd 	bl	e204 <putc>
    fecc:	e5953414 	ldr	r3, [r5, #1044]
    fed0:	e3530002 	cmp	r3, #2	; 0x2
    fed4:	1a00000b 	bne	ff08 <xyzModem_stream_read+0x184>
    fed8:	e5950428 	ldr	r0, [r5, #1064]
    fedc:	e3500000 	cmp	r0, #0	; 0x0
    fee0:	13a00043 	movne	r0, #67	; 0x43
    fee4:	03a00015 	moveq	r0, #21	; 0x15
    fee8:	ebfff8c5 	bl	e204 <putc>
    feec:	e5953418 	ldr	r3, [r5, #1048]
    fef0:	e2833001 	add	r3, r3, #1	; 0x1
    fef4:	e5853418 	str	r3, [r5, #1048]
    fef8:	ebfffef0 	bl	fac0 <xyzModem_get_hdr>
    fefc:	e1a06000 	mov	r6, r0
    ff00:	e3a00006 	mov	r0, #6	; 0x6
    ff04:	ebfff8be 	bl	e204 <putc>
    ff08:	e3a03001 	mov	r3, #1	; 0x1
    ff0c:	e585342c 	str	r3, [r5, #1068]
    ff10:	ea000009 	b	ff3c <xyzModem_stream_read+0x1b8>
    ff14:	e5950428 	ldr	r0, [r5, #1064]
    ff18:	e3500000 	cmp	r0, #0	; 0x0
    ff1c:	13a00043 	movne	r0, #67	; 0x43
    ff20:	03a00015 	moveq	r0, #21	; 0x15
    ff24:	ebfff8b6 	bl	e204 <putc>
    ff28:	e5953418 	ldr	r3, [r5, #1048]
    ff2c:	e2833001 	add	r3, r3, #1	; 0x1
    ff30:	e5853418 	str	r3, [r5, #1048]
    ff34:	e2544001 	subs	r4, r4, #1	; 0x1
    ff38:	2affffa0 	bcs	fdc0 <xyzModem_stream_read+0x3c>
    ff3c:	e3560000 	cmp	r6, #0	; 0x0
    ff40:	aa000004 	bge	ff58 <xyzModem_stream_read+0x1d4>
    ff44:	e59f3084 	ldr	r3, [pc, #132]	; ffd0 <xyzModem_stream_read+0x24c>
    ff48:	e3e02000 	mvn	r2, #0	; 0x0
    ff4c:	e58b6000 	str	r6, [fp]
    ff50:	e5832410 	str	r2, [r3, #1040]
    ff54:	ea00001a 	b	ffc4 <xyzModem_stream_read+0x240>
    ff58:	e595342c 	ldr	r3, [r5, #1068]
    ff5c:	e3530000 	cmp	r3, #0	; 0x0
    ff60:	1a000010 	bne	ffa8 <xyzModem_stream_read+0x224>
    ff64:	e5954410 	ldr	r4, [r5, #1040]
    ff68:	e1a00008 	mov	r0, r8
    ff6c:	e1570004 	cmp	r7, r4
    ff70:	b1a04007 	movlt	r4, r7
    ff74:	a1a04004 	movge	r4, r4
    ff78:	e1a02004 	mov	r2, r4
    ff7c:	e5951404 	ldr	r1, [r5, #1028]
    ff80:	ebffc51e 	bl	1400 <memcpy>
    ff84:	e5953410 	ldr	r3, [r5, #1040]
    ff88:	e5952404 	ldr	r2, [r5, #1028]
    ff8c:	e0643003 	rsb	r3, r4, r3
    ff90:	e0822004 	add	r2, r2, r4
    ff94:	e5853410 	str	r3, [r5, #1040]
    ff98:	e5852404 	str	r2, [r5, #1028]
    ff9c:	e0647007 	rsb	r7, r4, r7
    ffa0:	e0888004 	add	r8, r8, r4
    ffa4:	e08aa004 	add	sl, sl, r4
    ffa8:	e595342c 	ldr	r3, [r5, #1068]
    ffac:	e2733001 	rsbs	r3, r3, #1	; 0x1
    ffb0:	33a03000 	movcc	r3, #0	; 0x0
    ffb4:	e3570000 	cmp	r7, #0	; 0x0
    ffb8:	d3a03000 	movle	r3, #0	; 0x0
    ffbc:	e3530000 	cmp	r3, #0	; 0x0
    ffc0:	1affff79 	bne	fdac <xyzModem_stream_read+0x28>
    ffc4:	e1a0000a 	mov	r0, sl
    ffc8:	e28dd004 	add	sp, sp, #4	; 0x4
    ffcc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ffd0:	0800d328 	.word	0x0800d328

0000ffd4 <xyzModem_stream_open>:
    ffd4:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    ffd8:	e59f4140 	ldr	r4, [pc, #320]	; 10120 <xyzModem_stream_open+0x14c>
    ffdc:	e24dd00c 	sub	sp, sp, #12	; 0xc
    ffe0:	e3a05000 	mov	r5, #0	; 0x0
    ffe4:	e28d3008 	add	r3, sp, #8	; 0x8
    ffe8:	e3a02001 	mov	r2, #1	; 0x1
    ffec:	e5235004 	str	r5, [r3, #-4]!
    fff0:	e5842428 	str	r2, [r4, #1064]
    fff4:	e5845410 	str	r5, [r4, #1040]
    fff8:	e584542c 	str	r5, [r4, #1068]
    fffc:	e5845430 	str	r5, [r4, #1072]
   10000:	e5902004 	ldr	r2, [r0, #4]
   10004:	e5843000 	str	r3, [r4]
   10008:	e3a00043 	mov	r0, #67	; 0x43
   1000c:	e5842414 	str	r2, [r4, #1044]
   10010:	e5845418 	str	r5, [r4, #1048]
   10014:	e584541c 	str	r5, [r4, #1052]
   10018:	e5845420 	str	r5, [r4, #1056]
   1001c:	e5845424 	str	r5, [r4, #1060]
   10020:	e5845438 	str	r5, [r4, #1080]
   10024:	e5845434 	str	r5, [r4, #1076]
   10028:	e1a08001 	mov	r8, r1
   1002c:	ebfff874 	bl	e204 <putc>
   10030:	e5943414 	ldr	r3, [r4, #1044]
   10034:	e3530001 	cmp	r3, #1	; 0x1
   10038:	05c4340c 	strbeq	r3, [r4, #1036]
   1003c:	01a00005 	moveq	r0, r5
   10040:	0a000034 	beq	10118 <xyzModem_stream_open+0x144>
   10044:	e1a07004 	mov	r7, r4
   10048:	e3a06014 	mov	r6, #20	; 0x14
   1004c:	e3a0400a 	mov	r4, #10	; 0xa
   10050:	e1a0a005 	mov	sl, r5
   10054:	ea00002b 	b	10108 <xyzModem_stream_open+0x134>
   10058:	ebfffe98 	bl	fac0 <xyzModem_get_hdr>
   1005c:	e2505000 	subs	r5, r0, #0	; 0x0
   10060:	1a000016 	bne	100c0 <xyzModem_stream_open+0xec>
   10064:	e59f20b4 	ldr	r2, [pc, #180]	; 10120 <xyzModem_stream_open+0x14c>
   10068:	e5d23408 	ldrb	r3, [r2, #1032]
   1006c:	e3530000 	cmp	r3, #0	; 0x0
   10070:	1a00000b 	bne	100a4 <xyzModem_stream_open+0xd0>
   10074:	e1a03002 	mov	r3, r2
   10078:	e5930404 	ldr	r0, [r3, #1028]
   1007c:	e4d02001 	ldrb	r2, [r0], #1
   10080:	e3520000 	cmp	r2, #0	; 0x0
   10084:	e5830404 	str	r0, [r3, #1028]
   10088:	1afffffa 	bne	10078 <xyzModem_stream_open+0xa4>
   1008c:	e59f3090 	ldr	r3, [pc, #144]	; 10124 <xyzModem_stream_open+0x150>
   10090:	e59f1090 	ldr	r1, [pc, #144]	; 10128 <xyzModem_stream_open+0x154>
   10094:	ebfffb5a 	bl	ee04 <parse_num>
   10098:	e59f3080 	ldr	r3, [pc, #128]	; 10120 <xyzModem_stream_open+0x14c>
   1009c:	e3a02001 	mov	r2, #1	; 0x1
   100a0:	e5832430 	str	r2, [r3, #1072]
   100a4:	e59f3074 	ldr	r3, [pc, #116]	; 10120 <xyzModem_stream_open+0x14c>
   100a8:	e3a01000 	mov	r1, #0	; 0x0
   100ac:	e3a02001 	mov	r2, #1	; 0x1
   100b0:	e5c3240c 	strb	r2, [r3, #1036]
   100b4:	e5831410 	str	r1, [r3, #1040]
   100b8:	e1a00001 	mov	r0, r1
   100bc:	ea000015 	b	10118 <xyzModem_stream_open+0x144>
   100c0:	e3750003 	cmn	r5, #3	; 0x3
   100c4:	1a00000d 	bne	10100 <xyzModem_stream_open+0x12c>
   100c8:	e2444001 	sub	r4, r4, #1	; 0x1
   100cc:	e3540000 	cmp	r4, #0	; 0x0
   100d0:	e59f0054 	ldr	r0, [pc, #84]	; 1012c <xyzModem_stream_open+0x158>
   100d4:	d587a428 	strle	sl, [r7, #1064]
   100d8:	ebffe20b 	bl	890c <udelay>
   100dc:	e5970428 	ldr	r0, [r7, #1064]
   100e0:	e3500000 	cmp	r0, #0	; 0x0
   100e4:	13a00043 	movne	r0, #67	; 0x43
   100e8:	03a00015 	moveq	r0, #21	; 0x15
   100ec:	ebfff844 	bl	e204 <putc>
   100f0:	e5973418 	ldr	r3, [r7, #1048]
   100f4:	e2833001 	add	r3, r3, #1	; 0x1
   100f8:	e5873418 	str	r3, [r7, #1048]
   100fc:	ea000001 	b	10108 <xyzModem_stream_open+0x134>
   10100:	e3750005 	cmn	r5, #5	; 0x5
   10104:	0a000001 	beq	10110 <xyzModem_stream_open+0x13c>
   10108:	e2566001 	subs	r6, r6, #1	; 0x1
   1010c:	2affffd1 	bcs	10058 <xyzModem_stream_open+0x84>
   10110:	e5885000 	str	r5, [r8]
   10114:	e3e00000 	mvn	r0, #0	; 0x0
   10118:	e28dd00c 	add	sp, sp, #12	; 0xc
   1011c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
   10120:	0800d328 	.word	0x0800d328
   10124:	00014256 	.word	0x00014256
   10128:	0800d75c 	.word	0x0800d75c
   1012c:	0007a120 	.word	0x0007a120

00010130 <cyg_crc16>:
   10130:	e3a02000 	mov	r2, #0	; 0x0
   10134:	e92d4010 	push	{r4, lr}
   10138:	e1a0c000 	mov	ip, r0
   1013c:	e59f402c 	ldr	r4, [pc, #44]	; 10170 <cyg_crc16+0x40>
   10140:	e1a00002 	mov	r0, r2
   10144:	ea000006 	b	10164 <cyg_crc16+0x34>
   10148:	e7dc3002 	ldrb	r3, [ip, r2]
   1014c:	e2822001 	add	r2, r2, #1	; 0x1
   10150:	e0233420 	eor	r3, r3, r0, lsr #8
   10154:	e1a03083 	lsl	r3, r3, #1
   10158:	e19430b3 	ldrh	r3, [r4, r3]
   1015c:	e0233400 	eor	r3, r3, r0, lsl #8
   10160:	e6ff0073 	uxth	r0, r3
   10164:	e1520001 	cmp	r2, r1
   10168:	bafffff6 	blt	10148 <cyg_crc16+0x18>
   1016c:	e8bd8010 	pop	{r4, pc}
   10170:	000116a0 	.word	0x000116a0

00010174 <ss_jtag_en>:
   10174:	e59f300c 	ldr	r3, [pc, #12]	; 10188 <ss_jtag_en+0x14>
   10178:	e2500000 	subs	r0, r0, #0	; 0x0
   1017c:	13a00001 	movne	r0, #1	; 0x1
   10180:	e58308b8 	str	r0, [r3, #2232]
   10184:	e12fff1e 	bx	lr
   10188:	2d008000 	.word	0x2d008000

0001018c <raise>:
   1018c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   10190:	e1a01000 	mov	r1, r0
   10194:	e24dd004 	sub	sp, sp, #4	; 0x4
   10198:	e59f0014 	ldr	r0, [pc, #20]	; 101b4 <raise+0x28>
   1019c:	e59f3014 	ldr	r3, [pc, #20]	; 101b8 <raise+0x2c>
   101a0:	e1a0e00f 	mov	lr, pc
   101a4:	e593f000 	ldr	pc, [r3]
   101a8:	e3a00000 	mov	r0, #0	; 0x0
   101ac:	e28dd004 	add	sp, sp, #4	; 0x4
   101b0:	e8bd8000 	pop	{pc}
   101b4:	000144f2 	.word	0x000144f2
   101b8:	0800d764 	.word	0x0800d764

000101bc <__aeabi_uidiv>:
   101bc:	e2512001 	subs	r2, r1, #1	; 0x1
   101c0:	012fff1e 	bxeq	lr
   101c4:	3a000074 	bcc	1039c <__aeabi_uidiv+0x1e0>
   101c8:	e1500001 	cmp	r0, r1
   101cc:	9a00006b 	bls	10380 <__aeabi_uidiv+0x1c4>
   101d0:	e1110002 	tst	r1, r2
   101d4:	0a00006c 	beq	1038c <__aeabi_uidiv+0x1d0>
   101d8:	e16f3f10 	clz	r3, r0
   101dc:	e16f2f11 	clz	r2, r1
   101e0:	e0423003 	sub	r3, r2, r3
   101e4:	e273301f 	rsbs	r3, r3, #31	; 0x1f
   101e8:	10833083 	addne	r3, r3, r3, lsl #1
   101ec:	e3a02000 	mov	r2, #0	; 0x0
   101f0:	108ff103 	addne	pc, pc, r3, lsl #2
   101f4:	e1a00000 	nop			(mov r0,r0)
   101f8:	e1500f81 	cmp	r0, r1, lsl #31
   101fc:	e0a22002 	adc	r2, r2, r2
   10200:	20400f81 	subcs	r0, r0, r1, lsl #31
   10204:	e1500f01 	cmp	r0, r1, lsl #30
   10208:	e0a22002 	adc	r2, r2, r2
   1020c:	20400f01 	subcs	r0, r0, r1, lsl #30
   10210:	e1500e81 	cmp	r0, r1, lsl #29
   10214:	e0a22002 	adc	r2, r2, r2
   10218:	20400e81 	subcs	r0, r0, r1, lsl #29
   1021c:	e1500e01 	cmp	r0, r1, lsl #28
   10220:	e0a22002 	adc	r2, r2, r2
   10224:	20400e01 	subcs	r0, r0, r1, lsl #28
   10228:	e1500d81 	cmp	r0, r1, lsl #27
   1022c:	e0a22002 	adc	r2, r2, r2
   10230:	20400d81 	subcs	r0, r0, r1, lsl #27
   10234:	e1500d01 	cmp	r0, r1, lsl #26
   10238:	e0a22002 	adc	r2, r2, r2
   1023c:	20400d01 	subcs	r0, r0, r1, lsl #26
   10240:	e1500c81 	cmp	r0, r1, lsl #25
   10244:	e0a22002 	adc	r2, r2, r2
   10248:	20400c81 	subcs	r0, r0, r1, lsl #25
   1024c:	e1500c01 	cmp	r0, r1, lsl #24
   10250:	e0a22002 	adc	r2, r2, r2
   10254:	20400c01 	subcs	r0, r0, r1, lsl #24
   10258:	e1500b81 	cmp	r0, r1, lsl #23
   1025c:	e0a22002 	adc	r2, r2, r2
   10260:	20400b81 	subcs	r0, r0, r1, lsl #23
   10264:	e1500b01 	cmp	r0, r1, lsl #22
   10268:	e0a22002 	adc	r2, r2, r2
   1026c:	20400b01 	subcs	r0, r0, r1, lsl #22
   10270:	e1500a81 	cmp	r0, r1, lsl #21
   10274:	e0a22002 	adc	r2, r2, r2
   10278:	20400a81 	subcs	r0, r0, r1, lsl #21
   1027c:	e1500a01 	cmp	r0, r1, lsl #20
   10280:	e0a22002 	adc	r2, r2, r2
   10284:	20400a01 	subcs	r0, r0, r1, lsl #20
   10288:	e1500981 	cmp	r0, r1, lsl #19
   1028c:	e0a22002 	adc	r2, r2, r2
   10290:	20400981 	subcs	r0, r0, r1, lsl #19
   10294:	e1500901 	cmp	r0, r1, lsl #18
   10298:	e0a22002 	adc	r2, r2, r2
   1029c:	20400901 	subcs	r0, r0, r1, lsl #18
   102a0:	e1500881 	cmp	r0, r1, lsl #17
   102a4:	e0a22002 	adc	r2, r2, r2
   102a8:	20400881 	subcs	r0, r0, r1, lsl #17
   102ac:	e1500801 	cmp	r0, r1, lsl #16
   102b0:	e0a22002 	adc	r2, r2, r2
   102b4:	20400801 	subcs	r0, r0, r1, lsl #16
   102b8:	e1500781 	cmp	r0, r1, lsl #15
   102bc:	e0a22002 	adc	r2, r2, r2
   102c0:	20400781 	subcs	r0, r0, r1, lsl #15
   102c4:	e1500701 	cmp	r0, r1, lsl #14
   102c8:	e0a22002 	adc	r2, r2, r2
   102cc:	20400701 	subcs	r0, r0, r1, lsl #14
   102d0:	e1500681 	cmp	r0, r1, lsl #13
   102d4:	e0a22002 	adc	r2, r2, r2
   102d8:	20400681 	subcs	r0, r0, r1, lsl #13
   102dc:	e1500601 	cmp	r0, r1, lsl #12
   102e0:	e0a22002 	adc	r2, r2, r2
   102e4:	20400601 	subcs	r0, r0, r1, lsl #12
   102e8:	e1500581 	cmp	r0, r1, lsl #11
   102ec:	e0a22002 	adc	r2, r2, r2
   102f0:	20400581 	subcs	r0, r0, r1, lsl #11
   102f4:	e1500501 	cmp	r0, r1, lsl #10
   102f8:	e0a22002 	adc	r2, r2, r2
   102fc:	20400501 	subcs	r0, r0, r1, lsl #10
   10300:	e1500481 	cmp	r0, r1, lsl #9
   10304:	e0a22002 	adc	r2, r2, r2
   10308:	20400481 	subcs	r0, r0, r1, lsl #9
   1030c:	e1500401 	cmp	r0, r1, lsl #8
   10310:	e0a22002 	adc	r2, r2, r2
   10314:	20400401 	subcs	r0, r0, r1, lsl #8
   10318:	e1500381 	cmp	r0, r1, lsl #7
   1031c:	e0a22002 	adc	r2, r2, r2
   10320:	20400381 	subcs	r0, r0, r1, lsl #7
   10324:	e1500301 	cmp	r0, r1, lsl #6
   10328:	e0a22002 	adc	r2, r2, r2
   1032c:	20400301 	subcs	r0, r0, r1, lsl #6
   10330:	e1500281 	cmp	r0, r1, lsl #5
   10334:	e0a22002 	adc	r2, r2, r2
   10338:	20400281 	subcs	r0, r0, r1, lsl #5
   1033c:	e1500201 	cmp	r0, r1, lsl #4
   10340:	e0a22002 	adc	r2, r2, r2
   10344:	20400201 	subcs	r0, r0, r1, lsl #4
   10348:	e1500181 	cmp	r0, r1, lsl #3
   1034c:	e0a22002 	adc	r2, r2, r2
   10350:	20400181 	subcs	r0, r0, r1, lsl #3
   10354:	e1500101 	cmp	r0, r1, lsl #2
   10358:	e0a22002 	adc	r2, r2, r2
   1035c:	20400101 	subcs	r0, r0, r1, lsl #2
   10360:	e1500081 	cmp	r0, r1, lsl #1
   10364:	e0a22002 	adc	r2, r2, r2
   10368:	20400081 	subcs	r0, r0, r1, lsl #1
   1036c:	e1500001 	cmp	r0, r1
   10370:	e0a22002 	adc	r2, r2, r2
   10374:	20400001 	subcs	r0, r0, r1
   10378:	e1a00002 	mov	r0, r2
   1037c:	e12fff1e 	bx	lr
   10380:	03a00001 	moveq	r0, #1	; 0x1
   10384:	13a00000 	movne	r0, #0	; 0x0
   10388:	e12fff1e 	bx	lr
   1038c:	e16f2f11 	clz	r2, r1
   10390:	e262201f 	rsb	r2, r2, #31	; 0x1f
   10394:	e1a00230 	lsr	r0, r0, r2
   10398:	e12fff1e 	bx	lr
   1039c:	e52de008 	str	lr, [sp, #-8]!
   103a0:	eb0002c4 	bl	10eb8 <__div0>
   103a4:	e3a00000 	mov	r0, #0	; 0x0
   103a8:	e49df008 	ldr	pc, [sp], #8

000103ac <__aeabi_uidivmod>:
   103ac:	e92d4003 	push	{r0, r1, lr}
   103b0:	ebffff81 	bl	101bc <__aeabi_uidiv>
   103b4:	e8bd4006 	pop	{r1, r2, lr}
   103b8:	e0030092 	mul	r3, r2, r0
   103bc:	e0411003 	sub	r1, r1, r3
   103c0:	e12fff1e 	bx	lr

000103c4 <__aeabi_idiv>:
   103c4:	e3510000 	cmp	r1, #0	; 0x0
   103c8:	e020c001 	eor	ip, r0, r1
   103cc:	0a000080 	beq	105d4 <__aeabi_idiv+0x210>
   103d0:	42611000 	rsbmi	r1, r1, #0	; 0x0
   103d4:	e2512001 	subs	r2, r1, #1	; 0x1
   103d8:	0a000070 	beq	105a0 <__aeabi_idiv+0x1dc>
   103dc:	e1b03000 	movs	r3, r0
   103e0:	42603000 	rsbmi	r3, r0, #0	; 0x0
   103e4:	e1530001 	cmp	r3, r1
   103e8:	9a00006f 	bls	105ac <__aeabi_idiv+0x1e8>
   103ec:	e1110002 	tst	r1, r2
   103f0:	0a000071 	beq	105bc <__aeabi_idiv+0x1f8>
   103f4:	e16f2f13 	clz	r2, r3
   103f8:	e16f0f11 	clz	r0, r1
   103fc:	e0402002 	sub	r2, r0, r2
   10400:	e272201f 	rsbs	r2, r2, #31	; 0x1f
   10404:	10822082 	addne	r2, r2, r2, lsl #1
   10408:	e3a00000 	mov	r0, #0	; 0x0
   1040c:	108ff102 	addne	pc, pc, r2, lsl #2
   10410:	e1a00000 	nop			(mov r0,r0)
   10414:	e1530f81 	cmp	r3, r1, lsl #31
   10418:	e0a00000 	adc	r0, r0, r0
   1041c:	20433f81 	subcs	r3, r3, r1, lsl #31
   10420:	e1530f01 	cmp	r3, r1, lsl #30
   10424:	e0a00000 	adc	r0, r0, r0
   10428:	20433f01 	subcs	r3, r3, r1, lsl #30
   1042c:	e1530e81 	cmp	r3, r1, lsl #29
   10430:	e0a00000 	adc	r0, r0, r0
   10434:	20433e81 	subcs	r3, r3, r1, lsl #29
   10438:	e1530e01 	cmp	r3, r1, lsl #28
   1043c:	e0a00000 	adc	r0, r0, r0
   10440:	20433e01 	subcs	r3, r3, r1, lsl #28
   10444:	e1530d81 	cmp	r3, r1, lsl #27
   10448:	e0a00000 	adc	r0, r0, r0
   1044c:	20433d81 	subcs	r3, r3, r1, lsl #27
   10450:	e1530d01 	cmp	r3, r1, lsl #26
   10454:	e0a00000 	adc	r0, r0, r0
   10458:	20433d01 	subcs	r3, r3, r1, lsl #26
   1045c:	e1530c81 	cmp	r3, r1, lsl #25
   10460:	e0a00000 	adc	r0, r0, r0
   10464:	20433c81 	subcs	r3, r3, r1, lsl #25
   10468:	e1530c01 	cmp	r3, r1, lsl #24
   1046c:	e0a00000 	adc	r0, r0, r0
   10470:	20433c01 	subcs	r3, r3, r1, lsl #24
   10474:	e1530b81 	cmp	r3, r1, lsl #23
   10478:	e0a00000 	adc	r0, r0, r0
   1047c:	20433b81 	subcs	r3, r3, r1, lsl #23
   10480:	e1530b01 	cmp	r3, r1, lsl #22
   10484:	e0a00000 	adc	r0, r0, r0
   10488:	20433b01 	subcs	r3, r3, r1, lsl #22
   1048c:	e1530a81 	cmp	r3, r1, lsl #21
   10490:	e0a00000 	adc	r0, r0, r0
   10494:	20433a81 	subcs	r3, r3, r1, lsl #21
   10498:	e1530a01 	cmp	r3, r1, lsl #20
   1049c:	e0a00000 	adc	r0, r0, r0
   104a0:	20433a01 	subcs	r3, r3, r1, lsl #20
   104a4:	e1530981 	cmp	r3, r1, lsl #19
   104a8:	e0a00000 	adc	r0, r0, r0
   104ac:	20433981 	subcs	r3, r3, r1, lsl #19
   104b0:	e1530901 	cmp	r3, r1, lsl #18
   104b4:	e0a00000 	adc	r0, r0, r0
   104b8:	20433901 	subcs	r3, r3, r1, lsl #18
   104bc:	e1530881 	cmp	r3, r1, lsl #17
   104c0:	e0a00000 	adc	r0, r0, r0
   104c4:	20433881 	subcs	r3, r3, r1, lsl #17
   104c8:	e1530801 	cmp	r3, r1, lsl #16
   104cc:	e0a00000 	adc	r0, r0, r0
   104d0:	20433801 	subcs	r3, r3, r1, lsl #16
   104d4:	e1530781 	cmp	r3, r1, lsl #15
   104d8:	e0a00000 	adc	r0, r0, r0
   104dc:	20433781 	subcs	r3, r3, r1, lsl #15
   104e0:	e1530701 	cmp	r3, r1, lsl #14
   104e4:	e0a00000 	adc	r0, r0, r0
   104e8:	20433701 	subcs	r3, r3, r1, lsl #14
   104ec:	e1530681 	cmp	r3, r1, lsl #13
   104f0:	e0a00000 	adc	r0, r0, r0
   104f4:	20433681 	subcs	r3, r3, r1, lsl #13
   104f8:	e1530601 	cmp	r3, r1, lsl #12
   104fc:	e0a00000 	adc	r0, r0, r0
   10500:	20433601 	subcs	r3, r3, r1, lsl #12
   10504:	e1530581 	cmp	r3, r1, lsl #11
   10508:	e0a00000 	adc	r0, r0, r0
   1050c:	20433581 	subcs	r3, r3, r1, lsl #11
   10510:	e1530501 	cmp	r3, r1, lsl #10
   10514:	e0a00000 	adc	r0, r0, r0
   10518:	20433501 	subcs	r3, r3, r1, lsl #10
   1051c:	e1530481 	cmp	r3, r1, lsl #9
   10520:	e0a00000 	adc	r0, r0, r0
   10524:	20433481 	subcs	r3, r3, r1, lsl #9
   10528:	e1530401 	cmp	r3, r1, lsl #8
   1052c:	e0a00000 	adc	r0, r0, r0
   10530:	20433401 	subcs	r3, r3, r1, lsl #8
   10534:	e1530381 	cmp	r3, r1, lsl #7
   10538:	e0a00000 	adc	r0, r0, r0
   1053c:	20433381 	subcs	r3, r3, r1, lsl #7
   10540:	e1530301 	cmp	r3, r1, lsl #6
   10544:	e0a00000 	adc	r0, r0, r0
   10548:	20433301 	subcs	r3, r3, r1, lsl #6
   1054c:	e1530281 	cmp	r3, r1, lsl #5
   10550:	e0a00000 	adc	r0, r0, r0
   10554:	20433281 	subcs	r3, r3, r1, lsl #5
   10558:	e1530201 	cmp	r3, r1, lsl #4
   1055c:	e0a00000 	adc	r0, r0, r0
   10560:	20433201 	subcs	r3, r3, r1, lsl #4
   10564:	e1530181 	cmp	r3, r1, lsl #3
   10568:	e0a00000 	adc	r0, r0, r0
   1056c:	20433181 	subcs	r3, r3, r1, lsl #3
   10570:	e1530101 	cmp	r3, r1, lsl #2
   10574:	e0a00000 	adc	r0, r0, r0
   10578:	20433101 	subcs	r3, r3, r1, lsl #2
   1057c:	e1530081 	cmp	r3, r1, lsl #1
   10580:	e0a00000 	adc	r0, r0, r0
   10584:	20433081 	subcs	r3, r3, r1, lsl #1
   10588:	e1530001 	cmp	r3, r1
   1058c:	e0a00000 	adc	r0, r0, r0
   10590:	20433001 	subcs	r3, r3, r1
   10594:	e35c0000 	cmp	ip, #0	; 0x0
   10598:	42600000 	rsbmi	r0, r0, #0	; 0x0
   1059c:	e12fff1e 	bx	lr
   105a0:	e13c0000 	teq	ip, r0
   105a4:	42600000 	rsbmi	r0, r0, #0	; 0x0
   105a8:	e12fff1e 	bx	lr
   105ac:	33a00000 	movcc	r0, #0	; 0x0
   105b0:	01a00fcc 	asreq	r0, ip, #31
   105b4:	03800001 	orreq	r0, r0, #1	; 0x1
   105b8:	e12fff1e 	bx	lr
   105bc:	e16f2f11 	clz	r2, r1
   105c0:	e262201f 	rsb	r2, r2, #31	; 0x1f
   105c4:	e35c0000 	cmp	ip, #0	; 0x0
   105c8:	e1a00233 	lsr	r0, r3, r2
   105cc:	42600000 	rsbmi	r0, r0, #0	; 0x0
   105d0:	e12fff1e 	bx	lr
   105d4:	e52de008 	str	lr, [sp, #-8]!
   105d8:	eb000236 	bl	10eb8 <__div0>
   105dc:	e3a00000 	mov	r0, #0	; 0x0
   105e0:	e49df008 	ldr	pc, [sp], #8

000105e4 <__aeabi_idivmod>:
   105e4:	e92d4003 	push	{r0, r1, lr}
   105e8:	ebffff75 	bl	103c4 <__aeabi_idiv>
   105ec:	e8bd4006 	pop	{r1, r2, lr}
   105f0:	e0030092 	mul	r3, r2, r0
   105f4:	e0411003 	sub	r1, r1, r3
   105f8:	e12fff1e 	bx	lr

000105fc <__aeabi_llsr>:
   105fc:	e2523020 	subs	r3, r2, #32	; 0x20
   10600:	e262c020 	rsb	ip, r2, #32	; 0x20
   10604:	41a00230 	lsrmi	r0, r0, r2
   10608:	51a00331 	lsrpl	r0, r1, r3
   1060c:	41800c11 	orrmi	r0, r0, r1, lsl ip
   10610:	e1a01231 	lsr	r1, r1, r2
   10614:	e12fff1e 	bx	lr

00010618 <__aeabi_llsl>:
   10618:	e2523020 	subs	r3, r2, #32	; 0x20
   1061c:	e262c020 	rsb	ip, r2, #32	; 0x20
   10620:	41a01211 	lslmi	r1, r1, r2
   10624:	51a01310 	lslpl	r1, r0, r3
   10628:	41811c30 	orrmi	r1, r1, r0, lsr ip
   1062c:	e1a00210 	lsl	r0, r0, r2
   10630:	e12fff1e 	bx	lr

00010634 <__aeabi_drsub>:
   10634:	e2211102 	eor	r1, r1, #-2147483648	; 0x80000000
   10638:	ea000000 	b	10640 <__adddf3>

0001063c <__aeabi_dsub>:
   1063c:	e2233102 	eor	r3, r3, #-2147483648	; 0x80000000

00010640 <__adddf3>:
   10640:	e92d4030 	push	{r4, r5, lr}
   10644:	e1a04081 	lsl	r4, r1, #1
   10648:	e1a05083 	lsl	r5, r3, #1
   1064c:	e1340005 	teq	r4, r5
   10650:	01300002 	teqeq	r0, r2
   10654:	1194c000 	orrsne	ip, r4, r0
   10658:	1195c002 	orrsne	ip, r5, r2
   1065c:	11f0cac4 	mvnsne	ip, r4, asr #21
   10660:	11f0cac5 	mvnsne	ip, r5, asr #21
   10664:	0a000079 	beq	10850 <__adddf3+0x210>
   10668:	e1a04aa4 	lsr	r4, r4, #21
   1066c:	e0745aa5 	rsbs	r5, r4, r5, lsr #21
   10670:	b2655000 	rsblt	r5, r5, #0	; 0x0
   10674:	da000006 	ble	10694 <__adddf3+0x54>
   10678:	e0844005 	add	r4, r4, r5
   1067c:	e0202002 	eor	r2, r0, r2
   10680:	e0213003 	eor	r3, r1, r3
   10684:	e0220000 	eor	r0, r2, r0
   10688:	e0231001 	eor	r1, r3, r1
   1068c:	e0202002 	eor	r2, r0, r2
   10690:	e0213003 	eor	r3, r1, r3
   10694:	e3550036 	cmp	r5, #54	; 0x36
   10698:	88bd8030 	pophi	{r4, r5, pc}
   1069c:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   106a0:	e1a01601 	lsl	r1, r1, #12
   106a4:	e3a0c601 	mov	ip, #1048576	; 0x100000
   106a8:	e18c1621 	orr	r1, ip, r1, lsr #12
   106ac:	0a000001 	beq	106b8 <__adddf3+0x78>
   106b0:	e2700000 	rsbs	r0, r0, #0	; 0x0
   106b4:	e2e11000 	rsc	r1, r1, #0	; 0x0
   106b8:	e3130102 	tst	r3, #-2147483648	; 0x80000000
   106bc:	e1a03603 	lsl	r3, r3, #12
   106c0:	e18c3623 	orr	r3, ip, r3, lsr #12
   106c4:	0a000001 	beq	106d0 <__adddf3+0x90>
   106c8:	e2722000 	rsbs	r2, r2, #0	; 0x0
   106cc:	e2e33000 	rsc	r3, r3, #0	; 0x0
   106d0:	e1340005 	teq	r4, r5
   106d4:	0a000057 	beq	10838 <__adddf3+0x1f8>
   106d8:	e2444001 	sub	r4, r4, #1	; 0x1
   106dc:	e275e020 	rsbs	lr, r5, #32	; 0x20
   106e0:	ba000005 	blt	106fc <__adddf3+0xbc>
   106e4:	e1a0ce12 	lsl	ip, r2, lr
   106e8:	e0900532 	adds	r0, r0, r2, lsr r5
   106ec:	e2a11000 	adc	r1, r1, #0	; 0x0
   106f0:	e0900e13 	adds	r0, r0, r3, lsl lr
   106f4:	e0b11553 	adcs	r1, r1, r3, asr r5
   106f8:	ea000006 	b	10718 <__adddf3+0xd8>
   106fc:	e2455020 	sub	r5, r5, #32	; 0x20
   10700:	e28ee020 	add	lr, lr, #32	; 0x20
   10704:	e3520001 	cmp	r2, #1	; 0x1
   10708:	e1a0ce13 	lsl	ip, r3, lr
   1070c:	238cc002 	orrcs	ip, ip, #2	; 0x2
   10710:	e0900553 	adds	r0, r0, r3, asr r5
   10714:	e0b11fc3 	adcs	r1, r1, r3, asr #31
   10718:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   1071c:	5a000002 	bpl	1072c <__adddf3+0xec>
   10720:	e27cc000 	rsbs	ip, ip, #0	; 0x0
   10724:	e2f00000 	rscs	r0, r0, #0	; 0x0
   10728:	e2e11000 	rsc	r1, r1, #0	; 0x0
   1072c:	e3510601 	cmp	r1, #1048576	; 0x100000
   10730:	3a00000e 	bcc	10770 <__adddf3+0x130>
   10734:	e3510602 	cmp	r1, #2097152	; 0x200000
   10738:	3a000006 	bcc	10758 <__adddf3+0x118>
   1073c:	e1b010a1 	lsrs	r1, r1, #1
   10740:	e1b00060 	rrxs	r0, r0
   10744:	e1a0c06c 	rrx	ip, ip
   10748:	e2844001 	add	r4, r4, #1	; 0x1
   1074c:	e1a02a84 	lsl	r2, r4, #21
   10750:	e3720501 	cmn	r2, #4194304	; 0x400000
   10754:	2a000055 	bcs	108b0 <__adddf3+0x270>
   10758:	e35c0102 	cmp	ip, #-2147483648	; 0x80000000
   1075c:	01b0c0a0 	lsrseq	ip, r0, #1
   10760:	e2b00000 	adcs	r0, r0, #0	; 0x0
   10764:	e0a11a04 	adc	r1, r1, r4, lsl #20
   10768:	e1811005 	orr	r1, r1, r5
   1076c:	e8bd8030 	pop	{r4, r5, pc}
   10770:	e1b0c08c 	lsls	ip, ip, #1
   10774:	e0b00000 	adcs	r0, r0, r0
   10778:	e0a11001 	adc	r1, r1, r1
   1077c:	e3110601 	tst	r1, #1048576	; 0x100000
   10780:	e2444001 	sub	r4, r4, #1	; 0x1
   10784:	1afffff3 	bne	10758 <__adddf3+0x118>
   10788:	e3310000 	teq	r1, #0	; 0x0
   1078c:	01a01000 	moveq	r1, r0
   10790:	03a00000 	moveq	r0, #0	; 0x0
   10794:	e16f3f11 	clz	r3, r1
   10798:	02833020 	addeq	r3, r3, #32	; 0x20
   1079c:	e243300b 	sub	r3, r3, #11	; 0xb
   107a0:	e2532020 	subs	r2, r3, #32	; 0x20
   107a4:	aa000007 	bge	107c8 <__adddf3+0x188>
   107a8:	e292200c 	adds	r2, r2, #12	; 0xc
   107ac:	da000004 	ble	107c4 <__adddf3+0x184>
   107b0:	e282c014 	add	ip, r2, #20	; 0x14
   107b4:	e262200c 	rsb	r2, r2, #12	; 0xc
   107b8:	e1a00c11 	lsl	r0, r1, ip
   107bc:	e1a01231 	lsr	r1, r1, r2
   107c0:	ea000004 	b	107d8 <__adddf3+0x198>
   107c4:	e2822014 	add	r2, r2, #20	; 0x14
   107c8:	d262c020 	rsble	ip, r2, #32	; 0x20
   107cc:	e1a01211 	lsl	r1, r1, r2
   107d0:	d1811c30 	orrle	r1, r1, r0, lsr ip
   107d4:	d1a00210 	lslle	r0, r0, r2
   107d8:	e0544003 	subs	r4, r4, r3
   107dc:	a0811a04 	addge	r1, r1, r4, lsl #20
   107e0:	a1811005 	orrge	r1, r1, r5
   107e4:	a8bd8030 	popge	{r4, r5, pc}
   107e8:	e1e04004 	mvn	r4, r4
   107ec:	e254401f 	subs	r4, r4, #31	; 0x1f
   107f0:	aa00000d 	bge	1082c <__adddf3+0x1ec>
   107f4:	e294400c 	adds	r4, r4, #12	; 0xc
   107f8:	ca000005 	bgt	10814 <__adddf3+0x1d4>
   107fc:	e2844014 	add	r4, r4, #20	; 0x14
   10800:	e2642020 	rsb	r2, r4, #32	; 0x20
   10804:	e1a00430 	lsr	r0, r0, r4
   10808:	e1800211 	orr	r0, r0, r1, lsl r2
   1080c:	e1851431 	orr	r1, r5, r1, lsr r4
   10810:	e8bd8030 	pop	{r4, r5, pc}
   10814:	e264400c 	rsb	r4, r4, #12	; 0xc
   10818:	e2642020 	rsb	r2, r4, #32	; 0x20
   1081c:	e1a00230 	lsr	r0, r0, r2
   10820:	e1800411 	orr	r0, r0, r1, lsl r4
   10824:	e1a01005 	mov	r1, r5
   10828:	e8bd8030 	pop	{r4, r5, pc}
   1082c:	e1a00431 	lsr	r0, r1, r4
   10830:	e1a01005 	mov	r1, r5
   10834:	e8bd8030 	pop	{r4, r5, pc}
   10838:	e3340000 	teq	r4, #0	; 0x0
   1083c:	e2233601 	eor	r3, r3, #1048576	; 0x100000
   10840:	02211601 	eoreq	r1, r1, #1048576	; 0x100000
   10844:	02844001 	addeq	r4, r4, #1	; 0x1
   10848:	12455001 	subne	r5, r5, #1	; 0x1
   1084c:	eaffffa1 	b	106d8 <__adddf3+0x98>
   10850:	e1f0cac4 	mvns	ip, r4, asr #21
   10854:	11f0cac5 	mvnsne	ip, r5, asr #21
   10858:	0a000018 	beq	108c0 <__adddf3+0x280>
   1085c:	e1340005 	teq	r4, r5
   10860:	01300002 	teqeq	r0, r2
   10864:	0a000003 	beq	10878 <__adddf3+0x238>
   10868:	e194c000 	orrs	ip, r4, r0
   1086c:	01a01003 	moveq	r1, r3
   10870:	01a00002 	moveq	r0, r2
   10874:	e8bd8030 	pop	{r4, r5, pc}
   10878:	e1310003 	teq	r1, r3
   1087c:	13a01000 	movne	r1, #0	; 0x0
   10880:	13a00000 	movne	r0, #0	; 0x0
   10884:	18bd8030 	popne	{r4, r5, pc}
   10888:	e1b0caa4 	lsrs	ip, r4, #21
   1088c:	1a000003 	bne	108a0 <__adddf3+0x260>
   10890:	e1b00080 	lsls	r0, r0, #1
   10894:	e0b11001 	adcs	r1, r1, r1
   10898:	23811102 	orrcs	r1, r1, #-2147483648	; 0x80000000
   1089c:	e8bd8030 	pop	{r4, r5, pc}
   108a0:	e2944501 	adds	r4, r4, #4194304	; 0x400000
   108a4:	32811601 	addcc	r1, r1, #1048576	; 0x100000
   108a8:	38bd8030 	popcc	{r4, r5, pc}
   108ac:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   108b0:	e385147f 	orr	r1, r5, #2130706432	; 0x7f000000
   108b4:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   108b8:	e3a00000 	mov	r0, #0	; 0x0
   108bc:	e8bd8030 	pop	{r4, r5, pc}
   108c0:	e1f0cac4 	mvns	ip, r4, asr #21
   108c4:	11a01003 	movne	r1, r3
   108c8:	11a00002 	movne	r0, r2
   108cc:	01f0cac5 	mvnseq	ip, r5, asr #21
   108d0:	11a03001 	movne	r3, r1
   108d4:	11a02000 	movne	r2, r0
   108d8:	e1904601 	orrs	r4, r0, r1, lsl #12
   108dc:	01925603 	orrseq	r5, r2, r3, lsl #12
   108e0:	01310003 	teqeq	r1, r3
   108e4:	13811702 	orrne	r1, r1, #524288	; 0x80000
   108e8:	e8bd8030 	pop	{r4, r5, pc}

000108ec <__aeabi_ui2d>:
   108ec:	e3300000 	teq	r0, #0	; 0x0
   108f0:	03a01000 	moveq	r1, #0	; 0x0
   108f4:	012fff1e 	bxeq	lr
   108f8:	e92d4030 	push	{r4, r5, lr}
   108fc:	e3a04b01 	mov	r4, #1024	; 0x400
   10900:	e2844032 	add	r4, r4, #50	; 0x32
   10904:	e3a05000 	mov	r5, #0	; 0x0
   10908:	e3a01000 	mov	r1, #0	; 0x0
   1090c:	eaffff9d 	b	10788 <__adddf3+0x148>

00010910 <__aeabi_i2d>:
   10910:	e3300000 	teq	r0, #0	; 0x0
   10914:	03a01000 	moveq	r1, #0	; 0x0
   10918:	012fff1e 	bxeq	lr
   1091c:	e92d4030 	push	{r4, r5, lr}
   10920:	e3a04b01 	mov	r4, #1024	; 0x400
   10924:	e2844032 	add	r4, r4, #50	; 0x32
   10928:	e2105102 	ands	r5, r0, #-2147483648	; 0x80000000
   1092c:	42600000 	rsbmi	r0, r0, #0	; 0x0
   10930:	e3a01000 	mov	r1, #0	; 0x0
   10934:	eaffff93 	b	10788 <__adddf3+0x148>

00010938 <__aeabi_f2d>:
   10938:	e1b02080 	lsls	r2, r0, #1
   1093c:	e1a011c2 	asr	r1, r2, #3
   10940:	e1a01061 	rrx	r1, r1
   10944:	e1a00e02 	lsl	r0, r2, #28
   10948:	121234ff 	andsne	r3, r2, #-16777216	; 0xff000000
   1094c:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   10950:	1221130e 	eorne	r1, r1, #939524096	; 0x38000000
   10954:	112fff1e 	bxne	lr
   10958:	e3320000 	teq	r2, #0	; 0x0
   1095c:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   10960:	012fff1e 	bxeq	lr
   10964:	e92d4030 	push	{r4, r5, lr}
   10968:	e3a04d0e 	mov	r4, #896	; 0x380
   1096c:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   10970:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   10974:	eaffff83 	b	10788 <__adddf3+0x148>

00010978 <__aeabi_ul2d>:
   10978:	e1902001 	orrs	r2, r0, r1
   1097c:	012fff1e 	bxeq	lr
   10980:	e92d4030 	push	{r4, r5, lr}
   10984:	e3a05000 	mov	r5, #0	; 0x0
   10988:	ea000006 	b	109a8 <__aeabi_l2d+0x1c>

0001098c <__aeabi_l2d>:
   1098c:	e1902001 	orrs	r2, r0, r1
   10990:	012fff1e 	bxeq	lr
   10994:	e92d4030 	push	{r4, r5, lr}
   10998:	e2115102 	ands	r5, r1, #-2147483648	; 0x80000000
   1099c:	5a000001 	bpl	109a8 <__aeabi_l2d+0x1c>
   109a0:	e2700000 	rsbs	r0, r0, #0	; 0x0
   109a4:	e2e11000 	rsc	r1, r1, #0	; 0x0
   109a8:	e3a04b01 	mov	r4, #1024	; 0x400
   109ac:	e2844032 	add	r4, r4, #50	; 0x32
   109b0:	e1b0cb21 	lsrs	ip, r1, #22
   109b4:	0affff5c 	beq	1072c <__adddf3+0xec>
   109b8:	e3a02003 	mov	r2, #3	; 0x3
   109bc:	e1b0c1ac 	lsrs	ip, ip, #3
   109c0:	12822003 	addne	r2, r2, #3	; 0x3
   109c4:	e1b0c1ac 	lsrs	ip, ip, #3
   109c8:	12822003 	addne	r2, r2, #3	; 0x3
   109cc:	e08221ac 	add	r2, r2, ip, lsr #3
   109d0:	e2623020 	rsb	r3, r2, #32	; 0x20
   109d4:	e1a0c310 	lsl	ip, r0, r3
   109d8:	e1a00230 	lsr	r0, r0, r2
   109dc:	e1800311 	orr	r0, r0, r1, lsl r3
   109e0:	e1a01231 	lsr	r1, r1, r2
   109e4:	e0844002 	add	r4, r4, r2
   109e8:	eaffff4f 	b	1072c <__adddf3+0xec>

000109ec <__aeabi_dmul>:
   109ec:	e92d4070 	push	{r4, r5, r6, lr}
   109f0:	e3a0c0ff 	mov	ip, #255	; 0xff
   109f4:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   109f8:	e01c4a21 	ands	r4, ip, r1, lsr #20
   109fc:	101c5a23 	andsne	r5, ip, r3, lsr #20
   10a00:	1134000c 	teqne	r4, ip
   10a04:	1135000c 	teqne	r5, ip
   10a08:	0b00006f 	bleq	10bcc <__aeabi_dmul+0x1e0>
   10a0c:	e0844005 	add	r4, r4, r5
   10a10:	e0216003 	eor	r6, r1, r3
   10a14:	e1c11a8c 	bic	r1, r1, ip, lsl #21
   10a18:	e1c33a8c 	bic	r3, r3, ip, lsl #21
   10a1c:	e1905601 	orrs	r5, r0, r1, lsl #12
   10a20:	11925603 	orrsne	r5, r2, r3, lsl #12
   10a24:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   10a28:	e3833601 	orr	r3, r3, #1048576	; 0x100000
   10a2c:	0a00001c 	beq	10aa4 <__aeabi_dmul+0xb8>
   10a30:	e08ec290 	umull	ip, lr, r0, r2
   10a34:	e3a05000 	mov	r5, #0	; 0x0
   10a38:	e0a5e291 	umlal	lr, r5, r1, r2
   10a3c:	e2062102 	and	r2, r6, #-2147483648	; 0x80000000
   10a40:	e0a5e390 	umlal	lr, r5, r0, r3
   10a44:	e3a06000 	mov	r6, #0	; 0x0
   10a48:	e0a65391 	umlal	r5, r6, r1, r3
   10a4c:	e33c0000 	teq	ip, #0	; 0x0
   10a50:	138ee001 	orrne	lr, lr, #1	; 0x1
   10a54:	e24440ff 	sub	r4, r4, #255	; 0xff
   10a58:	e3560c02 	cmp	r6, #512	; 0x200
   10a5c:	e2c44c03 	sbc	r4, r4, #768	; 0x300
   10a60:	2a000002 	bcs	10a70 <__aeabi_dmul+0x84>
   10a64:	e1b0e08e 	lsls	lr, lr, #1
   10a68:	e0b55005 	adcs	r5, r5, r5
   10a6c:	e0a66006 	adc	r6, r6, r6
   10a70:	e1821586 	orr	r1, r2, r6, lsl #11
   10a74:	e1811aa5 	orr	r1, r1, r5, lsr #21
   10a78:	e1a00585 	lsl	r0, r5, #11
   10a7c:	e1800aae 	orr	r0, r0, lr, lsr #21
   10a80:	e1a0e58e 	lsl	lr, lr, #11
   10a84:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   10a88:	835c0c07 	cmphi	ip, #1792	; 0x700
   10a8c:	8a00000f 	bhi	10ad0 <__aeabi_dmul+0xe4>
   10a90:	e35e0102 	cmp	lr, #-2147483648	; 0x80000000
   10a94:	01b0e0a0 	lsrseq	lr, r0, #1
   10a98:	e2b00000 	adcs	r0, r0, #0	; 0x0
   10a9c:	e0a11a04 	adc	r1, r1, r4, lsl #20
   10aa0:	e8bd8070 	pop	{r4, r5, r6, pc}
   10aa4:	e2066102 	and	r6, r6, #-2147483648	; 0x80000000
   10aa8:	e1861001 	orr	r1, r6, r1
   10aac:	e1800002 	orr	r0, r0, r2
   10ab0:	e0211003 	eor	r1, r1, r3
   10ab4:	e05440ac 	subs	r4, r4, ip, lsr #1
   10ab8:	c074500c 	rsbsgt	r5, r4, ip
   10abc:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   10ac0:	c8bd8070 	popgt	{r4, r5, r6, pc}
   10ac4:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   10ac8:	e3a0e000 	mov	lr, #0	; 0x0
   10acc:	e2544001 	subs	r4, r4, #1	; 0x1
   10ad0:	ca000058 	bgt	10c38 <__aeabi_dmul+0x24c>
   10ad4:	e3740036 	cmn	r4, #54	; 0x36
   10ad8:	d3a00000 	movle	r0, #0	; 0x0
   10adc:	d2011102 	andle	r1, r1, #-2147483648	; 0x80000000
   10ae0:	d8bd8070 	pople	{r4, r5, r6, pc}
   10ae4:	e2644000 	rsb	r4, r4, #0	; 0x0
   10ae8:	e2544020 	subs	r4, r4, #32	; 0x20
   10aec:	aa000018 	bge	10b54 <__aeabi_dmul+0x168>
   10af0:	e294400c 	adds	r4, r4, #12	; 0xc
   10af4:	ca00000b 	bgt	10b28 <__aeabi_dmul+0x13c>
   10af8:	e2844014 	add	r4, r4, #20	; 0x14
   10afc:	e2645020 	rsb	r5, r4, #32	; 0x20
   10b00:	e1a03510 	lsl	r3, r0, r5
   10b04:	e1a00430 	lsr	r0, r0, r4
   10b08:	e1800511 	orr	r0, r0, r1, lsl r5
   10b0c:	e2012102 	and	r2, r1, #-2147483648	; 0x80000000
   10b10:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   10b14:	e0900fa3 	adds	r0, r0, r3, lsr #31
   10b18:	e0a21431 	adc	r1, r2, r1, lsr r4
   10b1c:	e19ee083 	orrs	lr, lr, r3, lsl #1
   10b20:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   10b24:	e8bd8070 	pop	{r4, r5, r6, pc}
   10b28:	e264400c 	rsb	r4, r4, #12	; 0xc
   10b2c:	e2645020 	rsb	r5, r4, #32	; 0x20
   10b30:	e1a03410 	lsl	r3, r0, r4
   10b34:	e1a00530 	lsr	r0, r0, r5
   10b38:	e1800411 	orr	r0, r0, r1, lsl r4
   10b3c:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10b40:	e0900fa3 	adds	r0, r0, r3, lsr #31
   10b44:	e2a11000 	adc	r1, r1, #0	; 0x0
   10b48:	e19ee083 	orrs	lr, lr, r3, lsl #1
   10b4c:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   10b50:	e8bd8070 	pop	{r4, r5, r6, pc}
   10b54:	e2645020 	rsb	r5, r4, #32	; 0x20
   10b58:	e18ee510 	orr	lr, lr, r0, lsl r5
   10b5c:	e1a03430 	lsr	r3, r0, r4
   10b60:	e1833511 	orr	r3, r3, r1, lsl r5
   10b64:	e1a00431 	lsr	r0, r1, r4
   10b68:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10b6c:	e1c00431 	bic	r0, r0, r1, lsr r4
   10b70:	e0800fa3 	add	r0, r0, r3, lsr #31
   10b74:	e19ee083 	orrs	lr, lr, r3, lsl #1
   10b78:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   10b7c:	e8bd8070 	pop	{r4, r5, r6, pc}
   10b80:	e3340000 	teq	r4, #0	; 0x0
   10b84:	1a000008 	bne	10bac <__aeabi_dmul+0x1c0>
   10b88:	e2016102 	and	r6, r1, #-2147483648	; 0x80000000
   10b8c:	e1b00080 	lsls	r0, r0, #1
   10b90:	e0a11001 	adc	r1, r1, r1
   10b94:	e3110601 	tst	r1, #1048576	; 0x100000
   10b98:	02444001 	subeq	r4, r4, #1	; 0x1
   10b9c:	0afffffa 	beq	10b8c <__aeabi_dmul+0x1a0>
   10ba0:	e1811006 	orr	r1, r1, r6
   10ba4:	e3350000 	teq	r5, #0	; 0x0
   10ba8:	11a0f00e 	movne	pc, lr
   10bac:	e2036102 	and	r6, r3, #-2147483648	; 0x80000000
   10bb0:	e1b02082 	lsls	r2, r2, #1
   10bb4:	e0a33003 	adc	r3, r3, r3
   10bb8:	e3130601 	tst	r3, #1048576	; 0x100000
   10bbc:	02455001 	subeq	r5, r5, #1	; 0x1
   10bc0:	0afffffa 	beq	10bb0 <__aeabi_dmul+0x1c4>
   10bc4:	e1833006 	orr	r3, r3, r6
   10bc8:	e1a0f00e 	mov	pc, lr
   10bcc:	e134000c 	teq	r4, ip
   10bd0:	e00c5a23 	and	r5, ip, r3, lsr #20
   10bd4:	1135000c 	teqne	r5, ip
   10bd8:	0a000006 	beq	10bf8 <__aeabi_dmul+0x20c>
   10bdc:	e1906081 	orrs	r6, r0, r1, lsl #1
   10be0:	11926083 	orrsne	r6, r2, r3, lsl #1
   10be4:	1affffe5 	bne	10b80 <__aeabi_dmul+0x194>
   10be8:	e0211003 	eor	r1, r1, r3
   10bec:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10bf0:	e3a00000 	mov	r0, #0	; 0x0
   10bf4:	e8bd8070 	pop	{r4, r5, r6, pc}
   10bf8:	e1906081 	orrs	r6, r0, r1, lsl #1
   10bfc:	01a00002 	moveq	r0, r2
   10c00:	01a01003 	moveq	r1, r3
   10c04:	11926083 	orrsne	r6, r2, r3, lsl #1
   10c08:	0a00000f 	beq	10c4c <__aeabi_dmul+0x260>
   10c0c:	e134000c 	teq	r4, ip
   10c10:	1a000001 	bne	10c1c <__aeabi_dmul+0x230>
   10c14:	e1906601 	orrs	r6, r0, r1, lsl #12
   10c18:	1a00000b 	bne	10c4c <__aeabi_dmul+0x260>
   10c1c:	e135000c 	teq	r5, ip
   10c20:	1a000003 	bne	10c34 <__aeabi_dmul+0x248>
   10c24:	e1926603 	orrs	r6, r2, r3, lsl #12
   10c28:	11a00002 	movne	r0, r2
   10c2c:	11a01003 	movne	r1, r3
   10c30:	1a000005 	bne	10c4c <__aeabi_dmul+0x260>
   10c34:	e0211003 	eor	r1, r1, r3
   10c38:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10c3c:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   10c40:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   10c44:	e3a00000 	mov	r0, #0	; 0x0
   10c48:	e8bd8070 	pop	{r4, r5, r6, pc}
   10c4c:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   10c50:	e381173e 	orr	r1, r1, #16252928	; 0xf80000
   10c54:	e8bd8070 	pop	{r4, r5, r6, pc}

00010c58 <__aeabi_ddiv>:
   10c58:	e92d4070 	push	{r4, r5, r6, lr}
   10c5c:	e3a0c0ff 	mov	ip, #255	; 0xff
   10c60:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   10c64:	e01c4a21 	ands	r4, ip, r1, lsr #20
   10c68:	101c5a23 	andsne	r5, ip, r3, lsr #20
   10c6c:	1134000c 	teqne	r4, ip
   10c70:	1135000c 	teqne	r5, ip
   10c74:	0b00005c 	bleq	10dec <__aeabi_ddiv+0x194>
   10c78:	e0444005 	sub	r4, r4, r5
   10c7c:	e021e003 	eor	lr, r1, r3
   10c80:	e1925603 	orrs	r5, r2, r3, lsl #12
   10c84:	e1a01601 	lsl	r1, r1, #12
   10c88:	0a00004b 	beq	10dbc <__aeabi_ddiv+0x164>
   10c8c:	e1a03603 	lsl	r3, r3, #12
   10c90:	e3a05201 	mov	r5, #268435456	; 0x10000000
   10c94:	e1853223 	orr	r3, r5, r3, lsr #4
   10c98:	e1833c22 	orr	r3, r3, r2, lsr #24
   10c9c:	e1a02402 	lsl	r2, r2, #8
   10ca0:	e1855221 	orr	r5, r5, r1, lsr #4
   10ca4:	e1855c20 	orr	r5, r5, r0, lsr #24
   10ca8:	e1a06400 	lsl	r6, r0, #8
   10cac:	e20e1102 	and	r1, lr, #-2147483648	; 0x80000000
   10cb0:	e1550003 	cmp	r5, r3
   10cb4:	01560002 	cmpeq	r6, r2
   10cb8:	e2a440fd 	adc	r4, r4, #253	; 0xfd
   10cbc:	e2844c03 	add	r4, r4, #768	; 0x300
   10cc0:	2a000001 	bcs	10ccc <__aeabi_ddiv+0x74>
   10cc4:	e1b030a3 	lsrs	r3, r3, #1
   10cc8:	e1a02062 	rrx	r2, r2
   10ccc:	e0566002 	subs	r6, r6, r2
   10cd0:	e0c55003 	sbc	r5, r5, r3
   10cd4:	e1b030a3 	lsrs	r3, r3, #1
   10cd8:	e1a02062 	rrx	r2, r2
   10cdc:	e3a00601 	mov	r0, #1048576	; 0x100000
   10ce0:	e3a0c702 	mov	ip, #524288	; 0x80000
   10ce4:	e056e002 	subs	lr, r6, r2
   10ce8:	e0d5e003 	sbcs	lr, r5, r3
   10cec:	20466002 	subcs	r6, r6, r2
   10cf0:	21a0500e 	movcs	r5, lr
   10cf4:	2180000c 	orrcs	r0, r0, ip
   10cf8:	e1b030a3 	lsrs	r3, r3, #1
   10cfc:	e1a02062 	rrx	r2, r2
   10d00:	e056e002 	subs	lr, r6, r2
   10d04:	e0d5e003 	sbcs	lr, r5, r3
   10d08:	20466002 	subcs	r6, r6, r2
   10d0c:	21a0500e 	movcs	r5, lr
   10d10:	218000ac 	orrcs	r0, r0, ip, lsr #1
   10d14:	e1b030a3 	lsrs	r3, r3, #1
   10d18:	e1a02062 	rrx	r2, r2
   10d1c:	e056e002 	subs	lr, r6, r2
   10d20:	e0d5e003 	sbcs	lr, r5, r3
   10d24:	20466002 	subcs	r6, r6, r2
   10d28:	21a0500e 	movcs	r5, lr
   10d2c:	2180012c 	orrcs	r0, r0, ip, lsr #2
   10d30:	e1b030a3 	lsrs	r3, r3, #1
   10d34:	e1a02062 	rrx	r2, r2
   10d38:	e056e002 	subs	lr, r6, r2
   10d3c:	e0d5e003 	sbcs	lr, r5, r3
   10d40:	20466002 	subcs	r6, r6, r2
   10d44:	21a0500e 	movcs	r5, lr
   10d48:	218001ac 	orrcs	r0, r0, ip, lsr #3
   10d4c:	e195e006 	orrs	lr, r5, r6
   10d50:	0a00000d 	beq	10d8c <__aeabi_ddiv+0x134>
   10d54:	e1a05205 	lsl	r5, r5, #4
   10d58:	e1855e26 	orr	r5, r5, r6, lsr #28
   10d5c:	e1a06206 	lsl	r6, r6, #4
   10d60:	e1a03183 	lsl	r3, r3, #3
   10d64:	e1833ea2 	orr	r3, r3, r2, lsr #29
   10d68:	e1a02182 	lsl	r2, r2, #3
   10d6c:	e1b0c22c 	lsrs	ip, ip, #4
   10d70:	1affffdb 	bne	10ce4 <__aeabi_ddiv+0x8c>
   10d74:	e3110601 	tst	r1, #1048576	; 0x100000
   10d78:	1a000006 	bne	10d98 <__aeabi_ddiv+0x140>
   10d7c:	e1811000 	orr	r1, r1, r0
   10d80:	e3a00000 	mov	r0, #0	; 0x0
   10d84:	e3a0c102 	mov	ip, #-2147483648	; 0x80000000
   10d88:	eaffffd5 	b	10ce4 <__aeabi_ddiv+0x8c>
   10d8c:	e3110601 	tst	r1, #1048576	; 0x100000
   10d90:	01811000 	orreq	r1, r1, r0
   10d94:	03a00000 	moveq	r0, #0	; 0x0
   10d98:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   10d9c:	835c0c07 	cmphi	ip, #1792	; 0x700
   10da0:	8affff4a 	bhi	10ad0 <__aeabi_dmul+0xe4>
   10da4:	e055c003 	subs	ip, r5, r3
   10da8:	0056c002 	subseq	ip, r6, r2
   10dac:	01b0c0a0 	lsrseq	ip, r0, #1
   10db0:	e2b00000 	adcs	r0, r0, #0	; 0x0
   10db4:	e0a11a04 	adc	r1, r1, r4, lsl #20
   10db8:	e8bd8070 	pop	{r4, r5, r6, pc}
   10dbc:	e20ee102 	and	lr, lr, #-2147483648	; 0x80000000
   10dc0:	e18e1621 	orr	r1, lr, r1, lsr #12
   10dc4:	e09440ac 	adds	r4, r4, ip, lsr #1
   10dc8:	c074500c 	rsbsgt	r5, r4, ip
   10dcc:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   10dd0:	c8bd8070 	popgt	{r4, r5, r6, pc}
   10dd4:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   10dd8:	e3a0e000 	mov	lr, #0	; 0x0
   10ddc:	e2544001 	subs	r4, r4, #1	; 0x1
   10de0:	eaffff3a 	b	10ad0 <__aeabi_dmul+0xe4>
   10de4:	e185e006 	orr	lr, r5, r6
   10de8:	eaffff38 	b	10ad0 <__aeabi_dmul+0xe4>
   10dec:	e00c5a23 	and	r5, ip, r3, lsr #20
   10df0:	e134000c 	teq	r4, ip
   10df4:	0135000c 	teqeq	r5, ip
   10df8:	0affff93 	beq	10c4c <__aeabi_dmul+0x260>
   10dfc:	e134000c 	teq	r4, ip
   10e00:	1a000006 	bne	10e20 <__aeabi_ddiv+0x1c8>
   10e04:	e1904601 	orrs	r4, r0, r1, lsl #12
   10e08:	1affff8f 	bne	10c4c <__aeabi_dmul+0x260>
   10e0c:	e135000c 	teq	r5, ip
   10e10:	1affff87 	bne	10c34 <__aeabi_dmul+0x248>
   10e14:	e1a00002 	mov	r0, r2
   10e18:	e1a01003 	mov	r1, r3
   10e1c:	eaffff8a 	b	10c4c <__aeabi_dmul+0x260>
   10e20:	e135000c 	teq	r5, ip
   10e24:	1a000004 	bne	10e3c <__aeabi_ddiv+0x1e4>
   10e28:	e1925603 	orrs	r5, r2, r3, lsl #12
   10e2c:	0affff6d 	beq	10be8 <__aeabi_dmul+0x1fc>
   10e30:	e1a00002 	mov	r0, r2
   10e34:	e1a01003 	mov	r1, r3
   10e38:	eaffff83 	b	10c4c <__aeabi_dmul+0x260>
   10e3c:	e1906081 	orrs	r6, r0, r1, lsl #1
   10e40:	11926083 	orrsne	r6, r2, r3, lsl #1
   10e44:	1affff4d 	bne	10b80 <__aeabi_dmul+0x194>
   10e48:	e1904081 	orrs	r4, r0, r1, lsl #1
   10e4c:	1affff78 	bne	10c34 <__aeabi_dmul+0x248>
   10e50:	e1925083 	orrs	r5, r2, r3, lsl #1
   10e54:	1affff63 	bne	10be8 <__aeabi_dmul+0x1fc>
   10e58:	eaffff7b 	b	10c4c <__aeabi_dmul+0x260>

00010e5c <__aeabi_d2iz>:
   10e5c:	e1a02081 	lsl	r2, r1, #1
   10e60:	e2922602 	adds	r2, r2, #2097152	; 0x200000
   10e64:	2a00000c 	bcs	10e9c <__aeabi_d2iz+0x40>
   10e68:	5a000009 	bpl	10e94 <__aeabi_d2iz+0x38>
   10e6c:	e3e03e3e 	mvn	r3, #992	; 0x3e0
   10e70:	e0532ac2 	subs	r2, r3, r2, asr #21
   10e74:	9a00000a 	bls	10ea4 <__aeabi_d2iz+0x48>
   10e78:	e1a03581 	lsl	r3, r1, #11
   10e7c:	e3833102 	orr	r3, r3, #-2147483648	; 0x80000000
   10e80:	e1833aa0 	orr	r3, r3, r0, lsr #21
   10e84:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   10e88:	e1a00233 	lsr	r0, r3, r2
   10e8c:	12600000 	rsbne	r0, r0, #0	; 0x0
   10e90:	e12fff1e 	bx	lr
   10e94:	e3a00000 	mov	r0, #0	; 0x0
   10e98:	e12fff1e 	bx	lr
   10e9c:	e1900601 	orrs	r0, r0, r1, lsl #12
   10ea0:	1a000002 	bne	10eb0 <__aeabi_d2iz+0x54>
   10ea4:	e2110102 	ands	r0, r1, #-2147483648	; 0x80000000
   10ea8:	03e00102 	mvneq	r0, #-2147483648	; 0x80000000
   10eac:	e12fff1e 	bx	lr
   10eb0:	e3a00000 	mov	r0, #0	; 0x0
   10eb4:	e12fff1e 	bx	lr

00010eb8 <__div0>:
   10eb8:	e92d4002 	push	{r1, lr}
   10ebc:	e3a00008 	mov	r0, #8	; 0x8
   10ec0:	ebfffcb1 	bl	1018c <raise>
   10ec4:	e8bd8002 	pop	{r1, pc}
