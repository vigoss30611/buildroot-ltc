
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
      78:	eb0003bd 	bl	f74 <chip_test>
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
     16c:	eb0013cf 	bl	50b0 <vs_assign_by_id>
     170:	e2506000 	subs	r6, r0, #0	; 0x0
     174:	0a000008 	beq	19c <bl_load_isi+0x4c>
     178:	e59f3178 	ldr	r3, [pc, #376]	; 2f8 <_data_len+0x154>
     17c:	e1a00004 	mov	r0, r4
     180:	e5934000 	ldr	r4, [r3]
     184:	eb0012d2 	bl	4cd4 <vs_device_name>
     188:	e1a02006 	mov	r2, r6
     18c:	e1a01000 	mov	r1, r0
     190:	e59f0164 	ldr	r0, [pc, #356]	; 2fc <_data_len+0x158>
     194:	e12fff34 	blx	r4
     198:	ea000053 	b	2ec <_data_len+0x148>
     19c:	e1a00004 	mov	r0, r4
     1a0:	e59f1158 	ldr	r1, [pc, #344]	; 300 <_data_len+0x15c>
     1a4:	eb001301 	bl	4db0 <vs_is_device>
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
     204:	eb0005b6 	bl	18e4 <isi_check_header>
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
     260:	eb0005f8 	bl	1a48 <isi_check_data>
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
     2b0:	eb0012d7 	bl	4e14 <vs_read>
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
     2fc:	000119c2 	.word	0x000119c2
     300:	000119e6 	.word	0x000119e6
     304:	0001ffff 	.word	0x0001ffff
     308:	fffe0000 	.word	0xfffe0000
     30c:	000119eb 	.word	0x000119eb
     310:	00011a02 	.word	0x00011a02
     314:	00011a14 	.word	0x00011a14

00000318 <try_get_bl>:
     318:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     31c:	e3a02602 	mov	r2, #2097152	; 0x200000
     320:	e24dd00c 	sub	sp, sp, #12	; 0xc
     324:	e3a03000 	mov	r3, #0	; 0x0
     328:	e88d000c 	stm	sp, {r2, r3}
     32c:	e1a07001 	mov	r7, r1
     330:	e1a06000 	mov	r6, r0
     334:	eb0000a8 	bl	5dc <boot_device>
     338:	e1a04000 	mov	r4, r0
     33c:	eb00121d 	bl	4bb8 <vs_device_bootable>
     340:	e2505000 	subs	r5, r0, #0	; 0x0
     344:	1a000008 	bne	36c <try_get_bl+0x54>
     348:	e59f308c 	ldr	r3, [pc, #140]	; 3dc <try_get_bl+0xc4>
     34c:	e1a00004 	mov	r0, r4
     350:	e5934000 	ldr	r4, [r3]
     354:	eb00125e 	bl	4cd4 <vs_device_name>
     358:	e1a01000 	mov	r1, r0
     35c:	e59f007c 	ldr	r0, [pc, #124]	; 3e0 <try_get_bl+0xc8>
     360:	e12fff34 	blx	r4
     364:	e1a00005 	mov	r0, r5
     368:	ea000019 	b	3d4 <try_get_bl+0xbc>
     36c:	e1a00004 	mov	r0, r4
     370:	eb001245 	bl	4c8c <vs_device_boot0>
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
     3cc:	eb00170b 	bl	6000 <cdbg_verify_burn_enable>
     3d0:	e3a00001 	mov	r0, #1	; 0x1
     3d4:	e28dd00c 	add	sp, sp, #12	; 0xc
     3d8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     3dc:	0800d76c 	.word	0x0800d76c
     3e0:	00011a26 	.word	0x00011a26
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
     408:	eb000059 	bl	574 <set_xom>
     40c:	eb00005f 	bl	590 <boot_info>
     410:	e28d8004 	add	r8, sp, #4	; 0x4
     414:	e1a05000 	mov	r5, r0
     418:	ea000008 	b	440 <try_boot+0x58>
     41c:	eb00012c 	bl	8d4 <gtc_time>
     420:	e1a00004 	mov	r0, r4
     424:	eb000052 	bl	574 <set_xom>
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
     450:	eb00011f 	bl	8d4 <gtc_time>
     454:	e3a00001 	mov	r0, #1	; 0x1
     458:	eb000133 	bl	92c <ecfg_check_flag>
     45c:	e3500000 	cmp	r0, #0	; 0x0
     460:	1a00000c 	bne	498 <try_boot+0xb0>
     464:	e3570000 	cmp	r7, #0	; 0x0
     468:	1a00000a 	bne	498 <try_boot+0xb0>
     46c:	eb0000c9 	bl	798 <get_reset_stats>
     470:	e3500000 	cmp	r0, #0	; 0x0
     474:	13500002 	cmpne	r0, #2	; 0x2
     478:	1a000006 	bne	498 <try_boot+0xb0>
     47c:	e3500002 	cmp	r0, #2	; 0x2
     480:	1a000001 	bne	48c <try_boot+0xa4>
     484:	e28d0004 	add	r0, sp, #4	; 0x4
     488:	eb0000cf 	bl	7cc <update_mask_on_reset>
     48c:	e59d0004 	ldr	r0, [sp, #4]
     490:	eb001898 	bl	66f8 <burn>
     494:	eb00010e 	bl	8d4 <gtc_time>
     498:	e3540000 	cmp	r4, #0	; 0x0
     49c:	0a000004 	beq	4b4 <try_boot+0xcc>
     4a0:	e3a03302 	mov	r3, #134217728	; 0x8000000
     4a4:	e5930014 	ldr	r0, [r3, #20]
     4a8:	e59d1000 	ldr	r1, [sp]
     4ac:	eb0016de 	bl	602c <cdbg_jump>
     4b0:	eafffffe 	b	4b0 <try_boot+0xc8>
     4b4:	eb00170e 	bl	60f4 <cdbg_shell>
     4b8:	eafffffe 	b	4b8 <try_boot+0xd0>

000004bc <init_data_section>:
     4bc:	e59f1010 	ldr	r1, [pc, #16]	; 4d4 <init_data_section+0x18>
     4c0:	e59f0010 	ldr	r0, [pc, #16]	; 4d8 <init_data_section+0x1c>
     4c4:	e1510000 	cmp	r1, r0
     4c8:	012fff1e 	bxeq	lr
     4cc:	e59f2008 	ldr	r2, [pc, #8]	; 4dc <init_data_section+0x20>
     4d0:	ea00039b 	b	1344 <memcpy>
     4d4:	00014744 	.word	0x00014744
     4d8:	0800d000 	.word	0x0800d000
     4dc:	000001a4 	.word	0x000001a4

000004e0 <initialization>:
     4e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     4e4:	e24dd004 	sub	sp, sp, #4	; 0x4
     4e8:	ebfffff3 	bl	4bc <init_data_section>
     4ec:	eb000025 	bl	588 <init_xom>
     4f0:	eb000097 	bl	754 <init_lowlevel>
     4f4:	eb0004bc 	bl	17ec <init_malloc>
     4f8:	eb002118 	bl	8960 <init_timer>
     4fc:	eb000101 	bl	908 <init_gtc>
     500:	eb0020a9 	bl	87ac <init_serial>
     504:	eb0019d6 	bl	6c64 <init_mmc>
     508:	eb0000a1 	bl	794 <init_crypto>
     50c:	eb00021e 	bl	d8c <init_alternate>
     510:	eb00001d 	bl	58c <init_boot_sequence>
     514:	e3a00000 	mov	r0, #0	; 0x0
     518:	e28dd004 	add	sp, sp, #4	; 0x4
     51c:	e8bd8000 	pop	{pc}

00000520 <main>:
     520:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     524:	e3a00000 	mov	r0, #0	; 0x0
     528:	e24dd004 	sub	sp, sp, #4	; 0x4
     52c:	eb001697 	bl	5f90 <cdbg_log_toggle>
     530:	ebffffea 	bl	4e0 <initialization>
     534:	eb0000e6 	bl	8d4 <gtc_time>
     538:	e3a00001 	mov	r0, #1	; 0x1
     53c:	eb001693 	bl	5f90 <cdbg_log_toggle>
     540:	e59f0014 	ldr	r0, [pc, #20]	; 55c <main+0x3c>
     544:	e59f3014 	ldr	r3, [pc, #20]	; 560 <main+0x40>
     548:	e1a0e00f 	mov	lr, pc
     54c:	e593f000 	ldr	pc, [r3]
     550:	e3a00000 	mov	r0, #0	; 0x0
     554:	ebffffa3 	bl	3e8 <try_boot>
     558:	eafffffe 	b	558 <main+0x38>
     55c:	00011a40 	.word	0x00011a40
     560:	0800d76c 	.word	0x0800d76c

00000564 <get_xom>:
     564:	e59f3004 	ldr	r3, [pc, #4]	; 570 <get_xom+0xc>
     568:	e1d300b0 	ldrh	r0, [r3]
     56c:	e12fff1e 	bx	lr
     570:	0800d1a8 	.word	0x0800d1a8

00000574 <set_xom>:
     574:	e59f3008 	ldr	r3, [pc, #8]	; 584 <set_xom+0x10>
     578:	e200001f 	and	r0, r0, #31	; 0x1f
     57c:	e1c300b0 	strh	r0, [r3]
     580:	e12fff1e 	bx	lr
     584:	0800d1a8 	.word	0x0800d1a8

00000588 <init_xom>:
     588:	e12fff1e 	bx	lr

0000058c <init_boot_sequence>:
     58c:	e12fff1e 	bx	lr

00000590 <boot_info>:
     590:	e92d4010 	push	{r4, lr}
     594:	eb0000ed 	bl	950 <ecfg_paras>
     598:	e1a04000 	mov	r4, r0
     59c:	e3a00b01 	mov	r0, #1024	; 0x400
     5a0:	eb0000e1 	bl	92c <ecfg_check_flag>
     5a4:	e59f3028 	ldr	r3, [pc, #40]	; 5d4 <boot_info+0x44>
     5a8:	e3500000 	cmp	r0, #0	; 0x0
     5ac:	11d330b0 	ldrhne	r3, [r3]
     5b0:	01d330b0 	ldrheq	r3, [r3]
     5b4:	1203301f 	andne	r3, r3, #31	; 0x1f
     5b8:	0203301f 	andeq	r3, r3, #31	; 0x1f
     5bc:	159f2014 	ldrne	r2, [pc, #20]	; 5d8 <boot_info+0x48>
     5c0:	11a03083 	lslne	r3, r3, #1
     5c4:	00843083 	addeq	r3, r4, r3, lsl #1
     5c8:	119200b3 	ldrhne	r0, [r2, r3]
     5cc:	01d302b0 	ldrheq	r0, [r3, #32]
     5d0:	e8bd8010 	pop	{r4, pc}
     5d4:	0800d1a8 	.word	0x0800d1a8
     5d8:	000115dc 	.word	0x000115dc

000005dc <boot_device>:
     5dc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     5e0:	e24dd004 	sub	sp, sp, #4	; 0x4
     5e4:	ebffffe9 	bl	590 <boot_info>
     5e8:	e1a00620 	lsr	r0, r0, #12
     5ec:	e6ff0070 	uxth	r0, r0
     5f0:	e28dd004 	add	sp, sp, #4	; 0x4
     5f4:	e8bd8000 	pop	{pc}

000005f8 <set_pll>:
     5f8:	e3500003 	cmp	r0, #3	; 0x3
     5fc:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
     600:	e1a02001 	mov	r2, r1
     604:	88bd85f0 	pophi	{r4, r5, r6, r7, r8, sl, pc}
     608:	e3a03018 	mov	r3, #24	; 0x18
     60c:	e0030093 	mul	r3, r3, r0
     610:	e59f808c 	ldr	r8, [pc, #140]	; 6a4 <set_pll+0xac>
     614:	e59f008c 	ldr	r0, [pc, #140]	; 6a8 <set_pll+0xb0>
     618:	e59fc08c 	ldr	ip, [pc, #140]	; 6ac <set_pll+0xb4>
     61c:	e59f508c 	ldr	r5, [pc, #140]	; 6b0 <set_pll+0xb8>
     620:	e59fa08c 	ldr	sl, [pc, #140]	; 6b4 <set_pll+0xbc>
     624:	e283142d 	add	r1, r3, #754974720	; 0x2d000000
     628:	e0838008 	add	r8, r3, r8
     62c:	e083c00c 	add	ip, r3, ip
     630:	e3a07001 	mov	r7, #1	; 0x1
     634:	e2811a0a 	add	r1, r1, #40960	; 0xa000
     638:	e0830000 	add	r0, r3, r0
     63c:	e1a06422 	lsr	r6, r2, #8
     640:	e0835005 	add	r5, r3, r5
     644:	e3a04000 	mov	r4, #0	; 0x0
     648:	e6ef2072 	uxtb	r2, r2
     64c:	e083a00a 	add	sl, r3, sl
     650:	e3a03002 	mov	r3, #2	; 0x2
     654:	e5c87000 	strb	r7, [r8]
     658:	e5c12000 	strb	r2, [r1]
     65c:	e5c06000 	strb	r6, [r0]
     660:	e5cc4000 	strb	r4, [ip]
     664:	e5c53000 	strb	r3, [r5]
     668:	e0833007 	add	r3, r3, r7
     66c:	e5c83000 	strb	r3, [r8]
     670:	e1a02004 	mov	r2, r4
     674:	e5cc7000 	strb	r7, [ip]
     678:	e3011a0a 	movw	r1, #6666	; 0x1a0a
     67c:	ea000001 	b	688 <set_pll+0x90>
     680:	e1520001 	cmp	r2, r1
     684:	0a000003 	beq	698 <set_pll+0xa0>
     688:	e5da3000 	ldrb	r3, [sl]
     68c:	e2822001 	add	r2, r2, #1	; 0x1
     690:	e3130001 	tst	r3, #1	; 0x1
     694:	0afffff9 	beq	680 <set_pll+0x88>
     698:	e3a03002 	mov	r3, #2	; 0x2
     69c:	e5883000 	str	r3, [r8]
     6a0:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
     6a4:	2d00a00c 	.word	0x2d00a00c
     6a8:	2d00a004 	.word	0x2d00a004
     6ac:	2d00a010 	.word	0x2d00a010
     6b0:	2d00a008 	.word	0x2d00a008
     6b4:	2d00a014 	.word	0x2d00a014

000006b8 <set_clk_divider>:
     6b8:	e3500005 	cmp	r0, #5	; 0x5
     6bc:	812fff1e 	bxhi	lr
     6c0:	e59fc020 	ldr	ip, [pc, #32]	; 6e8 <set_clk_divider+0x30>
     6c4:	e1a03280 	lsl	r3, r0, #5
     6c8:	e083c00c 	add	ip, r3, ip
     6cc:	e3a02000 	mov	r2, #0	; 0x0
     6d0:	e7d13002 	ldrb	r3, [r1, r2]
     6d4:	e2822001 	add	r2, r2, #1	; 0x1
     6d8:	e3520008 	cmp	r2, #8	; 0x8
     6dc:	e4cc3004 	strb	r3, [ip], #4
     6e0:	1afffffa 	bne	6d0 <set_clk_divider+0x18>
     6e4:	e12fff1e 	bx	lr
     6e8:	2d00a070 	.word	0x2d00a070

000006ec <module_set_clock>:
     6ec:	e59f3020 	ldr	r3, [pc, #32]	; 714 <module_set_clock+0x28>
     6f0:	e1500003 	cmp	r0, r3
     6f4:	13a03001 	movne	r3, #1	; 0x1
     6f8:	1580300c 	strne	r3, [r0, #12]
     6fc:	e2013007 	and	r3, r1, #7	; 0x7
     700:	e5803000 	str	r3, [r0]
     704:	e3a03000 	mov	r3, #0	; 0x0
     708:	e5802004 	str	r2, [r0, #4]
     70c:	e580300c 	str	r3, [r0, #12]
     710:	e12fff1e 	bx	lr
     714:	2d00a360 	.word	0x2d00a360

00000718 <module_enable>:
     718:	e3a020ff 	mov	r2, #255	; 0xff
     71c:	e3a01000 	mov	r1, #0	; 0x0
     720:	e3a03011 	mov	r3, #17	; 0x11
     724:	e580100c 	str	r1, [r0, #12]
     728:	e5802000 	str	r2, [r0]
     72c:	e5802004 	str	r2, [r0, #4]
     730:	e5803008 	str	r3, [r0, #8]
     734:	e5801014 	str	r1, [r0, #20]
     738:	e5903008 	ldr	r3, [r0, #8]
     73c:	e3c33010 	bic	r3, r3, #16	; 0x10
     740:	e5803008 	str	r3, [r0, #8]
     744:	e5802018 	str	r2, [r0, #24]
     748:	e5801000 	str	r1, [r0]
     74c:	e580200c 	str	r2, [r0, #12]
     750:	e12fff1e 	bx	lr

00000754 <init_lowlevel>:
     754:	e3a01000 	mov	r1, #0	; 0x0
     758:	e3e00000 	mvn	r0, #0	; 0x0
     75c:	e59f3018 	ldr	r3, [pc, #24]	; 77c <init_lowlevel+0x28>
     760:	e1a02101 	lsl	r2, r1, #2
     764:	e2811001 	add	r1, r1, #1	; 0x1
     768:	e0823003 	add	r3, r2, r3
     76c:	e3510008 	cmp	r1, #8	; 0x8
     770:	e5830000 	str	r0, [r3]
     774:	1afffff8 	bne	75c <init_lowlevel+0x8>
     778:	e12fff1e 	bx	lr
     77c:	21b01180 	.word	0x21b01180

00000780 <init_igps_ram>:
     780:	e59f3008 	ldr	r3, [pc, #8]	; 790 <init_igps_ram+0x10>
     784:	e3a02001 	mov	r2, #1	; 0x1
     788:	e5832410 	str	r2, [r3, #1040]
     78c:	e12fff1e 	bx	lr
     790:	2d00c000 	.word	0x2d00c000

00000794 <init_crypto>:
     794:	e12fff1e 	bx	lr

00000798 <get_reset_stats>:
     798:	e59f2028 	ldr	r2, [pc, #40]	; 7c8 <get_reset_stats+0x30>
     79c:	e5923c10 	ldr	r3, [r2, #3088]
     7a0:	e2130002 	ands	r0, r3, #2	; 0x2
     7a4:	012fff1e 	bxeq	lr
     7a8:	e5920c2c 	ldr	r0, [r2, #3116]
     7ac:	e3100001 	tst	r0, #1	; 0x1
     7b0:	03a00001 	moveq	r0, #1	; 0x1
     7b4:	012fff1e 	bxeq	lr
     7b8:	e3500037 	cmp	r0, #55	; 0x37
     7bc:	03a00002 	moveq	r0, #2	; 0x2
     7c0:	13a00003 	movne	r0, #3	; 0x3
     7c4:	e12fff1e 	bx	lr
     7c8:	2d009000 	.word	0x2d009000

000007cc <update_mask_on_reset>:
     7cc:	e59f3018 	ldr	r3, [pc, #24]	; 7ec <update_mask_on_reset+0x20>
     7d0:	e5932c30 	ldr	r2, [r3, #3120]
     7d4:	e3120080 	tst	r2, #128	; 0x80
     7d8:	13a03001 	movne	r3, #1	; 0x1
     7dc:	1202200f 	andne	r2, r2, #15	; 0xf
     7e0:	11e03213 	mvnne	r3, r3, lsl r2
     7e4:	15803000 	strne	r3, [r0]
     7e8:	e12fff1e 	bx	lr
     7ec:	2d009000 	.word	0x2d009000

000007f0 <get_wakeup_entry>:
     7f0:	e59f301c 	ldr	r3, [pc, #28]	; 814 <get_wakeup_entry+0x24>
     7f4:	e5931c38 	ldr	r1, [r3, #3128]
     7f8:	e5932c34 	ldr	r2, [r3, #3124]
     7fc:	e5930c30 	ldr	r0, [r3, #3120]
     800:	e1822401 	orr	r2, r2, r1, lsl #8
     804:	e5933c2c 	ldr	r3, [r3, #3116]
     808:	e1800402 	orr	r0, r0, r2, lsl #8
     80c:	e1830400 	orr	r0, r3, r0, lsl #8
     810:	e12fff1e 	bx	lr
     814:	2d009000 	.word	0x2d009000

00000818 <system_reset>:
     818:	e59f3010 	ldr	r3, [pc, #16]	; 830 <system_reset+0x18>
     81c:	e3a02001 	mov	r2, #1	; 0x1
     820:	e5832c2c 	str	r2, [r3, #3116]
     824:	e2822002 	add	r2, r2, #2	; 0x2
     828:	e5832c00 	str	r2, [r3, #3072]
     82c:	e12fff1e 	bx	lr
     830:	2d009000 	.word	0x2d009000

00000834 <pads_init_module>:
     834:	e92d4030 	push	{r4, r5, lr}
     838:	e1a05000 	mov	r5, r0
     83c:	e24dd004 	sub	sp, sp, #4	; 0x4
     840:	e59f4064 	ldr	r4, [pc, #100]	; 8ac <pads_init_module+0x78>
     844:	ea000013 	b	898 <pads_init_module+0x64>
     848:	e1500005 	cmp	r0, r5
     84c:	1a000010 	bne	894 <pads_init_module+0x60>
     850:	e3a01000 	mov	r1, #0	; 0x0
     854:	e1a02001 	mov	r2, r1
     858:	e514000c 	ldr	r0, [r4, #-12]
     85c:	eb0000c6 	bl	b7c <pads_chmod>
     860:	e5140004 	ldr	r0, [r4, #-4]
     864:	e3a01000 	mov	r1, #0	; 0x0
     868:	e1500001 	cmp	r0, r1
     86c:	e1a02001 	mov	r2, r1
     870:	da000000 	ble	878 <pads_init_module+0x44>
     874:	eb0000c0 	bl	b7c <pads_chmod>
     878:	e5943000 	ldr	r3, [r4]
     87c:	e3a01000 	mov	r1, #0	; 0x0
     880:	e1530001 	cmp	r3, r1
     884:	e1a02001 	mov	r2, r1
     888:	e1a00003 	mov	r0, r3
     88c:	da000000 	ble	894 <pads_init_module+0x60>
     890:	eb0000b9 	bl	b7c <pads_chmod>
     894:	e2844014 	add	r4, r4, #20	; 0x14
     898:	e5140010 	ldr	r0, [r4, #-16]
     89c:	e3500000 	cmp	r0, #0	; 0x0
     8a0:	1affffe8 	bne	848 <pads_init_module+0x14>
     8a4:	e28dd004 	add	sp, sp, #4	; 0x4
     8a8:	e8bd8030 	pop	{r4, r5, pc}
     8ac:	0800d010 	.word	0x0800d010

000008b0 <module_reset>:
     8b0:	e3a030ff 	mov	r3, #255	; 0xff
     8b4:	e5803000 	str	r3, [r0]
     8b8:	e92d4010 	push	{r4, lr}
     8bc:	e1a04000 	mov	r4, r0
     8c0:	e3a0000a 	mov	r0, #10	; 0xa
     8c4:	eb001fe1 	bl	8850 <udelay>
     8c8:	e3a03000 	mov	r3, #0	; 0x0
     8cc:	e5843000 	str	r3, [r4]
     8d0:	e8bd8010 	pop	{r4, pc}

000008d4 <gtc_time>:
     8d4:	e59f3020 	ldr	r3, [pc, #32]	; 8fc <gtc_time+0x28>
     8d8:	e92d4010 	push	{r4, lr}
     8dc:	e5930000 	ldr	r0, [r3]
     8e0:	e59f3018 	ldr	r3, [pc, #24]	; 900 <gtc_time+0x2c>
     8e4:	e5934000 	ldr	r4, [r3]
     8e8:	eb001ffc 	bl	88e0 <get_timer>
     8ec:	e1a01000 	mov	r1, r0
     8f0:	e59f000c 	ldr	r0, [pc, #12]	; 904 <gtc_time+0x30>
     8f4:	e12fff34 	blx	r4
     8f8:	e8bd8010 	pop	{r4, pc}
     8fc:	0800d1ac 	.word	0x0800d1ac
     900:	0800d76c 	.word	0x0800d76c
     904:	00011a45 	.word	0x00011a45

00000908 <init_gtc>:
     908:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     90c:	e3a00000 	mov	r0, #0	; 0x0
     910:	e24dd004 	sub	sp, sp, #4	; 0x4
     914:	eb001ff1 	bl	88e0 <get_timer>
     918:	e59f3008 	ldr	r3, [pc, #8]	; 928 <init_gtc+0x20>
     91c:	e5830000 	str	r0, [r3]
     920:	e28dd004 	add	sp, sp, #4	; 0x4
     924:	e8bd8000 	pop	{pc}
     928:	0800d1ac 	.word	0x0800d1ac

0000092c <ecfg_check_flag>:
     92c:	e3500801 	cmp	r0, #65536	; 0x10000
     930:	0a000004 	beq	948 <ecfg_check_flag+0x1c>
     934:	e3500101 	cmp	r0, #1073741824	; 0x40000000
     938:	0a000002 	beq	948 <ecfg_check_flag+0x1c>
     93c:	e3500001 	cmp	r0, #1	; 0x1
     940:	13a00001 	movne	r0, #1	; 0x1
     944:	112fff1e 	bxne	lr
     948:	e3a00000 	mov	r0, #0	; 0x0
     94c:	e12fff1e 	bx	lr

00000950 <ecfg_paras>:
     950:	e59f0000 	ldr	r0, [pc, #0]	; 958 <ecfg_paras+0x8>
     954:	e12fff1e 	bx	lr
     958:	0800d770 	.word	0x0800d770

0000095c <efuse_mac>:
     95c:	e3a00000 	mov	r0, #0	; 0x0
     960:	e3a01000 	mov	r1, #0	; 0x0
     964:	e12fff1e 	bx	lr

00000968 <efuse_info>:
     968:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     96c:	e1a04000 	mov	r4, r0
     970:	e24dd004 	sub	sp, sp, #4	; 0x4
     974:	e59f6144 	ldr	r6, [pc, #324]	; ac0 <efuse_info+0x158>
     978:	e59f0144 	ldr	r0, [pc, #324]	; ac4 <efuse_info+0x15c>
     97c:	e1a0e00f 	mov	lr, pc
     980:	e596f000 	ldr	pc, [r6]
     984:	e59f013c 	ldr	r0, [pc, #316]	; ac8 <efuse_info+0x160>
     988:	e1a0e00f 	mov	lr, pc
     98c:	e596f000 	ldr	pc, [r6]
     990:	e3540000 	cmp	r4, #0	; 0x0
     994:	0a00003c 	beq	a8c <efuse_info+0x124>
     998:	e59f412c 	ldr	r4, [pc, #300]	; acc <efuse_info+0x164>
     99c:	e59f012c 	ldr	r0, [pc, #300]	; ad0 <efuse_info+0x168>
     9a0:	e5941000 	ldr	r1, [r4]
     9a4:	e1a0e00f 	mov	lr, pc
     9a8:	e596f000 	ldr	pc, [r6]
     9ac:	e5d41004 	ldrb	r1, [r4, #4]
     9b0:	e59f011c 	ldr	r0, [pc, #284]	; ad4 <efuse_info+0x16c>
     9b4:	e1a0e00f 	mov	lr, pc
     9b8:	e596f000 	ldr	pc, [r6]
     9bc:	e5d41005 	ldrb	r1, [r4, #5]
     9c0:	e59f0110 	ldr	r0, [pc, #272]	; ad8 <efuse_info+0x170>
     9c4:	e1a0e00f 	mov	lr, pc
     9c8:	e596f000 	ldr	pc, [r6]
     9cc:	e5d41006 	ldrb	r1, [r4, #6]
     9d0:	e59f0104 	ldr	r0, [pc, #260]	; adc <efuse_info+0x174>
     9d4:	e1a0e00f 	mov	lr, pc
     9d8:	e596f000 	ldr	pc, [r6]
     9dc:	e5d41007 	ldrb	r1, [r4, #7]
     9e0:	e59f00f8 	ldr	r0, [pc, #248]	; ae0 <efuse_info+0x178>
     9e4:	e1a0e00f 	mov	lr, pc
     9e8:	e596f000 	ldr	pc, [r6]
     9ec:	e1d410b8 	ldrh	r1, [r4, #8]
     9f0:	e59f00ec 	ldr	r0, [pc, #236]	; ae4 <efuse_info+0x17c>
     9f4:	e1a0e00f 	mov	lr, pc
     9f8:	e596f000 	ldr	pc, [r6]
     9fc:	e1d410ba 	ldrh	r1, [r4, #10]
     a00:	e59f00e0 	ldr	r0, [pc, #224]	; ae8 <efuse_info+0x180>
     a04:	e1a0e00f 	mov	lr, pc
     a08:	e596f000 	ldr	pc, [r6]
     a0c:	e1d410bc 	ldrh	r1, [r4, #12]
     a10:	e59f00d4 	ldr	r0, [pc, #212]	; aec <efuse_info+0x184>
     a14:	e1a0e00f 	mov	lr, pc
     a18:	e596f000 	ldr	pc, [r6]
     a1c:	e1d410be 	ldrh	r1, [r4, #14]
     a20:	e59f00c8 	ldr	r0, [pc, #200]	; af0 <efuse_info+0x188>
     a24:	e1a0e00f 	mov	lr, pc
     a28:	e596f000 	ldr	pc, [r6]
     a2c:	e1a05004 	mov	r5, r4
     a30:	e3a04000 	mov	r4, #0	; 0x0
     a34:	e1a01004 	mov	r1, r4
     a38:	e5952010 	ldr	r2, [r5, #16]
     a3c:	e2844001 	add	r4, r4, #1	; 0x1
     a40:	e59f00ac 	ldr	r0, [pc, #172]	; af4 <efuse_info+0x18c>
     a44:	e1a0e00f 	mov	lr, pc
     a48:	e596f000 	ldr	pc, [r6]
     a4c:	e3540004 	cmp	r4, #4	; 0x4
     a50:	e2855004 	add	r5, r5, #4	; 0x4
     a54:	1afffff6 	bne	a34 <efuse_info+0xcc>
     a58:	e59f6060 	ldr	r6, [pc, #96]	; ac0 <efuse_info+0x158>
     a5c:	e59f5068 	ldr	r5, [pc, #104]	; acc <efuse_info+0x164>
     a60:	e284400c 	add	r4, r4, #12	; 0xc
     a64:	e1a03084 	lsl	r3, r4, #1
     a68:	e1a01004 	mov	r1, r4
     a6c:	e19520b3 	ldrh	r2, [r5, r3]
     a70:	e2844001 	add	r4, r4, #1	; 0x1
     a74:	e59f007c 	ldr	r0, [pc, #124]	; af8 <efuse_info+0x190>
     a78:	e1a0e00f 	mov	lr, pc
     a7c:	e596f000 	ldr	pc, [r6]
     a80:	e354001c 	cmp	r4, #28	; 0x1c
     a84:	1afffff6 	bne	a64 <efuse_info+0xfc>
     a88:	ea00000a 	b	ab8 <efuse_info+0x150>
     a8c:	e59f4068 	ldr	r4, [pc, #104]	; afc <efuse_info+0x194>
     a90:	e59f7068 	ldr	r7, [pc, #104]	; b00 <efuse_info+0x198>
     a94:	e1a05006 	mov	r5, r6
     a98:	e5942000 	ldr	r2, [r4]
     a9c:	e1a01004 	mov	r1, r4
     aa0:	e59f005c 	ldr	r0, [pc, #92]	; b04 <efuse_info+0x19c>
     aa4:	e2844004 	add	r4, r4, #4	; 0x4
     aa8:	e1a0e00f 	mov	lr, pc
     aac:	e595f000 	ldr	pc, [r5]
     ab0:	e1540007 	cmp	r4, r7
     ab4:	1afffff7 	bne	a98 <efuse_info+0x130>
     ab8:	e28dd004 	add	sp, sp, #4	; 0x4
     abc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     ac0:	0800d76c 	.word	0x0800d76c
     ac4:	00011a4d 	.word	0x00011a4d
     ac8:	00011a62 	.word	0x00011a62
     acc:	0800d770 	.word	0x0800d770
     ad0:	00011a79 	.word	0x00011a79
     ad4:	00011a88 	.word	0x00011a88
     ad8:	00011a97 	.word	0x00011a97
     adc:	00011aaa 	.word	0x00011aaa
     ae0:	00011ab9 	.word	0x00011ab9
     ae4:	00011ace 	.word	0x00011ace
     ae8:	00011adb 	.word	0x00011adb
     aec:	00011aeb 	.word	0x00011aeb
     af0:	00011afc 	.word	0x00011afc
     af4:	00011b0e 	.word	0x00011b0e
     af8:	00011b1e 	.word	0x00011b1e
     afc:	2d00d9b4 	.word	0x2d00d9b4
     b00:	2d00da94 	.word	0x2d00da94
     b04:	00011b2f 	.word	0x00011b2f

00000b08 <init_efuse>:
     b08:	e59f0008 	ldr	r0, [pc, #8]	; b18 <init_efuse+0x10>
     b0c:	e3a01000 	mov	r1, #0	; 0x0
     b10:	e3a02038 	mov	r2, #56	; 0x38
     b14:	ea000202 	b	1324 <memset>
     b18:	0800d770 	.word	0x0800d770

00000b1c <assert_linkpc>:
     b1c:	e3a00001 	mov	r0, #1	; 0x1
     b20:	e12fff1e 	bx	lr

00000b24 <pads_states>:
     b24:	e3500000 	cmp	r0, #0	; 0x0
     b28:	e2803007 	add	r3, r0, #7	; 0x7
     b2c:	a1a03000 	movge	r3, r0
     b30:	e1a031c3 	asr	r3, r3, #3
     b34:	e59f203c 	ldr	r2, [pc, #60]	; b78 <pads_states+0x54>
     b38:	e1a03103 	lsl	r3, r3, #2
     b3c:	e0832002 	add	r2, r3, r2
     b40:	e5921000 	ldr	r1, [r2]
     b44:	e3a03001 	mov	r3, #1	; 0x1
     b48:	e2002007 	and	r2, r0, #7	; 0x7
     b4c:	e0113213 	ands	r3, r1, r3, lsl r2
     b50:	01a00003 	moveq	r0, r3
     b54:	012fff1e 	bxeq	lr
     b58:	e1a03300 	lsl	r3, r0, #6
     b5c:	e2833282 	add	r3, r3, #536870920	; 0x20000008
     b60:	e283360f 	add	r3, r3, #15728640	; 0xf00000
     b64:	e5933000 	ldr	r3, [r3]
     b68:	e3530000 	cmp	r3, #0	; 0x0
     b6c:	13a00002 	movne	r0, #2	; 0x2
     b70:	03a00001 	moveq	r0, #1	; 0x1
     b74:	e12fff1e 	bx	lr
     b78:	2d009004 	.word	0x2d009004

00000b7c <pads_chmod>:
     b7c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     b80:	e1b07840 	asrs	r7, r0, #16
     b84:	e1a00800 	lsl	r0, r0, #16
     b88:	03a03000 	moveq	r3, #0	; 0x0
     b8c:	13a03001 	movne	r3, #1	; 0x1
     b90:	e1a00820 	lsr	r0, r0, #16
     b94:	e1570000 	cmp	r7, r0
     b98:	a3a0c000 	movge	ip, #0	; 0x0
     b9c:	b203c001 	andlt	ip, r3, #1	; 0x1
     ba0:	e35c0000 	cmp	ip, #0	; 0x0
     ba4:	e24dd004 	sub	sp, sp, #4	; 0x4
     ba8:	e1a05001 	mov	r5, r1
     bac:	e1a09002 	mov	r9, r2
     bb0:	1a000038 	bne	c98 <pads_chmod+0x11c>
     bb4:	e2803883 	add	r3, r0, #8585216	; 0x830000
     bb8:	e2833903 	add	r3, r3, #49152	; 0xc000
     bbc:	e1a02303 	lsl	r2, r3, #6
     bc0:	e3570000 	cmp	r7, #0	; 0x0
     bc4:	01a07000 	moveq	r7, r0
     bc8:	e2826008 	add	r6, r2, #8	; 0x8
     bcc:	e2828004 	add	r8, r2, #4	; 0x4
     bd0:	e1a04000 	mov	r4, r0
     bd4:	e3a0a001 	mov	sl, #1	; 0x1
     bd8:	e59fb0c0 	ldr	fp, [pc, #192]	; ca0 <pads_chmod+0x124>
     bdc:	ea00002a 	b	c8c <pads_chmod+0x110>
     be0:	e3540000 	cmp	r4, #0	; 0x0
     be4:	a1a03004 	movge	r3, r4
     be8:	e3550001 	cmp	r5, #1	; 0x1
     bec:	e1a011c3 	asr	r1, r3, #3
     bf0:	e204c007 	and	ip, r4, #7	; 0x7
     bf4:	0a00000b 	beq	c28 <pads_chmod+0xac>
     bf8:	e3550002 	cmp	r5, #2	; 0x2
     bfc:	0a000012 	beq	c4c <pads_chmod+0xd0>
     c00:	e59f209c 	ldr	r2, [pc, #156]	; ca4 <pads_chmod+0x128>
     c04:	e1a03101 	lsl	r3, r1, #2
     c08:	e2551000 	subs	r1, r5, #0	; 0x0
     c0c:	e0832002 	add	r2, r3, r2
     c10:	e59f0090 	ldr	r0, [pc, #144]	; ca8 <pads_chmod+0x12c>
     c14:	1a000017 	bne	c78 <pads_chmod+0xfc>
     c18:	e5923000 	ldr	r3, [r2]
     c1c:	e1c33c1a 	bic	r3, r3, sl, lsl ip
     c20:	e5823000 	str	r3, [r2]
     c24:	ea000015 	b	c80 <pads_chmod+0x104>
     c28:	e59f3074 	ldr	r3, [pc, #116]	; ca4 <pads_chmod+0x128>
     c2c:	e1a02101 	lsl	r2, r1, #2
     c30:	e0823003 	add	r3, r2, r3
     c34:	e5932000 	ldr	r2, [r3]
     c38:	e1822c15 	orr	r2, r2, r5, lsl ip
     c3c:	e5832000 	str	r2, [r3]
     c40:	e3a03000 	mov	r3, #0	; 0x0
     c44:	e5863000 	str	r3, [r6]
     c48:	ea00000c 	b	c80 <pads_chmod+0x104>
     c4c:	e59f2050 	ldr	r2, [pc, #80]	; ca4 <pads_chmod+0x128>
     c50:	e1a03101 	lsl	r3, r1, #2
     c54:	e0832002 	add	r2, r3, r2
     c58:	e5923000 	ldr	r3, [r2]
     c5c:	e2591000 	subs	r1, r9, #0	; 0x0
     c60:	13a01001 	movne	r1, #1	; 0x1
     c64:	e1833c1a 	orr	r3, r3, sl, lsl ip
     c68:	e5823000 	str	r3, [r2]
     c6c:	e586a000 	str	sl, [r6]
     c70:	e5881000 	str	r1, [r8]
     c74:	ea000001 	b	c80 <pads_chmod+0x104>
     c78:	e1a0e00f 	mov	lr, pc
     c7c:	e59bf000 	ldr	pc, [fp]
     c80:	e2844001 	add	r4, r4, #1	; 0x1
     c84:	e2866040 	add	r6, r6, #64	; 0x40
     c88:	e2888040 	add	r8, r8, #64	; 0x40
     c8c:	e1540007 	cmp	r4, r7
     c90:	e2843007 	add	r3, r4, #7	; 0x7
     c94:	daffffd1 	ble	be0 <pads_chmod+0x64>
     c98:	e28dd004 	add	sp, sp, #4	; 0x4
     c9c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     ca0:	0800d76c 	.word	0x0800d76c
     ca4:	2d009004 	.word	0x2d009004
     ca8:	00011b3f 	.word	0x00011b3f

00000cac <pads_pull>:
     cac:	e1b02840 	asrs	r2, r0, #16
     cb0:	e1a00800 	lsl	r0, r0, #16
     cb4:	03a03000 	moveq	r3, #0	; 0x0
     cb8:	13a03001 	movne	r3, #1	; 0x1
     cbc:	e1a00820 	lsr	r0, r0, #16
     cc0:	e1520000 	cmp	r2, r0
     cc4:	a3a03000 	movge	r3, #0	; 0x0
     cc8:	b2033001 	andlt	r3, r3, #1	; 0x1
     ccc:	e3530000 	cmp	r3, #0	; 0x0
     cd0:	112fff1e 	bxne	lr
     cd4:	e1a03300 	lsl	r3, r0, #6
     cd8:	e28332c2 	add	r3, r3, #536870924	; 0x2000000c
     cdc:	e3520000 	cmp	r2, #0	; 0x0
     ce0:	01a02000 	moveq	r2, r0
     ce4:	e283360f 	add	r3, r3, #15728640	; 0xf00000
     ce8:	e3a0c001 	mov	ip, #1	; 0x1
     cec:	ea000004 	b	d04 <pads_pull+0x58>
     cf0:	e3510000 	cmp	r1, #0	; 0x0
     cf4:	05831000 	streq	r1, [r3]
     cf8:	1583c000 	strne	ip, [r3]
     cfc:	e2800001 	add	r0, r0, #1	; 0x1
     d00:	e2833040 	add	r3, r3, #64	; 0x40
     d04:	e1500002 	cmp	r0, r2
     d08:	dafffff8 	ble	cf0 <pads_pull+0x44>
     d0c:	e12fff1e 	bx	lr

00000d10 <pads_setpin>:
     d10:	e1b0c840 	asrs	ip, r0, #16
     d14:	e1a00800 	lsl	r0, r0, #16
     d18:	03a03000 	moveq	r3, #0	; 0x0
     d1c:	13a03001 	movne	r3, #1	; 0x1
     d20:	e1a00820 	lsr	r0, r0, #16
     d24:	e15c0000 	cmp	ip, r0
     d28:	a3a03000 	movge	r3, #0	; 0x0
     d2c:	b2033001 	andlt	r3, r3, #1	; 0x1
     d30:	e3530000 	cmp	r3, #0	; 0x0
     d34:	112fff1e 	bxne	lr
     d38:	e1a03300 	lsl	r3, r0, #6
     d3c:	e2832242 	add	r2, r3, #536870916	; 0x20000004
     d40:	e35c0000 	cmp	ip, #0	; 0x0
     d44:	01a0c000 	moveq	ip, r0
     d48:	e282260f 	add	r2, r2, #15728640	; 0xf00000
     d4c:	ea000002 	b	d5c <pads_setpin+0x4c>
     d50:	e2513000 	subs	r3, r1, #0	; 0x0
     d54:	13a03001 	movne	r3, #1	; 0x1
     d58:	e4823040 	str	r3, [r2], #64
     d5c:	e150000c 	cmp	r0, ip
     d60:	e2800001 	add	r0, r0, #1	; 0x1
     d64:	dafffff9 	ble	d50 <pads_setpin+0x40>
     d68:	e12fff1e 	bx	lr

00000d6c <pads_getpin>:
     d6c:	e59f3014 	ldr	r3, [pc, #20]	; d88 <pads_getpin+0x1c>
     d70:	e6f30070 	uxtah	r0, r3, r0
     d74:	e1a00300 	lsl	r0, r0, #6
     d78:	e5900000 	ldr	r0, [r0]
     d7c:	e2500000 	subs	r0, r0, #0	; 0x0
     d80:	13a00001 	movne	r0, #1	; 0x1
     d84:	e12fff1e 	bx	lr
     d88:	0083c000 	.word	0x0083c000

00000d8c <init_alternate>:
     d8c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     d90:	e59f0010 	ldr	r0, [pc, #16]	; da8 <init_alternate+0x1c>
     d94:	e24dd004 	sub	sp, sp, #4	; 0x4
     d98:	ebfffe5e 	bl	718 <module_enable>
     d9c:	e3a00000 	mov	r0, #0	; 0x0
     da0:	e28dd004 	add	sp, sp, #4	; 0x4
     da4:	e8bd8000 	pop	{pc}
     da8:	2d013c00 	.word	0x2d013c00

00000dac <cpu_recase>:
     dac:	e92d4010 	push	{r4, lr}
     db0:	e59f1098 	ldr	r1, [pc, #152]	; e50 <cpu_recase+0xa4>
     db4:	e3a02004 	mov	r2, #4	; 0x4
     db8:	e1a04000 	mov	r4, r0
     dbc:	eb000127 	bl	1260 <strncmp>
     dc0:	e3500000 	cmp	r0, #0	; 0x0
     dc4:	1a000002 	bne	dd4 <cpu_recase+0x28>
     dc8:	eb0000ba 	bl	10b8 <get_mux>
     dcc:	e3c00004 	bic	r0, r0, #4	; 0x4
     dd0:	ea00000d 	b	e0c <cpu_recase+0x60>
     dd4:	e1a00004 	mov	r0, r4
     dd8:	e59f1074 	ldr	r1, [pc, #116]	; e54 <cpu_recase+0xa8>
     ddc:	e3a02005 	mov	r2, #5	; 0x5
     de0:	eb00011e 	bl	1260 <strncmp>
     de4:	e3500000 	cmp	r0, #0	; 0x0
     de8:	0a000005 	beq	e04 <cpu_recase+0x58>
     dec:	e1a00004 	mov	r0, r4
     df0:	e59f1060 	ldr	r1, [pc, #96]	; e58 <cpu_recase+0xac>
     df4:	e3a02004 	mov	r2, #4	; 0x4
     df8:	eb000118 	bl	1260 <strncmp>
     dfc:	e3500000 	cmp	r0, #0	; 0x0
     e00:	18bd8010 	popne	{r4, pc}
     e04:	eb0000ab 	bl	10b8 <get_mux>
     e08:	e3800004 	orr	r0, r0, #4	; 0x4
     e0c:	e59f3048 	ldr	r3, [pc, #72]	; e5c <cpu_recase+0xb0>
     e10:	e59f4044 	ldr	r4, [pc, #68]	; e5c <cpu_recase+0xb0>
     e14:	e59f1044 	ldr	r1, [pc, #68]	; e60 <cpu_recase+0xb4>
     e18:	e5830000 	str	r0, [r3]
     e1c:	e5942000 	ldr	r2, [r4]
     e20:	e5913000 	ldr	r3, [r1]
     e24:	e1520003 	cmp	r2, r3
     e28:	08bd8010 	popeq	{r4, pc}
     e2c:	e1a00002 	mov	r0, r2
     e30:	e5812000 	str	r2, [r1]
     e34:	eb00009b 	bl	10a8 <set_mux>
     e38:	e5941000 	ldr	r1, [r4]
     e3c:	e59f0020 	ldr	r0, [pc, #32]	; e64 <cpu_recase+0xb8>
     e40:	e59f3020 	ldr	r3, [pc, #32]	; e68 <cpu_recase+0xbc>
     e44:	e1a0e00f 	mov	lr, pc
     e48:	e593f000 	ldr	pc, [r3]
     e4c:	e8bd8010 	pop	{r4, pc}
     e50:	00011b56 	.word	0x00011b56
     e54:	00011b5b 	.word	0x00011b5b
     e58:	000119e6 	.word	0x000119e6
     e5c:	0800d1b0 	.word	0x0800d1b0
     e60:	0800d104 	.word	0x0800d104
     e64:	00011b61 	.word	0x00011b61
     e68:	0800d76c 	.word	0x0800d76c

00000e6c <set_clk>:
     e6c:	e92d4030 	push	{r4, r5, lr}
     e70:	e1d010b2 	ldrh	r1, [r0, #2]
     e74:	e24dd004 	sub	sp, sp, #4	; 0x4
     e78:	e3510000 	cmp	r1, #0	; 0x0
     e7c:	e1a04000 	mov	r4, r0
     e80:	0a000001 	beq	e8c <set_clk+0x20>
     e84:	e3a00001 	mov	r0, #1	; 0x1
     e88:	ebfffdda 	bl	5f8 <set_pll>
     e8c:	e1d410b4 	ldrh	r1, [r4, #4]
     e90:	e3510000 	cmp	r1, #0	; 0x0
     e94:	0a000001 	beq	ea0 <set_clk+0x34>
     e98:	e3a00002 	mov	r0, #2	; 0x2
     e9c:	ebfffdd5 	bl	5f8 <set_pll>
     ea0:	e1d410b6 	ldrh	r1, [r4, #6]
     ea4:	e3510000 	cmp	r1, #0	; 0x0
     ea8:	0a000001 	beq	eb4 <set_clk+0x48>
     eac:	e3a00003 	mov	r0, #3	; 0x3
     eb0:	ebfffdd0 	bl	5f8 <set_pll>
     eb4:	e59f0094 	ldr	r0, [pc, #148]	; f50 <set_clk+0xe4>
     eb8:	e3a01001 	mov	r1, #1	; 0x1
     ebc:	e3a0200b 	mov	r2, #11	; 0xb
     ec0:	ebfffe09 	bl	6ec <module_set_clock>
     ec4:	e59f0088 	ldr	r0, [pc, #136]	; f54 <set_clk+0xe8>
     ec8:	e3a01001 	mov	r1, #1	; 0x1
     ecc:	e3a02013 	mov	r2, #19	; 0x13
     ed0:	ebfffe05 	bl	6ec <module_set_clock>
     ed4:	e59f007c 	ldr	r0, [pc, #124]	; f58 <set_clk+0xec>
     ed8:	e3a01002 	mov	r1, #2	; 0x2
     edc:	e3a0200f 	mov	r2, #15	; 0xf
     ee0:	ebfffe01 	bl	6ec <module_set_clock>
     ee4:	e59f0070 	ldr	r0, [pc, #112]	; f5c <set_clk+0xf0>
     ee8:	e3a01002 	mov	r1, #2	; 0x2
     eec:	e3a02019 	mov	r2, #25	; 0x19
     ef0:	ebfffdfd 	bl	6ec <module_set_clock>
     ef4:	e59f4064 	ldr	r4, [pc, #100]	; f60 <set_clk+0xf4>
     ef8:	e59f5064 	ldr	r5, [pc, #100]	; f64 <set_clk+0xf8>
     efc:	e1a00004 	mov	r0, r4
     f00:	e3a01002 	mov	r1, #2	; 0x2
     f04:	e2844010 	add	r4, r4, #16	; 0x10
     f08:	e3a02003 	mov	r2, #3	; 0x3
     f0c:	ebfffdf6 	bl	6ec <module_set_clock>
     f10:	e1540005 	cmp	r4, r5
     f14:	1afffff8 	bne	efc <set_clk+0x90>
     f18:	e59f0048 	ldr	r0, [pc, #72]	; f68 <set_clk+0xfc>
     f1c:	e3a01003 	mov	r1, #3	; 0x3
     f20:	e3a02013 	mov	r2, #19	; 0x13
     f24:	ebfffdf0 	bl	6ec <module_set_clock>
     f28:	e59f003c 	ldr	r0, [pc, #60]	; f6c <set_clk+0x100>
     f2c:	e3a01001 	mov	r1, #1	; 0x1
     f30:	e3a02004 	mov	r2, #4	; 0x4
     f34:	ebfffdec 	bl	6ec <module_set_clock>
     f38:	e59f0030 	ldr	r0, [pc, #48]	; f70 <set_clk+0x104>
     f3c:	e3a01002 	mov	r1, #2	; 0x2
     f40:	e3a02007 	mov	r2, #7	; 0x7
     f44:	e28dd004 	add	sp, sp, #4	; 0x4
     f48:	e8bd4030 	pop	{r4, r5, lr}
     f4c:	eafffde6 	b	6ec <module_set_clock>
     f50:	2d00a250 	.word	0x2d00a250
     f54:	2d00a2c0 	.word	0x2d00a2c0
     f58:	2d00a240 	.word	0x2d00a240
     f5c:	2d00a290 	.word	0x2d00a290
     f60:	2d00a130 	.word	0x2d00a130
     f64:	2d00a190 	.word	0x2d00a190
     f68:	2d00a260 	.word	0x2d00a260
     f6c:	2d00a2a0 	.word	0x2d00a2a0
     f70:	2d00a360 	.word	0x2d00a360

00000f74 <chip_test>:
     f74:	e59f0044 	ldr	r0, [pc, #68]	; fc0 <end+0x4>
     f78:	e5901000 	ldr	r1, [r0]
     f7c:	e59f2040 	ldr	r2, [pc, #64]	; fc4 <end+0x8>
     f80:	e1520001 	cmp	r2, r1
     f84:	1a00000c 	bne	fbc <end>
     f88:	e5901004 	ldr	r1, [r0, #4]
     f8c:	e59f2034 	ldr	r2, [pc, #52]	; fc8 <end+0xc>
     f90:	e1520001 	cmp	r2, r1
     f94:	1a000008 	bne	fbc <end>
     f98:	e5901008 	ldr	r1, [r0, #8]
     f9c:	e59f2028 	ldr	r2, [pc, #40]	; fcc <end+0x10>
     fa0:	e1520001 	cmp	r2, r1
     fa4:	1a000004 	bne	fbc <end>
     fa8:	e590100c 	ldr	r1, [r0, #12]
     fac:	e59f201c 	ldr	r2, [pc, #28]	; fd0 <end+0x14>
     fb0:	e1520001 	cmp	r2, r1
     fb4:	1a000000 	bne	fbc <end>
     fb8:	e59ff014 	ldr	pc, [pc, #20]	; fd4 <end+0x18>

00000fbc <end>:
     fbc:	e1a0f00e 	mov	pc, lr
     fc0:	08000040 	.word	0x08000040
     fc4:	6f666e69 	.word	0x6f666e69
     fc8:	612d6d74 	.word	0x612d6d74
     fcc:	2d6e696c 	.word	0x2d6e696c
     fd0:	796c6e6f 	.word	0x796c6e6f
     fd4:	08000200 	.word	0x08000200

00000fd8 <module_get_clock>:
     fd8:	e59f30a4 	ldr	r3, [pc, #164]	; 1084 <module_get_clock+0xac>
     fdc:	e1500003 	cmp	r0, r3
     fe0:	0a00001d 	beq	105c <module_get_clock+0x84>
     fe4:	8a00000c 	bhi	101c <module_get_clock+0x44>
     fe8:	e3500003 	cmp	r0, #3	; 0x3
     fec:	0a000022 	beq	107c <module_get_clock+0xa4>
     ff0:	8a000002 	bhi	1000 <module_get_clock+0x28>
     ff4:	e3500001 	cmp	r0, #1	; 0x1
     ff8:	1a00001d 	bne	1074 <module_get_clock+0x9c>
     ffc:	ea00001e 	b	107c <module_get_clock+0xa4>
    1000:	e59f3080 	ldr	r3, [pc, #128]	; 1088 <module_get_clock+0xb0>
    1004:	e1500003 	cmp	r0, r3
    1008:	0a000013 	beq	105c <module_get_clock+0x84>
    100c:	e2833010 	add	r3, r3, #16	; 0x10
    1010:	e1500003 	cmp	r0, r3
    1014:	1a000016 	bne	1074 <module_get_clock+0x9c>
    1018:	ea00000f 	b	105c <module_get_clock+0x84>
    101c:	e59f3068 	ldr	r3, [pc, #104]	; 108c <module_get_clock+0xb4>
    1020:	e1500003 	cmp	r0, r3
    1024:	0a00000c 	beq	105c <module_get_clock+0x84>
    1028:	8a000003 	bhi	103c <module_get_clock+0x64>
    102c:	e2433010 	sub	r3, r3, #16	; 0x10
    1030:	e1500003 	cmp	r0, r3
    1034:	1a00000e 	bne	1074 <module_get_clock+0x9c>
    1038:	ea000009 	b	1064 <module_get_clock+0x8c>
    103c:	e59f304c 	ldr	r3, [pc, #76]	; 1090 <module_get_clock+0xb8>
    1040:	e1500003 	cmp	r0, r3
    1044:	059f0048 	ldreq	r0, [pc, #72]	; 1094 <module_get_clock+0xbc>
    1048:	012fff1e 	bxeq	lr
    104c:	e28330a0 	add	r3, r3, #160	; 0xa0
    1050:	e1500003 	cmp	r0, r3
    1054:	1a000006 	bne	1074 <module_get_clock+0x9c>
    1058:	ea000003 	b	106c <module_get_clock+0x94>
    105c:	e59f0034 	ldr	r0, [pc, #52]	; 1098 <module_get_clock+0xc0>
    1060:	e12fff1e 	bx	lr
    1064:	e59f0030 	ldr	r0, [pc, #48]	; 109c <module_get_clock+0xc4>
    1068:	e12fff1e 	bx	lr
    106c:	e59f002c 	ldr	r0, [pc, #44]	; 10a0 <module_get_clock+0xc8>
    1070:	e12fff1e 	bx	lr
    1074:	e3a00000 	mov	r0, #0	; 0x0
    1078:	e12fff1e 	bx	lr
    107c:	e59f0020 	ldr	r0, [pc, #32]	; 10a4 <module_get_clock+0xcc>
    1080:	e12fff1e 	bx	lr
    1084:	2d00a260 	.word	0x2d00a260
    1088:	2d00a240 	.word	0x2d00a240
    108c:	2d00a2a0 	.word	0x2d00a2a0
    1090:	2d00a2c0 	.word	0x2d00a2c0
    1094:	02dc6c00 	.word	0x02dc6c00
    1098:	016e3600 	.word	0x016e3600
    109c:	02625a00 	.word	0x02625a00
    10a0:	00e4e1c0 	.word	0x00e4e1c0
    10a4:	01312d00 	.word	0x01312d00

000010a8 <set_mux>:
    10a8:	e59f3004 	ldr	r3, [pc, #4]	; 10b4 <set_mux+0xc>
    10ac:	e5830000 	str	r0, [r3]
    10b0:	e12fff1e 	bx	lr
    10b4:	2d009000 	.word	0x2d009000

000010b8 <get_mux>:
    10b8:	e59f3004 	ldr	r3, [pc, #4]	; 10c4 <get_mux+0xc>
    10bc:	e5930000 	ldr	r0, [r3]
    10c0:	e12fff1e 	bx	lr
    10c4:	2d009000 	.word	0x2d009000

000010c8 <efuse_inject>:
    10c8:	e92d4010 	push	{r4, lr}
    10cc:	e24dd030 	sub	sp, sp, #48	; 0x30
    10d0:	e59f109c 	ldr	r1, [pc, #156]	; 1174 <efuse_inject+0xac>
    10d4:	e3a02020 	mov	r2, #32	; 0x20
    10d8:	e1a0000d 	mov	r0, sp
    10dc:	eb000098 	bl	1344 <memcpy>
    10e0:	e3a01000 	mov	r1, #0	; 0x0
    10e4:	e3a02010 	mov	r2, #16	; 0x10
    10e8:	e28d0020 	add	r0, sp, #32	; 0x20
    10ec:	eb00008c 	bl	1324 <memset>
    10f0:	e3a0302e 	mov	r3, #46	; 0x2e
    10f4:	e5cd3020 	strb	r3, [sp, #32]
    10f8:	e59f1078 	ldr	r1, [pc, #120]	; 1178 <efuse_inject+0xb0>
    10fc:	e3a03016 	mov	r3, #22	; 0x16
    1100:	e5cd3021 	strb	r3, [sp, #33]
    1104:	e1a0400d 	mov	r4, sp
    1108:	e3a02000 	mov	r2, #0	; 0x0
    110c:	e7d43002 	ldrb	r3, [r4, r2]
    1110:	e2822001 	add	r2, r2, #1	; 0x1
    1114:	e3520020 	cmp	r2, #32	; 0x20
    1118:	e4813004 	str	r3, [r1], #4
    111c:	1afffffa 	bne	110c <efuse_inject+0x44>
    1120:	e59f1054 	ldr	r1, [pc, #84]	; 117c <efuse_inject+0xb4>
    1124:	e3a02000 	mov	r2, #0	; 0x0
    1128:	e28d0020 	add	r0, sp, #32	; 0x20
    112c:	e7d03002 	ldrb	r3, [r0, r2]
    1130:	e2822001 	add	r2, r2, #1	; 0x1
    1134:	e3520010 	cmp	r2, #16	; 0x10
    1138:	e4813004 	str	r3, [r1], #4
    113c:	1afffffa 	bne	112c <efuse_inject+0x64>
    1140:	e59f3030 	ldr	r3, [pc, #48]	; 1178 <efuse_inject+0xb0>
    1144:	e3a02018 	mov	r2, #24	; 0x18
    1148:	e5832100 	str	r2, [r3, #256]
    114c:	e3a02001 	mov	r2, #1	; 0x1
    1150:	e2433a01 	sub	r3, r3, #4096	; 0x1000
    1154:	e5832000 	str	r2, [r3]
    1158:	e3a04000 	mov	r4, #0	; 0x0
    115c:	e2433a03 	sub	r3, r3, #12288	; 0x3000
    1160:	e5834c34 	str	r4, [r3, #3124]
    1164:	ebfffe67 	bl	b08 <init_efuse>
    1168:	e1a00004 	mov	r0, r4
    116c:	e28dd030 	add	sp, sp, #48	; 0x30
    1170:	e8bd8010 	pop	{r4, pc}
    1174:	000117e4 	.word	0x000117e4
    1178:	2d00e000 	.word	0x2d00e000
    117c:	2d00e080 	.word	0x2d00e080

00001180 <strcpy>:
    1180:	e3a02000 	mov	r2, #0	; 0x0
    1184:	e7d13002 	ldrb	r3, [r1, r2]
    1188:	e3530000 	cmp	r3, #0	; 0x0
    118c:	e7c03002 	strb	r3, [r0, r2]
    1190:	e2822001 	add	r2, r2, #1	; 0x1
    1194:	1afffffa 	bne	1184 <strcpy+0x4>
    1198:	e12fff1e 	bx	lr

0000119c <strncpy>:
    119c:	e3a0c000 	mov	ip, #0	; 0x0
    11a0:	e3520000 	cmp	r2, #0	; 0x0
    11a4:	e2422001 	sub	r2, r2, #1	; 0x1
    11a8:	012fff1e 	bxeq	lr
    11ac:	e7d1300c 	ldrb	r3, [r1, ip]
    11b0:	e3530000 	cmp	r3, #0	; 0x0
    11b4:	e7c0300c 	strb	r3, [r0, ip]
    11b8:	e28cc001 	add	ip, ip, #1	; 0x1
    11bc:	1afffff7 	bne	11a0 <strncpy+0x4>
    11c0:	e12fff1e 	bx	lr

000011c4 <strncat>:
    11c4:	e92d4030 	push	{r4, r5, lr}
    11c8:	e2525000 	subs	r5, r2, #0	; 0x0
    11cc:	e1a04001 	mov	r4, r1
    11d0:	11a0c000 	movne	ip, r0
    11d4:	1a000001 	bne	11e0 <strncat+0x1c>
    11d8:	e8bd8030 	pop	{r4, r5, pc}
    11dc:	e28cc001 	add	ip, ip, #1	; 0x1
    11e0:	e5dc3000 	ldrb	r3, [ip]
    11e4:	e3530000 	cmp	r3, #0	; 0x0
    11e8:	1afffffb 	bne	11dc <strncat+0x18>
    11ec:	ea000004 	b	1204 <strncat+0x40>
    11f0:	e1550002 	cmp	r5, r2
    11f4:	1a000004 	bne	120c <strncat+0x48>
    11f8:	e3a03000 	mov	r3, #0	; 0x0
    11fc:	e5cc3000 	strb	r3, [ip]
    1200:	e8bd8030 	pop	{r4, r5, pc}
    1204:	e1a02003 	mov	r2, r3
    1208:	e1a0100c 	mov	r1, ip
    120c:	e7d43002 	ldrb	r3, [r4, r2]
    1210:	e28cc001 	add	ip, ip, #1	; 0x1
    1214:	e3530000 	cmp	r3, #0	; 0x0
    1218:	e7c13002 	strb	r3, [r1, r2]
    121c:	e2822001 	add	r2, r2, #1	; 0x1
    1220:	1afffff2 	bne	11f0 <strncat+0x2c>
    1224:	e8bd8030 	pop	{r4, r5, pc}

00001228 <strcmp>:
    1228:	e92d4010 	push	{r4, lr}
    122c:	e3a02000 	mov	r2, #0	; 0x0
    1230:	e1a04000 	mov	r4, r0
    1234:	e7d4c002 	ldrb	ip, [r4, r2]
    1238:	e7d13002 	ldrb	r3, [r1, r2]
    123c:	e2822001 	add	r2, r2, #1	; 0x1
    1240:	e063300c 	rsb	r3, r3, ip
    1244:	e6ef0073 	uxtb	r0, r3
    1248:	e3500000 	cmp	r0, #0	; 0x0
    124c:	1a000001 	bne	1258 <strcmp+0x30>
    1250:	e35c0000 	cmp	ip, #0	; 0x0
    1254:	1afffff6 	bne	1234 <strcmp+0xc>
    1258:	e6af0070 	sxtb	r0, r0
    125c:	e8bd8010 	pop	{r4, pc}

00001260 <strncmp>:
    1260:	e92d4030 	push	{r4, r5, lr}
    1264:	e3a0c000 	mov	ip, #0	; 0x0
    1268:	e1a05000 	mov	r5, r0
    126c:	e1a04001 	mov	r4, r1
    1270:	ea000009 	b	129c <strncmp+0x3c>
    1274:	e7d5100c 	ldrb	r1, [r5, ip]
    1278:	e7d4300c 	ldrb	r3, [r4, ip]
    127c:	e2422001 	sub	r2, r2, #1	; 0x1
    1280:	e0633001 	rsb	r3, r3, r1
    1284:	e6ef0073 	uxtb	r0, r3
    1288:	e3500000 	cmp	r0, #0	; 0x0
    128c:	e28cc001 	add	ip, ip, #1	; 0x1
    1290:	1a000004 	bne	12a8 <strncmp+0x48>
    1294:	e3510000 	cmp	r1, #0	; 0x0
    1298:	0a000002 	beq	12a8 <strncmp+0x48>
    129c:	e3520000 	cmp	r2, #0	; 0x0
    12a0:	1afffff3 	bne	1274 <strncmp+0x14>
    12a4:	e1a00002 	mov	r0, r2
    12a8:	e6af0070 	sxtb	r0, r0
    12ac:	e8bd8030 	pop	{r4, r5, pc}

000012b0 <strchr>:
    12b0:	e1a03000 	mov	r3, r0
    12b4:	e6ef1071 	uxtb	r1, r1
    12b8:	ea000002 	b	12c8 <strchr+0x18>
    12bc:	e3500000 	cmp	r0, #0	; 0x0
    12c0:	012fff1e 	bxeq	lr
    12c4:	e2833001 	add	r3, r3, #1	; 0x1
    12c8:	e5d30000 	ldrb	r0, [r3]
    12cc:	e1500001 	cmp	r0, r1
    12d0:	1afffff9 	bne	12bc <strchr+0xc>
    12d4:	e1a00003 	mov	r0, r3
    12d8:	e12fff1e 	bx	lr

000012dc <strlen>:
    12dc:	e1a02000 	mov	r2, r0
    12e0:	ea000000 	b	12e8 <strlen+0xc>
    12e4:	e2800001 	add	r0, r0, #1	; 0x1
    12e8:	e5d03000 	ldrb	r3, [r0]
    12ec:	e3530000 	cmp	r3, #0	; 0x0
    12f0:	1afffffb 	bne	12e4 <strlen+0x8>
    12f4:	e0620000 	rsb	r0, r2, r0
    12f8:	e12fff1e 	bx	lr

000012fc <strnlen>:
    12fc:	e3a02000 	mov	r2, #0	; 0x0
    1300:	e1510002 	cmp	r1, r2
    1304:	e080c002 	add	ip, r0, r2
    1308:	0a000003 	beq	131c <strnlen+0x20>
    130c:	e7d03002 	ldrb	r3, [r0, r2]
    1310:	e2822001 	add	r2, r2, #1	; 0x1
    1314:	e3530000 	cmp	r3, #0	; 0x0
    1318:	1afffff8 	bne	1300 <strnlen+0x4>
    131c:	e060000c 	rsb	r0, r0, ip
    1320:	e12fff1e 	bx	lr

00001324 <memset>:
    1324:	e3a03000 	mov	r3, #0	; 0x0
    1328:	ea000001 	b	1334 <memset+0x10>
    132c:	e7c01003 	strb	r1, [r0, r3]
    1330:	e2833001 	add	r3, r3, #1	; 0x1
    1334:	e3520000 	cmp	r2, #0	; 0x0
    1338:	e2422001 	sub	r2, r2, #1	; 0x1
    133c:	1afffffa 	bne	132c <memset+0x8>
    1340:	e12fff1e 	bx	lr

00001344 <memcpy>:
    1344:	e3a0c000 	mov	ip, #0	; 0x0
    1348:	ea000002 	b	1358 <memcpy+0x14>
    134c:	e7d1300c 	ldrb	r3, [r1, ip]
    1350:	e7c0300c 	strb	r3, [r0, ip]
    1354:	e28cc001 	add	ip, ip, #1	; 0x1
    1358:	e3520000 	cmp	r2, #0	; 0x0
    135c:	e2422001 	sub	r2, r2, #1	; 0x1
    1360:	1afffff9 	bne	134c <memcpy+0x8>
    1364:	e12fff1e 	bx	lr

00001368 <memmove>:
    1368:	e1500001 	cmp	r0, r1
    136c:	e92d4010 	push	{r4, lr}
    1370:	08bd8010 	popeq	{r4, pc}
    1374:	93a0c000 	movls	ip, #0	; 0x0
    1378:	9a000003 	bls	138c <memmove+0x24>
    137c:	ea000006 	b	139c <memmove+0x34>
    1380:	e7d1300c 	ldrb	r3, [r1, ip]
    1384:	e7c0300c 	strb	r3, [r0, ip]
    1388:	e28cc001 	add	ip, ip, #1	; 0x1
    138c:	e3520000 	cmp	r2, #0	; 0x0
    1390:	e2422001 	sub	r2, r2, #1	; 0x1
    1394:	1afffff9 	bne	1380 <memmove+0x18>
    1398:	e8bd8010 	pop	{r4, pc}
    139c:	e0814002 	add	r4, r1, r2
    13a0:	e3a0c000 	mov	ip, #0	; 0x0
    13a4:	e0801002 	add	r1, r0, r2
    13a8:	ea000001 	b	13b4 <memmove+0x4c>
    13ac:	e7d4300c 	ldrb	r3, [r4, ip]
    13b0:	e7c1300c 	strb	r3, [r1, ip]
    13b4:	e3520000 	cmp	r2, #0	; 0x0
    13b8:	e24cc001 	sub	ip, ip, #1	; 0x1
    13bc:	e2422001 	sub	r2, r2, #1	; 0x1
    13c0:	1afffff9 	bne	13ac <memmove+0x44>
    13c4:	e8bd8010 	pop	{r4, pc}

000013c8 <memcmp>:
    13c8:	e92d4030 	push	{r4, r5, lr}
    13cc:	e3a0c000 	mov	ip, #0	; 0x0
    13d0:	e1a04001 	mov	r4, r1
    13d4:	e1a05000 	mov	r5, r0
    13d8:	e1a01002 	mov	r1, r2
    13dc:	ea000005 	b	13f8 <memcmp+0x30>
    13e0:	e7d5200c 	ldrb	r2, [r5, ip]
    13e4:	e7d4300c 	ldrb	r3, [r4, ip]
    13e8:	e28cc001 	add	ip, ip, #1	; 0x1
    13ec:	e0520003 	subs	r0, r2, r3
    13f0:	18bd8030 	popne	{r4, r5, pc}
    13f4:	e2411001 	sub	r1, r1, #1	; 0x1
    13f8:	e3510000 	cmp	r1, #0	; 0x0
    13fc:	1afffff7 	bne	13e0 <memcmp+0x18>
    1400:	e1a00001 	mov	r0, r1
    1404:	e8bd8030 	pop	{r4, r5, pc}

00001408 <so_m_isused>:
    1408:	e5903004 	ldr	r3, [r0, #4]
    140c:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    1410:	03a00000 	moveq	r0, #0	; 0x0
    1414:	13a00001 	movne	r0, #1	; 0x1
    1418:	e12fff1e 	bx	lr

0000141c <so_m_set_used>:
    141c:	e5903004 	ldr	r3, [r0, #4]
    1420:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    1424:	e5803004 	str	r3, [r0, #4]
    1428:	e12fff1e 	bx	lr

0000142c <so_m_unset_used>:
    142c:	e5903004 	ldr	r3, [r0, #4]
    1430:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1434:	e5803004 	str	r3, [r0, #4]
    1438:	e12fff1e 	bx	lr

0000143c <so_m_get_size>:
    143c:	e5900004 	ldr	r0, [r0, #4]
    1440:	e3c00103 	bic	r0, r0, #-1073741824	; 0xc0000000
    1444:	e12fff1e 	bx	lr

00001448 <so_m_set_size>:
    1448:	e5903004 	ldr	r3, [r0, #4]
    144c:	e3c11103 	bic	r1, r1, #-1073741824	; 0xc0000000
    1450:	e2033103 	and	r3, r3, #-1073741824	; 0xc0000000
    1454:	e1811003 	orr	r1, r1, r3
    1458:	e5801004 	str	r1, [r0, #4]
    145c:	e12fff1e 	bx	lr

00001460 <so_m_alloc>:
    1460:	e59f3048 	ldr	r3, [pc, #72]	; 14b0 <so_m_alloc+0x50>
    1464:	e3a02000 	mov	r2, #0	; 0x0
    1468:	e593c000 	ldr	ip, [r3]
    146c:	e5931008 	ldr	r1, [r3, #8]
    1470:	ea000007 	b	1494 <so_m_alloc+0x34>
    1474:	e5903004 	ldr	r3, [r0, #4]
    1478:	e3530000 	cmp	r3, #0	; 0x0
    147c:	1a000004 	bne	1494 <so_m_alloc+0x34>
    1480:	e59f2028 	ldr	r2, [pc, #40]	; 14b0 <so_m_alloc+0x50>
    1484:	e5923004 	ldr	r3, [r2, #4]
    1488:	e2433001 	sub	r3, r3, #1	; 0x1
    148c:	e5823004 	str	r3, [r2, #4]
    1490:	e12fff1e 	bx	lr
    1494:	e152000c 	cmp	r2, ip
    1498:	e1a00001 	mov	r0, r1
    149c:	e2822001 	add	r2, r2, #1	; 0x1
    14a0:	e2811010 	add	r1, r1, #16	; 0x10
    14a4:	3afffff2 	bcc	1474 <so_m_alloc+0x14>
    14a8:	e3a00000 	mov	r0, #0	; 0x0
    14ac:	e12fff1e 	bx	lr
    14b0:	0800d7ac 	.word	0x0800d7ac

000014b4 <so_m_remove>:
    14b4:	e3a03000 	mov	r3, #0	; 0x0
    14b8:	e59f2014 	ldr	r2, [pc, #20]	; 14d4 <so_m_remove+0x20>
    14bc:	e5803000 	str	r3, [r0]
    14c0:	e5803004 	str	r3, [r0, #4]
    14c4:	e5923004 	ldr	r3, [r2, #4]
    14c8:	e2833001 	add	r3, r3, #1	; 0x1
    14cc:	e5823004 	str	r3, [r2, #4]
    14d0:	e12fff1e 	bx	lr
    14d4:	0800d7ac 	.word	0x0800d7ac

000014d8 <malloc>:
    14d8:	e3500000 	cmp	r0, #0	; 0x0
    14dc:	e92d4070 	push	{r4, r5, r6, lr}
    14e0:	0a000047 	beq	1604 <malloc+0x12c>
    14e4:	e59f1120 	ldr	r1, [pc, #288]	; 160c <malloc+0x134>
    14e8:	e280301f 	add	r3, r0, #31	; 0x1f
    14ec:	e5b12014 	ldr	r2, [r1, #20]!
    14f0:	e3c3401f 	bic	r4, r3, #31	; 0x1f
    14f4:	e2425008 	sub	r5, r2, #8	; 0x8
    14f8:	ea000009 	b	1524 <malloc+0x4c>
    14fc:	e5953004 	ldr	r3, [r5, #4]
    1500:	e2130103 	ands	r0, r3, #-1073741824	; 0xc0000000
    1504:	e3c32103 	bic	r2, r3, #-1073741824	; 0xc0000000
    1508:	1a000003 	bne	151c <malloc+0x44>
    150c:	e1520004 	cmp	r2, r4
    1510:	3a000001 	bcc	151c <malloc+0x44>
    1514:	9a000030 	bls	15dc <malloc+0x104>
    1518:	ea000005 	b	1534 <malloc+0x5c>
    151c:	e5953008 	ldr	r3, [r5, #8]
    1520:	e2435008 	sub	r5, r3, #8	; 0x8
    1524:	e2853008 	add	r3, r5, #8	; 0x8
    1528:	e1530001 	cmp	r3, r1
    152c:	1afffff2 	bne	14fc <malloc+0x24>
    1530:	ea000033 	b	1604 <malloc+0x12c>
    1534:	e59f30d0 	ldr	r3, [pc, #208]	; 160c <malloc+0x134>
    1538:	e1a01000 	mov	r1, r0
    153c:	e5932008 	ldr	r2, [r3, #8]
    1540:	e5930000 	ldr	r0, [r3]
    1544:	ea00000a 	b	1574 <malloc+0x9c>
    1548:	e5923004 	ldr	r3, [r2, #4]
    154c:	e2822010 	add	r2, r2, #16	; 0x10
    1550:	e3530000 	cmp	r3, #0	; 0x0
    1554:	1a000006 	bne	1574 <malloc+0x9c>
    1558:	e59f20ac 	ldr	r2, [pc, #172]	; 160c <malloc+0x134>
    155c:	e3560000 	cmp	r6, #0	; 0x0
    1560:	e5923004 	ldr	r3, [r2, #4]
    1564:	e2433001 	sub	r3, r3, #1	; 0x1
    1568:	e5823004 	str	r3, [r2, #4]
    156c:	1a000005 	bne	1588 <malloc+0xb0>
    1570:	ea000023 	b	1604 <malloc+0x12c>
    1574:	e1510000 	cmp	r1, r0
    1578:	e1a06002 	mov	r6, r2
    157c:	e2811001 	add	r1, r1, #1	; 0x1
    1580:	3afffff0 	bcc	1548 <malloc+0x70>
    1584:	ea00001e 	b	1604 <malloc+0x12c>
    1588:	e5953004 	ldr	r3, [r5, #4]
    158c:	e5962004 	ldr	r2, [r6, #4]
    1590:	e0643003 	rsb	r3, r4, r3
    1594:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    1598:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    159c:	e1833002 	orr	r3, r3, r2
    15a0:	e5863004 	str	r3, [r6, #4]
    15a4:	e5951000 	ldr	r1, [r5]
    15a8:	e5950004 	ldr	r0, [r5, #4]
    15ac:	e1a0c005 	mov	ip, r5
    15b0:	e1a03006 	mov	r3, r6
    15b4:	e0841001 	add	r1, r4, r1
    15b8:	e3c42103 	bic	r2, r4, #-1073741824	; 0xc0000000
    15bc:	e2000103 	and	r0, r0, #-1073741824	; 0xc0000000
    15c0:	e5bc4008 	ldr	r4, [ip, #8]!
    15c4:	e4831008 	str	r1, [r3], #8
    15c8:	e1822000 	orr	r2, r2, r0
    15cc:	e5864008 	str	r4, [r6, #8]
    15d0:	e5843004 	str	r3, [r4, #4]
    15d4:	e985000c 	stmib	r5, {r2, r3}
    15d8:	e586c00c 	str	ip, [r6, #12]
    15dc:	e5953004 	ldr	r3, [r5, #4]
    15e0:	e59f1024 	ldr	r1, [pc, #36]	; 160c <malloc+0x134>
    15e4:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    15e8:	e5853004 	str	r3, [r5, #4]
    15ec:	e5912010 	ldr	r2, [r1, #16]
    15f0:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    15f4:	e0833002 	add	r3, r3, r2
    15f8:	e5813010 	str	r3, [r1, #16]
    15fc:	e5950000 	ldr	r0, [r5]
    1600:	e8bd8070 	pop	{r4, r5, r6, pc}
    1604:	e3a00000 	mov	r0, #0	; 0x0
    1608:	e8bd8070 	pop	{r4, r5, r6, pc}
    160c:	0800d7ac 	.word	0x0800d7ac

00001610 <__sOMERGe>:
    1610:	e5902004 	ldr	r2, [r0, #4]
    1614:	e92d4030 	push	{r4, r5, lr}
    1618:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    161c:	e1a05000 	mov	r5, r0
    1620:	e1a0c001 	mov	ip, r1
    1624:	18bd8030 	popne	{r4, r5, pc}
    1628:	e5913004 	ldr	r3, [r1, #4]
    162c:	e2134103 	ands	r4, r3, #-1073741824	; 0xc0000000
    1630:	18bd8030 	popne	{r4, r5, pc}
    1634:	e0823003 	add	r3, r2, r3
    1638:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    163c:	e59f0030 	ldr	r0, [pc, #48]	; 1674 <__sOMERGe+0x64>
    1640:	e5853004 	str	r3, [r5, #4]
    1644:	e5814000 	str	r4, [r1]
    1648:	e5814004 	str	r4, [r1, #4]
    164c:	e5903004 	ldr	r3, [r0, #4]
    1650:	e591100c 	ldr	r1, [r1, #12]
    1654:	e59c2008 	ldr	r2, [ip, #8]
    1658:	e2833001 	add	r3, r3, #1	; 0x1
    165c:	e5812000 	str	r2, [r1]
    1660:	e5821004 	str	r1, [r2, #4]
    1664:	e5803004 	str	r3, [r0, #4]
    1668:	e58c4008 	str	r4, [ip, #8]
    166c:	e58c400c 	str	r4, [ip, #12]
    1670:	e8bd8030 	pop	{r4, r5, pc}
    1674:	0800d7ac 	.word	0x0800d7ac

00001678 <free>:
    1678:	e3500000 	cmp	r0, #0	; 0x0
    167c:	159f2090 	ldrne	r2, [pc, #144]	; 1714 <free+0x9c>
    1680:	e92d4010 	push	{r4, lr}
    1684:	15b23014 	ldrne	r3, [r2, #20]!
    1688:	1a000004 	bne	16a0 <free+0x28>
    168c:	e8bd8010 	pop	{r4, pc}
    1690:	e5943000 	ldr	r3, [r4]
    1694:	e1500003 	cmp	r0, r3
    1698:	0a000005 	beq	16b4 <free+0x3c>
    169c:	e5943008 	ldr	r3, [r4, #8]
    16a0:	e2434008 	sub	r4, r3, #8	; 0x8
    16a4:	e2843008 	add	r3, r4, #8	; 0x8
    16a8:	e1530002 	cmp	r3, r2
    16ac:	1afffff7 	bne	1690 <free+0x18>
    16b0:	e8bd8010 	pop	{r4, pc}
    16b4:	e5943004 	ldr	r3, [r4, #4]
    16b8:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    16bc:	08bd8010 	popeq	{r4, pc}
    16c0:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    16c4:	e59f1048 	ldr	r1, [pc, #72]	; 1714 <free+0x9c>
    16c8:	e5843004 	str	r3, [r4, #4]
    16cc:	e5912010 	ldr	r2, [r1, #16]
    16d0:	e5940008 	ldr	r0, [r4, #8]
    16d4:	e0632002 	rsb	r2, r3, r2
    16d8:	e59f3038 	ldr	r3, [pc, #56]	; 1718 <free+0xa0>
    16dc:	e5812010 	str	r2, [r1, #16]
    16e0:	e1500003 	cmp	r0, r3
    16e4:	0a000002 	beq	16f4 <free+0x7c>
    16e8:	e2401008 	sub	r1, r0, #8	; 0x8
    16ec:	e1a00004 	mov	r0, r4
    16f0:	ebffffc6 	bl	1610 <__sOMERGe>
    16f4:	e594000c 	ldr	r0, [r4, #12]
    16f8:	e59f3018 	ldr	r3, [pc, #24]	; 1718 <free+0xa0>
    16fc:	e1500003 	cmp	r0, r3
    1700:	08bd8010 	popeq	{r4, pc}
    1704:	e2400008 	sub	r0, r0, #8	; 0x8
    1708:	e1a01004 	mov	r1, r4
    170c:	e8bd4010 	pop	{r4, lr}
    1710:	eaffffbe 	b	1610 <__sOMERGe>
    1714:	0800d7ac 	.word	0x0800d7ac
    1718:	0800d7c0 	.word	0x0800d7c0

0000171c <__sOINFo>:
    171c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1720:	e59f50a8 	ldr	r5, [pc, #168]	; 17d0 <__sOINFo+0xb4>
    1724:	e24dd004 	sub	sp, sp, #4	; 0x4
    1728:	e59f60a4 	ldr	r6, [pc, #164]	; 17d4 <__sOINFo+0xb8>
    172c:	e8950006 	ldm	r5, {r1, r2}
    1730:	e59f00a0 	ldr	r0, [pc, #160]	; 17d8 <__sOINFo+0xbc>
    1734:	e1a0e00f 	mov	lr, pc
    1738:	e596f000 	ldr	pc, [r6]
    173c:	e2853008 	add	r3, r5, #8	; 0x8
    1740:	e8930098 	ldm	r3, {r3, r4, r7}
    1744:	e2444b01 	sub	r4, r4, #1024	; 0x400
    1748:	e0634004 	rsb	r4, r3, r4
    174c:	e3a00064 	mov	r0, #100	; 0x64
    1750:	e1a01004 	mov	r1, r4
    1754:	e0000790 	mul	r0, r0, r7
    1758:	eb003a68 	bl	10100 <__aeabi_uidiv>
    175c:	e1a01004 	mov	r1, r4
    1760:	e1a03000 	mov	r3, r0
    1764:	e1a02007 	mov	r2, r7
    1768:	e59f006c 	ldr	r0, [pc, #108]	; 17dc <__sOINFo+0xc0>
    176c:	e1a0e00f 	mov	lr, pc
    1770:	e596f000 	ldr	pc, [r6]
    1774:	e59f0064 	ldr	r0, [pc, #100]	; 17e0 <__sOINFo+0xc4>
    1778:	e1a0e00f 	mov	lr, pc
    177c:	e596f000 	ldr	pc, [r6]
    1780:	e59f005c 	ldr	r0, [pc, #92]	; 17e4 <__sOINFo+0xc8>
    1784:	e1a0e00f 	mov	lr, pc
    1788:	e596f000 	ldr	pc, [r6]
    178c:	e5b53014 	ldr	r3, [r5, #20]!
    1790:	ea000007 	b	17b4 <__sOINFo+0x98>
    1794:	e8940006 	ldm	r4, {r1, r2}
    1798:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    179c:	03a03000 	moveq	r3, #0	; 0x0
    17a0:	13a03001 	movne	r3, #1	; 0x1
    17a4:	e3c22103 	bic	r2, r2, #-1073741824	; 0xc0000000
    17a8:	e1a0e00f 	mov	lr, pc
    17ac:	e596f000 	ldr	pc, [r6]
    17b0:	e5943008 	ldr	r3, [r4, #8]
    17b4:	e2434008 	sub	r4, r3, #8	; 0x8
    17b8:	e2843008 	add	r3, r4, #8	; 0x8
    17bc:	e1530005 	cmp	r3, r5
    17c0:	e59f0020 	ldr	r0, [pc, #32]	; 17e8 <__sOINFo+0xcc>
    17c4:	1afffff2 	bne	1794 <__sOINFo+0x78>
    17c8:	e28dd004 	add	sp, sp, #4	; 0x4
    17cc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    17d0:	0800d7ac 	.word	0x0800d7ac
    17d4:	0800d76c 	.word	0x0800d76c
    17d8:	00011b77 	.word	0x00011b77
    17dc:	00011b91 	.word	0x00011b91
    17e0:	00011bb6 	.word	0x00011bb6
    17e4:	00011a64 	.word	0x00011a64
    17e8:	00011bc7 	.word	0x00011bc7

000017ec <init_malloc>:
    17ec:	e59f20e4 	ldr	r2, [pc, #228]	; 18d8 <init_malloc+0xec>
    17f0:	e59f30e4 	ldr	r3, [pc, #228]	; 18dc <init_malloc+0xf0>
    17f4:	e3c2001f 	bic	r0, r2, #31	; 0x1f
    17f8:	e3c3301f 	bic	r3, r3, #31	; 0x1f
    17fc:	e0602003 	rsb	r2, r0, r3
    1800:	e92d4010 	push	{r4, lr}
    1804:	e59f40d4 	ldr	r4, [pc, #212]	; 18e0 <init_malloc+0xf4>
    1808:	e3520b01 	cmp	r2, #1024	; 0x400
    180c:	e5840008 	str	r0, [r4, #8]
    1810:	e584300c 	str	r3, [r4, #12]
    1814:	93e00000 	mvnls	r0, #0	; 0x0
    1818:	98bd8010 	popls	{r4, pc}
    181c:	e284c014 	add	ip, r4, #20	; 0x14
    1820:	e3a03040 	mov	r3, #64	; 0x40
    1824:	e3a01000 	mov	r1, #0	; 0x0
    1828:	e3a02b01 	mov	r2, #1024	; 0x400
    182c:	e584c018 	str	ip, [r4, #24]
    1830:	e584c014 	str	ip, [r4, #20]
    1834:	e5843004 	str	r3, [r4, #4]
    1838:	e5843000 	str	r3, [r4]
    183c:	ebfffeb8 	bl	1324 <memset>
    1840:	e5940008 	ldr	r0, [r4, #8]
    1844:	e594c000 	ldr	ip, [r4]
    1848:	e1a01000 	mov	r1, r0
    184c:	e3a02000 	mov	r2, #0	; 0x0
    1850:	ea000006 	b	1870 <init_malloc+0x84>
    1854:	e59e3004 	ldr	r3, [lr, #4]
    1858:	e3530000 	cmp	r3, #0	; 0x0
    185c:	059f207c 	ldreq	r2, [pc, #124]	; 18e0 <init_malloc+0xf4>
    1860:	05923004 	ldreq	r3, [r2, #4]
    1864:	02433001 	subeq	r3, r3, #1	; 0x1
    1868:	05823004 	streq	r3, [r2, #4]
    186c:	0a000005 	beq	1888 <init_malloc+0x9c>
    1870:	e152000c 	cmp	r2, ip
    1874:	e1a0e001 	mov	lr, r1
    1878:	e2822001 	add	r2, r2, #1	; 0x1
    187c:	e2811010 	add	r1, r1, #16	; 0x10
    1880:	3afffff3 	bcc	1854 <init_malloc+0x68>
    1884:	e3a0e000 	mov	lr, #0	; 0x0
    1888:	e59fc050 	ldr	ip, [pc, #80]	; 18e0 <init_malloc+0xf4>
    188c:	e59e2004 	ldr	r2, [lr, #4]
    1890:	e59c300c 	ldr	r3, [ip, #12]
    1894:	e1a0100e 	mov	r1, lr
    1898:	e2433b01 	sub	r3, r3, #1024	; 0x400
    189c:	e0603003 	rsb	r3, r0, r3
    18a0:	e59c4018 	ldr	r4, [ip, #24]
    18a4:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    18a8:	e2800b01 	add	r0, r0, #1024	; 0x400
    18ac:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    18b0:	e4810008 	str	r0, [r1], #8
    18b4:	e1833002 	orr	r3, r3, r2
    18b8:	e28c2014 	add	r2, ip, #20	; 0x14
    18bc:	e58e2008 	str	r2, [lr, #8]
    18c0:	e58c1018 	str	r1, [ip, #24]
    18c4:	e5841000 	str	r1, [r4]
    18c8:	e58e3004 	str	r3, [lr, #4]
    18cc:	e58e400c 	str	r4, [lr, #12]
    18d0:	e3a00000 	mov	r0, #0	; 0x0
    18d4:	e8bd8010 	pop	{r4, pc}
    18d8:	0800f01f 	.word	0x0800f01f
    18dc:	0801a000 	.word	0x0801a000
    18e0:	0800d7ac 	.word	0x0800d7ac

000018e4 <isi_check_header>:
    18e4:	e92d4070 	push	{r4, r5, r6, lr}
    18e8:	e5902000 	ldr	r2, [r0]
    18ec:	e59f3088 	ldr	r3, [pc, #136]	; 197c <isi_check_header+0x98>
    18f0:	e1a05000 	mov	r5, r0
    18f4:	e1520003 	cmp	r2, r3
    18f8:	e24dd008 	sub	sp, sp, #8	; 0x8
    18fc:	13e0001e 	mvnne	r0, #30	; 0x1e
    1900:	1a00001b 	bne	1974 <isi_check_header+0x90>
    1904:	e3a06000 	mov	r6, #0	; 0x0
    1908:	e595401c 	ldr	r4, [r5, #28]
    190c:	e3a01c02 	mov	r1, #512	; 0x200
    1910:	e585601c 	str	r6, [r5, #28]
    1914:	e1a00006 	mov	r0, r6
    1918:	eb000f63 	bl	56ac <hash_init>
    191c:	e3a01c02 	mov	r1, #512	; 0x200
    1920:	e1a00005 	mov	r0, r5
    1924:	eb000f3b 	bl	5618 <hash_data>
    1928:	e28d0004 	add	r0, sp, #4	; 0x4
    192c:	eb000f1d 	bl	55a8 <hash_value>
    1930:	e585401c 	str	r4, [r5, #28]
    1934:	e59d3004 	ldr	r3, [sp, #4]
    1938:	e1540003 	cmp	r4, r3
    193c:	13e0001f 	mvnne	r0, #31	; 0x1f
    1940:	1a00000b 	bne	1974 <isi_check_header+0x90>
    1944:	e5951004 	ldr	r1, [r5, #4]
    1948:	e59f4030 	ldr	r4, [pc, #48]	; 1980 <isi_check_header+0x9c>
    194c:	e3c114ff 	bic	r1, r1, #-16777216	; 0xff000000
    1950:	e59f002c 	ldr	r0, [pc, #44]	; 1984 <isi_check_header+0xa0>
    1954:	e1a0e00f 	mov	lr, pc
    1958:	e594f000 	ldr	pc, [r4]
    195c:	e5d51007 	ldrb	r1, [r5, #7]
    1960:	e59f0020 	ldr	r0, [pc, #32]	; 1988 <isi_check_header+0xa4>
    1964:	e201100f 	and	r1, r1, #15	; 0xf
    1968:	e1a0e00f 	mov	lr, pc
    196c:	e594f000 	ldr	pc, [r4]
    1970:	e1a00006 	mov	r0, r6
    1974:	e28dd008 	add	sp, sp, #8	; 0x8
    1978:	e8bd8070 	pop	{r4, r5, r6, pc}
    197c:	6973692b 	.word	0x6973692b
    1980:	0800d76c 	.word	0x0800d76c
    1984:	00011bef 	.word	0x00011bef
    1988:	00011bfd 	.word	0x00011bfd

0000198c <isi_check_data_separate>:
    198c:	e92d4070 	push	{r4, r5, r6, lr}
    1990:	e1a05000 	mov	r5, r0
    1994:	e5d00007 	ldrb	r0, [r0, #7]
    1998:	e24dd028 	sub	sp, sp, #40	; 0x28
    199c:	e1a04001 	mov	r4, r1
    19a0:	e200000f 	and	r0, r0, #15	; 0xf
    19a4:	e595100c 	ldr	r1, [r5, #12]
    19a8:	eb000f3f 	bl	56ac <hash_init>
    19ac:	e28d6008 	add	r6, sp, #8	; 0x8
    19b0:	e595100c 	ldr	r1, [r5, #12]
    19b4:	e1a00004 	mov	r0, r4
    19b8:	eb000f16 	bl	5618 <hash_data>
    19bc:	e1a00006 	mov	r0, r6
    19c0:	eb000ef8 	bl	55a8 <hash_value>
    19c4:	eb000ef0 	bl	558c <hash_len>
    19c8:	e2854020 	add	r4, r5, #32	; 0x20
    19cc:	e1a02000 	mov	r2, r0
    19d0:	e1a01004 	mov	r1, r4
    19d4:	e1a00006 	mov	r0, r6
    19d8:	ebfffe7a 	bl	13c8 <memcmp>
    19dc:	e3500000 	cmp	r0, #0	; 0x0
    19e0:	0a000014 	beq	1a38 <isi_check_data_separate+0xac>
    19e4:	e595100c 	ldr	r1, [r5, #12]
    19e8:	e59f3050 	ldr	r3, [pc, #80]	; 1a40 <isi_check_data_separate+0xb4>
    19ec:	e59f0050 	ldr	r0, [pc, #80]	; 1a44 <isi_check_data_separate+0xb8>
    19f0:	e1a0e00f 	mov	lr, pc
    19f4:	e593f000 	ldr	pc, [r3]
    19f8:	eb000ee3 	bl	558c <hash_len>
    19fc:	e1a01004 	mov	r1, r4
    1a00:	e1a03000 	mov	r3, r0
    1a04:	e3a02001 	mov	r2, #1	; 0x1
    1a08:	e1a00004 	mov	r0, r4
    1a0c:	e3a04020 	mov	r4, #32	; 0x20
    1a10:	e58d4000 	str	r4, [sp]
    1a14:	eb002d1d 	bl	ce90 <print_buffer>
    1a18:	eb000edb 	bl	558c <hash_len>
    1a1c:	e1a01006 	mov	r1, r6
    1a20:	e1a03000 	mov	r3, r0
    1a24:	e3a02001 	mov	r2, #1	; 0x1
    1a28:	e1a00006 	mov	r0, r6
    1a2c:	e58d4000 	str	r4, [sp]
    1a30:	eb002d16 	bl	ce90 <print_buffer>
    1a34:	e3e00020 	mvn	r0, #32	; 0x20
    1a38:	e28dd028 	add	sp, sp, #40	; 0x28
    1a3c:	e8bd8070 	pop	{r4, r5, r6, pc}
    1a40:	0800d76c 	.word	0x0800d76c
    1a44:	00011c10 	.word	0x00011c10

00001a48 <isi_check_data>:
    1a48:	e2801c02 	add	r1, r0, #512	; 0x200
    1a4c:	eaffffce 	b	198c <isi_check_data_separate>

00001a50 <isi_check>:
    1a50:	e92d4010 	push	{r4, lr}
    1a54:	e1a04000 	mov	r4, r0
    1a58:	ebffffa1 	bl	18e4 <isi_check_header>
    1a5c:	e3500000 	cmp	r0, #0	; 0x0
    1a60:	18bd8010 	popne	{r4, pc}
    1a64:	e1a00004 	mov	r0, r4
    1a68:	e8bd4010 	pop	{r4, lr}
    1a6c:	eafffff5 	b	1a48 <isi_check_data>

00001a70 <load_serial_ymodem>:
    1a70:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    1a74:	e24dde41 	sub	sp, sp, #1040	; 0x410
    1a78:	e28d4b01 	add	r4, sp, #1024	; 0x400
    1a7c:	e284400c 	add	r4, r4, #12	; 0xc
    1a80:	e3a03002 	mov	r3, #2	; 0x2
    1a84:	e1a07000 	mov	r7, r0
    1a88:	e1a01004 	mov	r1, r4
    1a8c:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1a90:	e58d3404 	str	r3, [sp, #1028]
    1a94:	eb00391f 	bl	ff18 <xyzModem_stream_open>
    1a98:	e3500000 	cmp	r0, #0	; 0x0
    1a9c:	1a000012 	bne	1aec <load_serial_ymodem+0x7c>
    1aa0:	e1a08004 	mov	r8, r4
    1aa4:	e1a05000 	mov	r5, r0
    1aa8:	e1a04000 	mov	r4, r0
    1aac:	e1a0600d 	mov	r6, sp
    1ab0:	ea000001 	b	1abc <load_serial_ymodem+0x4c>
    1ab4:	e085500c 	add	r5, r5, ip
    1ab8:	ebfffe21 	bl	1344 <memcpy>
    1abc:	e3a01b01 	mov	r1, #1024	; 0x400
    1ac0:	e1a02008 	mov	r2, r8
    1ac4:	e1a0000d 	mov	r0, sp
    1ac8:	eb00387e 	bl	fcc8 <xyzModem_stream_read>
    1acc:	e0843007 	add	r3, r4, r7
    1ad0:	e250c000 	subs	ip, r0, #0	; 0x0
    1ad4:	e1a0100d 	mov	r1, sp
    1ad8:	e1a0200c 	mov	r2, ip
    1adc:	e1a00003 	mov	r0, r3
    1ae0:	e084400c 	add	r4, r4, ip
    1ae4:	cafffff2 	bgt	1ab4 <load_serial_ymodem+0x44>
    1ae8:	ea000007 	b	1b0c <load_serial_ymodem+0x9c>
    1aec:	e59f3054 	ldr	r3, [pc, #84]	; 1b48 <load_serial_ymodem+0xd8>
    1af0:	e59d040c 	ldr	r0, [sp, #1036]
    1af4:	e5934000 	ldr	r4, [r3]
    1af8:	eb0034f9 	bl	eee4 <xyzModem_error>
    1afc:	e1a01000 	mov	r1, r0
    1b00:	e59f0044 	ldr	r0, [pc, #68]	; 1b4c <load_serial_ymodem+0xdc>
    1b04:	e12fff34 	blx	r4
    1b08:	e3a05000 	mov	r5, #0	; 0x0
    1b0c:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1b10:	e280000c 	add	r0, r0, #12	; 0xc
    1b14:	eb0034da 	bl	ee84 <xyzModem_stream_close>
    1b18:	e3a00000 	mov	r0, #0	; 0x0
    1b1c:	e59f102c 	ldr	r1, [pc, #44]	; 1b50 <load_serial_ymodem+0xe0>
    1b20:	eb003788 	bl	f948 <xyzModem_stream_terminate>
    1b24:	e1a01005 	mov	r1, r5
    1b28:	e59f0024 	ldr	r0, [pc, #36]	; 1b54 <load_serial_ymodem+0xe4>
    1b2c:	e1a02005 	mov	r2, r5
    1b30:	e59f3010 	ldr	r3, [pc, #16]	; 1b48 <load_serial_ymodem+0xd8>
    1b34:	e1a0e00f 	mov	lr, pc
    1b38:	e593f000 	ldr	pc, [r3]
    1b3c:	e1a00007 	mov	r0, r7
    1b40:	e28dde41 	add	sp, sp, #1040	; 0x410
    1b44:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    1b48:	0800d76c 	.word	0x0800d76c
    1b4c:	00011d0c 	.word	0x00011d0c
    1b50:	00001b58 	.word	0x00001b58
    1b54:	00011c34 	.word	0x00011c34

00001b58 <getcxmodem>:
    1b58:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    1b5c:	e24dd004 	sub	sp, sp, #4	; 0x4
    1b60:	eb003179 	bl	e14c <tstc>
    1b64:	e3500000 	cmp	r0, #0	; 0x0
    1b68:	1a000002 	bne	1b78 <getcxmodem+0x20>
    1b6c:	e3e00000 	mvn	r0, #0	; 0x0
    1b70:	e28dd004 	add	sp, sp, #4	; 0x4
    1b74:	e8bd8000 	pop	{pc}
    1b78:	e28dd004 	add	sp, sp, #4	; 0x4
    1b7c:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    1b80:	ea003172 	b	e150 <getc>

00001b84 <do_load_serial_bin>:
    1b84:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1b88:	e3520001 	cmp	r2, #1	; 0x1
    1b8c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1b90:	e1a06002 	mov	r6, r2
    1b94:	e1a07000 	mov	r7, r0
    1b98:	e1a05003 	mov	r5, r3
    1b9c:	ca000006 	bgt	1bbc <do_load_serial_bin+0x38>
    1ba0:	e59f0120 	ldr	r0, [pc, #288]	; 1cc8 <do_load_serial_bin+0x144>
    1ba4:	e3a06000 	mov	r6, #0	; 0x0
    1ba8:	e59f311c 	ldr	r3, [pc, #284]	; 1ccc <do_load_serial_bin+0x148>
    1bac:	e1a0e00f 	mov	lr, pc
    1bb0:	e593f000 	ldr	pc, [r3]
    1bb4:	e1a04006 	mov	r4, r6
    1bb8:	ea00000c 	b	1bf0 <do_load_serial_bin+0x6c>
    1bbc:	e5930004 	ldr	r0, [r3, #4]
    1bc0:	e3a01000 	mov	r1, #0	; 0x0
    1bc4:	e3a02010 	mov	r2, #16	; 0x10
    1bc8:	eb002d48 	bl	d0f0 <simple_strtoul>
    1bcc:	e3560003 	cmp	r6, #3	; 0x3
    1bd0:	e1a04000 	mov	r4, r0
    1bd4:	13a06000 	movne	r6, #0	; 0x0
    1bd8:	1a000004 	bne	1bf0 <do_load_serial_bin+0x6c>
    1bdc:	e5950008 	ldr	r0, [r5, #8]
    1be0:	e3a01000 	mov	r1, #0	; 0x0
    1be4:	e3a02010 	mov	r2, #16	; 0x10
    1be8:	eb002d40 	bl	d0f0 <simple_strtoul>
    1bec:	e1a06000 	mov	r6, r0
    1bf0:	e5950000 	ldr	r0, [r5]
    1bf4:	e59f10d4 	ldr	r1, [pc, #212]	; 1cd0 <do_load_serial_bin+0x14c>
    1bf8:	ebfffd8a 	bl	1228 <strcmp>
    1bfc:	e3500000 	cmp	r0, #0	; 0x0
    1c00:	1a000007 	bne	1c24 <do_load_serial_bin+0xa0>
    1c04:	e1a01004 	mov	r1, r4
    1c08:	e59f00c4 	ldr	r0, [pc, #196]	; 1cd4 <do_load_serial_bin+0x150>
    1c0c:	e59f30b8 	ldr	r3, [pc, #184]	; 1ccc <do_load_serial_bin+0x148>
    1c10:	e1a0e00f 	mov	lr, pc
    1c14:	e593f000 	ldr	pc, [r3]
    1c18:	e1a00004 	mov	r0, r4
    1c1c:	ebffff93 	bl	1a70 <load_serial_ymodem>
    1c20:	ea000025 	b	1cbc <do_load_serial_bin+0x138>
    1c24:	e5950000 	ldr	r0, [r5]
    1c28:	e59f10a8 	ldr	r1, [pc, #168]	; 1cd8 <do_load_serial_bin+0x154>
    1c2c:	ebfffd7d 	bl	1228 <strcmp>
    1c30:	e3500000 	cmp	r0, #0	; 0x0
    1c34:	e59f5090 	ldr	r5, [pc, #144]	; 1ccc <do_load_serial_bin+0x148>
    1c38:	1a00001b 	bne	1cac <do_load_serial_bin+0x128>
    1c3c:	e1a02006 	mov	r2, r6
    1c40:	e59f0094 	ldr	r0, [pc, #148]	; 1cdc <do_load_serial_bin+0x158>
    1c44:	e1a01004 	mov	r1, r4
    1c48:	e1a0e00f 	mov	lr, pc
    1c4c:	e595f000 	ldr	pc, [r5]
    1c50:	e1a00004 	mov	r0, r4
    1c54:	e1a01006 	mov	r1, r6
    1c58:	eb00369d 	bl	f6d4 <control_send>
    1c5c:	e3500000 	cmp	r0, #0	; 0x0
    1c60:	aa000005 	bge	1c7c <do_load_serial_bin+0xf8>
    1c64:	e59d0004 	ldr	r0, [sp, #4]
    1c68:	e5954000 	ldr	r4, [r5]
    1c6c:	eb00349c 	bl	eee4 <xyzModem_error>
    1c70:	e1a01000 	mov	r1, r0
    1c74:	e59f0064 	ldr	r0, [pc, #100]	; 1ce0 <do_load_serial_bin+0x15c>
    1c78:	e12fff34 	blx	r4
    1c7c:	e28d0004 	add	r0, sp, #4	; 0x4
    1c80:	eb00347f 	bl	ee84 <xyzModem_stream_close>
    1c84:	e3a00000 	mov	r0, #0	; 0x0
    1c88:	e59f1054 	ldr	r1, [pc, #84]	; 1ce4 <do_load_serial_bin+0x160>
    1c8c:	eb00372d 	bl	f948 <xyzModem_stream_terminate>
    1c90:	e1a01006 	mov	r1, r6
    1c94:	e59f004c 	ldr	r0, [pc, #76]	; 1ce8 <do_load_serial_bin+0x164>
    1c98:	e1a02006 	mov	r2, r6
    1c9c:	e59f3028 	ldr	r3, [pc, #40]	; 1ccc <do_load_serial_bin+0x148>
    1ca0:	e1a0e00f 	mov	lr, pc
    1ca4:	e593f000 	ldr	pc, [r3]
    1ca8:	ea000003 	b	1cbc <do_load_serial_bin+0x138>
    1cac:	e5971014 	ldr	r1, [r7, #20]
    1cb0:	e59f0034 	ldr	r0, [pc, #52]	; 1cec <do_load_serial_bin+0x168>
    1cb4:	e1a0e00f 	mov	lr, pc
    1cb8:	e595f000 	ldr	pc, [r5]
    1cbc:	e3a00000 	mov	r0, #0	; 0x0
    1cc0:	e28dd00c 	add	sp, sp, #12	; 0xc
    1cc4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1cc8:	00011c5c 	.word	0x00011c5c
    1ccc:	0800d76c 	.word	0x0800d76c
    1cd0:	00011c76 	.word	0x00011c76
    1cd4:	00011c79 	.word	0x00011c79
    1cd8:	00011cbc 	.word	0x00011cbc
    1cdc:	00011cbf 	.word	0x00011cbf
    1ce0:	00011d0c 	.word	0x00011d0c
    1ce4:	00001b58 	.word	0x00001b58
    1ce8:	00011c34 	.word	0x00011c34
    1cec:	00011d0b 	.word	0x00011d0b

00001cf0 <do_mem_cp>:
    1cf0:	e3520004 	cmp	r2, #4	; 0x4
    1cf4:	e92d4070 	push	{r4, r5, r6, lr}
    1cf8:	e1a04003 	mov	r4, r3
    1cfc:	0a000002 	beq	1d0c <do_mem_cp+0x1c>
    1d00:	eb0003ad 	bl	2bbc <cmd_usage>
    1d04:	e3a00001 	mov	r0, #1	; 0x1
    1d08:	e8bd8070 	pop	{r4, r5, r6, pc}
    1d0c:	e5930004 	ldr	r0, [r3, #4]
    1d10:	e3a01000 	mov	r1, #0	; 0x0
    1d14:	e3a02010 	mov	r2, #16	; 0x10
    1d18:	eb002cf4 	bl	d0f0 <simple_strtoul>
    1d1c:	e3a01000 	mov	r1, #0	; 0x0
    1d20:	e3a02010 	mov	r2, #16	; 0x10
    1d24:	e1a06000 	mov	r6, r0
    1d28:	e5940008 	ldr	r0, [r4, #8]
    1d2c:	eb002cef 	bl	d0f0 <simple_strtoul>
    1d30:	e3a01000 	mov	r1, #0	; 0x0
    1d34:	e1a05000 	mov	r5, r0
    1d38:	e3a02010 	mov	r2, #16	; 0x10
    1d3c:	e594000c 	ldr	r0, [r4, #12]
    1d40:	eb002cea 	bl	d0f0 <simple_strtoul>
    1d44:	e2800003 	add	r0, r0, #3	; 0x3
    1d48:	e1b00120 	lsrs	r0, r0, #2
    1d4c:	1a000002 	bne	1d5c <do_mem_cp+0x6c>
    1d50:	ea000006 	b	1d70 <do_mem_cp+0x80>
    1d54:	e5163004 	ldr	r3, [r6, #-4]
    1d58:	e5053004 	str	r3, [r5, #-4]
    1d5c:	e3500000 	cmp	r0, #0	; 0x0
    1d60:	e2866004 	add	r6, r6, #4	; 0x4
    1d64:	e2400001 	sub	r0, r0, #1	; 0x1
    1d68:	e2855004 	add	r5, r5, #4	; 0x4
    1d6c:	1afffff8 	bne	1d54 <do_mem_cp+0x64>
    1d70:	e3a00000 	mov	r0, #0	; 0x0
    1d74:	e8bd8070 	pop	{r4, r5, r6, pc}

00001d78 <do_mem_cmp>:
    1d78:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1d7c:	e3520004 	cmp	r2, #4	; 0x4
    1d80:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1d84:	e1a04003 	mov	r4, r3
    1d88:	0a000002 	beq	1d98 <do_mem_cmp+0x20>
    1d8c:	eb00038a 	bl	2bbc <cmd_usage>
    1d90:	e3a04001 	mov	r4, #1	; 0x1
    1d94:	ea000029 	b	1e40 <do_mem_cmp+0xc8>
    1d98:	e5930004 	ldr	r0, [r3, #4]
    1d9c:	e3a01000 	mov	r1, #0	; 0x0
    1da0:	e3a02010 	mov	r2, #16	; 0x10
    1da4:	eb002cd1 	bl	d0f0 <simple_strtoul>
    1da8:	e3a01000 	mov	r1, #0	; 0x0
    1dac:	e3a02010 	mov	r2, #16	; 0x10
    1db0:	e1a07000 	mov	r7, r0
    1db4:	e5940008 	ldr	r0, [r4, #8]
    1db8:	eb002ccc 	bl	d0f0 <simple_strtoul>
    1dbc:	e3a01000 	mov	r1, #0	; 0x0
    1dc0:	e1a06000 	mov	r6, r0
    1dc4:	e3a02010 	mov	r2, #16	; 0x10
    1dc8:	e594000c 	ldr	r0, [r4, #12]
    1dcc:	eb002cc7 	bl	d0f0 <simple_strtoul>
    1dd0:	e2800003 	add	r0, r0, #3	; 0x3
    1dd4:	e1a00120 	lsr	r0, r0, #2
    1dd8:	e3a05000 	mov	r5, #0	; 0x0
    1ddc:	ea00000f 	b	1e20 <do_mem_cmp+0xa8>
    1de0:	e5972000 	ldr	r2, [r7]
    1de4:	e5963000 	ldr	r3, [r6]
    1de8:	e1520003 	cmp	r2, r3
    1dec:	0a000008 	beq	1e14 <do_mem_cmp+0x9c>
    1df0:	e58d3000 	str	r3, [sp]
    1df4:	e1a01007 	mov	r1, r7
    1df8:	e1a03006 	mov	r3, r6
    1dfc:	e59f0048 	ldr	r0, [pc, #72]	; 1e4c <do_mem_cmp+0xd4>
    1e00:	e59fc048 	ldr	ip, [pc, #72]	; 1e50 <do_mem_cmp+0xd8>
    1e04:	e1a0e00f 	mov	lr, pc
    1e08:	e59cf000 	ldr	pc, [ip]
    1e0c:	e3a04001 	mov	r4, #1	; 0x1
    1e10:	ea000005 	b	1e2c <do_mem_cmp+0xb4>
    1e14:	e2855001 	add	r5, r5, #1	; 0x1
    1e18:	e2877004 	add	r7, r7, #4	; 0x4
    1e1c:	e2866004 	add	r6, r6, #4	; 0x4
    1e20:	e1500005 	cmp	r0, r5
    1e24:	1affffed 	bne	1de0 <do_mem_cmp+0x68>
    1e28:	e3a04000 	mov	r4, #0	; 0x0
    1e2c:	e1a01005 	mov	r1, r5
    1e30:	e59f001c 	ldr	r0, [pc, #28]	; 1e54 <do_mem_cmp+0xdc>
    1e34:	e59f3014 	ldr	r3, [pc, #20]	; 1e50 <do_mem_cmp+0xd8>
    1e38:	e1a0e00f 	mov	lr, pc
    1e3c:	e593f000 	ldr	pc, [r3]
    1e40:	e1a00004 	mov	r0, r4
    1e44:	e28dd00c 	add	sp, sp, #12	; 0xc
    1e48:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1e4c:	00011e11 	.word	0x00011e11
    1e50:	0800d76c 	.word	0x0800d76c
    1e54:	00011e49 	.word	0x00011e49

00001e58 <do_mem_mw>:
    1e58:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1e5c:	e1a06002 	mov	r6, r2
    1e60:	e2422003 	sub	r2, r2, #3	; 0x3
    1e64:	e3520001 	cmp	r2, #1	; 0x1
    1e68:	e24dd004 	sub	sp, sp, #4	; 0x4
    1e6c:	e1a04003 	mov	r4, r3
    1e70:	9a000002 	bls	1e80 <do_mem_mw+0x28>
    1e74:	eb000350 	bl	2bbc <cmd_usage>
    1e78:	e3a00001 	mov	r0, #1	; 0x1
    1e7c:	ea000016 	b	1edc <do_mem_mw+0x84>
    1e80:	e3a01000 	mov	r1, #0	; 0x0
    1e84:	e3a02010 	mov	r2, #16	; 0x10
    1e88:	e5930004 	ldr	r0, [r3, #4]
    1e8c:	eb002c97 	bl	d0f0 <simple_strtoul>
    1e90:	e3a01000 	mov	r1, #0	; 0x0
    1e94:	e1a05000 	mov	r5, r0
    1e98:	e3a02010 	mov	r2, #16	; 0x10
    1e9c:	e5940008 	ldr	r0, [r4, #8]
    1ea0:	eb002c92 	bl	d0f0 <simple_strtoul>
    1ea4:	e3560004 	cmp	r6, #4	; 0x4
    1ea8:	e1a07000 	mov	r7, r0
    1eac:	13a00001 	movne	r0, #1	; 0x1
    1eb0:	1a000006 	bne	1ed0 <do_mem_mw+0x78>
    1eb4:	e594000c 	ldr	r0, [r4, #12]
    1eb8:	e3a01000 	mov	r1, #0	; 0x0
    1ebc:	e3a02010 	mov	r2, #16	; 0x10
    1ec0:	eb002c8a 	bl	d0f0 <simple_strtoul>
    1ec4:	ea000001 	b	1ed0 <do_mem_mw+0x78>
    1ec8:	e5057004 	str	r7, [r5, #-4]
    1ecc:	e2400001 	sub	r0, r0, #1	; 0x1
    1ed0:	e3500000 	cmp	r0, #0	; 0x0
    1ed4:	e2855004 	add	r5, r5, #4	; 0x4
    1ed8:	1afffffa 	bne	1ec8 <do_mem_mw+0x70>
    1edc:	e28dd004 	add	sp, sp, #4	; 0x4
    1ee0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00001ee4 <do_mem_md>:
    1ee4:	e92d4070 	push	{r4, r5, r6, lr}
    1ee8:	e3520001 	cmp	r2, #1	; 0x1
    1eec:	e24dd008 	sub	sp, sp, #8	; 0x8
    1ef0:	e1a04002 	mov	r4, r2
    1ef4:	e1a05003 	mov	r5, r3
    1ef8:	ca000002 	bgt	1f08 <do_mem_md+0x24>
    1efc:	eb00032e 	bl	2bbc <cmd_usage>
    1f00:	e3a00001 	mov	r0, #1	; 0x1
    1f04:	ea000014 	b	1f5c <do_mem_md+0x78>
    1f08:	e5930004 	ldr	r0, [r3, #4]
    1f0c:	e3a01000 	mov	r1, #0	; 0x0
    1f10:	e3a02010 	mov	r2, #16	; 0x10
    1f14:	eb002c75 	bl	d0f0 <simple_strtoul>
    1f18:	e3540002 	cmp	r4, #2	; 0x2
    1f1c:	e1a06000 	mov	r6, r0
    1f20:	03a030a0 	moveq	r3, #160	; 0xa0
    1f24:	0a000004 	beq	1f3c <do_mem_md+0x58>
    1f28:	e5950008 	ldr	r0, [r5, #8]
    1f2c:	e3a01000 	mov	r1, #0	; 0x0
    1f30:	e3a02010 	mov	r2, #16	; 0x10
    1f34:	eb002c6d 	bl	d0f0 <simple_strtoul>
    1f38:	e1a03000 	mov	r3, r0
    1f3c:	e3a0c004 	mov	ip, #4	; 0x4
    1f40:	e1a00006 	mov	r0, r6
    1f44:	e1a0200c 	mov	r2, ip
    1f48:	e1a03123 	lsr	r3, r3, #2
    1f4c:	e1a01006 	mov	r1, r6
    1f50:	e58dc000 	str	ip, [sp]
    1f54:	eb002bcd 	bl	ce90 <print_buffer>
    1f58:	e3a00000 	mov	r0, #0	; 0x0
    1f5c:	e28dd008 	add	sp, sp, #8	; 0x8
    1f60:	e8bd8070 	pop	{r4, r5, r6, pc}

00001f64 <do_sc>:
    1f64:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1f68:	e3520002 	cmp	r2, #2	; 0x2
    1f6c:	e24ddf47 	sub	sp, sp, #284	; 0x11c
    1f70:	0a000002 	beq	1f80 <do_sc+0x1c>
    1f74:	eb000310 	bl	2bbc <cmd_usage>
    1f78:	e3a00001 	mov	r0, #1	; 0x1
    1f7c:	ea000060 	b	2104 <do_sc+0x1a0>
    1f80:	e5930004 	ldr	r0, [r3, #4]
    1f84:	e3a01000 	mov	r1, #0	; 0x0
    1f88:	e3a02010 	mov	r2, #16	; 0x10
    1f8c:	eb002c57 	bl	d0f0 <simple_strtoul>
    1f90:	e28d4f41 	add	r4, sp, #260	; 0x104
    1f94:	e2844002 	add	r4, r4, #2	; 0x2
    1f98:	e59f116c 	ldr	r1, [pc, #364]	; 210c <do_sc+0x1a8>
    1f9c:	e1a06000 	mov	r6, r0
    1fa0:	e3a02012 	mov	r2, #18	; 0x12
    1fa4:	e1a00004 	mov	r0, r4
    1fa8:	ebfffce5 	bl	1344 <memcpy>
    1fac:	e3a01000 	mov	r1, #0	; 0x0
    1fb0:	ea000004 	b	1fc8 <do_sc+0x64>
    1fb4:	e7d12006 	ldrb	r2, [r1, r6]
    1fb8:	e7d43001 	ldrb	r3, [r4, r1]
    1fbc:	e2811001 	add	r1, r1, #1	; 0x1
    1fc0:	e1520003 	cmp	r2, r3
    1fc4:	1a000014 	bne	201c <do_sc+0xb8>
    1fc8:	e3510012 	cmp	r1, #18	; 0x12
    1fcc:	e0815006 	add	r5, r1, r6
    1fd0:	1afffff7 	bne	1fb4 <do_sc+0x50>
    1fd4:	e2851eff 	add	r1, r5, #4080	; 0xff0
    1fd8:	e281100f 	add	r1, r1, #15	; 0xf
    1fdc:	e1a02005 	mov	r2, r5
    1fe0:	e5d23000 	ldrb	r3, [r2]
    1fe4:	e353000a 	cmp	r3, #10	; 0xa
    1fe8:	1a000005 	bne	2004 <do_sc+0xa0>
    1fec:	e5d23001 	ldrb	r3, [r2, #1]
    1ff0:	e353005d 	cmp	r3, #93	; 0x5d
    1ff4:	1a000002 	bne	2004 <do_sc+0xa0>
    1ff8:	e5d23002 	ldrb	r3, [r2, #2]
    1ffc:	e353005d 	cmp	r3, #93	; 0x5d
    2000:	0a000003 	beq	2014 <do_sc+0xb0>
    2004:	e1520001 	cmp	r2, r1
    2008:	e2822001 	add	r2, r2, #1	; 0x1
    200c:	1afffff3 	bne	1fe0 <do_sc+0x7c>
    2010:	ea000001 	b	201c <do_sc+0xb8>
    2014:	e3550000 	cmp	r5, #0	; 0x0
    2018:	1a000006 	bne	2038 <do_sc+0xd4>
    201c:	e59f00ec 	ldr	r0, [pc, #236]	; 2110 <do_sc+0x1ac>
    2020:	e1a01006 	mov	r1, r6
    2024:	e59f30e8 	ldr	r3, [pc, #232]	; 2114 <do_sc+0x1b0>
    2028:	e1a0e00f 	mov	lr, pc
    202c:	e593f000 	ldr	pc, [r3]
    2030:	e3e00000 	mvn	r0, #0	; 0x0
    2034:	ea000032 	b	2104 <do_sc+0x1a0>
    2038:	e59f40d4 	ldr	r4, [pc, #212]	; 2114 <do_sc+0x1b0>
    203c:	e1a01006 	mov	r1, r6
    2040:	e59f00d0 	ldr	r0, [pc, #208]	; 2118 <do_sc+0x1b4>
    2044:	e28d6008 	add	r6, sp, #8	; 0x8
    2048:	e1a0e00f 	mov	lr, pc
    204c:	e594f000 	ldr	pc, [r4]
    2050:	e2466002 	sub	r6, r6, #2	; 0x2
    2054:	e1a07004 	mov	r7, r4
    2058:	ea000000 	b	2060 <do_sc+0xfc>
    205c:	e2855001 	add	r5, r5, #1	; 0x1
    2060:	e5d53000 	ldrb	r3, [r5]
    2064:	e353002d 	cmp	r3, #45	; 0x2d
    2068:	1353000a 	cmpne	r3, #10	; 0xa
    206c:	0afffffa 	beq	205c <do_sc+0xf8>
    2070:	e353005b 	cmp	r3, #91	; 0x5b
    2074:	0afffff8 	beq	205c <do_sc+0xf8>
    2078:	e353005d 	cmp	r3, #93	; 0x5d
    207c:	0a000001 	beq	2088 <do_sc+0x124>
    2080:	e1a02006 	mov	r2, r6
    2084:	ea000005 	b	20a0 <do_sc+0x13c>
    2088:	e5d53001 	ldrb	r3, [r5, #1]
    208c:	e353005d 	cmp	r3, #93	; 0x5d
    2090:	1afffffa 	bne	2080 <do_sc+0x11c>
    2094:	ea00000c 	b	20cc <do_sc+0x168>
    2098:	e5423001 	strb	r3, [r2, #-1]
    209c:	e2855001 	add	r5, r5, #1	; 0x1
    20a0:	e5d53000 	ldrb	r3, [r5]
    20a4:	e1a01002 	mov	r1, r2
    20a8:	e353000a 	cmp	r3, #10	; 0xa
    20ac:	1353005d 	cmpne	r3, #93	; 0x5d
    20b0:	03a04000 	moveq	r4, #0	; 0x0
    20b4:	13a04001 	movne	r4, #1	; 0x1
    20b8:	e2822001 	add	r2, r2, #1	; 0x1
    20bc:	1afffff5 	bne	2098 <do_sc+0x134>
    20c0:	e3550000 	cmp	r5, #0	; 0x0
    20c4:	e5c14000 	strb	r4, [r1]
    20c8:	1a000005 	bne	20e4 <do_sc+0x180>
    20cc:	e59f0048 	ldr	r0, [pc, #72]	; 211c <do_sc+0x1b8>
    20d0:	e59f303c 	ldr	r3, [pc, #60]	; 2114 <do_sc+0x1b0>
    20d4:	e1a0e00f 	mov	lr, pc
    20d8:	e593f000 	ldr	pc, [r3]
    20dc:	e3a00000 	mov	r0, #0	; 0x0
    20e0:	ea000007 	b	2104 <do_sc+0x1a0>
    20e4:	e1a01006 	mov	r1, r6
    20e8:	e59f0030 	ldr	r0, [pc, #48]	; 2120 <do_sc+0x1bc>
    20ec:	e1a0e00f 	mov	lr, pc
    20f0:	e597f000 	ldr	pc, [r7]
    20f4:	e1a00006 	mov	r0, r6
    20f8:	e1a01004 	mov	r1, r4
    20fc:	eb00304f 	bl	e240 <run_command>
    2100:	eaffffd6 	b	2060 <do_sc+0xfc>
    2104:	e28ddf47 	add	sp, sp, #284	; 0x11c
    2108:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    210c:	00011804 	.word	0x00011804
    2110:	00011f13 	.word	0x00011f13
    2114:	0800d76c 	.word	0x0800d76c
    2118:	00011f30 	.word	0x00011f30
    211c:	00011f49 	.word	0x00011f49
    2120:	00011f56 	.word	0x00011f56

00002124 <do_ls>:
    2124:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2128:	e59f001c 	ldr	r0, [pc, #28]	; 214c <do_ls+0x28>
    212c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2130:	e59f1018 	ldr	r1, [pc, #24]	; 2150 <do_ls+0x2c>
    2134:	e59f3018 	ldr	r3, [pc, #24]	; 2154 <do_ls+0x30>
    2138:	e1a0e00f 	mov	lr, pc
    213c:	e593f000 	ldr	pc, [r3]
    2140:	e3a00000 	mov	r0, #0	; 0x0
    2144:	e28dd004 	add	sp, sp, #4	; 0x4
    2148:	e8bd8000 	pop	{pc}
    214c:	00011d0c 	.word	0x00011d0c
    2150:	0800d10c 	.word	0x0800d10c
    2154:	0800d76c 	.word	0x0800d76c

00002158 <do_linkpc>:
    2158:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    215c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2160:	eb002964 	bl	c6f8 <linkpc_loop>
    2164:	e3a00000 	mov	r0, #0	; 0x0
    2168:	e28dd004 	add	sp, sp, #4	; 0x4
    216c:	e8bd8000 	pop	{pc}

00002170 <do_pads>:
    2170:	e3520002 	cmp	r2, #2	; 0x2
    2174:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    2178:	d59f0194 	ldrle	r0, [pc, #404]	; 2314 <do_pads+0x1a4>
    217c:	e1a05002 	mov	r5, r2
    2180:	e1a06003 	mov	r6, r3
    2184:	da000015 	ble	21e0 <do_pads+0x70>
    2188:	e5930008 	ldr	r0, [r3, #8]
    218c:	e3a01000 	mov	r1, #0	; 0x0
    2190:	e3a0200a 	mov	r2, #10	; 0xa
    2194:	eb002bd5 	bl	d0f0 <simple_strtoul>
    2198:	e3550003 	cmp	r5, #3	; 0x3
    219c:	e1a04000 	mov	r4, r0
    21a0:	01a07000 	moveq	r7, r0
    21a4:	0a000004 	beq	21bc <do_pads+0x4c>
    21a8:	e596000c 	ldr	r0, [r6, #12]
    21ac:	e3a01000 	mov	r1, #0	; 0x0
    21b0:	e3a0200a 	mov	r2, #10	; 0xa
    21b4:	eb002bcd 	bl	d0f0 <simple_strtoul>
    21b8:	e1a07000 	mov	r7, r0
    21bc:	e1943007 	orrs	r3, r4, r7
    21c0:	4a000005 	bmi	21dc <do_pads+0x6c>
    21c4:	e1a05004 	mov	r5, r4
    21c8:	e2866004 	add	r6, r6, #4	; 0x4
    21cc:	e59f9144 	ldr	r9, [pc, #324]	; 2318 <do_pads+0x1a8>
    21d0:	e59f8144 	ldr	r8, [pc, #324]	; 231c <do_pads+0x1ac>
    21d4:	e59fa144 	ldr	sl, [pc, #324]	; 2320 <do_pads+0x1b0>
    21d8:	ea000049 	b	2304 <do_pads+0x194>
    21dc:	e59f0140 	ldr	r0, [pc, #320]	; 2324 <do_pads+0x1b4>
    21e0:	e59f3134 	ldr	r3, [pc, #308]	; 231c <do_pads+0x1ac>
    21e4:	e1a0e00f 	mov	lr, pc
    21e8:	e593f000 	ldr	pc, [r3]
    21ec:	ea000046 	b	230c <do_pads+0x19c>
    21f0:	e5960000 	ldr	r0, [r6]
    21f4:	e59f111c 	ldr	r1, [pc, #284]	; 2318 <do_pads+0x1a8>
    21f8:	e3a02002 	mov	r2, #2	; 0x2
    21fc:	ebfffc17 	bl	1260 <strncmp>
    2200:	e3500000 	cmp	r0, #0	; 0x0
    2204:	1a00000b 	bne	2238 <do_pads+0xc8>
    2208:	e1a02000 	mov	r2, r0
    220c:	e3a01001 	mov	r1, #1	; 0x1
    2210:	e1a00005 	mov	r0, r5
    2214:	ebfffa58 	bl	b7c <pads_chmod>
    2218:	e1a00005 	mov	r0, r5
    221c:	e5984000 	ldr	r4, [r8]
    2220:	ebfffad1 	bl	d6c <pads_getpin>
    2224:	e1a01005 	mov	r1, r5
    2228:	e1a02000 	mov	r2, r0
    222c:	e59f00f4 	ldr	r0, [pc, #244]	; 2328 <do_pads+0x1b8>
    2230:	e12fff34 	blx	r4
    2234:	ea000031 	b	2300 <do_pads+0x190>
    2238:	e59f10ec 	ldr	r1, [pc, #236]	; 232c <do_pads+0x1bc>
    223c:	e3a02004 	mov	r2, #4	; 0x4
    2240:	e5960000 	ldr	r0, [r6]
    2244:	ebfffc05 	bl	1260 <strncmp>
    2248:	e3500000 	cmp	r0, #0	; 0x0
    224c:	01a00005 	moveq	r0, r5
    2250:	03a01002 	moveq	r1, #2	; 0x2
    2254:	03a02001 	moveq	r2, #1	; 0x1
    2258:	0a000011 	beq	22a4 <do_pads+0x134>
    225c:	e59f10cc 	ldr	r1, [pc, #204]	; 2330 <do_pads+0x1c0>
    2260:	e3a02003 	mov	r2, #3	; 0x3
    2264:	e5960000 	ldr	r0, [r6]
    2268:	ebfffbfc 	bl	1260 <strncmp>
    226c:	e3500000 	cmp	r0, #0	; 0x0
    2270:	01a02000 	moveq	r2, r0
    2274:	03a01002 	moveq	r1, #2	; 0x2
    2278:	01a00005 	moveq	r0, r5
    227c:	0a000008 	beq	22a4 <do_pads+0x134>
    2280:	e5960000 	ldr	r0, [r6]
    2284:	e59f10a8 	ldr	r1, [pc, #168]	; 2334 <do_pads+0x1c4>
    2288:	e3a02004 	mov	r2, #4	; 0x4
    228c:	ebfffbf3 	bl	1260 <strncmp>
    2290:	e3500000 	cmp	r0, #0	; 0x0
    2294:	1a000004 	bne	22ac <do_pads+0x13c>
    2298:	e1a01000 	mov	r1, r0
    229c:	e1a02001 	mov	r2, r1
    22a0:	e1a00005 	mov	r0, r5
    22a4:	ebfffa34 	bl	b7c <pads_chmod>
    22a8:	ea000014 	b	2300 <do_pads+0x190>
    22ac:	e5960000 	ldr	r0, [r6]
    22b0:	e59f1080 	ldr	r1, [pc, #128]	; 2338 <do_pads+0x1c8>
    22b4:	e3a02002 	mov	r2, #2	; 0x2
    22b8:	ebfffbe8 	bl	1260 <strncmp>
    22bc:	e3500000 	cmp	r0, #0	; 0x0
    22c0:	1a00000c 	bne	22f8 <do_pads+0x188>
    22c4:	e1a00005 	mov	r0, r5
    22c8:	ebfffa15 	bl	b24 <pads_states>
    22cc:	e3500000 	cmp	r0, #0	; 0x0
    22d0:	e5983000 	ldr	r3, [r8]
    22d4:	059f2060 	ldreq	r2, [pc, #96]	; 233c <do_pads+0x1cc>
    22d8:	0a000002 	beq	22e8 <do_pads+0x178>
    22dc:	e3500001 	cmp	r0, #1	; 0x1
    22e0:	01a02009 	moveq	r2, r9
    22e4:	11a0200a 	movne	r2, sl
    22e8:	e59f0050 	ldr	r0, [pc, #80]	; 2340 <do_pads+0x1d0>
    22ec:	e1a01005 	mov	r1, r5
    22f0:	e12fff33 	blx	r3
    22f4:	ea000001 	b	2300 <do_pads+0x190>
    22f8:	e59f0044 	ldr	r0, [pc, #68]	; 2344 <do_pads+0x1d4>
    22fc:	eaffffb7 	b	21e0 <do_pads+0x70>
    2300:	e2855001 	add	r5, r5, #1	; 0x1
    2304:	e1550007 	cmp	r5, r7
    2308:	daffffb8 	ble	21f0 <do_pads+0x80>
    230c:	e3a00000 	mov	r0, #0	; 0x0
    2310:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    2314:	00011f7a 	.word	0x00011f7a
    2318:	00011fb7 	.word	0x00011fb7
    231c:	0800d76c 	.word	0x0800d76c
    2320:	00014330 	.word	0x00014330
    2324:	00011f92 	.word	0x00011f92
    2328:	00011fba 	.word	0x00011fba
    232c:	00011fce 	.word	0x00011fce
    2330:	00011fd3 	.word	0x00011fd3
    2334:	00011fd7 	.word	0x00011fd7
    2338:	00011fdc 	.word	0x00011fdc
    233c:	00011fdf 	.word	0x00011fdf
    2340:	00011fe8 	.word	0x00011fe8
    2344:	00011ffd 	.word	0x00011ffd

00002348 <do_mux>:
    2348:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    234c:	e3520001 	cmp	r2, #1	; 0x1
    2350:	e24dd004 	sub	sp, sp, #4	; 0x4
    2354:	da000005 	ble	2370 <do_mux+0x28>
    2358:	e5930004 	ldr	r0, [r3, #4]
    235c:	e3a01000 	mov	r1, #0	; 0x0
    2360:	e3a0200a 	mov	r2, #10	; 0xa
    2364:	eb002b61 	bl	d0f0 <simple_strtoul>
    2368:	e6ff0070 	uxth	r0, r0
    236c:	ebfffb4d 	bl	10a8 <set_mux>
    2370:	e59f301c 	ldr	r3, [pc, #28]	; 2394 <do_mux+0x4c>
    2374:	e59f001c 	ldr	r0, [pc, #28]	; 2398 <do_mux+0x50>
    2378:	e5931000 	ldr	r1, [r3]
    237c:	e59f3018 	ldr	r3, [pc, #24]	; 239c <do_mux+0x54>
    2380:	e1a0e00f 	mov	lr, pc
    2384:	e593f000 	ldr	pc, [r3]
    2388:	e3a00000 	mov	r0, #0	; 0x0
    238c:	e28dd004 	add	sp, sp, #4	; 0x4
    2390:	e8bd8000 	pop	{pc}
    2394:	2d009000 	.word	0x2d009000
    2398:	00012016 	.word	0x00012016
    239c:	0800d76c 	.word	0x0800d76c

000023a0 <do_mac>:
    23a0:	e59f3020 	ldr	r3, [pc, #32]	; 23c8 <do_mac+0x28>
    23a4:	e92d4010 	push	{r4, lr}
    23a8:	e5934000 	ldr	r4, [r3]
    23ac:	ebfff96a 	bl	95c <efuse_mac>
    23b0:	e1a02000 	mov	r2, r0
    23b4:	e1a03001 	mov	r3, r1
    23b8:	e59f000c 	ldr	r0, [pc, #12]	; 23cc <do_mac+0x2c>
    23bc:	e12fff34 	blx	r4
    23c0:	e3a00000 	mov	r0, #0	; 0x0
    23c4:	e8bd8010 	pop	{r4, pc}
    23c8:	0800d76c 	.word	0x0800d76c
    23cc:	00012028 	.word	0x00012028

000023d0 <do_boot>:
    23d0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    23d4:	e3a00000 	mov	r0, #0	; 0x0
    23d8:	e24dd004 	sub	sp, sp, #4	; 0x4
    23dc:	ebfff801 	bl	3e8 <try_boot>
    23e0:	e3a00000 	mov	r0, #0	; 0x0
    23e4:	e28dd004 	add	sp, sp, #4	; 0x4
    23e8:	e8bd8000 	pop	{pc}

000023ec <do_efuse>:
    23ec:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    23f0:	e3520001 	cmp	r2, #1	; 0x1
    23f4:	e24dd004 	sub	sp, sp, #4	; 0x4
    23f8:	da000006 	ble	2418 <do_efuse+0x2c>
    23fc:	e5930004 	ldr	r0, [r3, #4]
    2400:	e59f1024 	ldr	r1, [pc, #36]	; 242c <do_efuse+0x40>
    2404:	e3a02002 	mov	r2, #2	; 0x2
    2408:	ebfffb94 	bl	1260 <strncmp>
    240c:	e3500000 	cmp	r0, #0	; 0x0
    2410:	02800001 	addeq	r0, r0, #1	; 0x1
    2414:	0a000000 	beq	241c <do_efuse+0x30>
    2418:	e3a00000 	mov	r0, #0	; 0x0
    241c:	ebfff951 	bl	968 <efuse_info>
    2420:	e3a00000 	mov	r0, #0	; 0x0
    2424:	e28dd004 	add	sp, sp, #4	; 0x4
    2428:	e8bd8000 	pop	{pc}
    242c:	00012040 	.word	0x00012040

00002430 <do_pi>:
    2430:	e92d4030 	push	{r4, r5, lr}
    2434:	e3520001 	cmp	r2, #1	; 0x1
    2438:	e24dd004 	sub	sp, sp, #4	; 0x4
    243c:	e1a04002 	mov	r4, r2
    2440:	e1a05003 	mov	r5, r3
    2444:	da000015 	ble	24a0 <do_pi+0x70>
    2448:	e59f10ac 	ldr	r1, [pc, #172]	; 24fc <do_pi+0xcc>
    244c:	e5930004 	ldr	r0, [r3, #4]
    2450:	e3a02004 	mov	r2, #4	; 0x4
    2454:	ebfffb81 	bl	1260 <strncmp>
    2458:	e2501000 	subs	r1, r0, #0	; 0x0
    245c:	1a00000d 	bne	2498 <do_pi+0x68>
    2460:	e3540002 	cmp	r4, #2	; 0x2
    2464:	01a04001 	moveq	r4, r1
    2468:	0a000003 	beq	247c <do_pi+0x4c>
    246c:	e5950008 	ldr	r0, [r5, #8]
    2470:	e3a02010 	mov	r2, #16	; 0x10
    2474:	eb002b1d 	bl	d0f0 <simple_strtoul>
    2478:	e1a04000 	mov	r4, r0
    247c:	e59f007c 	ldr	r0, [pc, #124]	; 2500 <do_pi+0xd0>
    2480:	e1a01004 	mov	r1, r4
    2484:	e59f3078 	ldr	r3, [pc, #120]	; 2504 <do_pi+0xd4>
    2488:	e1a0e00f 	mov	lr, pc
    248c:	e593f000 	ldr	pc, [r3]
    2490:	e1a00004 	mov	r0, r4
    2494:	eb001097 	bl	66f8 <burn>
    2498:	e3a04000 	mov	r4, #0	; 0x0
    249c:	ea000013 	b	24f0 <do_pi+0xc0>
    24a0:	e3a00441 	mov	r0, #1090519040	; 0x41000000
    24a4:	eb002ea5 	bl	df40 <ius_check_header>
    24a8:	e2504000 	subs	r4, r0, #0	; 0x0
    24ac:	159f0054 	ldrne	r0, [pc, #84]	; 2508 <do_pi+0xd8>
    24b0:	1a000005 	bne	24cc <do_pi+0x9c>
    24b4:	e3a0000d 	mov	r0, #13	; 0xd
    24b8:	e3a01001 	mov	r1, #1	; 0x1
    24bc:	eb000afb 	bl	50b0 <vs_assign_by_id>
    24c0:	e2504000 	subs	r4, r0, #0	; 0x0
    24c4:	0a000005 	beq	24e0 <do_pi+0xb0>
    24c8:	e59f003c 	ldr	r0, [pc, #60]	; 250c <do_pi+0xdc>
    24cc:	e1a01004 	mov	r1, r4
    24d0:	e59f302c 	ldr	r3, [pc, #44]	; 2504 <do_pi+0xd4>
    24d4:	e1a0e00f 	mov	lr, pc
    24d8:	e593f000 	ldr	pc, [r3]
    24dc:	ea000003 	b	24f0 <do_pi+0xc0>
    24e0:	e3a00441 	mov	r0, #1090519040	; 0x41000000
    24e4:	e3a0100d 	mov	r1, #13	; 0xd
    24e8:	e1a02004 	mov	r2, r4
    24ec:	eb001008 	bl	6514 <burn_images>
    24f0:	e1a00004 	mov	r0, r4
    24f4:	e28dd004 	add	sp, sp, #4	; 0x4
    24f8:	e8bd8030 	pop	{r4, r5, pc}
    24fc:	00012043 	.word	0x00012043
    2500:	00012048 	.word	0x00012048
    2504:	0800d76c 	.word	0x0800d76c
    2508:	00012072 	.word	0x00012072
    250c:	00012092 	.word	0x00012092

00002510 <do_spbyte>:
    2510:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2514:	e5930004 	ldr	r0, [r3, #4]
    2518:	e24dd004 	sub	sp, sp, #4	; 0x4
    251c:	e3a01000 	mov	r1, #0	; 0x0
    2520:	e3a0200a 	mov	r2, #10	; 0xa
    2524:	eb002af1 	bl	d0f0 <simple_strtoul>
    2528:	eb001b38 	bl	9210 <spi_set_bytes>
    252c:	e3a00000 	mov	r0, #0	; 0x0
    2530:	e28dd004 	add	sp, sp, #4	; 0x4
    2534:	e8bd8000 	pop	{pc}

00002538 <do_cfg>:
    2538:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    253c:	e5930004 	ldr	r0, [r3, #4]
    2540:	e24dd004 	sub	sp, sp, #4	; 0x4
    2544:	e3a01000 	mov	r1, #0	; 0x0
    2548:	e3a02010 	mov	r2, #16	; 0x10
    254c:	eb002ae7 	bl	d0f0 <simple_strtoul>
    2550:	eb000f2a 	bl	6200 <cdbg_config_isi>
    2554:	e3a00000 	mov	r0, #0	; 0x0
    2558:	e28dd004 	add	sp, sp, #4	; 0x4
    255c:	e8bd8000 	pop	{pc}

00002560 <do_sleep>:
    2560:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2564:	e3a01000 	mov	r1, #0	; 0x0
    2568:	e24dd004 	sub	sp, sp, #4	; 0x4
    256c:	e3a0200a 	mov	r2, #10	; 0xa
    2570:	e5930004 	ldr	r0, [r3, #4]
    2574:	eb002add 	bl	d0f0 <simple_strtoul>
    2578:	e59f3020 	ldr	r3, [pc, #32]	; 25a0 <do_sleep+0x40>
    257c:	e0000093 	mul	r0, r3, r0
    2580:	eb0018b2 	bl	8850 <udelay>
    2584:	e59f0018 	ldr	r0, [pc, #24]	; 25a4 <do_sleep+0x44>
    2588:	e59f3018 	ldr	r3, [pc, #24]	; 25a8 <do_sleep+0x48>
    258c:	e1a0e00f 	mov	lr, pc
    2590:	e593f000 	ldr	pc, [r3]
    2594:	e3a00000 	mov	r0, #0	; 0x0
    2598:	e28dd004 	add	sp, sp, #4	; 0x4
    259c:	e8bd8000 	pop	{pc}
    25a0:	000f4240 	.word	0x000f4240
    25a4:	000120ad 	.word	0x000120ad
    25a8:	0800d76c 	.word	0x0800d76c

000025ac <do_go>:
    25ac:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    25b0:	e5930004 	ldr	r0, [r3, #4]
    25b4:	e24dd004 	sub	sp, sp, #4	; 0x4
    25b8:	e3a02010 	mov	r2, #16	; 0x10
    25bc:	e3a01000 	mov	r1, #0	; 0x0
    25c0:	eb002aca 	bl	d0f0 <simple_strtoul>
    25c4:	e3a01000 	mov	r1, #0	; 0x0
    25c8:	eb000e97 	bl	602c <cdbg_jump>
    25cc:	e3a00000 	mov	r0, #0	; 0x0
    25d0:	e28dd004 	add	sp, sp, #4	; 0x4
    25d4:	e8bd8000 	pop	{pc}

000025d8 <do_ml>:
    25d8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    25dc:	e24dd004 	sub	sp, sp, #4	; 0x4
    25e0:	ebfffc4d 	bl	171c <__sOINFo>
    25e4:	e3a00000 	mov	r0, #0	; 0x0
    25e8:	e28dd004 	add	sp, sp, #4	; 0x4
    25ec:	e8bd8000 	pop	{pc}

000025f0 <do_bp>:
    25f0:	e92d4030 	push	{r4, r5, lr}
    25f4:	e3520001 	cmp	r2, #1	; 0x1
    25f8:	e24dd004 	sub	sp, sp, #4	; 0x4
    25fc:	da000005 	ble	2618 <do_bp+0x28>
    2600:	e5930004 	ldr	r0, [r3, #4]
    2604:	e3a01000 	mov	r1, #0	; 0x0
    2608:	e3a02010 	mov	r2, #16	; 0x10
    260c:	eb002ab7 	bl	d0f0 <simple_strtoul>
    2610:	e6ff0070 	uxth	r0, r0
    2614:	ebfff7d6 	bl	574 <set_xom>
    2618:	e59f5034 	ldr	r5, [pc, #52]	; 2654 <do_bp+0x64>
    261c:	e3a00001 	mov	r0, #1	; 0x1
    2620:	e5954000 	ldr	r4, [r5]
    2624:	ebfff7ce 	bl	564 <get_xom>
    2628:	e1a01000 	mov	r1, r0
    262c:	e59f0024 	ldr	r0, [pc, #36]	; 2658 <do_bp+0x68>
    2630:	e12fff34 	blx	r4
    2634:	e5954000 	ldr	r4, [r5]
    2638:	ebfff7d4 	bl	590 <boot_info>
    263c:	e1a01000 	mov	r1, r0
    2640:	e59f0014 	ldr	r0, [pc, #20]	; 265c <do_bp+0x6c>
    2644:	e12fff34 	blx	r4
    2648:	e3a00000 	mov	r0, #0	; 0x0
    264c:	e28dd004 	add	sp, sp, #4	; 0x4
    2650:	e8bd8030 	pop	{r4, r5, pc}
    2654:	0800d76c 	.word	0x0800d76c
    2658:	000120b8 	.word	0x000120b8
    265c:	000120ce 	.word	0x000120ce

00002660 <do_reset>:
    2660:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2664:	e24dd004 	sub	sp, sp, #4	; 0x4
    2668:	ebfff86a 	bl	818 <system_reset>
    266c:	e3a00000 	mov	r0, #0	; 0x0
    2670:	e28dd004 	add	sp, sp, #4	; 0x4
    2674:	e8bd8000 	pop	{pc}

00002678 <output_debug>:
    2678:	e24dd010 	sub	sp, sp, #16	; 0x10
    267c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2680:	e24dd004 	sub	sp, sp, #4	; 0x4
    2684:	e28dc008 	add	ip, sp, #8	; 0x8
    2688:	e3700002 	cmn	r0, #2	; 0x2
    268c:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    2690:	e1a01000 	mov	r1, r0
    2694:	0a000012 	beq	26e4 <output_debug+0x6c>
    2698:	e3700001 	cmn	r0, #1	; 0x1
    269c:	0a000008 	beq	26c4 <output_debug+0x4c>
    26a0:	e3700003 	cmn	r0, #3	; 0x3
    26a4:	1a000010 	bne	26ec <output_debug+0x74>
    26a8:	e59f30a4 	ldr	r3, [pc, #164]	; 2754 <output_debug+0xdc>
    26ac:	e59f00a4 	ldr	r0, [pc, #164]	; 2758 <output_debug+0xe0>
    26b0:	e5931000 	ldr	r1, [r3]
    26b4:	e59f30a0 	ldr	r3, [pc, #160]	; 275c <output_debug+0xe4>
    26b8:	e1a0e00f 	mov	lr, pc
    26bc:	e593f000 	ldr	pc, [r3]
    26c0:	ea00001e 	b	2740 <output_debug+0xc8>
    26c4:	e59f3088 	ldr	r3, [pc, #136]	; 2754 <output_debug+0xdc>
    26c8:	e59f0090 	ldr	r0, [pc, #144]	; 2760 <output_debug+0xe8>
    26cc:	e5932000 	ldr	r2, [r3]
    26d0:	e28d1010 	add	r1, sp, #16	; 0x10
    26d4:	e59f3080 	ldr	r3, [pc, #128]	; 275c <output_debug+0xe4>
    26d8:	e1a0e00f 	mov	lr, pc
    26dc:	e593f000 	ldr	pc, [r3]
    26e0:	ea000016 	b	2740 <output_debug+0xc8>
    26e4:	e59f0078 	ldr	r0, [pc, #120]	; 2764 <output_debug+0xec>
    26e8:	ea000003 	b	26fc <output_debug+0x84>
    26ec:	e59d300c 	ldr	r3, [sp, #12]
    26f0:	e3530001 	cmp	r3, #1	; 0x1
    26f4:	1a000004 	bne	270c <output_debug+0x94>
    26f8:	e59f0068 	ldr	r0, [pc, #104]	; 2768 <output_debug+0xf0>
    26fc:	e59f3058 	ldr	r3, [pc, #88]	; 275c <output_debug+0xe4>
    2700:	e1a0e00f 	mov	lr, pc
    2704:	e593f000 	ldr	pc, [r3]
    2708:	ea00000c 	b	2740 <output_debug+0xc8>
    270c:	e3530002 	cmp	r3, #2	; 0x2
    2710:	e59fc044 	ldr	ip, [pc, #68]	; 275c <output_debug+0xe4>
    2714:	1a000003 	bne	2728 <output_debug+0xb0>
    2718:	e59f004c 	ldr	r0, [pc, #76]	; 276c <output_debug+0xf4>
    271c:	e1a0e00f 	mov	lr, pc
    2720:	e59cf000 	ldr	pc, [ip]
    2724:	ea000005 	b	2740 <output_debug+0xc8>
    2728:	e59f3024 	ldr	r3, [pc, #36]	; 2754 <output_debug+0xdc>
    272c:	e59f003c 	ldr	r0, [pc, #60]	; 2770 <output_debug+0xf8>
    2730:	e5933000 	ldr	r3, [r3]
    2734:	e28d2010 	add	r2, sp, #16	; 0x10
    2738:	e1a0e00f 	mov	lr, pc
    273c:	e59cf000 	ldr	pc, [ip]
    2740:	e3a00000 	mov	r0, #0	; 0x0
    2744:	e28dd004 	add	sp, sp, #4	; 0x4
    2748:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    274c:	e28dd010 	add	sp, sp, #16	; 0x10
    2750:	e12fff1e 	bx	lr
    2754:	0800d1b4 	.word	0x0800d1b4
    2758:	00012256 	.word	0x00012256
    275c:	0800d76c 	.word	0x0800d76c
    2760:	0001227e 	.word	0x0001227e
    2764:	0001228d 	.word	0x0001228d
    2768:	000122a7 	.word	0x000122a7
    276c:	000122b2 	.word	0x000122b2
    2770:	000122c0 	.word	0x000122c0

00002774 <do_vs>:
    2774:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    2778:	e3520006 	cmp	r2, #6	; 0x6
    277c:	e24dd024 	sub	sp, sp, #36	; 0x24
    2780:	e1a06000 	mov	r6, r0
    2784:	e1a04003 	mov	r4, r3
    2788:	1a000005 	bne	27a4 <do_vs+0x30>
    278c:	e5930014 	ldr	r0, [r3, #20]
    2790:	e3a01000 	mov	r1, #0	; 0x0
    2794:	e282200a 	add	r2, r2, #10	; 0xa
    2798:	eb002a54 	bl	d0f0 <simple_strtoul>
    279c:	e1a05000 	mov	r5, r0
    27a0:	ea00008f 	b	29e4 <do_vs+0x270>
    27a4:	e2423001 	sub	r3, r2, #1	; 0x1
    27a8:	e3530005 	cmp	r3, #5	; 0x5
    27ac:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    27b0:	ea0000c4 	b	2ac8 <do_vs+0x354>
    27b4:	00002ac8 	.word	0x00002ac8
    27b8:	000027cc 	.word	0x000027cc
    27bc:	000028c4 	.word	0x000028c4
    27c0:	00002934 	.word	0x00002934
    27c4:	000029e0 	.word	0x000029e0
    27c8:	000029e0 	.word	0x000029e0
    27cc:	e5940004 	ldr	r0, [r4, #4]
    27d0:	e59f1310 	ldr	r1, [pc, #784]	; 2ae8 <do_vs+0x374>
    27d4:	ebfffa93 	bl	1228 <strcmp>
    27d8:	e2505000 	subs	r5, r0, #0	; 0x0
    27dc:	1a000025 	bne	2878 <do_vs+0x104>
    27e0:	e59f4304 	ldr	r4, [pc, #772]	; 2aec <do_vs+0x378>
    27e4:	e59f0304 	ldr	r0, [pc, #772]	; 2af0 <do_vs+0x37c>
    27e8:	e1a0e00f 	mov	lr, pc
    27ec:	e594f000 	ldr	pc, [r4]
    27f0:	e3a0802c 	mov	r8, #44	; 0x2c
    27f4:	e59f72f8 	ldr	r7, [pc, #760]	; 2af4 <do_vs+0x380>
    27f8:	ea000003 	b	280c <do_vs+0x98>
    27fc:	e0227598 	mla	r2, r8, r5, r7
    2800:	e1a0e00f 	mov	lr, pc
    2804:	e594f000 	ldr	pc, [r4]
    2808:	e2855001 	add	r5, r5, #1	; 0x1
    280c:	eb000951 	bl	4d58 <vs_device_count>
    2810:	e1550000 	cmp	r5, r0
    2814:	e1a01005 	mov	r1, r5
    2818:	e59f02d8 	ldr	r0, [pc, #728]	; 2af8 <do_vs+0x384>
    281c:	e59f62c8 	ldr	r6, [pc, #712]	; 2aec <do_vs+0x378>
    2820:	bafffff5 	blt	27fc <do_vs+0x88>
    2824:	e59f02d0 	ldr	r0, [pc, #720]	; 2afc <do_vs+0x388>
    2828:	e1a0e00f 	mov	lr, pc
    282c:	e596f000 	ldr	pc, [r6]
    2830:	eb00084e 	bl	4970 <vs_is_assigned>
    2834:	e2504000 	subs	r4, r0, #0	; 0x0
    2838:	0a000009 	beq	2864 <do_vs+0xf0>
    283c:	e59f32bc 	ldr	r3, [pc, #700]	; 2b00 <do_vs+0x38c>
    2840:	e5965000 	ldr	r5, [r6]
    2844:	e5934000 	ldr	r4, [r3]
    2848:	e1a00004 	mov	r0, r4
    284c:	eb0008c2 	bl	4b5c <vs_align>
    2850:	e1a01004 	mov	r1, r4
    2854:	e1a02000 	mov	r2, r0
    2858:	e59f02a4 	ldr	r0, [pc, #676]	; 2b04 <do_vs+0x390>
    285c:	e12fff35 	blx	r5
    2860:	ea00009d 	b	2adc <do_vs+0x368>
    2864:	e59f029c 	ldr	r0, [pc, #668]	; 2b08 <do_vs+0x394>
    2868:	e1a0e00f 	mov	lr, pc
    286c:	e596f000 	ldr	pc, [r6]
    2870:	e1a00004 	mov	r0, r4
    2874:	ea000099 	b	2ae0 <do_vs+0x36c>
    2878:	e5940004 	ldr	r0, [r4, #4]
    287c:	e59f1288 	ldr	r1, [pc, #648]	; 2b0c <do_vs+0x398>
    2880:	ebfffa68 	bl	1228 <strcmp>
    2884:	e2505000 	subs	r5, r0, #0	; 0x0
    2888:	1a000087 	bne	2aac <do_vs+0x338>
    288c:	eb0009f8 	bl	5074 <vs_reset>
    2890:	e28d4020 	add	r4, sp, #32	; 0x20
    2894:	e5240018 	str	r0, [r4, #-24]!
    2898:	e59f126c 	ldr	r1, [pc, #620]	; 2b0c <do_vs+0x398>
    289c:	e2840008 	add	r0, r4, #8	; 0x8
    28a0:	ebfffa36 	bl	1180 <strcpy>
    28a4:	e3a03001 	mov	r3, #1	; 0x1
    28a8:	e58d300c 	str	r3, [sp, #12]
    28ac:	e28d3018 	add	r3, sp, #24	; 0x18
    28b0:	e8930003 	ldm	r3, {r0, r1}
    28b4:	e88d0003 	stm	sp, {r0, r1}
    28b8:	e894000f 	ldm	r4, {r0, r1, r2, r3}
    28bc:	ebffff6d 	bl	2678 <output_debug>
    28c0:	ea000019 	b	292c <do_vs+0x1b8>
    28c4:	e5940004 	ldr	r0, [r4, #4]
    28c8:	e59f1240 	ldr	r1, [pc, #576]	; 2b10 <do_vs+0x39c>
    28cc:	ebfffa55 	bl	1228 <strcmp>
    28d0:	e2505000 	subs	r5, r0, #0	; 0x0
    28d4:	1a000074 	bne	2aac <do_vs+0x338>
    28d8:	e5940008 	ldr	r0, [r4, #8]
    28dc:	e3a01001 	mov	r1, #1	; 0x1
    28e0:	eb000a10 	bl	5128 <vs_assign_by_name>
    28e4:	e2507000 	subs	r7, r0, #0	; 0x0
    28e8:	e59f61fc 	ldr	r6, [pc, #508]	; 2aec <do_vs+0x378>
    28ec:	1a00000b 	bne	2920 <do_vs+0x1ac>
    28f0:	e5943008 	ldr	r3, [r4, #8]
    28f4:	e59f2218 	ldr	r2, [pc, #536]	; 2b14 <do_vs+0x3a0>
    28f8:	e1a00003 	mov	r0, r3
    28fc:	e5823000 	str	r3, [r2]
    2900:	eb000916 	bl	4d60 <vs_device_id>
    2904:	e59f31f4 	ldr	r3, [pc, #500]	; 2b00 <do_vs+0x38c>
    2908:	e5830000 	str	r0, [r3]
    290c:	e59f0204 	ldr	r0, [pc, #516]	; 2b18 <do_vs+0x3a4>
    2910:	e1a0e00f 	mov	lr, pc
    2914:	e596f000 	ldr	pc, [r6]
    2918:	e1a00007 	mov	r0, r7
    291c:	ea00006f 	b	2ae0 <do_vs+0x36c>
    2920:	e59f01f4 	ldr	r0, [pc, #500]	; 2b1c <do_vs+0x3a8>
    2924:	e1a0e00f 	mov	lr, pc
    2928:	e596f000 	ldr	pc, [r6]
    292c:	e1a00005 	mov	r0, r5
    2930:	ea00006a 	b	2ae0 <do_vs+0x36c>
    2934:	e3a02010 	mov	r2, #16	; 0x10
    2938:	e5940008 	ldr	r0, [r4, #8]
    293c:	e3a01000 	mov	r1, #0	; 0x0
    2940:	eb002a42 	bl	d250 <simple_strtoull>
    2944:	e3a02010 	mov	r2, #16	; 0x10
    2948:	e1a07000 	mov	r7, r0
    294c:	e1a08001 	mov	r8, r1
    2950:	e594000c 	ldr	r0, [r4, #12]
    2954:	e3a01000 	mov	r1, #0	; 0x0
    2958:	eb002a3c 	bl	d250 <simple_strtoull>
    295c:	e59f11bc 	ldr	r1, [pc, #444]	; 2b20 <do_vs+0x3ac>
    2960:	e1a05000 	mov	r5, r0
    2964:	e28d0010 	add	r0, sp, #16	; 0x10
    2968:	ebfffa04 	bl	1180 <strcpy>
    296c:	e3a03002 	mov	r3, #2	; 0x2
    2970:	e5940004 	ldr	r0, [r4, #4]
    2974:	e59f11a4 	ldr	r1, [pc, #420]	; 2b20 <do_vs+0x3ac>
    2978:	e58d300c 	str	r3, [sp, #12]
    297c:	ebfffa29 	bl	1228 <strcmp>
    2980:	e3500000 	cmp	r0, #0	; 0x0
    2984:	1a000004 	bne	299c <do_vs+0x228>
    2988:	e1a00007 	mov	r0, r7
    298c:	e1a01008 	mov	r1, r8
    2990:	e1a02005 	mov	r2, r5
    2994:	eb000834 	bl	4a6c <vs_erase>
    2998:	ea000008 	b	29c0 <do_vs+0x24c>
    299c:	e5940004 	ldr	r0, [r4, #4]
    29a0:	e59f117c 	ldr	r1, [pc, #380]	; 2b24 <do_vs+0x3b0>
    29a4:	ebfffa1f 	bl	1228 <strcmp>
    29a8:	e3500000 	cmp	r0, #0	; 0x0
    29ac:	1a00003e 	bne	2aac <do_vs+0x338>
    29b0:	e1a00007 	mov	r0, r7
    29b4:	e1a01008 	mov	r1, r8
    29b8:	e1a02005 	mov	r2, r5
    29bc:	eb000848 	bl	4ae4 <vs_scrub>
    29c0:	e58d0008 	str	r0, [sp, #8]
    29c4:	e28d2018 	add	r2, sp, #24	; 0x18
    29c8:	e8920003 	ldm	r2, {r0, r1}
    29cc:	e88d0003 	stm	sp, {r0, r1}
    29d0:	e28d3008 	add	r3, sp, #8	; 0x8
    29d4:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    29d8:	ebffff26 	bl	2678 <output_debug>
    29dc:	ea00003e 	b	2adc <do_vs+0x368>
    29e0:	e3a05000 	mov	r5, #0	; 0x0
    29e4:	e3a01000 	mov	r1, #0	; 0x0
    29e8:	e3a02010 	mov	r2, #16	; 0x10
    29ec:	e5940008 	ldr	r0, [r4, #8]
    29f0:	eb0029be 	bl	d0f0 <simple_strtoul>
    29f4:	e3a02010 	mov	r2, #16	; 0x10
    29f8:	e1a09000 	mov	r9, r0
    29fc:	e3a01000 	mov	r1, #0	; 0x0
    2a00:	e594000c 	ldr	r0, [r4, #12]
    2a04:	eb002a11 	bl	d250 <simple_strtoull>
    2a08:	e3a02010 	mov	r2, #16	; 0x10
    2a0c:	e1a08000 	mov	r8, r0
    2a10:	e1a0a001 	mov	sl, r1
    2a14:	e5940010 	ldr	r0, [r4, #16]
    2a18:	e3a01000 	mov	r1, #0	; 0x0
    2a1c:	e08db002 	add	fp, sp, r2
    2a20:	eb002a0a 	bl	d250 <simple_strtoull>
    2a24:	e3a03000 	mov	r3, #0	; 0x0
    2a28:	e59f10f8 	ldr	r1, [pc, #248]	; 2b28 <do_vs+0x3b4>
    2a2c:	e1a07000 	mov	r7, r0
    2a30:	e1a0000b 	mov	r0, fp
    2a34:	e58d300c 	str	r3, [sp, #12]
    2a38:	ebfff9d0 	bl	1180 <strcpy>
    2a3c:	e5940004 	ldr	r0, [r4, #4]
    2a40:	e59f10e4 	ldr	r1, [pc, #228]	; 2b2c <do_vs+0x3b8>
    2a44:	ebfff9f7 	bl	1228 <strcmp>
    2a48:	e3500000 	cmp	r0, #0	; 0x0
    2a4c:	1a000006 	bne	2a6c <do_vs+0x2f8>
    2a50:	e1a00009 	mov	r0, r9
    2a54:	e1a02008 	mov	r2, r8
    2a58:	e1a0300a 	mov	r3, sl
    2a5c:	e58d7000 	str	r7, [sp]
    2a60:	e58d5004 	str	r5, [sp, #4]
    2a64:	eb0007ea 	bl	4a14 <vs_write>
    2a68:	eaffffd4 	b	29c0 <do_vs+0x24c>
    2a6c:	e5940004 	ldr	r0, [r4, #4]
    2a70:	e59f10b8 	ldr	r1, [pc, #184]	; 2b30 <do_vs+0x3bc>
    2a74:	ebfff9eb 	bl	1228 <strcmp>
    2a78:	e3500000 	cmp	r0, #0	; 0x0
    2a7c:	1a00000a 	bne	2aac <do_vs+0x338>
    2a80:	e1a00009 	mov	r0, r9
    2a84:	e1a02008 	mov	r2, r8
    2a88:	e1a0300a 	mov	r3, sl
    2a8c:	e58d7000 	str	r7, [sp]
    2a90:	e58d5004 	str	r5, [sp, #4]
    2a94:	eb0008de 	bl	4e14 <vs_read>
    2a98:	e59f1094 	ldr	r1, [pc, #148]	; 2b34 <do_vs+0x3c0>
    2a9c:	e58d0008 	str	r0, [sp, #8]
    2aa0:	e1a0000b 	mov	r0, fp
    2aa4:	ebfff9b5 	bl	1180 <strcpy>
    2aa8:	eaffffc5 	b	29c4 <do_vs+0x250>
    2aac:	e59f0084 	ldr	r0, [pc, #132]	; 2b38 <do_vs+0x3c4>
    2ab0:	e5961014 	ldr	r1, [r6, #20]
    2ab4:	e59f3030 	ldr	r3, [pc, #48]	; 2aec <do_vs+0x378>
    2ab8:	e1a0e00f 	mov	lr, pc
    2abc:	e593f000 	ldr	pc, [r3]
    2ac0:	e3e00000 	mvn	r0, #0	; 0x0
    2ac4:	ea000005 	b	2ae0 <do_vs+0x36c>
    2ac8:	e5901014 	ldr	r1, [r0, #20]
    2acc:	e59f3018 	ldr	r3, [pc, #24]	; 2aec <do_vs+0x378>
    2ad0:	e59f0060 	ldr	r0, [pc, #96]	; 2b38 <do_vs+0x3c4>
    2ad4:	e1a0e00f 	mov	lr, pc
    2ad8:	e593f000 	ldr	pc, [r3]
    2adc:	e3a00000 	mov	r0, #0	; 0x0
    2ae0:	e28dd024 	add	sp, sp, #36	; 0x24
    2ae4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    2ae8:	000120fd 	.word	0x000120fd
    2aec:	0800d76c 	.word	0x0800d76c
    2af0:	000122d7 	.word	0x000122d7
    2af4:	00010ee4 	.word	0x00010ee4
    2af8:	000122e5 	.word	0x000122e5
    2afc:	000122ed 	.word	0x000122ed
    2b00:	0800d1b4 	.word	0x0800d1b4
    2b04:	000122ff 	.word	0x000122ff
    2b08:	00012309 	.word	0x00012309
    2b0c:	000120eb 	.word	0x000120eb
    2b10:	00014104 	.word	0x00014104
    2b14:	0800d1b8 	.word	0x0800d1b8
    2b18:	0001231c 	.word	0x0001231c
    2b1c:	00012328 	.word	0x00012328
    2b20:	00012355 	.word	0x00012355
    2b24:	0001235b 	.word	0x0001235b
    2b28:	00012361 	.word	0x00012361
    2b2c:	0001236a 	.word	0x0001236a
    2b30:	00012370 	.word	0x00012370
    2b34:	00012375 	.word	0x00012375
    2b38:	00011d0b 	.word	0x00011d0b

00002b3c <cmd_get_data_size>:
    2b3c:	e92d4030 	push	{r4, r5, lr}
    2b40:	e24dd004 	sub	sp, sp, #4	; 0x4
    2b44:	e1a04001 	mov	r4, r1
    2b48:	e1a05000 	mov	r5, r0
    2b4c:	ebfff9e2 	bl	12dc <strlen>
    2b50:	e3500002 	cmp	r0, #2	; 0x2
    2b54:	da000015 	ble	2bb0 <cmd_get_data_size+0x74>
    2b58:	e0850000 	add	r0, r5, r0
    2b5c:	e5503002 	ldrb	r3, [r0, #-2]
    2b60:	e353002e 	cmp	r3, #46	; 0x2e
    2b64:	1a000011 	bne	2bb0 <cmd_get_data_size+0x74>
    2b68:	e5500001 	ldrb	r0, [r0, #-1]
    2b6c:	e350006c 	cmp	r0, #108	; 0x6c
    2b70:	03a04004 	moveq	r4, #4	; 0x4
    2b74:	0a00000d 	beq	2bb0 <cmd_get_data_size+0x74>
    2b78:	8a000002 	bhi	2b88 <cmd_get_data_size+0x4c>
    2b7c:	e3500062 	cmp	r0, #98	; 0x62
    2b80:	1a000009 	bne	2bac <cmd_get_data_size+0x70>
    2b84:	ea000006 	b	2ba4 <cmd_get_data_size+0x68>
    2b88:	e3500073 	cmp	r0, #115	; 0x73
    2b8c:	03e04001 	mvneq	r4, #1	; 0x1
    2b90:	0a000006 	beq	2bb0 <cmd_get_data_size+0x74>
    2b94:	e3500077 	cmp	r0, #119	; 0x77
    2b98:	03a04002 	moveq	r4, #2	; 0x2
    2b9c:	1a000002 	bne	2bac <cmd_get_data_size+0x70>
    2ba0:	ea000002 	b	2bb0 <cmd_get_data_size+0x74>
    2ba4:	e3a04001 	mov	r4, #1	; 0x1
    2ba8:	ea000000 	b	2bb0 <cmd_get_data_size+0x74>
    2bac:	e3e04000 	mvn	r4, #0	; 0x0
    2bb0:	e1a00004 	mov	r0, r4
    2bb4:	e28dd004 	add	sp, sp, #4	; 0x4
    2bb8:	e8bd8030 	pop	{r4, r5, pc}

00002bbc <cmd_usage>:
    2bbc:	e92d4030 	push	{r4, r5, lr}
    2bc0:	e1a04000 	mov	r4, r0
    2bc4:	e24dd004 	sub	sp, sp, #4	; 0x4
    2bc8:	e59f5054 	ldr	r5, [pc, #84]	; 2c24 <cmd_usage+0x68>
    2bcc:	e59f0054 	ldr	r0, [pc, #84]	; 2c28 <cmd_usage+0x6c>
    2bd0:	e5941000 	ldr	r1, [r4]
    2bd4:	e5942010 	ldr	r2, [r4, #16]
    2bd8:	e1a0e00f 	mov	lr, pc
    2bdc:	e595f000 	ldr	pc, [r5]
    2be0:	e59f0044 	ldr	r0, [pc, #68]	; 2c2c <cmd_usage+0x70>
    2be4:	e5941000 	ldr	r1, [r4]
    2be8:	e1a0e00f 	mov	lr, pc
    2bec:	e595f000 	ldr	pc, [r5]
    2bf0:	e5940014 	ldr	r0, [r4, #20]
    2bf4:	e3500000 	cmp	r0, #0	; 0x0
    2bf8:	1a000003 	bne	2c0c <cmd_usage+0x50>
    2bfc:	e59f002c 	ldr	r0, [pc, #44]	; 2c30 <cmd_usage+0x74>
    2c00:	eb002d28 	bl	e0a8 <puts>
    2c04:	e3a00001 	mov	r0, #1	; 0x1
    2c08:	ea000003 	b	2c1c <cmd_usage+0x60>
    2c0c:	eb002d25 	bl	e0a8 <puts>
    2c10:	e3a0000a 	mov	r0, #10	; 0xa
    2c14:	eb002d4b 	bl	e148 <putc>
    2c18:	e3a00000 	mov	r0, #0	; 0x0
    2c1c:	e28dd004 	add	sp, sp, #4	; 0x4
    2c20:	e8bd8030 	pop	{r4, r5, pc}
    2c24:	0800d76c 	.word	0x0800d76c
    2c28:	0001246a 	.word	0x0001246a
    2c2c:	00012474 	.word	0x00012474
    2c30:	0001247f 	.word	0x0001247f

00002c34 <find_cmd_tbl>:
    2c34:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    2c38:	e1a05001 	mov	r5, r1
    2c3c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2c40:	e3a0102e 	mov	r1, #46	; 0x2e
    2c44:	e1a07000 	mov	r7, r0
    2c48:	e1a08002 	mov	r8, r2
    2c4c:	ebfff997 	bl	12b0 <strchr>
    2c50:	e3500000 	cmp	r0, #0	; 0x0
    2c54:	10676000 	rsbne	r6, r7, r0
    2c58:	1a000002 	bne	2c68 <find_cmd_tbl+0x34>
    2c5c:	e1a00007 	mov	r0, r7
    2c60:	ebfff99d 	bl	12dc <strlen>
    2c64:	e1a06000 	mov	r6, r0
    2c68:	e3a03018 	mov	r3, #24	; 0x18
    2c6c:	e02a5893 	mla	sl, r3, r8, r5
    2c70:	e1a04005 	mov	r4, r5
    2c74:	e1a08005 	mov	r8, r5
    2c78:	e3a05000 	mov	r5, #0	; 0x0
    2c7c:	ea00000a 	b	2cac <find_cmd_tbl+0x78>
    2c80:	e5941000 	ldr	r1, [r4]
    2c84:	ebfff975 	bl	1260 <strncmp>
    2c88:	e3500000 	cmp	r0, #0	; 0x0
    2c8c:	1a000005 	bne	2ca8 <find_cmd_tbl+0x74>
    2c90:	e5940000 	ldr	r0, [r4]
    2c94:	ebfff990 	bl	12dc <strlen>
    2c98:	e1560000 	cmp	r6, r0
    2c9c:	0a000009 	beq	2cc8 <find_cmd_tbl+0x94>
    2ca0:	e2855001 	add	r5, r5, #1	; 0x1
    2ca4:	e1a08004 	mov	r8, r4
    2ca8:	e2844018 	add	r4, r4, #24	; 0x18
    2cac:	e154000a 	cmp	r4, sl
    2cb0:	e1a02006 	mov	r2, r6
    2cb4:	e1a00007 	mov	r0, r7
    2cb8:	1afffff0 	bne	2c80 <find_cmd_tbl+0x4c>
    2cbc:	e3550001 	cmp	r5, #1	; 0x1
    2cc0:	01a04008 	moveq	r4, r8
    2cc4:	13a04000 	movne	r4, #0	; 0x0
    2cc8:	e1a00004 	mov	r0, r4
    2ccc:	e28dd004 	add	sp, sp, #4	; 0x4
    2cd0:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00002cd4 <find_cmd>:
    2cd4:	e59f1014 	ldr	r1, [pc, #20]	; 2cf0 <find_cmd+0x1c>
    2cd8:	e59f2014 	ldr	r2, [pc, #20]	; 2cf4 <find_cmd+0x20>
    2cdc:	e59f3014 	ldr	r3, [pc, #20]	; 2cf8 <find_cmd+0x24>
    2ce0:	e0612002 	rsb	r2, r1, r2
    2ce4:	e1a021c2 	asr	r2, r2, #3
    2ce8:	e0020293 	mul	r2, r3, r2
    2cec:	eaffffd0 	b	2c34 <find_cmd_tbl>
    2cf0:	00014474 	.word	0x00014474
    2cf4:	00014744 	.word	0x00014744
    2cf8:	aaaaaaab 	.word	0xaaaaaaab

00002cfc <_do_help>:
    2cfc:	e1a0c00d 	mov	ip, sp
    2d00:	e92ddff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}
    2d04:	e24cb004 	sub	fp, ip, #4	; 0x4
    2d08:	e24dd004 	sub	sp, sp, #4	; 0x4
    2d0c:	e59b8004 	ldr	r8, [fp, #4]
    2d10:	e1a07000 	mov	r7, r0
    2d14:	e3580001 	cmp	r8, #1	; 0x1
    2d18:	e1a0a001 	mov	sl, r1
    2d1c:	159b5008 	ldrne	r5, [fp, #8]
    2d20:	13a06001 	movne	r6, #1	; 0x1
    2d24:	13a04000 	movne	r4, #0	; 0x0
    2d28:	159f9158 	ldrne	r9, [pc, #344]	; 2e88 <_do_help+0x18c>
    2d2c:	1a00004e 	bne	2e6c <_do_help+0x170>
    2d30:	e1a03101 	lsl	r3, r1, #2
    2d34:	e283300e 	add	r3, r3, #14	; 0xe
    2d38:	e3c33007 	bic	r3, r3, #7	; 0x7
    2d3c:	e1a0900d 	mov	r9, sp
    2d40:	e063d00d 	rsb	sp, r3, sp
    2d44:	e1a0800d 	mov	r8, sp
    2d48:	e3a03000 	mov	r3, #0	; 0x0
    2d4c:	ea000002 	b	2d5c <_do_help+0x60>
    2d50:	e7880103 	str	r0, [r8, r3, lsl #2]
    2d54:	e2800018 	add	r0, r0, #24	; 0x18
    2d58:	e2833001 	add	r3, r3, #1	; 0x1
    2d5c:	e153000a 	cmp	r3, sl
    2d60:	bafffffa 	blt	2d50 <_do_help+0x54>
    2d64:	e24a7001 	sub	r7, sl, #1	; 0x1
    2d68:	ea000015 	b	2dc4 <_do_help+0xc8>
    2d6c:	e5943000 	ldr	r3, [r4]
    2d70:	e5942004 	ldr	r2, [r4, #4]
    2d74:	e5930000 	ldr	r0, [r3]
    2d78:	e5921000 	ldr	r1, [r2]
    2d7c:	ebfff929 	bl	1228 <strcmp>
    2d80:	e3500000 	cmp	r0, #0	; 0x0
    2d84:	c5942000 	ldrgt	r2, [r4]
    2d88:	c5943004 	ldrgt	r3, [r4, #4]
    2d8c:	e2855001 	add	r5, r5, #1	; 0x1
    2d90:	c5843000 	strgt	r3, [r4]
    2d94:	c5842004 	strgt	r2, [r4, #4]
    2d98:	c2866001 	addgt	r6, r6, #1	; 0x1
    2d9c:	e1550007 	cmp	r5, r7
    2da0:	e2844004 	add	r4, r4, #4	; 0x4
    2da4:	bafffff0 	blt	2d6c <_do_help+0x70>
    2da8:	e3560000 	cmp	r6, #0	; 0x0
    2dac:	1a000003 	bne	2dc0 <_do_help+0xc4>
    2db0:	e3a05000 	mov	r5, #0	; 0x0
    2db4:	e1a04005 	mov	r4, r5
    2db8:	e59f70c8 	ldr	r7, [pc, #200]	; 2e88 <_do_help+0x18c>
    2dbc:	ea000016 	b	2e1c <_do_help+0x120>
    2dc0:	e2477001 	sub	r7, r7, #1	; 0x1
    2dc4:	e3570000 	cmp	r7, #0	; 0x0
    2dc8:	dafffff8 	ble	2db0 <_do_help+0xb4>
    2dcc:	e3a05000 	mov	r5, #0	; 0x0
    2dd0:	e1a04008 	mov	r4, r8
    2dd4:	e1a06005 	mov	r6, r5
    2dd8:	eaffffe3 	b	2d6c <_do_help+0x70>
    2ddc:	e7983004 	ldr	r3, [r8, r4]
    2de0:	e5936010 	ldr	r6, [r3, #16]
    2de4:	eb002cda 	bl	e154 <ctrlc>
    2de8:	e3500000 	cmp	r0, #0	; 0x0
    2dec:	e1a03006 	mov	r3, r6
    2df0:	e3a01008 	mov	r1, #8	; 0x8
    2df4:	e59f0090 	ldr	r0, [pc, #144]	; 2e8c <_do_help+0x190>
    2df8:	13a04001 	movne	r4, #1	; 0x1
    2dfc:	1a00000a 	bne	2e2c <_do_help+0x130>
    2e00:	e3560000 	cmp	r6, #0	; 0x0
    2e04:	0a000003 	beq	2e18 <_do_help+0x11c>
    2e08:	e7982004 	ldr	r2, [r8, r4]
    2e0c:	e5922000 	ldr	r2, [r2]
    2e10:	e1a0e00f 	mov	lr, pc
    2e14:	e597f000 	ldr	pc, [r7]
    2e18:	e2844004 	add	r4, r4, #4	; 0x4
    2e1c:	e155000a 	cmp	r5, sl
    2e20:	e2855001 	add	r5, r5, #1	; 0x1
    2e24:	baffffec 	blt	2ddc <_do_help+0xe0>
    2e28:	e3a04000 	mov	r4, #0	; 0x0
    2e2c:	e1a0d009 	mov	sp, r9
    2e30:	ea000011 	b	2e7c <_do_help+0x180>
    2e34:	e5950004 	ldr	r0, [r5, #4]
    2e38:	ebffff7d 	bl	2c34 <find_cmd_tbl>
    2e3c:	e3500000 	cmp	r0, #0	; 0x0
    2e40:	0a000002 	beq	2e50 <_do_help+0x154>
    2e44:	ebffff5c 	bl	2bbc <cmd_usage>
    2e48:	e1844000 	orr	r4, r4, r0
    2e4c:	ea000004 	b	2e64 <_do_help+0x168>
    2e50:	e5951004 	ldr	r1, [r5, #4]
    2e54:	e59f0034 	ldr	r0, [pc, #52]	; 2e90 <_do_help+0x194>
    2e58:	e1a0e00f 	mov	lr, pc
    2e5c:	e599f000 	ldr	pc, [r9]
    2e60:	e3a04001 	mov	r4, #1	; 0x1
    2e64:	e2866001 	add	r6, r6, #1	; 0x1
    2e68:	e2855004 	add	r5, r5, #4	; 0x4
    2e6c:	e1560008 	cmp	r6, r8
    2e70:	e1a01007 	mov	r1, r7
    2e74:	e1a0200a 	mov	r2, sl
    2e78:	baffffed 	blt	2e34 <_do_help+0x138>
    2e7c:	e1a00004 	mov	r0, r4
    2e80:	e24bd028 	sub	sp, fp, #40	; 0x28
    2e84:	e89daff0 	ldm	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
    2e88:	0800d76c 	.word	0x0800d76c
    2e8c:	000124a0 	.word	0x000124a0
    2e90:	000124aa 	.word	0x000124aa

00002e94 <do_help>:
    2e94:	e92d4030 	push	{r4, r5, lr}
    2e98:	e59fc038 	ldr	ip, [pc, #56]	; 2ed8 <do_help+0x44>
    2e9c:	e59fe038 	ldr	lr, [pc, #56]	; 2edc <do_help+0x48>
    2ea0:	e1a05001 	mov	r5, r1
    2ea4:	e06ec00c 	rsb	ip, lr, ip
    2ea8:	e59f1030 	ldr	r1, [pc, #48]	; 2ee0 <do_help+0x4c>
    2eac:	e24dd00c 	sub	sp, sp, #12	; 0xc
    2eb0:	e1a04000 	mov	r4, r0
    2eb4:	e1a0c1cc 	asr	ip, ip, #3
    2eb8:	e88d000c 	stm	sp, {r2, r3}
    2ebc:	e0010c91 	mul	r1, r1, ip
    2ec0:	e1a0000e 	mov	r0, lr
    2ec4:	e1a02004 	mov	r2, r4
    2ec8:	e1a03005 	mov	r3, r5
    2ecc:	ebffff8a 	bl	2cfc <_do_help>
    2ed0:	e28dd00c 	add	sp, sp, #12	; 0xc
    2ed4:	e8bd8030 	pop	{r4, r5, pc}
    2ed8:	00014744 	.word	0x00014744
    2edc:	00014474 	.word	0x00014474
    2ee0:	aaaaaaab 	.word	0xaaaaaaab

00002ee4 <tt_blk_trolling>:
    2ee4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    2ee8:	e3a01001 	mov	r1, #1	; 0x1
    2eec:	e24dd02c 	sub	sp, sp, #44	; 0x2c
    2ef0:	e1a04002 	mov	r4, r2
    2ef4:	e1a05003 	mov	r5, r3
    2ef8:	e58d0010 	str	r0, [sp, #16]
    2efc:	e59da050 	ldr	sl, [sp, #80]
    2f00:	eb000888 	bl	5128 <vs_assign_by_name>
    2f04:	e3500000 	cmp	r0, #0	; 0x0
    2f08:	0a000005 	beq	2f24 <tt_blk_trolling+0x40>
    2f0c:	e59d1010 	ldr	r1, [sp, #16]
    2f10:	e59f03d4 	ldr	r0, [pc, #980]	; 32ec <tt_blk_trolling+0x408>
    2f14:	e59f33d4 	ldr	r3, [pc, #980]	; 32f0 <tt_blk_trolling+0x40c>
    2f18:	e1a0e00f 	mov	lr, pc
    2f1c:	e593f000 	ldr	pc, [r3]
    2f20:	ea000026 	b	2fc0 <tt_blk_trolling+0xdc>
    2f24:	e1940005 	orrs	r0, r4, r5
    2f28:	03a04502 	moveq	r4, #8388608	; 0x800000
    2f2c:	03a06502 	moveq	r6, #8388608	; 0x800000
    2f30:	03a07000 	moveq	r7, #0	; 0x0
    2f34:	0a00000b 	beq	2f68 <tt_blk_trolling+0x84>
    2f38:	e3550000 	cmp	r5, #0	; 0x0
    2f3c:	1a000006 	bne	2f5c <tt_blk_trolling+0x78>
    2f40:	e3540502 	cmp	r4, #8388608	; 0x800000
    2f44:	2a000004 	bcs	2f5c <tt_blk_trolling+0x78>
    2f48:	e3a0000e 	mov	r0, #14	; 0xe
    2f4c:	eb000702 	bl	4b5c <vs_align>
    2f50:	e1a06004 	mov	r6, r4
    2f54:	e1a07005 	mov	r7, r5
    2f58:	ea000002 	b	2f68 <tt_blk_trolling+0x84>
    2f5c:	e1a06004 	mov	r6, r4
    2f60:	e1a07005 	mov	r7, r5
    2f64:	e3a04502 	mov	r4, #8388608	; 0x800000
    2f68:	e3a0000e 	mov	r0, #14	; 0xe
    2f6c:	eb0006fa 	bl	4b5c <vs_align>
    2f70:	e1a01000 	mov	r1, r0
    2f74:	e1a00004 	mov	r0, r4
    2f78:	eb003460 	bl	10100 <__aeabi_uidiv>
    2f7c:	e1a01000 	mov	r1, r0
    2f80:	e3a00000 	mov	r0, #0	; 0x0
    2f84:	eb000514 	bl	43dc <tt_rnd_get_int>
    2f88:	e1a04000 	mov	r4, r0
    2f8c:	e3a0000e 	mov	r0, #14	; 0xe
    2f90:	eb0006f1 	bl	4b5c <vs_align>
    2f94:	e0040094 	mul	r4, r4, r0
    2f98:	e28d0024 	add	r0, sp, #36	; 0x24
    2f9c:	e1a01004 	mov	r1, r4
    2fa0:	e1a02004 	mov	r2, r4
    2fa4:	eb000535 	bl	4480 <tt_rnd_get_data>
    2fa8:	e250b000 	subs	fp, r0, #0	; 0x0
    2fac:	1a000005 	bne	2fc8 <tt_blk_trolling+0xe4>
    2fb0:	e59f033c 	ldr	r0, [pc, #828]	; 32f4 <tt_blk_trolling+0x410>
    2fb4:	e59f3334 	ldr	r3, [pc, #820]	; 32f0 <tt_blk_trolling+0x40c>
    2fb8:	e1a0e00f 	mov	lr, pc
    2fbc:	e593f000 	ldr	pc, [r3]
    2fc0:	e3e00000 	mvn	r0, #0	; 0x0
    2fc4:	ea0000c6 	b	32e4 <tt_blk_trolling+0x400>
    2fc8:	e59d3024 	ldr	r3, [sp, #36]
    2fcc:	e3a0000e 	mov	r0, #14	; 0xe
    2fd0:	e0564003 	subs	r4, r6, r3
    2fd4:	e0c75fc3 	sbc	r5, r7, r3, asr #31
    2fd8:	eb0006df 	bl	4b5c <vs_align>
    2fdc:	e3550000 	cmp	r5, #0	; 0x0
    2fe0:	e1cd41f8 	strd	r4, [sp, #24]
    2fe4:	1a000005 	bne	3000 <tt_blk_trolling+0x11c>
    2fe8:	e1a01000 	mov	r1, r0
    2fec:	e1a00004 	mov	r0, r4
    2ff0:	eb003442 	bl	10100 <__aeabi_uidiv>
    2ff4:	e58d501c 	str	r5, [sp, #28]
    2ff8:	e58d0018 	str	r0, [sp, #24]
    2ffc:	ea000002 	b	300c <tt_blk_trolling+0x128>
    3000:	e1a01000 	mov	r1, r0
    3004:	e28d0018 	add	r0, sp, #24	; 0x18
    3008:	eb0027fb 	bl	cffc <__div64_32>
    300c:	e59d1018 	ldr	r1, [sp, #24]
    3010:	e3a00000 	mov	r0, #0	; 0x0
    3014:	eb0004f0 	bl	43dc <tt_rnd_get_int>
    3018:	e1a04000 	mov	r4, r0
    301c:	e3a0000e 	mov	r0, #14	; 0xe
    3020:	eb0006cd 	bl	4b5c <vs_align>
    3024:	e0c98490 	smull	r8, r9, r0, r4
    3028:	e59d1010 	ldr	r1, [sp, #16]
    302c:	e1a0200b 	mov	r2, fp
    3030:	e59d3024 	ldr	r3, [sp, #36]
    3034:	e59f02bc 	ldr	r0, [pc, #700]	; 32f8 <tt_blk_trolling+0x414>
    3038:	e88d0300 	stm	sp, {r8, r9}
    303c:	e59f42ac 	ldr	r4, [pc, #684]	; 32f0 <tt_blk_trolling+0x40c>
    3040:	e1a0e00f 	mov	lr, pc
    3044:	e594f000 	ldr	pc, [r4]
    3048:	e59d0010 	ldr	r0, [sp, #16]
    304c:	eb000743 	bl	4d60 <vs_device_id>
    3050:	eb00072e 	bl	4d10 <vs_device_erasable>
    3054:	e3500000 	cmp	r0, #0	; 0x0
    3058:	0a000006 	beq	3078 <tt_blk_trolling+0x194>
    305c:	e59f0298 	ldr	r0, [pc, #664]	; 32fc <tt_blk_trolling+0x418>
    3060:	e1a0e00f 	mov	lr, pc
    3064:	e594f000 	ldr	pc, [r4]
    3068:	e1a00008 	mov	r0, r8
    306c:	e1a01009 	mov	r1, r9
    3070:	e59d2024 	ldr	r2, [sp, #36]
    3074:	eb00067c 	bl	4a6c <vs_erase>
    3078:	e59f0280 	ldr	r0, [pc, #640]	; 3300 <tt_blk_trolling+0x41c>
    307c:	e59f326c 	ldr	r3, [pc, #620]	; 32f0 <tt_blk_trolling+0x40c>
    3080:	e1a0e00f 	mov	lr, pc
    3084:	e593f000 	ldr	pc, [r3]
    3088:	e35a0000 	cmp	sl, #0	; 0x0
    308c:	13a07000 	movne	r7, #0	; 0x0
    3090:	159d6024 	ldrne	r6, [sp, #36]
    3094:	1a00001c 	bne	310c <tt_blk_trolling+0x228>
    3098:	ea00001f 	b	311c <tt_blk_trolling+0x238>
    309c:	e3560902 	cmp	r6, #32768	; 0x8000
    30a0:	b1a01006 	movlt	r1, r6
    30a4:	a3a01902 	movge	r1, #32768	; 0x8000
    30a8:	e58d100c 	str	r1, [sp, #12]
    30ac:	e59d1024 	ldr	r1, [sp, #36]
    30b0:	e59d200c 	ldr	r2, [sp, #12]
    30b4:	e0661001 	rsb	r1, r6, r1
    30b8:	e08b1001 	add	r1, fp, r1
    30bc:	ebfff8a0 	bl	1344 <memcpy>
    30c0:	e3a00000 	mov	r0, #0	; 0x0
    30c4:	eb001605 	bl	88e0 <get_timer>
    30c8:	e59d3024 	ldr	r3, [sp, #36]
    30cc:	e3a01000 	mov	r1, #0	; 0x0
    30d0:	e0663003 	rsb	r3, r6, r3
    30d4:	e0984003 	adds	r4, r8, r3
    30d8:	e0a95fc3 	adc	r5, r9, r3, asr #31
    30dc:	e1a03005 	mov	r3, r5
    30e0:	e59d500c 	ldr	r5, [sp, #12]
    30e4:	e58d0008 	str	r0, [sp, #8]
    30e8:	e1a02004 	mov	r2, r4
    30ec:	e1a0000a 	mov	r0, sl
    30f0:	e58d1004 	str	r1, [sp, #4]
    30f4:	e58d5000 	str	r5, [sp]
    30f8:	eb000645 	bl	4a14 <vs_write>
    30fc:	e59d0008 	ldr	r0, [sp, #8]
    3100:	eb0015f6 	bl	88e0 <get_timer>
    3104:	e0656006 	rsb	r6, r5, r6
    3108:	e0877000 	add	r7, r7, r0
    310c:	e3560000 	cmp	r6, #0	; 0x0
    3110:	e1a0000a 	mov	r0, sl
    3114:	1affffe0 	bne	309c <tt_blk_trolling+0x1b8>
    3118:	ea00000b 	b	314c <tt_blk_trolling+0x268>
    311c:	e1a0000a 	mov	r0, sl
    3120:	eb0015ee 	bl	88e0 <get_timer>
    3124:	e59d1024 	ldr	r1, [sp, #36]
    3128:	e1a04000 	mov	r4, r0
    312c:	e1a02008 	mov	r2, r8
    3130:	e1a03009 	mov	r3, r9
    3134:	e1a0000b 	mov	r0, fp
    3138:	e88d0402 	stm	sp, {r1, sl}
    313c:	eb000634 	bl	4a14 <vs_write>
    3140:	e1a00004 	mov	r0, r4
    3144:	eb0015e5 	bl	88e0 <get_timer>
    3148:	e1a07000 	mov	r7, r0
    314c:	e59d0024 	ldr	r0, [sp, #36]
    3150:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    3154:	e1a00540 	asr	r0, r0, #10
    3158:	e1a01007 	mov	r1, r7
    315c:	e0000093 	mul	r0, r3, r0
    3160:	eb0033e6 	bl	10100 <__aeabi_uidiv>
    3164:	e59f3184 	ldr	r3, [pc, #388]	; 32f0 <tt_blk_trolling+0x40c>
    3168:	e58d0014 	str	r0, [sp, #20]
    316c:	e59f0190 	ldr	r0, [pc, #400]	; 3304 <tt_blk_trolling+0x420>
    3170:	e1a0e00f 	mov	lr, pc
    3174:	e593f000 	ldr	pc, [r3]
    3178:	e35a0000 	cmp	sl, #0	; 0x0
    317c:	13a07000 	movne	r7, #0	; 0x0
    3180:	159d6024 	ldrne	r6, [sp, #36]
    3184:	1a00001b 	bne	31f8 <tt_blk_trolling+0x314>
    3188:	ea00001e 	b	3208 <tt_blk_trolling+0x324>
    318c:	eb0015d3 	bl	88e0 <get_timer>
    3190:	e59d3024 	ldr	r3, [sp, #36]
    3194:	e3560902 	cmp	r6, #32768	; 0x8000
    3198:	b1a04006 	movlt	r4, r6
    319c:	a3a04902 	movge	r4, #32768	; 0x8000
    31a0:	e0663003 	rsb	r3, r6, r3
    31a4:	e1a05000 	mov	r5, r0
    31a8:	e0980003 	adds	r0, r8, r3
    31ac:	e0a91fc3 	adc	r1, r9, r3, asr #31
    31b0:	e1a02000 	mov	r2, r0
    31b4:	e1a03001 	mov	r3, r1
    31b8:	e1a0000a 	mov	r0, sl
    31bc:	e3a01000 	mov	r1, #0	; 0x0
    31c0:	e58d1004 	str	r1, [sp, #4]
    31c4:	e58d4000 	str	r4, [sp]
    31c8:	eb000711 	bl	4e14 <vs_read>
    31cc:	e1a00005 	mov	r0, r5
    31d0:	eb0015c2 	bl	88e0 <get_timer>
    31d4:	e1a05000 	mov	r5, r0
    31d8:	e59d0024 	ldr	r0, [sp, #36]
    31dc:	e1a0100a 	mov	r1, sl
    31e0:	e0660000 	rsb	r0, r6, r0
    31e4:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    31e8:	e1a02004 	mov	r2, r4
    31ec:	ebfff854 	bl	1344 <memcpy>
    31f0:	e0877005 	add	r7, r7, r5
    31f4:	e0646006 	rsb	r6, r4, r6
    31f8:	e3a00000 	mov	r0, #0	; 0x0
    31fc:	e1560000 	cmp	r6, r0
    3200:	1affffe1 	bne	318c <tt_blk_trolling+0x2a8>
    3204:	ea00000b 	b	3238 <tt_blk_trolling+0x354>
    3208:	e1a0000a 	mov	r0, sl
    320c:	eb0015b3 	bl	88e0 <get_timer>
    3210:	e59d1024 	ldr	r1, [sp, #36]
    3214:	e1a04000 	mov	r4, r0
    3218:	e1a02008 	mov	r2, r8
    321c:	e1a03009 	mov	r3, r9
    3220:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3224:	e88d0402 	stm	sp, {r1, sl}
    3228:	eb0006f9 	bl	4e14 <vs_read>
    322c:	e1a00004 	mov	r0, r4
    3230:	eb0015aa 	bl	88e0 <get_timer>
    3234:	e1a07000 	mov	r7, r0
    3238:	e59d0024 	ldr	r0, [sp, #36]
    323c:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    3240:	e1a00540 	asr	r0, r0, #10
    3244:	e0000093 	mul	r0, r3, r0
    3248:	e1a01007 	mov	r1, r7
    324c:	eb0033ab 	bl	10100 <__aeabi_uidiv>
    3250:	e59f4098 	ldr	r4, [pc, #152]	; 32f0 <tt_blk_trolling+0x40c>
    3254:	e1a05000 	mov	r5, r0
    3258:	e59f00a8 	ldr	r0, [pc, #168]	; 3308 <tt_blk_trolling+0x424>
    325c:	e1a0e00f 	mov	lr, pc
    3260:	e594f000 	ldr	pc, [r4]
    3264:	e1a0000b 	mov	r0, fp
    3268:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    326c:	e59d2024 	ldr	r2, [sp, #36]
    3270:	e3a0300c 	mov	r3, #12	; 0xc
    3274:	eb000404 	bl	428c <tt_cmp_buffer>
    3278:	e3500000 	cmp	r0, #0	; 0x0
    327c:	0a000007 	beq	32a0 <tt_blk_trolling+0x3bc>
    3280:	e58d0000 	str	r0, [sp]
    3284:	e59d1010 	ldr	r1, [sp, #16]
    3288:	e59d2014 	ldr	r2, [sp, #20]
    328c:	e1a03005 	mov	r3, r5
    3290:	e59f0074 	ldr	r0, [pc, #116]	; 330c <tt_blk_trolling+0x428>
    3294:	e1a0e00f 	mov	lr, pc
    3298:	e594f000 	ldr	pc, [r4]
    329c:	ea000005 	b	32b8 <tt_blk_trolling+0x3d4>
    32a0:	e59d1010 	ldr	r1, [sp, #16]
    32a4:	e59d2014 	ldr	r2, [sp, #20]
    32a8:	e1a03005 	mov	r3, r5
    32ac:	e59f005c 	ldr	r0, [pc, #92]	; 3310 <tt_blk_trolling+0x42c>
    32b0:	e1a0e00f 	mov	lr, pc
    32b4:	e594f000 	ldr	pc, [r4]
    32b8:	e59f0054 	ldr	r0, [pc, #84]	; 3314 <tt_blk_trolling+0x430>
    32bc:	e59d1024 	ldr	r1, [sp, #36]
    32c0:	e890000c 	ldm	r0, {r2, r3}
    32c4:	e0922001 	adds	r2, r2, r1
    32c8:	e0a33fc1 	adc	r3, r3, r1, asr #31
    32cc:	e880000c 	stm	r0, {r2, r3}
    32d0:	e59f0040 	ldr	r0, [pc, #64]	; 3318 <tt_blk_trolling+0x434>
    32d4:	e59f3014 	ldr	r3, [pc, #20]	; 32f0 <tt_blk_trolling+0x40c>
    32d8:	e1a0e00f 	mov	lr, pc
    32dc:	e593f000 	ldr	pc, [r3]
    32e0:	e3a00000 	mov	r0, #0	; 0x0
    32e4:	e28dd02c 	add	sp, sp, #44	; 0x2c
    32e8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    32ec:	00012651 	.word	0x00012651
    32f0:	0800d76c 	.word	0x0800d76c
    32f4:	0001266f 	.word	0x0001266f
    32f8:	0001268f 	.word	0x0001268f
    32fc:	00013ff9 	.word	0x00013ff9
    3300:	00011e91 	.word	0x00011e91
    3304:	00014380 	.word	0x00014380
    3308:	000126c3 	.word	0x000126c3
    330c:	000126c6 	.word	0x000126c6
    3310:	00012700 	.word	0x00012700
    3314:	0800d1c0 	.word	0x0800d1c0
    3318:	00012472 	.word	0x00012472

0000331c <do_tt_blk>:
    331c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3320:	e3520001 	cmp	r2, #1	; 0x1
    3324:	e24dd00c 	sub	sp, sp, #12	; 0xc
    3328:	e1a05002 	mov	r5, r2
    332c:	e1a07003 	mov	r7, r3
    3330:	ca000005 	bgt	334c <do_tt_blk+0x30>
    3334:	e59f01bc 	ldr	r0, [pc, #444]	; 34f8 <do_tt_blk+0x1dc>
    3338:	e59f31bc 	ldr	r3, [pc, #444]	; 34fc <do_tt_blk+0x1e0>
    333c:	e1a0e00f 	mov	lr, pc
    3340:	e593f000 	ldr	pc, [r3]
    3344:	e3e00000 	mvn	r0, #0	; 0x0
    3348:	ea000068 	b	34f0 <do_tt_blk+0x1d4>
    334c:	e3520002 	cmp	r2, #2	; 0x2
    3350:	03a04000 	moveq	r4, #0	; 0x0
    3354:	01a06004 	moveq	r6, r4
    3358:	0a000025 	beq	33f4 <do_tt_blk+0xd8>
    335c:	e3a01000 	mov	r1, #0	; 0x0
    3360:	e3a0200a 	mov	r2, #10	; 0xa
    3364:	e5930008 	ldr	r0, [r3, #8]
    3368:	eb002760 	bl	d0f0 <simple_strtoul>
    336c:	e59f118c 	ldr	r1, [pc, #396]	; 3500 <do_tt_blk+0x1e4>
    3370:	e1a04000 	mov	r4, r0
    3374:	e3a02005 	mov	r2, #5	; 0x5
    3378:	e5970004 	ldr	r0, [r7, #4]
    337c:	ebfff7b7 	bl	1260 <strncmp>
    3380:	e3500000 	cmp	r0, #0	; 0x0
    3384:	059f0178 	ldreq	r0, [pc, #376]	; 3504 <do_tt_blk+0x1e8>
    3388:	01a01004 	moveq	r1, r4
    338c:	059f3168 	ldreq	r3, [pc, #360]	; 34fc <do_tt_blk+0x1e0>
    3390:	0a000008 	beq	33b8 <do_tt_blk+0x9c>
    3394:	e59f116c 	ldr	r1, [pc, #364]	; 3508 <do_tt_blk+0x1ec>
    3398:	e5970004 	ldr	r0, [r7, #4]
    339c:	e3a02003 	mov	r2, #3	; 0x3
    33a0:	ebfff7ae 	bl	1260 <strncmp>
    33a4:	e3500000 	cmp	r0, #0	; 0x0
    33a8:	e59f314c 	ldr	r3, [pc, #332]	; 34fc <do_tt_blk+0x1e0>
    33ac:	059f0150 	ldreq	r0, [pc, #336]	; 3504 <do_tt_blk+0x1e8>
    33b0:	159f0154 	ldrne	r0, [pc, #340]	; 350c <do_tt_blk+0x1f0>
    33b4:	e1a01004 	mov	r1, r4
    33b8:	e1a0e00f 	mov	lr, pc
    33bc:	e593f000 	ldr	pc, [r3]
    33c0:	e3550003 	cmp	r5, #3	; 0x3
    33c4:	03a06000 	moveq	r6, #0	; 0x0
    33c8:	0a000009 	beq	33f4 <do_tt_blk+0xd8>
    33cc:	e3a01000 	mov	r1, #0	; 0x0
    33d0:	e597000c 	ldr	r0, [r7, #12]
    33d4:	e3a02010 	mov	r2, #16	; 0x10
    33d8:	eb002744 	bl	d0f0 <simple_strtoul>
    33dc:	e1a06000 	mov	r6, r0
    33e0:	e1a01006 	mov	r1, r6
    33e4:	e59f0124 	ldr	r0, [pc, #292]	; 3510 <do_tt_blk+0x1f4>
    33e8:	e59f310c 	ldr	r3, [pc, #268]	; 34fc <do_tt_blk+0x1e0>
    33ec:	e1a0e00f 	mov	lr, pc
    33f0:	e593f000 	ldr	pc, [r3]
    33f4:	e3a00000 	mov	r0, #0	; 0x0
    33f8:	eb001538 	bl	88e0 <get_timer>
    33fc:	e59f1110 	ldr	r1, [pc, #272]	; 3514 <do_tt_blk+0x1f8>
    3400:	e3a02000 	mov	r2, #0	; 0x0
    3404:	e3a03000 	mov	r3, #0	; 0x0
    3408:	e881000c 	stm	r1, {r2, r3}
    340c:	e1a09a04 	lsl	r9, r4, #20
    3410:	e2875004 	add	r5, r7, #4	; 0x4
    3414:	e1a08b24 	lsr	r8, r4, #22
    3418:	e1a07504 	lsl	r7, r4, #10
    341c:	e1a0a624 	lsr	sl, r4, #12
    3420:	e1a0b000 	mov	fp, r0
    3424:	e3a04000 	mov	r4, #0	; 0x0
    3428:	e59f30cc 	ldr	r3, [pc, #204]	; 34fc <do_tt_blk+0x1e0>
    342c:	e1a01004 	mov	r1, r4
    3430:	e59f00e0 	ldr	r0, [pc, #224]	; 3518 <do_tt_blk+0x1fc>
    3434:	e1a0e00f 	mov	lr, pc
    3438:	e593f000 	ldr	pc, [r3]
    343c:	e59f10bc 	ldr	r1, [pc, #188]	; 3500 <do_tt_blk+0x1e4>
    3440:	e3a02005 	mov	r2, #5	; 0x5
    3444:	e5950000 	ldr	r0, [r5]
    3448:	ebfff784 	bl	1260 <strncmp>
    344c:	e3500000 	cmp	r0, #0	; 0x0
    3450:	e3a02003 	mov	r2, #3	; 0x3
    3454:	e59f10ac 	ldr	r1, [pc, #172]	; 3508 <do_tt_blk+0x1ec>
    3458:	e1a03008 	mov	r3, r8
    345c:	05950000 	ldreq	r0, [r5]
    3460:	01a02007 	moveq	r2, r7
    3464:	0a000008 	beq	348c <do_tt_blk+0x170>
    3468:	e5950000 	ldr	r0, [r5]
    346c:	ebfff77b 	bl	1260 <strncmp>
    3470:	e3500000 	cmp	r0, #0	; 0x0
    3474:	05950000 	ldreq	r0, [r5]
    3478:	15950000 	ldrne	r0, [r5]
    347c:	e1a02009 	mov	r2, r9
    3480:	e1a0300a 	mov	r3, sl
    3484:	01a02007 	moveq	r2, r7
    3488:	01a03008 	moveq	r3, r8
    348c:	e58d6000 	str	r6, [sp]
    3490:	ebfffe93 	bl	2ee4 <tt_blk_trolling>
    3494:	eb002b2c 	bl	e14c <tstc>
    3498:	e3500000 	cmp	r0, #0	; 0x0
    349c:	e2844001 	add	r4, r4, #1	; 0x1
    34a0:	0affffe0 	beq	3428 <do_tt_blk+0x10c>
    34a4:	eb002b29 	bl	e150 <getc>
    34a8:	e3500003 	cmp	r0, #3	; 0x3
    34ac:	1affffdd 	bne	3428 <do_tt_blk+0x10c>
    34b0:	e1a0000b 	mov	r0, fp
    34b4:	eb001509 	bl	88e0 <get_timer>
    34b8:	e59f3054 	ldr	r3, [pc, #84]	; 3514 <do_tt_blk+0x1f8>
    34bc:	e30e1a60 	movw	r1, #60000	; 0xea60
    34c0:	e5934000 	ldr	r4, [r3]
    34c4:	e5933004 	ldr	r3, [r3, #4]
    34c8:	e1a04a24 	lsr	r4, r4, #20
    34cc:	e1844603 	orr	r4, r4, r3, lsl #12
    34d0:	eb00330a 	bl	10100 <__aeabi_uidiv>
    34d4:	e1a01004 	mov	r1, r4
    34d8:	e1a02000 	mov	r2, r0
    34dc:	e59f3018 	ldr	r3, [pc, #24]	; 34fc <do_tt_blk+0x1e0>
    34e0:	e59f0034 	ldr	r0, [pc, #52]	; 351c <do_tt_blk+0x200>
    34e4:	e1a0e00f 	mov	lr, pc
    34e8:	e593f000 	ldr	pc, [r3]
    34ec:	e3a00000 	mov	r0, #0	; 0x0
    34f0:	e28dd00c 	add	sp, sp, #12	; 0xc
    34f4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    34f8:	0001272d 	.word	0x0001272d
    34fc:	0800d76c 	.word	0x0800d76c
    3500:	00011b5b 	.word	0x00011b5b
    3504:	00012774 	.word	0x00012774
    3508:	00012132 	.word	0x00012132
    350c:	00012798 	.word	0x00012798
    3510:	000127bc 	.word	0x000127bc
    3514:	0800d1c0 	.word	0x0800d1c0
    3518:	000127df 	.word	0x000127df
    351c:	000127e9 	.word	0x000127e9

00003520 <__show_strbuf>:
    3520:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    3524:	e59f4058 	ldr	r4, [pc, #88]	; 3584 <__show_strbuf+0x64>
    3528:	e24dd004 	sub	sp, sp, #4	; 0x4
    352c:	e1a06001 	mov	r6, r1
    3530:	e1a01000 	mov	r1, r0
    3534:	e59f004c 	ldr	r0, [pc, #76]	; 3588 <__show_strbuf+0x68>
    3538:	e1a05002 	mov	r5, r2
    353c:	e1a07004 	mov	r7, r4
    3540:	e1a0e00f 	mov	lr, pc
    3544:	e594f000 	ldr	pc, [r4]
    3548:	e3a04000 	mov	r4, #0	; 0x0
    354c:	ea000003 	b	3560 <__show_strbuf+0x40>
    3550:	e7d61004 	ldrb	r1, [r6, r4]
    3554:	e1a0e00f 	mov	lr, pc
    3558:	e597f000 	ldr	pc, [r7]
    355c:	e2844001 	add	r4, r4, #1	; 0x1
    3560:	e1540005 	cmp	r4, r5
    3564:	e59f0020 	ldr	r0, [pc, #32]	; 358c <__show_strbuf+0x6c>
    3568:	bafffff8 	blt	3550 <__show_strbuf+0x30>
    356c:	e59f001c 	ldr	r0, [pc, #28]	; 3590 <__show_strbuf+0x70>
    3570:	e59f300c 	ldr	r3, [pc, #12]	; 3584 <__show_strbuf+0x64>
    3574:	e1a0e00f 	mov	lr, pc
    3578:	e593f000 	ldr	pc, [r3]
    357c:	e28dd004 	add	sp, sp, #4	; 0x4
    3580:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    3584:	0800d76c 	.word	0x0800d76c
    3588:	00012837 	.word	0x00012837
    358c:	000141b2 	.word	0x000141b2
    3590:	00012472 	.word	0x00012472

00003594 <do_crypto_quick>:
    3594:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    3598:	e24dd0f0 	sub	sp, sp, #240	; 0xf0
    359c:	e28da0e0 	add	sl, sp, #224	; 0xe0
    35a0:	e3a04010 	mov	r4, #16	; 0x10
    35a4:	e28d70c0 	add	r7, sp, #192	; 0xc0
    35a8:	e1a02004 	mov	r2, r4
    35ac:	e3a09020 	mov	r9, #32	; 0x20
    35b0:	e59f11f0 	ldr	r1, [pc, #496]	; 37a8 <do_crypto_quick+0x214>
    35b4:	e1a0000a 	mov	r0, sl
    35b8:	e3a05040 	mov	r5, #64	; 0x40
    35bc:	e28d6080 	add	r6, sp, #128	; 0x80
    35c0:	ebfff75f 	bl	1344 <memcpy>
    35c4:	e1a02009 	mov	r2, r9
    35c8:	e59f11dc 	ldr	r1, [pc, #476]	; 37ac <do_crypto_quick+0x218>
    35cc:	e1a00007 	mov	r0, r7
    35d0:	ebfff75b 	bl	1344 <memcpy>
    35d4:	e08d8005 	add	r8, sp, r5
    35d8:	e1a02005 	mov	r2, r5
    35dc:	e59f11cc 	ldr	r1, [pc, #460]	; 37b0 <do_crypto_quick+0x21c>
    35e0:	e1a00006 	mov	r0, r6
    35e4:	ebfff756 	bl	1344 <memcpy>
    35e8:	e1a02005 	mov	r2, r5
    35ec:	e59f11c0 	ldr	r1, [pc, #448]	; 37b4 <do_crypto_quick+0x220>
    35f0:	e1a00008 	mov	r0, r8
    35f4:	ebfff752 	bl	1344 <memcpy>
    35f8:	e1a02004 	mov	r2, r4
    35fc:	e1a0100a 	mov	r1, sl
    3600:	e59f01b0 	ldr	r0, [pc, #432]	; 37b8 <do_crypto_quick+0x224>
    3604:	ebffffc5 	bl	3520 <__show_strbuf>
    3608:	e1a02009 	mov	r2, r9
    360c:	e1a01007 	mov	r1, r7
    3610:	e59f01a4 	ldr	r0, [pc, #420]	; 37bc <do_crypto_quick+0x228>
    3614:	ebffffc1 	bl	3520 <__show_strbuf>
    3618:	e1a01006 	mov	r1, r6
    361c:	e1a02005 	mov	r2, r5
    3620:	e59f0198 	ldr	r0, [pc, #408]	; 37c0 <do_crypto_quick+0x22c>
    3624:	ebffffbd 	bl	3520 <__show_strbuf>
    3628:	e1a01008 	mov	r1, r8
    362c:	e1a02005 	mov	r2, r5
    3630:	e59f018c 	ldr	r0, [pc, #396]	; 37c4 <do_crypto_quick+0x230>
    3634:	ebffffb9 	bl	3520 <__show_strbuf>
    3638:	e1a01007 	mov	r1, r7
    363c:	e1a0200a 	mov	r2, sl
    3640:	e3a00001 	mov	r0, #1	; 0x1
    3644:	eb0006fa 	bl	5234 <ss_aes_init>
    3648:	e1a0100d 	mov	r1, sp
    364c:	e1a02005 	mov	r2, r5
    3650:	e1a00006 	mov	r0, r6
    3654:	eb0006e5 	bl	51f0 <ss_aes_data>
    3658:	e1a0100d 	mov	r1, sp
    365c:	e1a02005 	mov	r2, r5
    3660:	e59f0160 	ldr	r0, [pc, #352]	; 37c8 <do_crypto_quick+0x234>
    3664:	ebffffad 	bl	3520 <__show_strbuf>
    3668:	e1a01007 	mov	r1, r7
    366c:	e1a0200a 	mov	r2, sl
    3670:	e3a00000 	mov	r0, #0	; 0x0
    3674:	eb0006ee 	bl	5234 <ss_aes_init>
    3678:	e59f714c 	ldr	r7, [pc, #332]	; 37cc <do_crypto_quick+0x238>
    367c:	e1a0100d 	mov	r1, sp
    3680:	e1a02005 	mov	r2, r5
    3684:	e1a00008 	mov	r0, r8
    3688:	eb0006d8 	bl	51f0 <ss_aes_data>
    368c:	e1a0100d 	mov	r1, sp
    3690:	e1a02005 	mov	r2, r5
    3694:	e59f0134 	ldr	r0, [pc, #308]	; 37d0 <do_crypto_quick+0x23c>
    3698:	ebffffa0 	bl	3520 <__show_strbuf>
    369c:	e1a02007 	mov	r2, r7
    36a0:	e3a01000 	mov	r1, #0	; 0x0
    36a4:	e3a00001 	mov	r0, #1	; 0x1
    36a8:	eb0006e1 	bl	5234 <ss_aes_init>
    36ac:	e1a0100d 	mov	r1, sp
    36b0:	e1a02005 	mov	r2, r5
    36b4:	e1a00006 	mov	r0, r6
    36b8:	eb0006cc 	bl	51f0 <ss_aes_data>
    36bc:	e1a0100d 	mov	r1, sp
    36c0:	e1a02005 	mov	r2, r5
    36c4:	e59f0108 	ldr	r0, [pc, #264]	; 37d4 <do_crypto_quick+0x240>
    36c8:	ebffff94 	bl	3520 <__show_strbuf>
    36cc:	e3a00000 	mov	r0, #0	; 0x0
    36d0:	e1a02007 	mov	r2, r7
    36d4:	e1a01000 	mov	r1, r0
    36d8:	eb0006d5 	bl	5234 <ss_aes_init>
    36dc:	e1a0100d 	mov	r1, sp
    36e0:	e1a02005 	mov	r2, r5
    36e4:	e1a0000d 	mov	r0, sp
    36e8:	eb0006c0 	bl	51f0 <ss_aes_data>
    36ec:	e1a02005 	mov	r2, r5
    36f0:	e1a0100d 	mov	r1, sp
    36f4:	e59f00dc 	ldr	r0, [pc, #220]	; 37d8 <do_crypto_quick+0x244>
    36f8:	ebffff88 	bl	3520 <__show_strbuf>
    36fc:	e1a01005 	mov	r1, r5
    3700:	e3a00001 	mov	r0, #1	; 0x1
    3704:	eb0007e8 	bl	56ac <hash_init>
    3708:	e1a01005 	mov	r1, r5
    370c:	e1a00006 	mov	r0, r6
    3710:	eb0007c0 	bl	5618 <hash_data>
    3714:	e1a0000d 	mov	r0, sp
    3718:	eb0007a2 	bl	55a8 <hash_value>
    371c:	eb00079a 	bl	558c <hash_len>
    3720:	e1a0100d 	mov	r1, sp
    3724:	e1a02000 	mov	r2, r0
    3728:	e59f00ac 	ldr	r0, [pc, #172]	; 37dc <do_crypto_quick+0x248>
    372c:	ebffff7b 	bl	3520 <__show_strbuf>
    3730:	e1a01005 	mov	r1, r5
    3734:	e3a00002 	mov	r0, #2	; 0x2
    3738:	eb0007db 	bl	56ac <hash_init>
    373c:	e1a01005 	mov	r1, r5
    3740:	e1a00006 	mov	r0, r6
    3744:	eb0007b3 	bl	5618 <hash_data>
    3748:	e1a0000d 	mov	r0, sp
    374c:	eb000795 	bl	55a8 <hash_value>
    3750:	eb00078d 	bl	558c <hash_len>
    3754:	e1a0100d 	mov	r1, sp
    3758:	e1a02000 	mov	r2, r0
    375c:	e59f007c 	ldr	r0, [pc, #124]	; 37e0 <do_crypto_quick+0x24c>
    3760:	ebffff6e 	bl	3520 <__show_strbuf>
    3764:	e1a01005 	mov	r1, r5
    3768:	e3a00003 	mov	r0, #3	; 0x3
    376c:	eb0007ce 	bl	56ac <hash_init>
    3770:	e1a01005 	mov	r1, r5
    3774:	e1a00006 	mov	r0, r6
    3778:	eb0007a6 	bl	5618 <hash_data>
    377c:	e1a0000d 	mov	r0, sp
    3780:	eb000788 	bl	55a8 <hash_value>
    3784:	eb000780 	bl	558c <hash_len>
    3788:	e1a0100d 	mov	r1, sp
    378c:	e1a02000 	mov	r2, r0
    3790:	e59f004c 	ldr	r0, [pc, #76]	; 37e4 <do_crypto_quick+0x250>
    3794:	ebffff61 	bl	3520 <__show_strbuf>
    3798:	e1a0400d 	mov	r4, sp
    379c:	e3a00000 	mov	r0, #0	; 0x0
    37a0:	e28dd0f0 	add	sp, sp, #240	; 0xf0
    37a4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    37a8:	00010eac 	.word	0x00010eac
    37ac:	00010e8c 	.word	0x00010e8c
    37b0:	00010e4c 	.word	0x00010e4c
    37b4:	00010e0c 	.word	0x00010e0c
    37b8:	0001283c 	.word	0x0001283c
    37bc:	0001283f 	.word	0x0001283f
    37c0:	00012843 	.word	0x00012843
    37c4:	0001284e 	.word	0x0001284e
    37c8:	0001285f 	.word	0x0001285f
    37cc:	0800d10c 	.word	0x0800d10c
    37d0:	00012872 	.word	0x00012872
    37d4:	0001285b 	.word	0x0001285b
    37d8:	0001286e 	.word	0x0001286e
    37dc:	00012881 	.word	0x00012881
    37e0:	00012885 	.word	0x00012885
    37e4:	0001288a 	.word	0x0001288a

000037e8 <tt_crypto_hash>:
    37e8:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    37ec:	e3a0544e 	mov	r5, #1308622848	; 0x4e000000
    37f0:	e5951000 	ldr	r1, [r5]
    37f4:	e59f2220 	ldr	r2, [pc, #544]	; 3a1c <tt_crypto_hash+0x234>
    37f8:	e24dd038 	sub	sp, sp, #56	; 0x38
    37fc:	e59f321c 	ldr	r3, [pc, #540]	; 3a20 <tt_crypto_hash+0x238>
    3800:	e1510002 	cmp	r1, r2
    3804:	e28dc028 	add	ip, sp, #40	; 0x28
    3808:	e1a06000 	mov	r6, r0
    380c:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    3810:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    3814:	0a000006 	beq	3834 <tt_crypto_hash+0x4c>
    3818:	e59f0204 	ldr	r0, [pc, #516]	; 3a24 <tt_crypto_hash+0x23c>
    381c:	e59f3204 	ldr	r3, [pc, #516]	; 3a28 <tt_crypto_hash+0x240>
    3820:	e1a0e00f 	mov	lr, pc
    3824:	e593f000 	ldr	pc, [r3]
    3828:	e59f01fc 	ldr	r0, [pc, #508]	; 3a2c <tt_crypto_hash+0x244>
    382c:	eb001407 	bl	8850 <udelay>
    3830:	ea000008 	b	3858 <tt_crypto_hash+0x70>
    3834:	e5951004 	ldr	r1, [r5, #4]
    3838:	e3a04000 	mov	r4, #0	; 0x0
    383c:	e3510003 	cmp	r1, #3	; 0x3
    3840:	e5854000 	str	r4, [r5]
    3844:	e59fc1dc 	ldr	ip, [pc, #476]	; 3a28 <tt_crypto_hash+0x240>
    3848:	9a000004 	bls	3860 <tt_crypto_hash+0x78>
    384c:	e59f01dc 	ldr	r0, [pc, #476]	; 3a30 <tt_crypto_hash+0x248>
    3850:	e1a0e00f 	mov	lr, pc
    3854:	e59cf000 	ldr	pc, [ip]
    3858:	e3e0c000 	mvn	ip, #0	; 0x0
    385c:	ea00006b 	b	3a10 <tt_crypto_hash+0x228>
    3860:	e28d2038 	add	r2, sp, #56	; 0x38
    3864:	e0823101 	add	r3, r2, r1, lsl #2
    3868:	e5131010 	ldr	r1, [r3, #-16]
    386c:	e5952008 	ldr	r2, [r5, #8]
    3870:	e59f01bc 	ldr	r0, [pc, #444]	; 3a34 <tt_crypto_hash+0x24c>
    3874:	e1a0e00f 	mov	lr, pc
    3878:	e59cf000 	ldr	pc, [ip]
    387c:	e9950003 	ldmib	r5, {r0, r1}
    3880:	eb000789 	bl	56ac <hash_init>
    3884:	eb000740 	bl	558c <hash_len>
    3888:	e1a03000 	mov	r3, r0
    388c:	e59f01a4 	ldr	r0, [pc, #420]	; 3a38 <tt_crypto_hash+0x250>
    3890:	e3a0c020 	mov	ip, #32	; 0x20
    3894:	e1a01000 	mov	r1, r0
    3898:	e3a02001 	mov	r2, #1	; 0x1
    389c:	e58dc000 	str	ip, [sp]
    38a0:	eb00257a 	bl	ce90 <print_buffer>
    38a4:	e3560000 	cmp	r6, #0	; 0x0
    38a8:	0a000022 	beq	3938 <tt_crypto_hash+0x150>
    38ac:	e1a09005 	mov	r9, r5
    38b0:	e1a0a004 	mov	sl, r4
    38b4:	e5955008 	ldr	r5, [r5, #8]
    38b8:	e2868901 	add	r8, r6, #16384	; 0x4000
    38bc:	ea000019 	b	3928 <tt_crypto_hash+0x140>
    38c0:	e5991008 	ldr	r1, [r9, #8]
    38c4:	e3550902 	cmp	r5, #32768	; 0x8000
    38c8:	b1a04005 	movlt	r4, r5
    38cc:	a3a04902 	movge	r4, #32768	; 0x8000
    38d0:	e0651001 	rsb	r1, r5, r1
    38d4:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    38d8:	e2811030 	add	r1, r1, #48	; 0x30
    38dc:	e1a02004 	mov	r2, r4
    38e0:	ebfff697 	bl	1344 <memcpy>
    38e4:	e3a00000 	mov	r0, #0	; 0x0
    38e8:	eb0013fc 	bl	88e0 <get_timer>
    38ec:	e3550901 	cmp	r5, #16384	; 0x4000
    38f0:	b1a01005 	movlt	r1, r5
    38f4:	a3a01901 	movge	r1, #16384	; 0x4000
    38f8:	e1a07000 	mov	r7, r0
    38fc:	e1a00006 	mov	r0, r6
    3900:	eb000744 	bl	5618 <hash_data>
    3904:	e3540902 	cmp	r4, #32768	; 0x8000
    3908:	e0645005 	rsb	r5, r4, r5
    390c:	1a000002 	bne	391c <tt_crypto_hash+0x134>
    3910:	e3a01901 	mov	r1, #16384	; 0x4000
    3914:	e1a00008 	mov	r0, r8
    3918:	eb00073e 	bl	5618 <hash_data>
    391c:	e1a00007 	mov	r0, r7
    3920:	eb0013ee 	bl	88e0 <get_timer>
    3924:	e08aa000 	add	sl, sl, r0
    3928:	e3550000 	cmp	r5, #0	; 0x0
    392c:	e1a00006 	mov	r0, r6
    3930:	1affffe2 	bne	38c0 <tt_crypto_hash+0xd8>
    3934:	ea000008 	b	395c <tt_crypto_hash+0x174>
    3938:	e1a00006 	mov	r0, r6
    393c:	eb0013e7 	bl	88e0 <get_timer>
    3940:	e5951008 	ldr	r1, [r5, #8]
    3944:	e1a04000 	mov	r4, r0
    3948:	e59f00ec 	ldr	r0, [pc, #236]	; 3a3c <tt_crypto_hash+0x254>
    394c:	eb000731 	bl	5618 <hash_data>
    3950:	e1a00004 	mov	r0, r4
    3954:	eb0013e1 	bl	88e0 <get_timer>
    3958:	e1a0a000 	mov	sl, r0
    395c:	e28d4008 	add	r4, sp, #8	; 0x8
    3960:	e3a0644e 	mov	r6, #1308622848	; 0x4e000000
    3964:	e1a00004 	mov	r0, r4
    3968:	eb00070e 	bl	55a8 <hash_value>
    396c:	e5965008 	ldr	r5, [r6, #8]
    3970:	eb000705 	bl	558c <hash_len>
    3974:	e3a0c020 	mov	ip, #32	; 0x20
    3978:	e1a03000 	mov	r3, r0
    397c:	e1a01004 	mov	r1, r4
    3980:	e3a02001 	mov	r2, #1	; 0x1
    3984:	e1a00004 	mov	r0, r4
    3988:	e58dc000 	str	ip, [sp]
    398c:	eb00253f 	bl	ce90 <print_buffer>
    3990:	e5963004 	ldr	r3, [r6, #4]
    3994:	e28d1038 	add	r1, sp, #56	; 0x38
    3998:	e0812103 	add	r2, r1, r3, lsl #2
    399c:	e59f3084 	ldr	r3, [pc, #132]	; 3a28 <tt_crypto_hash+0x240>
    39a0:	e5127010 	ldr	r7, [r2, #-16]
    39a4:	e5938000 	ldr	r8, [r3]
    39a8:	eb0006f7 	bl	558c <hash_len>
    39ac:	e1a01004 	mov	r1, r4
    39b0:	e1a02000 	mov	r2, r0
    39b4:	e59f007c 	ldr	r0, [pc, #124]	; 3a38 <tt_crypto_hash+0x250>
    39b8:	ebfff682 	bl	13c8 <memcmp>
    39bc:	e1a05525 	lsr	r5, r5, #10
    39c0:	e3500000 	cmp	r0, #0	; 0x0
    39c4:	e59f3074 	ldr	r3, [pc, #116]	; 3a40 <tt_crypto_hash+0x258>
    39c8:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    39cc:	e1a0100a 	mov	r1, sl
    39d0:	e0000590 	mul	r0, r0, r5
    39d4:	e59f4068 	ldr	r4, [pc, #104]	; 3a44 <tt_crypto_hash+0x25c>
    39d8:	01a04003 	moveq	r4, r3
    39dc:	eb0031c7 	bl	10100 <__aeabi_uidiv>
    39e0:	e1a01007 	mov	r1, r7
    39e4:	e1a03000 	mov	r3, r0
    39e8:	e1a02004 	mov	r2, r4
    39ec:	e59f0054 	ldr	r0, [pc, #84]	; 3a48 <tt_crypto_hash+0x260>
    39f0:	e12fff38 	blx	r8
    39f4:	e59f1050 	ldr	r1, [pc, #80]	; 3a4c <tt_crypto_hash+0x264>
    39f8:	e5960008 	ldr	r0, [r6, #8]
    39fc:	e891000c 	ldm	r1, {r2, r3}
    3a00:	e0922000 	adds	r2, r2, r0
    3a04:	e2a33000 	adc	r3, r3, #0	; 0x0
    3a08:	e881000c 	stm	r1, {r2, r3}
    3a0c:	e3a0c000 	mov	ip, #0	; 0x0
    3a10:	e1a0000c 	mov	r0, ip
    3a14:	e28dd038 	add	sp, sp, #56	; 0x38
    3a18:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3a1c:	99997777 	.word	0x99997777
    3a20:	00010ebc 	.word	0x00010ebc
    3a24:	00012891 	.word	0x00012891
    3a28:	0800d76c 	.word	0x0800d76c
    3a2c:	004c4b40 	.word	0x004c4b40
    3a30:	000128b0 	.word	0x000128b0
    3a34:	000128cb 	.word	0x000128cb
    3a38:	4e000010 	.word	0x4e000010
    3a3c:	4e000030 	.word	0x4e000030
    3a40:	000128e6 	.word	0x000128e6
    3a44:	000128eb 	.word	0x000128eb
    3a48:	000128f2 	.word	0x000128f2
    3a4c:	0800d1d0 	.word	0x0800d1d0

00003a50 <tt_crypto_aes>:
    3a50:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3a54:	e24dd014 	sub	sp, sp, #20	; 0x14
    3a58:	e1a06000 	mov	r6, r0
    3a5c:	e3a01000 	mov	r1, #0	; 0x0
    3a60:	e28d000c 	add	r0, sp, #12	; 0xc
    3a64:	e3a02502 	mov	r2, #8388608	; 0x800000
    3a68:	eb000284 	bl	4480 <tt_rnd_get_data>
    3a6c:	e250b000 	subs	fp, r0, #0	; 0x0
    3a70:	e59f4340 	ldr	r4, [pc, #832]	; 3db8 <tt_crypto_aes+0x368>
    3a74:	059f0340 	ldreq	r0, [pc, #832]	; 3dbc <tt_crypto_aes+0x36c>
    3a78:	0a00005c 	beq	3bf0 <tt_crypto_aes+0x1a0>
    3a7c:	e59d200c 	ldr	r2, [sp, #12]
    3a80:	e1a0100b 	mov	r1, fp
    3a84:	e3c2200f 	bic	r2, r2, #15	; 0xf
    3a88:	e1a03002 	mov	r3, r2
    3a8c:	e58d200c 	str	r2, [sp, #12]
    3a90:	e59f0328 	ldr	r0, [pc, #808]	; 3dc0 <tt_crypto_aes+0x370>
    3a94:	e1a02006 	mov	r2, r6
    3a98:	e1a0e00f 	mov	lr, pc
    3a9c:	e594f000 	ldr	pc, [r4]
    3aa0:	e59f031c 	ldr	r0, [pc, #796]	; 3dc4 <tt_crypto_aes+0x374>
    3aa4:	e1a0e00f 	mov	lr, pc
    3aa8:	e594f000 	ldr	pc, [r4]
    3aac:	e1a0100b 	mov	r1, fp
    3ab0:	e59d200c 	ldr	r2, [sp, #12]
    3ab4:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3ab8:	ebfff621 	bl	1344 <memcpy>
    3abc:	e59f0304 	ldr	r0, [pc, #772]	; 3dc8 <tt_crypto_aes+0x378>
    3ac0:	e1a0e00f 	mov	lr, pc
    3ac4:	e594f000 	ldr	pc, [r4]
    3ac8:	e3a01000 	mov	r1, #0	; 0x0
    3acc:	e59f22f8 	ldr	r2, [pc, #760]	; 3dcc <tt_crypto_aes+0x37c>
    3ad0:	e3a00001 	mov	r0, #1	; 0x1
    3ad4:	eb0005d6 	bl	5234 <ss_aes_init>
    3ad8:	e59f02f0 	ldr	r0, [pc, #752]	; 3dd0 <tt_crypto_aes+0x380>
    3adc:	e1a0e00f 	mov	lr, pc
    3ae0:	e594f000 	ldr	pc, [r4]
    3ae4:	e3560000 	cmp	r6, #0	; 0x0
    3ae8:	159d500c 	ldrne	r5, [sp, #12]
    3aec:	13a0a000 	movne	sl, #0	; 0x0
    3af0:	12868901 	addne	r8, r6, #16384	; 0x4000
    3af4:	1a000022 	bne	3b84 <tt_crypto_aes+0x134>
    3af8:	ea000025 	b	3b94 <tt_crypto_aes+0x144>
    3afc:	e59d100c 	ldr	r1, [sp, #12]
    3b00:	e3550902 	cmp	r5, #32768	; 0x8000
    3b04:	b1a07005 	movlt	r7, r5
    3b08:	a3a07902 	movge	r7, #32768	; 0x8000
    3b0c:	e0651001 	rsb	r1, r5, r1
    3b10:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3b14:	e1a02007 	mov	r2, r7
    3b18:	ebfff609 	bl	1344 <memcpy>
    3b1c:	e3a00000 	mov	r0, #0	; 0x0
    3b20:	eb00136e 	bl	88e0 <get_timer>
    3b24:	e3550901 	cmp	r5, #16384	; 0x4000
    3b28:	b1a02005 	movlt	r2, r5
    3b2c:	a3a02901 	movge	r2, #16384	; 0x4000
    3b30:	e1a04000 	mov	r4, r0
    3b34:	e1a01006 	mov	r1, r6
    3b38:	e1a00006 	mov	r0, r6
    3b3c:	eb0005ab 	bl	51f0 <ss_aes_data>
    3b40:	e3570902 	cmp	r7, #32768	; 0x8000
    3b44:	1a000003 	bne	3b58 <tt_crypto_aes+0x108>
    3b48:	e1a01008 	mov	r1, r8
    3b4c:	e3a02901 	mov	r2, #16384	; 0x4000
    3b50:	e1a00008 	mov	r0, r8
    3b54:	eb0005a5 	bl	51f0 <ss_aes_data>
    3b58:	e1a00004 	mov	r0, r4
    3b5c:	eb00135f 	bl	88e0 <get_timer>
    3b60:	e1a04000 	mov	r4, r0
    3b64:	e59d000c 	ldr	r0, [sp, #12]
    3b68:	e1a01006 	mov	r1, r6
    3b6c:	e0650000 	rsb	r0, r5, r0
    3b70:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    3b74:	e1a02007 	mov	r2, r7
    3b78:	ebfff5f1 	bl	1344 <memcpy>
    3b7c:	e08aa004 	add	sl, sl, r4
    3b80:	e0675005 	rsb	r5, r7, r5
    3b84:	e3550000 	cmp	r5, #0	; 0x0
    3b88:	e1a00006 	mov	r0, r6
    3b8c:	1affffda 	bne	3afc <tt_crypto_aes+0xac>
    3b90:	ea000009 	b	3bbc <tt_crypto_aes+0x16c>
    3b94:	e1a00006 	mov	r0, r6
    3b98:	eb001350 	bl	88e0 <get_timer>
    3b9c:	e1a04000 	mov	r4, r0
    3ba0:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3ba4:	e1a01000 	mov	r1, r0
    3ba8:	e59d200c 	ldr	r2, [sp, #12]
    3bac:	eb00058f 	bl	51f0 <ss_aes_data>
    3bb0:	e1a00004 	mov	r0, r4
    3bb4:	eb001349 	bl	88e0 <get_timer>
    3bb8:	e1a0a000 	mov	sl, r0
    3bbc:	e59d300c 	ldr	r3, [sp, #12]
    3bc0:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3bc4:	e1a0100b 	mov	r1, fp
    3bc8:	e3a02010 	mov	r2, #16	; 0x10
    3bcc:	e58d3004 	str	r3, [sp, #4]
    3bd0:	ebfff5fc 	bl	13c8 <memcmp>
    3bd4:	e3500000 	cmp	r0, #0	; 0x0
    3bd8:	e59f41d8 	ldr	r4, [pc, #472]	; 3db8 <tt_crypto_aes+0x368>
    3bdc:	1a000007 	bne	3c00 <tt_crypto_aes+0x1b0>
    3be0:	e59f01ec 	ldr	r0, [pc, #492]	; 3dd4 <tt_crypto_aes+0x384>
    3be4:	e1a0e00f 	mov	lr, pc
    3be8:	e594f000 	ldr	pc, [r4]
    3bec:	e59f01e4 	ldr	r0, [pc, #484]	; 3dd8 <tt_crypto_aes+0x388>
    3bf0:	e1a0e00f 	mov	lr, pc
    3bf4:	e594f000 	ldr	pc, [r4]
    3bf8:	e3e00000 	mvn	r0, #0	; 0x0
    3bfc:	ea00006b 	b	3db0 <tt_crypto_aes+0x360>
    3c00:	e3a00000 	mov	r0, #0	; 0x0
    3c04:	e1a01000 	mov	r1, r0
    3c08:	e59f21bc 	ldr	r2, [pc, #444]	; 3dcc <tt_crypto_aes+0x37c>
    3c0c:	eb000588 	bl	5234 <ss_aes_init>
    3c10:	e59f01c4 	ldr	r0, [pc, #452]	; 3ddc <tt_crypto_aes+0x38c>
    3c14:	e1a0e00f 	mov	lr, pc
    3c18:	e594f000 	ldr	pc, [r4]
    3c1c:	e3560000 	cmp	r6, #0	; 0x0
    3c20:	159d500c 	ldrne	r5, [sp, #12]
    3c24:	13a08000 	movne	r8, #0	; 0x0
    3c28:	12869901 	addne	r9, r6, #16384	; 0x4000
    3c2c:	1a000022 	bne	3cbc <tt_crypto_aes+0x26c>
    3c30:	ea000025 	b	3ccc <tt_crypto_aes+0x27c>
    3c34:	eb001329 	bl	88e0 <get_timer>
    3c38:	e59d100c 	ldr	r1, [sp, #12]
    3c3c:	e3550902 	cmp	r5, #32768	; 0x8000
    3c40:	b1a07005 	movlt	r7, r5
    3c44:	a3a07902 	movge	r7, #32768	; 0x8000
    3c48:	e0651001 	rsb	r1, r5, r1
    3c4c:	e281144e 	add	r1, r1, #1308622848	; 0x4e000000
    3c50:	e1a02007 	mov	r2, r7
    3c54:	e1a04000 	mov	r4, r0
    3c58:	e1a00006 	mov	r0, r6
    3c5c:	ebfff5b8 	bl	1344 <memcpy>
    3c60:	e3550901 	cmp	r5, #16384	; 0x4000
    3c64:	b1a02005 	movlt	r2, r5
    3c68:	a3a02901 	movge	r2, #16384	; 0x4000
    3c6c:	e1a00006 	mov	r0, r6
    3c70:	e1a01006 	mov	r1, r6
    3c74:	eb00055d 	bl	51f0 <ss_aes_data>
    3c78:	e3570902 	cmp	r7, #32768	; 0x8000
    3c7c:	1a000003 	bne	3c90 <tt_crypto_aes+0x240>
    3c80:	e1a01009 	mov	r1, r9
    3c84:	e3a02901 	mov	r2, #16384	; 0x4000
    3c88:	e1a00009 	mov	r0, r9
    3c8c:	eb000557 	bl	51f0 <ss_aes_data>
    3c90:	e1a00004 	mov	r0, r4
    3c94:	eb001311 	bl	88e0 <get_timer>
    3c98:	e1a04000 	mov	r4, r0
    3c9c:	e59d000c 	ldr	r0, [sp, #12]
    3ca0:	e1a01006 	mov	r1, r6
    3ca4:	e0650000 	rsb	r0, r5, r0
    3ca8:	e280044e 	add	r0, r0, #1308622848	; 0x4e000000
    3cac:	e1a02007 	mov	r2, r7
    3cb0:	ebfff5a3 	bl	1344 <memcpy>
    3cb4:	e0888004 	add	r8, r8, r4
    3cb8:	e0675005 	rsb	r5, r7, r5
    3cbc:	e3a00000 	mov	r0, #0	; 0x0
    3cc0:	e1550000 	cmp	r5, r0
    3cc4:	1affffda 	bne	3c34 <tt_crypto_aes+0x1e4>
    3cc8:	ea000009 	b	3cf4 <tt_crypto_aes+0x2a4>
    3ccc:	e1a00006 	mov	r0, r6
    3cd0:	eb001302 	bl	88e0 <get_timer>
    3cd4:	e1a04000 	mov	r4, r0
    3cd8:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3cdc:	e1a01000 	mov	r1, r0
    3ce0:	e59d200c 	ldr	r2, [sp, #12]
    3ce4:	eb000541 	bl	51f0 <ss_aes_data>
    3ce8:	e1a00004 	mov	r0, r4
    3cec:	eb0012fb 	bl	88e0 <get_timer>
    3cf0:	e1a08000 	mov	r8, r0
    3cf4:	e59d3004 	ldr	r3, [sp, #4]
    3cf8:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    3cfc:	e1a00543 	asr	r0, r3, #10
    3d00:	e1a0100a 	mov	r1, sl
    3d04:	e0000094 	mul	r0, r4, r0
    3d08:	eb00317e 	bl	10308 <__aeabi_idiv>
    3d0c:	e1a06000 	mov	r6, r0
    3d10:	e59d000c 	ldr	r0, [sp, #12]
    3d14:	e1a01008 	mov	r1, r8
    3d18:	e1a00540 	asr	r0, r0, #10
    3d1c:	e0000094 	mul	r0, r4, r0
    3d20:	eb003178 	bl	10308 <__aeabi_idiv>
    3d24:	e59f408c 	ldr	r4, [pc, #140]	; 3db8 <tt_crypto_aes+0x368>
    3d28:	e1a05000 	mov	r5, r0
    3d2c:	e59f00ac 	ldr	r0, [pc, #172]	; 3de0 <tt_crypto_aes+0x390>
    3d30:	e1a0e00f 	mov	lr, pc
    3d34:	e594f000 	ldr	pc, [r4]
    3d38:	e1a0000b 	mov	r0, fp
    3d3c:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    3d40:	e59d200c 	ldr	r2, [sp, #12]
    3d44:	e3a0300c 	mov	r3, #12	; 0xc
    3d48:	eb00014f 	bl	428c <tt_cmp_buffer>
    3d4c:	e3500000 	cmp	r0, #0	; 0x0
    3d50:	0a000006 	beq	3d70 <tt_crypto_aes+0x320>
    3d54:	e1a03000 	mov	r3, r0
    3d58:	e1a01006 	mov	r1, r6
    3d5c:	e1a02005 	mov	r2, r5
    3d60:	e59f007c 	ldr	r0, [pc, #124]	; 3de4 <tt_crypto_aes+0x394>
    3d64:	e1a0e00f 	mov	lr, pc
    3d68:	e594f000 	ldr	pc, [r4]
    3d6c:	ea000004 	b	3d84 <tt_crypto_aes+0x334>
    3d70:	e1a01006 	mov	r1, r6
    3d74:	e1a02005 	mov	r2, r5
    3d78:	e59f0068 	ldr	r0, [pc, #104]	; 3de8 <tt_crypto_aes+0x398>
    3d7c:	e1a0e00f 	mov	lr, pc
    3d80:	e594f000 	ldr	pc, [r4]
    3d84:	e59f0060 	ldr	r0, [pc, #96]	; 3dec <tt_crypto_aes+0x39c>
    3d88:	e59d100c 	ldr	r1, [sp, #12]
    3d8c:	e890000c 	ldm	r0, {r2, r3}
    3d90:	e0922001 	adds	r2, r2, r1
    3d94:	e0a33fc1 	adc	r3, r3, r1, asr #31
    3d98:	e880000c 	stm	r0, {r2, r3}
    3d9c:	e59f0030 	ldr	r0, [pc, #48]	; 3dd4 <tt_crypto_aes+0x384>
    3da0:	e59f3010 	ldr	r3, [pc, #16]	; 3db8 <tt_crypto_aes+0x368>
    3da4:	e1a0e00f 	mov	lr, pc
    3da8:	e593f000 	ldr	pc, [r3]
    3dac:	e3a00000 	mov	r0, #0	; 0x0
    3db0:	e28dd014 	add	sp, sp, #20	; 0x14
    3db4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    3db8:	0800d76c 	.word	0x0800d76c
    3dbc:	00012912 	.word	0x00012912
    3dc0:	00012935 	.word	0x00012935
    3dc4:	0001237d 	.word	0x0001237d
    3dc8:	00013f5e 	.word	0x00013f5e
    3dcc:	0800d10c 	.word	0x0800d10c
    3dd0:	00013ff9 	.word	0x00013ff9
    3dd4:	00012472 	.word	0x00012472
    3dd8:	00012961 	.word	0x00012961
    3ddc:	00013224 	.word	0x00013224
    3de0:	000126c3 	.word	0x000126c3
    3de4:	00012982 	.word	0x00012982
    3de8:	000129c0 	.word	0x000129c0
    3dec:	0800d1c8 	.word	0x0800d1c8

00003df0 <do_tt_crypto>:
    3df0:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    3df4:	e3520001 	cmp	r2, #1	; 0x1
    3df8:	e24dd004 	sub	sp, sp, #4	; 0x4
    3dfc:	d3a07000 	movle	r7, #0	; 0x0
    3e00:	da000009 	ble	3e2c <do_tt_crypto+0x3c>
    3e04:	e5930004 	ldr	r0, [r3, #4]
    3e08:	e3a01000 	mov	r1, #0	; 0x0
    3e0c:	e3a02010 	mov	r2, #16	; 0x10
    3e10:	eb0024b6 	bl	d0f0 <simple_strtoul>
    3e14:	e1a07000 	mov	r7, r0
    3e18:	e1a01007 	mov	r1, r7
    3e1c:	e59f0114 	ldr	r0, [pc, #276]	; 3f38 <do_tt_crypto+0x148>
    3e20:	e59f3114 	ldr	r3, [pc, #276]	; 3f3c <do_tt_crypto+0x14c>
    3e24:	e1a0e00f 	mov	lr, pc
    3e28:	e593f000 	ldr	pc, [r3]
    3e2c:	e3a00000 	mov	r0, #0	; 0x0
    3e30:	eb0012aa 	bl	88e0 <get_timer>
    3e34:	e59f1104 	ldr	r1, [pc, #260]	; 3f40 <do_tt_crypto+0x150>
    3e38:	e3a02000 	mov	r2, #0	; 0x0
    3e3c:	e3a03000 	mov	r3, #0	; 0x0
    3e40:	e881000c 	stm	r1, {r2, r3}
    3e44:	e59f10f8 	ldr	r1, [pc, #248]	; 3f44 <do_tt_crypto+0x154>
    3e48:	e59fa0ec 	ldr	sl, [pc, #236]	; 3f3c <do_tt_crypto+0x14c>
    3e4c:	e881000c 	stm	r1, {r2, r3}
    3e50:	e1a08000 	mov	r8, r0
    3e54:	e3a06000 	mov	r6, #0	; 0x0
    3e58:	e3a01006 	mov	r1, #6	; 0x6
    3e5c:	e3a00000 	mov	r0, #0	; 0x0
    3e60:	eb00015d 	bl	43dc <tt_rnd_get_int>
    3e64:	e3a01002 	mov	r1, #2	; 0x2
    3e68:	e1a04000 	mov	r4, r0
    3e6c:	e3a00000 	mov	r0, #0	; 0x0
    3e70:	eb000159 	bl	43dc <tt_rnd_get_int>
    3e74:	e1a01006 	mov	r1, r6
    3e78:	e1a03000 	mov	r3, r0
    3e7c:	e1a05000 	mov	r5, r0
    3e80:	e1a02004 	mov	r2, r4
    3e84:	e59f00bc 	ldr	r0, [pc, #188]	; 3f48 <do_tt_crypto+0x158>
    3e88:	e1a0e00f 	mov	lr, pc
    3e8c:	e59af000 	ldr	pc, [sl]
    3e90:	e3540003 	cmp	r4, #3	; 0x3
    3e94:	9a000004 	bls	3eac <do_tt_crypto+0xbc>
    3e98:	e3550000 	cmp	r5, #0	; 0x0
    3e9c:	11a00007 	movne	r0, r7
    3ea0:	03a00000 	moveq	r0, #0	; 0x0
    3ea4:	ebfffee9 	bl	3a50 <tt_crypto_aes>
    3ea8:	ea000003 	b	3ebc <do_tt_crypto+0xcc>
    3eac:	e3550000 	cmp	r5, #0	; 0x0
    3eb0:	11a00007 	movne	r0, r7
    3eb4:	03a00000 	moveq	r0, #0	; 0x0
    3eb8:	ebfffe4a 	bl	37e8 <tt_crypto_hash>
    3ebc:	eb0028a2 	bl	e14c <tstc>
    3ec0:	e3500000 	cmp	r0, #0	; 0x0
    3ec4:	e2866001 	add	r6, r6, #1	; 0x1
    3ec8:	0affffe2 	beq	3e58 <do_tt_crypto+0x68>
    3ecc:	eb00289f 	bl	e150 <getc>
    3ed0:	e3500003 	cmp	r0, #3	; 0x3
    3ed4:	1affffdf 	bne	3e58 <do_tt_crypto+0x68>
    3ed8:	e1a00008 	mov	r0, r8
    3edc:	eb00127f 	bl	88e0 <get_timer>
    3ee0:	e59f305c 	ldr	r3, [pc, #92]	; 3f44 <do_tt_crypto+0x154>
    3ee4:	e59f2054 	ldr	r2, [pc, #84]	; 3f40 <do_tt_crypto+0x150>
    3ee8:	e5934000 	ldr	r4, [r3]
    3eec:	e5925000 	ldr	r5, [r2]
    3ef0:	e5933004 	ldr	r3, [r3, #4]
    3ef4:	e5922004 	ldr	r2, [r2, #4]
    3ef8:	e30e1a60 	movw	r1, #60000	; 0xea60
    3efc:	e1a04a24 	lsr	r4, r4, #20
    3f00:	e1a05a25 	lsr	r5, r5, #20
    3f04:	e1844603 	orr	r4, r4, r3, lsl #12
    3f08:	e1855602 	orr	r5, r5, r2, lsl #12
    3f0c:	eb00307b 	bl	10100 <__aeabi_uidiv>
    3f10:	e1a01004 	mov	r1, r4
    3f14:	e1a03000 	mov	r3, r0
    3f18:	e1a02005 	mov	r2, r5
    3f1c:	e59f0028 	ldr	r0, [pc, #40]	; 3f4c <do_tt_crypto+0x15c>
    3f20:	e59fc014 	ldr	ip, [pc, #20]	; 3f3c <do_tt_crypto+0x14c>
    3f24:	e1a0e00f 	mov	lr, pc
    3f28:	e59cf000 	ldr	pc, [ip]
    3f2c:	e3a00000 	mov	r0, #0	; 0x0
    3f30:	e28dd004 	add	sp, sp, #4	; 0x4
    3f34:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    3f38:	000129f1 	.word	0x000129f1
    3f3c:	0800d76c 	.word	0x0800d76c
    3f40:	0800d1d0 	.word	0x0800d1d0
    3f44:	0800d1c8 	.word	0x0800d1c8
    3f48:	00012a17 	.word	0x00012a17
    3f4c:	00012a36 	.word	0x00012a36

00003f50 <tt_ff_trolling>:
    3f50:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3f54:	e3a01001 	mov	r1, #1	; 0x1
    3f58:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    3f5c:	e58d200c 	str	r2, [sp, #12]
    3f60:	e58d3008 	str	r3, [sp, #8]
    3f64:	e1a05000 	mov	r5, r0
    3f68:	eb00046e 	bl	5128 <vs_assign_by_name>
    3f6c:	e2506000 	subs	r6, r0, #0	; 0x0
    3f70:	0a000005 	beq	3f8c <tt_ff_trolling+0x3c>
    3f74:	e1a01005 	mov	r1, r5
    3f78:	e59f0258 	ldr	r0, [pc, #600]	; 41d8 <tt_ff_trolling+0x288>
    3f7c:	e59f3258 	ldr	r3, [pc, #600]	; 41dc <tt_ff_trolling+0x28c>
    3f80:	e1a0e00f 	mov	lr, pc
    3f84:	e593f000 	ldr	pc, [r3]
    3f88:	ea00000c 	b	3fc0 <tt_ff_trolling+0x70>
    3f8c:	e3a0000e 	mov	r0, #14	; 0xe
    3f90:	eb0002f1 	bl	4b5c <vs_align>
    3f94:	e1a01000 	mov	r1, r0
    3f98:	e28d0018 	add	r0, sp, #24	; 0x18
    3f9c:	e5201004 	str	r1, [r0, #-4]!
    3fa0:	e1a02001 	mov	r2, r1
    3fa4:	eb000135 	bl	4480 <tt_rnd_get_data>
    3fa8:	e250a000 	subs	sl, r0, #0	; 0x0
    3fac:	e59f4228 	ldr	r4, [pc, #552]	; 41dc <tt_ff_trolling+0x28c>
    3fb0:	1a000004 	bne	3fc8 <tt_ff_trolling+0x78>
    3fb4:	e59f0224 	ldr	r0, [pc, #548]	; 41e0 <tt_ff_trolling+0x290>
    3fb8:	e1a0e00f 	mov	lr, pc
    3fbc:	e594f000 	ldr	pc, [r4]
    3fc0:	e3e00000 	mvn	r0, #0	; 0x0
    3fc4:	ea000073 	b	4198 <tt_ff_trolling+0x248>
    3fc8:	e59d2014 	ldr	r2, [sp, #20]
    3fcc:	e1a0100a 	mov	r1, sl
    3fd0:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    3fd4:	ebfff4da 	bl	1344 <memcpy>
    3fd8:	e3a01501 	mov	r1, #4194304	; 0x400000
    3fdc:	e59f0200 	ldr	r0, [pc, #512]	; 41e4 <tt_ff_trolling+0x294>
    3fe0:	e1a0e00f 	mov	lr, pc
    3fe4:	e594f000 	ldr	pc, [r4]
    3fe8:	e1a00005 	mov	r0, r5
    3fec:	eb00035b 	bl	4d60 <vs_device_id>
    3ff0:	e3a04000 	mov	r4, #0	; 0x0
    3ff4:	e3a05000 	mov	r5, #0	; 0x0
    3ff8:	e1a0b006 	mov	fp, r6
    3ffc:	e3a08501 	mov	r8, #4194304	; 0x400000
    4000:	e3a09000 	mov	r9, #0	; 0x0
    4004:	e1a07000 	mov	r7, r0
    4008:	ea000021 	b	4094 <tt_ff_trolling+0x144>
    400c:	eb00033f 	bl	4d10 <vs_device_erasable>
    4010:	e3500000 	cmp	r0, #0	; 0x0
    4014:	0a000003 	beq	4028 <tt_ff_trolling+0xd8>
    4018:	e1a00004 	mov	r0, r4
    401c:	e1a01005 	mov	r1, r5
    4020:	e3a02501 	mov	r2, #4194304	; 0x400000
    4024:	eb000290 	bl	4a6c <vs_erase>
    4028:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
    402c:	e5816000 	str	r6, [r1]
    4030:	e1a00001 	mov	r0, r1
    4034:	e59d1014 	ldr	r1, [sp, #20]
    4038:	e1a02004 	mov	r2, r4
    403c:	e1a03005 	mov	r3, r5
    4040:	e88d0802 	stm	sp, {r1, fp}
    4044:	eb000272 	bl	4a14 <vs_write>
    4048:	e3500000 	cmp	r0, #0	; 0x0
    404c:	aa000006 	bge	406c <tt_ff_trolling+0x11c>
    4050:	e1a02004 	mov	r2, r4
    4054:	e1a03005 	mov	r3, r5
    4058:	e59f0188 	ldr	r0, [pc, #392]	; 41e8 <tt_ff_trolling+0x298>
    405c:	e59f1178 	ldr	r1, [pc, #376]	; 41dc <tt_ff_trolling+0x28c>
    4060:	e1a0e00f 	mov	lr, pc
    4064:	e591f000 	ldr	pc, [r1]
    4068:	ea000012 	b	40b8 <tt_ff_trolling+0x168>
    406c:	e316001f 	tst	r6, #31	; 0x1f
    4070:	1a000004 	bne	4088 <tt_ff_trolling+0x138>
    4074:	e1a01106 	lsl	r1, r6, #2
    4078:	e59f016c 	ldr	r0, [pc, #364]	; 41ec <tt_ff_trolling+0x29c>
    407c:	e59f3158 	ldr	r3, [pc, #344]	; 41dc <tt_ff_trolling+0x28c>
    4080:	e1a0e00f 	mov	lr, pc
    4084:	e593f000 	ldr	pc, [r3]
    4088:	e0944008 	adds	r4, r4, r8
    408c:	e0a55009 	adc	r5, r5, r9
    4090:	e2866001 	add	r6, r6, #1	; 0x1
    4094:	e59d1008 	ldr	r1, [sp, #8]
    4098:	e1a00007 	mov	r0, r7
    409c:	e1510005 	cmp	r1, r5
    40a0:	8affffd9 	bhi	400c <tt_ff_trolling+0xbc>
    40a4:	1a00003d 	bne	41a0 <tt_ff_trolling+0x250>
    40a8:	e59d300c 	ldr	r3, [sp, #12]
    40ac:	e1530004 	cmp	r3, r4
    40b0:	8affffd5 	bhi	400c <tt_ff_trolling+0xbc>
    40b4:	ea000039 	b	41a0 <tt_ff_trolling+0x250>
    40b8:	e59f411c 	ldr	r4, [pc, #284]	; 41dc <tt_ff_trolling+0x28c>
    40bc:	e59f012c 	ldr	r0, [pc, #300]	; 41f0 <tt_ff_trolling+0x2a0>
    40c0:	e3a01501 	mov	r1, #4194304	; 0x400000
    40c4:	e3a06000 	mov	r6, #0	; 0x0
    40c8:	e1a0e00f 	mov	lr, pc
    40cc:	e594f000 	ldr	pc, [r4]
    40d0:	e28a7004 	add	r7, sl, #4	; 0x4
    40d4:	e3a04000 	mov	r4, #0	; 0x0
    40d8:	e3a05000 	mov	r5, #0	; 0x0
    40dc:	e1a0b006 	mov	fp, r6
    40e0:	e3a0a44e 	mov	sl, #1308622848	; 0x4e000000
    40e4:	e3a08501 	mov	r8, #4194304	; 0x400000
    40e8:	e3a09000 	mov	r9, #0	; 0x0
    40ec:	ea00001e 	b	416c <tt_ff_trolling+0x21c>
    40f0:	e59d1014 	ldr	r1, [sp, #20]
    40f4:	e88d0802 	stm	sp, {r1, fp}
    40f8:	eb000345 	bl	4e14 <vs_read>
    40fc:	e316001f 	tst	r6, #31	; 0x1f
    4100:	1a000004 	bne	4118 <tt_ff_trolling+0x1c8>
    4104:	e1a01106 	lsl	r1, r6, #2
    4108:	e59f00dc 	ldr	r0, [pc, #220]	; 41ec <tt_ff_trolling+0x29c>
    410c:	e59f30c8 	ldr	r3, [pc, #200]	; 41dc <tt_ff_trolling+0x28c>
    4110:	e1a0e00f 	mov	lr, pc
    4114:	e593f000 	ldr	pc, [r3]
    4118:	e59a2000 	ldr	r2, [sl]
    411c:	e1a00007 	mov	r0, r7
    4120:	e1520006 	cmp	r2, r6
    4124:	e59f10c8 	ldr	r1, [pc, #200]	; 41f4 <tt_ff_trolling+0x2a4>
    4128:	e3a0300a 	mov	r3, #10	; 0xa
    412c:	e2866001 	add	r6, r6, #1	; 0x1
    4130:	1a000004 	bne	4148 <tt_ff_trolling+0x1f8>
    4134:	e59d2014 	ldr	r2, [sp, #20]
    4138:	e2422004 	sub	r2, r2, #4	; 0x4
    413c:	eb000052 	bl	428c <tt_cmp_buffer>
    4140:	e3500000 	cmp	r0, #0	; 0x0
    4144:	0a000006 	beq	4164 <tt_ff_trolling+0x214>
    4148:	e1a02004 	mov	r2, r4
    414c:	e1a03005 	mov	r3, r5
    4150:	e59f00a0 	ldr	r0, [pc, #160]	; 41f8 <tt_ff_trolling+0x2a8>
    4154:	e59f1080 	ldr	r1, [pc, #128]	; 41dc <tt_ff_trolling+0x28c>
    4158:	e1a0e00f 	mov	lr, pc
    415c:	e591f000 	ldr	pc, [r1]
    4160:	ea00001a 	b	41d0 <tt_ff_trolling+0x280>
    4164:	e0944008 	adds	r4, r4, r8
    4168:	e0a55009 	adc	r5, r5, r9
    416c:	e59d1008 	ldr	r1, [sp, #8]
    4170:	e1a02004 	mov	r2, r4
    4174:	e1510005 	cmp	r1, r5
    4178:	e1a03005 	mov	r3, r5
    417c:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    4180:	8affffda 	bhi	40f0 <tt_ff_trolling+0x1a0>
    4184:	1a00000a 	bne	41b4 <tt_ff_trolling+0x264>
    4188:	e59d100c 	ldr	r1, [sp, #12]
    418c:	e1510004 	cmp	r1, r4
    4190:	8affffd6 	bhi	40f0 <tt_ff_trolling+0x1a0>
    4194:	ea000006 	b	41b4 <tt_ff_trolling+0x264>
    4198:	e28dd01c 	add	sp, sp, #28	; 0x1c
    419c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    41a0:	e59f0054 	ldr	r0, [pc, #84]	; 41fc <tt_ff_trolling+0x2ac>
    41a4:	e59f3030 	ldr	r3, [pc, #48]	; 41dc <tt_ff_trolling+0x28c>
    41a8:	e1a0e00f 	mov	lr, pc
    41ac:	e593f000 	ldr	pc, [r3]
    41b0:	eaffffc0 	b	40b8 <tt_ff_trolling+0x168>
    41b4:	e59f0040 	ldr	r0, [pc, #64]	; 41fc <tt_ff_trolling+0x2ac>
    41b8:	e59f401c 	ldr	r4, [pc, #28]	; 41dc <tt_ff_trolling+0x28c>
    41bc:	e1a0e00f 	mov	lr, pc
    41c0:	e594f000 	ldr	pc, [r4]
    41c4:	e59f0034 	ldr	r0, [pc, #52]	; 4200 <tt_ff_trolling+0x2b0>
    41c8:	e1a0e00f 	mov	lr, pc
    41cc:	e594f000 	ldr	pc, [r4]
    41d0:	e3a00000 	mov	r0, #0	; 0x0
    41d4:	eaffffef 	b	4198 <tt_ff_trolling+0x248>
    41d8:	00012aba 	.word	0x00012aba
    41dc:	0800d76c 	.word	0x0800d76c
    41e0:	00012ad7 	.word	0x00012ad7
    41e4:	00012af6 	.word	0x00012af6
    41e8:	00012b14 	.word	0x00012b14
    41ec:	00012b2c 	.word	0x00012b2c
    41f0:	00012b33 	.word	0x00012b33
    41f4:	4e000004 	.word	0x4e000004
    41f8:	00012b53 	.word	0x00012b53
    41fc:	00012b63 	.word	0x00012b63
    4200:	00012b6a 	.word	0x00012b6a

00004204 <do_tt_ff>:
    4204:	e92d4030 	push	{r4, r5, lr}
    4208:	e3520001 	cmp	r2, #1	; 0x1
    420c:	e24dd004 	sub	sp, sp, #4	; 0x4
    4210:	e1a05003 	mov	r5, r3
    4214:	ca000005 	bgt	4230 <do_tt_ff+0x2c>
    4218:	e59f0060 	ldr	r0, [pc, #96]	; 4280 <do_tt_ff+0x7c>
    421c:	e59f3060 	ldr	r3, [pc, #96]	; 4284 <do_tt_ff+0x80>
    4220:	e1a0e00f 	mov	lr, pc
    4224:	e593f000 	ldr	pc, [r3]
    4228:	e3e00000 	mvn	r0, #0	; 0x0
    422c:	ea000011 	b	4278 <do_tt_ff+0x74>
    4230:	e3520002 	cmp	r2, #2	; 0x2
    4234:	03a04000 	moveq	r4, #0	; 0x0
    4238:	0a000009 	beq	4264 <do_tt_ff+0x60>
    423c:	e3a01000 	mov	r1, #0	; 0x0
    4240:	e5930008 	ldr	r0, [r3, #8]
    4244:	e3a0200a 	mov	r2, #10	; 0xa
    4248:	eb0023a8 	bl	d0f0 <simple_strtoul>
    424c:	e1a04000 	mov	r4, r0
    4250:	e1a01004 	mov	r1, r4
    4254:	e59f002c 	ldr	r0, [pc, #44]	; 4288 <do_tt_ff+0x84>
    4258:	e59f3024 	ldr	r3, [pc, #36]	; 4284 <do_tt_ff+0x80>
    425c:	e1a0e00f 	mov	lr, pc
    4260:	e593f000 	ldr	pc, [r3]
    4264:	e5950004 	ldr	r0, [r5, #4]
    4268:	e1a03624 	lsr	r3, r4, #12
    426c:	e1a02a04 	lsl	r2, r4, #20
    4270:	ebffff36 	bl	3f50 <tt_ff_trolling>
    4274:	e3a00000 	mov	r0, #0	; 0x0
    4278:	e28dd004 	add	sp, sp, #4	; 0x4
    427c:	e8bd8030 	pop	{r4, r5, pc}
    4280:	00012b82 	.word	0x00012b82
    4284:	0800d76c 	.word	0x0800d76c
    4288:	00012bc8 	.word	0x00012bc8

0000428c <tt_cmp_buffer>:
    428c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4290:	e3a04000 	mov	r4, #0	; 0x0
    4294:	e24dd004 	sub	sp, sp, #4	; 0x4
    4298:	e1a0a002 	mov	sl, r2
    429c:	e1a07000 	mov	r7, r0
    42a0:	e1a06001 	mov	r6, r1
    42a4:	e1a08003 	mov	r8, r3
    42a8:	e1a09142 	asr	r9, r2, #2
    42ac:	e1a05004 	mov	r5, r4
    42b0:	e59fb0e4 	ldr	fp, [pc, #228]	; 439c <tt_cmp_buffer+0x110>
    42b4:	ea00001f 	b	4338 <tt_cmp_buffer+0xac>
    42b8:	e5970000 	ldr	r0, [r7]
    42bc:	e5961000 	ldr	r1, [r6]
    42c0:	e1500001 	cmp	r0, r1
    42c4:	0a000018 	beq	432c <tt_cmp_buffer+0xa0>
    42c8:	e5d72000 	ldrb	r2, [r7]
    42cc:	e5d63000 	ldrb	r3, [r6]
    42d0:	e1520003 	cmp	r2, r3
    42d4:	e5d72001 	ldrb	r2, [r7, #1]
    42d8:	e5d63001 	ldrb	r3, [r6, #1]
    42dc:	12855001 	addne	r5, r5, #1	; 0x1
    42e0:	e1520003 	cmp	r2, r3
    42e4:	e5d72002 	ldrb	r2, [r7, #2]
    42e8:	e5d63002 	ldrb	r3, [r6, #2]
    42ec:	12855001 	addne	r5, r5, #1	; 0x1
    42f0:	e1520003 	cmp	r2, r3
    42f4:	e5d72003 	ldrb	r2, [r7, #3]
    42f8:	e5d63003 	ldrb	r3, [r6, #3]
    42fc:	12855001 	addne	r5, r5, #1	; 0x1
    4300:	e1520003 	cmp	r2, r3
    4304:	12855001 	addne	r5, r5, #1	; 0x1
    4308:	e3580000 	cmp	r8, #0	; 0x0
    430c:	0a000006 	beq	432c <tt_cmp_buffer+0xa0>
    4310:	e1a02000 	mov	r2, r0
    4314:	e1a03001 	mov	r3, r1
    4318:	e59f0080 	ldr	r0, [pc, #128]	; 43a0 <tt_cmp_buffer+0x114>
    431c:	e1a01104 	lsl	r1, r4, #2
    4320:	e1a0e00f 	mov	lr, pc
    4324:	e59bf000 	ldr	pc, [fp]
    4328:	e2488001 	sub	r8, r8, #1	; 0x1
    432c:	e2844001 	add	r4, r4, #1	; 0x1
    4330:	e2877004 	add	r7, r7, #4	; 0x4
    4334:	e2866004 	add	r6, r6, #4	; 0x4
    4338:	e1540009 	cmp	r4, r9
    433c:	baffffdd 	blt	42b8 <tt_cmp_buffer+0x2c>
    4340:	e3ca9003 	bic	r9, sl, #3	; 0x3
    4344:	e3a04000 	mov	r4, #0	; 0x0
    4348:	e20aa003 	and	sl, sl, #3	; 0x3
    434c:	e59fb048 	ldr	fp, [pc, #72]	; 439c <tt_cmp_buffer+0x110>
    4350:	ea00000c 	b	4388 <tt_cmp_buffer+0xfc>
    4354:	e7d72004 	ldrb	r2, [r7, r4]
    4358:	e7d63004 	ldrb	r3, [r6, r4]
    435c:	e1520003 	cmp	r2, r3
    4360:	0a000007 	beq	4384 <tt_cmp_buffer+0xf8>
    4364:	e3580000 	cmp	r8, #0	; 0x0
    4368:	e2855001 	add	r5, r5, #1	; 0x1
    436c:	0a000004 	beq	4384 <tt_cmp_buffer+0xf8>
    4370:	e0841009 	add	r1, r4, r9
    4374:	e59f0028 	ldr	r0, [pc, #40]	; 43a4 <tt_cmp_buffer+0x118>
    4378:	e1a0e00f 	mov	lr, pc
    437c:	e59bf000 	ldr	pc, [fp]
    4380:	e2488001 	sub	r8, r8, #1	; 0x1
    4384:	e2844001 	add	r4, r4, #1	; 0x1
    4388:	e154000a 	cmp	r4, sl
    438c:	bafffff0 	blt	4354 <tt_cmp_buffer+0xc8>
    4390:	e1a00005 	mov	r0, r5
    4394:	e28dd004 	add	sp, sp, #4	; 0x4
    4398:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    439c:	0800d76c 	.word	0x0800d76c
    43a0:	00012bef 	.word	0x00012bef
    43a4:	00012c1a 	.word	0x00012c1a

000043a8 <tt_rnd_seed>:
    43a8:	e92d4030 	push	{r4, r5, lr}
    43ac:	e59f5024 	ldr	r5, [pc, #36]	; 43d8 <tt_rnd_seed+0x30>
    43b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    43b4:	e3a00000 	mov	r0, #0	; 0x0
    43b8:	e5954000 	ldr	r4, [r5]
    43bc:	eb001147 	bl	88e0 <get_timer>
    43c0:	e30f3ffc 	movw	r3, #65532	; 0xfffc
    43c4:	e6e40070 	uxtab	r0, r4, r0
    43c8:	e0003003 	and	r3, r0, r3
    43cc:	e5853000 	str	r3, [r5]
    43d0:	e28dd004 	add	sp, sp, #4	; 0x4
    43d4:	e8bd8030 	pop	{r4, r5, pc}
    43d8:	0800d1d8 	.word	0x0800d1d8

000043dc <tt_rnd_get_int>:
    43dc:	e59fc094 	ldr	ip, [pc, #148]	; 4478 <tt_rnd_get_int+0x9c>
    43e0:	e3a02101 	mov	r2, #1073741824	; 0x40000000
    43e4:	e59c3000 	ldr	r3, [ip]
    43e8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    43ec:	e19330b2 	ldrh	r3, [r3, r2]
    43f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    43f4:	e3c33003 	bic	r3, r3, #3	; 0x3
    43f8:	e6ff3073 	uxth	r3, r3
    43fc:	e19340b2 	ldrh	r4, [r3, r2]
    4400:	e58c3000 	str	r3, [ip]
    4404:	e1a07000 	mov	r7, r0
    4408:	e1a06001 	mov	r6, r1
    440c:	ebffffe5 	bl	43a8 <tt_rnd_seed>
    4410:	e1a00004 	mov	r0, r4
    4414:	eb003105 	bl	10830 <__aeabi_ui2d>
    4418:	e3a02000 	mov	r2, #0	; 0x0
    441c:	e59f3058 	ldr	r3, [pc, #88]	; 447c <tt_rnd_get_int+0xa0>
    4420:	eb0031dd 	bl	10b9c <__aeabi_ddiv>
    4424:	e1a04000 	mov	r4, r0
    4428:	e0670006 	rsb	r0, r7, r6
    442c:	e1a05001 	mov	r5, r1
    4430:	eb003107 	bl	10854 <__aeabi_i2d>
    4434:	e1a02000 	mov	r2, r0
    4438:	e1a03001 	mov	r3, r1
    443c:	e1a00004 	mov	r0, r4
    4440:	e1a01005 	mov	r1, r5
    4444:	eb003139 	bl	10930 <__aeabi_dmul>
    4448:	e1a04000 	mov	r4, r0
    444c:	e1a00007 	mov	r0, r7
    4450:	e1a05001 	mov	r5, r1
    4454:	eb0030fe 	bl	10854 <__aeabi_i2d>
    4458:	e1a02000 	mov	r2, r0
    445c:	e1a03001 	mov	r3, r1
    4460:	e1a00004 	mov	r0, r4
    4464:	e1a01005 	mov	r1, r5
    4468:	eb003045 	bl	10584 <__adddf3>
    446c:	eb00324b 	bl	10da0 <__aeabi_d2iz>
    4470:	e28dd004 	add	sp, sp, #4	; 0x4
    4474:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4478:	0800d1d8 	.word	0x0800d1d8
    447c:	40efffe0 	.word	0x40efffe0

00004480 <tt_rnd_get_data>:
    4480:	e92d4010 	push	{r4, lr}
    4484:	e1a04000 	mov	r4, r0
    4488:	e1a00001 	mov	r0, r1
    448c:	e1a01002 	mov	r1, r2
    4490:	ebffffd1 	bl	43dc <tt_rnd_get_int>
    4494:	e59f3010 	ldr	r3, [pc, #16]	; 44ac <tt_rnd_get_data+0x2c>
    4498:	e5840000 	str	r0, [r4]
    449c:	e5930000 	ldr	r0, [r3]
    44a0:	e3c00003 	bic	r0, r0, #3	; 0x3
    44a4:	e2800101 	add	r0, r0, #1073741824	; 0x40000000
    44a8:	e8bd8010 	pop	{r4, pc}
    44ac:	0800d1d8 	.word	0x0800d1d8

000044b0 <tt_wire_get_opt>:
    44b0:	e5900008 	ldr	r0, [r0, #8]
    44b4:	e12fff1e 	bx	lr

000044b8 <tt_wire_get_len>:
    44b8:	e590000c 	ldr	r0, [r0, #12]
    44bc:	e12fff1e 	bx	lr

000044c0 <tt_wire_make_data>:
    44c0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    44c4:	e24dd004 	sub	sp, sp, #4	; 0x4
    44c8:	e1a04000 	mov	r4, r0
    44cc:	e1a07002 	mov	r7, r2
    44d0:	e1a06003 	mov	r6, r3
    44d4:	ebfff39a 	bl	1344 <memcpy>
    44d8:	e59f3040 	ldr	r3, [pc, #64]	; 4520 <tt_wire_make_data+0x60>
    44dc:	e5846008 	str	r6, [r4, #8]
    44e0:	e5843000 	str	r3, [r4]
    44e4:	e59d3018 	ldr	r3, [sp, #24]
    44e8:	e3a05000 	mov	r5, #0	; 0x0
    44ec:	e584300c 	str	r3, [r4, #12]
    44f0:	e1a01007 	mov	r1, r7
    44f4:	e5845004 	str	r5, [r4, #4]
    44f8:	e1a00005 	mov	r0, r5
    44fc:	eb00046a 	bl	56ac <hash_init>
    4500:	e1a01007 	mov	r1, r7
    4504:	e1a00004 	mov	r0, r4
    4508:	eb000442 	bl	5618 <hash_data>
    450c:	e2840004 	add	r0, r4, #4	; 0x4
    4510:	eb000424 	bl	55a8 <hash_value>
    4514:	e1a00005 	mov	r0, r5
    4518:	e28dd004 	add	sp, sp, #4	; 0x4
    451c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4520:	7766dd80 	.word	0x7766dd80

00004524 <tt_wire_check_valid>:
    4524:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4528:	e5903000 	ldr	r3, [r0]
    452c:	e59f2084 	ldr	r2, [pc, #132]	; 45b8 <tt_wire_check_valid+0x94>
    4530:	e24dd00c 	sub	sp, sp, #12	; 0xc
    4534:	e1530002 	cmp	r3, r2
    4538:	e1a04000 	mov	r4, r0
    453c:	e1a06001 	mov	r6, r1
    4540:	e5907004 	ldr	r7, [r0, #4]
    4544:	0a000006 	beq	4564 <tt_wire_check_valid+0x40>
    4548:	e1a01003 	mov	r1, r3
    454c:	e59f0068 	ldr	r0, [pc, #104]	; 45bc <tt_wire_check_valid+0x98>
    4550:	e59f3068 	ldr	r3, [pc, #104]	; 45c0 <tt_wire_check_valid+0x9c>
    4554:	e1a0e00f 	mov	lr, pc
    4558:	e593f000 	ldr	pc, [r3]
    455c:	e3a00000 	mov	r0, #0	; 0x0
    4560:	ea000012 	b	45b0 <tt_wire_check_valid+0x8c>
    4564:	e3a05000 	mov	r5, #0	; 0x0
    4568:	e5805004 	str	r5, [r0, #4]
    456c:	e1a00005 	mov	r0, r5
    4570:	eb00044d 	bl	56ac <hash_init>
    4574:	e1a01006 	mov	r1, r6
    4578:	e1a00004 	mov	r0, r4
    457c:	eb000425 	bl	5618 <hash_data>
    4580:	e28d0004 	add	r0, sp, #4	; 0x4
    4584:	eb000407 	bl	55a8 <hash_value>
    4588:	e59d1004 	ldr	r1, [sp, #4]
    458c:	e1510007 	cmp	r1, r7
    4590:	03a00001 	moveq	r0, #1	; 0x1
    4594:	0a000005 	beq	45b0 <tt_wire_check_valid+0x8c>
    4598:	e59f0024 	ldr	r0, [pc, #36]	; 45c4 <tt_wire_check_valid+0xa0>
    459c:	e1a02007 	mov	r2, r7
    45a0:	e59f3018 	ldr	r3, [pc, #24]	; 45c0 <tt_wire_check_valid+0x9c>
    45a4:	e1a0e00f 	mov	lr, pc
    45a8:	e593f000 	ldr	pc, [r3]
    45ac:	e1a00005 	mov	r0, r5
    45b0:	e28dd00c 	add	sp, sp, #12	; 0xc
    45b4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    45b8:	7766dd80 	.word	0x7766dd80
    45bc:	00012c45 	.word	0x00012c45
    45c0:	0800d76c 	.word	0x0800d76c
    45c4:	00012c6e 	.word	0x00012c6e

000045c8 <do_tt_wire>:
    45c8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    45cc:	e3520001 	cmp	r2, #1	; 0x1
    45d0:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    45d4:	e1a04003 	mov	r4, r3
    45d8:	d59f032c 	ldrle	r0, [pc, #812]	; 490c <do_tt_wire+0x344>
    45dc:	da00002b 	ble	4690 <do_tt_wire+0xc8>
    45e0:	e3a02003 	mov	r2, #3	; 0x3
    45e4:	e5930004 	ldr	r0, [r3, #4]
    45e8:	e59f1320 	ldr	r1, [pc, #800]	; 4910 <do_tt_wire+0x348>
    45ec:	ebfff31b 	bl	1260 <strncmp>
    45f0:	e3500000 	cmp	r0, #0	; 0x0
    45f4:	059f2318 	ldreq	r2, [pc, #792]	; 4914 <do_tt_wire+0x34c>
    45f8:	0a000006 	beq	4618 <do_tt_wire+0x50>
    45fc:	e5940004 	ldr	r0, [r4, #4]
    4600:	e59f1310 	ldr	r1, [pc, #784]	; 4918 <do_tt_wire+0x350>
    4604:	e3a02003 	mov	r2, #3	; 0x3
    4608:	ebfff314 	bl	1260 <strncmp>
    460c:	e3500000 	cmp	r0, #0	; 0x0
    4610:	1a000009 	bne	463c <do_tt_wire+0x74>
    4614:	e59f2300 	ldr	r2, [pc, #768]	; 491c <do_tt_wire+0x354>
    4618:	e59f3300 	ldr	r3, [pc, #768]	; 4920 <do_tt_wire+0x358>
    461c:	e3a00000 	mov	r0, #0	; 0x0
    4620:	e5832000 	str	r2, [r3]
    4624:	eb0010ad 	bl	88e0 <get_timer>
    4628:	e59f82f4 	ldr	r8, [pc, #756]	; 4924 <do_tt_wire+0x35c>
    462c:	e59f92ec 	ldr	r9, [pc, #748]	; 4920 <do_tt_wire+0x358>
    4630:	e1a0a008 	mov	sl, r8
    4634:	e58d000c 	str	r0, [sp, #12]
    4638:	ea000006 	b	4658 <do_tt_wire+0x90>
    463c:	e5941004 	ldr	r1, [r4, #4]
    4640:	e59f02e0 	ldr	r0, [pc, #736]	; 4928 <do_tt_wire+0x360>
    4644:	e59f32d8 	ldr	r3, [pc, #728]	; 4924 <do_tt_wire+0x35c>
    4648:	e1a0e00f 	mov	lr, pc
    464c:	e593f000 	ldr	pc, [r3]
    4650:	e3e00000 	mvn	r0, #0	; 0x0
    4654:	ea0000aa 	b	4904 <do_tt_wire+0x33c>
    4658:	e59f02cc 	ldr	r0, [pc, #716]	; 492c <do_tt_wire+0x364>
    465c:	eb00107b 	bl	8850 <udelay>
    4660:	e59f02c8 	ldr	r0, [pc, #712]	; 4930 <do_tt_wire+0x368>
    4664:	e1a0e00f 	mov	lr, pc
    4668:	e598f000 	ldr	pc, [r8]
    466c:	e5993000 	ldr	r3, [r9]
    4670:	e1a0e00f 	mov	lr, pc
    4674:	e593f000 	ldr	pc, [r3]
    4678:	e3500000 	cmp	r0, #0	; 0x0
    467c:	a3a07000 	movge	r7, #0	; 0x0
    4680:	a3a06020 	movge	r6, #32	; 0x20
    4684:	a59fb2a8 	ldrge	fp, [pc, #680]	; 4934 <do_tt_wire+0x36c>
    4688:	aa000004 	bge	46a0 <do_tt_wire+0xd8>
    468c:	e59f02a4 	ldr	r0, [pc, #676]	; 4938 <do_tt_wire+0x370>
    4690:	e59f328c 	ldr	r3, [pc, #652]	; 4924 <do_tt_wire+0x35c>
    4694:	e1a0e00f 	mov	lr, pc
    4698:	e593f000 	ldr	pc, [r3]
    469c:	eaffffeb 	b	4650 <do_tt_wire+0x88>
    46a0:	e3570000 	cmp	r7, #0	; 0x0
    46a4:	e1a01006 	mov	r1, r6
    46a8:	e59f028c 	ldr	r0, [pc, #652]	; 493c <do_tt_wire+0x374>
    46ac:	1a00002f 	bne	4770 <do_tt_wire+0x1a8>
    46b0:	e59f0288 	ldr	r0, [pc, #648]	; 4940 <do_tt_wire+0x378>
    46b4:	e1a0e00f 	mov	lr, pc
    46b8:	e598f000 	ldr	pc, [r8]
    46bc:	e1a00007 	mov	r0, r7
    46c0:	eb001086 	bl	88e0 <get_timer>
    46c4:	e1a01006 	mov	r1, r6
    46c8:	e1a04000 	mov	r4, r0
    46cc:	e5993000 	ldr	r3, [r9]
    46d0:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    46d4:	e1a0e00f 	mov	lr, pc
    46d8:	e593f004 	ldr	pc, [r3, #4]
    46dc:	e1a05000 	mov	r5, r0
    46e0:	e1a00004 	mov	r0, r4
    46e4:	eb00107d 	bl	88e0 <get_timer>
    46e8:	e3550000 	cmp	r5, #0	; 0x0
    46ec:	e1a04000 	mov	r4, r0
    46f0:	e1a01006 	mov	r1, r6
    46f4:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    46f8:	ba00003d 	blt	47f4 <do_tt_wire+0x22c>
    46fc:	ebffff88 	bl	4524 <tt_wire_check_valid>
    4700:	e3500000 	cmp	r0, #0	; 0x0
    4704:	e1a01004 	mov	r1, r4
    4708:	e1a03546 	asr	r3, r6, #10
    470c:	1a000007 	bne	4730 <do_tt_wire+0x168>
    4710:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    4714:	e0000390 	mul	r0, r0, r3
    4718:	eb002efa 	bl	10308 <__aeabi_idiv>
    471c:	e1a01000 	mov	r1, r0
    4720:	e59f021c 	ldr	r0, [pc, #540]	; 4944 <do_tt_wire+0x37c>
    4724:	e1a0e00f 	mov	lr, pc
    4728:	e59af000 	ldr	pc, [sl]
    472c:	eaffffc9 	b	4658 <do_tt_wire+0x90>
    4730:	e59fc210 	ldr	ip, [pc, #528]	; 4948 <do_tt_wire+0x380>
    4734:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    4738:	e0000392 	mul	r0, r2, r3
    473c:	e89c000c 	ldm	ip, {r2, r3}
    4740:	e0922006 	adds	r2, r2, r6
    4744:	e0a33fc6 	adc	r3, r3, r6, asr #31
    4748:	e88c000c 	stm	ip, {r2, r3}
    474c:	eb002eed 	bl	10308 <__aeabi_idiv>
    4750:	e1a01000 	mov	r1, r0
    4754:	e59f01f0 	ldr	r0, [pc, #496]	; 494c <do_tt_wire+0x384>
    4758:	e1a0e00f 	mov	lr, pc
    475c:	e598f000 	ldr	pc, [r8]
    4760:	e3a0344e 	mov	r3, #1308622848	; 0x4e000000
    4764:	e5937008 	ldr	r7, [r3, #8]
    4768:	e593600c 	ldr	r6, [r3, #12]
    476c:	ea000044 	b	4884 <do_tt_wire+0x2bc>
    4770:	e3570001 	cmp	r7, #1	; 0x1
    4774:	1a000030 	bne	483c <do_tt_wire+0x274>
    4778:	e1a0e00f 	mov	lr, pc
    477c:	e598f000 	ldr	pc, [r8]
    4780:	e1a01007 	mov	r1, r7
    4784:	e3a00000 	mov	r0, #0	; 0x0
    4788:	ebffff13 	bl	43dc <tt_rnd_get_int>
    478c:	e3a01020 	mov	r1, #32	; 0x20
    4790:	e1a07000 	mov	r7, r0
    4794:	e3a02502 	mov	r2, #8388608	; 0x800000
    4798:	e28d0014 	add	r0, sp, #20	; 0x14
    479c:	ebffff37 	bl	4480 <tt_rnd_get_data>
    47a0:	e59dc014 	ldr	ip, [sp, #20]
    47a4:	e1a01000 	mov	r1, r0
    47a8:	e1a02006 	mov	r2, r6
    47ac:	e1a03007 	mov	r3, r7
    47b0:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    47b4:	e58dc000 	str	ip, [sp]
    47b8:	ebffff40 	bl	44c0 <tt_wire_make_data>
    47bc:	e3a00000 	mov	r0, #0	; 0x0
    47c0:	eb001046 	bl	88e0 <get_timer>
    47c4:	e1a01006 	mov	r1, r6
    47c8:	e1a05000 	mov	r5, r0
    47cc:	e5993000 	ldr	r3, [r9]
    47d0:	e3a0044e 	mov	r0, #1308622848	; 0x4e000000
    47d4:	e1a0e00f 	mov	lr, pc
    47d8:	e593f008 	ldr	pc, [r3, #8]
    47dc:	e1a04000 	mov	r4, r0
    47e0:	e1a00005 	mov	r0, r5
    47e4:	eb00103d 	bl	88e0 <get_timer>
    47e8:	e3540000 	cmp	r4, #0	; 0x0
    47ec:	e1a01000 	mov	r1, r0
    47f0:	aa000003 	bge	4804 <do_tt_wire+0x23c>
    47f4:	e59f0154 	ldr	r0, [pc, #340]	; 4950 <do_tt_wire+0x388>
    47f8:	e1a0e00f 	mov	lr, pc
    47fc:	e59af000 	ldr	pc, [sl]
    4800:	eaffff94 	b	4658 <do_tt_wire+0x90>
    4804:	e89b000c 	ldm	fp, {r2, r3}
    4808:	e0922006 	adds	r2, r2, r6
    480c:	e0a33fc6 	adc	r3, r3, r6, asr #31
    4810:	e59d6014 	ldr	r6, [sp, #20]
    4814:	e88b000c 	stm	fp, {r2, r3}
    4818:	e1a00546 	asr	r0, r6, #10
    481c:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    4820:	e0000092 	mul	r0, r2, r0
    4824:	eb002eb7 	bl	10308 <__aeabi_idiv>
    4828:	e1a01000 	mov	r1, r0
    482c:	e59f0120 	ldr	r0, [pc, #288]	; 4954 <do_tt_wire+0x38c>
    4830:	e1a0e00f 	mov	lr, pc
    4834:	e598f000 	ldr	pc, [r8]
    4838:	ea000011 	b	4884 <do_tt_wire+0x2bc>
    483c:	e3570003 	cmp	r7, #3	; 0x3
    4840:	1a000004 	bne	4858 <do_tt_wire+0x290>
    4844:	e59f010c 	ldr	r0, [pc, #268]	; 4958 <do_tt_wire+0x390>
    4848:	e59f30d4 	ldr	r3, [pc, #212]	; 4924 <do_tt_wire+0x35c>
    484c:	e1a0e00f 	mov	lr, pc
    4850:	e593f000 	ldr	pc, [r3]
    4854:	ea000010 	b	489c <do_tt_wire+0x2d4>
    4858:	e3570002 	cmp	r7, #2	; 0x2
    485c:	1a000003 	bne	4870 <do_tt_wire+0x2a8>
    4860:	e59f00f4 	ldr	r0, [pc, #244]	; 495c <do_tt_wire+0x394>
    4864:	e1a0e00f 	mov	lr, pc
    4868:	e598f000 	ldr	pc, [r8]
    486c:	eaffff79 	b	4658 <do_tt_wire+0x90>
    4870:	e59f00e8 	ldr	r0, [pc, #232]	; 4960 <do_tt_wire+0x398>
    4874:	e59f30a8 	ldr	r3, [pc, #168]	; 4924 <do_tt_wire+0x35c>
    4878:	e1a0e00f 	mov	lr, pc
    487c:	e593f000 	ldr	pc, [r3]
    4880:	eafffffe 	b	4880 <do_tt_wire+0x2b8>
    4884:	eb002630 	bl	e14c <tstc>
    4888:	e3500000 	cmp	r0, #0	; 0x0
    488c:	0affff83 	beq	46a0 <do_tt_wire+0xd8>
    4890:	eb00262e 	bl	e150 <getc>
    4894:	e3500003 	cmp	r0, #3	; 0x3
    4898:	1affff80 	bne	46a0 <do_tt_wire+0xd8>
    489c:	e59d000c 	ldr	r0, [sp, #12]
    48a0:	eb00100e 	bl	88e0 <get_timer>
    48a4:	e59f3088 	ldr	r3, [pc, #136]	; 4934 <do_tt_wire+0x36c>
    48a8:	e59f4074 	ldr	r4, [pc, #116]	; 4924 <do_tt_wire+0x35c>
    48ac:	e893000a 	ldm	r3, {r1, r3}
    48b0:	e1a01a21 	lsr	r1, r1, #20
    48b4:	e1811603 	orr	r1, r1, r3, lsl #12
    48b8:	e1a05000 	mov	r5, r0
    48bc:	e59f00a0 	ldr	r0, [pc, #160]	; 4964 <do_tt_wire+0x39c>
    48c0:	e1a0e00f 	mov	lr, pc
    48c4:	e594f000 	ldr	pc, [r4]
    48c8:	e59f3078 	ldr	r3, [pc, #120]	; 4948 <do_tt_wire+0x380>
    48cc:	e59f0094 	ldr	r0, [pc, #148]	; 4968 <do_tt_wire+0x3a0>
    48d0:	e893000a 	ldm	r3, {r1, r3}
    48d4:	e1a01a21 	lsr	r1, r1, #20
    48d8:	e1811603 	orr	r1, r1, r3, lsl #12
    48dc:	e1a0e00f 	mov	lr, pc
    48e0:	e594f000 	ldr	pc, [r4]
    48e4:	e30e1a60 	movw	r1, #60000	; 0xea60
    48e8:	e1a00005 	mov	r0, r5
    48ec:	eb002e85 	bl	10308 <__aeabi_idiv>
    48f0:	e1a01000 	mov	r1, r0
    48f4:	e59f0070 	ldr	r0, [pc, #112]	; 496c <do_tt_wire+0x3a4>
    48f8:	e1a0e00f 	mov	lr, pc
    48fc:	e594f000 	ldr	pc, [r4]
    4900:	e3a00000 	mov	r0, #0	; 0x0
    4904:	e28dd01c 	add	sp, sp, #28	; 0x1c
    4908:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    490c:	00012c97 	.word	0x00012c97
    4910:	00012ce0 	.word	0x00012ce0
    4914:	00010ecc 	.word	0x00010ecc
    4918:	00012ce4 	.word	0x00012ce4
    491c:	00010ee4 	.word	0x00010ee4
    4920:	0800d1f0 	.word	0x0800d1f0
    4924:	0800d76c 	.word	0x0800d76c
    4928:	00012ce8 	.word	0x00012ce8
    492c:	001e8480 	.word	0x001e8480
    4930:	00012d08 	.word	0x00012d08
    4934:	0800d1e8 	.word	0x0800d1e8
    4938:	00012d2a 	.word	0x00012d2a
    493c:	00012d97 	.word	0x00012d97
    4940:	00012d4d 	.word	0x00012d4d
    4944:	00012d79 	.word	0x00012d79
    4948:	0800d1e0 	.word	0x0800d1e0
    494c:	00012d88 	.word	0x00012d88
    4950:	00012d69 	.word	0x00012d69
    4954:	00012db3 	.word	0x00012db3
    4958:	00012dc2 	.word	0x00012dc2
    495c:	00012dd4 	.word	0x00012dd4
    4960:	00012ded 	.word	0x00012ded
    4964:	00012e1a 	.word	0x00012e1a
    4968:	00012e34 	.word	0x00012e34
    496c:	00012e52 	.word	0x00012e52

00004970 <vs_is_assigned>:
    4970:	e59f300c 	ldr	r3, [pc, #12]	; 4984 <vs_is_assigned+0x14>
    4974:	e5930000 	ldr	r0, [r3]
    4978:	e2500000 	subs	r0, r0, #0	; 0x0
    497c:	13a00001 	movne	r0, #1	; 0x1
    4980:	e12fff1e 	bx	lr
    4984:	0800d1f8 	.word	0x0800d1f8

00004988 <vs_is_opt_available>:
    4988:	e350000d 	cmp	r0, #13	; 0xd
    498c:	959f2078 	ldrls	r2, [pc, #120]	; 4a0c <vs_is_opt_available+0x84>
    4990:	93a0302c 	movls	r3, #44	; 0x2c
    4994:	859f3074 	ldrhi	r3, [pc, #116]	; 4a10 <vs_is_opt_available+0x88>
    4998:	90202093 	mlals	r0, r3, r0, r2
    499c:	85930000 	ldrhi	r0, [r3]
    49a0:	e3500000 	cmp	r0, #0	; 0x0
    49a4:	0a000016 	beq	4a04 <vs_is_opt_available+0x7c>
    49a8:	e3510005 	cmp	r1, #5	; 0x5
    49ac:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    49b0:	ea000013 	b	4a04 <vs_is_opt_available+0x7c>
    49b4:	000049cc 	.word	0x000049cc
    49b8:	000049d4 	.word	0x000049d4
    49bc:	000049dc 	.word	0x000049dc
    49c0:	000049e4 	.word	0x000049e4
    49c4:	000049ec 	.word	0x000049ec
    49c8:	000049f4 	.word	0x000049f4
    49cc:	e590300c 	ldr	r3, [r0, #12]
    49d0:	ea000008 	b	49f8 <vs_is_opt_available+0x70>
    49d4:	e5903010 	ldr	r3, [r0, #16]
    49d8:	ea000006 	b	49f8 <vs_is_opt_available+0x70>
    49dc:	e5903014 	ldr	r3, [r0, #20]
    49e0:	ea000004 	b	49f8 <vs_is_opt_available+0x70>
    49e4:	e5903018 	ldr	r3, [r0, #24]
    49e8:	ea000002 	b	49f8 <vs_is_opt_available+0x70>
    49ec:	e590301c 	ldr	r3, [r0, #28]
    49f0:	ea000000 	b	49f8 <vs_is_opt_available+0x70>
    49f4:	e5903020 	ldr	r3, [r0, #32]
    49f8:	e2530000 	subs	r0, r3, #0	; 0x0
    49fc:	13a00001 	movne	r0, #1	; 0x1
    4a00:	e12fff1e 	bx	lr
    4a04:	e3a00000 	mov	r0, #0	; 0x0
    4a08:	e12fff1e 	bx	lr
    4a0c:	00010ee4 	.word	0x00010ee4
    4a10:	0800d1f8 	.word	0x0800d1f8

00004a14 <vs_write>:
    4a14:	e92d4010 	push	{r4, lr}
    4a18:	e24dd008 	sub	sp, sp, #8	; 0x8
    4a1c:	e59d4010 	ldr	r4, [sp, #16]
    4a20:	e3540000 	cmp	r4, #0	; 0x0
    4a24:	01a00004 	moveq	r0, r4
    4a28:	0a00000c 	beq	4a60 <vs_write+0x4c>
    4a2c:	e59f1034 	ldr	r1, [pc, #52]	; 4a68 <vs_write+0x54>
    4a30:	e5911000 	ldr	r1, [r1]
    4a34:	e3510000 	cmp	r1, #0	; 0x0
    4a38:	03e00001 	mvneq	r0, #1	; 0x1
    4a3c:	0a000007 	beq	4a60 <vs_write+0x4c>
    4a40:	e591c014 	ldr	ip, [r1, #20]
    4a44:	e35c0000 	cmp	ip, #0	; 0x0
    4a48:	03e00002 	mvneq	r0, #2	; 0x2
    4a4c:	0a000003 	beq	4a60 <vs_write+0x4c>
    4a50:	e59d1014 	ldr	r1, [sp, #20]
    4a54:	e58d4000 	str	r4, [sp]
    4a58:	e58d1004 	str	r1, [sp, #4]
    4a5c:	e12fff3c 	blx	ip
    4a60:	e28dd008 	add	sp, sp, #8	; 0x8
    4a64:	e8bd8010 	pop	{r4, pc}
    4a68:	0800d1f8 	.word	0x0800d1f8

00004a6c <vs_erase>:
    4a6c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4a70:	e59f302c 	ldr	r3, [pc, #44]	; 4aa4 <vs_erase+0x38>
    4a74:	e24dd004 	sub	sp, sp, #4	; 0x4
    4a78:	e5933000 	ldr	r3, [r3]
    4a7c:	e3530000 	cmp	r3, #0	; 0x0
    4a80:	03e00001 	mvneq	r0, #1	; 0x1
    4a84:	0a000004 	beq	4a9c <vs_erase+0x30>
    4a88:	e593301c 	ldr	r3, [r3, #28]
    4a8c:	e3530000 	cmp	r3, #0	; 0x0
    4a90:	03e00002 	mvneq	r0, #2	; 0x2
    4a94:	0a000000 	beq	4a9c <vs_erase+0x30>
    4a98:	e12fff33 	blx	r3
    4a9c:	e28dd004 	add	sp, sp, #4	; 0x4
    4aa0:	e8bd8000 	pop	{pc}
    4aa4:	0800d1f8 	.word	0x0800d1f8

00004aa8 <vs_isbad>:
    4aa8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4aac:	e59f302c 	ldr	r3, [pc, #44]	; 4ae0 <vs_isbad+0x38>
    4ab0:	e24dd004 	sub	sp, sp, #4	; 0x4
    4ab4:	e5933000 	ldr	r3, [r3]
    4ab8:	e3530000 	cmp	r3, #0	; 0x0
    4abc:	03e00001 	mvneq	r0, #1	; 0x1
    4ac0:	0a000004 	beq	4ad8 <vs_isbad+0x30>
    4ac4:	e5933024 	ldr	r3, [r3, #36]
    4ac8:	e3530000 	cmp	r3, #0	; 0x0
    4acc:	03e00002 	mvneq	r0, #2	; 0x2
    4ad0:	0a000000 	beq	4ad8 <vs_isbad+0x30>
    4ad4:	e12fff33 	blx	r3
    4ad8:	e28dd004 	add	sp, sp, #4	; 0x4
    4adc:	e8bd8000 	pop	{pc}
    4ae0:	0800d1f8 	.word	0x0800d1f8

00004ae4 <vs_scrub>:
    4ae4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4ae8:	e59f302c 	ldr	r3, [pc, #44]	; 4b1c <vs_scrub+0x38>
    4aec:	e24dd004 	sub	sp, sp, #4	; 0x4
    4af0:	e5933000 	ldr	r3, [r3]
    4af4:	e3530000 	cmp	r3, #0	; 0x0
    4af8:	03e00001 	mvneq	r0, #1	; 0x1
    4afc:	0a000004 	beq	4b14 <vs_scrub+0x30>
    4b00:	e5933020 	ldr	r3, [r3, #32]
    4b04:	e3530000 	cmp	r3, #0	; 0x0
    4b08:	03e00002 	mvneq	r0, #2	; 0x2
    4b0c:	0a000000 	beq	4b14 <vs_scrub+0x30>
    4b10:	e12fff33 	blx	r3
    4b14:	e28dd004 	add	sp, sp, #4	; 0x4
    4b18:	e8bd8000 	pop	{pc}
    4b1c:	0800d1f8 	.word	0x0800d1f8

00004b20 <vs_special>:
    4b20:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4b24:	e59f302c 	ldr	r3, [pc, #44]	; 4b58 <vs_special+0x38>
    4b28:	e24dd004 	sub	sp, sp, #4	; 0x4
    4b2c:	e5933000 	ldr	r3, [r3]
    4b30:	e3530000 	cmp	r3, #0	; 0x0
    4b34:	03e00001 	mvneq	r0, #1	; 0x1
    4b38:	0a000004 	beq	4b50 <vs_special+0x30>
    4b3c:	e5933028 	ldr	r3, [r3, #40]
    4b40:	e3530000 	cmp	r3, #0	; 0x0
    4b44:	03e00002 	mvneq	r0, #2	; 0x2
    4b48:	0a000000 	beq	4b50 <vs_special+0x30>
    4b4c:	e12fff33 	blx	r3
    4b50:	e28dd004 	add	sp, sp, #4	; 0x4
    4b54:	e8bd8000 	pop	{pc}
    4b58:	0800d1f8 	.word	0x0800d1f8

00004b5c <vs_align>:
    4b5c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4b60:	e350000e 	cmp	r0, #14	; 0xe
    4b64:	059f3044 	ldreq	r3, [pc, #68]	; 4bb0 <vs_align+0x54>
    4b68:	e24dd004 	sub	sp, sp, #4	; 0x4
    4b6c:	05930000 	ldreq	r0, [r3]
    4b70:	0a000004 	beq	4b88 <vs_align+0x2c>
    4b74:	e350000d 	cmp	r0, #13	; 0xd
    4b78:	8a000009 	bhi	4ba4 <vs_align+0x48>
    4b7c:	e59f2030 	ldr	r2, [pc, #48]	; 4bb4 <vs_align+0x58>
    4b80:	e3a0302c 	mov	r3, #44	; 0x2c
    4b84:	e0202093 	mla	r0, r3, r0, r2
    4b88:	e3500000 	cmp	r0, #0	; 0x0
    4b8c:	0a000004 	beq	4ba4 <vs_align+0x48>
    4b90:	e590000c 	ldr	r0, [r0, #12]
    4b94:	e3500000 	cmp	r0, #0	; 0x0
    4b98:	0a000001 	beq	4ba4 <vs_align+0x48>
    4b9c:	e12fff30 	blx	r0
    4ba0:	ea000000 	b	4ba8 <vs_align+0x4c>
    4ba4:	e3a00000 	mov	r0, #0	; 0x0
    4ba8:	e28dd004 	add	sp, sp, #4	; 0x4
    4bac:	e8bd8000 	pop	{pc}
    4bb0:	0800d1f8 	.word	0x0800d1f8
    4bb4:	00010ee4 	.word	0x00010ee4

00004bb8 <vs_device_bootable>:
    4bb8:	e350000e 	cmp	r0, #14	; 0xe
    4bbc:	059f3030 	ldreq	r3, [pc, #48]	; 4bf4 <vs_device_bootable+0x3c>
    4bc0:	05930000 	ldreq	r0, [r3]
    4bc4:	0a000004 	beq	4bdc <vs_device_bootable+0x24>
    4bc8:	e350000d 	cmp	r0, #13	; 0xd
    4bcc:	8a000006 	bhi	4bec <vs_device_bootable+0x34>
    4bd0:	e59f2020 	ldr	r2, [pc, #32]	; 4bf8 <vs_device_bootable+0x40>
    4bd4:	e3a0302c 	mov	r3, #44	; 0x2c
    4bd8:	e0202093 	mla	r0, r3, r0, r2
    4bdc:	e3500000 	cmp	r0, #0	; 0x0
    4be0:	15903008 	ldrne	r3, [r0, #8]
    4be4:	12030001 	andne	r0, r3, #1	; 0x1
    4be8:	112fff1e 	bxne	lr
    4bec:	e3a00000 	mov	r0, #0	; 0x0
    4bf0:	e12fff1e 	bx	lr
    4bf4:	0800d1f8 	.word	0x0800d1f8
    4bf8:	00010ee4 	.word	0x00010ee4

00004bfc <vs_device_burnsrc>:
    4bfc:	e350000e 	cmp	r0, #14	; 0xe
    4c00:	059f3034 	ldreq	r3, [pc, #52]	; 4c3c <vs_device_burnsrc+0x40>
    4c04:	05930000 	ldreq	r0, [r3]
    4c08:	0a000004 	beq	4c20 <vs_device_burnsrc+0x24>
    4c0c:	e350000d 	cmp	r0, #13	; 0xd
    4c10:	8a000007 	bhi	4c34 <vs_device_burnsrc+0x38>
    4c14:	e59f2024 	ldr	r2, [pc, #36]	; 4c40 <vs_device_burnsrc+0x44>
    4c18:	e3a0302c 	mov	r3, #44	; 0x2c
    4c1c:	e0202093 	mla	r0, r3, r0, r2
    4c20:	e3500000 	cmp	r0, #0	; 0x0
    4c24:	15903008 	ldrne	r3, [r0, #8]
    4c28:	11a030a3 	lsrne	r3, r3, #1
    4c2c:	12030001 	andne	r0, r3, #1	; 0x1
    4c30:	112fff1e 	bxne	lr
    4c34:	e3a00000 	mov	r0, #0	; 0x0
    4c38:	e12fff1e 	bx	lr
    4c3c:	0800d1f8 	.word	0x0800d1f8
    4c40:	00010ee4 	.word	0x00010ee4

00004c44 <vs_device_burndst>:
    4c44:	e350000e 	cmp	r0, #14	; 0xe
    4c48:	059f3034 	ldreq	r3, [pc, #52]	; 4c84 <vs_device_burndst+0x40>
    4c4c:	05930000 	ldreq	r0, [r3]
    4c50:	0a000004 	beq	4c68 <vs_device_burndst+0x24>
    4c54:	e350000d 	cmp	r0, #13	; 0xd
    4c58:	8a000007 	bhi	4c7c <vs_device_burndst+0x38>
    4c5c:	e59f2024 	ldr	r2, [pc, #36]	; 4c88 <vs_device_burndst+0x44>
    4c60:	e3a0302c 	mov	r3, #44	; 0x2c
    4c64:	e0202093 	mla	r0, r3, r0, r2
    4c68:	e3500000 	cmp	r0, #0	; 0x0
    4c6c:	15903008 	ldrne	r3, [r0, #8]
    4c70:	11a03123 	lsrne	r3, r3, #2
    4c74:	12030001 	andne	r0, r3, #1	; 0x1
    4c78:	112fff1e 	bxne	lr
    4c7c:	e3a00000 	mov	r0, #0	; 0x0
    4c80:	e12fff1e 	bx	lr
    4c84:	0800d1f8 	.word	0x0800d1f8
    4c88:	00010ee4 	.word	0x00010ee4

00004c8c <vs_device_boot0>:
    4c8c:	e350000e 	cmp	r0, #14	; 0xe
    4c90:	059f3034 	ldreq	r3, [pc, #52]	; 4ccc <vs_device_boot0+0x40>
    4c94:	05930000 	ldreq	r0, [r3]
    4c98:	0a000004 	beq	4cb0 <vs_device_boot0+0x24>
    4c9c:	e350000d 	cmp	r0, #13	; 0xd
    4ca0:	8a000007 	bhi	4cc4 <vs_device_boot0+0x38>
    4ca4:	e59f2024 	ldr	r2, [pc, #36]	; 4cd0 <vs_device_boot0+0x44>
    4ca8:	e3a0302c 	mov	r3, #44	; 0x2c
    4cac:	e0202093 	mla	r0, r3, r0, r2
    4cb0:	e3500000 	cmp	r0, #0	; 0x0
    4cb4:	15903008 	ldrne	r3, [r0, #8]
    4cb8:	11a031a3 	lsrne	r3, r3, #3
    4cbc:	12030001 	andne	r0, r3, #1	; 0x1
    4cc0:	112fff1e 	bxne	lr
    4cc4:	e3a00000 	mov	r0, #0	; 0x0
    4cc8:	e12fff1e 	bx	lr
    4ccc:	0800d1f8 	.word	0x0800d1f8
    4cd0:	00010ee4 	.word	0x00010ee4

00004cd4 <vs_device_name>:
    4cd4:	e350000e 	cmp	r0, #14	; 0xe
    4cd8:	059f3028 	ldreq	r3, [pc, #40]	; 4d08 <vs_device_name+0x34>
    4cdc:	05930000 	ldreq	r0, [r3]
    4ce0:	0a000004 	beq	4cf8 <vs_device_name+0x24>
    4ce4:	e350000d 	cmp	r0, #13	; 0xd
    4ce8:	8a000004 	bhi	4d00 <vs_device_name+0x2c>
    4cec:	e59f2018 	ldr	r2, [pc, #24]	; 4d0c <vs_device_name+0x38>
    4cf0:	e3a0302c 	mov	r3, #44	; 0x2c
    4cf4:	e0202093 	mla	r0, r3, r0, r2
    4cf8:	e3500000 	cmp	r0, #0	; 0x0
    4cfc:	112fff1e 	bxne	lr
    4d00:	e3a00000 	mov	r0, #0	; 0x0
    4d04:	e12fff1e 	bx	lr
    4d08:	0800d1f8 	.word	0x0800d1f8
    4d0c:	00010ee4 	.word	0x00010ee4

00004d10 <vs_device_erasable>:
    4d10:	e350000e 	cmp	r0, #14	; 0xe
    4d14:	059f3034 	ldreq	r3, [pc, #52]	; 4d50 <vs_device_erasable+0x40>
    4d18:	05930000 	ldreq	r0, [r3]
    4d1c:	0a000004 	beq	4d34 <vs_device_erasable+0x24>
    4d20:	e350000d 	cmp	r0, #13	; 0xd
    4d24:	8a000007 	bhi	4d48 <vs_device_erasable+0x38>
    4d28:	e59f2024 	ldr	r2, [pc, #36]	; 4d54 <vs_device_erasable+0x44>
    4d2c:	e3a0302c 	mov	r3, #44	; 0x2c
    4d30:	e0202093 	mla	r0, r3, r0, r2
    4d34:	e3500000 	cmp	r0, #0	; 0x0
    4d38:	15903008 	ldrne	r3, [r0, #8]
    4d3c:	11a03223 	lsrne	r3, r3, #4
    4d40:	12030001 	andne	r0, r3, #1	; 0x1
    4d44:	112fff1e 	bxne	lr
    4d48:	e3a00000 	mov	r0, #0	; 0x0
    4d4c:	e12fff1e 	bx	lr
    4d50:	0800d1f8 	.word	0x0800d1f8
    4d54:	00010ee4 	.word	0x00010ee4

00004d58 <vs_device_count>:
    4d58:	e3a0000e 	mov	r0, #14	; 0xe
    4d5c:	e12fff1e 	bx	lr

00004d60 <vs_device_id>:
    4d60:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4d64:	e59f7040 	ldr	r7, [pc, #64]	; 4dac <vs_device_id+0x4c>
    4d68:	e24dd004 	sub	sp, sp, #4	; 0x4
    4d6c:	e1a05000 	mov	r5, r0
    4d70:	e3a04000 	mov	r4, #0	; 0x0
    4d74:	e3a0602c 	mov	r6, #44	; 0x2c
    4d78:	e0217496 	mla	r1, r6, r4, r7
    4d7c:	e1a00005 	mov	r0, r5
    4d80:	e3a02010 	mov	r2, #16	; 0x10
    4d84:	ebfff135 	bl	1260 <strncmp>
    4d88:	e3500000 	cmp	r0, #0	; 0x0
    4d8c:	0a000003 	beq	4da0 <vs_device_id+0x40>
    4d90:	e2844001 	add	r4, r4, #1	; 0x1
    4d94:	e354000e 	cmp	r4, #14	; 0xe
    4d98:	1afffff6 	bne	4d78 <vs_device_id+0x18>
    4d9c:	e2844001 	add	r4, r4, #1	; 0x1
    4da0:	e1a00004 	mov	r0, r4
    4da4:	e28dd004 	add	sp, sp, #4	; 0x4
    4da8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4dac:	00010ee4 	.word	0x00010ee4

00004db0 <vs_is_device>:
    4db0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    4db4:	e350000e 	cmp	r0, #14	; 0xe
    4db8:	059f304c 	ldreq	r3, [pc, #76]	; 4e0c <vs_is_device+0x5c>
    4dbc:	e1a0c001 	mov	ip, r1
    4dc0:	e24dd004 	sub	sp, sp, #4	; 0x4
    4dc4:	05931000 	ldreq	r1, [r3]
    4dc8:	0a000004 	beq	4de0 <vs_is_device+0x30>
    4dcc:	e350000d 	cmp	r0, #13	; 0xd
    4dd0:	8a00000a 	bhi	4e00 <vs_is_device+0x50>
    4dd4:	e59f2034 	ldr	r2, [pc, #52]	; 4e10 <vs_is_device+0x60>
    4dd8:	e3a0302c 	mov	r3, #44	; 0x2c
    4ddc:	e0212093 	mla	r1, r3, r0, r2
    4de0:	e3510000 	cmp	r1, #0	; 0x0
    4de4:	0a000005 	beq	4e00 <vs_is_device+0x50>
    4de8:	e1a0000c 	mov	r0, ip
    4dec:	e3a02010 	mov	r2, #16	; 0x10
    4df0:	ebfff11a 	bl	1260 <strncmp>
    4df4:	e2700001 	rsbs	r0, r0, #1	; 0x1
    4df8:	33a00000 	movcc	r0, #0	; 0x0
    4dfc:	ea000000 	b	4e04 <vs_is_device+0x54>
    4e00:	e3a00000 	mov	r0, #0	; 0x0
    4e04:	e28dd004 	add	sp, sp, #4	; 0x4
    4e08:	e8bd8000 	pop	{pc}
    4e0c:	0800d1f8 	.word	0x0800d1f8
    4e10:	00010ee4 	.word	0x00010ee4

00004e14 <vs_read>:
    4e14:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4e18:	e59f4238 	ldr	r4, [pc, #568]	; 5058 <vs_read+0x244>
    4e1c:	e1a0b000 	mov	fp, r0
    4e20:	e5940000 	ldr	r0, [r4]
    4e24:	e24dd024 	sub	sp, sp, #36	; 0x24
    4e28:	e3500000 	cmp	r0, #0	; 0x0
    4e2c:	e1a06002 	mov	r6, r2
    4e30:	e1a07003 	mov	r7, r3
    4e34:	e59d8048 	ldr	r8, [sp, #72]
    4e38:	03e05001 	mvneq	r5, #1	; 0x1
    4e3c:	0a000082 	beq	504c <vs_read+0x238>
    4e40:	e5903010 	ldr	r3, [r0, #16]
    4e44:	e3530000 	cmp	r3, #0	; 0x0
    4e48:	03e05002 	mvneq	r5, #2	; 0x2
    4e4c:	0a00007e 	beq	504c <vs_read+0x238>
    4e50:	e3580000 	cmp	r8, #0	; 0x0
    4e54:	01a05008 	moveq	r5, r8
    4e58:	0a00007b 	beq	504c <vs_read+0x238>
    4e5c:	e59f11f8 	ldr	r1, [pc, #504]	; 505c <vs_read+0x248>
    4e60:	e3a02003 	mov	r2, #3	; 0x3
    4e64:	ebfff0fd 	bl	1260 <strncmp>
    4e68:	e3500000 	cmp	r0, #0	; 0x0
    4e6c:	1a00000a 	bne	4e9c <vs_read+0x88>
    4e70:	e59d204c 	ldr	r2, [sp, #76]
    4e74:	e58d8000 	str	r8, [sp]
    4e78:	e58d2004 	str	r2, [sp, #4]
    4e7c:	e5941000 	ldr	r1, [r4]
    4e80:	e1a0000b 	mov	r0, fp
    4e84:	e1a02006 	mov	r2, r6
    4e88:	e1a03007 	mov	r3, r7
    4e8c:	e1a0e00f 	mov	lr, pc
    4e90:	e591f010 	ldr	pc, [r1, #16]
    4e94:	e1a05000 	mov	r5, r0
    4e98:	ea00006b 	b	504c <vs_read+0x238>
    4e9c:	e3a0000e 	mov	r0, #14	; 0xe
    4ea0:	ebffff2d 	bl	4b5c <vs_align>
    4ea4:	e2402001 	sub	r2, r0, #1	; 0x1
    4ea8:	e0124006 	ands	r4, r2, r6
    4eac:	10644000 	rsbne	r4, r4, r0
    4eb0:	e0643008 	rsb	r3, r4, r8
    4eb4:	e0032002 	and	r2, r3, r2
    4eb8:	e1929004 	orrs	r9, r2, r4
    4ebc:	e1a0a000 	mov	sl, r0
    4ec0:	e58d3014 	str	r3, [sp, #20]
    4ec4:	e58d200c 	str	r2, [sp, #12]
    4ec8:	e58d9018 	str	r9, [sp, #24]
    4ecc:	01a05009 	moveq	r5, r9
    4ed0:	058d901c 	streq	r9, [sp, #28]
    4ed4:	0a00002d 	beq	4f90 <vs_read+0x17c>
    4ed8:	ebfff17e 	bl	14d8 <malloc>
    4edc:	e2502000 	subs	r2, r0, #0	; 0x0
    4ee0:	e58d201c 	str	r2, [sp, #28]
    4ee4:	1a00000c 	bne	4f1c <vs_read+0x108>
    4ee8:	e59f4170 	ldr	r4, [pc, #368]	; 5060 <vs_read+0x24c>
    4eec:	e59f0170 	ldr	r0, [pc, #368]	; 5064 <vs_read+0x250>
    4ef0:	e3001112 	movw	r1, #274	; 0x112
    4ef4:	e1a0e00f 	mov	lr, pc
    4ef8:	e594f000 	ldr	pc, [r4]
    4efc:	e59f0164 	ldr	r0, [pc, #356]	; 5068 <vs_read+0x254>
    4f00:	e59f1164 	ldr	r1, [pc, #356]	; 506c <vs_read+0x258>
    4f04:	e1a0e00f 	mov	lr, pc
    4f08:	e594f000 	ldr	pc, [r4]
    4f0c:	e59f015c 	ldr	r0, [pc, #348]	; 5070 <vs_read+0x25c>
    4f10:	e1a0e00f 	mov	lr, pc
    4f14:	e594f000 	ldr	pc, [r4]
    4f18:	eafffffe 	b	4f18 <vs_read+0x104>
    4f1c:	e3540000 	cmp	r4, #0	; 0x0
    4f20:	01a05004 	moveq	r5, r4
    4f24:	0a000019 	beq	4f90 <vs_read+0x17c>
    4f28:	e59d804c 	ldr	r8, [sp, #76]
    4f2c:	e59f2124 	ldr	r2, [pc, #292]	; 5058 <vs_read+0x244>
    4f30:	e26a3000 	rsb	r3, sl, #0	; 0x0
    4f34:	e5921000 	ldr	r1, [r2]
    4f38:	e3a09000 	mov	r9, #0	; 0x0
    4f3c:	e58d8004 	str	r8, [sp, #4]
    4f40:	e0068003 	and	r8, r6, r3
    4f44:	e58da000 	str	sl, [sp]
    4f48:	e1a02008 	mov	r2, r8
    4f4c:	e1a03009 	mov	r3, r9
    4f50:	e1a0e00f 	mov	lr, pc
    4f54:	e591f010 	ldr	pc, [r1, #16]
    4f58:	e2505000 	subs	r5, r0, #0	; 0x0
    4f5c:	ba000038 	blt	5044 <vs_read+0x230>
    4f60:	e59d901c 	ldr	r9, [sp, #28]
    4f64:	e064100a 	rsb	r1, r4, sl
    4f68:	e1a0000b 	mov	r0, fp
    4f6c:	e0891001 	add	r1, r9, r1
    4f70:	e1a02004 	mov	r2, r4
    4f74:	ebfff0f2 	bl	1344 <memcpy>
    4f78:	e1550004 	cmp	r5, r4
    4f7c:	21a05004 	movcs	r5, r4
    4f80:	e59d8014 	ldr	r8, [sp, #20]
    4f84:	e0966004 	adds	r6, r6, r4
    4f88:	e2a77000 	adc	r7, r7, #0	; 0x0
    4f8c:	e08bb004 	add	fp, fp, r4
    4f90:	e59d200c 	ldr	r2, [sp, #12]
    4f94:	e0584002 	subs	r4, r8, r2
    4f98:	0a00000f 	beq	4fdc <vs_read+0x1c8>
    4f9c:	e59f30b4 	ldr	r3, [pc, #180]	; 5058 <vs_read+0x244>
    4fa0:	e58d4000 	str	r4, [sp]
    4fa4:	e5931000 	ldr	r1, [r3]
    4fa8:	e59d304c 	ldr	r3, [sp, #76]
    4fac:	e1a0000b 	mov	r0, fp
    4fb0:	e58d3004 	str	r3, [sp, #4]
    4fb4:	e1a02006 	mov	r2, r6
    4fb8:	e1a03007 	mov	r3, r7
    4fbc:	e1a0e00f 	mov	lr, pc
    4fc0:	e591f010 	ldr	pc, [r1, #16]
    4fc4:	e3500000 	cmp	r0, #0	; 0x0
    4fc8:	ba000012 	blt	5018 <vs_read+0x204>
    4fcc:	e0966004 	adds	r6, r6, r4
    4fd0:	e0a77fc4 	adc	r7, r7, r4, asr #31
    4fd4:	e0855000 	add	r5, r5, r0
    4fd8:	e08bb004 	add	fp, fp, r4
    4fdc:	e59d800c 	ldr	r8, [sp, #12]
    4fe0:	e3580000 	cmp	r8, #0	; 0x0
    4fe4:	0a000013 	beq	5038 <vs_read+0x224>
    4fe8:	e59d904c 	ldr	r9, [sp, #76]
    4fec:	e59f3064 	ldr	r3, [pc, #100]	; 5058 <vs_read+0x244>
    4ff0:	e58da000 	str	sl, [sp]
    4ff4:	e58d9004 	str	r9, [sp, #4]
    4ff8:	e5931000 	ldr	r1, [r3]
    4ffc:	e1a02006 	mov	r2, r6
    5000:	e1a03007 	mov	r3, r7
    5004:	e59d001c 	ldr	r0, [sp, #28]
    5008:	e1a0e00f 	mov	lr, pc
    500c:	e591f010 	ldr	pc, [r1, #16]
    5010:	e3500000 	cmp	r0, #0	; 0x0
    5014:	aa000001 	bge	5020 <vs_read+0x20c>
    5018:	e1a05000 	mov	r5, r0
    501c:	ea000005 	b	5038 <vs_read+0x224>
    5020:	e59d200c 	ldr	r2, [sp, #12]
    5024:	e1a0000b 	mov	r0, fp
    5028:	e59d101c 	ldr	r1, [sp, #28]
    502c:	ebfff0c4 	bl	1344 <memcpy>
    5030:	e59d200c 	ldr	r2, [sp, #12]
    5034:	e0855002 	add	r5, r5, r2
    5038:	e59d3018 	ldr	r3, [sp, #24]
    503c:	e3530000 	cmp	r3, #0	; 0x0
    5040:	0a000001 	beq	504c <vs_read+0x238>
    5044:	e59d001c 	ldr	r0, [sp, #28]
    5048:	ebfff18a 	bl	1678 <free>
    504c:	e1a00005 	mov	r0, r5
    5050:	e28dd024 	add	sp, sp, #36	; 0x24
    5054:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5058:	0800d1f8 	.word	0x0800d1f8
    505c:	00012e7b 	.word	0x00012e7b
    5060:	0800d76c 	.word	0x0800d76c
    5064:	00012e7f 	.word	0x00012e7f
    5068:	00011d0c 	.word	0x00011d0c
    506c:	00012e91 	.word	0x00012e91
    5070:	00012e9c 	.word	0x00012e9c

00005074 <vs_reset>:
    5074:	e92d4010 	push	{r4, lr}
    5078:	e59f402c 	ldr	r4, [pc, #44]	; 50ac <vs_reset+0x38>
    507c:	e5940000 	ldr	r0, [r4]
    5080:	ebffef49 	bl	dac <cpu_recase>
    5084:	e5944000 	ldr	r4, [r4]
    5088:	e3540000 	cmp	r4, #0	; 0x0
    508c:	03e00001 	mvneq	r0, #1	; 0x1
    5090:	08bd8010 	popeq	{r4, pc}
    5094:	e5944018 	ldr	r4, [r4, #24]
    5098:	e3540000 	cmp	r4, #0	; 0x0
    509c:	03e00002 	mvneq	r0, #2	; 0x2
    50a0:	08bd8010 	popeq	{r4, pc}
    50a4:	e12fff34 	blx	r4
    50a8:	e8bd8010 	pop	{r4, pc}
    50ac:	0800d1f8 	.word	0x0800d1f8

000050b0 <vs_assign_by_id>:
    50b0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    50b4:	e59fc05c 	ldr	ip, [pc, #92]	; 5118 <vs_assign_by_id+0x68>
    50b8:	e3a03000 	mov	r3, #0	; 0x0
    50bc:	e350000d 	cmp	r0, #13	; 0xd
    50c0:	e24dd004 	sub	sp, sp, #4	; 0x4
    50c4:	e58c3000 	str	r3, [ip]
    50c8:	8a00000b 	bhi	50fc <vs_assign_by_id+0x4c>
    50cc:	e59f2048 	ldr	r2, [pc, #72]	; 511c <vs_assign_by_id+0x6c>
    50d0:	e283302c 	add	r3, r3, #44	; 0x2c
    50d4:	e0232093 	mla	r3, r3, r0, r2
    50d8:	e3510000 	cmp	r1, #0	; 0x0
    50dc:	e58c3000 	str	r3, [ip]
    50e0:	01a00001 	moveq	r0, r1
    50e4:	0a000002 	beq	50f4 <vs_assign_by_id+0x44>
    50e8:	e28dd004 	add	sp, sp, #4	; 0x4
    50ec:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    50f0:	eaffffdf 	b	5074 <vs_reset>
    50f4:	e28dd004 	add	sp, sp, #4	; 0x4
    50f8:	e8bd8000 	pop	{pc}
    50fc:	e1a01000 	mov	r1, r0
    5100:	e59f3018 	ldr	r3, [pc, #24]	; 5120 <vs_assign_by_id+0x70>
    5104:	e59f0018 	ldr	r0, [pc, #24]	; 5124 <vs_assign_by_id+0x74>
    5108:	e1a0e00f 	mov	lr, pc
    510c:	e593f000 	ldr	pc, [r3]
    5110:	e3e00000 	mvn	r0, #0	; 0x0
    5114:	eafffff6 	b	50f4 <vs_assign_by_id+0x44>
    5118:	0800d1f8 	.word	0x0800d1f8
    511c:	00010ee4 	.word	0x00010ee4
    5120:	0800d76c 	.word	0x0800d76c
    5124:	00012ecd 	.word	0x00012ecd

00005128 <vs_assign_by_name>:
    5128:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    512c:	e59f3084 	ldr	r3, [pc, #132]	; 51b8 <vs_assign_by_name+0x90>
    5130:	e3a02000 	mov	r2, #0	; 0x0
    5134:	e24dd004 	sub	sp, sp, #4	; 0x4
    5138:	e5832000 	str	r2, [r3]
    513c:	e1a06000 	mov	r6, r0
    5140:	e1a07001 	mov	r7, r1
    5144:	e1a04002 	mov	r4, r2
    5148:	e3a0a02c 	mov	sl, #44	; 0x2c
    514c:	e59f8068 	ldr	r8, [pc, #104]	; 51bc <vs_assign_by_name+0x94>
    5150:	ea00000d 	b	518c <vs_assign_by_name+0x64>
    5154:	e025849a 	mla	r5, sl, r4, r8
    5158:	e1a00005 	mov	r0, r5
    515c:	ebfff03f 	bl	1260 <strncmp>
    5160:	e3500000 	cmp	r0, #0	; 0x0
    5164:	1a000007 	bne	5188 <vs_assign_by_name+0x60>
    5168:	e59f3048 	ldr	r3, [pc, #72]	; 51b8 <vs_assign_by_name+0x90>
    516c:	e3570000 	cmp	r7, #0	; 0x0
    5170:	e5835000 	str	r5, [r3]
    5174:	01a00007 	moveq	r0, r7
    5178:	0a00000c 	beq	51b0 <vs_assign_by_name+0x88>
    517c:	e28dd004 	add	sp, sp, #4	; 0x4
    5180:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    5184:	eaffffba 	b	5074 <vs_reset>
    5188:	e2844001 	add	r4, r4, #1	; 0x1
    518c:	e354000e 	cmp	r4, #14	; 0xe
    5190:	e1a01006 	mov	r1, r6
    5194:	e3a02010 	mov	r2, #16	; 0x10
    5198:	1affffed 	bne	5154 <vs_assign_by_name+0x2c>
    519c:	e59f001c 	ldr	r0, [pc, #28]	; 51c0 <vs_assign_by_name+0x98>
    51a0:	e59f301c 	ldr	r3, [pc, #28]	; 51c4 <vs_assign_by_name+0x9c>
    51a4:	e1a0e00f 	mov	lr, pc
    51a8:	e593f000 	ldr	pc, [r3]
    51ac:	e3e00000 	mvn	r0, #0	; 0x0
    51b0:	e28dd004 	add	sp, sp, #4	; 0x4
    51b4:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    51b8:	0800d1f8 	.word	0x0800d1f8
    51bc:	00010ee4 	.word	0x00010ee4
    51c0:	00012ee6 	.word	0x00012ee6
    51c4:	0800d76c 	.word	0x0800d76c

000051c8 <ss_lock_isk>:
    51c8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    51cc:	e24dd004 	sub	sp, sp, #4	; 0x4
    51d0:	eb0001d5 	bl	592c <security_enabled>
    51d4:	e3500000 	cmp	r0, #0	; 0x0
    51d8:	0a000002 	beq	51e8 <ss_lock_isk+0x20>
    51dc:	e28dd004 	add	sp, sp, #4	; 0x4
    51e0:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    51e4:	ea00002e 	b	52a4 <block_cipher_lock_isk>
    51e8:	e28dd004 	add	sp, sp, #4	; 0x4
    51ec:	e8bd8000 	pop	{pc}

000051f0 <ss_aes_data>:
    51f0:	e92d4070 	push	{r4, r5, r6, lr}
    51f4:	e1a05000 	mov	r5, r0
    51f8:	e1a06001 	mov	r6, r1
    51fc:	e1a04002 	mov	r4, r2
    5200:	eb0001c9 	bl	592c <security_enabled>
    5204:	e3500000 	cmp	r0, #0	; 0x0
    5208:	0a000007 	beq	522c <ss_aes_data+0x3c>
    520c:	e284200f 	add	r2, r4, #15	; 0xf
    5210:	e1a00005 	mov	r0, r5
    5214:	e1a01006 	mov	r1, r6
    5218:	e3c2200f 	bic	r2, r2, #15	; 0xf
    521c:	eb0000a0 	bl	54a4 <block_cipher_operation>
    5220:	e3500000 	cmp	r0, #0	; 0x0
    5224:	13e00000 	mvnne	r0, #0	; 0x0
    5228:	18bd8070 	popne	{r4, r5, r6, pc}
    522c:	e3a00000 	mov	r0, #0	; 0x0
    5230:	e8bd8070 	pop	{r4, r5, r6, pc}

00005234 <ss_aes_init>:
    5234:	e92d4070 	push	{r4, r5, r6, lr}
    5238:	e1a05000 	mov	r5, r0
    523c:	e1a04001 	mov	r4, r1
    5240:	e1a06002 	mov	r6, r2
    5244:	eb0001b8 	bl	592c <security_enabled>
    5248:	e3500000 	cmp	r0, #0	; 0x0
    524c:	0a00000b 	beq	5280 <ss_aes_init+0x4c>
    5250:	e3540000 	cmp	r4, #0	; 0x0
    5254:	01a00005 	moveq	r0, r5
    5258:	01a02004 	moveq	r2, r4
    525c:	01a03006 	moveq	r3, r6
    5260:	03a01001 	moveq	r1, #1	; 0x1
    5264:	11a00005 	movne	r0, r5
    5268:	11a02004 	movne	r2, r4
    526c:	11a03006 	movne	r3, r6
    5270:	13a01000 	movne	r1, #0	; 0x0
    5274:	eb00002c 	bl	532c <block_cipher_aes_init>
    5278:	e3500000 	cmp	r0, #0	; 0x0
    527c:	1a000001 	bne	5288 <ss_aes_init+0x54>
    5280:	e3a00000 	mov	r0, #0	; 0x0
    5284:	e8bd8070 	pop	{r4, r5, r6, pc}
    5288:	e3e00000 	mvn	r0, #0	; 0x0
    528c:	e8bd8070 	pop	{r4, r5, r6, pc}

00005290 <check_register_bit>:
    5290:	e3a03001 	mov	r3, #1	; 0x1
    5294:	e0103113 	ands	r3, r0, r3, lsl r1
    5298:	03a00000 	moveq	r0, #0	; 0x0
    529c:	13a00001 	movne	r0, #1	; 0x1
    52a0:	e12fff1e 	bx	lr

000052a4 <block_cipher_lock_isk>:
    52a4:	e59f200c 	ldr	r2, [pc, #12]	; 52b8 <block_cipher_lock_isk+0x14>
    52a8:	e5923400 	ldr	r3, [r2, #1024]
    52ac:	e3833706 	orr	r3, r3, #1572864	; 0x180000
    52b0:	e5823400 	str	r3, [r2, #1024]
    52b4:	e12fff1e 	bx	lr
    52b8:	27700000 	.word	0x27700000

000052bc <block_cipher_reset>:
    52bc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    52c0:	e59f4060 	ldr	r4, [pc, #96]	; 5328 <block_cipher_reset+0x6c>
    52c4:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    52c8:	e5843400 	str	r3, [r4, #1024]
    52cc:	e24dd004 	sub	sp, sp, #4	; 0x4
    52d0:	eb000d4d 	bl	880c <get_ticks>
    52d4:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    52d8:	e3a07000 	mov	r7, #0	; 0x0
    52dc:	e0966000 	adds	r6, r6, r0
    52e0:	e1a05004 	mov	r5, r4
    52e4:	e0a77001 	adc	r7, r7, r1
    52e8:	e5944400 	ldr	r4, [r4, #1024]
    52ec:	ea000008 	b	5314 <block_cipher_reset+0x58>
    52f0:	e5954400 	ldr	r4, [r5, #1024]
    52f4:	eb000d44 	bl	880c <get_ticks>
    52f8:	e1510007 	cmp	r1, r7
    52fc:	8a000002 	bhi	530c <block_cipher_reset+0x50>
    5300:	1a000003 	bne	5314 <block_cipher_reset+0x58>
    5304:	e1500006 	cmp	r0, r6
    5308:	9a000001 	bls	5314 <block_cipher_reset+0x58>
    530c:	e3a00001 	mov	r0, #1	; 0x1
    5310:	ea000002 	b	5320 <block_cipher_reset+0x64>
    5314:	e3540000 	cmp	r4, #0	; 0x0
    5318:	bafffff4 	blt	52f0 <block_cipher_reset+0x34>
    531c:	e3a00000 	mov	r0, #0	; 0x0
    5320:	e28dd004 	add	sp, sp, #4	; 0x4
    5324:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5328:	27700000 	.word	0x27700000

0000532c <block_cipher_aes_init>:
    532c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5330:	e24dd004 	sub	sp, sp, #4	; 0x4
    5334:	e1a05000 	mov	r5, r0
    5338:	e1a04001 	mov	r4, r1
    533c:	e1a06002 	mov	r6, r2
    5340:	e1a07003 	mov	r7, r3
    5344:	ebffffdc 	bl	52bc <block_cipher_reset>
    5348:	e3500000 	cmp	r0, #0	; 0x0
    534c:	1a00003d 	bne	5448 <block_cipher_aes_init+0x11c>
    5350:	e59f3134 	ldr	r3, [pc, #308]	; 548c <block_cipher_aes_init+0x160>
    5354:	e3a02001 	mov	r2, #1	; 0x1
    5358:	e3550001 	cmp	r5, #1	; 0x1
    535c:	e5832404 	str	r2, [r3, #1028]
    5360:	8a000038 	bhi	5448 <block_cipher_aes_init+0x11c>
    5364:	e3540003 	cmp	r4, #3	; 0x3
    5368:	13540000 	cmpne	r4, #0	; 0x0
    536c:	0a000001 	beq	5378 <block_cipher_aes_init+0x4c>
    5370:	e1540002 	cmp	r4, r2
    5374:	1a000033 	bne	5448 <block_cipher_aes_init+0x11c>
    5378:	e1a03305 	lsl	r3, r5, #6
    537c:	e3833901 	orr	r3, r3, #16384	; 0x4000
    5380:	e3833004 	orr	r3, r3, #4	; 0x4
    5384:	e59f2100 	ldr	r2, [pc, #256]	; 548c <block_cipher_aes_init+0x160>
    5388:	e1833984 	orr	r3, r3, r4, lsl #19
    538c:	e3540000 	cmp	r4, #0	; 0x0
    5390:	e5823400 	str	r3, [r2, #1024]
    5394:	1a000008 	bne	53bc <block_cipher_aes_init+0x90>
    5398:	e59f10f0 	ldr	r1, [pc, #240]	; 5490 <block_cipher_aes_init+0x164>
    539c:	e59f00f0 	ldr	r0, [pc, #240]	; 5494 <block_cipher_aes_init+0x168>
    53a0:	e59f30f0 	ldr	r3, [pc, #240]	; 5498 <block_cipher_aes_init+0x16c>
    53a4:	e0862001 	add	r2, r6, r1
    53a8:	e0823003 	add	r3, r2, r3
    53ac:	e5933000 	ldr	r3, [r3]
    53b0:	e4813004 	str	r3, [r1], #4
    53b4:	e1510000 	cmp	r1, r0
    53b8:	1afffff8 	bne	53a0 <block_cipher_aes_init+0x74>
    53bc:	e59f10d0 	ldr	r1, [pc, #208]	; 5494 <block_cipher_aes_init+0x168>
    53c0:	e59f00d4 	ldr	r0, [pc, #212]	; 549c <block_cipher_aes_init+0x170>
    53c4:	e59f30d4 	ldr	r3, [pc, #212]	; 54a0 <block_cipher_aes_init+0x174>
    53c8:	e0872001 	add	r2, r7, r1
    53cc:	e0823003 	add	r3, r2, r3
    53d0:	e5933000 	ldr	r3, [r3]
    53d4:	e4813004 	str	r3, [r1], #4
    53d8:	e1510000 	cmp	r1, r0
    53dc:	1afffff8 	bne	53c4 <block_cipher_aes_init+0x98>
    53e0:	e3540000 	cmp	r4, #0	; 0x0
    53e4:	1a000008 	bne	540c <block_cipher_aes_init+0xe0>
    53e8:	eb000d07 	bl	880c <get_ticks>
    53ec:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    53f0:	e3a05000 	mov	r5, #0	; 0x0
    53f4:	e0944000 	adds	r4, r4, r0
    53f8:	e59f608c 	ldr	r6, [pc, #140]	; 548c <block_cipher_aes_init+0x160>
    53fc:	e0a55001 	adc	r5, r5, r1
    5400:	e596340c 	ldr	r3, [r6, #1036]
    5404:	e3130008 	tst	r3, #8	; 0x8
    5408:	0a000011 	beq	5454 <block_cipher_aes_init+0x128>
    540c:	eb000cfe 	bl	880c <get_ticks>
    5410:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    5414:	e3a05000 	mov	r5, #0	; 0x0
    5418:	e0944000 	adds	r4, r4, r0
    541c:	e59f6068 	ldr	r6, [pc, #104]	; 548c <block_cipher_aes_init+0x160>
    5420:	e0a55001 	adc	r5, r5, r1
    5424:	e596340c 	ldr	r3, [r6, #1036]
    5428:	e59f205c 	ldr	r2, [pc, #92]	; 548c <block_cipher_aes_init+0x160>
    542c:	e3130010 	tst	r3, #16	; 0x10
    5430:	0a00000e 	beq	5470 <block_cipher_aes_init+0x144>
    5434:	e5923400 	ldr	r3, [r2, #1024]
    5438:	e3a00000 	mov	r0, #0	; 0x0
    543c:	e3833001 	orr	r3, r3, #1	; 0x1
    5440:	e5823400 	str	r3, [r2, #1024]
    5444:	ea000000 	b	544c <block_cipher_aes_init+0x120>
    5448:	e3a00001 	mov	r0, #1	; 0x1
    544c:	e28dd004 	add	sp, sp, #4	; 0x4
    5450:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5454:	eb000cec 	bl	880c <get_ticks>
    5458:	e1510005 	cmp	r1, r5
    545c:	8afffff9 	bhi	5448 <block_cipher_aes_init+0x11c>
    5460:	1affffe6 	bne	5400 <block_cipher_aes_init+0xd4>
    5464:	e1500004 	cmp	r0, r4
    5468:	8afffff6 	bhi	5448 <block_cipher_aes_init+0x11c>
    546c:	eaffffe3 	b	5400 <block_cipher_aes_init+0xd4>
    5470:	eb000ce5 	bl	880c <get_ticks>
    5474:	e1510005 	cmp	r1, r5
    5478:	8afffff2 	bhi	5448 <block_cipher_aes_init+0x11c>
    547c:	1affffe8 	bne	5424 <block_cipher_aes_init+0xf8>
    5480:	e1500004 	cmp	r0, r4
    5484:	8affffef 	bhi	5448 <block_cipher_aes_init+0x11c>
    5488:	eaffffe5 	b	5424 <block_cipher_aes_init+0xf8>
    548c:	27700000 	.word	0x27700000
    5490:	27700420 	.word	0x27700420
    5494:	27700440 	.word	0x27700440
    5498:	d88ffbe0 	.word	0xd88ffbe0
    549c:	27700450 	.word	0x27700450
    54a0:	d88ffbc0 	.word	0xd88ffbc0

000054a4 <block_cipher_operation>:
    54a4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    54a8:	e24dd00c 	sub	sp, sp, #12	; 0xc
    54ac:	e1a08002 	mov	r8, r2
    54b0:	e58d0004 	str	r0, [sp, #4]
    54b4:	e1a09001 	mov	r9, r1
    54b8:	e1a06002 	mov	r6, r2
    54bc:	e59fa0c4 	ldr	sl, [pc, #196]	; 5588 <block_cipher_operation+0xe4>
    54c0:	e3a0b000 	mov	fp, #0	; 0x0
    54c4:	ea000028 	b	556c <block_cipher_operation+0xc8>
    54c8:	e59d3004 	ldr	r3, [sp, #4]
    54cc:	e3560902 	cmp	r6, #32768	; 0x8000
    54d0:	b1a07006 	movlt	r7, r6
    54d4:	a3a07902 	movge	r7, #32768	; 0x8000
    54d8:	e0664008 	rsb	r4, r6, r8
    54dc:	e3a00000 	mov	r0, #0	; 0x0
    54e0:	e0831004 	add	r1, r3, r4
    54e4:	e1a02007 	mov	r2, r7
    54e8:	eb0000ed 	bl	58a4 <dma_sdma_init>
    54ec:	e3500000 	cmp	r0, #0	; 0x0
    54f0:	1a000021 	bne	557c <block_cipher_operation+0xd8>
    54f4:	e0891004 	add	r1, r9, r4
    54f8:	e2800001 	add	r0, r0, #1	; 0x1
    54fc:	e1a02007 	mov	r2, r7
    5500:	eb0000e7 	bl	58a4 <dma_sdma_init>
    5504:	e3500000 	cmp	r0, #0	; 0x0
    5508:	1a00001b 	bne	557c <block_cipher_operation+0xd8>
    550c:	eb0000d4 	bl	5864 <dma_start>
    5510:	e3a00001 	mov	r0, #1	; 0x1
    5514:	eb0000d2 	bl	5864 <dma_start>
    5518:	eb000cbb 	bl	880c <get_ticks>
    551c:	e090400a 	adds	r4, r0, sl
    5520:	e0a1500b 	adc	r5, r1, fp
    5524:	ea000005 	b	5540 <block_cipher_operation+0x9c>
    5528:	eb000cb7 	bl	880c <get_ticks>
    552c:	e1510005 	cmp	r1, r5
    5530:	8a000011 	bhi	557c <block_cipher_operation+0xd8>
    5534:	1a000001 	bne	5540 <block_cipher_operation+0x9c>
    5538:	e1500004 	cmp	r0, r4
    553c:	8a00000e 	bhi	557c <block_cipher_operation+0xd8>
    5540:	e3a01003 	mov	r1, #3	; 0x3
    5544:	e3a00001 	mov	r0, #1	; 0x1
    5548:	eb0000cb 	bl	587c <dma_get_status>
    554c:	e3500000 	cmp	r0, #0	; 0x0
    5550:	0afffff4 	beq	5528 <block_cipher_operation+0x84>
    5554:	e3a00000 	mov	r0, #0	; 0x0
    5558:	e3a01003 	mov	r1, #3	; 0x3
    555c:	eb0000c6 	bl	587c <dma_get_status>
    5560:	e3500000 	cmp	r0, #0	; 0x0
    5564:	0a000004 	beq	557c <block_cipher_operation+0xd8>
    5568:	e0676006 	rsb	r6, r7, r6
    556c:	e3560000 	cmp	r6, #0	; 0x0
    5570:	1affffd4 	bne	54c8 <block_cipher_operation+0x24>
    5574:	e1a00006 	mov	r0, r6
    5578:	ea000000 	b	5580 <block_cipher_operation+0xdc>
    557c:	e3a00001 	mov	r0, #1	; 0x1
    5580:	e28dd00c 	add	sp, sp, #12	; 0xc
    5584:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5588:	000f4240 	.word	0x000f4240

0000558c <hash_len>:
    558c:	e59f300c 	ldr	r3, [pc, #12]	; 55a0 <hash_len+0x14>
    5590:	e5932000 	ldr	r2, [r3]
    5594:	e59f3008 	ldr	r3, [pc, #8]	; 55a4 <hash_len+0x18>
    5598:	e7930202 	ldr	r0, [r3, r2, lsl #4]
    559c:	e12fff1e 	bx	lr
    55a0:	0800d1fc 	.word	0x0800d1fc
    55a4:	0001114c 	.word	0x0001114c

000055a8 <hash_value>:
    55a8:	e92d4070 	push	{r4, r5, r6, lr}
    55ac:	e59f6054 	ldr	r6, [pc, #84]	; 5608 <hash_value+0x60>
    55b0:	e1a05000 	mov	r5, r0
    55b4:	e5963000 	ldr	r3, [r6]
    55b8:	e3530000 	cmp	r3, #0	; 0x0
    55bc:	0a000009 	beq	55e8 <hash_value+0x40>
    55c0:	eb0000d9 	bl	592c <security_enabled>
    55c4:	e2504000 	subs	r4, r0, #0	; 0x0
    55c8:	1a000006 	bne	55e8 <hash_value+0x40>
    55cc:	e59f0038 	ldr	r0, [pc, #56]	; 560c <hash_value+0x64>
    55d0:	e5961000 	ldr	r1, [r6]
    55d4:	e59f3034 	ldr	r3, [pc, #52]	; 5610 <hash_value+0x68>
    55d8:	e1a0e00f 	mov	lr, pc
    55dc:	e593f000 	ldr	pc, [r3]
    55e0:	e1a00004 	mov	r0, r4
    55e4:	e8bd8070 	pop	{r4, r5, r6, pc}
    55e8:	e59f3018 	ldr	r3, [pc, #24]	; 5608 <hash_value+0x60>
    55ec:	e1a00005 	mov	r0, r5
    55f0:	e5932000 	ldr	r2, [r3]
    55f4:	e59f3018 	ldr	r3, [pc, #24]	; 5614 <hash_value+0x6c>
    55f8:	e0833202 	add	r3, r3, r2, lsl #4
    55fc:	e1a0e00f 	mov	lr, pc
    5600:	e593f00c 	ldr	pc, [r3, #12]
    5604:	e8bd8070 	pop	{r4, r5, r6, pc}
    5608:	0800d1fc 	.word	0x0800d1fc
    560c:	00012f01 	.word	0x00012f01
    5610:	0800d76c 	.word	0x0800d76c
    5614:	0001114c 	.word	0x0001114c

00005618 <hash_data>:
    5618:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    561c:	e59f5074 	ldr	r5, [pc, #116]	; 5698 <hash_data+0x80>
    5620:	e24dd004 	sub	sp, sp, #4	; 0x4
    5624:	e5953000 	ldr	r3, [r5]
    5628:	e1a06000 	mov	r6, r0
    562c:	e3530000 	cmp	r3, #0	; 0x0
    5630:	e1a07001 	mov	r7, r1
    5634:	0a000009 	beq	5660 <hash_data+0x48>
    5638:	eb0000bb 	bl	592c <security_enabled>
    563c:	e2504000 	subs	r4, r0, #0	; 0x0
    5640:	1a000006 	bne	5660 <hash_data+0x48>
    5644:	e59f0050 	ldr	r0, [pc, #80]	; 569c <hash_data+0x84>
    5648:	e5951000 	ldr	r1, [r5]
    564c:	e59f304c 	ldr	r3, [pc, #76]	; 56a0 <hash_data+0x88>
    5650:	e1a0e00f 	mov	lr, pc
    5654:	e593f000 	ldr	pc, [r3]
    5658:	e1a00004 	mov	r0, r4
    565c:	ea00000b 	b	5690 <hash_data+0x78>
    5660:	e59f003c 	ldr	r0, [pc, #60]	; 56a4 <hash_data+0x8c>
    5664:	e59f3034 	ldr	r3, [pc, #52]	; 56a0 <hash_data+0x88>
    5668:	e1a0e00f 	mov	lr, pc
    566c:	e593f000 	ldr	pc, [r3]
    5670:	e59f3020 	ldr	r3, [pc, #32]	; 5698 <hash_data+0x80>
    5674:	e1a00006 	mov	r0, r6
    5678:	e5932000 	ldr	r2, [r3]
    567c:	e59f3024 	ldr	r3, [pc, #36]	; 56a8 <hash_data+0x90>
    5680:	e1a01007 	mov	r1, r7
    5684:	e0833202 	add	r3, r3, r2, lsl #4
    5688:	e1a0e00f 	mov	lr, pc
    568c:	e593f008 	ldr	pc, [r3, #8]
    5690:	e28dd004 	add	sp, sp, #4	; 0x4
    5694:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5698:	0800d1fc 	.word	0x0800d1fc
    569c:	00012f01 	.word	0x00012f01
    56a0:	0800d76c 	.word	0x0800d76c
    56a4:	00012f22 	.word	0x00012f22
    56a8:	0001114c 	.word	0x0001114c

000056ac <hash_init>:
    56ac:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    56b0:	e3500003 	cmp	r0, #3	; 0x3
    56b4:	e24dd004 	sub	sp, sp, #4	; 0x4
    56b8:	e1a04000 	mov	r4, r0
    56bc:	e1a07001 	mov	r7, r1
    56c0:	9a000005 	bls	56dc <hash_init+0x30>
    56c4:	e59f0064 	ldr	r0, [pc, #100]	; 5730 <hash_init+0x84>
    56c8:	e59f3064 	ldr	r3, [pc, #100]	; 5734 <hash_init+0x88>
    56cc:	e1a0e00f 	mov	lr, pc
    56d0:	e593f000 	ldr	pc, [r3]
    56d4:	e3e00000 	mvn	r0, #0	; 0x0
    56d8:	ea000012 	b	5728 <hash_init+0x7c>
    56dc:	e59f6054 	ldr	r6, [pc, #84]	; 5738 <hash_init+0x8c>
    56e0:	e3500000 	cmp	r0, #0	; 0x0
    56e4:	e5860000 	str	r0, [r6]
    56e8:	0a000009 	beq	5714 <hash_init+0x68>
    56ec:	eb00008e 	bl	592c <security_enabled>
    56f0:	e2505000 	subs	r5, r0, #0	; 0x0
    56f4:	1a000006 	bne	5714 <hash_init+0x68>
    56f8:	e59f003c 	ldr	r0, [pc, #60]	; 573c <hash_init+0x90>
    56fc:	e5961000 	ldr	r1, [r6]
    5700:	e59f302c 	ldr	r3, [pc, #44]	; 5734 <hash_init+0x88>
    5704:	e1a0e00f 	mov	lr, pc
    5708:	e593f000 	ldr	pc, [r3]
    570c:	e1a00005 	mov	r0, r5
    5710:	ea000004 	b	5728 <hash_init+0x7c>
    5714:	e59f3024 	ldr	r3, [pc, #36]	; 5740 <hash_init+0x94>
    5718:	e1a00007 	mov	r0, r7
    571c:	e0833204 	add	r3, r3, r4, lsl #4
    5720:	e1a0e00f 	mov	lr, pc
    5724:	e593f004 	ldr	pc, [r3, #4]
    5728:	e28dd004 	add	sp, sp, #4	; 0x4
    572c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5730:	00012f2d 	.word	0x00012f2d
    5734:	0800d76c 	.word	0x0800d76c
    5738:	0800d1fc 	.word	0x0800d1fc
    573c:	00012f01 	.word	0x00012f01
    5740:	0001114c 	.word	0x0001114c

00005744 <md5_value>:
    5744:	e92d4030 	push	{r4, r5, lr}
    5748:	e59f3054 	ldr	r3, [pc, #84]	; 57a4 <md5_value+0x60>
    574c:	e1a05000 	mov	r5, r0
    5750:	e893000c 	ldm	r3, {r2, r3}
    5754:	e1823003 	orr	r3, r2, r3
    5758:	e24dd024 	sub	sp, sp, #36	; 0x24
    575c:	e3530000 	cmp	r3, #0	; 0x0
    5760:	13e00000 	mvnne	r0, #0	; 0x0
    5764:	1a00000c 	bne	579c <md5_value+0x58>
    5768:	e1a0000d 	mov	r0, sp
    576c:	eb000178 	bl	5d54 <hash_module_get_result>
    5770:	e3500000 	cmp	r0, #0	; 0x0
    5774:	e1a0400d 	mov	r4, sp
    5778:	13e00001 	mvnne	r0, #1	; 0x1
    577c:	1a000006 	bne	579c <md5_value+0x58>
    5780:	e1a0200d 	mov	r2, sp
    5784:	e7d23000 	ldrb	r3, [r2, r0]
    5788:	e7c53000 	strb	r3, [r5, r0]
    578c:	e2800001 	add	r0, r0, #1	; 0x1
    5790:	e3500010 	cmp	r0, #16	; 0x10
    5794:	1afffffa 	bne	5784 <md5_value+0x40>
    5798:	e3a00000 	mov	r0, #0	; 0x0
    579c:	e28dd024 	add	sp, sp, #36	; 0x24
    57a0:	e8bd8030 	pop	{r4, r5, pc}
    57a4:	0800d200 	.word	0x0800d200

000057a8 <md5_data>:
    57a8:	e59f305c 	ldr	r3, [pc, #92]	; 580c <md5_data+0x64>
    57ac:	e92d4070 	push	{r4, r5, r6, lr}
    57b0:	e1a06001 	mov	r6, r1
    57b4:	e5931004 	ldr	r1, [r3, #4]
    57b8:	e1a04006 	mov	r4, r6
    57bc:	e3a05000 	mov	r5, #0	; 0x0
    57c0:	e1550001 	cmp	r5, r1
    57c4:	e593c000 	ldr	ip, [r3]
    57c8:	8a000002 	bhi	57d8 <md5_data+0x30>
    57cc:	1a000003 	bne	57e0 <md5_data+0x38>
    57d0:	e154000c 	cmp	r4, ip
    57d4:	9a000001 	bls	57e0 <md5_data+0x38>
    57d8:	e3e00000 	mvn	r0, #0	; 0x0
    57dc:	e8bd8070 	pop	{r4, r5, r6, pc}
    57e0:	e1a0200c 	mov	r2, ip
    57e4:	e1a03001 	mov	r3, r1
    57e8:	e59fc01c 	ldr	ip, [pc, #28]	; 580c <md5_data+0x64>
    57ec:	e0522004 	subs	r2, r2, r4
    57f0:	e0c33005 	sbc	r3, r3, r5
    57f4:	e1a01006 	mov	r1, r6
    57f8:	e88c000c 	stm	ip, {r2, r3}
    57fc:	eb0001b7 	bl	5ee0 <hash_dma_transfer>
    5800:	e3500000 	cmp	r0, #0	; 0x0
    5804:	13e00001 	mvnne	r0, #1	; 0x1
    5808:	e8bd8070 	pop	{r4, r5, r6, pc}
    580c:	0800d200 	.word	0x0800d200

00005810 <md5_init>:
    5810:	e1a03ea0 	lsr	r3, r0, #29
    5814:	e1a0c180 	lsl	ip, r0, #3
    5818:	e92d4010 	push	{r4, lr}
    581c:	e59f403c 	ldr	r4, [pc, #60]	; 5860 <md5_init+0x50>
    5820:	e1a0200c 	mov	r2, ip
    5824:	e1a01003 	mov	r1, r3
    5828:	e3a00000 	mov	r0, #0	; 0x0
    582c:	e5843004 	str	r3, [r4, #4]
    5830:	e584c000 	str	ip, [r4]
    5834:	eb00018d 	bl	5e70 <hash_module_init>
    5838:	e3500000 	cmp	r0, #0	; 0x0
    583c:	05942000 	ldreq	r2, [r4]
    5840:	05943004 	ldreq	r3, [r4, #4]
    5844:	01a021a2 	lsreq	r2, r2, #3
    5848:	01822e83 	orreq	r2, r2, r3, lsl #29
    584c:	01a031a3 	lsreq	r3, r3, #3
    5850:	13e00000 	mvnne	r0, #0	; 0x0
    5854:	05843004 	streq	r3, [r4, #4]
    5858:	05842000 	streq	r2, [r4]
    585c:	e8bd8010 	pop	{r4, pc}
    5860:	0800d200 	.word	0x0800d200

00005864 <dma_start>:
    5864:	e280099d 	add	r0, r0, #2572288	; 0x274000
    5868:	e2800a03 	add	r0, r0, #12288	; 0x3000
    586c:	e1a00400 	lsl	r0, r0, #8
    5870:	e3a03001 	mov	r3, #1	; 0x1
    5874:	e5803000 	str	r3, [r0]
    5878:	e12fff1e 	bx	lr

0000587c <dma_get_status>:
    587c:	e3a0c001 	mov	ip, #1	; 0x1
    5880:	e1a0211c 	lsl	r2, ip, r1
    5884:	e280099d 	add	r0, r0, #2572288	; 0x274000
    5888:	e2800a03 	add	r0, r0, #12288	; 0x3000
    588c:	e1a01400 	lsl	r1, r0, #8
    5890:	e5913024 	ldr	r3, [r1, #36]
    5894:	e0130002 	ands	r0, r3, r2
    5898:	15812024 	strne	r2, [r1, #36]
    589c:	11a0000c 	movne	r0, ip
    58a0:	e12fff1e 	bx	lr

000058a4 <dma_sdma_init>:
    58a4:	e280c99d 	add	ip, r0, #2572288	; 0x274000
    58a8:	e28cca03 	add	ip, ip, #12288	; 0x3000
    58ac:	e1a0040c 	lsl	r0, ip, #8
    58b0:	e2800004 	add	r0, r0, #4	; 0x4
    58b4:	e5903000 	ldr	r3, [r0]
    58b8:	e3520801 	cmp	r2, #65536	; 0x10000
    58bc:	e3833001 	orr	r3, r3, #1	; 0x1
    58c0:	e5803000 	str	r3, [r0]
    58c4:	e92d4010 	push	{r4, lr}
    58c8:	e3a03341 	mov	r3, #67108865	; 0x4000001
    58cc:	e1a0440c 	lsl	r4, ip, #8
    58d0:	e584300c 	str	r3, [r4, #12]
    58d4:	31a02802 	lslcc	r2, r2, #16
    58d8:	31a0340c 	lslcc	r3, ip, #8
    58dc:	31a02822 	lsrcc	r2, r2, #16
    58e0:	32833010 	addcc	r3, r3, #16	; 0x10
    58e4:	35832000 	strcc	r2, [r3]
    58e8:	23a00001 	movcs	r0, #1	; 0x1
    58ec:	35841014 	strcc	r1, [r4, #20]
    58f0:	33a00000 	movcc	r0, #0	; 0x0
    58f4:	e8bd8010 	pop	{r4, pc}

000058f8 <security_cke_control>:
    58f8:	e59fc014 	ldr	ip, [pc, #20]	; 5914 <security_cke_control+0x1c>
    58fc:	e3a02001 	mov	r2, #1	; 0x1
    5900:	e59c3000 	ldr	r3, [ip]
    5904:	e1c33012 	bic	r3, r3, r2, lsl r0
    5908:	e1833011 	orr	r3, r3, r1, lsl r0
    590c:	e58c3000 	str	r3, [ip]
    5910:	e12fff1e 	bx	lr
    5914:	27702000 	.word	0x27702000

00005918 <security_enable>:
    5918:	e59f3008 	ldr	r3, [pc, #8]	; 5928 <security_enable+0x10>
    591c:	e3a02001 	mov	r2, #1	; 0x1
    5920:	e5832000 	str	r2, [r3]
    5924:	e12fff1e 	bx	lr
    5928:	0800d208 	.word	0x0800d208

0000592c <security_enabled>:
    592c:	e59f300c 	ldr	r3, [pc, #12]	; 5940 <security_enabled+0x14>
    5930:	e5930000 	ldr	r0, [r3]
    5934:	e2500000 	subs	r0, r0, #0	; 0x0
    5938:	13a00001 	movne	r0, #1	; 0x1
    593c:	e12fff1e 	bx	lr
    5940:	0800d208 	.word	0x0800d208

00005944 <dma_reset>:
    5944:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5948:	e280099d 	add	r0, r0, #2572288	; 0x274000
    594c:	e2800a03 	add	r0, r0, #12288	; 0x3000
    5950:	e1a07400 	lsl	r7, r0, #8
    5954:	e3a03002 	mov	r3, #2	; 0x2
    5958:	e24dd004 	sub	sp, sp, #4	; 0x4
    595c:	e5873000 	str	r3, [r7]
    5960:	eb000ba9 	bl	880c <get_ticks>
    5964:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    5968:	e3a05000 	mov	r5, #0	; 0x0
    596c:	e0944000 	adds	r4, r4, r0
    5970:	e0a55001 	adc	r5, r5, r1
    5974:	e5976000 	ldr	r6, [r7]
    5978:	ea000008 	b	59a0 <dma_reset+0x5c>
    597c:	e5976000 	ldr	r6, [r7]
    5980:	eb000ba1 	bl	880c <get_ticks>
    5984:	e1510005 	cmp	r1, r5
    5988:	8a000002 	bhi	5998 <dma_reset+0x54>
    598c:	1a000003 	bne	59a0 <dma_reset+0x5c>
    5990:	e1500004 	cmp	r0, r4
    5994:	9a000001 	bls	59a0 <dma_reset+0x5c>
    5998:	e3a00001 	mov	r0, #1	; 0x1
    599c:	ea000002 	b	59ac <dma_reset+0x68>
    59a0:	e3560000 	cmp	r6, #0	; 0x0
    59a4:	1afffff4 	bne	597c <dma_reset+0x38>
    59a8:	e1a00006 	mov	r0, r6
    59ac:	e28dd004 	add	sp, sp, #4	; 0x4
    59b0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

000059b4 <sha1_value>:
    59b4:	e92d4030 	push	{r4, r5, lr}
    59b8:	e59f3054 	ldr	r3, [pc, #84]	; 5a14 <sha1_value+0x60>
    59bc:	e1a05000 	mov	r5, r0
    59c0:	e893000c 	ldm	r3, {r2, r3}
    59c4:	e1823003 	orr	r3, r2, r3
    59c8:	e24dd024 	sub	sp, sp, #36	; 0x24
    59cc:	e3530000 	cmp	r3, #0	; 0x0
    59d0:	13e00000 	mvnne	r0, #0	; 0x0
    59d4:	1a00000c 	bne	5a0c <sha1_value+0x58>
    59d8:	e1a0000d 	mov	r0, sp
    59dc:	eb0000dc 	bl	5d54 <hash_module_get_result>
    59e0:	e3500000 	cmp	r0, #0	; 0x0
    59e4:	e1a0400d 	mov	r4, sp
    59e8:	13e00001 	mvnne	r0, #1	; 0x1
    59ec:	1a000006 	bne	5a0c <sha1_value+0x58>
    59f0:	e1a0200d 	mov	r2, sp
    59f4:	e7d23000 	ldrb	r3, [r2, r0]
    59f8:	e7c53000 	strb	r3, [r5, r0]
    59fc:	e2800001 	add	r0, r0, #1	; 0x1
    5a00:	e3500014 	cmp	r0, #20	; 0x14
    5a04:	1afffffa 	bne	59f4 <sha1_value+0x40>
    5a08:	e3a00000 	mov	r0, #0	; 0x0
    5a0c:	e28dd024 	add	sp, sp, #36	; 0x24
    5a10:	e8bd8030 	pop	{r4, r5, pc}
    5a14:	0800d210 	.word	0x0800d210

00005a18 <sha1_data>:
    5a18:	e59f305c 	ldr	r3, [pc, #92]	; 5a7c <sha1_data+0x64>
    5a1c:	e92d4070 	push	{r4, r5, r6, lr}
    5a20:	e1a06001 	mov	r6, r1
    5a24:	e5931004 	ldr	r1, [r3, #4]
    5a28:	e1a04006 	mov	r4, r6
    5a2c:	e3a05000 	mov	r5, #0	; 0x0
    5a30:	e1550001 	cmp	r5, r1
    5a34:	e593c000 	ldr	ip, [r3]
    5a38:	8a000002 	bhi	5a48 <sha1_data+0x30>
    5a3c:	1a000003 	bne	5a50 <sha1_data+0x38>
    5a40:	e154000c 	cmp	r4, ip
    5a44:	9a000001 	bls	5a50 <sha1_data+0x38>
    5a48:	e3e00000 	mvn	r0, #0	; 0x0
    5a4c:	e8bd8070 	pop	{r4, r5, r6, pc}
    5a50:	e1a0200c 	mov	r2, ip
    5a54:	e1a03001 	mov	r3, r1
    5a58:	e59fc01c 	ldr	ip, [pc, #28]	; 5a7c <sha1_data+0x64>
    5a5c:	e0522004 	subs	r2, r2, r4
    5a60:	e0c33005 	sbc	r3, r3, r5
    5a64:	e1a01006 	mov	r1, r6
    5a68:	e88c000c 	stm	ip, {r2, r3}
    5a6c:	eb00011b 	bl	5ee0 <hash_dma_transfer>
    5a70:	e3500000 	cmp	r0, #0	; 0x0
    5a74:	13e00001 	mvnne	r0, #1	; 0x1
    5a78:	e8bd8070 	pop	{r4, r5, r6, pc}
    5a7c:	0800d210 	.word	0x0800d210

00005a80 <sha1_init>:
    5a80:	e1a03ea0 	lsr	r3, r0, #29
    5a84:	e1a0c180 	lsl	ip, r0, #3
    5a88:	e92d4010 	push	{r4, lr}
    5a8c:	e59f403c 	ldr	r4, [pc, #60]	; 5ad0 <sha1_init+0x50>
    5a90:	e1a0200c 	mov	r2, ip
    5a94:	e1a01003 	mov	r1, r3
    5a98:	e3a00001 	mov	r0, #1	; 0x1
    5a9c:	e5843004 	str	r3, [r4, #4]
    5aa0:	e584c000 	str	ip, [r4]
    5aa4:	eb0000f1 	bl	5e70 <hash_module_init>
    5aa8:	e3500000 	cmp	r0, #0	; 0x0
    5aac:	05942000 	ldreq	r2, [r4]
    5ab0:	05943004 	ldreq	r3, [r4, #4]
    5ab4:	01a021a2 	lsreq	r2, r2, #3
    5ab8:	01822e83 	orreq	r2, r2, r3, lsl #29
    5abc:	01a031a3 	lsreq	r3, r3, #3
    5ac0:	13e00000 	mvnne	r0, #0	; 0x0
    5ac4:	05843004 	streq	r3, [r4, #4]
    5ac8:	05842000 	streq	r2, [r4]
    5acc:	e8bd8010 	pop	{r4, pc}
    5ad0:	0800d210 	.word	0x0800d210

00005ad4 <sha256_value>:
    5ad4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5ad8:	e59f3028 	ldr	r3, [pc, #40]	; 5b08 <sha256_value+0x34>
    5adc:	e24dd004 	sub	sp, sp, #4	; 0x4
    5ae0:	e893000c 	ldm	r3, {r2, r3}
    5ae4:	e1823003 	orr	r3, r2, r3
    5ae8:	e3530000 	cmp	r3, #0	; 0x0
    5aec:	13e00000 	mvnne	r0, #0	; 0x0
    5af0:	1a000002 	bne	5b00 <sha256_value+0x2c>
    5af4:	eb000096 	bl	5d54 <hash_module_get_result>
    5af8:	e3500000 	cmp	r0, #0	; 0x0
    5afc:	13e00001 	mvnne	r0, #1	; 0x1
    5b00:	e28dd004 	add	sp, sp, #4	; 0x4
    5b04:	e8bd8000 	pop	{pc}
    5b08:	0800d218 	.word	0x0800d218

00005b0c <sha256_data>:
    5b0c:	e59f305c 	ldr	r3, [pc, #92]	; 5b70 <sha256_data+0x64>
    5b10:	e92d4070 	push	{r4, r5, r6, lr}
    5b14:	e1a06001 	mov	r6, r1
    5b18:	e5931004 	ldr	r1, [r3, #4]
    5b1c:	e1a04006 	mov	r4, r6
    5b20:	e3a05000 	mov	r5, #0	; 0x0
    5b24:	e1550001 	cmp	r5, r1
    5b28:	e593c000 	ldr	ip, [r3]
    5b2c:	8a000002 	bhi	5b3c <sha256_data+0x30>
    5b30:	1a000003 	bne	5b44 <sha256_data+0x38>
    5b34:	e154000c 	cmp	r4, ip
    5b38:	9a000001 	bls	5b44 <sha256_data+0x38>
    5b3c:	e3e00000 	mvn	r0, #0	; 0x0
    5b40:	e8bd8070 	pop	{r4, r5, r6, pc}
    5b44:	e1a0200c 	mov	r2, ip
    5b48:	e1a03001 	mov	r3, r1
    5b4c:	e59fc01c 	ldr	ip, [pc, #28]	; 5b70 <sha256_data+0x64>
    5b50:	e0522004 	subs	r2, r2, r4
    5b54:	e0c33005 	sbc	r3, r3, r5
    5b58:	e1a01006 	mov	r1, r6
    5b5c:	e88c000c 	stm	ip, {r2, r3}
    5b60:	eb0000de 	bl	5ee0 <hash_dma_transfer>
    5b64:	e3500000 	cmp	r0, #0	; 0x0
    5b68:	13e00001 	mvnne	r0, #1	; 0x1
    5b6c:	e8bd8070 	pop	{r4, r5, r6, pc}
    5b70:	0800d218 	.word	0x0800d218

00005b74 <sha256_init>:
    5b74:	e1a03ea0 	lsr	r3, r0, #29
    5b78:	e1a0c180 	lsl	ip, r0, #3
    5b7c:	e92d4010 	push	{r4, lr}
    5b80:	e59f403c 	ldr	r4, [pc, #60]	; 5bc4 <sha256_init+0x50>
    5b84:	e1a0200c 	mov	r2, ip
    5b88:	e1a01003 	mov	r1, r3
    5b8c:	e3a00003 	mov	r0, #3	; 0x3
    5b90:	e5843004 	str	r3, [r4, #4]
    5b94:	e584c000 	str	ip, [r4]
    5b98:	eb0000b4 	bl	5e70 <hash_module_init>
    5b9c:	e3500000 	cmp	r0, #0	; 0x0
    5ba0:	05942000 	ldreq	r2, [r4]
    5ba4:	05943004 	ldreq	r3, [r4, #4]
    5ba8:	01a021a2 	lsreq	r2, r2, #3
    5bac:	01822e83 	orreq	r2, r2, r3, lsl #29
    5bb0:	01a031a3 	lsreq	r3, r3, #3
    5bb4:	13e00000 	mvnne	r0, #0	; 0x0
    5bb8:	05843004 	streq	r3, [r4, #4]
    5bbc:	05842000 	streq	r2, [r4]
    5bc0:	e8bd8010 	pop	{r4, pc}
    5bc4:	0800d218 	.word	0x0800d218

00005bc8 <crc32_init>:
    5bc8:	e59f300c 	ldr	r3, [pc, #12]	; 5bdc <crc32_init+0x14>
    5bcc:	e3e02000 	mvn	r2, #0	; 0x0
    5bd0:	e5832000 	str	r2, [r3]
    5bd4:	e3a00000 	mov	r0, #0	; 0x0
    5bd8:	e12fff1e 	bx	lr
    5bdc:	0800d220 	.word	0x0800d220

00005be0 <crc32_data>:
    5be0:	e59f311c 	ldr	r3, [pc, #284]	; 5d04 <crc32_data+0x124>
    5be4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    5be8:	e593c000 	ldr	ip, [r3]
    5bec:	e1a08000 	mov	r8, r0
    5bf0:	e1a06001 	mov	r6, r1
    5bf4:	e1a04000 	mov	r4, r0
    5bf8:	e1a07001 	mov	r7, r1
    5bfc:	e59f5104 	ldr	r5, [pc, #260]	; 5d08 <crc32_data+0x128>
    5c00:	ea000027 	b	5ca4 <crc32_data+0xc4>
    5c04:	e5543008 	ldrb	r3, [r4, #-8]
    5c08:	e5542007 	ldrb	r2, [r4, #-7]
    5c0c:	e02c3003 	eor	r3, ip, r3
    5c10:	e20330ff 	and	r3, r3, #255	; 0xff
    5c14:	e7953103 	ldr	r3, [r5, r3, lsl #2]
    5c18:	e5541006 	ldrb	r1, [r4, #-6]
    5c1c:	e023342c 	eor	r3, r3, ip, lsr #8
    5c20:	e0232002 	eor	r2, r3, r2
    5c24:	e20220ff 	and	r2, r2, #255	; 0xff
    5c28:	e7952102 	ldr	r2, [r5, r2, lsl #2]
    5c2c:	e5540005 	ldrb	r0, [r4, #-5]
    5c30:	e0222423 	eor	r2, r2, r3, lsr #8
    5c34:	e0221001 	eor	r1, r2, r1
    5c38:	e20110ff 	and	r1, r1, #255	; 0xff
    5c3c:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    5c40:	e5541004 	ldrb	r1, [r4, #-4]
    5c44:	e0233422 	eor	r3, r3, r2, lsr #8
    5c48:	e0230000 	eor	r0, r3, r0
    5c4c:	e20000ff 	and	r0, r0, #255	; 0xff
    5c50:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    5c54:	e5540003 	ldrb	r0, [r4, #-3]
    5c58:	e0222423 	eor	r2, r2, r3, lsr #8
    5c5c:	e0221001 	eor	r1, r2, r1
    5c60:	e20110ff 	and	r1, r1, #255	; 0xff
    5c64:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    5c68:	e554c002 	ldrb	ip, [r4, #-2]
    5c6c:	e0233422 	eor	r3, r3, r2, lsr #8
    5c70:	e0230000 	eor	r0, r3, r0
    5c74:	e20000ff 	and	r0, r0, #255	; 0xff
    5c78:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    5c7c:	e5541001 	ldrb	r1, [r4, #-1]
    5c80:	e0222423 	eor	r2, r2, r3, lsr #8
    5c84:	e022c00c 	eor	ip, r2, ip
    5c88:	e20cc0ff 	and	ip, ip, #255	; 0xff
    5c8c:	e795310c 	ldr	r3, [r5, ip, lsl #2]
    5c90:	e0233422 	eor	r3, r3, r2, lsr #8
    5c94:	e0231001 	eor	r1, r3, r1
    5c98:	e20110ff 	and	r1, r1, #255	; 0xff
    5c9c:	e7952101 	ldr	r2, [r5, r1, lsl #2]
    5ca0:	e022c423 	eor	ip, r2, r3, lsr #8
    5ca4:	e3570007 	cmp	r7, #7	; 0x7
    5ca8:	e2844008 	add	r4, r4, #8	; 0x8
    5cac:	e2477008 	sub	r7, r7, #8	; 0x8
    5cb0:	8affffd3 	bhi	5c04 <crc32_data+0x24>
    5cb4:	e3c63007 	bic	r3, r6, #7	; 0x7
    5cb8:	e0880003 	add	r0, r8, r3
    5cbc:	e59f3040 	ldr	r3, [pc, #64]	; 5d04 <crc32_data+0x124>
    5cc0:	e2161007 	ands	r1, r6, #7	; 0x7
    5cc4:	e583c000 	str	ip, [r3]
    5cc8:	0a00000b 	beq	5cfc <crc32_data+0x11c>
    5ccc:	e59f4034 	ldr	r4, [pc, #52]	; 5d08 <crc32_data+0x128>
    5cd0:	e3a02000 	mov	r2, #0	; 0x0
    5cd4:	e7d03002 	ldrb	r3, [r0, r2]
    5cd8:	e2822001 	add	r2, r2, #1	; 0x1
    5cdc:	e02c3003 	eor	r3, ip, r3
    5ce0:	e20330ff 	and	r3, r3, #255	; 0xff
    5ce4:	e7943103 	ldr	r3, [r4, r3, lsl #2]
    5ce8:	e1510002 	cmp	r1, r2
    5cec:	e023c42c 	eor	ip, r3, ip, lsr #8
    5cf0:	1afffff7 	bne	5cd4 <crc32_data+0xf4>
    5cf4:	e59f3008 	ldr	r3, [pc, #8]	; 5d04 <crc32_data+0x124>
    5cf8:	e583c000 	str	ip, [r3]
    5cfc:	e3a00000 	mov	r0, #0	; 0x0
    5d00:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5d04:	0800d220 	.word	0x0800d220
    5d08:	0001118c 	.word	0x0001118c

00005d0c <crc32_value>:
    5d0c:	e59f3010 	ldr	r3, [pc, #16]	; 5d24 <crc32_value+0x18>
    5d10:	e5933000 	ldr	r3, [r3]
    5d14:	e1e03003 	mvn	r3, r3
    5d18:	e5803000 	str	r3, [r0]
    5d1c:	e3a00000 	mov	r0, #0	; 0x0
    5d20:	e12fff1e 	bx	lr
    5d24:	0800d220 	.word	0x0800d220

00005d28 <crc32_le>:
    5d28:	e92d4010 	push	{r4, lr}
    5d2c:	e59f401c 	ldr	r4, [pc, #28]	; 5d50 <crc32_le+0x28>
    5d30:	e3e03000 	mvn	r3, #0	; 0x0
    5d34:	e1a00001 	mov	r0, r1
    5d38:	e1a01002 	mov	r1, r2
    5d3c:	e5843000 	str	r3, [r4]
    5d40:	ebffffa6 	bl	5be0 <crc32_data>
    5d44:	e5940000 	ldr	r0, [r4]
    5d48:	e1e00000 	mvn	r0, r0
    5d4c:	e8bd8010 	pop	{r4, pc}
    5d50:	0800d220 	.word	0x0800d220

00005d54 <hash_module_get_result>:
    5d54:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    5d58:	e1a08000 	mov	r8, r0
    5d5c:	eb000aaa 	bl	880c <get_ticks>
    5d60:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    5d64:	e59f3088 	ldr	r3, [pc, #136]	; 5df4 <hash_module_get_result+0xa0>
    5d68:	e3a05000 	mov	r5, #0	; 0x0
    5d6c:	e0944000 	adds	r4, r4, r0
    5d70:	e0a55001 	adc	r5, r5, r1
    5d74:	e1a07003 	mov	r7, r3
    5d78:	e593680c 	ldr	r6, [r3, #2060]
    5d7c:	ea000008 	b	5da4 <hash_module_get_result+0x50>
    5d80:	e597680c 	ldr	r6, [r7, #2060]
    5d84:	eb000aa0 	bl	880c <get_ticks>
    5d88:	e1510005 	cmp	r1, r5
    5d8c:	8a000002 	bhi	5d9c <hash_module_get_result+0x48>
    5d90:	1a000003 	bne	5da4 <hash_module_get_result+0x50>
    5d94:	e1500004 	cmp	r0, r4
    5d98:	9a000001 	bls	5da4 <hash_module_get_result+0x50>
    5d9c:	e3a00001 	mov	r0, #1	; 0x1
    5da0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5da4:	e3160001 	tst	r6, #1	; 0x1
    5da8:	0afffff4 	beq	5d80 <hash_module_get_result+0x2c>
    5dac:	e59f0044 	ldr	r0, [pc, #68]	; 5df8 <hash_module_get_result+0xa4>
    5db0:	e59fc044 	ldr	ip, [pc, #68]	; 5dfc <hash_module_get_result+0xa8>
    5db4:	e1a03000 	mov	r3, r0
    5db8:	e6931008 	ldr	r1, [r3], r8
    5dbc:	e59f203c 	ldr	r2, [pc, #60]	; 5e00 <hash_module_get_result+0xac>
    5dc0:	e2800004 	add	r0, r0, #4	; 0x4
    5dc4:	e0832002 	add	r2, r3, r2
    5dc8:	e150000c 	cmp	r0, ip
    5dcc:	e5821000 	str	r1, [r2]
    5dd0:	1afffff7 	bne	5db4 <hash_module_get_result+0x60>
    5dd4:	e59f2018 	ldr	r2, [pc, #24]	; 5df4 <hash_module_get_result+0xa0>
    5dd8:	e3a03001 	mov	r3, #1	; 0x1
    5ddc:	e582380c 	str	r3, [r2, #2060]
    5de0:	e5923800 	ldr	r3, [r2, #2048]
    5de4:	e3a00000 	mov	r0, #0	; 0x0
    5de8:	e3c33080 	bic	r3, r3, #128	; 0x80
    5dec:	e5823800 	str	r3, [r2, #2048]
    5df0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    5df4:	27700000 	.word	0x27700000
    5df8:	27700858 	.word	0x27700858
    5dfc:	27700878 	.word	0x27700878
    5e00:	d88ff7a8 	.word	0xd88ff7a8

00005e04 <hash_module_clear>:
    5e04:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5e08:	e59f405c 	ldr	r4, [pc, #92]	; 5e6c <hash_module_clear+0x68>
    5e0c:	e3a03002 	mov	r3, #2	; 0x2
    5e10:	e5843800 	str	r3, [r4, #2048]
    5e14:	e24dd004 	sub	sp, sp, #4	; 0x4
    5e18:	eb000a7b 	bl	880c <get_ticks>
    5e1c:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    5e20:	e3a07000 	mov	r7, #0	; 0x0
    5e24:	e0966000 	adds	r6, r6, r0
    5e28:	e1a05004 	mov	r5, r4
    5e2c:	e0a77001 	adc	r7, r7, r1
    5e30:	e5944800 	ldr	r4, [r4, #2048]
    5e34:	ea000008 	b	5e5c <hash_module_clear+0x58>
    5e38:	e5954800 	ldr	r4, [r5, #2048]
    5e3c:	eb000a72 	bl	880c <get_ticks>
    5e40:	e1510007 	cmp	r1, r7
    5e44:	8a000002 	bhi	5e54 <hash_module_clear+0x50>
    5e48:	1a000003 	bne	5e5c <hash_module_clear+0x58>
    5e4c:	e1500006 	cmp	r0, r6
    5e50:	9a000001 	bls	5e5c <hash_module_clear+0x58>
    5e54:	e3a00001 	mov	r0, #1	; 0x1
    5e58:	ea000001 	b	5e64 <hash_module_clear+0x60>
    5e5c:	e2140002 	ands	r0, r4, #2	; 0x2
    5e60:	1afffff4 	bne	5e38 <hash_module_clear+0x34>
    5e64:	e28dd004 	add	sp, sp, #4	; 0x4
    5e68:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5e6c:	27700000 	.word	0x27700000

00005e70 <hash_module_init>:
    5e70:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5e74:	e59f4060 	ldr	r4, [pc, #96]	; 5edc <hash_module_init+0x6c>
    5e78:	e24dd004 	sub	sp, sp, #4	; 0x4
    5e7c:	e5943800 	ldr	r3, [r4, #2048]
    5e80:	e1a05000 	mov	r5, r0
    5e84:	e3c33080 	bic	r3, r3, #128	; 0x80
    5e88:	e5843800 	str	r3, [r4, #2048]
    5e8c:	e1a07001 	mov	r7, r1
    5e90:	e1a06002 	mov	r6, r2
    5e94:	ebffffda 	bl	5e04 <hash_module_clear>
    5e98:	e3500000 	cmp	r0, #0	; 0x0
    5e9c:	13a00001 	movne	r0, #1	; 0x1
    5ea0:	1a00000b 	bne	5ed4 <hash_module_init+0x64>
    5ea4:	e3a03001 	mov	r3, #1	; 0x1
    5ea8:	e1a02185 	lsl	r2, r5, #3
    5eac:	e5843804 	str	r3, [r4, #2052]
    5eb0:	e5842800 	str	r2, [r4, #2048]
    5eb4:	e5847814 	str	r7, [r4, #2068]
    5eb8:	e5846810 	str	r6, [r4, #2064]
    5ebc:	e5943800 	ldr	r3, [r4, #2048]
    5ec0:	e3833080 	orr	r3, r3, #128	; 0x80
    5ec4:	e5843800 	str	r3, [r4, #2048]
    5ec8:	e5943800 	ldr	r3, [r4, #2048]
    5ecc:	e3833004 	orr	r3, r3, #4	; 0x4
    5ed0:	e5843800 	str	r3, [r4, #2048]
    5ed4:	e28dd004 	add	sp, sp, #4	; 0x4
    5ed8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5edc:	27700000 	.word	0x27700000

00005ee0 <hash_dma_transfer>:
    5ee0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    5ee4:	e1a08001 	mov	r8, r1
    5ee8:	e24dd004 	sub	sp, sp, #4	; 0x4
    5eec:	e1a09000 	mov	r9, r0
    5ef0:	e1a06001 	mov	r6, r1
    5ef4:	e59fa090 	ldr	sl, [pc, #144]	; 5f8c <hash_dma_transfer+0xac>
    5ef8:	e3a0b000 	mov	fp, #0	; 0x0
    5efc:	ea00001b 	b	5f70 <hash_dma_transfer+0x90>
    5f00:	e3560902 	cmp	r6, #32768	; 0x8000
    5f04:	b1a07006 	movlt	r7, r6
    5f08:	a3a07902 	movge	r7, #32768	; 0x8000
    5f0c:	e0661008 	rsb	r1, r6, r8
    5f10:	e0891001 	add	r1, r9, r1
    5f14:	e3a00002 	mov	r0, #2	; 0x2
    5f18:	e1a02007 	mov	r2, r7
    5f1c:	ebfffe60 	bl	58a4 <dma_sdma_init>
    5f20:	e3500000 	cmp	r0, #0	; 0x0
    5f24:	1a000015 	bne	5f80 <hash_dma_transfer+0xa0>
    5f28:	e2800002 	add	r0, r0, #2	; 0x2
    5f2c:	ebfffe4c 	bl	5864 <dma_start>
    5f30:	eb000a35 	bl	880c <get_ticks>
    5f34:	e090400a 	adds	r4, r0, sl
    5f38:	e0a1500b 	adc	r5, r1, fp
    5f3c:	ea000005 	b	5f58 <hash_dma_transfer+0x78>
    5f40:	eb000a31 	bl	880c <get_ticks>
    5f44:	e1510005 	cmp	r1, r5
    5f48:	8a00000c 	bhi	5f80 <hash_dma_transfer+0xa0>
    5f4c:	1a000001 	bne	5f58 <hash_dma_transfer+0x78>
    5f50:	e1500004 	cmp	r0, r4
    5f54:	8a000009 	bhi	5f80 <hash_dma_transfer+0xa0>
    5f58:	e3a01003 	mov	r1, #3	; 0x3
    5f5c:	e3a00002 	mov	r0, #2	; 0x2
    5f60:	ebfffe45 	bl	587c <dma_get_status>
    5f64:	e3500000 	cmp	r0, #0	; 0x0
    5f68:	0afffff4 	beq	5f40 <hash_dma_transfer+0x60>
    5f6c:	e0676006 	rsb	r6, r7, r6
    5f70:	e3560000 	cmp	r6, #0	; 0x0
    5f74:	1affffe1 	bne	5f00 <hash_dma_transfer+0x20>
    5f78:	e1a00006 	mov	r0, r6
    5f7c:	ea000000 	b	5f84 <hash_dma_transfer+0xa4>
    5f80:	e3a00001 	mov	r0, #1	; 0x1
    5f84:	e28dd004 	add	sp, sp, #4	; 0x4
    5f88:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5f8c:	000f4240 	.word	0x000f4240

00005f90 <cdbg_log_toggle>:
    5f90:	e59f3014 	ldr	r3, [pc, #20]	; 5fac <cdbg_log_toggle+0x1c>
    5f94:	e3500000 	cmp	r0, #0	; 0x0
    5f98:	e59f2010 	ldr	r2, [pc, #16]	; 5fb0 <cdbg_log_toggle+0x20>
    5f9c:	01a02003 	moveq	r2, r3
    5fa0:	e59f300c 	ldr	r3, [pc, #12]	; 5fb4 <cdbg_log_toggle+0x24>
    5fa4:	e5832000 	str	r2, [r3]
    5fa8:	e12fff1e 	bx	lr
    5fac:	0000e054 	.word	0x0000e054
    5fb0:	0000e0d8 	.word	0x0000e0d8
    5fb4:	0800d76c 	.word	0x0800d76c

00005fb8 <cdbg_wipe_bootarea>:
    5fb8:	e59f3018 	ldr	r3, [pc, #24]	; 5fd8 <cdbg_wipe_bootarea+0x20>
    5fbc:	e59f1018 	ldr	r1, [pc, #24]	; 5fdc <cdbg_wipe_bootarea+0x24>
    5fc0:	e59f2018 	ldr	r2, [pc, #24]	; 5fe0 <cdbg_wipe_bootarea+0x28>
    5fc4:	e5031004 	str	r1, [r3, #-4]
    5fc8:	e2833004 	add	r3, r3, #4	; 0x4
    5fcc:	e1530002 	cmp	r3, r2
    5fd0:	1afffffb 	bne	5fc4 <cdbg_wipe_bootarea+0xc>
    5fd4:	e12fff1e 	bx	lr
    5fd8:	08000004 	.word	0x08000004
    5fdc:	eafffffe 	.word	0xeafffffe
    5fe0:	08008004 	.word	0x08008004

00005fe4 <cdbg_verify_burn>:
    5fe4:	e59f3004 	ldr	r3, [pc, #4]	; 5ff0 <cdbg_verify_burn+0xc>
    5fe8:	e5930000 	ldr	r0, [r3]
    5fec:	e12fff1e 	bx	lr
    5ff0:	0800d224 	.word	0x0800d224

00005ff4 <cdbg_verify_burn_key>:
    5ff4:	e59f0000 	ldr	r0, [pc, #0]	; 5ffc <cdbg_verify_burn_key+0x8>
    5ff8:	e12fff1e 	bx	lr
    5ffc:	0800d228 	.word	0x0800d228

00006000 <cdbg_verify_burn_enable>:
    6000:	e3500000 	cmp	r0, #0	; 0x0
    6004:	e59f3018 	ldr	r3, [pc, #24]	; 6024 <cdbg_verify_burn_enable+0x24>
    6008:	03a02000 	moveq	r2, #0	; 0x0
    600c:	13a02001 	movne	r2, #1	; 0x1
    6010:	e5832000 	str	r2, [r3]
    6014:	012fff1e 	bxeq	lr
    6018:	e59f0008 	ldr	r0, [pc, #8]	; 6028 <cdbg_verify_burn_enable+0x28>
    601c:	e3a02020 	mov	r2, #32	; 0x20
    6020:	eaffecc7 	b	1344 <memcpy>
    6024:	0800d224 	.word	0x0800d224
    6028:	0800d228 	.word	0x0800d228

0000602c <cdbg_jump>:
    602c:	e92d4030 	push	{r4, r5, lr}
    6030:	e1a04000 	mov	r4, r0
    6034:	e24dd004 	sub	sp, sp, #4	; 0x4
    6038:	e1a05001 	mov	r5, r1
    603c:	ebfffc61 	bl	51c8 <ss_lock_isk>
    6040:	e59f001c 	ldr	r0, [pc, #28]	; 6064 <cdbg_jump+0x38>
    6044:	e1a01004 	mov	r1, r4
    6048:	e59f3018 	ldr	r3, [pc, #24]	; 6068 <cdbg_jump+0x3c>
    604c:	e1a0e00f 	mov	lr, pc
    6050:	e593f000 	ldr	pc, [r3]
    6054:	e3000bb8 	movw	r0, #3000	; 0xbb8
    6058:	eb0009fc 	bl	8850 <udelay>
    605c:	e1a00005 	mov	r0, r5
    6060:	e12fff34 	blx	r4
    6064:	00012f6e 	.word	0x00012f6e
    6068:	0800d76c 	.word	0x0800d76c

0000606c <cdbg_restore_os>:
    606c:	e92d4010 	push	{r4, lr}
    6070:	ebffe9de 	bl	7f0 <get_wakeup_entry>
    6074:	e59f301c 	ldr	r3, [pc, #28]	; 6098 <cdbg_restore_os+0x2c>
    6078:	e1a04000 	mov	r4, r0
    607c:	e59f0018 	ldr	r0, [pc, #24]	; 609c <cdbg_restore_os+0x30>
    6080:	e1a0e00f 	mov	lr, pc
    6084:	e593f000 	ldr	pc, [r3]
    6088:	e1a00004 	mov	r0, r4
    608c:	e3a01000 	mov	r1, #0	; 0x0
    6090:	e8bd4010 	pop	{r4, lr}
    6094:	eaffffe4 	b	602c <cdbg_jump>
    6098:	0800d76c 	.word	0x0800d76c
    609c:	00012f7e 	.word	0x00012f7e

000060a0 <cdbg_get_pp>:
    60a0:	e92d4070 	push	{r4, r5, r6, lr}
    60a4:	e24dd008 	sub	sp, sp, #8	; 0x8
    60a8:	e1a0000d 	mov	r0, sp
    60ac:	e59f103c 	ldr	r1, [pc, #60]	; 60f0 <cdbg_get_pp+0x50>
    60b0:	e3a02008 	mov	r2, #8	; 0x8
    60b4:	ebffeca2 	bl	1344 <memcpy>
    60b8:	e1a0400d 	mov	r4, sp
    60bc:	e1a0600d 	mov	r6, sp
    60c0:	ea000004 	b	60d8 <cdbg_get_pp+0x38>
    60c4:	eb002021 	bl	e150 <getc>
    60c8:	e1500004 	cmp	r0, r4
    60cc:	e2854001 	add	r4, r5, #1	; 0x1
    60d0:	e1a05006 	mov	r5, r6
    60d4:	1a000000 	bne	60dc <cdbg_get_pp+0x3c>
    60d8:	e1a05004 	mov	r5, r4
    60dc:	e5d54000 	ldrb	r4, [r5]
    60e0:	e3540000 	cmp	r4, #0	; 0x0
    60e4:	1afffff6 	bne	60c4 <cdbg_get_pp+0x24>
    60e8:	e28dd008 	add	sp, sp, #8	; 0x8
    60ec:	e8bd8070 	pop	{r4, r5, r6, pc}
    60f0:	00011816 	.word	0x00011816

000060f4 <cdbg_shell>:
    60f4:	e92d4010 	push	{r4, lr}
    60f8:	ebfffc32 	bl	51c8 <ss_lock_isk>
    60fc:	e59f3048 	ldr	r3, [pc, #72]	; 614c <cdbg_shell+0x58>
    6100:	e59f1048 	ldr	r1, [pc, #72]	; 6150 <cdbg_shell+0x5c>
    6104:	e59f2048 	ldr	r2, [pc, #72]	; 6154 <cdbg_shell+0x60>
    6108:	e5031004 	str	r1, [r3, #-4]
    610c:	e2833004 	add	r3, r3, #4	; 0x4
    6110:	e1530002 	cmp	r3, r2
    6114:	1afffffb 	bne	6108 <cdbg_shell+0x14>
    6118:	e3a00001 	mov	r0, #1	; 0x1
    611c:	eb0027e5 	bl	100b8 <ss_jtag_en>
    6120:	e59f4030 	ldr	r4, [pc, #48]	; 6158 <cdbg_shell+0x64>
    6124:	e59f3030 	ldr	r3, [pc, #48]	; 615c <cdbg_shell+0x68>
    6128:	e59f0030 	ldr	r0, [pc, #48]	; 6160 <cdbg_shell+0x6c>
    612c:	e5843000 	str	r3, [r4]
    6130:	eb001fe8 	bl	e0d8 <printf_on>
    6134:	ebffffd9 	bl	60a0 <cdbg_get_pp>
    6138:	e59f0024 	ldr	r0, [pc, #36]	; 6164 <cdbg_shell+0x70>
    613c:	e1a0e00f 	mov	lr, pc
    6140:	e594f000 	ldr	pc, [r4]
    6144:	eb0022ec 	bl	ecfc <main_loop>
    6148:	eafffffd 	b	6144 <cdbg_shell+0x50>
    614c:	08000004 	.word	0x08000004
    6150:	eafffffe 	.word	0xeafffffe
    6154:	08008004 	.word	0x08008004
    6158:	0800d76c 	.word	0x0800d76c
    615c:	0000e0d8 	.word	0x0000e0d8
    6160:	00012f8d 	.word	0x00012f8d
    6164:	00012f92 	.word	0x00012f92

00006168 <cdbg_do_config>:
    6168:	e92d4030 	push	{r4, r5, lr}
    616c:	e59f307c 	ldr	r3, [pc, #124]	; 61f0 <cdbg_do_config+0x88>
    6170:	e24dd004 	sub	sp, sp, #4	; 0x4
    6174:	e1a04000 	mov	r4, r0
    6178:	e1a05001 	mov	r5, r1
    617c:	e59f0070 	ldr	r0, [pc, #112]	; 61f4 <cdbg_do_config+0x8c>
    6180:	e59f1070 	ldr	r1, [pc, #112]	; 61f8 <cdbg_do_config+0x90>
    6184:	e1a0e00f 	mov	lr, pc
    6188:	e593f000 	ldr	pc, [r3]
    618c:	e3a02000 	mov	r2, #0	; 0x0
    6190:	e59f0060 	ldr	r0, [pc, #96]	; 61f8 <cdbg_do_config+0x90>
    6194:	ea000006 	b	61b4 <cdbg_do_config+0x4c>
    6198:	e7943002 	ldr	r3, [r4, r2]
    619c:	e2822004 	add	r2, r2, #4	; 0x4
    61a0:	e1530000 	cmp	r3, r0
    61a4:	1a000002 	bne	61b4 <cdbg_do_config+0x4c>
    61a8:	e3510000 	cmp	r1, #0	; 0x0
    61ac:	1a000004 	bne	61c4 <cdbg_do_config+0x5c>
    61b0:	ea000007 	b	61d4 <cdbg_do_config+0x6c>
    61b4:	e1520005 	cmp	r2, r5
    61b8:	e0841002 	add	r1, r4, r2
    61bc:	bafffff5 	blt	6198 <cdbg_do_config+0x30>
    61c0:	ea000003 	b	61d4 <cdbg_do_config+0x6c>
    61c4:	e5910004 	ldr	r0, [r1, #4]
    61c8:	eb000c10 	bl	9210 <spi_set_bytes>
    61cc:	e3a00000 	mov	r0, #0	; 0x0
    61d0:	ea000004 	b	61e8 <cdbg_do_config+0x80>
    61d4:	e59f0020 	ldr	r0, [pc, #32]	; 61fc <cdbg_do_config+0x94>
    61d8:	e59f3010 	ldr	r3, [pc, #16]	; 61f0 <cdbg_do_config+0x88>
    61dc:	e1a0e00f 	mov	lr, pc
    61e0:	e593f000 	ldr	pc, [r3]
    61e4:	e3e00000 	mvn	r0, #0	; 0x0
    61e8:	e28dd004 	add	sp, sp, #4	; 0x4
    61ec:	e8bd8030 	pop	{r4, r5, pc}
    61f0:	0800d76c 	.word	0x0800d76c
    61f4:	00012fa3 	.word	0x00012fa3
    61f8:	4a82467c 	.word	0x4a82467c
    61fc:	00012fc0 	.word	0x00012fc0

00006200 <cdbg_config_isi>:
    6200:	e92d4030 	push	{r4, r5, lr}
    6204:	e24dd004 	sub	sp, sp, #4	; 0x4
    6208:	e1a04000 	mov	r4, r0
    620c:	ebffee0f 	bl	1a50 <isi_check>
    6210:	e2505000 	subs	r5, r0, #0	; 0x0
    6214:	0a000007 	beq	6238 <cdbg_config_isi+0x38>
    6218:	e59f002c 	ldr	r0, [pc, #44]	; 624c <cdbg_config_isi+0x4c>
    621c:	e1a01005 	mov	r1, r5
    6220:	e59f3028 	ldr	r3, [pc, #40]	; 6250 <cdbg_config_isi+0x50>
    6224:	e1a0e00f 	mov	lr, pc
    6228:	e593f000 	ldr	pc, [r3]
    622c:	e1a00005 	mov	r0, r5
    6230:	e28dd004 	add	sp, sp, #4	; 0x4
    6234:	e8bd8030 	pop	{r4, r5, pc}
    6238:	e594100c 	ldr	r1, [r4, #12]
    623c:	e2840c02 	add	r0, r4, #512	; 0x200
    6240:	e28dd004 	add	sp, sp, #4	; 0x4
    6244:	e8bd4030 	pop	{r4, r5, lr}
    6248:	eaffffc6 	b	6168 <cdbg_do_config>
    624c:	00012fdf 	.word	0x00012fdf
    6250:	0800d76c 	.word	0x0800d76c

00006254 <burn_image_loop>:
    6254:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6258:	e24dd024 	sub	sp, sp, #36	; 0x24
    625c:	e59d704c 	ldr	r7, [sp, #76]
    6260:	e1a06000 	mov	r6, r0
    6264:	e3570000 	cmp	r7, #0	; 0x0
    6268:	e58d1014 	str	r1, [sp, #20]
    626c:	e58d2010 	str	r2, [sp, #16]
    6270:	e58d300c 	str	r3, [sp, #12]
    6274:	1a00000c 	bne	62ac <burn_image_loop+0x58>
    6278:	e59f426c 	ldr	r4, [pc, #620]	; 64ec <burn_image_loop+0x298>
    627c:	e59f026c 	ldr	r0, [pc, #620]	; 64f0 <burn_image_loop+0x29c>
    6280:	e3a0102e 	mov	r1, #46	; 0x2e
    6284:	e1a0e00f 	mov	lr, pc
    6288:	e594f000 	ldr	pc, [r4]
    628c:	e59f0260 	ldr	r0, [pc, #608]	; 64f4 <burn_image_loop+0x2a0>
    6290:	e59f1260 	ldr	r1, [pc, #608]	; 64f8 <burn_image_loop+0x2a4>
    6294:	e1a0e00f 	mov	lr, pc
    6298:	e594f000 	ldr	pc, [r4]
    629c:	e59f0258 	ldr	r0, [pc, #600]	; 64fc <burn_image_loop+0x2a8>
    62a0:	e1a0e00f 	mov	lr, pc
    62a4:	e594f000 	ldr	pc, [r4]
    62a8:	eafffffe 	b	62a8 <burn_image_loop+0x54>
    62ac:	e59d0014 	ldr	r0, [sp, #20]
    62b0:	e59d100c 	ldr	r1, [sp, #12]
    62b4:	e1500001 	cmp	r0, r1
    62b8:	1a000005 	bne	62d4 <burn_image_loop+0x80>
    62bc:	e59f023c 	ldr	r0, [pc, #572]	; 6500 <burn_image_loop+0x2ac>
    62c0:	e59f3224 	ldr	r3, [pc, #548]	; 64ec <burn_image_loop+0x298>
    62c4:	e1a0e00f 	mov	lr, pc
    62c8:	e593f000 	ldr	pc, [r3]
    62cc:	e3e00001 	mvn	r0, #1	; 0x1
    62d0:	ea000083 	b	64e4 <burn_image_loop+0x290>
    62d4:	e596c000 	ldr	ip, [r6]
    62d8:	e3a03000 	mov	r3, #0	; 0x0
    62dc:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    62e0:	e002200c 	and	r2, r2, ip
    62e4:	e1a01483 	lsl	r1, r3, #9
    62e8:	e1d600b8 	ldrh	r0, [r6, #8]
    62ec:	e1811ba2 	orr	r1, r1, r2, lsr #23
    62f0:	e1a0e482 	lsl	lr, r2, #9
    62f4:	e1a03001 	mov	r3, r1
    62f8:	e3a02401 	mov	r2, #16777216	; 0x1000000
    62fc:	e5961004 	ldr	r1, [r6, #4]
    6300:	e1a0cc2c 	lsr	ip, ip, #24
    6304:	e1a05000 	mov	r5, r0
    6308:	e09e8002 	adds	r8, lr, r2
    630c:	e1a09003 	mov	r9, r3
    6310:	e3a04000 	mov	r4, #0	; 0x0
    6314:	e3a03000 	mov	r3, #0	; 0x0
    6318:	e20cc03f 	and	ip, ip, #63	; 0x3f
    631c:	e59d000c 	ldr	r0, [sp, #12]
    6320:	e0a99003 	adc	r9, r9, r3
    6324:	e184a001 	orr	sl, r4, r1
    6328:	e1a0b005 	mov	fp, r5
    632c:	e58dc018 	str	ip, [sp, #24]
    6330:	e596500c 	ldr	r5, [r6, #12]
    6334:	ebfffa75 	bl	4d10 <vs_device_erasable>
    6338:	e1500004 	cmp	r0, r4
    633c:	0a000006 	beq	635c <burn_image_loop+0x108>
    6340:	e1a01004 	mov	r1, r4
    6344:	e59d000c 	ldr	r0, [sp, #12]
    6348:	ebfffb58 	bl	50b0 <vs_assign_by_id>
    634c:	e1a0000a 	mov	r0, sl
    6350:	e1a0100b 	mov	r1, fp
    6354:	e59d2048 	ldr	r2, [sp, #72]
    6358:	ebfff9c3 	bl	4a6c <vs_erase>
    635c:	e59d0014 	ldr	r0, [sp, #20]
    6360:	e3a01001 	mov	r1, #1	; 0x1
    6364:	ebfffb51 	bl	50b0 <vs_assign_by_id>
    6368:	e59d2018 	ldr	r2, [sp, #24]
    636c:	e3520002 	cmp	r2, #2	; 0x2
    6370:	1a000019 	bne	63dc <burn_image_loop+0x188>
    6374:	e3a01c02 	mov	r1, #512	; 0x200
    6378:	e58d1000 	str	r1, [sp]
    637c:	e59d1010 	ldr	r1, [sp, #16]
    6380:	e1a02008 	mov	r2, r8
    6384:	e1a03009 	mov	r3, r9
    6388:	e1a00007 	mov	r0, r7
    638c:	e58d1004 	str	r1, [sp, #4]
    6390:	ebfffa9f 	bl	4e14 <vs_read>
    6394:	e1a00007 	mov	r0, r7
    6398:	ebffed51 	bl	18e4 <isi_check_header>
    639c:	e2504000 	subs	r4, r0, #0	; 0x0
    63a0:	1a00000d 	bne	63dc <burn_image_loop+0x188>
    63a4:	e5973004 	ldr	r3, [r7, #4]
    63a8:	e1b03e23 	lsrs	r3, r3, #28
    63ac:	0a00000a 	beq	63dc <burn_image_loop+0x188>
    63b0:	e59f014c 	ldr	r0, [pc, #332]	; 6504 <burn_image_loop+0x2b0>
    63b4:	e59f3130 	ldr	r3, [pc, #304]	; 64ec <burn_image_loop+0x298>
    63b8:	e1a0e00f 	mov	lr, pc
    63bc:	e593f000 	ldr	pc, [r3]
    63c0:	e59f2140 	ldr	r2, [pc, #320]	; 6508 <burn_image_loop+0x2b4>
    63c4:	e1a01004 	mov	r1, r4
    63c8:	e3a00001 	mov	r0, #1	; 0x1
    63cc:	ebfffb98 	bl	5234 <ss_aes_init>
    63d0:	e3a02001 	mov	r2, #1	; 0x1
    63d4:	e58d201c 	str	r2, [sp, #28]
    63d8:	ea000001 	b	63e4 <burn_image_loop+0x190>
    63dc:	e3a03000 	mov	r3, #0	; 0x0
    63e0:	e58d301c 	str	r3, [sp, #28]
    63e4:	e59f4100 	ldr	r4, [pc, #256]	; 64ec <burn_image_loop+0x298>
    63e8:	e59d1048 	ldr	r1, [sp, #72]
    63ec:	e59f0118 	ldr	r0, [pc, #280]	; 650c <burn_image_loop+0x2b8>
    63f0:	e3a02901 	mov	r2, #16384	; 0x4000
    63f4:	e1a0e00f 	mov	lr, pc
    63f8:	e594f000 	ldr	pc, [r4]
    63fc:	e3a06901 	mov	r6, #16384	; 0x4000
    6400:	e1a04005 	mov	r4, r5
    6404:	ea000032 	b	64d4 <burn_image_loop+0x280>
    6408:	e1b03a23 	lsrs	r3, r3, #20
    640c:	0a000005 	beq	6428 <burn_image_loop+0x1d4>
    6410:	e1a01a25 	lsr	r1, r5, #20
    6414:	e59f00f4 	ldr	r0, [pc, #244]	; 6510 <burn_image_loop+0x2bc>
    6418:	e59f20cc 	ldr	r2, [pc, #204]	; 64ec <burn_image_loop+0x298>
    641c:	e1a0e00f 	mov	lr, pc
    6420:	e592f000 	ldr	pc, [r2]
    6424:	e1a04005 	mov	r4, r5
    6428:	e3a01000 	mov	r1, #0	; 0x0
    642c:	e59d0014 	ldr	r0, [sp, #20]
    6430:	ebfffb1e 	bl	50b0 <vs_assign_by_id>
    6434:	e59d1010 	ldr	r1, [sp, #16]
    6438:	e1a02008 	mov	r2, r8
    643c:	e1a03009 	mov	r3, r9
    6440:	e1a00007 	mov	r0, r7
    6444:	e58d6000 	str	r6, [sp]
    6448:	e58d1004 	str	r1, [sp, #4]
    644c:	ebfffa70 	bl	4e14 <vs_read>
    6450:	e59d201c 	ldr	r2, [sp, #28]
    6454:	e3520000 	cmp	r2, #0	; 0x0
    6458:	0a000003 	beq	646c <burn_image_loop+0x218>
    645c:	e1a01007 	mov	r1, r7
    6460:	e1a02006 	mov	r2, r6
    6464:	e1a00007 	mov	r0, r7
    6468:	ebfffb60 	bl	51f0 <ss_aes_data>
    646c:	e3a01000 	mov	r1, #0	; 0x0
    6470:	e59d000c 	ldr	r0, [sp, #12]
    6474:	ebfffb0d 	bl	50b0 <vs_assign_by_id>
    6478:	e2501000 	subs	r1, r0, #0	; 0x0
    647c:	1a000005 	bne	6498 <burn_image_loop+0x244>
    6480:	e1a00007 	mov	r0, r7
    6484:	e1a0200a 	mov	r2, sl
    6488:	e1a0300b 	mov	r3, fp
    648c:	e58d1004 	str	r1, [sp, #4]
    6490:	e58d6000 	str	r6, [sp]
    6494:	ebfff95e 	bl	4a14 <vs_write>
    6498:	e59d0018 	ldr	r0, [sp, #24]
    649c:	e3a01000 	mov	r1, #0	; 0x0
    64a0:	e3500001 	cmp	r0, #1	; 0x1
    64a4:	e3a00901 	mov	r0, #16384	; 0x4000
    64a8:	93a03901 	movls	r3, #16384	; 0x4000
    64ac:	83a03000 	movhi	r3, #0	; 0x0
    64b0:	e3550901 	cmp	r5, #16384	; 0x4000
    64b4:	31a02005 	movcc	r2, r5
    64b8:	23a02901 	movcs	r2, #16384	; 0x4000
    64bc:	e09aa000 	adds	sl, sl, r0
    64c0:	e0abb001 	adc	fp, fp, r1
    64c4:	e0988000 	adds	r8, r8, r0
    64c8:	e0877003 	add	r7, r7, r3
    64cc:	e0a99001 	adc	r9, r9, r1
    64d0:	e0625005 	rsb	r5, r2, r5
    64d4:	e3550000 	cmp	r5, #0	; 0x0
    64d8:	e0653004 	rsb	r3, r5, r4
    64dc:	1affffc9 	bne	6408 <burn_image_loop+0x1b4>
    64e0:	e1a00005 	mov	r0, r5
    64e4:	e28dd024 	add	sp, sp, #36	; 0x24
    64e8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    64ec:	0800d76c 	.word	0x0800d76c
    64f0:	00012e7f 	.word	0x00012e7f
    64f4:	00011d0c 	.word	0x00011d0c
    64f8:	00012ffa 	.word	0x00012ffa
    64fc:	00013001 	.word	0x00013001
    6500:	00013017 	.word	0x00013017
    6504:	00013044 	.word	0x00013044
    6508:	0800d10c 	.word	0x0800d10c
    650c:	0001305d 	.word	0x0001305d
    6510:	0001306c 	.word	0x0001306c

00006514 <burn_images>:
    6514:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6518:	e590901c 	ldr	r9, [r0, #28]
    651c:	e24dd014 	sub	sp, sp, #20	; 0x14
    6520:	e59f41ac 	ldr	r4, [pc, #428]	; 66d4 <burn_images+0x1c0>
    6524:	e1a05000 	mov	r5, r0
    6528:	e58d100c 	str	r1, [sp, #12]
    652c:	e59f01a4 	ldr	r0, [pc, #420]	; 66d8 <burn_images+0x1c4>
    6530:	e1a01009 	mov	r1, r9
    6534:	e1a0b002 	mov	fp, r2
    6538:	e2857020 	add	r7, r5, #32	; 0x20
    653c:	e1a0e00f 	mov	lr, pc
    6540:	e594f000 	ldr	pc, [r4]
    6544:	e1a0a004 	mov	sl, r4
    6548:	e3a08000 	mov	r8, #0	; 0x0
    654c:	ea000055 	b	66a8 <burn_images+0x194>
    6550:	e59f0184 	ldr	r0, [pc, #388]	; 66dc <burn_images+0x1c8>
    6554:	e1a01008 	mov	r1, r8
    6558:	e1a0e00f 	mov	lr, pc
    655c:	e59af000 	ldr	pc, [sl]
    6560:	e5d73003 	ldrb	r3, [r7, #3]
    6564:	e5976008 	ldr	r6, [r7, #8]
    6568:	e203503f 	and	r5, r3, #63	; 0x3f
    656c:	e3550002 	cmp	r5, #2	; 0x2
    6570:	0a00000a 	beq	65a0 <burn_images+0x8c>
    6574:	ca000004 	bgt	658c <burn_images+0x78>
    6578:	e3550000 	cmp	r5, #0	; 0x0
    657c:	0a000051 	beq	66c8 <burn_images+0x1b4>
    6580:	e3550001 	cmp	r5, #1	; 0x1
    6584:	1a00001a 	bne	65f4 <burn_images+0xe0>
    6588:	ea000008 	b	65b0 <burn_images+0x9c>
    658c:	e3550030 	cmp	r5, #48	; 0x30
    6590:	0a00000c 	beq	65c8 <burn_images+0xb4>
    6594:	e3550031 	cmp	r5, #49	; 0x31
    6598:	1a000015 	bne	65f4 <burn_images+0xe0>
    659c:	ea00000e 	b	65dc <burn_images+0xc8>
    65a0:	e1a04826 	lsr	r4, r6, #16
    65a4:	e354000f 	cmp	r4, #15	; 0xf
    65a8:	1a000015 	bne	6604 <burn_images+0xf0>
    65ac:	ea00001f 	b	6630 <burn_images+0x11c>
    65b0:	e59f0128 	ldr	r0, [pc, #296]	; 66e0 <burn_images+0x1cc>
    65b4:	e1a01006 	mov	r1, r6
    65b8:	e1a0e00f 	mov	lr, pc
    65bc:	e59af000 	ldr	pc, [sl]
    65c0:	e1a0e006 	mov	lr, r6
    65c4:	ea000040 	b	66cc <burn_images+0x1b8>
    65c8:	e59f0114 	ldr	r0, [pc, #276]	; 66e4 <burn_images+0x1d0>
    65cc:	e1a0e00f 	mov	lr, pc
    65d0:	e59af000 	ldr	pc, [sl]
    65d4:	ebffe88f 	bl	818 <system_reset>
    65d8:	ea00003a 	b	66c8 <burn_images+0x1b4>
    65dc:	e59f0104 	ldr	r0, [pc, #260]	; 66e8 <burn_images+0x1d4>
    65e0:	e1a0e00f 	mov	lr, pc
    65e4:	e59af000 	ldr	pc, [sl]
    65e8:	e3a00001 	mov	r0, #1	; 0x1
    65ec:	ebffe77d 	bl	3e8 <try_boot>
    65f0:	ea000034 	b	66c8 <burn_images+0x1b4>
    65f4:	e59f00f0 	ldr	r0, [pc, #240]	; 66ec <burn_images+0x1d8>
    65f8:	e1a0e00f 	mov	lr, pc
    65fc:	e59af000 	ldr	pc, [sl]
    6600:	ea000026 	b	66a0 <burn_images+0x18c>
    6604:	e1a00004 	mov	r0, r4
    6608:	e3a01001 	mov	r1, #1	; 0x1
    660c:	ebfffaa7 	bl	50b0 <vs_assign_by_id>
    6610:	e3500000 	cmp	r0, #0	; 0x0
    6614:	0a000005 	beq	6630 <burn_images+0x11c>
    6618:	e59f00d0 	ldr	r0, [pc, #208]	; 66f0 <burn_images+0x1dc>
    661c:	e59f30b0 	ldr	r3, [pc, #176]	; 66d4 <burn_images+0x1c0>
    6620:	e1a0e00f 	mov	lr, pc
    6624:	e593f000 	ldr	pc, [r3]
    6628:	e3e04002 	mvn	r4, #2	; 0x2
    662c:	ea000020 	b	66b4 <burn_images+0x1a0>
    6630:	e3a0e302 	mov	lr, #134217728	; 0x8000000
    6634:	e1a0c826 	lsr	ip, r6, #16
    6638:	e1a0c80c 	lsl	ip, ip, #16
    663c:	e1a03004 	mov	r3, r4
    6640:	e1a00007 	mov	r0, r7
    6644:	e59d100c 	ldr	r1, [sp, #12]
    6648:	e1a0200b 	mov	r2, fp
    664c:	e88d5000 	stm	sp, {ip, lr}
    6650:	ebfffeff 	bl	6254 <burn_image_loop>
    6654:	e3500000 	cmp	r0, #0	; 0x0
    6658:	aa000006 	bge	6678 <burn_images+0x164>
    665c:	e1a01008 	mov	r1, r8
    6660:	e59f008c 	ldr	r0, [pc, #140]	; 66f4 <burn_images+0x1e0>
    6664:	e59f3068 	ldr	r3, [pc, #104]	; 66d4 <burn_images+0x1c0>
    6668:	e1a0e00f 	mov	lr, pc
    666c:	e593f000 	ldr	pc, [r3]
    6670:	e3e04000 	mvn	r4, #0	; 0x0
    6674:	ea00000e 	b	66b4 <burn_images+0x1a0>
    6678:	e3550001 	cmp	r5, #1	; 0x1
    667c:	1a000003 	bne	6690 <burn_images+0x17c>
    6680:	e5970004 	ldr	r0, [r7, #4]
    6684:	e3a01000 	mov	r1, #0	; 0x0
    6688:	ebfffe67 	bl	602c <cdbg_jump>
    668c:	ea000003 	b	66a0 <burn_images+0x18c>
    6690:	e3550000 	cmp	r5, #0	; 0x0
    6694:	1a000001 	bne	66a0 <burn_images+0x18c>
    6698:	e3a00302 	mov	r0, #134217728	; 0x8000000
    669c:	ebfffed7 	bl	6200 <cdbg_config_isi>
    66a0:	e2888001 	add	r8, r8, #1	; 0x1
    66a4:	e2877010 	add	r7, r7, #16	; 0x10
    66a8:	e1580009 	cmp	r8, r9
    66ac:	baffffa7 	blt	6550 <burn_images+0x3c>
    66b0:	e3a04000 	mov	r4, #0	; 0x0
    66b4:	e3a00302 	mov	r0, #134217728	; 0x8000000
    66b8:	ebffebee 	bl	1678 <free>
    66bc:	e1a00004 	mov	r0, r4
    66c0:	e28dd014 	add	sp, sp, #20	; 0x14
    66c4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    66c8:	e3a0e302 	mov	lr, #134217728	; 0x8000000
    66cc:	e3a0400f 	mov	r4, #15	; 0xf
    66d0:	eaffffd7 	b	6634 <burn_images+0x120>
    66d4:	0800d76c 	.word	0x0800d76c
    66d8:	00013079 	.word	0x00013079
    66dc:	00013096 	.word	0x00013096
    66e0:	000130ac 	.word	0x000130ac
    66e4:	000130c9 	.word	0x000130c9
    66e8:	000130de 	.word	0x000130de
    66ec:	000130f1 	.word	0x000130f1
    66f0:	00013105 	.word	0x00013105
    66f4:	00013122 	.word	0x00013122

000066f8 <burn>:
    66f8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    66fc:	e1a09000 	mov	r9, r0
    6700:	e24dd00c 	sub	sp, sp, #12	; 0xc
    6704:	e3a00c02 	mov	r0, #512	; 0x200
    6708:	ebffeb72 	bl	14d8 <malloc>
    670c:	e2507000 	subs	r7, r0, #0	; 0x0
    6710:	1a000004 	bne	6728 <burn+0x30>
    6714:	e59f019c 	ldr	r0, [pc, #412]	; 68b8 <burn+0x1c0>
    6718:	e59f319c 	ldr	r3, [pc, #412]	; 68bc <burn+0x1c4>
    671c:	e1a0e00f 	mov	lr, pc
    6720:	e593f000 	ldr	pc, [r3]
    6724:	ea000060 	b	68ac <burn+0x1b4>
    6728:	ebffe7ab 	bl	5dc <boot_device>
    672c:	e3a05000 	mov	r5, #0	; 0x0
    6730:	e59f8184 	ldr	r8, [pc, #388]	; 68bc <burn+0x1c4>
    6734:	e3a0bc02 	mov	fp, #512	; 0x200
    6738:	e1a0a000 	mov	sl, r0
    673c:	ea00004c 	b	6874 <burn+0x17c>
    6740:	e1a00005 	mov	r0, r5
    6744:	ebfff92c 	bl	4bfc <vs_device_burnsrc>
    6748:	e3500000 	cmp	r0, #0	; 0x0
    674c:	0a000047 	beq	6870 <burn+0x178>
    6750:	e155000a 	cmp	r5, sl
    6754:	0a000045 	beq	6870 <burn+0x178>
    6758:	e3a03001 	mov	r3, #1	; 0x1
    675c:	e0193513 	ands	r3, r9, r3, lsl r5
    6760:	1a000042 	bne	6870 <burn+0x178>
    6764:	ebffe85a 	bl	8d4 <gtc_time>
    6768:	e1a00005 	mov	r0, r5
    676c:	e59f114c 	ldr	r1, [pc, #332]	; 68c0 <burn+0x1c8>
    6770:	ebfff98e 	bl	4db0 <vs_is_device>
    6774:	e3500000 	cmp	r0, #0	; 0x0
    6778:	1a000004 	bne	6790 <burn+0x98>
    677c:	e1a00005 	mov	r0, r5
    6780:	e59f113c 	ldr	r1, [pc, #316]	; 68c4 <burn+0x1cc>
    6784:	ebfff989 	bl	4db0 <vs_is_device>
    6788:	e3500000 	cmp	r0, #0	; 0x0
    678c:	0a00000c 	beq	67c4 <burn+0xcc>
    6790:	ebffe8e1 	bl	b1c <assert_linkpc>
    6794:	e3500000 	cmp	r0, #0	; 0x0
    6798:	0a000009 	beq	67c4 <burn+0xcc>
    679c:	eb0017d5 	bl	c6f8 <linkpc_loop>
    67a0:	e2506000 	subs	r6, r0, #0	; 0x0
    67a4:	0a000038 	beq	688c <burn+0x194>
    67a8:	e1a00005 	mov	r0, r5
    67ac:	e5984000 	ldr	r4, [r8]
    67b0:	ebfff947 	bl	4cd4 <vs_device_name>
    67b4:	e1a02006 	mov	r2, r6
    67b8:	e1a01000 	mov	r1, r0
    67bc:	e59f0104 	ldr	r0, [pc, #260]	; 68c8 <burn+0x1d0>
    67c0:	ea00000a 	b	67f0 <burn+0xf8>
    67c4:	e1a00005 	mov	r0, r5
    67c8:	e3a01001 	mov	r1, #1	; 0x1
    67cc:	ebfffa37 	bl	50b0 <vs_assign_by_id>
    67d0:	e2506000 	subs	r6, r0, #0	; 0x0
    67d4:	0a000007 	beq	67f8 <burn+0x100>
    67d8:	e1a00005 	mov	r0, r5
    67dc:	e5984000 	ldr	r4, [r8]
    67e0:	ebfff93b 	bl	4cd4 <vs_device_name>
    67e4:	e1a01000 	mov	r1, r0
    67e8:	e59f00dc 	ldr	r0, [pc, #220]	; 68cc <burn+0x1d4>
    67ec:	e1a02006 	mov	r2, r6
    67f0:	e12fff34 	blx	r4
    67f4:	ea00001d 	b	6870 <burn+0x178>
    67f8:	e1a01007 	mov	r1, r7
    67fc:	e59f00cc 	ldr	r0, [pc, #204]	; 68d0 <burn+0x1d8>
    6800:	e3a02401 	mov	r2, #16777216	; 0x1000000
    6804:	e3a03c02 	mov	r3, #512	; 0x200
    6808:	e58d6000 	str	r6, [sp]
    680c:	e1a0e00f 	mov	lr, pc
    6810:	e598f000 	ldr	pc, [r8]
    6814:	e3a02401 	mov	r2, #16777216	; 0x1000000
    6818:	e3a03000 	mov	r3, #0	; 0x0
    681c:	e1a00007 	mov	r0, r7
    6820:	e58d6004 	str	r6, [sp, #4]
    6824:	e58db000 	str	fp, [sp]
    6828:	ebfff979 	bl	4e14 <vs_read>
    682c:	e1a00007 	mov	r0, r7
    6830:	eb001dc2 	bl	df40 <ius_check_header>
    6834:	e2504000 	subs	r4, r0, #0	; 0x0
    6838:	1a00000c 	bne	6870 <burn+0x178>
    683c:	e1a01005 	mov	r1, r5
    6840:	e59f3074 	ldr	r3, [pc, #116]	; 68bc <burn+0x1c4>
    6844:	e59f0088 	ldr	r0, [pc, #136]	; 68d4 <burn+0x1dc>
    6848:	e1a0e00f 	mov	lr, pc
    684c:	e593f000 	ldr	pc, [r3]
    6850:	e1a00004 	mov	r0, r4
    6854:	e1a01004 	mov	r1, r4
    6858:	ebfffde8 	bl	6000 <cdbg_verify_burn_enable>
    685c:	e1a00007 	mov	r0, r7
    6860:	e1a01005 	mov	r1, r5
    6864:	e1a02004 	mov	r2, r4
    6868:	ebffff29 	bl	6514 <burn_images>
    686c:	ea000006 	b	688c <burn+0x194>
    6870:	e2855001 	add	r5, r5, #1	; 0x1
    6874:	ebfff937 	bl	4d58 <vs_device_count>
    6878:	e1550000 	cmp	r5, r0
    687c:	baffffaf 	blt	6740 <burn+0x48>
    6880:	e1a00007 	mov	r0, r7
    6884:	ebffeb7b 	bl	1678 <free>
    6888:	ea000007 	b	68ac <burn+0x1b4>
    688c:	e59f0044 	ldr	r0, [pc, #68]	; 68d8 <burn+0x1e0>
    6890:	e59f3024 	ldr	r3, [pc, #36]	; 68bc <burn+0x1c4>
    6894:	e1a0e00f 	mov	lr, pc
    6898:	e593f000 	ldr	pc, [r3]
    689c:	e1a00007 	mov	r0, r7
    68a0:	ebffeb74 	bl	1678 <free>
    68a4:	ebfffe12 	bl	60f4 <cdbg_shell>
    68a8:	eafffffe 	b	68a8 <burn+0x1b0>
    68ac:	e3e00000 	mvn	r0, #0	; 0x0
    68b0:	e28dd00c 	add	sp, sp, #12	; 0xc
    68b4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    68b8:	0001313a 	.word	0x0001313a
    68bc:	0800d76c 	.word	0x0800d76c
    68c0:	00012ce4 	.word	0x00012ce4
    68c4:	00012ce0 	.word	0x00012ce0
    68c8:	0001315a 	.word	0x0001315a
    68cc:	0001317c 	.word	0x0001317c
    68d0:	0001319b 	.word	0x0001319b
    68d4:	000131cb 	.word	0x000131cb
    68d8:	000131e3 	.word	0x000131e3

000068dc <mmc_send_cmd>:
    68dc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    68e0:	e24dd004 	sub	sp, sp, #4	; 0x4
    68e4:	e1a0e00f 	mov	lr, pc
    68e8:	e590f104 	ldr	pc, [r0, #260]
    68ec:	e28dd004 	add	sp, sp, #4	; 0x4
    68f0:	e8bd8000 	pop	{pc}

000068f4 <mmc_set_blocklen>:
    68f4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    68f8:	e3a03015 	mov	r3, #21	; 0x15
    68fc:	e24dd024 	sub	sp, sp, #36	; 0x24
    6900:	e3a02000 	mov	r2, #0	; 0x0
    6904:	e58d3004 	str	r3, [sp, #4]
    6908:	e3a03010 	mov	r3, #16	; 0x10
    690c:	e58d1008 	str	r1, [sp, #8]
    6910:	e1cd30b0 	strh	r3, [sp]
    6914:	e58d201c 	str	r2, [sp, #28]
    6918:	e1a0100d 	mov	r1, sp
    691c:	e1a0e00f 	mov	lr, pc
    6920:	e590f104 	ldr	pc, [r0, #260]
    6924:	e28dd024 	add	sp, sp, #36	; 0x24
    6928:	e8bd8000 	pop	{pc}

0000692c <find_mmc_device>:
    692c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6930:	e59f3048 	ldr	r3, [pc, #72]	; 6980 <find_mmc_device+0x54>
    6934:	e24dd004 	sub	sp, sp, #4	; 0x4
    6938:	e1a01000 	mov	r1, r0
    693c:	e1a0c003 	mov	ip, r3
    6940:	e5932000 	ldr	r2, [r3]
    6944:	ea000004 	b	695c <find_mmc_device+0x30>
    6948:	e592309c 	ldr	r3, [r2, #156]
    694c:	e1a00002 	mov	r0, r2
    6950:	e1530001 	cmp	r3, r1
    6954:	0a000007 	beq	6978 <find_mmc_device+0x4c>
    6958:	e5922000 	ldr	r2, [r2]
    695c:	e152000c 	cmp	r2, ip
    6960:	1afffff8 	bne	6948 <find_mmc_device+0x1c>
    6964:	e59f0018 	ldr	r0, [pc, #24]	; 6984 <find_mmc_device+0x58>
    6968:	e59f3018 	ldr	r3, [pc, #24]	; 6988 <find_mmc_device+0x5c>
    696c:	e1a0e00f 	mov	lr, pc
    6970:	e593f000 	ldr	pc, [r3]
    6974:	e3a00000 	mov	r0, #0	; 0x0
    6978:	e28dd004 	add	sp, sp, #4	; 0x4
    697c:	e8bd8000 	pop	{pc}
    6980:	0800d248 	.word	0x0800d248
    6984:	00013206 	.word	0x00013206
    6988:	0800d76c 	.word	0x0800d76c

0000698c <mmc_send_ext_csd>:
    698c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6990:	e3a03015 	mov	r3, #21	; 0x15
    6994:	e24dd034 	sub	sp, sp, #52	; 0x34
    6998:	e58d3004 	str	r3, [sp, #4]
    699c:	e3a03c02 	mov	r3, #512	; 0x200
    69a0:	e3a02000 	mov	r2, #0	; 0x0
    69a4:	e3a0c001 	mov	ip, #1	; 0x1
    69a8:	e58d302c 	str	r3, [sp, #44]
    69ac:	e3a03008 	mov	r3, #8	; 0x8
    69b0:	e58d1020 	str	r1, [sp, #32]
    69b4:	e58d201c 	str	r2, [sp, #28]
    69b8:	e58d2008 	str	r2, [sp, #8]
    69bc:	e58dc024 	str	ip, [sp, #36]
    69c0:	e1cd30b0 	strh	r3, [sp]
    69c4:	e58dc028 	str	ip, [sp, #40]
    69c8:	e1a0100d 	mov	r1, sp
    69cc:	e28d2020 	add	r2, sp, #32	; 0x20
    69d0:	e1a0e00f 	mov	lr, pc
    69d4:	e590f104 	ldr	pc, [r0, #260]
    69d8:	e28dd034 	add	sp, sp, #52	; 0x34
    69dc:	e8bd8000 	pop	{pc}

000069e0 <sd_switch>:
    69e0:	e3e0c4ff 	mvn	ip, #-16777216	; 0xff000000
    69e4:	e1a02102 	lsl	r2, r2, #2
    69e8:	e18ccf81 	orr	ip, ip, r1, lsl #31
    69ec:	e3a0100f 	mov	r1, #15	; 0xf
    69f0:	e1ccc211 	bic	ip, ip, r1, lsl r2
    69f4:	e18cc213 	orr	ip, ip, r3, lsl r2
    69f8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    69fc:	e3a03015 	mov	r3, #21	; 0x15
    6a00:	e24dd034 	sub	sp, sp, #52	; 0x34
    6a04:	e58d3004 	str	r3, [sp, #4]
    6a08:	e3a03000 	mov	r3, #0	; 0x0
    6a0c:	e58d301c 	str	r3, [sp, #28]
    6a10:	e59d3038 	ldr	r3, [sp, #56]
    6a14:	e3a02001 	mov	r2, #1	; 0x1
    6a18:	e58d3020 	str	r3, [sp, #32]
    6a1c:	e3a03040 	mov	r3, #64	; 0x40
    6a20:	e58d302c 	str	r3, [sp, #44]
    6a24:	e3a03006 	mov	r3, #6	; 0x6
    6a28:	e58d2024 	str	r2, [sp, #36]
    6a2c:	e58d2028 	str	r2, [sp, #40]
    6a30:	e58dc008 	str	ip, [sp, #8]
    6a34:	e1cd30b0 	strh	r3, [sp]
    6a38:	e1a0100d 	mov	r1, sp
    6a3c:	e28d2020 	add	r2, sp, #32	; 0x20
    6a40:	e1a0e00f 	mov	lr, pc
    6a44:	e590f104 	ldr	pc, [r0, #260]
    6a48:	e28dd034 	add	sp, sp, #52	; 0x34
    6a4c:	e8bd8000 	pop	{pc}

00006a50 <mmc_set_ios>:
    6a50:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6a54:	e24dd004 	sub	sp, sp, #4	; 0x4
    6a58:	e1a0e00f 	mov	lr, pc
    6a5c:	e590f108 	ldr	pc, [r0, #264]
    6a60:	e28dd004 	add	sp, sp, #4	; 0x4
    6a64:	e8bd8000 	pop	{pc}

00006a68 <mmc_set_clock>:
    6a68:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6a6c:	e590203c 	ldr	r2, [r0, #60]
    6a70:	e590c038 	ldr	ip, [r0, #56]
    6a74:	e1510002 	cmp	r1, r2
    6a78:	21a01002 	movcs	r1, r2
    6a7c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6a80:	e151000c 	cmp	r1, ip
    6a84:	25801048 	strcs	r1, [r0, #72]
    6a88:	3580c048 	strcc	ip, [r0, #72]
    6a8c:	e1a0e00f 	mov	lr, pc
    6a90:	e590f108 	ldr	pc, [r0, #264]
    6a94:	e28dd004 	add	sp, sp, #4	; 0x4
    6a98:	e8bd8000 	pop	{pc}

00006a9c <mmc_set_bus_width>:
    6a9c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6aa0:	e5801044 	str	r1, [r0, #68]
    6aa4:	e24dd004 	sub	sp, sp, #4	; 0x4
    6aa8:	e1a0e00f 	mov	lr, pc
    6aac:	e590f108 	ldr	pc, [r0, #264]
    6ab0:	e28dd004 	add	sp, sp, #4	; 0x4
    6ab4:	e8bd8000 	pop	{pc}

00006ab8 <mmc_send_if_cond>:
    6ab8:	e92d4010 	push	{r4, lr}
    6abc:	e590202c 	ldr	r2, [r0, #44]
    6ac0:	e59f3060 	ldr	r3, [pc, #96]	; 6b28 <mmc_send_if_cond+0x70>
    6ac4:	e24dd020 	sub	sp, sp, #32	; 0x20
    6ac8:	e0023003 	and	r3, r2, r3
    6acc:	e3530000 	cmp	r3, #0	; 0x0
    6ad0:	e28d1020 	add	r1, sp, #32	; 0x20
    6ad4:	e3a03008 	mov	r3, #8	; 0x8
    6ad8:	e300c1aa 	movw	ip, #426	; 0x1aa
    6adc:	e16132b0 	strh	r3, [r1, #-32]!
    6ae0:	03a0c0aa 	moveq	ip, #170	; 0xaa
    6ae4:	e3a02000 	mov	r2, #0	; 0x0
    6ae8:	e3a03015 	mov	r3, #21	; 0x15
    6aec:	e98d1008 	stmib	sp, {r3, ip}
    6af0:	e58d201c 	str	r2, [sp, #28]
    6af4:	e1a0100d 	mov	r1, sp
    6af8:	e5903104 	ldr	r3, [r0, #260]
    6afc:	e1a04000 	mov	r4, r0
    6b00:	e12fff33 	blx	r3
    6b04:	e3500000 	cmp	r0, #0	; 0x0
    6b08:	1a000004 	bne	6b20 <mmc_send_if_cond+0x68>
    6b0c:	e5dd300c 	ldrb	r3, [sp, #12]
    6b10:	e35300aa 	cmp	r3, #170	; 0xaa
    6b14:	059f3010 	ldreq	r3, [pc, #16]	; 6b2c <mmc_send_if_cond+0x74>
    6b18:	13e00010 	mvnne	r0, #16	; 0x10
    6b1c:	05843030 	streq	r3, [r4, #48]
    6b20:	e28dd020 	add	sp, sp, #32	; 0x20
    6b24:	e8bd8010 	pop	{r4, pc}
    6b28:	00ff8000 	.word	0x00ff8000
    6b2c:	00020020 	.word	0x00020020

00006b30 <mmc_register>:
    6b30:	e5903110 	ldr	r3, [r0, #272]
    6b34:	e59fc05c 	ldr	ip, [pc, #92]	; 6b98 <mmc_register+0x68>
    6b38:	e3530000 	cmp	r3, #0	; 0x0
    6b3c:	e3a03006 	mov	r3, #6	; 0x6
    6b40:	e59c2000 	ldr	r2, [ip]
    6b44:	e5803098 	str	r3, [r0, #152]
    6b48:	e59f304c 	ldr	r3, [pc, #76]	; 6b9c <mmc_register+0x6c>
    6b4c:	e2821001 	add	r1, r2, #1	; 0x1
    6b50:	e58030f8 	str	r3, [r0, #248]
    6b54:	e59f3044 	ldr	r3, [pc, #68]	; 6ba0 <mmc_register+0x70>
    6b58:	e580209c 	str	r2, [r0, #156]
    6b5c:	e58030fc 	str	r3, [r0, #252]
    6b60:	e3a03001 	mov	r3, #1	; 0x1
    6b64:	e58c1000 	str	r1, [ip]
    6b68:	e5c030a4 	strb	r3, [r0, #164]
    6b6c:	028330ff 	addeq	r3, r3, #255	; 0xff
    6b70:	05803110 	streq	r3, [r0, #272]
    6b74:	e59f3028 	ldr	r3, [pc, #40]	; 6ba4 <mmc_register+0x74>
    6b78:	e5800004 	str	r0, [r0, #4]
    6b7c:	e5932004 	ldr	r2, [r3, #4]
    6b80:	e5803000 	str	r3, [r0]
    6b84:	e5830004 	str	r0, [r3, #4]
    6b88:	e5820000 	str	r0, [r2]
    6b8c:	e5802004 	str	r2, [r0, #4]
    6b90:	e3a00000 	mov	r0, #0	; 0x0
    6b94:	e12fff1e 	bx	lr
    6b98:	0800d12c 	.word	0x0800d12c
    6b9c:	00007d6c 	.word	0x00007d6c
    6ba0:	00007288 	.word	0x00007288
    6ba4:	0800d248 	.word	0x0800d248

00006ba8 <mmc_get_dev>:
    6ba8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6bac:	e24dd004 	sub	sp, sp, #4	; 0x4
    6bb0:	ebffff5d 	bl	692c <find_mmc_device>
    6bb4:	e3500000 	cmp	r0, #0	; 0x0
    6bb8:	12800098 	addne	r0, r0, #152	; 0x98
    6bbc:	e28dd004 	add	sp, sp, #4	; 0x4
    6bc0:	e8bd8000 	pop	{pc}

00006bc4 <print_mmc_devices>:
    6bc4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6bc8:	e59f3068 	ldr	r3, [pc, #104]	; 6c38 <print_mmc_devices+0x74>
    6bcc:	e24dd004 	sub	sp, sp, #4	; 0x4
    6bd0:	e1a05000 	mov	r5, r0
    6bd4:	e1a07003 	mov	r7, r3
    6bd8:	e5934000 	ldr	r4, [r3]
    6bdc:	e59f6058 	ldr	r6, [pc, #88]	; 6c3c <print_mmc_devices+0x78>
    6be0:	ea00000a 	b	6c10 <print_mmc_devices+0x4c>
    6be4:	e594209c 	ldr	r2, [r4, #156]
    6be8:	e1a0e00f 	mov	lr, pc
    6bec:	e596f000 	ldr	pc, [r6]
    6bf0:	e5943000 	ldr	r3, [r4]
    6bf4:	e59f0044 	ldr	r0, [pc, #68]	; 6c40 <print_mmc_devices+0x7c>
    6bf8:	e1530007 	cmp	r3, r7
    6bfc:	e1a01005 	mov	r1, r5
    6c00:	0a000001 	beq	6c0c <print_mmc_devices+0x48>
    6c04:	e1a0e00f 	mov	lr, pc
    6c08:	e596f000 	ldr	pc, [r6]
    6c0c:	e5944000 	ldr	r4, [r4]
    6c10:	e1540007 	cmp	r4, r7
    6c14:	e2841008 	add	r1, r4, #8	; 0x8
    6c18:	e59f0024 	ldr	r0, [pc, #36]	; 6c44 <print_mmc_devices+0x80>
    6c1c:	1afffff0 	bne	6be4 <print_mmc_devices+0x20>
    6c20:	e59f0020 	ldr	r0, [pc, #32]	; 6c48 <print_mmc_devices+0x84>
    6c24:	e59f3010 	ldr	r3, [pc, #16]	; 6c3c <print_mmc_devices+0x78>
    6c28:	e1a0e00f 	mov	lr, pc
    6c2c:	e593f000 	ldr	pc, [r3]
    6c30:	e28dd004 	add	sp, sp, #4	; 0x4
    6c34:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6c38:	0800d248 	.word	0x0800d248
    6c3c:	0800d76c 	.word	0x0800d76c
    6c40:	00013226 	.word	0x00013226
    6c44:	0001321f 	.word	0x0001321f
    6c48:	00012472 	.word	0x00012472

00006c4c <get_mmc_num>:
    6c4c:	e59f3004 	ldr	r3, [pc, #4]	; 6c58 <get_mmc_num+0xc>
    6c50:	e5930000 	ldr	r0, [r3]
    6c54:	e12fff1e 	bx	lr
    6c58:	0800d12c 	.word	0x0800d12c

00006c5c <mmc_vs_align>:
    6c5c:	e3a00c02 	mov	r0, #512	; 0x200
    6c60:	e12fff1e 	bx	lr

00006c64 <init_mmc>:
    6c64:	e59f3060 	ldr	r3, [pc, #96]	; 6ccc <init_mmc+0x68>
    6c68:	e59f2060 	ldr	r2, [pc, #96]	; 6cd0 <init_mmc+0x6c>
    6c6c:	e92d4070 	push	{r4, r5, r6, lr}
    6c70:	e3a04000 	mov	r4, #0	; 0x0
    6c74:	e3a00b02 	mov	r0, #2048	; 0x800
    6c78:	e5833004 	str	r3, [r3, #4]
    6c7c:	e5824000 	str	r4, [r2]
    6c80:	e5833000 	str	r3, [r3]
    6c84:	ebffea13 	bl	14d8 <malloc>
    6c88:	e1a05000 	mov	r5, r0
    6c8c:	e2806b01 	add	r6, r0, #1024	; 0x400
    6c90:	e1a00005 	mov	r0, r5
    6c94:	e3a01000 	mov	r1, #0	; 0x0
    6c98:	e3a02f4e 	mov	r2, #312	; 0x138
    6c9c:	ebffe9a0 	bl	1324 <memset>
    6ca0:	e5856130 	str	r6, [r5, #304]
    6ca4:	e1a00004 	mov	r0, r4
    6ca8:	e1a01005 	mov	r1, r5
    6cac:	eb000490 	bl	7ef4 <imap_sdhc_initialize>
    6cb0:	e3500000 	cmp	r0, #0	; 0x0
    6cb4:	e2844001 	add	r4, r4, #1	; 0x1
    6cb8:	e2855f4e 	add	r5, r5, #312	; 0x138
    6cbc:	18bd8070 	popne	{r4, r5, r6, pc}
    6cc0:	e3540003 	cmp	r4, #3	; 0x3
    6cc4:	1afffff1 	bne	6c90 <init_mmc+0x2c>
    6cc8:	e8bd8070 	pop	{r4, r5, r6, pc}
    6ccc:	0800d248 	.word	0x0800d248
    6cd0:	0800d12c 	.word	0x0800d12c

00006cd4 <sd_change_freq>:
    6cd4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6cd8:	e5903050 	ldr	r3, [r0, #80]
    6cdc:	e3a01000 	mov	r1, #0	; 0x0
    6ce0:	e2132b01 	ands	r2, r3, #1024	; 0x400
    6ce4:	e1a04000 	mov	r4, r0
    6ce8:	e580104c 	str	r1, [r0, #76]
    6cec:	e24dd084 	sub	sp, sp, #132	; 0x84
    6cf0:	11a00001 	movne	r0, r1
    6cf4:	1a000070 	bne	6ebc <sd_change_freq+0x1e8>
    6cf8:	e1d438b0 	ldrh	r3, [r4, #128]
    6cfc:	e3a05015 	mov	r5, #21	; 0x15
    6d00:	e1a03803 	lsl	r3, r3, #16
    6d04:	e58d3050 	str	r3, [sp, #80]
    6d08:	e28d6048 	add	r6, sp, #72	; 0x48
    6d0c:	e3a03037 	mov	r3, #55	; 0x37
    6d10:	e1cd34b8 	strh	r3, [sp, #72]
    6d14:	e58d504c 	str	r5, [sp, #76]
    6d18:	e58d2064 	str	r2, [sp, #100]
    6d1c:	e1a01006 	mov	r1, r6
    6d20:	e1a0e00f 	mov	lr, pc
    6d24:	e594f104 	ldr	pc, [r4, #260]
    6d28:	e3500000 	cmp	r0, #0	; 0x0
    6d2c:	1a000062 	bne	6ebc <sd_change_freq+0x1e8>
    6d30:	e3a03033 	mov	r3, #51	; 0x33
    6d34:	e58d504c 	str	r5, [sp, #76]
    6d38:	e1a09006 	mov	r9, r6
    6d3c:	e58d0064 	str	r0, [sp, #100]
    6d40:	e1cd34b8 	strh	r3, [sp, #72]
    6d44:	e58d0050 	str	r0, [sp, #80]
    6d48:	e3a05003 	mov	r5, #3	; 0x3
    6d4c:	e28da078 	add	sl, sp, #120	; 0x78
    6d50:	e3a08008 	mov	r8, #8	; 0x8
    6d54:	e3a06001 	mov	r6, #1	; 0x1
    6d58:	e28d7068 	add	r7, sp, #104	; 0x68
    6d5c:	e58da068 	str	sl, [sp, #104]
    6d60:	e58d8074 	str	r8, [sp, #116]
    6d64:	e58d6070 	str	r6, [sp, #112]
    6d68:	e58d606c 	str	r6, [sp, #108]
    6d6c:	e1a00004 	mov	r0, r4
    6d70:	e1a01009 	mov	r1, r9
    6d74:	e1a02007 	mov	r2, r7
    6d78:	e1a0e00f 	mov	lr, pc
    6d7c:	e594f104 	ldr	pc, [r4, #260]
    6d80:	e250b000 	subs	fp, r0, #0	; 0x0
    6d84:	0a000003 	beq	6d98 <sd_change_freq+0xc4>
    6d88:	e3550000 	cmp	r5, #0	; 0x0
    6d8c:	e2455001 	sub	r5, r5, #1	; 0x1
    6d90:	1afffff1 	bne	6d5c <sd_change_freq+0x88>
    6d94:	ea000047 	b	6eb8 <sd_change_freq+0x1e4>
    6d98:	e59d3078 	ldr	r3, [sp, #120]
    6d9c:	e59d007c 	ldr	r0, [sp, #124]
    6da0:	e1a01c03 	lsl	r1, r3, #24
    6da4:	e2032cff 	and	r2, r3, #65280	; 0xff00
    6da8:	e1811c23 	orr	r1, r1, r3, lsr #24
    6dac:	e1811402 	orr	r1, r1, r2, lsl #8
    6db0:	e20338ff 	and	r3, r3, #16711680	; 0xff0000
    6db4:	e1811423 	orr	r1, r1, r3, lsr #8
    6db8:	e1a03c00 	lsl	r3, r0, #24
    6dbc:	e1833c20 	orr	r3, r3, r0, lsr #24
    6dc0:	e2002cff 	and	r2, r0, #65280	; 0xff00
    6dc4:	e1a0cc21 	lsr	ip, r1, #24
    6dc8:	e1833402 	orr	r3, r3, r2, lsl #8
    6dcc:	e20008ff 	and	r0, r0, #16711680	; 0xff0000
    6dd0:	e20cc00f 	and	ip, ip, #15	; 0xf
    6dd4:	e1833420 	orr	r3, r3, r0, lsr #8
    6dd8:	e35c0001 	cmp	ip, #1	; 0x1
    6ddc:	e584305c 	str	r3, [r4, #92]
    6de0:	e5841058 	str	r1, [r4, #88]
    6de4:	059f30d8 	ldreq	r3, [pc, #216]	; 6ec4 <sd_change_freq+0x1f0>
    6de8:	0a000004 	beq	6e00 <sd_change_freq+0x12c>
    6dec:	3a000002 	bcc	6dfc <sd_change_freq+0x128>
    6df0:	e35c0002 	cmp	ip, #2	; 0x2
    6df4:	059f30cc 	ldreq	r3, [pc, #204]	; 6ec8 <sd_change_freq+0x1f4>
    6df8:	0a000000 	beq	6e00 <sd_change_freq+0x12c>
    6dfc:	e59f30c8 	ldr	r3, [pc, #200]	; 6ecc <sd_change_freq+0x1f8>
    6e00:	e5843030 	str	r3, [r4, #48]
    6e04:	e5943058 	ldr	r3, [r4, #88]
    6e08:	e5942030 	ldr	r2, [r4, #48]
    6e0c:	e3130701 	tst	r3, #262144	; 0x40000
    6e10:	1594304c 	ldrne	r3, [r4, #76]
    6e14:	13833c01 	orrne	r3, r3, #256	; 0x100
    6e18:	1584304c 	strne	r3, [r4, #76]
    6e1c:	e59f30a8 	ldr	r3, [pc, #168]	; 6ecc <sd_change_freq+0x1f8>
    6e20:	e1520003 	cmp	r2, r3
    6e24:	13a05004 	movne	r5, #4	; 0x4
    6e28:	128d6008 	addne	r6, sp, #8	; 0x8
    6e2c:	1a000007 	bne	6e50 <sd_change_freq+0x17c>
    6e30:	ea000020 	b	6eb8 <sd_change_freq+0x1e4>
    6e34:	e58d6000 	str	r6, [sp]
    6e38:	ebfffee8 	bl	69e0 <sd_switch>
    6e3c:	e3500000 	cmp	r0, #0	; 0x0
    6e40:	1a00001d 	bne	6ebc <sd_change_freq+0x1e8>
    6e44:	e59d3024 	ldr	r3, [sp, #36]
    6e48:	e3130c02 	tst	r3, #512	; 0x200
    6e4c:	0a000005 	beq	6e68 <sd_change_freq+0x194>
    6e50:	e3a01000 	mov	r1, #0	; 0x0
    6e54:	e2555001 	subs	r5, r5, #1	; 0x1
    6e58:	e1a00004 	mov	r0, r4
    6e5c:	e1a02001 	mov	r2, r1
    6e60:	e3a03001 	mov	r3, #1	; 0x1
    6e64:	2afffff2 	bcs	6e34 <sd_change_freq+0x160>
    6e68:	e59d3014 	ldr	r3, [sp, #20]
    6e6c:	e3130c02 	tst	r3, #512	; 0x200
    6e70:	0a000010 	beq	6eb8 <sd_change_freq+0x1e4>
    6e74:	e3a01001 	mov	r1, #1	; 0x1
    6e78:	e28dc008 	add	ip, sp, #8	; 0x8
    6e7c:	e1a00004 	mov	r0, r4
    6e80:	e3a02000 	mov	r2, #0	; 0x0
    6e84:	e1a03001 	mov	r3, r1
    6e88:	e58dc000 	str	ip, [sp]
    6e8c:	ebfffed3 	bl	69e0 <sd_switch>
    6e90:	e3500000 	cmp	r0, #0	; 0x0
    6e94:	1a000008 	bne	6ebc <sd_change_freq+0x1e8>
    6e98:	e59d3018 	ldr	r3, [sp, #24]
    6e9c:	e1a03c03 	lsl	r3, r3, #24
    6ea0:	e203340f 	and	r3, r3, #251658240	; 0xf000000
    6ea4:	e3530401 	cmp	r3, #16777216	; 0x1000000
    6ea8:	0594304c 	ldreq	r3, [r4, #76]
    6eac:	03833001 	orreq	r3, r3, #1	; 0x1
    6eb0:	0584304c 	streq	r3, [r4, #76]
    6eb4:	ea000000 	b	6ebc <sd_change_freq+0x1e8>
    6eb8:	e1a0000b 	mov	r0, fp
    6ebc:	e28dd084 	add	sp, sp, #132	; 0x84
    6ec0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6ec4:	0002001a 	.word	0x0002001a
    6ec8:	00020020 	.word	0x00020020
    6ecc:	00020010 	.word	0x00020010

00006ed0 <sd_send_op_cond>:
    6ed0:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    6ed4:	e59f9140 	ldr	r9, [pc, #320]	; 701c <sd_send_op_cond+0x14c>
    6ed8:	e24dd020 	sub	sp, sp, #32	; 0x20
    6edc:	e1a04000 	mov	r4, r0
    6ee0:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    6ee4:	e3a0a015 	mov	sl, #21	; 0x15
    6ee8:	e3a06000 	mov	r6, #0	; 0x0
    6eec:	e1a0700d 	mov	r7, sp
    6ef0:	e3a08001 	mov	r8, #1	; 0x1
    6ef4:	e3a03037 	mov	r3, #55	; 0x37
    6ef8:	e1cd30b0 	strh	r3, [sp]
    6efc:	e1a0100d 	mov	r1, sp
    6f00:	e1a02006 	mov	r2, r6
    6f04:	e58da004 	str	sl, [sp, #4]
    6f08:	e58d6008 	str	r6, [sp, #8]
    6f0c:	e58d601c 	str	r6, [sp, #28]
    6f10:	e1a00004 	mov	r0, r4
    6f14:	e1a0e00f 	mov	lr, pc
    6f18:	e594f104 	ldr	pc, [r4, #260]
    6f1c:	e2503000 	subs	r3, r0, #0	; 0x0
    6f20:	e1a0100d 	mov	r1, sp
    6f24:	e3a02000 	mov	r2, #0	; 0x0
    6f28:	e59fe0f0 	ldr	lr, [pc, #240]	; 7020 <sd_send_op_cond+0x150>
    6f2c:	e1a00004 	mov	r0, r4
    6f30:	1a000036 	bne	7010 <sd_send_op_cond+0x140>
    6f34:	e594302c 	ldr	r3, [r4, #44]
    6f38:	e594c030 	ldr	ip, [r4, #48]
    6f3c:	e003e00e 	and	lr, r3, lr
    6f40:	e15c0009 	cmp	ip, r9
    6f44:	038e3101 	orreq	r3, lr, #1073741824	; 0x40000000
    6f48:	e58de008 	str	lr, [sp, #8]
    6f4c:	058d3008 	streq	r3, [sp, #8]
    6f50:	e3a03029 	mov	r3, #41	; 0x29
    6f54:	e1cd30b0 	strh	r3, [sp]
    6f58:	e58d8004 	str	r8, [sp, #4]
    6f5c:	e5943104 	ldr	r3, [r4, #260]
    6f60:	e12fff33 	blx	r3
    6f64:	e2503000 	subs	r3, r0, #0	; 0x0
    6f68:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    6f6c:	1a000027 	bne	7010 <sd_send_op_cond+0x140>
    6f70:	eb000636 	bl	8850 <udelay>
    6f74:	e59d300c 	ldr	r3, [sp, #12]
    6f78:	e3530000 	cmp	r3, #0	; 0x0
    6f7c:	ba000002 	blt	6f8c <sd_send_op_cond+0xbc>
    6f80:	e2555001 	subs	r5, r5, #1	; 0x1
    6f84:	2affffda 	bcs	6ef4 <sd_send_op_cond+0x24>
    6f88:	ea00001f 	b	700c <sd_send_op_cond+0x13c>
    6f8c:	e3550000 	cmp	r5, #0	; 0x0
    6f90:	da00001d 	ble	700c <sd_send_op_cond+0x13c>
    6f94:	e59f3080 	ldr	r3, [pc, #128]	; 701c <sd_send_op_cond+0x14c>
    6f98:	e5942030 	ldr	r2, [r4, #48]
    6f9c:	e1520003 	cmp	r2, r3
    6fa0:	12433010 	subne	r3, r3, #16	; 0x10
    6fa4:	15843030 	strne	r3, [r4, #48]
    6fa8:	e5943050 	ldr	r3, [r4, #80]
    6fac:	e3130b01 	tst	r3, #1024	; 0x400
    6fb0:	0a00000c 	beq	6fe8 <sd_send_op_cond+0x118>
    6fb4:	e3a03001 	mov	r3, #1	; 0x1
    6fb8:	e3a02000 	mov	r2, #0	; 0x0
    6fbc:	e58d3004 	str	r3, [sp, #4]
    6fc0:	e3a0303a 	mov	r3, #58	; 0x3a
    6fc4:	e1cd30b0 	strh	r3, [sp]
    6fc8:	e58d2008 	str	r2, [sp, #8]
    6fcc:	e58d201c 	str	r2, [sp, #28]
    6fd0:	e1a00004 	mov	r0, r4
    6fd4:	e1a0100d 	mov	r1, sp
    6fd8:	e1a0e00f 	mov	lr, pc
    6fdc:	e594f104 	ldr	pc, [r4, #260]
    6fe0:	e2503000 	subs	r3, r0, #0	; 0x0
    6fe4:	1a000009 	bne	7010 <sd_send_op_cond+0x140>
    6fe8:	e59d200c 	ldr	r2, [sp, #12]
    6fec:	e3a03000 	mov	r3, #0	; 0x0
    6ff0:	e1c438b0 	strh	r3, [r4, #128]
    6ff4:	e1a03f22 	lsr	r3, r2, #30
    6ff8:	e2033001 	and	r3, r3, #1	; 0x1
    6ffc:	e5843040 	str	r3, [r4, #64]
    7000:	e5842054 	str	r2, [r4, #84]
    7004:	e3a03000 	mov	r3, #0	; 0x0
    7008:	ea000000 	b	7010 <sd_send_op_cond+0x140>
    700c:	e3e03010 	mvn	r3, #16	; 0x10
    7010:	e1a00003 	mov	r0, r3
    7014:	e28dd020 	add	sp, sp, #32	; 0x20
    7018:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    701c:	00020020 	.word	0x00020020
    7020:	00ff8000 	.word	0x00ff8000

00007024 <mmc_go_idle>:
    7024:	e92d4010 	push	{r4, lr}
    7028:	e1a04000 	mov	r4, r0
    702c:	e24dd020 	sub	sp, sp, #32	; 0x20
    7030:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    7034:	eb000605 	bl	8850 <udelay>
    7038:	e3a02000 	mov	r2, #0	; 0x0
    703c:	e1a00004 	mov	r0, r4
    7040:	e1cd20b0 	strh	r2, [sp]
    7044:	e58d2008 	str	r2, [sp, #8]
    7048:	e58d2004 	str	r2, [sp, #4]
    704c:	e58d201c 	str	r2, [sp, #28]
    7050:	e1a0100d 	mov	r1, sp
    7054:	e1a0e00f 	mov	lr, pc
    7058:	e594f104 	ldr	pc, [r4, #260]
    705c:	e2504000 	subs	r4, r0, #0	; 0x0
    7060:	1a000001 	bne	706c <mmc_go_idle+0x48>
    7064:	e2800e7d 	add	r0, r0, #2000	; 0x7d0
    7068:	eb0005f8 	bl	8850 <udelay>
    706c:	e1a00004 	mov	r0, r4
    7070:	e28dd020 	add	sp, sp, #32	; 0x20
    7074:	e8bd8010 	pop	{r4, pc}

00007078 <mmc_send_op_cond>:
    7078:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    707c:	e24dd020 	sub	sp, sp, #32	; 0x20
    7080:	e1a05000 	mov	r5, r0
    7084:	e3a06001 	mov	r6, #1	; 0x1
    7088:	ebffffe5 	bl	7024 <mmc_go_idle>
    708c:	e3a02000 	mov	r2, #0	; 0x0
    7090:	e1cd60b0 	strh	r6, [sp]
    7094:	e58d6004 	str	r6, [sp, #4]
    7098:	e58d2008 	str	r2, [sp, #8]
    709c:	e58d201c 	str	r2, [sp, #28]
    70a0:	e1a00005 	mov	r0, r5
    70a4:	e1a0100d 	mov	r1, sp
    70a8:	e1a0e00f 	mov	lr, pc
    70ac:	e595f104 	ldr	pc, [r5, #260]
    70b0:	e2504000 	subs	r4, r0, #0	; 0x0
    70b4:	e1a0700d 	mov	r7, sp
    70b8:	1a000039 	bne	71a4 <mmc_send_op_cond+0x12c>
    70bc:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    70c0:	eb0005e2 	bl	8850 <udelay>
    70c4:	e1a08006 	mov	r8, r6
    70c8:	e1a0700d 	mov	r7, sp
    70cc:	e1a06004 	mov	r6, r4
    70d0:	e3024710 	movw	r4, #10000	; 0x2710
    70d4:	e595302c 	ldr	r3, [r5, #44]
    70d8:	e3a02001 	mov	r2, #1	; 0x1
    70dc:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    70e0:	e1cd20b0 	strh	r2, [sp]
    70e4:	e58d3008 	str	r3, [sp, #8]
    70e8:	e1a02006 	mov	r2, r6
    70ec:	e58d8004 	str	r8, [sp, #4]
    70f0:	e58d601c 	str	r6, [sp, #28]
    70f4:	e1a0100d 	mov	r1, sp
    70f8:	e1a00005 	mov	r0, r5
    70fc:	e1a0e00f 	mov	lr, pc
    7100:	e595f104 	ldr	pc, [r5, #260]
    7104:	e2502000 	subs	r2, r0, #0	; 0x0
    7108:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    710c:	11a04002 	movne	r4, r2
    7110:	1a000023 	bne	71a4 <mmc_send_op_cond+0x12c>
    7114:	eb0005cd 	bl	8850 <udelay>
    7118:	e59d300c 	ldr	r3, [sp, #12]
    711c:	e3530000 	cmp	r3, #0	; 0x0
    7120:	ba000002 	blt	7130 <mmc_send_op_cond+0xb8>
    7124:	e2544001 	subs	r4, r4, #1	; 0x1
    7128:	2affffe9 	bcs	70d4 <mmc_send_op_cond+0x5c>
    712c:	ea00001b 	b	71a0 <mmc_send_op_cond+0x128>
    7130:	e3540000 	cmp	r4, #0	; 0x0
    7134:	da000019 	ble	71a0 <mmc_send_op_cond+0x128>
    7138:	e5953050 	ldr	r3, [r5, #80]
    713c:	e3130b01 	tst	r3, #1024	; 0x400
    7140:	0a00000b 	beq	7174 <mmc_send_op_cond+0xfc>
    7144:	e3a0303a 	mov	r3, #58	; 0x3a
    7148:	e58d8004 	str	r8, [sp, #4]
    714c:	e1cd30b0 	strh	r3, [sp]
    7150:	e58d6008 	str	r6, [sp, #8]
    7154:	e58d601c 	str	r6, [sp, #28]
    7158:	e1a0100d 	mov	r1, sp
    715c:	e1a02006 	mov	r2, r6
    7160:	e1a00005 	mov	r0, r5
    7164:	e1a0e00f 	mov	lr, pc
    7168:	e595f104 	ldr	pc, [r5, #260]
    716c:	e2504000 	subs	r4, r0, #0	; 0x0
    7170:	1a00000b 	bne	71a4 <mmc_send_op_cond+0x12c>
    7174:	e59d300c 	ldr	r3, [sp, #12]
    7178:	e3a01801 	mov	r1, #65536	; 0x10000
    717c:	e1a02f23 	lsr	r2, r3, #30
    7180:	e2022001 	and	r2, r2, #1	; 0x1
    7184:	e5852040 	str	r2, [r5, #64]
    7188:	e3a02000 	mov	r2, #0	; 0x0
    718c:	e5851030 	str	r1, [r5, #48]
    7190:	e1c528b0 	strh	r2, [r5, #128]
    7194:	e5853054 	str	r3, [r5, #84]
    7198:	e3a04000 	mov	r4, #0	; 0x0
    719c:	ea000000 	b	71a4 <mmc_send_op_cond+0x12c>
    71a0:	e3e04010 	mvn	r4, #16	; 0x10
    71a4:	e1a00004 	mov	r0, r4
    71a8:	e28dd020 	add	sp, sp, #32	; 0x20
    71ac:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

000071b0 <mmc_send_status>:
    71b0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    71b4:	e1d038b0 	ldrh	r3, [r0, #128]
    71b8:	e24dd024 	sub	sp, sp, #36	; 0x24
    71bc:	e1a03803 	lsl	r3, r3, #16
    71c0:	e58d3008 	str	r3, [sp, #8]
    71c4:	e3a03000 	mov	r3, #0	; 0x0
    71c8:	e3a02015 	mov	r2, #21	; 0x15
    71cc:	e58d301c 	str	r3, [sp, #28]
    71d0:	e3a0300d 	mov	r3, #13	; 0xd
    71d4:	e1a06000 	mov	r6, r0
    71d8:	e58d2004 	str	r2, [sp, #4]
    71dc:	e1a04001 	mov	r4, r1
    71e0:	e1cd30b0 	strh	r3, [sp]
    71e4:	e1a0700d 	mov	r7, sp
    71e8:	e1a0100d 	mov	r1, sp
    71ec:	e3a02000 	mov	r2, #0	; 0x0
    71f0:	e1a00006 	mov	r0, r6
    71f4:	e1a0e00f 	mov	lr, pc
    71f8:	e596f104 	ldr	pc, [r6, #260]
    71fc:	e2505000 	subs	r5, r0, #0	; 0x0
    7200:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    7204:	1a000018 	bne	726c <mmc_send_status+0xbc>
    7208:	e59d300c 	ldr	r3, [sp, #12]
    720c:	e3130c01 	tst	r3, #256	; 0x100
    7210:	1a00000e 	bne	7250 <mmc_send_status+0xa0>
    7214:	eb00058d 	bl	8850 <udelay>
    7218:	e59d100c 	ldr	r1, [sp, #12]
    721c:	e59f3054 	ldr	r3, [pc, #84]	; 7278 <mmc_send_status+0xc8>
    7220:	e2444001 	sub	r4, r4, #1	; 0x1
    7224:	e0013003 	and	r3, r1, r3
    7228:	e3530000 	cmp	r3, #0	; 0x0
    722c:	0a000005 	beq	7248 <mmc_send_status+0x98>
    7230:	e59f0044 	ldr	r0, [pc, #68]	; 727c <mmc_send_status+0xcc>
    7234:	e59f3044 	ldr	r3, [pc, #68]	; 7280 <mmc_send_status+0xd0>
    7238:	e1a0e00f 	mov	lr, pc
    723c:	e593f000 	ldr	pc, [r3]
    7240:	e3e05011 	mvn	r5, #17	; 0x11
    7244:	ea000008 	b	726c <mmc_send_status+0xbc>
    7248:	e3740001 	cmn	r4, #1	; 0x1
    724c:	1affffe5 	bne	71e8 <mmc_send_status+0x38>
    7250:	e3540000 	cmp	r4, #0	; 0x0
    7254:	1a000004 	bne	726c <mmc_send_status+0xbc>
    7258:	e59f0024 	ldr	r0, [pc, #36]	; 7284 <mmc_send_status+0xd4>
    725c:	e59f301c 	ldr	r3, [pc, #28]	; 7280 <mmc_send_status+0xd0>
    7260:	e1a0e00f 	mov	lr, pc
    7264:	e593f000 	ldr	pc, [r3]
    7268:	e3e05012 	mvn	r5, #18	; 0x12
    726c:	e1a00005 	mov	r0, r5
    7270:	e28dd024 	add	sp, sp, #36	; 0x24
    7274:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    7278:	fdf94080 	.word	0xfdf94080
    727c:	0001322a 	.word	0x0001322a
    7280:	0800d76c 	.word	0x0800d76c
    7284:	00013240 	.word	0x00013240

00007288 <mmc_bwrite>:
    7288:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    728c:	e24dd034 	sub	sp, sp, #52	; 0x34
    7290:	e1a06001 	mov	r6, r1
    7294:	e1a0b002 	mov	fp, r2
    7298:	e1a0a003 	mov	sl, r3
    729c:	ebfffda2 	bl	692c <find_mmc_device>
    72a0:	e2504000 	subs	r4, r0, #0	; 0x0
    72a4:	0a000060 	beq	742c <mmc_bwrite+0x1a4>
    72a8:	e594108c 	ldr	r1, [r4, #140]
    72ac:	ebfffd90 	bl	68f4 <mmc_set_blocklen>
    72b0:	e3500000 	cmp	r0, #0	; 0x0
    72b4:	01a09000 	moveq	r9, r0
    72b8:	01a0700b 	moveq	r7, fp
    72bc:	0a000005 	beq	72d8 <mmc_bwrite+0x50>
    72c0:	e594108c 	ldr	r1, [r4, #140]
    72c4:	e59f016c 	ldr	r0, [pc, #364]	; 7438 <mmc_bwrite+0x1b0>
    72c8:	e59f316c 	ldr	r3, [pc, #364]	; 743c <mmc_bwrite+0x1b4>
    72cc:	e1a0e00f 	mov	lr, pc
    72d0:	e593f000 	ldr	pc, [r3]
    72d4:	ea000054 	b	742c <mmc_bwrite+0x1a4>
    72d8:	e5943110 	ldr	r3, [r4, #272]
    72dc:	e59420a8 	ldr	r2, [r4, #168]
    72e0:	e1570003 	cmp	r7, r3
    72e4:	31a05007 	movcc	r5, r7
    72e8:	21a05003 	movcs	r5, r3
    72ec:	e0858006 	add	r8, r5, r6
    72f0:	e1580002 	cmp	r8, r2
    72f4:	9a000005 	bls	7310 <mmc_bwrite+0x88>
    72f8:	e59f0140 	ldr	r0, [pc, #320]	; 7440 <mmc_bwrite+0x1b8>
    72fc:	e1a01008 	mov	r1, r8
    7300:	e59f3134 	ldr	r3, [pc, #308]	; 743c <mmc_bwrite+0x1b4>
    7304:	e1a0e00f 	mov	lr, pc
    7308:	e593f000 	ldr	pc, [r3]
    730c:	ea00003d 	b	7408 <mmc_bwrite+0x180>
    7310:	e3550001 	cmp	r5, #1	; 0x1
    7314:	93a03018 	movls	r3, #24	; 0x18
    7318:	91cd30b0 	strhls	r3, [sp]
    731c:	e5943040 	ldr	r3, [r4, #64]
    7320:	83a02019 	movhi	r2, #25	; 0x19
    7324:	81cd20b0 	strhhi	r2, [sp]
    7328:	e3530000 	cmp	r3, #0	; 0x0
    732c:	0594308c 	ldreq	r3, [r4, #140]
    7330:	e3a02015 	mov	r2, #21	; 0x15
    7334:	00030693 	muleq	r3, r3, r6
    7338:	158d6008 	strne	r6, [sp, #8]
    733c:	058d3008 	streq	r3, [sp, #8]
    7340:	e58d2004 	str	r2, [sp, #4]
    7344:	e58d901c 	str	r9, [sp, #28]
    7348:	e58da020 	str	sl, [sp, #32]
    734c:	e58d5028 	str	r5, [sp, #40]
    7350:	e594308c 	ldr	r3, [r4, #140]
    7354:	e3a02002 	mov	r2, #2	; 0x2
    7358:	e58d2024 	str	r2, [sp, #36]
    735c:	e58d302c 	str	r3, [sp, #44]
    7360:	e1a00004 	mov	r0, r4
    7364:	e1a0100d 	mov	r1, sp
    7368:	e28d2020 	add	r2, sp, #32	; 0x20
    736c:	e1a0e00f 	mov	lr, pc
    7370:	e594f104 	ldr	pc, [r4, #260]
    7374:	e3500000 	cmp	r0, #0	; 0x0
    7378:	0a000004 	beq	7390 <mmc_bwrite+0x108>
    737c:	e59f00c0 	ldr	r0, [pc, #192]	; 7444 <mmc_bwrite+0x1bc>
    7380:	e59f30b4 	ldr	r3, [pc, #180]	; 743c <mmc_bwrite+0x1b4>
    7384:	e1a0e00f 	mov	lr, pc
    7388:	e593f000 	ldr	pc, [r3]
    738c:	ea00001d 	b	7408 <mmc_bwrite+0x180>
    7390:	e5943050 	ldr	r3, [r4, #80]
    7394:	e1a03523 	lsr	r3, r3, #10
    7398:	e2233001 	eor	r3, r3, #1	; 0x1
    739c:	e3550001 	cmp	r5, #1	; 0x1
    73a0:	93a03000 	movls	r3, #0	; 0x0
    73a4:	82033001 	andhi	r3, r3, #1	; 0x1
    73a8:	e3530000 	cmp	r3, #0	; 0x0
    73ac:	0a000011 	beq	73f8 <mmc_bwrite+0x170>
    73b0:	e3a0200c 	mov	r2, #12	; 0xc
    73b4:	e3a0301d 	mov	r3, #29	; 0x1d
    73b8:	e1cd20b0 	strh	r2, [sp]
    73bc:	e98d0208 	stmib	sp, {r3, r9}
    73c0:	e58d901c 	str	r9, [sp, #28]
    73c4:	e1a00004 	mov	r0, r4
    73c8:	e1a0100d 	mov	r1, sp
    73cc:	e1a02009 	mov	r2, r9
    73d0:	e1a0e00f 	mov	lr, pc
    73d4:	e594f104 	ldr	pc, [r4, #260]
    73d8:	e3500000 	cmp	r0, #0	; 0x0
    73dc:	0a000005 	beq	73f8 <mmc_bwrite+0x170>
    73e0:	e59f0060 	ldr	r0, [pc, #96]	; 7448 <mmc_bwrite+0x1c0>
    73e4:	e59f3050 	ldr	r3, [pc, #80]	; 743c <mmc_bwrite+0x1b4>
    73e8:	e1a0e00f 	mov	lr, pc
    73ec:	e593f000 	ldr	pc, [r3]
    73f0:	e1a00009 	mov	r0, r9
    73f4:	ea00000d 	b	7430 <mmc_bwrite+0x1a8>
    73f8:	e1a00004 	mov	r0, r4
    73fc:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    7400:	ebffff6a 	bl	71b0 <mmc_send_status>
    7404:	ea000001 	b	7410 <mmc_bwrite+0x188>
    7408:	e3550000 	cmp	r5, #0	; 0x0
    740c:	1a000006 	bne	742c <mmc_bwrite+0x1a4>
    7410:	e0577005 	subs	r7, r7, r5
    7414:	e594008c 	ldr	r0, [r4, #140]
    7418:	102aa590 	mlane	sl, r0, r5, sl
    741c:	11a06008 	movne	r6, r8
    7420:	1affffac 	bne	72d8 <mmc_bwrite+0x50>
    7424:	e000009b 	mul	r0, fp, r0
    7428:	ea000000 	b	7430 <mmc_bwrite+0x1a8>
    742c:	e3a00000 	mov	r0, #0	; 0x0
    7430:	e28dd034 	add	sp, sp, #52	; 0x34
    7434:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7438:	0001325c 	.word	0x0001325c
    743c:	0800d76c 	.word	0x0800d76c
    7440:	00013266 	.word	0x00013266
    7444:	00013292 	.word	0x00013292
    7448:	000132a4 	.word	0x000132a4

0000744c <mmc2_vs_write>:
    744c:	e1a024a2 	lsr	r2, r2, #9
    7450:	e1821b83 	orr	r1, r2, r3, lsl #23
    7454:	e59d2000 	ldr	r2, [sp]
    7458:	e1a03000 	mov	r3, r0
    745c:	e1a024c2 	asr	r2, r2, #9
    7460:	e3a00002 	mov	r0, #2	; 0x2
    7464:	eaffff87 	b	7288 <mmc_bwrite>

00007468 <mmc1_vs_write>:
    7468:	e1a024a2 	lsr	r2, r2, #9
    746c:	e1821b83 	orr	r1, r2, r3, lsl #23
    7470:	e59d2000 	ldr	r2, [sp]
    7474:	e1a03000 	mov	r3, r0
    7478:	e1a024c2 	asr	r2, r2, #9
    747c:	e3a00001 	mov	r0, #1	; 0x1
    7480:	eaffff80 	b	7288 <mmc_bwrite>

00007484 <mmc0_vs_write>:
    7484:	e1a024a2 	lsr	r2, r2, #9
    7488:	e1821b83 	orr	r1, r2, r3, lsl #23
    748c:	e59d2000 	ldr	r2, [sp]
    7490:	e1a03000 	mov	r3, r0
    7494:	e1a024c2 	asr	r2, r2, #9
    7498:	e3a00000 	mov	r0, #0	; 0x0
    749c:	eaffff79 	b	7288 <mmc_bwrite>

000074a0 <mmc_switch>:
    74a0:	e92d4030 	push	{r4, r5, lr}
    74a4:	e1a02802 	lsl	r2, r2, #16
    74a8:	e3822403 	orr	r2, r2, #50331648	; 0x3000000
    74ac:	e24dd024 	sub	sp, sp, #36	; 0x24
    74b0:	e1822403 	orr	r2, r2, r3, lsl #8
    74b4:	e3a0301d 	mov	r3, #29	; 0x1d
    74b8:	e3a01000 	mov	r1, #0	; 0x0
    74bc:	e58d3004 	str	r3, [sp, #4]
    74c0:	e3a03006 	mov	r3, #6	; 0x6
    74c4:	e58d2008 	str	r2, [sp, #8]
    74c8:	e1cd30b0 	strh	r3, [sp]
    74cc:	e1a02001 	mov	r2, r1
    74d0:	e58d101c 	str	r1, [sp, #28]
    74d4:	e1a0100d 	mov	r1, sp
    74d8:	e1a04000 	mov	r4, r0
    74dc:	e1a0e00f 	mov	lr, pc
    74e0:	e590f104 	ldr	pc, [r0, #260]
    74e4:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    74e8:	e1a05000 	mov	r5, r0
    74ec:	e1a00004 	mov	r0, r4
    74f0:	ebffff2e 	bl	71b0 <mmc_send_status>
    74f4:	e1a00005 	mov	r0, r5
    74f8:	e28dd024 	add	sp, sp, #36	; 0x24
    74fc:	e8bd8030 	pop	{r4, r5, pc}

00007500 <mmc_change_freq>:
    7500:	e92d4070 	push	{r4, r5, r6, lr}
    7504:	e5903050 	ldr	r3, [r0, #80]
    7508:	e24ddc02 	sub	sp, sp, #512	; 0x200
    750c:	e3130b01 	tst	r3, #1024	; 0x400
    7510:	e3a03000 	mov	r3, #0	; 0x0
    7514:	e1a04000 	mov	r4, r0
    7518:	e580304c 	str	r3, [r0, #76]
    751c:	1a000020 	bne	75a4 <mmc_change_freq+0xa4>
    7520:	e5902030 	ldr	r2, [r0, #48]
    7524:	e59f3084 	ldr	r3, [pc, #132]	; 75b0 <mmc_change_freq+0xb0>
    7528:	e1520003 	cmp	r2, r3
    752c:	9a00001c 	bls	75a4 <mmc_change_freq+0xa4>
    7530:	e3a03c01 	mov	r3, #256	; 0x100
    7534:	e580304c 	str	r3, [r0, #76]
    7538:	e1a0100d 	mov	r1, sp
    753c:	ebfffd12 	bl	698c <mmc_send_ext_csd>
    7540:	e3500000 	cmp	r0, #0	; 0x0
    7544:	e1a0600d 	mov	r6, sp
    7548:	1a000016 	bne	75a8 <mmc_change_freq+0xa8>
    754c:	e3a01001 	mov	r1, #1	; 0x1
    7550:	e1a00004 	mov	r0, r4
    7554:	e3a020b9 	mov	r2, #185	; 0xb9
    7558:	e1a03001 	mov	r3, r1
    755c:	e5dd50c4 	ldrb	r5, [sp, #196]
    7560:	ebffffce 	bl	74a0 <mmc_switch>
    7564:	e3500000 	cmp	r0, #0	; 0x0
    7568:	1a00000e 	bne	75a8 <mmc_change_freq+0xa8>
    756c:	e1a0100d 	mov	r1, sp
    7570:	e1a00004 	mov	r0, r4
    7574:	ebfffd04 	bl	698c <mmc_send_ext_csd>
    7578:	e3500000 	cmp	r0, #0	; 0x0
    757c:	1a000009 	bne	75a8 <mmc_change_freq+0xa8>
    7580:	e5dd30b9 	ldrb	r3, [sp, #185]
    7584:	e3530000 	cmp	r3, #0	; 0x0
    7588:	0a000006 	beq	75a8 <mmc_change_freq+0xa8>
    758c:	e594304c 	ldr	r3, [r4, #76]
    7590:	e3150002 	tst	r5, #2	; 0x2
    7594:	13833011 	orrne	r3, r3, #17	; 0x11
    7598:	03833001 	orreq	r3, r3, #1	; 0x1
    759c:	e584304c 	str	r3, [r4, #76]
    75a0:	ea000000 	b	75a8 <mmc_change_freq+0xa8>
    75a4:	e3a00000 	mov	r0, #0	; 0x0
    75a8:	e28ddc02 	add	sp, sp, #512	; 0x200
    75ac:	e8bd8070 	pop	{r4, r5, r6, pc}
    75b0:	0001003f 	.word	0x0001003f

000075b4 <mmc_startup>:
    75b4:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    75b8:	e5903050 	ldr	r3, [r0, #80]
    75bc:	e24ddf8e 	sub	sp, sp, #568	; 0x238
    75c0:	e3130b01 	tst	r3, #1024	; 0x400
    75c4:	13a0300a 	movne	r3, #10	; 0xa
    75c8:	03a03002 	moveq	r3, #2	; 0x2
    75cc:	e28d1c02 	add	r1, sp, #512	; 0x200
    75d0:	e3a02000 	mov	r2, #0	; 0x0
    75d4:	e1c131b0 	strh	r3, [r1, #16]
    75d8:	e28d5e21 	add	r5, sp, #528	; 0x210
    75dc:	e3a03007 	mov	r3, #7	; 0x7
    75e0:	e58d3214 	str	r3, [sp, #532]
    75e4:	e58d2218 	str	r2, [sp, #536]
    75e8:	e58d222c 	str	r2, [sp, #556]
    75ec:	e1a01005 	mov	r1, r5
    75f0:	e1a06000 	mov	r6, r0
    75f4:	e1a0e00f 	mov	lr, pc
    75f8:	e590f104 	ldr	pc, [r0, #260]
    75fc:	e2504000 	subs	r4, r0, #0	; 0x0
    7600:	1a000159 	bne	7b6c <mmc_startup+0x5b8>
    7604:	e3a02010 	mov	r2, #16	; 0x10
    7608:	e2860070 	add	r0, r6, #112	; 0x70
    760c:	e285100c 	add	r1, r5, #12	; 0xc
    7610:	ebffe74b 	bl	1344 <memcpy>
    7614:	e5963050 	ldr	r3, [r6, #80]
    7618:	e2132b01 	ands	r2, r3, #1024	; 0x400
    761c:	1a000013 	bne	7670 <mmc_startup+0xbc>
    7620:	e1d638b0 	ldrh	r3, [r6, #128]
    7624:	e3a00003 	mov	r0, #3	; 0x3
    7628:	e1a03803 	lsl	r3, r3, #16
    762c:	e58d3218 	str	r3, [sp, #536]
    7630:	e3a03015 	mov	r3, #21	; 0x15
    7634:	e58d3214 	str	r3, [sp, #532]
    7638:	e28d3c02 	add	r3, sp, #512	; 0x200
    763c:	e1c301b0 	strh	r0, [r3, #16]
    7640:	e58d222c 	str	r2, [sp, #556]
    7644:	e1a01005 	mov	r1, r5
    7648:	e1a00006 	mov	r0, r6
    764c:	e1a0e00f 	mov	lr, pc
    7650:	e596f104 	ldr	pc, [r6, #260]
    7654:	e2504000 	subs	r4, r0, #0	; 0x0
    7658:	1a000143 	bne	7b6c <mmc_startup+0x5b8>
    765c:	e5963030 	ldr	r3, [r6, #48]
    7660:	e3130802 	tst	r3, #131072	; 0x20000
    7664:	128d1c02 	addne	r1, sp, #512	; 0x200
    7668:	11d111be 	ldrhne	r1, [r1, #30]
    766c:	11c618b0 	strhne	r1, [r6, #128]
    7670:	e1d638b0 	ldrh	r3, [r6, #128]
    7674:	e3a01000 	mov	r1, #0	; 0x0
    7678:	e1a03803 	lsl	r3, r3, #16
    767c:	e3a02007 	mov	r2, #7	; 0x7
    7680:	e58d3218 	str	r3, [sp, #536]
    7684:	e3a00009 	mov	r0, #9	; 0x9
    7688:	e28d3c02 	add	r3, sp, #512	; 0x200
    768c:	e1c301b0 	strh	r0, [r3, #16]
    7690:	e58d2214 	str	r2, [sp, #532]
    7694:	e58d122c 	str	r1, [sp, #556]
    7698:	e1a02001 	mov	r2, r1
    769c:	e1a00006 	mov	r0, r6
    76a0:	e28d1e21 	add	r1, sp, #528	; 0x210
    76a4:	e1a0e00f 	mov	lr, pc
    76a8:	e596f104 	ldr	pc, [r6, #260]
    76ac:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    76b0:	e1a04000 	mov	r4, r0
    76b4:	e1a00006 	mov	r0, r6
    76b8:	ebfffebc 	bl	71b0 <mmc_send_status>
    76bc:	e3540000 	cmp	r4, #0	; 0x0
    76c0:	1a000129 	bne	7b6c <mmc_startup+0x5b8>
    76c4:	e5963030 	ldr	r3, [r6, #48]
    76c8:	e59d021c 	ldr	r0, [sp, #540]
    76cc:	e59d2224 	ldr	r2, [sp, #548]
    76d0:	e59d1228 	ldr	r1, [sp, #552]
    76d4:	e3530801 	cmp	r3, #65536	; 0x10000
    76d8:	e59d3220 	ldr	r3, [sp, #544]
    76dc:	e5862068 	str	r2, [r6, #104]
    76e0:	e5863064 	str	r3, [r6, #100]
    76e4:	e586106c 	str	r1, [r6, #108]
    76e8:	e5860060 	str	r0, [r6, #96]
    76ec:	1a000013 	bne	7740 <mmc_startup+0x18c>
    76f0:	e1a03d20 	lsr	r3, r0, #26
    76f4:	e203300f 	and	r3, r3, #15	; 0xf
    76f8:	e3530004 	cmp	r3, #4	; 0x4
    76fc:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    7700:	ea00000c 	b	7738 <mmc_startup+0x184>
    7704:	00007738 	.word	0x00007738
    7708:	00007718 	.word	0x00007718
    770c:	00007720 	.word	0x00007720
    7710:	00007728 	.word	0x00007728
    7714:	00007730 	.word	0x00007730
    7718:	e59f3458 	ldr	r3, [pc, #1112]	; 7b78 <mmc_startup+0x5c4>
    771c:	ea000006 	b	773c <mmc_startup+0x188>
    7720:	e59f3454 	ldr	r3, [pc, #1108]	; 7b7c <mmc_startup+0x5c8>
    7724:	ea000004 	b	773c <mmc_startup+0x188>
    7728:	e59f3450 	ldr	r3, [pc, #1104]	; 7b80 <mmc_startup+0x5cc>
    772c:	ea000002 	b	773c <mmc_startup+0x188>
    7730:	e59f344c 	ldr	r3, [pc, #1100]	; 7b84 <mmc_startup+0x5d0>
    7734:	ea000000 	b	773c <mmc_startup+0x188>
    7738:	e59f3448 	ldr	r3, [pc, #1096]	; 7b88 <mmc_startup+0x5d4>
    773c:	e5863030 	str	r3, [r6, #48]
    7740:	e28d1c02 	add	r1, sp, #512	; 0x200
    7744:	e1d132b2 	ldrh	r3, [r1, #34]
    7748:	e3a0e001 	mov	lr, #1	; 0x1
    774c:	e203300f 	and	r3, r3, #15	; 0xf
    7750:	e1a0c31e 	lsl	ip, lr, r3
    7754:	e59d121c 	ldr	r1, [sp, #540]
    7758:	e59f242c 	ldr	r2, [pc, #1068]	; 7b8c <mmc_startup+0x5d8>
    775c:	e1a031a1 	lsr	r3, r1, #3
    7760:	e203300f 	and	r3, r3, #15	; 0xf
    7764:	e7920103 	ldr	r0, [r2, r3, lsl #2]
    7768:	e59f3420 	ldr	r3, [pc, #1056]	; 7b90 <mmc_startup+0x5dc>
    776c:	e2011007 	and	r1, r1, #7	; 0x7
    7770:	e7933101 	ldr	r3, [r3, r1, lsl #2]
    7774:	e5962030 	ldr	r2, [r6, #48]
    7778:	e0030093 	mul	r3, r3, r0
    777c:	e3120802 	tst	r2, #131072	; 0x20000
    7780:	e5863084 	str	r3, [r6, #132]
    7784:	e586c088 	str	ip, [r6, #136]
    7788:	059d3228 	ldreq	r3, [sp, #552]
    778c:	1586c08c 	strne	ip, [r6, #140]
    7790:	01a03b23 	lsreq	r3, r3, #22
    7794:	0203300f 	andeq	r3, r3, #15	; 0xf
    7798:	01a0331e 	lsleq	r3, lr, r3
    779c:	0586308c 	streq	r3, [r6, #140]
    77a0:	e5963040 	ldr	r3, [r6, #64]
    77a4:	e5962064 	ldr	r2, [r6, #100]
    77a8:	e3530000 	cmp	r3, #0	; 0x0
    77ac:	e5960068 	ldr	r0, [r6, #104]
    77b0:	0a000007 	beq	77d4 <mmc_startup+0x220>
    77b4:	e202203f 	and	r2, r2, #63	; 0x3f
    77b8:	e1a03820 	lsr	r3, r0, #16
    77bc:	e1833802 	orr	r3, r3, r2, lsl #16
    77c0:	e1a08003 	mov	r8, r3
    77c4:	e3a09000 	mov	r9, #0	; 0x0
    77c8:	e3a04008 	mov	r4, #8	; 0x8
    77cc:	e3a05000 	mov	r5, #0	; 0x0
    77d0:	ea000009 	b	77fc <mmc_startup+0x248>
    77d4:	e1a03b02 	lsl	r3, r2, #22
    77d8:	e1a03b23 	lsr	r3, r3, #22
    77dc:	e200190e 	and	r1, r0, #229376	; 0x38000
    77e0:	e1a02f20 	lsr	r2, r0, #30
    77e4:	e1822103 	orr	r2, r2, r3, lsl #2
    77e8:	e1a017a1 	lsr	r1, r1, #15
    77ec:	e1a08002 	mov	r8, r2
    77f0:	e3a09000 	mov	r9, #0	; 0x0
    77f4:	e1a04001 	mov	r4, r1
    77f8:	e3a05000 	mov	r5, #0	; 0x0
    77fc:	e3a02001 	mov	r2, #1	; 0x1
    7800:	e0980002 	adds	r0, r8, r2
    7804:	e3a03000 	mov	r3, #0	; 0x0
    7808:	e0a91003 	adc	r1, r9, r3
    780c:	e2842002 	add	r2, r4, #2	; 0x2
    7810:	eb002351 	bl	1055c <__aeabi_llsl>
    7814:	e596c088 	ldr	ip, [r6, #136]
    7818:	e0832c90 	umull	r2, r3, r0, ip
    781c:	e023319c 	mla	r3, ip, r1, r3
    7820:	e35c0c02 	cmp	ip, #512	; 0x200
    7824:	e1c629f0 	strd	r2, [r6, #144]
    7828:	83a03c02 	movhi	r3, #512	; 0x200
    782c:	85863088 	strhi	r3, [r6, #136]
    7830:	e596308c 	ldr	r3, [r6, #140]
    7834:	e3530c02 	cmp	r3, #512	; 0x200
    7838:	83a03c02 	movhi	r3, #512	; 0x200
    783c:	8586308c 	strhi	r3, [r6, #140]
    7840:	e5963050 	ldr	r3, [r6, #80]
    7844:	e2131b01 	ands	r1, r3, #1024	; 0x400
    7848:	1a00000f 	bne	788c <mmc_startup+0x2d8>
    784c:	e1d638b0 	ldrh	r3, [r6, #128]
    7850:	e3a0201d 	mov	r2, #29	; 0x1d
    7854:	e1a03803 	lsl	r3, r3, #16
    7858:	e58d2214 	str	r2, [sp, #532]
    785c:	e58d3218 	str	r3, [sp, #536]
    7860:	e28d2c02 	add	r2, sp, #512	; 0x200
    7864:	e3a03007 	mov	r3, #7	; 0x7
    7868:	e1c231b0 	strh	r3, [r2, #16]
    786c:	e58d122c 	str	r1, [sp, #556]
    7870:	e1a02001 	mov	r2, r1
    7874:	e1a00006 	mov	r0, r6
    7878:	e28d1e21 	add	r1, sp, #528	; 0x210
    787c:	e1a0e00f 	mov	lr, pc
    7880:	e596f104 	ldr	pc, [r6, #260]
    7884:	e2504000 	subs	r4, r0, #0	; 0x0
    7888:	1a0000b7 	bne	7b6c <mmc_startup+0x5b8>
    788c:	e5962030 	ldr	r2, [r6, #48]
    7890:	e3e03000 	mvn	r3, #0	; 0x0
    7894:	e3120802 	tst	r2, #131072	; 0x20000
    7898:	e5c63082 	strb	r3, [r6, #130]
    789c:	1a00001e 	bne	791c <mmc_startup+0x368>
    78a0:	e59f32ec 	ldr	r3, [pc, #748]	; 7b94 <mmc_startup+0x5e0>
    78a4:	e1520003 	cmp	r2, r3
    78a8:	9a00001b 	bls	791c <mmc_startup+0x368>
    78ac:	e28d1010 	add	r1, sp, #16	; 0x10
    78b0:	e1a00006 	mov	r0, r6
    78b4:	ebfffc34 	bl	698c <mmc_send_ext_csd>
    78b8:	e5dd30d0 	ldrb	r3, [sp, #208]
    78bc:	e2701001 	rsbs	r1, r0, #1	; 0x1
    78c0:	33a01000 	movcc	r1, #0	; 0x0
    78c4:	e3530001 	cmp	r3, #1	; 0x1
    78c8:	93a01000 	movls	r1, #0	; 0x0
    78cc:	e3510000 	cmp	r1, #0	; 0x0
    78d0:	0a00000d 	beq	790c <mmc_startup+0x358>
    78d4:	e5dd20e6 	ldrb	r2, [sp, #230]
    78d8:	e5dd10e5 	ldrb	r1, [sp, #229]
    78dc:	e5dd00e4 	ldrb	r0, [sp, #228]
    78e0:	e1a02802 	lsl	r2, r2, #16
    78e4:	e1822401 	orr	r2, r2, r1, lsl #8
    78e8:	e5dd10e7 	ldrb	r1, [sp, #231]
    78ec:	e1822000 	orr	r2, r2, r0
    78f0:	e1822c01 	orr	r2, r2, r1, lsl #24
    78f4:	e1a03fc2 	asr	r3, r2, #31
    78f8:	e1a01483 	lsl	r1, r3, #9
    78fc:	e1811ba2 	orr	r1, r1, r2, lsr #23
    7900:	e1a02482 	lsl	r2, r2, #9
    7904:	e5861094 	str	r1, [r6, #148]
    7908:	e5862090 	str	r2, [r6, #144]
    790c:	e5dd30b0 	ldrb	r3, [sp, #176]
    7910:	e3130001 	tst	r3, #1	; 0x1
    7914:	15dd30c3 	ldrbne	r3, [sp, #195]
    7918:	15c63082 	strbne	r3, [r6, #130]
    791c:	e5963030 	ldr	r3, [r6, #48]
    7920:	e3130802 	tst	r3, #131072	; 0x20000
    7924:	0a000002 	beq	7934 <mmc_startup+0x380>
    7928:	e1a00006 	mov	r0, r6
    792c:	ebfffce8 	bl	6cd4 <sd_change_freq>
    7930:	ea000001 	b	793c <mmc_startup+0x388>
    7934:	e1a00006 	mov	r0, r6
    7938:	ebfffef0 	bl	7500 <mmc_change_freq>
    793c:	e3500000 	cmp	r0, #0	; 0x0
    7940:	e1a05000 	mov	r5, r0
    7944:	11a04000 	movne	r4, r0
    7948:	1a000087 	bne	7b6c <mmc_startup+0x5b8>
    794c:	e596304c 	ldr	r3, [r6, #76]
    7950:	e5962050 	ldr	r2, [r6, #80]
    7954:	e5961030 	ldr	r1, [r6, #48]
    7958:	e0022003 	and	r2, r2, r3
    795c:	e3110802 	tst	r1, #131072	; 0x20000
    7960:	e586204c 	str	r2, [r6, #76]
    7964:	e2023c01 	and	r3, r2, #256	; 0x100
    7968:	0a00002c 	beq	7a20 <mmc_startup+0x46c>
    796c:	e3530000 	cmp	r3, #0	; 0x0
    7970:	0a000023 	beq	7a04 <mmc_startup+0x450>
    7974:	e1d638b0 	ldrh	r3, [r6, #128]
    7978:	e28d2c02 	add	r2, sp, #512	; 0x200
    797c:	e1a03803 	lsl	r3, r3, #16
    7980:	e58d3218 	str	r3, [sp, #536]
    7984:	e3a08015 	mov	r8, #21	; 0x15
    7988:	e3a03037 	mov	r3, #55	; 0x37
    798c:	e28d9e21 	add	r9, sp, #528	; 0x210
    7990:	e1c231b0 	strh	r3, [r2, #16]
    7994:	e58d022c 	str	r0, [sp, #556]
    7998:	e58d8214 	str	r8, [sp, #532]
    799c:	e1a00006 	mov	r0, r6
    79a0:	e1a01009 	mov	r1, r9
    79a4:	e1a02005 	mov	r2, r5
    79a8:	e1a0e00f 	mov	lr, pc
    79ac:	e596f104 	ldr	pc, [r6, #260]
    79b0:	e2504000 	subs	r4, r0, #0	; 0x0
    79b4:	1a00006c 	bne	7b6c <mmc_startup+0x5b8>
    79b8:	e3a03002 	mov	r3, #2	; 0x2
    79bc:	e3a01006 	mov	r1, #6	; 0x6
    79c0:	e28d0c02 	add	r0, sp, #512	; 0x200
    79c4:	e1c011b0 	strh	r1, [r0, #16]
    79c8:	e58d8214 	str	r8, [sp, #532]
    79cc:	e58d3218 	str	r3, [sp, #536]
    79d0:	e58d522c 	str	r5, [sp, #556]
    79d4:	e1a01009 	mov	r1, r9
    79d8:	e1a02005 	mov	r2, r5
    79dc:	e1a00006 	mov	r0, r6
    79e0:	e1a0e00f 	mov	lr, pc
    79e4:	e596f104 	ldr	pc, [r6, #260]
    79e8:	e2504000 	subs	r4, r0, #0	; 0x0
    79ec:	1a00005e 	bne	7b6c <mmc_startup+0x5b8>
    79f0:	e3a03004 	mov	r3, #4	; 0x4
    79f4:	e5863044 	str	r3, [r6, #68]
    79f8:	e1a00006 	mov	r0, r6
    79fc:	e1a0e00f 	mov	lr, pc
    7a00:	e596f108 	ldr	pc, [r6, #264]
    7a04:	e596304c 	ldr	r3, [r6, #76]
    7a08:	e3130001 	tst	r3, #1	; 0x1
    7a0c:	11a00006 	movne	r0, r6
    7a10:	159f1180 	ldrne	r1, [pc, #384]	; 7b98 <mmc_startup+0x5e4>
    7a14:	01a00006 	moveq	r0, r6
    7a18:	059f117c 	ldreq	r1, [pc, #380]	; 7b9c <mmc_startup+0x5e8>
    7a1c:	ea000022 	b	7aac <mmc_startup+0x4f8>
    7a20:	e3530000 	cmp	r3, #0	; 0x0
    7a24:	0a000008 	beq	7a4c <mmc_startup+0x498>
    7a28:	e3a01001 	mov	r1, #1	; 0x1
    7a2c:	e1a03001 	mov	r3, r1
    7a30:	e1a00006 	mov	r0, r6
    7a34:	e3a020b7 	mov	r2, #183	; 0xb7
    7a38:	ebfffe98 	bl	74a0 <mmc_switch>
    7a3c:	e2504000 	subs	r4, r0, #0	; 0x0
    7a40:	03a03004 	moveq	r3, #4	; 0x4
    7a44:	1a000048 	bne	7b6c <mmc_startup+0x5b8>
    7a48:	ea000009 	b	7a74 <mmc_startup+0x4c0>
    7a4c:	e3120c02 	tst	r2, #512	; 0x200
    7a50:	0a00000b 	beq	7a84 <mmc_startup+0x4d0>
    7a54:	e1a00006 	mov	r0, r6
    7a58:	e3a01001 	mov	r1, #1	; 0x1
    7a5c:	e3a020b7 	mov	r2, #183	; 0xb7
    7a60:	e3a03002 	mov	r3, #2	; 0x2
    7a64:	ebfffe8d 	bl	74a0 <mmc_switch>
    7a68:	e2504000 	subs	r4, r0, #0	; 0x0
    7a6c:	1a00003e 	bne	7b6c <mmc_startup+0x5b8>
    7a70:	e3a03008 	mov	r3, #8	; 0x8
    7a74:	e5863044 	str	r3, [r6, #68]
    7a78:	e1a00006 	mov	r0, r6
    7a7c:	e1a0e00f 	mov	lr, pc
    7a80:	e596f108 	ldr	pc, [r6, #264]
    7a84:	e596304c 	ldr	r3, [r6, #76]
    7a88:	e3130001 	tst	r3, #1	; 0x1
    7a8c:	01a00006 	moveq	r0, r6
    7a90:	059f1108 	ldreq	r1, [pc, #264]	; 7ba0 <mmc_startup+0x5ec>
    7a94:	0a000004 	beq	7aac <mmc_startup+0x4f8>
    7a98:	e3130010 	tst	r3, #16	; 0x10
    7a9c:	159f1100 	ldrne	r1, [pc, #256]	; 7ba4 <mmc_startup+0x5f0>
    7aa0:	059f1100 	ldreq	r1, [pc, #256]	; 7ba8 <mmc_startup+0x5f4>
    7aa4:	11a00006 	movne	r0, r6
    7aa8:	01a00006 	moveq	r0, r6
    7aac:	ebfffbed 	bl	6a68 <mmc_set_clock>
    7ab0:	e5964094 	ldr	r4, [r6, #148]
    7ab4:	e5960090 	ldr	r0, [r6, #144]
    7ab8:	e5961088 	ldr	r1, [r6, #136]
    7abc:	e3a03000 	mov	r3, #0	; 0x0
    7ac0:	e3540000 	cmp	r4, #0	; 0x0
    7ac4:	e5c630a3 	strb	r3, [r6, #163]
    7ac8:	e5c630a2 	strb	r3, [r6, #162]
    7acc:	e58610ac 	str	r1, [r6, #172]
    7ad0:	e58d0230 	str	r0, [sp, #560]
    7ad4:	e58d4234 	str	r4, [sp, #564]
    7ad8:	1a000003 	bne	7aec <mmc_startup+0x538>
    7adc:	eb002187 	bl	10100 <__aeabi_uidiv>
    7ae0:	e58d4234 	str	r4, [sp, #564]
    7ae4:	e58d0230 	str	r0, [sp, #560]
    7ae8:	ea000001 	b	7af4 <mmc_startup+0x540>
    7aec:	e28d0e23 	add	r0, sp, #560	; 0x230
    7af0:	eb001541 	bl	cffc <__div64_32>
    7af4:	e5d6007f 	ldrb	r0, [r6, #127]
    7af8:	e59d1230 	ldr	r1, [sp, #560]
    7afc:	e5962070 	ldr	r2, [r6, #112]
    7b00:	e5963078 	ldr	r3, [r6, #120]
    7b04:	e58610a8 	str	r1, [r6, #168]
    7b08:	e1803403 	orr	r3, r0, r3, lsl #8
    7b0c:	e1a02422 	lsr	r2, r2, #8
    7b10:	e59f1094 	ldr	r1, [pc, #148]	; 7bac <mmc_startup+0x5f8>
    7b14:	e28600b0 	add	r0, r6, #176	; 0xb0
    7b18:	eb001900 	bl	df20 <sprintf>
    7b1c:	e596c074 	ldr	ip, [r6, #116]
    7b20:	e5d62070 	ldrb	r2, [r6, #112]
    7b24:	e1a0482c 	lsr	r4, ip, #16
    7b28:	e1a0542c 	lsr	r5, ip, #8
    7b2c:	e1a03c2c 	lsr	r3, ip, #24
    7b30:	e20440ff 	and	r4, r4, #255	; 0xff
    7b34:	e20cc0ff 	and	ip, ip, #255	; 0xff
    7b38:	e59f1070 	ldr	r1, [pc, #112]	; 7bb0 <mmc_startup+0x5fc>
    7b3c:	e20550ff 	and	r5, r5, #255	; 0xff
    7b40:	e28600d9 	add	r0, r6, #217	; 0xd9
    7b44:	e88d1030 	stm	sp, {r4, r5, ip}
    7b48:	eb0018f4 	bl	df20 <sprintf>
    7b4c:	e5962078 	ldr	r2, [r6, #120]
    7b50:	e28600ee 	add	r0, r6, #238	; 0xee
    7b54:	e1a03c22 	lsr	r3, r2, #24
    7b58:	e203300f 	and	r3, r3, #15	; 0xf
    7b5c:	e1a02e22 	lsr	r2, r2, #28
    7b60:	e59f104c 	ldr	r1, [pc, #76]	; 7bb4 <mmc_startup+0x600>
    7b64:	eb0018ed 	bl	df20 <sprintf>
    7b68:	e3a04000 	mov	r4, #0	; 0x0
    7b6c:	e1a00004 	mov	r0, r4
    7b70:	e28ddf8e 	add	sp, sp, #568	; 0x238
    7b74:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    7b78:	00010014 	.word	0x00010014
    7b7c:	00010022 	.word	0x00010022
    7b80:	00010030 	.word	0x00010030
    7b84:	00010040 	.word	0x00010040
    7b88:	00010012 	.word	0x00010012
    7b8c:	0001159c 	.word	0x0001159c
    7b90:	0001158c 	.word	0x0001158c
    7b94:	0001003f 	.word	0x0001003f
    7b98:	02faf080 	.word	0x02faf080
    7b9c:	017d7840 	.word	0x017d7840
    7ba0:	01312d00 	.word	0x01312d00
    7ba4:	03197500 	.word	0x03197500
    7ba8:	018cba80 	.word	0x018cba80
    7bac:	000132bf 	.word	0x000132bf
    7bb0:	000132d1 	.word	0x000132d1
    7bb4:	000132dc 	.word	0x000132dc

00007bb8 <mmc_init>:
    7bb8:	e92d4010 	push	{r4, lr}
    7bbc:	ebfffb5a 	bl	692c <find_mmc_device>
    7bc0:	e1a04000 	mov	r4, r0
    7bc4:	e1a0e00f 	mov	lr, pc
    7bc8:	e590f10c 	ldr	pc, [r0, #268]
    7bcc:	e3500000 	cmp	r0, #0	; 0x0
    7bd0:	18bd8010 	popne	{r4, pc}
    7bd4:	e3a03001 	mov	r3, #1	; 0x1
    7bd8:	e5843044 	str	r3, [r4, #68]
    7bdc:	e1a00004 	mov	r0, r4
    7be0:	e1a0e00f 	mov	lr, pc
    7be4:	e594f108 	ldr	pc, [r4, #264]
    7be8:	e1a00004 	mov	r0, r4
    7bec:	e59f1060 	ldr	r1, [pc, #96]	; 7c54 <mmc_init+0x9c>
    7bf0:	ebfffb9c 	bl	6a68 <mmc_set_clock>
    7bf4:	e1a00004 	mov	r0, r4
    7bf8:	ebfffd09 	bl	7024 <mmc_go_idle>
    7bfc:	e3500000 	cmp	r0, #0	; 0x0
    7c00:	18bd8010 	popne	{r4, pc}
    7c04:	e5c40083 	strb	r0, [r4, #131]
    7c08:	e1a00004 	mov	r0, r4
    7c0c:	ebfffba9 	bl	6ab8 <mmc_send_if_cond>
    7c10:	e1a00004 	mov	r0, r4
    7c14:	ebfffcad 	bl	6ed0 <sd_send_op_cond>
    7c18:	e3700013 	cmn	r0, #19	; 0x13
    7c1c:	1a000009 	bne	7c48 <mmc_init+0x90>
    7c20:	e1a00004 	mov	r0, r4
    7c24:	ebfffd13 	bl	7078 <mmc_send_op_cond>
    7c28:	e3500000 	cmp	r0, #0	; 0x0
    7c2c:	0a000005 	beq	7c48 <mmc_init+0x90>
    7c30:	e59f0020 	ldr	r0, [pc, #32]	; 7c58 <mmc_init+0xa0>
    7c34:	e59f3020 	ldr	r3, [pc, #32]	; 7c5c <mmc_init+0xa4>
    7c38:	e1a0e00f 	mov	lr, pc
    7c3c:	e593f000 	ldr	pc, [r3]
    7c40:	e3e00010 	mvn	r0, #16	; 0x10
    7c44:	e8bd8010 	pop	{r4, pc}
    7c48:	e1a00004 	mov	r0, r4
    7c4c:	e8bd4010 	pop	{r4, lr}
    7c50:	eafffe57 	b	75b4 <mmc_startup>
    7c54:	00061a80 	.word	0x00061a80
    7c58:	000132e2 	.word	0x000132e2
    7c5c:	0800d76c 	.word	0x0800d76c

00007c60 <mmc2_vs_reset>:
    7c60:	e3a00002 	mov	r0, #2	; 0x2
    7c64:	eaffffd3 	b	7bb8 <mmc_init>

00007c68 <mmc1_vs_reset>:
    7c68:	e3a00001 	mov	r0, #1	; 0x1
    7c6c:	eaffffd1 	b	7bb8 <mmc_init>

00007c70 <mmc0_vs_reset>:
    7c70:	e3a00000 	mov	r0, #0	; 0x0
    7c74:	eaffffcf 	b	7bb8 <mmc_init>

00007c78 <mmc_read_blocks>:
    7c78:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7c7c:	e3530001 	cmp	r3, #1	; 0x1
    7c80:	e24dd030 	sub	sp, sp, #48	; 0x30
    7c84:	e1a06003 	mov	r6, r3
    7c88:	83a03012 	movhi	r3, #18	; 0x12
    7c8c:	93a03011 	movls	r3, #17	; 0x11
    7c90:	e1cd30b0 	strh	r3, [sp]
    7c94:	e5903040 	ldr	r3, [r0, #64]
    7c98:	e3a07000 	mov	r7, #0	; 0x0
    7c9c:	e3530000 	cmp	r3, #0	; 0x0
    7ca0:	05903088 	ldreq	r3, [r0, #136]
    7ca4:	158d2008 	strne	r2, [sp, #8]
    7ca8:	00030293 	muleq	r3, r3, r2
    7cac:	058d3008 	streq	r3, [sp, #8]
    7cb0:	e5903088 	ldr	r3, [r0, #136]
    7cb4:	e3a02015 	mov	r2, #21	; 0x15
    7cb8:	e58d302c 	str	r3, [sp, #44]
    7cbc:	e3a03001 	mov	r3, #1	; 0x1
    7cc0:	e58d2004 	str	r2, [sp, #4]
    7cc4:	e58d1020 	str	r1, [sp, #32]
    7cc8:	e1a04000 	mov	r4, r0
    7ccc:	e58d3024 	str	r3, [sp, #36]
    7cd0:	e58d701c 	str	r7, [sp, #28]
    7cd4:	e58d6028 	str	r6, [sp, #40]
    7cd8:	e1a0100d 	mov	r1, sp
    7cdc:	e28d2020 	add	r2, sp, #32	; 0x20
    7ce0:	e1a0e00f 	mov	lr, pc
    7ce4:	e594f104 	ldr	pc, [r4, #260]
    7ce8:	e2505000 	subs	r5, r0, #0	; 0x0
    7cec:	e1a0800d 	mov	r8, sp
    7cf0:	11a00007 	movne	r0, r7
    7cf4:	1a000018 	bne	7d5c <mmc_read_blocks+0xe4>
    7cf8:	e3560001 	cmp	r6, #1	; 0x1
    7cfc:	9a000012 	bls	7d4c <mmc_read_blocks+0xd4>
    7d00:	e3a0301d 	mov	r3, #29	; 0x1d
    7d04:	e58d3004 	str	r3, [sp, #4]
    7d08:	e3a0300c 	mov	r3, #12	; 0xc
    7d0c:	e1cd30b0 	strh	r3, [sp]
    7d10:	e58d5008 	str	r5, [sp, #8]
    7d14:	e58d501c 	str	r5, [sp, #28]
    7d18:	e1a0100d 	mov	r1, sp
    7d1c:	e1a00004 	mov	r0, r4
    7d20:	e1a02005 	mov	r2, r5
    7d24:	e1a0e00f 	mov	lr, pc
    7d28:	e594f104 	ldr	pc, [r4, #260]
    7d2c:	e3500000 	cmp	r0, #0	; 0x0
    7d30:	0a000005 	beq	7d4c <mmc_read_blocks+0xd4>
    7d34:	e59f0028 	ldr	r0, [pc, #40]	; 7d64 <mmc_read_blocks+0xec>
    7d38:	e59f3028 	ldr	r3, [pc, #40]	; 7d68 <mmc_read_blocks+0xf0>
    7d3c:	e1a0e00f 	mov	lr, pc
    7d40:	e593f000 	ldr	pc, [r3]
    7d44:	e1a00005 	mov	r0, r5
    7d48:	ea000003 	b	7d5c <mmc_read_blocks+0xe4>
    7d4c:	e1a00004 	mov	r0, r4
    7d50:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    7d54:	ebfffd15 	bl	71b0 <mmc_send_status>
    7d58:	e1a00006 	mov	r0, r6
    7d5c:	e28dd030 	add	sp, sp, #48	; 0x30
    7d60:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    7d64:	000132a4 	.word	0x000132a4
    7d68:	0800d76c 	.word	0x0800d76c

00007d6c <mmc_bread>:
    7d6c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    7d70:	e252a000 	subs	sl, r2, #0	; 0x0
    7d74:	e24dd004 	sub	sp, sp, #4	; 0x4
    7d78:	e1a07001 	mov	r7, r1
    7d7c:	e1a08003 	mov	r8, r3
    7d80:	0a000027 	beq	7e24 <mmc_bread+0xb8>
    7d84:	ebfffae8 	bl	692c <find_mmc_device>
    7d88:	e2506000 	subs	r6, r0, #0	; 0x0
    7d8c:	0a000024 	beq	7e24 <mmc_bread+0xb8>
    7d90:	e59620a8 	ldr	r2, [r6, #168]
    7d94:	e08a1007 	add	r1, sl, r7
    7d98:	e1510002 	cmp	r1, r2
    7d9c:	9a000004 	bls	7db4 <mmc_bread+0x48>
    7da0:	e59f0088 	ldr	r0, [pc, #136]	; 7e30 <mmc_bread+0xc4>
    7da4:	e59f3088 	ldr	r3, [pc, #136]	; 7e34 <mmc_bread+0xc8>
    7da8:	e1a0e00f 	mov	lr, pc
    7dac:	e593f000 	ldr	pc, [r3]
    7db0:	ea00001b 	b	7e24 <mmc_bread+0xb8>
    7db4:	e5961088 	ldr	r1, [r6, #136]
    7db8:	ebfffacd 	bl	68f4 <mmc_set_blocklen>
    7dbc:	e3500000 	cmp	r0, #0	; 0x0
    7dc0:	01a0500a 	moveq	r5, sl
    7dc4:	0a000005 	beq	7de0 <mmc_bread+0x74>
    7dc8:	e5961088 	ldr	r1, [r6, #136]
    7dcc:	e59f0064 	ldr	r0, [pc, #100]	; 7e38 <mmc_bread+0xcc>
    7dd0:	e59f305c 	ldr	r3, [pc, #92]	; 7e34 <mmc_bread+0xc8>
    7dd4:	e1a0e00f 	mov	lr, pc
    7dd8:	e593f000 	ldr	pc, [r3]
    7ddc:	ea000010 	b	7e24 <mmc_bread+0xb8>
    7de0:	e5964110 	ldr	r4, [r6, #272]
    7de4:	e1a02007 	mov	r2, r7
    7de8:	e1550004 	cmp	r5, r4
    7dec:	31a04005 	movcc	r4, r5
    7df0:	e1a01008 	mov	r1, r8
    7df4:	e1a00006 	mov	r0, r6
    7df8:	e1a03004 	mov	r3, r4
    7dfc:	ebffff9d 	bl	7c78 <mmc_read_blocks>
    7e00:	e1500004 	cmp	r0, r4
    7e04:	e0877000 	add	r7, r7, r0
    7e08:	1a000005 	bne	7e24 <mmc_bread+0xb8>
    7e0c:	e0555000 	subs	r5, r5, r0
    7e10:	e5962088 	ldr	r2, [r6, #136]
    7e14:	10288092 	mlane	r8, r2, r0, r8
    7e18:	1afffff0 	bne	7de0 <mmc_bread+0x74>
    7e1c:	e000029a 	mul	r0, sl, r2
    7e20:	ea000000 	b	7e28 <mmc_bread+0xbc>
    7e24:	e3a00000 	mov	r0, #0	; 0x0
    7e28:	e28dd004 	add	sp, sp, #4	; 0x4
    7e2c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    7e30:	00013266 	.word	0x00013266
    7e34:	0800d76c 	.word	0x0800d76c
    7e38:	0001330b 	.word	0x0001330b

00007e3c <mmc2_vs_read>:
    7e3c:	e1a024a2 	lsr	r2, r2, #9
    7e40:	e1821b83 	orr	r1, r2, r3, lsl #23
    7e44:	e59d2000 	ldr	r2, [sp]
    7e48:	e1a03000 	mov	r3, r0
    7e4c:	e1a024c2 	asr	r2, r2, #9
    7e50:	e3a00002 	mov	r0, #2	; 0x2
    7e54:	eaffffc4 	b	7d6c <mmc_bread>

00007e58 <mmc1_vs_read>:
    7e58:	e1a024a2 	lsr	r2, r2, #9
    7e5c:	e1821b83 	orr	r1, r2, r3, lsl #23
    7e60:	e59d2000 	ldr	r2, [sp]
    7e64:	e1a03000 	mov	r3, r0
    7e68:	e1a024c2 	asr	r2, r2, #9
    7e6c:	e3a00001 	mov	r0, #1	; 0x1
    7e70:	eaffffbd 	b	7d6c <mmc_bread>

00007e74 <mmc0_vs_read>:
    7e74:	e1a024a2 	lsr	r2, r2, #9
    7e78:	e1821b83 	orr	r1, r2, r3, lsl #23
    7e7c:	e59d2000 	ldr	r2, [sp]
    7e80:	e1a03000 	mov	r3, r0
    7e84:	e1a024c2 	asr	r2, r2, #9
    7e88:	e3a00000 	mov	r0, #0	; 0x0
    7e8c:	eaffffb6 	b	7d6c <mmc_bread>

00007e90 <mci_pull_data32>:
    7e90:	e1a02142 	asr	r2, r2, #2
    7e94:	e3a0c000 	mov	ip, #0	; 0x0
    7e98:	ea000003 	b	7eac <mci_pull_data32+0x1c>
    7e9c:	e5903118 	ldr	r3, [r0, #280]
    7ea0:	e5933200 	ldr	r3, [r3, #512]
    7ea4:	e781300c 	str	r3, [r1, ip]
    7ea8:	e28cc004 	add	ip, ip, #4	; 0x4
    7eac:	e3520000 	cmp	r2, #0	; 0x0
    7eb0:	e2422001 	sub	r2, r2, #1	; 0x1
    7eb4:	cafffff8 	bgt	7e9c <mci_pull_data32+0xc>
    7eb8:	e12fff1e 	bx	lr

00007ebc <mci_push_data32>:
    7ebc:	e92d4010 	push	{r4, lr}
    7ec0:	e3a0c000 	mov	ip, #0	; 0x0
    7ec4:	e1a04000 	mov	r4, r0
    7ec8:	e1a00001 	mov	r0, r1
    7ecc:	e1a01142 	asr	r1, r2, #2
    7ed0:	ea000003 	b	7ee4 <mci_push_data32+0x28>
    7ed4:	e790200c 	ldr	r2, [r0, ip]
    7ed8:	e5943118 	ldr	r3, [r4, #280]
    7edc:	e28cc004 	add	ip, ip, #4	; 0x4
    7ee0:	e5832200 	str	r2, [r3, #512]
    7ee4:	e3510000 	cmp	r1, #0	; 0x0
    7ee8:	e2411001 	sub	r1, r1, #1	; 0x1
    7eec:	cafffff8 	bgt	7ed4 <mci_push_data32+0x18>
    7ef0:	e8bd8010 	pop	{r4, pc}

00007ef4 <imap_sdhc_initialize>:
    7ef4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    7ef8:	e3510000 	cmp	r1, #0	; 0x0
    7efc:	e24dd004 	sub	sp, sp, #4	; 0x4
    7f00:	1a000005 	bne	7f1c <imap_sdhc_initialize+0x28>
    7f04:	e59f00dc 	ldr	r0, [pc, #220]	; 7fe8 <imap_sdhc_initialize+0xf4>
    7f08:	e59f30dc 	ldr	r3, [pc, #220]	; 7fec <imap_sdhc_initialize+0xf8>
    7f0c:	e1a0e00f 	mov	lr, pc
    7f10:	e593f000 	ldr	pc, [r3]
    7f14:	e3e00004 	mvn	r0, #4	; 0x4
    7f18:	ea000030 	b	7fe0 <imap_sdhc_initialize+0xec>
    7f1c:	e3500000 	cmp	r0, #0	; 0x0
    7f20:	03a0359d 	moveq	r3, #658505728	; 0x27400000
    7f24:	0a000005 	beq	7f40 <imap_sdhc_initialize+0x4c>
    7f28:	e3500001 	cmp	r0, #1	; 0x1
    7f2c:	059f30bc 	ldreq	r3, [pc, #188]	; 7ff0 <imap_sdhc_initialize+0xfc>
    7f30:	0a000002 	beq	7f40 <imap_sdhc_initialize+0x4c>
    7f34:	e3500002 	cmp	r0, #2	; 0x2
    7f38:	1a000018 	bne	7fa0 <imap_sdhc_initialize+0xac>
    7f3c:	e59f30b0 	ldr	r3, [pc, #176]	; 7ff4 <imap_sdhc_initialize+0x100>
    7f40:	e5813118 	str	r3, [r1, #280]
    7f44:	e59f30ac 	ldr	r3, [pc, #172]	; 7ff8 <imap_sdhc_initialize+0x104>
    7f48:	e591c130 	ldr	ip, [r1, #304]
    7f4c:	e5813104 	str	r3, [r1, #260]
    7f50:	e59f30a4 	ldr	r3, [pc, #164]	; 7ffc <imap_sdhc_initialize+0x108>
    7f54:	e3a02000 	mov	r2, #0	; 0x0
    7f58:	e5813108 	str	r3, [r1, #264]
    7f5c:	e59f309c 	ldr	r3, [pc, #156]	; 8000 <imap_sdhc_initialize+0x10c>
    7f60:	e5812034 	str	r2, [r1, #52]
    7f64:	e581310c 	str	r3, [r1, #268]
    7f68:	e59f3094 	ldr	r3, [pc, #148]	; 8004 <imap_sdhc_initialize+0x110>
    7f6c:	e5812028 	str	r2, [r1, #40]
    7f70:	e581302c 	str	r3, [r1, #44]
    7f74:	e3003101 	movw	r3, #257	; 0x101
    7f78:	e5813050 	str	r3, [r1, #80]
    7f7c:	e59f3084 	ldr	r3, [pc, #132]	; 8008 <imap_sdhc_initialize+0x114>
    7f80:	e5812110 	str	r2, [r1, #272]
    7f84:	e5813038 	str	r3, [r1, #56]
    7f88:	e59f307c 	ldr	r3, [pc, #124]	; 800c <imap_sdhc_initialize+0x118>
    7f8c:	e5810114 	str	r0, [r1, #276]
    7f90:	e581303c 	str	r3, [r1, #60]
    7f94:	e28c2b01 	add	r2, ip, #1024	; 0x400
    7f98:	e28c3010 	add	r3, ip, #16	; 0x10
    7f9c:	ea000005 	b	7fb8 <imap_sdhc_initialize+0xc4>
    7fa0:	e59f0068 	ldr	r0, [pc, #104]	; 8010 <imap_sdhc_initialize+0x11c>
    7fa4:	e59f3040 	ldr	r3, [pc, #64]	; 7fec <imap_sdhc_initialize+0xf8>
    7fa8:	e1a0e00f 	mov	lr, pc
    7fac:	e593f000 	ldr	pc, [r3]
    7fb0:	e3e00000 	mvn	r0, #0	; 0x0
    7fb4:	ea000009 	b	7fe0 <imap_sdhc_initialize+0xec>
    7fb8:	e5033004 	str	r3, [r3, #-4]
    7fbc:	e2833010 	add	r3, r3, #16	; 0x10
    7fc0:	e1530002 	cmp	r3, r2
    7fc4:	1afffffb 	bne	7fb8 <imap_sdhc_initialize+0xc4>
    7fc8:	e3a03020 	mov	r3, #32	; 0x20
    7fcc:	e58c33f0 	str	r3, [ip, #1008]
    7fd0:	e58cc3fc 	str	ip, [ip, #1020]
    7fd4:	e1a00001 	mov	r0, r1
    7fd8:	ebfffad4 	bl	6b30 <mmc_register>
    7fdc:	e3a00000 	mov	r0, #0	; 0x0
    7fe0:	e28dd004 	add	sp, sp, #4	; 0x4
    7fe4:	e8bd8000 	pop	{pc}
    7fe8:	00013315 	.word	0x00013315
    7fec:	0800d76c 	.word	0x0800d76c
    7ff0:	27200000 	.word	0x27200000
    7ff4:	27300000 	.word	0x27300000
    7ff8:	00008084 	.word	0x00008084
    7ffc:	00008568 	.word	0x00008568
    8000:	000083a8 	.word	0x000083a8
    8004:	00ff8000 	.word	0x00ff8000
    8008:	00061a80 	.word	0x00061a80
    800c:	018cba80 	.word	0x018cba80
    8010:	0001332b 	.word	0x0001332b

00008014 <mci_send_cmd>:
    8014:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8018:	e5903118 	ldr	r3, [r0, #280]
    801c:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    8020:	e5832028 	str	r2, [r3, #40]
    8024:	e5903118 	ldr	r3, [r0, #280]
    8028:	e24dd004 	sub	sp, sp, #4	; 0x4
    802c:	e583102c 	str	r1, [r3, #44]
    8030:	e1a04000 	mov	r4, r0
    8034:	eb0001f4 	bl	880c <get_ticks>
    8038:	e59f5040 	ldr	r5, [pc, #64]	; 8080 <mci_send_cmd+0x6c>
    803c:	e1a06000 	mov	r6, r0
    8040:	e1a07001 	mov	r7, r1
    8044:	e5943118 	ldr	r3, [r4, #280]
    8048:	e593302c 	ldr	r3, [r3, #44]
    804c:	e3530000 	cmp	r3, #0	; 0x0
    8050:	aa000008 	bge	8078 <mci_send_cmd+0x64>
    8054:	eb0001ec 	bl	880c <get_ticks>
    8058:	e1a02000 	mov	r2, r0
    805c:	e1a03001 	mov	r3, r1
    8060:	e0522006 	subs	r2, r2, r6
    8064:	e0c33007 	sbc	r3, r3, r7
    8068:	e3530000 	cmp	r3, #0	; 0x0
    806c:	1a000001 	bne	8078 <mci_send_cmd+0x64>
    8070:	e1520005 	cmp	r2, r5
    8074:	9afffff2 	bls	8044 <mci_send_cmd+0x30>
    8078:	e28dd004 	add	sp, sp, #4	; 0x4
    807c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8080:	0007a11f 	.word	0x0007a11f

00008084 <imap_sdhc_send_cmd>:
    8084:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    8088:	e252a000 	subs	sl, r2, #0	; 0x0
    808c:	e24dd004 	sub	sp, sp, #4	; 0x4
    8090:	e1a04000 	mov	r4, r0
    8094:	e1a08001 	mov	r8, r1
    8098:	0a000038 	beq	8180 <imap_sdhc_send_cmd+0xfc>
    809c:	e99a000c 	ldmib	sl, {r2, r3}
    80a0:	e59a100c 	ldr	r1, [sl, #12]
    80a4:	e3120001 	tst	r2, #1	; 0x1
    80a8:	e0000193 	mul	r0, r3, r1
    80ac:	15942118 	ldrne	r2, [r4, #280]
    80b0:	11a03801 	lslne	r3, r1, #16
    80b4:	13833001 	orrne	r3, r3, #1	; 0x1
    80b8:	15823100 	strne	r3, [r2, #256]
    80bc:	e59a200c 	ldr	r2, [sl, #12]
    80c0:	e5943118 	ldr	r3, [r4, #280]
    80c4:	e1a0e000 	mov	lr, r0
    80c8:	e583201c 	str	r2, [r3, #28]
    80cc:	e5943118 	ldr	r3, [r4, #280]
    80d0:	e3a05000 	mov	r5, #0	; 0x0
    80d4:	e5830020 	str	r0, [r3, #32]
    80d8:	e5942118 	ldr	r2, [r4, #280]
    80dc:	e3a0714a 	mov	r7, #-2147483630	; 0x80000012
    80e0:	e5923000 	ldr	r3, [r2]
    80e4:	e3833020 	orr	r3, r3, #32	; 0x20
    80e8:	e5823000 	str	r3, [r2]
    80ec:	e59a6000 	ldr	r6, [sl]
    80f0:	e594c130 	ldr	ip, [r4, #304]
    80f4:	e35e0eff 	cmp	lr, #4080	; 0xff0
    80f8:	31a0100e 	movcc	r1, lr
    80fc:	23a01eff 	movcs	r1, #4080	; 0xff0
    8100:	e59c0004 	ldr	r0, [ip, #4]
    8104:	e59f328c 	ldr	r3, [pc, #652]	; 8398 <imap_sdhc_send_cmd+0x314>
    8108:	e1a02a01 	lsl	r2, r1, #20
    810c:	e0003003 	and	r3, r0, r3
    8110:	e1a02a22 	lsr	r2, r2, #20
    8114:	e1822003 	orr	r2, r2, r3
    8118:	e05ee001 	subs	lr, lr, r1
    811c:	e0853006 	add	r3, r5, r6
    8120:	e98c000c 	stmib	ip, {r2, r3}
    8124:	e58c7000 	str	r7, [ip]
    8128:	e1a0200c 	mov	r2, ip
    812c:	e0855001 	add	r5, r5, r1
    8130:	e28cc010 	add	ip, ip, #16	; 0x10
    8134:	1affffee 	bne	80f4 <imap_sdhc_send_cmd+0x70>
    8138:	e3c73002 	bic	r3, r7, #2	; 0x2
    813c:	e3833004 	orr	r3, r3, #4	; 0x4
    8140:	e5823000 	str	r3, [r2]
    8144:	e5942130 	ldr	r2, [r4, #304]
    8148:	e5923000 	ldr	r3, [r2]
    814c:	e3833008 	orr	r3, r3, #8	; 0x8
    8150:	e5823000 	str	r3, [r2]
    8154:	e5942118 	ldr	r2, [r4, #280]
    8158:	e5923000 	ldr	r3, [r2]
    815c:	e3833402 	orr	r3, r3, #33554432	; 0x2000000
    8160:	e5823000 	str	r3, [r2]
    8164:	e5942118 	ldr	r2, [r4, #280]
    8168:	e5923080 	ldr	r3, [r2, #128]
    816c:	e3833080 	orr	r3, r3, #128	; 0x80
    8170:	e5823080 	str	r3, [r2, #128]
    8174:	e5942118 	ldr	r2, [r4, #280]
    8178:	e3a03001 	mov	r3, #1	; 0x1
    817c:	e5823084 	str	r3, [r2, #132]
    8180:	e1d800b0 	ldrh	r0, [r8]
    8184:	e5982004 	ldr	r2, [r8, #4]
    8188:	e350000c 	cmp	r0, #12	; 0xc
    818c:	03a03901 	moveq	r3, #16384	; 0x4000
    8190:	13a03a02 	movne	r3, #8192	; 0x2000
    8194:	e3120004 	tst	r2, #4	; 0x4
    8198:	13833c01 	orrne	r3, r3, #256	; 0x100
    819c:	e3120001 	tst	r2, #1	; 0x1
    81a0:	e3831202 	orr	r1, r3, #536870912	; 0x20000000
    81a4:	0a000002 	beq	81b4 <imap_sdhc_send_cmd+0x130>
    81a8:	e3120002 	tst	r2, #2	; 0x2
    81ac:	e3811040 	orr	r1, r1, #64	; 0x40
    81b0:	13811080 	orrne	r1, r1, #128	; 0x80
    81b4:	e35a0000 	cmp	sl, #0	; 0x0
    81b8:	0a000003 	beq	81cc <imap_sdhc_send_cmd+0x148>
    81bc:	e59a3004 	ldr	r3, [sl, #4]
    81c0:	e3811c02 	orr	r1, r1, #512	; 0x200
    81c4:	e3130002 	tst	r3, #2	; 0x2
    81c8:	13811b01 	orrne	r1, r1, #1024	; 0x400
    81cc:	e594312c 	ldr	r3, [r4, #300]
    81d0:	e1811000 	orr	r1, r1, r0
    81d4:	e3530000 	cmp	r3, #0	; 0x0
    81d8:	13a03000 	movne	r3, #0	; 0x0
    81dc:	1584312c 	strne	r3, [r4, #300]
    81e0:	e5982008 	ldr	r2, [r8, #8]
    81e4:	e5943118 	ldr	r3, [r4, #280]
    81e8:	13811902 	orrne	r1, r1, #32768	; 0x8000
    81ec:	e5832028 	str	r2, [r3, #40]
    81f0:	e5943118 	ldr	r3, [r4, #280]
    81f4:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    81f8:	e583102c 	str	r1, [r3, #44]
    81fc:	eb000182 	bl	880c <get_ticks>
    8200:	e3005bb8 	movw	r5, #3000	; 0xbb8
    8204:	e1a06000 	mov	r6, r0
    8208:	e1a07001 	mov	r7, r1
    820c:	eb00017e 	bl	880c <get_ticks>
    8210:	e1a02000 	mov	r2, r0
    8214:	e1a03001 	mov	r3, r1
    8218:	e0522006 	subs	r2, r2, r6
    821c:	e0c33007 	sbc	r3, r3, r7
    8220:	e3530000 	cmp	r3, #0	; 0x0
    8224:	1a000007 	bne	8248 <imap_sdhc_send_cmd+0x1c4>
    8228:	e1520005 	cmp	r2, r5
    822c:	8a000005 	bhi	8248 <imap_sdhc_send_cmd+0x1c4>
    8230:	e5943118 	ldr	r3, [r4, #280]
    8234:	e5932044 	ldr	r2, [r3, #68]
    8238:	e2831044 	add	r1, r3, #68	; 0x44
    823c:	e3120004 	tst	r2, #4	; 0x4
    8240:	0afffff1 	beq	820c <imap_sdhc_send_cmd+0x188>
    8244:	ea000005 	b	8260 <imap_sdhc_send_cmd+0x1dc>
    8248:	e59f014c 	ldr	r0, [pc, #332]	; 839c <imap_sdhc_send_cmd+0x318>
    824c:	e59f314c 	ldr	r3, [pc, #332]	; 83a0 <imap_sdhc_send_cmd+0x31c>
    8250:	e1a0e00f 	mov	lr, pc
    8254:	e593f000 	ldr	pc, [r3]
    8258:	e3e00011 	mvn	r0, #17	; 0x11
    825c:	ea00004b 	b	8390 <imap_sdhc_send_cmd+0x30c>
    8260:	e5915000 	ldr	r5, [r1]
    8264:	e3003142 	movw	r3, #322	; 0x142
    8268:	e0053003 	and	r3, r5, r3
    826c:	e3530000 	cmp	r3, #0	; 0x0
    8270:	e3a03004 	mov	r3, #4	; 0x4
    8274:	e5813000 	str	r3, [r1]
    8278:	0a00000e 	beq	82b8 <imap_sdhc_send_cmd+0x234>
    827c:	e1d810b0 	ldrh	r1, [r8]
    8280:	e5943118 	ldr	r3, [r4, #280]
    8284:	e3510037 	cmp	r1, #55	; 0x37
    8288:	13510008 	cmpne	r1, #8	; 0x8
    828c:	e5835044 	str	r5, [r3, #68]
    8290:	0a000004 	beq	82a8 <imap_sdhc_send_cmd+0x224>
    8294:	e59f0108 	ldr	r0, [pc, #264]	; 83a4 <imap_sdhc_send_cmd+0x320>
    8298:	e1a02005 	mov	r2, r5
    829c:	e59f30fc 	ldr	r3, [pc, #252]	; 83a0 <imap_sdhc_send_cmd+0x31c>
    82a0:	e1a0e00f 	mov	lr, pc
    82a4:	e593f000 	ldr	pc, [r3]
    82a8:	e3150c01 	tst	r5, #256	; 0x100
    82ac:	13e00012 	mvnne	r0, #18	; 0x12
    82b0:	03e00011 	mvneq	r0, #17	; 0x11
    82b4:	ea000035 	b	8390 <imap_sdhc_send_cmd+0x30c>
    82b8:	e5983004 	ldr	r3, [r8, #4]
    82bc:	e5942118 	ldr	r2, [r4, #280]
    82c0:	e3130002 	tst	r3, #2	; 0x2
    82c4:	05923030 	ldreq	r3, [r2, #48]
    82c8:	0588300c 	streq	r3, [r8, #12]
    82cc:	0a00000a 	beq	82fc <imap_sdhc_send_cmd+0x278>
    82d0:	e592303c 	ldr	r3, [r2, #60]
    82d4:	e588300c 	str	r3, [r8, #12]
    82d8:	e5943118 	ldr	r3, [r4, #280]
    82dc:	e5933038 	ldr	r3, [r3, #56]
    82e0:	e5883010 	str	r3, [r8, #16]
    82e4:	e5943118 	ldr	r3, [r4, #280]
    82e8:	e5933034 	ldr	r3, [r3, #52]
    82ec:	e5883014 	str	r3, [r8, #20]
    82f0:	e5943118 	ldr	r3, [r4, #280]
    82f4:	e5933030 	ldr	r3, [r3, #48]
    82f8:	e5883018 	str	r3, [r8, #24]
    82fc:	e35a0000 	cmp	sl, #0	; 0x0
    8300:	01a0000a 	moveq	r0, sl
    8304:	0a000021 	beq	8390 <imap_sdhc_send_cmd+0x30c>
    8308:	e594c118 	ldr	ip, [r4, #280]
    830c:	e59a3004 	ldr	r3, [sl, #4]
    8310:	e28c7044 	add	r7, ip, #68	; 0x44
    8314:	e2030001 	and	r0, r3, #1	; 0x1
    8318:	e28c608c 	add	r6, ip, #140	; 0x8c
    831c:	e5975000 	ldr	r5, [r7]
    8320:	e3500000 	cmp	r0, #0	; 0x0
    8324:	e596e000 	ldr	lr, [r6]
    8328:	e2051008 	and	r1, r5, #8	; 0x8
    832c:	0a000005 	beq	8348 <imap_sdhc_send_cmd+0x2c4>
    8330:	e3510000 	cmp	r1, #0	; 0x0
    8334:	0afffff8 	beq	831c <imap_sdhc_send_cmd+0x298>
    8338:	e31e0002 	tst	lr, #2	; 0x2
    833c:	0afffff6 	beq	831c <imap_sdhc_send_cmd+0x298>
    8340:	e3a03002 	mov	r3, #2	; 0x2
    8344:	ea000004 	b	835c <imap_sdhc_send_cmd+0x2d8>
    8348:	e3510000 	cmp	r1, #0	; 0x0
    834c:	0afffff2 	beq	831c <imap_sdhc_send_cmd+0x298>
    8350:	e31e0001 	tst	lr, #1	; 0x1
    8354:	0afffff0 	beq	831c <imap_sdhc_send_cmd+0x298>
    8358:	e3a03001 	mov	r3, #1	; 0x1
    835c:	e58c308c 	str	r3, [ip, #140]
    8360:	e5943118 	ldr	r3, [r4, #280]
    8364:	e3a00000 	mov	r0, #0	; 0x0
    8368:	e5835044 	str	r5, [r3, #68]
    836c:	e5942118 	ldr	r2, [r4, #280]
    8370:	e5923000 	ldr	r3, [r2]
    8374:	e3c33402 	bic	r3, r3, #33554432	; 0x2000000
    8378:	e3833004 	orr	r3, r3, #4	; 0x4
    837c:	e5823000 	str	r3, [r2]
    8380:	e5942118 	ldr	r2, [r4, #280]
    8384:	e5923080 	ldr	r3, [r2, #128]
    8388:	e3c33080 	bic	r3, r3, #128	; 0x80
    838c:	e5823080 	str	r3, [r2, #128]
    8390:	e28dd004 	add	sp, sp, #4	; 0x4
    8394:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    8398:	03ffe000 	.word	0x03ffe000
    839c:	0001333d 	.word	0x0001333d
    83a0:	0800d76c 	.word	0x0800d76c
    83a4:	0001334b 	.word	0x0001334b

000083a8 <imap_sdhc_init>:
    83a8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    83ac:	e5901114 	ldr	r1, [r0, #276]
    83b0:	e3a02000 	mov	r2, #0	; 0x0
    83b4:	e3a03001 	mov	r3, #1	; 0x1
    83b8:	e3510001 	cmp	r1, #1	; 0x1
    83bc:	e24dd004 	sub	sp, sp, #4	; 0x4
    83c0:	e1a04000 	mov	r4, r0
    83c4:	e5802048 	str	r2, [r0, #72]
    83c8:	e580312c 	str	r3, [r0, #300]
    83cc:	e580211c 	str	r2, [r0, #284]
    83d0:	0a000007 	beq	83f4 <imap_sdhc_init+0x4c>
    83d4:	3a000002 	bcc	83e4 <imap_sdhc_init+0x3c>
    83d8:	e3510002 	cmp	r1, #2	; 0x2
    83dc:	1a00000c 	bne	8414 <imap_sdhc_init+0x6c>
    83e0:	ea000007 	b	8404 <imap_sdhc_init+0x5c>
    83e4:	e3a0000c 	mov	r0, #12	; 0xc
    83e8:	ebffe111 	bl	834 <pads_init_module>
    83ec:	e59f0150 	ldr	r0, [pc, #336]	; 8544 <imap_sdhc_init+0x19c>
    83f0:	ea000006 	b	8410 <imap_sdhc_init+0x68>
    83f4:	e3a0000d 	mov	r0, #13	; 0xd
    83f8:	ebffe10d 	bl	834 <pads_init_module>
    83fc:	e59f0144 	ldr	r0, [pc, #324]	; 8548 <imap_sdhc_init+0x1a0>
    8400:	ea000002 	b	8410 <imap_sdhc_init+0x68>
    8404:	e3a0000e 	mov	r0, #14	; 0xe
    8408:	ebffe109 	bl	834 <pads_init_module>
    840c:	e59f0138 	ldr	r0, [pc, #312]	; 854c <imap_sdhc_init+0x1a4>
    8410:	ebffe0c0 	bl	718 <module_enable>
    8414:	e5942118 	ldr	r2, [r4, #280]
    8418:	e59f5130 	ldr	r5, [pc, #304]	; 8550 <imap_sdhc_init+0x1a8>
    841c:	e5923070 	ldr	r3, [r2, #112]
    8420:	e1a03943 	asr	r3, r3, #18
    8424:	e2033007 	and	r3, r3, #7	; 0x7
    8428:	e3530002 	cmp	r3, #2	; 0x2
    842c:	03a03003 	moveq	r3, #3	; 0x3
    8430:	13a03002 	movne	r3, #2	; 0x2
    8434:	e5843120 	str	r3, [r4, #288]
    8438:	e3a03007 	mov	r3, #7	; 0x7
    843c:	e5823000 	str	r3, [r2]
    8440:	e59f310c 	ldr	r3, [pc, #268]	; 8554 <imap_sdhc_init+0x1ac>
    8444:	e5843124 	str	r3, [r4, #292]
    8448:	e59f3108 	ldr	r3, [pc, #264]	; 8558 <imap_sdhc_init+0x1b0>
    844c:	e5843128 	str	r3, [r4, #296]
    8450:	eb0000ed 	bl	880c <get_ticks>
    8454:	e1a06000 	mov	r6, r0
    8458:	e1a07001 	mov	r7, r1
    845c:	e5943118 	ldr	r3, [r4, #280]
    8460:	e3a00001 	mov	r0, #1	; 0x1
    8464:	e5933000 	ldr	r3, [r3]
    8468:	e3130007 	tst	r3, #7	; 0x7
    846c:	0a00000d 	beq	84a8 <imap_sdhc_init+0x100>
    8470:	eb0000f6 	bl	8850 <udelay>
    8474:	eb0000e4 	bl	880c <get_ticks>
    8478:	e1a02000 	mov	r2, r0
    847c:	e1a03001 	mov	r3, r1
    8480:	e0522006 	subs	r2, r2, r6
    8484:	e0c33007 	sbc	r3, r3, r7
    8488:	e3530000 	cmp	r3, #0	; 0x0
    848c:	1a000001 	bne	8498 <imap_sdhc_init+0xf0>
    8490:	e1520005 	cmp	r2, r5
    8494:	9afffff0 	bls	845c <imap_sdhc_init+0xb4>
    8498:	e59f00bc 	ldr	r0, [pc, #188]	; 855c <imap_sdhc_init+0x1b4>
    849c:	e59f30bc 	ldr	r3, [pc, #188]	; 8560 <imap_sdhc_init+0x1b8>
    84a0:	e1a0e00f 	mov	lr, pc
    84a4:	e593f000 	ldr	pc, [r3]
    84a8:	e5943118 	ldr	r3, [r4, #280]
    84ac:	e3a01000 	mov	r1, #0	; 0x0
    84b0:	e5831024 	str	r1, [r3, #36]
    84b4:	e5943118 	ldr	r3, [r4, #280]
    84b8:	e3e02000 	mvn	r2, #0	; 0x0
    84bc:	e5832044 	str	r2, [r3, #68]
    84c0:	e5943118 	ldr	r3, [r4, #280]
    84c4:	e1a00001 	mov	r0, r1
    84c8:	e5832014 	str	r2, [r3, #20]
    84cc:	e5942118 	ldr	r2, [r4, #280]
    84d0:	e592304c 	ldr	r3, [r2, #76]
    84d4:	e1a03283 	lsl	r3, r3, #5
    84d8:	e1a03b23 	lsr	r3, r3, #22
    84dc:	e1833803 	orr	r3, r3, r3, lsl #16
    84e0:	e582304c 	str	r3, [r2, #76]
    84e4:	e5943118 	ldr	r3, [r4, #280]
    84e8:	e5831010 	str	r1, [r3, #16]
    84ec:	e5942118 	ldr	r2, [r4, #280]
    84f0:	e5923004 	ldr	r3, [r2, #4]
    84f4:	e3833001 	orr	r3, r3, #1	; 0x1
    84f8:	e5823004 	str	r3, [r2, #4]
    84fc:	e5943118 	ldr	r3, [r4, #280]
    8500:	e5831058 	str	r1, [r3, #88]
    8504:	e5942118 	ldr	r2, [r4, #280]
    8508:	e59f3054 	ldr	r3, [pc, #84]	; 8564 <imap_sdhc_init+0x1bc>
    850c:	e5823108 	str	r3, [r2, #264]
    8510:	e5942118 	ldr	r2, [r4, #280]
    8514:	e3003103 	movw	r3, #259	; 0x103
    8518:	e5823090 	str	r3, [r2, #144]
    851c:	e5942130 	ldr	r2, [r4, #304]
    8520:	e5943118 	ldr	r3, [r4, #280]
    8524:	e5832088 	str	r2, [r3, #136]
    8528:	e5942118 	ldr	r2, [r4, #280]
    852c:	e5923080 	ldr	r3, [r2, #128]
    8530:	e3c3307c 	bic	r3, r3, #124	; 0x7c
    8534:	e3833008 	orr	r3, r3, #8	; 0x8
    8538:	e5823080 	str	r3, [r2, #128]
    853c:	e28dd004 	add	sp, sp, #4	; 0x4
    8540:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8544:	2d039000 	.word	0x2d039000
    8548:	2d038800 	.word	0x2d038800
    854c:	2d038c00 	.word	0x2d038c00
    8550:	0007a11f 	.word	0x0007a11f
    8554:	00007e90 	.word	0x00007e90
    8558:	00007ebc 	.word	0x00007ebc
    855c:	00013363 	.word	0x00013363
    8560:	0800d76c 	.word	0x0800d76c
    8564:	3fa80000 	.word	0x3fa80000

00008568 <imap_sdhc_set_ios>:
    8568:	e5902048 	ldr	r2, [r0, #72]
    856c:	e590311c 	ldr	r3, [r0, #284]
    8570:	e92d4070 	push	{r4, r5, r6, lr}
    8574:	e1520003 	cmp	r2, r3
    8578:	e1a05000 	mov	r5, r0
    857c:	0a00002a 	beq	862c <imap_sdhc_set_ios+0xc4>
    8580:	e5903114 	ldr	r3, [r0, #276]
    8584:	e59f00d0 	ldr	r0, [pc, #208]	; 865c <imap_sdhc_set_ios+0xf4>
    8588:	e0830000 	add	r0, r3, r0
    858c:	e1a00200 	lsl	r0, r0, #4
    8590:	ebffe290 	bl	fd8 <module_get_clock>
    8594:	e5954048 	ldr	r4, [r5, #72]
    8598:	e1a06000 	mov	r6, r0
    859c:	e1a01004 	mov	r1, r4
    85a0:	eb001f52 	bl	102f0 <__aeabi_uidivmod>
    85a4:	e3510000 	cmp	r1, #0	; 0x0
    85a8:	0a000005 	beq	85c4 <imap_sdhc_set_ios+0x5c>
    85ac:	e1a00006 	mov	r0, r6
    85b0:	e1a01004 	mov	r1, r4
    85b4:	eb001ed1 	bl	10100 <__aeabi_uidiv>
    85b8:	e1a000a0 	lsr	r0, r0, #1
    85bc:	e2806001 	add	r6, r0, #1	; 0x1
    85c0:	ea000003 	b	85d4 <imap_sdhc_set_ios+0x6c>
    85c4:	e1a00006 	mov	r0, r6
    85c8:	e1a01004 	mov	r1, r4
    85cc:	eb001ecb 	bl	10100 <__aeabi_uidiv>
    85d0:	e1a060a0 	lsr	r6, r0, #1
    85d4:	e5953118 	ldr	r3, [r5, #280]
    85d8:	e3a04000 	mov	r4, #0	; 0x0
    85dc:	e5834010 	str	r4, [r3, #16]
    85e0:	e5953118 	ldr	r3, [r5, #280]
    85e4:	e1a00005 	mov	r0, r5
    85e8:	e583400c 	str	r4, [r3, #12]
    85ec:	e59f106c 	ldr	r1, [pc, #108]	; 8660 <imap_sdhc_set_ios+0xf8>
    85f0:	e1a02004 	mov	r2, r4
    85f4:	ebfffe86 	bl	8014 <mci_send_cmd>
    85f8:	e5953118 	ldr	r3, [r5, #280]
    85fc:	e1a00005 	mov	r0, r5
    8600:	e5836008 	str	r6, [r3, #8]
    8604:	e59f1054 	ldr	r1, [pc, #84]	; 8660 <imap_sdhc_set_ios+0xf8>
    8608:	e1a02004 	mov	r2, r4
    860c:	ebfffe80 	bl	8014 <mci_send_cmd>
    8610:	e5952118 	ldr	r2, [r5, #280]
    8614:	e3a03001 	mov	r3, #1	; 0x1
    8618:	e5823010 	str	r3, [r2, #16]
    861c:	e1a00005 	mov	r0, r5
    8620:	e1a02004 	mov	r2, r4
    8624:	e59f1034 	ldr	r1, [pc, #52]	; 8660 <imap_sdhc_set_ios+0xf8>
    8628:	ebfffe79 	bl	8014 <mci_send_cmd>
    862c:	e5953044 	ldr	r3, [r5, #68]
    8630:	e3530004 	cmp	r3, #4	; 0x4
    8634:	05952118 	ldreq	r2, [r5, #280]
    8638:	03a03001 	moveq	r3, #1	; 0x1
    863c:	05823018 	streq	r3, [r2, #24]
    8640:	08bd8070 	popeq	{r4, r5, r6, pc}
    8644:	e3530008 	cmp	r3, #8	; 0x8
    8648:	e5950118 	ldr	r0, [r5, #280]
    864c:	059f3010 	ldreq	r3, [pc, #16]	; 8664 <imap_sdhc_set_ios+0xfc>
    8650:	13a03000 	movne	r3, #0	; 0x0
    8654:	e5803018 	str	r3, [r0, #24]
    8658:	e8bd8070 	pop	{r4, r5, r6, pc}
    865c:	02d00a24 	.word	0x02d00a24
    8660:	00202000 	.word	0x00202000
    8664:	00010001 	.word	0x00010001

00008668 <serial_tstc>:
    8668:	e59f3010 	ldr	r3, [pc, #16]	; 8680 <serial_tstc+0x18>
    866c:	e5930018 	ldr	r0, [r3, #24]
    8670:	e1a00220 	lsr	r0, r0, #4
    8674:	e2200001 	eor	r0, r0, #1	; 0x1
    8678:	e2000001 	and	r0, r0, #1	; 0x1
    867c:	e12fff1e 	bx	lr
    8680:	20803000 	.word	0x20803000

00008684 <serial_putc>:
    8684:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8688:	e24dd004 	sub	sp, sp, #4	; 0x4
    868c:	e1a06000 	mov	r6, r0
    8690:	eb00005d 	bl	880c <get_ticks>
    8694:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    8698:	e3a05000 	mov	r5, #0	; 0x0
    869c:	e0944000 	adds	r4, r4, r0
    86a0:	e59f7034 	ldr	r7, [pc, #52]	; 86dc <serial_putc+0x58>
    86a4:	e0a55001 	adc	r5, r5, r1
    86a8:	e5973018 	ldr	r3, [r7, #24]
    86ac:	e3130020 	tst	r3, #32	; 0x20
    86b0:	0a000005 	beq	86cc <serial_putc+0x48>
    86b4:	eb000054 	bl	880c <get_ticks>
    86b8:	e1510005 	cmp	r1, r5
    86bc:	8a000002 	bhi	86cc <serial_putc+0x48>
    86c0:	1afffff8 	bne	86a8 <serial_putc+0x24>
    86c4:	e1500004 	cmp	r0, r4
    86c8:	9afffff6 	bls	86a8 <serial_putc+0x24>
    86cc:	e59f3008 	ldr	r3, [pc, #8]	; 86dc <serial_putc+0x58>
    86d0:	e5836000 	str	r6, [r3]
    86d4:	e28dd004 	add	sp, sp, #4	; 0x4
    86d8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    86dc:	20803000 	.word	0x20803000

000086e0 <serial_puts>:
    86e0:	e92d4030 	push	{r4, r5, lr}
    86e4:	e1a05000 	mov	r5, r0
    86e8:	e24dd004 	sub	sp, sp, #4	; 0x4
    86ec:	e3a04000 	mov	r4, #0	; 0x0
    86f0:	ea000006 	b	8710 <serial_puts+0x30>
    86f4:	e353000a 	cmp	r3, #10	; 0xa
    86f8:	1a000001 	bne	8704 <serial_puts+0x24>
    86fc:	e3a0000d 	mov	r0, #13	; 0xd
    8700:	ebffffdf 	bl	8684 <serial_putc>
    8704:	e7d50004 	ldrb	r0, [r5, r4]
    8708:	ebffffdd 	bl	8684 <serial_putc>
    870c:	e2844001 	add	r4, r4, #1	; 0x1
    8710:	e7d53004 	ldrb	r3, [r5, r4]
    8714:	e3530000 	cmp	r3, #0	; 0x0
    8718:	1afffff5 	bne	86f4 <serial_puts+0x14>
    871c:	e28dd004 	add	sp, sp, #4	; 0x4
    8720:	e8bd8030 	pop	{r4, r5, pc}

00008724 <serial_getc>:
    8724:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8728:	e59f2024 	ldr	r2, [pc, #36]	; 8754 <serial_getc+0x30>
    872c:	e24dd004 	sub	sp, sp, #4	; 0x4
    8730:	e5923018 	ldr	r3, [r2, #24]
    8734:	e3130010 	tst	r3, #16	; 0x10
    8738:	1afffffc 	bne	8730 <serial_getc+0xc>
    873c:	ebffef19 	bl	43a8 <tt_rnd_seed>
    8740:	e59f300c 	ldr	r3, [pc, #12]	; 8754 <serial_getc+0x30>
    8744:	e5930000 	ldr	r0, [r3]
    8748:	e6ef0070 	uxtb	r0, r0
    874c:	e28dd004 	add	sp, sp, #4	; 0x4
    8750:	e8bd8000 	pop	{pc}
    8754:	20803000 	.word	0x20803000

00008758 <serial_setbrg>:
    8758:	e92d4010 	push	{r4, lr}
    875c:	e59f0038 	ldr	r0, [pc, #56]	; 879c <serial_setbrg+0x44>
    8760:	ebffe21c 	bl	fd8 <module_get_clock>
    8764:	e59f1034 	ldr	r1, [pc, #52]	; 87a0 <serial_setbrg+0x48>
    8768:	e1a04000 	mov	r4, r0
    876c:	eb001ee5 	bl	10308 <__aeabi_idiv>
    8770:	e6bf3070 	sxth	r3, r0
    8774:	e59f0028 	ldr	r0, [pc, #40]	; 87a4 <serial_setbrg+0x4c>
    8778:	e59f1020 	ldr	r1, [pc, #32]	; 87a0 <serial_setbrg+0x48>
    877c:	e0204390 	mla	r0, r0, r3, r4
    8780:	e59f4020 	ldr	r4, [pc, #32]	; 87a8 <serial_setbrg+0x50>
    8784:	e1a00300 	lsl	r0, r0, #6
    8788:	e5843024 	str	r3, [r4, #36]
    878c:	eb001edd 	bl	10308 <__aeabi_idiv>
    8790:	e200003f 	and	r0, r0, #63	; 0x3f
    8794:	e5840028 	str	r0, [r4, #40]
    8798:	e8bd8010 	pop	{r4, pc}
    879c:	2d00a290 	.word	0x2d00a290
    87a0:	001c2000 	.word	0x001c2000
    87a4:	ffe3e000 	.word	0xffe3e000
    87a8:	20803000 	.word	0x20803000

000087ac <init_serial>:
    87ac:	e92d4010 	push	{r4, lr}
    87b0:	e59f004c 	ldr	r0, [pc, #76]	; 8804 <init_serial+0x58>
    87b4:	e59f404c 	ldr	r4, [pc, #76]	; 8808 <init_serial+0x5c>
    87b8:	ebffdfd6 	bl	718 <module_enable>
    87bc:	e3a0000f 	mov	r0, #15	; 0xf
    87c0:	ebffe01b 	bl	834 <pads_init_module>
    87c4:	e594302c 	ldr	r3, [r4, #44]
    87c8:	e3c330ff 	bic	r3, r3, #255	; 0xff
    87cc:	e3833060 	orr	r3, r3, #96	; 0x60
    87d0:	e584302c 	str	r3, [r4, #44]
    87d4:	ebffffdf 	bl	8758 <serial_setbrg>
    87d8:	e5943030 	ldr	r3, [r4, #48]
    87dc:	e3a00000 	mov	r0, #0	; 0x0
    87e0:	e3c33cff 	bic	r3, r3, #65280	; 0xff00
    87e4:	e3c33087 	bic	r3, r3, #135	; 0x87
    87e8:	e3833c03 	orr	r3, r3, #768	; 0x300
    87ec:	e3833001 	orr	r3, r3, #1	; 0x1
    87f0:	e5843030 	str	r3, [r4, #48]
    87f4:	e594302c 	ldr	r3, [r4, #44]
    87f8:	e3833010 	orr	r3, r3, #16	; 0x10
    87fc:	e584302c 	str	r3, [r4, #44]
    8800:	e8bd8010 	pop	{r4, pc}
    8804:	2d012000 	.word	0x2d012000
    8808:	20803000 	.word	0x20803000

0000880c <get_ticks>:
    880c:	e59f1038 	ldr	r1, [pc, #56]	; 884c <get_ticks+0x40>
    8810:	e5913204 	ldr	r3, [r1, #516]
    8814:	e5912200 	ldr	r2, [r1, #512]
    8818:	e591c204 	ldr	ip, [r1, #516]
    881c:	e15c0003 	cmp	ip, r3
    8820:	15913200 	ldrne	r3, [r1, #512]
    8824:	01a00002 	moveq	r0, r2
    8828:	03a01000 	moveq	r1, #0	; 0x0
    882c:	11a00003 	movne	r0, r3
    8830:	13a01000 	movne	r1, #0	; 0x0
    8834:	e3a02000 	mov	r2, #0	; 0x0
    8838:	e1822000 	orr	r2, r2, r0
    883c:	e18c3001 	orr	r3, ip, r1
    8840:	e1a01003 	mov	r1, r3
    8844:	e1a00002 	mov	r0, r2
    8848:	e12fff1e 	bx	lr
    884c:	21b00000 	.word	0x21b00000

00008850 <udelay>:
    8850:	e59f1084 	ldr	r1, [pc, #132]	; 88dc <udelay+0x8c>
    8854:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    8858:	e5913204 	ldr	r3, [r1, #516]
    885c:	e5912200 	ldr	r2, [r1, #512]
    8860:	e591c204 	ldr	ip, [r1, #516]
    8864:	e15c0003 	cmp	ip, r3
    8868:	15913200 	ldrne	r3, [r1, #512]
    886c:	01a04002 	moveq	r4, r2
    8870:	03a05000 	moveq	r5, #0	; 0x0
    8874:	11a04003 	movne	r4, r3
    8878:	13a05000 	movne	r5, #0	; 0x0
    887c:	e3a02000 	mov	r2, #0	; 0x0
    8880:	e1822004 	orr	r2, r2, r4
    8884:	e18c3005 	orr	r3, ip, r5
    8888:	e0926000 	adds	r6, r2, r0
    888c:	e59fc048 	ldr	ip, [pc, #72]	; 88dc <udelay+0x8c>
    8890:	e2a37000 	adc	r7, r3, #0	; 0x0
    8894:	e59c3204 	ldr	r3, [ip, #516]
    8898:	e59c2200 	ldr	r2, [ip, #512]
    889c:	e59c1204 	ldr	r1, [ip, #516]
    88a0:	e1510003 	cmp	r1, r3
    88a4:	159c3200 	ldrne	r3, [ip, #512]
    88a8:	01a04002 	moveq	r4, r2
    88ac:	03a05000 	moveq	r5, #0	; 0x0
    88b0:	11a04003 	movne	r4, r3
    88b4:	13a05000 	movne	r5, #0	; 0x0
    88b8:	e1851001 	orr	r1, r5, r1
    88bc:	e3a02000 	mov	r2, #0	; 0x0
    88c0:	e1570001 	cmp	r7, r1
    88c4:	e1840002 	orr	r0, r4, r2
    88c8:	8afffff1 	bhi	8894 <udelay+0x44>
    88cc:	18bd80f0 	popne	{r4, r5, r6, r7, pc}
    88d0:	e1560000 	cmp	r6, r0
    88d4:	8affffee 	bhi	8894 <udelay+0x44>
    88d8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    88dc:	21b00000 	.word	0x21b00000

000088e0 <get_timer>:
    88e0:	e59f2074 	ldr	r2, [pc, #116]	; 895c <get_timer+0x7c>
    88e4:	e92d4070 	push	{r4, r5, r6, lr}
    88e8:	e5923204 	ldr	r3, [r2, #516]
    88ec:	e1a06000 	mov	r6, r0
    88f0:	e5921200 	ldr	r1, [r2, #512]
    88f4:	e5920204 	ldr	r0, [r2, #516]
    88f8:	e24dd008 	sub	sp, sp, #8	; 0x8
    88fc:	e1500003 	cmp	r0, r3
    8900:	15923200 	ldrne	r3, [r2, #512]
    8904:	01a04001 	moveq	r4, r1
    8908:	03a05000 	moveq	r5, #0	; 0x0
    890c:	11a04003 	movne	r4, r3
    8910:	13a05000 	movne	r5, #0	; 0x0
    8914:	e1a03000 	mov	r3, r0
    8918:	e3a02000 	mov	r2, #0	; 0x0
    891c:	e1851003 	orr	r1, r5, r3
    8920:	e1840002 	orr	r0, r4, r2
    8924:	e1510002 	cmp	r1, r2
    8928:	e88d0003 	stm	sp, {r0, r1}
    892c:	1a000003 	bne	8940 <get_timer+0x60>
    8930:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8934:	eb001df1 	bl	10100 <__aeabi_uidiv>
    8938:	e58d0000 	str	r0, [sp]
    893c:	ea000002 	b	894c <get_timer+0x6c>
    8940:	e1a0000d 	mov	r0, sp
    8944:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8948:	eb0011ab 	bl	cffc <__div64_32>
    894c:	e59d0000 	ldr	r0, [sp]
    8950:	e0660000 	rsb	r0, r6, r0
    8954:	e28dd008 	add	sp, sp, #8	; 0x8
    8958:	e8bd8070 	pop	{r4, r5, r6, pc}
    895c:	21b00000 	.word	0x21b00000

00008960 <init_timer>:
    8960:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8964:	e3a00003 	mov	r0, #3	; 0x3
    8968:	e24dd004 	sub	sp, sp, #4	; 0x4
    896c:	ebffe199 	bl	fd8 <module_get_clock>
    8970:	e59f1030 	ldr	r1, [pc, #48]	; 89a8 <init_timer+0x48>
    8974:	eb001e63 	bl	10308 <__aeabi_idiv>
    8978:	e2401001 	sub	r1, r0, #1	; 0x1
    897c:	e59f2028 	ldr	r2, [pc, #40]	; 89ac <init_timer+0x4c>
    8980:	e1a01401 	lsl	r1, r1, #8
    8984:	e3a03000 	mov	r3, #0	; 0x0
    8988:	e3811001 	orr	r1, r1, #1	; 0x1
    898c:	e5823208 	str	r3, [r2, #520]
    8990:	e1a00003 	mov	r0, r3
    8994:	e5823200 	str	r3, [r2, #512]
    8998:	e5823204 	str	r3, [r2, #516]
    899c:	e5821208 	str	r1, [r2, #520]
    89a0:	e28dd004 	add	sp, sp, #4	; 0x4
    89a4:	e8bd8000 	pop	{pc}
    89a8:	000f4240 	.word	0x000f4240
    89ac:	21b00000 	.word	0x21b00000

000089b0 <e2p_vs_align>:
    89b0:	e3a00010 	mov	r0, #16	; 0x10
    89b4:	e12fff1e 	bx	lr

000089b8 <e2p_vs_reset>:
    89b8:	e3a00000 	mov	r0, #0	; 0x0
    89bc:	e12fff1e 	bx	lr

000089c0 <iic_address_num>:
    89c0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    89c4:	e24dd004 	sub	sp, sp, #4	; 0x4
    89c8:	ebffdef0 	bl	590 <boot_info>
    89cc:	e59f3014 	ldr	r3, [pc, #20]	; 89e8 <iic_address_num+0x28>
    89d0:	e1a00080 	lsl	r0, r0, #1
    89d4:	e20000ff 	and	r0, r0, #255	; 0xff
    89d8:	e5830000 	str	r0, [r3]
    89dc:	e3a00001 	mov	r0, #1	; 0x1
    89e0:	e28dd004 	add	sp, sp, #4	; 0x4
    89e4:	e8bd8000 	pop	{pc}
    89e8:	0800d250 	.word	0x0800d250

000089ec <iic_read_get_address>:
    89ec:	e1d020b2 	ldrh	r2, [r0, #2]
    89f0:	e590300c 	ldr	r3, [r0, #12]
    89f4:	e92d4010 	push	{r4, lr}
    89f8:	e0833002 	add	r3, r3, r2
    89fc:	e1c030b2 	strh	r3, [r0, #2]
    8a00:	e1a04000 	mov	r4, r0
    8a04:	ebffffed 	bl	89c0 <iic_address_num>
    8a08:	e3500000 	cmp	r0, #0	; 0x0
    8a0c:	1a000008 	bne	8a34 <iic_read_get_address+0x48>
    8a10:	e1d430b2 	ldrh	r3, [r4, #2]
    8a14:	e3530c01 	cmp	r3, #256	; 0x100
    8a18:	1a000005 	bne	8a34 <iic_read_get_address+0x48>
    8a1c:	e1d430b0 	ldrh	r3, [r4]
    8a20:	e1c400b2 	strh	r0, [r4, #2]
    8a24:	e2833002 	add	r3, r3, #2	; 0x2
    8a28:	e1c430b0 	strh	r3, [r4]
    8a2c:	e3a03001 	mov	r3, #1	; 0x1
    8a30:	e1c431b0 	strh	r3, [r4, #16]
    8a34:	ebffffe1 	bl	89c0 <iic_address_num>
    8a38:	e3500000 	cmp	r0, #0	; 0x0
    8a3c:	e5941008 	ldr	r1, [r4, #8]
    8a40:	0a000004 	beq	8a58 <iic_read_get_address+0x6c>
    8a44:	e3510000 	cmp	r1, #0	; 0x0
    8a48:	13a03000 	movne	r3, #0	; 0x0
    8a4c:	15843008 	strne	r3, [r4, #8]
    8a50:	1a00000e 	bne	8a90 <iic_read_get_address+0xa4>
    8a54:	ea000010 	b	8a9c <iic_read_get_address+0xb0>
    8a58:	e1d430b2 	ldrh	r3, [r4, #2]
    8a5c:	e2632c01 	rsb	r2, r3, #256	; 0x100
    8a60:	e1520001 	cmp	r2, r1
    8a64:	e5842004 	str	r2, [r4, #4]
    8a68:	2a000005 	bcs	8a84 <iic_read_get_address+0x98>
    8a6c:	e0623001 	rsb	r3, r2, r1
    8a70:	e5843008 	str	r3, [r4, #8]
    8a74:	e3a03001 	mov	r3, #1	; 0x1
    8a78:	e1c431b0 	strh	r3, [r4, #16]
    8a7c:	e584200c 	str	r2, [r4, #12]
    8a80:	ea000003 	b	8a94 <iic_read_get_address+0xa8>
    8a84:	e3510000 	cmp	r1, #0	; 0x0
    8a88:	0a000003 	beq	8a9c <iic_read_get_address+0xb0>
    8a8c:	e5840008 	str	r0, [r4, #8]
    8a90:	e584100c 	str	r1, [r4, #12]
    8a94:	e3a00001 	mov	r0, #1	; 0x1
    8a98:	e8bd8010 	pop	{r4, pc}
    8a9c:	e3a00000 	mov	r0, #0	; 0x0
    8aa0:	e8bd8010 	pop	{r4, pc}

00008aa4 <iic_write_get_address>:
    8aa4:	e1d020b2 	ldrh	r2, [r0, #2]
    8aa8:	e590300c 	ldr	r3, [r0, #12]
    8aac:	e92d4010 	push	{r4, lr}
    8ab0:	e0833002 	add	r3, r3, r2
    8ab4:	e1c030b2 	strh	r3, [r0, #2]
    8ab8:	e1a04000 	mov	r4, r0
    8abc:	ebffffbf 	bl	89c0 <iic_address_num>
    8ac0:	e3500000 	cmp	r0, #0	; 0x0
    8ac4:	1a000008 	bne	8aec <iic_write_get_address+0x48>
    8ac8:	e1d430b2 	ldrh	r3, [r4, #2]
    8acc:	e3530c01 	cmp	r3, #256	; 0x100
    8ad0:	1a000005 	bne	8aec <iic_write_get_address+0x48>
    8ad4:	e1d430b0 	ldrh	r3, [r4]
    8ad8:	e1c400b2 	strh	r0, [r4, #2]
    8adc:	e2833002 	add	r3, r3, #2	; 0x2
    8ae0:	e1c430b0 	strh	r3, [r4]
    8ae4:	e3a03001 	mov	r3, #1	; 0x1
    8ae8:	e1c431b0 	strh	r3, [r4, #16]
    8aec:	e1d430b2 	ldrh	r3, [r4, #2]
    8af0:	e5940008 	ldr	r0, [r4, #8]
    8af4:	e203300f 	and	r3, r3, #15	; 0xf
    8af8:	e2632010 	rsb	r2, r3, #16	; 0x10
    8afc:	e1520000 	cmp	r2, r0
    8b00:	e5842004 	str	r2, [r4, #4]
    8b04:	2a000004 	bcs	8b1c <iic_write_get_address+0x78>
    8b08:	e0623000 	rsb	r3, r2, r0
    8b0c:	e5843008 	str	r3, [r4, #8]
    8b10:	e584200c 	str	r2, [r4, #12]
    8b14:	e3a00001 	mov	r0, #1	; 0x1
    8b18:	e8bd8010 	pop	{r4, pc}
    8b1c:	e3500000 	cmp	r0, #0	; 0x0
    8b20:	13a03000 	movne	r3, #0	; 0x0
    8b24:	1584000c 	strne	r0, [r4, #12]
    8b28:	15843008 	strne	r3, [r4, #8]
    8b2c:	13a00001 	movne	r0, #1	; 0x1
    8b30:	e8bd8010 	pop	{r4, pc}

00008b34 <iic_word_addr_func>:
    8b34:	e92d4030 	push	{r4, r5, lr}
    8b38:	e24dd004 	sub	sp, sp, #4	; 0x4
    8b3c:	e1a04000 	mov	r4, r0
    8b40:	e1a05001 	mov	r5, r1
    8b44:	ebffff9d 	bl	89c0 <iic_address_num>
    8b48:	e3500000 	cmp	r0, #0	; 0x0
    8b4c:	16ff0074 	uxthne	r0, r4
    8b50:	1a00000c 	bne	8b88 <iic_word_addr_func+0x54>
    8b54:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    8b58:	e0051003 	and	r1, r5, r3
    8b5c:	e3a020ff 	mov	r2, #255	; 0xff
    8b60:	e3510000 	cmp	r1, #0	; 0x0
    8b64:	e0040002 	and	r0, r4, r2
    8b68:	aa000005 	bge	8b84 <iic_word_addr_func+0x50>
    8b6c:	e3e02000 	mvn	r2, #0	; 0x0
    8b70:	e0900002 	adds	r0, r0, r2
    8b74:	e3e020ff 	mvn	r2, #255	; 0xff
    8b78:	e1800002 	orr	r0, r0, r2
    8b7c:	e3a02001 	mov	r2, #1	; 0x1
    8b80:	e0900002 	adds	r0, r0, r2
    8b84:	e6ff0070 	uxth	r0, r0
    8b88:	e28dd004 	add	sp, sp, #4	; 0x4
    8b8c:	e8bd8030 	pop	{r4, r5, pc}

00008b90 <iic_addr_func>:
    8b90:	e92d4030 	push	{r4, r5, lr}
    8b94:	e24dd004 	sub	sp, sp, #4	; 0x4
    8b98:	e1a04000 	mov	r4, r0
    8b9c:	e1a05001 	mov	r5, r1
    8ba0:	ebffff86 	bl	89c0 <iic_address_num>
    8ba4:	e3500000 	cmp	r0, #0	; 0x0
    8ba8:	e59f003c 	ldr	r0, [pc, #60]	; 8bec <iic_addr_func+0x5c>
    8bac:	11d000b0 	ldrhne	r0, [r0]
    8bb0:	1a00000b 	bne	8be4 <iic_addr_func+0x54>
    8bb4:	e1a02005 	mov	r2, r5
    8bb8:	e1a03fc2 	asr	r3, r2, #31
    8bbc:	e1a02fc3 	asr	r2, r3, #31
    8bc0:	e20220ff 	and	r2, r2, #255	; 0xff
    8bc4:	e3a03000 	mov	r3, #0	; 0x0
    8bc8:	e0922004 	adds	r2, r2, r4
    8bcc:	e0a33005 	adc	r3, r3, r5
    8bd0:	e5900000 	ldr	r0, [r0]
    8bd4:	e1a01422 	lsr	r1, r2, #8
    8bd8:	e1811c03 	orr	r1, r1, r3, lsl #24
    8bdc:	e1800081 	orr	r0, r0, r1, lsl #1
    8be0:	e6ff0070 	uxth	r0, r0
    8be4:	e28dd004 	add	sp, sp, #4	; 0x4
    8be8:	e8bd8030 	pop	{r4, r5, pc}
    8bec:	0800d250 	.word	0x0800d250

00008bf0 <e2p_vs_write>:
    8bf0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8bf4:	e1a01003 	mov	r1, r3
    8bf8:	e24dd024 	sub	sp, sp, #36	; 0x24
    8bfc:	e1a06000 	mov	r6, r0
    8c00:	e1a00002 	mov	r0, r2
    8c04:	e1a05003 	mov	r5, r3
    8c08:	e1a04002 	mov	r4, r2
    8c0c:	ebffffdf 	bl	8b90 <iic_addr_func>
    8c10:	e1a01005 	mov	r1, r5
    8c14:	e1cd00bc 	strh	r0, [sp, #12]
    8c18:	e1a00004 	mov	r0, r4
    8c1c:	ebffffc4 	bl	8b34 <iic_word_addr_func>
    8c20:	e3a03000 	mov	r3, #0	; 0x0
    8c24:	e59d2048 	ldr	r2, [sp, #72]
    8c28:	e1a05003 	mov	r5, r3
    8c2c:	e58d3018 	str	r3, [sp, #24]
    8c30:	e28dc00c 	add	ip, sp, #12	; 0xc
    8c34:	e3a03001 	mov	r3, #1	; 0x1
    8c38:	e1cd00be 	strh	r0, [sp, #14]
    8c3c:	e58d2014 	str	r2, [sp, #20]
    8c40:	e1cd31bc 	strh	r3, [sp, #28]
    8c44:	e59fb10c 	ldr	fp, [pc, #268]	; 8d58 <e2p_vs_write+0x168>
    8c48:	e3a0a001 	mov	sl, #1	; 0x1
    8c4c:	e1a08005 	mov	r8, r5
    8c50:	e28c9012 	add	r9, ip, #18	; 0x12
    8c54:	ea000037 	b	8d38 <e2p_vs_write+0x148>
    8c58:	ebffff58 	bl	89c0 <iic_address_num>
    8c5c:	e3500000 	cmp	r0, #0	; 0x0
    8c60:	e1dd10be 	ldrh	r1, [sp, #14]
    8c64:	11a03421 	lsrne	r3, r1, #8
    8c68:	15cd301e 	strbne	r3, [sp, #30]
    8c6c:	15cd101f 	strbne	r1, [sp, #31]
    8c70:	13a07002 	movne	r7, #2	; 0x2
    8c74:	1a000005 	bne	8c90 <e2p_vs_write+0xa0>
    8c78:	e6ef1071 	uxtb	r1, r1
    8c7c:	e5cd101e 	strb	r1, [sp, #30]
    8c80:	e59f00d4 	ldr	r0, [pc, #212]	; 8d5c <e2p_vs_write+0x16c>
    8c84:	e1a0e00f 	mov	lr, pc
    8c88:	e59bf000 	ldr	pc, [fp]
    8c8c:	e3a07001 	mov	r7, #1	; 0x1
    8c90:	ebfffedd 	bl	880c <get_ticks>
    8c94:	e1dd31bc 	ldrh	r3, [sp, #28]
    8c98:	e1a04000 	mov	r4, r0
    8c9c:	e3530000 	cmp	r3, #0	; 0x0
    8ca0:	0a000016 	beq	8d00 <e2p_vs_write+0x110>
    8ca4:	e3a0c000 	mov	ip, #0	; 0x0
    8ca8:	e5dd200c 	ldrb	r2, [sp, #12]
    8cac:	e1a00008 	mov	r0, r8
    8cb0:	e1a0100a 	mov	r1, sl
    8cb4:	e1a03008 	mov	r3, r8
    8cb8:	e1cdc1bc 	strh	ip, [sp, #28]
    8cbc:	e58da000 	str	sl, [sp]
    8cc0:	e58d8004 	str	r8, [sp, #4]
    8cc4:	eb000137 	bl	91a8 <iic_init>
    8cc8:	e3500000 	cmp	r0, #0	; 0x0
    8ccc:	1a00000b 	bne	8d00 <e2p_vs_write+0x110>
    8cd0:	e1a00008 	mov	r0, r8
    8cd4:	ea00001d 	b	8d50 <e2p_vs_write+0x160>
    8cd8:	ebfffecb 	bl	880c <get_ticks>
    8cdc:	e1540000 	cmp	r4, r0
    8ce0:	2a000008 	bcs	8d08 <e2p_vs_write+0x118>
    8ce4:	e59f0074 	ldr	r0, [pc, #116]	; 8d60 <e2p_vs_write+0x170>
    8ce8:	e1a0e00f 	mov	lr, pc
    8cec:	e59bf000 	ldr	pc, [fp]
    8cf0:	e59d3018 	ldr	r3, [sp, #24]
    8cf4:	e0853003 	add	r3, r5, r3
    8cf8:	e6ff5073 	uxth	r5, r3
    8cfc:	ea00000b 	b	8d30 <e2p_vs_write+0x140>
    8d00:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    8d04:	e2844e12 	add	r4, r4, #288	; 0x120
    8d08:	e59dc018 	ldr	ip, [sp, #24]
    8d0c:	e1a01009 	mov	r1, r9
    8d10:	e1a02007 	mov	r2, r7
    8d14:	e1a03006 	mov	r3, r6
    8d18:	e3a00000 	mov	r0, #0	; 0x0
    8d1c:	e58dc000 	str	ip, [sp]
    8d20:	e58da004 	str	sl, [sp, #4]
    8d24:	eb000078 	bl	8f0c <iicreg_write>
    8d28:	e3500000 	cmp	r0, #0	; 0x0
    8d2c:	0affffe9 	beq	8cd8 <e2p_vs_write+0xe8>
    8d30:	e59d3018 	ldr	r3, [sp, #24]
    8d34:	e0866003 	add	r6, r6, r3
    8d38:	e28d000c 	add	r0, sp, #12	; 0xc
    8d3c:	ebffff58 	bl	8aa4 <iic_write_get_address>
    8d40:	e3500000 	cmp	r0, #0	; 0x0
    8d44:	1affffc3 	bne	8c58 <e2p_vs_write+0x68>
    8d48:	e59d2048 	ldr	r2, [sp, #72]
    8d4c:	e0650002 	rsb	r0, r5, r2
    8d50:	e28dd024 	add	sp, sp, #36	; 0x24
    8d54:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    8d58:	0800d76c 	.word	0x0800d76c
    8d5c:	00013376 	.word	0x00013376
    8d60:	00013b58 	.word	0x00013b58

00008d64 <e2p_vs_read>:
    8d64:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8d68:	e1a01003 	mov	r1, r3
    8d6c:	e24dd024 	sub	sp, sp, #36	; 0x24
    8d70:	e1a06000 	mov	r6, r0
    8d74:	e1a00002 	mov	r0, r2
    8d78:	e1a05003 	mov	r5, r3
    8d7c:	e1a04002 	mov	r4, r2
    8d80:	e59d7048 	ldr	r7, [sp, #72]
    8d84:	ebffff81 	bl	8b90 <iic_addr_func>
    8d88:	e1a01005 	mov	r1, r5
    8d8c:	e1cd00bc 	strh	r0, [sp, #12]
    8d90:	e1a00004 	mov	r0, r4
    8d94:	ebffff66 	bl	8b34 <iic_word_addr_func>
    8d98:	e28db00c 	add	fp, sp, #12	; 0xc
    8d9c:	e3a03000 	mov	r3, #0	; 0x0
    8da0:	e3a02001 	mov	r2, #1	; 0x1
    8da4:	e1cd00be 	strh	r0, [sp, #14]
    8da8:	e1cd21bc 	strh	r2, [sp, #28]
    8dac:	e58d7014 	str	r7, [sp, #20]
    8db0:	e58d3018 	str	r3, [sp, #24]
    8db4:	e1a05003 	mov	r5, r3
    8db8:	e3a09001 	mov	r9, #1	; 0x1
    8dbc:	e28ba012 	add	sl, fp, #18	; 0x12
    8dc0:	ea00002b 	b	8e74 <e2p_vs_read+0x110>
    8dc4:	ebfffefd 	bl	89c0 <iic_address_num>
    8dc8:	e1dd20be 	ldrh	r2, [sp, #14]
    8dcc:	e3500000 	cmp	r0, #0	; 0x0
    8dd0:	11a03422 	lsrne	r3, r2, #8
    8dd4:	15cd301e 	strbne	r3, [sp, #30]
    8dd8:	15cd201f 	strbne	r2, [sp, #31]
    8ddc:	13a08002 	movne	r8, #2	; 0x2
    8de0:	05cd201e 	strbeq	r2, [sp, #30]
    8de4:	03a08001 	moveq	r8, #1	; 0x1
    8de8:	ebfffe87 	bl	880c <get_ticks>
    8dec:	e1dd31bc 	ldrh	r3, [sp, #28]
    8df0:	e1a04000 	mov	r4, r0
    8df4:	e3530000 	cmp	r3, #0	; 0x0
    8df8:	0a000010 	beq	8e40 <e2p_vs_read+0xdc>
    8dfc:	e3a0c000 	mov	ip, #0	; 0x0
    8e00:	e5dd200c 	ldrb	r2, [sp, #12]
    8e04:	e1a00005 	mov	r0, r5
    8e08:	e1a01009 	mov	r1, r9
    8e0c:	e1a03005 	mov	r3, r5
    8e10:	e1cdc1bc 	strh	ip, [sp, #28]
    8e14:	e58d9000 	str	r9, [sp]
    8e18:	e58d5004 	str	r5, [sp, #4]
    8e1c:	eb0000e1 	bl	91a8 <iic_init>
    8e20:	e3500000 	cmp	r0, #0	; 0x0
    8e24:	1a000005 	bne	8e40 <e2p_vs_read+0xdc>
    8e28:	e1a07005 	mov	r7, r5
    8e2c:	ea000014 	b	8e84 <e2p_vs_read+0x120>
    8e30:	ebfffe75 	bl	880c <get_ticks>
    8e34:	e1540000 	cmp	r4, r0
    8e38:	2a000002 	bcs	8e48 <e2p_vs_read+0xe4>
    8e3c:	ea00000a 	b	8e6c <e2p_vs_read+0x108>
    8e40:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    8e44:	e2844e12 	add	r4, r4, #288	; 0x120
    8e48:	e59dc018 	ldr	ip, [sp, #24]
    8e4c:	e1a0100a 	mov	r1, sl
    8e50:	e1a02008 	mov	r2, r8
    8e54:	e1a03006 	mov	r3, r6
    8e58:	e3a00000 	mov	r0, #0	; 0x0
    8e5c:	e58dc000 	str	ip, [sp]
    8e60:	eb000061 	bl	8fec <iic_read_func>
    8e64:	e3500000 	cmp	r0, #0	; 0x0
    8e68:	0afffff0 	beq	8e30 <e2p_vs_read+0xcc>
    8e6c:	e59d3018 	ldr	r3, [sp, #24]
    8e70:	e0866003 	add	r6, r6, r3
    8e74:	e1a0000b 	mov	r0, fp
    8e78:	ebfffedb 	bl	89ec <iic_read_get_address>
    8e7c:	e3500000 	cmp	r0, #0	; 0x0
    8e80:	1affffcf 	bne	8dc4 <e2p_vs_read+0x60>
    8e84:	e1a00007 	mov	r0, r7
    8e88:	e28dd024 	add	sp, sp, #36	; 0x24
    8e8c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

00008e90 <iicreg_read>:
    8e90:	e3500001 	cmp	r0, #1	; 0x1
    8e94:	e92d4070 	push	{r4, r5, r6, lr}
    8e98:	83a00000 	movhi	r0, #0	; 0x0
    8e9c:	e1a05001 	mov	r5, r1
    8ea0:	e1a04002 	mov	r4, r2
    8ea4:	88bd8070 	pophi	{r4, r5, r6, pc}
    8ea8:	e59f3058 	ldr	r3, [pc, #88]	; 8f08 <iicreg_read+0x78>
    8eac:	e5932070 	ldr	r2, [r3, #112]
    8eb0:	e2022008 	and	r2, r2, #8	; 0x8
    8eb4:	ea000000 	b	8ebc <iicreg_read+0x2c>
    8eb8:	e5931010 	ldr	r1, [r3, #16]
    8ebc:	e3520000 	cmp	r2, #0	; 0x0
    8ec0:	1afffffc 	bne	8eb8 <iicreg_read+0x28>
    8ec4:	e59f103c 	ldr	r1, [pc, #60]	; 8f08 <iicreg_read+0x78>
    8ec8:	e1a00004 	mov	r0, r4
    8ecc:	e3a06c01 	mov	r6, #256	; 0x100
    8ed0:	e591c070 	ldr	ip, [r1, #112]
    8ed4:	e2503000 	subs	r3, r0, #0	; 0x0
    8ed8:	13a03001 	movne	r3, #1	; 0x1
    8edc:	e01330ac 	ands	r3, r3, ip, lsr #1
    8ee0:	15816010 	strne	r6, [r1, #16]
    8ee4:	12400001 	subne	r0, r0, #1	; 0x1
    8ee8:	e31c0008 	tst	ip, #8	; 0x8
    8eec:	15913010 	ldrne	r3, [r1, #16]
    8ef0:	12822001 	addne	r2, r2, #1	; 0x1
    8ef4:	14c53001 	strbne	r3, [r5], #1
    8ef8:	e1520004 	cmp	r2, r4
    8efc:	1afffff3 	bne	8ed0 <iicreg_read+0x40>
    8f00:	e3a00001 	mov	r0, #1	; 0x1
    8f04:	e8bd8070 	pop	{r4, r5, r6, pc}
    8f08:	20300000 	.word	0x20300000

00008f0c <iicreg_write>:
    8f0c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    8f10:	e59d4018 	ldr	r4, [sp, #24]
    8f14:	e3500001 	cmp	r0, #1	; 0x1
    8f18:	e1a05001 	mov	r5, r1
    8f1c:	e5dd801c 	ldrb	r8, [sp, #28]
    8f20:	e1a01003 	mov	r1, r3
    8f24:	e0847002 	add	r7, r4, r2
    8f28:	8a00002c 	bhi	8fe0 <iicreg_write+0xd4>
    8f2c:	e59f00b4 	ldr	r0, [pc, #180]	; 8fe8 <iicreg_write+0xdc>
    8f30:	e5903034 	ldr	r3, [r0, #52]
    8f34:	e213c040 	ands	ip, r3, #64	; 0x40
    8f38:	15903080 	ldrne	r3, [r0, #128]
    8f3c:	15903054 	ldrne	r3, [r0, #84]
    8f40:	13a00000 	movne	r0, #0	; 0x0
    8f44:	18bd81f0 	popne	{r4, r5, r6, r7, r8, pc}
    8f48:	e5903070 	ldr	r3, [r0, #112]
    8f4c:	e3130004 	tst	r3, #4	; 0x4
    8f50:	11a06000 	movne	r6, r0
    8f54:	11a0000c 	movne	r0, ip
    8f58:	11a0c006 	movne	ip, r6
    8f5c:	1a00000d 	bne	8f98 <iicreg_write+0x8c>
    8f60:	ea00001e 	b	8fe0 <iicreg_write+0xd4>
    8f64:	e5963070 	ldr	r3, [r6, #112]
    8f68:	e3130002 	tst	r3, #2	; 0x2
    8f6c:	0afffffc 	beq	8f64 <iicreg_write+0x58>
    8f70:	e3520000 	cmp	r2, #0	; 0x0
    8f74:	14d53001 	ldrbne	r3, [r5], #1
    8f78:	12422001 	subne	r2, r2, #1	; 0x1
    8f7c:	1a000003 	bne	8f90 <iicreg_write+0x84>
    8f80:	e3540000 	cmp	r4, #0	; 0x0
    8f84:	0a000002 	beq	8f94 <iicreg_write+0x88>
    8f88:	e4d13001 	ldrb	r3, [r1], #1
    8f8c:	e2444001 	sub	r4, r4, #1	; 0x1
    8f90:	e58c3010 	str	r3, [ip, #16]
    8f94:	e2800001 	add	r0, r0, #1	; 0x1
    8f98:	e1500007 	cmp	r0, r7
    8f9c:	3afffff0 	bcc	8f64 <iicreg_write+0x58>
    8fa0:	e59f2040 	ldr	r2, [pc, #64]	; 8fe8 <iicreg_write+0xdc>
    8fa4:	e5923070 	ldr	r3, [r2, #112]
    8fa8:	e3130004 	tst	r3, #4	; 0x4
    8fac:	0afffffc 	beq	8fa4 <iicreg_write+0x98>
    8fb0:	e3580000 	cmp	r8, #0	; 0x0
    8fb4:	159f202c 	ldrne	r2, [pc, #44]	; 8fe8 <iicreg_write+0xdc>
    8fb8:	0a000006 	beq	8fd8 <iicreg_write+0xcc>
    8fbc:	e5923034 	ldr	r3, [r2, #52]
    8fc0:	e3130c02 	tst	r3, #512	; 0x200
    8fc4:	0afffffc 	beq	8fbc <iicreg_write+0xb0>
    8fc8:	e59f3018 	ldr	r3, [pc, #24]	; 8fe8 <iicreg_write+0xdc>
    8fcc:	e3010964 	movw	r0, #6500	; 0x1964
    8fd0:	e5933060 	ldr	r3, [r3, #96]
    8fd4:	ebfffe1d 	bl	8850 <udelay>
    8fd8:	e3a00001 	mov	r0, #1	; 0x1
    8fdc:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    8fe0:	e3a00000 	mov	r0, #0	; 0x0
    8fe4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    8fe8:	20300000 	.word	0x20300000

00008fec <iic_read_func>:
    8fec:	e92d4070 	push	{r4, r5, r6, lr}
    8ff0:	e59f4080 	ldr	r4, [pc, #128]	; 9078 <iic_read_func+0x8c>
    8ff4:	e1a06003 	mov	r6, r3
    8ff8:	e5d43000 	ldrb	r3, [r4]
    8ffc:	e24dd008 	sub	sp, sp, #8	; 0x8
    9000:	e243300a 	sub	r3, r3, #10	; 0xa
    9004:	e6ef3073 	uxtb	r3, r3
    9008:	e3530001 	cmp	r3, #1	; 0x1
    900c:	e1a05000 	mov	r5, r0
    9010:	9a00000b 	bls	9044 <iic_read_func+0x58>
    9014:	e3a0e000 	mov	lr, #0	; 0x0
    9018:	e1a0300e 	mov	r3, lr
    901c:	e3a0c001 	mov	ip, #1	; 0x1
    9020:	e58dc004 	str	ip, [sp, #4]
    9024:	e58de000 	str	lr, [sp]
    9028:	ebffffb7 	bl	8f0c <iicreg_write>
    902c:	e3500000 	cmp	r0, #0	; 0x0
    9030:	03a0300c 	moveq	r3, #12	; 0xc
    9034:	05c43000 	strbeq	r3, [r4]
    9038:	0a00000c 	beq	9070 <iic_read_func+0x84>
    903c:	e3a0300a 	mov	r3, #10	; 0xa
    9040:	e5c43000 	strb	r3, [r4]
    9044:	e59d2018 	ldr	r2, [sp, #24]
    9048:	e1a00005 	mov	r0, r5
    904c:	e1a01006 	mov	r1, r6
    9050:	ebffff8e 	bl	8e90 <iicreg_read>
    9054:	e59f201c 	ldr	r2, [pc, #28]	; 9078 <iic_read_func+0x8c>
    9058:	e3500000 	cmp	r0, #0	; 0x0
    905c:	13a0300d 	movne	r3, #13	; 0xd
    9060:	03a0300b 	moveq	r3, #11	; 0xb
    9064:	15c23000 	strbne	r3, [r2]
    9068:	13a00001 	movne	r0, #1	; 0x1
    906c:	05c23000 	strbeq	r3, [r2]
    9070:	e28dd008 	add	sp, sp, #8	; 0x8
    9074:	e8bd8070 	pop	{r4, r5, r6, pc}
    9078:	0800d254 	.word	0x0800d254

0000907c <iic_writeread>:
    907c:	e92d4070 	push	{r4, r5, r6, lr}
    9080:	e59f4088 	ldr	r4, [pc, #136]	; 9110 <iic_writeread+0x94>
    9084:	e24dd010 	sub	sp, sp, #16	; 0x10
    9088:	e5d43000 	ldrb	r3, [r4]
    908c:	e5cd100f 	strb	r1, [sp, #15]
    9090:	e243300a 	sub	r3, r3, #10	; 0xa
    9094:	e6ef3073 	uxtb	r3, r3
    9098:	e3530001 	cmp	r3, #1	; 0x1
    909c:	e1a06002 	mov	r6, r2
    90a0:	e1a05000 	mov	r5, r0
    90a4:	9a00000c 	bls	90dc <iic_writeread+0x60>
    90a8:	e3a0c000 	mov	ip, #0	; 0x0
    90ac:	e1a0300c 	mov	r3, ip
    90b0:	e28d100f 	add	r1, sp, #15	; 0xf
    90b4:	e3a02001 	mov	r2, #1	; 0x1
    90b8:	e58dc000 	str	ip, [sp]
    90bc:	e58dc004 	str	ip, [sp, #4]
    90c0:	ebffff91 	bl	8f0c <iicreg_write>
    90c4:	e3500000 	cmp	r0, #0	; 0x0
    90c8:	03a0300c 	moveq	r3, #12	; 0xc
    90cc:	05c43000 	strbeq	r3, [r4]
    90d0:	0a00000c 	beq	9108 <iic_writeread+0x8c>
    90d4:	e3a0300a 	mov	r3, #10	; 0xa
    90d8:	e5c43000 	strb	r3, [r4]
    90dc:	e3a02001 	mov	r2, #1	; 0x1
    90e0:	e1a00005 	mov	r0, r5
    90e4:	e1a01006 	mov	r1, r6
    90e8:	ebffff68 	bl	8e90 <iicreg_read>
    90ec:	e59f201c 	ldr	r2, [pc, #28]	; 9110 <iic_writeread+0x94>
    90f0:	e3500000 	cmp	r0, #0	; 0x0
    90f4:	13a0300d 	movne	r3, #13	; 0xd
    90f8:	03a0300b 	moveq	r3, #11	; 0xb
    90fc:	15c23000 	strbne	r3, [r2]
    9100:	13a00001 	movne	r0, #1	; 0x1
    9104:	05c23000 	strbeq	r3, [r2]
    9108:	e28dd010 	add	sp, sp, #16	; 0x10
    910c:	e8bd8070 	pop	{r4, r5, r6, pc}
    9110:	0800d254 	.word	0x0800d254

00009114 <iicreg_init>:
    9114:	e92d4030 	push	{r4, r5, lr}
    9118:	e1a04000 	mov	r4, r0
    911c:	e24dd004 	sub	sp, sp, #4	; 0x4
    9120:	e59f0074 	ldr	r0, [pc, #116]	; 919c <iicreg_init+0x88>
    9124:	e1a05002 	mov	r5, r2
    9128:	ebffdfaa 	bl	fd8 <module_get_clock>
    912c:	e3540001 	cmp	r4, #1	; 0x1
    9130:	83a00000 	movhi	r0, #0	; 0x0
    9134:	8a000016 	bhi	9194 <iicreg_init+0x80>
    9138:	e59f1060 	ldr	r1, [pc, #96]	; 91a0 <iicreg_init+0x8c>
    913c:	eb001bef 	bl	10100 <__aeabi_uidiv>
    9140:	e59f205c 	ldr	r2, [pc, #92]	; 91a4 <iicreg_init+0x90>
    9144:	e1a010a0 	lsr	r1, r0, #1
    9148:	e3a0c000 	mov	ip, #0	; 0x0
    914c:	e3a03043 	mov	r3, #67	; 0x43
    9150:	e582c06c 	str	ip, [r2, #108]
    9154:	e5823000 	str	r3, [r2]
    9158:	e5820014 	str	r0, [r2, #20]
    915c:	e5820018 	str	r0, [r2, #24]
    9160:	e58210a0 	str	r1, [r2, #160]
    9164:	e59230a0 	ldr	r3, [r2, #160]
    9168:	e1a000a5 	lsr	r0, r5, #1
    916c:	e3c33c01 	bic	r3, r3, #256	; 0x100
    9170:	e58230a0 	str	r3, [r2, #160]
    9174:	e5923000 	ldr	r3, [r2]
    9178:	e3a01001 	mov	r1, #1	; 0x1
    917c:	e3833020 	orr	r3, r3, #32	; 0x20
    9180:	e5823000 	str	r3, [r2]
    9184:	e582c030 	str	ip, [r2, #48]
    9188:	e5820004 	str	r0, [r2, #4]
    918c:	e582106c 	str	r1, [r2, #108]
    9190:	e1a00001 	mov	r0, r1
    9194:	e28dd004 	add	sp, sp, #4	; 0x4
    9198:	e8bd8030 	pop	{r4, r5, pc}
    919c:	2d00a360 	.word	0x2d00a360
    91a0:	0009c400 	.word	0x0009c400
    91a4:	20300000 	.word	0x20300000

000091a8 <iic_init>:
    91a8:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    91ac:	e59fc054 	ldr	ip, [pc, #84]	; 9208 <iic_init+0x60>
    91b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    91b4:	e3a0e00d 	mov	lr, #13	; 0xd
    91b8:	e1a0a000 	mov	sl, r0
    91bc:	e59f0048 	ldr	r0, [pc, #72]	; 920c <iic_init+0x64>
    91c0:	e1a04001 	mov	r4, r1
    91c4:	e1a05002 	mov	r5, r2
    91c8:	e1a06003 	mov	r6, r3
    91cc:	e5cce000 	strb	lr, [ip]
    91d0:	e5dd7020 	ldrb	r7, [sp, #32]
    91d4:	e5dd8024 	ldrb	r8, [sp, #36]
    91d8:	ebffdd4e 	bl	718 <module_enable>
    91dc:	e3a0000a 	mov	r0, #10	; 0xa
    91e0:	ebffdd93 	bl	834 <pads_init_module>
    91e4:	e1a0000a 	mov	r0, sl
    91e8:	e1a01004 	mov	r1, r4
    91ec:	e1a02005 	mov	r2, r5
    91f0:	e1a03006 	mov	r3, r6
    91f4:	e58d7020 	str	r7, [sp, #32]
    91f8:	e58d8024 	str	r8, [sp, #36]
    91fc:	e28dd004 	add	sp, sp, #4	; 0x4
    9200:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    9204:	eaffffc2 	b	9114 <iicreg_init>
    9208:	0800d254 	.word	0x0800d254
    920c:	2d010c00 	.word	0x2d010c00

00009210 <spi_set_bytes>:
    9210:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9214:	e59f3020 	ldr	r3, [pc, #32]	; 923c <spi_set_bytes+0x2c>
    9218:	e1a01000 	mov	r1, r0
    921c:	e5830000 	str	r0, [r3]
    9220:	e24dd004 	sub	sp, sp, #4	; 0x4
    9224:	e59f0014 	ldr	r0, [pc, #20]	; 9240 <spi_set_bytes+0x30>
    9228:	e59f3014 	ldr	r3, [pc, #20]	; 9244 <spi_set_bytes+0x34>
    922c:	e1a0e00f 	mov	lr, pc
    9230:	e593f000 	ldr	pc, [r3]
    9234:	e28dd004 	add	sp, sp, #4	; 0x4
    9238:	e8bd8000 	pop	{pc}
    923c:	0800d130 	.word	0x0800d130
    9240:	00013380 	.word	0x00013380
    9244:	0800d76c 	.word	0x0800d76c

00009248 <spi_fifo_clear>:
    9248:	e59f2028 	ldr	r2, [pc, #40]	; 9278 <spi_fifo_clear+0x30>
    924c:	e592300c 	ldr	r3, [r2, #12]
    9250:	e3130010 	tst	r3, #16	; 0x10
    9254:	1afffffc 	bne	924c <spi_fifo_clear+0x4>
    9258:	ea000001 	b	9264 <spi_fifo_clear+0x1c>
    925c:	e5923008 	ldr	r3, [r2, #8]
    9260:	ea000000 	b	9268 <spi_fifo_clear+0x20>
    9264:	e59f200c 	ldr	r2, [pc, #12]	; 9278 <spi_fifo_clear+0x30>
    9268:	e592300c 	ldr	r3, [r2, #12]
    926c:	e3130004 	tst	r3, #4	; 0x4
    9270:	1afffff9 	bne	925c <spi_fifo_clear+0x14>
    9274:	e12fff1e 	bx	lr
    9278:	20500000 	.word	0x20500000

0000927c <flash_vs_align>:
    927c:	e3a00c01 	mov	r0, #256	; 0x100
    9280:	e12fff1e 	bx	lr

00009284 <spi_exchange>:
    9284:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    9288:	e5905008 	ldr	r5, [r0, #8]
    928c:	e3510001 	cmp	r1, #1	; 0x1
    9290:	e24dd00c 	sub	sp, sp, #12	; 0xc
    9294:	e1a0a000 	mov	sl, r0
    9298:	e5908004 	ldr	r8, [r0, #4]
    929c:	d1a04005 	movle	r4, r5
    92a0:	d3a07000 	movle	r7, #0	; 0x0
    92a4:	da000020 	ble	932c <spi_exchange+0xa8>
    92a8:	e280a00c 	add	sl, r0, #12	; 0xc
    92ac:	e1a01008 	mov	r1, r8
    92b0:	e1a0000d 	mov	r0, sp
    92b4:	e1a02005 	mov	r2, r5
    92b8:	ebffe021 	bl	1344 <memcpy>
    92bc:	e99a0012 	ldmib	sl, {r1, r4}
    92c0:	e0843005 	add	r3, r4, r5
    92c4:	e3530008 	cmp	r3, #8	; 0x8
    92c8:	a3a03008 	movge	r3, #8	; 0x8
    92cc:	e6ef6073 	uxtb	r6, r3
    92d0:	e3510000 	cmp	r1, #0	; 0x0
    92d4:	e1a0700d 	mov	r7, sp
    92d8:	059a8000 	ldreq	r8, [sl]
    92dc:	00667084 	rsbeq	r7, r6, r4, lsl #1
    92e0:	0a000008 	beq	9308 <spi_exchange+0x84>
    92e4:	e0654006 	rsb	r4, r5, r6
    92e8:	e0870005 	add	r0, r7, r5
    92ec:	e1a02004 	mov	r2, r4
    92f0:	ebffe013 	bl	1344 <memcpy>
    92f4:	e99a000c 	ldmib	sl, {r2, r3}
    92f8:	e0855003 	add	r5, r5, r3
    92fc:	e0828004 	add	r8, r2, r4
    9300:	e3a07000 	mov	r7, #0	; 0x0
    9304:	e0664005 	rsb	r4, r6, r5
    9308:	e3a02000 	mov	r2, #0	; 0x0
    930c:	e1a0000d 	mov	r0, sp
    9310:	e59f126c 	ldr	r1, [pc, #620]	; 9584 <spi_exchange+0x300>
    9314:	ea000002 	b	9324 <spi_exchange+0xa0>
    9318:	e7d03002 	ldrb	r3, [r0, r2]
    931c:	e2822001 	add	r2, r2, #1	; 0x1
    9320:	e5813008 	str	r3, [r1, #8]
    9324:	e1520006 	cmp	r2, r6
    9328:	bafffffa 	blt	9318 <spi_exchange+0x94>
    932c:	e59f6250 	ldr	r6, [pc, #592]	; 9584 <spi_exchange+0x300>
    9330:	e3a0b005 	mov	fp, #5	; 0x5
    9334:	e5969018 	ldr	r9, [r6, #24]
    9338:	e3190004 	tst	r9, #4	; 0x4
    933c:	0a000030 	beq	9404 <spi_exchange+0x180>
    9340:	e3550003 	cmp	r5, #3	; 0x3
    9344:	da000005 	ble	9360 <spi_exchange+0xdc>
    9348:	e5963008 	ldr	r3, [r6, #8]
    934c:	e2455004 	sub	r5, r5, #4	; 0x4
    9350:	e5963008 	ldr	r3, [r6, #8]
    9354:	e5963008 	ldr	r3, [r6, #8]
    9358:	e5963008 	ldr	r3, [r6, #8]
    935c:	ea000028 	b	9404 <spi_exchange+0x180>
    9360:	e3550000 	cmp	r5, #0	; 0x0
    9364:	0a000003 	beq	9378 <spi_exchange+0xf4>
    9368:	e2555001 	subs	r5, r5, #1	; 0x1
    936c:	e5963008 	ldr	r3, [r6, #8]
    9370:	1afffffc 	bne	9368 <spi_exchange+0xe4>
    9374:	ea000022 	b	9404 <spi_exchange+0x180>
    9378:	e59a3000 	ldr	r3, [sl]
    937c:	e3530000 	cmp	r3, #0	; 0x0
    9380:	1a000004 	bne	9398 <spi_exchange+0x114>
    9384:	e59f01fc 	ldr	r0, [pc, #508]	; 9588 <spi_exchange+0x304>
    9388:	e59f31fc 	ldr	r3, [pc, #508]	; 958c <spi_exchange+0x308>
    938c:	e1a0e00f 	mov	lr, pc
    9390:	e593f000 	ldr	pc, [r3]
    9394:	ea00001a 	b	9404 <spi_exchange+0x180>
    9398:	e3540003 	cmp	r4, #3	; 0x3
    939c:	da00000e 	ble	93dc <spi_exchange+0x158>
    93a0:	e5962008 	ldr	r2, [r6, #8]
    93a4:	e1a03008 	mov	r3, r8
    93a8:	e4c32001 	strb	r2, [r3], #1
    93ac:	e5962008 	ldr	r2, [r6, #8]
    93b0:	e2831001 	add	r1, r3, #1	; 0x1
    93b4:	e5c82001 	strb	r2, [r8, #1]
    93b8:	e5962008 	ldr	r2, [r6, #8]
    93bc:	e2544004 	subs	r4, r4, #4	; 0x4
    93c0:	e5c32001 	strb	r2, [r3, #1]
    93c4:	e5963008 	ldr	r3, [r6, #8]
    93c8:	e2812001 	add	r2, r1, #1	; 0x1
    93cc:	e5c13001 	strb	r3, [r1, #1]
    93d0:	0a00005e 	beq	9550 <spi_exchange+0x2cc>
    93d4:	e2828001 	add	r8, r2, #1	; 0x1
    93d8:	ea000009 	b	9404 <spi_exchange+0x180>
    93dc:	e3540000 	cmp	r4, #0	; 0x0
    93e0:	0a00005a 	beq	9550 <spi_exchange+0x2cc>
    93e4:	e59f2198 	ldr	r2, [pc, #408]	; 9584 <spi_exchange+0x300>
    93e8:	e1a01005 	mov	r1, r5
    93ec:	e5923008 	ldr	r3, [r2, #8]
    93f0:	e7c83001 	strb	r3, [r8, r1]
    93f4:	e2811001 	add	r1, r1, #1	; 0x1
    93f8:	e1510004 	cmp	r1, r4
    93fc:	1afffffa 	bne	93ec <spi_exchange+0x168>
    9400:	ea000052 	b	9550 <spi_exchange+0x2cc>
    9404:	e3190008 	tst	r9, #8	; 0x8
    9408:	0a00002a 	beq	94b8 <spi_exchange+0x234>
    940c:	e59a3004 	ldr	r3, [sl, #4]
    9410:	e3530000 	cmp	r3, #0	; 0x0
    9414:	0a000019 	beq	9480 <spi_exchange+0x1fc>
    9418:	e3540003 	cmp	r4, #3	; 0x3
    941c:	da00000d 	ble	9458 <spi_exchange+0x1d4>
    9420:	e1a03008 	mov	r3, r8
    9424:	e4d32001 	ldrb	r2, [r3], #1
    9428:	e2544004 	subs	r4, r4, #4	; 0x4
    942c:	e5862008 	str	r2, [r6, #8]
    9430:	e5d82001 	ldrb	r2, [r8, #1]
    9434:	e2831001 	add	r1, r3, #1	; 0x1
    9438:	e5862008 	str	r2, [r6, #8]
    943c:	e5d33001 	ldrb	r3, [r3, #1]
    9440:	e2818002 	add	r8, r1, #2	; 0x2
    9444:	e5863008 	str	r3, [r6, #8]
    9448:	e5d13001 	ldrb	r3, [r1, #1]
    944c:	e5863008 	str	r3, [r6, #8]
    9450:	1a000018 	bne	94b8 <spi_exchange+0x234>
    9454:	ea00003d 	b	9550 <spi_exchange+0x2cc>
    9458:	e3540000 	cmp	r4, #0	; 0x0
    945c:	0a00003b 	beq	9550 <spi_exchange+0x2cc>
    9460:	e59f111c 	ldr	r1, [pc, #284]	; 9584 <spi_exchange+0x300>
    9464:	e3a02000 	mov	r2, #0	; 0x0
    9468:	e7d83002 	ldrb	r3, [r8, r2]
    946c:	e2822001 	add	r2, r2, #1	; 0x1
    9470:	e1520004 	cmp	r2, r4
    9474:	e5813008 	str	r3, [r1, #8]
    9478:	1afffffa 	bne	9468 <spi_exchange+0x1e4>
    947c:	ea000033 	b	9550 <spi_exchange+0x2cc>
    9480:	e3570003 	cmp	r7, #3	; 0x3
    9484:	da000005 	ble	94a0 <spi_exchange+0x21c>
    9488:	e586b008 	str	fp, [r6, #8]
    948c:	e2477004 	sub	r7, r7, #4	; 0x4
    9490:	e586b008 	str	fp, [r6, #8]
    9494:	e586b008 	str	fp, [r6, #8]
    9498:	e586b008 	str	fp, [r6, #8]
    949c:	ea000005 	b	94b8 <spi_exchange+0x234>
    94a0:	e3570000 	cmp	r7, #0	; 0x0
    94a4:	0a000003 	beq	94b8 <spi_exchange+0x234>
    94a8:	e3a03005 	mov	r3, #5	; 0x5
    94ac:	e2577001 	subs	r7, r7, #1	; 0x1
    94b0:	e5863008 	str	r3, [r6, #8]
    94b4:	1afffffb 	bne	94a8 <spi_exchange+0x224>
    94b8:	e3190002 	tst	r9, #2	; 0x2
    94bc:	0affff9c 	beq	9334 <spi_exchange+0xb0>
    94c0:	ea000009 	b	94ec <spi_exchange+0x268>
    94c4:	e596300c 	ldr	r3, [r6, #12]
    94c8:	e2455001 	sub	r5, r5, #1	; 0x1
    94cc:	e3130004 	tst	r3, #4	; 0x4
    94d0:	15963008 	ldrne	r3, [r6, #8]
    94d4:	1a000006 	bne	94f4 <spi_exchange+0x270>
    94d8:	e59f00b0 	ldr	r0, [pc, #176]	; 9590 <spi_exchange+0x30c>
    94dc:	e1a01005 	mov	r1, r5
    94e0:	e1a0e00f 	mov	lr, pc
    94e4:	e597f000 	ldr	pc, [r7]
    94e8:	ea000001 	b	94f4 <spi_exchange+0x270>
    94ec:	e59f6090 	ldr	r6, [pc, #144]	; 9584 <spi_exchange+0x300>
    94f0:	e59f7094 	ldr	r7, [pc, #148]	; 958c <spi_exchange+0x308>
    94f4:	e3550000 	cmp	r5, #0	; 0x0
    94f8:	1afffff1 	bne	94c4 <spi_exchange+0x240>
    94fc:	ea00000a 	b	952c <spi_exchange+0x2a8>
    9500:	e595300c 	ldr	r3, [r5, #12]
    9504:	e2444001 	sub	r4, r4, #1	; 0x1
    9508:	e3130004 	tst	r3, #4	; 0x4
    950c:	15953008 	ldrne	r3, [r5, #8]
    9510:	14c83001 	strbne	r3, [r8], #1
    9514:	1a000006 	bne	9534 <spi_exchange+0x2b0>
    9518:	e59f0074 	ldr	r0, [pc, #116]	; 9594 <spi_exchange+0x310>
    951c:	e1a01004 	mov	r1, r4
    9520:	e1a0e00f 	mov	lr, pc
    9524:	e596f000 	ldr	pc, [r6]
    9528:	ea000001 	b	9534 <spi_exchange+0x2b0>
    952c:	e59f5050 	ldr	r5, [pc, #80]	; 9584 <spi_exchange+0x300>
    9530:	e59f6054 	ldr	r6, [pc, #84]	; 958c <spi_exchange+0x308>
    9534:	e59a3000 	ldr	r3, [sl]
    9538:	e3530000 	cmp	r3, #0	; 0x0
    953c:	13540000 	cmpne	r4, #0	; 0x0
    9540:	1affffee 	bne	9500 <spi_exchange+0x27c>
    9544:	e59f3038 	ldr	r3, [pc, #56]	; 9584 <spi_exchange+0x300>
    9548:	e3a02003 	mov	r2, #3	; 0x3
    954c:	e5832020 	str	r2, [r3, #32]
    9550:	e59f202c 	ldr	r2, [pc, #44]	; 9584 <spi_exchange+0x300>
    9554:	e592300c 	ldr	r3, [r2, #12]
    9558:	e3130010 	tst	r3, #16	; 0x10
    955c:	1afffffc 	bne	9554 <spi_exchange+0x2d0>
    9560:	ea000001 	b	956c <spi_exchange+0x2e8>
    9564:	e5923008 	ldr	r3, [r2, #8]
    9568:	ea000000 	b	9570 <spi_exchange+0x2ec>
    956c:	e59f2010 	ldr	r2, [pc, #16]	; 9584 <spi_exchange+0x300>
    9570:	e592300c 	ldr	r3, [r2, #12]
    9574:	e3130004 	tst	r3, #4	; 0x4
    9578:	1afffff9 	bne	9564 <spi_exchange+0x2e0>
    957c:	e28dd00c 	add	sp, sp, #12	; 0xc
    9580:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    9584:	20500000 	.word	0x20500000
    9588:	00013397 	.word	0x00013397
    958c:	0800d76c 	.word	0x0800d76c
    9590:	000133ae 	.word	0x000133ae
    9594:	000133c6 	.word	0x000133c6

00009598 <flash_vs_read>:
    9598:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    959c:	e24dd024 	sub	sp, sp, #36	; 0x24
    95a0:	e28d4018 	add	r4, sp, #24	; 0x18
    95a4:	e2841003 	add	r1, r4, #3	; 0x3
    95a8:	e59da040 	ldr	sl, [sp, #64]
    95ac:	e58d1004 	str	r1, [sp, #4]
    95b0:	e3a01005 	mov	r1, #5	; 0x5
    95b4:	e3a0c000 	mov	ip, #0	; 0x0
    95b8:	e58d1008 	str	r1, [sp, #8]
    95bc:	e2811006 	add	r1, r1, #6	; 0x6
    95c0:	e58dc018 	str	ip, [sp, #24]
    95c4:	e2845004 	add	r5, r4, #4	; 0x4
    95c8:	e58d000c 	str	r0, [sp, #12]
    95cc:	e58dc010 	str	ip, [sp, #16]
    95d0:	e5cd101b 	strb	r1, [sp, #27]
    95d4:	e1a06002 	mov	r6, r2
    95d8:	e1a07003 	mov	r7, r3
    95dc:	e58dc000 	str	ip, [sp]
    95e0:	e1a0400a 	mov	r4, sl
    95e4:	e1a0800d 	mov	r8, sp
    95e8:	ea00000c 	b	9620 <flash_vs_read+0x88>
    95ec:	e3540a01 	cmp	r4, #4096	; 0x1000
    95f0:	b1a03004 	movlt	r3, r4
    95f4:	a3a03a01 	movge	r3, #4096	; 0x1000
    95f8:	e5852000 	str	r2, [r5]
    95fc:	e58d3014 	str	r3, [sp, #20]
    9600:	ebffff1f 	bl	9284 <spi_exchange>
    9604:	e59d2014 	ldr	r2, [sp, #20]
    9608:	e59d300c 	ldr	r3, [sp, #12]
    960c:	e0966002 	adds	r6, r6, r2
    9610:	e0833002 	add	r3, r3, r2
    9614:	e0a77fc2 	adc	r7, r7, r2, asr #31
    9618:	e58d300c 	str	r3, [sp, #12]
    961c:	e0624004 	rsb	r4, r2, r4
    9620:	e1a02c06 	lsl	r2, r6, #24
    9624:	e2061cff 	and	r1, r6, #65280	; 0xff00
    9628:	e1822c26 	orr	r2, r2, r6, lsr #24
    962c:	e1822401 	orr	r2, r2, r1, lsl #8
    9630:	e20638ff 	and	r3, r6, #16711680	; 0xff0000
    9634:	e1822423 	orr	r2, r2, r3, lsr #8
    9638:	e3540000 	cmp	r4, #0	; 0x0
    963c:	e1a02442 	asr	r2, r2, #8
    9640:	e1a0000d 	mov	r0, sp
    9644:	e3a01002 	mov	r1, #2	; 0x2
    9648:	1affffe7 	bne	95ec <flash_vs_read+0x54>
    964c:	e1a0000a 	mov	r0, sl
    9650:	e28dd024 	add	sp, sp, #36	; 0x24
    9654:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00009658 <flash_reset>:
    9658:	e92d4030 	push	{r4, r5, lr}
    965c:	e24dd024 	sub	sp, sp, #36	; 0x24
    9660:	e3a04001 	mov	r4, #1	; 0x1
    9664:	e28dc018 	add	ip, sp, #24	; 0x18
    9668:	e08c3004 	add	r3, ip, r4
    966c:	e3a02000 	mov	r2, #0	; 0x0
    9670:	e58d3010 	str	r3, [sp, #16]
    9674:	e3a03066 	mov	r3, #102	; 0x66
    9678:	e58d2018 	str	r2, [sp, #24]
    967c:	e1a0000d 	mov	r0, sp
    9680:	e1a01004 	mov	r1, r4
    9684:	e5cd3018 	strb	r3, [sp, #24]
    9688:	e3e03066 	mvn	r3, #102	; 0x66
    968c:	e58d200c 	str	r2, [sp, #12]
    9690:	e5cd3019 	strb	r3, [sp, #25]
    9694:	e58d201c 	str	r2, [sp, #28]
    9698:	e88d1004 	stm	sp, {r2, ip}
    969c:	e58d4008 	str	r4, [sp, #8]
    96a0:	e58d4014 	str	r4, [sp, #20]
    96a4:	ebfffef6 	bl	9284 <spi_exchange>
    96a8:	e28d000c 	add	r0, sp, #12	; 0xc
    96ac:	e1a01004 	mov	r1, r4
    96b0:	ebfffef3 	bl	9284 <spi_exchange>
    96b4:	e3a0003c 	mov	r0, #60	; 0x3c
    96b8:	e1a0500d 	mov	r5, sp
    96bc:	ebfffc63 	bl	8850 <udelay>
    96c0:	e28dd024 	add	sp, sp, #36	; 0x24
    96c4:	e8bd8030 	pop	{r4, r5, pc}

000096c8 <flash_poll>:
    96c8:	e92d4030 	push	{r4, r5, lr}
    96cc:	e24dd024 	sub	sp, sp, #36	; 0x24
    96d0:	e28dc018 	add	ip, sp, #24	; 0x18
    96d4:	e28c3001 	add	r3, ip, #1	; 0x1
    96d8:	e3a02000 	mov	r2, #0	; 0x0
    96dc:	e3a01001 	mov	r1, #1	; 0x1
    96e0:	e58d300c 	str	r3, [sp, #12]
    96e4:	e3a03005 	mov	r3, #5	; 0x5
    96e8:	e58d2018 	str	r2, [sp, #24]
    96ec:	e58d2010 	str	r2, [sp, #16]
    96f0:	e58d1014 	str	r1, [sp, #20]
    96f4:	e5cd3018 	strb	r3, [sp, #24]
    96f8:	e1a04000 	mov	r4, r0
    96fc:	e58d201c 	str	r2, [sp, #28]
    9700:	e88d1004 	stm	sp, {r2, ip}
    9704:	e58d1008 	str	r1, [sp, #8]
    9708:	e1a0500d 	mov	r5, sp
    970c:	e1a0000d 	mov	r0, sp
    9710:	e3a01002 	mov	r1, #2	; 0x2
    9714:	ebfffeda 	bl	9284 <spi_exchange>
    9718:	e5dd3019 	ldrb	r3, [sp, #25]
    971c:	e3540000 	cmp	r4, #0	; 0x0
    9720:	11a030a3 	lsrne	r3, r3, #1
    9724:	12233001 	eorne	r3, r3, #1	; 0x1
    9728:	e2033001 	and	r3, r3, #1	; 0x1
    972c:	e3530000 	cmp	r3, #0	; 0x0
    9730:	1afffff5 	bne	970c <flash_poll+0x44>
    9734:	e28dd024 	add	sp, sp, #36	; 0x24
    9738:	e8bd8030 	pop	{r4, r5, pc}

0000973c <flash_wren>:
    973c:	e92d4010 	push	{r4, lr}
    9740:	e24dd020 	sub	sp, sp, #32	; 0x20
    9744:	e3a04001 	mov	r4, #1	; 0x1
    9748:	e28dc018 	add	ip, sp, #24	; 0x18
    974c:	e08c3004 	add	r3, ip, r4
    9750:	e3a02000 	mov	r2, #0	; 0x0
    9754:	e58d3010 	str	r3, [sp, #16]
    9758:	e3a03006 	mov	r3, #6	; 0x6
    975c:	e58d2018 	str	r2, [sp, #24]
    9760:	e1a0000d 	mov	r0, sp
    9764:	e5cd3018 	strb	r3, [sp, #24]
    9768:	e1a01004 	mov	r1, r4
    976c:	e3a03004 	mov	r3, #4	; 0x4
    9770:	e58d200c 	str	r2, [sp, #12]
    9774:	e5cd3019 	strb	r3, [sp, #25]
    9778:	e58d201c 	str	r2, [sp, #28]
    977c:	e88d1004 	stm	sp, {r2, ip}
    9780:	e58d4008 	str	r4, [sp, #8]
    9784:	e58d4014 	str	r4, [sp, #20]
    9788:	ebfffebd 	bl	9284 <spi_exchange>
    978c:	e1a00004 	mov	r0, r4
    9790:	ebffffcc 	bl	96c8 <flash_poll>
    9794:	e28dd020 	add	sp, sp, #32	; 0x20
    9798:	e8bd8010 	pop	{r4, pc}

0000979c <flash_vs_erase>:
    979c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    97a0:	e24dd034 	sub	sp, sp, #52	; 0x34
    97a4:	e28d4028 	add	r4, sp, #40	; 0x28
    97a8:	e3a06001 	mov	r6, #1	; 0x1
    97ac:	e59f109c 	ldr	r1, [pc, #156]	; 9850 <flash_vs_erase+0xb4>
    97b0:	e3a02008 	mov	r2, #8	; 0x8
    97b4:	e1a00004 	mov	r0, r4
    97b8:	ebffdee1 	bl	1344 <memcpy>
    97bc:	e0843006 	add	r3, r4, r6
    97c0:	e2842003 	add	r2, r4, #3	; 0x3
    97c4:	e3a05000 	mov	r5, #0	; 0x0
    97c8:	e28d7004 	add	r7, sp, #4	; 0x4
    97cc:	e58d3008 	str	r3, [sp, #8]
    97d0:	e3a03002 	mov	r3, #2	; 0x2
    97d4:	e58d300c 	str	r3, [sp, #12]
    97d8:	e58d2014 	str	r2, [sp, #20]
    97dc:	e58d4020 	str	r4, [sp, #32]
    97e0:	e58d5004 	str	r5, [sp, #4]
    97e4:	e58d5010 	str	r5, [sp, #16]
    97e8:	e58d6018 	str	r6, [sp, #24]
    97ec:	e58d501c 	str	r5, [sp, #28]
    97f0:	e58d6024 	str	r6, [sp, #36]
    97f4:	ebffffd0 	bl	973c <flash_wren>
    97f8:	e1a01006 	mov	r1, r6
    97fc:	e1a00007 	mov	r0, r7
    9800:	ebfffe9f 	bl	9284 <spi_exchange>
    9804:	e1a00005 	mov	r0, r5
    9808:	ebffffae 	bl	96c8 <flash_poll>
    980c:	e2870018 	add	r0, r7, #24	; 0x18
    9810:	e1a01006 	mov	r1, r6
    9814:	ebfffe9a 	bl	9284 <spi_exchange>
    9818:	e1a01006 	mov	r1, r6
    981c:	e1a00007 	mov	r0, r7
    9820:	ebfffe97 	bl	9284 <spi_exchange>
    9824:	e1a00005 	mov	r0, r5
    9828:	ebffffa6 	bl	96c8 <flash_poll>
    982c:	ebffffc2 	bl	973c <flash_wren>
    9830:	e1a01006 	mov	r1, r6
    9834:	e287000c 	add	r0, r7, #12	; 0xc
    9838:	ebfffe91 	bl	9284 <spi_exchange>
    983c:	e1a00005 	mov	r0, r5
    9840:	ebffffa0 	bl	96c8 <flash_poll>
    9844:	e1a00005 	mov	r0, r5
    9848:	e28dd034 	add	sp, sp, #52	; 0x34
    984c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    9850:	0001181e 	.word	0x0001181e

00009854 <flash_vs_write>:
    9854:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    9858:	e3a01004 	mov	r1, #4	; 0x4
    985c:	e24dd020 	sub	sp, sp, #32	; 0x20
    9860:	e3a0c000 	mov	ip, #0	; 0x0
    9864:	e59d9040 	ldr	r9, [sp, #64]
    9868:	e58d1008 	str	r1, [sp, #8]
    986c:	e2811002 	add	r1, r1, #2	; 0x2
    9870:	e58dc018 	str	ip, [sp, #24]
    9874:	e28d701c 	add	r7, sp, #28	; 0x1c
    9878:	e5cd1018 	strb	r1, [sp, #24]
    987c:	e3a01002 	mov	r1, #2	; 0x2
    9880:	e58dc00c 	str	ip, [sp, #12]
    9884:	e58d0010 	str	r0, [sp, #16]
    9888:	e5cd101b 	strb	r1, [sp, #27]
    988c:	e1a04002 	mov	r4, r2
    9890:	e1a05003 	mov	r5, r3
    9894:	e58dc000 	str	ip, [sp]
    9898:	e58d7004 	str	r7, [sp, #4]
    989c:	e1a06009 	mov	r6, r9
    98a0:	e59fa080 	ldr	sl, [pc, #128]	; 9928 <flash_vs_write+0xd4>
    98a4:	e1a0800d 	mov	r8, sp
    98a8:	ea000011 	b	98f4 <flash_vs_write+0xa0>
    98ac:	e59a3000 	ldr	r3, [sl]
    98b0:	e5872000 	str	r2, [r7]
    98b4:	e1560003 	cmp	r6, r3
    98b8:	d58d6014 	strle	r6, [sp, #20]
    98bc:	c58d3014 	strgt	r3, [sp, #20]
    98c0:	ebffff9d 	bl	973c <flash_wren>
    98c4:	e1a0000d 	mov	r0, sp
    98c8:	e3a01002 	mov	r1, #2	; 0x2
    98cc:	ebfffe6c 	bl	9284 <spi_exchange>
    98d0:	e3a00000 	mov	r0, #0	; 0x0
    98d4:	ebffff7b 	bl	96c8 <flash_poll>
    98d8:	e59d2014 	ldr	r2, [sp, #20]
    98dc:	e59d3010 	ldr	r3, [sp, #16]
    98e0:	e0944002 	adds	r4, r4, r2
    98e4:	e0833002 	add	r3, r3, r2
    98e8:	e0a55fc2 	adc	r5, r5, r2, asr #31
    98ec:	e58d3010 	str	r3, [sp, #16]
    98f0:	e0626006 	rsb	r6, r2, r6
    98f4:	e1a02c04 	lsl	r2, r4, #24
    98f8:	e1822c24 	orr	r2, r2, r4, lsr #24
    98fc:	e2041cff 	and	r1, r4, #65280	; 0xff00
    9900:	e1822401 	orr	r2, r2, r1, lsl #8
    9904:	e20438ff 	and	r3, r4, #16711680	; 0xff0000
    9908:	e1822423 	orr	r2, r2, r3, lsr #8
    990c:	e3c220ff 	bic	r2, r2, #255	; 0xff
    9910:	e3560000 	cmp	r6, #0	; 0x0
    9914:	e3822002 	orr	r2, r2, #2	; 0x2
    9918:	1affffe3 	bne	98ac <flash_vs_write+0x58>
    991c:	e1a00009 	mov	r0, r9
    9920:	e28dd020 	add	sp, sp, #32	; 0x20
    9924:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    9928:	0800d130 	.word	0x0800d130

0000992c <spi_init_pads>:
    992c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9930:	e3a0000b 	mov	r0, #11	; 0xb
    9934:	e24dd004 	sub	sp, sp, #4	; 0x4
    9938:	ebffdbbd 	bl	834 <pads_init_module>
    993c:	e3a00000 	mov	r0, #0	; 0x0
    9940:	e28dd004 	add	sp, sp, #4	; 0x4
    9944:	e8bd8000 	pop	{pc}

00009948 <spi_init_clk>:
    9948:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    994c:	e59f00f8 	ldr	r0, [pc, #248]	; 9a4c <spi_init_clk+0x104>
    9950:	e24dd00c 	sub	sp, sp, #12	; 0xc
    9954:	ebffdd9f 	bl	fd8 <module_get_clock>
    9958:	e59f20f0 	ldr	r2, [pc, #240]	; 9a50 <spi_init_clk+0x108>
    995c:	e1a08000 	mov	r8, r0
    9960:	e5923404 	ldr	r3, [r2, #1028]
    9964:	e3a05002 	mov	r5, #2	; 0x2
    9968:	e3833008 	orr	r3, r3, #8	; 0x8
    996c:	e59fa0e0 	ldr	sl, [pc, #224]	; 9a54 <spi_init_clk+0x10c>
    9970:	e5823404 	str	r3, [r2, #1028]
    9974:	ea00000c 	b	99ac <spi_init_clk+0x64>
    9978:	e1a06004 	mov	r6, r4
    997c:	e1a01007 	mov	r1, r7
    9980:	e1a00008 	mov	r0, r8
    9984:	eb001a5f 	bl	10308 <__aeabi_idiv>
    9988:	e150000a 	cmp	r0, sl
    998c:	e0877005 	add	r7, r7, r5
    9990:	e2864001 	add	r4, r6, #1	; 0x1
    9994:	da000007 	ble	99b8 <spi_init_clk+0x70>
    9998:	e3540c01 	cmp	r4, #256	; 0x100
    999c:	1afffff5 	bne	9978 <spi_init_clk+0x30>
    99a0:	e2855002 	add	r5, r5, #2	; 0x2
    99a4:	e3550c01 	cmp	r5, #256	; 0x100
    99a8:	0a000003 	beq	99bc <spi_init_clk+0x74>
    99ac:	e1a07005 	mov	r7, r5
    99b0:	e3a06000 	mov	r6, #0	; 0x0
    99b4:	eafffff0 	b	997c <spi_init_clk+0x34>
    99b8:	e1a04006 	mov	r4, r6
    99bc:	e35400ff 	cmp	r4, #255	; 0xff
    99c0:	d35500fe 	cmple	r5, #254	; 0xfe
    99c4:	da000007 	ble	99e8 <spi_init_clk+0xa0>
    99c8:	e1a01005 	mov	r1, r5
    99cc:	e1a02004 	mov	r2, r4
    99d0:	e59f0080 	ldr	r0, [pc, #128]	; 9a58 <spi_init_clk+0x110>
    99d4:	e59f3080 	ldr	r3, [pc, #128]	; 9a5c <spi_init_clk+0x114>
    99d8:	e1a0e00f 	mov	lr, pc
    99dc:	e593f000 	ldr	pc, [r3]
    99e0:	e3a05002 	mov	r5, #2	; 0x2
    99e4:	e3a04027 	mov	r4, #39	; 0x27
    99e8:	e2841001 	add	r1, r4, #1	; 0x1
    99ec:	e0010195 	mul	r1, r5, r1
    99f0:	e1a00008 	mov	r0, r8
    99f4:	eb001a43 	bl	10308 <__aeabi_idiv>
    99f8:	e1a01008 	mov	r1, r8
    99fc:	e58d0000 	str	r0, [sp]
    9a00:	e1a02005 	mov	r2, r5
    9a04:	e1a03004 	mov	r3, r4
    9a08:	e59f0050 	ldr	r0, [pc, #80]	; 9a60 <spi_init_clk+0x118>
    9a0c:	e59fc048 	ldr	ip, [pc, #72]	; 9a5c <spi_init_clk+0x114>
    9a10:	e1a0e00f 	mov	lr, pc
    9a14:	e59cf000 	ldr	pc, [ip]
    9a18:	e59f2044 	ldr	r2, [pc, #68]	; 9a64 <spi_init_clk+0x11c>
    9a1c:	e3a03000 	mov	r3, #0	; 0x0
    9a20:	e1a01404 	lsl	r1, r4, #8
    9a24:	e5823004 	str	r3, [r2, #4]
    9a28:	e38110c7 	orr	r1, r1, #199	; 0xc7
    9a2c:	e5823014 	str	r3, [r2, #20]
    9a30:	e1a00003 	mov	r0, r3
    9a34:	e283300a 	add	r3, r3, #10	; 0xa
    9a38:	e5825010 	str	r5, [r2, #16]
    9a3c:	e5821000 	str	r1, [r2]
    9a40:	e5823004 	str	r3, [r2, #4]
    9a44:	e28dd00c 	add	sp, sp, #12	; 0xc
    9a48:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    9a4c:	2d00a2a0 	.word	0x2d00a2a0
    9a50:	2d011000 	.word	0x2d011000
    9a54:	00061a80 	.word	0x00061a80
    9a58:	000133de 	.word	0x000133de
    9a5c:	0800d76c 	.word	0x0800d76c
    9a60:	0001340c 	.word	0x0001340c
    9a64:	20500000 	.word	0x20500000

00009a68 <spi_sequence_brd_reboot>:
    9a68:	e92d4010 	push	{r4, lr}
    9a6c:	ebffdb49 	bl	798 <get_reset_stats>
    9a70:	e3a01002 	mov	r1, #2	; 0x2
    9a74:	e1a04000 	mov	r4, r0
    9a78:	e3a02001 	mov	r2, #1	; 0x1
    9a7c:	e3a0004a 	mov	r0, #74	; 0x4a
    9a80:	ebffdc3d 	bl	b7c <pads_chmod>
    9a84:	e3540000 	cmp	r4, #0	; 0x0
    9a88:	08bd8010 	popeq	{r4, pc}
    9a8c:	e3a01002 	mov	r1, #2	; 0x2
    9a90:	e3a02000 	mov	r2, #0	; 0x0
    9a94:	e3a00077 	mov	r0, #119	; 0x77
    9a98:	ebffdc37 	bl	b7c <pads_chmod>
    9a9c:	e3020710 	movw	r0, #10000	; 0x2710
    9aa0:	ebfffb6a 	bl	8850 <udelay>
    9aa4:	e3a00077 	mov	r0, #119	; 0x77
    9aa8:	e3a01001 	mov	r1, #1	; 0x1
    9aac:	e3a02000 	mov	r2, #0	; 0x0
    9ab0:	ebffdc31 	bl	b7c <pads_chmod>
    9ab4:	e3020710 	movw	r0, #10000	; 0x2710
    9ab8:	e8bd4010 	pop	{r4, lr}
    9abc:	eafffb63 	b	8850 <udelay>

00009ac0 <flash_vs_reset>:
    9ac0:	e92d4010 	push	{r4, lr}
    9ac4:	ebffffe7 	bl	9a68 <spi_sequence_brd_reboot>
    9ac8:	e59f0044 	ldr	r0, [pc, #68]	; 9b14 <flash_vs_reset+0x54>
    9acc:	ebffdb11 	bl	718 <module_enable>
    9ad0:	ebffff9c 	bl	9948 <spi_init_clk>
    9ad4:	e3a0000b 	mov	r0, #11	; 0xb
    9ad8:	ebffdb55 	bl	834 <pads_init_module>
    9adc:	e59f2034 	ldr	r2, [pc, #52]	; 9b18 <flash_vs_reset+0x58>
    9ae0:	e592300c 	ldr	r3, [r2, #12]
    9ae4:	e3130010 	tst	r3, #16	; 0x10
    9ae8:	1afffffc 	bne	9ae0 <flash_vs_reset+0x20>
    9aec:	ea000001 	b	9af8 <flash_vs_reset+0x38>
    9af0:	e5923008 	ldr	r3, [r2, #8]
    9af4:	ea000000 	b	9afc <flash_vs_reset+0x3c>
    9af8:	e59f2018 	ldr	r2, [pc, #24]	; 9b18 <flash_vs_reset+0x58>
    9afc:	e592300c 	ldr	r3, [r2, #12]
    9b00:	e2134004 	ands	r4, r3, #4	; 0x4
    9b04:	1afffff9 	bne	9af0 <flash_vs_reset+0x30>
    9b08:	ebfffed2 	bl	9658 <flash_reset>
    9b0c:	e1a00004 	mov	r0, r4
    9b10:	e8bd8010 	pop	{r4, pc}
    9b14:	2d011400 	.word	0x2d011400
    9b18:	20500000 	.word	0x20500000

00009b1c <ssp_log_open>:
    9b1c:	e59f3008 	ldr	r3, [pc, #8]	; 9b2c <ssp_log_open+0x10>
    9b20:	e3a02001 	mov	r2, #1	; 0x1
    9b24:	e5832000 	str	r2, [r3]
    9b28:	e12fff1e 	bx	lr
    9b2c:	0800d2b8 	.word	0x0800d2b8

00009b30 <ssp_log_close>:
    9b30:	e59f3008 	ldr	r3, [pc, #8]	; 9b40 <ssp_log_close+0x10>
    9b34:	e3a02000 	mov	r2, #0	; 0x0
    9b38:	e5832000 	str	r2, [r3]
    9b3c:	e12fff1e 	bx	lr
    9b40:	0800d2b8 	.word	0x0800d2b8

00009b44 <ssp_manager_set_wire>:
    9b44:	e59f3004 	ldr	r3, [pc, #4]	; 9b50 <ssp_manager_set_wire+0xc>
    9b48:	e5830000 	str	r0, [r3]
    9b4c:	e12fff1e 	bx	lr
    9b50:	0800d258 	.word	0x0800d258

00009b54 <ssp_manager_set_rwmode>:
    9b54:	e2403064 	sub	r3, r0, #100	; 0x64
    9b58:	e59f2020 	ldr	r2, [pc, #32]	; 9b80 <ssp_manager_set_rwmode+0x2c>
    9b5c:	e3530002 	cmp	r3, #2	; 0x2
    9b60:	83a03064 	movhi	r3, #100	; 0x64
    9b64:	e5820050 	str	r0, [r2, #80]
    9b68:	85823050 	strhi	r3, [r2, #80]
    9b6c:	e3500066 	cmp	r0, #102	; 0x66
    9b70:	059f3008 	ldreq	r3, [pc, #8]	; 9b80 <ssp_manager_set_rwmode+0x2c>
    9b74:	03a02064 	moveq	r2, #100	; 0x64
    9b78:	05832050 	streq	r2, [r3, #80]
    9b7c:	e12fff1e 	bx	lr
    9b80:	0800d258 	.word	0x0800d258

00009b84 <ssp_manager_init>:
    9b84:	e59f2058 	ldr	r2, [pc, #88]	; 9be4 <ssp_manager_init+0x60>
    9b88:	e30033ff 	movw	r3, #1023	; 0x3ff
    9b8c:	e5823038 	str	r3, [r2, #56]
    9b90:	e2433e3f 	sub	r3, r3, #1008	; 0x3f0
    9b94:	e5823000 	str	r3, [r2]
    9b98:	e2833011 	add	r3, r3, #17	; 0x11
    9b9c:	e5823058 	str	r3, [r2, #88]
    9ba0:	e2833044 	add	r3, r3, #68	; 0x44
    9ba4:	e5823050 	str	r3, [r2, #80]
    9ba8:	e3a03040 	mov	r3, #64	; 0x40
    9bac:	e3a01000 	mov	r1, #0	; 0x0
    9bb0:	e3a00001 	mov	r0, #1	; 0x1
    9bb4:	e3a0cb02 	mov	ip, #2048	; 0x800
    9bb8:	e5823030 	str	r3, [r2, #48]
    9bbc:	e0833003 	add	r3, r3, r3
    9bc0:	e582000c 	str	r0, [r2, #12]
    9bc4:	e582c028 	str	ip, [r2, #40]
    9bc8:	e582302c 	str	r3, [r2, #44]
    9bcc:	e5821034 	str	r1, [r2, #52]
    9bd0:	e5821004 	str	r1, [r2, #4]
    9bd4:	e5821008 	str	r1, [r2, #8]
    9bd8:	e582005c 	str	r0, [r2, #92]
    9bdc:	e582c044 	str	ip, [r2, #68]
    9be0:	e12fff1e 	bx	lr
    9be4:	0800d258 	.word	0x0800d258

00009be8 <spi_status_is_busy>:
    9be8:	e59f300c 	ldr	r3, [pc, #12]	; 9bfc <spi_status_is_busy+0x14>
    9bec:	e593000c 	ldr	r0, [r3, #12]
    9bf0:	e1a00220 	lsr	r0, r0, #4
    9bf4:	e2000001 	and	r0, r0, #1	; 0x1
    9bf8:	e12fff1e 	bx	lr
    9bfc:	20500000 	.word	0x20500000

00009c00 <spi_status_recv_fifo_not_empty>:
    9c00:	e59f300c 	ldr	r3, [pc, #12]	; 9c14 <spi_status_recv_fifo_not_empty+0x14>
    9c04:	e593000c 	ldr	r0, [r3, #12]
    9c08:	e1a00120 	lsr	r0, r0, #2
    9c0c:	e2000001 	and	r0, r0, #1	; 0x1
    9c10:	e12fff1e 	bx	lr
    9c14:	20500000 	.word	0x20500000

00009c18 <spi_status_trans_fifo_empty>:
    9c18:	e59f3008 	ldr	r3, [pc, #8]	; 9c28 <spi_status_trans_fifo_empty+0x10>
    9c1c:	e593000c 	ldr	r0, [r3, #12]
    9c20:	e2000001 	and	r0, r0, #1	; 0x1
    9c24:	e12fff1e 	bx	lr
    9c28:	20500000 	.word	0x20500000

00009c2c <spi_set_intr_recv_overrun>:
    9c2c:	e59f30b4 	ldr	r3, [pc, #180]	; 9ce8 <spi_set_intr_recv_overrun+0xbc>
    9c30:	e92d4030 	push	{r4, r5, lr}
    9c34:	e5932000 	ldr	r2, [r3]
    9c38:	e59f30ac 	ldr	r3, [pc, #172]	; 9cec <spi_set_intr_recv_overrun+0xc0>
    9c3c:	e3520000 	cmp	r2, #0	; 0x0
    9c40:	e5933014 	ldr	r3, [r3, #20]
    9c44:	e24dd004 	sub	sp, sp, #4	; 0x4
    9c48:	e1a05000 	mov	r5, r0
    9c4c:	e3c34001 	bic	r4, r3, #1	; 0x1
    9c50:	0a000003 	beq	9c64 <spi_set_intr_recv_overrun+0x38>
    9c54:	e59f0094 	ldr	r0, [pc, #148]	; 9cf0 <spi_set_intr_recv_overrun+0xc4>
    9c58:	e59f3094 	ldr	r3, [pc, #148]	; 9cf4 <spi_set_intr_recv_overrun+0xc8>
    9c5c:	e1a0e00f 	mov	lr, pc
    9c60:	e593f000 	ldr	pc, [r3]
    9c64:	e3550000 	cmp	r5, #0	; 0x0
    9c68:	0a000002 	beq	9c78 <spi_set_intr_recv_overrun+0x4c>
    9c6c:	e3550001 	cmp	r5, #1	; 0x1
    9c70:	1a000011 	bne	9cbc <spi_set_intr_recv_overrun+0x90>
    9c74:	ea000005 	b	9c90 <spi_set_intr_recv_overrun+0x64>
    9c78:	e59f3068 	ldr	r3, [pc, #104]	; 9ce8 <spi_set_intr_recv_overrun+0xbc>
    9c7c:	e5933000 	ldr	r3, [r3]
    9c80:	e3530000 	cmp	r3, #0	; 0x0
    9c84:	159f006c 	ldrne	r0, [pc, #108]	; 9cf8 <spi_set_intr_recv_overrun+0xcc>
    9c88:	0a000011 	beq	9cd4 <spi_set_intr_recv_overrun+0xa8>
    9c8c:	ea000005 	b	9ca8 <spi_set_intr_recv_overrun+0x7c>
    9c90:	e59f3050 	ldr	r3, [pc, #80]	; 9ce8 <spi_set_intr_recv_overrun+0xbc>
    9c94:	e3844001 	orr	r4, r4, #1	; 0x1
    9c98:	e5933000 	ldr	r3, [r3]
    9c9c:	e3530000 	cmp	r3, #0	; 0x0
    9ca0:	0a00000b 	beq	9cd4 <spi_set_intr_recv_overrun+0xa8>
    9ca4:	e59f0050 	ldr	r0, [pc, #80]	; 9cfc <spi_set_intr_recv_overrun+0xd0>
    9ca8:	e1a01004 	mov	r1, r4
    9cac:	e59f3040 	ldr	r3, [pc, #64]	; 9cf4 <spi_set_intr_recv_overrun+0xc8>
    9cb0:	e1a0e00f 	mov	lr, pc
    9cb4:	e593f000 	ldr	pc, [r3]
    9cb8:	ea000005 	b	9cd4 <spi_set_intr_recv_overrun+0xa8>
    9cbc:	e59f003c 	ldr	r0, [pc, #60]	; 9d00 <spi_set_intr_recv_overrun+0xd4>
    9cc0:	e59f302c 	ldr	r3, [pc, #44]	; 9cf4 <spi_set_intr_recv_overrun+0xc8>
    9cc4:	e1a0e00f 	mov	lr, pc
    9cc8:	e593f000 	ldr	pc, [r3]
    9ccc:	e3e00000 	mvn	r0, #0	; 0x0
    9cd0:	ea000002 	b	9ce0 <spi_set_intr_recv_overrun+0xb4>
    9cd4:	e59f3010 	ldr	r3, [pc, #16]	; 9cec <spi_set_intr_recv_overrun+0xc0>
    9cd8:	e3a00000 	mov	r0, #0	; 0x0
    9cdc:	e5834014 	str	r4, [r3, #20]
    9ce0:	e28dd004 	add	sp, sp, #4	; 0x4
    9ce4:	e8bd8030 	pop	{r4, r5, pc}
    9ce8:	0800d2b8 	.word	0x0800d2b8
    9cec:	20500000 	.word	0x20500000
    9cf0:	00013431 	.word	0x00013431
    9cf4:	0800d76c 	.word	0x0800d76c
    9cf8:	00013470 	.word	0x00013470
    9cfc:	000134ad 	.word	0x000134ad
    9d00:	000134ec 	.word	0x000134ec

00009d04 <spi_set_intr_recv_timeout>:
    9d04:	e59f30b4 	ldr	r3, [pc, #180]	; 9dc0 <spi_set_intr_recv_timeout+0xbc>
    9d08:	e92d4030 	push	{r4, r5, lr}
    9d0c:	e5932000 	ldr	r2, [r3]
    9d10:	e59f30ac 	ldr	r3, [pc, #172]	; 9dc4 <spi_set_intr_recv_timeout+0xc0>
    9d14:	e3520000 	cmp	r2, #0	; 0x0
    9d18:	e5933014 	ldr	r3, [r3, #20]
    9d1c:	e24dd004 	sub	sp, sp, #4	; 0x4
    9d20:	e1a05000 	mov	r5, r0
    9d24:	e3c34002 	bic	r4, r3, #2	; 0x2
    9d28:	0a000003 	beq	9d3c <spi_set_intr_recv_timeout+0x38>
    9d2c:	e59f0094 	ldr	r0, [pc, #148]	; 9dc8 <spi_set_intr_recv_timeout+0xc4>
    9d30:	e59f3094 	ldr	r3, [pc, #148]	; 9dcc <spi_set_intr_recv_timeout+0xc8>
    9d34:	e1a0e00f 	mov	lr, pc
    9d38:	e593f000 	ldr	pc, [r3]
    9d3c:	e3550000 	cmp	r5, #0	; 0x0
    9d40:	0a000002 	beq	9d50 <spi_set_intr_recv_timeout+0x4c>
    9d44:	e3550002 	cmp	r5, #2	; 0x2
    9d48:	1a000011 	bne	9d94 <spi_set_intr_recv_timeout+0x90>
    9d4c:	ea000005 	b	9d68 <spi_set_intr_recv_timeout+0x64>
    9d50:	e59f3068 	ldr	r3, [pc, #104]	; 9dc0 <spi_set_intr_recv_timeout+0xbc>
    9d54:	e5933000 	ldr	r3, [r3]
    9d58:	e3530000 	cmp	r3, #0	; 0x0
    9d5c:	159f006c 	ldrne	r0, [pc, #108]	; 9dd0 <spi_set_intr_recv_timeout+0xcc>
    9d60:	0a000011 	beq	9dac <spi_set_intr_recv_timeout+0xa8>
    9d64:	ea000005 	b	9d80 <spi_set_intr_recv_timeout+0x7c>
    9d68:	e59f3050 	ldr	r3, [pc, #80]	; 9dc0 <spi_set_intr_recv_timeout+0xbc>
    9d6c:	e3844002 	orr	r4, r4, #2	; 0x2
    9d70:	e5933000 	ldr	r3, [r3]
    9d74:	e3530000 	cmp	r3, #0	; 0x0
    9d78:	0a00000b 	beq	9dac <spi_set_intr_recv_timeout+0xa8>
    9d7c:	e59f0050 	ldr	r0, [pc, #80]	; 9dd4 <spi_set_intr_recv_timeout+0xd0>
    9d80:	e1a01004 	mov	r1, r4
    9d84:	e59f3040 	ldr	r3, [pc, #64]	; 9dcc <spi_set_intr_recv_timeout+0xc8>
    9d88:	e1a0e00f 	mov	lr, pc
    9d8c:	e593f000 	ldr	pc, [r3]
    9d90:	ea000005 	b	9dac <spi_set_intr_recv_timeout+0xa8>
    9d94:	e59f003c 	ldr	r0, [pc, #60]	; 9dd8 <spi_set_intr_recv_timeout+0xd4>
    9d98:	e59f302c 	ldr	r3, [pc, #44]	; 9dcc <spi_set_intr_recv_timeout+0xc8>
    9d9c:	e1a0e00f 	mov	lr, pc
    9da0:	e593f000 	ldr	pc, [r3]
    9da4:	e3e00000 	mvn	r0, #0	; 0x0
    9da8:	ea000002 	b	9db8 <spi_set_intr_recv_timeout+0xb4>
    9dac:	e59f3010 	ldr	r3, [pc, #16]	; 9dc4 <spi_set_intr_recv_timeout+0xc0>
    9db0:	e3a00000 	mov	r0, #0	; 0x0
    9db4:	e5834014 	str	r4, [r3, #20]
    9db8:	e28dd004 	add	sp, sp, #4	; 0x4
    9dbc:	e8bd8030 	pop	{r4, r5, pc}
    9dc0:	0800d2b8 	.word	0x0800d2b8
    9dc4:	20500000 	.word	0x20500000
    9dc8:	0001350d 	.word	0x0001350d
    9dcc:	0800d76c 	.word	0x0800d76c
    9dd0:	0001354c 	.word	0x0001354c
    9dd4:	00013591 	.word	0x00013591
    9dd8:	000134ec 	.word	0x000134ec

00009ddc <spi_set_intr_recv_fifo>:
    9ddc:	e59f30b4 	ldr	r3, [pc, #180]	; 9e98 <spi_set_intr_recv_fifo+0xbc>
    9de0:	e92d4030 	push	{r4, r5, lr}
    9de4:	e5932000 	ldr	r2, [r3]
    9de8:	e59f30ac 	ldr	r3, [pc, #172]	; 9e9c <spi_set_intr_recv_fifo+0xc0>
    9dec:	e3520000 	cmp	r2, #0	; 0x0
    9df0:	e5933014 	ldr	r3, [r3, #20]
    9df4:	e24dd004 	sub	sp, sp, #4	; 0x4
    9df8:	e1a05000 	mov	r5, r0
    9dfc:	e3c34004 	bic	r4, r3, #4	; 0x4
    9e00:	0a000003 	beq	9e14 <spi_set_intr_recv_fifo+0x38>
    9e04:	e59f0094 	ldr	r0, [pc, #148]	; 9ea0 <spi_set_intr_recv_fifo+0xc4>
    9e08:	e59f3094 	ldr	r3, [pc, #148]	; 9ea4 <spi_set_intr_recv_fifo+0xc8>
    9e0c:	e1a0e00f 	mov	lr, pc
    9e10:	e593f000 	ldr	pc, [r3]
    9e14:	e3550000 	cmp	r5, #0	; 0x0
    9e18:	0a000002 	beq	9e28 <spi_set_intr_recv_fifo+0x4c>
    9e1c:	e3550004 	cmp	r5, #4	; 0x4
    9e20:	1a000011 	bne	9e6c <spi_set_intr_recv_fifo+0x90>
    9e24:	ea000005 	b	9e40 <spi_set_intr_recv_fifo+0x64>
    9e28:	e59f3068 	ldr	r3, [pc, #104]	; 9e98 <spi_set_intr_recv_fifo+0xbc>
    9e2c:	e5933000 	ldr	r3, [r3]
    9e30:	e3530000 	cmp	r3, #0	; 0x0
    9e34:	159f006c 	ldrne	r0, [pc, #108]	; 9ea8 <spi_set_intr_recv_fifo+0xcc>
    9e38:	0a000011 	beq	9e84 <spi_set_intr_recv_fifo+0xa8>
    9e3c:	ea000005 	b	9e58 <spi_set_intr_recv_fifo+0x7c>
    9e40:	e59f3050 	ldr	r3, [pc, #80]	; 9e98 <spi_set_intr_recv_fifo+0xbc>
    9e44:	e3844004 	orr	r4, r4, #4	; 0x4
    9e48:	e5933000 	ldr	r3, [r3]
    9e4c:	e3530000 	cmp	r3, #0	; 0x0
    9e50:	0a00000b 	beq	9e84 <spi_set_intr_recv_fifo+0xa8>
    9e54:	e59f0050 	ldr	r0, [pc, #80]	; 9eac <spi_set_intr_recv_fifo+0xd0>
    9e58:	e1a01004 	mov	r1, r4
    9e5c:	e59f3040 	ldr	r3, [pc, #64]	; 9ea4 <spi_set_intr_recv_fifo+0xc8>
    9e60:	e1a0e00f 	mov	lr, pc
    9e64:	e593f000 	ldr	pc, [r3]
    9e68:	ea000005 	b	9e84 <spi_set_intr_recv_fifo+0xa8>
    9e6c:	e59f003c 	ldr	r0, [pc, #60]	; 9eb0 <spi_set_intr_recv_fifo+0xd4>
    9e70:	e59f302c 	ldr	r3, [pc, #44]	; 9ea4 <spi_set_intr_recv_fifo+0xc8>
    9e74:	e1a0e00f 	mov	lr, pc
    9e78:	e593f000 	ldr	pc, [r3]
    9e7c:	e3e00000 	mvn	r0, #0	; 0x0
    9e80:	ea000002 	b	9e90 <spi_set_intr_recv_fifo+0xb4>
    9e84:	e59f3010 	ldr	r3, [pc, #16]	; 9e9c <spi_set_intr_recv_fifo+0xc0>
    9e88:	e3a00000 	mov	r0, #0	; 0x0
    9e8c:	e5834014 	str	r4, [r3, #20]
    9e90:	e28dd004 	add	sp, sp, #4	; 0x4
    9e94:	e8bd8030 	pop	{r4, r5, pc}
    9e98:	0800d2b8 	.word	0x0800d2b8
    9e9c:	20500000 	.word	0x20500000
    9ea0:	000135d8 	.word	0x000135d8
    9ea4:	0800d76c 	.word	0x0800d76c
    9ea8:	00013614 	.word	0x00013614
    9eac:	00013656 	.word	0x00013656
    9eb0:	000134ec 	.word	0x000134ec

00009eb4 <spi_set_intr_transmit_fifo>:
    9eb4:	e59f30b4 	ldr	r3, [pc, #180]	; 9f70 <spi_set_intr_transmit_fifo+0xbc>
    9eb8:	e92d4030 	push	{r4, r5, lr}
    9ebc:	e5932000 	ldr	r2, [r3]
    9ec0:	e59f30ac 	ldr	r3, [pc, #172]	; 9f74 <spi_set_intr_transmit_fifo+0xc0>
    9ec4:	e3520000 	cmp	r2, #0	; 0x0
    9ec8:	e5933014 	ldr	r3, [r3, #20]
    9ecc:	e24dd004 	sub	sp, sp, #4	; 0x4
    9ed0:	e1a05000 	mov	r5, r0
    9ed4:	e3c34008 	bic	r4, r3, #8	; 0x8
    9ed8:	0a000003 	beq	9eec <spi_set_intr_transmit_fifo+0x38>
    9edc:	e59f0094 	ldr	r0, [pc, #148]	; 9f78 <spi_set_intr_transmit_fifo+0xc4>
    9ee0:	e59f3094 	ldr	r3, [pc, #148]	; 9f7c <spi_set_intr_transmit_fifo+0xc8>
    9ee4:	e1a0e00f 	mov	lr, pc
    9ee8:	e593f000 	ldr	pc, [r3]
    9eec:	e3550000 	cmp	r5, #0	; 0x0
    9ef0:	0a000002 	beq	9f00 <spi_set_intr_transmit_fifo+0x4c>
    9ef4:	e3550008 	cmp	r5, #8	; 0x8
    9ef8:	1a000011 	bne	9f44 <spi_set_intr_transmit_fifo+0x90>
    9efc:	ea000005 	b	9f18 <spi_set_intr_transmit_fifo+0x64>
    9f00:	e59f3068 	ldr	r3, [pc, #104]	; 9f70 <spi_set_intr_transmit_fifo+0xbc>
    9f04:	e5933000 	ldr	r3, [r3]
    9f08:	e3530000 	cmp	r3, #0	; 0x0
    9f0c:	159f006c 	ldrne	r0, [pc, #108]	; 9f80 <spi_set_intr_transmit_fifo+0xcc>
    9f10:	0a000011 	beq	9f5c <spi_set_intr_transmit_fifo+0xa8>
    9f14:	ea000005 	b	9f30 <spi_set_intr_transmit_fifo+0x7c>
    9f18:	e59f3050 	ldr	r3, [pc, #80]	; 9f70 <spi_set_intr_transmit_fifo+0xbc>
    9f1c:	e3844008 	orr	r4, r4, #8	; 0x8
    9f20:	e5933000 	ldr	r3, [r3]
    9f24:	e3530000 	cmp	r3, #0	; 0x0
    9f28:	0a00000b 	beq	9f5c <spi_set_intr_transmit_fifo+0xa8>
    9f2c:	e59f0050 	ldr	r0, [pc, #80]	; 9f84 <spi_set_intr_transmit_fifo+0xd0>
    9f30:	e1a01004 	mov	r1, r4
    9f34:	e59f3040 	ldr	r3, [pc, #64]	; 9f7c <spi_set_intr_transmit_fifo+0xc8>
    9f38:	e1a0e00f 	mov	lr, pc
    9f3c:	e593f000 	ldr	pc, [r3]
    9f40:	ea000005 	b	9f5c <spi_set_intr_transmit_fifo+0xa8>
    9f44:	e59f003c 	ldr	r0, [pc, #60]	; 9f88 <spi_set_intr_transmit_fifo+0xd4>
    9f48:	e59f302c 	ldr	r3, [pc, #44]	; 9f7c <spi_set_intr_transmit_fifo+0xc8>
    9f4c:	e1a0e00f 	mov	lr, pc
    9f50:	e593f000 	ldr	pc, [r3]
    9f54:	e3e00000 	mvn	r0, #0	; 0x0
    9f58:	ea000002 	b	9f68 <spi_set_intr_transmit_fifo+0xb4>
    9f5c:	e59f3010 	ldr	r3, [pc, #16]	; 9f74 <spi_set_intr_transmit_fifo+0xc0>
    9f60:	e3a00000 	mov	r0, #0	; 0x0
    9f64:	e5834014 	str	r4, [r3, #20]
    9f68:	e28dd004 	add	sp, sp, #4	; 0x4
    9f6c:	e8bd8030 	pop	{r4, r5, pc}
    9f70:	0800d2b8 	.word	0x0800d2b8
    9f74:	20500000 	.word	0x20500000
    9f78:	0001369a 	.word	0x0001369a
    9f7c:	0800d76c 	.word	0x0800d76c
    9f80:	000136d7 	.word	0x000136d7
    9f84:	0001371a 	.word	0x0001371a
    9f88:	000134ec 	.word	0x000134ec

00009f8c <spi_get_raw_intr_recv_timeout>:
    9f8c:	e59f300c 	ldr	r3, [pc, #12]	; 9fa0 <spi_get_raw_intr_recv_timeout+0x14>
    9f90:	e5930018 	ldr	r0, [r3, #24]
    9f94:	e1a000a0 	lsr	r0, r0, #1
    9f98:	e2000001 	and	r0, r0, #1	; 0x1
    9f9c:	e12fff1e 	bx	lr
    9fa0:	20500000 	.word	0x20500000

00009fa4 <spi_get_raw_intr_recv_fifo>:
    9fa4:	e59f300c 	ldr	r3, [pc, #12]	; 9fb8 <spi_get_raw_intr_recv_fifo+0x14>
    9fa8:	e5930018 	ldr	r0, [r3, #24]
    9fac:	e1a00120 	lsr	r0, r0, #2
    9fb0:	e2000001 	and	r0, r0, #1	; 0x1
    9fb4:	e12fff1e 	bx	lr
    9fb8:	20500000 	.word	0x20500000

00009fbc <spi_get_raw_intr_trans_fifo>:
    9fbc:	e59f300c 	ldr	r3, [pc, #12]	; 9fd0 <spi_get_raw_intr_trans_fifo+0x14>
    9fc0:	e5930018 	ldr	r0, [r3, #24]
    9fc4:	e1a001a0 	lsr	r0, r0, #3
    9fc8:	e2000001 	and	r0, r0, #1	; 0x1
    9fcc:	e12fff1e 	bx	lr
    9fd0:	20500000 	.word	0x20500000

00009fd4 <spi_clear_intr_recv_overun>:
    9fd4:	e59f3008 	ldr	r3, [pc, #8]	; 9fe4 <spi_clear_intr_recv_overun+0x10>
    9fd8:	e3a02001 	mov	r2, #1	; 0x1
    9fdc:	e5832020 	str	r2, [r3, #32]
    9fe0:	e12fff1e 	bx	lr
    9fe4:	20500000 	.word	0x20500000

00009fe8 <spi_clear_intr_recv_timeout>:
    9fe8:	e59f3008 	ldr	r3, [pc, #8]	; 9ff8 <spi_clear_intr_recv_timeout+0x10>
    9fec:	e3a02002 	mov	r2, #2	; 0x2
    9ff0:	e5832020 	str	r2, [r3, #32]
    9ff4:	e12fff1e 	bx	lr
    9ff8:	20500000 	.word	0x20500000

00009ffc <spi_clear_recv_fifo>:
    9ffc:	e59f2014 	ldr	r2, [pc, #20]	; a018 <spi_clear_recv_fifo+0x1c>
    a000:	ea000000 	b	a008 <spi_clear_recv_fifo+0xc>
    a004:	e5923008 	ldr	r3, [r2, #8]
    a008:	e592300c 	ldr	r3, [r2, #12]
    a00c:	e3130004 	tst	r3, #4	; 0x4
    a010:	1afffffb 	bne	a004 <spi_clear_recv_fifo+0x8>
    a014:	e12fff1e 	bx	lr
    a018:	20500000 	.word	0x20500000

0000a01c <flash_nand_vs_align>:
    a01c:	e3a00b02 	mov	r0, #2048	; 0x800
    a020:	e12fff1e 	bx	lr

0000a024 <spi_transfer_init>:
    a024:	e92d4010 	push	{r4, lr}
    a028:	e2504000 	subs	r4, r0, #0	; 0x0
    a02c:	1a000005 	bne	a048 <spi_transfer_init+0x24>
    a030:	e59f00b4 	ldr	r0, [pc, #180]	; a0ec <spi_transfer_init+0xc8>
    a034:	e59f30b4 	ldr	r3, [pc, #180]	; a0f0 <spi_transfer_init+0xcc>
    a038:	e1a0e00f 	mov	lr, pc
    a03c:	e593f000 	ldr	pc, [r3]
    a040:	e3e00001 	mvn	r0, #1	; 0x1
    a044:	e8bd8010 	pop	{r4, pc}
    a048:	e3a01000 	mov	r1, #0	; 0x0
    a04c:	e3a0205c 	mov	r2, #92	; 0x5c
    a050:	ebffdcb3 	bl	1324 <memset>
    a054:	e3a0200f 	mov	r2, #15	; 0xf
    a058:	e5c42013 	strb	r2, [r4, #19]
    a05c:	e3a02002 	mov	r2, #2	; 0x2
    a060:	e3a03000 	mov	r3, #0	; 0x0
    a064:	e5842030 	str	r2, [r4, #48]
    a068:	e3a02001 	mov	r2, #1	; 0x1
    a06c:	e5842034 	str	r2, [r4, #52]
    a070:	e5c43058 	strb	r3, [r4, #88]
    a074:	e5c43000 	strb	r3, [r4]
    a078:	e5c43001 	strb	r3, [r4, #1]
    a07c:	e5c43002 	strb	r3, [r4, #2]
    a080:	e5c43003 	strb	r3, [r4, #3]
    a084:	e5c43004 	strb	r3, [r4, #4]
    a088:	e5843008 	str	r3, [r4, #8]
    a08c:	e5c4300c 	strb	r3, [r4, #12]
    a090:	e5c4300d 	strb	r3, [r4, #13]
    a094:	e5c4300e 	strb	r3, [r4, #14]
    a098:	e5c4300f 	strb	r3, [r4, #15]
    a09c:	e5c43010 	strb	r3, [r4, #16]
    a0a0:	e5c43011 	strb	r3, [r4, #17]
    a0a4:	e5c43012 	strb	r3, [r4, #18]
    a0a8:	e5843014 	str	r3, [r4, #20]
    a0ac:	e5843018 	str	r3, [r4, #24]
    a0b0:	e584301c 	str	r3, [r4, #28]
    a0b4:	e5843020 	str	r3, [r4, #32]
    a0b8:	e5843024 	str	r3, [r4, #36]
    a0bc:	e5843028 	str	r3, [r4, #40]
    a0c0:	e584302c 	str	r3, [r4, #44]
    a0c4:	e5843038 	str	r3, [r4, #56]
    a0c8:	e584303c 	str	r3, [r4, #60]
    a0cc:	e5843040 	str	r3, [r4, #64]
    a0d0:	e5843044 	str	r3, [r4, #68]
    a0d4:	e5843048 	str	r3, [r4, #72]
    a0d8:	e584304c 	str	r3, [r4, #76]
    a0dc:	e5843050 	str	r3, [r4, #80]
    a0e0:	e5843054 	str	r3, [r4, #84]
    a0e4:	e1a00003 	mov	r0, r3
    a0e8:	e8bd8010 	pop	{r4, pc}
    a0ec:	0001375f 	.word	0x0001375f
    a0f0:	0800d76c 	.word	0x0800d76c

0000a0f4 <spi_read_write_data>:
    a0f4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a0f8:	e2505000 	subs	r5, r0, #0	; 0x0
    a0fc:	e24dd014 	sub	sp, sp, #20	; 0x14
    a100:	03e00001 	mvneq	r0, #1	; 0x1
    a104:	0a00010a 	beq	a534 <spi_read_write_data+0x440>
    a108:	e1a0000d 	mov	r0, sp
    a10c:	e3a01000 	mov	r1, #0	; 0x0
    a110:	e3a02010 	mov	r2, #16	; 0x10
    a114:	ebffdc82 	bl	1324 <memset>
    a118:	e1d531d3 	ldrsb	r3, [r5, #19]
    a11c:	e353000f 	cmp	r3, #15	; 0xf
    a120:	0a000006 	beq	a140 <spi_read_write_data+0x4c>
    a124:	ea0000fe 	b	a524 <spi_read_write_data+0x430>
    a128:	e3510000 	cmp	r1, #0	; 0x0
    a12c:	0a000007 	beq	a150 <spi_read_write_data+0x5c>
    a130:	e59f0404 	ldr	r0, [pc, #1028]	; a53c <spi_read_write_data+0x448>
    a134:	e1a0e00f 	mov	lr, pc
    a138:	e594f000 	ldr	pc, [r4]
    a13c:	ea000002 	b	a14c <spi_read_write_data+0x58>
    a140:	e59f73f8 	ldr	r7, [pc, #1016]	; a540 <spi_read_write_data+0x44c>
    a144:	e59f63f8 	ldr	r6, [pc, #1016]	; a544 <spi_read_write_data+0x450>
    a148:	e59f43f8 	ldr	r4, [pc, #1016]	; a548 <spi_read_write_data+0x454>
    a14c:	e5971000 	ldr	r1, [r7]
    a150:	e596300c 	ldr	r3, [r6, #12]
    a154:	e3130001 	tst	r3, #1	; 0x1
    a158:	0afffff2 	beq	a128 <spi_read_write_data+0x34>
    a15c:	ea000001 	b	a168 <spi_read_write_data+0x74>
    a160:	e5923008 	ldr	r3, [r2, #8]
    a164:	ea000000 	b	a16c <spi_read_write_data+0x78>
    a168:	e59f23d4 	ldr	r2, [pc, #980]	; a544 <spi_read_write_data+0x450>
    a16c:	e592300c 	ldr	r3, [r2, #12]
    a170:	e3130004 	tst	r3, #4	; 0x4
    a174:	1afffff9 	bne	a160 <spi_read_write_data+0x6c>
    a178:	e3510000 	cmp	r1, #0	; 0x0
    a17c:	0a000003 	beq	a190 <spi_read_write_data+0x9c>
    a180:	e59f03c4 	ldr	r0, [pc, #964]	; a54c <spi_read_write_data+0x458>
    a184:	e59f33bc 	ldr	r3, [pc, #956]	; a548 <spi_read_write_data+0x454>
    a188:	e1a0e00f 	mov	lr, pc
    a18c:	e593f000 	ldr	pc, [r3]
    a190:	e1d531d0 	ldrsb	r3, [r5, #16]
    a194:	e1d520d4 	ldrsb	r2, [r5, #4]
    a198:	e5954008 	ldr	r4, [r5, #8]
    a19c:	e1d511d2 	ldrsb	r1, [r5, #18]
    a1a0:	e0833002 	add	r3, r3, r2
    a1a4:	e3540000 	cmp	r4, #0	; 0x0
    a1a8:	e0837001 	add	r7, r3, r1
    a1ac:	1a00001f 	bne	a230 <spi_read_write_data+0x13c>
    a1b0:	e3a0004a 	mov	r0, #74	; 0x4a
    a1b4:	e1a01004 	mov	r1, r4
    a1b8:	ebffdad4 	bl	d10 <pads_setpin>
    a1bc:	e1a01004 	mov	r1, r4
    a1c0:	e1d520d4 	ldrsb	r2, [r5, #4]
    a1c4:	e59f0378 	ldr	r0, [pc, #888]	; a544 <spi_read_write_data+0x450>
    a1c8:	ea000002 	b	a1d8 <spi_read_write_data+0xe4>
    a1cc:	e7d53001 	ldrb	r3, [r5, r1]
    a1d0:	e2811001 	add	r1, r1, #1	; 0x1
    a1d4:	e5803008 	str	r3, [r0, #8]
    a1d8:	e1510002 	cmp	r1, r2
    a1dc:	bafffffa 	blt	a1cc <spi_read_write_data+0xd8>
    a1e0:	e59f235c 	ldr	r2, [pc, #860]	; a544 <spi_read_write_data+0x450>
    a1e4:	e592300c 	ldr	r3, [r2, #12]
    a1e8:	e3130010 	tst	r3, #16	; 0x10
    a1ec:	1afffffc 	bne	a1e4 <spi_read_write_data+0xf0>
    a1f0:	e59f234c 	ldr	r2, [pc, #844]	; a544 <spi_read_write_data+0x450>
    a1f4:	e592300c 	ldr	r3, [r2, #12]
    a1f8:	e3130001 	tst	r3, #1	; 0x1
    a1fc:	0afffffc 	beq	a1f4 <spi_read_write_data+0x100>
    a200:	ea000001 	b	a20c <spi_read_write_data+0x118>
    a204:	e5923008 	ldr	r3, [r2, #8]
    a208:	ea000000 	b	a210 <spi_read_write_data+0x11c>
    a20c:	e59f2330 	ldr	r2, [pc, #816]	; a544 <spi_read_write_data+0x450>
    a210:	e592300c 	ldr	r3, [r2, #12]
    a214:	e2134004 	ands	r4, r3, #4	; 0x4
    a218:	1afffff9 	bne	a204 <spi_read_write_data+0x110>
    a21c:	e3a0004a 	mov	r0, #74	; 0x4a
    a220:	e3a01001 	mov	r1, #1	; 0x1
    a224:	ebffdab9 	bl	d10 <pads_setpin>
    a228:	e1a00004 	mov	r0, r4
    a22c:	ea0000c0 	b	a534 <spi_read_write_data+0x440>
    a230:	e3a02010 	mov	r2, #16	; 0x10
    a234:	e3a01000 	mov	r1, #0	; 0x0
    a238:	e1a0000d 	mov	r0, sp
    a23c:	ebffdc38 	bl	1324 <memset>
    a240:	e1a01005 	mov	r1, r5
    a244:	e1d520d4 	ldrsb	r2, [r5, #4]
    a248:	e1a0000d 	mov	r0, sp
    a24c:	ebffdc3c 	bl	1344 <memcpy>
    a250:	e1d500d4 	ldrsb	r0, [r5, #4]
    a254:	e285100c 	add	r1, r5, #12	; 0xc
    a258:	e1d521d0 	ldrsb	r2, [r5, #16]
    a25c:	e08d0000 	add	r0, sp, r0
    a260:	ebffdc37 	bl	1344 <memcpy>
    a264:	e1d530d4 	ldrsb	r3, [r5, #4]
    a268:	e1d501d0 	ldrsb	r0, [r5, #16]
    a26c:	e1d521d2 	ldrsb	r2, [r5, #18]
    a270:	e0800003 	add	r0, r0, r3
    a274:	e2851011 	add	r1, r5, #17	; 0x11
    a278:	e08d0000 	add	r0, sp, r0
    a27c:	ebffdc30 	bl	1344 <memcpy>
    a280:	e3a01000 	mov	r1, #0	; 0x0
    a284:	e3a0004a 	mov	r0, #74	; 0x4a
    a288:	ebffdaa0 	bl	d10 <pads_setpin>
    a28c:	e1a0400d 	mov	r4, sp
    a290:	e3a02000 	mov	r2, #0	; 0x0
    a294:	e59f12a8 	ldr	r1, [pc, #680]	; a544 <spi_read_write_data+0x450>
    a298:	ea000002 	b	a2a8 <spi_read_write_data+0x1b4>
    a29c:	e19430d2 	ldrsb	r3, [r4, r2]
    a2a0:	e2822001 	add	r2, r2, #1	; 0x1
    a2a4:	e5813008 	str	r3, [r1, #8]
    a2a8:	e1520007 	cmp	r2, r7
    a2ac:	bafffffa 	blt	a29c <spi_read_write_data+0x1a8>
    a2b0:	e5956018 	ldr	r6, [r5, #24]
    a2b4:	e595a024 	ldr	sl, [r5, #36]
    a2b8:	e595801c 	ldr	r8, [r5, #28]
    a2bc:	e59f4280 	ldr	r4, [pc, #640]	; a544 <spi_read_write_data+0x450>
    a2c0:	e3a09000 	mov	r9, #0	; 0x0
    a2c4:	e3a0b001 	mov	fp, #1	; 0x1
    a2c8:	e5943018 	ldr	r3, [r4, #24]
    a2cc:	e3130004 	tst	r3, #4	; 0x4
    a2d0:	0a000036 	beq	a3b0 <spi_read_write_data+0x2bc>
    a2d4:	e3570003 	cmp	r7, #3	; 0x3
    a2d8:	da000005 	ble	a2f4 <spi_read_write_data+0x200>
    a2dc:	e5943008 	ldr	r3, [r4, #8]
    a2e0:	e2477004 	sub	r7, r7, #4	; 0x4
    a2e4:	e5943008 	ldr	r3, [r4, #8]
    a2e8:	e5943008 	ldr	r3, [r4, #8]
    a2ec:	e5943008 	ldr	r3, [r4, #8]
    a2f0:	ea00002e 	b	a3b0 <spi_read_write_data+0x2bc>
    a2f4:	e3570000 	cmp	r7, #0	; 0x0
    a2f8:	da000003 	ble	a30c <spi_read_write_data+0x218>
    a2fc:	e2577001 	subs	r7, r7, #1	; 0x1
    a300:	e5943008 	ldr	r3, [r4, #8]
    a304:	1afffffc 	bne	a2fc <spi_read_write_data+0x208>
    a308:	ea000028 	b	a3b0 <spi_read_write_data+0x2bc>
    a30c:	e3560003 	cmp	r6, #3	; 0x3
    a310:	da000018 	ble	a378 <spi_read_write_data+0x284>
    a314:	e5951014 	ldr	r1, [r5, #20]
    a318:	e3510000 	cmp	r1, #0	; 0x0
    a31c:	0a00000f 	beq	a360 <spi_read_write_data+0x26c>
    a320:	e5942008 	ldr	r2, [r4, #8]
    a324:	e1a03001 	mov	r3, r1
    a328:	e4c32001 	strb	r2, [r3], #1
    a32c:	e5942008 	ldr	r2, [r4, #8]
    a330:	e5853014 	str	r3, [r5, #20]
    a334:	e5c12001 	strb	r2, [r1, #1]
    a338:	e5941008 	ldr	r1, [r4, #8]
    a33c:	e2832001 	add	r2, r3, #1	; 0x1
    a340:	e5852014 	str	r2, [r5, #20]
    a344:	e5c31001 	strb	r1, [r3, #1]
    a348:	e5941008 	ldr	r1, [r4, #8]
    a34c:	e2823001 	add	r3, r2, #1	; 0x1
    a350:	e2830001 	add	r0, r3, #1	; 0x1
    a354:	e5c21001 	strb	r1, [r2, #1]
    a358:	e5850014 	str	r0, [r5, #20]
    a35c:	ea000003 	b	a370 <spi_read_write_data+0x27c>
    a360:	e5943008 	ldr	r3, [r4, #8]
    a364:	e5943008 	ldr	r3, [r4, #8]
    a368:	e5943008 	ldr	r3, [r4, #8]
    a36c:	e5943008 	ldr	r3, [r4, #8]
    a370:	e2466004 	sub	r6, r6, #4	; 0x4
    a374:	ea00000b 	b	a3a8 <spi_read_write_data+0x2b4>
    a378:	e3560000 	cmp	r6, #0	; 0x0
    a37c:	da000009 	ble	a3a8 <spi_read_write_data+0x2b4>
    a380:	e59f11bc 	ldr	r1, [pc, #444]	; a544 <spi_read_write_data+0x450>
    a384:	e5953014 	ldr	r3, [r5, #20]
    a388:	e2532000 	subs	r2, r3, #0	; 0x0
    a38c:	15913008 	ldrne	r3, [r1, #8]
    a390:	05913008 	ldreq	r3, [r1, #8]
    a394:	14c23001 	strbne	r3, [r2], #1
    a398:	15852014 	strne	r2, [r5, #20]
    a39c:	e2566001 	subs	r6, r6, #1	; 0x1
    a3a0:	1afffff7 	bne	a384 <spi_read_write_data+0x290>
    a3a4:	ea000056 	b	a504 <spi_read_write_data+0x410>
    a3a8:	e3560000 	cmp	r6, #0	; 0x0
    a3ac:	0a000054 	beq	a504 <spi_read_write_data+0x410>
    a3b0:	e5943018 	ldr	r3, [r4, #24]
    a3b4:	e3130008 	tst	r3, #8	; 0x8
    a3b8:	0a00002b 	beq	a46c <spi_read_write_data+0x378>
    a3bc:	e35a0003 	cmp	sl, #3	; 0x3
    a3c0:	da000012 	ble	a410 <spi_read_write_data+0x31c>
    a3c4:	e5951020 	ldr	r1, [r5, #32]
    a3c8:	e24aa004 	sub	sl, sl, #4	; 0x4
    a3cc:	e1a02001 	mov	r2, r1
    a3d0:	e4d23001 	ldrb	r3, [r2], #1
    a3d4:	e5843008 	str	r3, [r4, #8]
    a3d8:	e5852020 	str	r2, [r5, #32]
    a3dc:	e5d13001 	ldrb	r3, [r1, #1]
    a3e0:	e2821001 	add	r1, r2, #1	; 0x1
    a3e4:	e5843008 	str	r3, [r4, #8]
    a3e8:	e5851020 	str	r1, [r5, #32]
    a3ec:	e5d22001 	ldrb	r2, [r2, #1]
    a3f0:	e2813001 	add	r3, r1, #1	; 0x1
    a3f4:	e5853020 	str	r3, [r5, #32]
    a3f8:	e5842008 	str	r2, [r4, #8]
    a3fc:	e5d12001 	ldrb	r2, [r1, #1]
    a400:	e2833001 	add	r3, r3, #1	; 0x1
    a404:	e5842008 	str	r2, [r4, #8]
    a408:	e5853020 	str	r3, [r5, #32]
    a40c:	ea000016 	b	a46c <spi_read_write_data+0x378>
    a410:	e35a0000 	cmp	sl, #0	; 0x0
    a414:	da000006 	ble	a434 <spi_read_write_data+0x340>
    a418:	e5953020 	ldr	r3, [r5, #32]
    a41c:	e25aa001 	subs	sl, sl, #1	; 0x1
    a420:	e4d32001 	ldrb	r2, [r3], #1
    a424:	e5842008 	str	r2, [r4, #8]
    a428:	e5853020 	str	r3, [r5, #32]
    a42c:	1afffff9 	bne	a418 <spi_read_write_data+0x324>
    a430:	ea00000d 	b	a46c <spi_read_write_data+0x378>
    a434:	e3580003 	cmp	r8, #3	; 0x3
    a438:	da000005 	ble	a454 <spi_read_write_data+0x360>
    a43c:	e5849008 	str	r9, [r4, #8]
    a440:	e2488004 	sub	r8, r8, #4	; 0x4
    a444:	e5849008 	str	r9, [r4, #8]
    a448:	e5849008 	str	r9, [r4, #8]
    a44c:	e5849008 	str	r9, [r4, #8]
    a450:	ea000005 	b	a46c <spi_read_write_data+0x378>
    a454:	e3580000 	cmp	r8, #0	; 0x0
    a458:	da000003 	ble	a46c <spi_read_write_data+0x378>
    a45c:	e3a03000 	mov	r3, #0	; 0x0
    a460:	e2588001 	subs	r8, r8, #1	; 0x1
    a464:	e5843008 	str	r3, [r4, #8]
    a468:	1afffffb 	bne	a45c <spi_read_write_data+0x368>
    a46c:	e5943018 	ldr	r3, [r4, #24]
    a470:	e3130002 	tst	r3, #2	; 0x2
    a474:	0a000020 	beq	a4fc <spi_read_write_data+0x408>
    a478:	e594300c 	ldr	r3, [r4, #12]
    a47c:	e3130004 	tst	r3, #4	; 0x4
    a480:	0a00001d 	beq	a4fc <spi_read_write_data+0x408>
    a484:	ea00000a 	b	a4b4 <spi_read_write_data+0x3c0>
    a488:	e594300c 	ldr	r3, [r4, #12]
    a48c:	e3130004 	tst	r3, #4	; 0x4
    a490:	15943008 	ldrne	r3, [r4, #8]
    a494:	12477001 	subne	r7, r7, #1	; 0x1
    a498:	1a000005 	bne	a4b4 <spi_read_write_data+0x3c0>
    a49c:	e59f00ac 	ldr	r0, [pc, #172]	; a550 <spi_read_write_data+0x45c>
    a4a0:	e1a01007 	mov	r1, r7
    a4a4:	e59f309c 	ldr	r3, [pc, #156]	; a548 <spi_read_write_data+0x454>
    a4a8:	e1a0e00f 	mov	lr, pc
    a4ac:	e593f000 	ldr	pc, [r3]
    a4b0:	ea000011 	b	a4fc <spi_read_write_data+0x408>
    a4b4:	e3570000 	cmp	r7, #0	; 0x0
    a4b8:	cafffff2 	bgt	a488 <spi_read_write_data+0x394>
    a4bc:	ea000009 	b	a4e8 <spi_read_write_data+0x3f4>
    a4c0:	e594300c 	ldr	r3, [r4, #12]
    a4c4:	e3130004 	tst	r3, #4	; 0x4
    a4c8:	0a00000b 	beq	a4fc <spi_read_write_data+0x408>
    a4cc:	e5953014 	ldr	r3, [r5, #20]
    a4d0:	e2466001 	sub	r6, r6, #1	; 0x1
    a4d4:	e2532000 	subs	r2, r3, #0	; 0x0
    a4d8:	15943008 	ldrne	r3, [r4, #8]
    a4dc:	05943008 	ldreq	r3, [r4, #8]
    a4e0:	14c23001 	strbne	r3, [r2], #1
    a4e4:	15852014 	strne	r2, [r5, #20]
    a4e8:	e3560000 	cmp	r6, #0	; 0x0
    a4ec:	cafffff3 	bgt	a4c0 <spi_read_write_data+0x3cc>
    a4f0:	e3a03002 	mov	r3, #2	; 0x2
    a4f4:	e5843020 	str	r3, [r4, #32]
    a4f8:	0a000001 	beq	a504 <spi_read_write_data+0x410>
    a4fc:	e584b020 	str	fp, [r4, #32]
    a500:	eaffff70 	b	a2c8 <spi_read_write_data+0x1d4>
    a504:	e59f2038 	ldr	r2, [pc, #56]	; a544 <spi_read_write_data+0x450>
    a508:	e592300c 	ldr	r3, [r2, #12]
    a50c:	e3130010 	tst	r3, #16	; 0x10
    a510:	1afffffc 	bne	a508 <spi_read_write_data+0x414>
    a514:	e59f2028 	ldr	r2, [pc, #40]	; a544 <spi_read_write_data+0x450>
    a518:	e592300c 	ldr	r3, [r2, #12]
    a51c:	e3130001 	tst	r3, #1	; 0x1
    a520:	0afffffc 	beq	a518 <spi_read_write_data+0x424>
    a524:	e3a0004a 	mov	r0, #74	; 0x4a
    a528:	e3a01001 	mov	r1, #1	; 0x1
    a52c:	ebffd9f7 	bl	d10 <pads_setpin>
    a530:	e3a00000 	mov	r0, #0	; 0x0
    a534:	e28dd014 	add	sp, sp, #20	; 0x14
    a538:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a53c:	00013782 	.word	0x00013782
    a540:	0800d2b8 	.word	0x0800d2b8
    a544:	20500000 	.word	0x20500000
    a548:	0800d76c 	.word	0x0800d76c
    a54c:	000137aa 	.word	0x000137aa
    a550:	000137cb 	.word	0x000137cb

0000a554 <flash_set_device_status>:
    a554:	e92d4070 	push	{r4, r5, r6, lr}
    a558:	e24dd060 	sub	sp, sp, #96	; 0x60
    a55c:	e1a04000 	mov	r4, r0
    a560:	e28d0004 	add	r0, sp, #4	; 0x4
    a564:	e1a06001 	mov	r6, r1
    a568:	e1a05002 	mov	r5, r2
    a56c:	ebfffeac 	bl	a024 <spi_transfer_init>
    a570:	e59f303c 	ldr	r3, [pc, #60]	; a5b4 <flash_set_device_status+0x60>
    a574:	e28d0060 	add	r0, sp, #96	; 0x60
    a578:	e5933000 	ldr	r3, [r3]
    a57c:	e560405c 	strb	r4, [r0, #-92]!
    a580:	e353000f 	cmp	r3, #15	; 0xf
    a584:	13a03001 	movne	r3, #1	; 0x1
    a588:	e5cd3017 	strb	r3, [sp, #23]
    a58c:	e3a04000 	mov	r4, #0	; 0x0
    a590:	e3a03003 	mov	r3, #3	; 0x3
    a594:	e5cd3008 	strb	r3, [sp, #8]
    a598:	e5cd5006 	strb	r5, [sp, #6]
    a59c:	e5cd6005 	strb	r6, [sp, #5]
    a5a0:	e58d400c 	str	r4, [sp, #12]
    a5a4:	ebfffed2 	bl	a0f4 <spi_read_write_data>
    a5a8:	e1a00004 	mov	r0, r4
    a5ac:	e28dd060 	add	sp, sp, #96	; 0x60
    a5b0:	e8bd8070 	pop	{r4, r5, r6, pc}
    a5b4:	0800d258 	.word	0x0800d258

0000a5b8 <flash_write_enable>:
    a5b8:	e92d4010 	push	{r4, lr}
    a5bc:	e24dd060 	sub	sp, sp, #96	; 0x60
    a5c0:	e28d0004 	add	r0, sp, #4	; 0x4
    a5c4:	ebfffe96 	bl	a024 <spi_transfer_init>
    a5c8:	e59f3030 	ldr	r3, [pc, #48]	; a600 <flash_write_enable+0x48>
    a5cc:	e28d0060 	add	r0, sp, #96	; 0x60
    a5d0:	e5933000 	ldr	r3, [r3]
    a5d4:	e3a04000 	mov	r4, #0	; 0x0
    a5d8:	e5cd3017 	strb	r3, [sp, #23]
    a5dc:	e3a03006 	mov	r3, #6	; 0x6
    a5e0:	e560305c 	strb	r3, [r0, #-92]!
    a5e4:	e3a03001 	mov	r3, #1	; 0x1
    a5e8:	e5cd3008 	strb	r3, [sp, #8]
    a5ec:	e58d400c 	str	r4, [sp, #12]
    a5f0:	ebfffebf 	bl	a0f4 <spi_read_write_data>
    a5f4:	e1a00004 	mov	r0, r4
    a5f8:	e28dd060 	add	sp, sp, #96	; 0x60
    a5fc:	e8bd8010 	pop	{r4, pc}
    a600:	0800d258 	.word	0x0800d258

0000a604 <nand_flash_reset>:
    a604:	e92d4010 	push	{r4, lr}
    a608:	e24dd060 	sub	sp, sp, #96	; 0x60
    a60c:	e28d4004 	add	r4, sp, #4	; 0x4
    a610:	e1a00004 	mov	r0, r4
    a614:	ebfffe82 	bl	a024 <spi_transfer_init>
    a618:	e3a03001 	mov	r3, #1	; 0x1
    a61c:	e5cd3008 	strb	r3, [sp, #8]
    a620:	e3a03000 	mov	r3, #0	; 0x0
    a624:	e58d300c 	str	r3, [sp, #12]
    a628:	e1a00004 	mov	r0, r4
    a62c:	e283300f 	add	r3, r3, #15	; 0xf
    a630:	e3e04000 	mvn	r4, #0	; 0x0
    a634:	e5cd3017 	strb	r3, [sp, #23]
    a638:	e5cd4004 	strb	r4, [sp, #4]
    a63c:	ebfffeac 	bl	a0f4 <spi_read_write_data>
    a640:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    a644:	ebfff881 	bl	8850 <udelay>
    a648:	e1a00004 	mov	r0, r4
    a64c:	e28dd060 	add	sp, sp, #96	; 0x60
    a650:	e8bd8010 	pop	{r4, pc}

0000a654 <flash_wait_device_status>:
    a654:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    a658:	e2514000 	subs	r4, r1, #0	; 0x0
    a65c:	e24dd064 	sub	sp, sp, #100	; 0x64
    a660:	e1a03000 	mov	r3, r0
    a664:	aa000005 	bge	a680 <flash_wait_device_status+0x2c>
    a668:	e59f0364 	ldr	r0, [pc, #868]	; a9d4 <flash_wait_device_status+0x380>
    a66c:	e59f3364 	ldr	r3, [pc, #868]	; a9d8 <flash_wait_device_status+0x384>
    a670:	e1a0e00f 	mov	lr, pc
    a674:	e593f000 	ldr	pc, [r3]
    a678:	e3e00001 	mvn	r0, #1	; 0x1
    a67c:	ea0000d2 	b	a9cc <flash_wait_device_status+0x378>
    a680:	e1a0000d 	mov	r0, sp
    a684:	e6ef5073 	uxtb	r5, r3
    a688:	ebfffe65 	bl	a024 <spi_transfer_init>
    a68c:	e3a03002 	mov	r3, #2	; 0x2
    a690:	e58d3008 	str	r3, [sp, #8]
    a694:	e28d305c 	add	r3, sp, #92	; 0x5c
    a698:	e3a02001 	mov	r2, #1	; 0x1
    a69c:	e58d3014 	str	r3, [sp, #20]
    a6a0:	e2441001 	sub	r1, r4, #1	; 0x1
    a6a4:	e3a0300f 	mov	r3, #15	; 0xf
    a6a8:	e58d201c 	str	r2, [sp, #28]
    a6ac:	e5cd3013 	strb	r3, [sp, #19]
    a6b0:	e5cd5000 	strb	r5, [sp]
    a6b4:	e5cd2004 	strb	r2, [sp, #4]
    a6b8:	e58d2018 	str	r2, [sp, #24]
    a6bc:	e1510003 	cmp	r1, r3
    a6c0:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    a6c4:	ea0000b6 	b	a9a4 <flash_wait_device_status+0x350>
    a6c8:	0000a708 	.word	0x0000a708
    a6cc:	0000a9a4 	.word	0x0000a9a4
    a6d0:	0000a9a4 	.word	0x0000a9a4
    a6d4:	0000a9a4 	.word	0x0000a9a4
    a6d8:	0000a9a4 	.word	0x0000a9a4
    a6dc:	0000a8b8 	.word	0x0000a8b8
    a6e0:	0000a934 	.word	0x0000a934
    a6e4:	0000a78c 	.word	0x0000a78c
    a6e8:	0000a7f4 	.word	0x0000a7f4
    a6ec:	0000a9a4 	.word	0x0000a9a4
    a6f0:	0000a9a4 	.word	0x0000a9a4
    a6f4:	0000a9a4 	.word	0x0000a9a4
    a6f8:	0000a9a4 	.word	0x0000a9a4
    a6fc:	0000a9a4 	.word	0x0000a9a4
    a700:	0000a9a4 	.word	0x0000a9a4
    a704:	0000a84c 	.word	0x0000a84c
    a708:	e59f72cc 	ldr	r7, [pc, #716]	; a9dc <flash_wait_device_status+0x388>
    a70c:	e59f62c4 	ldr	r6, [pc, #708]	; a9d8 <flash_wait_device_status+0x384>
    a710:	e28d405c 	add	r4, sp, #92	; 0x5c
    a714:	e1a0500d 	mov	r5, sp
    a718:	e3a01000 	mov	r1, #0	; 0x0
    a71c:	e3a02002 	mov	r2, #2	; 0x2
    a720:	e1a00004 	mov	r0, r4
    a724:	ebffdafe 	bl	1324 <memset>
    a728:	e1a0000d 	mov	r0, sp
    a72c:	e58d4014 	str	r4, [sp, #20]
    a730:	ebfffe6f 	bl	a0f4 <spi_read_write_data>
    a734:	e5dd105c 	ldrb	r1, [sp, #92]
    a738:	e3110002 	tst	r1, #2	; 0x2
    a73c:	0a000007 	beq	a760 <flash_wait_device_status+0x10c>
    a740:	e3110001 	tst	r1, #1	; 0x1
    a744:	1a000005 	bne	a760 <flash_wait_device_status+0x10c>
    a748:	e59f328c 	ldr	r3, [pc, #652]	; a9dc <flash_wait_device_status+0x388>
    a74c:	e5933000 	ldr	r3, [r3]
    a750:	e3530000 	cmp	r3, #0	; 0x0
    a754:	159f0284 	ldrne	r0, [pc, #644]	; a9e0 <flash_wait_device_status+0x38c>
    a758:	0a000091 	beq	a9a4 <flash_wait_device_status+0x350>
    a75c:	ea000020 	b	a7e4 <flash_wait_device_status+0x190>
    a760:	ebffff94 	bl	a5b8 <flash_write_enable>
    a764:	e5973000 	ldr	r3, [r7]
    a768:	e3530000 	cmp	r3, #0	; 0x0
    a76c:	0a000003 	beq	a780 <flash_wait_device_status+0x12c>
    a770:	e5dd105c 	ldrb	r1, [sp, #92]
    a774:	e59f0268 	ldr	r0, [pc, #616]	; a9e4 <flash_wait_device_status+0x390>
    a778:	e1a0e00f 	mov	lr, pc
    a77c:	e596f000 	ldr	pc, [r6]
    a780:	e3a00032 	mov	r0, #50	; 0x32
    a784:	ebfff831 	bl	8850 <udelay>
    a788:	eaffffe2 	b	a718 <flash_wait_device_status+0xc4>
    a78c:	e3a0c002 	mov	ip, #2	; 0x2
    a790:	e28d405c 	add	r4, sp, #92	; 0x5c
    a794:	e3e0305f 	mvn	r3, #95	; 0x5f
    a798:	e1a0200c 	mov	r2, ip
    a79c:	e3a01000 	mov	r1, #0	; 0x0
    a7a0:	e1a00004 	mov	r0, r4
    a7a4:	e5cd3001 	strb	r3, [sp, #1]
    a7a8:	e5cdc004 	strb	ip, [sp, #4]
    a7ac:	e58dc008 	str	ip, [sp, #8]
    a7b0:	e5cd5000 	strb	r5, [sp]
    a7b4:	ebffdada 	bl	1324 <memset>
    a7b8:	e3a03001 	mov	r3, #1	; 0x1
    a7bc:	e1a0000d 	mov	r0, sp
    a7c0:	e58d3018 	str	r3, [sp, #24]
    a7c4:	e58d4014 	str	r4, [sp, #20]
    a7c8:	ebfffe49 	bl	a0f4 <spi_read_write_data>
    a7cc:	e59f3208 	ldr	r3, [pc, #520]	; a9dc <flash_wait_device_status+0x388>
    a7d0:	e5933000 	ldr	r3, [r3]
    a7d4:	e3530000 	cmp	r3, #0	; 0x0
    a7d8:	0a000071 	beq	a9a4 <flash_wait_device_status+0x350>
    a7dc:	e59f0204 	ldr	r0, [pc, #516]	; a9e8 <flash_wait_device_status+0x394>
    a7e0:	e5dd105c 	ldrb	r1, [sp, #92]
    a7e4:	e59f31ec 	ldr	r3, [pc, #492]	; a9d8 <flash_wait_device_status+0x384>
    a7e8:	e1a0e00f 	mov	lr, pc
    a7ec:	e593f000 	ldr	pc, [r3]
    a7f0:	ea00006b 	b	a9a4 <flash_wait_device_status+0x350>
    a7f4:	e3a0c002 	mov	ip, #2	; 0x2
    a7f8:	e28d405c 	add	r4, sp, #92	; 0x5c
    a7fc:	e3e0304f 	mvn	r3, #79	; 0x4f
    a800:	e1a0200c 	mov	r2, ip
    a804:	e3a01000 	mov	r1, #0	; 0x0
    a808:	e1a00004 	mov	r0, r4
    a80c:	e5cd3001 	strb	r3, [sp, #1]
    a810:	e5cdc004 	strb	ip, [sp, #4]
    a814:	e58dc008 	str	ip, [sp, #8]
    a818:	e5cd5000 	strb	r5, [sp]
    a81c:	ebffdac0 	bl	1324 <memset>
    a820:	e3a03001 	mov	r3, #1	; 0x1
    a824:	e1a0000d 	mov	r0, sp
    a828:	e58d3018 	str	r3, [sp, #24]
    a82c:	e58d4014 	str	r4, [sp, #20]
    a830:	ebfffe2f 	bl	a0f4 <spi_read_write_data>
    a834:	e59f31a0 	ldr	r3, [pc, #416]	; a9dc <flash_wait_device_status+0x388>
    a838:	e5933000 	ldr	r3, [r3]
    a83c:	e3530000 	cmp	r3, #0	; 0x0
    a840:	159f01a4 	ldrne	r0, [pc, #420]	; a9ec <flash_wait_device_status+0x398>
    a844:	0a000019 	beq	a8b0 <flash_wait_device_status+0x25c>
    a848:	ea000014 	b	a8a0 <flash_wait_device_status+0x24c>
    a84c:	e3a0c002 	mov	ip, #2	; 0x2
    a850:	e28d405c 	add	r4, sp, #92	; 0x5c
    a854:	e3e0303f 	mvn	r3, #63	; 0x3f
    a858:	e1a0200c 	mov	r2, ip
    a85c:	e3a01000 	mov	r1, #0	; 0x0
    a860:	e1a00004 	mov	r0, r4
    a864:	e5cd3001 	strb	r3, [sp, #1]
    a868:	e5cdc004 	strb	ip, [sp, #4]
    a86c:	e58dc008 	str	ip, [sp, #8]
    a870:	e5cd5000 	strb	r5, [sp]
    a874:	ebffdaaa 	bl	1324 <memset>
    a878:	e3a03001 	mov	r3, #1	; 0x1
    a87c:	e1a0000d 	mov	r0, sp
    a880:	e58d3018 	str	r3, [sp, #24]
    a884:	e58d4014 	str	r4, [sp, #20]
    a888:	ebfffe19 	bl	a0f4 <spi_read_write_data>
    a88c:	e59f3148 	ldr	r3, [pc, #328]	; a9dc <flash_wait_device_status+0x388>
    a890:	e5933000 	ldr	r3, [r3]
    a894:	e3530000 	cmp	r3, #0	; 0x0
    a898:	0a000004 	beq	a8b0 <flash_wait_device_status+0x25c>
    a89c:	e59f014c 	ldr	r0, [pc, #332]	; a9f0 <flash_wait_device_status+0x39c>
    a8a0:	e5dd105c 	ldrb	r1, [sp, #92]
    a8a4:	e59f312c 	ldr	r3, [pc, #300]	; a9d8 <flash_wait_device_status+0x384>
    a8a8:	e1a0e00f 	mov	lr, pc
    a8ac:	e593f000 	ldr	pc, [r3]
    a8b0:	e5dd005c 	ldrb	r0, [sp, #92]
    a8b4:	ea000044 	b	a9cc <flash_wait_device_status+0x378>
    a8b8:	e3a02002 	mov	r2, #2	; 0x2
    a8bc:	e3e0303f 	mvn	r3, #63	; 0x3f
    a8c0:	e5cd3001 	strb	r3, [sp, #1]
    a8c4:	e58d2008 	str	r2, [sp, #8]
    a8c8:	e5cd5000 	strb	r5, [sp]
    a8cc:	e5cd2004 	strb	r2, [sp, #4]
    a8d0:	e3a04000 	mov	r4, #0	; 0x0
    a8d4:	e28d605c 	add	r6, sp, #92	; 0x5c
    a8d8:	e1a0700d 	mov	r7, sp
    a8dc:	e3a01000 	mov	r1, #0	; 0x0
    a8e0:	e3a02002 	mov	r2, #2	; 0x2
    a8e4:	e1a00006 	mov	r0, r6
    a8e8:	ebffda8d 	bl	1324 <memset>
    a8ec:	e1a0000d 	mov	r0, sp
    a8f0:	e58d6014 	str	r6, [sp, #20]
    a8f4:	ebfffdfe 	bl	a0f4 <spi_read_write_data>
    a8f8:	e5dd305c 	ldrb	r3, [sp, #92]
    a8fc:	e2844001 	add	r4, r4, #1	; 0x1
    a900:	e3130001 	tst	r3, #1	; 0x1
    a904:	e3a0001e 	mov	r0, #30	; 0x1e
    a908:	0a000009 	beq	a934 <flash_wait_device_status+0x2e0>
    a90c:	ebfff7cf 	bl	8850 <udelay>
    a910:	e3540064 	cmp	r4, #100	; 0x64
    a914:	1afffff0 	bne	a8dc <flash_wait_device_status+0x288>
    a918:	e59f00d4 	ldr	r0, [pc, #212]	; a9f4 <flash_wait_device_status+0x3a0>
    a91c:	e5dd105c 	ldrb	r1, [sp, #92]
    a920:	e59f30b0 	ldr	r3, [pc, #176]	; a9d8 <flash_wait_device_status+0x384>
    a924:	e1a0e00f 	mov	lr, pc
    a928:	e593f000 	ldr	pc, [r3]
    a92c:	e3e00000 	mvn	r0, #0	; 0x0
    a930:	ea000025 	b	a9cc <flash_wait_device_status+0x378>
    a934:	e3a02002 	mov	r2, #2	; 0x2
    a938:	e3e0303f 	mvn	r3, #63	; 0x3f
    a93c:	e59f7098 	ldr	r7, [pc, #152]	; a9dc <flash_wait_device_status+0x388>
    a940:	e59f6090 	ldr	r6, [pc, #144]	; a9d8 <flash_wait_device_status+0x384>
    a944:	e5cd5000 	strb	r5, [sp]
    a948:	e5cd3001 	strb	r3, [sp, #1]
    a94c:	e58d2008 	str	r2, [sp, #8]
    a950:	e5cd2004 	strb	r2, [sp, #4]
    a954:	e28d405c 	add	r4, sp, #92	; 0x5c
    a958:	e1a0500d 	mov	r5, sp
    a95c:	e3a01000 	mov	r1, #0	; 0x0
    a960:	e3a02002 	mov	r2, #2	; 0x2
    a964:	e1a00004 	mov	r0, r4
    a968:	ebffda6d 	bl	1324 <memset>
    a96c:	e1a0000d 	mov	r0, sp
    a970:	e58d4014 	str	r4, [sp, #20]
    a974:	ebfffdde 	bl	a0f4 <spi_read_write_data>
    a978:	e5dd105c 	ldrb	r1, [sp, #92]
    a97c:	e3110002 	tst	r1, #2	; 0x2
    a980:	1a000007 	bne	a9a4 <flash_wait_device_status+0x350>
    a984:	e5973000 	ldr	r3, [r7]
    a988:	e3530000 	cmp	r3, #0	; 0x0
    a98c:	0a000002 	beq	a99c <flash_wait_device_status+0x348>
    a990:	e59f0060 	ldr	r0, [pc, #96]	; a9f8 <flash_wait_device_status+0x3a4>
    a994:	e1a0e00f 	mov	lr, pc
    a998:	e596f000 	ldr	pc, [r6]
    a99c:	ebffff05 	bl	a5b8 <flash_write_enable>
    a9a0:	eaffffed 	b	a95c <flash_wait_device_status+0x308>
    a9a4:	e59f3030 	ldr	r3, [pc, #48]	; a9dc <flash_wait_device_status+0x388>
    a9a8:	e5930000 	ldr	r0, [r3]
    a9ac:	e3500000 	cmp	r0, #0	; 0x0
    a9b0:	0a000005 	beq	a9cc <flash_wait_device_status+0x378>
    a9b4:	e59f0040 	ldr	r0, [pc, #64]	; a9fc <flash_wait_device_status+0x3a8>
    a9b8:	e5dd105c 	ldrb	r1, [sp, #92]
    a9bc:	e59f3014 	ldr	r3, [pc, #20]	; a9d8 <flash_wait_device_status+0x384>
    a9c0:	e1a0e00f 	mov	lr, pc
    a9c4:	e593f000 	ldr	pc, [r3]
    a9c8:	e3a00000 	mov	r0, #0	; 0x0
    a9cc:	e28dd064 	add	sp, sp, #100	; 0x64
    a9d0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    a9d4:	000137f7 	.word	0x000137f7
    a9d8:	0800d76c 	.word	0x0800d76c
    a9dc:	0800d2b8 	.word	0x0800d2b8
    a9e0:	00013823 	.word	0x00013823
    a9e4:	0001384d 	.word	0x0001384d
    a9e8:	00013870 	.word	0x00013870
    a9ec:	0001389b 	.word	0x0001389b
    a9f0:	000138c6 	.word	0x000138c6
    a9f4:	000138f1 	.word	0x000138f1
    a9f8:	00013930 	.word	0x00013930
    a9fc:	00013971 	.word	0x00013971

0000aa00 <nand_write_cache_to_memory>:
    aa00:	e92d4030 	push	{r4, r5, lr}
    aa04:	e24dd064 	sub	sp, sp, #100	; 0x64
    aa08:	e1a05000 	mov	r5, r0
    aa0c:	e28d0004 	add	r0, sp, #4	; 0x4
    aa10:	e1a04001 	mov	r4, r1
    aa14:	ebfffd82 	bl	a024 <spi_transfer_init>
    aa18:	e354000f 	cmp	r4, #15	; 0xf
    aa1c:	13a03001 	movne	r3, #1	; 0x1
    aa20:	15cd3017 	strbne	r3, [sp, #23]
    aa24:	e3a03010 	mov	r3, #16	; 0x10
    aa28:	e1a01335 	lsr	r1, r5, r3
    aa2c:	e28d0060 	add	r0, sp, #96	; 0x60
    aa30:	05cd4017 	strbeq	r4, [sp, #23]
    aa34:	e1a02425 	lsr	r2, r5, #8
    aa38:	e3a04000 	mov	r4, #0	; 0x0
    aa3c:	e560305c 	strb	r3, [r0, #-92]!
    aa40:	e3a03004 	mov	r3, #4	; 0x4
    aa44:	e5cd3008 	strb	r3, [sp, #8]
    aa48:	e5cd2006 	strb	r2, [sp, #6]
    aa4c:	e5cd1005 	strb	r1, [sp, #5]
    aa50:	e58d400c 	str	r4, [sp, #12]
    aa54:	e5cd5007 	strb	r5, [sp, #7]
    aa58:	ebfffda5 	bl	a0f4 <spi_read_write_data>
    aa5c:	e3a01006 	mov	r1, #6	; 0x6
    aa60:	e3a0000f 	mov	r0, #15	; 0xf
    aa64:	ebfffefa 	bl	a654 <flash_wait_device_status>
    aa68:	e1a00004 	mov	r0, r4
    aa6c:	e28dd064 	add	sp, sp, #100	; 0x64
    aa70:	e8bd8030 	pop	{r4, r5, pc}

0000aa74 <nand_read_page_to_cache>:
    aa74:	e92d4030 	push	{r4, r5, lr}
    aa78:	e24dd064 	sub	sp, sp, #100	; 0x64
    aa7c:	e1a04001 	mov	r4, r1
    aa80:	e1a05000 	mov	r5, r0
    aa84:	e28d0004 	add	r0, sp, #4	; 0x4
    aa88:	ebfffd65 	bl	a024 <spi_transfer_init>
    aa8c:	e354000f 	cmp	r4, #15	; 0xf
    aa90:	e28d0060 	add	r0, sp, #96	; 0x60
    aa94:	e3a03013 	mov	r3, #19	; 0x13
    aa98:	05cd4017 	strbeq	r4, [sp, #23]
    aa9c:	e1a01825 	lsr	r1, r5, #16
    aaa0:	e1a02425 	lsr	r2, r5, #8
    aaa4:	e3a04000 	mov	r4, #0	; 0x0
    aaa8:	e560305c 	strb	r3, [r0, #-92]!
    aaac:	e3a03004 	mov	r3, #4	; 0x4
    aab0:	e5cd3008 	strb	r3, [sp, #8]
    aab4:	e5cd2006 	strb	r2, [sp, #6]
    aab8:	e5cd1005 	strb	r1, [sp, #5]
    aabc:	e58d400c 	str	r4, [sp, #12]
    aac0:	e5cd5007 	strb	r5, [sp, #7]
    aac4:	ebfffd8a 	bl	a0f4 <spi_read_write_data>
    aac8:	e3a01006 	mov	r1, #6	; 0x6
    aacc:	e3a0000f 	mov	r0, #15	; 0xf
    aad0:	ebfffedf 	bl	a654 <flash_wait_device_status>
    aad4:	e1a00004 	mov	r0, r4
    aad8:	e28dd064 	add	sp, sp, #100	; 0x64
    aadc:	e8bd8030 	pop	{r4, r5, pc}

0000aae0 <nand_blk_prg_era_fail>:
    aae0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    aae4:	e3a0000f 	mov	r0, #15	; 0xf
    aae8:	e24dd004 	sub	sp, sp, #4	; 0x4
    aaec:	e3a01010 	mov	r1, #16	; 0x10
    aaf0:	ebfffed7 	bl	a654 <flash_wait_device_status>
    aaf4:	e3100008 	tst	r0, #8	; 0x8
    aaf8:	0a000004 	beq	ab10 <nand_blk_prg_era_fail+0x30>
    aafc:	e59f0038 	ldr	r0, [pc, #56]	; ab3c <nand_blk_prg_era_fail+0x5c>
    ab00:	e59f3038 	ldr	r3, [pc, #56]	; ab40 <nand_blk_prg_era_fail+0x60>
    ab04:	e1a0e00f 	mov	lr, pc
    ab08:	e593f000 	ldr	pc, [r3]
    ab0c:	ea000007 	b	ab30 <nand_blk_prg_era_fail+0x50>
    ab10:	e2103004 	ands	r3, r0, #4	; 0x4
    ab14:	01a00003 	moveq	r0, r3
    ab18:	0a000005 	beq	ab34 <nand_blk_prg_era_fail+0x54>
    ab1c:	e1a01000 	mov	r1, r0
    ab20:	e59f3018 	ldr	r3, [pc, #24]	; ab40 <nand_blk_prg_era_fail+0x60>
    ab24:	e59f0018 	ldr	r0, [pc, #24]	; ab44 <nand_blk_prg_era_fail+0x64>
    ab28:	e1a0e00f 	mov	lr, pc
    ab2c:	e593f000 	ldr	pc, [r3]
    ab30:	e3a00001 	mov	r0, #1	; 0x1
    ab34:	e28dd004 	add	sp, sp, #4	; 0x4
    ab38:	e8bd8000 	pop	{pc}
    ab3c:	000139b0 	.word	0x000139b0
    ab40:	0800d76c 	.word	0x0800d76c
    ab44:	000139d4 	.word	0x000139d4

0000ab48 <nand_ecc_bits_check>:
    ab48:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ab4c:	e3a01010 	mov	r1, #16	; 0x10
    ab50:	e24dd004 	sub	sp, sp, #4	; 0x4
    ab54:	e3a0000f 	mov	r0, #15	; 0xf
    ab58:	ebfffebd 	bl	a654 <flash_wait_device_status>
    ab5c:	e2000030 	and	r0, r0, #48	; 0x30
    ab60:	e1a00220 	lsr	r0, r0, #4
    ab64:	e28dd004 	add	sp, sp, #4	; 0x4
    ab68:	e8bd8000 	pop	{pc}

0000ab6c <nand_ecc_enable>:
    ab6c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    ab70:	e59f40b4 	ldr	r4, [pc, #180]	; ac2c <nand_ecc_enable+0xc0>
    ab74:	e24dd064 	sub	sp, sp, #100	; 0x64
    ab78:	e594600c 	ldr	r6, [r4, #12]
    ab7c:	e3560001 	cmp	r6, #1	; 0x1
    ab80:	1a000026 	bne	ac20 <nand_ecc_enable+0xb4>
    ab84:	e5943040 	ldr	r3, [r4, #64]
    ab88:	e3530001 	cmp	r3, #1	; 0x1
    ab8c:	0a000006 	beq	abac <nand_ecc_enable+0x40>
    ab90:	e3a0000f 	mov	r0, #15	; 0xf
    ab94:	e3a01009 	mov	r1, #9	; 0x9
    ab98:	ebfffead 	bl	a654 <flash_wait_device_status>
    ab9c:	e2107010 	ands	r7, r0, #16	; 0x10
    aba0:	e1a05000 	mov	r5, r0
    aba4:	0a000002 	beq	abb4 <nand_ecc_enable+0x48>
    aba8:	e5846040 	str	r6, [r4, #64]
    abac:	e3a00000 	mov	r0, #0	; 0x0
    abb0:	ea00001b 	b	ac24 <nand_ecc_enable+0xb8>
    abb4:	e28d4004 	add	r4, sp, #4	; 0x4
    abb8:	e1a00004 	mov	r0, r4
    abbc:	ebfffd18 	bl	a024 <spi_transfer_init>
    abc0:	e3a0301f 	mov	r3, #31	; 0x1f
    abc4:	e5cd3004 	strb	r3, [sp, #4]
    abc8:	e3e0304f 	mvn	r3, #79	; 0x4f
    abcc:	e3852010 	orr	r2, r5, #16	; 0x10
    abd0:	e1a00004 	mov	r0, r4
    abd4:	e5cd3005 	strb	r3, [sp, #5]
    abd8:	e3a0400f 	mov	r4, #15	; 0xf
    abdc:	e2833053 	add	r3, r3, #83	; 0x53
    abe0:	e5cd2006 	strb	r2, [sp, #6]
    abe4:	e5cd3008 	strb	r3, [sp, #8]
    abe8:	e58d700c 	str	r7, [sp, #12]
    abec:	e5cd4017 	strb	r4, [sp, #23]
    abf0:	ebfffd3f 	bl	a0f4 <spi_read_write_data>
    abf4:	e1a00004 	mov	r0, r4
    abf8:	e3a01009 	mov	r1, #9	; 0x9
    abfc:	ebfffe94 	bl	a654 <flash_wait_device_status>
    ac00:	e3100010 	tst	r0, #16	; 0x10
    ac04:	0a000005 	beq	ac20 <nand_ecc_enable+0xb4>
    ac08:	e59f0020 	ldr	r0, [pc, #32]	; ac30 <nand_ecc_enable+0xc4>
    ac0c:	e59f3020 	ldr	r3, [pc, #32]	; ac34 <nand_ecc_enable+0xc8>
    ac10:	e1a0e00f 	mov	lr, pc
    ac14:	e593f000 	ldr	pc, [r3]
    ac18:	e1a00007 	mov	r0, r7
    ac1c:	ea000000 	b	ac24 <nand_ecc_enable+0xb8>
    ac20:	e3e00000 	mvn	r0, #0	; 0x0
    ac24:	e28dd064 	add	sp, sp, #100	; 0x64
    ac28:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    ac2c:	0800d258 	.word	0x0800d258
    ac30:	00013a0a 	.word	0x00013a0a
    ac34:	0800d76c 	.word	0x0800d76c

0000ac38 <flash_nand_vs_read>:
    ac38:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ac3c:	e59f4438 	ldr	r4, [pc, #1080]	; b07c <flash_nand_vs_read+0x444>
    ac40:	e24dd074 	sub	sp, sp, #116	; 0x74
    ac44:	e1a05000 	mov	r5, r0
    ac48:	e5941028 	ldr	r1, [r4, #40]
    ac4c:	e1a00002 	mov	r0, r2
    ac50:	eb0015ac 	bl	10308 <__aeabi_idiv>
    ac54:	e594305c 	ldr	r3, [r4, #92]
    ac58:	e1a07000 	mov	r7, r0
    ac5c:	e3530001 	cmp	r3, #1	; 0x1
    ac60:	e59d6098 	ldr	r6, [sp, #152]
    ac64:	e1a08000 	mov	r8, r0
    ac68:	e1a09fc8 	asr	r9, r8, #31
    ac6c:	0a000004 	beq	ac84 <flash_nand_vs_read+0x4c>
    ac70:	ebfffbc3 	bl	9b84 <ssp_manager_init>
    ac74:	e3a03064 	mov	r3, #100	; 0x64
    ac78:	e5843050 	str	r3, [r4, #80]
    ac7c:	e3a0300f 	mov	r3, #15	; 0xf
    ac80:	e5843000 	str	r3, [r4]
    ac84:	e59f33f4 	ldr	r3, [pc, #1012]	; b080 <flash_nand_vs_read+0x448>
    ac88:	e5933000 	ldr	r3, [r3]
    ac8c:	e3530000 	cmp	r3, #0	; 0x0
    ac90:	0a000007 	beq	acb4 <flash_nand_vs_read+0x7c>
    ac94:	e59f23e0 	ldr	r2, [pc, #992]	; b07c <flash_nand_vs_read+0x444>
    ac98:	e59f03e4 	ldr	r0, [pc, #996]	; b084 <flash_nand_vs_read+0x44c>
    ac9c:	e5923050 	ldr	r3, [r2, #80]
    aca0:	e5921000 	ldr	r1, [r2]
    aca4:	e59fc3dc 	ldr	ip, [pc, #988]	; b088 <flash_nand_vs_read+0x450>
    aca8:	e5922004 	ldr	r2, [r2, #4]
    acac:	e1a0e00f 	mov	lr, pc
    acb0:	e59cf000 	ldr	pc, [ip]
    acb4:	e59f33c4 	ldr	r3, [pc, #964]	; b080 <flash_nand_vs_read+0x448>
    acb8:	e59f23bc 	ldr	r2, [pc, #956]	; b07c <flash_nand_vs_read+0x444>
    acbc:	e5933000 	ldr	r3, [r3]
    acc0:	e5921044 	ldr	r1, [r2, #68]
    acc4:	e3530000 	cmp	r3, #0	; 0x0
    acc8:	e59f33bc 	ldr	r3, [pc, #956]	; b08c <flash_nand_vs_read+0x454>
    accc:	e5924000 	ldr	r4, [r2]
    acd0:	e5831000 	str	r1, [r3]
    acd4:	e5921004 	ldr	r1, [r2, #4]
    acd8:	0a00000e 	beq	ad18 <flash_nand_vs_read+0xe0>
    acdc:	e59f23ac 	ldr	r2, [pc, #940]	; b090 <flash_nand_vs_read+0x458>
    ace0:	e59f33ac 	ldr	r3, [pc, #940]	; b094 <flash_nand_vs_read+0x45c>
    ace4:	e3510000 	cmp	r1, #0	; 0x0
    ace8:	01a03002 	moveq	r3, r2
    acec:	e58d300c 	str	r3, [sp, #12]
    acf0:	e59f3390 	ldr	r3, [pc, #912]	; b088 <flash_nand_vs_read+0x450>
    acf4:	e58d1008 	str	r1, [sp, #8]
    acf8:	e593c000 	ldr	ip, [r3]
    acfc:	e59f0394 	ldr	r0, [pc, #916]	; b098 <flash_nand_vs_read+0x460>
    ad00:	e58d6000 	str	r6, [sp]
    ad04:	e58d4004 	str	r4, [sp, #4]
    ad08:	e59f138c 	ldr	r1, [pc, #908]	; b09c <flash_nand_vs_read+0x464>
    ad0c:	e1a02008 	mov	r2, r8
    ad10:	e1a03009 	mov	r3, r9
    ad14:	e12fff3c 	blx	ip
    ad18:	e28d0014 	add	r0, sp, #20	; 0x14
    ad1c:	ebfffcc0 	bl	a024 <spi_transfer_init>
    ad20:	e354000f 	cmp	r4, #15	; 0xf
    ad24:	159f0374 	ldrne	r0, [pc, #884]	; b0a0 <flash_nand_vs_read+0x468>
    ad28:	1a0000cc 	bne	b060 <flash_nand_vs_read+0x428>
    ad2c:	e59f334c 	ldr	r3, [pc, #844]	; b080 <flash_nand_vs_read+0x448>
    ad30:	e5933000 	ldr	r3, [r3]
    ad34:	e3530000 	cmp	r3, #0	; 0x0
    ad38:	0a000005 	beq	ad54 <flash_nand_vs_read+0x11c>
    ad3c:	e59f0360 	ldr	r0, [pc, #864]	; b0a4 <flash_nand_vs_read+0x46c>
    ad40:	e59f1354 	ldr	r1, [pc, #852]	; b09c <flash_nand_vs_read+0x464>
    ad44:	e3002381 	movw	r2, #897	; 0x381
    ad48:	e59f3338 	ldr	r3, [pc, #824]	; b088 <flash_nand_vs_read+0x450>
    ad4c:	e1a0e00f 	mov	lr, pc
    ad50:	e593f000 	ldr	pc, [r3]
    ad54:	e59f3324 	ldr	r3, [pc, #804]	; b080 <flash_nand_vs_read+0x448>
    ad58:	e3a02001 	mov	r2, #1	; 0x1
    ad5c:	e5933000 	ldr	r3, [r3]
    ad60:	e5cd2026 	strb	r2, [sp, #38]
    ad64:	e3530000 	cmp	r3, #0	; 0x0
    ad68:	e3a03003 	mov	r3, #3	; 0x3
    ad6c:	e5cd3014 	strb	r3, [sp, #20]
    ad70:	e3a03002 	mov	r3, #2	; 0x2
    ad74:	e58d301c 	str	r3, [sp, #28]
    ad78:	e3a03000 	mov	r3, #0	; 0x0
    ad7c:	e5cd3025 	strb	r3, [sp, #37]
    ad80:	e283300f 	add	r3, r3, #15	; 0xf
    ad84:	e5cd3027 	strb	r3, [sp, #39]
    ad88:	e5cd2018 	strb	r2, [sp, #24]
    ad8c:	0a000005 	beq	ada8 <flash_nand_vs_read+0x170>
    ad90:	e59f030c 	ldr	r0, [pc, #780]	; b0a4 <flash_nand_vs_read+0x46c>
    ad94:	e59f1300 	ldr	r1, [pc, #768]	; b09c <flash_nand_vs_read+0x464>
    ad98:	e3a02fe2 	mov	r2, #904	; 0x388
    ad9c:	e59f32e4 	ldr	r3, [pc, #740]	; b088 <flash_nand_vs_read+0x450>
    ada0:	e1a0e00f 	mov	lr, pc
    ada4:	e593f000 	ldr	pc, [r3]
    ada8:	e59f32d0 	ldr	r3, [pc, #720]	; b080 <flash_nand_vs_read+0x448>
    adac:	e58d5028 	str	r5, [sp, #40]
    adb0:	e5932000 	ldr	r2, [r3]
    adb4:	e59f32d0 	ldr	r3, [pc, #720]	; b08c <flash_nand_vs_read+0x454>
    adb8:	e3520000 	cmp	r2, #0	; 0x0
    adbc:	e5933000 	ldr	r3, [r3]
    adc0:	e58d803c 	str	r8, [sp, #60]
    adc4:	e58d3030 	str	r3, [sp, #48]
    adc8:	e58d302c 	str	r3, [sp, #44]
    adcc:	0a000005 	beq	ade8 <flash_nand_vs_read+0x1b0>
    add0:	e59f02cc 	ldr	r0, [pc, #716]	; b0a4 <flash_nand_vs_read+0x46c>
    add4:	e59f12c0 	ldr	r1, [pc, #704]	; b09c <flash_nand_vs_read+0x464>
    add8:	e3002395 	movw	r2, #917	; 0x395
    addc:	e59f32a4 	ldr	r3, [pc, #676]	; b088 <flash_nand_vs_read+0x450>
    ade0:	e1a0e00f 	mov	lr, pc
    ade4:	e593f000 	ldr	pc, [r3]
    ade8:	e59f329c 	ldr	r3, [pc, #668]	; b08c <flash_nand_vs_read+0x454>
    adec:	e1a00006 	mov	r0, r6
    adf0:	e593b000 	ldr	fp, [r3]
    adf4:	e1a0100b 	mov	r1, fp
    adf8:	eb001542 	bl	10308 <__aeabi_idiv>
    adfc:	e59f327c 	ldr	r3, [pc, #636]	; b080 <flash_nand_vs_read+0x448>
    ae00:	e1a0a000 	mov	sl, r0
    ae04:	e5933000 	ldr	r3, [r3]
    ae08:	e3530000 	cmp	r3, #0	; 0x0
    ae0c:	0a000004 	beq	ae24 <flash_nand_vs_read+0x1ec>
    ae10:	e59f0290 	ldr	r0, [pc, #656]	; b0a8 <flash_nand_vs_read+0x470>
    ae14:	e1a0100a 	mov	r1, sl
    ae18:	e59f3268 	ldr	r3, [pc, #616]	; b088 <flash_nand_vs_read+0x450>
    ae1c:	e1a0e00f 	mov	lr, pc
    ae20:	e593f000 	ldr	pc, [r3]
    ae24:	e3a05000 	mov	r5, #0	; 0x0
    ae28:	e1a09005 	mov	r9, r5
    ae2c:	e59f8248 	ldr	r8, [pc, #584]	; b07c <flash_nand_vs_read+0x444>
    ae30:	ea000047 	b	af54 <flash_nand_vs_read+0x31c>
    ae34:	e59d303c 	ldr	r3, [sp, #60]
    ae38:	e598200c 	ldr	r2, [r8, #12]
    ae3c:	e1a01823 	lsr	r1, r3, #16
    ae40:	e3520001 	cmp	r2, #1	; 0x1
    ae44:	e1a02423 	lsr	r2, r3, #8
    ae48:	e5cd2021 	strb	r2, [sp, #33]
    ae4c:	13a02004 	movne	r2, #4	; 0x4
    ae50:	e5cd1020 	strb	r1, [sp, #32]
    ae54:	e5cd3022 	strb	r3, [sp, #34]
    ae58:	e5cd9023 	strb	r9, [sp, #35]
    ae5c:	15cd2024 	strbne	r2, [sp, #36]
    ae60:	15cd9026 	strbne	r9, [sp, #38]
    ae64:	1a000014 	bne	aebc <flash_nand_vs_read+0x284>
    ae68:	e3a04000 	mov	r4, #0	; 0x0
    ae6c:	ebffff3e 	bl	ab6c <nand_ecc_enable>
    ae70:	e3500000 	cmp	r0, #0	; 0x0
    ae74:	e2844001 	add	r4, r4, #1	; 0x1
    ae78:	0a000001 	beq	ae84 <flash_nand_vs_read+0x24c>
    ae7c:	e354000a 	cmp	r4, #10	; 0xa
    ae80:	1afffff9 	bne	ae6c <flash_nand_vs_read+0x234>
    ae84:	e598303c 	ldr	r3, [r8, #60]
    ae88:	e3a02000 	mov	r2, #0	; 0x0
    ae8c:	e3530001 	cmp	r3, #1	; 0x1
    ae90:	02833007 	addeq	r3, r3, #7	; 0x7
    ae94:	e5cd9020 	strb	r9, [sp, #32]
    ae98:	e5cd2021 	strb	r2, [sp, #33]
    ae9c:	05cd3020 	strbeq	r3, [sp, #32]
    aea0:	e3a02002 	mov	r2, #2	; 0x2
    aea4:	e3a03001 	mov	r3, #1	; 0x1
    aea8:	e0850007 	add	r0, r5, r7
    aeac:	e3a0100f 	mov	r1, #15	; 0xf
    aeb0:	e5cd2024 	strb	r2, [sp, #36]
    aeb4:	e5cd3026 	strb	r3, [sp, #38]
    aeb8:	ebfffeed 	bl	aa74 <nand_read_page_to_cache>
    aebc:	e59f21bc 	ldr	r2, [pc, #444]	; b080 <flash_nand_vs_read+0x448>
    aec0:	e5923000 	ldr	r3, [r2]
    aec4:	e3530000 	cmp	r3, #0	; 0x0
    aec8:	0a000005 	beq	aee4 <flash_nand_vs_read+0x2ac>
    aecc:	e59f01d8 	ldr	r0, [pc, #472]	; b0ac <flash_nand_vs_read+0x474>
    aed0:	e59d102c 	ldr	r1, [sp, #44]
    aed4:	e59d203c 	ldr	r2, [sp, #60]
    aed8:	e59f31a8 	ldr	r3, [pc, #424]	; b088 <flash_nand_vs_read+0x450>
    aedc:	e1a0e00f 	mov	lr, pc
    aee0:	e593f000 	ldr	pc, [r3]
    aee4:	e59f2194 	ldr	r2, [pc, #404]	; b080 <flash_nand_vs_read+0x448>
    aee8:	e5923000 	ldr	r3, [r2]
    aeec:	e3530000 	cmp	r3, #0	; 0x0
    aef0:	0a000004 	beq	af08 <flash_nand_vs_read+0x2d0>
    aef4:	e59f01b4 	ldr	r0, [pc, #436]	; b0b0 <flash_nand_vs_read+0x478>
    aef8:	e1a01005 	mov	r1, r5
    aefc:	e59f3184 	ldr	r3, [pc, #388]	; b088 <flash_nand_vs_read+0x450>
    af00:	e1a0e00f 	mov	lr, pc
    af04:	e593f000 	ldr	pc, [r3]
    af08:	e5983028 	ldr	r3, [r8, #40]
    af0c:	e28d0014 	add	r0, sp, #20	; 0x14
    af10:	e58d3030 	str	r3, [sp, #48]
    af14:	e58d302c 	str	r3, [sp, #44]
    af18:	ebfffc75 	bl	a0f4 <spi_read_write_data>
    af1c:	e598300c 	ldr	r3, [r8, #12]
    af20:	e3530001 	cmp	r3, #1	; 0x1
    af24:	1a000002 	bne	af34 <flash_nand_vs_read+0x2fc>
    af28:	ebffff06 	bl	ab48 <nand_ecc_bits_check>
    af2c:	e3500002 	cmp	r0, #2	; 0x2
    af30:	0a000049 	beq	b05c <flash_nand_vs_read+0x424>
    af34:	e59f3150 	ldr	r3, [pc, #336]	; b08c <flash_nand_vs_read+0x454>
    af38:	e2855001 	add	r5, r5, #1	; 0x1
    af3c:	e5932000 	ldr	r2, [r3]
    af40:	e59d303c 	ldr	r3, [sp, #60]
    af44:	e58d2030 	str	r2, [sp, #48]
    af48:	e0833002 	add	r3, r3, r2
    af4c:	e58d303c 	str	r3, [sp, #60]
    af50:	e58d202c 	str	r2, [sp, #44]
    af54:	e155000a 	cmp	r5, sl
    af58:	3affffb5 	bcc	ae34 <flash_nand_vs_read+0x1fc>
    af5c:	e1a0100b 	mov	r1, fp
    af60:	e1a00006 	mov	r0, r6
    af64:	eb00156f 	bl	10528 <__aeabi_idivmod>
    af68:	e2518000 	subs	r8, r1, #0	; 0x0
    af6c:	0a00003f 	beq	b070 <flash_nand_vs_read+0x438>
    af70:	e59f3104 	ldr	r3, [pc, #260]	; b07c <flash_nand_vs_read+0x444>
    af74:	e59d203c 	ldr	r2, [sp, #60]
    af78:	e593300c 	ldr	r3, [r3, #12]
    af7c:	e3a00000 	mov	r0, #0	; 0x0
    af80:	e3530001 	cmp	r3, #1	; 0x1
    af84:	e1a03422 	lsr	r3, r2, #8
    af88:	e1a01822 	lsr	r1, r2, #16
    af8c:	e5cd3021 	strb	r3, [sp, #33]
    af90:	13a03004 	movne	r3, #4	; 0x4
    af94:	e5cd1020 	strb	r1, [sp, #32]
    af98:	e5cd2022 	strb	r2, [sp, #34]
    af9c:	e5cd0023 	strb	r0, [sp, #35]
    afa0:	15cd3024 	strbne	r3, [sp, #36]
    afa4:	15cd0026 	strbne	r0, [sp, #38]
    afa8:	1a000015 	bne	b004 <flash_nand_vs_read+0x3cc>
    afac:	e1a04000 	mov	r4, r0
    afb0:	ebfffeed 	bl	ab6c <nand_ecc_enable>
    afb4:	e3500000 	cmp	r0, #0	; 0x0
    afb8:	e2844001 	add	r4, r4, #1	; 0x1
    afbc:	0a000001 	beq	afc8 <flash_nand_vs_read+0x390>
    afc0:	e354000a 	cmp	r4, #10	; 0xa
    afc4:	1afffff9 	bne	afb0 <flash_nand_vs_read+0x378>
    afc8:	e59f20ac 	ldr	r2, [pc, #172]	; b07c <flash_nand_vs_read+0x444>
    afcc:	e3a03000 	mov	r3, #0	; 0x0
    afd0:	e592203c 	ldr	r2, [r2, #60]
    afd4:	e5cd3020 	strb	r3, [sp, #32]
    afd8:	e1520003 	cmp	r2, r3
    afdc:	e5cd3021 	strb	r3, [sp, #33]
    afe0:	12833008 	addne	r3, r3, #8	; 0x8
    afe4:	15cd3020 	strbne	r3, [sp, #32]
    afe8:	e3a03002 	mov	r3, #2	; 0x2
    afec:	e5cd3024 	strb	r3, [sp, #36]
    aff0:	e0850007 	add	r0, r5, r7
    aff4:	e3a03001 	mov	r3, #1	; 0x1
    aff8:	e3a0100f 	mov	r1, #15	; 0xf
    affc:	e5cd3026 	strb	r3, [sp, #38]
    b000:	ebfffe9b 	bl	aa74 <nand_read_page_to_cache>
    b004:	e59f3074 	ldr	r3, [pc, #116]	; b080 <flash_nand_vs_read+0x448>
    b008:	e5933000 	ldr	r3, [r3]
    b00c:	e3530000 	cmp	r3, #0	; 0x0
    b010:	0a000006 	beq	b030 <flash_nand_vs_read+0x3f8>
    b014:	e59f0098 	ldr	r0, [pc, #152]	; b0b4 <flash_nand_vs_read+0x47c>
    b018:	e59d102c 	ldr	r1, [sp, #44]
    b01c:	e59d203c 	ldr	r2, [sp, #60]
    b020:	e1dd32d7 	ldrsb	r3, [sp, #39]
    b024:	e59fc05c 	ldr	ip, [pc, #92]	; b088 <flash_nand_vs_read+0x450>
    b028:	e1a0e00f 	mov	lr, pc
    b02c:	e59cf000 	ldr	pc, [ip]
    b030:	e28d0014 	add	r0, sp, #20	; 0x14
    b034:	e58d8030 	str	r8, [sp, #48]
    b038:	e58d802c 	str	r8, [sp, #44]
    b03c:	ebfffc2c 	bl	a0f4 <spi_read_write_data>
    b040:	e59f3034 	ldr	r3, [pc, #52]	; b07c <flash_nand_vs_read+0x444>
    b044:	e593300c 	ldr	r3, [r3, #12]
    b048:	e3530001 	cmp	r3, #1	; 0x1
    b04c:	1a000007 	bne	b070 <flash_nand_vs_read+0x438>
    b050:	ebfffebc 	bl	ab48 <nand_ecc_bits_check>
    b054:	e3500002 	cmp	r0, #2	; 0x2
    b058:	1a000004 	bne	b070 <flash_nand_vs_read+0x438>
    b05c:	e59f0054 	ldr	r0, [pc, #84]	; b0b8 <flash_nand_vs_read+0x480>
    b060:	e59f3020 	ldr	r3, [pc, #32]	; b088 <flash_nand_vs_read+0x450>
    b064:	e1a0e00f 	mov	lr, pc
    b068:	e593f000 	ldr	pc, [r3]
    b06c:	e3e06000 	mvn	r6, #0	; 0x0
    b070:	e1a00006 	mov	r0, r6
    b074:	e28dd074 	add	sp, sp, #116	; 0x74
    b078:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b07c:	0800d258 	.word	0x0800d258
    b080:	0800d2b8 	.word	0x0800d2b8
    b084:	00013a3d 	.word	0x00013a3d
    b088:	0800d76c 	.word	0x0800d76c
    b08c:	0800d134 	.word	0x0800d134
    b090:	00014001 	.word	0x00014001
    b094:	00013a6b 	.word	0x00013a6b
    b098:	00013a6f 	.word	0x00013a6f
    b09c:	0001183a 	.word	0x0001183a
    b0a0:	000134ec 	.word	0x000134ec
    b0a4:	00013ab8 	.word	0x00013ab8
    b0a8:	00013acc 	.word	0x00013acc
    b0ac:	00013ae7 	.word	0x00013ae7
    b0b0:	00013b17 	.word	0x00013b17
    b0b4:	00013b5f 	.word	0x00013b5f
    b0b8:	00013b41 	.word	0x00013b41

0000b0bc <flash_nand_vs_write>:
    b0bc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b0c0:	e59f444c 	ldr	r4, [pc, #1100]	; b514 <flash_nand_vs_write+0x458>
    b0c4:	e24dd074 	sub	sp, sp, #116	; 0x74
    b0c8:	e594305c 	ldr	r3, [r4, #92]
    b0cc:	e1a09000 	mov	r9, r0
    b0d0:	e3530001 	cmp	r3, #1	; 0x1
    b0d4:	e1a05002 	mov	r5, r2
    b0d8:	e59d8098 	ldr	r8, [sp, #152]
    b0dc:	0a000004 	beq	b0f4 <flash_nand_vs_write+0x38>
    b0e0:	ebfffaa7 	bl	9b84 <ssp_manager_init>
    b0e4:	e3a03064 	mov	r3, #100	; 0x64
    b0e8:	e5843050 	str	r3, [r4, #80]
    b0ec:	e3a0300f 	mov	r3, #15	; 0xf
    b0f0:	e5843000 	str	r3, [r4]
    b0f4:	e59f4418 	ldr	r4, [pc, #1048]	; b514 <flash_nand_vs_write+0x458>
    b0f8:	e1a00005 	mov	r0, r5
    b0fc:	e5941028 	ldr	r1, [r4, #40]
    b100:	eb001480 	bl	10308 <__aeabi_idiv>
    b104:	e59f340c 	ldr	r3, [pc, #1036]	; b518 <flash_nand_vs_write+0x45c>
    b108:	e5942044 	ldr	r2, [r4, #68]
    b10c:	e5941030 	ldr	r1, [r4, #48]
    b110:	e1a0b000 	mov	fp, r0
    b114:	e5832000 	str	r2, [r3]
    b118:	e28d0014 	add	r0, sp, #20	; 0x14
    b11c:	e58d100c 	str	r1, [sp, #12]
    b120:	ebfffbbf 	bl	a024 <spi_transfer_init>
    b124:	e59f33f0 	ldr	r3, [pc, #1008]	; b51c <flash_nand_vs_write+0x460>
    b128:	e1a0600b 	mov	r6, fp
    b12c:	e1a07fc6 	asr	r7, r6, #31
    b130:	e5933000 	ldr	r3, [r3]
    b134:	e58d603c 	str	r6, [sp, #60]
    b138:	e3530000 	cmp	r3, #0	; 0x0
    b13c:	e5945000 	ldr	r5, [r4]
    b140:	e1a0a006 	mov	sl, r6
    b144:	0a000003 	beq	b158 <flash_nand_vs_write+0x9c>
    b148:	e59f03d0 	ldr	r0, [pc, #976]	; b520 <flash_nand_vs_write+0x464>
    b14c:	e59f33d0 	ldr	r3, [pc, #976]	; b524 <flash_nand_vs_write+0x468>
    b150:	e1a0e00f 	mov	lr, pc
    b154:	e593f000 	ldr	pc, [r3]
    b158:	e59f33bc 	ldr	r3, [pc, #956]	; b51c <flash_nand_vs_write+0x460>
    b15c:	e5933000 	ldr	r3, [r3]
    b160:	e3530000 	cmp	r3, #0	; 0x0
    b164:	0a000007 	beq	b188 <flash_nand_vs_write+0xcc>
    b168:	e58d8000 	str	r8, [sp]
    b16c:	e1a02006 	mov	r2, r6
    b170:	e1a03007 	mov	r3, r7
    b174:	e59f03ac 	ldr	r0, [pc, #940]	; b528 <flash_nand_vs_write+0x46c>
    b178:	e59f13ac 	ldr	r1, [pc, #940]	; b52c <flash_nand_vs_write+0x470>
    b17c:	e59fc3a0 	ldr	ip, [pc, #928]	; b524 <flash_nand_vs_write+0x468>
    b180:	e1a0e00f 	mov	lr, pc
    b184:	e59cf000 	ldr	pc, [ip]
    b188:	e59f338c 	ldr	r3, [pc, #908]	; b51c <flash_nand_vs_write+0x460>
    b18c:	e5933000 	ldr	r3, [r3]
    b190:	e3530000 	cmp	r3, #0	; 0x0
    b194:	0a000006 	beq	b1b4 <flash_nand_vs_write+0xf8>
    b198:	e59f3374 	ldr	r3, [pc, #884]	; b514 <flash_nand_vs_write+0x458>
    b19c:	e59f038c 	ldr	r0, [pc, #908]	; b530 <flash_nand_vs_write+0x474>
    b1a0:	e593104c 	ldr	r1, [r3, #76]
    b1a4:	e1a02005 	mov	r2, r5
    b1a8:	e59f3374 	ldr	r3, [pc, #884]	; b524 <flash_nand_vs_write+0x468>
    b1ac:	e1a0e00f 	mov	lr, pc
    b1b0:	e593f000 	ldr	pc, [r3]
    b1b4:	e355000f 	cmp	r5, #15	; 0xf
    b1b8:	1a00001d 	bne	b234 <flash_nand_vs_write+0x178>
    b1bc:	e59f3354 	ldr	r3, [pc, #852]	; b518 <flash_nand_vs_write+0x45c>
    b1c0:	e3a04000 	mov	r4, #0	; 0x0
    b1c4:	e5936000 	ldr	r6, [r3]
    b1c8:	e3a03002 	mov	r3, #2	; 0x2
    b1cc:	e3a02003 	mov	r2, #3	; 0x3
    b1d0:	e5cd3014 	strb	r3, [sp, #20]
    b1d4:	e1a00008 	mov	r0, r8
    b1d8:	e3a03001 	mov	r3, #1	; 0x1
    b1dc:	e1a01006 	mov	r1, r6
    b1e0:	e5cd3018 	strb	r3, [sp, #24]
    b1e4:	e5cd2024 	strb	r2, [sp, #36]
    b1e8:	e58d201c 	str	r2, [sp, #28]
    b1ec:	e5cd5027 	strb	r5, [sp, #39]
    b1f0:	e5cd4026 	strb	r4, [sp, #38]
    b1f4:	eb001443 	bl	10308 <__aeabi_idiv>
    b1f8:	e59f331c 	ldr	r3, [pc, #796]	; b51c <flash_nand_vs_write+0x460>
    b1fc:	e1580006 	cmp	r8, r6
    b200:	b1a02008 	movlt	r2, r8
    b204:	a1a02006 	movge	r2, r6
    b208:	e5933000 	ldr	r3, [r3]
    b20c:	e58d0008 	str	r0, [sp, #8]
    b210:	e1530004 	cmp	r3, r4
    b214:	e58da03c 	str	sl, [sp, #60]
    b218:	e58d9034 	str	r9, [sp, #52]
    b21c:	e58d202c 	str	r2, [sp, #44]
    b220:	e58d4040 	str	r4, [sp, #64]
    b224:	e58d4028 	str	r4, [sp, #40]
    b228:	e58d2038 	str	r2, [sp, #56]
    b22c:	0a00000b 	beq	b260 <flash_nand_vs_write+0x1a4>
    b230:	ea000005 	b	b24c <flash_nand_vs_write+0x190>
    b234:	e59f02f8 	ldr	r0, [pc, #760]	; b534 <flash_nand_vs_write+0x478>
    b238:	e59f32e4 	ldr	r3, [pc, #740]	; b524 <flash_nand_vs_write+0x468>
    b23c:	e1a0e00f 	mov	lr, pc
    b240:	e593f000 	ldr	pc, [r3]
    b244:	e3e08000 	mvn	r8, #0	; 0x0
    b248:	ea0000ae 	b	b508 <flash_nand_vs_write+0x44c>
    b24c:	e59f02e4 	ldr	r0, [pc, #740]	; b538 <flash_nand_vs_write+0x47c>
    b250:	e59d1008 	ldr	r1, [sp, #8]
    b254:	e59f32c8 	ldr	r3, [pc, #712]	; b524 <flash_nand_vs_write+0x468>
    b258:	e1a0e00f 	mov	lr, pc
    b25c:	e593f000 	ldr	pc, [r3]
    b260:	e3a05000 	mov	r5, #0	; 0x0
    b264:	e59f92b0 	ldr	r9, [pc, #688]	; b51c <flash_nand_vs_write+0x460>
    b268:	e59fa2b4 	ldr	sl, [pc, #692]	; b524 <flash_nand_vs_write+0x468>
    b26c:	e1a07005 	mov	r7, r5
    b270:	ea000049 	b	b39c <flash_nand_vs_write+0x2e0>
    b274:	e5993000 	ldr	r3, [r9]
    b278:	e3530000 	cmp	r3, #0	; 0x0
    b27c:	0a000003 	beq	b290 <flash_nand_vs_write+0x1d4>
    b280:	e59f02b4 	ldr	r0, [pc, #692]	; b53c <flash_nand_vs_write+0x480>
    b284:	e1a01005 	mov	r1, r5
    b288:	e1a0e00f 	mov	lr, pc
    b28c:	e59af000 	ldr	pc, [sl]
    b290:	ebfffcc8 	bl	a5b8 <flash_write_enable>
    b294:	e3a01007 	mov	r1, #7	; 0x7
    b298:	e3a0000f 	mov	r0, #15	; 0xf
    b29c:	ebfffcec 	bl	a654 <flash_wait_device_status>
    b2a0:	e3a01006 	mov	r1, #6	; 0x6
    b2a4:	e3a0000f 	mov	r0, #15	; 0xf
    b2a8:	ebfffce9 	bl	a654 <flash_wait_device_status>
    b2ac:	e59f1260 	ldr	r1, [pc, #608]	; b514 <flash_nand_vs_write+0x458>
    b2b0:	e59d303c 	ldr	r3, [sp, #60]
    b2b4:	e591200c 	ldr	r2, [r1, #12]
    b2b8:	e1a01823 	lsr	r1, r3, #16
    b2bc:	e3520001 	cmp	r2, #1	; 0x1
    b2c0:	e1a02423 	lsr	r2, r3, #8
    b2c4:	e5cd2021 	strb	r2, [sp, #33]
    b2c8:	e3a02003 	mov	r2, #3	; 0x3
    b2cc:	e5cd1020 	strb	r1, [sp, #32]
    b2d0:	e5cd3022 	strb	r3, [sp, #34]
    b2d4:	e5cd7023 	strb	r7, [sp, #35]
    b2d8:	e5cd2024 	strb	r2, [sp, #36]
    b2dc:	1a000017 	bne	b340 <flash_nand_vs_write+0x284>
    b2e0:	e5993000 	ldr	r3, [r9]
    b2e4:	e3530000 	cmp	r3, #0	; 0x0
    b2e8:	0a000002 	beq	b2f8 <flash_nand_vs_write+0x23c>
    b2ec:	e59f024c 	ldr	r0, [pc, #588]	; b540 <flash_nand_vs_write+0x484>
    b2f0:	e1a0e00f 	mov	lr, pc
    b2f4:	e59af000 	ldr	pc, [sl]
    b2f8:	e3a04000 	mov	r4, #0	; 0x0
    b2fc:	ebfffe1a 	bl	ab6c <nand_ecc_enable>
    b300:	e3500000 	cmp	r0, #0	; 0x0
    b304:	e2844001 	add	r4, r4, #1	; 0x1
    b308:	0a000001 	beq	b314 <flash_nand_vs_write+0x258>
    b30c:	e354000a 	cmp	r4, #10	; 0xa
    b310:	1afffff9 	bne	b2fc <flash_nand_vs_write+0x240>
    b314:	e5993000 	ldr	r3, [r9]
    b318:	e3530000 	cmp	r3, #0	; 0x0
    b31c:	0a000002 	beq	b32c <flash_nand_vs_write+0x270>
    b320:	e59f021c 	ldr	r0, [pc, #540]	; b544 <flash_nand_vs_write+0x488>
    b324:	e1a0e00f 	mov	lr, pc
    b328:	e59af000 	ldr	pc, [sl]
    b32c:	e3a03002 	mov	r3, #2	; 0x2
    b330:	e5cd7020 	strb	r7, [sp, #32]
    b334:	e5cd7021 	strb	r7, [sp, #33]
    b338:	e5cd3024 	strb	r3, [sp, #36]
    b33c:	e5cd7026 	strb	r7, [sp, #38]
    b340:	e59f11d0 	ldr	r1, [pc, #464]	; b518 <flash_nand_vs_write+0x45c>
    b344:	e28d0014 	add	r0, sp, #20	; 0x14
    b348:	e5913000 	ldr	r3, [r1]
    b34c:	e58d302c 	str	r3, [sp, #44]
    b350:	e58d3038 	str	r3, [sp, #56]
    b354:	ebfffb66 	bl	a0f4 <spi_read_write_data>
    b358:	e59f31b8 	ldr	r3, [pc, #440]	; b518 <flash_nand_vs_write+0x45c>
    b35c:	e5932000 	ldr	r2, [r3]
    b360:	e59f31ac 	ldr	r3, [pc, #428]	; b514 <flash_nand_vs_write+0x458>
    b364:	e593100c 	ldr	r1, [r3, #12]
    b368:	e59d303c 	ldr	r3, [sp, #60]
    b36c:	e3510001 	cmp	r1, #1	; 0x1
    b370:	e0833002 	add	r3, r3, r2
    b374:	e58d303c 	str	r3, [sp, #60]
    b378:	1a000006 	bne	b398 <flash_nand_vs_write+0x2dc>
    b37c:	e085400b 	add	r4, r5, fp
    b380:	e281100e 	add	r1, r1, #14	; 0xe
    b384:	e1a00004 	mov	r0, r4
    b388:	ebfffd9c 	bl	aa00 <nand_write_cache_to_memory>
    b38c:	ebfffdd3 	bl	aae0 <nand_blk_prg_era_fail>
    b390:	e3500001 	cmp	r0, #1	; 0x1
    b394:	0a000051 	beq	b4e0 <flash_nand_vs_write+0x424>
    b398:	e2855001 	add	r5, r5, #1	; 0x1
    b39c:	e59d1008 	ldr	r1, [sp, #8]
    b3a0:	e1550001 	cmp	r5, r1
    b3a4:	3affffb2 	bcc	b274 <flash_nand_vs_write+0x1b8>
    b3a8:	e1a01006 	mov	r1, r6
    b3ac:	e1a00008 	mov	r0, r8
    b3b0:	eb00145c 	bl	10528 <__aeabi_idivmod>
    b3b4:	e2516000 	subs	r6, r1, #0	; 0x0
    b3b8:	0a000052 	beq	b508 <flash_nand_vs_write+0x44c>
    b3bc:	e59f3158 	ldr	r3, [pc, #344]	; b51c <flash_nand_vs_write+0x460>
    b3c0:	e5933000 	ldr	r3, [r3]
    b3c4:	e3530000 	cmp	r3, #0	; 0x0
    b3c8:	0a000003 	beq	b3dc <flash_nand_vs_write+0x320>
    b3cc:	e59f0174 	ldr	r0, [pc, #372]	; b548 <flash_nand_vs_write+0x48c>
    b3d0:	e59f314c 	ldr	r3, [pc, #332]	; b524 <flash_nand_vs_write+0x468>
    b3d4:	e1a0e00f 	mov	lr, pc
    b3d8:	e593f000 	ldr	pc, [r3]
    b3dc:	ebfffc75 	bl	a5b8 <flash_write_enable>
    b3e0:	e3a01007 	mov	r1, #7	; 0x7
    b3e4:	e3a0000f 	mov	r0, #15	; 0xf
    b3e8:	ebfffc99 	bl	a654 <flash_wait_device_status>
    b3ec:	e3a01006 	mov	r1, #6	; 0x6
    b3f0:	e3a0000f 	mov	r0, #15	; 0xf
    b3f4:	ebfffc96 	bl	a654 <flash_wait_device_status>
    b3f8:	e59f3114 	ldr	r3, [pc, #276]	; b514 <flash_nand_vs_write+0x458>
    b3fc:	e59d203c 	ldr	r2, [sp, #60]
    b400:	e593300c 	ldr	r3, [r3, #12]
    b404:	e1a01822 	lsr	r1, r2, #16
    b408:	e3530001 	cmp	r3, #1	; 0x1
    b40c:	e1a03422 	lsr	r3, r2, #8
    b410:	e5cd3021 	strb	r3, [sp, #33]
    b414:	e3a03000 	mov	r3, #0	; 0x0
    b418:	e5cd3023 	strb	r3, [sp, #35]
    b41c:	e2833003 	add	r3, r3, #3	; 0x3
    b420:	e5cd1020 	strb	r1, [sp, #32]
    b424:	e5cd2022 	strb	r2, [sp, #34]
    b428:	e5cd3024 	strb	r3, [sp, #36]
    b42c:	1a00001c 	bne	b4a4 <flash_nand_vs_write+0x3e8>
    b430:	e59f30e4 	ldr	r3, [pc, #228]	; b51c <flash_nand_vs_write+0x460>
    b434:	e5933000 	ldr	r3, [r3]
    b438:	e3530000 	cmp	r3, #0	; 0x0
    b43c:	0a000003 	beq	b450 <flash_nand_vs_write+0x394>
    b440:	e59f00f8 	ldr	r0, [pc, #248]	; b540 <flash_nand_vs_write+0x484>
    b444:	e59f30d8 	ldr	r3, [pc, #216]	; b524 <flash_nand_vs_write+0x468>
    b448:	e1a0e00f 	mov	lr, pc
    b44c:	e593f000 	ldr	pc, [r3]
    b450:	e3a04000 	mov	r4, #0	; 0x0
    b454:	ebfffdc4 	bl	ab6c <nand_ecc_enable>
    b458:	e3500000 	cmp	r0, #0	; 0x0
    b45c:	e2844001 	add	r4, r4, #1	; 0x1
    b460:	0a000001 	beq	b46c <flash_nand_vs_write+0x3b0>
    b464:	e354000a 	cmp	r4, #10	; 0xa
    b468:	1afffff9 	bne	b454 <flash_nand_vs_write+0x398>
    b46c:	e59f30a8 	ldr	r3, [pc, #168]	; b51c <flash_nand_vs_write+0x460>
    b470:	e5933000 	ldr	r3, [r3]
    b474:	e3530000 	cmp	r3, #0	; 0x0
    b478:	0a000003 	beq	b48c <flash_nand_vs_write+0x3d0>
    b47c:	e59f00c0 	ldr	r0, [pc, #192]	; b544 <flash_nand_vs_write+0x488>
    b480:	e59f309c 	ldr	r3, [pc, #156]	; b524 <flash_nand_vs_write+0x468>
    b484:	e1a0e00f 	mov	lr, pc
    b488:	e593f000 	ldr	pc, [r3]
    b48c:	e3a02000 	mov	r2, #0	; 0x0
    b490:	e3a03002 	mov	r3, #2	; 0x2
    b494:	e5cd3024 	strb	r3, [sp, #36]
    b498:	e5cd2026 	strb	r2, [sp, #38]
    b49c:	e5cd2020 	strb	r2, [sp, #32]
    b4a0:	e5cd2021 	strb	r2, [sp, #33]
    b4a4:	e28d0014 	add	r0, sp, #20	; 0x14
    b4a8:	e58d602c 	str	r6, [sp, #44]
    b4ac:	e58d6038 	str	r6, [sp, #56]
    b4b0:	ebfffb0f 	bl	a0f4 <spi_read_write_data>
    b4b4:	e59f3058 	ldr	r3, [pc, #88]	; b514 <flash_nand_vs_write+0x458>
    b4b8:	e593300c 	ldr	r3, [r3, #12]
    b4bc:	e3530001 	cmp	r3, #1	; 0x1
    b4c0:	1a000010 	bne	b508 <flash_nand_vs_write+0x44c>
    b4c4:	e085400b 	add	r4, r5, fp
    b4c8:	e3a0100f 	mov	r1, #15	; 0xf
    b4cc:	e1a00004 	mov	r0, r4
    b4d0:	ebfffd4a 	bl	aa00 <nand_write_cache_to_memory>
    b4d4:	ebfffd81 	bl	aae0 <nand_blk_prg_era_fail>
    b4d8:	e3500001 	cmp	r0, #1	; 0x1
    b4dc:	1a000009 	bne	b508 <flash_nand_vs_write+0x44c>
    b4e0:	e59d100c 	ldr	r1, [sp, #12]
    b4e4:	e1a00004 	mov	r0, r4
    b4e8:	eb001386 	bl	10308 <__aeabi_idiv>
    b4ec:	e1a02005 	mov	r2, r5
    b4f0:	e1a01000 	mov	r1, r0
    b4f4:	e59f3028 	ldr	r3, [pc, #40]	; b524 <flash_nand_vs_write+0x468>
    b4f8:	e59f004c 	ldr	r0, [pc, #76]	; b54c <flash_nand_vs_write+0x490>
    b4fc:	e1a0e00f 	mov	lr, pc
    b500:	e593f000 	ldr	pc, [r3]
    b504:	e3e08002 	mvn	r8, #2	; 0x2
    b508:	e1a00008 	mov	r0, r8
    b50c:	e28dd074 	add	sp, sp, #116	; 0x74
    b510:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b514:	0800d258 	.word	0x0800d258
    b518:	0800d134 	.word	0x0800d134
    b51c:	0800d2b8 	.word	0x0800d2b8
    b520:	00012472 	.word	0x00012472
    b524:	0800d76c 	.word	0x0800d76c
    b528:	00013b9e 	.word	0x00013b9e
    b52c:	00011826 	.word	0x00011826
    b530:	00013bcd 	.word	0x00013bcd
    b534:	00013bf0 	.word	0x00013bf0
    b538:	00013c13 	.word	0x00013c13
    b53c:	00013c2f 	.word	0x00013c2f
    b540:	00013c51 	.word	0x00013c51
    b544:	00013c7f 	.word	0x00013c7f
    b548:	00013cd9 	.word	0x00013cd9
    b54c:	00013cac 	.word	0x00013cac

0000b550 <flash_nand_vs_erase>:
    b550:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b554:	e59f418c 	ldr	r4, [pc, #396]	; b6e8 <flash_nand_vs_erase+0x198>
    b558:	e24dd064 	sub	sp, sp, #100	; 0x64
    b55c:	e594305c 	ldr	r3, [r4, #92]
    b560:	e1a06000 	mov	r6, r0
    b564:	e3530001 	cmp	r3, #1	; 0x1
    b568:	e1a07001 	mov	r7, r1
    b56c:	e1a05002 	mov	r5, r2
    b570:	0a000004 	beq	b588 <flash_nand_vs_erase+0x38>
    b574:	ebfff982 	bl	9b84 <ssp_manager_init>
    b578:	e3a03064 	mov	r3, #100	; 0x64
    b57c:	e5843050 	str	r3, [r4, #80]
    b580:	e3a0300f 	mov	r3, #15	; 0xf
    b584:	e5843000 	str	r3, [r4]
    b588:	e59f8158 	ldr	r8, [pc, #344]	; b6e8 <flash_nand_vs_erase+0x198>
    b58c:	e5984000 	ldr	r4, [r8]
    b590:	e354000f 	cmp	r4, #15	; 0xf
    b594:	0a000005 	beq	b5b0 <flash_nand_vs_erase+0x60>
    b598:	e59f014c 	ldr	r0, [pc, #332]	; b6ec <flash_nand_vs_erase+0x19c>
    b59c:	e59f314c 	ldr	r3, [pc, #332]	; b6f0 <flash_nand_vs_erase+0x1a0>
    b5a0:	e1a0e00f 	mov	lr, pc
    b5a4:	e593f000 	ldr	pc, [r3]
    b5a8:	e3e00000 	mvn	r0, #0	; 0x0
    b5ac:	ea00004b 	b	b6e0 <flash_nand_vs_erase+0x190>
    b5b0:	e59f213c 	ldr	r2, [pc, #316]	; b6f4 <flash_nand_vs_erase+0x1a4>
    b5b4:	e3a03000 	mov	r3, #0	; 0x0
    b5b8:	e0922006 	adds	r2, r2, r6
    b5bc:	e0a33007 	adc	r3, r3, r7
    b5c0:	e0922005 	adds	r2, r2, r5
    b5c4:	e2a33000 	adc	r3, r3, #0	; 0x0
    b5c8:	e1a018a2 	lsr	r1, r2, #17
    b5cc:	e181a783 	orr	sl, r1, r3, lsl #15
    b5d0:	ebfffbf8 	bl	a5b8 <flash_write_enable>
    b5d4:	e1a00004 	mov	r0, r4
    b5d8:	e3a01007 	mov	r1, #7	; 0x7
    b5dc:	ebfffc1c 	bl	a654 <flash_wait_device_status>
    b5e0:	e3a01006 	mov	r1, #6	; 0x6
    b5e4:	e1a00004 	mov	r0, r4
    b5e8:	ebfffc19 	bl	a654 <flash_wait_device_status>
    b5ec:	e28d0004 	add	r0, sp, #4	; 0x4
    b5f0:	ebfffa8b 	bl	a024 <spi_transfer_init>
    b5f4:	e5983000 	ldr	r3, [r8]
    b5f8:	e3a04000 	mov	r4, #0	; 0x0
    b5fc:	e353000f 	cmp	r3, #15	; 0xf
    b600:	e1a058a6 	lsr	r5, r6, #17
    b604:	05cd3017 	strbeq	r3, [sp, #23]
    b608:	e1855787 	orr	r5, r5, r7, lsl #15
    b60c:	e3e03027 	mvn	r3, #39	; 0x27
    b610:	e3a0001f 	mov	r0, #31	; 0x1f
    b614:	e3a010a0 	mov	r1, #160	; 0xa0
    b618:	e1a02004 	mov	r2, r4
    b61c:	e5cd3004 	strb	r3, [sp, #4]
    b620:	e58d400c 	str	r4, [sp, #12]
    b624:	e5cd4005 	strb	r4, [sp, #5]
    b628:	e1a06005 	mov	r6, r5
    b62c:	ebfffbc8 	bl	a554 <flash_set_device_status>
    b630:	e59fb0b0 	ldr	fp, [pc, #176]	; b6e8 <flash_nand_vs_erase+0x198>
    b634:	e28d7004 	add	r7, sp, #4	; 0x4
    b638:	e3a09004 	mov	r9, #4	; 0x4
    b63c:	e3a08019 	mov	r8, #25	; 0x19
    b640:	ea00001d 	b	b6bc <flash_nand_vs_erase+0x16c>
    b644:	ebfffbdb 	bl	a5b8 <flash_write_enable>
    b648:	e3a01007 	mov	r1, #7	; 0x7
    b64c:	e3a0000f 	mov	r0, #15	; 0xf
    b650:	ebfffbff 	bl	a654 <flash_wait_device_status>
    b654:	e3a01006 	mov	r1, #6	; 0x6
    b658:	e3a0000f 	mov	r0, #15	; 0xf
    b65c:	ebfffbfc 	bl	a654 <flash_wait_device_status>
    b660:	e59b3030 	ldr	r3, [fp, #48]
    b664:	e1a00007 	mov	r0, r7
    b668:	e0030693 	mul	r3, r3, r6
    b66c:	e1a02403 	lsl	r2, r3, #8
    b670:	e2033cff 	and	r3, r3, #65280	; 0xff00
    b674:	e1822423 	orr	r2, r2, r3, lsr #8
    b678:	e1c720b2 	strh	r2, [r7, #2]
    b67c:	e2866001 	add	r6, r6, #1	; 0x1
    b680:	e5cd9008 	strb	r9, [sp, #8]
    b684:	ebfffa9a 	bl	a0f4 <spi_read_write_data>
    b688:	e3a01006 	mov	r1, #6	; 0x6
    b68c:	e3a0000f 	mov	r0, #15	; 0xf
    b690:	ebfffbef 	bl	a654 <flash_wait_device_status>
    b694:	ebfffd11 	bl	aae0 <nand_blk_prg_era_fail>
    b698:	e3560000 	cmp	r6, #0	; 0x0
    b69c:	e28610ff 	add	r1, r6, #255	; 0xff
    b6a0:	a1a01006 	movge	r1, r6
    b6a4:	e1a01441 	asr	r1, r1, #8
    b6a8:	e0010198 	mul	r1, r8, r1
    b6ac:	e59f0044 	ldr	r0, [pc, #68]	; b6f8 <flash_nand_vs_erase+0x1a8>
    b6b0:	e59f3038 	ldr	r3, [pc, #56]	; b6f0 <flash_nand_vs_erase+0x1a0>
    b6b4:	e1a0e00f 	mov	lr, pc
    b6b8:	e593f000 	ldr	pc, [r3]
    b6bc:	e0843005 	add	r3, r4, r5
    b6c0:	e153000a 	cmp	r3, sl
    b6c4:	e2844001 	add	r4, r4, #1	; 0x1
    b6c8:	baffffdd 	blt	b644 <flash_nand_vs_erase+0xf4>
    b6cc:	e59f0028 	ldr	r0, [pc, #40]	; b6fc <flash_nand_vs_erase+0x1ac>
    b6d0:	e59f3018 	ldr	r3, [pc, #24]	; b6f0 <flash_nand_vs_erase+0x1a0>
    b6d4:	e1a0e00f 	mov	lr, pc
    b6d8:	e593f000 	ldr	pc, [r3]
    b6dc:	e3a00000 	mov	r0, #0	; 0x0
    b6e0:	e28dd064 	add	sp, sp, #100	; 0x64
    b6e4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b6e8:	0800d258 	.word	0x0800d258
    b6ec:	00013ced 	.word	0x00013ced
    b6f0:	0800d76c 	.word	0x0800d76c
    b6f4:	0001ffff 	.word	0x0001ffff
    b6f8:	00013d15 	.word	0x00013d15
    b6fc:	00013d29 	.word	0x00013d29

0000b700 <spi_module_init>:
    b700:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    b704:	e2504000 	subs	r4, r0, #0	; 0x0
    b708:	e24dd00c 	sub	sp, sp, #12	; 0xc
    b70c:	1a000005 	bne	b728 <spi_module_init+0x28>
    b710:	e59f0168 	ldr	r0, [pc, #360]	; b880 <spi_module_init+0x180>
    b714:	e59f3168 	ldr	r3, [pc, #360]	; b884 <spi_module_init+0x184>
    b718:	e1a0e00f 	mov	lr, pc
    b71c:	e593f000 	ldr	pc, [r3]
    b720:	e3e00001 	mvn	r0, #1	; 0x1
    b724:	ea000053 	b	b878 <spi_module_init+0x178>
    b728:	e59f0158 	ldr	r0, [pc, #344]	; b888 <spi_module_init+0x188>
    b72c:	ebffd629 	bl	fd8 <module_get_clock>
    b730:	e2508000 	subs	r8, r0, #0	; 0x0
    b734:	a3a07002 	movge	r7, #2	; 0x2
    b738:	a59fa14c 	ldrge	sl, [pc, #332]	; b88c <spi_module_init+0x18c>
    b73c:	aa000013 	bge	b790 <spi_module_init+0x90>
    b740:	e59f0148 	ldr	r0, [pc, #328]	; b890 <spi_module_init+0x190>
    b744:	e1a01004 	mov	r1, r4
    b748:	e59f3134 	ldr	r3, [pc, #308]	; b884 <spi_module_init+0x184>
    b74c:	e1a0e00f 	mov	lr, pc
    b750:	e593f000 	ldr	pc, [r3]
    b754:	e3e00000 	mvn	r0, #0	; 0x0
    b758:	ea000046 	b	b878 <spi_module_init+0x178>
    b75c:	e1a05004 	mov	r5, r4
    b760:	e1a01006 	mov	r1, r6
    b764:	e1a00008 	mov	r0, r8
    b768:	eb0012e6 	bl	10308 <__aeabi_idiv>
    b76c:	e150000a 	cmp	r0, sl
    b770:	e0866007 	add	r6, r6, r7
    b774:	e2854001 	add	r4, r5, #1	; 0x1
    b778:	da000007 	ble	b79c <spi_module_init+0x9c>
    b77c:	e3540c01 	cmp	r4, #256	; 0x100
    b780:	1afffff5 	bne	b75c <spi_module_init+0x5c>
    b784:	e2877002 	add	r7, r7, #2	; 0x2
    b788:	e3570c01 	cmp	r7, #256	; 0x100
    b78c:	0a000003 	beq	b7a0 <spi_module_init+0xa0>
    b790:	e1a06007 	mov	r6, r7
    b794:	e3a05000 	mov	r5, #0	; 0x0
    b798:	eafffff0 	b	b760 <spi_module_init+0x60>
    b79c:	e1a04005 	mov	r4, r5
    b7a0:	e35700fe 	cmp	r7, #254	; 0xfe
    b7a4:	c35400ff 	cmpgt	r4, #255	; 0xff
    b7a8:	da00000a 	ble	b7d8 <spi_module_init+0xd8>
    b7ac:	e3a01002 	mov	r1, #2	; 0x2
    b7b0:	e3a02027 	mov	r2, #39	; 0x27
    b7b4:	e59f00d8 	ldr	r0, [pc, #216]	; b894 <spi_module_init+0x194>
    b7b8:	e59f30c4 	ldr	r3, [pc, #196]	; b884 <spi_module_init+0x184>
    b7bc:	e1a0e00f 	mov	lr, pc
    b7c0:	e593f000 	ldr	pc, [r3]
    b7c4:	e1a00008 	mov	r0, r8
    b7c8:	e3a01050 	mov	r1, #80	; 0x50
    b7cc:	eb0012cd 	bl	10308 <__aeabi_idiv>
    b7d0:	e3a07002 	mov	r7, #2	; 0x2
    b7d4:	e3a04027 	mov	r4, #39	; 0x27
    b7d8:	e59f50a4 	ldr	r5, [pc, #164]	; b884 <spi_module_init+0x184>
    b7dc:	e1a02007 	mov	r2, r7
    b7e0:	e1a03004 	mov	r3, r4
    b7e4:	e58d0000 	str	r0, [sp]
    b7e8:	e1a01008 	mov	r1, r8
    b7ec:	e59f00a4 	ldr	r0, [pc, #164]	; b898 <spi_module_init+0x198>
    b7f0:	e1a0e00f 	mov	lr, pc
    b7f4:	e595f000 	ldr	pc, [r5]
    b7f8:	e1a03404 	lsl	r3, r4, #8
    b7fc:	e59f4098 	ldr	r4, [pc, #152]	; b89c <spi_module_init+0x19c>
    b800:	e3a06000 	mov	r6, #0	; 0x0
    b804:	e38330c7 	orr	r3, r3, #199	; 0xc7
    b808:	e5847010 	str	r7, [r4, #16]
    b80c:	e5846014 	str	r6, [r4, #20]
    b810:	e5846000 	str	r6, [r4]
    b814:	e5846004 	str	r6, [r4, #4]
    b818:	e5843000 	str	r3, [r4]
    b81c:	e3a0300a 	mov	r3, #10	; 0xa
    b820:	e5843004 	str	r3, [r4, #4]
    b824:	e5941000 	ldr	r1, [r4]
    b828:	e59f0070 	ldr	r0, [pc, #112]	; b8a0 <spi_module_init+0x1a0>
    b82c:	e1a0e00f 	mov	lr, pc
    b830:	e595f000 	ldr	pc, [r5]
    b834:	e5941004 	ldr	r1, [r4, #4]
    b838:	e59f0064 	ldr	r0, [pc, #100]	; b8a4 <spi_module_init+0x1a4>
    b83c:	e1a0e00f 	mov	lr, pc
    b840:	e595f000 	ldr	pc, [r5]
    b844:	e59f005c 	ldr	r0, [pc, #92]	; b8a8 <spi_module_init+0x1a8>
    b848:	e1a0e00f 	mov	lr, pc
    b84c:	e595f000 	ldr	pc, [r5]
    b850:	e59f0054 	ldr	r0, [pc, #84]	; b8ac <spi_module_init+0x1ac>
    b854:	e1a0e00f 	mov	lr, pc
    b858:	e595f000 	ldr	pc, [r5]
    b85c:	e59f004c 	ldr	r0, [pc, #76]	; b8b0 <spi_module_init+0x1b0>
    b860:	e1a0e00f 	mov	lr, pc
    b864:	e595f000 	ldr	pc, [r5]
    b868:	e59f0044 	ldr	r0, [pc, #68]	; b8b4 <spi_module_init+0x1b4>
    b86c:	e1a0e00f 	mov	lr, pc
    b870:	e595f000 	ldr	pc, [r5]
    b874:	e1a00006 	mov	r0, r6
    b878:	e28dd00c 	add	sp, sp, #12	; 0xc
    b87c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    b880:	00013d31 	.word	0x00013d31
    b884:	0800d76c 	.word	0x0800d76c
    b888:	2d00a2a0 	.word	0x2d00a2a0
    b88c:	004c4b40 	.word	0x004c4b40
    b890:	00013d47 	.word	0x00013d47
    b894:	00013d7b 	.word	0x00013d7b
    b898:	00013dd5 	.word	0x00013dd5
    b89c:	20500000 	.word	0x20500000
    b8a0:	00013e16 	.word	0x00013e16
    b8a4:	00013e3a 	.word	0x00013e3a
    b8a8:	00013e5e 	.word	0x00013e5e
    b8ac:	00013e83 	.word	0x00013e83
    b8b0:	00013ea3 	.word	0x00013ea3
    b8b4:	00013ed1 	.word	0x00013ed1

0000b8b8 <flash_nand_vs_reset>:
    b8b8:	e92d4030 	push	{r4, r5, lr}
    b8bc:	e59f4120 	ldr	r4, [pc, #288]	; b9e4 <flash_nand_vs_reset+0x12c>
    b8c0:	e24dd004 	sub	sp, sp, #4	; 0x4
    b8c4:	e59f511c 	ldr	r5, [pc, #284]	; b9e8 <flash_nand_vs_reset+0x130>
    b8c8:	e59f011c 	ldr	r0, [pc, #284]	; b9ec <flash_nand_vs_reset+0x134>
    b8cc:	e1a0e00f 	mov	lr, pc
    b8d0:	e595f000 	ldr	pc, [r5]
    b8d4:	e594305c 	ldr	r3, [r4, #92]
    b8d8:	e3530001 	cmp	r3, #1	; 0x1
    b8dc:	0a000007 	beq	b900 <flash_nand_vs_reset+0x48>
    b8e0:	ebfff8a7 	bl	9b84 <ssp_manager_init>
    b8e4:	e3a03064 	mov	r3, #100	; 0x64
    b8e8:	e5843050 	str	r3, [r4, #80]
    b8ec:	e3a0300f 	mov	r3, #15	; 0xf
    b8f0:	e5843000 	str	r3, [r4]
    b8f4:	e59f00f4 	ldr	r0, [pc, #244]	; b9f0 <flash_nand_vs_reset+0x138>
    b8f8:	e1a0e00f 	mov	lr, pc
    b8fc:	e595f000 	ldr	pc, [r5]
    b900:	e59f30ec 	ldr	r3, [pc, #236]	; b9f4 <flash_nand_vs_reset+0x13c>
    b904:	e5933000 	ldr	r3, [r3]
    b908:	e3530000 	cmp	r3, #0	; 0x0
    b90c:	0a000007 	beq	b930 <flash_nand_vs_reset+0x78>
    b910:	e59f20cc 	ldr	r2, [pc, #204]	; b9e4 <flash_nand_vs_reset+0x12c>
    b914:	e59f00dc 	ldr	r0, [pc, #220]	; b9f8 <flash_nand_vs_reset+0x140>
    b918:	e5923050 	ldr	r3, [r2, #80]
    b91c:	e5921000 	ldr	r1, [r2]
    b920:	e59fc0c0 	ldr	ip, [pc, #192]	; b9e8 <flash_nand_vs_reset+0x130>
    b924:	e5922004 	ldr	r2, [r2, #4]
    b928:	e1a0e00f 	mov	lr, pc
    b92c:	e59cf000 	ldr	pc, [ip]
    b930:	e59f30ac 	ldr	r3, [pc, #172]	; b9e4 <flash_nand_vs_reset+0x12c>
    b934:	e5933000 	ldr	r3, [r3]
    b938:	e353000f 	cmp	r3, #15	; 0xf
    b93c:	1a000020 	bne	b9c4 <flash_nand_vs_reset+0x10c>
    b940:	e59f00b4 	ldr	r0, [pc, #180]	; b9fc <flash_nand_vs_reset+0x144>
    b944:	ebffd373 	bl	718 <module_enable>
    b948:	e59f20b0 	ldr	r2, [pc, #176]	; ba00 <flash_nand_vs_reset+0x148>
    b94c:	e59f00b0 	ldr	r0, [pc, #176]	; ba04 <flash_nand_vs_reset+0x14c>
    b950:	e5923404 	ldr	r3, [r2, #1028]
    b954:	e3833008 	orr	r3, r3, #8	; 0x8
    b958:	e5823404 	str	r3, [r2, #1028]
    b95c:	ebffff67 	bl	b700 <spi_module_init>
    b960:	e59f20a0 	ldr	r2, [pc, #160]	; ba08 <flash_nand_vs_reset+0x150>
    b964:	e3a0000b 	mov	r0, #11	; 0xb
    b968:	e5923000 	ldr	r3, [r2]
    b96c:	e3833004 	orr	r3, r3, #4	; 0x4
    b970:	e5823000 	str	r3, [r2]
    b974:	ebffd3ae 	bl	834 <pads_init_module>
    b978:	e3a02001 	mov	r2, #1	; 0x1
    b97c:	e3a0004a 	mov	r0, #74	; 0x4a
    b980:	e3a01002 	mov	r1, #2	; 0x2
    b984:	ebffd47c 	bl	b7c <pads_chmod>
    b988:	e59f207c 	ldr	r2, [pc, #124]	; ba0c <flash_nand_vs_reset+0x154>
    b98c:	ea000000 	b	b994 <flash_nand_vs_reset+0xdc>
    b990:	e5923008 	ldr	r3, [r2, #8]
    b994:	e592300c 	ldr	r3, [r2, #12]
    b998:	e3130004 	tst	r3, #4	; 0x4
    b99c:	1afffffb 	bne	b990 <flash_nand_vs_reset+0xd8>
    b9a0:	e3a00002 	mov	r0, #2	; 0x2
    b9a4:	ebfff8d6 	bl	9d04 <spi_set_intr_recv_timeout>
    b9a8:	e3a00008 	mov	r0, #8	; 0x8
    b9ac:	ebfff940 	bl	9eb4 <spi_set_intr_transmit_fifo>
    b9b0:	e3a00004 	mov	r0, #4	; 0x4
    b9b4:	ebfff908 	bl	9ddc <spi_set_intr_recv_fifo>
    b9b8:	e3a00001 	mov	r0, #1	; 0x1
    b9bc:	ebfff89a 	bl	9c2c <spi_set_intr_recv_overrun>
    b9c0:	ea000003 	b	b9d4 <flash_nand_vs_reset+0x11c>
    b9c4:	e59f0044 	ldr	r0, [pc, #68]	; ba10 <flash_nand_vs_reset+0x158>
    b9c8:	e59f3018 	ldr	r3, [pc, #24]	; b9e8 <flash_nand_vs_reset+0x130>
    b9cc:	e1a0e00f 	mov	lr, pc
    b9d0:	e593f000 	ldr	pc, [r3]
    b9d4:	ebfffb0a 	bl	a604 <nand_flash_reset>
    b9d8:	e3a00000 	mov	r0, #0	; 0x0
    b9dc:	e28dd004 	add	sp, sp, #4	; 0x4
    b9e0:	e8bd8030 	pop	{r4, r5, pc}
    b9e4:	0800d258 	.word	0x0800d258
    b9e8:	0800d76c 	.word	0x0800d76c
    b9ec:	00013efc 	.word	0x00013efc
    b9f0:	00013f27 	.word	0x00013f27
    b9f4:	0800d2b8 	.word	0x0800d2b8
    b9f8:	00013a3d 	.word	0x00013a3d
    b9fc:	2d011400 	.word	0x2d011400
    ba00:	2d011000 	.word	0x2d011000
    ba04:	00013f5c 	.word	0x00013f5c
    ba08:	2d009000 	.word	0x2d009000
    ba0c:	20500000 	.word	0x20500000
    ba10:	00013bf0 	.word	0x00013bf0

0000ba14 <ram_addr2offs>:
    ba14:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    ba18:	93a02101 	movls	r2, #1073741824	; 0x40000000
    ba1c:	e3a01000 	mov	r1, #0	; 0x0
    ba20:	83a02103 	movhi	r2, #-1073741824	; 0xc0000000
    ba24:	93a03000 	movls	r3, #0	; 0x0
    ba28:	83e03000 	mvnhi	r3, #0	; 0x0
    ba2c:	e0922000 	adds	r2, r2, r0
    ba30:	e0a33001 	adc	r3, r3, r1
    ba34:	e1a01003 	mov	r1, r3
    ba38:	e1a00002 	mov	r0, r2
    ba3c:	e12fff1e 	bx	lr

0000ba40 <ram_offs2addr>:
    ba40:	e3510000 	cmp	r1, #0	; 0x0
    ba44:	ca000002 	bgt	ba54 <ram_offs2addr+0x14>
    ba48:	1a000003 	bne	ba5c <ram_offs2addr+0x1c>
    ba4c:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    ba50:	9a000001 	bls	ba5c <ram_offs2addr+0x1c>
    ba54:	e2800103 	add	r0, r0, #-1073741824	; 0xc0000000
    ba58:	e12fff1e 	bx	lr
    ba5c:	e2800101 	add	r0, r0, #1073741824	; 0x40000000
    ba60:	e12fff1e 	bx	lr

0000ba64 <ram_vs_align>:
    ba64:	e3a00c02 	mov	r0, #512	; 0x200
    ba68:	e12fff1e 	bx	lr

0000ba6c <ram_vs_reset>:
    ba6c:	e3a00000 	mov	r0, #0	; 0x0
    ba70:	e12fff1e 	bx	lr

0000ba74 <ram_vs_write>:
    ba74:	e92d4010 	push	{r4, lr}
    ba78:	e3530000 	cmp	r3, #0	; 0x0
    ba7c:	e1a01000 	mov	r1, r0
    ba80:	e59d4008 	ldr	r4, [sp, #8]
    ba84:	ca000002 	bgt	ba94 <ram_vs_write+0x20>
    ba88:	1a000003 	bne	ba9c <ram_vs_write+0x28>
    ba8c:	e3720107 	cmn	r2, #-1073741823	; 0xc0000001
    ba90:	9a000001 	bls	ba9c <ram_vs_write+0x28>
    ba94:	e2820103 	add	r0, r2, #-1073741824	; 0xc0000000
    ba98:	ea000000 	b	baa0 <ram_vs_write+0x2c>
    ba9c:	e2820101 	add	r0, r2, #1073741824	; 0x40000000
    baa0:	e1a02004 	mov	r2, r4
    baa4:	ebffd626 	bl	1344 <memcpy>
    baa8:	e1a00004 	mov	r0, r4
    baac:	e8bd8010 	pop	{r4, pc}

0000bab0 <ram_vs_read>:
    bab0:	e92d4010 	push	{r4, lr}
    bab4:	e3530000 	cmp	r3, #0	; 0x0
    bab8:	e59d4008 	ldr	r4, [sp, #8]
    babc:	ca000002 	bgt	bacc <ram_vs_read+0x1c>
    bac0:	1a000003 	bne	bad4 <ram_vs_read+0x24>
    bac4:	e3720107 	cmn	r2, #-1073741823	; 0xc0000001
    bac8:	9a000001 	bls	bad4 <ram_vs_read+0x24>
    bacc:	e2821103 	add	r1, r2, #-1073741824	; 0xc0000000
    bad0:	ea000000 	b	bad8 <ram_vs_read+0x28>
    bad4:	e2821101 	add	r1, r2, #1073741824	; 0x40000000
    bad8:	e1a02004 	mov	r2, r4
    badc:	ebffd618 	bl	1344 <memcpy>
    bae0:	e1a00004 	mov	r0, r4
    bae4:	e8bd8010 	pop	{r4, pc}

0000bae8 <udc_get_cable_state>:
    bae8:	e3a00001 	mov	r0, #1	; 0x1
    baec:	e12fff1e 	bx	lr

0000baf0 <udc_disconnect>:
    baf0:	e59f103c 	ldr	r1, [pc, #60]	; bb34 <udc_disconnect+0x44>
    baf4:	e59f303c 	ldr	r3, [pc, #60]	; bb38 <udc_disconnect+0x48>
    baf8:	e5912010 	ldr	r2, [r1, #16]
    bafc:	e3a00000 	mov	r0, #0	; 0x0
    bb00:	e3822001 	orr	r2, r2, #1	; 0x1
    bb04:	e92d4010 	push	{r4, lr}
    bb08:	e5c30000 	strb	r0, [r3]
    bb0c:	e1a04001 	mov	r4, r1
    bb10:	e5812010 	str	r2, [r1, #16]
    bb14:	ea000000 	b	bb1c <udc_disconnect+0x2c>
    bb18:	ebfff34c 	bl	8850 <udelay>
    bb1c:	e5943010 	ldr	r3, [r4, #16]
    bb20:	e3a0000a 	mov	r0, #10	; 0xa
    bb24:	e2133001 	ands	r3, r3, #1	; 0x1
    bb28:	1afffffa 	bne	bb18 <udc_disconnect+0x28>
    bb2c:	e1a00003 	mov	r0, r3
    bb30:	e8bd8010 	pop	{r4, pc}
    bb34:	27100000 	.word	0x27100000
    bb38:	0800d2c4 	.word	0x0800d2c4

0000bb3c <udc_event_handle>:
    bb3c:	e59f2768 	ldr	r2, [pc, #1896]	; c2ac <udc_event_handle+0x770>
    bb40:	e59f3768 	ldr	r3, [pc, #1896]	; c2b0 <udc_event_handle+0x774>
    bb44:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    bb48:	e5934014 	ldr	r4, [r3, #20]
    bb4c:	e5923000 	ldr	r3, [r2]
    bb50:	e59f0758 	ldr	r0, [pc, #1880]	; c2b0 <udc_event_handle+0x774>
    bb54:	e1530004 	cmp	r3, r4
    bb58:	e204ca03 	and	ip, r4, #12288	; 0x3000
    bb5c:	15824000 	strne	r4, [r2]
    bb60:	e2043b03 	and	r3, r4, #3072	; 0xc00
    bb64:	e35c0a01 	cmp	ip, #4096	; 0x1000
    bb68:	e24dd008 	sub	sp, sp, #8	; 0x8
    bb6c:	e5803014 	str	r3, [r0, #20]
    bb70:	0a000002 	beq	bb80 <udc_event_handle+0x44>
    bb74:	e35c0a02 	cmp	ip, #8192	; 0x2000
    bb78:	1a000034 	bne	bc50 <udc_event_handle+0x114>
    bb7c:	ea000025 	b	bc18 <udc_event_handle+0xdc>
    bb80:	e3a03302 	mov	r3, #134217728	; 0x8000000
    bb84:	e5803b00 	str	r3, [r0, #2816]
    bb88:	e5803b40 	str	r3, [r0, #2880]
    bb8c:	e59f3720 	ldr	r3, [pc, #1824]	; c2b4 <udc_event_handle+0x778>
    bb90:	e3a02009 	mov	r2, #9	; 0x9
    bb94:	e580381c 	str	r3, [r0, #2076]
    bb98:	e3a03b02 	mov	r3, #2048	; 0x800
    bb9c:	e5802814 	str	r2, [r0, #2068]
    bba0:	e5802810 	str	r2, [r0, #2064]
    bba4:	e5803024 	str	r3, [r0, #36]
    bba8:	e2833301 	add	r3, r3, #67108864	; 0x4000000
    bbac:	e5803028 	str	r3, [r0, #40]
    bbb0:	e2833b01 	add	r3, r3, #1024	; 0x400
    bbb4:	e5803104 	str	r3, [r0, #260]
    bbb8:	e59f36f8 	ldr	r3, [pc, #1784]	; c2b8 <udc_event_handle+0x77c>
    bbbc:	e1a05000 	mov	r5, r0
    bbc0:	e580305c 	str	r3, [r0, #92]
    bbc4:	e5903010 	ldr	r3, [r0, #16]
    bbc8:	e3c33d1f 	bic	r3, r3, #1984	; 0x7c0
    bbcc:	e3833e43 	orr	r3, r3, #1072	; 0x430
    bbd0:	e5803010 	str	r3, [r0, #16]
    bbd4:	ea000000 	b	bbdc <udc_event_handle+0xa0>
    bbd8:	ebfff31c 	bl	8850 <udelay>
    bbdc:	e5953010 	ldr	r3, [r5, #16]
    bbe0:	e3a00001 	mov	r0, #1	; 0x1
    bbe4:	e3130030 	tst	r3, #48	; 0x30
    bbe8:	e59f16c0 	ldr	r1, [pc, #1728]	; c2b0 <udc_event_handle+0x774>
    bbec:	1afffff9 	bne	bbd8 <udc_event_handle+0x9c>
    bbf0:	e59f36c4 	ldr	r3, [pc, #1732]	; c2bc <udc_event_handle+0x780>
    bbf4:	e2800009 	add	r0, r0, #9	; 0x9
    bbf8:	e5932024 	ldr	r2, [r3, #36]
    bbfc:	e59f36bc 	ldr	r3, [pc, #1724]	; c2c0 <udc_event_handle+0x784>
    bc00:	e5813b10 	str	r3, [r1, #2832]
    bc04:	e3a03a01 	mov	r3, #4096	; 0x1000
    bc08:	e5812b14 	str	r2, [r1, #2836]
    bc0c:	e5813014 	str	r3, [r1, #20]
    bc10:	ebfff30e 	bl	8850 <udelay>
    bc14:	ea00000d 	b	bc50 <udc_event_handle+0x114>
    bc18:	e5902808 	ldr	r2, [r0, #2056]
    bc1c:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bc20:	e5803b00 	str	r3, [r0, #2816]
    bc24:	e1a020a2 	lsr	r2, r2, #1
    bc28:	e59f368c 	ldr	r3, [pc, #1676]	; c2bc <udc_event_handle+0x780>
    bc2c:	e5901018 	ldr	r1, [r0, #24]
    bc30:	e2022003 	and	r2, r2, #3	; 0x3
    bc34:	e5c3201c 	strb	r2, [r3, #28]
    bc38:	e59f3684 	ldr	r3, [pc, #1668]	; c2c4 <udc_event_handle+0x788>
    bc3c:	e3811008 	orr	r1, r1, #8	; 0x8
    bc40:	e3a02001 	mov	r2, #1	; 0x1
    bc44:	e5801018 	str	r1, [r0, #24]
    bc48:	e5832000 	str	r2, [r3]
    bc4c:	e580c014 	str	ip, [r0, #20]
    bc50:	e59f3658 	ldr	r3, [pc, #1624]	; c2b0 <udc_event_handle+0x774>
    bc54:	e3140701 	tst	r4, #262144	; 0x40000
    bc58:	e5939818 	ldr	r9, [r3, #2072]
    bc5c:	0a000051 	beq	bda8 <udc_event_handle+0x26c>
    bc60:	e3190001 	tst	r9, #1	; 0x1
    bc64:	0a000042 	beq	bd74 <udc_event_handle+0x238>
    bc68:	e5935908 	ldr	r5, [r3, #2312]
    bc6c:	e3150001 	tst	r5, #1	; 0x1
    bc70:	0a00003b 	beq	bd64 <udc_event_handle+0x228>
    bc74:	e59f3640 	ldr	r3, [pc, #1600]	; c2bc <udc_event_handle+0x780>
    bc78:	e5933020 	ldr	r3, [r3, #32]
    bc7c:	e2433002 	sub	r3, r3, #2	; 0x2
    bc80:	e3530003 	cmp	r3, #3	; 0x3
    bc84:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    bc88:	ea000035 	b	bd64 <udc_event_handle+0x228>
    bc8c:	0000bccc 	.word	0x0000bccc
    bc90:	0000bc9c 	.word	0x0000bc9c
    bc94:	0000bd00 	.word	0x0000bd00
    bc98:	0000bd00 	.word	0x0000bd00
    bc9c:	e59f160c 	ldr	r1, [pc, #1548]	; c2b0 <udc_event_handle+0x774>
    bca0:	e59f0614 	ldr	r0, [pc, #1556]	; c2bc <udc_event_handle+0x780>
    bca4:	e5912910 	ldr	r2, [r1, #2320]
    bca8:	e590c00c 	ldr	ip, [r0, #12]
    bcac:	e3a03004 	mov	r3, #4	; 0x4
    bcb0:	e3822702 	orr	r2, r2, #524288	; 0x80000
    bcb4:	e5803020 	str	r3, [r0, #32]
    bcb8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bcbc:	e5812910 	str	r2, [r1, #2320]
    bcc0:	e581c914 	str	ip, [r1, #2324]
    bcc4:	e5813900 	str	r3, [r1, #2304]
    bcc8:	ea000025 	b	bd64 <udc_event_handle+0x228>
    bccc:	e59f15dc 	ldr	r1, [pc, #1500]	; c2b0 <udc_event_handle+0x774>
    bcd0:	e59f05e4 	ldr	r0, [pc, #1508]	; c2bc <udc_event_handle+0x780>
    bcd4:	e5913b10 	ldr	r3, [r1, #2832]
    bcd8:	e590c024 	ldr	ip, [r0, #36]
    bcdc:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    bce0:	e3833702 	orr	r3, r3, #524288	; 0x80000
    bce4:	e5813b10 	str	r3, [r1, #2832]
    bce8:	e3a02005 	mov	r2, #5	; 0x5
    bcec:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bcf0:	e581cb14 	str	ip, [r1, #2836]
    bcf4:	e5802020 	str	r2, [r0, #32]
    bcf8:	e5813b00 	str	r3, [r1, #2816]
    bcfc:	ea000018 	b	bd64 <udc_event_handle+0x228>
    bd00:	e59f05b4 	ldr	r0, [pc, #1460]	; c2bc <udc_event_handle+0x780>
    bd04:	e5d03015 	ldrb	r3, [r0, #21]
    bd08:	e3530009 	cmp	r3, #9	; 0x9
    bd0c:	1a00000a 	bne	bd3c <udc_event_handle+0x200>
    bd10:	e59f1598 	ldr	r1, [pc, #1432]	; c2b0 <udc_event_handle+0x774>
    bd14:	e59f35ac 	ldr	r3, [pc, #1452]	; c2c8 <udc_event_handle+0x78c>
    bd18:	e3a02001 	mov	r2, #1	; 0x1
    bd1c:	e5813920 	str	r3, [r1, #2336]
    bd20:	e2433d07 	sub	r3, r3, #448	; 0x1c0
    bd24:	e5813b40 	str	r3, [r1, #2880]
    bd28:	e591381c 	ldr	r3, [r1, #2076]
    bd2c:	e5c02000 	strb	r2, [r0]
    bd30:	e3833701 	orr	r3, r3, #262144	; 0x40000
    bd34:	e3833002 	orr	r3, r3, #2	; 0x2
    bd38:	e581381c 	str	r3, [r1, #2076]
    bd3c:	e59f256c 	ldr	r2, [pc, #1388]	; c2b0 <udc_event_handle+0x774>
    bd40:	e59f1574 	ldr	r1, [pc, #1396]	; c2bc <udc_event_handle+0x780>
    bd44:	e59f3574 	ldr	r3, [pc, #1396]	; c2c0 <udc_event_handle+0x784>
    bd48:	e5910024 	ldr	r0, [r1, #36]
    bd4c:	e5823b10 	str	r3, [r2, #2832]
    bd50:	e3a03001 	mov	r3, #1	; 0x1
    bd54:	e5813020 	str	r3, [r1, #32]
    bd58:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    bd5c:	e5820b14 	str	r0, [r2, #2836]
    bd60:	e5823b00 	str	r3, [r2, #2816]
    bd64:	e300320d 	movw	r3, #525	; 0x20d
    bd68:	e59f2540 	ldr	r2, [pc, #1344]	; c2b0 <udc_event_handle+0x774>
    bd6c:	e0053003 	and	r3, r5, r3
    bd70:	e5823908 	str	r3, [r2, #2312]
    bd74:	e3190002 	tst	r9, #2	; 0x2
    bd78:	0a00000a 	beq	bda8 <udc_event_handle+0x26c>
    bd7c:	e59f352c 	ldr	r3, [pc, #1324]	; c2b0 <udc_event_handle+0x774>
    bd80:	e5932928 	ldr	r2, [r3, #2344]
    bd84:	e3120001 	tst	r2, #1	; 0x1
    bd88:	0a000003 	beq	bd9c <udc_event_handle+0x260>
    bd8c:	e5933930 	ldr	r3, [r3, #2352]
    bd90:	e3d3120f 	bics	r1, r3, #-268435456	; 0xf0000000
    bd94:	059f3520 	ldreq	r3, [pc, #1312]	; c2bc <udc_event_handle+0x780>
    bd98:	05c31002 	strbeq	r1, [r3, #2]
    bd9c:	e59f350c 	ldr	r3, [pc, #1292]	; c2b0 <udc_event_handle+0x774>
    bda0:	e2022005 	and	r2, r2, #5	; 0x5
    bda4:	e5832928 	str	r2, [r3, #2344]
    bda8:	e3140702 	tst	r4, #524288	; 0x80000
    bdac:	0a00013c 	beq	c2a4 <udc_event_handle+0x768>
    bdb0:	e3190801 	tst	r9, #65536	; 0x10000
    bdb4:	0a000124 	beq	c24c <udc_event_handle+0x710>
    bdb8:	e59f34f0 	ldr	r3, [pc, #1264]	; c2b0 <udc_event_handle+0x774>
    bdbc:	e593ab08 	ldr	sl, [r3, #2824]
    bdc0:	e31a0001 	tst	sl, #1	; 0x1
    bdc4:	0a00002b 	beq	be78 <udc_event_handle+0x33c>
    bdc8:	e59f34ec 	ldr	r3, [pc, #1260]	; c2bc <udc_event_handle+0x780>
    bdcc:	e5933020 	ldr	r3, [r3, #32]
    bdd0:	e2433002 	sub	r3, r3, #2	; 0x2
    bdd4:	e3530003 	cmp	r3, #3	; 0x3
    bdd8:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    bddc:	ea000025 	b	be78 <udc_event_handle+0x33c>
    bde0:	0000be1c 	.word	0x0000be1c
    bde4:	0000bdf0 	.word	0x0000bdf0
    bde8:	0000be50 	.word	0x0000be50
    bdec:	0000be50 	.word	0x0000be50
    bdf0:	e59f14c4 	ldr	r1, [pc, #1220]	; c2bc <udc_event_handle+0x780>
    bdf4:	e59f24b4 	ldr	r2, [pc, #1204]	; c2b0 <udc_event_handle+0x774>
    bdf8:	e3a03004 	mov	r3, #4	; 0x4
    bdfc:	e591000c 	ldr	r0, [r1, #12]
    be00:	e5813020 	str	r3, [r1, #32]
    be04:	e3a03702 	mov	r3, #524288	; 0x80000
    be08:	e5823910 	str	r3, [r2, #2320]
    be0c:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    be10:	e5820914 	str	r0, [r2, #2324]
    be14:	e5823900 	str	r3, [r2, #2304]
    be18:	ea000016 	b	be78 <udc_event_handle+0x33c>
    be1c:	e59f148c 	ldr	r1, [pc, #1164]	; c2b0 <udc_event_handle+0x774>
    be20:	e59f0494 	ldr	r0, [pc, #1172]	; c2bc <udc_event_handle+0x780>
    be24:	e5913b10 	ldr	r3, [r1, #2832]
    be28:	e590c00c 	ldr	ip, [r0, #12]
    be2c:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    be30:	e3833702 	orr	r3, r3, #524288	; 0x80000
    be34:	e5813b10 	str	r3, [r1, #2832]
    be38:	e3a02005 	mov	r2, #5	; 0x5
    be3c:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    be40:	e581cb14 	str	ip, [r1, #2836]
    be44:	e5802020 	str	r2, [r0, #32]
    be48:	e5813b00 	str	r3, [r1, #2816]
    be4c:	ea000009 	b	be78 <udc_event_handle+0x33c>
    be50:	e59f2458 	ldr	r2, [pc, #1112]	; c2b0 <udc_event_handle+0x774>
    be54:	e59f1460 	ldr	r1, [pc, #1120]	; c2bc <udc_event_handle+0x780>
    be58:	e59f3460 	ldr	r3, [pc, #1120]	; c2c0 <udc_event_handle+0x784>
    be5c:	e5910024 	ldr	r0, [r1, #36]
    be60:	e5823b10 	str	r3, [r2, #2832]
    be64:	e3a03001 	mov	r3, #1	; 0x1
    be68:	e5813020 	str	r3, [r1, #32]
    be6c:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    be70:	e5820b14 	str	r0, [r2, #2836]
    be74:	e5823b00 	str	r3, [r2, #2816]
    be78:	e31a0004 	tst	sl, #4	; 0x4
    be7c:	159f342c 	ldrne	r3, [pc, #1068]	; c2b0 <udc_event_handle+0x774>
    be80:	13a02004 	movne	r2, #4	; 0x4
    be84:	15832b08 	strne	r2, [r3, #2824]
    be88:	e31a0008 	tst	sl, #8	; 0x8
    be8c:	0a0000ea 	beq	c23c <udc_event_handle+0x700>
    be90:	e31a0040 	tst	sl, #64	; 0x40
    be94:	e59f1430 	ldr	r1, [pc, #1072]	; c2cc <udc_event_handle+0x790>
    be98:	0a000006 	beq	beb8 <udc_event_handle+0x37c>
    be9c:	e59f340c 	ldr	r3, [pc, #1036]	; c2b0 <udc_event_handle+0x774>
    bea0:	e5933b14 	ldr	r3, [r3, #2836]
    bea4:	e2432004 	sub	r2, r3, #4	; 0x4
    bea8:	e2433008 	sub	r3, r3, #8	; 0x8
    beac:	e5812004 	str	r2, [r1, #4]
    beb0:	e5813000 	str	r3, [r1]
    beb4:	ea000005 	b	bed0 <udc_event_handle+0x394>
    beb8:	e59f33fc 	ldr	r3, [pc, #1020]	; c2bc <udc_event_handle+0x780>
    bebc:	e5932024 	ldr	r2, [r3, #36]
    bec0:	e5923000 	ldr	r3, [r2]
    bec4:	e5813000 	str	r3, [r1]
    bec8:	e5923004 	ldr	r3, [r2, #4]
    becc:	e5813004 	str	r3, [r1, #4]
    bed0:	e59f23e4 	ldr	r2, [pc, #996]	; c2bc <udc_event_handle+0x780>
    bed4:	e5d20014 	ldrb	r0, [r2, #20]
    bed8:	e3100080 	tst	r0, #128	; 0x80
    bedc:	13a03002 	movne	r3, #2	; 0x2
    bee0:	03a03003 	moveq	r3, #3	; 0x3
    bee4:	e5823020 	str	r3, [r2, #32]
    bee8:	e59f23cc 	ldr	r2, [pc, #972]	; c2bc <udc_event_handle+0x780>
    beec:	e1d211ba 	ldrh	r1, [r2, #26]
    bef0:	e3510000 	cmp	r1, #0	; 0x0
    bef4:	03a03004 	moveq	r3, #4	; 0x4
    bef8:	05823020 	streq	r3, [r2, #32]
    befc:	e59f33b8 	ldr	r3, [pc, #952]	; c2bc <udc_event_handle+0x780>
    bf00:	e5d33015 	ldrb	r3, [r3, #21]
    bf04:	e3530009 	cmp	r3, #9	; 0x9
    bf08:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    bf0c:	ea0000ca 	b	c23c <udc_event_handle+0x700>
    bf10:	0000c1d4 	.word	0x0000c1d4
    bf14:	0000c23c 	.word	0x0000c23c
    bf18:	0000c23c 	.word	0x0000c23c
    bf1c:	0000c23c 	.word	0x0000c23c
    bf20:	0000c23c 	.word	0x0000c23c
    bf24:	0000c120 	.word	0x0000c120
    bf28:	0000bf38 	.word	0x0000bf38
    bf2c:	0000c23c 	.word	0x0000c23c
    bf30:	0000c23c 	.word	0x0000c23c
    bf34:	0000c170 	.word	0x0000c170
    bf38:	e3a00040 	mov	r0, #64	; 0x40
    bf3c:	ebffd565 	bl	14d8 <malloc>
    bf40:	e2506000 	subs	r6, r0, #0	; 0x0
    bf44:	0a0000bc 	beq	c23c <udc_event_handle+0x700>
    bf48:	e59f536c 	ldr	r5, [pc, #876]	; c2bc <udc_event_handle+0x780>
    bf4c:	e1d521b6 	ldrh	r2, [r5, #22]
    bf50:	e1a03422 	lsr	r3, r2, #8
    bf54:	e2433001 	sub	r3, r3, #1	; 0x1
    bf58:	e3530005 	cmp	r3, #5	; 0x5
    bf5c:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    bf60:	ea000054 	b	c0b8 <udc_event_handle+0x57c>
    bf64:	0000bf7c 	.word	0x0000bf7c
    bf68:	0000bf94 	.word	0x0000bf94
    bf6c:	0000bfec 	.word	0x0000bfec
    bf70:	0000c0b8 	.word	0x0000c0b8
    bf74:	0000c0b8 	.word	0x0000c0b8
    bf78:	0000bfc0 	.word	0x0000bfc0
    bf7c:	e1a00006 	mov	r0, r6
    bf80:	e59f1348 	ldr	r1, [pc, #840]	; c2d0 <udc_event_handle+0x794>
    bf84:	e3a02012 	mov	r2, #18	; 0x12
    bf88:	ebffd4ed 	bl	1344 <memcpy>
    bf8c:	e3a03012 	mov	r3, #18	; 0x12
    bf90:	ea000028 	b	c038 <udc_event_handle+0x4fc>
    bf94:	e59f3338 	ldr	r3, [pc, #824]	; c2d4 <udc_event_handle+0x798>
    bf98:	e3a04020 	mov	r4, #32	; 0x20
    bf9c:	e3a0c000 	mov	ip, #0	; 0x0
    bfa0:	e1a01003 	mov	r1, r3
    bfa4:	e1a00006 	mov	r0, r6
    bfa8:	e1a02004 	mov	r2, r4
    bfac:	e5c3c003 	strb	ip, [r3, #3]
    bfb0:	e5c34002 	strb	r4, [r3, #2]
    bfb4:	ebffd4e2 	bl	1344 <memcpy>
    bfb8:	e5854004 	str	r4, [r5, #4]
    bfbc:	ea000045 	b	c0d8 <udc_event_handle+0x59c>
    bfc0:	e1d531ba 	ldrh	r3, [r5, #26]
    bfc4:	e59fc30c 	ldr	ip, [pc, #780]	; c2d8 <udc_event_handle+0x79c>
    bfc8:	e353000a 	cmp	r3, #10	; 0xa
    bfcc:	23a0300a 	movcs	r3, #10	; 0xa
    bfd0:	e5cc3000 	strb	r3, [ip]
    bfd4:	e1a0100c 	mov	r1, ip
    bfd8:	e1a00006 	mov	r0, r6
    bfdc:	e3a0200a 	mov	r2, #10	; 0xa
    bfe0:	ebffd4d7 	bl	1344 <memcpy>
    bfe4:	e3a0300a 	mov	r3, #10	; 0xa
    bfe8:	ea000012 	b	c038 <udc_event_handle+0x4fc>
    bfec:	e21270ff 	ands	r7, r2, #255	; 0xff
    bff0:	13a03000 	movne	r3, #0	; 0x0
    bff4:	03a03001 	moveq	r3, #1	; 0x1
    bff8:	e3570005 	cmp	r7, #5	; 0x5
    bffc:	d1a08003 	movle	r8, r3
    c000:	c3838001 	orrgt	r8, r3, #1	; 0x1
    c004:	e3580000 	cmp	r8, #0	; 0x0
    c008:	e59f52ac 	ldr	r5, [pc, #684]	; c2bc <udc_event_handle+0x780>
    c00c:	0a00000b 	beq	c040 <udc_event_handle+0x504>
    c010:	e1d531ba 	ldrh	r3, [r5, #26]
    c014:	e59fc2c0 	ldr	ip, [pc, #704]	; c2dc <udc_event_handle+0x7a0>
    c018:	e3530004 	cmp	r3, #4	; 0x4
    c01c:	23a03004 	movcs	r3, #4	; 0x4
    c020:	e5cc3000 	strb	r3, [ip]
    c024:	e1a0100c 	mov	r1, ip
    c028:	e1a00006 	mov	r0, r6
    c02c:	e3a02004 	mov	r2, #4	; 0x4
    c030:	ebffd4c3 	bl	1344 <memcpy>
    c034:	e3a03004 	mov	r3, #4	; 0x4
    c038:	e5853004 	str	r3, [r5, #4]
    c03c:	ea000025 	b	c0d8 <udc_event_handle+0x59c>
    c040:	e59f4298 	ldr	r4, [pc, #664]	; c2e0 <udc_event_handle+0x7a4>
    c044:	e7940107 	ldr	r0, [r4, r7, lsl #2]
    c048:	ebffd4a3 	bl	12dc <strlen>
    c04c:	e6ef0070 	uxtb	r0, r0
    c050:	e2803001 	add	r3, r0, #1	; 0x1
    c054:	e5d5201a 	ldrb	r2, [r5, #26]
    c058:	e1a03083 	lsl	r3, r3, #1
    c05c:	e6ef3073 	uxtb	r3, r3
    c060:	e1530002 	cmp	r3, r2
    c064:	31a0e003 	movcc	lr, r3
    c068:	21a0e002 	movcs	lr, r2
    c06c:	e3a03003 	mov	r3, #3	; 0x3
    c070:	e5c63001 	strb	r3, [r6, #1]
    c074:	e5c6e000 	strb	lr, [r6]
    c078:	e1a05008 	mov	r5, r8
    c07c:	e3a02001 	mov	r2, #1	; 0x1
    c080:	ea000005 	b	c09c <udc_event_handle+0x560>
    c084:	e7943107 	ldr	r3, [r4, r7, lsl #2]
    c088:	e0833002 	add	r3, r3, r2
    c08c:	e5533001 	ldrb	r3, [r3, #-1]
    c090:	e7c63082 	strb	r3, [r6, r2, lsl #1]
    c094:	e5c15001 	strb	r5, [r1, #1]
    c098:	e6ef207c 	uxtb	r2, ip
    c09c:	e1500002 	cmp	r0, r2
    c0a0:	e282c001 	add	ip, r2, #1	; 0x1
    c0a4:	e0861082 	add	r1, r6, r2, lsl #1
    c0a8:	aafffff5 	bge	c084 <udc_event_handle+0x548>
    c0ac:	e59f3208 	ldr	r3, [pc, #520]	; c2bc <udc_event_handle+0x780>
    c0b0:	e583e004 	str	lr, [r3, #4]
    c0b4:	ea000007 	b	c0d8 <udc_event_handle+0x59c>
    c0b8:	e59f21f0 	ldr	r2, [pc, #496]	; c2b0 <udc_event_handle+0x774>
    c0bc:	e3a03702 	mov	r3, #524288	; 0x80000
    c0c0:	e5951024 	ldr	r1, [r5, #36]
    c0c4:	e5823910 	str	r3, [r2, #2320]
    c0c8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    c0cc:	e5821914 	str	r1, [r2, #2324]
    c0d0:	e5823900 	str	r3, [r2, #2304]
    c0d4:	ea00000e 	b	c114 <udc_event_handle+0x5d8>
    c0d8:	e59f11dc 	ldr	r1, [pc, #476]	; c2bc <udc_event_handle+0x780>
    c0dc:	e59f01cc 	ldr	r0, [pc, #460]	; c2b0 <udc_event_handle+0x774>
    c0e0:	e1d120b4 	ldrh	r2, [r1, #4]
    c0e4:	e1d131ba 	ldrh	r3, [r1, #26]
    c0e8:	e1520003 	cmp	r2, r3
    c0ec:	21a02003 	movcs	r2, r3
    c0f0:	e3823702 	orr	r3, r2, #524288	; 0x80000
    c0f4:	e5803910 	str	r3, [r0, #2320]
    c0f8:	e3a03000 	mov	r3, #0	; 0x0
    c0fc:	e5c13001 	strb	r3, [r1, #1]
    c100:	e2833321 	add	r3, r3, #-2080374784	; 0x84000000
    c104:	e5812004 	str	r2, [r1, #4]
    c108:	e5806914 	str	r6, [r0, #2324]
    c10c:	e5803900 	str	r3, [r0, #2304]
    c110:	e581600c 	str	r6, [r1, #12]
    c114:	e1a00006 	mov	r0, r6
    c118:	ebffd556 	bl	1678 <free>
    c11c:	ea000046 	b	c23c <udc_event_handle+0x700>
    c120:	e59fc1bc 	ldr	ip, [pc, #444]	; c2e4 <udc_event_handle+0x7a8>
    c124:	e5dc3000 	ldrb	r3, [ip]
    c128:	e35300ff 	cmp	r3, #255	; 0xff
    c12c:	1a000042 	bne	c23c <udc_event_handle+0x700>
    c130:	e59f1178 	ldr	r1, [pc, #376]	; c2b0 <udc_event_handle+0x774>
    c134:	e59f0180 	ldr	r0, [pc, #384]	; c2bc <udc_event_handle+0x780>
    c138:	e5913800 	ldr	r3, [r1, #2048]
    c13c:	e1d021b6 	ldrh	r2, [r0, #22]
    c140:	e3c33e7f 	bic	r3, r3, #2032	; 0x7f0
    c144:	e202207f 	and	r2, r2, #127	; 0x7f
    c148:	e1833202 	orr	r3, r3, r2, lsl #4
    c14c:	e5813800 	str	r3, [r1, #2048]
    c150:	e5900024 	ldr	r0, [r0, #36]
    c154:	e3a03702 	mov	r3, #524288	; 0x80000
    c158:	e5cc2000 	strb	r2, [ip]
    c15c:	e5813910 	str	r3, [r1, #2320]
    c160:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    c164:	e5810914 	str	r0, [r1, #2324]
    c168:	e5813900 	str	r3, [r1, #2304]
    c16c:	ea000032 	b	c23c <udc_event_handle+0x700>
    c170:	e59f016c 	ldr	r0, [pc, #364]	; c2e4 <udc_event_handle+0x7a8>
    c174:	e5d03000 	ldrb	r3, [r0]
    c178:	e35300ff 	cmp	r3, #255	; 0xff
    c17c:	0a00002e 	beq	c23c <udc_event_handle+0x700>
    c180:	e59f1134 	ldr	r1, [pc, #308]	; c2bc <udc_event_handle+0x780>
    c184:	e1d131b6 	ldrh	r3, [r1, #22]
    c188:	e3530000 	cmp	r3, #0	; 0x0
    c18c:	03e03000 	mvneq	r3, #0	; 0x0
    c190:	05911024 	ldreq	r1, [r1, #36]
    c194:	059f2114 	ldreq	r2, [pc, #276]	; c2b0 <udc_event_handle+0x774>
    c198:	05c03000 	strbeq	r3, [r0]
    c19c:	0a000013 	beq	c1f0 <udc_event_handle+0x6b4>
    c1a0:	e3530001 	cmp	r3, #1	; 0x1
    c1a4:	1a000024 	bne	c23c <udc_event_handle+0x700>
    c1a8:	e59f2100 	ldr	r2, [pc, #256]	; c2b0 <udc_event_handle+0x774>
    c1ac:	e59f3134 	ldr	r3, [pc, #308]	; c2e8 <udc_event_handle+0x7ac>
    c1b0:	e5911024 	ldr	r1, [r1, #36]
    c1b4:	e5823920 	str	r3, [r2, #2336]
    c1b8:	e2833d07 	add	r3, r3, #448	; 0x1c0
    c1bc:	e5823b40 	str	r3, [r2, #2880]
    c1c0:	e592381c 	ldr	r3, [r2, #2076]
    c1c4:	e3833701 	orr	r3, r3, #262144	; 0x40000
    c1c8:	e3833002 	orr	r3, r3, #2	; 0x2
    c1cc:	e582381c 	str	r3, [r2, #2076]
    c1d0:	ea000006 	b	c1f0 <udc_event_handle+0x6b4>
    c1d4:	e210c01f 	ands	ip, r0, #31	; 0x1f
    c1d8:	1a000017 	bne	c23c <udc_event_handle+0x700>
    c1dc:	e3510000 	cmp	r1, #0	; 0x0
    c1e0:	e59f50d4 	ldr	r5, [pc, #212]	; c2bc <udc_event_handle+0x780>
    c1e4:	1a000004 	bne	c1fc <udc_event_handle+0x6c0>
    c1e8:	e5951024 	ldr	r1, [r5, #36]
    c1ec:	e59f20bc 	ldr	r2, [pc, #188]	; c2b0 <udc_event_handle+0x774>
    c1f0:	e3a03702 	mov	r3, #524288	; 0x80000
    c1f4:	e5823910 	str	r3, [r2, #2320]
    c1f8:	ea00000c 	b	c230 <udc_event_handle+0x6f4>
    c1fc:	e1d531b6 	ldrh	r3, [r5, #22]
    c200:	e59f20a8 	ldr	r2, [pc, #168]	; c2b0 <udc_event_handle+0x774>
    c204:	e3530002 	cmp	r3, #2	; 0x2
    c208:	23a03002 	movcs	r3, #2	; 0x2
    c20c:	e6ef3073 	uxtb	r3, r3
    c210:	e1530001 	cmp	r3, r1
    c214:	21a03001 	movcs	r3, r1
    c218:	e3830702 	orr	r0, r3, #524288	; 0x80000
    c21c:	e28d1006 	add	r1, sp, #6	; 0x6
    c220:	e5c5c001 	strb	ip, [r5, #1]
    c224:	e5853004 	str	r3, [r5, #4]
    c228:	e585100c 	str	r1, [r5, #12]
    c22c:	e5820910 	str	r0, [r2, #2320]
    c230:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
    c234:	e5821914 	str	r1, [r2, #2324]
    c238:	e5823900 	str	r3, [r2, #2304]
    c23c:	e3003209 	movw	r3, #521	; 0x209
    c240:	e59f2068 	ldr	r2, [pc, #104]	; c2b0 <udc_event_handle+0x774>
    c244:	e00a3003 	and	r3, sl, r3
    c248:	e5823b08 	str	r3, [r2, #2824]
    c24c:	e3190701 	tst	r9, #262144	; 0x40000
    c250:	0a000013 	beq	c2a4 <udc_event_handle+0x768>
    c254:	e59f1054 	ldr	r1, [pc, #84]	; c2b0 <udc_event_handle+0x774>
    c258:	e5912b48 	ldr	r2, [r1, #2888]
    c25c:	e3120001 	tst	r2, #1	; 0x1
    c260:	0a00000c 	beq	c298 <udc_event_handle+0x75c>
    c264:	e5913b50 	ldr	r3, [r1, #2896]
    c268:	e59f504c 	ldr	r5, [pc, #76]	; c2bc <udc_event_handle+0x780>
    c26c:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
    c270:	e3c3373e 	bic	r3, r3, #16252928	; 0xf80000
    c274:	e3530c02 	cmp	r3, #512	; 0x200
    c278:	05953028 	ldreq	r3, [r5, #40]
    c27c:	13a03000 	movne	r3, #0	; 0x0
    c280:	03833702 	orreq	r3, r3, #524288	; 0x80000
    c284:	05813b50 	streq	r3, [r1, #2896]
    c288:	05913b40 	ldreq	r3, [r1, #2880]
    c28c:	15c53003 	strbne	r3, [r5, #3]
    c290:	03833321 	orreq	r3, r3, #-2080374784	; 0x84000000
    c294:	05813b40 	streq	r3, [r1, #2880]
    c298:	e59f3010 	ldr	r3, [pc, #16]	; c2b0 <udc_event_handle+0x774>
    c29c:	e2022005 	and	r2, r2, #5	; 0x5
    c2a0:	e5832b48 	str	r2, [r3, #2888]
    c2a4:	e28dd008 	add	sp, sp, #8	; 0x8
    c2a8:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    c2ac:	0800d2c0 	.word	0x0800d2c0
    c2b0:	27100000 	.word	0x27100000
    c2b4:	00010001 	.word	0x00010001
    c2b8:	0c001780 	.word	0x0c001780
    c2bc:	0800d2c4 	.word	0x0800d2c4
    c2c0:	60080040 	.word	0x60080040
    c2c4:	0800d2bc 	.word	0x0800d2bc
    c2c8:	18088200 	.word	0x18088200
    c2cc:	0800d2d8 	.word	0x0800d2d8
    c2d0:	0001184d 	.word	0x0001184d
    c2d4:	0800d161 	.word	0x0800d161
    c2d8:	0800d181 	.word	0x0800d181
    c2dc:	0800d138 	.word	0x0800d138
    c2e0:	0800d7d4 	.word	0x0800d7d4
    c2e4:	0800d160 	.word	0x0800d160
    c2e8:	18088040 	.word	0x18088040

0000c2ec <udc_write>:
    c2ec:	e59f30a4 	ldr	r3, [pc, #164]	; c398 <udc_write+0xac>
    c2f0:	e3a02001 	mov	r2, #1	; 0x1
    c2f4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    c2f8:	e583000c 	str	r0, [r3, #12]
    c2fc:	e1a05001 	mov	r5, r1
    c300:	e5c32001 	strb	r2, [r3, #1]
    c304:	e1a04003 	mov	r4, r3
    c308:	e1a08002 	mov	r8, r2
    c30c:	e59f6088 	ldr	r6, [pc, #136]	; c39c <udc_write+0xb0>
    c310:	e59f7088 	ldr	r7, [pc, #136]	; c3a0 <udc_write+0xb4>
    c314:	ea00001b 	b	c388 <udc_write+0x9c>
    c318:	e3550701 	cmp	r5, #262144	; 0x40000
    c31c:	b1a02005 	movlt	r2, r5
    c320:	a3a02701 	movge	r2, #262144	; 0x40000
    c324:	e292303f 	adds	r3, r2, #63	; 0x3f
    c328:	4282307e 	addmi	r3, r2, #126	; 0x7e
    c32c:	e594100c 	ldr	r1, [r4, #12]
    c330:	e1a03343 	asr	r3, r3, #6
    c334:	e1823983 	orr	r3, r2, r3, lsl #19
    c338:	e5863930 	str	r3, [r6, #2352]
    c33c:	e5861934 	str	r1, [r6, #2356]
    c340:	e5963920 	ldr	r3, [r6, #2336]
    c344:	e5842004 	str	r2, [r4, #4]
    c348:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    c34c:	e5863920 	str	r3, [r6, #2336]
    c350:	e5c48002 	strb	r8, [r4, #2]
    c354:	ea000000 	b	c35c <udc_write+0x70>
    c358:	ebfffdf7 	bl	bb3c <udc_event_handle>
    c35c:	e5d43002 	ldrb	r3, [r4, #2]
    c360:	e3530000 	cmp	r3, #0	; 0x0
    c364:	0a000002 	beq	c374 <udc_write+0x88>
    c368:	e5973000 	ldr	r3, [r7]
    c36c:	e3530000 	cmp	r3, #0	; 0x0
    c370:	1afffff8 	bne	c358 <udc_write+0x6c>
    c374:	e5943004 	ldr	r3, [r4, #4]
    c378:	e594200c 	ldr	r2, [r4, #12]
    c37c:	e0635005 	rsb	r5, r3, r5
    c380:	e0833002 	add	r3, r3, r2
    c384:	e584300c 	str	r3, [r4, #12]
    c388:	e3550000 	cmp	r5, #0	; 0x0
    c38c:	1affffe1 	bne	c318 <udc_write+0x2c>
    c390:	e1a00005 	mov	r0, r5
    c394:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    c398:	0800d2c4 	.word	0x0800d2c4
    c39c:	27100000 	.word	0x27100000
    c3a0:	0800d15c 	.word	0x0800d15c

0000c3a4 <udc_read>:
    c3a4:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    c3a8:	e59f30b4 	ldr	r3, [pc, #180]	; c464 <udc_read+0xc0>
    c3ac:	e24dd004 	sub	sp, sp, #4	; 0x4
    c3b0:	e1a04001 	mov	r4, r1
    c3b4:	e1a05003 	mov	r5, r3
    c3b8:	e5830010 	str	r0, [r3, #16]
    c3bc:	e3a0a001 	mov	sl, #1	; 0x1
    c3c0:	e59f60a0 	ldr	r6, [pc, #160]	; c468 <udc_read+0xc4>
    c3c4:	e59f80a0 	ldr	r8, [pc, #160]	; c46c <udc_read+0xc8>
    c3c8:	e1a07003 	mov	r7, r3
    c3cc:	ea00001f 	b	c450 <udc_read+0xac>
    c3d0:	e3540701 	cmp	r4, #262144	; 0x40000
    c3d4:	b1a02004 	movlt	r2, r4
    c3d8:	a3a02701 	movge	r2, #262144	; 0x40000
    c3dc:	e2823f7f 	add	r3, r2, #508	; 0x1fc
    c3e0:	e2833003 	add	r3, r3, #3	; 0x3
    c3e4:	e5951010 	ldr	r1, [r5, #16]
    c3e8:	e1a034c3 	asr	r3, r3, #9
    c3ec:	e1823983 	orr	r3, r2, r3, lsl #19
    c3f0:	e5863b50 	str	r3, [r6, #2896]
    c3f4:	e5861b54 	str	r1, [r6, #2900]
    c3f8:	e5963b40 	ldr	r3, [r6, #2880]
    c3fc:	e5852008 	str	r2, [r5, #8]
    c400:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
    c404:	e5863b40 	str	r3, [r6, #2880]
    c408:	e5c5a003 	strb	sl, [r5, #3]
    c40c:	ea000002 	b	c41c <udc_read+0x78>
    c410:	ebfffdc9 	bl	bb3c <udc_event_handle>
    c414:	e3a00001 	mov	r0, #1	; 0x1
    c418:	ebfff10c 	bl	8850 <udelay>
    c41c:	e5d53003 	ldrb	r3, [r5, #3]
    c420:	e3530000 	cmp	r3, #0	; 0x0
    c424:	0a000004 	beq	c43c <udc_read+0x98>
    c428:	e5983000 	ldr	r3, [r8]
    c42c:	e3530000 	cmp	r3, #0	; 0x0
    c430:	1afffff6 	bne	c410 <udc_read+0x6c>
    c434:	e3e04000 	mvn	r4, #0	; 0x0
    c438:	ea000006 	b	c458 <udc_read+0xb4>
    c43c:	e5973008 	ldr	r3, [r7, #8]
    c440:	e5972010 	ldr	r2, [r7, #16]
    c444:	e0634004 	rsb	r4, r3, r4
    c448:	e0833002 	add	r3, r3, r2
    c44c:	e5873010 	str	r3, [r7, #16]
    c450:	e3540000 	cmp	r4, #0	; 0x0
    c454:	1affffdd 	bne	c3d0 <udc_read+0x2c>
    c458:	e1a00004 	mov	r0, r4
    c45c:	e28dd004 	add	sp, sp, #4	; 0x4
    c460:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    c464:	0800d2c4 	.word	0x0800d2c4
    c468:	27100000 	.word	0x27100000
    c46c:	0800d15c 	.word	0x0800d15c

0000c470 <udc_connect>:
    c470:	e92d4070 	push	{r4, r5, r6, lr}
    c474:	e59f41e4 	ldr	r4, [pc, #484]	; c660 <udc_connect+0x1f0>
    c478:	e3a02000 	mov	r2, #0	; 0x0
    c47c:	e5943024 	ldr	r3, [r4, #36]
    c480:	e3e01000 	mvn	r1, #0	; 0x0
    c484:	e1530002 	cmp	r3, r2
    c488:	e59f31d4 	ldr	r3, [pc, #468]	; c664 <udc_connect+0x1f4>
    c48c:	e5c42003 	strb	r2, [r4, #3]
    c490:	e5c31000 	strb	r1, [r3]
    c494:	e5c42000 	strb	r2, [r4]
    c498:	e5c42002 	strb	r2, [r4, #2]
    c49c:	1a000002 	bne	c4ac <udc_connect+0x3c>
    c4a0:	e3a00028 	mov	r0, #40	; 0x28
    c4a4:	ebffd40b 	bl	14d8 <malloc>
    c4a8:	e5840024 	str	r0, [r4, #36]
    c4ac:	e59f31ac 	ldr	r3, [pc, #428]	; c660 <udc_connect+0x1f0>
    c4b0:	e3a06001 	mov	r6, #1	; 0x1
    c4b4:	e59f51ac 	ldr	r5, [pc, #428]	; c668 <udc_connect+0x1f8>
    c4b8:	e59f01ac 	ldr	r0, [pc, #428]	; c66c <udc_connect+0x1fc>
    c4bc:	e5836020 	str	r6, [r3, #32]
    c4c0:	ebffd094 	bl	718 <module_enable>
    c4c4:	e3a02004 	mov	r2, #4	; 0x4
    c4c8:	e3a03003 	mov	r3, #3	; 0x3
    c4cc:	e3a01000 	mov	r1, #0	; 0x0
    c4d0:	e585243c 	str	r2, [r5, #1084]
    c4d4:	e5853440 	str	r3, [r5, #1088]
    c4d8:	e5852444 	str	r2, [r5, #1092]
    c4dc:	e5853448 	str	r3, [r5, #1096]
    c4e0:	e585344c 	str	r3, [r5, #1100]
    c4e4:	e5853450 	str	r3, [r5, #1104]
    c4e8:	e5856454 	str	r6, [r5, #1108]
    c4ec:	e5856458 	str	r6, [r5, #1112]
    c4f0:	e585145c 	str	r1, [r5, #1116]
    c4f4:	e5851460 	str	r1, [r5, #1120]
    c4f8:	e5953420 	ldr	r3, [r5, #1056]
    c4fc:	e1a00006 	mov	r0, r6
    c500:	e3c33007 	bic	r3, r3, #7	; 0x7
    c504:	e3833005 	orr	r3, r3, #5	; 0x5
    c508:	e5853420 	str	r3, [r5, #1056]
    c50c:	e5953424 	ldr	r3, [r5, #1060]
    c510:	e3833003 	orr	r3, r3, #3	; 0x3
    c514:	e5853424 	str	r3, [r5, #1060]
    c518:	e595342c 	ldr	r3, [r5, #1068]
    c51c:	e1833006 	orr	r3, r3, r6
    c520:	e585342c 	str	r3, [r5, #1068]
    c524:	e5954400 	ldr	r4, [r5, #1024]
    c528:	e384400f 	orr	r4, r4, #15	; 0xf
    c52c:	e5854400 	str	r4, [r5, #1024]
    c530:	ebfff0c6 	bl	8850 <udelay>
    c534:	e3c4300e 	bic	r3, r4, #14	; 0xe
    c538:	e5853400 	str	r3, [r5, #1024]
    c53c:	e1a00006 	mov	r0, r6
    c540:	e3c4400f 	bic	r4, r4, #15	; 0xf
    c544:	ebfff0c1 	bl	8850 <udelay>
    c548:	e5854400 	str	r4, [r5, #1024]
    c54c:	e1a00006 	mov	r0, r6
    c550:	ebfff0be 	bl	8850 <udelay>
    c554:	e59f2114 	ldr	r2, [pc, #276]	; c670 <udc_connect+0x200>
    c558:	e59f3114 	ldr	r3, [pc, #276]	; c674 <udc_connect+0x204>
    c55c:	e1a05002 	mov	r5, r2
    c560:	e582300c 	str	r3, [r2, #12]
    c564:	e5954010 	ldr	r4, [r5, #16]
    c568:	e3a00028 	mov	r0, #40	; 0x28
    c56c:	ebfff0b7 	bl	8850 <udelay>
    c570:	e3540000 	cmp	r4, #0	; 0x0
    c574:	e59f40f4 	ldr	r4, [pc, #244]	; c670 <udc_connect+0x200>
    c578:	aafffff9 	bge	c564 <udc_connect+0xf4>
    c57c:	e5943010 	ldr	r3, [r4, #16]
    c580:	e3a0000a 	mov	r0, #10	; 0xa
    c584:	e3833001 	orr	r3, r3, #1	; 0x1
    c588:	e5843010 	str	r3, [r4, #16]
    c58c:	ebfff0af 	bl	8850 <udelay>
    c590:	e1a05004 	mov	r5, r4
    c594:	e5954010 	ldr	r4, [r5, #16]
    c598:	e3a0000a 	mov	r0, #10	; 0xa
    c59c:	ebfff0ab 	bl	8850 <udelay>
    c5a0:	e3140001 	tst	r4, #1	; 0x1
    c5a4:	e59fc0c4 	ldr	ip, [pc, #196]	; c670 <udc_connect+0x200>
    c5a8:	1afffff9 	bne	c594 <udc_connect+0x124>
    c5ac:	e3a03027 	mov	r3, #39	; 0x27
    c5b0:	e58c3008 	str	r3, [ip, #8]
    c5b4:	e59c303c 	ldr	r3, [ip, #60]
    c5b8:	e59c3014 	ldr	r3, [ip, #20]
    c5bc:	e2130001 	ands	r0, r3, #1	; 0x1
    c5c0:	1a000024 	bne	c658 <udc_connect+0x1e8>
    c5c4:	e3a03301 	mov	r3, #67108864	; 0x4000000
    c5c8:	e58c3800 	str	r3, [ip, #2048]
    c5cc:	e59f30a4 	ldr	r3, [pc, #164]	; c678 <udc_connect+0x208>
    c5d0:	e59f20a4 	ldr	r2, [pc, #164]	; c67c <udc_connect+0x20c>
    c5d4:	e58c3018 	str	r3, [ip, #24]
    c5d8:	e59f30a0 	ldr	r3, [pc, #160]	; c680 <udc_connect+0x210>
    c5dc:	e59f10a0 	ldr	r1, [pc, #160]	; c684 <udc_connect+0x214>
    c5e0:	e5823014 	str	r3, [r2, #20]
    c5e4:	e59f309c 	ldr	r3, [pc, #156]	; c688 <udc_connect+0x218>
    c5e8:	e5821010 	str	r1, [r2, #16]
    c5ec:	e5823004 	str	r3, [r2, #4]
    c5f0:	e59f3094 	ldr	r3, [pc, #148]	; c68c <udc_connect+0x21c>
    c5f4:	e582100c 	str	r1, [r2, #12]
    c5f8:	e5823008 	str	r3, [r2, #8]
    c5fc:	ebfff0b7 	bl	88e0 <get_timer>
    c600:	e59f5058 	ldr	r5, [pc, #88]	; c660 <udc_connect+0x1f0>
    c604:	e1a04000 	mov	r4, r0
    c608:	ebfffd4b 	bl	bb3c <udc_event_handle>
    c60c:	e5d53000 	ldrb	r3, [r5]
    c610:	e1a00004 	mov	r0, r4
    c614:	e3530000 	cmp	r3, #0	; 0x0
    c618:	1a000003 	bne	c62c <udc_connect+0x1bc>
    c61c:	ebfff0af 	bl	88e0 <get_timer>
    c620:	e3500ffa 	cmp	r0, #1000	; 0x3e8
    c624:	9afffff7 	bls	c608 <udc_connect+0x198>
    c628:	ea00000a 	b	c658 <udc_connect+0x1e8>
    c62c:	e59f203c 	ldr	r2, [pc, #60]	; c670 <udc_connect+0x200>
    c630:	e59f3058 	ldr	r3, [pc, #88]	; c690 <udc_connect+0x220>
    c634:	e3a00000 	mov	r0, #0	; 0x0
    c638:	e5823920 	str	r3, [r2, #2336]
    c63c:	e2833d07 	add	r3, r3, #448	; 0x1c0
    c640:	e5823b40 	str	r3, [r2, #2880]
    c644:	e592381c 	ldr	r3, [r2, #2076]
    c648:	e3833701 	orr	r3, r3, #262144	; 0x40000
    c64c:	e3833002 	orr	r3, r3, #2	; 0x2
    c650:	e582381c 	str	r3, [r2, #2076]
    c654:	e8bd8070 	pop	{r4, r5, r6, pc}
    c658:	e3e00000 	mvn	r0, #0	; 0x0
    c65c:	e8bd8070 	pop	{r4, r5, r6, pc}
    c660:	0800d2c4 	.word	0x0800d2c4
    c664:	0800d160 	.word	0x0800d160
    c668:	2d038000 	.word	0x2d038000
    c66c:	2d038400 	.word	0x2d038400
    c670:	27100000 	.word	0x27100000
    c674:	4c001408 	.word	0x4c001408
    c678:	000c3c00 	.word	0x000c3c00
    c67c:	0800d7d4 	.word	0x0800d7d4
    c680:	00013f60 	.word	0x00013f60
    c684:	00013f75 	.word	0x00013f75
    c688:	00013f69 	.word	0x00013f69
    c68c:	0800d13c 	.word	0x0800d13c
    c690:	18088040 	.word	0x18088040

0000c694 <linkpc_set_stage>:
    c694:	e1a01000 	mov	r1, r0
    c698:	e59f0004 	ldr	r0, [pc, #4]	; c6a4 <linkpc_set_stage+0x10>
    c69c:	e3a02010 	mov	r2, #16	; 0x10
    c6a0:	eaffd2bd 	b	119c <strncpy>
    c6a4:	0800d18c 	.word	0x0800d18c

0000c6a8 <__response_ok>:
    c6a8:	e92d4010 	push	{r4, lr}
    c6ac:	e24dd020 	sub	sp, sp, #32	; 0x20
    c6b0:	e59f1038 	ldr	r1, [pc, #56]	; c6f0 <__response_ok+0x48>
    c6b4:	e3a02005 	mov	r2, #5	; 0x5
    c6b8:	e1a0000d 	mov	r0, sp
    c6bc:	ebffd320 	bl	1344 <memcpy>
    c6c0:	e3a0201b 	mov	r2, #27	; 0x1b
    c6c4:	e3a01000 	mov	r1, #0	; 0x0
    c6c8:	e28d0005 	add	r0, sp, #5	; 0x5
    c6cc:	ebffd314 	bl	1324 <memset>
    c6d0:	e1a0000d 	mov	r0, sp
    c6d4:	e3a01004 	mov	r1, #4	; 0x4
    c6d8:	ebffff03 	bl	c2ec <udc_write>
    c6dc:	e59f0010 	ldr	r0, [pc, #16]	; c6f4 <__response_ok+0x4c>
    c6e0:	e1a0400d 	mov	r4, sp
    c6e4:	ebfff059 	bl	8850 <udelay>
    c6e8:	e28dd020 	add	sp, sp, #32	; 0x20
    c6ec:	e8bd8010 	pop	{r4, pc}
    c6f0:	0001185f 	.word	0x0001185f
    c6f4:	0007a120 	.word	0x0007a120

0000c6f8 <linkpc_loop>:
    c6f8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    c6fc:	e59f05e0 	ldr	r0, [pc, #1504]	; cce4 <linkpc_loop+0x5ec>
    c700:	e24ddf59 	sub	sp, sp, #356	; 0x164
    c704:	e59f45dc 	ldr	r4, [pc, #1500]	; cce8 <linkpc_loop+0x5f0>
    c708:	e1a0e00f 	mov	lr, pc
    c70c:	e594f000 	ldr	pc, [r4]
    c710:	ebffff56 	bl	c470 <udc_connect>
    c714:	e3500000 	cmp	r0, #0	; 0x0
    c718:	13a04001 	movne	r4, #1	; 0x1
    c71c:	1a000169 	bne	ccc8 <linkpc_loop+0x5d0>
    c720:	e59f05c4 	ldr	r0, [pc, #1476]	; ccec <linkpc_loop+0x5f4>
    c724:	e1a0e00f 	mov	lr, pc
    c728:	e594f000 	ldr	pc, [r4]
    c72c:	e59f05bc 	ldr	r0, [pc, #1468]	; ccf0 <linkpc_loop+0x5f8>
    c730:	e1a0e00f 	mov	lr, pc
    c734:	e594f000 	ldr	pc, [r4]
    c738:	e28d708c 	add	r7, sp, #140	; 0x8c
    c73c:	e59f95b0 	ldr	r9, [pc, #1456]	; ccf4 <linkpc_loop+0x5fc>
    c740:	e1a0a004 	mov	sl, r4
    c744:	e287b004 	add	fp, r7, #4	; 0x4
    c748:	e5991000 	ldr	r1, [r9]
    c74c:	e3510000 	cmp	r1, #0	; 0x0
    c750:	159f25a0 	ldrne	r2, [pc, #1440]	; ccf8 <linkpc_loop+0x600>
    c754:	028d4f43 	addeq	r4, sp, #268	; 0x10c
    c758:	15923000 	ldrne	r3, [r2]
    c75c:	03a05040 	moveq	r5, #64	; 0x40
    c760:	12834302 	addne	r4, r3, #134217728	; 0x8000000
    c764:	11a05001 	movne	r5, r1
    c768:	e1a01005 	mov	r1, r5
    c76c:	e1a00004 	mov	r0, r4
    c770:	ebffff0b 	bl	c3a4 <udc_read>
    c774:	e1a00007 	mov	r0, r7
    c778:	e3a01000 	mov	r1, #0	; 0x0
    c77c:	e3a02080 	mov	r2, #128	; 0x80
    c780:	ebffd2e7 	bl	1324 <memset>
    c784:	e5993000 	ldr	r3, [r9]
    c788:	e3530000 	cmp	r3, #0	; 0x0
    c78c:	0a00002a 	beq	c83c <linkpc_loop+0x144>
    c790:	e28d400c 	add	r4, sp, #12	; 0xc
    c794:	e3a02080 	mov	r2, #128	; 0x80
    c798:	e3a01000 	mov	r1, #0	; 0x0
    c79c:	e1a00004 	mov	r0, r4
    c7a0:	ebffd2df 	bl	1324 <memset>
    c7a4:	e59f0550 	ldr	r0, [pc, #1360]	; ccfc <linkpc_loop+0x604>
    c7a8:	e59f1550 	ldr	r1, [pc, #1360]	; cd00 <linkpc_loop+0x608>
    c7ac:	e1a0e00f 	mov	lr, pc
    c7b0:	e59af000 	ldr	pc, [sl]
    c7b4:	e59fc53c 	ldr	ip, [pc, #1340]	; ccf8 <linkpc_loop+0x600>
    c7b8:	e5990000 	ldr	r0, [r9]
    c7bc:	e59c3000 	ldr	r3, [ip]
    c7c0:	e0632000 	rsb	r2, r3, r0
    c7c4:	e1550002 	cmp	r5, r2
    c7c8:	90833005 	addls	r3, r3, r5
    c7cc:	80833002 	addhi	r3, r3, r2
    c7d0:	e1530000 	cmp	r3, r0
    c7d4:	e58c3000 	str	r3, [ip]
    c7d8:	3affffda 	bcc	c748 <linkpc_loop+0x50>
    c7dc:	e59f3520 	ldr	r3, [pc, #1312]	; cd04 <linkpc_loop+0x60c>
    c7e0:	e5933000 	ldr	r3, [r3]
    c7e4:	e3530000 	cmp	r3, #0	; 0x0
    c7e8:	e59f3518 	ldr	r3, [pc, #1304]	; cd08 <linkpc_loop+0x610>
    c7ec:	159f1518 	ldrne	r1, [pc, #1304]	; cd0c <linkpc_loop+0x614>
    c7f0:	059f1518 	ldreq	r1, [pc, #1304]	; cd10 <linkpc_loop+0x618>
    c7f4:	e5830000 	str	r0, [r3]
    c7f8:	e3a03000 	mov	r3, #0	; 0x0
    c7fc:	11a00004 	movne	r0, r4
    c800:	01a00004 	moveq	r0, r4
    c804:	e28d400c 	add	r4, sp, #12	; 0xc
    c808:	e5893000 	str	r3, [r9]
    c80c:	eb0005c3 	bl	df20 <sprintf>
    c810:	e1a00004 	mov	r0, r4
    c814:	ebffd2b0 	bl	12dc <strlen>
    c818:	e1a01000 	mov	r1, r0
    c81c:	e1a00004 	mov	r0, r4
    c820:	ebfffeb1 	bl	c2ec <udc_write>
    c824:	e59f24cc 	ldr	r2, [pc, #1228]	; ccf8 <linkpc_loop+0x600>
    c828:	e59f04e4 	ldr	r0, [pc, #1252]	; cd14 <linkpc_loop+0x61c>
    c82c:	e5921000 	ldr	r1, [r2]
    c830:	e1a0e00f 	mov	lr, pc
    c834:	e59af000 	ldr	pc, [sl]
    c838:	eaffffc2 	b	c748 <linkpc_loop+0x50>
    c83c:	e1a01004 	mov	r1, r4
    c840:	e59f04b4 	ldr	r0, [pc, #1204]	; ccfc <linkpc_loop+0x604>
    c844:	e1a0e00f 	mov	lr, pc
    c848:	e59af000 	ldr	pc, [sl]
    c84c:	e59f14c4 	ldr	r1, [pc, #1220]	; cd18 <linkpc_loop+0x620>
    c850:	e1a00007 	mov	r0, r7
    c854:	eb0005b1 	bl	df20 <sprintf>
    c858:	e1a00004 	mov	r0, r4
    c85c:	e59f14b8 	ldr	r1, [pc, #1208]	; cd1c <linkpc_loop+0x624>
    c860:	e3a02006 	mov	r2, #6	; 0x6
    c864:	ebffd27d 	bl	1260 <strncmp>
    c868:	e3500000 	cmp	r0, #0	; 0x0
    c86c:	1a000002 	bne	c87c <linkpc_loop+0x184>
    c870:	ebffff8c 	bl	c6a8 <__response_ok>
    c874:	ebffcfe7 	bl	818 <system_reset>
    c878:	ea000105 	b	cc94 <linkpc_loop+0x59c>
    c87c:	e1a00004 	mov	r0, r4
    c880:	e59f1498 	ldr	r1, [pc, #1176]	; cd20 <linkpc_loop+0x628>
    c884:	e3a02007 	mov	r2, #7	; 0x7
    c888:	ebffd274 	bl	1260 <strncmp>
    c88c:	e3500000 	cmp	r0, #0	; 0x0
    c890:	1a00002d 	bne	c94c <linkpc_loop+0x254>
    c894:	e2844007 	add	r4, r4, #7	; 0x7
    c898:	e1a02004 	mov	r2, r4
    c89c:	e59f1480 	ldr	r1, [pc, #1152]	; cd24 <linkpc_loop+0x62c>
    c8a0:	e59f0480 	ldr	r0, [pc, #1152]	; cd28 <linkpc_loop+0x630>
    c8a4:	e1a0e00f 	mov	lr, pc
    c8a8:	e59af000 	ldr	pc, [sl]
    c8ac:	e59f145c 	ldr	r1, [pc, #1116]	; cd10 <linkpc_loop+0x618>
    c8b0:	e1a00007 	mov	r0, r7
    c8b4:	ebffd231 	bl	1180 <strcpy>
    c8b8:	e59f146c 	ldr	r1, [pc, #1132]	; cd2c <linkpc_loop+0x634>
    c8bc:	e1a00004 	mov	r0, r4
    c8c0:	e3a02007 	mov	r2, #7	; 0x7
    c8c4:	ebffd265 	bl	1260 <strncmp>
    c8c8:	e3500000 	cmp	r0, #0	; 0x0
    c8cc:	01a0000b 	moveq	r0, fp
    c8d0:	059f1458 	ldreq	r1, [pc, #1112]	; cd30 <linkpc_loop+0x638>
    c8d4:	0a000010 	beq	c91c <linkpc_loop+0x224>
    c8d8:	e59f1454 	ldr	r1, [pc, #1108]	; cd34 <linkpc_loop+0x63c>
    c8dc:	e1a00004 	mov	r0, r4
    c8e0:	e3a02005 	mov	r2, #5	; 0x5
    c8e4:	ebffd25d 	bl	1260 <strncmp>
    c8e8:	e3500000 	cmp	r0, #0	; 0x0
    c8ec:	01a0000b 	moveq	r0, fp
    c8f0:	059f1440 	ldreq	r1, [pc, #1088]	; cd38 <linkpc_loop+0x640>
    c8f4:	0a000008 	beq	c91c <linkpc_loop+0x224>
    c8f8:	e1a00004 	mov	r0, r4
    c8fc:	e59f1438 	ldr	r1, [pc, #1080]	; cd3c <linkpc_loop+0x644>
    c900:	e3a02008 	mov	r2, #8	; 0x8
    c904:	ebffd255 	bl	1260 <strncmp>
    c908:	e3500000 	cmp	r0, #0	; 0x0
    c90c:	1a000004 	bne	c924 <linkpc_loop+0x22c>
    c910:	e59f3428 	ldr	r3, [pc, #1064]	; cd40 <linkpc_loop+0x648>
    c914:	e1a0000b 	mov	r0, fp
    c918:	e5931008 	ldr	r1, [r3, #8]
    c91c:	ebffd217 	bl	1180 <strcpy>
    c920:	ea0000db 	b	cc94 <linkpc_loop+0x59c>
    c924:	e1a00004 	mov	r0, r4
    c928:	e59f1414 	ldr	r1, [pc, #1044]	; cd44 <linkpc_loop+0x64c>
    c92c:	e3a0200c 	mov	r2, #12	; 0xc
    c930:	ebffd24a 	bl	1260 <strncmp>
    c934:	e3500000 	cmp	r0, #0	; 0x0
    c938:	1a0000d5 	bne	cc94 <linkpc_loop+0x59c>
    c93c:	e1a0000b 	mov	r0, fp
    c940:	e59f1400 	ldr	r1, [pc, #1024]	; cd48 <linkpc_loop+0x650>
    c944:	e3a02903 	mov	r2, #49152	; 0xc000
    c948:	ea000020 	b	c9d0 <linkpc_loop+0x2d8>
    c94c:	e1a00004 	mov	r0, r4
    c950:	e59f13f4 	ldr	r1, [pc, #1012]	; cd4c <linkpc_loop+0x654>
    c954:	e3a02009 	mov	r2, #9	; 0x9
    c958:	ebffd240 	bl	1260 <strncmp>
    c95c:	e2505000 	subs	r5, r0, #0	; 0x0
    c960:	1a00001c 	bne	c9d8 <linkpc_loop+0x2e0>
    c964:	e59f3398 	ldr	r3, [pc, #920]	; cd04 <linkpc_loop+0x60c>
    c968:	e59fc388 	ldr	ip, [pc, #904]	; ccf8 <linkpc_loop+0x600>
    c96c:	e3a02010 	mov	r2, #16	; 0x10
    c970:	e1a01005 	mov	r1, r5
    c974:	e5835000 	str	r5, [r3]
    c978:	e58c5000 	str	r5, [ip]
    c97c:	e2840009 	add	r0, r4, #9	; 0x9
    c980:	eb0001da 	bl	d0f0 <simple_strtoul>
    c984:	e1a01000 	mov	r1, r0
    c988:	e5890000 	str	r0, [r9]
    c98c:	e59f03bc 	ldr	r0, [pc, #956]	; cd50 <linkpc_loop+0x658>
    c990:	e1a0e00f 	mov	lr, pc
    c994:	e59af000 	ldr	pc, [sl]
    c998:	e5992000 	ldr	r2, [r9]
    c99c:	e3520000 	cmp	r2, #0	; 0x0
    c9a0:	01a00007 	moveq	r0, r7
    c9a4:	059f13a8 	ldreq	r1, [pc, #936]	; cd54 <linkpc_loop+0x65c>
    c9a8:	0a0000b8 	beq	cc90 <linkpc_loop+0x598>
    c9ac:	e3520903 	cmp	r2, #49152	; 0xc000
    c9b0:	91a00007 	movls	r0, r7
    c9b4:	959f139c 	ldrls	r1, [pc, #924]	; cd58 <linkpc_loop+0x660>
    c9b8:	9a000004 	bls	c9d0 <linkpc_loop+0x2d8>
    c9bc:	e1a00007 	mov	r0, r7
    c9c0:	e59f1394 	ldr	r1, [pc, #916]	; cd5c <linkpc_loop+0x664>
    c9c4:	eb000555 	bl	df20 <sprintf>
    c9c8:	e5895000 	str	r5, [r9]
    c9cc:	ea0000b0 	b	cc94 <linkpc_loop+0x59c>
    c9d0:	eb000552 	bl	df20 <sprintf>
    c9d4:	ea0000ae 	b	cc94 <linkpc_loop+0x59c>
    c9d8:	e1a00004 	mov	r0, r4
    c9dc:	e59f137c 	ldr	r1, [pc, #892]	; cd60 <linkpc_loop+0x668>
    c9e0:	e3a02006 	mov	r2, #6	; 0x6
    c9e4:	ebffd21d 	bl	1260 <strncmp>
    c9e8:	e3500000 	cmp	r0, #0	; 0x0
    c9ec:	1a000065 	bne	cb88 <linkpc_loop+0x490>
    c9f0:	e2844006 	add	r4, r4, #6	; 0x6
    c9f4:	e1a00004 	mov	r0, r4
    c9f8:	e59f1364 	ldr	r1, [pc, #868]	; cd64 <linkpc_loop+0x66c>
    c9fc:	e3a02003 	mov	r2, #3	; 0x3
    ca00:	ebffd216 	bl	1260 <strncmp>
    ca04:	e3500000 	cmp	r0, #0	; 0x0
    ca08:	1a000007 	bne	ca2c <linkpc_loop+0x334>
    ca0c:	e2800302 	add	r0, r0, #134217728	; 0x8000000
    ca10:	ebffe5fa 	bl	6200 <cdbg_config_isi>
    ca14:	e3500000 	cmp	r0, #0	; 0x0
    ca18:	11a00007 	movne	r0, r7
    ca1c:	159f1344 	ldrne	r1, [pc, #836]	; cd68 <linkpc_loop+0x670>
    ca20:	01a00007 	moveq	r0, r7
    ca24:	059f12e4 	ldreq	r1, [pc, #740]	; cd10 <linkpc_loop+0x618>
    ca28:	ea000098 	b	cc90 <linkpc_loop+0x598>
    ca2c:	e1a00004 	mov	r0, r4
    ca30:	ebffe0ca 	bl	4d60 <vs_device_id>
    ca34:	e59f6330 	ldr	r6, [pc, #816]	; cd6c <linkpc_loop+0x674>
    ca38:	e59fc330 	ldr	ip, [pc, #816]	; cd70 <linkpc_loop+0x678>
    ca3c:	e5963000 	ldr	r3, [r6]
    ca40:	e1a05000 	mov	r5, r0
    ca44:	e58d3000 	str	r3, [sp]
    ca48:	e59f0324 	ldr	r0, [pc, #804]	; cd74 <linkpc_loop+0x67c>
    ca4c:	e59f1324 	ldr	r1, [pc, #804]	; cd78 <linkpc_loop+0x680>
    ca50:	e1a02005 	mov	r2, r5
    ca54:	e59c3000 	ldr	r3, [ip]
    ca58:	e1a0e00f 	mov	lr, pc
    ca5c:	e59af000 	ldr	pc, [sl]
    ca60:	e355000e 	cmp	r5, #14	; 0xe
    ca64:	ca000006 	bgt	ca84 <linkpc_loop+0x38c>
    ca68:	e1a00005 	mov	r0, r5
    ca6c:	ebffe0a7 	bl	4d10 <vs_device_erasable>
    ca70:	e3500000 	cmp	r0, #0	; 0x0
    ca74:	0a000005 	beq	ca90 <linkpc_loop+0x398>
    ca78:	e5963000 	ldr	r3, [r6]
    ca7c:	e3530000 	cmp	r3, #0	; 0x0
    ca80:	1a000002 	bne	ca90 <linkpc_loop+0x398>
    ca84:	e1a00007 	mov	r0, r7
    ca88:	e59f12ec 	ldr	r1, [pc, #748]	; cd7c <linkpc_loop+0x684>
    ca8c:	ea00007f 	b	cc90 <linkpc_loop+0x598>
    ca90:	e3a00901 	mov	r0, #16384	; 0x4000
    ca94:	ebffd28f 	bl	14d8 <malloc>
    ca98:	e2508000 	subs	r8, r0, #0	; 0x0
    ca9c:	01a00007 	moveq	r0, r7
    caa0:	059f12d8 	ldreq	r1, [pc, #728]	; cd80 <linkpc_loop+0x688>
    caa4:	0a000079 	beq	cc90 <linkpc_loop+0x598>
    caa8:	e3a01001 	mov	r1, #1	; 0x1
    caac:	e1a00005 	mov	r0, r5
    cab0:	ebffe17e 	bl	50b0 <vs_assign_by_id>
    cab4:	e3500000 	cmp	r0, #0	; 0x0
    cab8:	b1a00007 	movlt	r0, r7
    cabc:	b59f12c0 	ldrlt	r1, [pc, #704]	; cd84 <linkpc_loop+0x68c>
    cac0:	ba00002c 	blt	cb78 <linkpc_loop+0x480>
    cac4:	e355000d 	cmp	r5, #13	; 0xd
    cac8:	13a06002 	movne	r6, #2	; 0x2
    cacc:	03a06001 	moveq	r6, #1	; 0x1
    cad0:	e1a04c06 	lsl	r4, r6, #24
    cad4:	e3a00302 	mov	r0, #134217728	; 0x8000000
    cad8:	03a0500f 	moveq	r5, #15	; 0xf
    cadc:	e58d414c 	str	r4, [sp, #332]
    cae0:	ebfffbcb 	bl	ba14 <ram_addr2offs>
    cae4:	e3a024ff 	mov	r2, #-16777216	; 0xff000000
    cae8:	e0922000 	adds	r2, r2, r0
    caec:	e3e03000 	mvn	r3, #0	; 0x0
    caf0:	e0a33001 	adc	r3, r3, r1
    caf4:	e59f1270 	ldr	r1, [pc, #624]	; cd6c <linkpc_loop+0x674>
    caf8:	e59fc270 	ldr	ip, [pc, #624]	; cd70 <linkpc_loop+0x678>
    cafc:	e1d100b2 	ldrh	r0, [r1, #2]
    cb00:	e1a014a2 	lsr	r1, r2, #9
    cb04:	e1811b83 	orr	r1, r1, r3, lsl #23
    cb08:	e59f31f8 	ldr	r3, [pc, #504]	; cd08 <linkpc_loop+0x610>
    cb0c:	e1844001 	orr	r4, r4, r1
    cb10:	e5932000 	ldr	r2, [r3]
    cb14:	e59c3000 	ldr	r3, [ip]
    cb18:	e3560001 	cmp	r6, #1	; 0x1
    cb1c:	e1a0c800 	lsl	ip, r0, #16
    cb20:	e58d414c 	str	r4, [sp, #332]
    cb24:	e58d3150 	str	r3, [sp, #336]
    cb28:	e58d2158 	str	r2, [sp, #344]
    cb2c:	e58dc154 	str	ip, [sp, #340]
    cb30:	1a000004 	bne	cb48 <linkpc_loop+0x450>
    cb34:	ebfffedb 	bl	c6a8 <__response_ok>
    cb38:	e59d0150 	ldr	r0, [sp, #336]
    cb3c:	e3a01000 	mov	r1, #0	; 0x0
    cb40:	ebffe539 	bl	602c <cdbg_jump>
    cb44:	eafffffe 	b	cb44 <linkpc_loop+0x44c>
    cb48:	e3a0100d 	mov	r1, #13	; 0xd
    cb4c:	e1a03005 	mov	r3, r5
    cb50:	e28d0f53 	add	r0, sp, #332	; 0x14c
    cb54:	e3a02000 	mov	r2, #0	; 0x0
    cb58:	e58dc000 	str	ip, [sp]
    cb5c:	e58d8004 	str	r8, [sp, #4]
    cb60:	ebffe5bb 	bl	6254 <burn_image_loop>
    cb64:	e3500000 	cmp	r0, #0	; 0x0
    cb68:	159f1218 	ldrne	r1, [pc, #536]	; cd88 <linkpc_loop+0x690>
    cb6c:	059f119c 	ldreq	r1, [pc, #412]	; cd10 <linkpc_loop+0x618>
    cb70:	11a00007 	movne	r0, r7
    cb74:	01a00007 	moveq	r0, r7
    cb78:	eb0004e8 	bl	df20 <sprintf>
    cb7c:	e1a00008 	mov	r0, r8
    cb80:	ebffd2bc 	bl	1678 <free>
    cb84:	ea000042 	b	cc94 <linkpc_loop+0x59c>
    cb88:	e1a00004 	mov	r0, r4
    cb8c:	e59f11f8 	ldr	r1, [pc, #504]	; cd8c <linkpc_loop+0x694>
    cb90:	e3a02008 	mov	r2, #8	; 0x8
    cb94:	ebffd1b1 	bl	1260 <strncmp>
    cb98:	e3500000 	cmp	r0, #0	; 0x0
    cb9c:	1a000003 	bne	cbb0 <linkpc_loop+0x4b8>
    cba0:	ebfffec0 	bl	c6a8 <__response_ok>
    cba4:	e3a00001 	mov	r0, #1	; 0x1
    cba8:	ebffce0e 	bl	3e8 <try_boot>
    cbac:	ea000038 	b	cc94 <linkpc_loop+0x59c>
    cbb0:	e1a00004 	mov	r0, r4
    cbb4:	e59f11d4 	ldr	r1, [pc, #468]	; cd90 <linkpc_loop+0x698>
    cbb8:	e3a0200a 	mov	r2, #10	; 0xa
    cbbc:	ebffd1a7 	bl	1260 <strncmp>
    cbc0:	e2505000 	subs	r5, r0, #0	; 0x0
    cbc4:	1a000019 	bne	cc30 <linkpc_loop+0x538>
    cbc8:	e28d1f57 	add	r1, sp, #348	; 0x15c
    cbcc:	e284000b 	add	r0, r4, #11	; 0xb
    cbd0:	e3a02010 	mov	r2, #16	; 0x10
    cbd4:	eb000145 	bl	d0f0 <simple_strtoul>
    cbd8:	e59f2190 	ldr	r2, [pc, #400]	; cd70 <linkpc_loop+0x678>
    cbdc:	e1a01005 	mov	r1, r5
    cbe0:	e5820000 	str	r0, [r2]
    cbe4:	e59d015c 	ldr	r0, [sp, #348]
    cbe8:	e3a02010 	mov	r2, #16	; 0x10
    cbec:	e2800001 	add	r0, r0, #1	; 0x1
    cbf0:	eb00013e 	bl	d0f0 <simple_strtoul>
    cbf4:	e59f4170 	ldr	r4, [pc, #368]	; cd6c <linkpc_loop+0x674>
    cbf8:	e59f3170 	ldr	r3, [pc, #368]	; cd70 <linkpc_loop+0x678>
    cbfc:	e1a02000 	mov	r2, r0
    cc00:	e5840000 	str	r0, [r4]
    cc04:	e5931000 	ldr	r1, [r3]
    cc08:	e59f0184 	ldr	r0, [pc, #388]	; cd94 <linkpc_loop+0x69c>
    cc0c:	e1a0e00f 	mov	lr, pc
    cc10:	e59af000 	ldr	pc, [sl]
    cc14:	e59fc154 	ldr	ip, [pc, #340]	; cd70 <linkpc_loop+0x678>
    cc18:	e1a00007 	mov	r0, r7
    cc1c:	e5943000 	ldr	r3, [r4]
    cc20:	e59f1170 	ldr	r1, [pc, #368]	; cd98 <linkpc_loop+0x6a0>
    cc24:	e59c2000 	ldr	r2, [ip]
    cc28:	eb0004bc 	bl	df20 <sprintf>
    cc2c:	ea000018 	b	cc94 <linkpc_loop+0x59c>
    cc30:	e1a00004 	mov	r0, r4
    cc34:	e59f1160 	ldr	r1, [pc, #352]	; cd9c <linkpc_loop+0x6a4>
    cc38:	e3a0200d 	mov	r2, #13	; 0xd
    cc3c:	ebffd187 	bl	1260 <strncmp>
    cc40:	e3500000 	cmp	r0, #0	; 0x0
    cc44:	1a000006 	bne	cc64 <linkpc_loop+0x56c>
    cc48:	e59f20f0 	ldr	r2, [pc, #240]	; cd40 <linkpc_loop+0x648>
    cc4c:	e284100e 	add	r1, r4, #14	; 0xe
    cc50:	e5920008 	ldr	r0, [r2, #8]
    cc54:	ebffd149 	bl	1180 <strcpy>
    cc58:	ebfffe92 	bl	c6a8 <__response_ok>
    cc5c:	ebfffea5 	bl	c6f8 <linkpc_loop>
    cc60:	ea00000b 	b	cc94 <linkpc_loop+0x59c>
    cc64:	e1a00004 	mov	r0, r4
    cc68:	e59f1130 	ldr	r1, [pc, #304]	; cda0 <linkpc_loop+0x6a8>
    cc6c:	e3a02007 	mov	r2, #7	; 0x7
    cc70:	ebffd17a 	bl	1260 <strncmp>
    cc74:	e3500000 	cmp	r0, #0	; 0x0
    cc78:	1a000002 	bne	cc88 <linkpc_loop+0x590>
    cc7c:	ebfffe89 	bl	c6a8 <__response_ok>
    cc80:	ebffe51b 	bl	60f4 <cdbg_shell>
    cc84:	eafffffe 	b	cc84 <linkpc_loop+0x58c>
    cc88:	e59f1114 	ldr	r1, [pc, #276]	; cda4 <linkpc_loop+0x6ac>
    cc8c:	e1a00007 	mov	r0, r7
    cc90:	eb0004a2 	bl	df20 <sprintf>
    cc94:	e3a01c01 	mov	r1, #256	; 0x100
    cc98:	e1a00007 	mov	r0, r7
    cc9c:	ebffd196 	bl	12fc <strnlen>
    cca0:	e1a01000 	mov	r1, r0
    cca4:	e1a00007 	mov	r0, r7
    cca8:	ebfffd8f 	bl	c2ec <udc_write>
    ccac:	e3500000 	cmp	r0, #0	; 0x0
    ccb0:	0afffea4 	beq	c748 <linkpc_loop+0x50>
    ccb4:	e59f00ec 	ldr	r0, [pc, #236]	; cda8 <linkpc_loop+0x6b0>
    ccb8:	e59f3028 	ldr	r3, [pc, #40]	; cce8 <linkpc_loop+0x5f0>
    ccbc:	e1a0e00f 	mov	lr, pc
    ccc0:	e593f000 	ldr	pc, [r3]
    ccc4:	e3a04000 	mov	r4, #0	; 0x0
    ccc8:	e59f00dc 	ldr	r0, [pc, #220]	; cdac <linkpc_loop+0x6b4>
    cccc:	e59f3014 	ldr	r3, [pc, #20]	; cce8 <linkpc_loop+0x5f0>
    ccd0:	e1a0e00f 	mov	lr, pc
    ccd4:	e593f000 	ldr	pc, [r3]
    ccd8:	e1a00004 	mov	r0, r4
    ccdc:	e28ddf59 	add	sp, sp, #356	; 0x164
    cce0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    cce4:	00013f86 	.word	0x00013f86
    cce8:	0800d76c 	.word	0x0800d76c
    ccec:	000122ae 	.word	0x000122ae
    ccf0:	00013f95 	.word	0x00013f95
    ccf4:	0800d2f0 	.word	0x0800d2f0
    ccf8:	0800d2f4 	.word	0x0800d2f4
    ccfc:	00011d0c 	.word	0x00011d0c
    cd00:	0001187f 	.word	0x0001187f
    cd04:	0800d2f8 	.word	0x0800d2f8
    cd08:	0800d2fc 	.word	0x0800d2fc
    cd0c:	00013fa8 	.word	0x00013fa8
    cd10:	0001185f 	.word	0x0001185f
    cd14:	00013fae 	.word	0x00013fae
    cd18:	00013fd1 	.word	0x00013fd1
    cd1c:	00013fd6 	.word	0x00013fd6
    cd20:	00013fdd 	.word	0x00013fdd
    cd24:	0001188b 	.word	0x0001188b
    cd28:	00013fe5 	.word	0x00013fe5
    cd2c:	00013fed 	.word	0x00013fed
    cd30:	0800d10c 	.word	0x0800d10c
    cd34:	00013ff5 	.word	0x00013ff5
    cd38:	0800d18c 	.word	0x0800d18c
    cd3c:	00013ffb 	.word	0x00013ffb
    cd40:	0800d7d4 	.word	0x0800d7d4
    cd44:	00014004 	.word	0x00014004
    cd48:	00014067 	.word	0x00014067
    cd4c:	00014011 	.word	0x00014011
    cd50:	0001401b 	.word	0x0001401b
    cd54:	0001403a 	.word	0x0001403a
    cd58:	0001406c 	.word	0x0001406c
    cd5c:	00014053 	.word	0x00014053
    cd60:	00014075 	.word	0x00014075
    cd64:	00012136 	.word	0x00012136
    cd68:	0001407c 	.word	0x0001407c
    cd6c:	0800d304 	.word	0x0800d304
    cd70:	0800d300 	.word	0x0800d300
    cd74:	00014094 	.word	0x00014094
    cd78:	00011898 	.word	0x00011898
    cd7c:	000140a4 	.word	0x000140a4
    cd80:	000140bb 	.word	0x000140bb
    cd84:	000140c9 	.word	0x000140c9
    cd88:	000140e2 	.word	0x000140e2
    cd8c:	000140f7 	.word	0x000140f7
    cd90:	00014100 	.word	0x00014100
    cd94:	0001410b 	.word	0x0001410b
    cd98:	00014123 	.word	0x00014123
    cd9c:	0001413e 	.word	0x0001413e
    cda0:	0001414c 	.word	0x0001414c
    cda4:	00014154 	.word	0x00014154
    cda8:	00014168 	.word	0x00014168
    cdac:	0001418b 	.word	0x0001418b

0000cdb0 <print_size>:
    cdb0:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
    cdb4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    cdb8:	e1a05000 	mov	r5, r0
    cdbc:	e1a08001 	mov	r8, r1
    cdc0:	83a04101 	movhi	r4, #1073741824	; 0x40000000
    cdc4:	83a07047 	movhi	r7, #71	; 0x47
    cdc8:	8a000005 	bhi	cde4 <print_size+0x34>
    cdcc:	e59f30a8 	ldr	r3, [pc, #168]	; ce7c <print_size+0xcc>
    cdd0:	e1500003 	cmp	r0, r3
    cdd4:	83a0704d 	movhi	r7, #77	; 0x4d
    cdd8:	93a0706b 	movls	r7, #107	; 0x6b
    cddc:	83a04601 	movhi	r4, #1048576	; 0x100000
    cde0:	93a04b01 	movls	r4, #1024	; 0x400
    cde4:	e1a01004 	mov	r1, r4
    cde8:	e1a00005 	mov	r0, r5
    cdec:	eb000cc3 	bl	10100 <__aeabi_uidiv>
    cdf0:	e1a01004 	mov	r1, r4
    cdf4:	e1a06000 	mov	r6, r0
    cdf8:	e1a00005 	mov	r0, r5
    cdfc:	eb000d3b 	bl	102f0 <__aeabi_uidivmod>
    ce00:	e2510000 	subs	r0, r1, #0	; 0x0
    ce04:	01a04000 	moveq	r4, r0
    ce08:	0a000009 	beq	ce34 <print_size+0x84>
    ce0c:	e0625694 	mls	r2, r4, r6, r5
    ce10:	e1a030a4 	lsr	r3, r4, #1
    ce14:	e3a0000a 	mov	r0, #10	; 0xa
    ce18:	e1a01004 	mov	r1, r4
    ce1c:	e0203290 	mla	r0, r0, r2, r3
    ce20:	eb000cb6 	bl	10100 <__aeabi_uidiv>
    ce24:	e3500009 	cmp	r0, #9	; 0x9
    ce28:	e1a04000 	mov	r4, r0
    ce2c:	82866001 	addhi	r6, r6, #1	; 0x1
    ce30:	8240400a 	subhi	r4, r0, #10	; 0xa
    ce34:	e1a01006 	mov	r1, r6
    ce38:	e59f5040 	ldr	r5, [pc, #64]	; ce80 <print_size+0xd0>
    ce3c:	e59f0040 	ldr	r0, [pc, #64]	; ce84 <print_size+0xd4>
    ce40:	e1a0e00f 	mov	lr, pc
    ce44:	e595f000 	ldr	pc, [r5]
    ce48:	e3540000 	cmp	r4, #0	; 0x0
    ce4c:	0a000003 	beq	ce60 <print_size+0xb0>
    ce50:	e1a01004 	mov	r1, r4
    ce54:	e59f002c 	ldr	r0, [pc, #44]	; ce88 <print_size+0xd8>
    ce58:	e1a0e00f 	mov	lr, pc
    ce5c:	e595f000 	ldr	pc, [r5]
    ce60:	e1a01007 	mov	r1, r7
    ce64:	e1a02008 	mov	r2, r8
    ce68:	e59f001c 	ldr	r0, [pc, #28]	; ce8c <print_size+0xdc>
    ce6c:	e59f300c 	ldr	r3, [pc, #12]	; ce80 <print_size+0xd0>
    ce70:	e1a0e00f 	mov	lr, pc
    ce74:	e593f000 	ldr	pc, [r3]
    ce78:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    ce7c:	000fffff 	.word	0x000fffff
    ce80:	0800d76c 	.word	0x0800d76c
    ce84:	00014193 	.word	0x00014193
    ce88:	00014198 	.word	0x00014198
    ce8c:	0001419d 	.word	0x0001419d

0000ce90 <print_buffer>:
    ce90:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ce94:	e24dd044 	sub	sp, sp, #68	; 0x44
    ce98:	e59d6068 	ldr	r6, [sp, #104]
    ce9c:	e1a07002 	mov	r7, r2
    cea0:	e0020692 	mul	r2, r2, r6
    cea4:	e3520040 	cmp	r2, #64	; 0x40
    cea8:	e1a09000 	mov	r9, r0
    ceac:	e1a0a001 	mov	sl, r1
    ceb0:	e1a08003 	mov	r8, r3
    ceb4:	9a000003 	bls	cec8 <print_buffer+0x38>
    ceb8:	e3a00040 	mov	r0, #64	; 0x40
    cebc:	e1a01007 	mov	r1, r7
    cec0:	eb000c8e 	bl	10100 <__aeabi_uidiv>
    cec4:	e1a06000 	mov	r6, r0
    cec8:	e3560000 	cmp	r6, #0	; 0x0
    cecc:	1a00003c 	bne	cfc4 <print_buffer+0x134>
    ced0:	e3a00010 	mov	r0, #16	; 0x10
    ced4:	e1a01007 	mov	r1, r7
    ced8:	eb000c88 	bl	10100 <__aeabi_uidiv>
    cedc:	e1a06000 	mov	r6, r0
    cee0:	ea000037 	b	cfc4 <print_buffer+0x134>
    cee4:	e59f00f0 	ldr	r0, [pc, #240]	; cfdc <print_buffer+0x14c>
    cee8:	e1a01009 	mov	r1, r9
    ceec:	e1a0e00f 	mov	lr, pc
    cef0:	e59bf000 	ldr	pc, [fp]
    cef4:	e1560008 	cmp	r6, r8
    cef8:	21a06008 	movcs	r6, r8
    cefc:	e1a0500a 	mov	r5, sl
    cf00:	e3a04000 	mov	r4, #0	; 0x0
    cf04:	ea00000f 	b	cf48 <print_buffer+0xb8>
    cf08:	e3570004 	cmp	r7, #4	; 0x4
    cf0c:	05951000 	ldreq	r1, [r5]
    cf10:	059f00c8 	ldreq	r0, [pc, #200]	; cfe0 <print_buffer+0x150>
    cf14:	078d1104 	streq	r1, [sp, r4, lsl #2]
    cf18:	0a000006 	beq	cf38 <print_buffer+0xa8>
    cf1c:	e3570002 	cmp	r7, #2	; 0x2
    cf20:	01d510b0 	ldrheq	r1, [r5]
    cf24:	15d51000 	ldrbne	r1, [r5]
    cf28:	01a03084 	lsleq	r3, r4, #1
    cf2c:	059f00b0 	ldreq	r0, [pc, #176]	; cfe4 <print_buffer+0x154>
    cf30:	018d10b3 	strheq	r1, [sp, r3]
    cf34:	17cd1004 	strbne	r1, [sp, r4]
    cf38:	e1a0e00f 	mov	lr, pc
    cf3c:	e59bf000 	ldr	pc, [fp]
    cf40:	e0855007 	add	r5, r5, r7
    cf44:	e2844001 	add	r4, r4, #1	; 0x1
    cf48:	e1540006 	cmp	r4, r6
    cf4c:	e59f0094 	ldr	r0, [pc, #148]	; cfe8 <print_buffer+0x158>
    cf50:	3affffec 	bcc	cf08 <print_buffer+0x78>
    cf54:	e0050697 	mul	r5, r7, r6
    cf58:	e59f008c 	ldr	r0, [pc, #140]	; cfec <print_buffer+0x15c>
    cf5c:	e08aa005 	add	sl, sl, r5
    cf60:	eb000450 	bl	e0a8 <puts>
    cf64:	e3a04000 	mov	r4, #0	; 0x0
    cf68:	ea000009 	b	cf94 <print_buffer+0x104>
    cf6c:	e7dd0004 	ldrb	r0, [sp, r4]
    cf70:	e59f2078 	ldr	r2, [pc, #120]	; cff0 <print_buffer+0x160>
    cf74:	e2844001 	add	r4, r4, #1	; 0x1
    cf78:	e7d23000 	ldrb	r3, [r2, r0]
    cf7c:	e3130097 	tst	r3, #151	; 0x97
    cf80:	0a000001 	beq	cf8c <print_buffer+0xfc>
    cf84:	e3100080 	tst	r0, #128	; 0x80
    cf88:	0a000000 	beq	cf90 <print_buffer+0x100>
    cf8c:	e3a0002e 	mov	r0, #46	; 0x2e
    cf90:	eb00046c 	bl	e148 <putc>
    cf94:	e1540005 	cmp	r4, r5
    cf98:	3afffff3 	bcc	cf6c <print_buffer+0xdc>
    cf9c:	e59f0050 	ldr	r0, [pc, #80]	; cff4 <print_buffer+0x164>
    cfa0:	e1a0e00f 	mov	lr, pc
    cfa4:	e59bf000 	ldr	pc, [fp]
    cfa8:	eb000469 	bl	e154 <ctrlc>
    cfac:	e3500000 	cmp	r0, #0	; 0x0
    cfb0:	13e00000 	mvnne	r0, #0	; 0x0
    cfb4:	1a000006 	bne	cfd4 <print_buffer+0x144>
    cfb8:	e0899005 	add	r9, r9, r5
    cfbc:	e0668008 	rsb	r8, r6, r8
    cfc0:	ea000000 	b	cfc8 <print_buffer+0x138>
    cfc4:	e59fb02c 	ldr	fp, [pc, #44]	; cff8 <print_buffer+0x168>
    cfc8:	e3580000 	cmp	r8, #0	; 0x0
    cfcc:	1affffc4 	bne	cee4 <print_buffer+0x54>
    cfd0:	e1a00008 	mov	r0, r8
    cfd4:	e28dd044 	add	sp, sp, #68	; 0x44
    cfd8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    cfdc:	000141a4 	.word	0x000141a4
    cfe0:	000132cb 	.word	0x000132cb
    cfe4:	000141ab 	.word	0x000141ab
    cfe8:	000141b1 	.word	0x000141b1
    cfec:	000141b7 	.word	0x000141b7
    cff0:	000118c2 	.word	0x000118c2
    cff4:	00012472 	.word	0x00012472
    cff8:	0800d76c 	.word	0x0800d76c

0000cffc <__div64_32>:
    cffc:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    d000:	e8900300 	ldm	r0, {r8, r9}
    d004:	e1510009 	cmp	r1, r9
    d008:	e1a0a000 	mov	sl, r0
    d00c:	e1a04001 	mov	r4, r1
    d010:	83a06000 	movhi	r6, #0	; 0x0
    d014:	83a07000 	movhi	r7, #0	; 0x0
    d018:	8a000007 	bhi	d03c <__div64_32+0x40>
    d01c:	e1a00009 	mov	r0, r9
    d020:	eb000c36 	bl	10100 <__aeabi_uidiv>
    d024:	e0030094 	mul	r3, r4, r0
    d028:	e3a06000 	mov	r6, #0	; 0x0
    d02c:	e1a02006 	mov	r2, r6
    d030:	e1a07000 	mov	r7, r0
    d034:	e0588002 	subs	r8, r8, r2
    d038:	e0c99003 	sbc	r9, r9, r3
    d03c:	e1a00004 	mov	r0, r4
    d040:	e3a01000 	mov	r1, #0	; 0x0
    d044:	e3a05000 	mov	r5, #0	; 0x0
    d048:	e3a04001 	mov	r4, #1	; 0x1
    d04c:	ea000003 	b	d060 <__div64_32+0x64>
    d050:	e0900000 	adds	r0, r0, r0
    d054:	e0a11001 	adc	r1, r1, r1
    d058:	e0944004 	adds	r4, r4, r4
    d05c:	e0a55005 	adc	r5, r5, r5
    d060:	e3510000 	cmp	r1, #0	; 0x0
    d064:	e3a03000 	mov	r3, #0	; 0x0
    d068:	ca000002 	bgt	d078 <__div64_32+0x7c>
    d06c:	1a000002 	bne	d07c <__div64_32+0x80>
    d070:	e3500000 	cmp	r0, #0	; 0x0
    d074:	0a000000 	beq	d07c <__div64_32+0x80>
    d078:	e3a03001 	mov	r3, #1	; 0x1
    d07c:	e1590001 	cmp	r9, r1
    d080:	e3a02000 	mov	r2, #0	; 0x0
    d084:	8a000002 	bhi	d094 <__div64_32+0x98>
    d088:	1a000002 	bne	d098 <__div64_32+0x9c>
    d08c:	e1580000 	cmp	r8, r0
    d090:	9a000000 	bls	d098 <__div64_32+0x9c>
    d094:	e3a02001 	mov	r2, #1	; 0x1
    d098:	e0033002 	and	r3, r3, r2
    d09c:	e31300ff 	tst	r3, #255	; 0xff
    d0a0:	1affffea 	bne	d050 <__div64_32+0x54>
    d0a4:	e1510009 	cmp	r1, r9
    d0a8:	8a000006 	bhi	d0c8 <__div64_32+0xcc>
    d0ac:	1a000001 	bne	d0b8 <__div64_32+0xbc>
    d0b0:	e1500008 	cmp	r0, r8
    d0b4:	8a000003 	bhi	d0c8 <__div64_32+0xcc>
    d0b8:	e0588000 	subs	r8, r8, r0
    d0bc:	e0c99001 	sbc	r9, r9, r1
    d0c0:	e0966004 	adds	r6, r6, r4
    d0c4:	e0a77005 	adc	r7, r7, r5
    d0c8:	e1b050a5 	lsrs	r5, r5, #1
    d0cc:	e1a04064 	rrx	r4, r4
    d0d0:	e1943005 	orrs	r3, r4, r5
    d0d4:	0a000002 	beq	d0e4 <__div64_32+0xe8>
    d0d8:	e1b010a1 	lsrs	r1, r1, #1
    d0dc:	e1a00060 	rrx	r0, r0
    d0e0:	eaffffef 	b	d0a4 <__div64_32+0xa8>
    d0e4:	e88a00c0 	stm	sl, {r6, r7}
    d0e8:	e1a00008 	mov	r0, r8
    d0ec:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

0000d0f0 <simple_strtoul>:
    d0f0:	e92d4070 	push	{r4, r5, r6, lr}
    d0f4:	e5d03000 	ldrb	r3, [r0]
    d0f8:	e1a05001 	mov	r5, r1
    d0fc:	e3530030 	cmp	r3, #48	; 0x30
    d100:	e1a01002 	mov	r1, r2
    d104:	1a00000d 	bne	d140 <simple_strtoul+0x50>
    d108:	e5f03001 	ldrb	r3, [r0, #1]!
    d10c:	e3530078 	cmp	r3, #120	; 0x78
    d110:	1a000007 	bne	d134 <simple_strtoul+0x44>
    d114:	e5d03001 	ldrb	r3, [r0, #1]
    d118:	e59f2084 	ldr	r2, [pc, #132]	; d1a4 <simple_strtoul+0xb4>
    d11c:	e280c001 	add	ip, r0, #1	; 0x1
    d120:	e7d23003 	ldrb	r3, [r2, r3]
    d124:	e3130044 	tst	r3, #68	; 0x44
    d128:	11a0000c 	movne	r0, ip
    d12c:	13a01010 	movne	r1, #16	; 0x10
    d130:	1a000004 	bne	d148 <simple_strtoul+0x58>
    d134:	e3510000 	cmp	r1, #0	; 0x0
    d138:	03a01008 	moveq	r1, #8	; 0x8
    d13c:	ea000001 	b	d148 <simple_strtoul+0x58>
    d140:	e3520000 	cmp	r2, #0	; 0x0
    d144:	03a0100a 	moveq	r1, #10	; 0xa
    d148:	e3a04000 	mov	r4, #0	; 0x0
    d14c:	e59f6050 	ldr	r6, [pc, #80]	; d1a4 <simple_strtoul+0xb4>
    d150:	ea000001 	b	d15c <simple_strtoul+0x6c>
    d154:	e0243491 	mla	r4, r1, r4, r3
    d158:	e2800001 	add	r0, r0, #1	; 0x1
    d15c:	e5d0c000 	ldrb	ip, [r0]
    d160:	e7d6200c 	ldrb	r2, [r6, ip]
    d164:	e24c3030 	sub	r3, ip, #48	; 0x30
    d168:	e3120044 	tst	r2, #68	; 0x44
    d16c:	0a000008 	beq	d194 <simple_strtoul+0xa4>
    d170:	e3120004 	tst	r2, #4	; 0x4
    d174:	1a000004 	bne	d18c <simple_strtoul+0x9c>
    d178:	e24c3020 	sub	r3, ip, #32	; 0x20
    d17c:	e3120002 	tst	r2, #2	; 0x2
    d180:	e6ef3073 	uxtb	r3, r3
    d184:	12433037 	subne	r3, r3, #55	; 0x37
    d188:	024c3037 	subeq	r3, ip, #55	; 0x37
    d18c:	e1530001 	cmp	r3, r1
    d190:	3affffef 	bcc	d154 <simple_strtoul+0x64>
    d194:	e3550000 	cmp	r5, #0	; 0x0
    d198:	15850000 	strne	r0, [r5]
    d19c:	e1a00004 	mov	r0, r4
    d1a0:	e8bd8070 	pop	{r4, r5, r6, pc}
    d1a4:	000118c2 	.word	0x000118c2

0000d1a8 <simple_strtol>:
    d1a8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    d1ac:	e5d03000 	ldrb	r3, [r0]
    d1b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    d1b4:	e353002d 	cmp	r3, #45	; 0x2d
    d1b8:	1a000004 	bne	d1d0 <simple_strtol+0x28>
    d1bc:	e2800001 	add	r0, r0, #1	; 0x1
    d1c0:	ebffffca 	bl	d0f0 <simple_strtoul>
    d1c4:	e2600000 	rsb	r0, r0, #0	; 0x0
    d1c8:	e28dd004 	add	sp, sp, #4	; 0x4
    d1cc:	e8bd8000 	pop	{pc}
    d1d0:	e28dd004 	add	sp, sp, #4	; 0x4
    d1d4:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    d1d8:	eaffffc4 	b	d0f0 <simple_strtoul>

0000d1dc <ustrtoul>:
    d1dc:	e92d4010 	push	{r4, lr}
    d1e0:	e1a04001 	mov	r4, r1
    d1e4:	ebffffc1 	bl	d0f0 <simple_strtoul>
    d1e8:	e5941000 	ldr	r1, [r4]
    d1ec:	e5d13000 	ldrb	r3, [r1]
    d1f0:	e353004b 	cmp	r3, #75	; 0x4b
    d1f4:	0a00000a 	beq	d224 <ustrtoul+0x48>
    d1f8:	8a000002 	bhi	d208 <ustrtoul+0x2c>
    d1fc:	e3530047 	cmp	r3, #71	; 0x47
    d200:	18bd8010 	popne	{r4, pc}
    d204:	ea000004 	b	d21c <ustrtoul+0x40>
    d208:	e353004d 	cmp	r3, #77	; 0x4d
    d20c:	0a000003 	beq	d220 <ustrtoul+0x44>
    d210:	e353006b 	cmp	r3, #107	; 0x6b
    d214:	18bd8010 	popne	{r4, pc}
    d218:	ea000001 	b	d224 <ustrtoul+0x48>
    d21c:	e1a00500 	lsl	r0, r0, #10
    d220:	e1a00500 	lsl	r0, r0, #10
    d224:	e5d13001 	ldrb	r3, [r1, #1]
    d228:	e1a00500 	lsl	r0, r0, #10
    d22c:	e3530069 	cmp	r3, #105	; 0x69
    d230:	18bd8010 	popne	{r4, pc}
    d234:	e5d13002 	ldrb	r3, [r1, #2]
    d238:	e2812002 	add	r2, r1, #2	; 0x2
    d23c:	e3530042 	cmp	r3, #66	; 0x42
    d240:	02813003 	addeq	r3, r1, #3	; 0x3
    d244:	05843000 	streq	r3, [r4]
    d248:	15842000 	strne	r2, [r4]
    d24c:	e8bd8010 	pop	{r4, pc}

0000d250 <simple_strtoull>:
    d250:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    d254:	e5d03000 	ldrb	r3, [r0]
    d258:	e1a07000 	mov	r7, r0
    d25c:	e3530030 	cmp	r3, #48	; 0x30
    d260:	e1a08001 	mov	r8, r1
    d264:	e1a06002 	mov	r6, r2
    d268:	1a00000d 	bne	d2a4 <simple_strtoull+0x54>
    d26c:	e5f73001 	ldrb	r3, [r7, #1]!
    d270:	e3530078 	cmp	r3, #120	; 0x78
    d274:	1a000007 	bne	d298 <simple_strtoull+0x48>
    d278:	e5d73001 	ldrb	r3, [r7, #1]
    d27c:	e59f20ac 	ldr	r2, [pc, #172]	; d330 <simple_strtoull+0xe0>
    d280:	e2870001 	add	r0, r7, #1	; 0x1
    d284:	e7d23003 	ldrb	r3, [r2, r3]
    d288:	e3130044 	tst	r3, #68	; 0x44
    d28c:	11a07000 	movne	r7, r0
    d290:	13a06010 	movne	r6, #16	; 0x10
    d294:	1a000004 	bne	d2ac <simple_strtoull+0x5c>
    d298:	e3560000 	cmp	r6, #0	; 0x0
    d29c:	03a06008 	moveq	r6, #8	; 0x8
    d2a0:	ea000001 	b	d2ac <simple_strtoull+0x5c>
    d2a4:	e3520000 	cmp	r2, #0	; 0x0
    d2a8:	03a0600a 	moveq	r6, #10	; 0xa
    d2ac:	e3a04000 	mov	r4, #0	; 0x0
    d2b0:	e3a05000 	mov	r5, #0	; 0x0
    d2b4:	e59f9074 	ldr	r9, [pc, #116]	; d330 <simple_strtoull+0xe0>
    d2b8:	e3a0a000 	mov	sl, #0	; 0x0
    d2bc:	ea000004 	b	d2d4 <simple_strtoull+0x84>
    d2c0:	e0832694 	umull	r2, r3, r4, r6
    d2c4:	e0233596 	mla	r3, r6, r5, r3
    d2c8:	e0904002 	adds	r4, r0, r2
    d2cc:	e0a15003 	adc	r5, r1, r3
    d2d0:	e2877001 	add	r7, r7, #1	; 0x1
    d2d4:	e5d7c000 	ldrb	ip, [r7]
    d2d8:	e7d9200c 	ldrb	r2, [r9, ip]
    d2dc:	e24c3030 	sub	r3, ip, #48	; 0x30
    d2e0:	e3120044 	tst	r2, #68	; 0x44
    d2e4:	0a00000c 	beq	d31c <simple_strtoull+0xcc>
    d2e8:	e3120004 	tst	r2, #4	; 0x4
    d2ec:	1a000004 	bne	d304 <simple_strtoull+0xb4>
    d2f0:	e24c3020 	sub	r3, ip, #32	; 0x20
    d2f4:	e3120002 	tst	r2, #2	; 0x2
    d2f8:	e6ef3073 	uxtb	r3, r3
    d2fc:	12433037 	subne	r3, r3, #55	; 0x37
    d300:	024c3037 	subeq	r3, ip, #55	; 0x37
    d304:	e1a00003 	mov	r0, r3
    d308:	e1a01fc0 	asr	r1, r0, #31
    d30c:	e15a0001 	cmp	sl, r1
    d310:	1a000001 	bne	d31c <simple_strtoull+0xcc>
    d314:	e1560000 	cmp	r6, r0
    d318:	8affffe8 	bhi	d2c0 <simple_strtoull+0x70>
    d31c:	e3580000 	cmp	r8, #0	; 0x0
    d320:	15887000 	strne	r7, [r8]
    d324:	e1a01005 	mov	r1, r5
    d328:	e1a00004 	mov	r0, r4
    d32c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    d330:	000118c2 	.word	0x000118c2

0000d334 <put_dec_trunc>:
    d334:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    d338:	e1a03421 	lsr	r3, r1, #8
    d33c:	e1a04221 	lsr	r4, r1, #4
    d340:	e204400f 	and	r4, r4, #15	; 0xf
    d344:	e203600f 	and	r6, r3, #15	; 0xf
    d348:	e1a05621 	lsr	r5, r1, #12
    d34c:	e0842006 	add	r2, r4, r6
    d350:	e201100f 	and	r1, r1, #15	; 0xf
    d354:	e3a03006 	mov	r3, #6	; 0x6
    d358:	e0822005 	add	r2, r2, r5
    d35c:	e0221293 	mla	r2, r3, r2, r1
    d360:	e3a080cd 	mov	r8, #205	; 0xcd
    d364:	e00c0298 	mul	ip, r8, r2
    d368:	e3a01009 	mov	r1, #9	; 0x9
    d36c:	e0214591 	mla	r1, r1, r5, r4
    d370:	e1a0c5ac 	lsr	ip, ip, #11
    d374:	e3a0700a 	mov	r7, #10	; 0xa
    d378:	e3a03005 	mov	r3, #5	; 0x5
    d37c:	e0231693 	mla	r3, r3, r6, r1
    d380:	e0622c97 	mls	r2, r7, ip, r2
    d384:	e093100c 	adds	r1, r3, ip
    d388:	e2822030 	add	r2, r2, #48	; 0x30
    d38c:	e1a04000 	mov	r4, r0
    d390:	e4c02001 	strb	r2, [r0], #1
    d394:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    d398:	e0030198 	mul	r3, r8, r1
    d39c:	e1a035a3 	lsr	r3, r3, #11
    d3a0:	e0621397 	mls	r2, r7, r3, r1
    d3a4:	e0831086 	add	r1, r3, r6, lsl #1
    d3a8:	e2822030 	add	r2, r2, #48	; 0x30
    d3ac:	e1913005 	orrs	r3, r1, r5
    d3b0:	e5c42001 	strb	r2, [r4, #1]
    d3b4:	e2800001 	add	r0, r0, #1	; 0x1
    d3b8:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    d3bc:	e3a0300d 	mov	r3, #13	; 0xd
    d3c0:	e0030193 	mul	r3, r3, r1
    d3c4:	e1a033a3 	lsr	r3, r3, #7
    d3c8:	e0621397 	mls	r2, r7, r3, r1
    d3cc:	e0931105 	adds	r1, r3, r5, lsl #2
    d3d0:	e2822030 	add	r2, r2, #48	; 0x30
    d3d4:	e4c02001 	strb	r2, [r0], #1
    d3d8:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
    d3dc:	e0030198 	mul	r3, r8, r1
    d3e0:	e1a0c5a3 	lsr	ip, r3, #11
    d3e4:	e0631c97 	mls	r3, r7, ip, r1
    d3e8:	e35c0000 	cmp	ip, #0	; 0x0
    d3ec:	e2833030 	add	r3, r3, #48	; 0x30
    d3f0:	e4c03001 	strb	r3, [r0], #1
    d3f4:	128c3030 	addne	r3, ip, #48	; 0x30
    d3f8:	14c03001 	strbne	r3, [r0], #1
    d3fc:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000d400 <string>:
    d400:	e59fc0a4 	ldr	ip, [pc, #164]	; d4ac <string+0xac>
    d404:	e3510000 	cmp	r1, #0	; 0x0
    d408:	e92d4070 	push	{r4, r5, r6, lr}
    d40c:	11a06001 	movne	r6, r1
    d410:	01a0600c 	moveq	r6, ip
    d414:	e1a01003 	mov	r1, r3
    d418:	e1a04000 	mov	r4, r0
    d41c:	e1a00006 	mov	r0, r6
    d420:	e1a05002 	mov	r5, r2
    d424:	ebffcfb4 	bl	12fc <strnlen>
    d428:	e59d3010 	ldr	r3, [sp, #16]
    d42c:	e1a0c000 	mov	ip, r0
    d430:	e3130010 	tst	r3, #16	; 0x10
    d434:	01a02004 	moveq	r2, r4
    d438:	03a01020 	moveq	r1, #32	; 0x20
    d43c:	0a000001 	beq	d448 <string+0x48>
    d440:	ea000006 	b	d460 <string+0x60>
    d444:	e5421001 	strb	r1, [r2, #-1]
    d448:	e2455001 	sub	r5, r5, #1	; 0x1
    d44c:	e2853001 	add	r3, r5, #1	; 0x1
    d450:	e15c0003 	cmp	ip, r3
    d454:	e1a04002 	mov	r4, r2
    d458:	e2822001 	add	r2, r2, #1	; 0x1
    d45c:	bafffff8 	blt	d444 <string+0x44>
    d460:	e3a02000 	mov	r2, #0	; 0x0
    d464:	ea000002 	b	d474 <string+0x74>
    d468:	e7d63002 	ldrb	r3, [r6, r2]
    d46c:	e7c43002 	strb	r3, [r4, r2]
    d470:	e2822001 	add	r2, r2, #1	; 0x1
    d474:	e152000c 	cmp	r2, ip
    d478:	e0843002 	add	r3, r4, r2
    d47c:	bafffff9 	blt	d468 <string+0x68>
    d480:	e1a01003 	mov	r1, r3
    d484:	e1a02005 	mov	r2, r5
    d488:	e3a03020 	mov	r3, #32	; 0x20
    d48c:	ea000000 	b	d494 <string+0x94>
    d490:	e5413001 	strb	r3, [r1, #-1]
    d494:	e15c0002 	cmp	ip, r2
    d498:	e1a00001 	mov	r0, r1
    d49c:	e2422001 	sub	r2, r2, #1	; 0x1
    d4a0:	e2811001 	add	r1, r1, #1	; 0x1
    d4a4:	bafffff9 	blt	d490 <string+0x90>
    d4a8:	e8bd8070 	pop	{r4, r5, r6, pc}
    d4ac:	000141bc 	.word	0x000141bc

0000d4b0 <put_dec>:
    d4b0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d4b4:	e24dd00c 	sub	sp, sp, #12	; 0xc
    d4b8:	e28d8008 	add	r8, sp, #8	; 0x8
    d4bc:	e16820f8 	strd	r2, [r8, #-8]!
    d4c0:	e1a07000 	mov	r7, r0
    d4c4:	e3a090cd 	mov	r9, #205	; 0xcd
    d4c8:	e3a0a00a 	mov	sl, #10	; 0xa
    d4cc:	e3a0b00d 	mov	fp, #13	; 0xd
    d4d0:	e59d5004 	ldr	r5, [sp, #4]
    d4d4:	e1a0000d 	mov	r0, sp
    d4d8:	e3550000 	cmp	r5, #0	; 0x0
    d4dc:	e59f10f0 	ldr	r1, [pc, #240]	; d5d4 <put_dec+0x124>
    d4e0:	e59d4000 	ldr	r4, [sp]
    d4e4:	1a00000f 	bne	d528 <put_dec+0x78>
    d4e8:	e59f30e8 	ldr	r3, [pc, #232]	; d5d8 <put_dec+0x128>
    d4ec:	e1a00004 	mov	r0, r4
    d4f0:	e1540003 	cmp	r4, r3
    d4f4:	8a000004 	bhi	d50c <put_dec+0x5c>
    d4f8:	e1a00007 	mov	r0, r7
    d4fc:	e1a01004 	mov	r1, r4
    d500:	ebffff8b 	bl	d334 <put_dec_trunc>
    d504:	e28dd00c 	add	sp, sp, #12	; 0xc
    d508:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d50c:	eb000b77 	bl	102f0 <__aeabi_uidivmod>
    d510:	e1a00004 	mov	r0, r4
    d514:	e1a06001 	mov	r6, r1
    d518:	e59f10b4 	ldr	r1, [pc, #180]	; d5d4 <put_dec+0x124>
    d51c:	eb000af7 	bl	10100 <__aeabi_uidiv>
    d520:	e88d0021 	stm	sp, {r0, r5}
    d524:	ea000001 	b	d530 <put_dec+0x80>
    d528:	ebfffeb3 	bl	cffc <__div64_32>
    d52c:	e1a06000 	mov	r6, r0
    d530:	e1a02226 	lsr	r2, r6, #4
    d534:	e1a0c426 	lsr	ip, r6, #8
    d538:	e20cc00f 	and	ip, ip, #15	; 0xf
    d53c:	e202200f 	and	r2, r2, #15	; 0xf
    d540:	e1a0e626 	lsr	lr, r6, #12
    d544:	e082500c 	add	r5, r2, ip
    d548:	e085500e 	add	r5, r5, lr
    d54c:	e206300f 	and	r3, r6, #15	; 0xf
    d550:	e3a01006 	mov	r1, #6	; 0x6
    d554:	e0253591 	mla	r5, r1, r5, r3
    d558:	e3a03009 	mov	r3, #9	; 0x9
    d55c:	e0222e93 	mla	r2, r3, lr, r2
    d560:	e3a01005 	mov	r1, #5	; 0x5
    d564:	e0000599 	mul	r0, r9, r5
    d568:	e0222c91 	mla	r2, r1, ip, r2
    d56c:	e1a005a0 	lsr	r0, r0, #11
    d570:	e0822000 	add	r2, r2, r0
    d574:	e0010299 	mul	r1, r9, r2
    d578:	e1a015a1 	lsr	r1, r1, #11
    d57c:	e081c08c 	add	ip, r1, ip, lsl #1
    d580:	e0030c9b 	mul	r3, fp, ip
    d584:	e1a033a3 	lsr	r3, r3, #7
    d588:	e083e10e 	add	lr, r3, lr, lsl #2
    d58c:	e0040e99 	mul	r4, r9, lr
    d590:	e1a045a4 	lsr	r4, r4, #11
    d594:	e06ee49a 	mls	lr, sl, r4, lr
    d598:	e060509a 	mls	r0, sl, r0, r5
    d59c:	e061219a 	mls	r1, sl, r1, r2
    d5a0:	e063c39a 	mls	r3, sl, r3, ip
    d5a4:	e2800030 	add	r0, r0, #48	; 0x30
    d5a8:	e2811030 	add	r1, r1, #48	; 0x30
    d5ac:	e2833030 	add	r3, r3, #48	; 0x30
    d5b0:	e28ee030 	add	lr, lr, #48	; 0x30
    d5b4:	e2844030 	add	r4, r4, #48	; 0x30
    d5b8:	e5c70000 	strb	r0, [r7]
    d5bc:	e5c71001 	strb	r1, [r7, #1]
    d5c0:	e5c73002 	strb	r3, [r7, #2]
    d5c4:	e5c7e003 	strb	lr, [r7, #3]
    d5c8:	e5c74004 	strb	r4, [r7, #4]
    d5cc:	e2877005 	add	r7, r7, #5	; 0x5
    d5d0:	eaffffbe 	b	d4d0 <put_dec+0x20>
    d5d4:	000186a0 	.word	0x000186a0
    d5d8:	0001869f 	.word	0x0001869f

0000d5dc <number>:
    d5dc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d5e0:	e24dd05c 	sub	sp, sp, #92	; 0x5c
    d5e4:	e1a0a000 	mov	sl, r0
    d5e8:	e59d0080 	ldr	r0, [sp, #128]
    d5ec:	e59d808c 	ldr	r8, [sp, #140]
    d5f0:	e250100a 	subs	r1, r0, #10	; 0xa
    d5f4:	13a01001 	movne	r1, #1	; 0x1
    d5f8:	e0011328 	and	r1, r1, r8, lsr #6
    d5fc:	e3180010 	tst	r8, #16	; 0x10
    d600:	e58d1008 	str	r1, [sp, #8]
    d604:	e2081020 	and	r1, r8, #32	; 0x20
    d608:	13c88001 	bicne	r8, r8, #1	; 0x1
    d60c:	e3180002 	tst	r8, #2	; 0x2
    d610:	e1a06002 	mov	r6, r2
    d614:	e1a07003 	mov	r7, r3
    d618:	e59d5084 	ldr	r5, [sp, #132]
    d61c:	e58d1004 	str	r1, [sp, #4]
    d620:	0a000010 	beq	d668 <number+0x8c>
    d624:	e3530000 	cmp	r3, #0	; 0x0
    d628:	ba000002 	blt	d638 <number+0x5c>
    d62c:	e3180004 	tst	r8, #4	; 0x4
    d630:	0a000008 	beq	d658 <number+0x7c>
    d634:	ea000004 	b	d64c <number+0x70>
    d638:	e2766000 	rsbs	r6, r6, #0	; 0x0
    d63c:	e2e77000 	rsc	r7, r7, #0	; 0x0
    d640:	e2455001 	sub	r5, r5, #1	; 0x1
    d644:	e3a0902d 	mov	r9, #45	; 0x2d
    d648:	ea000007 	b	d66c <number+0x90>
    d64c:	e2455001 	sub	r5, r5, #1	; 0x1
    d650:	e3a0902b 	mov	r9, #43	; 0x2b
    d654:	ea000004 	b	d66c <number+0x90>
    d658:	e3180008 	tst	r8, #8	; 0x8
    d65c:	12455001 	subne	r5, r5, #1	; 0x1
    d660:	13a09020 	movne	r9, #32	; 0x20
    d664:	1a000000 	bne	d66c <number+0x90>
    d668:	e3a09000 	mov	r9, #0	; 0x0
    d66c:	e59d2008 	ldr	r2, [sp, #8]
    d670:	e3520000 	cmp	r2, #0	; 0x0
    d674:	0a000003 	beq	d688 <number+0xac>
    d678:	e59d3080 	ldr	r3, [sp, #128]
    d67c:	e2455001 	sub	r5, r5, #1	; 0x1
    d680:	e3530010 	cmp	r3, #16	; 0x10
    d684:	02455001 	subeq	r5, r5, #1	; 0x1
    d688:	e196c007 	orrs	ip, r6, r7
    d68c:	03a03030 	moveq	r3, #48	; 0x30
    d690:	05cd3016 	strbeq	r3, [sp, #22]
    d694:	03a04001 	moveq	r4, #1	; 0x1
    d698:	0a000020 	beq	d720 <number+0x144>
    d69c:	e59d0080 	ldr	r0, [sp, #128]
    d6a0:	e350000a 	cmp	r0, #10	; 0xa
    d6a4:	0a000017 	beq	d708 <number+0x12c>
    d6a8:	e3500010 	cmp	r0, #16	; 0x10
    d6ac:	03a01004 	moveq	r1, #4	; 0x4
    d6b0:	13a01003 	movne	r1, #3	; 0x3
    d6b4:	e58d100c 	str	r1, [sp, #12]
    d6b8:	e240b001 	sub	fp, r0, #1	; 0x1
    d6bc:	e3a04000 	mov	r4, #0	; 0x0
    d6c0:	e6ef3076 	uxtb	r3, r6
    d6c4:	e59f2164 	ldr	r2, [pc, #356]	; d830 <number+0x254>
    d6c8:	e003300b 	and	r3, r3, fp
    d6cc:	e59dc004 	ldr	ip, [sp, #4]
    d6d0:	e7d23003 	ldrb	r3, [r2, r3]
    d6d4:	e1a00006 	mov	r0, r6
    d6d8:	e18c3003 	orr	r3, ip, r3
    d6dc:	e28dc016 	add	ip, sp, #22	; 0x16
    d6e0:	e1a01007 	mov	r1, r7
    d6e4:	e59d200c 	ldr	r2, [sp, #12]
    d6e8:	e7cc3004 	strb	r3, [ip, r4]
    d6ec:	eb000b93 	bl	10540 <__aeabi_llsr>
    d6f0:	e1a06000 	mov	r6, r0
    d6f4:	e1960001 	orrs	r0, r6, r1
    d6f8:	e1a07001 	mov	r7, r1
    d6fc:	e2844001 	add	r4, r4, #1	; 0x1
    d700:	1affffee 	bne	d6c0 <number+0xe4>
    d704:	ea000005 	b	d720 <number+0x144>
    d708:	e28d4016 	add	r4, sp, #22	; 0x16
    d70c:	e1a00004 	mov	r0, r4
    d710:	e1a02006 	mov	r2, r6
    d714:	e1a03007 	mov	r3, r7
    d718:	ebffff64 	bl	d4b0 <put_dec>
    d71c:	e0644000 	rsb	r4, r4, r0
    d720:	e59d3088 	ldr	r3, [sp, #136]
    d724:	e1540003 	cmp	r4, r3
    d728:	a1a01004 	movge	r1, r4
    d72c:	b1a01003 	movlt	r1, r3
    d730:	e3180011 	tst	r8, #17	; 0x11
    d734:	e061c005 	rsb	ip, r1, r5
    d738:	01a0300a 	moveq	r3, sl
    d73c:	03a02020 	moveq	r2, #32	; 0x20
    d740:	0a000001 	beq	d74c <number+0x170>
    d744:	ea000004 	b	d75c <number+0x180>
    d748:	e5432001 	strb	r2, [r3, #-1]
    d74c:	e25cc001 	subs	ip, ip, #1	; 0x1
    d750:	e1a0a003 	mov	sl, r3
    d754:	e2833001 	add	r3, r3, #1	; 0x1
    d758:	5afffffa 	bpl	d748 <number+0x16c>
    d75c:	e3590000 	cmp	r9, #0	; 0x0
    d760:	14ca9001 	strbne	r9, [sl], #1
    d764:	e59d2008 	ldr	r2, [sp, #8]
    d768:	e3520000 	cmp	r2, #0	; 0x0
    d76c:	0a000006 	beq	d78c <number+0x1b0>
    d770:	e59d3080 	ldr	r3, [sp, #128]
    d774:	e3530010 	cmp	r3, #16	; 0x10
    d778:	e3a03030 	mov	r3, #48	; 0x30
    d77c:	e4ca3001 	strb	r3, [sl], #1
    d780:	059d0004 	ldreq	r0, [sp, #4]
    d784:	03803058 	orreq	r3, r0, #88	; 0x58
    d788:	04ca3001 	strbeq	r3, [sl], #1
    d78c:	e3180010 	tst	r8, #16	; 0x10
    d790:	1a000009 	bne	d7bc <number+0x1e0>
    d794:	e3180001 	tst	r8, #1	; 0x1
    d798:	13a02030 	movne	r2, #48	; 0x30
    d79c:	03a02020 	moveq	r2, #32	; 0x20
    d7a0:	e1a0300a 	mov	r3, sl
    d7a4:	ea000000 	b	d7ac <number+0x1d0>
    d7a8:	e5432001 	strb	r2, [r3, #-1]
    d7ac:	e25cc001 	subs	ip, ip, #1	; 0x1
    d7b0:	e1a0a003 	mov	sl, r3
    d7b4:	e2833001 	add	r3, r3, #1	; 0x1
    d7b8:	5afffffa 	bpl	d7a8 <number+0x1cc>
    d7bc:	e1a0000a 	mov	r0, sl
    d7c0:	e3a03030 	mov	r3, #48	; 0x30
    d7c4:	ea000000 	b	d7cc <number+0x1f0>
    d7c8:	e5403001 	strb	r3, [r0, #-1]
    d7cc:	e2411001 	sub	r1, r1, #1	; 0x1
    d7d0:	e1540001 	cmp	r4, r1
    d7d4:	e1a02000 	mov	r2, r0
    d7d8:	e2800001 	add	r0, r0, #1	; 0x1
    d7dc:	dafffff9 	ble	d7c8 <number+0x1ec>
    d7e0:	e28d1058 	add	r1, sp, #88	; 0x58
    d7e4:	e0813004 	add	r3, r1, r4
    d7e8:	e2431043 	sub	r1, r3, #67	; 0x43
    d7ec:	ea000001 	b	d7f8 <number+0x21c>
    d7f0:	e5d13001 	ldrb	r3, [r1, #1]
    d7f4:	e5423001 	strb	r3, [r2, #-1]
    d7f8:	e2544001 	subs	r4, r4, #1	; 0x1
    d7fc:	e1a03002 	mov	r3, r2
    d800:	e2411001 	sub	r1, r1, #1	; 0x1
    d804:	e2822001 	add	r2, r2, #1	; 0x1
    d808:	5afffff8 	bpl	d7f0 <number+0x214>
    d80c:	e3a02020 	mov	r2, #32	; 0x20
    d810:	ea000000 	b	d818 <number+0x23c>
    d814:	e5432001 	strb	r2, [r3, #-1]
    d818:	e25cc001 	subs	ip, ip, #1	; 0x1
    d81c:	e1a00003 	mov	r0, r3
    d820:	e2833001 	add	r3, r3, #1	; 0x1
    d824:	5afffffa 	bpl	d814 <number+0x238>
    d828:	e28dd05c 	add	sp, sp, #92	; 0x5c
    d82c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d830:	000118b2 	.word	0x000118b2

0000d834 <vsprintf>:
    d834:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    d838:	e24dd084 	sub	sp, sp, #132	; 0x84
    d83c:	e58d0028 	str	r0, [sp, #40]
    d840:	e1a08001 	mov	r8, r1
    d844:	e1a0a000 	mov	sl, r0
    d848:	e28d107b 	add	r1, sp, #123	; 0x7b
    d84c:	e28d007d 	add	r0, sp, #125	; 0x7d
    d850:	e58d2024 	str	r2, [sp, #36]
    d854:	e1a06002 	mov	r6, r2
    d858:	e58d001c 	str	r0, [sp, #28]
    d85c:	e58d1018 	str	r1, [sp, #24]
    d860:	ea0001a3 	b	def4 <vsprintf+0x6c0>
    d864:	e3530025 	cmp	r3, #37	; 0x25
    d868:	1a00015f 	bne	ddec <vsprintf+0x5b8>
    d86c:	e3a07000 	mov	r7, #0	; 0x0
    d870:	e5f82001 	ldrb	r2, [r8, #1]!
    d874:	e352002b 	cmp	r2, #43	; 0x2b
    d878:	0a00000c 	beq	d8b0 <vsprintf+0x7c>
    d87c:	8a000004 	bhi	d894 <vsprintf+0x60>
    d880:	e3520020 	cmp	r2, #32	; 0x20
    d884:	0a00000b 	beq	d8b8 <vsprintf+0x84>
    d888:	e3520023 	cmp	r2, #35	; 0x23
    d88c:	1a00000f 	bne	d8d0 <vsprintf+0x9c>
    d890:	ea00000a 	b	d8c0 <vsprintf+0x8c>
    d894:	e352002d 	cmp	r2, #45	; 0x2d
    d898:	0a000002 	beq	d8a8 <vsprintf+0x74>
    d89c:	e3520030 	cmp	r2, #48	; 0x30
    d8a0:	1a00000a 	bne	d8d0 <vsprintf+0x9c>
    d8a4:	ea000007 	b	d8c8 <vsprintf+0x94>
    d8a8:	e3877010 	orr	r7, r7, #16	; 0x10
    d8ac:	eaffffef 	b	d870 <vsprintf+0x3c>
    d8b0:	e3877004 	orr	r7, r7, #4	; 0x4
    d8b4:	eaffffed 	b	d870 <vsprintf+0x3c>
    d8b8:	e3877008 	orr	r7, r7, #8	; 0x8
    d8bc:	eaffffeb 	b	d870 <vsprintf+0x3c>
    d8c0:	e3877040 	orr	r7, r7, #64	; 0x40
    d8c4:	eaffffe9 	b	d870 <vsprintf+0x3c>
    d8c8:	e3877001 	orr	r7, r7, #1	; 0x1
    d8cc:	eaffffe7 	b	d870 <vsprintf+0x3c>
    d8d0:	e2423030 	sub	r3, r2, #48	; 0x30
    d8d4:	e6ef3073 	uxtb	r3, r3
    d8d8:	e3530009 	cmp	r3, #9	; 0x9
    d8dc:	93a09000 	movls	r9, #0	; 0x0
    d8e0:	93a0100a 	movls	r1, #10	; 0xa
    d8e4:	9a000002 	bls	d8f4 <vsprintf+0xc0>
    d8e8:	ea000007 	b	d90c <vsprintf+0xd8>
    d8ec:	e0292991 	mla	r9, r1, r9, r2
    d8f0:	e2888001 	add	r8, r8, #1	; 0x1
    d8f4:	e5d83000 	ldrb	r3, [r8]
    d8f8:	e2432030 	sub	r2, r3, #48	; 0x30
    d8fc:	e6ef3072 	uxtb	r3, r2
    d900:	e3530009 	cmp	r3, #9	; 0x9
    d904:	9afffff8 	bls	d8ec <vsprintf+0xb8>
    d908:	ea000008 	b	d930 <vsprintf+0xfc>
    d90c:	e352002a 	cmp	r2, #42	; 0x2a
    d910:	13e09000 	mvnne	r9, #0	; 0x0
    d914:	1a000005 	bne	d930 <vsprintf+0xfc>
    d918:	e5969000 	ldr	r9, [r6]
    d91c:	e2888001 	add	r8, r8, #1	; 0x1
    d920:	e3590000 	cmp	r9, #0	; 0x0
    d924:	e2866004 	add	r6, r6, #4	; 0x4
    d928:	b2699000 	rsblt	r9, r9, #0	; 0x0
    d92c:	b3877010 	orrlt	r7, r7, #16	; 0x10
    d930:	e5d83000 	ldrb	r3, [r8]
    d934:	e353002e 	cmp	r3, #46	; 0x2e
    d938:	13e02000 	mvnne	r2, #0	; 0x0
    d93c:	158d202c 	strne	r2, [sp, #44]
    d940:	1a00001d 	bne	d9bc <vsprintf+0x188>
    d944:	e5f82001 	ldrb	r2, [r8, #1]!
    d948:	e2423030 	sub	r3, r2, #48	; 0x30
    d94c:	e6ef3073 	uxtb	r3, r3
    d950:	e3530009 	cmp	r3, #9	; 0x9
    d954:	93a03000 	movls	r3, #0	; 0x0
    d958:	958d302c 	strls	r3, [sp, #44]
    d95c:	93a0100a 	movls	r1, #10	; 0xa
    d960:	9a000004 	bls	d978 <vsprintf+0x144>
    d964:	ea000009 	b	d990 <vsprintf+0x15c>
    d968:	e59d002c 	ldr	r0, [sp, #44]
    d96c:	e2888001 	add	r8, r8, #1	; 0x1
    d970:	e0202091 	mla	r0, r1, r0, r2
    d974:	e58d002c 	str	r0, [sp, #44]
    d978:	e5d83000 	ldrb	r3, [r8]
    d97c:	e2432030 	sub	r2, r3, #48	; 0x30
    d980:	e6ef3072 	uxtb	r3, r2
    d984:	e3530009 	cmp	r3, #9	; 0x9
    d988:	9afffff6 	bls	d968 <vsprintf+0x134>
    d98c:	ea000005 	b	d9a8 <vsprintf+0x174>
    d990:	e352002a 	cmp	r2, #42	; 0x2a
    d994:	1a000006 	bne	d9b4 <vsprintf+0x180>
    d998:	e5961000 	ldr	r1, [r6]
    d99c:	e2888001 	add	r8, r8, #1	; 0x1
    d9a0:	e58d102c 	str	r1, [sp, #44]
    d9a4:	e2866004 	add	r6, r6, #4	; 0x4
    d9a8:	e59d202c 	ldr	r2, [sp, #44]
    d9ac:	e3520000 	cmp	r2, #0	; 0x0
    d9b0:	aa000001 	bge	d9bc <vsprintf+0x188>
    d9b4:	e3a03000 	mov	r3, #0	; 0x0
    d9b8:	e58d302c 	str	r3, [sp, #44]
    d9bc:	e5d83000 	ldrb	r3, [r8]
    d9c0:	e353006c 	cmp	r3, #108	; 0x6c
    d9c4:	13530068 	cmpne	r3, #104	; 0x68
    d9c8:	0a000008 	beq	d9f0 <vsprintf+0x1bc>
    d9cc:	e353004c 	cmp	r3, #76	; 0x4c
    d9d0:	0a000006 	beq	d9f0 <vsprintf+0x1bc>
    d9d4:	e353005a 	cmp	r3, #90	; 0x5a
    d9d8:	0a000004 	beq	d9f0 <vsprintf+0x1bc>
    d9dc:	e353007a 	cmp	r3, #122	; 0x7a
    d9e0:	0a000002 	beq	d9f0 <vsprintf+0x1bc>
    d9e4:	e3530074 	cmp	r3, #116	; 0x74
    d9e8:	13e02000 	mvnne	r2, #0	; 0x0
    d9ec:	1a000007 	bne	da10 <vsprintf+0x1dc>
    d9f0:	e353006c 	cmp	r3, #108	; 0x6c
    d9f4:	e1a02003 	mov	r2, r3
    d9f8:	e2888001 	add	r8, r8, #1	; 0x1
    d9fc:	1a000003 	bne	da10 <vsprintf+0x1dc>
    da00:	e5d83000 	ldrb	r3, [r8]
    da04:	e353006c 	cmp	r3, #108	; 0x6c
    da08:	02888001 	addeq	r8, r8, #1	; 0x1
    da0c:	03a0204c 	moveq	r2, #76	; 0x4c
    da10:	e5d83000 	ldrb	r3, [r8]
    da14:	e353006e 	cmp	r3, #110	; 0x6e
    da18:	0a0000df 	beq	dd9c <vsprintf+0x568>
    da1c:	8a00000c 	bhi	da54 <vsprintf+0x220>
    da20:	e3530063 	cmp	r3, #99	; 0x63
    da24:	0a000017 	beq	da88 <vsprintf+0x254>
    da28:	8a000004 	bhi	da40 <vsprintf+0x20c>
    da2c:	e3530025 	cmp	r3, #37	; 0x25
    da30:	0a0000ed 	beq	ddec <vsprintf+0x5b8>
    da34:	e3530058 	cmp	r3, #88	; 0x58
    da38:	1a0000e5 	bne	ddd4 <vsprintf+0x5a0>
    da3c:	ea0000e0 	b	ddc4 <vsprintf+0x590>
    da40:	e3530064 	cmp	r3, #100	; 0x64
    da44:	0a0000e0 	beq	ddcc <vsprintf+0x598>
    da48:	e3530069 	cmp	r3, #105	; 0x69
    da4c:	1a0000e0 	bne	ddd4 <vsprintf+0x5a0>
    da50:	ea0000dd 	b	ddcc <vsprintf+0x598>
    da54:	e3530073 	cmp	r3, #115	; 0x73
    da58:	0a000021 	beq	dae4 <vsprintf+0x2b0>
    da5c:	8a000004 	bhi	da74 <vsprintf+0x240>
    da60:	e353006f 	cmp	r3, #111	; 0x6f
    da64:	0a0000d3 	beq	ddb8 <vsprintf+0x584>
    da68:	e3530070 	cmp	r3, #112	; 0x70
    da6c:	1a0000d8 	bne	ddd4 <vsprintf+0x5a0>
    da70:	ea000023 	b	db04 <vsprintf+0x2d0>
    da74:	e3530075 	cmp	r3, #117	; 0x75
    da78:	0a0000dd 	beq	ddf4 <vsprintf+0x5c0>
    da7c:	e3530078 	cmp	r3, #120	; 0x78
    da80:	1a0000d3 	bne	ddd4 <vsprintf+0x5a0>
    da84:	ea0000cd 	b	ddc0 <vsprintf+0x58c>
    da88:	e3170010 	tst	r7, #16	; 0x10
    da8c:	01a0300a 	moveq	r3, sl
    da90:	03a02020 	moveq	r2, #32	; 0x20
    da94:	0a000001 	beq	daa0 <vsprintf+0x26c>
    da98:	ea000005 	b	dab4 <vsprintf+0x280>
    da9c:	e5432001 	strb	r2, [r3, #-1]
    daa0:	e2499001 	sub	r9, r9, #1	; 0x1
    daa4:	e3590000 	cmp	r9, #0	; 0x0
    daa8:	e1a0a003 	mov	sl, r3
    daac:	e2833001 	add	r3, r3, #1	; 0x1
    dab0:	cafffff9 	bgt	da9c <vsprintf+0x268>
    dab4:	e5963000 	ldr	r3, [r6]
    dab8:	e1a0200a 	mov	r2, sl
    dabc:	e4c23001 	strb	r3, [r2], #1
    dac0:	e3a03020 	mov	r3, #32	; 0x20
    dac4:	ea000000 	b	dacc <vsprintf+0x298>
    dac8:	e5423001 	strb	r3, [r2, #-1]
    dacc:	e2499001 	sub	r9, r9, #1	; 0x1
    dad0:	e3590000 	cmp	r9, #0	; 0x0
    dad4:	e1a0a002 	mov	sl, r2
    dad8:	e2822001 	add	r2, r2, #1	; 0x1
    dadc:	cafffff9 	bgt	dac8 <vsprintf+0x294>
    dae0:	ea0000ab 	b	dd94 <vsprintf+0x560>
    dae4:	e58d7000 	str	r7, [sp]
    dae8:	e1a0000a 	mov	r0, sl
    daec:	e5961000 	ldr	r1, [r6]
    daf0:	e1a02009 	mov	r2, r9
    daf4:	e59d302c 	ldr	r3, [sp, #44]
    daf8:	ebfffe40 	bl	d400 <string>
    dafc:	e1a0a000 	mov	sl, r0
    db00:	ea0000a3 	b	dd94 <vsprintf+0x560>
    db04:	e596b000 	ldr	fp, [r6]
    db08:	e35b0000 	cmp	fp, #0	; 0x0
    db0c:	1a000005 	bne	db28 <vsprintf+0x2f4>
    db10:	e1a0000a 	mov	r0, sl
    db14:	e1a02009 	mov	r2, r9
    db18:	e59d302c 	ldr	r3, [sp, #44]
    db1c:	e59f13f0 	ldr	r1, [pc, #1008]	; df14 <vsprintf+0x6e0>
    db20:	e58d7000 	str	r7, [sp]
    db24:	ea000081 	b	dd30 <vsprintf+0x4fc>
    db28:	e5d83001 	ldrb	r3, [r8, #1]
    db2c:	e2882001 	add	r2, r8, #1	; 0x1
    db30:	e353004d 	cmp	r3, #77	; 0x4d
    db34:	0a000008 	beq	db5c <vsprintf+0x328>
    db38:	8a000002 	bhi	db48 <vsprintf+0x314>
    db3c:	e3530049 	cmp	r3, #73	; 0x49
    db40:	1a00007c 	bne	dd38 <vsprintf+0x504>
    db44:	ea000025 	b	dbe0 <vsprintf+0x3ac>
    db48:	e3530069 	cmp	r3, #105	; 0x69
    db4c:	0a000022 	beq	dbdc <vsprintf+0x3a8>
    db50:	e353006d 	cmp	r3, #109	; 0x6d
    db54:	1a000077 	bne	dd38 <vsprintf+0x504>
    db58:	e3877040 	orr	r7, r7, #64	; 0x40
    db5c:	e1a03327 	lsr	r3, r7, #6
    db60:	e2233001 	eor	r3, r3, #1	; 0x1
    db64:	e2034001 	and	r4, r3, #1	; 0x1
    db68:	e3a0e000 	mov	lr, #0	; 0x0
    db6c:	e28dc05b 	add	ip, sp, #91	; 0x5b
    db70:	e3a0503a 	mov	r5, #58	; 0x3a
    db74:	e7db200e 	ldrb	r2, [fp, lr]
    db78:	e59f0398 	ldr	r0, [pc, #920]	; df18 <vsprintf+0x6e4>
    db7c:	e202300f 	and	r3, r2, #15	; 0xf
    db80:	e7d02222 	ldrb	r2, [r0, r2, lsr #4]
    db84:	e7d03003 	ldrb	r3, [r0, r3]
    db88:	e1a0000c 	mov	r0, ip
    db8c:	e4c02001 	strb	r2, [r0], #1
    db90:	e35e0005 	cmp	lr, #5	; 0x5
    db94:	03a01000 	moveq	r1, #0	; 0x0
    db98:	12041001 	andne	r1, r4, #1	; 0x1
    db9c:	e3510000 	cmp	r1, #0	; 0x0
    dba0:	e5cc3001 	strb	r3, [ip, #1]
    dba4:	e28ee001 	add	lr, lr, #1	; 0x1
    dba8:	e280c001 	add	ip, r0, #1	; 0x1
    dbac:	128cc001 	addne	ip, ip, #1	; 0x1
    dbb0:	15c05001 	strbne	r5, [r0, #1]
    dbb4:	e35e0006 	cmp	lr, #6	; 0x6
    dbb8:	1affffed 	bne	db74 <vsprintf+0x340>
    dbbc:	e3a03000 	mov	r3, #0	; 0x0
    dbc0:	e5cc3000 	strb	r3, [ip]
    dbc4:	e1a0000a 	mov	r0, sl
    dbc8:	e3c7c040 	bic	ip, r7, #64	; 0x40
    dbcc:	e1a02009 	mov	r2, r9
    dbd0:	e59d302c 	ldr	r3, [sp, #44]
    dbd4:	e28d105b 	add	r1, sp, #91	; 0x5b
    dbd8:	ea000053 	b	dd2c <vsprintf+0x4f8>
    dbdc:	e3877040 	orr	r7, r7, #64	; 0x40
    dbe0:	e5d23001 	ldrb	r3, [r2, #1]
    dbe4:	e3530036 	cmp	r3, #54	; 0x36
    dbe8:	1a00002d 	bne	dca4 <vsprintf+0x470>
    dbec:	e1a03327 	lsr	r3, r7, #6
    dbf0:	e2233001 	eor	r3, r3, #1	; 0x1
    dbf4:	e1a0500b 	mov	r5, fp
    dbf8:	e3a04000 	mov	r4, #0	; 0x0
    dbfc:	e203b001 	and	fp, r3, #1	; 0x1
    dc00:	e28dc033 	add	ip, sp, #51	; 0x33
    dc04:	e5d52000 	ldrb	r2, [r5]
    dc08:	e59fe308 	ldr	lr, [pc, #776]	; df18 <vsprintf+0x6e4>
    dc0c:	e202300f 	and	r3, r2, #15	; 0xf
    dc10:	e7de0003 	ldrb	r0, [lr, r3]
    dc14:	e7de3222 	ldrb	r3, [lr, r2, lsr #4]
    dc18:	e1a0100c 	mov	r1, ip
    dc1c:	e4c13001 	strb	r3, [r1], #1
    dc20:	e5cc0001 	strb	r0, [ip, #1]
    dc24:	e5d52001 	ldrb	r2, [r5, #1]
    dc28:	e59f02e8 	ldr	r0, [pc, #744]	; df18 <vsprintf+0x6e4>
    dc2c:	e202300f 	and	r3, r2, #15	; 0xf
    dc30:	e7d0c003 	ldrb	ip, [r0, r3]
    dc34:	e59f32dc 	ldr	r3, [pc, #732]	; df18 <vsprintf+0x6e4>
    dc38:	e3540007 	cmp	r4, #7	; 0x7
    dc3c:	03a0e000 	moveq	lr, #0	; 0x0
    dc40:	120be001 	andne	lr, fp, #1	; 0x1
    dc44:	e7d32222 	ldrb	r2, [r3, r2, lsr #4]
    dc48:	e35e0000 	cmp	lr, #0	; 0x0
    dc4c:	e58d2014 	str	r2, [sp, #20]
    dc50:	e2810001 	add	r0, r1, #1	; 0x1
    dc54:	e59de014 	ldr	lr, [sp, #20]
    dc58:	e2802001 	add	r2, r0, #1	; 0x1
    dc5c:	e5c1e001 	strb	lr, [r1, #1]
    dc60:	e2844001 	add	r4, r4, #1	; 0x1
    dc64:	e5c0c001 	strb	ip, [r0, #1]
    dc68:	13a0003a 	movne	r0, #58	; 0x3a
    dc6c:	e282c001 	add	ip, r2, #1	; 0x1
    dc70:	128cc001 	addne	ip, ip, #1	; 0x1
    dc74:	15c20001 	strbne	r0, [r2, #1]
    dc78:	e3540008 	cmp	r4, #8	; 0x8
    dc7c:	e2855002 	add	r5, r5, #2	; 0x2
    dc80:	1affffdf 	bne	dc04 <vsprintf+0x3d0>
    dc84:	e3a03000 	mov	r3, #0	; 0x0
    dc88:	e5cc3000 	strb	r3, [ip]
    dc8c:	e1a0000a 	mov	r0, sl
    dc90:	e3c7c040 	bic	ip, r7, #64	; 0x40
    dc94:	e1a02009 	mov	r2, r9
    dc98:	e59d302c 	ldr	r3, [sp, #44]
    dc9c:	e28d1033 	add	r1, sp, #51	; 0x33
    dca0:	ea000021 	b	dd2c <vsprintf+0x4f8>
    dca4:	e3530034 	cmp	r3, #52	; 0x34
    dca8:	13c77040 	bicne	r7, r7, #64	; 0x40
    dcac:	1a000021 	bne	dd38 <vsprintf+0x504>
    dcb0:	e3a05000 	mov	r5, #0	; 0x0
    dcb4:	e28d406d 	add	r4, sp, #109	; 0x6d
    dcb8:	e7db1005 	ldrb	r1, [fp, r5]
    dcbc:	e28d007d 	add	r0, sp, #125	; 0x7d
    dcc0:	ebfffd9b 	bl	d334 <put_dec_trunc>
    dcc4:	e59d101c 	ldr	r1, [sp, #28]
    dcc8:	e2402001 	sub	r2, r0, #1	; 0x1
    dccc:	e0610000 	rsb	r0, r1, r0
    dcd0:	e3a01000 	mov	r1, #0	; 0x0
    dcd4:	ea000002 	b	dce4 <vsprintf+0x4b0>
    dcd8:	e5d23001 	ldrb	r3, [r2, #1]
    dcdc:	e7c43001 	strb	r3, [r4, r1]
    dce0:	e2811001 	add	r1, r1, #1	; 0x1
    dce4:	e59d3018 	ldr	r3, [sp, #24]
    dce8:	e2422001 	sub	r2, r2, #1	; 0x1
    dcec:	e1520003 	cmp	r2, r3
    dcf0:	1afffff8 	bne	dcd8 <vsprintf+0x4a4>
    dcf4:	e3550003 	cmp	r5, #3	; 0x3
    dcf8:	13a0e02e 	movne	lr, #46	; 0x2e
    dcfc:	e0844000 	add	r4, r4, r0
    dd00:	e2855001 	add	r5, r5, #1	; 0x1
    dd04:	14c4e001 	strbne	lr, [r4], #1
    dd08:	e3550004 	cmp	r5, #4	; 0x4
    dd0c:	1affffe9 	bne	dcb8 <vsprintf+0x484>
    dd10:	e3a03000 	mov	r3, #0	; 0x0
    dd14:	e5c43000 	strb	r3, [r4]
    dd18:	e59d302c 	ldr	r3, [sp, #44]
    dd1c:	e3c7c040 	bic	ip, r7, #64	; 0x40
    dd20:	e1a0000a 	mov	r0, sl
    dd24:	e1a02009 	mov	r2, r9
    dd28:	e28d106d 	add	r1, sp, #109	; 0x6d
    dd2c:	e58dc000 	str	ip, [sp]
    dd30:	ebfffdb2 	bl	d400 <string>
    dd34:	ea00000d 	b	dd70 <vsprintf+0x53c>
    dd38:	e3a01010 	mov	r1, #16	; 0x10
    dd3c:	e3790001 	cmn	r9, #1	; 0x1
    dd40:	e58d1000 	str	r1, [sp]
    dd44:	e59d102c 	ldr	r1, [sp, #44]
    dd48:	e387c020 	orr	ip, r7, #32	; 0x20
    dd4c:	02899009 	addeq	r9, r9, #9	; 0x9
    dd50:	0387c021 	orreq	ip, r7, #33	; 0x21
    dd54:	e1a0000a 	mov	r0, sl
    dd58:	e1a0200b 	mov	r2, fp
    dd5c:	e3a03000 	mov	r3, #0	; 0x0
    dd60:	e58d9004 	str	r9, [sp, #4]
    dd64:	e58d1008 	str	r1, [sp, #8]
    dd68:	e58dc00c 	str	ip, [sp, #12]
    dd6c:	ebfffe1a 	bl	d5dc <number>
    dd70:	e1a0a000 	mov	sl, r0
    dd74:	e1a02008 	mov	r2, r8
    dd78:	e5d23001 	ldrb	r3, [r2, #1]
    dd7c:	e59fe198 	ldr	lr, [pc, #408]	; df1c <vsprintf+0x6e8>
    dd80:	e1a08002 	mov	r8, r2
    dd84:	e7de3003 	ldrb	r3, [lr, r3]
    dd88:	e2822001 	add	r2, r2, #1	; 0x1
    dd8c:	e3130007 	tst	r3, #7	; 0x7
    dd90:	1afffff8 	bne	dd78 <vsprintf+0x544>
    dd94:	e2866004 	add	r6, r6, #4	; 0x4
    dd98:	ea000054 	b	def0 <vsprintf+0x6bc>
    dd9c:	e59d0028 	ldr	r0, [sp, #40]
    dda0:	e5963000 	ldr	r3, [r6]
    dda4:	e2861004 	add	r1, r6, #4	; 0x4
    dda8:	e060200a 	rsb	r2, r0, sl
    ddac:	e1a06001 	mov	r6, r1
    ddb0:	e5832000 	str	r2, [r3]
    ddb4:	ea00004d 	b	def0 <vsprintf+0x6bc>
    ddb8:	e3a0c008 	mov	ip, #8	; 0x8
    ddbc:	ea00000d 	b	ddf8 <vsprintf+0x5c4>
    ddc0:	e3877020 	orr	r7, r7, #32	; 0x20
    ddc4:	e3a0c010 	mov	ip, #16	; 0x10
    ddc8:	ea00000a 	b	ddf8 <vsprintf+0x5c4>
    ddcc:	e3877002 	orr	r7, r7, #2	; 0x2
    ddd0:	ea000007 	b	ddf4 <vsprintf+0x5c0>
    ddd4:	e3a03025 	mov	r3, #37	; 0x25
    ddd8:	e4ca3001 	strb	r3, [sl], #1
    dddc:	e5d83000 	ldrb	r3, [r8]
    dde0:	e3530000 	cmp	r3, #0	; 0x0
    dde4:	02488001 	subeq	r8, r8, #1	; 0x1
    dde8:	0a000040 	beq	def0 <vsprintf+0x6bc>
    ddec:	e4ca3001 	strb	r3, [sl], #1
    ddf0:	ea00003e 	b	def0 <vsprintf+0x6bc>
    ddf4:	e3a0c00a 	mov	ip, #10	; 0xa
    ddf8:	e352004c 	cmp	r2, #76	; 0x4c
    ddfc:	1a000009 	bne	de28 <vsprintf+0x5f4>
    de00:	e59d1024 	ldr	r1, [sp, #36]
    de04:	e3a02000 	mov	r2, #0	; 0x0
    de08:	e0613006 	rsb	r3, r1, r6
    de0c:	e3130007 	tst	r3, #7	; 0x7
    de10:	02866004 	addeq	r6, r6, #4	; 0x4
    de14:	e896000a 	ldm	r6, {r1, r3}
    de18:	e1a05003 	mov	r5, r3
    de1c:	e1824001 	orr	r4, r2, r1
    de20:	e2866008 	add	r6, r6, #8	; 0x8
    de24:	ea000028 	b	decc <vsprintf+0x698>
    de28:	e352006c 	cmp	r2, #108	; 0x6c
    de2c:	1a000006 	bne	de4c <vsprintf+0x618>
    de30:	e5963000 	ldr	r3, [r6]
    de34:	e3170002 	tst	r7, #2	; 0x2
    de38:	e1a04003 	mov	r4, r3
    de3c:	e3a05000 	mov	r5, #0	; 0x0
    de40:	e2866004 	add	r6, r6, #4	; 0x4
    de44:	0a000020 	beq	decc <vsprintf+0x698>
    de48:	ea00001e 	b	dec8 <vsprintf+0x694>
    de4c:	e352007a 	cmp	r2, #122	; 0x7a
    de50:	1352005a 	cmpne	r2, #90	; 0x5a
    de54:	05963000 	ldreq	r3, [r6]
    de58:	02866004 	addeq	r6, r6, #4	; 0x4
    de5c:	01a04003 	moveq	r4, r3
    de60:	03a05000 	moveq	r5, #0	; 0x0
    de64:	0a000018 	beq	decc <vsprintf+0x698>
    de68:	e3520074 	cmp	r2, #116	; 0x74
    de6c:	05963000 	ldreq	r3, [r6]
    de70:	02866004 	addeq	r6, r6, #4	; 0x4
    de74:	0a00000b 	beq	dea8 <vsprintf+0x674>
    de78:	e3520068 	cmp	r2, #104	; 0x68
    de7c:	e2861004 	add	r1, r6, #4	; 0x4
    de80:	e5962000 	ldr	r2, [r6]
    de84:	e2070002 	and	r0, r7, #2	; 0x2
    de88:	1a000009 	bne	deb4 <vsprintf+0x680>
    de8c:	e6ff3072 	uxth	r3, r2
    de90:	e3500000 	cmp	r0, #0	; 0x0
    de94:	e1a06001 	mov	r6, r1
    de98:	e1a04003 	mov	r4, r3
    de9c:	e3a05000 	mov	r5, #0	; 0x0
    dea0:	0a000009 	beq	decc <vsprintf+0x698>
    dea4:	e6bf3074 	sxth	r3, r4
    dea8:	e1a04003 	mov	r4, r3
    deac:	e1a05fc4 	asr	r5, r4, #31
    deb0:	ea000005 	b	decc <vsprintf+0x698>
    deb4:	e3500000 	cmp	r0, #0	; 0x0
    deb8:	e1a06001 	mov	r6, r1
    debc:	e1a04002 	mov	r4, r2
    dec0:	e3a05000 	mov	r5, #0	; 0x0
    dec4:	0a000000 	beq	decc <vsprintf+0x698>
    dec8:	e1a05fc4 	asr	r5, r4, #31
    decc:	e59de02c 	ldr	lr, [sp, #44]
    ded0:	e1a0000a 	mov	r0, sl
    ded4:	e1a02004 	mov	r2, r4
    ded8:	e1a03005 	mov	r3, r5
    dedc:	e58dc000 	str	ip, [sp]
    dee0:	e98d4200 	stmib	sp, {r9, lr}
    dee4:	e58d700c 	str	r7, [sp, #12]
    dee8:	ebfffdbb 	bl	d5dc <number>
    deec:	e1a0a000 	mov	sl, r0
    def0:	e2888001 	add	r8, r8, #1	; 0x1
    def4:	e5d83000 	ldrb	r3, [r8]
    def8:	e3530000 	cmp	r3, #0	; 0x0
    defc:	1afffe58 	bne	d864 <vsprintf+0x30>
    df00:	e5ca3000 	strb	r3, [sl]
    df04:	e59d1028 	ldr	r1, [sp, #40]
    df08:	e061000a 	rsb	r0, r1, sl
    df0c:	e28dd084 	add	sp, sp, #132	; 0x84
    df10:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    df14:	000141c3 	.word	0x000141c3
    df18:	000118a1 	.word	0x000118a1
    df1c:	000118c2 	.word	0x000118c2

0000df20 <sprintf>:
    df20:	e92d000e 	push	{r1, r2, r3}
    df24:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    df28:	e59d1004 	ldr	r1, [sp, #4]
    df2c:	e28d2008 	add	r2, sp, #8	; 0x8
    df30:	ebfffe3f 	bl	d834 <vsprintf>
    df34:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    df38:	e28dd00c 	add	sp, sp, #12	; 0xc
    df3c:	e12fff1e 	bx	lr

0000df40 <ius_check_header>:
    df40:	e92d4070 	push	{r4, r5, r6, lr}
    df44:	e5901000 	ldr	r1, [r0]
    df48:	e59f30e8 	ldr	r3, [pc, #232]	; e038 <ius_check_header+0xf8>
    df4c:	e24dd008 	sub	sp, sp, #8	; 0x8
    df50:	e1510003 	cmp	r1, r3
    df54:	e1a04000 	mov	r4, r0
    df58:	0a000005 	beq	df74 <ius_check_header+0x34>
    df5c:	e59f00d8 	ldr	r0, [pc, #216]	; e03c <ius_check_header+0xfc>
    df60:	e59f30d8 	ldr	r3, [pc, #216]	; e040 <ius_check_header+0x100>
    df64:	e1a0e00f 	mov	lr, pc
    df68:	e593f000 	ldr	pc, [r3]
    df6c:	e3e00000 	mvn	r0, #0	; 0x0
    df70:	ea00002e 	b	e030 <ius_check_header+0xf0>
    df74:	ebffe01a 	bl	5fe4 <cdbg_verify_burn>
    df78:	e3500000 	cmp	r0, #0	; 0x0
    df7c:	0a000011 	beq	dfc8 <ius_check_header+0x88>
    df80:	e5943004 	ldr	r3, [r4, #4]
    df84:	e3130002 	tst	r3, #2	; 0x2
    df88:	1a000005 	bne	dfa4 <ius_check_header+0x64>
    df8c:	e59f00b0 	ldr	r0, [pc, #176]	; e044 <ius_check_header+0x104>
    df90:	e59f30a8 	ldr	r3, [pc, #168]	; e040 <ius_check_header+0x100>
    df94:	e1a0e00f 	mov	lr, pc
    df98:	e593f000 	ldr	pc, [r3]
    df9c:	e3e00021 	mvn	r0, #33	; 0x21
    dfa0:	ea000022 	b	e030 <ius_check_header+0xf0>
    dfa4:	ebffe012 	bl	5ff4 <cdbg_verify_burn_key>
    dfa8:	e59f2098 	ldr	r2, [pc, #152]	; e048 <ius_check_header+0x108>
    dfac:	e1a01000 	mov	r1, r0
    dfb0:	e3a00000 	mov	r0, #0	; 0x0
    dfb4:	ebffdc9e 	bl	5234 <ss_aes_init>
    dfb8:	e2840010 	add	r0, r4, #16	; 0x10
    dfbc:	e1a01000 	mov	r1, r0
    dfc0:	e3a02e1f 	mov	r2, #496	; 0x1f0
    dfc4:	ebffdc89 	bl	51f0 <ss_aes_data>
    dfc8:	e3a06000 	mov	r6, #0	; 0x0
    dfcc:	e5945018 	ldr	r5, [r4, #24]
    dfd0:	e3a01c02 	mov	r1, #512	; 0x200
    dfd4:	e5846018 	str	r6, [r4, #24]
    dfd8:	e1a00006 	mov	r0, r6
    dfdc:	ebffddb2 	bl	56ac <hash_init>
    dfe0:	e3a01c02 	mov	r1, #512	; 0x200
    dfe4:	e1a00004 	mov	r0, r4
    dfe8:	ebffdd8a 	bl	5618 <hash_data>
    dfec:	e28d0004 	add	r0, sp, #4	; 0x4
    dff0:	ebffdd6c 	bl	55a8 <hash_value>
    dff4:	e59d2004 	ldr	r2, [sp, #4]
    dff8:	e59f3040 	ldr	r3, [pc, #64]	; e040 <ius_check_header+0x100>
    dffc:	e1550002 	cmp	r5, r2
    e000:	e5845018 	str	r5, [r4, #24]
    e004:	0a000005 	beq	e020 <ius_check_header+0xe0>
    e008:	e59f003c 	ldr	r0, [pc, #60]	; e04c <ius_check_header+0x10c>
    e00c:	e1a01005 	mov	r1, r5
    e010:	e1a0e00f 	mov	lr, pc
    e014:	e593f000 	ldr	pc, [r3]
    e018:	e3e0001f 	mvn	r0, #31	; 0x1f
    e01c:	ea000003 	b	e030 <ius_check_header+0xf0>
    e020:	e59f0028 	ldr	r0, [pc, #40]	; e050 <ius_check_header+0x110>
    e024:	e1a0e00f 	mov	lr, pc
    e028:	e593f000 	ldr	pc, [r3]
    e02c:	e1a00006 	mov	r0, r6
    e030:	e28dd008 	add	sp, sp, #8	; 0x8
    e034:	e8bd8070 	pop	{r4, r5, r6, pc}
    e038:	7375692b 	.word	0x7375692b
    e03c:	000141ca 	.word	0x000141ca
    e040:	0800d76c 	.word	0x0800d76c
    e044:	000141e4 	.word	0x000141e4
    e048:	0800d10c 	.word	0x0800d10c
    e04c:	000141f3 	.word	0x000141f3
    e050:	0001421b 	.word	0x0001421b

0000e054 <printf_off>:
    e054:	e92d000f 	push	{r0, r1, r2, r3}
    e058:	e28dd010 	add	sp, sp, #16	; 0x10
    e05c:	e12fff1e 	bx	lr

0000e060 <disable_ctrlc>:
    e060:	e59f300c 	ldr	r3, [pc, #12]	; e074 <disable_ctrlc+0x14>
    e064:	e5932000 	ldr	r2, [r3]
    e068:	e5830000 	str	r0, [r3]
    e06c:	e1a00002 	mov	r0, r2
    e070:	e12fff1e 	bx	lr
    e074:	0800d30c 	.word	0x0800d30c

0000e078 <had_ctrlc>:
    e078:	e59f3004 	ldr	r3, [pc, #4]	; e084 <had_ctrlc+0xc>
    e07c:	e5930000 	ldr	r0, [r3]
    e080:	e12fff1e 	bx	lr
    e084:	0800d308 	.word	0x0800d308

0000e088 <clear_ctrlc>:
    e088:	e59f3008 	ldr	r3, [pc, #8]	; e098 <clear_ctrlc+0x10>
    e08c:	e3a02000 	mov	r2, #0	; 0x0
    e090:	e5832000 	str	r2, [r3]
    e094:	e12fff1e 	bx	lr
    e098:	0800d308 	.word	0x0800d308

0000e09c <dbg>:
    e09c:	e92d000f 	push	{r0, r1, r2, r3}
    e0a0:	e28dd010 	add	sp, sp, #16	; 0x10
    e0a4:	e12fff1e 	bx	lr

0000e0a8 <puts>:
    e0a8:	eaffe98c 	b	86e0 <serial_puts>

0000e0ac <vprintf>:
    e0ac:	e92d4010 	push	{r4, lr}
    e0b0:	e24ddd06 	sub	sp, sp, #384	; 0x180
    e0b4:	e1a02001 	mov	r2, r1
    e0b8:	e1a01000 	mov	r1, r0
    e0bc:	e1a0000d 	mov	r0, sp
    e0c0:	ebfffddb 	bl	d834 <vsprintf>
    e0c4:	e1a0000d 	mov	r0, sp
    e0c8:	e1a0400d 	mov	r4, sp
    e0cc:	ebffe983 	bl	86e0 <serial_puts>
    e0d0:	e28ddd06 	add	sp, sp, #384	; 0x180
    e0d4:	e8bd8010 	pop	{r4, pc}

0000e0d8 <printf_on>:
    e0d8:	e92d000f 	push	{r0, r1, r2, r3}
    e0dc:	e92d4010 	push	{r4, lr}
    e0e0:	e24ddd06 	sub	sp, sp, #384	; 0x180
    e0e4:	e1a0000d 	mov	r0, sp
    e0e8:	e59d1188 	ldr	r1, [sp, #392]
    e0ec:	e28d2f63 	add	r2, sp, #396	; 0x18c
    e0f0:	ebfffdcf 	bl	d834 <vsprintf>
    e0f4:	e1a0000d 	mov	r0, sp
    e0f8:	e1a0400d 	mov	r4, sp
    e0fc:	ebffe977 	bl	86e0 <serial_puts>
    e100:	e28ddd06 	add	sp, sp, #384	; 0x180
    e104:	e8bd4010 	pop	{r4, lr}
    e108:	e28dd010 	add	sp, sp, #16	; 0x10
    e10c:	e12fff1e 	bx	lr

0000e110 <serial_printf>:
    e110:	e92d000f 	push	{r0, r1, r2, r3}
    e114:	e92d4010 	push	{r4, lr}
    e118:	e24ddd06 	sub	sp, sp, #384	; 0x180
    e11c:	e1a0000d 	mov	r0, sp
    e120:	e59d1188 	ldr	r1, [sp, #392]
    e124:	e28d2f63 	add	r2, sp, #396	; 0x18c
    e128:	ebfffdc1 	bl	d834 <vsprintf>
    e12c:	e1a0000d 	mov	r0, sp
    e130:	e1a0400d 	mov	r4, sp
    e134:	ebffe969 	bl	86e0 <serial_puts>
    e138:	e28ddd06 	add	sp, sp, #384	; 0x180
    e13c:	e8bd4010 	pop	{r4, lr}
    e140:	e28dd010 	add	sp, sp, #16	; 0x10
    e144:	e12fff1e 	bx	lr

0000e148 <putc>:
    e148:	eaffe94d 	b	8684 <serial_putc>

0000e14c <tstc>:
    e14c:	eaffe945 	b	8668 <serial_tstc>

0000e150 <getc>:
    e150:	eaffe973 	b	8724 <serial_getc>

0000e154 <ctrlc>:
    e154:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    e158:	e59f3040 	ldr	r3, [pc, #64]	; e1a0 <ctrlc+0x4c>
    e15c:	e24dd004 	sub	sp, sp, #4	; 0x4
    e160:	e5933000 	ldr	r3, [r3]
    e164:	e3530000 	cmp	r3, #0	; 0x0
    e168:	1a000009 	bne	e194 <ctrlc+0x40>
    e16c:	ebffe93d 	bl	8668 <serial_tstc>
    e170:	e3500000 	cmp	r0, #0	; 0x0
    e174:	0a000006 	beq	e194 <ctrlc+0x40>
    e178:	ebffe969 	bl	8724 <serial_getc>
    e17c:	e3500003 	cmp	r0, #3	; 0x3
    e180:	059f301c 	ldreq	r3, [pc, #28]	; e1a4 <ctrlc+0x50>
    e184:	03a02001 	moveq	r2, #1	; 0x1
    e188:	05832000 	streq	r2, [r3]
    e18c:	01a00002 	moveq	r0, r2
    e190:	0a000000 	beq	e198 <ctrlc+0x44>
    e194:	e3a00000 	mov	r0, #0	; 0x0
    e198:	e28dd004 	add	sp, sp, #4	; 0x4
    e19c:	e8bd8000 	pop	{pc}
    e1a0:	0800d30c 	.word	0x0800d30c
    e1a4:	0800d308 	.word	0x0800d308

0000e1a8 <parse_line>:
    e1a8:	e92d4010 	push	{r4, lr}
    e1ac:	e3a04000 	mov	r4, #0	; 0x0
    e1b0:	e1a02004 	mov	r2, r4
    e1b4:	ea000000 	b	e1bc <parse_line+0x14>
    e1b8:	e2800001 	add	r0, r0, #1	; 0x1
    e1bc:	e5d03000 	ldrb	r3, [r0]
    e1c0:	e3530009 	cmp	r3, #9	; 0x9
    e1c4:	13530020 	cmpne	r3, #32	; 0x20
    e1c8:	0afffffa 	beq	e1b8 <parse_line+0x10>
    e1cc:	e3530000 	cmp	r3, #0	; 0x0
    e1d0:	0a00000c 	beq	e208 <parse_line+0x60>
    e1d4:	e7810104 	str	r0, [r1, r4, lsl #2]
    e1d8:	e2844001 	add	r4, r4, #1	; 0x1
    e1dc:	ea000000 	b	e1e4 <parse_line+0x3c>
    e1e0:	e2800001 	add	r0, r0, #1	; 0x1
    e1e4:	e5d03000 	ldrb	r3, [r0]
    e1e8:	e3530000 	cmp	r3, #0	; 0x0
    e1ec:	13530020 	cmpne	r3, #32	; 0x20
    e1f0:	0a000002 	beq	e200 <parse_line+0x58>
    e1f4:	e3530009 	cmp	r3, #9	; 0x9
    e1f8:	1afffff8 	bne	e1e0 <parse_line+0x38>
    e1fc:	ea000003 	b	e210 <parse_line+0x68>
    e200:	e3530000 	cmp	r3, #0	; 0x0
    e204:	1a000001 	bne	e210 <parse_line+0x68>
    e208:	e7813104 	str	r3, [r1, r4, lsl #2]
    e20c:	ea000007 	b	e230 <parse_line+0x88>
    e210:	e3540010 	cmp	r4, #16	; 0x10
    e214:	e5c02000 	strb	r2, [r0]
    e218:	1affffe6 	bne	e1b8 <parse_line+0x10>
    e21c:	e59f0014 	ldr	r0, [pc, #20]	; e238 <parse_line+0x90>
    e220:	e1a01004 	mov	r1, r4
    e224:	e59f3010 	ldr	r3, [pc, #16]	; e23c <parse_line+0x94>
    e228:	e1a0e00f 	mov	lr, pc
    e22c:	e593f000 	ldr	pc, [r3]
    e230:	e1a00004 	mov	r0, r4
    e234:	e8bd8010 	pop	{r4, pc}
    e238:	00014231 	.word	0x00014231
    e23c:	0800d76c 	.word	0x0800d76c

0000e240 <run_command>:
    e240:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    e244:	e1a05000 	mov	r5, r0
    e248:	e24ddf93 	sub	sp, sp, #588	; 0x24c
    e24c:	e1a0a001 	mov	sl, r1
    e250:	ebffff8c 	bl	e088 <clear_ctrlc>
    e254:	e3550000 	cmp	r5, #0	; 0x0
    e258:	0a0000ac 	beq	e510 <run_command+0x2d0>
    e25c:	e5d53000 	ldrb	r3, [r5]
    e260:	e3530000 	cmp	r3, #0	; 0x0
    e264:	0a0000a9 	beq	e510 <run_command+0x2d0>
    e268:	e1a00005 	mov	r0, r5
    e26c:	ebffcc1a 	bl	12dc <strlen>
    e270:	e35000ff 	cmp	r0, #255	; 0xff
    e274:	9a000002 	bls	e284 <run_command+0x44>
    e278:	e59f02a0 	ldr	r0, [pc, #672]	; e520 <run_command+0x2e0>
    e27c:	ebffff89 	bl	e0a8 <puts>
    e280:	ea0000a2 	b	e510 <run_command+0x2d0>
    e284:	e28d4f41 	add	r4, sp, #260	; 0x104
    e288:	e1a01005 	mov	r1, r5
    e28c:	e1a00004 	mov	r0, r4
    e290:	e3a06000 	mov	r6, #0	; 0x0
    e294:	ebffcbb9 	bl	1180 <strcpy>
    e298:	e3a07001 	mov	r7, #1	; 0x1
    e29c:	e1a0b006 	mov	fp, r6
    e2a0:	e28d9f81 	add	r9, sp, #516	; 0x204
    e2a4:	ea000091 	b	e4f0 <run_command+0x2b0>
    e2a8:	e3510027 	cmp	r1, #39	; 0x27
    e2ac:	1a000002 	bne	e2bc <run_command+0x7c>
    e2b0:	e5523001 	ldrb	r3, [r2, #-1]
    e2b4:	e353005c 	cmp	r3, #92	; 0x5c
    e2b8:	12200001 	eorne	r0, r0, #1	; 0x1
    e2bc:	e3500000 	cmp	r0, #0	; 0x0
    e2c0:	1a000009 	bne	e2ec <run_command+0xac>
    e2c4:	e351003b 	cmp	r1, #59	; 0x3b
    e2c8:	13a03000 	movne	r3, #0	; 0x0
    e2cc:	03a03001 	moveq	r3, #1	; 0x1
    e2d0:	e1520004 	cmp	r2, r4
    e2d4:	03a03000 	moveq	r3, #0	; 0x0
    e2d8:	e3530000 	cmp	r3, #0	; 0x0
    e2dc:	0a000002 	beq	e2ec <run_command+0xac>
    e2e0:	e5523001 	ldrb	r3, [r2, #-1]
    e2e4:	e353005c 	cmp	r3, #92	; 0x5c
    e2e8:	1a000005 	bne	e304 <run_command+0xc4>
    e2ec:	e2822001 	add	r2, r2, #1	; 0x1
    e2f0:	e5d21000 	ldrb	r1, [r2]
    e2f4:	e3510000 	cmp	r1, #0	; 0x0
    e2f8:	1affffea 	bne	e2a8 <run_command+0x68>
    e2fc:	e1a08002 	mov	r8, r2
    e300:	ea000001 	b	e30c <run_command+0xcc>
    e304:	e5c20000 	strb	r0, [r2]
    e308:	e2828001 	add	r8, r2, #1	; 0x1
    e30c:	e1a00004 	mov	r0, r4
    e310:	ebffcbf1 	bl	12dc <strlen>
    e314:	e3a0c000 	mov	ip, #0	; 0x0
    e318:	e3a0ec01 	mov	lr, #256	; 0x100
    e31c:	e1a0100c 	mov	r1, ip
    e320:	e28d5004 	add	r5, sp, #4	; 0x4
    e324:	ea000049 	b	e450 <run_command+0x210>
    e328:	e35c0003 	cmp	ip, #3	; 0x3
    e32c:	e2400001 	sub	r0, r0, #1	; 0x1
    e330:	e4d42001 	ldrb	r2, [r4], #1
    e334:	0a000035 	beq	e410 <run_command+0x1d0>
    e338:	e352005c 	cmp	r2, #92	; 0x5c
    e33c:	13a03000 	movne	r3, #0	; 0x0
    e340:	03a03001 	moveq	r3, #1	; 0x1
    e344:	e351005c 	cmp	r1, #92	; 0x5c
    e348:	03a03000 	moveq	r3, #0	; 0x0
    e34c:	e3530000 	cmp	r3, #0	; 0x0
    e350:	0a000004 	beq	e368 <run_command+0x128>
    e354:	e3500000 	cmp	r0, #0	; 0x0
    e358:	0a00003f 	beq	e45c <run_command+0x21c>
    e35c:	e4d42001 	ldrb	r2, [r4], #1
    e360:	e2400001 	sub	r0, r0, #1	; 0x1
    e364:	e3a0105c 	mov	r1, #92	; 0x5c
    e368:	e35c0003 	cmp	ip, #3	; 0x3
    e36c:	979ff10c 	ldrls	pc, [pc, ip, lsl #2]
    e370:	ea000035 	b	e44c <run_command+0x20c>
    e374:	0000e384 	.word	0x0000e384
    e378:	0000e3c8 	.word	0x0000e3c8
    e37c:	0000e400 	.word	0x0000e400
    e380:	0000e410 	.word	0x0000e410
    e384:	e251105c 	subs	r1, r1, #92	; 0x5c
    e388:	13a01001 	movne	r1, #1	; 0x1
    e38c:	e3520027 	cmp	r2, #39	; 0x27
    e390:	13a03000 	movne	r3, #0	; 0x0
    e394:	02013001 	andeq	r3, r1, #1	; 0x1
    e398:	e3530000 	cmp	r3, #0	; 0x0
    e39c:	13a0c003 	movne	ip, #3	; 0x3
    e3a0:	1a000022 	bne	e430 <run_command+0x1f0>
    e3a4:	e3520024 	cmp	r2, #36	; 0x24
    e3a8:	13a03000 	movne	r3, #0	; 0x0
    e3ac:	02013001 	andeq	r3, r1, #1	; 0x1
    e3b0:	e3530000 	cmp	r3, #0	; 0x0
    e3b4:	13a0c001 	movne	ip, #1	; 0x1
    e3b8:	13a02024 	movne	r2, #36	; 0x24
    e3bc:	04c52001 	strbeq	r2, [r5], #1
    e3c0:	024ee001 	subeq	lr, lr, #1	; 0x1
    e3c4:	ea000020 	b	e44c <run_command+0x20c>
    e3c8:	e352007b 	cmp	r2, #123	; 0x7b
    e3cc:	13520028 	cmpne	r2, #40	; 0x28
    e3d0:	13a03000 	movne	r3, #0	; 0x0
    e3d4:	03a03001 	moveq	r3, #1	; 0x1
    e3d8:	03a0c002 	moveq	ip, #2	; 0x2
    e3dc:	0a00001a 	beq	e44c <run_command+0x20c>
    e3e0:	e3a01024 	mov	r1, #36	; 0x24
    e3e4:	e25ee001 	subs	lr, lr, #1	; 0x1
    e3e8:	e4c51001 	strb	r1, [r5], #1
    e3ec:	0a000015 	beq	e448 <run_command+0x208>
    e3f0:	e4c52001 	strb	r2, [r5], #1
    e3f4:	e24ee001 	sub	lr, lr, #1	; 0x1
    e3f8:	e1a0c003 	mov	ip, r3
    e3fc:	ea000012 	b	e44c <run_command+0x20c>
    e400:	e352007d 	cmp	r2, #125	; 0x7d
    e404:	13520029 	cmpne	r2, #41	; 0x29
    e408:	0a00000e 	beq	e448 <run_command+0x208>
    e40c:	ea00000e 	b	e44c <run_command+0x20c>
    e410:	e3520027 	cmp	r2, #39	; 0x27
    e414:	13a03000 	movne	r3, #0	; 0x0
    e418:	03a03001 	moveq	r3, #1	; 0x1
    e41c:	e351005c 	cmp	r1, #92	; 0x5c
    e420:	03a03000 	moveq	r3, #0	; 0x0
    e424:	e3530000 	cmp	r3, #0	; 0x0
    e428:	0a000002 	beq	e438 <run_command+0x1f8>
    e42c:	e3a0c000 	mov	ip, #0	; 0x0
    e430:	e3a02027 	mov	r2, #39	; 0x27
    e434:	ea000004 	b	e44c <run_command+0x20c>
    e438:	e4c52001 	strb	r2, [r5], #1
    e43c:	e24ee001 	sub	lr, lr, #1	; 0x1
    e440:	e3a0c003 	mov	ip, #3	; 0x3
    e444:	ea000000 	b	e44c <run_command+0x20c>
    e448:	e3a0c000 	mov	ip, #0	; 0x0
    e44c:	e1a01002 	mov	r1, r2
    e450:	e3500000 	cmp	r0, #0	; 0x0
    e454:	135e0000 	cmpne	lr, #0	; 0x0
    e458:	1affffb2 	bne	e328 <run_command+0xe8>
    e45c:	e35e0000 	cmp	lr, #0	; 0x0
    e460:	15c5b000 	strbne	fp, [r5]
    e464:	0545e001 	strbeq	lr, [r5, #-1]
    e468:	e28d0004 	add	r0, sp, #4	; 0x4
    e46c:	e1a01009 	mov	r1, r9
    e470:	ebffff4c 	bl	e1a8 <parse_line>
    e474:	e2505000 	subs	r5, r0, #0	; 0x0
    e478:	0a00000d 	beq	e4b4 <run_command+0x274>
    e47c:	e59d0204 	ldr	r0, [sp, #516]
    e480:	ebffd213 	bl	2cd4 <find_cmd>
    e484:	e2504000 	subs	r4, r0, #0	; 0x0
    e488:	1a000005 	bne	e4a4 <run_command+0x264>
    e48c:	e59f0090 	ldr	r0, [pc, #144]	; e524 <run_command+0x2e4>
    e490:	e59d1204 	ldr	r1, [sp, #516]
    e494:	e59f308c 	ldr	r3, [pc, #140]	; e528 <run_command+0x2e8>
    e498:	e1a0e00f 	mov	lr, pc
    e49c:	e593f000 	ldr	pc, [r3]
    e4a0:	ea000003 	b	e4b4 <run_command+0x274>
    e4a4:	e5943004 	ldr	r3, [r4, #4]
    e4a8:	e1550003 	cmp	r5, r3
    e4ac:	da000002 	ble	e4bc <run_command+0x27c>
    e4b0:	ebffd1c1 	bl	2bbc <cmd_usage>
    e4b4:	e3e06000 	mvn	r6, #0	; 0x0
    e4b8:	ea00000b 	b	e4ec <run_command+0x2ac>
    e4bc:	e1a02005 	mov	r2, r5
    e4c0:	e1a0100a 	mov	r1, sl
    e4c4:	e1a03009 	mov	r3, r9
    e4c8:	e1a0e00f 	mov	lr, pc
    e4cc:	e594f00c 	ldr	pc, [r4, #12]
    e4d0:	e3500000 	cmp	r0, #0	; 0x0
    e4d4:	13e06000 	mvnne	r6, #0	; 0x0
    e4d8:	e5944008 	ldr	r4, [r4, #8]
    e4dc:	ebfffee5 	bl	e078 <had_ctrlc>
    e4e0:	e3500000 	cmp	r0, #0	; 0x0
    e4e4:	1a000009 	bne	e510 <run_command+0x2d0>
    e4e8:	e0077004 	and	r7, r7, r4
    e4ec:	e1a04008 	mov	r4, r8
    e4f0:	e5d43000 	ldrb	r3, [r4]
    e4f4:	e3530000 	cmp	r3, #0	; 0x0
    e4f8:	11a02004 	movne	r2, r4
    e4fc:	13a00000 	movne	r0, #0	; 0x0
    e500:	1affff7a 	bne	e2f0 <run_command+0xb0>
    e504:	e3560000 	cmp	r6, #0	; 0x0
    e508:	01a06007 	moveq	r6, r7
    e50c:	ea000000 	b	e514 <run_command+0x2d4>
    e510:	e3e06000 	mvn	r6, #0	; 0x0
    e514:	e1a00006 	mov	r0, r6
    e518:	e28ddf93 	add	sp, sp, #588	; 0x24c
    e51c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    e520:	00014250 	.word	0x00014250
    e524:	00014266 	.word	0x00014266
    e528:	0800d76c 	.word	0x0800d76c

0000e52c <cread_add_char>:
    e52c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    e530:	e3510000 	cmp	r1, #0	; 0x0
    e534:	e24dd004 	sub	sp, sp, #4	; 0x4
    e538:	e1a07000 	mov	r7, r0
    e53c:	e1a04002 	mov	r4, r2
    e540:	e1a00003 	mov	r0, r3
    e544:	e59d6018 	ldr	r6, [sp, #24]
    e548:	e59dc01c 	ldr	ip, [sp, #28]
    e54c:	1a000003 	bne	e560 <cread_add_char+0x34>
    e550:	e5922000 	ldr	r2, [r2]
    e554:	e5933000 	ldr	r3, [r3]
    e558:	e1520003 	cmp	r2, r3
    e55c:	1a000026 	bne	e5fc <cread_add_char+0xd0>
    e560:	e5902000 	ldr	r2, [r0]
    e564:	e24c3001 	sub	r3, ip, #1	; 0x1
    e568:	e1520003 	cmp	r2, r3
    e56c:	9a000003 	bls	e580 <cread_add_char+0x54>
    e570:	e3a00007 	mov	r0, #7	; 0x7
    e574:	e28dd004 	add	sp, sp, #4	; 0x4
    e578:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
    e57c:	eafffef1 	b	e148 <putc>
    e580:	e3510000 	cmp	r1, #0	; 0x0
    e584:	e2821001 	add	r1, r2, #1	; 0x1
    e588:	e5801000 	str	r1, [r0]
    e58c:	0a00001a 	beq	e5fc <cread_add_char+0xd0>
    e590:	e5943000 	ldr	r3, [r4]
    e594:	e0635001 	rsb	r5, r3, r1
    e598:	e3550001 	cmp	r5, #1	; 0x1
    e59c:	9a000004 	bls	e5b4 <cread_add_char+0x88>
    e5a0:	e2830001 	add	r0, r3, #1	; 0x1
    e5a4:	e0860000 	add	r0, r6, r0
    e5a8:	e0861003 	add	r1, r6, r3
    e5ac:	e2452001 	sub	r2, r5, #1	; 0x1
    e5b0:	ebffcb6c 	bl	1368 <memmove>
    e5b4:	e5943000 	ldr	r3, [r4]
    e5b8:	e59f0074 	ldr	r0, [pc, #116]	; e634 <cread_add_char+0x108>
    e5bc:	e7c67003 	strb	r7, [r6, r3]
    e5c0:	e5942000 	ldr	r2, [r4]
    e5c4:	e59f306c 	ldr	r3, [pc, #108]	; e638 <cread_add_char+0x10c>
    e5c8:	e0862002 	add	r2, r6, r2
    e5cc:	e1a01005 	mov	r1, r5
    e5d0:	e1a0e00f 	mov	lr, pc
    e5d4:	e593f000 	ldr	pc, [r3]
    e5d8:	e5943000 	ldr	r3, [r4]
    e5dc:	e2833001 	add	r3, r3, #1	; 0x1
    e5e0:	e5843000 	str	r3, [r4]
    e5e4:	ea000000 	b	e5ec <cread_add_char+0xc0>
    e5e8:	ebfffed6 	bl	e148 <putc>
    e5ec:	e2555001 	subs	r5, r5, #1	; 0x1
    e5f0:	e3a00008 	mov	r0, #8	; 0x8
    e5f4:	1afffffb 	bne	e5e8 <cread_add_char+0xbc>
    e5f8:	ea00000b 	b	e62c <cread_add_char+0x100>
    e5fc:	e5943000 	ldr	r3, [r4]
    e600:	e59f002c 	ldr	r0, [pc, #44]	; e634 <cread_add_char+0x108>
    e604:	e7c67003 	strb	r7, [r6, r3]
    e608:	e5942000 	ldr	r2, [r4]
    e60c:	e59f3024 	ldr	r3, [pc, #36]	; e638 <cread_add_char+0x10c>
    e610:	e0862002 	add	r2, r6, r2
    e614:	e3a01001 	mov	r1, #1	; 0x1
    e618:	e1a0e00f 	mov	lr, pc
    e61c:	e593f000 	ldr	pc, [r3]
    e620:	e5943000 	ldr	r3, [r4]
    e624:	e2833001 	add	r3, r3, #1	; 0x1
    e628:	e5843000 	str	r3, [r4]
    e62c:	e28dd004 	add	sp, sp, #4	; 0x4
    e630:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    e634:	00014289 	.word	0x00014289
    e638:	0800d76c 	.word	0x0800d76c

0000e63c <readline_into_buffer>:
    e63c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    e640:	e59f367c 	ldr	r3, [pc, #1660]	; ecc4 <readline_into_buffer+0x688>
    e644:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    e648:	e593e000 	ldr	lr, [r3]
    e64c:	e1a06001 	mov	r6, r1
    e650:	e35e0000 	cmp	lr, #0	; 0x0
    e654:	1a000014 	bne	e6ac <readline_into_buffer+0x70>
    e658:	e59f3668 	ldr	r3, [pc, #1640]	; ecc8 <readline_into_buffer+0x68c>
    e65c:	e3e02000 	mvn	r2, #0	; 0x0
    e660:	e5832000 	str	r2, [r3]
    e664:	e59f3660 	ldr	r3, [pc, #1632]	; eccc <readline_into_buffer+0x690>
    e668:	e59fc660 	ldr	ip, [pc, #1632]	; ecd0 <readline_into_buffer+0x694>
    e66c:	e583e000 	str	lr, [r3]
    e670:	e59f365c 	ldr	r3, [pc, #1628]	; ecd4 <readline_into_buffer+0x698>
    e674:	e59f465c 	ldr	r4, [pc, #1628]	; ecd8 <readline_into_buffer+0x69c>
    e678:	e583e000 	str	lr, [r3]
    e67c:	e59f3658 	ldr	r3, [pc, #1624]	; ecdc <readline_into_buffer+0x6a0>
    e680:	e1a0100e 	mov	r1, lr
    e684:	e583e000 	str	lr, [r3]
    e688:	e1a0300e 	mov	r3, lr
    e68c:	e784c001 	str	ip, [r4, r1]
    e690:	e2811004 	add	r1, r1, #4	; 0x4
    e694:	e3510010 	cmp	r1, #16	; 0x10
    e698:	e4cc3100 	strb	r3, [ip], #256
    e69c:	1afffffa 	bne	e68c <readline_into_buffer+0x50>
    e6a0:	e59f361c 	ldr	r3, [pc, #1564]	; ecc4 <readline_into_buffer+0x688>
    e6a4:	e3a02001 	mov	r2, #1	; 0x1
    e6a8:	e5832000 	str	r2, [r3]
    e6ac:	ebfffe7d 	bl	e0a8 <puts>
    e6b0:	e3a03000 	mov	r3, #0	; 0x0
    e6b4:	e58d3014 	str	r3, [sp, #20]
    e6b8:	e58d3010 	str	r3, [sp, #16]
    e6bc:	e1a05003 	mov	r5, r3
    e6c0:	e3a09001 	mov	r9, #1	; 0x1
    e6c4:	e59fb614 	ldr	fp, [pc, #1556]	; ece0 <readline_into_buffer+0x6a4>
    e6c8:	e59fa5f8 	ldr	sl, [pc, #1528]	; ecc8 <readline_into_buffer+0x68c>
    e6cc:	ea000000 	b	e6d4 <readline_into_buffer+0x98>
    e6d0:	e3a05000 	mov	r5, #0	; 0x0
    e6d4:	ebfffe9d 	bl	e150 <getc>
    e6d8:	e6ef0070 	uxtb	r0, r0
    e6dc:	e350000d 	cmp	r0, #13	; 0xd
    e6e0:	1350000a 	cmpne	r0, #10	; 0xa
    e6e4:	13a04000 	movne	r4, #0	; 0x0
    e6e8:	03a04001 	moveq	r4, #1	; 0x1
    e6ec:	1a00000b 	bne	e720 <readline_into_buffer+0xe4>
    e6f0:	e59f35e8 	ldr	r3, [pc, #1512]	; ece0 <readline_into_buffer+0x6a4>
    e6f4:	e59f05e8 	ldr	r0, [pc, #1512]	; ece4 <readline_into_buffer+0x6a8>
    e6f8:	e1a0e00f 	mov	lr, pc
    e6fc:	e593f000 	ldr	pc, [r3]
    e700:	e59d5010 	ldr	r5, [sp, #16]
    e704:	e3a03000 	mov	r3, #0	; 0x0
    e708:	e7c63005 	strb	r3, [r6, r5]
    e70c:	e5d63000 	ldrb	r3, [r6]
    e710:	e3530000 	cmp	r3, #0	; 0x0
    e714:	13530021 	cmpne	r3, #33	; 0x21
    e718:	0a000162 	beq	eca8 <readline_into_buffer+0x66c>
    e71c:	ea00014e 	b	ec5c <readline_into_buffer+0x620>
    e720:	e3550000 	cmp	r5, #0	; 0x0
    e724:	0a000033 	beq	e7f8 <readline_into_buffer+0x1bc>
    e728:	e3550001 	cmp	r5, #1	; 0x1
    e72c:	1a00000c 	bne	e764 <readline_into_buffer+0x128>
    e730:	e350005b 	cmp	r0, #91	; 0x5b
    e734:	05cd0009 	strbeq	r0, [sp, #9]
    e738:	03a05002 	moveq	r5, #2	; 0x2
    e73c:	0affffe4 	beq	e6d4 <readline_into_buffer+0x98>
    e740:	e3a0cc01 	mov	ip, #256	; 0x100
    e744:	e5dd0008 	ldrb	r0, [sp, #8]
    e748:	e1a01009 	mov	r1, r9
    e74c:	e28d2014 	add	r2, sp, #20	; 0x14
    e750:	e28d3010 	add	r3, sp, #16	; 0x10
    e754:	e88d1040 	stm	sp, {r6, ip}
    e758:	e1a05004 	mov	r5, r4
    e75c:	ebffff72 	bl	e52c <cread_add_char>
    e760:	eaffffdb 	b	e6d4 <readline_into_buffer+0x98>
    e764:	e2403041 	sub	r3, r0, #65	; 0x41
    e768:	e3530007 	cmp	r3, #7	; 0x7
    e76c:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    e770:	ea000009 	b	e79c <readline_into_buffer+0x160>
    e774:	0000eae0 	.word	0x0000eae0
    e778:	0000eb20 	.word	0x0000eb20
    e77c:	0000e7e8 	.word	0x0000e7e8
    e780:	0000e8f4 	.word	0x0000e8f4
    e784:	0000e79c 	.word	0x0000e79c
    e788:	0000e79c 	.word	0x0000e79c
    e78c:	0000e79c 	.word	0x0000e79c
    e790:	0000e794 	.word	0x0000e794
    e794:	e3a05000 	mov	r5, #0	; 0x0
    e798:	ea000043 	b	e8ac <readline_into_buffer+0x270>
    e79c:	e28d1018 	add	r1, sp, #24	; 0x18
    e7a0:	e0813005 	add	r3, r1, r5
    e7a4:	e5430010 	strb	r0, [r3, #-16]
    e7a8:	e2855001 	add	r5, r5, #1	; 0x1
    e7ac:	e3a04000 	mov	r4, #0	; 0x0
    e7b0:	e28d8010 	add	r8, sp, #16	; 0x10
    e7b4:	e3a07c01 	mov	r7, #256	; 0x100
    e7b8:	ea000004 	b	e7d0 <readline_into_buffer+0x194>
    e7bc:	e28dc008 	add	ip, sp, #8	; 0x8
    e7c0:	e7dc0004 	ldrb	r0, [ip, r4]
    e7c4:	e88d00c0 	stm	sp, {r6, r7}
    e7c8:	ebffff57 	bl	e52c <cread_add_char>
    e7cc:	e2844001 	add	r4, r4, #1	; 0x1
    e7d0:	e1540005 	cmp	r4, r5
    e7d4:	e1a01009 	mov	r1, r9
    e7d8:	e28d2014 	add	r2, sp, #20	; 0x14
    e7dc:	e1a03008 	mov	r3, r8
    e7e0:	1afffff5 	bne	e7bc <readline_into_buffer+0x180>
    e7e4:	eaffffb9 	b	e6d0 <readline_into_buffer+0x94>
    e7e8:	e3a00006 	mov	r0, #6	; 0x6
    e7ec:	e350000b 	cmp	r0, #11	; 0xb
    e7f0:	e3a05000 	mov	r5, #0	; 0x0
    e7f4:	ea000001 	b	e800 <readline_into_buffer+0x1c4>
    e7f8:	e350000b 	cmp	r0, #11	; 0xb
    e7fc:	0a00005f 	beq	e980 <readline_into_buffer+0x344>
    e800:	8a00000d 	bhi	e83c <readline_into_buffer+0x200>
    e804:	e3500004 	cmp	r0, #4	; 0x4
    e808:	0a000042 	beq	e918 <readline_into_buffer+0x2dc>
    e80c:	8a000005 	bhi	e828 <readline_into_buffer+0x1ec>
    e810:	e3500002 	cmp	r0, #2	; 0x2
    e814:	0a000037 	beq	e8f8 <readline_into_buffer+0x2bc>
    e818:	8a000028 	bhi	e8c0 <readline_into_buffer+0x284>
    e81c:	e3500001 	cmp	r0, #1	; 0x1
    e820:	1a000106 	bne	ec40 <readline_into_buffer+0x604>
    e824:	ea000020 	b	e8ac <readline_into_buffer+0x270>
    e828:	e3500006 	cmp	r0, #6	; 0x6
    e82c:	0a000027 	beq	e8d0 <readline_into_buffer+0x294>
    e830:	3a000066 	bcc	e9d0 <readline_into_buffer+0x394>
    e834:	e3500008 	cmp	r0, #8	; 0x8
    e838:	ea000015 	b	e894 <readline_into_buffer+0x258>
    e83c:	e3500015 	cmp	r0, #21	; 0x15
    e840:	0a000070 	beq	ea08 <readline_into_buffer+0x3cc>
    e844:	8a000007 	bhi	e868 <readline_into_buffer+0x22c>
    e848:	e350000f 	cmp	r0, #15	; 0xf
    e84c:	02299001 	eoreq	r9, r9, #1	; 0x1
    e850:	0affff9f 	beq	e6d4 <readline_into_buffer+0x98>
    e854:	e3500010 	cmp	r0, #16	; 0x10
    e858:	0a0000a0 	beq	eae0 <readline_into_buffer+0x4a4>
    e85c:	e350000e 	cmp	r0, #14	; 0xe
    e860:	1a0000f6 	bne	ec40 <readline_into_buffer+0x604>
    e864:	ea0000ad 	b	eb20 <readline_into_buffer+0x4e4>
    e868:	e350001b 	cmp	r0, #27	; 0x1b
    e86c:	05cd0008 	strbeq	r0, [sp, #8]
    e870:	03a05001 	moveq	r5, #1	; 0x1
    e874:	0affff96 	beq	e6d4 <readline_into_buffer+0x98>
    e878:	8a000002 	bhi	e888 <readline_into_buffer+0x24c>
    e87c:	e3500018 	cmp	r0, #24	; 0x18
    e880:	1a0000ee 	bne	ec40 <readline_into_buffer+0x604>
    e884:	ea00005f 	b	ea08 <readline_into_buffer+0x3cc>
    e888:	e350007f 	cmp	r0, #127	; 0x7f
    e88c:	0a000075 	beq	ea68 <readline_into_buffer+0x42c>
    e890:	e35000ff 	cmp	r0, #255	; 0xff
    e894:	1a0000e9 	bne	ec40 <readline_into_buffer+0x604>
    e898:	ea000072 	b	ea68 <readline_into_buffer+0x42c>
    e89c:	ebfffe29 	bl	e148 <putc>
    e8a0:	e59d3014 	ldr	r3, [sp, #20]
    e8a4:	e2433001 	sub	r3, r3, #1	; 0x1
    e8a8:	e58d3014 	str	r3, [sp, #20]
    e8ac:	e59d3014 	ldr	r3, [sp, #20]
    e8b0:	e3a00008 	mov	r0, #8	; 0x8
    e8b4:	e3530000 	cmp	r3, #0	; 0x0
    e8b8:	1afffff7 	bne	e89c <readline_into_buffer+0x260>
    e8bc:	eaffff84 	b	e6d4 <readline_into_buffer+0x98>
    e8c0:	e3a03000 	mov	r3, #0	; 0x0
    e8c4:	e5c63000 	strb	r3, [r6]
    e8c8:	e3e00000 	mvn	r0, #0	; 0x0
    e8cc:	ea0000fa 	b	ecbc <readline_into_buffer+0x680>
    e8d0:	e59d2014 	ldr	r2, [sp, #20]
    e8d4:	e59d3010 	ldr	r3, [sp, #16]
    e8d8:	e1520003 	cmp	r2, r3
    e8dc:	2affff7c 	bcs	e6d4 <readline_into_buffer+0x98>
    e8e0:	e7d60002 	ldrb	r0, [r6, r2]
    e8e4:	ebfffe17 	bl	e148 <putc>
    e8e8:	e59d3014 	ldr	r3, [sp, #20]
    e8ec:	e2833001 	add	r3, r3, #1	; 0x1
    e8f0:	ea0000d0 	b	ec38 <readline_into_buffer+0x5fc>
    e8f4:	e3a05000 	mov	r5, #0	; 0x0
    e8f8:	e59d3014 	ldr	r3, [sp, #20]
    e8fc:	e3530000 	cmp	r3, #0	; 0x0
    e900:	0affff73 	beq	e6d4 <readline_into_buffer+0x98>
    e904:	e3a00008 	mov	r0, #8	; 0x8
    e908:	ebfffe0e 	bl	e148 <putc>
    e90c:	e59d3014 	ldr	r3, [sp, #20]
    e910:	e2433001 	sub	r3, r3, #1	; 0x1
    e914:	ea0000c7 	b	ec38 <readline_into_buffer+0x5fc>
    e918:	e59d2014 	ldr	r2, [sp, #20]
    e91c:	e59d3010 	ldr	r3, [sp, #16]
    e920:	e1520003 	cmp	r2, r3
    e924:	2affff6a 	bcs	e6d4 <readline_into_buffer+0x98>
    e928:	e2433001 	sub	r3, r3, #1	; 0x1
    e92c:	e0534002 	subs	r4, r3, r2
    e930:	0a00000a 	beq	e960 <readline_into_buffer+0x324>
    e934:	e2821001 	add	r1, r2, #1	; 0x1
    e938:	e0861001 	add	r1, r6, r1
    e93c:	e0860002 	add	r0, r6, r2
    e940:	e1a02004 	mov	r2, r4
    e944:	ebffca87 	bl	1368 <memmove>
    e948:	e59d2014 	ldr	r2, [sp, #20]
    e94c:	e59f0394 	ldr	r0, [pc, #916]	; ece8 <readline_into_buffer+0x6ac>
    e950:	e0862002 	add	r2, r6, r2
    e954:	e1a01004 	mov	r1, r4
    e958:	e1a0e00f 	mov	lr, pc
    e95c:	e59bf000 	ldr	pc, [fp]
    e960:	e3a00020 	mov	r0, #32	; 0x20
    e964:	ebfffdf7 	bl	e148 <putc>
    e968:	e2444001 	sub	r4, r4, #1	; 0x1
    e96c:	e3a00008 	mov	r0, #8	; 0x8
    e970:	ebfffdf4 	bl	e148 <putc>
    e974:	e3740001 	cmn	r4, #1	; 0x1
    e978:	1afffffa 	bne	e968 <readline_into_buffer+0x32c>
    e97c:	ea000053 	b	ead0 <readline_into_buffer+0x494>
    e980:	e59d2014 	ldr	r2, [sp, #20]
    e984:	e59d3010 	ldr	r3, [sp, #16]
    e988:	e1520003 	cmp	r2, r3
    e98c:	31a04002 	movcc	r4, r2
    e990:	3a000002 	bcc	e9a0 <readline_into_buffer+0x364>
    e994:	eaffff4e 	b	e6d4 <readline_into_buffer+0x98>
    e998:	ebfffdea 	bl	e148 <putc>
    e99c:	e2844001 	add	r4, r4, #1	; 0x1
    e9a0:	e59d3010 	ldr	r3, [sp, #16]
    e9a4:	e3a00020 	mov	r0, #32	; 0x20
    e9a8:	e1540003 	cmp	r4, r3
    e9ac:	3afffff9 	bcc	e998 <readline_into_buffer+0x35c>
    e9b0:	ea000000 	b	e9b8 <readline_into_buffer+0x37c>
    e9b4:	ebfffde3 	bl	e148 <putc>
    e9b8:	e59d2014 	ldr	r2, [sp, #20]
    e9bc:	e3a00008 	mov	r0, #8	; 0x8
    e9c0:	e1540002 	cmp	r4, r2
    e9c4:	e2444001 	sub	r4, r4, #1	; 0x1
    e9c8:	8afffff9 	bhi	e9b4 <readline_into_buffer+0x378>
    e9cc:	ea000023 	b	ea60 <readline_into_buffer+0x424>
    e9d0:	e59d3014 	ldr	r3, [sp, #20]
    e9d4:	e59d1010 	ldr	r1, [sp, #16]
    e9d8:	e1530001 	cmp	r3, r1
    e9dc:	2affff3c 	bcs	e6d4 <readline_into_buffer+0x98>
    e9e0:	e0631001 	rsb	r1, r3, r1
    e9e4:	e0862003 	add	r2, r6, r3
    e9e8:	e59f02f8 	ldr	r0, [pc, #760]	; ece8 <readline_into_buffer+0x6ac>
    e9ec:	e1a0e00f 	mov	lr, pc
    e9f0:	e59bf000 	ldr	pc, [fp]
    e9f4:	ea00008e 	b	ec34 <readline_into_buffer+0x5f8>
    e9f8:	ebfffdd2 	bl	e148 <putc>
    e9fc:	e59d3014 	ldr	r3, [sp, #20]
    ea00:	e2433001 	sub	r3, r3, #1	; 0x1
    ea04:	e58d3014 	str	r3, [sp, #20]
    ea08:	e59d2014 	ldr	r2, [sp, #20]
    ea0c:	e3a00008 	mov	r0, #8	; 0x8
    ea10:	e3520000 	cmp	r2, #0	; 0x0
    ea14:	1afffff7 	bne	e9f8 <readline_into_buffer+0x3bc>
    ea18:	e59d3010 	ldr	r3, [sp, #16]
    ea1c:	e3530000 	cmp	r3, #0	; 0x0
    ea20:	11a04002 	movne	r4, r2
    ea24:	1a000002 	bne	ea34 <readline_into_buffer+0x3f8>
    ea28:	eaffff29 	b	e6d4 <readline_into_buffer+0x98>
    ea2c:	ebfffdc5 	bl	e148 <putc>
    ea30:	e2844001 	add	r4, r4, #1	; 0x1
    ea34:	e59d3010 	ldr	r3, [sp, #16]
    ea38:	e3a00020 	mov	r0, #32	; 0x20
    ea3c:	e1540003 	cmp	r4, r3
    ea40:	3afffff9 	bcc	ea2c <readline_into_buffer+0x3f0>
    ea44:	ea000000 	b	ea4c <readline_into_buffer+0x410>
    ea48:	ebfffdbe 	bl	e148 <putc>
    ea4c:	e59d2014 	ldr	r2, [sp, #20]
    ea50:	e3a00008 	mov	r0, #8	; 0x8
    ea54:	e1540002 	cmp	r4, r2
    ea58:	e2444001 	sub	r4, r4, #1	; 0x1
    ea5c:	8afffff9 	bhi	ea48 <readline_into_buffer+0x40c>
    ea60:	e58d2010 	str	r2, [sp, #16]
    ea64:	eaffff1a 	b	e6d4 <readline_into_buffer+0x98>
    ea68:	e59d0014 	ldr	r0, [sp, #20]
    ea6c:	e3500000 	cmp	r0, #0	; 0x0
    ea70:	0affff17 	beq	e6d4 <readline_into_buffer+0x98>
    ea74:	e59d2010 	ldr	r2, [sp, #16]
    ea78:	e2403001 	sub	r3, r0, #1	; 0x1
    ea7c:	e0604002 	rsb	r4, r0, r2
    ea80:	e0861000 	add	r1, r6, r0
    ea84:	e1a02004 	mov	r2, r4
    ea88:	e0860003 	add	r0, r6, r3
    ea8c:	e58d3014 	str	r3, [sp, #20]
    ea90:	ebffca34 	bl	1368 <memmove>
    ea94:	e3a00008 	mov	r0, #8	; 0x8
    ea98:	ebfffdaa 	bl	e148 <putc>
    ea9c:	e59d2014 	ldr	r2, [sp, #20]
    eaa0:	e59f0240 	ldr	r0, [pc, #576]	; ece8 <readline_into_buffer+0x6ac>
    eaa4:	e0862002 	add	r2, r6, r2
    eaa8:	e1a01004 	mov	r1, r4
    eaac:	e1a0e00f 	mov	lr, pc
    eab0:	e59bf000 	ldr	pc, [fp]
    eab4:	e3a00020 	mov	r0, #32	; 0x20
    eab8:	ebfffda2 	bl	e148 <putc>
    eabc:	e2444001 	sub	r4, r4, #1	; 0x1
    eac0:	e3a00008 	mov	r0, #8	; 0x8
    eac4:	ebfffd9f 	bl	e148 <putc>
    eac8:	e3740001 	cmn	r4, #1	; 0x1
    eacc:	1afffffa 	bne	eabc <readline_into_buffer+0x480>
    ead0:	e59d3010 	ldr	r3, [sp, #16]
    ead4:	e2433001 	sub	r3, r3, #1	; 0x1
    ead8:	e58d3010 	str	r3, [sp, #16]
    eadc:	eafffefc 	b	e6d4 <readline_into_buffer+0x98>
    eae0:	e59a2000 	ldr	r2, [sl]
    eae4:	e3520000 	cmp	r2, #0	; 0x0
    eae8:	ba000024 	blt	eb80 <readline_into_buffer+0x544>
    eaec:	e2423001 	sub	r3, r2, #1	; 0x1
    eaf0:	e3730001 	cmn	r3, #1	; 0x1
    eaf4:	059f01d0 	ldreq	r0, [pc, #464]	; eccc <readline_into_buffer+0x690>
    eaf8:	e58a3000 	str	r3, [sl]
    eafc:	05903000 	ldreq	r3, [r0]
    eb00:	e59fc1cc 	ldr	ip, [pc, #460]	; ecd4 <readline_into_buffer+0x698>
    eb04:	058a3000 	streq	r3, [sl]
    eb08:	e59a1000 	ldr	r1, [sl]
    eb0c:	e59c3000 	ldr	r3, [ip]
    eb10:	e1510003 	cmp	r1, r3
    eb14:	058a2000 	streq	r2, [sl]
    eb18:	1a000014 	bne	eb70 <readline_into_buffer+0x534>
    eb1c:	ea000017 	b	eb80 <readline_into_buffer+0x544>
    eb20:	e59a1000 	ldr	r1, [sl]
    eb24:	e3510000 	cmp	r1, #0	; 0x0
    eb28:	ba000014 	blt	eb80 <readline_into_buffer+0x544>
    eb2c:	e59f21a0 	ldr	r2, [pc, #416]	; ecd4 <readline_into_buffer+0x698>
    eb30:	e5923000 	ldr	r3, [r2]
    eb34:	e1510003 	cmp	r1, r3
    eb38:	0a000010 	beq	eb80 <readline_into_buffer+0x544>
    eb3c:	e59f3188 	ldr	r3, [pc, #392]	; eccc <readline_into_buffer+0x690>
    eb40:	e59fc18c 	ldr	ip, [pc, #396]	; ecd4 <readline_into_buffer+0x698>
    eb44:	e5932000 	ldr	r2, [r3]
    eb48:	e2813001 	add	r3, r1, #1	; 0x1
    eb4c:	e1530002 	cmp	r3, r2
    eb50:	e58a3000 	str	r3, [sl]
    eb54:	c3a03000 	movgt	r3, #0	; 0x0
    eb58:	c58a3000 	strgt	r3, [sl]
    eb5c:	e59a1000 	ldr	r1, [sl]
    eb60:	e59c3000 	ldr	r3, [ip]
    eb64:	e1510003 	cmp	r1, r3
    eb68:	059f517c 	ldreq	r5, [pc, #380]	; ecec <readline_into_buffer+0x6b0>
    eb6c:	0a00000a 	beq	eb9c <readline_into_buffer+0x560>
    eb70:	e59f0160 	ldr	r0, [pc, #352]	; ecd8 <readline_into_buffer+0x69c>
    eb74:	e7905101 	ldr	r5, [r0, r1, lsl #2]
    eb78:	e3550000 	cmp	r5, #0	; 0x0
    eb7c:	1a000006 	bne	eb9c <readline_into_buffer+0x560>
    eb80:	e3a00007 	mov	r0, #7	; 0x7
    eb84:	ebfffd6f 	bl	e148 <putc>
    eb88:	eafffed0 	b	e6d0 <readline_into_buffer+0x94>
    eb8c:	ebfffd6d 	bl	e148 <putc>
    eb90:	e59d3014 	ldr	r3, [sp, #20]
    eb94:	e2433001 	sub	r3, r3, #1	; 0x1
    eb98:	e58d3014 	str	r3, [sp, #20]
    eb9c:	e59d2014 	ldr	r2, [sp, #20]
    eba0:	e3a00008 	mov	r0, #8	; 0x8
    eba4:	e3520000 	cmp	r2, #0	; 0x0
    eba8:	1afffff7 	bne	eb8c <readline_into_buffer+0x550>
    ebac:	e59d3010 	ldr	r3, [sp, #16]
    ebb0:	e3530000 	cmp	r3, #0	; 0x0
    ebb4:	11a04002 	movne	r4, r2
    ebb8:	1a000002 	bne	ebc8 <readline_into_buffer+0x58c>
    ebbc:	ea00000d 	b	ebf8 <readline_into_buffer+0x5bc>
    ebc0:	ebfffd60 	bl	e148 <putc>
    ebc4:	e2844001 	add	r4, r4, #1	; 0x1
    ebc8:	e59d3010 	ldr	r3, [sp, #16]
    ebcc:	e3a00020 	mov	r0, #32	; 0x20
    ebd0:	e1540003 	cmp	r4, r3
    ebd4:	3afffff9 	bcc	ebc0 <readline_into_buffer+0x584>
    ebd8:	ea000000 	b	ebe0 <readline_into_buffer+0x5a4>
    ebdc:	ebfffd59 	bl	e148 <putc>
    ebe0:	e59d2014 	ldr	r2, [sp, #20]
    ebe4:	e3a00008 	mov	r0, #8	; 0x8
    ebe8:	e1540002 	cmp	r4, r2
    ebec:	e2444001 	sub	r4, r4, #1	; 0x1
    ebf0:	8afffff9 	bhi	ebdc <readline_into_buffer+0x5a0>
    ebf4:	e58d2010 	str	r2, [sp, #16]
    ebf8:	e1a01005 	mov	r1, r5
    ebfc:	e1a00006 	mov	r0, r6
    ec00:	ebffc95e 	bl	1180 <strcpy>
    ec04:	e1a00006 	mov	r0, r6
    ec08:	ebffc9b3 	bl	12dc <strlen>
    ec0c:	e59d3014 	ldr	r3, [sp, #20]
    ec10:	e58d0010 	str	r0, [sp, #16]
    ec14:	e1530000 	cmp	r3, r0
    ec18:	2afffeac 	bcs	e6d0 <readline_into_buffer+0x94>
    ec1c:	e0631000 	rsb	r1, r3, r0
    ec20:	e0862003 	add	r2, r6, r3
    ec24:	e59f00bc 	ldr	r0, [pc, #188]	; ece8 <readline_into_buffer+0x6ac>
    ec28:	e1a0e00f 	mov	lr, pc
    ec2c:	e59bf000 	ldr	pc, [fp]
    ec30:	e3a05000 	mov	r5, #0	; 0x0
    ec34:	e59d3010 	ldr	r3, [sp, #16]
    ec38:	e58d3014 	str	r3, [sp, #20]
    ec3c:	eafffea4 	b	e6d4 <readline_into_buffer+0x98>
    ec40:	e3a0cc01 	mov	ip, #256	; 0x100
    ec44:	e1a01009 	mov	r1, r9
    ec48:	e28d2014 	add	r2, sp, #20	; 0x14
    ec4c:	e28d3010 	add	r3, sp, #16	; 0x10
    ec50:	e88d1040 	stm	sp, {r6, ip}
    ec54:	ebfffe34 	bl	e52c <cread_add_char>
    ec58:	eafffe9d 	b	e6d4 <readline_into_buffer+0x98>
    ec5c:	e59f4070 	ldr	r4, [pc, #112]	; ecd4 <readline_into_buffer+0x698>
    ec60:	e59f3070 	ldr	r3, [pc, #112]	; ecd8 <readline_into_buffer+0x69c>
    ec64:	e5942000 	ldr	r2, [r4]
    ec68:	e1a01006 	mov	r1, r6
    ec6c:	e7930102 	ldr	r0, [r3, r2, lsl #2]
    ec70:	ebffc942 	bl	1180 <strcpy>
    ec74:	e5943000 	ldr	r3, [r4]
    ec78:	e59f104c 	ldr	r1, [pc, #76]	; eccc <readline_into_buffer+0x690>
    ec7c:	e2833001 	add	r3, r3, #1	; 0x1
    ec80:	e5912000 	ldr	r2, [r1]
    ec84:	e3530003 	cmp	r3, #3	; 0x3
    ec88:	c3a03000 	movgt	r3, #0	; 0x0
    ec8c:	e1530002 	cmp	r3, r2
    ec90:	e59f2044 	ldr	r2, [pc, #68]	; ecdc <readline_into_buffer+0x6a0>
    ec94:	e5843000 	str	r3, [r4]
    ec98:	c5813000 	strgt	r3, [r1]
    ec9c:	e5923000 	ldr	r3, [r2]
    eca0:	e2833001 	add	r3, r3, #1	; 0x1
    eca4:	e5823000 	str	r3, [r2]
    eca8:	e59f3024 	ldr	r3, [pc, #36]	; ecd4 <readline_into_buffer+0x698>
    ecac:	e1a00005 	mov	r0, r5
    ecb0:	e5932000 	ldr	r2, [r3]
    ecb4:	e59f300c 	ldr	r3, [pc, #12]	; ecc8 <readline_into_buffer+0x68c>
    ecb8:	e5832000 	str	r2, [r3]
    ecbc:	e28dd01c 	add	sp, sp, #28	; 0x1c
    ecc0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ecc4:	0800d314 	.word	0x0800d314
    ecc8:	0800d19c 	.word	0x0800d19c
    eccc:	0800d318 	.word	0x0800d318
    ecd0:	0800d8fc 	.word	0x0800d8fc
    ecd4:	0800d31c 	.word	0x0800d31c
    ecd8:	0800d8ec 	.word	0x0800d8ec
    ecdc:	0800d310 	.word	0x0800d310
    ece0:	0800d76c 	.word	0x0800d76c
    ece4:	00012472 	.word	0x00012472
    ece8:	00014289 	.word	0x00014289
    ecec:	00012473 	.word	0x00012473

0000ecf0 <readline>:
    ecf0:	e59f1000 	ldr	r1, [pc, #0]	; ecf8 <readline+0x8>
    ecf4:	eafffe50 	b	e63c <readline_into_buffer>
    ecf8:	0800d7ec 	.word	0x0800d7ec

0000ecfc <main_loop>:
    ecfc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ed00:	e24dd004 	sub	sp, sp, #4	; 0x4
    ed04:	e59f0030 	ldr	r0, [pc, #48]	; ed3c <main_loop+0x40>
    ed08:	ebfffff8 	bl	ecf0 <readline>
    ed0c:	e1a03000 	mov	r3, r0
    ed10:	e3730001 	cmn	r3, #1	; 0x1
    ed14:	e3a01000 	mov	r1, #0	; 0x0
    ed18:	e59f0020 	ldr	r0, [pc, #32]	; ed40 <main_loop+0x44>
    ed1c:	1a000002 	bne	ed2c <main_loop+0x30>
    ed20:	e59f001c 	ldr	r0, [pc, #28]	; ed44 <main_loop+0x48>
    ed24:	ebfffcdf 	bl	e0a8 <puts>
    ed28:	eafffff5 	b	ed04 <main_loop+0x8>
    ed2c:	e3530000 	cmp	r3, #0	; 0x0
    ed30:	dafffff3 	ble	ed04 <main_loop+0x8>
    ed34:	ebfffd41 	bl	e240 <run_command>
    ed38:	eafffff1 	b	ed04 <main_loop+0x8>
    ed3c:	000142a9 	.word	0x000142a9
    ed40:	0800d7ec 	.word	0x0800d7ec
    ed44:	00012472 	.word	0x00012472

0000ed48 <parse_num>:
    ed48:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    ed4c:	e1a08001 	mov	r8, r1
    ed50:	e1a07002 	mov	r7, r2
    ed54:	e1a06003 	mov	r6, r3
    ed58:	ea000000 	b	ed60 <parse_num+0x18>
    ed5c:	e2800001 	add	r0, r0, #1	; 0x1
    ed60:	e5d03000 	ldrb	r3, [r0]
    ed64:	e3530020 	cmp	r3, #32	; 0x20
    ed68:	0afffffb 	beq	ed5c <parse_num+0x14>
    ed6c:	ea000037 	b	ee50 <parse_num+0x108>
    ed70:	e3520000 	cmp	r2, #0	; 0x0
    ed74:	0a00000a 	beq	eda4 <parse_num+0x5c>
    ed78:	e3530030 	cmp	r3, #48	; 0x30
    ed7c:	1a000008 	bne	eda4 <parse_num+0x5c>
    ed80:	e5d02001 	ldrb	r2, [r0, #1]
    ed84:	e2423041 	sub	r3, r2, #65	; 0x41
    ed88:	e6ef3073 	uxtb	r3, r3
    ed8c:	e3530019 	cmp	r3, #25	; 0x19
    ed90:	92823020 	addls	r3, r2, #32	; 0x20
    ed94:	96ef2073 	uxtbls	r2, r3
    ed98:	e3520078 	cmp	r2, #120	; 0x78
    ed9c:	02800002 	addeq	r0, r0, #2	; 0x2
    eda0:	03a04010 	moveq	r4, #16	; 0x10
    eda4:	e4d0c001 	ldrb	ip, [r0], #1
    eda8:	e24c3030 	sub	r3, ip, #48	; 0x30
    edac:	e24c2041 	sub	r2, ip, #65	; 0x41
    edb0:	e6ef3073 	uxtb	r3, r3
    edb4:	e6ef2072 	uxtb	r2, r2
    edb8:	e3530009 	cmp	r3, #9	; 0x9
    edbc:	83a01000 	movhi	r1, #0	; 0x0
    edc0:	93a01001 	movls	r1, #1	; 0x1
    edc4:	e3520005 	cmp	r2, #5	; 0x5
    edc8:	83a02000 	movhi	r2, #0	; 0x0
    edcc:	93a02001 	movls	r2, #1	; 0x1
    edd0:	e1913002 	orrs	r3, r1, r2
    edd4:	1a000003 	bne	ede8 <parse_num+0xa0>
    edd8:	e24c3061 	sub	r3, ip, #97	; 0x61
    eddc:	e6ef3073 	uxtb	r3, r3
    ede0:	e3530005 	cmp	r3, #5	; 0x5
    ede4:	8a00000f 	bhi	ee28 <parse_num+0xe0>
    ede8:	e3510000 	cmp	r1, #0	; 0x0
    edec:	124c2030 	subne	r2, ip, #48	; 0x30
    edf0:	1a000007 	bne	ee14 <parse_num+0xcc>
    edf4:	e24c3061 	sub	r3, ip, #97	; 0x61
    edf8:	e6ef3073 	uxtb	r3, r3
    edfc:	e3530005 	cmp	r3, #5	; 0x5
    ee00:	924c2057 	subls	r2, ip, #87	; 0x57
    ee04:	9a000002 	bls	ee14 <parse_num+0xcc>
    ee08:	e3520000 	cmp	r2, #0	; 0x0
    ee0c:	0a000002 	beq	ee1c <parse_num+0xd4>
    ee10:	e24c2037 	sub	r2, ip, #55	; 0x37
    ee14:	e1520004 	cmp	r2, r4
    ee18:	aa000002 	bge	ee28 <parse_num+0xe0>
    ee1c:	e0252495 	mla	r5, r5, r4, r2
    ee20:	e3a02000 	mov	r2, #0	; 0x0
    ee24:	ea00000c 	b	ee5c <parse_num+0x114>
    ee28:	e3560000 	cmp	r6, #0	; 0x0
    ee2c:	0a000012 	beq	ee7c <parse_num+0x134>
    ee30:	e3a03000 	mov	r3, #0	; 0x0
    ee34:	e7d62003 	ldrb	r2, [r6, r3]
    ee38:	e2833001 	add	r3, r3, #1	; 0x1
    ee3c:	e3520000 	cmp	r2, #0	; 0x0
    ee40:	0a00000d 	beq	ee7c <parse_num+0x134>
    ee44:	e15c0002 	cmp	ip, r2
    ee48:	1afffff9 	bne	ee34 <parse_num+0xec>
    ee4c:	ea000005 	b	ee68 <parse_num+0x120>
    ee50:	e3a02001 	mov	r2, #1	; 0x1
    ee54:	e3a0400a 	mov	r4, #10	; 0xa
    ee58:	e3a05000 	mov	r5, #0	; 0x0
    ee5c:	e5d03000 	ldrb	r3, [r0]
    ee60:	e3530000 	cmp	r3, #0	; 0x0
    ee64:	1affffc1 	bne	ed70 <parse_num+0x28>
    ee68:	e3570000 	cmp	r7, #0	; 0x0
    ee6c:	15870000 	strne	r0, [r7]
    ee70:	e5885000 	str	r5, [r8]
    ee74:	e3a00001 	mov	r0, #1	; 0x1
    ee78:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    ee7c:	e3a00000 	mov	r0, #0	; 0x0
    ee80:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000ee84 <xyzModem_stream_close>:
    ee84:	e59f2044 	ldr	r2, [pc, #68]	; eed0 <xyzModem_stream_close+0x4c>
    ee88:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ee8c:	e5920428 	ldr	r0, [r2, #1064]
    ee90:	e59f303c 	ldr	r3, [pc, #60]	; eed4 <xyzModem_stream_close+0x50>
    ee94:	e592c424 	ldr	ip, [r2, #1060]
    ee98:	e592e418 	ldr	lr, [r2, #1048]
    ee9c:	e3500000 	cmp	r0, #0	; 0x0
    eea0:	e24dd00c 	sub	sp, sp, #12	; 0xc
    eea4:	e59f102c 	ldr	r1, [pc, #44]	; eed8 <xyzModem_stream_close+0x54>
    eea8:	01a01003 	moveq	r1, r3
    eeac:	e59f3028 	ldr	r3, [pc, #40]	; eedc <xyzModem_stream_close+0x58>
    eeb0:	e88d5000 	stm	sp, {ip, lr}
    eeb4:	e593c000 	ldr	ip, [r3]
    eeb8:	e59f0020 	ldr	r0, [pc, #32]	; eee0 <xyzModem_stream_close+0x5c>
    eebc:	e5923420 	ldr	r3, [r2, #1056]
    eec0:	e592241c 	ldr	r2, [r2, #1052]
    eec4:	e12fff3c 	blx	ip
    eec8:	e28dd00c 	add	sp, sp, #12	; 0xc
    eecc:	e8bd8000 	pop	{pc}
    eed0:	0800d330 	.word	0x0800d330
    eed4:	000142ad 	.word	0x000142ad
    eed8:	000142b3 	.word	0x000142b3
    eedc:	0800d76c 	.word	0x0800d76c
    eee0:	000142b7 	.word	0x000142b7

0000eee4 <xyzModem_error>:
    eee4:	e2800008 	add	r0, r0, #8	; 0x8
    eee8:	e3500007 	cmp	r0, #7	; 0x7
    eeec:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    eef0:	ea000017 	b	ef54 <xyzModem_error+0x70>
    eef4:	0000ef4c 	.word	0x0000ef4c
    eef8:	0000ef44 	.word	0x0000ef44
    eefc:	0000ef3c 	.word	0x0000ef3c
    ef00:	0000ef34 	.word	0x0000ef34
    ef04:	0000ef2c 	.word	0x0000ef2c
    ef08:	0000ef24 	.word	0x0000ef24
    ef0c:	0000ef14 	.word	0x0000ef14
    ef10:	0000ef1c 	.word	0x0000ef1c
    ef14:	e59f0040 	ldr	r0, [pc, #64]	; ef5c <xyzModem_error+0x78>
    ef18:	e12fff1e 	bx	lr
    ef1c:	e59f003c 	ldr	r0, [pc, #60]	; ef60 <xyzModem_error+0x7c>
    ef20:	e12fff1e 	bx	lr
    ef24:	e59f0038 	ldr	r0, [pc, #56]	; ef64 <xyzModem_error+0x80>
    ef28:	e12fff1e 	bx	lr
    ef2c:	e59f0034 	ldr	r0, [pc, #52]	; ef68 <xyzModem_error+0x84>
    ef30:	e12fff1e 	bx	lr
    ef34:	e59f0030 	ldr	r0, [pc, #48]	; ef6c <xyzModem_error+0x88>
    ef38:	e12fff1e 	bx	lr
    ef3c:	e59f002c 	ldr	r0, [pc, #44]	; ef70 <xyzModem_error+0x8c>
    ef40:	e12fff1e 	bx	lr
    ef44:	e59f0028 	ldr	r0, [pc, #40]	; ef74 <xyzModem_error+0x90>
    ef48:	e12fff1e 	bx	lr
    ef4c:	e59f0024 	ldr	r0, [pc, #36]	; ef78 <xyzModem_error+0x94>
    ef50:	e12fff1e 	bx	lr
    ef54:	e59f0020 	ldr	r0, [pc, #32]	; ef7c <xyzModem_error+0x98>
    ef58:	e12fff1e 	bx	lr
    ef5c:	000142f8 	.word	0x000142f8
    ef60:	00014318 	.word	0x00014318
    ef64:	0001432a 	.word	0x0001432a
    ef68:	00014334 	.word	0x00014334
    ef6c:	00014340 	.word	0x00014340
    ef70:	0001434a 	.word	0x0001434a
    ef74:	0001435a 	.word	0x0001435a
    ef78:	0001436d 	.word	0x0001436d
    ef7c:	00014382 	.word	0x00014382

0000ef80 <delay>:
    ef80:	e12fff1e 	bx	lr

0000ef84 <init_num>:
    ef84:	e59f305c 	ldr	r3, [pc, #92]	; efe8 <init_num+0x64>
    ef88:	e3a01001 	mov	r1, #1	; 0x1
    ef8c:	e5c31000 	strb	r1, [r3]
    ef90:	e59f3054 	ldr	r3, [pc, #84]	; efec <init_num+0x68>
    ef94:	e3a02000 	mov	r2, #0	; 0x0
    ef98:	e5c32000 	strb	r2, [r3]
    ef9c:	e59f304c 	ldr	r3, [pc, #76]	; eff0 <init_num+0x6c>
    efa0:	e5c32000 	strb	r2, [r3]
    efa4:	e59f3048 	ldr	r3, [pc, #72]	; eff4 <init_num+0x70>
    efa8:	e5c32000 	strb	r2, [r3]
    efac:	e59f3044 	ldr	r3, [pc, #68]	; eff8 <init_num+0x74>
    efb0:	e5c32000 	strb	r2, [r3]
    efb4:	e59f3040 	ldr	r3, [pc, #64]	; effc <init_num+0x78>
    efb8:	e5832000 	str	r2, [r3]
    efbc:	e59f303c 	ldr	r3, [pc, #60]	; f000 <init_num+0x7c>
    efc0:	e5c32000 	strb	r2, [r3]
    efc4:	e59f3038 	ldr	r3, [pc, #56]	; f004 <init_num+0x80>
    efc8:	e5c32000 	strb	r2, [r3]
    efcc:	e59f3034 	ldr	r3, [pc, #52]	; f008 <init_num+0x84>
    efd0:	e5c32000 	strb	r2, [r3]
    efd4:	e59f3030 	ldr	r3, [pc, #48]	; f00c <init_num+0x88>
    efd8:	e5c32000 	strb	r2, [r3]
    efdc:	e59f302c 	ldr	r3, [pc, #44]	; f010 <init_num+0x8c>
    efe0:	e5c32000 	strb	r2, [r3]
    efe4:	e12fff1e 	bx	lr
    efe8:	0800d1a0 	.word	0x0800d1a0
    efec:	0800d32d 	.word	0x0800d32d
    eff0:	0800d320 	.word	0x0800d320
    eff4:	0800d321 	.word	0x0800d321
    eff8:	0800d322 	.word	0x0800d322
    effc:	0800d324 	.word	0x0800d324
    f000:	0800d328 	.word	0x0800d328
    f004:	0800d329 	.word	0x0800d329
    f008:	0800d32a 	.word	0x0800d32a
    f00c:	0800d32b 	.word	0x0800d32b
    f010:	0800d32c 	.word	0x0800d32c

0000f014 <out_message>:
    f014:	e92d4010 	push	{r4, lr}
    f018:	e59f30c8 	ldr	r3, [pc, #200]	; f0e8 <out_message+0xd4>
    f01c:	e59f40c8 	ldr	r4, [pc, #200]	; f0ec <out_message+0xd8>
    f020:	e5d31000 	ldrb	r1, [r3]
    f024:	e59f00c4 	ldr	r0, [pc, #196]	; f0f0 <out_message+0xdc>
    f028:	e1a0e00f 	mov	lr, pc
    f02c:	e594f000 	ldr	pc, [r4]
    f030:	e59f30bc 	ldr	r3, [pc, #188]	; f0f4 <out_message+0xe0>
    f034:	e59f00bc 	ldr	r0, [pc, #188]	; f0f8 <out_message+0xe4>
    f038:	e5d31000 	ldrb	r1, [r3]
    f03c:	e1a0e00f 	mov	lr, pc
    f040:	e594f000 	ldr	pc, [r4]
    f044:	e59f30b0 	ldr	r3, [pc, #176]	; f0fc <out_message+0xe8>
    f048:	e59f00b0 	ldr	r0, [pc, #176]	; f100 <out_message+0xec>
    f04c:	e5931000 	ldr	r1, [r3]
    f050:	e1a0e00f 	mov	lr, pc
    f054:	e594f000 	ldr	pc, [r4]
    f058:	e59f30a4 	ldr	r3, [pc, #164]	; f104 <out_message+0xf0>
    f05c:	e59f00a4 	ldr	r0, [pc, #164]	; f108 <out_message+0xf4>
    f060:	e5d31000 	ldrb	r1, [r3]
    f064:	e1a0e00f 	mov	lr, pc
    f068:	e594f000 	ldr	pc, [r4]
    f06c:	e59f3098 	ldr	r3, [pc, #152]	; f10c <out_message+0xf8>
    f070:	e59f0098 	ldr	r0, [pc, #152]	; f110 <out_message+0xfc>
    f074:	e5d31000 	ldrb	r1, [r3]
    f078:	e1a0e00f 	mov	lr, pc
    f07c:	e594f000 	ldr	pc, [r4]
    f080:	e59f308c 	ldr	r3, [pc, #140]	; f114 <out_message+0x100>
    f084:	e59f008c 	ldr	r0, [pc, #140]	; f118 <out_message+0x104>
    f088:	e5d31000 	ldrb	r1, [r3]
    f08c:	e1a0e00f 	mov	lr, pc
    f090:	e594f000 	ldr	pc, [r4]
    f094:	e59f3080 	ldr	r3, [pc, #128]	; f11c <out_message+0x108>
    f098:	e59f0080 	ldr	r0, [pc, #128]	; f120 <out_message+0x10c>
    f09c:	e5d31000 	ldrb	r1, [r3]
    f0a0:	e1a0e00f 	mov	lr, pc
    f0a4:	e594f000 	ldr	pc, [r4]
    f0a8:	e59f3074 	ldr	r3, [pc, #116]	; f124 <out_message+0x110>
    f0ac:	e59f0074 	ldr	r0, [pc, #116]	; f128 <out_message+0x114>
    f0b0:	e5d31000 	ldrb	r1, [r3]
    f0b4:	e1a0e00f 	mov	lr, pc
    f0b8:	e594f000 	ldr	pc, [r4]
    f0bc:	e59f3068 	ldr	r3, [pc, #104]	; f12c <out_message+0x118>
    f0c0:	e59f0068 	ldr	r0, [pc, #104]	; f130 <out_message+0x11c>
    f0c4:	e5d31000 	ldrb	r1, [r3]
    f0c8:	e1a0e00f 	mov	lr, pc
    f0cc:	e594f000 	ldr	pc, [r4]
    f0d0:	e59f305c 	ldr	r3, [pc, #92]	; f134 <out_message+0x120>
    f0d4:	e59f005c 	ldr	r0, [pc, #92]	; f138 <out_message+0x124>
    f0d8:	e5d31000 	ldrb	r1, [r3]
    f0dc:	e1a0e00f 	mov	lr, pc
    f0e0:	e594f000 	ldr	pc, [r4]
    f0e4:	e8bd8010 	pop	{r4, pc}
    f0e8:	0800d320 	.word	0x0800d320
    f0ec:	0800d76c 	.word	0x0800d76c
    f0f0:	00014390 	.word	0x00014390
    f0f4:	0800d321 	.word	0x0800d321
    f0f8:	0001439e 	.word	0x0001439e
    f0fc:	0800d324 	.word	0x0800d324
    f100:	000143b1 	.word	0x000143b1
    f104:	0800d328 	.word	0x0800d328
    f108:	000143c3 	.word	0x000143c3
    f10c:	0800d329 	.word	0x0800d329
    f110:	000143d6 	.word	0x000143d6
    f114:	0800d32a 	.word	0x0800d32a
    f118:	000143ed 	.word	0x000143ed
    f11c:	0800d1a0 	.word	0x0800d1a0
    f120:	00014405 	.word	0x00014405
    f124:	0800d32b 	.word	0x0800d32b
    f128:	00014414 	.word	0x00014414
    f12c:	0800d32c 	.word	0x0800d32c
    f130:	0001442d 	.word	0x0001442d
    f134:	0800d32d 	.word	0x0800d32d
    f138:	0001443f 	.word	0x0001443f

0000f13c <Procpacket>:
    f13c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    f140:	e59fc1ac 	ldr	ip, [pc, #428]	; f2f4 <Procpacket+0x1b8>
    f144:	e59f21ac 	ldr	r2, [pc, #428]	; f2f8 <Procpacket+0x1bc>
    f148:	e5dc3000 	ldrb	r3, [ip]
    f14c:	e1a06000 	mov	r6, r0
    f150:	e5c03000 	strb	r3, [r0]
    f154:	e5923000 	ldr	r3, [r2]
    f158:	e1a05001 	mov	r5, r1
    f15c:	e5c03001 	strb	r3, [r0, #1]
    f160:	e5923000 	ldr	r3, [r2]
    f164:	e1e03003 	mvn	r3, r3
    f168:	e5c03002 	strb	r3, [r0, #2]
    f16c:	e59f3188 	ldr	r3, [pc, #392]	; f2fc <Procpacket+0x1c0>
    f170:	e5922000 	ldr	r2, [r2]
    f174:	e5933000 	ldr	r3, [r3]
    f178:	e1520003 	cmp	r2, r3
    f17c:	1a000008 	bne	f1a4 <Procpacket+0x68>
    f180:	e59f3178 	ldr	r3, [pc, #376]	; f300 <Procpacket+0x1c4>
    f184:	e5d32000 	ldrb	r2, [r3]
    f188:	e59f3174 	ldr	r3, [pc, #372]	; f304 <Procpacket+0x1c8>
    f18c:	e3520001 	cmp	r2, #1	; 0x1
    f190:	e5934000 	ldr	r4, [r3]
    f194:	e5cc2000 	strb	r2, [ip]
    f198:	03a07080 	moveq	r7, #128	; 0x80
    f19c:	13a07b01 	movne	r7, #1024	; 0x400
    f1a0:	ea000004 	b	f1b8 <Procpacket+0x7c>
    f1a4:	e5dc3000 	ldrb	r3, [ip]
    f1a8:	e3530001 	cmp	r3, #1	; 0x1
    f1ac:	13a04b01 	movne	r4, #1024	; 0x400
    f1b0:	03a04080 	moveq	r4, #128	; 0x80
    f1b4:	e1a07004 	mov	r7, r4
    f1b8:	e59f3148 	ldr	r3, [pc, #328]	; f308 <Procpacket+0x1cc>
    f1bc:	e5d33000 	ldrb	r3, [r3]
    f1c0:	e3530000 	cmp	r3, #0	; 0x0
    f1c4:	1a000008 	bne	f1ec <Procpacket+0xb0>
    f1c8:	e59f313c 	ldr	r3, [pc, #316]	; f30c <Procpacket+0x1d0>
    f1cc:	e2860003 	add	r0, r6, #3	; 0x3
    f1d0:	e5931000 	ldr	r1, [r3]
    f1d4:	e59f3134 	ldr	r3, [pc, #308]	; f310 <Procpacket+0x1d4>
    f1d8:	e1a02004 	mov	r2, r4
    f1dc:	e5933000 	ldr	r3, [r3]
    f1e0:	e0631001 	rsb	r1, r3, r1
    f1e4:	e0851001 	add	r1, r5, r1
    f1e8:	ebffc855 	bl	1344 <memcpy>
    f1ec:	e59f111c 	ldr	r1, [pc, #284]	; f310 <Procpacket+0x1d4>
    f1f0:	e59f311c 	ldr	r3, [pc, #284]	; f314 <Procpacket+0x1d8>
    f1f4:	e5912000 	ldr	r2, [r1]
    f1f8:	e5d33000 	ldrb	r3, [r3]
    f1fc:	e0642002 	rsb	r2, r4, r2
    f200:	e3530000 	cmp	r3, #0	; 0x0
    f204:	e5812000 	str	r2, [r1]
    f208:	0a000006 	beq	f228 <Procpacket+0xec>
    f20c:	e1a02007 	mov	r2, r7
    f210:	e2860003 	add	r0, r6, #3	; 0x3
    f214:	e3a01000 	mov	r1, #0	; 0x0
    f218:	ebffc841 	bl	1324 <memset>
    f21c:	e59f30f4 	ldr	r3, [pc, #244]	; f318 <Procpacket+0x1dc>
    f220:	e3a02001 	mov	r2, #1	; 0x1
    f224:	e5c32000 	strb	r2, [r3]
    f228:	e59f30c8 	ldr	r3, [pc, #200]	; f2f8 <Procpacket+0x1bc>
    f22c:	e5932000 	ldr	r2, [r3]
    f230:	e59f30c4 	ldr	r3, [pc, #196]	; f2fc <Procpacket+0x1c0>
    f234:	e5933000 	ldr	r3, [r3]
    f238:	e1520003 	cmp	r2, r3
    f23c:	1a000004 	bne	f254 <Procpacket+0x118>
    f240:	e2840003 	add	r0, r4, #3	; 0x3
    f244:	e0860000 	add	r0, r6, r0
    f248:	e0642007 	rsb	r2, r4, r7
    f24c:	e3a0101a 	mov	r1, #26	; 0x1a
    f250:	ebffc833 	bl	1324 <memset>
    f254:	e3a00000 	mov	r0, #0	; 0x0
    f258:	e59f80bc 	ldr	r8, [pc, #188]	; f31c <Procpacket+0x1e0>
    f25c:	e1a04006 	mov	r4, r6
    f260:	e1a0c000 	mov	ip, r0
    f264:	e1a05000 	mov	r5, r0
    f268:	e1a0342c 	lsr	r3, ip, #8
    f26c:	e5d42003 	ldrb	r2, [r4, #3]
    f270:	e1a03083 	lsl	r3, r3, #1
    f274:	e19810b3 	ldrh	r1, [r8, r3]
    f278:	e2800001 	add	r0, r0, #1	; 0x1
    f27c:	e182340c 	orr	r3, r2, ip, lsl #8
    f280:	e0233001 	eor	r3, r3, r1
    f284:	e0852002 	add	r2, r5, r2
    f288:	e1500007 	cmp	r0, r7
    f28c:	e6ffc073 	uxth	ip, r3
    f290:	e6ef5072 	uxtb	r5, r2
    f294:	e2844001 	add	r4, r4, #1	; 0x1
    f298:	e59f107c 	ldr	r1, [pc, #124]	; f31c <Procpacket+0x1e0>
    f29c:	3afffff1 	bcc	f268 <Procpacket+0x12c>
    f2a0:	e59f3078 	ldr	r3, [pc, #120]	; f320 <Procpacket+0x1e4>
    f2a4:	e0860007 	add	r0, r6, r7
    f2a8:	e5d33000 	ldrb	r3, [r3]
    f2ac:	e3530000 	cmp	r3, #0	; 0x0
    f2b0:	05c05003 	strbeq	r5, [r0, #3]
    f2b4:	0a00000c 	beq	f2ec <Procpacket+0x1b0>
    f2b8:	e1a0342c 	lsr	r3, ip, #8
    f2bc:	e1a03083 	lsl	r3, r3, #1
    f2c0:	e19120b3 	ldrh	r2, [r1, r3]
    f2c4:	e022240c 	eor	r2, r2, ip, lsl #8
    f2c8:	e1a03442 	asr	r3, r2, #8
    f2cc:	e20330ff 	and	r3, r3, #255	; 0xff
    f2d0:	e1a03083 	lsl	r3, r3, #1
    f2d4:	e19130b3 	ldrh	r3, [r1, r3]
    f2d8:	e0233402 	eor	r3, r3, r2, lsl #8
    f2dc:	e6ff3073 	uxth	r3, r3
    f2e0:	e1a02423 	lsr	r2, r3, #8
    f2e4:	e5c03004 	strb	r3, [r0, #4]
    f2e8:	e5c02003 	strb	r2, [r0, #3]
    f2ec:	e3a00000 	mov	r0, #0	; 0x0
    f2f0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    f2f4:	0800d322 	.word	0x0800d322
    f2f8:	0800d324 	.word	0x0800d324
    f2fc:	0800dcfc 	.word	0x0800dcfc
    f300:	0800dd04 	.word	0x0800dd04
    f304:	0800dd00 	.word	0x0800dd00
    f308:	0800d328 	.word	0x0800d328
    f30c:	0800d7cc 	.word	0x0800d7cc
    f310:	0800d7d0 	.word	0x0800d7d0
    f314:	0800d32d 	.word	0x0800d32d
    f318:	0800d32c 	.word	0x0800d32c
    f31c:	000115e4 	.word	0x000115e4
    f320:	0800d7c8 	.word	0x0800d7c8

0000f324 <procsend>:
    f324:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    f328:	e59f31f8 	ldr	r3, [pc, #504]	; f528 <procsend+0x204>
    f32c:	e1a06000 	mov	r6, r0
    f330:	e5d30000 	ldrb	r0, [r3]
    f334:	e24dd004 	sub	sp, sp, #4	; 0x4
    f338:	e3500000 	cmp	r0, #0	; 0x0
    f33c:	e1a0c002 	mov	ip, r2
    f340:	e59f41e4 	ldr	r4, [pc, #484]	; f52c <procsend+0x208>
    f344:	0a000065 	beq	f4e0 <procsend+0x1bc>
    f348:	e59f31e0 	ldr	r3, [pc, #480]	; f530 <procsend+0x20c>
    f34c:	e1a02b02 	lsl	r2, r2, #22
    f350:	e1a02b22 	lsr	r2, r2, #22
    f354:	e583c000 	str	ip, [r3]
    f358:	e59f31d4 	ldr	r3, [pc, #468]	; f534 <procsend+0x210>
    f35c:	e59f01d4 	ldr	r0, [pc, #468]	; f538 <procsend+0x214>
    f360:	e3520000 	cmp	r2, #0	; 0x0
    f364:	e1a0152c 	lsr	r1, ip, #10
    f368:	e583c000 	str	ip, [r3]
    f36c:	03a03b01 	moveq	r3, #1024	; 0x400
    f370:	e5802000 	str	r2, [r0]
    f374:	e5841000 	str	r1, [r4]
    f378:	05803000 	streq	r3, [r0]
    f37c:	0a000006 	beq	f39c <procsend+0x78>
    f380:	e2813001 	add	r3, r1, #1	; 0x1
    f384:	e352007f 	cmp	r2, #127	; 0x7f
    f388:	e59f21ac 	ldr	r2, [pc, #428]	; f53c <procsend+0x218>
    f38c:	e5843000 	str	r3, [r4]
    f390:	93a03001 	movls	r3, #1	; 0x1
    f394:	83a03002 	movhi	r3, #2	; 0x2
    f398:	e5c23000 	strb	r3, [r2]
    f39c:	e59f019c 	ldr	r0, [pc, #412]	; f540 <procsend+0x21c>
    f3a0:	ebffc7cd 	bl	12dc <strlen>
    f3a4:	e59f2198 	ldr	r2, [pc, #408]	; f544 <procsend+0x220>
    f3a8:	e3500080 	cmp	r0, #128	; 0x80
    f3ac:	83a03002 	movhi	r3, #2	; 0x2
    f3b0:	93a03001 	movls	r3, #1	; 0x1
    f3b4:	85c23000 	strbhi	r3, [r2]
    f3b8:	95c23000 	strbls	r3, [r2]
    f3bc:	e59f3184 	ldr	r3, [pc, #388]	; f548 <procsend+0x224>
    f3c0:	83a07b01 	movhi	r7, #1024	; 0x400
    f3c4:	e5d31000 	ldrb	r1, [r3]
    f3c8:	93a07080 	movls	r7, #128	; 0x80
    f3cc:	e3510000 	cmp	r1, #0	; 0x0
    f3d0:	13a01000 	movne	r1, #0	; 0x0
    f3d4:	12872005 	addne	r2, r7, #5	; 0x5
    f3d8:	02872004 	addeq	r2, r7, #4	; 0x4
    f3dc:	11a00006 	movne	r0, r6
    f3e0:	01a00006 	moveq	r0, r6
    f3e4:	ebffc7ce 	bl	1324 <memset>
    f3e8:	e59f3154 	ldr	r3, [pc, #340]	; f544 <procsend+0x220>
    f3ec:	e3a05000 	mov	r5, #0	; 0x0
    f3f0:	e5d33000 	ldrb	r3, [r3]
    f3f4:	e59f0144 	ldr	r0, [pc, #324]	; f540 <procsend+0x21c>
    f3f8:	e5c63000 	strb	r3, [r6]
    f3fc:	e3e03000 	mvn	r3, #0	; 0x0
    f400:	e5c63002 	strb	r3, [r6, #2]
    f404:	e5c65001 	strb	r5, [r6, #1]
    f408:	ebffc7b3 	bl	12dc <strlen>
    f40c:	e59f112c 	ldr	r1, [pc, #300]	; f540 <procsend+0x21c>
    f410:	e1a02000 	mov	r2, r0
    f414:	e1a04000 	mov	r4, r0
    f418:	e2860003 	add	r0, r6, #3	; 0x3
    f41c:	ebffc75e 	bl	119c <strncpy>
    f420:	e59f3108 	ldr	r3, [pc, #264]	; f530 <procsend+0x20c>
    f424:	e2844004 	add	r4, r4, #4	; 0x4
    f428:	e0860004 	add	r0, r6, r4
    f42c:	e5932000 	ldr	r2, [r3]
    f430:	e59f1114 	ldr	r1, [pc, #276]	; f54c <procsend+0x228>
    f434:	e59f4114 	ldr	r4, [pc, #276]	; f550 <procsend+0x22c>
    f438:	ebfffab8 	bl	df20 <sprintf>
    f43c:	e1a00006 	mov	r0, r6
    f440:	e1a0c005 	mov	ip, r5
    f444:	e1a0e005 	mov	lr, r5
    f448:	e1a0342c 	lsr	r3, ip, #8
    f44c:	e5d02003 	ldrb	r2, [r0, #3]
    f450:	e1a03083 	lsl	r3, r3, #1
    f454:	e19410b3 	ldrh	r1, [r4, r3]
    f458:	e2855001 	add	r5, r5, #1	; 0x1
    f45c:	e182340c 	orr	r3, r2, ip, lsl #8
    f460:	e0233001 	eor	r3, r3, r1
    f464:	e08e2002 	add	r2, lr, r2
    f468:	e1550007 	cmp	r5, r7
    f46c:	e6ffc073 	uxth	ip, r3
    f470:	e6efe072 	uxtb	lr, r2
    f474:	e2800001 	add	r0, r0, #1	; 0x1
    f478:	e59f10d0 	ldr	r1, [pc, #208]	; f550 <procsend+0x22c>
    f47c:	3afffff1 	bcc	f448 <procsend+0x124>
    f480:	e59f30c0 	ldr	r3, [pc, #192]	; f548 <procsend+0x224>
    f484:	e0860007 	add	r0, r6, r7
    f488:	e5d33000 	ldrb	r3, [r3]
    f48c:	e3530000 	cmp	r3, #0	; 0x0
    f490:	05c0e003 	strbeq	lr, [r0, #3]
    f494:	0a00000c 	beq	f4cc <procsend+0x1a8>
    f498:	e1a0342c 	lsr	r3, ip, #8
    f49c:	e1a03083 	lsl	r3, r3, #1
    f4a0:	e19120b3 	ldrh	r2, [r1, r3]
    f4a4:	e022240c 	eor	r2, r2, ip, lsl #8
    f4a8:	e1a03442 	asr	r3, r2, #8
    f4ac:	e20330ff 	and	r3, r3, #255	; 0xff
    f4b0:	e1a03083 	lsl	r3, r3, #1
    f4b4:	e19130b3 	ldrh	r3, [r1, r3]
    f4b8:	e0233402 	eor	r3, r3, r2, lsl #8
    f4bc:	e6ff3073 	uxth	r3, r3
    f4c0:	e1a02423 	lsr	r2, r3, #8
    f4c4:	e5c03004 	strb	r3, [r0, #4]
    f4c8:	e5c02003 	strb	r2, [r0, #3]
    f4cc:	e59f3080 	ldr	r3, [pc, #128]	; f554 <procsend+0x230>
    f4d0:	e3a02001 	mov	r2, #1	; 0x1
    f4d4:	e5c32000 	strb	r2, [r3]
    f4d8:	e3a00000 	mov	r0, #0	; 0x0
    f4dc:	ea00000f 	b	f520 <procsend+0x1fc>
    f4e0:	e59f2070 	ldr	r2, [pc, #112]	; f558 <procsend+0x234>
    f4e4:	e5943000 	ldr	r3, [r4]
    f4e8:	e5922000 	ldr	r2, [r2]
    f4ec:	e1520003 	cmp	r2, r3
    f4f0:	8a00000a 	bhi	f520 <procsend+0x1fc>
    f4f4:	e59f3060 	ldr	r3, [pc, #96]	; f55c <procsend+0x238>
    f4f8:	e59f2044 	ldr	r2, [pc, #68]	; f544 <procsend+0x220>
    f4fc:	e5d33000 	ldrb	r3, [r3]
    f500:	e1a00006 	mov	r0, r6
    f504:	e3530000 	cmp	r3, #0	; 0x0
    f508:	13a03001 	movne	r3, #1	; 0x1
    f50c:	03a03002 	moveq	r3, #2	; 0x2
    f510:	e5c23000 	strb	r3, [r2]
    f514:	e1a0200c 	mov	r2, ip
    f518:	ebffff07 	bl	f13c <Procpacket>
    f51c:	e6ef0070 	uxtb	r0, r0
    f520:	e28dd004 	add	sp, sp, #4	; 0x4
    f524:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    f528:	0800d1a0 	.word	0x0800d1a0
    f52c:	0800dcfc 	.word	0x0800dcfc
    f530:	0800d7d0 	.word	0x0800d7d0
    f534:	0800d7cc 	.word	0x0800d7cc
    f538:	0800dd00 	.word	0x0800dd00
    f53c:	0800dd04 	.word	0x0800dd04
    f540:	0001444e 	.word	0x0001444e
    f544:	0800d322 	.word	0x0800d322
    f548:	0800d7c8 	.word	0x0800d7c8
    f54c:	00013223 	.word	0x00013223
    f550:	000115e4 	.word	0x000115e4
    f554:	0800d321 	.word	0x0800d321
    f558:	0800d324 	.word	0x0800d324
    f55c:	0800d32d 	.word	0x0800d32d

0000f560 <CYGACC_COMM_IF_PUTC>:
    f560:	e1a00001 	mov	r0, r1
    f564:	eafffaf7 	b	e148 <putc>

0000f568 <real_send>:
    f568:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    f56c:	e59f30ec 	ldr	r3, [pc, #236]	; f660 <real_send+0xf8>
    f570:	e24ddb01 	sub	sp, sp, #1024	; 0x400
    f574:	e5d33000 	ldrb	r3, [r3]
    f578:	e24dd00c 	sub	sp, sp, #12	; 0xc
    f57c:	e3530000 	cmp	r3, #0	; 0x0
    f580:	e1a0e000 	mov	lr, r0
    f584:	e1a0c001 	mov	ip, r1
    f588:	159f30d4 	ldrne	r3, [pc, #212]	; f664 <real_send+0xfc>
    f58c:	13a02000 	movne	r2, #0	; 0x0
    f590:	15832000 	strne	r2, [r3]
    f594:	1a000006 	bne	f5b4 <real_send+0x4c>
    f598:	e59f30c8 	ldr	r3, [pc, #200]	; f668 <real_send+0x100>
    f59c:	e5d33000 	ldrb	r3, [r3]
    f5a0:	e3530000 	cmp	r3, #0	; 0x0
    f5a4:	059f20b8 	ldreq	r2, [pc, #184]	; f664 <real_send+0xfc>
    f5a8:	05923000 	ldreq	r3, [r2]
    f5ac:	02833001 	addeq	r3, r3, #1	; 0x1
    f5b0:	05823000 	streq	r3, [r2]
    f5b4:	e28d0008 	add	r0, sp, #8	; 0x8
    f5b8:	e2400005 	sub	r0, r0, #5	; 0x5
    f5bc:	e1a0100e 	mov	r1, lr
    f5c0:	e1a0200c 	mov	r2, ip
    f5c4:	ebffff56 	bl	f324 <procsend>
    f5c8:	e2507000 	subs	r7, r0, #0	; 0x0
    f5cc:	1a00001f 	bne	f650 <real_send+0xe8>
    f5d0:	e59f3094 	ldr	r3, [pc, #148]	; f66c <real_send+0x104>
    f5d4:	e28d5008 	add	r5, sp, #8	; 0x8
    f5d8:	e5d31000 	ldrb	r1, [r3]
    f5dc:	e59f308c 	ldr	r3, [pc, #140]	; f670 <real_send+0x108>
    f5e0:	e3510001 	cmp	r1, #1	; 0x1
    f5e4:	e5d32000 	ldrb	r2, [r3]
    f5e8:	e3003404 	movw	r3, #1028	; 0x404
    f5ec:	11a06003 	movne	r6, r3
    f5f0:	03a06084 	moveq	r6, #132	; 0x84
    f5f4:	e3520000 	cmp	r2, #0	; 0x0
    f5f8:	12866001 	addne	r6, r6, #1	; 0x1
    f5fc:	e2455005 	sub	r5, r5, #5	; 0x5
    f600:	e3a04000 	mov	r4, #0	; 0x0
    f604:	e7d50004 	ldrb	r0, [r5, r4]
    f608:	e2844001 	add	r4, r4, #1	; 0x1
    f60c:	ebfffacd 	bl	e148 <putc>
    f610:	e1540006 	cmp	r4, r6
    f614:	3afffffa 	bcc	f604 <real_send+0x9c>
    f618:	e59f3054 	ldr	r3, [pc, #84]	; f674 <real_send+0x10c>
    f61c:	e5d33000 	ldrb	r3, [r3]
    f620:	e3530000 	cmp	r3, #0	; 0x0
    f624:	13e07000 	mvnne	r7, #0	; 0x0
    f628:	1a000008 	bne	f650 <real_send+0xe8>
    f62c:	e59f3030 	ldr	r3, [pc, #48]	; f664 <real_send+0xfc>
    f630:	e5932000 	ldr	r2, [r3]
    f634:	e59f303c 	ldr	r3, [pc, #60]	; f678 <real_send+0x110>
    f638:	e5933000 	ldr	r3, [r3]
    f63c:	e1520003 	cmp	r2, r3
    f640:	059f3034 	ldreq	r3, [pc, #52]	; f67c <real_send+0x114>
    f644:	03a02001 	moveq	r2, #1	; 0x1
    f648:	05c32000 	strbeq	r2, [r3]
    f64c:	03a07003 	moveq	r7, #3	; 0x3
    f650:	e1a00007 	mov	r0, r7
    f654:	e28dd00c 	add	sp, sp, #12	; 0xc
    f658:	e28ddb01 	add	sp, sp, #1024	; 0x400
    f65c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    f660:	0800d32d 	.word	0x0800d32d
    f664:	0800d324 	.word	0x0800d324
    f668:	0800d1a0 	.word	0x0800d1a0
    f66c:	0800d322 	.word	0x0800d322
    f670:	0800d7c8 	.word	0x0800d7c8
    f674:	0800d32c 	.word	0x0800d32c
    f678:	0800dcfc 	.word	0x0800dcfc
    f67c:	0800d328 	.word	0x0800d328

0000f680 <CYGACC_COMM_IF_GETC_TIMEOUT>:
    f680:	e92d4070 	push	{r4, r5, r6, lr}
    f684:	e1a05001 	mov	r5, r1
    f688:	e3a04000 	mov	r4, #0	; 0x0
    f68c:	e59f603c 	ldr	r6, [pc, #60]	; f6d0 <CYGACC_COMM_IF_GETC_TIMEOUT+0x50>
    f690:	ea000000 	b	f698 <CYGACC_COMM_IF_GETC_TIMEOUT+0x18>
    f694:	ebffe46d 	bl	8850 <udelay>
    f698:	ebfffaab 	bl	e14c <tstc>
    f69c:	e3500000 	cmp	r0, #0	; 0x0
    f6a0:	e3a00014 	mov	r0, #20	; 0x14
    f6a4:	1a000002 	bne	f6b4 <CYGACC_COMM_IF_GETC_TIMEOUT+0x34>
    f6a8:	e1540006 	cmp	r4, r6
    f6ac:	e2844001 	add	r4, r4, #1	; 0x1
    f6b0:	1afffff7 	bne	f694 <CYGACC_COMM_IF_GETC_TIMEOUT+0x14>
    f6b4:	ebfffaa4 	bl	e14c <tstc>
    f6b8:	e3500000 	cmp	r0, #0	; 0x0
    f6bc:	08bd8070 	popeq	{r4, r5, r6, pc}
    f6c0:	ebfffaa2 	bl	e150 <getc>
    f6c4:	e5c50000 	strb	r0, [r5]
    f6c8:	e3a00001 	mov	r0, #1	; 0x1
    f6cc:	e8bd8070 	pop	{r4, r5, r6, pc}
    f6d0:	000186a0 	.word	0x000186a0

0000f6d4 <control_send>:
    f6d4:	e59f323c 	ldr	r3, [pc, #572]	; f918 <control_send+0x244>
    f6d8:	e3a02000 	mov	r2, #0	; 0x0
    f6dc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    f6e0:	e5c32000 	strb	r2, [r3]
    f6e4:	e59f3230 	ldr	r3, [pc, #560]	; f91c <control_send+0x248>
    f6e8:	e59f4230 	ldr	r4, [pc, #560]	; f920 <control_send+0x24c>
    f6ec:	e5c32000 	strb	r2, [r3]
    f6f0:	e59f322c 	ldr	r3, [pc, #556]	; f924 <control_send+0x250>
    f6f4:	e59f522c 	ldr	r5, [pc, #556]	; f928 <control_send+0x254>
    f6f8:	e5832000 	str	r2, [r3]
    f6fc:	e3a03001 	mov	r3, #1	; 0x1
    f700:	e59fe224 	ldr	lr, [pc, #548]	; f92c <control_send+0x258>
    f704:	e59f8224 	ldr	r8, [pc, #548]	; f930 <control_send+0x25c>
    f708:	e59f6224 	ldr	r6, [pc, #548]	; f934 <control_send+0x260>
    f70c:	e59f7224 	ldr	r7, [pc, #548]	; f938 <control_send+0x264>
    f710:	e59fc224 	ldr	ip, [pc, #548]	; f93c <control_send+0x268>
    f714:	e5c43000 	strb	r3, [r4]
    f718:	e59f3220 	ldr	r3, [pc, #544]	; f940 <control_send+0x26c>
    f71c:	e24dd014 	sub	sp, sp, #20	; 0x14
    f720:	e5c52000 	strb	r2, [r5]
    f724:	e5c32000 	strb	r2, [r3]
    f728:	e58d0004 	str	r0, [sp, #4]
    f72c:	e1a0b001 	mov	fp, r1
    f730:	e5c82000 	strb	r2, [r8]
    f734:	e5c62000 	strb	r2, [r6]
    f738:	e5c72000 	strb	r2, [r7]
    f73c:	e5ce2000 	strb	r2, [lr]
    f740:	e5cc2000 	strb	r2, [ip]
    f744:	e1a0a002 	mov	sl, r2
    f748:	e1a0900e 	mov	r9, lr
    f74c:	e59f51f0 	ldr	r5, [pc, #496]	; f944 <control_send+0x270>
    f750:	ea000000 	b	f758 <control_send+0x84>
    f754:	e3a0a000 	mov	sl, #0	; 0x0
    f758:	e3a00000 	mov	r0, #0	; 0x0
    f75c:	e28d100f 	add	r1, sp, #15	; 0xf
    f760:	ebffffc6 	bl	f680 <CYGACC_COMM_IF_GETC_TIMEOUT>
    f764:	e3500000 	cmp	r0, #0	; 0x0
    f768:	0afffffa 	beq	f758 <control_send+0x84>
    f76c:	e5dd300f 	ldrb	r3, [sp, #15]
    f770:	e3530018 	cmp	r3, #24	; 0x18
    f774:	0a000042 	beq	f884 <control_send+0x1b0>
    f778:	8a000004 	bhi	f790 <control_send+0xbc>
    f77c:	e3530006 	cmp	r3, #6	; 0x6
    f780:	0a00002c 	beq	f838 <control_send+0x164>
    f784:	e3530015 	cmp	r3, #21	; 0x15
    f788:	1afffff1 	bne	f754 <control_send+0x80>
    f78c:	ea000010 	b	f7d4 <control_send+0x100>
    f790:	e3530043 	cmp	r3, #67	; 0x43
    f794:	0a000001 	beq	f7a0 <control_send+0xcc>
    f798:	e3530063 	cmp	r3, #99	; 0x63
    f79c:	1affffec 	bne	f754 <control_send+0x80>
    f7a0:	e5d73000 	ldrb	r3, [r7]
    f7a4:	e5d42000 	ldrb	r2, [r4]
    f7a8:	e5d61000 	ldrb	r1, [r6]
    f7ac:	e1833002 	orr	r3, r3, r2
    f7b0:	e1913003 	orrs	r3, r1, r3
    f7b4:	0affffe6 	beq	f754 <control_send+0x80>
    f7b8:	e3510000 	cmp	r1, #0	; 0x0
    f7bc:	159f2178 	ldrne	r2, [pc, #376]	; f93c <control_send+0x268>
    f7c0:	13a03001 	movne	r3, #1	; 0x1
    f7c4:	15c23000 	strbne	r3, [r2]
    f7c8:	e3a03001 	mov	r3, #1	; 0x1
    f7cc:	e5c53000 	strb	r3, [r5]
    f7d0:	ea00002f 	b	f894 <control_send+0x1c0>
    f7d4:	e59f313c 	ldr	r3, [pc, #316]	; f918 <control_send+0x244>
    f7d8:	e5d33000 	ldrb	r3, [r3]
    f7dc:	e3530000 	cmp	r3, #0	; 0x0
    f7e0:	0a000008 	beq	f808 <control_send+0x134>
    f7e4:	e59f2138 	ldr	r2, [pc, #312]	; f924 <control_send+0x250>
    f7e8:	e5923000 	ldr	r3, [r2]
    f7ec:	e2433001 	sub	r3, r3, #1	; 0x1
    f7f0:	e5823000 	str	r3, [r2]
    f7f4:	e3a03001 	mov	r3, #1	; 0x1
    f7f8:	e5c43000 	strb	r3, [r4]
    f7fc:	e3a03000 	mov	r3, #0	; 0x0
    f800:	e5c93000 	strb	r3, [r9]
    f804:	ea000022 	b	f894 <control_send+0x1c0>
    f808:	e5d42000 	ldrb	r2, [r4]
    f80c:	e3520000 	cmp	r2, #0	; 0x0
    f810:	15c53000 	strbne	r3, [r5]
    f814:	1a000007 	bne	f838 <control_send+0x164>
    f818:	e5d93000 	ldrb	r3, [r9]
    f81c:	e3530000 	cmp	r3, #0	; 0x0
    f820:	15c92000 	strbne	r2, [r9]
    f824:	e59f20f8 	ldr	r2, [pc, #248]	; f924 <control_send+0x250>
    f828:	e5923000 	ldr	r3, [r2]
    f82c:	e2433001 	sub	r3, r3, #1	; 0x1
    f830:	e5823000 	str	r3, [r2]
    f834:	ea000016 	b	f894 <control_send+0x1c0>
    f838:	e5d42000 	ldrb	r2, [r4]
    f83c:	e3520000 	cmp	r2, #0	; 0x0
    f840:	1affffc3 	bne	f754 <control_send+0x80>
    f844:	e5d91000 	ldrb	r1, [r9]
    f848:	e3510000 	cmp	r1, #0	; 0x0
    f84c:	0a000005 	beq	f868 <control_send+0x194>
    f850:	e59f30c0 	ldr	r3, [pc, #192]	; f918 <control_send+0x244>
    f854:	e1a0a002 	mov	sl, r2
    f858:	e5c32000 	strb	r2, [r3]
    f85c:	e3a03001 	mov	r3, #1	; 0x1
    f860:	e5c73000 	strb	r3, [r7]
    f864:	eaffffbb 	b	f758 <control_send+0x84>
    f868:	e5d83000 	ldrb	r3, [r8]
    f86c:	e3530000 	cmp	r3, #0	; 0x0
    f870:	13a03001 	movne	r3, #1	; 0x1
    f874:	15c63000 	strbne	r3, [r6]
    f878:	11a0a001 	movne	sl, r1
    f87c:	1affffb5 	bne	f758 <control_send+0x84>
    f880:	ea000003 	b	f894 <control_send+0x1c0>
    f884:	e28aa001 	add	sl, sl, #1	; 0x1
    f888:	e35a0002 	cmp	sl, #2	; 0x2
    f88c:	1affffb1 	bne	f758 <control_send+0x84>
    f890:	ea00001d 	b	f90c <control_send+0x238>
    f894:	e59f20a0 	ldr	r2, [pc, #160]	; f93c <control_send+0x268>
    f898:	e5d23000 	ldrb	r3, [r2]
    f89c:	e3530000 	cmp	r3, #0	; 0x0
    f8a0:	1a000008 	bne	f8c8 <control_send+0x1f4>
    f8a4:	e59f207c 	ldr	r2, [pc, #124]	; f928 <control_send+0x254>
    f8a8:	e5d23000 	ldrb	r3, [r2]
    f8ac:	e3530000 	cmp	r3, #0	; 0x0
    f8b0:	0a000004 	beq	f8c8 <control_send+0x1f4>
    f8b4:	e3a00004 	mov	r0, #4	; 0x4
    f8b8:	ebfffa22 	bl	e148 <putc>
    f8bc:	e3a03001 	mov	r3, #1	; 0x1
    f8c0:	e5c83000 	strb	r3, [r8]
    f8c4:	eaffffa2 	b	f754 <control_send+0x80>
    f8c8:	e59d0004 	ldr	r0, [sp, #4]
    f8cc:	e1a0100b 	mov	r1, fp
    f8d0:	ebffff24 	bl	f568 <real_send>
    f8d4:	e3500003 	cmp	r0, #3	; 0x3
    f8d8:	0affff9d 	beq	f754 <control_send+0x80>
    f8dc:	e3500000 	cmp	r0, #0	; 0x0
    f8e0:	1a00000a 	bne	f910 <control_send+0x23c>
    f8e4:	e5d93000 	ldrb	r3, [r9]
    f8e8:	e3530000 	cmp	r3, #0	; 0x0
    f8ec:	e5d43000 	ldrb	r3, [r4]
    f8f0:	15c90000 	strbne	r0, [r9]
    f8f4:	e3530000 	cmp	r3, #0	; 0x0
    f8f8:	13a03000 	movne	r3, #0	; 0x0
    f8fc:	15c43000 	strbne	r3, [r4]
    f900:	12833001 	addne	r3, r3, #1	; 0x1
    f904:	15c93000 	strbne	r3, [r9]
    f908:	eaffff91 	b	f754 <control_send+0x80>
    f90c:	e3a00000 	mov	r0, #0	; 0x0
    f910:	e28dd014 	add	sp, sp, #20	; 0x14
    f914:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    f918:	0800d321 	.word	0x0800d321
    f91c:	0800d322 	.word	0x0800d322
    f920:	0800d1a0 	.word	0x0800d1a0
    f924:	0800d324 	.word	0x0800d324
    f928:	0800d328 	.word	0x0800d328
    f92c:	0800d32b 	.word	0x0800d32b
    f930:	0800d320 	.word	0x0800d320
    f934:	0800d329 	.word	0x0800d329
    f938:	0800d32a 	.word	0x0800d32a
    f93c:	0800d32d 	.word	0x0800d32d
    f940:	0800d32c 	.word	0x0800d32c
    f944:	0800d7c8 	.word	0x0800d7c8

0000f948 <xyzModem_stream_terminate>:
    f948:	e92d4030 	push	{r4, r5, lr}
    f94c:	e3500000 	cmp	r0, #0	; 0x0
    f950:	e24dd00c 	sub	sp, sp, #12	; 0xc
    f954:	e1a04001 	mov	r4, r1
    f958:	0a000020 	beq	f9e0 <xyzModem_stream_terminate+0x98>
    f95c:	e59f4098 	ldr	r4, [pc, #152]	; f9fc <xyzModem_stream_terminate+0xb4>
    f960:	e5943414 	ldr	r3, [r4, #1044]
    f964:	e2433001 	sub	r3, r3, #1	; 0x1
    f968:	e3530001 	cmp	r3, #1	; 0x1
    f96c:	8a000020 	bhi	f9f4 <xyzModem_stream_terminate+0xac>
    f970:	e3a00018 	mov	r0, #24	; 0x18
    f974:	ebfff9f3 	bl	e148 <putc>
    f978:	e3a00018 	mov	r0, #24	; 0x18
    f97c:	ebfff9f1 	bl	e148 <putc>
    f980:	e3a00018 	mov	r0, #24	; 0x18
    f984:	ebfff9ef 	bl	e148 <putc>
    f988:	e3a00018 	mov	r0, #24	; 0x18
    f98c:	ebfff9ed 	bl	e148 <putc>
    f990:	e3a00008 	mov	r0, #8	; 0x8
    f994:	ebfff9eb 	bl	e148 <putc>
    f998:	e3a00008 	mov	r0, #8	; 0x8
    f99c:	ebfff9e9 	bl	e148 <putc>
    f9a0:	e3a00008 	mov	r0, #8	; 0x8
    f9a4:	ebfff9e7 	bl	e148 <putc>
    f9a8:	e3a00008 	mov	r0, #8	; 0x8
    f9ac:	ebfff9e5 	bl	e148 <putc>
    f9b0:	e1a05004 	mov	r5, r4
    f9b4:	e28d4007 	add	r4, sp, #7	; 0x7
    f9b8:	e5953000 	ldr	r3, [r5]
    f9bc:	e1a01004 	mov	r1, r4
    f9c0:	e5d30000 	ldrb	r0, [r3]
    f9c4:	ebffff2d 	bl	f680 <CYGACC_COMM_IF_GETC_TIMEOUT>
    f9c8:	e3500000 	cmp	r0, #0	; 0x0
    f9cc:	1afffff9 	bne	f9b8 <xyzModem_stream_terminate+0x70>
    f9d0:	e59f3024 	ldr	r3, [pc, #36]	; f9fc <xyzModem_stream_terminate+0xb4>
    f9d4:	e3a02001 	mov	r2, #1	; 0x1
    f9d8:	e583242c 	str	r2, [r3, #1068]
    f9dc:	ea000004 	b	f9f4 <xyzModem_stream_terminate+0xac>
    f9e0:	e12fff34 	blx	r4
    f9e4:	e3500000 	cmp	r0, #0	; 0x0
    f9e8:	aafffffc 	bge	f9e0 <xyzModem_stream_terminate+0x98>
    f9ec:	e59f000c 	ldr	r0, [pc, #12]	; fa00 <xyzModem_stream_terminate+0xb8>
    f9f0:	ebffe396 	bl	8850 <udelay>
    f9f4:	e28dd00c 	add	sp, sp, #12	; 0xc
    f9f8:	e8bd8030 	pop	{r4, r5, pc}
    f9fc:	0800d330 	.word	0x0800d330
    fa00:	0003d090 	.word	0x0003d090

0000fa04 <xyzModem_get_hdr>:
    fa04:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    fa08:	e59f42a0 	ldr	r4, [pc, #672]	; fcb0 <xyzModem_get_hdr+0x2ac>
    fa0c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    fa10:	e5943430 	ldr	r3, [r4, #1072]
    fa14:	e3530000 	cmp	r3, #0	; 0x0
    fa18:	0a000003 	beq	fa2c <xyzModem_get_hdr+0x28>
    fa1c:	e3a00006 	mov	r0, #6	; 0x6
    fa20:	ebfff9c8 	bl	e148 <putc>
    fa24:	e3a03000 	mov	r3, #0	; 0x0
    fa28:	e5843430 	str	r3, [r4, #1072]
    fa2c:	e3a04000 	mov	r4, #0	; 0x0
    fa30:	e59f6278 	ldr	r6, [pc, #632]	; fcb0 <xyzModem_get_hdr+0x2ac>
    fa34:	e1a05004 	mov	r5, r4
    fa38:	e28d7007 	add	r7, sp, #7	; 0x7
    fa3c:	e5963000 	ldr	r3, [r6]
    fa40:	e1a01007 	mov	r1, r7
    fa44:	e5d30000 	ldrb	r0, [r3]
    fa48:	ebffff0c 	bl	f680 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fa4c:	e3500000 	cmp	r0, #0	; 0x0
    fa50:	e2855001 	add	r5, r5, #1	; 0x1
    fa54:	e59f2254 	ldr	r2, [pc, #596]	; fcb0 <xyzModem_get_hdr+0x2ac>
    fa58:	0a000021 	beq	fae4 <xyzModem_get_hdr+0xe0>
    fa5c:	e5dd3007 	ldrb	r3, [sp, #7]
    fa60:	e3530002 	cmp	r3, #2	; 0x2
    fa64:	0a00000c 	beq	fa9c <xyzModem_get_hdr+0x98>
    fa68:	8a000002 	bhi	fa78 <xyzModem_get_hdr+0x74>
    fa6c:	e3530001 	cmp	r3, #1	; 0x1
    fa70:	1afffff1 	bne	fa3c <xyzModem_get_hdr+0x38>
    fa74:	ea000004 	b	fa8c <xyzModem_get_hdr+0x88>
    fa78:	e3530004 	cmp	r3, #4	; 0x4
    fa7c:	0a000012 	beq	facc <xyzModem_get_hdr+0xc8>
    fa80:	e3530018 	cmp	r3, #24	; 0x18
    fa84:	1affffec 	bne	fa3c <xyzModem_get_hdr+0x38>
    fa88:	ea000007 	b	faac <xyzModem_get_hdr+0xa8>
    fa8c:	e592341c 	ldr	r3, [r2, #1052]
    fa90:	e2833001 	add	r3, r3, #1	; 0x1
    fa94:	e582341c 	str	r3, [r2, #1052]
    fa98:	ea00007c 	b	fc90 <xyzModem_get_hdr+0x28c>
    fa9c:	e5923420 	ldr	r3, [r2, #1056]
    faa0:	e2833001 	add	r3, r3, #1	; 0x1
    faa4:	e5823420 	str	r3, [r2, #1056]
    faa8:	ea000078 	b	fc90 <xyzModem_get_hdr+0x28c>
    faac:	e5963424 	ldr	r3, [r6, #1060]
    fab0:	e2844001 	add	r4, r4, #1	; 0x1
    fab4:	e2833001 	add	r3, r3, #1	; 0x1
    fab8:	e3540003 	cmp	r4, #3	; 0x3
    fabc:	e5863424 	str	r3, [r6, #1060]
    fac0:	1affffdd 	bne	fa3c <xyzModem_get_hdr+0x38>
    fac4:	e3e00004 	mvn	r0, #4	; 0x4
    fac8:	ea00006e 	b	fc88 <xyzModem_get_hdr+0x284>
    facc:	e3550001 	cmp	r5, #1	; 0x1
    fad0:	1affffd9 	bne	fa3c <xyzModem_get_hdr+0x38>
    fad4:	e3a00006 	mov	r0, #6	; 0x6
    fad8:	ebfff99a 	bl	e148 <putc>
    fadc:	e3e00003 	mvn	r0, #3	; 0x3
    fae0:	ea000068 	b	fc88 <xyzModem_get_hdr+0x284>
    fae4:	e1a05002 	mov	r5, r2
    fae8:	e28d4006 	add	r4, sp, #6	; 0x6
    faec:	e5953000 	ldr	r3, [r5]
    faf0:	e1a01004 	mov	r1, r4
    faf4:	e5d30000 	ldrb	r0, [r3]
    faf8:	ebfffee0 	bl	f680 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fafc:	e3500000 	cmp	r0, #0	; 0x0
    fb00:	1afffff9 	bne	faec <xyzModem_get_hdr+0xe8>
    fb04:	e59f01a8 	ldr	r0, [pc, #424]	; fcb4 <xyzModem_get_hdr+0x2b0>
    fb08:	ebffe350 	bl	8850 <udelay>
    fb0c:	ea00005a 	b	fc7c <xyzModem_get_hdr+0x278>
    fb10:	e5943000 	ldr	r3, [r4]
    fb14:	e59f119c 	ldr	r1, [pc, #412]	; fcb8 <xyzModem_get_hdr+0x2b4>
    fb18:	e5d30000 	ldrb	r0, [r3]
    fb1c:	ebfffed7 	bl	f680 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fb20:	e3500000 	cmp	r0, #0	; 0x0
    fb24:	0a000054 	beq	fc7c <xyzModem_get_hdr+0x278>
    fb28:	e28d5008 	add	r5, sp, #8	; 0x8
    fb2c:	e5753001 	ldrb	r3, [r5, #-1]!
    fb30:	e2842004 	add	r2, r4, #4	; 0x4
    fb34:	e3530001 	cmp	r3, #1	; 0x1
    fb38:	03a03080 	moveq	r3, #128	; 0x80
    fb3c:	13a03b01 	movne	r3, #1024	; 0x400
    fb40:	e5843410 	str	r3, [r4, #1040]
    fb44:	e5842404 	str	r2, [r4, #1028]
    fb48:	e1a07004 	mov	r7, r4
    fb4c:	e3a04000 	mov	r4, #0	; 0x0
    fb50:	ea000008 	b	fb78 <xyzModem_get_hdr+0x174>
    fb54:	e5973000 	ldr	r3, [r7]
    fb58:	e5d30000 	ldrb	r0, [r3]
    fb5c:	ebfffec7 	bl	f680 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fb60:	e3500000 	cmp	r0, #0	; 0x0
    fb64:	e0872004 	add	r2, r7, r4
    fb68:	e2844001 	add	r4, r4, #1	; 0x1
    fb6c:	0a000042 	beq	fc7c <xyzModem_get_hdr+0x278>
    fb70:	e5dd3007 	ldrb	r3, [sp, #7]
    fb74:	e5c23004 	strb	r3, [r2, #4]
    fb78:	e5973410 	ldr	r3, [r7, #1040]
    fb7c:	e1a01005 	mov	r1, r5
    fb80:	e1540003 	cmp	r4, r3
    fb84:	e59f6124 	ldr	r6, [pc, #292]	; fcb0 <xyzModem_get_hdr+0x2ac>
    fb88:	bafffff1 	blt	fb54 <xyzModem_get_hdr+0x150>
    fb8c:	e5963000 	ldr	r3, [r6]
    fb90:	e59f1124 	ldr	r1, [pc, #292]	; fcbc <xyzModem_get_hdr+0x2b8>
    fb94:	e5d30000 	ldrb	r0, [r3]
    fb98:	ebfffeb8 	bl	f680 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fb9c:	e3500000 	cmp	r0, #0	; 0x0
    fba0:	0a000035 	beq	fc7c <xyzModem_get_hdr+0x278>
    fba4:	e5963428 	ldr	r3, [r6, #1064]
    fba8:	e3530000 	cmp	r3, #0	; 0x0
    fbac:	0a000005 	beq	fbc8 <xyzModem_get_hdr+0x1c4>
    fbb0:	e5963000 	ldr	r3, [r6]
    fbb4:	e59f1104 	ldr	r1, [pc, #260]	; fcc0 <xyzModem_get_hdr+0x2bc>
    fbb8:	e5d30000 	ldrb	r0, [r3]
    fbbc:	ebfffeaf 	bl	f680 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fbc0:	e3500000 	cmp	r0, #0	; 0x0
    fbc4:	0a00002c 	beq	fc7c <xyzModem_get_hdr+0x278>
    fbc8:	e59f40e0 	ldr	r4, [pc, #224]	; fcb0 <xyzModem_get_hdr+0x2ac>
    fbcc:	e5d43409 	ldrb	r3, [r4, #1033]
    fbd0:	e5d42408 	ldrb	r2, [r4, #1032]
    fbd4:	e0233002 	eor	r3, r3, r2
    fbd8:	e35300ff 	cmp	r3, #255	; 0xff
    fbdc:	0a000009 	beq	fc08 <xyzModem_get_hdr+0x204>
    fbe0:	e1a05004 	mov	r5, r4
    fbe4:	e28d4006 	add	r4, sp, #6	; 0x6
    fbe8:	e5953000 	ldr	r3, [r5]
    fbec:	e1a01004 	mov	r1, r4
    fbf0:	e5d30000 	ldrb	r0, [r3]
    fbf4:	ebfffea1 	bl	f680 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fbf8:	e3500000 	cmp	r0, #0	; 0x0
    fbfc:	1afffff9 	bne	fbe8 <xyzModem_get_hdr+0x1e4>
    fc00:	e3e00005 	mvn	r0, #5	; 0x5
    fc04:	ea00001f 	b	fc88 <xyzModem_get_hdr+0x284>
    fc08:	e5943428 	ldr	r3, [r4, #1064]
    fc0c:	e3530000 	cmp	r3, #0	; 0x0
    fc10:	01a02003 	moveq	r2, r3
    fc14:	01a0c004 	moveq	ip, r4
    fc18:	05940410 	ldreq	r0, [r4, #1040]
    fc1c:	01a01003 	moveq	r1, r3
    fc20:	0a00000a 	beq	fc50 <xyzModem_get_hdr+0x24c>
    fc24:	e2840004 	add	r0, r4, #4	; 0x4
    fc28:	e5941410 	ldr	r1, [r4, #1040]
    fc2c:	eb000110 	bl	10074 <cyg_crc16>
    fc30:	e5d4340b 	ldrb	r3, [r4, #1035]
    fc34:	e5d4240a 	ldrb	r2, [r4, #1034]
    fc38:	e1833402 	orr	r3, r3, r2, lsl #8
    fc3c:	e1500003 	cmp	r0, r3
    fc40:	ea00000a 	b	fc70 <xyzModem_get_hdr+0x26c>
    fc44:	e5d33004 	ldrb	r3, [r3, #4]
    fc48:	e0813003 	add	r3, r1, r3
    fc4c:	e6ff1073 	uxth	r1, r3
    fc50:	e1520000 	cmp	r2, r0
    fc54:	e08c3002 	add	r3, ip, r2
    fc58:	e2822001 	add	r2, r2, #1	; 0x1
    fc5c:	bafffff8 	blt	fc44 <xyzModem_get_hdr+0x240>
    fc60:	e59f3048 	ldr	r3, [pc, #72]	; fcb0 <xyzModem_get_hdr+0x2ac>
    fc64:	e5d3240a 	ldrb	r2, [r3, #1034]
    fc68:	e20130ff 	and	r3, r1, #255	; 0xff
    fc6c:	e1520003 	cmp	r2, r3
    fc70:	13e00006 	mvnne	r0, #6	; 0x6
    fc74:	1a000003 	bne	fc88 <xyzModem_get_hdr+0x284>
    fc78:	ea000001 	b	fc84 <xyzModem_get_hdr+0x280>
    fc7c:	e3e00002 	mvn	r0, #2	; 0x2
    fc80:	ea000000 	b	fc88 <xyzModem_get_hdr+0x284>
    fc84:	e3a00000 	mov	r0, #0	; 0x0
    fc88:	e28dd00c 	add	sp, sp, #12	; 0xc
    fc8c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    fc90:	e59f4018 	ldr	r4, [pc, #24]	; fcb0 <xyzModem_get_hdr+0x2ac>
    fc94:	e59f1028 	ldr	r1, [pc, #40]	; fcc4 <xyzModem_get_hdr+0x2c0>
    fc98:	e5943000 	ldr	r3, [r4]
    fc9c:	e5d30000 	ldrb	r0, [r3]
    fca0:	ebfffe76 	bl	f680 <CYGACC_COMM_IF_GETC_TIMEOUT>
    fca4:	e3500000 	cmp	r0, #0	; 0x0
    fca8:	0afffff3 	beq	fc7c <xyzModem_get_hdr+0x278>
    fcac:	eaffff97 	b	fb10 <xyzModem_get_hdr+0x10c>
    fcb0:	0800d330 	.word	0x0800d330
    fcb4:	0003d090 	.word	0x0003d090
    fcb8:	0800d739 	.word	0x0800d739
    fcbc:	0800d73a 	.word	0x0800d73a
    fcc0:	0800d73b 	.word	0x0800d73b
    fcc4:	0800d738 	.word	0x0800d738

0000fcc8 <xyzModem_stream_read>:
    fcc8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    fccc:	e59f5240 	ldr	r5, [pc, #576]	; ff14 <xyzModem_stream_read+0x24c>
    fcd0:	e24dd004 	sub	sp, sp, #4	; 0x4
    fcd4:	e1a08000 	mov	r8, r0
    fcd8:	e1a07001 	mov	r7, r1
    fcdc:	e1a0b002 	mov	fp, r2
    fce0:	e3e06004 	mvn	r6, #4	; 0x4
    fce4:	e3a0a000 	mov	sl, #0	; 0x0
    fce8:	e1a09005 	mov	r9, r5
    fcec:	ea00007e 	b	feec <xyzModem_stream_read+0x224>
    fcf0:	e5953410 	ldr	r3, [r5, #1040]
    fcf4:	e3530000 	cmp	r3, #0	; 0x0
    fcf8:	03a04014 	moveq	r4, #20	; 0x14
    fcfc:	0a00005d 	beq	fe78 <xyzModem_stream_read+0x1b0>
    fd00:	ea000065 	b	fe9c <xyzModem_stream_read+0x1d4>
    fd04:	ebffff3e 	bl	fa04 <xyzModem_get_hdr>
    fd08:	e2506000 	subs	r6, r0, #0	; 0x0
    fd0c:	1a000039 	bne	fdf8 <xyzModem_stream_read+0x130>
    fd10:	e5d5240c 	ldrb	r2, [r5, #1036]
    fd14:	e5d51408 	ldrb	r1, [r5, #1032]
    fd18:	e2423001 	sub	r3, r2, #1	; 0x1
    fd1c:	e1510002 	cmp	r1, r2
    fd20:	e20330ff 	and	r3, r3, #255	; 0xff
    fd24:	1a00002d 	bne	fde0 <xyzModem_stream_read+0x118>
    fd28:	e599c414 	ldr	ip, [r9, #1044]
    fd2c:	e2813001 	add	r3, r1, #1	; 0x1
    fd30:	e5c9340c 	strb	r3, [r9, #1036]
    fd34:	e35c0001 	cmp	ip, #1	; 0x1
    fd38:	e3a03001 	mov	r3, #1	; 0x1
    fd3c:	e5893430 	str	r3, [r9, #1072]
    fd40:	0a000002 	beq	fd50 <xyzModem_stream_read+0x88>
    fd44:	e5993434 	ldr	r3, [r9, #1076]
    fd48:	e3530000 	cmp	r3, #0	; 0x0
    fd4c:	1a000017 	bne	fdb0 <xyzModem_stream_read+0xe8>
    fd50:	e5953410 	ldr	r3, [r5, #1040]
    fd54:	e5950404 	ldr	r0, [r5, #1028]
    fd58:	e0802003 	add	r2, r0, r3
    fd5c:	e5523001 	ldrb	r3, [r2, #-1]
    fd60:	e353001a 	cmp	r3, #26	; 0x1a
    fd64:	1a00000f 	bne	fda8 <xyzModem_stream_read+0xe0>
    fd68:	e5523002 	ldrb	r3, [r2, #-2]
    fd6c:	e353001a 	cmp	r3, #26	; 0x1a
    fd70:	1a00000c 	bne	fda8 <xyzModem_stream_read+0xe0>
    fd74:	e5523003 	ldrb	r3, [r2, #-3]
    fd78:	e353001a 	cmp	r3, #26	; 0x1a
    fd7c:	1a000009 	bne	fda8 <xyzModem_stream_read+0xe0>
    fd80:	ea000000 	b	fd88 <xyzModem_stream_read+0xc0>
    fd84:	e5851410 	str	r1, [r5, #1040]
    fd88:	e5953410 	ldr	r3, [r5, #1040]
    fd8c:	e3530000 	cmp	r3, #0	; 0x0
    fd90:	e0802003 	add	r2, r0, r3
    fd94:	e2431001 	sub	r1, r3, #1	; 0x1
    fd98:	0a000002 	beq	fda8 <xyzModem_stream_read+0xe0>
    fd9c:	e5523001 	ldrb	r3, [r2, #-1]
    fda0:	e353001a 	cmp	r3, #26	; 0x1a
    fda4:	0afffff6 	beq	fd84 <xyzModem_stream_read+0xbc>
    fda8:	e35c0001 	cmp	ip, #1	; 0x1
    fdac:	0a00003a 	beq	fe9c <xyzModem_stream_read+0x1d4>
    fdb0:	e5951434 	ldr	r1, [r5, #1076]
    fdb4:	e3510000 	cmp	r1, #0	; 0x0
    fdb8:	0a000037 	beq	fe9c <xyzModem_stream_read+0x1d4>
    fdbc:	e5953438 	ldr	r3, [r5, #1080]
    fdc0:	e5950410 	ldr	r0, [r5, #1040]
    fdc4:	e0802003 	add	r2, r0, r3
    fdc8:	e1520001 	cmp	r2, r1
    fdcc:	80803001 	addhi	r3, r0, r1
    fdd0:	80623003 	rsbhi	r3, r2, r3
    fdd4:	e5852438 	str	r2, [r5, #1080]
    fdd8:	85853410 	strhi	r3, [r5, #1040]
    fddc:	ea00002e 	b	fe9c <xyzModem_stream_read+0x1d4>
    fde0:	e1510003 	cmp	r1, r3
    fde4:	13e06007 	mvnne	r6, #7	; 0x7
    fde8:	1a00001a 	bne	fe58 <xyzModem_stream_read+0x190>
    fdec:	e3a00006 	mov	r0, #6	; 0x6
    fdf0:	ebfff8d4 	bl	e148 <putc>
    fdf4:	ea00001f 	b	fe78 <xyzModem_stream_read+0x1b0>
    fdf8:	e3760005 	cmn	r6, #5	; 0x5
    fdfc:	0a000021 	beq	fe88 <xyzModem_stream_read+0x1c0>
    fe00:	e3760004 	cmn	r6, #4	; 0x4
    fe04:	1a000013 	bne	fe58 <xyzModem_stream_read+0x190>
    fe08:	e3a00006 	mov	r0, #6	; 0x6
    fe0c:	ebfff8cd 	bl	e148 <putc>
    fe10:	e5953414 	ldr	r3, [r5, #1044]
    fe14:	e3530002 	cmp	r3, #2	; 0x2
    fe18:	1a00000b 	bne	fe4c <xyzModem_stream_read+0x184>
    fe1c:	e5950428 	ldr	r0, [r5, #1064]
    fe20:	e3500000 	cmp	r0, #0	; 0x0
    fe24:	13a00043 	movne	r0, #67	; 0x43
    fe28:	03a00015 	moveq	r0, #21	; 0x15
    fe2c:	ebfff8c5 	bl	e148 <putc>
    fe30:	e5953418 	ldr	r3, [r5, #1048]
    fe34:	e2833001 	add	r3, r3, #1	; 0x1
    fe38:	e5853418 	str	r3, [r5, #1048]
    fe3c:	ebfffef0 	bl	fa04 <xyzModem_get_hdr>
    fe40:	e1a06000 	mov	r6, r0
    fe44:	e3a00006 	mov	r0, #6	; 0x6
    fe48:	ebfff8be 	bl	e148 <putc>
    fe4c:	e3a03001 	mov	r3, #1	; 0x1
    fe50:	e585342c 	str	r3, [r5, #1068]
    fe54:	ea000009 	b	fe80 <xyzModem_stream_read+0x1b8>
    fe58:	e5950428 	ldr	r0, [r5, #1064]
    fe5c:	e3500000 	cmp	r0, #0	; 0x0
    fe60:	13a00043 	movne	r0, #67	; 0x43
    fe64:	03a00015 	moveq	r0, #21	; 0x15
    fe68:	ebfff8b6 	bl	e148 <putc>
    fe6c:	e5953418 	ldr	r3, [r5, #1048]
    fe70:	e2833001 	add	r3, r3, #1	; 0x1
    fe74:	e5853418 	str	r3, [r5, #1048]
    fe78:	e2544001 	subs	r4, r4, #1	; 0x1
    fe7c:	2affffa0 	bcs	fd04 <xyzModem_stream_read+0x3c>
    fe80:	e3560000 	cmp	r6, #0	; 0x0
    fe84:	aa000004 	bge	fe9c <xyzModem_stream_read+0x1d4>
    fe88:	e59f3084 	ldr	r3, [pc, #132]	; ff14 <xyzModem_stream_read+0x24c>
    fe8c:	e3e02000 	mvn	r2, #0	; 0x0
    fe90:	e58b6000 	str	r6, [fp]
    fe94:	e5832410 	str	r2, [r3, #1040]
    fe98:	ea00001a 	b	ff08 <xyzModem_stream_read+0x240>
    fe9c:	e595342c 	ldr	r3, [r5, #1068]
    fea0:	e3530000 	cmp	r3, #0	; 0x0
    fea4:	1a000010 	bne	feec <xyzModem_stream_read+0x224>
    fea8:	e5954410 	ldr	r4, [r5, #1040]
    feac:	e1a00008 	mov	r0, r8
    feb0:	e1570004 	cmp	r7, r4
    feb4:	b1a04007 	movlt	r4, r7
    feb8:	a1a04004 	movge	r4, r4
    febc:	e1a02004 	mov	r2, r4
    fec0:	e5951404 	ldr	r1, [r5, #1028]
    fec4:	ebffc51e 	bl	1344 <memcpy>
    fec8:	e5953410 	ldr	r3, [r5, #1040]
    fecc:	e5952404 	ldr	r2, [r5, #1028]
    fed0:	e0643003 	rsb	r3, r4, r3
    fed4:	e0822004 	add	r2, r2, r4
    fed8:	e5853410 	str	r3, [r5, #1040]
    fedc:	e5852404 	str	r2, [r5, #1028]
    fee0:	e0647007 	rsb	r7, r4, r7
    fee4:	e0888004 	add	r8, r8, r4
    fee8:	e08aa004 	add	sl, sl, r4
    feec:	e595342c 	ldr	r3, [r5, #1068]
    fef0:	e2733001 	rsbs	r3, r3, #1	; 0x1
    fef4:	33a03000 	movcc	r3, #0	; 0x0
    fef8:	e3570000 	cmp	r7, #0	; 0x0
    fefc:	d3a03000 	movle	r3, #0	; 0x0
    ff00:	e3530000 	cmp	r3, #0	; 0x0
    ff04:	1affff79 	bne	fcf0 <xyzModem_stream_read+0x28>
    ff08:	e1a0000a 	mov	r0, sl
    ff0c:	e28dd004 	add	sp, sp, #4	; 0x4
    ff10:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ff14:	0800d330 	.word	0x0800d330

0000ff18 <xyzModem_stream_open>:
    ff18:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    ff1c:	e59f4140 	ldr	r4, [pc, #320]	; 10064 <xyzModem_stream_open+0x14c>
    ff20:	e24dd00c 	sub	sp, sp, #12	; 0xc
    ff24:	e3a05000 	mov	r5, #0	; 0x0
    ff28:	e28d3008 	add	r3, sp, #8	; 0x8
    ff2c:	e3a02001 	mov	r2, #1	; 0x1
    ff30:	e5235004 	str	r5, [r3, #-4]!
    ff34:	e5842428 	str	r2, [r4, #1064]
    ff38:	e5845410 	str	r5, [r4, #1040]
    ff3c:	e584542c 	str	r5, [r4, #1068]
    ff40:	e5845430 	str	r5, [r4, #1072]
    ff44:	e5902004 	ldr	r2, [r0, #4]
    ff48:	e5843000 	str	r3, [r4]
    ff4c:	e3a00043 	mov	r0, #67	; 0x43
    ff50:	e5842414 	str	r2, [r4, #1044]
    ff54:	e5845418 	str	r5, [r4, #1048]
    ff58:	e584541c 	str	r5, [r4, #1052]
    ff5c:	e5845420 	str	r5, [r4, #1056]
    ff60:	e5845424 	str	r5, [r4, #1060]
    ff64:	e5845438 	str	r5, [r4, #1080]
    ff68:	e5845434 	str	r5, [r4, #1076]
    ff6c:	e1a08001 	mov	r8, r1
    ff70:	ebfff874 	bl	e148 <putc>
    ff74:	e5943414 	ldr	r3, [r4, #1044]
    ff78:	e3530001 	cmp	r3, #1	; 0x1
    ff7c:	05c4340c 	strbeq	r3, [r4, #1036]
    ff80:	01a00005 	moveq	r0, r5
    ff84:	0a000034 	beq	1005c <xyzModem_stream_open+0x144>
    ff88:	e1a07004 	mov	r7, r4
    ff8c:	e3a06014 	mov	r6, #20	; 0x14
    ff90:	e3a0400a 	mov	r4, #10	; 0xa
    ff94:	e1a0a005 	mov	sl, r5
    ff98:	ea00002b 	b	1004c <xyzModem_stream_open+0x134>
    ff9c:	ebfffe98 	bl	fa04 <xyzModem_get_hdr>
    ffa0:	e2505000 	subs	r5, r0, #0	; 0x0
    ffa4:	1a000016 	bne	10004 <xyzModem_stream_open+0xec>
    ffa8:	e59f20b4 	ldr	r2, [pc, #180]	; 10064 <xyzModem_stream_open+0x14c>
    ffac:	e5d23408 	ldrb	r3, [r2, #1032]
    ffb0:	e3530000 	cmp	r3, #0	; 0x0
    ffb4:	1a00000b 	bne	ffe8 <xyzModem_stream_open+0xd0>
    ffb8:	e1a03002 	mov	r3, r2
    ffbc:	e5930404 	ldr	r0, [r3, #1028]
    ffc0:	e4d02001 	ldrb	r2, [r0], #1
    ffc4:	e3520000 	cmp	r2, #0	; 0x0
    ffc8:	e5830404 	str	r0, [r3, #1028]
    ffcc:	1afffffa 	bne	ffbc <xyzModem_stream_open+0xa4>
    ffd0:	e59f3090 	ldr	r3, [pc, #144]	; 10068 <xyzModem_stream_open+0x150>
    ffd4:	e59f1090 	ldr	r1, [pc, #144]	; 1006c <xyzModem_stream_open+0x154>
    ffd8:	ebfffb5a 	bl	ed48 <parse_num>
    ffdc:	e59f3080 	ldr	r3, [pc, #128]	; 10064 <xyzModem_stream_open+0x14c>
    ffe0:	e3a02001 	mov	r2, #1	; 0x1
    ffe4:	e5832430 	str	r2, [r3, #1072]
    ffe8:	e59f3074 	ldr	r3, [pc, #116]	; 10064 <xyzModem_stream_open+0x14c>
    ffec:	e3a01000 	mov	r1, #0	; 0x0
    fff0:	e3a02001 	mov	r2, #1	; 0x1
    fff4:	e5c3240c 	strb	r2, [r3, #1036]
    fff8:	e5831410 	str	r1, [r3, #1040]
    fffc:	e1a00001 	mov	r0, r1
   10000:	ea000015 	b	1005c <xyzModem_stream_open+0x144>
   10004:	e3750003 	cmn	r5, #3	; 0x3
   10008:	1a00000d 	bne	10044 <xyzModem_stream_open+0x12c>
   1000c:	e2444001 	sub	r4, r4, #1	; 0x1
   10010:	e3540000 	cmp	r4, #0	; 0x0
   10014:	e59f0054 	ldr	r0, [pc, #84]	; 10070 <xyzModem_stream_open+0x158>
   10018:	d587a428 	strle	sl, [r7, #1064]
   1001c:	ebffe20b 	bl	8850 <udelay>
   10020:	e5970428 	ldr	r0, [r7, #1064]
   10024:	e3500000 	cmp	r0, #0	; 0x0
   10028:	13a00043 	movne	r0, #67	; 0x43
   1002c:	03a00015 	moveq	r0, #21	; 0x15
   10030:	ebfff844 	bl	e148 <putc>
   10034:	e5973418 	ldr	r3, [r7, #1048]
   10038:	e2833001 	add	r3, r3, #1	; 0x1
   1003c:	e5873418 	str	r3, [r7, #1048]
   10040:	ea000001 	b	1004c <xyzModem_stream_open+0x134>
   10044:	e3750005 	cmn	r5, #5	; 0x5
   10048:	0a000001 	beq	10054 <xyzModem_stream_open+0x13c>
   1004c:	e2566001 	subs	r6, r6, #1	; 0x1
   10050:	2affffd1 	bcs	ff9c <xyzModem_stream_open+0x84>
   10054:	e5885000 	str	r5, [r8]
   10058:	e3e00000 	mvn	r0, #0	; 0x0
   1005c:	e28dd00c 	add	sp, sp, #12	; 0xc
   10060:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
   10064:	0800d330 	.word	0x0800d330
   10068:	000141ba 	.word	0x000141ba
   1006c:	0800d764 	.word	0x0800d764
   10070:	0007a120 	.word	0x0007a120

00010074 <cyg_crc16>:
   10074:	e3a02000 	mov	r2, #0	; 0x0
   10078:	e92d4010 	push	{r4, lr}
   1007c:	e1a0c000 	mov	ip, r0
   10080:	e59f402c 	ldr	r4, [pc, #44]	; 100b4 <cyg_crc16+0x40>
   10084:	e1a00002 	mov	r0, r2
   10088:	ea000006 	b	100a8 <cyg_crc16+0x34>
   1008c:	e7dc3002 	ldrb	r3, [ip, r2]
   10090:	e2822001 	add	r2, r2, #1	; 0x1
   10094:	e0233420 	eor	r3, r3, r0, lsr #8
   10098:	e1a03083 	lsl	r3, r3, #1
   1009c:	e19430b3 	ldrh	r3, [r4, r3]
   100a0:	e0233400 	eor	r3, r3, r0, lsl #8
   100a4:	e6ff0073 	uxth	r0, r3
   100a8:	e1520001 	cmp	r2, r1
   100ac:	bafffff6 	blt	1008c <cyg_crc16+0x18>
   100b0:	e8bd8010 	pop	{r4, pc}
   100b4:	000115e4 	.word	0x000115e4

000100b8 <ss_jtag_en>:
   100b8:	e59f300c 	ldr	r3, [pc, #12]	; 100cc <ss_jtag_en+0x14>
   100bc:	e2500000 	subs	r0, r0, #0	; 0x0
   100c0:	13a00001 	movne	r0, #1	; 0x1
   100c4:	e58308b8 	str	r0, [r3, #2232]
   100c8:	e12fff1e 	bx	lr
   100cc:	2d008000 	.word	0x2d008000

000100d0 <raise>:
   100d0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   100d4:	e1a01000 	mov	r1, r0
   100d8:	e24dd004 	sub	sp, sp, #4	; 0x4
   100dc:	e59f0014 	ldr	r0, [pc, #20]	; 100f8 <raise+0x28>
   100e0:	e59f3014 	ldr	r3, [pc, #20]	; 100fc <raise+0x2c>
   100e4:	e1a0e00f 	mov	lr, pc
   100e8:	e593f000 	ldr	pc, [r3]
   100ec:	e3a00000 	mov	r0, #0	; 0x0
   100f0:	e28dd004 	add	sp, sp, #4	; 0x4
   100f4:	e8bd8000 	pop	{pc}
   100f8:	00014456 	.word	0x00014456
   100fc:	0800d76c 	.word	0x0800d76c

00010100 <__aeabi_uidiv>:
   10100:	e2512001 	subs	r2, r1, #1	; 0x1
   10104:	012fff1e 	bxeq	lr
   10108:	3a000074 	bcc	102e0 <__aeabi_uidiv+0x1e0>
   1010c:	e1500001 	cmp	r0, r1
   10110:	9a00006b 	bls	102c4 <__aeabi_uidiv+0x1c4>
   10114:	e1110002 	tst	r1, r2
   10118:	0a00006c 	beq	102d0 <__aeabi_uidiv+0x1d0>
   1011c:	e16f3f10 	clz	r3, r0
   10120:	e16f2f11 	clz	r2, r1
   10124:	e0423003 	sub	r3, r2, r3
   10128:	e273301f 	rsbs	r3, r3, #31	; 0x1f
   1012c:	10833083 	addne	r3, r3, r3, lsl #1
   10130:	e3a02000 	mov	r2, #0	; 0x0
   10134:	108ff103 	addne	pc, pc, r3, lsl #2
   10138:	e1a00000 	nop			(mov r0,r0)
   1013c:	e1500f81 	cmp	r0, r1, lsl #31
   10140:	e0a22002 	adc	r2, r2, r2
   10144:	20400f81 	subcs	r0, r0, r1, lsl #31
   10148:	e1500f01 	cmp	r0, r1, lsl #30
   1014c:	e0a22002 	adc	r2, r2, r2
   10150:	20400f01 	subcs	r0, r0, r1, lsl #30
   10154:	e1500e81 	cmp	r0, r1, lsl #29
   10158:	e0a22002 	adc	r2, r2, r2
   1015c:	20400e81 	subcs	r0, r0, r1, lsl #29
   10160:	e1500e01 	cmp	r0, r1, lsl #28
   10164:	e0a22002 	adc	r2, r2, r2
   10168:	20400e01 	subcs	r0, r0, r1, lsl #28
   1016c:	e1500d81 	cmp	r0, r1, lsl #27
   10170:	e0a22002 	adc	r2, r2, r2
   10174:	20400d81 	subcs	r0, r0, r1, lsl #27
   10178:	e1500d01 	cmp	r0, r1, lsl #26
   1017c:	e0a22002 	adc	r2, r2, r2
   10180:	20400d01 	subcs	r0, r0, r1, lsl #26
   10184:	e1500c81 	cmp	r0, r1, lsl #25
   10188:	e0a22002 	adc	r2, r2, r2
   1018c:	20400c81 	subcs	r0, r0, r1, lsl #25
   10190:	e1500c01 	cmp	r0, r1, lsl #24
   10194:	e0a22002 	adc	r2, r2, r2
   10198:	20400c01 	subcs	r0, r0, r1, lsl #24
   1019c:	e1500b81 	cmp	r0, r1, lsl #23
   101a0:	e0a22002 	adc	r2, r2, r2
   101a4:	20400b81 	subcs	r0, r0, r1, lsl #23
   101a8:	e1500b01 	cmp	r0, r1, lsl #22
   101ac:	e0a22002 	adc	r2, r2, r2
   101b0:	20400b01 	subcs	r0, r0, r1, lsl #22
   101b4:	e1500a81 	cmp	r0, r1, lsl #21
   101b8:	e0a22002 	adc	r2, r2, r2
   101bc:	20400a81 	subcs	r0, r0, r1, lsl #21
   101c0:	e1500a01 	cmp	r0, r1, lsl #20
   101c4:	e0a22002 	adc	r2, r2, r2
   101c8:	20400a01 	subcs	r0, r0, r1, lsl #20
   101cc:	e1500981 	cmp	r0, r1, lsl #19
   101d0:	e0a22002 	adc	r2, r2, r2
   101d4:	20400981 	subcs	r0, r0, r1, lsl #19
   101d8:	e1500901 	cmp	r0, r1, lsl #18
   101dc:	e0a22002 	adc	r2, r2, r2
   101e0:	20400901 	subcs	r0, r0, r1, lsl #18
   101e4:	e1500881 	cmp	r0, r1, lsl #17
   101e8:	e0a22002 	adc	r2, r2, r2
   101ec:	20400881 	subcs	r0, r0, r1, lsl #17
   101f0:	e1500801 	cmp	r0, r1, lsl #16
   101f4:	e0a22002 	adc	r2, r2, r2
   101f8:	20400801 	subcs	r0, r0, r1, lsl #16
   101fc:	e1500781 	cmp	r0, r1, lsl #15
   10200:	e0a22002 	adc	r2, r2, r2
   10204:	20400781 	subcs	r0, r0, r1, lsl #15
   10208:	e1500701 	cmp	r0, r1, lsl #14
   1020c:	e0a22002 	adc	r2, r2, r2
   10210:	20400701 	subcs	r0, r0, r1, lsl #14
   10214:	e1500681 	cmp	r0, r1, lsl #13
   10218:	e0a22002 	adc	r2, r2, r2
   1021c:	20400681 	subcs	r0, r0, r1, lsl #13
   10220:	e1500601 	cmp	r0, r1, lsl #12
   10224:	e0a22002 	adc	r2, r2, r2
   10228:	20400601 	subcs	r0, r0, r1, lsl #12
   1022c:	e1500581 	cmp	r0, r1, lsl #11
   10230:	e0a22002 	adc	r2, r2, r2
   10234:	20400581 	subcs	r0, r0, r1, lsl #11
   10238:	e1500501 	cmp	r0, r1, lsl #10
   1023c:	e0a22002 	adc	r2, r2, r2
   10240:	20400501 	subcs	r0, r0, r1, lsl #10
   10244:	e1500481 	cmp	r0, r1, lsl #9
   10248:	e0a22002 	adc	r2, r2, r2
   1024c:	20400481 	subcs	r0, r0, r1, lsl #9
   10250:	e1500401 	cmp	r0, r1, lsl #8
   10254:	e0a22002 	adc	r2, r2, r2
   10258:	20400401 	subcs	r0, r0, r1, lsl #8
   1025c:	e1500381 	cmp	r0, r1, lsl #7
   10260:	e0a22002 	adc	r2, r2, r2
   10264:	20400381 	subcs	r0, r0, r1, lsl #7
   10268:	e1500301 	cmp	r0, r1, lsl #6
   1026c:	e0a22002 	adc	r2, r2, r2
   10270:	20400301 	subcs	r0, r0, r1, lsl #6
   10274:	e1500281 	cmp	r0, r1, lsl #5
   10278:	e0a22002 	adc	r2, r2, r2
   1027c:	20400281 	subcs	r0, r0, r1, lsl #5
   10280:	e1500201 	cmp	r0, r1, lsl #4
   10284:	e0a22002 	adc	r2, r2, r2
   10288:	20400201 	subcs	r0, r0, r1, lsl #4
   1028c:	e1500181 	cmp	r0, r1, lsl #3
   10290:	e0a22002 	adc	r2, r2, r2
   10294:	20400181 	subcs	r0, r0, r1, lsl #3
   10298:	e1500101 	cmp	r0, r1, lsl #2
   1029c:	e0a22002 	adc	r2, r2, r2
   102a0:	20400101 	subcs	r0, r0, r1, lsl #2
   102a4:	e1500081 	cmp	r0, r1, lsl #1
   102a8:	e0a22002 	adc	r2, r2, r2
   102ac:	20400081 	subcs	r0, r0, r1, lsl #1
   102b0:	e1500001 	cmp	r0, r1
   102b4:	e0a22002 	adc	r2, r2, r2
   102b8:	20400001 	subcs	r0, r0, r1
   102bc:	e1a00002 	mov	r0, r2
   102c0:	e12fff1e 	bx	lr
   102c4:	03a00001 	moveq	r0, #1	; 0x1
   102c8:	13a00000 	movne	r0, #0	; 0x0
   102cc:	e12fff1e 	bx	lr
   102d0:	e16f2f11 	clz	r2, r1
   102d4:	e262201f 	rsb	r2, r2, #31	; 0x1f
   102d8:	e1a00230 	lsr	r0, r0, r2
   102dc:	e12fff1e 	bx	lr
   102e0:	e52de008 	str	lr, [sp, #-8]!
   102e4:	eb0002c4 	bl	10dfc <__div0>
   102e8:	e3a00000 	mov	r0, #0	; 0x0
   102ec:	e49df008 	ldr	pc, [sp], #8

000102f0 <__aeabi_uidivmod>:
   102f0:	e92d4003 	push	{r0, r1, lr}
   102f4:	ebffff81 	bl	10100 <__aeabi_uidiv>
   102f8:	e8bd4006 	pop	{r1, r2, lr}
   102fc:	e0030092 	mul	r3, r2, r0
   10300:	e0411003 	sub	r1, r1, r3
   10304:	e12fff1e 	bx	lr

00010308 <__aeabi_idiv>:
   10308:	e3510000 	cmp	r1, #0	; 0x0
   1030c:	e020c001 	eor	ip, r0, r1
   10310:	0a000080 	beq	10518 <__aeabi_idiv+0x210>
   10314:	42611000 	rsbmi	r1, r1, #0	; 0x0
   10318:	e2512001 	subs	r2, r1, #1	; 0x1
   1031c:	0a000070 	beq	104e4 <__aeabi_idiv+0x1dc>
   10320:	e1b03000 	movs	r3, r0
   10324:	42603000 	rsbmi	r3, r0, #0	; 0x0
   10328:	e1530001 	cmp	r3, r1
   1032c:	9a00006f 	bls	104f0 <__aeabi_idiv+0x1e8>
   10330:	e1110002 	tst	r1, r2
   10334:	0a000071 	beq	10500 <__aeabi_idiv+0x1f8>
   10338:	e16f2f13 	clz	r2, r3
   1033c:	e16f0f11 	clz	r0, r1
   10340:	e0402002 	sub	r2, r0, r2
   10344:	e272201f 	rsbs	r2, r2, #31	; 0x1f
   10348:	10822082 	addne	r2, r2, r2, lsl #1
   1034c:	e3a00000 	mov	r0, #0	; 0x0
   10350:	108ff102 	addne	pc, pc, r2, lsl #2
   10354:	e1a00000 	nop			(mov r0,r0)
   10358:	e1530f81 	cmp	r3, r1, lsl #31
   1035c:	e0a00000 	adc	r0, r0, r0
   10360:	20433f81 	subcs	r3, r3, r1, lsl #31
   10364:	e1530f01 	cmp	r3, r1, lsl #30
   10368:	e0a00000 	adc	r0, r0, r0
   1036c:	20433f01 	subcs	r3, r3, r1, lsl #30
   10370:	e1530e81 	cmp	r3, r1, lsl #29
   10374:	e0a00000 	adc	r0, r0, r0
   10378:	20433e81 	subcs	r3, r3, r1, lsl #29
   1037c:	e1530e01 	cmp	r3, r1, lsl #28
   10380:	e0a00000 	adc	r0, r0, r0
   10384:	20433e01 	subcs	r3, r3, r1, lsl #28
   10388:	e1530d81 	cmp	r3, r1, lsl #27
   1038c:	e0a00000 	adc	r0, r0, r0
   10390:	20433d81 	subcs	r3, r3, r1, lsl #27
   10394:	e1530d01 	cmp	r3, r1, lsl #26
   10398:	e0a00000 	adc	r0, r0, r0
   1039c:	20433d01 	subcs	r3, r3, r1, lsl #26
   103a0:	e1530c81 	cmp	r3, r1, lsl #25
   103a4:	e0a00000 	adc	r0, r0, r0
   103a8:	20433c81 	subcs	r3, r3, r1, lsl #25
   103ac:	e1530c01 	cmp	r3, r1, lsl #24
   103b0:	e0a00000 	adc	r0, r0, r0
   103b4:	20433c01 	subcs	r3, r3, r1, lsl #24
   103b8:	e1530b81 	cmp	r3, r1, lsl #23
   103bc:	e0a00000 	adc	r0, r0, r0
   103c0:	20433b81 	subcs	r3, r3, r1, lsl #23
   103c4:	e1530b01 	cmp	r3, r1, lsl #22
   103c8:	e0a00000 	adc	r0, r0, r0
   103cc:	20433b01 	subcs	r3, r3, r1, lsl #22
   103d0:	e1530a81 	cmp	r3, r1, lsl #21
   103d4:	e0a00000 	adc	r0, r0, r0
   103d8:	20433a81 	subcs	r3, r3, r1, lsl #21
   103dc:	e1530a01 	cmp	r3, r1, lsl #20
   103e0:	e0a00000 	adc	r0, r0, r0
   103e4:	20433a01 	subcs	r3, r3, r1, lsl #20
   103e8:	e1530981 	cmp	r3, r1, lsl #19
   103ec:	e0a00000 	adc	r0, r0, r0
   103f0:	20433981 	subcs	r3, r3, r1, lsl #19
   103f4:	e1530901 	cmp	r3, r1, lsl #18
   103f8:	e0a00000 	adc	r0, r0, r0
   103fc:	20433901 	subcs	r3, r3, r1, lsl #18
   10400:	e1530881 	cmp	r3, r1, lsl #17
   10404:	e0a00000 	adc	r0, r0, r0
   10408:	20433881 	subcs	r3, r3, r1, lsl #17
   1040c:	e1530801 	cmp	r3, r1, lsl #16
   10410:	e0a00000 	adc	r0, r0, r0
   10414:	20433801 	subcs	r3, r3, r1, lsl #16
   10418:	e1530781 	cmp	r3, r1, lsl #15
   1041c:	e0a00000 	adc	r0, r0, r0
   10420:	20433781 	subcs	r3, r3, r1, lsl #15
   10424:	e1530701 	cmp	r3, r1, lsl #14
   10428:	e0a00000 	adc	r0, r0, r0
   1042c:	20433701 	subcs	r3, r3, r1, lsl #14
   10430:	e1530681 	cmp	r3, r1, lsl #13
   10434:	e0a00000 	adc	r0, r0, r0
   10438:	20433681 	subcs	r3, r3, r1, lsl #13
   1043c:	e1530601 	cmp	r3, r1, lsl #12
   10440:	e0a00000 	adc	r0, r0, r0
   10444:	20433601 	subcs	r3, r3, r1, lsl #12
   10448:	e1530581 	cmp	r3, r1, lsl #11
   1044c:	e0a00000 	adc	r0, r0, r0
   10450:	20433581 	subcs	r3, r3, r1, lsl #11
   10454:	e1530501 	cmp	r3, r1, lsl #10
   10458:	e0a00000 	adc	r0, r0, r0
   1045c:	20433501 	subcs	r3, r3, r1, lsl #10
   10460:	e1530481 	cmp	r3, r1, lsl #9
   10464:	e0a00000 	adc	r0, r0, r0
   10468:	20433481 	subcs	r3, r3, r1, lsl #9
   1046c:	e1530401 	cmp	r3, r1, lsl #8
   10470:	e0a00000 	adc	r0, r0, r0
   10474:	20433401 	subcs	r3, r3, r1, lsl #8
   10478:	e1530381 	cmp	r3, r1, lsl #7
   1047c:	e0a00000 	adc	r0, r0, r0
   10480:	20433381 	subcs	r3, r3, r1, lsl #7
   10484:	e1530301 	cmp	r3, r1, lsl #6
   10488:	e0a00000 	adc	r0, r0, r0
   1048c:	20433301 	subcs	r3, r3, r1, lsl #6
   10490:	e1530281 	cmp	r3, r1, lsl #5
   10494:	e0a00000 	adc	r0, r0, r0
   10498:	20433281 	subcs	r3, r3, r1, lsl #5
   1049c:	e1530201 	cmp	r3, r1, lsl #4
   104a0:	e0a00000 	adc	r0, r0, r0
   104a4:	20433201 	subcs	r3, r3, r1, lsl #4
   104a8:	e1530181 	cmp	r3, r1, lsl #3
   104ac:	e0a00000 	adc	r0, r0, r0
   104b0:	20433181 	subcs	r3, r3, r1, lsl #3
   104b4:	e1530101 	cmp	r3, r1, lsl #2
   104b8:	e0a00000 	adc	r0, r0, r0
   104bc:	20433101 	subcs	r3, r3, r1, lsl #2
   104c0:	e1530081 	cmp	r3, r1, lsl #1
   104c4:	e0a00000 	adc	r0, r0, r0
   104c8:	20433081 	subcs	r3, r3, r1, lsl #1
   104cc:	e1530001 	cmp	r3, r1
   104d0:	e0a00000 	adc	r0, r0, r0
   104d4:	20433001 	subcs	r3, r3, r1
   104d8:	e35c0000 	cmp	ip, #0	; 0x0
   104dc:	42600000 	rsbmi	r0, r0, #0	; 0x0
   104e0:	e12fff1e 	bx	lr
   104e4:	e13c0000 	teq	ip, r0
   104e8:	42600000 	rsbmi	r0, r0, #0	; 0x0
   104ec:	e12fff1e 	bx	lr
   104f0:	33a00000 	movcc	r0, #0	; 0x0
   104f4:	01a00fcc 	asreq	r0, ip, #31
   104f8:	03800001 	orreq	r0, r0, #1	; 0x1
   104fc:	e12fff1e 	bx	lr
   10500:	e16f2f11 	clz	r2, r1
   10504:	e262201f 	rsb	r2, r2, #31	; 0x1f
   10508:	e35c0000 	cmp	ip, #0	; 0x0
   1050c:	e1a00233 	lsr	r0, r3, r2
   10510:	42600000 	rsbmi	r0, r0, #0	; 0x0
   10514:	e12fff1e 	bx	lr
   10518:	e52de008 	str	lr, [sp, #-8]!
   1051c:	eb000236 	bl	10dfc <__div0>
   10520:	e3a00000 	mov	r0, #0	; 0x0
   10524:	e49df008 	ldr	pc, [sp], #8

00010528 <__aeabi_idivmod>:
   10528:	e92d4003 	push	{r0, r1, lr}
   1052c:	ebffff75 	bl	10308 <__aeabi_idiv>
   10530:	e8bd4006 	pop	{r1, r2, lr}
   10534:	e0030092 	mul	r3, r2, r0
   10538:	e0411003 	sub	r1, r1, r3
   1053c:	e12fff1e 	bx	lr

00010540 <__aeabi_llsr>:
   10540:	e2523020 	subs	r3, r2, #32	; 0x20
   10544:	e262c020 	rsb	ip, r2, #32	; 0x20
   10548:	41a00230 	lsrmi	r0, r0, r2
   1054c:	51a00331 	lsrpl	r0, r1, r3
   10550:	41800c11 	orrmi	r0, r0, r1, lsl ip
   10554:	e1a01231 	lsr	r1, r1, r2
   10558:	e12fff1e 	bx	lr

0001055c <__aeabi_llsl>:
   1055c:	e2523020 	subs	r3, r2, #32	; 0x20
   10560:	e262c020 	rsb	ip, r2, #32	; 0x20
   10564:	41a01211 	lslmi	r1, r1, r2
   10568:	51a01310 	lslpl	r1, r0, r3
   1056c:	41811c30 	orrmi	r1, r1, r0, lsr ip
   10570:	e1a00210 	lsl	r0, r0, r2
   10574:	e12fff1e 	bx	lr

00010578 <__aeabi_drsub>:
   10578:	e2211102 	eor	r1, r1, #-2147483648	; 0x80000000
   1057c:	ea000000 	b	10584 <__adddf3>

00010580 <__aeabi_dsub>:
   10580:	e2233102 	eor	r3, r3, #-2147483648	; 0x80000000

00010584 <__adddf3>:
   10584:	e92d4030 	push	{r4, r5, lr}
   10588:	e1a04081 	lsl	r4, r1, #1
   1058c:	e1a05083 	lsl	r5, r3, #1
   10590:	e1340005 	teq	r4, r5
   10594:	01300002 	teqeq	r0, r2
   10598:	1194c000 	orrsne	ip, r4, r0
   1059c:	1195c002 	orrsne	ip, r5, r2
   105a0:	11f0cac4 	mvnsne	ip, r4, asr #21
   105a4:	11f0cac5 	mvnsne	ip, r5, asr #21
   105a8:	0a000079 	beq	10794 <__adddf3+0x210>
   105ac:	e1a04aa4 	lsr	r4, r4, #21
   105b0:	e0745aa5 	rsbs	r5, r4, r5, lsr #21
   105b4:	b2655000 	rsblt	r5, r5, #0	; 0x0
   105b8:	da000006 	ble	105d8 <__adddf3+0x54>
   105bc:	e0844005 	add	r4, r4, r5
   105c0:	e0202002 	eor	r2, r0, r2
   105c4:	e0213003 	eor	r3, r1, r3
   105c8:	e0220000 	eor	r0, r2, r0
   105cc:	e0231001 	eor	r1, r3, r1
   105d0:	e0202002 	eor	r2, r0, r2
   105d4:	e0213003 	eor	r3, r1, r3
   105d8:	e3550036 	cmp	r5, #54	; 0x36
   105dc:	88bd8030 	pophi	{r4, r5, pc}
   105e0:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   105e4:	e1a01601 	lsl	r1, r1, #12
   105e8:	e3a0c601 	mov	ip, #1048576	; 0x100000
   105ec:	e18c1621 	orr	r1, ip, r1, lsr #12
   105f0:	0a000001 	beq	105fc <__adddf3+0x78>
   105f4:	e2700000 	rsbs	r0, r0, #0	; 0x0
   105f8:	e2e11000 	rsc	r1, r1, #0	; 0x0
   105fc:	e3130102 	tst	r3, #-2147483648	; 0x80000000
   10600:	e1a03603 	lsl	r3, r3, #12
   10604:	e18c3623 	orr	r3, ip, r3, lsr #12
   10608:	0a000001 	beq	10614 <__adddf3+0x90>
   1060c:	e2722000 	rsbs	r2, r2, #0	; 0x0
   10610:	e2e33000 	rsc	r3, r3, #0	; 0x0
   10614:	e1340005 	teq	r4, r5
   10618:	0a000057 	beq	1077c <__adddf3+0x1f8>
   1061c:	e2444001 	sub	r4, r4, #1	; 0x1
   10620:	e275e020 	rsbs	lr, r5, #32	; 0x20
   10624:	ba000005 	blt	10640 <__adddf3+0xbc>
   10628:	e1a0ce12 	lsl	ip, r2, lr
   1062c:	e0900532 	adds	r0, r0, r2, lsr r5
   10630:	e2a11000 	adc	r1, r1, #0	; 0x0
   10634:	e0900e13 	adds	r0, r0, r3, lsl lr
   10638:	e0b11553 	adcs	r1, r1, r3, asr r5
   1063c:	ea000006 	b	1065c <__adddf3+0xd8>
   10640:	e2455020 	sub	r5, r5, #32	; 0x20
   10644:	e28ee020 	add	lr, lr, #32	; 0x20
   10648:	e3520001 	cmp	r2, #1	; 0x1
   1064c:	e1a0ce13 	lsl	ip, r3, lr
   10650:	238cc002 	orrcs	ip, ip, #2	; 0x2
   10654:	e0900553 	adds	r0, r0, r3, asr r5
   10658:	e0b11fc3 	adcs	r1, r1, r3, asr #31
   1065c:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   10660:	5a000002 	bpl	10670 <__adddf3+0xec>
   10664:	e27cc000 	rsbs	ip, ip, #0	; 0x0
   10668:	e2f00000 	rscs	r0, r0, #0	; 0x0
   1066c:	e2e11000 	rsc	r1, r1, #0	; 0x0
   10670:	e3510601 	cmp	r1, #1048576	; 0x100000
   10674:	3a00000e 	bcc	106b4 <__adddf3+0x130>
   10678:	e3510602 	cmp	r1, #2097152	; 0x200000
   1067c:	3a000006 	bcc	1069c <__adddf3+0x118>
   10680:	e1b010a1 	lsrs	r1, r1, #1
   10684:	e1b00060 	rrxs	r0, r0
   10688:	e1a0c06c 	rrx	ip, ip
   1068c:	e2844001 	add	r4, r4, #1	; 0x1
   10690:	e1a02a84 	lsl	r2, r4, #21
   10694:	e3720501 	cmn	r2, #4194304	; 0x400000
   10698:	2a000055 	bcs	107f4 <__adddf3+0x270>
   1069c:	e35c0102 	cmp	ip, #-2147483648	; 0x80000000
   106a0:	01b0c0a0 	lsrseq	ip, r0, #1
   106a4:	e2b00000 	adcs	r0, r0, #0	; 0x0
   106a8:	e0a11a04 	adc	r1, r1, r4, lsl #20
   106ac:	e1811005 	orr	r1, r1, r5
   106b0:	e8bd8030 	pop	{r4, r5, pc}
   106b4:	e1b0c08c 	lsls	ip, ip, #1
   106b8:	e0b00000 	adcs	r0, r0, r0
   106bc:	e0a11001 	adc	r1, r1, r1
   106c0:	e3110601 	tst	r1, #1048576	; 0x100000
   106c4:	e2444001 	sub	r4, r4, #1	; 0x1
   106c8:	1afffff3 	bne	1069c <__adddf3+0x118>
   106cc:	e3310000 	teq	r1, #0	; 0x0
   106d0:	01a01000 	moveq	r1, r0
   106d4:	03a00000 	moveq	r0, #0	; 0x0
   106d8:	e16f3f11 	clz	r3, r1
   106dc:	02833020 	addeq	r3, r3, #32	; 0x20
   106e0:	e243300b 	sub	r3, r3, #11	; 0xb
   106e4:	e2532020 	subs	r2, r3, #32	; 0x20
   106e8:	aa000007 	bge	1070c <__adddf3+0x188>
   106ec:	e292200c 	adds	r2, r2, #12	; 0xc
   106f0:	da000004 	ble	10708 <__adddf3+0x184>
   106f4:	e282c014 	add	ip, r2, #20	; 0x14
   106f8:	e262200c 	rsb	r2, r2, #12	; 0xc
   106fc:	e1a00c11 	lsl	r0, r1, ip
   10700:	e1a01231 	lsr	r1, r1, r2
   10704:	ea000004 	b	1071c <__adddf3+0x198>
   10708:	e2822014 	add	r2, r2, #20	; 0x14
   1070c:	d262c020 	rsble	ip, r2, #32	; 0x20
   10710:	e1a01211 	lsl	r1, r1, r2
   10714:	d1811c30 	orrle	r1, r1, r0, lsr ip
   10718:	d1a00210 	lslle	r0, r0, r2
   1071c:	e0544003 	subs	r4, r4, r3
   10720:	a0811a04 	addge	r1, r1, r4, lsl #20
   10724:	a1811005 	orrge	r1, r1, r5
   10728:	a8bd8030 	popge	{r4, r5, pc}
   1072c:	e1e04004 	mvn	r4, r4
   10730:	e254401f 	subs	r4, r4, #31	; 0x1f
   10734:	aa00000d 	bge	10770 <__adddf3+0x1ec>
   10738:	e294400c 	adds	r4, r4, #12	; 0xc
   1073c:	ca000005 	bgt	10758 <__adddf3+0x1d4>
   10740:	e2844014 	add	r4, r4, #20	; 0x14
   10744:	e2642020 	rsb	r2, r4, #32	; 0x20
   10748:	e1a00430 	lsr	r0, r0, r4
   1074c:	e1800211 	orr	r0, r0, r1, lsl r2
   10750:	e1851431 	orr	r1, r5, r1, lsr r4
   10754:	e8bd8030 	pop	{r4, r5, pc}
   10758:	e264400c 	rsb	r4, r4, #12	; 0xc
   1075c:	e2642020 	rsb	r2, r4, #32	; 0x20
   10760:	e1a00230 	lsr	r0, r0, r2
   10764:	e1800411 	orr	r0, r0, r1, lsl r4
   10768:	e1a01005 	mov	r1, r5
   1076c:	e8bd8030 	pop	{r4, r5, pc}
   10770:	e1a00431 	lsr	r0, r1, r4
   10774:	e1a01005 	mov	r1, r5
   10778:	e8bd8030 	pop	{r4, r5, pc}
   1077c:	e3340000 	teq	r4, #0	; 0x0
   10780:	e2233601 	eor	r3, r3, #1048576	; 0x100000
   10784:	02211601 	eoreq	r1, r1, #1048576	; 0x100000
   10788:	02844001 	addeq	r4, r4, #1	; 0x1
   1078c:	12455001 	subne	r5, r5, #1	; 0x1
   10790:	eaffffa1 	b	1061c <__adddf3+0x98>
   10794:	e1f0cac4 	mvns	ip, r4, asr #21
   10798:	11f0cac5 	mvnsne	ip, r5, asr #21
   1079c:	0a000018 	beq	10804 <__adddf3+0x280>
   107a0:	e1340005 	teq	r4, r5
   107a4:	01300002 	teqeq	r0, r2
   107a8:	0a000003 	beq	107bc <__adddf3+0x238>
   107ac:	e194c000 	orrs	ip, r4, r0
   107b0:	01a01003 	moveq	r1, r3
   107b4:	01a00002 	moveq	r0, r2
   107b8:	e8bd8030 	pop	{r4, r5, pc}
   107bc:	e1310003 	teq	r1, r3
   107c0:	13a01000 	movne	r1, #0	; 0x0
   107c4:	13a00000 	movne	r0, #0	; 0x0
   107c8:	18bd8030 	popne	{r4, r5, pc}
   107cc:	e1b0caa4 	lsrs	ip, r4, #21
   107d0:	1a000003 	bne	107e4 <__adddf3+0x260>
   107d4:	e1b00080 	lsls	r0, r0, #1
   107d8:	e0b11001 	adcs	r1, r1, r1
   107dc:	23811102 	orrcs	r1, r1, #-2147483648	; 0x80000000
   107e0:	e8bd8030 	pop	{r4, r5, pc}
   107e4:	e2944501 	adds	r4, r4, #4194304	; 0x400000
   107e8:	32811601 	addcc	r1, r1, #1048576	; 0x100000
   107ec:	38bd8030 	popcc	{r4, r5, pc}
   107f0:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   107f4:	e385147f 	orr	r1, r5, #2130706432	; 0x7f000000
   107f8:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   107fc:	e3a00000 	mov	r0, #0	; 0x0
   10800:	e8bd8030 	pop	{r4, r5, pc}
   10804:	e1f0cac4 	mvns	ip, r4, asr #21
   10808:	11a01003 	movne	r1, r3
   1080c:	11a00002 	movne	r0, r2
   10810:	01f0cac5 	mvnseq	ip, r5, asr #21
   10814:	11a03001 	movne	r3, r1
   10818:	11a02000 	movne	r2, r0
   1081c:	e1904601 	orrs	r4, r0, r1, lsl #12
   10820:	01925603 	orrseq	r5, r2, r3, lsl #12
   10824:	01310003 	teqeq	r1, r3
   10828:	13811702 	orrne	r1, r1, #524288	; 0x80000
   1082c:	e8bd8030 	pop	{r4, r5, pc}

00010830 <__aeabi_ui2d>:
   10830:	e3300000 	teq	r0, #0	; 0x0
   10834:	03a01000 	moveq	r1, #0	; 0x0
   10838:	012fff1e 	bxeq	lr
   1083c:	e92d4030 	push	{r4, r5, lr}
   10840:	e3a04b01 	mov	r4, #1024	; 0x400
   10844:	e2844032 	add	r4, r4, #50	; 0x32
   10848:	e3a05000 	mov	r5, #0	; 0x0
   1084c:	e3a01000 	mov	r1, #0	; 0x0
   10850:	eaffff9d 	b	106cc <__adddf3+0x148>

00010854 <__aeabi_i2d>:
   10854:	e3300000 	teq	r0, #0	; 0x0
   10858:	03a01000 	moveq	r1, #0	; 0x0
   1085c:	012fff1e 	bxeq	lr
   10860:	e92d4030 	push	{r4, r5, lr}
   10864:	e3a04b01 	mov	r4, #1024	; 0x400
   10868:	e2844032 	add	r4, r4, #50	; 0x32
   1086c:	e2105102 	ands	r5, r0, #-2147483648	; 0x80000000
   10870:	42600000 	rsbmi	r0, r0, #0	; 0x0
   10874:	e3a01000 	mov	r1, #0	; 0x0
   10878:	eaffff93 	b	106cc <__adddf3+0x148>

0001087c <__aeabi_f2d>:
   1087c:	e1b02080 	lsls	r2, r0, #1
   10880:	e1a011c2 	asr	r1, r2, #3
   10884:	e1a01061 	rrx	r1, r1
   10888:	e1a00e02 	lsl	r0, r2, #28
   1088c:	121234ff 	andsne	r3, r2, #-16777216	; 0xff000000
   10890:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   10894:	1221130e 	eorne	r1, r1, #939524096	; 0x38000000
   10898:	112fff1e 	bxne	lr
   1089c:	e3320000 	teq	r2, #0	; 0x0
   108a0:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   108a4:	012fff1e 	bxeq	lr
   108a8:	e92d4030 	push	{r4, r5, lr}
   108ac:	e3a04d0e 	mov	r4, #896	; 0x380
   108b0:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   108b4:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   108b8:	eaffff83 	b	106cc <__adddf3+0x148>

000108bc <__aeabi_ul2d>:
   108bc:	e1902001 	orrs	r2, r0, r1
   108c0:	012fff1e 	bxeq	lr
   108c4:	e92d4030 	push	{r4, r5, lr}
   108c8:	e3a05000 	mov	r5, #0	; 0x0
   108cc:	ea000006 	b	108ec <__aeabi_l2d+0x1c>

000108d0 <__aeabi_l2d>:
   108d0:	e1902001 	orrs	r2, r0, r1
   108d4:	012fff1e 	bxeq	lr
   108d8:	e92d4030 	push	{r4, r5, lr}
   108dc:	e2115102 	ands	r5, r1, #-2147483648	; 0x80000000
   108e0:	5a000001 	bpl	108ec <__aeabi_l2d+0x1c>
   108e4:	e2700000 	rsbs	r0, r0, #0	; 0x0
   108e8:	e2e11000 	rsc	r1, r1, #0	; 0x0
   108ec:	e3a04b01 	mov	r4, #1024	; 0x400
   108f0:	e2844032 	add	r4, r4, #50	; 0x32
   108f4:	e1b0cb21 	lsrs	ip, r1, #22
   108f8:	0affff5c 	beq	10670 <__adddf3+0xec>
   108fc:	e3a02003 	mov	r2, #3	; 0x3
   10900:	e1b0c1ac 	lsrs	ip, ip, #3
   10904:	12822003 	addne	r2, r2, #3	; 0x3
   10908:	e1b0c1ac 	lsrs	ip, ip, #3
   1090c:	12822003 	addne	r2, r2, #3	; 0x3
   10910:	e08221ac 	add	r2, r2, ip, lsr #3
   10914:	e2623020 	rsb	r3, r2, #32	; 0x20
   10918:	e1a0c310 	lsl	ip, r0, r3
   1091c:	e1a00230 	lsr	r0, r0, r2
   10920:	e1800311 	orr	r0, r0, r1, lsl r3
   10924:	e1a01231 	lsr	r1, r1, r2
   10928:	e0844002 	add	r4, r4, r2
   1092c:	eaffff4f 	b	10670 <__adddf3+0xec>

00010930 <__aeabi_dmul>:
   10930:	e92d4070 	push	{r4, r5, r6, lr}
   10934:	e3a0c0ff 	mov	ip, #255	; 0xff
   10938:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   1093c:	e01c4a21 	ands	r4, ip, r1, lsr #20
   10940:	101c5a23 	andsne	r5, ip, r3, lsr #20
   10944:	1134000c 	teqne	r4, ip
   10948:	1135000c 	teqne	r5, ip
   1094c:	0b00006f 	bleq	10b10 <__aeabi_dmul+0x1e0>
   10950:	e0844005 	add	r4, r4, r5
   10954:	e0216003 	eor	r6, r1, r3
   10958:	e1c11a8c 	bic	r1, r1, ip, lsl #21
   1095c:	e1c33a8c 	bic	r3, r3, ip, lsl #21
   10960:	e1905601 	orrs	r5, r0, r1, lsl #12
   10964:	11925603 	orrsne	r5, r2, r3, lsl #12
   10968:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   1096c:	e3833601 	orr	r3, r3, #1048576	; 0x100000
   10970:	0a00001c 	beq	109e8 <__aeabi_dmul+0xb8>
   10974:	e08ec290 	umull	ip, lr, r0, r2
   10978:	e3a05000 	mov	r5, #0	; 0x0
   1097c:	e0a5e291 	umlal	lr, r5, r1, r2
   10980:	e2062102 	and	r2, r6, #-2147483648	; 0x80000000
   10984:	e0a5e390 	umlal	lr, r5, r0, r3
   10988:	e3a06000 	mov	r6, #0	; 0x0
   1098c:	e0a65391 	umlal	r5, r6, r1, r3
   10990:	e33c0000 	teq	ip, #0	; 0x0
   10994:	138ee001 	orrne	lr, lr, #1	; 0x1
   10998:	e24440ff 	sub	r4, r4, #255	; 0xff
   1099c:	e3560c02 	cmp	r6, #512	; 0x200
   109a0:	e2c44c03 	sbc	r4, r4, #768	; 0x300
   109a4:	2a000002 	bcs	109b4 <__aeabi_dmul+0x84>
   109a8:	e1b0e08e 	lsls	lr, lr, #1
   109ac:	e0b55005 	adcs	r5, r5, r5
   109b0:	e0a66006 	adc	r6, r6, r6
   109b4:	e1821586 	orr	r1, r2, r6, lsl #11
   109b8:	e1811aa5 	orr	r1, r1, r5, lsr #21
   109bc:	e1a00585 	lsl	r0, r5, #11
   109c0:	e1800aae 	orr	r0, r0, lr, lsr #21
   109c4:	e1a0e58e 	lsl	lr, lr, #11
   109c8:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   109cc:	835c0c07 	cmphi	ip, #1792	; 0x700
   109d0:	8a00000f 	bhi	10a14 <__aeabi_dmul+0xe4>
   109d4:	e35e0102 	cmp	lr, #-2147483648	; 0x80000000
   109d8:	01b0e0a0 	lsrseq	lr, r0, #1
   109dc:	e2b00000 	adcs	r0, r0, #0	; 0x0
   109e0:	e0a11a04 	adc	r1, r1, r4, lsl #20
   109e4:	e8bd8070 	pop	{r4, r5, r6, pc}
   109e8:	e2066102 	and	r6, r6, #-2147483648	; 0x80000000
   109ec:	e1861001 	orr	r1, r6, r1
   109f0:	e1800002 	orr	r0, r0, r2
   109f4:	e0211003 	eor	r1, r1, r3
   109f8:	e05440ac 	subs	r4, r4, ip, lsr #1
   109fc:	c074500c 	rsbsgt	r5, r4, ip
   10a00:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   10a04:	c8bd8070 	popgt	{r4, r5, r6, pc}
   10a08:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   10a0c:	e3a0e000 	mov	lr, #0	; 0x0
   10a10:	e2544001 	subs	r4, r4, #1	; 0x1
   10a14:	ca000058 	bgt	10b7c <__aeabi_dmul+0x24c>
   10a18:	e3740036 	cmn	r4, #54	; 0x36
   10a1c:	d3a00000 	movle	r0, #0	; 0x0
   10a20:	d2011102 	andle	r1, r1, #-2147483648	; 0x80000000
   10a24:	d8bd8070 	pople	{r4, r5, r6, pc}
   10a28:	e2644000 	rsb	r4, r4, #0	; 0x0
   10a2c:	e2544020 	subs	r4, r4, #32	; 0x20
   10a30:	aa000018 	bge	10a98 <__aeabi_dmul+0x168>
   10a34:	e294400c 	adds	r4, r4, #12	; 0xc
   10a38:	ca00000b 	bgt	10a6c <__aeabi_dmul+0x13c>
   10a3c:	e2844014 	add	r4, r4, #20	; 0x14
   10a40:	e2645020 	rsb	r5, r4, #32	; 0x20
   10a44:	e1a03510 	lsl	r3, r0, r5
   10a48:	e1a00430 	lsr	r0, r0, r4
   10a4c:	e1800511 	orr	r0, r0, r1, lsl r5
   10a50:	e2012102 	and	r2, r1, #-2147483648	; 0x80000000
   10a54:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   10a58:	e0900fa3 	adds	r0, r0, r3, lsr #31
   10a5c:	e0a21431 	adc	r1, r2, r1, lsr r4
   10a60:	e19ee083 	orrs	lr, lr, r3, lsl #1
   10a64:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   10a68:	e8bd8070 	pop	{r4, r5, r6, pc}
   10a6c:	e264400c 	rsb	r4, r4, #12	; 0xc
   10a70:	e2645020 	rsb	r5, r4, #32	; 0x20
   10a74:	e1a03410 	lsl	r3, r0, r4
   10a78:	e1a00530 	lsr	r0, r0, r5
   10a7c:	e1800411 	orr	r0, r0, r1, lsl r4
   10a80:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10a84:	e0900fa3 	adds	r0, r0, r3, lsr #31
   10a88:	e2a11000 	adc	r1, r1, #0	; 0x0
   10a8c:	e19ee083 	orrs	lr, lr, r3, lsl #1
   10a90:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   10a94:	e8bd8070 	pop	{r4, r5, r6, pc}
   10a98:	e2645020 	rsb	r5, r4, #32	; 0x20
   10a9c:	e18ee510 	orr	lr, lr, r0, lsl r5
   10aa0:	e1a03430 	lsr	r3, r0, r4
   10aa4:	e1833511 	orr	r3, r3, r1, lsl r5
   10aa8:	e1a00431 	lsr	r0, r1, r4
   10aac:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10ab0:	e1c00431 	bic	r0, r0, r1, lsr r4
   10ab4:	e0800fa3 	add	r0, r0, r3, lsr #31
   10ab8:	e19ee083 	orrs	lr, lr, r3, lsl #1
   10abc:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   10ac0:	e8bd8070 	pop	{r4, r5, r6, pc}
   10ac4:	e3340000 	teq	r4, #0	; 0x0
   10ac8:	1a000008 	bne	10af0 <__aeabi_dmul+0x1c0>
   10acc:	e2016102 	and	r6, r1, #-2147483648	; 0x80000000
   10ad0:	e1b00080 	lsls	r0, r0, #1
   10ad4:	e0a11001 	adc	r1, r1, r1
   10ad8:	e3110601 	tst	r1, #1048576	; 0x100000
   10adc:	02444001 	subeq	r4, r4, #1	; 0x1
   10ae0:	0afffffa 	beq	10ad0 <__aeabi_dmul+0x1a0>
   10ae4:	e1811006 	orr	r1, r1, r6
   10ae8:	e3350000 	teq	r5, #0	; 0x0
   10aec:	11a0f00e 	movne	pc, lr
   10af0:	e2036102 	and	r6, r3, #-2147483648	; 0x80000000
   10af4:	e1b02082 	lsls	r2, r2, #1
   10af8:	e0a33003 	adc	r3, r3, r3
   10afc:	e3130601 	tst	r3, #1048576	; 0x100000
   10b00:	02455001 	subeq	r5, r5, #1	; 0x1
   10b04:	0afffffa 	beq	10af4 <__aeabi_dmul+0x1c4>
   10b08:	e1833006 	orr	r3, r3, r6
   10b0c:	e1a0f00e 	mov	pc, lr
   10b10:	e134000c 	teq	r4, ip
   10b14:	e00c5a23 	and	r5, ip, r3, lsr #20
   10b18:	1135000c 	teqne	r5, ip
   10b1c:	0a000006 	beq	10b3c <__aeabi_dmul+0x20c>
   10b20:	e1906081 	orrs	r6, r0, r1, lsl #1
   10b24:	11926083 	orrsne	r6, r2, r3, lsl #1
   10b28:	1affffe5 	bne	10ac4 <__aeabi_dmul+0x194>
   10b2c:	e0211003 	eor	r1, r1, r3
   10b30:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10b34:	e3a00000 	mov	r0, #0	; 0x0
   10b38:	e8bd8070 	pop	{r4, r5, r6, pc}
   10b3c:	e1906081 	orrs	r6, r0, r1, lsl #1
   10b40:	01a00002 	moveq	r0, r2
   10b44:	01a01003 	moveq	r1, r3
   10b48:	11926083 	orrsne	r6, r2, r3, lsl #1
   10b4c:	0a00000f 	beq	10b90 <__aeabi_dmul+0x260>
   10b50:	e134000c 	teq	r4, ip
   10b54:	1a000001 	bne	10b60 <__aeabi_dmul+0x230>
   10b58:	e1906601 	orrs	r6, r0, r1, lsl #12
   10b5c:	1a00000b 	bne	10b90 <__aeabi_dmul+0x260>
   10b60:	e135000c 	teq	r5, ip
   10b64:	1a000003 	bne	10b78 <__aeabi_dmul+0x248>
   10b68:	e1926603 	orrs	r6, r2, r3, lsl #12
   10b6c:	11a00002 	movne	r0, r2
   10b70:	11a01003 	movne	r1, r3
   10b74:	1a000005 	bne	10b90 <__aeabi_dmul+0x260>
   10b78:	e0211003 	eor	r1, r1, r3
   10b7c:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   10b80:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   10b84:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   10b88:	e3a00000 	mov	r0, #0	; 0x0
   10b8c:	e8bd8070 	pop	{r4, r5, r6, pc}
   10b90:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   10b94:	e381173e 	orr	r1, r1, #16252928	; 0xf80000
   10b98:	e8bd8070 	pop	{r4, r5, r6, pc}

00010b9c <__aeabi_ddiv>:
   10b9c:	e92d4070 	push	{r4, r5, r6, lr}
   10ba0:	e3a0c0ff 	mov	ip, #255	; 0xff
   10ba4:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   10ba8:	e01c4a21 	ands	r4, ip, r1, lsr #20
   10bac:	101c5a23 	andsne	r5, ip, r3, lsr #20
   10bb0:	1134000c 	teqne	r4, ip
   10bb4:	1135000c 	teqne	r5, ip
   10bb8:	0b00005c 	bleq	10d30 <__aeabi_ddiv+0x194>
   10bbc:	e0444005 	sub	r4, r4, r5
   10bc0:	e021e003 	eor	lr, r1, r3
   10bc4:	e1925603 	orrs	r5, r2, r3, lsl #12
   10bc8:	e1a01601 	lsl	r1, r1, #12
   10bcc:	0a00004b 	beq	10d00 <__aeabi_ddiv+0x164>
   10bd0:	e1a03603 	lsl	r3, r3, #12
   10bd4:	e3a05201 	mov	r5, #268435456	; 0x10000000
   10bd8:	e1853223 	orr	r3, r5, r3, lsr #4
   10bdc:	e1833c22 	orr	r3, r3, r2, lsr #24
   10be0:	e1a02402 	lsl	r2, r2, #8
   10be4:	e1855221 	orr	r5, r5, r1, lsr #4
   10be8:	e1855c20 	orr	r5, r5, r0, lsr #24
   10bec:	e1a06400 	lsl	r6, r0, #8
   10bf0:	e20e1102 	and	r1, lr, #-2147483648	; 0x80000000
   10bf4:	e1550003 	cmp	r5, r3
   10bf8:	01560002 	cmpeq	r6, r2
   10bfc:	e2a440fd 	adc	r4, r4, #253	; 0xfd
   10c00:	e2844c03 	add	r4, r4, #768	; 0x300
   10c04:	2a000001 	bcs	10c10 <__aeabi_ddiv+0x74>
   10c08:	e1b030a3 	lsrs	r3, r3, #1
   10c0c:	e1a02062 	rrx	r2, r2
   10c10:	e0566002 	subs	r6, r6, r2
   10c14:	e0c55003 	sbc	r5, r5, r3
   10c18:	e1b030a3 	lsrs	r3, r3, #1
   10c1c:	e1a02062 	rrx	r2, r2
   10c20:	e3a00601 	mov	r0, #1048576	; 0x100000
   10c24:	e3a0c702 	mov	ip, #524288	; 0x80000
   10c28:	e056e002 	subs	lr, r6, r2
   10c2c:	e0d5e003 	sbcs	lr, r5, r3
   10c30:	20466002 	subcs	r6, r6, r2
   10c34:	21a0500e 	movcs	r5, lr
   10c38:	2180000c 	orrcs	r0, r0, ip
   10c3c:	e1b030a3 	lsrs	r3, r3, #1
   10c40:	e1a02062 	rrx	r2, r2
   10c44:	e056e002 	subs	lr, r6, r2
   10c48:	e0d5e003 	sbcs	lr, r5, r3
   10c4c:	20466002 	subcs	r6, r6, r2
   10c50:	21a0500e 	movcs	r5, lr
   10c54:	218000ac 	orrcs	r0, r0, ip, lsr #1
   10c58:	e1b030a3 	lsrs	r3, r3, #1
   10c5c:	e1a02062 	rrx	r2, r2
   10c60:	e056e002 	subs	lr, r6, r2
   10c64:	e0d5e003 	sbcs	lr, r5, r3
   10c68:	20466002 	subcs	r6, r6, r2
   10c6c:	21a0500e 	movcs	r5, lr
   10c70:	2180012c 	orrcs	r0, r0, ip, lsr #2
   10c74:	e1b030a3 	lsrs	r3, r3, #1
   10c78:	e1a02062 	rrx	r2, r2
   10c7c:	e056e002 	subs	lr, r6, r2
   10c80:	e0d5e003 	sbcs	lr, r5, r3
   10c84:	20466002 	subcs	r6, r6, r2
   10c88:	21a0500e 	movcs	r5, lr
   10c8c:	218001ac 	orrcs	r0, r0, ip, lsr #3
   10c90:	e195e006 	orrs	lr, r5, r6
   10c94:	0a00000d 	beq	10cd0 <__aeabi_ddiv+0x134>
   10c98:	e1a05205 	lsl	r5, r5, #4
   10c9c:	e1855e26 	orr	r5, r5, r6, lsr #28
   10ca0:	e1a06206 	lsl	r6, r6, #4
   10ca4:	e1a03183 	lsl	r3, r3, #3
   10ca8:	e1833ea2 	orr	r3, r3, r2, lsr #29
   10cac:	e1a02182 	lsl	r2, r2, #3
   10cb0:	e1b0c22c 	lsrs	ip, ip, #4
   10cb4:	1affffdb 	bne	10c28 <__aeabi_ddiv+0x8c>
   10cb8:	e3110601 	tst	r1, #1048576	; 0x100000
   10cbc:	1a000006 	bne	10cdc <__aeabi_ddiv+0x140>
   10cc0:	e1811000 	orr	r1, r1, r0
   10cc4:	e3a00000 	mov	r0, #0	; 0x0
   10cc8:	e3a0c102 	mov	ip, #-2147483648	; 0x80000000
   10ccc:	eaffffd5 	b	10c28 <__aeabi_ddiv+0x8c>
   10cd0:	e3110601 	tst	r1, #1048576	; 0x100000
   10cd4:	01811000 	orreq	r1, r1, r0
   10cd8:	03a00000 	moveq	r0, #0	; 0x0
   10cdc:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   10ce0:	835c0c07 	cmphi	ip, #1792	; 0x700
   10ce4:	8affff4a 	bhi	10a14 <__aeabi_dmul+0xe4>
   10ce8:	e055c003 	subs	ip, r5, r3
   10cec:	0056c002 	subseq	ip, r6, r2
   10cf0:	01b0c0a0 	lsrseq	ip, r0, #1
   10cf4:	e2b00000 	adcs	r0, r0, #0	; 0x0
   10cf8:	e0a11a04 	adc	r1, r1, r4, lsl #20
   10cfc:	e8bd8070 	pop	{r4, r5, r6, pc}
   10d00:	e20ee102 	and	lr, lr, #-2147483648	; 0x80000000
   10d04:	e18e1621 	orr	r1, lr, r1, lsr #12
   10d08:	e09440ac 	adds	r4, r4, ip, lsr #1
   10d0c:	c074500c 	rsbsgt	r5, r4, ip
   10d10:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   10d14:	c8bd8070 	popgt	{r4, r5, r6, pc}
   10d18:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   10d1c:	e3a0e000 	mov	lr, #0	; 0x0
   10d20:	e2544001 	subs	r4, r4, #1	; 0x1
   10d24:	eaffff3a 	b	10a14 <__aeabi_dmul+0xe4>
   10d28:	e185e006 	orr	lr, r5, r6
   10d2c:	eaffff38 	b	10a14 <__aeabi_dmul+0xe4>
   10d30:	e00c5a23 	and	r5, ip, r3, lsr #20
   10d34:	e134000c 	teq	r4, ip
   10d38:	0135000c 	teqeq	r5, ip
   10d3c:	0affff93 	beq	10b90 <__aeabi_dmul+0x260>
   10d40:	e134000c 	teq	r4, ip
   10d44:	1a000006 	bne	10d64 <__aeabi_ddiv+0x1c8>
   10d48:	e1904601 	orrs	r4, r0, r1, lsl #12
   10d4c:	1affff8f 	bne	10b90 <__aeabi_dmul+0x260>
   10d50:	e135000c 	teq	r5, ip
   10d54:	1affff87 	bne	10b78 <__aeabi_dmul+0x248>
   10d58:	e1a00002 	mov	r0, r2
   10d5c:	e1a01003 	mov	r1, r3
   10d60:	eaffff8a 	b	10b90 <__aeabi_dmul+0x260>
   10d64:	e135000c 	teq	r5, ip
   10d68:	1a000004 	bne	10d80 <__aeabi_ddiv+0x1e4>
   10d6c:	e1925603 	orrs	r5, r2, r3, lsl #12
   10d70:	0affff6d 	beq	10b2c <__aeabi_dmul+0x1fc>
   10d74:	e1a00002 	mov	r0, r2
   10d78:	e1a01003 	mov	r1, r3
   10d7c:	eaffff83 	b	10b90 <__aeabi_dmul+0x260>
   10d80:	e1906081 	orrs	r6, r0, r1, lsl #1
   10d84:	11926083 	orrsne	r6, r2, r3, lsl #1
   10d88:	1affff4d 	bne	10ac4 <__aeabi_dmul+0x194>
   10d8c:	e1904081 	orrs	r4, r0, r1, lsl #1
   10d90:	1affff78 	bne	10b78 <__aeabi_dmul+0x248>
   10d94:	e1925083 	orrs	r5, r2, r3, lsl #1
   10d98:	1affff63 	bne	10b2c <__aeabi_dmul+0x1fc>
   10d9c:	eaffff7b 	b	10b90 <__aeabi_dmul+0x260>

00010da0 <__aeabi_d2iz>:
   10da0:	e1a02081 	lsl	r2, r1, #1
   10da4:	e2922602 	adds	r2, r2, #2097152	; 0x200000
   10da8:	2a00000c 	bcs	10de0 <__aeabi_d2iz+0x40>
   10dac:	5a000009 	bpl	10dd8 <__aeabi_d2iz+0x38>
   10db0:	e3e03e3e 	mvn	r3, #992	; 0x3e0
   10db4:	e0532ac2 	subs	r2, r3, r2, asr #21
   10db8:	9a00000a 	bls	10de8 <__aeabi_d2iz+0x48>
   10dbc:	e1a03581 	lsl	r3, r1, #11
   10dc0:	e3833102 	orr	r3, r3, #-2147483648	; 0x80000000
   10dc4:	e1833aa0 	orr	r3, r3, r0, lsr #21
   10dc8:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   10dcc:	e1a00233 	lsr	r0, r3, r2
   10dd0:	12600000 	rsbne	r0, r0, #0	; 0x0
   10dd4:	e12fff1e 	bx	lr
   10dd8:	e3a00000 	mov	r0, #0	; 0x0
   10ddc:	e12fff1e 	bx	lr
   10de0:	e1900601 	orrs	r0, r0, r1, lsl #12
   10de4:	1a000002 	bne	10df4 <__aeabi_d2iz+0x54>
   10de8:	e2110102 	ands	r0, r1, #-2147483648	; 0x80000000
   10dec:	03e00102 	mvneq	r0, #-2147483648	; 0x80000000
   10df0:	e12fff1e 	bx	lr
   10df4:	e3a00000 	mov	r0, #0	; 0x0
   10df8:	e12fff1e 	bx	lr

00010dfc <__div0>:
   10dfc:	e92d4002 	push	{r1, lr}
   10e00:	e3a00008 	mov	r0, #8	; 0x8
   10e04:	ebfffcb1 	bl	100d0 <raise>
   10e08:	e8bd8002 	pop	{r1, pc}
