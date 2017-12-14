
/home/user/wellie/infotm-irom/out/vmirom:     file format elf32-littlearm


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
      44:	3c00d120 	.word	0x3c00d120

00000048 <_bss_end>:
      48:	3c00ddb8 	.word	0x3c00ddb8

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
      78:	eb00041a 	bl	10e8 <chip_test>
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
      9c:	00000580 	.word	0x00000580

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
     15c:	e58d0020 	str	r0, [sp, #32]
     160:	e3a01001 	mov	r1, #1	; 0x1
     164:	e1a00004 	mov	r0, r4
     168:	e58d2008 	str	r2, [sp, #8]
     16c:	eb0018da 	bl	64dc <vs_assign_by_id>
     170:	e3500000 	cmp	r0, #0	; 0x0
     174:	e58d0024 	str	r0, [sp, #36]
     178:	0a000008 	beq	1a0 <bl_load_isi+0x50>
     17c:	e59f31d4 	ldr	r3, [pc, #468]	; 358 <bl_load_isi+0x208>
     180:	e1a00004 	mov	r0, r4
     184:	e5934000 	ldr	r4, [r3]
     188:	eb0017dd 	bl	6104 <vs_device_name>
     18c:	e59d2024 	ldr	r2, [sp, #36]
     190:	e1a01000 	mov	r1, r0
     194:	e59f01c0 	ldr	r0, [pc, #448]	; 35c <bl_load_isi+0x20c>
     198:	e12fff34 	blx	r4
     19c:	ea00006a 	b	34c <bl_load_isi+0x1fc>
     1a0:	e1a00004 	mov	r0, r4
     1a4:	e59f11b4 	ldr	r1, [pc, #436]	; 360 <bl_load_isi+0x210>
     1a8:	eb00180c 	bl	61e0 <vs_is_device>
     1ac:	e3500000 	cmp	r0, #0	; 0x0
     1b0:	03a09b01 	moveq	r9, #1024	; 0x400
     1b4:	0a000006 	beq	1d4 <bl_load_isi+0x84>
     1b8:	e59d0024 	ldr	r0, [sp, #36]
     1bc:	eb00275c 	bl	9f34 <nand_get_config>
     1c0:	e3500000 	cmp	r0, #0	; 0x0
     1c4:	03e00033 	mvneq	r0, #51	; 0x33
     1c8:	058d0024 	streq	r0, [sp, #36]
     1cc:	0a00005e 	beq	34c <bl_load_isi+0x1fc>
     1d0:	e5909014 	ldr	r9, [r0, #20]
     1d4:	e1a02009 	mov	r2, r9
     1d8:	e1a03fc2 	asr	r3, r2, #31
     1dc:	e1cd21f8 	strd	r2, [sp, #24]
     1e0:	e3a06000 	mov	r6, #0	; 0x0
     1e4:	e59da008 	ldr	sl, [sp, #8]
     1e8:	e59f0174 	ldr	r0, [pc, #372]	; 364 <bl_load_isi+0x214>
     1ec:	e89a000c 	ldm	sl, {r2, r3}
     1f0:	e3a01000 	mov	r1, #0	; 0x0
     1f4:	e0922000 	adds	r2, r2, r0
     1f8:	e3e0b000 	mvn	fp, #0	; 0x0
     1fc:	e0a33001 	adc	r3, r3, r1
     200:	e59fa160 	ldr	sl, [pc, #352]	; 368 <bl_load_isi+0x218>
     204:	e003300b 	and	r3, r3, fp
     208:	e59db008 	ldr	fp, [sp, #8]
     20c:	e002200a 	and	r2, r2, sl
     210:	e88b000c 	stm	fp, {r2, r3}
     214:	e3a04000 	mov	r4, #0	; 0x0
     218:	e1a07004 	mov	r7, r4
     21c:	e1a08004 	mov	r8, r4
     220:	e58d400c 	str	r4, [sp, #12]
     224:	e1540009 	cmp	r4, r9
     228:	1a000013 	bne	27c <bl_load_isi+0x12c>
     22c:	e59d0020 	ldr	r0, [sp, #32]
     230:	eb00062f 	bl	1af4 <isi_check_header>
     234:	e3500000 	cmp	r0, #0	; 0x0
     238:	e1a02006 	mov	r2, r6
     23c:	e59f0128 	ldr	r0, [pc, #296]	; 36c <bl_load_isi+0x21c>
     240:	1a000007 	bne	264 <bl_load_isi+0x114>
     244:	e59d1020 	ldr	r1, [sp, #32]
     248:	e591300c 	ldr	r3, [r1, #12]
     24c:	e2837c02 	add	r7, r3, #512	; 0x200
     250:	e1a01007 	mov	r1, r7
     254:	e59f30fc 	ldr	r3, [pc, #252]	; 358 <bl_load_isi+0x208>
     258:	e1a0e00f 	mov	lr, pc
     25c:	e593f000 	ldr	pc, [r3]
     260:	ea000005 	b	27c <bl_load_isi+0x12c>
     264:	e59f0104 	ldr	r0, [pc, #260]	; 370 <bl_load_isi+0x220>
     268:	e1a01006 	mov	r1, r6
     26c:	e59fa0e4 	ldr	sl, [pc, #228]	; 358 <bl_load_isi+0x208>
     270:	e1a0e00f 	mov	lr, pc
     274:	e59af000 	ldr	pc, [sl]
     278:	ea00002e 	b	338 <bl_load_isi+0x1e8>
     27c:	e2575000 	subs	r5, r7, #0	; 0x0
     280:	13a05001 	movne	r5, #1	; 0x1
     284:	e1540007 	cmp	r4, r7
     288:	e59db020 	ldr	fp, [sp, #32]
     28c:	b3a05000 	movlt	r5, #0	; 0x0
     290:	e3550000 	cmp	r5, #0	; 0x0
     294:	e08b0004 	add	r0, fp, r4
     298:	0a000009 	beq	2c4 <bl_load_isi+0x174>
     29c:	e1a0000b 	mov	r0, fp
     2a0:	eb00066c 	bl	1c58 <isi_check_data>
     2a4:	e3500000 	cmp	r0, #0	; 0x0
     2a8:	0a000027 	beq	34c <bl_load_isi+0x1fc>
     2ac:	e59f00c0 	ldr	r0, [pc, #192]	; 374 <bl_load_isi+0x224>
     2b0:	e1a01006 	mov	r1, r6
     2b4:	e59f209c 	ldr	r2, [pc, #156]	; 358 <bl_load_isi+0x208>
     2b8:	e1a0e00f 	mov	lr, pc
     2bc:	e592f000 	ldr	pc, [r2]
     2c0:	ea00001c 	b	338 <bl_load_isi+0x1e8>
     2c4:	e3580004 	cmp	r8, #4	; 0x4
     2c8:	e2888001 	add	r8, r8, #1	; 0x1
     2cc:	da000005 	ble	2e8 <bl_load_isi+0x198>
     2d0:	e59f009c 	ldr	r0, [pc, #156]	; 374 <bl_load_isi+0x224>
     2d4:	e1a01006 	mov	r1, r6
     2d8:	e59f3078 	ldr	r3, [pc, #120]	; 358 <bl_load_isi+0x208>
     2dc:	e1a0e00f 	mov	lr, pc
     2e0:	e593f000 	ldr	pc, [r3]
     2e4:	ea000013 	b	338 <bl_load_isi+0x1e8>
     2e8:	e59da008 	ldr	sl, [sp, #8]
     2ec:	e89a000c 	ldm	sl, {r2, r3}
     2f0:	e58d9000 	str	r9, [sp]
     2f4:	e58d5004 	str	r5, [sp, #4]
     2f8:	eb0017d1 	bl	6244 <vs_read>
     2fc:	e1cda1d8 	ldrd	sl, [sp, #24]
     300:	e59d3008 	ldr	r3, [sp, #8]
     304:	e893000c 	ldm	r3, {r2, r3}
     308:	e092200a 	adds	r2, r2, sl
     30c:	e0a3300b 	adc	r3, r3, fp
     310:	e1500009 	cmp	r0, r9
     314:	e59d000c 	ldr	r0, [sp, #12]
     318:	e59db008 	ldr	fp, [sp, #8]
     31c:	e2800001 	add	r0, r0, #1	; 0x1
     320:	00844009 	addeq	r4, r4, r9
     324:	01a08005 	moveq	r8, r5
     328:	e3500080 	cmp	r0, #128	; 0x80
     32c:	e88b000c 	stm	fp, {r2, r3}
     330:	e58d000c 	str	r0, [sp, #12]
     334:	1affffba 	bne	224 <bl_load_isi+0xd4>
     338:	e2866001 	add	r6, r6, #1	; 0x1
     33c:	e3560010 	cmp	r6, #16	; 0x10
     340:	1affffa7 	bne	1e4 <bl_load_isi+0x94>
     344:	e3e01000 	mvn	r1, #0	; 0x0
     348:	e58d1024 	str	r1, [sp, #36]
     34c:	e59d0024 	ldr	r0, [sp, #36]
     350:	e28dd02c 	add	sp, sp, #44	; 0x2c
     354:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     358:	3c00d6cc 	.word	0x3c00d6cc
     35c:	00016103 	.word	0x00016103
     360:	00016127 	.word	0x00016127
     364:	0001ffff 	.word	0x0001ffff
     368:	fffe0000 	.word	0xfffe0000
     36c:	0001612b 	.word	0x0001612b
     370:	00016142 	.word	0x00016142
     374:	00016154 	.word	0x00016154

00000378 <try_get_bl>:
     378:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     37c:	e3a02602 	mov	r2, #2097152	; 0x200000
     380:	e24dd00c 	sub	sp, sp, #12	; 0xc
     384:	e3a03000 	mov	r3, #0	; 0x0
     388:	e88d000c 	stm	sp, {r2, r3}
     38c:	e1a07001 	mov	r7, r1
     390:	e1a06000 	mov	r6, r0
     394:	eb0000a9 	bl	640 <boot_device>
     398:	e1a04000 	mov	r4, r0
     39c:	eb001711 	bl	5fe8 <vs_device_bootable>
     3a0:	e2505000 	subs	r5, r0, #0	; 0x0
     3a4:	1a000008 	bne	3cc <try_get_bl+0x54>
     3a8:	e59f308c 	ldr	r3, [pc, #140]	; 43c <try_get_bl+0xc4>
     3ac:	e1a00004 	mov	r0, r4
     3b0:	e5934000 	ldr	r4, [r3]
     3b4:	eb001752 	bl	6104 <vs_device_name>
     3b8:	e1a01000 	mov	r1, r0
     3bc:	e59f007c 	ldr	r0, [pc, #124]	; 440 <try_get_bl+0xc8>
     3c0:	e12fff34 	blx	r4
     3c4:	e1a00005 	mov	r0, r5
     3c8:	ea000019 	b	434 <try_get_bl+0xbc>
     3cc:	e1a00004 	mov	r0, r4
     3d0:	eb001739 	bl	60bc <vs_device_boot0>
     3d4:	e3500000 	cmp	r0, #0	; 0x0
     3d8:	13a02000 	movne	r2, #0	; 0x0
     3dc:	13a03000 	movne	r3, #0	; 0x0
     3e0:	188d000c 	stmne	sp, {r2, r3}
     3e4:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
     3e8:	e1a01004 	mov	r1, r4
     3ec:	e1a0200d 	mov	r2, sp
     3f0:	e3a03000 	mov	r3, #0	; 0x0
     3f4:	ebffff55 	bl	150 <bl_load_isi>
     3f8:	e3500000 	cmp	r0, #0	; 0x0
     3fc:	13a00000 	movne	r0, #0	; 0x0
     400:	1a00000b 	bne	434 <try_get_bl+0xbc>
     404:	e3a0330f 	mov	r3, #1006632960	; 0x3c000000
     408:	e5932008 	ldr	r2, [r3, #8]
     40c:	e3c22102 	bic	r2, r2, #-2147483648	; 0x80000000
     410:	e5862000 	str	r2, [r6]
     414:	e5874000 	str	r4, [r7]
     418:	e5933008 	ldr	r3, [r3, #8]
     41c:	e3530000 	cmp	r3, #0	; 0x0
     420:	aa000002 	bge	430 <try_get_bl+0xb8>
     424:	e3a00001 	mov	r0, #1	; 0x1
     428:	e59f1014 	ldr	r1, [pc, #20]	; 444 <try_get_bl+0xcc>
     42c:	eb001bf3 	bl	7400 <cdbg_verify_burn_enable>
     430:	e3a00001 	mov	r0, #1	; 0x1
     434:	e28dd00c 	add	sp, sp, #12	; 0xc
     438:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     43c:	3c00d6cc 	.word	0x3c00d6cc
     440:	00016166 	.word	0x00016166
     444:	3c000100 	.word	0x3c000100

00000448 <try_boot>:
     448:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
     44c:	e3a03000 	mov	r3, #0	; 0x0
     450:	e24dd008 	sub	sp, sp, #8	; 0x8
     454:	e1a07000 	mov	r7, r0
     458:	e1a00003 	mov	r0, r3
     45c:	e58d3004 	str	r3, [sp, #4]
     460:	e58d3000 	str	r3, [sp]
     464:	e1a04003 	mov	r4, r3
     468:	eb00005b 	bl	5dc <set_xom>
     46c:	eb000060 	bl	5f4 <boot_info>
     470:	e28d8004 	add	r8, sp, #4	; 0x4
     474:	e1a05000 	mov	r5, r0
     478:	ea000008 	b	4a0 <try_boot+0x58>
     47c:	eb000127 	bl	920 <gtc_time>
     480:	e1a00004 	mov	r0, r4
     484:	eb000054 	bl	5dc <set_xom>
     488:	e1a00008 	mov	r0, r8
     48c:	e1a0100d 	mov	r1, sp
     490:	ebffffb8 	bl	378 <try_get_bl>
     494:	e3500000 	cmp	r0, #0	; 0x0
     498:	13a04001 	movne	r4, #1	; 0x1
     49c:	1a000003 	bne	4b0 <try_boot+0x68>
     4a0:	e1540005 	cmp	r4, r5
     4a4:	e2844001 	add	r4, r4, #1	; 0x1
     4a8:	bafffff3 	blt	47c <try_boot+0x34>
     4ac:	e3a04000 	mov	r4, #0	; 0x0
     4b0:	eb00011a 	bl	920 <gtc_time>
     4b4:	e3a00001 	mov	r0, #1	; 0x1
     4b8:	eb00012e 	bl	978 <ecfg_check_flag>
     4bc:	e3500000 	cmp	r0, #0	; 0x0
     4c0:	1a00000c 	bne	4f8 <try_boot+0xb0>
     4c4:	e3570000 	cmp	r7, #0	; 0x0
     4c8:	1a00000a 	bne	4f8 <try_boot+0xb0>
     4cc:	eb0000ca 	bl	7fc <get_reset_stats>
     4d0:	e3500000 	cmp	r0, #0	; 0x0
     4d4:	13500002 	cmpne	r0, #2	; 0x2
     4d8:	1a000006 	bne	4f8 <try_boot+0xb0>
     4dc:	e3500002 	cmp	r0, #2	; 0x2
     4e0:	1a000001 	bne	4ec <try_boot+0xa4>
     4e4:	e28d0004 	add	r0, sp, #4	; 0x4
     4e8:	eb0000d0 	bl	830 <update_mask_on_reset>
     4ec:	e59d0004 	ldr	r0, [sp, #4]
     4f0:	eb001e41 	bl	7dfc <burn>
     4f4:	eb000109 	bl	920 <gtc_time>
     4f8:	e3540000 	cmp	r4, #0	; 0x0
     4fc:	0a000004 	beq	514 <try_boot+0xcc>
     500:	e3a0330f 	mov	r3, #1006632960	; 0x3c000000
     504:	e5930014 	ldr	r0, [r3, #20]
     508:	e59d1000 	ldr	r1, [sp]
     50c:	eb001bc6 	bl	742c <cdbg_jump>
     510:	eafffffe 	b	510 <try_boot+0xc8>
     514:	eb001bf6 	bl	74f4 <cdbg_shell>
     518:	eafffffe 	b	518 <try_boot+0xd0>

0000051c <init_data_section>:
     51c:	e59f1010 	ldr	r1, [pc, #16]	; 534 <init_data_section+0x18>
     520:	e59f0010 	ldr	r0, [pc, #16]	; 538 <init_data_section+0x1c>
     524:	e1510000 	cmp	r1, r0
     528:	012fff1e 	bxeq	lr
     52c:	e59f2008 	ldr	r2, [pc, #8]	; 53c <init_data_section+0x20>
     530:	ea000407 	b	1554 <memcpy>
     534:	00019254 	.word	0x00019254
     538:	3c00d000 	.word	0x3c00d000
     53c:	00000120 	.word	0x00000120

00000540 <initialization>:
     540:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     544:	e24dd004 	sub	sp, sp, #4	; 0x4
     548:	ebfffff3 	bl	51c <init_data_section>
     54c:	eb000027 	bl	5f0 <init_xom>
     550:	eb000098 	bl	7b8 <init_lowlevel>
     554:	eb000528 	bl	19fc <init_malloc>
     558:	eb003563 	bl	daec <init_timer>
     55c:	eb0000fc 	bl	954 <init_gtc>
     560:	eb0034f4 	bl	d938 <init_serial>
     564:	eb001f7f 	bl	8368 <init_mmc>
     568:	eb0026c6 	bl	a088 <init_nand>
     56c:	eb0000a1 	bl	7f8 <init_crypto>
     570:	eb000247 	bl	e94 <init_alternate>
     574:	e3a00000 	mov	r0, #0	; 0x0
     578:	e28dd004 	add	sp, sp, #4	; 0x4
     57c:	e8bd8000 	pop	{pc}

00000580 <main>:
     580:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     584:	e3a00000 	mov	r0, #0	; 0x0
     588:	e24dd004 	sub	sp, sp, #4	; 0x4
     58c:	eb001b7f 	bl	7390 <cdbg_log_toggle>
     590:	ebffffea 	bl	540 <initialization>
     594:	eb0000e1 	bl	920 <gtc_time>
     598:	e3a00001 	mov	r0, #1	; 0x1
     59c:	eb001b7b 	bl	7390 <cdbg_log_toggle>
     5a0:	e59f001c 	ldr	r0, [pc, #28]	; 5c4 <main+0x44>
     5a4:	e59f301c 	ldr	r3, [pc, #28]	; 5c8 <main+0x48>
     5a8:	e1a0e00f 	mov	lr, pc
     5ac:	e593f000 	ldr	pc, [r3]
     5b0:	e3a00000 	mov	r0, #0	; 0x0
     5b4:	eb001b75 	bl	7390 <cdbg_log_toggle>
     5b8:	e3a00000 	mov	r0, #0	; 0x0
     5bc:	ebffffa1 	bl	448 <try_boot>
     5c0:	eafffffe 	b	5c0 <main+0x40>
     5c4:	00016180 	.word	0x00016180
     5c8:	3c00d6cc 	.word	0x3c00d6cc

000005cc <get_xom>:
     5cc:	e59f3004 	ldr	r3, [pc, #4]	; 5d8 <get_xom+0xc>
     5d0:	e1d300b0 	ldrh	r0, [r3]
     5d4:	e12fff1e 	bx	lr
     5d8:	3c00d120 	.word	0x3c00d120

000005dc <set_xom>:
     5dc:	e59f3008 	ldr	r3, [pc, #8]	; 5ec <set_xom+0x10>
     5e0:	e200001f 	and	r0, r0, #31	; 0x1f
     5e4:	e1c300b0 	strh	r0, [r3]
     5e8:	e12fff1e 	bx	lr
     5ec:	3c00d120 	.word	0x3c00d120

000005f0 <init_xom>:
     5f0:	e12fff1e 	bx	lr

000005f4 <boot_info>:
     5f4:	e92d4010 	push	{r4, lr}
     5f8:	eb0000e7 	bl	99c <ecfg_paras>
     5fc:	e1a04000 	mov	r4, r0
     600:	e3a00b01 	mov	r0, #1024	; 0x400
     604:	eb0000db 	bl	978 <ecfg_check_flag>
     608:	e59f3028 	ldr	r3, [pc, #40]	; 638 <boot_info+0x44>
     60c:	e3500000 	cmp	r0, #0	; 0x0
     610:	11d330b0 	ldrhne	r3, [r3]
     614:	01d330b0 	ldrheq	r3, [r3]
     618:	1203301f 	andne	r3, r3, #31	; 0x1f
     61c:	0203301f 	andeq	r3, r3, #31	; 0x1f
     620:	159f2014 	ldrne	r2, [pc, #20]	; 63c <boot_info+0x48>
     624:	11a03083 	lslne	r3, r3, #1
     628:	00843083 	addeq	r3, r4, r3, lsl #1
     62c:	119200b3 	ldrhne	r0, [r2, r3]
     630:	01d302b0 	ldrheq	r0, [r3, #32]
     634:	e8bd8010 	pop	{r4, pc}
     638:	3c00d120 	.word	0x3c00d120
     63c:	00015d40 	.word	0x00015d40

00000640 <boot_device>:
     640:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     644:	e24dd004 	sub	sp, sp, #4	; 0x4
     648:	ebffffe9 	bl	5f4 <boot_info>
     64c:	e1a00620 	lsr	r0, r0, #12
     650:	e6ff0070 	uxth	r0, r0
     654:	e28dd004 	add	sp, sp, #4	; 0x4
     658:	e8bd8000 	pop	{pc}

0000065c <set_pll>:
     65c:	e3500003 	cmp	r0, #3	; 0x3
     660:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
     664:	e1a02001 	mov	r2, r1
     668:	88bd85f0 	pophi	{r4, r5, r6, r7, r8, sl, pc}
     66c:	e3a03018 	mov	r3, #24	; 0x18
     670:	e0030093 	mul	r3, r3, r0
     674:	e59f808c 	ldr	r8, [pc, #140]	; 708 <set_pll+0xac>
     678:	e59f108c 	ldr	r1, [pc, #140]	; 70c <set_pll+0xb0>
     67c:	e59f008c 	ldr	r0, [pc, #140]	; 710 <set_pll+0xb4>
     680:	e59fc08c 	ldr	ip, [pc, #140]	; 714 <set_pll+0xb8>
     684:	e59f408c 	ldr	r4, [pc, #140]	; 718 <set_pll+0xbc>
     688:	e59fa08c 	ldr	sl, [pc, #140]	; 71c <set_pll+0xc0>
     68c:	e0838008 	add	r8, r3, r8
     690:	e083c00c 	add	ip, r3, ip
     694:	e0831001 	add	r1, r3, r1
     698:	e3a07001 	mov	r7, #1	; 0x1
     69c:	e0830000 	add	r0, r3, r0
     6a0:	e1a06422 	lsr	r6, r2, #8
     6a4:	e0834004 	add	r4, r3, r4
     6a8:	e3a05000 	mov	r5, #0	; 0x0
     6ac:	e6ef2072 	uxtb	r2, r2
     6b0:	e083a00a 	add	sl, r3, sl
     6b4:	e3a03002 	mov	r3, #2	; 0x2
     6b8:	e5c87000 	strb	r7, [r8]
     6bc:	e5c12000 	strb	r2, [r1]
     6c0:	e5c06000 	strb	r6, [r0]
     6c4:	e5cc5000 	strb	r5, [ip]
     6c8:	e5c43000 	strb	r3, [r4]
     6cc:	e0833007 	add	r3, r3, r7
     6d0:	e5c83000 	strb	r3, [r8]
     6d4:	e1a02005 	mov	r2, r5
     6d8:	e5cc7000 	strb	r7, [ip]
     6dc:	e3011a0a 	movw	r1, #6666	; 0x1a0a
     6e0:	ea000001 	b	6ec <set_pll+0x90>
     6e4:	e1520001 	cmp	r2, r1
     6e8:	0a000003 	beq	6fc <set_pll+0xa0>
     6ec:	e5da3000 	ldrb	r3, [sl]
     6f0:	e2822001 	add	r2, r2, #1	; 0x1
     6f4:	e3130001 	tst	r3, #1	; 0x1
     6f8:	0afffff9 	beq	6e4 <set_pll+0x88>
     6fc:	e3a03002 	mov	r3, #2	; 0x2
     700:	e5883000 	str	r3, [r8]
     704:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
     708:	21e0a00c 	.word	0x21e0a00c
     70c:	21e0a000 	.word	0x21e0a000
     710:	21e0a004 	.word	0x21e0a004
     714:	21e0a010 	.word	0x21e0a010
     718:	21e0a008 	.word	0x21e0a008
     71c:	21e0a014 	.word	0x21e0a014

00000720 <set_clk_divider>:
     720:	e3500005 	cmp	r0, #5	; 0x5
     724:	812fff1e 	bxhi	lr
     728:	e59fc020 	ldr	ip, [pc, #32]	; 750 <set_clk_divider+0x30>
     72c:	e1a03280 	lsl	r3, r0, #5
     730:	e083c00c 	add	ip, r3, ip
     734:	e3a02000 	mov	r2, #0	; 0x0
     738:	e7d13002 	ldrb	r3, [r1, r2]
     73c:	e2822001 	add	r2, r2, #1	; 0x1
     740:	e3520008 	cmp	r2, #8	; 0x8
     744:	e4cc3004 	strb	r3, [ip], #4
     748:	1afffffa 	bne	738 <set_clk_divider+0x18>
     74c:	e12fff1e 	bx	lr
     750:	21e0a070 	.word	0x21e0a070

00000754 <module_set_clock>:
     754:	e59f3020 	ldr	r3, [pc, #32]	; 77c <module_set_clock+0x28>
     758:	e1500003 	cmp	r0, r3
     75c:	13a03001 	movne	r3, #1	; 0x1
     760:	1580300c 	strne	r3, [r0, #12]
     764:	e2013007 	and	r3, r1, #7	; 0x7
     768:	e5803000 	str	r3, [r0]
     76c:	e3a03000 	mov	r3, #0	; 0x0
     770:	e5802004 	str	r2, [r0, #4]
     774:	e580300c 	str	r3, [r0, #12]
     778:	e12fff1e 	bx	lr
     77c:	21e0a360 	.word	0x21e0a360

00000780 <module_enable>:
     780:	e3a020ff 	mov	r2, #255	; 0xff
     784:	e3a01000 	mov	r1, #0	; 0x0
     788:	e3a03011 	mov	r3, #17	; 0x11
     78c:	e580100c 	str	r1, [r0, #12]
     790:	e5802000 	str	r2, [r0]
     794:	e5802004 	str	r2, [r0, #4]
     798:	e5803008 	str	r3, [r0, #8]
     79c:	e5903008 	ldr	r3, [r0, #8]
     7a0:	e3c33010 	bic	r3, r3, #16	; 0x10
     7a4:	e5803008 	str	r3, [r0, #8]
     7a8:	e5802018 	str	r2, [r0, #24]
     7ac:	e5801000 	str	r1, [r0]
     7b0:	e580200c 	str	r2, [r0, #12]
     7b4:	e12fff1e 	bx	lr

000007b8 <init_lowlevel>:
     7b8:	e3a01000 	mov	r1, #0	; 0x0
     7bc:	e3e00000 	mvn	r0, #0	; 0x0
     7c0:	e59f3018 	ldr	r3, [pc, #24]	; 7e0 <init_lowlevel+0x28>
     7c4:	e1a02101 	lsl	r2, r1, #2
     7c8:	e2811001 	add	r1, r1, #1	; 0x1
     7cc:	e0823003 	add	r3, r2, r3
     7d0:	e3510008 	cmp	r1, #8	; 0x8
     7d4:	e5830000 	str	r0, [r3]
     7d8:	1afffff8 	bne	7c0 <init_lowlevel+0x8>
     7dc:	e12fff1e 	bx	lr
     7e0:	21b01180 	.word	0x21b01180

000007e4 <init_igps_ram>:
     7e4:	e59f3008 	ldr	r3, [pc, #8]	; 7f4 <init_igps_ram+0x10>
     7e8:	e3a02001 	mov	r2, #1	; 0x1
     7ec:	e5832c10 	str	r2, [r3, #3088]
     7f0:	e12fff1e 	bx	lr
     7f4:	21e0a000 	.word	0x21e0a000

000007f8 <init_crypto>:
     7f8:	e12fff1e 	bx	lr

000007fc <get_reset_stats>:
     7fc:	e59f2028 	ldr	r2, [pc, #40]	; 82c <get_reset_stats+0x30>
     800:	e5923c10 	ldr	r3, [r2, #3088]
     804:	e2130002 	ands	r0, r3, #2	; 0x2
     808:	012fff1e 	bxeq	lr
     80c:	e5920c2c 	ldr	r0, [r2, #3116]
     810:	e3100001 	tst	r0, #1	; 0x1
     814:	03a00001 	moveq	r0, #1	; 0x1
     818:	012fff1e 	bxeq	lr
     81c:	e3500037 	cmp	r0, #55	; 0x37
     820:	03a00002 	moveq	r0, #2	; 0x2
     824:	13a00003 	movne	r0, #3	; 0x3
     828:	e12fff1e 	bx	lr
     82c:	21e09000 	.word	0x21e09000

00000830 <update_mask_on_reset>:
     830:	e59f3018 	ldr	r3, [pc, #24]	; 850 <update_mask_on_reset+0x20>
     834:	e5932c30 	ldr	r2, [r3, #3120]
     838:	e3120080 	tst	r2, #128	; 0x80
     83c:	13a03001 	movne	r3, #1	; 0x1
     840:	1202200f 	andne	r2, r2, #15	; 0xf
     844:	11e03213 	mvnne	r3, r3, lsl r2
     848:	15803000 	strne	r3, [r0]
     84c:	e12fff1e 	bx	lr
     850:	21e09000 	.word	0x21e09000

00000854 <get_wakeup_entry>:
     854:	e59f301c 	ldr	r3, [pc, #28]	; 878 <get_wakeup_entry+0x24>
     858:	e5931c38 	ldr	r1, [r3, #3128]
     85c:	e5932c34 	ldr	r2, [r3, #3124]
     860:	e5930c30 	ldr	r0, [r3, #3120]
     864:	e1822401 	orr	r2, r2, r1, lsl #8
     868:	e5933c2c 	ldr	r3, [r3, #3116]
     86c:	e1800402 	orr	r0, r0, r2, lsl #8
     870:	e1830400 	orr	r0, r3, r0, lsl #8
     874:	e12fff1e 	bx	lr
     878:	21e09000 	.word	0x21e09000

0000087c <system_reset>:
     87c:	e59f3010 	ldr	r3, [pc, #16]	; 894 <system_reset+0x18>
     880:	e3a02001 	mov	r2, #1	; 0x1
     884:	e5832c2c 	str	r2, [r3, #3116]
     888:	e2822002 	add	r2, r2, #2	; 0x2
     88c:	e5832c00 	str	r2, [r3, #3072]
     890:	e12fff1e 	bx	lr
     894:	21e09000 	.word	0x21e09000

00000898 <pads_init_module>:
     898:	e92d4030 	push	{r4, r5, lr}
     89c:	e1a05000 	mov	r5, r0
     8a0:	e24dd004 	sub	sp, sp, #4	; 0x4
     8a4:	e59f404c 	ldr	r4, [pc, #76]	; 8f8 <pads_init_module+0x60>
     8a8:	ea00000d 	b	8e4 <pads_init_module+0x4c>
     8ac:	e1500005 	cmp	r0, r5
     8b0:	1a00000a 	bne	8e0 <pads_init_module+0x48>
     8b4:	e3a01000 	mov	r1, #0	; 0x0
     8b8:	e1a02001 	mov	r2, r1
     8bc:	e5140004 	ldr	r0, [r4, #-4]
     8c0:	eb0000c4 	bl	bd8 <pads_chmod>
     8c4:	e3a00101 	mov	r0, #1073741824	; 0x40000000
     8c8:	eb00002a 	bl	978 <ecfg_check_flag>
     8cc:	e3500000 	cmp	r0, #0	; 0x0
     8d0:	0a000002 	beq	8e0 <pads_init_module+0x48>
     8d4:	e8140005 	ldmda	r4, {r0, r2}
     8d8:	e3a01001 	mov	r1, #1	; 0x1
     8dc:	eb000112 	bl	d2c <pads_pull>
     8e0:	e284400c 	add	r4, r4, #12	; 0xc
     8e4:	e5140008 	ldr	r0, [r4, #-8]
     8e8:	e3500000 	cmp	r0, #0	; 0x0
     8ec:	1affffee 	bne	8ac <pads_init_module+0x14>
     8f0:	e28dd004 	add	sp, sp, #4	; 0x4
     8f4:	e8bd8030 	pop	{r4, r5, pc}
     8f8:	3c00d008 	.word	0x3c00d008

000008fc <module_reset>:
     8fc:	e3a030ff 	mov	r3, #255	; 0xff
     900:	e5803000 	str	r3, [r0]
     904:	e92d4010 	push	{r4, lr}
     908:	e1a04000 	mov	r4, r0
     90c:	e3a0000a 	mov	r0, #10	; 0xa
     910:	eb003431 	bl	d9dc <udelay>
     914:	e3a03000 	mov	r3, #0	; 0x0
     918:	e5843000 	str	r3, [r4]
     91c:	e8bd8010 	pop	{r4, pc}

00000920 <gtc_time>:
     920:	e59f3020 	ldr	r3, [pc, #32]	; 948 <gtc_time+0x28>
     924:	e92d4010 	push	{r4, lr}
     928:	e5930000 	ldr	r0, [r3]
     92c:	e59f3018 	ldr	r3, [pc, #24]	; 94c <gtc_time+0x2c>
     930:	e5934000 	ldr	r4, [r3]
     934:	eb00344c 	bl	da6c <get_timer>
     938:	e1a01000 	mov	r1, r0
     93c:	e59f000c 	ldr	r0, [pc, #12]	; 950 <gtc_time+0x30>
     940:	e12fff34 	blx	r4
     944:	e8bd8010 	pop	{r4, pc}
     948:	3c00d124 	.word	0x3c00d124
     94c:	3c00d6cc 	.word	0x3c00d6cc
     950:	00016185 	.word	0x00016185

00000954 <init_gtc>:
     954:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     958:	e3a00000 	mov	r0, #0	; 0x0
     95c:	e24dd004 	sub	sp, sp, #4	; 0x4
     960:	eb003441 	bl	da6c <get_timer>
     964:	e59f3008 	ldr	r3, [pc, #8]	; 974 <init_gtc+0x20>
     968:	e5830000 	str	r0, [r3]
     96c:	e28dd004 	add	sp, sp, #4	; 0x4
     970:	e8bd8000 	pop	{pc}
     974:	3c00d124 	.word	0x3c00d124

00000978 <ecfg_check_flag>:
     978:	e3500801 	cmp	r0, #65536	; 0x10000
     97c:	0a000004 	beq	994 <ecfg_check_flag+0x1c>
     980:	e3500101 	cmp	r0, #1073741824	; 0x40000000
     984:	0a000002 	beq	994 <ecfg_check_flag+0x1c>
     988:	e3500001 	cmp	r0, #1	; 0x1
     98c:	13a00001 	movne	r0, #1	; 0x1
     990:	112fff1e 	bxne	lr
     994:	e3a00000 	mov	r0, #0	; 0x0
     998:	e12fff1e 	bx	lr

0000099c <ecfg_paras>:
     99c:	e59f0000 	ldr	r0, [pc, #0]	; 9a4 <ecfg_paras+0x8>
     9a0:	e12fff1e 	bx	lr
     9a4:	3c00d6d0 	.word	0x3c00d6d0

000009a8 <efuse_mac>:
     9a8:	e3a00000 	mov	r0, #0	; 0x0
     9ac:	e3a01000 	mov	r1, #0	; 0x0
     9b0:	e12fff1e 	bx	lr

000009b4 <efuse_info>:
     9b4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     9b8:	e1a04000 	mov	r4, r0
     9bc:	e24dd004 	sub	sp, sp, #4	; 0x4
     9c0:	e59f6144 	ldr	r6, [pc, #324]	; b0c <efuse_info+0x158>
     9c4:	e59f0144 	ldr	r0, [pc, #324]	; b10 <efuse_info+0x15c>
     9c8:	e1a0e00f 	mov	lr, pc
     9cc:	e596f000 	ldr	pc, [r6]
     9d0:	e59f013c 	ldr	r0, [pc, #316]	; b14 <efuse_info+0x160>
     9d4:	e1a0e00f 	mov	lr, pc
     9d8:	e596f000 	ldr	pc, [r6]
     9dc:	e3540000 	cmp	r4, #0	; 0x0
     9e0:	0a00003c 	beq	ad8 <efuse_info+0x124>
     9e4:	e59f412c 	ldr	r4, [pc, #300]	; b18 <efuse_info+0x164>
     9e8:	e59f012c 	ldr	r0, [pc, #300]	; b1c <efuse_info+0x168>
     9ec:	e5941000 	ldr	r1, [r4]
     9f0:	e1a0e00f 	mov	lr, pc
     9f4:	e596f000 	ldr	pc, [r6]
     9f8:	e5d41004 	ldrb	r1, [r4, #4]
     9fc:	e59f011c 	ldr	r0, [pc, #284]	; b20 <efuse_info+0x16c>
     a00:	e1a0e00f 	mov	lr, pc
     a04:	e596f000 	ldr	pc, [r6]
     a08:	e5d41005 	ldrb	r1, [r4, #5]
     a0c:	e59f0110 	ldr	r0, [pc, #272]	; b24 <efuse_info+0x170>
     a10:	e1a0e00f 	mov	lr, pc
     a14:	e596f000 	ldr	pc, [r6]
     a18:	e5d41006 	ldrb	r1, [r4, #6]
     a1c:	e59f0104 	ldr	r0, [pc, #260]	; b28 <efuse_info+0x174>
     a20:	e1a0e00f 	mov	lr, pc
     a24:	e596f000 	ldr	pc, [r6]
     a28:	e5d41007 	ldrb	r1, [r4, #7]
     a2c:	e59f00f8 	ldr	r0, [pc, #248]	; b2c <efuse_info+0x178>
     a30:	e1a0e00f 	mov	lr, pc
     a34:	e596f000 	ldr	pc, [r6]
     a38:	e1d410b8 	ldrh	r1, [r4, #8]
     a3c:	e59f00ec 	ldr	r0, [pc, #236]	; b30 <efuse_info+0x17c>
     a40:	e1a0e00f 	mov	lr, pc
     a44:	e596f000 	ldr	pc, [r6]
     a48:	e1d410ba 	ldrh	r1, [r4, #10]
     a4c:	e59f00e0 	ldr	r0, [pc, #224]	; b34 <efuse_info+0x180>
     a50:	e1a0e00f 	mov	lr, pc
     a54:	e596f000 	ldr	pc, [r6]
     a58:	e1d410bc 	ldrh	r1, [r4, #12]
     a5c:	e59f00d4 	ldr	r0, [pc, #212]	; b38 <efuse_info+0x184>
     a60:	e1a0e00f 	mov	lr, pc
     a64:	e596f000 	ldr	pc, [r6]
     a68:	e1d410be 	ldrh	r1, [r4, #14]
     a6c:	e59f00c8 	ldr	r0, [pc, #200]	; b3c <efuse_info+0x188>
     a70:	e1a0e00f 	mov	lr, pc
     a74:	e596f000 	ldr	pc, [r6]
     a78:	e1a05004 	mov	r5, r4
     a7c:	e3a04000 	mov	r4, #0	; 0x0
     a80:	e1a01004 	mov	r1, r4
     a84:	e5952010 	ldr	r2, [r5, #16]
     a88:	e2844001 	add	r4, r4, #1	; 0x1
     a8c:	e59f00ac 	ldr	r0, [pc, #172]	; b40 <efuse_info+0x18c>
     a90:	e1a0e00f 	mov	lr, pc
     a94:	e596f000 	ldr	pc, [r6]
     a98:	e3540004 	cmp	r4, #4	; 0x4
     a9c:	e2855004 	add	r5, r5, #4	; 0x4
     aa0:	1afffff6 	bne	a80 <efuse_info+0xcc>
     aa4:	e59f6060 	ldr	r6, [pc, #96]	; b0c <efuse_info+0x158>
     aa8:	e59f5068 	ldr	r5, [pc, #104]	; b18 <efuse_info+0x164>
     aac:	e284400c 	add	r4, r4, #12	; 0xc
     ab0:	e1a03084 	lsl	r3, r4, #1
     ab4:	e1a01004 	mov	r1, r4
     ab8:	e19520b3 	ldrh	r2, [r5, r3]
     abc:	e2844001 	add	r4, r4, #1	; 0x1
     ac0:	e59f007c 	ldr	r0, [pc, #124]	; b44 <efuse_info+0x190>
     ac4:	e1a0e00f 	mov	lr, pc
     ac8:	e596f000 	ldr	pc, [r6]
     acc:	e354001c 	cmp	r4, #28	; 0x1c
     ad0:	1afffff6 	bne	ab0 <efuse_info+0xfc>
     ad4:	ea00000a 	b	b04 <efuse_info+0x150>
     ad8:	e59f4068 	ldr	r4, [pc, #104]	; b48 <efuse_info+0x194>
     adc:	e59f7068 	ldr	r7, [pc, #104]	; b4c <efuse_info+0x198>
     ae0:	e1a05006 	mov	r5, r6
     ae4:	e5942000 	ldr	r2, [r4]
     ae8:	e1a01004 	mov	r1, r4
     aec:	e59f005c 	ldr	r0, [pc, #92]	; b50 <efuse_info+0x19c>
     af0:	e2844004 	add	r4, r4, #4	; 0x4
     af4:	e1a0e00f 	mov	lr, pc
     af8:	e595f000 	ldr	pc, [r5]
     afc:	e1540007 	cmp	r4, r7
     b00:	1afffff7 	bne	ae4 <efuse_info+0x130>
     b04:	e28dd004 	add	sp, sp, #4	; 0x4
     b08:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     b0c:	3c00d6cc 	.word	0x3c00d6cc
     b10:	0001618d 	.word	0x0001618d
     b14:	000185c3 	.word	0x000185c3
     b18:	3c00d6d0 	.word	0x3c00d6d0
     b1c:	000161a2 	.word	0x000161a2
     b20:	000161b1 	.word	0x000161b1
     b24:	000161c0 	.word	0x000161c0
     b28:	000161d3 	.word	0x000161d3
     b2c:	000161e2 	.word	0x000161e2
     b30:	000161f7 	.word	0x000161f7
     b34:	00016204 	.word	0x00016204
     b38:	00016214 	.word	0x00016214
     b3c:	00016225 	.word	0x00016225
     b40:	00016237 	.word	0x00016237
     b44:	00016247 	.word	0x00016247
     b48:	21e0d9b4 	.word	0x21e0d9b4
     b4c:	21e0da94 	.word	0x21e0da94
     b50:	00016258 	.word	0x00016258

00000b54 <init_efuse>:
     b54:	e59f0008 	ldr	r0, [pc, #8]	; b64 <init_efuse+0x10>
     b58:	e3a01000 	mov	r1, #0	; 0x0
     b5c:	e3a02038 	mov	r2, #56	; 0x38
     b60:	ea000273 	b	1534 <memset>
     b64:	3c00d6d0 	.word	0x3c00d6d0

00000b68 <assert_linkpc>:
     b68:	e59f300c 	ldr	r3, [pc, #12]	; b7c <assert_linkpc+0x14>
     b6c:	e5930440 	ldr	r0, [r3, #1088]
     b70:	e2700001 	rsbs	r0, r0, #1	; 0x1
     b74:	33a00000 	movcc	r0, #0	; 0x0
     b78:	e12fff1e 	bx	lr
     b7c:	20f02000 	.word	0x20f02000

00000b80 <pads_states>:
     b80:	e3500000 	cmp	r0, #0	; 0x0
     b84:	e2803007 	add	r3, r0, #7	; 0x7
     b88:	a1a03000 	movge	r3, r0
     b8c:	e1a031c3 	asr	r3, r3, #3
     b90:	e59f203c 	ldr	r2, [pc, #60]	; bd4 <pads_states+0x54>
     b94:	e1a03103 	lsl	r3, r3, #2
     b98:	e0832002 	add	r2, r3, r2
     b9c:	e5921000 	ldr	r1, [r2]
     ba0:	e3a03001 	mov	r3, #1	; 0x1
     ba4:	e2002007 	and	r2, r0, #7	; 0x7
     ba8:	e0113213 	ands	r3, r1, r3, lsl r2
     bac:	01a00003 	moveq	r0, r3
     bb0:	012fff1e 	bxeq	lr
     bb4:	e1a03300 	lsl	r3, r0, #6
     bb8:	e2833282 	add	r3, r3, #536870920	; 0x20000008
     bbc:	e283360f 	add	r3, r3, #15728640	; 0xf00000
     bc0:	e5933000 	ldr	r3, [r3]
     bc4:	e3530000 	cmp	r3, #0	; 0x0
     bc8:	13a00002 	movne	r0, #2	; 0x2
     bcc:	03a00001 	moveq	r0, #1	; 0x1
     bd0:	e12fff1e 	bx	lr
     bd4:	21e09064 	.word	0x21e09064

00000bd8 <pads_chmod>:
     bd8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     bdc:	e1b08840 	asrs	r8, r0, #16
     be0:	e1a00800 	lsl	r0, r0, #16
     be4:	03a03000 	moveq	r3, #0	; 0x0
     be8:	13a03001 	movne	r3, #1	; 0x1
     bec:	e1a00820 	lsr	r0, r0, #16
     bf0:	e1580000 	cmp	r8, r0
     bf4:	a3a0c000 	movge	ip, #0	; 0x0
     bf8:	b203c001 	andlt	ip, r3, #1	; 0x1
     bfc:	e35c0000 	cmp	ip, #0	; 0x0
     c00:	e24dd004 	sub	sp, sp, #4	; 0x4
     c04:	e1a06001 	mov	r6, r1
     c08:	e1a0b002 	mov	fp, r2
     c0c:	1a000040 	bne	d14 <pads_chmod+0x13c>
     c10:	e2803883 	add	r3, r0, #8585216	; 0x830000
     c14:	e2833903 	add	r3, r3, #49152	; 0xc000
     c18:	e1a02303 	lsl	r2, r3, #6
     c1c:	e3580000 	cmp	r8, #0	; 0x0
     c20:	01a08000 	moveq	r8, r0
     c24:	e2827008 	add	r7, r2, #8	; 0x8
     c28:	e282a004 	add	sl, r2, #4	; 0x4
     c2c:	e1a04000 	mov	r4, r0
     c30:	e240501f 	sub	r5, r0, #31	; 0x1f
     c34:	e3a09001 	mov	r9, #1	; 0x1
     c38:	ea000033 	b	d0c <pads_chmod+0x134>
     c3c:	e3540000 	cmp	r4, #0	; 0x0
     c40:	e2843007 	add	r3, r4, #7	; 0x7
     c44:	a1a03004 	movge	r3, r4
     c48:	e3550006 	cmp	r5, #6	; 0x6
     c4c:	959f20c8 	ldrls	r2, [pc, #200]	; d1c <pads_chmod+0x144>
     c50:	e1a011c3 	asr	r1, r3, #3
     c54:	95923008 	ldrls	r3, [r2, #8]
     c58:	e2040007 	and	r0, r4, #7	; 0x7
     c5c:	91833519 	orrls	r3, r3, r9, lsl r5
     c60:	95823008 	strls	r3, [r2, #8]
     c64:	e3560001 	cmp	r6, #1	; 0x1
     c68:	0a00000a 	beq	c98 <pads_chmod+0xc0>
     c6c:	e3560002 	cmp	r6, #2	; 0x2
     c70:	0a000011 	beq	cbc <pads_chmod+0xe4>
     c74:	e3560000 	cmp	r6, #0	; 0x0
     c78:	1a00001a 	bne	ce8 <pads_chmod+0x110>
     c7c:	e59f309c 	ldr	r3, [pc, #156]	; d20 <pads_chmod+0x148>
     c80:	e1a02101 	lsl	r2, r1, #2
     c84:	e0823003 	add	r3, r2, r3
     c88:	e5932000 	ldr	r2, [r3]
     c8c:	e1c22019 	bic	r2, r2, r9, lsl r0
     c90:	e5832000 	str	r2, [r3]
     c94:	ea000018 	b	cfc <pads_chmod+0x124>
     c98:	e59f3080 	ldr	r3, [pc, #128]	; d20 <pads_chmod+0x148>
     c9c:	e1a02101 	lsl	r2, r1, #2
     ca0:	e0823003 	add	r3, r2, r3
     ca4:	e5932000 	ldr	r2, [r3]
     ca8:	e1822016 	orr	r2, r2, r6, lsl r0
     cac:	e5832000 	str	r2, [r3]
     cb0:	e3a03000 	mov	r3, #0	; 0x0
     cb4:	e5873000 	str	r3, [r7]
     cb8:	ea00000f 	b	cfc <pads_chmod+0x124>
     cbc:	e59f205c 	ldr	r2, [pc, #92]	; d20 <pads_chmod+0x148>
     cc0:	e1a03101 	lsl	r3, r1, #2
     cc4:	e0832002 	add	r2, r3, r2
     cc8:	e5923000 	ldr	r3, [r2]
     ccc:	e25b1000 	subs	r1, fp, #0	; 0x0
     cd0:	13a01001 	movne	r1, #1	; 0x1
     cd4:	e1833019 	orr	r3, r3, r9, lsl r0
     cd8:	e5823000 	str	r3, [r2]
     cdc:	e5879000 	str	r9, [r7]
     ce0:	e58a1000 	str	r1, [sl]
     ce4:	ea000004 	b	cfc <pads_chmod+0x124>
     ce8:	e59f0034 	ldr	r0, [pc, #52]	; d24 <pads_chmod+0x14c>
     cec:	e1a01006 	mov	r1, r6
     cf0:	e59f2030 	ldr	r2, [pc, #48]	; d28 <pads_chmod+0x150>
     cf4:	e1a0e00f 	mov	lr, pc
     cf8:	e592f000 	ldr	pc, [r2]
     cfc:	e2844001 	add	r4, r4, #1	; 0x1
     d00:	e2877040 	add	r7, r7, #64	; 0x40
     d04:	e28aa040 	add	sl, sl, #64	; 0x40
     d08:	e2855001 	add	r5, r5, #1	; 0x1
     d0c:	e1540008 	cmp	r4, r8
     d10:	daffffc9 	ble	c3c <pads_chmod+0x64>
     d14:	e28dd004 	add	sp, sp, #4	; 0x4
     d18:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     d1c:	21e09000 	.word	0x21e09000
     d20:	21e09064 	.word	0x21e09064
     d24:	00016268 	.word	0x00016268
     d28:	3c00d6cc 	.word	0x3c00d6cc

00000d2c <pads_pull>:
     d2c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
     d30:	e1b06840 	asrs	r6, r0, #16
     d34:	e1a00800 	lsl	r0, r0, #16
     d38:	03a03000 	moveq	r3, #0	; 0x0
     d3c:	13a03001 	movne	r3, #1	; 0x1
     d40:	e1a00820 	lsr	r0, r0, #16
     d44:	e1560000 	cmp	r6, r0
     d48:	a3a03000 	movge	r3, #0	; 0x0
     d4c:	b2033001 	andlt	r3, r3, #1	; 0x1
     d50:	e3530000 	cmp	r3, #0	; 0x0
     d54:	e1a09001 	mov	r9, r1
     d58:	e1a0a002 	mov	sl, r2
     d5c:	18bd87f0 	popne	{r4, r5, r6, r7, r8, r9, sl, pc}
     d60:	e3560000 	cmp	r6, #0	; 0x0
     d64:	01a06000 	moveq	r6, r0
     d68:	e1a0c000 	mov	ip, r0
     d6c:	e1a07003 	mov	r7, r3
     d70:	e240501f 	sub	r5, r0, #31	; 0x1f
     d74:	e3a08001 	mov	r8, #1	; 0x1
     d78:	ea000020 	b	e00 <pads_pull+0xd4>
     d7c:	e35c0000 	cmp	ip, #0	; 0x0
     d80:	e28c3007 	add	r3, ip, #7	; 0x7
     d84:	a1a0300c 	movge	r3, ip
     d88:	e1a031c3 	asr	r3, r3, #3
     d8c:	e3590000 	cmp	r9, #0	; 0x0
     d90:	e1a00103 	lsl	r0, r3, #2
     d94:	e20c4007 	and	r4, ip, #7	; 0x7
     d98:	059f206c 	ldreq	r2, [pc, #108]	; e0c <pads_pull+0xe0>
     d9c:	00802002 	addeq	r2, r0, r2
     da0:	05923000 	ldreq	r3, [r2]
     da4:	01c33418 	biceq	r3, r3, r8, lsl r4
     da8:	0a000010 	beq	df0 <pads_pull+0xc4>
     dac:	e3550006 	cmp	r5, #6	; 0x6
     db0:	859f2058 	ldrhi	r2, [pc, #88]	; e10 <pads_pull+0xe4>
     db4:	91a03005 	movls	r3, r5
     db8:	81a03004 	movhi	r3, r4
     dbc:	959f2050 	ldrls	r2, [pc, #80]	; e14 <pads_pull+0xe8>
     dc0:	80802002 	addhi	r2, r0, r2
     dc4:	e01a1718 	ands	r1, sl, r8, lsl r7
     dc8:	e1a01318 	lsl	r1, r8, r3
     dcc:	15923000 	ldrne	r3, [r2]
     dd0:	05923000 	ldreq	r3, [r2]
     dd4:	11813003 	orrne	r3, r1, r3
     dd8:	01c33001 	biceq	r3, r3, r1
     ddc:	e5823000 	str	r3, [r2]
     de0:	e59f2024 	ldr	r2, [pc, #36]	; e0c <pads_pull+0xe0>
     de4:	e0802002 	add	r2, r0, r2
     de8:	e5923000 	ldr	r3, [r2]
     dec:	e1833418 	orr	r3, r3, r8, lsl r4
     df0:	e5823000 	str	r3, [r2]
     df4:	e28cc001 	add	ip, ip, #1	; 0x1
     df8:	e2877001 	add	r7, r7, #1	; 0x1
     dfc:	e2855001 	add	r5, r5, #1	; 0x1
     e00:	e15c0006 	cmp	ip, r6
     e04:	daffffdc 	ble	d7c <pads_pull+0x50>
     e08:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
     e0c:	21e09014 	.word	0x21e09014
     e10:	21e091a4 	.word	0x21e091a4
     e14:	21e0900c 	.word	0x21e0900c

00000e18 <pads_setpin>:
     e18:	e1b0c840 	asrs	ip, r0, #16
     e1c:	e1a00800 	lsl	r0, r0, #16
     e20:	03a03000 	moveq	r3, #0	; 0x0
     e24:	13a03001 	movne	r3, #1	; 0x1
     e28:	e1a00820 	lsr	r0, r0, #16
     e2c:	e15c0000 	cmp	ip, r0
     e30:	a3a03000 	movge	r3, #0	; 0x0
     e34:	b2033001 	andlt	r3, r3, #1	; 0x1
     e38:	e3530000 	cmp	r3, #0	; 0x0
     e3c:	112fff1e 	bxne	lr
     e40:	e1a03300 	lsl	r3, r0, #6
     e44:	e2832242 	add	r2, r3, #536870916	; 0x20000004
     e48:	e35c0000 	cmp	ip, #0	; 0x0
     e4c:	01a0c000 	moveq	ip, r0
     e50:	e282260f 	add	r2, r2, #15728640	; 0xf00000
     e54:	ea000002 	b	e64 <pads_setpin+0x4c>
     e58:	e2513000 	subs	r3, r1, #0	; 0x0
     e5c:	13a03001 	movne	r3, #1	; 0x1
     e60:	e4823040 	str	r3, [r2], #64
     e64:	e150000c 	cmp	r0, ip
     e68:	e2800001 	add	r0, r0, #1	; 0x1
     e6c:	dafffff9 	ble	e58 <pads_setpin+0x40>
     e70:	e12fff1e 	bx	lr

00000e74 <pads_getpin>:
     e74:	e59f3014 	ldr	r3, [pc, #20]	; e90 <pads_getpin+0x1c>
     e78:	e6f30070 	uxtah	r0, r3, r0
     e7c:	e1a00300 	lsl	r0, r0, #6
     e80:	e5900000 	ldr	r0, [r0]
     e84:	e2500000 	subs	r0, r0, #0	; 0x0
     e88:	13a00001 	movne	r0, #1	; 0x1
     e8c:	e12fff1e 	bx	lr
     e90:	0083c000 	.word	0x0083c000

00000e94 <init_alternate>:
     e94:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     e98:	e59f0030 	ldr	r0, [pc, #48]	; ed0 <init_alternate+0x3c>
     e9c:	e24dd004 	sub	sp, sp, #4	; 0x4
     ea0:	ebfffe36 	bl	780 <module_enable>
     ea4:	e3a00091 	mov	r0, #145	; 0x91
     ea8:	e3a01001 	mov	r1, #1	; 0x1
     eac:	e3a02000 	mov	r2, #0	; 0x0
     eb0:	ebffff48 	bl	bd8 <pads_chmod>
     eb4:	e3a01001 	mov	r1, #1	; 0x1
     eb8:	e3a00091 	mov	r0, #145	; 0x91
     ebc:	e1a02001 	mov	r2, r1
     ec0:	ebffff99 	bl	d2c <pads_pull>
     ec4:	e3a00000 	mov	r0, #0	; 0x0
     ec8:	e28dd004 	add	sp, sp, #4	; 0x4
     ecc:	e8bd8000 	pop	{pc}
     ed0:	21e13c00 	.word	0x21e13c00

00000ed4 <cpu_recase>:
     ed4:	e92d4010 	push	{r4, lr}
     ed8:	e59f10a8 	ldr	r1, [pc, #168]	; f88 <cpu_recase+0xb4>
     edc:	e3a02003 	mov	r2, #3	; 0x3
     ee0:	e1a04000 	mov	r4, r0
     ee4:	eb000161 	bl	1470 <strncmp>
     ee8:	e3500000 	cmp	r0, #0	; 0x0
     eec:	0a00000b 	beq	f20 <cpu_recase+0x4c>
     ef0:	e1a00004 	mov	r0, r4
     ef4:	e59f1090 	ldr	r1, [pc, #144]	; f8c <cpu_recase+0xb8>
     ef8:	e3a02003 	mov	r2, #3	; 0x3
     efc:	eb00015b 	bl	1470 <strncmp>
     f00:	e3500000 	cmp	r0, #0	; 0x0
     f04:	0a000005 	beq	f20 <cpu_recase+0x4c>
     f08:	e1a00004 	mov	r0, r4
     f0c:	e59f107c 	ldr	r1, [pc, #124]	; f90 <cpu_recase+0xbc>
     f10:	e3a02003 	mov	r2, #3	; 0x3
     f14:	eb000155 	bl	1470 <strncmp>
     f18:	e3500000 	cmp	r0, #0	; 0x0
     f1c:	1a000001 	bne	f28 <cpu_recase+0x54>
     f20:	e3a02002 	mov	r2, #2	; 0x2
     f24:	ea000006 	b	f44 <cpu_recase+0x70>
     f28:	e1a00004 	mov	r0, r4
     f2c:	e59f1060 	ldr	r1, [pc, #96]	; f94 <cpu_recase+0xc0>
     f30:	e3a02004 	mov	r2, #4	; 0x4
     f34:	eb00014d 	bl	1470 <strncmp>
     f38:	e3500000 	cmp	r0, #0	; 0x0
     f3c:	18bd8010 	popne	{r4, pc}
     f40:	e3a02001 	mov	r2, #1	; 0x1
     f44:	e59f304c 	ldr	r3, [pc, #76]	; f98 <cpu_recase+0xc4>
     f48:	e59f4048 	ldr	r4, [pc, #72]	; f98 <cpu_recase+0xc4>
     f4c:	e59f1048 	ldr	r1, [pc, #72]	; f9c <cpu_recase+0xc8>
     f50:	e5832000 	str	r2, [r3]
     f54:	e5942000 	ldr	r2, [r4]
     f58:	e5913000 	ldr	r3, [r1]
     f5c:	e1520003 	cmp	r2, r3
     f60:	08bd8010 	popeq	{r4, pc}
     f64:	e1a00002 	mov	r0, r2
     f68:	e5812000 	str	r2, [r1]
     f6c:	eb00009f 	bl	11f0 <set_mux>
     f70:	e5941000 	ldr	r1, [r4]
     f74:	e59f0024 	ldr	r0, [pc, #36]	; fa0 <cpu_recase+0xcc>
     f78:	e59f3024 	ldr	r3, [pc, #36]	; fa4 <cpu_recase+0xd0>
     f7c:	e1a0e00f 	mov	lr, pc
     f80:	e593f000 	ldr	pc, [r3]
     f84:	e8bd8010 	pop	{r4, pc}
     f88:	00016127 	.word	0x00016127
     f8c:	0001627f 	.word	0x0001627f
     f90:	00016283 	.word	0x00016283
     f94:	00016287 	.word	0x00016287
     f98:	3c00d128 	.word	0x3c00d128
     f9c:	3c00d084 	.word	0x3c00d084
     fa0:	0001628c 	.word	0x0001628c
     fa4:	3c00d6cc 	.word	0x3c00d6cc

00000fa8 <set_clk>:
     fa8:	e92d4030 	push	{r4, r5, lr}
     fac:	e1d010b2 	ldrh	r1, [r0, #2]
     fb0:	e24dd004 	sub	sp, sp, #4	; 0x4
     fb4:	e3510000 	cmp	r1, #0	; 0x0
     fb8:	e1a04000 	mov	r4, r0
     fbc:	0a000001 	beq	fc8 <set_clk+0x20>
     fc0:	e3a00001 	mov	r0, #1	; 0x1
     fc4:	ebfffda4 	bl	65c <set_pll>
     fc8:	e1d410b4 	ldrh	r1, [r4, #4]
     fcc:	e3510000 	cmp	r1, #0	; 0x0
     fd0:	0a000001 	beq	fdc <set_clk+0x34>
     fd4:	e3a00002 	mov	r0, #2	; 0x2
     fd8:	ebfffd9f 	bl	65c <set_pll>
     fdc:	e1d410b6 	ldrh	r1, [r4, #6]
     fe0:	e3510000 	cmp	r1, #0	; 0x0
     fe4:	0a000001 	beq	ff0 <set_clk+0x48>
     fe8:	e3a00003 	mov	r0, #3	; 0x3
     fec:	ebfffd9a 	bl	65c <set_pll>
     ff0:	e59f00c4 	ldr	r0, [pc, #196]	; 10bc <set_clk+0x114>
     ff4:	e3a01000 	mov	r1, #0	; 0x0
     ff8:	e3a02003 	mov	r2, #3	; 0x3
     ffc:	ebfffdd4 	bl	754 <module_set_clock>
    1000:	e59f00b8 	ldr	r0, [pc, #184]	; 10c0 <set_clk+0x118>
    1004:	e3a01001 	mov	r1, #1	; 0x1
    1008:	e3a0200b 	mov	r2, #11	; 0xb
    100c:	ebfffdd0 	bl	754 <module_set_clock>
    1010:	e59f00ac 	ldr	r0, [pc, #172]	; 10c4 <set_clk+0x11c>
    1014:	e3a01001 	mov	r1, #1	; 0x1
    1018:	e3a02005 	mov	r2, #5	; 0x5
    101c:	ebfffdcc 	bl	754 <module_set_clock>
    1020:	e59f00a0 	ldr	r0, [pc, #160]	; 10c8 <set_clk+0x120>
    1024:	e3a01001 	mov	r1, #1	; 0x1
    1028:	e3a02013 	mov	r2, #19	; 0x13
    102c:	ebfffdc8 	bl	754 <module_set_clock>
    1030:	e59f0094 	ldr	r0, [pc, #148]	; 10cc <set_clk+0x124>
    1034:	e3a01002 	mov	r1, #2	; 0x2
    1038:	e3a0200f 	mov	r2, #15	; 0xf
    103c:	ebfffdc4 	bl	754 <module_set_clock>
    1040:	e59f0088 	ldr	r0, [pc, #136]	; 10d0 <set_clk+0x128>
    1044:	e3a01002 	mov	r1, #2	; 0x2
    1048:	e3a02005 	mov	r2, #5	; 0x5
    104c:	ebfffdc0 	bl	754 <module_set_clock>
    1050:	e59f007c 	ldr	r0, [pc, #124]	; 10d4 <set_clk+0x12c>
    1054:	e3a01002 	mov	r1, #2	; 0x2
    1058:	e3a0200b 	mov	r2, #11	; 0xb
    105c:	ebfffdbc 	bl	754 <module_set_clock>
    1060:	e59f0070 	ldr	r0, [pc, #112]	; 10d8 <set_clk+0x130>
    1064:	e3a01002 	mov	r1, #2	; 0x2
    1068:	e3a02019 	mov	r2, #25	; 0x19
    106c:	ebfffdb8 	bl	754 <module_set_clock>
    1070:	e59f4064 	ldr	r4, [pc, #100]	; 10dc <set_clk+0x134>
    1074:	e59f5040 	ldr	r5, [pc, #64]	; 10bc <set_clk+0x114>
    1078:	e3a01003 	mov	r1, #3	; 0x3
    107c:	e1a00004 	mov	r0, r4
    1080:	e1a02001 	mov	r2, r1
    1084:	e2844010 	add	r4, r4, #16	; 0x10
    1088:	ebfffdb1 	bl	754 <module_set_clock>
    108c:	e1540005 	cmp	r4, r5
    1090:	1afffff8 	bne	1078 <set_clk+0xd0>
    1094:	e59f0044 	ldr	r0, [pc, #68]	; 10e0 <set_clk+0x138>
    1098:	e3a01003 	mov	r1, #3	; 0x3
    109c:	e3a02013 	mov	r2, #19	; 0x13
    10a0:	ebfffdab 	bl	754 <module_set_clock>
    10a4:	e59f0038 	ldr	r0, [pc, #56]	; 10e4 <set_clk+0x13c>
    10a8:	e3a01000 	mov	r1, #0	; 0x0
    10ac:	e3a02005 	mov	r2, #5	; 0x5
    10b0:	e28dd004 	add	sp, sp, #4	; 0x4
    10b4:	e8bd4030 	pop	{r4, r5, lr}
    10b8:	eafffda5 	b	754 <module_set_clock>
    10bc:	21e0a190 	.word	0x21e0a190
    10c0:	21e0a250 	.word	0x21e0a250
    10c4:	21e0a270 	.word	0x21e0a270
    10c8:	21e0a2c0 	.word	0x21e0a2c0
    10cc:	a240      	.short	0xa240
    10ce:	e0          	.byte	0xe0
    10cf:	21          	.byte	0x21
    10d0:	21e0a2f0 	.word	0x21e0a2f0
    10d4:	21e0a2e0 	.word	0x21e0a2e0
    10d8:	21e0a290 	.word	0x21e0a290
    10dc:	21e0a130 	.word	0x21e0a130
    10e0:	21e0a260 	.word	0x21e0a260
    10e4:	21e0a360 	.word	0x21e0a360

000010e8 <chip_test>:
    10e8:	e59f0044 	ldr	r0, [pc, #68]	; 1134 <end+0x4>
    10ec:	e5901000 	ldr	r1, [r0]
    10f0:	e59f2040 	ldr	r2, [pc, #64]	; 1138 <end+0x8>
    10f4:	e1520001 	cmp	r2, r1
    10f8:	1a00000c 	bne	1130 <end>
    10fc:	e5901004 	ldr	r1, [r0, #4]
    1100:	e59f2034 	ldr	r2, [pc, #52]	; 113c <end+0xc>
    1104:	e1520001 	cmp	r2, r1
    1108:	1a000008 	bne	1130 <end>
    110c:	e5901008 	ldr	r1, [r0, #8]
    1110:	e59f2028 	ldr	r2, [pc, #40]	; 1140 <end+0x10>
    1114:	e1520001 	cmp	r2, r1
    1118:	1a000004 	bne	1130 <end>
    111c:	e590100c 	ldr	r1, [r0, #12]
    1120:	e59f201c 	ldr	r2, [pc, #28]	; 1144 <end+0x14>
    1124:	e1520001 	cmp	r2, r1
    1128:	1a000000 	bne	1130 <end>
    112c:	e3a0f30f 	mov	pc, #1006632960	; 0x3c000000

00001130 <end>:
    1130:	e1a0f00e 	mov	pc, lr
    1134:	3c007ff0 	.word	0x3c007ff0
    1138:	6f666e69 	.word	0x6f666e69
    113c:	612d6d74 	.word	0x612d6d74
    1140:	2d6e696c 	.word	0x2d6e696c
    1144:	796c6e6f 	.word	0x796c6e6f

00001148 <module_get_clock>:
    1148:	e59f3080 	ldr	r3, [pc, #128]	; 11d0 <module_get_clock+0x88>
    114c:	e1500003 	cmp	r0, r3
    1150:	0a000018 	beq	11b8 <module_get_clock+0x70>
    1154:	8a00000b 	bhi	1188 <module_get_clock+0x40>
    1158:	e3500003 	cmp	r0, #3	; 0x3
    115c:	059f0070 	ldreq	r0, [pc, #112]	; 11d4 <module_get_clock+0x8c>
    1160:	012fff1e 	bxeq	lr
    1164:	8a000002 	bhi	1174 <module_get_clock+0x2c>
    1168:	e3500001 	cmp	r0, #1	; 0x1
    116c:	1a000015 	bne	11c8 <module_get_clock+0x80>
    1170:	ea000012 	b	11c0 <module_get_clock+0x78>
    1174:	e59f305c 	ldr	r3, [pc, #92]	; 11d8 <module_get_clock+0x90>
    1178:	e1500003 	cmp	r0, r3
    117c:	12833010 	addne	r3, r3, #16	; 0x10
    1180:	1a00000a 	bne	11b0 <module_get_clock+0x68>
    1184:	ea00000b 	b	11b8 <module_get_clock+0x70>
    1188:	e59f304c 	ldr	r3, [pc, #76]	; 11dc <module_get_clock+0x94>
    118c:	e1500003 	cmp	r0, r3
    1190:	0a000008 	beq	11b8 <module_get_clock+0x70>
    1194:	92433020 	subls	r3, r3, #32	; 0x20
    1198:	9a000004 	bls	11b0 <module_get_clock+0x68>
    119c:	e59f303c 	ldr	r3, [pc, #60]	; 11e0 <module_get_clock+0x98>
    11a0:	e1500003 	cmp	r0, r3
    11a4:	059f0038 	ldreq	r0, [pc, #56]	; 11e4 <module_get_clock+0x9c>
    11a8:	012fff1e 	bxeq	lr
    11ac:	e28330a0 	add	r3, r3, #160	; 0xa0
    11b0:	e1500003 	cmp	r0, r3
    11b4:	1a000003 	bne	11c8 <module_get_clock+0x80>
    11b8:	e59f0028 	ldr	r0, [pc, #40]	; 11e8 <module_get_clock+0xa0>
    11bc:	e12fff1e 	bx	lr
    11c0:	e59f0024 	ldr	r0, [pc, #36]	; 11ec <module_get_clock+0xa4>
    11c4:	e12fff1e 	bx	lr
    11c8:	e3a00000 	mov	r0, #0	; 0x0
    11cc:	e12fff1e 	bx	lr
    11d0:	21e0a260 	.word	0x21e0a260
    11d4:	00989680 	.word	0x00989680
    11d8:	21e0a240 	.word	0x21e0a240
    11dc:	21e0a290 	.word	0x21e0a290
    11e0:	21e0a2c0 	.word	0x21e0a2c0
    11e4:	02dc6c00 	.word	0x02dc6c00
    11e8:	02625a00 	.word	0x02625a00
    11ec:	01312d00 	.word	0x01312d00

000011f0 <set_mux>:
    11f0:	e59f3004 	ldr	r3, [pc, #4]	; 11fc <set_mux+0xc>
    11f4:	e5830000 	str	r0, [r3]
    11f8:	e12fff1e 	bx	lr
    11fc:	21e09000 	.word	0x21e09000

00001200 <efuse_inject>:
    1200:	e92d4010 	push	{r4, lr}
    1204:	e24dd058 	sub	sp, sp, #88	; 0x58
    1208:	e3a01000 	mov	r1, #0	; 0x0
    120c:	e3a02038 	mov	r2, #56	; 0x38
    1210:	e1a0000d 	mov	r0, sp
    1214:	eb0000c6 	bl	1534 <memset>
    1218:	e3a03017 	mov	r3, #23	; 0x17
    121c:	e5cd3004 	strb	r3, [sp, #4]
    1220:	e3a03016 	mov	r3, #22	; 0x16
    1224:	e5cd3005 	strb	r3, [sp, #5]
    1228:	e3e03078 	mvn	r3, #120	; 0x78
    122c:	e5cd3006 	strb	r3, [sp, #6]
    1230:	e28330ca 	add	r3, r3, #202	; 0xca
    1234:	e5cd3007 	strb	r3, [sp, #7]
    1238:	e3073259 	movw	r3, #29273	; 0x7259
    123c:	e1cd30b8 	strh	r3, [sp, #8]
    1240:	e3043023 	movw	r3, #16419	; 0x4023
    1244:	e1cd30ba 	strh	r3, [sp, #10]
    1248:	e3003994 	movw	r3, #2452	; 0x994
    124c:	e1cd30be 	strh	r3, [sp, #14]
    1250:	e59f310c 	ldr	r3, [pc, #268]	; 1364 <efuse_inject+0x164>
    1254:	e59f110c 	ldr	r1, [pc, #268]	; 1368 <efuse_inject+0x168>
    1258:	e58d3010 	str	r3, [sp, #16]
    125c:	e59f3108 	ldr	r3, [pc, #264]	; 136c <efuse_inject+0x16c>
    1260:	e3a02020 	mov	r2, #32	; 0x20
    1264:	e58d3014 	str	r3, [sp, #20]
    1268:	e59f3100 	ldr	r3, [pc, #256]	; 1370 <efuse_inject+0x170>
    126c:	e28d0038 	add	r0, sp, #56	; 0x38
    1270:	e58d3018 	str	r3, [sp, #24]
    1274:	e59f30f8 	ldr	r3, [pc, #248]	; 1374 <efuse_inject+0x174>
    1278:	e1a0400d 	mov	r4, sp
    127c:	e58d301c 	str	r3, [sp, #28]
    1280:	e3003f93 	movw	r3, #3987	; 0xf93
    1284:	e1cd32b4 	strh	r3, [sp, #36]
    1288:	e2433010 	sub	r3, r3, #16	; 0x10
    128c:	e1cd32b6 	strh	r3, [sp, #38]
    1290:	e3a03a09 	mov	r3, #36864	; 0x9000
    1294:	e1cd32ba 	strh	r3, [sp, #42]
    1298:	e3a03c07 	mov	r3, #1792	; 0x700
    129c:	e1cd30bc 	strh	r3, [sp, #12]
    12a0:	e3a03005 	mov	r3, #5	; 0x5
    12a4:	e1cd32b0 	strh	r3, [sp, #32]
    12a8:	e3a03a07 	mov	r3, #28672	; 0x7000
    12ac:	e1cd32b2 	strh	r3, [sp, #34]
    12b0:	e3a03901 	mov	r3, #16384	; 0x4000
    12b4:	e1cd32b8 	strh	r3, [sp, #40]
    12b8:	eb0000a5 	bl	1554 <memcpy>
    12bc:	e59f10b4 	ldr	r1, [pc, #180]	; 1378 <efuse_inject+0x178>
    12c0:	e3a02000 	mov	r2, #0	; 0x0
    12c4:	e7d43002 	ldrb	r3, [r4, r2]
    12c8:	e2822001 	add	r2, r2, #1	; 0x1
    12cc:	e3520038 	cmp	r2, #56	; 0x38
    12d0:	e4813004 	str	r3, [r1], #4
    12d4:	1afffffa 	bne	12c4 <efuse_inject+0xc4>
    12d8:	e59f309c 	ldr	r3, [pc, #156]	; 137c <efuse_inject+0x17c>
    12dc:	e59f109c 	ldr	r1, [pc, #156]	; 1380 <efuse_inject+0x180>
    12e0:	e3a02000 	mov	r2, #0	; 0x0
    12e4:	e4832004 	str	r2, [r3], #4
    12e8:	e1530001 	cmp	r3, r1
    12ec:	1afffffc 	bne	12e4 <efuse_inject+0xe4>
    12f0:	e59f108c 	ldr	r1, [pc, #140]	; 1384 <efuse_inject+0x184>
    12f4:	e3a02000 	mov	r2, #0	; 0x0
    12f8:	e28d0038 	add	r0, sp, #56	; 0x38
    12fc:	e7d03002 	ldrb	r3, [r0, r2]
    1300:	e2822001 	add	r2, r2, #1	; 0x1
    1304:	e3520020 	cmp	r2, #32	; 0x20
    1308:	e4813004 	str	r3, [r1], #4
    130c:	1afffffa 	bne	12fc <efuse_inject+0xfc>
    1310:	e59f3070 	ldr	r3, [pc, #112]	; 1388 <efuse_inject+0x188>
    1314:	e59f1060 	ldr	r1, [pc, #96]	; 137c <efuse_inject+0x17c>
    1318:	e3a02001 	mov	r2, #1	; 0x1
    131c:	e4832004 	str	r2, [r3], #4
    1320:	e1530001 	cmp	r3, r1
    1324:	1afffffc 	bne	131c <efuse_inject+0x11c>
    1328:	e59f1050 	ldr	r1, [pc, #80]	; 1380 <efuse_inject+0x180>
    132c:	e28d2038 	add	r2, sp, #56	; 0x38
    1330:	e28d0032 	add	r0, sp, #50	; 0x32
    1334:	e5d23013 	ldrb	r3, [r2, #19]
    1338:	e2422001 	sub	r2, r2, #1	; 0x1
    133c:	e1520000 	cmp	r2, r0
    1340:	e4813004 	str	r3, [r1], #4
    1344:	1afffffa 	bne	1334 <efuse_inject+0x134>
    1348:	e59f303c 	ldr	r3, [pc, #60]	; 138c <efuse_inject+0x18c>
    134c:	e3a02001 	mov	r2, #1	; 0x1
    1350:	e5832000 	str	r2, [r3]
    1354:	ebfffdfe 	bl	b54 <init_efuse>
    1358:	e3a00000 	mov	r0, #0	; 0x0
    135c:	e28dd058 	add	sp, sp, #88	; 0x58
    1360:	e8bd8010 	pop	{r4, pc}
    1364:	1c723550 	.word	0x1c723550
    1368:	00015f4c 	.word	0x00015f4c
    136c:	20fe074e 	.word	0x20fe074e
    1370:	1698347e 	.word	0x1698347e
    1374:	3fff2c80 	.word	0x3fff2c80
    1378:	21e0e5b4 	.word	0x21e0e5b4
    137c:	21e0e57c 	.word	0x21e0e57c
    1380:	21e0e59c 	.word	0x21e0e59c
    1384:	21e0e47c 	.word	0x21e0e47c
    1388:	21e0e4fc 	.word	0x21e0e4fc
    138c:	21e0d000 	.word	0x21e0d000

00001390 <strcpy>:
    1390:	e3a02000 	mov	r2, #0	; 0x0
    1394:	e7d13002 	ldrb	r3, [r1, r2]
    1398:	e3530000 	cmp	r3, #0	; 0x0
    139c:	e7c03002 	strb	r3, [r0, r2]
    13a0:	e2822001 	add	r2, r2, #1	; 0x1
    13a4:	1afffffa 	bne	1394 <strcpy+0x4>
    13a8:	e12fff1e 	bx	lr

000013ac <strncpy>:
    13ac:	e3a0c000 	mov	ip, #0	; 0x0
    13b0:	e3520000 	cmp	r2, #0	; 0x0
    13b4:	e2422001 	sub	r2, r2, #1	; 0x1
    13b8:	012fff1e 	bxeq	lr
    13bc:	e7d1300c 	ldrb	r3, [r1, ip]
    13c0:	e3530000 	cmp	r3, #0	; 0x0
    13c4:	e7c0300c 	strb	r3, [r0, ip]
    13c8:	e28cc001 	add	ip, ip, #1	; 0x1
    13cc:	1afffff7 	bne	13b0 <strncpy+0x4>
    13d0:	e12fff1e 	bx	lr

000013d4 <strncat>:
    13d4:	e92d4030 	push	{r4, r5, lr}
    13d8:	e2525000 	subs	r5, r2, #0	; 0x0
    13dc:	e1a04001 	mov	r4, r1
    13e0:	11a0c000 	movne	ip, r0
    13e4:	1a000001 	bne	13f0 <strncat+0x1c>
    13e8:	e8bd8030 	pop	{r4, r5, pc}
    13ec:	e28cc001 	add	ip, ip, #1	; 0x1
    13f0:	e5dc3000 	ldrb	r3, [ip]
    13f4:	e3530000 	cmp	r3, #0	; 0x0
    13f8:	1afffffb 	bne	13ec <strncat+0x18>
    13fc:	ea000004 	b	1414 <strncat+0x40>
    1400:	e1550002 	cmp	r5, r2
    1404:	1a000004 	bne	141c <strncat+0x48>
    1408:	e3a03000 	mov	r3, #0	; 0x0
    140c:	e5cc3000 	strb	r3, [ip]
    1410:	e8bd8030 	pop	{r4, r5, pc}
    1414:	e1a02003 	mov	r2, r3
    1418:	e1a0100c 	mov	r1, ip
    141c:	e7d43002 	ldrb	r3, [r4, r2]
    1420:	e28cc001 	add	ip, ip, #1	; 0x1
    1424:	e3530000 	cmp	r3, #0	; 0x0
    1428:	e7c13002 	strb	r3, [r1, r2]
    142c:	e2822001 	add	r2, r2, #1	; 0x1
    1430:	1afffff2 	bne	1400 <strncat+0x2c>
    1434:	e8bd8030 	pop	{r4, r5, pc}

00001438 <strcmp>:
    1438:	e92d4010 	push	{r4, lr}
    143c:	e3a02000 	mov	r2, #0	; 0x0
    1440:	e1a04000 	mov	r4, r0
    1444:	e7d4c002 	ldrb	ip, [r4, r2]
    1448:	e7d13002 	ldrb	r3, [r1, r2]
    144c:	e2822001 	add	r2, r2, #1	; 0x1
    1450:	e063300c 	rsb	r3, r3, ip
    1454:	e6ef0073 	uxtb	r0, r3
    1458:	e3500000 	cmp	r0, #0	; 0x0
    145c:	1a000001 	bne	1468 <strcmp+0x30>
    1460:	e35c0000 	cmp	ip, #0	; 0x0
    1464:	1afffff6 	bne	1444 <strcmp+0xc>
    1468:	e6af0070 	sxtb	r0, r0
    146c:	e8bd8010 	pop	{r4, pc}

00001470 <strncmp>:
    1470:	e92d4030 	push	{r4, r5, lr}
    1474:	e3a0c000 	mov	ip, #0	; 0x0
    1478:	e1a05000 	mov	r5, r0
    147c:	e1a04001 	mov	r4, r1
    1480:	ea000009 	b	14ac <strncmp+0x3c>
    1484:	e7d5100c 	ldrb	r1, [r5, ip]
    1488:	e7d4300c 	ldrb	r3, [r4, ip]
    148c:	e2422001 	sub	r2, r2, #1	; 0x1
    1490:	e0633001 	rsb	r3, r3, r1
    1494:	e6ef0073 	uxtb	r0, r3
    1498:	e3500000 	cmp	r0, #0	; 0x0
    149c:	e28cc001 	add	ip, ip, #1	; 0x1
    14a0:	1a000004 	bne	14b8 <strncmp+0x48>
    14a4:	e3510000 	cmp	r1, #0	; 0x0
    14a8:	0a000002 	beq	14b8 <strncmp+0x48>
    14ac:	e3520000 	cmp	r2, #0	; 0x0
    14b0:	1afffff3 	bne	1484 <strncmp+0x14>
    14b4:	e1a00002 	mov	r0, r2
    14b8:	e6af0070 	sxtb	r0, r0
    14bc:	e8bd8030 	pop	{r4, r5, pc}

000014c0 <strchr>:
    14c0:	e1a03000 	mov	r3, r0
    14c4:	e6ef1071 	uxtb	r1, r1
    14c8:	ea000002 	b	14d8 <strchr+0x18>
    14cc:	e3500000 	cmp	r0, #0	; 0x0
    14d0:	012fff1e 	bxeq	lr
    14d4:	e2833001 	add	r3, r3, #1	; 0x1
    14d8:	e5d30000 	ldrb	r0, [r3]
    14dc:	e1500001 	cmp	r0, r1
    14e0:	1afffff9 	bne	14cc <strchr+0xc>
    14e4:	e1a00003 	mov	r0, r3
    14e8:	e12fff1e 	bx	lr

000014ec <strlen>:
    14ec:	e1a02000 	mov	r2, r0
    14f0:	ea000000 	b	14f8 <strlen+0xc>
    14f4:	e2800001 	add	r0, r0, #1	; 0x1
    14f8:	e5d03000 	ldrb	r3, [r0]
    14fc:	e3530000 	cmp	r3, #0	; 0x0
    1500:	1afffffb 	bne	14f4 <strlen+0x8>
    1504:	e0620000 	rsb	r0, r2, r0
    1508:	e12fff1e 	bx	lr

0000150c <strnlen>:
    150c:	e3a02000 	mov	r2, #0	; 0x0
    1510:	e1510002 	cmp	r1, r2
    1514:	e080c002 	add	ip, r0, r2
    1518:	0a000003 	beq	152c <strnlen+0x20>
    151c:	e7d03002 	ldrb	r3, [r0, r2]
    1520:	e2822001 	add	r2, r2, #1	; 0x1
    1524:	e3530000 	cmp	r3, #0	; 0x0
    1528:	1afffff8 	bne	1510 <strnlen+0x4>
    152c:	e060000c 	rsb	r0, r0, ip
    1530:	e12fff1e 	bx	lr

00001534 <memset>:
    1534:	e3a03000 	mov	r3, #0	; 0x0
    1538:	ea000001 	b	1544 <memset+0x10>
    153c:	e7c01003 	strb	r1, [r0, r3]
    1540:	e2833001 	add	r3, r3, #1	; 0x1
    1544:	e3520000 	cmp	r2, #0	; 0x0
    1548:	e2422001 	sub	r2, r2, #1	; 0x1
    154c:	1afffffa 	bne	153c <memset+0x8>
    1550:	e12fff1e 	bx	lr

00001554 <memcpy>:
    1554:	e3a0c000 	mov	ip, #0	; 0x0
    1558:	ea000002 	b	1568 <memcpy+0x14>
    155c:	e7d1300c 	ldrb	r3, [r1, ip]
    1560:	e7c0300c 	strb	r3, [r0, ip]
    1564:	e28cc001 	add	ip, ip, #1	; 0x1
    1568:	e3520000 	cmp	r2, #0	; 0x0
    156c:	e2422001 	sub	r2, r2, #1	; 0x1
    1570:	1afffff9 	bne	155c <memcpy+0x8>
    1574:	e12fff1e 	bx	lr

00001578 <memmove>:
    1578:	e1500001 	cmp	r0, r1
    157c:	e92d4010 	push	{r4, lr}
    1580:	08bd8010 	popeq	{r4, pc}
    1584:	93a0c000 	movls	ip, #0	; 0x0
    1588:	9a000003 	bls	159c <memmove+0x24>
    158c:	ea000006 	b	15ac <memmove+0x34>
    1590:	e7d1300c 	ldrb	r3, [r1, ip]
    1594:	e7c0300c 	strb	r3, [r0, ip]
    1598:	e28cc001 	add	ip, ip, #1	; 0x1
    159c:	e3520000 	cmp	r2, #0	; 0x0
    15a0:	e2422001 	sub	r2, r2, #1	; 0x1
    15a4:	1afffff9 	bne	1590 <memmove+0x18>
    15a8:	e8bd8010 	pop	{r4, pc}
    15ac:	e0814002 	add	r4, r1, r2
    15b0:	e3a0c000 	mov	ip, #0	; 0x0
    15b4:	e0801002 	add	r1, r0, r2
    15b8:	ea000001 	b	15c4 <memmove+0x4c>
    15bc:	e7d4300c 	ldrb	r3, [r4, ip]
    15c0:	e7c1300c 	strb	r3, [r1, ip]
    15c4:	e3520000 	cmp	r2, #0	; 0x0
    15c8:	e24cc001 	sub	ip, ip, #1	; 0x1
    15cc:	e2422001 	sub	r2, r2, #1	; 0x1
    15d0:	1afffff9 	bne	15bc <memmove+0x44>
    15d4:	e8bd8010 	pop	{r4, pc}

000015d8 <memcmp>:
    15d8:	e92d4030 	push	{r4, r5, lr}
    15dc:	e3a0c000 	mov	ip, #0	; 0x0
    15e0:	e1a04001 	mov	r4, r1
    15e4:	e1a05000 	mov	r5, r0
    15e8:	e1a01002 	mov	r1, r2
    15ec:	ea000005 	b	1608 <memcmp+0x30>
    15f0:	e7d5200c 	ldrb	r2, [r5, ip]
    15f4:	e7d4300c 	ldrb	r3, [r4, ip]
    15f8:	e28cc001 	add	ip, ip, #1	; 0x1
    15fc:	e0520003 	subs	r0, r2, r3
    1600:	18bd8030 	popne	{r4, r5, pc}
    1604:	e2411001 	sub	r1, r1, #1	; 0x1
    1608:	e3510000 	cmp	r1, #0	; 0x0
    160c:	1afffff7 	bne	15f0 <memcmp+0x18>
    1610:	e1a00001 	mov	r0, r1
    1614:	e8bd8030 	pop	{r4, r5, pc}

00001618 <so_m_isused>:
    1618:	e5903004 	ldr	r3, [r0, #4]
    161c:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    1620:	03a00000 	moveq	r0, #0	; 0x0
    1624:	13a00001 	movne	r0, #1	; 0x1
    1628:	e12fff1e 	bx	lr

0000162c <so_m_set_used>:
    162c:	e5903004 	ldr	r3, [r0, #4]
    1630:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    1634:	e5803004 	str	r3, [r0, #4]
    1638:	e12fff1e 	bx	lr

0000163c <so_m_unset_used>:
    163c:	e5903004 	ldr	r3, [r0, #4]
    1640:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1644:	e5803004 	str	r3, [r0, #4]
    1648:	e12fff1e 	bx	lr

0000164c <so_m_get_size>:
    164c:	e5900004 	ldr	r0, [r0, #4]
    1650:	e3c00103 	bic	r0, r0, #-1073741824	; 0xc0000000
    1654:	e12fff1e 	bx	lr

00001658 <so_m_set_size>:
    1658:	e5903004 	ldr	r3, [r0, #4]
    165c:	e3c11103 	bic	r1, r1, #-1073741824	; 0xc0000000
    1660:	e2033103 	and	r3, r3, #-1073741824	; 0xc0000000
    1664:	e1811003 	orr	r1, r1, r3
    1668:	e5801004 	str	r1, [r0, #4]
    166c:	e12fff1e 	bx	lr

00001670 <so_m_alloc>:
    1670:	e59f3048 	ldr	r3, [pc, #72]	; 16c0 <so_m_alloc+0x50>
    1674:	e3a02000 	mov	r2, #0	; 0x0
    1678:	e593c000 	ldr	ip, [r3]
    167c:	e5931008 	ldr	r1, [r3, #8]
    1680:	ea000007 	b	16a4 <so_m_alloc+0x34>
    1684:	e5903004 	ldr	r3, [r0, #4]
    1688:	e3530000 	cmp	r3, #0	; 0x0
    168c:	1a000004 	bne	16a4 <so_m_alloc+0x34>
    1690:	e59f2028 	ldr	r2, [pc, #40]	; 16c0 <so_m_alloc+0x50>
    1694:	e5923004 	ldr	r3, [r2, #4]
    1698:	e2433001 	sub	r3, r3, #1	; 0x1
    169c:	e5823004 	str	r3, [r2, #4]
    16a0:	e12fff1e 	bx	lr
    16a4:	e152000c 	cmp	r2, ip
    16a8:	e1a00001 	mov	r0, r1
    16ac:	e2822001 	add	r2, r2, #1	; 0x1
    16b0:	e2811010 	add	r1, r1, #16	; 0x10
    16b4:	3afffff2 	bcc	1684 <so_m_alloc+0x14>
    16b8:	e3a00000 	mov	r0, #0	; 0x0
    16bc:	e12fff1e 	bx	lr
    16c0:	3c00d70c 	.word	0x3c00d70c

000016c4 <so_m_remove>:
    16c4:	e3a03000 	mov	r3, #0	; 0x0
    16c8:	e59f2014 	ldr	r2, [pc, #20]	; 16e4 <so_m_remove+0x20>
    16cc:	e5803000 	str	r3, [r0]
    16d0:	e5803004 	str	r3, [r0, #4]
    16d4:	e5923004 	ldr	r3, [r2, #4]
    16d8:	e2833001 	add	r3, r3, #1	; 0x1
    16dc:	e5823004 	str	r3, [r2, #4]
    16e0:	e12fff1e 	bx	lr
    16e4:	3c00d70c 	.word	0x3c00d70c

000016e8 <malloc>:
    16e8:	e3500000 	cmp	r0, #0	; 0x0
    16ec:	e92d4070 	push	{r4, r5, r6, lr}
    16f0:	0a000047 	beq	1814 <malloc+0x12c>
    16f4:	e59f1120 	ldr	r1, [pc, #288]	; 181c <malloc+0x134>
    16f8:	e280301f 	add	r3, r0, #31	; 0x1f
    16fc:	e5b12014 	ldr	r2, [r1, #20]!
    1700:	e3c3401f 	bic	r4, r3, #31	; 0x1f
    1704:	e2425008 	sub	r5, r2, #8	; 0x8
    1708:	ea000009 	b	1734 <malloc+0x4c>
    170c:	e5953004 	ldr	r3, [r5, #4]
    1710:	e2130103 	ands	r0, r3, #-1073741824	; 0xc0000000
    1714:	e3c32103 	bic	r2, r3, #-1073741824	; 0xc0000000
    1718:	1a000003 	bne	172c <malloc+0x44>
    171c:	e1520004 	cmp	r2, r4
    1720:	3a000001 	bcc	172c <malloc+0x44>
    1724:	9a000030 	bls	17ec <malloc+0x104>
    1728:	ea000005 	b	1744 <malloc+0x5c>
    172c:	e5953008 	ldr	r3, [r5, #8]
    1730:	e2435008 	sub	r5, r3, #8	; 0x8
    1734:	e2853008 	add	r3, r5, #8	; 0x8
    1738:	e1530001 	cmp	r3, r1
    173c:	1afffff2 	bne	170c <malloc+0x24>
    1740:	ea000033 	b	1814 <malloc+0x12c>
    1744:	e59f30d0 	ldr	r3, [pc, #208]	; 181c <malloc+0x134>
    1748:	e1a01000 	mov	r1, r0
    174c:	e5932008 	ldr	r2, [r3, #8]
    1750:	e5930000 	ldr	r0, [r3]
    1754:	ea00000a 	b	1784 <malloc+0x9c>
    1758:	e5923004 	ldr	r3, [r2, #4]
    175c:	e2822010 	add	r2, r2, #16	; 0x10
    1760:	e3530000 	cmp	r3, #0	; 0x0
    1764:	1a000006 	bne	1784 <malloc+0x9c>
    1768:	e59f20ac 	ldr	r2, [pc, #172]	; 181c <malloc+0x134>
    176c:	e3560000 	cmp	r6, #0	; 0x0
    1770:	e5923004 	ldr	r3, [r2, #4]
    1774:	e2433001 	sub	r3, r3, #1	; 0x1
    1778:	e5823004 	str	r3, [r2, #4]
    177c:	1a000005 	bne	1798 <malloc+0xb0>
    1780:	ea000023 	b	1814 <malloc+0x12c>
    1784:	e1510000 	cmp	r1, r0
    1788:	e1a06002 	mov	r6, r2
    178c:	e2811001 	add	r1, r1, #1	; 0x1
    1790:	3afffff0 	bcc	1758 <malloc+0x70>
    1794:	ea00001e 	b	1814 <malloc+0x12c>
    1798:	e5953004 	ldr	r3, [r5, #4]
    179c:	e5962004 	ldr	r2, [r6, #4]
    17a0:	e0643003 	rsb	r3, r4, r3
    17a4:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    17a8:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    17ac:	e1833002 	orr	r3, r3, r2
    17b0:	e5863004 	str	r3, [r6, #4]
    17b4:	e5951000 	ldr	r1, [r5]
    17b8:	e5950004 	ldr	r0, [r5, #4]
    17bc:	e1a0c005 	mov	ip, r5
    17c0:	e1a03006 	mov	r3, r6
    17c4:	e0841001 	add	r1, r4, r1
    17c8:	e3c42103 	bic	r2, r4, #-1073741824	; 0xc0000000
    17cc:	e2000103 	and	r0, r0, #-1073741824	; 0xc0000000
    17d0:	e5bc4008 	ldr	r4, [ip, #8]!
    17d4:	e4831008 	str	r1, [r3], #8
    17d8:	e1822000 	orr	r2, r2, r0
    17dc:	e5864008 	str	r4, [r6, #8]
    17e0:	e5843004 	str	r3, [r4, #4]
    17e4:	e985000c 	stmib	r5, {r2, r3}
    17e8:	e586c00c 	str	ip, [r6, #12]
    17ec:	e5953004 	ldr	r3, [r5, #4]
    17f0:	e59f1024 	ldr	r1, [pc, #36]	; 181c <malloc+0x134>
    17f4:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    17f8:	e5853004 	str	r3, [r5, #4]
    17fc:	e5912010 	ldr	r2, [r1, #16]
    1800:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1804:	e0833002 	add	r3, r3, r2
    1808:	e5813010 	str	r3, [r1, #16]
    180c:	e5950000 	ldr	r0, [r5]
    1810:	e8bd8070 	pop	{r4, r5, r6, pc}
    1814:	e3a00000 	mov	r0, #0	; 0x0
    1818:	e8bd8070 	pop	{r4, r5, r6, pc}
    181c:	3c00d70c 	.word	0x3c00d70c

00001820 <__sOMERGe>:
    1820:	e5902004 	ldr	r2, [r0, #4]
    1824:	e92d4030 	push	{r4, r5, lr}
    1828:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    182c:	e1a05000 	mov	r5, r0
    1830:	e1a0c001 	mov	ip, r1
    1834:	18bd8030 	popne	{r4, r5, pc}
    1838:	e5913004 	ldr	r3, [r1, #4]
    183c:	e2134103 	ands	r4, r3, #-1073741824	; 0xc0000000
    1840:	18bd8030 	popne	{r4, r5, pc}
    1844:	e0823003 	add	r3, r2, r3
    1848:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    184c:	e59f0030 	ldr	r0, [pc, #48]	; 1884 <__sOMERGe+0x64>
    1850:	e5853004 	str	r3, [r5, #4]
    1854:	e5814000 	str	r4, [r1]
    1858:	e5814004 	str	r4, [r1, #4]
    185c:	e5903004 	ldr	r3, [r0, #4]
    1860:	e591100c 	ldr	r1, [r1, #12]
    1864:	e59c2008 	ldr	r2, [ip, #8]
    1868:	e2833001 	add	r3, r3, #1	; 0x1
    186c:	e5812000 	str	r2, [r1]
    1870:	e5821004 	str	r1, [r2, #4]
    1874:	e5803004 	str	r3, [r0, #4]
    1878:	e58c4008 	str	r4, [ip, #8]
    187c:	e58c400c 	str	r4, [ip, #12]
    1880:	e8bd8030 	pop	{r4, r5, pc}
    1884:	3c00d70c 	.word	0x3c00d70c

00001888 <free>:
    1888:	e3500000 	cmp	r0, #0	; 0x0
    188c:	159f2090 	ldrne	r2, [pc, #144]	; 1924 <free+0x9c>
    1890:	e92d4010 	push	{r4, lr}
    1894:	15b23014 	ldrne	r3, [r2, #20]!
    1898:	1a000004 	bne	18b0 <free+0x28>
    189c:	e8bd8010 	pop	{r4, pc}
    18a0:	e5943000 	ldr	r3, [r4]
    18a4:	e1500003 	cmp	r0, r3
    18a8:	0a000005 	beq	18c4 <free+0x3c>
    18ac:	e5943008 	ldr	r3, [r4, #8]
    18b0:	e2434008 	sub	r4, r3, #8	; 0x8
    18b4:	e2843008 	add	r3, r4, #8	; 0x8
    18b8:	e1530002 	cmp	r3, r2
    18bc:	1afffff7 	bne	18a0 <free+0x18>
    18c0:	e8bd8010 	pop	{r4, pc}
    18c4:	e5943004 	ldr	r3, [r4, #4]
    18c8:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    18cc:	08bd8010 	popeq	{r4, pc}
    18d0:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    18d4:	e59f1048 	ldr	r1, [pc, #72]	; 1924 <free+0x9c>
    18d8:	e5843004 	str	r3, [r4, #4]
    18dc:	e5912010 	ldr	r2, [r1, #16]
    18e0:	e5940008 	ldr	r0, [r4, #8]
    18e4:	e0632002 	rsb	r2, r3, r2
    18e8:	e59f3038 	ldr	r3, [pc, #56]	; 1928 <free+0xa0>
    18ec:	e5812010 	str	r2, [r1, #16]
    18f0:	e1500003 	cmp	r0, r3
    18f4:	0a000002 	beq	1904 <free+0x7c>
    18f8:	e2401008 	sub	r1, r0, #8	; 0x8
    18fc:	e1a00004 	mov	r0, r4
    1900:	ebffffc6 	bl	1820 <__sOMERGe>
    1904:	e594000c 	ldr	r0, [r4, #12]
    1908:	e59f3018 	ldr	r3, [pc, #24]	; 1928 <free+0xa0>
    190c:	e1500003 	cmp	r0, r3
    1910:	08bd8010 	popeq	{r4, pc}
    1914:	e2400008 	sub	r0, r0, #8	; 0x8
    1918:	e1a01004 	mov	r1, r4
    191c:	e8bd4010 	pop	{r4, lr}
    1920:	eaffffbe 	b	1820 <__sOMERGe>
    1924:	3c00d70c 	.word	0x3c00d70c
    1928:	3c00d720 	.word	0x3c00d720

0000192c <__sOINFo>:
    192c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1930:	e59f50a8 	ldr	r5, [pc, #168]	; 19e0 <__sOINFo+0xb4>
    1934:	e24dd004 	sub	sp, sp, #4	; 0x4
    1938:	e59f60a4 	ldr	r6, [pc, #164]	; 19e4 <__sOINFo+0xb8>
    193c:	e8950006 	ldm	r5, {r1, r2}
    1940:	e59f00a0 	ldr	r0, [pc, #160]	; 19e8 <__sOINFo+0xbc>
    1944:	e1a0e00f 	mov	lr, pc
    1948:	e596f000 	ldr	pc, [r6]
    194c:	e2853008 	add	r3, r5, #8	; 0x8
    1950:	e8930098 	ldm	r3, {r3, r4, r7}
    1954:	e2444b01 	sub	r4, r4, #1024	; 0x400
    1958:	e0634004 	rsb	r4, r3, r4
    195c:	e3a00064 	mov	r0, #100	; 0x64
    1960:	e1a01004 	mov	r1, r4
    1964:	e0000790 	mul	r0, r0, r7
    1968:	eb004b87 	bl	1478c <__aeabi_uidiv>
    196c:	e1a01004 	mov	r1, r4
    1970:	e1a03000 	mov	r3, r0
    1974:	e1a02007 	mov	r2, r7
    1978:	e59f006c 	ldr	r0, [pc, #108]	; 19ec <__sOINFo+0xc0>
    197c:	e1a0e00f 	mov	lr, pc
    1980:	e596f000 	ldr	pc, [r6]
    1984:	e59f0064 	ldr	r0, [pc, #100]	; 19f0 <__sOINFo+0xc4>
    1988:	e1a0e00f 	mov	lr, pc
    198c:	e596f000 	ldr	pc, [r6]
    1990:	e59f005c 	ldr	r0, [pc, #92]	; 19f4 <__sOINFo+0xc8>
    1994:	e1a0e00f 	mov	lr, pc
    1998:	e596f000 	ldr	pc, [r6]
    199c:	e5b53014 	ldr	r3, [r5, #20]!
    19a0:	ea000007 	b	19c4 <__sOINFo+0x98>
    19a4:	e8940006 	ldm	r4, {r1, r2}
    19a8:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    19ac:	03a03000 	moveq	r3, #0	; 0x0
    19b0:	13a03001 	movne	r3, #1	; 0x1
    19b4:	e3c22103 	bic	r2, r2, #-1073741824	; 0xc0000000
    19b8:	e1a0e00f 	mov	lr, pc
    19bc:	e596f000 	ldr	pc, [r6]
    19c0:	e5943008 	ldr	r3, [r4, #8]
    19c4:	e2434008 	sub	r4, r3, #8	; 0x8
    19c8:	e2843008 	add	r3, r4, #8	; 0x8
    19cc:	e1530005 	cmp	r3, r5
    19d0:	e59f0020 	ldr	r0, [pc, #32]	; 19f8 <__sOINFo+0xcc>
    19d4:	1afffff2 	bne	19a4 <__sOINFo+0x78>
    19d8:	e28dd004 	add	sp, sp, #4	; 0x4
    19dc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    19e0:	3c00d70c 	.word	0x3c00d70c
    19e4:	3c00d6cc 	.word	0x3c00d6cc
    19e8:	000162a2 	.word	0x000162a2
    19ec:	000162bc 	.word	0x000162bc
    19f0:	000162e1 	.word	0x000162e1
    19f4:	000185c5 	.word	0x000185c5
    19f8:	000162f2 	.word	0x000162f2

000019fc <init_malloc>:
    19fc:	e59f20e4 	ldr	r2, [pc, #228]	; 1ae8 <init_malloc+0xec>
    1a00:	e59f30e4 	ldr	r3, [pc, #228]	; 1aec <init_malloc+0xf0>
    1a04:	e3c2001f 	bic	r0, r2, #31	; 0x1f
    1a08:	e3c3301f 	bic	r3, r3, #31	; 0x1f
    1a0c:	e0602003 	rsb	r2, r0, r3
    1a10:	e92d4010 	push	{r4, lr}
    1a14:	e59f40d4 	ldr	r4, [pc, #212]	; 1af0 <init_malloc+0xf4>
    1a18:	e3520b01 	cmp	r2, #1024	; 0x400
    1a1c:	e5840008 	str	r0, [r4, #8]
    1a20:	e584300c 	str	r3, [r4, #12]
    1a24:	93e00000 	mvnls	r0, #0	; 0x0
    1a28:	98bd8010 	popls	{r4, pc}
    1a2c:	e284c014 	add	ip, r4, #20	; 0x14
    1a30:	e3a03040 	mov	r3, #64	; 0x40
    1a34:	e3a01000 	mov	r1, #0	; 0x0
    1a38:	e3a02b01 	mov	r2, #1024	; 0x400
    1a3c:	e584c018 	str	ip, [r4, #24]
    1a40:	e584c014 	str	ip, [r4, #20]
    1a44:	e5843004 	str	r3, [r4, #4]
    1a48:	e5843000 	str	r3, [r4]
    1a4c:	ebfffeb8 	bl	1534 <memset>
    1a50:	e5940008 	ldr	r0, [r4, #8]
    1a54:	e594c000 	ldr	ip, [r4]
    1a58:	e1a01000 	mov	r1, r0
    1a5c:	e3a02000 	mov	r2, #0	; 0x0
    1a60:	ea000006 	b	1a80 <init_malloc+0x84>
    1a64:	e59e3004 	ldr	r3, [lr, #4]
    1a68:	e3530000 	cmp	r3, #0	; 0x0
    1a6c:	059f207c 	ldreq	r2, [pc, #124]	; 1af0 <init_malloc+0xf4>
    1a70:	05923004 	ldreq	r3, [r2, #4]
    1a74:	02433001 	subeq	r3, r3, #1	; 0x1
    1a78:	05823004 	streq	r3, [r2, #4]
    1a7c:	0a000005 	beq	1a98 <init_malloc+0x9c>
    1a80:	e152000c 	cmp	r2, ip
    1a84:	e1a0e001 	mov	lr, r1
    1a88:	e2822001 	add	r2, r2, #1	; 0x1
    1a8c:	e2811010 	add	r1, r1, #16	; 0x10
    1a90:	3afffff3 	bcc	1a64 <init_malloc+0x68>
    1a94:	e3a0e000 	mov	lr, #0	; 0x0
    1a98:	e59fc050 	ldr	ip, [pc, #80]	; 1af0 <init_malloc+0xf4>
    1a9c:	e59e2004 	ldr	r2, [lr, #4]
    1aa0:	e59c300c 	ldr	r3, [ip, #12]
    1aa4:	e1a0100e 	mov	r1, lr
    1aa8:	e2433b01 	sub	r3, r3, #1024	; 0x400
    1aac:	e0603003 	rsb	r3, r0, r3
    1ab0:	e59c4018 	ldr	r4, [ip, #24]
    1ab4:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    1ab8:	e2800b01 	add	r0, r0, #1024	; 0x400
    1abc:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1ac0:	e4810008 	str	r0, [r1], #8
    1ac4:	e1833002 	orr	r3, r3, r2
    1ac8:	e28c2014 	add	r2, ip, #20	; 0x14
    1acc:	e58e2008 	str	r2, [lr, #8]
    1ad0:	e58c1018 	str	r1, [ip, #24]
    1ad4:	e5841000 	str	r1, [r4]
    1ad8:	e58e3004 	str	r3, [lr, #4]
    1adc:	e58e400c 	str	r4, [lr, #12]
    1ae0:	e3a00000 	mov	r0, #0	; 0x0
    1ae4:	e8bd8010 	pop	{r4, pc}
    1ae8:	3c00e01f 	.word	0x3c00e01f
    1aec:	3c018000 	.word	0x3c018000
    1af0:	3c00d70c 	.word	0x3c00d70c

00001af4 <isi_check_header>:
    1af4:	e92d4070 	push	{r4, r5, r6, lr}
    1af8:	e5902000 	ldr	r2, [r0]
    1afc:	e59f3088 	ldr	r3, [pc, #136]	; 1b8c <isi_check_header+0x98>
    1b00:	e1a05000 	mov	r5, r0
    1b04:	e1520003 	cmp	r2, r3
    1b08:	e24dd008 	sub	sp, sp, #8	; 0x8
    1b0c:	13e0001e 	mvnne	r0, #30	; 0x1e
    1b10:	1a00001b 	bne	1b84 <isi_check_header+0x90>
    1b14:	e3a06000 	mov	r6, #0	; 0x0
    1b18:	e595401c 	ldr	r4, [r5, #28]
    1b1c:	e3a01c02 	mov	r1, #512	; 0x200
    1b20:	e585601c 	str	r6, [r5, #28]
    1b24:	e1a00006 	mov	r0, r6
    1b28:	eb0013ea 	bl	6ad8 <hash_init>
    1b2c:	e3a01c02 	mov	r1, #512	; 0x200
    1b30:	e1a00005 	mov	r0, r5
    1b34:	eb0013c2 	bl	6a44 <hash_data>
    1b38:	e28d0004 	add	r0, sp, #4	; 0x4
    1b3c:	eb0013a4 	bl	69d4 <hash_value>
    1b40:	e585401c 	str	r4, [r5, #28]
    1b44:	e59d3004 	ldr	r3, [sp, #4]
    1b48:	e1540003 	cmp	r4, r3
    1b4c:	13e0001f 	mvnne	r0, #31	; 0x1f
    1b50:	1a00000b 	bne	1b84 <isi_check_header+0x90>
    1b54:	e5951004 	ldr	r1, [r5, #4]
    1b58:	e59f4030 	ldr	r4, [pc, #48]	; 1b90 <isi_check_header+0x9c>
    1b5c:	e3c114ff 	bic	r1, r1, #-16777216	; 0xff000000
    1b60:	e59f002c 	ldr	r0, [pc, #44]	; 1b94 <isi_check_header+0xa0>
    1b64:	e1a0e00f 	mov	lr, pc
    1b68:	e594f000 	ldr	pc, [r4]
    1b6c:	e5d51007 	ldrb	r1, [r5, #7]
    1b70:	e59f0020 	ldr	r0, [pc, #32]	; 1b98 <isi_check_header+0xa4>
    1b74:	e201100f 	and	r1, r1, #15	; 0xf
    1b78:	e1a0e00f 	mov	lr, pc
    1b7c:	e594f000 	ldr	pc, [r4]
    1b80:	e1a00006 	mov	r0, r6
    1b84:	e28dd008 	add	sp, sp, #8	; 0x8
    1b88:	e8bd8070 	pop	{r4, r5, r6, pc}
    1b8c:	6973692b 	.word	0x6973692b
    1b90:	3c00d6cc 	.word	0x3c00d6cc
    1b94:	0001631a 	.word	0x0001631a
    1b98:	00016328 	.word	0x00016328

00001b9c <isi_check_data_separate>:
    1b9c:	e92d4070 	push	{r4, r5, r6, lr}
    1ba0:	e1a05000 	mov	r5, r0
    1ba4:	e5d00007 	ldrb	r0, [r0, #7]
    1ba8:	e24dd028 	sub	sp, sp, #40	; 0x28
    1bac:	e1a04001 	mov	r4, r1
    1bb0:	e200000f 	and	r0, r0, #15	; 0xf
    1bb4:	e595100c 	ldr	r1, [r5, #12]
    1bb8:	eb0013c6 	bl	6ad8 <hash_init>
    1bbc:	e28d6008 	add	r6, sp, #8	; 0x8
    1bc0:	e595100c 	ldr	r1, [r5, #12]
    1bc4:	e1a00004 	mov	r0, r4
    1bc8:	eb00139d 	bl	6a44 <hash_data>
    1bcc:	e1a00006 	mov	r0, r6
    1bd0:	eb00137f 	bl	69d4 <hash_value>
    1bd4:	eb001377 	bl	69b8 <hash_len>
    1bd8:	e2854020 	add	r4, r5, #32	; 0x20
    1bdc:	e1a02000 	mov	r2, r0
    1be0:	e1a01004 	mov	r1, r4
    1be4:	e1a00006 	mov	r0, r6
    1be8:	ebfffe7a 	bl	15d8 <memcmp>
    1bec:	e3500000 	cmp	r0, #0	; 0x0
    1bf0:	0a000014 	beq	1c48 <isi_check_data_separate+0xac>
    1bf4:	e595100c 	ldr	r1, [r5, #12]
    1bf8:	e59f3050 	ldr	r3, [pc, #80]	; 1c50 <isi_check_data_separate+0xb4>
    1bfc:	e59f0050 	ldr	r0, [pc, #80]	; 1c54 <isi_check_data_separate+0xb8>
    1c00:	e1a0e00f 	mov	lr, pc
    1c04:	e593f000 	ldr	pc, [r3]
    1c08:	eb00136a 	bl	69b8 <hash_len>
    1c0c:	e1a01004 	mov	r1, r4
    1c10:	e1a03000 	mov	r3, r0
    1c14:	e3a02001 	mov	r2, #1	; 0x1
    1c18:	e1a00004 	mov	r0, r4
    1c1c:	e3a04020 	mov	r4, #32	; 0x20
    1c20:	e58d4000 	str	r4, [sp]
    1c24:	eb003e3c 	bl	1151c <print_buffer>
    1c28:	eb001362 	bl	69b8 <hash_len>
    1c2c:	e1a01006 	mov	r1, r6
    1c30:	e1a03000 	mov	r3, r0
    1c34:	e3a02001 	mov	r2, #1	; 0x1
    1c38:	e1a00006 	mov	r0, r6
    1c3c:	e58d4000 	str	r4, [sp]
    1c40:	eb003e35 	bl	1151c <print_buffer>
    1c44:	e3e00020 	mvn	r0, #32	; 0x20
    1c48:	e28dd028 	add	sp, sp, #40	; 0x28
    1c4c:	e8bd8070 	pop	{r4, r5, r6, pc}
    1c50:	3c00d6cc 	.word	0x3c00d6cc
    1c54:	0001633b 	.word	0x0001633b

00001c58 <isi_check_data>:
    1c58:	e2801c02 	add	r1, r0, #512	; 0x200
    1c5c:	eaffffce 	b	1b9c <isi_check_data_separate>

00001c60 <isi_check>:
    1c60:	e92d4010 	push	{r4, lr}
    1c64:	e1a04000 	mov	r4, r0
    1c68:	ebffffa1 	bl	1af4 <isi_check_header>
    1c6c:	e3500000 	cmp	r0, #0	; 0x0
    1c70:	18bd8010 	popne	{r4, pc}
    1c74:	e1a00004 	mov	r0, r4
    1c78:	e8bd4010 	pop	{r4, lr}
    1c7c:	eafffff5 	b	1c58 <isi_check_data>

00001c80 <load_serial_ymodem>:
    1c80:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    1c84:	e24dde41 	sub	sp, sp, #1040	; 0x410
    1c88:	e28d4b01 	add	r4, sp, #1024	; 0x400
    1c8c:	e284400c 	add	r4, r4, #12	; 0xc
    1c90:	e3a03002 	mov	r3, #2	; 0x2
    1c94:	e1a07000 	mov	r7, r0
    1c98:	e1a01004 	mov	r1, r4
    1c9c:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1ca0:	e58d3404 	str	r3, [sp, #1028]
    1ca4:	eb004a3e 	bl	145a4 <xyzModem_stream_open>
    1ca8:	e3500000 	cmp	r0, #0	; 0x0
    1cac:	1a000012 	bne	1cfc <load_serial_ymodem+0x7c>
    1cb0:	e1a08004 	mov	r8, r4
    1cb4:	e1a05000 	mov	r5, r0
    1cb8:	e1a04000 	mov	r4, r0
    1cbc:	e1a0600d 	mov	r6, sp
    1cc0:	ea000001 	b	1ccc <load_serial_ymodem+0x4c>
    1cc4:	e085500c 	add	r5, r5, ip
    1cc8:	ebfffe21 	bl	1554 <memcpy>
    1ccc:	e3a01b01 	mov	r1, #1024	; 0x400
    1cd0:	e1a02008 	mov	r2, r8
    1cd4:	e1a0000d 	mov	r0, sp
    1cd8:	eb00499d 	bl	14354 <xyzModem_stream_read>
    1cdc:	e0843007 	add	r3, r4, r7
    1ce0:	e250c000 	subs	ip, r0, #0	; 0x0
    1ce4:	e1a0100d 	mov	r1, sp
    1ce8:	e1a0200c 	mov	r2, ip
    1cec:	e1a00003 	mov	r0, r3
    1cf0:	e084400c 	add	r4, r4, ip
    1cf4:	cafffff2 	bgt	1cc4 <load_serial_ymodem+0x44>
    1cf8:	ea000007 	b	1d1c <load_serial_ymodem+0x9c>
    1cfc:	e59f3054 	ldr	r3, [pc, #84]	; 1d58 <load_serial_ymodem+0xd8>
    1d00:	e59d040c 	ldr	r0, [sp, #1036]
    1d04:	e5934000 	ldr	r4, [r3]
    1d08:	eb004618 	bl	13570 <xyzModem_error>
    1d0c:	e1a01000 	mov	r1, r0
    1d10:	e59f0044 	ldr	r0, [pc, #68]	; 1d5c <load_serial_ymodem+0xdc>
    1d14:	e12fff34 	blx	r4
    1d18:	e3a05000 	mov	r5, #0	; 0x0
    1d1c:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1d20:	e280000c 	add	r0, r0, #12	; 0xc
    1d24:	eb0045f9 	bl	13510 <xyzModem_stream_close>
    1d28:	e3a00000 	mov	r0, #0	; 0x0
    1d2c:	e59f102c 	ldr	r1, [pc, #44]	; 1d60 <load_serial_ymodem+0xe0>
    1d30:	eb0048a7 	bl	13fd4 <xyzModem_stream_terminate>
    1d34:	e1a01005 	mov	r1, r5
    1d38:	e59f0024 	ldr	r0, [pc, #36]	; 1d64 <load_serial_ymodem+0xe4>
    1d3c:	e1a02005 	mov	r2, r5
    1d40:	e59f3010 	ldr	r3, [pc, #16]	; 1d58 <load_serial_ymodem+0xd8>
    1d44:	e1a0e00f 	mov	lr, pc
    1d48:	e593f000 	ldr	pc, [r3]
    1d4c:	e1a00007 	mov	r0, r7
    1d50:	e28dde41 	add	sp, sp, #1040	; 0x410
    1d54:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    1d58:	3c00d6cc 	.word	0x3c00d6cc
    1d5c:	00016437 	.word	0x00016437
    1d60:	00001d68 	.word	0x00001d68
    1d64:	0001635f 	.word	0x0001635f

00001d68 <getcxmodem>:
    1d68:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    1d6c:	e24dd004 	sub	sp, sp, #4	; 0x4
    1d70:	eb004298 	bl	127d8 <tstc>
    1d74:	e3500000 	cmp	r0, #0	; 0x0
    1d78:	1a000002 	bne	1d88 <getcxmodem+0x20>
    1d7c:	e3e00000 	mvn	r0, #0	; 0x0
    1d80:	e28dd004 	add	sp, sp, #4	; 0x4
    1d84:	e8bd8000 	pop	{pc}
    1d88:	e28dd004 	add	sp, sp, #4	; 0x4
    1d8c:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    1d90:	ea004291 	b	127dc <getc>

00001d94 <do_load_serial_bin>:
    1d94:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1d98:	e3520001 	cmp	r2, #1	; 0x1
    1d9c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1da0:	e1a06002 	mov	r6, r2
    1da4:	e1a07000 	mov	r7, r0
    1da8:	e1a05003 	mov	r5, r3
    1dac:	ca000006 	bgt	1dcc <do_load_serial_bin+0x38>
    1db0:	e59f0120 	ldr	r0, [pc, #288]	; 1ed8 <do_load_serial_bin+0x144>
    1db4:	e3a06000 	mov	r6, #0	; 0x0
    1db8:	e59f311c 	ldr	r3, [pc, #284]	; 1edc <do_load_serial_bin+0x148>
    1dbc:	e1a0e00f 	mov	lr, pc
    1dc0:	e593f000 	ldr	pc, [r3]
    1dc4:	e1a04006 	mov	r4, r6
    1dc8:	ea00000c 	b	1e00 <do_load_serial_bin+0x6c>
    1dcc:	e5930004 	ldr	r0, [r3, #4]
    1dd0:	e3a01000 	mov	r1, #0	; 0x0
    1dd4:	e3a02010 	mov	r2, #16	; 0x10
    1dd8:	eb003e67 	bl	1177c <simple_strtoul>
    1ddc:	e3560003 	cmp	r6, #3	; 0x3
    1de0:	e1a04000 	mov	r4, r0
    1de4:	13a06000 	movne	r6, #0	; 0x0
    1de8:	1a000004 	bne	1e00 <do_load_serial_bin+0x6c>
    1dec:	e5950008 	ldr	r0, [r5, #8]
    1df0:	e3a01000 	mov	r1, #0	; 0x0
    1df4:	e3a02010 	mov	r2, #16	; 0x10
    1df8:	eb003e5f 	bl	1177c <simple_strtoul>
    1dfc:	e1a06000 	mov	r6, r0
    1e00:	e5950000 	ldr	r0, [r5]
    1e04:	e59f10d4 	ldr	r1, [pc, #212]	; 1ee0 <do_load_serial_bin+0x14c>
    1e08:	ebfffd8a 	bl	1438 <strcmp>
    1e0c:	e3500000 	cmp	r0, #0	; 0x0
    1e10:	1a000007 	bne	1e34 <do_load_serial_bin+0xa0>
    1e14:	e1a01004 	mov	r1, r4
    1e18:	e59f00c4 	ldr	r0, [pc, #196]	; 1ee4 <do_load_serial_bin+0x150>
    1e1c:	e59f30b8 	ldr	r3, [pc, #184]	; 1edc <do_load_serial_bin+0x148>
    1e20:	e1a0e00f 	mov	lr, pc
    1e24:	e593f000 	ldr	pc, [r3]
    1e28:	e1a00004 	mov	r0, r4
    1e2c:	ebffff93 	bl	1c80 <load_serial_ymodem>
    1e30:	ea000025 	b	1ecc <do_load_serial_bin+0x138>
    1e34:	e5950000 	ldr	r0, [r5]
    1e38:	e59f10a8 	ldr	r1, [pc, #168]	; 1ee8 <do_load_serial_bin+0x154>
    1e3c:	ebfffd7d 	bl	1438 <strcmp>
    1e40:	e3500000 	cmp	r0, #0	; 0x0
    1e44:	e59f5090 	ldr	r5, [pc, #144]	; 1edc <do_load_serial_bin+0x148>
    1e48:	1a00001b 	bne	1ebc <do_load_serial_bin+0x128>
    1e4c:	e1a02006 	mov	r2, r6
    1e50:	e59f0094 	ldr	r0, [pc, #148]	; 1eec <do_load_serial_bin+0x158>
    1e54:	e1a01004 	mov	r1, r4
    1e58:	e1a0e00f 	mov	lr, pc
    1e5c:	e595f000 	ldr	pc, [r5]
    1e60:	e1a00004 	mov	r0, r4
    1e64:	e1a01006 	mov	r1, r6
    1e68:	eb0047bc 	bl	13d60 <control_send>
    1e6c:	e3500000 	cmp	r0, #0	; 0x0
    1e70:	aa000005 	bge	1e8c <do_load_serial_bin+0xf8>
    1e74:	e59d0004 	ldr	r0, [sp, #4]
    1e78:	e5954000 	ldr	r4, [r5]
    1e7c:	eb0045bb 	bl	13570 <xyzModem_error>
    1e80:	e1a01000 	mov	r1, r0
    1e84:	e59f0064 	ldr	r0, [pc, #100]	; 1ef0 <do_load_serial_bin+0x15c>
    1e88:	e12fff34 	blx	r4
    1e8c:	e28d0004 	add	r0, sp, #4	; 0x4
    1e90:	eb00459e 	bl	13510 <xyzModem_stream_close>
    1e94:	e3a00000 	mov	r0, #0	; 0x0
    1e98:	e59f1054 	ldr	r1, [pc, #84]	; 1ef4 <do_load_serial_bin+0x160>
    1e9c:	eb00484c 	bl	13fd4 <xyzModem_stream_terminate>
    1ea0:	e1a01006 	mov	r1, r6
    1ea4:	e59f004c 	ldr	r0, [pc, #76]	; 1ef8 <do_load_serial_bin+0x164>
    1ea8:	e1a02006 	mov	r2, r6
    1eac:	e59f3028 	ldr	r3, [pc, #40]	; 1edc <do_load_serial_bin+0x148>
    1eb0:	e1a0e00f 	mov	lr, pc
    1eb4:	e593f000 	ldr	pc, [r3]
    1eb8:	ea000003 	b	1ecc <do_load_serial_bin+0x138>
    1ebc:	e5971014 	ldr	r1, [r7, #20]
    1ec0:	e59f0034 	ldr	r0, [pc, #52]	; 1efc <do_load_serial_bin+0x168>
    1ec4:	e1a0e00f 	mov	lr, pc
    1ec8:	e595f000 	ldr	pc, [r5]
    1ecc:	e3a00000 	mov	r0, #0	; 0x0
    1ed0:	e28dd00c 	add	sp, sp, #12	; 0xc
    1ed4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1ed8:	00016387 	.word	0x00016387
    1edc:	3c00d6cc 	.word	0x3c00d6cc
    1ee0:	000163a1 	.word	0x000163a1
    1ee4:	000163a4 	.word	0x000163a4
    1ee8:	000163e7 	.word	0x000163e7
    1eec:	000163ea 	.word	0x000163ea
    1ef0:	00016437 	.word	0x00016437
    1ef4:	00001d68 	.word	0x00001d68
    1ef8:	0001635f 	.word	0x0001635f
    1efc:	00016436 	.word	0x00016436

00001f00 <do_mem_cp>:
    1f00:	e3520004 	cmp	r2, #4	; 0x4
    1f04:	e92d4070 	push	{r4, r5, r6, lr}
    1f08:	e1a04003 	mov	r4, r3
    1f0c:	0a000002 	beq	1f1c <do_mem_cp+0x1c>
    1f10:	eb0003e2 	bl	2ea0 <cmd_usage>
    1f14:	e3a00001 	mov	r0, #1	; 0x1
    1f18:	e8bd8070 	pop	{r4, r5, r6, pc}
    1f1c:	e5930004 	ldr	r0, [r3, #4]
    1f20:	e3a01000 	mov	r1, #0	; 0x0
    1f24:	e3a02010 	mov	r2, #16	; 0x10
    1f28:	eb003e13 	bl	1177c <simple_strtoul>
    1f2c:	e3a01000 	mov	r1, #0	; 0x0
    1f30:	e3a02010 	mov	r2, #16	; 0x10
    1f34:	e1a06000 	mov	r6, r0
    1f38:	e5940008 	ldr	r0, [r4, #8]
    1f3c:	eb003e0e 	bl	1177c <simple_strtoul>
    1f40:	e3a01000 	mov	r1, #0	; 0x0
    1f44:	e1a05000 	mov	r5, r0
    1f48:	e3a02010 	mov	r2, #16	; 0x10
    1f4c:	e594000c 	ldr	r0, [r4, #12]
    1f50:	eb003e09 	bl	1177c <simple_strtoul>
    1f54:	e2800003 	add	r0, r0, #3	; 0x3
    1f58:	e1b00120 	lsrs	r0, r0, #2
    1f5c:	1a000002 	bne	1f6c <do_mem_cp+0x6c>
    1f60:	ea000006 	b	1f80 <do_mem_cp+0x80>
    1f64:	e5163004 	ldr	r3, [r6, #-4]
    1f68:	e5053004 	str	r3, [r5, #-4]
    1f6c:	e3500000 	cmp	r0, #0	; 0x0
    1f70:	e2866004 	add	r6, r6, #4	; 0x4
    1f74:	e2400001 	sub	r0, r0, #1	; 0x1
    1f78:	e2855004 	add	r5, r5, #4	; 0x4
    1f7c:	1afffff8 	bne	1f64 <do_mem_cp+0x64>
    1f80:	e3a00000 	mov	r0, #0	; 0x0
    1f84:	e8bd8070 	pop	{r4, r5, r6, pc}

00001f88 <do_mem_cmp>:
    1f88:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1f8c:	e3520004 	cmp	r2, #4	; 0x4
    1f90:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1f94:	e1a04003 	mov	r4, r3
    1f98:	0a000002 	beq	1fa8 <do_mem_cmp+0x20>
    1f9c:	eb0003bf 	bl	2ea0 <cmd_usage>
    1fa0:	e3a04001 	mov	r4, #1	; 0x1
    1fa4:	ea000029 	b	2050 <do_mem_cmp+0xc8>
    1fa8:	e5930004 	ldr	r0, [r3, #4]
    1fac:	e3a01000 	mov	r1, #0	; 0x0
    1fb0:	e3a02010 	mov	r2, #16	; 0x10
    1fb4:	eb003df0 	bl	1177c <simple_strtoul>
    1fb8:	e3a01000 	mov	r1, #0	; 0x0
    1fbc:	e3a02010 	mov	r2, #16	; 0x10
    1fc0:	e1a07000 	mov	r7, r0
    1fc4:	e5940008 	ldr	r0, [r4, #8]
    1fc8:	eb003deb 	bl	1177c <simple_strtoul>
    1fcc:	e3a01000 	mov	r1, #0	; 0x0
    1fd0:	e1a06000 	mov	r6, r0
    1fd4:	e3a02010 	mov	r2, #16	; 0x10
    1fd8:	e594000c 	ldr	r0, [r4, #12]
    1fdc:	eb003de6 	bl	1177c <simple_strtoul>
    1fe0:	e2800003 	add	r0, r0, #3	; 0x3
    1fe4:	e1a00120 	lsr	r0, r0, #2
    1fe8:	e3a05000 	mov	r5, #0	; 0x0
    1fec:	ea00000f 	b	2030 <do_mem_cmp+0xa8>
    1ff0:	e5972000 	ldr	r2, [r7]
    1ff4:	e5963000 	ldr	r3, [r6]
    1ff8:	e1520003 	cmp	r2, r3
    1ffc:	0a000008 	beq	2024 <do_mem_cmp+0x9c>
    2000:	e58d3000 	str	r3, [sp]
    2004:	e1a01007 	mov	r1, r7
    2008:	e1a03006 	mov	r3, r6
    200c:	e59f0048 	ldr	r0, [pc, #72]	; 205c <do_mem_cmp+0xd4>
    2010:	e59fc048 	ldr	ip, [pc, #72]	; 2060 <do_mem_cmp+0xd8>
    2014:	e1a0e00f 	mov	lr, pc
    2018:	e59cf000 	ldr	pc, [ip]
    201c:	e3a04001 	mov	r4, #1	; 0x1
    2020:	ea000005 	b	203c <do_mem_cmp+0xb4>
    2024:	e2855001 	add	r5, r5, #1	; 0x1
    2028:	e2877004 	add	r7, r7, #4	; 0x4
    202c:	e2866004 	add	r6, r6, #4	; 0x4
    2030:	e1500005 	cmp	r0, r5
    2034:	1affffed 	bne	1ff0 <do_mem_cmp+0x68>
    2038:	e3a04000 	mov	r4, #0	; 0x0
    203c:	e1a01005 	mov	r1, r5
    2040:	e59f001c 	ldr	r0, [pc, #28]	; 2064 <do_mem_cmp+0xdc>
    2044:	e59f3014 	ldr	r3, [pc, #20]	; 2060 <do_mem_cmp+0xd8>
    2048:	e1a0e00f 	mov	lr, pc
    204c:	e593f000 	ldr	pc, [r3]
    2050:	e1a00004 	mov	r0, r4
    2054:	e28dd00c 	add	sp, sp, #12	; 0xc
    2058:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    205c:	0001653c 	.word	0x0001653c
    2060:	3c00d6cc 	.word	0x3c00d6cc
    2064:	00016574 	.word	0x00016574

00002068 <do_mem_mw>:
    2068:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    206c:	e1a06002 	mov	r6, r2
    2070:	e2422003 	sub	r2, r2, #3	; 0x3
    2074:	e3520001 	cmp	r2, #1	; 0x1
    2078:	e24dd004 	sub	sp, sp, #4	; 0x4
    207c:	e1a04003 	mov	r4, r3
    2080:	9a000002 	bls	2090 <do_mem_mw+0x28>
    2084:	eb000385 	bl	2ea0 <cmd_usage>
    2088:	e3a00001 	mov	r0, #1	; 0x1
    208c:	ea000016 	b	20ec <do_mem_mw+0x84>
    2090:	e3a01000 	mov	r1, #0	; 0x0
    2094:	e3a02010 	mov	r2, #16	; 0x10
    2098:	e5930004 	ldr	r0, [r3, #4]
    209c:	eb003db6 	bl	1177c <simple_strtoul>
    20a0:	e3a01000 	mov	r1, #0	; 0x0
    20a4:	e1a05000 	mov	r5, r0
    20a8:	e3a02010 	mov	r2, #16	; 0x10
    20ac:	e5940008 	ldr	r0, [r4, #8]
    20b0:	eb003db1 	bl	1177c <simple_strtoul>
    20b4:	e3560004 	cmp	r6, #4	; 0x4
    20b8:	e1a07000 	mov	r7, r0
    20bc:	13a00001 	movne	r0, #1	; 0x1
    20c0:	1a000006 	bne	20e0 <do_mem_mw+0x78>
    20c4:	e594000c 	ldr	r0, [r4, #12]
    20c8:	e3a01000 	mov	r1, #0	; 0x0
    20cc:	e3a02010 	mov	r2, #16	; 0x10
    20d0:	eb003da9 	bl	1177c <simple_strtoul>
    20d4:	ea000001 	b	20e0 <do_mem_mw+0x78>
    20d8:	e5057004 	str	r7, [r5, #-4]
    20dc:	e2400001 	sub	r0, r0, #1	; 0x1
    20e0:	e3500000 	cmp	r0, #0	; 0x0
    20e4:	e2855004 	add	r5, r5, #4	; 0x4
    20e8:	1afffffa 	bne	20d8 <do_mem_mw+0x70>
    20ec:	e28dd004 	add	sp, sp, #4	; 0x4
    20f0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

000020f4 <do_mem_md>:
    20f4:	e92d4070 	push	{r4, r5, r6, lr}
    20f8:	e3520001 	cmp	r2, #1	; 0x1
    20fc:	e24dd008 	sub	sp, sp, #8	; 0x8
    2100:	e1a04002 	mov	r4, r2
    2104:	e1a05003 	mov	r5, r3
    2108:	ca000002 	bgt	2118 <do_mem_md+0x24>
    210c:	eb000363 	bl	2ea0 <cmd_usage>
    2110:	e3a00001 	mov	r0, #1	; 0x1
    2114:	ea000014 	b	216c <do_mem_md+0x78>
    2118:	e5930004 	ldr	r0, [r3, #4]
    211c:	e3a01000 	mov	r1, #0	; 0x0
    2120:	e3a02010 	mov	r2, #16	; 0x10
    2124:	eb003d94 	bl	1177c <simple_strtoul>
    2128:	e3540002 	cmp	r4, #2	; 0x2
    212c:	e1a06000 	mov	r6, r0
    2130:	03a030a0 	moveq	r3, #160	; 0xa0
    2134:	0a000004 	beq	214c <do_mem_md+0x58>
    2138:	e5950008 	ldr	r0, [r5, #8]
    213c:	e3a01000 	mov	r1, #0	; 0x0
    2140:	e3a02010 	mov	r2, #16	; 0x10
    2144:	eb003d8c 	bl	1177c <simple_strtoul>
    2148:	e1a03000 	mov	r3, r0
    214c:	e3a0c004 	mov	ip, #4	; 0x4
    2150:	e1a00006 	mov	r0, r6
    2154:	e1a0200c 	mov	r2, ip
    2158:	e1a03123 	lsr	r3, r3, #2
    215c:	e1a01006 	mov	r1, r6
    2160:	e58dc000 	str	ip, [sp]
    2164:	eb003cec 	bl	1151c <print_buffer>
    2168:	e3a00000 	mov	r0, #0	; 0x0
    216c:	e28dd008 	add	sp, sp, #8	; 0x8
    2170:	e8bd8070 	pop	{r4, r5, r6, pc}

00002174 <do_nand>:
    2174:	e3520001 	cmp	r2, #1	; 0x1
    2178:	e92d4070 	push	{r4, r5, r6, lr}
    217c:	e1a06002 	mov	r6, r2
    2180:	e1a05003 	mov	r5, r3
    2184:	da000020 	ble	220c <do_nand+0x98>
    2188:	e5930004 	ldr	r0, [r3, #4]
    218c:	e59f10a0 	ldr	r1, [pc, #160]	; 2234 <do_nand+0xc0>
    2190:	e3a02007 	mov	r2, #7	; 0x7
    2194:	ebfffcb5 	bl	1470 <strncmp>
    2198:	e2504000 	subs	r4, r0, #0	; 0x0
    219c:	1a000001 	bne	21a8 <do_nand+0x34>
    21a0:	eb0026f6 	bl	bd80 <ecc_print_stat>
    21a4:	ea000009 	b	21d0 <do_nand+0x5c>
    21a8:	e5950004 	ldr	r0, [r5, #4]
    21ac:	e59f1084 	ldr	r1, [pc, #132]	; 2238 <do_nand+0xc4>
    21b0:	e3a02003 	mov	r2, #3	; 0x3
    21b4:	ebfffcad 	bl	1470 <strncmp>
    21b8:	e2504000 	subs	r4, r0, #0	; 0x0
    21bc:	1a000005 	bne	21d8 <do_nand+0x64>
    21c0:	e3560002 	cmp	r6, #2	; 0x2
    21c4:	d3a00000 	movle	r0, #0	; 0x0
    21c8:	c3a00001 	movgt	r0, #1	; 0x1
    21cc:	eb0022e5 	bl	ad68 <base_scan_badblocks>
    21d0:	e1a00004 	mov	r0, r4
    21d4:	e8bd8070 	pop	{r4, r5, r6, pc}
    21d8:	e5950004 	ldr	r0, [r5, #4]
    21dc:	e59f1058 	ldr	r1, [pc, #88]	; 223c <do_nand+0xc8>
    21e0:	e3a02007 	mov	r2, #7	; 0x7
    21e4:	ebfffca1 	bl	1470 <strncmp>
    21e8:	e2504000 	subs	r4, r0, #0	; 0x0
    21ec:	13a00000 	movne	r0, #0	; 0x0
    21f0:	18bd8070 	popne	{r4, r5, r6, pc}
    21f4:	e3560002 	cmp	r6, #2	; 0x2
    21f8:	1a000005 	bne	2214 <do_nand+0xa0>
    21fc:	e59f003c 	ldr	r0, [pc, #60]	; 2240 <do_nand+0xcc>
    2200:	e59f303c 	ldr	r3, [pc, #60]	; 2244 <do_nand+0xd0>
    2204:	e1a0e00f 	mov	lr, pc
    2208:	e593f000 	ldr	pc, [r3]
    220c:	e3e00000 	mvn	r0, #0	; 0x0
    2210:	e8bd8070 	pop	{r4, r5, r6, pc}
    2214:	e3a02010 	mov	r2, #16	; 0x10
    2218:	e1a01004 	mov	r1, r4
    221c:	e5950008 	ldr	r0, [r5, #8]
    2220:	eb003d55 	bl	1177c <simple_strtoul>
    2224:	e1a01004 	mov	r1, r4
    2228:	eb0023aa 	bl	b0d8 <base_block_markbad>
    222c:	e1a00004 	mov	r0, r4
    2230:	e8bd8070 	pop	{r4, r5, r6, pc}
    2234:	0001663e 	.word	0x0001663e
    2238:	0001664a 	.word	0x0001664a
    223c:	00016646 	.word	0x00016646
    2240:	0001664e 	.word	0x0001664e
    2244:	3c00d6cc 	.word	0x3c00d6cc

00002248 <do_sc>:
    2248:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    224c:	e3520002 	cmp	r2, #2	; 0x2
    2250:	e24ddf47 	sub	sp, sp, #284	; 0x11c
    2254:	0a000002 	beq	2264 <do_sc+0x1c>
    2258:	eb000310 	bl	2ea0 <cmd_usage>
    225c:	e3a00001 	mov	r0, #1	; 0x1
    2260:	ea000060 	b	23e8 <do_sc+0x1a0>
    2264:	e5930004 	ldr	r0, [r3, #4]
    2268:	e3a01000 	mov	r1, #0	; 0x0
    226c:	e3a02010 	mov	r2, #16	; 0x10
    2270:	eb003d41 	bl	1177c <simple_strtoul>
    2274:	e28d4f41 	add	r4, sp, #260	; 0x104
    2278:	e2844002 	add	r4, r4, #2	; 0x2
    227c:	e59f116c 	ldr	r1, [pc, #364]	; 23f0 <do_sc+0x1a8>
    2280:	e1a06000 	mov	r6, r0
    2284:	e3a02012 	mov	r2, #18	; 0x12
    2288:	e1a00004 	mov	r0, r4
    228c:	ebfffcb0 	bl	1554 <memcpy>
    2290:	e3a01000 	mov	r1, #0	; 0x0
    2294:	ea000004 	b	22ac <do_sc+0x64>
    2298:	e7d12006 	ldrb	r2, [r1, r6]
    229c:	e7d43001 	ldrb	r3, [r4, r1]
    22a0:	e2811001 	add	r1, r1, #1	; 0x1
    22a4:	e1520003 	cmp	r2, r3
    22a8:	1a000014 	bne	2300 <do_sc+0xb8>
    22ac:	e3510012 	cmp	r1, #18	; 0x12
    22b0:	e0815006 	add	r5, r1, r6
    22b4:	1afffff7 	bne	2298 <do_sc+0x50>
    22b8:	e2851eff 	add	r1, r5, #4080	; 0xff0
    22bc:	e281100f 	add	r1, r1, #15	; 0xf
    22c0:	e1a02005 	mov	r2, r5
    22c4:	e5d23000 	ldrb	r3, [r2]
    22c8:	e353000a 	cmp	r3, #10	; 0xa
    22cc:	1a000005 	bne	22e8 <do_sc+0xa0>
    22d0:	e5d23001 	ldrb	r3, [r2, #1]
    22d4:	e353005d 	cmp	r3, #93	; 0x5d
    22d8:	1a000002 	bne	22e8 <do_sc+0xa0>
    22dc:	e5d23002 	ldrb	r3, [r2, #2]
    22e0:	e353005d 	cmp	r3, #93	; 0x5d
    22e4:	0a000003 	beq	22f8 <do_sc+0xb0>
    22e8:	e1520001 	cmp	r2, r1
    22ec:	e2822001 	add	r2, r2, #1	; 0x1
    22f0:	1afffff3 	bne	22c4 <do_sc+0x7c>
    22f4:	ea000001 	b	2300 <do_sc+0xb8>
    22f8:	e3550000 	cmp	r5, #0	; 0x0
    22fc:	1a000006 	bne	231c <do_sc+0xd4>
    2300:	e59f00ec 	ldr	r0, [pc, #236]	; 23f4 <do_sc+0x1ac>
    2304:	e1a01006 	mov	r1, r6
    2308:	e59f30e8 	ldr	r3, [pc, #232]	; 23f8 <do_sc+0x1b0>
    230c:	e1a0e00f 	mov	lr, pc
    2310:	e593f000 	ldr	pc, [r3]
    2314:	e3e00000 	mvn	r0, #0	; 0x0
    2318:	ea000032 	b	23e8 <do_sc+0x1a0>
    231c:	e59f40d4 	ldr	r4, [pc, #212]	; 23f8 <do_sc+0x1b0>
    2320:	e1a01006 	mov	r1, r6
    2324:	e59f00d0 	ldr	r0, [pc, #208]	; 23fc <do_sc+0x1b4>
    2328:	e28d6008 	add	r6, sp, #8	; 0x8
    232c:	e1a0e00f 	mov	lr, pc
    2330:	e594f000 	ldr	pc, [r4]
    2334:	e2466002 	sub	r6, r6, #2	; 0x2
    2338:	e1a07004 	mov	r7, r4
    233c:	ea000000 	b	2344 <do_sc+0xfc>
    2340:	e2855001 	add	r5, r5, #1	; 0x1
    2344:	e5d53000 	ldrb	r3, [r5]
    2348:	e353002d 	cmp	r3, #45	; 0x2d
    234c:	1353000a 	cmpne	r3, #10	; 0xa
    2350:	0afffffa 	beq	2340 <do_sc+0xf8>
    2354:	e353005b 	cmp	r3, #91	; 0x5b
    2358:	0afffff8 	beq	2340 <do_sc+0xf8>
    235c:	e353005d 	cmp	r3, #93	; 0x5d
    2360:	0a000001 	beq	236c <do_sc+0x124>
    2364:	e1a02006 	mov	r2, r6
    2368:	ea000005 	b	2384 <do_sc+0x13c>
    236c:	e5d53001 	ldrb	r3, [r5, #1]
    2370:	e353005d 	cmp	r3, #93	; 0x5d
    2374:	1afffffa 	bne	2364 <do_sc+0x11c>
    2378:	ea00000c 	b	23b0 <do_sc+0x168>
    237c:	e5423001 	strb	r3, [r2, #-1]
    2380:	e2855001 	add	r5, r5, #1	; 0x1
    2384:	e5d53000 	ldrb	r3, [r5]
    2388:	e1a01002 	mov	r1, r2
    238c:	e353000a 	cmp	r3, #10	; 0xa
    2390:	1353005d 	cmpne	r3, #93	; 0x5d
    2394:	03a04000 	moveq	r4, #0	; 0x0
    2398:	13a04001 	movne	r4, #1	; 0x1
    239c:	e2822001 	add	r2, r2, #1	; 0x1
    23a0:	1afffff5 	bne	237c <do_sc+0x134>
    23a4:	e3550000 	cmp	r5, #0	; 0x0
    23a8:	e5c14000 	strb	r4, [r1]
    23ac:	1a000005 	bne	23c8 <do_sc+0x180>
    23b0:	e59f0048 	ldr	r0, [pc, #72]	; 2400 <do_sc+0x1b8>
    23b4:	e59f303c 	ldr	r3, [pc, #60]	; 23f8 <do_sc+0x1b0>
    23b8:	e1a0e00f 	mov	lr, pc
    23bc:	e593f000 	ldr	pc, [r3]
    23c0:	e3a00000 	mov	r0, #0	; 0x0
    23c4:	ea000007 	b	23e8 <do_sc+0x1a0>
    23c8:	e1a01006 	mov	r1, r6
    23cc:	e59f0030 	ldr	r0, [pc, #48]	; 2404 <do_sc+0x1bc>
    23d0:	e1a0e00f 	mov	lr, pc
    23d4:	e597f000 	ldr	pc, [r7]
    23d8:	e1a00006 	mov	r0, r6
    23dc:	e1a01004 	mov	r1, r4
    23e0:	eb004139 	bl	128cc <run_command>
    23e4:	eaffffd6 	b	2344 <do_sc+0xfc>
    23e8:	e28ddf47 	add	sp, sp, #284	; 0x11c
    23ec:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    23f0:	00015f6c 	.word	0x00015f6c
    23f4:	0001668d 	.word	0x0001668d
    23f8:	3c00d6cc 	.word	0x3c00d6cc
    23fc:	000166aa 	.word	0x000166aa
    2400:	000166c3 	.word	0x000166c3
    2404:	000166d0 	.word	0x000166d0

00002408 <do_ls>:
    2408:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    240c:	e59f001c 	ldr	r0, [pc, #28]	; 2430 <do_ls+0x28>
    2410:	e24dd004 	sub	sp, sp, #4	; 0x4
    2414:	e59f1018 	ldr	r1, [pc, #24]	; 2434 <do_ls+0x2c>
    2418:	e59f3018 	ldr	r3, [pc, #24]	; 2438 <do_ls+0x30>
    241c:	e1a0e00f 	mov	lr, pc
    2420:	e593f000 	ldr	pc, [r3]
    2424:	e3a00000 	mov	r0, #0	; 0x0
    2428:	e28dd004 	add	sp, sp, #4	; 0x4
    242c:	e8bd8000 	pop	{pc}
    2430:	00016437 	.word	0x00016437
    2434:	3c00d08c 	.word	0x3c00d08c
    2438:	3c00d6cc 	.word	0x3c00d6cc

0000243c <do_linkpc>:
    243c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2440:	e24dd004 	sub	sp, sp, #4	; 0x4
    2444:	eb003a40 	bl	10d4c <linkpc_loop>
    2448:	e3a00000 	mov	r0, #0	; 0x0
    244c:	e28dd004 	add	sp, sp, #4	; 0x4
    2450:	e8bd8000 	pop	{pc}

00002454 <do_pads>:
    2454:	e3520002 	cmp	r2, #2	; 0x2
    2458:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    245c:	d59f0194 	ldrle	r0, [pc, #404]	; 25f8 <do_pads+0x1a4>
    2460:	e1a05002 	mov	r5, r2
    2464:	e1a06003 	mov	r6, r3
    2468:	da000015 	ble	24c4 <do_pads+0x70>
    246c:	e5930008 	ldr	r0, [r3, #8]
    2470:	e3a01000 	mov	r1, #0	; 0x0
    2474:	e3a0200a 	mov	r2, #10	; 0xa
    2478:	eb003cbf 	bl	1177c <simple_strtoul>
    247c:	e3550003 	cmp	r5, #3	; 0x3
    2480:	e1a04000 	mov	r4, r0
    2484:	01a07000 	moveq	r7, r0
    2488:	0a000004 	beq	24a0 <do_pads+0x4c>
    248c:	e596000c 	ldr	r0, [r6, #12]
    2490:	e3a01000 	mov	r1, #0	; 0x0
    2494:	e3a0200a 	mov	r2, #10	; 0xa
    2498:	eb003cb7 	bl	1177c <simple_strtoul>
    249c:	e1a07000 	mov	r7, r0
    24a0:	e1943007 	orrs	r3, r4, r7
    24a4:	4a000005 	bmi	24c0 <do_pads+0x6c>
    24a8:	e1a05004 	mov	r5, r4
    24ac:	e2866004 	add	r6, r6, #4	; 0x4
    24b0:	e59f9144 	ldr	r9, [pc, #324]	; 25fc <do_pads+0x1a8>
    24b4:	e59f8144 	ldr	r8, [pc, #324]	; 2600 <do_pads+0x1ac>
    24b8:	e59fa144 	ldr	sl, [pc, #324]	; 2604 <do_pads+0x1b0>
    24bc:	ea000049 	b	25e8 <do_pads+0x194>
    24c0:	e59f0140 	ldr	r0, [pc, #320]	; 2608 <do_pads+0x1b4>
    24c4:	e59f3134 	ldr	r3, [pc, #308]	; 2600 <do_pads+0x1ac>
    24c8:	e1a0e00f 	mov	lr, pc
    24cc:	e593f000 	ldr	pc, [r3]
    24d0:	ea000046 	b	25f0 <do_pads+0x19c>
    24d4:	e5960000 	ldr	r0, [r6]
    24d8:	e59f111c 	ldr	r1, [pc, #284]	; 25fc <do_pads+0x1a8>
    24dc:	e3a02002 	mov	r2, #2	; 0x2
    24e0:	ebfffbe2 	bl	1470 <strncmp>
    24e4:	e3500000 	cmp	r0, #0	; 0x0
    24e8:	1a00000b 	bne	251c <do_pads+0xc8>
    24ec:	e1a02000 	mov	r2, r0
    24f0:	e3a01001 	mov	r1, #1	; 0x1
    24f4:	e1a00005 	mov	r0, r5
    24f8:	ebfff9b6 	bl	bd8 <pads_chmod>
    24fc:	e1a00005 	mov	r0, r5
    2500:	e5984000 	ldr	r4, [r8]
    2504:	ebfffa5a 	bl	e74 <pads_getpin>
    2508:	e1a01005 	mov	r1, r5
    250c:	e1a02000 	mov	r2, r0
    2510:	e59f00f4 	ldr	r0, [pc, #244]	; 260c <do_pads+0x1b8>
    2514:	e12fff34 	blx	r4
    2518:	ea000031 	b	25e4 <do_pads+0x190>
    251c:	e59f10ec 	ldr	r1, [pc, #236]	; 2610 <do_pads+0x1bc>
    2520:	e3a02004 	mov	r2, #4	; 0x4
    2524:	e5960000 	ldr	r0, [r6]
    2528:	ebfffbd0 	bl	1470 <strncmp>
    252c:	e3500000 	cmp	r0, #0	; 0x0
    2530:	01a00005 	moveq	r0, r5
    2534:	03a01002 	moveq	r1, #2	; 0x2
    2538:	03a02001 	moveq	r2, #1	; 0x1
    253c:	0a000011 	beq	2588 <do_pads+0x134>
    2540:	e59f10cc 	ldr	r1, [pc, #204]	; 2614 <do_pads+0x1c0>
    2544:	e3a02003 	mov	r2, #3	; 0x3
    2548:	e5960000 	ldr	r0, [r6]
    254c:	ebfffbc7 	bl	1470 <strncmp>
    2550:	e3500000 	cmp	r0, #0	; 0x0
    2554:	01a02000 	moveq	r2, r0
    2558:	03a01002 	moveq	r1, #2	; 0x2
    255c:	01a00005 	moveq	r0, r5
    2560:	0a000008 	beq	2588 <do_pads+0x134>
    2564:	e5960000 	ldr	r0, [r6]
    2568:	e59f10a8 	ldr	r1, [pc, #168]	; 2618 <do_pads+0x1c4>
    256c:	e3a02004 	mov	r2, #4	; 0x4
    2570:	ebfffbbe 	bl	1470 <strncmp>
    2574:	e3500000 	cmp	r0, #0	; 0x0
    2578:	1a000004 	bne	2590 <do_pads+0x13c>
    257c:	e1a01000 	mov	r1, r0
    2580:	e1a02001 	mov	r2, r1
    2584:	e1a00005 	mov	r0, r5
    2588:	ebfff992 	bl	bd8 <pads_chmod>
    258c:	ea000014 	b	25e4 <do_pads+0x190>
    2590:	e5960000 	ldr	r0, [r6]
    2594:	e59f1080 	ldr	r1, [pc, #128]	; 261c <do_pads+0x1c8>
    2598:	e3a02002 	mov	r2, #2	; 0x2
    259c:	ebfffbb3 	bl	1470 <strncmp>
    25a0:	e3500000 	cmp	r0, #0	; 0x0
    25a4:	1a00000c 	bne	25dc <do_pads+0x188>
    25a8:	e1a00005 	mov	r0, r5
    25ac:	ebfff973 	bl	b80 <pads_states>
    25b0:	e3500000 	cmp	r0, #0	; 0x0
    25b4:	e5983000 	ldr	r3, [r8]
    25b8:	059f2060 	ldreq	r2, [pc, #96]	; 2620 <do_pads+0x1cc>
    25bc:	0a000002 	beq	25cc <do_pads+0x178>
    25c0:	e3500001 	cmp	r0, #1	; 0x1
    25c4:	01a02009 	moveq	r2, r9
    25c8:	11a0200a 	movne	r2, sl
    25cc:	e59f0050 	ldr	r0, [pc, #80]	; 2624 <do_pads+0x1d0>
    25d0:	e1a01005 	mov	r1, r5
    25d4:	e12fff33 	blx	r3
    25d8:	ea000001 	b	25e4 <do_pads+0x190>
    25dc:	e59f0044 	ldr	r0, [pc, #68]	; 2628 <do_pads+0x1d4>
    25e0:	eaffffb7 	b	24c4 <do_pads+0x70>
    25e4:	e2855001 	add	r5, r5, #1	; 0x1
    25e8:	e1550007 	cmp	r5, r7
    25ec:	daffffb8 	ble	24d4 <do_pads+0x80>
    25f0:	e3a00000 	mov	r0, #0	; 0x0
    25f4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    25f8:	000166f4 	.word	0x000166f4
    25fc:	00016731 	.word	0x00016731
    2600:	3c00d6cc 	.word	0x3c00d6cc
    2604:	00018d9b 	.word	0x00018d9b
    2608:	0001670c 	.word	0x0001670c
    260c:	00016734 	.word	0x00016734
    2610:	00016748 	.word	0x00016748
    2614:	0001674d 	.word	0x0001674d
    2618:	00016751 	.word	0x00016751
    261c:	00016756 	.word	0x00016756
    2620:	00016759 	.word	0x00016759
    2624:	00016762 	.word	0x00016762
    2628:	00016777 	.word	0x00016777

0000262c <do_mux>:
    262c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2630:	e3520001 	cmp	r2, #1	; 0x1
    2634:	e24dd004 	sub	sp, sp, #4	; 0x4
    2638:	da000005 	ble	2654 <do_mux+0x28>
    263c:	e5930004 	ldr	r0, [r3, #4]
    2640:	e3a01000 	mov	r1, #0	; 0x0
    2644:	e3a0200a 	mov	r2, #10	; 0xa
    2648:	eb003c4b 	bl	1177c <simple_strtoul>
    264c:	e6ff0070 	uxth	r0, r0
    2650:	ebfffae6 	bl	11f0 <set_mux>
    2654:	e59f301c 	ldr	r3, [pc, #28]	; 2678 <do_mux+0x4c>
    2658:	e59f001c 	ldr	r0, [pc, #28]	; 267c <do_mux+0x50>
    265c:	e5931000 	ldr	r1, [r3]
    2660:	e59f3018 	ldr	r3, [pc, #24]	; 2680 <do_mux+0x54>
    2664:	e1a0e00f 	mov	lr, pc
    2668:	e593f000 	ldr	pc, [r3]
    266c:	e3a00000 	mov	r0, #0	; 0x0
    2670:	e28dd004 	add	sp, sp, #4	; 0x4
    2674:	e8bd8000 	pop	{pc}
    2678:	21e09000 	.word	0x21e09000
    267c:	00016790 	.word	0x00016790
    2680:	3c00d6cc 	.word	0x3c00d6cc

00002684 <do_mac>:
    2684:	e59f3020 	ldr	r3, [pc, #32]	; 26ac <do_mac+0x28>
    2688:	e92d4010 	push	{r4, lr}
    268c:	e5934000 	ldr	r4, [r3]
    2690:	ebfff8c4 	bl	9a8 <efuse_mac>
    2694:	e1a02000 	mov	r2, r0
    2698:	e1a03001 	mov	r3, r1
    269c:	e59f000c 	ldr	r0, [pc, #12]	; 26b0 <do_mac+0x2c>
    26a0:	e12fff34 	blx	r4
    26a4:	e3a00000 	mov	r0, #0	; 0x0
    26a8:	e8bd8010 	pop	{r4, pc}
    26ac:	3c00d6cc 	.word	0x3c00d6cc
    26b0:	000167a2 	.word	0x000167a2

000026b4 <do_boot>:
    26b4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    26b8:	e3a00000 	mov	r0, #0	; 0x0
    26bc:	e24dd004 	sub	sp, sp, #4	; 0x4
    26c0:	ebfff760 	bl	448 <try_boot>
    26c4:	e3a00000 	mov	r0, #0	; 0x0
    26c8:	e28dd004 	add	sp, sp, #4	; 0x4
    26cc:	e8bd8000 	pop	{pc}

000026d0 <do_efuse>:
    26d0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    26d4:	e3520001 	cmp	r2, #1	; 0x1
    26d8:	e24dd004 	sub	sp, sp, #4	; 0x4
    26dc:	da000006 	ble	26fc <do_efuse+0x2c>
    26e0:	e5930004 	ldr	r0, [r3, #4]
    26e4:	e59f1024 	ldr	r1, [pc, #36]	; 2710 <do_efuse+0x40>
    26e8:	e3a02002 	mov	r2, #2	; 0x2
    26ec:	ebfffb5f 	bl	1470 <strncmp>
    26f0:	e3500000 	cmp	r0, #0	; 0x0
    26f4:	02800001 	addeq	r0, r0, #1	; 0x1
    26f8:	0a000000 	beq	2700 <do_efuse+0x30>
    26fc:	e3a00000 	mov	r0, #0	; 0x0
    2700:	ebfff8ab 	bl	9b4 <efuse_info>
    2704:	e3a00000 	mov	r0, #0	; 0x0
    2708:	e28dd004 	add	sp, sp, #4	; 0x4
    270c:	e8bd8000 	pop	{pc}
    2710:	000167ba 	.word	0x000167ba

00002714 <do_pi>:
    2714:	e92d4030 	push	{r4, r5, lr}
    2718:	e3520001 	cmp	r2, #1	; 0x1
    271c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2720:	e1a04002 	mov	r4, r2
    2724:	e1a05003 	mov	r5, r3
    2728:	da000015 	ble	2784 <do_pi+0x70>
    272c:	e59f10ac 	ldr	r1, [pc, #172]	; 27e0 <do_pi+0xcc>
    2730:	e5930004 	ldr	r0, [r3, #4]
    2734:	e3a02004 	mov	r2, #4	; 0x4
    2738:	ebfffb4c 	bl	1470 <strncmp>
    273c:	e2501000 	subs	r1, r0, #0	; 0x0
    2740:	1a00000d 	bne	277c <do_pi+0x68>
    2744:	e3540002 	cmp	r4, #2	; 0x2
    2748:	01a04001 	moveq	r4, r1
    274c:	0a000003 	beq	2760 <do_pi+0x4c>
    2750:	e5950008 	ldr	r0, [r5, #8]
    2754:	e3a02010 	mov	r2, #16	; 0x10
    2758:	eb003c07 	bl	1177c <simple_strtoul>
    275c:	e1a04000 	mov	r4, r0
    2760:	e59f007c 	ldr	r0, [pc, #124]	; 27e4 <do_pi+0xd0>
    2764:	e1a01004 	mov	r1, r4
    2768:	e59f3078 	ldr	r3, [pc, #120]	; 27e8 <do_pi+0xd4>
    276c:	e1a0e00f 	mov	lr, pc
    2770:	e593f000 	ldr	pc, [r3]
    2774:	e1a00004 	mov	r0, r4
    2778:	eb00159f 	bl	7dfc <burn>
    277c:	e3a04000 	mov	r4, #0	; 0x0
    2780:	ea000013 	b	27d4 <do_pi+0xc0>
    2784:	e3a00481 	mov	r0, #-2130706432	; 0x81000000
    2788:	eb003f8f 	bl	125cc <ius_check_header>
    278c:	e2504000 	subs	r4, r0, #0	; 0x0
    2790:	159f0054 	ldrne	r0, [pc, #84]	; 27ec <do_pi+0xd8>
    2794:	1a000005 	bne	27b0 <do_pi+0x9c>
    2798:	e3a0000d 	mov	r0, #13	; 0xd
    279c:	e3a01001 	mov	r1, #1	; 0x1
    27a0:	eb000f4d 	bl	64dc <vs_assign_by_id>
    27a4:	e2504000 	subs	r4, r0, #0	; 0x0
    27a8:	0a000005 	beq	27c4 <do_pi+0xb0>
    27ac:	e59f003c 	ldr	r0, [pc, #60]	; 27f0 <do_pi+0xdc>
    27b0:	e1a01004 	mov	r1, r4
    27b4:	e59f302c 	ldr	r3, [pc, #44]	; 27e8 <do_pi+0xd4>
    27b8:	e1a0e00f 	mov	lr, pc
    27bc:	e593f000 	ldr	pc, [r3]
    27c0:	ea000003 	b	27d4 <do_pi+0xc0>
    27c4:	e3a00481 	mov	r0, #-2130706432	; 0x81000000
    27c8:	e3a0100d 	mov	r1, #13	; 0xd
    27cc:	e1a02004 	mov	r2, r4
    27d0:	eb001501 	bl	7bdc <burn_images>
    27d4:	e1a00004 	mov	r0, r4
    27d8:	e28dd004 	add	sp, sp, #4	; 0x4
    27dc:	e8bd8030 	pop	{r4, r5, pc}
    27e0:	000167bd 	.word	0x000167bd
    27e4:	000167c2 	.word	0x000167c2
    27e8:	3c00d6cc 	.word	0x3c00d6cc
    27ec:	000167ec 	.word	0x000167ec
    27f0:	0001680c 	.word	0x0001680c

000027f4 <do_spbyte>:
    27f4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    27f8:	e5930004 	ldr	r0, [r3, #4]
    27fc:	e24dd004 	sub	sp, sp, #4	; 0x4
    2800:	e3a01000 	mov	r1, #0	; 0x0
    2804:	e3a0200a 	mov	r2, #10	; 0xa
    2808:	eb003bdb 	bl	1177c <simple_strtoul>
    280c:	eb002ee2 	bl	e39c <spi_set_bytes>
    2810:	e3a00000 	mov	r0, #0	; 0x0
    2814:	e28dd004 	add	sp, sp, #4	; 0x4
    2818:	e8bd8000 	pop	{pc}

0000281c <do_cfg>:
    281c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2820:	e5930004 	ldr	r0, [r3, #4]
    2824:	e24dd004 	sub	sp, sp, #4	; 0x4
    2828:	e3a01000 	mov	r1, #0	; 0x0
    282c:	e3a02010 	mov	r2, #16	; 0x10
    2830:	eb003bd1 	bl	1177c <simple_strtoul>
    2834:	eb00138f 	bl	7678 <cdbg_config_isi>
    2838:	e3a00000 	mov	r0, #0	; 0x0
    283c:	e28dd004 	add	sp, sp, #4	; 0x4
    2840:	e8bd8000 	pop	{pc}

00002844 <do_sleep>:
    2844:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2848:	e3a01000 	mov	r1, #0	; 0x0
    284c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2850:	e3a0200a 	mov	r2, #10	; 0xa
    2854:	e5930004 	ldr	r0, [r3, #4]
    2858:	eb003bc7 	bl	1177c <simple_strtoul>
    285c:	e59f3020 	ldr	r3, [pc, #32]	; 2884 <do_sleep+0x40>
    2860:	e0000093 	mul	r0, r3, r0
    2864:	eb002c5c 	bl	d9dc <udelay>
    2868:	e59f0018 	ldr	r0, [pc, #24]	; 2888 <do_sleep+0x44>
    286c:	e59f3018 	ldr	r3, [pc, #24]	; 288c <do_sleep+0x48>
    2870:	e1a0e00f 	mov	lr, pc
    2874:	e593f000 	ldr	pc, [r3]
    2878:	e3a00000 	mov	r0, #0	; 0x0
    287c:	e28dd004 	add	sp, sp, #4	; 0x4
    2880:	e8bd8000 	pop	{pc}
    2884:	000f4240 	.word	0x000f4240
    2888:	00016827 	.word	0x00016827
    288c:	3c00d6cc 	.word	0x3c00d6cc

00002890 <do_go>:
    2890:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2894:	e5930004 	ldr	r0, [r3, #4]
    2898:	e24dd004 	sub	sp, sp, #4	; 0x4
    289c:	e3a02010 	mov	r2, #16	; 0x10
    28a0:	e3a01000 	mov	r1, #0	; 0x0
    28a4:	eb003bb4 	bl	1177c <simple_strtoul>
    28a8:	e3a01000 	mov	r1, #0	; 0x0
    28ac:	eb0012de 	bl	742c <cdbg_jump>
    28b0:	e3a00000 	mov	r0, #0	; 0x0
    28b4:	e28dd004 	add	sp, sp, #4	; 0x4
    28b8:	e8bd8000 	pop	{pc}

000028bc <do_ml>:
    28bc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    28c0:	e24dd004 	sub	sp, sp, #4	; 0x4
    28c4:	ebfffc18 	bl	192c <__sOINFo>
    28c8:	e3a00000 	mov	r0, #0	; 0x0
    28cc:	e28dd004 	add	sp, sp, #4	; 0x4
    28d0:	e8bd8000 	pop	{pc}

000028d4 <do_bp>:
    28d4:	e92d4030 	push	{r4, r5, lr}
    28d8:	e3520001 	cmp	r2, #1	; 0x1
    28dc:	e24dd004 	sub	sp, sp, #4	; 0x4
    28e0:	da000005 	ble	28fc <do_bp+0x28>
    28e4:	e5930004 	ldr	r0, [r3, #4]
    28e8:	e3a01000 	mov	r1, #0	; 0x0
    28ec:	e3a02010 	mov	r2, #16	; 0x10
    28f0:	eb003ba1 	bl	1177c <simple_strtoul>
    28f4:	e6ff0070 	uxth	r0, r0
    28f8:	ebfff737 	bl	5dc <set_xom>
    28fc:	e59f5034 	ldr	r5, [pc, #52]	; 2938 <do_bp+0x64>
    2900:	e3a00001 	mov	r0, #1	; 0x1
    2904:	e5954000 	ldr	r4, [r5]
    2908:	ebfff72f 	bl	5cc <get_xom>
    290c:	e1a01000 	mov	r1, r0
    2910:	e59f0024 	ldr	r0, [pc, #36]	; 293c <do_bp+0x68>
    2914:	e12fff34 	blx	r4
    2918:	e5954000 	ldr	r4, [r5]
    291c:	ebfff734 	bl	5f4 <boot_info>
    2920:	e1a01000 	mov	r1, r0
    2924:	e59f0014 	ldr	r0, [pc, #20]	; 2940 <do_bp+0x6c>
    2928:	e12fff34 	blx	r4
    292c:	e3a00000 	mov	r0, #0	; 0x0
    2930:	e28dd004 	add	sp, sp, #4	; 0x4
    2934:	e8bd8030 	pop	{r4, r5, pc}
    2938:	3c00d6cc 	.word	0x3c00d6cc
    293c:	00016832 	.word	0x00016832
    2940:	00016848 	.word	0x00016848

00002944 <do_reset>:
    2944:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2948:	e24dd004 	sub	sp, sp, #4	; 0x4
    294c:	ebfff7ca 	bl	87c <system_reset>
    2950:	e3a00000 	mov	r0, #0	; 0x0
    2954:	e28dd004 	add	sp, sp, #4	; 0x4
    2958:	e8bd8000 	pop	{pc}

0000295c <output_debug>:
    295c:	e24dd010 	sub	sp, sp, #16	; 0x10
    2960:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2964:	e24dd004 	sub	sp, sp, #4	; 0x4
    2968:	e28dc008 	add	ip, sp, #8	; 0x8
    296c:	e3700002 	cmn	r0, #2	; 0x2
    2970:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    2974:	e1a01000 	mov	r1, r0
    2978:	0a000012 	beq	29c8 <output_debug+0x6c>
    297c:	e3700001 	cmn	r0, #1	; 0x1
    2980:	0a000008 	beq	29a8 <output_debug+0x4c>
    2984:	e3700003 	cmn	r0, #3	; 0x3
    2988:	1a000010 	bne	29d0 <output_debug+0x74>
    298c:	e59f30a4 	ldr	r3, [pc, #164]	; 2a38 <output_debug+0xdc>
    2990:	e59f00a4 	ldr	r0, [pc, #164]	; 2a3c <output_debug+0xe0>
    2994:	e5931000 	ldr	r1, [r3]
    2998:	e59f30a0 	ldr	r3, [pc, #160]	; 2a40 <output_debug+0xe4>
    299c:	e1a0e00f 	mov	lr, pc
    29a0:	e593f000 	ldr	pc, [r3]
    29a4:	ea00001e 	b	2a24 <output_debug+0xc8>
    29a8:	e59f3088 	ldr	r3, [pc, #136]	; 2a38 <output_debug+0xdc>
    29ac:	e59f0090 	ldr	r0, [pc, #144]	; 2a44 <output_debug+0xe8>
    29b0:	e5932000 	ldr	r2, [r3]
    29b4:	e28d1010 	add	r1, sp, #16	; 0x10
    29b8:	e59f3080 	ldr	r3, [pc, #128]	; 2a40 <output_debug+0xe4>
    29bc:	e1a0e00f 	mov	lr, pc
    29c0:	e593f000 	ldr	pc, [r3]
    29c4:	ea000016 	b	2a24 <output_debug+0xc8>
    29c8:	e59f0078 	ldr	r0, [pc, #120]	; 2a48 <output_debug+0xec>
    29cc:	ea000003 	b	29e0 <output_debug+0x84>
    29d0:	e59d300c 	ldr	r3, [sp, #12]
    29d4:	e3530001 	cmp	r3, #1	; 0x1
    29d8:	1a000004 	bne	29f0 <output_debug+0x94>
    29dc:	e59f0068 	ldr	r0, [pc, #104]	; 2a4c <output_debug+0xf0>
    29e0:	e59f3058 	ldr	r3, [pc, #88]	; 2a40 <output_debug+0xe4>
    29e4:	e1a0e00f 	mov	lr, pc
    29e8:	e593f000 	ldr	pc, [r3]
    29ec:	ea00000c 	b	2a24 <output_debug+0xc8>
    29f0:	e3530002 	cmp	r3, #2	; 0x2
    29f4:	e59fc044 	ldr	ip, [pc, #68]	; 2a40 <output_debug+0xe4>
    29f8:	1a000003 	bne	2a0c <output_debug+0xb0>
    29fc:	e59f004c 	ldr	r0, [pc, #76]	; 2a50 <output_debug+0xf4>
    2a00:	e1a0e00f 	mov	lr, pc
    2a04:	e59cf000 	ldr	pc, [ip]
    2a08:	ea000005 	b	2a24 <output_debug+0xc8>
    2a0c:	e59f3024 	ldr	r3, [pc, #36]	; 2a38 <output_debug+0xdc>
    2a10:	e59f003c 	ldr	r0, [pc, #60]	; 2a54 <output_debug+0xf8>
    2a14:	e5933000 	ldr	r3, [r3]
    2a18:	e28d2010 	add	r2, sp, #16	; 0x10
    2a1c:	e1a0e00f 	mov	lr, pc
    2a20:	e59cf000 	ldr	pc, [ip]
    2a24:	e3a00000 	mov	r0, #0	; 0x0
    2a28:	e28dd004 	add	sp, sp, #4	; 0x4
    2a2c:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    2a30:	e28dd010 	add	sp, sp, #16	; 0x10
    2a34:	e12fff1e 	bx	lr
    2a38:	3c00d12c 	.word	0x3c00d12c
    2a3c:	000169d3 	.word	0x000169d3
    2a40:	3c00d6cc 	.word	0x3c00d6cc
    2a44:	000169fb 	.word	0x000169fb
    2a48:	00016a0a 	.word	0x00016a0a
    2a4c:	00016a24 	.word	0x00016a24
    2a50:	00016a2f 	.word	0x00016a2f
    2a54:	00016a3d 	.word	0x00016a3d

00002a58 <do_vs>:
    2a58:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    2a5c:	e3520006 	cmp	r2, #6	; 0x6
    2a60:	e24dd024 	sub	sp, sp, #36	; 0x24
    2a64:	e1a06000 	mov	r6, r0
    2a68:	e1a04003 	mov	r4, r3
    2a6c:	1a000005 	bne	2a88 <do_vs+0x30>
    2a70:	e5930014 	ldr	r0, [r3, #20]
    2a74:	e3a01000 	mov	r1, #0	; 0x0
    2a78:	e282200a 	add	r2, r2, #10	; 0xa
    2a7c:	eb003b3e 	bl	1177c <simple_strtoul>
    2a80:	e1a05000 	mov	r5, r0
    2a84:	ea00008f 	b	2cc8 <do_vs+0x270>
    2a88:	e2423001 	sub	r3, r2, #1	; 0x1
    2a8c:	e3530005 	cmp	r3, #5	; 0x5
    2a90:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    2a94:	ea0000c4 	b	2dac <do_vs+0x354>
    2a98:	00002dac 	.word	0x00002dac
    2a9c:	00002ab0 	.word	0x00002ab0
    2aa0:	00002ba8 	.word	0x00002ba8
    2aa4:	00002c18 	.word	0x00002c18
    2aa8:	00002cc4 	.word	0x00002cc4
    2aac:	00002cc4 	.word	0x00002cc4
    2ab0:	e5940004 	ldr	r0, [r4, #4]
    2ab4:	e59f1310 	ldr	r1, [pc, #784]	; 2dcc <do_vs+0x374>
    2ab8:	ebfffa5e 	bl	1438 <strcmp>
    2abc:	e2505000 	subs	r5, r0, #0	; 0x0
    2ac0:	1a000025 	bne	2b5c <do_vs+0x104>
    2ac4:	e59f4304 	ldr	r4, [pc, #772]	; 2dd0 <do_vs+0x378>
    2ac8:	e59f0304 	ldr	r0, [pc, #772]	; 2dd4 <do_vs+0x37c>
    2acc:	e1a0e00f 	mov	lr, pc
    2ad0:	e594f000 	ldr	pc, [r4]
    2ad4:	e3a0802c 	mov	r8, #44	; 0x2c
    2ad8:	e59f72f8 	ldr	r7, [pc, #760]	; 2dd8 <do_vs+0x380>
    2adc:	ea000003 	b	2af0 <do_vs+0x98>
    2ae0:	e0227598 	mla	r2, r8, r5, r7
    2ae4:	e1a0e00f 	mov	lr, pc
    2ae8:	e594f000 	ldr	pc, [r4]
    2aec:	e2855001 	add	r5, r5, #1	; 0x1
    2af0:	eb000da4 	bl	6188 <vs_device_count>
    2af4:	e1550000 	cmp	r5, r0
    2af8:	e1a01005 	mov	r1, r5
    2afc:	e59f02d8 	ldr	r0, [pc, #728]	; 2ddc <do_vs+0x384>
    2b00:	e59f62c8 	ldr	r6, [pc, #712]	; 2dd0 <do_vs+0x378>
    2b04:	bafffff5 	blt	2ae0 <do_vs+0x88>
    2b08:	e59f02d0 	ldr	r0, [pc, #720]	; 2de0 <do_vs+0x388>
    2b0c:	e1a0e00f 	mov	lr, pc
    2b10:	e596f000 	ldr	pc, [r6]
    2b14:	eb000ca1 	bl	5da0 <vs_is_assigned>
    2b18:	e2504000 	subs	r4, r0, #0	; 0x0
    2b1c:	0a000009 	beq	2b48 <do_vs+0xf0>
    2b20:	e59f32bc 	ldr	r3, [pc, #700]	; 2de4 <do_vs+0x38c>
    2b24:	e5965000 	ldr	r5, [r6]
    2b28:	e5934000 	ldr	r4, [r3]
    2b2c:	e1a00004 	mov	r0, r4
    2b30:	eb000d15 	bl	5f8c <vs_align>
    2b34:	e1a01004 	mov	r1, r4
    2b38:	e1a02000 	mov	r2, r0
    2b3c:	e59f02a4 	ldr	r0, [pc, #676]	; 2de8 <do_vs+0x390>
    2b40:	e12fff35 	blx	r5
    2b44:	ea00009d 	b	2dc0 <do_vs+0x368>
    2b48:	e59f029c 	ldr	r0, [pc, #668]	; 2dec <do_vs+0x394>
    2b4c:	e1a0e00f 	mov	lr, pc
    2b50:	e596f000 	ldr	pc, [r6]
    2b54:	e1a00004 	mov	r0, r4
    2b58:	ea000099 	b	2dc4 <do_vs+0x36c>
    2b5c:	e5940004 	ldr	r0, [r4, #4]
    2b60:	e59f1288 	ldr	r1, [pc, #648]	; 2df0 <do_vs+0x398>
    2b64:	ebfffa33 	bl	1438 <strcmp>
    2b68:	e2505000 	subs	r5, r0, #0	; 0x0
    2b6c:	1a000087 	bne	2d90 <do_vs+0x338>
    2b70:	eb000e4a 	bl	64a0 <vs_reset>
    2b74:	e28d4020 	add	r4, sp, #32	; 0x20
    2b78:	e5240018 	str	r0, [r4, #-24]!
    2b7c:	e59f126c 	ldr	r1, [pc, #620]	; 2df0 <do_vs+0x398>
    2b80:	e2840008 	add	r0, r4, #8	; 0x8
    2b84:	ebfffa01 	bl	1390 <strcpy>
    2b88:	e3a03001 	mov	r3, #1	; 0x1
    2b8c:	e58d300c 	str	r3, [sp, #12]
    2b90:	e28d3018 	add	r3, sp, #24	; 0x18
    2b94:	e8930003 	ldm	r3, {r0, r1}
    2b98:	e88d0003 	stm	sp, {r0, r1}
    2b9c:	e894000f 	ldm	r4, {r0, r1, r2, r3}
    2ba0:	ebffff6d 	bl	295c <output_debug>
    2ba4:	ea000019 	b	2c10 <do_vs+0x1b8>
    2ba8:	e5940004 	ldr	r0, [r4, #4]
    2bac:	e59f1240 	ldr	r1, [pc, #576]	; 2df4 <do_vs+0x39c>
    2bb0:	ebfffa20 	bl	1438 <strcmp>
    2bb4:	e2505000 	subs	r5, r0, #0	; 0x0
    2bb8:	1a000074 	bne	2d90 <do_vs+0x338>
    2bbc:	e5940008 	ldr	r0, [r4, #8]
    2bc0:	e3a01001 	mov	r1, #1	; 0x1
    2bc4:	eb000e62 	bl	6554 <vs_assign_by_name>
    2bc8:	e2507000 	subs	r7, r0, #0	; 0x0
    2bcc:	e59f61fc 	ldr	r6, [pc, #508]	; 2dd0 <do_vs+0x378>
    2bd0:	1a00000b 	bne	2c04 <do_vs+0x1ac>
    2bd4:	e5943008 	ldr	r3, [r4, #8]
    2bd8:	e59f2218 	ldr	r2, [pc, #536]	; 2df8 <do_vs+0x3a0>
    2bdc:	e1a00003 	mov	r0, r3
    2be0:	e5823000 	str	r3, [r2]
    2be4:	eb000d69 	bl	6190 <vs_device_id>
    2be8:	e59f31f4 	ldr	r3, [pc, #500]	; 2de4 <do_vs+0x38c>
    2bec:	e5830000 	str	r0, [r3]
    2bf0:	e59f0204 	ldr	r0, [pc, #516]	; 2dfc <do_vs+0x3a4>
    2bf4:	e1a0e00f 	mov	lr, pc
    2bf8:	e596f000 	ldr	pc, [r6]
    2bfc:	e1a00007 	mov	r0, r7
    2c00:	ea00006f 	b	2dc4 <do_vs+0x36c>
    2c04:	e59f01f4 	ldr	r0, [pc, #500]	; 2e00 <do_vs+0x3a8>
    2c08:	e1a0e00f 	mov	lr, pc
    2c0c:	e596f000 	ldr	pc, [r6]
    2c10:	e1a00005 	mov	r0, r5
    2c14:	ea00006a 	b	2dc4 <do_vs+0x36c>
    2c18:	e3a02010 	mov	r2, #16	; 0x10
    2c1c:	e5940008 	ldr	r0, [r4, #8]
    2c20:	e3a01000 	mov	r1, #0	; 0x0
    2c24:	eb003b2c 	bl	118dc <simple_strtoull>
    2c28:	e3a02010 	mov	r2, #16	; 0x10
    2c2c:	e1a07000 	mov	r7, r0
    2c30:	e1a08001 	mov	r8, r1
    2c34:	e594000c 	ldr	r0, [r4, #12]
    2c38:	e3a01000 	mov	r1, #0	; 0x0
    2c3c:	eb003b26 	bl	118dc <simple_strtoull>
    2c40:	e59f11bc 	ldr	r1, [pc, #444]	; 2e04 <do_vs+0x3ac>
    2c44:	e1a05000 	mov	r5, r0
    2c48:	e28d0010 	add	r0, sp, #16	; 0x10
    2c4c:	ebfff9cf 	bl	1390 <strcpy>
    2c50:	e3a03002 	mov	r3, #2	; 0x2
    2c54:	e5940004 	ldr	r0, [r4, #4]
    2c58:	e59f11a4 	ldr	r1, [pc, #420]	; 2e04 <do_vs+0x3ac>
    2c5c:	e58d300c 	str	r3, [sp, #12]
    2c60:	ebfff9f4 	bl	1438 <strcmp>
    2c64:	e3500000 	cmp	r0, #0	; 0x0
    2c68:	1a000004 	bne	2c80 <do_vs+0x228>
    2c6c:	e1a00007 	mov	r0, r7
    2c70:	e1a01008 	mov	r1, r8
    2c74:	e1a02005 	mov	r2, r5
    2c78:	eb000c87 	bl	5e9c <vs_erase>
    2c7c:	ea000008 	b	2ca4 <do_vs+0x24c>
    2c80:	e5940004 	ldr	r0, [r4, #4]
    2c84:	e59f117c 	ldr	r1, [pc, #380]	; 2e08 <do_vs+0x3b0>
    2c88:	ebfff9ea 	bl	1438 <strcmp>
    2c8c:	e3500000 	cmp	r0, #0	; 0x0
    2c90:	1a00003e 	bne	2d90 <do_vs+0x338>
    2c94:	e1a00007 	mov	r0, r7
    2c98:	e1a01008 	mov	r1, r8
    2c9c:	e1a02005 	mov	r2, r5
    2ca0:	eb000c9b 	bl	5f14 <vs_scrub>
    2ca4:	e58d0008 	str	r0, [sp, #8]
    2ca8:	e28d2018 	add	r2, sp, #24	; 0x18
    2cac:	e8920003 	ldm	r2, {r0, r1}
    2cb0:	e88d0003 	stm	sp, {r0, r1}
    2cb4:	e28d3008 	add	r3, sp, #8	; 0x8
    2cb8:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    2cbc:	ebffff26 	bl	295c <output_debug>
    2cc0:	ea00003e 	b	2dc0 <do_vs+0x368>
    2cc4:	e3a05000 	mov	r5, #0	; 0x0
    2cc8:	e3a01000 	mov	r1, #0	; 0x0
    2ccc:	e3a02010 	mov	r2, #16	; 0x10
    2cd0:	e5940008 	ldr	r0, [r4, #8]
    2cd4:	eb003aa8 	bl	1177c <simple_strtoul>
    2cd8:	e3a02010 	mov	r2, #16	; 0x10
    2cdc:	e1a09000 	mov	r9, r0
    2ce0:	e3a01000 	mov	r1, #0	; 0x0
    2ce4:	e594000c 	ldr	r0, [r4, #12]
    2ce8:	eb003afb 	bl	118dc <simple_strtoull>
    2cec:	e3a02010 	mov	r2, #16	; 0x10
    2cf0:	e1a08000 	mov	r8, r0
    2cf4:	e1a0a001 	mov	sl, r1
    2cf8:	e5940010 	ldr	r0, [r4, #16]
    2cfc:	e3a01000 	mov	r1, #0	; 0x0
    2d00:	e08db002 	add	fp, sp, r2
    2d04:	eb003af4 	bl	118dc <simple_strtoull>
    2d08:	e3a03000 	mov	r3, #0	; 0x0
    2d0c:	e59f10f8 	ldr	r1, [pc, #248]	; 2e0c <do_vs+0x3b4>
    2d10:	e1a07000 	mov	r7, r0
    2d14:	e1a0000b 	mov	r0, fp
    2d18:	e58d300c 	str	r3, [sp, #12]
    2d1c:	ebfff99b 	bl	1390 <strcpy>
    2d20:	e5940004 	ldr	r0, [r4, #4]
    2d24:	e59f10e4 	ldr	r1, [pc, #228]	; 2e10 <do_vs+0x3b8>
    2d28:	ebfff9c2 	bl	1438 <strcmp>
    2d2c:	e3500000 	cmp	r0, #0	; 0x0
    2d30:	1a000006 	bne	2d50 <do_vs+0x2f8>
    2d34:	e1a00009 	mov	r0, r9
    2d38:	e1a02008 	mov	r2, r8
    2d3c:	e1a0300a 	mov	r3, sl
    2d40:	e58d7000 	str	r7, [sp]
    2d44:	e58d5004 	str	r5, [sp, #4]
    2d48:	eb000c3d 	bl	5e44 <vs_write>
    2d4c:	eaffffd4 	b	2ca4 <do_vs+0x24c>
    2d50:	e5940004 	ldr	r0, [r4, #4]
    2d54:	e59f10b8 	ldr	r1, [pc, #184]	; 2e14 <do_vs+0x3bc>
    2d58:	ebfff9b6 	bl	1438 <strcmp>
    2d5c:	e3500000 	cmp	r0, #0	; 0x0
    2d60:	1a00000a 	bne	2d90 <do_vs+0x338>
    2d64:	e1a00009 	mov	r0, r9
    2d68:	e1a02008 	mov	r2, r8
    2d6c:	e1a0300a 	mov	r3, sl
    2d70:	e58d7000 	str	r7, [sp]
    2d74:	e58d5004 	str	r5, [sp, #4]
    2d78:	eb000d31 	bl	6244 <vs_read>
    2d7c:	e59f1094 	ldr	r1, [pc, #148]	; 2e18 <do_vs+0x3c0>
    2d80:	e58d0008 	str	r0, [sp, #8]
    2d84:	e1a0000b 	mov	r0, fp
    2d88:	ebfff980 	bl	1390 <strcpy>
    2d8c:	eaffffc5 	b	2ca8 <do_vs+0x250>
    2d90:	e59f0084 	ldr	r0, [pc, #132]	; 2e1c <do_vs+0x3c4>
    2d94:	e5961014 	ldr	r1, [r6, #20]
    2d98:	e59f3030 	ldr	r3, [pc, #48]	; 2dd0 <do_vs+0x378>
    2d9c:	e1a0e00f 	mov	lr, pc
    2da0:	e593f000 	ldr	pc, [r3]
    2da4:	e3e00000 	mvn	r0, #0	; 0x0
    2da8:	ea000005 	b	2dc4 <do_vs+0x36c>
    2dac:	e5901014 	ldr	r1, [r0, #20]
    2db0:	e59f3018 	ldr	r3, [pc, #24]	; 2dd0 <do_vs+0x378>
    2db4:	e59f0060 	ldr	r0, [pc, #96]	; 2e1c <do_vs+0x3c4>
    2db8:	e1a0e00f 	mov	lr, pc
    2dbc:	e593f000 	ldr	pc, [r3]
    2dc0:	e3a00000 	mov	r0, #0	; 0x0
    2dc4:	e28dd024 	add	sp, sp, #36	; 0x24
    2dc8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    2dcc:	00016877 	.word	0x00016877
    2dd0:	3c00d6cc 	.word	0x3c00d6cc
    2dd4:	00016a54 	.word	0x00016a54
    2dd8:	0001561c 	.word	0x0001561c
    2ddc:	00016a62 	.word	0x00016a62
    2de0:	00016a6a 	.word	0x00016a6a
    2de4:	3c00d12c 	.word	0x3c00d12c
    2de8:	00016a7c 	.word	0x00016a7c
    2dec:	00016a86 	.word	0x00016a86
    2df0:	00016865 	.word	0x00016865
    2df4:	00018b6f 	.word	0x00018b6f
    2df8:	3c00d130 	.word	0x3c00d130
    2dfc:	00016a99 	.word	0x00016a99
    2e00:	00016aa5 	.word	0x00016aa5
    2e04:	00016ad2 	.word	0x00016ad2
    2e08:	00016ad8 	.word	0x00016ad8
    2e0c:	00016ade 	.word	0x00016ade
    2e10:	00016ae7 	.word	0x00016ae7
    2e14:	00016aed 	.word	0x00016aed
    2e18:	00016af2 	.word	0x00016af2
    2e1c:	00016436 	.word	0x00016436

00002e20 <cmd_get_data_size>:
    2e20:	e92d4030 	push	{r4, r5, lr}
    2e24:	e24dd004 	sub	sp, sp, #4	; 0x4
    2e28:	e1a04001 	mov	r4, r1
    2e2c:	e1a05000 	mov	r5, r0
    2e30:	ebfff9ad 	bl	14ec <strlen>
    2e34:	e3500002 	cmp	r0, #2	; 0x2
    2e38:	da000015 	ble	2e94 <cmd_get_data_size+0x74>
    2e3c:	e0850000 	add	r0, r5, r0
    2e40:	e5503002 	ldrb	r3, [r0, #-2]
    2e44:	e353002e 	cmp	r3, #46	; 0x2e
    2e48:	1a000011 	bne	2e94 <cmd_get_data_size+0x74>
    2e4c:	e5500001 	ldrb	r0, [r0, #-1]
    2e50:	e350006c 	cmp	r0, #108	; 0x6c
    2e54:	03a04004 	moveq	r4, #4	; 0x4
    2e58:	0a00000d 	beq	2e94 <cmd_get_data_size+0x74>
    2e5c:	8a000002 	bhi	2e6c <cmd_get_data_size+0x4c>
    2e60:	e3500062 	cmp	r0, #98	; 0x62
    2e64:	1a000009 	bne	2e90 <cmd_get_data_size+0x70>
    2e68:	ea000006 	b	2e88 <cmd_get_data_size+0x68>
    2e6c:	e3500073 	cmp	r0, #115	; 0x73
    2e70:	03e04001 	mvneq	r4, #1	; 0x1
    2e74:	0a000006 	beq	2e94 <cmd_get_data_size+0x74>
    2e78:	e3500077 	cmp	r0, #119	; 0x77
    2e7c:	03a04002 	moveq	r4, #2	; 0x2
    2e80:	1a000002 	bne	2e90 <cmd_get_data_size+0x70>
    2e84:	ea000002 	b	2e94 <cmd_get_data_size+0x74>
    2e88:	e3a04001 	mov	r4, #1	; 0x1
    2e8c:	ea000000 	b	2e94 <cmd_get_data_size+0x74>
    2e90:	e3e04000 	mvn	r4, #0	; 0x0
    2e94:	e1a00004 	mov	r0, r4
    2e98:	e28dd004 	add	sp, sp, #4	; 0x4
    2e9c:	e8bd8030 	pop	{r4, r5, pc}

00002ea0 <cmd_usage>:
    2ea0:	e92d4030 	push	{r4, r5, lr}
    2ea4:	e1a04000 	mov	r4, r0
    2ea8:	e24dd004 	sub	sp, sp, #4	; 0x4
    2eac:	e59f5054 	ldr	r5, [pc, #84]	; 2f08 <cmd_usage+0x68>
    2eb0:	e59f0054 	ldr	r0, [pc, #84]	; 2f0c <cmd_usage+0x6c>
    2eb4:	e5941000 	ldr	r1, [r4]
    2eb8:	e5942010 	ldr	r2, [r4, #16]
    2ebc:	e1a0e00f 	mov	lr, pc
    2ec0:	e595f000 	ldr	pc, [r5]
    2ec4:	e59f0044 	ldr	r0, [pc, #68]	; 2f10 <cmd_usage+0x70>
    2ec8:	e5941000 	ldr	r1, [r4]
    2ecc:	e1a0e00f 	mov	lr, pc
    2ed0:	e595f000 	ldr	pc, [r5]
    2ed4:	e5940014 	ldr	r0, [r4, #20]
    2ed8:	e3500000 	cmp	r0, #0	; 0x0
    2edc:	1a000003 	bne	2ef0 <cmd_usage+0x50>
    2ee0:	e59f002c 	ldr	r0, [pc, #44]	; 2f14 <cmd_usage+0x74>
    2ee4:	eb003e12 	bl	12734 <puts>
    2ee8:	e3a00001 	mov	r0, #1	; 0x1
    2eec:	ea000003 	b	2f00 <cmd_usage+0x60>
    2ef0:	eb003e0f 	bl	12734 <puts>
    2ef4:	e3a0000a 	mov	r0, #10	; 0xa
    2ef8:	eb003e35 	bl	127d4 <putc>
    2efc:	e3a00000 	mov	r0, #0	; 0x0
    2f00:	e28dd004 	add	sp, sp, #4	; 0x4
    2f04:	e8bd8030 	pop	{r4, r5, pc}
    2f08:	3c00d6cc 	.word	0x3c00d6cc
    2f0c:	00016be7 	.word	0x00016be7
    2f10:	00016bf1 	.word	0x00016bf1
    2f14:	00016bfc 	.word	0x00016bfc

00002f18 <find_cmd_tbl>:
    2f18:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    2f1c:	e1a05001 	mov	r5, r1
    2f20:	e24dd004 	sub	sp, sp, #4	; 0x4
    2f24:	e3a0102e 	mov	r1, #46	; 0x2e
    2f28:	e1a07000 	mov	r7, r0
    2f2c:	e1a08002 	mov	r8, r2
    2f30:	ebfff962 	bl	14c0 <strchr>
    2f34:	e3500000 	cmp	r0, #0	; 0x0
    2f38:	10676000 	rsbne	r6, r7, r0
    2f3c:	1a000002 	bne	2f4c <find_cmd_tbl+0x34>
    2f40:	e1a00007 	mov	r0, r7
    2f44:	ebfff968 	bl	14ec <strlen>
    2f48:	e1a06000 	mov	r6, r0
    2f4c:	e3a03018 	mov	r3, #24	; 0x18
    2f50:	e02a5893 	mla	sl, r3, r8, r5
    2f54:	e1a04005 	mov	r4, r5
    2f58:	e1a08005 	mov	r8, r5
    2f5c:	e3a05000 	mov	r5, #0	; 0x0
    2f60:	ea00000a 	b	2f90 <find_cmd_tbl+0x78>
    2f64:	e5941000 	ldr	r1, [r4]
    2f68:	ebfff940 	bl	1470 <strncmp>
    2f6c:	e3500000 	cmp	r0, #0	; 0x0
    2f70:	1a000005 	bne	2f8c <find_cmd_tbl+0x74>
    2f74:	e5940000 	ldr	r0, [r4]
    2f78:	ebfff95b 	bl	14ec <strlen>
    2f7c:	e1560000 	cmp	r6, r0
    2f80:	0a000009 	beq	2fac <find_cmd_tbl+0x94>
    2f84:	e2855001 	add	r5, r5, #1	; 0x1
    2f88:	e1a08004 	mov	r8, r4
    2f8c:	e2844018 	add	r4, r4, #24	; 0x18
    2f90:	e154000a 	cmp	r4, sl
    2f94:	e1a02006 	mov	r2, r6
    2f98:	e1a00007 	mov	r0, r7
    2f9c:	1afffff0 	bne	2f64 <find_cmd_tbl+0x4c>
    2fa0:	e3550001 	cmp	r5, #1	; 0x1
    2fa4:	01a04008 	moveq	r4, r8
    2fa8:	13a04000 	movne	r4, #0	; 0x0
    2fac:	e1a00004 	mov	r0, r4
    2fb0:	e28dd004 	add	sp, sp, #4	; 0x4
    2fb4:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00002fb8 <find_cmd>:
    2fb8:	e59f1014 	ldr	r1, [pc, #20]	; 2fd4 <find_cmd+0x1c>
    2fbc:	e59f2014 	ldr	r2, [pc, #20]	; 2fd8 <find_cmd+0x20>
    2fc0:	e59f3014 	ldr	r3, [pc, #20]	; 2fdc <find_cmd+0x24>
    2fc4:	e0612002 	rsb	r2, r1, r2
    2fc8:	e1a021c2 	asr	r2, r2, #3
    2fcc:	e0020293 	mul	r2, r3, r2
    2fd0:	eaffffd0 	b	2f18 <find_cmd_tbl>
    2fd4:	00018edc 	.word	0x00018edc
    2fd8:	00019254 	.word	0x00019254
    2fdc:	aaaaaaab 	.word	0xaaaaaaab

00002fe0 <_do_help>:
    2fe0:	e1a0c00d 	mov	ip, sp
    2fe4:	e92ddff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}
    2fe8:	e24cb004 	sub	fp, ip, #4	; 0x4
    2fec:	e24dd004 	sub	sp, sp, #4	; 0x4
    2ff0:	e59b8004 	ldr	r8, [fp, #4]
    2ff4:	e1a07000 	mov	r7, r0
    2ff8:	e3580001 	cmp	r8, #1	; 0x1
    2ffc:	e1a0a001 	mov	sl, r1
    3000:	159b5008 	ldrne	r5, [fp, #8]
    3004:	13a06001 	movne	r6, #1	; 0x1
    3008:	13a04000 	movne	r4, #0	; 0x0
    300c:	159f9158 	ldrne	r9, [pc, #344]	; 316c <_do_help+0x18c>
    3010:	1a00004e 	bne	3150 <_do_help+0x170>
    3014:	e1a03101 	lsl	r3, r1, #2
    3018:	e283300e 	add	r3, r3, #14	; 0xe
    301c:	e3c33007 	bic	r3, r3, #7	; 0x7
    3020:	e1a0900d 	mov	r9, sp
    3024:	e063d00d 	rsb	sp, r3, sp
    3028:	e1a0800d 	mov	r8, sp
    302c:	e3a03000 	mov	r3, #0	; 0x0
    3030:	ea000002 	b	3040 <_do_help+0x60>
    3034:	e7880103 	str	r0, [r8, r3, lsl #2]
    3038:	e2800018 	add	r0, r0, #24	; 0x18
    303c:	e2833001 	add	r3, r3, #1	; 0x1
    3040:	e153000a 	cmp	r3, sl
    3044:	bafffffa 	blt	3034 <_do_help+0x54>
    3048:	e24a7001 	sub	r7, sl, #1	; 0x1
    304c:	ea000015 	b	30a8 <_do_help+0xc8>
    3050:	e5943000 	ldr	r3, [r4]
    3054:	e5942004 	ldr	r2, [r4, #4]
    3058:	e5930000 	ldr	r0, [r3]
    305c:	e5921000 	ldr	r1, [r2]
    3060:	ebfff8f4 	bl	1438 <strcmp>
    3064:	e3500000 	cmp	r0, #0	; 0x0
    3068:	c5942000 	ldrgt	r2, [r4]
    306c:	c5943004 	ldrgt	r3, [r4, #4]
    3070:	e2855001 	add	r5, r5, #1	; 0x1
    3074:	c5843000 	strgt	r3, [r4]
    3078:	c5842004 	strgt	r2, [r4, #4]
    307c:	c2866001 	addgt	r6, r6, #1	; 0x1
    3080:	e1550007 	cmp	r5, r7
    3084:	e2844004 	add	r4, r4, #4	; 0x4
    3088:	bafffff0 	blt	3050 <_do_help+0x70>
    308c:	e3560000 	cmp	r6, #0	; 0x0
    3090:	1a000003 	bne	30a4 <_do_help+0xc4>
    3094:	e3a05000 	mov	r5, #0	; 0x0
    3098:	e1a04005 	mov	r4, r5
    309c:	e59f70c8 	ldr	r7, [pc, #200]	; 316c <_do_help+0x18c>
    30a0:	ea000016 	b	3100 <_do_help+0x120>
    30a4:	e2477001 	sub	r7, r7, #1	; 0x1
    30a8:	e3570000 	cmp	r7, #0	; 0x0
    30ac:	dafffff8 	ble	3094 <_do_help+0xb4>
    30b0:	e3a05000 	mov	r5, #0	; 0x0
    30b4:	e1a04008 	mov	r4, r8
    30b8:	e1a06005 	mov	r6, r5
    30bc:	eaffffe3 	b	3050 <_do_help+0x70>
    30c0:	e7983004 	ldr	r3, [r8, r4]
    30c4:	e5936010 	ldr	r6, [r3, #16]
    30c8:	eb003dc4 	bl	127e0 <ctrlc>
    30cc:	e3500000 	cmp	r0, #0	; 0x0
    30d0:	e1a03006 	mov	r3, r6
    30d4:	e3a01008 	mov	r1, #8	; 0x8
    30d8:	e59f0090 	ldr	r0, [pc, #144]	; 3170 <_do_help+0x190>
    30dc:	13a04001 	movne	r4, #1	; 0x1
    30e0:	1a00000a 	bne	3110 <_do_help+0x130>
    30e4:	e3560000 	cmp	r6, #0	; 0x0
    30e8:	0a000003 	beq	30fc <_do_help+0x11c>
    30ec:	e7982004 	ldr	r2, [r8, r4]
    30f0:	e5922000 	ldr	r2, [r2]
    30f4:	e1a0e00f 	mov	lr, pc
    30f8:	e597f000 	ldr	pc, [r7]
    30fc:	e2844004 	add	r4, r4, #4	; 0x4
    3100:	e155000a 	cmp	r5, sl
    3104:	e2855001 	add	r5, r5, #1	; 0x1
    3108:	baffffec 	blt	30c0 <_do_help+0xe0>
    310c:	e3a04000 	mov	r4, #0	; 0x0
    3110:	e1a0d009 	mov	sp, r9
    3114:	ea000011 	b	3160 <_do_help+0x180>
    3118:	e5950004 	ldr	r0, [r5, #4]
    311c:	ebffff7d 	bl	2f18 <find_cmd_tbl>
    3120:	e3500000 	cmp	r0, #0	; 0x0
    3124:	0a000002 	beq	3134 <_do_help+0x154>
    3128:	ebffff5c 	bl	2ea0 <cmd_usage>
    312c:	e1844000 	orr	r4, r4, r0
    3130:	ea000004 	b	3148 <_do_help+0x168>
    3134:	e5951004 	ldr	r1, [r5, #4]
    3138:	e59f0034 	ldr	r0, [pc, #52]	; 3174 <_do_help+0x194>
    313c:	e1a0e00f 	mov	lr, pc
    3140:	e599f000 	ldr	pc, [r9]
    3144:	e3a04001 	mov	r4, #1	; 0x1
    3148:	e2866001 	add	r6, r6, #1	; 0x1
    314c:	e2855004 	add	r5, r5, #4	; 0x4
    3150:	e1560008 	cmp	r6, r8
    3154:	e1a01007 	mov	r1, r7
    3158:	e1a0200a 	mov	r2, sl
    315c:	baffffed 	blt	3118 <_do_help+0x138>
    3160:	e1a00004 	mov	r0, r4
    3164:	e24bd028 	sub	sp, fp, #40	; 0x28
    3168:	e89daff0 	ldm	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
    316c:	3c00d6cc 	.word	0x3c00d6cc
    3170:	00016c1d 	.word	0x00016c1d
    3174:	00016c27 	.word	0x00016c27

00003178 <do_help>:
    3178:	e92d4030 	push	{r4, r5, lr}
    317c:	e59fc038 	ldr	ip, [pc, #56]	; 31bc <do_help+0x44>
    3180:	e59fe038 	ldr	lr, [pc, #56]	; 31c0 <do_help+0x48>
    3184:	e1a05001 	mov	r5, r1
    3188:	e06ec00c 	rsb	ip, lr, ip
    318c:	e59f1030 	ldr	r1, [pc, #48]	; 31c4 <do_help+0x4c>
    3190:	e24dd00c 	sub	sp, sp, #12	; 0xc
    3194:	e1a04000 	mov	r4, r0
    3198:	e1a0c1cc 	asr	ip, ip, #3
    319c:	e88d000c 	stm	sp, {r2, r3}
    31a0:	e0010c91 	mul	r1, r1, ip
    31a4:	e1a0000e 	mov	r0, lr
    31a8:	e1a02004 	mov	r2, r4
    31ac:	e1a03005 	mov	r3, r5
    31b0:	ebffff8a 	bl	2fe0 <_do_help>
    31b4:	e28dd00c 	add	sp, sp, #12	; 0xc
    31b8:	e8bd8030 	pop	{r4, r5, pc}
    31bc:	00019254 	.word	0x00019254
    31c0:	00018edc 	.word	0x00018edc
    31c4:	aaaaaaab 	.word	0xaaaaaaab

000031c8 <tt_blk_trolling>:
    31c8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    31cc:	e3a01001 	mov	r1, #1	; 0x1
    31d0:	e24dd02c 	sub	sp, sp, #44	; 0x2c
    31d4:	e1a04002 	mov	r4, r2
    31d8:	e1a05003 	mov	r5, r3
    31dc:	e58d0010 	str	r0, [sp, #16]
    31e0:	e59da050 	ldr	sl, [sp, #80]
    31e4:	eb000cda 	bl	6554 <vs_assign_by_name>
    31e8:	e3500000 	cmp	r0, #0	; 0x0
    31ec:	0a000005 	beq	3208 <tt_blk_trolling+0x40>
    31f0:	e59d1010 	ldr	r1, [sp, #16]
    31f4:	e59f03d4 	ldr	r0, [pc, #980]	; 35d0 <tt_blk_trolling+0x408>
    31f8:	e59f33d4 	ldr	r3, [pc, #980]	; 35d4 <tt_blk_trolling+0x40c>
    31fc:	e1a0e00f 	mov	lr, pc
    3200:	e593f000 	ldr	pc, [r3]
    3204:	ea000026 	b	32a4 <tt_blk_trolling+0xdc>
    3208:	e1940005 	orrs	r0, r4, r5
    320c:	03a04502 	moveq	r4, #8388608	; 0x800000
    3210:	03a06502 	moveq	r6, #8388608	; 0x800000
    3214:	03a07000 	moveq	r7, #0	; 0x0
    3218:	0a00000b 	beq	324c <tt_blk_trolling+0x84>
    321c:	e3550000 	cmp	r5, #0	; 0x0
    3220:	1a000006 	bne	3240 <tt_blk_trolling+0x78>
    3224:	e3540502 	cmp	r4, #8388608	; 0x800000
    3228:	2a000004 	bcs	3240 <tt_blk_trolling+0x78>
    322c:	e3a0000e 	mov	r0, #14	; 0xe
    3230:	eb000b55 	bl	5f8c <vs_align>
    3234:	e1a06004 	mov	r6, r4
    3238:	e1a07005 	mov	r7, r5
    323c:	ea000002 	b	324c <tt_blk_trolling+0x84>
    3240:	e1a06004 	mov	r6, r4
    3244:	e1a07005 	mov	r7, r5
    3248:	e3a04502 	mov	r4, #8388608	; 0x800000
    324c:	e3a0000e 	mov	r0, #14	; 0xe
    3250:	eb000b4d 	bl	5f8c <vs_align>
    3254:	e1a01000 	mov	r1, r0
    3258:	e1a00004 	mov	r0, r4
    325c:	eb00454a 	bl	1478c <__aeabi_uidiv>
    3260:	e1a01000 	mov	r1, r0
    3264:	e3a00000 	mov	r0, #0	; 0x0
    3268:	eb00054e 	bl	47a8 <tt_rnd_get_int>
    326c:	e1a04000 	mov	r4, r0
    3270:	e3a0000e 	mov	r0, #14	; 0xe
    3274:	eb000b44 	bl	5f8c <vs_align>
    3278:	e0040094 	mul	r4, r4, r0
    327c:	e28d0024 	add	r0, sp, #36	; 0x24
    3280:	e1a01004 	mov	r1, r4
    3284:	e1a02004 	mov	r2, r4
    3288:	eb00056f 	bl	484c <tt_rnd_get_data>
    328c:	e250b000 	subs	fp, r0, #0	; 0x0
    3290:	1a000005 	bne	32ac <tt_blk_trolling+0xe4>
    3294:	e59f033c 	ldr	r0, [pc, #828]	; 35d8 <tt_blk_trolling+0x410>
    3298:	e59f3334 	ldr	r3, [pc, #820]	; 35d4 <tt_blk_trolling+0x40c>
    329c:	e1a0e00f 	mov	lr, pc
    32a0:	e593f000 	ldr	pc, [r3]
    32a4:	e3e00000 	mvn	r0, #0	; 0x0
    32a8:	ea0000c6 	b	35c8 <tt_blk_trolling+0x400>
    32ac:	e59d3024 	ldr	r3, [sp, #36]
    32b0:	e3a0000e 	mov	r0, #14	; 0xe
    32b4:	e0564003 	subs	r4, r6, r3
    32b8:	e0c75fc3 	sbc	r5, r7, r3, asr #31
    32bc:	eb000b32 	bl	5f8c <vs_align>
    32c0:	e3550000 	cmp	r5, #0	; 0x0
    32c4:	e1cd41f8 	strd	r4, [sp, #24]
    32c8:	1a000005 	bne	32e4 <tt_blk_trolling+0x11c>
    32cc:	e1a01000 	mov	r1, r0
    32d0:	e1a00004 	mov	r0, r4
    32d4:	eb00452c 	bl	1478c <__aeabi_uidiv>
    32d8:	e58d501c 	str	r5, [sp, #28]
    32dc:	e58d0018 	str	r0, [sp, #24]
    32e0:	ea000002 	b	32f0 <tt_blk_trolling+0x128>
    32e4:	e1a01000 	mov	r1, r0
    32e8:	e28d0018 	add	r0, sp, #24	; 0x18
    32ec:	eb0038e5 	bl	11688 <__div64_32>
    32f0:	e59d1018 	ldr	r1, [sp, #24]
    32f4:	e3a00000 	mov	r0, #0	; 0x0
    32f8:	eb00052a 	bl	47a8 <tt_rnd_get_int>
    32fc:	e1a04000 	mov	r4, r0
    3300:	e3a0000e 	mov	r0, #14	; 0xe
    3304:	eb000b20 	bl	5f8c <vs_align>
    3308:	e0c98490 	smull	r8, r9, r0, r4
    330c:	e59d1010 	ldr	r1, [sp, #16]
    3310:	e1a0200b 	mov	r2, fp
    3314:	e59d3024 	ldr	r3, [sp, #36]
    3318:	e59f02bc 	ldr	r0, [pc, #700]	; 35dc <tt_blk_trolling+0x414>
    331c:	e88d0300 	stm	sp, {r8, r9}
    3320:	e59f42ac 	ldr	r4, [pc, #684]	; 35d4 <tt_blk_trolling+0x40c>
    3324:	e1a0e00f 	mov	lr, pc
    3328:	e594f000 	ldr	pc, [r4]
    332c:	e59d0010 	ldr	r0, [sp, #16]
    3330:	eb000b96 	bl	6190 <vs_device_id>
    3334:	eb000b81 	bl	6140 <vs_device_erasable>
    3338:	e3500000 	cmp	r0, #0	; 0x0
    333c:	0a000006 	beq	335c <tt_blk_trolling+0x194>
    3340:	e59f0298 	ldr	r0, [pc, #664]	; 35e0 <tt_blk_trolling+0x418>
    3344:	e1a0e00f 	mov	lr, pc
    3348:	e594f000 	ldr	pc, [r4]
    334c:	e1a00008 	mov	r0, r8
    3350:	e1a01009 	mov	r1, r9
    3354:	e59d2024 	ldr	r2, [sp, #36]
    3358:	eb000acf 	bl	5e9c <vs_erase>
    335c:	e59f0280 	ldr	r0, [pc, #640]	; 35e4 <tt_blk_trolling+0x41c>
    3360:	e59f326c 	ldr	r3, [pc, #620]	; 35d4 <tt_blk_trolling+0x40c>
    3364:	e1a0e00f 	mov	lr, pc
    3368:	e593f000 	ldr	pc, [r3]
    336c:	e35a0000 	cmp	sl, #0	; 0x0
    3370:	13a07000 	movne	r7, #0	; 0x0
    3374:	159d6024 	ldrne	r6, [sp, #36]
    3378:	1a00001c 	bne	33f0 <tt_blk_trolling+0x228>
    337c:	ea00001f 	b	3400 <tt_blk_trolling+0x238>
    3380:	e3560902 	cmp	r6, #32768	; 0x8000
    3384:	b1a01006 	movlt	r1, r6
    3388:	a3a01902 	movge	r1, #32768	; 0x8000
    338c:	e58d100c 	str	r1, [sp, #12]
    3390:	e59d1024 	ldr	r1, [sp, #36]
    3394:	e59d200c 	ldr	r2, [sp, #12]
    3398:	e0661001 	rsb	r1, r6, r1
    339c:	e08b1001 	add	r1, fp, r1
    33a0:	ebfff86b 	bl	1554 <memcpy>
    33a4:	e3a00000 	mov	r0, #0	; 0x0
    33a8:	eb0029af 	bl	da6c <get_timer>
    33ac:	e59d3024 	ldr	r3, [sp, #36]
    33b0:	e3a01000 	mov	r1, #0	; 0x0
    33b4:	e0663003 	rsb	r3, r6, r3
    33b8:	e0984003 	adds	r4, r8, r3
    33bc:	e0a95fc3 	adc	r5, r9, r3, asr #31
    33c0:	e1a03005 	mov	r3, r5
    33c4:	e59d500c 	ldr	r5, [sp, #12]
    33c8:	e58d0008 	str	r0, [sp, #8]
    33cc:	e1a02004 	mov	r2, r4
    33d0:	e1a0000a 	mov	r0, sl
    33d4:	e58d1004 	str	r1, [sp, #4]
    33d8:	e58d5000 	str	r5, [sp]
    33dc:	eb000a98 	bl	5e44 <vs_write>
    33e0:	e59d0008 	ldr	r0, [sp, #8]
    33e4:	eb0029a0 	bl	da6c <get_timer>
    33e8:	e0656006 	rsb	r6, r5, r6
    33ec:	e0877000 	add	r7, r7, r0
    33f0:	e3560000 	cmp	r6, #0	; 0x0
    33f4:	e1a0000a 	mov	r0, sl
    33f8:	1affffe0 	bne	3380 <tt_blk_trolling+0x1b8>
    33fc:	ea00000b 	b	3430 <tt_blk_trolling+0x268>
    3400:	e1a0000a 	mov	r0, sl
    3404:	eb002998 	bl	da6c <get_timer>
    3408:	e59d1024 	ldr	r1, [sp, #36]
    340c:	e1a04000 	mov	r4, r0
    3410:	e1a02008 	mov	r2, r8
    3414:	e1a03009 	mov	r3, r9
    3418:	e1a0000b 	mov	r0, fp
    341c:	e88d0402 	stm	sp, {r1, sl}
    3420:	eb000a87 	bl	5e44 <vs_write>
    3424:	e1a00004 	mov	r0, r4
    3428:	eb00298f 	bl	da6c <get_timer>
    342c:	e1a07000 	mov	r7, r0
    3430:	e59d0024 	ldr	r0, [sp, #36]
    3434:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    3438:	e1a00540 	asr	r0, r0, #10
    343c:	e1a01007 	mov	r1, r7
    3440:	e0000093 	mul	r0, r3, r0
    3444:	eb0044d0 	bl	1478c <__aeabi_uidiv>
    3448:	e59f3184 	ldr	r3, [pc, #388]	; 35d4 <tt_blk_trolling+0x40c>
    344c:	e58d0014 	str	r0, [sp, #20]
    3450:	e59f0190 	ldr	r0, [pc, #400]	; 35e8 <tt_blk_trolling+0x420>
    3454:	e1a0e00f 	mov	lr, pc
    3458:	e593f000 	ldr	pc, [r3]
    345c:	e35a0000 	cmp	sl, #0	; 0x0
    3460:	13a07000 	movne	r7, #0	; 0x0
    3464:	159d6024 	ldrne	r6, [sp, #36]
    3468:	1a00001b 	bne	34dc <tt_blk_trolling+0x314>
    346c:	ea00001e 	b	34ec <tt_blk_trolling+0x324>
    3470:	eb00297d 	bl	da6c <get_timer>
    3474:	e59d3024 	ldr	r3, [sp, #36]
    3478:	e3560902 	cmp	r6, #32768	; 0x8000
    347c:	b1a04006 	movlt	r4, r6
    3480:	a3a04902 	movge	r4, #32768	; 0x8000
    3484:	e0663003 	rsb	r3, r6, r3
    3488:	e1a05000 	mov	r5, r0
    348c:	e0980003 	adds	r0, r8, r3
    3490:	e0a91fc3 	adc	r1, r9, r3, asr #31
    3494:	e1a02000 	mov	r2, r0
    3498:	e1a03001 	mov	r3, r1
    349c:	e1a0000a 	mov	r0, sl
    34a0:	e3a01000 	mov	r1, #0	; 0x0
    34a4:	e58d1004 	str	r1, [sp, #4]
    34a8:	e58d4000 	str	r4, [sp]
    34ac:	eb000b64 	bl	6244 <vs_read>
    34b0:	e1a00005 	mov	r0, r5
    34b4:	eb00296c 	bl	da6c <get_timer>
    34b8:	e1a05000 	mov	r5, r0
    34bc:	e59d0024 	ldr	r0, [sp, #36]
    34c0:	e1a0100a 	mov	r1, sl
    34c4:	e0660000 	rsb	r0, r6, r0
    34c8:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    34cc:	e1a02004 	mov	r2, r4
    34d0:	ebfff81f 	bl	1554 <memcpy>
    34d4:	e0877005 	add	r7, r7, r5
    34d8:	e0646006 	rsb	r6, r4, r6
    34dc:	e3a00000 	mov	r0, #0	; 0x0
    34e0:	e1560000 	cmp	r6, r0
    34e4:	1affffe1 	bne	3470 <tt_blk_trolling+0x2a8>
    34e8:	ea00000b 	b	351c <tt_blk_trolling+0x354>
    34ec:	e1a0000a 	mov	r0, sl
    34f0:	eb00295d 	bl	da6c <get_timer>
    34f4:	e59d1024 	ldr	r1, [sp, #36]
    34f8:	e1a04000 	mov	r4, r0
    34fc:	e1a02008 	mov	r2, r8
    3500:	e1a03009 	mov	r3, r9
    3504:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3508:	e88d0402 	stm	sp, {r1, sl}
    350c:	eb000b4c 	bl	6244 <vs_read>
    3510:	e1a00004 	mov	r0, r4
    3514:	eb002954 	bl	da6c <get_timer>
    3518:	e1a07000 	mov	r7, r0
    351c:	e59d0024 	ldr	r0, [sp, #36]
    3520:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    3524:	e1a00540 	asr	r0, r0, #10
    3528:	e0000093 	mul	r0, r3, r0
    352c:	e1a01007 	mov	r1, r7
    3530:	eb004495 	bl	1478c <__aeabi_uidiv>
    3534:	e59f4098 	ldr	r4, [pc, #152]	; 35d4 <tt_blk_trolling+0x40c>
    3538:	e1a05000 	mov	r5, r0
    353c:	e59f00a8 	ldr	r0, [pc, #168]	; 35ec <tt_blk_trolling+0x424>
    3540:	e1a0e00f 	mov	lr, pc
    3544:	e594f000 	ldr	pc, [r4]
    3548:	e1a0000b 	mov	r0, fp
    354c:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    3550:	e59d2024 	ldr	r2, [sp, #36]
    3554:	e3a0300c 	mov	r3, #12	; 0xc
    3558:	eb00043e 	bl	4658 <tt_cmp_buffer>
    355c:	e3500000 	cmp	r0, #0	; 0x0
    3560:	0a000007 	beq	3584 <tt_blk_trolling+0x3bc>
    3564:	e58d0000 	str	r0, [sp]
    3568:	e59d1010 	ldr	r1, [sp, #16]
    356c:	e59d2014 	ldr	r2, [sp, #20]
    3570:	e1a03005 	mov	r3, r5
    3574:	e59f0074 	ldr	r0, [pc, #116]	; 35f0 <tt_blk_trolling+0x428>
    3578:	e1a0e00f 	mov	lr, pc
    357c:	e594f000 	ldr	pc, [r4]
    3580:	ea000005 	b	359c <tt_blk_trolling+0x3d4>
    3584:	e59d1010 	ldr	r1, [sp, #16]
    3588:	e59d2014 	ldr	r2, [sp, #20]
    358c:	e1a03005 	mov	r3, r5
    3590:	e59f005c 	ldr	r0, [pc, #92]	; 35f4 <tt_blk_trolling+0x42c>
    3594:	e1a0e00f 	mov	lr, pc
    3598:	e594f000 	ldr	pc, [r4]
    359c:	e59f0054 	ldr	r0, [pc, #84]	; 35f8 <tt_blk_trolling+0x430>
    35a0:	e59d1024 	ldr	r1, [sp, #36]
    35a4:	e890000c 	ldm	r0, {r2, r3}
    35a8:	e0922001 	adds	r2, r2, r1
    35ac:	e0a33fc1 	adc	r3, r3, r1, asr #31
    35b0:	e880000c 	stm	r0, {r2, r3}
    35b4:	e59f0040 	ldr	r0, [pc, #64]	; 35fc <tt_blk_trolling+0x434>
    35b8:	e59f3014 	ldr	r3, [pc, #20]	; 35d4 <tt_blk_trolling+0x40c>
    35bc:	e1a0e00f 	mov	lr, pc
    35c0:	e593f000 	ldr	pc, [r3]
    35c4:	e3a00000 	mov	r0, #0	; 0x0
    35c8:	e28dd02c 	add	sp, sp, #44	; 0x2c
    35cc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    35d0:	00016dce 	.word	0x00016dce
    35d4:	3c00d6cc 	.word	0x3c00d6cc
    35d8:	00016dec 	.word	0x00016dec
    35dc:	00016e0c 	.word	0x00016e0c
    35e0:	00018a64 	.word	0x00018a64
    35e4:	000165bc 	.word	0x000165bc
    35e8:	0001791e 	.word	0x0001791e
    35ec:	00016e40 	.word	0x00016e40
    35f0:	00016e43 	.word	0x00016e43
    35f4:	00016e7d 	.word	0x00016e7d
    35f8:	3c00d138 	.word	0x3c00d138
    35fc:	00016bef 	.word	0x00016bef

00003600 <do_tt_blk>:
    3600:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3604:	e3520001 	cmp	r2, #1	; 0x1
    3608:	e24dd00c 	sub	sp, sp, #12	; 0xc
    360c:	e1a05002 	mov	r5, r2
    3610:	e1a07003 	mov	r7, r3
    3614:	ca000005 	bgt	3630 <do_tt_blk+0x30>
    3618:	e59f01bc 	ldr	r0, [pc, #444]	; 37dc <do_tt_blk+0x1dc>
    361c:	e59f31bc 	ldr	r3, [pc, #444]	; 37e0 <do_tt_blk+0x1e0>
    3620:	e1a0e00f 	mov	lr, pc
    3624:	e593f000 	ldr	pc, [r3]
    3628:	e3e00000 	mvn	r0, #0	; 0x0
    362c:	ea000068 	b	37d4 <do_tt_blk+0x1d4>
    3630:	e3520002 	cmp	r2, #2	; 0x2
    3634:	03a04000 	moveq	r4, #0	; 0x0
    3638:	01a06004 	moveq	r6, r4
    363c:	0a000025 	beq	36d8 <do_tt_blk+0xd8>
    3640:	e3a01000 	mov	r1, #0	; 0x0
    3644:	e3a0200a 	mov	r2, #10	; 0xa
    3648:	e5930008 	ldr	r0, [r3, #8]
    364c:	eb00384a 	bl	1177c <simple_strtoul>
    3650:	e59f118c 	ldr	r1, [pc, #396]	; 37e4 <do_tt_blk+0x1e4>
    3654:	e1a04000 	mov	r4, r0
    3658:	e3a02005 	mov	r2, #5	; 0x5
    365c:	e5970004 	ldr	r0, [r7, #4]
    3660:	ebfff782 	bl	1470 <strncmp>
    3664:	e3500000 	cmp	r0, #0	; 0x0
    3668:	059f0178 	ldreq	r0, [pc, #376]	; 37e8 <do_tt_blk+0x1e8>
    366c:	01a01004 	moveq	r1, r4
    3670:	059f3168 	ldreq	r3, [pc, #360]	; 37e0 <do_tt_blk+0x1e0>
    3674:	0a000008 	beq	369c <do_tt_blk+0x9c>
    3678:	e59f116c 	ldr	r1, [pc, #364]	; 37ec <do_tt_blk+0x1ec>
    367c:	e5970004 	ldr	r0, [r7, #4]
    3680:	e3a02003 	mov	r2, #3	; 0x3
    3684:	ebfff779 	bl	1470 <strncmp>
    3688:	e3500000 	cmp	r0, #0	; 0x0
    368c:	e59f314c 	ldr	r3, [pc, #332]	; 37e0 <do_tt_blk+0x1e0>
    3690:	059f0150 	ldreq	r0, [pc, #336]	; 37e8 <do_tt_blk+0x1e8>
    3694:	159f0154 	ldrne	r0, [pc, #340]	; 37f0 <do_tt_blk+0x1f0>
    3698:	e1a01004 	mov	r1, r4
    369c:	e1a0e00f 	mov	lr, pc
    36a0:	e593f000 	ldr	pc, [r3]
    36a4:	e3550003 	cmp	r5, #3	; 0x3
    36a8:	03a06000 	moveq	r6, #0	; 0x0
    36ac:	0a000009 	beq	36d8 <do_tt_blk+0xd8>
    36b0:	e3a01000 	mov	r1, #0	; 0x0
    36b4:	e597000c 	ldr	r0, [r7, #12]
    36b8:	e3a02010 	mov	r2, #16	; 0x10
    36bc:	eb00382e 	bl	1177c <simple_strtoul>
    36c0:	e1a06000 	mov	r6, r0
    36c4:	e1a01006 	mov	r1, r6
    36c8:	e59f0124 	ldr	r0, [pc, #292]	; 37f4 <do_tt_blk+0x1f4>
    36cc:	e59f310c 	ldr	r3, [pc, #268]	; 37e0 <do_tt_blk+0x1e0>
    36d0:	e1a0e00f 	mov	lr, pc
    36d4:	e593f000 	ldr	pc, [r3]
    36d8:	e3a00000 	mov	r0, #0	; 0x0
    36dc:	eb0028e2 	bl	da6c <get_timer>
    36e0:	e59f1110 	ldr	r1, [pc, #272]	; 37f8 <do_tt_blk+0x1f8>
    36e4:	e3a02000 	mov	r2, #0	; 0x0
    36e8:	e3a03000 	mov	r3, #0	; 0x0
    36ec:	e881000c 	stm	r1, {r2, r3}
    36f0:	e1a09a04 	lsl	r9, r4, #20
    36f4:	e2875004 	add	r5, r7, #4	; 0x4
    36f8:	e1a08b24 	lsr	r8, r4, #22
    36fc:	e1a07504 	lsl	r7, r4, #10
    3700:	e1a0a624 	lsr	sl, r4, #12
    3704:	e1a0b000 	mov	fp, r0
    3708:	e3a04000 	mov	r4, #0	; 0x0
    370c:	e59f30cc 	ldr	r3, [pc, #204]	; 37e0 <do_tt_blk+0x1e0>
    3710:	e1a01004 	mov	r1, r4
    3714:	e59f00e0 	ldr	r0, [pc, #224]	; 37fc <do_tt_blk+0x1fc>
    3718:	e1a0e00f 	mov	lr, pc
    371c:	e593f000 	ldr	pc, [r3]
    3720:	e59f10bc 	ldr	r1, [pc, #188]	; 37e4 <do_tt_blk+0x1e4>
    3724:	e3a02005 	mov	r2, #5	; 0x5
    3728:	e5950000 	ldr	r0, [r5]
    372c:	ebfff74f 	bl	1470 <strncmp>
    3730:	e3500000 	cmp	r0, #0	; 0x0
    3734:	e3a02003 	mov	r2, #3	; 0x3
    3738:	e59f10ac 	ldr	r1, [pc, #172]	; 37ec <do_tt_blk+0x1ec>
    373c:	e1a03008 	mov	r3, r8
    3740:	05950000 	ldreq	r0, [r5]
    3744:	01a02007 	moveq	r2, r7
    3748:	0a000008 	beq	3770 <do_tt_blk+0x170>
    374c:	e5950000 	ldr	r0, [r5]
    3750:	ebfff746 	bl	1470 <strncmp>
    3754:	e3500000 	cmp	r0, #0	; 0x0
    3758:	05950000 	ldreq	r0, [r5]
    375c:	15950000 	ldrne	r0, [r5]
    3760:	e1a02009 	mov	r2, r9
    3764:	e1a0300a 	mov	r3, sl
    3768:	01a02007 	moveq	r2, r7
    376c:	01a03008 	moveq	r3, r8
    3770:	e58d6000 	str	r6, [sp]
    3774:	ebfffe93 	bl	31c8 <tt_blk_trolling>
    3778:	eb003c16 	bl	127d8 <tstc>
    377c:	e3500000 	cmp	r0, #0	; 0x0
    3780:	e2844001 	add	r4, r4, #1	; 0x1
    3784:	0affffe0 	beq	370c <do_tt_blk+0x10c>
    3788:	eb003c13 	bl	127dc <getc>
    378c:	e3500003 	cmp	r0, #3	; 0x3
    3790:	1affffdd 	bne	370c <do_tt_blk+0x10c>
    3794:	e1a0000b 	mov	r0, fp
    3798:	eb0028b3 	bl	da6c <get_timer>
    379c:	e59f3054 	ldr	r3, [pc, #84]	; 37f8 <do_tt_blk+0x1f8>
    37a0:	e30e1a60 	movw	r1, #60000	; 0xea60
    37a4:	e5934000 	ldr	r4, [r3]
    37a8:	e5933004 	ldr	r3, [r3, #4]
    37ac:	e1a04a24 	lsr	r4, r4, #20
    37b0:	e1844603 	orr	r4, r4, r3, lsl #12
    37b4:	eb0043f4 	bl	1478c <__aeabi_uidiv>
    37b8:	e1a01004 	mov	r1, r4
    37bc:	e1a02000 	mov	r2, r0
    37c0:	e59f3018 	ldr	r3, [pc, #24]	; 37e0 <do_tt_blk+0x1e0>
    37c4:	e59f0034 	ldr	r0, [pc, #52]	; 3800 <do_tt_blk+0x200>
    37c8:	e1a0e00f 	mov	lr, pc
    37cc:	e593f000 	ldr	pc, [r3]
    37d0:	e3a00000 	mov	r0, #0	; 0x0
    37d4:	e28dd00c 	add	sp, sp, #12	; 0xc
    37d8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    37dc:	00016eaa 	.word	0x00016eaa
    37e0:	3c00d6cc 	.word	0x3c00d6cc
    37e4:	00016ef1 	.word	0x00016ef1
    37e8:	00016ef7 	.word	0x00016ef7
    37ec:	000168ac 	.word	0x000168ac
    37f0:	00016f1b 	.word	0x00016f1b
    37f4:	00016f3f 	.word	0x00016f3f
    37f8:	3c00d138 	.word	0x3c00d138
    37fc:	00016f62 	.word	0x00016f62
    3800:	00016f6c 	.word	0x00016f6c

00003804 <__show_strbuf>:
    3804:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    3808:	e59f4058 	ldr	r4, [pc, #88]	; 3868 <__show_strbuf+0x64>
    380c:	e24dd004 	sub	sp, sp, #4	; 0x4
    3810:	e1a06001 	mov	r6, r1
    3814:	e1a01000 	mov	r1, r0
    3818:	e59f004c 	ldr	r0, [pc, #76]	; 386c <__show_strbuf+0x68>
    381c:	e1a05002 	mov	r5, r2
    3820:	e1a07004 	mov	r7, r4
    3824:	e1a0e00f 	mov	lr, pc
    3828:	e594f000 	ldr	pc, [r4]
    382c:	e3a04000 	mov	r4, #0	; 0x0
    3830:	ea000003 	b	3844 <__show_strbuf+0x40>
    3834:	e7d61004 	ldrb	r1, [r6, r4]
    3838:	e1a0e00f 	mov	lr, pc
    383c:	e597f000 	ldr	pc, [r7]
    3840:	e2844001 	add	r4, r4, #1	; 0x1
    3844:	e1540005 	cmp	r4, r5
    3848:	e59f0020 	ldr	r0, [pc, #32]	; 3870 <__show_strbuf+0x6c>
    384c:	bafffff8 	blt	3834 <__show_strbuf+0x30>
    3850:	e59f001c 	ldr	r0, [pc, #28]	; 3874 <__show_strbuf+0x70>
    3854:	e59f300c 	ldr	r3, [pc, #12]	; 3868 <__show_strbuf+0x64>
    3858:	e1a0e00f 	mov	lr, pc
    385c:	e593f000 	ldr	pc, [r3]
    3860:	e28dd004 	add	sp, sp, #4	; 0x4
    3864:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    3868:	3c00d6cc 	.word	0x3c00d6cc
    386c:	00016fba 	.word	0x00016fba
    3870:	00018c1d 	.word	0x00018c1d
    3874:	00016bef 	.word	0x00016bef

00003878 <do_crypto_quick>:
    3878:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    387c:	e24dd0f0 	sub	sp, sp, #240	; 0xf0
    3880:	e28da0e0 	add	sl, sp, #224	; 0xe0
    3884:	e3a04010 	mov	r4, #16	; 0x10
    3888:	e28d70c0 	add	r7, sp, #192	; 0xc0
    388c:	e1a02004 	mov	r2, r4
    3890:	e3a09020 	mov	r9, #32	; 0x20
    3894:	e59f11f0 	ldr	r1, [pc, #496]	; 3a8c <do_crypto_quick+0x214>
    3898:	e1a0000a 	mov	r0, sl
    389c:	e3a05040 	mov	r5, #64	; 0x40
    38a0:	e28d6080 	add	r6, sp, #128	; 0x80
    38a4:	ebfff72a 	bl	1554 <memcpy>
    38a8:	e1a02009 	mov	r2, r9
    38ac:	e59f11dc 	ldr	r1, [pc, #476]	; 3a90 <do_crypto_quick+0x218>
    38b0:	e1a00007 	mov	r0, r7
    38b4:	ebfff726 	bl	1554 <memcpy>
    38b8:	e08d8005 	add	r8, sp, r5
    38bc:	e1a02005 	mov	r2, r5
    38c0:	e59f11cc 	ldr	r1, [pc, #460]	; 3a94 <do_crypto_quick+0x21c>
    38c4:	e1a00006 	mov	r0, r6
    38c8:	ebfff721 	bl	1554 <memcpy>
    38cc:	e1a02005 	mov	r2, r5
    38d0:	e59f11c0 	ldr	r1, [pc, #448]	; 3a98 <do_crypto_quick+0x220>
    38d4:	e1a00008 	mov	r0, r8
    38d8:	ebfff71d 	bl	1554 <memcpy>
    38dc:	e1a02004 	mov	r2, r4
    38e0:	e1a0100a 	mov	r1, sl
    38e4:	e59f01b0 	ldr	r0, [pc, #432]	; 3a9c <do_crypto_quick+0x224>
    38e8:	ebffffc5 	bl	3804 <__show_strbuf>
    38ec:	e1a02009 	mov	r2, r9
    38f0:	e1a01007 	mov	r1, r7
    38f4:	e59f01a4 	ldr	r0, [pc, #420]	; 3aa0 <do_crypto_quick+0x228>
    38f8:	ebffffc1 	bl	3804 <__show_strbuf>
    38fc:	e1a01006 	mov	r1, r6
    3900:	e1a02005 	mov	r2, r5
    3904:	e59f0198 	ldr	r0, [pc, #408]	; 3aa4 <do_crypto_quick+0x22c>
    3908:	ebffffbd 	bl	3804 <__show_strbuf>
    390c:	e1a01008 	mov	r1, r8
    3910:	e1a02005 	mov	r2, r5
    3914:	e59f018c 	ldr	r0, [pc, #396]	; 3aa8 <do_crypto_quick+0x230>
    3918:	ebffffb9 	bl	3804 <__show_strbuf>
    391c:	e1a01007 	mov	r1, r7
    3920:	e1a0200a 	mov	r2, sl
    3924:	e3a00001 	mov	r0, #1	; 0x1
    3928:	eb000b4c 	bl	6660 <ss_aes_init>
    392c:	e1a0100d 	mov	r1, sp
    3930:	e1a02005 	mov	r2, r5
    3934:	e1a00006 	mov	r0, r6
    3938:	eb000b37 	bl	661c <ss_aes_data>
    393c:	e1a0100d 	mov	r1, sp
    3940:	e1a02005 	mov	r2, r5
    3944:	e59f0160 	ldr	r0, [pc, #352]	; 3aac <do_crypto_quick+0x234>
    3948:	ebffffad 	bl	3804 <__show_strbuf>
    394c:	e1a01007 	mov	r1, r7
    3950:	e1a0200a 	mov	r2, sl
    3954:	e3a00000 	mov	r0, #0	; 0x0
    3958:	eb000b40 	bl	6660 <ss_aes_init>
    395c:	e59f714c 	ldr	r7, [pc, #332]	; 3ab0 <do_crypto_quick+0x238>
    3960:	e1a0100d 	mov	r1, sp
    3964:	e1a02005 	mov	r2, r5
    3968:	e1a00008 	mov	r0, r8
    396c:	eb000b2a 	bl	661c <ss_aes_data>
    3970:	e1a0100d 	mov	r1, sp
    3974:	e1a02005 	mov	r2, r5
    3978:	e59f0134 	ldr	r0, [pc, #308]	; 3ab4 <do_crypto_quick+0x23c>
    397c:	ebffffa0 	bl	3804 <__show_strbuf>
    3980:	e1a02007 	mov	r2, r7
    3984:	e3a01000 	mov	r1, #0	; 0x0
    3988:	e3a00001 	mov	r0, #1	; 0x1
    398c:	eb000b33 	bl	6660 <ss_aes_init>
    3990:	e1a0100d 	mov	r1, sp
    3994:	e1a02005 	mov	r2, r5
    3998:	e1a00006 	mov	r0, r6
    399c:	eb000b1e 	bl	661c <ss_aes_data>
    39a0:	e1a0100d 	mov	r1, sp
    39a4:	e1a02005 	mov	r2, r5
    39a8:	e59f0108 	ldr	r0, [pc, #264]	; 3ab8 <do_crypto_quick+0x240>
    39ac:	ebffff94 	bl	3804 <__show_strbuf>
    39b0:	e3a00000 	mov	r0, #0	; 0x0
    39b4:	e1a02007 	mov	r2, r7
    39b8:	e1a01000 	mov	r1, r0
    39bc:	eb000b27 	bl	6660 <ss_aes_init>
    39c0:	e1a0100d 	mov	r1, sp
    39c4:	e1a02005 	mov	r2, r5
    39c8:	e1a0000d 	mov	r0, sp
    39cc:	eb000b12 	bl	661c <ss_aes_data>
    39d0:	e1a02005 	mov	r2, r5
    39d4:	e1a0100d 	mov	r1, sp
    39d8:	e59f00dc 	ldr	r0, [pc, #220]	; 3abc <do_crypto_quick+0x244>
    39dc:	ebffff88 	bl	3804 <__show_strbuf>
    39e0:	e1a01005 	mov	r1, r5
    39e4:	e3a00001 	mov	r0, #1	; 0x1
    39e8:	eb000c3a 	bl	6ad8 <hash_init>
    39ec:	e1a01005 	mov	r1, r5
    39f0:	e1a00006 	mov	r0, r6
    39f4:	eb000c12 	bl	6a44 <hash_data>
    39f8:	e1a0000d 	mov	r0, sp
    39fc:	eb000bf4 	bl	69d4 <hash_value>
    3a00:	eb000bec 	bl	69b8 <hash_len>
    3a04:	e1a0100d 	mov	r1, sp
    3a08:	e1a02000 	mov	r2, r0
    3a0c:	e59f00ac 	ldr	r0, [pc, #172]	; 3ac0 <do_crypto_quick+0x248>
    3a10:	ebffff7b 	bl	3804 <__show_strbuf>
    3a14:	e1a01005 	mov	r1, r5
    3a18:	e3a00002 	mov	r0, #2	; 0x2
    3a1c:	eb000c2d 	bl	6ad8 <hash_init>
    3a20:	e1a01005 	mov	r1, r5
    3a24:	e1a00006 	mov	r0, r6
    3a28:	eb000c05 	bl	6a44 <hash_data>
    3a2c:	e1a0000d 	mov	r0, sp
    3a30:	eb000be7 	bl	69d4 <hash_value>
    3a34:	eb000bdf 	bl	69b8 <hash_len>
    3a38:	e1a0100d 	mov	r1, sp
    3a3c:	e1a02000 	mov	r2, r0
    3a40:	e59f007c 	ldr	r0, [pc, #124]	; 3ac4 <do_crypto_quick+0x24c>
    3a44:	ebffff6e 	bl	3804 <__show_strbuf>
    3a48:	e1a01005 	mov	r1, r5
    3a4c:	e3a00003 	mov	r0, #3	; 0x3
    3a50:	eb000c20 	bl	6ad8 <hash_init>
    3a54:	e1a01005 	mov	r1, r5
    3a58:	e1a00006 	mov	r0, r6
    3a5c:	eb000bf8 	bl	6a44 <hash_data>
    3a60:	e1a0000d 	mov	r0, sp
    3a64:	eb000bda 	bl	69d4 <hash_value>
    3a68:	eb000bd2 	bl	69b8 <hash_len>
    3a6c:	e1a0100d 	mov	r1, sp
    3a70:	e1a02000 	mov	r2, r0
    3a74:	e59f004c 	ldr	r0, [pc, #76]	; 3ac8 <do_crypto_quick+0x250>
    3a78:	ebffff61 	bl	3804 <__show_strbuf>
    3a7c:	e1a0400d 	mov	r4, sp
    3a80:	e3a00000 	mov	r0, #0	; 0x0
    3a84:	e28dd0f0 	add	sp, sp, #240	; 0xf0
    3a88:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3a8c:	00015554 	.word	0x00015554
    3a90:	00015534 	.word	0x00015534
    3a94:	000154f4 	.word	0x000154f4
    3a98:	000154b4 	.word	0x000154b4
    3a9c:	00016fbf 	.word	0x00016fbf
    3aa0:	00016fc2 	.word	0x00016fc2
    3aa4:	00016fc6 	.word	0x00016fc6
    3aa8:	00016fd1 	.word	0x00016fd1
    3aac:	00016fe2 	.word	0x00016fe2
    3ab0:	3c00d08c 	.word	0x3c00d08c
    3ab4:	00016ff5 	.word	0x00016ff5
    3ab8:	00016fde 	.word	0x00016fde
    3abc:	00016ff1 	.word	0x00016ff1
    3ac0:	00017004 	.word	0x00017004
    3ac4:	00017008 	.word	0x00017008
    3ac8:	0001700d 	.word	0x0001700d

00003acc <tt_crypto_hash>:
    3acc:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    3ad0:	e3a0548e 	mov	r5, #-1912602624	; 0x8e000000
    3ad4:	e5951000 	ldr	r1, [r5]
    3ad8:	e59f2220 	ldr	r2, [pc, #544]	; 3d00 <tt_crypto_hash+0x234>
    3adc:	e24dd038 	sub	sp, sp, #56	; 0x38
    3ae0:	e59f321c 	ldr	r3, [pc, #540]	; 3d04 <tt_crypto_hash+0x238>
    3ae4:	e1510002 	cmp	r1, r2
    3ae8:	e28dc028 	add	ip, sp, #40	; 0x28
    3aec:	e1a06000 	mov	r6, r0
    3af0:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    3af4:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    3af8:	0a000006 	beq	3b18 <tt_crypto_hash+0x4c>
    3afc:	e59f0204 	ldr	r0, [pc, #516]	; 3d08 <tt_crypto_hash+0x23c>
    3b00:	e59f3204 	ldr	r3, [pc, #516]	; 3d0c <tt_crypto_hash+0x240>
    3b04:	e1a0e00f 	mov	lr, pc
    3b08:	e593f000 	ldr	pc, [r3]
    3b0c:	e59f01fc 	ldr	r0, [pc, #508]	; 3d10 <tt_crypto_hash+0x244>
    3b10:	eb0027b1 	bl	d9dc <udelay>
    3b14:	ea000008 	b	3b3c <tt_crypto_hash+0x70>
    3b18:	e5951004 	ldr	r1, [r5, #4]
    3b1c:	e3a04000 	mov	r4, #0	; 0x0
    3b20:	e3510003 	cmp	r1, #3	; 0x3
    3b24:	e5854000 	str	r4, [r5]
    3b28:	e59fc1dc 	ldr	ip, [pc, #476]	; 3d0c <tt_crypto_hash+0x240>
    3b2c:	9a000004 	bls	3b44 <tt_crypto_hash+0x78>
    3b30:	e59f01dc 	ldr	r0, [pc, #476]	; 3d14 <tt_crypto_hash+0x248>
    3b34:	e1a0e00f 	mov	lr, pc
    3b38:	e59cf000 	ldr	pc, [ip]
    3b3c:	e3e0c000 	mvn	ip, #0	; 0x0
    3b40:	ea00006b 	b	3cf4 <tt_crypto_hash+0x228>
    3b44:	e28d2038 	add	r2, sp, #56	; 0x38
    3b48:	e0823101 	add	r3, r2, r1, lsl #2
    3b4c:	e5131010 	ldr	r1, [r3, #-16]
    3b50:	e5952008 	ldr	r2, [r5, #8]
    3b54:	e59f01bc 	ldr	r0, [pc, #444]	; 3d18 <tt_crypto_hash+0x24c>
    3b58:	e1a0e00f 	mov	lr, pc
    3b5c:	e59cf000 	ldr	pc, [ip]
    3b60:	e9950003 	ldmib	r5, {r0, r1}
    3b64:	eb000bdb 	bl	6ad8 <hash_init>
    3b68:	eb000b92 	bl	69b8 <hash_len>
    3b6c:	e1a03000 	mov	r3, r0
    3b70:	e59f01a4 	ldr	r0, [pc, #420]	; 3d1c <tt_crypto_hash+0x250>
    3b74:	e3a0c020 	mov	ip, #32	; 0x20
    3b78:	e1a01000 	mov	r1, r0
    3b7c:	e3a02001 	mov	r2, #1	; 0x1
    3b80:	e58dc000 	str	ip, [sp]
    3b84:	eb003664 	bl	1151c <print_buffer>
    3b88:	e3560000 	cmp	r6, #0	; 0x0
    3b8c:	0a000022 	beq	3c1c <tt_crypto_hash+0x150>
    3b90:	e1a09005 	mov	r9, r5
    3b94:	e1a0a004 	mov	sl, r4
    3b98:	e5955008 	ldr	r5, [r5, #8]
    3b9c:	e2868901 	add	r8, r6, #16384	; 0x4000
    3ba0:	ea000019 	b	3c0c <tt_crypto_hash+0x140>
    3ba4:	e5991008 	ldr	r1, [r9, #8]
    3ba8:	e3550902 	cmp	r5, #32768	; 0x8000
    3bac:	b1a04005 	movlt	r4, r5
    3bb0:	a3a04902 	movge	r4, #32768	; 0x8000
    3bb4:	e0651001 	rsb	r1, r5, r1
    3bb8:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3bbc:	e2811030 	add	r1, r1, #48	; 0x30
    3bc0:	e1a02004 	mov	r2, r4
    3bc4:	ebfff662 	bl	1554 <memcpy>
    3bc8:	e3a00000 	mov	r0, #0	; 0x0
    3bcc:	eb0027a6 	bl	da6c <get_timer>
    3bd0:	e3550901 	cmp	r5, #16384	; 0x4000
    3bd4:	b1a01005 	movlt	r1, r5
    3bd8:	a3a01901 	movge	r1, #16384	; 0x4000
    3bdc:	e1a07000 	mov	r7, r0
    3be0:	e1a00006 	mov	r0, r6
    3be4:	eb000b96 	bl	6a44 <hash_data>
    3be8:	e3540902 	cmp	r4, #32768	; 0x8000
    3bec:	e0645005 	rsb	r5, r4, r5
    3bf0:	1a000002 	bne	3c00 <tt_crypto_hash+0x134>
    3bf4:	e3a01901 	mov	r1, #16384	; 0x4000
    3bf8:	e1a00008 	mov	r0, r8
    3bfc:	eb000b90 	bl	6a44 <hash_data>
    3c00:	e1a00007 	mov	r0, r7
    3c04:	eb002798 	bl	da6c <get_timer>
    3c08:	e08aa000 	add	sl, sl, r0
    3c0c:	e3550000 	cmp	r5, #0	; 0x0
    3c10:	e1a00006 	mov	r0, r6
    3c14:	1affffe2 	bne	3ba4 <tt_crypto_hash+0xd8>
    3c18:	ea000008 	b	3c40 <tt_crypto_hash+0x174>
    3c1c:	e1a00006 	mov	r0, r6
    3c20:	eb002791 	bl	da6c <get_timer>
    3c24:	e5951008 	ldr	r1, [r5, #8]
    3c28:	e1a04000 	mov	r4, r0
    3c2c:	e59f00ec 	ldr	r0, [pc, #236]	; 3d20 <tt_crypto_hash+0x254>
    3c30:	eb000b83 	bl	6a44 <hash_data>
    3c34:	e1a00004 	mov	r0, r4
    3c38:	eb00278b 	bl	da6c <get_timer>
    3c3c:	e1a0a000 	mov	sl, r0
    3c40:	e28d4008 	add	r4, sp, #8	; 0x8
    3c44:	e3a0648e 	mov	r6, #-1912602624	; 0x8e000000
    3c48:	e1a00004 	mov	r0, r4
    3c4c:	eb000b60 	bl	69d4 <hash_value>
    3c50:	e5965008 	ldr	r5, [r6, #8]
    3c54:	eb000b57 	bl	69b8 <hash_len>
    3c58:	e3a0c020 	mov	ip, #32	; 0x20
    3c5c:	e1a03000 	mov	r3, r0
    3c60:	e1a01004 	mov	r1, r4
    3c64:	e3a02001 	mov	r2, #1	; 0x1
    3c68:	e1a00004 	mov	r0, r4
    3c6c:	e58dc000 	str	ip, [sp]
    3c70:	eb003629 	bl	1151c <print_buffer>
    3c74:	e5963004 	ldr	r3, [r6, #4]
    3c78:	e28d1038 	add	r1, sp, #56	; 0x38
    3c7c:	e0812103 	add	r2, r1, r3, lsl #2
    3c80:	e59f3084 	ldr	r3, [pc, #132]	; 3d0c <tt_crypto_hash+0x240>
    3c84:	e5127010 	ldr	r7, [r2, #-16]
    3c88:	e5938000 	ldr	r8, [r3]
    3c8c:	eb000b49 	bl	69b8 <hash_len>
    3c90:	e1a01004 	mov	r1, r4
    3c94:	e1a02000 	mov	r2, r0
    3c98:	e59f007c 	ldr	r0, [pc, #124]	; 3d1c <tt_crypto_hash+0x250>
    3c9c:	ebfff64d 	bl	15d8 <memcmp>
    3ca0:	e1a05525 	lsr	r5, r5, #10
    3ca4:	e3500000 	cmp	r0, #0	; 0x0
    3ca8:	e59f3074 	ldr	r3, [pc, #116]	; 3d24 <tt_crypto_hash+0x258>
    3cac:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    3cb0:	e1a0100a 	mov	r1, sl
    3cb4:	e0000590 	mul	r0, r0, r5
    3cb8:	e59f4068 	ldr	r4, [pc, #104]	; 3d28 <tt_crypto_hash+0x25c>
    3cbc:	01a04003 	moveq	r4, r3
    3cc0:	eb0042b1 	bl	1478c <__aeabi_uidiv>
    3cc4:	e1a01007 	mov	r1, r7
    3cc8:	e1a03000 	mov	r3, r0
    3ccc:	e1a02004 	mov	r2, r4
    3cd0:	e59f0054 	ldr	r0, [pc, #84]	; 3d2c <tt_crypto_hash+0x260>
    3cd4:	e12fff38 	blx	r8
    3cd8:	e59f1050 	ldr	r1, [pc, #80]	; 3d30 <tt_crypto_hash+0x264>
    3cdc:	e5960008 	ldr	r0, [r6, #8]
    3ce0:	e891000c 	ldm	r1, {r2, r3}
    3ce4:	e0922000 	adds	r2, r2, r0
    3ce8:	e2a33000 	adc	r3, r3, #0	; 0x0
    3cec:	e881000c 	stm	r1, {r2, r3}
    3cf0:	e3a0c000 	mov	ip, #0	; 0x0
    3cf4:	e1a0000c 	mov	r0, ip
    3cf8:	e28dd038 	add	sp, sp, #56	; 0x38
    3cfc:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3d00:	99997777 	.word	0x99997777
    3d04:	00015564 	.word	0x00015564
    3d08:	00017014 	.word	0x00017014
    3d0c:	3c00d6cc 	.word	0x3c00d6cc
    3d10:	004c4b40 	.word	0x004c4b40
    3d14:	00017033 	.word	0x00017033
    3d18:	0001704e 	.word	0x0001704e
    3d1c:	8e000010 	.word	0x8e000010
    3d20:	8e000030 	.word	0x8e000030
    3d24:	00017069 	.word	0x00017069
    3d28:	0001706e 	.word	0x0001706e
    3d2c:	00017075 	.word	0x00017075
    3d30:	3c00d148 	.word	0x3c00d148

00003d34 <tt_crypto_aes>:
    3d34:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3d38:	e24dd014 	sub	sp, sp, #20	; 0x14
    3d3c:	e1a06000 	mov	r6, r0
    3d40:	e3a01000 	mov	r1, #0	; 0x0
    3d44:	e28d000c 	add	r0, sp, #12	; 0xc
    3d48:	e3a02502 	mov	r2, #8388608	; 0x800000
    3d4c:	eb0002be 	bl	484c <tt_rnd_get_data>
    3d50:	e250b000 	subs	fp, r0, #0	; 0x0
    3d54:	e59f4340 	ldr	r4, [pc, #832]	; 409c <tt_crypto_aes+0x368>
    3d58:	059f0340 	ldreq	r0, [pc, #832]	; 40a0 <tt_crypto_aes+0x36c>
    3d5c:	0a00005c 	beq	3ed4 <tt_crypto_aes+0x1a0>
    3d60:	e59d200c 	ldr	r2, [sp, #12]
    3d64:	e1a0100b 	mov	r1, fp
    3d68:	e3c2200f 	bic	r2, r2, #15	; 0xf
    3d6c:	e1a03002 	mov	r3, r2
    3d70:	e58d200c 	str	r2, [sp, #12]
    3d74:	e59f0328 	ldr	r0, [pc, #808]	; 40a4 <tt_crypto_aes+0x370>
    3d78:	e1a02006 	mov	r2, r6
    3d7c:	e1a0e00f 	mov	lr, pc
    3d80:	e594f000 	ldr	pc, [r4]
    3d84:	e59f031c 	ldr	r0, [pc, #796]	; 40a8 <tt_crypto_aes+0x374>
    3d88:	e1a0e00f 	mov	lr, pc
    3d8c:	e594f000 	ldr	pc, [r4]
    3d90:	e1a0100b 	mov	r1, fp
    3d94:	e59d200c 	ldr	r2, [sp, #12]
    3d98:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3d9c:	ebfff5ec 	bl	1554 <memcpy>
    3da0:	e59f0304 	ldr	r0, [pc, #772]	; 40ac <tt_crypto_aes+0x378>
    3da4:	e1a0e00f 	mov	lr, pc
    3da8:	e594f000 	ldr	pc, [r4]
    3dac:	e3a01000 	mov	r1, #0	; 0x0
    3db0:	e59f22f8 	ldr	r2, [pc, #760]	; 40b0 <tt_crypto_aes+0x37c>
    3db4:	e3a00001 	mov	r0, #1	; 0x1
    3db8:	eb000a28 	bl	6660 <ss_aes_init>
    3dbc:	e59f02f0 	ldr	r0, [pc, #752]	; 40b4 <tt_crypto_aes+0x380>
    3dc0:	e1a0e00f 	mov	lr, pc
    3dc4:	e594f000 	ldr	pc, [r4]
    3dc8:	e3560000 	cmp	r6, #0	; 0x0
    3dcc:	159d500c 	ldrne	r5, [sp, #12]
    3dd0:	13a0a000 	movne	sl, #0	; 0x0
    3dd4:	12868901 	addne	r8, r6, #16384	; 0x4000
    3dd8:	1a000022 	bne	3e68 <tt_crypto_aes+0x134>
    3ddc:	ea000025 	b	3e78 <tt_crypto_aes+0x144>
    3de0:	e59d100c 	ldr	r1, [sp, #12]
    3de4:	e3550902 	cmp	r5, #32768	; 0x8000
    3de8:	b1a07005 	movlt	r7, r5
    3dec:	a3a07902 	movge	r7, #32768	; 0x8000
    3df0:	e0651001 	rsb	r1, r5, r1
    3df4:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3df8:	e1a02007 	mov	r2, r7
    3dfc:	ebfff5d4 	bl	1554 <memcpy>
    3e00:	e3a00000 	mov	r0, #0	; 0x0
    3e04:	eb002718 	bl	da6c <get_timer>
    3e08:	e3550901 	cmp	r5, #16384	; 0x4000
    3e0c:	b1a02005 	movlt	r2, r5
    3e10:	a3a02901 	movge	r2, #16384	; 0x4000
    3e14:	e1a04000 	mov	r4, r0
    3e18:	e1a01006 	mov	r1, r6
    3e1c:	e1a00006 	mov	r0, r6
    3e20:	eb0009fd 	bl	661c <ss_aes_data>
    3e24:	e3570902 	cmp	r7, #32768	; 0x8000
    3e28:	1a000003 	bne	3e3c <tt_crypto_aes+0x108>
    3e2c:	e1a01008 	mov	r1, r8
    3e30:	e3a02901 	mov	r2, #16384	; 0x4000
    3e34:	e1a00008 	mov	r0, r8
    3e38:	eb0009f7 	bl	661c <ss_aes_data>
    3e3c:	e1a00004 	mov	r0, r4
    3e40:	eb002709 	bl	da6c <get_timer>
    3e44:	e1a04000 	mov	r4, r0
    3e48:	e59d000c 	ldr	r0, [sp, #12]
    3e4c:	e1a01006 	mov	r1, r6
    3e50:	e0650000 	rsb	r0, r5, r0
    3e54:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    3e58:	e1a02007 	mov	r2, r7
    3e5c:	ebfff5bc 	bl	1554 <memcpy>
    3e60:	e08aa004 	add	sl, sl, r4
    3e64:	e0675005 	rsb	r5, r7, r5
    3e68:	e3550000 	cmp	r5, #0	; 0x0
    3e6c:	e1a00006 	mov	r0, r6
    3e70:	1affffda 	bne	3de0 <tt_crypto_aes+0xac>
    3e74:	ea000009 	b	3ea0 <tt_crypto_aes+0x16c>
    3e78:	e1a00006 	mov	r0, r6
    3e7c:	eb0026fa 	bl	da6c <get_timer>
    3e80:	e1a04000 	mov	r4, r0
    3e84:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3e88:	e1a01000 	mov	r1, r0
    3e8c:	e59d200c 	ldr	r2, [sp, #12]
    3e90:	eb0009e1 	bl	661c <ss_aes_data>
    3e94:	e1a00004 	mov	r0, r4
    3e98:	eb0026f3 	bl	da6c <get_timer>
    3e9c:	e1a0a000 	mov	sl, r0
    3ea0:	e59d300c 	ldr	r3, [sp, #12]
    3ea4:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3ea8:	e1a0100b 	mov	r1, fp
    3eac:	e3a02010 	mov	r2, #16	; 0x10
    3eb0:	e58d3004 	str	r3, [sp, #4]
    3eb4:	ebfff5c7 	bl	15d8 <memcmp>
    3eb8:	e3500000 	cmp	r0, #0	; 0x0
    3ebc:	e59f41d8 	ldr	r4, [pc, #472]	; 409c <tt_crypto_aes+0x368>
    3ec0:	1a000007 	bne	3ee4 <tt_crypto_aes+0x1b0>
    3ec4:	e59f01ec 	ldr	r0, [pc, #492]	; 40b8 <tt_crypto_aes+0x384>
    3ec8:	e1a0e00f 	mov	lr, pc
    3ecc:	e594f000 	ldr	pc, [r4]
    3ed0:	e59f01e4 	ldr	r0, [pc, #484]	; 40bc <tt_crypto_aes+0x388>
    3ed4:	e1a0e00f 	mov	lr, pc
    3ed8:	e594f000 	ldr	pc, [r4]
    3edc:	e3e00000 	mvn	r0, #0	; 0x0
    3ee0:	ea00006b 	b	4094 <tt_crypto_aes+0x360>
    3ee4:	e3a00000 	mov	r0, #0	; 0x0
    3ee8:	e1a01000 	mov	r1, r0
    3eec:	e59f21bc 	ldr	r2, [pc, #444]	; 40b0 <tt_crypto_aes+0x37c>
    3ef0:	eb0009da 	bl	6660 <ss_aes_init>
    3ef4:	e59f01c4 	ldr	r0, [pc, #452]	; 40c0 <tt_crypto_aes+0x38c>
    3ef8:	e1a0e00f 	mov	lr, pc
    3efc:	e594f000 	ldr	pc, [r4]
    3f00:	e3560000 	cmp	r6, #0	; 0x0
    3f04:	159d500c 	ldrne	r5, [sp, #12]
    3f08:	13a08000 	movne	r8, #0	; 0x0
    3f0c:	12869901 	addne	r9, r6, #16384	; 0x4000
    3f10:	1a000022 	bne	3fa0 <tt_crypto_aes+0x26c>
    3f14:	ea000025 	b	3fb0 <tt_crypto_aes+0x27c>
    3f18:	eb0026d3 	bl	da6c <get_timer>
    3f1c:	e59d100c 	ldr	r1, [sp, #12]
    3f20:	e3550902 	cmp	r5, #32768	; 0x8000
    3f24:	b1a07005 	movlt	r7, r5
    3f28:	a3a07902 	movge	r7, #32768	; 0x8000
    3f2c:	e0651001 	rsb	r1, r5, r1
    3f30:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3f34:	e1a02007 	mov	r2, r7
    3f38:	e1a04000 	mov	r4, r0
    3f3c:	e1a00006 	mov	r0, r6
    3f40:	ebfff583 	bl	1554 <memcpy>
    3f44:	e3550901 	cmp	r5, #16384	; 0x4000
    3f48:	b1a02005 	movlt	r2, r5
    3f4c:	a3a02901 	movge	r2, #16384	; 0x4000
    3f50:	e1a00006 	mov	r0, r6
    3f54:	e1a01006 	mov	r1, r6
    3f58:	eb0009af 	bl	661c <ss_aes_data>
    3f5c:	e3570902 	cmp	r7, #32768	; 0x8000
    3f60:	1a000003 	bne	3f74 <tt_crypto_aes+0x240>
    3f64:	e1a01009 	mov	r1, r9
    3f68:	e3a02901 	mov	r2, #16384	; 0x4000
    3f6c:	e1a00009 	mov	r0, r9
    3f70:	eb0009a9 	bl	661c <ss_aes_data>
    3f74:	e1a00004 	mov	r0, r4
    3f78:	eb0026bb 	bl	da6c <get_timer>
    3f7c:	e1a04000 	mov	r4, r0
    3f80:	e59d000c 	ldr	r0, [sp, #12]
    3f84:	e1a01006 	mov	r1, r6
    3f88:	e0650000 	rsb	r0, r5, r0
    3f8c:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    3f90:	e1a02007 	mov	r2, r7
    3f94:	ebfff56e 	bl	1554 <memcpy>
    3f98:	e0888004 	add	r8, r8, r4
    3f9c:	e0675005 	rsb	r5, r7, r5
    3fa0:	e3a00000 	mov	r0, #0	; 0x0
    3fa4:	e1550000 	cmp	r5, r0
    3fa8:	1affffda 	bne	3f18 <tt_crypto_aes+0x1e4>
    3fac:	ea000009 	b	3fd8 <tt_crypto_aes+0x2a4>
    3fb0:	e1a00006 	mov	r0, r6
    3fb4:	eb0026ac 	bl	da6c <get_timer>
    3fb8:	e1a04000 	mov	r4, r0
    3fbc:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3fc0:	e1a01000 	mov	r1, r0
    3fc4:	e59d200c 	ldr	r2, [sp, #12]
    3fc8:	eb000993 	bl	661c <ss_aes_data>
    3fcc:	e1a00004 	mov	r0, r4
    3fd0:	eb0026a5 	bl	da6c <get_timer>
    3fd4:	e1a08000 	mov	r8, r0
    3fd8:	e59d3004 	ldr	r3, [sp, #4]
    3fdc:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    3fe0:	e1a00543 	asr	r0, r3, #10
    3fe4:	e1a0100a 	mov	r1, sl
    3fe8:	e0000094 	mul	r0, r4, r0
    3fec:	eb004268 	bl	14994 <__aeabi_idiv>
    3ff0:	e1a06000 	mov	r6, r0
    3ff4:	e59d000c 	ldr	r0, [sp, #12]
    3ff8:	e1a01008 	mov	r1, r8
    3ffc:	e1a00540 	asr	r0, r0, #10
    4000:	e0000094 	mul	r0, r4, r0
    4004:	eb004262 	bl	14994 <__aeabi_idiv>
    4008:	e59f408c 	ldr	r4, [pc, #140]	; 409c <tt_crypto_aes+0x368>
    400c:	e1a05000 	mov	r5, r0
    4010:	e59f00ac 	ldr	r0, [pc, #172]	; 40c4 <tt_crypto_aes+0x390>
    4014:	e1a0e00f 	mov	lr, pc
    4018:	e594f000 	ldr	pc, [r4]
    401c:	e1a0000b 	mov	r0, fp
    4020:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    4024:	e59d200c 	ldr	r2, [sp, #12]
    4028:	e3a0300c 	mov	r3, #12	; 0xc
    402c:	eb000189 	bl	4658 <tt_cmp_buffer>
    4030:	e3500000 	cmp	r0, #0	; 0x0
    4034:	0a000006 	beq	4054 <tt_crypto_aes+0x320>
    4038:	e1a03000 	mov	r3, r0
    403c:	e1a01006 	mov	r1, r6
    4040:	e1a02005 	mov	r2, r5
    4044:	e59f007c 	ldr	r0, [pc, #124]	; 40c8 <tt_crypto_aes+0x394>
    4048:	e1a0e00f 	mov	lr, pc
    404c:	e594f000 	ldr	pc, [r4]
    4050:	ea000004 	b	4068 <tt_crypto_aes+0x334>
    4054:	e1a01006 	mov	r1, r6
    4058:	e1a02005 	mov	r2, r5
    405c:	e59f0068 	ldr	r0, [pc, #104]	; 40cc <tt_crypto_aes+0x398>
    4060:	e1a0e00f 	mov	lr, pc
    4064:	e594f000 	ldr	pc, [r4]
    4068:	e59f0060 	ldr	r0, [pc, #96]	; 40d0 <tt_crypto_aes+0x39c>
    406c:	e59d100c 	ldr	r1, [sp, #12]
    4070:	e890000c 	ldm	r0, {r2, r3}
    4074:	e0922001 	adds	r2, r2, r1
    4078:	e0a33fc1 	adc	r3, r3, r1, asr #31
    407c:	e880000c 	stm	r0, {r2, r3}
    4080:	e59f0030 	ldr	r0, [pc, #48]	; 40b8 <tt_crypto_aes+0x384>
    4084:	e59f3010 	ldr	r3, [pc, #16]	; 409c <tt_crypto_aes+0x368>
    4088:	e1a0e00f 	mov	lr, pc
    408c:	e593f000 	ldr	pc, [r3]
    4090:	e3a00000 	mov	r0, #0	; 0x0
    4094:	e28dd014 	add	sp, sp, #20	; 0x14
    4098:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    409c:	3c00d6cc 	.word	0x3c00d6cc
    40a0:	00017095 	.word	0x00017095
    40a4:	000170b8 	.word	0x000170b8
    40a8:	00016afa 	.word	0x00016afa
    40ac:	00017932 	.word	0x00017932
    40b0:	3c00d08c 	.word	0x3c00d08c
    40b4:	00018a64 	.word	0x00018a64
    40b8:	00016bef 	.word	0x00016bef
    40bc:	000170e4 	.word	0x000170e4
    40c0:	00017fa8 	.word	0x00017fa8
    40c4:	00016e40 	.word	0x00016e40
    40c8:	00017105 	.word	0x00017105
    40cc:	00017143 	.word	0x00017143
    40d0:	3c00d140 	.word	0x3c00d140

000040d4 <do_tt_crypto>:
    40d4:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    40d8:	e3520001 	cmp	r2, #1	; 0x1
    40dc:	e24dd004 	sub	sp, sp, #4	; 0x4
    40e0:	d3a07000 	movle	r7, #0	; 0x0
    40e4:	da000009 	ble	4110 <do_tt_crypto+0x3c>
    40e8:	e5930004 	ldr	r0, [r3, #4]
    40ec:	e3a01000 	mov	r1, #0	; 0x0
    40f0:	e3a02010 	mov	r2, #16	; 0x10
    40f4:	eb0035a0 	bl	1177c <simple_strtoul>
    40f8:	e1a07000 	mov	r7, r0
    40fc:	e1a01007 	mov	r1, r7
    4100:	e59f0114 	ldr	r0, [pc, #276]	; 421c <do_tt_crypto+0x148>
    4104:	e59f3114 	ldr	r3, [pc, #276]	; 4220 <do_tt_crypto+0x14c>
    4108:	e1a0e00f 	mov	lr, pc
    410c:	e593f000 	ldr	pc, [r3]
    4110:	e3a00000 	mov	r0, #0	; 0x0
    4114:	eb002654 	bl	da6c <get_timer>
    4118:	e59f1104 	ldr	r1, [pc, #260]	; 4224 <do_tt_crypto+0x150>
    411c:	e3a02000 	mov	r2, #0	; 0x0
    4120:	e3a03000 	mov	r3, #0	; 0x0
    4124:	e881000c 	stm	r1, {r2, r3}
    4128:	e59f10f8 	ldr	r1, [pc, #248]	; 4228 <do_tt_crypto+0x154>
    412c:	e59fa0ec 	ldr	sl, [pc, #236]	; 4220 <do_tt_crypto+0x14c>
    4130:	e881000c 	stm	r1, {r2, r3}
    4134:	e1a08000 	mov	r8, r0
    4138:	e3a06000 	mov	r6, #0	; 0x0
    413c:	e3a01006 	mov	r1, #6	; 0x6
    4140:	e3a00000 	mov	r0, #0	; 0x0
    4144:	eb000197 	bl	47a8 <tt_rnd_get_int>
    4148:	e3a01002 	mov	r1, #2	; 0x2
    414c:	e1a04000 	mov	r4, r0
    4150:	e3a00000 	mov	r0, #0	; 0x0
    4154:	eb000193 	bl	47a8 <tt_rnd_get_int>
    4158:	e1a01006 	mov	r1, r6
    415c:	e1a03000 	mov	r3, r0
    4160:	e1a05000 	mov	r5, r0
    4164:	e1a02004 	mov	r2, r4
    4168:	e59f00bc 	ldr	r0, [pc, #188]	; 422c <do_tt_crypto+0x158>
    416c:	e1a0e00f 	mov	lr, pc
    4170:	e59af000 	ldr	pc, [sl]
    4174:	e3540003 	cmp	r4, #3	; 0x3
    4178:	9a000004 	bls	4190 <do_tt_crypto+0xbc>
    417c:	e3550000 	cmp	r5, #0	; 0x0
    4180:	11a00007 	movne	r0, r7
    4184:	03a00000 	moveq	r0, #0	; 0x0
    4188:	ebfffee9 	bl	3d34 <tt_crypto_aes>
    418c:	ea000003 	b	41a0 <do_tt_crypto+0xcc>
    4190:	e3550000 	cmp	r5, #0	; 0x0
    4194:	11a00007 	movne	r0, r7
    4198:	03a00000 	moveq	r0, #0	; 0x0
    419c:	ebfffe4a 	bl	3acc <tt_crypto_hash>
    41a0:	eb00398c 	bl	127d8 <tstc>
    41a4:	e3500000 	cmp	r0, #0	; 0x0
    41a8:	e2866001 	add	r6, r6, #1	; 0x1
    41ac:	0affffe2 	beq	413c <do_tt_crypto+0x68>
    41b0:	eb003989 	bl	127dc <getc>
    41b4:	e3500003 	cmp	r0, #3	; 0x3
    41b8:	1affffdf 	bne	413c <do_tt_crypto+0x68>
    41bc:	e1a00008 	mov	r0, r8
    41c0:	eb002629 	bl	da6c <get_timer>
    41c4:	e59f305c 	ldr	r3, [pc, #92]	; 4228 <do_tt_crypto+0x154>
    41c8:	e59f2054 	ldr	r2, [pc, #84]	; 4224 <do_tt_crypto+0x150>
    41cc:	e5934000 	ldr	r4, [r3]
    41d0:	e5925000 	ldr	r5, [r2]
    41d4:	e5933004 	ldr	r3, [r3, #4]
    41d8:	e5922004 	ldr	r2, [r2, #4]
    41dc:	e30e1a60 	movw	r1, #60000	; 0xea60
    41e0:	e1a04a24 	lsr	r4, r4, #20
    41e4:	e1a05a25 	lsr	r5, r5, #20
    41e8:	e1844603 	orr	r4, r4, r3, lsl #12
    41ec:	e1855602 	orr	r5, r5, r2, lsl #12
    41f0:	eb004165 	bl	1478c <__aeabi_uidiv>
    41f4:	e1a01004 	mov	r1, r4
    41f8:	e1a03000 	mov	r3, r0
    41fc:	e1a02005 	mov	r2, r5
    4200:	e59f0028 	ldr	r0, [pc, #40]	; 4230 <do_tt_crypto+0x15c>
    4204:	e59fc014 	ldr	ip, [pc, #20]	; 4220 <do_tt_crypto+0x14c>
    4208:	e1a0e00f 	mov	lr, pc
    420c:	e59cf000 	ldr	pc, [ip]
    4210:	e3a00000 	mov	r0, #0	; 0x0
    4214:	e28dd004 	add	sp, sp, #4	; 0x4
    4218:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    421c:	00017174 	.word	0x00017174
    4220:	3c00d6cc 	.word	0x3c00d6cc
    4224:	3c00d148 	.word	0x3c00d148
    4228:	3c00d140 	.word	0x3c00d140
    422c:	0001719a 	.word	0x0001719a
    4230:	000171b9 	.word	0x000171b9

00004234 <tt_ff_trolling>:
    4234:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4238:	e3a01001 	mov	r1, #1	; 0x1
    423c:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    4240:	e58d200c 	str	r2, [sp, #12]
    4244:	e58d3008 	str	r3, [sp, #8]
    4248:	e1a05000 	mov	r5, r0
    424c:	eb0008c0 	bl	6554 <vs_assign_by_name>
    4250:	e2506000 	subs	r6, r0, #0	; 0x0
    4254:	0a000005 	beq	4270 <tt_ff_trolling+0x3c>
    4258:	e1a01005 	mov	r1, r5
    425c:	e59f0330 	ldr	r0, [pc, #816]	; 4594 <tt_ff_trolling+0x360>
    4260:	e59f3330 	ldr	r3, [pc, #816]	; 4598 <tt_ff_trolling+0x364>
    4264:	e1a0e00f 	mov	lr, pc
    4268:	e593f000 	ldr	pc, [r3]
    426c:	ea00000c 	b	42a4 <tt_ff_trolling+0x70>
    4270:	e3a0000e 	mov	r0, #14	; 0xe
    4274:	eb000744 	bl	5f8c <vs_align>
    4278:	e1a01000 	mov	r1, r0
    427c:	e28d0018 	add	r0, sp, #24	; 0x18
    4280:	e5201004 	str	r1, [r0, #-4]!
    4284:	e1a02001 	mov	r2, r1
    4288:	eb00016f 	bl	484c <tt_rnd_get_data>
    428c:	e250a000 	subs	sl, r0, #0	; 0x0
    4290:	e59f4300 	ldr	r4, [pc, #768]	; 4598 <tt_ff_trolling+0x364>
    4294:	1a000004 	bne	42ac <tt_ff_trolling+0x78>
    4298:	e59f02fc 	ldr	r0, [pc, #764]	; 459c <tt_ff_trolling+0x368>
    429c:	e1a0e00f 	mov	lr, pc
    42a0:	e594f000 	ldr	pc, [r4]
    42a4:	e3e00000 	mvn	r0, #0	; 0x0
    42a8:	ea0000a9 	b	4554 <tt_ff_trolling+0x320>
    42ac:	e59d2014 	ldr	r2, [sp, #20]
    42b0:	e1a0100a 	mov	r1, sl
    42b4:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    42b8:	ebfff4a5 	bl	1554 <memcpy>
    42bc:	e3a01501 	mov	r1, #4194304	; 0x400000
    42c0:	e59f02d8 	ldr	r0, [pc, #728]	; 45a0 <tt_ff_trolling+0x36c>
    42c4:	e1a0e00f 	mov	lr, pc
    42c8:	e594f000 	ldr	pc, [r4]
    42cc:	e1a00005 	mov	r0, r5
    42d0:	eb0007ae 	bl	6190 <vs_device_id>
    42d4:	e1a0b004 	mov	fp, r4
    42d8:	e3a05000 	mov	r5, #0	; 0x0
    42dc:	e3a04000 	mov	r4, #0	; 0x0
    42e0:	e3a08501 	mov	r8, #4194304	; 0x400000
    42e4:	e3a09000 	mov	r9, #0	; 0x0
    42e8:	e1a07000 	mov	r7, r0
    42ec:	ea00003d 	b	43e8 <tt_ff_trolling+0x1b4>
    42f0:	e1a00007 	mov	r0, r7
    42f4:	eb000791 	bl	6140 <vs_device_erasable>
    42f8:	e3500000 	cmp	r0, #0	; 0x0
    42fc:	0a000003 	beq	4310 <tt_ff_trolling+0xdc>
    4300:	e1a00004 	mov	r0, r4
    4304:	e1a01005 	mov	r1, r5
    4308:	e3a02501 	mov	r2, #4194304	; 0x400000
    430c:	eb0006e2 	bl	5e9c <vs_erase>
    4310:	e1a00007 	mov	r0, r7
    4314:	e59f1288 	ldr	r1, [pc, #648]	; 45a4 <tt_ff_trolling+0x370>
    4318:	eb0007b0 	bl	61e0 <vs_is_device>
    431c:	e3500000 	cmp	r0, #0	; 0x0
    4320:	1a000009 	bne	434c <tt_ff_trolling+0x118>
    4324:	e1a00007 	mov	r0, r7
    4328:	e59f1278 	ldr	r1, [pc, #632]	; 45a8 <tt_ff_trolling+0x374>
    432c:	eb0007ab 	bl	61e0 <vs_is_device>
    4330:	e3500000 	cmp	r0, #0	; 0x0
    4334:	1a000004 	bne	434c <tt_ff_trolling+0x118>
    4338:	e1a00007 	mov	r0, r7
    433c:	e59f1268 	ldr	r1, [pc, #616]	; 45ac <tt_ff_trolling+0x378>
    4340:	eb0007a6 	bl	61e0 <vs_is_device>
    4344:	e3500000 	cmp	r0, #0	; 0x0
    4348:	0a00000a 	beq	4378 <tt_ff_trolling+0x144>
    434c:	e1a00004 	mov	r0, r4
    4350:	e1a01005 	mov	r1, r5
    4354:	eb001a54 	bl	acac <nnd_vs_isbad>
    4358:	e3500000 	cmp	r0, #0	; 0x0
    435c:	0a000005 	beq	4378 <tt_ff_trolling+0x144>
    4360:	e59f0248 	ldr	r0, [pc, #584]	; 45b0 <tt_ff_trolling+0x37c>
    4364:	e1a02004 	mov	r2, r4
    4368:	e1a03005 	mov	r3, r5
    436c:	e1a0e00f 	mov	lr, pc
    4370:	e59bf000 	ldr	pc, [fp]
    4374:	ea000018 	b	43dc <tt_ff_trolling+0x1a8>
    4378:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    437c:	e5816000 	str	r6, [r1]
    4380:	e1a00001 	mov	r0, r1
    4384:	e59d1014 	ldr	r1, [sp, #20]
    4388:	e1a02004 	mov	r2, r4
    438c:	e58d1000 	str	r1, [sp]
    4390:	e1a03005 	mov	r3, r5
    4394:	e3a01000 	mov	r1, #0	; 0x0
    4398:	e58d1004 	str	r1, [sp, #4]
    439c:	eb0006a8 	bl	5e44 <vs_write>
    43a0:	e3500000 	cmp	r0, #0	; 0x0
    43a4:	aa000006 	bge	43c4 <tt_ff_trolling+0x190>
    43a8:	e1a02004 	mov	r2, r4
    43ac:	e1a03005 	mov	r3, r5
    43b0:	e59f01fc 	ldr	r0, [pc, #508]	; 45b4 <tt_ff_trolling+0x380>
    43b4:	e59f11dc 	ldr	r1, [pc, #476]	; 4598 <tt_ff_trolling+0x364>
    43b8:	e1a0e00f 	mov	lr, pc
    43bc:	e591f000 	ldr	pc, [r1]
    43c0:	ea000010 	b	4408 <tt_ff_trolling+0x1d4>
    43c4:	e316001f 	tst	r6, #31	; 0x1f
    43c8:	1a000003 	bne	43dc <tt_ff_trolling+0x1a8>
    43cc:	e59f01e4 	ldr	r0, [pc, #484]	; 45b8 <tt_ff_trolling+0x384>
    43d0:	e1a01106 	lsl	r1, r6, #2
    43d4:	e1a0e00f 	mov	lr, pc
    43d8:	e59bf000 	ldr	pc, [fp]
    43dc:	e0944008 	adds	r4, r4, r8
    43e0:	e0a55009 	adc	r5, r5, r9
    43e4:	e2866001 	add	r6, r6, #1	; 0x1
    43e8:	e59d3008 	ldr	r3, [sp, #8]
    43ec:	e1530005 	cmp	r3, r5
    43f0:	8affffbe 	bhi	42f0 <tt_ff_trolling+0xbc>
    43f4:	1a000058 	bne	455c <tt_ff_trolling+0x328>
    43f8:	e59d100c 	ldr	r1, [sp, #12]
    43fc:	e1510004 	cmp	r1, r4
    4400:	8affffba 	bhi	42f0 <tt_ff_trolling+0xbc>
    4404:	ea000054 	b	455c <tt_ff_trolling+0x328>
    4408:	e59f4188 	ldr	r4, [pc, #392]	; 4598 <tt_ff_trolling+0x364>
    440c:	e59f01a8 	ldr	r0, [pc, #424]	; 45bc <tt_ff_trolling+0x388>
    4410:	e3a01501 	mov	r1, #4194304	; 0x400000
    4414:	e1a0e00f 	mov	lr, pc
    4418:	e594f000 	ldr	pc, [r4]
    441c:	e1a0b004 	mov	fp, r4
    4420:	e3a06000 	mov	r6, #0	; 0x0
    4424:	e28aa004 	add	sl, sl, #4	; 0x4
    4428:	e3a04000 	mov	r4, #0	; 0x0
    442c:	e3a05000 	mov	r5, #0	; 0x0
    4430:	e3a08501 	mov	r8, #4194304	; 0x400000
    4434:	e3a09000 	mov	r9, #0	; 0x0
    4438:	ea00003d 	b	4534 <tt_ff_trolling+0x300>
    443c:	e1a00007 	mov	r0, r7
    4440:	e59f115c 	ldr	r1, [pc, #348]	; 45a4 <tt_ff_trolling+0x370>
    4444:	eb000765 	bl	61e0 <vs_is_device>
    4448:	e3500000 	cmp	r0, #0	; 0x0
    444c:	1a000009 	bne	4478 <tt_ff_trolling+0x244>
    4450:	e1a00007 	mov	r0, r7
    4454:	e59f114c 	ldr	r1, [pc, #332]	; 45a8 <tt_ff_trolling+0x374>
    4458:	eb000760 	bl	61e0 <vs_is_device>
    445c:	e3500000 	cmp	r0, #0	; 0x0
    4460:	1a000004 	bne	4478 <tt_ff_trolling+0x244>
    4464:	e1a00007 	mov	r0, r7
    4468:	e59f113c 	ldr	r1, [pc, #316]	; 45ac <tt_ff_trolling+0x378>
    446c:	eb00075b 	bl	61e0 <vs_is_device>
    4470:	e3500000 	cmp	r0, #0	; 0x0
    4474:	0a00000a 	beq	44a4 <tt_ff_trolling+0x270>
    4478:	e1a00004 	mov	r0, r4
    447c:	e1a01005 	mov	r1, r5
    4480:	eb001a09 	bl	acac <nnd_vs_isbad>
    4484:	e3500000 	cmp	r0, #0	; 0x0
    4488:	0a000005 	beq	44a4 <tt_ff_trolling+0x270>
    448c:	e59f011c 	ldr	r0, [pc, #284]	; 45b0 <tt_ff_trolling+0x37c>
    4490:	e1a02004 	mov	r2, r4
    4494:	e1a03005 	mov	r3, r5
    4498:	e1a0e00f 	mov	lr, pc
    449c:	e59bf000 	ldr	pc, [fp]
    44a0:	ea000020 	b	4528 <tt_ff_trolling+0x2f4>
    44a4:	e59d1014 	ldr	r1, [sp, #20]
    44a8:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    44ac:	e58d1000 	str	r1, [sp]
    44b0:	e1a02004 	mov	r2, r4
    44b4:	e3a01000 	mov	r1, #0	; 0x0
    44b8:	e1a03005 	mov	r3, r5
    44bc:	e58d1004 	str	r1, [sp, #4]
    44c0:	eb00075f 	bl	6244 <vs_read>
    44c4:	e316001f 	tst	r6, #31	; 0x1f
    44c8:	1a000003 	bne	44dc <tt_ff_trolling+0x2a8>
    44cc:	e59f00e4 	ldr	r0, [pc, #228]	; 45b8 <tt_ff_trolling+0x384>
    44d0:	e1a01106 	lsl	r1, r6, #2
    44d4:	e1a0e00f 	mov	lr, pc
    44d8:	e59bf000 	ldr	pc, [fp]
    44dc:	e3a0348e 	mov	r3, #-1912602624	; 0x8e000000
    44e0:	e5933000 	ldr	r3, [r3]
    44e4:	e1530006 	cmp	r3, r6
    44e8:	1a000007 	bne	450c <tt_ff_trolling+0x2d8>
    44ec:	e59d2014 	ldr	r2, [sp, #20]
    44f0:	e1a0000a 	mov	r0, sl
    44f4:	e2422004 	sub	r2, r2, #4	; 0x4
    44f8:	e59f10c0 	ldr	r1, [pc, #192]	; 45c0 <tt_ff_trolling+0x38c>
    44fc:	e3a0300a 	mov	r3, #10	; 0xa
    4500:	eb000054 	bl	4658 <tt_cmp_buffer>
    4504:	e3500000 	cmp	r0, #0	; 0x0
    4508:	0a000006 	beq	4528 <tt_ff_trolling+0x2f4>
    450c:	e1a02004 	mov	r2, r4
    4510:	e1a03005 	mov	r3, r5
    4514:	e59f00a8 	ldr	r0, [pc, #168]	; 45c4 <tt_ff_trolling+0x390>
    4518:	e59f1078 	ldr	r1, [pc, #120]	; 4598 <tt_ff_trolling+0x364>
    451c:	e1a0e00f 	mov	lr, pc
    4520:	e591f000 	ldr	pc, [r1]
    4524:	ea000018 	b	458c <tt_ff_trolling+0x358>
    4528:	e0944008 	adds	r4, r4, r8
    452c:	e0a55009 	adc	r5, r5, r9
    4530:	e2866001 	add	r6, r6, #1	; 0x1
    4534:	e59d3008 	ldr	r3, [sp, #8]
    4538:	e1530005 	cmp	r3, r5
    453c:	8affffbe 	bhi	443c <tt_ff_trolling+0x208>
    4540:	1a00000a 	bne	4570 <tt_ff_trolling+0x33c>
    4544:	e59d100c 	ldr	r1, [sp, #12]
    4548:	e1510004 	cmp	r1, r4
    454c:	8affffba 	bhi	443c <tt_ff_trolling+0x208>
    4550:	ea000006 	b	4570 <tt_ff_trolling+0x33c>
    4554:	e28dd01c 	add	sp, sp, #28	; 0x1c
    4558:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    455c:	e59f0064 	ldr	r0, [pc, #100]	; 45c8 <tt_ff_trolling+0x394>
    4560:	e59f3030 	ldr	r3, [pc, #48]	; 4598 <tt_ff_trolling+0x364>
    4564:	e1a0e00f 	mov	lr, pc
    4568:	e593f000 	ldr	pc, [r3]
    456c:	eaffffa5 	b	4408 <tt_ff_trolling+0x1d4>
    4570:	e59f0050 	ldr	r0, [pc, #80]	; 45c8 <tt_ff_trolling+0x394>
    4574:	e59f401c 	ldr	r4, [pc, #28]	; 4598 <tt_ff_trolling+0x364>
    4578:	e1a0e00f 	mov	lr, pc
    457c:	e594f000 	ldr	pc, [r4]
    4580:	e59f0044 	ldr	r0, [pc, #68]	; 45cc <tt_ff_trolling+0x398>
    4584:	e1a0e00f 	mov	lr, pc
    4588:	e594f000 	ldr	pc, [r4]
    458c:	e3a00000 	mov	r0, #0	; 0x0
    4590:	eaffffef 	b	4554 <tt_ff_trolling+0x320>
    4594:	0001723d 	.word	0x0001723d
    4598:	3c00d6cc 	.word	0x3c00d6cc
    459c:	0001725a 	.word	0x0001725a
    45a0:	00017279 	.word	0x00017279
    45a4:	00016127 	.word	0x00016127
    45a8:	00016283 	.word	0x00016283
    45ac:	0001627f 	.word	0x0001627f
    45b0:	00017297 	.word	0x00017297
    45b4:	000172b6 	.word	0x000172b6
    45b8:	000172ce 	.word	0x000172ce
    45bc:	000172d5 	.word	0x000172d5
    45c0:	8e000004 	.word	0x8e000004
    45c4:	000172f5 	.word	0x000172f5
    45c8:	00017305 	.word	0x00017305
    45cc:	0001730c 	.word	0x0001730c

000045d0 <do_tt_ff>:
    45d0:	e92d4030 	push	{r4, r5, lr}
    45d4:	e3520001 	cmp	r2, #1	; 0x1
    45d8:	e24dd004 	sub	sp, sp, #4	; 0x4
    45dc:	e1a05003 	mov	r5, r3
    45e0:	ca000005 	bgt	45fc <do_tt_ff+0x2c>
    45e4:	e59f0060 	ldr	r0, [pc, #96]	; 464c <do_tt_ff+0x7c>
    45e8:	e59f3060 	ldr	r3, [pc, #96]	; 4650 <do_tt_ff+0x80>
    45ec:	e1a0e00f 	mov	lr, pc
    45f0:	e593f000 	ldr	pc, [r3]
    45f4:	e3e00000 	mvn	r0, #0	; 0x0
    45f8:	ea000011 	b	4644 <do_tt_ff+0x74>
    45fc:	e3520002 	cmp	r2, #2	; 0x2
    4600:	03a04000 	moveq	r4, #0	; 0x0
    4604:	0a000009 	beq	4630 <do_tt_ff+0x60>
    4608:	e3a01000 	mov	r1, #0	; 0x0
    460c:	e5930008 	ldr	r0, [r3, #8]
    4610:	e3a0200a 	mov	r2, #10	; 0xa
    4614:	eb003458 	bl	1177c <simple_strtoul>
    4618:	e1a04000 	mov	r4, r0
    461c:	e1a01004 	mov	r1, r4
    4620:	e59f002c 	ldr	r0, [pc, #44]	; 4654 <do_tt_ff+0x84>
    4624:	e59f3024 	ldr	r3, [pc, #36]	; 4650 <do_tt_ff+0x80>
    4628:	e1a0e00f 	mov	lr, pc
    462c:	e593f000 	ldr	pc, [r3]
    4630:	e5950004 	ldr	r0, [r5, #4]
    4634:	e1a03624 	lsr	r3, r4, #12
    4638:	e1a02a04 	lsl	r2, r4, #20
    463c:	ebfffefc 	bl	4234 <tt_ff_trolling>
    4640:	e3a00000 	mov	r0, #0	; 0x0
    4644:	e28dd004 	add	sp, sp, #4	; 0x4
    4648:	e8bd8030 	pop	{r4, r5, pc}
    464c:	00017324 	.word	0x00017324
    4650:	3c00d6cc 	.word	0x3c00d6cc
    4654:	0001736a 	.word	0x0001736a

00004658 <tt_cmp_buffer>:
    4658:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    465c:	e3a04000 	mov	r4, #0	; 0x0
    4660:	e24dd004 	sub	sp, sp, #4	; 0x4
    4664:	e1a0a002 	mov	sl, r2
    4668:	e1a07000 	mov	r7, r0
    466c:	e1a06001 	mov	r6, r1
    4670:	e1a08003 	mov	r8, r3
    4674:	e1a09142 	asr	r9, r2, #2
    4678:	e1a05004 	mov	r5, r4
    467c:	e59fb0e4 	ldr	fp, [pc, #228]	; 4768 <tt_cmp_buffer+0x110>
    4680:	ea00001f 	b	4704 <tt_cmp_buffer+0xac>
    4684:	e5970000 	ldr	r0, [r7]
    4688:	e5961000 	ldr	r1, [r6]
    468c:	e1500001 	cmp	r0, r1
    4690:	0a000018 	beq	46f8 <tt_cmp_buffer+0xa0>
    4694:	e5d72000 	ldrb	r2, [r7]
    4698:	e5d63000 	ldrb	r3, [r6]
    469c:	e1520003 	cmp	r2, r3
    46a0:	e5d72001 	ldrb	r2, [r7, #1]
    46a4:	e5d63001 	ldrb	r3, [r6, #1]
    46a8:	12855001 	addne	r5, r5, #1	; 0x1
    46ac:	e1520003 	cmp	r2, r3
    46b0:	e5d72002 	ldrb	r2, [r7, #2]
    46b4:	e5d63002 	ldrb	r3, [r6, #2]
    46b8:	12855001 	addne	r5, r5, #1	; 0x1
    46bc:	e1520003 	cmp	r2, r3
    46c0:	e5d72003 	ldrb	r2, [r7, #3]
    46c4:	e5d63003 	ldrb	r3, [r6, #3]
    46c8:	12855001 	addne	r5, r5, #1	; 0x1
    46cc:	e1520003 	cmp	r2, r3
    46d0:	12855001 	addne	r5, r5, #1	; 0x1
    46d4:	e3580000 	cmp	r8, #0	; 0x0
    46d8:	0a000006 	beq	46f8 <tt_cmp_buffer+0xa0>
    46dc:	e1a02000 	mov	r2, r0
    46e0:	e1a03001 	mov	r3, r1
    46e4:	e59f0080 	ldr	r0, [pc, #128]	; 476c <tt_cmp_buffer+0x114>
    46e8:	e1a01104 	lsl	r1, r4, #2
    46ec:	e1a0e00f 	mov	lr, pc
    46f0:	e59bf000 	ldr	pc, [fp]
    46f4:	e2488001 	sub	r8, r8, #1	; 0x1
    46f8:	e2844001 	add	r4, r4, #1	; 0x1
    46fc:	e2877004 	add	r7, r7, #4	; 0x4
    4700:	e2866004 	add	r6, r6, #4	; 0x4
    4704:	e1540009 	cmp	r4, r9
    4708:	baffffdd 	blt	4684 <tt_cmp_buffer+0x2c>
    470c:	e3ca9003 	bic	r9, sl, #3	; 0x3
    4710:	e3a04000 	mov	r4, #0	; 0x0
    4714:	e20aa003 	and	sl, sl, #3	; 0x3
    4718:	e59fb048 	ldr	fp, [pc, #72]	; 4768 <tt_cmp_buffer+0x110>
    471c:	ea00000c 	b	4754 <tt_cmp_buffer+0xfc>
    4720:	e7d72004 	ldrb	r2, [r7, r4]
    4724:	e7d63004 	ldrb	r3, [r6, r4]
    4728:	e1520003 	cmp	r2, r3
    472c:	0a000007 	beq	4750 <tt_cmp_buffer+0xf8>
    4730:	e3580000 	cmp	r8, #0	; 0x0
    4734:	e2855001 	add	r5, r5, #1	; 0x1
    4738:	0a000004 	beq	4750 <tt_cmp_buffer+0xf8>
    473c:	e0841009 	add	r1, r4, r9
    4740:	e59f0028 	ldr	r0, [pc, #40]	; 4770 <tt_cmp_buffer+0x118>
    4744:	e1a0e00f 	mov	lr, pc
    4748:	e59bf000 	ldr	pc, [fp]
    474c:	e2488001 	sub	r8, r8, #1	; 0x1
    4750:	e2844001 	add	r4, r4, #1	; 0x1
    4754:	e154000a 	cmp	r4, sl
    4758:	bafffff0 	blt	4720 <tt_cmp_buffer+0xc8>
    475c:	e1a00005 	mov	r0, r5
    4760:	e28dd004 	add	sp, sp, #4	; 0x4
    4764:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4768:	3c00d6cc 	.word	0x3c00d6cc
    476c:	00017391 	.word	0x00017391
    4770:	000173bc 	.word	0x000173bc

00004774 <tt_rnd_seed>:
    4774:	e92d4030 	push	{r4, r5, lr}
    4778:	e59f5024 	ldr	r5, [pc, #36]	; 47a4 <tt_rnd_seed+0x30>
    477c:	e24dd004 	sub	sp, sp, #4	; 0x4
    4780:	e3a00000 	mov	r0, #0	; 0x0
    4784:	e5954000 	ldr	r4, [r5]
    4788:	eb0024b7 	bl	da6c <get_timer>
    478c:	e30f3ffc 	movw	r3, #65532	; 0xfffc
    4790:	e6e40070 	uxtab	r0, r4, r0
    4794:	e0003003 	and	r3, r0, r3
    4798:	e5853000 	str	r3, [r5]
    479c:	e28dd004 	add	sp, sp, #4	; 0x4
    47a0:	e8bd8030 	pop	{r4, r5, pc}
    47a4:	3c00d150 	.word	0x3c00d150

000047a8 <tt_rnd_get_int>:
    47a8:	e59fc094 	ldr	ip, [pc, #148]	; 4844 <tt_rnd_get_int+0x9c>
    47ac:	e3a02102 	mov	r2, #-2147483648	; 0x80000000
    47b0:	e59c3000 	ldr	r3, [ip]
    47b4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    47b8:	e19330b2 	ldrh	r3, [r3, r2]
    47bc:	e24dd004 	sub	sp, sp, #4	; 0x4
    47c0:	e3c33003 	bic	r3, r3, #3	; 0x3
    47c4:	e6ff3073 	uxth	r3, r3
    47c8:	e19340b2 	ldrh	r4, [r3, r2]
    47cc:	e58c3000 	str	r3, [ip]
    47d0:	e1a07000 	mov	r7, r0
    47d4:	e1a06001 	mov	r6, r1
    47d8:	ebffffe5 	bl	4774 <tt_rnd_seed>
    47dc:	e1a00004 	mov	r0, r4
    47e0:	eb0041bc 	bl	14ed8 <__aeabi_ui2d>
    47e4:	e3a02000 	mov	r2, #0	; 0x0
    47e8:	e59f3058 	ldr	r3, [pc, #88]	; 4848 <tt_rnd_get_int+0xa0>
    47ec:	eb004294 	bl	15244 <__aeabi_ddiv>
    47f0:	e1a04000 	mov	r4, r0
    47f4:	e0670006 	rsb	r0, r7, r6
    47f8:	e1a05001 	mov	r5, r1
    47fc:	eb0041be 	bl	14efc <__aeabi_i2d>
    4800:	e1a02000 	mov	r2, r0
    4804:	e1a03001 	mov	r3, r1
    4808:	e1a00004 	mov	r0, r4
    480c:	e1a01005 	mov	r1, r5
    4810:	eb0041f0 	bl	14fd8 <__aeabi_dmul>
    4814:	e1a04000 	mov	r4, r0
    4818:	e1a00007 	mov	r0, r7
    481c:	e1a05001 	mov	r5, r1
    4820:	eb0041b5 	bl	14efc <__aeabi_i2d>
    4824:	e1a02000 	mov	r2, r0
    4828:	e1a03001 	mov	r3, r1
    482c:	e1a00004 	mov	r0, r4
    4830:	e1a01005 	mov	r1, r5
    4834:	eb0040fc 	bl	14c2c <__adddf3>
    4838:	eb004302 	bl	15448 <__aeabi_d2iz>
    483c:	e28dd004 	add	sp, sp, #4	; 0x4
    4840:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    4844:	3c00d150 	.word	0x3c00d150
    4848:	40efffe0 	.word	0x40efffe0

0000484c <tt_rnd_get_data>:
    484c:	e92d4010 	push	{r4, lr}
    4850:	e1a04000 	mov	r4, r0
    4854:	e1a00001 	mov	r0, r1
    4858:	e1a01002 	mov	r1, r2
    485c:	ebffffd1 	bl	47a8 <tt_rnd_get_int>
    4860:	e59f3010 	ldr	r3, [pc, #16]	; 4878 <tt_rnd_get_data+0x2c>
    4864:	e5840000 	str	r0, [r4]
    4868:	e5930000 	ldr	r0, [r3]
    486c:	e3c00003 	bic	r0, r0, #3	; 0x3
    4870:	e2800102 	add	r0, r0, #-2147483648	; 0x80000000
    4874:	e8bd8010 	pop	{r4, pc}
    4878:	3c00d150 	.word	0x3c00d150

0000487c <do_tt_nbi>:
    487c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    4880:	e3a00001 	mov	r0, #1	; 0x1
    4884:	e24dd054 	sub	sp, sp, #84	; 0x54
    4888:	ebffef4f 	bl	5cc <get_xom>
    488c:	e59f30d0 	ldr	r3, [pc, #208]	; 4964 <do_tt_nbi+0xe8>
    4890:	e28d4010 	add	r4, sp, #16	; 0x10
    4894:	e1a0c003 	mov	ip, r3
    4898:	e1a06000 	mov	r6, r0
    489c:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    48a0:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    48a4:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    48a8:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    48ac:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    48b0:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    48b4:	e89c000f 	ldm	ip, {r0, r1, r2, r3}
    48b8:	e59fa0a8 	ldr	sl, [pc, #168]	; 4968 <do_tt_nbi+0xec>
    48bc:	e59f80a8 	ldr	r8, [pc, #168]	; 496c <do_tt_nbi+0xf0>
    48c0:	e59f70a8 	ldr	r7, [pc, #168]	; 4970 <do_tt_nbi+0xf4>
    48c4:	e884000f 	stm	r4, {r0, r1, r2, r3}
    48c8:	e3a0400a 	mov	r4, #10	; 0xa
    48cc:	e1a00004 	mov	r0, r4
    48d0:	ebffef41 	bl	5dc <set_xom>
    48d4:	eb001a4c 	bl	b20c <nand_rescan_bootcfg>
    48d8:	e3a00000 	mov	r0, #0	; 0x0
    48dc:	eb001594 	bl	9f34 <nand_get_config>
    48e0:	e5d03005 	ldrb	r3, [r0, #5]
    48e4:	e5d02006 	ldrb	r2, [r0, #6]
    48e8:	e3530000 	cmp	r3, #0	; 0x0
    48ec:	15d03003 	ldrbne	r3, [r0, #3]
    48f0:	128d1050 	addne	r1, sp, #80	; 0x50
    48f4:	10813103 	addne	r3, r1, r3, lsl #2
    48f8:	e5d01000 	ldrb	r1, [r0]
    48fc:	15133040 	ldrne	r3, [r3, #-64]
    4900:	e3510000 	cmp	r1, #0	; 0x0
    4904:	e590e014 	ldr	lr, [r0, #20]
    4908:	e59a5000 	ldr	r5, [sl]
    490c:	e5d0c002 	ldrb	ip, [r0, #2]
    4910:	059f105c 	ldreq	r1, [pc, #92]	; 4974 <do_tt_nbi+0xf8>
    4914:	0a000002 	beq	4924 <do_tt_nbi+0xa8>
    4918:	e3510001 	cmp	r1, #1	; 0x1
    491c:	01a01008 	moveq	r1, r8
    4920:	11a01007 	movne	r1, r7
    4924:	e58d1008 	str	r1, [sp, #8]
    4928:	e88d1008 	stm	sp, {r3, ip}
    492c:	e5d03001 	ldrb	r3, [r0, #1]
    4930:	e1a01004 	mov	r1, r4
    4934:	e58d300c 	str	r3, [sp, #12]
    4938:	e2844001 	add	r4, r4, #1	; 0x1
    493c:	e1a0300e 	mov	r3, lr
    4940:	e59f0030 	ldr	r0, [pc, #48]	; 4978 <do_tt_nbi+0xfc>
    4944:	e12fff35 	blx	r5
    4948:	e354001c 	cmp	r4, #28	; 0x1c
    494c:	1affffde 	bne	48cc <do_tt_nbi+0x50>
    4950:	e1a00006 	mov	r0, r6
    4954:	ebffef20 	bl	5dc <set_xom>
    4958:	e3a00000 	mov	r0, #0	; 0x0
    495c:	e28dd054 	add	sp, sp, #84	; 0x54
    4960:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    4964:	00015574 	.word	0x00015574
    4968:	3c00d6cc 	.word	0x3c00d6cc
    496c:	000173f5 	.word	0x000173f5
    4970:	000173ee 	.word	0x000173ee
    4974:	000173e7 	.word	0x000173e7
    4978:	000173fa 	.word	0x000173fa

0000497c <tt_simple_test>:
    497c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    4980:	e1a0a001 	mov	sl, r1
    4984:	e24dd044 	sub	sp, sp, #68	; 0x44
    4988:	e3a01001 	mov	r1, #1	; 0x1
    498c:	e1a05000 	mov	r5, r0
    4990:	eb0006ef 	bl	6554 <vs_assign_by_name>
    4994:	e3500000 	cmp	r0, #0	; 0x0
    4998:	13e00000 	mvnne	r0, #0	; 0x0
    499c:	1a000048 	bne	4ac4 <tt_simple_test+0x148>
    49a0:	e1a00005 	mov	r0, r5
    49a4:	e59f1120 	ldr	r1, [pc, #288]	; 4acc <tt_simple_test+0x150>
    49a8:	e3a02003 	mov	r2, #3	; 0x3
    49ac:	ebfff2af 	bl	1470 <strncmp>
    49b0:	e3500000 	cmp	r0, #0	; 0x0
    49b4:	1a000001 	bne	49c0 <tt_simple_test+0x44>
    49b8:	eb00158b 	bl	9fec <fnd_align>
    49bc:	ea000001 	b	49c8 <tt_simple_test+0x4c>
    49c0:	e3a0000e 	mov	r0, #14	; 0xe
    49c4:	eb000570 	bl	5f8c <vs_align>
    49c8:	e1a06400 	lsl	r6, r0, #8
    49cc:	e1a00005 	mov	r0, r5
    49d0:	eb0005ee 	bl	6190 <vs_device_id>
    49d4:	e59f80f4 	ldr	r8, [pc, #244]	; 4ad0 <tt_simple_test+0x154>
    49d8:	e1a07000 	mov	r7, r0
    49dc:	e5984000 	ldr	r4, [r8]
    49e0:	eb000569 	bl	5f8c <vs_align>
    49e4:	e1a01005 	mov	r1, r5
    49e8:	e1a02000 	mov	r2, r0
    49ec:	e59f00e0 	ldr	r0, [pc, #224]	; 4ad4 <tt_simple_test+0x158>
    49f0:	e12fff34 	blx	r4
    49f4:	e35a0000 	cmp	sl, #0	; 0x0
    49f8:	1a00001a 	bne	4a68 <tt_simple_test+0xec>
    49fc:	e1a00007 	mov	r0, r7
    4a00:	eb0005ce 	bl	6140 <vs_device_erasable>
    4a04:	e3500000 	cmp	r0, #0	; 0x0
    4a08:	0a000009 	beq	4a34 <tt_simple_test+0xb8>
    4a0c:	e59f00c4 	ldr	r0, [pc, #196]	; 4ad8 <tt_simple_test+0x15c>
    4a10:	e1a0e00f 	mov	lr, pc
    4a14:	e598f000 	ldr	pc, [r8]
    4a18:	e59f10bc 	ldr	r1, [pc, #188]	; 4adc <tt_simple_test+0x160>
    4a1c:	e1a0000d 	mov	r0, sp
    4a20:	e1a02006 	mov	r2, r6
    4a24:	eb0036e0 	bl	125ac <sprintf>
    4a28:	e1a0000d 	mov	r0, sp
    4a2c:	e1a0100a 	mov	r1, sl
    4a30:	eb0037a5 	bl	128cc <run_command>
    4a34:	e59f3094 	ldr	r3, [pc, #148]	; 4ad0 <tt_simple_test+0x154>
    4a38:	e59f00a0 	ldr	r0, [pc, #160]	; 4ae0 <tt_simple_test+0x164>
    4a3c:	e1a0e00f 	mov	lr, pc
    4a40:	e593f000 	ldr	pc, [r3]
    4a44:	e1a02006 	mov	r2, r6
    4a48:	e59f1094 	ldr	r1, [pc, #148]	; 4ae4 <tt_simple_test+0x168>
    4a4c:	e1a0000d 	mov	r0, sp
    4a50:	eb0036d5 	bl	125ac <sprintf>
    4a54:	e1a0000d 	mov	r0, sp
    4a58:	e3a01000 	mov	r1, #0	; 0x0
    4a5c:	e1a0400d 	mov	r4, sp
    4a60:	eb003799 	bl	128cc <run_command>
    4a64:	ea000015 	b	4ac0 <tt_simple_test+0x144>
    4a68:	e59f0078 	ldr	r0, [pc, #120]	; 4ae8 <tt_simple_test+0x16c>
    4a6c:	e1a0e00f 	mov	lr, pc
    4a70:	e598f000 	ldr	pc, [r8]
    4a74:	e1a02006 	mov	r2, r6
    4a78:	e59f106c 	ldr	r1, [pc, #108]	; 4aec <tt_simple_test+0x170>
    4a7c:	e1a0000d 	mov	r0, sp
    4a80:	eb0036c9 	bl	125ac <sprintf>
    4a84:	e3a01000 	mov	r1, #0	; 0x0
    4a88:	e1a0000d 	mov	r0, sp
    4a8c:	eb00378e 	bl	128cc <run_command>
    4a90:	e59f0058 	ldr	r0, [pc, #88]	; 4af0 <tt_simple_test+0x174>
    4a94:	e1a0e00f 	mov	lr, pc
    4a98:	e598f000 	ldr	pc, [r8]
    4a9c:	e1a02006 	mov	r2, r6
    4aa0:	e59f104c 	ldr	r1, [pc, #76]	; 4af4 <tt_simple_test+0x178>
    4aa4:	e3a03003 	mov	r3, #3	; 0x3
    4aa8:	e59f0048 	ldr	r0, [pc, #72]	; 4af8 <tt_simple_test+0x17c>
    4aac:	ebfffee9 	bl	4658 <tt_cmp_buffer>
    4ab0:	e59f0044 	ldr	r0, [pc, #68]	; 4afc <tt_simple_test+0x180>
    4ab4:	e1a0400d 	mov	r4, sp
    4ab8:	e1a0e00f 	mov	lr, pc
    4abc:	e598f000 	ldr	pc, [r8]
    4ac0:	e3a00000 	mov	r0, #0	; 0x0
    4ac4:	e28dd044 	add	sp, sp, #68	; 0x44
    4ac8:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    4acc:	00016283 	.word	0x00016283
    4ad0:	3c00d6cc 	.word	0x3c00d6cc
    4ad4:	000176f6 	.word	0x000176f6
    4ad8:	00017416 	.word	0x00017416
    4adc:	00017423 	.word	0x00017423
    4ae0:	00017438 	.word	0x00017438
    4ae4:	00017445 	.word	0x00017445
    4ae8:	00017463 	.word	0x00017463
    4aec:	00017470 	.word	0x00017470
    4af0:	0001748d 	.word	0x0001748d
    4af4:	50008000 	.word	0x50008000
    4af8:	40008000 	.word	0x40008000
    4afc:	00016bef 	.word	0x00016bef

00004b00 <do_tt_randomizer>:
    4b00:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4b04:	e24dd064 	sub	sp, sp, #100	; 0x64
    4b08:	e1a06002 	mov	r6, r2
    4b0c:	e28d0010 	add	r0, sp, #16	; 0x10
    4b10:	e59f12dc 	ldr	r1, [pc, #732]	; 4df4 <do_tt_randomizer+0x2f4>
    4b14:	e3a02050 	mov	r2, #80	; 0x50
    4b18:	e1a05003 	mov	r5, r3
    4b1c:	ebfff28c 	bl	1554 <memcpy>
    4b20:	e3560001 	cmp	r6, #1	; 0x1
    4b24:	d59f02cc 	ldrle	r0, [pc, #716]	; 4df8 <do_tt_randomizer+0x2f8>
    4b28:	da000009 	ble	4b54 <do_tt_randomizer+0x54>
    4b2c:	e5950004 	ldr	r0, [r5, #4]
    4b30:	e59f12c4 	ldr	r1, [pc, #708]	; 4dfc <do_tt_randomizer+0x2fc>
    4b34:	e3a02003 	mov	r2, #3	; 0x3
    4b38:	ebfff24c 	bl	1470 <strncmp>
    4b3c:	e2504000 	subs	r4, r0, #0	; 0x0
    4b40:	e2858004 	add	r8, r5, #4	; 0x4
    4b44:	1a000010 	bne	4b8c <do_tt_randomizer+0x8c>
    4b48:	e3560002 	cmp	r6, #2	; 0x2
    4b4c:	1a000005 	bne	4b68 <do_tt_randomizer+0x68>
    4b50:	e59f02a8 	ldr	r0, [pc, #680]	; 4e00 <do_tt_randomizer+0x300>
    4b54:	e59f32a8 	ldr	r3, [pc, #680]	; 4e04 <do_tt_randomizer+0x304>
    4b58:	e1a0e00f 	mov	lr, pc
    4b5c:	e593f000 	ldr	pc, [r3]
    4b60:	e3e00000 	mvn	r0, #0	; 0x0
    4b64:	ea0000a0 	b	4dec <do_tt_randomizer+0x2ec>
    4b68:	e1a01004 	mov	r1, r4
    4b6c:	e3a02010 	mov	r2, #16	; 0x10
    4b70:	e5950008 	ldr	r0, [r5, #8]
    4b74:	eb003300 	bl	1177c <simple_strtoul>
    4b78:	ebffee97 	bl	5dc <set_xom>
    4b7c:	e1a00004 	mov	r0, r4
    4b80:	eb001aee 	bl	b740 <nand_rescan_config>
    4b84:	e1a00004 	mov	r0, r4
    4b88:	ea000000 	b	4b90 <do_tt_randomizer+0x90>
    4b8c:	e3a00001 	mov	r0, #1	; 0x1
    4b90:	eb0014e7 	bl	9f34 <nand_get_config>
    4b94:	e3500000 	cmp	r0, #0	; 0x0
    4b98:	e1a05000 	mov	r5, r0
    4b9c:	059f0264 	ldreq	r0, [pc, #612]	; 4e08 <do_tt_randomizer+0x308>
    4ba0:	0affffeb 	beq	4b54 <do_tt_randomizer+0x54>
    4ba4:	e3a00001 	mov	r0, #1	; 0x1
    4ba8:	e3a01000 	mov	r1, #0	; 0x0
    4bac:	e59fb250 	ldr	fp, [pc, #592]	; 4e04 <do_tt_randomizer+0x304>
    4bb0:	e58d0008 	str	r0, [sp, #8]
    4bb4:	e58d100c 	str	r1, [sp, #12]
    4bb8:	e59d000c 	ldr	r0, [sp, #12]
    4bbc:	e59f2248 	ldr	r2, [pc, #584]	; 4e0c <do_tt_randomizer+0x30c>
    4bc0:	e3a09000 	mov	r9, #0	; 0x0
    4bc4:	e7923000 	ldr	r3, [r2, r0]
    4bc8:	e59f0240 	ldr	r0, [pc, #576]	; 4e10 <do_tt_randomizer+0x310>
    4bcc:	e1a01003 	mov	r1, r3
    4bd0:	e5853048 	str	r3, [r5, #72]
    4bd4:	e59f2228 	ldr	r2, [pc, #552]	; 4e04 <do_tt_randomizer+0x304>
    4bd8:	e1a0e00f 	mov	lr, pc
    4bdc:	e592f000 	ldr	pc, [r2]
    4be0:	e59f1224 	ldr	r1, [pc, #548]	; 4e0c <do_tt_randomizer+0x30c>
    4be4:	e59d200c 	ldr	r2, [sp, #12]
    4be8:	e59d0008 	ldr	r0, [sp, #8]
    4bec:	e0813002 	add	r3, r1, r2
    4bf0:	e3a0100a 	mov	r1, #10	; 0xa
    4bf4:	e58d3000 	str	r3, [sp]
    4bf8:	eb003fed 	bl	14bb4 <__aeabi_idivmod>
    4bfc:	e28da010 	add	sl, sp, #16	; 0x10
    4c00:	e58d1004 	str	r1, [sp, #4]
    4c04:	ea00006a 	b	4db4 <do_tt_randomizer+0x2b4>
    4c08:	e4972004 	ldr	r2, [r7], #4
    4c0c:	e1a01004 	mov	r1, r4
    4c10:	e5862038 	str	r2, [r6, #56]
    4c14:	e2844001 	add	r4, r4, #1	; 0x1
    4c18:	e59f01f4 	ldr	r0, [pc, #500]	; 4e14 <do_tt_randomizer+0x314>
    4c1c:	e59f31e0 	ldr	r3, [pc, #480]	; 4e04 <do_tt_randomizer+0x304>
    4c20:	e1a0e00f 	mov	lr, pc
    4c24:	e593f000 	ldr	pc, [r3]
    4c28:	e3540004 	cmp	r4, #4	; 0x4
    4c2c:	e2866004 	add	r6, r6, #4	; 0x4
    4c30:	1afffff4 	bne	4c08 <do_tt_randomizer+0x108>
    4c34:	e3a00001 	mov	r0, #1	; 0x1
    4c38:	e5c50001 	strb	r0, [r5, #1]
    4c3c:	e3a00000 	mov	r0, #0	; 0x0
    4c40:	eb002389 	bl	da6c <get_timer>
    4c44:	e1a04000 	mov	r4, r0
    4c48:	e1a00005 	mov	r0, r5
    4c4c:	eb0019e8 	bl	b3f4 <nand_init_randomizer>
    4c50:	e1a00004 	mov	r0, r4
    4c54:	eb002384 	bl	da6c <get_timer>
    4c58:	e1a01000 	mov	r1, r0
    4c5c:	e59f01b4 	ldr	r0, [pc, #436]	; 4e18 <do_tt_randomizer+0x318>
    4c60:	e1a0e00f 	mov	lr, pc
    4c64:	e59bf000 	ldr	pc, [fp]
    4c68:	e3a01000 	mov	r1, #0	; 0x0
    4c6c:	e5980000 	ldr	r0, [r8]
    4c70:	ebffff41 	bl	497c <tt_simple_test>
    4c74:	e3a01000 	mov	r1, #0	; 0x0
    4c78:	e5c51001 	strb	r1, [r5, #1]
    4c7c:	e59f0198 	ldr	r0, [pc, #408]	; 4e1c <do_tt_randomizer+0x31c>
    4c80:	e1a0e00f 	mov	lr, pc
    4c84:	e59bf000 	ldr	pc, [fp]
    4c88:	e3a01001 	mov	r1, #1	; 0x1
    4c8c:	e5980000 	ldr	r0, [r8]
    4c90:	ebffff39 	bl	497c <tt_simple_test>
    4c94:	e3a02001 	mov	r2, #1	; 0x1
    4c98:	e5c52001 	strb	r2, [r5, #1]
    4c9c:	e59f017c 	ldr	r0, [pc, #380]	; 4e20 <do_tt_randomizer+0x320>
    4ca0:	e1a0e00f 	mov	lr, pc
    4ca4:	e59bf000 	ldr	pc, [fp]
    4ca8:	e3a01001 	mov	r1, #1	; 0x1
    4cac:	e5980000 	ldr	r0, [r8]
    4cb0:	ebffff31 	bl	497c <tt_simple_test>
    4cb4:	e2899001 	add	r9, r9, #1	; 0x1
    4cb8:	e59f0164 	ldr	r0, [pc, #356]	; 4e24 <do_tt_randomizer+0x324>
    4cbc:	e1a0e00f 	mov	lr, pc
    4cc0:	e59bf000 	ldr	pc, [fp]
    4cc4:	e1a00009 	mov	r0, r9
    4cc8:	e3a01005 	mov	r1, #5	; 0x5
    4ccc:	eb003fb8 	bl	14bb4 <__aeabi_idivmod>
    4cd0:	e28d3060 	add	r3, sp, #96	; 0x60
    4cd4:	e083c201 	add	ip, r3, r1, lsl #4
    4cd8:	e51c3050 	ldr	r3, [ip, #-80]
    4cdc:	e1a00005 	mov	r0, r5
    4ce0:	e5853038 	str	r3, [r5, #56]
    4ce4:	e51c304c 	ldr	r3, [ip, #-76]
    4ce8:	e585303c 	str	r3, [r5, #60]
    4cec:	e51c3048 	ldr	r3, [ip, #-72]
    4cf0:	e5853040 	str	r3, [r5, #64]
    4cf4:	e51c3044 	ldr	r3, [ip, #-68]
    4cf8:	e5853044 	str	r3, [r5, #68]
    4cfc:	eb0019bc 	bl	b3f4 <nand_init_randomizer>
    4d00:	e3a01001 	mov	r1, #1	; 0x1
    4d04:	e5980000 	ldr	r0, [r8]
    4d08:	ebffff1b 	bl	497c <tt_simple_test>
    4d0c:	e59f0114 	ldr	r0, [pc, #276]	; 4e28 <do_tt_randomizer+0x328>
    4d10:	e1a0e00f 	mov	lr, pc
    4d14:	e59bf000 	ldr	pc, [fp]
    4d18:	e59a3000 	ldr	r3, [sl]
    4d1c:	e1a00005 	mov	r0, r5
    4d20:	e5853038 	str	r3, [r5, #56]
    4d24:	e59a3004 	ldr	r3, [sl, #4]
    4d28:	e585303c 	str	r3, [r5, #60]
    4d2c:	e59a3008 	ldr	r3, [sl, #8]
    4d30:	e5853040 	str	r3, [r5, #64]
    4d34:	e59a300c 	ldr	r3, [sl, #12]
    4d38:	e28aa010 	add	sl, sl, #16	; 0x10
    4d3c:	e5853044 	str	r3, [r5, #68]
    4d40:	eb0019ab 	bl	b3f4 <nand_init_randomizer>
    4d44:	e3a01001 	mov	r1, #1	; 0x1
    4d48:	e5980000 	ldr	r0, [r8]
    4d4c:	ebffff0a 	bl	497c <tt_simple_test>
    4d50:	e59f00d4 	ldr	r0, [pc, #212]	; 4e2c <do_tt_randomizer+0x32c>
    4d54:	e1a0e00f 	mov	lr, pc
    4d58:	e59bf000 	ldr	pc, [fp]
    4d5c:	e59f20a8 	ldr	r2, [pc, #168]	; 4e0c <do_tt_randomizer+0x30c>
    4d60:	e59d1004 	ldr	r1, [sp, #4]
    4d64:	e1a00005 	mov	r0, r5
    4d68:	e7923101 	ldr	r3, [r2, r1, lsl #2]
    4d6c:	e5853048 	str	r3, [r5, #72]
    4d70:	eb00199f 	bl	b3f4 <nand_init_randomizer>
    4d74:	e3a01001 	mov	r1, #1	; 0x1
    4d78:	e5980000 	ldr	r0, [r8]
    4d7c:	ebfffefe 	bl	497c <tt_simple_test>
    4d80:	e59f00a8 	ldr	r0, [pc, #168]	; 4e30 <do_tt_randomizer+0x330>
    4d84:	e1a0e00f 	mov	lr, pc
    4d88:	e59bf000 	ldr	pc, [fp]
    4d8c:	e59d0000 	ldr	r0, [sp]
    4d90:	e5903000 	ldr	r3, [r0]
    4d94:	e1a00005 	mov	r0, r5
    4d98:	e5853048 	str	r3, [r5, #72]
    4d9c:	eb001994 	bl	b3f4 <nand_init_randomizer>
    4da0:	e5980000 	ldr	r0, [r8]
    4da4:	e3a01001 	mov	r1, #1	; 0x1
    4da8:	ebfffef3 	bl	497c <tt_simple_test>
    4dac:	e3590004 	cmp	r9, #4	; 0x4
    4db0:	0a000004 	beq	4dc8 <do_tt_randomizer+0x2c8>
    4db4:	e28d1010 	add	r1, sp, #16	; 0x10
    4db8:	e0817209 	add	r7, r1, r9, lsl #4
    4dbc:	e1a06005 	mov	r6, r5
    4dc0:	e3a04000 	mov	r4, #0	; 0x0
    4dc4:	eaffff8f 	b	4c08 <do_tt_randomizer+0x108>
    4dc8:	e59d2008 	ldr	r2, [sp, #8]
    4dcc:	e59d300c 	ldr	r3, [sp, #12]
    4dd0:	e2822001 	add	r2, r2, #1	; 0x1
    4dd4:	e2833004 	add	r3, r3, #4	; 0x4
    4dd8:	e3520006 	cmp	r2, #6	; 0x6
    4ddc:	e58d2008 	str	r2, [sp, #8]
    4de0:	e58d300c 	str	r3, [sp, #12]
    4de4:	1affff73 	bne	4bb8 <do_tt_randomizer+0xb8>
    4de8:	e3a00000 	mov	r0, #0	; 0x0
    4dec:	e28dd064 	add	sp, sp, #100	; 0x64
    4df0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4df4:	000155b4 	.word	0x000155b4
    4df8:	0001749c 	.word	0x0001749c
    4dfc:	00016127 	.word	0x00016127
    4e00:	000174bc 	.word	0x000174bc
    4e04:	3c00d6cc 	.word	0x3c00d6cc
    4e08:	000174d3 	.word	0x000174d3
    4e0c:	000113e0 	.word	0x000113e0
    4e10:	000174f8 	.word	0x000174f8
    4e14:	00017510 	.word	0x00017510
    4e18:	0001752a 	.word	0x0001752a
    4e1c:	0001754d 	.word	0x0001754d
    4e20:	00017576 	.word	0x00017576
    4e24:	0001759e 	.word	0x0001759e
    4e28:	000175c6 	.word	0x000175c6
    4e2c:	000175f3 	.word	0x000175f3
    4e30:	0001761b 	.word	0x0001761b

00004e34 <tt_nand_cross_cmp>:
    4e34:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4e38:	e24dd004 	sub	sp, sp, #4	; 0x4
    4e3c:	e59da028 	ldr	sl, [sp, #40]
    4e40:	e3530000 	cmp	r3, #0	; 0x0
    4e44:	e1a05002 	mov	r5, r2
    4e48:	13a02000 	movne	r2, #0	; 0x0
    4e4c:	059a2010 	ldreq	r2, [sl, #16]
    4e50:	e3530000 	cmp	r3, #0	; 0x0
    4e54:	e1a06001 	mov	r6, r1
    4e58:	159a3010 	ldrne	r3, [sl, #16]
    4e5c:	e59a1014 	ldr	r1, [sl, #20]
    4e60:	e1a07000 	mov	r7, r0
    4e64:	e0838001 	add	r8, r3, r1
    4e68:	e082b001 	add	fp, r2, r1
    4e6c:	e3a04000 	mov	r4, #0	; 0x0
    4e70:	e2689000 	rsb	r9, r8, #0	; 0x0
    4e74:	e3a03000 	mov	r3, #0	; 0x0
    4e78:	e1550003 	cmp	r5, r3
    4e7c:	e1a00007 	mov	r0, r7
    4e80:	e1a01006 	mov	r1, r6
    4e84:	e087700b 	add	r7, r7, fp
    4e88:	e0866008 	add	r6, r6, r8
    4e8c:	e0855009 	add	r5, r5, r9
    4e90:	da000003 	ble	4ea4 <tt_nand_cross_cmp+0x70>
    4e94:	e59a2014 	ldr	r2, [sl, #20]
    4e98:	ebfffdee 	bl	4658 <tt_cmp_buffer>
    4e9c:	e0844000 	add	r4, r4, r0
    4ea0:	eafffff3 	b	4e74 <tt_nand_cross_cmp+0x40>
    4ea4:	e1a00004 	mov	r0, r4
    4ea8:	e28dd004 	add	sp, sp, #4	; 0x4
    4eac:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

00004eb0 <do_tt_sysinfo>:
    4eb0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4eb4:	e3520001 	cmp	r2, #1	; 0x1
    4eb8:	e24dd044 	sub	sp, sp, #68	; 0x44
    4ebc:	e1a04003 	mov	r4, r3
    4ec0:	ca000003 	bgt	4ed4 <do_tt_sysinfo+0x24>
    4ec4:	e59f01ac 	ldr	r0, [pc, #428]	; 5078 <do_tt_sysinfo+0x1c8>
    4ec8:	e59f31ac 	ldr	r3, [pc, #428]	; 507c <do_tt_sysinfo+0x1cc>
    4ecc:	e1a0e00f 	mov	lr, pc
    4ed0:	e593f000 	ldr	pc, [r3]
    4ed4:	e3a00001 	mov	r0, #1	; 0x1
    4ed8:	eb001415 	bl	9f34 <nand_get_config>
    4edc:	e2505000 	subs	r5, r0, #0	; 0x0
    4ee0:	1a000004 	bne	4ef8 <do_tt_sysinfo+0x48>
    4ee4:	e59f0194 	ldr	r0, [pc, #404]	; 5080 <do_tt_sysinfo+0x1d0>
    4ee8:	e59f318c 	ldr	r3, [pc, #396]	; 507c <do_tt_sysinfo+0x1cc>
    4eec:	e1a0e00f 	mov	lr, pc
    4ef0:	e593f000 	ldr	pc, [r3]
    4ef4:	ea000004 	b	4f0c <do_tt_sysinfo+0x5c>
    4ef8:	e59f0184 	ldr	r0, [pc, #388]	; 5084 <do_tt_sysinfo+0x1d4>
    4efc:	e3a01001 	mov	r1, #1	; 0x1
    4f00:	eb000593 	bl	6554 <vs_assign_by_name>
    4f04:	e2507000 	subs	r7, r0, #0	; 0x0
    4f08:	0a000001 	beq	4f14 <do_tt_sysinfo+0x64>
    4f0c:	e3e00000 	mvn	r0, #0	; 0x0
    4f10:	ea000056 	b	5070 <do_tt_sysinfo+0x1c0>
    4f14:	e1a01007 	mov	r1, r7
    4f18:	e3a0200a 	mov	r2, #10	; 0xa
    4f1c:	e5940004 	ldr	r0, [r4, #4]
    4f20:	eb003215 	bl	1177c <simple_strtoul>
    4f24:	e5850010 	str	r0, [r5, #16]
    4f28:	e1a04000 	mov	r4, r0
    4f2c:	eb00142e 	bl	9fec <fnd_align>
    4f30:	e1a06480 	lsl	r6, r0, #9
    4f34:	e1a01004 	mov	r1, r4
    4f38:	e59f513c 	ldr	r5, [pc, #316]	; 507c <do_tt_sysinfo+0x1cc>
    4f3c:	e59f0144 	ldr	r0, [pc, #324]	; 5088 <do_tt_sysinfo+0x1d8>
    4f40:	e1a02006 	mov	r2, r6
    4f44:	e1a0e00f 	mov	lr, pc
    4f48:	e595f000 	ldr	pc, [r5]
    4f4c:	e1a0400d 	mov	r4, sp
    4f50:	e1a01007 	mov	r1, r7
    4f54:	e59f0130 	ldr	r0, [pc, #304]	; 508c <do_tt_sysinfo+0x1dc>
    4f58:	e1a0e00f 	mov	lr, pc
    4f5c:	e595f000 	ldr	pc, [r5]
    4f60:	e59f0128 	ldr	r0, [pc, #296]	; 5090 <do_tt_sysinfo+0x1e0>
    4f64:	e1a0e00f 	mov	lr, pc
    4f68:	e595f000 	ldr	pc, [r5]
    4f6c:	e1a02006 	mov	r2, r6
    4f70:	e59f111c 	ldr	r1, [pc, #284]	; 5094 <do_tt_sysinfo+0x1e4>
    4f74:	e1a0000d 	mov	r0, sp
    4f78:	eb00358b 	bl	125ac <sprintf>
    4f7c:	e3a01000 	mov	r1, #0	; 0x0
    4f80:	e1a0000d 	mov	r0, sp
    4f84:	eb003650 	bl	128cc <run_command>
    4f88:	eb003612 	bl	127d8 <tstc>
    4f8c:	e2873001 	add	r3, r7, #1	; 0x1
    4f90:	e3500000 	cmp	r0, #0	; 0x0
    4f94:	e1a07003 	mov	r7, r3
    4f98:	0a000002 	beq	4fa8 <do_tt_sysinfo+0xf8>
    4f9c:	eb00360e 	bl	127dc <getc>
    4fa0:	e3500003 	cmp	r0, #3	; 0x3
    4fa4:	0a000030 	beq	506c <do_tt_sysinfo+0x1bc>
    4fa8:	e59f00e8 	ldr	r0, [pc, #232]	; 5098 <do_tt_sysinfo+0x1e8>
    4fac:	e1a0e00f 	mov	lr, pc
    4fb0:	e595f000 	ldr	pc, [r5]
    4fb4:	e1a02006 	mov	r2, r6
    4fb8:	e59f10dc 	ldr	r1, [pc, #220]	; 509c <do_tt_sysinfo+0x1ec>
    4fbc:	e1a0000d 	mov	r0, sp
    4fc0:	eb003579 	bl	125ac <sprintf>
    4fc4:	e3a01000 	mov	r1, #0	; 0x0
    4fc8:	e1a0000d 	mov	r0, sp
    4fcc:	eb00363e 	bl	128cc <run_command>
    4fd0:	eb003600 	bl	127d8 <tstc>
    4fd4:	e3500000 	cmp	r0, #0	; 0x0
    4fd8:	0a000002 	beq	4fe8 <do_tt_sysinfo+0x138>
    4fdc:	eb0035fe 	bl	127dc <getc>
    4fe0:	e3500003 	cmp	r0, #3	; 0x3
    4fe4:	0a000020 	beq	506c <do_tt_sysinfo+0x1bc>
    4fe8:	e59f00b0 	ldr	r0, [pc, #176]	; 50a0 <do_tt_sysinfo+0x1f0>
    4fec:	e1a0e00f 	mov	lr, pc
    4ff0:	e595f000 	ldr	pc, [r5]
    4ff4:	e1a02006 	mov	r2, r6
    4ff8:	e59f10a4 	ldr	r1, [pc, #164]	; 50a4 <do_tt_sysinfo+0x1f4>
    4ffc:	e1a0000d 	mov	r0, sp
    5000:	eb003569 	bl	125ac <sprintf>
    5004:	e3a01000 	mov	r1, #0	; 0x0
    5008:	e1a0000d 	mov	r0, sp
    500c:	eb00362e 	bl	128cc <run_command>
    5010:	eb0035f0 	bl	127d8 <tstc>
    5014:	e3500000 	cmp	r0, #0	; 0x0
    5018:	0a000002 	beq	5028 <do_tt_sysinfo+0x178>
    501c:	eb0035ee 	bl	127dc <getc>
    5020:	e3500003 	cmp	r0, #3	; 0x3
    5024:	0a000010 	beq	506c <do_tt_sysinfo+0x1bc>
    5028:	e59f0078 	ldr	r0, [pc, #120]	; 50a8 <do_tt_sysinfo+0x1f8>
    502c:	e1a0e00f 	mov	lr, pc
    5030:	e595f000 	ldr	pc, [r5]
    5034:	e59f1070 	ldr	r1, [pc, #112]	; 50ac <do_tt_sysinfo+0x1fc>
    5038:	e1a02006 	mov	r2, r6
    503c:	e3a0300c 	mov	r3, #12	; 0xc
    5040:	e59f0068 	ldr	r0, [pc, #104]	; 50b0 <do_tt_sysinfo+0x200>
    5044:	ebfffd83 	bl	4658 <tt_cmp_buffer>
    5048:	e59f0064 	ldr	r0, [pc, #100]	; 50b4 <do_tt_sysinfo+0x204>
    504c:	e1a0e00f 	mov	lr, pc
    5050:	e595f000 	ldr	pc, [r5]
    5054:	eb0035df 	bl	127d8 <tstc>
    5058:	e3500000 	cmp	r0, #0	; 0x0
    505c:	0affffbb 	beq	4f50 <do_tt_sysinfo+0xa0>
    5060:	eb0035dd 	bl	127dc <getc>
    5064:	e3500003 	cmp	r0, #3	; 0x3
    5068:	1affffb8 	bne	4f50 <do_tt_sysinfo+0xa0>
    506c:	e3a00000 	mov	r0, #0	; 0x0
    5070:	e28dd044 	add	sp, sp, #68	; 0x44
    5074:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5078:	00017648 	.word	0x00017648
    507c:	3c00d6cc 	.word	0x3c00d6cc
    5080:	0001765f 	.word	0x0001765f
    5084:	00016283 	.word	0x00016283
    5088:	0001767a 	.word	0x0001767a
    508c:	00016f62 	.word	0x00016f62
    5090:	00017416 	.word	0x00017416
    5094:	0001769b 	.word	0x0001769b
    5098:	00017438 	.word	0x00017438
    509c:	000176af 	.word	0x000176af
    50a0:	00017463 	.word	0x00017463
    50a4:	000176cc 	.word	0x000176cc
    50a8:	0001748d 	.word	0x0001748d
    50ac:	50008000 	.word	0x50008000
    50b0:	40008000 	.word	0x40008000
    50b4:	00016bef 	.word	0x00016bef

000050b8 <do_tt_sn>:
    50b8:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    50bc:	e3520001 	cmp	r2, #1	; 0x1
    50c0:	e24dd044 	sub	sp, sp, #68	; 0x44
    50c4:	e1a04002 	mov	r4, r2
    50c8:	e1a05003 	mov	r5, r3
    50cc:	ca000003 	bgt	50e0 <do_tt_sn+0x28>
    50d0:	e59f01d8 	ldr	r0, [pc, #472]	; 52b0 <do_tt_sn+0x1f8>
    50d4:	e59f31d8 	ldr	r3, [pc, #472]	; 52b4 <do_tt_sn+0x1fc>
    50d8:	e1a0e00f 	mov	lr, pc
    50dc:	e593f000 	ldr	pc, [r3]
    50e0:	e5950004 	ldr	r0, [r5, #4]
    50e4:	e3a01001 	mov	r1, #1	; 0x1
    50e8:	eb000519 	bl	6554 <vs_assign_by_name>
    50ec:	e250a000 	subs	sl, r0, #0	; 0x0
    50f0:	13e00000 	mvnne	r0, #0	; 0x0
    50f4:	1a00006b 	bne	52a8 <do_tt_sn+0x1f0>
    50f8:	e5950004 	ldr	r0, [r5, #4]
    50fc:	e59f11b4 	ldr	r1, [pc, #436]	; 52b8 <do_tt_sn+0x200>
    5100:	e3a02003 	mov	r2, #3	; 0x3
    5104:	ebfff0d9 	bl	1470 <strncmp>
    5108:	e3500000 	cmp	r0, #0	; 0x0
    510c:	13a07501 	movne	r7, #4194304	; 0x400000
    5110:	1a000001 	bne	511c <do_tt_sn+0x64>
    5114:	eb0013b4 	bl	9fec <fnd_align>
    5118:	e1a07500 	lsl	r7, r0, #10
    511c:	e5950004 	ldr	r0, [r5, #4]
    5120:	eb00041a 	bl	6190 <vs_device_id>
    5124:	e3540002 	cmp	r4, #2	; 0x2
    5128:	e1a08000 	mov	r8, r0
    512c:	da000007 	ble	5150 <do_tt_sn+0x98>
    5130:	e3a01000 	mov	r1, #0	; 0x0
    5134:	e3a0200a 	mov	r2, #10	; 0xa
    5138:	e5950008 	ldr	r0, [r5, #8]
    513c:	eb00318e 	bl	1177c <simple_strtoul>
    5140:	e1a04000 	mov	r4, r0
    5144:	e1a00008 	mov	r0, r8
    5148:	eb00038f 	bl	5f8c <vs_align>
    514c:	e0070094 	mul	r7, r4, r0
    5150:	e59f615c 	ldr	r6, [pc, #348]	; 52b4 <do_tt_sn+0x1fc>
    5154:	e1a00008 	mov	r0, r8
    5158:	e5954004 	ldr	r4, [r5, #4]
    515c:	e5965000 	ldr	r5, [r6]
    5160:	eb000389 	bl	5f8c <vs_align>
    5164:	e1a01004 	mov	r1, r4
    5168:	e1a02000 	mov	r2, r0
    516c:	e59f0148 	ldr	r0, [pc, #328]	; 52bc <do_tt_sn+0x204>
    5170:	e12fff35 	blx	r5
    5174:	e1a0400d 	mov	r4, sp
    5178:	e1a0100a 	mov	r1, sl
    517c:	e59f013c 	ldr	r0, [pc, #316]	; 52c0 <do_tt_sn+0x208>
    5180:	e1a0e00f 	mov	lr, pc
    5184:	e596f000 	ldr	pc, [r6]
    5188:	e1a00008 	mov	r0, r8
    518c:	eb0003eb 	bl	6140 <vs_device_erasable>
    5190:	e28a3001 	add	r3, sl, #1	; 0x1
    5194:	e3500000 	cmp	r0, #0	; 0x0
    5198:	e1a0a003 	mov	sl, r3
    519c:	e59f0120 	ldr	r0, [pc, #288]	; 52c4 <do_tt_sn+0x20c>
    51a0:	0a00000e 	beq	51e0 <do_tt_sn+0x128>
    51a4:	e1a0e00f 	mov	lr, pc
    51a8:	e596f000 	ldr	pc, [r6]
    51ac:	e1a02007 	mov	r2, r7
    51b0:	e59f1110 	ldr	r1, [pc, #272]	; 52c8 <do_tt_sn+0x210>
    51b4:	e1a0000d 	mov	r0, sp
    51b8:	eb0034fb 	bl	125ac <sprintf>
    51bc:	e3a01000 	mov	r1, #0	; 0x0
    51c0:	e1a0000d 	mov	r0, sp
    51c4:	eb0035c0 	bl	128cc <run_command>
    51c8:	eb003582 	bl	127d8 <tstc>
    51cc:	e3500000 	cmp	r0, #0	; 0x0
    51d0:	0a000002 	beq	51e0 <do_tt_sn+0x128>
    51d4:	eb003580 	bl	127dc <getc>
    51d8:	e3500003 	cmp	r0, #3	; 0x3
    51dc:	0a000030 	beq	52a4 <do_tt_sn+0x1ec>
    51e0:	e59f00e4 	ldr	r0, [pc, #228]	; 52cc <do_tt_sn+0x214>
    51e4:	e1a0e00f 	mov	lr, pc
    51e8:	e596f000 	ldr	pc, [r6]
    51ec:	e1a02007 	mov	r2, r7
    51f0:	e59f10d8 	ldr	r1, [pc, #216]	; 52d0 <do_tt_sn+0x218>
    51f4:	e1a0000d 	mov	r0, sp
    51f8:	eb0034eb 	bl	125ac <sprintf>
    51fc:	e3a01000 	mov	r1, #0	; 0x0
    5200:	e1a0000d 	mov	r0, sp
    5204:	eb0035b0 	bl	128cc <run_command>
    5208:	eb003572 	bl	127d8 <tstc>
    520c:	e3500000 	cmp	r0, #0	; 0x0
    5210:	0a000002 	beq	5220 <do_tt_sn+0x168>
    5214:	eb003570 	bl	127dc <getc>
    5218:	e3500003 	cmp	r0, #3	; 0x3
    521c:	0a000020 	beq	52a4 <do_tt_sn+0x1ec>
    5220:	e59f00ac 	ldr	r0, [pc, #172]	; 52d4 <do_tt_sn+0x21c>
    5224:	e1a0e00f 	mov	lr, pc
    5228:	e596f000 	ldr	pc, [r6]
    522c:	e1a02007 	mov	r2, r7
    5230:	e59f10a0 	ldr	r1, [pc, #160]	; 52d8 <do_tt_sn+0x220>
    5234:	e1a0000d 	mov	r0, sp
    5238:	eb0034db 	bl	125ac <sprintf>
    523c:	e3a01000 	mov	r1, #0	; 0x0
    5240:	e1a0000d 	mov	r0, sp
    5244:	eb0035a0 	bl	128cc <run_command>
    5248:	eb003562 	bl	127d8 <tstc>
    524c:	e3500000 	cmp	r0, #0	; 0x0
    5250:	0a000002 	beq	5260 <do_tt_sn+0x1a8>
    5254:	eb003560 	bl	127dc <getc>
    5258:	e3500003 	cmp	r0, #3	; 0x3
    525c:	0a000010 	beq	52a4 <do_tt_sn+0x1ec>
    5260:	e59f0074 	ldr	r0, [pc, #116]	; 52dc <do_tt_sn+0x224>
    5264:	e1a0e00f 	mov	lr, pc
    5268:	e596f000 	ldr	pc, [r6]
    526c:	e59f106c 	ldr	r1, [pc, #108]	; 52e0 <do_tt_sn+0x228>
    5270:	e1a02007 	mov	r2, r7
    5274:	e3a0300c 	mov	r3, #12	; 0xc
    5278:	e59f0064 	ldr	r0, [pc, #100]	; 52e4 <do_tt_sn+0x22c>
    527c:	ebfffcf5 	bl	4658 <tt_cmp_buffer>
    5280:	e59f0060 	ldr	r0, [pc, #96]	; 52e8 <do_tt_sn+0x230>
    5284:	e1a0e00f 	mov	lr, pc
    5288:	e596f000 	ldr	pc, [r6]
    528c:	eb003551 	bl	127d8 <tstc>
    5290:	e3500000 	cmp	r0, #0	; 0x0
    5294:	0affffb7 	beq	5178 <do_tt_sn+0xc0>
    5298:	eb00354f 	bl	127dc <getc>
    529c:	e3500003 	cmp	r0, #3	; 0x3
    52a0:	1affffb4 	bne	5178 <do_tt_sn+0xc0>
    52a4:	e3a00000 	mov	r0, #0	; 0x0
    52a8:	e28dd044 	add	sp, sp, #68	; 0x44
    52ac:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    52b0:	000176e8 	.word	0x000176e8
    52b4:	3c00d6cc 	.word	0x3c00d6cc
    52b8:	00016283 	.word	0x00016283
    52bc:	000176f5 	.word	0x000176f5
    52c0:	00016f62 	.word	0x00016f62
    52c4:	00017416 	.word	0x00017416
    52c8:	0001769b 	.word	0x0001769b
    52cc:	00017438 	.word	0x00017438
    52d0:	af          	.byte	0xaf
    52d1:	76          	.byte	0x76
    52d2:	0001      	.short	0x0001
    52d4:	00017463 	.word	0x00017463
    52d8:	000176cc 	.word	0x000176cc
    52dc:	0001748d 	.word	0x0001748d
    52e0:	50008000 	.word	0x50008000
    52e4:	40008000 	.word	0x40008000
    52e8:	00016bef 	.word	0x00016bef

000052ec <tt_nand_trolling>:
    52ec:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    52f0:	e24dd034 	sub	sp, sp, #52	; 0x34
    52f4:	e58d000c 	str	r0, [sp, #12]
    52f8:	e1a06001 	mov	r6, r1
    52fc:	e3a00001 	mov	r0, #1	; 0x1
    5300:	e3a01064 	mov	r1, #100	; 0x64
    5304:	e1a09002 	mov	r9, r2
    5308:	ebfffd26 	bl	47a8 <tt_rnd_get_int>
    530c:	e3500032 	cmp	r0, #50	; 0x32
    5310:	c5991020 	ldrgt	r1, [r9, #32]
    5314:	c58d1010 	strgt	r1, [sp, #16]
    5318:	ca000003 	bgt	532c <tt_nand_trolling+0x40>
    531c:	e3a00001 	mov	r0, #1	; 0x1
    5320:	e5991020 	ldr	r1, [r9, #32]
    5324:	ebfffd1f 	bl	47a8 <tt_rnd_get_int>
    5328:	e58d0010 	str	r0, [sp, #16]
    532c:	e59d200c 	ldr	r2, [sp, #12]
    5330:	e5991014 	ldr	r1, [r9, #20]
    5334:	e3520002 	cmp	r2, #2	; 0x2
    5338:	05993010 	ldreq	r3, [r9, #16]
    533c:	059d2010 	ldreq	r2, [sp, #16]
    5340:	159d3010 	ldrne	r3, [sp, #16]
    5344:	00833001 	addeq	r3, r3, r1
    5348:	00010293 	muleq	r1, r3, r2
    534c:	10010391 	mulne	r1, r1, r3
    5350:	e28d002c 	add	r0, sp, #44	; 0x2c
    5354:	e1a02001 	mov	r2, r1
    5358:	ebfffd3b 	bl	484c <tt_rnd_get_data>
    535c:	e3500000 	cmp	r0, #0	; 0x0
    5360:	e58d0014 	str	r0, [sp, #20]
    5364:	1a000004 	bne	537c <tt_nand_trolling+0x90>
    5368:	e59f0330 	ldr	r0, [pc, #816]	; 56a0 <tt_nand_trolling+0x3b4>
    536c:	e59f3330 	ldr	r3, [pc, #816]	; 56a4 <tt_nand_trolling+0x3b8>
    5370:	e1a0e00f 	mov	lr, pc
    5374:	e593f000 	ldr	pc, [r3]
    5378:	ea000028 	b	5420 <tt_nand_trolling+0x134>
    537c:	e5991024 	ldr	r1, [r9, #36]
    5380:	e3a00000 	mov	r0, #0	; 0x0
    5384:	ebfffd07 	bl	47a8 <tt_rnd_get_int>
    5388:	e5995020 	ldr	r5, [r9, #32]
    538c:	e59d2010 	ldr	r2, [sp, #16]
    5390:	e1a04000 	mov	r4, r0
    5394:	e0621005 	rsb	r1, r2, r5
    5398:	e3a00000 	mov	r0, #0	; 0x0
    539c:	ebfffd01 	bl	47a8 <tt_rnd_get_int>
    53a0:	e59d300c 	ldr	r3, [sp, #12]
    53a4:	e0240495 	mla	r4, r5, r4, r0
    53a8:	e3530000 	cmp	r3, #0	; 0x0
    53ac:	05963014 	ldreq	r3, [r6, #20]
    53b0:	15993014 	ldrne	r3, [r9, #20]
    53b4:	e1a05fc4 	asr	r5, r4, #31
    53b8:	e0876394 	umull	r6, r7, r4, r3
    53bc:	e3a01000 	mov	r1, #0	; 0x0
    53c0:	e3a02010 	mov	r2, #16	; 0x10
    53c4:	e28d001c 	add	r0, sp, #28	; 0x1c
    53c8:	e0277593 	mla	r7, r3, r5, r7
    53cc:	ebfff058 	bl	1534 <memset>
    53d0:	e59f32d0 	ldr	r3, [pc, #720]	; 56a8 <tt_nand_trolling+0x3bc>
    53d4:	e59d200c 	ldr	r2, [sp, #12]
    53d8:	e58d301c 	str	r3, [sp, #28]
    53dc:	e59f32c8 	ldr	r3, [pc, #712]	; 56ac <tt_nand_trolling+0x3c0>
    53e0:	e28d1030 	add	r1, sp, #48	; 0x30
    53e4:	e58d3020 	str	r3, [sp, #32]
    53e8:	e59f32c0 	ldr	r3, [pc, #704]	; 56b0 <tt_nand_trolling+0x3c4>
    53ec:	e59f82b0 	ldr	r8, [pc, #688]	; 56a4 <tt_nand_trolling+0x3b8>
    53f0:	e58d3024 	str	r3, [sp, #36]
    53f4:	e0813102 	add	r3, r1, r2, lsl #2
    53f8:	e513b014 	ldr	fp, [r3, #-20]
    53fc:	e3a01001 	mov	r1, #1	; 0x1
    5400:	e1a0000b 	mov	r0, fp
    5404:	eb000452 	bl	6554 <vs_assign_by_name>
    5408:	e3500000 	cmp	r0, #0	; 0x0
    540c:	0a000005 	beq	5428 <tt_nand_trolling+0x13c>
    5410:	e1a0100b 	mov	r1, fp
    5414:	e59f0298 	ldr	r0, [pc, #664]	; 56b4 <tt_nand_trolling+0x3c8>
    5418:	e1a0e00f 	mov	lr, pc
    541c:	e598f000 	ldr	pc, [r8]
    5420:	e3e00000 	mvn	r0, #0	; 0x0
    5424:	ea00009b 	b	5698 <tt_nand_trolling+0x3ac>
    5428:	e59d2014 	ldr	r2, [sp, #20]
    542c:	e59d302c 	ldr	r3, [sp, #44]
    5430:	e59f0280 	ldr	r0, [pc, #640]	; 56b8 <tt_nand_trolling+0x3cc>
    5434:	e1a0100b 	mov	r1, fp
    5438:	e88d00c0 	stm	sp, {r6, r7}
    543c:	e1a0e00f 	mov	lr, pc
    5440:	e598f000 	ldr	pc, [r8]
    5444:	e1a00006 	mov	r0, r6
    5448:	e1a01007 	mov	r1, r7
    544c:	eb0002a1 	bl	5ed8 <vs_isbad>
    5450:	e250a000 	subs	sl, r0, #0	; 0x0
    5454:	0a000004 	beq	546c <tt_nand_trolling+0x180>
    5458:	e59f025c 	ldr	r0, [pc, #604]	; 56bc <tt_nand_trolling+0x3d0>
    545c:	e1a0e00f 	mov	lr, pc
    5460:	e598f000 	ldr	pc, [r8]
    5464:	e3e0000a 	mvn	r0, #10	; 0xa
    5468:	ea00008a 	b	5698 <tt_nand_trolling+0x3ac>
    546c:	e1a01007 	mov	r1, r7
    5470:	e59d202c 	ldr	r2, [sp, #44]
    5474:	e1a00006 	mov	r0, r6
    5478:	eb000287 	bl	5e9c <vs_erase>
    547c:	e1a0000a 	mov	r0, sl
    5480:	eb002179 	bl	da6c <get_timer>
    5484:	e59d102c 	ldr	r1, [sp, #44]
    5488:	e1a02006 	mov	r2, r6
    548c:	e1a03007 	mov	r3, r7
    5490:	e1a04000 	mov	r4, r0
    5494:	e59d0014 	ldr	r0, [sp, #20]
    5498:	e88d0402 	stm	sp, {r1, sl}
    549c:	eb000268 	bl	5e44 <vs_write>
    54a0:	e1a00004 	mov	r0, r4
    54a4:	eb002170 	bl	da6c <get_timer>
    54a8:	e1a01000 	mov	r1, r0
    54ac:	e59d002c 	ldr	r0, [sp, #44]
    54b0:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    54b4:	e1a00540 	asr	r0, r0, #10
    54b8:	e0000095 	mul	r0, r5, r0
    54bc:	eb003cb2 	bl	1478c <__aeabi_uidiv>
    54c0:	e58d0008 	str	r0, [sp, #8]
    54c4:	e1a0000a 	mov	r0, sl
    54c8:	eb002167 	bl	da6c <get_timer>
    54cc:	e59d102c 	ldr	r1, [sp, #44]
    54d0:	e1a02006 	mov	r2, r6
    54d4:	e1a03007 	mov	r3, r7
    54d8:	e1a04000 	mov	r4, r0
    54dc:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    54e0:	e88d0402 	stm	sp, {r1, sl}
    54e4:	eb000356 	bl	6244 <vs_read>
    54e8:	e1a00004 	mov	r0, r4
    54ec:	eb00215e 	bl	da6c <get_timer>
    54f0:	e59d402c 	ldr	r4, [sp, #44]
    54f4:	e1a01000 	mov	r1, r0
    54f8:	e1a00544 	asr	r0, r4, #10
    54fc:	e0000095 	mul	r0, r5, r0
    5500:	eb003ca1 	bl	1478c <__aeabi_uidiv>
    5504:	e1a02004 	mov	r2, r4
    5508:	e1a05000 	mov	r5, r0
    550c:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    5510:	e59d0014 	ldr	r0, [sp, #20]
    5514:	e3a0300c 	mov	r3, #12	; 0xc
    5518:	ebfffc4e 	bl	4658 <tt_cmp_buffer>
    551c:	e3500000 	cmp	r0, #0	; 0x0
    5520:	0a000007 	beq	5544 <tt_nand_trolling+0x258>
    5524:	e58d0000 	str	r0, [sp]
    5528:	e1a0100b 	mov	r1, fp
    552c:	e59d2008 	ldr	r2, [sp, #8]
    5530:	e1a03005 	mov	r3, r5
    5534:	e59f0184 	ldr	r0, [pc, #388]	; 56c0 <tt_nand_trolling+0x3d4>
    5538:	e1a0e00f 	mov	lr, pc
    553c:	e598f000 	ldr	pc, [r8]
    5540:	ea000005 	b	555c <tt_nand_trolling+0x270>
    5544:	e1a0100b 	mov	r1, fp
    5548:	e59d2008 	ldr	r2, [sp, #8]
    554c:	e1a03005 	mov	r3, r5
    5550:	e59f016c 	ldr	r0, [pc, #364]	; 56c4 <tt_nand_trolling+0x3d8>
    5554:	e1a0e00f 	mov	lr, pc
    5558:	e598f000 	ldr	pc, [r8]
    555c:	e59d102c 	ldr	r1, [sp, #44]
    5560:	e59f0160 	ldr	r0, [pc, #352]	; 56c8 <tt_nand_trolling+0x3dc>
    5564:	e890000c 	ldm	r0, {r2, r3}
    5568:	e0922001 	adds	r2, r2, r1
    556c:	e0a33fc1 	adc	r3, r3, r1, asr #31
    5570:	e59d100c 	ldr	r1, [sp, #12]
    5574:	e880000c 	stm	r0, {r2, r3}
    5578:	e3510000 	cmp	r1, #0	; 0x0
    557c:	0a000040 	beq	5684 <tt_nand_trolling+0x398>
    5580:	e2618003 	rsb	r8, r1, #3	; 0x3
    5584:	e28d2030 	add	r2, sp, #48	; 0x30
    5588:	e0823108 	add	r3, r2, r8, lsl #2
    558c:	e513a014 	ldr	sl, [r3, #-20]
    5590:	e3a01001 	mov	r1, #1	; 0x1
    5594:	e1a0000a 	mov	r0, sl
    5598:	eb0003ed 	bl	6554 <vs_assign_by_name>
    559c:	e3500000 	cmp	r0, #0	; 0x0
    55a0:	0a000005 	beq	55bc <tt_nand_trolling+0x2d0>
    55a4:	e1a0100a 	mov	r1, sl
    55a8:	e59f0104 	ldr	r0, [pc, #260]	; 56b4 <tt_nand_trolling+0x3c8>
    55ac:	e59f30f0 	ldr	r3, [pc, #240]	; 56a4 <tt_nand_trolling+0x3b8>
    55b0:	e1a0e00f 	mov	lr, pc
    55b4:	e593f000 	ldr	pc, [r3]
    55b8:	eaffff98 	b	5420 <tt_nand_trolling+0x134>
    55bc:	e3580001 	cmp	r8, #1	; 0x1
    55c0:	15990010 	ldrne	r0, [r9, #16]
    55c4:	e5993014 	ldr	r3, [r9, #20]
    55c8:	e59d1010 	ldr	r1, [sp, #16]
    55cc:	e0803003 	add	r3, r0, r3
    55d0:	e0030391 	mul	r3, r1, r3
    55d4:	e3a00000 	mov	r0, #0	; 0x0
    55d8:	e58d302c 	str	r3, [sp, #44]
    55dc:	eb002122 	bl	da6c <get_timer>
    55e0:	e59d102c 	ldr	r1, [sp, #44]
    55e4:	e1a02006 	mov	r2, r6
    55e8:	e1a04000 	mov	r4, r0
    55ec:	e1a03007 	mov	r3, r7
    55f0:	e58d1000 	str	r1, [sp]
    55f4:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    55f8:	e3a01000 	mov	r1, #0	; 0x0
    55fc:	e58d1004 	str	r1, [sp, #4]
    5600:	eb00030f 	bl	6244 <vs_read>
    5604:	e1a00004 	mov	r0, r4
    5608:	eb002117 	bl	da6c <get_timer>
    560c:	e59d402c 	ldr	r4, [sp, #44]
    5610:	e1a01000 	mov	r1, r0
    5614:	e1a03544 	asr	r3, r4, #10
    5618:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    561c:	e0000390 	mul	r0, r0, r3
    5620:	eb003c59 	bl	1478c <__aeabi_uidiv>
    5624:	e3580002 	cmp	r8, #2	; 0x2
    5628:	13a03000 	movne	r3, #0	; 0x0
    562c:	03a03001 	moveq	r3, #1	; 0x1
    5630:	e1a05000 	mov	r5, r0
    5634:	e1a02004 	mov	r2, r4
    5638:	e59d0014 	ldr	r0, [sp, #20]
    563c:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    5640:	e58d9000 	str	r9, [sp]
    5644:	ebfffdfa 	bl	4e34 <tt_nand_cross_cmp>
    5648:	e3500000 	cmp	r0, #0	; 0x0
    564c:	e59f8050 	ldr	r8, [pc, #80]	; 56a4 <tt_nand_trolling+0x3b8>
    5650:	0a000006 	beq	5670 <tt_nand_trolling+0x384>
    5654:	e1a03000 	mov	r3, r0
    5658:	e1a0100a 	mov	r1, sl
    565c:	e1a02005 	mov	r2, r5
    5660:	e59f0064 	ldr	r0, [pc, #100]	; 56cc <tt_nand_trolling+0x3e0>
    5664:	e1a0e00f 	mov	lr, pc
    5668:	e598f000 	ldr	pc, [r8]
    566c:	ea000004 	b	5684 <tt_nand_trolling+0x398>
    5670:	e1a0100a 	mov	r1, sl
    5674:	e1a02005 	mov	r2, r5
    5678:	e59f0050 	ldr	r0, [pc, #80]	; 56d0 <tt_nand_trolling+0x3e4>
    567c:	e1a0e00f 	mov	lr, pc
    5680:	e598f000 	ldr	pc, [r8]
    5684:	e59f0048 	ldr	r0, [pc, #72]	; 56d4 <tt_nand_trolling+0x3e8>
    5688:	e59f3014 	ldr	r3, [pc, #20]	; 56a4 <tt_nand_trolling+0x3b8>
    568c:	e1a0e00f 	mov	lr, pc
    5690:	e593f000 	ldr	pc, [r3]
    5694:	e3a00000 	mov	r0, #0	; 0x0
    5698:	e28dd034 	add	sp, sp, #52	; 0x34
    569c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    56a0:	00017713 	.word	0x00017713
    56a4:	3c00d6cc 	.word	0x3c00d6cc
    56a8:	00016127 	.word	0x00016127
    56ac:	0001627f 	.word	0x0001627f
    56b0:	00016283 	.word	0x00016283
    56b4:	00017732 	.word	0x00017732
    56b8:	0001774f 	.word	0x0001774f
    56bc:	0001777f 	.word	0x0001777f
    56c0:	000177a7 	.word	0x000177a7
    56c4:	000177e0 	.word	0x000177e0
    56c8:	3c00d158 	.word	0x3c00d158
    56cc:	0001780c 	.word	0x0001780c
    56d0:	00017839 	.word	0x00017839
    56d4:	00016bef 	.word	0x00016bef

000056d8 <do_tt_ndloop>:
    56d8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    56dc:	e3a00001 	mov	r0, #1	; 0x1
    56e0:	e24dd004 	sub	sp, sp, #4	; 0x4
    56e4:	eb001212 	bl	9f34 <nand_get_config>
    56e8:	e2505000 	subs	r5, r0, #0	; 0x0
    56ec:	1a000005 	bne	5708 <do_tt_ndloop+0x30>
    56f0:	e59f00d0 	ldr	r0, [pc, #208]	; 57c8 <do_tt_ndloop+0xf0>
    56f4:	e59f30d0 	ldr	r3, [pc, #208]	; 57cc <do_tt_ndloop+0xf4>
    56f8:	e1a0e00f 	mov	lr, pc
    56fc:	e593f000 	ldr	pc, [r3]
    5700:	e3e00001 	mvn	r0, #1	; 0x1
    5704:	ea00002d 	b	57c0 <do_tt_ndloop+0xe8>
    5708:	e3a00000 	mov	r0, #0	; 0x0
    570c:	eb0020d6 	bl	da6c <get_timer>
    5710:	e59f10b8 	ldr	r1, [pc, #184]	; 57d0 <do_tt_ndloop+0xf8>
    5714:	e3a02000 	mov	r2, #0	; 0x0
    5718:	e3a03000 	mov	r3, #0	; 0x0
    571c:	e881000c 	stm	r1, {r2, r3}
    5720:	e59f70a4 	ldr	r7, [pc, #164]	; 57cc <do_tt_ndloop+0xf4>
    5724:	e1a06000 	mov	r6, r0
    5728:	e3a04000 	mov	r4, #0	; 0x0
    572c:	e1a01004 	mov	r1, r4
    5730:	e59f009c 	ldr	r0, [pc, #156]	; 57d4 <do_tt_ndloop+0xfc>
    5734:	e1a0e00f 	mov	lr, pc
    5738:	e597f000 	ldr	pc, [r7]
    573c:	e3a01000 	mov	r1, #0	; 0x0
    5740:	e1a02005 	mov	r2, r5
    5744:	e3a00001 	mov	r0, #1	; 0x1
    5748:	ebfffee7 	bl	52ec <tt_nand_trolling>
    574c:	e3500000 	cmp	r0, #0	; 0x0
    5750:	e2844001 	add	r4, r4, #1	; 0x1
    5754:	1a000003 	bne	5768 <do_tt_ndloop+0x90>
    5758:	e1a01000 	mov	r1, r0
    575c:	e1a02005 	mov	r2, r5
    5760:	e3a00002 	mov	r0, #2	; 0x2
    5764:	ebfffee0 	bl	52ec <tt_nand_trolling>
    5768:	eb00341a 	bl	127d8 <tstc>
    576c:	e3500000 	cmp	r0, #0	; 0x0
    5770:	0affffed 	beq	572c <do_tt_ndloop+0x54>
    5774:	eb003418 	bl	127dc <getc>
    5778:	e3500003 	cmp	r0, #3	; 0x3
    577c:	1affffea 	bne	572c <do_tt_ndloop+0x54>
    5780:	e1a00006 	mov	r0, r6
    5784:	eb0020b8 	bl	da6c <get_timer>
    5788:	e59f3040 	ldr	r3, [pc, #64]	; 57d0 <do_tt_ndloop+0xf8>
    578c:	e30e1a60 	movw	r1, #60000	; 0xea60
    5790:	e5934000 	ldr	r4, [r3]
    5794:	e5933004 	ldr	r3, [r3, #4]
    5798:	e1a04a24 	lsr	r4, r4, #20
    579c:	e1844603 	orr	r4, r4, r3, lsl #12
    57a0:	eb003c7b 	bl	14994 <__aeabi_idiv>
    57a4:	e1a01004 	mov	r1, r4
    57a8:	e1a02000 	mov	r2, r0
    57ac:	e59f3018 	ldr	r3, [pc, #24]	; 57cc <do_tt_ndloop+0xf4>
    57b0:	e59f0020 	ldr	r0, [pc, #32]	; 57d8 <do_tt_ndloop+0x100>
    57b4:	e1a0e00f 	mov	lr, pc
    57b8:	e593f000 	ldr	pc, [r3]
    57bc:	e3a00000 	mov	r0, #0	; 0x0
    57c0:	e28dd004 	add	sp, sp, #4	; 0x4
    57c4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    57c8:	00017859 	.word	0x00017859
    57cc:	3c00d6cc 	.word	0x3c00d6cc
    57d0:	3c00d158 	.word	0x3c00d158
    57d4:	00016f62 	.word	0x00016f62
    57d8:	00016f6c 	.word	0x00016f6c

000057dc <do_tt_nand>:
    57dc:	e3520001 	cmp	r2, #1	; 0x1
    57e0:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    57e4:	e1a08002 	mov	r8, r2
    57e8:	e1a0a003 	mov	sl, r3
    57ec:	c59f90dc 	ldrgt	r9, [pc, #220]	; 58d0 <do_tt_nand+0xf4>
    57f0:	c3a06001 	movgt	r6, #1	; 0x1
    57f4:	ca000005 	bgt	5810 <do_tt_nand+0x34>
    57f8:	e59f00d4 	ldr	r0, [pc, #212]	; 58d4 <do_tt_nand+0xf8>
    57fc:	e59f30cc 	ldr	r3, [pc, #204]	; 58d0 <do_tt_nand+0xf4>
    5800:	e1a0e00f 	mov	lr, pc
    5804:	e593f000 	ldr	pc, [r3]
    5808:	e3e00000 	mvn	r0, #0	; 0x0
    580c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    5810:	e3a01000 	mov	r1, #0	; 0x0
    5814:	e3a0200a 	mov	r2, #10	; 0xa
    5818:	e79a0106 	ldr	r0, [sl, r6, lsl #2]
    581c:	eb002fd6 	bl	1177c <simple_strtoul>
    5820:	e1a07000 	mov	r7, r0
    5824:	ebffeb6c 	bl	5dc <set_xom>
    5828:	e3a00000 	mov	r0, #0	; 0x0
    582c:	eb0017c3 	bl	b740 <nand_rescan_config>
    5830:	e3a00000 	mov	r0, #0	; 0x0
    5834:	eb0011be 	bl	9f34 <nand_get_config>
    5838:	e1a05000 	mov	r5, r0
    583c:	e3a00001 	mov	r0, #1	; 0x1
    5840:	eb0011bb 	bl	9f34 <nand_get_config>
    5844:	e3500000 	cmp	r0, #0	; 0x0
    5848:	13550000 	cmpne	r5, #0	; 0x0
    584c:	e1a04000 	mov	r4, r0
    5850:	1a000005 	bne	586c <do_tt_nand+0x90>
    5854:	e59f007c 	ldr	r0, [pc, #124]	; 58d8 <do_tt_nand+0xfc>
    5858:	e59f3070 	ldr	r3, [pc, #112]	; 58d0 <do_tt_nand+0xf4>
    585c:	e1a0e00f 	mov	lr, pc
    5860:	e593f000 	ldr	pc, [r3]
    5864:	e3e00001 	mvn	r0, #1	; 0x1
    5868:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    586c:	e1a01007 	mov	r1, r7
    5870:	e59f0064 	ldr	r0, [pc, #100]	; 58dc <do_tt_nand+0x100>
    5874:	e1a0e00f 	mov	lr, pc
    5878:	e599f000 	ldr	pc, [r9]
    587c:	e3a00000 	mov	r0, #0	; 0x0
    5880:	e1a01005 	mov	r1, r5
    5884:	e1a02004 	mov	r2, r4
    5888:	ebfffe97 	bl	52ec <tt_nand_trolling>
    588c:	e370000b 	cmn	r0, #11	; 0xb
    5890:	0afffff9 	beq	587c <do_tt_nand+0xa0>
    5894:	e3a00001 	mov	r0, #1	; 0x1
    5898:	e1a01005 	mov	r1, r5
    589c:	e1a02004 	mov	r2, r4
    58a0:	ebfffe91 	bl	52ec <tt_nand_trolling>
    58a4:	e370000b 	cmn	r0, #11	; 0xb
    58a8:	0afffff9 	beq	5894 <do_tt_nand+0xb8>
    58ac:	e1a01005 	mov	r1, r5
    58b0:	e1a02004 	mov	r2, r4
    58b4:	e2866001 	add	r6, r6, #1	; 0x1
    58b8:	e3a00002 	mov	r0, #2	; 0x2
    58bc:	ebfffe8a 	bl	52ec <tt_nand_trolling>
    58c0:	e1560008 	cmp	r6, r8
    58c4:	baffffd1 	blt	5810 <do_tt_nand+0x34>
    58c8:	e3a00000 	mov	r0, #0	; 0x0
    58cc:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    58d0:	3c00d6cc 	.word	0x3c00d6cc
    58d4:	0001788b 	.word	0x0001788b
    58d8:	00017859 	.word	0x00017859
    58dc:	000178c1 	.word	0x000178c1

000058e0 <tt_wire_get_opt>:
    58e0:	e5900008 	ldr	r0, [r0, #8]
    58e4:	e12fff1e 	bx	lr

000058e8 <tt_wire_get_len>:
    58e8:	e590000c 	ldr	r0, [r0, #12]
    58ec:	e12fff1e 	bx	lr

000058f0 <tt_wire_make_data>:
    58f0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    58f4:	e24dd004 	sub	sp, sp, #4	; 0x4
    58f8:	e1a04000 	mov	r4, r0
    58fc:	e1a07002 	mov	r7, r2
    5900:	e1a06003 	mov	r6, r3
    5904:	ebffef12 	bl	1554 <memcpy>
    5908:	e59f3040 	ldr	r3, [pc, #64]	; 5950 <tt_wire_make_data+0x60>
    590c:	e5846008 	str	r6, [r4, #8]
    5910:	e5843000 	str	r3, [r4]
    5914:	e59d3018 	ldr	r3, [sp, #24]
    5918:	e3a05000 	mov	r5, #0	; 0x0
    591c:	e584300c 	str	r3, [r4, #12]
    5920:	e1a01007 	mov	r1, r7
    5924:	e5845004 	str	r5, [r4, #4]
    5928:	e1a00005 	mov	r0, r5
    592c:	eb000469 	bl	6ad8 <hash_init>
    5930:	e1a01007 	mov	r1, r7
    5934:	e1a00004 	mov	r0, r4
    5938:	eb000441 	bl	6a44 <hash_data>
    593c:	e2840004 	add	r0, r4, #4	; 0x4
    5940:	eb000423 	bl	69d4 <hash_value>
    5944:	e1a00005 	mov	r0, r5
    5948:	e28dd004 	add	sp, sp, #4	; 0x4
    594c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5950:	7766dd80 	.word	0x7766dd80

00005954 <tt_wire_check_valid>:
    5954:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5958:	e5903000 	ldr	r3, [r0]
    595c:	e59f2084 	ldr	r2, [pc, #132]	; 59e8 <tt_wire_check_valid+0x94>
    5960:	e24dd00c 	sub	sp, sp, #12	; 0xc
    5964:	e1530002 	cmp	r3, r2
    5968:	e1a04000 	mov	r4, r0
    596c:	e1a06001 	mov	r6, r1
    5970:	e5907004 	ldr	r7, [r0, #4]
    5974:	0a000006 	beq	5994 <tt_wire_check_valid+0x40>
    5978:	e1a01003 	mov	r1, r3
    597c:	e59f0068 	ldr	r0, [pc, #104]	; 59ec <tt_wire_check_valid+0x98>
    5980:	e59f3068 	ldr	r3, [pc, #104]	; 59f0 <tt_wire_check_valid+0x9c>
    5984:	e1a0e00f 	mov	lr, pc
    5988:	e593f000 	ldr	pc, [r3]
    598c:	e3a00000 	mov	r0, #0	; 0x0
    5990:	ea000012 	b	59e0 <tt_wire_check_valid+0x8c>
    5994:	e3a05000 	mov	r5, #0	; 0x0
    5998:	e5805004 	str	r5, [r0, #4]
    599c:	e1a00005 	mov	r0, r5
    59a0:	eb00044c 	bl	6ad8 <hash_init>
    59a4:	e1a01006 	mov	r1, r6
    59a8:	e1a00004 	mov	r0, r4
    59ac:	eb000424 	bl	6a44 <hash_data>
    59b0:	e28d0004 	add	r0, sp, #4	; 0x4
    59b4:	eb000406 	bl	69d4 <hash_value>
    59b8:	e59d1004 	ldr	r1, [sp, #4]
    59bc:	e1510007 	cmp	r1, r7
    59c0:	03a00001 	moveq	r0, #1	; 0x1
    59c4:	0a000005 	beq	59e0 <tt_wire_check_valid+0x8c>
    59c8:	e59f0024 	ldr	r0, [pc, #36]	; 59f4 <tt_wire_check_valid+0xa0>
    59cc:	e1a02007 	mov	r2, r7
    59d0:	e59f3018 	ldr	r3, [pc, #24]	; 59f0 <tt_wire_check_valid+0x9c>
    59d4:	e1a0e00f 	mov	lr, pc
    59d8:	e593f000 	ldr	pc, [r3]
    59dc:	e1a00005 	mov	r0, r5
    59e0:	e28dd00c 	add	sp, sp, #12	; 0xc
    59e4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    59e8:	7766dd80 	.word	0x7766dd80
    59ec:	0001793b 	.word	0x0001793b
    59f0:	3c00d6cc 	.word	0x3c00d6cc
    59f4:	00017964 	.word	0x00017964

000059f8 <do_tt_wire>:
    59f8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    59fc:	e3520001 	cmp	r2, #1	; 0x1
    5a00:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    5a04:	e1a04003 	mov	r4, r3
    5a08:	d59f032c 	ldrle	r0, [pc, #812]	; 5d3c <do_tt_wire+0x344>
    5a0c:	da00002b 	ble	5ac0 <do_tt_wire+0xc8>
    5a10:	e3a02003 	mov	r2, #3	; 0x3
    5a14:	e5930004 	ldr	r0, [r3, #4]
    5a18:	e59f1320 	ldr	r1, [pc, #800]	; 5d40 <do_tt_wire+0x348>
    5a1c:	ebffee93 	bl	1470 <strncmp>
    5a20:	e3500000 	cmp	r0, #0	; 0x0
    5a24:	059f2318 	ldreq	r2, [pc, #792]	; 5d44 <do_tt_wire+0x34c>
    5a28:	0a000006 	beq	5a48 <do_tt_wire+0x50>
    5a2c:	e5940004 	ldr	r0, [r4, #4]
    5a30:	e59f1310 	ldr	r1, [pc, #784]	; 5d48 <do_tt_wire+0x350>
    5a34:	e3a02003 	mov	r2, #3	; 0x3
    5a38:	ebffee8c 	bl	1470 <strncmp>
    5a3c:	e3500000 	cmp	r0, #0	; 0x0
    5a40:	1a000009 	bne	5a6c <do_tt_wire+0x74>
    5a44:	e59f2300 	ldr	r2, [pc, #768]	; 5d4c <do_tt_wire+0x354>
    5a48:	e59f3300 	ldr	r3, [pc, #768]	; 5d50 <do_tt_wire+0x358>
    5a4c:	e3a00000 	mov	r0, #0	; 0x0
    5a50:	e5832000 	str	r2, [r3]
    5a54:	eb002004 	bl	da6c <get_timer>
    5a58:	e59f82f4 	ldr	r8, [pc, #756]	; 5d54 <do_tt_wire+0x35c>
    5a5c:	e59f92ec 	ldr	r9, [pc, #748]	; 5d50 <do_tt_wire+0x358>
    5a60:	e1a0a008 	mov	sl, r8
    5a64:	e58d000c 	str	r0, [sp, #12]
    5a68:	ea000006 	b	5a88 <do_tt_wire+0x90>
    5a6c:	e5941004 	ldr	r1, [r4, #4]
    5a70:	e59f02e0 	ldr	r0, [pc, #736]	; 5d58 <do_tt_wire+0x360>
    5a74:	e59f32d8 	ldr	r3, [pc, #728]	; 5d54 <do_tt_wire+0x35c>
    5a78:	e1a0e00f 	mov	lr, pc
    5a7c:	e593f000 	ldr	pc, [r3]
    5a80:	e3e00000 	mvn	r0, #0	; 0x0
    5a84:	ea0000aa 	b	5d34 <do_tt_wire+0x33c>
    5a88:	e59f02cc 	ldr	r0, [pc, #716]	; 5d5c <do_tt_wire+0x364>
    5a8c:	eb001fd2 	bl	d9dc <udelay>
    5a90:	e59f02c8 	ldr	r0, [pc, #712]	; 5d60 <do_tt_wire+0x368>
    5a94:	e1a0e00f 	mov	lr, pc
    5a98:	e598f000 	ldr	pc, [r8]
    5a9c:	e5993000 	ldr	r3, [r9]
    5aa0:	e1a0e00f 	mov	lr, pc
    5aa4:	e593f000 	ldr	pc, [r3]
    5aa8:	e3500000 	cmp	r0, #0	; 0x0
    5aac:	a3a07000 	movge	r7, #0	; 0x0
    5ab0:	a3a06020 	movge	r6, #32	; 0x20
    5ab4:	a59fb2a8 	ldrge	fp, [pc, #680]	; 5d64 <do_tt_wire+0x36c>
    5ab8:	aa000004 	bge	5ad0 <do_tt_wire+0xd8>
    5abc:	e59f02a4 	ldr	r0, [pc, #676]	; 5d68 <do_tt_wire+0x370>
    5ac0:	e59f328c 	ldr	r3, [pc, #652]	; 5d54 <do_tt_wire+0x35c>
    5ac4:	e1a0e00f 	mov	lr, pc
    5ac8:	e593f000 	ldr	pc, [r3]
    5acc:	eaffffeb 	b	5a80 <do_tt_wire+0x88>
    5ad0:	e3570000 	cmp	r7, #0	; 0x0
    5ad4:	e1a01006 	mov	r1, r6
    5ad8:	e59f028c 	ldr	r0, [pc, #652]	; 5d6c <do_tt_wire+0x374>
    5adc:	1a00002f 	bne	5ba0 <do_tt_wire+0x1a8>
    5ae0:	e59f0288 	ldr	r0, [pc, #648]	; 5d70 <do_tt_wire+0x378>
    5ae4:	e1a0e00f 	mov	lr, pc
    5ae8:	e598f000 	ldr	pc, [r8]
    5aec:	e1a00007 	mov	r0, r7
    5af0:	eb001fdd 	bl	da6c <get_timer>
    5af4:	e1a01006 	mov	r1, r6
    5af8:	e1a04000 	mov	r4, r0
    5afc:	e5993000 	ldr	r3, [r9]
    5b00:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5b04:	e1a0e00f 	mov	lr, pc
    5b08:	e593f004 	ldr	pc, [r3, #4]
    5b0c:	e1a05000 	mov	r5, r0
    5b10:	e1a00004 	mov	r0, r4
    5b14:	eb001fd4 	bl	da6c <get_timer>
    5b18:	e3550000 	cmp	r5, #0	; 0x0
    5b1c:	e1a04000 	mov	r4, r0
    5b20:	e1a01006 	mov	r1, r6
    5b24:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5b28:	ba00003d 	blt	5c24 <do_tt_wire+0x22c>
    5b2c:	ebffff88 	bl	5954 <tt_wire_check_valid>
    5b30:	e3500000 	cmp	r0, #0	; 0x0
    5b34:	e1a01004 	mov	r1, r4
    5b38:	e1a03546 	asr	r3, r6, #10
    5b3c:	1a000007 	bne	5b60 <do_tt_wire+0x168>
    5b40:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    5b44:	e0000390 	mul	r0, r0, r3
    5b48:	eb003b91 	bl	14994 <__aeabi_idiv>
    5b4c:	e1a01000 	mov	r1, r0
    5b50:	e59f021c 	ldr	r0, [pc, #540]	; 5d74 <do_tt_wire+0x37c>
    5b54:	e1a0e00f 	mov	lr, pc
    5b58:	e59af000 	ldr	pc, [sl]
    5b5c:	eaffffc9 	b	5a88 <do_tt_wire+0x90>
    5b60:	e59fc210 	ldr	ip, [pc, #528]	; 5d78 <do_tt_wire+0x380>
    5b64:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    5b68:	e0000392 	mul	r0, r2, r3
    5b6c:	e89c000c 	ldm	ip, {r2, r3}
    5b70:	e0922006 	adds	r2, r2, r6
    5b74:	e0a33fc6 	adc	r3, r3, r6, asr #31
    5b78:	e88c000c 	stm	ip, {r2, r3}
    5b7c:	eb003b84 	bl	14994 <__aeabi_idiv>
    5b80:	e1a01000 	mov	r1, r0
    5b84:	e59f01f0 	ldr	r0, [pc, #496]	; 5d7c <do_tt_wire+0x384>
    5b88:	e1a0e00f 	mov	lr, pc
    5b8c:	e598f000 	ldr	pc, [r8]
    5b90:	e3a0348e 	mov	r3, #-1912602624	; 0x8e000000
    5b94:	e5937008 	ldr	r7, [r3, #8]
    5b98:	e593600c 	ldr	r6, [r3, #12]
    5b9c:	ea000044 	b	5cb4 <do_tt_wire+0x2bc>
    5ba0:	e3570001 	cmp	r7, #1	; 0x1
    5ba4:	1a000030 	bne	5c6c <do_tt_wire+0x274>
    5ba8:	e1a0e00f 	mov	lr, pc
    5bac:	e598f000 	ldr	pc, [r8]
    5bb0:	e1a01007 	mov	r1, r7
    5bb4:	e3a00000 	mov	r0, #0	; 0x0
    5bb8:	ebfffafa 	bl	47a8 <tt_rnd_get_int>
    5bbc:	e3a01020 	mov	r1, #32	; 0x20
    5bc0:	e1a07000 	mov	r7, r0
    5bc4:	e3a02502 	mov	r2, #8388608	; 0x800000
    5bc8:	e28d0014 	add	r0, sp, #20	; 0x14
    5bcc:	ebfffb1e 	bl	484c <tt_rnd_get_data>
    5bd0:	e59dc014 	ldr	ip, [sp, #20]
    5bd4:	e1a01000 	mov	r1, r0
    5bd8:	e1a02006 	mov	r2, r6
    5bdc:	e1a03007 	mov	r3, r7
    5be0:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5be4:	e58dc000 	str	ip, [sp]
    5be8:	ebffff40 	bl	58f0 <tt_wire_make_data>
    5bec:	e3a00000 	mov	r0, #0	; 0x0
    5bf0:	eb001f9d 	bl	da6c <get_timer>
    5bf4:	e1a01006 	mov	r1, r6
    5bf8:	e1a05000 	mov	r5, r0
    5bfc:	e5993000 	ldr	r3, [r9]
    5c00:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5c04:	e1a0e00f 	mov	lr, pc
    5c08:	e593f008 	ldr	pc, [r3, #8]
    5c0c:	e1a04000 	mov	r4, r0
    5c10:	e1a00005 	mov	r0, r5
    5c14:	eb001f94 	bl	da6c <get_timer>
    5c18:	e3540000 	cmp	r4, #0	; 0x0
    5c1c:	e1a01000 	mov	r1, r0
    5c20:	aa000003 	bge	5c34 <do_tt_wire+0x23c>
    5c24:	e59f0154 	ldr	r0, [pc, #340]	; 5d80 <do_tt_wire+0x388>
    5c28:	e1a0e00f 	mov	lr, pc
    5c2c:	e59af000 	ldr	pc, [sl]
    5c30:	eaffff94 	b	5a88 <do_tt_wire+0x90>
    5c34:	e89b000c 	ldm	fp, {r2, r3}
    5c38:	e0922006 	adds	r2, r2, r6
    5c3c:	e0a33fc6 	adc	r3, r3, r6, asr #31
    5c40:	e59d6014 	ldr	r6, [sp, #20]
    5c44:	e88b000c 	stm	fp, {r2, r3}
    5c48:	e1a00546 	asr	r0, r6, #10
    5c4c:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    5c50:	e0000092 	mul	r0, r2, r0
    5c54:	eb003b4e 	bl	14994 <__aeabi_idiv>
    5c58:	e1a01000 	mov	r1, r0
    5c5c:	e59f0120 	ldr	r0, [pc, #288]	; 5d84 <do_tt_wire+0x38c>
    5c60:	e1a0e00f 	mov	lr, pc
    5c64:	e598f000 	ldr	pc, [r8]
    5c68:	ea000011 	b	5cb4 <do_tt_wire+0x2bc>
    5c6c:	e3570003 	cmp	r7, #3	; 0x3
    5c70:	1a000004 	bne	5c88 <do_tt_wire+0x290>
    5c74:	e59f010c 	ldr	r0, [pc, #268]	; 5d88 <do_tt_wire+0x390>
    5c78:	e59f30d4 	ldr	r3, [pc, #212]	; 5d54 <do_tt_wire+0x35c>
    5c7c:	e1a0e00f 	mov	lr, pc
    5c80:	e593f000 	ldr	pc, [r3]
    5c84:	ea000010 	b	5ccc <do_tt_wire+0x2d4>
    5c88:	e3570002 	cmp	r7, #2	; 0x2
    5c8c:	1a000003 	bne	5ca0 <do_tt_wire+0x2a8>
    5c90:	e59f00f4 	ldr	r0, [pc, #244]	; 5d8c <do_tt_wire+0x394>
    5c94:	e1a0e00f 	mov	lr, pc
    5c98:	e598f000 	ldr	pc, [r8]
    5c9c:	eaffff79 	b	5a88 <do_tt_wire+0x90>
    5ca0:	e59f00e8 	ldr	r0, [pc, #232]	; 5d90 <do_tt_wire+0x398>
    5ca4:	e59f30a8 	ldr	r3, [pc, #168]	; 5d54 <do_tt_wire+0x35c>
    5ca8:	e1a0e00f 	mov	lr, pc
    5cac:	e593f000 	ldr	pc, [r3]
    5cb0:	eafffffe 	b	5cb0 <do_tt_wire+0x2b8>
    5cb4:	eb0032c7 	bl	127d8 <tstc>
    5cb8:	e3500000 	cmp	r0, #0	; 0x0
    5cbc:	0affff83 	beq	5ad0 <do_tt_wire+0xd8>
    5cc0:	eb0032c5 	bl	127dc <getc>
    5cc4:	e3500003 	cmp	r0, #3	; 0x3
    5cc8:	1affff80 	bne	5ad0 <do_tt_wire+0xd8>
    5ccc:	e59d000c 	ldr	r0, [sp, #12]
    5cd0:	eb001f65 	bl	da6c <get_timer>
    5cd4:	e59f3088 	ldr	r3, [pc, #136]	; 5d64 <do_tt_wire+0x36c>
    5cd8:	e59f4074 	ldr	r4, [pc, #116]	; 5d54 <do_tt_wire+0x35c>
    5cdc:	e893000a 	ldm	r3, {r1, r3}
    5ce0:	e1a01a21 	lsr	r1, r1, #20
    5ce4:	e1811603 	orr	r1, r1, r3, lsl #12
    5ce8:	e1a05000 	mov	r5, r0
    5cec:	e59f00a0 	ldr	r0, [pc, #160]	; 5d94 <do_tt_wire+0x39c>
    5cf0:	e1a0e00f 	mov	lr, pc
    5cf4:	e594f000 	ldr	pc, [r4]
    5cf8:	e59f3078 	ldr	r3, [pc, #120]	; 5d78 <do_tt_wire+0x380>
    5cfc:	e59f0094 	ldr	r0, [pc, #148]	; 5d98 <do_tt_wire+0x3a0>
    5d00:	e893000a 	ldm	r3, {r1, r3}
    5d04:	e1a01a21 	lsr	r1, r1, #20
    5d08:	e1811603 	orr	r1, r1, r3, lsl #12
    5d0c:	e1a0e00f 	mov	lr, pc
    5d10:	e594f000 	ldr	pc, [r4]
    5d14:	e30e1a60 	movw	r1, #60000	; 0xea60
    5d18:	e1a00005 	mov	r0, r5
    5d1c:	eb003b1c 	bl	14994 <__aeabi_idiv>
    5d20:	e1a01000 	mov	r1, r0
    5d24:	e59f0070 	ldr	r0, [pc, #112]	; 5d9c <do_tt_wire+0x3a4>
    5d28:	e1a0e00f 	mov	lr, pc
    5d2c:	e594f000 	ldr	pc, [r4]
    5d30:	e3a00000 	mov	r0, #0	; 0x0
    5d34:	e28dd01c 	add	sp, sp, #28	; 0x1c
    5d38:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5d3c:	0001798d 	.word	0x0001798d
    5d40:	000179d6 	.word	0x000179d6
    5d44:	00015604 	.word	0x00015604
    5d48:	000179da 	.word	0x000179da
    5d4c:	0001561c 	.word	0x0001561c
    5d50:	3c00d170 	.word	0x3c00d170
    5d54:	3c00d6cc 	.word	0x3c00d6cc
    5d58:	000179de 	.word	0x000179de
    5d5c:	001e8480 	.word	0x001e8480
    5d60:	000179fe 	.word	0x000179fe
    5d64:	3c00d168 	.word	0x3c00d168
    5d68:	00017a20 	.word	0x00017a20
    5d6c:	00017a8d 	.word	0x00017a8d
    5d70:	00017a43 	.word	0x00017a43
    5d74:	00017a6f 	.word	0x00017a6f
    5d78:	3c00d160 	.word	0x3c00d160
    5d7c:	00017a7e 	.word	0x00017a7e
    5d80:	00017a5f 	.word	0x00017a5f
    5d84:	00017aa9 	.word	0x00017aa9
    5d88:	00017ab8 	.word	0x00017ab8
    5d8c:	00017aca 	.word	0x00017aca
    5d90:	00017ae3 	.word	0x00017ae3
    5d94:	00017b10 	.word	0x00017b10
    5d98:	00017b2a 	.word	0x00017b2a
    5d9c:	00017b48 	.word	0x00017b48

00005da0 <vs_is_assigned>:
    5da0:	e59f300c 	ldr	r3, [pc, #12]	; 5db4 <vs_is_assigned+0x14>
    5da4:	e5930000 	ldr	r0, [r3]
    5da8:	e2500000 	subs	r0, r0, #0	; 0x0
    5dac:	13a00001 	movne	r0, #1	; 0x1
    5db0:	e12fff1e 	bx	lr
    5db4:	3c00d178 	.word	0x3c00d178

00005db8 <vs_is_opt_available>:
    5db8:	e350000d 	cmp	r0, #13	; 0xd
    5dbc:	959f2078 	ldrls	r2, [pc, #120]	; 5e3c <vs_is_opt_available+0x84>
    5dc0:	93a0302c 	movls	r3, #44	; 0x2c
    5dc4:	859f3074 	ldrhi	r3, [pc, #116]	; 5e40 <vs_is_opt_available+0x88>
    5dc8:	90202093 	mlals	r0, r3, r0, r2
    5dcc:	85930000 	ldrhi	r0, [r3]
    5dd0:	e3500000 	cmp	r0, #0	; 0x0
    5dd4:	0a000016 	beq	5e34 <vs_is_opt_available+0x7c>
    5dd8:	e3510005 	cmp	r1, #5	; 0x5
    5ddc:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    5de0:	ea000013 	b	5e34 <vs_is_opt_available+0x7c>
    5de4:	00005dfc 	.word	0x00005dfc
    5de8:	00005e04 	.word	0x00005e04
    5dec:	00005e0c 	.word	0x00005e0c
    5df0:	00005e14 	.word	0x00005e14
    5df4:	00005e1c 	.word	0x00005e1c
    5df8:	00005e24 	.word	0x00005e24
    5dfc:	e590300c 	ldr	r3, [r0, #12]
    5e00:	ea000008 	b	5e28 <vs_is_opt_available+0x70>
    5e04:	e5903010 	ldr	r3, [r0, #16]
    5e08:	ea000006 	b	5e28 <vs_is_opt_available+0x70>
    5e0c:	e5903014 	ldr	r3, [r0, #20]
    5e10:	ea000004 	b	5e28 <vs_is_opt_available+0x70>
    5e14:	e5903018 	ldr	r3, [r0, #24]
    5e18:	ea000002 	b	5e28 <vs_is_opt_available+0x70>
    5e1c:	e590301c 	ldr	r3, [r0, #28]
    5e20:	ea000000 	b	5e28 <vs_is_opt_available+0x70>
    5e24:	e5903020 	ldr	r3, [r0, #32]
    5e28:	e2530000 	subs	r0, r3, #0	; 0x0
    5e2c:	13a00001 	movne	r0, #1	; 0x1
    5e30:	e12fff1e 	bx	lr
    5e34:	e3a00000 	mov	r0, #0	; 0x0
    5e38:	e12fff1e 	bx	lr
    5e3c:	0001561c 	.word	0x0001561c
    5e40:	3c00d178 	.word	0x3c00d178

00005e44 <vs_write>:
    5e44:	e92d4010 	push	{r4, lr}
    5e48:	e24dd008 	sub	sp, sp, #8	; 0x8
    5e4c:	e59d4010 	ldr	r4, [sp, #16]
    5e50:	e3540000 	cmp	r4, #0	; 0x0
    5e54:	01a00004 	moveq	r0, r4
    5e58:	0a00000c 	beq	5e90 <vs_write+0x4c>
    5e5c:	e59f1034 	ldr	r1, [pc, #52]	; 5e98 <vs_write+0x54>
    5e60:	e5911000 	ldr	r1, [r1]
    5e64:	e3510000 	cmp	r1, #0	; 0x0
    5e68:	03e00001 	mvneq	r0, #1	; 0x1
    5e6c:	0a000007 	beq	5e90 <vs_write+0x4c>
    5e70:	e591c014 	ldr	ip, [r1, #20]
    5e74:	e35c0000 	cmp	ip, #0	; 0x0
    5e78:	03e00002 	mvneq	r0, #2	; 0x2
    5e7c:	0a000003 	beq	5e90 <vs_write+0x4c>
    5e80:	e59d1014 	ldr	r1, [sp, #20]
    5e84:	e58d4000 	str	r4, [sp]
    5e88:	e58d1004 	str	r1, [sp, #4]
    5e8c:	e12fff3c 	blx	ip
    5e90:	e28dd008 	add	sp, sp, #8	; 0x8
    5e94:	e8bd8010 	pop	{r4, pc}
    5e98:	3c00d178 	.word	0x3c00d178

00005e9c <vs_erase>:
    5e9c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5ea0:	e59f302c 	ldr	r3, [pc, #44]	; 5ed4 <vs_erase+0x38>
    5ea4:	e24dd004 	sub	sp, sp, #4	; 0x4
    5ea8:	e5933000 	ldr	r3, [r3]
    5eac:	e3530000 	cmp	r3, #0	; 0x0
    5eb0:	03e00001 	mvneq	r0, #1	; 0x1
    5eb4:	0a000004 	beq	5ecc <vs_erase+0x30>
    5eb8:	e593301c 	ldr	r3, [r3, #28]
    5ebc:	e3530000 	cmp	r3, #0	; 0x0
    5ec0:	03e00002 	mvneq	r0, #2	; 0x2
    5ec4:	0a000000 	beq	5ecc <vs_erase+0x30>
    5ec8:	e12fff33 	blx	r3
    5ecc:	e28dd004 	add	sp, sp, #4	; 0x4
    5ed0:	e8bd8000 	pop	{pc}
    5ed4:	3c00d178 	.word	0x3c00d178

00005ed8 <vs_isbad>:
    5ed8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5edc:	e59f302c 	ldr	r3, [pc, #44]	; 5f10 <vs_isbad+0x38>
    5ee0:	e24dd004 	sub	sp, sp, #4	; 0x4
    5ee4:	e5933000 	ldr	r3, [r3]
    5ee8:	e3530000 	cmp	r3, #0	; 0x0
    5eec:	03e00001 	mvneq	r0, #1	; 0x1
    5ef0:	0a000004 	beq	5f08 <vs_isbad+0x30>
    5ef4:	e5933024 	ldr	r3, [r3, #36]
    5ef8:	e3530000 	cmp	r3, #0	; 0x0
    5efc:	03e00002 	mvneq	r0, #2	; 0x2
    5f00:	0a000000 	beq	5f08 <vs_isbad+0x30>
    5f04:	e12fff33 	blx	r3
    5f08:	e28dd004 	add	sp, sp, #4	; 0x4
    5f0c:	e8bd8000 	pop	{pc}
    5f10:	3c00d178 	.word	0x3c00d178

00005f14 <vs_scrub>:
    5f14:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5f18:	e59f302c 	ldr	r3, [pc, #44]	; 5f4c <vs_scrub+0x38>
    5f1c:	e24dd004 	sub	sp, sp, #4	; 0x4
    5f20:	e5933000 	ldr	r3, [r3]
    5f24:	e3530000 	cmp	r3, #0	; 0x0
    5f28:	03e00001 	mvneq	r0, #1	; 0x1
    5f2c:	0a000004 	beq	5f44 <vs_scrub+0x30>
    5f30:	e5933020 	ldr	r3, [r3, #32]
    5f34:	e3530000 	cmp	r3, #0	; 0x0
    5f38:	03e00002 	mvneq	r0, #2	; 0x2
    5f3c:	0a000000 	beq	5f44 <vs_scrub+0x30>
    5f40:	e12fff33 	blx	r3
    5f44:	e28dd004 	add	sp, sp, #4	; 0x4
    5f48:	e8bd8000 	pop	{pc}
    5f4c:	3c00d178 	.word	0x3c00d178

00005f50 <vs_special>:
    5f50:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5f54:	e59f302c 	ldr	r3, [pc, #44]	; 5f88 <vs_special+0x38>
    5f58:	e24dd004 	sub	sp, sp, #4	; 0x4
    5f5c:	e5933000 	ldr	r3, [r3]
    5f60:	e3530000 	cmp	r3, #0	; 0x0
    5f64:	03e00001 	mvneq	r0, #1	; 0x1
    5f68:	0a000004 	beq	5f80 <vs_special+0x30>
    5f6c:	e5933028 	ldr	r3, [r3, #40]
    5f70:	e3530000 	cmp	r3, #0	; 0x0
    5f74:	03e00002 	mvneq	r0, #2	; 0x2
    5f78:	0a000000 	beq	5f80 <vs_special+0x30>
    5f7c:	e12fff33 	blx	r3
    5f80:	e28dd004 	add	sp, sp, #4	; 0x4
    5f84:	e8bd8000 	pop	{pc}
    5f88:	3c00d178 	.word	0x3c00d178

00005f8c <vs_align>:
    5f8c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5f90:	e350000e 	cmp	r0, #14	; 0xe
    5f94:	059f3044 	ldreq	r3, [pc, #68]	; 5fe0 <vs_align+0x54>
    5f98:	e24dd004 	sub	sp, sp, #4	; 0x4
    5f9c:	05930000 	ldreq	r0, [r3]
    5fa0:	0a000004 	beq	5fb8 <vs_align+0x2c>
    5fa4:	e350000d 	cmp	r0, #13	; 0xd
    5fa8:	8a000009 	bhi	5fd4 <vs_align+0x48>
    5fac:	e59f2030 	ldr	r2, [pc, #48]	; 5fe4 <vs_align+0x58>
    5fb0:	e3a0302c 	mov	r3, #44	; 0x2c
    5fb4:	e0202093 	mla	r0, r3, r0, r2
    5fb8:	e3500000 	cmp	r0, #0	; 0x0
    5fbc:	0a000004 	beq	5fd4 <vs_align+0x48>
    5fc0:	e590000c 	ldr	r0, [r0, #12]
    5fc4:	e3500000 	cmp	r0, #0	; 0x0
    5fc8:	0a000001 	beq	5fd4 <vs_align+0x48>
    5fcc:	e12fff30 	blx	r0
    5fd0:	ea000000 	b	5fd8 <vs_align+0x4c>
    5fd4:	e3a00000 	mov	r0, #0	; 0x0
    5fd8:	e28dd004 	add	sp, sp, #4	; 0x4
    5fdc:	e8bd8000 	pop	{pc}
    5fe0:	3c00d178 	.word	0x3c00d178
    5fe4:	0001561c 	.word	0x0001561c

00005fe8 <vs_device_bootable>:
    5fe8:	e350000e 	cmp	r0, #14	; 0xe
    5fec:	059f3030 	ldreq	r3, [pc, #48]	; 6024 <vs_device_bootable+0x3c>
    5ff0:	05930000 	ldreq	r0, [r3]
    5ff4:	0a000004 	beq	600c <vs_device_bootable+0x24>
    5ff8:	e350000d 	cmp	r0, #13	; 0xd
    5ffc:	8a000006 	bhi	601c <vs_device_bootable+0x34>
    6000:	e59f2020 	ldr	r2, [pc, #32]	; 6028 <vs_device_bootable+0x40>
    6004:	e3a0302c 	mov	r3, #44	; 0x2c
    6008:	e0202093 	mla	r0, r3, r0, r2
    600c:	e3500000 	cmp	r0, #0	; 0x0
    6010:	15903008 	ldrne	r3, [r0, #8]
    6014:	12030001 	andne	r0, r3, #1	; 0x1
    6018:	112fff1e 	bxne	lr
    601c:	e3a00000 	mov	r0, #0	; 0x0
    6020:	e12fff1e 	bx	lr
    6024:	3c00d178 	.word	0x3c00d178
    6028:	0001561c 	.word	0x0001561c

0000602c <vs_device_burnsrc>:
    602c:	e350000e 	cmp	r0, #14	; 0xe
    6030:	059f3034 	ldreq	r3, [pc, #52]	; 606c <vs_device_burnsrc+0x40>
    6034:	05930000 	ldreq	r0, [r3]
    6038:	0a000004 	beq	6050 <vs_device_burnsrc+0x24>
    603c:	e350000d 	cmp	r0, #13	; 0xd
    6040:	8a000007 	bhi	6064 <vs_device_burnsrc+0x38>
    6044:	e59f2024 	ldr	r2, [pc, #36]	; 6070 <vs_device_burnsrc+0x44>
    6048:	e3a0302c 	mov	r3, #44	; 0x2c
    604c:	e0202093 	mla	r0, r3, r0, r2
    6050:	e3500000 	cmp	r0, #0	; 0x0
    6054:	15903008 	ldrne	r3, [r0, #8]
    6058:	11a030a3 	lsrne	r3, r3, #1
    605c:	12030001 	andne	r0, r3, #1	; 0x1
    6060:	112fff1e 	bxne	lr
    6064:	e3a00000 	mov	r0, #0	; 0x0
    6068:	e12fff1e 	bx	lr
    606c:	3c00d178 	.word	0x3c00d178
    6070:	0001561c 	.word	0x0001561c

00006074 <vs_device_burndst>:
    6074:	e350000e 	cmp	r0, #14	; 0xe
    6078:	059f3034 	ldreq	r3, [pc, #52]	; 60b4 <vs_device_burndst+0x40>
    607c:	05930000 	ldreq	r0, [r3]
    6080:	0a000004 	beq	6098 <vs_device_burndst+0x24>
    6084:	e350000d 	cmp	r0, #13	; 0xd
    6088:	8a000007 	bhi	60ac <vs_device_burndst+0x38>
    608c:	e59f2024 	ldr	r2, [pc, #36]	; 60b8 <vs_device_burndst+0x44>
    6090:	e3a0302c 	mov	r3, #44	; 0x2c
    6094:	e0202093 	mla	r0, r3, r0, r2
    6098:	e3500000 	cmp	r0, #0	; 0x0
    609c:	15903008 	ldrne	r3, [r0, #8]
    60a0:	11a03123 	lsrne	r3, r3, #2
    60a4:	12030001 	andne	r0, r3, #1	; 0x1
    60a8:	112fff1e 	bxne	lr
    60ac:	e3a00000 	mov	r0, #0	; 0x0
    60b0:	e12fff1e 	bx	lr
    60b4:	3c00d178 	.word	0x3c00d178
    60b8:	0001561c 	.word	0x0001561c

000060bc <vs_device_boot0>:
    60bc:	e350000e 	cmp	r0, #14	; 0xe
    60c0:	059f3034 	ldreq	r3, [pc, #52]	; 60fc <vs_device_boot0+0x40>
    60c4:	05930000 	ldreq	r0, [r3]
    60c8:	0a000004 	beq	60e0 <vs_device_boot0+0x24>
    60cc:	e350000d 	cmp	r0, #13	; 0xd
    60d0:	8a000007 	bhi	60f4 <vs_device_boot0+0x38>
    60d4:	e59f2024 	ldr	r2, [pc, #36]	; 6100 <vs_device_boot0+0x44>
    60d8:	e3a0302c 	mov	r3, #44	; 0x2c
    60dc:	e0202093 	mla	r0, r3, r0, r2
    60e0:	e3500000 	cmp	r0, #0	; 0x0
    60e4:	15903008 	ldrne	r3, [r0, #8]
    60e8:	11a031a3 	lsrne	r3, r3, #3
    60ec:	12030001 	andne	r0, r3, #1	; 0x1
    60f0:	112fff1e 	bxne	lr
    60f4:	e3a00000 	mov	r0, #0	; 0x0
    60f8:	e12fff1e 	bx	lr
    60fc:	3c00d178 	.word	0x3c00d178
    6100:	0001561c 	.word	0x0001561c

00006104 <vs_device_name>:
    6104:	e350000e 	cmp	r0, #14	; 0xe
    6108:	059f3028 	ldreq	r3, [pc, #40]	; 6138 <vs_device_name+0x34>
    610c:	05930000 	ldreq	r0, [r3]
    6110:	0a000004 	beq	6128 <vs_device_name+0x24>
    6114:	e350000d 	cmp	r0, #13	; 0xd
    6118:	8a000004 	bhi	6130 <vs_device_name+0x2c>
    611c:	e59f2018 	ldr	r2, [pc, #24]	; 613c <vs_device_name+0x38>
    6120:	e3a0302c 	mov	r3, #44	; 0x2c
    6124:	e0202093 	mla	r0, r3, r0, r2
    6128:	e3500000 	cmp	r0, #0	; 0x0
    612c:	112fff1e 	bxne	lr
    6130:	e3a00000 	mov	r0, #0	; 0x0
    6134:	e12fff1e 	bx	lr
    6138:	3c00d178 	.word	0x3c00d178
    613c:	0001561c 	.word	0x0001561c

00006140 <vs_device_erasable>:
    6140:	e350000e 	cmp	r0, #14	; 0xe
    6144:	059f3034 	ldreq	r3, [pc, #52]	; 6180 <vs_device_erasable+0x40>
    6148:	05930000 	ldreq	r0, [r3]
    614c:	0a000004 	beq	6164 <vs_device_erasable+0x24>
    6150:	e350000d 	cmp	r0, #13	; 0xd
    6154:	8a000007 	bhi	6178 <vs_device_erasable+0x38>
    6158:	e59f2024 	ldr	r2, [pc, #36]	; 6184 <vs_device_erasable+0x44>
    615c:	e3a0302c 	mov	r3, #44	; 0x2c
    6160:	e0202093 	mla	r0, r3, r0, r2
    6164:	e3500000 	cmp	r0, #0	; 0x0
    6168:	15903008 	ldrne	r3, [r0, #8]
    616c:	11a03223 	lsrne	r3, r3, #4
    6170:	12030001 	andne	r0, r3, #1	; 0x1
    6174:	112fff1e 	bxne	lr
    6178:	e3a00000 	mov	r0, #0	; 0x0
    617c:	e12fff1e 	bx	lr
    6180:	3c00d178 	.word	0x3c00d178
    6184:	0001561c 	.word	0x0001561c

00006188 <vs_device_count>:
    6188:	e3a0000e 	mov	r0, #14	; 0xe
    618c:	e12fff1e 	bx	lr

00006190 <vs_device_id>:
    6190:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6194:	e59f7040 	ldr	r7, [pc, #64]	; 61dc <vs_device_id+0x4c>
    6198:	e24dd004 	sub	sp, sp, #4	; 0x4
    619c:	e1a05000 	mov	r5, r0
    61a0:	e3a04000 	mov	r4, #0	; 0x0
    61a4:	e3a0602c 	mov	r6, #44	; 0x2c
    61a8:	e0217496 	mla	r1, r6, r4, r7
    61ac:	e1a00005 	mov	r0, r5
    61b0:	e3a02010 	mov	r2, #16	; 0x10
    61b4:	ebffecad 	bl	1470 <strncmp>
    61b8:	e3500000 	cmp	r0, #0	; 0x0
    61bc:	0a000003 	beq	61d0 <vs_device_id+0x40>
    61c0:	e2844001 	add	r4, r4, #1	; 0x1
    61c4:	e354000e 	cmp	r4, #14	; 0xe
    61c8:	1afffff6 	bne	61a8 <vs_device_id+0x18>
    61cc:	e2844001 	add	r4, r4, #1	; 0x1
    61d0:	e1a00004 	mov	r0, r4
    61d4:	e28dd004 	add	sp, sp, #4	; 0x4
    61d8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    61dc:	0001561c 	.word	0x0001561c

000061e0 <vs_is_device>:
    61e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    61e4:	e350000e 	cmp	r0, #14	; 0xe
    61e8:	059f304c 	ldreq	r3, [pc, #76]	; 623c <vs_is_device+0x5c>
    61ec:	e1a0c001 	mov	ip, r1
    61f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    61f4:	05931000 	ldreq	r1, [r3]
    61f8:	0a000004 	beq	6210 <vs_is_device+0x30>
    61fc:	e350000d 	cmp	r0, #13	; 0xd
    6200:	8a00000a 	bhi	6230 <vs_is_device+0x50>
    6204:	e59f2034 	ldr	r2, [pc, #52]	; 6240 <vs_is_device+0x60>
    6208:	e3a0302c 	mov	r3, #44	; 0x2c
    620c:	e0212093 	mla	r1, r3, r0, r2
    6210:	e3510000 	cmp	r1, #0	; 0x0
    6214:	0a000005 	beq	6230 <vs_is_device+0x50>
    6218:	e1a0000c 	mov	r0, ip
    621c:	e3a02010 	mov	r2, #16	; 0x10
    6220:	ebffec92 	bl	1470 <strncmp>
    6224:	e2700001 	rsbs	r0, r0, #1	; 0x1
    6228:	33a00000 	movcc	r0, #0	; 0x0
    622c:	ea000000 	b	6234 <vs_is_device+0x54>
    6230:	e3a00000 	mov	r0, #0	; 0x0
    6234:	e28dd004 	add	sp, sp, #4	; 0x4
    6238:	e8bd8000 	pop	{pc}
    623c:	3c00d178 	.word	0x3c00d178
    6240:	0001561c 	.word	0x0001561c

00006244 <vs_read>:
    6244:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6248:	e59f4234 	ldr	r4, [pc, #564]	; 6484 <vs_read+0x240>
    624c:	e1a09000 	mov	r9, r0
    6250:	e5940000 	ldr	r0, [r4]
    6254:	e24dd024 	sub	sp, sp, #36	; 0x24
    6258:	e3500000 	cmp	r0, #0	; 0x0
    625c:	e1a06002 	mov	r6, r2
    6260:	e1a07003 	mov	r7, r3
    6264:	e59d5048 	ldr	r5, [sp, #72]
    6268:	03e04001 	mvneq	r4, #1	; 0x1
    626c:	0a000081 	beq	6478 <vs_read+0x234>
    6270:	e5903010 	ldr	r3, [r0, #16]
    6274:	e3530000 	cmp	r3, #0	; 0x0
    6278:	03e04002 	mvneq	r4, #2	; 0x2
    627c:	0a00007d 	beq	6478 <vs_read+0x234>
    6280:	e3550000 	cmp	r5, #0	; 0x0
    6284:	01a04005 	moveq	r4, r5
    6288:	0a00007a 	beq	6478 <vs_read+0x234>
    628c:	e59f11f4 	ldr	r1, [pc, #500]	; 6488 <vs_read+0x244>
    6290:	e3a02003 	mov	r2, #3	; 0x3
    6294:	ebffec75 	bl	1470 <strncmp>
    6298:	e3500000 	cmp	r0, #0	; 0x0
    629c:	1a00000a 	bne	62cc <vs_read+0x88>
    62a0:	e59d204c 	ldr	r2, [sp, #76]
    62a4:	e58d5000 	str	r5, [sp]
    62a8:	e58d2004 	str	r2, [sp, #4]
    62ac:	e5941000 	ldr	r1, [r4]
    62b0:	e1a00009 	mov	r0, r9
    62b4:	e1a02006 	mov	r2, r6
    62b8:	e1a03007 	mov	r3, r7
    62bc:	e1a0e00f 	mov	lr, pc
    62c0:	e591f010 	ldr	pc, [r1, #16]
    62c4:	e1a04000 	mov	r4, r0
    62c8:	ea00006a 	b	6478 <vs_read+0x234>
    62cc:	e3a0000e 	mov	r0, #14	; 0xe
    62d0:	ebffff2d 	bl	5f8c <vs_align>
    62d4:	e2402001 	sub	r2, r0, #1	; 0x1
    62d8:	e0124006 	ands	r4, r2, r6
    62dc:	10644000 	rsbne	r4, r4, r0
    62e0:	e0643005 	rsb	r3, r4, r5
    62e4:	e0032002 	and	r2, r3, r2
    62e8:	e192a004 	orrs	sl, r2, r4
    62ec:	e1a08000 	mov	r8, r0
    62f0:	e58d3014 	str	r3, [sp, #20]
    62f4:	e58d200c 	str	r2, [sp, #12]
    62f8:	e58da018 	str	sl, [sp, #24]
    62fc:	01a0400a 	moveq	r4, sl
    6300:	058da01c 	streq	sl, [sp, #28]
    6304:	0a00002c 	beq	63bc <vs_read+0x178>
    6308:	ebffecf6 	bl	16e8 <malloc>
    630c:	e250b000 	subs	fp, r0, #0	; 0x0
    6310:	e58db01c 	str	fp, [sp, #28]
    6314:	1a00000c 	bne	634c <vs_read+0x108>
    6318:	e59f416c 	ldr	r4, [pc, #364]	; 648c <vs_read+0x248>
    631c:	e59f016c 	ldr	r0, [pc, #364]	; 6490 <vs_read+0x24c>
    6320:	e3001115 	movw	r1, #277	; 0x115
    6324:	e1a0e00f 	mov	lr, pc
    6328:	e594f000 	ldr	pc, [r4]
    632c:	e59f0160 	ldr	r0, [pc, #352]	; 6494 <vs_read+0x250>
    6330:	e59f1160 	ldr	r1, [pc, #352]	; 6498 <vs_read+0x254>
    6334:	e1a0e00f 	mov	lr, pc
    6338:	e594f000 	ldr	pc, [r4]
    633c:	e59f0158 	ldr	r0, [pc, #344]	; 649c <vs_read+0x258>
    6340:	e1a0e00f 	mov	lr, pc
    6344:	e594f000 	ldr	pc, [r4]
    6348:	eafffffe 	b	6348 <vs_read+0x104>
    634c:	e3540000 	cmp	r4, #0	; 0x0
    6350:	0a000019 	beq	63bc <vs_read+0x178>
    6354:	e59f2128 	ldr	r2, [pc, #296]	; 6484 <vs_read+0x240>
    6358:	e2683000 	rsb	r3, r8, #0	; 0x0
    635c:	e5921000 	ldr	r1, [r2]
    6360:	e59d204c 	ldr	r2, [sp, #76]
    6364:	e006a003 	and	sl, r6, r3
    6368:	e3a0b000 	mov	fp, #0	; 0x0
    636c:	e58d2004 	str	r2, [sp, #4]
    6370:	e1a0300b 	mov	r3, fp
    6374:	e58d8000 	str	r8, [sp]
    6378:	e1a0200a 	mov	r2, sl
    637c:	e1a0e00f 	mov	lr, pc
    6380:	e591f010 	ldr	pc, [r1, #16]
    6384:	e250b000 	subs	fp, r0, #0	; 0x0
    6388:	b1a0400b 	movlt	r4, fp
    638c:	ba000037 	blt	6470 <vs_read+0x22c>
    6390:	e59d201c 	ldr	r2, [sp, #28]
    6394:	e0641008 	rsb	r1, r4, r8
    6398:	e0821001 	add	r1, r2, r1
    639c:	e1a00009 	mov	r0, r9
    63a0:	e1a02004 	mov	r2, r4
    63a4:	ebffec6a 	bl	1554 <memcpy>
    63a8:	e0966004 	adds	r6, r6, r4
    63ac:	e59d5014 	ldr	r5, [sp, #20]
    63b0:	e0899004 	add	r9, r9, r4
    63b4:	e2a77000 	adc	r7, r7, #0	; 0x0
    63b8:	e1a0400b 	mov	r4, fp
    63bc:	e59d300c 	ldr	r3, [sp, #12]
    63c0:	e0555003 	subs	r5, r5, r3
    63c4:	0a00000f 	beq	6408 <vs_read+0x1c4>
    63c8:	e59f30b4 	ldr	r3, [pc, #180]	; 6484 <vs_read+0x240>
    63cc:	e59da04c 	ldr	sl, [sp, #76]
    63d0:	e58d5000 	str	r5, [sp]
    63d4:	e5931000 	ldr	r1, [r3]
    63d8:	e1a00009 	mov	r0, r9
    63dc:	e58da004 	str	sl, [sp, #4]
    63e0:	e1a02006 	mov	r2, r6
    63e4:	e1a03007 	mov	r3, r7
    63e8:	e1a0e00f 	mov	lr, pc
    63ec:	e591f010 	ldr	pc, [r1, #16]
    63f0:	e3500000 	cmp	r0, #0	; 0x0
    63f4:	ba000012 	blt	6444 <vs_read+0x200>
    63f8:	e0966005 	adds	r6, r6, r5
    63fc:	e0a77fc5 	adc	r7, r7, r5, asr #31
    6400:	e0844000 	add	r4, r4, r0
    6404:	e0899005 	add	r9, r9, r5
    6408:	e59db00c 	ldr	fp, [sp, #12]
    640c:	e35b0000 	cmp	fp, #0	; 0x0
    6410:	0a000013 	beq	6464 <vs_read+0x220>
    6414:	e59d204c 	ldr	r2, [sp, #76]
    6418:	e59f3064 	ldr	r3, [pc, #100]	; 6484 <vs_read+0x240>
    641c:	e58d2004 	str	r2, [sp, #4]
    6420:	e58d8000 	str	r8, [sp]
    6424:	e5931000 	ldr	r1, [r3]
    6428:	e1a02006 	mov	r2, r6
    642c:	e1a03007 	mov	r3, r7
    6430:	e59d001c 	ldr	r0, [sp, #28]
    6434:	e1a0e00f 	mov	lr, pc
    6438:	e591f010 	ldr	pc, [r1, #16]
    643c:	e3500000 	cmp	r0, #0	; 0x0
    6440:	aa000001 	bge	644c <vs_read+0x208>
    6444:	e1a04000 	mov	r4, r0
    6448:	ea000005 	b	6464 <vs_read+0x220>
    644c:	e1a00009 	mov	r0, r9
    6450:	e59d101c 	ldr	r1, [sp, #28]
    6454:	e59d200c 	ldr	r2, [sp, #12]
    6458:	ebffec3d 	bl	1554 <memcpy>
    645c:	e59d300c 	ldr	r3, [sp, #12]
    6460:	e0844003 	add	r4, r4, r3
    6464:	e59da018 	ldr	sl, [sp, #24]
    6468:	e35a0000 	cmp	sl, #0	; 0x0
    646c:	0a000001 	beq	6478 <vs_read+0x234>
    6470:	e59d001c 	ldr	r0, [sp, #28]
    6474:	ebffed03 	bl	1888 <free>
    6478:	e1a00004 	mov	r0, r4
    647c:	e28dd024 	add	sp, sp, #36	; 0x24
    6480:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    6484:	3c00d178 	.word	0x3c00d178
    6488:	00016283 	.word	0x00016283
    648c:	3c00d6cc 	.word	0x3c00d6cc
    6490:	00017b71 	.word	0x00017b71
    6494:	00016437 	.word	0x00016437
    6498:	00017b83 	.word	0x00017b83
    649c:	00017b8e 	.word	0x00017b8e

000064a0 <vs_reset>:
    64a0:	e92d4010 	push	{r4, lr}
    64a4:	e59f402c 	ldr	r4, [pc, #44]	; 64d8 <vs_reset+0x38>
    64a8:	e5940000 	ldr	r0, [r4]
    64ac:	ebffea88 	bl	ed4 <cpu_recase>
    64b0:	e5944000 	ldr	r4, [r4]
    64b4:	e3540000 	cmp	r4, #0	; 0x0
    64b8:	03e00001 	mvneq	r0, #1	; 0x1
    64bc:	08bd8010 	popeq	{r4, pc}
    64c0:	e5944018 	ldr	r4, [r4, #24]
    64c4:	e3540000 	cmp	r4, #0	; 0x0
    64c8:	03e00002 	mvneq	r0, #2	; 0x2
    64cc:	08bd8010 	popeq	{r4, pc}
    64d0:	e12fff34 	blx	r4
    64d4:	e8bd8010 	pop	{r4, pc}
    64d8:	3c00d178 	.word	0x3c00d178

000064dc <vs_assign_by_id>:
    64dc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    64e0:	e59fc05c 	ldr	ip, [pc, #92]	; 6544 <vs_assign_by_id+0x68>
    64e4:	e3a03000 	mov	r3, #0	; 0x0
    64e8:	e350000d 	cmp	r0, #13	; 0xd
    64ec:	e24dd004 	sub	sp, sp, #4	; 0x4
    64f0:	e58c3000 	str	r3, [ip]
    64f4:	8a00000b 	bhi	6528 <vs_assign_by_id+0x4c>
    64f8:	e59f2048 	ldr	r2, [pc, #72]	; 6548 <vs_assign_by_id+0x6c>
    64fc:	e283302c 	add	r3, r3, #44	; 0x2c
    6500:	e0232093 	mla	r3, r3, r0, r2
    6504:	e3510000 	cmp	r1, #0	; 0x0
    6508:	e58c3000 	str	r3, [ip]
    650c:	01a00001 	moveq	r0, r1
    6510:	0a000002 	beq	6520 <vs_assign_by_id+0x44>
    6514:	e28dd004 	add	sp, sp, #4	; 0x4
    6518:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    651c:	eaffffdf 	b	64a0 <vs_reset>
    6520:	e28dd004 	add	sp, sp, #4	; 0x4
    6524:	e8bd8000 	pop	{pc}
    6528:	e1a01000 	mov	r1, r0
    652c:	e59f3018 	ldr	r3, [pc, #24]	; 654c <vs_assign_by_id+0x70>
    6530:	e59f0018 	ldr	r0, [pc, #24]	; 6550 <vs_assign_by_id+0x74>
    6534:	e1a0e00f 	mov	lr, pc
    6538:	e593f000 	ldr	pc, [r3]
    653c:	e3e00000 	mvn	r0, #0	; 0x0
    6540:	eafffff6 	b	6520 <vs_assign_by_id+0x44>
    6544:	3c00d178 	.word	0x3c00d178
    6548:	0001561c 	.word	0x0001561c
    654c:	3c00d6cc 	.word	0x3c00d6cc
    6550:	00017bbf 	.word	0x00017bbf

00006554 <vs_assign_by_name>:
    6554:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    6558:	e59f3084 	ldr	r3, [pc, #132]	; 65e4 <vs_assign_by_name+0x90>
    655c:	e3a02000 	mov	r2, #0	; 0x0
    6560:	e24dd004 	sub	sp, sp, #4	; 0x4
    6564:	e5832000 	str	r2, [r3]
    6568:	e1a06000 	mov	r6, r0
    656c:	e1a07001 	mov	r7, r1
    6570:	e1a04002 	mov	r4, r2
    6574:	e3a0a02c 	mov	sl, #44	; 0x2c
    6578:	e59f8068 	ldr	r8, [pc, #104]	; 65e8 <vs_assign_by_name+0x94>
    657c:	ea00000d 	b	65b8 <vs_assign_by_name+0x64>
    6580:	e025849a 	mla	r5, sl, r4, r8
    6584:	e1a00005 	mov	r0, r5
    6588:	ebffebb8 	bl	1470 <strncmp>
    658c:	e3500000 	cmp	r0, #0	; 0x0
    6590:	1a000007 	bne	65b4 <vs_assign_by_name+0x60>
    6594:	e59f3048 	ldr	r3, [pc, #72]	; 65e4 <vs_assign_by_name+0x90>
    6598:	e3570000 	cmp	r7, #0	; 0x0
    659c:	e5835000 	str	r5, [r3]
    65a0:	01a00007 	moveq	r0, r7
    65a4:	0a00000c 	beq	65dc <vs_assign_by_name+0x88>
    65a8:	e28dd004 	add	sp, sp, #4	; 0x4
    65ac:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    65b0:	eaffffba 	b	64a0 <vs_reset>
    65b4:	e2844001 	add	r4, r4, #1	; 0x1
    65b8:	e354000e 	cmp	r4, #14	; 0xe
    65bc:	e1a01006 	mov	r1, r6
    65c0:	e3a02010 	mov	r2, #16	; 0x10
    65c4:	1affffed 	bne	6580 <vs_assign_by_name+0x2c>
    65c8:	e59f001c 	ldr	r0, [pc, #28]	; 65ec <vs_assign_by_name+0x98>
    65cc:	e59f301c 	ldr	r3, [pc, #28]	; 65f0 <vs_assign_by_name+0x9c>
    65d0:	e1a0e00f 	mov	lr, pc
    65d4:	e593f000 	ldr	pc, [r3]
    65d8:	e3e00000 	mvn	r0, #0	; 0x0
    65dc:	e28dd004 	add	sp, sp, #4	; 0x4
    65e0:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    65e4:	3c00d178 	.word	0x3c00d178
    65e8:	0001561c 	.word	0x0001561c
    65ec:	00017bd8 	.word	0x00017bd8
    65f0:	3c00d6cc 	.word	0x3c00d6cc

000065f4 <ss_lock_isk>:
    65f4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    65f8:	e24dd004 	sub	sp, sp, #4	; 0x4
    65fc:	eb0001d5 	bl	6d58 <security_enabled>
    6600:	e3500000 	cmp	r0, #0	; 0x0
    6604:	0a000002 	beq	6614 <ss_lock_isk+0x20>
    6608:	e28dd004 	add	sp, sp, #4	; 0x4
    660c:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    6610:	ea00002e 	b	66d0 <block_cipher_lock_isk>
    6614:	e28dd004 	add	sp, sp, #4	; 0x4
    6618:	e8bd8000 	pop	{pc}

0000661c <ss_aes_data>:
    661c:	e92d4070 	push	{r4, r5, r6, lr}
    6620:	e1a05000 	mov	r5, r0
    6624:	e1a06001 	mov	r6, r1
    6628:	e1a04002 	mov	r4, r2
    662c:	eb0001c9 	bl	6d58 <security_enabled>
    6630:	e3500000 	cmp	r0, #0	; 0x0
    6634:	0a000007 	beq	6658 <ss_aes_data+0x3c>
    6638:	e284200f 	add	r2, r4, #15	; 0xf
    663c:	e1a00005 	mov	r0, r5
    6640:	e1a01006 	mov	r1, r6
    6644:	e3c2200f 	bic	r2, r2, #15	; 0xf
    6648:	eb0000a0 	bl	68d0 <block_cipher_operation>
    664c:	e3500000 	cmp	r0, #0	; 0x0
    6650:	13e00000 	mvnne	r0, #0	; 0x0
    6654:	18bd8070 	popne	{r4, r5, r6, pc}
    6658:	e3a00000 	mov	r0, #0	; 0x0
    665c:	e8bd8070 	pop	{r4, r5, r6, pc}

00006660 <ss_aes_init>:
    6660:	e92d4070 	push	{r4, r5, r6, lr}
    6664:	e1a05000 	mov	r5, r0
    6668:	e1a04001 	mov	r4, r1
    666c:	e1a06002 	mov	r6, r2
    6670:	eb0001b8 	bl	6d58 <security_enabled>
    6674:	e3500000 	cmp	r0, #0	; 0x0
    6678:	0a00000b 	beq	66ac <ss_aes_init+0x4c>
    667c:	e3540000 	cmp	r4, #0	; 0x0
    6680:	01a00005 	moveq	r0, r5
    6684:	01a02004 	moveq	r2, r4
    6688:	01a03006 	moveq	r3, r6
    668c:	03a01001 	moveq	r1, #1	; 0x1
    6690:	11a00005 	movne	r0, r5
    6694:	11a02004 	movne	r2, r4
    6698:	11a03006 	movne	r3, r6
    669c:	13a01000 	movne	r1, #0	; 0x0
    66a0:	eb00002c 	bl	6758 <block_cipher_aes_init>
    66a4:	e3500000 	cmp	r0, #0	; 0x0
    66a8:	1a000001 	bne	66b4 <ss_aes_init+0x54>
    66ac:	e3a00000 	mov	r0, #0	; 0x0
    66b0:	e8bd8070 	pop	{r4, r5, r6, pc}
    66b4:	e3e00000 	mvn	r0, #0	; 0x0
    66b8:	e8bd8070 	pop	{r4, r5, r6, pc}

000066bc <check_register_bit>:
    66bc:	e3a03001 	mov	r3, #1	; 0x1
    66c0:	e0103113 	ands	r3, r0, r3, lsl r1
    66c4:	03a00000 	moveq	r0, #0	; 0x0
    66c8:	13a00001 	movne	r0, #1	; 0x1
    66cc:	e12fff1e 	bx	lr

000066d0 <block_cipher_lock_isk>:
    66d0:	e59f200c 	ldr	r2, [pc, #12]	; 66e4 <block_cipher_lock_isk+0x14>
    66d4:	e5923400 	ldr	r3, [r2, #1024]
    66d8:	e3833706 	orr	r3, r3, #1572864	; 0x180000
    66dc:	e5823400 	str	r3, [r2, #1024]
    66e0:	e12fff1e 	bx	lr
    66e4:	23200000 	.word	0x23200000

000066e8 <block_cipher_reset>:
    66e8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    66ec:	e59f4060 	ldr	r4, [pc, #96]	; 6754 <block_cipher_reset+0x6c>
    66f0:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    66f4:	e5843400 	str	r3, [r4, #1024]
    66f8:	e24dd004 	sub	sp, sp, #4	; 0x4
    66fc:	eb001ca5 	bl	d998 <get_ticks>
    6700:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    6704:	e3a07000 	mov	r7, #0	; 0x0
    6708:	e0966000 	adds	r6, r6, r0
    670c:	e1a05004 	mov	r5, r4
    6710:	e0a77001 	adc	r7, r7, r1
    6714:	e5944400 	ldr	r4, [r4, #1024]
    6718:	ea000008 	b	6740 <block_cipher_reset+0x58>
    671c:	e5954400 	ldr	r4, [r5, #1024]
    6720:	eb001c9c 	bl	d998 <get_ticks>
    6724:	e1510007 	cmp	r1, r7
    6728:	8a000002 	bhi	6738 <block_cipher_reset+0x50>
    672c:	1a000003 	bne	6740 <block_cipher_reset+0x58>
    6730:	e1500006 	cmp	r0, r6
    6734:	9a000001 	bls	6740 <block_cipher_reset+0x58>
    6738:	e3a00001 	mov	r0, #1	; 0x1
    673c:	ea000002 	b	674c <block_cipher_reset+0x64>
    6740:	e3540000 	cmp	r4, #0	; 0x0
    6744:	bafffff4 	blt	671c <block_cipher_reset+0x34>
    6748:	e3a00000 	mov	r0, #0	; 0x0
    674c:	e28dd004 	add	sp, sp, #4	; 0x4
    6750:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6754:	23200000 	.word	0x23200000

00006758 <block_cipher_aes_init>:
    6758:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    675c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6760:	e1a05000 	mov	r5, r0
    6764:	e1a04001 	mov	r4, r1
    6768:	e1a06002 	mov	r6, r2
    676c:	e1a07003 	mov	r7, r3
    6770:	ebffffdc 	bl	66e8 <block_cipher_reset>
    6774:	e3500000 	cmp	r0, #0	; 0x0
    6778:	1a00003d 	bne	6874 <block_cipher_aes_init+0x11c>
    677c:	e59f3134 	ldr	r3, [pc, #308]	; 68b8 <block_cipher_aes_init+0x160>
    6780:	e3a02001 	mov	r2, #1	; 0x1
    6784:	e3550001 	cmp	r5, #1	; 0x1
    6788:	e5832404 	str	r2, [r3, #1028]
    678c:	8a000038 	bhi	6874 <block_cipher_aes_init+0x11c>
    6790:	e3540003 	cmp	r4, #3	; 0x3
    6794:	13540000 	cmpne	r4, #0	; 0x0
    6798:	0a000001 	beq	67a4 <block_cipher_aes_init+0x4c>
    679c:	e1540002 	cmp	r4, r2
    67a0:	1a000033 	bne	6874 <block_cipher_aes_init+0x11c>
    67a4:	e1a03305 	lsl	r3, r5, #6
    67a8:	e3833901 	orr	r3, r3, #16384	; 0x4000
    67ac:	e3833004 	orr	r3, r3, #4	; 0x4
    67b0:	e59f2100 	ldr	r2, [pc, #256]	; 68b8 <block_cipher_aes_init+0x160>
    67b4:	e1833984 	orr	r3, r3, r4, lsl #19
    67b8:	e3540000 	cmp	r4, #0	; 0x0
    67bc:	e5823400 	str	r3, [r2, #1024]
    67c0:	1a000008 	bne	67e8 <block_cipher_aes_init+0x90>
    67c4:	e59f10f0 	ldr	r1, [pc, #240]	; 68bc <block_cipher_aes_init+0x164>
    67c8:	e59f00f0 	ldr	r0, [pc, #240]	; 68c0 <block_cipher_aes_init+0x168>
    67cc:	e59f30f0 	ldr	r3, [pc, #240]	; 68c4 <block_cipher_aes_init+0x16c>
    67d0:	e0862001 	add	r2, r6, r1
    67d4:	e0823003 	add	r3, r2, r3
    67d8:	e5933000 	ldr	r3, [r3]
    67dc:	e4813004 	str	r3, [r1], #4
    67e0:	e1510000 	cmp	r1, r0
    67e4:	1afffff8 	bne	67cc <block_cipher_aes_init+0x74>
    67e8:	e59f10d0 	ldr	r1, [pc, #208]	; 68c0 <block_cipher_aes_init+0x168>
    67ec:	e59f00d4 	ldr	r0, [pc, #212]	; 68c8 <block_cipher_aes_init+0x170>
    67f0:	e59f30d4 	ldr	r3, [pc, #212]	; 68cc <block_cipher_aes_init+0x174>
    67f4:	e0872001 	add	r2, r7, r1
    67f8:	e0823003 	add	r3, r2, r3
    67fc:	e5933000 	ldr	r3, [r3]
    6800:	e4813004 	str	r3, [r1], #4
    6804:	e1510000 	cmp	r1, r0
    6808:	1afffff8 	bne	67f0 <block_cipher_aes_init+0x98>
    680c:	e3540000 	cmp	r4, #0	; 0x0
    6810:	1a000008 	bne	6838 <block_cipher_aes_init+0xe0>
    6814:	eb001c5f 	bl	d998 <get_ticks>
    6818:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    681c:	e3a05000 	mov	r5, #0	; 0x0
    6820:	e0944000 	adds	r4, r4, r0
    6824:	e59f608c 	ldr	r6, [pc, #140]	; 68b8 <block_cipher_aes_init+0x160>
    6828:	e0a55001 	adc	r5, r5, r1
    682c:	e596340c 	ldr	r3, [r6, #1036]
    6830:	e3130008 	tst	r3, #8	; 0x8
    6834:	0a000011 	beq	6880 <block_cipher_aes_init+0x128>
    6838:	eb001c56 	bl	d998 <get_ticks>
    683c:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    6840:	e3a05000 	mov	r5, #0	; 0x0
    6844:	e0944000 	adds	r4, r4, r0
    6848:	e59f6068 	ldr	r6, [pc, #104]	; 68b8 <block_cipher_aes_init+0x160>
    684c:	e0a55001 	adc	r5, r5, r1
    6850:	e596340c 	ldr	r3, [r6, #1036]
    6854:	e59f205c 	ldr	r2, [pc, #92]	; 68b8 <block_cipher_aes_init+0x160>
    6858:	e3130010 	tst	r3, #16	; 0x10
    685c:	0a00000e 	beq	689c <block_cipher_aes_init+0x144>
    6860:	e5923400 	ldr	r3, [r2, #1024]
    6864:	e3a00000 	mov	r0, #0	; 0x0
    6868:	e3833001 	orr	r3, r3, #1	; 0x1
    686c:	e5823400 	str	r3, [r2, #1024]
    6870:	ea000000 	b	6878 <block_cipher_aes_init+0x120>
    6874:	e3a00001 	mov	r0, #1	; 0x1
    6878:	e28dd004 	add	sp, sp, #4	; 0x4
    687c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6880:	eb001c44 	bl	d998 <get_ticks>
    6884:	e1510005 	cmp	r1, r5
    6888:	8afffff9 	bhi	6874 <block_cipher_aes_init+0x11c>
    688c:	1affffe6 	bne	682c <block_cipher_aes_init+0xd4>
    6890:	e1500004 	cmp	r0, r4
    6894:	8afffff6 	bhi	6874 <block_cipher_aes_init+0x11c>
    6898:	eaffffe3 	b	682c <block_cipher_aes_init+0xd4>
    689c:	eb001c3d 	bl	d998 <get_ticks>
    68a0:	e1510005 	cmp	r1, r5
    68a4:	8afffff2 	bhi	6874 <block_cipher_aes_init+0x11c>
    68a8:	1affffe8 	bne	6850 <block_cipher_aes_init+0xf8>
    68ac:	e1500004 	cmp	r0, r4
    68b0:	8affffef 	bhi	6874 <block_cipher_aes_init+0x11c>
    68b4:	eaffffe5 	b	6850 <block_cipher_aes_init+0xf8>
    68b8:	23200000 	.word	0x23200000
    68bc:	23200420 	.word	0x23200420
    68c0:	23200440 	.word	0x23200440
    68c4:	dcdffbe0 	.word	0xdcdffbe0
    68c8:	23200450 	.word	0x23200450
    68cc:	dcdffbc0 	.word	0xdcdffbc0

000068d0 <block_cipher_operation>:
    68d0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    68d4:	e24dd00c 	sub	sp, sp, #12	; 0xc
    68d8:	e1a08002 	mov	r8, r2
    68dc:	e58d0004 	str	r0, [sp, #4]
    68e0:	e1a09001 	mov	r9, r1
    68e4:	e1a06002 	mov	r6, r2
    68e8:	e59fa0c4 	ldr	sl, [pc, #196]	; 69b4 <block_cipher_operation+0xe4>
    68ec:	e3a0b000 	mov	fp, #0	; 0x0
    68f0:	ea000028 	b	6998 <block_cipher_operation+0xc8>
    68f4:	e59d3004 	ldr	r3, [sp, #4]
    68f8:	e3560902 	cmp	r6, #32768	; 0x8000
    68fc:	b1a07006 	movlt	r7, r6
    6900:	a3a07902 	movge	r7, #32768	; 0x8000
    6904:	e0664008 	rsb	r4, r6, r8
    6908:	e3a00000 	mov	r0, #0	; 0x0
    690c:	e0831004 	add	r1, r3, r4
    6910:	e1a02007 	mov	r2, r7
    6914:	eb0000ed 	bl	6cd0 <dma_sdma_init>
    6918:	e3500000 	cmp	r0, #0	; 0x0
    691c:	1a000021 	bne	69a8 <block_cipher_operation+0xd8>
    6920:	e0891004 	add	r1, r9, r4
    6924:	e2800001 	add	r0, r0, #1	; 0x1
    6928:	e1a02007 	mov	r2, r7
    692c:	eb0000e7 	bl	6cd0 <dma_sdma_init>
    6930:	e3500000 	cmp	r0, #0	; 0x0
    6934:	1a00001b 	bne	69a8 <block_cipher_operation+0xd8>
    6938:	eb0000d4 	bl	6c90 <dma_start>
    693c:	e3a00001 	mov	r0, #1	; 0x1
    6940:	eb0000d2 	bl	6c90 <dma_start>
    6944:	eb001c13 	bl	d998 <get_ticks>
    6948:	e090400a 	adds	r4, r0, sl
    694c:	e0a1500b 	adc	r5, r1, fp
    6950:	ea000005 	b	696c <block_cipher_operation+0x9c>
    6954:	eb001c0f 	bl	d998 <get_ticks>
    6958:	e1510005 	cmp	r1, r5
    695c:	8a000011 	bhi	69a8 <block_cipher_operation+0xd8>
    6960:	1a000001 	bne	696c <block_cipher_operation+0x9c>
    6964:	e1500004 	cmp	r0, r4
    6968:	8a00000e 	bhi	69a8 <block_cipher_operation+0xd8>
    696c:	e3a01003 	mov	r1, #3	; 0x3
    6970:	e3a00001 	mov	r0, #1	; 0x1
    6974:	eb0000cb 	bl	6ca8 <dma_get_status>
    6978:	e3500000 	cmp	r0, #0	; 0x0
    697c:	0afffff4 	beq	6954 <block_cipher_operation+0x84>
    6980:	e3a00000 	mov	r0, #0	; 0x0
    6984:	e3a01003 	mov	r1, #3	; 0x3
    6988:	eb0000c6 	bl	6ca8 <dma_get_status>
    698c:	e3500000 	cmp	r0, #0	; 0x0
    6990:	0a000004 	beq	69a8 <block_cipher_operation+0xd8>
    6994:	e0676006 	rsb	r6, r7, r6
    6998:	e3560000 	cmp	r6, #0	; 0x0
    699c:	1affffd4 	bne	68f4 <block_cipher_operation+0x24>
    69a0:	e1a00006 	mov	r0, r6
    69a4:	ea000000 	b	69ac <block_cipher_operation+0xdc>
    69a8:	e3a00001 	mov	r0, #1	; 0x1
    69ac:	e28dd00c 	add	sp, sp, #12	; 0xc
    69b0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    69b4:	000f4240 	.word	0x000f4240

000069b8 <hash_len>:
    69b8:	e59f300c 	ldr	r3, [pc, #12]	; 69cc <hash_len+0x14>
    69bc:	e5932000 	ldr	r2, [r3]
    69c0:	e59f3008 	ldr	r3, [pc, #8]	; 69d0 <hash_len+0x18>
    69c4:	e7930202 	ldr	r0, [r3, r2, lsl #4]
    69c8:	e12fff1e 	bx	lr
    69cc:	3c00d17c 	.word	0x3c00d17c
    69d0:	00015884 	.word	0x00015884

000069d4 <hash_value>:
    69d4:	e92d4070 	push	{r4, r5, r6, lr}
    69d8:	e59f6054 	ldr	r6, [pc, #84]	; 6a34 <hash_value+0x60>
    69dc:	e1a05000 	mov	r5, r0
    69e0:	e5963000 	ldr	r3, [r6]
    69e4:	e3530000 	cmp	r3, #0	; 0x0
    69e8:	0a000009 	beq	6a14 <hash_value+0x40>
    69ec:	eb0000d9 	bl	6d58 <security_enabled>
    69f0:	e2504000 	subs	r4, r0, #0	; 0x0
    69f4:	1a000006 	bne	6a14 <hash_value+0x40>
    69f8:	e59f0038 	ldr	r0, [pc, #56]	; 6a38 <hash_value+0x64>
    69fc:	e5961000 	ldr	r1, [r6]
    6a00:	e59f3034 	ldr	r3, [pc, #52]	; 6a3c <hash_value+0x68>
    6a04:	e1a0e00f 	mov	lr, pc
    6a08:	e593f000 	ldr	pc, [r3]
    6a0c:	e1a00004 	mov	r0, r4
    6a10:	e8bd8070 	pop	{r4, r5, r6, pc}
    6a14:	e59f3018 	ldr	r3, [pc, #24]	; 6a34 <hash_value+0x60>
    6a18:	e1a00005 	mov	r0, r5
    6a1c:	e5932000 	ldr	r2, [r3]
    6a20:	e59f3018 	ldr	r3, [pc, #24]	; 6a40 <hash_value+0x6c>
    6a24:	e0833202 	add	r3, r3, r2, lsl #4
    6a28:	e1a0e00f 	mov	lr, pc
    6a2c:	e593f00c 	ldr	pc, [r3, #12]
    6a30:	e8bd8070 	pop	{r4, r5, r6, pc}
    6a34:	3c00d17c 	.word	0x3c00d17c
    6a38:	00017bf3 	.word	0x00017bf3
    6a3c:	3c00d6cc 	.word	0x3c00d6cc
    6a40:	00015884 	.word	0x00015884

00006a44 <hash_data>:
    6a44:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6a48:	e59f5074 	ldr	r5, [pc, #116]	; 6ac4 <hash_data+0x80>
    6a4c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6a50:	e5953000 	ldr	r3, [r5]
    6a54:	e1a06000 	mov	r6, r0
    6a58:	e3530000 	cmp	r3, #0	; 0x0
    6a5c:	e1a07001 	mov	r7, r1
    6a60:	0a000009 	beq	6a8c <hash_data+0x48>
    6a64:	eb0000bb 	bl	6d58 <security_enabled>
    6a68:	e2504000 	subs	r4, r0, #0	; 0x0
    6a6c:	1a000006 	bne	6a8c <hash_data+0x48>
    6a70:	e59f0050 	ldr	r0, [pc, #80]	; 6ac8 <hash_data+0x84>
    6a74:	e5951000 	ldr	r1, [r5]
    6a78:	e59f304c 	ldr	r3, [pc, #76]	; 6acc <hash_data+0x88>
    6a7c:	e1a0e00f 	mov	lr, pc
    6a80:	e593f000 	ldr	pc, [r3]
    6a84:	e1a00004 	mov	r0, r4
    6a88:	ea00000b 	b	6abc <hash_data+0x78>
    6a8c:	e59f003c 	ldr	r0, [pc, #60]	; 6ad0 <hash_data+0x8c>
    6a90:	e59f3034 	ldr	r3, [pc, #52]	; 6acc <hash_data+0x88>
    6a94:	e1a0e00f 	mov	lr, pc
    6a98:	e593f000 	ldr	pc, [r3]
    6a9c:	e59f3020 	ldr	r3, [pc, #32]	; 6ac4 <hash_data+0x80>
    6aa0:	e1a00006 	mov	r0, r6
    6aa4:	e5932000 	ldr	r2, [r3]
    6aa8:	e59f3024 	ldr	r3, [pc, #36]	; 6ad4 <hash_data+0x90>
    6aac:	e1a01007 	mov	r1, r7
    6ab0:	e0833202 	add	r3, r3, r2, lsl #4
    6ab4:	e1a0e00f 	mov	lr, pc
    6ab8:	e593f008 	ldr	pc, [r3, #8]
    6abc:	e28dd004 	add	sp, sp, #4	; 0x4
    6ac0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6ac4:	3c00d17c 	.word	0x3c00d17c
    6ac8:	00017bf3 	.word	0x00017bf3
    6acc:	3c00d6cc 	.word	0x3c00d6cc
    6ad0:	00017c14 	.word	0x00017c14
    6ad4:	00015884 	.word	0x00015884

00006ad8 <hash_init>:
    6ad8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6adc:	e3500003 	cmp	r0, #3	; 0x3
    6ae0:	e24dd004 	sub	sp, sp, #4	; 0x4
    6ae4:	e1a04000 	mov	r4, r0
    6ae8:	e1a07001 	mov	r7, r1
    6aec:	9a000005 	bls	6b08 <hash_init+0x30>
    6af0:	e59f0064 	ldr	r0, [pc, #100]	; 6b5c <hash_init+0x84>
    6af4:	e59f3064 	ldr	r3, [pc, #100]	; 6b60 <hash_init+0x88>
    6af8:	e1a0e00f 	mov	lr, pc
    6afc:	e593f000 	ldr	pc, [r3]
    6b00:	e3e00000 	mvn	r0, #0	; 0x0
    6b04:	ea000012 	b	6b54 <hash_init+0x7c>
    6b08:	e59f6054 	ldr	r6, [pc, #84]	; 6b64 <hash_init+0x8c>
    6b0c:	e3500000 	cmp	r0, #0	; 0x0
    6b10:	e5860000 	str	r0, [r6]
    6b14:	0a000009 	beq	6b40 <hash_init+0x68>
    6b18:	eb00008e 	bl	6d58 <security_enabled>
    6b1c:	e2505000 	subs	r5, r0, #0	; 0x0
    6b20:	1a000006 	bne	6b40 <hash_init+0x68>
    6b24:	e59f003c 	ldr	r0, [pc, #60]	; 6b68 <hash_init+0x90>
    6b28:	e5961000 	ldr	r1, [r6]
    6b2c:	e59f302c 	ldr	r3, [pc, #44]	; 6b60 <hash_init+0x88>
    6b30:	e1a0e00f 	mov	lr, pc
    6b34:	e593f000 	ldr	pc, [r3]
    6b38:	e1a00005 	mov	r0, r5
    6b3c:	ea000004 	b	6b54 <hash_init+0x7c>
    6b40:	e59f3024 	ldr	r3, [pc, #36]	; 6b6c <hash_init+0x94>
    6b44:	e1a00007 	mov	r0, r7
    6b48:	e0833204 	add	r3, r3, r4, lsl #4
    6b4c:	e1a0e00f 	mov	lr, pc
    6b50:	e593f004 	ldr	pc, [r3, #4]
    6b54:	e28dd004 	add	sp, sp, #4	; 0x4
    6b58:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6b5c:	00017c1f 	.word	0x00017c1f
    6b60:	3c00d6cc 	.word	0x3c00d6cc
    6b64:	3c00d17c 	.word	0x3c00d17c
    6b68:	00017bf3 	.word	0x00017bf3
    6b6c:	00015884 	.word	0x00015884

00006b70 <md5_value>:
    6b70:	e92d4030 	push	{r4, r5, lr}
    6b74:	e59f3054 	ldr	r3, [pc, #84]	; 6bd0 <md5_value+0x60>
    6b78:	e1a05000 	mov	r5, r0
    6b7c:	e893000c 	ldm	r3, {r2, r3}
    6b80:	e1823003 	orr	r3, r2, r3
    6b84:	e24dd024 	sub	sp, sp, #36	; 0x24
    6b88:	e3530000 	cmp	r3, #0	; 0x0
    6b8c:	13e00000 	mvnne	r0, #0	; 0x0
    6b90:	1a00000c 	bne	6bc8 <md5_value+0x58>
    6b94:	e1a0000d 	mov	r0, sp
    6b98:	eb00016d 	bl	7154 <hash_module_get_result>
    6b9c:	e3500000 	cmp	r0, #0	; 0x0
    6ba0:	e1a0400d 	mov	r4, sp
    6ba4:	13e00001 	mvnne	r0, #1	; 0x1
    6ba8:	1a000006 	bne	6bc8 <md5_value+0x58>
    6bac:	e1a0200d 	mov	r2, sp
    6bb0:	e7d23000 	ldrb	r3, [r2, r0]
    6bb4:	e7c53000 	strb	r3, [r5, r0]
    6bb8:	e2800001 	add	r0, r0, #1	; 0x1
    6bbc:	e3500010 	cmp	r0, #16	; 0x10
    6bc0:	1afffffa 	bne	6bb0 <md5_value+0x40>
    6bc4:	e3a00000 	mov	r0, #0	; 0x0
    6bc8:	e28dd024 	add	sp, sp, #36	; 0x24
    6bcc:	e8bd8030 	pop	{r4, r5, pc}
    6bd0:	3c00d180 	.word	0x3c00d180

00006bd4 <md5_data>:
    6bd4:	e59f305c 	ldr	r3, [pc, #92]	; 6c38 <md5_data+0x64>
    6bd8:	e92d4070 	push	{r4, r5, r6, lr}
    6bdc:	e1a06001 	mov	r6, r1
    6be0:	e5931004 	ldr	r1, [r3, #4]
    6be4:	e1a04006 	mov	r4, r6
    6be8:	e3a05000 	mov	r5, #0	; 0x0
    6bec:	e1550001 	cmp	r5, r1
    6bf0:	e593c000 	ldr	ip, [r3]
    6bf4:	8a000002 	bhi	6c04 <md5_data+0x30>
    6bf8:	1a000003 	bne	6c0c <md5_data+0x38>
    6bfc:	e154000c 	cmp	r4, ip
    6c00:	9a000001 	bls	6c0c <md5_data+0x38>
    6c04:	e3e00000 	mvn	r0, #0	; 0x0
    6c08:	e8bd8070 	pop	{r4, r5, r6, pc}
    6c0c:	e1a0200c 	mov	r2, ip
    6c10:	e1a03001 	mov	r3, r1
    6c14:	e59fc01c 	ldr	ip, [pc, #28]	; 6c38 <md5_data+0x64>
    6c18:	e0522004 	subs	r2, r2, r4
    6c1c:	e0c33005 	sbc	r3, r3, r5
    6c20:	e1a01006 	mov	r1, r6
    6c24:	e88c000c 	stm	ip, {r2, r3}
    6c28:	eb0001ac 	bl	72e0 <hash_dma_transfer>
    6c2c:	e3500000 	cmp	r0, #0	; 0x0
    6c30:	13e00001 	mvnne	r0, #1	; 0x1
    6c34:	e8bd8070 	pop	{r4, r5, r6, pc}
    6c38:	3c00d180 	.word	0x3c00d180

00006c3c <md5_init>:
    6c3c:	e1a03ea0 	lsr	r3, r0, #29
    6c40:	e1a0c180 	lsl	ip, r0, #3
    6c44:	e92d4010 	push	{r4, lr}
    6c48:	e59f403c 	ldr	r4, [pc, #60]	; 6c8c <md5_init+0x50>
    6c4c:	e1a0200c 	mov	r2, ip
    6c50:	e1a01003 	mov	r1, r3
    6c54:	e3a00000 	mov	r0, #0	; 0x0
    6c58:	e5843004 	str	r3, [r4, #4]
    6c5c:	e584c000 	str	ip, [r4]
    6c60:	eb000182 	bl	7270 <hash_module_init>
    6c64:	e3500000 	cmp	r0, #0	; 0x0
    6c68:	05942000 	ldreq	r2, [r4]
    6c6c:	05943004 	ldreq	r3, [r4, #4]
    6c70:	01a021a2 	lsreq	r2, r2, #3
    6c74:	01822e83 	orreq	r2, r2, r3, lsl #29
    6c78:	01a031a3 	lsreq	r3, r3, #3
    6c7c:	13e00000 	mvnne	r0, #0	; 0x0
    6c80:	05843004 	streq	r3, [r4, #4]
    6c84:	05842000 	streq	r2, [r4]
    6c88:	e8bd8010 	pop	{r4, pc}
    6c8c:	3c00d180 	.word	0x3c00d180

00006c90 <dma_start>:
    6c90:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6c94:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6c98:	e1a00400 	lsl	r0, r0, #8
    6c9c:	e3a03001 	mov	r3, #1	; 0x1
    6ca0:	e5803000 	str	r3, [r0]
    6ca4:	e12fff1e 	bx	lr

00006ca8 <dma_get_status>:
    6ca8:	e3a0c001 	mov	ip, #1	; 0x1
    6cac:	e1a0211c 	lsl	r2, ip, r1
    6cb0:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6cb4:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6cb8:	e1a01400 	lsl	r1, r0, #8
    6cbc:	e5913024 	ldr	r3, [r1, #36]
    6cc0:	e0130002 	ands	r0, r3, r2
    6cc4:	15812024 	strne	r2, [r1, #36]
    6cc8:	11a0000c 	movne	r0, ip
    6ccc:	e12fff1e 	bx	lr

00006cd0 <dma_sdma_init>:
    6cd0:	e280c823 	add	ip, r0, #2293760	; 0x230000
    6cd4:	e28cca02 	add	ip, ip, #8192	; 0x2000
    6cd8:	e1a0040c 	lsl	r0, ip, #8
    6cdc:	e2800004 	add	r0, r0, #4	; 0x4
    6ce0:	e5903000 	ldr	r3, [r0]
    6ce4:	e3520801 	cmp	r2, #65536	; 0x10000
    6ce8:	e3833001 	orr	r3, r3, #1	; 0x1
    6cec:	e5803000 	str	r3, [r0]
    6cf0:	e92d4010 	push	{r4, lr}
    6cf4:	e3a03341 	mov	r3, #67108865	; 0x4000001
    6cf8:	e1a0440c 	lsl	r4, ip, #8
    6cfc:	e584300c 	str	r3, [r4, #12]
    6d00:	31a02802 	lslcc	r2, r2, #16
    6d04:	31a0340c 	lslcc	r3, ip, #8
    6d08:	31a02822 	lsrcc	r2, r2, #16
    6d0c:	32833010 	addcc	r3, r3, #16	; 0x10
    6d10:	35832000 	strcc	r2, [r3]
    6d14:	23a00001 	movcs	r0, #1	; 0x1
    6d18:	35841014 	strcc	r1, [r4, #20]
    6d1c:	33a00000 	movcc	r0, #0	; 0x0
    6d20:	e8bd8010 	pop	{r4, pc}

00006d24 <security_cke_control>:
    6d24:	e59fc014 	ldr	ip, [pc, #20]	; 6d40 <security_cke_control+0x1c>
    6d28:	e3a02001 	mov	r2, #1	; 0x1
    6d2c:	e59c3000 	ldr	r3, [ip]
    6d30:	e1c33012 	bic	r3, r3, r2, lsl r0
    6d34:	e1833011 	orr	r3, r3, r1, lsl r0
    6d38:	e58c3000 	str	r3, [ip]
    6d3c:	e12fff1e 	bx	lr
    6d40:	23202000 	.word	0x23202000

00006d44 <security_enable>:
    6d44:	e59f3008 	ldr	r3, [pc, #8]	; 6d54 <security_enable+0x10>
    6d48:	e3a02001 	mov	r2, #1	; 0x1
    6d4c:	e5832000 	str	r2, [r3]
    6d50:	e12fff1e 	bx	lr
    6d54:	3c00d188 	.word	0x3c00d188

00006d58 <security_enabled>:
    6d58:	e59f300c 	ldr	r3, [pc, #12]	; 6d6c <security_enabled+0x14>
    6d5c:	e5930000 	ldr	r0, [r3]
    6d60:	e2500000 	subs	r0, r0, #0	; 0x0
    6d64:	13a00001 	movne	r0, #1	; 0x1
    6d68:	e12fff1e 	bx	lr
    6d6c:	3c00d188 	.word	0x3c00d188

00006d70 <dma_reset>:
    6d70:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6d74:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6d78:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6d7c:	e1a07400 	lsl	r7, r0, #8
    6d80:	e3a03002 	mov	r3, #2	; 0x2
    6d84:	e24dd004 	sub	sp, sp, #4	; 0x4
    6d88:	e5873000 	str	r3, [r7]
    6d8c:	eb001b01 	bl	d998 <get_ticks>
    6d90:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    6d94:	e3a05000 	mov	r5, #0	; 0x0
    6d98:	e0944000 	adds	r4, r4, r0
    6d9c:	e0a55001 	adc	r5, r5, r1
    6da0:	e5976000 	ldr	r6, [r7]
    6da4:	ea000008 	b	6dcc <dma_reset+0x5c>
    6da8:	e5976000 	ldr	r6, [r7]
    6dac:	eb001af9 	bl	d998 <get_ticks>
    6db0:	e1510005 	cmp	r1, r5
    6db4:	8a000002 	bhi	6dc4 <dma_reset+0x54>
    6db8:	1a000003 	bne	6dcc <dma_reset+0x5c>
    6dbc:	e1500004 	cmp	r0, r4
    6dc0:	9a000001 	bls	6dcc <dma_reset+0x5c>
    6dc4:	e3a00001 	mov	r0, #1	; 0x1
    6dc8:	ea000002 	b	6dd8 <dma_reset+0x68>
    6dcc:	e3560000 	cmp	r6, #0	; 0x0
    6dd0:	1afffff4 	bne	6da8 <dma_reset+0x38>
    6dd4:	e1a00006 	mov	r0, r6
    6dd8:	e28dd004 	add	sp, sp, #4	; 0x4
    6ddc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00006de0 <sha1_value>:
    6de0:	e92d4030 	push	{r4, r5, lr}
    6de4:	e59f3054 	ldr	r3, [pc, #84]	; 6e40 <sha1_value+0x60>
    6de8:	e1a05000 	mov	r5, r0
    6dec:	e893000c 	ldm	r3, {r2, r3}
    6df0:	e1823003 	orr	r3, r2, r3
    6df4:	e24dd024 	sub	sp, sp, #36	; 0x24
    6df8:	e3530000 	cmp	r3, #0	; 0x0
    6dfc:	13e00000 	mvnne	r0, #0	; 0x0
    6e00:	1a00000c 	bne	6e38 <sha1_value+0x58>
    6e04:	e1a0000d 	mov	r0, sp
    6e08:	eb0000d1 	bl	7154 <hash_module_get_result>
    6e0c:	e3500000 	cmp	r0, #0	; 0x0
    6e10:	e1a0400d 	mov	r4, sp
    6e14:	13e00001 	mvnne	r0, #1	; 0x1
    6e18:	1a000006 	bne	6e38 <sha1_value+0x58>
    6e1c:	e1a0200d 	mov	r2, sp
    6e20:	e7d23000 	ldrb	r3, [r2, r0]
    6e24:	e7c53000 	strb	r3, [r5, r0]
    6e28:	e2800001 	add	r0, r0, #1	; 0x1
    6e2c:	e3500014 	cmp	r0, #20	; 0x14
    6e30:	1afffffa 	bne	6e20 <sha1_value+0x40>
    6e34:	e3a00000 	mov	r0, #0	; 0x0
    6e38:	e28dd024 	add	sp, sp, #36	; 0x24
    6e3c:	e8bd8030 	pop	{r4, r5, pc}
    6e40:	3c00d190 	.word	0x3c00d190

00006e44 <sha1_data>:
    6e44:	e59f305c 	ldr	r3, [pc, #92]	; 6ea8 <sha1_data+0x64>
    6e48:	e92d4070 	push	{r4, r5, r6, lr}
    6e4c:	e1a06001 	mov	r6, r1
    6e50:	e5931004 	ldr	r1, [r3, #4]
    6e54:	e1a04006 	mov	r4, r6
    6e58:	e3a05000 	mov	r5, #0	; 0x0
    6e5c:	e1550001 	cmp	r5, r1
    6e60:	e593c000 	ldr	ip, [r3]
    6e64:	8a000002 	bhi	6e74 <sha1_data+0x30>
    6e68:	1a000003 	bne	6e7c <sha1_data+0x38>
    6e6c:	e154000c 	cmp	r4, ip
    6e70:	9a000001 	bls	6e7c <sha1_data+0x38>
    6e74:	e3e00000 	mvn	r0, #0	; 0x0
    6e78:	e8bd8070 	pop	{r4, r5, r6, pc}
    6e7c:	e1a0200c 	mov	r2, ip
    6e80:	e1a03001 	mov	r3, r1
    6e84:	e59fc01c 	ldr	ip, [pc, #28]	; 6ea8 <sha1_data+0x64>
    6e88:	e0522004 	subs	r2, r2, r4
    6e8c:	e0c33005 	sbc	r3, r3, r5
    6e90:	e1a01006 	mov	r1, r6
    6e94:	e88c000c 	stm	ip, {r2, r3}
    6e98:	eb000110 	bl	72e0 <hash_dma_transfer>
    6e9c:	e3500000 	cmp	r0, #0	; 0x0
    6ea0:	13e00001 	mvnne	r0, #1	; 0x1
    6ea4:	e8bd8070 	pop	{r4, r5, r6, pc}
    6ea8:	3c00d190 	.word	0x3c00d190

00006eac <sha1_init>:
    6eac:	e1a03ea0 	lsr	r3, r0, #29
    6eb0:	e1a0c180 	lsl	ip, r0, #3
    6eb4:	e92d4010 	push	{r4, lr}
    6eb8:	e59f403c 	ldr	r4, [pc, #60]	; 6efc <sha1_init+0x50>
    6ebc:	e1a0200c 	mov	r2, ip
    6ec0:	e1a01003 	mov	r1, r3
    6ec4:	e3a00001 	mov	r0, #1	; 0x1
    6ec8:	e5843004 	str	r3, [r4, #4]
    6ecc:	e584c000 	str	ip, [r4]
    6ed0:	eb0000e6 	bl	7270 <hash_module_init>
    6ed4:	e3500000 	cmp	r0, #0	; 0x0
    6ed8:	05942000 	ldreq	r2, [r4]
    6edc:	05943004 	ldreq	r3, [r4, #4]
    6ee0:	01a021a2 	lsreq	r2, r2, #3
    6ee4:	01822e83 	orreq	r2, r2, r3, lsl #29
    6ee8:	01a031a3 	lsreq	r3, r3, #3
    6eec:	13e00000 	mvnne	r0, #0	; 0x0
    6ef0:	05843004 	streq	r3, [r4, #4]
    6ef4:	05842000 	streq	r2, [r4]
    6ef8:	e8bd8010 	pop	{r4, pc}
    6efc:	3c00d190 	.word	0x3c00d190

00006f00 <sha256_value>:
    6f00:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6f04:	e59f3028 	ldr	r3, [pc, #40]	; 6f34 <sha256_value+0x34>
    6f08:	e24dd004 	sub	sp, sp, #4	; 0x4
    6f0c:	e893000c 	ldm	r3, {r2, r3}
    6f10:	e1823003 	orr	r3, r2, r3
    6f14:	e3530000 	cmp	r3, #0	; 0x0
    6f18:	13e00000 	mvnne	r0, #0	; 0x0
    6f1c:	1a000002 	bne	6f2c <sha256_value+0x2c>
    6f20:	eb00008b 	bl	7154 <hash_module_get_result>
    6f24:	e3500000 	cmp	r0, #0	; 0x0
    6f28:	13e00001 	mvnne	r0, #1	; 0x1
    6f2c:	e28dd004 	add	sp, sp, #4	; 0x4
    6f30:	e8bd8000 	pop	{pc}
    6f34:	3c00d198 	.word	0x3c00d198

00006f38 <sha256_data>:
    6f38:	e59f305c 	ldr	r3, [pc, #92]	; 6f9c <sha256_data+0x64>
    6f3c:	e92d4070 	push	{r4, r5, r6, lr}
    6f40:	e1a06001 	mov	r6, r1
    6f44:	e5931004 	ldr	r1, [r3, #4]
    6f48:	e1a04006 	mov	r4, r6
    6f4c:	e3a05000 	mov	r5, #0	; 0x0
    6f50:	e1550001 	cmp	r5, r1
    6f54:	e593c000 	ldr	ip, [r3]
    6f58:	8a000002 	bhi	6f68 <sha256_data+0x30>
    6f5c:	1a000003 	bne	6f70 <sha256_data+0x38>
    6f60:	e154000c 	cmp	r4, ip
    6f64:	9a000001 	bls	6f70 <sha256_data+0x38>
    6f68:	e3e00000 	mvn	r0, #0	; 0x0
    6f6c:	e8bd8070 	pop	{r4, r5, r6, pc}
    6f70:	e1a0200c 	mov	r2, ip
    6f74:	e1a03001 	mov	r3, r1
    6f78:	e59fc01c 	ldr	ip, [pc, #28]	; 6f9c <sha256_data+0x64>
    6f7c:	e0522004 	subs	r2, r2, r4
    6f80:	e0c33005 	sbc	r3, r3, r5
    6f84:	e1a01006 	mov	r1, r6
    6f88:	e88c000c 	stm	ip, {r2, r3}
    6f8c:	eb0000d3 	bl	72e0 <hash_dma_transfer>
    6f90:	e3500000 	cmp	r0, #0	; 0x0
    6f94:	13e00001 	mvnne	r0, #1	; 0x1
    6f98:	e8bd8070 	pop	{r4, r5, r6, pc}
    6f9c:	3c00d198 	.word	0x3c00d198

00006fa0 <sha256_init>:
    6fa0:	e1a03ea0 	lsr	r3, r0, #29
    6fa4:	e1a0c180 	lsl	ip, r0, #3
    6fa8:	e92d4010 	push	{r4, lr}
    6fac:	e59f403c 	ldr	r4, [pc, #60]	; 6ff0 <sha256_init+0x50>
    6fb0:	e1a0200c 	mov	r2, ip
    6fb4:	e1a01003 	mov	r1, r3
    6fb8:	e3a00003 	mov	r0, #3	; 0x3
    6fbc:	e5843004 	str	r3, [r4, #4]
    6fc0:	e584c000 	str	ip, [r4]
    6fc4:	eb0000a9 	bl	7270 <hash_module_init>
    6fc8:	e3500000 	cmp	r0, #0	; 0x0
    6fcc:	05942000 	ldreq	r2, [r4]
    6fd0:	05943004 	ldreq	r3, [r4, #4]
    6fd4:	01a021a2 	lsreq	r2, r2, #3
    6fd8:	01822e83 	orreq	r2, r2, r3, lsl #29
    6fdc:	01a031a3 	lsreq	r3, r3, #3
    6fe0:	13e00000 	mvnne	r0, #0	; 0x0
    6fe4:	05843004 	streq	r3, [r4, #4]
    6fe8:	05842000 	streq	r2, [r4]
    6fec:	e8bd8010 	pop	{r4, pc}
    6ff0:	3c00d198 	.word	0x3c00d198

00006ff4 <crc32_init>:
    6ff4:	e59f300c 	ldr	r3, [pc, #12]	; 7008 <crc32_init+0x14>
    6ff8:	e3e02000 	mvn	r2, #0	; 0x0
    6ffc:	e5832000 	str	r2, [r3]
    7000:	e3a00000 	mov	r0, #0	; 0x0
    7004:	e12fff1e 	bx	lr
    7008:	3c00d1a0 	.word	0x3c00d1a0

0000700c <crc32_data>:
    700c:	e59f311c 	ldr	r3, [pc, #284]	; 7130 <crc32_data+0x124>
    7010:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7014:	e593c000 	ldr	ip, [r3]
    7018:	e1a08000 	mov	r8, r0
    701c:	e1a06001 	mov	r6, r1
    7020:	e1a04000 	mov	r4, r0
    7024:	e1a07001 	mov	r7, r1
    7028:	e59f5104 	ldr	r5, [pc, #260]	; 7134 <crc32_data+0x128>
    702c:	ea000027 	b	70d0 <crc32_data+0xc4>
    7030:	e5543008 	ldrb	r3, [r4, #-8]
    7034:	e5542007 	ldrb	r2, [r4, #-7]
    7038:	e02c3003 	eor	r3, ip, r3
    703c:	e20330ff 	and	r3, r3, #255	; 0xff
    7040:	e7953103 	ldr	r3, [r5, r3, lsl #2]
    7044:	e5541006 	ldrb	r1, [r4, #-6]
    7048:	e023342c 	eor	r3, r3, ip, lsr #8
    704c:	e0232002 	eor	r2, r3, r2
    7050:	e20220ff 	and	r2, r2, #255	; 0xff
    7054:	e7952102 	ldr	r2, [r5, r2, lsl #2]
    7058:	e5540005 	ldrb	r0, [r4, #-5]
    705c:	e0222423 	eor	r2, r2, r3, lsr #8
    7060:	e0221001 	eor	r1, r2, r1
    7064:	e20110ff 	and	r1, r1, #255	; 0xff
    7068:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    706c:	e5541004 	ldrb	r1, [r4, #-4]
    7070:	e0233422 	eor	r3, r3, r2, lsr #8
    7074:	e0230000 	eor	r0, r3, r0
    7078:	e20000ff 	and	r0, r0, #255	; 0xff
    707c:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    7080:	e5540003 	ldrb	r0, [r4, #-3]
    7084:	e0222423 	eor	r2, r2, r3, lsr #8
    7088:	e0221001 	eor	r1, r2, r1
    708c:	e20110ff 	and	r1, r1, #255	; 0xff
    7090:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    7094:	e554c002 	ldrb	ip, [r4, #-2]
    7098:	e0233422 	eor	r3, r3, r2, lsr #8
    709c:	e0230000 	eor	r0, r3, r0
    70a0:	e20000ff 	and	r0, r0, #255	; 0xff
    70a4:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    70a8:	e5541001 	ldrb	r1, [r4, #-1]
    70ac:	e0222423 	eor	r2, r2, r3, lsr #8
    70b0:	e022c00c 	eor	ip, r2, ip
    70b4:	e20cc0ff 	and	ip, ip, #255	; 0xff
    70b8:	e795310c 	ldr	r3, [r5, ip, lsl #2]
    70bc:	e0233422 	eor	r3, r3, r2, lsr #8
    70c0:	e0231001 	eor	r1, r3, r1
    70c4:	e20110ff 	and	r1, r1, #255	; 0xff
    70c8:	e7952101 	ldr	r2, [r5, r1, lsl #2]
    70cc:	e022c423 	eor	ip, r2, r3, lsr #8
    70d0:	e3570007 	cmp	r7, #7	; 0x7
    70d4:	e2844008 	add	r4, r4, #8	; 0x8
    70d8:	e2477008 	sub	r7, r7, #8	; 0x8
    70dc:	8affffd3 	bhi	7030 <crc32_data+0x24>
    70e0:	e3c63007 	bic	r3, r6, #7	; 0x7
    70e4:	e0880003 	add	r0, r8, r3
    70e8:	e59f3040 	ldr	r3, [pc, #64]	; 7130 <crc32_data+0x124>
    70ec:	e2161007 	ands	r1, r6, #7	; 0x7
    70f0:	e583c000 	str	ip, [r3]
    70f4:	0a00000b 	beq	7128 <crc32_data+0x11c>
    70f8:	e59f4034 	ldr	r4, [pc, #52]	; 7134 <crc32_data+0x128>
    70fc:	e3a02000 	mov	r2, #0	; 0x0
    7100:	e7d03002 	ldrb	r3, [r0, r2]
    7104:	e2822001 	add	r2, r2, #1	; 0x1
    7108:	e02c3003 	eor	r3, ip, r3
    710c:	e20330ff 	and	r3, r3, #255	; 0xff
    7110:	e7943103 	ldr	r3, [r4, r3, lsl #2]
    7114:	e1510002 	cmp	r1, r2
    7118:	e023c42c 	eor	ip, r3, ip, lsr #8
    711c:	1afffff7 	bne	7100 <crc32_data+0xf4>
    7120:	e59f3008 	ldr	r3, [pc, #8]	; 7130 <crc32_data+0x124>
    7124:	e583c000 	str	ip, [r3]
    7128:	e3a00000 	mov	r0, #0	; 0x0
    712c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    7130:	3c00d1a0 	.word	0x3c00d1a0
    7134:	000158c4 	.word	0x000158c4

00007138 <crc32_value>:
    7138:	e59f3010 	ldr	r3, [pc, #16]	; 7150 <crc32_value+0x18>
    713c:	e5933000 	ldr	r3, [r3]
    7140:	e1e03003 	mvn	r3, r3
    7144:	e5803000 	str	r3, [r0]
    7148:	e3a00000 	mov	r0, #0	; 0x0
    714c:	e12fff1e 	bx	lr
    7150:	3c00d1a0 	.word	0x3c00d1a0

00007154 <hash_module_get_result>:
    7154:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7158:	e1a08000 	mov	r8, r0
    715c:	eb001a0d 	bl	d998 <get_ticks>
    7160:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    7164:	e59f3088 	ldr	r3, [pc, #136]	; 71f4 <hash_module_get_result+0xa0>
    7168:	e3a05000 	mov	r5, #0	; 0x0
    716c:	e0944000 	adds	r4, r4, r0
    7170:	e0a55001 	adc	r5, r5, r1
    7174:	e1a07003 	mov	r7, r3
    7178:	e593680c 	ldr	r6, [r3, #2060]
    717c:	ea000008 	b	71a4 <hash_module_get_result+0x50>
    7180:	e597680c 	ldr	r6, [r7, #2060]
    7184:	eb001a03 	bl	d998 <get_ticks>
    7188:	e1510005 	cmp	r1, r5
    718c:	8a000002 	bhi	719c <hash_module_get_result+0x48>
    7190:	1a000003 	bne	71a4 <hash_module_get_result+0x50>
    7194:	e1500004 	cmp	r0, r4
    7198:	9a000001 	bls	71a4 <hash_module_get_result+0x50>
    719c:	e3a00001 	mov	r0, #1	; 0x1
    71a0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    71a4:	e3160001 	tst	r6, #1	; 0x1
    71a8:	0afffff4 	beq	7180 <hash_module_get_result+0x2c>
    71ac:	e59f0044 	ldr	r0, [pc, #68]	; 71f8 <hash_module_get_result+0xa4>
    71b0:	e59fc044 	ldr	ip, [pc, #68]	; 71fc <hash_module_get_result+0xa8>
    71b4:	e1a03000 	mov	r3, r0
    71b8:	e6931008 	ldr	r1, [r3], r8
    71bc:	e59f203c 	ldr	r2, [pc, #60]	; 7200 <hash_module_get_result+0xac>
    71c0:	e2800004 	add	r0, r0, #4	; 0x4
    71c4:	e0832002 	add	r2, r3, r2
    71c8:	e150000c 	cmp	r0, ip
    71cc:	e5821000 	str	r1, [r2]
    71d0:	1afffff7 	bne	71b4 <hash_module_get_result+0x60>
    71d4:	e59f2018 	ldr	r2, [pc, #24]	; 71f4 <hash_module_get_result+0xa0>
    71d8:	e3a03001 	mov	r3, #1	; 0x1
    71dc:	e582380c 	str	r3, [r2, #2060]
    71e0:	e5923800 	ldr	r3, [r2, #2048]
    71e4:	e3a00000 	mov	r0, #0	; 0x0
    71e8:	e3c33080 	bic	r3, r3, #128	; 0x80
    71ec:	e5823800 	str	r3, [r2, #2048]
    71f0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    71f4:	23200000 	.word	0x23200000
    71f8:	23200858 	.word	0x23200858
    71fc:	23200878 	.word	0x23200878
    7200:	dcdff7a8 	.word	0xdcdff7a8

00007204 <hash_module_clear>:
    7204:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    7208:	e59f405c 	ldr	r4, [pc, #92]	; 726c <hash_module_clear+0x68>
    720c:	e3a03002 	mov	r3, #2	; 0x2
    7210:	e5843800 	str	r3, [r4, #2048]
    7214:	e24dd004 	sub	sp, sp, #4	; 0x4
    7218:	eb0019de 	bl	d998 <get_ticks>
    721c:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    7220:	e3a07000 	mov	r7, #0	; 0x0
    7224:	e0966000 	adds	r6, r6, r0
    7228:	e1a05004 	mov	r5, r4
    722c:	e0a77001 	adc	r7, r7, r1
    7230:	e5944800 	ldr	r4, [r4, #2048]
    7234:	ea000008 	b	725c <hash_module_clear+0x58>
    7238:	e5954800 	ldr	r4, [r5, #2048]
    723c:	eb0019d5 	bl	d998 <get_ticks>
    7240:	e1510007 	cmp	r1, r7
    7244:	8a000002 	bhi	7254 <hash_module_clear+0x50>
    7248:	1a000003 	bne	725c <hash_module_clear+0x58>
    724c:	e1500006 	cmp	r0, r6
    7250:	9a000001 	bls	725c <hash_module_clear+0x58>
    7254:	e3a00001 	mov	r0, #1	; 0x1
    7258:	ea000001 	b	7264 <hash_module_clear+0x60>
    725c:	e2140002 	ands	r0, r4, #2	; 0x2
    7260:	1afffff4 	bne	7238 <hash_module_clear+0x34>
    7264:	e28dd004 	add	sp, sp, #4	; 0x4
    7268:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    726c:	23200000 	.word	0x23200000

00007270 <hash_module_init>:
    7270:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    7274:	e59f4060 	ldr	r4, [pc, #96]	; 72dc <hash_module_init+0x6c>
    7278:	e24dd004 	sub	sp, sp, #4	; 0x4
    727c:	e5943800 	ldr	r3, [r4, #2048]
    7280:	e1a05000 	mov	r5, r0
    7284:	e3c33080 	bic	r3, r3, #128	; 0x80
    7288:	e5843800 	str	r3, [r4, #2048]
    728c:	e1a07001 	mov	r7, r1
    7290:	e1a06002 	mov	r6, r2
    7294:	ebffffda 	bl	7204 <hash_module_clear>
    7298:	e3500000 	cmp	r0, #0	; 0x0
    729c:	13a00001 	movne	r0, #1	; 0x1
    72a0:	1a00000b 	bne	72d4 <hash_module_init+0x64>
    72a4:	e3a03001 	mov	r3, #1	; 0x1
    72a8:	e1a02185 	lsl	r2, r5, #3
    72ac:	e5843804 	str	r3, [r4, #2052]
    72b0:	e5842800 	str	r2, [r4, #2048]
    72b4:	e5847814 	str	r7, [r4, #2068]
    72b8:	e5846810 	str	r6, [r4, #2064]
    72bc:	e5943800 	ldr	r3, [r4, #2048]
    72c0:	e3833080 	orr	r3, r3, #128	; 0x80
    72c4:	e5843800 	str	r3, [r4, #2048]
    72c8:	e5943800 	ldr	r3, [r4, #2048]
    72cc:	e3833004 	orr	r3, r3, #4	; 0x4
    72d0:	e5843800 	str	r3, [r4, #2048]
    72d4:	e28dd004 	add	sp, sp, #4	; 0x4
    72d8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    72dc:	23200000 	.word	0x23200000

000072e0 <hash_dma_transfer>:
    72e0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    72e4:	e1a08001 	mov	r8, r1
    72e8:	e24dd004 	sub	sp, sp, #4	; 0x4
    72ec:	e1a09000 	mov	r9, r0
    72f0:	e1a06001 	mov	r6, r1
    72f4:	e59fa090 	ldr	sl, [pc, #144]	; 738c <hash_dma_transfer+0xac>
    72f8:	e3a0b000 	mov	fp, #0	; 0x0
    72fc:	ea00001b 	b	7370 <hash_dma_transfer+0x90>
    7300:	e3560902 	cmp	r6, #32768	; 0x8000
    7304:	b1a07006 	movlt	r7, r6
    7308:	a3a07902 	movge	r7, #32768	; 0x8000
    730c:	e0661008 	rsb	r1, r6, r8
    7310:	e0891001 	add	r1, r9, r1
    7314:	e3a00002 	mov	r0, #2	; 0x2
    7318:	e1a02007 	mov	r2, r7
    731c:	ebfffe6b 	bl	6cd0 <dma_sdma_init>
    7320:	e3500000 	cmp	r0, #0	; 0x0
    7324:	1a000015 	bne	7380 <hash_dma_transfer+0xa0>
    7328:	e2800002 	add	r0, r0, #2	; 0x2
    732c:	ebfffe57 	bl	6c90 <dma_start>
    7330:	eb001998 	bl	d998 <get_ticks>
    7334:	e090400a 	adds	r4, r0, sl
    7338:	e0a1500b 	adc	r5, r1, fp
    733c:	ea000005 	b	7358 <hash_dma_transfer+0x78>
    7340:	eb001994 	bl	d998 <get_ticks>
    7344:	e1510005 	cmp	r1, r5
    7348:	8a00000c 	bhi	7380 <hash_dma_transfer+0xa0>
    734c:	1a000001 	bne	7358 <hash_dma_transfer+0x78>
    7350:	e1500004 	cmp	r0, r4
    7354:	8a000009 	bhi	7380 <hash_dma_transfer+0xa0>
    7358:	e3a01003 	mov	r1, #3	; 0x3
    735c:	e3a00002 	mov	r0, #2	; 0x2
    7360:	ebfffe50 	bl	6ca8 <dma_get_status>
    7364:	e3500000 	cmp	r0, #0	; 0x0
    7368:	0afffff4 	beq	7340 <hash_dma_transfer+0x60>
    736c:	e0676006 	rsb	r6, r7, r6
    7370:	e3560000 	cmp	r6, #0	; 0x0
    7374:	1affffe1 	bne	7300 <hash_dma_transfer+0x20>
    7378:	e1a00006 	mov	r0, r6
    737c:	ea000000 	b	7384 <hash_dma_transfer+0xa4>
    7380:	e3a00001 	mov	r0, #1	; 0x1
    7384:	e28dd004 	add	sp, sp, #4	; 0x4
    7388:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    738c:	000f4240 	.word	0x000f4240

00007390 <cdbg_log_toggle>:
    7390:	e59f3014 	ldr	r3, [pc, #20]	; 73ac <cdbg_log_toggle+0x1c>
    7394:	e3500000 	cmp	r0, #0	; 0x0
    7398:	e59f2010 	ldr	r2, [pc, #16]	; 73b0 <cdbg_log_toggle+0x20>
    739c:	01a02003 	moveq	r2, r3
    73a0:	e59f300c 	ldr	r3, [pc, #12]	; 73b4 <cdbg_log_toggle+0x24>
    73a4:	e5832000 	str	r2, [r3]
    73a8:	e12fff1e 	bx	lr
    73ac:	000126e0 	.word	0x000126e0
    73b0:	00012764 	.word	0x00012764
    73b4:	3c00d6cc 	.word	0x3c00d6cc

000073b8 <cdbg_wipe_bootarea>:
    73b8:	e59f3018 	ldr	r3, [pc, #24]	; 73d8 <cdbg_wipe_bootarea+0x20>
    73bc:	e59f1018 	ldr	r1, [pc, #24]	; 73dc <cdbg_wipe_bootarea+0x24>
    73c0:	e59f2018 	ldr	r2, [pc, #24]	; 73e0 <cdbg_wipe_bootarea+0x28>
    73c4:	e5031004 	str	r1, [r3, #-4]
    73c8:	e2833004 	add	r3, r3, #4	; 0x4
    73cc:	e1530002 	cmp	r3, r2
    73d0:	1afffffb 	bne	73c4 <cdbg_wipe_bootarea+0xc>
    73d4:	e12fff1e 	bx	lr
    73d8:	3c000004 	.word	0x3c000004
    73dc:	eafffffe 	.word	0xeafffffe
    73e0:	3c008004 	.word	0x3c008004

000073e4 <cdbg_verify_burn>:
    73e4:	e59f3004 	ldr	r3, [pc, #4]	; 73f0 <cdbg_verify_burn+0xc>
    73e8:	e5930000 	ldr	r0, [r3]
    73ec:	e12fff1e 	bx	lr
    73f0:	3c00d1a4 	.word	0x3c00d1a4

000073f4 <cdbg_verify_burn_key>:
    73f4:	e59f0000 	ldr	r0, [pc, #0]	; 73fc <cdbg_verify_burn_key+0x8>
    73f8:	e12fff1e 	bx	lr
    73fc:	3c00d1a8 	.word	0x3c00d1a8

00007400 <cdbg_verify_burn_enable>:
    7400:	e3500000 	cmp	r0, #0	; 0x0
    7404:	e59f3018 	ldr	r3, [pc, #24]	; 7424 <cdbg_verify_burn_enable+0x24>
    7408:	03a02000 	moveq	r2, #0	; 0x0
    740c:	13a02001 	movne	r2, #1	; 0x1
    7410:	e5832000 	str	r2, [r3]
    7414:	012fff1e 	bxeq	lr
    7418:	e59f0008 	ldr	r0, [pc, #8]	; 7428 <cdbg_verify_burn_enable+0x28>
    741c:	e3a02020 	mov	r2, #32	; 0x20
    7420:	eaffe84b 	b	1554 <memcpy>
    7424:	3c00d1a4 	.word	0x3c00d1a4
    7428:	3c00d1a8 	.word	0x3c00d1a8

0000742c <cdbg_jump>:
    742c:	e92d4030 	push	{r4, r5, lr}
    7430:	e1a04000 	mov	r4, r0
    7434:	e24dd004 	sub	sp, sp, #4	; 0x4
    7438:	e1a05001 	mov	r5, r1
    743c:	ebfffc6c 	bl	65f4 <ss_lock_isk>
    7440:	e59f001c 	ldr	r0, [pc, #28]	; 7464 <cdbg_jump+0x38>
    7444:	e1a01004 	mov	r1, r4
    7448:	e59f3018 	ldr	r3, [pc, #24]	; 7468 <cdbg_jump+0x3c>
    744c:	e1a0e00f 	mov	lr, pc
    7450:	e593f000 	ldr	pc, [r3]
    7454:	e3000bb8 	movw	r0, #3000	; 0xbb8
    7458:	eb00195f 	bl	d9dc <udelay>
    745c:	e1a00005 	mov	r0, r5
    7460:	e12fff34 	blx	r4
    7464:	00017c60 	.word	0x00017c60
    7468:	3c00d6cc 	.word	0x3c00d6cc

0000746c <cdbg_restore_os>:
    746c:	e92d4010 	push	{r4, lr}
    7470:	ebffe4f7 	bl	854 <get_wakeup_entry>
    7474:	e59f301c 	ldr	r3, [pc, #28]	; 7498 <cdbg_restore_os+0x2c>
    7478:	e1a04000 	mov	r4, r0
    747c:	e59f0018 	ldr	r0, [pc, #24]	; 749c <cdbg_restore_os+0x30>
    7480:	e1a0e00f 	mov	lr, pc
    7484:	e593f000 	ldr	pc, [r3]
    7488:	e1a00004 	mov	r0, r4
    748c:	e3a01000 	mov	r1, #0	; 0x0
    7490:	e8bd4010 	pop	{r4, lr}
    7494:	eaffffe4 	b	742c <cdbg_jump>
    7498:	3c00d6cc 	.word	0x3c00d6cc
    749c:	00017c70 	.word	0x00017c70

000074a0 <cdbg_get_pp>:
    74a0:	e92d4070 	push	{r4, r5, r6, lr}
    74a4:	e24dd008 	sub	sp, sp, #8	; 0x8
    74a8:	e1a0000d 	mov	r0, sp
    74ac:	e59f103c 	ldr	r1, [pc, #60]	; 74f0 <cdbg_get_pp+0x50>
    74b0:	e3a02008 	mov	r2, #8	; 0x8
    74b4:	ebffe826 	bl	1554 <memcpy>
    74b8:	e1a0400d 	mov	r4, sp
    74bc:	e1a0600d 	mov	r6, sp
    74c0:	ea000004 	b	74d8 <cdbg_get_pp+0x38>
    74c4:	eb002cc4 	bl	127dc <getc>
    74c8:	e1500004 	cmp	r0, r4
    74cc:	e2854001 	add	r4, r5, #1	; 0x1
    74d0:	e1a05006 	mov	r5, r6
    74d4:	1a000000 	bne	74dc <cdbg_get_pp+0x3c>
    74d8:	e1a05004 	mov	r5, r4
    74dc:	e5d54000 	ldrb	r4, [r5]
    74e0:	e3540000 	cmp	r4, #0	; 0x0
    74e4:	1afffff6 	bne	74c4 <cdbg_get_pp+0x24>
    74e8:	e28dd008 	add	sp, sp, #8	; 0x8
    74ec:	e8bd8070 	pop	{r4, r5, r6, pc}
    74f0:	00015f7e 	.word	0x00015f7e

000074f4 <cdbg_shell>:
    74f4:	e92d4010 	push	{r4, lr}
    74f8:	ebfffc3d 	bl	65f4 <ss_lock_isk>
    74fc:	e59f3048 	ldr	r3, [pc, #72]	; 754c <cdbg_shell+0x58>
    7500:	e59f1048 	ldr	r1, [pc, #72]	; 7550 <cdbg_shell+0x5c>
    7504:	e59f2048 	ldr	r2, [pc, #72]	; 7554 <cdbg_shell+0x60>
    7508:	e5031004 	str	r1, [r3, #-4]
    750c:	e2833004 	add	r3, r3, #4	; 0x4
    7510:	e1530002 	cmp	r3, r2
    7514:	1afffffb 	bne	7508 <cdbg_shell+0x14>
    7518:	e3a00001 	mov	r0, #1	; 0x1
    751c:	eb003488 	bl	14744 <ss_jtag_en>
    7520:	e59f4030 	ldr	r4, [pc, #48]	; 7558 <cdbg_shell+0x64>
    7524:	e59f3030 	ldr	r3, [pc, #48]	; 755c <cdbg_shell+0x68>
    7528:	e59f0030 	ldr	r0, [pc, #48]	; 7560 <cdbg_shell+0x6c>
    752c:	e5843000 	str	r3, [r4]
    7530:	eb002c8b 	bl	12764 <printf_on>
    7534:	ebffffd9 	bl	74a0 <cdbg_get_pp>
    7538:	e59f0024 	ldr	r0, [pc, #36]	; 7564 <cdbg_shell+0x70>
    753c:	e1a0e00f 	mov	lr, pc
    7540:	e594f000 	ldr	pc, [r4]
    7544:	eb002f8f 	bl	13388 <main_loop>
    7548:	eafffffd 	b	7544 <cdbg_shell+0x50>
    754c:	3c000004 	.word	0x3c000004
    7550:	eafffffe 	.word	0xeafffffe
    7554:	3c008004 	.word	0x3c008004
    7558:	3c00d6cc 	.word	0x3c00d6cc
    755c:	00012764 	.word	0x00012764
    7560:	00017c7f 	.word	0x00017c7f
    7564:	00017c84 	.word	0x00017c84

00007568 <cdbg_do_config>:
    7568:	e92d4070 	push	{r4, r5, r6, lr}
    756c:	e59f30ec 	ldr	r3, [pc, #236]	; 7660 <cdbg_do_config+0xf8>
    7570:	e1a04000 	mov	r4, r0
    7574:	e1a06001 	mov	r6, r1
    7578:	e59f00e4 	ldr	r0, [pc, #228]	; 7664 <cdbg_do_config+0xfc>
    757c:	e59f10e4 	ldr	r1, [pc, #228]	; 7668 <cdbg_do_config+0x100>
    7580:	e1a0e00f 	mov	lr, pc
    7584:	e593f000 	ldr	pc, [r3]
    7588:	e3a02000 	mov	r2, #0	; 0x0
    758c:	e59f00d4 	ldr	r0, [pc, #212]	; 7668 <cdbg_do_config+0x100>
    7590:	ea000006 	b	75b0 <cdbg_do_config+0x48>
    7594:	e7943002 	ldr	r3, [r4, r2]
    7598:	e2822004 	add	r2, r2, #4	; 0x4
    759c:	e1530000 	cmp	r3, r0
    75a0:	1a000002 	bne	75b0 <cdbg_do_config+0x48>
    75a4:	e3510000 	cmp	r1, #0	; 0x0
    75a8:	1a000004 	bne	75c0 <cdbg_do_config+0x58>
    75ac:	ea000007 	b	75d0 <cdbg_do_config+0x68>
    75b0:	e1520006 	cmp	r2, r6
    75b4:	e0841002 	add	r1, r4, r2
    75b8:	bafffff5 	blt	7594 <cdbg_do_config+0x2c>
    75bc:	ea000003 	b	75d0 <cdbg_do_config+0x68>
    75c0:	e5910004 	ldr	r0, [r1, #4]
    75c4:	eb001b74 	bl	e39c <spi_set_bytes>
    75c8:	e3a05000 	mov	r5, #0	; 0x0
    75cc:	ea000004 	b	75e4 <cdbg_do_config+0x7c>
    75d0:	e59f0094 	ldr	r0, [pc, #148]	; 766c <cdbg_do_config+0x104>
    75d4:	e59f3084 	ldr	r3, [pc, #132]	; 7660 <cdbg_do_config+0xf8>
    75d8:	e1a0e00f 	mov	lr, pc
    75dc:	e593f000 	ldr	pc, [r3]
    75e0:	e3e05000 	mvn	r5, #0	; 0x0
    75e4:	e59f1084 	ldr	r1, [pc, #132]	; 7670 <cdbg_do_config+0x108>
    75e8:	e59f0074 	ldr	r0, [pc, #116]	; 7664 <cdbg_do_config+0xfc>
    75ec:	e59f306c 	ldr	r3, [pc, #108]	; 7660 <cdbg_do_config+0xf8>
    75f0:	e1a0e00f 	mov	lr, pc
    75f4:	e593f000 	ldr	pc, [r3]
    75f8:	e3a02000 	mov	r2, #0	; 0x0
    75fc:	e59f106c 	ldr	r1, [pc, #108]	; 7670 <cdbg_do_config+0x108>
    7600:	ea000006 	b	7620 <cdbg_do_config+0xb8>
    7604:	e7943002 	ldr	r3, [r4, r2]
    7608:	e2822004 	add	r2, r2, #4	; 0x4
    760c:	e1530001 	cmp	r3, r1
    7610:	1a000002 	bne	7620 <cdbg_do_config+0xb8>
    7614:	e3500000 	cmp	r0, #0	; 0x0
    7618:	1a000004 	bne	7630 <cdbg_do_config+0xc8>
    761c:	ea000008 	b	7644 <cdbg_do_config+0xdc>
    7620:	e1520006 	cmp	r2, r6
    7624:	e0840002 	add	r0, r4, r2
    7628:	bafffff5 	blt	7604 <cdbg_do_config+0x9c>
    762c:	ea000004 	b	7644 <cdbg_do_config+0xdc>
    7630:	eb001042 	bl	b740 <nand_rescan_config>
    7634:	e3500000 	cmp	r0, #0	; 0x0
    7638:	01a03000 	moveq	r3, r0
    763c:	13e03000 	mvnne	r3, #0	; 0x0
    7640:	ea000004 	b	7658 <cdbg_do_config+0xf0>
    7644:	e59f3014 	ldr	r3, [pc, #20]	; 7660 <cdbg_do_config+0xf8>
    7648:	e59f0024 	ldr	r0, [pc, #36]	; 7674 <cdbg_do_config+0x10c>
    764c:	e1a0e00f 	mov	lr, pc
    7650:	e593f000 	ldr	pc, [r3]
    7654:	e3e03000 	mvn	r3, #0	; 0x0
    7658:	e0000395 	mul	r0, r5, r3
    765c:	e8bd8070 	pop	{r4, r5, r6, pc}
    7660:	3c00d6cc 	.word	0x3c00d6cc
    7664:	00017c95 	.word	0x00017c95
    7668:	4a82467c 	.word	0x4a82467c
    766c:	00017cb2 	.word	0x00017cb2
    7670:	aca4570e 	.word	0xaca4570e
    7674:	00017cd1 	.word	0x00017cd1

00007678 <cdbg_config_isi>:
    7678:	e92d4030 	push	{r4, r5, lr}
    767c:	e24dd004 	sub	sp, sp, #4	; 0x4
    7680:	e1a04000 	mov	r4, r0
    7684:	ebffe975 	bl	1c60 <isi_check>
    7688:	e2505000 	subs	r5, r0, #0	; 0x0
    768c:	0a000007 	beq	76b0 <cdbg_config_isi+0x38>
    7690:	e59f002c 	ldr	r0, [pc, #44]	; 76c4 <cdbg_config_isi+0x4c>
    7694:	e1a01005 	mov	r1, r5
    7698:	e59f3028 	ldr	r3, [pc, #40]	; 76c8 <cdbg_config_isi+0x50>
    769c:	e1a0e00f 	mov	lr, pc
    76a0:	e593f000 	ldr	pc, [r3]
    76a4:	e1a00005 	mov	r0, r5
    76a8:	e28dd004 	add	sp, sp, #4	; 0x4
    76ac:	e8bd8030 	pop	{r4, r5, pc}
    76b0:	e594100c 	ldr	r1, [r4, #12]
    76b4:	e2840c02 	add	r0, r4, #512	; 0x200
    76b8:	e28dd004 	add	sp, sp, #4	; 0x4
    76bc:	e8bd4030 	pop	{r4, r5, lr}
    76c0:	eaffffa8 	b	7568 <cdbg_do_config>
    76c4:	00017cf1 	.word	0x00017cf1
    76c8:	3c00d6cc 	.word	0x3c00d6cc

000076cc <burn_image_loop>:
    76cc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    76d0:	e24dd04c 	sub	sp, sp, #76	; 0x4c
    76d4:	e59db074 	ldr	fp, [sp, #116]
    76d8:	e1a06000 	mov	r6, r0
    76dc:	e35b0000 	cmp	fp, #0	; 0x0
    76e0:	e58d102c 	str	r1, [sp, #44]
    76e4:	e58d2028 	str	r2, [sp, #40]
    76e8:	e58d3024 	str	r3, [sp, #36]
    76ec:	e59da070 	ldr	sl, [sp, #112]
    76f0:	1a00000c 	bne	7728 <burn_image_loop+0x5c>
    76f4:	e59f449c 	ldr	r4, [pc, #1180]	; 7b98 <burn_image_loop+0x4cc>
    76f8:	e59f049c 	ldr	r0, [pc, #1180]	; 7b9c <burn_image_loop+0x4d0>
    76fc:	e3a0102e 	mov	r1, #46	; 0x2e
    7700:	e1a0e00f 	mov	lr, pc
    7704:	e594f000 	ldr	pc, [r4]
    7708:	e59f0490 	ldr	r0, [pc, #1168]	; 7ba0 <burn_image_loop+0x4d4>
    770c:	e59f1490 	ldr	r1, [pc, #1168]	; 7ba4 <burn_image_loop+0x4d8>
    7710:	e1a0e00f 	mov	lr, pc
    7714:	e594f000 	ldr	pc, [r4]
    7718:	e59f0488 	ldr	r0, [pc, #1160]	; 7ba8 <burn_image_loop+0x4dc>
    771c:	e1a0e00f 	mov	lr, pc
    7720:	e594f000 	ldr	pc, [r4]
    7724:	eafffffe 	b	7724 <burn_image_loop+0x58>
    7728:	e59d002c 	ldr	r0, [sp, #44]
    772c:	e59d1024 	ldr	r1, [sp, #36]
    7730:	e1500001 	cmp	r0, r1
    7734:	1a000005 	bne	7750 <burn_image_loop+0x84>
    7738:	e59f046c 	ldr	r0, [pc, #1132]	; 7bac <burn_image_loop+0x4e0>
    773c:	e59f3454 	ldr	r3, [pc, #1108]	; 7b98 <burn_image_loop+0x4cc>
    7740:	e1a0e00f 	mov	lr, pc
    7744:	e593f000 	ldr	pc, [r3]
    7748:	e3e00001 	mvn	r0, #1	; 0x1
    774c:	ea00010f 	b	7b90 <burn_image_loop+0x4c4>
    7750:	e596c000 	ldr	ip, [r6]
    7754:	e3a03000 	mov	r3, #0	; 0x0
    7758:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    775c:	e002200c 	and	r2, r2, ip
    7760:	e1a01483 	lsl	r1, r3, #9
    7764:	e1811ba2 	orr	r1, r1, r2, lsr #23
    7768:	e1a0e482 	lsl	lr, r2, #9
    776c:	e1a0200e 	mov	r2, lr
    7770:	e1a03001 	mov	r3, r1
    7774:	e1cd23f0 	strd	r2, [sp, #48]
    7778:	e1cd83d0 	ldrd	r8, [sp, #48]
    777c:	e1d600b8 	ldrh	r0, [r6, #8]
    7780:	e3a02401 	mov	r2, #16777216	; 0x1000000
    7784:	e0988002 	adds	r8, r8, r2
    7788:	e5961004 	ldr	r1, [r6, #4]
    778c:	e3a03000 	mov	r3, #0	; 0x0
    7790:	e1a0cc2c 	lsr	ip, ip, #24
    7794:	e0a99003 	adc	r9, r9, r3
    7798:	e1a05000 	mov	r5, r0
    779c:	e3a04000 	mov	r4, #0	; 0x0
    77a0:	e20cc03f 	and	ip, ip, #63	; 0x3f
    77a4:	e59d0024 	ldr	r0, [sp, #36]
    77a8:	e1cd83f0 	strd	r8, [sp, #48]
    77ac:	e58dc03c 	str	ip, [sp, #60]
    77b0:	e1848001 	orr	r8, r4, r1
    77b4:	e1a09005 	mov	r9, r5
    77b8:	e596500c 	ldr	r5, [r6, #12]
    77bc:	ebfffa5f 	bl	6140 <vs_device_erasable>
    77c0:	e1500004 	cmp	r0, r4
    77c4:	0a000006 	beq	77e4 <burn_image_loop+0x118>
    77c8:	e1a01004 	mov	r1, r4
    77cc:	e59d0024 	ldr	r0, [sp, #36]
    77d0:	ebfffb41 	bl	64dc <vs_assign_by_id>
    77d4:	e1a00008 	mov	r0, r8
    77d8:	e1a01009 	mov	r1, r9
    77dc:	e1a0200a 	mov	r2, sl
    77e0:	ebfff9ad 	bl	5e9c <vs_erase>
    77e4:	e59d002c 	ldr	r0, [sp, #44]
    77e8:	e3a01001 	mov	r1, #1	; 0x1
    77ec:	ebfffb3a 	bl	64dc <vs_assign_by_id>
    77f0:	e59d003c 	ldr	r0, [sp, #60]
    77f4:	e2403002 	sub	r3, r0, #2	; 0x2
    77f8:	e3530001 	cmp	r3, #1	; 0x1
    77fc:	8a000018 	bhi	7864 <burn_image_loop+0x198>
    7800:	e3a01c02 	mov	r1, #512	; 0x200
    7804:	e58d1000 	str	r1, [sp]
    7808:	e59d1028 	ldr	r1, [sp, #40]
    780c:	e1cd23d0 	ldrd	r2, [sp, #48]
    7810:	e1a0000b 	mov	r0, fp
    7814:	e58d1004 	str	r1, [sp, #4]
    7818:	ebfffa89 	bl	6244 <vs_read>
    781c:	e1a0000b 	mov	r0, fp
    7820:	ebffe8b3 	bl	1af4 <isi_check_header>
    7824:	e2504000 	subs	r4, r0, #0	; 0x0
    7828:	1a00000d 	bne	7864 <burn_image_loop+0x198>
    782c:	e59b3004 	ldr	r3, [fp, #4]
    7830:	e1b03e23 	lsrs	r3, r3, #28
    7834:	0a00000a 	beq	7864 <burn_image_loop+0x198>
    7838:	e59f0370 	ldr	r0, [pc, #880]	; 7bb0 <burn_image_loop+0x4e4>
    783c:	e59f3354 	ldr	r3, [pc, #852]	; 7b98 <burn_image_loop+0x4cc>
    7840:	e1a0e00f 	mov	lr, pc
    7844:	e593f000 	ldr	pc, [r3]
    7848:	e59f2364 	ldr	r2, [pc, #868]	; 7bb4 <burn_image_loop+0x4e8>
    784c:	e1a01004 	mov	r1, r4
    7850:	e3a00001 	mov	r0, #1	; 0x1
    7854:	ebfffb81 	bl	6660 <ss_aes_init>
    7858:	e3a02001 	mov	r2, #1	; 0x1
    785c:	e58d2044 	str	r2, [sp, #68]
    7860:	ea000001 	b	786c <burn_image_loop+0x1a0>
    7864:	e3a03000 	mov	r3, #0	; 0x0
    7868:	e58d3044 	str	r3, [sp, #68]
    786c:	e59d003c 	ldr	r0, [sp, #60]
    7870:	e3500005 	cmp	r0, #5	; 0x5
    7874:	11a06005 	movne	r6, r5
    7878:	13a07901 	movne	r7, #16384	; 0x4000
    787c:	1a000010 	bne	78c4 <burn_image_loop+0x1f8>
    7880:	e59d0024 	ldr	r0, [sp, #36]
    7884:	ebfff9c0 	bl	5f8c <vs_align>
    7888:	e1a04000 	mov	r4, r0
    788c:	e1a00005 	mov	r0, r5
    7890:	eb0009ed 	bl	a04c <fnd_size_shrink>
    7894:	e1a06000 	mov	r6, r0
    7898:	e1a00005 	mov	r0, r5
    789c:	eb0009dc 	bl	a014 <fnd_size_match>
    78a0:	e3500000 	cmp	r0, #0	; 0x0
    78a4:	11a07004 	movne	r7, r4
    78a8:	1a000005 	bne	78c4 <burn_image_loop+0x1f8>
    78ac:	e59f0304 	ldr	r0, [pc, #772]	; 7bb8 <burn_image_loop+0x4ec>
    78b0:	e59f32e0 	ldr	r3, [pc, #736]	; 7b98 <burn_image_loop+0x4cc>
    78b4:	e1a0e00f 	mov	lr, pc
    78b8:	e593f000 	ldr	pc, [r3]
    78bc:	e3e00000 	mvn	r0, #0	; 0x0
    78c0:	ea0000b2 	b	7b90 <burn_image_loop+0x4c4>
    78c4:	e59d0024 	ldr	r0, [sp, #36]
    78c8:	e59f12ec 	ldr	r1, [pc, #748]	; 7bbc <burn_image_loop+0x4f0>
    78cc:	ebfffa43 	bl	61e0 <vs_is_device>
    78d0:	e3500000 	cmp	r0, #0	; 0x0
    78d4:	1a00000a 	bne	7904 <burn_image_loop+0x238>
    78d8:	e59d0024 	ldr	r0, [sp, #36]
    78dc:	e59f12dc 	ldr	r1, [pc, #732]	; 7bc0 <burn_image_loop+0x4f4>
    78e0:	ebfffa3e 	bl	61e0 <vs_is_device>
    78e4:	e3500000 	cmp	r0, #0	; 0x0
    78e8:	1a000005 	bne	7904 <burn_image_loop+0x238>
    78ec:	e59d0024 	ldr	r0, [sp, #36]
    78f0:	e59f12cc 	ldr	r1, [pc, #716]	; 7bc4 <burn_image_loop+0x4f8>
    78f4:	ebfffa39 	bl	61e0 <vs_is_device>
    78f8:	e3500000 	cmp	r0, #0	; 0x0
    78fc:	01a05000 	moveq	r5, r0
    7900:	0a00001a 	beq	7970 <burn_image_loop+0x2a4>
    7904:	e59d0024 	ldr	r0, [sp, #36]
    7908:	e59f12b4 	ldr	r1, [pc, #692]	; 7bc4 <burn_image_loop+0x4f8>
    790c:	ebfffa33 	bl	61e0 <vs_is_device>
    7910:	e3500000 	cmp	r0, #0	; 0x0
    7914:	0a00000a 	beq	7944 <burn_image_loop+0x278>
    7918:	e3a00000 	mov	r0, #0	; 0x0
    791c:	eb000984 	bl	9f34 <nand_get_config>
    7920:	e3500000 	cmp	r0, #0	; 0x0
    7924:	0a00000b 	beq	7958 <burn_image_loop+0x28c>
    7928:	e590301c 	ldr	r3, [r0, #28]
    792c:	e3550902 	cmp	r5, #32768	; 0x8000
    7930:	33a06902 	movcc	r6, #32768	; 0x8000
    7934:	e3530802 	cmp	r3, #131072	; 0x20000
    7938:	21a05003 	movcs	r5, r3
    793c:	33a05802 	movcc	r5, #131072	; 0x20000
    7940:	ea00000a 	b	7970 <burn_image_loop+0x2a4>
    7944:	e3a00001 	mov	r0, #1	; 0x1
    7948:	eb000979 	bl	9f34 <nand_get_config>
    794c:	e3500000 	cmp	r0, #0	; 0x0
    7950:	1590501c 	ldrne	r5, [r0, #28]
    7954:	1a000005 	bne	7970 <burn_image_loop+0x2a4>
    7958:	e59f0268 	ldr	r0, [pc, #616]	; 7bc8 <burn_image_loop+0x4fc>
    795c:	e59f3234 	ldr	r3, [pc, #564]	; 7b98 <burn_image_loop+0x4cc>
    7960:	e1a0e00f 	mov	lr, pc
    7964:	e593f000 	ldr	pc, [r3]
    7968:	e3e00033 	mvn	r0, #51	; 0x33
    796c:	ea000087 	b	7b90 <burn_image_loop+0x4c4>
    7970:	e1a0100a 	mov	r1, sl
    7974:	e59f0250 	ldr	r0, [pc, #592]	; 7bcc <burn_image_loop+0x500>
    7978:	e1a02007 	mov	r2, r7
    797c:	e59f4214 	ldr	r4, [pc, #532]	; 7b98 <burn_image_loop+0x4cc>
    7980:	e1a0e00f 	mov	lr, pc
    7984:	e594f000 	ldr	pc, [r4]
    7988:	e2453001 	sub	r3, r5, #1	; 0x1
    798c:	e1a00003 	mov	r0, r3
    7990:	e1a01fc0 	asr	r1, r0, #31
    7994:	e066a00a 	rsb	sl, r6, sl
    7998:	e1cd01f8 	strd	r0, [sp, #24]
    799c:	e1a02005 	mov	r2, r5
    79a0:	e1a03fc2 	asr	r3, r2, #31
    79a4:	e1a00007 	mov	r0, r7
    79a8:	e3a01000 	mov	r1, #0	; 0x0
    79ac:	e58da040 	str	sl, [sp, #64]
    79b0:	e58d6038 	str	r6, [sp, #56]
    79b4:	e3a0a000 	mov	sl, #0	; 0x0
    79b8:	e1cd21f0 	strd	r2, [sp, #16]
    79bc:	e1cd00f8 	strd	r0, [sp, #8]
    79c0:	ea00006f 	b	7b84 <burn_image_loop+0x4b8>
    79c4:	e59d1038 	ldr	r1, [sp, #56]
    79c8:	e0663001 	rsb	r3, r6, r1
    79cc:	e1b03a23 	lsrs	r3, r3, #20
    79d0:	0a000005 	beq	79ec <burn_image_loop+0x320>
    79d4:	e59f01f4 	ldr	r0, [pc, #500]	; 7bd0 <burn_image_loop+0x504>
    79d8:	e1a01a26 	lsr	r1, r6, #20
    79dc:	e59f21b4 	ldr	r2, [pc, #436]	; 7b98 <burn_image_loop+0x4cc>
    79e0:	e1a0e00f 	mov	lr, pc
    79e4:	e592f000 	ldr	pc, [r2]
    79e8:	e58d6038 	str	r6, [sp, #56]
    79ec:	e35a0000 	cmp	sl, #0	; 0x0
    79f0:	d3a00000 	movle	r0, #0	; 0x0
    79f4:	c3a00001 	movgt	r0, #1	; 0x1
    79f8:	e28aa001 	add	sl, sl, #1	; 0x1
    79fc:	eb0021c4 	bl	10114 <led_ctrl>
    7a00:	e35a0032 	cmp	sl, #50	; 0x32
    7a04:	e3a01000 	mov	r1, #0	; 0x0
    7a08:	e59d002c 	ldr	r0, [sp, #44]
    7a0c:	c24aa064 	subgt	sl, sl, #100	; 0x64
    7a10:	ebfffab1 	bl	64dc <vs_assign_by_id>
    7a14:	e59d303c 	ldr	r3, [sp, #60]
    7a18:	e3530005 	cmp	r3, #5	; 0x5
    7a1c:	11a01007 	movne	r1, r7
    7a20:	1a000001 	bne	7a2c <burn_image_loop+0x360>
    7a24:	eb000970 	bl	9fec <fnd_align>
    7a28:	e1a01000 	mov	r1, r0
    7a2c:	e58d1000 	str	r1, [sp]
    7a30:	e59d1028 	ldr	r1, [sp, #40]
    7a34:	e1cd23d0 	ldrd	r2, [sp, #48]
    7a38:	e1a0000b 	mov	r0, fp
    7a3c:	e58d1004 	str	r1, [sp, #4]
    7a40:	ebfff9ff 	bl	6244 <vs_read>
    7a44:	e59d2044 	ldr	r2, [sp, #68]
    7a48:	e3520000 	cmp	r2, #0	; 0x0
    7a4c:	0a000003 	beq	7a60 <burn_image_loop+0x394>
    7a50:	e1a0000b 	mov	r0, fp
    7a54:	e1a0100b 	mov	r1, fp
    7a58:	e1a02007 	mov	r2, r7
    7a5c:	ebfffaee 	bl	661c <ss_aes_data>
    7a60:	e59d0024 	ldr	r0, [sp, #36]
    7a64:	e3a01000 	mov	r1, #0	; 0x0
    7a68:	ebfffa9b 	bl	64dc <vs_assign_by_id>
    7a6c:	e3500000 	cmp	r0, #0	; 0x0
    7a70:	1a00002e 	bne	7b30 <burn_image_loop+0x464>
    7a74:	e3550000 	cmp	r5, #0	; 0x0
    7a78:	0a00001f 	beq	7afc <burn_image_loop+0x430>
    7a7c:	e1cd01d8 	ldrd	r0, [sp, #24]
    7a80:	e0082000 	and	r2, r8, r0
    7a84:	e0093001 	and	r3, r9, r1
    7a88:	e1921003 	orrs	r1, r2, r3
    7a8c:	0a000012 	beq	7adc <burn_image_loop+0x410>
    7a90:	ea000019 	b	7afc <burn_image_loop+0x430>
    7a94:	e59d1040 	ldr	r1, [sp, #64]
    7a98:	e1510005 	cmp	r1, r5
    7a9c:	e0651001 	rsb	r1, r5, r1
    7aa0:	e58d1040 	str	r1, [sp, #64]
    7aa4:	da000006 	ble	7ac4 <burn_image_loop+0x3f8>
    7aa8:	e59f10e8 	ldr	r1, [pc, #232]	; 7b98 <burn_image_loop+0x4cc>
    7aac:	e1a0e00f 	mov	lr, pc
    7ab0:	e591f000 	ldr	pc, [r1]
    7ab4:	e1cd21d0 	ldrd	r2, [sp, #16]
    7ab8:	e0988002 	adds	r8, r8, r2
    7abc:	e0a99003 	adc	r9, r9, r3
    7ac0:	ea000005 	b	7adc <burn_image_loop+0x410>
    7ac4:	e59f0108 	ldr	r0, [pc, #264]	; 7bd4 <burn_image_loop+0x508>
    7ac8:	e59f30c8 	ldr	r3, [pc, #200]	; 7b98 <burn_image_loop+0x4cc>
    7acc:	e1a0e00f 	mov	lr, pc
    7ad0:	e593f000 	ldr	pc, [r3]
    7ad4:	e3e00034 	mvn	r0, #52	; 0x34
    7ad8:	ea00002c 	b	7b90 <burn_image_loop+0x4c4>
    7adc:	e1a00008 	mov	r0, r8
    7ae0:	e1a01009 	mov	r1, r9
    7ae4:	ebfff8fb 	bl	5ed8 <vs_isbad>
    7ae8:	e3500000 	cmp	r0, #0	; 0x0
    7aec:	e1a02008 	mov	r2, r8
    7af0:	e1a03009 	mov	r3, r9
    7af4:	e59f00dc 	ldr	r0, [pc, #220]	; 7bd8 <burn_image_loop+0x50c>
    7af8:	1affffe5 	bne	7a94 <burn_image_loop+0x3c8>
    7afc:	e59d303c 	ldr	r3, [sp, #60]
    7b00:	e3530005 	cmp	r3, #5	; 0x5
    7b04:	11a01007 	movne	r1, r7
    7b08:	1a000001 	bne	7b14 <burn_image_loop+0x448>
    7b0c:	eb000936 	bl	9fec <fnd_align>
    7b10:	e1a01000 	mov	r1, r0
    7b14:	e58d1000 	str	r1, [sp]
    7b18:	e1a0000b 	mov	r0, fp
    7b1c:	e3a01000 	mov	r1, #0	; 0x0
    7b20:	e1a02008 	mov	r2, r8
    7b24:	e1a03009 	mov	r3, r9
    7b28:	e58d1004 	str	r1, [sp, #4]
    7b2c:	ebfff8c4 	bl	5e44 <vs_write>
    7b30:	e59d003c 	ldr	r0, [sp, #60]
    7b34:	e3500001 	cmp	r0, #1	; 0x1
    7b38:	91a04007 	movls	r4, r7
    7b3c:	83a04000 	movhi	r4, #0	; 0x0
    7b40:	e3500005 	cmp	r0, #5	; 0x5
    7b44:	11a0c007 	movne	ip, r7
    7b48:	1a000001 	bne	7b54 <burn_image_loop+0x488>
    7b4c:	eb000926 	bl	9fec <fnd_align>
    7b50:	e1a0c000 	mov	ip, r0
    7b54:	e1cd03d0 	ldrd	r0, [sp, #48]
    7b58:	e1560007 	cmp	r6, r7
    7b5c:	31a0e006 	movcc	lr, r6
    7b60:	21a0e007 	movcs	lr, r7
    7b64:	e090200c 	adds	r2, r0, ip
    7b68:	e2a13000 	adc	r3, r1, #0	; 0x0
    7b6c:	e1cd23f0 	strd	r2, [sp, #48]
    7b70:	e1cd20d8 	ldrd	r2, [sp, #8]
    7b74:	e0988002 	adds	r8, r8, r2
    7b78:	e08bb004 	add	fp, fp, r4
    7b7c:	e0a99003 	adc	r9, r9, r3
    7b80:	e06e6006 	rsb	r6, lr, r6
    7b84:	e3560000 	cmp	r6, #0	; 0x0
    7b88:	1affff8d 	bne	79c4 <burn_image_loop+0x2f8>
    7b8c:	e1a00006 	mov	r0, r6
    7b90:	e28dd04c 	add	sp, sp, #76	; 0x4c
    7b94:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7b98:	3c00d6cc 	.word	0x3c00d6cc
    7b9c:	00017b71 	.word	0x00017b71
    7ba0:	00016437 	.word	0x00016437
    7ba4:	00017d0c 	.word	0x00017d0c
    7ba8:	00017d13 	.word	0x00017d13
    7bac:	00017d29 	.word	0x00017d29
    7bb0:	00017d56 	.word	0x00017d56
    7bb4:	3c00d08c 	.word	0x3c00d08c
    7bb8:	00017d6f 	.word	0x00017d6f
    7bbc:	0001627f 	.word	0x0001627f
    7bc0:	00016283 	.word	0x00016283
    7bc4:	00016127 	.word	0x00016127
    7bc8:	00017d8c 	.word	0x00017d8c
    7bcc:	00017da4 	.word	0x00017da4
    7bd0:	00017db3 	.word	0x00017db3
    7bd4:	00017ddf 	.word	0x00017ddf
    7bd8:	00017dc0 	.word	0x00017dc0

00007bdc <burn_images>:
    7bdc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7be0:	e590b01c 	ldr	fp, [r0, #28]
    7be4:	e24dd014 	sub	sp, sp, #20	; 0x14
    7be8:	e59f41dc 	ldr	r4, [pc, #476]	; 7dcc <burn_images+0x1f0>
    7bec:	e1a05000 	mov	r5, r0
    7bf0:	e58d100c 	str	r1, [sp, #12]
    7bf4:	e59f01d4 	ldr	r0, [pc, #468]	; 7dd0 <burn_images+0x1f4>
    7bf8:	e1a0100b 	mov	r1, fp
    7bfc:	e3a07000 	mov	r7, #0	; 0x0
    7c00:	e58d2008 	str	r2, [sp, #8]
    7c04:	e1a0e00f 	mov	lr, pc
    7c08:	e594f000 	ldr	pc, [r4]
    7c0c:	e2856020 	add	r6, r5, #32	; 0x20
    7c10:	e1a09004 	mov	r9, r4
    7c14:	e1a0a007 	mov	sl, r7
    7c18:	ea000061 	b	7da4 <burn_images+0x1c8>
    7c1c:	e59f01b0 	ldr	r0, [pc, #432]	; 7dd4 <burn_images+0x1f8>
    7c20:	e1a01007 	mov	r1, r7
    7c24:	e1a0e00f 	mov	lr, pc
    7c28:	e599f000 	ldr	pc, [r9]
    7c2c:	e5d63003 	ldrb	r3, [r6, #3]
    7c30:	e5968008 	ldr	r8, [r6, #8]
    7c34:	e203503f 	and	r5, r3, #63	; 0x3f
    7c38:	e3550003 	cmp	r5, #3	; 0x3
    7c3c:	0a000010 	beq	7c84 <burn_images+0xa8>
    7c40:	ca000006 	bgt	7c60 <burn_images+0x84>
    7c44:	e3550001 	cmp	r5, #1	; 0x1
    7c48:	0a000013 	beq	7c9c <burn_images+0xc0>
    7c4c:	c1a04828 	lsrgt	r4, r8, #16
    7c50:	ca000026 	bgt	7cf0 <burn_images+0x114>
    7c54:	e3550000 	cmp	r5, #0	; 0x0
    7c58:	0a000059 	beq	7dc4 <burn_images+0x1e8>
    7c5c:	ea00001f 	b	7ce0 <burn_images+0x104>
    7c60:	e3550005 	cmp	r5, #5	; 0x5
    7c64:	0a000008 	beq	7c8c <burn_images+0xb0>
    7c68:	b59f0168 	ldrlt	r0, [pc, #360]	; 7dd8 <burn_images+0x1fc>
    7c6c:	ba000007 	blt	7c90 <burn_images+0xb4>
    7c70:	e3550030 	cmp	r5, #48	; 0x30
    7c74:	0a00000e 	beq	7cb4 <burn_images+0xd8>
    7c78:	e3550031 	cmp	r5, #49	; 0x31
    7c7c:	1a000017 	bne	7ce0 <burn_images+0x104>
    7c80:	ea000010 	b	7cc8 <burn_images+0xec>
    7c84:	e59f0150 	ldr	r0, [pc, #336]	; 7ddc <burn_images+0x200>
    7c88:	ea000000 	b	7c90 <burn_images+0xb4>
    7c8c:	e59f014c 	ldr	r0, [pc, #332]	; 7de0 <burn_images+0x204>
    7c90:	ebfff93e 	bl	6190 <vs_device_id>
    7c94:	e1a04000 	mov	r4, r0
    7c98:	ea000014 	b	7cf0 <burn_images+0x114>
    7c9c:	e59f0140 	ldr	r0, [pc, #320]	; 7de4 <burn_images+0x208>
    7ca0:	e1a01008 	mov	r1, r8
    7ca4:	e1a0a008 	mov	sl, r8
    7ca8:	e1a0e00f 	mov	lr, pc
    7cac:	e599f000 	ldr	pc, [r9]
    7cb0:	ea000043 	b	7dc4 <burn_images+0x1e8>
    7cb4:	e59f012c 	ldr	r0, [pc, #300]	; 7de8 <burn_images+0x20c>
    7cb8:	e1a0e00f 	mov	lr, pc
    7cbc:	e599f000 	ldr	pc, [r9]
    7cc0:	ebffe2ed 	bl	87c <system_reset>
    7cc4:	ea00003e 	b	7dc4 <burn_images+0x1e8>
    7cc8:	e59f011c 	ldr	r0, [pc, #284]	; 7dec <burn_images+0x210>
    7ccc:	e1a0e00f 	mov	lr, pc
    7cd0:	e599f000 	ldr	pc, [r9]
    7cd4:	e3a00001 	mov	r0, #1	; 0x1
    7cd8:	ebffe1da 	bl	448 <try_boot>
    7cdc:	ea000038 	b	7dc4 <burn_images+0x1e8>
    7ce0:	e59f0108 	ldr	r0, [pc, #264]	; 7df0 <burn_images+0x214>
    7ce4:	e1a0e00f 	mov	lr, pc
    7ce8:	e599f000 	ldr	pc, [r9]
    7cec:	ea00002a 	b	7d9c <burn_images+0x1c0>
    7cf0:	e354000f 	cmp	r4, #15	; 0xf
    7cf4:	0a00000a 	beq	7d24 <burn_images+0x148>
    7cf8:	e1a00004 	mov	r0, r4
    7cfc:	e3a01001 	mov	r1, #1	; 0x1
    7d00:	ebfff9f5 	bl	64dc <vs_assign_by_id>
    7d04:	e3500000 	cmp	r0, #0	; 0x0
    7d08:	0a000005 	beq	7d24 <burn_images+0x148>
    7d0c:	e59f00e0 	ldr	r0, [pc, #224]	; 7df4 <burn_images+0x218>
    7d10:	e59f30b4 	ldr	r3, [pc, #180]	; 7dcc <burn_images+0x1f0>
    7d14:	e1a0e00f 	mov	lr, pc
    7d18:	e593f000 	ldr	pc, [r3]
    7d1c:	e3e04002 	mvn	r4, #2	; 0x2
    7d20:	ea000022 	b	7db0 <burn_images+0x1d4>
    7d24:	e3550001 	cmp	r5, #1	; 0x1
    7d28:	e1a0c828 	lsr	ip, r8, #16
    7d2c:	01a0e00a 	moveq	lr, sl
    7d30:	13a0e30f 	movne	lr, #1006632960	; 0x3c000000
    7d34:	e1a0c80c 	lsl	ip, ip, #16
    7d38:	e1a03004 	mov	r3, r4
    7d3c:	e1a00006 	mov	r0, r6
    7d40:	e59d100c 	ldr	r1, [sp, #12]
    7d44:	e59d2008 	ldr	r2, [sp, #8]
    7d48:	e88d5000 	stm	sp, {ip, lr}
    7d4c:	ebfffe5e 	bl	76cc <burn_image_loop>
    7d50:	e3500000 	cmp	r0, #0	; 0x0
    7d54:	aa000006 	bge	7d74 <burn_images+0x198>
    7d58:	e1a01007 	mov	r1, r7
    7d5c:	e59f0094 	ldr	r0, [pc, #148]	; 7df8 <burn_images+0x21c>
    7d60:	e59f3064 	ldr	r3, [pc, #100]	; 7dcc <burn_images+0x1f0>
    7d64:	e1a0e00f 	mov	lr, pc
    7d68:	e593f000 	ldr	pc, [r3]
    7d6c:	e3e04000 	mvn	r4, #0	; 0x0
    7d70:	ea00000e 	b	7db0 <burn_images+0x1d4>
    7d74:	e3550001 	cmp	r5, #1	; 0x1
    7d78:	1a000003 	bne	7d8c <burn_images+0x1b0>
    7d7c:	e5960004 	ldr	r0, [r6, #4]
    7d80:	e3a01000 	mov	r1, #0	; 0x0
    7d84:	ebfffda8 	bl	742c <cdbg_jump>
    7d88:	ea000003 	b	7d9c <burn_images+0x1c0>
    7d8c:	e3550000 	cmp	r5, #0	; 0x0
    7d90:	1a000001 	bne	7d9c <burn_images+0x1c0>
    7d94:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
    7d98:	ebfffe36 	bl	7678 <cdbg_config_isi>
    7d9c:	e2877001 	add	r7, r7, #1	; 0x1
    7da0:	e2866010 	add	r6, r6, #16	; 0x10
    7da4:	e157000b 	cmp	r7, fp
    7da8:	baffff9b 	blt	7c1c <burn_images+0x40>
    7dac:	e3a04000 	mov	r4, #0	; 0x0
    7db0:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
    7db4:	ebffe6b3 	bl	1888 <free>
    7db8:	e1a00004 	mov	r0, r4
    7dbc:	e28dd014 	add	sp, sp, #20	; 0x14
    7dc0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7dc4:	e3a0400f 	mov	r4, #15	; 0xf
    7dc8:	eaffffd5 	b	7d24 <burn_images+0x148>
    7dcc:	3c00d6cc 	.word	0x3c00d6cc
    7dd0:	00017dfd 	.word	0x00017dfd
    7dd4:	00017e1a 	.word	0x00017e1a
    7dd8:	0001627f 	.word	0x0001627f
    7ddc:	00016127 	.word	0x00016127
    7de0:	00016283 	.word	0x00016283
    7de4:	00017e30 	.word	0x00017e30
    7de8:	00017e4d 	.word	0x00017e4d
    7dec:	00017e62 	.word	0x00017e62
    7df0:	00017e75 	.word	0x00017e75
    7df4:	00017e89 	.word	0x00017e89
    7df8:	00017ea6 	.word	0x00017ea6

00007dfc <burn>:
    7dfc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7e00:	e1a09000 	mov	r9, r0
    7e04:	e24dd00c 	sub	sp, sp, #12	; 0xc
    7e08:	e3a00c02 	mov	r0, #512	; 0x200
    7e0c:	ebffe635 	bl	16e8 <malloc>
    7e10:	e2507000 	subs	r7, r0, #0	; 0x0
    7e14:	1a000004 	bne	7e2c <burn+0x30>
    7e18:	e59f019c 	ldr	r0, [pc, #412]	; 7fbc <burn+0x1c0>
    7e1c:	e59f319c 	ldr	r3, [pc, #412]	; 7fc0 <burn+0x1c4>
    7e20:	e1a0e00f 	mov	lr, pc
    7e24:	e593f000 	ldr	pc, [r3]
    7e28:	ea000060 	b	7fb0 <burn+0x1b4>
    7e2c:	ebffe203 	bl	640 <boot_device>
    7e30:	e3a05000 	mov	r5, #0	; 0x0
    7e34:	e59f8184 	ldr	r8, [pc, #388]	; 7fc0 <burn+0x1c4>
    7e38:	e3a0bc02 	mov	fp, #512	; 0x200
    7e3c:	e1a0a000 	mov	sl, r0
    7e40:	ea00004c 	b	7f78 <burn+0x17c>
    7e44:	e1a00005 	mov	r0, r5
    7e48:	ebfff877 	bl	602c <vs_device_burnsrc>
    7e4c:	e3500000 	cmp	r0, #0	; 0x0
    7e50:	0a000047 	beq	7f74 <burn+0x178>
    7e54:	e155000a 	cmp	r5, sl
    7e58:	0a000045 	beq	7f74 <burn+0x178>
    7e5c:	e3a03001 	mov	r3, #1	; 0x1
    7e60:	e0193513 	ands	r3, r9, r3, lsl r5
    7e64:	1a000042 	bne	7f74 <burn+0x178>
    7e68:	ebffe2ac 	bl	920 <gtc_time>
    7e6c:	e1a00005 	mov	r0, r5
    7e70:	e59f114c 	ldr	r1, [pc, #332]	; 7fc4 <burn+0x1c8>
    7e74:	ebfff8d9 	bl	61e0 <vs_is_device>
    7e78:	e3500000 	cmp	r0, #0	; 0x0
    7e7c:	1a000004 	bne	7e94 <burn+0x98>
    7e80:	e1a00005 	mov	r0, r5
    7e84:	e59f113c 	ldr	r1, [pc, #316]	; 7fc8 <burn+0x1cc>
    7e88:	ebfff8d4 	bl	61e0 <vs_is_device>
    7e8c:	e3500000 	cmp	r0, #0	; 0x0
    7e90:	0a00000c 	beq	7ec8 <burn+0xcc>
    7e94:	ebffe333 	bl	b68 <assert_linkpc>
    7e98:	e3500000 	cmp	r0, #0	; 0x0
    7e9c:	0a000009 	beq	7ec8 <burn+0xcc>
    7ea0:	eb0023a9 	bl	10d4c <linkpc_loop>
    7ea4:	e2506000 	subs	r6, r0, #0	; 0x0
    7ea8:	0a000038 	beq	7f90 <burn+0x194>
    7eac:	e1a00005 	mov	r0, r5
    7eb0:	e5984000 	ldr	r4, [r8]
    7eb4:	ebfff892 	bl	6104 <vs_device_name>
    7eb8:	e1a02006 	mov	r2, r6
    7ebc:	e1a01000 	mov	r1, r0
    7ec0:	e59f0104 	ldr	r0, [pc, #260]	; 7fcc <burn+0x1d0>
    7ec4:	ea00000a 	b	7ef4 <burn+0xf8>
    7ec8:	e1a00005 	mov	r0, r5
    7ecc:	e3a01001 	mov	r1, #1	; 0x1
    7ed0:	ebfff981 	bl	64dc <vs_assign_by_id>
    7ed4:	e2506000 	subs	r6, r0, #0	; 0x0
    7ed8:	0a000007 	beq	7efc <burn+0x100>
    7edc:	e1a00005 	mov	r0, r5
    7ee0:	e5984000 	ldr	r4, [r8]
    7ee4:	ebfff886 	bl	6104 <vs_device_name>
    7ee8:	e1a01000 	mov	r1, r0
    7eec:	e59f00dc 	ldr	r0, [pc, #220]	; 7fd0 <burn+0x1d4>
    7ef0:	e1a02006 	mov	r2, r6
    7ef4:	e12fff34 	blx	r4
    7ef8:	ea00001d 	b	7f74 <burn+0x178>
    7efc:	e1a01007 	mov	r1, r7
    7f00:	e59f00cc 	ldr	r0, [pc, #204]	; 7fd4 <burn+0x1d8>
    7f04:	e3a02401 	mov	r2, #16777216	; 0x1000000
    7f08:	e3a03c02 	mov	r3, #512	; 0x200
    7f0c:	e58d6000 	str	r6, [sp]
    7f10:	e1a0e00f 	mov	lr, pc
    7f14:	e598f000 	ldr	pc, [r8]
    7f18:	e3a02401 	mov	r2, #16777216	; 0x1000000
    7f1c:	e3a03000 	mov	r3, #0	; 0x0
    7f20:	e1a00007 	mov	r0, r7
    7f24:	e58d6004 	str	r6, [sp, #4]
    7f28:	e58db000 	str	fp, [sp]
    7f2c:	ebfff8c4 	bl	6244 <vs_read>
    7f30:	e1a00007 	mov	r0, r7
    7f34:	eb0029a4 	bl	125cc <ius_check_header>
    7f38:	e2504000 	subs	r4, r0, #0	; 0x0
    7f3c:	1a00000c 	bne	7f74 <burn+0x178>
    7f40:	e1a01005 	mov	r1, r5
    7f44:	e59f3074 	ldr	r3, [pc, #116]	; 7fc0 <burn+0x1c4>
    7f48:	e59f0088 	ldr	r0, [pc, #136]	; 7fd8 <burn+0x1dc>
    7f4c:	e1a0e00f 	mov	lr, pc
    7f50:	e593f000 	ldr	pc, [r3]
    7f54:	e1a00004 	mov	r0, r4
    7f58:	e1a01004 	mov	r1, r4
    7f5c:	ebfffd27 	bl	7400 <cdbg_verify_burn_enable>
    7f60:	e1a00007 	mov	r0, r7
    7f64:	e1a01005 	mov	r1, r5
    7f68:	e1a02004 	mov	r2, r4
    7f6c:	ebffff1a 	bl	7bdc <burn_images>
    7f70:	ea000006 	b	7f90 <burn+0x194>
    7f74:	e2855001 	add	r5, r5, #1	; 0x1
    7f78:	ebfff882 	bl	6188 <vs_device_count>
    7f7c:	e1550000 	cmp	r5, r0
    7f80:	baffffaf 	blt	7e44 <burn+0x48>
    7f84:	e1a00007 	mov	r0, r7
    7f88:	ebffe63e 	bl	1888 <free>
    7f8c:	ea000007 	b	7fb0 <burn+0x1b4>
    7f90:	e59f0044 	ldr	r0, [pc, #68]	; 7fdc <burn+0x1e0>
    7f94:	e59f3024 	ldr	r3, [pc, #36]	; 7fc0 <burn+0x1c4>
    7f98:	e1a0e00f 	mov	lr, pc
    7f9c:	e593f000 	ldr	pc, [r3]
    7fa0:	e1a00007 	mov	r0, r7
    7fa4:	ebffe637 	bl	1888 <free>
    7fa8:	ebfffd51 	bl	74f4 <cdbg_shell>
    7fac:	eafffffe 	b	7fac <burn+0x1b0>
    7fb0:	e3e00000 	mvn	r0, #0	; 0x0
    7fb4:	e28dd00c 	add	sp, sp, #12	; 0xc
    7fb8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7fbc:	00017ebe 	.word	0x00017ebe
    7fc0:	3c00d6cc 	.word	0x3c00d6cc
    7fc4:	000179da 	.word	0x000179da
    7fc8:	000179d6 	.word	0x000179d6
    7fcc:	00017ede 	.word	0x00017ede
    7fd0:	00017f00 	.word	0x00017f00
    7fd4:	00017f1f 	.word	0x00017f1f
    7fd8:	00017f4f 	.word	0x00017f4f
    7fdc:	00017f67 	.word	0x00017f67

00007fe0 <mmc_send_cmd>:
    7fe0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    7fe4:	e24dd004 	sub	sp, sp, #4	; 0x4
    7fe8:	e1a0e00f 	mov	lr, pc
    7fec:	e590f104 	ldr	pc, [r0, #260]
    7ff0:	e28dd004 	add	sp, sp, #4	; 0x4
    7ff4:	e8bd8000 	pop	{pc}

00007ff8 <mmc_set_blocklen>:
    7ff8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    7ffc:	e3a03015 	mov	r3, #21	; 0x15
    8000:	e24dd024 	sub	sp, sp, #36	; 0x24
    8004:	e3a02000 	mov	r2, #0	; 0x0
    8008:	e58d3004 	str	r3, [sp, #4]
    800c:	e3a03010 	mov	r3, #16	; 0x10
    8010:	e58d1008 	str	r1, [sp, #8]
    8014:	e1cd30b0 	strh	r3, [sp]
    8018:	e58d201c 	str	r2, [sp, #28]
    801c:	e1a0100d 	mov	r1, sp
    8020:	e1a0e00f 	mov	lr, pc
    8024:	e590f104 	ldr	pc, [r0, #260]
    8028:	e28dd024 	add	sp, sp, #36	; 0x24
    802c:	e8bd8000 	pop	{pc}

00008030 <find_mmc_device>:
    8030:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8034:	e59f3048 	ldr	r3, [pc, #72]	; 8084 <find_mmc_device+0x54>
    8038:	e24dd004 	sub	sp, sp, #4	; 0x4
    803c:	e1a01000 	mov	r1, r0
    8040:	e1a0c003 	mov	ip, r3
    8044:	e5932000 	ldr	r2, [r3]
    8048:	ea000004 	b	8060 <find_mmc_device+0x30>
    804c:	e592309c 	ldr	r3, [r2, #156]
    8050:	e1a00002 	mov	r0, r2
    8054:	e1530001 	cmp	r3, r1
    8058:	0a000007 	beq	807c <find_mmc_device+0x4c>
    805c:	e5922000 	ldr	r2, [r2]
    8060:	e152000c 	cmp	r2, ip
    8064:	1afffff8 	bne	804c <find_mmc_device+0x1c>
    8068:	e59f0018 	ldr	r0, [pc, #24]	; 8088 <find_mmc_device+0x58>
    806c:	e59f3018 	ldr	r3, [pc, #24]	; 808c <find_mmc_device+0x5c>
    8070:	e1a0e00f 	mov	lr, pc
    8074:	e593f000 	ldr	pc, [r3]
    8078:	e3a00000 	mov	r0, #0	; 0x0
    807c:	e28dd004 	add	sp, sp, #4	; 0x4
    8080:	e8bd8000 	pop	{pc}
    8084:	3c00d1c8 	.word	0x3c00d1c8
    8088:	00017f8a 	.word	0x00017f8a
    808c:	3c00d6cc 	.word	0x3c00d6cc

00008090 <mmc_send_ext_csd>:
    8090:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8094:	e3a03015 	mov	r3, #21	; 0x15
    8098:	e24dd034 	sub	sp, sp, #52	; 0x34
    809c:	e58d3004 	str	r3, [sp, #4]
    80a0:	e3a03c02 	mov	r3, #512	; 0x200
    80a4:	e3a02000 	mov	r2, #0	; 0x0
    80a8:	e3a0c001 	mov	ip, #1	; 0x1
    80ac:	e58d302c 	str	r3, [sp, #44]
    80b0:	e3a03008 	mov	r3, #8	; 0x8
    80b4:	e58d1020 	str	r1, [sp, #32]
    80b8:	e58d201c 	str	r2, [sp, #28]
    80bc:	e58d2008 	str	r2, [sp, #8]
    80c0:	e58dc024 	str	ip, [sp, #36]
    80c4:	e1cd30b0 	strh	r3, [sp]
    80c8:	e58dc028 	str	ip, [sp, #40]
    80cc:	e1a0100d 	mov	r1, sp
    80d0:	e28d2020 	add	r2, sp, #32	; 0x20
    80d4:	e1a0e00f 	mov	lr, pc
    80d8:	e590f104 	ldr	pc, [r0, #260]
    80dc:	e28dd034 	add	sp, sp, #52	; 0x34
    80e0:	e8bd8000 	pop	{pc}

000080e4 <sd_switch>:
    80e4:	e3e0c4ff 	mvn	ip, #-16777216	; 0xff000000
    80e8:	e1a02102 	lsl	r2, r2, #2
    80ec:	e18ccf81 	orr	ip, ip, r1, lsl #31
    80f0:	e3a0100f 	mov	r1, #15	; 0xf
    80f4:	e1ccc211 	bic	ip, ip, r1, lsl r2
    80f8:	e18cc213 	orr	ip, ip, r3, lsl r2
    80fc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8100:	e3a03015 	mov	r3, #21	; 0x15
    8104:	e24dd034 	sub	sp, sp, #52	; 0x34
    8108:	e58d3004 	str	r3, [sp, #4]
    810c:	e3a03000 	mov	r3, #0	; 0x0
    8110:	e58d301c 	str	r3, [sp, #28]
    8114:	e59d3038 	ldr	r3, [sp, #56]
    8118:	e3a02001 	mov	r2, #1	; 0x1
    811c:	e58d3020 	str	r3, [sp, #32]
    8120:	e3a03040 	mov	r3, #64	; 0x40
    8124:	e58d302c 	str	r3, [sp, #44]
    8128:	e3a03006 	mov	r3, #6	; 0x6
    812c:	e58d2024 	str	r2, [sp, #36]
    8130:	e58d2028 	str	r2, [sp, #40]
    8134:	e58dc008 	str	ip, [sp, #8]
    8138:	e1cd30b0 	strh	r3, [sp]
    813c:	e1a0100d 	mov	r1, sp
    8140:	e28d2020 	add	r2, sp, #32	; 0x20
    8144:	e1a0e00f 	mov	lr, pc
    8148:	e590f104 	ldr	pc, [r0, #260]
    814c:	e28dd034 	add	sp, sp, #52	; 0x34
    8150:	e8bd8000 	pop	{pc}

00008154 <mmc_set_ios>:
    8154:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8158:	e24dd004 	sub	sp, sp, #4	; 0x4
    815c:	e1a0e00f 	mov	lr, pc
    8160:	e590f108 	ldr	pc, [r0, #264]
    8164:	e28dd004 	add	sp, sp, #4	; 0x4
    8168:	e8bd8000 	pop	{pc}

0000816c <mmc_set_clock>:
    816c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8170:	e590203c 	ldr	r2, [r0, #60]
    8174:	e590c038 	ldr	ip, [r0, #56]
    8178:	e1510002 	cmp	r1, r2
    817c:	21a01002 	movcs	r1, r2
    8180:	e24dd004 	sub	sp, sp, #4	; 0x4
    8184:	e151000c 	cmp	r1, ip
    8188:	25801048 	strcs	r1, [r0, #72]
    818c:	3580c048 	strcc	ip, [r0, #72]
    8190:	e1a0e00f 	mov	lr, pc
    8194:	e590f108 	ldr	pc, [r0, #264]
    8198:	e28dd004 	add	sp, sp, #4	; 0x4
    819c:	e8bd8000 	pop	{pc}

000081a0 <mmc_set_bus_width>:
    81a0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    81a4:	e5801044 	str	r1, [r0, #68]
    81a8:	e24dd004 	sub	sp, sp, #4	; 0x4
    81ac:	e1a0e00f 	mov	lr, pc
    81b0:	e590f108 	ldr	pc, [r0, #264]
    81b4:	e28dd004 	add	sp, sp, #4	; 0x4
    81b8:	e8bd8000 	pop	{pc}

000081bc <mmc_send_if_cond>:
    81bc:	e92d4010 	push	{r4, lr}
    81c0:	e590202c 	ldr	r2, [r0, #44]
    81c4:	e59f3060 	ldr	r3, [pc, #96]	; 822c <mmc_send_if_cond+0x70>
    81c8:	e24dd020 	sub	sp, sp, #32	; 0x20
    81cc:	e0023003 	and	r3, r2, r3
    81d0:	e3530000 	cmp	r3, #0	; 0x0
    81d4:	e28d1020 	add	r1, sp, #32	; 0x20
    81d8:	e3a03008 	mov	r3, #8	; 0x8
    81dc:	e300c1aa 	movw	ip, #426	; 0x1aa
    81e0:	e16132b0 	strh	r3, [r1, #-32]!
    81e4:	03a0c0aa 	moveq	ip, #170	; 0xaa
    81e8:	e3a02000 	mov	r2, #0	; 0x0
    81ec:	e3a03015 	mov	r3, #21	; 0x15
    81f0:	e98d1008 	stmib	sp, {r3, ip}
    81f4:	e58d201c 	str	r2, [sp, #28]
    81f8:	e1a0100d 	mov	r1, sp
    81fc:	e5903104 	ldr	r3, [r0, #260]
    8200:	e1a04000 	mov	r4, r0
    8204:	e12fff33 	blx	r3
    8208:	e3500000 	cmp	r0, #0	; 0x0
    820c:	1a000004 	bne	8224 <mmc_send_if_cond+0x68>
    8210:	e5dd300c 	ldrb	r3, [sp, #12]
    8214:	e35300aa 	cmp	r3, #170	; 0xaa
    8218:	059f3010 	ldreq	r3, [pc, #16]	; 8230 <mmc_send_if_cond+0x74>
    821c:	13e00010 	mvnne	r0, #16	; 0x10
    8220:	05843030 	streq	r3, [r4, #48]
    8224:	e28dd020 	add	sp, sp, #32	; 0x20
    8228:	e8bd8010 	pop	{r4, pc}
    822c:	00ff8000 	.word	0x00ff8000
    8230:	00020020 	.word	0x00020020

00008234 <mmc_register>:
    8234:	e5903110 	ldr	r3, [r0, #272]
    8238:	e59fc05c 	ldr	ip, [pc, #92]	; 829c <mmc_register+0x68>
    823c:	e3530000 	cmp	r3, #0	; 0x0
    8240:	e3a03006 	mov	r3, #6	; 0x6
    8244:	e59c2000 	ldr	r2, [ip]
    8248:	e5803098 	str	r3, [r0, #152]
    824c:	e59f304c 	ldr	r3, [pc, #76]	; 82a0 <mmc_register+0x6c>
    8250:	e2821001 	add	r1, r2, #1	; 0x1
    8254:	e58030f8 	str	r3, [r0, #248]
    8258:	e59f3044 	ldr	r3, [pc, #68]	; 82a4 <mmc_register+0x70>
    825c:	e580209c 	str	r2, [r0, #156]
    8260:	e58030fc 	str	r3, [r0, #252]
    8264:	e3a03001 	mov	r3, #1	; 0x1
    8268:	e58c1000 	str	r1, [ip]
    826c:	e5c030a4 	strb	r3, [r0, #164]
    8270:	03a03c02 	moveq	r3, #512	; 0x200
    8274:	05803110 	streq	r3, [r0, #272]
    8278:	e59f3028 	ldr	r3, [pc, #40]	; 82a8 <mmc_register+0x74>
    827c:	e5800004 	str	r0, [r0, #4]
    8280:	e5932004 	ldr	r2, [r3, #4]
    8284:	e5803000 	str	r3, [r0]
    8288:	e5830004 	str	r0, [r3, #4]
    828c:	e5820000 	str	r0, [r2]
    8290:	e5802004 	str	r2, [r0, #4]
    8294:	e3a00000 	mov	r0, #0	; 0x0
    8298:	e12fff1e 	bx	lr
    829c:	3c00d0ac 	.word	0x3c00d0ac
    82a0:	00009470 	.word	0x00009470
    82a4:	0000898c 	.word	0x0000898c
    82a8:	3c00d1c8 	.word	0x3c00d1c8

000082ac <mmc_get_dev>:
    82ac:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    82b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    82b4:	ebffff5d 	bl	8030 <find_mmc_device>
    82b8:	e3500000 	cmp	r0, #0	; 0x0
    82bc:	12800098 	addne	r0, r0, #152	; 0x98
    82c0:	e28dd004 	add	sp, sp, #4	; 0x4
    82c4:	e8bd8000 	pop	{pc}

000082c8 <print_mmc_devices>:
    82c8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    82cc:	e59f3068 	ldr	r3, [pc, #104]	; 833c <print_mmc_devices+0x74>
    82d0:	e24dd004 	sub	sp, sp, #4	; 0x4
    82d4:	e1a05000 	mov	r5, r0
    82d8:	e1a07003 	mov	r7, r3
    82dc:	e5934000 	ldr	r4, [r3]
    82e0:	e59f6058 	ldr	r6, [pc, #88]	; 8340 <print_mmc_devices+0x78>
    82e4:	ea00000a 	b	8314 <print_mmc_devices+0x4c>
    82e8:	e594209c 	ldr	r2, [r4, #156]
    82ec:	e1a0e00f 	mov	lr, pc
    82f0:	e596f000 	ldr	pc, [r6]
    82f4:	e5943000 	ldr	r3, [r4]
    82f8:	e59f0044 	ldr	r0, [pc, #68]	; 8344 <print_mmc_devices+0x7c>
    82fc:	e1530007 	cmp	r3, r7
    8300:	e1a01005 	mov	r1, r5
    8304:	0a000001 	beq	8310 <print_mmc_devices+0x48>
    8308:	e1a0e00f 	mov	lr, pc
    830c:	e596f000 	ldr	pc, [r6]
    8310:	e5944000 	ldr	r4, [r4]
    8314:	e1540007 	cmp	r4, r7
    8318:	e2841008 	add	r1, r4, #8	; 0x8
    831c:	e59f0024 	ldr	r0, [pc, #36]	; 8348 <print_mmc_devices+0x80>
    8320:	1afffff0 	bne	82e8 <print_mmc_devices+0x20>
    8324:	e59f0020 	ldr	r0, [pc, #32]	; 834c <print_mmc_devices+0x84>
    8328:	e59f3010 	ldr	r3, [pc, #16]	; 8340 <print_mmc_devices+0x78>
    832c:	e1a0e00f 	mov	lr, pc
    8330:	e593f000 	ldr	pc, [r3]
    8334:	e28dd004 	add	sp, sp, #4	; 0x4
    8338:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    833c:	3c00d1c8 	.word	0x3c00d1c8
    8340:	3c00d6cc 	.word	0x3c00d6cc
    8344:	00017faa 	.word	0x00017faa
    8348:	00017fa3 	.word	0x00017fa3
    834c:	00016bef 	.word	0x00016bef

00008350 <get_mmc_num>:
    8350:	e59f3004 	ldr	r3, [pc, #4]	; 835c <get_mmc_num+0xc>
    8354:	e5930000 	ldr	r0, [r3]
    8358:	e12fff1e 	bx	lr
    835c:	3c00d0ac 	.word	0x3c00d0ac

00008360 <mmc_vs_align>:
    8360:	e3a00c02 	mov	r0, #512	; 0x200
    8364:	e12fff1e 	bx	lr

00008368 <init_mmc>:
    8368:	e59f3060 	ldr	r3, [pc, #96]	; 83d0 <init_mmc+0x68>
    836c:	e59f2060 	ldr	r2, [pc, #96]	; 83d4 <init_mmc+0x6c>
    8370:	e92d4070 	push	{r4, r5, r6, lr}
    8374:	e3a04000 	mov	r4, #0	; 0x0
    8378:	e3a00b02 	mov	r0, #2048	; 0x800
    837c:	e5833004 	str	r3, [r3, #4]
    8380:	e5824000 	str	r4, [r2]
    8384:	e5833000 	str	r3, [r3]
    8388:	ebffe4d6 	bl	16e8 <malloc>
    838c:	e1a05000 	mov	r5, r0
    8390:	e2806b01 	add	r6, r0, #1024	; 0x400
    8394:	e1a00005 	mov	r0, r5
    8398:	e3a01000 	mov	r1, #0	; 0x0
    839c:	e3a02f4e 	mov	r2, #312	; 0x138
    83a0:	ebffe463 	bl	1534 <memset>
    83a4:	e5856130 	str	r6, [r5, #304]
    83a8:	e1a00004 	mov	r0, r4
    83ac:	e1a01005 	mov	r1, r5
    83b0:	eb000490 	bl	95f8 <imap_sdhc_initialize>
    83b4:	e3500000 	cmp	r0, #0	; 0x0
    83b8:	e2844001 	add	r4, r4, #1	; 0x1
    83bc:	e2855f4e 	add	r5, r5, #312	; 0x138
    83c0:	18bd8070 	popne	{r4, r5, r6, pc}
    83c4:	e3540003 	cmp	r4, #3	; 0x3
    83c8:	1afffff1 	bne	8394 <init_mmc+0x2c>
    83cc:	e8bd8070 	pop	{r4, r5, r6, pc}
    83d0:	3c00d1c8 	.word	0x3c00d1c8
    83d4:	3c00d0ac 	.word	0x3c00d0ac

000083d8 <sd_change_freq>:
    83d8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    83dc:	e5903050 	ldr	r3, [r0, #80]
    83e0:	e3a01000 	mov	r1, #0	; 0x0
    83e4:	e2132b01 	ands	r2, r3, #1024	; 0x400
    83e8:	e1a04000 	mov	r4, r0
    83ec:	e580104c 	str	r1, [r0, #76]
    83f0:	e24dd084 	sub	sp, sp, #132	; 0x84
    83f4:	11a00001 	movne	r0, r1
    83f8:	1a000070 	bne	85c0 <sd_change_freq+0x1e8>
    83fc:	e1d438b0 	ldrh	r3, [r4, #128]
    8400:	e3a05015 	mov	r5, #21	; 0x15
    8404:	e1a03803 	lsl	r3, r3, #16
    8408:	e58d3050 	str	r3, [sp, #80]
    840c:	e28d6048 	add	r6, sp, #72	; 0x48
    8410:	e3a03037 	mov	r3, #55	; 0x37
    8414:	e1cd34b8 	strh	r3, [sp, #72]
    8418:	e58d504c 	str	r5, [sp, #76]
    841c:	e58d2064 	str	r2, [sp, #100]
    8420:	e1a01006 	mov	r1, r6
    8424:	e1a0e00f 	mov	lr, pc
    8428:	e594f104 	ldr	pc, [r4, #260]
    842c:	e3500000 	cmp	r0, #0	; 0x0
    8430:	1a000062 	bne	85c0 <sd_change_freq+0x1e8>
    8434:	e3a03033 	mov	r3, #51	; 0x33
    8438:	e58d504c 	str	r5, [sp, #76]
    843c:	e1a09006 	mov	r9, r6
    8440:	e58d0064 	str	r0, [sp, #100]
    8444:	e1cd34b8 	strh	r3, [sp, #72]
    8448:	e58d0050 	str	r0, [sp, #80]
    844c:	e3a05003 	mov	r5, #3	; 0x3
    8450:	e28da078 	add	sl, sp, #120	; 0x78
    8454:	e3a08008 	mov	r8, #8	; 0x8
    8458:	e3a06001 	mov	r6, #1	; 0x1
    845c:	e28d7068 	add	r7, sp, #104	; 0x68
    8460:	e58da068 	str	sl, [sp, #104]
    8464:	e58d8074 	str	r8, [sp, #116]
    8468:	e58d6070 	str	r6, [sp, #112]
    846c:	e58d606c 	str	r6, [sp, #108]
    8470:	e1a00004 	mov	r0, r4
    8474:	e1a01009 	mov	r1, r9
    8478:	e1a02007 	mov	r2, r7
    847c:	e1a0e00f 	mov	lr, pc
    8480:	e594f104 	ldr	pc, [r4, #260]
    8484:	e250b000 	subs	fp, r0, #0	; 0x0
    8488:	0a000003 	beq	849c <sd_change_freq+0xc4>
    848c:	e3550000 	cmp	r5, #0	; 0x0
    8490:	e2455001 	sub	r5, r5, #1	; 0x1
    8494:	1afffff1 	bne	8460 <sd_change_freq+0x88>
    8498:	ea000047 	b	85bc <sd_change_freq+0x1e4>
    849c:	e59d3078 	ldr	r3, [sp, #120]
    84a0:	e59d007c 	ldr	r0, [sp, #124]
    84a4:	e1a01c03 	lsl	r1, r3, #24
    84a8:	e2032cff 	and	r2, r3, #65280	; 0xff00
    84ac:	e1811c23 	orr	r1, r1, r3, lsr #24
    84b0:	e1811402 	orr	r1, r1, r2, lsl #8
    84b4:	e20338ff 	and	r3, r3, #16711680	; 0xff0000
    84b8:	e1811423 	orr	r1, r1, r3, lsr #8
    84bc:	e1a03c00 	lsl	r3, r0, #24
    84c0:	e1833c20 	orr	r3, r3, r0, lsr #24
    84c4:	e2002cff 	and	r2, r0, #65280	; 0xff00
    84c8:	e1a0cc21 	lsr	ip, r1, #24
    84cc:	e1833402 	orr	r3, r3, r2, lsl #8
    84d0:	e20008ff 	and	r0, r0, #16711680	; 0xff0000
    84d4:	e20cc00f 	and	ip, ip, #15	; 0xf
    84d8:	e1833420 	orr	r3, r3, r0, lsr #8
    84dc:	e35c0001 	cmp	ip, #1	; 0x1
    84e0:	e584305c 	str	r3, [r4, #92]
    84e4:	e5841058 	str	r1, [r4, #88]
    84e8:	059f30d8 	ldreq	r3, [pc, #216]	; 85c8 <sd_change_freq+0x1f0>
    84ec:	0a000004 	beq	8504 <sd_change_freq+0x12c>
    84f0:	3a000002 	bcc	8500 <sd_change_freq+0x128>
    84f4:	e35c0002 	cmp	ip, #2	; 0x2
    84f8:	059f30cc 	ldreq	r3, [pc, #204]	; 85cc <sd_change_freq+0x1f4>
    84fc:	0a000000 	beq	8504 <sd_change_freq+0x12c>
    8500:	e59f30c8 	ldr	r3, [pc, #200]	; 85d0 <sd_change_freq+0x1f8>
    8504:	e5843030 	str	r3, [r4, #48]
    8508:	e5943058 	ldr	r3, [r4, #88]
    850c:	e5942030 	ldr	r2, [r4, #48]
    8510:	e3130701 	tst	r3, #262144	; 0x40000
    8514:	1594304c 	ldrne	r3, [r4, #76]
    8518:	13833c01 	orrne	r3, r3, #256	; 0x100
    851c:	1584304c 	strne	r3, [r4, #76]
    8520:	e59f30a8 	ldr	r3, [pc, #168]	; 85d0 <sd_change_freq+0x1f8>
    8524:	e1520003 	cmp	r2, r3
    8528:	13a05004 	movne	r5, #4	; 0x4
    852c:	128d6008 	addne	r6, sp, #8	; 0x8
    8530:	1a000007 	bne	8554 <sd_change_freq+0x17c>
    8534:	ea000020 	b	85bc <sd_change_freq+0x1e4>
    8538:	e58d6000 	str	r6, [sp]
    853c:	ebfffee8 	bl	80e4 <sd_switch>
    8540:	e3500000 	cmp	r0, #0	; 0x0
    8544:	1a00001d 	bne	85c0 <sd_change_freq+0x1e8>
    8548:	e59d3024 	ldr	r3, [sp, #36]
    854c:	e3130c02 	tst	r3, #512	; 0x200
    8550:	0a000005 	beq	856c <sd_change_freq+0x194>
    8554:	e3a01000 	mov	r1, #0	; 0x0
    8558:	e2555001 	subs	r5, r5, #1	; 0x1
    855c:	e1a00004 	mov	r0, r4
    8560:	e1a02001 	mov	r2, r1
    8564:	e3a03001 	mov	r3, #1	; 0x1
    8568:	2afffff2 	bcs	8538 <sd_change_freq+0x160>
    856c:	e59d3014 	ldr	r3, [sp, #20]
    8570:	e3130c02 	tst	r3, #512	; 0x200
    8574:	0a000010 	beq	85bc <sd_change_freq+0x1e4>
    8578:	e3a01001 	mov	r1, #1	; 0x1
    857c:	e28dc008 	add	ip, sp, #8	; 0x8
    8580:	e1a00004 	mov	r0, r4
    8584:	e3a02000 	mov	r2, #0	; 0x0
    8588:	e1a03001 	mov	r3, r1
    858c:	e58dc000 	str	ip, [sp]
    8590:	ebfffed3 	bl	80e4 <sd_switch>
    8594:	e3500000 	cmp	r0, #0	; 0x0
    8598:	1a000008 	bne	85c0 <sd_change_freq+0x1e8>
    859c:	e59d3018 	ldr	r3, [sp, #24]
    85a0:	e1a03c03 	lsl	r3, r3, #24
    85a4:	e203340f 	and	r3, r3, #251658240	; 0xf000000
    85a8:	e3530401 	cmp	r3, #16777216	; 0x1000000
    85ac:	0594304c 	ldreq	r3, [r4, #76]
    85b0:	03833001 	orreq	r3, r3, #1	; 0x1
    85b4:	0584304c 	streq	r3, [r4, #76]
    85b8:	ea000000 	b	85c0 <sd_change_freq+0x1e8>
    85bc:	e1a0000b 	mov	r0, fp
    85c0:	e28dd084 	add	sp, sp, #132	; 0x84
    85c4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    85c8:	0002001a 	.word	0x0002001a
    85cc:	00020020 	.word	0x00020020
    85d0:	00020010 	.word	0x00020010

000085d4 <sd_send_op_cond>:
    85d4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    85d8:	e59f9140 	ldr	r9, [pc, #320]	; 8720 <sd_send_op_cond+0x14c>
    85dc:	e24dd020 	sub	sp, sp, #32	; 0x20
    85e0:	e1a04000 	mov	r4, r0
    85e4:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    85e8:	e3a0a015 	mov	sl, #21	; 0x15
    85ec:	e3a06000 	mov	r6, #0	; 0x0
    85f0:	e1a0700d 	mov	r7, sp
    85f4:	e3a08001 	mov	r8, #1	; 0x1
    85f8:	e3a03037 	mov	r3, #55	; 0x37
    85fc:	e1cd30b0 	strh	r3, [sp]
    8600:	e1a0100d 	mov	r1, sp
    8604:	e1a02006 	mov	r2, r6
    8608:	e58da004 	str	sl, [sp, #4]
    860c:	e58d6008 	str	r6, [sp, #8]
    8610:	e58d601c 	str	r6, [sp, #28]
    8614:	e1a00004 	mov	r0, r4
    8618:	e1a0e00f 	mov	lr, pc
    861c:	e594f104 	ldr	pc, [r4, #260]
    8620:	e2503000 	subs	r3, r0, #0	; 0x0
    8624:	e1a0100d 	mov	r1, sp
    8628:	e3a02000 	mov	r2, #0	; 0x0
    862c:	e59fe0f0 	ldr	lr, [pc, #240]	; 8724 <sd_send_op_cond+0x150>
    8630:	e1a00004 	mov	r0, r4
    8634:	1a000036 	bne	8714 <sd_send_op_cond+0x140>
    8638:	e594302c 	ldr	r3, [r4, #44]
    863c:	e594c030 	ldr	ip, [r4, #48]
    8640:	e003e00e 	and	lr, r3, lr
    8644:	e15c0009 	cmp	ip, r9
    8648:	038e3101 	orreq	r3, lr, #1073741824	; 0x40000000
    864c:	e58de008 	str	lr, [sp, #8]
    8650:	058d3008 	streq	r3, [sp, #8]
    8654:	e3a03029 	mov	r3, #41	; 0x29
    8658:	e1cd30b0 	strh	r3, [sp]
    865c:	e58d8004 	str	r8, [sp, #4]
    8660:	e5943104 	ldr	r3, [r4, #260]
    8664:	e12fff33 	blx	r3
    8668:	e2503000 	subs	r3, r0, #0	; 0x0
    866c:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    8670:	1a000027 	bne	8714 <sd_send_op_cond+0x140>
    8674:	eb0014d8 	bl	d9dc <udelay>
    8678:	e59d300c 	ldr	r3, [sp, #12]
    867c:	e3530000 	cmp	r3, #0	; 0x0
    8680:	ba000002 	blt	8690 <sd_send_op_cond+0xbc>
    8684:	e2555001 	subs	r5, r5, #1	; 0x1
    8688:	2affffda 	bcs	85f8 <sd_send_op_cond+0x24>
    868c:	ea00001f 	b	8710 <sd_send_op_cond+0x13c>
    8690:	e3550000 	cmp	r5, #0	; 0x0
    8694:	da00001d 	ble	8710 <sd_send_op_cond+0x13c>
    8698:	e59f3080 	ldr	r3, [pc, #128]	; 8720 <sd_send_op_cond+0x14c>
    869c:	e5942030 	ldr	r2, [r4, #48]
    86a0:	e1520003 	cmp	r2, r3
    86a4:	12433010 	subne	r3, r3, #16	; 0x10
    86a8:	15843030 	strne	r3, [r4, #48]
    86ac:	e5943050 	ldr	r3, [r4, #80]
    86b0:	e3130b01 	tst	r3, #1024	; 0x400
    86b4:	0a00000c 	beq	86ec <sd_send_op_cond+0x118>
    86b8:	e3a03001 	mov	r3, #1	; 0x1
    86bc:	e3a02000 	mov	r2, #0	; 0x0
    86c0:	e58d3004 	str	r3, [sp, #4]
    86c4:	e3a0303a 	mov	r3, #58	; 0x3a
    86c8:	e1cd30b0 	strh	r3, [sp]
    86cc:	e58d2008 	str	r2, [sp, #8]
    86d0:	e58d201c 	str	r2, [sp, #28]
    86d4:	e1a00004 	mov	r0, r4
    86d8:	e1a0100d 	mov	r1, sp
    86dc:	e1a0e00f 	mov	lr, pc
    86e0:	e594f104 	ldr	pc, [r4, #260]
    86e4:	e2503000 	subs	r3, r0, #0	; 0x0
    86e8:	1a000009 	bne	8714 <sd_send_op_cond+0x140>
    86ec:	e59d200c 	ldr	r2, [sp, #12]
    86f0:	e3a03000 	mov	r3, #0	; 0x0
    86f4:	e1c438b0 	strh	r3, [r4, #128]
    86f8:	e1a03f22 	lsr	r3, r2, #30
    86fc:	e2033001 	and	r3, r3, #1	; 0x1
    8700:	e5843040 	str	r3, [r4, #64]
    8704:	e5842054 	str	r2, [r4, #84]
    8708:	e3a03000 	mov	r3, #0	; 0x0
    870c:	ea000000 	b	8714 <sd_send_op_cond+0x140>
    8710:	e3e03010 	mvn	r3, #16	; 0x10
    8714:	e1a00003 	mov	r0, r3
    8718:	e28dd020 	add	sp, sp, #32	; 0x20
    871c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    8720:	00020020 	.word	0x00020020
    8724:	00ff8000 	.word	0x00ff8000

00008728 <mmc_go_idle>:
    8728:	e92d4010 	push	{r4, lr}
    872c:	e1a04000 	mov	r4, r0
    8730:	e24dd020 	sub	sp, sp, #32	; 0x20
    8734:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    8738:	eb0014a7 	bl	d9dc <udelay>
    873c:	e3a02000 	mov	r2, #0	; 0x0
    8740:	e1a00004 	mov	r0, r4
    8744:	e1cd20b0 	strh	r2, [sp]
    8748:	e58d2008 	str	r2, [sp, #8]
    874c:	e58d2004 	str	r2, [sp, #4]
    8750:	e58d201c 	str	r2, [sp, #28]
    8754:	e1a0100d 	mov	r1, sp
    8758:	e1a0e00f 	mov	lr, pc
    875c:	e594f104 	ldr	pc, [r4, #260]
    8760:	e2504000 	subs	r4, r0, #0	; 0x0
    8764:	1a000001 	bne	8770 <mmc_go_idle+0x48>
    8768:	e2800e7d 	add	r0, r0, #2000	; 0x7d0
    876c:	eb00149a 	bl	d9dc <udelay>
    8770:	e1a00004 	mov	r0, r4
    8774:	e28dd020 	add	sp, sp, #32	; 0x20
    8778:	e8bd8010 	pop	{r4, pc}

0000877c <mmc_send_op_cond>:
    877c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    8780:	e24dd020 	sub	sp, sp, #32	; 0x20
    8784:	e1a05000 	mov	r5, r0
    8788:	e3a06001 	mov	r6, #1	; 0x1
    878c:	ebffffe5 	bl	8728 <mmc_go_idle>
    8790:	e3a02000 	mov	r2, #0	; 0x0
    8794:	e1cd60b0 	strh	r6, [sp]
    8798:	e58d6004 	str	r6, [sp, #4]
    879c:	e58d2008 	str	r2, [sp, #8]
    87a0:	e58d201c 	str	r2, [sp, #28]
    87a4:	e1a00005 	mov	r0, r5
    87a8:	e1a0100d 	mov	r1, sp
    87ac:	e1a0e00f 	mov	lr, pc
    87b0:	e595f104 	ldr	pc, [r5, #260]
    87b4:	e2504000 	subs	r4, r0, #0	; 0x0
    87b8:	e1a0700d 	mov	r7, sp
    87bc:	1a000039 	bne	88a8 <mmc_send_op_cond+0x12c>
    87c0:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    87c4:	eb001484 	bl	d9dc <udelay>
    87c8:	e1a08006 	mov	r8, r6
    87cc:	e1a0700d 	mov	r7, sp
    87d0:	e1a06004 	mov	r6, r4
    87d4:	e3024710 	movw	r4, #10000	; 0x2710
    87d8:	e595302c 	ldr	r3, [r5, #44]
    87dc:	e3a02001 	mov	r2, #1	; 0x1
    87e0:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    87e4:	e1cd20b0 	strh	r2, [sp]
    87e8:	e58d3008 	str	r3, [sp, #8]
    87ec:	e1a02006 	mov	r2, r6
    87f0:	e58d8004 	str	r8, [sp, #4]
    87f4:	e58d601c 	str	r6, [sp, #28]
    87f8:	e1a0100d 	mov	r1, sp
    87fc:	e1a00005 	mov	r0, r5
    8800:	e1a0e00f 	mov	lr, pc
    8804:	e595f104 	ldr	pc, [r5, #260]
    8808:	e2502000 	subs	r2, r0, #0	; 0x0
    880c:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    8810:	11a04002 	movne	r4, r2
    8814:	1a000023 	bne	88a8 <mmc_send_op_cond+0x12c>
    8818:	eb00146f 	bl	d9dc <udelay>
    881c:	e59d300c 	ldr	r3, [sp, #12]
    8820:	e3530000 	cmp	r3, #0	; 0x0
    8824:	ba000002 	blt	8834 <mmc_send_op_cond+0xb8>
    8828:	e2544001 	subs	r4, r4, #1	; 0x1
    882c:	2affffe9 	bcs	87d8 <mmc_send_op_cond+0x5c>
    8830:	ea00001b 	b	88a4 <mmc_send_op_cond+0x128>
    8834:	e3540000 	cmp	r4, #0	; 0x0
    8838:	da000019 	ble	88a4 <mmc_send_op_cond+0x128>
    883c:	e5953050 	ldr	r3, [r5, #80]
    8840:	e3130b01 	tst	r3, #1024	; 0x400
    8844:	0a00000b 	beq	8878 <mmc_send_op_cond+0xfc>
    8848:	e3a0303a 	mov	r3, #58	; 0x3a
    884c:	e58d8004 	str	r8, [sp, #4]
    8850:	e1cd30b0 	strh	r3, [sp]
    8854:	e58d6008 	str	r6, [sp, #8]
    8858:	e58d601c 	str	r6, [sp, #28]
    885c:	e1a0100d 	mov	r1, sp
    8860:	e1a02006 	mov	r2, r6
    8864:	e1a00005 	mov	r0, r5
    8868:	e1a0e00f 	mov	lr, pc
    886c:	e595f104 	ldr	pc, [r5, #260]
    8870:	e2504000 	subs	r4, r0, #0	; 0x0
    8874:	1a00000b 	bne	88a8 <mmc_send_op_cond+0x12c>
    8878:	e59d300c 	ldr	r3, [sp, #12]
    887c:	e3a01801 	mov	r1, #65536	; 0x10000
    8880:	e1a02f23 	lsr	r2, r3, #30
    8884:	e2022001 	and	r2, r2, #1	; 0x1
    8888:	e5852040 	str	r2, [r5, #64]
    888c:	e3a02000 	mov	r2, #0	; 0x0
    8890:	e5851030 	str	r1, [r5, #48]
    8894:	e1c528b0 	strh	r2, [r5, #128]
    8898:	e5853054 	str	r3, [r5, #84]
    889c:	e3a04000 	mov	r4, #0	; 0x0
    88a0:	ea000000 	b	88a8 <mmc_send_op_cond+0x12c>
    88a4:	e3e04010 	mvn	r4, #16	; 0x10
    88a8:	e1a00004 	mov	r0, r4
    88ac:	e28dd020 	add	sp, sp, #32	; 0x20
    88b0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

000088b4 <mmc_send_status>:
    88b4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    88b8:	e1d038b0 	ldrh	r3, [r0, #128]
    88bc:	e24dd024 	sub	sp, sp, #36	; 0x24
    88c0:	e1a03803 	lsl	r3, r3, #16
    88c4:	e58d3008 	str	r3, [sp, #8]
    88c8:	e3a03000 	mov	r3, #0	; 0x0
    88cc:	e3a02015 	mov	r2, #21	; 0x15
    88d0:	e58d301c 	str	r3, [sp, #28]
    88d4:	e3a0300d 	mov	r3, #13	; 0xd
    88d8:	e1a06000 	mov	r6, r0
    88dc:	e58d2004 	str	r2, [sp, #4]
    88e0:	e1a04001 	mov	r4, r1
    88e4:	e1cd30b0 	strh	r3, [sp]
    88e8:	e1a0700d 	mov	r7, sp
    88ec:	e1a0100d 	mov	r1, sp
    88f0:	e3a02000 	mov	r2, #0	; 0x0
    88f4:	e1a00006 	mov	r0, r6
    88f8:	e1a0e00f 	mov	lr, pc
    88fc:	e596f104 	ldr	pc, [r6, #260]
    8900:	e2505000 	subs	r5, r0, #0	; 0x0
    8904:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    8908:	1a000018 	bne	8970 <mmc_send_status+0xbc>
    890c:	e59d300c 	ldr	r3, [sp, #12]
    8910:	e3130c01 	tst	r3, #256	; 0x100
    8914:	1a00000e 	bne	8954 <mmc_send_status+0xa0>
    8918:	eb00142f 	bl	d9dc <udelay>
    891c:	e59d100c 	ldr	r1, [sp, #12]
    8920:	e59f3054 	ldr	r3, [pc, #84]	; 897c <mmc_send_status+0xc8>
    8924:	e2444001 	sub	r4, r4, #1	; 0x1
    8928:	e0013003 	and	r3, r1, r3
    892c:	e3530000 	cmp	r3, #0	; 0x0
    8930:	0a000005 	beq	894c <mmc_send_status+0x98>
    8934:	e59f0044 	ldr	r0, [pc, #68]	; 8980 <mmc_send_status+0xcc>
    8938:	e59f3044 	ldr	r3, [pc, #68]	; 8984 <mmc_send_status+0xd0>
    893c:	e1a0e00f 	mov	lr, pc
    8940:	e593f000 	ldr	pc, [r3]
    8944:	e3e05011 	mvn	r5, #17	; 0x11
    8948:	ea000008 	b	8970 <mmc_send_status+0xbc>
    894c:	e3740001 	cmn	r4, #1	; 0x1
    8950:	1affffe5 	bne	88ec <mmc_send_status+0x38>
    8954:	e3540000 	cmp	r4, #0	; 0x0
    8958:	1a000004 	bne	8970 <mmc_send_status+0xbc>
    895c:	e59f0024 	ldr	r0, [pc, #36]	; 8988 <mmc_send_status+0xd4>
    8960:	e59f301c 	ldr	r3, [pc, #28]	; 8984 <mmc_send_status+0xd0>
    8964:	e1a0e00f 	mov	lr, pc
    8968:	e593f000 	ldr	pc, [r3]
    896c:	e3e05012 	mvn	r5, #18	; 0x12
    8970:	e1a00005 	mov	r0, r5
    8974:	e28dd024 	add	sp, sp, #36	; 0x24
    8978:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    897c:	fdf94080 	.word	0xfdf94080
    8980:	00017fae 	.word	0x00017fae
    8984:	3c00d6cc 	.word	0x3c00d6cc
    8988:	00017fc4 	.word	0x00017fc4

0000898c <mmc_bwrite>:
    898c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8990:	e24dd034 	sub	sp, sp, #52	; 0x34
    8994:	e1a06001 	mov	r6, r1
    8998:	e1a0b002 	mov	fp, r2
    899c:	e1a0a003 	mov	sl, r3
    89a0:	ebfffda2 	bl	8030 <find_mmc_device>
    89a4:	e2504000 	subs	r4, r0, #0	; 0x0
    89a8:	0a000060 	beq	8b30 <mmc_bwrite+0x1a4>
    89ac:	e594108c 	ldr	r1, [r4, #140]
    89b0:	ebfffd90 	bl	7ff8 <mmc_set_blocklen>
    89b4:	e3500000 	cmp	r0, #0	; 0x0
    89b8:	01a09000 	moveq	r9, r0
    89bc:	01a0700b 	moveq	r7, fp
    89c0:	0a000005 	beq	89dc <mmc_bwrite+0x50>
    89c4:	e594108c 	ldr	r1, [r4, #140]
    89c8:	e59f016c 	ldr	r0, [pc, #364]	; 8b3c <mmc_bwrite+0x1b0>
    89cc:	e59f316c 	ldr	r3, [pc, #364]	; 8b40 <mmc_bwrite+0x1b4>
    89d0:	e1a0e00f 	mov	lr, pc
    89d4:	e593f000 	ldr	pc, [r3]
    89d8:	ea000054 	b	8b30 <mmc_bwrite+0x1a4>
    89dc:	e5943110 	ldr	r3, [r4, #272]
    89e0:	e59420a8 	ldr	r2, [r4, #168]
    89e4:	e1570003 	cmp	r7, r3
    89e8:	31a05007 	movcc	r5, r7
    89ec:	21a05003 	movcs	r5, r3
    89f0:	e0858006 	add	r8, r5, r6
    89f4:	e1580002 	cmp	r8, r2
    89f8:	9a000005 	bls	8a14 <mmc_bwrite+0x88>
    89fc:	e59f0140 	ldr	r0, [pc, #320]	; 8b44 <mmc_bwrite+0x1b8>
    8a00:	e1a01008 	mov	r1, r8
    8a04:	e59f3134 	ldr	r3, [pc, #308]	; 8b40 <mmc_bwrite+0x1b4>
    8a08:	e1a0e00f 	mov	lr, pc
    8a0c:	e593f000 	ldr	pc, [r3]
    8a10:	ea00003d 	b	8b0c <mmc_bwrite+0x180>
    8a14:	e3550001 	cmp	r5, #1	; 0x1
    8a18:	93a03018 	movls	r3, #24	; 0x18
    8a1c:	91cd30b0 	strhls	r3, [sp]
    8a20:	e5943040 	ldr	r3, [r4, #64]
    8a24:	83a02019 	movhi	r2, #25	; 0x19
    8a28:	81cd20b0 	strhhi	r2, [sp]
    8a2c:	e3530000 	cmp	r3, #0	; 0x0
    8a30:	0594308c 	ldreq	r3, [r4, #140]
    8a34:	e3a02015 	mov	r2, #21	; 0x15
    8a38:	00030693 	muleq	r3, r3, r6
    8a3c:	158d6008 	strne	r6, [sp, #8]
    8a40:	058d3008 	streq	r3, [sp, #8]
    8a44:	e58d2004 	str	r2, [sp, #4]
    8a48:	e58d901c 	str	r9, [sp, #28]
    8a4c:	e58da020 	str	sl, [sp, #32]
    8a50:	e58d5028 	str	r5, [sp, #40]
    8a54:	e594308c 	ldr	r3, [r4, #140]
    8a58:	e3a02002 	mov	r2, #2	; 0x2
    8a5c:	e58d2024 	str	r2, [sp, #36]
    8a60:	e58d302c 	str	r3, [sp, #44]
    8a64:	e1a00004 	mov	r0, r4
    8a68:	e1a0100d 	mov	r1, sp
    8a6c:	e28d2020 	add	r2, sp, #32	; 0x20
    8a70:	e1a0e00f 	mov	lr, pc
    8a74:	e594f104 	ldr	pc, [r4, #260]
    8a78:	e3500000 	cmp	r0, #0	; 0x0
    8a7c:	0a000004 	beq	8a94 <mmc_bwrite+0x108>
    8a80:	e59f00c0 	ldr	r0, [pc, #192]	; 8b48 <mmc_bwrite+0x1bc>
    8a84:	e59f30b4 	ldr	r3, [pc, #180]	; 8b40 <mmc_bwrite+0x1b4>
    8a88:	e1a0e00f 	mov	lr, pc
    8a8c:	e593f000 	ldr	pc, [r3]
    8a90:	ea00001d 	b	8b0c <mmc_bwrite+0x180>
    8a94:	e5943050 	ldr	r3, [r4, #80]
    8a98:	e1a03523 	lsr	r3, r3, #10
    8a9c:	e2233001 	eor	r3, r3, #1	; 0x1
    8aa0:	e3550001 	cmp	r5, #1	; 0x1
    8aa4:	93a03000 	movls	r3, #0	; 0x0
    8aa8:	82033001 	andhi	r3, r3, #1	; 0x1
    8aac:	e3530000 	cmp	r3, #0	; 0x0
    8ab0:	0a000011 	beq	8afc <mmc_bwrite+0x170>
    8ab4:	e3a0200c 	mov	r2, #12	; 0xc
    8ab8:	e3a0301d 	mov	r3, #29	; 0x1d
    8abc:	e1cd20b0 	strh	r2, [sp]
    8ac0:	e98d0208 	stmib	sp, {r3, r9}
    8ac4:	e58d901c 	str	r9, [sp, #28]
    8ac8:	e1a00004 	mov	r0, r4
    8acc:	e1a0100d 	mov	r1, sp
    8ad0:	e1a02009 	mov	r2, r9
    8ad4:	e1a0e00f 	mov	lr, pc
    8ad8:	e594f104 	ldr	pc, [r4, #260]
    8adc:	e3500000 	cmp	r0, #0	; 0x0
    8ae0:	0a000005 	beq	8afc <mmc_bwrite+0x170>
    8ae4:	e59f0060 	ldr	r0, [pc, #96]	; 8b4c <mmc_bwrite+0x1c0>
    8ae8:	e59f3050 	ldr	r3, [pc, #80]	; 8b40 <mmc_bwrite+0x1b4>
    8aec:	e1a0e00f 	mov	lr, pc
    8af0:	e593f000 	ldr	pc, [r3]
    8af4:	e1a00009 	mov	r0, r9
    8af8:	ea00000d 	b	8b34 <mmc_bwrite+0x1a8>
    8afc:	e1a00004 	mov	r0, r4
    8b00:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8b04:	ebffff6a 	bl	88b4 <mmc_send_status>
    8b08:	ea000001 	b	8b14 <mmc_bwrite+0x188>
    8b0c:	e3550000 	cmp	r5, #0	; 0x0
    8b10:	1a000006 	bne	8b30 <mmc_bwrite+0x1a4>
    8b14:	e0577005 	subs	r7, r7, r5
    8b18:	e594008c 	ldr	r0, [r4, #140]
    8b1c:	102aa590 	mlane	sl, r0, r5, sl
    8b20:	11a06008 	movne	r6, r8
    8b24:	1affffac 	bne	89dc <mmc_bwrite+0x50>
    8b28:	e000009b 	mul	r0, fp, r0
    8b2c:	ea000000 	b	8b34 <mmc_bwrite+0x1a8>
    8b30:	e3a00000 	mov	r0, #0	; 0x0
    8b34:	e28dd034 	add	sp, sp, #52	; 0x34
    8b38:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    8b3c:	00017fe0 	.word	0x00017fe0
    8b40:	3c00d6cc 	.word	0x3c00d6cc
    8b44:	00017fea 	.word	0x00017fea
    8b48:	00018016 	.word	0x00018016
    8b4c:	00018028 	.word	0x00018028

00008b50 <mmc2_vs_write>:
    8b50:	e1a024a2 	lsr	r2, r2, #9
    8b54:	e1821b83 	orr	r1, r2, r3, lsl #23
    8b58:	e59d2000 	ldr	r2, [sp]
    8b5c:	e1a03000 	mov	r3, r0
    8b60:	e1a024c2 	asr	r2, r2, #9
    8b64:	e3a00002 	mov	r0, #2	; 0x2
    8b68:	eaffff87 	b	898c <mmc_bwrite>

00008b6c <mmc1_vs_write>:
    8b6c:	e1a024a2 	lsr	r2, r2, #9
    8b70:	e1821b83 	orr	r1, r2, r3, lsl #23
    8b74:	e59d2000 	ldr	r2, [sp]
    8b78:	e1a03000 	mov	r3, r0
    8b7c:	e1a024c2 	asr	r2, r2, #9
    8b80:	e3a00001 	mov	r0, #1	; 0x1
    8b84:	eaffff80 	b	898c <mmc_bwrite>

00008b88 <mmc0_vs_write>:
    8b88:	e1a024a2 	lsr	r2, r2, #9
    8b8c:	e1821b83 	orr	r1, r2, r3, lsl #23
    8b90:	e59d2000 	ldr	r2, [sp]
    8b94:	e1a03000 	mov	r3, r0
    8b98:	e1a024c2 	asr	r2, r2, #9
    8b9c:	e3a00000 	mov	r0, #0	; 0x0
    8ba0:	eaffff79 	b	898c <mmc_bwrite>

00008ba4 <mmc_switch>:
    8ba4:	e92d4030 	push	{r4, r5, lr}
    8ba8:	e1a02802 	lsl	r2, r2, #16
    8bac:	e3822403 	orr	r2, r2, #50331648	; 0x3000000
    8bb0:	e24dd024 	sub	sp, sp, #36	; 0x24
    8bb4:	e1822403 	orr	r2, r2, r3, lsl #8
    8bb8:	e3a0301d 	mov	r3, #29	; 0x1d
    8bbc:	e3a01000 	mov	r1, #0	; 0x0
    8bc0:	e58d3004 	str	r3, [sp, #4]
    8bc4:	e3a03006 	mov	r3, #6	; 0x6
    8bc8:	e58d2008 	str	r2, [sp, #8]
    8bcc:	e1cd30b0 	strh	r3, [sp]
    8bd0:	e1a02001 	mov	r2, r1
    8bd4:	e58d101c 	str	r1, [sp, #28]
    8bd8:	e1a0100d 	mov	r1, sp
    8bdc:	e1a04000 	mov	r4, r0
    8be0:	e1a0e00f 	mov	lr, pc
    8be4:	e590f104 	ldr	pc, [r0, #260]
    8be8:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8bec:	e1a05000 	mov	r5, r0
    8bf0:	e1a00004 	mov	r0, r4
    8bf4:	ebffff2e 	bl	88b4 <mmc_send_status>
    8bf8:	e1a00005 	mov	r0, r5
    8bfc:	e28dd024 	add	sp, sp, #36	; 0x24
    8c00:	e8bd8030 	pop	{r4, r5, pc}

00008c04 <mmc_change_freq>:
    8c04:	e92d4070 	push	{r4, r5, r6, lr}
    8c08:	e5903050 	ldr	r3, [r0, #80]
    8c0c:	e24ddc02 	sub	sp, sp, #512	; 0x200
    8c10:	e3130b01 	tst	r3, #1024	; 0x400
    8c14:	e3a03000 	mov	r3, #0	; 0x0
    8c18:	e1a04000 	mov	r4, r0
    8c1c:	e580304c 	str	r3, [r0, #76]
    8c20:	1a000020 	bne	8ca8 <mmc_change_freq+0xa4>
    8c24:	e5902030 	ldr	r2, [r0, #48]
    8c28:	e59f3084 	ldr	r3, [pc, #132]	; 8cb4 <mmc_change_freq+0xb0>
    8c2c:	e1520003 	cmp	r2, r3
    8c30:	9a00001c 	bls	8ca8 <mmc_change_freq+0xa4>
    8c34:	e3a03c01 	mov	r3, #256	; 0x100
    8c38:	e580304c 	str	r3, [r0, #76]
    8c3c:	e1a0100d 	mov	r1, sp
    8c40:	ebfffd12 	bl	8090 <mmc_send_ext_csd>
    8c44:	e3500000 	cmp	r0, #0	; 0x0
    8c48:	e1a0600d 	mov	r6, sp
    8c4c:	1a000016 	bne	8cac <mmc_change_freq+0xa8>
    8c50:	e3a01001 	mov	r1, #1	; 0x1
    8c54:	e1a00004 	mov	r0, r4
    8c58:	e3a020b9 	mov	r2, #185	; 0xb9
    8c5c:	e1a03001 	mov	r3, r1
    8c60:	e5dd50c4 	ldrb	r5, [sp, #196]
    8c64:	ebffffce 	bl	8ba4 <mmc_switch>
    8c68:	e3500000 	cmp	r0, #0	; 0x0
    8c6c:	1a00000e 	bne	8cac <mmc_change_freq+0xa8>
    8c70:	e1a0100d 	mov	r1, sp
    8c74:	e1a00004 	mov	r0, r4
    8c78:	ebfffd04 	bl	8090 <mmc_send_ext_csd>
    8c7c:	e3500000 	cmp	r0, #0	; 0x0
    8c80:	1a000009 	bne	8cac <mmc_change_freq+0xa8>
    8c84:	e5dd30b9 	ldrb	r3, [sp, #185]
    8c88:	e3530000 	cmp	r3, #0	; 0x0
    8c8c:	0a000006 	beq	8cac <mmc_change_freq+0xa8>
    8c90:	e594304c 	ldr	r3, [r4, #76]
    8c94:	e3150002 	tst	r5, #2	; 0x2
    8c98:	13833011 	orrne	r3, r3, #17	; 0x11
    8c9c:	03833001 	orreq	r3, r3, #1	; 0x1
    8ca0:	e584304c 	str	r3, [r4, #76]
    8ca4:	ea000000 	b	8cac <mmc_change_freq+0xa8>
    8ca8:	e3a00000 	mov	r0, #0	; 0x0
    8cac:	e28ddc02 	add	sp, sp, #512	; 0x200
    8cb0:	e8bd8070 	pop	{r4, r5, r6, pc}
    8cb4:	0001003f 	.word	0x0001003f

00008cb8 <mmc_startup>:
    8cb8:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    8cbc:	e5903050 	ldr	r3, [r0, #80]
    8cc0:	e24ddf8e 	sub	sp, sp, #568	; 0x238
    8cc4:	e3130b01 	tst	r3, #1024	; 0x400
    8cc8:	13a0300a 	movne	r3, #10	; 0xa
    8ccc:	03a03002 	moveq	r3, #2	; 0x2
    8cd0:	e28d1c02 	add	r1, sp, #512	; 0x200
    8cd4:	e3a02000 	mov	r2, #0	; 0x0
    8cd8:	e1c131b0 	strh	r3, [r1, #16]
    8cdc:	e28d5e21 	add	r5, sp, #528	; 0x210
    8ce0:	e3a03007 	mov	r3, #7	; 0x7
    8ce4:	e58d3214 	str	r3, [sp, #532]
    8ce8:	e58d2218 	str	r2, [sp, #536]
    8cec:	e58d222c 	str	r2, [sp, #556]
    8cf0:	e1a01005 	mov	r1, r5
    8cf4:	e1a06000 	mov	r6, r0
    8cf8:	e1a0e00f 	mov	lr, pc
    8cfc:	e590f104 	ldr	pc, [r0, #260]
    8d00:	e2504000 	subs	r4, r0, #0	; 0x0
    8d04:	1a000159 	bne	9270 <mmc_startup+0x5b8>
    8d08:	e3a02010 	mov	r2, #16	; 0x10
    8d0c:	e2860070 	add	r0, r6, #112	; 0x70
    8d10:	e285100c 	add	r1, r5, #12	; 0xc
    8d14:	ebffe20e 	bl	1554 <memcpy>
    8d18:	e5963050 	ldr	r3, [r6, #80]
    8d1c:	e2132b01 	ands	r2, r3, #1024	; 0x400
    8d20:	1a000013 	bne	8d74 <mmc_startup+0xbc>
    8d24:	e1d638b0 	ldrh	r3, [r6, #128]
    8d28:	e3a00003 	mov	r0, #3	; 0x3
    8d2c:	e1a03803 	lsl	r3, r3, #16
    8d30:	e58d3218 	str	r3, [sp, #536]
    8d34:	e3a03015 	mov	r3, #21	; 0x15
    8d38:	e58d3214 	str	r3, [sp, #532]
    8d3c:	e28d3c02 	add	r3, sp, #512	; 0x200
    8d40:	e1c301b0 	strh	r0, [r3, #16]
    8d44:	e58d222c 	str	r2, [sp, #556]
    8d48:	e1a01005 	mov	r1, r5
    8d4c:	e1a00006 	mov	r0, r6
    8d50:	e1a0e00f 	mov	lr, pc
    8d54:	e596f104 	ldr	pc, [r6, #260]
    8d58:	e2504000 	subs	r4, r0, #0	; 0x0
    8d5c:	1a000143 	bne	9270 <mmc_startup+0x5b8>
    8d60:	e5963030 	ldr	r3, [r6, #48]
    8d64:	e3130802 	tst	r3, #131072	; 0x20000
    8d68:	128d1c02 	addne	r1, sp, #512	; 0x200
    8d6c:	11d111be 	ldrhne	r1, [r1, #30]
    8d70:	11c618b0 	strhne	r1, [r6, #128]
    8d74:	e1d638b0 	ldrh	r3, [r6, #128]
    8d78:	e3a01000 	mov	r1, #0	; 0x0
    8d7c:	e1a03803 	lsl	r3, r3, #16
    8d80:	e3a02007 	mov	r2, #7	; 0x7
    8d84:	e58d3218 	str	r3, [sp, #536]
    8d88:	e3a00009 	mov	r0, #9	; 0x9
    8d8c:	e28d3c02 	add	r3, sp, #512	; 0x200
    8d90:	e1c301b0 	strh	r0, [r3, #16]
    8d94:	e58d2214 	str	r2, [sp, #532]
    8d98:	e58d122c 	str	r1, [sp, #556]
    8d9c:	e1a02001 	mov	r2, r1
    8da0:	e1a00006 	mov	r0, r6
    8da4:	e28d1e21 	add	r1, sp, #528	; 0x210
    8da8:	e1a0e00f 	mov	lr, pc
    8dac:	e596f104 	ldr	pc, [r6, #260]
    8db0:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8db4:	e1a04000 	mov	r4, r0
    8db8:	e1a00006 	mov	r0, r6
    8dbc:	ebfffebc 	bl	88b4 <mmc_send_status>
    8dc0:	e3540000 	cmp	r4, #0	; 0x0
    8dc4:	1a000129 	bne	9270 <mmc_startup+0x5b8>
    8dc8:	e5963030 	ldr	r3, [r6, #48]
    8dcc:	e59d021c 	ldr	r0, [sp, #540]
    8dd0:	e59d2224 	ldr	r2, [sp, #548]
    8dd4:	e59d1228 	ldr	r1, [sp, #552]
    8dd8:	e3530801 	cmp	r3, #65536	; 0x10000
    8ddc:	e59d3220 	ldr	r3, [sp, #544]
    8de0:	e5862068 	str	r2, [r6, #104]
    8de4:	e5863064 	str	r3, [r6, #100]
    8de8:	e586106c 	str	r1, [r6, #108]
    8dec:	e5860060 	str	r0, [r6, #96]
    8df0:	1a000013 	bne	8e44 <mmc_startup+0x18c>
    8df4:	e1a03d20 	lsr	r3, r0, #26
    8df8:	e203300f 	and	r3, r3, #15	; 0xf
    8dfc:	e3530004 	cmp	r3, #4	; 0x4
    8e00:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    8e04:	ea00000c 	b	8e3c <mmc_startup+0x184>
    8e08:	00008e3c 	.word	0x00008e3c
    8e0c:	00008e1c 	.word	0x00008e1c
    8e10:	00008e24 	.word	0x00008e24
    8e14:	00008e2c 	.word	0x00008e2c
    8e18:	00008e34 	.word	0x00008e34
    8e1c:	e59f3458 	ldr	r3, [pc, #1112]	; 927c <mmc_startup+0x5c4>
    8e20:	ea000006 	b	8e40 <mmc_startup+0x188>
    8e24:	e59f3454 	ldr	r3, [pc, #1108]	; 9280 <mmc_startup+0x5c8>
    8e28:	ea000004 	b	8e40 <mmc_startup+0x188>
    8e2c:	e59f3450 	ldr	r3, [pc, #1104]	; 9284 <mmc_startup+0x5cc>
    8e30:	ea000002 	b	8e40 <mmc_startup+0x188>
    8e34:	e59f344c 	ldr	r3, [pc, #1100]	; 9288 <mmc_startup+0x5d0>
    8e38:	ea000000 	b	8e40 <mmc_startup+0x188>
    8e3c:	e59f3448 	ldr	r3, [pc, #1096]	; 928c <mmc_startup+0x5d4>
    8e40:	e5863030 	str	r3, [r6, #48]
    8e44:	e28d1c02 	add	r1, sp, #512	; 0x200
    8e48:	e1d132b2 	ldrh	r3, [r1, #34]
    8e4c:	e3a0e001 	mov	lr, #1	; 0x1
    8e50:	e203300f 	and	r3, r3, #15	; 0xf
    8e54:	e1a0c31e 	lsl	ip, lr, r3
    8e58:	e59d121c 	ldr	r1, [sp, #540]
    8e5c:	e59f242c 	ldr	r2, [pc, #1068]	; 9290 <mmc_startup+0x5d8>
    8e60:	e1a031a1 	lsr	r3, r1, #3
    8e64:	e203300f 	and	r3, r3, #15	; 0xf
    8e68:	e7920103 	ldr	r0, [r2, r3, lsl #2]
    8e6c:	e59f3420 	ldr	r3, [pc, #1056]	; 9294 <mmc_startup+0x5dc>
    8e70:	e2011007 	and	r1, r1, #7	; 0x7
    8e74:	e7933101 	ldr	r3, [r3, r1, lsl #2]
    8e78:	e5962030 	ldr	r2, [r6, #48]
    8e7c:	e0030093 	mul	r3, r3, r0
    8e80:	e3120802 	tst	r2, #131072	; 0x20000
    8e84:	e5863084 	str	r3, [r6, #132]
    8e88:	e586c088 	str	ip, [r6, #136]
    8e8c:	059d3228 	ldreq	r3, [sp, #552]
    8e90:	1586c08c 	strne	ip, [r6, #140]
    8e94:	01a03b23 	lsreq	r3, r3, #22
    8e98:	0203300f 	andeq	r3, r3, #15	; 0xf
    8e9c:	01a0331e 	lsleq	r3, lr, r3
    8ea0:	0586308c 	streq	r3, [r6, #140]
    8ea4:	e5963040 	ldr	r3, [r6, #64]
    8ea8:	e5962064 	ldr	r2, [r6, #100]
    8eac:	e3530000 	cmp	r3, #0	; 0x0
    8eb0:	e5960068 	ldr	r0, [r6, #104]
    8eb4:	0a000007 	beq	8ed8 <mmc_startup+0x220>
    8eb8:	e202203f 	and	r2, r2, #63	; 0x3f
    8ebc:	e1a03820 	lsr	r3, r0, #16
    8ec0:	e1833802 	orr	r3, r3, r2, lsl #16
    8ec4:	e1a08003 	mov	r8, r3
    8ec8:	e3a09000 	mov	r9, #0	; 0x0
    8ecc:	e3a04008 	mov	r4, #8	; 0x8
    8ed0:	e3a05000 	mov	r5, #0	; 0x0
    8ed4:	ea000009 	b	8f00 <mmc_startup+0x248>
    8ed8:	e1a03b02 	lsl	r3, r2, #22
    8edc:	e1a03b23 	lsr	r3, r3, #22
    8ee0:	e200190e 	and	r1, r0, #229376	; 0x38000
    8ee4:	e1a02f20 	lsr	r2, r0, #30
    8ee8:	e1822103 	orr	r2, r2, r3, lsl #2
    8eec:	e1a017a1 	lsr	r1, r1, #15
    8ef0:	e1a08002 	mov	r8, r2
    8ef4:	e3a09000 	mov	r9, #0	; 0x0
    8ef8:	e1a04001 	mov	r4, r1
    8efc:	e3a05000 	mov	r5, #0	; 0x0
    8f00:	e3a02001 	mov	r2, #1	; 0x1
    8f04:	e0980002 	adds	r0, r8, r2
    8f08:	e3a03000 	mov	r3, #0	; 0x0
    8f0c:	e0a91003 	adc	r1, r9, r3
    8f10:	e2842002 	add	r2, r4, #2	; 0x2
    8f14:	eb002f3a 	bl	14c04 <__aeabi_llsl>
    8f18:	e596c088 	ldr	ip, [r6, #136]
    8f1c:	e0832c90 	umull	r2, r3, r0, ip
    8f20:	e023319c 	mla	r3, ip, r1, r3
    8f24:	e35c0c02 	cmp	ip, #512	; 0x200
    8f28:	e1c629f0 	strd	r2, [r6, #144]
    8f2c:	83a03c02 	movhi	r3, #512	; 0x200
    8f30:	85863088 	strhi	r3, [r6, #136]
    8f34:	e596308c 	ldr	r3, [r6, #140]
    8f38:	e3530c02 	cmp	r3, #512	; 0x200
    8f3c:	83a03c02 	movhi	r3, #512	; 0x200
    8f40:	8586308c 	strhi	r3, [r6, #140]
    8f44:	e5963050 	ldr	r3, [r6, #80]
    8f48:	e2131b01 	ands	r1, r3, #1024	; 0x400
    8f4c:	1a00000f 	bne	8f90 <mmc_startup+0x2d8>
    8f50:	e1d638b0 	ldrh	r3, [r6, #128]
    8f54:	e3a0201d 	mov	r2, #29	; 0x1d
    8f58:	e1a03803 	lsl	r3, r3, #16
    8f5c:	e58d2214 	str	r2, [sp, #532]
    8f60:	e58d3218 	str	r3, [sp, #536]
    8f64:	e28d2c02 	add	r2, sp, #512	; 0x200
    8f68:	e3a03007 	mov	r3, #7	; 0x7
    8f6c:	e1c231b0 	strh	r3, [r2, #16]
    8f70:	e58d122c 	str	r1, [sp, #556]
    8f74:	e1a02001 	mov	r2, r1
    8f78:	e1a00006 	mov	r0, r6
    8f7c:	e28d1e21 	add	r1, sp, #528	; 0x210
    8f80:	e1a0e00f 	mov	lr, pc
    8f84:	e596f104 	ldr	pc, [r6, #260]
    8f88:	e2504000 	subs	r4, r0, #0	; 0x0
    8f8c:	1a0000b7 	bne	9270 <mmc_startup+0x5b8>
    8f90:	e5962030 	ldr	r2, [r6, #48]
    8f94:	e3e03000 	mvn	r3, #0	; 0x0
    8f98:	e3120802 	tst	r2, #131072	; 0x20000
    8f9c:	e5c63082 	strb	r3, [r6, #130]
    8fa0:	1a00001e 	bne	9020 <mmc_startup+0x368>
    8fa4:	e59f32ec 	ldr	r3, [pc, #748]	; 9298 <mmc_startup+0x5e0>
    8fa8:	e1520003 	cmp	r2, r3
    8fac:	9a00001b 	bls	9020 <mmc_startup+0x368>
    8fb0:	e28d1010 	add	r1, sp, #16	; 0x10
    8fb4:	e1a00006 	mov	r0, r6
    8fb8:	ebfffc34 	bl	8090 <mmc_send_ext_csd>
    8fbc:	e5dd30d0 	ldrb	r3, [sp, #208]
    8fc0:	e2701001 	rsbs	r1, r0, #1	; 0x1
    8fc4:	33a01000 	movcc	r1, #0	; 0x0
    8fc8:	e3530001 	cmp	r3, #1	; 0x1
    8fcc:	93a01000 	movls	r1, #0	; 0x0
    8fd0:	e3510000 	cmp	r1, #0	; 0x0
    8fd4:	0a00000d 	beq	9010 <mmc_startup+0x358>
    8fd8:	e5dd20e6 	ldrb	r2, [sp, #230]
    8fdc:	e5dd10e5 	ldrb	r1, [sp, #229]
    8fe0:	e5dd00e4 	ldrb	r0, [sp, #228]
    8fe4:	e1a02802 	lsl	r2, r2, #16
    8fe8:	e1822401 	orr	r2, r2, r1, lsl #8
    8fec:	e5dd10e7 	ldrb	r1, [sp, #231]
    8ff0:	e1822000 	orr	r2, r2, r0
    8ff4:	e1822c01 	orr	r2, r2, r1, lsl #24
    8ff8:	e1a03fc2 	asr	r3, r2, #31
    8ffc:	e1a01483 	lsl	r1, r3, #9
    9000:	e1811ba2 	orr	r1, r1, r2, lsr #23
    9004:	e1a02482 	lsl	r2, r2, #9
    9008:	e5861094 	str	r1, [r6, #148]
    900c:	e5862090 	str	r2, [r6, #144]
    9010:	e5dd30b0 	ldrb	r3, [sp, #176]
    9014:	e3130001 	tst	r3, #1	; 0x1
    9018:	15dd30c3 	ldrbne	r3, [sp, #195]
    901c:	15c63082 	strbne	r3, [r6, #130]
    9020:	e5963030 	ldr	r3, [r6, #48]
    9024:	e3130802 	tst	r3, #131072	; 0x20000
    9028:	0a000002 	beq	9038 <mmc_startup+0x380>
    902c:	e1a00006 	mov	r0, r6
    9030:	ebfffce8 	bl	83d8 <sd_change_freq>
    9034:	ea000001 	b	9040 <mmc_startup+0x388>
    9038:	e1a00006 	mov	r0, r6
    903c:	ebfffef0 	bl	8c04 <mmc_change_freq>
    9040:	e3500000 	cmp	r0, #0	; 0x0
    9044:	e1a05000 	mov	r5, r0
    9048:	11a04000 	movne	r4, r0
    904c:	1a000087 	bne	9270 <mmc_startup+0x5b8>
    9050:	e596304c 	ldr	r3, [r6, #76]
    9054:	e5962050 	ldr	r2, [r6, #80]
    9058:	e5961030 	ldr	r1, [r6, #48]
    905c:	e0022003 	and	r2, r2, r3
    9060:	e3110802 	tst	r1, #131072	; 0x20000
    9064:	e586204c 	str	r2, [r6, #76]
    9068:	e2023c01 	and	r3, r2, #256	; 0x100
    906c:	0a00002c 	beq	9124 <mmc_startup+0x46c>
    9070:	e3530000 	cmp	r3, #0	; 0x0
    9074:	0a000023 	beq	9108 <mmc_startup+0x450>
    9078:	e1d638b0 	ldrh	r3, [r6, #128]
    907c:	e28d2c02 	add	r2, sp, #512	; 0x200
    9080:	e1a03803 	lsl	r3, r3, #16
    9084:	e58d3218 	str	r3, [sp, #536]
    9088:	e3a08015 	mov	r8, #21	; 0x15
    908c:	e3a03037 	mov	r3, #55	; 0x37
    9090:	e28d9e21 	add	r9, sp, #528	; 0x210
    9094:	e1c231b0 	strh	r3, [r2, #16]
    9098:	e58d022c 	str	r0, [sp, #556]
    909c:	e58d8214 	str	r8, [sp, #532]
    90a0:	e1a00006 	mov	r0, r6
    90a4:	e1a01009 	mov	r1, r9
    90a8:	e1a02005 	mov	r2, r5
    90ac:	e1a0e00f 	mov	lr, pc
    90b0:	e596f104 	ldr	pc, [r6, #260]
    90b4:	e2504000 	subs	r4, r0, #0	; 0x0
    90b8:	1a00006c 	bne	9270 <mmc_startup+0x5b8>
    90bc:	e3a03002 	mov	r3, #2	; 0x2
    90c0:	e3a01006 	mov	r1, #6	; 0x6
    90c4:	e28d0c02 	add	r0, sp, #512	; 0x200
    90c8:	e1c011b0 	strh	r1, [r0, #16]
    90cc:	e58d8214 	str	r8, [sp, #532]
    90d0:	e58d3218 	str	r3, [sp, #536]
    90d4:	e58d522c 	str	r5, [sp, #556]
    90d8:	e1a01009 	mov	r1, r9
    90dc:	e1a02005 	mov	r2, r5
    90e0:	e1a00006 	mov	r0, r6
    90e4:	e1a0e00f 	mov	lr, pc
    90e8:	e596f104 	ldr	pc, [r6, #260]
    90ec:	e2504000 	subs	r4, r0, #0	; 0x0
    90f0:	1a00005e 	bne	9270 <mmc_startup+0x5b8>
    90f4:	e3a03004 	mov	r3, #4	; 0x4
    90f8:	e5863044 	str	r3, [r6, #68]
    90fc:	e1a00006 	mov	r0, r6
    9100:	e1a0e00f 	mov	lr, pc
    9104:	e596f108 	ldr	pc, [r6, #264]
    9108:	e596304c 	ldr	r3, [r6, #76]
    910c:	e3130001 	tst	r3, #1	; 0x1
    9110:	11a00006 	movne	r0, r6
    9114:	159f1180 	ldrne	r1, [pc, #384]	; 929c <mmc_startup+0x5e4>
    9118:	01a00006 	moveq	r0, r6
    911c:	059f117c 	ldreq	r1, [pc, #380]	; 92a0 <mmc_startup+0x5e8>
    9120:	ea000022 	b	91b0 <mmc_startup+0x4f8>
    9124:	e3530000 	cmp	r3, #0	; 0x0
    9128:	0a000008 	beq	9150 <mmc_startup+0x498>
    912c:	e3a01001 	mov	r1, #1	; 0x1
    9130:	e1a03001 	mov	r3, r1
    9134:	e1a00006 	mov	r0, r6
    9138:	e3a020b7 	mov	r2, #183	; 0xb7
    913c:	ebfffe98 	bl	8ba4 <mmc_switch>
    9140:	e2504000 	subs	r4, r0, #0	; 0x0
    9144:	03a03004 	moveq	r3, #4	; 0x4
    9148:	1a000048 	bne	9270 <mmc_startup+0x5b8>
    914c:	ea000009 	b	9178 <mmc_startup+0x4c0>
    9150:	e3120c02 	tst	r2, #512	; 0x200
    9154:	0a00000b 	beq	9188 <mmc_startup+0x4d0>
    9158:	e1a00006 	mov	r0, r6
    915c:	e3a01001 	mov	r1, #1	; 0x1
    9160:	e3a020b7 	mov	r2, #183	; 0xb7
    9164:	e3a03002 	mov	r3, #2	; 0x2
    9168:	ebfffe8d 	bl	8ba4 <mmc_switch>
    916c:	e2504000 	subs	r4, r0, #0	; 0x0
    9170:	1a00003e 	bne	9270 <mmc_startup+0x5b8>
    9174:	e3a03008 	mov	r3, #8	; 0x8
    9178:	e5863044 	str	r3, [r6, #68]
    917c:	e1a00006 	mov	r0, r6
    9180:	e1a0e00f 	mov	lr, pc
    9184:	e596f108 	ldr	pc, [r6, #264]
    9188:	e596304c 	ldr	r3, [r6, #76]
    918c:	e3130001 	tst	r3, #1	; 0x1
    9190:	01a00006 	moveq	r0, r6
    9194:	059f1108 	ldreq	r1, [pc, #264]	; 92a4 <mmc_startup+0x5ec>
    9198:	0a000004 	beq	91b0 <mmc_startup+0x4f8>
    919c:	e3130010 	tst	r3, #16	; 0x10
    91a0:	159f1100 	ldrne	r1, [pc, #256]	; 92a8 <mmc_startup+0x5f0>
    91a4:	059f1100 	ldreq	r1, [pc, #256]	; 92ac <mmc_startup+0x5f4>
    91a8:	11a00006 	movne	r0, r6
    91ac:	01a00006 	moveq	r0, r6
    91b0:	ebfffbed 	bl	816c <mmc_set_clock>
    91b4:	e5964094 	ldr	r4, [r6, #148]
    91b8:	e5960090 	ldr	r0, [r6, #144]
    91bc:	e5961088 	ldr	r1, [r6, #136]
    91c0:	e3a03000 	mov	r3, #0	; 0x0
    91c4:	e3540000 	cmp	r4, #0	; 0x0
    91c8:	e5c630a3 	strb	r3, [r6, #163]
    91cc:	e5c630a2 	strb	r3, [r6, #162]
    91d0:	e58610ac 	str	r1, [r6, #172]
    91d4:	e58d0230 	str	r0, [sp, #560]
    91d8:	e58d4234 	str	r4, [sp, #564]
    91dc:	1a000003 	bne	91f0 <mmc_startup+0x538>
    91e0:	eb002d69 	bl	1478c <__aeabi_uidiv>
    91e4:	e58d4234 	str	r4, [sp, #564]
    91e8:	e58d0230 	str	r0, [sp, #560]
    91ec:	ea000001 	b	91f8 <mmc_startup+0x540>
    91f0:	e28d0e23 	add	r0, sp, #560	; 0x230
    91f4:	eb002123 	bl	11688 <__div64_32>
    91f8:	e5d6007f 	ldrb	r0, [r6, #127]
    91fc:	e59d1230 	ldr	r1, [sp, #560]
    9200:	e5962070 	ldr	r2, [r6, #112]
    9204:	e5963078 	ldr	r3, [r6, #120]
    9208:	e58610a8 	str	r1, [r6, #168]
    920c:	e1803403 	orr	r3, r0, r3, lsl #8
    9210:	e1a02422 	lsr	r2, r2, #8
    9214:	e59f1094 	ldr	r1, [pc, #148]	; 92b0 <mmc_startup+0x5f8>
    9218:	e28600b0 	add	r0, r6, #176	; 0xb0
    921c:	eb0024e2 	bl	125ac <sprintf>
    9220:	e596c074 	ldr	ip, [r6, #116]
    9224:	e5d62070 	ldrb	r2, [r6, #112]
    9228:	e1a0482c 	lsr	r4, ip, #16
    922c:	e1a0542c 	lsr	r5, ip, #8
    9230:	e1a03c2c 	lsr	r3, ip, #24
    9234:	e20440ff 	and	r4, r4, #255	; 0xff
    9238:	e20cc0ff 	and	ip, ip, #255	; 0xff
    923c:	e59f1070 	ldr	r1, [pc, #112]	; 92b4 <mmc_startup+0x5fc>
    9240:	e20550ff 	and	r5, r5, #255	; 0xff
    9244:	e28600d9 	add	r0, r6, #217	; 0xd9
    9248:	e88d1030 	stm	sp, {r4, r5, ip}
    924c:	eb0024d6 	bl	125ac <sprintf>
    9250:	e5962078 	ldr	r2, [r6, #120]
    9254:	e28600ee 	add	r0, r6, #238	; 0xee
    9258:	e1a03c22 	lsr	r3, r2, #24
    925c:	e203300f 	and	r3, r3, #15	; 0xf
    9260:	e1a02e22 	lsr	r2, r2, #28
    9264:	e59f104c 	ldr	r1, [pc, #76]	; 92b8 <mmc_startup+0x600>
    9268:	eb0024cf 	bl	125ac <sprintf>
    926c:	e3a04000 	mov	r4, #0	; 0x0
    9270:	e1a00004 	mov	r0, r4
    9274:	e28ddf8e 	add	sp, sp, #568	; 0x238
    9278:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    927c:	00010014 	.word	0x00010014
    9280:	00010022 	.word	0x00010022
    9284:	00010030 	.word	0x00010030
    9288:	00010040 	.word	0x00010040
    928c:	00010012 	.word	0x00010012
    9290:	00015cd4 	.word	0x00015cd4
    9294:	00015cc4 	.word	0x00015cc4
    9298:	0001003f 	.word	0x0001003f
    929c:	02faf080 	.word	0x02faf080
    92a0:	017d7840 	.word	0x017d7840
    92a4:	01312d00 	.word	0x01312d00
    92a8:	03197500 	.word	0x03197500
    92ac:	018cba80 	.word	0x018cba80
    92b0:	00018043 	.word	0x00018043
    92b4:	00018055 	.word	0x00018055
    92b8:	00018060 	.word	0x00018060

000092bc <mmc_init>:
    92bc:	e92d4010 	push	{r4, lr}
    92c0:	ebfffb5a 	bl	8030 <find_mmc_device>
    92c4:	e1a04000 	mov	r4, r0
    92c8:	e1a0e00f 	mov	lr, pc
    92cc:	e590f10c 	ldr	pc, [r0, #268]
    92d0:	e3500000 	cmp	r0, #0	; 0x0
    92d4:	18bd8010 	popne	{r4, pc}
    92d8:	e3a03001 	mov	r3, #1	; 0x1
    92dc:	e5843044 	str	r3, [r4, #68]
    92e0:	e1a00004 	mov	r0, r4
    92e4:	e1a0e00f 	mov	lr, pc
    92e8:	e594f108 	ldr	pc, [r4, #264]
    92ec:	e1a00004 	mov	r0, r4
    92f0:	e59f1060 	ldr	r1, [pc, #96]	; 9358 <mmc_init+0x9c>
    92f4:	ebfffb9c 	bl	816c <mmc_set_clock>
    92f8:	e1a00004 	mov	r0, r4
    92fc:	ebfffd09 	bl	8728 <mmc_go_idle>
    9300:	e3500000 	cmp	r0, #0	; 0x0
    9304:	18bd8010 	popne	{r4, pc}
    9308:	e5c40083 	strb	r0, [r4, #131]
    930c:	e1a00004 	mov	r0, r4
    9310:	ebfffba9 	bl	81bc <mmc_send_if_cond>
    9314:	e1a00004 	mov	r0, r4
    9318:	ebfffcad 	bl	85d4 <sd_send_op_cond>
    931c:	e3700013 	cmn	r0, #19	; 0x13
    9320:	1a000009 	bne	934c <mmc_init+0x90>
    9324:	e1a00004 	mov	r0, r4
    9328:	ebfffd13 	bl	877c <mmc_send_op_cond>
    932c:	e3500000 	cmp	r0, #0	; 0x0
    9330:	0a000005 	beq	934c <mmc_init+0x90>
    9334:	e59f0020 	ldr	r0, [pc, #32]	; 935c <mmc_init+0xa0>
    9338:	e59f3020 	ldr	r3, [pc, #32]	; 9360 <mmc_init+0xa4>
    933c:	e1a0e00f 	mov	lr, pc
    9340:	e593f000 	ldr	pc, [r3]
    9344:	e3e00010 	mvn	r0, #16	; 0x10
    9348:	e8bd8010 	pop	{r4, pc}
    934c:	e1a00004 	mov	r0, r4
    9350:	e8bd4010 	pop	{r4, lr}
    9354:	eafffe57 	b	8cb8 <mmc_startup>
    9358:	00061a80 	.word	0x00061a80
    935c:	00018066 	.word	0x00018066
    9360:	3c00d6cc 	.word	0x3c00d6cc

00009364 <mmc2_vs_reset>:
    9364:	e3a00002 	mov	r0, #2	; 0x2
    9368:	eaffffd3 	b	92bc <mmc_init>

0000936c <mmc1_vs_reset>:
    936c:	e3a00001 	mov	r0, #1	; 0x1
    9370:	eaffffd1 	b	92bc <mmc_init>

00009374 <mmc0_vs_reset>:
    9374:	e3a00000 	mov	r0, #0	; 0x0
    9378:	eaffffcf 	b	92bc <mmc_init>

0000937c <mmc_read_blocks>:
    937c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    9380:	e3530001 	cmp	r3, #1	; 0x1
    9384:	e24dd030 	sub	sp, sp, #48	; 0x30
    9388:	e1a06003 	mov	r6, r3
    938c:	83a03012 	movhi	r3, #18	; 0x12
    9390:	93a03011 	movls	r3, #17	; 0x11
    9394:	e1cd30b0 	strh	r3, [sp]
    9398:	e5903040 	ldr	r3, [r0, #64]
    939c:	e3a07000 	mov	r7, #0	; 0x0
    93a0:	e3530000 	cmp	r3, #0	; 0x0
    93a4:	05903088 	ldreq	r3, [r0, #136]
    93a8:	158d2008 	strne	r2, [sp, #8]
    93ac:	00030293 	muleq	r3, r3, r2
    93b0:	058d3008 	streq	r3, [sp, #8]
    93b4:	e5903088 	ldr	r3, [r0, #136]
    93b8:	e3a02015 	mov	r2, #21	; 0x15
    93bc:	e58d302c 	str	r3, [sp, #44]
    93c0:	e3a03001 	mov	r3, #1	; 0x1
    93c4:	e58d2004 	str	r2, [sp, #4]
    93c8:	e58d1020 	str	r1, [sp, #32]
    93cc:	e1a04000 	mov	r4, r0
    93d0:	e58d3024 	str	r3, [sp, #36]
    93d4:	e58d701c 	str	r7, [sp, #28]
    93d8:	e58d6028 	str	r6, [sp, #40]
    93dc:	e1a0100d 	mov	r1, sp
    93e0:	e28d2020 	add	r2, sp, #32	; 0x20
    93e4:	e1a0e00f 	mov	lr, pc
    93e8:	e594f104 	ldr	pc, [r4, #260]
    93ec:	e2505000 	subs	r5, r0, #0	; 0x0
    93f0:	e1a0800d 	mov	r8, sp
    93f4:	11a00007 	movne	r0, r7
    93f8:	1a000018 	bne	9460 <mmc_read_blocks+0xe4>
    93fc:	e3560001 	cmp	r6, #1	; 0x1
    9400:	9a000012 	bls	9450 <mmc_read_blocks+0xd4>
    9404:	e3a0301d 	mov	r3, #29	; 0x1d
    9408:	e58d3004 	str	r3, [sp, #4]
    940c:	e3a0300c 	mov	r3, #12	; 0xc
    9410:	e1cd30b0 	strh	r3, [sp]
    9414:	e58d5008 	str	r5, [sp, #8]
    9418:	e58d501c 	str	r5, [sp, #28]
    941c:	e1a0100d 	mov	r1, sp
    9420:	e1a00004 	mov	r0, r4
    9424:	e1a02005 	mov	r2, r5
    9428:	e1a0e00f 	mov	lr, pc
    942c:	e594f104 	ldr	pc, [r4, #260]
    9430:	e3500000 	cmp	r0, #0	; 0x0
    9434:	0a000005 	beq	9450 <mmc_read_blocks+0xd4>
    9438:	e59f0028 	ldr	r0, [pc, #40]	; 9468 <mmc_read_blocks+0xec>
    943c:	e59f3028 	ldr	r3, [pc, #40]	; 946c <mmc_read_blocks+0xf0>
    9440:	e1a0e00f 	mov	lr, pc
    9444:	e593f000 	ldr	pc, [r3]
    9448:	e1a00005 	mov	r0, r5
    944c:	ea000003 	b	9460 <mmc_read_blocks+0xe4>
    9450:	e1a00004 	mov	r0, r4
    9454:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    9458:	ebfffd15 	bl	88b4 <mmc_send_status>
    945c:	e1a00006 	mov	r0, r6
    9460:	e28dd030 	add	sp, sp, #48	; 0x30
    9464:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    9468:	00018028 	.word	0x00018028
    946c:	3c00d6cc 	.word	0x3c00d6cc

00009470 <mmc_bread>:
    9470:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    9474:	e252a000 	subs	sl, r2, #0	; 0x0
    9478:	e24dd004 	sub	sp, sp, #4	; 0x4
    947c:	e1a07001 	mov	r7, r1
    9480:	e1a08003 	mov	r8, r3
    9484:	0a000027 	beq	9528 <mmc_bread+0xb8>
    9488:	ebfffae8 	bl	8030 <find_mmc_device>
    948c:	e2506000 	subs	r6, r0, #0	; 0x0
    9490:	0a000024 	beq	9528 <mmc_bread+0xb8>
    9494:	e59620a8 	ldr	r2, [r6, #168]
    9498:	e08a1007 	add	r1, sl, r7
    949c:	e1510002 	cmp	r1, r2
    94a0:	9a000004 	bls	94b8 <mmc_bread+0x48>
    94a4:	e59f0088 	ldr	r0, [pc, #136]	; 9534 <mmc_bread+0xc4>
    94a8:	e59f3088 	ldr	r3, [pc, #136]	; 9538 <mmc_bread+0xc8>
    94ac:	e1a0e00f 	mov	lr, pc
    94b0:	e593f000 	ldr	pc, [r3]
    94b4:	ea00001b 	b	9528 <mmc_bread+0xb8>
    94b8:	e5961088 	ldr	r1, [r6, #136]
    94bc:	ebfffacd 	bl	7ff8 <mmc_set_blocklen>
    94c0:	e3500000 	cmp	r0, #0	; 0x0
    94c4:	01a0500a 	moveq	r5, sl
    94c8:	0a000005 	beq	94e4 <mmc_bread+0x74>
    94cc:	e5961088 	ldr	r1, [r6, #136]
    94d0:	e59f0064 	ldr	r0, [pc, #100]	; 953c <mmc_bread+0xcc>
    94d4:	e59f305c 	ldr	r3, [pc, #92]	; 9538 <mmc_bread+0xc8>
    94d8:	e1a0e00f 	mov	lr, pc
    94dc:	e593f000 	ldr	pc, [r3]
    94e0:	ea000010 	b	9528 <mmc_bread+0xb8>
    94e4:	e5964110 	ldr	r4, [r6, #272]
    94e8:	e1a02007 	mov	r2, r7
    94ec:	e1550004 	cmp	r5, r4
    94f0:	31a04005 	movcc	r4, r5
    94f4:	e1a01008 	mov	r1, r8
    94f8:	e1a00006 	mov	r0, r6
    94fc:	e1a03004 	mov	r3, r4
    9500:	ebffff9d 	bl	937c <mmc_read_blocks>
    9504:	e1500004 	cmp	r0, r4
    9508:	e0877000 	add	r7, r7, r0
    950c:	1a000005 	bne	9528 <mmc_bread+0xb8>
    9510:	e0555000 	subs	r5, r5, r0
    9514:	e5962088 	ldr	r2, [r6, #136]
    9518:	10288092 	mlane	r8, r2, r0, r8
    951c:	1afffff0 	bne	94e4 <mmc_bread+0x74>
    9520:	e000029a 	mul	r0, sl, r2
    9524:	ea000000 	b	952c <mmc_bread+0xbc>
    9528:	e3a00000 	mov	r0, #0	; 0x0
    952c:	e28dd004 	add	sp, sp, #4	; 0x4
    9530:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    9534:	00017fea 	.word	0x00017fea
    9538:	3c00d6cc 	.word	0x3c00d6cc
    953c:	0001808f 	.word	0x0001808f

00009540 <mmc2_vs_read>:
    9540:	e1a024a2 	lsr	r2, r2, #9
    9544:	e1821b83 	orr	r1, r2, r3, lsl #23
    9548:	e59d2000 	ldr	r2, [sp]
    954c:	e1a03000 	mov	r3, r0
    9550:	e1a024c2 	asr	r2, r2, #9
    9554:	e3a00002 	mov	r0, #2	; 0x2
    9558:	eaffffc4 	b	9470 <mmc_bread>

0000955c <mmc1_vs_read>:
    955c:	e1a024a2 	lsr	r2, r2, #9
    9560:	e1821b83 	orr	r1, r2, r3, lsl #23
    9564:	e59d2000 	ldr	r2, [sp]
    9568:	e1a03000 	mov	r3, r0
    956c:	e1a024c2 	asr	r2, r2, #9
    9570:	e3a00001 	mov	r0, #1	; 0x1
    9574:	eaffffbd 	b	9470 <mmc_bread>

00009578 <mmc0_vs_read>:
    9578:	e1a024a2 	lsr	r2, r2, #9
    957c:	e1821b83 	orr	r1, r2, r3, lsl #23
    9580:	e59d2000 	ldr	r2, [sp]
    9584:	e1a03000 	mov	r3, r0
    9588:	e1a024c2 	asr	r2, r2, #9
    958c:	e3a00000 	mov	r0, #0	; 0x0
    9590:	eaffffb6 	b	9470 <mmc_bread>

00009594 <mci_pull_data32>:
    9594:	e1a02142 	asr	r2, r2, #2
    9598:	e3a0c000 	mov	ip, #0	; 0x0
    959c:	ea000003 	b	95b0 <mci_pull_data32+0x1c>
    95a0:	e5903118 	ldr	r3, [r0, #280]
    95a4:	e5933200 	ldr	r3, [r3, #512]
    95a8:	e781300c 	str	r3, [r1, ip]
    95ac:	e28cc004 	add	ip, ip, #4	; 0x4
    95b0:	e3520000 	cmp	r2, #0	; 0x0
    95b4:	e2422001 	sub	r2, r2, #1	; 0x1
    95b8:	cafffff8 	bgt	95a0 <mci_pull_data32+0xc>
    95bc:	e12fff1e 	bx	lr

000095c0 <mci_push_data32>:
    95c0:	e92d4010 	push	{r4, lr}
    95c4:	e3a0c000 	mov	ip, #0	; 0x0
    95c8:	e1a04000 	mov	r4, r0
    95cc:	e1a00001 	mov	r0, r1
    95d0:	e1a01142 	asr	r1, r2, #2
    95d4:	ea000003 	b	95e8 <mci_push_data32+0x28>
    95d8:	e790200c 	ldr	r2, [r0, ip]
    95dc:	e5943118 	ldr	r3, [r4, #280]
    95e0:	e28cc004 	add	ip, ip, #4	; 0x4
    95e4:	e5832200 	str	r2, [r3, #512]
    95e8:	e3510000 	cmp	r1, #0	; 0x0
    95ec:	e2411001 	sub	r1, r1, #1	; 0x1
    95f0:	cafffff8 	bgt	95d8 <mci_push_data32+0x18>
    95f4:	e8bd8010 	pop	{r4, pc}

000095f8 <imap_sdhc_initialize>:
    95f8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    95fc:	e3510000 	cmp	r1, #0	; 0x0
    9600:	e24dd004 	sub	sp, sp, #4	; 0x4
    9604:	1a000005 	bne	9620 <imap_sdhc_initialize+0x28>
    9608:	e59f00ec 	ldr	r0, [pc, #236]	; 96fc <imap_sdhc_initialize+0x104>
    960c:	e59f30ec 	ldr	r3, [pc, #236]	; 9700 <imap_sdhc_initialize+0x108>
    9610:	e1a0e00f 	mov	lr, pc
    9614:	e593f000 	ldr	pc, [r3]
    9618:	e3e00004 	mvn	r0, #4	; 0x4
    961c:	ea000034 	b	96f4 <imap_sdhc_initialize+0xfc>
    9620:	e3500000 	cmp	r0, #0	; 0x0
    9624:	03a0359d 	moveq	r3, #658505728	; 0x27400000
    9628:	0a000005 	beq	9644 <imap_sdhc_initialize+0x4c>
    962c:	e3500001 	cmp	r0, #1	; 0x1
    9630:	059f30cc 	ldreq	r3, [pc, #204]	; 9704 <imap_sdhc_initialize+0x10c>
    9634:	0a000002 	beq	9644 <imap_sdhc_initialize+0x4c>
    9638:	e3500002 	cmp	r0, #2	; 0x2
    963c:	1a000018 	bne	96a4 <imap_sdhc_initialize+0xac>
    9640:	e59f30c0 	ldr	r3, [pc, #192]	; 9708 <imap_sdhc_initialize+0x110>
    9644:	e5813118 	str	r3, [r1, #280]
    9648:	e59f30bc 	ldr	r3, [pc, #188]	; 970c <imap_sdhc_initialize+0x114>
    964c:	e591c130 	ldr	ip, [r1, #304]
    9650:	e5813104 	str	r3, [r1, #260]
    9654:	e59f30b4 	ldr	r3, [pc, #180]	; 9710 <imap_sdhc_initialize+0x118>
    9658:	e3a02000 	mov	r2, #0	; 0x0
    965c:	e5813108 	str	r3, [r1, #264]
    9660:	e59f30ac 	ldr	r3, [pc, #172]	; 9714 <imap_sdhc_initialize+0x11c>
    9664:	e5810114 	str	r0, [r1, #276]
    9668:	e581310c 	str	r3, [r1, #268]
    966c:	e59f30a4 	ldr	r3, [pc, #164]	; 9718 <imap_sdhc_initialize+0x120>
    9670:	e5812034 	str	r2, [r1, #52]
    9674:	e581302c 	str	r3, [r1, #44]
    9678:	e3003101 	movw	r3, #257	; 0x101
    967c:	e5813050 	str	r3, [r1, #80]
    9680:	e59f3094 	ldr	r3, [pc, #148]	; 971c <imap_sdhc_initialize+0x124>
    9684:	e5812028 	str	r2, [r1, #40]
    9688:	e5813038 	str	r3, [r1, #56]
    968c:	e59f308c 	ldr	r3, [pc, #140]	; 9720 <imap_sdhc_initialize+0x128>
    9690:	e5812110 	str	r2, [r1, #272]
    9694:	e581303c 	str	r3, [r1, #60]
    9698:	e1a0000c 	mov	r0, ip
    969c:	e2822010 	add	r2, r2, #16	; 0x10
    96a0:	ea000005 	b	96bc <imap_sdhc_initialize+0xc4>
    96a4:	e59f0078 	ldr	r0, [pc, #120]	; 9724 <imap_sdhc_initialize+0x12c>
    96a8:	e59f3050 	ldr	r3, [pc, #80]	; 9700 <imap_sdhc_initialize+0x108>
    96ac:	e1a0e00f 	mov	lr, pc
    96b0:	e593f000 	ldr	pc, [r3]
    96b4:	e3e00000 	mvn	r0, #0	; 0x0
    96b8:	ea00000d 	b	96f4 <imap_sdhc_initialize+0xfc>
    96bc:	e5913130 	ldr	r3, [r1, #304]
    96c0:	e0823003 	add	r3, r2, r3
    96c4:	e2822010 	add	r2, r2, #16	; 0x10
    96c8:	e3520b01 	cmp	r2, #1024	; 0x400
    96cc:	e580300c 	str	r3, [r0, #12]
    96d0:	e2800010 	add	r0, r0, #16	; 0x10
    96d4:	1afffff8 	bne	96bc <imap_sdhc_initialize+0xc4>
    96d8:	e5912130 	ldr	r2, [r1, #304]
    96dc:	e3a03020 	mov	r3, #32	; 0x20
    96e0:	e58c33f0 	str	r3, [ip, #1008]
    96e4:	e58c23fc 	str	r2, [ip, #1020]
    96e8:	e1a00001 	mov	r0, r1
    96ec:	ebfffad0 	bl	8234 <mmc_register>
    96f0:	e3a00000 	mov	r0, #0	; 0x0
    96f4:	e28dd004 	add	sp, sp, #4	; 0x4
    96f8:	e8bd8000 	pop	{pc}
    96fc:	00018099 	.word	0x00018099
    9700:	3c00d6cc 	.word	0x3c00d6cc
    9704:	27200000 	.word	0x27200000
    9708:	27300000 	.word	0x27300000
    970c:	00009798 	.word	0x00009798
    9710:	00009c70 	.word	0x00009c70
    9714:	00009ab8 	.word	0x00009ab8
    9718:	00ff8000 	.word	0x00ff8000
    971c:	00061a80 	.word	0x00061a80
    9720:	018cba80 	.word	0x018cba80
    9724:	000180af 	.word	0x000180af

00009728 <mci_send_cmd>:
    9728:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    972c:	e5903118 	ldr	r3, [r0, #280]
    9730:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    9734:	e5832028 	str	r2, [r3, #40]
    9738:	e5903118 	ldr	r3, [r0, #280]
    973c:	e24dd004 	sub	sp, sp, #4	; 0x4
    9740:	e583102c 	str	r1, [r3, #44]
    9744:	e1a04000 	mov	r4, r0
    9748:	eb001092 	bl	d998 <get_ticks>
    974c:	e59f5040 	ldr	r5, [pc, #64]	; 9794 <mci_send_cmd+0x6c>
    9750:	e1a06000 	mov	r6, r0
    9754:	e1a07001 	mov	r7, r1
    9758:	e5943118 	ldr	r3, [r4, #280]
    975c:	e593302c 	ldr	r3, [r3, #44]
    9760:	e3530000 	cmp	r3, #0	; 0x0
    9764:	aa000008 	bge	978c <mci_send_cmd+0x64>
    9768:	eb00108a 	bl	d998 <get_ticks>
    976c:	e1a02000 	mov	r2, r0
    9770:	e1a03001 	mov	r3, r1
    9774:	e0522006 	subs	r2, r2, r6
    9778:	e0c33007 	sbc	r3, r3, r7
    977c:	e3530000 	cmp	r3, #0	; 0x0
    9780:	1a000001 	bne	978c <mci_send_cmd+0x64>
    9784:	e1520005 	cmp	r2, r5
    9788:	9afffff2 	bls	9758 <mci_send_cmd+0x30>
    978c:	e28dd004 	add	sp, sp, #4	; 0x4
    9790:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    9794:	0007a11f 	.word	0x0007a11f

00009798 <imap_sdhc_send_cmd>:
    9798:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    979c:	e252a000 	subs	sl, r2, #0	; 0x0
    97a0:	e1a04000 	mov	r4, r0
    97a4:	e1a08001 	mov	r8, r1
    97a8:	0a000039 	beq	9894 <imap_sdhc_send_cmd+0xfc>
    97ac:	e99a000c 	ldmib	sl, {r2, r3}
    97b0:	e59a100c 	ldr	r1, [sl, #12]
    97b4:	e3120001 	tst	r2, #1	; 0x1
    97b8:	e0000193 	mul	r0, r3, r1
    97bc:	15942118 	ldrne	r2, [r4, #280]
    97c0:	11a03801 	lslne	r3, r1, #16
    97c4:	13833001 	orrne	r3, r3, #1	; 0x1
    97c8:	15823100 	strne	r3, [r2, #256]
    97cc:	e59a200c 	ldr	r2, [sl, #12]
    97d0:	e5943118 	ldr	r3, [r4, #280]
    97d4:	e1a0e000 	mov	lr, r0
    97d8:	e583201c 	str	r2, [r3, #28]
    97dc:	e5943118 	ldr	r3, [r4, #280]
    97e0:	e3a05000 	mov	r5, #0	; 0x0
    97e4:	e5830020 	str	r0, [r3, #32]
    97e8:	e5942118 	ldr	r2, [r4, #280]
    97ec:	e3019ff0 	movw	r9, #8176	; 0x1ff0
    97f0:	e5923000 	ldr	r3, [r2]
    97f4:	e3a0714a 	mov	r7, #-2147483630	; 0x80000012
    97f8:	e3833020 	orr	r3, r3, #32	; 0x20
    97fc:	e5823000 	str	r3, [r2]
    9800:	e59a6000 	ldr	r6, [sl]
    9804:	e594c130 	ldr	ip, [r4, #304]
    9808:	e15e0009 	cmp	lr, r9
    980c:	31a0100e 	movcc	r1, lr
    9810:	21a01009 	movcs	r1, r9
    9814:	e59c0004 	ldr	r0, [ip, #4]
    9818:	e59f3288 	ldr	r3, [pc, #648]	; 9aa8 <imap_sdhc_send_cmd+0x310>
    981c:	e1a02981 	lsl	r2, r1, #19
    9820:	e0003003 	and	r3, r0, r3
    9824:	e1a029a2 	lsr	r2, r2, #19
    9828:	e1822003 	orr	r2, r2, r3
    982c:	e05ee001 	subs	lr, lr, r1
    9830:	e0853006 	add	r3, r5, r6
    9834:	e98c000c 	stmib	ip, {r2, r3}
    9838:	e58c7000 	str	r7, [ip]
    983c:	e1a0200c 	mov	r2, ip
    9840:	e0855001 	add	r5, r5, r1
    9844:	e28cc010 	add	ip, ip, #16	; 0x10
    9848:	1affffee 	bne	9808 <imap_sdhc_send_cmd+0x70>
    984c:	e3c73002 	bic	r3, r7, #2	; 0x2
    9850:	e3833004 	orr	r3, r3, #4	; 0x4
    9854:	e5823000 	str	r3, [r2]
    9858:	e5942130 	ldr	r2, [r4, #304]
    985c:	e5923000 	ldr	r3, [r2]
    9860:	e3833008 	orr	r3, r3, #8	; 0x8
    9864:	e5823000 	str	r3, [r2]
    9868:	e5942118 	ldr	r2, [r4, #280]
    986c:	e5923000 	ldr	r3, [r2]
    9870:	e3833402 	orr	r3, r3, #33554432	; 0x2000000
    9874:	e5823000 	str	r3, [r2]
    9878:	e5942118 	ldr	r2, [r4, #280]
    987c:	e5923080 	ldr	r3, [r2, #128]
    9880:	e3833080 	orr	r3, r3, #128	; 0x80
    9884:	e5823080 	str	r3, [r2, #128]
    9888:	e5942118 	ldr	r2, [r4, #280]
    988c:	e3a03001 	mov	r3, #1	; 0x1
    9890:	e5823084 	str	r3, [r2, #132]
    9894:	e1d800b0 	ldrh	r0, [r8]
    9898:	e5982004 	ldr	r2, [r8, #4]
    989c:	e350000c 	cmp	r0, #12	; 0xc
    98a0:	03a03901 	moveq	r3, #16384	; 0x4000
    98a4:	13a03a02 	movne	r3, #8192	; 0x2000
    98a8:	e3120004 	tst	r2, #4	; 0x4
    98ac:	13833c01 	orrne	r3, r3, #256	; 0x100
    98b0:	e3120001 	tst	r2, #1	; 0x1
    98b4:	e3831202 	orr	r1, r3, #536870912	; 0x20000000
    98b8:	0a000002 	beq	98c8 <imap_sdhc_send_cmd+0x130>
    98bc:	e3120002 	tst	r2, #2	; 0x2
    98c0:	e3811040 	orr	r1, r1, #64	; 0x40
    98c4:	13811080 	orrne	r1, r1, #128	; 0x80
    98c8:	e35a0000 	cmp	sl, #0	; 0x0
    98cc:	0a000003 	beq	98e0 <imap_sdhc_send_cmd+0x148>
    98d0:	e59a3004 	ldr	r3, [sl, #4]
    98d4:	e3811c02 	orr	r1, r1, #512	; 0x200
    98d8:	e3130002 	tst	r3, #2	; 0x2
    98dc:	13811b01 	orrne	r1, r1, #1024	; 0x400
    98e0:	e594312c 	ldr	r3, [r4, #300]
    98e4:	e1811000 	orr	r1, r1, r0
    98e8:	e3530000 	cmp	r3, #0	; 0x0
    98ec:	13a03000 	movne	r3, #0	; 0x0
    98f0:	1584312c 	strne	r3, [r4, #300]
    98f4:	e5982008 	ldr	r2, [r8, #8]
    98f8:	e5943118 	ldr	r3, [r4, #280]
    98fc:	13811902 	orrne	r1, r1, #32768	; 0x8000
    9900:	e5832028 	str	r2, [r3, #40]
    9904:	e5943118 	ldr	r3, [r4, #280]
    9908:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    990c:	e583102c 	str	r1, [r3, #44]
    9910:	eb001020 	bl	d998 <get_ticks>
    9914:	e3005bb8 	movw	r5, #3000	; 0xbb8
    9918:	e1a06000 	mov	r6, r0
    991c:	e1a07001 	mov	r7, r1
    9920:	eb00101c 	bl	d998 <get_ticks>
    9924:	e1a02000 	mov	r2, r0
    9928:	e1a03001 	mov	r3, r1
    992c:	e0522006 	subs	r2, r2, r6
    9930:	e0c33007 	sbc	r3, r3, r7
    9934:	e3530000 	cmp	r3, #0	; 0x0
    9938:	1a000007 	bne	995c <imap_sdhc_send_cmd+0x1c4>
    993c:	e1520005 	cmp	r2, r5
    9940:	8a000005 	bhi	995c <imap_sdhc_send_cmd+0x1c4>
    9944:	e5943118 	ldr	r3, [r4, #280]
    9948:	e5932044 	ldr	r2, [r3, #68]
    994c:	e2831044 	add	r1, r3, #68	; 0x44
    9950:	e3120004 	tst	r2, #4	; 0x4
    9954:	0afffff1 	beq	9920 <imap_sdhc_send_cmd+0x188>
    9958:	ea000005 	b	9974 <imap_sdhc_send_cmd+0x1dc>
    995c:	e59f0148 	ldr	r0, [pc, #328]	; 9aac <imap_sdhc_send_cmd+0x314>
    9960:	e59f3148 	ldr	r3, [pc, #328]	; 9ab0 <imap_sdhc_send_cmd+0x318>
    9964:	e1a0e00f 	mov	lr, pc
    9968:	e593f000 	ldr	pc, [r3]
    996c:	e3e00011 	mvn	r0, #17	; 0x11
    9970:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    9974:	e5915000 	ldr	r5, [r1]
    9978:	e3003142 	movw	r3, #322	; 0x142
    997c:	e0053003 	and	r3, r5, r3
    9980:	e3530000 	cmp	r3, #0	; 0x0
    9984:	e3a03004 	mov	r3, #4	; 0x4
    9988:	e5813000 	str	r3, [r1]
    998c:	0a00000e 	beq	99cc <imap_sdhc_send_cmd+0x234>
    9990:	e1d810b0 	ldrh	r1, [r8]
    9994:	e5943118 	ldr	r3, [r4, #280]
    9998:	e3510037 	cmp	r1, #55	; 0x37
    999c:	13510008 	cmpne	r1, #8	; 0x8
    99a0:	e5835044 	str	r5, [r3, #68]
    99a4:	0a000004 	beq	99bc <imap_sdhc_send_cmd+0x224>
    99a8:	e59f0104 	ldr	r0, [pc, #260]	; 9ab4 <imap_sdhc_send_cmd+0x31c>
    99ac:	e1a02005 	mov	r2, r5
    99b0:	e59f30f8 	ldr	r3, [pc, #248]	; 9ab0 <imap_sdhc_send_cmd+0x318>
    99b4:	e1a0e00f 	mov	lr, pc
    99b8:	e593f000 	ldr	pc, [r3]
    99bc:	e3150c01 	tst	r5, #256	; 0x100
    99c0:	13e00012 	mvnne	r0, #18	; 0x12
    99c4:	03e00011 	mvneq	r0, #17	; 0x11
    99c8:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    99cc:	e5983004 	ldr	r3, [r8, #4]
    99d0:	e5942118 	ldr	r2, [r4, #280]
    99d4:	e3130002 	tst	r3, #2	; 0x2
    99d8:	05923030 	ldreq	r3, [r2, #48]
    99dc:	0588300c 	streq	r3, [r8, #12]
    99e0:	0a00000a 	beq	9a10 <imap_sdhc_send_cmd+0x278>
    99e4:	e592303c 	ldr	r3, [r2, #60]
    99e8:	e588300c 	str	r3, [r8, #12]
    99ec:	e5943118 	ldr	r3, [r4, #280]
    99f0:	e5933038 	ldr	r3, [r3, #56]
    99f4:	e5883010 	str	r3, [r8, #16]
    99f8:	e5943118 	ldr	r3, [r4, #280]
    99fc:	e5933034 	ldr	r3, [r3, #52]
    9a00:	e5883014 	str	r3, [r8, #20]
    9a04:	e5943118 	ldr	r3, [r4, #280]
    9a08:	e5933030 	ldr	r3, [r3, #48]
    9a0c:	e5883018 	str	r3, [r8, #24]
    9a10:	e35a0000 	cmp	sl, #0	; 0x0
    9a14:	01a0000a 	moveq	r0, sl
    9a18:	08bd87f0 	popeq	{r4, r5, r6, r7, r8, r9, sl, pc}
    9a1c:	e594c118 	ldr	ip, [r4, #280]
    9a20:	e59a3004 	ldr	r3, [sl, #4]
    9a24:	e28c7044 	add	r7, ip, #68	; 0x44
    9a28:	e2030001 	and	r0, r3, #1	; 0x1
    9a2c:	e28c608c 	add	r6, ip, #140	; 0x8c
    9a30:	e5975000 	ldr	r5, [r7]
    9a34:	e3500000 	cmp	r0, #0	; 0x0
    9a38:	e596e000 	ldr	lr, [r6]
    9a3c:	e2051008 	and	r1, r5, #8	; 0x8
    9a40:	0a000005 	beq	9a5c <imap_sdhc_send_cmd+0x2c4>
    9a44:	e3510000 	cmp	r1, #0	; 0x0
    9a48:	0afffff8 	beq	9a30 <imap_sdhc_send_cmd+0x298>
    9a4c:	e31e0002 	tst	lr, #2	; 0x2
    9a50:	0afffff6 	beq	9a30 <imap_sdhc_send_cmd+0x298>
    9a54:	e3a03002 	mov	r3, #2	; 0x2
    9a58:	ea000004 	b	9a70 <imap_sdhc_send_cmd+0x2d8>
    9a5c:	e3510000 	cmp	r1, #0	; 0x0
    9a60:	0afffff2 	beq	9a30 <imap_sdhc_send_cmd+0x298>
    9a64:	e31e0001 	tst	lr, #1	; 0x1
    9a68:	0afffff0 	beq	9a30 <imap_sdhc_send_cmd+0x298>
    9a6c:	e3a03001 	mov	r3, #1	; 0x1
    9a70:	e58c308c 	str	r3, [ip, #140]
    9a74:	e5943118 	ldr	r3, [r4, #280]
    9a78:	e3a00000 	mov	r0, #0	; 0x0
    9a7c:	e5835044 	str	r5, [r3, #68]
    9a80:	e5942118 	ldr	r2, [r4, #280]
    9a84:	e5923000 	ldr	r3, [r2]
    9a88:	e3c33402 	bic	r3, r3, #33554432	; 0x2000000
    9a8c:	e3833004 	orr	r3, r3, #4	; 0x4
    9a90:	e5823000 	str	r3, [r2]
    9a94:	e5942118 	ldr	r2, [r4, #280]
    9a98:	e5923080 	ldr	r3, [r2, #128]
    9a9c:	e3c33080 	bic	r3, r3, #128	; 0x80
    9aa0:	e5823080 	str	r3, [r2, #128]
    9aa4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    9aa8:	03ffe000 	.word	0x03ffe000
    9aac:	000180c1 	.word	0x000180c1
    9ab0:	3c00d6cc 	.word	0x3c00d6cc
    9ab4:	000180cf 	.word	0x000180cf

00009ab8 <imap_sdhc_init>:
    9ab8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    9abc:	e5901114 	ldr	r1, [r0, #276]
    9ac0:	e3a02000 	mov	r2, #0	; 0x0
    9ac4:	e3a03001 	mov	r3, #1	; 0x1
    9ac8:	e3510001 	cmp	r1, #1	; 0x1
    9acc:	e24dd004 	sub	sp, sp, #4	; 0x4
    9ad0:	e1a04000 	mov	r4, r0
    9ad4:	e5802048 	str	r2, [r0, #72]
    9ad8:	e580312c 	str	r3, [r0, #300]
    9adc:	e580211c 	str	r2, [r0, #284]
    9ae0:	0a000007 	beq	9b04 <imap_sdhc_init+0x4c>
    9ae4:	3a000002 	bcc	9af4 <imap_sdhc_init+0x3c>
    9ae8:	e3510002 	cmp	r1, #2	; 0x2
    9aec:	1a00000c 	bne	9b24 <imap_sdhc_init+0x6c>
    9af0:	ea000007 	b	9b14 <imap_sdhc_init+0x5c>
    9af4:	e3a0000d 	mov	r0, #13	; 0xd
    9af8:	ebffdb66 	bl	898 <pads_init_module>
    9afc:	e59f0148 	ldr	r0, [pc, #328]	; 9c4c <imap_sdhc_init+0x194>
    9b00:	ea000006 	b	9b20 <imap_sdhc_init+0x68>
    9b04:	e3a0000e 	mov	r0, #14	; 0xe
    9b08:	ebffdb62 	bl	898 <pads_init_module>
    9b0c:	e59f013c 	ldr	r0, [pc, #316]	; 9c50 <imap_sdhc_init+0x198>
    9b10:	ea000002 	b	9b20 <imap_sdhc_init+0x68>
    9b14:	e3a0000f 	mov	r0, #15	; 0xf
    9b18:	ebffdb5e 	bl	898 <pads_init_module>
    9b1c:	e59f0130 	ldr	r0, [pc, #304]	; 9c54 <imap_sdhc_init+0x19c>
    9b20:	ebffdb16 	bl	780 <module_enable>
    9b24:	e5942118 	ldr	r2, [r4, #280]
    9b28:	e59f5128 	ldr	r5, [pc, #296]	; 9c58 <imap_sdhc_init+0x1a0>
    9b2c:	e5923070 	ldr	r3, [r2, #112]
    9b30:	e1a03943 	asr	r3, r3, #18
    9b34:	e2033007 	and	r3, r3, #7	; 0x7
    9b38:	e3530002 	cmp	r3, #2	; 0x2
    9b3c:	03a03003 	moveq	r3, #3	; 0x3
    9b40:	13a03002 	movne	r3, #2	; 0x2
    9b44:	e5843120 	str	r3, [r4, #288]
    9b48:	e3a03007 	mov	r3, #7	; 0x7
    9b4c:	e5823000 	str	r3, [r2]
    9b50:	e59f3104 	ldr	r3, [pc, #260]	; 9c5c <imap_sdhc_init+0x1a4>
    9b54:	e5843124 	str	r3, [r4, #292]
    9b58:	e59f3100 	ldr	r3, [pc, #256]	; 9c60 <imap_sdhc_init+0x1a8>
    9b5c:	e5843128 	str	r3, [r4, #296]
    9b60:	eb000f8c 	bl	d998 <get_ticks>
    9b64:	e1a06000 	mov	r6, r0
    9b68:	e1a07001 	mov	r7, r1
    9b6c:	e5943118 	ldr	r3, [r4, #280]
    9b70:	e3a00001 	mov	r0, #1	; 0x1
    9b74:	e5933000 	ldr	r3, [r3]
    9b78:	e3130007 	tst	r3, #7	; 0x7
    9b7c:	0a00000d 	beq	9bb8 <imap_sdhc_init+0x100>
    9b80:	eb000f95 	bl	d9dc <udelay>
    9b84:	eb000f83 	bl	d998 <get_ticks>
    9b88:	e1a02000 	mov	r2, r0
    9b8c:	e1a03001 	mov	r3, r1
    9b90:	e0522006 	subs	r2, r2, r6
    9b94:	e0c33007 	sbc	r3, r3, r7
    9b98:	e3530000 	cmp	r3, #0	; 0x0
    9b9c:	1a000001 	bne	9ba8 <imap_sdhc_init+0xf0>
    9ba0:	e1520005 	cmp	r2, r5
    9ba4:	9afffff0 	bls	9b6c <imap_sdhc_init+0xb4>
    9ba8:	e59f00b4 	ldr	r0, [pc, #180]	; 9c64 <imap_sdhc_init+0x1ac>
    9bac:	e59f30b4 	ldr	r3, [pc, #180]	; 9c68 <imap_sdhc_init+0x1b0>
    9bb0:	e1a0e00f 	mov	lr, pc
    9bb4:	e593f000 	ldr	pc, [r3]
    9bb8:	e5943118 	ldr	r3, [r4, #280]
    9bbc:	e3a01000 	mov	r1, #0	; 0x0
    9bc0:	e5831024 	str	r1, [r3, #36]
    9bc4:	e5943118 	ldr	r3, [r4, #280]
    9bc8:	e3e02000 	mvn	r2, #0	; 0x0
    9bcc:	e5832044 	str	r2, [r3, #68]
    9bd0:	e5943118 	ldr	r3, [r4, #280]
    9bd4:	e1a00001 	mov	r0, r1
    9bd8:	e5832014 	str	r2, [r3, #20]
    9bdc:	e5942118 	ldr	r2, [r4, #280]
    9be0:	e592304c 	ldr	r3, [r2, #76]
    9be4:	e1a03283 	lsl	r3, r3, #5
    9be8:	e1a03b23 	lsr	r3, r3, #22
    9bec:	e1833803 	orr	r3, r3, r3, lsl #16
    9bf0:	e582304c 	str	r3, [r2, #76]
    9bf4:	e5943118 	ldr	r3, [r4, #280]
    9bf8:	e5831010 	str	r1, [r3, #16]
    9bfc:	e5942118 	ldr	r2, [r4, #280]
    9c00:	e5923004 	ldr	r3, [r2, #4]
    9c04:	e3833001 	orr	r3, r3, #1	; 0x1
    9c08:	e5823004 	str	r3, [r2, #4]
    9c0c:	e5942118 	ldr	r2, [r4, #280]
    9c10:	e59f3054 	ldr	r3, [pc, #84]	; 9c6c <imap_sdhc_init+0x1b4>
    9c14:	e5823058 	str	r3, [r2, #88]
    9c18:	e5942118 	ldr	r2, [r4, #280]
    9c1c:	e3003103 	movw	r3, #259	; 0x103
    9c20:	e5823090 	str	r3, [r2, #144]
    9c24:	e5942130 	ldr	r2, [r4, #304]
    9c28:	e5943118 	ldr	r3, [r4, #280]
    9c2c:	e5832088 	str	r2, [r3, #136]
    9c30:	e5942118 	ldr	r2, [r4, #280]
    9c34:	e5923080 	ldr	r3, [r2, #128]
    9c38:	e3c3307c 	bic	r3, r3, #124	; 0x7c
    9c3c:	e3833008 	orr	r3, r3, #8	; 0x8
    9c40:	e5823080 	str	r3, [r2, #128]
    9c44:	e28dd004 	add	sp, sp, #4	; 0x4
    9c48:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    9c4c:	21e39000 	.word	0x21e39000
    9c50:	21e38800 	.word	0x21e38800
    9c54:	21e38c00 	.word	0x21e38c00
    9c58:	0007a11f 	.word	0x0007a11f
    9c5c:	00009594 	.word	0x00009594
    9c60:	000095c0 	.word	0x000095c0
    9c64:	000180e7 	.word	0x000180e7
    9c68:	3c00d6cc 	.word	0x3c00d6cc
    9c6c:	008c2800 	.word	0x008c2800

00009c70 <imap_sdhc_set_ios>:
    9c70:	e5902048 	ldr	r2, [r0, #72]
    9c74:	e590311c 	ldr	r3, [r0, #284]
    9c78:	e92d4070 	push	{r4, r5, r6, lr}
    9c7c:	e1520003 	cmp	r2, r3
    9c80:	e1a05000 	mov	r5, r0
    9c84:	0a00002a 	beq	9d34 <imap_sdhc_set_ios+0xc4>
    9c88:	e5903114 	ldr	r3, [r0, #276]
    9c8c:	e59f00d0 	ldr	r0, [pc, #208]	; 9d64 <imap_sdhc_set_ios+0xf4>
    9c90:	e0830000 	add	r0, r3, r0
    9c94:	e1a00200 	lsl	r0, r0, #4
    9c98:	ebffdd2a 	bl	1148 <module_get_clock>
    9c9c:	e5954048 	ldr	r4, [r5, #72]
    9ca0:	e1a06000 	mov	r6, r0
    9ca4:	e1a01004 	mov	r1, r4
    9ca8:	eb002b33 	bl	1497c <__aeabi_uidivmod>
    9cac:	e3510000 	cmp	r1, #0	; 0x0
    9cb0:	0a000005 	beq	9ccc <imap_sdhc_set_ios+0x5c>
    9cb4:	e1a00006 	mov	r0, r6
    9cb8:	e1a01004 	mov	r1, r4
    9cbc:	eb002ab2 	bl	1478c <__aeabi_uidiv>
    9cc0:	e1a000a0 	lsr	r0, r0, #1
    9cc4:	e2806001 	add	r6, r0, #1	; 0x1
    9cc8:	ea000003 	b	9cdc <imap_sdhc_set_ios+0x6c>
    9ccc:	e1a00006 	mov	r0, r6
    9cd0:	e1a01004 	mov	r1, r4
    9cd4:	eb002aac 	bl	1478c <__aeabi_uidiv>
    9cd8:	e1a060a0 	lsr	r6, r0, #1
    9cdc:	e5953118 	ldr	r3, [r5, #280]
    9ce0:	e3a04000 	mov	r4, #0	; 0x0
    9ce4:	e5834010 	str	r4, [r3, #16]
    9ce8:	e5953118 	ldr	r3, [r5, #280]
    9cec:	e1a00005 	mov	r0, r5
    9cf0:	e583400c 	str	r4, [r3, #12]
    9cf4:	e59f106c 	ldr	r1, [pc, #108]	; 9d68 <imap_sdhc_set_ios+0xf8>
    9cf8:	e1a02004 	mov	r2, r4
    9cfc:	ebfffe89 	bl	9728 <mci_send_cmd>
    9d00:	e5953118 	ldr	r3, [r5, #280]
    9d04:	e1a00005 	mov	r0, r5
    9d08:	e5836008 	str	r6, [r3, #8]
    9d0c:	e59f1054 	ldr	r1, [pc, #84]	; 9d68 <imap_sdhc_set_ios+0xf8>
    9d10:	e1a02004 	mov	r2, r4
    9d14:	ebfffe83 	bl	9728 <mci_send_cmd>
    9d18:	e5952118 	ldr	r2, [r5, #280]
    9d1c:	e3a03001 	mov	r3, #1	; 0x1
    9d20:	e5823010 	str	r3, [r2, #16]
    9d24:	e1a00005 	mov	r0, r5
    9d28:	e1a02004 	mov	r2, r4
    9d2c:	e59f1034 	ldr	r1, [pc, #52]	; 9d68 <imap_sdhc_set_ios+0xf8>
    9d30:	ebfffe7c 	bl	9728 <mci_send_cmd>
    9d34:	e5953044 	ldr	r3, [r5, #68]
    9d38:	e3530004 	cmp	r3, #4	; 0x4
    9d3c:	05952118 	ldreq	r2, [r5, #280]
    9d40:	03a03001 	moveq	r3, #1	; 0x1
    9d44:	05823018 	streq	r3, [r2, #24]
    9d48:	08bd8070 	popeq	{r4, r5, r6, pc}
    9d4c:	e3530008 	cmp	r3, #8	; 0x8
    9d50:	e5950118 	ldr	r0, [r5, #280]
    9d54:	059f3010 	ldreq	r3, [pc, #16]	; 9d6c <imap_sdhc_set_ios+0xfc>
    9d58:	13a03000 	movne	r3, #0	; 0x0
    9d5c:	e5803018 	str	r3, [r0, #24]
    9d60:	e8bd8070 	pop	{r4, r5, r6, pc}
    9d64:	021e0a24 	.word	0x021e0a24
    9d68:	00202000 	.word	0x00202000
    9d6c:	00010001 	.word	0x00010001

00009d70 <generic_ffs>:
    9d70:	e3500000 	cmp	r0, #0	; 0x0
    9d74:	01a03000 	moveq	r3, r0
    9d78:	0a000010 	beq	9dc0 <generic_ffs+0x50>
    9d7c:	e1a03800 	lsl	r3, r0, #16
    9d80:	e1a03823 	lsr	r3, r3, #16
    9d84:	e3530000 	cmp	r3, #0	; 0x0
    9d88:	01a00840 	asreq	r0, r0, #16
    9d8c:	13a03001 	movne	r3, #1	; 0x1
    9d90:	03a03011 	moveq	r3, #17	; 0x11
    9d94:	e31000ff 	tst	r0, #255	; 0xff
    9d98:	01a00440 	asreq	r0, r0, #8
    9d9c:	02833008 	addeq	r3, r3, #8	; 0x8
    9da0:	e310000f 	tst	r0, #15	; 0xf
    9da4:	01a00240 	asreq	r0, r0, #4
    9da8:	02833004 	addeq	r3, r3, #4	; 0x4
    9dac:	e3100003 	tst	r0, #3	; 0x3
    9db0:	01a00140 	asreq	r0, r0, #2
    9db4:	02833002 	addeq	r3, r3, #2	; 0x2
    9db8:	e3100001 	tst	r0, #1	; 0x1
    9dbc:	02833001 	addeq	r3, r3, #1	; 0x1
    9dc0:	e1a00003 	mov	r0, r3
    9dc4:	e12fff1e 	bx	lr

00009dc8 <print_nand_cfg>:
    9dc8:	e92d4030 	push	{r4, r5, lr}
    9dcc:	e1a05000 	mov	r5, r0
    9dd0:	e24dd004 	sub	sp, sp, #4	; 0x4
    9dd4:	e59f4110 	ldr	r4, [pc, #272]	; 9eec <print_nand_cfg+0x124>
    9dd8:	e59f0110 	ldr	r0, [pc, #272]	; 9ef0 <print_nand_cfg+0x128>
    9ddc:	e1a0e00f 	mov	lr, pc
    9de0:	e594f000 	ldr	pc, [r4]
    9de4:	e5d51000 	ldrb	r1, [r5]
    9de8:	e59f0104 	ldr	r0, [pc, #260]	; 9ef4 <print_nand_cfg+0x12c>
    9dec:	e1a0e00f 	mov	lr, pc
    9df0:	e594f000 	ldr	pc, [r4]
    9df4:	e5d51001 	ldrb	r1, [r5, #1]
    9df8:	e59f00f8 	ldr	r0, [pc, #248]	; 9ef8 <print_nand_cfg+0x130>
    9dfc:	e1a0e00f 	mov	lr, pc
    9e00:	e594f000 	ldr	pc, [r4]
    9e04:	e5d51002 	ldrb	r1, [r5, #2]
    9e08:	e59f00ec 	ldr	r0, [pc, #236]	; 9efc <print_nand_cfg+0x134>
    9e0c:	e1a0e00f 	mov	lr, pc
    9e10:	e594f000 	ldr	pc, [r4]
    9e14:	e5d51003 	ldrb	r1, [r5, #3]
    9e18:	e59f00e0 	ldr	r0, [pc, #224]	; 9f00 <print_nand_cfg+0x138>
    9e1c:	e1a0e00f 	mov	lr, pc
    9e20:	e594f000 	ldr	pc, [r4]
    9e24:	e5d51004 	ldrb	r1, [r5, #4]
    9e28:	e59f00d4 	ldr	r0, [pc, #212]	; 9f04 <print_nand_cfg+0x13c>
    9e2c:	e1a0e00f 	mov	lr, pc
    9e30:	e594f000 	ldr	pc, [r4]
    9e34:	e5d51005 	ldrb	r1, [r5, #5]
    9e38:	e59f00c8 	ldr	r0, [pc, #200]	; 9f08 <print_nand_cfg+0x140>
    9e3c:	e1a0e00f 	mov	lr, pc
    9e40:	e594f000 	ldr	pc, [r4]
    9e44:	e5951008 	ldr	r1, [r5, #8]
    9e48:	e59f00bc 	ldr	r0, [pc, #188]	; 9f0c <print_nand_cfg+0x144>
    9e4c:	e1a0e00f 	mov	lr, pc
    9e50:	e594f000 	ldr	pc, [r4]
    9e54:	e595100c 	ldr	r1, [r5, #12]
    9e58:	e59f00b0 	ldr	r0, [pc, #176]	; 9f10 <print_nand_cfg+0x148>
    9e5c:	e1a0e00f 	mov	lr, pc
    9e60:	e594f000 	ldr	pc, [r4]
    9e64:	e5951010 	ldr	r1, [r5, #16]
    9e68:	e59f00a4 	ldr	r0, [pc, #164]	; 9f14 <print_nand_cfg+0x14c>
    9e6c:	e1a0e00f 	mov	lr, pc
    9e70:	e594f000 	ldr	pc, [r4]
    9e74:	e5951014 	ldr	r1, [r5, #20]
    9e78:	e59f0098 	ldr	r0, [pc, #152]	; 9f18 <print_nand_cfg+0x150>
    9e7c:	e1a0e00f 	mov	lr, pc
    9e80:	e594f000 	ldr	pc, [r4]
    9e84:	e5951018 	ldr	r1, [r5, #24]
    9e88:	e59f008c 	ldr	r0, [pc, #140]	; 9f1c <print_nand_cfg+0x154>
    9e8c:	e1a0e00f 	mov	lr, pc
    9e90:	e594f000 	ldr	pc, [r4]
    9e94:	e595101c 	ldr	r1, [r5, #28]
    9e98:	e59f0080 	ldr	r0, [pc, #128]	; 9f20 <print_nand_cfg+0x158>
    9e9c:	e1a0e00f 	mov	lr, pc
    9ea0:	e594f000 	ldr	pc, [r4]
    9ea4:	e5951028 	ldr	r1, [r5, #40]
    9ea8:	e59f0074 	ldr	r0, [pc, #116]	; 9f24 <print_nand_cfg+0x15c>
    9eac:	e1a0e00f 	mov	lr, pc
    9eb0:	e594f000 	ldr	pc, [r4]
    9eb4:	e5951030 	ldr	r1, [r5, #48]
    9eb8:	e59f0068 	ldr	r0, [pc, #104]	; 9f28 <print_nand_cfg+0x160>
    9ebc:	e1a0e00f 	mov	lr, pc
    9ec0:	e594f000 	ldr	pc, [r4]
    9ec4:	e5951034 	ldr	r1, [r5, #52]
    9ec8:	e59f005c 	ldr	r0, [pc, #92]	; 9f2c <print_nand_cfg+0x164>
    9ecc:	e1a0e00f 	mov	lr, pc
    9ed0:	e594f000 	ldr	pc, [r4]
    9ed4:	e5d51006 	ldrb	r1, [r5, #6]
    9ed8:	e59f0050 	ldr	r0, [pc, #80]	; 9f30 <print_nand_cfg+0x168>
    9edc:	e1a0e00f 	mov	lr, pc
    9ee0:	e594f000 	ldr	pc, [r4]
    9ee4:	e28dd004 	add	sp, sp, #4	; 0x4
    9ee8:	e8bd8030 	pop	{r4, r5, pc}
    9eec:	3c00d6cc 	.word	0x3c00d6cc
    9ef0:	000180fa 	.word	0x000180fa
    9ef4:	0001810c 	.word	0x0001810c
    9ef8:	00018114 	.word	0x00018114
    9efc:	00018124 	.word	0x00018124
    9f00:	0001812f 	.word	0x0001812f
    9f04:	0001813f 	.word	0x0001813f
    9f08:	0001814f 	.word	0x0001814f
    9f0c:	0001815f 	.word	0x0001815f
    9f10:	00018169 	.word	0x00018169
    9f14:	00018173 	.word	0x00018173
    9f18:	00018180 	.word	0x00018180
    9f1c:	0001818e 	.word	0x0001818e
    9f20:	0001819d 	.word	0x0001819d
    9f24:	000181ac 	.word	0x000181ac
    9f28:	000181b8 	.word	0x000181b8
    9f2c:	000181c5 	.word	0x000181c5
    9f30:	000181d3 	.word	0x000181d3

00009f34 <nand_get_config>:
    9f34:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9f38:	e3500000 	cmp	r0, #0	; 0x0
    9f3c:	e24dd004 	sub	sp, sp, #4	; 0x4
    9f40:	0a000007 	beq	9f64 <nand_get_config+0x30>
    9f44:	e3500001 	cmp	r0, #1	; 0x1
    9f48:	1a00000b 	bne	9f7c <nand_get_config+0x48>
    9f4c:	e59f0048 	ldr	r0, [pc, #72]	; 9f9c <nand_get_config+0x68>
    9f50:	e5903000 	ldr	r3, [r0]
    9f54:	e3530000 	cmp	r3, #0	; 0x0
    9f58:	128000b0 	addne	r0, r0, #176	; 0xb0
    9f5c:	0a00000b 	beq	9f90 <nand_get_config+0x5c>
    9f60:	ea00000b 	b	9f94 <nand_get_config+0x60>
    9f64:	e59f0030 	ldr	r0, [pc, #48]	; 9f9c <nand_get_config+0x68>
    9f68:	e5903004 	ldr	r3, [r0, #4]
    9f6c:	e3530000 	cmp	r3, #0	; 0x0
    9f70:	12800014 	addne	r0, r0, #20	; 0x14
    9f74:	0a000005 	beq	9f90 <nand_get_config+0x5c>
    9f78:	ea000005 	b	9f94 <nand_get_config+0x60>
    9f7c:	e1a01000 	mov	r1, r0
    9f80:	e59f3018 	ldr	r3, [pc, #24]	; 9fa0 <nand_get_config+0x6c>
    9f84:	e59f0018 	ldr	r0, [pc, #24]	; 9fa4 <nand_get_config+0x70>
    9f88:	e1a0e00f 	mov	lr, pc
    9f8c:	e593f000 	ldr	pc, [r3]
    9f90:	e3a00000 	mov	r0, #0	; 0x0
    9f94:	e28dd004 	add	sp, sp, #4	; 0x4
    9f98:	e8bd8000 	pop	{pc}
    9f9c:	3c00d734 	.word	0x3c00d734
    9fa0:	3c00d6cc 	.word	0x3c00d6cc
    9fa4:	000181dd 	.word	0x000181dd

00009fa8 <bnd_vs_align>:
    9fa8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9fac:	e3a00000 	mov	r0, #0	; 0x0
    9fb0:	e24dd004 	sub	sp, sp, #4	; 0x4
    9fb4:	ebffffde 	bl	9f34 <nand_get_config>
    9fb8:	e3500000 	cmp	r0, #0	; 0x0
    9fbc:	15900014 	ldrne	r0, [r0, #20]
    9fc0:	e28dd004 	add	sp, sp, #4	; 0x4
    9fc4:	e8bd8000 	pop	{pc}

00009fc8 <nnd_vs_align>:
    9fc8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9fcc:	e3a00001 	mov	r0, #1	; 0x1
    9fd0:	e24dd004 	sub	sp, sp, #4	; 0x4
    9fd4:	ebffffd6 	bl	9f34 <nand_get_config>
    9fd8:	e3500000 	cmp	r0, #0	; 0x0
    9fdc:	15900014 	ldrne	r0, [r0, #20]
    9fe0:	e28dd004 	add	sp, sp, #4	; 0x4
    9fe4:	e8bd8000 	pop	{pc}

00009fe8 <fnd_vs_align>:
    9fe8:	eafffff6 	b	9fc8 <nnd_vs_align>

00009fec <fnd_align>:
    9fec:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9ff0:	e3a00001 	mov	r0, #1	; 0x1
    9ff4:	e24dd004 	sub	sp, sp, #4	; 0x4
    9ff8:	ebffffcd 	bl	9f34 <nand_get_config>
    9ffc:	e3500000 	cmp	r0, #0	; 0x0
    a000:	15902014 	ldrne	r2, [r0, #20]
    a004:	15903010 	ldrne	r3, [r0, #16]
    a008:	10830002 	addne	r0, r3, r2
    a00c:	e28dd004 	add	sp, sp, #4	; 0x4
    a010:	e8bd8000 	pop	{pc}

0000a014 <fnd_size_match>:
    a014:	e92d4010 	push	{r4, lr}
    a018:	e1a04000 	mov	r4, r0
    a01c:	e3a00001 	mov	r0, #1	; 0x1
    a020:	ebffffc3 	bl	9f34 <nand_get_config>
    a024:	e3500000 	cmp	r0, #0	; 0x0
    a028:	08bd8010 	popeq	{r4, pc}
    a02c:	e5903014 	ldr	r3, [r0, #20]
    a030:	e5901010 	ldr	r1, [r0, #16]
    a034:	e1a00004 	mov	r0, r4
    a038:	e0811003 	add	r1, r1, r3
    a03c:	eb002a4e 	bl	1497c <__aeabi_uidivmod>
    a040:	e2710001 	rsbs	r0, r1, #1	; 0x1
    a044:	33a00000 	movcc	r0, #0	; 0x0
    a048:	e8bd8010 	pop	{r4, pc}

0000a04c <fnd_size_shrink>:
    a04c:	e92d4030 	push	{r4, r5, lr}
    a050:	e1a05000 	mov	r5, r0
    a054:	e24dd004 	sub	sp, sp, #4	; 0x4
    a058:	e3a00001 	mov	r0, #1	; 0x1
    a05c:	ebffffb4 	bl	9f34 <nand_get_config>
    a060:	e3500000 	cmp	r0, #0	; 0x0
    a064:	0a000005 	beq	a080 <fnd_size_shrink+0x34>
    a068:	e5904014 	ldr	r4, [r0, #20]
    a06c:	e5901010 	ldr	r1, [r0, #16]
    a070:	e1a00005 	mov	r0, r5
    a074:	e0841001 	add	r1, r4, r1
    a078:	eb0029c3 	bl	1478c <__aeabi_uidiv>
    a07c:	e0000094 	mul	r0, r4, r0
    a080:	e28dd004 	add	sp, sp, #4	; 0x4
    a084:	e8bd8030 	pop	{r4, r5, pc}

0000a088 <init_nand>:
    a088:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    a08c:	e3a01000 	mov	r1, #0	; 0x0
    a090:	e24dd004 	sub	sp, sp, #4	; 0x4
    a094:	e3a02f53 	mov	r2, #332	; 0x14c
    a098:	e59f000c 	ldr	r0, [pc, #12]	; a0ac <init_nand+0x24>
    a09c:	ebffdd24 	bl	1534 <memset>
    a0a0:	e3a00000 	mov	r0, #0	; 0x0
    a0a4:	e28dd004 	add	sp, sp, #4	; 0x4
    a0a8:	e8bd8000 	pop	{pc}
    a0ac:	3c00d734 	.word	0x3c00d734

0000a0b0 <__base_block_markbad>:
    a0b0:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    a0b4:	e24dd07c 	sub	sp, sp, #124	; 0x7c
    a0b8:	e1cd06f0 	strd	r0, [sp, #96]
    a0bc:	e3a00001 	mov	r0, #1	; 0x1
    a0c0:	ebffff9b 	bl	9f34 <nand_get_config>
    a0c4:	e250a000 	subs	sl, r0, #0	; 0x0
    a0c8:	03e00000 	mvneq	r0, #0	; 0x0
    a0cc:	0a000035 	beq	a1a8 <__base_block_markbad+0xf8>
    a0d0:	e3a04000 	mov	r4, #0	; 0x0
    a0d4:	e58d4074 	str	r4, [sp, #116]
    a0d8:	e59a5014 	ldr	r5, [sl, #20]
    a0dc:	e5da7005 	ldrb	r7, [sl, #5]
    a0e0:	e5da8003 	ldrb	r8, [sl, #3]
    a0e4:	e59f30c4 	ldr	r3, [pc, #196]	; a1b0 <__base_block_markbad+0x100>
    a0e8:	e58d5000 	str	r5, [sp]
    a0ec:	e59ac010 	ldr	ip, [sl, #16]
    a0f0:	e5932010 	ldr	r2, [r3, #16]
    a0f4:	e3a06001 	mov	r6, #1	; 0x1
    a0f8:	e1cd06d0 	ldrd	r0, [sp, #96]
    a0fc:	e58dc004 	str	ip, [sp, #4]
    a100:	e58d6008 	str	r6, [sp, #8]
    a104:	eb002ab7 	bl	14be8 <__aeabi_lasr>
    a108:	e58d5014 	str	r5, [sp, #20]
    a10c:	e58d000c 	str	r0, [sp, #12]
    a110:	e58d4010 	str	r4, [sp, #16]
    a114:	e58d7018 	str	r7, [sp, #24]
    a118:	e5da3004 	ldrb	r3, [sl, #4]
    a11c:	e58d4024 	str	r4, [sp, #36]
    a120:	e58d4020 	str	r4, [sp, #32]
    a124:	e58d301c 	str	r3, [sp, #28]
    a128:	e5da3002 	ldrb	r3, [sl, #2]
    a12c:	e28a204c 	add	r2, sl, #76	; 0x4c
    a130:	e58d3028 	str	r3, [sp, #40]
    a134:	e5da3001 	ldrb	r3, [sl, #1]
    a138:	e28a105c 	add	r1, sl, #92	; 0x5c
    a13c:	e58d302c 	str	r3, [sp, #44]
    a140:	e28d3070 	add	r3, sp, #112	; 0x70
    a144:	e58d3048 	str	r3, [sp, #72]
    a148:	e28d306c 	add	r3, sp, #108	; 0x6c
    a14c:	e58d304c 	str	r3, [sp, #76]
    a150:	e28d3074 	add	r3, sp, #116	; 0x74
    a154:	e28a006c 	add	r0, sl, #108	; 0x6c
    a158:	e28ac07c 	add	ip, sl, #124	; 0x7c
    a15c:	e58d3050 	str	r3, [sp, #80]
    a160:	e28a5038 	add	r5, sl, #56	; 0x38
    a164:	e3a03004 	mov	r3, #4	; 0x4
    a168:	e28a408c 	add	r4, sl, #140	; 0x8c
    a16c:	e58d2034 	str	r2, [sp, #52]
    a170:	e58d1038 	str	r1, [sp, #56]
    a174:	e58d003c 	str	r0, [sp, #60]
    a178:	e58d3054 	str	r3, [sp, #84]
    a17c:	e58d5030 	str	r5, [sp, #48]
    a180:	e58dc040 	str	ip, [sp, #64]
    a184:	e58d4044 	str	r4, [sp, #68]
    a188:	e58d6058 	str	r6, [sp, #88]
    a18c:	e5dac006 	ldrb	ip, [sl, #6]
    a190:	e1a01006 	mov	r1, r6
    a194:	e1a00007 	mov	r0, r7
    a198:	e1a02008 	mov	r2, r8
    a19c:	e1a03006 	mov	r3, r6
    a1a0:	e58dc05c 	str	ip, [sp, #92]
    a1a4:	eb000a35 	bl	ca80 <nf2_page_op>
    a1a8:	e28dd07c 	add	sp, sp, #124	; 0x7c
    a1ac:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    a1b0:	3c00d734 	.word	0x3c00d734

0000a1b4 <bnd_vs_write>:
    a1b4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a1b8:	e3a05000 	mov	r5, #0	; 0x0
    a1bc:	e24dd08c 	sub	sp, sp, #140	; 0x8c
    a1c0:	e1a08000 	mov	r8, r0
    a1c4:	e1a00005 	mov	r0, r5
    a1c8:	e1a06002 	mov	r6, r2
    a1cc:	e1a07003 	mov	r7, r3
    a1d0:	e58d5084 	str	r5, [sp, #132]
    a1d4:	e58d5080 	str	r5, [sp, #128]
    a1d8:	e58d507c 	str	r5, [sp, #124]
    a1dc:	ebffff54 	bl	9f34 <nand_get_config>
    a1e0:	e1a04000 	mov	r4, r0
    a1e4:	e3a00001 	mov	r0, #1	; 0x1
    a1e8:	ebffff51 	bl	9f34 <nand_get_config>
    a1ec:	e1540005 	cmp	r4, r5
    a1f0:	11500005 	cmpne	r0, r5
    a1f4:	0a00004f 	beq	a338 <bnd_vs_write+0x184>
    a1f8:	e59f3148 	ldr	r3, [pc, #328]	; a348 <bnd_vs_write+0x194>
    a1fc:	e1a00006 	mov	r0, r6
    a200:	e593200c 	ldr	r2, [r3, #12]
    a204:	e1a01007 	mov	r1, r7
    a208:	eb002a76 	bl	14be8 <__aeabi_lasr>
    a20c:	e5941014 	ldr	r1, [r4, #20]
    a210:	e58d0060 	str	r0, [sp, #96]
    a214:	e59d00b0 	ldr	r0, [sp, #176]
    a218:	eb00295b 	bl	1478c <__aeabi_uidiv>
    a21c:	e2842038 	add	r2, r4, #56	; 0x38
    a220:	e284304c 	add	r3, r4, #76	; 0x4c
    a224:	e284c05c 	add	ip, r4, #92	; 0x5c
    a228:	e284e06c 	add	lr, r4, #108	; 0x6c
    a22c:	e1a07008 	mov	r7, r8
    a230:	e58d0074 	str	r0, [sp, #116]
    a234:	e58d2064 	str	r2, [sp, #100]
    a238:	e58d3068 	str	r3, [sp, #104]
    a23c:	e58dc06c 	str	ip, [sp, #108]
    a240:	e58de070 	str	lr, [sp, #112]
    a244:	e284b07c 	add	fp, r4, #124	; 0x7c
    a248:	e284908c 	add	r9, r4, #140	; 0x8c
    a24c:	e1a06005 	mov	r6, r5
    a250:	e1a0a005 	mov	sl, r5
    a254:	e3a08001 	mov	r8, #1	; 0x1
    a258:	ea00002d 	b	a314 <bnd_vs_write+0x160>
    a25c:	e59f20e8 	ldr	r2, [pc, #232]	; a34c <bnd_vs_write+0x198>
    a260:	e594c014 	ldr	ip, [r4, #20]
    a264:	e58d207c 	str	r2, [sp, #124]
    a268:	e5d40005 	ldrb	r0, [r4, #5]
    a26c:	e5d42003 	ldrb	r2, [r4, #3]
    a270:	e58de00c 	str	lr, [sp, #12]
    a274:	e58dc014 	str	ip, [sp, #20]
    a278:	e58d7010 	str	r7, [sp, #16]
    a27c:	e58dc000 	str	ip, [sp]
    a280:	e58da004 	str	sl, [sp, #4]
    a284:	e58d8008 	str	r8, [sp, #8]
    a288:	e58da018 	str	sl, [sp, #24]
    a28c:	e58da01c 	str	sl, [sp, #28]
    a290:	e58d8020 	str	r8, [sp, #32]
    a294:	e58d8024 	str	r8, [sp, #36]
    a298:	e5d4c002 	ldrb	ip, [r4, #2]
    a29c:	e59de064 	ldr	lr, [sp, #100]
    a2a0:	e58dc028 	str	ip, [sp, #40]
    a2a4:	e5d4c001 	ldrb	ip, [r4, #1]
    a2a8:	e58de030 	str	lr, [sp, #48]
    a2ac:	e58dc02c 	str	ip, [sp, #44]
    a2b0:	e59dc068 	ldr	ip, [sp, #104]
    a2b4:	e59de06c 	ldr	lr, [sp, #108]
    a2b8:	e58dc034 	str	ip, [sp, #52]
    a2bc:	e59dc070 	ldr	ip, [sp, #112]
    a2c0:	e58de038 	str	lr, [sp, #56]
    a2c4:	e58dc03c 	str	ip, [sp, #60]
    a2c8:	e28de084 	add	lr, sp, #132	; 0x84
    a2cc:	e28dc080 	add	ip, sp, #128	; 0x80
    a2d0:	e58de048 	str	lr, [sp, #72]
    a2d4:	e58dc04c 	str	ip, [sp, #76]
    a2d8:	e28de07c 	add	lr, sp, #124	; 0x7c
    a2dc:	e3a0c004 	mov	ip, #4	; 0x4
    a2e0:	e58de050 	str	lr, [sp, #80]
    a2e4:	e58db040 	str	fp, [sp, #64]
    a2e8:	e58d9044 	str	r9, [sp, #68]
    a2ec:	e58dc054 	str	ip, [sp, #84]
    a2f0:	e58d8058 	str	r8, [sp, #88]
    a2f4:	e5d4c006 	ldrb	ip, [r4, #6]
    a2f8:	e58dc05c 	str	ip, [sp, #92]
    a2fc:	eb0009df 	bl	ca80 <nf2_page_op>
    a300:	e3500000 	cmp	r0, #0	; 0x0
    a304:	1a00000b 	bne	a338 <bnd_vs_write+0x184>
    a308:	e5943014 	ldr	r3, [r4, #20]
    a30c:	e0866003 	add	r6, r6, r3
    a310:	e0877003 	add	r7, r7, r3
    a314:	e59de074 	ldr	lr, [sp, #116]
    a318:	e59d2060 	ldr	r2, [sp, #96]
    a31c:	e155000e 	cmp	r5, lr
    a320:	e1a0100a 	mov	r1, sl
    a324:	e085e002 	add	lr, r5, r2
    a328:	e1a03008 	mov	r3, r8
    a32c:	e2855001 	add	r5, r5, #1	; 0x1
    a330:	baffffc9 	blt	a25c <bnd_vs_write+0xa8>
    a334:	ea000000 	b	a33c <bnd_vs_write+0x188>
    a338:	e3e06000 	mvn	r6, #0	; 0x0
    a33c:	e1a00006 	mov	r0, r6
    a340:	e28dd08c 	add	sp, sp, #140	; 0x8c
    a344:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a348:	3c00d734 	.word	0x3c00d734
    a34c:	031800ff 	.word	0x031800ff

0000a350 <bnd_vs_read>:
    a350:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a354:	e3a05000 	mov	r5, #0	; 0x0
    a358:	e24dd08c 	sub	sp, sp, #140	; 0x8c
    a35c:	e1a08000 	mov	r8, r0
    a360:	e1a00005 	mov	r0, r5
    a364:	e1a06002 	mov	r6, r2
    a368:	e1a07003 	mov	r7, r3
    a36c:	e58d5084 	str	r5, [sp, #132]
    a370:	e58d5080 	str	r5, [sp, #128]
    a374:	e58d507c 	str	r5, [sp, #124]
    a378:	ebfffeed 	bl	9f34 <nand_get_config>
    a37c:	e2504000 	subs	r4, r0, #0	; 0x0
    a380:	0a000056 	beq	a4e0 <bnd_vs_read+0x190>
    a384:	e59f3164 	ldr	r3, [pc, #356]	; a4f0 <bnd_vs_read+0x1a0>
    a388:	e1a00006 	mov	r0, r6
    a38c:	e593200c 	ldr	r2, [r3, #12]
    a390:	e1a01007 	mov	r1, r7
    a394:	eb002a13 	bl	14be8 <__aeabi_lasr>
    a398:	e5941014 	ldr	r1, [r4, #20]
    a39c:	e58d0060 	str	r0, [sp, #96]
    a3a0:	e59d00b0 	ldr	r0, [sp, #176]
    a3a4:	eb0028f8 	bl	1478c <__aeabi_uidiv>
    a3a8:	e2842038 	add	r2, r4, #56	; 0x38
    a3ac:	e284304c 	add	r3, r4, #76	; 0x4c
    a3b0:	e284c05c 	add	ip, r4, #92	; 0x5c
    a3b4:	e284e06c 	add	lr, r4, #108	; 0x6c
    a3b8:	e1a07008 	mov	r7, r8
    a3bc:	e58d0074 	str	r0, [sp, #116]
    a3c0:	e58d2064 	str	r2, [sp, #100]
    a3c4:	e58d3068 	str	r3, [sp, #104]
    a3c8:	e58dc06c 	str	ip, [sp, #108]
    a3cc:	e58de070 	str	lr, [sp, #112]
    a3d0:	e284b07c 	add	fp, r4, #124	; 0x7c
    a3d4:	e284908c 	add	r9, r4, #140	; 0x8c
    a3d8:	e1a06005 	mov	r6, r5
    a3dc:	e1a08005 	mov	r8, r5
    a3e0:	e3a0a001 	mov	sl, #1	; 0x1
    a3e4:	ea000034 	b	a4bc <bnd_vs_read+0x16c>
    a3e8:	e594c014 	ldr	ip, [r4, #20]
    a3ec:	e5d40005 	ldrb	r0, [r4, #5]
    a3f0:	e5d42003 	ldrb	r2, [r4, #3]
    a3f4:	e58de00c 	str	lr, [sp, #12]
    a3f8:	e58dc014 	str	ip, [sp, #20]
    a3fc:	e58d7010 	str	r7, [sp, #16]
    a400:	e58dc000 	str	ip, [sp]
    a404:	e58d8004 	str	r8, [sp, #4]
    a408:	e58d8008 	str	r8, [sp, #8]
    a40c:	e58d8018 	str	r8, [sp, #24]
    a410:	e58d801c 	str	r8, [sp, #28]
    a414:	e58da020 	str	sl, [sp, #32]
    a418:	e58da024 	str	sl, [sp, #36]
    a41c:	e5d4c002 	ldrb	ip, [r4, #2]
    a420:	e59de064 	ldr	lr, [sp, #100]
    a424:	e58dc028 	str	ip, [sp, #40]
    a428:	e5d4c001 	ldrb	ip, [r4, #1]
    a42c:	e58de030 	str	lr, [sp, #48]
    a430:	e58dc02c 	str	ip, [sp, #44]
    a434:	e59dc068 	ldr	ip, [sp, #104]
    a438:	e59de06c 	ldr	lr, [sp, #108]
    a43c:	e58dc034 	str	ip, [sp, #52]
    a440:	e59dc070 	ldr	ip, [sp, #112]
    a444:	e58de038 	str	lr, [sp, #56]
    a448:	e58dc03c 	str	ip, [sp, #60]
    a44c:	e28de084 	add	lr, sp, #132	; 0x84
    a450:	e28dc080 	add	ip, sp, #128	; 0x80
    a454:	e58de048 	str	lr, [sp, #72]
    a458:	e58dc04c 	str	ip, [sp, #76]
    a45c:	e28de07c 	add	lr, sp, #124	; 0x7c
    a460:	e3a0c004 	mov	ip, #4	; 0x4
    a464:	e58de050 	str	lr, [sp, #80]
    a468:	e58db040 	str	fp, [sp, #64]
    a46c:	e58d9044 	str	r9, [sp, #68]
    a470:	e58dc054 	str	ip, [sp, #84]
    a474:	e58da058 	str	sl, [sp, #88]
    a478:	e5d4c006 	ldrb	ip, [r4, #6]
    a47c:	e58dc05c 	str	ip, [sp, #92]
    a480:	eb00097e 	bl	ca80 <nf2_page_op>
    a484:	e3500000 	cmp	r0, #0	; 0x0
    a488:	1a000014 	bne	a4e0 <bnd_vs_read+0x190>
    a48c:	e59d3084 	ldr	r3, [sp, #132]
    a490:	e1dd27be 	ldrh	r2, [sp, #126]
    a494:	e3530000 	cmp	r3, #0	; 0x0
    a498:	1a000006 	bne	a4b8 <bnd_vs_read+0x168>
    a49c:	e59d3080 	ldr	r3, [sp, #128]
    a4a0:	e3530000 	cmp	r3, #0	; 0x0
    a4a4:	1a000003 	bne	a4b8 <bnd_vs_read+0x168>
    a4a8:	e3520fc6 	cmp	r2, #792	; 0x318
    a4ac:	05943014 	ldreq	r3, [r4, #20]
    a4b0:	00877003 	addeq	r7, r7, r3
    a4b4:	00866003 	addeq	r6, r6, r3
    a4b8:	e58d207c 	str	r2, [sp, #124]
    a4bc:	e59de074 	ldr	lr, [sp, #116]
    a4c0:	e59d2060 	ldr	r2, [sp, #96]
    a4c4:	e155000e 	cmp	r5, lr
    a4c8:	e1a01008 	mov	r1, r8
    a4cc:	e085e002 	add	lr, r5, r2
    a4d0:	e1a0300a 	mov	r3, sl
    a4d4:	e2855001 	add	r5, r5, #1	; 0x1
    a4d8:	baffffc2 	blt	a3e8 <bnd_vs_read+0x98>
    a4dc:	ea000000 	b	a4e4 <bnd_vs_read+0x194>
    a4e0:	e3e06000 	mvn	r6, #0	; 0x0
    a4e4:	e1a00006 	mov	r0, r6
    a4e8:	e28dd08c 	add	sp, sp, #140	; 0x8c
    a4ec:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a4f0:	3c00d734 	.word	0x3c00d734

0000a4f4 <base_vs_write>:
    a4f4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a4f8:	e3a01000 	mov	r1, #0	; 0x0
    a4fc:	e24dd094 	sub	sp, sp, #148	; 0x94
    a500:	e1a0a000 	mov	sl, r0
    a504:	e3a00001 	mov	r0, #1	; 0x1
    a508:	e58d1088 	str	r1, [sp, #136]
    a50c:	e1a08002 	mov	r8, r2
    a510:	e1a09003 	mov	r9, r3
    a514:	e58d108c 	str	r1, [sp, #140]
    a518:	e59db0bc 	ldr	fp, [sp, #188]
    a51c:	ebfffe84 	bl	9f34 <nand_get_config>
    a520:	e1a04000 	mov	r4, r0
    a524:	e5900010 	ldr	r0, [r0, #16]
    a528:	e2800004 	add	r0, r0, #4	; 0x4
    a52c:	ebffdc6d 	bl	16e8 <malloc>
    a530:	e2507000 	subs	r7, r0, #0	; 0x0
    a534:	1a000007 	bne	a558 <base_vs_write+0x64>
    a538:	e5941010 	ldr	r1, [r4, #16]
    a53c:	e59f0240 	ldr	r0, [pc, #576]	; a784 <base_vs_write+0x290>
    a540:	e2811004 	add	r1, r1, #4	; 0x4
    a544:	e59f323c 	ldr	r3, [pc, #572]	; a788 <base_vs_write+0x294>
    a548:	e1a0e00f 	mov	lr, pc
    a54c:	e593f000 	ldr	pc, [r3]
    a550:	e3e05000 	mvn	r5, #0	; 0x0
    a554:	ea000087 	b	a778 <base_vs_write+0x284>
    a558:	e35b0000 	cmp	fp, #0	; 0x0
    a55c:	01a0200b 	moveq	r2, fp
    a560:	03e01000 	mvneq	r1, #0	; 0x0
    a564:	0a000002 	beq	a574 <base_vs_write+0x80>
    a568:	ea000007 	b	a58c <base_vs_write+0x98>
    a56c:	e7c71002 	strb	r1, [r7, r2]
    a570:	e2822001 	add	r2, r2, #1	; 0x1
    a574:	e5943010 	ldr	r3, [r4, #16]
    a578:	e2833004 	add	r3, r3, #4	; 0x4
    a57c:	e1520003 	cmp	r2, r3
    a580:	3afffff9 	bcc	a56c <base_vs_write+0x78>
    a584:	e3a03000 	mov	r3, #0	; 0x0
    a588:	e5c73001 	strb	r3, [r7, #1]
    a58c:	e59f31f8 	ldr	r3, [pc, #504]	; a78c <base_vs_write+0x298>
    a590:	e35b0000 	cmp	fp, #0	; 0x0
    a594:	e5932010 	ldr	r2, [r3, #16]
    a598:	e1a00008 	mov	r0, r8
    a59c:	e1a01009 	mov	r1, r9
    a5a0:	01a0500b 	moveq	r5, fp
    a5a4:	15945010 	ldrne	r5, [r4, #16]
    a5a8:	eb00298e 	bl	14be8 <__aeabi_lasr>
    a5ac:	e5946014 	ldr	r6, [r4, #20]
    a5b0:	e58d0060 	str	r0, [sp, #96]
    a5b4:	e0851006 	add	r1, r5, r6
    a5b8:	e59d00b8 	ldr	r0, [sp, #184]
    a5bc:	eb002872 	bl	1478c <__aeabi_uidiv>
    a5c0:	e58d007c 	str	r0, [sp, #124]
    a5c4:	e5d43005 	ldrb	r3, [r4, #5]
    a5c8:	e3a06000 	mov	r6, #0	; 0x0
    a5cc:	e58d3080 	str	r3, [sp, #128]
    a5d0:	e5d43004 	ldrb	r3, [r4, #4]
    a5d4:	e1a0800a 	mov	r8, sl
    a5d8:	e58d3084 	str	r3, [sp, #132]
    a5dc:	e2843038 	add	r3, r4, #56	; 0x38
    a5e0:	e58d3064 	str	r3, [sp, #100]
    a5e4:	e284304c 	add	r3, r4, #76	; 0x4c
    a5e8:	e58d3068 	str	r3, [sp, #104]
    a5ec:	e284305c 	add	r3, r4, #92	; 0x5c
    a5f0:	e58d306c 	str	r3, [sp, #108]
    a5f4:	e284306c 	add	r3, r4, #108	; 0x6c
    a5f8:	e58d3070 	str	r3, [sp, #112]
    a5fc:	e284307c 	add	r3, r4, #124	; 0x7c
    a600:	e58d3074 	str	r3, [sp, #116]
    a604:	e284308c 	add	r3, r4, #140	; 0x8c
    a608:	e58d3078 	str	r3, [sp, #120]
    a60c:	e1a05006 	mov	r5, r6
    a610:	e1a0a006 	mov	sl, r6
    a614:	e3a09001 	mov	r9, #1	; 0x1
    a618:	ea00004f 	b	a75c <base_vs_write+0x268>
    a61c:	e35b0001 	cmp	fp, #1	; 0x1
    a620:	1a000012 	bne	a670 <base_vs_write+0x17c>
    a624:	e3e03000 	mvn	r3, #0	; 0x0
    a628:	e5c73000 	strb	r3, [r7]
    a62c:	e3a03000 	mov	r3, #0	; 0x0
    a630:	e5c73001 	strb	r3, [r7, #1]
    a634:	e3e03000 	mvn	r3, #0	; 0x0
    a638:	e5c73002 	strb	r3, [r7, #2]
    a63c:	e5c73003 	strb	r3, [r7, #3]
    a640:	e1a01007 	mov	r1, r7
    a644:	e3a02000 	mov	r2, #0	; 0x0
    a648:	ea000004 	b	a660 <base_vs_write+0x16c>
    a64c:	e5943014 	ldr	r3, [r4, #20]
    a650:	e0883003 	add	r3, r8, r3
    a654:	e7d33002 	ldrb	r3, [r3, r2]
    a658:	e2822001 	add	r2, r2, #1	; 0x1
    a65c:	e5c13003 	strb	r3, [r1, #3]
    a660:	e5943010 	ldr	r3, [r4, #16]
    a664:	e2811001 	add	r1, r1, #1	; 0x1
    a668:	e1520003 	cmp	r2, r3
    a66c:	3afffff6 	bcc	a64c <base_vs_write+0x158>
    a670:	e5943014 	ldr	r3, [r4, #20]
    a674:	e5941010 	ldr	r1, [r4, #16]
    a678:	e5d40005 	ldrb	r0, [r4, #5]
    a67c:	e5d42003 	ldrb	r2, [r4, #3]
    a680:	e58d3014 	str	r3, [sp, #20]
    a684:	e58d3000 	str	r3, [sp]
    a688:	e59d3080 	ldr	r3, [sp, #128]
    a68c:	e58dc00c 	str	ip, [sp, #12]
    a690:	e58d3018 	str	r3, [sp, #24]
    a694:	e59d3084 	ldr	r3, [sp, #132]
    a698:	e58d1004 	str	r1, [sp, #4]
    a69c:	e58d301c 	str	r3, [sp, #28]
    a6a0:	e3a03000 	mov	r3, #0	; 0x0
    a6a4:	e58d9008 	str	r9, [sp, #8]
    a6a8:	e58d8010 	str	r8, [sp, #16]
    a6ac:	e58d3020 	str	r3, [sp, #32]
    a6b0:	e58d9024 	str	r9, [sp, #36]
    a6b4:	e5d43002 	ldrb	r3, [r4, #2]
    a6b8:	e2811004 	add	r1, r1, #4	; 0x4
    a6bc:	e58d3028 	str	r3, [sp, #40]
    a6c0:	e5d43001 	ldrb	r3, [r4, #1]
    a6c4:	e58d1054 	str	r1, [sp, #84]
    a6c8:	e58d302c 	str	r3, [sp, #44]
    a6cc:	e59d3064 	ldr	r3, [sp, #100]
    a6d0:	e3a01000 	mov	r1, #0	; 0x0
    a6d4:	e58d3030 	str	r3, [sp, #48]
    a6d8:	e59d3068 	ldr	r3, [sp, #104]
    a6dc:	e58d3034 	str	r3, [sp, #52]
    a6e0:	e59d306c 	ldr	r3, [sp, #108]
    a6e4:	e58d3038 	str	r3, [sp, #56]
    a6e8:	e59d3070 	ldr	r3, [sp, #112]
    a6ec:	e58d303c 	str	r3, [sp, #60]
    a6f0:	e59d3074 	ldr	r3, [sp, #116]
    a6f4:	e58d3040 	str	r3, [sp, #64]
    a6f8:	e59d3078 	ldr	r3, [sp, #120]
    a6fc:	e58d3044 	str	r3, [sp, #68]
    a700:	e28d308c 	add	r3, sp, #140	; 0x8c
    a704:	e58d3048 	str	r3, [sp, #72]
    a708:	e28d3088 	add	r3, sp, #136	; 0x88
    a70c:	e58d304c 	str	r3, [sp, #76]
    a710:	e58d7050 	str	r7, [sp, #80]
    a714:	e58d9058 	str	r9, [sp, #88]
    a718:	e5d4c006 	ldrb	ip, [r4, #6]
    a71c:	e1a03009 	mov	r3, r9
    a720:	e58dc05c 	str	ip, [sp, #92]
    a724:	eb0008d5 	bl	ca80 <nf2_page_op>
    a728:	e3500000 	cmp	r0, #0	; 0x0
    a72c:	13e05000 	mvnne	r5, #0	; 0x0
    a730:	1a00000e 	bne	a770 <base_vs_write+0x27c>
    a734:	e35b0000 	cmp	fp, #0	; 0x0
    a738:	e5942014 	ldr	r2, [r4, #20]
    a73c:	15943010 	ldrne	r3, [r4, #16]
    a740:	008aa002 	addeq	sl, sl, r2
    a744:	10833002 	addne	r3, r3, r2
    a748:	108aa003 	addne	sl, sl, r3
    a74c:	00888002 	addeq	r8, r8, r2
    a750:	10888003 	addne	r8, r8, r3
    a754:	e2866001 	add	r6, r6, #1	; 0x1
    a758:	e1a0500a 	mov	r5, sl
    a75c:	e59d307c 	ldr	r3, [sp, #124]
    a760:	e1560003 	cmp	r6, r3
    a764:	e59d3060 	ldr	r3, [sp, #96]
    a768:	e086c003 	add	ip, r6, r3
    a76c:	baffffaa 	blt	a61c <base_vs_write+0x128>
    a770:	e1a00007 	mov	r0, r7
    a774:	ebffdc43 	bl	1888 <free>
    a778:	e1a00005 	mov	r0, r5
    a77c:	e28dd094 	add	sp, sp, #148	; 0x94
    a780:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a784:	000181fb 	.word	0x000181fb
    a788:	3c00d6cc 	.word	0x3c00d6cc
    a78c:	3c00d734 	.word	0x3c00d734

0000a790 <fnd_vs_write>:
    a790:	e3a01001 	mov	r1, #1	; 0x1
    a794:	e58d1004 	str	r1, [sp, #4]
    a798:	eaffff55 	b	a4f4 <base_vs_write>

0000a79c <nnd_vs_write>:
    a79c:	e3a01000 	mov	r1, #0	; 0x0
    a7a0:	e58d1004 	str	r1, [sp, #4]
    a7a4:	eaffff52 	b	a4f4 <base_vs_write>

0000a7a8 <base_vs_read>:
    a7a8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a7ac:	e3a01000 	mov	r1, #0	; 0x0
    a7b0:	e24dd094 	sub	sp, sp, #148	; 0x94
    a7b4:	e1a09000 	mov	r9, r0
    a7b8:	e3a00001 	mov	r0, #1	; 0x1
    a7bc:	e58d1088 	str	r1, [sp, #136]
    a7c0:	e1a06002 	mov	r6, r2
    a7c4:	e1a07003 	mov	r7, r3
    a7c8:	e58d108c 	str	r1, [sp, #140]
    a7cc:	e59db0bc 	ldr	fp, [sp, #188]
    a7d0:	ebfffdd7 	bl	9f34 <nand_get_config>
    a7d4:	e1a04000 	mov	r4, r0
    a7d8:	e5900010 	ldr	r0, [r0, #16]
    a7dc:	e2800004 	add	r0, r0, #4	; 0x4
    a7e0:	ebffdbc0 	bl	16e8 <malloc>
    a7e4:	e250a000 	subs	sl, r0, #0	; 0x0
    a7e8:	1a000007 	bne	a80c <base_vs_read+0x64>
    a7ec:	e5941010 	ldr	r1, [r4, #16]
    a7f0:	e59f0278 	ldr	r0, [pc, #632]	; aa70 <base_vs_read+0x2c8>
    a7f4:	e2811004 	add	r1, r1, #4	; 0x4
    a7f8:	e59f3274 	ldr	r3, [pc, #628]	; aa74 <base_vs_read+0x2cc>
    a7fc:	e1a0e00f 	mov	lr, pc
    a800:	e593f000 	ldr	pc, [r3]
    a804:	e3e05000 	mvn	r5, #0	; 0x0
    a808:	ea000095 	b	aa64 <base_vs_read+0x2bc>
    a80c:	e59f3264 	ldr	r3, [pc, #612]	; aa78 <base_vs_read+0x2d0>
    a810:	e35b0000 	cmp	fp, #0	; 0x0
    a814:	e5932010 	ldr	r2, [r3, #16]
    a818:	e1a00006 	mov	r0, r6
    a81c:	e1a01007 	mov	r1, r7
    a820:	01a0500b 	moveq	r5, fp
    a824:	15945010 	ldrne	r5, [r4, #16]
    a828:	eb0028ee 	bl	14be8 <__aeabi_lasr>
    a82c:	e5948014 	ldr	r8, [r4, #20]
    a830:	e58d0060 	str	r0, [sp, #96]
    a834:	e0851008 	add	r1, r5, r8
    a838:	e59d00b8 	ldr	r0, [sp, #184]
    a83c:	eb0027d2 	bl	1478c <__aeabi_uidiv>
    a840:	e58d007c 	str	r0, [sp, #124]
    a844:	e5d43005 	ldrb	r3, [r4, #5]
    a848:	e3a07000 	mov	r7, #0	; 0x0
    a84c:	e58d3080 	str	r3, [sp, #128]
    a850:	e5d43004 	ldrb	r3, [r4, #4]
    a854:	e1a06009 	mov	r6, r9
    a858:	e58d3084 	str	r3, [sp, #132]
    a85c:	e2843038 	add	r3, r4, #56	; 0x38
    a860:	e58d3064 	str	r3, [sp, #100]
    a864:	e284304c 	add	r3, r4, #76	; 0x4c
    a868:	e58d3068 	str	r3, [sp, #104]
    a86c:	e284305c 	add	r3, r4, #92	; 0x5c
    a870:	e58d306c 	str	r3, [sp, #108]
    a874:	e284306c 	add	r3, r4, #108	; 0x6c
    a878:	e58d3070 	str	r3, [sp, #112]
    a87c:	e284307c 	add	r3, r4, #124	; 0x7c
    a880:	e58d3074 	str	r3, [sp, #116]
    a884:	e284308c 	add	r3, r4, #140	; 0x8c
    a888:	e58d3078 	str	r3, [sp, #120]
    a88c:	e1a05007 	mov	r5, r7
    a890:	e1a08007 	mov	r8, r7
    a894:	e3a09001 	mov	r9, #1	; 0x1
    a898:	ea00006a 	b	aa48 <base_vs_read+0x2a0>
    a89c:	e5943014 	ldr	r3, [r4, #20]
    a8a0:	e5941010 	ldr	r1, [r4, #16]
    a8a4:	e5d40005 	ldrb	r0, [r4, #5]
    a8a8:	e5d42003 	ldrb	r2, [r4, #3]
    a8ac:	e58d3014 	str	r3, [sp, #20]
    a8b0:	e58d3000 	str	r3, [sp]
    a8b4:	e3a03000 	mov	r3, #0	; 0x0
    a8b8:	e58d3008 	str	r3, [sp, #8]
    a8bc:	e59d3080 	ldr	r3, [sp, #128]
    a8c0:	e58dc00c 	str	ip, [sp, #12]
    a8c4:	e58d3018 	str	r3, [sp, #24]
    a8c8:	e59d3084 	ldr	r3, [sp, #132]
    a8cc:	e58d1004 	str	r1, [sp, #4]
    a8d0:	e58d301c 	str	r3, [sp, #28]
    a8d4:	e3a03000 	mov	r3, #0	; 0x0
    a8d8:	e58d6010 	str	r6, [sp, #16]
    a8dc:	e58d3020 	str	r3, [sp, #32]
    a8e0:	e58d9024 	str	r9, [sp, #36]
    a8e4:	e5d43002 	ldrb	r3, [r4, #2]
    a8e8:	e2811004 	add	r1, r1, #4	; 0x4
    a8ec:	e58d3028 	str	r3, [sp, #40]
    a8f0:	e5d43001 	ldrb	r3, [r4, #1]
    a8f4:	e58d1054 	str	r1, [sp, #84]
    a8f8:	e58d302c 	str	r3, [sp, #44]
    a8fc:	e59d3064 	ldr	r3, [sp, #100]
    a900:	e3a01000 	mov	r1, #0	; 0x0
    a904:	e58d3030 	str	r3, [sp, #48]
    a908:	e59d3068 	ldr	r3, [sp, #104]
    a90c:	e58d3034 	str	r3, [sp, #52]
    a910:	e59d306c 	ldr	r3, [sp, #108]
    a914:	e58d3038 	str	r3, [sp, #56]
    a918:	e59d3070 	ldr	r3, [sp, #112]
    a91c:	e58d303c 	str	r3, [sp, #60]
    a920:	e59d3074 	ldr	r3, [sp, #116]
    a924:	e58d3040 	str	r3, [sp, #64]
    a928:	e59d3078 	ldr	r3, [sp, #120]
    a92c:	e58d3044 	str	r3, [sp, #68]
    a930:	e28d308c 	add	r3, sp, #140	; 0x8c
    a934:	e58d3048 	str	r3, [sp, #72]
    a938:	e28d3088 	add	r3, sp, #136	; 0x88
    a93c:	e58d304c 	str	r3, [sp, #76]
    a940:	e58da050 	str	sl, [sp, #80]
    a944:	e58d9058 	str	r9, [sp, #88]
    a948:	e5d4c006 	ldrb	ip, [r4, #6]
    a94c:	e1a03009 	mov	r3, r9
    a950:	e58dc05c 	str	ip, [sp, #92]
    a954:	eb000849 	bl	ca80 <nf2_page_op>
    a958:	e3500000 	cmp	r0, #0	; 0x0
    a95c:	1a00001d 	bne	a9d8 <base_vs_read+0x230>
    a960:	e5da1001 	ldrb	r1, [sl, #1]
    a964:	e35100ff 	cmp	r1, #255	; 0xff
    a968:	1a00000b 	bne	a99c <base_vs_read+0x1f4>
    a96c:	e35b0000 	cmp	fp, #0	; 0x0
    a970:	1a000003 	bne	a984 <base_vs_read+0x1dc>
    a974:	e1a00006 	mov	r0, r6
    a978:	e5942014 	ldr	r2, [r4, #20]
    a97c:	ebffdaec 	bl	1534 <memset>
    a980:	ea000027 	b	aa24 <base_vs_read+0x27c>
    a984:	e5943010 	ldr	r3, [r4, #16]
    a988:	e5942014 	ldr	r2, [r4, #20]
    a98c:	e1a00006 	mov	r0, r6
    a990:	e0832002 	add	r2, r3, r2
    a994:	ebffdae6 	bl	1534 <memset>
    a998:	ea000010 	b	a9e0 <base_vs_read+0x238>
    a99c:	e59d108c 	ldr	r1, [sp, #140]
    a9a0:	e3510000 	cmp	r1, #0	; 0x0
    a9a4:	1a000002 	bne	a9b4 <base_vs_read+0x20c>
    a9a8:	e59d3088 	ldr	r3, [sp, #136]
    a9ac:	e3530000 	cmp	r3, #0	; 0x0
    a9b0:	0a00000a 	beq	a9e0 <base_vs_read+0x238>
    a9b4:	e59d2088 	ldr	r2, [sp, #136]
    a9b8:	e2511000 	subs	r1, r1, #0	; 0x0
    a9bc:	13a01001 	movne	r1, #1	; 0x1
    a9c0:	e2522000 	subs	r2, r2, #0	; 0x0
    a9c4:	13a02001 	movne	r2, #1	; 0x1
    a9c8:	e59f00ac 	ldr	r0, [pc, #172]	; aa7c <base_vs_read+0x2d4>
    a9cc:	e59f30a0 	ldr	r3, [pc, #160]	; aa74 <base_vs_read+0x2cc>
    a9d0:	e1a0e00f 	mov	lr, pc
    a9d4:	e593f000 	ldr	pc, [r3]
    a9d8:	e3e05000 	mvn	r5, #0	; 0x0
    a9dc:	ea00001e 	b	aa5c <base_vs_read+0x2b4>
    a9e0:	e35b0001 	cmp	fp, #1	; 0x1
    a9e4:	01a0000a 	moveq	r0, sl
    a9e8:	03a01000 	moveq	r1, #0	; 0x0
    a9ec:	1a00000a 	bne	aa1c <base_vs_read+0x274>
    a9f0:	ea000004 	b	aa08 <base_vs_read+0x260>
    a9f4:	e5943014 	ldr	r3, [r4, #20]
    a9f8:	e5d02003 	ldrb	r2, [r0, #3]
    a9fc:	e0863003 	add	r3, r6, r3
    aa00:	e7c32001 	strb	r2, [r3, r1]
    aa04:	e2811001 	add	r1, r1, #1	; 0x1
    aa08:	e5943010 	ldr	r3, [r4, #16]
    aa0c:	e2800001 	add	r0, r0, #1	; 0x1
    aa10:	e1510003 	cmp	r1, r3
    aa14:	3afffff6 	bcc	a9f4 <base_vs_read+0x24c>
    aa18:	ea000003 	b	aa2c <base_vs_read+0x284>
    aa1c:	e35b0000 	cmp	fp, #0	; 0x0
    aa20:	1a000001 	bne	aa2c <base_vs_read+0x284>
    aa24:	e5943014 	ldr	r3, [r4, #20]
    aa28:	ea000002 	b	aa38 <base_vs_read+0x290>
    aa2c:	e5943010 	ldr	r3, [r4, #16]
    aa30:	e5942014 	ldr	r2, [r4, #20]
    aa34:	e0833002 	add	r3, r3, r2
    aa38:	e0888003 	add	r8, r8, r3
    aa3c:	e0866003 	add	r6, r6, r3
    aa40:	e2877001 	add	r7, r7, #1	; 0x1
    aa44:	e1a05008 	mov	r5, r8
    aa48:	e59d307c 	ldr	r3, [sp, #124]
    aa4c:	e1570003 	cmp	r7, r3
    aa50:	e59d3060 	ldr	r3, [sp, #96]
    aa54:	e087c003 	add	ip, r7, r3
    aa58:	baffff8f 	blt	a89c <base_vs_read+0xf4>
    aa5c:	e1a0000a 	mov	r0, sl
    aa60:	ebffdb88 	bl	1888 <free>
    aa64:	e1a00005 	mov	r0, r5
    aa68:	e28dd094 	add	sp, sp, #148	; 0x94
    aa6c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    aa70:	00018219 	.word	0x00018219
    aa74:	3c00d6cc 	.word	0x3c00d6cc
    aa78:	3c00d734 	.word	0x3c00d734
    aa7c:	00018236 	.word	0x00018236

0000aa80 <fnd_vs_read>:
    aa80:	e3a01001 	mov	r1, #1	; 0x1
    aa84:	e58d1004 	str	r1, [sp, #4]
    aa88:	eaffff46 	b	a7a8 <base_vs_read>

0000aa8c <nnd_vs_read>:
    aa8c:	e3a01000 	mov	r1, #0	; 0x0
    aa90:	e58d1004 	str	r1, [sp, #4]
    aa94:	eaffff43 	b	a7a8 <base_vs_read>

0000aa98 <base_get_badmark>:
    aa98:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    aa9c:	e24dd07c 	sub	sp, sp, #124	; 0x7c
    aaa0:	e58d0064 	str	r0, [sp, #100]
    aaa4:	e3a05000 	mov	r5, #0	; 0x0
    aaa8:	e3a00001 	mov	r0, #1	; 0x1
    aaac:	e58d5074 	str	r5, [sp, #116]
    aab0:	e58d5070 	str	r5, [sp, #112]
    aab4:	ebfffd1e 	bl	9f34 <nand_get_config>
    aab8:	e2504000 	subs	r4, r0, #0	; 0x0
    aabc:	0a000008 	beq	aae4 <base_get_badmark+0x4c>
    aac0:	e5940010 	ldr	r0, [r4, #16]
    aac4:	e2800004 	add	r0, r0, #4	; 0x4
    aac8:	ebffdb06 	bl	16e8 <malloc>
    aacc:	e2506000 	subs	r6, r0, #0	; 0x0
    aad0:	1a000005 	bne	aaec <base_get_badmark+0x54>
    aad4:	e59f01c4 	ldr	r0, [pc, #452]	; aca0 <base_get_badmark+0x208>
    aad8:	e59f31c4 	ldr	r3, [pc, #452]	; aca4 <base_get_badmark+0x20c>
    aadc:	e1a0e00f 	mov	lr, pc
    aae0:	e593f000 	ldr	pc, [r3]
    aae4:	e3e05000 	mvn	r5, #0	; 0x0
    aae8:	ea000069 	b	ac94 <base_get_badmark+0x1fc>
    aaec:	e5941010 	ldr	r1, [r4, #16]
    aaf0:	e5d42003 	ldrb	r2, [r4, #3]
    aaf4:	e5943014 	ldr	r3, [r4, #20]
    aaf8:	e58d1014 	str	r1, [sp, #20]
    aafc:	e58d1004 	str	r1, [sp, #4]
    ab00:	e59d1064 	ldr	r1, [sp, #100]
    ab04:	e3a07001 	mov	r7, #1	; 0x1
    ab08:	e58d5008 	str	r5, [sp, #8]
    ab0c:	e58d100c 	str	r1, [sp, #12]
    ab10:	e58d5018 	str	r5, [sp, #24]
    ab14:	e58d5020 	str	r5, [sp, #32]
    ab18:	e58d5024 	str	r5, [sp, #36]
    ab1c:	e58d3000 	str	r3, [sp]
    ab20:	e58d6010 	str	r6, [sp, #16]
    ab24:	e58d701c 	str	r7, [sp, #28]
    ab28:	e5d43002 	ldrb	r3, [r4, #2]
    ab2c:	e2841038 	add	r1, r4, #56	; 0x38
    ab30:	e58d1068 	str	r1, [sp, #104]
    ab34:	e58d3028 	str	r3, [sp, #40]
    ab38:	e59d3068 	ldr	r3, [sp, #104]
    ab3c:	e284104c 	add	r1, r4, #76	; 0x4c
    ab40:	e58d106c 	str	r1, [sp, #108]
    ab44:	e58d3030 	str	r3, [sp, #48]
    ab48:	e58d1034 	str	r1, [sp, #52]
    ab4c:	e28d3070 	add	r3, sp, #112	; 0x70
    ab50:	e28d1074 	add	r1, sp, #116	; 0x74
    ab54:	e284b05c 	add	fp, r4, #92	; 0x5c
    ab58:	e284906c 	add	r9, r4, #108	; 0x6c
    ab5c:	e284a07c 	add	sl, r4, #124	; 0x7c
    ab60:	e284808c 	add	r8, r4, #140	; 0x8c
    ab64:	e58d502c 	str	r5, [sp, #44]
    ab68:	e58d1048 	str	r1, [sp, #72]
    ab6c:	e58d304c 	str	r3, [sp, #76]
    ab70:	e58d5050 	str	r5, [sp, #80]
    ab74:	e58d5054 	str	r5, [sp, #84]
    ab78:	e58d5058 	str	r5, [sp, #88]
    ab7c:	e58db038 	str	fp, [sp, #56]
    ab80:	e58d903c 	str	r9, [sp, #60]
    ab84:	e58da040 	str	sl, [sp, #64]
    ab88:	e58d8044 	str	r8, [sp, #68]
    ab8c:	e5d4c006 	ldrb	ip, [r4, #6]
    ab90:	e1a00005 	mov	r0, r5
    ab94:	e1a01007 	mov	r1, r7
    ab98:	e1a03007 	mov	r3, r7
    ab9c:	e58dc05c 	str	ip, [sp, #92]
    aba0:	eb0007b6 	bl	ca80 <nf2_page_op>
    aba4:	e2505000 	subs	r5, r0, #0	; 0x0
    aba8:	0a000004 	beq	abc0 <base_get_badmark+0x128>
    abac:	e59f00f4 	ldr	r0, [pc, #244]	; aca8 <base_get_badmark+0x210>
    abb0:	e59f30ec 	ldr	r3, [pc, #236]	; aca4 <base_get_badmark+0x20c>
    abb4:	e1a0e00f 	mov	lr, pc
    abb8:	e593f000 	ldr	pc, [r3]
    abbc:	ea000032 	b	ac8c <base_get_badmark+0x1f4>
    abc0:	e5d63000 	ldrb	r3, [r6]
    abc4:	e35300ff 	cmp	r3, #255	; 0xff
    abc8:	0a00002f 	beq	ac8c <base_get_badmark+0x1f4>
    abcc:	e5943014 	ldr	r3, [r4, #20]
    abd0:	e5d40005 	ldrb	r0, [r4, #5]
    abd4:	e5941010 	ldr	r1, [r4, #16]
    abd8:	e5d42003 	ldrb	r2, [r4, #3]
    abdc:	e58d3000 	str	r3, [sp]
    abe0:	e59d3064 	ldr	r3, [sp, #100]
    abe4:	e58d1014 	str	r1, [sp, #20]
    abe8:	e98d0022 	stmib	sp, {r1, r5}
    abec:	e58d0018 	str	r0, [sp, #24]
    abf0:	e58d300c 	str	r3, [sp, #12]
    abf4:	e58d6010 	str	r6, [sp, #16]
    abf8:	e5d43004 	ldrb	r3, [r4, #4]
    abfc:	e58d5020 	str	r5, [sp, #32]
    ac00:	e58d5024 	str	r5, [sp, #36]
    ac04:	e58d301c 	str	r3, [sp, #28]
    ac08:	e5d43002 	ldrb	r3, [r4, #2]
    ac0c:	e1a01007 	mov	r1, r7
    ac10:	e58d3028 	str	r3, [sp, #40]
    ac14:	e5d43001 	ldrb	r3, [r4, #1]
    ac18:	e58d5058 	str	r5, [sp, #88]
    ac1c:	e58d302c 	str	r3, [sp, #44]
    ac20:	e59d3068 	ldr	r3, [sp, #104]
    ac24:	e58d5050 	str	r5, [sp, #80]
    ac28:	e58d3030 	str	r3, [sp, #48]
    ac2c:	e59d306c 	ldr	r3, [sp, #108]
    ac30:	e58d5054 	str	r5, [sp, #84]
    ac34:	e58d3034 	str	r3, [sp, #52]
    ac38:	e28d3074 	add	r3, sp, #116	; 0x74
    ac3c:	e58d3048 	str	r3, [sp, #72]
    ac40:	e28d3070 	add	r3, sp, #112	; 0x70
    ac44:	e58d304c 	str	r3, [sp, #76]
    ac48:	e58db038 	str	fp, [sp, #56]
    ac4c:	e58d903c 	str	r9, [sp, #60]
    ac50:	e58da040 	str	sl, [sp, #64]
    ac54:	e58d8044 	str	r8, [sp, #68]
    ac58:	e5d4c006 	ldrb	ip, [r4, #6]
    ac5c:	e1a03007 	mov	r3, r7
    ac60:	e58dc05c 	str	ip, [sp, #92]
    ac64:	eb000785 	bl	ca80 <nf2_page_op>
    ac68:	e2505000 	subs	r5, r0, #0	; 0x0
    ac6c:	1a000006 	bne	ac8c <base_get_badmark+0x1f4>
    ac70:	e59d3070 	ldr	r3, [sp, #112]
    ac74:	e3530000 	cmp	r3, #0	; 0x0
    ac78:	1a000002 	bne	ac88 <base_get_badmark+0x1f0>
    ac7c:	e5d63000 	ldrb	r3, [r6]
    ac80:	e35300ff 	cmp	r3, #255	; 0xff
    ac84:	0a000000 	beq	ac8c <base_get_badmark+0x1f4>
    ac88:	e3a05001 	mov	r5, #1	; 0x1
    ac8c:	e1a00006 	mov	r0, r6
    ac90:	ebffdafc 	bl	1888 <free>
    ac94:	e1a00005 	mov	r0, r5
    ac98:	e28dd07c 	add	sp, sp, #124	; 0x7c
    ac9c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    aca0:	0001824a 	.word	0x0001824a
    aca4:	3c00d6cc 	.word	0x3c00d6cc
    aca8:	00018271 	.word	0x00018271

0000acac <nnd_vs_isbad>:
    acac:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    acb0:	e1a08000 	mov	r8, r0
    acb4:	e3a00001 	mov	r0, #1	; 0x1
    acb8:	e1a09001 	mov	r9, r1
    acbc:	ebfffc9c 	bl	9f34 <nand_get_config>
    acc0:	e2506000 	subs	r6, r0, #0	; 0x0
    acc4:	0a00001f 	beq	ad48 <nnd_vs_isbad+0x9c>
    acc8:	e596301c 	ldr	r3, [r6, #28]
    accc:	e1a01009 	mov	r1, r9
    acd0:	e2433001 	sub	r3, r3, #1	; 0x1
    acd4:	e1c84003 	bic	r4, r8, r3
    acd8:	e59f307c 	ldr	r3, [pc, #124]	; ad5c <nnd_vs_isbad+0xb0>
    acdc:	e1a00004 	mov	r0, r4
    ace0:	e5932010 	ldr	r2, [r3, #16]
    ace4:	eb0027bf 	bl	14be8 <__aeabi_lasr>
    ace8:	e1a08000 	mov	r8, r0
    acec:	e5960008 	ldr	r0, [r6, #8]
    acf0:	e1a05009 	mov	r5, r9
    acf4:	e0880000 	add	r0, r8, r0
    acf8:	ebffff66 	bl	aa98 <base_get_badmark>
    acfc:	e2502000 	subs	r2, r0, #0	; 0x0
    ad00:	ba00000a 	blt	ad30 <nnd_vs_isbad+0x84>
    ad04:	13a00001 	movne	r0, #1	; 0x1
    ad08:	18bd8370 	popne	{r4, r5, r6, r8, r9, pc}
    ad0c:	e596000c 	ldr	r0, [r6, #12]
    ad10:	e5963008 	ldr	r3, [r6, #8]
    ad14:	e1500003 	cmp	r0, r3
    ad18:	01a00002 	moveq	r0, r2
    ad1c:	08bd8370 	popeq	{r4, r5, r6, r8, r9, pc}
    ad20:	e0880000 	add	r0, r8, r0
    ad24:	ebffff5b 	bl	aa98 <base_get_badmark>
    ad28:	e3500000 	cmp	r0, #0	; 0x0
    ad2c:	aa000007 	bge	ad50 <nnd_vs_isbad+0xa4>
    ad30:	e1a02004 	mov	r2, r4
    ad34:	e1a03005 	mov	r3, r5
    ad38:	e59f0020 	ldr	r0, [pc, #32]	; ad60 <nnd_vs_isbad+0xb4>
    ad3c:	e59f1020 	ldr	r1, [pc, #32]	; ad64 <nnd_vs_isbad+0xb8>
    ad40:	e1a0e00f 	mov	lr, pc
    ad44:	e591f000 	ldr	pc, [r1]
    ad48:	e3e00000 	mvn	r0, #0	; 0x0
    ad4c:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    ad50:	03a00000 	moveq	r0, #0	; 0x0
    ad54:	13a00001 	movne	r0, #1	; 0x1
    ad58:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    ad5c:	3c00d734 	.word	0x3c00d734
    ad60:	0001828a 	.word	0x0001828a
    ad64:	3c00d6cc 	.word	0x3c00d6cc

0000ad68 <base_scan_badblocks>:
    ad68:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ad6c:	e1a0b000 	mov	fp, r0
    ad70:	e24dd004 	sub	sp, sp, #4	; 0x4
    ad74:	e3a00001 	mov	r0, #1	; 0x1
    ad78:	ebfffc6d 	bl	9f34 <nand_get_config>
    ad7c:	e250a000 	subs	sl, r0, #0	; 0x0
    ad80:	0a000005 	beq	ad9c <base_scan_badblocks+0x34>
    ad84:	e3a07000 	mov	r7, #0	; 0x0
    ad88:	e1a08007 	mov	r8, r7
    ad8c:	e3a04000 	mov	r4, #0	; 0x0
    ad90:	e3a05000 	mov	r5, #0	; 0x0
    ad94:	e59f90b8 	ldr	r9, [pc, #184]	; ae54 <base_scan_badblocks+0xec>
    ad98:	ea00001a 	b	ae08 <base_scan_badblocks+0xa0>
    ad9c:	e59f00b4 	ldr	r0, [pc, #180]	; ae58 <base_scan_badblocks+0xf0>
    ada0:	e59f30ac 	ldr	r3, [pc, #172]	; ae54 <base_scan_badblocks+0xec>
    ada4:	e1a0e00f 	mov	lr, pc
    ada8:	e593f000 	ldr	pc, [r3]
    adac:	e3e00000 	mvn	r0, #0	; 0x0
    adb0:	ea000025 	b	ae4c <base_scan_badblocks+0xe4>
    adb4:	ebffffbc 	bl	acac <nnd_vs_isbad>
    adb8:	e3500000 	cmp	r0, #0	; 0x0
    adbc:	0a00000d 	beq	adf8 <base_scan_badblocks+0x90>
    adc0:	aa000004 	bge	add8 <base_scan_badblocks+0x70>
    adc4:	e59f0090 	ldr	r0, [pc, #144]	; ae5c <base_scan_badblocks+0xf4>
    adc8:	e1a01007 	mov	r1, r7
    adcc:	e1a0e00f 	mov	lr, pc
    add0:	e599f000 	ldr	pc, [r9]
    add4:	ea000006 	b	adf4 <base_scan_badblocks+0x8c>
    add8:	e35b0000 	cmp	fp, #0	; 0x0
    addc:	0a000004 	beq	adf4 <base_scan_badblocks+0x8c>
    ade0:	e1a02004 	mov	r2, r4
    ade4:	e1a03005 	mov	r3, r5
    ade8:	e59f0070 	ldr	r0, [pc, #112]	; ae60 <base_scan_badblocks+0xf8>
    adec:	e1a0e00f 	mov	lr, pc
    adf0:	e599f000 	ldr	pc, [r9]
    adf4:	e2888001 	add	r8, r8, #1	; 0x1
    adf8:	e59a301c 	ldr	r3, [sl, #28]
    adfc:	e2877001 	add	r7, r7, #1	; 0x1
    ae00:	e0944003 	adds	r4, r4, r3
    ae04:	e2a55000 	adc	r5, r5, #0	; 0x0
    ae08:	e59a6024 	ldr	r6, [sl, #36]
    ae0c:	e1a00004 	mov	r0, r4
    ae10:	e1570006 	cmp	r7, r6
    ae14:	e1a01005 	mov	r1, r5
    ae18:	3affffe5 	bcc	adb4 <base_scan_badblocks+0x4c>
    ae1c:	e3a00064 	mov	r0, #100	; 0x64
    ae20:	e1a01006 	mov	r1, r6
    ae24:	e0000890 	mul	r0, r0, r8
    ae28:	eb002657 	bl	1478c <__aeabi_uidiv>
    ae2c:	e1a01008 	mov	r1, r8
    ae30:	e1a03000 	mov	r3, r0
    ae34:	e1a02006 	mov	r2, r6
    ae38:	e59f0024 	ldr	r0, [pc, #36]	; ae64 <base_scan_badblocks+0xfc>
    ae3c:	e59fc010 	ldr	ip, [pc, #16]	; ae54 <base_scan_badblocks+0xec>
    ae40:	e1a0e00f 	mov	lr, pc
    ae44:	e59cf000 	ldr	pc, [ip]
    ae48:	e3a00000 	mov	r0, #0	; 0x0
    ae4c:	e28dd004 	add	sp, sp, #4	; 0x4
    ae50:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ae54:	3c00d6cc 	.word	0x3c00d6cc
    ae58:	000182a8 	.word	0x000182a8
    ae5c:	000182c4 	.word	0x000182c4
    ae60:	000167af 	.word	0x000167af
    ae64:	000182de 	.word	0x000182de

0000ae68 <fnd_vs_isbad>:
    ae68:	eaffff8f 	b	acac <nnd_vs_isbad>

0000ae6c <bnd_vs_isbad>:
    ae6c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ae70:	e59f3014 	ldr	r3, [pc, #20]	; ae8c <bnd_vs_isbad+0x20>
    ae74:	e24dd004 	sub	sp, sp, #4	; 0x4
    ae78:	e5932008 	ldr	r2, [r3, #8]
    ae7c:	eb002760 	bl	14c04 <__aeabi_llsl>
    ae80:	e28dd004 	add	sp, sp, #4	; 0x4
    ae84:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    ae88:	eaffff87 	b	acac <nnd_vs_isbad>
    ae8c:	3c00d734 	.word	0x3c00d734

0000ae90 <nnd_vs_reset>:
    ae90:	e92d4030 	push	{r4, r5, lr}
    ae94:	e3a00001 	mov	r0, #1	; 0x1
    ae98:	e24dd004 	sub	sp, sp, #4	; 0x4
    ae9c:	ebfffc24 	bl	9f34 <nand_get_config>
    aea0:	e2504000 	subs	r4, r0, #0	; 0x0
    aea4:	0a000014 	beq	aefc <nnd_vs_reset+0x6c>
    aea8:	e3a01001 	mov	r1, #1	; 0x1
    aeac:	eb000a2b 	bl	d760 <nf2_hardware_reset>
    aeb0:	e3500000 	cmp	r0, #0	; 0x0
    aeb4:	0a000011 	beq	af00 <nnd_vs_reset+0x70>
    aeb8:	e5d41000 	ldrb	r1, [r4]
    aebc:	e59f5044 	ldr	r5, [pc, #68]	; af08 <nnd_vs_reset+0x78>
    aec0:	e3510001 	cmp	r1, #1	; 0x1
    aec4:	1a000009 	bne	aef0 <nnd_vs_reset+0x60>
    aec8:	e59f003c 	ldr	r0, [pc, #60]	; af0c <nnd_vs_reset+0x7c>
    aecc:	e1a0e00f 	mov	lr, pc
    aed0:	e595f000 	ldr	pc, [r5]
    aed4:	e3a01000 	mov	r1, #0	; 0x0
    aed8:	e5c41000 	strb	r1, [r4]
    aedc:	e1a00004 	mov	r0, r4
    aee0:	eb000a1e 	bl	d760 <nf2_hardware_reset>
    aee4:	e3500000 	cmp	r0, #0	; 0x0
    aee8:	0a000004 	beq	af00 <nnd_vs_reset+0x70>
    aeec:	e5d41000 	ldrb	r1, [r4]
    aef0:	e59f0018 	ldr	r0, [pc, #24]	; af10 <nnd_vs_reset+0x80>
    aef4:	e1a0e00f 	mov	lr, pc
    aef8:	e595f000 	ldr	pc, [r5]
    aefc:	e3e00003 	mvn	r0, #3	; 0x3
    af00:	e28dd004 	add	sp, sp, #4	; 0x4
    af04:	e8bd8030 	pop	{r4, r5, pc}
    af08:	3c00d6cc 	.word	0x3c00d6cc
    af0c:	0001831f 	.word	0x0001831f
    af10:	000182fd 	.word	0x000182fd

0000af14 <fnd_vs_reset>:
    af14:	eaffffdd 	b	ae90 <nnd_vs_reset>

0000af18 <base_vs_erase>:
    af18:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    af1c:	e1a04000 	mov	r4, r0
    af20:	e24dd00c 	sub	sp, sp, #12	; 0xc
    af24:	e3a00001 	mov	r0, #1	; 0x1
    af28:	e1a05001 	mov	r5, r1
    af2c:	e1a06002 	mov	r6, r2
    af30:	e58d3004 	str	r3, [sp, #4]
    af34:	ebfffbfe 	bl	9f34 <nand_get_config>
    af38:	e2507000 	subs	r7, r0, #0	; 0x0
    af3c:	0a000047 	beq	b060 <base_vs_erase+0x148>
    af40:	e597201c 	ldr	r2, [r7, #28]
    af44:	e242c001 	sub	ip, r2, #1	; 0x1
    af48:	e00c3004 	and	r3, ip, r4
    af4c:	e3530000 	cmp	r3, #0	; 0x0
    af50:	e0836006 	add	r6, r3, r6
    af54:	1a000001 	bne	af60 <base_vs_erase+0x48>
    af58:	e116000c 	tst	r6, ip
    af5c:	0a00000a 	beq	af8c <base_vs_erase+0x74>
    af60:	e08c3006 	add	r3, ip, r6
    af64:	e2622000 	rsb	r2, r2, #0	; 0x0
    af68:	e0036002 	and	r6, r3, r2
    af6c:	e1c4400c 	bic	r4, r4, ip
    af70:	e59f0108 	ldr	r0, [pc, #264]	; b080 <base_vs_erase+0x168>
    af74:	e1a01006 	mov	r1, r6
    af78:	e1a02004 	mov	r2, r4
    af7c:	e1a03005 	mov	r3, r5
    af80:	e59fc0fc 	ldr	ip, [pc, #252]	; b084 <base_vs_erase+0x16c>
    af84:	e1a0e00f 	mov	lr, pc
    af88:	e59cf000 	ldr	pc, [ip]
    af8c:	e59f30f4 	ldr	r3, [pc, #244]	; b088 <base_vs_erase+0x170>
    af90:	e1a00004 	mov	r0, r4
    af94:	e5932010 	ldr	r2, [r3, #16]
    af98:	e1a01005 	mov	r1, r5
    af9c:	eb002711 	bl	14be8 <__aeabi_lasr>
    afa0:	e597101c 	ldr	r1, [r7, #28]
    afa4:	e1a08000 	mov	r8, r0
    afa8:	e1a00006 	mov	r0, r6
    afac:	eb0025f6 	bl	1478c <__aeabi_uidiv>
    afb0:	e3a09000 	mov	r9, #0	; 0x0
    afb4:	e1a0b000 	mov	fp, r0
    afb8:	e1a06009 	mov	r6, r9
    afbc:	ea000022 	b	b04c <base_vs_erase+0x134>
    afc0:	e59d3004 	ldr	r3, [sp, #4]
    afc4:	e3530000 	cmp	r3, #0	; 0x0
    afc8:	1a000002 	bne	afd8 <base_vs_erase+0xc0>
    afcc:	ebffff36 	bl	acac <nnd_vs_isbad>
    afd0:	e3500000 	cmp	r0, #0	; 0x0
    afd4:	1a000010 	bne	b01c <base_vs_erase+0x104>
    afd8:	e1a01008 	mov	r1, r8
    afdc:	e5d72002 	ldrb	r2, [r7, #2]
    afe0:	e3a00000 	mov	r0, #0	; 0x0
    afe4:	eb000662 	bl	c974 <nf2_erase>
    afe8:	e250a000 	subs	sl, r0, #0	; 0x0
    afec:	0a000010 	beq	b034 <base_vs_erase+0x11c>
    aff0:	e5971020 	ldr	r1, [r7, #32]
    aff4:	e1a00008 	mov	r0, r8
    aff8:	eb0025e3 	bl	1478c <__aeabi_uidiv>
    affc:	e1a0200a 	mov	r2, sl
    b000:	e1a01000 	mov	r1, r0
    b004:	e59f3078 	ldr	r3, [pc, #120]	; b084 <base_vs_erase+0x16c>
    b008:	e59f007c 	ldr	r0, [pc, #124]	; b08c <base_vs_erase+0x174>
    b00c:	e1a0e00f 	mov	lr, pc
    b010:	e593f000 	ldr	pc, [r3]
    b014:	e3e06000 	mvn	r6, #0	; 0x0
    b018:	ea000005 	b	b034 <base_vs_erase+0x11c>
    b01c:	e5971014 	ldr	r1, [r7, #20]
    b020:	e59f0068 	ldr	r0, [pc, #104]	; b090 <base_vs_erase+0x178>
    b024:	e0010891 	mul	r1, r1, r8
    b028:	e59f3054 	ldr	r3, [pc, #84]	; b084 <base_vs_erase+0x16c>
    b02c:	e1a0e00f 	mov	lr, pc
    b030:	e593f000 	ldr	pc, [r3]
    b034:	e597201c 	ldr	r2, [r7, #28]
    b038:	e5973020 	ldr	r3, [r7, #32]
    b03c:	e0944002 	adds	r4, r4, r2
    b040:	e0888003 	add	r8, r8, r3
    b044:	e2a55000 	adc	r5, r5, #0	; 0x0
    b048:	e2899001 	add	r9, r9, #1	; 0x1
    b04c:	e159000b 	cmp	r9, fp
    b050:	e1a00004 	mov	r0, r4
    b054:	e1a01005 	mov	r1, r5
    b058:	baffffd8 	blt	afc0 <base_vs_erase+0xa8>
    b05c:	ea000004 	b	b074 <base_vs_erase+0x15c>
    b060:	e59f002c 	ldr	r0, [pc, #44]	; b094 <base_vs_erase+0x17c>
    b064:	e59f3018 	ldr	r3, [pc, #24]	; b084 <base_vs_erase+0x16c>
    b068:	e1a0e00f 	mov	lr, pc
    b06c:	e593f000 	ldr	pc, [r3]
    b070:	e3e06000 	mvn	r6, #0	; 0x0
    b074:	e1a00006 	mov	r0, r6
    b078:	e28dd00c 	add	sp, sp, #12	; 0xc
    b07c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b080:	0001833f 	.word	0x0001833f
    b084:	3c00d6cc 	.word	0x3c00d6cc
    b088:	3c00d734 	.word	0x3c00d734
    b08c:	00018366 	.word	0x00018366
    b090:	00018386 	.word	0x00018386
    b094:	0001839c 	.word	0x0001839c

0000b098 <nnd_vs_scrub>:
    b098:	e3a03001 	mov	r3, #1	; 0x1
    b09c:	eaffff9d 	b	af18 <base_vs_erase>

0000b0a0 <bnd_vs_erase>:
    b0a0:	e92d4030 	push	{r4, r5, lr}
    b0a4:	e59f3020 	ldr	r3, [pc, #32]	; b0cc <bnd_vs_erase+0x2c>
    b0a8:	e1a04002 	mov	r4, r2
    b0ac:	e5935008 	ldr	r5, [r3, #8]
    b0b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    b0b4:	e1a02005 	mov	r2, r5
    b0b8:	eb0026d1 	bl	14c04 <__aeabi_llsl>
    b0bc:	e1a02514 	lsl	r2, r4, r5
    b0c0:	e28dd004 	add	sp, sp, #4	; 0x4
    b0c4:	e8bd4030 	pop	{r4, r5, lr}
    b0c8:	eafffff2 	b	b098 <nnd_vs_scrub>
    b0cc:	3c00d734 	.word	0x3c00d734

0000b0d0 <nnd_vs_erase>:
    b0d0:	e3a03000 	mov	r3, #0	; 0x0
    b0d4:	eaffff8f 	b	af18 <base_vs_erase>

0000b0d8 <base_block_markbad>:
    b0d8:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
    b0dc:	e1a08000 	mov	r8, r0
    b0e0:	e24dd004 	sub	sp, sp, #4	; 0x4
    b0e4:	e3a00001 	mov	r0, #1	; 0x1
    b0e8:	e1a09001 	mov	r9, r1
    b0ec:	ebfffb90 	bl	9f34 <nand_get_config>
    b0f0:	e2504000 	subs	r4, r0, #0	; 0x0
    b0f4:	03e05000 	mvneq	r5, #0	; 0x0
    b0f8:	0a000029 	beq	b1a4 <base_block_markbad+0xcc>
    b0fc:	e594301c 	ldr	r3, [r4, #28]
    b100:	e59f00a8 	ldr	r0, [pc, #168]	; b1b0 <base_block_markbad+0xd8>
    b104:	e2433001 	sub	r3, r3, #1	; 0x1
    b108:	e1c86003 	bic	r6, r8, r3
    b10c:	e1a02006 	mov	r2, r6
    b110:	e1a03009 	mov	r3, r9
    b114:	e59f5098 	ldr	r5, [pc, #152]	; b1b4 <base_block_markbad+0xdc>
    b118:	e1a0e00f 	mov	lr, pc
    b11c:	e595f000 	ldr	pc, [r5]
    b120:	e1a00006 	mov	r0, r6
    b124:	e1a01009 	mov	r1, r9
    b128:	e594201c 	ldr	r2, [r4, #28]
    b12c:	ebffffe7 	bl	b0d0 <nnd_vs_erase>
    b130:	e3500000 	cmp	r0, #0	; 0x0
    b134:	e1a07009 	mov	r7, r9
    b138:	0a000002 	beq	b148 <base_block_markbad+0x70>
    b13c:	e59f0074 	ldr	r0, [pc, #116]	; b1b8 <base_block_markbad+0xe0>
    b140:	e1a0e00f 	mov	lr, pc
    b144:	e595f000 	ldr	pc, [r5]
    b148:	e5943014 	ldr	r3, [r4, #20]
    b14c:	e5942008 	ldr	r2, [r4, #8]
    b150:	e0020392 	mul	r2, r2, r3
    b154:	e0960002 	adds	r0, r6, r2
    b158:	e2a71000 	adc	r1, r7, #0	; 0x0
    b15c:	ebfffbd3 	bl	a0b0 <__base_block_markbad>
    b160:	e5943008 	ldr	r3, [r4, #8]
    b164:	e1a05000 	mov	r5, r0
    b168:	e594000c 	ldr	r0, [r4, #12]
    b16c:	e1500003 	cmp	r0, r3
    b170:	0a000005 	beq	b18c <base_block_markbad+0xb4>
    b174:	e5943014 	ldr	r3, [r4, #20]
    b178:	e0030093 	mul	r3, r3, r0
    b17c:	e0960003 	adds	r0, r6, r3
    b180:	e2a71000 	adc	r1, r7, #0	; 0x0
    b184:	ebfffbc9 	bl	a0b0 <__base_block_markbad>
    b188:	e1855000 	orr	r5, r5, r0
    b18c:	e3550000 	cmp	r5, #0	; 0x0
    b190:	0a000003 	beq	b1a4 <base_block_markbad+0xcc>
    b194:	e59f0020 	ldr	r0, [pc, #32]	; b1bc <base_block_markbad+0xe4>
    b198:	e59f3014 	ldr	r3, [pc, #20]	; b1b4 <base_block_markbad+0xdc>
    b19c:	e1a0e00f 	mov	lr, pc
    b1a0:	e593f000 	ldr	pc, [r3]
    b1a4:	e1a00005 	mov	r0, r5
    b1a8:	e28dd004 	add	sp, sp, #4	; 0x4
    b1ac:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
    b1b0:	000183b0 	.word	0x000183b0
    b1b4:	3c00d6cc 	.word	0x3c00d6cc
    b1b8:	000183d0 	.word	0x000183d0
    b1bc:	000183e8 	.word	0x000183e8

0000b1c0 <fnd_vs_erase>:
    b1c0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    b1c4:	e1a04000 	mov	r4, r0
    b1c8:	e24dd004 	sub	sp, sp, #4	; 0x4
    b1cc:	e3a00001 	mov	r0, #1	; 0x1
    b1d0:	e1a06002 	mov	r6, r2
    b1d4:	e1a05001 	mov	r5, r1
    b1d8:	ebfffb55 	bl	9f34 <nand_get_config>
    b1dc:	e1a07000 	mov	r7, r0
    b1e0:	ebfffb81 	bl	9fec <fnd_align>
    b1e4:	e1a01000 	mov	r1, r0
    b1e8:	e1a00006 	mov	r0, r6
    b1ec:	eb002566 	bl	1478c <__aeabi_uidiv>
    b1f0:	e5972014 	ldr	r2, [r7, #20]
    b1f4:	e1a01005 	mov	r1, r5
    b1f8:	e0020092 	mul	r2, r2, r0
    b1fc:	e1a00004 	mov	r0, r4
    b200:	e28dd004 	add	sp, sp, #4	; 0x4
    b204:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
    b208:	eaffffb0 	b	b0d0 <nnd_vs_erase>

0000b20c <nand_rescan_bootcfg>:
    b20c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    b210:	ebffd5e1 	bl	99c <ecfg_paras>
    b214:	e59f31ac 	ldr	r3, [pc, #428]	; b3c8 <nand_rescan_bootcfg+0x1bc>
    b218:	e1a08000 	mov	r8, r0
    b21c:	e1d360b0 	ldrh	r6, [r3]
    b220:	e3560000 	cmp	r6, #0	; 0x0
    b224:	1a00000b 	bne	b258 <nand_rescan_bootcfg+0x4c>
    b228:	ebffd504 	bl	640 <boot_device>
    b22c:	e3500000 	cmp	r0, #0	; 0x0
    b230:	1a000002 	bne	b240 <nand_rescan_bootcfg+0x34>
    b234:	ebffd4ee 	bl	5f4 <boot_info>
    b238:	e1a06000 	mov	r6, r0
    b23c:	ea000005 	b	b258 <nand_rescan_bootcfg+0x4c>
    b240:	e59f0184 	ldr	r0, [pc, #388]	; b3cc <nand_rescan_bootcfg+0x1c0>
    b244:	e59f3184 	ldr	r3, [pc, #388]	; b3d0 <nand_rescan_bootcfg+0x1c4>
    b248:	e1a0e00f 	mov	lr, pc
    b24c:	e593f000 	ldr	pc, [r3]
    b250:	e3e00000 	mvn	r0, #0	; 0x0
    b254:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    b258:	e59f7174 	ldr	r7, [pc, #372]	; b3d4 <nand_rescan_bootcfg+0x1c8>
    b25c:	e1a00426 	lsr	r0, r6, #8
    b260:	e1a023a6 	lsr	r2, r6, #7
    b264:	e200000f 	and	r0, r0, #15	; 0xf
    b268:	e2022001 	and	r2, r2, #1	; 0x1
    b26c:	e3a03001 	mov	r3, #1	; 0x1
    b270:	e1a01000 	mov	r1, r0
    b274:	e5873004 	str	r3, [r7, #4]
    b278:	e5c72019 	strb	r2, [r7, #25]
    b27c:	e5c70017 	strb	r0, [r7, #23]
    b280:	e59f5148 	ldr	r5, [pc, #328]	; b3d0 <nand_rescan_bootcfg+0x1c4>
    b284:	e59f014c 	ldr	r0, [pc, #332]	; b3d8 <nand_rescan_bootcfg+0x1cc>
    b288:	e1a0e00f 	mov	lr, pc
    b28c:	e595f000 	ldr	pc, [r5]
    b290:	e3160070 	tst	r6, #112	; 0x70
    b294:	e3a0cb01 	mov	ip, #1024	; 0x400
    b298:	13a04010 	movne	r4, #16	; 0x10
    b29c:	03a04008 	moveq	r4, #8	; 0x8
    b2a0:	e1a03126 	lsr	r3, r6, #2
    b2a4:	e3160002 	tst	r6, #2	; 0x2
    b2a8:	13a01005 	movne	r1, #5	; 0x5
    b2ac:	03a01004 	moveq	r1, #4	; 0x4
    b2b0:	e1a0000c 	mov	r0, ip
    b2b4:	e2033003 	and	r3, r3, #3	; 0x3
    b2b8:	e2062001 	and	r2, r6, #1	; 0x1
    b2bc:	e587c028 	str	ip, [r7, #40]
    b2c0:	e5c71016 	strb	r1, [r7, #22]
    b2c4:	e5c73014 	strb	r3, [r7, #20]
    b2c8:	e5c72015 	strb	r2, [r7, #21]
    b2cc:	e5c7401a 	strb	r4, [r7, #26]
    b2d0:	ebfffaa6 	bl	9d70 <generic_ffs>
    b2d4:	e59730c4 	ldr	r3, [r7, #196]
    b2d8:	e2402001 	sub	r2, r0, #1	; 0x1
    b2dc:	e1a00523 	lsr	r0, r3, #10
    b2e0:	e587200c 	str	r2, [r7, #12]
    b2e4:	ebfffaa1 	bl	9d70 <generic_ffs>
    b2e8:	e2400001 	sub	r0, r0, #1	; 0x1
    b2ec:	e5870008 	str	r0, [r7, #8]
    b2f0:	e1a01004 	mov	r1, r4
    b2f4:	e59f00e0 	ldr	r0, [pc, #224]	; b3dc <nand_rescan_bootcfg+0x1d0>
    b2f8:	e1a0e00f 	mov	lr, pc
    b2fc:	e595f000 	ldr	pc, [r5]
    b300:	e3a00c02 	mov	r0, #512	; 0x200
    b304:	ebffd59b 	bl	978 <ecfg_check_flag>
    b308:	e3500000 	cmp	r0, #0	; 0x0
    b30c:	0a000008 	beq	b334 <nand_rescan_bootcfg+0x128>
    b310:	e30c30c0 	movw	r3, #49344	; 0xc0c0
    b314:	e5873048 	str	r3, [r7, #72]
    b318:	e3e034ae 	mvn	r3, #-1375731712	; 0xae000000
    b31c:	e587303c 	str	r3, [r7, #60]
    b320:	e59f30b8 	ldr	r3, [pc, #184]	; b3e0 <nand_rescan_bootcfg+0x1d4>
    b324:	e5873040 	str	r3, [r7, #64]
    b328:	e3043023 	movw	r3, #16419	; 0x4023
    b32c:	e5873044 	str	r3, [r7, #68]
    b330:	ea000008 	b	b358 <nand_rescan_bootcfg+0x14c>
    b334:	e5d83007 	ldrb	r3, [r8, #7]
    b338:	e1d810ba 	ldrh	r1, [r8, #10]
    b33c:	e1a03c03 	lsl	r3, r3, #24
    b340:	e1e03c23 	mvn	r3, r3, lsr #24
    b344:	e1d820bc 	ldrh	r2, [r8, #12]
    b348:	e1e03c03 	mvn	r3, r3, lsl #24
    b34c:	e5872048 	str	r2, [r7, #72]
    b350:	e587303c 	str	r3, [r7, #60]
    b354:	e5871044 	str	r1, [r7, #68]
    b358:	e3a00c01 	mov	r0, #256	; 0x100
    b35c:	ebffd585 	bl	978 <ecfg_check_flag>
    b360:	e3500000 	cmp	r0, #0	; 0x0
    b364:	e59f1068 	ldr	r1, [pc, #104]	; b3d4 <nand_rescan_bootcfg+0x1c8>
    b368:	0a00000b 	beq	b39c <nand_rescan_bootcfg+0x190>
    b36c:	e3003994 	movw	r3, #2452	; 0x994
    b370:	e581305c 	str	r3, [r1, #92]
    b374:	e59f3068 	ldr	r3, [pc, #104]	; b3e4 <nand_rescan_bootcfg+0x1d8>
    b378:	e3a00000 	mov	r0, #0	; 0x0
    b37c:	e581304c 	str	r3, [r1, #76]
    b380:	e59f3060 	ldr	r3, [pc, #96]	; b3e8 <nand_rescan_bootcfg+0x1dc>
    b384:	e5813050 	str	r3, [r1, #80]
    b388:	e59f305c 	ldr	r3, [pc, #92]	; b3ec <nand_rescan_bootcfg+0x1e0>
    b38c:	e5813054 	str	r3, [r1, #84]
    b390:	e59f3058 	ldr	r3, [pc, #88]	; b3f0 <nand_rescan_bootcfg+0x1e4>
    b394:	e5813058 	str	r3, [r1, #88]
    b398:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    b39c:	e5983010 	ldr	r3, [r8, #16]
    b3a0:	e1d820be 	ldrh	r2, [r8, #14]
    b3a4:	e581304c 	str	r3, [r1, #76]
    b3a8:	e5983014 	ldr	r3, [r8, #20]
    b3ac:	e5813050 	str	r3, [r1, #80]
    b3b0:	e5983018 	ldr	r3, [r8, #24]
    b3b4:	e5813054 	str	r3, [r1, #84]
    b3b8:	e598301c 	ldr	r3, [r8, #28]
    b3bc:	e581205c 	str	r2, [r1, #92]
    b3c0:	e5813058 	str	r3, [r1, #88]
    b3c4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    b3c8:	3c00d1d0 	.word	0x3c00d1d0
    b3cc:	0001840f 	.word	0x0001840f
    b3d0:	3c00d6cc 	.word	0x3c00d6cc
    b3d4:	3c00d734 	.word	0x3c00d734
    b3d8:	0001842d 	.word	0x0001842d
    b3dc:	00018447 	.word	0x00018447
    b3e0:	00fffaf0 	.word	0x00fffaf0
    b3e4:	1c723550 	.word	0x1c723550
    b3e8:	20fe074e 	.word	0x20fe074e
    b3ec:	1698347e 	.word	0x1698347e
    b3f0:	3fff2c80 	.word	0x3fff2c80

0000b3f4 <nand_init_randomizer>:
    b3f4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b3f8:	e5d03001 	ldrb	r3, [r0, #1]
    b3fc:	e24dd004 	sub	sp, sp, #4	; 0x4
    b400:	e3530000 	cmp	r3, #0	; 0x0
    b404:	e1a05000 	mov	r5, r0
    b408:	0a000036 	beq	b4e8 <nand_init_randomizer+0xf4>
    b40c:	e5901014 	ldr	r1, [r0, #20]
    b410:	e3a02000 	mov	r2, #0	; 0x0
    b414:	e5d00003 	ldrb	r0, [r0, #3]
    b418:	eb0003e8 	bl	c3c0 <nf2_ecc_num>
    b41c:	e1a04000 	mov	r4, r0
    b420:	e1a01004 	mov	r1, r4
    b424:	e59f00c8 	ldr	r0, [pc, #200]	; b4f4 <nand_init_randomizer+0x100>
    b428:	e59f30c8 	ldr	r3, [pc, #200]	; b4f8 <nand_init_randomizer+0x104>
    b42c:	e1a0e00f 	mov	lr, pc
    b430:	e593f000 	ldr	pc, [r3]
    b434:	e5d53006 	ldrb	r3, [r5, #6]
    b438:	e5952014 	ldr	r2, [r5, #20]
    b43c:	e3530010 	cmp	r3, #16	; 0x10
    b440:	e5953010 	ldr	r3, [r5, #16]
    b444:	e59f00b0 	ldr	r0, [pc, #176]	; b4fc <nand_init_randomizer+0x108>
    b448:	11a07002 	movne	r7, r2
    b44c:	11a06003 	movne	r6, r3
    b450:	01a070a2 	lsreq	r7, r2, #1
    b454:	01a060a3 	lsreq	r6, r3, #1
    b458:	01a040c4 	asreq	r4, r4, #1
    b45c:	ebffd4c7 	bl	780 <module_enable>
    b460:	e5950048 	ldr	r0, [r5, #72]
    b464:	eb000485 	bl	c680 <nf2_set_polynomial>
    b468:	e247b001 	sub	fp, r7, #1	; 0x1
    b46c:	e2869001 	add	r9, r6, #1	; 0x1
    b470:	e244a001 	sub	sl, r4, #1	; 0x1
    b474:	e2858038 	add	r8, r5, #56	; 0x38
    b478:	e1a04005 	mov	r4, r5
    b47c:	e3a07000 	mov	r7, #0	; 0x0
    b480:	e2866003 	add	r6, r6, #3	; 0x3
    b484:	e19800b7 	ldrh	r0, [r8, r7]
    b488:	e1a0100b 	mov	r1, fp
    b48c:	e1a00880 	lsl	r0, r0, #17
    b490:	e5952048 	ldr	r2, [r5, #72]
    b494:	e1a008a0 	lsr	r0, r0, #17
    b498:	eb0007a2 	bl	d328 <nf2_randc_seed_hw>
    b49c:	e5d53006 	ldrb	r3, [r5, #6]
    b4a0:	e6ff2070 	uxth	r2, r0
    b4a4:	e3530010 	cmp	r3, #16	; 0x10
    b4a8:	e1a01006 	mov	r1, r6
    b4ac:	e1c424bc 	strh	r2, [r4, #76]
    b4b0:	01a01009 	moveq	r1, r9
    b4b4:	e1a00002 	mov	r0, r2
    b4b8:	e5952048 	ldr	r2, [r5, #72]
    b4bc:	eb000799 	bl	d328 <nf2_randc_seed_hw>
    b4c0:	e1c405bc 	strh	r0, [r4, #92]
    b4c4:	e1d405bc 	ldrh	r0, [r4, #92]
    b4c8:	e1a0100a 	mov	r1, sl
    b4cc:	e5952048 	ldr	r2, [r5, #72]
    b4d0:	eb000794 	bl	d328 <nf2_randc_seed_hw>
    b4d4:	e2877002 	add	r7, r7, #2	; 0x2
    b4d8:	e3570010 	cmp	r7, #16	; 0x10
    b4dc:	e1c406bc 	strh	r0, [r4, #108]
    b4e0:	e2844002 	add	r4, r4, #2	; 0x2
    b4e4:	1affffe6 	bne	b484 <nand_init_randomizer+0x90>
    b4e8:	e3a00000 	mov	r0, #0	; 0x0
    b4ec:	e28dd004 	add	sp, sp, #4	; 0x4
    b4f0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b4f4:	00018455 	.word	0x00018455
    b4f8:	3c00d6cc 	.word	0x3c00d6cc
    b4fc:	21e3c000 	.word	0x21e3c000

0000b500 <nand_calculate_rnbtimeout>:
    b500:	e92d4030 	push	{r4, r5, lr}
    b504:	e59f4040 	ldr	r4, [pc, #64]	; b54c <nand_calculate_rnbtimeout+0x4c>
    b508:	e24dd004 	sub	sp, sp, #4	; 0x4
    b50c:	e1d053bc 	ldrh	r5, [r0, #60]
    b510:	e59f0038 	ldr	r0, [pc, #56]	; b550 <nand_calculate_rnbtimeout+0x50>
    b514:	ebffd70b 	bl	1148 <module_get_clock>
    b518:	e1a01004 	mov	r1, r4
    b51c:	eb00251c 	bl	14994 <__aeabi_idiv>
    b520:	e1a01000 	mov	r1, r0
    b524:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    b528:	eb002519 	bl	14994 <__aeabi_idiv>
    b52c:	e0050594 	mul	r5, r4, r5
    b530:	e1a01000 	mov	r1, r0
    b534:	e1a00005 	mov	r0, r5
    b538:	eb002515 	bl	14994 <__aeabi_idiv>
    b53c:	e3c004ff 	bic	r0, r0, #-16777216	; 0xff000000
    b540:	e3c000ff 	bic	r0, r0, #255	; 0xff
    b544:	e28dd004 	add	sp, sp, #4	; 0x4
    b548:	e8bd8030 	pop	{r4, r5, pc}
    b54c:	000f4240 	.word	0x000f4240
    b550:	21e0a270 	.word	0x21e0a270

0000b554 <nand_calculate_synctiming>:
    b554:	e92d4030 	push	{r4, r5, lr}
    b558:	e1a05000 	mov	r5, r0
    b55c:	e24dd004 	sub	sp, sp, #4	; 0x4
    b560:	e59f0044 	ldr	r0, [pc, #68]	; b5ac <nand_calculate_synctiming+0x58>
    b564:	ebffd6f7 	bl	1148 <module_get_clock>
    b568:	e59f1040 	ldr	r1, [pc, #64]	; b5b0 <nand_calculate_synctiming+0x5c>
    b56c:	eb002508 	bl	14994 <__aeabi_idiv>
    b570:	e1a01000 	mov	r1, r0
    b574:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    b578:	eb002505 	bl	14994 <__aeabi_idiv>
    b57c:	e1a04000 	mov	r4, r0
    b580:	e1a01000 	mov	r1, r0
    b584:	e5d5002c 	ldrb	r0, [r5, #44]
    b588:	eb002501 	bl	14994 <__aeabi_idiv>
    b58c:	e1a01004 	mov	r1, r4
    b590:	e2804001 	add	r4, r0, #1	; 0x1
    b594:	e5d5002d 	ldrb	r0, [r5, #45]
    b598:	eb0024fd 	bl	14994 <__aeabi_idiv>
    b59c:	e2800002 	add	r0, r0, #2	; 0x2
    b5a0:	e1840200 	orr	r0, r4, r0, lsl #4
    b5a4:	e28dd004 	add	sp, sp, #4	; 0x4
    b5a8:	e8bd8030 	pop	{r4, r5, pc}
    b5ac:	21e0a270 	.word	0x21e0a270
    b5b0:	007a1200 	.word	0x007a1200

0000b5b4 <nand_calculate_phydelay>:
    b5b4:	e92d4030 	push	{r4, r5, lr}
    b5b8:	e1d043be 	ldrh	r4, [r0, #62]
    b5bc:	e24dd004 	sub	sp, sp, #4	; 0x4
    b5c0:	e59f0050 	ldr	r0, [pc, #80]	; b618 <nand_calculate_phydelay+0x64>
    b5c4:	ebffd6df 	bl	1148 <module_get_clock>
    b5c8:	e59f104c 	ldr	r1, [pc, #76]	; b61c <nand_calculate_phydelay+0x68>
    b5cc:	eb0024f0 	bl	14994 <__aeabi_idiv>
    b5d0:	e1a05424 	lsr	r5, r4, #8
    b5d4:	e20440ff 	and	r4, r4, #255	; 0xff
    b5d8:	e3540000 	cmp	r4, #0	; 0x0
    b5dc:	03a04001 	moveq	r4, #1	; 0x1
    b5e0:	e0010490 	mul	r1, r0, r4
    b5e4:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    b5e8:	eb0024e9 	bl	14994 <__aeabi_idiv>
    b5ec:	e3550000 	cmp	r5, #0	; 0x0
    b5f0:	03a05001 	moveq	r5, #1	; 0x1
    b5f4:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    b5f8:	e0000093 	mul	r0, r3, r0
    b5fc:	e1a01005 	mov	r1, r5
    b600:	eb0024e3 	bl	14994 <__aeabi_idiv>
    b604:	e35000ff 	cmp	r0, #255	; 0xff
    b608:	c30f0efe 	movwgt	r0, #65278	; 0xfefe
    b60c:	d1800400 	orrle	r0, r0, r0, lsl #8
    b610:	e28dd004 	add	sp, sp, #4	; 0x4
    b614:	e8bd8030 	pop	{r4, r5, pc}
    b618:	a270      	.short	0xa270
    b61a:	21e0      	.short	0x21e0
    b61c:	007a1200 	.word	0x007a1200

0000b620 <nand_calculate_timing>:
    b620:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    b624:	e1a04000 	mov	r4, r0
    b628:	e59f0108 	ldr	r0, [pc, #264]	; b738 <nand_calculate_timing+0x118>
    b62c:	ebffd6c5 	bl	1148 <module_get_clock>
    b630:	e59f1104 	ldr	r1, [pc, #260]	; b73c <nand_calculate_timing+0x11c>
    b634:	eb0024d6 	bl	14994 <__aeabi_idiv>
    b638:	e1a01000 	mov	r1, r0
    b63c:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    b640:	eb0024d3 	bl	14994 <__aeabi_idiv>
    b644:	e5d45029 	ldrb	r5, [r4, #41]
    b648:	e5d43026 	ldrb	r3, [r4, #38]
    b64c:	e1a07000 	mov	r7, r0
    b650:	e5d46025 	ldrb	r6, [r4, #37]
    b654:	e0653003 	rsb	r3, r5, r3
    b658:	e5d42024 	ldrb	r2, [r4, #36]
    b65c:	e5d4102a 	ldrb	r1, [r4, #42]
    b660:	e5d4002b 	ldrb	r0, [r4, #43]
    b664:	e5d4c028 	ldrb	ip, [r4, #40]
    b668:	e1c33fc3 	bic	r3, r3, r3, asr #31
    b66c:	e1520001 	cmp	r2, r1
    b670:	31a02001 	movcc	r2, r1
    b674:	e1500005 	cmp	r0, r5
    b678:	21a08000 	movcs	r8, r0
    b67c:	31a08005 	movcc	r8, r5
    b680:	e15c0006 	cmp	ip, r6
    b684:	21a0900c 	movcs	r9, ip
    b688:	31a09006 	movcc	r9, r6
    b68c:	e1570003 	cmp	r7, r3
    b690:	e2825005 	add	r5, r2, #5	; 0x5
    b694:	e5d44027 	ldrb	r4, [r4, #39]
    b698:	c3a0a001 	movgt	sl, #1	; 0x1
    b69c:	ca000003 	bgt	b6b0 <nand_calculate_timing+0x90>
    b6a0:	e1a00003 	mov	r0, r3
    b6a4:	e1a01007 	mov	r1, r7
    b6a8:	eb0024b9 	bl	14994 <__aeabi_idiv>
    b6ac:	e280a001 	add	sl, r0, #1	; 0x1
    b6b0:	e1570005 	cmp	r7, r5
    b6b4:	c3a06001 	movgt	r6, #1	; 0x1
    b6b8:	ca000003 	bgt	b6cc <nand_calculate_timing+0xac>
    b6bc:	e1a00005 	mov	r0, r5
    b6c0:	e1a01007 	mov	r1, r7
    b6c4:	eb0024b2 	bl	14994 <__aeabi_idiv>
    b6c8:	e2806001 	add	r6, r0, #1	; 0x1
    b6cc:	e1570008 	cmp	r7, r8
    b6d0:	c3a05001 	movgt	r5, #1	; 0x1
    b6d4:	ca000003 	bgt	b6e8 <nand_calculate_timing+0xc8>
    b6d8:	e1a00008 	mov	r0, r8
    b6dc:	e1a01007 	mov	r1, r7
    b6e0:	eb0024ab 	bl	14994 <__aeabi_idiv>
    b6e4:	e2805001 	add	r5, r0, #1	; 0x1
    b6e8:	e1570004 	cmp	r7, r4
    b6ec:	c3a04001 	movgt	r4, #1	; 0x1
    b6f0:	ca000003 	bgt	b704 <nand_calculate_timing+0xe4>
    b6f4:	e1a00004 	mov	r0, r4
    b6f8:	e1a01007 	mov	r1, r7
    b6fc:	eb0024a4 	bl	14994 <__aeabi_idiv>
    b700:	e2804001 	add	r4, r0, #1	; 0x1
    b704:	e1570009 	cmp	r7, r9
    b708:	c3a03001 	movgt	r3, #1	; 0x1
    b70c:	ca000003 	bgt	b720 <nand_calculate_timing+0x100>
    b710:	e1a00009 	mov	r0, r9
    b714:	e1a01007 	mov	r1, r7
    b718:	eb00249d 	bl	14994 <__aeabi_idiv>
    b71c:	e2803001 	add	r3, r0, #1	; 0x1
    b720:	e1a0080a 	lsl	r0, sl, #16
    b724:	e1800606 	orr	r0, r0, r6, lsl #12
    b728:	e1800405 	orr	r0, r0, r5, lsl #8
    b72c:	e1800204 	orr	r0, r0, r4, lsl #4
    b730:	e1800003 	orr	r0, r0, r3
    b734:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    b738:	21e0a270 	.word	0x21e0a270
    b73c:	000f4240 	.word	0x000f4240

0000b740 <nand_rescan_config>:
    b740:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b744:	e59f343c 	ldr	r3, [pc, #1084]	; bb88 <nand_rescan_config+0x448>
    b748:	e24dd024 	sub	sp, sp, #36	; 0x24
    b74c:	e1a04000 	mov	r4, r0
    b750:	e59f0434 	ldr	r0, [pc, #1076]	; bb8c <nand_rescan_config+0x44c>
    b754:	e1a0e00f 	mov	lr, pc
    b758:	e593f000 	ldr	pc, [r3]
    b75c:	e59f042c 	ldr	r0, [pc, #1068]	; bb90 <nand_rescan_config+0x450>
    b760:	ebffd5db 	bl	ed4 <cpu_recase>
    b764:	ebfffea8 	bl	b20c <nand_rescan_bootcfg>
    b768:	e3500000 	cmp	r0, #0	; 0x0
    b76c:	1a000001 	bne	b778 <nand_rescan_config+0x38>
    b770:	e59f041c 	ldr	r0, [pc, #1052]	; bb94 <nand_rescan_config+0x454>
    b774:	ebffff1e 	bl	b3f4 <nand_init_randomizer>
    b778:	e3540000 	cmp	r4, #0	; 0x0
    b77c:	11a07004 	movne	r7, r4
    b780:	1a0000ee 	bne	bb40 <nand_rescan_config+0x400>
    b784:	ea0000fc 	b	bb7c <nand_rescan_config+0x43c>
    b788:	e1a00007 	mov	r0, r7
    b78c:	ebffffa3 	bl	b620 <nand_calculate_timing>
    b790:	e1a04000 	mov	r4, r0
    b794:	e1a00007 	mov	r0, r7
    b798:	ebffff6d 	bl	b554 <nand_calculate_synctiming>
    b79c:	e184bc00 	orr	fp, r4, r0, lsl #24
    b7a0:	e1a00007 	mov	r0, r7
    b7a4:	ebffff82 	bl	b5b4 <nand_calculate_phydelay>
    b7a8:	e1a09000 	mov	r9, r0
    b7ac:	e1a00007 	mov	r0, r7
    b7b0:	ebffff52 	bl	b500 <nand_calculate_rnbtimeout>
    b7b4:	e1a0a000 	mov	sl, r0
    b7b8:	e1a0100b 	mov	r1, fp
    b7bc:	e59f03d4 	ldr	r0, [pc, #980]	; bb98 <nand_rescan_config+0x458>
    b7c0:	e1a02009 	mov	r2, r9
    b7c4:	e1a0300a 	mov	r3, sl
    b7c8:	e59fc3b8 	ldr	ip, [pc, #952]	; bb88 <nand_rescan_config+0x448>
    b7cc:	e1a0e00f 	mov	lr, pc
    b7d0:	e59cf000 	ldr	pc, [ip]
    b7d4:	e5971054 	ldr	r1, [r7, #84]
    b7d8:	e1d744b0 	ldrh	r4, [r7, #64]
    b7dc:	e1a0c121 	lsr	ip, r1, #2
    b7e0:	e20cc003 	and	ip, ip, #3	; 0x3
    b7e4:	e2011003 	and	r1, r1, #3	; 0x3
    b7e8:	e1a0200b 	mov	r2, fp
    b7ec:	e1a0300a 	mov	r3, sl
    b7f0:	e28d0018 	add	r0, sp, #24	; 0x18
    b7f4:	e88d1210 	stm	sp, {r4, r9, ip}
    b7f8:	eb000782 	bl	d608 <nand_readid>
    b7fc:	e5dd001b 	ldrb	r0, [sp, #27]
    b800:	e5ddc01c 	ldrb	ip, [sp, #28]
    b804:	e5dd401d 	ldrb	r4, [sp, #29]
    b808:	e5dd501e 	ldrb	r5, [sp, #30]
    b80c:	e5dd601f 	ldrb	r6, [sp, #31]
    b810:	e5dd1018 	ldrb	r1, [sp, #24]
    b814:	e88d1001 	stm	sp, {r0, ip}
    b818:	e58d4008 	str	r4, [sp, #8]
    b81c:	e5dd2019 	ldrb	r2, [sp, #25]
    b820:	e5dd301a 	ldrb	r3, [sp, #26]
    b824:	e58d500c 	str	r5, [sp, #12]
    b828:	e58d6010 	str	r6, [sp, #16]
    b82c:	e59f0368 	ldr	r0, [pc, #872]	; bb9c <nand_rescan_config+0x45c>
    b830:	e59fc350 	ldr	ip, [pc, #848]	; bb88 <nand_rescan_config+0x448>
    b834:	e1a0e00f 	mov	lr, pc
    b838:	e59cf000 	ldr	pc, [ip]
    b83c:	e3a04000 	mov	r4, #0	; 0x0
    b840:	e1a01007 	mov	r1, r7
    b844:	e5d1301c 	ldrb	r3, [r1, #28]
    b848:	e2811001 	add	r1, r1, #1	; 0x1
    b84c:	e3530000 	cmp	r3, #0	; 0x0
    b850:	0a000009 	beq	b87c <nand_rescan_config+0x13c>
    b854:	e28d0018 	add	r0, sp, #24	; 0x18
    b858:	e7d02004 	ldrb	r2, [r0, r4]
    b85c:	e1520003 	cmp	r2, r3
    b860:	0a000005 	beq	b87c <nand_rescan_config+0x13c>
    b864:	e59f0334 	ldr	r0, [pc, #820]	; bba0 <nand_rescan_config+0x460>
    b868:	e1a01004 	mov	r1, r4
    b86c:	e59fc314 	ldr	ip, [pc, #788]	; bb88 <nand_rescan_config+0x448>
    b870:	e1a0e00f 	mov	lr, pc
    b874:	e59cf000 	ldr	pc, [ip]
    b878:	ea000002 	b	b888 <nand_rescan_config+0x148>
    b87c:	e2844001 	add	r4, r4, #1	; 0x1
    b880:	e3540008 	cmp	r4, #8	; 0x8
    b884:	1affffee 	bne	b844 <nand_rescan_config+0x104>
    b888:	e3540008 	cmp	r4, #8	; 0x8
    b88c:	e2877064 	add	r7, r7, #100	; 0x64
    b890:	1a0000aa 	bne	bb40 <nand_rescan_config+0x400>
    b894:	e59f32ec 	ldr	r3, [pc, #748]	; bb88 <nand_rescan_config+0x448>
    b898:	e59f0304 	ldr	r0, [pc, #772]	; bba4 <nand_rescan_config+0x464>
    b89c:	e1a0e00f 	mov	lr, pc
    b8a0:	e593f000 	ldr	pc, [r3]
    b8a4:	e5983054 	ldr	r3, [r8, #84]
    b8a8:	e59f22f8 	ldr	r2, [pc, #760]	; bba8 <nand_rescan_config+0x468>
    b8ac:	e2033003 	and	r3, r3, #3	; 0x3
    b8b0:	e5c230b0 	strb	r3, [r2, #176]
    b8b4:	e5983054 	ldr	r3, [r8, #84]
    b8b8:	e1a033a3 	lsr	r3, r3, #7
    b8bc:	e2033001 	and	r3, r3, #1	; 0x1
    b8c0:	e5c230b1 	strb	r3, [r2, #177]
    b8c4:	e5983054 	ldr	r3, [r8, #84]
    b8c8:	e1a03223 	lsr	r3, r3, #4
    b8cc:	e2033007 	and	r3, r3, #7	; 0x7
    b8d0:	e5c230b2 	strb	r3, [r2, #178]
    b8d4:	e5d81055 	ldrb	r1, [r8, #85]
    b8d8:	e351004f 	cmp	r1, #79	; 0x4f
    b8dc:	c3a0100c 	movgt	r1, #12	; 0xc
    b8e0:	ca000020 	bgt	b968 <nand_rescan_config+0x228>
    b8e4:	e3510047 	cmp	r1, #71	; 0x47
    b8e8:	c3a0100b 	movgt	r1, #11	; 0xb
    b8ec:	ca00001d 	bgt	b968 <nand_rescan_config+0x228>
    b8f0:	e351003f 	cmp	r1, #63	; 0x3f
    b8f4:	c3a0100a 	movgt	r1, #10	; 0xa
    b8f8:	ca00001a 	bgt	b968 <nand_rescan_config+0x228>
    b8fc:	e351003b 	cmp	r1, #59	; 0x3b
    b900:	c3a01009 	movgt	r1, #9	; 0x9
    b904:	ca000017 	bgt	b968 <nand_rescan_config+0x228>
    b908:	e3510037 	cmp	r1, #55	; 0x37
    b90c:	c1a01004 	movgt	r1, r4
    b910:	ca000014 	bgt	b968 <nand_rescan_config+0x228>
    b914:	e351002f 	cmp	r1, #47	; 0x2f
    b918:	c3a01007 	movgt	r1, #7	; 0x7
    b91c:	ca000011 	bgt	b968 <nand_rescan_config+0x228>
    b920:	e3510027 	cmp	r1, #39	; 0x27
    b924:	c3a01006 	movgt	r1, #6	; 0x6
    b928:	ca00000e 	bgt	b968 <nand_rescan_config+0x228>
    b92c:	e351001f 	cmp	r1, #31	; 0x1f
    b930:	c3a01005 	movgt	r1, #5	; 0x5
    b934:	ca00000b 	bgt	b968 <nand_rescan_config+0x228>
    b938:	e3510017 	cmp	r1, #23	; 0x17
    b93c:	c3a01004 	movgt	r1, #4	; 0x4
    b940:	ca000008 	bgt	b968 <nand_rescan_config+0x228>
    b944:	e351000f 	cmp	r1, #15	; 0xf
    b948:	c3a01003 	movgt	r1, #3	; 0x3
    b94c:	ca000005 	bgt	b968 <nand_rescan_config+0x228>
    b950:	e3510007 	cmp	r1, #7	; 0x7
    b954:	c3a01002 	movgt	r1, #2	; 0x2
    b958:	ca000002 	bgt	b968 <nand_rescan_config+0x228>
    b95c:	e3510003 	cmp	r1, #3	; 0x3
    b960:	d3a01000 	movle	r1, #0	; 0x0
    b964:	c3a01001 	movgt	r1, #1	; 0x1
    b968:	e59f2238 	ldr	r2, [pc, #568]	; bba8 <nand_rescan_config+0x468>
    b96c:	e5c210b3 	strb	r1, [r2, #179]
    b970:	e5d83056 	ldrb	r3, [r8, #86]
    b974:	e3530007 	cmp	r3, #7	; 0x7
    b978:	93a03000 	movls	r3, #0	; 0x0
    b97c:	83a03001 	movhi	r3, #1	; 0x1
    b980:	e5c230b4 	strb	r3, [r2, #180]
    b984:	e5983054 	ldr	r3, [r8, #84]
    b988:	e1a03423 	lsr	r3, r3, #8
    b98c:	e1a03803 	lsl	r3, r3, #16
    b990:	e1a03823 	lsr	r3, r3, #16
    b994:	e2533000 	subs	r3, r3, #0	; 0x0
    b998:	13a03001 	movne	r3, #1	; 0x1
    b99c:	e5c230b5 	strb	r3, [r2, #181]
    b9a0:	e1d8e3b0 	ldrh	lr, [r8, #48]
    b9a4:	e1d803b8 	ldrh	r0, [r8, #56]
    b9a8:	e1d813b6 	ldrh	r1, [r8, #54]
    b9ac:	e1d833ba 	ldrh	r3, [r8, #58]
    b9b0:	e35e0000 	cmp	lr, #0	; 0x0
    b9b4:	e58200bc 	str	r0, [r2, #188]
    b9b8:	e58230c0 	str	r3, [r2, #192]
    b9bc:	e58210b8 	str	r1, [r2, #184]
    b9c0:	01a0000e 	moveq	r0, lr
    b9c4:	0a00000c 	beq	b9fc <nand_rescan_config+0x2bc>
    b9c8:	e1a0380e 	lsl	r3, lr, #16
    b9cc:	e31304ff 	tst	r3, #-16777216	; 0xff000000
    b9d0:	01a03403 	lsleq	r3, r3, #8
    b9d4:	e3a00010 	mov	r0, #16	; 0x10
    b9d8:	02400008 	subeq	r0, r0, #8	; 0x8
    b9dc:	e313020f 	tst	r3, #-268435456	; 0xf0000000
    b9e0:	01a03203 	lsleq	r3, r3, #4
    b9e4:	02400004 	subeq	r0, r0, #4	; 0x4
    b9e8:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    b9ec:	01a03103 	lsleq	r3, r3, #2
    b9f0:	02400002 	subeq	r0, r0, #2	; 0x2
    b9f4:	e3530000 	cmp	r3, #0	; 0x0
    b9f8:	a2400001 	subge	r0, r0, #1	; 0x1
    b9fc:	e3a04001 	mov	r4, #1	; 0x1
    ba00:	e2400001 	sub	r0, r0, #1	; 0x1
    ba04:	e1a00014 	lsl	r0, r4, r0
    ba08:	e1d823b2 	ldrh	r2, [r8, #50]
    ba0c:	e59f5194 	ldr	r5, [pc, #404]	; bba8 <nand_rescan_config+0x468>
    ba10:	e0020092 	mul	r2, r2, r0
    ba14:	e1d8c4b0 	ldrh	ip, [r8, #64]
    ba18:	e1d813b4 	ldrh	r1, [r8, #52]
    ba1c:	e2403001 	sub	r3, r0, #1	; 0x1
    ba20:	e003300e 	and	r3, r3, lr
    ba24:	e58530c8 	str	r3, [r5, #200]
    ba28:	e58520cc 	str	r2, [r5, #204]
    ba2c:	e58510d4 	str	r1, [r5, #212]
    ba30:	e1a02009 	mov	r2, r9
    ba34:	e58dc000 	str	ip, [sp]
    ba38:	e1a0100b 	mov	r1, fp
    ba3c:	e1a0300a 	mov	r3, sl
    ba40:	e58500c4 	str	r0, [r5, #196]
    ba44:	e585c0e0 	str	ip, [r5, #224]
    ba48:	e59f015c 	ldr	r0, [pc, #348]	; bbac <nand_rescan_config+0x46c>
    ba4c:	e59fc134 	ldr	ip, [pc, #308]	; bb88 <nand_rescan_config+0x448>
    ba50:	e585b0d8 	str	fp, [r5, #216]
    ba54:	e58590e4 	str	r9, [r5, #228]
    ba58:	e585a0dc 	str	sl, [r5, #220]
    ba5c:	e1a0e00f 	mov	lr, pc
    ba60:	e59cf000 	ldr	pc, [ip]
    ba64:	e5983044 	ldr	r3, [r8, #68]
    ba68:	e1d824b2 	ldrh	r2, [r8, #66]
    ba6c:	e58530e8 	str	r3, [r5, #232]
    ba70:	e5983048 	ldr	r3, [r8, #72]
    ba74:	e1a00005 	mov	r0, r5
    ba78:	e58530ec 	str	r3, [r5, #236]
    ba7c:	e598304c 	ldr	r3, [r8, #76]
    ba80:	e58530f0 	str	r3, [r5, #240]
    ba84:	e5983050 	ldr	r3, [r8, #80]
    ba88:	e58520f8 	str	r2, [r5, #248]
    ba8c:	e58530f4 	str	r3, [r5, #244]
    ba90:	e5983054 	ldr	r3, [r8, #84]
    ba94:	e48040b0 	str	r4, [r0], #176
    ba98:	e313000c 	tst	r3, #12	; 0xc
    ba9c:	13a03010 	movne	r3, #16	; 0x10
    baa0:	03a03008 	moveq	r3, #8	; 0x8
    baa4:	e5c530b6 	strb	r3, [r5, #182]
    baa8:	e1d833b2 	ldrh	r3, [r8, #50]
    baac:	e58530d0 	str	r3, [r5, #208]
    bab0:	ebfff8c4 	bl	9dc8 <print_nand_cfg>
    bab4:	e59500c4 	ldr	r0, [r5, #196]
    bab8:	ebfff8ac 	bl	9d70 <generic_ffs>
    babc:	e2400001 	sub	r0, r0, #1	; 0x1
    bac0:	e5850010 	str	r0, [r5, #16]
    bac4:	ebffd2dd 	bl	640 <boot_device>
    bac8:	e3500000 	cmp	r0, #0	; 0x0
    bacc:	1a000005 	bne	bae8 <nand_rescan_config+0x3a8>
    bad0:	e5951028 	ldr	r1, [r5, #40]
    bad4:	e59500c4 	ldr	r0, [r5, #196]
    bad8:	eb00232b 	bl	1478c <__aeabi_uidiv>
    badc:	ebfff8a3 	bl	9d70 <generic_ffs>
    bae0:	e2400001 	sub	r0, r0, #1	; 0x1
    bae4:	e5850008 	str	r0, [r5, #8]
    bae8:	e59f20b8 	ldr	r2, [pc, #184]	; bba8 <nand_rescan_config+0x468>
    baec:	e5d230b6 	ldrb	r3, [r2, #182]
    baf0:	e3530008 	cmp	r3, #8	; 0x8
    baf4:	1a000006 	bne	bb14 <nand_rescan_config+0x3d4>
    baf8:	e5d230b0 	ldrb	r3, [r2, #176]
    bafc:	e59f20ac 	ldr	r2, [pc, #172]	; bbb0 <nand_rescan_config+0x470>
    bb00:	e3530000 	cmp	r3, #0	; 0x0
    bb04:	03003c83 	movweq	r3, #3203	; 0xc83
    bb08:	13003c8b 	movwne	r3, #3211	; 0xc8b
    bb0c:	e1c230b0 	strh	r3, [r2]
    bb10:	ea000002 	b	bb20 <nand_rescan_config+0x3e0>
    bb14:	e59f3094 	ldr	r3, [pc, #148]	; bbb0 <nand_rescan_config+0x470>
    bb18:	e3002c93 	movw	r2, #3219	; 0xc93
    bb1c:	e1c320b0 	strh	r2, [r3]
    bb20:	e59f3080 	ldr	r3, [pc, #128]	; bba8 <nand_rescan_config+0x468>
    bb24:	e59320cc 	ldr	r2, [r3, #204]
    bb28:	e5931008 	ldr	r1, [r3, #8]
    bb2c:	e28300b0 	add	r0, r3, #176	; 0xb0
    bb30:	e1a02132 	lsr	r2, r2, r1
    bb34:	e5832030 	str	r2, [r3, #48]
    bb38:	ebfffe2d 	bl	b3f4 <nand_init_randomizer>
    bb3c:	ea000004 	b	bb54 <nand_rescan_config+0x414>
    bb40:	e5973000 	ldr	r3, [r7]
    bb44:	e59f0068 	ldr	r0, [pc, #104]	; bbb4 <nand_rescan_config+0x474>
    bb48:	e1a08007 	mov	r8, r7
    bb4c:	e1530000 	cmp	r3, r0
    bb50:	0affff0c 	beq	b788 <nand_rescan_config+0x48>
    bb54:	e5982000 	ldr	r2, [r8]
    bb58:	e59f3054 	ldr	r3, [pc, #84]	; bbb4 <nand_rescan_config+0x474>
    bb5c:	e1520003 	cmp	r2, r3
    bb60:	0a000005 	beq	bb7c <nand_rescan_config+0x43c>
    bb64:	e59f004c 	ldr	r0, [pc, #76]	; bbb8 <nand_rescan_config+0x478>
    bb68:	e59f3018 	ldr	r3, [pc, #24]	; bb88 <nand_rescan_config+0x448>
    bb6c:	e1a0e00f 	mov	lr, pc
    bb70:	e593f000 	ldr	pc, [r3]
    bb74:	e3e00000 	mvn	r0, #0	; 0x0
    bb78:	ea000000 	b	bb80 <nand_rescan_config+0x440>
    bb7c:	e3a00000 	mov	r0, #0	; 0x0
    bb80:	e28dd024 	add	sp, sp, #36	; 0x24
    bb84:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    bb88:	3c00d6cc 	.word	0x3c00d6cc
    bb8c:	00018469 	.word	0x00018469
    bb90:	0001627f 	.word	0x0001627f
    bb94:	3c00d748 	.word	0x3c00d748
    bb98:	000184f2 	.word	0x000184f2
    bb9c:	0001847d 	.word	0x0001847d
    bba0:	000184ae 	.word	0x000184ae
    bba4:	000184ca 	.word	0x000184ca
    bba8:	3c00d734 	.word	0x3c00d734
    bbac:	000184e7 	.word	0x000184e7
    bbb0:	3c00d1d0 	.word	0x3c00d1d0
    bbb4:	aca4570e 	.word	0xaca4570e
    bbb8:	000184fe 	.word	0x000184fe

0000bbbc <bnd_vs_reset>:
    bbbc:	e92d4010 	push	{r4, lr}
    bbc0:	e3a00000 	mov	r0, #0	; 0x0
    bbc4:	ebfffedd 	bl	b740 <nand_rescan_config>
    bbc8:	e3a00000 	mov	r0, #0	; 0x0
    bbcc:	ebfff8d8 	bl	9f34 <nand_get_config>
    bbd0:	e1a04000 	mov	r4, r0
    bbd4:	e3a00001 	mov	r0, #1	; 0x1
    bbd8:	ebfff8d5 	bl	9f34 <nand_get_config>
    bbdc:	e3540000 	cmp	r4, #0	; 0x0
    bbe0:	03e04000 	mvneq	r4, #0	; 0x0
    bbe4:	0a00000a 	beq	bc14 <bnd_vs_reset+0x58>
    bbe8:	e3500000 	cmp	r0, #0	; 0x0
    bbec:	01a00004 	moveq	r0, r4
    bbf0:	e3a01000 	mov	r1, #0	; 0x0
    bbf4:	eb0006d9 	bl	d760 <nf2_hardware_reset>
    bbf8:	e2504000 	subs	r4, r0, #0	; 0x0
    bbfc:	0a000004 	beq	bc14 <bnd_vs_reset+0x58>
    bc00:	e59f0014 	ldr	r0, [pc, #20]	; bc1c <bnd_vs_reset+0x60>
    bc04:	e1a01004 	mov	r1, r4
    bc08:	e59f3010 	ldr	r3, [pc, #16]	; bc20 <bnd_vs_reset+0x64>
    bc0c:	e1a0e00f 	mov	lr, pc
    bc10:	e593f000 	ldr	pc, [r3]
    bc14:	e1a00004 	mov	r0, r4
    bc18:	e8bd8010 	pop	{r4, pc}
    bc1c:	00018519 	.word	0x00018519
    bc20:	3c00d6cc 	.word	0x3c00d6cc

0000bc24 <ecc_add_stat>:
    bc24:	e92d4070 	push	{r4, r5, r6, lr}
    bc28:	e59f3134 	ldr	r3, [pc, #308]	; bd64 <ecc_add_stat+0x140>
    bc2c:	e24dd030 	sub	sp, sp, #48	; 0x30
    bc30:	e28d4004 	add	r4, sp, #4	; 0x4
    bc34:	e1a0c003 	mov	ip, r3
    bc38:	e1a06000 	mov	r6, r0
    bc3c:	e1a05001 	mov	r5, r1
    bc40:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    bc44:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    bc48:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    bc4c:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    bc50:	e59c1000 	ldr	r1, [ip]
    bc54:	e59f210c 	ldr	r2, [pc, #268]	; bd68 <ecc_add_stat+0x144>
    bc58:	e28d3028 	add	r3, sp, #40	; 0x28
    bc5c:	e3560000 	cmp	r6, #0	; 0x0
    bc60:	e5841000 	str	r1, [r4]
    bc64:	e8920003 	ldm	r2, {r0, r1}
    bc68:	e8830003 	stm	r3, {r0, r1}
    bc6c:	0a00001b 	beq	bce0 <ecc_add_stat+0xbc>
    bc70:	e3550000 	cmp	r5, #0	; 0x0
    bc74:	b59f20f0 	ldrlt	r2, [pc, #240]	; bd6c <ecc_add_stat+0x148>
    bc78:	b5923008 	ldrlt	r3, [r2, #8]
    bc7c:	b2833001 	addlt	r3, r3, #1	; 0x1
    bc80:	b5823008 	strlt	r3, [r2, #8]
    bc84:	ba000034 	blt	bd5c <ecc_add_stat+0x138>
    bc88:	059f20dc 	ldreq	r2, [pc, #220]	; bd6c <ecc_add_stat+0x148>
    bc8c:	0592300c 	ldreq	r3, [r2, #12]
    bc90:	02833001 	addeq	r3, r3, #1	; 0x1
    bc94:	0582300c 	streq	r3, [r2, #12]
    bc98:	0a00002f 	beq	bd5c <ecc_add_stat+0x138>
    bc9c:	e3550008 	cmp	r5, #8	; 0x8
    bca0:	c1a01005 	movgt	r1, r5
    bca4:	c59f00c4 	ldrgt	r0, [pc, #196]	; bd70 <ecc_add_stat+0x14c>
    bca8:	ca00001d 	bgt	bd24 <ecc_add_stat+0x100>
    bcac:	e59d3028 	ldr	r3, [sp, #40]
    bcb0:	e1550003 	cmp	r5, r3
    bcb4:	d3a01000 	movle	r1, #0	; 0x0
    bcb8:	da000003 	ble	bccc <ecc_add_stat+0xa8>
    bcbc:	e59d302c 	ldr	r3, [sp, #44]
    bcc0:	e1550003 	cmp	r5, r3
    bcc4:	d3a01001 	movle	r1, #1	; 0x1
    bcc8:	c3a01002 	movgt	r1, #2	; 0x2
    bccc:	e59f2098 	ldr	r2, [pc, #152]	; bd6c <ecc_add_stat+0x148>
    bcd0:	e7923101 	ldr	r3, [r2, r1, lsl #2]
    bcd4:	e2833001 	add	r3, r3, #1	; 0x1
    bcd8:	e7823101 	str	r3, [r2, r1, lsl #2]
    bcdc:	ea00001e 	b	bd5c <ecc_add_stat+0x138>
    bce0:	e3550000 	cmp	r5, #0	; 0x0
    bce4:	b59f2088 	ldrlt	r2, [pc, #136]	; bd74 <ecc_add_stat+0x150>
    bce8:	b5923024 	ldrlt	r3, [r2, #36]
    bcec:	b2833001 	addlt	r3, r3, #1	; 0x1
    bcf0:	b5823024 	strlt	r3, [r2, #36]
    bcf4:	ba000018 	blt	bd5c <ecc_add_stat+0x138>
    bcf8:	059f2074 	ldreq	r2, [pc, #116]	; bd74 <ecc_add_stat+0x150>
    bcfc:	05923028 	ldreq	r3, [r2, #40]
    bd00:	02833001 	addeq	r3, r3, #1	; 0x1
    bd04:	05823028 	streq	r3, [r2, #40]
    bd08:	0a000013 	beq	bd5c <ecc_add_stat+0x138>
    bd0c:	e355007f 	cmp	r5, #127	; 0x7f
    bd10:	d1a00006 	movle	r0, r6
    bd14:	d28d2004 	addle	r2, sp, #4	; 0x4
    bd18:	da000005 	ble	bd34 <ecc_add_stat+0x110>
    bd1c:	e59f0054 	ldr	r0, [pc, #84]	; bd78 <ecc_add_stat+0x154>
    bd20:	e1a01005 	mov	r1, r5
    bd24:	e59f3050 	ldr	r3, [pc, #80]	; bd7c <ecc_add_stat+0x158>
    bd28:	e1a0e00f 	mov	lr, pc
    bd2c:	e593f000 	ldr	pc, [r3]
    bd30:	ea000009 	b	bd5c <ecc_add_stat+0x138>
    bd34:	e7923100 	ldr	r3, [r2, r0, lsl #2]
    bd38:	e1550003 	cmp	r5, r3
    bd3c:	da000002 	ble	bd4c <ecc_add_stat+0x128>
    bd40:	e2800001 	add	r0, r0, #1	; 0x1
    bd44:	e3500009 	cmp	r0, #9	; 0x9
    bd48:	1afffff9 	bne	bd34 <ecc_add_stat+0x110>
    bd4c:	e59f2020 	ldr	r2, [pc, #32]	; bd74 <ecc_add_stat+0x150>
    bd50:	e7923100 	ldr	r3, [r2, r0, lsl #2]
    bd54:	e2833001 	add	r3, r3, #1	; 0x1
    bd58:	e7823100 	str	r3, [r2, r0, lsl #2]
    bd5c:	e28dd030 	add	sp, sp, #48	; 0x30
    bd60:	e8bd8070 	pop	{r4, r5, r6, pc}
    bd64:	00015d1c 	.word	0x00015d1c
    bd68:	00015d14 	.word	0x00015d14
    bd6c:	3c00d204 	.word	0x3c00d204
    bd70:	00018539 	.word	0x00018539
    bd74:	3c00d1d4 	.word	0x3c00d1d4
    bd78:	00018564 	.word	0x00018564
    bd7c:	3c00d6cc 	.word	0x3c00d6cc

0000bd80 <ecc_print_stat>:
    bd80:	e92d4070 	push	{r4, r5, r6, lr}
    bd84:	e59f1204 	ldr	r1, [pc, #516]	; bf90 <ecc_print_stat+0x210>
    bd88:	e3a06000 	mov	r6, #0	; 0x0
    bd8c:	e1a02006 	mov	r2, r6
    bd90:	e7913002 	ldr	r3, [r1, r2]
    bd94:	e2822004 	add	r2, r2, #4	; 0x4
    bd98:	e352002c 	cmp	r2, #44	; 0x2c
    bd9c:	e0866003 	add	r6, r6, r3
    bda0:	e59f41e8 	ldr	r4, [pc, #488]	; bf90 <ecc_print_stat+0x210>
    bda4:	1afffff9 	bne	bd90 <ecc_print_stat+0x10>
    bda8:	e3560063 	cmp	r6, #99	; 0x63
    bdac:	e59f51e0 	ldr	r5, [pc, #480]	; bf94 <ecc_print_stat+0x214>
    bdb0:	ca000003 	bgt	bdc4 <ecc_print_stat+0x44>
    bdb4:	e59f01dc 	ldr	r0, [pc, #476]	; bf98 <ecc_print_stat+0x218>
    bdb8:	e1a0e00f 	mov	lr, pc
    bdbc:	e595f000 	ldr	pc, [r5]
    bdc0:	e8bd8070 	pop	{r4, r5, r6, pc}
    bdc4:	e59f01d0 	ldr	r0, [pc, #464]	; bf9c <ecc_print_stat+0x21c>
    bdc8:	e1a0e00f 	mov	lr, pc
    bdcc:	e595f000 	ldr	pc, [r5]
    bdd0:	e59f01c8 	ldr	r0, [pc, #456]	; bfa0 <ecc_print_stat+0x220>
    bdd4:	e1a0e00f 	mov	lr, pc
    bdd8:	e595f000 	ldr	pc, [r5]
    bddc:	e5941028 	ldr	r1, [r4, #40]
    bde0:	e59f01bc 	ldr	r0, [pc, #444]	; bfa4 <ecc_print_stat+0x224>
    bde4:	e1a0e00f 	mov	lr, pc
    bde8:	e595f000 	ldr	pc, [r5]
    bdec:	e5941000 	ldr	r1, [r4]
    bdf0:	e59f01b0 	ldr	r0, [pc, #432]	; bfa8 <ecc_print_stat+0x228>
    bdf4:	e1a0e00f 	mov	lr, pc
    bdf8:	e595f000 	ldr	pc, [r5]
    bdfc:	e5941004 	ldr	r1, [r4, #4]
    be00:	e59f01a4 	ldr	r0, [pc, #420]	; bfac <ecc_print_stat+0x22c>
    be04:	e1a0e00f 	mov	lr, pc
    be08:	e595f000 	ldr	pc, [r5]
    be0c:	e5941008 	ldr	r1, [r4, #8]
    be10:	e59f0198 	ldr	r0, [pc, #408]	; bfb0 <ecc_print_stat+0x230>
    be14:	e1a0e00f 	mov	lr, pc
    be18:	e595f000 	ldr	pc, [r5]
    be1c:	e594100c 	ldr	r1, [r4, #12]
    be20:	e59f018c 	ldr	r0, [pc, #396]	; bfb4 <ecc_print_stat+0x234>
    be24:	e1a0e00f 	mov	lr, pc
    be28:	e595f000 	ldr	pc, [r5]
    be2c:	e5941010 	ldr	r1, [r4, #16]
    be30:	e59f0180 	ldr	r0, [pc, #384]	; bfb8 <ecc_print_stat+0x238>
    be34:	e1a0e00f 	mov	lr, pc
    be38:	e595f000 	ldr	pc, [r5]
    be3c:	e5941014 	ldr	r1, [r4, #20]
    be40:	e59f0174 	ldr	r0, [pc, #372]	; bfbc <ecc_print_stat+0x23c>
    be44:	e1a0e00f 	mov	lr, pc
    be48:	e595f000 	ldr	pc, [r5]
    be4c:	e5941018 	ldr	r1, [r4, #24]
    be50:	e59f0168 	ldr	r0, [pc, #360]	; bfc0 <ecc_print_stat+0x240>
    be54:	e1a0e00f 	mov	lr, pc
    be58:	e595f000 	ldr	pc, [r5]
    be5c:	e594101c 	ldr	r1, [r4, #28]
    be60:	e59f015c 	ldr	r0, [pc, #348]	; bfc4 <ecc_print_stat+0x244>
    be64:	e1a0e00f 	mov	lr, pc
    be68:	e595f000 	ldr	pc, [r5]
    be6c:	e5941020 	ldr	r1, [r4, #32]
    be70:	e59f0150 	ldr	r0, [pc, #336]	; bfc8 <ecc_print_stat+0x248>
    be74:	e1a0e00f 	mov	lr, pc
    be78:	e595f000 	ldr	pc, [r5]
    be7c:	e5941024 	ldr	r1, [r4, #36]
    be80:	e59f0144 	ldr	r0, [pc, #324]	; bfcc <ecc_print_stat+0x24c>
    be84:	e1a0e00f 	mov	lr, pc
    be88:	e595f000 	ldr	pc, [r5]
    be8c:	e59f010c 	ldr	r0, [pc, #268]	; bfa0 <ecc_print_stat+0x220>
    be90:	e1a0e00f 	mov	lr, pc
    be94:	e595f000 	ldr	pc, [r5]
    be98:	e1a00006 	mov	r0, r6
    be9c:	e3a01064 	mov	r1, #100	; 0x64
    bea0:	eb0022bb 	bl	14994 <__aeabi_idiv>
    bea4:	e5944028 	ldr	r4, [r4, #40]
    bea8:	e1a01000 	mov	r1, r0
    beac:	e0644006 	rsb	r4, r4, r6
    beb0:	e1a00004 	mov	r0, r4
    beb4:	eb0022b6 	bl	14994 <__aeabi_idiv>
    beb8:	e59f4110 	ldr	r4, [pc, #272]	; bfd0 <ecc_print_stat+0x250>
    bebc:	e1a02546 	asr	r2, r6, #10
    bec0:	e1a03000 	mov	r3, r0
    bec4:	e1a01006 	mov	r1, r6
    bec8:	e59f0104 	ldr	r0, [pc, #260]	; bfd4 <ecc_print_stat+0x254>
    becc:	e1a0e00f 	mov	lr, pc
    bed0:	e595f000 	ldr	pc, [r5]
    bed4:	e894000c 	ldm	r4, {r2, r3}
    bed8:	e0833002 	add	r3, r3, r2
    bedc:	e5942008 	ldr	r2, [r4, #8]
    bee0:	e594100c 	ldr	r1, [r4, #12]
    bee4:	e0833002 	add	r3, r3, r2
    bee8:	e0836001 	add	r6, r3, r1
    beec:	e3560063 	cmp	r6, #99	; 0x63
    bef0:	d8bd8070 	pople	{r4, r5, r6, pc}
    bef4:	e59f00dc 	ldr	r0, [pc, #220]	; bfd8 <ecc_print_stat+0x258>
    bef8:	e1a0e00f 	mov	lr, pc
    befc:	e595f000 	ldr	pc, [r5]
    bf00:	e59f0098 	ldr	r0, [pc, #152]	; bfa0 <ecc_print_stat+0x220>
    bf04:	e1a0e00f 	mov	lr, pc
    bf08:	e595f000 	ldr	pc, [r5]
    bf0c:	e594100c 	ldr	r1, [r4, #12]
    bf10:	e59f008c 	ldr	r0, [pc, #140]	; bfa4 <ecc_print_stat+0x224>
    bf14:	e1a0e00f 	mov	lr, pc
    bf18:	e595f000 	ldr	pc, [r5]
    bf1c:	e5941000 	ldr	r1, [r4]
    bf20:	e59f00b4 	ldr	r0, [pc, #180]	; bfdc <ecc_print_stat+0x25c>
    bf24:	e1a0e00f 	mov	lr, pc
    bf28:	e595f000 	ldr	pc, [r5]
    bf2c:	e5941004 	ldr	r1, [r4, #4]
    bf30:	e59f0078 	ldr	r0, [pc, #120]	; bfb0 <ecc_print_stat+0x230>
    bf34:	e1a0e00f 	mov	lr, pc
    bf38:	e595f000 	ldr	pc, [r5]
    bf3c:	e5941008 	ldr	r1, [r4, #8]
    bf40:	e59f0084 	ldr	r0, [pc, #132]	; bfcc <ecc_print_stat+0x24c>
    bf44:	e1a0e00f 	mov	lr, pc
    bf48:	e595f000 	ldr	pc, [r5]
    bf4c:	e59f004c 	ldr	r0, [pc, #76]	; bfa0 <ecc_print_stat+0x220>
    bf50:	e1a0e00f 	mov	lr, pc
    bf54:	e595f000 	ldr	pc, [r5]
    bf58:	e1a00006 	mov	r0, r6
    bf5c:	e3a01064 	mov	r1, #100	; 0x64
    bf60:	eb00228b 	bl	14994 <__aeabi_idiv>
    bf64:	e594400c 	ldr	r4, [r4, #12]
    bf68:	e1a01000 	mov	r1, r0
    bf6c:	e0644006 	rsb	r4, r4, r6
    bf70:	e1a00004 	mov	r0, r4
    bf74:	eb002286 	bl	14994 <__aeabi_idiv>
    bf78:	e1a01006 	mov	r1, r6
    bf7c:	e1a02000 	mov	r2, r0
    bf80:	e59f0058 	ldr	r0, [pc, #88]	; bfe0 <ecc_print_stat+0x260>
    bf84:	e1a0e00f 	mov	lr, pc
    bf88:	e595f000 	ldr	pc, [r5]
    bf8c:	e8bd8070 	pop	{r4, r5, r6, pc}
    bf90:	3c00d1d4 	.word	0x3c00d1d4
    bf94:	3c00d6cc 	.word	0x3c00d6cc
    bf98:	0001858f 	.word	0x0001858f
    bf9c:	000185a3 	.word	0x000185a3
    bfa0:	000185bc 	.word	0x000185bc
    bfa4:	000185da 	.word	0x000185da
    bfa8:	000185e9 	.word	0x000185e9
    bfac:	000185f8 	.word	0x000185f8
    bfb0:	00018607 	.word	0x00018607
    bfb4:	00018616 	.word	0x00018616
    bfb8:	00018625 	.word	0x00018625
    bfbc:	00018634 	.word	0x00018634
    bfc0:	00018643 	.word	0x00018643
    bfc4:	00018652 	.word	0x00018652
    bfc8:	00018661 	.word	0x00018661
    bfcc:	00018670 	.word	0x00018670
    bfd0:	3c00d204 	.word	0x3c00d204
    bfd4:	0001867f 	.word	0x0001867f
    bfd8:	000186b3 	.word	0x000186b3
    bfdc:	000186cd 	.word	0x000186cd
    bfe0:	000186dc 	.word	0x000186dc

0000bfe4 <nf2_set_busw>:
    bfe4:	e3500010 	cmp	r0, #16	; 0x10
    bfe8:	03a02001 	moveq	r2, #1	; 0x1
    bfec:	13a02000 	movne	r2, #0	; 0x0
    bff0:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    bff4:	e58321b8 	str	r2, [r3, #440]
    bff8:	e3a00000 	mov	r0, #0	; 0x0
    bffc:	e12fff1e 	bx	lr

0000c000 <nf2_ecc_cap>:
    c000:	e350000f 	cmp	r0, #15	; 0xf
    c004:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    c008:	ea000011 	b	c054 <nf2_ecc_cap+0x54>
    c00c:	0000c04c 	.word	0x0000c04c
    c010:	0000c05c 	.word	0x0000c05c
    c014:	0000c064 	.word	0x0000c064
    c018:	0000c06c 	.word	0x0000c06c
    c01c:	0000c074 	.word	0x0000c074
    c020:	0000c07c 	.word	0x0000c07c
    c024:	0000c084 	.word	0x0000c084
    c028:	0000c08c 	.word	0x0000c08c
    c02c:	0000c094 	.word	0x0000c094
    c030:	0000c09c 	.word	0x0000c09c
    c034:	0000c0a4 	.word	0x0000c0a4
    c038:	0000c0ac 	.word	0x0000c0ac
    c03c:	0000c0b4 	.word	0x0000c0b4
    c040:	0000c0bc 	.word	0x0000c0bc
    c044:	0000c0c4 	.word	0x0000c0c4
    c048:	0000c0cc 	.word	0x0000c0cc
    c04c:	e3a00002 	mov	r0, #2	; 0x2
    c050:	e12fff1e 	bx	lr
    c054:	e3a00000 	mov	r0, #0	; 0x0
    c058:	e12fff1e 	bx	lr
    c05c:	e3a00004 	mov	r0, #4	; 0x4
    c060:	e12fff1e 	bx	lr
    c064:	e3a00008 	mov	r0, #8	; 0x8
    c068:	e12fff1e 	bx	lr
    c06c:	e3a00010 	mov	r0, #16	; 0x10
    c070:	e12fff1e 	bx	lr
    c074:	e3a00018 	mov	r0, #24	; 0x18
    c078:	e12fff1e 	bx	lr
    c07c:	e3a00020 	mov	r0, #32	; 0x20
    c080:	e12fff1e 	bx	lr
    c084:	e3a00028 	mov	r0, #40	; 0x28
    c088:	e12fff1e 	bx	lr
    c08c:	e3a00030 	mov	r0, #48	; 0x30
    c090:	e12fff1e 	bx	lr
    c094:	e3a00038 	mov	r0, #56	; 0x38
    c098:	e12fff1e 	bx	lr
    c09c:	e3a0003c 	mov	r0, #60	; 0x3c
    c0a0:	e12fff1e 	bx	lr
    c0a4:	e3a00040 	mov	r0, #64	; 0x40
    c0a8:	e12fff1e 	bx	lr
    c0ac:	e3a00048 	mov	r0, #72	; 0x48
    c0b0:	e12fff1e 	bx	lr
    c0b4:	e3a00050 	mov	r0, #80	; 0x50
    c0b8:	e12fff1e 	bx	lr
    c0bc:	e3a00060 	mov	r0, #96	; 0x60
    c0c0:	e12fff1e 	bx	lr
    c0c4:	e3a00070 	mov	r0, #112	; 0x70
    c0c8:	e12fff1e 	bx	lr
    c0cc:	e3a0007f 	mov	r0, #127	; 0x7f
    c0d0:	e12fff1e 	bx	lr

0000c0d4 <trx_afifo_reg>:
    c0d4:	e2503000 	subs	r3, r0, #0	; 0x0
    c0d8:	e1a00002 	mov	r0, r2
    c0dc:	1a000008 	bne	c104 <trx_afifo_reg+0x30>
    c0e0:	e1a02802 	lsl	r2, r2, #16
    c0e4:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c0e8:	e3811a0f 	orr	r1, r1, #61440	; 0xf000
    c0ec:	e1a02822 	lsr	r2, r2, #16
    c0f0:	e1a00820 	lsr	r0, r0, #16
    c0f4:	e5831060 	str	r1, [r3, #96]
    c0f8:	e5832060 	str	r2, [r3, #96]
    c0fc:	e5830060 	str	r0, [r3, #96]
    c100:	e12fff1e 	bx	lr
    c104:	e3530001 	cmp	r3, #1	; 0x1
    c108:	01a03802 	lsleq	r3, r2, #16
    c10c:	11813403 	orrne	r3, r1, r3, lsl #8
    c110:	03a0230a 	moveq	r2, #671088640	; 0x28000000
    c114:	03811cf1 	orreq	r1, r1, #61696	; 0xf100
    c118:	01a03823 	lsreq	r3, r3, #16
    c11c:	13833a0f 	orrne	r3, r3, #61440	; 0xf000
    c120:	13a0230a 	movne	r2, #671088640	; 0x28000000
    c124:	05821060 	streq	r1, [r2, #96]
    c128:	e5823060 	str	r3, [r2, #96]
    c12c:	e12fff1e 	bx	lr

0000c130 <trx_afifo_enable>:
    c130:	e24dd008 	sub	sp, sp, #8	; 0x8
    c134:	e3a03000 	mov	r3, #0	; 0x0
    c138:	e58d3004 	str	r3, [sp, #4]
    c13c:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c140:	e5923054 	ldr	r3, [r2, #84]
    c144:	e58d3004 	str	r3, [sp, #4]
    c148:	e59d3004 	ldr	r3, [sp, #4]
    c14c:	e3833001 	orr	r3, r3, #1	; 0x1
    c150:	e58d3004 	str	r3, [sp, #4]
    c154:	e59d3004 	ldr	r3, [sp, #4]
    c158:	e5823054 	str	r3, [r2, #84]
    c15c:	e28dd008 	add	sp, sp, #8	; 0x8
    c160:	e12fff1e 	bx	lr

0000c164 <trx_afifo_disable>:
    c164:	e3a02000 	mov	r2, #0	; 0x0
    c168:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c16c:	e5832054 	str	r2, [r3, #84]
    c170:	e12fff1e 	bx	lr

0000c174 <trx_nand_cs>:
    c174:	e3500001 	cmp	r0, #1	; 0x1
    c178:	03e02001 	mvneq	r2, #1	; 0x1
    c17c:	13a0200f 	movne	r2, #15	; 0xf
    c180:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c184:	e583204c 	str	r2, [r3, #76]
    c188:	e12fff1e 	bx	lr

0000c18c <trx_afifo_start>:
    c18c:	e3a02001 	mov	r2, #1	; 0x1
    c190:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c194:	e5832084 	str	r2, [r3, #132]
    c198:	e12fff1e 	bx	lr

0000c19c <trx_afifo_is_finish>:
    c19c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c1a0:	e593308c 	ldr	r3, [r3, #140]
    c1a4:	e0000003 	and	r0, r0, r3
    c1a8:	e12fff1e 	bx	lr

0000c1ac <trx_afifo_intr_clear>:
    c1ac:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    c1b0:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c1b4:	e583208c 	str	r2, [r3, #140]
    c1b8:	e12fff1e 	bx	lr

0000c1bc <trx_afifo_read_state>:
    c1bc:	e3500003 	cmp	r0, #3	; 0x3
    c1c0:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    c1c4:	ea000003 	b	c1d8 <trx_afifo_read_state+0x1c>
    c1c8:	0000c1e0 	.word	0x0000c1e0
    c1cc:	0000c1ec 	.word	0x0000c1ec
    c1d0:	0000c1f8 	.word	0x0000c1f8
    c1d4:	0000c204 	.word	0x0000c204
    c1d8:	e3e00000 	mvn	r0, #0	; 0x0
    c1dc:	e12fff1e 	bx	lr
    c1e0:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c1e4:	e5930010 	ldr	r0, [r3, #16]
    c1e8:	e12fff1e 	bx	lr
    c1ec:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c1f0:	e5930014 	ldr	r0, [r3, #20]
    c1f4:	e12fff1e 	bx	lr
    c1f8:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c1fc:	e59300e0 	ldr	r0, [r3, #224]
    c200:	e12fff1e 	bx	lr
    c204:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c208:	e59300e4 	ldr	r0, [r3, #228]
    c20c:	e12fff1e 	bx	lr

0000c210 <trx_afifo_etd>:
    c210:	e1811080 	orr	r1, r1, r0, lsl #1
    c214:	e3811902 	orr	r1, r1, #32768	; 0x8000
    c218:	e1811102 	orr	r1, r1, r2, lsl #2
    c21c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c220:	e5831060 	str	r1, [r3, #96]
    c224:	e12fff1e 	bx	lr

0000c228 <cpu_afifo_disable>:
    c228:	e24dd008 	sub	sp, sp, #8	; 0x8
    c22c:	e3a03000 	mov	r3, #0	; 0x0
    c230:	e58d3004 	str	r3, [sp, #4]
    c234:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c238:	e5923054 	ldr	r3, [r2, #84]
    c23c:	e58d3004 	str	r3, [sp, #4]
    c240:	e59d3004 	ldr	r3, [sp, #4]
    c244:	e3c33002 	bic	r3, r3, #2	; 0x2
    c248:	e58d3004 	str	r3, [sp, #4]
    c24c:	e59d3004 	ldr	r3, [sp, #4]
    c250:	e5823054 	str	r3, [r2, #84]
    c254:	e28dd008 	add	sp, sp, #8	; 0x8
    c258:	e12fff1e 	bx	lr

0000c25c <cpu_afifo_enable>:
    c25c:	e24dd008 	sub	sp, sp, #8	; 0x8
    c260:	e3a03000 	mov	r3, #0	; 0x0
    c264:	e58d3004 	str	r3, [sp, #4]
    c268:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c26c:	e5923054 	ldr	r3, [r2, #84]
    c270:	e58d3004 	str	r3, [sp, #4]
    c274:	e59d3004 	ldr	r3, [sp, #4]
    c278:	e3833002 	orr	r3, r3, #2	; 0x2
    c27c:	e58d3004 	str	r3, [sp, #4]
    c280:	e59d3004 	ldr	r3, [sp, #4]
    c284:	e5823054 	str	r3, [r2, #84]
    c288:	e28dd008 	add	sp, sp, #8	; 0x8
    c28c:	e12fff1e 	bx	lr

0000c290 <trx_afifo_nop>:
    c290:	e1a01b01 	lsl	r1, r1, #22
    c294:	e1a01b21 	lsr	r1, r1, #22
    c298:	e200000f 	and	r0, r0, #15	; 0xf
    c29c:	e1811500 	orr	r1, r1, r0, lsl #10
    c2a0:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c2a4:	e5831060 	str	r1, [r3, #96]
    c2a8:	e12fff1e 	bx	lr

0000c2ac <trx_afifo_ncmd>:
    c2ac:	e200000f 	and	r0, r0, #15	; 0xf
    c2b0:	e1a01401 	lsl	r1, r1, #8
    c2b4:	e1811500 	orr	r1, r1, r0, lsl #10
    c2b8:	e3811901 	orr	r1, r1, #16384	; 0x4000
    c2bc:	e1811002 	orr	r1, r1, r2
    c2c0:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c2c4:	e5831060 	str	r1, [r3, #96]
    c2c8:	e12fff1e 	bx	lr

0000c2cc <nf2_secc_cap>:
    c2cc:	e3500000 	cmp	r0, #0	; 0x0
    c2d0:	0a000003 	beq	c2e4 <nf2_secc_cap+0x18>
    c2d4:	e3500001 	cmp	r0, #1	; 0x1
    c2d8:	03a00008 	moveq	r0, #8	; 0x8
    c2dc:	13a00000 	movne	r0, #0	; 0x0
    c2e0:	e12fff1e 	bx	lr
    c2e4:	e3a00004 	mov	r0, #4	; 0x4
    c2e8:	e12fff1e 	bx	lr

0000c2ec <nf2_secc_cfg>:
    c2ec:	e3a0c30a 	mov	ip, #671088640	; 0x28000000
    c2f0:	e59c3018 	ldr	r3, [ip, #24]
    c2f4:	e24dd008 	sub	sp, sp, #8	; 0x8
    c2f8:	e58d3004 	str	r3, [sp, #4]
    c2fc:	e1a00800 	lsl	r0, r0, #16
    c300:	e59d3004 	ldr	r3, [sp, #4]
    c304:	e1800881 	orr	r0, r0, r1, lsl #17
    c308:	e1800182 	orr	r0, r0, r2, lsl #3
    c30c:	e1800003 	orr	r0, r0, r3
    c310:	e58d0004 	str	r0, [sp, #4]
    c314:	e59d3004 	ldr	r3, [sp, #4]
    c318:	e3510000 	cmp	r1, #0	; 0x0
    c31c:	e58c3018 	str	r3, [ip, #24]
    c320:	03a01004 	moveq	r1, #4	; 0x4
    c324:	0a000002 	beq	c334 <nf2_secc_cfg+0x48>
    c328:	e3510001 	cmp	r1, #1	; 0x1
    c32c:	03a01008 	moveq	r1, #8	; 0x8
    c330:	13a01000 	movne	r1, #0	; 0x0
    c334:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c338:	e592311c 	ldr	r3, [r2, #284]
    c33c:	e58d3004 	str	r3, [sp, #4]
    c340:	e59d3004 	ldr	r3, [sp, #4]
    c344:	e3c3380f 	bic	r3, r3, #983040	; 0xf0000
    c348:	e58d3004 	str	r3, [sp, #4]
    c34c:	e59d3004 	ldr	r3, [sp, #4]
    c350:	e1833801 	orr	r3, r3, r1, lsl #16
    c354:	e58d3004 	str	r3, [sp, #4]
    c358:	e59d3004 	ldr	r3, [sp, #4]
    c35c:	e582311c 	str	r3, [r2, #284]
    c360:	e28dd008 	add	sp, sp, #8	; 0x8
    c364:	e12fff1e 	bx	lr

0000c368 <nf2_ecc_cfg>:
    c368:	e380cc03 	orr	ip, r0, #768	; 0x300
    c36c:	e18cc581 	orr	ip, ip, r1, lsl #11
    c370:	e92d4010 	push	{r4, lr}
    c374:	e18cc202 	orr	ip, ip, r2, lsl #4
    c378:	e18cc603 	orr	ip, ip, r3, lsl #12
    c37c:	e59d3010 	ldr	r3, [sp, #16]
    c380:	e59d100c 	ldr	r1, [sp, #12]
    c384:	e18cc083 	orr	ip, ip, r3, lsl #1
    c388:	e59d3014 	ldr	r3, [sp, #20]
    c38c:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    c390:	e18cc903 	orr	ip, ip, r3, lsl #18
    c394:	e59d3008 	ldr	r3, [sp, #8]
    c398:	e1a00002 	mov	r0, r2
    c39c:	e1811803 	orr	r1, r1, r3, lsl #16
    c3a0:	e584c018 	str	ip, [r4, #24]
    c3a4:	e584101c 	str	r1, [r4, #28]
    c3a8:	ebffff14 	bl	c000 <nf2_ecc_cap>
    c3ac:	e594311c 	ldr	r3, [r4, #284]
    c3b0:	e3c33c7f 	bic	r3, r3, #32512	; 0x7f00
    c3b4:	e1833400 	orr	r3, r3, r0, lsl #8
    c3b8:	e584311c 	str	r3, [r4, #284]
    c3bc:	e8bd8010 	pop	{r4, pc}

0000c3c0 <nf2_ecc_num>:
    c3c0:	e1a01541 	asr	r1, r1, #10
    c3c4:	e350000f 	cmp	r0, #15	; 0xf
    c3c8:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    c3cc:	ea00000f 	b	c410 <nf2_ecc_num+0x50>
    c3d0:	0000c418 	.word	0x0000c418
    c3d4:	0000c420 	.word	0x0000c420
    c3d8:	0000c428 	.word	0x0000c428
    c3dc:	0000c430 	.word	0x0000c430
    c3e0:	0000c438 	.word	0x0000c438
    c3e4:	0000c440 	.word	0x0000c440
    c3e8:	0000c448 	.word	0x0000c448
    c3ec:	0000c450 	.word	0x0000c450
    c3f0:	0000c458 	.word	0x0000c458
    c3f4:	0000c460 	.word	0x0000c460
    c3f8:	0000c468 	.word	0x0000c468
    c3fc:	0000c470 	.word	0x0000c470
    c400:	0000c478 	.word	0x0000c478
    c404:	0000c480 	.word	0x0000c480
    c408:	0000c488 	.word	0x0000c488
    c40c:	0000c490 	.word	0x0000c490
    c410:	e3a00000 	mov	r0, #0	; 0x0
    c414:	ea00001f 	b	c498 <nf2_ecc_num+0xd8>
    c418:	e1a00101 	lsl	r0, r1, #2
    c41c:	ea00001d 	b	c498 <nf2_ecc_num+0xd8>
    c420:	e1a00181 	lsl	r0, r1, #3
    c424:	ea00001b 	b	c498 <nf2_ecc_num+0xd8>
    c428:	e1a00201 	lsl	r0, r1, #4
    c42c:	ea000019 	b	c498 <nf2_ecc_num+0xd8>
    c430:	e3a0301c 	mov	r3, #28	; 0x1c
    c434:	ea000016 	b	c494 <nf2_ecc_num+0xd4>
    c438:	e3a0302c 	mov	r3, #44	; 0x2c
    c43c:	ea000014 	b	c494 <nf2_ecc_num+0xd4>
    c440:	e3a03038 	mov	r3, #56	; 0x38
    c444:	ea000012 	b	c494 <nf2_ecc_num+0xd4>
    c448:	e3a03048 	mov	r3, #72	; 0x48
    c44c:	ea000010 	b	c494 <nf2_ecc_num+0xd4>
    c450:	e3a03054 	mov	r3, #84	; 0x54
    c454:	ea00000e 	b	c494 <nf2_ecc_num+0xd4>
    c458:	e3a03064 	mov	r3, #100	; 0x64
    c45c:	ea00000c 	b	c494 <nf2_ecc_num+0xd4>
    c460:	e3a0306c 	mov	r3, #108	; 0x6c
    c464:	ea00000a 	b	c494 <nf2_ecc_num+0xd4>
    c468:	e3a03070 	mov	r3, #112	; 0x70
    c46c:	ea000008 	b	c494 <nf2_ecc_num+0xd4>
    c470:	e1a00381 	lsl	r0, r1, #7
    c474:	ea000007 	b	c498 <nf2_ecc_num+0xd8>
    c478:	e3a0308c 	mov	r3, #140	; 0x8c
    c47c:	ea000004 	b	c494 <nf2_ecc_num+0xd4>
    c480:	e3a030a8 	mov	r3, #168	; 0xa8
    c484:	ea000002 	b	c494 <nf2_ecc_num+0xd4>
    c488:	e3a030c4 	mov	r3, #196	; 0xc4
    c48c:	ea000000 	b	c494 <nf2_ecc_num+0xd4>
    c490:	e3a030e0 	mov	r3, #224	; 0xe0
    c494:	e0000193 	mul	r0, r3, r1
    c498:	e3520001 	cmp	r2, #1	; 0x1
    c49c:	01a00080 	lsleq	r0, r0, #1
    c4a0:	e12fff1e 	bx	lr

0000c4a4 <nf2_addr_cfg>:
    c4a4:	e3530010 	cmp	r3, #16	; 0x10
    c4a8:	01a010c1 	asreq	r1, r1, #1
    c4ac:	01a020c2 	asreq	r2, r2, #1
    c4b0:	e1813802 	orr	r3, r1, r2, lsl #16
    c4b4:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c4b8:	e3a01000 	mov	r1, #0	; 0x0
    c4bc:	e5820020 	str	r0, [r2, #32]
    c4c0:	e5821024 	str	r1, [r2, #36]
    c4c4:	e5821028 	str	r1, [r2, #40]
    c4c8:	e582102c 	str	r1, [r2, #44]
    c4cc:	e5823030 	str	r3, [r2, #48]
    c4d0:	e12fff1e 	bx	lr

0000c4d4 <nf2_sdma_cfg>:
    c4d4:	e59dc000 	ldr	ip, [sp]
    c4d8:	e1813803 	orr	r3, r1, r3, lsl #16
    c4dc:	e1a0c10c 	lsl	ip, ip, #2
    c4e0:	e3a0130a 	mov	r1, #671088640	; 0x28000000
    c4e4:	e5810038 	str	r0, [r1, #56]
    c4e8:	e38cc001 	orr	ip, ip, #1	; 0x1
    c4ec:	e581203c 	str	r2, [r1, #60]
    c4f0:	e5813040 	str	r3, [r1, #64]
    c4f4:	e3a03341 	mov	r3, #67108865	; 0x4000001
    c4f8:	e5813044 	str	r3, [r1, #68]
    c4fc:	e581c048 	str	ip, [r1, #72]
    c500:	e12fff1e 	bx	lr

0000c504 <nf2_check_ecc>:
    c504:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    c508:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c50c:	e59230e8 	ldr	r3, [r2, #232]
    c510:	e24dd028 	sub	sp, sp, #40	; 0x28
    c514:	e58d3000 	str	r3, [sp]
    c518:	e59230ec 	ldr	r3, [r2, #236]
    c51c:	e3a0c000 	mov	ip, #0	; 0x0
    c520:	e58d3004 	str	r3, [sp, #4]
    c524:	e59230f0 	ldr	r3, [r2, #240]
    c528:	e1a06001 	mov	r6, r1
    c52c:	e58d3008 	str	r3, [sp, #8]
    c530:	e59230f4 	ldr	r3, [r2, #244]
    c534:	e1a0400c 	mov	r4, ip
    c538:	e58d300c 	str	r3, [sp, #12]
    c53c:	e59230f8 	ldr	r3, [r2, #248]
    c540:	e1a08540 	asr	r8, r0, #10
    c544:	e58d3010 	str	r3, [sp, #16]
    c548:	e59230fc 	ldr	r3, [r2, #252]
    c54c:	e1a0500c 	mov	r5, ip
    c550:	e58d3014 	str	r3, [sp, #20]
    c554:	e5923100 	ldr	r3, [r2, #256]
    c558:	e58d3018 	str	r3, [sp, #24]
    c55c:	e5923104 	ldr	r3, [r2, #260]
    c560:	e58d301c 	str	r3, [sp, #28]
    c564:	e5927108 	ldr	r7, [r2, #264]
    c568:	e581c000 	str	ip, [r1]
    c56c:	e58d7020 	str	r7, [sp, #32]
    c570:	ea000028 	b	c618 <nf2_check_ecc+0x114>
    c574:	e1a03457 	asr	r3, r7, r4
    c578:	e3130001 	tst	r3, #1	; 0x1
    c57c:	0a000004 	beq	c594 <nf2_check_ecc+0x90>
    c580:	e3a00000 	mov	r0, #0	; 0x0
    c584:	e3e01000 	mvn	r1, #0	; 0x0
    c588:	ebfffda5 	bl	bc24 <ecc_add_stat>
    c58c:	e3e00000 	mvn	r0, #0	; 0x0
    c590:	ea000024 	b	c628 <nf2_check_ecc+0x124>
    c594:	e1a03144 	asr	r3, r4, #2
    c598:	e3520003 	cmp	r2, #3	; 0x3
    c59c:	979ff102 	ldrls	pc, [pc, r2, lsl #2]
    c5a0:	ea000015 	b	c5fc <nf2_check_ecc+0xf8>
    c5a4:	0000c5b4 	.word	0x0000c5b4
    c5a8:	0000c5c4 	.word	0x0000c5c4
    c5ac:	0000c5dc 	.word	0x0000c5dc
    c5b0:	0000c5ec 	.word	0x0000c5ec
    c5b4:	e28d2028 	add	r2, sp, #40	; 0x28
    c5b8:	e0823103 	add	r3, r2, r3, lsl #2
    c5bc:	e5133028 	ldr	r3, [r3, #-40]
    c5c0:	ea00000c 	b	c5f8 <nf2_check_ecc+0xf4>
    c5c4:	e28d2028 	add	r2, sp, #40	; 0x28
    c5c8:	e0823103 	add	r3, r2, r3, lsl #2
    c5cc:	e5133028 	ldr	r3, [r3, #-40]
    c5d0:	e2033c7f 	and	r3, r3, #32512	; 0x7f00
    c5d4:	e1a05443 	asr	r5, r3, #8
    c5d8:	ea000007 	b	c5fc <nf2_check_ecc+0xf8>
    c5dc:	e28d2028 	add	r2, sp, #40	; 0x28
    c5e0:	e0823103 	add	r3, r2, r3, lsl #2
    c5e4:	e15332b6 	ldrh	r3, [r3, #-38]
    c5e8:	ea000002 	b	c5f8 <nf2_check_ecc+0xf4>
    c5ec:	e28d2028 	add	r2, sp, #40	; 0x28
    c5f0:	e0823103 	add	r3, r2, r3, lsl #2
    c5f4:	e5533025 	ldrb	r3, [r3, #-37]
    c5f8:	e203507f 	and	r5, r3, #127	; 0x7f
    c5fc:	e3a00000 	mov	r0, #0	; 0x0
    c600:	e1a01005 	mov	r1, r5
    c604:	ebfffd86 	bl	bc24 <ecc_add_stat>
    c608:	e5963000 	ldr	r3, [r6]
    c60c:	e2844001 	add	r4, r4, #1	; 0x1
    c610:	e0833005 	add	r3, r3, r5
    c614:	e5863000 	str	r3, [r6]
    c618:	e1540008 	cmp	r4, r8
    c61c:	e2042003 	and	r2, r4, #3	; 0x3
    c620:	baffffd3 	blt	c574 <nf2_check_ecc+0x70>
    c624:	e3a00000 	mov	r0, #0	; 0x0
    c628:	e28dd028 	add	sp, sp, #40	; 0x28
    c62c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000c630 <nf2_check_secc>:
    c630:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    c634:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c638:	e593310c 	ldr	r3, [r3, #268]
    c63c:	e24dd004 	sub	sp, sp, #4	; 0x4
    c640:	e3130040 	tst	r3, #64	; 0x40
    c644:	0a000004 	beq	c65c <nf2_check_secc+0x2c>
    c648:	e3a00001 	mov	r0, #1	; 0x1
    c64c:	e3e01000 	mvn	r1, #0	; 0x0
    c650:	ebfffd73 	bl	bc24 <ecc_add_stat>
    c654:	e3e00000 	mvn	r0, #0	; 0x0
    c658:	ea000006 	b	c678 <nf2_check_secc+0x48>
    c65c:	e3130080 	tst	r3, #128	; 0x80
    c660:	1203300f 	andne	r3, r3, #15	; 0xf
    c664:	15803000 	strne	r3, [r0]
    c668:	e5901000 	ldr	r1, [r0]
    c66c:	e3a00001 	mov	r0, #1	; 0x1
    c670:	ebfffd6b 	bl	bc24 <ecc_add_stat>
    c674:	e3a00000 	mov	r0, #0	; 0x0
    c678:	e28dd004 	add	sp, sp, #4	; 0x4
    c67c:	e8bd8000 	pop	{pc}

0000c680 <nf2_set_polynomial>:
    c680:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c684:	e5830160 	str	r0, [r3, #352]
    c688:	e3a00001 	mov	r0, #1	; 0x1
    c68c:	e12fff1e 	bx	lr

0000c690 <nf2_afifo_reset>:
    c690:	e92d4070 	push	{r4, r5, r6, lr}
    c694:	e3a02000 	mov	r2, #0	; 0x0
    c698:	e24dd008 	sub	sp, sp, #8	; 0x8
    c69c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c6a0:	e58d2004 	str	r2, [sp, #4]
    c6a4:	e2822010 	add	r2, r2, #16	; 0x10
    c6a8:	e5832080 	str	r2, [r3, #128]
    c6ac:	e1a05003 	mov	r5, r3
    c6b0:	eb0004b8 	bl	d998 <get_ticks>
    c6b4:	e59f6034 	ldr	r6, [pc, #52]	; c6f0 <nf2_afifo_reset+0x60>
    c6b8:	e1a04000 	mov	r4, r0
    c6bc:	eb0004b5 	bl	d998 <get_ticks>
    c6c0:	e0640000 	rsb	r0, r4, r0
    c6c4:	e1500006 	cmp	r0, r6
    c6c8:	c3e00000 	mvngt	r0, #0	; 0x0
    c6cc:	ca000005 	bgt	c6e8 <nf2_afifo_reset+0x58>
    c6d0:	e5953080 	ldr	r3, [r5, #128]
    c6d4:	e203301f 	and	r3, r3, #31	; 0x1f
    c6d8:	e58d3004 	str	r3, [sp, #4]
    c6dc:	e59d0004 	ldr	r0, [sp, #4]
    c6e0:	e3500000 	cmp	r0, #0	; 0x0
    c6e4:	1afffff4 	bne	c6bc <nf2_afifo_reset+0x2c>
    c6e8:	e28dd008 	add	sp, sp, #8	; 0x8
    c6ec:	e8bd8070 	pop	{r4, r5, r6, pc}
    c6f0:	0007a120 	.word	0x0007a120

0000c6f4 <nf2_soft_reset>:
    c6f4:	e92d4070 	push	{r4, r5, r6, lr}
    c6f8:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c6fc:	e24dd008 	sub	sp, sp, #8	; 0x8
    c700:	e3a03000 	mov	r3, #0	; 0x0
    c704:	e58d3004 	str	r3, [sp, #4]
    c708:	e5820080 	str	r0, [r2, #128]
    c70c:	e1a05002 	mov	r5, r2
    c710:	eb0004a0 	bl	d998 <get_ticks>
    c714:	e59f6034 	ldr	r6, [pc, #52]	; c750 <nf2_soft_reset+0x5c>
    c718:	e1a04000 	mov	r4, r0
    c71c:	eb00049d 	bl	d998 <get_ticks>
    c720:	e0640000 	rsb	r0, r4, r0
    c724:	e1500006 	cmp	r0, r6
    c728:	c3e00000 	mvngt	r0, #0	; 0x0
    c72c:	ca000005 	bgt	c748 <nf2_soft_reset+0x54>
    c730:	e5953080 	ldr	r3, [r5, #128]
    c734:	e203301f 	and	r3, r3, #31	; 0x1f
    c738:	e58d3004 	str	r3, [sp, #4]
    c73c:	e59d0004 	ldr	r0, [sp, #4]
    c740:	e3500000 	cmp	r0, #0	; 0x0
    c744:	1afffff4 	bne	c71c <nf2_soft_reset+0x28>
    c748:	e28dd008 	add	sp, sp, #8	; 0x8
    c74c:	e8bd8070 	pop	{r4, r5, r6, pc}
    c750:	0007a120 	.word	0x0007a120

0000c754 <nf2_check_irq>:
    c754:	e92d4030 	push	{r4, r5, lr}
    c758:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    c75c:	e594208c 	ldr	r2, [r4, #140]
    c760:	e5943090 	ldr	r3, [r4, #144]
    c764:	e24dd004 	sub	sp, sp, #4	; 0x4
    c768:	e0035002 	and	r5, r3, r2
    c76c:	e3150701 	tst	r5, #262144	; 0x40000
    c770:	0a000006 	beq	c790 <nf2_check_irq+0x3c>
    c774:	e5943090 	ldr	r3, [r4, #144]
    c778:	e3c55701 	bic	r5, r5, #262144	; 0x40000
    c77c:	e3c33701 	bic	r3, r3, #262144	; 0x40000
    c780:	e5843090 	str	r3, [r4, #144]
    c784:	e3a03701 	mov	r3, #262144	; 0x40000
    c788:	e584308c 	str	r3, [r4, #140]
    c78c:	ea000072 	b	c95c <nf2_check_irq+0x208>
    c790:	e3150802 	tst	r5, #131072	; 0x20000
    c794:	0a000006 	beq	c7b4 <nf2_check_irq+0x60>
    c798:	e5943090 	ldr	r3, [r4, #144]
    c79c:	e3c55802 	bic	r5, r5, #131072	; 0x20000
    c7a0:	e3c33802 	bic	r3, r3, #131072	; 0x20000
    c7a4:	e5843090 	str	r3, [r4, #144]
    c7a8:	e3a03802 	mov	r3, #131072	; 0x20000
    c7ac:	e584308c 	str	r3, [r4, #140]
    c7b0:	ea000069 	b	c95c <nf2_check_irq+0x208>
    c7b4:	e59f21ac 	ldr	r2, [pc, #428]	; c968 <nf2_check_irq+0x214>
    c7b8:	e59f31a8 	ldr	r3, [pc, #424]	; c968 <nf2_check_irq+0x214>
    c7bc:	e0052002 	and	r2, r5, r2
    c7c0:	e1520003 	cmp	r2, r3
    c7c4:	1a000006 	bne	c7e4 <nf2_check_irq+0x90>
    c7c8:	e5943090 	ldr	r3, [r4, #144]
    c7cc:	e3c55702 	bic	r5, r5, #524288	; 0x80000
    c7d0:	e3c33702 	bic	r3, r3, #524288	; 0x80000
    c7d4:	e3c33b01 	bic	r3, r3, #1024	; 0x400
    c7d8:	e5843090 	str	r3, [r4, #144]
    c7dc:	e584208c 	str	r2, [r4, #140]
    c7e0:	ea000006 	b	c800 <nf2_check_irq+0xac>
    c7e4:	e3150b01 	tst	r5, #1024	; 0x400
    c7e8:	0a000006 	beq	c808 <nf2_check_irq+0xb4>
    c7ec:	e5943090 	ldr	r3, [r4, #144]
    c7f0:	e3c33b01 	bic	r3, r3, #1024	; 0x400
    c7f4:	e5843090 	str	r3, [r4, #144]
    c7f8:	e3a03b01 	mov	r3, #1024	; 0x400
    c7fc:	e584308c 	str	r3, [r4, #140]
    c800:	e3c55b01 	bic	r5, r5, #1024	; 0x400
    c804:	ea000054 	b	c95c <nf2_check_irq+0x208>
    c808:	e3150a01 	tst	r5, #4096	; 0x1000
    c80c:	0a000006 	beq	c82c <nf2_check_irq+0xd8>
    c810:	e59f3154 	ldr	r3, [pc, #340]	; c96c <nf2_check_irq+0x218>
    c814:	e59f0154 	ldr	r0, [pc, #340]	; c970 <nf2_check_irq+0x21c>
    c818:	e1a0e00f 	mov	lr, pc
    c81c:	e593f000 	ldr	pc, [r3]
    c820:	e3a03a01 	mov	r3, #4096	; 0x1000
    c824:	e5843090 	str	r3, [r4, #144]
    c828:	ea00004b 	b	c95c <nf2_check_irq+0x208>
    c82c:	e3150001 	tst	r5, #1	; 0x1
    c830:	13a0300f 	movne	r3, #15	; 0xf
    c834:	1584508c 	strne	r5, [r4, #140]
    c838:	1584304c 	strne	r3, [r4, #76]
    c83c:	1a000046 	bne	c95c <nf2_check_irq+0x208>
    c840:	e3150002 	tst	r5, #2	; 0x2
    c844:	13a03002 	movne	r3, #2	; 0x2
    c848:	1584308c 	strne	r3, [r4, #140]
    c84c:	13a03001 	movne	r3, #1	; 0x1
    c850:	15843088 	strne	r3, [r4, #136]
    c854:	1a000040 	bne	c95c <nf2_check_irq+0x208>
    c858:	e3150902 	tst	r5, #32768	; 0x8000
    c85c:	13a03902 	movne	r3, #32768	; 0x8000
    c860:	1584308c 	strne	r3, [r4, #140]
    c864:	13c55902 	bicne	r5, r5, #32768	; 0x8000
    c868:	1a00003b 	bne	c95c <nf2_check_irq+0x208>
    c86c:	e3150c01 	tst	r5, #256	; 0x100
    c870:	13a03c01 	movne	r3, #256	; 0x100
    c874:	1584308c 	strne	r3, [r4, #140]
    c878:	13c55c01 	bicne	r5, r5, #256	; 0x100
    c87c:	1a000036 	bne	c95c <nf2_check_irq+0x208>
    c880:	e3150c02 	tst	r5, #512	; 0x200
    c884:	0a000007 	beq	c8a8 <nf2_check_irq+0x154>
    c888:	e3a02c02 	mov	r2, #512	; 0x200
    c88c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c890:	e583208c 	str	r2, [r3, #140]
    c894:	e3a02039 	mov	r2, #57	; 0x39
    c898:	e3e0332f 	mvn	r3, #-1140850688	; 0xbc000000
    c89c:	e5032f77 	str	r2, [r3, #-3959]
    c8a0:	e3c55c02 	bic	r5, r5, #512	; 0x200
    c8a4:	ea00002c 	b	c95c <nf2_check_irq+0x208>
    c8a8:	e3150010 	tst	r5, #16	; 0x10
    c8ac:	0a000008 	beq	c8d4 <nf2_check_irq+0x180>
    c8b0:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c8b4:	e3a03010 	mov	r3, #16	; 0x10
    c8b8:	e582308c 	str	r3, [r2, #140]
    c8bc:	e592309c 	ldr	r3, [r2, #156]
    c8c0:	e3c55010 	bic	r5, r5, #16	; 0x10
    c8c4:	e5823034 	str	r3, [r2, #52]
    c8c8:	e3a03002 	mov	r3, #2	; 0x2
    c8cc:	e5823088 	str	r3, [r2, #136]
    c8d0:	ea000021 	b	c95c <nf2_check_irq+0x208>
    c8d4:	e3150080 	tst	r5, #128	; 0x80
    c8d8:	0a00000b 	beq	c90c <nf2_check_irq+0x1b8>
    c8dc:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    c8e0:	e3a03080 	mov	r3, #128	; 0x80
    c8e4:	e584308c 	str	r3, [r4, #140]
    c8e8:	e594309c 	ldr	r3, [r4, #156]
    c8ec:	e3a00008 	mov	r0, #8	; 0x8
    c8f0:	e0833000 	add	r3, r3, r0
    c8f4:	e5843034 	str	r3, [r4, #52]
    c8f8:	ebffff7d 	bl	c6f4 <nf2_soft_reset>
    c8fc:	e3a03002 	mov	r3, #2	; 0x2
    c900:	e5843088 	str	r3, [r4, #136]
    c904:	e3c55080 	bic	r5, r5, #128	; 0x80
    c908:	ea000013 	b	c95c <nf2_check_irq+0x208>
    c90c:	e3150040 	tst	r5, #64	; 0x40
    c910:	0a000009 	beq	c93c <nf2_check_irq+0x1e8>
    c914:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c918:	e3a03040 	mov	r3, #64	; 0x40
    c91c:	e582308c 	str	r3, [r2, #140]
    c920:	e3a03004 	mov	r3, #4	; 0x4
    c924:	e5823088 	str	r3, [r2, #136]
    c928:	e3a02038 	mov	r2, #56	; 0x38
    c92c:	e3e0332f 	mvn	r3, #-1140850688	; 0xbc000000
    c930:	e5032f77 	str	r2, [r3, #-3959]
    c934:	e3c55040 	bic	r5, r5, #64	; 0x40
    c938:	ea000007 	b	c95c <nf2_check_irq+0x208>
    c93c:	e3150801 	tst	r5, #65536	; 0x10000
    c940:	0a000005 	beq	c95c <nf2_check_irq+0x208>
    c944:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c948:	e3a02801 	mov	r2, #65536	; 0x10000
    c94c:	e583208c 	str	r2, [r3, #140]
    c950:	e3a02008 	mov	r2, #8	; 0x8
    c954:	e5832088 	str	r2, [r3, #136]
    c958:	e3c55801 	bic	r5, r5, #65536	; 0x10000
    c95c:	e1a00005 	mov	r0, r5
    c960:	e28dd004 	add	sp, sp, #4	; 0x4
    c964:	e8bd8030 	pop	{r4, r5, pc}
    c968:	00080400 	.word	0x00080400
    c96c:	3c00d6cc 	.word	0x3c00d6cc
    c970:	00018704 	.word	0x00018704

0000c974 <nf2_erase>:
    c974:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    c978:	e3a00001 	mov	r0, #1	; 0x1
    c97c:	e24dd008 	sub	sp, sp, #8	; 0x8
    c980:	e1a06001 	mov	r6, r1
    c984:	e1a05002 	mov	r5, r2
    c988:	ebffff59 	bl	c6f4 <nf2_soft_reset>
    c98c:	e2504000 	subs	r4, r0, #0	; 0x0
    c990:	13e04000 	mvnne	r4, #0	; 0x0
    c994:	1a000035 	bne	ca70 <nf2_erase+0xfc>
    c998:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c99c:	e5826020 	str	r6, [r2, #32]
    c9a0:	e58d4004 	str	r4, [sp, #4]
    c9a4:	e5923054 	ldr	r3, [r2, #84]
    c9a8:	e1a0e002 	mov	lr, r2
    c9ac:	e58d3004 	str	r3, [sp, #4]
    c9b0:	e59d3004 	ldr	r3, [sp, #4]
    c9b4:	e2450002 	sub	r0, r5, #2	; 0x2
    c9b8:	e3833001 	orr	r3, r3, #1	; 0x1
    c9bc:	e58d3004 	str	r3, [sp, #4]
    c9c0:	e59d3004 	ldr	r3, [sp, #4]
    c9c4:	e3a0c0ff 	mov	ip, #255	; 0xff
    c9c8:	e5823054 	str	r3, [r2, #84]
    c9cc:	e3043060 	movw	r3, #16480	; 0x4060
    c9d0:	e5823060 	str	r3, [r2, #96]
    c9d4:	e1a02004 	mov	r2, r4
    c9d8:	ea000003 	b	c9ec <nf2_erase+0x78>
    c9dc:	e006311c 	and	r3, r6, ip, lsl r1
    c9e0:	e1a03153 	asr	r3, r3, r1
    c9e4:	e3833c41 	orr	r3, r3, #16640	; 0x4100
    c9e8:	e58e3060 	str	r3, [lr, #96]
    c9ec:	e1520000 	cmp	r2, r0
    c9f0:	e1a01182 	lsl	r1, r2, #3
    c9f4:	e2822001 	add	r2, r2, #1	; 0x1
    c9f8:	bafffff7 	blt	c9dc <nf2_erase+0x68>
    c9fc:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    ca00:	e30438d0 	movw	r3, #18640	; 0x48d0
    ca04:	e5823060 	str	r3, [r2, #96]
    ca08:	e3a03000 	mov	r3, #0	; 0x0
    ca0c:	e5823054 	str	r3, [r2, #84]
    ca10:	e3e03001 	mvn	r3, #1	; 0x1
    ca14:	e582304c 	str	r3, [r2, #76]
    ca18:	e2833003 	add	r3, r3, #3	; 0x3
    ca1c:	e5823084 	str	r3, [r2, #132]
    ca20:	e1a07002 	mov	r7, r2
    ca24:	eb0003db 	bl	d998 <get_ticks>
    ca28:	e59f804c 	ldr	r8, [pc, #76]	; ca7c <nf2_erase+0x108>
    ca2c:	e1a05000 	mov	r5, r0
    ca30:	e3006801 	movw	r6, #2049	; 0x801
    ca34:	eb0003d7 	bl	d998 <get_ticks>
    ca38:	e0650000 	rsb	r0, r5, r0
    ca3c:	e1500008 	cmp	r0, r8
    ca40:	e3002801 	movw	r2, #2049	; 0x801
    ca44:	c3e04000 	mvngt	r4, #0	; 0x0
    ca48:	ca000003 	bgt	ca5c <nf2_erase+0xe8>
    ca4c:	e597308c 	ldr	r3, [r7, #140]
    ca50:	e0032002 	and	r2, r3, r2
    ca54:	e1520006 	cmp	r2, r6
    ca58:	1afffff5 	bne	ca34 <nf2_erase+0xc0>
    ca5c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    ca60:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    ca64:	e583208c 	str	r2, [r3, #140]
    ca68:	e3a0200f 	mov	r2, #15	; 0xf
    ca6c:	e583204c 	str	r2, [r3, #76]
    ca70:	e1a00004 	mov	r0, r4
    ca74:	e28dd008 	add	sp, sp, #8	; 0x8
    ca78:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    ca7c:	0007a120 	.word	0x0007a120

0000ca80 <nf2_page_op>:
    ca80:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ca84:	e24dd0b4 	sub	sp, sp, #180	; 0xb4
    ca88:	e59d9120 	ldr	r9, [sp, #288]
    ca8c:	e59da124 	ldr	sl, [sp, #292]
    ca90:	e3a0c000 	mov	ip, #0	; 0x0
    ca94:	e589c000 	str	ip, [r9]
    ca98:	e58d0074 	str	r0, [sp, #116]
    ca9c:	e58ac000 	str	ip, [sl]
    caa0:	e3a00001 	mov	r0, #1	; 0x1
    caa4:	e58dc0ac 	str	ip, [sp, #172]
    caa8:	e58d1070 	str	r1, [sp, #112]
    caac:	e58d206c 	str	r2, [sp, #108]
    cab0:	e58d3068 	str	r3, [sp, #104]
    cab4:	e59db0f0 	ldr	fp, [sp, #240]
    cab8:	e59d5104 	ldr	r5, [sp, #260]
    cabc:	e59d8108 	ldr	r8, [sp, #264]
    cac0:	e59d710c 	ldr	r7, [sp, #268]
    cac4:	e59d4110 	ldr	r4, [sp, #272]
    cac8:	e59da118 	ldr	sl, [sp, #280]
    cacc:	e59d911c 	ldr	r9, [sp, #284]
    cad0:	ebffff07 	bl	c6f4 <nf2_soft_reset>
    cad4:	e3500000 	cmp	r0, #0	; 0x0
    cad8:	13e0c000 	mvnne	ip, #0	; 0x0
    cadc:	e58d009c 	str	r0, [sp, #156]
    cae0:	158dc09c 	strne	ip, [sp, #156]
    cae4:	1a000208 	bne	d30c <nf2_page_op+0x88c>
    cae8:	e59d30dc 	ldr	r3, [sp, #220]
    caec:	e3550001 	cmp	r5, #1	; 0x1
    caf0:	e2833004 	add	r3, r3, #4	; 0x4
    caf4:	159de09c 	ldrne	lr, [sp, #156]
    caf8:	e59d1070 	ldr	r1, [sp, #112]
    cafc:	e58d3098 	str	r3, [sp, #152]
    cb00:	03a0330a 	moveq	r3, #671088640	; 0x28000000
    cb04:	13a0330a 	movne	r3, #671088640	; 0x28000000
    cb08:	05835164 	streq	r5, [r3, #356]
    cb0c:	1583e164 	strne	lr, [r3, #356]
    cb10:	e3510000 	cmp	r1, #0	; 0x0
    cb14:	1a000042 	bne	cc24 <nf2_page_op+0x1a4>
    cb18:	e59d2074 	ldr	r2, [sp, #116]
    cb1c:	e3520001 	cmp	r2, #1	; 0x1
    cb20:	1a00002f 	bne	cbe4 <nf2_page_op+0x164>
    cb24:	e3550001 	cmp	r5, #1	; 0x1
    cb28:	1a000018 	bne	cb90 <nf2_page_op+0x110>
    cb2c:	e5982000 	ldr	r2, [r8]
    cb30:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cb34:	e5832140 	str	r2, [r3, #320]
    cb38:	e5982004 	ldr	r2, [r8, #4]
    cb3c:	e1d4c0bc 	ldrh	ip, [r4, #12]
    cb40:	e5832144 	str	r2, [r3, #324]
    cb44:	e5982008 	ldr	r2, [r8, #8]
    cb48:	e1d470b2 	ldrh	r7, [r4, #2]
    cb4c:	e5832148 	str	r2, [r3, #328]
    cb50:	e1d400b0 	ldrh	r0, [r4]
    cb54:	e1d460b6 	ldrh	r6, [r4, #6]
    cb58:	e1d410b4 	ldrh	r1, [r4, #4]
    cb5c:	e1d450ba 	ldrh	r5, [r4, #10]
    cb60:	e1d420b8 	ldrh	r2, [r4, #8]
    cb64:	e1d4e0be 	ldrh	lr, [r4, #14]
    cb68:	e598400c 	ldr	r4, [r8, #12]
    cb6c:	e1800807 	orr	r0, r0, r7, lsl #16
    cb70:	e1811806 	orr	r1, r1, r6, lsl #16
    cb74:	e1822805 	orr	r2, r2, r5, lsl #16
    cb78:	e18cc80e 	orr	ip, ip, lr, lsl #16
    cb7c:	e583414c 	str	r4, [r3, #332]
    cb80:	e5830150 	str	r0, [r3, #336]
    cb84:	e5831154 	str	r1, [r3, #340]
    cb88:	e5832158 	str	r2, [r3, #344]
    cb8c:	e583c15c 	str	ip, [r3, #348]
    cb90:	e59d006c 	ldr	r0, [sp, #108]
    cb94:	e59d10d8 	ldr	r1, [sp, #216]
    cb98:	e59d20f8 	ldr	r2, [sp, #248]
    cb9c:	ebfffe07 	bl	c3c0 <nf2_ecc_num>
    cba0:	e59d9098 	ldr	r9, [sp, #152]
    cba4:	e59da0d8 	ldr	sl, [sp, #216]
    cba8:	e35b0001 	cmp	fp, #1	; 0x1
    cbac:	e089300a 	add	r3, r9, sl
    cbb0:	11a04003 	movne	r4, r3
    cbb4:	11a0c000 	movne	ip, r0
    cbb8:	1a000005 	bne	cbd4 <nf2_page_op+0x154>
    cbbc:	e59dc0f4 	ldr	ip, [sp, #244]
    cbc0:	e1a04003 	mov	r4, r3
    cbc4:	e35c0000 	cmp	ip, #0	; 0x0
    cbc8:	03a03008 	moveq	r3, #8	; 0x8
    cbcc:	13a0300c 	movne	r3, #12	; 0xc
    cbd0:	e083c000 	add	ip, r3, r0
    cbd4:	e1a0e004 	mov	lr, r4
    cbd8:	e1a06004 	mov	r6, r4
    cbdc:	e3a05000 	mov	r5, #0	; 0x0
    cbe0:	ea0000ec 	b	cf98 <nf2_page_op+0x518>
    cbe4:	e3550001 	cmp	r5, #1	; 0x1
    cbe8:	1a000008 	bne	cc10 <nf2_page_op+0x190>
    cbec:	e5983000 	ldr	r3, [r8]
    cbf0:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    cbf4:	e5823140 	str	r3, [r2, #320]
    cbf8:	e5983004 	ldr	r3, [r8, #4]
    cbfc:	e5823144 	str	r3, [r2, #324]
    cc00:	e5983008 	ldr	r3, [r8, #8]
    cc04:	e5823148 	str	r3, [r2, #328]
    cc08:	e598300c 	ldr	r3, [r8, #12]
    cc0c:	e582314c 	str	r3, [r2, #332]
    cc10:	e59de098 	ldr	lr, [sp, #152]
    cc14:	e59d10d8 	ldr	r1, [sp, #216]
    cc18:	e3a0b000 	mov	fp, #0	; 0x0
    cc1c:	e08e4001 	add	r4, lr, r1
    cc20:	ea000030 	b	cce8 <nf2_page_op+0x268>
    cc24:	e59d30fc 	ldr	r3, [sp, #252]
    cc28:	e3530001 	cmp	r3, #1	; 0x1
    cc2c:	1a00003f 	bne	cd30 <nf2_page_op+0x2b0>
    cc30:	e59d2074 	ldr	r2, [sp, #116]
    cc34:	e3520001 	cmp	r2, #1	; 0x1
    cc38:	1a000026 	bne	ccd8 <nf2_page_op+0x258>
    cc3c:	e3550001 	cmp	r5, #1	; 0x1
    cc40:	1a000018 	bne	cca8 <nf2_page_op+0x228>
    cc44:	e5982000 	ldr	r2, [r8]
    cc48:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cc4c:	e5832140 	str	r2, [r3, #320]
    cc50:	e5982004 	ldr	r2, [r8, #4]
    cc54:	e1d4c0bc 	ldrh	ip, [r4, #12]
    cc58:	e5832144 	str	r2, [r3, #324]
    cc5c:	e5982008 	ldr	r2, [r8, #8]
    cc60:	e1d470b2 	ldrh	r7, [r4, #2]
    cc64:	e5832148 	str	r2, [r3, #328]
    cc68:	e1d400b0 	ldrh	r0, [r4]
    cc6c:	e1d460b6 	ldrh	r6, [r4, #6]
    cc70:	e1d410b4 	ldrh	r1, [r4, #4]
    cc74:	e1d450ba 	ldrh	r5, [r4, #10]
    cc78:	e1d420b8 	ldrh	r2, [r4, #8]
    cc7c:	e1d4e0be 	ldrh	lr, [r4, #14]
    cc80:	e598400c 	ldr	r4, [r8, #12]
    cc84:	e1800807 	orr	r0, r0, r7, lsl #16
    cc88:	e1811806 	orr	r1, r1, r6, lsl #16
    cc8c:	e1822805 	orr	r2, r2, r5, lsl #16
    cc90:	e18cc80e 	orr	ip, ip, lr, lsl #16
    cc94:	e583414c 	str	r4, [r3, #332]
    cc98:	e5830150 	str	r0, [r3, #336]
    cc9c:	e5831154 	str	r1, [r3, #340]
    cca0:	e5832158 	str	r2, [r3, #344]
    cca4:	e583c15c 	str	ip, [r3, #348]
    cca8:	e59d006c 	ldr	r0, [sp, #108]
    ccac:	e59d10d8 	ldr	r1, [sp, #216]
    ccb0:	e59d20f8 	ldr	r2, [sp, #248]
    ccb4:	ebfffdc1 	bl	c3c0 <nf2_ecc_num>
    ccb8:	e59d90d8 	ldr	r9, [sp, #216]
    ccbc:	e59d3098 	ldr	r3, [sp, #152]
    ccc0:	e3a0b000 	mov	fp, #0	; 0x0
    ccc4:	e1a0c000 	mov	ip, r0
    ccc8:	e0836009 	add	r6, r3, r9
    cccc:	e1a04009 	mov	r4, r9
    ccd0:	e1a0e009 	mov	lr, r9
    ccd4:	ea000013 	b	cd28 <nf2_page_op+0x2a8>
    ccd8:	e3550001 	cmp	r5, #1	; 0x1
    ccdc:	0a000003 	beq	ccf0 <nf2_page_op+0x270>
    cce0:	e59d40d8 	ldr	r4, [sp, #216]
    cce4:	e3a0b000 	mov	fp, #0	; 0x0
    cce8:	e1a0e004 	mov	lr, r4
    ccec:	ea00000b 	b	cd20 <nf2_page_op+0x2a0>
    ccf0:	e5983000 	ldr	r3, [r8]
    ccf4:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    ccf8:	e5823140 	str	r3, [r2, #320]
    ccfc:	e5983004 	ldr	r3, [r8, #4]
    cd00:	e3a0b000 	mov	fp, #0	; 0x0
    cd04:	e5823144 	str	r3, [r2, #324]
    cd08:	e5983008 	ldr	r3, [r8, #8]
    cd0c:	e59d40d8 	ldr	r4, [sp, #216]
    cd10:	e5823148 	str	r3, [r2, #328]
    cd14:	e598300c 	ldr	r3, [r8, #12]
    cd18:	e59de0d8 	ldr	lr, [sp, #216]
    cd1c:	e582314c 	str	r3, [r2, #332]
    cd20:	e1a0c00b 	mov	ip, fp
    cd24:	e1a0600b 	mov	r6, fp
    cd28:	e1a0500b 	mov	r5, fp
    cd2c:	ea000099 	b	cf98 <nf2_page_op+0x518>
    cd30:	e59dc074 	ldr	ip, [sp, #116]
    cd34:	e35c0001 	cmp	ip, #1	; 0x1
    cd38:	1a00007b 	bne	cf2c <nf2_page_op+0x4ac>
    cd3c:	e59d10d8 	ldr	r1, [sp, #216]
    cd40:	e59d006c 	ldr	r0, [sp, #108]
    cd44:	e59d20f8 	ldr	r2, [sp, #248]
    cd48:	ebfffd9c 	bl	c3c0 <nf2_ecc_num>
    cd4c:	e59de098 	ldr	lr, [sp, #152]
    cd50:	e59d10d8 	ldr	r1, [sp, #216]
    cd54:	e35b0001 	cmp	fp, #1	; 0x1
    cd58:	e08ee001 	add	lr, lr, r1
    cd5c:	e58d00a0 	str	r0, [sp, #160]
    cd60:	e58de0a4 	str	lr, [sp, #164]
    cd64:	1a000037 	bne	ce48 <nf2_page_op+0x3c8>
    cd68:	e3550001 	cmp	r5, #1	; 0x1
    cd6c:	1a00002a 	bne	ce1c <nf2_page_op+0x39c>
    cd70:	e1d790b6 	ldrh	r9, [r7, #6]
    cd74:	e1d730b2 	ldrh	r3, [r7, #2]
    cd78:	e58d9060 	str	r9, [sp, #96]
    cd7c:	e59d9114 	ldr	r9, [sp, #276]
    cd80:	e59d2114 	ldr	r2, [sp, #276]
    cd84:	e58d3064 	str	r3, [sp, #100]
    cd88:	e1d7a0ba 	ldrh	sl, [r7, #10]
    cd8c:	e1d930be 	ldrh	r3, [r9, #14]
    cd90:	e1d740b0 	ldrh	r4, [r7]
    cd94:	e59d9064 	ldr	r9, [sp, #100]
    cd98:	e1d210b2 	ldrh	r1, [r2, #2]
    cd9c:	e1d750bc 	ldrh	r5, [r7, #12]
    cda0:	e1d7e0b4 	ldrh	lr, [r7, #4]
    cda4:	e58da05c 	str	sl, [sp, #92]
    cda8:	e1844809 	orr	r4, r4, r9, lsl #16
    cdac:	e1d7a0be 	ldrh	sl, [r7, #14]
    cdb0:	e59d9060 	ldr	r9, [sp, #96]
    cdb4:	e58d1058 	str	r1, [sp, #88]
    cdb8:	e1d260bc 	ldrh	r6, [r2, #12]
    cdbc:	e1d7c0b8 	ldrh	ip, [r7, #8]
    cdc0:	e1d200b0 	ldrh	r0, [r2]
    cdc4:	e1d280b6 	ldrh	r8, [r2, #6]
    cdc8:	e1d210b4 	ldrh	r1, [r2, #4]
    cdcc:	e1d270ba 	ldrh	r7, [r2, #10]
    cdd0:	e18ee809 	orr	lr, lr, r9, lsl #16
    cdd4:	e1d220b8 	ldrh	r2, [r2, #8]
    cdd8:	e59d905c 	ldr	r9, [sp, #92]
    cddc:	e185580a 	orr	r5, r5, sl, lsl #16
    cde0:	e59da058 	ldr	sl, [sp, #88]
    cde4:	e1866803 	orr	r6, r6, r3, lsl #16
    cde8:	e18cc809 	orr	ip, ip, r9, lsl #16
    cdec:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cdf0:	e180080a 	orr	r0, r0, sl, lsl #16
    cdf4:	e1811808 	orr	r1, r1, r8, lsl #16
    cdf8:	e1822807 	orr	r2, r2, r7, lsl #16
    cdfc:	e5834140 	str	r4, [r3, #320]
    ce00:	e583e144 	str	lr, [r3, #324]
    ce04:	e583c148 	str	ip, [r3, #328]
    ce08:	e583514c 	str	r5, [r3, #332]
    ce0c:	e5830150 	str	r0, [r3, #336]
    ce10:	e5831154 	str	r1, [r3, #340]
    ce14:	e5832158 	str	r2, [r3, #344]
    ce18:	e583615c 	str	r6, [r3, #348]
    ce1c:	e59d10f4 	ldr	r1, [sp, #244]
    ce20:	e59dc0a4 	ldr	ip, [sp, #164]
    ce24:	e59de0a0 	ldr	lr, [sp, #160]
    ce28:	e59d4098 	ldr	r4, [sp, #152]
    ce2c:	e3510000 	cmp	r1, #0	; 0x0
    ce30:	e08c600e 	add	r6, ip, lr
    ce34:	e59d50d8 	ldr	r5, [sp, #216]
    ce38:	03a0c008 	moveq	ip, #8	; 0x8
    ce3c:	13a0c00c 	movne	ip, #12	; 0xc
    ce40:	e1a0e004 	mov	lr, r4
    ce44:	ea000053 	b	cf98 <nf2_page_op+0x518>
    ce48:	e3550001 	cmp	r5, #1	; 0x1
    ce4c:	1a000028 	bne	cef4 <nf2_page_op+0x474>
    ce50:	e1da20b2 	ldrh	r2, [sl, #2]
    ce54:	e1da30b6 	ldrh	r3, [sl, #6]
    ce58:	e1d910b2 	ldrh	r1, [r9, #2]
    ce5c:	e58d2054 	str	r2, [sp, #84]
    ce60:	e1dac0ba 	ldrh	ip, [sl, #10]
    ce64:	e1d960bc 	ldrh	r6, [r9, #12]
    ce68:	e1da40b0 	ldrh	r4, [sl]
    ce6c:	e58d3050 	str	r3, [sp, #80]
    ce70:	e58d1048 	str	r1, [sp, #72]
    ce74:	e1d930be 	ldrh	r3, [r9, #14]
    ce78:	e1d900b0 	ldrh	r0, [r9]
    ce7c:	e1d980b6 	ldrh	r8, [r9, #6]
    ce80:	e1d910b4 	ldrh	r1, [r9, #4]
    ce84:	e1d970ba 	ldrh	r7, [r9, #10]
    ce88:	e1d920b8 	ldrh	r2, [r9, #8]
    ce8c:	e59d9054 	ldr	r9, [sp, #84]
    ce90:	e1da50bc 	ldrh	r5, [sl, #12]
    ce94:	e1dae0b4 	ldrh	lr, [sl, #4]
    ce98:	e58dc04c 	str	ip, [sp, #76]
    ce9c:	e1844809 	orr	r4, r4, r9, lsl #16
    cea0:	e1dac0b8 	ldrh	ip, [sl, #8]
    cea4:	e59d9050 	ldr	r9, [sp, #80]
    cea8:	e1daa0be 	ldrh	sl, [sl, #14]
    ceac:	e18ee809 	orr	lr, lr, r9, lsl #16
    ceb0:	e185580a 	orr	r5, r5, sl, lsl #16
    ceb4:	e59d904c 	ldr	r9, [sp, #76]
    ceb8:	e59da048 	ldr	sl, [sp, #72]
    cebc:	e1866803 	orr	r6, r6, r3, lsl #16
    cec0:	e18cc809 	orr	ip, ip, r9, lsl #16
    cec4:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cec8:	e180080a 	orr	r0, r0, sl, lsl #16
    cecc:	e1811808 	orr	r1, r1, r8, lsl #16
    ced0:	e1822807 	orr	r2, r2, r7, lsl #16
    ced4:	e5834140 	str	r4, [r3, #320]
    ced8:	e583e144 	str	lr, [r3, #324]
    cedc:	e583c148 	str	ip, [r3, #328]
    cee0:	e583514c 	str	r5, [r3, #332]
    cee4:	e5830150 	str	r0, [r3, #336]
    cee8:	e5831154 	str	r1, [r3, #340]
    ceec:	e5832158 	str	r2, [r3, #344]
    cef0:	e583615c 	str	r6, [r3, #348]
    cef4:	e59dc0d8 	ldr	ip, [sp, #216]
    cef8:	e59d00a0 	ldr	r0, [sp, #160]
    cefc:	e1a0454c 	asr	r4, ip, #10
    cf00:	e1a01004 	mov	r1, r4
    cf04:	eb001ea2 	bl	14994 <__aeabi_idiv>
    cf08:	e59de0a4 	ldr	lr, [sp, #164]
    cf0c:	e2444001 	sub	r4, r4, #1	; 0x1
    cf10:	e026e490 	mla	r6, r0, r4, lr
    cf14:	e59d10d8 	ldr	r1, [sp, #216]
    cf18:	e59d4098 	ldr	r4, [sp, #152]
    cf1c:	e1a0c000 	mov	ip, r0
    cf20:	e284eb01 	add	lr, r4, #1024	; 0x400
    cf24:	e2415b01 	sub	r5, r1, #1024	; 0x400
    cf28:	ea00001a 	b	cf98 <nf2_page_op+0x518>
    cf2c:	e3550001 	cmp	r5, #1	; 0x1
    cf30:	159d4098 	ldrne	r4, [sp, #152]
    cf34:	13a0b000 	movne	fp, #0	; 0x0
    cf38:	1a000012 	bne	cf88 <nf2_page_op+0x508>
    cf3c:	e1d740b6 	ldrh	r4, [r7, #6]
    cf40:	e1d730be 	ldrh	r3, [r7, #14]
    cf44:	e1d7c0bc 	ldrh	ip, [r7, #12]
    cf48:	e1d750b2 	ldrh	r5, [r7, #2]
    cf4c:	e1d700b0 	ldrh	r0, [r7]
    cf50:	e1d710b4 	ldrh	r1, [r7, #4]
    cf54:	e1d7e0ba 	ldrh	lr, [r7, #10]
    cf58:	e1d720b8 	ldrh	r2, [r7, #8]
    cf5c:	e1811804 	orr	r1, r1, r4, lsl #16
    cf60:	e18cc803 	orr	ip, ip, r3, lsl #16
    cf64:	e1800805 	orr	r0, r0, r5, lsl #16
    cf68:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cf6c:	e182280e 	orr	r2, r2, lr, lsl #16
    cf70:	e59d4098 	ldr	r4, [sp, #152]
    cf74:	e5830140 	str	r0, [r3, #320]
    cf78:	e3a0b000 	mov	fp, #0	; 0x0
    cf7c:	e5831144 	str	r1, [r3, #324]
    cf80:	e5832148 	str	r2, [r3, #328]
    cf84:	e583c14c 	str	ip, [r3, #332]
    cf88:	e59d50d8 	ldr	r5, [sp, #216]
    cf8c:	e1a0e004 	mov	lr, r4
    cf90:	e1a0c00b 	mov	ip, fp
    cf94:	e1a0600b 	mov	r6, fp
    cf98:	e59d2130 	ldr	r2, [sp, #304]
    cf9c:	e59d90f8 	ldr	r9, [sp, #248]
    cfa0:	e3520001 	cmp	r2, #1	; 0x1
    cfa4:	059d312c 	ldreq	r3, [sp, #300]
    cfa8:	e59da0e0 	ldr	sl, [sp, #224]
    cfac:	00634004 	rsbeq	r4, r3, r4
    cfb0:	e59d1070 	ldr	r1, [sp, #112]
    cfb4:	e3a03000 	mov	r3, #0	; 0x0
    cfb8:	e59d206c 	ldr	r2, [sp, #108]
    cfbc:	e59d0074 	ldr	r0, [sp, #116]
    cfc0:	e88d5000 	stm	sp, {ip, lr}
    cfc4:	e58d900c 	str	r9, [sp, #12]
    cfc8:	e58da008 	str	sl, [sp, #8]
    cfcc:	ebfffce5 	bl	c368 <nf2_ecc_cfg>
    cfd0:	e1a0000b 	mov	r0, fp
    cfd4:	e59d10f4 	ldr	r1, [sp, #244]
    cfd8:	e59d2068 	ldr	r2, [sp, #104]
    cfdc:	ebfffcc2 	bl	c2ec <nf2_secc_cfg>
    cfe0:	e59d3134 	ldr	r3, [sp, #308]
    cfe4:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    cfe8:	e3530010 	cmp	r3, #16	; 0x10
    cfec:	e59d30e8 	ldr	r3, [sp, #232]
    cff0:	e59dc130 	ldr	ip, [sp, #304]
    cff4:	e59de12c 	ldr	lr, [sp, #300]
    cff8:	e5823038 	str	r3, [r2, #56]
    cffc:	e59d3128 	ldr	r3, [sp, #296]
    d000:	e1a0110c 	lsl	r1, ip, #2
    d004:	e582303c 	str	r3, [r2, #60]
    d008:	e184080e 	orr	r0, r4, lr, lsl #16
    d00c:	e3811001 	orr	r1, r1, #1	; 0x1
    d010:	e3a03341 	mov	r3, #67108865	; 0x4000001
    d014:	e5820040 	str	r0, [r2, #64]
    d018:	e5823044 	str	r3, [r2, #68]
    d01c:	e5821048 	str	r1, [r2, #72]
    d020:	e59d20e4 	ldr	r2, [sp, #228]
    d024:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    d028:	e59d10e0 	ldr	r1, [sp, #224]
    d02c:	e5842020 	str	r2, [r4, #32]
    d030:	e59d2100 	ldr	r2, [sp, #256]
    d034:	01a050c5 	asreq	r5, r5, #1
    d038:	01a060c6 	asreq	r6, r6, #1
    d03c:	e1853806 	orr	r3, r5, r6, lsl #16
    d040:	e3510000 	cmp	r1, #0	; 0x0
    d044:	e3a05000 	mov	r5, #0	; 0x0
    d048:	e5845024 	str	r5, [r4, #36]
    d04c:	e2422001 	sub	r2, r2, #1	; 0x1
    d050:	e5845028 	str	r5, [r4, #40]
    d054:	e584502c 	str	r5, [r4, #44]
    d058:	e5843030 	str	r3, [r4, #48]
    d05c:	1a000041 	bne	d168 <nf2_page_op+0x6e8>
    d060:	e382c008 	orr	ip, r2, #8	; 0x8
    d064:	e3a03020 	mov	r3, #32	; 0x20
    d068:	e20cc00f 	and	ip, ip, #15	; 0xf
    d06c:	e59d20e0 	ldr	r2, [sp, #224]
    d070:	e3a0e001 	mov	lr, #1	; 0x1
    d074:	e5843090 	str	r3, [r4, #144]
    d078:	e1a0c50c 	lsl	ip, ip, #10
    d07c:	e3a03901 	mov	r3, #16384	; 0x4000
    d080:	e584e054 	str	lr, [r4, #84]
    d084:	e38ccc41 	orr	ip, ip, #16640	; 0x4100
    d088:	e5843060 	str	r3, [r4, #96]
    d08c:	e2833e43 	add	r3, r3, #1072	; 0x430
    d090:	e584c060 	str	ip, [r4, #96]
    d094:	e1a0100e 	mov	r1, lr
    d098:	e5843060 	str	r3, [r4, #96]
    d09c:	e3a00008 	mov	r0, #8	; 0x8
    d0a0:	ebfffc0b 	bl	c0d4 <trx_afifo_reg>
    d0a4:	e59d30e0 	ldr	r3, [sp, #224]
    d0a8:	e59d2074 	ldr	r2, [sp, #116]
    d0ac:	e5843054 	str	r3, [r4, #84]
    d0b0:	e1520005 	cmp	r2, r5
    d0b4:	e3a0300e 	mov	r3, #14	; 0xe
    d0b8:	e584304c 	str	r3, [r4, #76]
    d0bc:	13a03007 	movne	r3, #7	; 0x7
    d0c0:	03a03005 	moveq	r3, #5	; 0x5
    d0c4:	e5843084 	str	r3, [r4, #132]
    d0c8:	eb000232 	bl	d998 <get_ticks>
    d0cc:	e59f5244 	ldr	r5, [pc, #580]	; d318 <nf2_page_op+0x898>
    d0d0:	e1a04000 	mov	r4, r0
    d0d4:	eb00022f 	bl	d998 <get_ticks>
    d0d8:	e0640000 	rsb	r0, r4, r0
    d0dc:	e1500005 	cmp	r0, r5
    d0e0:	da000006 	ble	d100 <nf2_page_op+0x680>
    d0e4:	e59f0230 	ldr	r0, [pc, #560]	; d31c <nf2_page_op+0x89c>
    d0e8:	e59f3230 	ldr	r3, [pc, #560]	; d320 <nf2_page_op+0x8a0>
    d0ec:	e3e09000 	mvn	r9, #0	; 0x0
    d0f0:	e1a0e00f 	mov	lr, pc
    d0f4:	e593f000 	ldr	pc, [r3]
    d0f8:	e58d909c 	str	r9, [sp, #156]
    d0fc:	ea000002 	b	d10c <nf2_page_op+0x68c>
    d100:	ebfffd93 	bl	c754 <nf2_check_irq>
    d104:	e3500020 	cmp	r0, #32	; 0x20
    d108:	1afffff1 	bne	d0d4 <nf2_page_op+0x654>
    d10c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d110:	e593208c 	ldr	r2, [r3, #140]
    d114:	e59d00d8 	ldr	r0, [sp, #216]
    d118:	e583208c 	str	r2, [r3, #140]
    d11c:	e3a0200f 	mov	r2, #15	; 0xf
    d120:	e583204c 	str	r2, [r3, #76]
    d124:	e28d10ac 	add	r1, sp, #172	; 0xac
    d128:	ebfffcf5 	bl	c504 <nf2_check_ecc>
    d12c:	e3500000 	cmp	r0, #0	; 0x0
    d130:	059da120 	ldreq	sl, [sp, #288]
    d134:	e59dc120 	ldr	ip, [sp, #288]
    d138:	059a3000 	ldreq	r3, [sl]
    d13c:	13a030ff 	movne	r3, #255	; 0xff
    d140:	e35b0001 	cmp	fp, #1	; 0x1
    d144:	e58c3000 	str	r3, [ip]
    d148:	1a00006f 	bne	d30c <nf2_page_op+0x88c>
    d14c:	e28d00ac 	add	r0, sp, #172	; 0xac
    d150:	ebfffd36 	bl	c630 <nf2_check_secc>
    d154:	e3500000 	cmp	r0, #0	; 0x0
    d158:	159de124 	ldrne	lr, [sp, #292]
    d15c:	13a030ff 	movne	r3, #255	; 0xff
    d160:	158e3000 	strne	r3, [lr]
    d164:	ea000068 	b	d30c <nf2_page_op+0x88c>
    d168:	e382c008 	orr	ip, r2, #8	; 0x8
    d16c:	e3013001 	movw	r3, #4097	; 0x1001
    d170:	e20cc00f 	and	ip, ip, #15	; 0xf
    d174:	e5843090 	str	r3, [r4, #144]
    d178:	e3a0e001 	mov	lr, #1	; 0x1
    d17c:	e3043080 	movw	r3, #16512	; 0x4080
    d180:	e1a0c50c 	lsl	ip, ip, #10
    d184:	e584e054 	str	lr, [r4, #84]
    d188:	e38ccc41 	orr	ip, ip, #16640	; 0x4100
    d18c:	e5843060 	str	r3, [r4, #96]
    d190:	e2433dd2 	sub	r3, r3, #13440	; 0x3480
    d194:	e584c060 	str	ip, [r4, #96]
    d198:	e5843060 	str	r3, [r4, #96]
    d19c:	e3043810 	movw	r3, #18448	; 0x4810
    d1a0:	e1a0100e 	mov	r1, lr
    d1a4:	e1a02005 	mov	r2, r5
    d1a8:	e5843060 	str	r3, [r4, #96]
    d1ac:	e3a00008 	mov	r0, #8	; 0x8
    d1b0:	ebfffbc7 	bl	c0d4 <trx_afifo_reg>
    d1b4:	e59d1074 	ldr	r1, [sp, #116]
    d1b8:	e3a0300e 	mov	r3, #14	; 0xe
    d1bc:	e3510000 	cmp	r1, #0	; 0x0
    d1c0:	e5845054 	str	r5, [r4, #84]
    d1c4:	e584304c 	str	r3, [r4, #76]
    d1c8:	13a03007 	movne	r3, #7	; 0x7
    d1cc:	03a03005 	moveq	r3, #5	; 0x5
    d1d0:	e5843084 	str	r3, [r4, #132]
    d1d4:	eb0001ef 	bl	d998 <get_ticks>
    d1d8:	e59f5138 	ldr	r5, [pc, #312]	; d318 <nf2_page_op+0x898>
    d1dc:	e1a04000 	mov	r4, r0
    d1e0:	eb0001ec 	bl	d998 <get_ticks>
    d1e4:	e0640000 	rsb	r0, r4, r0
    d1e8:	e1500005 	cmp	r0, r5
    d1ec:	da00003d 	ble	d2e8 <nf2_page_op+0x868>
    d1f0:	e3a0030a 	mov	r0, #671088640	; 0x28000000
    d1f4:	e590108c 	ldr	r1, [r0, #140]
    d1f8:	e5902094 	ldr	r2, [r0, #148]
    d1fc:	e59030a0 	ldr	r3, [r0, #160]
    d200:	e590c0a4 	ldr	ip, [r0, #164]
    d204:	e590e0a8 	ldr	lr, [r0, #168]
    d208:	e590405c 	ldr	r4, [r0, #92]
    d20c:	e5905098 	ldr	r5, [r0, #152]
    d210:	e5906038 	ldr	r6, [r0, #56]
    d214:	e590703c 	ldr	r7, [r0, #60]
    d218:	e5908168 	ldr	r8, [r0, #360]
    d21c:	e590a16c 	ldr	sl, [r0, #364]
    d220:	e5909170 	ldr	r9, [r0, #368]
    d224:	e58d9078 	str	r9, [sp, #120]
    d228:	e5909174 	ldr	r9, [r0, #372]
    d22c:	e58d907c 	str	r9, [sp, #124]
    d230:	e5909178 	ldr	r9, [r0, #376]
    d234:	e58d9080 	str	r9, [sp, #128]
    d238:	e590917c 	ldr	r9, [r0, #380]
    d23c:	e58d9084 	str	r9, [sp, #132]
    d240:	e5909180 	ldr	r9, [r0, #384]
    d244:	e58d9088 	str	r9, [sp, #136]
    d248:	e5909184 	ldr	r9, [r0, #388]
    d24c:	e58d908c 	str	r9, [sp, #140]
    d250:	e5909188 	ldr	r9, [r0, #392]
    d254:	e58d9090 	str	r9, [sp, #144]
    d258:	e590918c 	ldr	r9, [r0, #396]
    d25c:	e590b190 	ldr	fp, [r0, #400]
    d260:	e58d9094 	str	r9, [sp, #148]
    d264:	e5900194 	ldr	r0, [r0, #404]
    d268:	e88d5000 	stm	sp, {ip, lr}
    d26c:	e58d4008 	str	r4, [sp, #8]
    d270:	e58d500c 	str	r5, [sp, #12]
    d274:	e58d6010 	str	r6, [sp, #16]
    d278:	e59dc07c 	ldr	ip, [sp, #124]
    d27c:	e59de080 	ldr	lr, [sp, #128]
    d280:	e59d9084 	ldr	r9, [sp, #132]
    d284:	e58da01c 	str	sl, [sp, #28]
    d288:	e59da078 	ldr	sl, [sp, #120]
    d28c:	e58dc024 	str	ip, [sp, #36]
    d290:	e58da020 	str	sl, [sp, #32]
    d294:	e59dc08c 	ldr	ip, [sp, #140]
    d298:	e59da088 	ldr	sl, [sp, #136]
    d29c:	e58de028 	str	lr, [sp, #40]
    d2a0:	e58d902c 	str	r9, [sp, #44]
    d2a4:	e59de090 	ldr	lr, [sp, #144]
    d2a8:	e59d9094 	ldr	r9, [sp, #148]
    d2ac:	e58da030 	str	sl, [sp, #48]
    d2b0:	e58dc034 	str	ip, [sp, #52]
    d2b4:	e58d0044 	str	r0, [sp, #68]
    d2b8:	e58d7014 	str	r7, [sp, #20]
    d2bc:	e58d8018 	str	r8, [sp, #24]
    d2c0:	e58de038 	str	lr, [sp, #56]
    d2c4:	e58d903c 	str	r9, [sp, #60]
    d2c8:	e58db040 	str	fp, [sp, #64]
    d2cc:	e59f0050 	ldr	r0, [pc, #80]	; d324 <nf2_page_op+0x8a4>
    d2d0:	e59fc048 	ldr	ip, [pc, #72]	; d320 <nf2_page_op+0x8a0>
    d2d4:	e3e0a000 	mvn	sl, #0	; 0x0
    d2d8:	e1a0e00f 	mov	lr, pc
    d2dc:	e59cf000 	ldr	pc, [ip]
    d2e0:	e58da09c 	str	sl, [sp, #156]
    d2e4:	ea000002 	b	d2f4 <nf2_page_op+0x874>
    d2e8:	ebfffd19 	bl	c754 <nf2_check_irq>
    d2ec:	e3500001 	cmp	r0, #1	; 0x1
    d2f0:	1affffba 	bne	d1e0 <nf2_page_op+0x760>
    d2f4:	ebfffd16 	bl	c754 <nf2_check_irq>
    d2f8:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d2fc:	e593208c 	ldr	r2, [r3, #140]
    d300:	e583208c 	str	r2, [r3, #140]
    d304:	e3a0200f 	mov	r2, #15	; 0xf
    d308:	e583204c 	str	r2, [r3, #76]
    d30c:	e59d009c 	ldr	r0, [sp, #156]
    d310:	e28dd0b4 	add	sp, sp, #180	; 0xb4
    d314:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d318:	0007a120 	.word	0x0007a120
    d31c:	00018717 	.word	0x00018717
    d320:	3c00d6cc 	.word	0x3c00d6cc
    d324:	00018725 	.word	0x00018725

0000d328 <nf2_randc_seed_hw>:
    d328:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d32c:	e1801801 	orr	r1, r0, r1, lsl #16
    d330:	e3a03001 	mov	r3, #1	; 0x1
    d334:	e58211c4 	str	r1, [r2, #452]
    d338:	e58231c8 	str	r3, [r2, #456]
    d33c:	e3a03000 	mov	r3, #0	; 0x0
    d340:	e92d4070 	push	{r4, r5, r6, lr}
    d344:	e58231c8 	str	r3, [r2, #456]
    d348:	e1a05002 	mov	r5, r2
    d34c:	eb000191 	bl	d998 <get_ticks>
    d350:	e59f6048 	ldr	r6, [pc, #72]	; d3a0 <nf2_randc_seed_hw+0x78>
    d354:	e1a04000 	mov	r4, r0
    d358:	eb00018e 	bl	d998 <get_ticks>
    d35c:	e0640000 	rsb	r0, r4, r0
    d360:	e1500006 	cmp	r0, r6
    d364:	da000005 	ble	d380 <nf2_randc_seed_hw+0x58>
    d368:	e59f0034 	ldr	r0, [pc, #52]	; d3a4 <nf2_randc_seed_hw+0x7c>
    d36c:	e59f3034 	ldr	r3, [pc, #52]	; d3a8 <nf2_randc_seed_hw+0x80>
    d370:	e1a0e00f 	mov	lr, pc
    d374:	e593f000 	ldr	pc, [r3]
    d378:	e3e00000 	mvn	r0, #0	; 0x0
    d37c:	e8bd8070 	pop	{r4, r5, r6, pc}
    d380:	e59531cc 	ldr	r3, [r5, #460]
    d384:	e3130801 	tst	r3, #65536	; 0x10000
    d388:	1afffff2 	bne	d358 <nf2_randc_seed_hw+0x30>
    d38c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d390:	e59331cc 	ldr	r3, [r3, #460]
    d394:	e1a00803 	lsl	r0, r3, #16
    d398:	e1a00820 	lsr	r0, r0, #16
    d39c:	e8bd8070 	pop	{r4, r5, r6, pc}
    d3a0:	0007a120 	.word	0x0007a120
    d3a4:	000187eb 	.word	0x000187eb
    d3a8:	3c00d6cc 	.word	0x3c00d6cc

0000d3ac <nf2_asyn_reset>:
    d3ac:	e92d4070 	push	{r4, r5, r6, lr}
    d3b0:	e1a04000 	mov	r4, r0
    d3b4:	e3a00001 	mov	r0, #1	; 0x1
    d3b8:	ebfffccd 	bl	c6f4 <nf2_soft_reset>
    d3bc:	e3a01001 	mov	r1, #1	; 0x1
    d3c0:	e1e04411 	mvn	r4, r1, lsl r4
    d3c4:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d3c8:	e30520ff 	movw	r2, #20735	; 0x50ff
    d3cc:	e5831054 	str	r1, [r3, #84]
    d3d0:	e5832060 	str	r2, [r3, #96]
    d3d4:	e3a02000 	mov	r2, #0	; 0x0
    d3d8:	e5832054 	str	r2, [r3, #84]
    d3dc:	e593204c 	ldr	r2, [r3, #76]
    d3e0:	e583404c 	str	r4, [r3, #76]
    d3e4:	e5831084 	str	r1, [r3, #132]
    d3e8:	e1a06003 	mov	r6, r3
    d3ec:	eb000169 	bl	d998 <get_ticks>
    d3f0:	e3005bb8 	movw	r5, #3000	; 0xbb8
    d3f4:	e1a04000 	mov	r4, r0
    d3f8:	eb000166 	bl	d998 <get_ticks>
    d3fc:	e0640000 	rsb	r0, r4, r0
    d400:	e1500005 	cmp	r0, r5
    d404:	da000005 	ble	d420 <nf2_asyn_reset+0x74>
    d408:	e59f0038 	ldr	r0, [pc, #56]	; d448 <nf2_asyn_reset+0x9c>
    d40c:	e59f3038 	ldr	r3, [pc, #56]	; d44c <nf2_asyn_reset+0xa0>
    d410:	e1a0e00f 	mov	lr, pc
    d414:	e593f000 	ldr	pc, [r3]
    d418:	e3e00000 	mvn	r0, #0	; 0x0
    d41c:	ea000003 	b	d430 <nf2_asyn_reset+0x84>
    d420:	e596308c 	ldr	r3, [r6, #140]
    d424:	e3130001 	tst	r3, #1	; 0x1
    d428:	0afffff2 	beq	d3f8 <nf2_asyn_reset+0x4c>
    d42c:	e3a00000 	mov	r0, #0	; 0x0
    d430:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d434:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    d438:	e583208c 	str	r2, [r3, #140]
    d43c:	e3a0200f 	mov	r2, #15	; 0xf
    d440:	e583204c 	str	r2, [r3, #76]
    d444:	e8bd8070 	pop	{r4, r5, r6, pc}
    d448:	00018806 	.word	0x00018806
    d44c:	3c00d6cc 	.word	0x3c00d6cc

0000d450 <nf2_active_sync>:
    d450:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    d454:	e1a05000 	mov	r5, r0
    d458:	e3a00001 	mov	r0, #1	; 0x1
    d45c:	ebfffca4 	bl	c6f4 <nf2_soft_reset>
    d460:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    d464:	e3a06000 	mov	r6, #0	; 0x0
    d468:	e3a07001 	mov	r7, #1	; 0x1
    d46c:	e30430ef 	movw	r3, #16623	; 0x40ef
    d470:	e58460c4 	str	r6, [r4, #196]
    d474:	e5847054 	str	r7, [r4, #84]
    d478:	e5843060 	str	r3, [r4, #96]
    d47c:	e2833012 	add	r3, r3, #18	; 0x12
    d480:	e5843060 	str	r3, [r4, #96]
    d484:	e3a0cc42 	mov	ip, #16896	; 0x4200
    d488:	e3a03c52 	mov	r3, #20992	; 0x5200
    d48c:	e3855c42 	orr	r5, r5, #16896	; 0x4200
    d490:	e5845060 	str	r5, [r4, #96]
    d494:	e3a00002 	mov	r0, #2	; 0x2
    d498:	e584c060 	str	ip, [r4, #96]
    d49c:	e3a0100f 	mov	r1, #15	; 0xf
    d4a0:	e584c060 	str	ip, [r4, #96]
    d4a4:	e1a02006 	mov	r2, r6
    d4a8:	e5843060 	str	r3, [r4, #96]
    d4ac:	ebfffb08 	bl	c0d4 <trx_afifo_reg>
    d4b0:	e3a03c01 	mov	r3, #256	; 0x100
    d4b4:	e3a00003 	mov	r0, #3	; 0x3
    d4b8:	e1a01007 	mov	r1, r7
    d4bc:	e1a02006 	mov	r2, r6
    d4c0:	e5843060 	str	r3, [r4, #96]
    d4c4:	ebfffb02 	bl	c0d4 <trx_afifo_reg>
    d4c8:	e3a0300e 	mov	r3, #14	; 0xe
    d4cc:	e5846054 	str	r6, [r4, #84]
    d4d0:	e584304c 	str	r3, [r4, #76]
    d4d4:	e5847084 	str	r7, [r4, #132]
    d4d8:	eb00012e 	bl	d998 <get_ticks>
    d4dc:	e59f8048 	ldr	r8, [pc, #72]	; d52c <nf2_active_sync+0xdc>
    d4e0:	e1a05000 	mov	r5, r0
    d4e4:	eb00012b 	bl	d998 <get_ticks>
    d4e8:	e0650000 	rsb	r0, r5, r0
    d4ec:	e1500008 	cmp	r0, r8
    d4f0:	da000005 	ble	d50c <nf2_active_sync+0xbc>
    d4f4:	e59f0034 	ldr	r0, [pc, #52]	; d530 <nf2_active_sync+0xe0>
    d4f8:	e59f3034 	ldr	r3, [pc, #52]	; d534 <nf2_active_sync+0xe4>
    d4fc:	e1a0e00f 	mov	lr, pc
    d500:	e593f000 	ldr	pc, [r3]
    d504:	e3e00000 	mvn	r0, #0	; 0x0
    d508:	ea000003 	b	d51c <nf2_active_sync+0xcc>
    d50c:	e594308c 	ldr	r3, [r4, #140]
    d510:	e3130001 	tst	r3, #1	; 0x1
    d514:	0afffff2 	beq	d4e4 <nf2_active_sync+0x94>
    d518:	e3a00000 	mov	r0, #0	; 0x0
    d51c:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    d520:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d524:	e583208c 	str	r2, [r3, #140]
    d528:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    d52c:	0007a120 	.word	0x0007a120
    d530:	0001881e 	.word	0x0001881e
    d534:	3c00d6cc 	.word	0x3c00d6cc

0000d538 <nf2_timing_init>:
    d538:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    d53c:	e1a04000 	mov	r4, r0
    d540:	e24dd004 	sub	sp, sp, #4	; 0x4
    d544:	e59f00b8 	ldr	r0, [pc, #184]	; d604 <nf2_timing_init+0xcc>
    d548:	e1a07002 	mov	r7, r2
    d54c:	e1a06003 	mov	r6, r3
    d550:	e1a05001 	mov	r5, r1
    d554:	ebffcc89 	bl	780 <module_enable>
    d558:	e3a00010 	mov	r0, #16	; 0x10
    d55c:	ebffcccd 	bl	898 <pads_init_module>
    d560:	e59d301c 	ldr	r3, [sp, #28]
    d564:	e3530010 	cmp	r3, #16	; 0x10
    d568:	13a02000 	movne	r2, #0	; 0x0
    d56c:	1a000002 	bne	d57c <nf2_timing_init+0x44>
    d570:	e3a00011 	mov	r0, #17	; 0x11
    d574:	ebffccc7 	bl	898 <pads_init_module>
    d578:	e3a02001 	mov	r2, #1	; 0x1
    d57c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d580:	e58321b8 	str	r2, [r3, #440]
    d584:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d588:	e59230c8 	ldr	r3, [r2, #200]
    d58c:	e3540000 	cmp	r4, #0	; 0x0
    d590:	e3c33070 	bic	r3, r3, #112	; 0x70
    d594:	e3833040 	orr	r3, r3, #64	; 0x40
    d598:	e58230c8 	str	r3, [r2, #200]
    d59c:	058240c4 	streq	r4, [r2, #196]
    d5a0:	059230c8 	ldreq	r3, [r2, #200]
    d5a4:	03833001 	orreq	r3, r3, #1	; 0x1
    d5a8:	0a000005 	beq	d5c4 <nf2_timing_init+0x8c>
    d5ac:	e3540001 	cmp	r4, #1	; 0x1
    d5b0:	13a03002 	movne	r3, #2	; 0x2
    d5b4:	058240c4 	streq	r4, [r2, #196]
    d5b8:	158230c4 	strne	r3, [r2, #196]
    d5bc:	e59230c8 	ldr	r3, [r2, #200]
    d5c0:	e3c33001 	bic	r3, r3, #1	; 0x1
    d5c4:	e58230c8 	str	r3, [r2, #200]
    d5c8:	e3c534ff 	bic	r3, r5, #-16777216	; 0xff000000
    d5cc:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d5d0:	e3c3360e 	bic	r3, r3, #14680064	; 0xe00000
    d5d4:	e5823000 	str	r3, [r2]
    d5d8:	e1a01c25 	lsr	r1, r5, #24
    d5dc:	e30e3001 	movw	r3, #57345	; 0xe001
    d5e0:	e5821004 	str	r1, [r2, #4]
    d5e4:	e582300c 	str	r3, [r2, #12]
    d5e8:	e58260d0 	str	r6, [r2, #208]
    d5ec:	e5827008 	str	r7, [r2, #8]
    d5f0:	e59d3018 	ldr	r3, [sp, #24]
    d5f4:	e3a00001 	mov	r0, #1	; 0x1
    d5f8:	e58230d4 	str	r3, [r2, #212]
    d5fc:	e28dd004 	add	sp, sp, #4	; 0x4
    d600:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    d604:	21e3c000 	.word	0x21e3c000

0000d608 <nand_readid>:
    d608:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    d60c:	e1a06000 	mov	r6, r0
    d610:	e24dd010 	sub	sp, sp, #16	; 0x10
    d614:	e59f013c 	ldr	r0, [pc, #316]	; d758 <nand_readid+0x150>
    d618:	e1a04002 	mov	r4, r2
    d61c:	e1a05003 	mov	r5, r3
    d620:	ebffccb5 	bl	8fc <module_reset>
    d624:	e59dc02c 	ldr	ip, [sp, #44]
    d628:	e1a01004 	mov	r1, r4
    d62c:	e58dc000 	str	ip, [sp]
    d630:	e59dc030 	ldr	ip, [sp, #48]
    d634:	e1a02005 	mov	r2, r5
    d638:	e59d3028 	ldr	r3, [sp, #40]
    d63c:	e3a00000 	mov	r0, #0	; 0x0
    d640:	e58dc004 	str	ip, [sp, #4]
    d644:	ebffffbb 	bl	d538 <nf2_timing_init>
    d648:	e3a00000 	mov	r0, #0	; 0x0
    d64c:	ebffff56 	bl	d3ac <nf2_asyn_reset>
    d650:	e3500000 	cmp	r0, #0	; 0x0
    d654:	1a00003b 	bne	d748 <nand_readid+0x140>
    d658:	e2800001 	add	r0, r0, #1	; 0x1
    d65c:	ebfffc24 	bl	c6f4 <nf2_soft_reset>
    d660:	e2505000 	subs	r5, r0, #0	; 0x0
    d664:	1a000037 	bne	d748 <nand_readid+0x140>
    d668:	e58d500c 	str	r5, [sp, #12]
    d66c:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d670:	e5923054 	ldr	r3, [r2, #84]
    d674:	e1a07002 	mov	r7, r2
    d678:	e58d300c 	str	r3, [sp, #12]
    d67c:	e59d300c 	ldr	r3, [sp, #12]
    d680:	e59f80d4 	ldr	r8, [pc, #212]	; d75c <nand_readid+0x154>
    d684:	e3833001 	orr	r3, r3, #1	; 0x1
    d688:	e58d300c 	str	r3, [sp, #12]
    d68c:	e59d300c 	ldr	r3, [sp, #12]
    d690:	e5823054 	str	r3, [r2, #84]
    d694:	e3043090 	movw	r3, #16528	; 0x4090
    d698:	e5823060 	str	r3, [r2, #96]
    d69c:	e2833070 	add	r3, r3, #112	; 0x70
    d6a0:	e5823060 	str	r3, [r2, #96]
    d6a4:	e3a03008 	mov	r3, #8	; 0x8
    d6a8:	e5823060 	str	r3, [r2, #96]
    d6ac:	e3a03c7f 	mov	r3, #32512	; 0x7f00
    d6b0:	e5823060 	str	r3, [r2, #96]
    d6b4:	e3e03001 	mvn	r3, #1	; 0x1
    d6b8:	e5825054 	str	r5, [r2, #84]
    d6bc:	e582304c 	str	r3, [r2, #76]
    d6c0:	e2833003 	add	r3, r3, #3	; 0x3
    d6c4:	e5823084 	str	r3, [r2, #132]
    d6c8:	eb0000b2 	bl	d998 <get_ticks>
    d6cc:	e1a04000 	mov	r4, r0
    d6d0:	eb0000b0 	bl	d998 <get_ticks>
    d6d4:	e0640000 	rsb	r0, r4, r0
    d6d8:	e1500008 	cmp	r0, r8
    d6dc:	c3e05000 	mvngt	r5, #0	; 0x0
    d6e0:	ca000002 	bgt	d6f0 <nand_readid+0xe8>
    d6e4:	e597308c 	ldr	r3, [r7, #140]
    d6e8:	e3130001 	tst	r3, #1	; 0x1
    d6ec:	0afffff7 	beq	d6d0 <nand_readid+0xc8>
    d6f0:	e3a0030a 	mov	r0, #671088640	; 0x28000000
    d6f4:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    d6f8:	e580308c 	str	r3, [r0, #140]
    d6fc:	e3a0300f 	mov	r3, #15	; 0xf
    d700:	e580304c 	str	r3, [r0, #76]
    d704:	e5903010 	ldr	r3, [r0, #16]
    d708:	e1a02423 	lsr	r2, r3, #8
    d70c:	e1a01823 	lsr	r1, r3, #16
    d710:	e1a0cc23 	lsr	ip, r3, #24
    d714:	e5c62006 	strb	r2, [r6, #6]
    d718:	e5c61005 	strb	r1, [r6, #5]
    d71c:	e5c6c004 	strb	ip, [r6, #4]
    d720:	e5c63007 	strb	r3, [r6, #7]
    d724:	e5903014 	ldr	r3, [r0, #20]
    d728:	e1a02c23 	lsr	r2, r3, #24
    d72c:	e1a01423 	lsr	r1, r3, #8
    d730:	e1a00823 	lsr	r0, r3, #16
    d734:	e5c62000 	strb	r2, [r6]
    d738:	e5c61002 	strb	r1, [r6, #2]
    d73c:	e5c60001 	strb	r0, [r6, #1]
    d740:	e5c63003 	strb	r3, [r6, #3]
    d744:	ea000000 	b	d74c <nand_readid+0x144>
    d748:	e3e05000 	mvn	r5, #0	; 0x0
    d74c:	e1a00005 	mov	r0, r5
    d750:	e28dd010 	add	sp, sp, #16	; 0x10
    d754:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    d758:	21e3c000 	.word	0x21e3c000
    d75c:	0007a120 	.word	0x0007a120

0000d760 <nf2_hardware_reset>:
    d760:	e92d4030 	push	{r4, r5, lr}
    d764:	e1a04000 	mov	r4, r0
    d768:	e5d00000 	ldrb	r0, [r0]
    d76c:	e594c034 	ldr	ip, [r4, #52]
    d770:	e3500002 	cmp	r0, #2	; 0x2
    d774:	e5d4e006 	ldrb	lr, [r4, #6]
    d778:	e5943030 	ldr	r3, [r4, #48]
    d77c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    d780:	e594202c 	ldr	r2, [r4, #44]
    d784:	03a00002 	moveq	r0, #2	; 0x2
    d788:	13a00000 	movne	r0, #0	; 0x0
    d78c:	e1a05001 	mov	r5, r1
    d790:	e5941028 	ldr	r1, [r4, #40]
    d794:	e88d5000 	stm	sp, {ip, lr}
    d798:	ebffff66 	bl	d538 <nf2_timing_init>
    d79c:	e3a00000 	mov	r0, #0	; 0x0
    d7a0:	ebffff01 	bl	d3ac <nf2_asyn_reset>
    d7a4:	e2503000 	subs	r3, r0, #0	; 0x0
    d7a8:	1a00000e 	bne	d7e8 <nf2_hardware_reset+0x88>
    d7ac:	e3550000 	cmp	r5, #0	; 0x0
    d7b0:	0a00000c 	beq	d7e8 <nf2_hardware_reset+0x88>
    d7b4:	e5d40000 	ldrb	r0, [r4]
    d7b8:	e3500001 	cmp	r0, #1	; 0x1
    d7bc:	1a000009 	bne	d7e8 <nf2_hardware_reset+0x88>
    d7c0:	e594c034 	ldr	ip, [r4, #52]
    d7c4:	e5d4e006 	ldrb	lr, [r4, #6]
    d7c8:	e2841028 	add	r1, r4, #40	; 0x28
    d7cc:	e891000e 	ldm	r1, {r1, r2, r3}
    d7d0:	e88d5000 	stm	sp, {ip, lr}
    d7d4:	ebffff57 	bl	d538 <nf2_timing_init>
    d7d8:	e3a00010 	mov	r0, #16	; 0x10
    d7dc:	e28dd00c 	add	sp, sp, #12	; 0xc
    d7e0:	e8bd4030 	pop	{r4, r5, lr}
    d7e4:	eaffff19 	b	d450 <nf2_active_sync>
    d7e8:	e1a00003 	mov	r0, r3
    d7ec:	e28dd00c 	add	sp, sp, #12	; 0xc
    d7f0:	e8bd8030 	pop	{r4, r5, pc}

0000d7f4 <serial_tstc>:
    d7f4:	e59f3010 	ldr	r3, [pc, #16]	; d80c <serial_tstc+0x18>
    d7f8:	e5930018 	ldr	r0, [r3, #24]
    d7fc:	e1a00220 	lsr	r0, r0, #4
    d800:	e2200001 	eor	r0, r0, #1	; 0x1
    d804:	e2000001 	and	r0, r0, #1	; 0x1
    d808:	e12fff1e 	bx	lr
    d80c:	20803000 	.word	0x20803000

0000d810 <serial_putc>:
    d810:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    d814:	e24dd004 	sub	sp, sp, #4	; 0x4
    d818:	e1a06000 	mov	r6, r0
    d81c:	eb00005d 	bl	d998 <get_ticks>
    d820:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    d824:	e3a05000 	mov	r5, #0	; 0x0
    d828:	e0944000 	adds	r4, r4, r0
    d82c:	e59f7034 	ldr	r7, [pc, #52]	; d868 <serial_putc+0x58>
    d830:	e0a55001 	adc	r5, r5, r1
    d834:	e5973018 	ldr	r3, [r7, #24]
    d838:	e3130020 	tst	r3, #32	; 0x20
    d83c:	0a000005 	beq	d858 <serial_putc+0x48>
    d840:	eb000054 	bl	d998 <get_ticks>
    d844:	e1510005 	cmp	r1, r5
    d848:	8a000002 	bhi	d858 <serial_putc+0x48>
    d84c:	1afffff8 	bne	d834 <serial_putc+0x24>
    d850:	e1500004 	cmp	r0, r4
    d854:	9afffff6 	bls	d834 <serial_putc+0x24>
    d858:	e59f3008 	ldr	r3, [pc, #8]	; d868 <serial_putc+0x58>
    d85c:	e5836000 	str	r6, [r3]
    d860:	e28dd004 	add	sp, sp, #4	; 0x4
    d864:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    d868:	20803000 	.word	0x20803000

0000d86c <serial_puts>:
    d86c:	e92d4030 	push	{r4, r5, lr}
    d870:	e1a05000 	mov	r5, r0
    d874:	e24dd004 	sub	sp, sp, #4	; 0x4
    d878:	e3a04000 	mov	r4, #0	; 0x0
    d87c:	ea000006 	b	d89c <serial_puts+0x30>
    d880:	e353000a 	cmp	r3, #10	; 0xa
    d884:	1a000001 	bne	d890 <serial_puts+0x24>
    d888:	e3a0000d 	mov	r0, #13	; 0xd
    d88c:	ebffffdf 	bl	d810 <serial_putc>
    d890:	e7d50004 	ldrb	r0, [r5, r4]
    d894:	ebffffdd 	bl	d810 <serial_putc>
    d898:	e2844001 	add	r4, r4, #1	; 0x1
    d89c:	e7d53004 	ldrb	r3, [r5, r4]
    d8a0:	e3530000 	cmp	r3, #0	; 0x0
    d8a4:	1afffff5 	bne	d880 <serial_puts+0x14>
    d8a8:	e28dd004 	add	sp, sp, #4	; 0x4
    d8ac:	e8bd8030 	pop	{r4, r5, pc}

0000d8b0 <serial_getc>:
    d8b0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    d8b4:	e59f2024 	ldr	r2, [pc, #36]	; d8e0 <serial_getc+0x30>
    d8b8:	e24dd004 	sub	sp, sp, #4	; 0x4
    d8bc:	e5923018 	ldr	r3, [r2, #24]
    d8c0:	e3130010 	tst	r3, #16	; 0x10
    d8c4:	1afffffc 	bne	d8bc <serial_getc+0xc>
    d8c8:	ebffdba9 	bl	4774 <tt_rnd_seed>
    d8cc:	e59f300c 	ldr	r3, [pc, #12]	; d8e0 <serial_getc+0x30>
    d8d0:	e5930000 	ldr	r0, [r3]
    d8d4:	e6ef0070 	uxtb	r0, r0
    d8d8:	e28dd004 	add	sp, sp, #4	; 0x4
    d8dc:	e8bd8000 	pop	{pc}
    d8e0:	20803000 	.word	0x20803000

0000d8e4 <serial_setbrg>:
    d8e4:	e92d4010 	push	{r4, lr}
    d8e8:	e59f0038 	ldr	r0, [pc, #56]	; d928 <serial_setbrg+0x44>
    d8ec:	ebffce15 	bl	1148 <module_get_clock>
    d8f0:	e59f1034 	ldr	r1, [pc, #52]	; d92c <serial_setbrg+0x48>
    d8f4:	e1a04000 	mov	r4, r0
    d8f8:	eb001c25 	bl	14994 <__aeabi_idiv>
    d8fc:	e6bf3070 	sxth	r3, r0
    d900:	e59f0028 	ldr	r0, [pc, #40]	; d930 <serial_setbrg+0x4c>
    d904:	e59f1020 	ldr	r1, [pc, #32]	; d92c <serial_setbrg+0x48>
    d908:	e0204390 	mla	r0, r0, r3, r4
    d90c:	e59f4020 	ldr	r4, [pc, #32]	; d934 <serial_setbrg+0x50>
    d910:	e1a00300 	lsl	r0, r0, #6
    d914:	e5843024 	str	r3, [r4, #36]
    d918:	eb001c1d 	bl	14994 <__aeabi_idiv>
    d91c:	e200003f 	and	r0, r0, #63	; 0x3f
    d920:	e5840028 	str	r0, [r4, #40]
    d924:	e8bd8010 	pop	{r4, pc}
    d928:	21e0a290 	.word	0x21e0a290
    d92c:	001c2000 	.word	0x001c2000
    d930:	ffe3e000 	.word	0xffe3e000
    d934:	20803000 	.word	0x20803000

0000d938 <init_serial>:
    d938:	e92d4010 	push	{r4, lr}
    d93c:	e59f004c 	ldr	r0, [pc, #76]	; d990 <init_serial+0x58>
    d940:	e59f404c 	ldr	r4, [pc, #76]	; d994 <init_serial+0x5c>
    d944:	ebffcb8d 	bl	780 <module_enable>
    d948:	e3a00013 	mov	r0, #19	; 0x13
    d94c:	ebffcbd1 	bl	898 <pads_init_module>
    d950:	e594302c 	ldr	r3, [r4, #44]
    d954:	e3c330ff 	bic	r3, r3, #255	; 0xff
    d958:	e3833060 	orr	r3, r3, #96	; 0x60
    d95c:	e584302c 	str	r3, [r4, #44]
    d960:	ebffffdf 	bl	d8e4 <serial_setbrg>
    d964:	e5943030 	ldr	r3, [r4, #48]
    d968:	e3a00000 	mov	r0, #0	; 0x0
    d96c:	e3c33cff 	bic	r3, r3, #65280	; 0xff00
    d970:	e3c33087 	bic	r3, r3, #135	; 0x87
    d974:	e3833c03 	orr	r3, r3, #768	; 0x300
    d978:	e3833001 	orr	r3, r3, #1	; 0x1
    d97c:	e5843030 	str	r3, [r4, #48]
    d980:	e594302c 	ldr	r3, [r4, #44]
    d984:	e3833010 	orr	r3, r3, #16	; 0x10
    d988:	e584302c 	str	r3, [r4, #44]
    d98c:	e8bd8010 	pop	{r4, pc}
    d990:	21e12000 	.word	0x21e12000
    d994:	20803000 	.word	0x20803000

0000d998 <get_ticks>:
    d998:	e59f1038 	ldr	r1, [pc, #56]	; d9d8 <get_ticks+0x40>
    d99c:	e5913204 	ldr	r3, [r1, #516]
    d9a0:	e5912200 	ldr	r2, [r1, #512]
    d9a4:	e591c204 	ldr	ip, [r1, #516]
    d9a8:	e15c0003 	cmp	ip, r3
    d9ac:	15913200 	ldrne	r3, [r1, #512]
    d9b0:	01a00002 	moveq	r0, r2
    d9b4:	03a01000 	moveq	r1, #0	; 0x0
    d9b8:	11a00003 	movne	r0, r3
    d9bc:	13a01000 	movne	r1, #0	; 0x0
    d9c0:	e3a02000 	mov	r2, #0	; 0x0
    d9c4:	e1822000 	orr	r2, r2, r0
    d9c8:	e18c3001 	orr	r3, ip, r1
    d9cc:	e1a01003 	mov	r1, r3
    d9d0:	e1a00002 	mov	r0, r2
    d9d4:	e12fff1e 	bx	lr
    d9d8:	21b00000 	.word	0x21b00000

0000d9dc <udelay>:
    d9dc:	e59f1084 	ldr	r1, [pc, #132]	; da68 <udelay+0x8c>
    d9e0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    d9e4:	e5913204 	ldr	r3, [r1, #516]
    d9e8:	e5912200 	ldr	r2, [r1, #512]
    d9ec:	e591c204 	ldr	ip, [r1, #516]
    d9f0:	e15c0003 	cmp	ip, r3
    d9f4:	15913200 	ldrne	r3, [r1, #512]
    d9f8:	01a04002 	moveq	r4, r2
    d9fc:	03a05000 	moveq	r5, #0	; 0x0
    da00:	11a04003 	movne	r4, r3
    da04:	13a05000 	movne	r5, #0	; 0x0
    da08:	e3a02000 	mov	r2, #0	; 0x0
    da0c:	e1822004 	orr	r2, r2, r4
    da10:	e18c3005 	orr	r3, ip, r5
    da14:	e0926000 	adds	r6, r2, r0
    da18:	e59fc048 	ldr	ip, [pc, #72]	; da68 <udelay+0x8c>
    da1c:	e2a37000 	adc	r7, r3, #0	; 0x0
    da20:	e59c3204 	ldr	r3, [ip, #516]
    da24:	e59c2200 	ldr	r2, [ip, #512]
    da28:	e59c1204 	ldr	r1, [ip, #516]
    da2c:	e1510003 	cmp	r1, r3
    da30:	159c3200 	ldrne	r3, [ip, #512]
    da34:	01a04002 	moveq	r4, r2
    da38:	03a05000 	moveq	r5, #0	; 0x0
    da3c:	11a04003 	movne	r4, r3
    da40:	13a05000 	movne	r5, #0	; 0x0
    da44:	e1851001 	orr	r1, r5, r1
    da48:	e3a02000 	mov	r2, #0	; 0x0
    da4c:	e1570001 	cmp	r7, r1
    da50:	e1840002 	orr	r0, r4, r2
    da54:	8afffff1 	bhi	da20 <udelay+0x44>
    da58:	18bd80f0 	popne	{r4, r5, r6, r7, pc}
    da5c:	e1560000 	cmp	r6, r0
    da60:	8affffee 	bhi	da20 <udelay+0x44>
    da64:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    da68:	21b00000 	.word	0x21b00000

0000da6c <get_timer>:
    da6c:	e59f2074 	ldr	r2, [pc, #116]	; dae8 <get_timer+0x7c>
    da70:	e92d4070 	push	{r4, r5, r6, lr}
    da74:	e5923204 	ldr	r3, [r2, #516]
    da78:	e1a06000 	mov	r6, r0
    da7c:	e5921200 	ldr	r1, [r2, #512]
    da80:	e5920204 	ldr	r0, [r2, #516]
    da84:	e24dd008 	sub	sp, sp, #8	; 0x8
    da88:	e1500003 	cmp	r0, r3
    da8c:	15923200 	ldrne	r3, [r2, #512]
    da90:	01a04001 	moveq	r4, r1
    da94:	03a05000 	moveq	r5, #0	; 0x0
    da98:	11a04003 	movne	r4, r3
    da9c:	13a05000 	movne	r5, #0	; 0x0
    daa0:	e1a03000 	mov	r3, r0
    daa4:	e3a02000 	mov	r2, #0	; 0x0
    daa8:	e1851003 	orr	r1, r5, r3
    daac:	e1840002 	orr	r0, r4, r2
    dab0:	e1510002 	cmp	r1, r2
    dab4:	e88d0003 	stm	sp, {r0, r1}
    dab8:	1a000003 	bne	dacc <get_timer+0x60>
    dabc:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    dac0:	eb001b31 	bl	1478c <__aeabi_uidiv>
    dac4:	e58d0000 	str	r0, [sp]
    dac8:	ea000002 	b	dad8 <get_timer+0x6c>
    dacc:	e1a0000d 	mov	r0, sp
    dad0:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    dad4:	eb000eeb 	bl	11688 <__div64_32>
    dad8:	e59d0000 	ldr	r0, [sp]
    dadc:	e0660000 	rsb	r0, r6, r0
    dae0:	e28dd008 	add	sp, sp, #8	; 0x8
    dae4:	e8bd8070 	pop	{r4, r5, r6, pc}
    dae8:	21b00000 	.word	0x21b00000

0000daec <init_timer>:
    daec:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    daf0:	e3a00003 	mov	r0, #3	; 0x3
    daf4:	e24dd004 	sub	sp, sp, #4	; 0x4
    daf8:	ebffcd92 	bl	1148 <module_get_clock>
    dafc:	e59f1030 	ldr	r1, [pc, #48]	; db34 <init_timer+0x48>
    db00:	eb001ba3 	bl	14994 <__aeabi_idiv>
    db04:	e2401001 	sub	r1, r0, #1	; 0x1
    db08:	e59f2028 	ldr	r2, [pc, #40]	; db38 <init_timer+0x4c>
    db0c:	e1a01401 	lsl	r1, r1, #8
    db10:	e3a03000 	mov	r3, #0	; 0x0
    db14:	e3811001 	orr	r1, r1, #1	; 0x1
    db18:	e5823208 	str	r3, [r2, #520]
    db1c:	e1a00003 	mov	r0, r3
    db20:	e5823200 	str	r3, [r2, #512]
    db24:	e5823204 	str	r3, [r2, #516]
    db28:	e5821208 	str	r1, [r2, #520]
    db2c:	e28dd004 	add	sp, sp, #4	; 0x4
    db30:	e8bd8000 	pop	{pc}
    db34:	000f4240 	.word	0x000f4240
    db38:	21b00000 	.word	0x21b00000

0000db3c <e2p_vs_align>:
    db3c:	e3a00010 	mov	r0, #16	; 0x10
    db40:	e12fff1e 	bx	lr

0000db44 <e2p_vs_reset>:
    db44:	e3a00000 	mov	r0, #0	; 0x0
    db48:	e12fff1e 	bx	lr

0000db4c <iic_address_num>:
    db4c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    db50:	e24dd004 	sub	sp, sp, #4	; 0x4
    db54:	ebffcaa6 	bl	5f4 <boot_info>
    db58:	e59f3014 	ldr	r3, [pc, #20]	; db74 <iic_address_num+0x28>
    db5c:	e1a00080 	lsl	r0, r0, #1
    db60:	e20000ff 	and	r0, r0, #255	; 0xff
    db64:	e5830000 	str	r0, [r3]
    db68:	e3a00001 	mov	r0, #1	; 0x1
    db6c:	e28dd004 	add	sp, sp, #4	; 0x4
    db70:	e8bd8000 	pop	{pc}
    db74:	3c00d214 	.word	0x3c00d214

0000db78 <iic_read_get_address>:
    db78:	e1d020b2 	ldrh	r2, [r0, #2]
    db7c:	e590300c 	ldr	r3, [r0, #12]
    db80:	e92d4010 	push	{r4, lr}
    db84:	e0833002 	add	r3, r3, r2
    db88:	e1c030b2 	strh	r3, [r0, #2]
    db8c:	e1a04000 	mov	r4, r0
    db90:	ebffffed 	bl	db4c <iic_address_num>
    db94:	e3500000 	cmp	r0, #0	; 0x0
    db98:	1a000008 	bne	dbc0 <iic_read_get_address+0x48>
    db9c:	e1d430b2 	ldrh	r3, [r4, #2]
    dba0:	e3530c01 	cmp	r3, #256	; 0x100
    dba4:	1a000005 	bne	dbc0 <iic_read_get_address+0x48>
    dba8:	e1d430b0 	ldrh	r3, [r4]
    dbac:	e1c400b2 	strh	r0, [r4, #2]
    dbb0:	e2833002 	add	r3, r3, #2	; 0x2
    dbb4:	e1c430b0 	strh	r3, [r4]
    dbb8:	e3a03001 	mov	r3, #1	; 0x1
    dbbc:	e1c431b0 	strh	r3, [r4, #16]
    dbc0:	ebffffe1 	bl	db4c <iic_address_num>
    dbc4:	e3500000 	cmp	r0, #0	; 0x0
    dbc8:	e5941008 	ldr	r1, [r4, #8]
    dbcc:	0a000004 	beq	dbe4 <iic_read_get_address+0x6c>
    dbd0:	e3510000 	cmp	r1, #0	; 0x0
    dbd4:	13a03000 	movne	r3, #0	; 0x0
    dbd8:	15843008 	strne	r3, [r4, #8]
    dbdc:	1a00000e 	bne	dc1c <iic_read_get_address+0xa4>
    dbe0:	ea000010 	b	dc28 <iic_read_get_address+0xb0>
    dbe4:	e1d430b2 	ldrh	r3, [r4, #2]
    dbe8:	e2632c01 	rsb	r2, r3, #256	; 0x100
    dbec:	e1520001 	cmp	r2, r1
    dbf0:	e5842004 	str	r2, [r4, #4]
    dbf4:	2a000005 	bcs	dc10 <iic_read_get_address+0x98>
    dbf8:	e0623001 	rsb	r3, r2, r1
    dbfc:	e5843008 	str	r3, [r4, #8]
    dc00:	e3a03001 	mov	r3, #1	; 0x1
    dc04:	e1c431b0 	strh	r3, [r4, #16]
    dc08:	e584200c 	str	r2, [r4, #12]
    dc0c:	ea000003 	b	dc20 <iic_read_get_address+0xa8>
    dc10:	e3510000 	cmp	r1, #0	; 0x0
    dc14:	0a000003 	beq	dc28 <iic_read_get_address+0xb0>
    dc18:	e5840008 	str	r0, [r4, #8]
    dc1c:	e584100c 	str	r1, [r4, #12]
    dc20:	e3a00001 	mov	r0, #1	; 0x1
    dc24:	e8bd8010 	pop	{r4, pc}
    dc28:	e3a00000 	mov	r0, #0	; 0x0
    dc2c:	e8bd8010 	pop	{r4, pc}

0000dc30 <iic_write_get_address>:
    dc30:	e1d020b2 	ldrh	r2, [r0, #2]
    dc34:	e590300c 	ldr	r3, [r0, #12]
    dc38:	e92d4010 	push	{r4, lr}
    dc3c:	e0833002 	add	r3, r3, r2
    dc40:	e1c030b2 	strh	r3, [r0, #2]
    dc44:	e1a04000 	mov	r4, r0
    dc48:	ebffffbf 	bl	db4c <iic_address_num>
    dc4c:	e3500000 	cmp	r0, #0	; 0x0
    dc50:	1a000008 	bne	dc78 <iic_write_get_address+0x48>
    dc54:	e1d430b2 	ldrh	r3, [r4, #2]
    dc58:	e3530c01 	cmp	r3, #256	; 0x100
    dc5c:	1a000005 	bne	dc78 <iic_write_get_address+0x48>
    dc60:	e1d430b0 	ldrh	r3, [r4]
    dc64:	e1c400b2 	strh	r0, [r4, #2]
    dc68:	e2833002 	add	r3, r3, #2	; 0x2
    dc6c:	e1c430b0 	strh	r3, [r4]
    dc70:	e3a03001 	mov	r3, #1	; 0x1
    dc74:	e1c431b0 	strh	r3, [r4, #16]
    dc78:	e1d430b2 	ldrh	r3, [r4, #2]
    dc7c:	e5940008 	ldr	r0, [r4, #8]
    dc80:	e203300f 	and	r3, r3, #15	; 0xf
    dc84:	e2632010 	rsb	r2, r3, #16	; 0x10
    dc88:	e1520000 	cmp	r2, r0
    dc8c:	e5842004 	str	r2, [r4, #4]
    dc90:	2a000004 	bcs	dca8 <iic_write_get_address+0x78>
    dc94:	e0623000 	rsb	r3, r2, r0
    dc98:	e5843008 	str	r3, [r4, #8]
    dc9c:	e584200c 	str	r2, [r4, #12]
    dca0:	e3a00001 	mov	r0, #1	; 0x1
    dca4:	e8bd8010 	pop	{r4, pc}
    dca8:	e3500000 	cmp	r0, #0	; 0x0
    dcac:	13a03000 	movne	r3, #0	; 0x0
    dcb0:	1584000c 	strne	r0, [r4, #12]
    dcb4:	15843008 	strne	r3, [r4, #8]
    dcb8:	13a00001 	movne	r0, #1	; 0x1
    dcbc:	e8bd8010 	pop	{r4, pc}

0000dcc0 <iic_word_addr_func>:
    dcc0:	e92d4030 	push	{r4, r5, lr}
    dcc4:	e24dd004 	sub	sp, sp, #4	; 0x4
    dcc8:	e1a04000 	mov	r4, r0
    dccc:	e1a05001 	mov	r5, r1
    dcd0:	ebffff9d 	bl	db4c <iic_address_num>
    dcd4:	e3500000 	cmp	r0, #0	; 0x0
    dcd8:	16ff0074 	uxthne	r0, r4
    dcdc:	1a00000c 	bne	dd14 <iic_word_addr_func+0x54>
    dce0:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    dce4:	e0051003 	and	r1, r5, r3
    dce8:	e3a020ff 	mov	r2, #255	; 0xff
    dcec:	e3510000 	cmp	r1, #0	; 0x0
    dcf0:	e0040002 	and	r0, r4, r2
    dcf4:	aa000005 	bge	dd10 <iic_word_addr_func+0x50>
    dcf8:	e3e02000 	mvn	r2, #0	; 0x0
    dcfc:	e0900002 	adds	r0, r0, r2
    dd00:	e3e020ff 	mvn	r2, #255	; 0xff
    dd04:	e1800002 	orr	r0, r0, r2
    dd08:	e3a02001 	mov	r2, #1	; 0x1
    dd0c:	e0900002 	adds	r0, r0, r2
    dd10:	e6ff0070 	uxth	r0, r0
    dd14:	e28dd004 	add	sp, sp, #4	; 0x4
    dd18:	e8bd8030 	pop	{r4, r5, pc}

0000dd1c <iic_addr_func>:
    dd1c:	e92d4030 	push	{r4, r5, lr}
    dd20:	e24dd004 	sub	sp, sp, #4	; 0x4
    dd24:	e1a04000 	mov	r4, r0
    dd28:	e1a05001 	mov	r5, r1
    dd2c:	ebffff86 	bl	db4c <iic_address_num>
    dd30:	e3500000 	cmp	r0, #0	; 0x0
    dd34:	e59f003c 	ldr	r0, [pc, #60]	; dd78 <iic_addr_func+0x5c>
    dd38:	11d000b0 	ldrhne	r0, [r0]
    dd3c:	1a00000b 	bne	dd70 <iic_addr_func+0x54>
    dd40:	e1a02005 	mov	r2, r5
    dd44:	e1a03fc2 	asr	r3, r2, #31
    dd48:	e1a02fc3 	asr	r2, r3, #31
    dd4c:	e20220ff 	and	r2, r2, #255	; 0xff
    dd50:	e3a03000 	mov	r3, #0	; 0x0
    dd54:	e0922004 	adds	r2, r2, r4
    dd58:	e0a33005 	adc	r3, r3, r5
    dd5c:	e5900000 	ldr	r0, [r0]
    dd60:	e1a01422 	lsr	r1, r2, #8
    dd64:	e1811c03 	orr	r1, r1, r3, lsl #24
    dd68:	e1800081 	orr	r0, r0, r1, lsl #1
    dd6c:	e6ff0070 	uxth	r0, r0
    dd70:	e28dd004 	add	sp, sp, #4	; 0x4
    dd74:	e8bd8030 	pop	{r4, r5, pc}
    dd78:	3c00d214 	.word	0x3c00d214

0000dd7c <e2p_vs_write>:
    dd7c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    dd80:	e1a01003 	mov	r1, r3
    dd84:	e24dd024 	sub	sp, sp, #36	; 0x24
    dd88:	e1a06000 	mov	r6, r0
    dd8c:	e1a00002 	mov	r0, r2
    dd90:	e1a05003 	mov	r5, r3
    dd94:	e1a04002 	mov	r4, r2
    dd98:	ebffffdf 	bl	dd1c <iic_addr_func>
    dd9c:	e1a01005 	mov	r1, r5
    dda0:	e1cd00bc 	strh	r0, [sp, #12]
    dda4:	e1a00004 	mov	r0, r4
    dda8:	ebffffc4 	bl	dcc0 <iic_word_addr_func>
    ddac:	e3a03000 	mov	r3, #0	; 0x0
    ddb0:	e59d2048 	ldr	r2, [sp, #72]
    ddb4:	e1a05003 	mov	r5, r3
    ddb8:	e58d3018 	str	r3, [sp, #24]
    ddbc:	e28dc00c 	add	ip, sp, #12	; 0xc
    ddc0:	e3a03001 	mov	r3, #1	; 0x1
    ddc4:	e1cd00be 	strh	r0, [sp, #14]
    ddc8:	e58d2014 	str	r2, [sp, #20]
    ddcc:	e1cd31bc 	strh	r3, [sp, #28]
    ddd0:	e59fb10c 	ldr	fp, [pc, #268]	; dee4 <e2p_vs_write+0x168>
    ddd4:	e3a0a001 	mov	sl, #1	; 0x1
    ddd8:	e1a08005 	mov	r8, r5
    dddc:	e28c9012 	add	r9, ip, #18	; 0x12
    dde0:	ea000037 	b	dec4 <e2p_vs_write+0x148>
    dde4:	ebffff58 	bl	db4c <iic_address_num>
    dde8:	e3500000 	cmp	r0, #0	; 0x0
    ddec:	e1dd10be 	ldrh	r1, [sp, #14]
    ddf0:	11a03421 	lsrne	r3, r1, #8
    ddf4:	15cd301e 	strbne	r3, [sp, #30]
    ddf8:	15cd101f 	strbne	r1, [sp, #31]
    ddfc:	13a07002 	movne	r7, #2	; 0x2
    de00:	1a000005 	bne	de1c <e2p_vs_write+0xa0>
    de04:	e6ef1071 	uxtb	r1, r1
    de08:	e5cd101e 	strb	r1, [sp, #30]
    de0c:	e59f00d4 	ldr	r0, [pc, #212]	; dee8 <e2p_vs_write+0x16c>
    de10:	e1a0e00f 	mov	lr, pc
    de14:	e59bf000 	ldr	pc, [fp]
    de18:	e3a07001 	mov	r7, #1	; 0x1
    de1c:	ebfffedd 	bl	d998 <get_ticks>
    de20:	e1dd31bc 	ldrh	r3, [sp, #28]
    de24:	e1a04000 	mov	r4, r0
    de28:	e3530000 	cmp	r3, #0	; 0x0
    de2c:	0a000016 	beq	de8c <e2p_vs_write+0x110>
    de30:	e3a0c000 	mov	ip, #0	; 0x0
    de34:	e5dd200c 	ldrb	r2, [sp, #12]
    de38:	e1a00008 	mov	r0, r8
    de3c:	e1a0100a 	mov	r1, sl
    de40:	e1a03008 	mov	r3, r8
    de44:	e1cdc1bc 	strh	ip, [sp, #28]
    de48:	e58da000 	str	sl, [sp]
    de4c:	e58d8004 	str	r8, [sp, #4]
    de50:	eb000137 	bl	e334 <iic_init>
    de54:	e3500000 	cmp	r0, #0	; 0x0
    de58:	1a00000b 	bne	de8c <e2p_vs_write+0x110>
    de5c:	e1a00008 	mov	r0, r8
    de60:	ea00001d 	b	dedc <e2p_vs_write+0x160>
    de64:	ebfffecb 	bl	d998 <get_ticks>
    de68:	e1540000 	cmp	r4, r0
    de6c:	2a000008 	bcs	de94 <e2p_vs_write+0x118>
    de70:	e59f0074 	ldr	r0, [pc, #116]	; deec <e2p_vs_write+0x170>
    de74:	e1a0e00f 	mov	lr, pc
    de78:	e59bf000 	ldr	pc, [fp]
    de7c:	e59d3018 	ldr	r3, [sp, #24]
    de80:	e0853003 	add	r3, r5, r3
    de84:	e6ff5073 	uxth	r5, r3
    de88:	ea00000b 	b	debc <e2p_vs_write+0x140>
    de8c:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    de90:	e2844e12 	add	r4, r4, #288	; 0x120
    de94:	e59dc018 	ldr	ip, [sp, #24]
    de98:	e1a01009 	mov	r1, r9
    de9c:	e1a02007 	mov	r2, r7
    dea0:	e1a03006 	mov	r3, r6
    dea4:	e3a00000 	mov	r0, #0	; 0x0
    dea8:	e58dc000 	str	ip, [sp]
    deac:	e58da004 	str	sl, [sp, #4]
    deb0:	eb000078 	bl	e098 <iicreg_write>
    deb4:	e3500000 	cmp	r0, #0	; 0x0
    deb8:	0affffe9 	beq	de64 <e2p_vs_write+0xe8>
    debc:	e59d3018 	ldr	r3, [sp, #24]
    dec0:	e0866003 	add	r6, r6, r3
    dec4:	e28d000c 	add	r0, sp, #12	; 0xc
    dec8:	ebffff58 	bl	dc30 <iic_write_get_address>
    decc:	e3500000 	cmp	r0, #0	; 0x0
    ded0:	1affffc3 	bne	dde4 <e2p_vs_write+0x68>
    ded4:	e59d2048 	ldr	r2, [sp, #72]
    ded8:	e0650002 	rsb	r0, r5, r2
    dedc:	e28dd024 	add	sp, sp, #36	; 0x24
    dee0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    dee4:	3c00d6cc 	.word	0x3c00d6cc
    dee8:	00018837 	.word	0x00018837
    deec:	00018841 	.word	0x00018841

0000def0 <e2p_vs_read>:
    def0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    def4:	e1a01003 	mov	r1, r3
    def8:	e24dd024 	sub	sp, sp, #36	; 0x24
    defc:	e1a06000 	mov	r6, r0
    df00:	e1a00002 	mov	r0, r2
    df04:	e1a05003 	mov	r5, r3
    df08:	e1a04002 	mov	r4, r2
    df0c:	e59d7048 	ldr	r7, [sp, #72]
    df10:	ebffff81 	bl	dd1c <iic_addr_func>
    df14:	e1a01005 	mov	r1, r5
    df18:	e1cd00bc 	strh	r0, [sp, #12]
    df1c:	e1a00004 	mov	r0, r4
    df20:	ebffff66 	bl	dcc0 <iic_word_addr_func>
    df24:	e28db00c 	add	fp, sp, #12	; 0xc
    df28:	e3a03000 	mov	r3, #0	; 0x0
    df2c:	e3a02001 	mov	r2, #1	; 0x1
    df30:	e1cd00be 	strh	r0, [sp, #14]
    df34:	e1cd21bc 	strh	r2, [sp, #28]
    df38:	e58d7014 	str	r7, [sp, #20]
    df3c:	e58d3018 	str	r3, [sp, #24]
    df40:	e1a05003 	mov	r5, r3
    df44:	e3a09001 	mov	r9, #1	; 0x1
    df48:	e28ba012 	add	sl, fp, #18	; 0x12
    df4c:	ea00002b 	b	e000 <e2p_vs_read+0x110>
    df50:	ebfffefd 	bl	db4c <iic_address_num>
    df54:	e1dd20be 	ldrh	r2, [sp, #14]
    df58:	e3500000 	cmp	r0, #0	; 0x0
    df5c:	11a03422 	lsrne	r3, r2, #8
    df60:	15cd301e 	strbne	r3, [sp, #30]
    df64:	15cd201f 	strbne	r2, [sp, #31]
    df68:	13a08002 	movne	r8, #2	; 0x2
    df6c:	05cd201e 	strbeq	r2, [sp, #30]
    df70:	03a08001 	moveq	r8, #1	; 0x1
    df74:	ebfffe87 	bl	d998 <get_ticks>
    df78:	e1dd31bc 	ldrh	r3, [sp, #28]
    df7c:	e1a04000 	mov	r4, r0
    df80:	e3530000 	cmp	r3, #0	; 0x0
    df84:	0a000010 	beq	dfcc <e2p_vs_read+0xdc>
    df88:	e3a0c000 	mov	ip, #0	; 0x0
    df8c:	e5dd200c 	ldrb	r2, [sp, #12]
    df90:	e1a00005 	mov	r0, r5
    df94:	e1a01009 	mov	r1, r9
    df98:	e1a03005 	mov	r3, r5
    df9c:	e1cdc1bc 	strh	ip, [sp, #28]
    dfa0:	e58d9000 	str	r9, [sp]
    dfa4:	e58d5004 	str	r5, [sp, #4]
    dfa8:	eb0000e1 	bl	e334 <iic_init>
    dfac:	e3500000 	cmp	r0, #0	; 0x0
    dfb0:	1a000005 	bne	dfcc <e2p_vs_read+0xdc>
    dfb4:	e1a07005 	mov	r7, r5
    dfb8:	ea000014 	b	e010 <e2p_vs_read+0x120>
    dfbc:	ebfffe75 	bl	d998 <get_ticks>
    dfc0:	e1540000 	cmp	r4, r0
    dfc4:	2a000002 	bcs	dfd4 <e2p_vs_read+0xe4>
    dfc8:	ea00000a 	b	dff8 <e2p_vs_read+0x108>
    dfcc:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    dfd0:	e2844e12 	add	r4, r4, #288	; 0x120
    dfd4:	e59dc018 	ldr	ip, [sp, #24]
    dfd8:	e1a0100a 	mov	r1, sl
    dfdc:	e1a02008 	mov	r2, r8
    dfe0:	e1a03006 	mov	r3, r6
    dfe4:	e3a00000 	mov	r0, #0	; 0x0
    dfe8:	e58dc000 	str	ip, [sp]
    dfec:	eb000061 	bl	e178 <iic_read_func>
    dff0:	e3500000 	cmp	r0, #0	; 0x0
    dff4:	0afffff0 	beq	dfbc <e2p_vs_read+0xcc>
    dff8:	e59d3018 	ldr	r3, [sp, #24]
    dffc:	e0866003 	add	r6, r6, r3
    e000:	e1a0000b 	mov	r0, fp
    e004:	ebfffedb 	bl	db78 <iic_read_get_address>
    e008:	e3500000 	cmp	r0, #0	; 0x0
    e00c:	1affffcf 	bne	df50 <e2p_vs_read+0x60>
    e010:	e1a00007 	mov	r0, r7
    e014:	e28dd024 	add	sp, sp, #36	; 0x24
    e018:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

0000e01c <iicreg_read>:
    e01c:	e3500001 	cmp	r0, #1	; 0x1
    e020:	e92d4070 	push	{r4, r5, r6, lr}
    e024:	83a00000 	movhi	r0, #0	; 0x0
    e028:	e1a05001 	mov	r5, r1
    e02c:	e1a04002 	mov	r4, r2
    e030:	88bd8070 	pophi	{r4, r5, r6, pc}
    e034:	e59f3058 	ldr	r3, [pc, #88]	; e094 <iicreg_read+0x78>
    e038:	e5932070 	ldr	r2, [r3, #112]
    e03c:	e2022008 	and	r2, r2, #8	; 0x8
    e040:	ea000000 	b	e048 <iicreg_read+0x2c>
    e044:	e5931010 	ldr	r1, [r3, #16]
    e048:	e3520000 	cmp	r2, #0	; 0x0
    e04c:	1afffffc 	bne	e044 <iicreg_read+0x28>
    e050:	e59f103c 	ldr	r1, [pc, #60]	; e094 <iicreg_read+0x78>
    e054:	e1a00004 	mov	r0, r4
    e058:	e3a06c01 	mov	r6, #256	; 0x100
    e05c:	e591c070 	ldr	ip, [r1, #112]
    e060:	e2503000 	subs	r3, r0, #0	; 0x0
    e064:	13a03001 	movne	r3, #1	; 0x1
    e068:	e01330ac 	ands	r3, r3, ip, lsr #1
    e06c:	15816010 	strne	r6, [r1, #16]
    e070:	12400001 	subne	r0, r0, #1	; 0x1
    e074:	e31c0008 	tst	ip, #8	; 0x8
    e078:	15913010 	ldrne	r3, [r1, #16]
    e07c:	12822001 	addne	r2, r2, #1	; 0x1
    e080:	14c53001 	strbne	r3, [r5], #1
    e084:	e1520004 	cmp	r2, r4
    e088:	1afffff3 	bne	e05c <iicreg_read+0x40>
    e08c:	e3a00001 	mov	r0, #1	; 0x1
    e090:	e8bd8070 	pop	{r4, r5, r6, pc}
    e094:	20300000 	.word	0x20300000

0000e098 <iicreg_write>:
    e098:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    e09c:	e59d4018 	ldr	r4, [sp, #24]
    e0a0:	e3500001 	cmp	r0, #1	; 0x1
    e0a4:	e1a05001 	mov	r5, r1
    e0a8:	e5dd801c 	ldrb	r8, [sp, #28]
    e0ac:	e1a01003 	mov	r1, r3
    e0b0:	e0847002 	add	r7, r4, r2
    e0b4:	8a00002c 	bhi	e16c <iicreg_write+0xd4>
    e0b8:	e59f00b4 	ldr	r0, [pc, #180]	; e174 <iicreg_write+0xdc>
    e0bc:	e5903034 	ldr	r3, [r0, #52]
    e0c0:	e213c040 	ands	ip, r3, #64	; 0x40
    e0c4:	15903080 	ldrne	r3, [r0, #128]
    e0c8:	15903054 	ldrne	r3, [r0, #84]
    e0cc:	13a00000 	movne	r0, #0	; 0x0
    e0d0:	18bd81f0 	popne	{r4, r5, r6, r7, r8, pc}
    e0d4:	e5903070 	ldr	r3, [r0, #112]
    e0d8:	e3130004 	tst	r3, #4	; 0x4
    e0dc:	11a06000 	movne	r6, r0
    e0e0:	11a0000c 	movne	r0, ip
    e0e4:	11a0c006 	movne	ip, r6
    e0e8:	1a00000d 	bne	e124 <iicreg_write+0x8c>
    e0ec:	ea00001e 	b	e16c <iicreg_write+0xd4>
    e0f0:	e5963070 	ldr	r3, [r6, #112]
    e0f4:	e3130002 	tst	r3, #2	; 0x2
    e0f8:	0afffffc 	beq	e0f0 <iicreg_write+0x58>
    e0fc:	e3520000 	cmp	r2, #0	; 0x0
    e100:	14d53001 	ldrbne	r3, [r5], #1
    e104:	12422001 	subne	r2, r2, #1	; 0x1
    e108:	1a000003 	bne	e11c <iicreg_write+0x84>
    e10c:	e3540000 	cmp	r4, #0	; 0x0
    e110:	0a000002 	beq	e120 <iicreg_write+0x88>
    e114:	e4d13001 	ldrb	r3, [r1], #1
    e118:	e2444001 	sub	r4, r4, #1	; 0x1
    e11c:	e58c3010 	str	r3, [ip, #16]
    e120:	e2800001 	add	r0, r0, #1	; 0x1
    e124:	e1500007 	cmp	r0, r7
    e128:	3afffff0 	bcc	e0f0 <iicreg_write+0x58>
    e12c:	e59f2040 	ldr	r2, [pc, #64]	; e174 <iicreg_write+0xdc>
    e130:	e5923070 	ldr	r3, [r2, #112]
    e134:	e3130004 	tst	r3, #4	; 0x4
    e138:	0afffffc 	beq	e130 <iicreg_write+0x98>
    e13c:	e3580000 	cmp	r8, #0	; 0x0
    e140:	159f202c 	ldrne	r2, [pc, #44]	; e174 <iicreg_write+0xdc>
    e144:	0a000006 	beq	e164 <iicreg_write+0xcc>
    e148:	e5923034 	ldr	r3, [r2, #52]
    e14c:	e3130c02 	tst	r3, #512	; 0x200
    e150:	0afffffc 	beq	e148 <iicreg_write+0xb0>
    e154:	e59f3018 	ldr	r3, [pc, #24]	; e174 <iicreg_write+0xdc>
    e158:	e3010964 	movw	r0, #6500	; 0x1964
    e15c:	e5933060 	ldr	r3, [r3, #96]
    e160:	ebfffe1d 	bl	d9dc <udelay>
    e164:	e3a00001 	mov	r0, #1	; 0x1
    e168:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    e16c:	e3a00000 	mov	r0, #0	; 0x0
    e170:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    e174:	20300000 	.word	0x20300000

0000e178 <iic_read_func>:
    e178:	e92d4070 	push	{r4, r5, r6, lr}
    e17c:	e59f4080 	ldr	r4, [pc, #128]	; e204 <iic_read_func+0x8c>
    e180:	e1a06003 	mov	r6, r3
    e184:	e5d43000 	ldrb	r3, [r4]
    e188:	e24dd008 	sub	sp, sp, #8	; 0x8
    e18c:	e243300a 	sub	r3, r3, #10	; 0xa
    e190:	e6ef3073 	uxtb	r3, r3
    e194:	e3530001 	cmp	r3, #1	; 0x1
    e198:	e1a05000 	mov	r5, r0
    e19c:	9a00000b 	bls	e1d0 <iic_read_func+0x58>
    e1a0:	e3a0e000 	mov	lr, #0	; 0x0
    e1a4:	e1a0300e 	mov	r3, lr
    e1a8:	e3a0c001 	mov	ip, #1	; 0x1
    e1ac:	e58dc004 	str	ip, [sp, #4]
    e1b0:	e58de000 	str	lr, [sp]
    e1b4:	ebffffb7 	bl	e098 <iicreg_write>
    e1b8:	e3500000 	cmp	r0, #0	; 0x0
    e1bc:	03a0300c 	moveq	r3, #12	; 0xc
    e1c0:	05c43000 	strbeq	r3, [r4]
    e1c4:	0a00000c 	beq	e1fc <iic_read_func+0x84>
    e1c8:	e3a0300a 	mov	r3, #10	; 0xa
    e1cc:	e5c43000 	strb	r3, [r4]
    e1d0:	e59d2018 	ldr	r2, [sp, #24]
    e1d4:	e1a00005 	mov	r0, r5
    e1d8:	e1a01006 	mov	r1, r6
    e1dc:	ebffff8e 	bl	e01c <iicreg_read>
    e1e0:	e59f201c 	ldr	r2, [pc, #28]	; e204 <iic_read_func+0x8c>
    e1e4:	e3500000 	cmp	r0, #0	; 0x0
    e1e8:	13a0300d 	movne	r3, #13	; 0xd
    e1ec:	03a0300b 	moveq	r3, #11	; 0xb
    e1f0:	15c23000 	strbne	r3, [r2]
    e1f4:	13a00001 	movne	r0, #1	; 0x1
    e1f8:	05c23000 	strbeq	r3, [r2]
    e1fc:	e28dd008 	add	sp, sp, #8	; 0x8
    e200:	e8bd8070 	pop	{r4, r5, r6, pc}
    e204:	3c00d218 	.word	0x3c00d218

0000e208 <iic_writeread>:
    e208:	e92d4070 	push	{r4, r5, r6, lr}
    e20c:	e59f4088 	ldr	r4, [pc, #136]	; e29c <iic_writeread+0x94>
    e210:	e24dd010 	sub	sp, sp, #16	; 0x10
    e214:	e5d43000 	ldrb	r3, [r4]
    e218:	e5cd100f 	strb	r1, [sp, #15]
    e21c:	e243300a 	sub	r3, r3, #10	; 0xa
    e220:	e6ef3073 	uxtb	r3, r3
    e224:	e3530001 	cmp	r3, #1	; 0x1
    e228:	e1a06002 	mov	r6, r2
    e22c:	e1a05000 	mov	r5, r0
    e230:	9a00000c 	bls	e268 <iic_writeread+0x60>
    e234:	e3a0c000 	mov	ip, #0	; 0x0
    e238:	e1a0300c 	mov	r3, ip
    e23c:	e28d100f 	add	r1, sp, #15	; 0xf
    e240:	e3a02001 	mov	r2, #1	; 0x1
    e244:	e58dc000 	str	ip, [sp]
    e248:	e58dc004 	str	ip, [sp, #4]
    e24c:	ebffff91 	bl	e098 <iicreg_write>
    e250:	e3500000 	cmp	r0, #0	; 0x0
    e254:	03a0300c 	moveq	r3, #12	; 0xc
    e258:	05c43000 	strbeq	r3, [r4]
    e25c:	0a00000c 	beq	e294 <iic_writeread+0x8c>
    e260:	e3a0300a 	mov	r3, #10	; 0xa
    e264:	e5c43000 	strb	r3, [r4]
    e268:	e3a02001 	mov	r2, #1	; 0x1
    e26c:	e1a00005 	mov	r0, r5
    e270:	e1a01006 	mov	r1, r6
    e274:	ebffff68 	bl	e01c <iicreg_read>
    e278:	e59f201c 	ldr	r2, [pc, #28]	; e29c <iic_writeread+0x94>
    e27c:	e3500000 	cmp	r0, #0	; 0x0
    e280:	13a0300d 	movne	r3, #13	; 0xd
    e284:	03a0300b 	moveq	r3, #11	; 0xb
    e288:	15c23000 	strbne	r3, [r2]
    e28c:	13a00001 	movne	r0, #1	; 0x1
    e290:	05c23000 	strbeq	r3, [r2]
    e294:	e28dd010 	add	sp, sp, #16	; 0x10
    e298:	e8bd8070 	pop	{r4, r5, r6, pc}
    e29c:	3c00d218 	.word	0x3c00d218

0000e2a0 <iicreg_init>:
    e2a0:	e92d4030 	push	{r4, r5, lr}
    e2a4:	e1a04000 	mov	r4, r0
    e2a8:	e24dd004 	sub	sp, sp, #4	; 0x4
    e2ac:	e59f0074 	ldr	r0, [pc, #116]	; e328 <iicreg_init+0x88>
    e2b0:	e1a05002 	mov	r5, r2
    e2b4:	ebffcba3 	bl	1148 <module_get_clock>
    e2b8:	e3540001 	cmp	r4, #1	; 0x1
    e2bc:	83a00000 	movhi	r0, #0	; 0x0
    e2c0:	8a000016 	bhi	e320 <iicreg_init+0x80>
    e2c4:	e59f1060 	ldr	r1, [pc, #96]	; e32c <iicreg_init+0x8c>
    e2c8:	eb00192f 	bl	1478c <__aeabi_uidiv>
    e2cc:	e59f205c 	ldr	r2, [pc, #92]	; e330 <iicreg_init+0x90>
    e2d0:	e1a010a0 	lsr	r1, r0, #1
    e2d4:	e3a0c000 	mov	ip, #0	; 0x0
    e2d8:	e3a03043 	mov	r3, #67	; 0x43
    e2dc:	e582c06c 	str	ip, [r2, #108]
    e2e0:	e5823000 	str	r3, [r2]
    e2e4:	e5820014 	str	r0, [r2, #20]
    e2e8:	e5820018 	str	r0, [r2, #24]
    e2ec:	e58210a0 	str	r1, [r2, #160]
    e2f0:	e59230a0 	ldr	r3, [r2, #160]
    e2f4:	e1a000a5 	lsr	r0, r5, #1
    e2f8:	e3c33c01 	bic	r3, r3, #256	; 0x100
    e2fc:	e58230a0 	str	r3, [r2, #160]
    e300:	e5923000 	ldr	r3, [r2]
    e304:	e3a01001 	mov	r1, #1	; 0x1
    e308:	e3833020 	orr	r3, r3, #32	; 0x20
    e30c:	e5823000 	str	r3, [r2]
    e310:	e582c030 	str	ip, [r2, #48]
    e314:	e5820004 	str	r0, [r2, #4]
    e318:	e582106c 	str	r1, [r2, #108]
    e31c:	e1a00001 	mov	r0, r1
    e320:	e28dd004 	add	sp, sp, #4	; 0x4
    e324:	e8bd8030 	pop	{r4, r5, pc}
    e328:	21e0a360 	.word	0x21e0a360
    e32c:	0009c400 	.word	0x0009c400
    e330:	20300000 	.word	0x20300000

0000e334 <iic_init>:
    e334:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    e338:	e59fc054 	ldr	ip, [pc, #84]	; e394 <iic_init+0x60>
    e33c:	e24dd004 	sub	sp, sp, #4	; 0x4
    e340:	e3a0e00d 	mov	lr, #13	; 0xd
    e344:	e1a0a000 	mov	sl, r0
    e348:	e59f0048 	ldr	r0, [pc, #72]	; e398 <iic_init+0x64>
    e34c:	e1a04001 	mov	r4, r1
    e350:	e1a05002 	mov	r5, r2
    e354:	e1a06003 	mov	r6, r3
    e358:	e5cce000 	strb	lr, [ip]
    e35c:	e5dd7020 	ldrb	r7, [sp, #32]
    e360:	e5dd8024 	ldrb	r8, [sp, #36]
    e364:	ebffc905 	bl	780 <module_enable>
    e368:	e3a0000a 	mov	r0, #10	; 0xa
    e36c:	ebffc949 	bl	898 <pads_init_module>
    e370:	e1a0000a 	mov	r0, sl
    e374:	e1a01004 	mov	r1, r4
    e378:	e1a02005 	mov	r2, r5
    e37c:	e1a03006 	mov	r3, r6
    e380:	e58d7020 	str	r7, [sp, #32]
    e384:	e58d8024 	str	r8, [sp, #36]
    e388:	e28dd004 	add	sp, sp, #4	; 0x4
    e38c:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    e390:	eaffffc2 	b	e2a0 <iicreg_init>
    e394:	3c00d218 	.word	0x3c00d218
    e398:	21e10c00 	.word	0x21e10c00

0000e39c <spi_set_bytes>:
    e39c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    e3a0:	e59f3020 	ldr	r3, [pc, #32]	; e3c8 <spi_set_bytes+0x2c>
    e3a4:	e1a01000 	mov	r1, r0
    e3a8:	e5830000 	str	r0, [r3]
    e3ac:	e24dd004 	sub	sp, sp, #4	; 0x4
    e3b0:	e59f0014 	ldr	r0, [pc, #20]	; e3cc <spi_set_bytes+0x30>
    e3b4:	e59f3014 	ldr	r3, [pc, #20]	; e3d0 <spi_set_bytes+0x34>
    e3b8:	e1a0e00f 	mov	lr, pc
    e3bc:	e593f000 	ldr	pc, [r3]
    e3c0:	e28dd004 	add	sp, sp, #4	; 0x4
    e3c4:	e8bd8000 	pop	{pc}
    e3c8:	3c00d0b0 	.word	0x3c00d0b0
    e3cc:	00018848 	.word	0x00018848
    e3d0:	3c00d6cc 	.word	0x3c00d6cc

0000e3d4 <spi_fifo_clear>:
    e3d4:	e59f2028 	ldr	r2, [pc, #40]	; e404 <spi_fifo_clear+0x30>
    e3d8:	e592300c 	ldr	r3, [r2, #12]
    e3dc:	e3130010 	tst	r3, #16	; 0x10
    e3e0:	1afffffc 	bne	e3d8 <spi_fifo_clear+0x4>
    e3e4:	ea000001 	b	e3f0 <spi_fifo_clear+0x1c>
    e3e8:	e5923008 	ldr	r3, [r2, #8]
    e3ec:	ea000000 	b	e3f4 <spi_fifo_clear+0x20>
    e3f0:	e59f200c 	ldr	r2, [pc, #12]	; e404 <spi_fifo_clear+0x30>
    e3f4:	e592300c 	ldr	r3, [r2, #12]
    e3f8:	e3130004 	tst	r3, #4	; 0x4
    e3fc:	1afffff9 	bne	e3e8 <spi_fifo_clear+0x14>
    e400:	e12fff1e 	bx	lr
    e404:	20500000 	.word	0x20500000

0000e408 <flash_vs_align>:
    e408:	e3a00c01 	mov	r0, #256	; 0x100
    e40c:	e12fff1e 	bx	lr

0000e410 <spi_exchange>:
    e410:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    e414:	e5905008 	ldr	r5, [r0, #8]
    e418:	e3510001 	cmp	r1, #1	; 0x1
    e41c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    e420:	e1a0a000 	mov	sl, r0
    e424:	e5908004 	ldr	r8, [r0, #4]
    e428:	d1a04005 	movle	r4, r5
    e42c:	d3a07000 	movle	r7, #0	; 0x0
    e430:	da000020 	ble	e4b8 <spi_exchange+0xa8>
    e434:	e280a00c 	add	sl, r0, #12	; 0xc
    e438:	e1a01008 	mov	r1, r8
    e43c:	e1a0000d 	mov	r0, sp
    e440:	e1a02005 	mov	r2, r5
    e444:	ebffcc42 	bl	1554 <memcpy>
    e448:	e99a0012 	ldmib	sl, {r1, r4}
    e44c:	e0843005 	add	r3, r4, r5
    e450:	e3530008 	cmp	r3, #8	; 0x8
    e454:	a3a03008 	movge	r3, #8	; 0x8
    e458:	e6ef6073 	uxtb	r6, r3
    e45c:	e3510000 	cmp	r1, #0	; 0x0
    e460:	e1a0700d 	mov	r7, sp
    e464:	059a8000 	ldreq	r8, [sl]
    e468:	00667084 	rsbeq	r7, r6, r4, lsl #1
    e46c:	0a000008 	beq	e494 <spi_exchange+0x84>
    e470:	e0654006 	rsb	r4, r5, r6
    e474:	e0870005 	add	r0, r7, r5
    e478:	e1a02004 	mov	r2, r4
    e47c:	ebffcc34 	bl	1554 <memcpy>
    e480:	e99a000c 	ldmib	sl, {r2, r3}
    e484:	e0855003 	add	r5, r5, r3
    e488:	e0828004 	add	r8, r2, r4
    e48c:	e3a07000 	mov	r7, #0	; 0x0
    e490:	e0664005 	rsb	r4, r6, r5
    e494:	e3a02000 	mov	r2, #0	; 0x0
    e498:	e1a0000d 	mov	r0, sp
    e49c:	e59f126c 	ldr	r1, [pc, #620]	; e710 <spi_exchange+0x300>
    e4a0:	ea000002 	b	e4b0 <spi_exchange+0xa0>
    e4a4:	e7d03002 	ldrb	r3, [r0, r2]
    e4a8:	e2822001 	add	r2, r2, #1	; 0x1
    e4ac:	e5813008 	str	r3, [r1, #8]
    e4b0:	e1520006 	cmp	r2, r6
    e4b4:	bafffffa 	blt	e4a4 <spi_exchange+0x94>
    e4b8:	e59f6250 	ldr	r6, [pc, #592]	; e710 <spi_exchange+0x300>
    e4bc:	e3a0b005 	mov	fp, #5	; 0x5
    e4c0:	e5969018 	ldr	r9, [r6, #24]
    e4c4:	e3190004 	tst	r9, #4	; 0x4
    e4c8:	0a000030 	beq	e590 <spi_exchange+0x180>
    e4cc:	e3550003 	cmp	r5, #3	; 0x3
    e4d0:	da000005 	ble	e4ec <spi_exchange+0xdc>
    e4d4:	e5963008 	ldr	r3, [r6, #8]
    e4d8:	e2455004 	sub	r5, r5, #4	; 0x4
    e4dc:	e5963008 	ldr	r3, [r6, #8]
    e4e0:	e5963008 	ldr	r3, [r6, #8]
    e4e4:	e5963008 	ldr	r3, [r6, #8]
    e4e8:	ea000028 	b	e590 <spi_exchange+0x180>
    e4ec:	e3550000 	cmp	r5, #0	; 0x0
    e4f0:	0a000003 	beq	e504 <spi_exchange+0xf4>
    e4f4:	e2555001 	subs	r5, r5, #1	; 0x1
    e4f8:	e5963008 	ldr	r3, [r6, #8]
    e4fc:	1afffffc 	bne	e4f4 <spi_exchange+0xe4>
    e500:	ea000022 	b	e590 <spi_exchange+0x180>
    e504:	e59a3000 	ldr	r3, [sl]
    e508:	e3530000 	cmp	r3, #0	; 0x0
    e50c:	1a000004 	bne	e524 <spi_exchange+0x114>
    e510:	e59f01fc 	ldr	r0, [pc, #508]	; e714 <spi_exchange+0x304>
    e514:	e59f31fc 	ldr	r3, [pc, #508]	; e718 <spi_exchange+0x308>
    e518:	e1a0e00f 	mov	lr, pc
    e51c:	e593f000 	ldr	pc, [r3]
    e520:	ea00001a 	b	e590 <spi_exchange+0x180>
    e524:	e3540003 	cmp	r4, #3	; 0x3
    e528:	da00000e 	ble	e568 <spi_exchange+0x158>
    e52c:	e5962008 	ldr	r2, [r6, #8]
    e530:	e1a03008 	mov	r3, r8
    e534:	e4c32001 	strb	r2, [r3], #1
    e538:	e5962008 	ldr	r2, [r6, #8]
    e53c:	e2831001 	add	r1, r3, #1	; 0x1
    e540:	e5c82001 	strb	r2, [r8, #1]
    e544:	e5962008 	ldr	r2, [r6, #8]
    e548:	e2544004 	subs	r4, r4, #4	; 0x4
    e54c:	e5c32001 	strb	r2, [r3, #1]
    e550:	e5963008 	ldr	r3, [r6, #8]
    e554:	e2812001 	add	r2, r1, #1	; 0x1
    e558:	e5c13001 	strb	r3, [r1, #1]
    e55c:	0a00005e 	beq	e6dc <spi_exchange+0x2cc>
    e560:	e2828001 	add	r8, r2, #1	; 0x1
    e564:	ea000009 	b	e590 <spi_exchange+0x180>
    e568:	e3540000 	cmp	r4, #0	; 0x0
    e56c:	0a00005a 	beq	e6dc <spi_exchange+0x2cc>
    e570:	e59f2198 	ldr	r2, [pc, #408]	; e710 <spi_exchange+0x300>
    e574:	e1a01005 	mov	r1, r5
    e578:	e5923008 	ldr	r3, [r2, #8]
    e57c:	e7c83001 	strb	r3, [r8, r1]
    e580:	e2811001 	add	r1, r1, #1	; 0x1
    e584:	e1510004 	cmp	r1, r4
    e588:	1afffffa 	bne	e578 <spi_exchange+0x168>
    e58c:	ea000052 	b	e6dc <spi_exchange+0x2cc>
    e590:	e3190008 	tst	r9, #8	; 0x8
    e594:	0a00002a 	beq	e644 <spi_exchange+0x234>
    e598:	e59a3004 	ldr	r3, [sl, #4]
    e59c:	e3530000 	cmp	r3, #0	; 0x0
    e5a0:	0a000019 	beq	e60c <spi_exchange+0x1fc>
    e5a4:	e3540003 	cmp	r4, #3	; 0x3
    e5a8:	da00000d 	ble	e5e4 <spi_exchange+0x1d4>
    e5ac:	e1a03008 	mov	r3, r8
    e5b0:	e4d32001 	ldrb	r2, [r3], #1
    e5b4:	e2544004 	subs	r4, r4, #4	; 0x4
    e5b8:	e5862008 	str	r2, [r6, #8]
    e5bc:	e5d82001 	ldrb	r2, [r8, #1]
    e5c0:	e2831001 	add	r1, r3, #1	; 0x1
    e5c4:	e5862008 	str	r2, [r6, #8]
    e5c8:	e5d33001 	ldrb	r3, [r3, #1]
    e5cc:	e2818002 	add	r8, r1, #2	; 0x2
    e5d0:	e5863008 	str	r3, [r6, #8]
    e5d4:	e5d13001 	ldrb	r3, [r1, #1]
    e5d8:	e5863008 	str	r3, [r6, #8]
    e5dc:	1a000018 	bne	e644 <spi_exchange+0x234>
    e5e0:	ea00003d 	b	e6dc <spi_exchange+0x2cc>
    e5e4:	e3540000 	cmp	r4, #0	; 0x0
    e5e8:	0a00003b 	beq	e6dc <spi_exchange+0x2cc>
    e5ec:	e59f111c 	ldr	r1, [pc, #284]	; e710 <spi_exchange+0x300>
    e5f0:	e3a02000 	mov	r2, #0	; 0x0
    e5f4:	e7d83002 	ldrb	r3, [r8, r2]
    e5f8:	e2822001 	add	r2, r2, #1	; 0x1
    e5fc:	e1520004 	cmp	r2, r4
    e600:	e5813008 	str	r3, [r1, #8]
    e604:	1afffffa 	bne	e5f4 <spi_exchange+0x1e4>
    e608:	ea000033 	b	e6dc <spi_exchange+0x2cc>
    e60c:	e3570003 	cmp	r7, #3	; 0x3
    e610:	da000005 	ble	e62c <spi_exchange+0x21c>
    e614:	e586b008 	str	fp, [r6, #8]
    e618:	e2477004 	sub	r7, r7, #4	; 0x4
    e61c:	e586b008 	str	fp, [r6, #8]
    e620:	e586b008 	str	fp, [r6, #8]
    e624:	e586b008 	str	fp, [r6, #8]
    e628:	ea000005 	b	e644 <spi_exchange+0x234>
    e62c:	e3570000 	cmp	r7, #0	; 0x0
    e630:	0a000003 	beq	e644 <spi_exchange+0x234>
    e634:	e3a03005 	mov	r3, #5	; 0x5
    e638:	e2577001 	subs	r7, r7, #1	; 0x1
    e63c:	e5863008 	str	r3, [r6, #8]
    e640:	1afffffb 	bne	e634 <spi_exchange+0x224>
    e644:	e3190002 	tst	r9, #2	; 0x2
    e648:	0affff9c 	beq	e4c0 <spi_exchange+0xb0>
    e64c:	ea000009 	b	e678 <spi_exchange+0x268>
    e650:	e596300c 	ldr	r3, [r6, #12]
    e654:	e2455001 	sub	r5, r5, #1	; 0x1
    e658:	e3130004 	tst	r3, #4	; 0x4
    e65c:	15963008 	ldrne	r3, [r6, #8]
    e660:	1a000006 	bne	e680 <spi_exchange+0x270>
    e664:	e59f00b0 	ldr	r0, [pc, #176]	; e71c <spi_exchange+0x30c>
    e668:	e1a01005 	mov	r1, r5
    e66c:	e1a0e00f 	mov	lr, pc
    e670:	e597f000 	ldr	pc, [r7]
    e674:	ea000001 	b	e680 <spi_exchange+0x270>
    e678:	e59f6090 	ldr	r6, [pc, #144]	; e710 <spi_exchange+0x300>
    e67c:	e59f7094 	ldr	r7, [pc, #148]	; e718 <spi_exchange+0x308>
    e680:	e3550000 	cmp	r5, #0	; 0x0
    e684:	1afffff1 	bne	e650 <spi_exchange+0x240>
    e688:	ea00000a 	b	e6b8 <spi_exchange+0x2a8>
    e68c:	e595300c 	ldr	r3, [r5, #12]
    e690:	e2444001 	sub	r4, r4, #1	; 0x1
    e694:	e3130004 	tst	r3, #4	; 0x4
    e698:	15953008 	ldrne	r3, [r5, #8]
    e69c:	14c83001 	strbne	r3, [r8], #1
    e6a0:	1a000006 	bne	e6c0 <spi_exchange+0x2b0>
    e6a4:	e59f0074 	ldr	r0, [pc, #116]	; e720 <spi_exchange+0x310>
    e6a8:	e1a01004 	mov	r1, r4
    e6ac:	e1a0e00f 	mov	lr, pc
    e6b0:	e596f000 	ldr	pc, [r6]
    e6b4:	ea000001 	b	e6c0 <spi_exchange+0x2b0>
    e6b8:	e59f5050 	ldr	r5, [pc, #80]	; e710 <spi_exchange+0x300>
    e6bc:	e59f6054 	ldr	r6, [pc, #84]	; e718 <spi_exchange+0x308>
    e6c0:	e59a3000 	ldr	r3, [sl]
    e6c4:	e3530000 	cmp	r3, #0	; 0x0
    e6c8:	13540000 	cmpne	r4, #0	; 0x0
    e6cc:	1affffee 	bne	e68c <spi_exchange+0x27c>
    e6d0:	e59f3038 	ldr	r3, [pc, #56]	; e710 <spi_exchange+0x300>
    e6d4:	e3a02003 	mov	r2, #3	; 0x3
    e6d8:	e5832020 	str	r2, [r3, #32]
    e6dc:	e59f202c 	ldr	r2, [pc, #44]	; e710 <spi_exchange+0x300>
    e6e0:	e592300c 	ldr	r3, [r2, #12]
    e6e4:	e3130010 	tst	r3, #16	; 0x10
    e6e8:	1afffffc 	bne	e6e0 <spi_exchange+0x2d0>
    e6ec:	ea000001 	b	e6f8 <spi_exchange+0x2e8>
    e6f0:	e5923008 	ldr	r3, [r2, #8]
    e6f4:	ea000000 	b	e6fc <spi_exchange+0x2ec>
    e6f8:	e59f2010 	ldr	r2, [pc, #16]	; e710 <spi_exchange+0x300>
    e6fc:	e592300c 	ldr	r3, [r2, #12]
    e700:	e3130004 	tst	r3, #4	; 0x4
    e704:	1afffff9 	bne	e6f0 <spi_exchange+0x2e0>
    e708:	e28dd00c 	add	sp, sp, #12	; 0xc
    e70c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    e710:	20500000 	.word	0x20500000
    e714:	0001885f 	.word	0x0001885f
    e718:	3c00d6cc 	.word	0x3c00d6cc
    e71c:	00018876 	.word	0x00018876
    e720:	0001888e 	.word	0x0001888e

0000e724 <flash_vs_read>:
    e724:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    e728:	e24dd024 	sub	sp, sp, #36	; 0x24
    e72c:	e28d4018 	add	r4, sp, #24	; 0x18
    e730:	e2841003 	add	r1, r4, #3	; 0x3
    e734:	e59da040 	ldr	sl, [sp, #64]
    e738:	e58d1004 	str	r1, [sp, #4]
    e73c:	e3a01005 	mov	r1, #5	; 0x5
    e740:	e3a0c000 	mov	ip, #0	; 0x0
    e744:	e58d1008 	str	r1, [sp, #8]
    e748:	e2811006 	add	r1, r1, #6	; 0x6
    e74c:	e58dc018 	str	ip, [sp, #24]
    e750:	e2845004 	add	r5, r4, #4	; 0x4
    e754:	e58d000c 	str	r0, [sp, #12]
    e758:	e58dc010 	str	ip, [sp, #16]
    e75c:	e5cd101b 	strb	r1, [sp, #27]
    e760:	e1a06002 	mov	r6, r2
    e764:	e1a07003 	mov	r7, r3
    e768:	e58dc000 	str	ip, [sp]
    e76c:	e1a0400a 	mov	r4, sl
    e770:	e1a0800d 	mov	r8, sp
    e774:	ea00000c 	b	e7ac <flash_vs_read+0x88>
    e778:	e3540a01 	cmp	r4, #4096	; 0x1000
    e77c:	b1a03004 	movlt	r3, r4
    e780:	a3a03a01 	movge	r3, #4096	; 0x1000
    e784:	e5852000 	str	r2, [r5]
    e788:	e58d3014 	str	r3, [sp, #20]
    e78c:	ebffff1f 	bl	e410 <spi_exchange>
    e790:	e59d2014 	ldr	r2, [sp, #20]
    e794:	e59d300c 	ldr	r3, [sp, #12]
    e798:	e0966002 	adds	r6, r6, r2
    e79c:	e0833002 	add	r3, r3, r2
    e7a0:	e0a77fc2 	adc	r7, r7, r2, asr #31
    e7a4:	e58d300c 	str	r3, [sp, #12]
    e7a8:	e0624004 	rsb	r4, r2, r4
    e7ac:	e1a02c06 	lsl	r2, r6, #24
    e7b0:	e2061cff 	and	r1, r6, #65280	; 0xff00
    e7b4:	e1822c26 	orr	r2, r2, r6, lsr #24
    e7b8:	e1822401 	orr	r2, r2, r1, lsl #8
    e7bc:	e20638ff 	and	r3, r6, #16711680	; 0xff0000
    e7c0:	e1822423 	orr	r2, r2, r3, lsr #8
    e7c4:	e3540000 	cmp	r4, #0	; 0x0
    e7c8:	e1a02442 	asr	r2, r2, #8
    e7cc:	e1a0000d 	mov	r0, sp
    e7d0:	e3a01002 	mov	r1, #2	; 0x2
    e7d4:	1affffe7 	bne	e778 <flash_vs_read+0x54>
    e7d8:	e1a0000a 	mov	r0, sl
    e7dc:	e28dd024 	add	sp, sp, #36	; 0x24
    e7e0:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

0000e7e4 <flash_poll>:
    e7e4:	e92d4030 	push	{r4, r5, lr}
    e7e8:	e24dd024 	sub	sp, sp, #36	; 0x24
    e7ec:	e28dc018 	add	ip, sp, #24	; 0x18
    e7f0:	e28c3001 	add	r3, ip, #1	; 0x1
    e7f4:	e3a02000 	mov	r2, #0	; 0x0
    e7f8:	e3a01001 	mov	r1, #1	; 0x1
    e7fc:	e58d300c 	str	r3, [sp, #12]
    e800:	e3a03005 	mov	r3, #5	; 0x5
    e804:	e58d2018 	str	r2, [sp, #24]
    e808:	e58d2010 	str	r2, [sp, #16]
    e80c:	e58d1014 	str	r1, [sp, #20]
    e810:	e5cd3018 	strb	r3, [sp, #24]
    e814:	e1a04000 	mov	r4, r0
    e818:	e58d201c 	str	r2, [sp, #28]
    e81c:	e88d1004 	stm	sp, {r2, ip}
    e820:	e58d1008 	str	r1, [sp, #8]
    e824:	e1a0500d 	mov	r5, sp
    e828:	e1a0000d 	mov	r0, sp
    e82c:	e3a01002 	mov	r1, #2	; 0x2
    e830:	ebfffef6 	bl	e410 <spi_exchange>
    e834:	e5dd3019 	ldrb	r3, [sp, #25]
    e838:	e3540000 	cmp	r4, #0	; 0x0
    e83c:	11a030a3 	lsrne	r3, r3, #1
    e840:	12233001 	eorne	r3, r3, #1	; 0x1
    e844:	e2033001 	and	r3, r3, #1	; 0x1
    e848:	e3530000 	cmp	r3, #0	; 0x0
    e84c:	1afffff5 	bne	e828 <flash_poll+0x44>
    e850:	e28dd024 	add	sp, sp, #36	; 0x24
    e854:	e8bd8030 	pop	{r4, r5, pc}

0000e858 <flash_wren>:
    e858:	e92d4010 	push	{r4, lr}
    e85c:	e24dd020 	sub	sp, sp, #32	; 0x20
    e860:	e3a04001 	mov	r4, #1	; 0x1
    e864:	e28dc018 	add	ip, sp, #24	; 0x18
    e868:	e08c3004 	add	r3, ip, r4
    e86c:	e3a02000 	mov	r2, #0	; 0x0
    e870:	e58d3010 	str	r3, [sp, #16]
    e874:	e3a03006 	mov	r3, #6	; 0x6
    e878:	e58d2018 	str	r2, [sp, #24]
    e87c:	e1a0000d 	mov	r0, sp
    e880:	e5cd3018 	strb	r3, [sp, #24]
    e884:	e1a01004 	mov	r1, r4
    e888:	e3a03004 	mov	r3, #4	; 0x4
    e88c:	e58d200c 	str	r2, [sp, #12]
    e890:	e5cd3019 	strb	r3, [sp, #25]
    e894:	e58d201c 	str	r2, [sp, #28]
    e898:	e88d1004 	stm	sp, {r2, ip}
    e89c:	e58d4008 	str	r4, [sp, #8]
    e8a0:	e58d4014 	str	r4, [sp, #20]
    e8a4:	ebfffed9 	bl	e410 <spi_exchange>
    e8a8:	e1a00004 	mov	r0, r4
    e8ac:	ebffffcc 	bl	e7e4 <flash_poll>
    e8b0:	e28dd020 	add	sp, sp, #32	; 0x20
    e8b4:	e8bd8010 	pop	{r4, pc}

0000e8b8 <flash_vs_erase>:
    e8b8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    e8bc:	e24dd034 	sub	sp, sp, #52	; 0x34
    e8c0:	e28d4028 	add	r4, sp, #40	; 0x28
    e8c4:	e3a06001 	mov	r6, #1	; 0x1
    e8c8:	e59f109c 	ldr	r1, [pc, #156]	; e96c <flash_vs_erase+0xb4>
    e8cc:	e3a02008 	mov	r2, #8	; 0x8
    e8d0:	e1a00004 	mov	r0, r4
    e8d4:	ebffcb1e 	bl	1554 <memcpy>
    e8d8:	e0843006 	add	r3, r4, r6
    e8dc:	e2842003 	add	r2, r4, #3	; 0x3
    e8e0:	e3a05000 	mov	r5, #0	; 0x0
    e8e4:	e28d7004 	add	r7, sp, #4	; 0x4
    e8e8:	e58d3008 	str	r3, [sp, #8]
    e8ec:	e3a03002 	mov	r3, #2	; 0x2
    e8f0:	e58d300c 	str	r3, [sp, #12]
    e8f4:	e58d2014 	str	r2, [sp, #20]
    e8f8:	e58d4020 	str	r4, [sp, #32]
    e8fc:	e58d5004 	str	r5, [sp, #4]
    e900:	e58d5010 	str	r5, [sp, #16]
    e904:	e58d6018 	str	r6, [sp, #24]
    e908:	e58d501c 	str	r5, [sp, #28]
    e90c:	e58d6024 	str	r6, [sp, #36]
    e910:	ebffffd0 	bl	e858 <flash_wren>
    e914:	e1a01006 	mov	r1, r6
    e918:	e1a00007 	mov	r0, r7
    e91c:	ebfffebb 	bl	e410 <spi_exchange>
    e920:	e1a00005 	mov	r0, r5
    e924:	ebffffae 	bl	e7e4 <flash_poll>
    e928:	e2870018 	add	r0, r7, #24	; 0x18
    e92c:	e1a01006 	mov	r1, r6
    e930:	ebfffeb6 	bl	e410 <spi_exchange>
    e934:	e1a01006 	mov	r1, r6
    e938:	e1a00007 	mov	r0, r7
    e93c:	ebfffeb3 	bl	e410 <spi_exchange>
    e940:	e1a00005 	mov	r0, r5
    e944:	ebffffa6 	bl	e7e4 <flash_poll>
    e948:	ebffffc2 	bl	e858 <flash_wren>
    e94c:	e1a01006 	mov	r1, r6
    e950:	e287000c 	add	r0, r7, #12	; 0xc
    e954:	ebfffead 	bl	e410 <spi_exchange>
    e958:	e1a00005 	mov	r0, r5
    e95c:	ebffffa0 	bl	e7e4 <flash_poll>
    e960:	e1a00005 	mov	r0, r5
    e964:	e28dd034 	add	sp, sp, #52	; 0x34
    e968:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    e96c:	00015f86 	.word	0x00015f86

0000e970 <flash_vs_write>:
    e970:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    e974:	e3a01004 	mov	r1, #4	; 0x4
    e978:	e24dd020 	sub	sp, sp, #32	; 0x20
    e97c:	e3a0c000 	mov	ip, #0	; 0x0
    e980:	e59d9040 	ldr	r9, [sp, #64]
    e984:	e58d1008 	str	r1, [sp, #8]
    e988:	e2811002 	add	r1, r1, #2	; 0x2
    e98c:	e58dc018 	str	ip, [sp, #24]
    e990:	e28d701c 	add	r7, sp, #28	; 0x1c
    e994:	e5cd1018 	strb	r1, [sp, #24]
    e998:	e3a01002 	mov	r1, #2	; 0x2
    e99c:	e58dc00c 	str	ip, [sp, #12]
    e9a0:	e58d0010 	str	r0, [sp, #16]
    e9a4:	e5cd101b 	strb	r1, [sp, #27]
    e9a8:	e1a04002 	mov	r4, r2
    e9ac:	e1a05003 	mov	r5, r3
    e9b0:	e58dc000 	str	ip, [sp]
    e9b4:	e58d7004 	str	r7, [sp, #4]
    e9b8:	e1a06009 	mov	r6, r9
    e9bc:	e59fa080 	ldr	sl, [pc, #128]	; ea44 <flash_vs_write+0xd4>
    e9c0:	e1a0800d 	mov	r8, sp
    e9c4:	ea000011 	b	ea10 <flash_vs_write+0xa0>
    e9c8:	e59a3000 	ldr	r3, [sl]
    e9cc:	e5872000 	str	r2, [r7]
    e9d0:	e1560003 	cmp	r6, r3
    e9d4:	d58d6014 	strle	r6, [sp, #20]
    e9d8:	c58d3014 	strgt	r3, [sp, #20]
    e9dc:	ebffff9d 	bl	e858 <flash_wren>
    e9e0:	e1a0000d 	mov	r0, sp
    e9e4:	e3a01002 	mov	r1, #2	; 0x2
    e9e8:	ebfffe88 	bl	e410 <spi_exchange>
    e9ec:	e3a00000 	mov	r0, #0	; 0x0
    e9f0:	ebffff7b 	bl	e7e4 <flash_poll>
    e9f4:	e59d2014 	ldr	r2, [sp, #20]
    e9f8:	e59d3010 	ldr	r3, [sp, #16]
    e9fc:	e0944002 	adds	r4, r4, r2
    ea00:	e0833002 	add	r3, r3, r2
    ea04:	e0a55fc2 	adc	r5, r5, r2, asr #31
    ea08:	e58d3010 	str	r3, [sp, #16]
    ea0c:	e0626006 	rsb	r6, r2, r6
    ea10:	e1a02c04 	lsl	r2, r4, #24
    ea14:	e1822c24 	orr	r2, r2, r4, lsr #24
    ea18:	e2041cff 	and	r1, r4, #65280	; 0xff00
    ea1c:	e1822401 	orr	r2, r2, r1, lsl #8
    ea20:	e20438ff 	and	r3, r4, #16711680	; 0xff0000
    ea24:	e1822423 	orr	r2, r2, r3, lsr #8
    ea28:	e3c220ff 	bic	r2, r2, #255	; 0xff
    ea2c:	e3560000 	cmp	r6, #0	; 0x0
    ea30:	e3822002 	orr	r2, r2, #2	; 0x2
    ea34:	1affffe3 	bne	e9c8 <flash_vs_write+0x58>
    ea38:	e1a00009 	mov	r0, r9
    ea3c:	e28dd020 	add	sp, sp, #32	; 0x20
    ea40:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    ea44:	3c00d0b0 	.word	0x3c00d0b0

0000ea48 <spi_init_pads>:
    ea48:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ea4c:	e3a0000b 	mov	r0, #11	; 0xb
    ea50:	e24dd004 	sub	sp, sp, #4	; 0x4
    ea54:	ebffc78f 	bl	898 <pads_init_module>
    ea58:	e3a00000 	mov	r0, #0	; 0x0
    ea5c:	e28dd004 	add	sp, sp, #4	; 0x4
    ea60:	e8bd8000 	pop	{pc}

0000ea64 <spi_init_clk>:
    ea64:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    ea68:	e59f00e8 	ldr	r0, [pc, #232]	; eb58 <spi_init_clk+0xf4>
    ea6c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    ea70:	ebffc9b4 	bl	1148 <module_get_clock>
    ea74:	e3a05002 	mov	r5, #2	; 0x2
    ea78:	e59fa0dc 	ldr	sl, [pc, #220]	; eb5c <spi_init_clk+0xf8>
    ea7c:	e1a08000 	mov	r8, r0
    ea80:	ea00000c 	b	eab8 <spi_init_clk+0x54>
    ea84:	e1a06004 	mov	r6, r4
    ea88:	e1a01007 	mov	r1, r7
    ea8c:	e1a00008 	mov	r0, r8
    ea90:	eb0017bf 	bl	14994 <__aeabi_idiv>
    ea94:	e150000a 	cmp	r0, sl
    ea98:	e0877005 	add	r7, r7, r5
    ea9c:	e2864001 	add	r4, r6, #1	; 0x1
    eaa0:	da000007 	ble	eac4 <spi_init_clk+0x60>
    eaa4:	e3540c01 	cmp	r4, #256	; 0x100
    eaa8:	1afffff5 	bne	ea84 <spi_init_clk+0x20>
    eaac:	e2855002 	add	r5, r5, #2	; 0x2
    eab0:	e3550c01 	cmp	r5, #256	; 0x100
    eab4:	0a000003 	beq	eac8 <spi_init_clk+0x64>
    eab8:	e1a07005 	mov	r7, r5
    eabc:	e3a06000 	mov	r6, #0	; 0x0
    eac0:	eafffff0 	b	ea88 <spi_init_clk+0x24>
    eac4:	e1a04006 	mov	r4, r6
    eac8:	e35400ff 	cmp	r4, #255	; 0xff
    eacc:	d35500fe 	cmple	r5, #254	; 0xfe
    ead0:	da000007 	ble	eaf4 <spi_init_clk+0x90>
    ead4:	e1a01005 	mov	r1, r5
    ead8:	e1a02004 	mov	r2, r4
    eadc:	e59f007c 	ldr	r0, [pc, #124]	; eb60 <spi_init_clk+0xfc>
    eae0:	e59f307c 	ldr	r3, [pc, #124]	; eb64 <spi_init_clk+0x100>
    eae4:	e1a0e00f 	mov	lr, pc
    eae8:	e593f000 	ldr	pc, [r3]
    eaec:	e3a05002 	mov	r5, #2	; 0x2
    eaf0:	e3a04027 	mov	r4, #39	; 0x27
    eaf4:	e2841001 	add	r1, r4, #1	; 0x1
    eaf8:	e0010195 	mul	r1, r5, r1
    eafc:	e1a00008 	mov	r0, r8
    eb00:	eb0017a3 	bl	14994 <__aeabi_idiv>
    eb04:	e1a01008 	mov	r1, r8
    eb08:	e58d0000 	str	r0, [sp]
    eb0c:	e1a02005 	mov	r2, r5
    eb10:	e1a03004 	mov	r3, r4
    eb14:	e59f004c 	ldr	r0, [pc, #76]	; eb68 <spi_init_clk+0x104>
    eb18:	e59fc044 	ldr	ip, [pc, #68]	; eb64 <spi_init_clk+0x100>
    eb1c:	e1a0e00f 	mov	lr, pc
    eb20:	e59cf000 	ldr	pc, [ip]
    eb24:	e59f2040 	ldr	r2, [pc, #64]	; eb6c <spi_init_clk+0x108>
    eb28:	e3a03000 	mov	r3, #0	; 0x0
    eb2c:	e1a01404 	lsl	r1, r4, #8
    eb30:	e5823004 	str	r3, [r2, #4]
    eb34:	e38110c7 	orr	r1, r1, #199	; 0xc7
    eb38:	e5823014 	str	r3, [r2, #20]
    eb3c:	e1a00003 	mov	r0, r3
    eb40:	e283300a 	add	r3, r3, #10	; 0xa
    eb44:	e5825010 	str	r5, [r2, #16]
    eb48:	e5821000 	str	r1, [r2]
    eb4c:	e5823004 	str	r3, [r2, #4]
    eb50:	e28dd00c 	add	sp, sp, #12	; 0xc
    eb54:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    eb58:	21e0a360 	.word	0x21e0a360
    eb5c:	00551928 	.word	0x00551928
    eb60:	000188a6 	.word	0x000188a6
    eb64:	3c00d6cc 	.word	0x3c00d6cc
    eb68:	000188d4 	.word	0x000188d4
    eb6c:	20500000 	.word	0x20500000

0000eb70 <flash_vs_reset>:
    eb70:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    eb74:	e59f0044 	ldr	r0, [pc, #68]	; ebc0 <flash_vs_reset+0x50>
    eb78:	e24dd004 	sub	sp, sp, #4	; 0x4
    eb7c:	ebffc6ff 	bl	780 <module_enable>
    eb80:	ebffffb7 	bl	ea64 <spi_init_clk>
    eb84:	e3a0000b 	mov	r0, #11	; 0xb
    eb88:	ebffc742 	bl	898 <pads_init_module>
    eb8c:	e59f2030 	ldr	r2, [pc, #48]	; ebc4 <flash_vs_reset+0x54>
    eb90:	e592300c 	ldr	r3, [r2, #12]
    eb94:	e3130010 	tst	r3, #16	; 0x10
    eb98:	1afffffc 	bne	eb90 <flash_vs_reset+0x20>
    eb9c:	ea000001 	b	eba8 <flash_vs_reset+0x38>
    eba0:	e5923008 	ldr	r3, [r2, #8]
    eba4:	ea000000 	b	ebac <flash_vs_reset+0x3c>
    eba8:	e59f2014 	ldr	r2, [pc, #20]	; ebc4 <flash_vs_reset+0x54>
    ebac:	e592300c 	ldr	r3, [r2, #12]
    ebb0:	e2130004 	ands	r0, r3, #4	; 0x4
    ebb4:	1afffff9 	bne	eba0 <flash_vs_reset+0x30>
    ebb8:	e28dd004 	add	sp, sp, #4	; 0x4
    ebbc:	e8bd8000 	pop	{pc}
    ebc0:	21e11400 	.word	0x21e11400
    ebc4:	20500000 	.word	0x20500000

0000ebc8 <ram_addr2offs>:
    ebc8:	e3500000 	cmp	r0, #0	; 0x0
    ebcc:	e3a01000 	mov	r1, #0	; 0x0
    ebd0:	a3a02102 	movge	r2, #-2147483648	; 0x80000000
    ebd4:	b3a02102 	movlt	r2, #-2147483648	; 0x80000000
    ebd8:	a3a03000 	movge	r3, #0	; 0x0
    ebdc:	b3e03000 	mvnlt	r3, #0	; 0x0
    ebe0:	e0922000 	adds	r2, r2, r0
    ebe4:	e0a33001 	adc	r3, r3, r1
    ebe8:	e1a01003 	mov	r1, r3
    ebec:	e1a00002 	mov	r0, r2
    ebf0:	e12fff1e 	bx	lr

0000ebf4 <ram_offs2addr>:
    ebf4:	e2800102 	add	r0, r0, #-2147483648	; 0x80000000
    ebf8:	e12fff1e 	bx	lr

0000ebfc <ram_vs_align>:
    ebfc:	e3a00c02 	mov	r0, #512	; 0x200
    ec00:	e12fff1e 	bx	lr

0000ec04 <ram_vs_reset>:
    ec04:	e3a00000 	mov	r0, #0	; 0x0
    ec08:	e12fff1e 	bx	lr

0000ec0c <ram_vs_write>:
    ec0c:	e92d4010 	push	{r4, lr}
    ec10:	e59d4008 	ldr	r4, [sp, #8]
    ec14:	e1a01000 	mov	r1, r0
    ec18:	e2820102 	add	r0, r2, #-2147483648	; 0x80000000
    ec1c:	e1a02004 	mov	r2, r4
    ec20:	ebffca4b 	bl	1554 <memcpy>
    ec24:	e1a00004 	mov	r0, r4
    ec28:	e8bd8010 	pop	{r4, pc}

0000ec2c <ram_vs_read>:
    ec2c:	e92d4010 	push	{r4, lr}
    ec30:	e59d4008 	ldr	r4, [sp, #8]
    ec34:	e2821102 	add	r1, r2, #-2147483648	; 0x80000000
    ec38:	e1a02004 	mov	r2, r4
    ec3c:	ebffca44 	bl	1554 <memcpy>
    ec40:	e1a00004 	mov	r0, r4
    ec44:	e8bd8010 	pop	{r4, pc}

0000ec48 <CreateAtaCFIS>:
    ec48:	e3a0c000 	mov	ip, #0	; 0x0
    ec4c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    ec50:	e1a05003 	mov	r5, r3
    ec54:	e1a04000 	mov	r4, r0
    ec58:	e1a06001 	mov	r6, r1
    ec5c:	e1a08002 	mov	r8, r2
    ec60:	e1a0300c 	mov	r3, ip
    ec64:	e7c4300c 	strb	r3, [r4, ip]
    ec68:	e28cc001 	add	ip, ip, #1	; 0x1
    ec6c:	e35c0014 	cmp	ip, #20	; 0x14
    ec70:	e3a07000 	mov	r7, #0	; 0x0
    ec74:	1afffffa 	bne	ec64 <CreateAtaCFIS+0x1c>
    ec78:	e5d43001 	ldrb	r3, [r4, #1]
    ec7c:	e59d2018 	ldr	r2, [sp, #24]
    ec80:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    ec84:	e1520007 	cmp	r2, r7
    ec88:	e6ef3073 	uxtb	r3, r3
    ec8c:	03a02080 	moveq	r2, #128	; 0x80
    ec90:	11a02007 	movne	r2, r7
    ec94:	e3c33080 	bic	r3, r3, #128	; 0x80
    ec98:	e35600ca 	cmp	r6, #202	; 0xca
    ec9c:	135600c8 	cmpne	r6, #200	; 0xc8
    eca0:	e1822003 	orr	r2, r2, r3
    eca4:	e3a03027 	mov	r3, #39	; 0x27
    eca8:	e5c43000 	strb	r3, [r4]
    ecac:	01a03c25 	lsreq	r3, r5, #24
    ecb0:	e1a0c428 	lsr	ip, r8, #8
    ecb4:	e1a01425 	lsr	r1, r5, #8
    ecb8:	e1a00825 	lsr	r0, r5, #16
    ecbc:	0203300f 	andeq	r3, r3, #15	; 0xf
    ecc0:	e5c47007 	strb	r7, [r4, #7]
    ecc4:	e5c42001 	strb	r2, [r4, #1]
    ecc8:	e5c4700b 	strb	r7, [r4, #11]
    eccc:	e5c4c00d 	strb	ip, [r4, #13]
    ecd0:	e5c41005 	strb	r1, [r4, #5]
    ecd4:	e5c40006 	strb	r0, [r4, #6]
    ecd8:	e5c46002 	strb	r6, [r4, #2]
    ecdc:	e5c47003 	strb	r7, [r4, #3]
    ece0:	e5c4800c 	strb	r8, [r4, #12]
    ece4:	e5c45004 	strb	r5, [r4, #4]
    ece8:	05c43007 	strbeq	r3, [r4, #7]
    ecec:	0a000003 	beq	ed00 <CreateAtaCFIS+0xb8>
    ecf0:	e3560035 	cmp	r6, #53	; 0x35
    ecf4:	13560025 	cmpne	r6, #37	; 0x25
    ecf8:	01a03c25 	lsreq	r3, r5, #24
    ecfc:	05c43008 	strbeq	r3, [r4, #8]
    ed00:	e59d201c 	ldr	r2, [sp, #28]
    ed04:	e5d43007 	ldrb	r3, [r4, #7]
    ed08:	e3520000 	cmp	r2, #0	; 0x0
    ed0c:	e3a01000 	mov	r1, #0	; 0x0
    ed10:	13a02004 	movne	r2, #4	; 0x4
    ed14:	03a02000 	moveq	r2, #0	; 0x0
    ed18:	e3833040 	orr	r3, r3, #64	; 0x40
    ed1c:	e35600ec 	cmp	r6, #236	; 0xec
    ed20:	e5c43007 	strb	r3, [r4, #7]
    ed24:	e5c4200f 	strb	r2, [r4, #15]
    ed28:	e5c41009 	strb	r1, [r4, #9]
    ed2c:	e5c4100a 	strb	r1, [r4, #10]
    ed30:	e5c4100e 	strb	r1, [r4, #14]
    ed34:	e5841010 	str	r1, [r4, #16]
    ed38:	05c41007 	strbeq	r1, [r4, #7]
    ed3c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000ed40 <CreateCommandHeader>:
    ed40:	e92d4070 	push	{r4, r5, r6, lr}
    ed44:	e59d6018 	ldr	r6, [sp, #24]
    ed48:	e1a04000 	mov	r4, r0
    ed4c:	e1a00001 	mov	r0, r1
    ed50:	e3a01000 	mov	r1, #0	; 0x0
    ed54:	e1a05003 	mov	r5, r3
    ed58:	e084c280 	add	ip, r4, r0, lsl #5
    ed5c:	e1a03001 	mov	r3, r1
    ed60:	e7c1300c 	strb	r3, [r1, ip]
    ed64:	e2811001 	add	r1, r1, #1	; 0x1
    ed68:	e3510020 	cmp	r1, #32	; 0x20
    ed6c:	1afffffb 	bne	ed60 <CreateCommandHeader+0x20>
    ed70:	e3a03000 	mov	r3, #0	; 0x0
    ed74:	e58c2008 	str	r2, [ip, #8]
    ed78:	e58c300c 	str	r3, [ip, #12]
    ed7c:	e1cc50b2 	strh	r5, [ip, #2]
    ed80:	e1560003 	cmp	r6, r3
    ed84:	e7d43280 	ldrb	r3, [r4, r0, lsl #5]
    ed88:	13c3301c 	bicne	r3, r3, #28	; 0x1c
    ed8c:	03c3301a 	biceq	r3, r3, #26	; 0x1a
    ed90:	13833003 	orrne	r3, r3, #3	; 0x3
    ed94:	03833005 	orreq	r3, r3, #5	; 0x5
    ed98:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    ed9c:	e59d3010 	ldr	r3, [sp, #16]
    eda0:	e7d42280 	ldrb	r2, [r4, r0, lsl #5]
    eda4:	e3530000 	cmp	r3, #0	; 0x0
    eda8:	13a03020 	movne	r3, #32	; 0x20
    edac:	03a03000 	moveq	r3, #0	; 0x0
    edb0:	e3c22020 	bic	r2, r2, #32	; 0x20
    edb4:	e1833002 	orr	r3, r3, r2
    edb8:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    edbc:	e6ef2073 	uxtb	r2, r3
    edc0:	e59d3014 	ldr	r3, [sp, #20]
    edc4:	e3c22040 	bic	r2, r2, #64	; 0x40
    edc8:	e3530000 	cmp	r3, #0	; 0x0
    edcc:	13a03040 	movne	r3, #64	; 0x40
    edd0:	03a03000 	moveq	r3, #0	; 0x0
    edd4:	e1833002 	orr	r3, r3, r2
    edd8:	e6ef3073 	uxtb	r3, r3
    eddc:	e3c33080 	bic	r3, r3, #128	; 0x80
    ede0:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    ede4:	e5dc3001 	ldrb	r3, [ip, #1]
    ede8:	e3560000 	cmp	r6, #0	; 0x0
    edec:	e3c330f0 	bic	r3, r3, #240	; 0xf0
    edf0:	e6ef3073 	uxtb	r3, r3
    edf4:	13a01002 	movne	r1, #2	; 0x2
    edf8:	03a01000 	moveq	r1, #0	; 0x0
    edfc:	e3c33002 	bic	r3, r3, #2	; 0x2
    ee00:	e59d001c 	ldr	r0, [sp, #28]
    ee04:	e1811003 	orr	r1, r1, r3
    ee08:	e6ef3071 	uxtb	r3, r1
    ee0c:	e2500000 	subs	r0, r0, #0	; 0x0
    ee10:	13a00001 	movne	r0, #1	; 0x1
    ee14:	e3c33001 	bic	r3, r3, #1	; 0x1
    ee18:	e1803003 	orr	r3, r0, r3
    ee1c:	e6ef3073 	uxtb	r3, r3
    ee20:	e3c33004 	bic	r3, r3, #4	; 0x4
    ee24:	e3a02000 	mov	r2, #0	; 0x0
    ee28:	e1833100 	orr	r3, r3, r0, lsl #2
    ee2c:	e58c201c 	str	r2, [ip, #28]
    ee30:	e58c2004 	str	r2, [ip, #4]
    ee34:	e5cc3001 	strb	r3, [ip, #1]
    ee38:	e58c2010 	str	r2, [ip, #16]
    ee3c:	e58c2014 	str	r2, [ip, #20]
    ee40:	e58c2018 	str	r2, [ip, #24]
    ee44:	e8bd8070 	pop	{r4, r5, r6, pc}

0000ee48 <get_max_lba>:
    ee48:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    ee4c:	e3a04000 	mov	r4, #0	; 0x0
    ee50:	e2807c05 	add	r7, r0, #1280	; 0x500
    ee54:	e1a06000 	mov	r6, r0
    ee58:	e24dd010 	sub	sp, sp, #16	; 0x10
    ee5c:	e3a010ec 	mov	r1, #236	; 0xec
    ee60:	e3a02001 	mov	r2, #1	; 0x1
    ee64:	e1a03004 	mov	r3, r4
    ee68:	e1a00007 	mov	r0, r7
    ee6c:	e58d4000 	str	r4, [sp]
    ee70:	e58d4004 	str	r4, [sp, #4]
    ee74:	e2868b01 	add	r8, r6, #1024	; 0x400
    ee78:	ebffff72 	bl	ec48 <CreateAtaCFIS>
    ee7c:	e2862b02 	add	r2, r6, #2048	; 0x800
    ee80:	e2863d16 	add	r3, r6, #1408	; 0x580
    ee84:	e1a01004 	mov	r1, r4
    ee88:	e7c31004 	strb	r1, [r3, r4]
    ee8c:	e2844001 	add	r4, r4, #1	; 0x1
    ee90:	e3540010 	cmp	r4, #16	; 0x10
    ee94:	e3a05000 	mov	r5, #0	; 0x0
    ee98:	1afffffa 	bne	ee88 <get_max_lba+0x40>
    ee9c:	e5872080 	str	r2, [r7, #128]
    eea0:	e596258c 	ldr	r2, [r6, #1420]
    eea4:	e5865584 	str	r5, [r6, #1412]
    eea8:	e1a03b22 	lsr	r3, r2, #22
    eeac:	e1a03b03 	lsl	r3, r3, #22
    eeb0:	e1e024a3 	mvn	r2, r3, lsr #9
    eeb4:	e1e02482 	mvn	r2, r2, lsl #9
    eeb8:	e1a03c22 	lsr	r3, r2, #24
    eebc:	e3c33080 	bic	r3, r3, #128	; 0x80
    eec0:	e586258c 	str	r2, [r6, #1420]
    eec4:	e1a01005 	mov	r1, r5
    eec8:	e5c6358f 	strb	r3, [r6, #1423]
    eecc:	e1a02007 	mov	r2, r7
    eed0:	e3a03001 	mov	r3, #1	; 0x1
    eed4:	e1a00006 	mov	r0, r6
    eed8:	e58d5000 	str	r5, [sp]
    eedc:	e58d5004 	str	r5, [sp, #4]
    eee0:	e58d5008 	str	r5, [sp, #8]
    eee4:	e58d500c 	str	r5, [sp, #12]
    eee8:	ebffff94 	bl	ed40 <CreateCommandHeader>
    eeec:	e1a01005 	mov	r1, r5
    eef0:	e1a03005 	mov	r3, r5
    eef4:	e7c83001 	strb	r3, [r8, r1]
    eef8:	e2811001 	add	r1, r1, #1	; 0x1
    eefc:	e3510c01 	cmp	r1, #256	; 0x100
    ef00:	1afffffb 	bne	eef4 <get_max_lba+0xac>
    ef04:	e59f20c8 	ldr	r2, [pc, #200]	; efd4 <get_max_lba+0x18c>
    ef08:	e3e03000 	mvn	r3, #0	; 0x0
    ef0c:	e5826100 	str	r6, [r2, #256]
    ef10:	e5828108 	str	r8, [r2, #264]
    ef14:	e5823110 	str	r3, [r2, #272]
    ef18:	e5823130 	str	r3, [r2, #304]
    ef1c:	e5923118 	ldr	r3, [r2, #280]
    ef20:	e1a00002 	mov	r0, r2
    ef24:	e3833010 	orr	r3, r3, #16	; 0x10
    ef28:	e5823118 	str	r3, [r2, #280]
    ef2c:	e5923118 	ldr	r3, [r2, #280]
    ef30:	e3833001 	orr	r3, r3, #1	; 0x1
    ef34:	e5823118 	str	r3, [r2, #280]
    ef38:	e3a03001 	mov	r3, #1	; 0x1
    ef3c:	e5823138 	str	r3, [r2, #312]
    ef40:	e5903110 	ldr	r3, [r0, #272]
    ef44:	e59f1088 	ldr	r1, [pc, #136]	; efd4 <get_max_lba+0x18c>
    ef48:	e3130002 	tst	r3, #2	; 0x2
    ef4c:	0afffffb 	beq	ef40 <get_max_lba+0xf8>
    ef50:	e3e03000 	mvn	r3, #0	; 0x0
    ef54:	e5912008 	ldr	r2, [r1, #8]
    ef58:	e5813110 	str	r3, [r1, #272]
    ef5c:	e5813008 	str	r3, [r1, #8]
    ef60:	e5912118 	ldr	r2, [r1, #280]
    ef64:	e30c3011 	movw	r3, #49169	; 0xc011
    ef68:	e0023003 	and	r3, r2, r3
    ef6c:	e3530000 	cmp	r3, #0	; 0x0
    ef70:	e596e8c8 	ldr	lr, [r6, #2248]
    ef74:	e59648cc 	ldr	r4, [r6, #2252]
    ef78:	0a00000e 	beq	efb8 <get_max_lba+0x170>
    ef7c:	e5913118 	ldr	r3, [r1, #280]
    ef80:	e1a0c001 	mov	ip, r1
    ef84:	e3c33001 	bic	r3, r3, #1	; 0x1
    ef88:	e5813118 	str	r3, [r1, #280]
    ef8c:	e59c3118 	ldr	r3, [ip, #280]
    ef90:	e59f203c 	ldr	r2, [pc, #60]	; efd4 <get_max_lba+0x18c>
    ef94:	e3130902 	tst	r3, #32768	; 0x8000
    ef98:	1afffffb 	bne	ef8c <get_max_lba+0x144>
    ef9c:	e5923118 	ldr	r3, [r2, #280]
    efa0:	e1a01002 	mov	r1, r2
    efa4:	e3c33010 	bic	r3, r3, #16	; 0x10
    efa8:	e5823118 	str	r3, [r2, #280]
    efac:	e5913118 	ldr	r3, [r1, #280]
    efb0:	e3130901 	tst	r3, #16384	; 0x4000
    efb4:	1afffffc 	bne	efac <get_max_lba+0x164>
    efb8:	e59f2014 	ldr	r2, [pc, #20]	; efd4 <get_max_lba+0x18c>
    efbc:	e5923120 	ldr	r3, [r2, #288]
    efc0:	e2130088 	ands	r0, r3, #136	; 0x88
    efc4:	1afffffc 	bne	efbc <get_max_lba+0x174>
    efc8:	e08e0000 	add	r0, lr, r0
    efcc:	e28dd010 	add	sp, sp, #16	; 0x10
    efd0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    efd4:	28200000 	.word	0x28200000

0000efd8 <sata_vs_align>:
    efd8:	e3a00c02 	mov	r0, #512	; 0x200
    efdc:	e12fff1e 	bx	lr

0000efe0 <init_sata>:
    efe0:	e12fff1e 	bx	lr

0000efe4 <sata_vs_write>:
    efe4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    efe8:	e1a08000 	mov	r8, r0
    efec:	e24dd034 	sub	sp, sp, #52	; 0x34
    eff0:	e3a00c0e 	mov	r0, #3584	; 0xe00
    eff4:	e1a06002 	mov	r6, r2
    eff8:	e1a07003 	mov	r7, r3
    effc:	ebffc9b9 	bl	16e8 <malloc>
    f000:	e3500000 	cmp	r0, #0	; 0x0
    f004:	e58d0014 	str	r0, [sp, #20]
    f008:	1a000004 	bne	f020 <sata_vs_write+0x3c>
    f00c:	e59f0680 	ldr	r0, [pc, #1664]	; f694 <sata_vs_write+0x6b0>
    f010:	e59f3680 	ldr	r3, [pc, #1664]	; f698 <sata_vs_write+0x6b4>
    f014:	e1a0e00f 	mov	lr, pc
    f018:	e593f000 	ldr	pc, [r3]
    f01c:	ea0000ea 	b	f3cc <sata_vs_write+0x3e8>
    f020:	e59d0014 	ldr	r0, [sp, #20]
    f024:	e2803fff 	add	r3, r0, #1020	; 0x3fc
    f028:	e2833003 	add	r3, r3, #3	; 0x3
    f02c:	e3c33fff 	bic	r3, r3, #1020	; 0x3fc
    f030:	e3c33003 	bic	r3, r3, #3	; 0x3
    f034:	e1530000 	cmp	r3, r0
    f038:	e58d3018 	str	r3, [sp, #24]
    f03c:	02833a01 	addeq	r3, r3, #4096	; 0x1000
    f040:	058d3018 	streq	r3, [sp, #24]
    f044:	e59d0018 	ldr	r0, [sp, #24]
    f048:	ebffff7e 	bl	ee48 <get_max_lba>
    f04c:	e59d1058 	ldr	r1, [sp, #88]
    f050:	e1a0c000 	mov	ip, r0
    f054:	e1a03b81 	lsl	r3, r1, #23
    f058:	e1a03ba3 	lsr	r3, r3, #23
    f05c:	e3530000 	cmp	r3, #0	; 0x0
    f060:	1a0000d3 	bne	f3b4 <sata_vs_write+0x3d0>
    f064:	e30041ff 	movw	r4, #511	; 0x1ff
    f068:	e3a05000 	mov	r5, #0	; 0x0
    f06c:	e0062004 	and	r2, r6, r4
    f070:	e0073005 	and	r3, r7, r5
    f074:	e1920003 	orrs	r0, r2, r3
    f078:	1a0000cd 	bne	f3b4 <sata_vs_write+0x3d0>
    f07c:	e3180001 	tst	r8, #1	; 0x1
    f080:	1a0000cb 	bne	f3b4 <sata_vs_write+0x3d0>
    f084:	e1a00007 	mov	r0, r7
    f088:	e1a01fc0 	asr	r1, r0, #31
    f08c:	e1a02fc1 	asr	r2, r1, #31
    f090:	e1a03002 	mov	r3, r2
    f094:	e0022004 	and	r2, r2, r4
    f098:	e0922006 	adds	r2, r2, r6
    f09c:	e0033005 	and	r3, r3, r5
    f0a0:	e0a33007 	adc	r3, r3, r7
    f0a4:	e1a014a2 	lsr	r1, r2, #9
    f0a8:	e1a0600c 	mov	r6, ip
    f0ac:	e1a07fc6 	asr	r7, r6, #31
    f0b0:	e1811b83 	orr	r1, r1, r3, lsl #23
    f0b4:	e3570000 	cmp	r7, #0	; 0x0
    f0b8:	e58d1028 	str	r1, [sp, #40]
    f0bc:	e1a00001 	mov	r0, r1
    f0c0:	e3a01000 	mov	r1, #0	; 0x0
    f0c4:	8a000002 	bhi	f0d4 <sata_vs_write+0xf0>
    f0c8:	1a0000b9 	bne	f3b4 <sata_vs_write+0x3d0>
    f0cc:	e1560000 	cmp	r6, r0
    f0d0:	9a0000b7 	bls	f3b4 <sata_vs_write+0x3d0>
    f0d4:	e59d2058 	ldr	r2, [sp, #88]
    f0d8:	e3520402 	cmp	r2, #33554432	; 0x2000000
    f0dc:	ba000007 	blt	f100 <sata_vs_write+0x11c>
    f0e0:	e1a02006 	mov	r2, r6
    f0e4:	e1a03007 	mov	r3, r7
    f0e8:	e0522000 	subs	r2, r2, r0
    f0ec:	e0c33001 	sbc	r3, r3, r1
    f0f0:	e3530000 	cmp	r3, #0	; 0x0
    f0f4:	1a000001 	bne	f100 <sata_vs_write+0x11c>
    f0f8:	e3520801 	cmp	r2, #65536	; 0x10000
    f0fc:	3a0000ac 	bcc	f3b4 <sata_vs_write+0x3d0>
    f100:	e59d3058 	ldr	r3, [sp, #88]
    f104:	e3530401 	cmp	r3, #16777216	; 0x1000000
    f108:	ba000007 	blt	f12c <sata_vs_write+0x148>
    f10c:	e1a02006 	mov	r2, r6
    f110:	e1a03007 	mov	r3, r7
    f114:	e0522000 	subs	r2, r2, r0
    f118:	e0c33001 	sbc	r3, r3, r1
    f11c:	e3530000 	cmp	r3, #0	; 0x0
    f120:	1a000001 	bne	f12c <sata_vs_write+0x148>
    f124:	e3520902 	cmp	r2, #32768	; 0x8000
    f128:	3a0000a1 	bcc	f3b4 <sata_vs_write+0x3d0>
    f12c:	e59dc058 	ldr	ip, [sp, #88]
    f130:	e35c0502 	cmp	ip, #8388608	; 0x800000
    f134:	ba000007 	blt	f158 <sata_vs_write+0x174>
    f138:	e1a02006 	mov	r2, r6
    f13c:	e1a03007 	mov	r3, r7
    f140:	e0522000 	subs	r2, r2, r0
    f144:	e0c33001 	sbc	r3, r3, r1
    f148:	e3530000 	cmp	r3, #0	; 0x0
    f14c:	1a000001 	bne	f158 <sata_vs_write+0x174>
    f150:	e3520901 	cmp	r2, #16384	; 0x4000
    f154:	3a000096 	bcc	f3b4 <sata_vs_write+0x3d0>
    f158:	e59d2058 	ldr	r2, [sp, #88]
    f15c:	e3520501 	cmp	r2, #4194304	; 0x400000
    f160:	ba000007 	blt	f184 <sata_vs_write+0x1a0>
    f164:	e1a02006 	mov	r2, r6
    f168:	e1a03007 	mov	r3, r7
    f16c:	e0522000 	subs	r2, r2, r0
    f170:	e0c33001 	sbc	r3, r3, r1
    f174:	e3530000 	cmp	r3, #0	; 0x0
    f178:	1a000001 	bne	f184 <sata_vs_write+0x1a0>
    f17c:	e3520a02 	cmp	r2, #8192	; 0x2000
    f180:	3a00008b 	bcc	f3b4 <sata_vs_write+0x3d0>
    f184:	e59d3058 	ldr	r3, [sp, #88]
    f188:	e3530602 	cmp	r3, #2097152	; 0x200000
    f18c:	ba000007 	blt	f1b0 <sata_vs_write+0x1cc>
    f190:	e1a02006 	mov	r2, r6
    f194:	e1a03007 	mov	r3, r7
    f198:	e0522000 	subs	r2, r2, r0
    f19c:	e0c33001 	sbc	r3, r3, r1
    f1a0:	e3530000 	cmp	r3, #0	; 0x0
    f1a4:	1a000001 	bne	f1b0 <sata_vs_write+0x1cc>
    f1a8:	e3520a01 	cmp	r2, #4096	; 0x1000
    f1ac:	3a000080 	bcc	f3b4 <sata_vs_write+0x3d0>
    f1b0:	e59dc058 	ldr	ip, [sp, #88]
    f1b4:	e35c0601 	cmp	ip, #1048576	; 0x100000
    f1b8:	ba000007 	blt	f1dc <sata_vs_write+0x1f8>
    f1bc:	e1a02006 	mov	r2, r6
    f1c0:	e1a03007 	mov	r3, r7
    f1c4:	e0522000 	subs	r2, r2, r0
    f1c8:	e0c33001 	sbc	r3, r3, r1
    f1cc:	e3530000 	cmp	r3, #0	; 0x0
    f1d0:	1a000001 	bne	f1dc <sata_vs_write+0x1f8>
    f1d4:	e3520b02 	cmp	r2, #2048	; 0x800
    f1d8:	3a000075 	bcc	f3b4 <sata_vs_write+0x3d0>
    f1dc:	e59d2058 	ldr	r2, [sp, #88]
    f1e0:	e3520702 	cmp	r2, #524288	; 0x80000
    f1e4:	ba000007 	blt	f208 <sata_vs_write+0x224>
    f1e8:	e1a02006 	mov	r2, r6
    f1ec:	e1a03007 	mov	r3, r7
    f1f0:	e0522000 	subs	r2, r2, r0
    f1f4:	e0c33001 	sbc	r3, r3, r1
    f1f8:	e3530000 	cmp	r3, #0	; 0x0
    f1fc:	1a000001 	bne	f208 <sata_vs_write+0x224>
    f200:	e3520b01 	cmp	r2, #1024	; 0x400
    f204:	3a00006a 	bcc	f3b4 <sata_vs_write+0x3d0>
    f208:	e59d3058 	ldr	r3, [sp, #88]
    f20c:	e3530701 	cmp	r3, #262144	; 0x40000
    f210:	ba000007 	blt	f234 <sata_vs_write+0x250>
    f214:	e1a02006 	mov	r2, r6
    f218:	e1a03007 	mov	r3, r7
    f21c:	e0522000 	subs	r2, r2, r0
    f220:	e0c33001 	sbc	r3, r3, r1
    f224:	e3530000 	cmp	r3, #0	; 0x0
    f228:	1a000001 	bne	f234 <sata_vs_write+0x250>
    f22c:	e3520c02 	cmp	r2, #512	; 0x200
    f230:	3a00005f 	bcc	f3b4 <sata_vs_write+0x3d0>
    f234:	e59dc058 	ldr	ip, [sp, #88]
    f238:	e35c0802 	cmp	ip, #131072	; 0x20000
    f23c:	ba000008 	blt	f264 <sata_vs_write+0x280>
    f240:	e3e020ff 	mvn	r2, #255	; 0xff
    f244:	e0922006 	adds	r2, r2, r6
    f248:	e3e03000 	mvn	r3, #0	; 0x0
    f24c:	e0a33007 	adc	r3, r3, r7
    f250:	e1510003 	cmp	r1, r3
    f254:	8a000056 	bhi	f3b4 <sata_vs_write+0x3d0>
    f258:	1a000001 	bne	f264 <sata_vs_write+0x280>
    f25c:	e1500002 	cmp	r0, r2
    f260:	8a000053 	bhi	f3b4 <sata_vs_write+0x3d0>
    f264:	e59d2058 	ldr	r2, [sp, #88]
    f268:	e3520801 	cmp	r2, #65536	; 0x10000
    f26c:	ba000007 	blt	f290 <sata_vs_write+0x2ac>
    f270:	e1a02006 	mov	r2, r6
    f274:	e1a03007 	mov	r3, r7
    f278:	e0522000 	subs	r2, r2, r0
    f27c:	e0c33001 	sbc	r3, r3, r1
    f280:	e3530000 	cmp	r3, #0	; 0x0
    f284:	1a000001 	bne	f290 <sata_vs_write+0x2ac>
    f288:	e352007f 	cmp	r2, #127	; 0x7f
    f28c:	9a000048 	bls	f3b4 <sata_vs_write+0x3d0>
    f290:	e59d3058 	ldr	r3, [sp, #88]
    f294:	e3530902 	cmp	r3, #32768	; 0x8000
    f298:	ba000007 	blt	f2bc <sata_vs_write+0x2d8>
    f29c:	e1a02006 	mov	r2, r6
    f2a0:	e1a03007 	mov	r3, r7
    f2a4:	e0522000 	subs	r2, r2, r0
    f2a8:	e0c33001 	sbc	r3, r3, r1
    f2ac:	e3530000 	cmp	r3, #0	; 0x0
    f2b0:	1a000001 	bne	f2bc <sata_vs_write+0x2d8>
    f2b4:	e352003f 	cmp	r2, #63	; 0x3f
    f2b8:	9a00003d 	bls	f3b4 <sata_vs_write+0x3d0>
    f2bc:	e59dc058 	ldr	ip, [sp, #88]
    f2c0:	e35c0901 	cmp	ip, #16384	; 0x4000
    f2c4:	ba000007 	blt	f2e8 <sata_vs_write+0x304>
    f2c8:	e1a02006 	mov	r2, r6
    f2cc:	e1a03007 	mov	r3, r7
    f2d0:	e0522000 	subs	r2, r2, r0
    f2d4:	e0c33001 	sbc	r3, r3, r1
    f2d8:	e3530000 	cmp	r3, #0	; 0x0
    f2dc:	1a000001 	bne	f2e8 <sata_vs_write+0x304>
    f2e0:	e352001f 	cmp	r2, #31	; 0x1f
    f2e4:	9a000032 	bls	f3b4 <sata_vs_write+0x3d0>
    f2e8:	e59d2058 	ldr	r2, [sp, #88]
    f2ec:	e3520a02 	cmp	r2, #8192	; 0x2000
    f2f0:	ba000007 	blt	f314 <sata_vs_write+0x330>
    f2f4:	e1a02006 	mov	r2, r6
    f2f8:	e1a03007 	mov	r3, r7
    f2fc:	e0522000 	subs	r2, r2, r0
    f300:	e0c33001 	sbc	r3, r3, r1
    f304:	e3530000 	cmp	r3, #0	; 0x0
    f308:	1a000001 	bne	f314 <sata_vs_write+0x330>
    f30c:	e352000f 	cmp	r2, #15	; 0xf
    f310:	9a000027 	bls	f3b4 <sata_vs_write+0x3d0>
    f314:	e59d3058 	ldr	r3, [sp, #88]
    f318:	e3530a01 	cmp	r3, #4096	; 0x1000
    f31c:	ba000007 	blt	f340 <sata_vs_write+0x35c>
    f320:	e1a02006 	mov	r2, r6
    f324:	e1a03007 	mov	r3, r7
    f328:	e0522000 	subs	r2, r2, r0
    f32c:	e0c33001 	sbc	r3, r3, r1
    f330:	e3530000 	cmp	r3, #0	; 0x0
    f334:	1a000001 	bne	f340 <sata_vs_write+0x35c>
    f338:	e3520007 	cmp	r2, #7	; 0x7
    f33c:	9a00001c 	bls	f3b4 <sata_vs_write+0x3d0>
    f340:	e59dc058 	ldr	ip, [sp, #88]
    f344:	e35c0b02 	cmp	ip, #2048	; 0x800
    f348:	ba000007 	blt	f36c <sata_vs_write+0x388>
    f34c:	e1a02006 	mov	r2, r6
    f350:	e1a03007 	mov	r3, r7
    f354:	e0522000 	subs	r2, r2, r0
    f358:	e0c33001 	sbc	r3, r3, r1
    f35c:	e3530000 	cmp	r3, #0	; 0x0
    f360:	1a000001 	bne	f36c <sata_vs_write+0x388>
    f364:	e3520003 	cmp	r2, #3	; 0x3
    f368:	9a000011 	bls	f3b4 <sata_vs_write+0x3d0>
    f36c:	e59d2058 	ldr	r2, [sp, #88]
    f370:	e3520b01 	cmp	r2, #1024	; 0x400
    f374:	ba000007 	blt	f398 <sata_vs_write+0x3b4>
    f378:	e1a02006 	mov	r2, r6
    f37c:	e1a03007 	mov	r3, r7
    f380:	e0522000 	subs	r2, r2, r0
    f384:	e0c33001 	sbc	r3, r3, r1
    f388:	e3530000 	cmp	r3, #0	; 0x0
    f38c:	1a000001 	bne	f398 <sata_vs_write+0x3b4>
    f390:	e3520001 	cmp	r2, #1	; 0x1
    f394:	9a000006 	bls	f3b4 <sata_vs_write+0x3d0>
    f398:	e59d3058 	ldr	r3, [sp, #88]
    f39c:	e3530c02 	cmp	r3, #512	; 0x200
    f3a0:	ba00000b 	blt	f3d4 <sata_vs_write+0x3f0>
    f3a4:	e1560000 	cmp	r6, r0
    f3a8:	1a000009 	bne	f3d4 <sata_vs_write+0x3f0>
    f3ac:	e1570001 	cmp	r7, r1
    f3b0:	1a000007 	bne	f3d4 <sata_vs_write+0x3f0>
    f3b4:	e59f02e0 	ldr	r0, [pc, #736]	; f69c <sata_vs_write+0x6b8>
    f3b8:	e59f32d8 	ldr	r3, [pc, #728]	; f698 <sata_vs_write+0x6b4>
    f3bc:	e1a0e00f 	mov	lr, pc
    f3c0:	e593f000 	ldr	pc, [r3]
    f3c4:	e59d0014 	ldr	r0, [sp, #20]
    f3c8:	ebffc92e 	bl	1888 <free>
    f3cc:	e3e04000 	mvn	r4, #0	; 0x0
    f3d0:	ea0000ac 	b	f688 <sata_vs_write+0x6a4>
    f3d4:	e59dc018 	ldr	ip, [sp, #24]
    f3d8:	e59d0018 	ldr	r0, [sp, #24]
    f3dc:	e59d1018 	ldr	r1, [sp, #24]
    f3e0:	e28ccb01 	add	ip, ip, #1024	; 0x400
    f3e4:	e2800c05 	add	r0, r0, #1280	; 0x500
    f3e8:	e2811d16 	add	r1, r1, #1408	; 0x580
    f3ec:	e59fb2ac 	ldr	fp, [pc, #684]	; f6a0 <sata_vs_write+0x6bc>
    f3f0:	e59d5058 	ldr	r5, [sp, #88]
    f3f4:	e58d8024 	str	r8, [sp, #36]
    f3f8:	e58dc01c 	str	ip, [sp, #28]
    f3fc:	e58d0020 	str	r0, [sp, #32]
    f400:	e58d102c 	str	r1, [sp, #44]
    f404:	e3e024fe 	mvn	r2, #-33554432	; 0xfe000000
    f408:	e1550002 	cmp	r5, r2
    f40c:	c3a09501 	movgt	r9, #4194304	; 0x400000
    f410:	c3a08008 	movgt	r8, #8	; 0x8
    f414:	c3a0a801 	movgt	sl, #65536	; 0x10000
    f418:	ca00001c 	bgt	f490 <sata_vs_write+0x4ac>
    f41c:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    f420:	e1550003 	cmp	r5, r3
    f424:	c3a09501 	movgt	r9, #4194304	; 0x400000
    f428:	c3a08004 	movgt	r8, #4	; 0x4
    f42c:	c3a0a902 	movgt	sl, #32768	; 0x8000
    f430:	ca000016 	bgt	f490 <sata_vs_write+0x4ac>
    f434:	e59fc268 	ldr	ip, [pc, #616]	; f6a4 <sata_vs_write+0x6c0>
    f438:	e155000c 	cmp	r5, ip
    f43c:	c3a09501 	movgt	r9, #4194304	; 0x400000
    f440:	c3a08002 	movgt	r8, #2	; 0x2
    f444:	c3a0a901 	movgt	sl, #16384	; 0x4000
    f448:	ca000010 	bgt	f490 <sata_vs_write+0x4ac>
    f44c:	e59f0254 	ldr	r0, [pc, #596]	; f6a8 <sata_vs_write+0x6c4>
    f450:	e1550000 	cmp	r5, r0
    f454:	c3a09501 	movgt	r9, #4194304	; 0x400000
    f458:	c3a08001 	movgt	r8, #1	; 0x1
    f45c:	c3a0aa02 	movgt	sl, #8192	; 0x2000
    f460:	ca00000a 	bgt	f490 <sata_vs_write+0x4ac>
    f464:	e59f1240 	ldr	r1, [pc, #576]	; f6ac <sata_vs_write+0x6c8>
    f468:	e2453c02 	sub	r3, r5, #512	; 0x200
    f46c:	e1530001 	cmp	r3, r1
    f470:	8a000080 	bhi	f678 <sata_vs_write+0x694>
    f474:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    f478:	e3550000 	cmp	r5, #0	; 0x0
    f47c:	e2833003 	add	r3, r3, #3	; 0x3
    f480:	a1a03005 	movge	r3, r5
    f484:	e1a0a4c3 	asr	sl, r3, #9
    f488:	e1a0948a 	lsl	r9, sl, #9
    f48c:	e3a08001 	mov	r8, #1	; 0x1
    f490:	e3a04000 	mov	r4, #0	; 0x0
    f494:	e1a0200a 	mov	r2, sl
    f498:	e59d3028 	ldr	r3, [sp, #40]
    f49c:	e59d0020 	ldr	r0, [sp, #32]
    f4a0:	e3a01035 	mov	r1, #53	; 0x35
    f4a4:	e58d4000 	str	r4, [sp]
    f4a8:	e58d4004 	str	r4, [sp, #4]
    f4ac:	ebfffde5 	bl	ec48 <CreateAtaCFIS>
    f4b0:	e1a03208 	lsl	r3, r8, #4
    f4b4:	e1a02004 	mov	r2, r4
    f4b8:	e59dc02c 	ldr	ip, [sp, #44]
    f4bc:	e7cc2004 	strb	r2, [ip, r4]
    f4c0:	e2844001 	add	r4, r4, #1	; 0x1
    f4c4:	e1540003 	cmp	r4, r3
    f4c8:	3afffffa 	bcc	f4b8 <sata_vs_write+0x4d4>
    f4cc:	e2493001 	sub	r3, r9, #1	; 0x1
    f4d0:	e3a01000 	mov	r1, #0	; 0x0
    f4d4:	e1a04503 	lsl	r4, r3, #10
    f4d8:	e59dc024 	ldr	ip, [sp, #36]
    f4dc:	ea00000b 	b	f510 <sata_vs_write+0x52c>
    f4e0:	e3a03000 	mov	r3, #0	; 0x0
    f4e4:	e58e3004 	str	r3, [lr, #4]
    f4e8:	e59d3020 	ldr	r3, [sp, #32]
    f4ec:	e783c202 	str	ip, [r3, r2, lsl #4]
    f4f0:	e5902004 	ldr	r2, [r0, #4]
    f4f4:	e08cc009 	add	ip, ip, r9
    f4f8:	e1843b22 	orr	r3, r4, r2, lsr #22
    f4fc:	e1a02563 	ror	r2, r3, #10
    f500:	e1a03c22 	lsr	r3, r2, #24
    f504:	e3c33080 	bic	r3, r3, #128	; 0x80
    f508:	e5802004 	str	r2, [r0, #4]
    f50c:	e5c03007 	strb	r3, [r0, #7]
    f510:	e59d0020 	ldr	r0, [sp, #32]
    f514:	e2812008 	add	r2, r1, #8	; 0x8
    f518:	e0803201 	add	r3, r0, r1, lsl #4
    f51c:	e080e202 	add	lr, r0, r2, lsl #4
    f520:	e1510008 	cmp	r1, r8
    f524:	e2830088 	add	r0, r3, #136	; 0x88
    f528:	e2813001 	add	r3, r1, #1	; 0x1
    f52c:	e6ff1073 	uxth	r1, r3
    f530:	3affffea 	bcc	f4e0 <sata_vs_write+0x4fc>
    f534:	e3a04000 	mov	r4, #0	; 0x0
    f538:	e1a03008 	mov	r3, r8
    f53c:	e3a0c001 	mov	ip, #1	; 0x1
    f540:	e59d0018 	ldr	r0, [sp, #24]
    f544:	e1a01004 	mov	r1, r4
    f548:	e59d2020 	ldr	r2, [sp, #32]
    f54c:	e88d1010 	stm	sp, {r4, ip}
    f550:	e58d4008 	str	r4, [sp, #8]
    f554:	e58d400c 	str	r4, [sp, #12]
    f558:	ebfffdf8 	bl	ed40 <CreateCommandHeader>
    f55c:	e1a03004 	mov	r3, r4
    f560:	e59d001c 	ldr	r0, [sp, #28]
    f564:	e7c03004 	strb	r3, [r0, r4]
    f568:	e2844001 	add	r4, r4, #1	; 0x1
    f56c:	e3540c01 	cmp	r4, #256	; 0x100
    f570:	1afffffa 	bne	f560 <sata_vs_write+0x57c>
    f574:	e59d1018 	ldr	r1, [sp, #24]
    f578:	e3e03000 	mvn	r3, #0	; 0x0
    f57c:	e58b1100 	str	r1, [fp, #256]
    f580:	e58b0108 	str	r0, [fp, #264]
    f584:	e58b3110 	str	r3, [fp, #272]
    f588:	e58b3130 	str	r3, [fp, #304]
    f58c:	e59b3118 	ldr	r3, [fp, #280]
    f590:	e3a02001 	mov	r2, #1	; 0x1
    f594:	e3833010 	orr	r3, r3, #16	; 0x10
    f598:	e58b3118 	str	r3, [fp, #280]
    f59c:	e59b3118 	ldr	r3, [fp, #280]
    f5a0:	e3833001 	orr	r3, r3, #1	; 0x1
    f5a4:	e58b3118 	str	r3, [fp, #280]
    f5a8:	e58b2138 	str	r2, [fp, #312]
    f5ac:	e59b3110 	ldr	r3, [fp, #272]
    f5b0:	e3130001 	tst	r3, #1	; 0x1
    f5b4:	0afffffc 	beq	f5ac <sata_vs_write+0x5c8>
    f5b8:	e59fc0e0 	ldr	ip, [pc, #224]	; f6a0 <sata_vs_write+0x6bc>
    f5bc:	e3e03000 	mvn	r3, #0	; 0x0
    f5c0:	e58c3110 	str	r3, [ip, #272]
    f5c4:	e58c3008 	str	r3, [ip, #8]
    f5c8:	e59c2118 	ldr	r2, [ip, #280]
    f5cc:	e30c3011 	movw	r3, #49169	; 0xc011
    f5d0:	e0023003 	and	r3, r2, r3
    f5d4:	e3530000 	cmp	r3, #0	; 0x0
    f5d8:	0a00000c 	beq	f610 <sata_vs_write+0x62c>
    f5dc:	e59c3118 	ldr	r3, [ip, #280]
    f5e0:	e3c33001 	bic	r3, r3, #1	; 0x1
    f5e4:	e58c3118 	str	r3, [ip, #280]
    f5e8:	e59b3118 	ldr	r3, [fp, #280]
    f5ec:	e3130902 	tst	r3, #32768	; 0x8000
    f5f0:	1afffffc 	bne	f5e8 <sata_vs_write+0x604>
    f5f4:	e59f00a4 	ldr	r0, [pc, #164]	; f6a0 <sata_vs_write+0x6bc>
    f5f8:	e5903118 	ldr	r3, [r0, #280]
    f5fc:	e3c33010 	bic	r3, r3, #16	; 0x10
    f600:	e5803118 	str	r3, [r0, #280]
    f604:	e59b3118 	ldr	r3, [fp, #280]
    f608:	e3130901 	tst	r3, #16384	; 0x4000
    f60c:	1afffffc 	bne	f604 <sata_vs_write+0x620>
    f610:	e59b3120 	ldr	r3, [fp, #288]
    f614:	e3130088 	tst	r3, #136	; 0x88
    f618:	1afffffc 	bne	f610 <sata_vs_write+0x62c>
    f61c:	e0010899 	mul	r1, r9, r8
    f620:	e0555001 	subs	r5, r5, r1
    f624:	0a000013 	beq	f678 <sata_vs_write+0x694>
    f628:	e59dc028 	ldr	ip, [sp, #40]
    f62c:	e1b02b85 	lsls	r2, r5, #23
    f630:	13a02001 	movne	r2, #1	; 0x1
    f634:	e08cc00a 	add	ip, ip, sl
    f638:	e3550000 	cmp	r5, #0	; 0x0
    f63c:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    f640:	e2833003 	add	r3, r3, #3	; 0x3
    f644:	e082200c 	add	r2, r2, ip
    f648:	a1a03005 	movge	r3, r5
    f64c:	e3570000 	cmp	r7, #0	; 0x0
    f650:	e58dc028 	str	ip, [sp, #40]
    f654:	e08234c3 	add	r3, r2, r3, asr #9
    f658:	8a000002 	bhi	f668 <sata_vs_write+0x684>
    f65c:	1a000005 	bne	f678 <sata_vs_write+0x694>
    f660:	e1560003 	cmp	r6, r3
    f664:	9a000003 	bls	f678 <sata_vs_write+0x694>
    f668:	e59d0024 	ldr	r0, [sp, #36]
    f66c:	e0800001 	add	r0, r0, r1
    f670:	e58d0024 	str	r0, [sp, #36]
    f674:	eaffff62 	b	f404 <sata_vs_write+0x420>
    f678:	e59d1058 	ldr	r1, [sp, #88]
    f67c:	e59d0014 	ldr	r0, [sp, #20]
    f680:	e0654001 	rsb	r4, r5, r1
    f684:	ebffc87f 	bl	1888 <free>
    f688:	e1a00004 	mov	r0, r4
    f68c:	e28dd034 	add	sp, sp, #52	; 0x34
    f690:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    f694:	000188f9 	.word	0x000188f9
    f698:	3c00d6cc 	.word	0x3c00d6cc
    f69c:	0001891c 	.word	0x0001891c
    f6a0:	28200000 	.word	0x28200000
    f6a4:	007fffff 	.word	0x007fffff
    f6a8:	003fffff 	.word	0x003fffff
    f6ac:	003ffdff 	.word	0x003ffdff

0000f6b0 <sata_vs_read>:
    f6b0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    f6b4:	e1a08000 	mov	r8, r0
    f6b8:	e24dd034 	sub	sp, sp, #52	; 0x34
    f6bc:	e3a00c0e 	mov	r0, #3584	; 0xe00
    f6c0:	e1a06002 	mov	r6, r2
    f6c4:	e1a07003 	mov	r7, r3
    f6c8:	ebffc806 	bl	16e8 <malloc>
    f6cc:	e3500000 	cmp	r0, #0	; 0x0
    f6d0:	e58d0014 	str	r0, [sp, #20]
    f6d4:	1a000004 	bne	f6ec <sata_vs_read+0x3c>
    f6d8:	e59f0680 	ldr	r0, [pc, #1664]	; fd60 <sata_vs_read+0x6b0>
    f6dc:	e59f3680 	ldr	r3, [pc, #1664]	; fd64 <sata_vs_read+0x6b4>
    f6e0:	e1a0e00f 	mov	lr, pc
    f6e4:	e593f000 	ldr	pc, [r3]
    f6e8:	ea0000ea 	b	fa98 <sata_vs_read+0x3e8>
    f6ec:	e59d0014 	ldr	r0, [sp, #20]
    f6f0:	e2803fff 	add	r3, r0, #1020	; 0x3fc
    f6f4:	e2833003 	add	r3, r3, #3	; 0x3
    f6f8:	e3c33fff 	bic	r3, r3, #1020	; 0x3fc
    f6fc:	e3c33003 	bic	r3, r3, #3	; 0x3
    f700:	e1530000 	cmp	r3, r0
    f704:	e58d3018 	str	r3, [sp, #24]
    f708:	02833a01 	addeq	r3, r3, #4096	; 0x1000
    f70c:	058d3018 	streq	r3, [sp, #24]
    f710:	e59d0018 	ldr	r0, [sp, #24]
    f714:	ebfffdcb 	bl	ee48 <get_max_lba>
    f718:	e59d1058 	ldr	r1, [sp, #88]
    f71c:	e1a0c000 	mov	ip, r0
    f720:	e1a03b81 	lsl	r3, r1, #23
    f724:	e1a03ba3 	lsr	r3, r3, #23
    f728:	e3530000 	cmp	r3, #0	; 0x0
    f72c:	1a0000d3 	bne	fa80 <sata_vs_read+0x3d0>
    f730:	e30041ff 	movw	r4, #511	; 0x1ff
    f734:	e3a05000 	mov	r5, #0	; 0x0
    f738:	e0062004 	and	r2, r6, r4
    f73c:	e0073005 	and	r3, r7, r5
    f740:	e1920003 	orrs	r0, r2, r3
    f744:	1a0000cd 	bne	fa80 <sata_vs_read+0x3d0>
    f748:	e3180001 	tst	r8, #1	; 0x1
    f74c:	1a0000cb 	bne	fa80 <sata_vs_read+0x3d0>
    f750:	e1a00007 	mov	r0, r7
    f754:	e1a01fc0 	asr	r1, r0, #31
    f758:	e1a02fc1 	asr	r2, r1, #31
    f75c:	e1a03002 	mov	r3, r2
    f760:	e0022004 	and	r2, r2, r4
    f764:	e0922006 	adds	r2, r2, r6
    f768:	e0033005 	and	r3, r3, r5
    f76c:	e0a33007 	adc	r3, r3, r7
    f770:	e1a014a2 	lsr	r1, r2, #9
    f774:	e1a0600c 	mov	r6, ip
    f778:	e1a07fc6 	asr	r7, r6, #31
    f77c:	e1811b83 	orr	r1, r1, r3, lsl #23
    f780:	e3570000 	cmp	r7, #0	; 0x0
    f784:	e58d1028 	str	r1, [sp, #40]
    f788:	e1a00001 	mov	r0, r1
    f78c:	e3a01000 	mov	r1, #0	; 0x0
    f790:	8a000002 	bhi	f7a0 <sata_vs_read+0xf0>
    f794:	1a0000b9 	bne	fa80 <sata_vs_read+0x3d0>
    f798:	e1560000 	cmp	r6, r0
    f79c:	9a0000b7 	bls	fa80 <sata_vs_read+0x3d0>
    f7a0:	e59d2058 	ldr	r2, [sp, #88]
    f7a4:	e3520402 	cmp	r2, #33554432	; 0x2000000
    f7a8:	ba000007 	blt	f7cc <sata_vs_read+0x11c>
    f7ac:	e1a02006 	mov	r2, r6
    f7b0:	e1a03007 	mov	r3, r7
    f7b4:	e0522000 	subs	r2, r2, r0
    f7b8:	e0c33001 	sbc	r3, r3, r1
    f7bc:	e3530000 	cmp	r3, #0	; 0x0
    f7c0:	1a000001 	bne	f7cc <sata_vs_read+0x11c>
    f7c4:	e3520801 	cmp	r2, #65536	; 0x10000
    f7c8:	3a0000ac 	bcc	fa80 <sata_vs_read+0x3d0>
    f7cc:	e59d3058 	ldr	r3, [sp, #88]
    f7d0:	e3530401 	cmp	r3, #16777216	; 0x1000000
    f7d4:	ba000007 	blt	f7f8 <sata_vs_read+0x148>
    f7d8:	e1a02006 	mov	r2, r6
    f7dc:	e1a03007 	mov	r3, r7
    f7e0:	e0522000 	subs	r2, r2, r0
    f7e4:	e0c33001 	sbc	r3, r3, r1
    f7e8:	e3530000 	cmp	r3, #0	; 0x0
    f7ec:	1a000001 	bne	f7f8 <sata_vs_read+0x148>
    f7f0:	e3520902 	cmp	r2, #32768	; 0x8000
    f7f4:	3a0000a1 	bcc	fa80 <sata_vs_read+0x3d0>
    f7f8:	e59d2058 	ldr	r2, [sp, #88]
    f7fc:	e3520502 	cmp	r2, #8388608	; 0x800000
    f800:	ba000007 	blt	f824 <sata_vs_read+0x174>
    f804:	e1a02006 	mov	r2, r6
    f808:	e1a03007 	mov	r3, r7
    f80c:	e0522000 	subs	r2, r2, r0
    f810:	e0c33001 	sbc	r3, r3, r1
    f814:	e3530000 	cmp	r3, #0	; 0x0
    f818:	1a000001 	bne	f824 <sata_vs_read+0x174>
    f81c:	e3520901 	cmp	r2, #16384	; 0x4000
    f820:	3a000096 	bcc	fa80 <sata_vs_read+0x3d0>
    f824:	e59d3058 	ldr	r3, [sp, #88]
    f828:	e3530501 	cmp	r3, #4194304	; 0x400000
    f82c:	ba000007 	blt	f850 <sata_vs_read+0x1a0>
    f830:	e1a02006 	mov	r2, r6
    f834:	e1a03007 	mov	r3, r7
    f838:	e0522000 	subs	r2, r2, r0
    f83c:	e0c33001 	sbc	r3, r3, r1
    f840:	e3530000 	cmp	r3, #0	; 0x0
    f844:	1a000001 	bne	f850 <sata_vs_read+0x1a0>
    f848:	e3520a02 	cmp	r2, #8192	; 0x2000
    f84c:	3a00008b 	bcc	fa80 <sata_vs_read+0x3d0>
    f850:	e59d2058 	ldr	r2, [sp, #88]
    f854:	e3520602 	cmp	r2, #2097152	; 0x200000
    f858:	ba000007 	blt	f87c <sata_vs_read+0x1cc>
    f85c:	e1a02006 	mov	r2, r6
    f860:	e1a03007 	mov	r3, r7
    f864:	e0522000 	subs	r2, r2, r0
    f868:	e0c33001 	sbc	r3, r3, r1
    f86c:	e3530000 	cmp	r3, #0	; 0x0
    f870:	1a000001 	bne	f87c <sata_vs_read+0x1cc>
    f874:	e3520a01 	cmp	r2, #4096	; 0x1000
    f878:	3a000080 	bcc	fa80 <sata_vs_read+0x3d0>
    f87c:	e59d3058 	ldr	r3, [sp, #88]
    f880:	e3530601 	cmp	r3, #1048576	; 0x100000
    f884:	ba000007 	blt	f8a8 <sata_vs_read+0x1f8>
    f888:	e1a02006 	mov	r2, r6
    f88c:	e1a03007 	mov	r3, r7
    f890:	e0522000 	subs	r2, r2, r0
    f894:	e0c33001 	sbc	r3, r3, r1
    f898:	e3530000 	cmp	r3, #0	; 0x0
    f89c:	1a000001 	bne	f8a8 <sata_vs_read+0x1f8>
    f8a0:	e3520b02 	cmp	r2, #2048	; 0x800
    f8a4:	3a000075 	bcc	fa80 <sata_vs_read+0x3d0>
    f8a8:	e59d2058 	ldr	r2, [sp, #88]
    f8ac:	e3520702 	cmp	r2, #524288	; 0x80000
    f8b0:	ba000007 	blt	f8d4 <sata_vs_read+0x224>
    f8b4:	e1a02006 	mov	r2, r6
    f8b8:	e1a03007 	mov	r3, r7
    f8bc:	e0522000 	subs	r2, r2, r0
    f8c0:	e0c33001 	sbc	r3, r3, r1
    f8c4:	e3530000 	cmp	r3, #0	; 0x0
    f8c8:	1a000001 	bne	f8d4 <sata_vs_read+0x224>
    f8cc:	e3520b01 	cmp	r2, #1024	; 0x400
    f8d0:	3a00006a 	bcc	fa80 <sata_vs_read+0x3d0>
    f8d4:	e59d3058 	ldr	r3, [sp, #88]
    f8d8:	e3530701 	cmp	r3, #262144	; 0x40000
    f8dc:	ba000007 	blt	f900 <sata_vs_read+0x250>
    f8e0:	e1a02006 	mov	r2, r6
    f8e4:	e1a03007 	mov	r3, r7
    f8e8:	e0522000 	subs	r2, r2, r0
    f8ec:	e0c33001 	sbc	r3, r3, r1
    f8f0:	e3530000 	cmp	r3, #0	; 0x0
    f8f4:	1a000001 	bne	f900 <sata_vs_read+0x250>
    f8f8:	e3520c02 	cmp	r2, #512	; 0x200
    f8fc:	3a00005f 	bcc	fa80 <sata_vs_read+0x3d0>
    f900:	e59d2058 	ldr	r2, [sp, #88]
    f904:	e3520802 	cmp	r2, #131072	; 0x20000
    f908:	ba000008 	blt	f930 <sata_vs_read+0x280>
    f90c:	e3e020ff 	mvn	r2, #255	; 0xff
    f910:	e0922006 	adds	r2, r2, r6
    f914:	e3e03000 	mvn	r3, #0	; 0x0
    f918:	e0a33007 	adc	r3, r3, r7
    f91c:	e1510003 	cmp	r1, r3
    f920:	8a000056 	bhi	fa80 <sata_vs_read+0x3d0>
    f924:	1a000001 	bne	f930 <sata_vs_read+0x280>
    f928:	e1500002 	cmp	r0, r2
    f92c:	8a000053 	bhi	fa80 <sata_vs_read+0x3d0>
    f930:	e59d3058 	ldr	r3, [sp, #88]
    f934:	e3530801 	cmp	r3, #65536	; 0x10000
    f938:	ba000007 	blt	f95c <sata_vs_read+0x2ac>
    f93c:	e1a02006 	mov	r2, r6
    f940:	e1a03007 	mov	r3, r7
    f944:	e0522000 	subs	r2, r2, r0
    f948:	e0c33001 	sbc	r3, r3, r1
    f94c:	e3530000 	cmp	r3, #0	; 0x0
    f950:	1a000001 	bne	f95c <sata_vs_read+0x2ac>
    f954:	e352007f 	cmp	r2, #127	; 0x7f
    f958:	9a000048 	bls	fa80 <sata_vs_read+0x3d0>
    f95c:	e59d2058 	ldr	r2, [sp, #88]
    f960:	e3520902 	cmp	r2, #32768	; 0x8000
    f964:	ba000007 	blt	f988 <sata_vs_read+0x2d8>
    f968:	e1a02006 	mov	r2, r6
    f96c:	e1a03007 	mov	r3, r7
    f970:	e0522000 	subs	r2, r2, r0
    f974:	e0c33001 	sbc	r3, r3, r1
    f978:	e3530000 	cmp	r3, #0	; 0x0
    f97c:	1a000001 	bne	f988 <sata_vs_read+0x2d8>
    f980:	e352003f 	cmp	r2, #63	; 0x3f
    f984:	9a00003d 	bls	fa80 <sata_vs_read+0x3d0>
    f988:	e59d3058 	ldr	r3, [sp, #88]
    f98c:	e3530901 	cmp	r3, #16384	; 0x4000
    f990:	ba000007 	blt	f9b4 <sata_vs_read+0x304>
    f994:	e1a02006 	mov	r2, r6
    f998:	e1a03007 	mov	r3, r7
    f99c:	e0522000 	subs	r2, r2, r0
    f9a0:	e0c33001 	sbc	r3, r3, r1
    f9a4:	e3530000 	cmp	r3, #0	; 0x0
    f9a8:	1a000001 	bne	f9b4 <sata_vs_read+0x304>
    f9ac:	e352001f 	cmp	r2, #31	; 0x1f
    f9b0:	9a000032 	bls	fa80 <sata_vs_read+0x3d0>
    f9b4:	e59d2058 	ldr	r2, [sp, #88]
    f9b8:	e3520a02 	cmp	r2, #8192	; 0x2000
    f9bc:	ba000007 	blt	f9e0 <sata_vs_read+0x330>
    f9c0:	e1a02006 	mov	r2, r6
    f9c4:	e1a03007 	mov	r3, r7
    f9c8:	e0522000 	subs	r2, r2, r0
    f9cc:	e0c33001 	sbc	r3, r3, r1
    f9d0:	e3530000 	cmp	r3, #0	; 0x0
    f9d4:	1a000001 	bne	f9e0 <sata_vs_read+0x330>
    f9d8:	e352000f 	cmp	r2, #15	; 0xf
    f9dc:	9a000027 	bls	fa80 <sata_vs_read+0x3d0>
    f9e0:	e59d3058 	ldr	r3, [sp, #88]
    f9e4:	e3530a01 	cmp	r3, #4096	; 0x1000
    f9e8:	ba000007 	blt	fa0c <sata_vs_read+0x35c>
    f9ec:	e1a02006 	mov	r2, r6
    f9f0:	e1a03007 	mov	r3, r7
    f9f4:	e0522000 	subs	r2, r2, r0
    f9f8:	e0c33001 	sbc	r3, r3, r1
    f9fc:	e3530000 	cmp	r3, #0	; 0x0
    fa00:	1a000001 	bne	fa0c <sata_vs_read+0x35c>
    fa04:	e3520007 	cmp	r2, #7	; 0x7
    fa08:	9a00001c 	bls	fa80 <sata_vs_read+0x3d0>
    fa0c:	e59d2058 	ldr	r2, [sp, #88]
    fa10:	e3520b02 	cmp	r2, #2048	; 0x800
    fa14:	ba000007 	blt	fa38 <sata_vs_read+0x388>
    fa18:	e1a02006 	mov	r2, r6
    fa1c:	e1a03007 	mov	r3, r7
    fa20:	e0522000 	subs	r2, r2, r0
    fa24:	e0c33001 	sbc	r3, r3, r1
    fa28:	e3530000 	cmp	r3, #0	; 0x0
    fa2c:	1a000001 	bne	fa38 <sata_vs_read+0x388>
    fa30:	e3520003 	cmp	r2, #3	; 0x3
    fa34:	9a000011 	bls	fa80 <sata_vs_read+0x3d0>
    fa38:	e59d3058 	ldr	r3, [sp, #88]
    fa3c:	e3530b01 	cmp	r3, #1024	; 0x400
    fa40:	ba000007 	blt	fa64 <sata_vs_read+0x3b4>
    fa44:	e1a02006 	mov	r2, r6
    fa48:	e1a03007 	mov	r3, r7
    fa4c:	e0522000 	subs	r2, r2, r0
    fa50:	e0c33001 	sbc	r3, r3, r1
    fa54:	e3530000 	cmp	r3, #0	; 0x0
    fa58:	1a000001 	bne	fa64 <sata_vs_read+0x3b4>
    fa5c:	e3520001 	cmp	r2, #1	; 0x1
    fa60:	9a000006 	bls	fa80 <sata_vs_read+0x3d0>
    fa64:	e59d2058 	ldr	r2, [sp, #88]
    fa68:	e3520c02 	cmp	r2, #512	; 0x200
    fa6c:	ba00000b 	blt	faa0 <sata_vs_read+0x3f0>
    fa70:	e1560000 	cmp	r6, r0
    fa74:	1a000009 	bne	faa0 <sata_vs_read+0x3f0>
    fa78:	e1570001 	cmp	r7, r1
    fa7c:	1a000007 	bne	faa0 <sata_vs_read+0x3f0>
    fa80:	e59f02e0 	ldr	r0, [pc, #736]	; fd68 <sata_vs_read+0x6b8>
    fa84:	e59f32d8 	ldr	r3, [pc, #728]	; fd64 <sata_vs_read+0x6b4>
    fa88:	e1a0e00f 	mov	lr, pc
    fa8c:	e593f000 	ldr	pc, [r3]
    fa90:	e59d0014 	ldr	r0, [sp, #20]
    fa94:	ebffc77b 	bl	1888 <free>
    fa98:	e3e04000 	mvn	r4, #0	; 0x0
    fa9c:	ea0000ac 	b	fd54 <sata_vs_read+0x6a4>
    faa0:	e59d3018 	ldr	r3, [sp, #24]
    faa4:	e59d0018 	ldr	r0, [sp, #24]
    faa8:	e59d1018 	ldr	r1, [sp, #24]
    faac:	e2833b01 	add	r3, r3, #1024	; 0x400
    fab0:	e2800c05 	add	r0, r0, #1280	; 0x500
    fab4:	e2811d16 	add	r1, r1, #1408	; 0x580
    fab8:	e59fb2ac 	ldr	fp, [pc, #684]	; fd6c <sata_vs_read+0x6bc>
    fabc:	e59d5058 	ldr	r5, [sp, #88]
    fac0:	e58d8024 	str	r8, [sp, #36]
    fac4:	e58d301c 	str	r3, [sp, #28]
    fac8:	e58d0020 	str	r0, [sp, #32]
    facc:	e58d102c 	str	r1, [sp, #44]
    fad0:	e3e024fe 	mvn	r2, #-33554432	; 0xfe000000
    fad4:	e1550002 	cmp	r5, r2
    fad8:	c3a09501 	movgt	r9, #4194304	; 0x400000
    fadc:	c3a08008 	movgt	r8, #8	; 0x8
    fae0:	c3a0a801 	movgt	sl, #65536	; 0x10000
    fae4:	ca00001c 	bgt	fb5c <sata_vs_read+0x4ac>
    fae8:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    faec:	e1550003 	cmp	r5, r3
    faf0:	c3a09501 	movgt	r9, #4194304	; 0x400000
    faf4:	c3a08004 	movgt	r8, #4	; 0x4
    faf8:	c3a0a902 	movgt	sl, #32768	; 0x8000
    fafc:	ca000016 	bgt	fb5c <sata_vs_read+0x4ac>
    fb00:	e59f0268 	ldr	r0, [pc, #616]	; fd70 <sata_vs_read+0x6c0>
    fb04:	e1550000 	cmp	r5, r0
    fb08:	c3a09501 	movgt	r9, #4194304	; 0x400000
    fb0c:	c3a08002 	movgt	r8, #2	; 0x2
    fb10:	c3a0a901 	movgt	sl, #16384	; 0x4000
    fb14:	ca000010 	bgt	fb5c <sata_vs_read+0x4ac>
    fb18:	e59f1254 	ldr	r1, [pc, #596]	; fd74 <sata_vs_read+0x6c4>
    fb1c:	e1550001 	cmp	r5, r1
    fb20:	c3a09501 	movgt	r9, #4194304	; 0x400000
    fb24:	c3a08001 	movgt	r8, #1	; 0x1
    fb28:	c3a0aa02 	movgt	sl, #8192	; 0x2000
    fb2c:	ca00000a 	bgt	fb5c <sata_vs_read+0x4ac>
    fb30:	e59f2240 	ldr	r2, [pc, #576]	; fd78 <sata_vs_read+0x6c8>
    fb34:	e2453c02 	sub	r3, r5, #512	; 0x200
    fb38:	e1530002 	cmp	r3, r2
    fb3c:	8a000080 	bhi	fd44 <sata_vs_read+0x694>
    fb40:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    fb44:	e3550000 	cmp	r5, #0	; 0x0
    fb48:	e2833003 	add	r3, r3, #3	; 0x3
    fb4c:	a1a03005 	movge	r3, r5
    fb50:	e1a0a4c3 	asr	sl, r3, #9
    fb54:	e1a0948a 	lsl	r9, sl, #9
    fb58:	e3a08001 	mov	r8, #1	; 0x1
    fb5c:	e3a04000 	mov	r4, #0	; 0x0
    fb60:	e1a0200a 	mov	r2, sl
    fb64:	e59d3028 	ldr	r3, [sp, #40]
    fb68:	e59d0020 	ldr	r0, [sp, #32]
    fb6c:	e3a01025 	mov	r1, #37	; 0x25
    fb70:	e58d4000 	str	r4, [sp]
    fb74:	e58d4004 	str	r4, [sp, #4]
    fb78:	ebfffc32 	bl	ec48 <CreateAtaCFIS>
    fb7c:	e1a03208 	lsl	r3, r8, #4
    fb80:	e1a02004 	mov	r2, r4
    fb84:	e59d002c 	ldr	r0, [sp, #44]
    fb88:	e7c02004 	strb	r2, [r0, r4]
    fb8c:	e2844001 	add	r4, r4, #1	; 0x1
    fb90:	e1540003 	cmp	r4, r3
    fb94:	3afffffa 	bcc	fb84 <sata_vs_read+0x4d4>
    fb98:	e2493001 	sub	r3, r9, #1	; 0x1
    fb9c:	e3a01000 	mov	r1, #0	; 0x0
    fba0:	e1a04503 	lsl	r4, r3, #10
    fba4:	e59dc024 	ldr	ip, [sp, #36]
    fba8:	ea00000b 	b	fbdc <sata_vs_read+0x52c>
    fbac:	e3a03000 	mov	r3, #0	; 0x0
    fbb0:	e58e3004 	str	r3, [lr, #4]
    fbb4:	e59d3020 	ldr	r3, [sp, #32]
    fbb8:	e783c202 	str	ip, [r3, r2, lsl #4]
    fbbc:	e5902004 	ldr	r2, [r0, #4]
    fbc0:	e08cc009 	add	ip, ip, r9
    fbc4:	e1843b22 	orr	r3, r4, r2, lsr #22
    fbc8:	e1a02563 	ror	r2, r3, #10
    fbcc:	e1a03c22 	lsr	r3, r2, #24
    fbd0:	e3c33080 	bic	r3, r3, #128	; 0x80
    fbd4:	e5802004 	str	r2, [r0, #4]
    fbd8:	e5c03007 	strb	r3, [r0, #7]
    fbdc:	e59d0020 	ldr	r0, [sp, #32]
    fbe0:	e2812008 	add	r2, r1, #8	; 0x8
    fbe4:	e0803201 	add	r3, r0, r1, lsl #4
    fbe8:	e080e202 	add	lr, r0, r2, lsl #4
    fbec:	e1510008 	cmp	r1, r8
    fbf0:	e2830088 	add	r0, r3, #136	; 0x88
    fbf4:	e2813001 	add	r3, r1, #1	; 0x1
    fbf8:	e6ff1073 	uxth	r1, r3
    fbfc:	3affffea 	bcc	fbac <sata_vs_read+0x4fc>
    fc00:	e3a04000 	mov	r4, #0	; 0x0
    fc04:	e1a03008 	mov	r3, r8
    fc08:	e59d0018 	ldr	r0, [sp, #24]
    fc0c:	e1a01004 	mov	r1, r4
    fc10:	e59d2020 	ldr	r2, [sp, #32]
    fc14:	e58d4000 	str	r4, [sp]
    fc18:	e58d4004 	str	r4, [sp, #4]
    fc1c:	e58d4008 	str	r4, [sp, #8]
    fc20:	e58d400c 	str	r4, [sp, #12]
    fc24:	ebfffc45 	bl	ed40 <CreateCommandHeader>
    fc28:	e1a03004 	mov	r3, r4
    fc2c:	e59d101c 	ldr	r1, [sp, #28]
    fc30:	e7c13004 	strb	r3, [r1, r4]
    fc34:	e2844001 	add	r4, r4, #1	; 0x1
    fc38:	e3540c01 	cmp	r4, #256	; 0x100
    fc3c:	1afffffa 	bne	fc2c <sata_vs_read+0x57c>
    fc40:	e59d2018 	ldr	r2, [sp, #24]
    fc44:	e3e03000 	mvn	r3, #0	; 0x0
    fc48:	e58b2100 	str	r2, [fp, #256]
    fc4c:	e58b1108 	str	r1, [fp, #264]
    fc50:	e58b3110 	str	r3, [fp, #272]
    fc54:	e58b3130 	str	r3, [fp, #304]
    fc58:	e59b3118 	ldr	r3, [fp, #280]
    fc5c:	e3833010 	orr	r3, r3, #16	; 0x10
    fc60:	e58b3118 	str	r3, [fp, #280]
    fc64:	e59b3118 	ldr	r3, [fp, #280]
    fc68:	e3833001 	orr	r3, r3, #1	; 0x1
    fc6c:	e58b3118 	str	r3, [fp, #280]
    fc70:	e3a03001 	mov	r3, #1	; 0x1
    fc74:	e58b3138 	str	r3, [fp, #312]
    fc78:	e59b3110 	ldr	r3, [fp, #272]
    fc7c:	e3130001 	tst	r3, #1	; 0x1
    fc80:	0afffffc 	beq	fc78 <sata_vs_read+0x5c8>
    fc84:	e59f00e0 	ldr	r0, [pc, #224]	; fd6c <sata_vs_read+0x6bc>
    fc88:	e3e03000 	mvn	r3, #0	; 0x0
    fc8c:	e5803110 	str	r3, [r0, #272]
    fc90:	e5803008 	str	r3, [r0, #8]
    fc94:	e5902118 	ldr	r2, [r0, #280]
    fc98:	e30c3011 	movw	r3, #49169	; 0xc011
    fc9c:	e0023003 	and	r3, r2, r3
    fca0:	e3530000 	cmp	r3, #0	; 0x0
    fca4:	0a00000c 	beq	fcdc <sata_vs_read+0x62c>
    fca8:	e5903118 	ldr	r3, [r0, #280]
    fcac:	e3c33001 	bic	r3, r3, #1	; 0x1
    fcb0:	e5803118 	str	r3, [r0, #280]
    fcb4:	e59b3118 	ldr	r3, [fp, #280]
    fcb8:	e3130902 	tst	r3, #32768	; 0x8000
    fcbc:	1afffffc 	bne	fcb4 <sata_vs_read+0x604>
    fcc0:	e59f10a4 	ldr	r1, [pc, #164]	; fd6c <sata_vs_read+0x6bc>
    fcc4:	e5913118 	ldr	r3, [r1, #280]
    fcc8:	e3c33010 	bic	r3, r3, #16	; 0x10
    fccc:	e5813118 	str	r3, [r1, #280]
    fcd0:	e59b3118 	ldr	r3, [fp, #280]
    fcd4:	e3130901 	tst	r3, #16384	; 0x4000
    fcd8:	1afffffc 	bne	fcd0 <sata_vs_read+0x620>
    fcdc:	e59b3120 	ldr	r3, [fp, #288]
    fce0:	e3130088 	tst	r3, #136	; 0x88
    fce4:	1afffffc 	bne	fcdc <sata_vs_read+0x62c>
    fce8:	e0010899 	mul	r1, r9, r8
    fcec:	e0555001 	subs	r5, r5, r1
    fcf0:	0a000013 	beq	fd44 <sata_vs_read+0x694>
    fcf4:	e59d0028 	ldr	r0, [sp, #40]
    fcf8:	e1b02b85 	lsls	r2, r5, #23
    fcfc:	13a02001 	movne	r2, #1	; 0x1
    fd00:	e080000a 	add	r0, r0, sl
    fd04:	e3550000 	cmp	r5, #0	; 0x0
    fd08:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    fd0c:	e2833003 	add	r3, r3, #3	; 0x3
    fd10:	e0822000 	add	r2, r2, r0
    fd14:	a1a03005 	movge	r3, r5
    fd18:	e3570000 	cmp	r7, #0	; 0x0
    fd1c:	e58d0028 	str	r0, [sp, #40]
    fd20:	e08234c3 	add	r3, r2, r3, asr #9
    fd24:	8a000002 	bhi	fd34 <sata_vs_read+0x684>
    fd28:	1a000005 	bne	fd44 <sata_vs_read+0x694>
    fd2c:	e1560003 	cmp	r6, r3
    fd30:	9a000003 	bls	fd44 <sata_vs_read+0x694>
    fd34:	e59d2024 	ldr	r2, [sp, #36]
    fd38:	e0822001 	add	r2, r2, r1
    fd3c:	e58d2024 	str	r2, [sp, #36]
    fd40:	eaffff62 	b	fad0 <sata_vs_read+0x420>
    fd44:	e59d3058 	ldr	r3, [sp, #88]
    fd48:	e59d0014 	ldr	r0, [sp, #20]
    fd4c:	e0654003 	rsb	r4, r5, r3
    fd50:	ebffc6cc 	bl	1888 <free>
    fd54:	e1a00004 	mov	r0, r4
    fd58:	e28dd034 	add	sp, sp, #52	; 0x34
    fd5c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    fd60:	0001893f 	.word	0x0001893f
    fd64:	3c00d6cc 	.word	0x3c00d6cc
    fd68:	0001891c 	.word	0x0001891c
    fd6c:	28200000 	.word	0x28200000
    fd70:	007fffff 	.word	0x007fffff
    fd74:	003fffff 	.word	0x003fffff
    fd78:	003ffdff 	.word	0x003ffdff

0000fd7c <sata_vs_reset>:
    fd7c:	e92d4070 	push	{r4, r5, r6, lr}
    fd80:	e59f02d4 	ldr	r0, [pc, #724]	; 1005c <sata_vs_reset+0x2e0>
    fd84:	ebffc27d 	bl	780 <module_enable>
    fd88:	e59f32d0 	ldr	r3, [pc, #720]	; 10060 <sata_vs_reset+0x2e4>
    fd8c:	e3a01000 	mov	r1, #0	; 0x0
    fd90:	e3a0c001 	mov	ip, #1	; 0x1
    fd94:	e3a06010 	mov	r6, #16	; 0x10
    fd98:	e3a02006 	mov	r2, #6	; 0x6
    fd9c:	e583c830 	str	ip, [r3, #2096]
    fda0:	e3a04002 	mov	r4, #2	; 0x2
    fda4:	e5836820 	str	r6, [r3, #2080]
    fda8:	e5831824 	str	r1, [r3, #2084]
    fdac:	e5832828 	str	r2, [r3, #2088]
    fdb0:	e282200c 	add	r2, r2, #12	; 0xc
    fdb4:	e583482c 	str	r4, [r3, #2092]
    fdb8:	e5834834 	str	r4, [r3, #2100]
    fdbc:	e5831838 	str	r1, [r3, #2104]
    fdc0:	e583183c 	str	r1, [r3, #2108]
    fdc4:	e5831840 	str	r1, [r3, #2112]
    fdc8:	e5832844 	str	r2, [r3, #2116]
    fdcc:	e3a02009 	mov	r2, #9	; 0x9
    fdd0:	e583c848 	str	ip, [r3, #2120]
    fdd4:	e3a05003 	mov	r5, #3	; 0x3
    fdd8:	e583184c 	str	r1, [r3, #2124]
    fddc:	e5832850 	str	r2, [r3, #2128]
    fde0:	e082200c 	add	r2, r2, ip
    fde4:	e5831854 	str	r1, [r3, #2132]
    fde8:	e3040e20 	movw	r0, #20000	; 0x4e20
    fdec:	e5832858 	str	r2, [r3, #2136]
    fdf0:	e583585c 	str	r5, [r3, #2140]
    fdf4:	e583c860 	str	ip, [r3, #2144]
    fdf8:	e5831864 	str	r1, [r3, #2148]
    fdfc:	e5831868 	str	r1, [r3, #2152]
    fe00:	e583486c 	str	r4, [r3, #2156]
    fe04:	e583c870 	str	ip, [r3, #2160]
    fe08:	e5835874 	str	r5, [r3, #2164]
    fe0c:	e5836878 	str	r6, [r3, #2168]
    fe10:	e5831830 	str	r1, [r3, #2096]
    fe14:	ebfff6f0 	bl	d9dc <udelay>
    fe18:	e59f2244 	ldr	r2, [pc, #580]	; 10064 <sata_vs_reset+0x2e8>
    fe1c:	e5923004 	ldr	r3, [r2, #4]
    fe20:	e1a01002 	mov	r1, r2
    fe24:	e3833001 	orr	r3, r3, #1	; 0x1
    fe28:	e5823004 	str	r3, [r2, #4]
    fe2c:	e5913004 	ldr	r3, [r1, #4]
    fe30:	e59f222c 	ldr	r2, [pc, #556]	; 10064 <sata_vs_reset+0x2e8>
    fe34:	e3130001 	tst	r3, #1	; 0x1
    fe38:	1afffffb 	bne	fe2c <sata_vs_reset+0xb0>
    fe3c:	e5923000 	ldr	r3, [r2]
    fe40:	e3a00b03 	mov	r0, #3072	; 0xc00
    fe44:	e3833306 	orr	r3, r3, #402653184	; 0x18000000
    fe48:	e5823000 	str	r3, [r2]
    fe4c:	e3a03001 	mov	r3, #1	; 0x1
    fe50:	e582300c 	str	r3, [r2, #12]
    fe54:	e5923118 	ldr	r3, [r2, #280]
    fe58:	e3833707 	orr	r3, r3, #1835008	; 0x1c0000
    fe5c:	e5823118 	str	r3, [r2, #280]
    fe60:	ebffc620 	bl	16e8 <malloc>
    fe64:	e2505000 	subs	r5, r0, #0	; 0x0
    fe68:	1a000004 	bne	fe80 <sata_vs_reset+0x104>
    fe6c:	e59f01f4 	ldr	r0, [pc, #500]	; 10068 <sata_vs_reset+0x2ec>
    fe70:	e59f31f4 	ldr	r3, [pc, #500]	; 1006c <sata_vs_reset+0x2f0>
    fe74:	e1a0e00f 	mov	lr, pc
    fe78:	e593f000 	ldr	pc, [r3]
    fe7c:	ea000041 	b	ff88 <sata_vs_reset+0x20c>
    fe80:	e2853fff 	add	r3, r5, #1020	; 0x3fc
    fe84:	e2833003 	add	r3, r3, #3	; 0x3
    fe88:	e59f11d4 	ldr	r1, [pc, #468]	; 10064 <sata_vs_reset+0x2e8>
    fe8c:	e3c30fff 	bic	r0, r3, #1020	; 0x3fc
    fe90:	e3c00003 	bic	r0, r0, #3	; 0x3
    fe94:	e5913000 	ldr	r3, [r1]
    fe98:	e1500005 	cmp	r0, r5
    fe9c:	02800a01 	addeq	r0, r0, #4096	; 0x1000
    fea0:	e3130302 	tst	r3, #134217728	; 0x8000000
    fea4:	0a00002e 	beq	ff64 <sata_vs_reset+0x1e8>
    fea8:	e5912118 	ldr	r2, [r1, #280]
    feac:	e30c3011 	movw	r3, #49169	; 0xc011
    feb0:	e0023003 	and	r3, r2, r3
    feb4:	e3530000 	cmp	r3, #0	; 0x0
    feb8:	0a00000e 	beq	fef8 <sata_vs_reset+0x17c>
    febc:	e5913118 	ldr	r3, [r1, #280]
    fec0:	e1a0c001 	mov	ip, r1
    fec4:	e3c33001 	bic	r3, r3, #1	; 0x1
    fec8:	e5813118 	str	r3, [r1, #280]
    fecc:	e59c3118 	ldr	r3, [ip, #280]
    fed0:	e59f218c 	ldr	r2, [pc, #396]	; 10064 <sata_vs_reset+0x2e8>
    fed4:	e3130902 	tst	r3, #32768	; 0x8000
    fed8:	1afffffb 	bne	fecc <sata_vs_reset+0x150>
    fedc:	e5923118 	ldr	r3, [r2, #280]
    fee0:	e1a01002 	mov	r1, r2
    fee4:	e3c33010 	bic	r3, r3, #16	; 0x10
    fee8:	e5823118 	str	r3, [r2, #280]
    feec:	e5913118 	ldr	r3, [r1, #280]
    fef0:	e3130901 	tst	r3, #16384	; 0x4000
    fef4:	1afffffc 	bne	feec <sata_vs_reset+0x170>
    fef8:	e59f2164 	ldr	r2, [pc, #356]	; 10064 <sata_vs_reset+0x2e8>
    fefc:	e280eb01 	add	lr, r0, #1024	; 0x400
    ff00:	e592312c 	ldr	r3, [r2, #300]
    ff04:	e1a01002 	mov	r1, r2
    ff08:	e3c3300f 	bic	r3, r3, #15	; 0xf
    ff0c:	e582312c 	str	r3, [r2, #300]
    ff10:	e5820100 	str	r0, [r2, #256]
    ff14:	e582e108 	str	lr, [r2, #264]
    ff18:	e5923118 	ldr	r3, [r2, #280]
    ff1c:	e3833010 	orr	r3, r3, #16	; 0x10
    ff20:	e5823118 	str	r3, [r2, #280]
    ff24:	e5923118 	ldr	r3, [r2, #280]
    ff28:	e3833002 	orr	r3, r3, #2	; 0x2
    ff2c:	e5823118 	str	r3, [r2, #280]
    ff30:	e5913128 	ldr	r3, [r1, #296]
    ff34:	e59f2128 	ldr	r2, [pc, #296]	; 10064 <sata_vs_reset+0x2e8>
    ff38:	e203300f 	and	r3, r3, #15	; 0xf
    ff3c:	e3530003 	cmp	r3, #3	; 0x3
    ff40:	13530001 	cmpne	r3, #1	; 0x1
    ff44:	1afffff9 	bne	ff30 <sata_vs_reset+0x1b4>
    ff48:	e3e03000 	mvn	r3, #0	; 0x0
    ff4c:	e5823130 	str	r3, [r2, #304]
    ff50:	e5923120 	ldr	r3, [r2, #288]
    ff54:	e59f1108 	ldr	r1, [pc, #264]	; 10064 <sata_vs_reset+0x2e8>
    ff58:	e3130089 	tst	r3, #137	; 0x89
    ff5c:	1afffffb 	bne	ff50 <sata_vs_reset+0x1d4>
    ff60:	ea00000a 	b	ff90 <sata_vs_reset+0x214>
    ff64:	e59f4100 	ldr	r4, [pc, #256]	; 1006c <sata_vs_reset+0x2f0>
    ff68:	e59f0100 	ldr	r0, [pc, #256]	; 10070 <sata_vs_reset+0x2f4>
    ff6c:	e1a0e00f 	mov	lr, pc
    ff70:	e594f000 	ldr	pc, [r4]
    ff74:	e59f00f8 	ldr	r0, [pc, #248]	; 10074 <sata_vs_reset+0x2f8>
    ff78:	e1a0e00f 	mov	lr, pc
    ff7c:	e594f000 	ldr	pc, [r4]
    ff80:	e1a00005 	mov	r0, r5
    ff84:	ebffc63f 	bl	1888 <free>
    ff88:	e3e00000 	mvn	r0, #0	; 0x0
    ff8c:	e8bd8070 	pop	{r4, r5, r6, pc}
    ff90:	e5912118 	ldr	r2, [r1, #280]
    ff94:	e30c3011 	movw	r3, #49169	; 0xc011
    ff98:	e0023003 	and	r3, r2, r3
    ff9c:	e3530000 	cmp	r3, #0	; 0x0
    ffa0:	0a00000e 	beq	ffe0 <sata_vs_reset+0x264>
    ffa4:	e5913118 	ldr	r3, [r1, #280]
    ffa8:	e1a0c001 	mov	ip, r1
    ffac:	e3c33001 	bic	r3, r3, #1	; 0x1
    ffb0:	e5813118 	str	r3, [r1, #280]
    ffb4:	e59c3118 	ldr	r3, [ip, #280]
    ffb8:	e59f20a4 	ldr	r2, [pc, #164]	; 10064 <sata_vs_reset+0x2e8>
    ffbc:	e3130902 	tst	r3, #32768	; 0x8000
    ffc0:	1afffffb 	bne	ffb4 <sata_vs_reset+0x238>
    ffc4:	e5923118 	ldr	r3, [r2, #280]
    ffc8:	e1a01002 	mov	r1, r2
    ffcc:	e3c33010 	bic	r3, r3, #16	; 0x10
    ffd0:	e5823118 	str	r3, [r2, #280]
    ffd4:	e5913118 	ldr	r3, [r1, #280]
    ffd8:	e3130901 	tst	r3, #16384	; 0x4000
    ffdc:	1afffffc 	bne	ffd4 <sata_vs_reset+0x258>
    ffe0:	e59f207c 	ldr	r2, [pc, #124]	; 10064 <sata_vs_reset+0x2e8>
    ffe4:	e3e01000 	mvn	r1, #0	; 0x0
    ffe8:	e5820100 	str	r0, [r2, #256]
    ffec:	e582e108 	str	lr, [r2, #264]
    fff0:	e5923118 	ldr	r3, [r2, #280]
    fff4:	e3833010 	orr	r3, r3, #16	; 0x10
    fff8:	e5823118 	str	r3, [r2, #280]
    fffc:	e3e03502 	mvn	r3, #8388608	; 0x800000
   10000:	e5821130 	str	r1, [r2, #304]
   10004:	e5821110 	str	r1, [r2, #272]
   10008:	e5821008 	str	r1, [r2, #8]
   1000c:	e5823114 	str	r3, [r2, #276]
   10010:	e5923004 	ldr	r3, [r2, #4]
   10014:	e1a01002 	mov	r1, r2
   10018:	e3833002 	orr	r3, r3, #2	; 0x2
   1001c:	e5823004 	str	r3, [r2, #4]
   10020:	e5913128 	ldr	r3, [r1, #296]
   10024:	e5912120 	ldr	r2, [r1, #288]
   10028:	e203300f 	and	r3, r3, #15	; 0xf
   1002c:	e3530003 	cmp	r3, #3	; 0x3
   10030:	1afffffa 	bne	10020 <sata_vs_reset+0x2a4>
   10034:	e2124089 	ands	r4, r2, #137	; 0x89
   10038:	1afffff8 	bne	10020 <sata_vs_reset+0x2a4>
   1003c:	e1a00005 	mov	r0, r5
   10040:	ebffc610 	bl	1888 <free>
   10044:	e59f002c 	ldr	r0, [pc, #44]	; 10078 <sata_vs_reset+0x2fc>
   10048:	e59f301c 	ldr	r3, [pc, #28]	; 1006c <sata_vs_reset+0x2f0>
   1004c:	e1a0e00f 	mov	lr, pc
   10050:	e593f000 	ldr	pc, [r3]
   10054:	e1a00004 	mov	r0, r4
   10058:	e8bd8070 	pop	{r4, r5, r6, pc}
   1005c:	21e3c800 	.word	0x21e3c800
   10060:	21e3c000 	.word	0x21e3c000
   10064:	28200000 	.word	0x28200000
   10068:	00018961 	.word	0x00018961
   1006c:	3c00d6cc 	.word	0x3c00d6cc
   10070:	00018982 	.word	0x00018982
   10074:	000189a5 	.word	0x000189a5
   10078:	000189bb 	.word	0x000189bb

0001007c <init_led>:
   1007c:	e3a0c000 	mov	ip, #0	; 0x0
   10080:	e59f3010 	ldr	r3, [pc, #16]	; 10098 <init_led+0x1c>
   10084:	e59f0010 	ldr	r0, [pc, #16]	; 1009c <init_led+0x20>
   10088:	e1a0200c 	mov	r2, ip
   1008c:	e3a01002 	mov	r1, #2	; 0x2
   10090:	e583c000 	str	ip, [r3]
   10094:	eaffc2cf 	b	bd8 <pads_chmod>
   10098:	3c00d880 	.word	0x3c00d880
   1009c:	002e002d 	.word	0x002e002d

000100a0 <led_off>:
   100a0:	e3a02000 	mov	r2, #0	; 0x0
   100a4:	e59f300c 	ldr	r3, [pc, #12]	; 100b8 <led_off+0x18>
   100a8:	e1a01002 	mov	r1, r2
   100ac:	e3a0002e 	mov	r0, #46	; 0x2e
   100b0:	e5832000 	str	r2, [r3]
   100b4:	eaffc357 	b	e18 <pads_setpin>
   100b8:	3c00d880 	.word	0x3c00d880

000100bc <led_on>:
   100bc:	e3a02001 	mov	r2, #1	; 0x1
   100c0:	e59f300c 	ldr	r3, [pc, #12]	; 100d4 <led_on+0x18>
   100c4:	e1a01002 	mov	r1, r2
   100c8:	e3a0002e 	mov	r0, #46	; 0x2e
   100cc:	e5832000 	str	r2, [r3]
   100d0:	eaffc350 	b	e18 <pads_setpin>
   100d4:	3c00d880 	.word	0x3c00d880

000100d8 <led_pulse>:
   100d8:	e92d4010 	push	{r4, lr}
   100dc:	e1a04000 	mov	r4, r0
   100e0:	ebfffff5 	bl	100bc <led_on>
   100e4:	e3a00005 	mov	r0, #5	; 0x5
   100e8:	ea000001 	b	100f4 <led_pulse+0x1c>
   100ec:	ebfffff2 	bl	100bc <led_on>
   100f0:	e3a00002 	mov	r0, #2	; 0x2
   100f4:	ebfff638 	bl	d9dc <udelay>
   100f8:	ebffffe8 	bl	100a0 <led_off>
   100fc:	e3a00002 	mov	r0, #2	; 0x2
   10100:	ebfff635 	bl	d9dc <udelay>
   10104:	e3540000 	cmp	r4, #0	; 0x0
   10108:	e2444001 	sub	r4, r4, #1	; 0x1
   1010c:	cafffff6 	bgt	100ec <led_pulse+0x14>
   10110:	e8bd8010 	pop	{r4, pc}

00010114 <led_ctrl>:
   10114:	e3500000 	cmp	r0, #0	; 0x0
   10118:	0a000000 	beq	10120 <led_ctrl+0xc>
   1011c:	eaffffe6 	b	100bc <led_on>
   10120:	eaffffde 	b	100a0 <led_off>

00010124 <led_toggle>:
   10124:	e59f300c 	ldr	r3, [pc, #12]	; 10138 <led_toggle+0x14>
   10128:	e5930000 	ldr	r0, [r3]
   1012c:	e2700001 	rsbs	r0, r0, #1	; 0x1
   10130:	33a00000 	movcc	r0, #0	; 0x0
   10134:	eafffff6 	b	10114 <led_ctrl>
   10138:	3c00d880 	.word	0x3c00d880

0001013c <udc_get_cable_state>:
   1013c:	e3a00001 	mov	r0, #1	; 0x1
   10140:	e12fff1e 	bx	lr

00010144 <udc_disconnect>:
   10144:	e59f103c 	ldr	r1, [pc, #60]	; 10188 <udc_disconnect+0x44>
   10148:	e59f303c 	ldr	r3, [pc, #60]	; 1018c <udc_disconnect+0x48>
   1014c:	e5912010 	ldr	r2, [r1, #16]
   10150:	e3a00000 	mov	r0, #0	; 0x0
   10154:	e3822001 	orr	r2, r2, #1	; 0x1
   10158:	e92d4010 	push	{r4, lr}
   1015c:	e5c30000 	strb	r0, [r3]
   10160:	e1a04001 	mov	r4, r1
   10164:	e5812010 	str	r2, [r1, #16]
   10168:	ea000000 	b	10170 <udc_disconnect+0x2c>
   1016c:	ebfff61a 	bl	d9dc <udelay>
   10170:	e5943010 	ldr	r3, [r4, #16]
   10174:	e3a0000a 	mov	r0, #10	; 0xa
   10178:	e2133001 	ands	r3, r3, #1	; 0x1
   1017c:	1afffffa 	bne	1016c <udc_disconnect+0x28>
   10180:	e1a00003 	mov	r0, r3
   10184:	e8bd8010 	pop	{r4, pc}
   10188:	27100000 	.word	0x27100000
   1018c:	3c00d224 	.word	0x3c00d224

00010190 <udc_event_handle>:
   10190:	e59f2768 	ldr	r2, [pc, #1896]	; 10900 <udc_event_handle+0x770>
   10194:	e59f3768 	ldr	r3, [pc, #1896]	; 10904 <udc_event_handle+0x774>
   10198:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
   1019c:	e5934014 	ldr	r4, [r3, #20]
   101a0:	e5923000 	ldr	r3, [r2]
   101a4:	e59f0758 	ldr	r0, [pc, #1880]	; 10904 <udc_event_handle+0x774>
   101a8:	e1530004 	cmp	r3, r4
   101ac:	e204ca03 	and	ip, r4, #12288	; 0x3000
   101b0:	15824000 	strne	r4, [r2]
   101b4:	e2043b03 	and	r3, r4, #3072	; 0xc00
   101b8:	e35c0a01 	cmp	ip, #4096	; 0x1000
   101bc:	e24dd008 	sub	sp, sp, #8	; 0x8
   101c0:	e5803014 	str	r3, [r0, #20]
   101c4:	0a000002 	beq	101d4 <udc_event_handle+0x44>
   101c8:	e35c0a02 	cmp	ip, #8192	; 0x2000
   101cc:	1a000034 	bne	102a4 <udc_event_handle+0x114>
   101d0:	ea000025 	b	1026c <udc_event_handle+0xdc>
   101d4:	e3a03302 	mov	r3, #134217728	; 0x8000000
   101d8:	e5803b00 	str	r3, [r0, #2816]
   101dc:	e5803b40 	str	r3, [r0, #2880]
   101e0:	e59f3720 	ldr	r3, [pc, #1824]	; 10908 <udc_event_handle+0x778>
   101e4:	e3a02009 	mov	r2, #9	; 0x9
   101e8:	e580381c 	str	r3, [r0, #2076]
   101ec:	e3a03b02 	mov	r3, #2048	; 0x800
   101f0:	e5802814 	str	r2, [r0, #2068]
   101f4:	e5802810 	str	r2, [r0, #2064]
   101f8:	e5803024 	str	r3, [r0, #36]
   101fc:	e2833301 	add	r3, r3, #67108864	; 0x4000000
   10200:	e5803028 	str	r3, [r0, #40]
   10204:	e2833b01 	add	r3, r3, #1024	; 0x400
   10208:	e5803104 	str	r3, [r0, #260]
   1020c:	e59f36f8 	ldr	r3, [pc, #1784]	; 1090c <udc_event_handle+0x77c>
   10210:	e1a05000 	mov	r5, r0
   10214:	e580305c 	str	r3, [r0, #92]
   10218:	e5903010 	ldr	r3, [r0, #16]
   1021c:	e3c33d1f 	bic	r3, r3, #1984	; 0x7c0
   10220:	e3833e43 	orr	r3, r3, #1072	; 0x430
   10224:	e5803010 	str	r3, [r0, #16]
   10228:	ea000000 	b	10230 <udc_event_handle+0xa0>
   1022c:	ebfff5ea 	bl	d9dc <udelay>
   10230:	e5953010 	ldr	r3, [r5, #16]
   10234:	e3a00001 	mov	r0, #1	; 0x1
   10238:	e3130030 	tst	r3, #48	; 0x30
   1023c:	e59f16c0 	ldr	r1, [pc, #1728]	; 10904 <udc_event_handle+0x774>
   10240:	1afffff9 	bne	1022c <udc_event_handle+0x9c>
   10244:	e59f36c4 	ldr	r3, [pc, #1732]	; 10910 <udc_event_handle+0x780>
   10248:	e2800009 	add	r0, r0, #9	; 0x9
   1024c:	e5932024 	ldr	r2, [r3, #36]
   10250:	e59f36bc 	ldr	r3, [pc, #1724]	; 10914 <udc_event_handle+0x784>
   10254:	e5813b10 	str	r3, [r1, #2832]
   10258:	e3a03a01 	mov	r3, #4096	; 0x1000
   1025c:	e5812b14 	str	r2, [r1, #2836]
   10260:	e5813014 	str	r3, [r1, #20]
   10264:	ebfff5dc 	bl	d9dc <udelay>
   10268:	ea00000d 	b	102a4 <udc_event_handle+0x114>
   1026c:	e5902808 	ldr	r2, [r0, #2056]
   10270:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   10274:	e5803b00 	str	r3, [r0, #2816]
   10278:	e1a020a2 	lsr	r2, r2, #1
   1027c:	e59f368c 	ldr	r3, [pc, #1676]	; 10910 <udc_event_handle+0x780>
   10280:	e5901018 	ldr	r1, [r0, #24]
   10284:	e2022003 	and	r2, r2, #3	; 0x3
   10288:	e5c3201c 	strb	r2, [r3, #28]
   1028c:	e59f3684 	ldr	r3, [pc, #1668]	; 10918 <udc_event_handle+0x788>
   10290:	e3811008 	orr	r1, r1, #8	; 0x8
   10294:	e3a02001 	mov	r2, #1	; 0x1
   10298:	e5801018 	str	r1, [r0, #24]
   1029c:	e5832000 	str	r2, [r3]
   102a0:	e580c014 	str	ip, [r0, #20]
   102a4:	e59f3658 	ldr	r3, [pc, #1624]	; 10904 <udc_event_handle+0x774>
   102a8:	e3140701 	tst	r4, #262144	; 0x40000
   102ac:	e5939818 	ldr	r9, [r3, #2072]
   102b0:	0a000051 	beq	103fc <udc_event_handle+0x26c>
   102b4:	e3190001 	tst	r9, #1	; 0x1
   102b8:	0a000042 	beq	103c8 <udc_event_handle+0x238>
   102bc:	e5935908 	ldr	r5, [r3, #2312]
   102c0:	e3150001 	tst	r5, #1	; 0x1
   102c4:	0a00003b 	beq	103b8 <udc_event_handle+0x228>
   102c8:	e59f3640 	ldr	r3, [pc, #1600]	; 10910 <udc_event_handle+0x780>
   102cc:	e5933020 	ldr	r3, [r3, #32]
   102d0:	e2433002 	sub	r3, r3, #2	; 0x2
   102d4:	e3530003 	cmp	r3, #3	; 0x3
   102d8:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   102dc:	ea000035 	b	103b8 <udc_event_handle+0x228>
   102e0:	00010320 	.word	0x00010320
   102e4:	000102f0 	.word	0x000102f0
   102e8:	00010354 	.word	0x00010354
   102ec:	00010354 	.word	0x00010354
   102f0:	e59f160c 	ldr	r1, [pc, #1548]	; 10904 <udc_event_handle+0x774>
   102f4:	e59f0614 	ldr	r0, [pc, #1556]	; 10910 <udc_event_handle+0x780>
   102f8:	e5912910 	ldr	r2, [r1, #2320]
   102fc:	e590c00c 	ldr	ip, [r0, #12]
   10300:	e3a03004 	mov	r3, #4	; 0x4
   10304:	e3822702 	orr	r2, r2, #524288	; 0x80000
   10308:	e5803020 	str	r3, [r0, #32]
   1030c:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   10310:	e5812910 	str	r2, [r1, #2320]
   10314:	e581c914 	str	ip, [r1, #2324]
   10318:	e5813900 	str	r3, [r1, #2304]
   1031c:	ea000025 	b	103b8 <udc_event_handle+0x228>
   10320:	e59f15dc 	ldr	r1, [pc, #1500]	; 10904 <udc_event_handle+0x774>
   10324:	e59f05e4 	ldr	r0, [pc, #1508]	; 10910 <udc_event_handle+0x780>
   10328:	e5913b10 	ldr	r3, [r1, #2832]
   1032c:	e590c024 	ldr	ip, [r0, #36]
   10330:	e3c3307f 	bic	r3, r3, #127	; 0x7f
   10334:	e3833702 	orr	r3, r3, #524288	; 0x80000
   10338:	e5813b10 	str	r3, [r1, #2832]
   1033c:	e3a02005 	mov	r2, #5	; 0x5
   10340:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   10344:	e581cb14 	str	ip, [r1, #2836]
   10348:	e5802020 	str	r2, [r0, #32]
   1034c:	e5813b00 	str	r3, [r1, #2816]
   10350:	ea000018 	b	103b8 <udc_event_handle+0x228>
   10354:	e59f05b4 	ldr	r0, [pc, #1460]	; 10910 <udc_event_handle+0x780>
   10358:	e5d03015 	ldrb	r3, [r0, #21]
   1035c:	e3530009 	cmp	r3, #9	; 0x9
   10360:	1a00000a 	bne	10390 <udc_event_handle+0x200>
   10364:	e59f1598 	ldr	r1, [pc, #1432]	; 10904 <udc_event_handle+0x774>
   10368:	e59f35ac 	ldr	r3, [pc, #1452]	; 1091c <udc_event_handle+0x78c>
   1036c:	e3a02001 	mov	r2, #1	; 0x1
   10370:	e5813920 	str	r3, [r1, #2336]
   10374:	e2433d07 	sub	r3, r3, #448	; 0x1c0
   10378:	e5813b40 	str	r3, [r1, #2880]
   1037c:	e591381c 	ldr	r3, [r1, #2076]
   10380:	e5c02000 	strb	r2, [r0]
   10384:	e3833701 	orr	r3, r3, #262144	; 0x40000
   10388:	e3833002 	orr	r3, r3, #2	; 0x2
   1038c:	e581381c 	str	r3, [r1, #2076]
   10390:	e59f256c 	ldr	r2, [pc, #1388]	; 10904 <udc_event_handle+0x774>
   10394:	e59f1574 	ldr	r1, [pc, #1396]	; 10910 <udc_event_handle+0x780>
   10398:	e59f3574 	ldr	r3, [pc, #1396]	; 10914 <udc_event_handle+0x784>
   1039c:	e5910024 	ldr	r0, [r1, #36]
   103a0:	e5823b10 	str	r3, [r2, #2832]
   103a4:	e3a03001 	mov	r3, #1	; 0x1
   103a8:	e5813020 	str	r3, [r1, #32]
   103ac:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   103b0:	e5820b14 	str	r0, [r2, #2836]
   103b4:	e5823b00 	str	r3, [r2, #2816]
   103b8:	e300320d 	movw	r3, #525	; 0x20d
   103bc:	e59f2540 	ldr	r2, [pc, #1344]	; 10904 <udc_event_handle+0x774>
   103c0:	e0053003 	and	r3, r5, r3
   103c4:	e5823908 	str	r3, [r2, #2312]
   103c8:	e3190002 	tst	r9, #2	; 0x2
   103cc:	0a00000a 	beq	103fc <udc_event_handle+0x26c>
   103d0:	e59f352c 	ldr	r3, [pc, #1324]	; 10904 <udc_event_handle+0x774>
   103d4:	e5932928 	ldr	r2, [r3, #2344]
   103d8:	e3120001 	tst	r2, #1	; 0x1
   103dc:	0a000003 	beq	103f0 <udc_event_handle+0x260>
   103e0:	e5933930 	ldr	r3, [r3, #2352]
   103e4:	e3d3120f 	bics	r1, r3, #-268435456	; 0xf0000000
   103e8:	059f3520 	ldreq	r3, [pc, #1312]	; 10910 <udc_event_handle+0x780>
   103ec:	05c31002 	strbeq	r1, [r3, #2]
   103f0:	e59f350c 	ldr	r3, [pc, #1292]	; 10904 <udc_event_handle+0x774>
   103f4:	e2022005 	and	r2, r2, #5	; 0x5
   103f8:	e5832928 	str	r2, [r3, #2344]
   103fc:	e3140702 	tst	r4, #524288	; 0x80000
   10400:	0a00013c 	beq	108f8 <udc_event_handle+0x768>
   10404:	e3190801 	tst	r9, #65536	; 0x10000
   10408:	0a000124 	beq	108a0 <udc_event_handle+0x710>
   1040c:	e59f34f0 	ldr	r3, [pc, #1264]	; 10904 <udc_event_handle+0x774>
   10410:	e593ab08 	ldr	sl, [r3, #2824]
   10414:	e31a0001 	tst	sl, #1	; 0x1
   10418:	0a00002b 	beq	104cc <udc_event_handle+0x33c>
   1041c:	e59f34ec 	ldr	r3, [pc, #1260]	; 10910 <udc_event_handle+0x780>
   10420:	e5933020 	ldr	r3, [r3, #32]
   10424:	e2433002 	sub	r3, r3, #2	; 0x2
   10428:	e3530003 	cmp	r3, #3	; 0x3
   1042c:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   10430:	ea000025 	b	104cc <udc_event_handle+0x33c>
   10434:	00010470 	.word	0x00010470
   10438:	00010444 	.word	0x00010444
   1043c:	000104a4 	.word	0x000104a4
   10440:	000104a4 	.word	0x000104a4
   10444:	e59f14c4 	ldr	r1, [pc, #1220]	; 10910 <udc_event_handle+0x780>
   10448:	e59f24b4 	ldr	r2, [pc, #1204]	; 10904 <udc_event_handle+0x774>
   1044c:	e3a03004 	mov	r3, #4	; 0x4
   10450:	e591000c 	ldr	r0, [r1, #12]
   10454:	e5813020 	str	r3, [r1, #32]
   10458:	e3a03702 	mov	r3, #524288	; 0x80000
   1045c:	e5823910 	str	r3, [r2, #2320]
   10460:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   10464:	e5820914 	str	r0, [r2, #2324]
   10468:	e5823900 	str	r3, [r2, #2304]
   1046c:	ea000016 	b	104cc <udc_event_handle+0x33c>
   10470:	e59f148c 	ldr	r1, [pc, #1164]	; 10904 <udc_event_handle+0x774>
   10474:	e59f0494 	ldr	r0, [pc, #1172]	; 10910 <udc_event_handle+0x780>
   10478:	e5913b10 	ldr	r3, [r1, #2832]
   1047c:	e590c00c 	ldr	ip, [r0, #12]
   10480:	e3c3307f 	bic	r3, r3, #127	; 0x7f
   10484:	e3833702 	orr	r3, r3, #524288	; 0x80000
   10488:	e5813b10 	str	r3, [r1, #2832]
   1048c:	e3a02005 	mov	r2, #5	; 0x5
   10490:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   10494:	e581cb14 	str	ip, [r1, #2836]
   10498:	e5802020 	str	r2, [r0, #32]
   1049c:	e5813b00 	str	r3, [r1, #2816]
   104a0:	ea000009 	b	104cc <udc_event_handle+0x33c>
   104a4:	e59f2458 	ldr	r2, [pc, #1112]	; 10904 <udc_event_handle+0x774>
   104a8:	e59f1460 	ldr	r1, [pc, #1120]	; 10910 <udc_event_handle+0x780>
   104ac:	e59f3460 	ldr	r3, [pc, #1120]	; 10914 <udc_event_handle+0x784>
   104b0:	e5910024 	ldr	r0, [r1, #36]
   104b4:	e5823b10 	str	r3, [r2, #2832]
   104b8:	e3a03001 	mov	r3, #1	; 0x1
   104bc:	e5813020 	str	r3, [r1, #32]
   104c0:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   104c4:	e5820b14 	str	r0, [r2, #2836]
   104c8:	e5823b00 	str	r3, [r2, #2816]
   104cc:	e31a0004 	tst	sl, #4	; 0x4
   104d0:	159f342c 	ldrne	r3, [pc, #1068]	; 10904 <udc_event_handle+0x774>
   104d4:	13a02004 	movne	r2, #4	; 0x4
   104d8:	15832b08 	strne	r2, [r3, #2824]
   104dc:	e31a0008 	tst	sl, #8	; 0x8
   104e0:	0a0000ea 	beq	10890 <udc_event_handle+0x700>
   104e4:	e31a0040 	tst	sl, #64	; 0x40
   104e8:	e59f1430 	ldr	r1, [pc, #1072]	; 10920 <udc_event_handle+0x790>
   104ec:	0a000006 	beq	1050c <udc_event_handle+0x37c>
   104f0:	e59f340c 	ldr	r3, [pc, #1036]	; 10904 <udc_event_handle+0x774>
   104f4:	e5933b14 	ldr	r3, [r3, #2836]
   104f8:	e2432004 	sub	r2, r3, #4	; 0x4
   104fc:	e2433008 	sub	r3, r3, #8	; 0x8
   10500:	e5812004 	str	r2, [r1, #4]
   10504:	e5813000 	str	r3, [r1]
   10508:	ea000005 	b	10524 <udc_event_handle+0x394>
   1050c:	e59f33fc 	ldr	r3, [pc, #1020]	; 10910 <udc_event_handle+0x780>
   10510:	e5932024 	ldr	r2, [r3, #36]
   10514:	e5923000 	ldr	r3, [r2]
   10518:	e5813000 	str	r3, [r1]
   1051c:	e5923004 	ldr	r3, [r2, #4]
   10520:	e5813004 	str	r3, [r1, #4]
   10524:	e59f23e4 	ldr	r2, [pc, #996]	; 10910 <udc_event_handle+0x780>
   10528:	e5d20014 	ldrb	r0, [r2, #20]
   1052c:	e3100080 	tst	r0, #128	; 0x80
   10530:	13a03002 	movne	r3, #2	; 0x2
   10534:	03a03003 	moveq	r3, #3	; 0x3
   10538:	e5823020 	str	r3, [r2, #32]
   1053c:	e59f23cc 	ldr	r2, [pc, #972]	; 10910 <udc_event_handle+0x780>
   10540:	e1d211ba 	ldrh	r1, [r2, #26]
   10544:	e3510000 	cmp	r1, #0	; 0x0
   10548:	03a03004 	moveq	r3, #4	; 0x4
   1054c:	05823020 	streq	r3, [r2, #32]
   10550:	e59f33b8 	ldr	r3, [pc, #952]	; 10910 <udc_event_handle+0x780>
   10554:	e5d33015 	ldrb	r3, [r3, #21]
   10558:	e3530009 	cmp	r3, #9	; 0x9
   1055c:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   10560:	ea0000ca 	b	10890 <udc_event_handle+0x700>
   10564:	00010828 	.word	0x00010828
   10568:	00010890 	.word	0x00010890
   1056c:	00010890 	.word	0x00010890
   10570:	00010890 	.word	0x00010890
   10574:	00010890 	.word	0x00010890
   10578:	00010774 	.word	0x00010774
   1057c:	0001058c 	.word	0x0001058c
   10580:	00010890 	.word	0x00010890
   10584:	00010890 	.word	0x00010890
   10588:	000107c4 	.word	0x000107c4
   1058c:	e3a00040 	mov	r0, #64	; 0x40
   10590:	ebffc454 	bl	16e8 <malloc>
   10594:	e2506000 	subs	r6, r0, #0	; 0x0
   10598:	0a0000bc 	beq	10890 <udc_event_handle+0x700>
   1059c:	e59f536c 	ldr	r5, [pc, #876]	; 10910 <udc_event_handle+0x780>
   105a0:	e1d521b6 	ldrh	r2, [r5, #22]
   105a4:	e1a03422 	lsr	r3, r2, #8
   105a8:	e2433001 	sub	r3, r3, #1	; 0x1
   105ac:	e3530005 	cmp	r3, #5	; 0x5
   105b0:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   105b4:	ea000054 	b	1070c <udc_event_handle+0x57c>
   105b8:	000105d0 	.word	0x000105d0
   105bc:	000105e8 	.word	0x000105e8
   105c0:	00010640 	.word	0x00010640
   105c4:	0001070c 	.word	0x0001070c
   105c8:	0001070c 	.word	0x0001070c
   105cc:	00010614 	.word	0x00010614
   105d0:	e1a00006 	mov	r0, r6
   105d4:	e59f1348 	ldr	r1, [pc, #840]	; 10924 <udc_event_handle+0x794>
   105d8:	e3a02012 	mov	r2, #18	; 0x12
   105dc:	ebffc3dc 	bl	1554 <memcpy>
   105e0:	e3a03012 	mov	r3, #18	; 0x12
   105e4:	ea000028 	b	1068c <udc_event_handle+0x4fc>
   105e8:	e59f3338 	ldr	r3, [pc, #824]	; 10928 <udc_event_handle+0x798>
   105ec:	e3a04020 	mov	r4, #32	; 0x20
   105f0:	e3a0c000 	mov	ip, #0	; 0x0
   105f4:	e1a01003 	mov	r1, r3
   105f8:	e1a00006 	mov	r0, r6
   105fc:	e1a02004 	mov	r2, r4
   10600:	e5c3c003 	strb	ip, [r3, #3]
   10604:	e5c34002 	strb	r4, [r3, #2]
   10608:	ebffc3d1 	bl	1554 <memcpy>
   1060c:	e5854004 	str	r4, [r5, #4]
   10610:	ea000045 	b	1072c <udc_event_handle+0x59c>
   10614:	e1d531ba 	ldrh	r3, [r5, #26]
   10618:	e59fc30c 	ldr	ip, [pc, #780]	; 1092c <udc_event_handle+0x79c>
   1061c:	e353000a 	cmp	r3, #10	; 0xa
   10620:	23a0300a 	movcs	r3, #10	; 0xa
   10624:	e5cc3000 	strb	r3, [ip]
   10628:	e1a0100c 	mov	r1, ip
   1062c:	e1a00006 	mov	r0, r6
   10630:	e3a0200a 	mov	r2, #10	; 0xa
   10634:	ebffc3c6 	bl	1554 <memcpy>
   10638:	e3a0300a 	mov	r3, #10	; 0xa
   1063c:	ea000012 	b	1068c <udc_event_handle+0x4fc>
   10640:	e21270ff 	ands	r7, r2, #255	; 0xff
   10644:	13a03000 	movne	r3, #0	; 0x0
   10648:	03a03001 	moveq	r3, #1	; 0x1
   1064c:	e3570005 	cmp	r7, #5	; 0x5
   10650:	d1a08003 	movle	r8, r3
   10654:	c3838001 	orrgt	r8, r3, #1	; 0x1
   10658:	e3580000 	cmp	r8, #0	; 0x0
   1065c:	e59f52ac 	ldr	r5, [pc, #684]	; 10910 <udc_event_handle+0x780>
   10660:	0a00000b 	beq	10694 <udc_event_handle+0x504>
   10664:	e1d531ba 	ldrh	r3, [r5, #26]
   10668:	e59fc2c0 	ldr	ip, [pc, #704]	; 10930 <udc_event_handle+0x7a0>
   1066c:	e3530004 	cmp	r3, #4	; 0x4
   10670:	23a03004 	movcs	r3, #4	; 0x4
   10674:	e5cc3000 	strb	r3, [ip]
   10678:	e1a0100c 	mov	r1, ip
   1067c:	e1a00006 	mov	r0, r6
   10680:	e3a02004 	mov	r2, #4	; 0x4
   10684:	ebffc3b2 	bl	1554 <memcpy>
   10688:	e3a03004 	mov	r3, #4	; 0x4
   1068c:	e5853004 	str	r3, [r5, #4]
   10690:	ea000025 	b	1072c <udc_event_handle+0x59c>
   10694:	e59f4298 	ldr	r4, [pc, #664]	; 10934 <udc_event_handle+0x7a4>
   10698:	e7940107 	ldr	r0, [r4, r7, lsl #2]
   1069c:	ebffc392 	bl	14ec <strlen>
   106a0:	e6ef0070 	uxtb	r0, r0
   106a4:	e2803001 	add	r3, r0, #1	; 0x1
   106a8:	e5d5201a 	ldrb	r2, [r5, #26]
   106ac:	e1a03083 	lsl	r3, r3, #1
   106b0:	e6ef3073 	uxtb	r3, r3
   106b4:	e1530002 	cmp	r3, r2
   106b8:	31a0e003 	movcc	lr, r3
   106bc:	21a0e002 	movcs	lr, r2
   106c0:	e3a03003 	mov	r3, #3	; 0x3
   106c4:	e5c63001 	strb	r3, [r6, #1]
   106c8:	e5c6e000 	strb	lr, [r6]
   106cc:	e1a05008 	mov	r5, r8
   106d0:	e3a02001 	mov	r2, #1	; 0x1
   106d4:	ea000005 	b	106f0 <udc_event_handle+0x560>
   106d8:	e7943107 	ldr	r3, [r4, r7, lsl #2]
   106dc:	e0833002 	add	r3, r3, r2
   106e0:	e5533001 	ldrb	r3, [r3, #-1]
   106e4:	e7c63082 	strb	r3, [r6, r2, lsl #1]
   106e8:	e5c15001 	strb	r5, [r1, #1]
   106ec:	e6ef207c 	uxtb	r2, ip
   106f0:	e1500002 	cmp	r0, r2
   106f4:	e282c001 	add	ip, r2, #1	; 0x1
   106f8:	e0861082 	add	r1, r6, r2, lsl #1
   106fc:	aafffff5 	bge	106d8 <udc_event_handle+0x548>
   10700:	e59f3208 	ldr	r3, [pc, #520]	; 10910 <udc_event_handle+0x780>
   10704:	e583e004 	str	lr, [r3, #4]
   10708:	ea000007 	b	1072c <udc_event_handle+0x59c>
   1070c:	e59f21f0 	ldr	r2, [pc, #496]	; 10904 <udc_event_handle+0x774>
   10710:	e3a03702 	mov	r3, #524288	; 0x80000
   10714:	e5951024 	ldr	r1, [r5, #36]
   10718:	e5823910 	str	r3, [r2, #2320]
   1071c:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   10720:	e5821914 	str	r1, [r2, #2324]
   10724:	e5823900 	str	r3, [r2, #2304]
   10728:	ea00000e 	b	10768 <udc_event_handle+0x5d8>
   1072c:	e59f11dc 	ldr	r1, [pc, #476]	; 10910 <udc_event_handle+0x780>
   10730:	e59f01cc 	ldr	r0, [pc, #460]	; 10904 <udc_event_handle+0x774>
   10734:	e1d120b4 	ldrh	r2, [r1, #4]
   10738:	e1d131ba 	ldrh	r3, [r1, #26]
   1073c:	e1520003 	cmp	r2, r3
   10740:	21a02003 	movcs	r2, r3
   10744:	e3823702 	orr	r3, r2, #524288	; 0x80000
   10748:	e5803910 	str	r3, [r0, #2320]
   1074c:	e3a03000 	mov	r3, #0	; 0x0
   10750:	e5c13001 	strb	r3, [r1, #1]
   10754:	e2833321 	add	r3, r3, #-2080374784	; 0x84000000
   10758:	e5812004 	str	r2, [r1, #4]
   1075c:	e5806914 	str	r6, [r0, #2324]
   10760:	e5803900 	str	r3, [r0, #2304]
   10764:	e581600c 	str	r6, [r1, #12]
   10768:	e1a00006 	mov	r0, r6
   1076c:	ebffc445 	bl	1888 <free>
   10770:	ea000046 	b	10890 <udc_event_handle+0x700>
   10774:	e59fc1bc 	ldr	ip, [pc, #444]	; 10938 <udc_event_handle+0x7a8>
   10778:	e5dc3000 	ldrb	r3, [ip]
   1077c:	e35300ff 	cmp	r3, #255	; 0xff
   10780:	1a000042 	bne	10890 <udc_event_handle+0x700>
   10784:	e59f1178 	ldr	r1, [pc, #376]	; 10904 <udc_event_handle+0x774>
   10788:	e59f0180 	ldr	r0, [pc, #384]	; 10910 <udc_event_handle+0x780>
   1078c:	e5913800 	ldr	r3, [r1, #2048]
   10790:	e1d021b6 	ldrh	r2, [r0, #22]
   10794:	e3c33e7f 	bic	r3, r3, #2032	; 0x7f0
   10798:	e202207f 	and	r2, r2, #127	; 0x7f
   1079c:	e1833202 	orr	r3, r3, r2, lsl #4
   107a0:	e5813800 	str	r3, [r1, #2048]
   107a4:	e5900024 	ldr	r0, [r0, #36]
   107a8:	e3a03702 	mov	r3, #524288	; 0x80000
   107ac:	e5cc2000 	strb	r2, [ip]
   107b0:	e5813910 	str	r3, [r1, #2320]
   107b4:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   107b8:	e5810914 	str	r0, [r1, #2324]
   107bc:	e5813900 	str	r3, [r1, #2304]
   107c0:	ea000032 	b	10890 <udc_event_handle+0x700>
   107c4:	e59f016c 	ldr	r0, [pc, #364]	; 10938 <udc_event_handle+0x7a8>
   107c8:	e5d03000 	ldrb	r3, [r0]
   107cc:	e35300ff 	cmp	r3, #255	; 0xff
   107d0:	0a00002e 	beq	10890 <udc_event_handle+0x700>
   107d4:	e59f1134 	ldr	r1, [pc, #308]	; 10910 <udc_event_handle+0x780>
   107d8:	e1d131b6 	ldrh	r3, [r1, #22]
   107dc:	e3530000 	cmp	r3, #0	; 0x0
   107e0:	03e03000 	mvneq	r3, #0	; 0x0
   107e4:	05911024 	ldreq	r1, [r1, #36]
   107e8:	059f2114 	ldreq	r2, [pc, #276]	; 10904 <udc_event_handle+0x774>
   107ec:	05c03000 	strbeq	r3, [r0]
   107f0:	0a000013 	beq	10844 <udc_event_handle+0x6b4>
   107f4:	e3530001 	cmp	r3, #1	; 0x1
   107f8:	1a000024 	bne	10890 <udc_event_handle+0x700>
   107fc:	e59f2100 	ldr	r2, [pc, #256]	; 10904 <udc_event_handle+0x774>
   10800:	e59f3134 	ldr	r3, [pc, #308]	; 1093c <udc_event_handle+0x7ac>
   10804:	e5911024 	ldr	r1, [r1, #36]
   10808:	e5823920 	str	r3, [r2, #2336]
   1080c:	e2833d07 	add	r3, r3, #448	; 0x1c0
   10810:	e5823b40 	str	r3, [r2, #2880]
   10814:	e592381c 	ldr	r3, [r2, #2076]
   10818:	e3833701 	orr	r3, r3, #262144	; 0x40000
   1081c:	e3833002 	orr	r3, r3, #2	; 0x2
   10820:	e582381c 	str	r3, [r2, #2076]
   10824:	ea000006 	b	10844 <udc_event_handle+0x6b4>
   10828:	e210c01f 	ands	ip, r0, #31	; 0x1f
   1082c:	1a000017 	bne	10890 <udc_event_handle+0x700>
   10830:	e3510000 	cmp	r1, #0	; 0x0
   10834:	e59f50d4 	ldr	r5, [pc, #212]	; 10910 <udc_event_handle+0x780>
   10838:	1a000004 	bne	10850 <udc_event_handle+0x6c0>
   1083c:	e5951024 	ldr	r1, [r5, #36]
   10840:	e59f20bc 	ldr	r2, [pc, #188]	; 10904 <udc_event_handle+0x774>
   10844:	e3a03702 	mov	r3, #524288	; 0x80000
   10848:	e5823910 	str	r3, [r2, #2320]
   1084c:	ea00000c 	b	10884 <udc_event_handle+0x6f4>
   10850:	e1d531b6 	ldrh	r3, [r5, #22]
   10854:	e59f20a8 	ldr	r2, [pc, #168]	; 10904 <udc_event_handle+0x774>
   10858:	e3530002 	cmp	r3, #2	; 0x2
   1085c:	23a03002 	movcs	r3, #2	; 0x2
   10860:	e6ef3073 	uxtb	r3, r3
   10864:	e1530001 	cmp	r3, r1
   10868:	21a03001 	movcs	r3, r1
   1086c:	e3830702 	orr	r0, r3, #524288	; 0x80000
   10870:	e28d1006 	add	r1, sp, #6	; 0x6
   10874:	e5c5c001 	strb	ip, [r5, #1]
   10878:	e5853004 	str	r3, [r5, #4]
   1087c:	e585100c 	str	r1, [r5, #12]
   10880:	e5820910 	str	r0, [r2, #2320]
   10884:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   10888:	e5821914 	str	r1, [r2, #2324]
   1088c:	e5823900 	str	r3, [r2, #2304]
   10890:	e3003209 	movw	r3, #521	; 0x209
   10894:	e59f2068 	ldr	r2, [pc, #104]	; 10904 <udc_event_handle+0x774>
   10898:	e00a3003 	and	r3, sl, r3
   1089c:	e5823b08 	str	r3, [r2, #2824]
   108a0:	e3190701 	tst	r9, #262144	; 0x40000
   108a4:	0a000013 	beq	108f8 <udc_event_handle+0x768>
   108a8:	e59f1054 	ldr	r1, [pc, #84]	; 10904 <udc_event_handle+0x774>
   108ac:	e5912b48 	ldr	r2, [r1, #2888]
   108b0:	e3120001 	tst	r2, #1	; 0x1
   108b4:	0a00000c 	beq	108ec <udc_event_handle+0x75c>
   108b8:	e5913b50 	ldr	r3, [r1, #2896]
   108bc:	e59f504c 	ldr	r5, [pc, #76]	; 10910 <udc_event_handle+0x780>
   108c0:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
   108c4:	e3c3373e 	bic	r3, r3, #16252928	; 0xf80000
   108c8:	e3530c02 	cmp	r3, #512	; 0x200
   108cc:	05953028 	ldreq	r3, [r5, #40]
   108d0:	13a03000 	movne	r3, #0	; 0x0
   108d4:	03833702 	orreq	r3, r3, #524288	; 0x80000
   108d8:	05813b50 	streq	r3, [r1, #2896]
   108dc:	05913b40 	ldreq	r3, [r1, #2880]
   108e0:	15c53003 	strbne	r3, [r5, #3]
   108e4:	03833321 	orreq	r3, r3, #-2080374784	; 0x84000000
   108e8:	05813b40 	streq	r3, [r1, #2880]
   108ec:	e59f3010 	ldr	r3, [pc, #16]	; 10904 <udc_event_handle+0x774>
   108f0:	e2022005 	and	r2, r2, #5	; 0x5
   108f4:	e5832b48 	str	r2, [r3, #2888]
   108f8:	e28dd008 	add	sp, sp, #8	; 0x8
   108fc:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
   10900:	3c00d220 	.word	0x3c00d220
   10904:	27100000 	.word	0x27100000
   10908:	00010001 	.word	0x00010001
   1090c:	0c001780 	.word	0x0c001780
   10910:	3c00d224 	.word	0x3c00d224
   10914:	60080040 	.word	0x60080040
   10918:	3c00d21c 	.word	0x3c00d21c
   1091c:	18088200 	.word	0x18088200
   10920:	3c00d238 	.word	0x3c00d238
   10924:	00015f8e 	.word	0x00015f8e
   10928:	3c00d0dd 	.word	0x3c00d0dd
   1092c:	3c00d0fd 	.word	0x3c00d0fd
   10930:	3c00d0b4 	.word	0x3c00d0b4
   10934:	3c00d884 	.word	0x3c00d884
   10938:	3c00d0dc 	.word	0x3c00d0dc
   1093c:	18088040 	.word	0x18088040

00010940 <udc_write>:
   10940:	e59f30a4 	ldr	r3, [pc, #164]	; 109ec <udc_write+0xac>
   10944:	e3a02001 	mov	r2, #1	; 0x1
   10948:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   1094c:	e583000c 	str	r0, [r3, #12]
   10950:	e1a05001 	mov	r5, r1
   10954:	e5c32001 	strb	r2, [r3, #1]
   10958:	e1a04003 	mov	r4, r3
   1095c:	e1a08002 	mov	r8, r2
   10960:	e59f6088 	ldr	r6, [pc, #136]	; 109f0 <udc_write+0xb0>
   10964:	e59f7088 	ldr	r7, [pc, #136]	; 109f4 <udc_write+0xb4>
   10968:	ea00001b 	b	109dc <udc_write+0x9c>
   1096c:	e3550701 	cmp	r5, #262144	; 0x40000
   10970:	b1a02005 	movlt	r2, r5
   10974:	a3a02701 	movge	r2, #262144	; 0x40000
   10978:	e292303f 	adds	r3, r2, #63	; 0x3f
   1097c:	4282307e 	addmi	r3, r2, #126	; 0x7e
   10980:	e594100c 	ldr	r1, [r4, #12]
   10984:	e1a03343 	asr	r3, r3, #6
   10988:	e1823983 	orr	r3, r2, r3, lsl #19
   1098c:	e5863930 	str	r3, [r6, #2352]
   10990:	e5861934 	str	r1, [r6, #2356]
   10994:	e5963920 	ldr	r3, [r6, #2336]
   10998:	e5842004 	str	r2, [r4, #4]
   1099c:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
   109a0:	e5863920 	str	r3, [r6, #2336]
   109a4:	e5c48002 	strb	r8, [r4, #2]
   109a8:	ea000000 	b	109b0 <udc_write+0x70>
   109ac:	ebfffdf7 	bl	10190 <udc_event_handle>
   109b0:	e5d43002 	ldrb	r3, [r4, #2]
   109b4:	e3530000 	cmp	r3, #0	; 0x0
   109b8:	0a000002 	beq	109c8 <udc_write+0x88>
   109bc:	e5973000 	ldr	r3, [r7]
   109c0:	e3530000 	cmp	r3, #0	; 0x0
   109c4:	1afffff8 	bne	109ac <udc_write+0x6c>
   109c8:	e5943004 	ldr	r3, [r4, #4]
   109cc:	e594200c 	ldr	r2, [r4, #12]
   109d0:	e0635005 	rsb	r5, r3, r5
   109d4:	e0833002 	add	r3, r3, r2
   109d8:	e584300c 	str	r3, [r4, #12]
   109dc:	e3550000 	cmp	r5, #0	; 0x0
   109e0:	1affffe1 	bne	1096c <udc_write+0x2c>
   109e4:	e1a00005 	mov	r0, r5
   109e8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   109ec:	3c00d224 	.word	0x3c00d224
   109f0:	27100000 	.word	0x27100000
   109f4:	3c00d0d8 	.word	0x3c00d0d8

000109f8 <udc_read>:
   109f8:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
   109fc:	e59f30b4 	ldr	r3, [pc, #180]	; 10ab8 <udc_read+0xc0>
   10a00:	e24dd004 	sub	sp, sp, #4	; 0x4
   10a04:	e1a04001 	mov	r4, r1
   10a08:	e1a05003 	mov	r5, r3
   10a0c:	e5830010 	str	r0, [r3, #16]
   10a10:	e3a0a001 	mov	sl, #1	; 0x1
   10a14:	e59f60a0 	ldr	r6, [pc, #160]	; 10abc <udc_read+0xc4>
   10a18:	e59f80a0 	ldr	r8, [pc, #160]	; 10ac0 <udc_read+0xc8>
   10a1c:	e1a07003 	mov	r7, r3
   10a20:	ea00001f 	b	10aa4 <udc_read+0xac>
   10a24:	e3540701 	cmp	r4, #262144	; 0x40000
   10a28:	b1a02004 	movlt	r2, r4
   10a2c:	a3a02701 	movge	r2, #262144	; 0x40000
   10a30:	e2823f7f 	add	r3, r2, #508	; 0x1fc
   10a34:	e2833003 	add	r3, r3, #3	; 0x3
   10a38:	e5951010 	ldr	r1, [r5, #16]
   10a3c:	e1a034c3 	asr	r3, r3, #9
   10a40:	e1823983 	orr	r3, r2, r3, lsl #19
   10a44:	e5863b50 	str	r3, [r6, #2896]
   10a48:	e5861b54 	str	r1, [r6, #2900]
   10a4c:	e5963b40 	ldr	r3, [r6, #2880]
   10a50:	e5852008 	str	r2, [r5, #8]
   10a54:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
   10a58:	e5863b40 	str	r3, [r6, #2880]
   10a5c:	e5c5a003 	strb	sl, [r5, #3]
   10a60:	ea000002 	b	10a70 <udc_read+0x78>
   10a64:	ebfffdc9 	bl	10190 <udc_event_handle>
   10a68:	e3a00001 	mov	r0, #1	; 0x1
   10a6c:	ebfff3da 	bl	d9dc <udelay>
   10a70:	e5d53003 	ldrb	r3, [r5, #3]
   10a74:	e3530000 	cmp	r3, #0	; 0x0
   10a78:	0a000004 	beq	10a90 <udc_read+0x98>
   10a7c:	e5983000 	ldr	r3, [r8]
   10a80:	e3530000 	cmp	r3, #0	; 0x0
   10a84:	1afffff6 	bne	10a64 <udc_read+0x6c>
   10a88:	e3e04000 	mvn	r4, #0	; 0x0
   10a8c:	ea000006 	b	10aac <udc_read+0xb4>
   10a90:	e5973008 	ldr	r3, [r7, #8]
   10a94:	e5972010 	ldr	r2, [r7, #16]
   10a98:	e0634004 	rsb	r4, r3, r4
   10a9c:	e0833002 	add	r3, r3, r2
   10aa0:	e5873010 	str	r3, [r7, #16]
   10aa4:	e3540000 	cmp	r4, #0	; 0x0
   10aa8:	1affffdd 	bne	10a24 <udc_read+0x2c>
   10aac:	e1a00004 	mov	r0, r4
   10ab0:	e28dd004 	add	sp, sp, #4	; 0x4
   10ab4:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
   10ab8:	3c00d224 	.word	0x3c00d224
   10abc:	27100000 	.word	0x27100000
   10ac0:	3c00d0d8 	.word	0x3c00d0d8

00010ac4 <udc_connect>:
   10ac4:	e92d4070 	push	{r4, r5, r6, lr}
   10ac8:	e59f41e4 	ldr	r4, [pc, #484]	; 10cb4 <udc_connect+0x1f0>
   10acc:	e3a02000 	mov	r2, #0	; 0x0
   10ad0:	e5943024 	ldr	r3, [r4, #36]
   10ad4:	e3e01000 	mvn	r1, #0	; 0x0
   10ad8:	e1530002 	cmp	r3, r2
   10adc:	e59f31d4 	ldr	r3, [pc, #468]	; 10cb8 <udc_connect+0x1f4>
   10ae0:	e5c42003 	strb	r2, [r4, #3]
   10ae4:	e5c31000 	strb	r1, [r3]
   10ae8:	e5c42000 	strb	r2, [r4]
   10aec:	e5c42002 	strb	r2, [r4, #2]
   10af0:	1a000002 	bne	10b00 <udc_connect+0x3c>
   10af4:	e3a00028 	mov	r0, #40	; 0x28
   10af8:	ebffc2fa 	bl	16e8 <malloc>
   10afc:	e5840024 	str	r0, [r4, #36]
   10b00:	e59f31ac 	ldr	r3, [pc, #428]	; 10cb4 <udc_connect+0x1f0>
   10b04:	e3a06001 	mov	r6, #1	; 0x1
   10b08:	e59f51ac 	ldr	r5, [pc, #428]	; 10cbc <udc_connect+0x1f8>
   10b0c:	e59f01ac 	ldr	r0, [pc, #428]	; 10cc0 <udc_connect+0x1fc>
   10b10:	e5836020 	str	r6, [r3, #32]
   10b14:	ebffbf19 	bl	780 <module_enable>
   10b18:	e3a02004 	mov	r2, #4	; 0x4
   10b1c:	e3a03003 	mov	r3, #3	; 0x3
   10b20:	e3a01000 	mov	r1, #0	; 0x0
   10b24:	e585243c 	str	r2, [r5, #1084]
   10b28:	e5853440 	str	r3, [r5, #1088]
   10b2c:	e5852444 	str	r2, [r5, #1092]
   10b30:	e5853448 	str	r3, [r5, #1096]
   10b34:	e585344c 	str	r3, [r5, #1100]
   10b38:	e5853450 	str	r3, [r5, #1104]
   10b3c:	e5856454 	str	r6, [r5, #1108]
   10b40:	e5856458 	str	r6, [r5, #1112]
   10b44:	e585145c 	str	r1, [r5, #1116]
   10b48:	e5851460 	str	r1, [r5, #1120]
   10b4c:	e5953420 	ldr	r3, [r5, #1056]
   10b50:	e1a00006 	mov	r0, r6
   10b54:	e3c33007 	bic	r3, r3, #7	; 0x7
   10b58:	e3833005 	orr	r3, r3, #5	; 0x5
   10b5c:	e5853420 	str	r3, [r5, #1056]
   10b60:	e5953424 	ldr	r3, [r5, #1060]
   10b64:	e3833003 	orr	r3, r3, #3	; 0x3
   10b68:	e5853424 	str	r3, [r5, #1060]
   10b6c:	e595342c 	ldr	r3, [r5, #1068]
   10b70:	e1833006 	orr	r3, r3, r6
   10b74:	e585342c 	str	r3, [r5, #1068]
   10b78:	e5954400 	ldr	r4, [r5, #1024]
   10b7c:	e384400f 	orr	r4, r4, #15	; 0xf
   10b80:	e5854400 	str	r4, [r5, #1024]
   10b84:	ebfff394 	bl	d9dc <udelay>
   10b88:	e3c4300e 	bic	r3, r4, #14	; 0xe
   10b8c:	e5853400 	str	r3, [r5, #1024]
   10b90:	e1a00006 	mov	r0, r6
   10b94:	e3c4400f 	bic	r4, r4, #15	; 0xf
   10b98:	ebfff38f 	bl	d9dc <udelay>
   10b9c:	e5854400 	str	r4, [r5, #1024]
   10ba0:	e1a00006 	mov	r0, r6
   10ba4:	ebfff38c 	bl	d9dc <udelay>
   10ba8:	e59f2114 	ldr	r2, [pc, #276]	; 10cc4 <udc_connect+0x200>
   10bac:	e59f3114 	ldr	r3, [pc, #276]	; 10cc8 <udc_connect+0x204>
   10bb0:	e1a05002 	mov	r5, r2
   10bb4:	e582300c 	str	r3, [r2, #12]
   10bb8:	e5954010 	ldr	r4, [r5, #16]
   10bbc:	e3a00028 	mov	r0, #40	; 0x28
   10bc0:	ebfff385 	bl	d9dc <udelay>
   10bc4:	e3540000 	cmp	r4, #0	; 0x0
   10bc8:	e59f40f4 	ldr	r4, [pc, #244]	; 10cc4 <udc_connect+0x200>
   10bcc:	aafffff9 	bge	10bb8 <udc_connect+0xf4>
   10bd0:	e5943010 	ldr	r3, [r4, #16]
   10bd4:	e3a0000a 	mov	r0, #10	; 0xa
   10bd8:	e3833001 	orr	r3, r3, #1	; 0x1
   10bdc:	e5843010 	str	r3, [r4, #16]
   10be0:	ebfff37d 	bl	d9dc <udelay>
   10be4:	e1a05004 	mov	r5, r4
   10be8:	e5954010 	ldr	r4, [r5, #16]
   10bec:	e3a0000a 	mov	r0, #10	; 0xa
   10bf0:	ebfff379 	bl	d9dc <udelay>
   10bf4:	e3140001 	tst	r4, #1	; 0x1
   10bf8:	e59fc0c4 	ldr	ip, [pc, #196]	; 10cc4 <udc_connect+0x200>
   10bfc:	1afffff9 	bne	10be8 <udc_connect+0x124>
   10c00:	e3a03027 	mov	r3, #39	; 0x27
   10c04:	e58c3008 	str	r3, [ip, #8]
   10c08:	e59c303c 	ldr	r3, [ip, #60]
   10c0c:	e59c3014 	ldr	r3, [ip, #20]
   10c10:	e2130001 	ands	r0, r3, #1	; 0x1
   10c14:	1a000024 	bne	10cac <udc_connect+0x1e8>
   10c18:	e3a03301 	mov	r3, #67108864	; 0x4000000
   10c1c:	e58c3800 	str	r3, [ip, #2048]
   10c20:	e59f30a4 	ldr	r3, [pc, #164]	; 10ccc <udc_connect+0x208>
   10c24:	e59f20a4 	ldr	r2, [pc, #164]	; 10cd0 <udc_connect+0x20c>
   10c28:	e58c3018 	str	r3, [ip, #24]
   10c2c:	e59f30a0 	ldr	r3, [pc, #160]	; 10cd4 <udc_connect+0x210>
   10c30:	e59f10a0 	ldr	r1, [pc, #160]	; 10cd8 <udc_connect+0x214>
   10c34:	e5823014 	str	r3, [r2, #20]
   10c38:	e59f309c 	ldr	r3, [pc, #156]	; 10cdc <udc_connect+0x218>
   10c3c:	e5821010 	str	r1, [r2, #16]
   10c40:	e5823004 	str	r3, [r2, #4]
   10c44:	e59f3094 	ldr	r3, [pc, #148]	; 10ce0 <udc_connect+0x21c>
   10c48:	e582100c 	str	r1, [r2, #12]
   10c4c:	e5823008 	str	r3, [r2, #8]
   10c50:	ebfff385 	bl	da6c <get_timer>
   10c54:	e59f5058 	ldr	r5, [pc, #88]	; 10cb4 <udc_connect+0x1f0>
   10c58:	e1a04000 	mov	r4, r0
   10c5c:	ebfffd4b 	bl	10190 <udc_event_handle>
   10c60:	e5d53000 	ldrb	r3, [r5]
   10c64:	e1a00004 	mov	r0, r4
   10c68:	e3530000 	cmp	r3, #0	; 0x0
   10c6c:	1a000003 	bne	10c80 <udc_connect+0x1bc>
   10c70:	ebfff37d 	bl	da6c <get_timer>
   10c74:	e3500ffa 	cmp	r0, #1000	; 0x3e8
   10c78:	9afffff7 	bls	10c5c <udc_connect+0x198>
   10c7c:	ea00000a 	b	10cac <udc_connect+0x1e8>
   10c80:	e59f203c 	ldr	r2, [pc, #60]	; 10cc4 <udc_connect+0x200>
   10c84:	e59f3058 	ldr	r3, [pc, #88]	; 10ce4 <udc_connect+0x220>
   10c88:	e3a00000 	mov	r0, #0	; 0x0
   10c8c:	e5823920 	str	r3, [r2, #2336]
   10c90:	e2833d07 	add	r3, r3, #448	; 0x1c0
   10c94:	e5823b40 	str	r3, [r2, #2880]
   10c98:	e592381c 	ldr	r3, [r2, #2076]
   10c9c:	e3833701 	orr	r3, r3, #262144	; 0x40000
   10ca0:	e3833002 	orr	r3, r3, #2	; 0x2
   10ca4:	e582381c 	str	r3, [r2, #2076]
   10ca8:	e8bd8070 	pop	{r4, r5, r6, pc}
   10cac:	e3e00000 	mvn	r0, #0	; 0x0
   10cb0:	e8bd8070 	pop	{r4, r5, r6, pc}
   10cb4:	3c00d224 	.word	0x3c00d224
   10cb8:	3c00d0dc 	.word	0x3c00d0dc
   10cbc:	21e38000 	.word	0x21e38000
   10cc0:	21e38400 	.word	0x21e38400
   10cc4:	27100000 	.word	0x27100000
   10cc8:	4c001408 	.word	0x4c001408
   10ccc:	000c3c00 	.word	0x000c3c00
   10cd0:	3c00d884 	.word	0x3c00d884
   10cd4:	000189cb 	.word	0x000189cb
   10cd8:	000189e0 	.word	0x000189e0
   10cdc:	000189d4 	.word	0x000189d4
   10ce0:	3c00d0b8 	.word	0x3c00d0b8
   10ce4:	18088040 	.word	0x18088040

00010ce8 <linkpc_set_stage>:
   10ce8:	e1a01000 	mov	r1, r0
   10cec:	e59f0004 	ldr	r0, [pc, #4]	; 10cf8 <linkpc_set_stage+0x10>
   10cf0:	e3a02010 	mov	r2, #16	; 0x10
   10cf4:	eaffc1ac 	b	13ac <strncpy>
   10cf8:	3c00d108 	.word	0x3c00d108

00010cfc <__response_ok>:
   10cfc:	e92d4010 	push	{r4, lr}
   10d00:	e24dd020 	sub	sp, sp, #32	; 0x20
   10d04:	e59f1038 	ldr	r1, [pc, #56]	; 10d44 <__response_ok+0x48>
   10d08:	e3a02005 	mov	r2, #5	; 0x5
   10d0c:	e1a0000d 	mov	r0, sp
   10d10:	ebffc20f 	bl	1554 <memcpy>
   10d14:	e3a0201b 	mov	r2, #27	; 0x1b
   10d18:	e3a01000 	mov	r1, #0	; 0x0
   10d1c:	e28d0005 	add	r0, sp, #5	; 0x5
   10d20:	ebffc203 	bl	1534 <memset>
   10d24:	e1a0000d 	mov	r0, sp
   10d28:	e3a01004 	mov	r1, #4	; 0x4
   10d2c:	ebffff03 	bl	10940 <udc_write>
   10d30:	e59f0010 	ldr	r0, [pc, #16]	; 10d48 <__response_ok+0x4c>
   10d34:	e1a0400d 	mov	r4, sp
   10d38:	ebfff327 	bl	d9dc <udelay>
   10d3c:	e28dd020 	add	sp, sp, #32	; 0x20
   10d40:	e8bd8010 	pop	{r4, pc}
   10d44:	00015fa0 	.word	0x00015fa0
   10d48:	0007a120 	.word	0x0007a120

00010d4c <linkpc_loop>:
   10d4c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   10d50:	e59f05bc 	ldr	r0, [pc, #1468]	; 11314 <linkpc_loop+0x5c8>
   10d54:	e24ddf5b 	sub	sp, sp, #364	; 0x16c
   10d58:	e59f45b8 	ldr	r4, [pc, #1464]	; 11318 <linkpc_loop+0x5cc>
   10d5c:	e1a0e00f 	mov	lr, pc
   10d60:	e594f000 	ldr	pc, [r4]
   10d64:	ebffff56 	bl	10ac4 <udc_connect>
   10d68:	e3500000 	cmp	r0, #0	; 0x0
   10d6c:	13a04001 	movne	r4, #1	; 0x1
   10d70:	1a000160 	bne	112f8 <linkpc_loop+0x5ac>
   10d74:	e59f05a0 	ldr	r0, [pc, #1440]	; 1131c <linkpc_loop+0x5d0>
   10d78:	e1a0e00f 	mov	lr, pc
   10d7c:	e594f000 	ldr	pc, [r4]
   10d80:	e28d8094 	add	r8, sp, #148	; 0x94
   10d84:	e59f0594 	ldr	r0, [pc, #1428]	; 11320 <linkpc_loop+0x5d4>
   10d88:	e1a0e00f 	mov	lr, pc
   10d8c:	e594f000 	ldr	pc, [r4]
   10d90:	e59fb58c 	ldr	fp, [pc, #1420]	; 11324 <linkpc_loop+0x5d8>
   10d94:	e2882004 	add	r2, r8, #4	; 0x4
   10d98:	e1a09004 	mov	r9, r4
   10d9c:	e58d200c 	str	r2, [sp, #12]
   10da0:	e59b1000 	ldr	r1, [fp]
   10da4:	e3510000 	cmp	r1, #0	; 0x0
   10da8:	159fc578 	ldrne	ip, [pc, #1400]	; 11328 <linkpc_loop+0x5dc>
   10dac:	028d4f45 	addeq	r4, sp, #276	; 0x114
   10db0:	159c3000 	ldrne	r3, [ip]
   10db4:	03a05040 	moveq	r5, #64	; 0x40
   10db8:	1283430f 	addne	r4, r3, #1006632960	; 0x3c000000
   10dbc:	11a05001 	movne	r5, r1
   10dc0:	e1a01005 	mov	r1, r5
   10dc4:	e1a00004 	mov	r0, r4
   10dc8:	ebffff0a 	bl	109f8 <udc_read>
   10dcc:	e1a00008 	mov	r0, r8
   10dd0:	e3a01000 	mov	r1, #0	; 0x0
   10dd4:	e3a02080 	mov	r2, #128	; 0x80
   10dd8:	ebffc1d5 	bl	1534 <memset>
   10ddc:	e59b3000 	ldr	r3, [fp]
   10de0:	e3530000 	cmp	r3, #0	; 0x0
   10de4:	0a00002b 	beq	10e98 <linkpc_loop+0x14c>
   10de8:	e28d4014 	add	r4, sp, #20	; 0x14
   10dec:	e3a02080 	mov	r2, #128	; 0x80
   10df0:	e3a01000 	mov	r1, #0	; 0x0
   10df4:	e1a00004 	mov	r0, r4
   10df8:	ebffc1cd 	bl	1534 <memset>
   10dfc:	e59f0528 	ldr	r0, [pc, #1320]	; 1132c <linkpc_loop+0x5e0>
   10e00:	e59f1528 	ldr	r1, [pc, #1320]	; 11330 <linkpc_loop+0x5e4>
   10e04:	e1a0e00f 	mov	lr, pc
   10e08:	e599f000 	ldr	pc, [r9]
   10e0c:	e59f2514 	ldr	r2, [pc, #1300]	; 11328 <linkpc_loop+0x5dc>
   10e10:	e59b0000 	ldr	r0, [fp]
   10e14:	e5923000 	ldr	r3, [r2]
   10e18:	e59fc508 	ldr	ip, [pc, #1288]	; 11328 <linkpc_loop+0x5dc>
   10e1c:	e0632000 	rsb	r2, r3, r0
   10e20:	e1550002 	cmp	r5, r2
   10e24:	90833005 	addls	r3, r3, r5
   10e28:	80833002 	addhi	r3, r3, r2
   10e2c:	e1530000 	cmp	r3, r0
   10e30:	e58c3000 	str	r3, [ip]
   10e34:	3affffd9 	bcc	10da0 <linkpc_loop+0x54>
   10e38:	e59f34f4 	ldr	r3, [pc, #1268]	; 11334 <linkpc_loop+0x5e8>
   10e3c:	e5933000 	ldr	r3, [r3]
   10e40:	e3530000 	cmp	r3, #0	; 0x0
   10e44:	e59f34ec 	ldr	r3, [pc, #1260]	; 11338 <linkpc_loop+0x5ec>
   10e48:	159f14ec 	ldrne	r1, [pc, #1260]	; 1133c <linkpc_loop+0x5f0>
   10e4c:	e5830000 	str	r0, [r3]
   10e50:	e3a03000 	mov	r3, #0	; 0x0
   10e54:	11a00004 	movne	r0, r4
   10e58:	01a00004 	moveq	r0, r4
   10e5c:	059f14dc 	ldreq	r1, [pc, #1244]	; 11340 <linkpc_loop+0x5f4>
   10e60:	e58b3000 	str	r3, [fp]
   10e64:	e28d4014 	add	r4, sp, #20	; 0x14
   10e68:	eb0005cf 	bl	125ac <sprintf>
   10e6c:	e1a00004 	mov	r0, r4
   10e70:	ebffc19d 	bl	14ec <strlen>
   10e74:	e1a01000 	mov	r1, r0
   10e78:	e1a00004 	mov	r0, r4
   10e7c:	ebfffeaf 	bl	10940 <udc_write>
   10e80:	e59f24a0 	ldr	r2, [pc, #1184]	; 11328 <linkpc_loop+0x5dc>
   10e84:	e59f04b8 	ldr	r0, [pc, #1208]	; 11344 <linkpc_loop+0x5f8>
   10e88:	e5921000 	ldr	r1, [r2]
   10e8c:	e1a0e00f 	mov	lr, pc
   10e90:	e599f000 	ldr	pc, [r9]
   10e94:	eaffffc1 	b	10da0 <linkpc_loop+0x54>
   10e98:	e1a01004 	mov	r1, r4
   10e9c:	e59f0488 	ldr	r0, [pc, #1160]	; 1132c <linkpc_loop+0x5e0>
   10ea0:	e1a0e00f 	mov	lr, pc
   10ea4:	e599f000 	ldr	pc, [r9]
   10ea8:	e59f1498 	ldr	r1, [pc, #1176]	; 11348 <linkpc_loop+0x5fc>
   10eac:	e1a00008 	mov	r0, r8
   10eb0:	eb0005bd 	bl	125ac <sprintf>
   10eb4:	e1a00004 	mov	r0, r4
   10eb8:	e59f148c 	ldr	r1, [pc, #1164]	; 1134c <linkpc_loop+0x600>
   10ebc:	e3a02006 	mov	r2, #6	; 0x6
   10ec0:	ebffc16a 	bl	1470 <strncmp>
   10ec4:	e3500000 	cmp	r0, #0	; 0x0
   10ec8:	1a000002 	bne	10ed8 <linkpc_loop+0x18c>
   10ecc:	ebffff8a 	bl	10cfc <__response_ok>
   10ed0:	ebffbe69 	bl	87c <system_reset>
   10ed4:	ea0000fa 	b	112c4 <linkpc_loop+0x578>
   10ed8:	e1a00004 	mov	r0, r4
   10edc:	e59f146c 	ldr	r1, [pc, #1132]	; 11350 <linkpc_loop+0x604>
   10ee0:	e3a02007 	mov	r2, #7	; 0x7
   10ee4:	ebffc161 	bl	1470 <strncmp>
   10ee8:	e3500000 	cmp	r0, #0	; 0x0
   10eec:	1a00002d 	bne	10fa8 <linkpc_loop+0x25c>
   10ef0:	e2844007 	add	r4, r4, #7	; 0x7
   10ef4:	e1a02004 	mov	r2, r4
   10ef8:	e59f1454 	ldr	r1, [pc, #1108]	; 11354 <linkpc_loop+0x608>
   10efc:	e59f0454 	ldr	r0, [pc, #1108]	; 11358 <linkpc_loop+0x60c>
   10f00:	e1a0e00f 	mov	lr, pc
   10f04:	e599f000 	ldr	pc, [r9]
   10f08:	e59f1430 	ldr	r1, [pc, #1072]	; 11340 <linkpc_loop+0x5f4>
   10f0c:	e1a00008 	mov	r0, r8
   10f10:	ebffc11e 	bl	1390 <strcpy>
   10f14:	e59f1440 	ldr	r1, [pc, #1088]	; 1135c <linkpc_loop+0x610>
   10f18:	e1a00004 	mov	r0, r4
   10f1c:	e3a02007 	mov	r2, #7	; 0x7
   10f20:	ebffc152 	bl	1470 <strncmp>
   10f24:	e3500000 	cmp	r0, #0	; 0x0
   10f28:	059d000c 	ldreq	r0, [sp, #12]
   10f2c:	059f142c 	ldreq	r1, [pc, #1068]	; 11360 <linkpc_loop+0x614>
   10f30:	0a000010 	beq	10f78 <linkpc_loop+0x22c>
   10f34:	e59f1428 	ldr	r1, [pc, #1064]	; 11364 <linkpc_loop+0x618>
   10f38:	e1a00004 	mov	r0, r4
   10f3c:	e3a02005 	mov	r2, #5	; 0x5
   10f40:	ebffc14a 	bl	1470 <strncmp>
   10f44:	e3500000 	cmp	r0, #0	; 0x0
   10f48:	059d000c 	ldreq	r0, [sp, #12]
   10f4c:	059f1414 	ldreq	r1, [pc, #1044]	; 11368 <linkpc_loop+0x61c>
   10f50:	0a000008 	beq	10f78 <linkpc_loop+0x22c>
   10f54:	e1a00004 	mov	r0, r4
   10f58:	e59f140c 	ldr	r1, [pc, #1036]	; 1136c <linkpc_loop+0x620>
   10f5c:	e3a02008 	mov	r2, #8	; 0x8
   10f60:	ebffc142 	bl	1470 <strncmp>
   10f64:	e3500000 	cmp	r0, #0	; 0x0
   10f68:	1a000004 	bne	10f80 <linkpc_loop+0x234>
   10f6c:	e59f33fc 	ldr	r3, [pc, #1020]	; 11370 <linkpc_loop+0x624>
   10f70:	e59d000c 	ldr	r0, [sp, #12]
   10f74:	e5931008 	ldr	r1, [r3, #8]
   10f78:	ebffc104 	bl	1390 <strcpy>
   10f7c:	ea0000d0 	b	112c4 <linkpc_loop+0x578>
   10f80:	e1a00004 	mov	r0, r4
   10f84:	e59f13e8 	ldr	r1, [pc, #1000]	; 11374 <linkpc_loop+0x628>
   10f88:	e3a0200c 	mov	r2, #12	; 0xc
   10f8c:	ebffc137 	bl	1470 <strncmp>
   10f90:	e3500000 	cmp	r0, #0	; 0x0
   10f94:	1a0000ca 	bne	112c4 <linkpc_loop+0x578>
   10f98:	e59d000c 	ldr	r0, [sp, #12]
   10f9c:	e59f13d4 	ldr	r1, [pc, #980]	; 11378 <linkpc_loop+0x62c>
   10fa0:	e3a02903 	mov	r2, #49152	; 0xc000
   10fa4:	ea000020 	b	1102c <linkpc_loop+0x2e0>
   10fa8:	e1a00004 	mov	r0, r4
   10fac:	e59f13c8 	ldr	r1, [pc, #968]	; 1137c <linkpc_loop+0x630>
   10fb0:	e3a02009 	mov	r2, #9	; 0x9
   10fb4:	ebffc12d 	bl	1470 <strncmp>
   10fb8:	e2505000 	subs	r5, r0, #0	; 0x0
   10fbc:	1a00001c 	bne	11034 <linkpc_loop+0x2e8>
   10fc0:	e59f336c 	ldr	r3, [pc, #876]	; 11334 <linkpc_loop+0x5e8>
   10fc4:	e59fc35c 	ldr	ip, [pc, #860]	; 11328 <linkpc_loop+0x5dc>
   10fc8:	e3a02010 	mov	r2, #16	; 0x10
   10fcc:	e1a01005 	mov	r1, r5
   10fd0:	e5835000 	str	r5, [r3]
   10fd4:	e58c5000 	str	r5, [ip]
   10fd8:	e2840009 	add	r0, r4, #9	; 0x9
   10fdc:	eb0001e6 	bl	1177c <simple_strtoul>
   10fe0:	e1a01000 	mov	r1, r0
   10fe4:	e58b0000 	str	r0, [fp]
   10fe8:	e59f0390 	ldr	r0, [pc, #912]	; 11380 <linkpc_loop+0x634>
   10fec:	e1a0e00f 	mov	lr, pc
   10ff0:	e599f000 	ldr	pc, [r9]
   10ff4:	e59b2000 	ldr	r2, [fp]
   10ff8:	e3520000 	cmp	r2, #0	; 0x0
   10ffc:	01a00008 	moveq	r0, r8
   11000:	059f137c 	ldreq	r1, [pc, #892]	; 11384 <linkpc_loop+0x638>
   11004:	0a0000ad 	beq	112c0 <linkpc_loop+0x574>
   11008:	e3520903 	cmp	r2, #49152	; 0xc000
   1100c:	91a00008 	movls	r0, r8
   11010:	959f1370 	ldrls	r1, [pc, #880]	; 11388 <linkpc_loop+0x63c>
   11014:	9a000004 	bls	1102c <linkpc_loop+0x2e0>
   11018:	e1a00008 	mov	r0, r8
   1101c:	e59f1368 	ldr	r1, [pc, #872]	; 1138c <linkpc_loop+0x640>
   11020:	eb000561 	bl	125ac <sprintf>
   11024:	e58b5000 	str	r5, [fp]
   11028:	ea0000a5 	b	112c4 <linkpc_loop+0x578>
   1102c:	eb00055e 	bl	125ac <sprintf>
   11030:	ea0000a3 	b	112c4 <linkpc_loop+0x578>
   11034:	e1a00004 	mov	r0, r4
   11038:	e59f1350 	ldr	r1, [pc, #848]	; 11390 <linkpc_loop+0x644>
   1103c:	e3a02006 	mov	r2, #6	; 0x6
   11040:	ebffc10a 	bl	1470 <strncmp>
   11044:	e3500000 	cmp	r0, #0	; 0x0
   11048:	1a00005a 	bne	111b8 <linkpc_loop+0x46c>
   1104c:	e2844006 	add	r4, r4, #6	; 0x6
   11050:	e1a00004 	mov	r0, r4
   11054:	e59f1338 	ldr	r1, [pc, #824]	; 11394 <linkpc_loop+0x648>
   11058:	e3a02003 	mov	r2, #3	; 0x3
   1105c:	ebffc103 	bl	1470 <strncmp>
   11060:	e3500000 	cmp	r0, #0	; 0x0
   11064:	1a000007 	bne	11088 <linkpc_loop+0x33c>
   11068:	e280030f 	add	r0, r0, #1006632960	; 0x3c000000
   1106c:	ebffd981 	bl	7678 <cdbg_config_isi>
   11070:	e3500000 	cmp	r0, #0	; 0x0
   11074:	11a00008 	movne	r0, r8
   11078:	159f1318 	ldrne	r1, [pc, #792]	; 11398 <linkpc_loop+0x64c>
   1107c:	01a00008 	moveq	r0, r8
   11080:	059f12b8 	ldreq	r1, [pc, #696]	; 11340 <linkpc_loop+0x5f4>
   11084:	ea00008d 	b	112c0 <linkpc_loop+0x574>
   11088:	e1a00004 	mov	r0, r4
   1108c:	ebffd43f 	bl	6190 <vs_device_id>
   11090:	e59f5304 	ldr	r5, [pc, #772]	; 1139c <linkpc_loop+0x650>
   11094:	e59fc304 	ldr	ip, [pc, #772]	; 113a0 <linkpc_loop+0x654>
   11098:	e5953000 	ldr	r3, [r5]
   1109c:	e1a07000 	mov	r7, r0
   110a0:	e58d3000 	str	r3, [sp]
   110a4:	e59f02f8 	ldr	r0, [pc, #760]	; 113a4 <linkpc_loop+0x658>
   110a8:	e59f12f8 	ldr	r1, [pc, #760]	; 113a8 <linkpc_loop+0x65c>
   110ac:	e1a02007 	mov	r2, r7
   110b0:	e59c3000 	ldr	r3, [ip]
   110b4:	e1a0e00f 	mov	lr, pc
   110b8:	e599f000 	ldr	pc, [r9]
   110bc:	e357000e 	cmp	r7, #14	; 0xe
   110c0:	ca000006 	bgt	110e0 <linkpc_loop+0x394>
   110c4:	e1a00007 	mov	r0, r7
   110c8:	ebffd41c 	bl	6140 <vs_device_erasable>
   110cc:	e3500000 	cmp	r0, #0	; 0x0
   110d0:	0a000005 	beq	110ec <linkpc_loop+0x3a0>
   110d4:	e5953000 	ldr	r3, [r5]
   110d8:	e3530000 	cmp	r3, #0	; 0x0
   110dc:	1a000002 	bne	110ec <linkpc_loop+0x3a0>
   110e0:	e1a00008 	mov	r0, r8
   110e4:	e59f12c0 	ldr	r1, [pc, #704]	; 113ac <linkpc_loop+0x660>
   110e8:	ea000074 	b	112c0 <linkpc_loop+0x574>
   110ec:	e3a00901 	mov	r0, #16384	; 0x4000
   110f0:	ebffc17c 	bl	16e8 <malloc>
   110f4:	e250a000 	subs	sl, r0, #0	; 0x0
   110f8:	01a00008 	moveq	r0, r8
   110fc:	059f12ac 	ldreq	r1, [pc, #684]	; 113b0 <linkpc_loop+0x664>
   11100:	0a00006e 	beq	112c0 <linkpc_loop+0x574>
   11104:	e3a01001 	mov	r1, #1	; 0x1
   11108:	e1a00007 	mov	r0, r7
   1110c:	ebffd4f2 	bl	64dc <vs_assign_by_id>
   11110:	e3500000 	cmp	r0, #0	; 0x0
   11114:	b1a00008 	movlt	r0, r8
   11118:	b59f1294 	ldrlt	r1, [pc, #660]	; 113b4 <linkpc_loop+0x668>
   1111c:	ba000021 	blt	111a8 <linkpc_loop+0x45c>
   11120:	e1a04c07 	lsl	r4, r7, #24
   11124:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
   11128:	e58d4154 	str	r4, [sp, #340]
   1112c:	ebfff6a5 	bl	ebc8 <ram_addr2offs>
   11130:	e3a024ff 	mov	r2, #-16777216	; 0xff000000
   11134:	e0922000 	adds	r2, r2, r0
   11138:	e3e03000 	mvn	r3, #0	; 0x0
   1113c:	e0a33001 	adc	r3, r3, r1
   11140:	e59f1254 	ldr	r1, [pc, #596]	; 1139c <linkpc_loop+0x650>
   11144:	e28d0f55 	add	r0, sp, #340	; 0x154
   11148:	e1d1c0b2 	ldrh	ip, [r1, #2]
   1114c:	e1a014a2 	lsr	r1, r2, #9
   11150:	e1811b83 	orr	r1, r1, r3, lsl #23
   11154:	e59f2244 	ldr	r2, [pc, #580]	; 113a0 <linkpc_loop+0x654>
   11158:	e59f31d8 	ldr	r3, [pc, #472]	; 11338 <linkpc_loop+0x5ec>
   1115c:	e5925000 	ldr	r5, [r2]
   11160:	e5936000 	ldr	r6, [r3]
   11164:	e1a0c80c 	lsl	ip, ip, #16
   11168:	e1844001 	orr	r4, r4, r1
   1116c:	e1a03007 	mov	r3, r7
   11170:	e3a0100d 	mov	r1, #13	; 0xd
   11174:	e3a02000 	mov	r2, #0	; 0x0
   11178:	e58d4154 	str	r4, [sp, #340]
   1117c:	e58d5158 	str	r5, [sp, #344]
   11180:	e58d6160 	str	r6, [sp, #352]
   11184:	e58dc000 	str	ip, [sp]
   11188:	e58dc15c 	str	ip, [sp, #348]
   1118c:	e58da004 	str	sl, [sp, #4]
   11190:	ebffd94d 	bl	76cc <burn_image_loop>
   11194:	e3500000 	cmp	r0, #0	; 0x0
   11198:	159f1218 	ldrne	r1, [pc, #536]	; 113b8 <linkpc_loop+0x66c>
   1119c:	059f119c 	ldreq	r1, [pc, #412]	; 11340 <linkpc_loop+0x5f4>
   111a0:	11a00008 	movne	r0, r8
   111a4:	01a00008 	moveq	r0, r8
   111a8:	eb0004ff 	bl	125ac <sprintf>
   111ac:	e1a0000a 	mov	r0, sl
   111b0:	ebffc1b4 	bl	1888 <free>
   111b4:	ea000042 	b	112c4 <linkpc_loop+0x578>
   111b8:	e1a00004 	mov	r0, r4
   111bc:	e59f11f8 	ldr	r1, [pc, #504]	; 113bc <linkpc_loop+0x670>
   111c0:	e3a02008 	mov	r2, #8	; 0x8
   111c4:	ebffc0a9 	bl	1470 <strncmp>
   111c8:	e3500000 	cmp	r0, #0	; 0x0
   111cc:	1a000003 	bne	111e0 <linkpc_loop+0x494>
   111d0:	ebfffec9 	bl	10cfc <__response_ok>
   111d4:	e3a00001 	mov	r0, #1	; 0x1
   111d8:	ebffbc9a 	bl	448 <try_boot>
   111dc:	ea000038 	b	112c4 <linkpc_loop+0x578>
   111e0:	e1a00004 	mov	r0, r4
   111e4:	e59f11d4 	ldr	r1, [pc, #468]	; 113c0 <linkpc_loop+0x674>
   111e8:	e3a0200a 	mov	r2, #10	; 0xa
   111ec:	ebffc09f 	bl	1470 <strncmp>
   111f0:	e2505000 	subs	r5, r0, #0	; 0x0
   111f4:	1a000019 	bne	11260 <linkpc_loop+0x514>
   111f8:	e28d1f59 	add	r1, sp, #356	; 0x164
   111fc:	e284000b 	add	r0, r4, #11	; 0xb
   11200:	e3a02010 	mov	r2, #16	; 0x10
   11204:	eb00015c 	bl	1177c <simple_strtoul>
   11208:	e59f3190 	ldr	r3, [pc, #400]	; 113a0 <linkpc_loop+0x654>
   1120c:	e1a01005 	mov	r1, r5
   11210:	e5830000 	str	r0, [r3]
   11214:	e59d0164 	ldr	r0, [sp, #356]
   11218:	e3a02010 	mov	r2, #16	; 0x10
   1121c:	e2800001 	add	r0, r0, #1	; 0x1
   11220:	eb000155 	bl	1177c <simple_strtoul>
   11224:	e59f4170 	ldr	r4, [pc, #368]	; 1139c <linkpc_loop+0x650>
   11228:	e59fc170 	ldr	ip, [pc, #368]	; 113a0 <linkpc_loop+0x654>
   1122c:	e1a02000 	mov	r2, r0
   11230:	e5840000 	str	r0, [r4]
   11234:	e59c1000 	ldr	r1, [ip]
   11238:	e59f0184 	ldr	r0, [pc, #388]	; 113c4 <linkpc_loop+0x678>
   1123c:	e1a0e00f 	mov	lr, pc
   11240:	e599f000 	ldr	pc, [r9]
   11244:	e59fc154 	ldr	ip, [pc, #340]	; 113a0 <linkpc_loop+0x654>
   11248:	e1a00008 	mov	r0, r8
   1124c:	e5943000 	ldr	r3, [r4]
   11250:	e59f1170 	ldr	r1, [pc, #368]	; 113c8 <linkpc_loop+0x67c>
   11254:	e59c2000 	ldr	r2, [ip]
   11258:	eb0004d3 	bl	125ac <sprintf>
   1125c:	ea000018 	b	112c4 <linkpc_loop+0x578>
   11260:	e1a00004 	mov	r0, r4
   11264:	e59f1160 	ldr	r1, [pc, #352]	; 113cc <linkpc_loop+0x680>
   11268:	e3a0200d 	mov	r2, #13	; 0xd
   1126c:	ebffc07f 	bl	1470 <strncmp>
   11270:	e3500000 	cmp	r0, #0	; 0x0
   11274:	1a000006 	bne	11294 <linkpc_loop+0x548>
   11278:	e59f20f0 	ldr	r2, [pc, #240]	; 11370 <linkpc_loop+0x624>
   1127c:	e284100e 	add	r1, r4, #14	; 0xe
   11280:	e5920008 	ldr	r0, [r2, #8]
   11284:	ebffc041 	bl	1390 <strcpy>
   11288:	ebfffe9b 	bl	10cfc <__response_ok>
   1128c:	ebfffeae 	bl	10d4c <linkpc_loop>
   11290:	ea00000b 	b	112c4 <linkpc_loop+0x578>
   11294:	e1a00004 	mov	r0, r4
   11298:	e59f1130 	ldr	r1, [pc, #304]	; 113d0 <linkpc_loop+0x684>
   1129c:	e3a02007 	mov	r2, #7	; 0x7
   112a0:	ebffc072 	bl	1470 <strncmp>
   112a4:	e3500000 	cmp	r0, #0	; 0x0
   112a8:	1a000002 	bne	112b8 <linkpc_loop+0x56c>
   112ac:	ebfffe92 	bl	10cfc <__response_ok>
   112b0:	ebffd88f 	bl	74f4 <cdbg_shell>
   112b4:	eafffffe 	b	112b4 <linkpc_loop+0x568>
   112b8:	e59f1114 	ldr	r1, [pc, #276]	; 113d4 <linkpc_loop+0x688>
   112bc:	e1a00008 	mov	r0, r8
   112c0:	eb0004b9 	bl	125ac <sprintf>
   112c4:	e3a01c01 	mov	r1, #256	; 0x100
   112c8:	e1a00008 	mov	r0, r8
   112cc:	ebffc08e 	bl	150c <strnlen>
   112d0:	e1a01000 	mov	r1, r0
   112d4:	e1a00008 	mov	r0, r8
   112d8:	ebfffd98 	bl	10940 <udc_write>
   112dc:	e3500000 	cmp	r0, #0	; 0x0
   112e0:	0afffeae 	beq	10da0 <linkpc_loop+0x54>
   112e4:	e59f00ec 	ldr	r0, [pc, #236]	; 113d8 <linkpc_loop+0x68c>
   112e8:	e59f3028 	ldr	r3, [pc, #40]	; 11318 <linkpc_loop+0x5cc>
   112ec:	e1a0e00f 	mov	lr, pc
   112f0:	e593f000 	ldr	pc, [r3]
   112f4:	e3a04000 	mov	r4, #0	; 0x0
   112f8:	e59f00dc 	ldr	r0, [pc, #220]	; 113dc <linkpc_loop+0x690>
   112fc:	e59f3014 	ldr	r3, [pc, #20]	; 11318 <linkpc_loop+0x5cc>
   11300:	e1a0e00f 	mov	lr, pc
   11304:	e593f000 	ldr	pc, [r3]
   11308:	e1a00004 	mov	r0, r4
   1130c:	e28ddf5b 	add	sp, sp, #364	; 0x16c
   11310:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   11314:	000189f1 	.word	0x000189f1
   11318:	3c00d6cc 	.word	0x3c00d6cc
   1131c:	00016a2b 	.word	0x00016a2b
   11320:	00018a00 	.word	0x00018a00
   11324:	3c00d250 	.word	0x3c00d250
   11328:	3c00d254 	.word	0x3c00d254
   1132c:	00016437 	.word	0x00016437
   11330:	00015fc0 	.word	0x00015fc0
   11334:	3c00d258 	.word	0x3c00d258
   11338:	3c00d25c 	.word	0x3c00d25c
   1133c:	00018a13 	.word	0x00018a13
   11340:	00015fa0 	.word	0x00015fa0
   11344:	00018a19 	.word	0x00018a19
   11348:	00018a3c 	.word	0x00018a3c
   1134c:	00018a41 	.word	0x00018a41
   11350:	00018a48 	.word	0x00018a48
   11354:	00015fcc 	.word	0x00015fcc
   11358:	00018a50 	.word	0x00018a50
   1135c:	00018a58 	.word	0x00018a58
   11360:	3c00d08c 	.word	0x3c00d08c
   11364:	00018a60 	.word	0x00018a60
   11368:	3c00d108 	.word	0x3c00d108
   1136c:	00018a66 	.word	0x00018a66
   11370:	3c00d884 	.word	0x3c00d884
   11374:	00018a6f 	.word	0x00018a6f
   11378:	00018ad2 	.word	0x00018ad2
   1137c:	00018a7c 	.word	0x00018a7c
   11380:	00018a86 	.word	0x00018a86
   11384:	00018aa5 	.word	0x00018aa5
   11388:	00018ad7 	.word	0x00018ad7
   1138c:	00018abe 	.word	0x00018abe
   11390:	00018ae0 	.word	0x00018ae0
   11394:	000168b0 	.word	0x000168b0
   11398:	00018ae7 	.word	0x00018ae7
   1139c:	3c00d264 	.word	0x3c00d264
   113a0:	3c00d260 	.word	0x3c00d260
   113a4:	00018aff 	.word	0x00018aff
   113a8:	00015fd9 	.word	0x00015fd9
   113ac:	00018b0f 	.word	0x00018b0f
   113b0:	00018b26 	.word	0x00018b26
   113b4:	00018b34 	.word	0x00018b34
   113b8:	00018b4d 	.word	0x00018b4d
   113bc:	00018b62 	.word	0x00018b62
   113c0:	00018b6b 	.word	0x00018b6b
   113c4:	00018b76 	.word	0x00018b76
   113c8:	00018b8e 	.word	0x00018b8e
   113cc:	00018ba9 	.word	0x00018ba9
   113d0:	00018bb7 	.word	0x00018bb7
   113d4:	00018bbf 	.word	0x00018bbf
   113d8:	00018bd3 	.word	0x00018bd3
   113dc:	00018bf6 	.word	0x00018bf6

000113e0 <pp_list>:
   113e0:	00000994 	.word	0x00000994
   113e4:	00003771 	.word	0x00003771
   113e8:	00003739 	.word	0x00003739
   113ec:	00003cb9 	.word	0x00003cb9
   113f0:	000029a0 	.word	0x000029a0
   113f4:	00003f91 	.word	0x00003f91
   113f8:	000031e1 	.word	0x000031e1
   113fc:	00002e6b 	.word	0x00002e6b
   11400:	0000275d 	.word	0x0000275d
   11404:	00002297 	.word	0x00002297
   11408:	00002378 	.word	0x00002378
   1140c:	000034db 	.word	0x000034db
   11410:	000029fc 	.word	0x000029fc
   11414:	0000271c 	.word	0x0000271c
   11418:	000036ec 	.word	0x000036ec
   1141c:	00003ab9 	.word	0x00003ab9
   11420:	00003218 	.word	0x00003218
   11424:	0000307f 	.word	0x0000307f
   11428:	0000267b 	.word	0x0000267b
   1142c:	00003d95 	.word	0x00003d95
   11430:	00003e0f 	.word	0x00003e0f
   11434:	00002d75 	.word	0x00002d75
   11438:	000031a3 	.word	0x000031a3

0001143c <print_size>:
   1143c:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
   11440:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   11444:	e1a05000 	mov	r5, r0
   11448:	e1a08001 	mov	r8, r1
   1144c:	83a04101 	movhi	r4, #1073741824	; 0x40000000
   11450:	83a07047 	movhi	r7, #71	; 0x47
   11454:	8a000005 	bhi	11470 <print_size+0x34>
   11458:	e59f30a8 	ldr	r3, [pc, #168]	; 11508 <print_size+0xcc>
   1145c:	e1500003 	cmp	r0, r3
   11460:	83a0704d 	movhi	r7, #77	; 0x4d
   11464:	93a0706b 	movls	r7, #107	; 0x6b
   11468:	83a04601 	movhi	r4, #1048576	; 0x100000
   1146c:	93a04b01 	movls	r4, #1024	; 0x400
   11470:	e1a01004 	mov	r1, r4
   11474:	e1a00005 	mov	r0, r5
   11478:	eb000cc3 	bl	1478c <__aeabi_uidiv>
   1147c:	e1a01004 	mov	r1, r4
   11480:	e1a06000 	mov	r6, r0
   11484:	e1a00005 	mov	r0, r5
   11488:	eb000d3b 	bl	1497c <__aeabi_uidivmod>
   1148c:	e2510000 	subs	r0, r1, #0	; 0x0
   11490:	01a04000 	moveq	r4, r0
   11494:	0a000009 	beq	114c0 <print_size+0x84>
   11498:	e0625694 	mls	r2, r4, r6, r5
   1149c:	e1a030a4 	lsr	r3, r4, #1
   114a0:	e3a0000a 	mov	r0, #10	; 0xa
   114a4:	e1a01004 	mov	r1, r4
   114a8:	e0203290 	mla	r0, r0, r2, r3
   114ac:	eb000cb6 	bl	1478c <__aeabi_uidiv>
   114b0:	e3500009 	cmp	r0, #9	; 0x9
   114b4:	e1a04000 	mov	r4, r0
   114b8:	82866001 	addhi	r6, r6, #1	; 0x1
   114bc:	8240400a 	subhi	r4, r0, #10	; 0xa
   114c0:	e1a01006 	mov	r1, r6
   114c4:	e59f5040 	ldr	r5, [pc, #64]	; 1150c <print_size+0xd0>
   114c8:	e59f0040 	ldr	r0, [pc, #64]	; 11510 <print_size+0xd4>
   114cc:	e1a0e00f 	mov	lr, pc
   114d0:	e595f000 	ldr	pc, [r5]
   114d4:	e3540000 	cmp	r4, #0	; 0x0
   114d8:	0a000003 	beq	114ec <print_size+0xb0>
   114dc:	e1a01004 	mov	r1, r4
   114e0:	e59f002c 	ldr	r0, [pc, #44]	; 11514 <print_size+0xd8>
   114e4:	e1a0e00f 	mov	lr, pc
   114e8:	e595f000 	ldr	pc, [r5]
   114ec:	e1a01007 	mov	r1, r7
   114f0:	e1a02008 	mov	r2, r8
   114f4:	e59f001c 	ldr	r0, [pc, #28]	; 11518 <print_size+0xdc>
   114f8:	e59f300c 	ldr	r3, [pc, #12]	; 1150c <print_size+0xd0>
   114fc:	e1a0e00f 	mov	lr, pc
   11500:	e593f000 	ldr	pc, [r3]
   11504:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   11508:	000fffff 	.word	0x000fffff
   1150c:	3c00d6cc 	.word	0x3c00d6cc
   11510:	00018bfe 	.word	0x00018bfe
   11514:	00018c03 	.word	0x00018c03
   11518:	00018c08 	.word	0x00018c08

0001151c <print_buffer>:
   1151c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   11520:	e24dd044 	sub	sp, sp, #68	; 0x44
   11524:	e59d6068 	ldr	r6, [sp, #104]
   11528:	e1a07002 	mov	r7, r2
   1152c:	e0020692 	mul	r2, r2, r6
   11530:	e3520040 	cmp	r2, #64	; 0x40
   11534:	e1a09000 	mov	r9, r0
   11538:	e1a0a001 	mov	sl, r1
   1153c:	e1a08003 	mov	r8, r3
   11540:	9a000003 	bls	11554 <print_buffer+0x38>
   11544:	e3a00040 	mov	r0, #64	; 0x40
   11548:	e1a01007 	mov	r1, r7
   1154c:	eb000c8e 	bl	1478c <__aeabi_uidiv>
   11550:	e1a06000 	mov	r6, r0
   11554:	e3560000 	cmp	r6, #0	; 0x0
   11558:	1a00003c 	bne	11650 <print_buffer+0x134>
   1155c:	e3a00010 	mov	r0, #16	; 0x10
   11560:	e1a01007 	mov	r1, r7
   11564:	eb000c88 	bl	1478c <__aeabi_uidiv>
   11568:	e1a06000 	mov	r6, r0
   1156c:	ea000037 	b	11650 <print_buffer+0x134>
   11570:	e59f00f0 	ldr	r0, [pc, #240]	; 11668 <print_buffer+0x14c>
   11574:	e1a01009 	mov	r1, r9
   11578:	e1a0e00f 	mov	lr, pc
   1157c:	e59bf000 	ldr	pc, [fp]
   11580:	e1560008 	cmp	r6, r8
   11584:	21a06008 	movcs	r6, r8
   11588:	e1a0500a 	mov	r5, sl
   1158c:	e3a04000 	mov	r4, #0	; 0x0
   11590:	ea00000f 	b	115d4 <print_buffer+0xb8>
   11594:	e3570004 	cmp	r7, #4	; 0x4
   11598:	05951000 	ldreq	r1, [r5]
   1159c:	059f00c8 	ldreq	r0, [pc, #200]	; 1166c <print_buffer+0x150>
   115a0:	078d1104 	streq	r1, [sp, r4, lsl #2]
   115a4:	0a000006 	beq	115c4 <print_buffer+0xa8>
   115a8:	e3570002 	cmp	r7, #2	; 0x2
   115ac:	01d510b0 	ldrheq	r1, [r5]
   115b0:	15d51000 	ldrbne	r1, [r5]
   115b4:	01a03084 	lsleq	r3, r4, #1
   115b8:	059f00b0 	ldreq	r0, [pc, #176]	; 11670 <print_buffer+0x154>
   115bc:	018d10b3 	strheq	r1, [sp, r3]
   115c0:	17cd1004 	strbne	r1, [sp, r4]
   115c4:	e1a0e00f 	mov	lr, pc
   115c8:	e59bf000 	ldr	pc, [fp]
   115cc:	e0855007 	add	r5, r5, r7
   115d0:	e2844001 	add	r4, r4, #1	; 0x1
   115d4:	e1540006 	cmp	r4, r6
   115d8:	e59f0094 	ldr	r0, [pc, #148]	; 11674 <print_buffer+0x158>
   115dc:	3affffec 	bcc	11594 <print_buffer+0x78>
   115e0:	e0050697 	mul	r5, r7, r6
   115e4:	e59f008c 	ldr	r0, [pc, #140]	; 11678 <print_buffer+0x15c>
   115e8:	e08aa005 	add	sl, sl, r5
   115ec:	eb000450 	bl	12734 <puts>
   115f0:	e3a04000 	mov	r4, #0	; 0x0
   115f4:	ea000009 	b	11620 <print_buffer+0x104>
   115f8:	e7dd0004 	ldrb	r0, [sp, r4]
   115fc:	e59f2078 	ldr	r2, [pc, #120]	; 1167c <print_buffer+0x160>
   11600:	e2844001 	add	r4, r4, #1	; 0x1
   11604:	e7d23000 	ldrb	r3, [r2, r0]
   11608:	e3130097 	tst	r3, #151	; 0x97
   1160c:	0a000001 	beq	11618 <print_buffer+0xfc>
   11610:	e3100080 	tst	r0, #128	; 0x80
   11614:	0a000000 	beq	1161c <print_buffer+0x100>
   11618:	e3a0002e 	mov	r0, #46	; 0x2e
   1161c:	eb00046c 	bl	127d4 <putc>
   11620:	e1540005 	cmp	r4, r5
   11624:	3afffff3 	bcc	115f8 <print_buffer+0xdc>
   11628:	e59f0050 	ldr	r0, [pc, #80]	; 11680 <print_buffer+0x164>
   1162c:	e1a0e00f 	mov	lr, pc
   11630:	e59bf000 	ldr	pc, [fp]
   11634:	eb000469 	bl	127e0 <ctrlc>
   11638:	e3500000 	cmp	r0, #0	; 0x0
   1163c:	13e00000 	mvnne	r0, #0	; 0x0
   11640:	1a000006 	bne	11660 <print_buffer+0x144>
   11644:	e0899005 	add	r9, r9, r5
   11648:	e0668008 	rsb	r8, r6, r8
   1164c:	ea000000 	b	11654 <print_buffer+0x138>
   11650:	e59fb02c 	ldr	fp, [pc, #44]	; 11684 <print_buffer+0x168>
   11654:	e3580000 	cmp	r8, #0	; 0x0
   11658:	1affffc4 	bne	11570 <print_buffer+0x54>
   1165c:	e1a00008 	mov	r0, r8
   11660:	e28dd044 	add	sp, sp, #68	; 0x44
   11664:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   11668:	00018c0f 	.word	0x00018c0f
   1166c:	0001804f 	.word	0x0001804f
   11670:	00018c16 	.word	0x00018c16
   11674:	00018c1c 	.word	0x00018c1c
   11678:	00018c22 	.word	0x00018c22
   1167c:	00016003 	.word	0x00016003
   11680:	00016bef 	.word	0x00016bef
   11684:	3c00d6cc 	.word	0x3c00d6cc

00011688 <__div64_32>:
   11688:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
   1168c:	e8900300 	ldm	r0, {r8, r9}
   11690:	e1510009 	cmp	r1, r9
   11694:	e1a0a000 	mov	sl, r0
   11698:	e1a04001 	mov	r4, r1
   1169c:	83a06000 	movhi	r6, #0	; 0x0
   116a0:	83a07000 	movhi	r7, #0	; 0x0
   116a4:	8a000007 	bhi	116c8 <__div64_32+0x40>
   116a8:	e1a00009 	mov	r0, r9
   116ac:	eb000c36 	bl	1478c <__aeabi_uidiv>
   116b0:	e0030094 	mul	r3, r4, r0
   116b4:	e3a06000 	mov	r6, #0	; 0x0
   116b8:	e1a02006 	mov	r2, r6
   116bc:	e1a07000 	mov	r7, r0
   116c0:	e0588002 	subs	r8, r8, r2
   116c4:	e0c99003 	sbc	r9, r9, r3
   116c8:	e1a00004 	mov	r0, r4
   116cc:	e3a01000 	mov	r1, #0	; 0x0
   116d0:	e3a05000 	mov	r5, #0	; 0x0
   116d4:	e3a04001 	mov	r4, #1	; 0x1
   116d8:	ea000003 	b	116ec <__div64_32+0x64>
   116dc:	e0900000 	adds	r0, r0, r0
   116e0:	e0a11001 	adc	r1, r1, r1
   116e4:	e0944004 	adds	r4, r4, r4
   116e8:	e0a55005 	adc	r5, r5, r5
   116ec:	e3510000 	cmp	r1, #0	; 0x0
   116f0:	e3a03000 	mov	r3, #0	; 0x0
   116f4:	ca000002 	bgt	11704 <__div64_32+0x7c>
   116f8:	1a000002 	bne	11708 <__div64_32+0x80>
   116fc:	e3500000 	cmp	r0, #0	; 0x0
   11700:	0a000000 	beq	11708 <__div64_32+0x80>
   11704:	e3a03001 	mov	r3, #1	; 0x1
   11708:	e1590001 	cmp	r9, r1
   1170c:	e3a02000 	mov	r2, #0	; 0x0
   11710:	8a000002 	bhi	11720 <__div64_32+0x98>
   11714:	1a000002 	bne	11724 <__div64_32+0x9c>
   11718:	e1580000 	cmp	r8, r0
   1171c:	9a000000 	bls	11724 <__div64_32+0x9c>
   11720:	e3a02001 	mov	r2, #1	; 0x1
   11724:	e0033002 	and	r3, r3, r2
   11728:	e31300ff 	tst	r3, #255	; 0xff
   1172c:	1affffea 	bne	116dc <__div64_32+0x54>
   11730:	e1510009 	cmp	r1, r9
   11734:	8a000006 	bhi	11754 <__div64_32+0xcc>
   11738:	1a000001 	bne	11744 <__div64_32+0xbc>
   1173c:	e1500008 	cmp	r0, r8
   11740:	8a000003 	bhi	11754 <__div64_32+0xcc>
   11744:	e0588000 	subs	r8, r8, r0
   11748:	e0c99001 	sbc	r9, r9, r1
   1174c:	e0966004 	adds	r6, r6, r4
   11750:	e0a77005 	adc	r7, r7, r5
   11754:	e1b050a5 	lsrs	r5, r5, #1
   11758:	e1a04064 	rrx	r4, r4
   1175c:	e1943005 	orrs	r3, r4, r5
   11760:	0a000002 	beq	11770 <__div64_32+0xe8>
   11764:	e1b010a1 	lsrs	r1, r1, #1
   11768:	e1a00060 	rrx	r0, r0
   1176c:	eaffffef 	b	11730 <__div64_32+0xa8>
   11770:	e88a00c0 	stm	sl, {r6, r7}
   11774:	e1a00008 	mov	r0, r8
   11778:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

0001177c <simple_strtoul>:
   1177c:	e92d4070 	push	{r4, r5, r6, lr}
   11780:	e5d03000 	ldrb	r3, [r0]
   11784:	e1a05001 	mov	r5, r1
   11788:	e3530030 	cmp	r3, #48	; 0x30
   1178c:	e1a01002 	mov	r1, r2
   11790:	1a00000d 	bne	117cc <simple_strtoul+0x50>
   11794:	e5f03001 	ldrb	r3, [r0, #1]!
   11798:	e3530078 	cmp	r3, #120	; 0x78
   1179c:	1a000007 	bne	117c0 <simple_strtoul+0x44>
   117a0:	e5d03001 	ldrb	r3, [r0, #1]
   117a4:	e59f2084 	ldr	r2, [pc, #132]	; 11830 <simple_strtoul+0xb4>
   117a8:	e280c001 	add	ip, r0, #1	; 0x1
   117ac:	e7d23003 	ldrb	r3, [r2, r3]
   117b0:	e3130044 	tst	r3, #68	; 0x44
   117b4:	11a0000c 	movne	r0, ip
   117b8:	13a01010 	movne	r1, #16	; 0x10
   117bc:	1a000004 	bne	117d4 <simple_strtoul+0x58>
   117c0:	e3510000 	cmp	r1, #0	; 0x0
   117c4:	03a01008 	moveq	r1, #8	; 0x8
   117c8:	ea000001 	b	117d4 <simple_strtoul+0x58>
   117cc:	e3520000 	cmp	r2, #0	; 0x0
   117d0:	03a0100a 	moveq	r1, #10	; 0xa
   117d4:	e3a04000 	mov	r4, #0	; 0x0
   117d8:	e59f6050 	ldr	r6, [pc, #80]	; 11830 <simple_strtoul+0xb4>
   117dc:	ea000001 	b	117e8 <simple_strtoul+0x6c>
   117e0:	e0243491 	mla	r4, r1, r4, r3
   117e4:	e2800001 	add	r0, r0, #1	; 0x1
   117e8:	e5d0c000 	ldrb	ip, [r0]
   117ec:	e7d6200c 	ldrb	r2, [r6, ip]
   117f0:	e24c3030 	sub	r3, ip, #48	; 0x30
   117f4:	e3120044 	tst	r2, #68	; 0x44
   117f8:	0a000008 	beq	11820 <simple_strtoul+0xa4>
   117fc:	e3120004 	tst	r2, #4	; 0x4
   11800:	1a000004 	bne	11818 <simple_strtoul+0x9c>
   11804:	e24c3020 	sub	r3, ip, #32	; 0x20
   11808:	e3120002 	tst	r2, #2	; 0x2
   1180c:	e6ef3073 	uxtb	r3, r3
   11810:	12433037 	subne	r3, r3, #55	; 0x37
   11814:	024c3037 	subeq	r3, ip, #55	; 0x37
   11818:	e1530001 	cmp	r3, r1
   1181c:	3affffef 	bcc	117e0 <simple_strtoul+0x64>
   11820:	e3550000 	cmp	r5, #0	; 0x0
   11824:	15850000 	strne	r0, [r5]
   11828:	e1a00004 	mov	r0, r4
   1182c:	e8bd8070 	pop	{r4, r5, r6, pc}
   11830:	00016003 	.word	0x00016003

00011834 <simple_strtol>:
   11834:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   11838:	e5d03000 	ldrb	r3, [r0]
   1183c:	e24dd004 	sub	sp, sp, #4	; 0x4
   11840:	e353002d 	cmp	r3, #45	; 0x2d
   11844:	1a000004 	bne	1185c <simple_strtol+0x28>
   11848:	e2800001 	add	r0, r0, #1	; 0x1
   1184c:	ebffffca 	bl	1177c <simple_strtoul>
   11850:	e2600000 	rsb	r0, r0, #0	; 0x0
   11854:	e28dd004 	add	sp, sp, #4	; 0x4
   11858:	e8bd8000 	pop	{pc}
   1185c:	e28dd004 	add	sp, sp, #4	; 0x4
   11860:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
   11864:	eaffffc4 	b	1177c <simple_strtoul>

00011868 <ustrtoul>:
   11868:	e92d4010 	push	{r4, lr}
   1186c:	e1a04001 	mov	r4, r1
   11870:	ebffffc1 	bl	1177c <simple_strtoul>
   11874:	e5941000 	ldr	r1, [r4]
   11878:	e5d13000 	ldrb	r3, [r1]
   1187c:	e353004b 	cmp	r3, #75	; 0x4b
   11880:	0a00000a 	beq	118b0 <ustrtoul+0x48>
   11884:	8a000002 	bhi	11894 <ustrtoul+0x2c>
   11888:	e3530047 	cmp	r3, #71	; 0x47
   1188c:	18bd8010 	popne	{r4, pc}
   11890:	ea000004 	b	118a8 <ustrtoul+0x40>
   11894:	e353004d 	cmp	r3, #77	; 0x4d
   11898:	0a000003 	beq	118ac <ustrtoul+0x44>
   1189c:	e353006b 	cmp	r3, #107	; 0x6b
   118a0:	18bd8010 	popne	{r4, pc}
   118a4:	ea000001 	b	118b0 <ustrtoul+0x48>
   118a8:	e1a00500 	lsl	r0, r0, #10
   118ac:	e1a00500 	lsl	r0, r0, #10
   118b0:	e5d13001 	ldrb	r3, [r1, #1]
   118b4:	e1a00500 	lsl	r0, r0, #10
   118b8:	e3530069 	cmp	r3, #105	; 0x69
   118bc:	18bd8010 	popne	{r4, pc}
   118c0:	e5d13002 	ldrb	r3, [r1, #2]
   118c4:	e2812002 	add	r2, r1, #2	; 0x2
   118c8:	e3530042 	cmp	r3, #66	; 0x42
   118cc:	02813003 	addeq	r3, r1, #3	; 0x3
   118d0:	05843000 	streq	r3, [r4]
   118d4:	15842000 	strne	r2, [r4]
   118d8:	e8bd8010 	pop	{r4, pc}

000118dc <simple_strtoull>:
   118dc:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
   118e0:	e5d03000 	ldrb	r3, [r0]
   118e4:	e1a07000 	mov	r7, r0
   118e8:	e3530030 	cmp	r3, #48	; 0x30
   118ec:	e1a08001 	mov	r8, r1
   118f0:	e1a06002 	mov	r6, r2
   118f4:	1a00000d 	bne	11930 <simple_strtoull+0x54>
   118f8:	e5f73001 	ldrb	r3, [r7, #1]!
   118fc:	e3530078 	cmp	r3, #120	; 0x78
   11900:	1a000007 	bne	11924 <simple_strtoull+0x48>
   11904:	e5d73001 	ldrb	r3, [r7, #1]
   11908:	e59f20ac 	ldr	r2, [pc, #172]	; 119bc <simple_strtoull+0xe0>
   1190c:	e2870001 	add	r0, r7, #1	; 0x1
   11910:	e7d23003 	ldrb	r3, [r2, r3]
   11914:	e3130044 	tst	r3, #68	; 0x44
   11918:	11a07000 	movne	r7, r0
   1191c:	13a06010 	movne	r6, #16	; 0x10
   11920:	1a000004 	bne	11938 <simple_strtoull+0x5c>
   11924:	e3560000 	cmp	r6, #0	; 0x0
   11928:	03a06008 	moveq	r6, #8	; 0x8
   1192c:	ea000001 	b	11938 <simple_strtoull+0x5c>
   11930:	e3520000 	cmp	r2, #0	; 0x0
   11934:	03a0600a 	moveq	r6, #10	; 0xa
   11938:	e3a04000 	mov	r4, #0	; 0x0
   1193c:	e3a05000 	mov	r5, #0	; 0x0
   11940:	e59f9074 	ldr	r9, [pc, #116]	; 119bc <simple_strtoull+0xe0>
   11944:	e3a0a000 	mov	sl, #0	; 0x0
   11948:	ea000004 	b	11960 <simple_strtoull+0x84>
   1194c:	e0832694 	umull	r2, r3, r4, r6
   11950:	e0233596 	mla	r3, r6, r5, r3
   11954:	e0904002 	adds	r4, r0, r2
   11958:	e0a15003 	adc	r5, r1, r3
   1195c:	e2877001 	add	r7, r7, #1	; 0x1
   11960:	e5d7c000 	ldrb	ip, [r7]
   11964:	e7d9200c 	ldrb	r2, [r9, ip]
   11968:	e24c3030 	sub	r3, ip, #48	; 0x30
   1196c:	e3120044 	tst	r2, #68	; 0x44
   11970:	0a00000c 	beq	119a8 <simple_strtoull+0xcc>
   11974:	e3120004 	tst	r2, #4	; 0x4
   11978:	1a000004 	bne	11990 <simple_strtoull+0xb4>
   1197c:	e24c3020 	sub	r3, ip, #32	; 0x20
   11980:	e3120002 	tst	r2, #2	; 0x2
   11984:	e6ef3073 	uxtb	r3, r3
   11988:	12433037 	subne	r3, r3, #55	; 0x37
   1198c:	024c3037 	subeq	r3, ip, #55	; 0x37
   11990:	e1a00003 	mov	r0, r3
   11994:	e1a01fc0 	asr	r1, r0, #31
   11998:	e15a0001 	cmp	sl, r1
   1199c:	1a000001 	bne	119a8 <simple_strtoull+0xcc>
   119a0:	e1560000 	cmp	r6, r0
   119a4:	8affffe8 	bhi	1194c <simple_strtoull+0x70>
   119a8:	e3580000 	cmp	r8, #0	; 0x0
   119ac:	15887000 	strne	r7, [r8]
   119b0:	e1a01005 	mov	r1, r5
   119b4:	e1a00004 	mov	r0, r4
   119b8:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
   119bc:	00016003 	.word	0x00016003

000119c0 <put_dec_trunc>:
   119c0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   119c4:	e1a03421 	lsr	r3, r1, #8
   119c8:	e1a04221 	lsr	r4, r1, #4
   119cc:	e204400f 	and	r4, r4, #15	; 0xf
   119d0:	e203600f 	and	r6, r3, #15	; 0xf
   119d4:	e1a05621 	lsr	r5, r1, #12
   119d8:	e0842006 	add	r2, r4, r6
   119dc:	e201100f 	and	r1, r1, #15	; 0xf
   119e0:	e3a03006 	mov	r3, #6	; 0x6
   119e4:	e0822005 	add	r2, r2, r5
   119e8:	e0221293 	mla	r2, r3, r2, r1
   119ec:	e3a080cd 	mov	r8, #205	; 0xcd
   119f0:	e00c0298 	mul	ip, r8, r2
   119f4:	e3a01009 	mov	r1, #9	; 0x9
   119f8:	e0214591 	mla	r1, r1, r5, r4
   119fc:	e1a0c5ac 	lsr	ip, ip, #11
   11a00:	e3a0700a 	mov	r7, #10	; 0xa
   11a04:	e3a03005 	mov	r3, #5	; 0x5
   11a08:	e0231693 	mla	r3, r3, r6, r1
   11a0c:	e0622c97 	mls	r2, r7, ip, r2
   11a10:	e093100c 	adds	r1, r3, ip
   11a14:	e2822030 	add	r2, r2, #48	; 0x30
   11a18:	e1a04000 	mov	r4, r0
   11a1c:	e4c02001 	strb	r2, [r0], #1
   11a20:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
   11a24:	e0030198 	mul	r3, r8, r1
   11a28:	e1a035a3 	lsr	r3, r3, #11
   11a2c:	e0621397 	mls	r2, r7, r3, r1
   11a30:	e0831086 	add	r1, r3, r6, lsl #1
   11a34:	e2822030 	add	r2, r2, #48	; 0x30
   11a38:	e1913005 	orrs	r3, r1, r5
   11a3c:	e5c42001 	strb	r2, [r4, #1]
   11a40:	e2800001 	add	r0, r0, #1	; 0x1
   11a44:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
   11a48:	e3a0300d 	mov	r3, #13	; 0xd
   11a4c:	e0030193 	mul	r3, r3, r1
   11a50:	e1a033a3 	lsr	r3, r3, #7
   11a54:	e0621397 	mls	r2, r7, r3, r1
   11a58:	e0931105 	adds	r1, r3, r5, lsl #2
   11a5c:	e2822030 	add	r2, r2, #48	; 0x30
   11a60:	e4c02001 	strb	r2, [r0], #1
   11a64:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
   11a68:	e0030198 	mul	r3, r8, r1
   11a6c:	e1a0c5a3 	lsr	ip, r3, #11
   11a70:	e0631c97 	mls	r3, r7, ip, r1
   11a74:	e35c0000 	cmp	ip, #0	; 0x0
   11a78:	e2833030 	add	r3, r3, #48	; 0x30
   11a7c:	e4c03001 	strb	r3, [r0], #1
   11a80:	128c3030 	addne	r3, ip, #48	; 0x30
   11a84:	14c03001 	strbne	r3, [r0], #1
   11a88:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

00011a8c <string>:
   11a8c:	e59fc0a4 	ldr	ip, [pc, #164]	; 11b38 <string+0xac>
   11a90:	e3510000 	cmp	r1, #0	; 0x0
   11a94:	e92d4070 	push	{r4, r5, r6, lr}
   11a98:	11a06001 	movne	r6, r1
   11a9c:	01a0600c 	moveq	r6, ip
   11aa0:	e1a01003 	mov	r1, r3
   11aa4:	e1a04000 	mov	r4, r0
   11aa8:	e1a00006 	mov	r0, r6
   11aac:	e1a05002 	mov	r5, r2
   11ab0:	ebffbe95 	bl	150c <strnlen>
   11ab4:	e59d3010 	ldr	r3, [sp, #16]
   11ab8:	e1a0c000 	mov	ip, r0
   11abc:	e3130010 	tst	r3, #16	; 0x10
   11ac0:	01a02004 	moveq	r2, r4
   11ac4:	03a01020 	moveq	r1, #32	; 0x20
   11ac8:	0a000001 	beq	11ad4 <string+0x48>
   11acc:	ea000006 	b	11aec <string+0x60>
   11ad0:	e5421001 	strb	r1, [r2, #-1]
   11ad4:	e2455001 	sub	r5, r5, #1	; 0x1
   11ad8:	e2853001 	add	r3, r5, #1	; 0x1
   11adc:	e15c0003 	cmp	ip, r3
   11ae0:	e1a04002 	mov	r4, r2
   11ae4:	e2822001 	add	r2, r2, #1	; 0x1
   11ae8:	bafffff8 	blt	11ad0 <string+0x44>
   11aec:	e3a02000 	mov	r2, #0	; 0x0
   11af0:	ea000002 	b	11b00 <string+0x74>
   11af4:	e7d63002 	ldrb	r3, [r6, r2]
   11af8:	e7c43002 	strb	r3, [r4, r2]
   11afc:	e2822001 	add	r2, r2, #1	; 0x1
   11b00:	e152000c 	cmp	r2, ip
   11b04:	e0843002 	add	r3, r4, r2
   11b08:	bafffff9 	blt	11af4 <string+0x68>
   11b0c:	e1a01003 	mov	r1, r3
   11b10:	e1a02005 	mov	r2, r5
   11b14:	e3a03020 	mov	r3, #32	; 0x20
   11b18:	ea000000 	b	11b20 <string+0x94>
   11b1c:	e5413001 	strb	r3, [r1, #-1]
   11b20:	e15c0002 	cmp	ip, r2
   11b24:	e1a00001 	mov	r0, r1
   11b28:	e2422001 	sub	r2, r2, #1	; 0x1
   11b2c:	e2811001 	add	r1, r1, #1	; 0x1
   11b30:	bafffff9 	blt	11b1c <string+0x90>
   11b34:	e8bd8070 	pop	{r4, r5, r6, pc}
   11b38:	00018c27 	.word	0x00018c27

00011b3c <put_dec>:
   11b3c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   11b40:	e24dd00c 	sub	sp, sp, #12	; 0xc
   11b44:	e28d8008 	add	r8, sp, #8	; 0x8
   11b48:	e16820f8 	strd	r2, [r8, #-8]!
   11b4c:	e1a07000 	mov	r7, r0
   11b50:	e3a090cd 	mov	r9, #205	; 0xcd
   11b54:	e3a0a00a 	mov	sl, #10	; 0xa
   11b58:	e3a0b00d 	mov	fp, #13	; 0xd
   11b5c:	e59d5004 	ldr	r5, [sp, #4]
   11b60:	e1a0000d 	mov	r0, sp
   11b64:	e3550000 	cmp	r5, #0	; 0x0
   11b68:	e59f10f0 	ldr	r1, [pc, #240]	; 11c60 <put_dec+0x124>
   11b6c:	e59d4000 	ldr	r4, [sp]
   11b70:	1a00000f 	bne	11bb4 <put_dec+0x78>
   11b74:	e59f30e8 	ldr	r3, [pc, #232]	; 11c64 <put_dec+0x128>
   11b78:	e1a00004 	mov	r0, r4
   11b7c:	e1540003 	cmp	r4, r3
   11b80:	8a000004 	bhi	11b98 <put_dec+0x5c>
   11b84:	e1a00007 	mov	r0, r7
   11b88:	e1a01004 	mov	r1, r4
   11b8c:	ebffff8b 	bl	119c0 <put_dec_trunc>
   11b90:	e28dd00c 	add	sp, sp, #12	; 0xc
   11b94:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   11b98:	eb000b77 	bl	1497c <__aeabi_uidivmod>
   11b9c:	e1a00004 	mov	r0, r4
   11ba0:	e1a06001 	mov	r6, r1
   11ba4:	e59f10b4 	ldr	r1, [pc, #180]	; 11c60 <put_dec+0x124>
   11ba8:	eb000af7 	bl	1478c <__aeabi_uidiv>
   11bac:	e88d0021 	stm	sp, {r0, r5}
   11bb0:	ea000001 	b	11bbc <put_dec+0x80>
   11bb4:	ebfffeb3 	bl	11688 <__div64_32>
   11bb8:	e1a06000 	mov	r6, r0
   11bbc:	e1a02226 	lsr	r2, r6, #4
   11bc0:	e1a0c426 	lsr	ip, r6, #8
   11bc4:	e20cc00f 	and	ip, ip, #15	; 0xf
   11bc8:	e202200f 	and	r2, r2, #15	; 0xf
   11bcc:	e1a0e626 	lsr	lr, r6, #12
   11bd0:	e082500c 	add	r5, r2, ip
   11bd4:	e085500e 	add	r5, r5, lr
   11bd8:	e206300f 	and	r3, r6, #15	; 0xf
   11bdc:	e3a01006 	mov	r1, #6	; 0x6
   11be0:	e0253591 	mla	r5, r1, r5, r3
   11be4:	e3a03009 	mov	r3, #9	; 0x9
   11be8:	e0222e93 	mla	r2, r3, lr, r2
   11bec:	e3a01005 	mov	r1, #5	; 0x5
   11bf0:	e0000599 	mul	r0, r9, r5
   11bf4:	e0222c91 	mla	r2, r1, ip, r2
   11bf8:	e1a005a0 	lsr	r0, r0, #11
   11bfc:	e0822000 	add	r2, r2, r0
   11c00:	e0010299 	mul	r1, r9, r2
   11c04:	e1a015a1 	lsr	r1, r1, #11
   11c08:	e081c08c 	add	ip, r1, ip, lsl #1
   11c0c:	e0030c9b 	mul	r3, fp, ip
   11c10:	e1a033a3 	lsr	r3, r3, #7
   11c14:	e083e10e 	add	lr, r3, lr, lsl #2
   11c18:	e0040e99 	mul	r4, r9, lr
   11c1c:	e1a045a4 	lsr	r4, r4, #11
   11c20:	e06ee49a 	mls	lr, sl, r4, lr
   11c24:	e060509a 	mls	r0, sl, r0, r5
   11c28:	e061219a 	mls	r1, sl, r1, r2
   11c2c:	e063c39a 	mls	r3, sl, r3, ip
   11c30:	e2800030 	add	r0, r0, #48	; 0x30
   11c34:	e2811030 	add	r1, r1, #48	; 0x30
   11c38:	e2833030 	add	r3, r3, #48	; 0x30
   11c3c:	e28ee030 	add	lr, lr, #48	; 0x30
   11c40:	e2844030 	add	r4, r4, #48	; 0x30
   11c44:	e5c70000 	strb	r0, [r7]
   11c48:	e5c71001 	strb	r1, [r7, #1]
   11c4c:	e5c73002 	strb	r3, [r7, #2]
   11c50:	e5c7e003 	strb	lr, [r7, #3]
   11c54:	e5c74004 	strb	r4, [r7, #4]
   11c58:	e2877005 	add	r7, r7, #5	; 0x5
   11c5c:	eaffffbe 	b	11b5c <put_dec+0x20>
   11c60:	000186a0 	.word	0x000186a0
   11c64:	0001869f 	.word	0x0001869f

00011c68 <number>:
   11c68:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   11c6c:	e24dd05c 	sub	sp, sp, #92	; 0x5c
   11c70:	e1a0a000 	mov	sl, r0
   11c74:	e59d0080 	ldr	r0, [sp, #128]
   11c78:	e59d808c 	ldr	r8, [sp, #140]
   11c7c:	e250100a 	subs	r1, r0, #10	; 0xa
   11c80:	13a01001 	movne	r1, #1	; 0x1
   11c84:	e0011328 	and	r1, r1, r8, lsr #6
   11c88:	e3180010 	tst	r8, #16	; 0x10
   11c8c:	e58d1008 	str	r1, [sp, #8]
   11c90:	e2081020 	and	r1, r8, #32	; 0x20
   11c94:	13c88001 	bicne	r8, r8, #1	; 0x1
   11c98:	e3180002 	tst	r8, #2	; 0x2
   11c9c:	e1a06002 	mov	r6, r2
   11ca0:	e1a07003 	mov	r7, r3
   11ca4:	e59d5084 	ldr	r5, [sp, #132]
   11ca8:	e58d1004 	str	r1, [sp, #4]
   11cac:	0a000010 	beq	11cf4 <number+0x8c>
   11cb0:	e3530000 	cmp	r3, #0	; 0x0
   11cb4:	ba000002 	blt	11cc4 <number+0x5c>
   11cb8:	e3180004 	tst	r8, #4	; 0x4
   11cbc:	0a000008 	beq	11ce4 <number+0x7c>
   11cc0:	ea000004 	b	11cd8 <number+0x70>
   11cc4:	e2766000 	rsbs	r6, r6, #0	; 0x0
   11cc8:	e2e77000 	rsc	r7, r7, #0	; 0x0
   11ccc:	e2455001 	sub	r5, r5, #1	; 0x1
   11cd0:	e3a0902d 	mov	r9, #45	; 0x2d
   11cd4:	ea000007 	b	11cf8 <number+0x90>
   11cd8:	e2455001 	sub	r5, r5, #1	; 0x1
   11cdc:	e3a0902b 	mov	r9, #43	; 0x2b
   11ce0:	ea000004 	b	11cf8 <number+0x90>
   11ce4:	e3180008 	tst	r8, #8	; 0x8
   11ce8:	12455001 	subne	r5, r5, #1	; 0x1
   11cec:	13a09020 	movne	r9, #32	; 0x20
   11cf0:	1a000000 	bne	11cf8 <number+0x90>
   11cf4:	e3a09000 	mov	r9, #0	; 0x0
   11cf8:	e59d2008 	ldr	r2, [sp, #8]
   11cfc:	e3520000 	cmp	r2, #0	; 0x0
   11d00:	0a000003 	beq	11d14 <number+0xac>
   11d04:	e59d3080 	ldr	r3, [sp, #128]
   11d08:	e2455001 	sub	r5, r5, #1	; 0x1
   11d0c:	e3530010 	cmp	r3, #16	; 0x10
   11d10:	02455001 	subeq	r5, r5, #1	; 0x1
   11d14:	e196c007 	orrs	ip, r6, r7
   11d18:	03a03030 	moveq	r3, #48	; 0x30
   11d1c:	05cd3016 	strbeq	r3, [sp, #22]
   11d20:	03a04001 	moveq	r4, #1	; 0x1
   11d24:	0a000020 	beq	11dac <number+0x144>
   11d28:	e59d0080 	ldr	r0, [sp, #128]
   11d2c:	e350000a 	cmp	r0, #10	; 0xa
   11d30:	0a000017 	beq	11d94 <number+0x12c>
   11d34:	e3500010 	cmp	r0, #16	; 0x10
   11d38:	03a01004 	moveq	r1, #4	; 0x4
   11d3c:	13a01003 	movne	r1, #3	; 0x3
   11d40:	e58d100c 	str	r1, [sp, #12]
   11d44:	e240b001 	sub	fp, r0, #1	; 0x1
   11d48:	e3a04000 	mov	r4, #0	; 0x0
   11d4c:	e6ef3076 	uxtb	r3, r6
   11d50:	e59f2164 	ldr	r2, [pc, #356]	; 11ebc <number+0x254>
   11d54:	e003300b 	and	r3, r3, fp
   11d58:	e59dc004 	ldr	ip, [sp, #4]
   11d5c:	e7d23003 	ldrb	r3, [r2, r3]
   11d60:	e1a00006 	mov	r0, r6
   11d64:	e18c3003 	orr	r3, ip, r3
   11d68:	e28dc016 	add	ip, sp, #22	; 0x16
   11d6c:	e1a01007 	mov	r1, r7
   11d70:	e59d200c 	ldr	r2, [sp, #12]
   11d74:	e7cc3004 	strb	r3, [ip, r4]
   11d78:	eb000b93 	bl	14bcc <__aeabi_llsr>
   11d7c:	e1a06000 	mov	r6, r0
   11d80:	e1960001 	orrs	r0, r6, r1
   11d84:	e1a07001 	mov	r7, r1
   11d88:	e2844001 	add	r4, r4, #1	; 0x1
   11d8c:	1affffee 	bne	11d4c <number+0xe4>
   11d90:	ea000005 	b	11dac <number+0x144>
   11d94:	e28d4016 	add	r4, sp, #22	; 0x16
   11d98:	e1a00004 	mov	r0, r4
   11d9c:	e1a02006 	mov	r2, r6
   11da0:	e1a03007 	mov	r3, r7
   11da4:	ebffff64 	bl	11b3c <put_dec>
   11da8:	e0644000 	rsb	r4, r4, r0
   11dac:	e59d3088 	ldr	r3, [sp, #136]
   11db0:	e1540003 	cmp	r4, r3
   11db4:	a1a01004 	movge	r1, r4
   11db8:	b1a01003 	movlt	r1, r3
   11dbc:	e3180011 	tst	r8, #17	; 0x11
   11dc0:	e061c005 	rsb	ip, r1, r5
   11dc4:	01a0300a 	moveq	r3, sl
   11dc8:	03a02020 	moveq	r2, #32	; 0x20
   11dcc:	0a000001 	beq	11dd8 <number+0x170>
   11dd0:	ea000004 	b	11de8 <number+0x180>
   11dd4:	e5432001 	strb	r2, [r3, #-1]
   11dd8:	e25cc001 	subs	ip, ip, #1	; 0x1
   11ddc:	e1a0a003 	mov	sl, r3
   11de0:	e2833001 	add	r3, r3, #1	; 0x1
   11de4:	5afffffa 	bpl	11dd4 <number+0x16c>
   11de8:	e3590000 	cmp	r9, #0	; 0x0
   11dec:	14ca9001 	strbne	r9, [sl], #1
   11df0:	e59d2008 	ldr	r2, [sp, #8]
   11df4:	e3520000 	cmp	r2, #0	; 0x0
   11df8:	0a000006 	beq	11e18 <number+0x1b0>
   11dfc:	e59d3080 	ldr	r3, [sp, #128]
   11e00:	e3530010 	cmp	r3, #16	; 0x10
   11e04:	e3a03030 	mov	r3, #48	; 0x30
   11e08:	e4ca3001 	strb	r3, [sl], #1
   11e0c:	059d0004 	ldreq	r0, [sp, #4]
   11e10:	03803058 	orreq	r3, r0, #88	; 0x58
   11e14:	04ca3001 	strbeq	r3, [sl], #1
   11e18:	e3180010 	tst	r8, #16	; 0x10
   11e1c:	1a000009 	bne	11e48 <number+0x1e0>
   11e20:	e3180001 	tst	r8, #1	; 0x1
   11e24:	13a02030 	movne	r2, #48	; 0x30
   11e28:	03a02020 	moveq	r2, #32	; 0x20
   11e2c:	e1a0300a 	mov	r3, sl
   11e30:	ea000000 	b	11e38 <number+0x1d0>
   11e34:	e5432001 	strb	r2, [r3, #-1]
   11e38:	e25cc001 	subs	ip, ip, #1	; 0x1
   11e3c:	e1a0a003 	mov	sl, r3
   11e40:	e2833001 	add	r3, r3, #1	; 0x1
   11e44:	5afffffa 	bpl	11e34 <number+0x1cc>
   11e48:	e1a0000a 	mov	r0, sl
   11e4c:	e3a03030 	mov	r3, #48	; 0x30
   11e50:	ea000000 	b	11e58 <number+0x1f0>
   11e54:	e5403001 	strb	r3, [r0, #-1]
   11e58:	e2411001 	sub	r1, r1, #1	; 0x1
   11e5c:	e1540001 	cmp	r4, r1
   11e60:	e1a02000 	mov	r2, r0
   11e64:	e2800001 	add	r0, r0, #1	; 0x1
   11e68:	dafffff9 	ble	11e54 <number+0x1ec>
   11e6c:	e28d1058 	add	r1, sp, #88	; 0x58
   11e70:	e0813004 	add	r3, r1, r4
   11e74:	e2431043 	sub	r1, r3, #67	; 0x43
   11e78:	ea000001 	b	11e84 <number+0x21c>
   11e7c:	e5d13001 	ldrb	r3, [r1, #1]
   11e80:	e5423001 	strb	r3, [r2, #-1]
   11e84:	e2544001 	subs	r4, r4, #1	; 0x1
   11e88:	e1a03002 	mov	r3, r2
   11e8c:	e2411001 	sub	r1, r1, #1	; 0x1
   11e90:	e2822001 	add	r2, r2, #1	; 0x1
   11e94:	5afffff8 	bpl	11e7c <number+0x214>
   11e98:	e3a02020 	mov	r2, #32	; 0x20
   11e9c:	ea000000 	b	11ea4 <number+0x23c>
   11ea0:	e5432001 	strb	r2, [r3, #-1]
   11ea4:	e25cc001 	subs	ip, ip, #1	; 0x1
   11ea8:	e1a00003 	mov	r0, r3
   11eac:	e2833001 	add	r3, r3, #1	; 0x1
   11eb0:	5afffffa 	bpl	11ea0 <number+0x238>
   11eb4:	e28dd05c 	add	sp, sp, #92	; 0x5c
   11eb8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   11ebc:	00015ff3 	.word	0x00015ff3

00011ec0 <vsprintf>:
   11ec0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   11ec4:	e24dd084 	sub	sp, sp, #132	; 0x84
   11ec8:	e58d0028 	str	r0, [sp, #40]
   11ecc:	e1a08001 	mov	r8, r1
   11ed0:	e1a0a000 	mov	sl, r0
   11ed4:	e28d107b 	add	r1, sp, #123	; 0x7b
   11ed8:	e28d007d 	add	r0, sp, #125	; 0x7d
   11edc:	e58d2024 	str	r2, [sp, #36]
   11ee0:	e1a06002 	mov	r6, r2
   11ee4:	e58d001c 	str	r0, [sp, #28]
   11ee8:	e58d1018 	str	r1, [sp, #24]
   11eec:	ea0001a3 	b	12580 <vsprintf+0x6c0>
   11ef0:	e3530025 	cmp	r3, #37	; 0x25
   11ef4:	1a00015f 	bne	12478 <vsprintf+0x5b8>
   11ef8:	e3a07000 	mov	r7, #0	; 0x0
   11efc:	e5f82001 	ldrb	r2, [r8, #1]!
   11f00:	e352002b 	cmp	r2, #43	; 0x2b
   11f04:	0a00000c 	beq	11f3c <vsprintf+0x7c>
   11f08:	8a000004 	bhi	11f20 <vsprintf+0x60>
   11f0c:	e3520020 	cmp	r2, #32	; 0x20
   11f10:	0a00000b 	beq	11f44 <vsprintf+0x84>
   11f14:	e3520023 	cmp	r2, #35	; 0x23
   11f18:	1a00000f 	bne	11f5c <vsprintf+0x9c>
   11f1c:	ea00000a 	b	11f4c <vsprintf+0x8c>
   11f20:	e352002d 	cmp	r2, #45	; 0x2d
   11f24:	0a000002 	beq	11f34 <vsprintf+0x74>
   11f28:	e3520030 	cmp	r2, #48	; 0x30
   11f2c:	1a00000a 	bne	11f5c <vsprintf+0x9c>
   11f30:	ea000007 	b	11f54 <vsprintf+0x94>
   11f34:	e3877010 	orr	r7, r7, #16	; 0x10
   11f38:	eaffffef 	b	11efc <vsprintf+0x3c>
   11f3c:	e3877004 	orr	r7, r7, #4	; 0x4
   11f40:	eaffffed 	b	11efc <vsprintf+0x3c>
   11f44:	e3877008 	orr	r7, r7, #8	; 0x8
   11f48:	eaffffeb 	b	11efc <vsprintf+0x3c>
   11f4c:	e3877040 	orr	r7, r7, #64	; 0x40
   11f50:	eaffffe9 	b	11efc <vsprintf+0x3c>
   11f54:	e3877001 	orr	r7, r7, #1	; 0x1
   11f58:	eaffffe7 	b	11efc <vsprintf+0x3c>
   11f5c:	e2423030 	sub	r3, r2, #48	; 0x30
   11f60:	e6ef3073 	uxtb	r3, r3
   11f64:	e3530009 	cmp	r3, #9	; 0x9
   11f68:	93a09000 	movls	r9, #0	; 0x0
   11f6c:	93a0100a 	movls	r1, #10	; 0xa
   11f70:	9a000002 	bls	11f80 <vsprintf+0xc0>
   11f74:	ea000007 	b	11f98 <vsprintf+0xd8>
   11f78:	e0292991 	mla	r9, r1, r9, r2
   11f7c:	e2888001 	add	r8, r8, #1	; 0x1
   11f80:	e5d83000 	ldrb	r3, [r8]
   11f84:	e2432030 	sub	r2, r3, #48	; 0x30
   11f88:	e6ef3072 	uxtb	r3, r2
   11f8c:	e3530009 	cmp	r3, #9	; 0x9
   11f90:	9afffff8 	bls	11f78 <vsprintf+0xb8>
   11f94:	ea000008 	b	11fbc <vsprintf+0xfc>
   11f98:	e352002a 	cmp	r2, #42	; 0x2a
   11f9c:	13e09000 	mvnne	r9, #0	; 0x0
   11fa0:	1a000005 	bne	11fbc <vsprintf+0xfc>
   11fa4:	e5969000 	ldr	r9, [r6]
   11fa8:	e2888001 	add	r8, r8, #1	; 0x1
   11fac:	e3590000 	cmp	r9, #0	; 0x0
   11fb0:	e2866004 	add	r6, r6, #4	; 0x4
   11fb4:	b2699000 	rsblt	r9, r9, #0	; 0x0
   11fb8:	b3877010 	orrlt	r7, r7, #16	; 0x10
   11fbc:	e5d83000 	ldrb	r3, [r8]
   11fc0:	e353002e 	cmp	r3, #46	; 0x2e
   11fc4:	13e02000 	mvnne	r2, #0	; 0x0
   11fc8:	158d202c 	strne	r2, [sp, #44]
   11fcc:	1a00001d 	bne	12048 <vsprintf+0x188>
   11fd0:	e5f82001 	ldrb	r2, [r8, #1]!
   11fd4:	e2423030 	sub	r3, r2, #48	; 0x30
   11fd8:	e6ef3073 	uxtb	r3, r3
   11fdc:	e3530009 	cmp	r3, #9	; 0x9
   11fe0:	93a03000 	movls	r3, #0	; 0x0
   11fe4:	958d302c 	strls	r3, [sp, #44]
   11fe8:	93a0100a 	movls	r1, #10	; 0xa
   11fec:	9a000004 	bls	12004 <vsprintf+0x144>
   11ff0:	ea000009 	b	1201c <vsprintf+0x15c>
   11ff4:	e59d002c 	ldr	r0, [sp, #44]
   11ff8:	e2888001 	add	r8, r8, #1	; 0x1
   11ffc:	e0202091 	mla	r0, r1, r0, r2
   12000:	e58d002c 	str	r0, [sp, #44]
   12004:	e5d83000 	ldrb	r3, [r8]
   12008:	e2432030 	sub	r2, r3, #48	; 0x30
   1200c:	e6ef3072 	uxtb	r3, r2
   12010:	e3530009 	cmp	r3, #9	; 0x9
   12014:	9afffff6 	bls	11ff4 <vsprintf+0x134>
   12018:	ea000005 	b	12034 <vsprintf+0x174>
   1201c:	e352002a 	cmp	r2, #42	; 0x2a
   12020:	1a000006 	bne	12040 <vsprintf+0x180>
   12024:	e5961000 	ldr	r1, [r6]
   12028:	e2888001 	add	r8, r8, #1	; 0x1
   1202c:	e58d102c 	str	r1, [sp, #44]
   12030:	e2866004 	add	r6, r6, #4	; 0x4
   12034:	e59d202c 	ldr	r2, [sp, #44]
   12038:	e3520000 	cmp	r2, #0	; 0x0
   1203c:	aa000001 	bge	12048 <vsprintf+0x188>
   12040:	e3a03000 	mov	r3, #0	; 0x0
   12044:	e58d302c 	str	r3, [sp, #44]
   12048:	e5d83000 	ldrb	r3, [r8]
   1204c:	e353006c 	cmp	r3, #108	; 0x6c
   12050:	13530068 	cmpne	r3, #104	; 0x68
   12054:	0a000008 	beq	1207c <vsprintf+0x1bc>
   12058:	e353004c 	cmp	r3, #76	; 0x4c
   1205c:	0a000006 	beq	1207c <vsprintf+0x1bc>
   12060:	e353005a 	cmp	r3, #90	; 0x5a
   12064:	0a000004 	beq	1207c <vsprintf+0x1bc>
   12068:	e353007a 	cmp	r3, #122	; 0x7a
   1206c:	0a000002 	beq	1207c <vsprintf+0x1bc>
   12070:	e3530074 	cmp	r3, #116	; 0x74
   12074:	13e02000 	mvnne	r2, #0	; 0x0
   12078:	1a000007 	bne	1209c <vsprintf+0x1dc>
   1207c:	e353006c 	cmp	r3, #108	; 0x6c
   12080:	e1a02003 	mov	r2, r3
   12084:	e2888001 	add	r8, r8, #1	; 0x1
   12088:	1a000003 	bne	1209c <vsprintf+0x1dc>
   1208c:	e5d83000 	ldrb	r3, [r8]
   12090:	e353006c 	cmp	r3, #108	; 0x6c
   12094:	02888001 	addeq	r8, r8, #1	; 0x1
   12098:	03a0204c 	moveq	r2, #76	; 0x4c
   1209c:	e5d83000 	ldrb	r3, [r8]
   120a0:	e353006e 	cmp	r3, #110	; 0x6e
   120a4:	0a0000df 	beq	12428 <vsprintf+0x568>
   120a8:	8a00000c 	bhi	120e0 <vsprintf+0x220>
   120ac:	e3530063 	cmp	r3, #99	; 0x63
   120b0:	0a000017 	beq	12114 <vsprintf+0x254>
   120b4:	8a000004 	bhi	120cc <vsprintf+0x20c>
   120b8:	e3530025 	cmp	r3, #37	; 0x25
   120bc:	0a0000ed 	beq	12478 <vsprintf+0x5b8>
   120c0:	e3530058 	cmp	r3, #88	; 0x58
   120c4:	1a0000e5 	bne	12460 <vsprintf+0x5a0>
   120c8:	ea0000e0 	b	12450 <vsprintf+0x590>
   120cc:	e3530064 	cmp	r3, #100	; 0x64
   120d0:	0a0000e0 	beq	12458 <vsprintf+0x598>
   120d4:	e3530069 	cmp	r3, #105	; 0x69
   120d8:	1a0000e0 	bne	12460 <vsprintf+0x5a0>
   120dc:	ea0000dd 	b	12458 <vsprintf+0x598>
   120e0:	e3530073 	cmp	r3, #115	; 0x73
   120e4:	0a000021 	beq	12170 <vsprintf+0x2b0>
   120e8:	8a000004 	bhi	12100 <vsprintf+0x240>
   120ec:	e353006f 	cmp	r3, #111	; 0x6f
   120f0:	0a0000d3 	beq	12444 <vsprintf+0x584>
   120f4:	e3530070 	cmp	r3, #112	; 0x70
   120f8:	1a0000d8 	bne	12460 <vsprintf+0x5a0>
   120fc:	ea000023 	b	12190 <vsprintf+0x2d0>
   12100:	e3530075 	cmp	r3, #117	; 0x75
   12104:	0a0000dd 	beq	12480 <vsprintf+0x5c0>
   12108:	e3530078 	cmp	r3, #120	; 0x78
   1210c:	1a0000d3 	bne	12460 <vsprintf+0x5a0>
   12110:	ea0000cd 	b	1244c <vsprintf+0x58c>
   12114:	e3170010 	tst	r7, #16	; 0x10
   12118:	01a0300a 	moveq	r3, sl
   1211c:	03a02020 	moveq	r2, #32	; 0x20
   12120:	0a000001 	beq	1212c <vsprintf+0x26c>
   12124:	ea000005 	b	12140 <vsprintf+0x280>
   12128:	e5432001 	strb	r2, [r3, #-1]
   1212c:	e2499001 	sub	r9, r9, #1	; 0x1
   12130:	e3590000 	cmp	r9, #0	; 0x0
   12134:	e1a0a003 	mov	sl, r3
   12138:	e2833001 	add	r3, r3, #1	; 0x1
   1213c:	cafffff9 	bgt	12128 <vsprintf+0x268>
   12140:	e5963000 	ldr	r3, [r6]
   12144:	e1a0200a 	mov	r2, sl
   12148:	e4c23001 	strb	r3, [r2], #1
   1214c:	e3a03020 	mov	r3, #32	; 0x20
   12150:	ea000000 	b	12158 <vsprintf+0x298>
   12154:	e5423001 	strb	r3, [r2, #-1]
   12158:	e2499001 	sub	r9, r9, #1	; 0x1
   1215c:	e3590000 	cmp	r9, #0	; 0x0
   12160:	e1a0a002 	mov	sl, r2
   12164:	e2822001 	add	r2, r2, #1	; 0x1
   12168:	cafffff9 	bgt	12154 <vsprintf+0x294>
   1216c:	ea0000ab 	b	12420 <vsprintf+0x560>
   12170:	e58d7000 	str	r7, [sp]
   12174:	e1a0000a 	mov	r0, sl
   12178:	e5961000 	ldr	r1, [r6]
   1217c:	e1a02009 	mov	r2, r9
   12180:	e59d302c 	ldr	r3, [sp, #44]
   12184:	ebfffe40 	bl	11a8c <string>
   12188:	e1a0a000 	mov	sl, r0
   1218c:	ea0000a3 	b	12420 <vsprintf+0x560>
   12190:	e596b000 	ldr	fp, [r6]
   12194:	e35b0000 	cmp	fp, #0	; 0x0
   12198:	1a000005 	bne	121b4 <vsprintf+0x2f4>
   1219c:	e1a0000a 	mov	r0, sl
   121a0:	e1a02009 	mov	r2, r9
   121a4:	e59d302c 	ldr	r3, [sp, #44]
   121a8:	e59f13f0 	ldr	r1, [pc, #1008]	; 125a0 <vsprintf+0x6e0>
   121ac:	e58d7000 	str	r7, [sp]
   121b0:	ea000081 	b	123bc <vsprintf+0x4fc>
   121b4:	e5d83001 	ldrb	r3, [r8, #1]
   121b8:	e2882001 	add	r2, r8, #1	; 0x1
   121bc:	e353004d 	cmp	r3, #77	; 0x4d
   121c0:	0a000008 	beq	121e8 <vsprintf+0x328>
   121c4:	8a000002 	bhi	121d4 <vsprintf+0x314>
   121c8:	e3530049 	cmp	r3, #73	; 0x49
   121cc:	1a00007c 	bne	123c4 <vsprintf+0x504>
   121d0:	ea000025 	b	1226c <vsprintf+0x3ac>
   121d4:	e3530069 	cmp	r3, #105	; 0x69
   121d8:	0a000022 	beq	12268 <vsprintf+0x3a8>
   121dc:	e353006d 	cmp	r3, #109	; 0x6d
   121e0:	1a000077 	bne	123c4 <vsprintf+0x504>
   121e4:	e3877040 	orr	r7, r7, #64	; 0x40
   121e8:	e1a03327 	lsr	r3, r7, #6
   121ec:	e2233001 	eor	r3, r3, #1	; 0x1
   121f0:	e2034001 	and	r4, r3, #1	; 0x1
   121f4:	e3a0e000 	mov	lr, #0	; 0x0
   121f8:	e28dc05b 	add	ip, sp, #91	; 0x5b
   121fc:	e3a0503a 	mov	r5, #58	; 0x3a
   12200:	e7db200e 	ldrb	r2, [fp, lr]
   12204:	e59f0398 	ldr	r0, [pc, #920]	; 125a4 <vsprintf+0x6e4>
   12208:	e202300f 	and	r3, r2, #15	; 0xf
   1220c:	e7d02222 	ldrb	r2, [r0, r2, lsr #4]
   12210:	e7d03003 	ldrb	r3, [r0, r3]
   12214:	e1a0000c 	mov	r0, ip
   12218:	e4c02001 	strb	r2, [r0], #1
   1221c:	e35e0005 	cmp	lr, #5	; 0x5
   12220:	03a01000 	moveq	r1, #0	; 0x0
   12224:	12041001 	andne	r1, r4, #1	; 0x1
   12228:	e3510000 	cmp	r1, #0	; 0x0
   1222c:	e5cc3001 	strb	r3, [ip, #1]
   12230:	e28ee001 	add	lr, lr, #1	; 0x1
   12234:	e280c001 	add	ip, r0, #1	; 0x1
   12238:	128cc001 	addne	ip, ip, #1	; 0x1
   1223c:	15c05001 	strbne	r5, [r0, #1]
   12240:	e35e0006 	cmp	lr, #6	; 0x6
   12244:	1affffed 	bne	12200 <vsprintf+0x340>
   12248:	e3a03000 	mov	r3, #0	; 0x0
   1224c:	e5cc3000 	strb	r3, [ip]
   12250:	e1a0000a 	mov	r0, sl
   12254:	e3c7c040 	bic	ip, r7, #64	; 0x40
   12258:	e1a02009 	mov	r2, r9
   1225c:	e59d302c 	ldr	r3, [sp, #44]
   12260:	e28d105b 	add	r1, sp, #91	; 0x5b
   12264:	ea000053 	b	123b8 <vsprintf+0x4f8>
   12268:	e3877040 	orr	r7, r7, #64	; 0x40
   1226c:	e5d23001 	ldrb	r3, [r2, #1]
   12270:	e3530036 	cmp	r3, #54	; 0x36
   12274:	1a00002d 	bne	12330 <vsprintf+0x470>
   12278:	e1a03327 	lsr	r3, r7, #6
   1227c:	e2233001 	eor	r3, r3, #1	; 0x1
   12280:	e1a0500b 	mov	r5, fp
   12284:	e3a04000 	mov	r4, #0	; 0x0
   12288:	e203b001 	and	fp, r3, #1	; 0x1
   1228c:	e28dc033 	add	ip, sp, #51	; 0x33
   12290:	e5d52000 	ldrb	r2, [r5]
   12294:	e59fe308 	ldr	lr, [pc, #776]	; 125a4 <vsprintf+0x6e4>
   12298:	e202300f 	and	r3, r2, #15	; 0xf
   1229c:	e7de0003 	ldrb	r0, [lr, r3]
   122a0:	e7de3222 	ldrb	r3, [lr, r2, lsr #4]
   122a4:	e1a0100c 	mov	r1, ip
   122a8:	e4c13001 	strb	r3, [r1], #1
   122ac:	e5cc0001 	strb	r0, [ip, #1]
   122b0:	e5d52001 	ldrb	r2, [r5, #1]
   122b4:	e59f02e8 	ldr	r0, [pc, #744]	; 125a4 <vsprintf+0x6e4>
   122b8:	e202300f 	and	r3, r2, #15	; 0xf
   122bc:	e7d0c003 	ldrb	ip, [r0, r3]
   122c0:	e59f32dc 	ldr	r3, [pc, #732]	; 125a4 <vsprintf+0x6e4>
   122c4:	e3540007 	cmp	r4, #7	; 0x7
   122c8:	03a0e000 	moveq	lr, #0	; 0x0
   122cc:	120be001 	andne	lr, fp, #1	; 0x1
   122d0:	e7d32222 	ldrb	r2, [r3, r2, lsr #4]
   122d4:	e35e0000 	cmp	lr, #0	; 0x0
   122d8:	e58d2014 	str	r2, [sp, #20]
   122dc:	e2810001 	add	r0, r1, #1	; 0x1
   122e0:	e59de014 	ldr	lr, [sp, #20]
   122e4:	e2802001 	add	r2, r0, #1	; 0x1
   122e8:	e5c1e001 	strb	lr, [r1, #1]
   122ec:	e2844001 	add	r4, r4, #1	; 0x1
   122f0:	e5c0c001 	strb	ip, [r0, #1]
   122f4:	13a0003a 	movne	r0, #58	; 0x3a
   122f8:	e282c001 	add	ip, r2, #1	; 0x1
   122fc:	128cc001 	addne	ip, ip, #1	; 0x1
   12300:	15c20001 	strbne	r0, [r2, #1]
   12304:	e3540008 	cmp	r4, #8	; 0x8
   12308:	e2855002 	add	r5, r5, #2	; 0x2
   1230c:	1affffdf 	bne	12290 <vsprintf+0x3d0>
   12310:	e3a03000 	mov	r3, #0	; 0x0
   12314:	e5cc3000 	strb	r3, [ip]
   12318:	e1a0000a 	mov	r0, sl
   1231c:	e3c7c040 	bic	ip, r7, #64	; 0x40
   12320:	e1a02009 	mov	r2, r9
   12324:	e59d302c 	ldr	r3, [sp, #44]
   12328:	e28d1033 	add	r1, sp, #51	; 0x33
   1232c:	ea000021 	b	123b8 <vsprintf+0x4f8>
   12330:	e3530034 	cmp	r3, #52	; 0x34
   12334:	13c77040 	bicne	r7, r7, #64	; 0x40
   12338:	1a000021 	bne	123c4 <vsprintf+0x504>
   1233c:	e3a05000 	mov	r5, #0	; 0x0
   12340:	e28d406d 	add	r4, sp, #109	; 0x6d
   12344:	e7db1005 	ldrb	r1, [fp, r5]
   12348:	e28d007d 	add	r0, sp, #125	; 0x7d
   1234c:	ebfffd9b 	bl	119c0 <put_dec_trunc>
   12350:	e59d101c 	ldr	r1, [sp, #28]
   12354:	e2402001 	sub	r2, r0, #1	; 0x1
   12358:	e0610000 	rsb	r0, r1, r0
   1235c:	e3a01000 	mov	r1, #0	; 0x0
   12360:	ea000002 	b	12370 <vsprintf+0x4b0>
   12364:	e5d23001 	ldrb	r3, [r2, #1]
   12368:	e7c43001 	strb	r3, [r4, r1]
   1236c:	e2811001 	add	r1, r1, #1	; 0x1
   12370:	e59d3018 	ldr	r3, [sp, #24]
   12374:	e2422001 	sub	r2, r2, #1	; 0x1
   12378:	e1520003 	cmp	r2, r3
   1237c:	1afffff8 	bne	12364 <vsprintf+0x4a4>
   12380:	e3550003 	cmp	r5, #3	; 0x3
   12384:	13a0e02e 	movne	lr, #46	; 0x2e
   12388:	e0844000 	add	r4, r4, r0
   1238c:	e2855001 	add	r5, r5, #1	; 0x1
   12390:	14c4e001 	strbne	lr, [r4], #1
   12394:	e3550004 	cmp	r5, #4	; 0x4
   12398:	1affffe9 	bne	12344 <vsprintf+0x484>
   1239c:	e3a03000 	mov	r3, #0	; 0x0
   123a0:	e5c43000 	strb	r3, [r4]
   123a4:	e59d302c 	ldr	r3, [sp, #44]
   123a8:	e3c7c040 	bic	ip, r7, #64	; 0x40
   123ac:	e1a0000a 	mov	r0, sl
   123b0:	e1a02009 	mov	r2, r9
   123b4:	e28d106d 	add	r1, sp, #109	; 0x6d
   123b8:	e58dc000 	str	ip, [sp]
   123bc:	ebfffdb2 	bl	11a8c <string>
   123c0:	ea00000d 	b	123fc <vsprintf+0x53c>
   123c4:	e3a01010 	mov	r1, #16	; 0x10
   123c8:	e3790001 	cmn	r9, #1	; 0x1
   123cc:	e58d1000 	str	r1, [sp]
   123d0:	e59d102c 	ldr	r1, [sp, #44]
   123d4:	e387c020 	orr	ip, r7, #32	; 0x20
   123d8:	02899009 	addeq	r9, r9, #9	; 0x9
   123dc:	0387c021 	orreq	ip, r7, #33	; 0x21
   123e0:	e1a0000a 	mov	r0, sl
   123e4:	e1a0200b 	mov	r2, fp
   123e8:	e3a03000 	mov	r3, #0	; 0x0
   123ec:	e58d9004 	str	r9, [sp, #4]
   123f0:	e58d1008 	str	r1, [sp, #8]
   123f4:	e58dc00c 	str	ip, [sp, #12]
   123f8:	ebfffe1a 	bl	11c68 <number>
   123fc:	e1a0a000 	mov	sl, r0
   12400:	e1a02008 	mov	r2, r8
   12404:	e5d23001 	ldrb	r3, [r2, #1]
   12408:	e59fe198 	ldr	lr, [pc, #408]	; 125a8 <vsprintf+0x6e8>
   1240c:	e1a08002 	mov	r8, r2
   12410:	e7de3003 	ldrb	r3, [lr, r3]
   12414:	e2822001 	add	r2, r2, #1	; 0x1
   12418:	e3130007 	tst	r3, #7	; 0x7
   1241c:	1afffff8 	bne	12404 <vsprintf+0x544>
   12420:	e2866004 	add	r6, r6, #4	; 0x4
   12424:	ea000054 	b	1257c <vsprintf+0x6bc>
   12428:	e59d0028 	ldr	r0, [sp, #40]
   1242c:	e5963000 	ldr	r3, [r6]
   12430:	e2861004 	add	r1, r6, #4	; 0x4
   12434:	e060200a 	rsb	r2, r0, sl
   12438:	e1a06001 	mov	r6, r1
   1243c:	e5832000 	str	r2, [r3]
   12440:	ea00004d 	b	1257c <vsprintf+0x6bc>
   12444:	e3a0c008 	mov	ip, #8	; 0x8
   12448:	ea00000d 	b	12484 <vsprintf+0x5c4>
   1244c:	e3877020 	orr	r7, r7, #32	; 0x20
   12450:	e3a0c010 	mov	ip, #16	; 0x10
   12454:	ea00000a 	b	12484 <vsprintf+0x5c4>
   12458:	e3877002 	orr	r7, r7, #2	; 0x2
   1245c:	ea000007 	b	12480 <vsprintf+0x5c0>
   12460:	e3a03025 	mov	r3, #37	; 0x25
   12464:	e4ca3001 	strb	r3, [sl], #1
   12468:	e5d83000 	ldrb	r3, [r8]
   1246c:	e3530000 	cmp	r3, #0	; 0x0
   12470:	02488001 	subeq	r8, r8, #1	; 0x1
   12474:	0a000040 	beq	1257c <vsprintf+0x6bc>
   12478:	e4ca3001 	strb	r3, [sl], #1
   1247c:	ea00003e 	b	1257c <vsprintf+0x6bc>
   12480:	e3a0c00a 	mov	ip, #10	; 0xa
   12484:	e352004c 	cmp	r2, #76	; 0x4c
   12488:	1a000009 	bne	124b4 <vsprintf+0x5f4>
   1248c:	e59d1024 	ldr	r1, [sp, #36]
   12490:	e3a02000 	mov	r2, #0	; 0x0
   12494:	e0613006 	rsb	r3, r1, r6
   12498:	e3130007 	tst	r3, #7	; 0x7
   1249c:	02866004 	addeq	r6, r6, #4	; 0x4
   124a0:	e896000a 	ldm	r6, {r1, r3}
   124a4:	e1a05003 	mov	r5, r3
   124a8:	e1824001 	orr	r4, r2, r1
   124ac:	e2866008 	add	r6, r6, #8	; 0x8
   124b0:	ea000028 	b	12558 <vsprintf+0x698>
   124b4:	e352006c 	cmp	r2, #108	; 0x6c
   124b8:	1a000006 	bne	124d8 <vsprintf+0x618>
   124bc:	e5963000 	ldr	r3, [r6]
   124c0:	e3170002 	tst	r7, #2	; 0x2
   124c4:	e1a04003 	mov	r4, r3
   124c8:	e3a05000 	mov	r5, #0	; 0x0
   124cc:	e2866004 	add	r6, r6, #4	; 0x4
   124d0:	0a000020 	beq	12558 <vsprintf+0x698>
   124d4:	ea00001e 	b	12554 <vsprintf+0x694>
   124d8:	e352007a 	cmp	r2, #122	; 0x7a
   124dc:	1352005a 	cmpne	r2, #90	; 0x5a
   124e0:	05963000 	ldreq	r3, [r6]
   124e4:	02866004 	addeq	r6, r6, #4	; 0x4
   124e8:	01a04003 	moveq	r4, r3
   124ec:	03a05000 	moveq	r5, #0	; 0x0
   124f0:	0a000018 	beq	12558 <vsprintf+0x698>
   124f4:	e3520074 	cmp	r2, #116	; 0x74
   124f8:	05963000 	ldreq	r3, [r6]
   124fc:	02866004 	addeq	r6, r6, #4	; 0x4
   12500:	0a00000b 	beq	12534 <vsprintf+0x674>
   12504:	e3520068 	cmp	r2, #104	; 0x68
   12508:	e2861004 	add	r1, r6, #4	; 0x4
   1250c:	e5962000 	ldr	r2, [r6]
   12510:	e2070002 	and	r0, r7, #2	; 0x2
   12514:	1a000009 	bne	12540 <vsprintf+0x680>
   12518:	e6ff3072 	uxth	r3, r2
   1251c:	e3500000 	cmp	r0, #0	; 0x0
   12520:	e1a06001 	mov	r6, r1
   12524:	e1a04003 	mov	r4, r3
   12528:	e3a05000 	mov	r5, #0	; 0x0
   1252c:	0a000009 	beq	12558 <vsprintf+0x698>
   12530:	e6bf3074 	sxth	r3, r4
   12534:	e1a04003 	mov	r4, r3
   12538:	e1a05fc4 	asr	r5, r4, #31
   1253c:	ea000005 	b	12558 <vsprintf+0x698>
   12540:	e3500000 	cmp	r0, #0	; 0x0
   12544:	e1a06001 	mov	r6, r1
   12548:	e1a04002 	mov	r4, r2
   1254c:	e3a05000 	mov	r5, #0	; 0x0
   12550:	0a000000 	beq	12558 <vsprintf+0x698>
   12554:	e1a05fc4 	asr	r5, r4, #31
   12558:	e59de02c 	ldr	lr, [sp, #44]
   1255c:	e1a0000a 	mov	r0, sl
   12560:	e1a02004 	mov	r2, r4
   12564:	e1a03005 	mov	r3, r5
   12568:	e58dc000 	str	ip, [sp]
   1256c:	e98d4200 	stmib	sp, {r9, lr}
   12570:	e58d700c 	str	r7, [sp, #12]
   12574:	ebfffdbb 	bl	11c68 <number>
   12578:	e1a0a000 	mov	sl, r0
   1257c:	e2888001 	add	r8, r8, #1	; 0x1
   12580:	e5d83000 	ldrb	r3, [r8]
   12584:	e3530000 	cmp	r3, #0	; 0x0
   12588:	1afffe58 	bne	11ef0 <vsprintf+0x30>
   1258c:	e5ca3000 	strb	r3, [sl]
   12590:	e59d1028 	ldr	r1, [sp, #40]
   12594:	e061000a 	rsb	r0, r1, sl
   12598:	e28dd084 	add	sp, sp, #132	; 0x84
   1259c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   125a0:	00018c2e 	.word	0x00018c2e
   125a4:	00015fe2 	.word	0x00015fe2
   125a8:	00016003 	.word	0x00016003

000125ac <sprintf>:
   125ac:	e92d000e 	push	{r1, r2, r3}
   125b0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   125b4:	e59d1004 	ldr	r1, [sp, #4]
   125b8:	e28d2008 	add	r2, sp, #8	; 0x8
   125bc:	ebfffe3f 	bl	11ec0 <vsprintf>
   125c0:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
   125c4:	e28dd00c 	add	sp, sp, #12	; 0xc
   125c8:	e12fff1e 	bx	lr

000125cc <ius_check_header>:
   125cc:	e92d4070 	push	{r4, r5, r6, lr}
   125d0:	e5901000 	ldr	r1, [r0]
   125d4:	e59f30e8 	ldr	r3, [pc, #232]	; 126c4 <ius_check_header+0xf8>
   125d8:	e24dd008 	sub	sp, sp, #8	; 0x8
   125dc:	e1510003 	cmp	r1, r3
   125e0:	e1a04000 	mov	r4, r0
   125e4:	0a000005 	beq	12600 <ius_check_header+0x34>
   125e8:	e59f00d8 	ldr	r0, [pc, #216]	; 126c8 <ius_check_header+0xfc>
   125ec:	e59f30d8 	ldr	r3, [pc, #216]	; 126cc <ius_check_header+0x100>
   125f0:	e1a0e00f 	mov	lr, pc
   125f4:	e593f000 	ldr	pc, [r3]
   125f8:	e3e00000 	mvn	r0, #0	; 0x0
   125fc:	ea00002e 	b	126bc <ius_check_header+0xf0>
   12600:	ebffd377 	bl	73e4 <cdbg_verify_burn>
   12604:	e3500000 	cmp	r0, #0	; 0x0
   12608:	0a000011 	beq	12654 <ius_check_header+0x88>
   1260c:	e5943004 	ldr	r3, [r4, #4]
   12610:	e3130002 	tst	r3, #2	; 0x2
   12614:	1a000005 	bne	12630 <ius_check_header+0x64>
   12618:	e59f00b0 	ldr	r0, [pc, #176]	; 126d0 <ius_check_header+0x104>
   1261c:	e59f30a8 	ldr	r3, [pc, #168]	; 126cc <ius_check_header+0x100>
   12620:	e1a0e00f 	mov	lr, pc
   12624:	e593f000 	ldr	pc, [r3]
   12628:	e3e00021 	mvn	r0, #33	; 0x21
   1262c:	ea000022 	b	126bc <ius_check_header+0xf0>
   12630:	ebffd36f 	bl	73f4 <cdbg_verify_burn_key>
   12634:	e59f2098 	ldr	r2, [pc, #152]	; 126d4 <ius_check_header+0x108>
   12638:	e1a01000 	mov	r1, r0
   1263c:	e3a00000 	mov	r0, #0	; 0x0
   12640:	ebffd006 	bl	6660 <ss_aes_init>
   12644:	e2840010 	add	r0, r4, #16	; 0x10
   12648:	e1a01000 	mov	r1, r0
   1264c:	e3a02e1f 	mov	r2, #496	; 0x1f0
   12650:	ebffcff1 	bl	661c <ss_aes_data>
   12654:	e3a06000 	mov	r6, #0	; 0x0
   12658:	e5945018 	ldr	r5, [r4, #24]
   1265c:	e3a01c02 	mov	r1, #512	; 0x200
   12660:	e5846018 	str	r6, [r4, #24]
   12664:	e1a00006 	mov	r0, r6
   12668:	ebffd11a 	bl	6ad8 <hash_init>
   1266c:	e3a01c02 	mov	r1, #512	; 0x200
   12670:	e1a00004 	mov	r0, r4
   12674:	ebffd0f2 	bl	6a44 <hash_data>
   12678:	e28d0004 	add	r0, sp, #4	; 0x4
   1267c:	ebffd0d4 	bl	69d4 <hash_value>
   12680:	e59d2004 	ldr	r2, [sp, #4]
   12684:	e59f3040 	ldr	r3, [pc, #64]	; 126cc <ius_check_header+0x100>
   12688:	e1550002 	cmp	r5, r2
   1268c:	e5845018 	str	r5, [r4, #24]
   12690:	0a000005 	beq	126ac <ius_check_header+0xe0>
   12694:	e59f003c 	ldr	r0, [pc, #60]	; 126d8 <ius_check_header+0x10c>
   12698:	e1a01005 	mov	r1, r5
   1269c:	e1a0e00f 	mov	lr, pc
   126a0:	e593f000 	ldr	pc, [r3]
   126a4:	e3e0001f 	mvn	r0, #31	; 0x1f
   126a8:	ea000003 	b	126bc <ius_check_header+0xf0>
   126ac:	e59f0028 	ldr	r0, [pc, #40]	; 126dc <ius_check_header+0x110>
   126b0:	e1a0e00f 	mov	lr, pc
   126b4:	e593f000 	ldr	pc, [r3]
   126b8:	e1a00006 	mov	r0, r6
   126bc:	e28dd008 	add	sp, sp, #8	; 0x8
   126c0:	e8bd8070 	pop	{r4, r5, r6, pc}
   126c4:	7375692b 	.word	0x7375692b
   126c8:	00018c35 	.word	0x00018c35
   126cc:	3c00d6cc 	.word	0x3c00d6cc
   126d0:	00018c4f 	.word	0x00018c4f
   126d4:	3c00d08c 	.word	0x3c00d08c
   126d8:	00018c5e 	.word	0x00018c5e
   126dc:	00018c86 	.word	0x00018c86

000126e0 <printf_off>:
   126e0:	e92d000f 	push	{r0, r1, r2, r3}
   126e4:	e28dd010 	add	sp, sp, #16	; 0x10
   126e8:	e12fff1e 	bx	lr

000126ec <disable_ctrlc>:
   126ec:	e59f300c 	ldr	r3, [pc, #12]	; 12700 <disable_ctrlc+0x14>
   126f0:	e5932000 	ldr	r2, [r3]
   126f4:	e5830000 	str	r0, [r3]
   126f8:	e1a00002 	mov	r0, r2
   126fc:	e12fff1e 	bx	lr
   12700:	3c00d26c 	.word	0x3c00d26c

00012704 <had_ctrlc>:
   12704:	e59f3004 	ldr	r3, [pc, #4]	; 12710 <had_ctrlc+0xc>
   12708:	e5930000 	ldr	r0, [r3]
   1270c:	e12fff1e 	bx	lr
   12710:	3c00d268 	.word	0x3c00d268

00012714 <clear_ctrlc>:
   12714:	e59f3008 	ldr	r3, [pc, #8]	; 12724 <clear_ctrlc+0x10>
   12718:	e3a02000 	mov	r2, #0	; 0x0
   1271c:	e5832000 	str	r2, [r3]
   12720:	e12fff1e 	bx	lr
   12724:	3c00d268 	.word	0x3c00d268

00012728 <dbg>:
   12728:	e92d000f 	push	{r0, r1, r2, r3}
   1272c:	e28dd010 	add	sp, sp, #16	; 0x10
   12730:	e12fff1e 	bx	lr

00012734 <puts>:
   12734:	eaffec4c 	b	d86c <serial_puts>

00012738 <vprintf>:
   12738:	e92d4010 	push	{r4, lr}
   1273c:	e24ddd06 	sub	sp, sp, #384	; 0x180
   12740:	e1a02001 	mov	r2, r1
   12744:	e1a01000 	mov	r1, r0
   12748:	e1a0000d 	mov	r0, sp
   1274c:	ebfffddb 	bl	11ec0 <vsprintf>
   12750:	e1a0000d 	mov	r0, sp
   12754:	e1a0400d 	mov	r4, sp
   12758:	ebffec43 	bl	d86c <serial_puts>
   1275c:	e28ddd06 	add	sp, sp, #384	; 0x180
   12760:	e8bd8010 	pop	{r4, pc}

00012764 <printf_on>:
   12764:	e92d000f 	push	{r0, r1, r2, r3}
   12768:	e92d4010 	push	{r4, lr}
   1276c:	e24ddd06 	sub	sp, sp, #384	; 0x180
   12770:	e1a0000d 	mov	r0, sp
   12774:	e59d1188 	ldr	r1, [sp, #392]
   12778:	e28d2f63 	add	r2, sp, #396	; 0x18c
   1277c:	ebfffdcf 	bl	11ec0 <vsprintf>
   12780:	e1a0000d 	mov	r0, sp
   12784:	e1a0400d 	mov	r4, sp
   12788:	ebffec37 	bl	d86c <serial_puts>
   1278c:	e28ddd06 	add	sp, sp, #384	; 0x180
   12790:	e8bd4010 	pop	{r4, lr}
   12794:	e28dd010 	add	sp, sp, #16	; 0x10
   12798:	e12fff1e 	bx	lr

0001279c <serial_printf>:
   1279c:	e92d000f 	push	{r0, r1, r2, r3}
   127a0:	e92d4010 	push	{r4, lr}
   127a4:	e24ddd06 	sub	sp, sp, #384	; 0x180
   127a8:	e1a0000d 	mov	r0, sp
   127ac:	e59d1188 	ldr	r1, [sp, #392]
   127b0:	e28d2f63 	add	r2, sp, #396	; 0x18c
   127b4:	ebfffdc1 	bl	11ec0 <vsprintf>
   127b8:	e1a0000d 	mov	r0, sp
   127bc:	e1a0400d 	mov	r4, sp
   127c0:	ebffec29 	bl	d86c <serial_puts>
   127c4:	e28ddd06 	add	sp, sp, #384	; 0x180
   127c8:	e8bd4010 	pop	{r4, lr}
   127cc:	e28dd010 	add	sp, sp, #16	; 0x10
   127d0:	e12fff1e 	bx	lr

000127d4 <putc>:
   127d4:	eaffec0d 	b	d810 <serial_putc>

000127d8 <tstc>:
   127d8:	eaffec05 	b	d7f4 <serial_tstc>

000127dc <getc>:
   127dc:	eaffec33 	b	d8b0 <serial_getc>

000127e0 <ctrlc>:
   127e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   127e4:	e59f3040 	ldr	r3, [pc, #64]	; 1282c <ctrlc+0x4c>
   127e8:	e24dd004 	sub	sp, sp, #4	; 0x4
   127ec:	e5933000 	ldr	r3, [r3]
   127f0:	e3530000 	cmp	r3, #0	; 0x0
   127f4:	1a000009 	bne	12820 <ctrlc+0x40>
   127f8:	ebffebfd 	bl	d7f4 <serial_tstc>
   127fc:	e3500000 	cmp	r0, #0	; 0x0
   12800:	0a000006 	beq	12820 <ctrlc+0x40>
   12804:	ebffec29 	bl	d8b0 <serial_getc>
   12808:	e3500003 	cmp	r0, #3	; 0x3
   1280c:	059f301c 	ldreq	r3, [pc, #28]	; 12830 <ctrlc+0x50>
   12810:	03a02001 	moveq	r2, #1	; 0x1
   12814:	05832000 	streq	r2, [r3]
   12818:	01a00002 	moveq	r0, r2
   1281c:	0a000000 	beq	12824 <ctrlc+0x44>
   12820:	e3a00000 	mov	r0, #0	; 0x0
   12824:	e28dd004 	add	sp, sp, #4	; 0x4
   12828:	e8bd8000 	pop	{pc}
   1282c:	3c00d26c 	.word	0x3c00d26c
   12830:	3c00d268 	.word	0x3c00d268

00012834 <parse_line>:
   12834:	e92d4010 	push	{r4, lr}
   12838:	e3a04000 	mov	r4, #0	; 0x0
   1283c:	e1a02004 	mov	r2, r4
   12840:	ea000000 	b	12848 <parse_line+0x14>
   12844:	e2800001 	add	r0, r0, #1	; 0x1
   12848:	e5d03000 	ldrb	r3, [r0]
   1284c:	e3530009 	cmp	r3, #9	; 0x9
   12850:	13530020 	cmpne	r3, #32	; 0x20
   12854:	0afffffa 	beq	12844 <parse_line+0x10>
   12858:	e3530000 	cmp	r3, #0	; 0x0
   1285c:	0a00000c 	beq	12894 <parse_line+0x60>
   12860:	e7810104 	str	r0, [r1, r4, lsl #2]
   12864:	e2844001 	add	r4, r4, #1	; 0x1
   12868:	ea000000 	b	12870 <parse_line+0x3c>
   1286c:	e2800001 	add	r0, r0, #1	; 0x1
   12870:	e5d03000 	ldrb	r3, [r0]
   12874:	e3530000 	cmp	r3, #0	; 0x0
   12878:	13530020 	cmpne	r3, #32	; 0x20
   1287c:	0a000002 	beq	1288c <parse_line+0x58>
   12880:	e3530009 	cmp	r3, #9	; 0x9
   12884:	1afffff8 	bne	1286c <parse_line+0x38>
   12888:	ea000003 	b	1289c <parse_line+0x68>
   1288c:	e3530000 	cmp	r3, #0	; 0x0
   12890:	1a000001 	bne	1289c <parse_line+0x68>
   12894:	e7813104 	str	r3, [r1, r4, lsl #2]
   12898:	ea000007 	b	128bc <parse_line+0x88>
   1289c:	e3540010 	cmp	r4, #16	; 0x10
   128a0:	e5c02000 	strb	r2, [r0]
   128a4:	1affffe6 	bne	12844 <parse_line+0x10>
   128a8:	e59f0014 	ldr	r0, [pc, #20]	; 128c4 <parse_line+0x90>
   128ac:	e1a01004 	mov	r1, r4
   128b0:	e59f3010 	ldr	r3, [pc, #16]	; 128c8 <parse_line+0x94>
   128b4:	e1a0e00f 	mov	lr, pc
   128b8:	e593f000 	ldr	pc, [r3]
   128bc:	e1a00004 	mov	r0, r4
   128c0:	e8bd8010 	pop	{r4, pc}
   128c4:	00018c9c 	.word	0x00018c9c
   128c8:	3c00d6cc 	.word	0x3c00d6cc

000128cc <run_command>:
   128cc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   128d0:	e1a05000 	mov	r5, r0
   128d4:	e24ddf93 	sub	sp, sp, #588	; 0x24c
   128d8:	e1a0a001 	mov	sl, r1
   128dc:	ebffff8c 	bl	12714 <clear_ctrlc>
   128e0:	e3550000 	cmp	r5, #0	; 0x0
   128e4:	0a0000ac 	beq	12b9c <run_command+0x2d0>
   128e8:	e5d53000 	ldrb	r3, [r5]
   128ec:	e3530000 	cmp	r3, #0	; 0x0
   128f0:	0a0000a9 	beq	12b9c <run_command+0x2d0>
   128f4:	e1a00005 	mov	r0, r5
   128f8:	ebffbafb 	bl	14ec <strlen>
   128fc:	e35000ff 	cmp	r0, #255	; 0xff
   12900:	9a000002 	bls	12910 <run_command+0x44>
   12904:	e59f02a0 	ldr	r0, [pc, #672]	; 12bac <run_command+0x2e0>
   12908:	ebffff89 	bl	12734 <puts>
   1290c:	ea0000a2 	b	12b9c <run_command+0x2d0>
   12910:	e28d4f41 	add	r4, sp, #260	; 0x104
   12914:	e1a01005 	mov	r1, r5
   12918:	e1a00004 	mov	r0, r4
   1291c:	e3a06000 	mov	r6, #0	; 0x0
   12920:	ebffba9a 	bl	1390 <strcpy>
   12924:	e3a07001 	mov	r7, #1	; 0x1
   12928:	e1a0b006 	mov	fp, r6
   1292c:	e28d9f81 	add	r9, sp, #516	; 0x204
   12930:	ea000091 	b	12b7c <run_command+0x2b0>
   12934:	e3510027 	cmp	r1, #39	; 0x27
   12938:	1a000002 	bne	12948 <run_command+0x7c>
   1293c:	e5523001 	ldrb	r3, [r2, #-1]
   12940:	e353005c 	cmp	r3, #92	; 0x5c
   12944:	12200001 	eorne	r0, r0, #1	; 0x1
   12948:	e3500000 	cmp	r0, #0	; 0x0
   1294c:	1a000009 	bne	12978 <run_command+0xac>
   12950:	e351003b 	cmp	r1, #59	; 0x3b
   12954:	13a03000 	movne	r3, #0	; 0x0
   12958:	03a03001 	moveq	r3, #1	; 0x1
   1295c:	e1520004 	cmp	r2, r4
   12960:	03a03000 	moveq	r3, #0	; 0x0
   12964:	e3530000 	cmp	r3, #0	; 0x0
   12968:	0a000002 	beq	12978 <run_command+0xac>
   1296c:	e5523001 	ldrb	r3, [r2, #-1]
   12970:	e353005c 	cmp	r3, #92	; 0x5c
   12974:	1a000005 	bne	12990 <run_command+0xc4>
   12978:	e2822001 	add	r2, r2, #1	; 0x1
   1297c:	e5d21000 	ldrb	r1, [r2]
   12980:	e3510000 	cmp	r1, #0	; 0x0
   12984:	1affffea 	bne	12934 <run_command+0x68>
   12988:	e1a08002 	mov	r8, r2
   1298c:	ea000001 	b	12998 <run_command+0xcc>
   12990:	e5c20000 	strb	r0, [r2]
   12994:	e2828001 	add	r8, r2, #1	; 0x1
   12998:	e1a00004 	mov	r0, r4
   1299c:	ebffbad2 	bl	14ec <strlen>
   129a0:	e3a0c000 	mov	ip, #0	; 0x0
   129a4:	e3a0ec01 	mov	lr, #256	; 0x100
   129a8:	e1a0100c 	mov	r1, ip
   129ac:	e28d5004 	add	r5, sp, #4	; 0x4
   129b0:	ea000049 	b	12adc <run_command+0x210>
   129b4:	e35c0003 	cmp	ip, #3	; 0x3
   129b8:	e2400001 	sub	r0, r0, #1	; 0x1
   129bc:	e4d42001 	ldrb	r2, [r4], #1
   129c0:	0a000035 	beq	12a9c <run_command+0x1d0>
   129c4:	e352005c 	cmp	r2, #92	; 0x5c
   129c8:	13a03000 	movne	r3, #0	; 0x0
   129cc:	03a03001 	moveq	r3, #1	; 0x1
   129d0:	e351005c 	cmp	r1, #92	; 0x5c
   129d4:	03a03000 	moveq	r3, #0	; 0x0
   129d8:	e3530000 	cmp	r3, #0	; 0x0
   129dc:	0a000004 	beq	129f4 <run_command+0x128>
   129e0:	e3500000 	cmp	r0, #0	; 0x0
   129e4:	0a00003f 	beq	12ae8 <run_command+0x21c>
   129e8:	e4d42001 	ldrb	r2, [r4], #1
   129ec:	e2400001 	sub	r0, r0, #1	; 0x1
   129f0:	e3a0105c 	mov	r1, #92	; 0x5c
   129f4:	e35c0003 	cmp	ip, #3	; 0x3
   129f8:	979ff10c 	ldrls	pc, [pc, ip, lsl #2]
   129fc:	ea000035 	b	12ad8 <run_command+0x20c>
   12a00:	00012a10 	.word	0x00012a10
   12a04:	00012a54 	.word	0x00012a54
   12a08:	00012a8c 	.word	0x00012a8c
   12a0c:	00012a9c 	.word	0x00012a9c
   12a10:	e251105c 	subs	r1, r1, #92	; 0x5c
   12a14:	13a01001 	movne	r1, #1	; 0x1
   12a18:	e3520027 	cmp	r2, #39	; 0x27
   12a1c:	13a03000 	movne	r3, #0	; 0x0
   12a20:	02013001 	andeq	r3, r1, #1	; 0x1
   12a24:	e3530000 	cmp	r3, #0	; 0x0
   12a28:	13a0c003 	movne	ip, #3	; 0x3
   12a2c:	1a000022 	bne	12abc <run_command+0x1f0>
   12a30:	e3520024 	cmp	r2, #36	; 0x24
   12a34:	13a03000 	movne	r3, #0	; 0x0
   12a38:	02013001 	andeq	r3, r1, #1	; 0x1
   12a3c:	e3530000 	cmp	r3, #0	; 0x0
   12a40:	13a0c001 	movne	ip, #1	; 0x1
   12a44:	13a02024 	movne	r2, #36	; 0x24
   12a48:	04c52001 	strbeq	r2, [r5], #1
   12a4c:	024ee001 	subeq	lr, lr, #1	; 0x1
   12a50:	ea000020 	b	12ad8 <run_command+0x20c>
   12a54:	e352007b 	cmp	r2, #123	; 0x7b
   12a58:	13520028 	cmpne	r2, #40	; 0x28
   12a5c:	13a03000 	movne	r3, #0	; 0x0
   12a60:	03a03001 	moveq	r3, #1	; 0x1
   12a64:	03a0c002 	moveq	ip, #2	; 0x2
   12a68:	0a00001a 	beq	12ad8 <run_command+0x20c>
   12a6c:	e3a01024 	mov	r1, #36	; 0x24
   12a70:	e25ee001 	subs	lr, lr, #1	; 0x1
   12a74:	e4c51001 	strb	r1, [r5], #1
   12a78:	0a000015 	beq	12ad4 <run_command+0x208>
   12a7c:	e4c52001 	strb	r2, [r5], #1
   12a80:	e24ee001 	sub	lr, lr, #1	; 0x1
   12a84:	e1a0c003 	mov	ip, r3
   12a88:	ea000012 	b	12ad8 <run_command+0x20c>
   12a8c:	e352007d 	cmp	r2, #125	; 0x7d
   12a90:	13520029 	cmpne	r2, #41	; 0x29
   12a94:	0a00000e 	beq	12ad4 <run_command+0x208>
   12a98:	ea00000e 	b	12ad8 <run_command+0x20c>
   12a9c:	e3520027 	cmp	r2, #39	; 0x27
   12aa0:	13a03000 	movne	r3, #0	; 0x0
   12aa4:	03a03001 	moveq	r3, #1	; 0x1
   12aa8:	e351005c 	cmp	r1, #92	; 0x5c
   12aac:	03a03000 	moveq	r3, #0	; 0x0
   12ab0:	e3530000 	cmp	r3, #0	; 0x0
   12ab4:	0a000002 	beq	12ac4 <run_command+0x1f8>
   12ab8:	e3a0c000 	mov	ip, #0	; 0x0
   12abc:	e3a02027 	mov	r2, #39	; 0x27
   12ac0:	ea000004 	b	12ad8 <run_command+0x20c>
   12ac4:	e4c52001 	strb	r2, [r5], #1
   12ac8:	e24ee001 	sub	lr, lr, #1	; 0x1
   12acc:	e3a0c003 	mov	ip, #3	; 0x3
   12ad0:	ea000000 	b	12ad8 <run_command+0x20c>
   12ad4:	e3a0c000 	mov	ip, #0	; 0x0
   12ad8:	e1a01002 	mov	r1, r2
   12adc:	e3500000 	cmp	r0, #0	; 0x0
   12ae0:	135e0000 	cmpne	lr, #0	; 0x0
   12ae4:	1affffb2 	bne	129b4 <run_command+0xe8>
   12ae8:	e35e0000 	cmp	lr, #0	; 0x0
   12aec:	15c5b000 	strbne	fp, [r5]
   12af0:	0545e001 	strbeq	lr, [r5, #-1]
   12af4:	e28d0004 	add	r0, sp, #4	; 0x4
   12af8:	e1a01009 	mov	r1, r9
   12afc:	ebffff4c 	bl	12834 <parse_line>
   12b00:	e2505000 	subs	r5, r0, #0	; 0x0
   12b04:	0a00000d 	beq	12b40 <run_command+0x274>
   12b08:	e59d0204 	ldr	r0, [sp, #516]
   12b0c:	ebffc129 	bl	2fb8 <find_cmd>
   12b10:	e2504000 	subs	r4, r0, #0	; 0x0
   12b14:	1a000005 	bne	12b30 <run_command+0x264>
   12b18:	e59f0090 	ldr	r0, [pc, #144]	; 12bb0 <run_command+0x2e4>
   12b1c:	e59d1204 	ldr	r1, [sp, #516]
   12b20:	e59f308c 	ldr	r3, [pc, #140]	; 12bb4 <run_command+0x2e8>
   12b24:	e1a0e00f 	mov	lr, pc
   12b28:	e593f000 	ldr	pc, [r3]
   12b2c:	ea000003 	b	12b40 <run_command+0x274>
   12b30:	e5943004 	ldr	r3, [r4, #4]
   12b34:	e1550003 	cmp	r5, r3
   12b38:	da000002 	ble	12b48 <run_command+0x27c>
   12b3c:	ebffc0d7 	bl	2ea0 <cmd_usage>
   12b40:	e3e06000 	mvn	r6, #0	; 0x0
   12b44:	ea00000b 	b	12b78 <run_command+0x2ac>
   12b48:	e1a02005 	mov	r2, r5
   12b4c:	e1a0100a 	mov	r1, sl
   12b50:	e1a03009 	mov	r3, r9
   12b54:	e1a0e00f 	mov	lr, pc
   12b58:	e594f00c 	ldr	pc, [r4, #12]
   12b5c:	e3500000 	cmp	r0, #0	; 0x0
   12b60:	13e06000 	mvnne	r6, #0	; 0x0
   12b64:	e5944008 	ldr	r4, [r4, #8]
   12b68:	ebfffee5 	bl	12704 <had_ctrlc>
   12b6c:	e3500000 	cmp	r0, #0	; 0x0
   12b70:	1a000009 	bne	12b9c <run_command+0x2d0>
   12b74:	e0077004 	and	r7, r7, r4
   12b78:	e1a04008 	mov	r4, r8
   12b7c:	e5d43000 	ldrb	r3, [r4]
   12b80:	e3530000 	cmp	r3, #0	; 0x0
   12b84:	11a02004 	movne	r2, r4
   12b88:	13a00000 	movne	r0, #0	; 0x0
   12b8c:	1affff7a 	bne	1297c <run_command+0xb0>
   12b90:	e3560000 	cmp	r6, #0	; 0x0
   12b94:	01a06007 	moveq	r6, r7
   12b98:	ea000000 	b	12ba0 <run_command+0x2d4>
   12b9c:	e3e06000 	mvn	r6, #0	; 0x0
   12ba0:	e1a00006 	mov	r0, r6
   12ba4:	e28ddf93 	add	sp, sp, #588	; 0x24c
   12ba8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   12bac:	00018cbb 	.word	0x00018cbb
   12bb0:	00018cd1 	.word	0x00018cd1
   12bb4:	3c00d6cc 	.word	0x3c00d6cc

00012bb8 <cread_add_char>:
   12bb8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   12bbc:	e3510000 	cmp	r1, #0	; 0x0
   12bc0:	e24dd004 	sub	sp, sp, #4	; 0x4
   12bc4:	e1a07000 	mov	r7, r0
   12bc8:	e1a04002 	mov	r4, r2
   12bcc:	e1a00003 	mov	r0, r3
   12bd0:	e59d6018 	ldr	r6, [sp, #24]
   12bd4:	e59dc01c 	ldr	ip, [sp, #28]
   12bd8:	1a000003 	bne	12bec <cread_add_char+0x34>
   12bdc:	e5922000 	ldr	r2, [r2]
   12be0:	e5933000 	ldr	r3, [r3]
   12be4:	e1520003 	cmp	r2, r3
   12be8:	1a000026 	bne	12c88 <cread_add_char+0xd0>
   12bec:	e5902000 	ldr	r2, [r0]
   12bf0:	e24c3001 	sub	r3, ip, #1	; 0x1
   12bf4:	e1520003 	cmp	r2, r3
   12bf8:	9a000003 	bls	12c0c <cread_add_char+0x54>
   12bfc:	e3a00007 	mov	r0, #7	; 0x7
   12c00:	e28dd004 	add	sp, sp, #4	; 0x4
   12c04:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
   12c08:	eafffef1 	b	127d4 <putc>
   12c0c:	e3510000 	cmp	r1, #0	; 0x0
   12c10:	e2821001 	add	r1, r2, #1	; 0x1
   12c14:	e5801000 	str	r1, [r0]
   12c18:	0a00001a 	beq	12c88 <cread_add_char+0xd0>
   12c1c:	e5943000 	ldr	r3, [r4]
   12c20:	e0635001 	rsb	r5, r3, r1
   12c24:	e3550001 	cmp	r5, #1	; 0x1
   12c28:	9a000004 	bls	12c40 <cread_add_char+0x88>
   12c2c:	e2830001 	add	r0, r3, #1	; 0x1
   12c30:	e0860000 	add	r0, r6, r0
   12c34:	e0861003 	add	r1, r6, r3
   12c38:	e2452001 	sub	r2, r5, #1	; 0x1
   12c3c:	ebffba4d 	bl	1578 <memmove>
   12c40:	e5943000 	ldr	r3, [r4]
   12c44:	e59f0074 	ldr	r0, [pc, #116]	; 12cc0 <cread_add_char+0x108>
   12c48:	e7c67003 	strb	r7, [r6, r3]
   12c4c:	e5942000 	ldr	r2, [r4]
   12c50:	e59f306c 	ldr	r3, [pc, #108]	; 12cc4 <cread_add_char+0x10c>
   12c54:	e0862002 	add	r2, r6, r2
   12c58:	e1a01005 	mov	r1, r5
   12c5c:	e1a0e00f 	mov	lr, pc
   12c60:	e593f000 	ldr	pc, [r3]
   12c64:	e5943000 	ldr	r3, [r4]
   12c68:	e2833001 	add	r3, r3, #1	; 0x1
   12c6c:	e5843000 	str	r3, [r4]
   12c70:	ea000000 	b	12c78 <cread_add_char+0xc0>
   12c74:	ebfffed6 	bl	127d4 <putc>
   12c78:	e2555001 	subs	r5, r5, #1	; 0x1
   12c7c:	e3a00008 	mov	r0, #8	; 0x8
   12c80:	1afffffb 	bne	12c74 <cread_add_char+0xbc>
   12c84:	ea00000b 	b	12cb8 <cread_add_char+0x100>
   12c88:	e5943000 	ldr	r3, [r4]
   12c8c:	e59f002c 	ldr	r0, [pc, #44]	; 12cc0 <cread_add_char+0x108>
   12c90:	e7c67003 	strb	r7, [r6, r3]
   12c94:	e5942000 	ldr	r2, [r4]
   12c98:	e59f3024 	ldr	r3, [pc, #36]	; 12cc4 <cread_add_char+0x10c>
   12c9c:	e0862002 	add	r2, r6, r2
   12ca0:	e3a01001 	mov	r1, #1	; 0x1
   12ca4:	e1a0e00f 	mov	lr, pc
   12ca8:	e593f000 	ldr	pc, [r3]
   12cac:	e5943000 	ldr	r3, [r4]
   12cb0:	e2833001 	add	r3, r3, #1	; 0x1
   12cb4:	e5843000 	str	r3, [r4]
   12cb8:	e28dd004 	add	sp, sp, #4	; 0x4
   12cbc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   12cc0:	00018cf4 	.word	0x00018cf4
   12cc4:	3c00d6cc 	.word	0x3c00d6cc

00012cc8 <readline_into_buffer>:
   12cc8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   12ccc:	e59f367c 	ldr	r3, [pc, #1660]	; 13350 <readline_into_buffer+0x688>
   12cd0:	e24dd01c 	sub	sp, sp, #28	; 0x1c
   12cd4:	e593e000 	ldr	lr, [r3]
   12cd8:	e1a06001 	mov	r6, r1
   12cdc:	e35e0000 	cmp	lr, #0	; 0x0
   12ce0:	1a000014 	bne	12d38 <readline_into_buffer+0x70>
   12ce4:	e59f3668 	ldr	r3, [pc, #1640]	; 13354 <readline_into_buffer+0x68c>
   12ce8:	e3e02000 	mvn	r2, #0	; 0x0
   12cec:	e5832000 	str	r2, [r3]
   12cf0:	e59f3660 	ldr	r3, [pc, #1632]	; 13358 <readline_into_buffer+0x690>
   12cf4:	e59fc660 	ldr	ip, [pc, #1632]	; 1335c <readline_into_buffer+0x694>
   12cf8:	e583e000 	str	lr, [r3]
   12cfc:	e59f365c 	ldr	r3, [pc, #1628]	; 13360 <readline_into_buffer+0x698>
   12d00:	e59f465c 	ldr	r4, [pc, #1628]	; 13364 <readline_into_buffer+0x69c>
   12d04:	e583e000 	str	lr, [r3]
   12d08:	e59f3658 	ldr	r3, [pc, #1624]	; 13368 <readline_into_buffer+0x6a0>
   12d0c:	e1a0100e 	mov	r1, lr
   12d10:	e583e000 	str	lr, [r3]
   12d14:	e1a0300e 	mov	r3, lr
   12d18:	e784c001 	str	ip, [r4, r1]
   12d1c:	e2811004 	add	r1, r1, #4	; 0x4
   12d20:	e3510010 	cmp	r1, #16	; 0x10
   12d24:	e4cc3100 	strb	r3, [ip], #256
   12d28:	1afffffa 	bne	12d18 <readline_into_buffer+0x50>
   12d2c:	e59f361c 	ldr	r3, [pc, #1564]	; 13350 <readline_into_buffer+0x688>
   12d30:	e3a02001 	mov	r2, #1	; 0x1
   12d34:	e5832000 	str	r2, [r3]
   12d38:	ebfffe7d 	bl	12734 <puts>
   12d3c:	e3a03000 	mov	r3, #0	; 0x0
   12d40:	e58d3014 	str	r3, [sp, #20]
   12d44:	e58d3010 	str	r3, [sp, #16]
   12d48:	e1a05003 	mov	r5, r3
   12d4c:	e3a09001 	mov	r9, #1	; 0x1
   12d50:	e59fb614 	ldr	fp, [pc, #1556]	; 1336c <readline_into_buffer+0x6a4>
   12d54:	e59fa5f8 	ldr	sl, [pc, #1528]	; 13354 <readline_into_buffer+0x68c>
   12d58:	ea000000 	b	12d60 <readline_into_buffer+0x98>
   12d5c:	e3a05000 	mov	r5, #0	; 0x0
   12d60:	ebfffe9d 	bl	127dc <getc>
   12d64:	e6ef0070 	uxtb	r0, r0
   12d68:	e350000d 	cmp	r0, #13	; 0xd
   12d6c:	1350000a 	cmpne	r0, #10	; 0xa
   12d70:	13a04000 	movne	r4, #0	; 0x0
   12d74:	03a04001 	moveq	r4, #1	; 0x1
   12d78:	1a00000b 	bne	12dac <readline_into_buffer+0xe4>
   12d7c:	e59f35e8 	ldr	r3, [pc, #1512]	; 1336c <readline_into_buffer+0x6a4>
   12d80:	e59f05e8 	ldr	r0, [pc, #1512]	; 13370 <readline_into_buffer+0x6a8>
   12d84:	e1a0e00f 	mov	lr, pc
   12d88:	e593f000 	ldr	pc, [r3]
   12d8c:	e59d5010 	ldr	r5, [sp, #16]
   12d90:	e3a03000 	mov	r3, #0	; 0x0
   12d94:	e7c63005 	strb	r3, [r6, r5]
   12d98:	e5d63000 	ldrb	r3, [r6]
   12d9c:	e3530000 	cmp	r3, #0	; 0x0
   12da0:	13530021 	cmpne	r3, #33	; 0x21
   12da4:	0a000162 	beq	13334 <readline_into_buffer+0x66c>
   12da8:	ea00014e 	b	132e8 <readline_into_buffer+0x620>
   12dac:	e3550000 	cmp	r5, #0	; 0x0
   12db0:	0a000033 	beq	12e84 <readline_into_buffer+0x1bc>
   12db4:	e3550001 	cmp	r5, #1	; 0x1
   12db8:	1a00000c 	bne	12df0 <readline_into_buffer+0x128>
   12dbc:	e350005b 	cmp	r0, #91	; 0x5b
   12dc0:	05cd0009 	strbeq	r0, [sp, #9]
   12dc4:	03a05002 	moveq	r5, #2	; 0x2
   12dc8:	0affffe4 	beq	12d60 <readline_into_buffer+0x98>
   12dcc:	e3a0cc01 	mov	ip, #256	; 0x100
   12dd0:	e5dd0008 	ldrb	r0, [sp, #8]
   12dd4:	e1a01009 	mov	r1, r9
   12dd8:	e28d2014 	add	r2, sp, #20	; 0x14
   12ddc:	e28d3010 	add	r3, sp, #16	; 0x10
   12de0:	e88d1040 	stm	sp, {r6, ip}
   12de4:	e1a05004 	mov	r5, r4
   12de8:	ebffff72 	bl	12bb8 <cread_add_char>
   12dec:	eaffffdb 	b	12d60 <readline_into_buffer+0x98>
   12df0:	e2403041 	sub	r3, r0, #65	; 0x41
   12df4:	e3530007 	cmp	r3, #7	; 0x7
   12df8:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   12dfc:	ea000009 	b	12e28 <readline_into_buffer+0x160>
   12e00:	0001316c 	.word	0x0001316c
   12e04:	000131ac 	.word	0x000131ac
   12e08:	00012e74 	.word	0x00012e74
   12e0c:	00012f80 	.word	0x00012f80
   12e10:	00012e28 	.word	0x00012e28
   12e14:	00012e28 	.word	0x00012e28
   12e18:	00012e28 	.word	0x00012e28
   12e1c:	00012e20 	.word	0x00012e20
   12e20:	e3a05000 	mov	r5, #0	; 0x0
   12e24:	ea000043 	b	12f38 <readline_into_buffer+0x270>
   12e28:	e28d1018 	add	r1, sp, #24	; 0x18
   12e2c:	e0813005 	add	r3, r1, r5
   12e30:	e5430010 	strb	r0, [r3, #-16]
   12e34:	e2855001 	add	r5, r5, #1	; 0x1
   12e38:	e3a04000 	mov	r4, #0	; 0x0
   12e3c:	e28d8010 	add	r8, sp, #16	; 0x10
   12e40:	e3a07c01 	mov	r7, #256	; 0x100
   12e44:	ea000004 	b	12e5c <readline_into_buffer+0x194>
   12e48:	e28dc008 	add	ip, sp, #8	; 0x8
   12e4c:	e7dc0004 	ldrb	r0, [ip, r4]
   12e50:	e88d00c0 	stm	sp, {r6, r7}
   12e54:	ebffff57 	bl	12bb8 <cread_add_char>
   12e58:	e2844001 	add	r4, r4, #1	; 0x1
   12e5c:	e1540005 	cmp	r4, r5
   12e60:	e1a01009 	mov	r1, r9
   12e64:	e28d2014 	add	r2, sp, #20	; 0x14
   12e68:	e1a03008 	mov	r3, r8
   12e6c:	1afffff5 	bne	12e48 <readline_into_buffer+0x180>
   12e70:	eaffffb9 	b	12d5c <readline_into_buffer+0x94>
   12e74:	e3a00006 	mov	r0, #6	; 0x6
   12e78:	e350000b 	cmp	r0, #11	; 0xb
   12e7c:	e3a05000 	mov	r5, #0	; 0x0
   12e80:	ea000001 	b	12e8c <readline_into_buffer+0x1c4>
   12e84:	e350000b 	cmp	r0, #11	; 0xb
   12e88:	0a00005f 	beq	1300c <readline_into_buffer+0x344>
   12e8c:	8a00000d 	bhi	12ec8 <readline_into_buffer+0x200>
   12e90:	e3500004 	cmp	r0, #4	; 0x4
   12e94:	0a000042 	beq	12fa4 <readline_into_buffer+0x2dc>
   12e98:	8a000005 	bhi	12eb4 <readline_into_buffer+0x1ec>
   12e9c:	e3500002 	cmp	r0, #2	; 0x2
   12ea0:	0a000037 	beq	12f84 <readline_into_buffer+0x2bc>
   12ea4:	8a000028 	bhi	12f4c <readline_into_buffer+0x284>
   12ea8:	e3500001 	cmp	r0, #1	; 0x1
   12eac:	1a000106 	bne	132cc <readline_into_buffer+0x604>
   12eb0:	ea000020 	b	12f38 <readline_into_buffer+0x270>
   12eb4:	e3500006 	cmp	r0, #6	; 0x6
   12eb8:	0a000027 	beq	12f5c <readline_into_buffer+0x294>
   12ebc:	3a000066 	bcc	1305c <readline_into_buffer+0x394>
   12ec0:	e3500008 	cmp	r0, #8	; 0x8
   12ec4:	ea000015 	b	12f20 <readline_into_buffer+0x258>
   12ec8:	e3500015 	cmp	r0, #21	; 0x15
   12ecc:	0a000070 	beq	13094 <readline_into_buffer+0x3cc>
   12ed0:	8a000007 	bhi	12ef4 <readline_into_buffer+0x22c>
   12ed4:	e350000f 	cmp	r0, #15	; 0xf
   12ed8:	02299001 	eoreq	r9, r9, #1	; 0x1
   12edc:	0affff9f 	beq	12d60 <readline_into_buffer+0x98>
   12ee0:	e3500010 	cmp	r0, #16	; 0x10
   12ee4:	0a0000a0 	beq	1316c <readline_into_buffer+0x4a4>
   12ee8:	e350000e 	cmp	r0, #14	; 0xe
   12eec:	1a0000f6 	bne	132cc <readline_into_buffer+0x604>
   12ef0:	ea0000ad 	b	131ac <readline_into_buffer+0x4e4>
   12ef4:	e350001b 	cmp	r0, #27	; 0x1b
   12ef8:	05cd0008 	strbeq	r0, [sp, #8]
   12efc:	03a05001 	moveq	r5, #1	; 0x1
   12f00:	0affff96 	beq	12d60 <readline_into_buffer+0x98>
   12f04:	8a000002 	bhi	12f14 <readline_into_buffer+0x24c>
   12f08:	e3500018 	cmp	r0, #24	; 0x18
   12f0c:	1a0000ee 	bne	132cc <readline_into_buffer+0x604>
   12f10:	ea00005f 	b	13094 <readline_into_buffer+0x3cc>
   12f14:	e350007f 	cmp	r0, #127	; 0x7f
   12f18:	0a000075 	beq	130f4 <readline_into_buffer+0x42c>
   12f1c:	e35000ff 	cmp	r0, #255	; 0xff
   12f20:	1a0000e9 	bne	132cc <readline_into_buffer+0x604>
   12f24:	ea000072 	b	130f4 <readline_into_buffer+0x42c>
   12f28:	ebfffe29 	bl	127d4 <putc>
   12f2c:	e59d3014 	ldr	r3, [sp, #20]
   12f30:	e2433001 	sub	r3, r3, #1	; 0x1
   12f34:	e58d3014 	str	r3, [sp, #20]
   12f38:	e59d3014 	ldr	r3, [sp, #20]
   12f3c:	e3a00008 	mov	r0, #8	; 0x8
   12f40:	e3530000 	cmp	r3, #0	; 0x0
   12f44:	1afffff7 	bne	12f28 <readline_into_buffer+0x260>
   12f48:	eaffff84 	b	12d60 <readline_into_buffer+0x98>
   12f4c:	e3a03000 	mov	r3, #0	; 0x0
   12f50:	e5c63000 	strb	r3, [r6]
   12f54:	e3e00000 	mvn	r0, #0	; 0x0
   12f58:	ea0000fa 	b	13348 <readline_into_buffer+0x680>
   12f5c:	e59d2014 	ldr	r2, [sp, #20]
   12f60:	e59d3010 	ldr	r3, [sp, #16]
   12f64:	e1520003 	cmp	r2, r3
   12f68:	2affff7c 	bcs	12d60 <readline_into_buffer+0x98>
   12f6c:	e7d60002 	ldrb	r0, [r6, r2]
   12f70:	ebfffe17 	bl	127d4 <putc>
   12f74:	e59d3014 	ldr	r3, [sp, #20]
   12f78:	e2833001 	add	r3, r3, #1	; 0x1
   12f7c:	ea0000d0 	b	132c4 <readline_into_buffer+0x5fc>
   12f80:	e3a05000 	mov	r5, #0	; 0x0
   12f84:	e59d3014 	ldr	r3, [sp, #20]
   12f88:	e3530000 	cmp	r3, #0	; 0x0
   12f8c:	0affff73 	beq	12d60 <readline_into_buffer+0x98>
   12f90:	e3a00008 	mov	r0, #8	; 0x8
   12f94:	ebfffe0e 	bl	127d4 <putc>
   12f98:	e59d3014 	ldr	r3, [sp, #20]
   12f9c:	e2433001 	sub	r3, r3, #1	; 0x1
   12fa0:	ea0000c7 	b	132c4 <readline_into_buffer+0x5fc>
   12fa4:	e59d2014 	ldr	r2, [sp, #20]
   12fa8:	e59d3010 	ldr	r3, [sp, #16]
   12fac:	e1520003 	cmp	r2, r3
   12fb0:	2affff6a 	bcs	12d60 <readline_into_buffer+0x98>
   12fb4:	e2433001 	sub	r3, r3, #1	; 0x1
   12fb8:	e0534002 	subs	r4, r3, r2
   12fbc:	0a00000a 	beq	12fec <readline_into_buffer+0x324>
   12fc0:	e2821001 	add	r1, r2, #1	; 0x1
   12fc4:	e0861001 	add	r1, r6, r1
   12fc8:	e0860002 	add	r0, r6, r2
   12fcc:	e1a02004 	mov	r2, r4
   12fd0:	ebffb968 	bl	1578 <memmove>
   12fd4:	e59d2014 	ldr	r2, [sp, #20]
   12fd8:	e59f0394 	ldr	r0, [pc, #916]	; 13374 <readline_into_buffer+0x6ac>
   12fdc:	e0862002 	add	r2, r6, r2
   12fe0:	e1a01004 	mov	r1, r4
   12fe4:	e1a0e00f 	mov	lr, pc
   12fe8:	e59bf000 	ldr	pc, [fp]
   12fec:	e3a00020 	mov	r0, #32	; 0x20
   12ff0:	ebfffdf7 	bl	127d4 <putc>
   12ff4:	e2444001 	sub	r4, r4, #1	; 0x1
   12ff8:	e3a00008 	mov	r0, #8	; 0x8
   12ffc:	ebfffdf4 	bl	127d4 <putc>
   13000:	e3740001 	cmn	r4, #1	; 0x1
   13004:	1afffffa 	bne	12ff4 <readline_into_buffer+0x32c>
   13008:	ea000053 	b	1315c <readline_into_buffer+0x494>
   1300c:	e59d2014 	ldr	r2, [sp, #20]
   13010:	e59d3010 	ldr	r3, [sp, #16]
   13014:	e1520003 	cmp	r2, r3
   13018:	31a04002 	movcc	r4, r2
   1301c:	3a000002 	bcc	1302c <readline_into_buffer+0x364>
   13020:	eaffff4e 	b	12d60 <readline_into_buffer+0x98>
   13024:	ebfffdea 	bl	127d4 <putc>
   13028:	e2844001 	add	r4, r4, #1	; 0x1
   1302c:	e59d3010 	ldr	r3, [sp, #16]
   13030:	e3a00020 	mov	r0, #32	; 0x20
   13034:	e1540003 	cmp	r4, r3
   13038:	3afffff9 	bcc	13024 <readline_into_buffer+0x35c>
   1303c:	ea000000 	b	13044 <readline_into_buffer+0x37c>
   13040:	ebfffde3 	bl	127d4 <putc>
   13044:	e59d2014 	ldr	r2, [sp, #20]
   13048:	e3a00008 	mov	r0, #8	; 0x8
   1304c:	e1540002 	cmp	r4, r2
   13050:	e2444001 	sub	r4, r4, #1	; 0x1
   13054:	8afffff9 	bhi	13040 <readline_into_buffer+0x378>
   13058:	ea000023 	b	130ec <readline_into_buffer+0x424>
   1305c:	e59d3014 	ldr	r3, [sp, #20]
   13060:	e59d1010 	ldr	r1, [sp, #16]
   13064:	e1530001 	cmp	r3, r1
   13068:	2affff3c 	bcs	12d60 <readline_into_buffer+0x98>
   1306c:	e0631001 	rsb	r1, r3, r1
   13070:	e0862003 	add	r2, r6, r3
   13074:	e59f02f8 	ldr	r0, [pc, #760]	; 13374 <readline_into_buffer+0x6ac>
   13078:	e1a0e00f 	mov	lr, pc
   1307c:	e59bf000 	ldr	pc, [fp]
   13080:	ea00008e 	b	132c0 <readline_into_buffer+0x5f8>
   13084:	ebfffdd2 	bl	127d4 <putc>
   13088:	e59d3014 	ldr	r3, [sp, #20]
   1308c:	e2433001 	sub	r3, r3, #1	; 0x1
   13090:	e58d3014 	str	r3, [sp, #20]
   13094:	e59d2014 	ldr	r2, [sp, #20]
   13098:	e3a00008 	mov	r0, #8	; 0x8
   1309c:	e3520000 	cmp	r2, #0	; 0x0
   130a0:	1afffff7 	bne	13084 <readline_into_buffer+0x3bc>
   130a4:	e59d3010 	ldr	r3, [sp, #16]
   130a8:	e3530000 	cmp	r3, #0	; 0x0
   130ac:	11a04002 	movne	r4, r2
   130b0:	1a000002 	bne	130c0 <readline_into_buffer+0x3f8>
   130b4:	eaffff29 	b	12d60 <readline_into_buffer+0x98>
   130b8:	ebfffdc5 	bl	127d4 <putc>
   130bc:	e2844001 	add	r4, r4, #1	; 0x1
   130c0:	e59d3010 	ldr	r3, [sp, #16]
   130c4:	e3a00020 	mov	r0, #32	; 0x20
   130c8:	e1540003 	cmp	r4, r3
   130cc:	3afffff9 	bcc	130b8 <readline_into_buffer+0x3f0>
   130d0:	ea000000 	b	130d8 <readline_into_buffer+0x410>
   130d4:	ebfffdbe 	bl	127d4 <putc>
   130d8:	e59d2014 	ldr	r2, [sp, #20]
   130dc:	e3a00008 	mov	r0, #8	; 0x8
   130e0:	e1540002 	cmp	r4, r2
   130e4:	e2444001 	sub	r4, r4, #1	; 0x1
   130e8:	8afffff9 	bhi	130d4 <readline_into_buffer+0x40c>
   130ec:	e58d2010 	str	r2, [sp, #16]
   130f0:	eaffff1a 	b	12d60 <readline_into_buffer+0x98>
   130f4:	e59d0014 	ldr	r0, [sp, #20]
   130f8:	e3500000 	cmp	r0, #0	; 0x0
   130fc:	0affff17 	beq	12d60 <readline_into_buffer+0x98>
   13100:	e59d2010 	ldr	r2, [sp, #16]
   13104:	e2403001 	sub	r3, r0, #1	; 0x1
   13108:	e0604002 	rsb	r4, r0, r2
   1310c:	e0861000 	add	r1, r6, r0
   13110:	e1a02004 	mov	r2, r4
   13114:	e0860003 	add	r0, r6, r3
   13118:	e58d3014 	str	r3, [sp, #20]
   1311c:	ebffb915 	bl	1578 <memmove>
   13120:	e3a00008 	mov	r0, #8	; 0x8
   13124:	ebfffdaa 	bl	127d4 <putc>
   13128:	e59d2014 	ldr	r2, [sp, #20]
   1312c:	e59f0240 	ldr	r0, [pc, #576]	; 13374 <readline_into_buffer+0x6ac>
   13130:	e0862002 	add	r2, r6, r2
   13134:	e1a01004 	mov	r1, r4
   13138:	e1a0e00f 	mov	lr, pc
   1313c:	e59bf000 	ldr	pc, [fp]
   13140:	e3a00020 	mov	r0, #32	; 0x20
   13144:	ebfffda2 	bl	127d4 <putc>
   13148:	e2444001 	sub	r4, r4, #1	; 0x1
   1314c:	e3a00008 	mov	r0, #8	; 0x8
   13150:	ebfffd9f 	bl	127d4 <putc>
   13154:	e3740001 	cmn	r4, #1	; 0x1
   13158:	1afffffa 	bne	13148 <readline_into_buffer+0x480>
   1315c:	e59d3010 	ldr	r3, [sp, #16]
   13160:	e2433001 	sub	r3, r3, #1	; 0x1
   13164:	e58d3010 	str	r3, [sp, #16]
   13168:	eafffefc 	b	12d60 <readline_into_buffer+0x98>
   1316c:	e59a2000 	ldr	r2, [sl]
   13170:	e3520000 	cmp	r2, #0	; 0x0
   13174:	ba000024 	blt	1320c <readline_into_buffer+0x544>
   13178:	e2423001 	sub	r3, r2, #1	; 0x1
   1317c:	e3730001 	cmn	r3, #1	; 0x1
   13180:	059f01d0 	ldreq	r0, [pc, #464]	; 13358 <readline_into_buffer+0x690>
   13184:	e58a3000 	str	r3, [sl]
   13188:	05903000 	ldreq	r3, [r0]
   1318c:	e59fc1cc 	ldr	ip, [pc, #460]	; 13360 <readline_into_buffer+0x698>
   13190:	058a3000 	streq	r3, [sl]
   13194:	e59a1000 	ldr	r1, [sl]
   13198:	e59c3000 	ldr	r3, [ip]
   1319c:	e1510003 	cmp	r1, r3
   131a0:	058a2000 	streq	r2, [sl]
   131a4:	1a000014 	bne	131fc <readline_into_buffer+0x534>
   131a8:	ea000017 	b	1320c <readline_into_buffer+0x544>
   131ac:	e59a1000 	ldr	r1, [sl]
   131b0:	e3510000 	cmp	r1, #0	; 0x0
   131b4:	ba000014 	blt	1320c <readline_into_buffer+0x544>
   131b8:	e59f21a0 	ldr	r2, [pc, #416]	; 13360 <readline_into_buffer+0x698>
   131bc:	e5923000 	ldr	r3, [r2]
   131c0:	e1510003 	cmp	r1, r3
   131c4:	0a000010 	beq	1320c <readline_into_buffer+0x544>
   131c8:	e59f3188 	ldr	r3, [pc, #392]	; 13358 <readline_into_buffer+0x690>
   131cc:	e59fc18c 	ldr	ip, [pc, #396]	; 13360 <readline_into_buffer+0x698>
   131d0:	e5932000 	ldr	r2, [r3]
   131d4:	e2813001 	add	r3, r1, #1	; 0x1
   131d8:	e1530002 	cmp	r3, r2
   131dc:	e58a3000 	str	r3, [sl]
   131e0:	c3a03000 	movgt	r3, #0	; 0x0
   131e4:	c58a3000 	strgt	r3, [sl]
   131e8:	e59a1000 	ldr	r1, [sl]
   131ec:	e59c3000 	ldr	r3, [ip]
   131f0:	e1510003 	cmp	r1, r3
   131f4:	059f517c 	ldreq	r5, [pc, #380]	; 13378 <readline_into_buffer+0x6b0>
   131f8:	0a00000a 	beq	13228 <readline_into_buffer+0x560>
   131fc:	e59f0160 	ldr	r0, [pc, #352]	; 13364 <readline_into_buffer+0x69c>
   13200:	e7905101 	ldr	r5, [r0, r1, lsl #2]
   13204:	e3550000 	cmp	r5, #0	; 0x0
   13208:	1a000006 	bne	13228 <readline_into_buffer+0x560>
   1320c:	e3a00007 	mov	r0, #7	; 0x7
   13210:	ebfffd6f 	bl	127d4 <putc>
   13214:	eafffed0 	b	12d5c <readline_into_buffer+0x94>
   13218:	ebfffd6d 	bl	127d4 <putc>
   1321c:	e59d3014 	ldr	r3, [sp, #20]
   13220:	e2433001 	sub	r3, r3, #1	; 0x1
   13224:	e58d3014 	str	r3, [sp, #20]
   13228:	e59d2014 	ldr	r2, [sp, #20]
   1322c:	e3a00008 	mov	r0, #8	; 0x8
   13230:	e3520000 	cmp	r2, #0	; 0x0
   13234:	1afffff7 	bne	13218 <readline_into_buffer+0x550>
   13238:	e59d3010 	ldr	r3, [sp, #16]
   1323c:	e3530000 	cmp	r3, #0	; 0x0
   13240:	11a04002 	movne	r4, r2
   13244:	1a000002 	bne	13254 <readline_into_buffer+0x58c>
   13248:	ea00000d 	b	13284 <readline_into_buffer+0x5bc>
   1324c:	ebfffd60 	bl	127d4 <putc>
   13250:	e2844001 	add	r4, r4, #1	; 0x1
   13254:	e59d3010 	ldr	r3, [sp, #16]
   13258:	e3a00020 	mov	r0, #32	; 0x20
   1325c:	e1540003 	cmp	r4, r3
   13260:	3afffff9 	bcc	1324c <readline_into_buffer+0x584>
   13264:	ea000000 	b	1326c <readline_into_buffer+0x5a4>
   13268:	ebfffd59 	bl	127d4 <putc>
   1326c:	e59d2014 	ldr	r2, [sp, #20]
   13270:	e3a00008 	mov	r0, #8	; 0x8
   13274:	e1540002 	cmp	r4, r2
   13278:	e2444001 	sub	r4, r4, #1	; 0x1
   1327c:	8afffff9 	bhi	13268 <readline_into_buffer+0x5a0>
   13280:	e58d2010 	str	r2, [sp, #16]
   13284:	e1a01005 	mov	r1, r5
   13288:	e1a00006 	mov	r0, r6
   1328c:	ebffb83f 	bl	1390 <strcpy>
   13290:	e1a00006 	mov	r0, r6
   13294:	ebffb894 	bl	14ec <strlen>
   13298:	e59d3014 	ldr	r3, [sp, #20]
   1329c:	e58d0010 	str	r0, [sp, #16]
   132a0:	e1530000 	cmp	r3, r0
   132a4:	2afffeac 	bcs	12d5c <readline_into_buffer+0x94>
   132a8:	e0631000 	rsb	r1, r3, r0
   132ac:	e0862003 	add	r2, r6, r3
   132b0:	e59f00bc 	ldr	r0, [pc, #188]	; 13374 <readline_into_buffer+0x6ac>
   132b4:	e1a0e00f 	mov	lr, pc
   132b8:	e59bf000 	ldr	pc, [fp]
   132bc:	e3a05000 	mov	r5, #0	; 0x0
   132c0:	e59d3010 	ldr	r3, [sp, #16]
   132c4:	e58d3014 	str	r3, [sp, #20]
   132c8:	eafffea4 	b	12d60 <readline_into_buffer+0x98>
   132cc:	e3a0cc01 	mov	ip, #256	; 0x100
   132d0:	e1a01009 	mov	r1, r9
   132d4:	e28d2014 	add	r2, sp, #20	; 0x14
   132d8:	e28d3010 	add	r3, sp, #16	; 0x10
   132dc:	e88d1040 	stm	sp, {r6, ip}
   132e0:	ebfffe34 	bl	12bb8 <cread_add_char>
   132e4:	eafffe9d 	b	12d60 <readline_into_buffer+0x98>
   132e8:	e59f4070 	ldr	r4, [pc, #112]	; 13360 <readline_into_buffer+0x698>
   132ec:	e59f3070 	ldr	r3, [pc, #112]	; 13364 <readline_into_buffer+0x69c>
   132f0:	e5942000 	ldr	r2, [r4]
   132f4:	e1a01006 	mov	r1, r6
   132f8:	e7930102 	ldr	r0, [r3, r2, lsl #2]
   132fc:	ebffb823 	bl	1390 <strcpy>
   13300:	e5943000 	ldr	r3, [r4]
   13304:	e59f104c 	ldr	r1, [pc, #76]	; 13358 <readline_into_buffer+0x690>
   13308:	e2833001 	add	r3, r3, #1	; 0x1
   1330c:	e5912000 	ldr	r2, [r1]
   13310:	e3530003 	cmp	r3, #3	; 0x3
   13314:	c3a03000 	movgt	r3, #0	; 0x0
   13318:	e1530002 	cmp	r3, r2
   1331c:	e59f2044 	ldr	r2, [pc, #68]	; 13368 <readline_into_buffer+0x6a0>
   13320:	e5843000 	str	r3, [r4]
   13324:	c5813000 	strgt	r3, [r1]
   13328:	e5923000 	ldr	r3, [r2]
   1332c:	e2833001 	add	r3, r3, #1	; 0x1
   13330:	e5823000 	str	r3, [r2]
   13334:	e59f3024 	ldr	r3, [pc, #36]	; 13360 <readline_into_buffer+0x698>
   13338:	e1a00005 	mov	r0, r5
   1333c:	e5932000 	ldr	r2, [r3]
   13340:	e59f300c 	ldr	r3, [pc, #12]	; 13354 <readline_into_buffer+0x68c>
   13344:	e5832000 	str	r2, [r3]
   13348:	e28dd01c 	add	sp, sp, #28	; 0x1c
   1334c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   13350:	3c00d274 	.word	0x3c00d274
   13354:	3c00d118 	.word	0x3c00d118
   13358:	3c00d278 	.word	0x3c00d278
   1335c:	3c00d9ac 	.word	0x3c00d9ac
   13360:	3c00d27c 	.word	0x3c00d27c
   13364:	3c00d99c 	.word	0x3c00d99c
   13368:	3c00d270 	.word	0x3c00d270
   1336c:	3c00d6cc 	.word	0x3c00d6cc
   13370:	00016bef 	.word	0x00016bef
   13374:	00018cf4 	.word	0x00018cf4
   13378:	00016bf0 	.word	0x00016bf0

0001337c <readline>:
   1337c:	e59f1000 	ldr	r1, [pc, #0]	; 13384 <readline+0x8>
   13380:	eafffe50 	b	12cc8 <readline_into_buffer>
   13384:	3c00d89c 	.word	0x3c00d89c

00013388 <main_loop>:
   13388:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   1338c:	e24dd004 	sub	sp, sp, #4	; 0x4
   13390:	e59f0030 	ldr	r0, [pc, #48]	; 133c8 <main_loop+0x40>
   13394:	ebfffff8 	bl	1337c <readline>
   13398:	e1a03000 	mov	r3, r0
   1339c:	e3730001 	cmn	r3, #1	; 0x1
   133a0:	e3a01000 	mov	r1, #0	; 0x0
   133a4:	e59f0020 	ldr	r0, [pc, #32]	; 133cc <main_loop+0x44>
   133a8:	1a000002 	bne	133b8 <main_loop+0x30>
   133ac:	e59f001c 	ldr	r0, [pc, #28]	; 133d0 <main_loop+0x48>
   133b0:	ebfffcdf 	bl	12734 <puts>
   133b4:	eafffff5 	b	13390 <main_loop+0x8>
   133b8:	e3530000 	cmp	r3, #0	; 0x0
   133bc:	dafffff3 	ble	13390 <main_loop+0x8>
   133c0:	ebfffd41 	bl	128cc <run_command>
   133c4:	eafffff1 	b	13390 <main_loop+0x8>
   133c8:	00018d14 	.word	0x00018d14
   133cc:	3c00d89c 	.word	0x3c00d89c
   133d0:	00016bef 	.word	0x00016bef

000133d4 <parse_num>:
   133d4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   133d8:	e1a08001 	mov	r8, r1
   133dc:	e1a07002 	mov	r7, r2
   133e0:	e1a06003 	mov	r6, r3
   133e4:	ea000000 	b	133ec <parse_num+0x18>
   133e8:	e2800001 	add	r0, r0, #1	; 0x1
   133ec:	e5d03000 	ldrb	r3, [r0]
   133f0:	e3530020 	cmp	r3, #32	; 0x20
   133f4:	0afffffb 	beq	133e8 <parse_num+0x14>
   133f8:	ea000037 	b	134dc <parse_num+0x108>
   133fc:	e3520000 	cmp	r2, #0	; 0x0
   13400:	0a00000a 	beq	13430 <parse_num+0x5c>
   13404:	e3530030 	cmp	r3, #48	; 0x30
   13408:	1a000008 	bne	13430 <parse_num+0x5c>
   1340c:	e5d02001 	ldrb	r2, [r0, #1]
   13410:	e2423041 	sub	r3, r2, #65	; 0x41
   13414:	e6ef3073 	uxtb	r3, r3
   13418:	e3530019 	cmp	r3, #25	; 0x19
   1341c:	92823020 	addls	r3, r2, #32	; 0x20
   13420:	96ef2073 	uxtbls	r2, r3
   13424:	e3520078 	cmp	r2, #120	; 0x78
   13428:	02800002 	addeq	r0, r0, #2	; 0x2
   1342c:	03a04010 	moveq	r4, #16	; 0x10
   13430:	e4d0c001 	ldrb	ip, [r0], #1
   13434:	e24c3030 	sub	r3, ip, #48	; 0x30
   13438:	e24c2041 	sub	r2, ip, #65	; 0x41
   1343c:	e6ef3073 	uxtb	r3, r3
   13440:	e6ef2072 	uxtb	r2, r2
   13444:	e3530009 	cmp	r3, #9	; 0x9
   13448:	83a01000 	movhi	r1, #0	; 0x0
   1344c:	93a01001 	movls	r1, #1	; 0x1
   13450:	e3520005 	cmp	r2, #5	; 0x5
   13454:	83a02000 	movhi	r2, #0	; 0x0
   13458:	93a02001 	movls	r2, #1	; 0x1
   1345c:	e1913002 	orrs	r3, r1, r2
   13460:	1a000003 	bne	13474 <parse_num+0xa0>
   13464:	e24c3061 	sub	r3, ip, #97	; 0x61
   13468:	e6ef3073 	uxtb	r3, r3
   1346c:	e3530005 	cmp	r3, #5	; 0x5
   13470:	8a00000f 	bhi	134b4 <parse_num+0xe0>
   13474:	e3510000 	cmp	r1, #0	; 0x0
   13478:	124c2030 	subne	r2, ip, #48	; 0x30
   1347c:	1a000007 	bne	134a0 <parse_num+0xcc>
   13480:	e24c3061 	sub	r3, ip, #97	; 0x61
   13484:	e6ef3073 	uxtb	r3, r3
   13488:	e3530005 	cmp	r3, #5	; 0x5
   1348c:	924c2057 	subls	r2, ip, #87	; 0x57
   13490:	9a000002 	bls	134a0 <parse_num+0xcc>
   13494:	e3520000 	cmp	r2, #0	; 0x0
   13498:	0a000002 	beq	134a8 <parse_num+0xd4>
   1349c:	e24c2037 	sub	r2, ip, #55	; 0x37
   134a0:	e1520004 	cmp	r2, r4
   134a4:	aa000002 	bge	134b4 <parse_num+0xe0>
   134a8:	e0252495 	mla	r5, r5, r4, r2
   134ac:	e3a02000 	mov	r2, #0	; 0x0
   134b0:	ea00000c 	b	134e8 <parse_num+0x114>
   134b4:	e3560000 	cmp	r6, #0	; 0x0
   134b8:	0a000012 	beq	13508 <parse_num+0x134>
   134bc:	e3a03000 	mov	r3, #0	; 0x0
   134c0:	e7d62003 	ldrb	r2, [r6, r3]
   134c4:	e2833001 	add	r3, r3, #1	; 0x1
   134c8:	e3520000 	cmp	r2, #0	; 0x0
   134cc:	0a00000d 	beq	13508 <parse_num+0x134>
   134d0:	e15c0002 	cmp	ip, r2
   134d4:	1afffff9 	bne	134c0 <parse_num+0xec>
   134d8:	ea000005 	b	134f4 <parse_num+0x120>
   134dc:	e3a02001 	mov	r2, #1	; 0x1
   134e0:	e3a0400a 	mov	r4, #10	; 0xa
   134e4:	e3a05000 	mov	r5, #0	; 0x0
   134e8:	e5d03000 	ldrb	r3, [r0]
   134ec:	e3530000 	cmp	r3, #0	; 0x0
   134f0:	1affffc1 	bne	133fc <parse_num+0x28>
   134f4:	e3570000 	cmp	r7, #0	; 0x0
   134f8:	15870000 	strne	r0, [r7]
   134fc:	e5885000 	str	r5, [r8]
   13500:	e3a00001 	mov	r0, #1	; 0x1
   13504:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   13508:	e3a00000 	mov	r0, #0	; 0x0
   1350c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

00013510 <xyzModem_stream_close>:
   13510:	e59f2044 	ldr	r2, [pc, #68]	; 1355c <xyzModem_stream_close+0x4c>
   13514:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   13518:	e5920428 	ldr	r0, [r2, #1064]
   1351c:	e59f303c 	ldr	r3, [pc, #60]	; 13560 <xyzModem_stream_close+0x50>
   13520:	e592c424 	ldr	ip, [r2, #1060]
   13524:	e592e418 	ldr	lr, [r2, #1048]
   13528:	e3500000 	cmp	r0, #0	; 0x0
   1352c:	e24dd00c 	sub	sp, sp, #12	; 0xc
   13530:	e59f102c 	ldr	r1, [pc, #44]	; 13564 <xyzModem_stream_close+0x54>
   13534:	01a01003 	moveq	r1, r3
   13538:	e59f3028 	ldr	r3, [pc, #40]	; 13568 <xyzModem_stream_close+0x58>
   1353c:	e88d5000 	stm	sp, {ip, lr}
   13540:	e593c000 	ldr	ip, [r3]
   13544:	e59f0020 	ldr	r0, [pc, #32]	; 1356c <xyzModem_stream_close+0x5c>
   13548:	e5923420 	ldr	r3, [r2, #1056]
   1354c:	e592241c 	ldr	r2, [r2, #1052]
   13550:	e12fff3c 	blx	ip
   13554:	e28dd00c 	add	sp, sp, #12	; 0xc
   13558:	e8bd8000 	pop	{pc}
   1355c:	3c00d290 	.word	0x3c00d290
   13560:	00018d18 	.word	0x00018d18
   13564:	00018d1e 	.word	0x00018d1e
   13568:	3c00d6cc 	.word	0x3c00d6cc
   1356c:	00018d22 	.word	0x00018d22

00013570 <xyzModem_error>:
   13570:	e2800008 	add	r0, r0, #8	; 0x8
   13574:	e3500007 	cmp	r0, #7	; 0x7
   13578:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
   1357c:	ea000017 	b	135e0 <xyzModem_error+0x70>
   13580:	000135d8 	.word	0x000135d8
   13584:	000135d0 	.word	0x000135d0
   13588:	000135c8 	.word	0x000135c8
   1358c:	000135c0 	.word	0x000135c0
   13590:	000135b8 	.word	0x000135b8
   13594:	000135b0 	.word	0x000135b0
   13598:	000135a0 	.word	0x000135a0
   1359c:	000135a8 	.word	0x000135a8
   135a0:	e59f0040 	ldr	r0, [pc, #64]	; 135e8 <xyzModem_error+0x78>
   135a4:	e12fff1e 	bx	lr
   135a8:	e59f003c 	ldr	r0, [pc, #60]	; 135ec <xyzModem_error+0x7c>
   135ac:	e12fff1e 	bx	lr
   135b0:	e59f0038 	ldr	r0, [pc, #56]	; 135f0 <xyzModem_error+0x80>
   135b4:	e12fff1e 	bx	lr
   135b8:	e59f0034 	ldr	r0, [pc, #52]	; 135f4 <xyzModem_error+0x84>
   135bc:	e12fff1e 	bx	lr
   135c0:	e59f0030 	ldr	r0, [pc, #48]	; 135f8 <xyzModem_error+0x88>
   135c4:	e12fff1e 	bx	lr
   135c8:	e59f002c 	ldr	r0, [pc, #44]	; 135fc <xyzModem_error+0x8c>
   135cc:	e12fff1e 	bx	lr
   135d0:	e59f0028 	ldr	r0, [pc, #40]	; 13600 <xyzModem_error+0x90>
   135d4:	e12fff1e 	bx	lr
   135d8:	e59f0024 	ldr	r0, [pc, #36]	; 13604 <xyzModem_error+0x94>
   135dc:	e12fff1e 	bx	lr
   135e0:	e59f0020 	ldr	r0, [pc, #32]	; 13608 <xyzModem_error+0x98>
   135e4:	e12fff1e 	bx	lr
   135e8:	00018d63 	.word	0x00018d63
   135ec:	00018d83 	.word	0x00018d83
   135f0:	00018d95 	.word	0x00018d95
   135f4:	00018d9f 	.word	0x00018d9f
   135f8:	00018dab 	.word	0x00018dab
   135fc:	00018db5 	.word	0x00018db5
   13600:	00018dc5 	.word	0x00018dc5
   13604:	00018dd8 	.word	0x00018dd8
   13608:	00018ded 	.word	0x00018ded

0001360c <delay>:
   1360c:	e12fff1e 	bx	lr

00013610 <init_num>:
   13610:	e59f305c 	ldr	r3, [pc, #92]	; 13674 <init_num+0x64>
   13614:	e3a01001 	mov	r1, #1	; 0x1
   13618:	e5c31000 	strb	r1, [r3]
   1361c:	e59f3054 	ldr	r3, [pc, #84]	; 13678 <init_num+0x68>
   13620:	e3a02000 	mov	r2, #0	; 0x0
   13624:	e5c32000 	strb	r2, [r3]
   13628:	e59f304c 	ldr	r3, [pc, #76]	; 1367c <init_num+0x6c>
   1362c:	e5c32000 	strb	r2, [r3]
   13630:	e59f3048 	ldr	r3, [pc, #72]	; 13680 <init_num+0x70>
   13634:	e5c32000 	strb	r2, [r3]
   13638:	e59f3044 	ldr	r3, [pc, #68]	; 13684 <init_num+0x74>
   1363c:	e5c32000 	strb	r2, [r3]
   13640:	e59f3040 	ldr	r3, [pc, #64]	; 13688 <init_num+0x78>
   13644:	e5832000 	str	r2, [r3]
   13648:	e59f303c 	ldr	r3, [pc, #60]	; 1368c <init_num+0x7c>
   1364c:	e5c32000 	strb	r2, [r3]
   13650:	e59f3038 	ldr	r3, [pc, #56]	; 13690 <init_num+0x80>
   13654:	e5c32000 	strb	r2, [r3]
   13658:	e59f3034 	ldr	r3, [pc, #52]	; 13694 <init_num+0x84>
   1365c:	e5c32000 	strb	r2, [r3]
   13660:	e59f3030 	ldr	r3, [pc, #48]	; 13698 <init_num+0x88>
   13664:	e5c32000 	strb	r2, [r3]
   13668:	e59f302c 	ldr	r3, [pc, #44]	; 1369c <init_num+0x8c>
   1366c:	e5c32000 	strb	r2, [r3]
   13670:	e12fff1e 	bx	lr
   13674:	3c00d11c 	.word	0x3c00d11c
   13678:	3c00d28d 	.word	0x3c00d28d
   1367c:	3c00d280 	.word	0x3c00d280
   13680:	3c00d281 	.word	0x3c00d281
   13684:	3c00d282 	.word	0x3c00d282
   13688:	3c00d284 	.word	0x3c00d284
   1368c:	3c00d288 	.word	0x3c00d288
   13690:	3c00d289 	.word	0x3c00d289
   13694:	3c00d28a 	.word	0x3c00d28a
   13698:	3c00d28b 	.word	0x3c00d28b
   1369c:	3c00d28c 	.word	0x3c00d28c

000136a0 <out_message>:
   136a0:	e92d4010 	push	{r4, lr}
   136a4:	e59f30c8 	ldr	r3, [pc, #200]	; 13774 <out_message+0xd4>
   136a8:	e59f40c8 	ldr	r4, [pc, #200]	; 13778 <out_message+0xd8>
   136ac:	e5d31000 	ldrb	r1, [r3]
   136b0:	e59f00c4 	ldr	r0, [pc, #196]	; 1377c <out_message+0xdc>
   136b4:	e1a0e00f 	mov	lr, pc
   136b8:	e594f000 	ldr	pc, [r4]
   136bc:	e59f30bc 	ldr	r3, [pc, #188]	; 13780 <out_message+0xe0>
   136c0:	e59f00bc 	ldr	r0, [pc, #188]	; 13784 <out_message+0xe4>
   136c4:	e5d31000 	ldrb	r1, [r3]
   136c8:	e1a0e00f 	mov	lr, pc
   136cc:	e594f000 	ldr	pc, [r4]
   136d0:	e59f30b0 	ldr	r3, [pc, #176]	; 13788 <out_message+0xe8>
   136d4:	e59f00b0 	ldr	r0, [pc, #176]	; 1378c <out_message+0xec>
   136d8:	e5931000 	ldr	r1, [r3]
   136dc:	e1a0e00f 	mov	lr, pc
   136e0:	e594f000 	ldr	pc, [r4]
   136e4:	e59f30a4 	ldr	r3, [pc, #164]	; 13790 <out_message+0xf0>
   136e8:	e59f00a4 	ldr	r0, [pc, #164]	; 13794 <out_message+0xf4>
   136ec:	e5d31000 	ldrb	r1, [r3]
   136f0:	e1a0e00f 	mov	lr, pc
   136f4:	e594f000 	ldr	pc, [r4]
   136f8:	e59f3098 	ldr	r3, [pc, #152]	; 13798 <out_message+0xf8>
   136fc:	e59f0098 	ldr	r0, [pc, #152]	; 1379c <out_message+0xfc>
   13700:	e5d31000 	ldrb	r1, [r3]
   13704:	e1a0e00f 	mov	lr, pc
   13708:	e594f000 	ldr	pc, [r4]
   1370c:	e59f308c 	ldr	r3, [pc, #140]	; 137a0 <out_message+0x100>
   13710:	e59f008c 	ldr	r0, [pc, #140]	; 137a4 <out_message+0x104>
   13714:	e5d31000 	ldrb	r1, [r3]
   13718:	e1a0e00f 	mov	lr, pc
   1371c:	e594f000 	ldr	pc, [r4]
   13720:	e59f3080 	ldr	r3, [pc, #128]	; 137a8 <out_message+0x108>
   13724:	e59f0080 	ldr	r0, [pc, #128]	; 137ac <out_message+0x10c>
   13728:	e5d31000 	ldrb	r1, [r3]
   1372c:	e1a0e00f 	mov	lr, pc
   13730:	e594f000 	ldr	pc, [r4]
   13734:	e59f3074 	ldr	r3, [pc, #116]	; 137b0 <out_message+0x110>
   13738:	e59f0074 	ldr	r0, [pc, #116]	; 137b4 <out_message+0x114>
   1373c:	e5d31000 	ldrb	r1, [r3]
   13740:	e1a0e00f 	mov	lr, pc
   13744:	e594f000 	ldr	pc, [r4]
   13748:	e59f3068 	ldr	r3, [pc, #104]	; 137b8 <out_message+0x118>
   1374c:	e59f0068 	ldr	r0, [pc, #104]	; 137bc <out_message+0x11c>
   13750:	e5d31000 	ldrb	r1, [r3]
   13754:	e1a0e00f 	mov	lr, pc
   13758:	e594f000 	ldr	pc, [r4]
   1375c:	e59f305c 	ldr	r3, [pc, #92]	; 137c0 <out_message+0x120>
   13760:	e59f005c 	ldr	r0, [pc, #92]	; 137c4 <out_message+0x124>
   13764:	e5d31000 	ldrb	r1, [r3]
   13768:	e1a0e00f 	mov	lr, pc
   1376c:	e594f000 	ldr	pc, [r4]
   13770:	e8bd8010 	pop	{r4, pc}
   13774:	3c00d280 	.word	0x3c00d280
   13778:	3c00d6cc 	.word	0x3c00d6cc
   1377c:	00018dfb 	.word	0x00018dfb
   13780:	3c00d281 	.word	0x3c00d281
   13784:	00018e09 	.word	0x00018e09
   13788:	3c00d284 	.word	0x3c00d284
   1378c:	00018e1c 	.word	0x00018e1c
   13790:	3c00d288 	.word	0x3c00d288
   13794:	00018e2e 	.word	0x00018e2e
   13798:	3c00d289 	.word	0x3c00d289
   1379c:	00018e41 	.word	0x00018e41
   137a0:	3c00d28a 	.word	0x3c00d28a
   137a4:	00018e58 	.word	0x00018e58
   137a8:	3c00d11c 	.word	0x3c00d11c
   137ac:	00018e70 	.word	0x00018e70
   137b0:	3c00d28b 	.word	0x3c00d28b
   137b4:	00018e7f 	.word	0x00018e7f
   137b8:	3c00d28c 	.word	0x3c00d28c
   137bc:	00018e98 	.word	0x00018e98
   137c0:	3c00d28d 	.word	0x3c00d28d
   137c4:	00018eaa 	.word	0x00018eaa

000137c8 <Procpacket>:
   137c8:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   137cc:	e59fc1ac 	ldr	ip, [pc, #428]	; 13980 <Procpacket+0x1b8>
   137d0:	e59f21ac 	ldr	r2, [pc, #428]	; 13984 <Procpacket+0x1bc>
   137d4:	e5dc3000 	ldrb	r3, [ip]
   137d8:	e1a06000 	mov	r6, r0
   137dc:	e5c03000 	strb	r3, [r0]
   137e0:	e5923000 	ldr	r3, [r2]
   137e4:	e1a05001 	mov	r5, r1
   137e8:	e5c03001 	strb	r3, [r0, #1]
   137ec:	e5923000 	ldr	r3, [r2]
   137f0:	e1e03003 	mvn	r3, r3
   137f4:	e5c03002 	strb	r3, [r0, #2]
   137f8:	e59f3188 	ldr	r3, [pc, #392]	; 13988 <Procpacket+0x1c0>
   137fc:	e5922000 	ldr	r2, [r2]
   13800:	e5933000 	ldr	r3, [r3]
   13804:	e1520003 	cmp	r2, r3
   13808:	1a000008 	bne	13830 <Procpacket+0x68>
   1380c:	e59f3178 	ldr	r3, [pc, #376]	; 1398c <Procpacket+0x1c4>
   13810:	e5d32000 	ldrb	r2, [r3]
   13814:	e59f3174 	ldr	r3, [pc, #372]	; 13990 <Procpacket+0x1c8>
   13818:	e3520001 	cmp	r2, #1	; 0x1
   1381c:	e5934000 	ldr	r4, [r3]
   13820:	e5cc2000 	strb	r2, [ip]
   13824:	03a07080 	moveq	r7, #128	; 0x80
   13828:	13a07b01 	movne	r7, #1024	; 0x400
   1382c:	ea000004 	b	13844 <Procpacket+0x7c>
   13830:	e5dc3000 	ldrb	r3, [ip]
   13834:	e3530001 	cmp	r3, #1	; 0x1
   13838:	13a04b01 	movne	r4, #1024	; 0x400
   1383c:	03a04080 	moveq	r4, #128	; 0x80
   13840:	e1a07004 	mov	r7, r4
   13844:	e59f3148 	ldr	r3, [pc, #328]	; 13994 <Procpacket+0x1cc>
   13848:	e5d33000 	ldrb	r3, [r3]
   1384c:	e3530000 	cmp	r3, #0	; 0x0
   13850:	1a000008 	bne	13878 <Procpacket+0xb0>
   13854:	e59f313c 	ldr	r3, [pc, #316]	; 13998 <Procpacket+0x1d0>
   13858:	e2860003 	add	r0, r6, #3	; 0x3
   1385c:	e5931000 	ldr	r1, [r3]
   13860:	e59f3134 	ldr	r3, [pc, #308]	; 1399c <Procpacket+0x1d4>
   13864:	e1a02004 	mov	r2, r4
   13868:	e5933000 	ldr	r3, [r3]
   1386c:	e0631001 	rsb	r1, r3, r1
   13870:	e0851001 	add	r1, r5, r1
   13874:	ebffb736 	bl	1554 <memcpy>
   13878:	e59f111c 	ldr	r1, [pc, #284]	; 1399c <Procpacket+0x1d4>
   1387c:	e59f311c 	ldr	r3, [pc, #284]	; 139a0 <Procpacket+0x1d8>
   13880:	e5912000 	ldr	r2, [r1]
   13884:	e5d33000 	ldrb	r3, [r3]
   13888:	e0642002 	rsb	r2, r4, r2
   1388c:	e3530000 	cmp	r3, #0	; 0x0
   13890:	e5812000 	str	r2, [r1]
   13894:	0a000006 	beq	138b4 <Procpacket+0xec>
   13898:	e1a02007 	mov	r2, r7
   1389c:	e2860003 	add	r0, r6, #3	; 0x3
   138a0:	e3a01000 	mov	r1, #0	; 0x0
   138a4:	ebffb722 	bl	1534 <memset>
   138a8:	e59f30f4 	ldr	r3, [pc, #244]	; 139a4 <Procpacket+0x1dc>
   138ac:	e3a02001 	mov	r2, #1	; 0x1
   138b0:	e5c32000 	strb	r2, [r3]
   138b4:	e59f30c8 	ldr	r3, [pc, #200]	; 13984 <Procpacket+0x1bc>
   138b8:	e5932000 	ldr	r2, [r3]
   138bc:	e59f30c4 	ldr	r3, [pc, #196]	; 13988 <Procpacket+0x1c0>
   138c0:	e5933000 	ldr	r3, [r3]
   138c4:	e1520003 	cmp	r2, r3
   138c8:	1a000004 	bne	138e0 <Procpacket+0x118>
   138cc:	e2840003 	add	r0, r4, #3	; 0x3
   138d0:	e0860000 	add	r0, r6, r0
   138d4:	e0642007 	rsb	r2, r4, r7
   138d8:	e3a0101a 	mov	r1, #26	; 0x1a
   138dc:	ebffb714 	bl	1534 <memset>
   138e0:	e3a00000 	mov	r0, #0	; 0x0
   138e4:	e59f80bc 	ldr	r8, [pc, #188]	; 139a8 <Procpacket+0x1e0>
   138e8:	e1a04006 	mov	r4, r6
   138ec:	e1a0c000 	mov	ip, r0
   138f0:	e1a05000 	mov	r5, r0
   138f4:	e1a0342c 	lsr	r3, ip, #8
   138f8:	e5d42003 	ldrb	r2, [r4, #3]
   138fc:	e1a03083 	lsl	r3, r3, #1
   13900:	e19810b3 	ldrh	r1, [r8, r3]
   13904:	e2800001 	add	r0, r0, #1	; 0x1
   13908:	e182340c 	orr	r3, r2, ip, lsl #8
   1390c:	e0233001 	eor	r3, r3, r1
   13910:	e0852002 	add	r2, r5, r2
   13914:	e1500007 	cmp	r0, r7
   13918:	e6ffc073 	uxth	ip, r3
   1391c:	e6ef5072 	uxtb	r5, r2
   13920:	e2844001 	add	r4, r4, #1	; 0x1
   13924:	e59f107c 	ldr	r1, [pc, #124]	; 139a8 <Procpacket+0x1e0>
   13928:	3afffff1 	bcc	138f4 <Procpacket+0x12c>
   1392c:	e59f3078 	ldr	r3, [pc, #120]	; 139ac <Procpacket+0x1e4>
   13930:	e0860007 	add	r0, r6, r7
   13934:	e5d33000 	ldrb	r3, [r3]
   13938:	e3530000 	cmp	r3, #0	; 0x0
   1393c:	05c05003 	strbeq	r5, [r0, #3]
   13940:	0a00000c 	beq	13978 <Procpacket+0x1b0>
   13944:	e1a0342c 	lsr	r3, ip, #8
   13948:	e1a03083 	lsl	r3, r3, #1
   1394c:	e19120b3 	ldrh	r2, [r1, r3]
   13950:	e022240c 	eor	r2, r2, ip, lsl #8
   13954:	e1a03442 	asr	r3, r2, #8
   13958:	e20330ff 	and	r3, r3, #255	; 0xff
   1395c:	e1a03083 	lsl	r3, r3, #1
   13960:	e19130b3 	ldrh	r3, [r1, r3]
   13964:	e0233402 	eor	r3, r3, r2, lsl #8
   13968:	e6ff3073 	uxth	r3, r3
   1396c:	e1a02423 	lsr	r2, r3, #8
   13970:	e5c03004 	strb	r3, [r0, #4]
   13974:	e5c02003 	strb	r2, [r0, #3]
   13978:	e3a00000 	mov	r0, #0	; 0x0
   1397c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   13980:	3c00d282 	.word	0x3c00d282
   13984:	3c00d284 	.word	0x3c00d284
   13988:	3c00ddac 	.word	0x3c00ddac
   1398c:	3c00ddb4 	.word	0x3c00ddb4
   13990:	3c00ddb0 	.word	0x3c00ddb0
   13994:	3c00d288 	.word	0x3c00d288
   13998:	3c00d72c 	.word	0x3c00d72c
   1399c:	3c00d730 	.word	0x3c00d730
   139a0:	3c00d28d 	.word	0x3c00d28d
   139a4:	3c00d28c 	.word	0x3c00d28c
   139a8:	00015d4c 	.word	0x00015d4c
   139ac:	3c00d728 	.word	0x3c00d728

000139b0 <procsend>:
   139b0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   139b4:	e59f31f8 	ldr	r3, [pc, #504]	; 13bb4 <procsend+0x204>
   139b8:	e1a06000 	mov	r6, r0
   139bc:	e5d30000 	ldrb	r0, [r3]
   139c0:	e24dd004 	sub	sp, sp, #4	; 0x4
   139c4:	e3500000 	cmp	r0, #0	; 0x0
   139c8:	e1a0c002 	mov	ip, r2
   139cc:	e59f41e4 	ldr	r4, [pc, #484]	; 13bb8 <procsend+0x208>
   139d0:	0a000065 	beq	13b6c <procsend+0x1bc>
   139d4:	e59f31e0 	ldr	r3, [pc, #480]	; 13bbc <procsend+0x20c>
   139d8:	e1a02b02 	lsl	r2, r2, #22
   139dc:	e1a02b22 	lsr	r2, r2, #22
   139e0:	e583c000 	str	ip, [r3]
   139e4:	e59f31d4 	ldr	r3, [pc, #468]	; 13bc0 <procsend+0x210>
   139e8:	e59f01d4 	ldr	r0, [pc, #468]	; 13bc4 <procsend+0x214>
   139ec:	e3520000 	cmp	r2, #0	; 0x0
   139f0:	e1a0152c 	lsr	r1, ip, #10
   139f4:	e583c000 	str	ip, [r3]
   139f8:	03a03b01 	moveq	r3, #1024	; 0x400
   139fc:	e5802000 	str	r2, [r0]
   13a00:	e5841000 	str	r1, [r4]
   13a04:	05803000 	streq	r3, [r0]
   13a08:	0a000006 	beq	13a28 <procsend+0x78>
   13a0c:	e2813001 	add	r3, r1, #1	; 0x1
   13a10:	e352007f 	cmp	r2, #127	; 0x7f
   13a14:	e59f21ac 	ldr	r2, [pc, #428]	; 13bc8 <procsend+0x218>
   13a18:	e5843000 	str	r3, [r4]
   13a1c:	93a03001 	movls	r3, #1	; 0x1
   13a20:	83a03002 	movhi	r3, #2	; 0x2
   13a24:	e5c23000 	strb	r3, [r2]
   13a28:	e59f019c 	ldr	r0, [pc, #412]	; 13bcc <procsend+0x21c>
   13a2c:	ebffb6ae 	bl	14ec <strlen>
   13a30:	e59f2198 	ldr	r2, [pc, #408]	; 13bd0 <procsend+0x220>
   13a34:	e3500080 	cmp	r0, #128	; 0x80
   13a38:	83a03002 	movhi	r3, #2	; 0x2
   13a3c:	93a03001 	movls	r3, #1	; 0x1
   13a40:	85c23000 	strbhi	r3, [r2]
   13a44:	95c23000 	strbls	r3, [r2]
   13a48:	e59f3184 	ldr	r3, [pc, #388]	; 13bd4 <procsend+0x224>
   13a4c:	83a07b01 	movhi	r7, #1024	; 0x400
   13a50:	e5d31000 	ldrb	r1, [r3]
   13a54:	93a07080 	movls	r7, #128	; 0x80
   13a58:	e3510000 	cmp	r1, #0	; 0x0
   13a5c:	13a01000 	movne	r1, #0	; 0x0
   13a60:	12872005 	addne	r2, r7, #5	; 0x5
   13a64:	02872004 	addeq	r2, r7, #4	; 0x4
   13a68:	11a00006 	movne	r0, r6
   13a6c:	01a00006 	moveq	r0, r6
   13a70:	ebffb6af 	bl	1534 <memset>
   13a74:	e59f3154 	ldr	r3, [pc, #340]	; 13bd0 <procsend+0x220>
   13a78:	e3a05000 	mov	r5, #0	; 0x0
   13a7c:	e5d33000 	ldrb	r3, [r3]
   13a80:	e59f0144 	ldr	r0, [pc, #324]	; 13bcc <procsend+0x21c>
   13a84:	e5c63000 	strb	r3, [r6]
   13a88:	e3e03000 	mvn	r3, #0	; 0x0
   13a8c:	e5c63002 	strb	r3, [r6, #2]
   13a90:	e5c65001 	strb	r5, [r6, #1]
   13a94:	ebffb694 	bl	14ec <strlen>
   13a98:	e59f112c 	ldr	r1, [pc, #300]	; 13bcc <procsend+0x21c>
   13a9c:	e1a02000 	mov	r2, r0
   13aa0:	e1a04000 	mov	r4, r0
   13aa4:	e2860003 	add	r0, r6, #3	; 0x3
   13aa8:	ebffb63f 	bl	13ac <strncpy>
   13aac:	e59f3108 	ldr	r3, [pc, #264]	; 13bbc <procsend+0x20c>
   13ab0:	e2844004 	add	r4, r4, #4	; 0x4
   13ab4:	e0860004 	add	r0, r6, r4
   13ab8:	e5932000 	ldr	r2, [r3]
   13abc:	e59f1114 	ldr	r1, [pc, #276]	; 13bd8 <procsend+0x228>
   13ac0:	e59f4114 	ldr	r4, [pc, #276]	; 13bdc <procsend+0x22c>
   13ac4:	ebfffab8 	bl	125ac <sprintf>
   13ac8:	e1a00006 	mov	r0, r6
   13acc:	e1a0c005 	mov	ip, r5
   13ad0:	e1a0e005 	mov	lr, r5
   13ad4:	e1a0342c 	lsr	r3, ip, #8
   13ad8:	e5d02003 	ldrb	r2, [r0, #3]
   13adc:	e1a03083 	lsl	r3, r3, #1
   13ae0:	e19410b3 	ldrh	r1, [r4, r3]
   13ae4:	e2855001 	add	r5, r5, #1	; 0x1
   13ae8:	e182340c 	orr	r3, r2, ip, lsl #8
   13aec:	e0233001 	eor	r3, r3, r1
   13af0:	e08e2002 	add	r2, lr, r2
   13af4:	e1550007 	cmp	r5, r7
   13af8:	e6ffc073 	uxth	ip, r3
   13afc:	e6efe072 	uxtb	lr, r2
   13b00:	e2800001 	add	r0, r0, #1	; 0x1
   13b04:	e59f10d0 	ldr	r1, [pc, #208]	; 13bdc <procsend+0x22c>
   13b08:	3afffff1 	bcc	13ad4 <procsend+0x124>
   13b0c:	e59f30c0 	ldr	r3, [pc, #192]	; 13bd4 <procsend+0x224>
   13b10:	e0860007 	add	r0, r6, r7
   13b14:	e5d33000 	ldrb	r3, [r3]
   13b18:	e3530000 	cmp	r3, #0	; 0x0
   13b1c:	05c0e003 	strbeq	lr, [r0, #3]
   13b20:	0a00000c 	beq	13b58 <procsend+0x1a8>
   13b24:	e1a0342c 	lsr	r3, ip, #8
   13b28:	e1a03083 	lsl	r3, r3, #1
   13b2c:	e19120b3 	ldrh	r2, [r1, r3]
   13b30:	e022240c 	eor	r2, r2, ip, lsl #8
   13b34:	e1a03442 	asr	r3, r2, #8
   13b38:	e20330ff 	and	r3, r3, #255	; 0xff
   13b3c:	e1a03083 	lsl	r3, r3, #1
   13b40:	e19130b3 	ldrh	r3, [r1, r3]
   13b44:	e0233402 	eor	r3, r3, r2, lsl #8
   13b48:	e6ff3073 	uxth	r3, r3
   13b4c:	e1a02423 	lsr	r2, r3, #8
   13b50:	e5c03004 	strb	r3, [r0, #4]
   13b54:	e5c02003 	strb	r2, [r0, #3]
   13b58:	e59f3080 	ldr	r3, [pc, #128]	; 13be0 <procsend+0x230>
   13b5c:	e3a02001 	mov	r2, #1	; 0x1
   13b60:	e5c32000 	strb	r2, [r3]
   13b64:	e3a00000 	mov	r0, #0	; 0x0
   13b68:	ea00000f 	b	13bac <procsend+0x1fc>
   13b6c:	e59f2070 	ldr	r2, [pc, #112]	; 13be4 <procsend+0x234>
   13b70:	e5943000 	ldr	r3, [r4]
   13b74:	e5922000 	ldr	r2, [r2]
   13b78:	e1520003 	cmp	r2, r3
   13b7c:	8a00000a 	bhi	13bac <procsend+0x1fc>
   13b80:	e59f3060 	ldr	r3, [pc, #96]	; 13be8 <procsend+0x238>
   13b84:	e59f2044 	ldr	r2, [pc, #68]	; 13bd0 <procsend+0x220>
   13b88:	e5d33000 	ldrb	r3, [r3]
   13b8c:	e1a00006 	mov	r0, r6
   13b90:	e3530000 	cmp	r3, #0	; 0x0
   13b94:	13a03001 	movne	r3, #1	; 0x1
   13b98:	03a03002 	moveq	r3, #2	; 0x2
   13b9c:	e5c23000 	strb	r3, [r2]
   13ba0:	e1a0200c 	mov	r2, ip
   13ba4:	ebffff07 	bl	137c8 <Procpacket>
   13ba8:	e6ef0070 	uxtb	r0, r0
   13bac:	e28dd004 	add	sp, sp, #4	; 0x4
   13bb0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   13bb4:	3c00d11c 	.word	0x3c00d11c
   13bb8:	3c00ddac 	.word	0x3c00ddac
   13bbc:	3c00d730 	.word	0x3c00d730
   13bc0:	3c00d72c 	.word	0x3c00d72c
   13bc4:	3c00ddb0 	.word	0x3c00ddb0
   13bc8:	3c00ddb4 	.word	0x3c00ddb4
   13bcc:	00018eb9 	.word	0x00018eb9
   13bd0:	3c00d282 	.word	0x3c00d282
   13bd4:	3c00d728 	.word	0x3c00d728
   13bd8:	00017fa7 	.word	0x00017fa7
   13bdc:	00015d4c 	.word	0x00015d4c
   13be0:	3c00d281 	.word	0x3c00d281
   13be4:	3c00d284 	.word	0x3c00d284
   13be8:	3c00d28d 	.word	0x3c00d28d

00013bec <CYGACC_COMM_IF_PUTC>:
   13bec:	e1a00001 	mov	r0, r1
   13bf0:	eafffaf7 	b	127d4 <putc>

00013bf4 <real_send>:
   13bf4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   13bf8:	e59f30ec 	ldr	r3, [pc, #236]	; 13cec <real_send+0xf8>
   13bfc:	e24ddb01 	sub	sp, sp, #1024	; 0x400
   13c00:	e5d33000 	ldrb	r3, [r3]
   13c04:	e24dd00c 	sub	sp, sp, #12	; 0xc
   13c08:	e3530000 	cmp	r3, #0	; 0x0
   13c0c:	e1a0e000 	mov	lr, r0
   13c10:	e1a0c001 	mov	ip, r1
   13c14:	159f30d4 	ldrne	r3, [pc, #212]	; 13cf0 <real_send+0xfc>
   13c18:	13a02000 	movne	r2, #0	; 0x0
   13c1c:	15832000 	strne	r2, [r3]
   13c20:	1a000006 	bne	13c40 <real_send+0x4c>
   13c24:	e59f30c8 	ldr	r3, [pc, #200]	; 13cf4 <real_send+0x100>
   13c28:	e5d33000 	ldrb	r3, [r3]
   13c2c:	e3530000 	cmp	r3, #0	; 0x0
   13c30:	059f20b8 	ldreq	r2, [pc, #184]	; 13cf0 <real_send+0xfc>
   13c34:	05923000 	ldreq	r3, [r2]
   13c38:	02833001 	addeq	r3, r3, #1	; 0x1
   13c3c:	05823000 	streq	r3, [r2]
   13c40:	e28d0008 	add	r0, sp, #8	; 0x8
   13c44:	e2400005 	sub	r0, r0, #5	; 0x5
   13c48:	e1a0100e 	mov	r1, lr
   13c4c:	e1a0200c 	mov	r2, ip
   13c50:	ebffff56 	bl	139b0 <procsend>
   13c54:	e2507000 	subs	r7, r0, #0	; 0x0
   13c58:	1a00001f 	bne	13cdc <real_send+0xe8>
   13c5c:	e59f3094 	ldr	r3, [pc, #148]	; 13cf8 <real_send+0x104>
   13c60:	e28d5008 	add	r5, sp, #8	; 0x8
   13c64:	e5d31000 	ldrb	r1, [r3]
   13c68:	e59f308c 	ldr	r3, [pc, #140]	; 13cfc <real_send+0x108>
   13c6c:	e3510001 	cmp	r1, #1	; 0x1
   13c70:	e5d32000 	ldrb	r2, [r3]
   13c74:	e3003404 	movw	r3, #1028	; 0x404
   13c78:	11a06003 	movne	r6, r3
   13c7c:	03a06084 	moveq	r6, #132	; 0x84
   13c80:	e3520000 	cmp	r2, #0	; 0x0
   13c84:	12866001 	addne	r6, r6, #1	; 0x1
   13c88:	e2455005 	sub	r5, r5, #5	; 0x5
   13c8c:	e3a04000 	mov	r4, #0	; 0x0
   13c90:	e7d50004 	ldrb	r0, [r5, r4]
   13c94:	e2844001 	add	r4, r4, #1	; 0x1
   13c98:	ebfffacd 	bl	127d4 <putc>
   13c9c:	e1540006 	cmp	r4, r6
   13ca0:	3afffffa 	bcc	13c90 <real_send+0x9c>
   13ca4:	e59f3054 	ldr	r3, [pc, #84]	; 13d00 <real_send+0x10c>
   13ca8:	e5d33000 	ldrb	r3, [r3]
   13cac:	e3530000 	cmp	r3, #0	; 0x0
   13cb0:	13e07000 	mvnne	r7, #0	; 0x0
   13cb4:	1a000008 	bne	13cdc <real_send+0xe8>
   13cb8:	e59f3030 	ldr	r3, [pc, #48]	; 13cf0 <real_send+0xfc>
   13cbc:	e5932000 	ldr	r2, [r3]
   13cc0:	e59f303c 	ldr	r3, [pc, #60]	; 13d04 <real_send+0x110>
   13cc4:	e5933000 	ldr	r3, [r3]
   13cc8:	e1520003 	cmp	r2, r3
   13ccc:	059f3034 	ldreq	r3, [pc, #52]	; 13d08 <real_send+0x114>
   13cd0:	03a02001 	moveq	r2, #1	; 0x1
   13cd4:	05c32000 	strbeq	r2, [r3]
   13cd8:	03a07003 	moveq	r7, #3	; 0x3
   13cdc:	e1a00007 	mov	r0, r7
   13ce0:	e28dd00c 	add	sp, sp, #12	; 0xc
   13ce4:	e28ddb01 	add	sp, sp, #1024	; 0x400
   13ce8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   13cec:	3c00d28d 	.word	0x3c00d28d
   13cf0:	3c00d284 	.word	0x3c00d284
   13cf4:	3c00d11c 	.word	0x3c00d11c
   13cf8:	3c00d282 	.word	0x3c00d282
   13cfc:	3c00d728 	.word	0x3c00d728
   13d00:	3c00d28c 	.word	0x3c00d28c
   13d04:	3c00ddac 	.word	0x3c00ddac
   13d08:	3c00d288 	.word	0x3c00d288

00013d0c <CYGACC_COMM_IF_GETC_TIMEOUT>:
   13d0c:	e92d4070 	push	{r4, r5, r6, lr}
   13d10:	e1a05001 	mov	r5, r1
   13d14:	e3a04000 	mov	r4, #0	; 0x0
   13d18:	e59f603c 	ldr	r6, [pc, #60]	; 13d5c <CYGACC_COMM_IF_GETC_TIMEOUT+0x50>
   13d1c:	ea000000 	b	13d24 <CYGACC_COMM_IF_GETC_TIMEOUT+0x18>
   13d20:	ebffe72d 	bl	d9dc <udelay>
   13d24:	ebfffaab 	bl	127d8 <tstc>
   13d28:	e3500000 	cmp	r0, #0	; 0x0
   13d2c:	e3a00014 	mov	r0, #20	; 0x14
   13d30:	1a000002 	bne	13d40 <CYGACC_COMM_IF_GETC_TIMEOUT+0x34>
   13d34:	e1540006 	cmp	r4, r6
   13d38:	e2844001 	add	r4, r4, #1	; 0x1
   13d3c:	1afffff7 	bne	13d20 <CYGACC_COMM_IF_GETC_TIMEOUT+0x14>
   13d40:	ebfffaa4 	bl	127d8 <tstc>
   13d44:	e3500000 	cmp	r0, #0	; 0x0
   13d48:	08bd8070 	popeq	{r4, r5, r6, pc}
   13d4c:	ebfffaa2 	bl	127dc <getc>
   13d50:	e5c50000 	strb	r0, [r5]
   13d54:	e3a00001 	mov	r0, #1	; 0x1
   13d58:	e8bd8070 	pop	{r4, r5, r6, pc}
   13d5c:	000186a0 	.word	0x000186a0

00013d60 <control_send>:
   13d60:	e59f323c 	ldr	r3, [pc, #572]	; 13fa4 <control_send+0x244>
   13d64:	e3a02000 	mov	r2, #0	; 0x0
   13d68:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   13d6c:	e5c32000 	strb	r2, [r3]
   13d70:	e59f3230 	ldr	r3, [pc, #560]	; 13fa8 <control_send+0x248>
   13d74:	e59f4230 	ldr	r4, [pc, #560]	; 13fac <control_send+0x24c>
   13d78:	e5c32000 	strb	r2, [r3]
   13d7c:	e59f322c 	ldr	r3, [pc, #556]	; 13fb0 <control_send+0x250>
   13d80:	e59f522c 	ldr	r5, [pc, #556]	; 13fb4 <control_send+0x254>
   13d84:	e5832000 	str	r2, [r3]
   13d88:	e3a03001 	mov	r3, #1	; 0x1
   13d8c:	e59fe224 	ldr	lr, [pc, #548]	; 13fb8 <control_send+0x258>
   13d90:	e59f8224 	ldr	r8, [pc, #548]	; 13fbc <control_send+0x25c>
   13d94:	e59f6224 	ldr	r6, [pc, #548]	; 13fc0 <control_send+0x260>
   13d98:	e59f7224 	ldr	r7, [pc, #548]	; 13fc4 <control_send+0x264>
   13d9c:	e59fc224 	ldr	ip, [pc, #548]	; 13fc8 <control_send+0x268>
   13da0:	e5c43000 	strb	r3, [r4]
   13da4:	e59f3220 	ldr	r3, [pc, #544]	; 13fcc <control_send+0x26c>
   13da8:	e24dd014 	sub	sp, sp, #20	; 0x14
   13dac:	e5c52000 	strb	r2, [r5]
   13db0:	e5c32000 	strb	r2, [r3]
   13db4:	e58d0004 	str	r0, [sp, #4]
   13db8:	e1a0b001 	mov	fp, r1
   13dbc:	e5c82000 	strb	r2, [r8]
   13dc0:	e5c62000 	strb	r2, [r6]
   13dc4:	e5c72000 	strb	r2, [r7]
   13dc8:	e5ce2000 	strb	r2, [lr]
   13dcc:	e5cc2000 	strb	r2, [ip]
   13dd0:	e1a0a002 	mov	sl, r2
   13dd4:	e1a0900e 	mov	r9, lr
   13dd8:	e59f51f0 	ldr	r5, [pc, #496]	; 13fd0 <control_send+0x270>
   13ddc:	ea000000 	b	13de4 <control_send+0x84>
   13de0:	e3a0a000 	mov	sl, #0	; 0x0
   13de4:	e3a00000 	mov	r0, #0	; 0x0
   13de8:	e28d100f 	add	r1, sp, #15	; 0xf
   13dec:	ebffffc6 	bl	13d0c <CYGACC_COMM_IF_GETC_TIMEOUT>
   13df0:	e3500000 	cmp	r0, #0	; 0x0
   13df4:	0afffffa 	beq	13de4 <control_send+0x84>
   13df8:	e5dd300f 	ldrb	r3, [sp, #15]
   13dfc:	e3530018 	cmp	r3, #24	; 0x18
   13e00:	0a000042 	beq	13f10 <control_send+0x1b0>
   13e04:	8a000004 	bhi	13e1c <control_send+0xbc>
   13e08:	e3530006 	cmp	r3, #6	; 0x6
   13e0c:	0a00002c 	beq	13ec4 <control_send+0x164>
   13e10:	e3530015 	cmp	r3, #21	; 0x15
   13e14:	1afffff1 	bne	13de0 <control_send+0x80>
   13e18:	ea000010 	b	13e60 <control_send+0x100>
   13e1c:	e3530043 	cmp	r3, #67	; 0x43
   13e20:	0a000001 	beq	13e2c <control_send+0xcc>
   13e24:	e3530063 	cmp	r3, #99	; 0x63
   13e28:	1affffec 	bne	13de0 <control_send+0x80>
   13e2c:	e5d73000 	ldrb	r3, [r7]
   13e30:	e5d42000 	ldrb	r2, [r4]
   13e34:	e5d61000 	ldrb	r1, [r6]
   13e38:	e1833002 	orr	r3, r3, r2
   13e3c:	e1913003 	orrs	r3, r1, r3
   13e40:	0affffe6 	beq	13de0 <control_send+0x80>
   13e44:	e3510000 	cmp	r1, #0	; 0x0
   13e48:	159f2178 	ldrne	r2, [pc, #376]	; 13fc8 <control_send+0x268>
   13e4c:	13a03001 	movne	r3, #1	; 0x1
   13e50:	15c23000 	strbne	r3, [r2]
   13e54:	e3a03001 	mov	r3, #1	; 0x1
   13e58:	e5c53000 	strb	r3, [r5]
   13e5c:	ea00002f 	b	13f20 <control_send+0x1c0>
   13e60:	e59f313c 	ldr	r3, [pc, #316]	; 13fa4 <control_send+0x244>
   13e64:	e5d33000 	ldrb	r3, [r3]
   13e68:	e3530000 	cmp	r3, #0	; 0x0
   13e6c:	0a000008 	beq	13e94 <control_send+0x134>
   13e70:	e59f2138 	ldr	r2, [pc, #312]	; 13fb0 <control_send+0x250>
   13e74:	e5923000 	ldr	r3, [r2]
   13e78:	e2433001 	sub	r3, r3, #1	; 0x1
   13e7c:	e5823000 	str	r3, [r2]
   13e80:	e3a03001 	mov	r3, #1	; 0x1
   13e84:	e5c43000 	strb	r3, [r4]
   13e88:	e3a03000 	mov	r3, #0	; 0x0
   13e8c:	e5c93000 	strb	r3, [r9]
   13e90:	ea000022 	b	13f20 <control_send+0x1c0>
   13e94:	e5d42000 	ldrb	r2, [r4]
   13e98:	e3520000 	cmp	r2, #0	; 0x0
   13e9c:	15c53000 	strbne	r3, [r5]
   13ea0:	1a000007 	bne	13ec4 <control_send+0x164>
   13ea4:	e5d93000 	ldrb	r3, [r9]
   13ea8:	e3530000 	cmp	r3, #0	; 0x0
   13eac:	15c92000 	strbne	r2, [r9]
   13eb0:	e59f20f8 	ldr	r2, [pc, #248]	; 13fb0 <control_send+0x250>
   13eb4:	e5923000 	ldr	r3, [r2]
   13eb8:	e2433001 	sub	r3, r3, #1	; 0x1
   13ebc:	e5823000 	str	r3, [r2]
   13ec0:	ea000016 	b	13f20 <control_send+0x1c0>
   13ec4:	e5d42000 	ldrb	r2, [r4]
   13ec8:	e3520000 	cmp	r2, #0	; 0x0
   13ecc:	1affffc3 	bne	13de0 <control_send+0x80>
   13ed0:	e5d91000 	ldrb	r1, [r9]
   13ed4:	e3510000 	cmp	r1, #0	; 0x0
   13ed8:	0a000005 	beq	13ef4 <control_send+0x194>
   13edc:	e59f30c0 	ldr	r3, [pc, #192]	; 13fa4 <control_send+0x244>
   13ee0:	e1a0a002 	mov	sl, r2
   13ee4:	e5c32000 	strb	r2, [r3]
   13ee8:	e3a03001 	mov	r3, #1	; 0x1
   13eec:	e5c73000 	strb	r3, [r7]
   13ef0:	eaffffbb 	b	13de4 <control_send+0x84>
   13ef4:	e5d83000 	ldrb	r3, [r8]
   13ef8:	e3530000 	cmp	r3, #0	; 0x0
   13efc:	13a03001 	movne	r3, #1	; 0x1
   13f00:	15c63000 	strbne	r3, [r6]
   13f04:	11a0a001 	movne	sl, r1
   13f08:	1affffb5 	bne	13de4 <control_send+0x84>
   13f0c:	ea000003 	b	13f20 <control_send+0x1c0>
   13f10:	e28aa001 	add	sl, sl, #1	; 0x1
   13f14:	e35a0002 	cmp	sl, #2	; 0x2
   13f18:	1affffb1 	bne	13de4 <control_send+0x84>
   13f1c:	ea00001d 	b	13f98 <control_send+0x238>
   13f20:	e59f20a0 	ldr	r2, [pc, #160]	; 13fc8 <control_send+0x268>
   13f24:	e5d23000 	ldrb	r3, [r2]
   13f28:	e3530000 	cmp	r3, #0	; 0x0
   13f2c:	1a000008 	bne	13f54 <control_send+0x1f4>
   13f30:	e59f207c 	ldr	r2, [pc, #124]	; 13fb4 <control_send+0x254>
   13f34:	e5d23000 	ldrb	r3, [r2]
   13f38:	e3530000 	cmp	r3, #0	; 0x0
   13f3c:	0a000004 	beq	13f54 <control_send+0x1f4>
   13f40:	e3a00004 	mov	r0, #4	; 0x4
   13f44:	ebfffa22 	bl	127d4 <putc>
   13f48:	e3a03001 	mov	r3, #1	; 0x1
   13f4c:	e5c83000 	strb	r3, [r8]
   13f50:	eaffffa2 	b	13de0 <control_send+0x80>
   13f54:	e59d0004 	ldr	r0, [sp, #4]
   13f58:	e1a0100b 	mov	r1, fp
   13f5c:	ebffff24 	bl	13bf4 <real_send>
   13f60:	e3500003 	cmp	r0, #3	; 0x3
   13f64:	0affff9d 	beq	13de0 <control_send+0x80>
   13f68:	e3500000 	cmp	r0, #0	; 0x0
   13f6c:	1a00000a 	bne	13f9c <control_send+0x23c>
   13f70:	e5d93000 	ldrb	r3, [r9]
   13f74:	e3530000 	cmp	r3, #0	; 0x0
   13f78:	e5d43000 	ldrb	r3, [r4]
   13f7c:	15c90000 	strbne	r0, [r9]
   13f80:	e3530000 	cmp	r3, #0	; 0x0
   13f84:	13a03000 	movne	r3, #0	; 0x0
   13f88:	15c43000 	strbne	r3, [r4]
   13f8c:	12833001 	addne	r3, r3, #1	; 0x1
   13f90:	15c93000 	strbne	r3, [r9]
   13f94:	eaffff91 	b	13de0 <control_send+0x80>
   13f98:	e3a00000 	mov	r0, #0	; 0x0
   13f9c:	e28dd014 	add	sp, sp, #20	; 0x14
   13fa0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   13fa4:	3c00d281 	.word	0x3c00d281
   13fa8:	3c00d282 	.word	0x3c00d282
   13fac:	3c00d11c 	.word	0x3c00d11c
   13fb0:	3c00d284 	.word	0x3c00d284
   13fb4:	3c00d288 	.word	0x3c00d288
   13fb8:	3c00d28b 	.word	0x3c00d28b
   13fbc:	3c00d280 	.word	0x3c00d280
   13fc0:	3c00d289 	.word	0x3c00d289
   13fc4:	3c00d28a 	.word	0x3c00d28a
   13fc8:	3c00d28d 	.word	0x3c00d28d
   13fcc:	3c00d28c 	.word	0x3c00d28c
   13fd0:	3c00d728 	.word	0x3c00d728

00013fd4 <xyzModem_stream_terminate>:
   13fd4:	e92d4030 	push	{r4, r5, lr}
   13fd8:	e3500000 	cmp	r0, #0	; 0x0
   13fdc:	e24dd00c 	sub	sp, sp, #12	; 0xc
   13fe0:	e1a04001 	mov	r4, r1
   13fe4:	0a000020 	beq	1406c <xyzModem_stream_terminate+0x98>
   13fe8:	e59f4098 	ldr	r4, [pc, #152]	; 14088 <xyzModem_stream_terminate+0xb4>
   13fec:	e5943414 	ldr	r3, [r4, #1044]
   13ff0:	e2433001 	sub	r3, r3, #1	; 0x1
   13ff4:	e3530001 	cmp	r3, #1	; 0x1
   13ff8:	8a000020 	bhi	14080 <xyzModem_stream_terminate+0xac>
   13ffc:	e3a00018 	mov	r0, #24	; 0x18
   14000:	ebfff9f3 	bl	127d4 <putc>
   14004:	e3a00018 	mov	r0, #24	; 0x18
   14008:	ebfff9f1 	bl	127d4 <putc>
   1400c:	e3a00018 	mov	r0, #24	; 0x18
   14010:	ebfff9ef 	bl	127d4 <putc>
   14014:	e3a00018 	mov	r0, #24	; 0x18
   14018:	ebfff9ed 	bl	127d4 <putc>
   1401c:	e3a00008 	mov	r0, #8	; 0x8
   14020:	ebfff9eb 	bl	127d4 <putc>
   14024:	e3a00008 	mov	r0, #8	; 0x8
   14028:	ebfff9e9 	bl	127d4 <putc>
   1402c:	e3a00008 	mov	r0, #8	; 0x8
   14030:	ebfff9e7 	bl	127d4 <putc>
   14034:	e3a00008 	mov	r0, #8	; 0x8
   14038:	ebfff9e5 	bl	127d4 <putc>
   1403c:	e1a05004 	mov	r5, r4
   14040:	e28d4007 	add	r4, sp, #7	; 0x7
   14044:	e5953000 	ldr	r3, [r5]
   14048:	e1a01004 	mov	r1, r4
   1404c:	e5d30000 	ldrb	r0, [r3]
   14050:	ebffff2d 	bl	13d0c <CYGACC_COMM_IF_GETC_TIMEOUT>
   14054:	e3500000 	cmp	r0, #0	; 0x0
   14058:	1afffff9 	bne	14044 <xyzModem_stream_terminate+0x70>
   1405c:	e59f3024 	ldr	r3, [pc, #36]	; 14088 <xyzModem_stream_terminate+0xb4>
   14060:	e3a02001 	mov	r2, #1	; 0x1
   14064:	e583242c 	str	r2, [r3, #1068]
   14068:	ea000004 	b	14080 <xyzModem_stream_terminate+0xac>
   1406c:	e12fff34 	blx	r4
   14070:	e3500000 	cmp	r0, #0	; 0x0
   14074:	aafffffc 	bge	1406c <xyzModem_stream_terminate+0x98>
   14078:	e59f000c 	ldr	r0, [pc, #12]	; 1408c <xyzModem_stream_terminate+0xb8>
   1407c:	ebffe656 	bl	d9dc <udelay>
   14080:	e28dd00c 	add	sp, sp, #12	; 0xc
   14084:	e8bd8030 	pop	{r4, r5, pc}
   14088:	3c00d290 	.word	0x3c00d290
   1408c:	0003d090 	.word	0x0003d090

00014090 <xyzModem_get_hdr>:
   14090:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   14094:	e59f42a0 	ldr	r4, [pc, #672]	; 1433c <xyzModem_get_hdr+0x2ac>
   14098:	e24dd00c 	sub	sp, sp, #12	; 0xc
   1409c:	e5943430 	ldr	r3, [r4, #1072]
   140a0:	e3530000 	cmp	r3, #0	; 0x0
   140a4:	0a000003 	beq	140b8 <xyzModem_get_hdr+0x28>
   140a8:	e3a00006 	mov	r0, #6	; 0x6
   140ac:	ebfff9c8 	bl	127d4 <putc>
   140b0:	e3a03000 	mov	r3, #0	; 0x0
   140b4:	e5843430 	str	r3, [r4, #1072]
   140b8:	e3a04000 	mov	r4, #0	; 0x0
   140bc:	e59f6278 	ldr	r6, [pc, #632]	; 1433c <xyzModem_get_hdr+0x2ac>
   140c0:	e1a05004 	mov	r5, r4
   140c4:	e28d7007 	add	r7, sp, #7	; 0x7
   140c8:	e5963000 	ldr	r3, [r6]
   140cc:	e1a01007 	mov	r1, r7
   140d0:	e5d30000 	ldrb	r0, [r3]
   140d4:	ebffff0c 	bl	13d0c <CYGACC_COMM_IF_GETC_TIMEOUT>
   140d8:	e3500000 	cmp	r0, #0	; 0x0
   140dc:	e2855001 	add	r5, r5, #1	; 0x1
   140e0:	e59f2254 	ldr	r2, [pc, #596]	; 1433c <xyzModem_get_hdr+0x2ac>
   140e4:	0a000021 	beq	14170 <xyzModem_get_hdr+0xe0>
   140e8:	e5dd3007 	ldrb	r3, [sp, #7]
   140ec:	e3530002 	cmp	r3, #2	; 0x2
   140f0:	0a00000c 	beq	14128 <xyzModem_get_hdr+0x98>
   140f4:	8a000002 	bhi	14104 <xyzModem_get_hdr+0x74>
   140f8:	e3530001 	cmp	r3, #1	; 0x1
   140fc:	1afffff1 	bne	140c8 <xyzModem_get_hdr+0x38>
   14100:	ea000004 	b	14118 <xyzModem_get_hdr+0x88>
   14104:	e3530004 	cmp	r3, #4	; 0x4
   14108:	0a000012 	beq	14158 <xyzModem_get_hdr+0xc8>
   1410c:	e3530018 	cmp	r3, #24	; 0x18
   14110:	1affffec 	bne	140c8 <xyzModem_get_hdr+0x38>
   14114:	ea000007 	b	14138 <xyzModem_get_hdr+0xa8>
   14118:	e592341c 	ldr	r3, [r2, #1052]
   1411c:	e2833001 	add	r3, r3, #1	; 0x1
   14120:	e582341c 	str	r3, [r2, #1052]
   14124:	ea00007c 	b	1431c <xyzModem_get_hdr+0x28c>
   14128:	e5923420 	ldr	r3, [r2, #1056]
   1412c:	e2833001 	add	r3, r3, #1	; 0x1
   14130:	e5823420 	str	r3, [r2, #1056]
   14134:	ea000078 	b	1431c <xyzModem_get_hdr+0x28c>
   14138:	e5963424 	ldr	r3, [r6, #1060]
   1413c:	e2844001 	add	r4, r4, #1	; 0x1
   14140:	e2833001 	add	r3, r3, #1	; 0x1
   14144:	e3540003 	cmp	r4, #3	; 0x3
   14148:	e5863424 	str	r3, [r6, #1060]
   1414c:	1affffdd 	bne	140c8 <xyzModem_get_hdr+0x38>
   14150:	e3e00004 	mvn	r0, #4	; 0x4
   14154:	ea00006e 	b	14314 <xyzModem_get_hdr+0x284>
   14158:	e3550001 	cmp	r5, #1	; 0x1
   1415c:	1affffd9 	bne	140c8 <xyzModem_get_hdr+0x38>
   14160:	e3a00006 	mov	r0, #6	; 0x6
   14164:	ebfff99a 	bl	127d4 <putc>
   14168:	e3e00003 	mvn	r0, #3	; 0x3
   1416c:	ea000068 	b	14314 <xyzModem_get_hdr+0x284>
   14170:	e1a05002 	mov	r5, r2
   14174:	e28d4006 	add	r4, sp, #6	; 0x6
   14178:	e5953000 	ldr	r3, [r5]
   1417c:	e1a01004 	mov	r1, r4
   14180:	e5d30000 	ldrb	r0, [r3]
   14184:	ebfffee0 	bl	13d0c <CYGACC_COMM_IF_GETC_TIMEOUT>
   14188:	e3500000 	cmp	r0, #0	; 0x0
   1418c:	1afffff9 	bne	14178 <xyzModem_get_hdr+0xe8>
   14190:	e59f01a8 	ldr	r0, [pc, #424]	; 14340 <xyzModem_get_hdr+0x2b0>
   14194:	ebffe610 	bl	d9dc <udelay>
   14198:	ea00005a 	b	14308 <xyzModem_get_hdr+0x278>
   1419c:	e5943000 	ldr	r3, [r4]
   141a0:	e59f119c 	ldr	r1, [pc, #412]	; 14344 <xyzModem_get_hdr+0x2b4>
   141a4:	e5d30000 	ldrb	r0, [r3]
   141a8:	ebfffed7 	bl	13d0c <CYGACC_COMM_IF_GETC_TIMEOUT>
   141ac:	e3500000 	cmp	r0, #0	; 0x0
   141b0:	0a000054 	beq	14308 <xyzModem_get_hdr+0x278>
   141b4:	e28d5008 	add	r5, sp, #8	; 0x8
   141b8:	e5753001 	ldrb	r3, [r5, #-1]!
   141bc:	e2842004 	add	r2, r4, #4	; 0x4
   141c0:	e3530001 	cmp	r3, #1	; 0x1
   141c4:	03a03080 	moveq	r3, #128	; 0x80
   141c8:	13a03b01 	movne	r3, #1024	; 0x400
   141cc:	e5843410 	str	r3, [r4, #1040]
   141d0:	e5842404 	str	r2, [r4, #1028]
   141d4:	e1a07004 	mov	r7, r4
   141d8:	e3a04000 	mov	r4, #0	; 0x0
   141dc:	ea000008 	b	14204 <xyzModem_get_hdr+0x174>
   141e0:	e5973000 	ldr	r3, [r7]
   141e4:	e5d30000 	ldrb	r0, [r3]
   141e8:	ebfffec7 	bl	13d0c <CYGACC_COMM_IF_GETC_TIMEOUT>
   141ec:	e3500000 	cmp	r0, #0	; 0x0
   141f0:	e0872004 	add	r2, r7, r4
   141f4:	e2844001 	add	r4, r4, #1	; 0x1
   141f8:	0a000042 	beq	14308 <xyzModem_get_hdr+0x278>
   141fc:	e5dd3007 	ldrb	r3, [sp, #7]
   14200:	e5c23004 	strb	r3, [r2, #4]
   14204:	e5973410 	ldr	r3, [r7, #1040]
   14208:	e1a01005 	mov	r1, r5
   1420c:	e1540003 	cmp	r4, r3
   14210:	e59f6124 	ldr	r6, [pc, #292]	; 1433c <xyzModem_get_hdr+0x2ac>
   14214:	bafffff1 	blt	141e0 <xyzModem_get_hdr+0x150>
   14218:	e5963000 	ldr	r3, [r6]
   1421c:	e59f1124 	ldr	r1, [pc, #292]	; 14348 <xyzModem_get_hdr+0x2b8>
   14220:	e5d30000 	ldrb	r0, [r3]
   14224:	ebfffeb8 	bl	13d0c <CYGACC_COMM_IF_GETC_TIMEOUT>
   14228:	e3500000 	cmp	r0, #0	; 0x0
   1422c:	0a000035 	beq	14308 <xyzModem_get_hdr+0x278>
   14230:	e5963428 	ldr	r3, [r6, #1064]
   14234:	e3530000 	cmp	r3, #0	; 0x0
   14238:	0a000005 	beq	14254 <xyzModem_get_hdr+0x1c4>
   1423c:	e5963000 	ldr	r3, [r6]
   14240:	e59f1104 	ldr	r1, [pc, #260]	; 1434c <xyzModem_get_hdr+0x2bc>
   14244:	e5d30000 	ldrb	r0, [r3]
   14248:	ebfffeaf 	bl	13d0c <CYGACC_COMM_IF_GETC_TIMEOUT>
   1424c:	e3500000 	cmp	r0, #0	; 0x0
   14250:	0a00002c 	beq	14308 <xyzModem_get_hdr+0x278>
   14254:	e59f40e0 	ldr	r4, [pc, #224]	; 1433c <xyzModem_get_hdr+0x2ac>
   14258:	e5d43409 	ldrb	r3, [r4, #1033]
   1425c:	e5d42408 	ldrb	r2, [r4, #1032]
   14260:	e0233002 	eor	r3, r3, r2
   14264:	e35300ff 	cmp	r3, #255	; 0xff
   14268:	0a000009 	beq	14294 <xyzModem_get_hdr+0x204>
   1426c:	e1a05004 	mov	r5, r4
   14270:	e28d4006 	add	r4, sp, #6	; 0x6
   14274:	e5953000 	ldr	r3, [r5]
   14278:	e1a01004 	mov	r1, r4
   1427c:	e5d30000 	ldrb	r0, [r3]
   14280:	ebfffea1 	bl	13d0c <CYGACC_COMM_IF_GETC_TIMEOUT>
   14284:	e3500000 	cmp	r0, #0	; 0x0
   14288:	1afffff9 	bne	14274 <xyzModem_get_hdr+0x1e4>
   1428c:	e3e00005 	mvn	r0, #5	; 0x5
   14290:	ea00001f 	b	14314 <xyzModem_get_hdr+0x284>
   14294:	e5943428 	ldr	r3, [r4, #1064]
   14298:	e3530000 	cmp	r3, #0	; 0x0
   1429c:	01a02003 	moveq	r2, r3
   142a0:	01a0c004 	moveq	ip, r4
   142a4:	05940410 	ldreq	r0, [r4, #1040]
   142a8:	01a01003 	moveq	r1, r3
   142ac:	0a00000a 	beq	142dc <xyzModem_get_hdr+0x24c>
   142b0:	e2840004 	add	r0, r4, #4	; 0x4
   142b4:	e5941410 	ldr	r1, [r4, #1040]
   142b8:	eb000110 	bl	14700 <cyg_crc16>
   142bc:	e5d4340b 	ldrb	r3, [r4, #1035]
   142c0:	e5d4240a 	ldrb	r2, [r4, #1034]
   142c4:	e1833402 	orr	r3, r3, r2, lsl #8
   142c8:	e1500003 	cmp	r0, r3
   142cc:	ea00000a 	b	142fc <xyzModem_get_hdr+0x26c>
   142d0:	e5d33004 	ldrb	r3, [r3, #4]
   142d4:	e0813003 	add	r3, r1, r3
   142d8:	e6ff1073 	uxth	r1, r3
   142dc:	e1520000 	cmp	r2, r0
   142e0:	e08c3002 	add	r3, ip, r2
   142e4:	e2822001 	add	r2, r2, #1	; 0x1
   142e8:	bafffff8 	blt	142d0 <xyzModem_get_hdr+0x240>
   142ec:	e59f3048 	ldr	r3, [pc, #72]	; 1433c <xyzModem_get_hdr+0x2ac>
   142f0:	e5d3240a 	ldrb	r2, [r3, #1034]
   142f4:	e20130ff 	and	r3, r1, #255	; 0xff
   142f8:	e1520003 	cmp	r2, r3
   142fc:	13e00006 	mvnne	r0, #6	; 0x6
   14300:	1a000003 	bne	14314 <xyzModem_get_hdr+0x284>
   14304:	ea000001 	b	14310 <xyzModem_get_hdr+0x280>
   14308:	e3e00002 	mvn	r0, #2	; 0x2
   1430c:	ea000000 	b	14314 <xyzModem_get_hdr+0x284>
   14310:	e3a00000 	mov	r0, #0	; 0x0
   14314:	e28dd00c 	add	sp, sp, #12	; 0xc
   14318:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   1431c:	e59f4018 	ldr	r4, [pc, #24]	; 1433c <xyzModem_get_hdr+0x2ac>
   14320:	e59f1028 	ldr	r1, [pc, #40]	; 14350 <xyzModem_get_hdr+0x2c0>
   14324:	e5943000 	ldr	r3, [r4]
   14328:	e5d30000 	ldrb	r0, [r3]
   1432c:	ebfffe76 	bl	13d0c <CYGACC_COMM_IF_GETC_TIMEOUT>
   14330:	e3500000 	cmp	r0, #0	; 0x0
   14334:	0afffff3 	beq	14308 <xyzModem_get_hdr+0x278>
   14338:	eaffff97 	b	1419c <xyzModem_get_hdr+0x10c>
   1433c:	3c00d290 	.word	0x3c00d290
   14340:	0003d090 	.word	0x0003d090
   14344:	3c00d699 	.word	0x3c00d699
   14348:	3c00d69a 	.word	0x3c00d69a
   1434c:	3c00d69b 	.word	0x3c00d69b
   14350:	3c00d698 	.word	0x3c00d698

00014354 <xyzModem_stream_read>:
   14354:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   14358:	e59f5240 	ldr	r5, [pc, #576]	; 145a0 <xyzModem_stream_read+0x24c>
   1435c:	e24dd004 	sub	sp, sp, #4	; 0x4
   14360:	e1a08000 	mov	r8, r0
   14364:	e1a07001 	mov	r7, r1
   14368:	e1a0b002 	mov	fp, r2
   1436c:	e3e06004 	mvn	r6, #4	; 0x4
   14370:	e3a0a000 	mov	sl, #0	; 0x0
   14374:	e1a09005 	mov	r9, r5
   14378:	ea00007e 	b	14578 <xyzModem_stream_read+0x224>
   1437c:	e5953410 	ldr	r3, [r5, #1040]
   14380:	e3530000 	cmp	r3, #0	; 0x0
   14384:	03a04014 	moveq	r4, #20	; 0x14
   14388:	0a00005d 	beq	14504 <xyzModem_stream_read+0x1b0>
   1438c:	ea000065 	b	14528 <xyzModem_stream_read+0x1d4>
   14390:	ebffff3e 	bl	14090 <xyzModem_get_hdr>
   14394:	e2506000 	subs	r6, r0, #0	; 0x0
   14398:	1a000039 	bne	14484 <xyzModem_stream_read+0x130>
   1439c:	e5d5240c 	ldrb	r2, [r5, #1036]
   143a0:	e5d51408 	ldrb	r1, [r5, #1032]
   143a4:	e2423001 	sub	r3, r2, #1	; 0x1
   143a8:	e1510002 	cmp	r1, r2
   143ac:	e20330ff 	and	r3, r3, #255	; 0xff
   143b0:	1a00002d 	bne	1446c <xyzModem_stream_read+0x118>
   143b4:	e599c414 	ldr	ip, [r9, #1044]
   143b8:	e2813001 	add	r3, r1, #1	; 0x1
   143bc:	e5c9340c 	strb	r3, [r9, #1036]
   143c0:	e35c0001 	cmp	ip, #1	; 0x1
   143c4:	e3a03001 	mov	r3, #1	; 0x1
   143c8:	e5893430 	str	r3, [r9, #1072]
   143cc:	0a000002 	beq	143dc <xyzModem_stream_read+0x88>
   143d0:	e5993434 	ldr	r3, [r9, #1076]
   143d4:	e3530000 	cmp	r3, #0	; 0x0
   143d8:	1a000017 	bne	1443c <xyzModem_stream_read+0xe8>
   143dc:	e5953410 	ldr	r3, [r5, #1040]
   143e0:	e5950404 	ldr	r0, [r5, #1028]
   143e4:	e0802003 	add	r2, r0, r3
   143e8:	e5523001 	ldrb	r3, [r2, #-1]
   143ec:	e353001a 	cmp	r3, #26	; 0x1a
   143f0:	1a00000f 	bne	14434 <xyzModem_stream_read+0xe0>
   143f4:	e5523002 	ldrb	r3, [r2, #-2]
   143f8:	e353001a 	cmp	r3, #26	; 0x1a
   143fc:	1a00000c 	bne	14434 <xyzModem_stream_read+0xe0>
   14400:	e5523003 	ldrb	r3, [r2, #-3]
   14404:	e353001a 	cmp	r3, #26	; 0x1a
   14408:	1a000009 	bne	14434 <xyzModem_stream_read+0xe0>
   1440c:	ea000000 	b	14414 <xyzModem_stream_read+0xc0>
   14410:	e5851410 	str	r1, [r5, #1040]
   14414:	e5953410 	ldr	r3, [r5, #1040]
   14418:	e3530000 	cmp	r3, #0	; 0x0
   1441c:	e0802003 	add	r2, r0, r3
   14420:	e2431001 	sub	r1, r3, #1	; 0x1
   14424:	0a000002 	beq	14434 <xyzModem_stream_read+0xe0>
   14428:	e5523001 	ldrb	r3, [r2, #-1]
   1442c:	e353001a 	cmp	r3, #26	; 0x1a
   14430:	0afffff6 	beq	14410 <xyzModem_stream_read+0xbc>
   14434:	e35c0001 	cmp	ip, #1	; 0x1
   14438:	0a00003a 	beq	14528 <xyzModem_stream_read+0x1d4>
   1443c:	e5951434 	ldr	r1, [r5, #1076]
   14440:	e3510000 	cmp	r1, #0	; 0x0
   14444:	0a000037 	beq	14528 <xyzModem_stream_read+0x1d4>
   14448:	e5953438 	ldr	r3, [r5, #1080]
   1444c:	e5950410 	ldr	r0, [r5, #1040]
   14450:	e0802003 	add	r2, r0, r3
   14454:	e1520001 	cmp	r2, r1
   14458:	80803001 	addhi	r3, r0, r1
   1445c:	80623003 	rsbhi	r3, r2, r3
   14460:	e5852438 	str	r2, [r5, #1080]
   14464:	85853410 	strhi	r3, [r5, #1040]
   14468:	ea00002e 	b	14528 <xyzModem_stream_read+0x1d4>
   1446c:	e1510003 	cmp	r1, r3
   14470:	13e06007 	mvnne	r6, #7	; 0x7
   14474:	1a00001a 	bne	144e4 <xyzModem_stream_read+0x190>
   14478:	e3a00006 	mov	r0, #6	; 0x6
   1447c:	ebfff8d4 	bl	127d4 <putc>
   14480:	ea00001f 	b	14504 <xyzModem_stream_read+0x1b0>
   14484:	e3760005 	cmn	r6, #5	; 0x5
   14488:	0a000021 	beq	14514 <xyzModem_stream_read+0x1c0>
   1448c:	e3760004 	cmn	r6, #4	; 0x4
   14490:	1a000013 	bne	144e4 <xyzModem_stream_read+0x190>
   14494:	e3a00006 	mov	r0, #6	; 0x6
   14498:	ebfff8cd 	bl	127d4 <putc>
   1449c:	e5953414 	ldr	r3, [r5, #1044]
   144a0:	e3530002 	cmp	r3, #2	; 0x2
   144a4:	1a00000b 	bne	144d8 <xyzModem_stream_read+0x184>
   144a8:	e5950428 	ldr	r0, [r5, #1064]
   144ac:	e3500000 	cmp	r0, #0	; 0x0
   144b0:	13a00043 	movne	r0, #67	; 0x43
   144b4:	03a00015 	moveq	r0, #21	; 0x15
   144b8:	ebfff8c5 	bl	127d4 <putc>
   144bc:	e5953418 	ldr	r3, [r5, #1048]
   144c0:	e2833001 	add	r3, r3, #1	; 0x1
   144c4:	e5853418 	str	r3, [r5, #1048]
   144c8:	ebfffef0 	bl	14090 <xyzModem_get_hdr>
   144cc:	e1a06000 	mov	r6, r0
   144d0:	e3a00006 	mov	r0, #6	; 0x6
   144d4:	ebfff8be 	bl	127d4 <putc>
   144d8:	e3a03001 	mov	r3, #1	; 0x1
   144dc:	e585342c 	str	r3, [r5, #1068]
   144e0:	ea000009 	b	1450c <xyzModem_stream_read+0x1b8>
   144e4:	e5950428 	ldr	r0, [r5, #1064]
   144e8:	e3500000 	cmp	r0, #0	; 0x0
   144ec:	13a00043 	movne	r0, #67	; 0x43
   144f0:	03a00015 	moveq	r0, #21	; 0x15
   144f4:	ebfff8b6 	bl	127d4 <putc>
   144f8:	e5953418 	ldr	r3, [r5, #1048]
   144fc:	e2833001 	add	r3, r3, #1	; 0x1
   14500:	e5853418 	str	r3, [r5, #1048]
   14504:	e2544001 	subs	r4, r4, #1	; 0x1
   14508:	2affffa0 	bcs	14390 <xyzModem_stream_read+0x3c>
   1450c:	e3560000 	cmp	r6, #0	; 0x0
   14510:	aa000004 	bge	14528 <xyzModem_stream_read+0x1d4>
   14514:	e59f3084 	ldr	r3, [pc, #132]	; 145a0 <xyzModem_stream_read+0x24c>
   14518:	e3e02000 	mvn	r2, #0	; 0x0
   1451c:	e58b6000 	str	r6, [fp]
   14520:	e5832410 	str	r2, [r3, #1040]
   14524:	ea00001a 	b	14594 <xyzModem_stream_read+0x240>
   14528:	e595342c 	ldr	r3, [r5, #1068]
   1452c:	e3530000 	cmp	r3, #0	; 0x0
   14530:	1a000010 	bne	14578 <xyzModem_stream_read+0x224>
   14534:	e5954410 	ldr	r4, [r5, #1040]
   14538:	e1a00008 	mov	r0, r8
   1453c:	e1570004 	cmp	r7, r4
   14540:	b1a04007 	movlt	r4, r7
   14544:	a1a04004 	movge	r4, r4
   14548:	e1a02004 	mov	r2, r4
   1454c:	e5951404 	ldr	r1, [r5, #1028]
   14550:	ebffb3ff 	bl	1554 <memcpy>
   14554:	e5953410 	ldr	r3, [r5, #1040]
   14558:	e5952404 	ldr	r2, [r5, #1028]
   1455c:	e0643003 	rsb	r3, r4, r3
   14560:	e0822004 	add	r2, r2, r4
   14564:	e5853410 	str	r3, [r5, #1040]
   14568:	e5852404 	str	r2, [r5, #1028]
   1456c:	e0647007 	rsb	r7, r4, r7
   14570:	e0888004 	add	r8, r8, r4
   14574:	e08aa004 	add	sl, sl, r4
   14578:	e595342c 	ldr	r3, [r5, #1068]
   1457c:	e2733001 	rsbs	r3, r3, #1	; 0x1
   14580:	33a03000 	movcc	r3, #0	; 0x0
   14584:	e3570000 	cmp	r7, #0	; 0x0
   14588:	d3a03000 	movle	r3, #0	; 0x0
   1458c:	e3530000 	cmp	r3, #0	; 0x0
   14590:	1affff79 	bne	1437c <xyzModem_stream_read+0x28>
   14594:	e1a0000a 	mov	r0, sl
   14598:	e28dd004 	add	sp, sp, #4	; 0x4
   1459c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   145a0:	3c00d290 	.word	0x3c00d290

000145a4 <xyzModem_stream_open>:
   145a4:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
   145a8:	e59f4140 	ldr	r4, [pc, #320]	; 146f0 <xyzModem_stream_open+0x14c>
   145ac:	e24dd00c 	sub	sp, sp, #12	; 0xc
   145b0:	e3a05000 	mov	r5, #0	; 0x0
   145b4:	e28d3008 	add	r3, sp, #8	; 0x8
   145b8:	e3a02001 	mov	r2, #1	; 0x1
   145bc:	e5235004 	str	r5, [r3, #-4]!
   145c0:	e5842428 	str	r2, [r4, #1064]
   145c4:	e5845410 	str	r5, [r4, #1040]
   145c8:	e584542c 	str	r5, [r4, #1068]
   145cc:	e5845430 	str	r5, [r4, #1072]
   145d0:	e5902004 	ldr	r2, [r0, #4]
   145d4:	e5843000 	str	r3, [r4]
   145d8:	e3a00043 	mov	r0, #67	; 0x43
   145dc:	e5842414 	str	r2, [r4, #1044]
   145e0:	e5845418 	str	r5, [r4, #1048]
   145e4:	e584541c 	str	r5, [r4, #1052]
   145e8:	e5845420 	str	r5, [r4, #1056]
   145ec:	e5845424 	str	r5, [r4, #1060]
   145f0:	e5845438 	str	r5, [r4, #1080]
   145f4:	e5845434 	str	r5, [r4, #1076]
   145f8:	e1a08001 	mov	r8, r1
   145fc:	ebfff874 	bl	127d4 <putc>
   14600:	e5943414 	ldr	r3, [r4, #1044]
   14604:	e3530001 	cmp	r3, #1	; 0x1
   14608:	05c4340c 	strbeq	r3, [r4, #1036]
   1460c:	01a00005 	moveq	r0, r5
   14610:	0a000034 	beq	146e8 <xyzModem_stream_open+0x144>
   14614:	e1a07004 	mov	r7, r4
   14618:	e3a06014 	mov	r6, #20	; 0x14
   1461c:	e3a0400a 	mov	r4, #10	; 0xa
   14620:	e1a0a005 	mov	sl, r5
   14624:	ea00002b 	b	146d8 <xyzModem_stream_open+0x134>
   14628:	ebfffe98 	bl	14090 <xyzModem_get_hdr>
   1462c:	e2505000 	subs	r5, r0, #0	; 0x0
   14630:	1a000016 	bne	14690 <xyzModem_stream_open+0xec>
   14634:	e59f20b4 	ldr	r2, [pc, #180]	; 146f0 <xyzModem_stream_open+0x14c>
   14638:	e5d23408 	ldrb	r3, [r2, #1032]
   1463c:	e3530000 	cmp	r3, #0	; 0x0
   14640:	1a00000b 	bne	14674 <xyzModem_stream_open+0xd0>
   14644:	e1a03002 	mov	r3, r2
   14648:	e5930404 	ldr	r0, [r3, #1028]
   1464c:	e4d02001 	ldrb	r2, [r0], #1
   14650:	e3520000 	cmp	r2, #0	; 0x0
   14654:	e5830404 	str	r0, [r3, #1028]
   14658:	1afffffa 	bne	14648 <xyzModem_stream_open+0xa4>
   1465c:	e59f3090 	ldr	r3, [pc, #144]	; 146f4 <xyzModem_stream_open+0x150>
   14660:	e59f1090 	ldr	r1, [pc, #144]	; 146f8 <xyzModem_stream_open+0x154>
   14664:	ebfffb5a 	bl	133d4 <parse_num>
   14668:	e59f3080 	ldr	r3, [pc, #128]	; 146f0 <xyzModem_stream_open+0x14c>
   1466c:	e3a02001 	mov	r2, #1	; 0x1
   14670:	e5832430 	str	r2, [r3, #1072]
   14674:	e59f3074 	ldr	r3, [pc, #116]	; 146f0 <xyzModem_stream_open+0x14c>
   14678:	e3a01000 	mov	r1, #0	; 0x0
   1467c:	e3a02001 	mov	r2, #1	; 0x1
   14680:	e5c3240c 	strb	r2, [r3, #1036]
   14684:	e5831410 	str	r1, [r3, #1040]
   14688:	e1a00001 	mov	r0, r1
   1468c:	ea000015 	b	146e8 <xyzModem_stream_open+0x144>
   14690:	e3750003 	cmn	r5, #3	; 0x3
   14694:	1a00000d 	bne	146d0 <xyzModem_stream_open+0x12c>
   14698:	e2444001 	sub	r4, r4, #1	; 0x1
   1469c:	e3540000 	cmp	r4, #0	; 0x0
   146a0:	e59f0054 	ldr	r0, [pc, #84]	; 146fc <xyzModem_stream_open+0x158>
   146a4:	d587a428 	strle	sl, [r7, #1064]
   146a8:	ebffe4cb 	bl	d9dc <udelay>
   146ac:	e5970428 	ldr	r0, [r7, #1064]
   146b0:	e3500000 	cmp	r0, #0	; 0x0
   146b4:	13a00043 	movne	r0, #67	; 0x43
   146b8:	03a00015 	moveq	r0, #21	; 0x15
   146bc:	ebfff844 	bl	127d4 <putc>
   146c0:	e5973418 	ldr	r3, [r7, #1048]
   146c4:	e2833001 	add	r3, r3, #1	; 0x1
   146c8:	e5873418 	str	r3, [r7, #1048]
   146cc:	ea000001 	b	146d8 <xyzModem_stream_open+0x134>
   146d0:	e3750005 	cmn	r5, #5	; 0x5
   146d4:	0a000001 	beq	146e0 <xyzModem_stream_open+0x13c>
   146d8:	e2566001 	subs	r6, r6, #1	; 0x1
   146dc:	2affffd1 	bcs	14628 <xyzModem_stream_open+0x84>
   146e0:	e5885000 	str	r5, [r8]
   146e4:	e3e00000 	mvn	r0, #0	; 0x0
   146e8:	e28dd00c 	add	sp, sp, #12	; 0xc
   146ec:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
   146f0:	3c00d290 	.word	0x3c00d290
   146f4:	00018c25 	.word	0x00018c25
   146f8:	3c00d6c4 	.word	0x3c00d6c4
   146fc:	0007a120 	.word	0x0007a120

00014700 <cyg_crc16>:
   14700:	e3a02000 	mov	r2, #0	; 0x0
   14704:	e92d4010 	push	{r4, lr}
   14708:	e1a0c000 	mov	ip, r0
   1470c:	e59f402c 	ldr	r4, [pc, #44]	; 14740 <cyg_crc16+0x40>
   14710:	e1a00002 	mov	r0, r2
   14714:	ea000006 	b	14734 <cyg_crc16+0x34>
   14718:	e7dc3002 	ldrb	r3, [ip, r2]
   1471c:	e2822001 	add	r2, r2, #1	; 0x1
   14720:	e0233420 	eor	r3, r3, r0, lsr #8
   14724:	e1a03083 	lsl	r3, r3, #1
   14728:	e19430b3 	ldrh	r3, [r4, r3]
   1472c:	e0233400 	eor	r3, r3, r0, lsl #8
   14730:	e6ff0073 	uxth	r0, r3
   14734:	e1520001 	cmp	r2, r1
   14738:	bafffff6 	blt	14718 <cyg_crc16+0x18>
   1473c:	e8bd8010 	pop	{r4, pc}
   14740:	00015d4c 	.word	0x00015d4c

00014744 <ss_jtag_en>:
   14744:	e59f300c 	ldr	r3, [pc, #12]	; 14758 <ss_jtag_en+0x14>
   14748:	e2500000 	subs	r0, r0, #0	; 0x0
   1474c:	13a00001 	movne	r0, #1	; 0x1
   14750:	e58308b8 	str	r0, [r3, #2232]
   14754:	e12fff1e 	bx	lr
   14758:	21e08000 	.word	0x21e08000

0001475c <raise>:
   1475c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   14760:	e1a01000 	mov	r1, r0
   14764:	e24dd004 	sub	sp, sp, #4	; 0x4
   14768:	e59f0014 	ldr	r0, [pc, #20]	; 14784 <raise+0x28>
   1476c:	e59f3014 	ldr	r3, [pc, #20]	; 14788 <raise+0x2c>
   14770:	e1a0e00f 	mov	lr, pc
   14774:	e593f000 	ldr	pc, [r3]
   14778:	e3a00000 	mov	r0, #0	; 0x0
   1477c:	e28dd004 	add	sp, sp, #4	; 0x4
   14780:	e8bd8000 	pop	{pc}
   14784:	00018ec1 	.word	0x00018ec1
   14788:	3c00d6cc 	.word	0x3c00d6cc

0001478c <__aeabi_uidiv>:
   1478c:	e2512001 	subs	r2, r1, #1	; 0x1
   14790:	012fff1e 	bxeq	lr
   14794:	3a000074 	bcc	1496c <__aeabi_uidiv+0x1e0>
   14798:	e1500001 	cmp	r0, r1
   1479c:	9a00006b 	bls	14950 <__aeabi_uidiv+0x1c4>
   147a0:	e1110002 	tst	r1, r2
   147a4:	0a00006c 	beq	1495c <__aeabi_uidiv+0x1d0>
   147a8:	e16f3f10 	clz	r3, r0
   147ac:	e16f2f11 	clz	r2, r1
   147b0:	e0423003 	sub	r3, r2, r3
   147b4:	e273301f 	rsbs	r3, r3, #31	; 0x1f
   147b8:	10833083 	addne	r3, r3, r3, lsl #1
   147bc:	e3a02000 	mov	r2, #0	; 0x0
   147c0:	108ff103 	addne	pc, pc, r3, lsl #2
   147c4:	e1a00000 	nop			(mov r0,r0)
   147c8:	e1500f81 	cmp	r0, r1, lsl #31
   147cc:	e0a22002 	adc	r2, r2, r2
   147d0:	20400f81 	subcs	r0, r0, r1, lsl #31
   147d4:	e1500f01 	cmp	r0, r1, lsl #30
   147d8:	e0a22002 	adc	r2, r2, r2
   147dc:	20400f01 	subcs	r0, r0, r1, lsl #30
   147e0:	e1500e81 	cmp	r0, r1, lsl #29
   147e4:	e0a22002 	adc	r2, r2, r2
   147e8:	20400e81 	subcs	r0, r0, r1, lsl #29
   147ec:	e1500e01 	cmp	r0, r1, lsl #28
   147f0:	e0a22002 	adc	r2, r2, r2
   147f4:	20400e01 	subcs	r0, r0, r1, lsl #28
   147f8:	e1500d81 	cmp	r0, r1, lsl #27
   147fc:	e0a22002 	adc	r2, r2, r2
   14800:	20400d81 	subcs	r0, r0, r1, lsl #27
   14804:	e1500d01 	cmp	r0, r1, lsl #26
   14808:	e0a22002 	adc	r2, r2, r2
   1480c:	20400d01 	subcs	r0, r0, r1, lsl #26
   14810:	e1500c81 	cmp	r0, r1, lsl #25
   14814:	e0a22002 	adc	r2, r2, r2
   14818:	20400c81 	subcs	r0, r0, r1, lsl #25
   1481c:	e1500c01 	cmp	r0, r1, lsl #24
   14820:	e0a22002 	adc	r2, r2, r2
   14824:	20400c01 	subcs	r0, r0, r1, lsl #24
   14828:	e1500b81 	cmp	r0, r1, lsl #23
   1482c:	e0a22002 	adc	r2, r2, r2
   14830:	20400b81 	subcs	r0, r0, r1, lsl #23
   14834:	e1500b01 	cmp	r0, r1, lsl #22
   14838:	e0a22002 	adc	r2, r2, r2
   1483c:	20400b01 	subcs	r0, r0, r1, lsl #22
   14840:	e1500a81 	cmp	r0, r1, lsl #21
   14844:	e0a22002 	adc	r2, r2, r2
   14848:	20400a81 	subcs	r0, r0, r1, lsl #21
   1484c:	e1500a01 	cmp	r0, r1, lsl #20
   14850:	e0a22002 	adc	r2, r2, r2
   14854:	20400a01 	subcs	r0, r0, r1, lsl #20
   14858:	e1500981 	cmp	r0, r1, lsl #19
   1485c:	e0a22002 	adc	r2, r2, r2
   14860:	20400981 	subcs	r0, r0, r1, lsl #19
   14864:	e1500901 	cmp	r0, r1, lsl #18
   14868:	e0a22002 	adc	r2, r2, r2
   1486c:	20400901 	subcs	r0, r0, r1, lsl #18
   14870:	e1500881 	cmp	r0, r1, lsl #17
   14874:	e0a22002 	adc	r2, r2, r2
   14878:	20400881 	subcs	r0, r0, r1, lsl #17
   1487c:	e1500801 	cmp	r0, r1, lsl #16
   14880:	e0a22002 	adc	r2, r2, r2
   14884:	20400801 	subcs	r0, r0, r1, lsl #16
   14888:	e1500781 	cmp	r0, r1, lsl #15
   1488c:	e0a22002 	adc	r2, r2, r2
   14890:	20400781 	subcs	r0, r0, r1, lsl #15
   14894:	e1500701 	cmp	r0, r1, lsl #14
   14898:	e0a22002 	adc	r2, r2, r2
   1489c:	20400701 	subcs	r0, r0, r1, lsl #14
   148a0:	e1500681 	cmp	r0, r1, lsl #13
   148a4:	e0a22002 	adc	r2, r2, r2
   148a8:	20400681 	subcs	r0, r0, r1, lsl #13
   148ac:	e1500601 	cmp	r0, r1, lsl #12
   148b0:	e0a22002 	adc	r2, r2, r2
   148b4:	20400601 	subcs	r0, r0, r1, lsl #12
   148b8:	e1500581 	cmp	r0, r1, lsl #11
   148bc:	e0a22002 	adc	r2, r2, r2
   148c0:	20400581 	subcs	r0, r0, r1, lsl #11
   148c4:	e1500501 	cmp	r0, r1, lsl #10
   148c8:	e0a22002 	adc	r2, r2, r2
   148cc:	20400501 	subcs	r0, r0, r1, lsl #10
   148d0:	e1500481 	cmp	r0, r1, lsl #9
   148d4:	e0a22002 	adc	r2, r2, r2
   148d8:	20400481 	subcs	r0, r0, r1, lsl #9
   148dc:	e1500401 	cmp	r0, r1, lsl #8
   148e0:	e0a22002 	adc	r2, r2, r2
   148e4:	20400401 	subcs	r0, r0, r1, lsl #8
   148e8:	e1500381 	cmp	r0, r1, lsl #7
   148ec:	e0a22002 	adc	r2, r2, r2
   148f0:	20400381 	subcs	r0, r0, r1, lsl #7
   148f4:	e1500301 	cmp	r0, r1, lsl #6
   148f8:	e0a22002 	adc	r2, r2, r2
   148fc:	20400301 	subcs	r0, r0, r1, lsl #6
   14900:	e1500281 	cmp	r0, r1, lsl #5
   14904:	e0a22002 	adc	r2, r2, r2
   14908:	20400281 	subcs	r0, r0, r1, lsl #5
   1490c:	e1500201 	cmp	r0, r1, lsl #4
   14910:	e0a22002 	adc	r2, r2, r2
   14914:	20400201 	subcs	r0, r0, r1, lsl #4
   14918:	e1500181 	cmp	r0, r1, lsl #3
   1491c:	e0a22002 	adc	r2, r2, r2
   14920:	20400181 	subcs	r0, r0, r1, lsl #3
   14924:	e1500101 	cmp	r0, r1, lsl #2
   14928:	e0a22002 	adc	r2, r2, r2
   1492c:	20400101 	subcs	r0, r0, r1, lsl #2
   14930:	e1500081 	cmp	r0, r1, lsl #1
   14934:	e0a22002 	adc	r2, r2, r2
   14938:	20400081 	subcs	r0, r0, r1, lsl #1
   1493c:	e1500001 	cmp	r0, r1
   14940:	e0a22002 	adc	r2, r2, r2
   14944:	20400001 	subcs	r0, r0, r1
   14948:	e1a00002 	mov	r0, r2
   1494c:	e12fff1e 	bx	lr
   14950:	03a00001 	moveq	r0, #1	; 0x1
   14954:	13a00000 	movne	r0, #0	; 0x0
   14958:	e12fff1e 	bx	lr
   1495c:	e16f2f11 	clz	r2, r1
   14960:	e262201f 	rsb	r2, r2, #31	; 0x1f
   14964:	e1a00230 	lsr	r0, r0, r2
   14968:	e12fff1e 	bx	lr
   1496c:	e52de008 	str	lr, [sp, #-8]!
   14970:	eb0002cb 	bl	154a4 <__div0>
   14974:	e3a00000 	mov	r0, #0	; 0x0
   14978:	e49df008 	ldr	pc, [sp], #8

0001497c <__aeabi_uidivmod>:
   1497c:	e92d4003 	push	{r0, r1, lr}
   14980:	ebffff81 	bl	1478c <__aeabi_uidiv>
   14984:	e8bd4006 	pop	{r1, r2, lr}
   14988:	e0030092 	mul	r3, r2, r0
   1498c:	e0411003 	sub	r1, r1, r3
   14990:	e12fff1e 	bx	lr

00014994 <__aeabi_idiv>:
   14994:	e3510000 	cmp	r1, #0	; 0x0
   14998:	e020c001 	eor	ip, r0, r1
   1499c:	0a000080 	beq	14ba4 <__aeabi_idiv+0x210>
   149a0:	42611000 	rsbmi	r1, r1, #0	; 0x0
   149a4:	e2512001 	subs	r2, r1, #1	; 0x1
   149a8:	0a000070 	beq	14b70 <__aeabi_idiv+0x1dc>
   149ac:	e1b03000 	movs	r3, r0
   149b0:	42603000 	rsbmi	r3, r0, #0	; 0x0
   149b4:	e1530001 	cmp	r3, r1
   149b8:	9a00006f 	bls	14b7c <__aeabi_idiv+0x1e8>
   149bc:	e1110002 	tst	r1, r2
   149c0:	0a000071 	beq	14b8c <__aeabi_idiv+0x1f8>
   149c4:	e16f2f13 	clz	r2, r3
   149c8:	e16f0f11 	clz	r0, r1
   149cc:	e0402002 	sub	r2, r0, r2
   149d0:	e272201f 	rsbs	r2, r2, #31	; 0x1f
   149d4:	10822082 	addne	r2, r2, r2, lsl #1
   149d8:	e3a00000 	mov	r0, #0	; 0x0
   149dc:	108ff102 	addne	pc, pc, r2, lsl #2
   149e0:	e1a00000 	nop			(mov r0,r0)
   149e4:	e1530f81 	cmp	r3, r1, lsl #31
   149e8:	e0a00000 	adc	r0, r0, r0
   149ec:	20433f81 	subcs	r3, r3, r1, lsl #31
   149f0:	e1530f01 	cmp	r3, r1, lsl #30
   149f4:	e0a00000 	adc	r0, r0, r0
   149f8:	20433f01 	subcs	r3, r3, r1, lsl #30
   149fc:	e1530e81 	cmp	r3, r1, lsl #29
   14a00:	e0a00000 	adc	r0, r0, r0
   14a04:	20433e81 	subcs	r3, r3, r1, lsl #29
   14a08:	e1530e01 	cmp	r3, r1, lsl #28
   14a0c:	e0a00000 	adc	r0, r0, r0
   14a10:	20433e01 	subcs	r3, r3, r1, lsl #28
   14a14:	e1530d81 	cmp	r3, r1, lsl #27
   14a18:	e0a00000 	adc	r0, r0, r0
   14a1c:	20433d81 	subcs	r3, r3, r1, lsl #27
   14a20:	e1530d01 	cmp	r3, r1, lsl #26
   14a24:	e0a00000 	adc	r0, r0, r0
   14a28:	20433d01 	subcs	r3, r3, r1, lsl #26
   14a2c:	e1530c81 	cmp	r3, r1, lsl #25
   14a30:	e0a00000 	adc	r0, r0, r0
   14a34:	20433c81 	subcs	r3, r3, r1, lsl #25
   14a38:	e1530c01 	cmp	r3, r1, lsl #24
   14a3c:	e0a00000 	adc	r0, r0, r0
   14a40:	20433c01 	subcs	r3, r3, r1, lsl #24
   14a44:	e1530b81 	cmp	r3, r1, lsl #23
   14a48:	e0a00000 	adc	r0, r0, r0
   14a4c:	20433b81 	subcs	r3, r3, r1, lsl #23
   14a50:	e1530b01 	cmp	r3, r1, lsl #22
   14a54:	e0a00000 	adc	r0, r0, r0
   14a58:	20433b01 	subcs	r3, r3, r1, lsl #22
   14a5c:	e1530a81 	cmp	r3, r1, lsl #21
   14a60:	e0a00000 	adc	r0, r0, r0
   14a64:	20433a81 	subcs	r3, r3, r1, lsl #21
   14a68:	e1530a01 	cmp	r3, r1, lsl #20
   14a6c:	e0a00000 	adc	r0, r0, r0
   14a70:	20433a01 	subcs	r3, r3, r1, lsl #20
   14a74:	e1530981 	cmp	r3, r1, lsl #19
   14a78:	e0a00000 	adc	r0, r0, r0
   14a7c:	20433981 	subcs	r3, r3, r1, lsl #19
   14a80:	e1530901 	cmp	r3, r1, lsl #18
   14a84:	e0a00000 	adc	r0, r0, r0
   14a88:	20433901 	subcs	r3, r3, r1, lsl #18
   14a8c:	e1530881 	cmp	r3, r1, lsl #17
   14a90:	e0a00000 	adc	r0, r0, r0
   14a94:	20433881 	subcs	r3, r3, r1, lsl #17
   14a98:	e1530801 	cmp	r3, r1, lsl #16
   14a9c:	e0a00000 	adc	r0, r0, r0
   14aa0:	20433801 	subcs	r3, r3, r1, lsl #16
   14aa4:	e1530781 	cmp	r3, r1, lsl #15
   14aa8:	e0a00000 	adc	r0, r0, r0
   14aac:	20433781 	subcs	r3, r3, r1, lsl #15
   14ab0:	e1530701 	cmp	r3, r1, lsl #14
   14ab4:	e0a00000 	adc	r0, r0, r0
   14ab8:	20433701 	subcs	r3, r3, r1, lsl #14
   14abc:	e1530681 	cmp	r3, r1, lsl #13
   14ac0:	e0a00000 	adc	r0, r0, r0
   14ac4:	20433681 	subcs	r3, r3, r1, lsl #13
   14ac8:	e1530601 	cmp	r3, r1, lsl #12
   14acc:	e0a00000 	adc	r0, r0, r0
   14ad0:	20433601 	subcs	r3, r3, r1, lsl #12
   14ad4:	e1530581 	cmp	r3, r1, lsl #11
   14ad8:	e0a00000 	adc	r0, r0, r0
   14adc:	20433581 	subcs	r3, r3, r1, lsl #11
   14ae0:	e1530501 	cmp	r3, r1, lsl #10
   14ae4:	e0a00000 	adc	r0, r0, r0
   14ae8:	20433501 	subcs	r3, r3, r1, lsl #10
   14aec:	e1530481 	cmp	r3, r1, lsl #9
   14af0:	e0a00000 	adc	r0, r0, r0
   14af4:	20433481 	subcs	r3, r3, r1, lsl #9
   14af8:	e1530401 	cmp	r3, r1, lsl #8
   14afc:	e0a00000 	adc	r0, r0, r0
   14b00:	20433401 	subcs	r3, r3, r1, lsl #8
   14b04:	e1530381 	cmp	r3, r1, lsl #7
   14b08:	e0a00000 	adc	r0, r0, r0
   14b0c:	20433381 	subcs	r3, r3, r1, lsl #7
   14b10:	e1530301 	cmp	r3, r1, lsl #6
   14b14:	e0a00000 	adc	r0, r0, r0
   14b18:	20433301 	subcs	r3, r3, r1, lsl #6
   14b1c:	e1530281 	cmp	r3, r1, lsl #5
   14b20:	e0a00000 	adc	r0, r0, r0
   14b24:	20433281 	subcs	r3, r3, r1, lsl #5
   14b28:	e1530201 	cmp	r3, r1, lsl #4
   14b2c:	e0a00000 	adc	r0, r0, r0
   14b30:	20433201 	subcs	r3, r3, r1, lsl #4
   14b34:	e1530181 	cmp	r3, r1, lsl #3
   14b38:	e0a00000 	adc	r0, r0, r0
   14b3c:	20433181 	subcs	r3, r3, r1, lsl #3
   14b40:	e1530101 	cmp	r3, r1, lsl #2
   14b44:	e0a00000 	adc	r0, r0, r0
   14b48:	20433101 	subcs	r3, r3, r1, lsl #2
   14b4c:	e1530081 	cmp	r3, r1, lsl #1
   14b50:	e0a00000 	adc	r0, r0, r0
   14b54:	20433081 	subcs	r3, r3, r1, lsl #1
   14b58:	e1530001 	cmp	r3, r1
   14b5c:	e0a00000 	adc	r0, r0, r0
   14b60:	20433001 	subcs	r3, r3, r1
   14b64:	e35c0000 	cmp	ip, #0	; 0x0
   14b68:	42600000 	rsbmi	r0, r0, #0	; 0x0
   14b6c:	e12fff1e 	bx	lr
   14b70:	e13c0000 	teq	ip, r0
   14b74:	42600000 	rsbmi	r0, r0, #0	; 0x0
   14b78:	e12fff1e 	bx	lr
   14b7c:	33a00000 	movcc	r0, #0	; 0x0
   14b80:	01a00fcc 	asreq	r0, ip, #31
   14b84:	03800001 	orreq	r0, r0, #1	; 0x1
   14b88:	e12fff1e 	bx	lr
   14b8c:	e16f2f11 	clz	r2, r1
   14b90:	e262201f 	rsb	r2, r2, #31	; 0x1f
   14b94:	e35c0000 	cmp	ip, #0	; 0x0
   14b98:	e1a00233 	lsr	r0, r3, r2
   14b9c:	42600000 	rsbmi	r0, r0, #0	; 0x0
   14ba0:	e12fff1e 	bx	lr
   14ba4:	e52de008 	str	lr, [sp, #-8]!
   14ba8:	eb00023d 	bl	154a4 <__div0>
   14bac:	e3a00000 	mov	r0, #0	; 0x0
   14bb0:	e49df008 	ldr	pc, [sp], #8

00014bb4 <__aeabi_idivmod>:
   14bb4:	e92d4003 	push	{r0, r1, lr}
   14bb8:	ebffff75 	bl	14994 <__aeabi_idiv>
   14bbc:	e8bd4006 	pop	{r1, r2, lr}
   14bc0:	e0030092 	mul	r3, r2, r0
   14bc4:	e0411003 	sub	r1, r1, r3
   14bc8:	e12fff1e 	bx	lr

00014bcc <__aeabi_llsr>:
   14bcc:	e2523020 	subs	r3, r2, #32	; 0x20
   14bd0:	e262c020 	rsb	ip, r2, #32	; 0x20
   14bd4:	41a00230 	lsrmi	r0, r0, r2
   14bd8:	51a00331 	lsrpl	r0, r1, r3
   14bdc:	41800c11 	orrmi	r0, r0, r1, lsl ip
   14be0:	e1a01231 	lsr	r1, r1, r2
   14be4:	e12fff1e 	bx	lr

00014be8 <__aeabi_lasr>:
   14be8:	e2523020 	subs	r3, r2, #32	; 0x20
   14bec:	e262c020 	rsb	ip, r2, #32	; 0x20
   14bf0:	41a00230 	lsrmi	r0, r0, r2
   14bf4:	51a00351 	asrpl	r0, r1, r3
   14bf8:	41800c11 	orrmi	r0, r0, r1, lsl ip
   14bfc:	e1a01251 	asr	r1, r1, r2
   14c00:	e12fff1e 	bx	lr

00014c04 <__aeabi_llsl>:
   14c04:	e2523020 	subs	r3, r2, #32	; 0x20
   14c08:	e262c020 	rsb	ip, r2, #32	; 0x20
   14c0c:	41a01211 	lslmi	r1, r1, r2
   14c10:	51a01310 	lslpl	r1, r0, r3
   14c14:	41811c30 	orrmi	r1, r1, r0, lsr ip
   14c18:	e1a00210 	lsl	r0, r0, r2
   14c1c:	e12fff1e 	bx	lr

00014c20 <__aeabi_drsub>:
   14c20:	e2211102 	eor	r1, r1, #-2147483648	; 0x80000000
   14c24:	ea000000 	b	14c2c <__adddf3>

00014c28 <__aeabi_dsub>:
   14c28:	e2233102 	eor	r3, r3, #-2147483648	; 0x80000000

00014c2c <__adddf3>:
   14c2c:	e92d4030 	push	{r4, r5, lr}
   14c30:	e1a04081 	lsl	r4, r1, #1
   14c34:	e1a05083 	lsl	r5, r3, #1
   14c38:	e1340005 	teq	r4, r5
   14c3c:	01300002 	teqeq	r0, r2
   14c40:	1194c000 	orrsne	ip, r4, r0
   14c44:	1195c002 	orrsne	ip, r5, r2
   14c48:	11f0cac4 	mvnsne	ip, r4, asr #21
   14c4c:	11f0cac5 	mvnsne	ip, r5, asr #21
   14c50:	0a000079 	beq	14e3c <__adddf3+0x210>
   14c54:	e1a04aa4 	lsr	r4, r4, #21
   14c58:	e0745aa5 	rsbs	r5, r4, r5, lsr #21
   14c5c:	b2655000 	rsblt	r5, r5, #0	; 0x0
   14c60:	da000006 	ble	14c80 <__adddf3+0x54>
   14c64:	e0844005 	add	r4, r4, r5
   14c68:	e0202002 	eor	r2, r0, r2
   14c6c:	e0213003 	eor	r3, r1, r3
   14c70:	e0220000 	eor	r0, r2, r0
   14c74:	e0231001 	eor	r1, r3, r1
   14c78:	e0202002 	eor	r2, r0, r2
   14c7c:	e0213003 	eor	r3, r1, r3
   14c80:	e3550036 	cmp	r5, #54	; 0x36
   14c84:	88bd8030 	pophi	{r4, r5, pc}
   14c88:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   14c8c:	e1a01601 	lsl	r1, r1, #12
   14c90:	e3a0c601 	mov	ip, #1048576	; 0x100000
   14c94:	e18c1621 	orr	r1, ip, r1, lsr #12
   14c98:	0a000001 	beq	14ca4 <__adddf3+0x78>
   14c9c:	e2700000 	rsbs	r0, r0, #0	; 0x0
   14ca0:	e2e11000 	rsc	r1, r1, #0	; 0x0
   14ca4:	e3130102 	tst	r3, #-2147483648	; 0x80000000
   14ca8:	e1a03603 	lsl	r3, r3, #12
   14cac:	e18c3623 	orr	r3, ip, r3, lsr #12
   14cb0:	0a000001 	beq	14cbc <__adddf3+0x90>
   14cb4:	e2722000 	rsbs	r2, r2, #0	; 0x0
   14cb8:	e2e33000 	rsc	r3, r3, #0	; 0x0
   14cbc:	e1340005 	teq	r4, r5
   14cc0:	0a000057 	beq	14e24 <__adddf3+0x1f8>
   14cc4:	e2444001 	sub	r4, r4, #1	; 0x1
   14cc8:	e275e020 	rsbs	lr, r5, #32	; 0x20
   14ccc:	ba000005 	blt	14ce8 <__adddf3+0xbc>
   14cd0:	e1a0ce12 	lsl	ip, r2, lr
   14cd4:	e0900532 	adds	r0, r0, r2, lsr r5
   14cd8:	e2a11000 	adc	r1, r1, #0	; 0x0
   14cdc:	e0900e13 	adds	r0, r0, r3, lsl lr
   14ce0:	e0b11553 	adcs	r1, r1, r3, asr r5
   14ce4:	ea000006 	b	14d04 <__adddf3+0xd8>
   14ce8:	e2455020 	sub	r5, r5, #32	; 0x20
   14cec:	e28ee020 	add	lr, lr, #32	; 0x20
   14cf0:	e3520001 	cmp	r2, #1	; 0x1
   14cf4:	e1a0ce13 	lsl	ip, r3, lr
   14cf8:	238cc002 	orrcs	ip, ip, #2	; 0x2
   14cfc:	e0900553 	adds	r0, r0, r3, asr r5
   14d00:	e0b11fc3 	adcs	r1, r1, r3, asr #31
   14d04:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   14d08:	5a000002 	bpl	14d18 <__adddf3+0xec>
   14d0c:	e27cc000 	rsbs	ip, ip, #0	; 0x0
   14d10:	e2f00000 	rscs	r0, r0, #0	; 0x0
   14d14:	e2e11000 	rsc	r1, r1, #0	; 0x0
   14d18:	e3510601 	cmp	r1, #1048576	; 0x100000
   14d1c:	3a00000e 	bcc	14d5c <__adddf3+0x130>
   14d20:	e3510602 	cmp	r1, #2097152	; 0x200000
   14d24:	3a000006 	bcc	14d44 <__adddf3+0x118>
   14d28:	e1b010a1 	lsrs	r1, r1, #1
   14d2c:	e1b00060 	rrxs	r0, r0
   14d30:	e1a0c06c 	rrx	ip, ip
   14d34:	e2844001 	add	r4, r4, #1	; 0x1
   14d38:	e1a02a84 	lsl	r2, r4, #21
   14d3c:	e3720501 	cmn	r2, #4194304	; 0x400000
   14d40:	2a000055 	bcs	14e9c <__adddf3+0x270>
   14d44:	e35c0102 	cmp	ip, #-2147483648	; 0x80000000
   14d48:	01b0c0a0 	lsrseq	ip, r0, #1
   14d4c:	e2b00000 	adcs	r0, r0, #0	; 0x0
   14d50:	e0a11a04 	adc	r1, r1, r4, lsl #20
   14d54:	e1811005 	orr	r1, r1, r5
   14d58:	e8bd8030 	pop	{r4, r5, pc}
   14d5c:	e1b0c08c 	lsls	ip, ip, #1
   14d60:	e0b00000 	adcs	r0, r0, r0
   14d64:	e0a11001 	adc	r1, r1, r1
   14d68:	e3110601 	tst	r1, #1048576	; 0x100000
   14d6c:	e2444001 	sub	r4, r4, #1	; 0x1
   14d70:	1afffff3 	bne	14d44 <__adddf3+0x118>
   14d74:	e3310000 	teq	r1, #0	; 0x0
   14d78:	01a01000 	moveq	r1, r0
   14d7c:	03a00000 	moveq	r0, #0	; 0x0
   14d80:	e16f3f11 	clz	r3, r1
   14d84:	02833020 	addeq	r3, r3, #32	; 0x20
   14d88:	e243300b 	sub	r3, r3, #11	; 0xb
   14d8c:	e2532020 	subs	r2, r3, #32	; 0x20
   14d90:	aa000007 	bge	14db4 <__adddf3+0x188>
   14d94:	e292200c 	adds	r2, r2, #12	; 0xc
   14d98:	da000004 	ble	14db0 <__adddf3+0x184>
   14d9c:	e282c014 	add	ip, r2, #20	; 0x14
   14da0:	e262200c 	rsb	r2, r2, #12	; 0xc
   14da4:	e1a00c11 	lsl	r0, r1, ip
   14da8:	e1a01231 	lsr	r1, r1, r2
   14dac:	ea000004 	b	14dc4 <__adddf3+0x198>
   14db0:	e2822014 	add	r2, r2, #20	; 0x14
   14db4:	d262c020 	rsble	ip, r2, #32	; 0x20
   14db8:	e1a01211 	lsl	r1, r1, r2
   14dbc:	d1811c30 	orrle	r1, r1, r0, lsr ip
   14dc0:	d1a00210 	lslle	r0, r0, r2
   14dc4:	e0544003 	subs	r4, r4, r3
   14dc8:	a0811a04 	addge	r1, r1, r4, lsl #20
   14dcc:	a1811005 	orrge	r1, r1, r5
   14dd0:	a8bd8030 	popge	{r4, r5, pc}
   14dd4:	e1e04004 	mvn	r4, r4
   14dd8:	e254401f 	subs	r4, r4, #31	; 0x1f
   14ddc:	aa00000d 	bge	14e18 <__adddf3+0x1ec>
   14de0:	e294400c 	adds	r4, r4, #12	; 0xc
   14de4:	ca000005 	bgt	14e00 <__adddf3+0x1d4>
   14de8:	e2844014 	add	r4, r4, #20	; 0x14
   14dec:	e2642020 	rsb	r2, r4, #32	; 0x20
   14df0:	e1a00430 	lsr	r0, r0, r4
   14df4:	e1800211 	orr	r0, r0, r1, lsl r2
   14df8:	e1851431 	orr	r1, r5, r1, lsr r4
   14dfc:	e8bd8030 	pop	{r4, r5, pc}
   14e00:	e264400c 	rsb	r4, r4, #12	; 0xc
   14e04:	e2642020 	rsb	r2, r4, #32	; 0x20
   14e08:	e1a00230 	lsr	r0, r0, r2
   14e0c:	e1800411 	orr	r0, r0, r1, lsl r4
   14e10:	e1a01005 	mov	r1, r5
   14e14:	e8bd8030 	pop	{r4, r5, pc}
   14e18:	e1a00431 	lsr	r0, r1, r4
   14e1c:	e1a01005 	mov	r1, r5
   14e20:	e8bd8030 	pop	{r4, r5, pc}
   14e24:	e3340000 	teq	r4, #0	; 0x0
   14e28:	e2233601 	eor	r3, r3, #1048576	; 0x100000
   14e2c:	02211601 	eoreq	r1, r1, #1048576	; 0x100000
   14e30:	02844001 	addeq	r4, r4, #1	; 0x1
   14e34:	12455001 	subne	r5, r5, #1	; 0x1
   14e38:	eaffffa1 	b	14cc4 <__adddf3+0x98>
   14e3c:	e1f0cac4 	mvns	ip, r4, asr #21
   14e40:	11f0cac5 	mvnsne	ip, r5, asr #21
   14e44:	0a000018 	beq	14eac <__adddf3+0x280>
   14e48:	e1340005 	teq	r4, r5
   14e4c:	01300002 	teqeq	r0, r2
   14e50:	0a000003 	beq	14e64 <__adddf3+0x238>
   14e54:	e194c000 	orrs	ip, r4, r0
   14e58:	01a01003 	moveq	r1, r3
   14e5c:	01a00002 	moveq	r0, r2
   14e60:	e8bd8030 	pop	{r4, r5, pc}
   14e64:	e1310003 	teq	r1, r3
   14e68:	13a01000 	movne	r1, #0	; 0x0
   14e6c:	13a00000 	movne	r0, #0	; 0x0
   14e70:	18bd8030 	popne	{r4, r5, pc}
   14e74:	e1b0caa4 	lsrs	ip, r4, #21
   14e78:	1a000003 	bne	14e8c <__adddf3+0x260>
   14e7c:	e1b00080 	lsls	r0, r0, #1
   14e80:	e0b11001 	adcs	r1, r1, r1
   14e84:	23811102 	orrcs	r1, r1, #-2147483648	; 0x80000000
   14e88:	e8bd8030 	pop	{r4, r5, pc}
   14e8c:	e2944501 	adds	r4, r4, #4194304	; 0x400000
   14e90:	32811601 	addcc	r1, r1, #1048576	; 0x100000
   14e94:	38bd8030 	popcc	{r4, r5, pc}
   14e98:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   14e9c:	e385147f 	orr	r1, r5, #2130706432	; 0x7f000000
   14ea0:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   14ea4:	e3a00000 	mov	r0, #0	; 0x0
   14ea8:	e8bd8030 	pop	{r4, r5, pc}
   14eac:	e1f0cac4 	mvns	ip, r4, asr #21
   14eb0:	11a01003 	movne	r1, r3
   14eb4:	11a00002 	movne	r0, r2
   14eb8:	01f0cac5 	mvnseq	ip, r5, asr #21
   14ebc:	11a03001 	movne	r3, r1
   14ec0:	11a02000 	movne	r2, r0
   14ec4:	e1904601 	orrs	r4, r0, r1, lsl #12
   14ec8:	01925603 	orrseq	r5, r2, r3, lsl #12
   14ecc:	01310003 	teqeq	r1, r3
   14ed0:	13811702 	orrne	r1, r1, #524288	; 0x80000
   14ed4:	e8bd8030 	pop	{r4, r5, pc}

00014ed8 <__aeabi_ui2d>:
   14ed8:	e3300000 	teq	r0, #0	; 0x0
   14edc:	03a01000 	moveq	r1, #0	; 0x0
   14ee0:	012fff1e 	bxeq	lr
   14ee4:	e92d4030 	push	{r4, r5, lr}
   14ee8:	e3a04b01 	mov	r4, #1024	; 0x400
   14eec:	e2844032 	add	r4, r4, #50	; 0x32
   14ef0:	e3a05000 	mov	r5, #0	; 0x0
   14ef4:	e3a01000 	mov	r1, #0	; 0x0
   14ef8:	eaffff9d 	b	14d74 <__adddf3+0x148>

00014efc <__aeabi_i2d>:
   14efc:	e3300000 	teq	r0, #0	; 0x0
   14f00:	03a01000 	moveq	r1, #0	; 0x0
   14f04:	012fff1e 	bxeq	lr
   14f08:	e92d4030 	push	{r4, r5, lr}
   14f0c:	e3a04b01 	mov	r4, #1024	; 0x400
   14f10:	e2844032 	add	r4, r4, #50	; 0x32
   14f14:	e2105102 	ands	r5, r0, #-2147483648	; 0x80000000
   14f18:	42600000 	rsbmi	r0, r0, #0	; 0x0
   14f1c:	e3a01000 	mov	r1, #0	; 0x0
   14f20:	eaffff93 	b	14d74 <__adddf3+0x148>

00014f24 <__aeabi_f2d>:
   14f24:	e1b02080 	lsls	r2, r0, #1
   14f28:	e1a011c2 	asr	r1, r2, #3
   14f2c:	e1a01061 	rrx	r1, r1
   14f30:	e1a00e02 	lsl	r0, r2, #28
   14f34:	121234ff 	andsne	r3, r2, #-16777216	; 0xff000000
   14f38:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   14f3c:	1221130e 	eorne	r1, r1, #939524096	; 0x38000000
   14f40:	112fff1e 	bxne	lr
   14f44:	e3320000 	teq	r2, #0	; 0x0
   14f48:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   14f4c:	012fff1e 	bxeq	lr
   14f50:	e92d4030 	push	{r4, r5, lr}
   14f54:	e3a04d0e 	mov	r4, #896	; 0x380
   14f58:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   14f5c:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   14f60:	eaffff83 	b	14d74 <__adddf3+0x148>

00014f64 <__aeabi_ul2d>:
   14f64:	e1902001 	orrs	r2, r0, r1
   14f68:	012fff1e 	bxeq	lr
   14f6c:	e92d4030 	push	{r4, r5, lr}
   14f70:	e3a05000 	mov	r5, #0	; 0x0
   14f74:	ea000006 	b	14f94 <__aeabi_l2d+0x1c>

00014f78 <__aeabi_l2d>:
   14f78:	e1902001 	orrs	r2, r0, r1
   14f7c:	012fff1e 	bxeq	lr
   14f80:	e92d4030 	push	{r4, r5, lr}
   14f84:	e2115102 	ands	r5, r1, #-2147483648	; 0x80000000
   14f88:	5a000001 	bpl	14f94 <__aeabi_l2d+0x1c>
   14f8c:	e2700000 	rsbs	r0, r0, #0	; 0x0
   14f90:	e2e11000 	rsc	r1, r1, #0	; 0x0
   14f94:	e3a04b01 	mov	r4, #1024	; 0x400
   14f98:	e2844032 	add	r4, r4, #50	; 0x32
   14f9c:	e1b0cb21 	lsrs	ip, r1, #22
   14fa0:	0affff5c 	beq	14d18 <__adddf3+0xec>
   14fa4:	e3a02003 	mov	r2, #3	; 0x3
   14fa8:	e1b0c1ac 	lsrs	ip, ip, #3
   14fac:	12822003 	addne	r2, r2, #3	; 0x3
   14fb0:	e1b0c1ac 	lsrs	ip, ip, #3
   14fb4:	12822003 	addne	r2, r2, #3	; 0x3
   14fb8:	e08221ac 	add	r2, r2, ip, lsr #3
   14fbc:	e2623020 	rsb	r3, r2, #32	; 0x20
   14fc0:	e1a0c310 	lsl	ip, r0, r3
   14fc4:	e1a00230 	lsr	r0, r0, r2
   14fc8:	e1800311 	orr	r0, r0, r1, lsl r3
   14fcc:	e1a01231 	lsr	r1, r1, r2
   14fd0:	e0844002 	add	r4, r4, r2
   14fd4:	eaffff4f 	b	14d18 <__adddf3+0xec>

00014fd8 <__aeabi_dmul>:
   14fd8:	e92d4070 	push	{r4, r5, r6, lr}
   14fdc:	e3a0c0ff 	mov	ip, #255	; 0xff
   14fe0:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   14fe4:	e01c4a21 	ands	r4, ip, r1, lsr #20
   14fe8:	101c5a23 	andsne	r5, ip, r3, lsr #20
   14fec:	1134000c 	teqne	r4, ip
   14ff0:	1135000c 	teqne	r5, ip
   14ff4:	0b00006f 	bleq	151b8 <__aeabi_dmul+0x1e0>
   14ff8:	e0844005 	add	r4, r4, r5
   14ffc:	e0216003 	eor	r6, r1, r3
   15000:	e1c11a8c 	bic	r1, r1, ip, lsl #21
   15004:	e1c33a8c 	bic	r3, r3, ip, lsl #21
   15008:	e1905601 	orrs	r5, r0, r1, lsl #12
   1500c:	11925603 	orrsne	r5, r2, r3, lsl #12
   15010:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   15014:	e3833601 	orr	r3, r3, #1048576	; 0x100000
   15018:	0a00001c 	beq	15090 <__aeabi_dmul+0xb8>
   1501c:	e08ec290 	umull	ip, lr, r0, r2
   15020:	e3a05000 	mov	r5, #0	; 0x0
   15024:	e0a5e291 	umlal	lr, r5, r1, r2
   15028:	e2062102 	and	r2, r6, #-2147483648	; 0x80000000
   1502c:	e0a5e390 	umlal	lr, r5, r0, r3
   15030:	e3a06000 	mov	r6, #0	; 0x0
   15034:	e0a65391 	umlal	r5, r6, r1, r3
   15038:	e33c0000 	teq	ip, #0	; 0x0
   1503c:	138ee001 	orrne	lr, lr, #1	; 0x1
   15040:	e24440ff 	sub	r4, r4, #255	; 0xff
   15044:	e3560c02 	cmp	r6, #512	; 0x200
   15048:	e2c44c03 	sbc	r4, r4, #768	; 0x300
   1504c:	2a000002 	bcs	1505c <__aeabi_dmul+0x84>
   15050:	e1b0e08e 	lsls	lr, lr, #1
   15054:	e0b55005 	adcs	r5, r5, r5
   15058:	e0a66006 	adc	r6, r6, r6
   1505c:	e1821586 	orr	r1, r2, r6, lsl #11
   15060:	e1811aa5 	orr	r1, r1, r5, lsr #21
   15064:	e1a00585 	lsl	r0, r5, #11
   15068:	e1800aae 	orr	r0, r0, lr, lsr #21
   1506c:	e1a0e58e 	lsl	lr, lr, #11
   15070:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   15074:	835c0c07 	cmphi	ip, #1792	; 0x700
   15078:	8a00000f 	bhi	150bc <__aeabi_dmul+0xe4>
   1507c:	e35e0102 	cmp	lr, #-2147483648	; 0x80000000
   15080:	01b0e0a0 	lsrseq	lr, r0, #1
   15084:	e2b00000 	adcs	r0, r0, #0	; 0x0
   15088:	e0a11a04 	adc	r1, r1, r4, lsl #20
   1508c:	e8bd8070 	pop	{r4, r5, r6, pc}
   15090:	e2066102 	and	r6, r6, #-2147483648	; 0x80000000
   15094:	e1861001 	orr	r1, r6, r1
   15098:	e1800002 	orr	r0, r0, r2
   1509c:	e0211003 	eor	r1, r1, r3
   150a0:	e05440ac 	subs	r4, r4, ip, lsr #1
   150a4:	c074500c 	rsbsgt	r5, r4, ip
   150a8:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   150ac:	c8bd8070 	popgt	{r4, r5, r6, pc}
   150b0:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   150b4:	e3a0e000 	mov	lr, #0	; 0x0
   150b8:	e2544001 	subs	r4, r4, #1	; 0x1
   150bc:	ca000058 	bgt	15224 <__aeabi_dmul+0x24c>
   150c0:	e3740036 	cmn	r4, #54	; 0x36
   150c4:	d3a00000 	movle	r0, #0	; 0x0
   150c8:	d2011102 	andle	r1, r1, #-2147483648	; 0x80000000
   150cc:	d8bd8070 	pople	{r4, r5, r6, pc}
   150d0:	e2644000 	rsb	r4, r4, #0	; 0x0
   150d4:	e2544020 	subs	r4, r4, #32	; 0x20
   150d8:	aa000018 	bge	15140 <__aeabi_dmul+0x168>
   150dc:	e294400c 	adds	r4, r4, #12	; 0xc
   150e0:	ca00000b 	bgt	15114 <__aeabi_dmul+0x13c>
   150e4:	e2844014 	add	r4, r4, #20	; 0x14
   150e8:	e2645020 	rsb	r5, r4, #32	; 0x20
   150ec:	e1a03510 	lsl	r3, r0, r5
   150f0:	e1a00430 	lsr	r0, r0, r4
   150f4:	e1800511 	orr	r0, r0, r1, lsl r5
   150f8:	e2012102 	and	r2, r1, #-2147483648	; 0x80000000
   150fc:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   15100:	e0900fa3 	adds	r0, r0, r3, lsr #31
   15104:	e0a21431 	adc	r1, r2, r1, lsr r4
   15108:	e19ee083 	orrs	lr, lr, r3, lsl #1
   1510c:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   15110:	e8bd8070 	pop	{r4, r5, r6, pc}
   15114:	e264400c 	rsb	r4, r4, #12	; 0xc
   15118:	e2645020 	rsb	r5, r4, #32	; 0x20
   1511c:	e1a03410 	lsl	r3, r0, r4
   15120:	e1a00530 	lsr	r0, r0, r5
   15124:	e1800411 	orr	r0, r0, r1, lsl r4
   15128:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   1512c:	e0900fa3 	adds	r0, r0, r3, lsr #31
   15130:	e2a11000 	adc	r1, r1, #0	; 0x0
   15134:	e19ee083 	orrs	lr, lr, r3, lsl #1
   15138:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   1513c:	e8bd8070 	pop	{r4, r5, r6, pc}
   15140:	e2645020 	rsb	r5, r4, #32	; 0x20
   15144:	e18ee510 	orr	lr, lr, r0, lsl r5
   15148:	e1a03430 	lsr	r3, r0, r4
   1514c:	e1833511 	orr	r3, r3, r1, lsl r5
   15150:	e1a00431 	lsr	r0, r1, r4
   15154:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   15158:	e1c00431 	bic	r0, r0, r1, lsr r4
   1515c:	e0800fa3 	add	r0, r0, r3, lsr #31
   15160:	e19ee083 	orrs	lr, lr, r3, lsl #1
   15164:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   15168:	e8bd8070 	pop	{r4, r5, r6, pc}
   1516c:	e3340000 	teq	r4, #0	; 0x0
   15170:	1a000008 	bne	15198 <__aeabi_dmul+0x1c0>
   15174:	e2016102 	and	r6, r1, #-2147483648	; 0x80000000
   15178:	e1b00080 	lsls	r0, r0, #1
   1517c:	e0a11001 	adc	r1, r1, r1
   15180:	e3110601 	tst	r1, #1048576	; 0x100000
   15184:	02444001 	subeq	r4, r4, #1	; 0x1
   15188:	0afffffa 	beq	15178 <__aeabi_dmul+0x1a0>
   1518c:	e1811006 	orr	r1, r1, r6
   15190:	e3350000 	teq	r5, #0	; 0x0
   15194:	11a0f00e 	movne	pc, lr
   15198:	e2036102 	and	r6, r3, #-2147483648	; 0x80000000
   1519c:	e1b02082 	lsls	r2, r2, #1
   151a0:	e0a33003 	adc	r3, r3, r3
   151a4:	e3130601 	tst	r3, #1048576	; 0x100000
   151a8:	02455001 	subeq	r5, r5, #1	; 0x1
   151ac:	0afffffa 	beq	1519c <__aeabi_dmul+0x1c4>
   151b0:	e1833006 	orr	r3, r3, r6
   151b4:	e1a0f00e 	mov	pc, lr
   151b8:	e134000c 	teq	r4, ip
   151bc:	e00c5a23 	and	r5, ip, r3, lsr #20
   151c0:	1135000c 	teqne	r5, ip
   151c4:	0a000006 	beq	151e4 <__aeabi_dmul+0x20c>
   151c8:	e1906081 	orrs	r6, r0, r1, lsl #1
   151cc:	11926083 	orrsne	r6, r2, r3, lsl #1
   151d0:	1affffe5 	bne	1516c <__aeabi_dmul+0x194>
   151d4:	e0211003 	eor	r1, r1, r3
   151d8:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   151dc:	e3a00000 	mov	r0, #0	; 0x0
   151e0:	e8bd8070 	pop	{r4, r5, r6, pc}
   151e4:	e1906081 	orrs	r6, r0, r1, lsl #1
   151e8:	01a00002 	moveq	r0, r2
   151ec:	01a01003 	moveq	r1, r3
   151f0:	11926083 	orrsne	r6, r2, r3, lsl #1
   151f4:	0a00000f 	beq	15238 <__aeabi_dmul+0x260>
   151f8:	e134000c 	teq	r4, ip
   151fc:	1a000001 	bne	15208 <__aeabi_dmul+0x230>
   15200:	e1906601 	orrs	r6, r0, r1, lsl #12
   15204:	1a00000b 	bne	15238 <__aeabi_dmul+0x260>
   15208:	e135000c 	teq	r5, ip
   1520c:	1a000003 	bne	15220 <__aeabi_dmul+0x248>
   15210:	e1926603 	orrs	r6, r2, r3, lsl #12
   15214:	11a00002 	movne	r0, r2
   15218:	11a01003 	movne	r1, r3
   1521c:	1a000005 	bne	15238 <__aeabi_dmul+0x260>
   15220:	e0211003 	eor	r1, r1, r3
   15224:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   15228:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   1522c:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   15230:	e3a00000 	mov	r0, #0	; 0x0
   15234:	e8bd8070 	pop	{r4, r5, r6, pc}
   15238:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   1523c:	e381173e 	orr	r1, r1, #16252928	; 0xf80000
   15240:	e8bd8070 	pop	{r4, r5, r6, pc}

00015244 <__aeabi_ddiv>:
   15244:	e92d4070 	push	{r4, r5, r6, lr}
   15248:	e3a0c0ff 	mov	ip, #255	; 0xff
   1524c:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   15250:	e01c4a21 	ands	r4, ip, r1, lsr #20
   15254:	101c5a23 	andsne	r5, ip, r3, lsr #20
   15258:	1134000c 	teqne	r4, ip
   1525c:	1135000c 	teqne	r5, ip
   15260:	0b00005c 	bleq	153d8 <__aeabi_ddiv+0x194>
   15264:	e0444005 	sub	r4, r4, r5
   15268:	e021e003 	eor	lr, r1, r3
   1526c:	e1925603 	orrs	r5, r2, r3, lsl #12
   15270:	e1a01601 	lsl	r1, r1, #12
   15274:	0a00004b 	beq	153a8 <__aeabi_ddiv+0x164>
   15278:	e1a03603 	lsl	r3, r3, #12
   1527c:	e3a05201 	mov	r5, #268435456	; 0x10000000
   15280:	e1853223 	orr	r3, r5, r3, lsr #4
   15284:	e1833c22 	orr	r3, r3, r2, lsr #24
   15288:	e1a02402 	lsl	r2, r2, #8
   1528c:	e1855221 	orr	r5, r5, r1, lsr #4
   15290:	e1855c20 	orr	r5, r5, r0, lsr #24
   15294:	e1a06400 	lsl	r6, r0, #8
   15298:	e20e1102 	and	r1, lr, #-2147483648	; 0x80000000
   1529c:	e1550003 	cmp	r5, r3
   152a0:	01560002 	cmpeq	r6, r2
   152a4:	e2a440fd 	adc	r4, r4, #253	; 0xfd
   152a8:	e2844c03 	add	r4, r4, #768	; 0x300
   152ac:	2a000001 	bcs	152b8 <__aeabi_ddiv+0x74>
   152b0:	e1b030a3 	lsrs	r3, r3, #1
   152b4:	e1a02062 	rrx	r2, r2
   152b8:	e0566002 	subs	r6, r6, r2
   152bc:	e0c55003 	sbc	r5, r5, r3
   152c0:	e1b030a3 	lsrs	r3, r3, #1
   152c4:	e1a02062 	rrx	r2, r2
   152c8:	e3a00601 	mov	r0, #1048576	; 0x100000
   152cc:	e3a0c702 	mov	ip, #524288	; 0x80000
   152d0:	e056e002 	subs	lr, r6, r2
   152d4:	e0d5e003 	sbcs	lr, r5, r3
   152d8:	20466002 	subcs	r6, r6, r2
   152dc:	21a0500e 	movcs	r5, lr
   152e0:	2180000c 	orrcs	r0, r0, ip
   152e4:	e1b030a3 	lsrs	r3, r3, #1
   152e8:	e1a02062 	rrx	r2, r2
   152ec:	e056e002 	subs	lr, r6, r2
   152f0:	e0d5e003 	sbcs	lr, r5, r3
   152f4:	20466002 	subcs	r6, r6, r2
   152f8:	21a0500e 	movcs	r5, lr
   152fc:	218000ac 	orrcs	r0, r0, ip, lsr #1
   15300:	e1b030a3 	lsrs	r3, r3, #1
   15304:	e1a02062 	rrx	r2, r2
   15308:	e056e002 	subs	lr, r6, r2
   1530c:	e0d5e003 	sbcs	lr, r5, r3
   15310:	20466002 	subcs	r6, r6, r2
   15314:	21a0500e 	movcs	r5, lr
   15318:	2180012c 	orrcs	r0, r0, ip, lsr #2
   1531c:	e1b030a3 	lsrs	r3, r3, #1
   15320:	e1a02062 	rrx	r2, r2
   15324:	e056e002 	subs	lr, r6, r2
   15328:	e0d5e003 	sbcs	lr, r5, r3
   1532c:	20466002 	subcs	r6, r6, r2
   15330:	21a0500e 	movcs	r5, lr
   15334:	218001ac 	orrcs	r0, r0, ip, lsr #3
   15338:	e195e006 	orrs	lr, r5, r6
   1533c:	0a00000d 	beq	15378 <__aeabi_ddiv+0x134>
   15340:	e1a05205 	lsl	r5, r5, #4
   15344:	e1855e26 	orr	r5, r5, r6, lsr #28
   15348:	e1a06206 	lsl	r6, r6, #4
   1534c:	e1a03183 	lsl	r3, r3, #3
   15350:	e1833ea2 	orr	r3, r3, r2, lsr #29
   15354:	e1a02182 	lsl	r2, r2, #3
   15358:	e1b0c22c 	lsrs	ip, ip, #4
   1535c:	1affffdb 	bne	152d0 <__aeabi_ddiv+0x8c>
   15360:	e3110601 	tst	r1, #1048576	; 0x100000
   15364:	1a000006 	bne	15384 <__aeabi_ddiv+0x140>
   15368:	e1811000 	orr	r1, r1, r0
   1536c:	e3a00000 	mov	r0, #0	; 0x0
   15370:	e3a0c102 	mov	ip, #-2147483648	; 0x80000000
   15374:	eaffffd5 	b	152d0 <__aeabi_ddiv+0x8c>
   15378:	e3110601 	tst	r1, #1048576	; 0x100000
   1537c:	01811000 	orreq	r1, r1, r0
   15380:	03a00000 	moveq	r0, #0	; 0x0
   15384:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   15388:	835c0c07 	cmphi	ip, #1792	; 0x700
   1538c:	8affff4a 	bhi	150bc <__aeabi_dmul+0xe4>
   15390:	e055c003 	subs	ip, r5, r3
   15394:	0056c002 	subseq	ip, r6, r2
   15398:	01b0c0a0 	lsrseq	ip, r0, #1
   1539c:	e2b00000 	adcs	r0, r0, #0	; 0x0
   153a0:	e0a11a04 	adc	r1, r1, r4, lsl #20
   153a4:	e8bd8070 	pop	{r4, r5, r6, pc}
   153a8:	e20ee102 	and	lr, lr, #-2147483648	; 0x80000000
   153ac:	e18e1621 	orr	r1, lr, r1, lsr #12
   153b0:	e09440ac 	adds	r4, r4, ip, lsr #1
   153b4:	c074500c 	rsbsgt	r5, r4, ip
   153b8:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   153bc:	c8bd8070 	popgt	{r4, r5, r6, pc}
   153c0:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   153c4:	e3a0e000 	mov	lr, #0	; 0x0
   153c8:	e2544001 	subs	r4, r4, #1	; 0x1
   153cc:	eaffff3a 	b	150bc <__aeabi_dmul+0xe4>
   153d0:	e185e006 	orr	lr, r5, r6
   153d4:	eaffff38 	b	150bc <__aeabi_dmul+0xe4>
   153d8:	e00c5a23 	and	r5, ip, r3, lsr #20
   153dc:	e134000c 	teq	r4, ip
   153e0:	0135000c 	teqeq	r5, ip
   153e4:	0affff93 	beq	15238 <__aeabi_dmul+0x260>
   153e8:	e134000c 	teq	r4, ip
   153ec:	1a000006 	bne	1540c <__aeabi_ddiv+0x1c8>
   153f0:	e1904601 	orrs	r4, r0, r1, lsl #12
   153f4:	1affff8f 	bne	15238 <__aeabi_dmul+0x260>
   153f8:	e135000c 	teq	r5, ip
   153fc:	1affff87 	bne	15220 <__aeabi_dmul+0x248>
   15400:	e1a00002 	mov	r0, r2
   15404:	e1a01003 	mov	r1, r3
   15408:	eaffff8a 	b	15238 <__aeabi_dmul+0x260>
   1540c:	e135000c 	teq	r5, ip
   15410:	1a000004 	bne	15428 <__aeabi_ddiv+0x1e4>
   15414:	e1925603 	orrs	r5, r2, r3, lsl #12
   15418:	0affff6d 	beq	151d4 <__aeabi_dmul+0x1fc>
   1541c:	e1a00002 	mov	r0, r2
   15420:	e1a01003 	mov	r1, r3
   15424:	eaffff83 	b	15238 <__aeabi_dmul+0x260>
   15428:	e1906081 	orrs	r6, r0, r1, lsl #1
   1542c:	11926083 	orrsne	r6, r2, r3, lsl #1
   15430:	1affff4d 	bne	1516c <__aeabi_dmul+0x194>
   15434:	e1904081 	orrs	r4, r0, r1, lsl #1
   15438:	1affff78 	bne	15220 <__aeabi_dmul+0x248>
   1543c:	e1925083 	orrs	r5, r2, r3, lsl #1
   15440:	1affff63 	bne	151d4 <__aeabi_dmul+0x1fc>
   15444:	eaffff7b 	b	15238 <__aeabi_dmul+0x260>

00015448 <__aeabi_d2iz>:
   15448:	e1a02081 	lsl	r2, r1, #1
   1544c:	e2922602 	adds	r2, r2, #2097152	; 0x200000
   15450:	2a00000c 	bcs	15488 <__aeabi_d2iz+0x40>
   15454:	5a000009 	bpl	15480 <__aeabi_d2iz+0x38>
   15458:	e3e03e3e 	mvn	r3, #992	; 0x3e0
   1545c:	e0532ac2 	subs	r2, r3, r2, asr #21
   15460:	9a00000a 	bls	15490 <__aeabi_d2iz+0x48>
   15464:	e1a03581 	lsl	r3, r1, #11
   15468:	e3833102 	orr	r3, r3, #-2147483648	; 0x80000000
   1546c:	e1833aa0 	orr	r3, r3, r0, lsr #21
   15470:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   15474:	e1a00233 	lsr	r0, r3, r2
   15478:	12600000 	rsbne	r0, r0, #0	; 0x0
   1547c:	e12fff1e 	bx	lr
   15480:	e3a00000 	mov	r0, #0	; 0x0
   15484:	e12fff1e 	bx	lr
   15488:	e1900601 	orrs	r0, r0, r1, lsl #12
   1548c:	1a000002 	bne	1549c <__aeabi_d2iz+0x54>
   15490:	e2110102 	ands	r0, r1, #-2147483648	; 0x80000000
   15494:	03e00102 	mvneq	r0, #-2147483648	; 0x80000000
   15498:	e12fff1e 	bx	lr
   1549c:	e3a00000 	mov	r0, #0	; 0x0
   154a0:	e12fff1e 	bx	lr

000154a4 <__div0>:
   154a4:	e92d4002 	push	{r1, lr}
   154a8:	e3a00008 	mov	r0, #8	; 0x8
   154ac:	ebfffcaa 	bl	1475c <raise>
   154b0:	e8bd8002 	pop	{r1, pc}
