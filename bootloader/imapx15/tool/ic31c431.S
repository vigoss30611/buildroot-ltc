
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
      78:	eb000449 	bl	11a4 <chip_test>
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
      9c:	00000584 	.word	0x00000584

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
     16c:	eb0018dc 	bl	64e4 <vs_assign_by_id>
     170:	e3500000 	cmp	r0, #0	; 0x0
     174:	e58d0024 	str	r0, [sp, #36]
     178:	0a000008 	beq	1a0 <bl_load_isi+0x50>
     17c:	e59f31d4 	ldr	r3, [pc, #468]	; 358 <bl_load_isi+0x208>
     180:	e1a00004 	mov	r0, r4
     184:	e5934000 	ldr	r4, [r3]
     188:	eb0017df 	bl	610c <vs_device_name>
     18c:	e59d2024 	ldr	r2, [sp, #36]
     190:	e1a01000 	mov	r1, r0
     194:	e59f01c0 	ldr	r0, [pc, #448]	; 35c <bl_load_isi+0x20c>
     198:	e12fff34 	blx	r4
     19c:	ea00006a 	b	34c <bl_load_isi+0x1fc>
     1a0:	e1a00004 	mov	r0, r4
     1a4:	e59f11b4 	ldr	r1, [pc, #436]	; 360 <bl_load_isi+0x210>
     1a8:	eb00180e 	bl	61e8 <vs_is_device>
     1ac:	e3500000 	cmp	r0, #0	; 0x0
     1b0:	03a09b01 	moveq	r9, #1024	; 0x400
     1b4:	0a000006 	beq	1d4 <bl_load_isi+0x84>
     1b8:	e59d0024 	ldr	r0, [sp, #36]
     1bc:	eb00275e 	bl	9f3c <nand_get_config>
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
     230:	eb000631 	bl	1afc <isi_check_header>
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
     2a0:	eb00066e 	bl	1c60 <isi_check_data>
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
     2f8:	eb0017d3 	bl	624c <vs_read>
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
     35c:	000160eb 	.word	0x000160eb
     360:	0001610f 	.word	0x0001610f
     364:	0001ffff 	.word	0x0001ffff
     368:	fffe0000 	.word	0xfffe0000
     36c:	00016113 	.word	0x00016113
     370:	0001612a 	.word	0x0001612a
     374:	0001613c 	.word	0x0001613c

00000378 <try_get_bl>:
     378:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     37c:	e3a02602 	mov	r2, #2097152	; 0x200000
     380:	e24dd00c 	sub	sp, sp, #12	; 0xc
     384:	e3a03000 	mov	r3, #0	; 0x0
     388:	e88d000c 	stm	sp, {r2, r3}
     38c:	e1a07001 	mov	r7, r1
     390:	e1a06000 	mov	r6, r0
     394:	eb0000aa 	bl	644 <boot_device>
     398:	e1a04000 	mov	r4, r0
     39c:	eb001713 	bl	5ff0 <vs_device_bootable>
     3a0:	e2505000 	subs	r5, r0, #0	; 0x0
     3a4:	1a000008 	bne	3cc <try_get_bl+0x54>
     3a8:	e59f308c 	ldr	r3, [pc, #140]	; 43c <try_get_bl+0xc4>
     3ac:	e1a00004 	mov	r0, r4
     3b0:	e5934000 	ldr	r4, [r3]
     3b4:	eb001754 	bl	610c <vs_device_name>
     3b8:	e1a01000 	mov	r1, r0
     3bc:	e59f007c 	ldr	r0, [pc, #124]	; 440 <try_get_bl+0xc8>
     3c0:	e12fff34 	blx	r4
     3c4:	e1a00005 	mov	r0, r5
     3c8:	ea000019 	b	434 <try_get_bl+0xbc>
     3cc:	e1a00004 	mov	r0, r4
     3d0:	eb00173b 	bl	60c4 <vs_device_boot0>
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
     42c:	eb001bf5 	bl	7408 <cdbg_verify_burn_enable>
     430:	e3a00001 	mov	r0, #1	; 0x1
     434:	e28dd00c 	add	sp, sp, #12	; 0xc
     438:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     43c:	3c00d6cc 	.word	0x3c00d6cc
     440:	0001614e 	.word	0x0001614e
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
     468:	eb00005c 	bl	5e0 <set_xom>
     46c:	eb000061 	bl	5f8 <boot_info>
     470:	e28d8004 	add	r8, sp, #4	; 0x4
     474:	e1a05000 	mov	r5, r0
     478:	ea000008 	b	4a0 <try_boot+0x58>
     47c:	eb000156 	bl	9dc <gtc_time>
     480:	e1a00004 	mov	r0, r4
     484:	eb000055 	bl	5e0 <set_xom>
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
     4b0:	eb000149 	bl	9dc <gtc_time>
     4b4:	e3a00001 	mov	r0, #1	; 0x1
     4b8:	eb00015d 	bl	a34 <ecfg_check_flag>
     4bc:	e3500000 	cmp	r0, #0	; 0x0
     4c0:	1a00000c 	bne	4f8 <try_boot+0xb0>
     4c4:	e3570000 	cmp	r7, #0	; 0x0
     4c8:	1a00000a 	bne	4f8 <try_boot+0xb0>
     4cc:	eb0000c0 	bl	7d4 <get_reset_stats>
     4d0:	e3500000 	cmp	r0, #0	; 0x0
     4d4:	13500002 	cmpne	r0, #2	; 0x2
     4d8:	1a000006 	bne	4f8 <try_boot+0xb0>
     4dc:	e3500002 	cmp	r0, #2	; 0x2
     4e0:	1a000001 	bne	4ec <try_boot+0xa4>
     4e4:	e28d0004 	add	r0, sp, #4	; 0x4
     4e8:	eb0000c6 	bl	808 <update_mask_on_reset>
     4ec:	e59d0004 	ldr	r0, [sp, #4]
     4f0:	eb001e43 	bl	7e04 <burn>
     4f4:	eb000138 	bl	9dc <gtc_time>
     4f8:	e3540000 	cmp	r4, #0	; 0x0
     4fc:	0a000004 	beq	514 <try_boot+0xcc>
     500:	e3a0330f 	mov	r3, #1006632960	; 0x3c000000
     504:	e5930014 	ldr	r0, [r3, #20]
     508:	e59d1000 	ldr	r1, [sp]
     50c:	eb001bc8 	bl	7434 <cdbg_jump>
     510:	eafffffe 	b	510 <try_boot+0xc8>
     514:	eb001bf8 	bl	74fc <cdbg_shell>
     518:	eafffffe 	b	518 <try_boot+0xd0>

0000051c <init_data_section>:
     51c:	e59f1010 	ldr	r1, [pc, #16]	; 534 <init_data_section+0x18>
     520:	e59f0010 	ldr	r0, [pc, #16]	; 538 <init_data_section+0x1c>
     524:	e1510000 	cmp	r1, r0
     528:	012fff1e 	bxeq	lr
     52c:	e59f2008 	ldr	r2, [pc, #8]	; 53c <init_data_section+0x20>
     530:	ea000409 	b	155c <memcpy>
     534:	0001923c 	.word	0x0001923c
     538:	3c00d000 	.word	0x3c00d000
     53c:	0000011c 	.word	0x0000011c

00000540 <initialization>:
     540:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     544:	e24dd004 	sub	sp, sp, #4	; 0x4
     548:	ebfffff3 	bl	51c <init_data_section>
     54c:	eb0001af 	bl	c10 <init_efuse>
     550:	eb000027 	bl	5f4 <init_xom>
     554:	eb0000de 	bl	8d4 <init_lowlevel>
     558:	eb000529 	bl	1a04 <init_malloc>
     55c:	eb003564 	bl	daf4 <init_timer>
     560:	eb00012a 	bl	a10 <init_gtc>
     564:	eb0034f5 	bl	d940 <init_serial>
     568:	eb001f80 	bl	8370 <init_mmc>
     56c:	eb0026c7 	bl	a090 <init_nand>
     570:	eb000096 	bl	7d0 <init_crypto>
     574:	eb000275 	bl	f50 <init_alternate>
     578:	e3a00000 	mov	r0, #0	; 0x0
     57c:	e28dd004 	add	sp, sp, #4	; 0x4
     580:	e8bd8000 	pop	{pc}

00000584 <main>:
     584:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     588:	e3a00000 	mov	r0, #0	; 0x0
     58c:	e24dd004 	sub	sp, sp, #4	; 0x4
     590:	eb001b80 	bl	7398 <cdbg_log_toggle>
     594:	ebffffe9 	bl	540 <initialization>
     598:	eb00010f 	bl	9dc <gtc_time>
     59c:	e3a00001 	mov	r0, #1	; 0x1
     5a0:	eb001b7c 	bl	7398 <cdbg_log_toggle>
     5a4:	e59f001c 	ldr	r0, [pc, #28]	; 5c8 <main+0x44>
     5a8:	e59f301c 	ldr	r3, [pc, #28]	; 5cc <main+0x48>
     5ac:	e1a0e00f 	mov	lr, pc
     5b0:	e593f000 	ldr	pc, [r3]
     5b4:	e3a00000 	mov	r0, #0	; 0x0
     5b8:	eb001b76 	bl	7398 <cdbg_log_toggle>
     5bc:	e3a00000 	mov	r0, #0	; 0x0
     5c0:	ebffffa0 	bl	448 <try_boot>
     5c4:	eafffffe 	b	5c4 <main+0x40>
     5c8:	00016168 	.word	0x00016168
     5cc:	3c00d6cc 	.word	0x3c00d6cc

000005d0 <get_xom>:
     5d0:	e59f3004 	ldr	r3, [pc, #4]	; 5dc <get_xom+0xc>
     5d4:	e1d300b0 	ldrh	r0, [r3]
     5d8:	e12fff1e 	bx	lr
     5dc:	3c00d120 	.word	0x3c00d120

000005e0 <set_xom>:
     5e0:	e59f3008 	ldr	r3, [pc, #8]	; 5f0 <set_xom+0x10>
     5e4:	e200001f 	and	r0, r0, #31	; 0x1f
     5e8:	e1c300b0 	strh	r0, [r3]
     5ec:	e12fff1e 	bx	lr
     5f0:	3c00d120 	.word	0x3c00d120

000005f4 <init_xom>:
     5f4:	e12fff1e 	bx	lr

000005f8 <boot_info>:
     5f8:	e92d4010 	push	{r4, lr}
     5fc:	eb000115 	bl	a58 <ecfg_paras>
     600:	e1a04000 	mov	r4, r0
     604:	e3a00b01 	mov	r0, #1024	; 0x400
     608:	eb000109 	bl	a34 <ecfg_check_flag>
     60c:	e59f3028 	ldr	r3, [pc, #40]	; 63c <boot_info+0x44>
     610:	e3500000 	cmp	r0, #0	; 0x0
     614:	11d330b0 	ldrhne	r3, [r3]
     618:	01d330b0 	ldrheq	r3, [r3]
     61c:	1203301f 	andne	r3, r3, #31	; 0x1f
     620:	0203301f 	andeq	r3, r3, #31	; 0x1f
     624:	159f2014 	ldrne	r2, [pc, #20]	; 640 <boot_info+0x48>
     628:	11a03083 	lslne	r3, r3, #1
     62c:	00843083 	addeq	r3, r4, r3, lsl #1
     630:	119200b3 	ldrhne	r0, [r2, r3]
     634:	01d302b0 	ldrheq	r0, [r3, #32]
     638:	e8bd8010 	pop	{r4, pc}
     63c:	3c00d120 	.word	0x3c00d120
     640:	00015d48 	.word	0x00015d48

00000644 <boot_device>:
     644:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     648:	e24dd004 	sub	sp, sp, #4	; 0x4
     64c:	ebffffe9 	bl	5f8 <boot_info>
     650:	e1a00620 	lsr	r0, r0, #12
     654:	e6ff0070 	uxth	r0, r0
     658:	e28dd004 	add	sp, sp, #4	; 0x4
     65c:	e8bd8000 	pop	{pc}

00000660 <set_pll>:
     660:	e3500003 	cmp	r0, #3	; 0x3
     664:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
     668:	e1a02001 	mov	r2, r1
     66c:	88bd85f0 	pophi	{r4, r5, r6, r7, r8, sl, pc}
     670:	e3a03018 	mov	r3, #24	; 0x18
     674:	e0030093 	mul	r3, r3, r0
     678:	e59f808c 	ldr	r8, [pc, #140]	; 70c <set_pll+0xac>
     67c:	e59f108c 	ldr	r1, [pc, #140]	; 710 <set_pll+0xb0>
     680:	e59f008c 	ldr	r0, [pc, #140]	; 714 <set_pll+0xb4>
     684:	e59fc08c 	ldr	ip, [pc, #140]	; 718 <set_pll+0xb8>
     688:	e59f408c 	ldr	r4, [pc, #140]	; 71c <set_pll+0xbc>
     68c:	e59fa08c 	ldr	sl, [pc, #140]	; 720 <set_pll+0xc0>
     690:	e0838008 	add	r8, r3, r8
     694:	e083c00c 	add	ip, r3, ip
     698:	e0831001 	add	r1, r3, r1
     69c:	e3a07001 	mov	r7, #1	; 0x1
     6a0:	e0830000 	add	r0, r3, r0
     6a4:	e1a06422 	lsr	r6, r2, #8
     6a8:	e0834004 	add	r4, r3, r4
     6ac:	e3a05000 	mov	r5, #0	; 0x0
     6b0:	e6ef2072 	uxtb	r2, r2
     6b4:	e083a00a 	add	sl, r3, sl
     6b8:	e3a03002 	mov	r3, #2	; 0x2
     6bc:	e5c87000 	strb	r7, [r8]
     6c0:	e5c12000 	strb	r2, [r1]
     6c4:	e5c06000 	strb	r6, [r0]
     6c8:	e5cc5000 	strb	r5, [ip]
     6cc:	e5c43000 	strb	r3, [r4]
     6d0:	e0833007 	add	r3, r3, r7
     6d4:	e5c83000 	strb	r3, [r8]
     6d8:	e1a02005 	mov	r2, r5
     6dc:	e5cc7000 	strb	r7, [ip]
     6e0:	e3011a0a 	movw	r1, #6666	; 0x1a0a
     6e4:	ea000001 	b	6f0 <set_pll+0x90>
     6e8:	e1520001 	cmp	r2, r1
     6ec:	0a000003 	beq	700 <set_pll+0xa0>
     6f0:	e5da3000 	ldrb	r3, [sl]
     6f4:	e2822001 	add	r2, r2, #1	; 0x1
     6f8:	e3130001 	tst	r3, #1	; 0x1
     6fc:	0afffff9 	beq	6e8 <set_pll+0x88>
     700:	e3a03002 	mov	r3, #2	; 0x2
     704:	e5883000 	str	r3, [r8]
     708:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
     70c:	21e0a00c 	.word	0x21e0a00c
     710:	21e0a000 	.word	0x21e0a000
     714:	21e0a004 	.word	0x21e0a004
     718:	21e0a010 	.word	0x21e0a010
     71c:	21e0a008 	.word	0x21e0a008
     720:	21e0a014 	.word	0x21e0a014

00000724 <set_clk_divider>:
     724:	e3500005 	cmp	r0, #5	; 0x5
     728:	812fff1e 	bxhi	lr
     72c:	e59fc020 	ldr	ip, [pc, #32]	; 754 <set_clk_divider+0x30>
     730:	e1a03280 	lsl	r3, r0, #5
     734:	e083c00c 	add	ip, r3, ip
     738:	e3a02000 	mov	r2, #0	; 0x0
     73c:	e7d13002 	ldrb	r3, [r1, r2]
     740:	e2822001 	add	r2, r2, #1	; 0x1
     744:	e3520008 	cmp	r2, #8	; 0x8
     748:	e4cc3004 	strb	r3, [ip], #4
     74c:	1afffffa 	bne	73c <set_clk_divider+0x18>
     750:	e12fff1e 	bx	lr
     754:	21e0a070 	.word	0x21e0a070

00000758 <module_set_clock>:
     758:	e59f3020 	ldr	r3, [pc, #32]	; 780 <module_set_clock+0x28>
     75c:	e1500003 	cmp	r0, r3
     760:	13a03001 	movne	r3, #1	; 0x1
     764:	1580300c 	strne	r3, [r0, #12]
     768:	e2013007 	and	r3, r1, #7	; 0x7
     76c:	e5803000 	str	r3, [r0]
     770:	e3a03000 	mov	r3, #0	; 0x0
     774:	e5802004 	str	r2, [r0, #4]
     778:	e580300c 	str	r3, [r0, #12]
     77c:	e12fff1e 	bx	lr
     780:	21e0a360 	.word	0x21e0a360

00000784 <module_enable>:
     784:	e3a020ff 	mov	r2, #255	; 0xff
     788:	e3a01000 	mov	r1, #0	; 0x0
     78c:	e3a03011 	mov	r3, #17	; 0x11
     790:	e580100c 	str	r1, [r0, #12]
     794:	e5802000 	str	r2, [r0]
     798:	e5802004 	str	r2, [r0, #4]
     79c:	e5803008 	str	r3, [r0, #8]
     7a0:	e5903008 	ldr	r3, [r0, #8]
     7a4:	e3c33010 	bic	r3, r3, #16	; 0x10
     7a8:	e5803008 	str	r3, [r0, #8]
     7ac:	e5802018 	str	r2, [r0, #24]
     7b0:	e5801000 	str	r1, [r0]
     7b4:	e580200c 	str	r2, [r0, #12]
     7b8:	e12fff1e 	bx	lr

000007bc <init_igps_ram>:
     7bc:	e59f3008 	ldr	r3, [pc, #8]	; 7cc <init_igps_ram+0x10>
     7c0:	e3a02001 	mov	r2, #1	; 0x1
     7c4:	e5832c10 	str	r2, [r3, #3088]
     7c8:	e12fff1e 	bx	lr
     7cc:	21e0a000 	.word	0x21e0a000

000007d0 <init_crypto>:
     7d0:	e12fff1e 	bx	lr

000007d4 <get_reset_stats>:
     7d4:	e59f2028 	ldr	r2, [pc, #40]	; 804 <get_reset_stats+0x30>
     7d8:	e5923c10 	ldr	r3, [r2, #3088]
     7dc:	e2130002 	ands	r0, r3, #2	; 0x2
     7e0:	012fff1e 	bxeq	lr
     7e4:	e5920c2c 	ldr	r0, [r2, #3116]
     7e8:	e3100001 	tst	r0, #1	; 0x1
     7ec:	03a00001 	moveq	r0, #1	; 0x1
     7f0:	012fff1e 	bxeq	lr
     7f4:	e3500037 	cmp	r0, #55	; 0x37
     7f8:	03a00002 	moveq	r0, #2	; 0x2
     7fc:	13a00003 	movne	r0, #3	; 0x3
     800:	e12fff1e 	bx	lr
     804:	21e09000 	.word	0x21e09000

00000808 <update_mask_on_reset>:
     808:	e59f3018 	ldr	r3, [pc, #24]	; 828 <update_mask_on_reset+0x20>
     80c:	e5932c30 	ldr	r2, [r3, #3120]
     810:	e3120080 	tst	r2, #128	; 0x80
     814:	13a03001 	movne	r3, #1	; 0x1
     818:	1202200f 	andne	r2, r2, #15	; 0xf
     81c:	11e03213 	mvnne	r3, r3, lsl r2
     820:	15803000 	strne	r3, [r0]
     824:	e12fff1e 	bx	lr
     828:	21e09000 	.word	0x21e09000

0000082c <get_wakeup_entry>:
     82c:	e59f301c 	ldr	r3, [pc, #28]	; 850 <get_wakeup_entry+0x24>
     830:	e5931c38 	ldr	r1, [r3, #3128]
     834:	e5932c34 	ldr	r2, [r3, #3124]
     838:	e5930c30 	ldr	r0, [r3, #3120]
     83c:	e1822401 	orr	r2, r2, r1, lsl #8
     840:	e5933c2c 	ldr	r3, [r3, #3116]
     844:	e1800402 	orr	r0, r0, r2, lsl #8
     848:	e1830400 	orr	r0, r3, r0, lsl #8
     84c:	e12fff1e 	bx	lr
     850:	21e09000 	.word	0x21e09000

00000854 <system_reset>:
     854:	e59f3010 	ldr	r3, [pc, #16]	; 86c <system_reset+0x18>
     858:	e3a02001 	mov	r2, #1	; 0x1
     85c:	e5832c2c 	str	r2, [r3, #3116]
     860:	e2822002 	add	r2, r2, #2	; 0x2
     864:	e5832c00 	str	r2, [r3, #3072]
     868:	e12fff1e 	bx	lr
     86c:	21e09000 	.word	0x21e09000

00000870 <pads_init_module>:
     870:	e92d4030 	push	{r4, r5, lr}
     874:	e1a05000 	mov	r5, r0
     878:	e24dd004 	sub	sp, sp, #4	; 0x4
     87c:	e59f404c 	ldr	r4, [pc, #76]	; 8d0 <pads_init_module+0x60>
     880:	ea00000d 	b	8bc <pads_init_module+0x4c>
     884:	e1500005 	cmp	r0, r5
     888:	1a00000a 	bne	8b8 <pads_init_module+0x48>
     88c:	e3a01000 	mov	r1, #0	; 0x0
     890:	e1a02001 	mov	r2, r1
     894:	e5140004 	ldr	r0, [r4, #-4]
     898:	eb0000fd 	bl	c94 <pads_chmod>
     89c:	e3a00101 	mov	r0, #1073741824	; 0x40000000
     8a0:	eb000063 	bl	a34 <ecfg_check_flag>
     8a4:	e3500000 	cmp	r0, #0	; 0x0
     8a8:	0a000002 	beq	8b8 <pads_init_module+0x48>
     8ac:	e8140005 	ldmda	r4, {r0, r2}
     8b0:	e3a01001 	mov	r1, #1	; 0x1
     8b4:	eb00014b 	bl	de8 <pads_pull>
     8b8:	e284400c 	add	r4, r4, #12	; 0xc
     8bc:	e5140008 	ldr	r0, [r4, #-8]
     8c0:	e3500000 	cmp	r0, #0	; 0x0
     8c4:	1affffee 	bne	884 <pads_init_module+0x14>
     8c8:	e28dd004 	add	sp, sp, #4	; 0x4
     8cc:	e8bd8030 	pop	{r4, r5, pc}
     8d0:	3c00d008 	.word	0x3c00d008

000008d4 <init_lowlevel>:
     8d4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     8d8:	e24dd044 	sub	sp, sp, #68	; 0x44
     8dc:	e3a01000 	mov	r1, #0	; 0x0
     8e0:	e3a02040 	mov	r2, #64	; 0x40
     8e4:	e1a0000d 	mov	r0, sp
     8e8:	eb000313 	bl	153c <memset>
     8ec:	e3a03063 	mov	r3, #99	; 0x63
     8f0:	e5cd3010 	strb	r3, [sp, #16]
     8f4:	e3e03073 	mvn	r3, #115	; 0x73
     8f8:	e5cd3011 	strb	r3, [sp, #17]
     8fc:	e28330a5 	add	r3, r3, #165	; 0xa5
     900:	e5cd3012 	strb	r3, [sp, #18]
     904:	e3a0301f 	mov	r3, #31	; 0x1f
     908:	e5cd3018 	strb	r3, [sp, #24]
     90c:	e283305d 	add	r3, r3, #93	; 0x5d
     910:	e5cd3019 	strb	r3, [sp, #25]
     914:	e3e0300f 	mvn	r3, #15	; 0xf
     918:	e5cd301a 	strb	r3, [sp, #26]
     91c:	e2833020 	add	r3, r3, #32	; 0x20
     920:	e5cd3020 	strb	r3, [sp, #32]
     924:	e2833032 	add	r3, r3, #50	; 0x32
     928:	e5cd3021 	strb	r3, [sp, #33]
     92c:	e3a03002 	mov	r3, #2	; 0x2
     930:	e5cd3029 	strb	r3, [sp, #41]
     934:	e3e0301f 	mvn	r3, #31	; 0x1f
     938:	e5cd3030 	strb	r3, [sp, #48]
     93c:	e3e0307c 	mvn	r3, #124	; 0x7c
     940:	e5cd3031 	strb	r3, [sp, #49]
     944:	e283308c 	add	r3, r3, #140	; 0x8c
     948:	e3a02008 	mov	r2, #8	; 0x8
     94c:	e5cd3032 	strb	r3, [sp, #50]
     950:	e3a03001 	mov	r3, #1	; 0x1
     954:	e5cd202a 	strb	r2, [sp, #42]
     958:	e58d3038 	str	r3, [sp, #56]
     95c:	e5cd2022 	strb	r2, [sp, #34]
     960:	e3a01000 	mov	r1, #0	; 0x0
     964:	e3e00000 	mvn	r0, #0	; 0x0
     968:	e59f3044 	ldr	r3, [pc, #68]	; 9b4 <init_lowlevel+0xe0>
     96c:	e1a02101 	lsl	r2, r1, #2
     970:	e2811001 	add	r1, r1, #1	; 0x1
     974:	e0823003 	add	r3, r2, r3
     978:	e3510008 	cmp	r1, #8	; 0x8
     97c:	e5830000 	str	r0, [r3]
     980:	1afffff8 	bne	968 <init_lowlevel+0x94>
     984:	e3012042 	movw	r2, #4162	; 0x1042
     988:	e28d0040 	add	r0, sp, #64	; 0x40
     98c:	e3013031 	movw	r3, #4145	; 0x1031
     990:	e16024b0 	strh	r2, [r0, #-64]!
     994:	e1cd30b4 	strh	r3, [sp, #4]
     998:	e1a0000d 	mov	r0, sp
     99c:	e302304f 	movw	r3, #8271	; 0x204f
     9a0:	e1cd30b2 	strh	r3, [sp, #2]
     9a4:	e1cd20b6 	strh	r2, [sp, #6]
     9a8:	eb0001ad 	bl	1064 <set_clk>
     9ac:	e28dd044 	add	sp, sp, #68	; 0x44
     9b0:	e8bd8000 	pop	{pc}
     9b4:	21b01180 	.word	0x21b01180

000009b8 <module_reset>:
     9b8:	e3a030ff 	mov	r3, #255	; 0xff
     9bc:	e5803000 	str	r3, [r0]
     9c0:	e92d4010 	push	{r4, lr}
     9c4:	e1a04000 	mov	r4, r0
     9c8:	e3a0000a 	mov	r0, #10	; 0xa
     9cc:	eb003404 	bl	d9e4 <udelay>
     9d0:	e3a03000 	mov	r3, #0	; 0x0
     9d4:	e5843000 	str	r3, [r4]
     9d8:	e8bd8010 	pop	{r4, pc}

000009dc <gtc_time>:
     9dc:	e59f3020 	ldr	r3, [pc, #32]	; a04 <gtc_time+0x28>
     9e0:	e92d4010 	push	{r4, lr}
     9e4:	e5930000 	ldr	r0, [r3]
     9e8:	e59f3018 	ldr	r3, [pc, #24]	; a08 <gtc_time+0x2c>
     9ec:	e5934000 	ldr	r4, [r3]
     9f0:	eb00341f 	bl	da74 <get_timer>
     9f4:	e1a01000 	mov	r1, r0
     9f8:	e59f000c 	ldr	r0, [pc, #12]	; a0c <gtc_time+0x30>
     9fc:	e12fff34 	blx	r4
     a00:	e8bd8010 	pop	{r4, pc}
     a04:	3c00d124 	.word	0x3c00d124
     a08:	3c00d6cc 	.word	0x3c00d6cc
     a0c:	0001616d 	.word	0x0001616d

00000a10 <init_gtc>:
     a10:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     a14:	e3a00000 	mov	r0, #0	; 0x0
     a18:	e24dd004 	sub	sp, sp, #4	; 0x4
     a1c:	eb003414 	bl	da74 <get_timer>
     a20:	e59f3008 	ldr	r3, [pc, #8]	; a30 <init_gtc+0x20>
     a24:	e5830000 	str	r0, [r3]
     a28:	e28dd004 	add	sp, sp, #4	; 0x4
     a2c:	e8bd8000 	pop	{pc}
     a30:	3c00d124 	.word	0x3c00d124

00000a34 <ecfg_check_flag>:
     a34:	e3500801 	cmp	r0, #65536	; 0x10000
     a38:	0a000004 	beq	a50 <ecfg_check_flag+0x1c>
     a3c:	e3500101 	cmp	r0, #1073741824	; 0x40000000
     a40:	0a000002 	beq	a50 <ecfg_check_flag+0x1c>
     a44:	e3500001 	cmp	r0, #1	; 0x1
     a48:	13a00001 	movne	r0, #1	; 0x1
     a4c:	112fff1e 	bxne	lr
     a50:	e3a00000 	mov	r0, #0	; 0x0
     a54:	e12fff1e 	bx	lr

00000a58 <ecfg_paras>:
     a58:	e59f0000 	ldr	r0, [pc, #0]	; a60 <ecfg_paras+0x8>
     a5c:	e12fff1e 	bx	lr
     a60:	3c00d6d0 	.word	0x3c00d6d0

00000a64 <efuse_mac>:
     a64:	e3a00000 	mov	r0, #0	; 0x0
     a68:	e3a01000 	mov	r1, #0	; 0x0
     a6c:	e12fff1e 	bx	lr

00000a70 <efuse_info>:
     a70:	e92d40f0 	push	{r4, r5, r6, r7, lr}
     a74:	e1a04000 	mov	r4, r0
     a78:	e24dd004 	sub	sp, sp, #4	; 0x4
     a7c:	e59f6144 	ldr	r6, [pc, #324]	; bc8 <efuse_info+0x158>
     a80:	e59f0144 	ldr	r0, [pc, #324]	; bcc <efuse_info+0x15c>
     a84:	e1a0e00f 	mov	lr, pc
     a88:	e596f000 	ldr	pc, [r6]
     a8c:	e59f013c 	ldr	r0, [pc, #316]	; bd0 <efuse_info+0x160>
     a90:	e1a0e00f 	mov	lr, pc
     a94:	e596f000 	ldr	pc, [r6]
     a98:	e3540000 	cmp	r4, #0	; 0x0
     a9c:	0a00003c 	beq	b94 <efuse_info+0x124>
     aa0:	e59f412c 	ldr	r4, [pc, #300]	; bd4 <efuse_info+0x164>
     aa4:	e59f012c 	ldr	r0, [pc, #300]	; bd8 <efuse_info+0x168>
     aa8:	e5941000 	ldr	r1, [r4]
     aac:	e1a0e00f 	mov	lr, pc
     ab0:	e596f000 	ldr	pc, [r6]
     ab4:	e5d41004 	ldrb	r1, [r4, #4]
     ab8:	e59f011c 	ldr	r0, [pc, #284]	; bdc <efuse_info+0x16c>
     abc:	e1a0e00f 	mov	lr, pc
     ac0:	e596f000 	ldr	pc, [r6]
     ac4:	e5d41005 	ldrb	r1, [r4, #5]
     ac8:	e59f0110 	ldr	r0, [pc, #272]	; be0 <efuse_info+0x170>
     acc:	e1a0e00f 	mov	lr, pc
     ad0:	e596f000 	ldr	pc, [r6]
     ad4:	e5d41006 	ldrb	r1, [r4, #6]
     ad8:	e59f0104 	ldr	r0, [pc, #260]	; be4 <efuse_info+0x174>
     adc:	e1a0e00f 	mov	lr, pc
     ae0:	e596f000 	ldr	pc, [r6]
     ae4:	e5d41007 	ldrb	r1, [r4, #7]
     ae8:	e59f00f8 	ldr	r0, [pc, #248]	; be8 <efuse_info+0x178>
     aec:	e1a0e00f 	mov	lr, pc
     af0:	e596f000 	ldr	pc, [r6]
     af4:	e1d410b8 	ldrh	r1, [r4, #8]
     af8:	e59f00ec 	ldr	r0, [pc, #236]	; bec <efuse_info+0x17c>
     afc:	e1a0e00f 	mov	lr, pc
     b00:	e596f000 	ldr	pc, [r6]
     b04:	e1d410ba 	ldrh	r1, [r4, #10]
     b08:	e59f00e0 	ldr	r0, [pc, #224]	; bf0 <efuse_info+0x180>
     b0c:	e1a0e00f 	mov	lr, pc
     b10:	e596f000 	ldr	pc, [r6]
     b14:	e1d410bc 	ldrh	r1, [r4, #12]
     b18:	e59f00d4 	ldr	r0, [pc, #212]	; bf4 <efuse_info+0x184>
     b1c:	e1a0e00f 	mov	lr, pc
     b20:	e596f000 	ldr	pc, [r6]
     b24:	e1d410be 	ldrh	r1, [r4, #14]
     b28:	e59f00c8 	ldr	r0, [pc, #200]	; bf8 <efuse_info+0x188>
     b2c:	e1a0e00f 	mov	lr, pc
     b30:	e596f000 	ldr	pc, [r6]
     b34:	e1a05004 	mov	r5, r4
     b38:	e3a04000 	mov	r4, #0	; 0x0
     b3c:	e1a01004 	mov	r1, r4
     b40:	e5952010 	ldr	r2, [r5, #16]
     b44:	e2844001 	add	r4, r4, #1	; 0x1
     b48:	e59f00ac 	ldr	r0, [pc, #172]	; bfc <efuse_info+0x18c>
     b4c:	e1a0e00f 	mov	lr, pc
     b50:	e596f000 	ldr	pc, [r6]
     b54:	e3540004 	cmp	r4, #4	; 0x4
     b58:	e2855004 	add	r5, r5, #4	; 0x4
     b5c:	1afffff6 	bne	b3c <efuse_info+0xcc>
     b60:	e59f6060 	ldr	r6, [pc, #96]	; bc8 <efuse_info+0x158>
     b64:	e59f5068 	ldr	r5, [pc, #104]	; bd4 <efuse_info+0x164>
     b68:	e284400c 	add	r4, r4, #12	; 0xc
     b6c:	e1a03084 	lsl	r3, r4, #1
     b70:	e1a01004 	mov	r1, r4
     b74:	e19520b3 	ldrh	r2, [r5, r3]
     b78:	e2844001 	add	r4, r4, #1	; 0x1
     b7c:	e59f007c 	ldr	r0, [pc, #124]	; c00 <efuse_info+0x190>
     b80:	e1a0e00f 	mov	lr, pc
     b84:	e596f000 	ldr	pc, [r6]
     b88:	e354001c 	cmp	r4, #28	; 0x1c
     b8c:	1afffff6 	bne	b6c <efuse_info+0xfc>
     b90:	ea00000a 	b	bc0 <efuse_info+0x150>
     b94:	e59f4068 	ldr	r4, [pc, #104]	; c04 <efuse_info+0x194>
     b98:	e59f7068 	ldr	r7, [pc, #104]	; c08 <efuse_info+0x198>
     b9c:	e1a05006 	mov	r5, r6
     ba0:	e5942000 	ldr	r2, [r4]
     ba4:	e1a01004 	mov	r1, r4
     ba8:	e59f005c 	ldr	r0, [pc, #92]	; c0c <efuse_info+0x19c>
     bac:	e2844004 	add	r4, r4, #4	; 0x4
     bb0:	e1a0e00f 	mov	lr, pc
     bb4:	e595f000 	ldr	pc, [r5]
     bb8:	e1540007 	cmp	r4, r7
     bbc:	1afffff7 	bne	ba0 <efuse_info+0x130>
     bc0:	e28dd004 	add	sp, sp, #4	; 0x4
     bc4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
     bc8:	3c00d6cc 	.word	0x3c00d6cc
     bcc:	00016175 	.word	0x00016175
     bd0:	000185ab 	.word	0x000185ab
     bd4:	3c00d6d0 	.word	0x3c00d6d0
     bd8:	0001618a 	.word	0x0001618a
     bdc:	00016199 	.word	0x00016199
     be0:	000161a8 	.word	0x000161a8
     be4:	000161bb 	.word	0x000161bb
     be8:	000161ca 	.word	0x000161ca
     bec:	000161df 	.word	0x000161df
     bf0:	000161ec 	.word	0x000161ec
     bf4:	000161fc 	.word	0x000161fc
     bf8:	0001620d 	.word	0x0001620d
     bfc:	0001621f 	.word	0x0001621f
     c00:	0001622f 	.word	0x0001622f
     c04:	21e0d9b4 	.word	0x21e0d9b4
     c08:	21e0da94 	.word	0x21e0da94
     c0c:	00016240 	.word	0x00016240

00000c10 <init_efuse>:
     c10:	e59f0008 	ldr	r0, [pc, #8]	; c20 <init_efuse+0x10>
     c14:	e3a01000 	mov	r1, #0	; 0x0
     c18:	e3a02038 	mov	r2, #56	; 0x38
     c1c:	ea000246 	b	153c <memset>
     c20:	3c00d6d0 	.word	0x3c00d6d0

00000c24 <assert_linkpc>:
     c24:	e59f300c 	ldr	r3, [pc, #12]	; c38 <assert_linkpc+0x14>
     c28:	e5930440 	ldr	r0, [r3, #1088]
     c2c:	e2700001 	rsbs	r0, r0, #1	; 0x1
     c30:	33a00000 	movcc	r0, #0	; 0x0
     c34:	e12fff1e 	bx	lr
     c38:	20f02000 	.word	0x20f02000

00000c3c <pads_states>:
     c3c:	e3500000 	cmp	r0, #0	; 0x0
     c40:	e2803007 	add	r3, r0, #7	; 0x7
     c44:	a1a03000 	movge	r3, r0
     c48:	e1a031c3 	asr	r3, r3, #3
     c4c:	e59f203c 	ldr	r2, [pc, #60]	; c90 <pads_states+0x54>
     c50:	e1a03103 	lsl	r3, r3, #2
     c54:	e0832002 	add	r2, r3, r2
     c58:	e5921000 	ldr	r1, [r2]
     c5c:	e3a03001 	mov	r3, #1	; 0x1
     c60:	e2002007 	and	r2, r0, #7	; 0x7
     c64:	e0113213 	ands	r3, r1, r3, lsl r2
     c68:	01a00003 	moveq	r0, r3
     c6c:	012fff1e 	bxeq	lr
     c70:	e1a03300 	lsl	r3, r0, #6
     c74:	e2833282 	add	r3, r3, #536870920	; 0x20000008
     c78:	e283360f 	add	r3, r3, #15728640	; 0xf00000
     c7c:	e5933000 	ldr	r3, [r3]
     c80:	e3530000 	cmp	r3, #0	; 0x0
     c84:	13a00002 	movne	r0, #2	; 0x2
     c88:	03a00001 	moveq	r0, #1	; 0x1
     c8c:	e12fff1e 	bx	lr
     c90:	21e09064 	.word	0x21e09064

00000c94 <pads_chmod>:
     c94:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
     c98:	e1b08840 	asrs	r8, r0, #16
     c9c:	e1a00800 	lsl	r0, r0, #16
     ca0:	03a03000 	moveq	r3, #0	; 0x0
     ca4:	13a03001 	movne	r3, #1	; 0x1
     ca8:	e1a00820 	lsr	r0, r0, #16
     cac:	e1580000 	cmp	r8, r0
     cb0:	a3a0c000 	movge	ip, #0	; 0x0
     cb4:	b203c001 	andlt	ip, r3, #1	; 0x1
     cb8:	e35c0000 	cmp	ip, #0	; 0x0
     cbc:	e24dd004 	sub	sp, sp, #4	; 0x4
     cc0:	e1a06001 	mov	r6, r1
     cc4:	e1a0b002 	mov	fp, r2
     cc8:	1a000040 	bne	dd0 <pads_chmod+0x13c>
     ccc:	e2803883 	add	r3, r0, #8585216	; 0x830000
     cd0:	e2833903 	add	r3, r3, #49152	; 0xc000
     cd4:	e1a02303 	lsl	r2, r3, #6
     cd8:	e3580000 	cmp	r8, #0	; 0x0
     cdc:	01a08000 	moveq	r8, r0
     ce0:	e2827008 	add	r7, r2, #8	; 0x8
     ce4:	e282a004 	add	sl, r2, #4	; 0x4
     ce8:	e1a04000 	mov	r4, r0
     cec:	e240501f 	sub	r5, r0, #31	; 0x1f
     cf0:	e3a09001 	mov	r9, #1	; 0x1
     cf4:	ea000033 	b	dc8 <pads_chmod+0x134>
     cf8:	e3540000 	cmp	r4, #0	; 0x0
     cfc:	e2843007 	add	r3, r4, #7	; 0x7
     d00:	a1a03004 	movge	r3, r4
     d04:	e3550006 	cmp	r5, #6	; 0x6
     d08:	959f20c8 	ldrls	r2, [pc, #200]	; dd8 <pads_chmod+0x144>
     d0c:	e1a011c3 	asr	r1, r3, #3
     d10:	95923008 	ldrls	r3, [r2, #8]
     d14:	e2040007 	and	r0, r4, #7	; 0x7
     d18:	91833519 	orrls	r3, r3, r9, lsl r5
     d1c:	95823008 	strls	r3, [r2, #8]
     d20:	e3560001 	cmp	r6, #1	; 0x1
     d24:	0a00000a 	beq	d54 <pads_chmod+0xc0>
     d28:	e3560002 	cmp	r6, #2	; 0x2
     d2c:	0a000011 	beq	d78 <pads_chmod+0xe4>
     d30:	e3560000 	cmp	r6, #0	; 0x0
     d34:	1a00001a 	bne	da4 <pads_chmod+0x110>
     d38:	e59f309c 	ldr	r3, [pc, #156]	; ddc <pads_chmod+0x148>
     d3c:	e1a02101 	lsl	r2, r1, #2
     d40:	e0823003 	add	r3, r2, r3
     d44:	e5932000 	ldr	r2, [r3]
     d48:	e1c22019 	bic	r2, r2, r9, lsl r0
     d4c:	e5832000 	str	r2, [r3]
     d50:	ea000018 	b	db8 <pads_chmod+0x124>
     d54:	e59f3080 	ldr	r3, [pc, #128]	; ddc <pads_chmod+0x148>
     d58:	e1a02101 	lsl	r2, r1, #2
     d5c:	e0823003 	add	r3, r2, r3
     d60:	e5932000 	ldr	r2, [r3]
     d64:	e1822016 	orr	r2, r2, r6, lsl r0
     d68:	e5832000 	str	r2, [r3]
     d6c:	e3a03000 	mov	r3, #0	; 0x0
     d70:	e5873000 	str	r3, [r7]
     d74:	ea00000f 	b	db8 <pads_chmod+0x124>
     d78:	e59f205c 	ldr	r2, [pc, #92]	; ddc <pads_chmod+0x148>
     d7c:	e1a03101 	lsl	r3, r1, #2
     d80:	e0832002 	add	r2, r3, r2
     d84:	e5923000 	ldr	r3, [r2]
     d88:	e25b1000 	subs	r1, fp, #0	; 0x0
     d8c:	13a01001 	movne	r1, #1	; 0x1
     d90:	e1833019 	orr	r3, r3, r9, lsl r0
     d94:	e5823000 	str	r3, [r2]
     d98:	e5879000 	str	r9, [r7]
     d9c:	e58a1000 	str	r1, [sl]
     da0:	ea000004 	b	db8 <pads_chmod+0x124>
     da4:	e59f0034 	ldr	r0, [pc, #52]	; de0 <pads_chmod+0x14c>
     da8:	e1a01006 	mov	r1, r6
     dac:	e59f2030 	ldr	r2, [pc, #48]	; de4 <pads_chmod+0x150>
     db0:	e1a0e00f 	mov	lr, pc
     db4:	e592f000 	ldr	pc, [r2]
     db8:	e2844001 	add	r4, r4, #1	; 0x1
     dbc:	e2877040 	add	r7, r7, #64	; 0x40
     dc0:	e28aa040 	add	sl, sl, #64	; 0x40
     dc4:	e2855001 	add	r5, r5, #1	; 0x1
     dc8:	e1540008 	cmp	r4, r8
     dcc:	daffffc9 	ble	cf8 <pads_chmod+0x64>
     dd0:	e28dd004 	add	sp, sp, #4	; 0x4
     dd4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
     dd8:	21e09000 	.word	0x21e09000
     ddc:	21e09064 	.word	0x21e09064
     de0:	00016250 	.word	0x00016250
     de4:	3c00d6cc 	.word	0x3c00d6cc

00000de8 <pads_pull>:
     de8:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
     dec:	e1b06840 	asrs	r6, r0, #16
     df0:	e1a00800 	lsl	r0, r0, #16
     df4:	03a03000 	moveq	r3, #0	; 0x0
     df8:	13a03001 	movne	r3, #1	; 0x1
     dfc:	e1a00820 	lsr	r0, r0, #16
     e00:	e1560000 	cmp	r6, r0
     e04:	a3a03000 	movge	r3, #0	; 0x0
     e08:	b2033001 	andlt	r3, r3, #1	; 0x1
     e0c:	e3530000 	cmp	r3, #0	; 0x0
     e10:	e1a09001 	mov	r9, r1
     e14:	e1a0a002 	mov	sl, r2
     e18:	18bd87f0 	popne	{r4, r5, r6, r7, r8, r9, sl, pc}
     e1c:	e3560000 	cmp	r6, #0	; 0x0
     e20:	01a06000 	moveq	r6, r0
     e24:	e1a0c000 	mov	ip, r0
     e28:	e1a07003 	mov	r7, r3
     e2c:	e240501f 	sub	r5, r0, #31	; 0x1f
     e30:	e3a08001 	mov	r8, #1	; 0x1
     e34:	ea000020 	b	ebc <pads_pull+0xd4>
     e38:	e35c0000 	cmp	ip, #0	; 0x0
     e3c:	e28c3007 	add	r3, ip, #7	; 0x7
     e40:	a1a0300c 	movge	r3, ip
     e44:	e1a031c3 	asr	r3, r3, #3
     e48:	e3590000 	cmp	r9, #0	; 0x0
     e4c:	e1a00103 	lsl	r0, r3, #2
     e50:	e20c4007 	and	r4, ip, #7	; 0x7
     e54:	059f206c 	ldreq	r2, [pc, #108]	; ec8 <pads_pull+0xe0>
     e58:	00802002 	addeq	r2, r0, r2
     e5c:	05923000 	ldreq	r3, [r2]
     e60:	01c33418 	biceq	r3, r3, r8, lsl r4
     e64:	0a000010 	beq	eac <pads_pull+0xc4>
     e68:	e3550006 	cmp	r5, #6	; 0x6
     e6c:	859f2058 	ldrhi	r2, [pc, #88]	; ecc <pads_pull+0xe4>
     e70:	91a03005 	movls	r3, r5
     e74:	81a03004 	movhi	r3, r4
     e78:	959f2050 	ldrls	r2, [pc, #80]	; ed0 <pads_pull+0xe8>
     e7c:	80802002 	addhi	r2, r0, r2
     e80:	e01a1718 	ands	r1, sl, r8, lsl r7
     e84:	e1a01318 	lsl	r1, r8, r3
     e88:	15923000 	ldrne	r3, [r2]
     e8c:	05923000 	ldreq	r3, [r2]
     e90:	11813003 	orrne	r3, r1, r3
     e94:	01c33001 	biceq	r3, r3, r1
     e98:	e5823000 	str	r3, [r2]
     e9c:	e59f2024 	ldr	r2, [pc, #36]	; ec8 <pads_pull+0xe0>
     ea0:	e0802002 	add	r2, r0, r2
     ea4:	e5923000 	ldr	r3, [r2]
     ea8:	e1833418 	orr	r3, r3, r8, lsl r4
     eac:	e5823000 	str	r3, [r2]
     eb0:	e28cc001 	add	ip, ip, #1	; 0x1
     eb4:	e2877001 	add	r7, r7, #1	; 0x1
     eb8:	e2855001 	add	r5, r5, #1	; 0x1
     ebc:	e15c0006 	cmp	ip, r6
     ec0:	daffffdc 	ble	e38 <pads_pull+0x50>
     ec4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
     ec8:	21e09014 	.word	0x21e09014
     ecc:	21e091a4 	.word	0x21e091a4
     ed0:	21e0900c 	.word	0x21e0900c

00000ed4 <pads_setpin>:
     ed4:	e1b0c840 	asrs	ip, r0, #16
     ed8:	e1a00800 	lsl	r0, r0, #16
     edc:	03a03000 	moveq	r3, #0	; 0x0
     ee0:	13a03001 	movne	r3, #1	; 0x1
     ee4:	e1a00820 	lsr	r0, r0, #16
     ee8:	e15c0000 	cmp	ip, r0
     eec:	a3a03000 	movge	r3, #0	; 0x0
     ef0:	b2033001 	andlt	r3, r3, #1	; 0x1
     ef4:	e3530000 	cmp	r3, #0	; 0x0
     ef8:	112fff1e 	bxne	lr
     efc:	e1a03300 	lsl	r3, r0, #6
     f00:	e2832242 	add	r2, r3, #536870916	; 0x20000004
     f04:	e35c0000 	cmp	ip, #0	; 0x0
     f08:	01a0c000 	moveq	ip, r0
     f0c:	e282260f 	add	r2, r2, #15728640	; 0xf00000
     f10:	ea000002 	b	f20 <pads_setpin+0x4c>
     f14:	e2513000 	subs	r3, r1, #0	; 0x0
     f18:	13a03001 	movne	r3, #1	; 0x1
     f1c:	e4823040 	str	r3, [r2], #64
     f20:	e150000c 	cmp	r0, ip
     f24:	e2800001 	add	r0, r0, #1	; 0x1
     f28:	dafffff9 	ble	f14 <pads_setpin+0x40>
     f2c:	e12fff1e 	bx	lr

00000f30 <pads_getpin>:
     f30:	e59f3014 	ldr	r3, [pc, #20]	; f4c <pads_getpin+0x1c>
     f34:	e6f30070 	uxtah	r0, r3, r0
     f38:	e1a00300 	lsl	r0, r0, #6
     f3c:	e5900000 	ldr	r0, [r0]
     f40:	e2500000 	subs	r0, r0, #0	; 0x0
     f44:	13a00001 	movne	r0, #1	; 0x1
     f48:	e12fff1e 	bx	lr
     f4c:	0083c000 	.word	0x0083c000

00000f50 <init_alternate>:
     f50:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
     f54:	e59f0030 	ldr	r0, [pc, #48]	; f8c <init_alternate+0x3c>
     f58:	e24dd004 	sub	sp, sp, #4	; 0x4
     f5c:	ebfffe08 	bl	784 <module_enable>
     f60:	e3a00091 	mov	r0, #145	; 0x91
     f64:	e3a01001 	mov	r1, #1	; 0x1
     f68:	e3a02000 	mov	r2, #0	; 0x0
     f6c:	ebffff48 	bl	c94 <pads_chmod>
     f70:	e3a01001 	mov	r1, #1	; 0x1
     f74:	e3a00091 	mov	r0, #145	; 0x91
     f78:	e1a02001 	mov	r2, r1
     f7c:	ebffff99 	bl	de8 <pads_pull>
     f80:	e3a00000 	mov	r0, #0	; 0x0
     f84:	e28dd004 	add	sp, sp, #4	; 0x4
     f88:	e8bd8000 	pop	{pc}
     f8c:	21e13c00 	.word	0x21e13c00

00000f90 <cpu_recase>:
     f90:	e92d4010 	push	{r4, lr}
     f94:	e59f10a8 	ldr	r1, [pc, #168]	; 1044 <cpu_recase+0xb4>
     f98:	e3a02003 	mov	r2, #3	; 0x3
     f9c:	e1a04000 	mov	r4, r0
     fa0:	eb000134 	bl	1478 <strncmp>
     fa4:	e3500000 	cmp	r0, #0	; 0x0
     fa8:	0a00000b 	beq	fdc <cpu_recase+0x4c>
     fac:	e1a00004 	mov	r0, r4
     fb0:	e59f1090 	ldr	r1, [pc, #144]	; 1048 <cpu_recase+0xb8>
     fb4:	e3a02003 	mov	r2, #3	; 0x3
     fb8:	eb00012e 	bl	1478 <strncmp>
     fbc:	e3500000 	cmp	r0, #0	; 0x0
     fc0:	0a000005 	beq	fdc <cpu_recase+0x4c>
     fc4:	e1a00004 	mov	r0, r4
     fc8:	e59f107c 	ldr	r1, [pc, #124]	; 104c <cpu_recase+0xbc>
     fcc:	e3a02003 	mov	r2, #3	; 0x3
     fd0:	eb000128 	bl	1478 <strncmp>
     fd4:	e3500000 	cmp	r0, #0	; 0x0
     fd8:	1a000001 	bne	fe4 <cpu_recase+0x54>
     fdc:	e3a02002 	mov	r2, #2	; 0x2
     fe0:	ea000006 	b	1000 <cpu_recase+0x70>
     fe4:	e1a00004 	mov	r0, r4
     fe8:	e59f1060 	ldr	r1, [pc, #96]	; 1050 <cpu_recase+0xc0>
     fec:	e3a02004 	mov	r2, #4	; 0x4
     ff0:	eb000120 	bl	1478 <strncmp>
     ff4:	e3500000 	cmp	r0, #0	; 0x0
     ff8:	18bd8010 	popne	{r4, pc}
     ffc:	e3a02001 	mov	r2, #1	; 0x1
    1000:	e59f304c 	ldr	r3, [pc, #76]	; 1054 <cpu_recase+0xc4>
    1004:	e59f4048 	ldr	r4, [pc, #72]	; 1054 <cpu_recase+0xc4>
    1008:	e59f1048 	ldr	r1, [pc, #72]	; 1058 <cpu_recase+0xc8>
    100c:	e5832000 	str	r2, [r3]
    1010:	e5942000 	ldr	r2, [r4]
    1014:	e5913000 	ldr	r3, [r1]
    1018:	e1520003 	cmp	r2, r3
    101c:	08bd8010 	popeq	{r4, pc}
    1020:	e1a00002 	mov	r0, r2
    1024:	e5812000 	str	r2, [r1]
    1028:	eb0000d6 	bl	1388 <set_mux>
    102c:	e5941000 	ldr	r1, [r4]
    1030:	e59f0024 	ldr	r0, [pc, #36]	; 105c <cpu_recase+0xcc>
    1034:	e59f3024 	ldr	r3, [pc, #36]	; 1060 <cpu_recase+0xd0>
    1038:	e1a0e00f 	mov	lr, pc
    103c:	e593f000 	ldr	pc, [r3]
    1040:	e8bd8010 	pop	{r4, pc}
    1044:	0001610f 	.word	0x0001610f
    1048:	00016267 	.word	0x00016267
    104c:	0001626b 	.word	0x0001626b
    1050:	0001626f 	.word	0x0001626f
    1054:	3c00d128 	.word	0x3c00d128
    1058:	3c00d084 	.word	0x3c00d084
    105c:	00016274 	.word	0x00016274
    1060:	3c00d6cc 	.word	0x3c00d6cc

00001064 <set_clk>:
    1064:	e92d4030 	push	{r4, r5, lr}
    1068:	e1d010b2 	ldrh	r1, [r0, #2]
    106c:	e24dd004 	sub	sp, sp, #4	; 0x4
    1070:	e3510000 	cmp	r1, #0	; 0x0
    1074:	e1a04000 	mov	r4, r0
    1078:	0a000001 	beq	1084 <set_clk+0x20>
    107c:	e3a00001 	mov	r0, #1	; 0x1
    1080:	ebfffd76 	bl	660 <set_pll>
    1084:	e1d410b4 	ldrh	r1, [r4, #4]
    1088:	e3510000 	cmp	r1, #0	; 0x0
    108c:	0a000001 	beq	1098 <set_clk+0x34>
    1090:	e3a00002 	mov	r0, #2	; 0x2
    1094:	ebfffd71 	bl	660 <set_pll>
    1098:	e1d410b6 	ldrh	r1, [r4, #6]
    109c:	e3510000 	cmp	r1, #0	; 0x0
    10a0:	0a000001 	beq	10ac <set_clk+0x48>
    10a4:	e3a00003 	mov	r0, #3	; 0x3
    10a8:	ebfffd6c 	bl	660 <set_pll>
    10ac:	e59f00c4 	ldr	r0, [pc, #196]	; 1178 <set_clk+0x114>
    10b0:	e3a01000 	mov	r1, #0	; 0x0
    10b4:	e3a02003 	mov	r2, #3	; 0x3
    10b8:	ebfffda6 	bl	758 <module_set_clock>
    10bc:	e59f00b8 	ldr	r0, [pc, #184]	; 117c <set_clk+0x118>
    10c0:	e3a01001 	mov	r1, #1	; 0x1
    10c4:	e3a0200b 	mov	r2, #11	; 0xb
    10c8:	ebfffda2 	bl	758 <module_set_clock>
    10cc:	e59f00ac 	ldr	r0, [pc, #172]	; 1180 <set_clk+0x11c>
    10d0:	e3a01001 	mov	r1, #1	; 0x1
    10d4:	e3a02005 	mov	r2, #5	; 0x5
    10d8:	ebfffd9e 	bl	758 <module_set_clock>
    10dc:	e59f00a0 	ldr	r0, [pc, #160]	; 1184 <set_clk+0x120>
    10e0:	e3a01001 	mov	r1, #1	; 0x1
    10e4:	e3a02013 	mov	r2, #19	; 0x13
    10e8:	ebfffd9a 	bl	758 <module_set_clock>
    10ec:	e59f0094 	ldr	r0, [pc, #148]	; 1188 <set_clk+0x124>
    10f0:	e3a01002 	mov	r1, #2	; 0x2
    10f4:	e3a0200f 	mov	r2, #15	; 0xf
    10f8:	ebfffd96 	bl	758 <module_set_clock>
    10fc:	e59f0088 	ldr	r0, [pc, #136]	; 118c <set_clk+0x128>
    1100:	e3a01002 	mov	r1, #2	; 0x2
    1104:	e3a02005 	mov	r2, #5	; 0x5
    1108:	ebfffd92 	bl	758 <module_set_clock>
    110c:	e59f007c 	ldr	r0, [pc, #124]	; 1190 <set_clk+0x12c>
    1110:	e3a01002 	mov	r1, #2	; 0x2
    1114:	e3a0200b 	mov	r2, #11	; 0xb
    1118:	ebfffd8e 	bl	758 <module_set_clock>
    111c:	e59f0070 	ldr	r0, [pc, #112]	; 1194 <set_clk+0x130>
    1120:	e3a01002 	mov	r1, #2	; 0x2
    1124:	e3a02019 	mov	r2, #25	; 0x19
    1128:	ebfffd8a 	bl	758 <module_set_clock>
    112c:	e59f4064 	ldr	r4, [pc, #100]	; 1198 <set_clk+0x134>
    1130:	e59f5040 	ldr	r5, [pc, #64]	; 1178 <set_clk+0x114>
    1134:	e3a01003 	mov	r1, #3	; 0x3
    1138:	e1a00004 	mov	r0, r4
    113c:	e1a02001 	mov	r2, r1
    1140:	e2844010 	add	r4, r4, #16	; 0x10
    1144:	ebfffd83 	bl	758 <module_set_clock>
    1148:	e1540005 	cmp	r4, r5
    114c:	1afffff8 	bne	1134 <set_clk+0xd0>
    1150:	e59f0044 	ldr	r0, [pc, #68]	; 119c <set_clk+0x138>
    1154:	e3a01003 	mov	r1, #3	; 0x3
    1158:	e3a02013 	mov	r2, #19	; 0x13
    115c:	ebfffd7d 	bl	758 <module_set_clock>
    1160:	e59f0038 	ldr	r0, [pc, #56]	; 11a0 <set_clk+0x13c>
    1164:	e3a01000 	mov	r1, #0	; 0x0
    1168:	e3a02005 	mov	r2, #5	; 0x5
    116c:	e28dd004 	add	sp, sp, #4	; 0x4
    1170:	e8bd4030 	pop	{r4, r5, lr}
    1174:	eafffd77 	b	758 <module_set_clock>
    1178:	21e0a190 	.word	0x21e0a190
    117c:	21e0a250 	.word	0x21e0a250
    1180:	21e0a270 	.word	0x21e0a270
    1184:	21e0a2c0 	.word	0x21e0a2c0
    1188:	21e0a240 	.word	0x21e0a240
    118c:	21e0a2f0 	.word	0x21e0a2f0
    1190:	21e0a2e0 	.word	0x21e0a2e0
    1194:	21e0a290 	.word	0x21e0a290
    1198:	21e0a130 	.word	0x21e0a130
    119c:	21e0a260 	.word	0x21e0a260
    11a0:	21e0a360 	.word	0x21e0a360

000011a4 <chip_test>:
    11a4:	e59f0044 	ldr	r0, [pc, #68]	; 11f0 <end+0x4>
    11a8:	e5901000 	ldr	r1, [r0]
    11ac:	e59f2040 	ldr	r2, [pc, #64]	; 11f4 <end+0x8>
    11b0:	e1520001 	cmp	r2, r1
    11b4:	1a00000c 	bne	11ec <end>
    11b8:	e5901004 	ldr	r1, [r0, #4]
    11bc:	e59f2034 	ldr	r2, [pc, #52]	; 11f8 <end+0xc>
    11c0:	e1520001 	cmp	r2, r1
    11c4:	1a000008 	bne	11ec <end>
    11c8:	e5901008 	ldr	r1, [r0, #8]
    11cc:	e59f2028 	ldr	r2, [pc, #40]	; 11fc <end+0x10>
    11d0:	e1520001 	cmp	r2, r1
    11d4:	1a000004 	bne	11ec <end>
    11d8:	e590100c 	ldr	r1, [r0, #12]
    11dc:	e59f201c 	ldr	r2, [pc, #28]	; 1200 <end+0x14>
    11e0:	e1520001 	cmp	r2, r1
    11e4:	1a000000 	bne	11ec <end>
    11e8:	e3a0f30f 	mov	pc, #1006632960	; 0x3c000000

000011ec <end>:
    11ec:	e1a0f00e 	mov	pc, lr
    11f0:	3c007ff0 	.word	0x3c007ff0
    11f4:	6f666e69 	.word	0x6f666e69
    11f8:	612d6d74 	.word	0x612d6d74
    11fc:	2d6e696c 	.word	0x2d6e696c
    1200:	796c6e6f 	.word	0x796c6e6f

00001204 <module_get_clock>:
    1204:	e59f3134 	ldr	r3, [pc, #308]	; 1340 <module_get_clock+0x13c>
    1208:	e1500003 	cmp	r0, r3
    120c:	0a000041 	beq	1318 <module_get_clock+0x114>
    1210:	8a000017 	bhi	1274 <module_get_clock+0x70>
    1214:	e24330f0 	sub	r3, r3, #240	; 0xf0
    1218:	e1500003 	cmp	r0, r3
    121c:	0a000039 	beq	1308 <module_get_clock+0x104>
    1220:	8a000007 	bhi	1244 <module_get_clock+0x40>
    1224:	e3500003 	cmp	r0, #3	; 0x3
    1228:	059f0114 	ldreq	r0, [pc, #276]	; 1344 <module_get_clock+0x140>
    122c:	012fff1e 	bxeq	lr
    1230:	859f3110 	ldrhi	r3, [pc, #272]	; 1348 <module_get_clock+0x144>
    1234:	8a000008 	bhi	125c <module_get_clock+0x58>
    1238:	e3500001 	cmp	r0, #1	; 0x1
    123c:	1a000033 	bne	1310 <module_get_clock+0x10c>
    1240:	ea00003c 	b	1338 <module_get_clock+0x134>
    1244:	e59f3100 	ldr	r3, [pc, #256]	; 134c <module_get_clock+0x148>
    1248:	e1500003 	cmp	r0, r3
    124c:	0a00002d 	beq	1308 <module_get_clock+0x104>
    1250:	92433010 	subls	r3, r3, #16	; 0x10
    1254:	9a000003 	bls	1268 <module_get_clock+0x64>
    1258:	e59f30f0 	ldr	r3, [pc, #240]	; 1350 <module_get_clock+0x14c>
    125c:	e1500003 	cmp	r0, r3
    1260:	0a000028 	beq	1308 <module_get_clock+0x104>
    1264:	e2833010 	add	r3, r3, #16	; 0x10
    1268:	e1500003 	cmp	r0, r3
    126c:	1a000027 	bne	1310 <module_get_clock+0x10c>
    1270:	ea000024 	b	1308 <module_get_clock+0x104>
    1274:	e59f30d8 	ldr	r3, [pc, #216]	; 1354 <module_get_clock+0x150>
    1278:	e1500003 	cmp	r0, r3
    127c:	059f00d4 	ldreq	r0, [pc, #212]	; 1358 <module_get_clock+0x154>
    1280:	012fff1e 	bxeq	lr
    1284:	8a00000f 	bhi	12c8 <module_get_clock+0xc4>
    1288:	e2433060 	sub	r3, r3, #96	; 0x60
    128c:	e1500003 	cmp	r0, r3
    1290:	0a000020 	beq	1318 <module_get_clock+0x114>
    1294:	8a000003 	bhi	12a8 <module_get_clock+0xa4>
    1298:	e2433010 	sub	r3, r3, #16	; 0x10
    129c:	e1500003 	cmp	r0, r3
    12a0:	1a00001a 	bne	1310 <module_get_clock+0x10c>
    12a4:	ea00001b 	b	1318 <module_get_clock+0x114>
    12a8:	e59f30ac 	ldr	r3, [pc, #172]	; 135c <module_get_clock+0x158>
    12ac:	e1500003 	cmp	r0, r3
    12b0:	059f00a8 	ldreq	r0, [pc, #168]	; 1360 <module_get_clock+0x15c>
    12b4:	012fff1e 	bxeq	lr
    12b8:	e2833020 	add	r3, r3, #32	; 0x20
    12bc:	e1500003 	cmp	r0, r3
    12c0:	1a000012 	bne	1310 <module_get_clock+0x10c>
    12c4:	ea000015 	b	1320 <module_get_clock+0x11c>
    12c8:	e59f3094 	ldr	r3, [pc, #148]	; 1364 <module_get_clock+0x160>
    12cc:	e1500003 	cmp	r0, r3
    12d0:	059f0090 	ldreq	r0, [pc, #144]	; 1368 <module_get_clock+0x164>
    12d4:	012fff1e 	bxeq	lr
    12d8:	8a000003 	bhi	12ec <module_get_clock+0xe8>
    12dc:	e2433020 	sub	r3, r3, #32	; 0x20
    12e0:	e1500003 	cmp	r0, r3
    12e4:	1a000009 	bne	1310 <module_get_clock+0x10c>
    12e8:	ea00000e 	b	1328 <module_get_clock+0x124>
    12ec:	e59f3078 	ldr	r3, [pc, #120]	; 136c <module_get_clock+0x168>
    12f0:	e1500003 	cmp	r0, r3
    12f4:	0a000003 	beq	1308 <module_get_clock+0x104>
    12f8:	e2833040 	add	r3, r3, #64	; 0x40
    12fc:	e1500003 	cmp	r0, r3
    1300:	1a000002 	bne	1310 <module_get_clock+0x10c>
    1304:	ea000009 	b	1330 <module_get_clock+0x12c>
    1308:	e59f0060 	ldr	r0, [pc, #96]	; 1370 <module_get_clock+0x16c>
    130c:	e12fff1e 	bx	lr
    1310:	e3a00000 	mov	r0, #0	; 0x0
    1314:	e12fff1e 	bx	lr
    1318:	e59f0054 	ldr	r0, [pc, #84]	; 1374 <module_get_clock+0x170>
    131c:	e12fff1e 	bx	lr
    1320:	e59f0050 	ldr	r0, [pc, #80]	; 1378 <module_get_clock+0x174>
    1324:	e12fff1e 	bx	lr
    1328:	e59f004c 	ldr	r0, [pc, #76]	; 137c <module_get_clock+0x178>
    132c:	e12fff1e 	bx	lr
    1330:	e59f0048 	ldr	r0, [pc, #72]	; 1380 <module_get_clock+0x17c>
    1334:	e12fff1e 	bx	lr
    1338:	e59f0044 	ldr	r0, [pc, #68]	; 1384 <module_get_clock+0x180>
    133c:	e12fff1e 	bx	lr
    1340:	21e0a240 	.word	0x21e0a240
    1344:	05fd8220 	.word	0x05fd8220
    1348:	21e0a130 	.word	0x21e0a130
    134c:	21e0a170 	.word	0x21e0a170
    1350:	21e0a180 	.word	0x21e0a180
    1354:	21e0a2c0 	.word	0x21e0a2c0
    1358:	016e3600 	.word	0x016e3600
    135c:	21e0a270 	.word	0x21e0a270
    1360:	04c4b400 	.word	0x04c4b400
    1364:	21e0a2f0 	.word	0x21e0a2f0
    1368:	05f5e100 	.word	0x05f5e100
    136c:	21e0a320 	.word	0x21e0a320
    1370:	0bfb0440 	.word	0x0bfb0440
    1374:	02625a00 	.word	0x02625a00
    1378:	0160203b 	.word	0x0160203b
    137c:	02faf080 	.word	0x02faf080
    1380:	07fcad80 	.word	0x07fcad80
    1384:	0ff95b00 	.word	0x0ff95b00

00001388 <set_mux>:
    1388:	e59f3004 	ldr	r3, [pc, #4]	; 1394 <set_mux+0xc>
    138c:	e5830000 	str	r0, [r3]
    1390:	e12fff1e 	bx	lr
    1394:	21e09000 	.word	0x21e09000

00001398 <strcpy>:
    1398:	e3a02000 	mov	r2, #0	; 0x0
    139c:	e7d13002 	ldrb	r3, [r1, r2]
    13a0:	e3530000 	cmp	r3, #0	; 0x0
    13a4:	e7c03002 	strb	r3, [r0, r2]
    13a8:	e2822001 	add	r2, r2, #1	; 0x1
    13ac:	1afffffa 	bne	139c <strcpy+0x4>
    13b0:	e12fff1e 	bx	lr

000013b4 <strncpy>:
    13b4:	e3a0c000 	mov	ip, #0	; 0x0
    13b8:	e3520000 	cmp	r2, #0	; 0x0
    13bc:	e2422001 	sub	r2, r2, #1	; 0x1
    13c0:	012fff1e 	bxeq	lr
    13c4:	e7d1300c 	ldrb	r3, [r1, ip]
    13c8:	e3530000 	cmp	r3, #0	; 0x0
    13cc:	e7c0300c 	strb	r3, [r0, ip]
    13d0:	e28cc001 	add	ip, ip, #1	; 0x1
    13d4:	1afffff7 	bne	13b8 <strncpy+0x4>
    13d8:	e12fff1e 	bx	lr

000013dc <strncat>:
    13dc:	e92d4030 	push	{r4, r5, lr}
    13e0:	e2525000 	subs	r5, r2, #0	; 0x0
    13e4:	e1a04001 	mov	r4, r1
    13e8:	11a0c000 	movne	ip, r0
    13ec:	1a000001 	bne	13f8 <strncat+0x1c>
    13f0:	e8bd8030 	pop	{r4, r5, pc}
    13f4:	e28cc001 	add	ip, ip, #1	; 0x1
    13f8:	e5dc3000 	ldrb	r3, [ip]
    13fc:	e3530000 	cmp	r3, #0	; 0x0
    1400:	1afffffb 	bne	13f4 <strncat+0x18>
    1404:	ea000004 	b	141c <strncat+0x40>
    1408:	e1550002 	cmp	r5, r2
    140c:	1a000004 	bne	1424 <strncat+0x48>
    1410:	e3a03000 	mov	r3, #0	; 0x0
    1414:	e5cc3000 	strb	r3, [ip]
    1418:	e8bd8030 	pop	{r4, r5, pc}
    141c:	e1a02003 	mov	r2, r3
    1420:	e1a0100c 	mov	r1, ip
    1424:	e7d43002 	ldrb	r3, [r4, r2]
    1428:	e28cc001 	add	ip, ip, #1	; 0x1
    142c:	e3530000 	cmp	r3, #0	; 0x0
    1430:	e7c13002 	strb	r3, [r1, r2]
    1434:	e2822001 	add	r2, r2, #1	; 0x1
    1438:	1afffff2 	bne	1408 <strncat+0x2c>
    143c:	e8bd8030 	pop	{r4, r5, pc}

00001440 <strcmp>:
    1440:	e92d4010 	push	{r4, lr}
    1444:	e3a02000 	mov	r2, #0	; 0x0
    1448:	e1a04000 	mov	r4, r0
    144c:	e7d4c002 	ldrb	ip, [r4, r2]
    1450:	e7d13002 	ldrb	r3, [r1, r2]
    1454:	e2822001 	add	r2, r2, #1	; 0x1
    1458:	e063300c 	rsb	r3, r3, ip
    145c:	e6ef0073 	uxtb	r0, r3
    1460:	e3500000 	cmp	r0, #0	; 0x0
    1464:	1a000001 	bne	1470 <strcmp+0x30>
    1468:	e35c0000 	cmp	ip, #0	; 0x0
    146c:	1afffff6 	bne	144c <strcmp+0xc>
    1470:	e6af0070 	sxtb	r0, r0
    1474:	e8bd8010 	pop	{r4, pc}

00001478 <strncmp>:
    1478:	e92d4030 	push	{r4, r5, lr}
    147c:	e3a0c000 	mov	ip, #0	; 0x0
    1480:	e1a05000 	mov	r5, r0
    1484:	e1a04001 	mov	r4, r1
    1488:	ea000009 	b	14b4 <strncmp+0x3c>
    148c:	e7d5100c 	ldrb	r1, [r5, ip]
    1490:	e7d4300c 	ldrb	r3, [r4, ip]
    1494:	e2422001 	sub	r2, r2, #1	; 0x1
    1498:	e0633001 	rsb	r3, r3, r1
    149c:	e6ef0073 	uxtb	r0, r3
    14a0:	e3500000 	cmp	r0, #0	; 0x0
    14a4:	e28cc001 	add	ip, ip, #1	; 0x1
    14a8:	1a000004 	bne	14c0 <strncmp+0x48>
    14ac:	e3510000 	cmp	r1, #0	; 0x0
    14b0:	0a000002 	beq	14c0 <strncmp+0x48>
    14b4:	e3520000 	cmp	r2, #0	; 0x0
    14b8:	1afffff3 	bne	148c <strncmp+0x14>
    14bc:	e1a00002 	mov	r0, r2
    14c0:	e6af0070 	sxtb	r0, r0
    14c4:	e8bd8030 	pop	{r4, r5, pc}

000014c8 <strchr>:
    14c8:	e1a03000 	mov	r3, r0
    14cc:	e6ef1071 	uxtb	r1, r1
    14d0:	ea000002 	b	14e0 <strchr+0x18>
    14d4:	e3500000 	cmp	r0, #0	; 0x0
    14d8:	012fff1e 	bxeq	lr
    14dc:	e2833001 	add	r3, r3, #1	; 0x1
    14e0:	e5d30000 	ldrb	r0, [r3]
    14e4:	e1500001 	cmp	r0, r1
    14e8:	1afffff9 	bne	14d4 <strchr+0xc>
    14ec:	e1a00003 	mov	r0, r3
    14f0:	e12fff1e 	bx	lr

000014f4 <strlen>:
    14f4:	e1a02000 	mov	r2, r0
    14f8:	ea000000 	b	1500 <strlen+0xc>
    14fc:	e2800001 	add	r0, r0, #1	; 0x1
    1500:	e5d03000 	ldrb	r3, [r0]
    1504:	e3530000 	cmp	r3, #0	; 0x0
    1508:	1afffffb 	bne	14fc <strlen+0x8>
    150c:	e0620000 	rsb	r0, r2, r0
    1510:	e12fff1e 	bx	lr

00001514 <strnlen>:
    1514:	e3a02000 	mov	r2, #0	; 0x0
    1518:	e1510002 	cmp	r1, r2
    151c:	e080c002 	add	ip, r0, r2
    1520:	0a000003 	beq	1534 <strnlen+0x20>
    1524:	e7d03002 	ldrb	r3, [r0, r2]
    1528:	e2822001 	add	r2, r2, #1	; 0x1
    152c:	e3530000 	cmp	r3, #0	; 0x0
    1530:	1afffff8 	bne	1518 <strnlen+0x4>
    1534:	e060000c 	rsb	r0, r0, ip
    1538:	e12fff1e 	bx	lr

0000153c <memset>:
    153c:	e3a03000 	mov	r3, #0	; 0x0
    1540:	ea000001 	b	154c <memset+0x10>
    1544:	e7c01003 	strb	r1, [r0, r3]
    1548:	e2833001 	add	r3, r3, #1	; 0x1
    154c:	e3520000 	cmp	r2, #0	; 0x0
    1550:	e2422001 	sub	r2, r2, #1	; 0x1
    1554:	1afffffa 	bne	1544 <memset+0x8>
    1558:	e12fff1e 	bx	lr

0000155c <memcpy>:
    155c:	e3a0c000 	mov	ip, #0	; 0x0
    1560:	ea000002 	b	1570 <memcpy+0x14>
    1564:	e7d1300c 	ldrb	r3, [r1, ip]
    1568:	e7c0300c 	strb	r3, [r0, ip]
    156c:	e28cc001 	add	ip, ip, #1	; 0x1
    1570:	e3520000 	cmp	r2, #0	; 0x0
    1574:	e2422001 	sub	r2, r2, #1	; 0x1
    1578:	1afffff9 	bne	1564 <memcpy+0x8>
    157c:	e12fff1e 	bx	lr

00001580 <memmove>:
    1580:	e1500001 	cmp	r0, r1
    1584:	e92d4010 	push	{r4, lr}
    1588:	08bd8010 	popeq	{r4, pc}
    158c:	93a0c000 	movls	ip, #0	; 0x0
    1590:	9a000003 	bls	15a4 <memmove+0x24>
    1594:	ea000006 	b	15b4 <memmove+0x34>
    1598:	e7d1300c 	ldrb	r3, [r1, ip]
    159c:	e7c0300c 	strb	r3, [r0, ip]
    15a0:	e28cc001 	add	ip, ip, #1	; 0x1
    15a4:	e3520000 	cmp	r2, #0	; 0x0
    15a8:	e2422001 	sub	r2, r2, #1	; 0x1
    15ac:	1afffff9 	bne	1598 <memmove+0x18>
    15b0:	e8bd8010 	pop	{r4, pc}
    15b4:	e0814002 	add	r4, r1, r2
    15b8:	e3a0c000 	mov	ip, #0	; 0x0
    15bc:	e0801002 	add	r1, r0, r2
    15c0:	ea000001 	b	15cc <memmove+0x4c>
    15c4:	e7d4300c 	ldrb	r3, [r4, ip]
    15c8:	e7c1300c 	strb	r3, [r1, ip]
    15cc:	e3520000 	cmp	r2, #0	; 0x0
    15d0:	e24cc001 	sub	ip, ip, #1	; 0x1
    15d4:	e2422001 	sub	r2, r2, #1	; 0x1
    15d8:	1afffff9 	bne	15c4 <memmove+0x44>
    15dc:	e8bd8010 	pop	{r4, pc}

000015e0 <memcmp>:
    15e0:	e92d4030 	push	{r4, r5, lr}
    15e4:	e3a0c000 	mov	ip, #0	; 0x0
    15e8:	e1a04001 	mov	r4, r1
    15ec:	e1a05000 	mov	r5, r0
    15f0:	e1a01002 	mov	r1, r2
    15f4:	ea000005 	b	1610 <memcmp+0x30>
    15f8:	e7d5200c 	ldrb	r2, [r5, ip]
    15fc:	e7d4300c 	ldrb	r3, [r4, ip]
    1600:	e28cc001 	add	ip, ip, #1	; 0x1
    1604:	e0520003 	subs	r0, r2, r3
    1608:	18bd8030 	popne	{r4, r5, pc}
    160c:	e2411001 	sub	r1, r1, #1	; 0x1
    1610:	e3510000 	cmp	r1, #0	; 0x0
    1614:	1afffff7 	bne	15f8 <memcmp+0x18>
    1618:	e1a00001 	mov	r0, r1
    161c:	e8bd8030 	pop	{r4, r5, pc}

00001620 <so_m_isused>:
    1620:	e5903004 	ldr	r3, [r0, #4]
    1624:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    1628:	03a00000 	moveq	r0, #0	; 0x0
    162c:	13a00001 	movne	r0, #1	; 0x1
    1630:	e12fff1e 	bx	lr

00001634 <so_m_set_used>:
    1634:	e5903004 	ldr	r3, [r0, #4]
    1638:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    163c:	e5803004 	str	r3, [r0, #4]
    1640:	e12fff1e 	bx	lr

00001644 <so_m_unset_used>:
    1644:	e5903004 	ldr	r3, [r0, #4]
    1648:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    164c:	e5803004 	str	r3, [r0, #4]
    1650:	e12fff1e 	bx	lr

00001654 <so_m_get_size>:
    1654:	e5900004 	ldr	r0, [r0, #4]
    1658:	e3c00103 	bic	r0, r0, #-1073741824	; 0xc0000000
    165c:	e12fff1e 	bx	lr

00001660 <so_m_set_size>:
    1660:	e5903004 	ldr	r3, [r0, #4]
    1664:	e3c11103 	bic	r1, r1, #-1073741824	; 0xc0000000
    1668:	e2033103 	and	r3, r3, #-1073741824	; 0xc0000000
    166c:	e1811003 	orr	r1, r1, r3
    1670:	e5801004 	str	r1, [r0, #4]
    1674:	e12fff1e 	bx	lr

00001678 <so_m_alloc>:
    1678:	e59f3048 	ldr	r3, [pc, #72]	; 16c8 <so_m_alloc+0x50>
    167c:	e3a02000 	mov	r2, #0	; 0x0
    1680:	e593c000 	ldr	ip, [r3]
    1684:	e5931008 	ldr	r1, [r3, #8]
    1688:	ea000007 	b	16ac <so_m_alloc+0x34>
    168c:	e5903004 	ldr	r3, [r0, #4]
    1690:	e3530000 	cmp	r3, #0	; 0x0
    1694:	1a000004 	bne	16ac <so_m_alloc+0x34>
    1698:	e59f2028 	ldr	r2, [pc, #40]	; 16c8 <so_m_alloc+0x50>
    169c:	e5923004 	ldr	r3, [r2, #4]
    16a0:	e2433001 	sub	r3, r3, #1	; 0x1
    16a4:	e5823004 	str	r3, [r2, #4]
    16a8:	e12fff1e 	bx	lr
    16ac:	e152000c 	cmp	r2, ip
    16b0:	e1a00001 	mov	r0, r1
    16b4:	e2822001 	add	r2, r2, #1	; 0x1
    16b8:	e2811010 	add	r1, r1, #16	; 0x10
    16bc:	3afffff2 	bcc	168c <so_m_alloc+0x14>
    16c0:	e3a00000 	mov	r0, #0	; 0x0
    16c4:	e12fff1e 	bx	lr
    16c8:	3c00d70c 	.word	0x3c00d70c

000016cc <so_m_remove>:
    16cc:	e3a03000 	mov	r3, #0	; 0x0
    16d0:	e59f2014 	ldr	r2, [pc, #20]	; 16ec <so_m_remove+0x20>
    16d4:	e5803000 	str	r3, [r0]
    16d8:	e5803004 	str	r3, [r0, #4]
    16dc:	e5923004 	ldr	r3, [r2, #4]
    16e0:	e2833001 	add	r3, r3, #1	; 0x1
    16e4:	e5823004 	str	r3, [r2, #4]
    16e8:	e12fff1e 	bx	lr
    16ec:	3c00d70c 	.word	0x3c00d70c

000016f0 <malloc>:
    16f0:	e3500000 	cmp	r0, #0	; 0x0
    16f4:	e92d4070 	push	{r4, r5, r6, lr}
    16f8:	0a000047 	beq	181c <malloc+0x12c>
    16fc:	e59f1120 	ldr	r1, [pc, #288]	; 1824 <malloc+0x134>
    1700:	e280301f 	add	r3, r0, #31	; 0x1f
    1704:	e5b12014 	ldr	r2, [r1, #20]!
    1708:	e3c3401f 	bic	r4, r3, #31	; 0x1f
    170c:	e2425008 	sub	r5, r2, #8	; 0x8
    1710:	ea000009 	b	173c <malloc+0x4c>
    1714:	e5953004 	ldr	r3, [r5, #4]
    1718:	e2130103 	ands	r0, r3, #-1073741824	; 0xc0000000
    171c:	e3c32103 	bic	r2, r3, #-1073741824	; 0xc0000000
    1720:	1a000003 	bne	1734 <malloc+0x44>
    1724:	e1520004 	cmp	r2, r4
    1728:	3a000001 	bcc	1734 <malloc+0x44>
    172c:	9a000030 	bls	17f4 <malloc+0x104>
    1730:	ea000005 	b	174c <malloc+0x5c>
    1734:	e5953008 	ldr	r3, [r5, #8]
    1738:	e2435008 	sub	r5, r3, #8	; 0x8
    173c:	e2853008 	add	r3, r5, #8	; 0x8
    1740:	e1530001 	cmp	r3, r1
    1744:	1afffff2 	bne	1714 <malloc+0x24>
    1748:	ea000033 	b	181c <malloc+0x12c>
    174c:	e59f30d0 	ldr	r3, [pc, #208]	; 1824 <malloc+0x134>
    1750:	e1a01000 	mov	r1, r0
    1754:	e5932008 	ldr	r2, [r3, #8]
    1758:	e5930000 	ldr	r0, [r3]
    175c:	ea00000a 	b	178c <malloc+0x9c>
    1760:	e5923004 	ldr	r3, [r2, #4]
    1764:	e2822010 	add	r2, r2, #16	; 0x10
    1768:	e3530000 	cmp	r3, #0	; 0x0
    176c:	1a000006 	bne	178c <malloc+0x9c>
    1770:	e59f20ac 	ldr	r2, [pc, #172]	; 1824 <malloc+0x134>
    1774:	e3560000 	cmp	r6, #0	; 0x0
    1778:	e5923004 	ldr	r3, [r2, #4]
    177c:	e2433001 	sub	r3, r3, #1	; 0x1
    1780:	e5823004 	str	r3, [r2, #4]
    1784:	1a000005 	bne	17a0 <malloc+0xb0>
    1788:	ea000023 	b	181c <malloc+0x12c>
    178c:	e1510000 	cmp	r1, r0
    1790:	e1a06002 	mov	r6, r2
    1794:	e2811001 	add	r1, r1, #1	; 0x1
    1798:	3afffff0 	bcc	1760 <malloc+0x70>
    179c:	ea00001e 	b	181c <malloc+0x12c>
    17a0:	e5953004 	ldr	r3, [r5, #4]
    17a4:	e5962004 	ldr	r2, [r6, #4]
    17a8:	e0643003 	rsb	r3, r4, r3
    17ac:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    17b0:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    17b4:	e1833002 	orr	r3, r3, r2
    17b8:	e5863004 	str	r3, [r6, #4]
    17bc:	e5951000 	ldr	r1, [r5]
    17c0:	e5950004 	ldr	r0, [r5, #4]
    17c4:	e1a0c005 	mov	ip, r5
    17c8:	e1a03006 	mov	r3, r6
    17cc:	e0841001 	add	r1, r4, r1
    17d0:	e3c42103 	bic	r2, r4, #-1073741824	; 0xc0000000
    17d4:	e2000103 	and	r0, r0, #-1073741824	; 0xc0000000
    17d8:	e5bc4008 	ldr	r4, [ip, #8]!
    17dc:	e4831008 	str	r1, [r3], #8
    17e0:	e1822000 	orr	r2, r2, r0
    17e4:	e5864008 	str	r4, [r6, #8]
    17e8:	e5843004 	str	r3, [r4, #4]
    17ec:	e985000c 	stmib	r5, {r2, r3}
    17f0:	e586c00c 	str	ip, [r6, #12]
    17f4:	e5953004 	ldr	r3, [r5, #4]
    17f8:	e59f1024 	ldr	r1, [pc, #36]	; 1824 <malloc+0x134>
    17fc:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    1800:	e5853004 	str	r3, [r5, #4]
    1804:	e5912010 	ldr	r2, [r1, #16]
    1808:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    180c:	e0833002 	add	r3, r3, r2
    1810:	e5813010 	str	r3, [r1, #16]
    1814:	e5950000 	ldr	r0, [r5]
    1818:	e8bd8070 	pop	{r4, r5, r6, pc}
    181c:	e3a00000 	mov	r0, #0	; 0x0
    1820:	e8bd8070 	pop	{r4, r5, r6, pc}
    1824:	3c00d70c 	.word	0x3c00d70c

00001828 <__sOMERGe>:
    1828:	e5902004 	ldr	r2, [r0, #4]
    182c:	e92d4030 	push	{r4, r5, lr}
    1830:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    1834:	e1a05000 	mov	r5, r0
    1838:	e1a0c001 	mov	ip, r1
    183c:	18bd8030 	popne	{r4, r5, pc}
    1840:	e5913004 	ldr	r3, [r1, #4]
    1844:	e2134103 	ands	r4, r3, #-1073741824	; 0xc0000000
    1848:	18bd8030 	popne	{r4, r5, pc}
    184c:	e0823003 	add	r3, r2, r3
    1850:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1854:	e59f0030 	ldr	r0, [pc, #48]	; 188c <__sOMERGe+0x64>
    1858:	e5853004 	str	r3, [r5, #4]
    185c:	e5814000 	str	r4, [r1]
    1860:	e5814004 	str	r4, [r1, #4]
    1864:	e5903004 	ldr	r3, [r0, #4]
    1868:	e591100c 	ldr	r1, [r1, #12]
    186c:	e59c2008 	ldr	r2, [ip, #8]
    1870:	e2833001 	add	r3, r3, #1	; 0x1
    1874:	e5812000 	str	r2, [r1]
    1878:	e5821004 	str	r1, [r2, #4]
    187c:	e5803004 	str	r3, [r0, #4]
    1880:	e58c4008 	str	r4, [ip, #8]
    1884:	e58c400c 	str	r4, [ip, #12]
    1888:	e8bd8030 	pop	{r4, r5, pc}
    188c:	3c00d70c 	.word	0x3c00d70c

00001890 <free>:
    1890:	e3500000 	cmp	r0, #0	; 0x0
    1894:	159f2090 	ldrne	r2, [pc, #144]	; 192c <free+0x9c>
    1898:	e92d4010 	push	{r4, lr}
    189c:	15b23014 	ldrne	r3, [r2, #20]!
    18a0:	1a000004 	bne	18b8 <free+0x28>
    18a4:	e8bd8010 	pop	{r4, pc}
    18a8:	e5943000 	ldr	r3, [r4]
    18ac:	e1500003 	cmp	r0, r3
    18b0:	0a000005 	beq	18cc <free+0x3c>
    18b4:	e5943008 	ldr	r3, [r4, #8]
    18b8:	e2434008 	sub	r4, r3, #8	; 0x8
    18bc:	e2843008 	add	r3, r4, #8	; 0x8
    18c0:	e1530002 	cmp	r3, r2
    18c4:	1afffff7 	bne	18a8 <free+0x18>
    18c8:	e8bd8010 	pop	{r4, pc}
    18cc:	e5943004 	ldr	r3, [r4, #4]
    18d0:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    18d4:	08bd8010 	popeq	{r4, pc}
    18d8:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    18dc:	e59f1048 	ldr	r1, [pc, #72]	; 192c <free+0x9c>
    18e0:	e5843004 	str	r3, [r4, #4]
    18e4:	e5912010 	ldr	r2, [r1, #16]
    18e8:	e5940008 	ldr	r0, [r4, #8]
    18ec:	e0632002 	rsb	r2, r3, r2
    18f0:	e59f3038 	ldr	r3, [pc, #56]	; 1930 <free+0xa0>
    18f4:	e5812010 	str	r2, [r1, #16]
    18f8:	e1500003 	cmp	r0, r3
    18fc:	0a000002 	beq	190c <free+0x7c>
    1900:	e2401008 	sub	r1, r0, #8	; 0x8
    1904:	e1a00004 	mov	r0, r4
    1908:	ebffffc6 	bl	1828 <__sOMERGe>
    190c:	e594000c 	ldr	r0, [r4, #12]
    1910:	e59f3018 	ldr	r3, [pc, #24]	; 1930 <free+0xa0>
    1914:	e1500003 	cmp	r0, r3
    1918:	08bd8010 	popeq	{r4, pc}
    191c:	e2400008 	sub	r0, r0, #8	; 0x8
    1920:	e1a01004 	mov	r1, r4
    1924:	e8bd4010 	pop	{r4, lr}
    1928:	eaffffbe 	b	1828 <__sOMERGe>
    192c:	3c00d70c 	.word	0x3c00d70c
    1930:	3c00d720 	.word	0x3c00d720

00001934 <__sOINFo>:
    1934:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1938:	e59f50a8 	ldr	r5, [pc, #168]	; 19e8 <__sOINFo+0xb4>
    193c:	e24dd004 	sub	sp, sp, #4	; 0x4
    1940:	e59f60a4 	ldr	r6, [pc, #164]	; 19ec <__sOINFo+0xb8>
    1944:	e8950006 	ldm	r5, {r1, r2}
    1948:	e59f00a0 	ldr	r0, [pc, #160]	; 19f0 <__sOINFo+0xbc>
    194c:	e1a0e00f 	mov	lr, pc
    1950:	e596f000 	ldr	pc, [r6]
    1954:	e2853008 	add	r3, r5, #8	; 0x8
    1958:	e8930098 	ldm	r3, {r3, r4, r7}
    195c:	e2444b01 	sub	r4, r4, #1024	; 0x400
    1960:	e0634004 	rsb	r4, r3, r4
    1964:	e3a00064 	mov	r0, #100	; 0x64
    1968:	e1a01004 	mov	r1, r4
    196c:	e0000790 	mul	r0, r0, r7
    1970:	eb004b87 	bl	14794 <__aeabi_uidiv>
    1974:	e1a01004 	mov	r1, r4
    1978:	e1a03000 	mov	r3, r0
    197c:	e1a02007 	mov	r2, r7
    1980:	e59f006c 	ldr	r0, [pc, #108]	; 19f4 <__sOINFo+0xc0>
    1984:	e1a0e00f 	mov	lr, pc
    1988:	e596f000 	ldr	pc, [r6]
    198c:	e59f0064 	ldr	r0, [pc, #100]	; 19f8 <__sOINFo+0xc4>
    1990:	e1a0e00f 	mov	lr, pc
    1994:	e596f000 	ldr	pc, [r6]
    1998:	e59f005c 	ldr	r0, [pc, #92]	; 19fc <__sOINFo+0xc8>
    199c:	e1a0e00f 	mov	lr, pc
    19a0:	e596f000 	ldr	pc, [r6]
    19a4:	e5b53014 	ldr	r3, [r5, #20]!
    19a8:	ea000007 	b	19cc <__sOINFo+0x98>
    19ac:	e8940006 	ldm	r4, {r1, r2}
    19b0:	e3120103 	tst	r2, #-1073741824	; 0xc0000000
    19b4:	03a03000 	moveq	r3, #0	; 0x0
    19b8:	13a03001 	movne	r3, #1	; 0x1
    19bc:	e3c22103 	bic	r2, r2, #-1073741824	; 0xc0000000
    19c0:	e1a0e00f 	mov	lr, pc
    19c4:	e596f000 	ldr	pc, [r6]
    19c8:	e5943008 	ldr	r3, [r4, #8]
    19cc:	e2434008 	sub	r4, r3, #8	; 0x8
    19d0:	e2843008 	add	r3, r4, #8	; 0x8
    19d4:	e1530005 	cmp	r3, r5
    19d8:	e59f0020 	ldr	r0, [pc, #32]	; 1a00 <__sOINFo+0xcc>
    19dc:	1afffff2 	bne	19ac <__sOINFo+0x78>
    19e0:	e28dd004 	add	sp, sp, #4	; 0x4
    19e4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    19e8:	3c00d70c 	.word	0x3c00d70c
    19ec:	3c00d6cc 	.word	0x3c00d6cc
    19f0:	0001628a 	.word	0x0001628a
    19f4:	000162a4 	.word	0x000162a4
    19f8:	000162c9 	.word	0x000162c9
    19fc:	000185ad 	.word	0x000185ad
    1a00:	000162da 	.word	0x000162da

00001a04 <init_malloc>:
    1a04:	e59f20e4 	ldr	r2, [pc, #228]	; 1af0 <init_malloc+0xec>
    1a08:	e59f30e4 	ldr	r3, [pc, #228]	; 1af4 <init_malloc+0xf0>
    1a0c:	e3c2001f 	bic	r0, r2, #31	; 0x1f
    1a10:	e3c3301f 	bic	r3, r3, #31	; 0x1f
    1a14:	e0602003 	rsb	r2, r0, r3
    1a18:	e92d4010 	push	{r4, lr}
    1a1c:	e59f40d4 	ldr	r4, [pc, #212]	; 1af8 <init_malloc+0xf4>
    1a20:	e3520b01 	cmp	r2, #1024	; 0x400
    1a24:	e5840008 	str	r0, [r4, #8]
    1a28:	e584300c 	str	r3, [r4, #12]
    1a2c:	93e00000 	mvnls	r0, #0	; 0x0
    1a30:	98bd8010 	popls	{r4, pc}
    1a34:	e284c014 	add	ip, r4, #20	; 0x14
    1a38:	e3a03040 	mov	r3, #64	; 0x40
    1a3c:	e3a01000 	mov	r1, #0	; 0x0
    1a40:	e3a02b01 	mov	r2, #1024	; 0x400
    1a44:	e584c018 	str	ip, [r4, #24]
    1a48:	e584c014 	str	ip, [r4, #20]
    1a4c:	e5843004 	str	r3, [r4, #4]
    1a50:	e5843000 	str	r3, [r4]
    1a54:	ebfffeb8 	bl	153c <memset>
    1a58:	e5940008 	ldr	r0, [r4, #8]
    1a5c:	e594c000 	ldr	ip, [r4]
    1a60:	e1a01000 	mov	r1, r0
    1a64:	e3a02000 	mov	r2, #0	; 0x0
    1a68:	ea000006 	b	1a88 <init_malloc+0x84>
    1a6c:	e59e3004 	ldr	r3, [lr, #4]
    1a70:	e3530000 	cmp	r3, #0	; 0x0
    1a74:	059f207c 	ldreq	r2, [pc, #124]	; 1af8 <init_malloc+0xf4>
    1a78:	05923004 	ldreq	r3, [r2, #4]
    1a7c:	02433001 	subeq	r3, r3, #1	; 0x1
    1a80:	05823004 	streq	r3, [r2, #4]
    1a84:	0a000005 	beq	1aa0 <init_malloc+0x9c>
    1a88:	e152000c 	cmp	r2, ip
    1a8c:	e1a0e001 	mov	lr, r1
    1a90:	e2822001 	add	r2, r2, #1	; 0x1
    1a94:	e2811010 	add	r1, r1, #16	; 0x10
    1a98:	3afffff3 	bcc	1a6c <init_malloc+0x68>
    1a9c:	e3a0e000 	mov	lr, #0	; 0x0
    1aa0:	e59fc050 	ldr	ip, [pc, #80]	; 1af8 <init_malloc+0xf4>
    1aa4:	e59e2004 	ldr	r2, [lr, #4]
    1aa8:	e59c300c 	ldr	r3, [ip, #12]
    1aac:	e1a0100e 	mov	r1, lr
    1ab0:	e2433b01 	sub	r3, r3, #1024	; 0x400
    1ab4:	e0603003 	rsb	r3, r0, r3
    1ab8:	e59c4018 	ldr	r4, [ip, #24]
    1abc:	e2022103 	and	r2, r2, #-1073741824	; 0xc0000000
    1ac0:	e2800b01 	add	r0, r0, #1024	; 0x400
    1ac4:	e3c33103 	bic	r3, r3, #-1073741824	; 0xc0000000
    1ac8:	e4810008 	str	r0, [r1], #8
    1acc:	e1833002 	orr	r3, r3, r2
    1ad0:	e28c2014 	add	r2, ip, #20	; 0x14
    1ad4:	e58e2008 	str	r2, [lr, #8]
    1ad8:	e58c1018 	str	r1, [ip, #24]
    1adc:	e5841000 	str	r1, [r4]
    1ae0:	e58e3004 	str	r3, [lr, #4]
    1ae4:	e58e400c 	str	r4, [lr, #12]
    1ae8:	e3a00000 	mov	r0, #0	; 0x0
    1aec:	e8bd8010 	pop	{r4, pc}
    1af0:	3c00e01f 	.word	0x3c00e01f
    1af4:	3c018000 	.word	0x3c018000
    1af8:	3c00d70c 	.word	0x3c00d70c

00001afc <isi_check_header>:
    1afc:	e92d4070 	push	{r4, r5, r6, lr}
    1b00:	e5902000 	ldr	r2, [r0]
    1b04:	e59f3088 	ldr	r3, [pc, #136]	; 1b94 <isi_check_header+0x98>
    1b08:	e1a05000 	mov	r5, r0
    1b0c:	e1520003 	cmp	r2, r3
    1b10:	e24dd008 	sub	sp, sp, #8	; 0x8
    1b14:	13e0001e 	mvnne	r0, #30	; 0x1e
    1b18:	1a00001b 	bne	1b8c <isi_check_header+0x90>
    1b1c:	e3a06000 	mov	r6, #0	; 0x0
    1b20:	e595401c 	ldr	r4, [r5, #28]
    1b24:	e3a01c02 	mov	r1, #512	; 0x200
    1b28:	e585601c 	str	r6, [r5, #28]
    1b2c:	e1a00006 	mov	r0, r6
    1b30:	eb0013ea 	bl	6ae0 <hash_init>
    1b34:	e3a01c02 	mov	r1, #512	; 0x200
    1b38:	e1a00005 	mov	r0, r5
    1b3c:	eb0013c2 	bl	6a4c <hash_data>
    1b40:	e28d0004 	add	r0, sp, #4	; 0x4
    1b44:	eb0013a4 	bl	69dc <hash_value>
    1b48:	e585401c 	str	r4, [r5, #28]
    1b4c:	e59d3004 	ldr	r3, [sp, #4]
    1b50:	e1540003 	cmp	r4, r3
    1b54:	13e0001f 	mvnne	r0, #31	; 0x1f
    1b58:	1a00000b 	bne	1b8c <isi_check_header+0x90>
    1b5c:	e5951004 	ldr	r1, [r5, #4]
    1b60:	e59f4030 	ldr	r4, [pc, #48]	; 1b98 <isi_check_header+0x9c>
    1b64:	e3c114ff 	bic	r1, r1, #-16777216	; 0xff000000
    1b68:	e59f002c 	ldr	r0, [pc, #44]	; 1b9c <isi_check_header+0xa0>
    1b6c:	e1a0e00f 	mov	lr, pc
    1b70:	e594f000 	ldr	pc, [r4]
    1b74:	e5d51007 	ldrb	r1, [r5, #7]
    1b78:	e59f0020 	ldr	r0, [pc, #32]	; 1ba0 <isi_check_header+0xa4>
    1b7c:	e201100f 	and	r1, r1, #15	; 0xf
    1b80:	e1a0e00f 	mov	lr, pc
    1b84:	e594f000 	ldr	pc, [r4]
    1b88:	e1a00006 	mov	r0, r6
    1b8c:	e28dd008 	add	sp, sp, #8	; 0x8
    1b90:	e8bd8070 	pop	{r4, r5, r6, pc}
    1b94:	6973692b 	.word	0x6973692b
    1b98:	3c00d6cc 	.word	0x3c00d6cc
    1b9c:	00016302 	.word	0x00016302
    1ba0:	00016310 	.word	0x00016310

00001ba4 <isi_check_data_separate>:
    1ba4:	e92d4070 	push	{r4, r5, r6, lr}
    1ba8:	e1a05000 	mov	r5, r0
    1bac:	e5d00007 	ldrb	r0, [r0, #7]
    1bb0:	e24dd028 	sub	sp, sp, #40	; 0x28
    1bb4:	e1a04001 	mov	r4, r1
    1bb8:	e200000f 	and	r0, r0, #15	; 0xf
    1bbc:	e595100c 	ldr	r1, [r5, #12]
    1bc0:	eb0013c6 	bl	6ae0 <hash_init>
    1bc4:	e28d6008 	add	r6, sp, #8	; 0x8
    1bc8:	e595100c 	ldr	r1, [r5, #12]
    1bcc:	e1a00004 	mov	r0, r4
    1bd0:	eb00139d 	bl	6a4c <hash_data>
    1bd4:	e1a00006 	mov	r0, r6
    1bd8:	eb00137f 	bl	69dc <hash_value>
    1bdc:	eb001377 	bl	69c0 <hash_len>
    1be0:	e2854020 	add	r4, r5, #32	; 0x20
    1be4:	e1a02000 	mov	r2, r0
    1be8:	e1a01004 	mov	r1, r4
    1bec:	e1a00006 	mov	r0, r6
    1bf0:	ebfffe7a 	bl	15e0 <memcmp>
    1bf4:	e3500000 	cmp	r0, #0	; 0x0
    1bf8:	0a000014 	beq	1c50 <isi_check_data_separate+0xac>
    1bfc:	e595100c 	ldr	r1, [r5, #12]
    1c00:	e59f3050 	ldr	r3, [pc, #80]	; 1c58 <isi_check_data_separate+0xb4>
    1c04:	e59f0050 	ldr	r0, [pc, #80]	; 1c5c <isi_check_data_separate+0xb8>
    1c08:	e1a0e00f 	mov	lr, pc
    1c0c:	e593f000 	ldr	pc, [r3]
    1c10:	eb00136a 	bl	69c0 <hash_len>
    1c14:	e1a01004 	mov	r1, r4
    1c18:	e1a03000 	mov	r3, r0
    1c1c:	e3a02001 	mov	r2, #1	; 0x1
    1c20:	e1a00004 	mov	r0, r4
    1c24:	e3a04020 	mov	r4, #32	; 0x20
    1c28:	e58d4000 	str	r4, [sp]
    1c2c:	eb003e3c 	bl	11524 <print_buffer>
    1c30:	eb001362 	bl	69c0 <hash_len>
    1c34:	e1a01006 	mov	r1, r6
    1c38:	e1a03000 	mov	r3, r0
    1c3c:	e3a02001 	mov	r2, #1	; 0x1
    1c40:	e1a00006 	mov	r0, r6
    1c44:	e58d4000 	str	r4, [sp]
    1c48:	eb003e35 	bl	11524 <print_buffer>
    1c4c:	e3e00020 	mvn	r0, #32	; 0x20
    1c50:	e28dd028 	add	sp, sp, #40	; 0x28
    1c54:	e8bd8070 	pop	{r4, r5, r6, pc}
    1c58:	3c00d6cc 	.word	0x3c00d6cc
    1c5c:	00016323 	.word	0x00016323

00001c60 <isi_check_data>:
    1c60:	e2801c02 	add	r1, r0, #512	; 0x200
    1c64:	eaffffce 	b	1ba4 <isi_check_data_separate>

00001c68 <isi_check>:
    1c68:	e92d4010 	push	{r4, lr}
    1c6c:	e1a04000 	mov	r4, r0
    1c70:	ebffffa1 	bl	1afc <isi_check_header>
    1c74:	e3500000 	cmp	r0, #0	; 0x0
    1c78:	18bd8010 	popne	{r4, pc}
    1c7c:	e1a00004 	mov	r0, r4
    1c80:	e8bd4010 	pop	{r4, lr}
    1c84:	eafffff5 	b	1c60 <isi_check_data>

00001c88 <load_serial_ymodem>:
    1c88:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    1c8c:	e24dde41 	sub	sp, sp, #1040	; 0x410
    1c90:	e28d4b01 	add	r4, sp, #1024	; 0x400
    1c94:	e284400c 	add	r4, r4, #12	; 0xc
    1c98:	e3a03002 	mov	r3, #2	; 0x2
    1c9c:	e1a07000 	mov	r7, r0
    1ca0:	e1a01004 	mov	r1, r4
    1ca4:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1ca8:	e58d3404 	str	r3, [sp, #1028]
    1cac:	eb004a3e 	bl	145ac <xyzModem_stream_open>
    1cb0:	e3500000 	cmp	r0, #0	; 0x0
    1cb4:	1a000012 	bne	1d04 <load_serial_ymodem+0x7c>
    1cb8:	e1a08004 	mov	r8, r4
    1cbc:	e1a05000 	mov	r5, r0
    1cc0:	e1a04000 	mov	r4, r0
    1cc4:	e1a0600d 	mov	r6, sp
    1cc8:	ea000001 	b	1cd4 <load_serial_ymodem+0x4c>
    1ccc:	e085500c 	add	r5, r5, ip
    1cd0:	ebfffe21 	bl	155c <memcpy>
    1cd4:	e3a01b01 	mov	r1, #1024	; 0x400
    1cd8:	e1a02008 	mov	r2, r8
    1cdc:	e1a0000d 	mov	r0, sp
    1ce0:	eb00499d 	bl	1435c <xyzModem_stream_read>
    1ce4:	e0843007 	add	r3, r4, r7
    1ce8:	e250c000 	subs	ip, r0, #0	; 0x0
    1cec:	e1a0100d 	mov	r1, sp
    1cf0:	e1a0200c 	mov	r2, ip
    1cf4:	e1a00003 	mov	r0, r3
    1cf8:	e084400c 	add	r4, r4, ip
    1cfc:	cafffff2 	bgt	1ccc <load_serial_ymodem+0x44>
    1d00:	ea000007 	b	1d24 <load_serial_ymodem+0x9c>
    1d04:	e59f3054 	ldr	r3, [pc, #84]	; 1d60 <load_serial_ymodem+0xd8>
    1d08:	e59d040c 	ldr	r0, [sp, #1036]
    1d0c:	e5934000 	ldr	r4, [r3]
    1d10:	eb004618 	bl	13578 <xyzModem_error>
    1d14:	e1a01000 	mov	r1, r0
    1d18:	e59f0044 	ldr	r0, [pc, #68]	; 1d64 <load_serial_ymodem+0xdc>
    1d1c:	e12fff34 	blx	r4
    1d20:	e3a05000 	mov	r5, #0	; 0x0
    1d24:	e28d0b01 	add	r0, sp, #1024	; 0x400
    1d28:	e280000c 	add	r0, r0, #12	; 0xc
    1d2c:	eb0045f9 	bl	13518 <xyzModem_stream_close>
    1d30:	e3a00000 	mov	r0, #0	; 0x0
    1d34:	e59f102c 	ldr	r1, [pc, #44]	; 1d68 <load_serial_ymodem+0xe0>
    1d38:	eb0048a7 	bl	13fdc <xyzModem_stream_terminate>
    1d3c:	e1a01005 	mov	r1, r5
    1d40:	e59f0024 	ldr	r0, [pc, #36]	; 1d6c <load_serial_ymodem+0xe4>
    1d44:	e1a02005 	mov	r2, r5
    1d48:	e59f3010 	ldr	r3, [pc, #16]	; 1d60 <load_serial_ymodem+0xd8>
    1d4c:	e1a0e00f 	mov	lr, pc
    1d50:	e593f000 	ldr	pc, [r3]
    1d54:	e1a00007 	mov	r0, r7
    1d58:	e28dde41 	add	sp, sp, #1040	; 0x410
    1d5c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    1d60:	3c00d6cc 	.word	0x3c00d6cc
    1d64:	0001641f 	.word	0x0001641f
    1d68:	00001d70 	.word	0x00001d70
    1d6c:	00016347 	.word	0x00016347

00001d70 <getcxmodem>:
    1d70:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    1d74:	e24dd004 	sub	sp, sp, #4	; 0x4
    1d78:	eb004298 	bl	127e0 <tstc>
    1d7c:	e3500000 	cmp	r0, #0	; 0x0
    1d80:	1a000002 	bne	1d90 <getcxmodem+0x20>
    1d84:	e3e00000 	mvn	r0, #0	; 0x0
    1d88:	e28dd004 	add	sp, sp, #4	; 0x4
    1d8c:	e8bd8000 	pop	{pc}
    1d90:	e28dd004 	add	sp, sp, #4	; 0x4
    1d94:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    1d98:	ea004291 	b	127e4 <getc>

00001d9c <do_load_serial_bin>:
    1d9c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1da0:	e3520001 	cmp	r2, #1	; 0x1
    1da4:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1da8:	e1a06002 	mov	r6, r2
    1dac:	e1a07000 	mov	r7, r0
    1db0:	e1a05003 	mov	r5, r3
    1db4:	ca000006 	bgt	1dd4 <do_load_serial_bin+0x38>
    1db8:	e59f0120 	ldr	r0, [pc, #288]	; 1ee0 <do_load_serial_bin+0x144>
    1dbc:	e3a06000 	mov	r6, #0	; 0x0
    1dc0:	e59f311c 	ldr	r3, [pc, #284]	; 1ee4 <do_load_serial_bin+0x148>
    1dc4:	e1a0e00f 	mov	lr, pc
    1dc8:	e593f000 	ldr	pc, [r3]
    1dcc:	e1a04006 	mov	r4, r6
    1dd0:	ea00000c 	b	1e08 <do_load_serial_bin+0x6c>
    1dd4:	e5930004 	ldr	r0, [r3, #4]
    1dd8:	e3a01000 	mov	r1, #0	; 0x0
    1ddc:	e3a02010 	mov	r2, #16	; 0x10
    1de0:	eb003e67 	bl	11784 <simple_strtoul>
    1de4:	e3560003 	cmp	r6, #3	; 0x3
    1de8:	e1a04000 	mov	r4, r0
    1dec:	13a06000 	movne	r6, #0	; 0x0
    1df0:	1a000004 	bne	1e08 <do_load_serial_bin+0x6c>
    1df4:	e5950008 	ldr	r0, [r5, #8]
    1df8:	e3a01000 	mov	r1, #0	; 0x0
    1dfc:	e3a02010 	mov	r2, #16	; 0x10
    1e00:	eb003e5f 	bl	11784 <simple_strtoul>
    1e04:	e1a06000 	mov	r6, r0
    1e08:	e5950000 	ldr	r0, [r5]
    1e0c:	e59f10d4 	ldr	r1, [pc, #212]	; 1ee8 <do_load_serial_bin+0x14c>
    1e10:	ebfffd8a 	bl	1440 <strcmp>
    1e14:	e3500000 	cmp	r0, #0	; 0x0
    1e18:	1a000007 	bne	1e3c <do_load_serial_bin+0xa0>
    1e1c:	e1a01004 	mov	r1, r4
    1e20:	e59f00c4 	ldr	r0, [pc, #196]	; 1eec <do_load_serial_bin+0x150>
    1e24:	e59f30b8 	ldr	r3, [pc, #184]	; 1ee4 <do_load_serial_bin+0x148>
    1e28:	e1a0e00f 	mov	lr, pc
    1e2c:	e593f000 	ldr	pc, [r3]
    1e30:	e1a00004 	mov	r0, r4
    1e34:	ebffff93 	bl	1c88 <load_serial_ymodem>
    1e38:	ea000025 	b	1ed4 <do_load_serial_bin+0x138>
    1e3c:	e5950000 	ldr	r0, [r5]
    1e40:	e59f10a8 	ldr	r1, [pc, #168]	; 1ef0 <do_load_serial_bin+0x154>
    1e44:	ebfffd7d 	bl	1440 <strcmp>
    1e48:	e3500000 	cmp	r0, #0	; 0x0
    1e4c:	e59f5090 	ldr	r5, [pc, #144]	; 1ee4 <do_load_serial_bin+0x148>
    1e50:	1a00001b 	bne	1ec4 <do_load_serial_bin+0x128>
    1e54:	e1a02006 	mov	r2, r6
    1e58:	e59f0094 	ldr	r0, [pc, #148]	; 1ef4 <do_load_serial_bin+0x158>
    1e5c:	e1a01004 	mov	r1, r4
    1e60:	e1a0e00f 	mov	lr, pc
    1e64:	e595f000 	ldr	pc, [r5]
    1e68:	e1a00004 	mov	r0, r4
    1e6c:	e1a01006 	mov	r1, r6
    1e70:	eb0047bc 	bl	13d68 <control_send>
    1e74:	e3500000 	cmp	r0, #0	; 0x0
    1e78:	aa000005 	bge	1e94 <do_load_serial_bin+0xf8>
    1e7c:	e59d0004 	ldr	r0, [sp, #4]
    1e80:	e5954000 	ldr	r4, [r5]
    1e84:	eb0045bb 	bl	13578 <xyzModem_error>
    1e88:	e1a01000 	mov	r1, r0
    1e8c:	e59f0064 	ldr	r0, [pc, #100]	; 1ef8 <do_load_serial_bin+0x15c>
    1e90:	e12fff34 	blx	r4
    1e94:	e28d0004 	add	r0, sp, #4	; 0x4
    1e98:	eb00459e 	bl	13518 <xyzModem_stream_close>
    1e9c:	e3a00000 	mov	r0, #0	; 0x0
    1ea0:	e59f1054 	ldr	r1, [pc, #84]	; 1efc <do_load_serial_bin+0x160>
    1ea4:	eb00484c 	bl	13fdc <xyzModem_stream_terminate>
    1ea8:	e1a01006 	mov	r1, r6
    1eac:	e59f004c 	ldr	r0, [pc, #76]	; 1f00 <do_load_serial_bin+0x164>
    1eb0:	e1a02006 	mov	r2, r6
    1eb4:	e59f3028 	ldr	r3, [pc, #40]	; 1ee4 <do_load_serial_bin+0x148>
    1eb8:	e1a0e00f 	mov	lr, pc
    1ebc:	e593f000 	ldr	pc, [r3]
    1ec0:	ea000003 	b	1ed4 <do_load_serial_bin+0x138>
    1ec4:	e5971014 	ldr	r1, [r7, #20]
    1ec8:	e59f0034 	ldr	r0, [pc, #52]	; 1f04 <do_load_serial_bin+0x168>
    1ecc:	e1a0e00f 	mov	lr, pc
    1ed0:	e595f000 	ldr	pc, [r5]
    1ed4:	e3a00000 	mov	r0, #0	; 0x0
    1ed8:	e28dd00c 	add	sp, sp, #12	; 0xc
    1edc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    1ee0:	0001636f 	.word	0x0001636f
    1ee4:	3c00d6cc 	.word	0x3c00d6cc
    1ee8:	00016389 	.word	0x00016389
    1eec:	0001638c 	.word	0x0001638c
    1ef0:	000163cf 	.word	0x000163cf
    1ef4:	000163d2 	.word	0x000163d2
    1ef8:	0001641f 	.word	0x0001641f
    1efc:	00001d70 	.word	0x00001d70
    1f00:	00016347 	.word	0x00016347
    1f04:	0001641e 	.word	0x0001641e

00001f08 <do_mem_cp>:
    1f08:	e3520004 	cmp	r2, #4	; 0x4
    1f0c:	e92d4070 	push	{r4, r5, r6, lr}
    1f10:	e1a04003 	mov	r4, r3
    1f14:	0a000002 	beq	1f24 <do_mem_cp+0x1c>
    1f18:	eb0003e2 	bl	2ea8 <cmd_usage>
    1f1c:	e3a00001 	mov	r0, #1	; 0x1
    1f20:	e8bd8070 	pop	{r4, r5, r6, pc}
    1f24:	e5930004 	ldr	r0, [r3, #4]
    1f28:	e3a01000 	mov	r1, #0	; 0x0
    1f2c:	e3a02010 	mov	r2, #16	; 0x10
    1f30:	eb003e13 	bl	11784 <simple_strtoul>
    1f34:	e3a01000 	mov	r1, #0	; 0x0
    1f38:	e3a02010 	mov	r2, #16	; 0x10
    1f3c:	e1a06000 	mov	r6, r0
    1f40:	e5940008 	ldr	r0, [r4, #8]
    1f44:	eb003e0e 	bl	11784 <simple_strtoul>
    1f48:	e3a01000 	mov	r1, #0	; 0x0
    1f4c:	e1a05000 	mov	r5, r0
    1f50:	e3a02010 	mov	r2, #16	; 0x10
    1f54:	e594000c 	ldr	r0, [r4, #12]
    1f58:	eb003e09 	bl	11784 <simple_strtoul>
    1f5c:	e2800003 	add	r0, r0, #3	; 0x3
    1f60:	e1b00120 	lsrs	r0, r0, #2
    1f64:	1a000002 	bne	1f74 <do_mem_cp+0x6c>
    1f68:	ea000006 	b	1f88 <do_mem_cp+0x80>
    1f6c:	e5163004 	ldr	r3, [r6, #-4]
    1f70:	e5053004 	str	r3, [r5, #-4]
    1f74:	e3500000 	cmp	r0, #0	; 0x0
    1f78:	e2866004 	add	r6, r6, #4	; 0x4
    1f7c:	e2400001 	sub	r0, r0, #1	; 0x1
    1f80:	e2855004 	add	r5, r5, #4	; 0x4
    1f84:	1afffff8 	bne	1f6c <do_mem_cp+0x64>
    1f88:	e3a00000 	mov	r0, #0	; 0x0
    1f8c:	e8bd8070 	pop	{r4, r5, r6, pc}

00001f90 <do_mem_cmp>:
    1f90:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    1f94:	e3520004 	cmp	r2, #4	; 0x4
    1f98:	e24dd00c 	sub	sp, sp, #12	; 0xc
    1f9c:	e1a04003 	mov	r4, r3
    1fa0:	0a000002 	beq	1fb0 <do_mem_cmp+0x20>
    1fa4:	eb0003bf 	bl	2ea8 <cmd_usage>
    1fa8:	e3a04001 	mov	r4, #1	; 0x1
    1fac:	ea000029 	b	2058 <do_mem_cmp+0xc8>
    1fb0:	e5930004 	ldr	r0, [r3, #4]
    1fb4:	e3a01000 	mov	r1, #0	; 0x0
    1fb8:	e3a02010 	mov	r2, #16	; 0x10
    1fbc:	eb003df0 	bl	11784 <simple_strtoul>
    1fc0:	e3a01000 	mov	r1, #0	; 0x0
    1fc4:	e3a02010 	mov	r2, #16	; 0x10
    1fc8:	e1a07000 	mov	r7, r0
    1fcc:	e5940008 	ldr	r0, [r4, #8]
    1fd0:	eb003deb 	bl	11784 <simple_strtoul>
    1fd4:	e3a01000 	mov	r1, #0	; 0x0
    1fd8:	e1a06000 	mov	r6, r0
    1fdc:	e3a02010 	mov	r2, #16	; 0x10
    1fe0:	e594000c 	ldr	r0, [r4, #12]
    1fe4:	eb003de6 	bl	11784 <simple_strtoul>
    1fe8:	e2800003 	add	r0, r0, #3	; 0x3
    1fec:	e1a00120 	lsr	r0, r0, #2
    1ff0:	e3a05000 	mov	r5, #0	; 0x0
    1ff4:	ea00000f 	b	2038 <do_mem_cmp+0xa8>
    1ff8:	e5972000 	ldr	r2, [r7]
    1ffc:	e5963000 	ldr	r3, [r6]
    2000:	e1520003 	cmp	r2, r3
    2004:	0a000008 	beq	202c <do_mem_cmp+0x9c>
    2008:	e58d3000 	str	r3, [sp]
    200c:	e1a01007 	mov	r1, r7
    2010:	e1a03006 	mov	r3, r6
    2014:	e59f0048 	ldr	r0, [pc, #72]	; 2064 <do_mem_cmp+0xd4>
    2018:	e59fc048 	ldr	ip, [pc, #72]	; 2068 <do_mem_cmp+0xd8>
    201c:	e1a0e00f 	mov	lr, pc
    2020:	e59cf000 	ldr	pc, [ip]
    2024:	e3a04001 	mov	r4, #1	; 0x1
    2028:	ea000005 	b	2044 <do_mem_cmp+0xb4>
    202c:	e2855001 	add	r5, r5, #1	; 0x1
    2030:	e2877004 	add	r7, r7, #4	; 0x4
    2034:	e2866004 	add	r6, r6, #4	; 0x4
    2038:	e1500005 	cmp	r0, r5
    203c:	1affffed 	bne	1ff8 <do_mem_cmp+0x68>
    2040:	e3a04000 	mov	r4, #0	; 0x0
    2044:	e1a01005 	mov	r1, r5
    2048:	e59f001c 	ldr	r0, [pc, #28]	; 206c <do_mem_cmp+0xdc>
    204c:	e59f3014 	ldr	r3, [pc, #20]	; 2068 <do_mem_cmp+0xd8>
    2050:	e1a0e00f 	mov	lr, pc
    2054:	e593f000 	ldr	pc, [r3]
    2058:	e1a00004 	mov	r0, r4
    205c:	e28dd00c 	add	sp, sp, #12	; 0xc
    2060:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    2064:	00016524 	.word	0x00016524
    2068:	3c00d6cc 	.word	0x3c00d6cc
    206c:	0001655c 	.word	0x0001655c

00002070 <do_mem_mw>:
    2070:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    2074:	e1a06002 	mov	r6, r2
    2078:	e2422003 	sub	r2, r2, #3	; 0x3
    207c:	e3520001 	cmp	r2, #1	; 0x1
    2080:	e24dd004 	sub	sp, sp, #4	; 0x4
    2084:	e1a04003 	mov	r4, r3
    2088:	9a000002 	bls	2098 <do_mem_mw+0x28>
    208c:	eb000385 	bl	2ea8 <cmd_usage>
    2090:	e3a00001 	mov	r0, #1	; 0x1
    2094:	ea000016 	b	20f4 <do_mem_mw+0x84>
    2098:	e3a01000 	mov	r1, #0	; 0x0
    209c:	e3a02010 	mov	r2, #16	; 0x10
    20a0:	e5930004 	ldr	r0, [r3, #4]
    20a4:	eb003db6 	bl	11784 <simple_strtoul>
    20a8:	e3a01000 	mov	r1, #0	; 0x0
    20ac:	e1a05000 	mov	r5, r0
    20b0:	e3a02010 	mov	r2, #16	; 0x10
    20b4:	e5940008 	ldr	r0, [r4, #8]
    20b8:	eb003db1 	bl	11784 <simple_strtoul>
    20bc:	e3560004 	cmp	r6, #4	; 0x4
    20c0:	e1a07000 	mov	r7, r0
    20c4:	13a00001 	movne	r0, #1	; 0x1
    20c8:	1a000006 	bne	20e8 <do_mem_mw+0x78>
    20cc:	e594000c 	ldr	r0, [r4, #12]
    20d0:	e3a01000 	mov	r1, #0	; 0x0
    20d4:	e3a02010 	mov	r2, #16	; 0x10
    20d8:	eb003da9 	bl	11784 <simple_strtoul>
    20dc:	ea000001 	b	20e8 <do_mem_mw+0x78>
    20e0:	e5057004 	str	r7, [r5, #-4]
    20e4:	e2400001 	sub	r0, r0, #1	; 0x1
    20e8:	e3500000 	cmp	r0, #0	; 0x0
    20ec:	e2855004 	add	r5, r5, #4	; 0x4
    20f0:	1afffffa 	bne	20e0 <do_mem_mw+0x70>
    20f4:	e28dd004 	add	sp, sp, #4	; 0x4
    20f8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

000020fc <do_mem_md>:
    20fc:	e92d4070 	push	{r4, r5, r6, lr}
    2100:	e3520001 	cmp	r2, #1	; 0x1
    2104:	e24dd008 	sub	sp, sp, #8	; 0x8
    2108:	e1a04002 	mov	r4, r2
    210c:	e1a05003 	mov	r5, r3
    2110:	ca000002 	bgt	2120 <do_mem_md+0x24>
    2114:	eb000363 	bl	2ea8 <cmd_usage>
    2118:	e3a00001 	mov	r0, #1	; 0x1
    211c:	ea000014 	b	2174 <do_mem_md+0x78>
    2120:	e5930004 	ldr	r0, [r3, #4]
    2124:	e3a01000 	mov	r1, #0	; 0x0
    2128:	e3a02010 	mov	r2, #16	; 0x10
    212c:	eb003d94 	bl	11784 <simple_strtoul>
    2130:	e3540002 	cmp	r4, #2	; 0x2
    2134:	e1a06000 	mov	r6, r0
    2138:	03a030a0 	moveq	r3, #160	; 0xa0
    213c:	0a000004 	beq	2154 <do_mem_md+0x58>
    2140:	e5950008 	ldr	r0, [r5, #8]
    2144:	e3a01000 	mov	r1, #0	; 0x0
    2148:	e3a02010 	mov	r2, #16	; 0x10
    214c:	eb003d8c 	bl	11784 <simple_strtoul>
    2150:	e1a03000 	mov	r3, r0
    2154:	e3a0c004 	mov	ip, #4	; 0x4
    2158:	e1a00006 	mov	r0, r6
    215c:	e1a0200c 	mov	r2, ip
    2160:	e1a03123 	lsr	r3, r3, #2
    2164:	e1a01006 	mov	r1, r6
    2168:	e58dc000 	str	ip, [sp]
    216c:	eb003cec 	bl	11524 <print_buffer>
    2170:	e3a00000 	mov	r0, #0	; 0x0
    2174:	e28dd008 	add	sp, sp, #8	; 0x8
    2178:	e8bd8070 	pop	{r4, r5, r6, pc}

0000217c <do_nand>:
    217c:	e3520001 	cmp	r2, #1	; 0x1
    2180:	e92d4070 	push	{r4, r5, r6, lr}
    2184:	e1a06002 	mov	r6, r2
    2188:	e1a05003 	mov	r5, r3
    218c:	da000020 	ble	2214 <do_nand+0x98>
    2190:	e5930004 	ldr	r0, [r3, #4]
    2194:	e59f10a0 	ldr	r1, [pc, #160]	; 223c <do_nand+0xc0>
    2198:	e3a02007 	mov	r2, #7	; 0x7
    219c:	ebfffcb5 	bl	1478 <strncmp>
    21a0:	e2504000 	subs	r4, r0, #0	; 0x0
    21a4:	1a000001 	bne	21b0 <do_nand+0x34>
    21a8:	eb0026f6 	bl	bd88 <ecc_print_stat>
    21ac:	ea000009 	b	21d8 <do_nand+0x5c>
    21b0:	e5950004 	ldr	r0, [r5, #4]
    21b4:	e59f1084 	ldr	r1, [pc, #132]	; 2240 <do_nand+0xc4>
    21b8:	e3a02003 	mov	r2, #3	; 0x3
    21bc:	ebfffcad 	bl	1478 <strncmp>
    21c0:	e2504000 	subs	r4, r0, #0	; 0x0
    21c4:	1a000005 	bne	21e0 <do_nand+0x64>
    21c8:	e3560002 	cmp	r6, #2	; 0x2
    21cc:	d3a00000 	movle	r0, #0	; 0x0
    21d0:	c3a00001 	movgt	r0, #1	; 0x1
    21d4:	eb0022e5 	bl	ad70 <base_scan_badblocks>
    21d8:	e1a00004 	mov	r0, r4
    21dc:	e8bd8070 	pop	{r4, r5, r6, pc}
    21e0:	e5950004 	ldr	r0, [r5, #4]
    21e4:	e59f1058 	ldr	r1, [pc, #88]	; 2244 <do_nand+0xc8>
    21e8:	e3a02007 	mov	r2, #7	; 0x7
    21ec:	ebfffca1 	bl	1478 <strncmp>
    21f0:	e2504000 	subs	r4, r0, #0	; 0x0
    21f4:	13a00000 	movne	r0, #0	; 0x0
    21f8:	18bd8070 	popne	{r4, r5, r6, pc}
    21fc:	e3560002 	cmp	r6, #2	; 0x2
    2200:	1a000005 	bne	221c <do_nand+0xa0>
    2204:	e59f003c 	ldr	r0, [pc, #60]	; 2248 <do_nand+0xcc>
    2208:	e59f303c 	ldr	r3, [pc, #60]	; 224c <do_nand+0xd0>
    220c:	e1a0e00f 	mov	lr, pc
    2210:	e593f000 	ldr	pc, [r3]
    2214:	e3e00000 	mvn	r0, #0	; 0x0
    2218:	e8bd8070 	pop	{r4, r5, r6, pc}
    221c:	e3a02010 	mov	r2, #16	; 0x10
    2220:	e1a01004 	mov	r1, r4
    2224:	e5950008 	ldr	r0, [r5, #8]
    2228:	eb003d55 	bl	11784 <simple_strtoul>
    222c:	e1a01004 	mov	r1, r4
    2230:	eb0023aa 	bl	b0e0 <base_block_markbad>
    2234:	e1a00004 	mov	r0, r4
    2238:	e8bd8070 	pop	{r4, r5, r6, pc}
    223c:	00016626 	.word	0x00016626
    2240:	00016632 	.word	0x00016632
    2244:	0001662e 	.word	0x0001662e
    2248:	00016636 	.word	0x00016636
    224c:	3c00d6cc 	.word	0x3c00d6cc

00002250 <do_sc>:
    2250:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    2254:	e3520002 	cmp	r2, #2	; 0x2
    2258:	e24ddf47 	sub	sp, sp, #284	; 0x11c
    225c:	0a000002 	beq	226c <do_sc+0x1c>
    2260:	eb000310 	bl	2ea8 <cmd_usage>
    2264:	e3a00001 	mov	r0, #1	; 0x1
    2268:	ea000060 	b	23f0 <do_sc+0x1a0>
    226c:	e5930004 	ldr	r0, [r3, #4]
    2270:	e3a01000 	mov	r1, #0	; 0x0
    2274:	e3a02010 	mov	r2, #16	; 0x10
    2278:	eb003d41 	bl	11784 <simple_strtoul>
    227c:	e28d4f41 	add	r4, sp, #260	; 0x104
    2280:	e2844002 	add	r4, r4, #2	; 0x2
    2284:	e59f116c 	ldr	r1, [pc, #364]	; 23f8 <do_sc+0x1a8>
    2288:	e1a06000 	mov	r6, r0
    228c:	e3a02012 	mov	r2, #18	; 0x12
    2290:	e1a00004 	mov	r0, r4
    2294:	ebfffcb0 	bl	155c <memcpy>
    2298:	e3a01000 	mov	r1, #0	; 0x0
    229c:	ea000004 	b	22b4 <do_sc+0x64>
    22a0:	e7d12006 	ldrb	r2, [r1, r6]
    22a4:	e7d43001 	ldrb	r3, [r4, r1]
    22a8:	e2811001 	add	r1, r1, #1	; 0x1
    22ac:	e1520003 	cmp	r2, r3
    22b0:	1a000014 	bne	2308 <do_sc+0xb8>
    22b4:	e3510012 	cmp	r1, #18	; 0x12
    22b8:	e0815006 	add	r5, r1, r6
    22bc:	1afffff7 	bne	22a0 <do_sc+0x50>
    22c0:	e2851eff 	add	r1, r5, #4080	; 0xff0
    22c4:	e281100f 	add	r1, r1, #15	; 0xf
    22c8:	e1a02005 	mov	r2, r5
    22cc:	e5d23000 	ldrb	r3, [r2]
    22d0:	e353000a 	cmp	r3, #10	; 0xa
    22d4:	1a000005 	bne	22f0 <do_sc+0xa0>
    22d8:	e5d23001 	ldrb	r3, [r2, #1]
    22dc:	e353005d 	cmp	r3, #93	; 0x5d
    22e0:	1a000002 	bne	22f0 <do_sc+0xa0>
    22e4:	e5d23002 	ldrb	r3, [r2, #2]
    22e8:	e353005d 	cmp	r3, #93	; 0x5d
    22ec:	0a000003 	beq	2300 <do_sc+0xb0>
    22f0:	e1520001 	cmp	r2, r1
    22f4:	e2822001 	add	r2, r2, #1	; 0x1
    22f8:	1afffff3 	bne	22cc <do_sc+0x7c>
    22fc:	ea000001 	b	2308 <do_sc+0xb8>
    2300:	e3550000 	cmp	r5, #0	; 0x0
    2304:	1a000006 	bne	2324 <do_sc+0xd4>
    2308:	e59f00ec 	ldr	r0, [pc, #236]	; 23fc <do_sc+0x1ac>
    230c:	e1a01006 	mov	r1, r6
    2310:	e59f30e8 	ldr	r3, [pc, #232]	; 2400 <do_sc+0x1b0>
    2314:	e1a0e00f 	mov	lr, pc
    2318:	e593f000 	ldr	pc, [r3]
    231c:	e3e00000 	mvn	r0, #0	; 0x0
    2320:	ea000032 	b	23f0 <do_sc+0x1a0>
    2324:	e59f40d4 	ldr	r4, [pc, #212]	; 2400 <do_sc+0x1b0>
    2328:	e1a01006 	mov	r1, r6
    232c:	e59f00d0 	ldr	r0, [pc, #208]	; 2404 <do_sc+0x1b4>
    2330:	e28d6008 	add	r6, sp, #8	; 0x8
    2334:	e1a0e00f 	mov	lr, pc
    2338:	e594f000 	ldr	pc, [r4]
    233c:	e2466002 	sub	r6, r6, #2	; 0x2
    2340:	e1a07004 	mov	r7, r4
    2344:	ea000000 	b	234c <do_sc+0xfc>
    2348:	e2855001 	add	r5, r5, #1	; 0x1
    234c:	e5d53000 	ldrb	r3, [r5]
    2350:	e353002d 	cmp	r3, #45	; 0x2d
    2354:	1353000a 	cmpne	r3, #10	; 0xa
    2358:	0afffffa 	beq	2348 <do_sc+0xf8>
    235c:	e353005b 	cmp	r3, #91	; 0x5b
    2360:	0afffff8 	beq	2348 <do_sc+0xf8>
    2364:	e353005d 	cmp	r3, #93	; 0x5d
    2368:	0a000001 	beq	2374 <do_sc+0x124>
    236c:	e1a02006 	mov	r2, r6
    2370:	ea000005 	b	238c <do_sc+0x13c>
    2374:	e5d53001 	ldrb	r3, [r5, #1]
    2378:	e353005d 	cmp	r3, #93	; 0x5d
    237c:	1afffffa 	bne	236c <do_sc+0x11c>
    2380:	ea00000c 	b	23b8 <do_sc+0x168>
    2384:	e5423001 	strb	r3, [r2, #-1]
    2388:	e2855001 	add	r5, r5, #1	; 0x1
    238c:	e5d53000 	ldrb	r3, [r5]
    2390:	e1a01002 	mov	r1, r2
    2394:	e353000a 	cmp	r3, #10	; 0xa
    2398:	1353005d 	cmpne	r3, #93	; 0x5d
    239c:	03a04000 	moveq	r4, #0	; 0x0
    23a0:	13a04001 	movne	r4, #1	; 0x1
    23a4:	e2822001 	add	r2, r2, #1	; 0x1
    23a8:	1afffff5 	bne	2384 <do_sc+0x134>
    23ac:	e3550000 	cmp	r5, #0	; 0x0
    23b0:	e5c14000 	strb	r4, [r1]
    23b4:	1a000005 	bne	23d0 <do_sc+0x180>
    23b8:	e59f0048 	ldr	r0, [pc, #72]	; 2408 <do_sc+0x1b8>
    23bc:	e59f303c 	ldr	r3, [pc, #60]	; 2400 <do_sc+0x1b0>
    23c0:	e1a0e00f 	mov	lr, pc
    23c4:	e593f000 	ldr	pc, [r3]
    23c8:	e3a00000 	mov	r0, #0	; 0x0
    23cc:	ea000007 	b	23f0 <do_sc+0x1a0>
    23d0:	e1a01006 	mov	r1, r6
    23d4:	e59f0030 	ldr	r0, [pc, #48]	; 240c <do_sc+0x1bc>
    23d8:	e1a0e00f 	mov	lr, pc
    23dc:	e597f000 	ldr	pc, [r7]
    23e0:	e1a00006 	mov	r0, r6
    23e4:	e1a01004 	mov	r1, r4
    23e8:	eb004139 	bl	128d4 <run_command>
    23ec:	eaffffd6 	b	234c <do_sc+0xfc>
    23f0:	e28ddf47 	add	sp, sp, #284	; 0x11c
    23f4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    23f8:	00015f54 	.word	0x00015f54
    23fc:	00016675 	.word	0x00016675
    2400:	3c00d6cc 	.word	0x3c00d6cc
    2404:	00016692 	.word	0x00016692
    2408:	000166ab 	.word	0x000166ab
    240c:	000166b8 	.word	0x000166b8

00002410 <do_ls>:
    2410:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2414:	e59f001c 	ldr	r0, [pc, #28]	; 2438 <do_ls+0x28>
    2418:	e24dd004 	sub	sp, sp, #4	; 0x4
    241c:	e59f1018 	ldr	r1, [pc, #24]	; 243c <do_ls+0x2c>
    2420:	e59f3018 	ldr	r3, [pc, #24]	; 2440 <do_ls+0x30>
    2424:	e1a0e00f 	mov	lr, pc
    2428:	e593f000 	ldr	pc, [r3]
    242c:	e3a00000 	mov	r0, #0	; 0x0
    2430:	e28dd004 	add	sp, sp, #4	; 0x4
    2434:	e8bd8000 	pop	{pc}
    2438:	0001641f 	.word	0x0001641f
    243c:	3c00d088 	.word	0x3c00d088
    2440:	3c00d6cc 	.word	0x3c00d6cc

00002444 <do_linkpc>:
    2444:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2448:	e24dd004 	sub	sp, sp, #4	; 0x4
    244c:	eb003a40 	bl	10d54 <linkpc_loop>
    2450:	e3a00000 	mov	r0, #0	; 0x0
    2454:	e28dd004 	add	sp, sp, #4	; 0x4
    2458:	e8bd8000 	pop	{pc}

0000245c <do_pads>:
    245c:	e3520002 	cmp	r2, #2	; 0x2
    2460:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    2464:	d59f0194 	ldrle	r0, [pc, #404]	; 2600 <do_pads+0x1a4>
    2468:	e1a05002 	mov	r5, r2
    246c:	e1a06003 	mov	r6, r3
    2470:	da000015 	ble	24cc <do_pads+0x70>
    2474:	e5930008 	ldr	r0, [r3, #8]
    2478:	e3a01000 	mov	r1, #0	; 0x0
    247c:	e3a0200a 	mov	r2, #10	; 0xa
    2480:	eb003cbf 	bl	11784 <simple_strtoul>
    2484:	e3550003 	cmp	r5, #3	; 0x3
    2488:	e1a04000 	mov	r4, r0
    248c:	01a07000 	moveq	r7, r0
    2490:	0a000004 	beq	24a8 <do_pads+0x4c>
    2494:	e596000c 	ldr	r0, [r6, #12]
    2498:	e3a01000 	mov	r1, #0	; 0x0
    249c:	e3a0200a 	mov	r2, #10	; 0xa
    24a0:	eb003cb7 	bl	11784 <simple_strtoul>
    24a4:	e1a07000 	mov	r7, r0
    24a8:	e1943007 	orrs	r3, r4, r7
    24ac:	4a000005 	bmi	24c8 <do_pads+0x6c>
    24b0:	e1a05004 	mov	r5, r4
    24b4:	e2866004 	add	r6, r6, #4	; 0x4
    24b8:	e59f9144 	ldr	r9, [pc, #324]	; 2604 <do_pads+0x1a8>
    24bc:	e59f8144 	ldr	r8, [pc, #324]	; 2608 <do_pads+0x1ac>
    24c0:	e59fa144 	ldr	sl, [pc, #324]	; 260c <do_pads+0x1b0>
    24c4:	ea000049 	b	25f0 <do_pads+0x194>
    24c8:	e59f0140 	ldr	r0, [pc, #320]	; 2610 <do_pads+0x1b4>
    24cc:	e59f3134 	ldr	r3, [pc, #308]	; 2608 <do_pads+0x1ac>
    24d0:	e1a0e00f 	mov	lr, pc
    24d4:	e593f000 	ldr	pc, [r3]
    24d8:	ea000046 	b	25f8 <do_pads+0x19c>
    24dc:	e5960000 	ldr	r0, [r6]
    24e0:	e59f111c 	ldr	r1, [pc, #284]	; 2604 <do_pads+0x1a8>
    24e4:	e3a02002 	mov	r2, #2	; 0x2
    24e8:	ebfffbe2 	bl	1478 <strncmp>
    24ec:	e3500000 	cmp	r0, #0	; 0x0
    24f0:	1a00000b 	bne	2524 <do_pads+0xc8>
    24f4:	e1a02000 	mov	r2, r0
    24f8:	e3a01001 	mov	r1, #1	; 0x1
    24fc:	e1a00005 	mov	r0, r5
    2500:	ebfff9e3 	bl	c94 <pads_chmod>
    2504:	e1a00005 	mov	r0, r5
    2508:	e5984000 	ldr	r4, [r8]
    250c:	ebfffa87 	bl	f30 <pads_getpin>
    2510:	e1a01005 	mov	r1, r5
    2514:	e1a02000 	mov	r2, r0
    2518:	e59f00f4 	ldr	r0, [pc, #244]	; 2614 <do_pads+0x1b8>
    251c:	e12fff34 	blx	r4
    2520:	ea000031 	b	25ec <do_pads+0x190>
    2524:	e59f10ec 	ldr	r1, [pc, #236]	; 2618 <do_pads+0x1bc>
    2528:	e3a02004 	mov	r2, #4	; 0x4
    252c:	e5960000 	ldr	r0, [r6]
    2530:	ebfffbd0 	bl	1478 <strncmp>
    2534:	e3500000 	cmp	r0, #0	; 0x0
    2538:	01a00005 	moveq	r0, r5
    253c:	03a01002 	moveq	r1, #2	; 0x2
    2540:	03a02001 	moveq	r2, #1	; 0x1
    2544:	0a000011 	beq	2590 <do_pads+0x134>
    2548:	e59f10cc 	ldr	r1, [pc, #204]	; 261c <do_pads+0x1c0>
    254c:	e3a02003 	mov	r2, #3	; 0x3
    2550:	e5960000 	ldr	r0, [r6]
    2554:	ebfffbc7 	bl	1478 <strncmp>
    2558:	e3500000 	cmp	r0, #0	; 0x0
    255c:	01a02000 	moveq	r2, r0
    2560:	03a01002 	moveq	r1, #2	; 0x2
    2564:	01a00005 	moveq	r0, r5
    2568:	0a000008 	beq	2590 <do_pads+0x134>
    256c:	e5960000 	ldr	r0, [r6]
    2570:	e59f10a8 	ldr	r1, [pc, #168]	; 2620 <do_pads+0x1c4>
    2574:	e3a02004 	mov	r2, #4	; 0x4
    2578:	ebfffbbe 	bl	1478 <strncmp>
    257c:	e3500000 	cmp	r0, #0	; 0x0
    2580:	1a000004 	bne	2598 <do_pads+0x13c>
    2584:	e1a01000 	mov	r1, r0
    2588:	e1a02001 	mov	r2, r1
    258c:	e1a00005 	mov	r0, r5
    2590:	ebfff9bf 	bl	c94 <pads_chmod>
    2594:	ea000014 	b	25ec <do_pads+0x190>
    2598:	e5960000 	ldr	r0, [r6]
    259c:	e59f1080 	ldr	r1, [pc, #128]	; 2624 <do_pads+0x1c8>
    25a0:	e3a02002 	mov	r2, #2	; 0x2
    25a4:	ebfffbb3 	bl	1478 <strncmp>
    25a8:	e3500000 	cmp	r0, #0	; 0x0
    25ac:	1a00000c 	bne	25e4 <do_pads+0x188>
    25b0:	e1a00005 	mov	r0, r5
    25b4:	ebfff9a0 	bl	c3c <pads_states>
    25b8:	e3500000 	cmp	r0, #0	; 0x0
    25bc:	e5983000 	ldr	r3, [r8]
    25c0:	059f2060 	ldreq	r2, [pc, #96]	; 2628 <do_pads+0x1cc>
    25c4:	0a000002 	beq	25d4 <do_pads+0x178>
    25c8:	e3500001 	cmp	r0, #1	; 0x1
    25cc:	01a02009 	moveq	r2, r9
    25d0:	11a0200a 	movne	r2, sl
    25d4:	e59f0050 	ldr	r0, [pc, #80]	; 262c <do_pads+0x1d0>
    25d8:	e1a01005 	mov	r1, r5
    25dc:	e12fff33 	blx	r3
    25e0:	ea000001 	b	25ec <do_pads+0x190>
    25e4:	e59f0044 	ldr	r0, [pc, #68]	; 2630 <do_pads+0x1d4>
    25e8:	eaffffb7 	b	24cc <do_pads+0x70>
    25ec:	e2855001 	add	r5, r5, #1	; 0x1
    25f0:	e1550007 	cmp	r5, r7
    25f4:	daffffb8 	ble	24dc <do_pads+0x80>
    25f8:	e3a00000 	mov	r0, #0	; 0x0
    25fc:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    2600:	000166dc 	.word	0x000166dc
    2604:	00016719 	.word	0x00016719
    2608:	3c00d6cc 	.word	0x3c00d6cc
    260c:	00018d83 	.word	0x00018d83
    2610:	000166f4 	.word	0x000166f4
    2614:	0001671c 	.word	0x0001671c
    2618:	00016730 	.word	0x00016730
    261c:	00016735 	.word	0x00016735
    2620:	00016739 	.word	0x00016739
    2624:	0001673e 	.word	0x0001673e
    2628:	00016741 	.word	0x00016741
    262c:	0001674a 	.word	0x0001674a
    2630:	0001675f 	.word	0x0001675f

00002634 <do_mux>:
    2634:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2638:	e3520001 	cmp	r2, #1	; 0x1
    263c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2640:	da000005 	ble	265c <do_mux+0x28>
    2644:	e5930004 	ldr	r0, [r3, #4]
    2648:	e3a01000 	mov	r1, #0	; 0x0
    264c:	e3a0200a 	mov	r2, #10	; 0xa
    2650:	eb003c4b 	bl	11784 <simple_strtoul>
    2654:	e6ff0070 	uxth	r0, r0
    2658:	ebfffb4a 	bl	1388 <set_mux>
    265c:	e59f301c 	ldr	r3, [pc, #28]	; 2680 <do_mux+0x4c>
    2660:	e59f001c 	ldr	r0, [pc, #28]	; 2684 <do_mux+0x50>
    2664:	e5931000 	ldr	r1, [r3]
    2668:	e59f3018 	ldr	r3, [pc, #24]	; 2688 <do_mux+0x54>
    266c:	e1a0e00f 	mov	lr, pc
    2670:	e593f000 	ldr	pc, [r3]
    2674:	e3a00000 	mov	r0, #0	; 0x0
    2678:	e28dd004 	add	sp, sp, #4	; 0x4
    267c:	e8bd8000 	pop	{pc}
    2680:	21e09000 	.word	0x21e09000
    2684:	00016778 	.word	0x00016778
    2688:	3c00d6cc 	.word	0x3c00d6cc

0000268c <do_mac>:
    268c:	e59f3020 	ldr	r3, [pc, #32]	; 26b4 <do_mac+0x28>
    2690:	e92d4010 	push	{r4, lr}
    2694:	e5934000 	ldr	r4, [r3]
    2698:	ebfff8f1 	bl	a64 <efuse_mac>
    269c:	e1a02000 	mov	r2, r0
    26a0:	e1a03001 	mov	r3, r1
    26a4:	e59f000c 	ldr	r0, [pc, #12]	; 26b8 <do_mac+0x2c>
    26a8:	e12fff34 	blx	r4
    26ac:	e3a00000 	mov	r0, #0	; 0x0
    26b0:	e8bd8010 	pop	{r4, pc}
    26b4:	3c00d6cc 	.word	0x3c00d6cc
    26b8:	0001678a 	.word	0x0001678a

000026bc <do_boot>:
    26bc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    26c0:	e3a00000 	mov	r0, #0	; 0x0
    26c4:	e24dd004 	sub	sp, sp, #4	; 0x4
    26c8:	ebfff75e 	bl	448 <try_boot>
    26cc:	e3a00000 	mov	r0, #0	; 0x0
    26d0:	e28dd004 	add	sp, sp, #4	; 0x4
    26d4:	e8bd8000 	pop	{pc}

000026d8 <do_efuse>:
    26d8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    26dc:	e3520001 	cmp	r2, #1	; 0x1
    26e0:	e24dd004 	sub	sp, sp, #4	; 0x4
    26e4:	da000006 	ble	2704 <do_efuse+0x2c>
    26e8:	e5930004 	ldr	r0, [r3, #4]
    26ec:	e59f1024 	ldr	r1, [pc, #36]	; 2718 <do_efuse+0x40>
    26f0:	e3a02002 	mov	r2, #2	; 0x2
    26f4:	ebfffb5f 	bl	1478 <strncmp>
    26f8:	e3500000 	cmp	r0, #0	; 0x0
    26fc:	02800001 	addeq	r0, r0, #1	; 0x1
    2700:	0a000000 	beq	2708 <do_efuse+0x30>
    2704:	e3a00000 	mov	r0, #0	; 0x0
    2708:	ebfff8d8 	bl	a70 <efuse_info>
    270c:	e3a00000 	mov	r0, #0	; 0x0
    2710:	e28dd004 	add	sp, sp, #4	; 0x4
    2714:	e8bd8000 	pop	{pc}
    2718:	000167a2 	.word	0x000167a2

0000271c <do_pi>:
    271c:	e92d4030 	push	{r4, r5, lr}
    2720:	e3520001 	cmp	r2, #1	; 0x1
    2724:	e24dd004 	sub	sp, sp, #4	; 0x4
    2728:	e1a04002 	mov	r4, r2
    272c:	e1a05003 	mov	r5, r3
    2730:	da000015 	ble	278c <do_pi+0x70>
    2734:	e59f10ac 	ldr	r1, [pc, #172]	; 27e8 <do_pi+0xcc>
    2738:	e5930004 	ldr	r0, [r3, #4]
    273c:	e3a02004 	mov	r2, #4	; 0x4
    2740:	ebfffb4c 	bl	1478 <strncmp>
    2744:	e2501000 	subs	r1, r0, #0	; 0x0
    2748:	1a00000d 	bne	2784 <do_pi+0x68>
    274c:	e3540002 	cmp	r4, #2	; 0x2
    2750:	01a04001 	moveq	r4, r1
    2754:	0a000003 	beq	2768 <do_pi+0x4c>
    2758:	e5950008 	ldr	r0, [r5, #8]
    275c:	e3a02010 	mov	r2, #16	; 0x10
    2760:	eb003c07 	bl	11784 <simple_strtoul>
    2764:	e1a04000 	mov	r4, r0
    2768:	e59f007c 	ldr	r0, [pc, #124]	; 27ec <do_pi+0xd0>
    276c:	e1a01004 	mov	r1, r4
    2770:	e59f3078 	ldr	r3, [pc, #120]	; 27f0 <do_pi+0xd4>
    2774:	e1a0e00f 	mov	lr, pc
    2778:	e593f000 	ldr	pc, [r3]
    277c:	e1a00004 	mov	r0, r4
    2780:	eb00159f 	bl	7e04 <burn>
    2784:	e3a04000 	mov	r4, #0	; 0x0
    2788:	ea000013 	b	27dc <do_pi+0xc0>
    278c:	e3a00481 	mov	r0, #-2130706432	; 0x81000000
    2790:	eb003f8f 	bl	125d4 <ius_check_header>
    2794:	e2504000 	subs	r4, r0, #0	; 0x0
    2798:	159f0054 	ldrne	r0, [pc, #84]	; 27f4 <do_pi+0xd8>
    279c:	1a000005 	bne	27b8 <do_pi+0x9c>
    27a0:	e3a0000d 	mov	r0, #13	; 0xd
    27a4:	e3a01001 	mov	r1, #1	; 0x1
    27a8:	eb000f4d 	bl	64e4 <vs_assign_by_id>
    27ac:	e2504000 	subs	r4, r0, #0	; 0x0
    27b0:	0a000005 	beq	27cc <do_pi+0xb0>
    27b4:	e59f003c 	ldr	r0, [pc, #60]	; 27f8 <do_pi+0xdc>
    27b8:	e1a01004 	mov	r1, r4
    27bc:	e59f302c 	ldr	r3, [pc, #44]	; 27f0 <do_pi+0xd4>
    27c0:	e1a0e00f 	mov	lr, pc
    27c4:	e593f000 	ldr	pc, [r3]
    27c8:	ea000003 	b	27dc <do_pi+0xc0>
    27cc:	e3a00481 	mov	r0, #-2130706432	; 0x81000000
    27d0:	e3a0100d 	mov	r1, #13	; 0xd
    27d4:	e1a02004 	mov	r2, r4
    27d8:	eb001501 	bl	7be4 <burn_images>
    27dc:	e1a00004 	mov	r0, r4
    27e0:	e28dd004 	add	sp, sp, #4	; 0x4
    27e4:	e8bd8030 	pop	{r4, r5, pc}
    27e8:	000167a5 	.word	0x000167a5
    27ec:	000167aa 	.word	0x000167aa
    27f0:	3c00d6cc 	.word	0x3c00d6cc
    27f4:	000167d4 	.word	0x000167d4
    27f8:	000167f4 	.word	0x000167f4

000027fc <do_spbyte>:
    27fc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2800:	e5930004 	ldr	r0, [r3, #4]
    2804:	e24dd004 	sub	sp, sp, #4	; 0x4
    2808:	e3a01000 	mov	r1, #0	; 0x0
    280c:	e3a0200a 	mov	r2, #10	; 0xa
    2810:	eb003bdb 	bl	11784 <simple_strtoul>
    2814:	eb002ee2 	bl	e3a4 <spi_set_bytes>
    2818:	e3a00000 	mov	r0, #0	; 0x0
    281c:	e28dd004 	add	sp, sp, #4	; 0x4
    2820:	e8bd8000 	pop	{pc}

00002824 <do_cfg>:
    2824:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2828:	e5930004 	ldr	r0, [r3, #4]
    282c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2830:	e3a01000 	mov	r1, #0	; 0x0
    2834:	e3a02010 	mov	r2, #16	; 0x10
    2838:	eb003bd1 	bl	11784 <simple_strtoul>
    283c:	eb00138f 	bl	7680 <cdbg_config_isi>
    2840:	e3a00000 	mov	r0, #0	; 0x0
    2844:	e28dd004 	add	sp, sp, #4	; 0x4
    2848:	e8bd8000 	pop	{pc}

0000284c <do_sleep>:
    284c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2850:	e3a01000 	mov	r1, #0	; 0x0
    2854:	e24dd004 	sub	sp, sp, #4	; 0x4
    2858:	e3a0200a 	mov	r2, #10	; 0xa
    285c:	e5930004 	ldr	r0, [r3, #4]
    2860:	eb003bc7 	bl	11784 <simple_strtoul>
    2864:	e59f3020 	ldr	r3, [pc, #32]	; 288c <do_sleep+0x40>
    2868:	e0000093 	mul	r0, r3, r0
    286c:	eb002c5c 	bl	d9e4 <udelay>
    2870:	e59f0018 	ldr	r0, [pc, #24]	; 2890 <do_sleep+0x44>
    2874:	e59f3018 	ldr	r3, [pc, #24]	; 2894 <do_sleep+0x48>
    2878:	e1a0e00f 	mov	lr, pc
    287c:	e593f000 	ldr	pc, [r3]
    2880:	e3a00000 	mov	r0, #0	; 0x0
    2884:	e28dd004 	add	sp, sp, #4	; 0x4
    2888:	e8bd8000 	pop	{pc}
    288c:	000f4240 	.word	0x000f4240
    2890:	0001680f 	.word	0x0001680f
    2894:	3c00d6cc 	.word	0x3c00d6cc

00002898 <do_go>:
    2898:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    289c:	e5930004 	ldr	r0, [r3, #4]
    28a0:	e24dd004 	sub	sp, sp, #4	; 0x4
    28a4:	e3a02010 	mov	r2, #16	; 0x10
    28a8:	e3a01000 	mov	r1, #0	; 0x0
    28ac:	eb003bb4 	bl	11784 <simple_strtoul>
    28b0:	e3a01000 	mov	r1, #0	; 0x0
    28b4:	eb0012de 	bl	7434 <cdbg_jump>
    28b8:	e3a00000 	mov	r0, #0	; 0x0
    28bc:	e28dd004 	add	sp, sp, #4	; 0x4
    28c0:	e8bd8000 	pop	{pc}

000028c4 <do_ml>:
    28c4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    28c8:	e24dd004 	sub	sp, sp, #4	; 0x4
    28cc:	ebfffc18 	bl	1934 <__sOINFo>
    28d0:	e3a00000 	mov	r0, #0	; 0x0
    28d4:	e28dd004 	add	sp, sp, #4	; 0x4
    28d8:	e8bd8000 	pop	{pc}

000028dc <do_bp>:
    28dc:	e92d4030 	push	{r4, r5, lr}
    28e0:	e3520001 	cmp	r2, #1	; 0x1
    28e4:	e24dd004 	sub	sp, sp, #4	; 0x4
    28e8:	da000005 	ble	2904 <do_bp+0x28>
    28ec:	e5930004 	ldr	r0, [r3, #4]
    28f0:	e3a01000 	mov	r1, #0	; 0x0
    28f4:	e3a02010 	mov	r2, #16	; 0x10
    28f8:	eb003ba1 	bl	11784 <simple_strtoul>
    28fc:	e6ff0070 	uxth	r0, r0
    2900:	ebfff736 	bl	5e0 <set_xom>
    2904:	e59f5034 	ldr	r5, [pc, #52]	; 2940 <do_bp+0x64>
    2908:	e3a00001 	mov	r0, #1	; 0x1
    290c:	e5954000 	ldr	r4, [r5]
    2910:	ebfff72e 	bl	5d0 <get_xom>
    2914:	e1a01000 	mov	r1, r0
    2918:	e59f0024 	ldr	r0, [pc, #36]	; 2944 <do_bp+0x68>
    291c:	e12fff34 	blx	r4
    2920:	e5954000 	ldr	r4, [r5]
    2924:	ebfff733 	bl	5f8 <boot_info>
    2928:	e1a01000 	mov	r1, r0
    292c:	e59f0014 	ldr	r0, [pc, #20]	; 2948 <do_bp+0x6c>
    2930:	e12fff34 	blx	r4
    2934:	e3a00000 	mov	r0, #0	; 0x0
    2938:	e28dd004 	add	sp, sp, #4	; 0x4
    293c:	e8bd8030 	pop	{r4, r5, pc}
    2940:	3c00d6cc 	.word	0x3c00d6cc
    2944:	0001681a 	.word	0x0001681a
    2948:	00016830 	.word	0x00016830

0000294c <do_reset>:
    294c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    2950:	e24dd004 	sub	sp, sp, #4	; 0x4
    2954:	ebfff7be 	bl	854 <system_reset>
    2958:	e3a00000 	mov	r0, #0	; 0x0
    295c:	e28dd004 	add	sp, sp, #4	; 0x4
    2960:	e8bd8000 	pop	{pc}

00002964 <output_debug>:
    2964:	e24dd010 	sub	sp, sp, #16	; 0x10
    2968:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    296c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2970:	e28dc008 	add	ip, sp, #8	; 0x8
    2974:	e3700002 	cmn	r0, #2	; 0x2
    2978:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    297c:	e1a01000 	mov	r1, r0
    2980:	0a000012 	beq	29d0 <output_debug+0x6c>
    2984:	e3700001 	cmn	r0, #1	; 0x1
    2988:	0a000008 	beq	29b0 <output_debug+0x4c>
    298c:	e3700003 	cmn	r0, #3	; 0x3
    2990:	1a000010 	bne	29d8 <output_debug+0x74>
    2994:	e59f30a4 	ldr	r3, [pc, #164]	; 2a40 <output_debug+0xdc>
    2998:	e59f00a4 	ldr	r0, [pc, #164]	; 2a44 <output_debug+0xe0>
    299c:	e5931000 	ldr	r1, [r3]
    29a0:	e59f30a0 	ldr	r3, [pc, #160]	; 2a48 <output_debug+0xe4>
    29a4:	e1a0e00f 	mov	lr, pc
    29a8:	e593f000 	ldr	pc, [r3]
    29ac:	ea00001e 	b	2a2c <output_debug+0xc8>
    29b0:	e59f3088 	ldr	r3, [pc, #136]	; 2a40 <output_debug+0xdc>
    29b4:	e59f0090 	ldr	r0, [pc, #144]	; 2a4c <output_debug+0xe8>
    29b8:	e5932000 	ldr	r2, [r3]
    29bc:	e28d1010 	add	r1, sp, #16	; 0x10
    29c0:	e59f3080 	ldr	r3, [pc, #128]	; 2a48 <output_debug+0xe4>
    29c4:	e1a0e00f 	mov	lr, pc
    29c8:	e593f000 	ldr	pc, [r3]
    29cc:	ea000016 	b	2a2c <output_debug+0xc8>
    29d0:	e59f0078 	ldr	r0, [pc, #120]	; 2a50 <output_debug+0xec>
    29d4:	ea000003 	b	29e8 <output_debug+0x84>
    29d8:	e59d300c 	ldr	r3, [sp, #12]
    29dc:	e3530001 	cmp	r3, #1	; 0x1
    29e0:	1a000004 	bne	29f8 <output_debug+0x94>
    29e4:	e59f0068 	ldr	r0, [pc, #104]	; 2a54 <output_debug+0xf0>
    29e8:	e59f3058 	ldr	r3, [pc, #88]	; 2a48 <output_debug+0xe4>
    29ec:	e1a0e00f 	mov	lr, pc
    29f0:	e593f000 	ldr	pc, [r3]
    29f4:	ea00000c 	b	2a2c <output_debug+0xc8>
    29f8:	e3530002 	cmp	r3, #2	; 0x2
    29fc:	e59fc044 	ldr	ip, [pc, #68]	; 2a48 <output_debug+0xe4>
    2a00:	1a000003 	bne	2a14 <output_debug+0xb0>
    2a04:	e59f004c 	ldr	r0, [pc, #76]	; 2a58 <output_debug+0xf4>
    2a08:	e1a0e00f 	mov	lr, pc
    2a0c:	e59cf000 	ldr	pc, [ip]
    2a10:	ea000005 	b	2a2c <output_debug+0xc8>
    2a14:	e59f3024 	ldr	r3, [pc, #36]	; 2a40 <output_debug+0xdc>
    2a18:	e59f003c 	ldr	r0, [pc, #60]	; 2a5c <output_debug+0xf8>
    2a1c:	e5933000 	ldr	r3, [r3]
    2a20:	e28d2010 	add	r2, sp, #16	; 0x10
    2a24:	e1a0e00f 	mov	lr, pc
    2a28:	e59cf000 	ldr	pc, [ip]
    2a2c:	e3a00000 	mov	r0, #0	; 0x0
    2a30:	e28dd004 	add	sp, sp, #4	; 0x4
    2a34:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    2a38:	e28dd010 	add	sp, sp, #16	; 0x10
    2a3c:	e12fff1e 	bx	lr
    2a40:	3c00d130 	.word	0x3c00d130
    2a44:	000169bb 	.word	0x000169bb
    2a48:	3c00d6cc 	.word	0x3c00d6cc
    2a4c:	000169e3 	.word	0x000169e3
    2a50:	000169f2 	.word	0x000169f2
    2a54:	00016a0c 	.word	0x00016a0c
    2a58:	00016a17 	.word	0x00016a17
    2a5c:	00016a25 	.word	0x00016a25

00002a60 <do_vs>:
    2a60:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    2a64:	e3520006 	cmp	r2, #6	; 0x6
    2a68:	e24dd024 	sub	sp, sp, #36	; 0x24
    2a6c:	e1a06000 	mov	r6, r0
    2a70:	e1a04003 	mov	r4, r3
    2a74:	1a000005 	bne	2a90 <do_vs+0x30>
    2a78:	e5930014 	ldr	r0, [r3, #20]
    2a7c:	e3a01000 	mov	r1, #0	; 0x0
    2a80:	e282200a 	add	r2, r2, #10	; 0xa
    2a84:	eb003b3e 	bl	11784 <simple_strtoul>
    2a88:	e1a05000 	mov	r5, r0
    2a8c:	ea00008f 	b	2cd0 <do_vs+0x270>
    2a90:	e2423001 	sub	r3, r2, #1	; 0x1
    2a94:	e3530005 	cmp	r3, #5	; 0x5
    2a98:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    2a9c:	ea0000c4 	b	2db4 <do_vs+0x354>
    2aa0:	00002db4 	.word	0x00002db4
    2aa4:	00002ab8 	.word	0x00002ab8
    2aa8:	00002bb0 	.word	0x00002bb0
    2aac:	00002c20 	.word	0x00002c20
    2ab0:	00002ccc 	.word	0x00002ccc
    2ab4:	00002ccc 	.word	0x00002ccc
    2ab8:	e5940004 	ldr	r0, [r4, #4]
    2abc:	e59f1310 	ldr	r1, [pc, #784]	; 2dd4 <do_vs+0x374>
    2ac0:	ebfffa5e 	bl	1440 <strcmp>
    2ac4:	e2505000 	subs	r5, r0, #0	; 0x0
    2ac8:	1a000025 	bne	2b64 <do_vs+0x104>
    2acc:	e59f4304 	ldr	r4, [pc, #772]	; 2dd8 <do_vs+0x378>
    2ad0:	e59f0304 	ldr	r0, [pc, #772]	; 2ddc <do_vs+0x37c>
    2ad4:	e1a0e00f 	mov	lr, pc
    2ad8:	e594f000 	ldr	pc, [r4]
    2adc:	e3a0802c 	mov	r8, #44	; 0x2c
    2ae0:	e59f72f8 	ldr	r7, [pc, #760]	; 2de0 <do_vs+0x380>
    2ae4:	ea000003 	b	2af8 <do_vs+0x98>
    2ae8:	e0227598 	mla	r2, r8, r5, r7
    2aec:	e1a0e00f 	mov	lr, pc
    2af0:	e594f000 	ldr	pc, [r4]
    2af4:	e2855001 	add	r5, r5, #1	; 0x1
    2af8:	eb000da4 	bl	6190 <vs_device_count>
    2afc:	e1550000 	cmp	r5, r0
    2b00:	e1a01005 	mov	r1, r5
    2b04:	e59f02d8 	ldr	r0, [pc, #728]	; 2de4 <do_vs+0x384>
    2b08:	e59f62c8 	ldr	r6, [pc, #712]	; 2dd8 <do_vs+0x378>
    2b0c:	bafffff5 	blt	2ae8 <do_vs+0x88>
    2b10:	e59f02d0 	ldr	r0, [pc, #720]	; 2de8 <do_vs+0x388>
    2b14:	e1a0e00f 	mov	lr, pc
    2b18:	e596f000 	ldr	pc, [r6]
    2b1c:	eb000ca1 	bl	5da8 <vs_is_assigned>
    2b20:	e2504000 	subs	r4, r0, #0	; 0x0
    2b24:	0a000009 	beq	2b50 <do_vs+0xf0>
    2b28:	e59f32bc 	ldr	r3, [pc, #700]	; 2dec <do_vs+0x38c>
    2b2c:	e5965000 	ldr	r5, [r6]
    2b30:	e5934000 	ldr	r4, [r3]
    2b34:	e1a00004 	mov	r0, r4
    2b38:	eb000d15 	bl	5f94 <vs_align>
    2b3c:	e1a01004 	mov	r1, r4
    2b40:	e1a02000 	mov	r2, r0
    2b44:	e59f02a4 	ldr	r0, [pc, #676]	; 2df0 <do_vs+0x390>
    2b48:	e12fff35 	blx	r5
    2b4c:	ea00009d 	b	2dc8 <do_vs+0x368>
    2b50:	e59f029c 	ldr	r0, [pc, #668]	; 2df4 <do_vs+0x394>
    2b54:	e1a0e00f 	mov	lr, pc
    2b58:	e596f000 	ldr	pc, [r6]
    2b5c:	e1a00004 	mov	r0, r4
    2b60:	ea000099 	b	2dcc <do_vs+0x36c>
    2b64:	e5940004 	ldr	r0, [r4, #4]
    2b68:	e59f1288 	ldr	r1, [pc, #648]	; 2df8 <do_vs+0x398>
    2b6c:	ebfffa33 	bl	1440 <strcmp>
    2b70:	e2505000 	subs	r5, r0, #0	; 0x0
    2b74:	1a000087 	bne	2d98 <do_vs+0x338>
    2b78:	eb000e4a 	bl	64a8 <vs_reset>
    2b7c:	e28d4020 	add	r4, sp, #32	; 0x20
    2b80:	e5240018 	str	r0, [r4, #-24]!
    2b84:	e59f126c 	ldr	r1, [pc, #620]	; 2df8 <do_vs+0x398>
    2b88:	e2840008 	add	r0, r4, #8	; 0x8
    2b8c:	ebfffa01 	bl	1398 <strcpy>
    2b90:	e3a03001 	mov	r3, #1	; 0x1
    2b94:	e58d300c 	str	r3, [sp, #12]
    2b98:	e28d3018 	add	r3, sp, #24	; 0x18
    2b9c:	e8930003 	ldm	r3, {r0, r1}
    2ba0:	e88d0003 	stm	sp, {r0, r1}
    2ba4:	e894000f 	ldm	r4, {r0, r1, r2, r3}
    2ba8:	ebffff6d 	bl	2964 <output_debug>
    2bac:	ea000019 	b	2c18 <do_vs+0x1b8>
    2bb0:	e5940004 	ldr	r0, [r4, #4]
    2bb4:	e59f1240 	ldr	r1, [pc, #576]	; 2dfc <do_vs+0x39c>
    2bb8:	ebfffa20 	bl	1440 <strcmp>
    2bbc:	e2505000 	subs	r5, r0, #0	; 0x0
    2bc0:	1a000074 	bne	2d98 <do_vs+0x338>
    2bc4:	e5940008 	ldr	r0, [r4, #8]
    2bc8:	e3a01001 	mov	r1, #1	; 0x1
    2bcc:	eb000e62 	bl	655c <vs_assign_by_name>
    2bd0:	e2507000 	subs	r7, r0, #0	; 0x0
    2bd4:	e59f61fc 	ldr	r6, [pc, #508]	; 2dd8 <do_vs+0x378>
    2bd8:	1a00000b 	bne	2c0c <do_vs+0x1ac>
    2bdc:	e5943008 	ldr	r3, [r4, #8]
    2be0:	e59f2218 	ldr	r2, [pc, #536]	; 2e00 <do_vs+0x3a0>
    2be4:	e1a00003 	mov	r0, r3
    2be8:	e5823000 	str	r3, [r2]
    2bec:	eb000d69 	bl	6198 <vs_device_id>
    2bf0:	e59f31f4 	ldr	r3, [pc, #500]	; 2dec <do_vs+0x38c>
    2bf4:	e5830000 	str	r0, [r3]
    2bf8:	e59f0204 	ldr	r0, [pc, #516]	; 2e04 <do_vs+0x3a4>
    2bfc:	e1a0e00f 	mov	lr, pc
    2c00:	e596f000 	ldr	pc, [r6]
    2c04:	e1a00007 	mov	r0, r7
    2c08:	ea00006f 	b	2dcc <do_vs+0x36c>
    2c0c:	e59f01f4 	ldr	r0, [pc, #500]	; 2e08 <do_vs+0x3a8>
    2c10:	e1a0e00f 	mov	lr, pc
    2c14:	e596f000 	ldr	pc, [r6]
    2c18:	e1a00005 	mov	r0, r5
    2c1c:	ea00006a 	b	2dcc <do_vs+0x36c>
    2c20:	e3a02010 	mov	r2, #16	; 0x10
    2c24:	e5940008 	ldr	r0, [r4, #8]
    2c28:	e3a01000 	mov	r1, #0	; 0x0
    2c2c:	eb003b2c 	bl	118e4 <simple_strtoull>
    2c30:	e3a02010 	mov	r2, #16	; 0x10
    2c34:	e1a07000 	mov	r7, r0
    2c38:	e1a08001 	mov	r8, r1
    2c3c:	e594000c 	ldr	r0, [r4, #12]
    2c40:	e3a01000 	mov	r1, #0	; 0x0
    2c44:	eb003b26 	bl	118e4 <simple_strtoull>
    2c48:	e59f11bc 	ldr	r1, [pc, #444]	; 2e0c <do_vs+0x3ac>
    2c4c:	e1a05000 	mov	r5, r0
    2c50:	e28d0010 	add	r0, sp, #16	; 0x10
    2c54:	ebfff9cf 	bl	1398 <strcpy>
    2c58:	e3a03002 	mov	r3, #2	; 0x2
    2c5c:	e5940004 	ldr	r0, [r4, #4]
    2c60:	e59f11a4 	ldr	r1, [pc, #420]	; 2e0c <do_vs+0x3ac>
    2c64:	e58d300c 	str	r3, [sp, #12]
    2c68:	ebfff9f4 	bl	1440 <strcmp>
    2c6c:	e3500000 	cmp	r0, #0	; 0x0
    2c70:	1a000004 	bne	2c88 <do_vs+0x228>
    2c74:	e1a00007 	mov	r0, r7
    2c78:	e1a01008 	mov	r1, r8
    2c7c:	e1a02005 	mov	r2, r5
    2c80:	eb000c87 	bl	5ea4 <vs_erase>
    2c84:	ea000008 	b	2cac <do_vs+0x24c>
    2c88:	e5940004 	ldr	r0, [r4, #4]
    2c8c:	e59f117c 	ldr	r1, [pc, #380]	; 2e10 <do_vs+0x3b0>
    2c90:	ebfff9ea 	bl	1440 <strcmp>
    2c94:	e3500000 	cmp	r0, #0	; 0x0
    2c98:	1a00003e 	bne	2d98 <do_vs+0x338>
    2c9c:	e1a00007 	mov	r0, r7
    2ca0:	e1a01008 	mov	r1, r8
    2ca4:	e1a02005 	mov	r2, r5
    2ca8:	eb000c9b 	bl	5f1c <vs_scrub>
    2cac:	e58d0008 	str	r0, [sp, #8]
    2cb0:	e28d2018 	add	r2, sp, #24	; 0x18
    2cb4:	e8920003 	ldm	r2, {r0, r1}
    2cb8:	e88d0003 	stm	sp, {r0, r1}
    2cbc:	e28d3008 	add	r3, sp, #8	; 0x8
    2cc0:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    2cc4:	ebffff26 	bl	2964 <output_debug>
    2cc8:	ea00003e 	b	2dc8 <do_vs+0x368>
    2ccc:	e3a05000 	mov	r5, #0	; 0x0
    2cd0:	e3a01000 	mov	r1, #0	; 0x0
    2cd4:	e3a02010 	mov	r2, #16	; 0x10
    2cd8:	e5940008 	ldr	r0, [r4, #8]
    2cdc:	eb003aa8 	bl	11784 <simple_strtoul>
    2ce0:	e3a02010 	mov	r2, #16	; 0x10
    2ce4:	e1a09000 	mov	r9, r0
    2ce8:	e3a01000 	mov	r1, #0	; 0x0
    2cec:	e594000c 	ldr	r0, [r4, #12]
    2cf0:	eb003afb 	bl	118e4 <simple_strtoull>
    2cf4:	e3a02010 	mov	r2, #16	; 0x10
    2cf8:	e1a08000 	mov	r8, r0
    2cfc:	e1a0a001 	mov	sl, r1
    2d00:	e5940010 	ldr	r0, [r4, #16]
    2d04:	e3a01000 	mov	r1, #0	; 0x0
    2d08:	e08db002 	add	fp, sp, r2
    2d0c:	eb003af4 	bl	118e4 <simple_strtoull>
    2d10:	e3a03000 	mov	r3, #0	; 0x0
    2d14:	e59f10f8 	ldr	r1, [pc, #248]	; 2e14 <do_vs+0x3b4>
    2d18:	e1a07000 	mov	r7, r0
    2d1c:	e1a0000b 	mov	r0, fp
    2d20:	e58d300c 	str	r3, [sp, #12]
    2d24:	ebfff99b 	bl	1398 <strcpy>
    2d28:	e5940004 	ldr	r0, [r4, #4]
    2d2c:	e59f10e4 	ldr	r1, [pc, #228]	; 2e18 <do_vs+0x3b8>
    2d30:	ebfff9c2 	bl	1440 <strcmp>
    2d34:	e3500000 	cmp	r0, #0	; 0x0
    2d38:	1a000006 	bne	2d58 <do_vs+0x2f8>
    2d3c:	e1a00009 	mov	r0, r9
    2d40:	e1a02008 	mov	r2, r8
    2d44:	e1a0300a 	mov	r3, sl
    2d48:	e58d7000 	str	r7, [sp]
    2d4c:	e58d5004 	str	r5, [sp, #4]
    2d50:	eb000c3d 	bl	5e4c <vs_write>
    2d54:	eaffffd4 	b	2cac <do_vs+0x24c>
    2d58:	e5940004 	ldr	r0, [r4, #4]
    2d5c:	e59f10b8 	ldr	r1, [pc, #184]	; 2e1c <do_vs+0x3bc>
    2d60:	ebfff9b6 	bl	1440 <strcmp>
    2d64:	e3500000 	cmp	r0, #0	; 0x0
    2d68:	1a00000a 	bne	2d98 <do_vs+0x338>
    2d6c:	e1a00009 	mov	r0, r9
    2d70:	e1a02008 	mov	r2, r8
    2d74:	e1a0300a 	mov	r3, sl
    2d78:	e58d7000 	str	r7, [sp]
    2d7c:	e58d5004 	str	r5, [sp, #4]
    2d80:	eb000d31 	bl	624c <vs_read>
    2d84:	e59f1094 	ldr	r1, [pc, #148]	; 2e20 <do_vs+0x3c0>
    2d88:	e58d0008 	str	r0, [sp, #8]
    2d8c:	e1a0000b 	mov	r0, fp
    2d90:	ebfff980 	bl	1398 <strcpy>
    2d94:	eaffffc5 	b	2cb0 <do_vs+0x250>
    2d98:	e59f0084 	ldr	r0, [pc, #132]	; 2e24 <do_vs+0x3c4>
    2d9c:	e5961014 	ldr	r1, [r6, #20]
    2da0:	e59f3030 	ldr	r3, [pc, #48]	; 2dd8 <do_vs+0x378>
    2da4:	e1a0e00f 	mov	lr, pc
    2da8:	e593f000 	ldr	pc, [r3]
    2dac:	e3e00000 	mvn	r0, #0	; 0x0
    2db0:	ea000005 	b	2dcc <do_vs+0x36c>
    2db4:	e5901014 	ldr	r1, [r0, #20]
    2db8:	e59f3018 	ldr	r3, [pc, #24]	; 2dd8 <do_vs+0x378>
    2dbc:	e59f0060 	ldr	r0, [pc, #96]	; 2e24 <do_vs+0x3c4>
    2dc0:	e1a0e00f 	mov	lr, pc
    2dc4:	e593f000 	ldr	pc, [r3]
    2dc8:	e3a00000 	mov	r0, #0	; 0x0
    2dcc:	e28dd024 	add	sp, sp, #36	; 0x24
    2dd0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    2dd4:	0001685f 	.word	0x0001685f
    2dd8:	3c00d6cc 	.word	0x3c00d6cc
    2ddc:	00016a3c 	.word	0x00016a3c
    2de0:	00015624 	.word	0x00015624
    2de4:	00016a4a 	.word	0x00016a4a
    2de8:	00016a52 	.word	0x00016a52
    2dec:	3c00d130 	.word	0x3c00d130
    2df0:	00016a64 	.word	0x00016a64
    2df4:	00016a6e 	.word	0x00016a6e
    2df8:	0001684d 	.word	0x0001684d
    2dfc:	00018b57 	.word	0x00018b57
    2e00:	3c00d134 	.word	0x3c00d134
    2e04:	00016a81 	.word	0x00016a81
    2e08:	00016a8d 	.word	0x00016a8d
    2e0c:	00016aba 	.word	0x00016aba
    2e10:	00016ac0 	.word	0x00016ac0
    2e14:	00016ac6 	.word	0x00016ac6
    2e18:	00016acf 	.word	0x00016acf
    2e1c:	00016ad5 	.word	0x00016ad5
    2e20:	00016ada 	.word	0x00016ada
    2e24:	0001641e 	.word	0x0001641e

00002e28 <cmd_get_data_size>:
    2e28:	e92d4030 	push	{r4, r5, lr}
    2e2c:	e24dd004 	sub	sp, sp, #4	; 0x4
    2e30:	e1a04001 	mov	r4, r1
    2e34:	e1a05000 	mov	r5, r0
    2e38:	ebfff9ad 	bl	14f4 <strlen>
    2e3c:	e3500002 	cmp	r0, #2	; 0x2
    2e40:	da000015 	ble	2e9c <cmd_get_data_size+0x74>
    2e44:	e0850000 	add	r0, r5, r0
    2e48:	e5503002 	ldrb	r3, [r0, #-2]
    2e4c:	e353002e 	cmp	r3, #46	; 0x2e
    2e50:	1a000011 	bne	2e9c <cmd_get_data_size+0x74>
    2e54:	e5500001 	ldrb	r0, [r0, #-1]
    2e58:	e350006c 	cmp	r0, #108	; 0x6c
    2e5c:	03a04004 	moveq	r4, #4	; 0x4
    2e60:	0a00000d 	beq	2e9c <cmd_get_data_size+0x74>
    2e64:	8a000002 	bhi	2e74 <cmd_get_data_size+0x4c>
    2e68:	e3500062 	cmp	r0, #98	; 0x62
    2e6c:	1a000009 	bne	2e98 <cmd_get_data_size+0x70>
    2e70:	ea000006 	b	2e90 <cmd_get_data_size+0x68>
    2e74:	e3500073 	cmp	r0, #115	; 0x73
    2e78:	03e04001 	mvneq	r4, #1	; 0x1
    2e7c:	0a000006 	beq	2e9c <cmd_get_data_size+0x74>
    2e80:	e3500077 	cmp	r0, #119	; 0x77
    2e84:	03a04002 	moveq	r4, #2	; 0x2
    2e88:	1a000002 	bne	2e98 <cmd_get_data_size+0x70>
    2e8c:	ea000002 	b	2e9c <cmd_get_data_size+0x74>
    2e90:	e3a04001 	mov	r4, #1	; 0x1
    2e94:	ea000000 	b	2e9c <cmd_get_data_size+0x74>
    2e98:	e3e04000 	mvn	r4, #0	; 0x0
    2e9c:	e1a00004 	mov	r0, r4
    2ea0:	e28dd004 	add	sp, sp, #4	; 0x4
    2ea4:	e8bd8030 	pop	{r4, r5, pc}

00002ea8 <cmd_usage>:
    2ea8:	e92d4030 	push	{r4, r5, lr}
    2eac:	e1a04000 	mov	r4, r0
    2eb0:	e24dd004 	sub	sp, sp, #4	; 0x4
    2eb4:	e59f5054 	ldr	r5, [pc, #84]	; 2f10 <cmd_usage+0x68>
    2eb8:	e59f0054 	ldr	r0, [pc, #84]	; 2f14 <cmd_usage+0x6c>
    2ebc:	e5941000 	ldr	r1, [r4]
    2ec0:	e5942010 	ldr	r2, [r4, #16]
    2ec4:	e1a0e00f 	mov	lr, pc
    2ec8:	e595f000 	ldr	pc, [r5]
    2ecc:	e59f0044 	ldr	r0, [pc, #68]	; 2f18 <cmd_usage+0x70>
    2ed0:	e5941000 	ldr	r1, [r4]
    2ed4:	e1a0e00f 	mov	lr, pc
    2ed8:	e595f000 	ldr	pc, [r5]
    2edc:	e5940014 	ldr	r0, [r4, #20]
    2ee0:	e3500000 	cmp	r0, #0	; 0x0
    2ee4:	1a000003 	bne	2ef8 <cmd_usage+0x50>
    2ee8:	e59f002c 	ldr	r0, [pc, #44]	; 2f1c <cmd_usage+0x74>
    2eec:	eb003e12 	bl	1273c <puts>
    2ef0:	e3a00001 	mov	r0, #1	; 0x1
    2ef4:	ea000003 	b	2f08 <cmd_usage+0x60>
    2ef8:	eb003e0f 	bl	1273c <puts>
    2efc:	e3a0000a 	mov	r0, #10	; 0xa
    2f00:	eb003e35 	bl	127dc <putc>
    2f04:	e3a00000 	mov	r0, #0	; 0x0
    2f08:	e28dd004 	add	sp, sp, #4	; 0x4
    2f0c:	e8bd8030 	pop	{r4, r5, pc}
    2f10:	3c00d6cc 	.word	0x3c00d6cc
    2f14:	00016bcf 	.word	0x00016bcf
    2f18:	00016bd9 	.word	0x00016bd9
    2f1c:	00016be4 	.word	0x00016be4

00002f20 <find_cmd_tbl>:
    2f20:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    2f24:	e1a05001 	mov	r5, r1
    2f28:	e24dd004 	sub	sp, sp, #4	; 0x4
    2f2c:	e3a0102e 	mov	r1, #46	; 0x2e
    2f30:	e1a07000 	mov	r7, r0
    2f34:	e1a08002 	mov	r8, r2
    2f38:	ebfff962 	bl	14c8 <strchr>
    2f3c:	e3500000 	cmp	r0, #0	; 0x0
    2f40:	10676000 	rsbne	r6, r7, r0
    2f44:	1a000002 	bne	2f54 <find_cmd_tbl+0x34>
    2f48:	e1a00007 	mov	r0, r7
    2f4c:	ebfff968 	bl	14f4 <strlen>
    2f50:	e1a06000 	mov	r6, r0
    2f54:	e3a03018 	mov	r3, #24	; 0x18
    2f58:	e02a5893 	mla	sl, r3, r8, r5
    2f5c:	e1a04005 	mov	r4, r5
    2f60:	e1a08005 	mov	r8, r5
    2f64:	e3a05000 	mov	r5, #0	; 0x0
    2f68:	ea00000a 	b	2f98 <find_cmd_tbl+0x78>
    2f6c:	e5941000 	ldr	r1, [r4]
    2f70:	ebfff940 	bl	1478 <strncmp>
    2f74:	e3500000 	cmp	r0, #0	; 0x0
    2f78:	1a000005 	bne	2f94 <find_cmd_tbl+0x74>
    2f7c:	e5940000 	ldr	r0, [r4]
    2f80:	ebfff95b 	bl	14f4 <strlen>
    2f84:	e1560000 	cmp	r6, r0
    2f88:	0a000009 	beq	2fb4 <find_cmd_tbl+0x94>
    2f8c:	e2855001 	add	r5, r5, #1	; 0x1
    2f90:	e1a08004 	mov	r8, r4
    2f94:	e2844018 	add	r4, r4, #24	; 0x18
    2f98:	e154000a 	cmp	r4, sl
    2f9c:	e1a02006 	mov	r2, r6
    2fa0:	e1a00007 	mov	r0, r7
    2fa4:	1afffff0 	bne	2f6c <find_cmd_tbl+0x4c>
    2fa8:	e3550001 	cmp	r5, #1	; 0x1
    2fac:	01a04008 	moveq	r4, r8
    2fb0:	13a04000 	movne	r4, #0	; 0x0
    2fb4:	e1a00004 	mov	r0, r4
    2fb8:	e28dd004 	add	sp, sp, #4	; 0x4
    2fbc:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

00002fc0 <find_cmd>:
    2fc0:	e59f1014 	ldr	r1, [pc, #20]	; 2fdc <find_cmd+0x1c>
    2fc4:	e59f2014 	ldr	r2, [pc, #20]	; 2fe0 <find_cmd+0x20>
    2fc8:	e59f3014 	ldr	r3, [pc, #20]	; 2fe4 <find_cmd+0x24>
    2fcc:	e0612002 	rsb	r2, r1, r2
    2fd0:	e1a021c2 	asr	r2, r2, #3
    2fd4:	e0020293 	mul	r2, r3, r2
    2fd8:	eaffffd0 	b	2f20 <find_cmd_tbl>
    2fdc:	00018ec4 	.word	0x00018ec4
    2fe0:	0001923c 	.word	0x0001923c
    2fe4:	aaaaaaab 	.word	0xaaaaaaab

00002fe8 <_do_help>:
    2fe8:	e1a0c00d 	mov	ip, sp
    2fec:	e92ddff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}
    2ff0:	e24cb004 	sub	fp, ip, #4	; 0x4
    2ff4:	e24dd004 	sub	sp, sp, #4	; 0x4
    2ff8:	e59b8004 	ldr	r8, [fp, #4]
    2ffc:	e1a07000 	mov	r7, r0
    3000:	e3580001 	cmp	r8, #1	; 0x1
    3004:	e1a0a001 	mov	sl, r1
    3008:	159b5008 	ldrne	r5, [fp, #8]
    300c:	13a06001 	movne	r6, #1	; 0x1
    3010:	13a04000 	movne	r4, #0	; 0x0
    3014:	159f9158 	ldrne	r9, [pc, #344]	; 3174 <_do_help+0x18c>
    3018:	1a00004e 	bne	3158 <_do_help+0x170>
    301c:	e1a03101 	lsl	r3, r1, #2
    3020:	e283300e 	add	r3, r3, #14	; 0xe
    3024:	e3c33007 	bic	r3, r3, #7	; 0x7
    3028:	e1a0900d 	mov	r9, sp
    302c:	e063d00d 	rsb	sp, r3, sp
    3030:	e1a0800d 	mov	r8, sp
    3034:	e3a03000 	mov	r3, #0	; 0x0
    3038:	ea000002 	b	3048 <_do_help+0x60>
    303c:	e7880103 	str	r0, [r8, r3, lsl #2]
    3040:	e2800018 	add	r0, r0, #24	; 0x18
    3044:	e2833001 	add	r3, r3, #1	; 0x1
    3048:	e153000a 	cmp	r3, sl
    304c:	bafffffa 	blt	303c <_do_help+0x54>
    3050:	e24a7001 	sub	r7, sl, #1	; 0x1
    3054:	ea000015 	b	30b0 <_do_help+0xc8>
    3058:	e5943000 	ldr	r3, [r4]
    305c:	e5942004 	ldr	r2, [r4, #4]
    3060:	e5930000 	ldr	r0, [r3]
    3064:	e5921000 	ldr	r1, [r2]
    3068:	ebfff8f4 	bl	1440 <strcmp>
    306c:	e3500000 	cmp	r0, #0	; 0x0
    3070:	c5942000 	ldrgt	r2, [r4]
    3074:	c5943004 	ldrgt	r3, [r4, #4]
    3078:	e2855001 	add	r5, r5, #1	; 0x1
    307c:	c5843000 	strgt	r3, [r4]
    3080:	c5842004 	strgt	r2, [r4, #4]
    3084:	c2866001 	addgt	r6, r6, #1	; 0x1
    3088:	e1550007 	cmp	r5, r7
    308c:	e2844004 	add	r4, r4, #4	; 0x4
    3090:	bafffff0 	blt	3058 <_do_help+0x70>
    3094:	e3560000 	cmp	r6, #0	; 0x0
    3098:	1a000003 	bne	30ac <_do_help+0xc4>
    309c:	e3a05000 	mov	r5, #0	; 0x0
    30a0:	e1a04005 	mov	r4, r5
    30a4:	e59f70c8 	ldr	r7, [pc, #200]	; 3174 <_do_help+0x18c>
    30a8:	ea000016 	b	3108 <_do_help+0x120>
    30ac:	e2477001 	sub	r7, r7, #1	; 0x1
    30b0:	e3570000 	cmp	r7, #0	; 0x0
    30b4:	dafffff8 	ble	309c <_do_help+0xb4>
    30b8:	e3a05000 	mov	r5, #0	; 0x0
    30bc:	e1a04008 	mov	r4, r8
    30c0:	e1a06005 	mov	r6, r5
    30c4:	eaffffe3 	b	3058 <_do_help+0x70>
    30c8:	e7983004 	ldr	r3, [r8, r4]
    30cc:	e5936010 	ldr	r6, [r3, #16]
    30d0:	eb003dc4 	bl	127e8 <ctrlc>
    30d4:	e3500000 	cmp	r0, #0	; 0x0
    30d8:	e1a03006 	mov	r3, r6
    30dc:	e3a01008 	mov	r1, #8	; 0x8
    30e0:	e59f0090 	ldr	r0, [pc, #144]	; 3178 <_do_help+0x190>
    30e4:	13a04001 	movne	r4, #1	; 0x1
    30e8:	1a00000a 	bne	3118 <_do_help+0x130>
    30ec:	e3560000 	cmp	r6, #0	; 0x0
    30f0:	0a000003 	beq	3104 <_do_help+0x11c>
    30f4:	e7982004 	ldr	r2, [r8, r4]
    30f8:	e5922000 	ldr	r2, [r2]
    30fc:	e1a0e00f 	mov	lr, pc
    3100:	e597f000 	ldr	pc, [r7]
    3104:	e2844004 	add	r4, r4, #4	; 0x4
    3108:	e155000a 	cmp	r5, sl
    310c:	e2855001 	add	r5, r5, #1	; 0x1
    3110:	baffffec 	blt	30c8 <_do_help+0xe0>
    3114:	e3a04000 	mov	r4, #0	; 0x0
    3118:	e1a0d009 	mov	sp, r9
    311c:	ea000011 	b	3168 <_do_help+0x180>
    3120:	e5950004 	ldr	r0, [r5, #4]
    3124:	ebffff7d 	bl	2f20 <find_cmd_tbl>
    3128:	e3500000 	cmp	r0, #0	; 0x0
    312c:	0a000002 	beq	313c <_do_help+0x154>
    3130:	ebffff5c 	bl	2ea8 <cmd_usage>
    3134:	e1844000 	orr	r4, r4, r0
    3138:	ea000004 	b	3150 <_do_help+0x168>
    313c:	e5951004 	ldr	r1, [r5, #4]
    3140:	e59f0034 	ldr	r0, [pc, #52]	; 317c <_do_help+0x194>
    3144:	e1a0e00f 	mov	lr, pc
    3148:	e599f000 	ldr	pc, [r9]
    314c:	e3a04001 	mov	r4, #1	; 0x1
    3150:	e2866001 	add	r6, r6, #1	; 0x1
    3154:	e2855004 	add	r5, r5, #4	; 0x4
    3158:	e1560008 	cmp	r6, r8
    315c:	e1a01007 	mov	r1, r7
    3160:	e1a0200a 	mov	r2, sl
    3164:	baffffed 	blt	3120 <_do_help+0x138>
    3168:	e1a00004 	mov	r0, r4
    316c:	e24bd028 	sub	sp, fp, #40	; 0x28
    3170:	e89daff0 	ldm	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
    3174:	3c00d6cc 	.word	0x3c00d6cc
    3178:	00016c05 	.word	0x00016c05
    317c:	00016c0f 	.word	0x00016c0f

00003180 <do_help>:
    3180:	e92d4030 	push	{r4, r5, lr}
    3184:	e59fc038 	ldr	ip, [pc, #56]	; 31c4 <do_help+0x44>
    3188:	e59fe038 	ldr	lr, [pc, #56]	; 31c8 <do_help+0x48>
    318c:	e1a05001 	mov	r5, r1
    3190:	e06ec00c 	rsb	ip, lr, ip
    3194:	e59f1030 	ldr	r1, [pc, #48]	; 31cc <do_help+0x4c>
    3198:	e24dd00c 	sub	sp, sp, #12	; 0xc
    319c:	e1a04000 	mov	r4, r0
    31a0:	e1a0c1cc 	asr	ip, ip, #3
    31a4:	e88d000c 	stm	sp, {r2, r3}
    31a8:	e0010c91 	mul	r1, r1, ip
    31ac:	e1a0000e 	mov	r0, lr
    31b0:	e1a02004 	mov	r2, r4
    31b4:	e1a03005 	mov	r3, r5
    31b8:	ebffff8a 	bl	2fe8 <_do_help>
    31bc:	e28dd00c 	add	sp, sp, #12	; 0xc
    31c0:	e8bd8030 	pop	{r4, r5, pc}
    31c4:	0001923c 	.word	0x0001923c
    31c8:	00018ec4 	.word	0x00018ec4
    31cc:	aaaaaaab 	.word	0xaaaaaaab

000031d0 <tt_blk_trolling>:
    31d0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    31d4:	e3a01001 	mov	r1, #1	; 0x1
    31d8:	e24dd02c 	sub	sp, sp, #44	; 0x2c
    31dc:	e1a04002 	mov	r4, r2
    31e0:	e1a05003 	mov	r5, r3
    31e4:	e58d0010 	str	r0, [sp, #16]
    31e8:	e59da050 	ldr	sl, [sp, #80]
    31ec:	eb000cda 	bl	655c <vs_assign_by_name>
    31f0:	e3500000 	cmp	r0, #0	; 0x0
    31f4:	0a000005 	beq	3210 <tt_blk_trolling+0x40>
    31f8:	e59d1010 	ldr	r1, [sp, #16]
    31fc:	e59f03d4 	ldr	r0, [pc, #980]	; 35d8 <tt_blk_trolling+0x408>
    3200:	e59f33d4 	ldr	r3, [pc, #980]	; 35dc <tt_blk_trolling+0x40c>
    3204:	e1a0e00f 	mov	lr, pc
    3208:	e593f000 	ldr	pc, [r3]
    320c:	ea000026 	b	32ac <tt_blk_trolling+0xdc>
    3210:	e1940005 	orrs	r0, r4, r5
    3214:	03a04502 	moveq	r4, #8388608	; 0x800000
    3218:	03a06502 	moveq	r6, #8388608	; 0x800000
    321c:	03a07000 	moveq	r7, #0	; 0x0
    3220:	0a00000b 	beq	3254 <tt_blk_trolling+0x84>
    3224:	e3550000 	cmp	r5, #0	; 0x0
    3228:	1a000006 	bne	3248 <tt_blk_trolling+0x78>
    322c:	e3540502 	cmp	r4, #8388608	; 0x800000
    3230:	2a000004 	bcs	3248 <tt_blk_trolling+0x78>
    3234:	e3a0000e 	mov	r0, #14	; 0xe
    3238:	eb000b55 	bl	5f94 <vs_align>
    323c:	e1a06004 	mov	r6, r4
    3240:	e1a07005 	mov	r7, r5
    3244:	ea000002 	b	3254 <tt_blk_trolling+0x84>
    3248:	e1a06004 	mov	r6, r4
    324c:	e1a07005 	mov	r7, r5
    3250:	e3a04502 	mov	r4, #8388608	; 0x800000
    3254:	e3a0000e 	mov	r0, #14	; 0xe
    3258:	eb000b4d 	bl	5f94 <vs_align>
    325c:	e1a01000 	mov	r1, r0
    3260:	e1a00004 	mov	r0, r4
    3264:	eb00454a 	bl	14794 <__aeabi_uidiv>
    3268:	e1a01000 	mov	r1, r0
    326c:	e3a00000 	mov	r0, #0	; 0x0
    3270:	eb00054e 	bl	47b0 <tt_rnd_get_int>
    3274:	e1a04000 	mov	r4, r0
    3278:	e3a0000e 	mov	r0, #14	; 0xe
    327c:	eb000b44 	bl	5f94 <vs_align>
    3280:	e0040094 	mul	r4, r4, r0
    3284:	e28d0024 	add	r0, sp, #36	; 0x24
    3288:	e1a01004 	mov	r1, r4
    328c:	e1a02004 	mov	r2, r4
    3290:	eb00056f 	bl	4854 <tt_rnd_get_data>
    3294:	e250b000 	subs	fp, r0, #0	; 0x0
    3298:	1a000005 	bne	32b4 <tt_blk_trolling+0xe4>
    329c:	e59f033c 	ldr	r0, [pc, #828]	; 35e0 <tt_blk_trolling+0x410>
    32a0:	e59f3334 	ldr	r3, [pc, #820]	; 35dc <tt_blk_trolling+0x40c>
    32a4:	e1a0e00f 	mov	lr, pc
    32a8:	e593f000 	ldr	pc, [r3]
    32ac:	e3e00000 	mvn	r0, #0	; 0x0
    32b0:	ea0000c6 	b	35d0 <tt_blk_trolling+0x400>
    32b4:	e59d3024 	ldr	r3, [sp, #36]
    32b8:	e3a0000e 	mov	r0, #14	; 0xe
    32bc:	e0564003 	subs	r4, r6, r3
    32c0:	e0c75fc3 	sbc	r5, r7, r3, asr #31
    32c4:	eb000b32 	bl	5f94 <vs_align>
    32c8:	e3550000 	cmp	r5, #0	; 0x0
    32cc:	e1cd41f8 	strd	r4, [sp, #24]
    32d0:	1a000005 	bne	32ec <tt_blk_trolling+0x11c>
    32d4:	e1a01000 	mov	r1, r0
    32d8:	e1a00004 	mov	r0, r4
    32dc:	eb00452c 	bl	14794 <__aeabi_uidiv>
    32e0:	e58d501c 	str	r5, [sp, #28]
    32e4:	e58d0018 	str	r0, [sp, #24]
    32e8:	ea000002 	b	32f8 <tt_blk_trolling+0x128>
    32ec:	e1a01000 	mov	r1, r0
    32f0:	e28d0018 	add	r0, sp, #24	; 0x18
    32f4:	eb0038e5 	bl	11690 <__div64_32>
    32f8:	e59d1018 	ldr	r1, [sp, #24]
    32fc:	e3a00000 	mov	r0, #0	; 0x0
    3300:	eb00052a 	bl	47b0 <tt_rnd_get_int>
    3304:	e1a04000 	mov	r4, r0
    3308:	e3a0000e 	mov	r0, #14	; 0xe
    330c:	eb000b20 	bl	5f94 <vs_align>
    3310:	e0c98490 	smull	r8, r9, r0, r4
    3314:	e59d1010 	ldr	r1, [sp, #16]
    3318:	e1a0200b 	mov	r2, fp
    331c:	e59d3024 	ldr	r3, [sp, #36]
    3320:	e59f02bc 	ldr	r0, [pc, #700]	; 35e4 <tt_blk_trolling+0x414>
    3324:	e88d0300 	stm	sp, {r8, r9}
    3328:	e59f42ac 	ldr	r4, [pc, #684]	; 35dc <tt_blk_trolling+0x40c>
    332c:	e1a0e00f 	mov	lr, pc
    3330:	e594f000 	ldr	pc, [r4]
    3334:	e59d0010 	ldr	r0, [sp, #16]
    3338:	eb000b96 	bl	6198 <vs_device_id>
    333c:	eb000b81 	bl	6148 <vs_device_erasable>
    3340:	e3500000 	cmp	r0, #0	; 0x0
    3344:	0a000006 	beq	3364 <tt_blk_trolling+0x194>
    3348:	e59f0298 	ldr	r0, [pc, #664]	; 35e8 <tt_blk_trolling+0x418>
    334c:	e1a0e00f 	mov	lr, pc
    3350:	e594f000 	ldr	pc, [r4]
    3354:	e1a00008 	mov	r0, r8
    3358:	e1a01009 	mov	r1, r9
    335c:	e59d2024 	ldr	r2, [sp, #36]
    3360:	eb000acf 	bl	5ea4 <vs_erase>
    3364:	e59f0280 	ldr	r0, [pc, #640]	; 35ec <tt_blk_trolling+0x41c>
    3368:	e59f326c 	ldr	r3, [pc, #620]	; 35dc <tt_blk_trolling+0x40c>
    336c:	e1a0e00f 	mov	lr, pc
    3370:	e593f000 	ldr	pc, [r3]
    3374:	e35a0000 	cmp	sl, #0	; 0x0
    3378:	13a07000 	movne	r7, #0	; 0x0
    337c:	159d6024 	ldrne	r6, [sp, #36]
    3380:	1a00001c 	bne	33f8 <tt_blk_trolling+0x228>
    3384:	ea00001f 	b	3408 <tt_blk_trolling+0x238>
    3388:	e3560902 	cmp	r6, #32768	; 0x8000
    338c:	b1a01006 	movlt	r1, r6
    3390:	a3a01902 	movge	r1, #32768	; 0x8000
    3394:	e58d100c 	str	r1, [sp, #12]
    3398:	e59d1024 	ldr	r1, [sp, #36]
    339c:	e59d200c 	ldr	r2, [sp, #12]
    33a0:	e0661001 	rsb	r1, r6, r1
    33a4:	e08b1001 	add	r1, fp, r1
    33a8:	ebfff86b 	bl	155c <memcpy>
    33ac:	e3a00000 	mov	r0, #0	; 0x0
    33b0:	eb0029af 	bl	da74 <get_timer>
    33b4:	e59d3024 	ldr	r3, [sp, #36]
    33b8:	e3a01000 	mov	r1, #0	; 0x0
    33bc:	e0663003 	rsb	r3, r6, r3
    33c0:	e0984003 	adds	r4, r8, r3
    33c4:	e0a95fc3 	adc	r5, r9, r3, asr #31
    33c8:	e1a03005 	mov	r3, r5
    33cc:	e59d500c 	ldr	r5, [sp, #12]
    33d0:	e58d0008 	str	r0, [sp, #8]
    33d4:	e1a02004 	mov	r2, r4
    33d8:	e1a0000a 	mov	r0, sl
    33dc:	e58d1004 	str	r1, [sp, #4]
    33e0:	e58d5000 	str	r5, [sp]
    33e4:	eb000a98 	bl	5e4c <vs_write>
    33e8:	e59d0008 	ldr	r0, [sp, #8]
    33ec:	eb0029a0 	bl	da74 <get_timer>
    33f0:	e0656006 	rsb	r6, r5, r6
    33f4:	e0877000 	add	r7, r7, r0
    33f8:	e3560000 	cmp	r6, #0	; 0x0
    33fc:	e1a0000a 	mov	r0, sl
    3400:	1affffe0 	bne	3388 <tt_blk_trolling+0x1b8>
    3404:	ea00000b 	b	3438 <tt_blk_trolling+0x268>
    3408:	e1a0000a 	mov	r0, sl
    340c:	eb002998 	bl	da74 <get_timer>
    3410:	e59d1024 	ldr	r1, [sp, #36]
    3414:	e1a04000 	mov	r4, r0
    3418:	e1a02008 	mov	r2, r8
    341c:	e1a03009 	mov	r3, r9
    3420:	e1a0000b 	mov	r0, fp
    3424:	e88d0402 	stm	sp, {r1, sl}
    3428:	eb000a87 	bl	5e4c <vs_write>
    342c:	e1a00004 	mov	r0, r4
    3430:	eb00298f 	bl	da74 <get_timer>
    3434:	e1a07000 	mov	r7, r0
    3438:	e59d0024 	ldr	r0, [sp, #36]
    343c:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    3440:	e1a00540 	asr	r0, r0, #10
    3444:	e1a01007 	mov	r1, r7
    3448:	e0000093 	mul	r0, r3, r0
    344c:	eb0044d0 	bl	14794 <__aeabi_uidiv>
    3450:	e59f3184 	ldr	r3, [pc, #388]	; 35dc <tt_blk_trolling+0x40c>
    3454:	e58d0014 	str	r0, [sp, #20]
    3458:	e59f0190 	ldr	r0, [pc, #400]	; 35f0 <tt_blk_trolling+0x420>
    345c:	e1a0e00f 	mov	lr, pc
    3460:	e593f000 	ldr	pc, [r3]
    3464:	e35a0000 	cmp	sl, #0	; 0x0
    3468:	13a07000 	movne	r7, #0	; 0x0
    346c:	159d6024 	ldrne	r6, [sp, #36]
    3470:	1a00001b 	bne	34e4 <tt_blk_trolling+0x314>
    3474:	ea00001e 	b	34f4 <tt_blk_trolling+0x324>
    3478:	eb00297d 	bl	da74 <get_timer>
    347c:	e59d3024 	ldr	r3, [sp, #36]
    3480:	e3560902 	cmp	r6, #32768	; 0x8000
    3484:	b1a04006 	movlt	r4, r6
    3488:	a3a04902 	movge	r4, #32768	; 0x8000
    348c:	e0663003 	rsb	r3, r6, r3
    3490:	e1a05000 	mov	r5, r0
    3494:	e0980003 	adds	r0, r8, r3
    3498:	e0a91fc3 	adc	r1, r9, r3, asr #31
    349c:	e1a02000 	mov	r2, r0
    34a0:	e1a03001 	mov	r3, r1
    34a4:	e1a0000a 	mov	r0, sl
    34a8:	e3a01000 	mov	r1, #0	; 0x0
    34ac:	e58d1004 	str	r1, [sp, #4]
    34b0:	e58d4000 	str	r4, [sp]
    34b4:	eb000b64 	bl	624c <vs_read>
    34b8:	e1a00005 	mov	r0, r5
    34bc:	eb00296c 	bl	da74 <get_timer>
    34c0:	e1a05000 	mov	r5, r0
    34c4:	e59d0024 	ldr	r0, [sp, #36]
    34c8:	e1a0100a 	mov	r1, sl
    34cc:	e0660000 	rsb	r0, r6, r0
    34d0:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    34d4:	e1a02004 	mov	r2, r4
    34d8:	ebfff81f 	bl	155c <memcpy>
    34dc:	e0877005 	add	r7, r7, r5
    34e0:	e0646006 	rsb	r6, r4, r6
    34e4:	e3a00000 	mov	r0, #0	; 0x0
    34e8:	e1560000 	cmp	r6, r0
    34ec:	1affffe1 	bne	3478 <tt_blk_trolling+0x2a8>
    34f0:	ea00000b 	b	3524 <tt_blk_trolling+0x354>
    34f4:	e1a0000a 	mov	r0, sl
    34f8:	eb00295d 	bl	da74 <get_timer>
    34fc:	e59d1024 	ldr	r1, [sp, #36]
    3500:	e1a04000 	mov	r4, r0
    3504:	e1a02008 	mov	r2, r8
    3508:	e1a03009 	mov	r3, r9
    350c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3510:	e88d0402 	stm	sp, {r1, sl}
    3514:	eb000b4c 	bl	624c <vs_read>
    3518:	e1a00004 	mov	r0, r4
    351c:	eb002954 	bl	da74 <get_timer>
    3520:	e1a07000 	mov	r7, r0
    3524:	e59d0024 	ldr	r0, [sp, #36]
    3528:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    352c:	e1a00540 	asr	r0, r0, #10
    3530:	e0000093 	mul	r0, r3, r0
    3534:	e1a01007 	mov	r1, r7
    3538:	eb004495 	bl	14794 <__aeabi_uidiv>
    353c:	e59f4098 	ldr	r4, [pc, #152]	; 35dc <tt_blk_trolling+0x40c>
    3540:	e1a05000 	mov	r5, r0
    3544:	e59f00a8 	ldr	r0, [pc, #168]	; 35f4 <tt_blk_trolling+0x424>
    3548:	e1a0e00f 	mov	lr, pc
    354c:	e594f000 	ldr	pc, [r4]
    3550:	e1a0000b 	mov	r0, fp
    3554:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    3558:	e59d2024 	ldr	r2, [sp, #36]
    355c:	e3a0300c 	mov	r3, #12	; 0xc
    3560:	eb00043e 	bl	4660 <tt_cmp_buffer>
    3564:	e3500000 	cmp	r0, #0	; 0x0
    3568:	0a000007 	beq	358c <tt_blk_trolling+0x3bc>
    356c:	e58d0000 	str	r0, [sp]
    3570:	e59d1010 	ldr	r1, [sp, #16]
    3574:	e59d2014 	ldr	r2, [sp, #20]
    3578:	e1a03005 	mov	r3, r5
    357c:	e59f0074 	ldr	r0, [pc, #116]	; 35f8 <tt_blk_trolling+0x428>
    3580:	e1a0e00f 	mov	lr, pc
    3584:	e594f000 	ldr	pc, [r4]
    3588:	ea000005 	b	35a4 <tt_blk_trolling+0x3d4>
    358c:	e59d1010 	ldr	r1, [sp, #16]
    3590:	e59d2014 	ldr	r2, [sp, #20]
    3594:	e1a03005 	mov	r3, r5
    3598:	e59f005c 	ldr	r0, [pc, #92]	; 35fc <tt_blk_trolling+0x42c>
    359c:	e1a0e00f 	mov	lr, pc
    35a0:	e594f000 	ldr	pc, [r4]
    35a4:	e59f0054 	ldr	r0, [pc, #84]	; 3600 <tt_blk_trolling+0x430>
    35a8:	e59d1024 	ldr	r1, [sp, #36]
    35ac:	e890000c 	ldm	r0, {r2, r3}
    35b0:	e0922001 	adds	r2, r2, r1
    35b4:	e0a33fc1 	adc	r3, r3, r1, asr #31
    35b8:	e880000c 	stm	r0, {r2, r3}
    35bc:	e59f0040 	ldr	r0, [pc, #64]	; 3604 <tt_blk_trolling+0x434>
    35c0:	e59f3014 	ldr	r3, [pc, #20]	; 35dc <tt_blk_trolling+0x40c>
    35c4:	e1a0e00f 	mov	lr, pc
    35c8:	e593f000 	ldr	pc, [r3]
    35cc:	e3a00000 	mov	r0, #0	; 0x0
    35d0:	e28dd02c 	add	sp, sp, #44	; 0x2c
    35d4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    35d8:	00016db6 	.word	0x00016db6
    35dc:	3c00d6cc 	.word	0x3c00d6cc
    35e0:	00016dd4 	.word	0x00016dd4
    35e4:	00016df4 	.word	0x00016df4
    35e8:	00018a4c 	.word	0x00018a4c
    35ec:	000165a4 	.word	0x000165a4
    35f0:	00017906 	.word	0x00017906
    35f4:	00016e28 	.word	0x00016e28
    35f8:	00016e2b 	.word	0x00016e2b
    35fc:	00016e65 	.word	0x00016e65
    3600:	3c00d138 	.word	0x3c00d138
    3604:	00016bd7 	.word	0x00016bd7

00003608 <do_tt_blk>:
    3608:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    360c:	e3520001 	cmp	r2, #1	; 0x1
    3610:	e24dd00c 	sub	sp, sp, #12	; 0xc
    3614:	e1a05002 	mov	r5, r2
    3618:	e1a07003 	mov	r7, r3
    361c:	ca000005 	bgt	3638 <do_tt_blk+0x30>
    3620:	e59f01bc 	ldr	r0, [pc, #444]	; 37e4 <do_tt_blk+0x1dc>
    3624:	e59f31bc 	ldr	r3, [pc, #444]	; 37e8 <do_tt_blk+0x1e0>
    3628:	e1a0e00f 	mov	lr, pc
    362c:	e593f000 	ldr	pc, [r3]
    3630:	e3e00000 	mvn	r0, #0	; 0x0
    3634:	ea000068 	b	37dc <do_tt_blk+0x1d4>
    3638:	e3520002 	cmp	r2, #2	; 0x2
    363c:	03a04000 	moveq	r4, #0	; 0x0
    3640:	01a06004 	moveq	r6, r4
    3644:	0a000025 	beq	36e0 <do_tt_blk+0xd8>
    3648:	e3a01000 	mov	r1, #0	; 0x0
    364c:	e3a0200a 	mov	r2, #10	; 0xa
    3650:	e5930008 	ldr	r0, [r3, #8]
    3654:	eb00384a 	bl	11784 <simple_strtoul>
    3658:	e59f118c 	ldr	r1, [pc, #396]	; 37ec <do_tt_blk+0x1e4>
    365c:	e1a04000 	mov	r4, r0
    3660:	e3a02005 	mov	r2, #5	; 0x5
    3664:	e5970004 	ldr	r0, [r7, #4]
    3668:	ebfff782 	bl	1478 <strncmp>
    366c:	e3500000 	cmp	r0, #0	; 0x0
    3670:	059f0178 	ldreq	r0, [pc, #376]	; 37f0 <do_tt_blk+0x1e8>
    3674:	01a01004 	moveq	r1, r4
    3678:	059f3168 	ldreq	r3, [pc, #360]	; 37e8 <do_tt_blk+0x1e0>
    367c:	0a000008 	beq	36a4 <do_tt_blk+0x9c>
    3680:	e59f116c 	ldr	r1, [pc, #364]	; 37f4 <do_tt_blk+0x1ec>
    3684:	e5970004 	ldr	r0, [r7, #4]
    3688:	e3a02003 	mov	r2, #3	; 0x3
    368c:	ebfff779 	bl	1478 <strncmp>
    3690:	e3500000 	cmp	r0, #0	; 0x0
    3694:	e59f314c 	ldr	r3, [pc, #332]	; 37e8 <do_tt_blk+0x1e0>
    3698:	059f0150 	ldreq	r0, [pc, #336]	; 37f0 <do_tt_blk+0x1e8>
    369c:	159f0154 	ldrne	r0, [pc, #340]	; 37f8 <do_tt_blk+0x1f0>
    36a0:	e1a01004 	mov	r1, r4
    36a4:	e1a0e00f 	mov	lr, pc
    36a8:	e593f000 	ldr	pc, [r3]
    36ac:	e3550003 	cmp	r5, #3	; 0x3
    36b0:	03a06000 	moveq	r6, #0	; 0x0
    36b4:	0a000009 	beq	36e0 <do_tt_blk+0xd8>
    36b8:	e3a01000 	mov	r1, #0	; 0x0
    36bc:	e597000c 	ldr	r0, [r7, #12]
    36c0:	e3a02010 	mov	r2, #16	; 0x10
    36c4:	eb00382e 	bl	11784 <simple_strtoul>
    36c8:	e1a06000 	mov	r6, r0
    36cc:	e1a01006 	mov	r1, r6
    36d0:	e59f0124 	ldr	r0, [pc, #292]	; 37fc <do_tt_blk+0x1f4>
    36d4:	e59f310c 	ldr	r3, [pc, #268]	; 37e8 <do_tt_blk+0x1e0>
    36d8:	e1a0e00f 	mov	lr, pc
    36dc:	e593f000 	ldr	pc, [r3]
    36e0:	e3a00000 	mov	r0, #0	; 0x0
    36e4:	eb0028e2 	bl	da74 <get_timer>
    36e8:	e59f1110 	ldr	r1, [pc, #272]	; 3800 <do_tt_blk+0x1f8>
    36ec:	e3a02000 	mov	r2, #0	; 0x0
    36f0:	e3a03000 	mov	r3, #0	; 0x0
    36f4:	e881000c 	stm	r1, {r2, r3}
    36f8:	e1a09a04 	lsl	r9, r4, #20
    36fc:	e2875004 	add	r5, r7, #4	; 0x4
    3700:	e1a08b24 	lsr	r8, r4, #22
    3704:	e1a07504 	lsl	r7, r4, #10
    3708:	e1a0a624 	lsr	sl, r4, #12
    370c:	e1a0b000 	mov	fp, r0
    3710:	e3a04000 	mov	r4, #0	; 0x0
    3714:	e59f30cc 	ldr	r3, [pc, #204]	; 37e8 <do_tt_blk+0x1e0>
    3718:	e1a01004 	mov	r1, r4
    371c:	e59f00e0 	ldr	r0, [pc, #224]	; 3804 <do_tt_blk+0x1fc>
    3720:	e1a0e00f 	mov	lr, pc
    3724:	e593f000 	ldr	pc, [r3]
    3728:	e59f10bc 	ldr	r1, [pc, #188]	; 37ec <do_tt_blk+0x1e4>
    372c:	e3a02005 	mov	r2, #5	; 0x5
    3730:	e5950000 	ldr	r0, [r5]
    3734:	ebfff74f 	bl	1478 <strncmp>
    3738:	e3500000 	cmp	r0, #0	; 0x0
    373c:	e3a02003 	mov	r2, #3	; 0x3
    3740:	e59f10ac 	ldr	r1, [pc, #172]	; 37f4 <do_tt_blk+0x1ec>
    3744:	e1a03008 	mov	r3, r8
    3748:	05950000 	ldreq	r0, [r5]
    374c:	01a02007 	moveq	r2, r7
    3750:	0a000008 	beq	3778 <do_tt_blk+0x170>
    3754:	e5950000 	ldr	r0, [r5]
    3758:	ebfff746 	bl	1478 <strncmp>
    375c:	e3500000 	cmp	r0, #0	; 0x0
    3760:	05950000 	ldreq	r0, [r5]
    3764:	15950000 	ldrne	r0, [r5]
    3768:	e1a02009 	mov	r2, r9
    376c:	e1a0300a 	mov	r3, sl
    3770:	01a02007 	moveq	r2, r7
    3774:	01a03008 	moveq	r3, r8
    3778:	e58d6000 	str	r6, [sp]
    377c:	ebfffe93 	bl	31d0 <tt_blk_trolling>
    3780:	eb003c16 	bl	127e0 <tstc>
    3784:	e3500000 	cmp	r0, #0	; 0x0
    3788:	e2844001 	add	r4, r4, #1	; 0x1
    378c:	0affffe0 	beq	3714 <do_tt_blk+0x10c>
    3790:	eb003c13 	bl	127e4 <getc>
    3794:	e3500003 	cmp	r0, #3	; 0x3
    3798:	1affffdd 	bne	3714 <do_tt_blk+0x10c>
    379c:	e1a0000b 	mov	r0, fp
    37a0:	eb0028b3 	bl	da74 <get_timer>
    37a4:	e59f3054 	ldr	r3, [pc, #84]	; 3800 <do_tt_blk+0x1f8>
    37a8:	e30e1a60 	movw	r1, #60000	; 0xea60
    37ac:	e5934000 	ldr	r4, [r3]
    37b0:	e5933004 	ldr	r3, [r3, #4]
    37b4:	e1a04a24 	lsr	r4, r4, #20
    37b8:	e1844603 	orr	r4, r4, r3, lsl #12
    37bc:	eb0043f4 	bl	14794 <__aeabi_uidiv>
    37c0:	e1a01004 	mov	r1, r4
    37c4:	e1a02000 	mov	r2, r0
    37c8:	e59f3018 	ldr	r3, [pc, #24]	; 37e8 <do_tt_blk+0x1e0>
    37cc:	e59f0034 	ldr	r0, [pc, #52]	; 3808 <do_tt_blk+0x200>
    37d0:	e1a0e00f 	mov	lr, pc
    37d4:	e593f000 	ldr	pc, [r3]
    37d8:	e3a00000 	mov	r0, #0	; 0x0
    37dc:	e28dd00c 	add	sp, sp, #12	; 0xc
    37e0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    37e4:	00016e92 	.word	0x00016e92
    37e8:	3c00d6cc 	.word	0x3c00d6cc
    37ec:	00016ed9 	.word	0x00016ed9
    37f0:	00016edf 	.word	0x00016edf
    37f4:	00016894 	.word	0x00016894
    37f8:	00016f03 	.word	0x00016f03
    37fc:	00016f27 	.word	0x00016f27
    3800:	3c00d138 	.word	0x3c00d138
    3804:	00016f4a 	.word	0x00016f4a
    3808:	00016f54 	.word	0x00016f54

0000380c <__show_strbuf>:
    380c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    3810:	e59f4058 	ldr	r4, [pc, #88]	; 3870 <__show_strbuf+0x64>
    3814:	e24dd004 	sub	sp, sp, #4	; 0x4
    3818:	e1a06001 	mov	r6, r1
    381c:	e1a01000 	mov	r1, r0
    3820:	e59f004c 	ldr	r0, [pc, #76]	; 3874 <__show_strbuf+0x68>
    3824:	e1a05002 	mov	r5, r2
    3828:	e1a07004 	mov	r7, r4
    382c:	e1a0e00f 	mov	lr, pc
    3830:	e594f000 	ldr	pc, [r4]
    3834:	e3a04000 	mov	r4, #0	; 0x0
    3838:	ea000003 	b	384c <__show_strbuf+0x40>
    383c:	e7d61004 	ldrb	r1, [r6, r4]
    3840:	e1a0e00f 	mov	lr, pc
    3844:	e597f000 	ldr	pc, [r7]
    3848:	e2844001 	add	r4, r4, #1	; 0x1
    384c:	e1540005 	cmp	r4, r5
    3850:	e59f0020 	ldr	r0, [pc, #32]	; 3878 <__show_strbuf+0x6c>
    3854:	bafffff8 	blt	383c <__show_strbuf+0x30>
    3858:	e59f001c 	ldr	r0, [pc, #28]	; 387c <__show_strbuf+0x70>
    385c:	e59f300c 	ldr	r3, [pc, #12]	; 3870 <__show_strbuf+0x64>
    3860:	e1a0e00f 	mov	lr, pc
    3864:	e593f000 	ldr	pc, [r3]
    3868:	e28dd004 	add	sp, sp, #4	; 0x4
    386c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    3870:	3c00d6cc 	.word	0x3c00d6cc
    3874:	00016fa2 	.word	0x00016fa2
    3878:	00018c05 	.word	0x00018c05
    387c:	00016bd7 	.word	0x00016bd7

00003880 <do_crypto_quick>:
    3880:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    3884:	e24dd0f0 	sub	sp, sp, #240	; 0xf0
    3888:	e28da0e0 	add	sl, sp, #224	; 0xe0
    388c:	e3a04010 	mov	r4, #16	; 0x10
    3890:	e28d70c0 	add	r7, sp, #192	; 0xc0
    3894:	e1a02004 	mov	r2, r4
    3898:	e3a09020 	mov	r9, #32	; 0x20
    389c:	e59f11f0 	ldr	r1, [pc, #496]	; 3a94 <do_crypto_quick+0x214>
    38a0:	e1a0000a 	mov	r0, sl
    38a4:	e3a05040 	mov	r5, #64	; 0x40
    38a8:	e28d6080 	add	r6, sp, #128	; 0x80
    38ac:	ebfff72a 	bl	155c <memcpy>
    38b0:	e1a02009 	mov	r2, r9
    38b4:	e59f11dc 	ldr	r1, [pc, #476]	; 3a98 <do_crypto_quick+0x218>
    38b8:	e1a00007 	mov	r0, r7
    38bc:	ebfff726 	bl	155c <memcpy>
    38c0:	e08d8005 	add	r8, sp, r5
    38c4:	e1a02005 	mov	r2, r5
    38c8:	e59f11cc 	ldr	r1, [pc, #460]	; 3a9c <do_crypto_quick+0x21c>
    38cc:	e1a00006 	mov	r0, r6
    38d0:	ebfff721 	bl	155c <memcpy>
    38d4:	e1a02005 	mov	r2, r5
    38d8:	e59f11c0 	ldr	r1, [pc, #448]	; 3aa0 <do_crypto_quick+0x220>
    38dc:	e1a00008 	mov	r0, r8
    38e0:	ebfff71d 	bl	155c <memcpy>
    38e4:	e1a02004 	mov	r2, r4
    38e8:	e1a0100a 	mov	r1, sl
    38ec:	e59f01b0 	ldr	r0, [pc, #432]	; 3aa4 <do_crypto_quick+0x224>
    38f0:	ebffffc5 	bl	380c <__show_strbuf>
    38f4:	e1a02009 	mov	r2, r9
    38f8:	e1a01007 	mov	r1, r7
    38fc:	e59f01a4 	ldr	r0, [pc, #420]	; 3aa8 <do_crypto_quick+0x228>
    3900:	ebffffc1 	bl	380c <__show_strbuf>
    3904:	e1a01006 	mov	r1, r6
    3908:	e1a02005 	mov	r2, r5
    390c:	e59f0198 	ldr	r0, [pc, #408]	; 3aac <do_crypto_quick+0x22c>
    3910:	ebffffbd 	bl	380c <__show_strbuf>
    3914:	e1a01008 	mov	r1, r8
    3918:	e1a02005 	mov	r2, r5
    391c:	e59f018c 	ldr	r0, [pc, #396]	; 3ab0 <do_crypto_quick+0x230>
    3920:	ebffffb9 	bl	380c <__show_strbuf>
    3924:	e1a01007 	mov	r1, r7
    3928:	e1a0200a 	mov	r2, sl
    392c:	e3a00001 	mov	r0, #1	; 0x1
    3930:	eb000b4c 	bl	6668 <ss_aes_init>
    3934:	e1a0100d 	mov	r1, sp
    3938:	e1a02005 	mov	r2, r5
    393c:	e1a00006 	mov	r0, r6
    3940:	eb000b37 	bl	6624 <ss_aes_data>
    3944:	e1a0100d 	mov	r1, sp
    3948:	e1a02005 	mov	r2, r5
    394c:	e59f0160 	ldr	r0, [pc, #352]	; 3ab4 <do_crypto_quick+0x234>
    3950:	ebffffad 	bl	380c <__show_strbuf>
    3954:	e1a01007 	mov	r1, r7
    3958:	e1a0200a 	mov	r2, sl
    395c:	e3a00000 	mov	r0, #0	; 0x0
    3960:	eb000b40 	bl	6668 <ss_aes_init>
    3964:	e59f714c 	ldr	r7, [pc, #332]	; 3ab8 <do_crypto_quick+0x238>
    3968:	e1a0100d 	mov	r1, sp
    396c:	e1a02005 	mov	r2, r5
    3970:	e1a00008 	mov	r0, r8
    3974:	eb000b2a 	bl	6624 <ss_aes_data>
    3978:	e1a0100d 	mov	r1, sp
    397c:	e1a02005 	mov	r2, r5
    3980:	e59f0134 	ldr	r0, [pc, #308]	; 3abc <do_crypto_quick+0x23c>
    3984:	ebffffa0 	bl	380c <__show_strbuf>
    3988:	e1a02007 	mov	r2, r7
    398c:	e3a01000 	mov	r1, #0	; 0x0
    3990:	e3a00001 	mov	r0, #1	; 0x1
    3994:	eb000b33 	bl	6668 <ss_aes_init>
    3998:	e1a0100d 	mov	r1, sp
    399c:	e1a02005 	mov	r2, r5
    39a0:	e1a00006 	mov	r0, r6
    39a4:	eb000b1e 	bl	6624 <ss_aes_data>
    39a8:	e1a0100d 	mov	r1, sp
    39ac:	e1a02005 	mov	r2, r5
    39b0:	e59f0108 	ldr	r0, [pc, #264]	; 3ac0 <do_crypto_quick+0x240>
    39b4:	ebffff94 	bl	380c <__show_strbuf>
    39b8:	e3a00000 	mov	r0, #0	; 0x0
    39bc:	e1a02007 	mov	r2, r7
    39c0:	e1a01000 	mov	r1, r0
    39c4:	eb000b27 	bl	6668 <ss_aes_init>
    39c8:	e1a0100d 	mov	r1, sp
    39cc:	e1a02005 	mov	r2, r5
    39d0:	e1a0000d 	mov	r0, sp
    39d4:	eb000b12 	bl	6624 <ss_aes_data>
    39d8:	e1a02005 	mov	r2, r5
    39dc:	e1a0100d 	mov	r1, sp
    39e0:	e59f00dc 	ldr	r0, [pc, #220]	; 3ac4 <do_crypto_quick+0x244>
    39e4:	ebffff88 	bl	380c <__show_strbuf>
    39e8:	e1a01005 	mov	r1, r5
    39ec:	e3a00001 	mov	r0, #1	; 0x1
    39f0:	eb000c3a 	bl	6ae0 <hash_init>
    39f4:	e1a01005 	mov	r1, r5
    39f8:	e1a00006 	mov	r0, r6
    39fc:	eb000c12 	bl	6a4c <hash_data>
    3a00:	e1a0000d 	mov	r0, sp
    3a04:	eb000bf4 	bl	69dc <hash_value>
    3a08:	eb000bec 	bl	69c0 <hash_len>
    3a0c:	e1a0100d 	mov	r1, sp
    3a10:	e1a02000 	mov	r2, r0
    3a14:	e59f00ac 	ldr	r0, [pc, #172]	; 3ac8 <do_crypto_quick+0x248>
    3a18:	ebffff7b 	bl	380c <__show_strbuf>
    3a1c:	e1a01005 	mov	r1, r5
    3a20:	e3a00002 	mov	r0, #2	; 0x2
    3a24:	eb000c2d 	bl	6ae0 <hash_init>
    3a28:	e1a01005 	mov	r1, r5
    3a2c:	e1a00006 	mov	r0, r6
    3a30:	eb000c05 	bl	6a4c <hash_data>
    3a34:	e1a0000d 	mov	r0, sp
    3a38:	eb000be7 	bl	69dc <hash_value>
    3a3c:	eb000bdf 	bl	69c0 <hash_len>
    3a40:	e1a0100d 	mov	r1, sp
    3a44:	e1a02000 	mov	r2, r0
    3a48:	e59f007c 	ldr	r0, [pc, #124]	; 3acc <do_crypto_quick+0x24c>
    3a4c:	ebffff6e 	bl	380c <__show_strbuf>
    3a50:	e1a01005 	mov	r1, r5
    3a54:	e3a00003 	mov	r0, #3	; 0x3
    3a58:	eb000c20 	bl	6ae0 <hash_init>
    3a5c:	e1a01005 	mov	r1, r5
    3a60:	e1a00006 	mov	r0, r6
    3a64:	eb000bf8 	bl	6a4c <hash_data>
    3a68:	e1a0000d 	mov	r0, sp
    3a6c:	eb000bda 	bl	69dc <hash_value>
    3a70:	eb000bd2 	bl	69c0 <hash_len>
    3a74:	e1a0100d 	mov	r1, sp
    3a78:	e1a02000 	mov	r2, r0
    3a7c:	e59f004c 	ldr	r0, [pc, #76]	; 3ad0 <do_crypto_quick+0x250>
    3a80:	ebffff61 	bl	380c <__show_strbuf>
    3a84:	e1a0400d 	mov	r4, sp
    3a88:	e3a00000 	mov	r0, #0	; 0x0
    3a8c:	e28dd0f0 	add	sp, sp, #240	; 0xf0
    3a90:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3a94:	0001555c 	.word	0x0001555c
    3a98:	0001553c 	.word	0x0001553c
    3a9c:	000154fc 	.word	0x000154fc
    3aa0:	000154bc 	.word	0x000154bc
    3aa4:	00016fa7 	.word	0x00016fa7
    3aa8:	00016faa 	.word	0x00016faa
    3aac:	00016fae 	.word	0x00016fae
    3ab0:	00016fb9 	.word	0x00016fb9
    3ab4:	00016fca 	.word	0x00016fca
    3ab8:	3c00d088 	.word	0x3c00d088
    3abc:	00016fdd 	.word	0x00016fdd
    3ac0:	00016fc6 	.word	0x00016fc6
    3ac4:	00016fd9 	.word	0x00016fd9
    3ac8:	00016fec 	.word	0x00016fec
    3acc:	00016ff0 	.word	0x00016ff0
    3ad0:	00016ff5 	.word	0x00016ff5

00003ad4 <tt_crypto_hash>:
    3ad4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    3ad8:	e3a0548e 	mov	r5, #-1912602624	; 0x8e000000
    3adc:	e5951000 	ldr	r1, [r5]
    3ae0:	e59f2220 	ldr	r2, [pc, #544]	; 3d08 <tt_crypto_hash+0x234>
    3ae4:	e24dd038 	sub	sp, sp, #56	; 0x38
    3ae8:	e59f321c 	ldr	r3, [pc, #540]	; 3d0c <tt_crypto_hash+0x238>
    3aec:	e1510002 	cmp	r1, r2
    3af0:	e28dc028 	add	ip, sp, #40	; 0x28
    3af4:	e1a06000 	mov	r6, r0
    3af8:	e893000f 	ldm	r3, {r0, r1, r2, r3}
    3afc:	e88c000f 	stm	ip, {r0, r1, r2, r3}
    3b00:	0a000006 	beq	3b20 <tt_crypto_hash+0x4c>
    3b04:	e59f0204 	ldr	r0, [pc, #516]	; 3d10 <tt_crypto_hash+0x23c>
    3b08:	e59f3204 	ldr	r3, [pc, #516]	; 3d14 <tt_crypto_hash+0x240>
    3b0c:	e1a0e00f 	mov	lr, pc
    3b10:	e593f000 	ldr	pc, [r3]
    3b14:	e59f01fc 	ldr	r0, [pc, #508]	; 3d18 <tt_crypto_hash+0x244>
    3b18:	eb0027b1 	bl	d9e4 <udelay>
    3b1c:	ea000008 	b	3b44 <tt_crypto_hash+0x70>
    3b20:	e5951004 	ldr	r1, [r5, #4]
    3b24:	e3a04000 	mov	r4, #0	; 0x0
    3b28:	e3510003 	cmp	r1, #3	; 0x3
    3b2c:	e5854000 	str	r4, [r5]
    3b30:	e59fc1dc 	ldr	ip, [pc, #476]	; 3d14 <tt_crypto_hash+0x240>
    3b34:	9a000004 	bls	3b4c <tt_crypto_hash+0x78>
    3b38:	e59f01dc 	ldr	r0, [pc, #476]	; 3d1c <tt_crypto_hash+0x248>
    3b3c:	e1a0e00f 	mov	lr, pc
    3b40:	e59cf000 	ldr	pc, [ip]
    3b44:	e3e0c000 	mvn	ip, #0	; 0x0
    3b48:	ea00006b 	b	3cfc <tt_crypto_hash+0x228>
    3b4c:	e28d2038 	add	r2, sp, #56	; 0x38
    3b50:	e0823101 	add	r3, r2, r1, lsl #2
    3b54:	e5131010 	ldr	r1, [r3, #-16]
    3b58:	e5952008 	ldr	r2, [r5, #8]
    3b5c:	e59f01bc 	ldr	r0, [pc, #444]	; 3d20 <tt_crypto_hash+0x24c>
    3b60:	e1a0e00f 	mov	lr, pc
    3b64:	e59cf000 	ldr	pc, [ip]
    3b68:	e9950003 	ldmib	r5, {r0, r1}
    3b6c:	eb000bdb 	bl	6ae0 <hash_init>
    3b70:	eb000b92 	bl	69c0 <hash_len>
    3b74:	e1a03000 	mov	r3, r0
    3b78:	e59f01a4 	ldr	r0, [pc, #420]	; 3d24 <tt_crypto_hash+0x250>
    3b7c:	e3a0c020 	mov	ip, #32	; 0x20
    3b80:	e1a01000 	mov	r1, r0
    3b84:	e3a02001 	mov	r2, #1	; 0x1
    3b88:	e58dc000 	str	ip, [sp]
    3b8c:	eb003664 	bl	11524 <print_buffer>
    3b90:	e3560000 	cmp	r6, #0	; 0x0
    3b94:	0a000022 	beq	3c24 <tt_crypto_hash+0x150>
    3b98:	e1a09005 	mov	r9, r5
    3b9c:	e1a0a004 	mov	sl, r4
    3ba0:	e5955008 	ldr	r5, [r5, #8]
    3ba4:	e2868901 	add	r8, r6, #16384	; 0x4000
    3ba8:	ea000019 	b	3c14 <tt_crypto_hash+0x140>
    3bac:	e5991008 	ldr	r1, [r9, #8]
    3bb0:	e3550902 	cmp	r5, #32768	; 0x8000
    3bb4:	b1a04005 	movlt	r4, r5
    3bb8:	a3a04902 	movge	r4, #32768	; 0x8000
    3bbc:	e0651001 	rsb	r1, r5, r1
    3bc0:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3bc4:	e2811030 	add	r1, r1, #48	; 0x30
    3bc8:	e1a02004 	mov	r2, r4
    3bcc:	ebfff662 	bl	155c <memcpy>
    3bd0:	e3a00000 	mov	r0, #0	; 0x0
    3bd4:	eb0027a6 	bl	da74 <get_timer>
    3bd8:	e3550901 	cmp	r5, #16384	; 0x4000
    3bdc:	b1a01005 	movlt	r1, r5
    3be0:	a3a01901 	movge	r1, #16384	; 0x4000
    3be4:	e1a07000 	mov	r7, r0
    3be8:	e1a00006 	mov	r0, r6
    3bec:	eb000b96 	bl	6a4c <hash_data>
    3bf0:	e3540902 	cmp	r4, #32768	; 0x8000
    3bf4:	e0645005 	rsb	r5, r4, r5
    3bf8:	1a000002 	bne	3c08 <tt_crypto_hash+0x134>
    3bfc:	e3a01901 	mov	r1, #16384	; 0x4000
    3c00:	e1a00008 	mov	r0, r8
    3c04:	eb000b90 	bl	6a4c <hash_data>
    3c08:	e1a00007 	mov	r0, r7
    3c0c:	eb002798 	bl	da74 <get_timer>
    3c10:	e08aa000 	add	sl, sl, r0
    3c14:	e3550000 	cmp	r5, #0	; 0x0
    3c18:	e1a00006 	mov	r0, r6
    3c1c:	1affffe2 	bne	3bac <tt_crypto_hash+0xd8>
    3c20:	ea000008 	b	3c48 <tt_crypto_hash+0x174>
    3c24:	e1a00006 	mov	r0, r6
    3c28:	eb002791 	bl	da74 <get_timer>
    3c2c:	e5951008 	ldr	r1, [r5, #8]
    3c30:	e1a04000 	mov	r4, r0
    3c34:	e59f00ec 	ldr	r0, [pc, #236]	; 3d28 <tt_crypto_hash+0x254>
    3c38:	eb000b83 	bl	6a4c <hash_data>
    3c3c:	e1a00004 	mov	r0, r4
    3c40:	eb00278b 	bl	da74 <get_timer>
    3c44:	e1a0a000 	mov	sl, r0
    3c48:	e28d4008 	add	r4, sp, #8	; 0x8
    3c4c:	e3a0648e 	mov	r6, #-1912602624	; 0x8e000000
    3c50:	e1a00004 	mov	r0, r4
    3c54:	eb000b60 	bl	69dc <hash_value>
    3c58:	e5965008 	ldr	r5, [r6, #8]
    3c5c:	eb000b57 	bl	69c0 <hash_len>
    3c60:	e3a0c020 	mov	ip, #32	; 0x20
    3c64:	e1a03000 	mov	r3, r0
    3c68:	e1a01004 	mov	r1, r4
    3c6c:	e3a02001 	mov	r2, #1	; 0x1
    3c70:	e1a00004 	mov	r0, r4
    3c74:	e58dc000 	str	ip, [sp]
    3c78:	eb003629 	bl	11524 <print_buffer>
    3c7c:	e5963004 	ldr	r3, [r6, #4]
    3c80:	e28d1038 	add	r1, sp, #56	; 0x38
    3c84:	e0812103 	add	r2, r1, r3, lsl #2
    3c88:	e59f3084 	ldr	r3, [pc, #132]	; 3d14 <tt_crypto_hash+0x240>
    3c8c:	e5127010 	ldr	r7, [r2, #-16]
    3c90:	e5938000 	ldr	r8, [r3]
    3c94:	eb000b49 	bl	69c0 <hash_len>
    3c98:	e1a01004 	mov	r1, r4
    3c9c:	e1a02000 	mov	r2, r0
    3ca0:	e59f007c 	ldr	r0, [pc, #124]	; 3d24 <tt_crypto_hash+0x250>
    3ca4:	ebfff64d 	bl	15e0 <memcmp>
    3ca8:	e1a05525 	lsr	r5, r5, #10
    3cac:	e3500000 	cmp	r0, #0	; 0x0
    3cb0:	e59f3074 	ldr	r3, [pc, #116]	; 3d2c <tt_crypto_hash+0x258>
    3cb4:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    3cb8:	e1a0100a 	mov	r1, sl
    3cbc:	e0000590 	mul	r0, r0, r5
    3cc0:	e59f4068 	ldr	r4, [pc, #104]	; 3d30 <tt_crypto_hash+0x25c>
    3cc4:	01a04003 	moveq	r4, r3
    3cc8:	eb0042b1 	bl	14794 <__aeabi_uidiv>
    3ccc:	e1a01007 	mov	r1, r7
    3cd0:	e1a03000 	mov	r3, r0
    3cd4:	e1a02004 	mov	r2, r4
    3cd8:	e59f0054 	ldr	r0, [pc, #84]	; 3d34 <tt_crypto_hash+0x260>
    3cdc:	e12fff38 	blx	r8
    3ce0:	e59f1050 	ldr	r1, [pc, #80]	; 3d38 <tt_crypto_hash+0x264>
    3ce4:	e5960008 	ldr	r0, [r6, #8]
    3ce8:	e891000c 	ldm	r1, {r2, r3}
    3cec:	e0922000 	adds	r2, r2, r0
    3cf0:	e2a33000 	adc	r3, r3, #0	; 0x0
    3cf4:	e881000c 	stm	r1, {r2, r3}
    3cf8:	e3a0c000 	mov	ip, #0	; 0x0
    3cfc:	e1a0000c 	mov	r0, ip
    3d00:	e28dd038 	add	sp, sp, #56	; 0x38
    3d04:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    3d08:	99997777 	.word	0x99997777
    3d0c:	0001556c 	.word	0x0001556c
    3d10:	00016ffc 	.word	0x00016ffc
    3d14:	3c00d6cc 	.word	0x3c00d6cc
    3d18:	004c4b40 	.word	0x004c4b40
    3d1c:	0001701b 	.word	0x0001701b
    3d20:	00017036 	.word	0x00017036
    3d24:	8e000010 	.word	0x8e000010
    3d28:	8e000030 	.word	0x8e000030
    3d2c:	00017051 	.word	0x00017051
    3d30:	00017056 	.word	0x00017056
    3d34:	0001705d 	.word	0x0001705d
    3d38:	3c00d148 	.word	0x3c00d148

00003d3c <tt_crypto_aes>:
    3d3c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    3d40:	e24dd014 	sub	sp, sp, #20	; 0x14
    3d44:	e1a06000 	mov	r6, r0
    3d48:	e3a01000 	mov	r1, #0	; 0x0
    3d4c:	e28d000c 	add	r0, sp, #12	; 0xc
    3d50:	e3a02502 	mov	r2, #8388608	; 0x800000
    3d54:	eb0002be 	bl	4854 <tt_rnd_get_data>
    3d58:	e250b000 	subs	fp, r0, #0	; 0x0
    3d5c:	e59f4340 	ldr	r4, [pc, #832]	; 40a4 <tt_crypto_aes+0x368>
    3d60:	059f0340 	ldreq	r0, [pc, #832]	; 40a8 <tt_crypto_aes+0x36c>
    3d64:	0a00005c 	beq	3edc <tt_crypto_aes+0x1a0>
    3d68:	e59d200c 	ldr	r2, [sp, #12]
    3d6c:	e1a0100b 	mov	r1, fp
    3d70:	e3c2200f 	bic	r2, r2, #15	; 0xf
    3d74:	e1a03002 	mov	r3, r2
    3d78:	e58d200c 	str	r2, [sp, #12]
    3d7c:	e59f0328 	ldr	r0, [pc, #808]	; 40ac <tt_crypto_aes+0x370>
    3d80:	e1a02006 	mov	r2, r6
    3d84:	e1a0e00f 	mov	lr, pc
    3d88:	e594f000 	ldr	pc, [r4]
    3d8c:	e59f031c 	ldr	r0, [pc, #796]	; 40b0 <tt_crypto_aes+0x374>
    3d90:	e1a0e00f 	mov	lr, pc
    3d94:	e594f000 	ldr	pc, [r4]
    3d98:	e1a0100b 	mov	r1, fp
    3d9c:	e59d200c 	ldr	r2, [sp, #12]
    3da0:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3da4:	ebfff5ec 	bl	155c <memcpy>
    3da8:	e59f0304 	ldr	r0, [pc, #772]	; 40b4 <tt_crypto_aes+0x378>
    3dac:	e1a0e00f 	mov	lr, pc
    3db0:	e594f000 	ldr	pc, [r4]
    3db4:	e3a01000 	mov	r1, #0	; 0x0
    3db8:	e59f22f8 	ldr	r2, [pc, #760]	; 40b8 <tt_crypto_aes+0x37c>
    3dbc:	e3a00001 	mov	r0, #1	; 0x1
    3dc0:	eb000a28 	bl	6668 <ss_aes_init>
    3dc4:	e59f02f0 	ldr	r0, [pc, #752]	; 40bc <tt_crypto_aes+0x380>
    3dc8:	e1a0e00f 	mov	lr, pc
    3dcc:	e594f000 	ldr	pc, [r4]
    3dd0:	e3560000 	cmp	r6, #0	; 0x0
    3dd4:	159d500c 	ldrne	r5, [sp, #12]
    3dd8:	13a0a000 	movne	sl, #0	; 0x0
    3ddc:	12868901 	addne	r8, r6, #16384	; 0x4000
    3de0:	1a000022 	bne	3e70 <tt_crypto_aes+0x134>
    3de4:	ea000025 	b	3e80 <tt_crypto_aes+0x144>
    3de8:	e59d100c 	ldr	r1, [sp, #12]
    3dec:	e3550902 	cmp	r5, #32768	; 0x8000
    3df0:	b1a07005 	movlt	r7, r5
    3df4:	a3a07902 	movge	r7, #32768	; 0x8000
    3df8:	e0651001 	rsb	r1, r5, r1
    3dfc:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3e00:	e1a02007 	mov	r2, r7
    3e04:	ebfff5d4 	bl	155c <memcpy>
    3e08:	e3a00000 	mov	r0, #0	; 0x0
    3e0c:	eb002718 	bl	da74 <get_timer>
    3e10:	e3550901 	cmp	r5, #16384	; 0x4000
    3e14:	b1a02005 	movlt	r2, r5
    3e18:	a3a02901 	movge	r2, #16384	; 0x4000
    3e1c:	e1a04000 	mov	r4, r0
    3e20:	e1a01006 	mov	r1, r6
    3e24:	e1a00006 	mov	r0, r6
    3e28:	eb0009fd 	bl	6624 <ss_aes_data>
    3e2c:	e3570902 	cmp	r7, #32768	; 0x8000
    3e30:	1a000003 	bne	3e44 <tt_crypto_aes+0x108>
    3e34:	e1a01008 	mov	r1, r8
    3e38:	e3a02901 	mov	r2, #16384	; 0x4000
    3e3c:	e1a00008 	mov	r0, r8
    3e40:	eb0009f7 	bl	6624 <ss_aes_data>
    3e44:	e1a00004 	mov	r0, r4
    3e48:	eb002709 	bl	da74 <get_timer>
    3e4c:	e1a04000 	mov	r4, r0
    3e50:	e59d000c 	ldr	r0, [sp, #12]
    3e54:	e1a01006 	mov	r1, r6
    3e58:	e0650000 	rsb	r0, r5, r0
    3e5c:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    3e60:	e1a02007 	mov	r2, r7
    3e64:	ebfff5bc 	bl	155c <memcpy>
    3e68:	e08aa004 	add	sl, sl, r4
    3e6c:	e0675005 	rsb	r5, r7, r5
    3e70:	e3550000 	cmp	r5, #0	; 0x0
    3e74:	e1a00006 	mov	r0, r6
    3e78:	1affffda 	bne	3de8 <tt_crypto_aes+0xac>
    3e7c:	ea000009 	b	3ea8 <tt_crypto_aes+0x16c>
    3e80:	e1a00006 	mov	r0, r6
    3e84:	eb0026fa 	bl	da74 <get_timer>
    3e88:	e1a04000 	mov	r4, r0
    3e8c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3e90:	e1a01000 	mov	r1, r0
    3e94:	e59d200c 	ldr	r2, [sp, #12]
    3e98:	eb0009e1 	bl	6624 <ss_aes_data>
    3e9c:	e1a00004 	mov	r0, r4
    3ea0:	eb0026f3 	bl	da74 <get_timer>
    3ea4:	e1a0a000 	mov	sl, r0
    3ea8:	e59d300c 	ldr	r3, [sp, #12]
    3eac:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3eb0:	e1a0100b 	mov	r1, fp
    3eb4:	e3a02010 	mov	r2, #16	; 0x10
    3eb8:	e58d3004 	str	r3, [sp, #4]
    3ebc:	ebfff5c7 	bl	15e0 <memcmp>
    3ec0:	e3500000 	cmp	r0, #0	; 0x0
    3ec4:	e59f41d8 	ldr	r4, [pc, #472]	; 40a4 <tt_crypto_aes+0x368>
    3ec8:	1a000007 	bne	3eec <tt_crypto_aes+0x1b0>
    3ecc:	e59f01ec 	ldr	r0, [pc, #492]	; 40c0 <tt_crypto_aes+0x384>
    3ed0:	e1a0e00f 	mov	lr, pc
    3ed4:	e594f000 	ldr	pc, [r4]
    3ed8:	e59f01e4 	ldr	r0, [pc, #484]	; 40c4 <tt_crypto_aes+0x388>
    3edc:	e1a0e00f 	mov	lr, pc
    3ee0:	e594f000 	ldr	pc, [r4]
    3ee4:	e3e00000 	mvn	r0, #0	; 0x0
    3ee8:	ea00006b 	b	409c <tt_crypto_aes+0x360>
    3eec:	e3a00000 	mov	r0, #0	; 0x0
    3ef0:	e1a01000 	mov	r1, r0
    3ef4:	e59f21bc 	ldr	r2, [pc, #444]	; 40b8 <tt_crypto_aes+0x37c>
    3ef8:	eb0009da 	bl	6668 <ss_aes_init>
    3efc:	e59f01c4 	ldr	r0, [pc, #452]	; 40c8 <tt_crypto_aes+0x38c>
    3f00:	e1a0e00f 	mov	lr, pc
    3f04:	e594f000 	ldr	pc, [r4]
    3f08:	e3560000 	cmp	r6, #0	; 0x0
    3f0c:	159d500c 	ldrne	r5, [sp, #12]
    3f10:	13a08000 	movne	r8, #0	; 0x0
    3f14:	12869901 	addne	r9, r6, #16384	; 0x4000
    3f18:	1a000022 	bne	3fa8 <tt_crypto_aes+0x26c>
    3f1c:	ea000025 	b	3fb8 <tt_crypto_aes+0x27c>
    3f20:	eb0026d3 	bl	da74 <get_timer>
    3f24:	e59d100c 	ldr	r1, [sp, #12]
    3f28:	e3550902 	cmp	r5, #32768	; 0x8000
    3f2c:	b1a07005 	movlt	r7, r5
    3f30:	a3a07902 	movge	r7, #32768	; 0x8000
    3f34:	e0651001 	rsb	r1, r5, r1
    3f38:	e281148e 	add	r1, r1, #-1912602624	; 0x8e000000
    3f3c:	e1a02007 	mov	r2, r7
    3f40:	e1a04000 	mov	r4, r0
    3f44:	e1a00006 	mov	r0, r6
    3f48:	ebfff583 	bl	155c <memcpy>
    3f4c:	e3550901 	cmp	r5, #16384	; 0x4000
    3f50:	b1a02005 	movlt	r2, r5
    3f54:	a3a02901 	movge	r2, #16384	; 0x4000
    3f58:	e1a00006 	mov	r0, r6
    3f5c:	e1a01006 	mov	r1, r6
    3f60:	eb0009af 	bl	6624 <ss_aes_data>
    3f64:	e3570902 	cmp	r7, #32768	; 0x8000
    3f68:	1a000003 	bne	3f7c <tt_crypto_aes+0x240>
    3f6c:	e1a01009 	mov	r1, r9
    3f70:	e3a02901 	mov	r2, #16384	; 0x4000
    3f74:	e1a00009 	mov	r0, r9
    3f78:	eb0009a9 	bl	6624 <ss_aes_data>
    3f7c:	e1a00004 	mov	r0, r4
    3f80:	eb0026bb 	bl	da74 <get_timer>
    3f84:	e1a04000 	mov	r4, r0
    3f88:	e59d000c 	ldr	r0, [sp, #12]
    3f8c:	e1a01006 	mov	r1, r6
    3f90:	e0650000 	rsb	r0, r5, r0
    3f94:	e280048e 	add	r0, r0, #-1912602624	; 0x8e000000
    3f98:	e1a02007 	mov	r2, r7
    3f9c:	ebfff56e 	bl	155c <memcpy>
    3fa0:	e0888004 	add	r8, r8, r4
    3fa4:	e0675005 	rsb	r5, r7, r5
    3fa8:	e3a00000 	mov	r0, #0	; 0x0
    3fac:	e1550000 	cmp	r5, r0
    3fb0:	1affffda 	bne	3f20 <tt_crypto_aes+0x1e4>
    3fb4:	ea000009 	b	3fe0 <tt_crypto_aes+0x2a4>
    3fb8:	e1a00006 	mov	r0, r6
    3fbc:	eb0026ac 	bl	da74 <get_timer>
    3fc0:	e1a04000 	mov	r4, r0
    3fc4:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    3fc8:	e1a01000 	mov	r1, r0
    3fcc:	e59d200c 	ldr	r2, [sp, #12]
    3fd0:	eb000993 	bl	6624 <ss_aes_data>
    3fd4:	e1a00004 	mov	r0, r4
    3fd8:	eb0026a5 	bl	da74 <get_timer>
    3fdc:	e1a08000 	mov	r8, r0
    3fe0:	e59d3004 	ldr	r3, [sp, #4]
    3fe4:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    3fe8:	e1a00543 	asr	r0, r3, #10
    3fec:	e1a0100a 	mov	r1, sl
    3ff0:	e0000094 	mul	r0, r4, r0
    3ff4:	eb004268 	bl	1499c <__aeabi_idiv>
    3ff8:	e1a06000 	mov	r6, r0
    3ffc:	e59d000c 	ldr	r0, [sp, #12]
    4000:	e1a01008 	mov	r1, r8
    4004:	e1a00540 	asr	r0, r0, #10
    4008:	e0000094 	mul	r0, r4, r0
    400c:	eb004262 	bl	1499c <__aeabi_idiv>
    4010:	e59f408c 	ldr	r4, [pc, #140]	; 40a4 <tt_crypto_aes+0x368>
    4014:	e1a05000 	mov	r5, r0
    4018:	e59f00ac 	ldr	r0, [pc, #172]	; 40cc <tt_crypto_aes+0x390>
    401c:	e1a0e00f 	mov	lr, pc
    4020:	e594f000 	ldr	pc, [r4]
    4024:	e1a0000b 	mov	r0, fp
    4028:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    402c:	e59d200c 	ldr	r2, [sp, #12]
    4030:	e3a0300c 	mov	r3, #12	; 0xc
    4034:	eb000189 	bl	4660 <tt_cmp_buffer>
    4038:	e3500000 	cmp	r0, #0	; 0x0
    403c:	0a000006 	beq	405c <tt_crypto_aes+0x320>
    4040:	e1a03000 	mov	r3, r0
    4044:	e1a01006 	mov	r1, r6
    4048:	e1a02005 	mov	r2, r5
    404c:	e59f007c 	ldr	r0, [pc, #124]	; 40d0 <tt_crypto_aes+0x394>
    4050:	e1a0e00f 	mov	lr, pc
    4054:	e594f000 	ldr	pc, [r4]
    4058:	ea000004 	b	4070 <tt_crypto_aes+0x334>
    405c:	e1a01006 	mov	r1, r6
    4060:	e1a02005 	mov	r2, r5
    4064:	e59f0068 	ldr	r0, [pc, #104]	; 40d4 <tt_crypto_aes+0x398>
    4068:	e1a0e00f 	mov	lr, pc
    406c:	e594f000 	ldr	pc, [r4]
    4070:	e59f0060 	ldr	r0, [pc, #96]	; 40d8 <tt_crypto_aes+0x39c>
    4074:	e59d100c 	ldr	r1, [sp, #12]
    4078:	e890000c 	ldm	r0, {r2, r3}
    407c:	e0922001 	adds	r2, r2, r1
    4080:	e0a33fc1 	adc	r3, r3, r1, asr #31
    4084:	e880000c 	stm	r0, {r2, r3}
    4088:	e59f0030 	ldr	r0, [pc, #48]	; 40c0 <tt_crypto_aes+0x384>
    408c:	e59f3010 	ldr	r3, [pc, #16]	; 40a4 <tt_crypto_aes+0x368>
    4090:	e1a0e00f 	mov	lr, pc
    4094:	e593f000 	ldr	pc, [r3]
    4098:	e3a00000 	mov	r0, #0	; 0x0
    409c:	e28dd014 	add	sp, sp, #20	; 0x14
    40a0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    40a4:	3c00d6cc 	.word	0x3c00d6cc
    40a8:	0001707d 	.word	0x0001707d
    40ac:	000170a0 	.word	0x000170a0
    40b0:	00016ae2 	.word	0x00016ae2
    40b4:	0001791a 	.word	0x0001791a
    40b8:	3c00d088 	.word	0x3c00d088
    40bc:	00018a4c 	.word	0x00018a4c
    40c0:	00016bd7 	.word	0x00016bd7
    40c4:	000170cc 	.word	0x000170cc
    40c8:	00017f90 	.word	0x00017f90
    40cc:	00016e28 	.word	0x00016e28
    40d0:	000170ed 	.word	0x000170ed
    40d4:	0001712b 	.word	0x0001712b
    40d8:	3c00d140 	.word	0x3c00d140

000040dc <do_tt_crypto>:
    40dc:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    40e0:	e3520001 	cmp	r2, #1	; 0x1
    40e4:	e24dd004 	sub	sp, sp, #4	; 0x4
    40e8:	d3a07000 	movle	r7, #0	; 0x0
    40ec:	da000009 	ble	4118 <do_tt_crypto+0x3c>
    40f0:	e5930004 	ldr	r0, [r3, #4]
    40f4:	e3a01000 	mov	r1, #0	; 0x0
    40f8:	e3a02010 	mov	r2, #16	; 0x10
    40fc:	eb0035a0 	bl	11784 <simple_strtoul>
    4100:	e1a07000 	mov	r7, r0
    4104:	e1a01007 	mov	r1, r7
    4108:	e59f0114 	ldr	r0, [pc, #276]	; 4224 <do_tt_crypto+0x148>
    410c:	e59f3114 	ldr	r3, [pc, #276]	; 4228 <do_tt_crypto+0x14c>
    4110:	e1a0e00f 	mov	lr, pc
    4114:	e593f000 	ldr	pc, [r3]
    4118:	e3a00000 	mov	r0, #0	; 0x0
    411c:	eb002654 	bl	da74 <get_timer>
    4120:	e59f1104 	ldr	r1, [pc, #260]	; 422c <do_tt_crypto+0x150>
    4124:	e3a02000 	mov	r2, #0	; 0x0
    4128:	e3a03000 	mov	r3, #0	; 0x0
    412c:	e881000c 	stm	r1, {r2, r3}
    4130:	e59f10f8 	ldr	r1, [pc, #248]	; 4230 <do_tt_crypto+0x154>
    4134:	e59fa0ec 	ldr	sl, [pc, #236]	; 4228 <do_tt_crypto+0x14c>
    4138:	e881000c 	stm	r1, {r2, r3}
    413c:	e1a08000 	mov	r8, r0
    4140:	e3a06000 	mov	r6, #0	; 0x0
    4144:	e3a01006 	mov	r1, #6	; 0x6
    4148:	e3a00000 	mov	r0, #0	; 0x0
    414c:	eb000197 	bl	47b0 <tt_rnd_get_int>
    4150:	e3a01002 	mov	r1, #2	; 0x2
    4154:	e1a04000 	mov	r4, r0
    4158:	e3a00000 	mov	r0, #0	; 0x0
    415c:	eb000193 	bl	47b0 <tt_rnd_get_int>
    4160:	e1a01006 	mov	r1, r6
    4164:	e1a03000 	mov	r3, r0
    4168:	e1a05000 	mov	r5, r0
    416c:	e1a02004 	mov	r2, r4
    4170:	e59f00bc 	ldr	r0, [pc, #188]	; 4234 <do_tt_crypto+0x158>
    4174:	e1a0e00f 	mov	lr, pc
    4178:	e59af000 	ldr	pc, [sl]
    417c:	e3540003 	cmp	r4, #3	; 0x3
    4180:	9a000004 	bls	4198 <do_tt_crypto+0xbc>
    4184:	e3550000 	cmp	r5, #0	; 0x0
    4188:	11a00007 	movne	r0, r7
    418c:	03a00000 	moveq	r0, #0	; 0x0
    4190:	ebfffee9 	bl	3d3c <tt_crypto_aes>
    4194:	ea000003 	b	41a8 <do_tt_crypto+0xcc>
    4198:	e3550000 	cmp	r5, #0	; 0x0
    419c:	11a00007 	movne	r0, r7
    41a0:	03a00000 	moveq	r0, #0	; 0x0
    41a4:	ebfffe4a 	bl	3ad4 <tt_crypto_hash>
    41a8:	eb00398c 	bl	127e0 <tstc>
    41ac:	e3500000 	cmp	r0, #0	; 0x0
    41b0:	e2866001 	add	r6, r6, #1	; 0x1
    41b4:	0affffe2 	beq	4144 <do_tt_crypto+0x68>
    41b8:	eb003989 	bl	127e4 <getc>
    41bc:	e3500003 	cmp	r0, #3	; 0x3
    41c0:	1affffdf 	bne	4144 <do_tt_crypto+0x68>
    41c4:	e1a00008 	mov	r0, r8
    41c8:	eb002629 	bl	da74 <get_timer>
    41cc:	e59f305c 	ldr	r3, [pc, #92]	; 4230 <do_tt_crypto+0x154>
    41d0:	e59f2054 	ldr	r2, [pc, #84]	; 422c <do_tt_crypto+0x150>
    41d4:	e5934000 	ldr	r4, [r3]
    41d8:	e5925000 	ldr	r5, [r2]
    41dc:	e5933004 	ldr	r3, [r3, #4]
    41e0:	e5922004 	ldr	r2, [r2, #4]
    41e4:	e30e1a60 	movw	r1, #60000	; 0xea60
    41e8:	e1a04a24 	lsr	r4, r4, #20
    41ec:	e1a05a25 	lsr	r5, r5, #20
    41f0:	e1844603 	orr	r4, r4, r3, lsl #12
    41f4:	e1855602 	orr	r5, r5, r2, lsl #12
    41f8:	eb004165 	bl	14794 <__aeabi_uidiv>
    41fc:	e1a01004 	mov	r1, r4
    4200:	e1a03000 	mov	r3, r0
    4204:	e1a02005 	mov	r2, r5
    4208:	e59f0028 	ldr	r0, [pc, #40]	; 4238 <do_tt_crypto+0x15c>
    420c:	e59fc014 	ldr	ip, [pc, #20]	; 4228 <do_tt_crypto+0x14c>
    4210:	e1a0e00f 	mov	lr, pc
    4214:	e59cf000 	ldr	pc, [ip]
    4218:	e3a00000 	mov	r0, #0	; 0x0
    421c:	e28dd004 	add	sp, sp, #4	; 0x4
    4220:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    4224:	0001715c 	.word	0x0001715c
    4228:	3c00d6cc 	.word	0x3c00d6cc
    422c:	3c00d148 	.word	0x3c00d148
    4230:	3c00d140 	.word	0x3c00d140
    4234:	00017182 	.word	0x00017182
    4238:	000171a1 	.word	0x000171a1

0000423c <tt_ff_trolling>:
    423c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4240:	e3a01001 	mov	r1, #1	; 0x1
    4244:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    4248:	e58d200c 	str	r2, [sp, #12]
    424c:	e58d3008 	str	r3, [sp, #8]
    4250:	e1a05000 	mov	r5, r0
    4254:	eb0008c0 	bl	655c <vs_assign_by_name>
    4258:	e2506000 	subs	r6, r0, #0	; 0x0
    425c:	0a000005 	beq	4278 <tt_ff_trolling+0x3c>
    4260:	e1a01005 	mov	r1, r5
    4264:	e59f0330 	ldr	r0, [pc, #816]	; 459c <tt_ff_trolling+0x360>
    4268:	e59f3330 	ldr	r3, [pc, #816]	; 45a0 <tt_ff_trolling+0x364>
    426c:	e1a0e00f 	mov	lr, pc
    4270:	e593f000 	ldr	pc, [r3]
    4274:	ea00000c 	b	42ac <tt_ff_trolling+0x70>
    4278:	e3a0000e 	mov	r0, #14	; 0xe
    427c:	eb000744 	bl	5f94 <vs_align>
    4280:	e1a01000 	mov	r1, r0
    4284:	e28d0018 	add	r0, sp, #24	; 0x18
    4288:	e5201004 	str	r1, [r0, #-4]!
    428c:	e1a02001 	mov	r2, r1
    4290:	eb00016f 	bl	4854 <tt_rnd_get_data>
    4294:	e250a000 	subs	sl, r0, #0	; 0x0
    4298:	e59f4300 	ldr	r4, [pc, #768]	; 45a0 <tt_ff_trolling+0x364>
    429c:	1a000004 	bne	42b4 <tt_ff_trolling+0x78>
    42a0:	e59f02fc 	ldr	r0, [pc, #764]	; 45a4 <tt_ff_trolling+0x368>
    42a4:	e1a0e00f 	mov	lr, pc
    42a8:	e594f000 	ldr	pc, [r4]
    42ac:	e3e00000 	mvn	r0, #0	; 0x0
    42b0:	ea0000a9 	b	455c <tt_ff_trolling+0x320>
    42b4:	e59d2014 	ldr	r2, [sp, #20]
    42b8:	e1a0100a 	mov	r1, sl
    42bc:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    42c0:	ebfff4a5 	bl	155c <memcpy>
    42c4:	e3a01501 	mov	r1, #4194304	; 0x400000
    42c8:	e59f02d8 	ldr	r0, [pc, #728]	; 45a8 <tt_ff_trolling+0x36c>
    42cc:	e1a0e00f 	mov	lr, pc
    42d0:	e594f000 	ldr	pc, [r4]
    42d4:	e1a00005 	mov	r0, r5
    42d8:	eb0007ae 	bl	6198 <vs_device_id>
    42dc:	e1a0b004 	mov	fp, r4
    42e0:	e3a05000 	mov	r5, #0	; 0x0
    42e4:	e3a04000 	mov	r4, #0	; 0x0
    42e8:	e3a08501 	mov	r8, #4194304	; 0x400000
    42ec:	e3a09000 	mov	r9, #0	; 0x0
    42f0:	e1a07000 	mov	r7, r0
    42f4:	ea00003d 	b	43f0 <tt_ff_trolling+0x1b4>
    42f8:	e1a00007 	mov	r0, r7
    42fc:	eb000791 	bl	6148 <vs_device_erasable>
    4300:	e3500000 	cmp	r0, #0	; 0x0
    4304:	0a000003 	beq	4318 <tt_ff_trolling+0xdc>
    4308:	e1a00004 	mov	r0, r4
    430c:	e1a01005 	mov	r1, r5
    4310:	e3a02501 	mov	r2, #4194304	; 0x400000
    4314:	eb0006e2 	bl	5ea4 <vs_erase>
    4318:	e1a00007 	mov	r0, r7
    431c:	e59f1288 	ldr	r1, [pc, #648]	; 45ac <tt_ff_trolling+0x370>
    4320:	eb0007b0 	bl	61e8 <vs_is_device>
    4324:	e3500000 	cmp	r0, #0	; 0x0
    4328:	1a000009 	bne	4354 <tt_ff_trolling+0x118>
    432c:	e1a00007 	mov	r0, r7
    4330:	e59f1278 	ldr	r1, [pc, #632]	; 45b0 <tt_ff_trolling+0x374>
    4334:	eb0007ab 	bl	61e8 <vs_is_device>
    4338:	e3500000 	cmp	r0, #0	; 0x0
    433c:	1a000004 	bne	4354 <tt_ff_trolling+0x118>
    4340:	e1a00007 	mov	r0, r7
    4344:	e59f1268 	ldr	r1, [pc, #616]	; 45b4 <tt_ff_trolling+0x378>
    4348:	eb0007a6 	bl	61e8 <vs_is_device>
    434c:	e3500000 	cmp	r0, #0	; 0x0
    4350:	0a00000a 	beq	4380 <tt_ff_trolling+0x144>
    4354:	e1a00004 	mov	r0, r4
    4358:	e1a01005 	mov	r1, r5
    435c:	eb001a54 	bl	acb4 <nnd_vs_isbad>
    4360:	e3500000 	cmp	r0, #0	; 0x0
    4364:	0a000005 	beq	4380 <tt_ff_trolling+0x144>
    4368:	e59f0248 	ldr	r0, [pc, #584]	; 45b8 <tt_ff_trolling+0x37c>
    436c:	e1a02004 	mov	r2, r4
    4370:	e1a03005 	mov	r3, r5
    4374:	e1a0e00f 	mov	lr, pc
    4378:	e59bf000 	ldr	pc, [fp]
    437c:	ea000018 	b	43e4 <tt_ff_trolling+0x1a8>
    4380:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    4384:	e5816000 	str	r6, [r1]
    4388:	e1a00001 	mov	r0, r1
    438c:	e59d1014 	ldr	r1, [sp, #20]
    4390:	e1a02004 	mov	r2, r4
    4394:	e58d1000 	str	r1, [sp]
    4398:	e1a03005 	mov	r3, r5
    439c:	e3a01000 	mov	r1, #0	; 0x0
    43a0:	e58d1004 	str	r1, [sp, #4]
    43a4:	eb0006a8 	bl	5e4c <vs_write>
    43a8:	e3500000 	cmp	r0, #0	; 0x0
    43ac:	aa000006 	bge	43cc <tt_ff_trolling+0x190>
    43b0:	e1a02004 	mov	r2, r4
    43b4:	e1a03005 	mov	r3, r5
    43b8:	e59f01fc 	ldr	r0, [pc, #508]	; 45bc <tt_ff_trolling+0x380>
    43bc:	e59f11dc 	ldr	r1, [pc, #476]	; 45a0 <tt_ff_trolling+0x364>
    43c0:	e1a0e00f 	mov	lr, pc
    43c4:	e591f000 	ldr	pc, [r1]
    43c8:	ea000010 	b	4410 <tt_ff_trolling+0x1d4>
    43cc:	e316001f 	tst	r6, #31	; 0x1f
    43d0:	1a000003 	bne	43e4 <tt_ff_trolling+0x1a8>
    43d4:	e59f01e4 	ldr	r0, [pc, #484]	; 45c0 <tt_ff_trolling+0x384>
    43d8:	e1a01106 	lsl	r1, r6, #2
    43dc:	e1a0e00f 	mov	lr, pc
    43e0:	e59bf000 	ldr	pc, [fp]
    43e4:	e0944008 	adds	r4, r4, r8
    43e8:	e0a55009 	adc	r5, r5, r9
    43ec:	e2866001 	add	r6, r6, #1	; 0x1
    43f0:	e59d3008 	ldr	r3, [sp, #8]
    43f4:	e1530005 	cmp	r3, r5
    43f8:	8affffbe 	bhi	42f8 <tt_ff_trolling+0xbc>
    43fc:	1a000058 	bne	4564 <tt_ff_trolling+0x328>
    4400:	e59d100c 	ldr	r1, [sp, #12]
    4404:	e1510004 	cmp	r1, r4
    4408:	8affffba 	bhi	42f8 <tt_ff_trolling+0xbc>
    440c:	ea000054 	b	4564 <tt_ff_trolling+0x328>
    4410:	e59f4188 	ldr	r4, [pc, #392]	; 45a0 <tt_ff_trolling+0x364>
    4414:	e59f01a8 	ldr	r0, [pc, #424]	; 45c4 <tt_ff_trolling+0x388>
    4418:	e3a01501 	mov	r1, #4194304	; 0x400000
    441c:	e1a0e00f 	mov	lr, pc
    4420:	e594f000 	ldr	pc, [r4]
    4424:	e1a0b004 	mov	fp, r4
    4428:	e3a06000 	mov	r6, #0	; 0x0
    442c:	e28aa004 	add	sl, sl, #4	; 0x4
    4430:	e3a04000 	mov	r4, #0	; 0x0
    4434:	e3a05000 	mov	r5, #0	; 0x0
    4438:	e3a08501 	mov	r8, #4194304	; 0x400000
    443c:	e3a09000 	mov	r9, #0	; 0x0
    4440:	ea00003d 	b	453c <tt_ff_trolling+0x300>
    4444:	e1a00007 	mov	r0, r7
    4448:	e59f115c 	ldr	r1, [pc, #348]	; 45ac <tt_ff_trolling+0x370>
    444c:	eb000765 	bl	61e8 <vs_is_device>
    4450:	e3500000 	cmp	r0, #0	; 0x0
    4454:	1a000009 	bne	4480 <tt_ff_trolling+0x244>
    4458:	e1a00007 	mov	r0, r7
    445c:	e59f114c 	ldr	r1, [pc, #332]	; 45b0 <tt_ff_trolling+0x374>
    4460:	eb000760 	bl	61e8 <vs_is_device>
    4464:	e3500000 	cmp	r0, #0	; 0x0
    4468:	1a000004 	bne	4480 <tt_ff_trolling+0x244>
    446c:	e1a00007 	mov	r0, r7
    4470:	e59f113c 	ldr	r1, [pc, #316]	; 45b4 <tt_ff_trolling+0x378>
    4474:	eb00075b 	bl	61e8 <vs_is_device>
    4478:	e3500000 	cmp	r0, #0	; 0x0
    447c:	0a00000a 	beq	44ac <tt_ff_trolling+0x270>
    4480:	e1a00004 	mov	r0, r4
    4484:	e1a01005 	mov	r1, r5
    4488:	eb001a09 	bl	acb4 <nnd_vs_isbad>
    448c:	e3500000 	cmp	r0, #0	; 0x0
    4490:	0a000005 	beq	44ac <tt_ff_trolling+0x270>
    4494:	e59f011c 	ldr	r0, [pc, #284]	; 45b8 <tt_ff_trolling+0x37c>
    4498:	e1a02004 	mov	r2, r4
    449c:	e1a03005 	mov	r3, r5
    44a0:	e1a0e00f 	mov	lr, pc
    44a4:	e59bf000 	ldr	pc, [fp]
    44a8:	ea000020 	b	4530 <tt_ff_trolling+0x2f4>
    44ac:	e59d1014 	ldr	r1, [sp, #20]
    44b0:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    44b4:	e58d1000 	str	r1, [sp]
    44b8:	e1a02004 	mov	r2, r4
    44bc:	e3a01000 	mov	r1, #0	; 0x0
    44c0:	e1a03005 	mov	r3, r5
    44c4:	e58d1004 	str	r1, [sp, #4]
    44c8:	eb00075f 	bl	624c <vs_read>
    44cc:	e316001f 	tst	r6, #31	; 0x1f
    44d0:	1a000003 	bne	44e4 <tt_ff_trolling+0x2a8>
    44d4:	e59f00e4 	ldr	r0, [pc, #228]	; 45c0 <tt_ff_trolling+0x384>
    44d8:	e1a01106 	lsl	r1, r6, #2
    44dc:	e1a0e00f 	mov	lr, pc
    44e0:	e59bf000 	ldr	pc, [fp]
    44e4:	e3a0348e 	mov	r3, #-1912602624	; 0x8e000000
    44e8:	e5933000 	ldr	r3, [r3]
    44ec:	e1530006 	cmp	r3, r6
    44f0:	1a000007 	bne	4514 <tt_ff_trolling+0x2d8>
    44f4:	e59d2014 	ldr	r2, [sp, #20]
    44f8:	e1a0000a 	mov	r0, sl
    44fc:	e2422004 	sub	r2, r2, #4	; 0x4
    4500:	e59f10c0 	ldr	r1, [pc, #192]	; 45c8 <tt_ff_trolling+0x38c>
    4504:	e3a0300a 	mov	r3, #10	; 0xa
    4508:	eb000054 	bl	4660 <tt_cmp_buffer>
    450c:	e3500000 	cmp	r0, #0	; 0x0
    4510:	0a000006 	beq	4530 <tt_ff_trolling+0x2f4>
    4514:	e1a02004 	mov	r2, r4
    4518:	e1a03005 	mov	r3, r5
    451c:	e59f00a8 	ldr	r0, [pc, #168]	; 45cc <tt_ff_trolling+0x390>
    4520:	e59f1078 	ldr	r1, [pc, #120]	; 45a0 <tt_ff_trolling+0x364>
    4524:	e1a0e00f 	mov	lr, pc
    4528:	e591f000 	ldr	pc, [r1]
    452c:	ea000018 	b	4594 <tt_ff_trolling+0x358>
    4530:	e0944008 	adds	r4, r4, r8
    4534:	e0a55009 	adc	r5, r5, r9
    4538:	e2866001 	add	r6, r6, #1	; 0x1
    453c:	e59d3008 	ldr	r3, [sp, #8]
    4540:	e1530005 	cmp	r3, r5
    4544:	8affffbe 	bhi	4444 <tt_ff_trolling+0x208>
    4548:	1a00000a 	bne	4578 <tt_ff_trolling+0x33c>
    454c:	e59d100c 	ldr	r1, [sp, #12]
    4550:	e1510004 	cmp	r1, r4
    4554:	8affffba 	bhi	4444 <tt_ff_trolling+0x208>
    4558:	ea000006 	b	4578 <tt_ff_trolling+0x33c>
    455c:	e28dd01c 	add	sp, sp, #28	; 0x1c
    4560:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4564:	e59f0064 	ldr	r0, [pc, #100]	; 45d0 <tt_ff_trolling+0x394>
    4568:	e59f3030 	ldr	r3, [pc, #48]	; 45a0 <tt_ff_trolling+0x364>
    456c:	e1a0e00f 	mov	lr, pc
    4570:	e593f000 	ldr	pc, [r3]
    4574:	eaffffa5 	b	4410 <tt_ff_trolling+0x1d4>
    4578:	e59f0050 	ldr	r0, [pc, #80]	; 45d0 <tt_ff_trolling+0x394>
    457c:	e59f401c 	ldr	r4, [pc, #28]	; 45a0 <tt_ff_trolling+0x364>
    4580:	e1a0e00f 	mov	lr, pc
    4584:	e594f000 	ldr	pc, [r4]
    4588:	e59f0044 	ldr	r0, [pc, #68]	; 45d4 <tt_ff_trolling+0x398>
    458c:	e1a0e00f 	mov	lr, pc
    4590:	e594f000 	ldr	pc, [r4]
    4594:	e3a00000 	mov	r0, #0	; 0x0
    4598:	eaffffef 	b	455c <tt_ff_trolling+0x320>
    459c:	00017225 	.word	0x00017225
    45a0:	3c00d6cc 	.word	0x3c00d6cc
    45a4:	00017242 	.word	0x00017242
    45a8:	00017261 	.word	0x00017261
    45ac:	0001610f 	.word	0x0001610f
    45b0:	0001626b 	.word	0x0001626b
    45b4:	00016267 	.word	0x00016267
    45b8:	0001727f 	.word	0x0001727f
    45bc:	0001729e 	.word	0x0001729e
    45c0:	000172b6 	.word	0x000172b6
    45c4:	000172bd 	.word	0x000172bd
    45c8:	8e000004 	.word	0x8e000004
    45cc:	000172dd 	.word	0x000172dd
    45d0:	000172ed 	.word	0x000172ed
    45d4:	000172f4 	.word	0x000172f4

000045d8 <do_tt_ff>:
    45d8:	e92d4030 	push	{r4, r5, lr}
    45dc:	e3520001 	cmp	r2, #1	; 0x1
    45e0:	e24dd004 	sub	sp, sp, #4	; 0x4
    45e4:	e1a05003 	mov	r5, r3
    45e8:	ca000005 	bgt	4604 <do_tt_ff+0x2c>
    45ec:	e59f0060 	ldr	r0, [pc, #96]	; 4654 <do_tt_ff+0x7c>
    45f0:	e59f3060 	ldr	r3, [pc, #96]	; 4658 <do_tt_ff+0x80>
    45f4:	e1a0e00f 	mov	lr, pc
    45f8:	e593f000 	ldr	pc, [r3]
    45fc:	e3e00000 	mvn	r0, #0	; 0x0
    4600:	ea000011 	b	464c <do_tt_ff+0x74>
    4604:	e3520002 	cmp	r2, #2	; 0x2
    4608:	03a04000 	moveq	r4, #0	; 0x0
    460c:	0a000009 	beq	4638 <do_tt_ff+0x60>
    4610:	e3a01000 	mov	r1, #0	; 0x0
    4614:	e5930008 	ldr	r0, [r3, #8]
    4618:	e3a0200a 	mov	r2, #10	; 0xa
    461c:	eb003458 	bl	11784 <simple_strtoul>
    4620:	e1a04000 	mov	r4, r0
    4624:	e1a01004 	mov	r1, r4
    4628:	e59f002c 	ldr	r0, [pc, #44]	; 465c <do_tt_ff+0x84>
    462c:	e59f3024 	ldr	r3, [pc, #36]	; 4658 <do_tt_ff+0x80>
    4630:	e1a0e00f 	mov	lr, pc
    4634:	e593f000 	ldr	pc, [r3]
    4638:	e5950004 	ldr	r0, [r5, #4]
    463c:	e1a03624 	lsr	r3, r4, #12
    4640:	e1a02a04 	lsl	r2, r4, #20
    4644:	ebfffefc 	bl	423c <tt_ff_trolling>
    4648:	e3a00000 	mov	r0, #0	; 0x0
    464c:	e28dd004 	add	sp, sp, #4	; 0x4
    4650:	e8bd8030 	pop	{r4, r5, pc}
    4654:	0001730c 	.word	0x0001730c
    4658:	3c00d6cc 	.word	0x3c00d6cc
    465c:	00017352 	.word	0x00017352

00004660 <tt_cmp_buffer>:
    4660:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4664:	e3a04000 	mov	r4, #0	; 0x0
    4668:	e24dd004 	sub	sp, sp, #4	; 0x4
    466c:	e1a0a002 	mov	sl, r2
    4670:	e1a07000 	mov	r7, r0
    4674:	e1a06001 	mov	r6, r1
    4678:	e1a08003 	mov	r8, r3
    467c:	e1a09142 	asr	r9, r2, #2
    4680:	e1a05004 	mov	r5, r4
    4684:	e59fb0e4 	ldr	fp, [pc, #228]	; 4770 <tt_cmp_buffer+0x110>
    4688:	ea00001f 	b	470c <tt_cmp_buffer+0xac>
    468c:	e5970000 	ldr	r0, [r7]
    4690:	e5961000 	ldr	r1, [r6]
    4694:	e1500001 	cmp	r0, r1
    4698:	0a000018 	beq	4700 <tt_cmp_buffer+0xa0>
    469c:	e5d72000 	ldrb	r2, [r7]
    46a0:	e5d63000 	ldrb	r3, [r6]
    46a4:	e1520003 	cmp	r2, r3
    46a8:	e5d72001 	ldrb	r2, [r7, #1]
    46ac:	e5d63001 	ldrb	r3, [r6, #1]
    46b0:	12855001 	addne	r5, r5, #1	; 0x1
    46b4:	e1520003 	cmp	r2, r3
    46b8:	e5d72002 	ldrb	r2, [r7, #2]
    46bc:	e5d63002 	ldrb	r3, [r6, #2]
    46c0:	12855001 	addne	r5, r5, #1	; 0x1
    46c4:	e1520003 	cmp	r2, r3
    46c8:	e5d72003 	ldrb	r2, [r7, #3]
    46cc:	e5d63003 	ldrb	r3, [r6, #3]
    46d0:	12855001 	addne	r5, r5, #1	; 0x1
    46d4:	e1520003 	cmp	r2, r3
    46d8:	12855001 	addne	r5, r5, #1	; 0x1
    46dc:	e3580000 	cmp	r8, #0	; 0x0
    46e0:	0a000006 	beq	4700 <tt_cmp_buffer+0xa0>
    46e4:	e1a02000 	mov	r2, r0
    46e8:	e1a03001 	mov	r3, r1
    46ec:	e59f0080 	ldr	r0, [pc, #128]	; 4774 <tt_cmp_buffer+0x114>
    46f0:	e1a01104 	lsl	r1, r4, #2
    46f4:	e1a0e00f 	mov	lr, pc
    46f8:	e59bf000 	ldr	pc, [fp]
    46fc:	e2488001 	sub	r8, r8, #1	; 0x1
    4700:	e2844001 	add	r4, r4, #1	; 0x1
    4704:	e2877004 	add	r7, r7, #4	; 0x4
    4708:	e2866004 	add	r6, r6, #4	; 0x4
    470c:	e1540009 	cmp	r4, r9
    4710:	baffffdd 	blt	468c <tt_cmp_buffer+0x2c>
    4714:	e3ca9003 	bic	r9, sl, #3	; 0x3
    4718:	e3a04000 	mov	r4, #0	; 0x0
    471c:	e20aa003 	and	sl, sl, #3	; 0x3
    4720:	e59fb048 	ldr	fp, [pc, #72]	; 4770 <tt_cmp_buffer+0x110>
    4724:	ea00000c 	b	475c <tt_cmp_buffer+0xfc>
    4728:	e7d72004 	ldrb	r2, [r7, r4]
    472c:	e7d63004 	ldrb	r3, [r6, r4]
    4730:	e1520003 	cmp	r2, r3
    4734:	0a000007 	beq	4758 <tt_cmp_buffer+0xf8>
    4738:	e3580000 	cmp	r8, #0	; 0x0
    473c:	e2855001 	add	r5, r5, #1	; 0x1
    4740:	0a000004 	beq	4758 <tt_cmp_buffer+0xf8>
    4744:	e0841009 	add	r1, r4, r9
    4748:	e59f0028 	ldr	r0, [pc, #40]	; 4778 <tt_cmp_buffer+0x118>
    474c:	e1a0e00f 	mov	lr, pc
    4750:	e59bf000 	ldr	pc, [fp]
    4754:	e2488001 	sub	r8, r8, #1	; 0x1
    4758:	e2844001 	add	r4, r4, #1	; 0x1
    475c:	e154000a 	cmp	r4, sl
    4760:	bafffff0 	blt	4728 <tt_cmp_buffer+0xc8>
    4764:	e1a00005 	mov	r0, r5
    4768:	e28dd004 	add	sp, sp, #4	; 0x4
    476c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4770:	3c00d6cc 	.word	0x3c00d6cc
    4774:	00017379 	.word	0x00017379
    4778:	000173a4 	.word	0x000173a4

0000477c <tt_rnd_seed>:
    477c:	e92d4030 	push	{r4, r5, lr}
    4780:	e59f5024 	ldr	r5, [pc, #36]	; 47ac <tt_rnd_seed+0x30>
    4784:	e24dd004 	sub	sp, sp, #4	; 0x4
    4788:	e3a00000 	mov	r0, #0	; 0x0
    478c:	e5954000 	ldr	r4, [r5]
    4790:	eb0024b7 	bl	da74 <get_timer>
    4794:	e30f3ffc 	movw	r3, #65532	; 0xfffc
    4798:	e6e40070 	uxtab	r0, r4, r0
    479c:	e0003003 	and	r3, r0, r3
    47a0:	e5853000 	str	r3, [r5]
    47a4:	e28dd004 	add	sp, sp, #4	; 0x4
    47a8:	e8bd8030 	pop	{r4, r5, pc}
    47ac:	3c00d150 	.word	0x3c00d150

000047b0 <tt_rnd_get_int>:
    47b0:	e59fc094 	ldr	ip, [pc, #148]	; 484c <tt_rnd_get_int+0x9c>
    47b4:	e3a02102 	mov	r2, #-2147483648	; 0x80000000
    47b8:	e59c3000 	ldr	r3, [ip]
    47bc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    47c0:	e19330b2 	ldrh	r3, [r3, r2]
    47c4:	e24dd004 	sub	sp, sp, #4	; 0x4
    47c8:	e3c33003 	bic	r3, r3, #3	; 0x3
    47cc:	e6ff3073 	uxth	r3, r3
    47d0:	e19340b2 	ldrh	r4, [r3, r2]
    47d4:	e58c3000 	str	r3, [ip]
    47d8:	e1a07000 	mov	r7, r0
    47dc:	e1a06001 	mov	r6, r1
    47e0:	ebffffe5 	bl	477c <tt_rnd_seed>
    47e4:	e1a00004 	mov	r0, r4
    47e8:	eb0041bc 	bl	14ee0 <__aeabi_ui2d>
    47ec:	e3a02000 	mov	r2, #0	; 0x0
    47f0:	e59f3058 	ldr	r3, [pc, #88]	; 4850 <tt_rnd_get_int+0xa0>
    47f4:	eb004294 	bl	1524c <__aeabi_ddiv>
    47f8:	e1a04000 	mov	r4, r0
    47fc:	e0670006 	rsb	r0, r7, r6
    4800:	e1a05001 	mov	r5, r1
    4804:	eb0041be 	bl	14f04 <__aeabi_i2d>
    4808:	e1a02000 	mov	r2, r0
    480c:	e1a03001 	mov	r3, r1
    4810:	e1a00004 	mov	r0, r4
    4814:	e1a01005 	mov	r1, r5
    4818:	eb0041f0 	bl	14fe0 <__aeabi_dmul>
    481c:	e1a04000 	mov	r4, r0
    4820:	e1a00007 	mov	r0, r7
    4824:	e1a05001 	mov	r5, r1
    4828:	eb0041b5 	bl	14f04 <__aeabi_i2d>
    482c:	e1a02000 	mov	r2, r0
    4830:	e1a03001 	mov	r3, r1
    4834:	e1a00004 	mov	r0, r4
    4838:	e1a01005 	mov	r1, r5
    483c:	eb0040fc 	bl	14c34 <__adddf3>
    4840:	eb004302 	bl	15450 <__aeabi_d2iz>
    4844:	e28dd004 	add	sp, sp, #4	; 0x4
    4848:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    484c:	3c00d150 	.word	0x3c00d150
    4850:	40efffe0 	.word	0x40efffe0

00004854 <tt_rnd_get_data>:
    4854:	e92d4010 	push	{r4, lr}
    4858:	e1a04000 	mov	r4, r0
    485c:	e1a00001 	mov	r0, r1
    4860:	e1a01002 	mov	r1, r2
    4864:	ebffffd1 	bl	47b0 <tt_rnd_get_int>
    4868:	e59f3010 	ldr	r3, [pc, #16]	; 4880 <tt_rnd_get_data+0x2c>
    486c:	e5840000 	str	r0, [r4]
    4870:	e5930000 	ldr	r0, [r3]
    4874:	e3c00003 	bic	r0, r0, #3	; 0x3
    4878:	e2800102 	add	r0, r0, #-2147483648	; 0x80000000
    487c:	e8bd8010 	pop	{r4, pc}
    4880:	3c00d150 	.word	0x3c00d150

00004884 <do_tt_nbi>:
    4884:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    4888:	e3a00001 	mov	r0, #1	; 0x1
    488c:	e24dd054 	sub	sp, sp, #84	; 0x54
    4890:	ebffef4e 	bl	5d0 <get_xom>
    4894:	e59f30d0 	ldr	r3, [pc, #208]	; 496c <do_tt_nbi+0xe8>
    4898:	e28d4010 	add	r4, sp, #16	; 0x10
    489c:	e1a0c003 	mov	ip, r3
    48a0:	e1a06000 	mov	r6, r0
    48a4:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    48a8:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    48ac:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    48b0:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    48b4:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    48b8:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    48bc:	e89c000f 	ldm	ip, {r0, r1, r2, r3}
    48c0:	e59fa0a8 	ldr	sl, [pc, #168]	; 4970 <do_tt_nbi+0xec>
    48c4:	e59f80a8 	ldr	r8, [pc, #168]	; 4974 <do_tt_nbi+0xf0>
    48c8:	e59f70a8 	ldr	r7, [pc, #168]	; 4978 <do_tt_nbi+0xf4>
    48cc:	e884000f 	stm	r4, {r0, r1, r2, r3}
    48d0:	e3a0400a 	mov	r4, #10	; 0xa
    48d4:	e1a00004 	mov	r0, r4
    48d8:	ebffef40 	bl	5e0 <set_xom>
    48dc:	eb001a4c 	bl	b214 <nand_rescan_bootcfg>
    48e0:	e3a00000 	mov	r0, #0	; 0x0
    48e4:	eb001594 	bl	9f3c <nand_get_config>
    48e8:	e5d03005 	ldrb	r3, [r0, #5]
    48ec:	e5d02006 	ldrb	r2, [r0, #6]
    48f0:	e3530000 	cmp	r3, #0	; 0x0
    48f4:	15d03003 	ldrbne	r3, [r0, #3]
    48f8:	128d1050 	addne	r1, sp, #80	; 0x50
    48fc:	10813103 	addne	r3, r1, r3, lsl #2
    4900:	e5d01000 	ldrb	r1, [r0]
    4904:	15133040 	ldrne	r3, [r3, #-64]
    4908:	e3510000 	cmp	r1, #0	; 0x0
    490c:	e590e014 	ldr	lr, [r0, #20]
    4910:	e59a5000 	ldr	r5, [sl]
    4914:	e5d0c002 	ldrb	ip, [r0, #2]
    4918:	059f105c 	ldreq	r1, [pc, #92]	; 497c <do_tt_nbi+0xf8>
    491c:	0a000002 	beq	492c <do_tt_nbi+0xa8>
    4920:	e3510001 	cmp	r1, #1	; 0x1
    4924:	01a01008 	moveq	r1, r8
    4928:	11a01007 	movne	r1, r7
    492c:	e58d1008 	str	r1, [sp, #8]
    4930:	e88d1008 	stm	sp, {r3, ip}
    4934:	e5d03001 	ldrb	r3, [r0, #1]
    4938:	e1a01004 	mov	r1, r4
    493c:	e58d300c 	str	r3, [sp, #12]
    4940:	e2844001 	add	r4, r4, #1	; 0x1
    4944:	e1a0300e 	mov	r3, lr
    4948:	e59f0030 	ldr	r0, [pc, #48]	; 4980 <do_tt_nbi+0xfc>
    494c:	e12fff35 	blx	r5
    4950:	e354001c 	cmp	r4, #28	; 0x1c
    4954:	1affffde 	bne	48d4 <do_tt_nbi+0x50>
    4958:	e1a00006 	mov	r0, r6
    495c:	ebffef1f 	bl	5e0 <set_xom>
    4960:	e3a00000 	mov	r0, #0	; 0x0
    4964:	e28dd054 	add	sp, sp, #84	; 0x54
    4968:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    496c:	0001557c 	.word	0x0001557c
    4970:	3c00d6cc 	.word	0x3c00d6cc
    4974:	000173dd 	.word	0x000173dd
    4978:	000173d6 	.word	0x000173d6
    497c:	000173cf 	.word	0x000173cf
    4980:	000173e2 	.word	0x000173e2

00004984 <tt_simple_test>:
    4984:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    4988:	e1a0a001 	mov	sl, r1
    498c:	e24dd044 	sub	sp, sp, #68	; 0x44
    4990:	e3a01001 	mov	r1, #1	; 0x1
    4994:	e1a05000 	mov	r5, r0
    4998:	eb0006ef 	bl	655c <vs_assign_by_name>
    499c:	e3500000 	cmp	r0, #0	; 0x0
    49a0:	13e00000 	mvnne	r0, #0	; 0x0
    49a4:	1a000048 	bne	4acc <tt_simple_test+0x148>
    49a8:	e1a00005 	mov	r0, r5
    49ac:	e59f1120 	ldr	r1, [pc, #288]	; 4ad4 <tt_simple_test+0x150>
    49b0:	e3a02003 	mov	r2, #3	; 0x3
    49b4:	ebfff2af 	bl	1478 <strncmp>
    49b8:	e3500000 	cmp	r0, #0	; 0x0
    49bc:	1a000001 	bne	49c8 <tt_simple_test+0x44>
    49c0:	eb00158b 	bl	9ff4 <fnd_align>
    49c4:	ea000001 	b	49d0 <tt_simple_test+0x4c>
    49c8:	e3a0000e 	mov	r0, #14	; 0xe
    49cc:	eb000570 	bl	5f94 <vs_align>
    49d0:	e1a06400 	lsl	r6, r0, #8
    49d4:	e1a00005 	mov	r0, r5
    49d8:	eb0005ee 	bl	6198 <vs_device_id>
    49dc:	e59f80f4 	ldr	r8, [pc, #244]	; 4ad8 <tt_simple_test+0x154>
    49e0:	e1a07000 	mov	r7, r0
    49e4:	e5984000 	ldr	r4, [r8]
    49e8:	eb000569 	bl	5f94 <vs_align>
    49ec:	e1a01005 	mov	r1, r5
    49f0:	e1a02000 	mov	r2, r0
    49f4:	e59f00e0 	ldr	r0, [pc, #224]	; 4adc <tt_simple_test+0x158>
    49f8:	e12fff34 	blx	r4
    49fc:	e35a0000 	cmp	sl, #0	; 0x0
    4a00:	1a00001a 	bne	4a70 <tt_simple_test+0xec>
    4a04:	e1a00007 	mov	r0, r7
    4a08:	eb0005ce 	bl	6148 <vs_device_erasable>
    4a0c:	e3500000 	cmp	r0, #0	; 0x0
    4a10:	0a000009 	beq	4a3c <tt_simple_test+0xb8>
    4a14:	e59f00c4 	ldr	r0, [pc, #196]	; 4ae0 <tt_simple_test+0x15c>
    4a18:	e1a0e00f 	mov	lr, pc
    4a1c:	e598f000 	ldr	pc, [r8]
    4a20:	e59f10bc 	ldr	r1, [pc, #188]	; 4ae4 <tt_simple_test+0x160>
    4a24:	e1a0000d 	mov	r0, sp
    4a28:	e1a02006 	mov	r2, r6
    4a2c:	eb0036e0 	bl	125b4 <sprintf>
    4a30:	e1a0000d 	mov	r0, sp
    4a34:	e1a0100a 	mov	r1, sl
    4a38:	eb0037a5 	bl	128d4 <run_command>
    4a3c:	e59f3094 	ldr	r3, [pc, #148]	; 4ad8 <tt_simple_test+0x154>
    4a40:	e59f00a0 	ldr	r0, [pc, #160]	; 4ae8 <tt_simple_test+0x164>
    4a44:	e1a0e00f 	mov	lr, pc
    4a48:	e593f000 	ldr	pc, [r3]
    4a4c:	e1a02006 	mov	r2, r6
    4a50:	e59f1094 	ldr	r1, [pc, #148]	; 4aec <tt_simple_test+0x168>
    4a54:	e1a0000d 	mov	r0, sp
    4a58:	eb0036d5 	bl	125b4 <sprintf>
    4a5c:	e1a0000d 	mov	r0, sp
    4a60:	e3a01000 	mov	r1, #0	; 0x0
    4a64:	e1a0400d 	mov	r4, sp
    4a68:	eb003799 	bl	128d4 <run_command>
    4a6c:	ea000015 	b	4ac8 <tt_simple_test+0x144>
    4a70:	e59f0078 	ldr	r0, [pc, #120]	; 4af0 <tt_simple_test+0x16c>
    4a74:	e1a0e00f 	mov	lr, pc
    4a78:	e598f000 	ldr	pc, [r8]
    4a7c:	e1a02006 	mov	r2, r6
    4a80:	e59f106c 	ldr	r1, [pc, #108]	; 4af4 <tt_simple_test+0x170>
    4a84:	e1a0000d 	mov	r0, sp
    4a88:	eb0036c9 	bl	125b4 <sprintf>
    4a8c:	e3a01000 	mov	r1, #0	; 0x0
    4a90:	e1a0000d 	mov	r0, sp
    4a94:	eb00378e 	bl	128d4 <run_command>
    4a98:	e59f0058 	ldr	r0, [pc, #88]	; 4af8 <tt_simple_test+0x174>
    4a9c:	e1a0e00f 	mov	lr, pc
    4aa0:	e598f000 	ldr	pc, [r8]
    4aa4:	e1a02006 	mov	r2, r6
    4aa8:	e59f104c 	ldr	r1, [pc, #76]	; 4afc <tt_simple_test+0x178>
    4aac:	e3a03003 	mov	r3, #3	; 0x3
    4ab0:	e59f0048 	ldr	r0, [pc, #72]	; 4b00 <tt_simple_test+0x17c>
    4ab4:	ebfffee9 	bl	4660 <tt_cmp_buffer>
    4ab8:	e59f0044 	ldr	r0, [pc, #68]	; 4b04 <tt_simple_test+0x180>
    4abc:	e1a0400d 	mov	r4, sp
    4ac0:	e1a0e00f 	mov	lr, pc
    4ac4:	e598f000 	ldr	pc, [r8]
    4ac8:	e3a00000 	mov	r0, #0	; 0x0
    4acc:	e28dd044 	add	sp, sp, #68	; 0x44
    4ad0:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    4ad4:	0001626b 	.word	0x0001626b
    4ad8:	3c00d6cc 	.word	0x3c00d6cc
    4adc:	000176de 	.word	0x000176de
    4ae0:	000173fe 	.word	0x000173fe
    4ae4:	0001740b 	.word	0x0001740b
    4ae8:	00017420 	.word	0x00017420
    4aec:	0001742d 	.word	0x0001742d
    4af0:	0001744b 	.word	0x0001744b
    4af4:	00017458 	.word	0x00017458
    4af8:	00017475 	.word	0x00017475
    4afc:	50008000 	.word	0x50008000
    4b00:	40008000 	.word	0x40008000
    4b04:	00016bd7 	.word	0x00016bd7

00004b08 <do_tt_randomizer>:
    4b08:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4b0c:	e24dd064 	sub	sp, sp, #100	; 0x64
    4b10:	e1a06002 	mov	r6, r2
    4b14:	e28d0010 	add	r0, sp, #16	; 0x10
    4b18:	e59f12dc 	ldr	r1, [pc, #732]	; 4dfc <do_tt_randomizer+0x2f4>
    4b1c:	e3a02050 	mov	r2, #80	; 0x50
    4b20:	e1a05003 	mov	r5, r3
    4b24:	ebfff28c 	bl	155c <memcpy>
    4b28:	e3560001 	cmp	r6, #1	; 0x1
    4b2c:	d59f02cc 	ldrle	r0, [pc, #716]	; 4e00 <do_tt_randomizer+0x2f8>
    4b30:	da000009 	ble	4b5c <do_tt_randomizer+0x54>
    4b34:	e5950004 	ldr	r0, [r5, #4]
    4b38:	e59f12c4 	ldr	r1, [pc, #708]	; 4e04 <do_tt_randomizer+0x2fc>
    4b3c:	e3a02003 	mov	r2, #3	; 0x3
    4b40:	ebfff24c 	bl	1478 <strncmp>
    4b44:	e2504000 	subs	r4, r0, #0	; 0x0
    4b48:	e2858004 	add	r8, r5, #4	; 0x4
    4b4c:	1a000010 	bne	4b94 <do_tt_randomizer+0x8c>
    4b50:	e3560002 	cmp	r6, #2	; 0x2
    4b54:	1a000005 	bne	4b70 <do_tt_randomizer+0x68>
    4b58:	e59f02a8 	ldr	r0, [pc, #680]	; 4e08 <do_tt_randomizer+0x300>
    4b5c:	e59f32a8 	ldr	r3, [pc, #680]	; 4e0c <do_tt_randomizer+0x304>
    4b60:	e1a0e00f 	mov	lr, pc
    4b64:	e593f000 	ldr	pc, [r3]
    4b68:	e3e00000 	mvn	r0, #0	; 0x0
    4b6c:	ea0000a0 	b	4df4 <do_tt_randomizer+0x2ec>
    4b70:	e1a01004 	mov	r1, r4
    4b74:	e3a02010 	mov	r2, #16	; 0x10
    4b78:	e5950008 	ldr	r0, [r5, #8]
    4b7c:	eb003300 	bl	11784 <simple_strtoul>
    4b80:	ebffee96 	bl	5e0 <set_xom>
    4b84:	e1a00004 	mov	r0, r4
    4b88:	eb001aee 	bl	b748 <nand_rescan_config>
    4b8c:	e1a00004 	mov	r0, r4
    4b90:	ea000000 	b	4b98 <do_tt_randomizer+0x90>
    4b94:	e3a00001 	mov	r0, #1	; 0x1
    4b98:	eb0014e7 	bl	9f3c <nand_get_config>
    4b9c:	e3500000 	cmp	r0, #0	; 0x0
    4ba0:	e1a05000 	mov	r5, r0
    4ba4:	059f0264 	ldreq	r0, [pc, #612]	; 4e10 <do_tt_randomizer+0x308>
    4ba8:	0affffeb 	beq	4b5c <do_tt_randomizer+0x54>
    4bac:	e3a00001 	mov	r0, #1	; 0x1
    4bb0:	e3a01000 	mov	r1, #0	; 0x0
    4bb4:	e59fb250 	ldr	fp, [pc, #592]	; 4e0c <do_tt_randomizer+0x304>
    4bb8:	e58d0008 	str	r0, [sp, #8]
    4bbc:	e58d100c 	str	r1, [sp, #12]
    4bc0:	e59d000c 	ldr	r0, [sp, #12]
    4bc4:	e59f2248 	ldr	r2, [pc, #584]	; 4e14 <do_tt_randomizer+0x30c>
    4bc8:	e3a09000 	mov	r9, #0	; 0x0
    4bcc:	e7923000 	ldr	r3, [r2, r0]
    4bd0:	e59f0240 	ldr	r0, [pc, #576]	; 4e18 <do_tt_randomizer+0x310>
    4bd4:	e1a01003 	mov	r1, r3
    4bd8:	e5853048 	str	r3, [r5, #72]
    4bdc:	e59f2228 	ldr	r2, [pc, #552]	; 4e0c <do_tt_randomizer+0x304>
    4be0:	e1a0e00f 	mov	lr, pc
    4be4:	e592f000 	ldr	pc, [r2]
    4be8:	e59f1224 	ldr	r1, [pc, #548]	; 4e14 <do_tt_randomizer+0x30c>
    4bec:	e59d200c 	ldr	r2, [sp, #12]
    4bf0:	e59d0008 	ldr	r0, [sp, #8]
    4bf4:	e0813002 	add	r3, r1, r2
    4bf8:	e3a0100a 	mov	r1, #10	; 0xa
    4bfc:	e58d3000 	str	r3, [sp]
    4c00:	eb003fed 	bl	14bbc <__aeabi_idivmod>
    4c04:	e28da010 	add	sl, sp, #16	; 0x10
    4c08:	e58d1004 	str	r1, [sp, #4]
    4c0c:	ea00006a 	b	4dbc <do_tt_randomizer+0x2b4>
    4c10:	e4972004 	ldr	r2, [r7], #4
    4c14:	e1a01004 	mov	r1, r4
    4c18:	e5862038 	str	r2, [r6, #56]
    4c1c:	e2844001 	add	r4, r4, #1	; 0x1
    4c20:	e59f01f4 	ldr	r0, [pc, #500]	; 4e1c <do_tt_randomizer+0x314>
    4c24:	e59f31e0 	ldr	r3, [pc, #480]	; 4e0c <do_tt_randomizer+0x304>
    4c28:	e1a0e00f 	mov	lr, pc
    4c2c:	e593f000 	ldr	pc, [r3]
    4c30:	e3540004 	cmp	r4, #4	; 0x4
    4c34:	e2866004 	add	r6, r6, #4	; 0x4
    4c38:	1afffff4 	bne	4c10 <do_tt_randomizer+0x108>
    4c3c:	e3a00001 	mov	r0, #1	; 0x1
    4c40:	e5c50001 	strb	r0, [r5, #1]
    4c44:	e3a00000 	mov	r0, #0	; 0x0
    4c48:	eb002389 	bl	da74 <get_timer>
    4c4c:	e1a04000 	mov	r4, r0
    4c50:	e1a00005 	mov	r0, r5
    4c54:	eb0019e8 	bl	b3fc <nand_init_randomizer>
    4c58:	e1a00004 	mov	r0, r4
    4c5c:	eb002384 	bl	da74 <get_timer>
    4c60:	e1a01000 	mov	r1, r0
    4c64:	e59f01b4 	ldr	r0, [pc, #436]	; 4e20 <do_tt_randomizer+0x318>
    4c68:	e1a0e00f 	mov	lr, pc
    4c6c:	e59bf000 	ldr	pc, [fp]
    4c70:	e3a01000 	mov	r1, #0	; 0x0
    4c74:	e5980000 	ldr	r0, [r8]
    4c78:	ebffff41 	bl	4984 <tt_simple_test>
    4c7c:	e3a01000 	mov	r1, #0	; 0x0
    4c80:	e5c51001 	strb	r1, [r5, #1]
    4c84:	e59f0198 	ldr	r0, [pc, #408]	; 4e24 <do_tt_randomizer+0x31c>
    4c88:	e1a0e00f 	mov	lr, pc
    4c8c:	e59bf000 	ldr	pc, [fp]
    4c90:	e3a01001 	mov	r1, #1	; 0x1
    4c94:	e5980000 	ldr	r0, [r8]
    4c98:	ebffff39 	bl	4984 <tt_simple_test>
    4c9c:	e3a02001 	mov	r2, #1	; 0x1
    4ca0:	e5c52001 	strb	r2, [r5, #1]
    4ca4:	e59f017c 	ldr	r0, [pc, #380]	; 4e28 <do_tt_randomizer+0x320>
    4ca8:	e1a0e00f 	mov	lr, pc
    4cac:	e59bf000 	ldr	pc, [fp]
    4cb0:	e3a01001 	mov	r1, #1	; 0x1
    4cb4:	e5980000 	ldr	r0, [r8]
    4cb8:	ebffff31 	bl	4984 <tt_simple_test>
    4cbc:	e2899001 	add	r9, r9, #1	; 0x1
    4cc0:	e59f0164 	ldr	r0, [pc, #356]	; 4e2c <do_tt_randomizer+0x324>
    4cc4:	e1a0e00f 	mov	lr, pc
    4cc8:	e59bf000 	ldr	pc, [fp]
    4ccc:	e1a00009 	mov	r0, r9
    4cd0:	e3a01005 	mov	r1, #5	; 0x5
    4cd4:	eb003fb8 	bl	14bbc <__aeabi_idivmod>
    4cd8:	e28d3060 	add	r3, sp, #96	; 0x60
    4cdc:	e083c201 	add	ip, r3, r1, lsl #4
    4ce0:	e51c3050 	ldr	r3, [ip, #-80]
    4ce4:	e1a00005 	mov	r0, r5
    4ce8:	e5853038 	str	r3, [r5, #56]
    4cec:	e51c304c 	ldr	r3, [ip, #-76]
    4cf0:	e585303c 	str	r3, [r5, #60]
    4cf4:	e51c3048 	ldr	r3, [ip, #-72]
    4cf8:	e5853040 	str	r3, [r5, #64]
    4cfc:	e51c3044 	ldr	r3, [ip, #-68]
    4d00:	e5853044 	str	r3, [r5, #68]
    4d04:	eb0019bc 	bl	b3fc <nand_init_randomizer>
    4d08:	e3a01001 	mov	r1, #1	; 0x1
    4d0c:	e5980000 	ldr	r0, [r8]
    4d10:	ebffff1b 	bl	4984 <tt_simple_test>
    4d14:	e59f0114 	ldr	r0, [pc, #276]	; 4e30 <do_tt_randomizer+0x328>
    4d18:	e1a0e00f 	mov	lr, pc
    4d1c:	e59bf000 	ldr	pc, [fp]
    4d20:	e59a3000 	ldr	r3, [sl]
    4d24:	e1a00005 	mov	r0, r5
    4d28:	e5853038 	str	r3, [r5, #56]
    4d2c:	e59a3004 	ldr	r3, [sl, #4]
    4d30:	e585303c 	str	r3, [r5, #60]
    4d34:	e59a3008 	ldr	r3, [sl, #8]
    4d38:	e5853040 	str	r3, [r5, #64]
    4d3c:	e59a300c 	ldr	r3, [sl, #12]
    4d40:	e28aa010 	add	sl, sl, #16	; 0x10
    4d44:	e5853044 	str	r3, [r5, #68]
    4d48:	eb0019ab 	bl	b3fc <nand_init_randomizer>
    4d4c:	e3a01001 	mov	r1, #1	; 0x1
    4d50:	e5980000 	ldr	r0, [r8]
    4d54:	ebffff0a 	bl	4984 <tt_simple_test>
    4d58:	e59f00d4 	ldr	r0, [pc, #212]	; 4e34 <do_tt_randomizer+0x32c>
    4d5c:	e1a0e00f 	mov	lr, pc
    4d60:	e59bf000 	ldr	pc, [fp]
    4d64:	e59f20a8 	ldr	r2, [pc, #168]	; 4e14 <do_tt_randomizer+0x30c>
    4d68:	e59d1004 	ldr	r1, [sp, #4]
    4d6c:	e1a00005 	mov	r0, r5
    4d70:	e7923101 	ldr	r3, [r2, r1, lsl #2]
    4d74:	e5853048 	str	r3, [r5, #72]
    4d78:	eb00199f 	bl	b3fc <nand_init_randomizer>
    4d7c:	e3a01001 	mov	r1, #1	; 0x1
    4d80:	e5980000 	ldr	r0, [r8]
    4d84:	ebfffefe 	bl	4984 <tt_simple_test>
    4d88:	e59f00a8 	ldr	r0, [pc, #168]	; 4e38 <do_tt_randomizer+0x330>
    4d8c:	e1a0e00f 	mov	lr, pc
    4d90:	e59bf000 	ldr	pc, [fp]
    4d94:	e59d0000 	ldr	r0, [sp]
    4d98:	e5903000 	ldr	r3, [r0]
    4d9c:	e1a00005 	mov	r0, r5
    4da0:	e5853048 	str	r3, [r5, #72]
    4da4:	eb001994 	bl	b3fc <nand_init_randomizer>
    4da8:	e5980000 	ldr	r0, [r8]
    4dac:	e3a01001 	mov	r1, #1	; 0x1
    4db0:	ebfffef3 	bl	4984 <tt_simple_test>
    4db4:	e3590004 	cmp	r9, #4	; 0x4
    4db8:	0a000004 	beq	4dd0 <do_tt_randomizer+0x2c8>
    4dbc:	e28d1010 	add	r1, sp, #16	; 0x10
    4dc0:	e0817209 	add	r7, r1, r9, lsl #4
    4dc4:	e1a06005 	mov	r6, r5
    4dc8:	e3a04000 	mov	r4, #0	; 0x0
    4dcc:	eaffff8f 	b	4c10 <do_tt_randomizer+0x108>
    4dd0:	e59d2008 	ldr	r2, [sp, #8]
    4dd4:	e59d300c 	ldr	r3, [sp, #12]
    4dd8:	e2822001 	add	r2, r2, #1	; 0x1
    4ddc:	e2833004 	add	r3, r3, #4	; 0x4
    4de0:	e3520006 	cmp	r2, #6	; 0x6
    4de4:	e58d2008 	str	r2, [sp, #8]
    4de8:	e58d300c 	str	r3, [sp, #12]
    4dec:	1affff73 	bne	4bc0 <do_tt_randomizer+0xb8>
    4df0:	e3a00000 	mov	r0, #0	; 0x0
    4df4:	e28dd064 	add	sp, sp, #100	; 0x64
    4df8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    4dfc:	000155bc 	.word	0x000155bc
    4e00:	00017484 	.word	0x00017484
    4e04:	0001610f 	.word	0x0001610f
    4e08:	000174a4 	.word	0x000174a4
    4e0c:	3c00d6cc 	.word	0x3c00d6cc
    4e10:	000174bb 	.word	0x000174bb
    4e14:	000113e8 	.word	0x000113e8
    4e18:	000174e0 	.word	0x000174e0
    4e1c:	000174f8 	.word	0x000174f8
    4e20:	00017512 	.word	0x00017512
    4e24:	00017535 	.word	0x00017535
    4e28:	0001755e 	.word	0x0001755e
    4e2c:	00017586 	.word	0x00017586
    4e30:	000175ae 	.word	0x000175ae
    4e34:	000175db 	.word	0x000175db
    4e38:	00017603 	.word	0x00017603

00004e3c <tt_nand_cross_cmp>:
    4e3c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    4e40:	e24dd004 	sub	sp, sp, #4	; 0x4
    4e44:	e59da028 	ldr	sl, [sp, #40]
    4e48:	e3530000 	cmp	r3, #0	; 0x0
    4e4c:	e1a05002 	mov	r5, r2
    4e50:	13a02000 	movne	r2, #0	; 0x0
    4e54:	059a2010 	ldreq	r2, [sl, #16]
    4e58:	e3530000 	cmp	r3, #0	; 0x0
    4e5c:	e1a06001 	mov	r6, r1
    4e60:	159a3010 	ldrne	r3, [sl, #16]
    4e64:	e59a1014 	ldr	r1, [sl, #20]
    4e68:	e1a07000 	mov	r7, r0
    4e6c:	e0838001 	add	r8, r3, r1
    4e70:	e082b001 	add	fp, r2, r1
    4e74:	e3a04000 	mov	r4, #0	; 0x0
    4e78:	e2689000 	rsb	r9, r8, #0	; 0x0
    4e7c:	e3a03000 	mov	r3, #0	; 0x0
    4e80:	e1550003 	cmp	r5, r3
    4e84:	e1a00007 	mov	r0, r7
    4e88:	e1a01006 	mov	r1, r6
    4e8c:	e087700b 	add	r7, r7, fp
    4e90:	e0866008 	add	r6, r6, r8
    4e94:	e0855009 	add	r5, r5, r9
    4e98:	da000003 	ble	4eac <tt_nand_cross_cmp+0x70>
    4e9c:	e59a2014 	ldr	r2, [sl, #20]
    4ea0:	ebfffdee 	bl	4660 <tt_cmp_buffer>
    4ea4:	e0844000 	add	r4, r4, r0
    4ea8:	eafffff3 	b	4e7c <tt_nand_cross_cmp+0x40>
    4eac:	e1a00004 	mov	r0, r4
    4eb0:	e28dd004 	add	sp, sp, #4	; 0x4
    4eb4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

00004eb8 <do_tt_sysinfo>:
    4eb8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    4ebc:	e3520001 	cmp	r2, #1	; 0x1
    4ec0:	e24dd044 	sub	sp, sp, #68	; 0x44
    4ec4:	e1a04003 	mov	r4, r3
    4ec8:	ca000003 	bgt	4edc <do_tt_sysinfo+0x24>
    4ecc:	e59f01ac 	ldr	r0, [pc, #428]	; 5080 <do_tt_sysinfo+0x1c8>
    4ed0:	e59f31ac 	ldr	r3, [pc, #428]	; 5084 <do_tt_sysinfo+0x1cc>
    4ed4:	e1a0e00f 	mov	lr, pc
    4ed8:	e593f000 	ldr	pc, [r3]
    4edc:	e3a00001 	mov	r0, #1	; 0x1
    4ee0:	eb001415 	bl	9f3c <nand_get_config>
    4ee4:	e2505000 	subs	r5, r0, #0	; 0x0
    4ee8:	1a000004 	bne	4f00 <do_tt_sysinfo+0x48>
    4eec:	e59f0194 	ldr	r0, [pc, #404]	; 5088 <do_tt_sysinfo+0x1d0>
    4ef0:	e59f318c 	ldr	r3, [pc, #396]	; 5084 <do_tt_sysinfo+0x1cc>
    4ef4:	e1a0e00f 	mov	lr, pc
    4ef8:	e593f000 	ldr	pc, [r3]
    4efc:	ea000004 	b	4f14 <do_tt_sysinfo+0x5c>
    4f00:	e59f0184 	ldr	r0, [pc, #388]	; 508c <do_tt_sysinfo+0x1d4>
    4f04:	e3a01001 	mov	r1, #1	; 0x1
    4f08:	eb000593 	bl	655c <vs_assign_by_name>
    4f0c:	e2507000 	subs	r7, r0, #0	; 0x0
    4f10:	0a000001 	beq	4f1c <do_tt_sysinfo+0x64>
    4f14:	e3e00000 	mvn	r0, #0	; 0x0
    4f18:	ea000056 	b	5078 <do_tt_sysinfo+0x1c0>
    4f1c:	e1a01007 	mov	r1, r7
    4f20:	e3a0200a 	mov	r2, #10	; 0xa
    4f24:	e5940004 	ldr	r0, [r4, #4]
    4f28:	eb003215 	bl	11784 <simple_strtoul>
    4f2c:	e5850010 	str	r0, [r5, #16]
    4f30:	e1a04000 	mov	r4, r0
    4f34:	eb00142e 	bl	9ff4 <fnd_align>
    4f38:	e1a06480 	lsl	r6, r0, #9
    4f3c:	e1a01004 	mov	r1, r4
    4f40:	e59f513c 	ldr	r5, [pc, #316]	; 5084 <do_tt_sysinfo+0x1cc>
    4f44:	e59f0144 	ldr	r0, [pc, #324]	; 5090 <do_tt_sysinfo+0x1d8>
    4f48:	e1a02006 	mov	r2, r6
    4f4c:	e1a0e00f 	mov	lr, pc
    4f50:	e595f000 	ldr	pc, [r5]
    4f54:	e1a0400d 	mov	r4, sp
    4f58:	e1a01007 	mov	r1, r7
    4f5c:	e59f0130 	ldr	r0, [pc, #304]	; 5094 <do_tt_sysinfo+0x1dc>
    4f60:	e1a0e00f 	mov	lr, pc
    4f64:	e595f000 	ldr	pc, [r5]
    4f68:	e59f0128 	ldr	r0, [pc, #296]	; 5098 <do_tt_sysinfo+0x1e0>
    4f6c:	e1a0e00f 	mov	lr, pc
    4f70:	e595f000 	ldr	pc, [r5]
    4f74:	e1a02006 	mov	r2, r6
    4f78:	e59f111c 	ldr	r1, [pc, #284]	; 509c <do_tt_sysinfo+0x1e4>
    4f7c:	e1a0000d 	mov	r0, sp
    4f80:	eb00358b 	bl	125b4 <sprintf>
    4f84:	e3a01000 	mov	r1, #0	; 0x0
    4f88:	e1a0000d 	mov	r0, sp
    4f8c:	eb003650 	bl	128d4 <run_command>
    4f90:	eb003612 	bl	127e0 <tstc>
    4f94:	e2873001 	add	r3, r7, #1	; 0x1
    4f98:	e3500000 	cmp	r0, #0	; 0x0
    4f9c:	e1a07003 	mov	r7, r3
    4fa0:	0a000002 	beq	4fb0 <do_tt_sysinfo+0xf8>
    4fa4:	eb00360e 	bl	127e4 <getc>
    4fa8:	e3500003 	cmp	r0, #3	; 0x3
    4fac:	0a000030 	beq	5074 <do_tt_sysinfo+0x1bc>
    4fb0:	e59f00e8 	ldr	r0, [pc, #232]	; 50a0 <do_tt_sysinfo+0x1e8>
    4fb4:	e1a0e00f 	mov	lr, pc
    4fb8:	e595f000 	ldr	pc, [r5]
    4fbc:	e1a02006 	mov	r2, r6
    4fc0:	e59f10dc 	ldr	r1, [pc, #220]	; 50a4 <do_tt_sysinfo+0x1ec>
    4fc4:	e1a0000d 	mov	r0, sp
    4fc8:	eb003579 	bl	125b4 <sprintf>
    4fcc:	e3a01000 	mov	r1, #0	; 0x0
    4fd0:	e1a0000d 	mov	r0, sp
    4fd4:	eb00363e 	bl	128d4 <run_command>
    4fd8:	eb003600 	bl	127e0 <tstc>
    4fdc:	e3500000 	cmp	r0, #0	; 0x0
    4fe0:	0a000002 	beq	4ff0 <do_tt_sysinfo+0x138>
    4fe4:	eb0035fe 	bl	127e4 <getc>
    4fe8:	e3500003 	cmp	r0, #3	; 0x3
    4fec:	0a000020 	beq	5074 <do_tt_sysinfo+0x1bc>
    4ff0:	e59f00b0 	ldr	r0, [pc, #176]	; 50a8 <do_tt_sysinfo+0x1f0>
    4ff4:	e1a0e00f 	mov	lr, pc
    4ff8:	e595f000 	ldr	pc, [r5]
    4ffc:	e1a02006 	mov	r2, r6
    5000:	e59f10a4 	ldr	r1, [pc, #164]	; 50ac <do_tt_sysinfo+0x1f4>
    5004:	e1a0000d 	mov	r0, sp
    5008:	eb003569 	bl	125b4 <sprintf>
    500c:	e3a01000 	mov	r1, #0	; 0x0
    5010:	e1a0000d 	mov	r0, sp
    5014:	eb00362e 	bl	128d4 <run_command>
    5018:	eb0035f0 	bl	127e0 <tstc>
    501c:	e3500000 	cmp	r0, #0	; 0x0
    5020:	0a000002 	beq	5030 <do_tt_sysinfo+0x178>
    5024:	eb0035ee 	bl	127e4 <getc>
    5028:	e3500003 	cmp	r0, #3	; 0x3
    502c:	0a000010 	beq	5074 <do_tt_sysinfo+0x1bc>
    5030:	e59f0078 	ldr	r0, [pc, #120]	; 50b0 <do_tt_sysinfo+0x1f8>
    5034:	e1a0e00f 	mov	lr, pc
    5038:	e595f000 	ldr	pc, [r5]
    503c:	e59f1070 	ldr	r1, [pc, #112]	; 50b4 <do_tt_sysinfo+0x1fc>
    5040:	e1a02006 	mov	r2, r6
    5044:	e3a0300c 	mov	r3, #12	; 0xc
    5048:	e59f0068 	ldr	r0, [pc, #104]	; 50b8 <do_tt_sysinfo+0x200>
    504c:	ebfffd83 	bl	4660 <tt_cmp_buffer>
    5050:	e59f0064 	ldr	r0, [pc, #100]	; 50bc <do_tt_sysinfo+0x204>
    5054:	e1a0e00f 	mov	lr, pc
    5058:	e595f000 	ldr	pc, [r5]
    505c:	eb0035df 	bl	127e0 <tstc>
    5060:	e3500000 	cmp	r0, #0	; 0x0
    5064:	0affffbb 	beq	4f58 <do_tt_sysinfo+0xa0>
    5068:	eb0035dd 	bl	127e4 <getc>
    506c:	e3500003 	cmp	r0, #3	; 0x3
    5070:	1affffb8 	bne	4f58 <do_tt_sysinfo+0xa0>
    5074:	e3a00000 	mov	r0, #0	; 0x0
    5078:	e28dd044 	add	sp, sp, #68	; 0x44
    507c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5080:	00017630 	.word	0x00017630
    5084:	3c00d6cc 	.word	0x3c00d6cc
    5088:	00017647 	.word	0x00017647
    508c:	0001626b 	.word	0x0001626b
    5090:	00017662 	.word	0x00017662
    5094:	00016f4a 	.word	0x00016f4a
    5098:	000173fe 	.word	0x000173fe
    509c:	00017683 	.word	0x00017683
    50a0:	00017420 	.word	0x00017420
    50a4:	00017697 	.word	0x00017697
    50a8:	0001744b 	.word	0x0001744b
    50ac:	000176b4 	.word	0x000176b4
    50b0:	00017475 	.word	0x00017475
    50b4:	50008000 	.word	0x50008000
    50b8:	40008000 	.word	0x40008000
    50bc:	00016bd7 	.word	0x00016bd7

000050c0 <do_tt_sn>:
    50c0:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    50c4:	e3520001 	cmp	r2, #1	; 0x1
    50c8:	e24dd044 	sub	sp, sp, #68	; 0x44
    50cc:	e1a04002 	mov	r4, r2
    50d0:	e1a05003 	mov	r5, r3
    50d4:	ca000003 	bgt	50e8 <do_tt_sn+0x28>
    50d8:	e59f01d8 	ldr	r0, [pc, #472]	; 52b8 <do_tt_sn+0x1f8>
    50dc:	e59f31d8 	ldr	r3, [pc, #472]	; 52bc <do_tt_sn+0x1fc>
    50e0:	e1a0e00f 	mov	lr, pc
    50e4:	e593f000 	ldr	pc, [r3]
    50e8:	e5950004 	ldr	r0, [r5, #4]
    50ec:	e3a01001 	mov	r1, #1	; 0x1
    50f0:	eb000519 	bl	655c <vs_assign_by_name>
    50f4:	e250a000 	subs	sl, r0, #0	; 0x0
    50f8:	13e00000 	mvnne	r0, #0	; 0x0
    50fc:	1a00006b 	bne	52b0 <do_tt_sn+0x1f0>
    5100:	e5950004 	ldr	r0, [r5, #4]
    5104:	e59f11b4 	ldr	r1, [pc, #436]	; 52c0 <do_tt_sn+0x200>
    5108:	e3a02003 	mov	r2, #3	; 0x3
    510c:	ebfff0d9 	bl	1478 <strncmp>
    5110:	e3500000 	cmp	r0, #0	; 0x0
    5114:	13a07501 	movne	r7, #4194304	; 0x400000
    5118:	1a000001 	bne	5124 <do_tt_sn+0x64>
    511c:	eb0013b4 	bl	9ff4 <fnd_align>
    5120:	e1a07500 	lsl	r7, r0, #10
    5124:	e5950004 	ldr	r0, [r5, #4]
    5128:	eb00041a 	bl	6198 <vs_device_id>
    512c:	e3540002 	cmp	r4, #2	; 0x2
    5130:	e1a08000 	mov	r8, r0
    5134:	da000007 	ble	5158 <do_tt_sn+0x98>
    5138:	e3a01000 	mov	r1, #0	; 0x0
    513c:	e3a0200a 	mov	r2, #10	; 0xa
    5140:	e5950008 	ldr	r0, [r5, #8]
    5144:	eb00318e 	bl	11784 <simple_strtoul>
    5148:	e1a04000 	mov	r4, r0
    514c:	e1a00008 	mov	r0, r8
    5150:	eb00038f 	bl	5f94 <vs_align>
    5154:	e0070094 	mul	r7, r4, r0
    5158:	e59f615c 	ldr	r6, [pc, #348]	; 52bc <do_tt_sn+0x1fc>
    515c:	e1a00008 	mov	r0, r8
    5160:	e5954004 	ldr	r4, [r5, #4]
    5164:	e5965000 	ldr	r5, [r6]
    5168:	eb000389 	bl	5f94 <vs_align>
    516c:	e1a01004 	mov	r1, r4
    5170:	e1a02000 	mov	r2, r0
    5174:	e59f0148 	ldr	r0, [pc, #328]	; 52c4 <do_tt_sn+0x204>
    5178:	e12fff35 	blx	r5
    517c:	e1a0400d 	mov	r4, sp
    5180:	e1a0100a 	mov	r1, sl
    5184:	e59f013c 	ldr	r0, [pc, #316]	; 52c8 <do_tt_sn+0x208>
    5188:	e1a0e00f 	mov	lr, pc
    518c:	e596f000 	ldr	pc, [r6]
    5190:	e1a00008 	mov	r0, r8
    5194:	eb0003eb 	bl	6148 <vs_device_erasable>
    5198:	e28a3001 	add	r3, sl, #1	; 0x1
    519c:	e3500000 	cmp	r0, #0	; 0x0
    51a0:	e1a0a003 	mov	sl, r3
    51a4:	e59f0120 	ldr	r0, [pc, #288]	; 52cc <do_tt_sn+0x20c>
    51a8:	0a00000e 	beq	51e8 <do_tt_sn+0x128>
    51ac:	e1a0e00f 	mov	lr, pc
    51b0:	e596f000 	ldr	pc, [r6]
    51b4:	e1a02007 	mov	r2, r7
    51b8:	e59f1110 	ldr	r1, [pc, #272]	; 52d0 <do_tt_sn+0x210>
    51bc:	e1a0000d 	mov	r0, sp
    51c0:	eb0034fb 	bl	125b4 <sprintf>
    51c4:	e3a01000 	mov	r1, #0	; 0x0
    51c8:	e1a0000d 	mov	r0, sp
    51cc:	eb0035c0 	bl	128d4 <run_command>
    51d0:	eb003582 	bl	127e0 <tstc>
    51d4:	e3500000 	cmp	r0, #0	; 0x0
    51d8:	0a000002 	beq	51e8 <do_tt_sn+0x128>
    51dc:	eb003580 	bl	127e4 <getc>
    51e0:	e3500003 	cmp	r0, #3	; 0x3
    51e4:	0a000030 	beq	52ac <do_tt_sn+0x1ec>
    51e8:	e59f00e4 	ldr	r0, [pc, #228]	; 52d4 <do_tt_sn+0x214>
    51ec:	e1a0e00f 	mov	lr, pc
    51f0:	e596f000 	ldr	pc, [r6]
    51f4:	e1a02007 	mov	r2, r7
    51f8:	e59f10d8 	ldr	r1, [pc, #216]	; 52d8 <do_tt_sn+0x218>
    51fc:	e1a0000d 	mov	r0, sp
    5200:	eb0034eb 	bl	125b4 <sprintf>
    5204:	e3a01000 	mov	r1, #0	; 0x0
    5208:	e1a0000d 	mov	r0, sp
    520c:	eb0035b0 	bl	128d4 <run_command>
    5210:	eb003572 	bl	127e0 <tstc>
    5214:	e3500000 	cmp	r0, #0	; 0x0
    5218:	0a000002 	beq	5228 <do_tt_sn+0x168>
    521c:	eb003570 	bl	127e4 <getc>
    5220:	e3500003 	cmp	r0, #3	; 0x3
    5224:	0a000020 	beq	52ac <do_tt_sn+0x1ec>
    5228:	e59f00ac 	ldr	r0, [pc, #172]	; 52dc <do_tt_sn+0x21c>
    522c:	e1a0e00f 	mov	lr, pc
    5230:	e596f000 	ldr	pc, [r6]
    5234:	e1a02007 	mov	r2, r7
    5238:	e59f10a0 	ldr	r1, [pc, #160]	; 52e0 <do_tt_sn+0x220>
    523c:	e1a0000d 	mov	r0, sp
    5240:	eb0034db 	bl	125b4 <sprintf>
    5244:	e3a01000 	mov	r1, #0	; 0x0
    5248:	e1a0000d 	mov	r0, sp
    524c:	eb0035a0 	bl	128d4 <run_command>
    5250:	eb003562 	bl	127e0 <tstc>
    5254:	e3500000 	cmp	r0, #0	; 0x0
    5258:	0a000002 	beq	5268 <do_tt_sn+0x1a8>
    525c:	eb003560 	bl	127e4 <getc>
    5260:	e3500003 	cmp	r0, #3	; 0x3
    5264:	0a000010 	beq	52ac <do_tt_sn+0x1ec>
    5268:	e59f0074 	ldr	r0, [pc, #116]	; 52e4 <do_tt_sn+0x224>
    526c:	e1a0e00f 	mov	lr, pc
    5270:	e596f000 	ldr	pc, [r6]
    5274:	e59f106c 	ldr	r1, [pc, #108]	; 52e8 <do_tt_sn+0x228>
    5278:	e1a02007 	mov	r2, r7
    527c:	e3a0300c 	mov	r3, #12	; 0xc
    5280:	e59f0064 	ldr	r0, [pc, #100]	; 52ec <do_tt_sn+0x22c>
    5284:	ebfffcf5 	bl	4660 <tt_cmp_buffer>
    5288:	e59f0060 	ldr	r0, [pc, #96]	; 52f0 <do_tt_sn+0x230>
    528c:	e1a0e00f 	mov	lr, pc
    5290:	e596f000 	ldr	pc, [r6]
    5294:	eb003551 	bl	127e0 <tstc>
    5298:	e3500000 	cmp	r0, #0	; 0x0
    529c:	0affffb7 	beq	5180 <do_tt_sn+0xc0>
    52a0:	eb00354f 	bl	127e4 <getc>
    52a4:	e3500003 	cmp	r0, #3	; 0x3
    52a8:	1affffb4 	bne	5180 <do_tt_sn+0xc0>
    52ac:	e3a00000 	mov	r0, #0	; 0x0
    52b0:	e28dd044 	add	sp, sp, #68	; 0x44
    52b4:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    52b8:	000176d0 	.word	0x000176d0
    52bc:	3c00d6cc 	.word	0x3c00d6cc
    52c0:	0001626b 	.word	0x0001626b
    52c4:	000176dd 	.word	0x000176dd
    52c8:	00016f4a 	.word	0x00016f4a
    52cc:	000173fe 	.word	0x000173fe
    52d0:	00017683 	.word	0x00017683
    52d4:	00017420 	.word	0x00017420
    52d8:	00017697 	.word	0x00017697
    52dc:	0001744b 	.word	0x0001744b
    52e0:	000176b4 	.word	0x000176b4
    52e4:	00017475 	.word	0x00017475
    52e8:	50008000 	.word	0x50008000
    52ec:	40008000 	.word	0x40008000
    52f0:	00016bd7 	.word	0x00016bd7

000052f4 <tt_nand_trolling>:
    52f4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    52f8:	e24dd034 	sub	sp, sp, #52	; 0x34
    52fc:	e58d000c 	str	r0, [sp, #12]
    5300:	e1a06001 	mov	r6, r1
    5304:	e3a00001 	mov	r0, #1	; 0x1
    5308:	e3a01064 	mov	r1, #100	; 0x64
    530c:	e1a09002 	mov	r9, r2
    5310:	ebfffd26 	bl	47b0 <tt_rnd_get_int>
    5314:	e3500032 	cmp	r0, #50	; 0x32
    5318:	c5991020 	ldrgt	r1, [r9, #32]
    531c:	c58d1010 	strgt	r1, [sp, #16]
    5320:	ca000003 	bgt	5334 <tt_nand_trolling+0x40>
    5324:	e3a00001 	mov	r0, #1	; 0x1
    5328:	e5991020 	ldr	r1, [r9, #32]
    532c:	ebfffd1f 	bl	47b0 <tt_rnd_get_int>
    5330:	e58d0010 	str	r0, [sp, #16]
    5334:	e59d200c 	ldr	r2, [sp, #12]
    5338:	e5991014 	ldr	r1, [r9, #20]
    533c:	e3520002 	cmp	r2, #2	; 0x2
    5340:	05993010 	ldreq	r3, [r9, #16]
    5344:	059d2010 	ldreq	r2, [sp, #16]
    5348:	159d3010 	ldrne	r3, [sp, #16]
    534c:	00833001 	addeq	r3, r3, r1
    5350:	00010293 	muleq	r1, r3, r2
    5354:	10010391 	mulne	r1, r1, r3
    5358:	e28d002c 	add	r0, sp, #44	; 0x2c
    535c:	e1a02001 	mov	r2, r1
    5360:	ebfffd3b 	bl	4854 <tt_rnd_get_data>
    5364:	e3500000 	cmp	r0, #0	; 0x0
    5368:	e58d0014 	str	r0, [sp, #20]
    536c:	1a000004 	bne	5384 <tt_nand_trolling+0x90>
    5370:	e59f0330 	ldr	r0, [pc, #816]	; 56a8 <tt_nand_trolling+0x3b4>
    5374:	e59f3330 	ldr	r3, [pc, #816]	; 56ac <tt_nand_trolling+0x3b8>
    5378:	e1a0e00f 	mov	lr, pc
    537c:	e593f000 	ldr	pc, [r3]
    5380:	ea000028 	b	5428 <tt_nand_trolling+0x134>
    5384:	e5991024 	ldr	r1, [r9, #36]
    5388:	e3a00000 	mov	r0, #0	; 0x0
    538c:	ebfffd07 	bl	47b0 <tt_rnd_get_int>
    5390:	e5995020 	ldr	r5, [r9, #32]
    5394:	e59d2010 	ldr	r2, [sp, #16]
    5398:	e1a04000 	mov	r4, r0
    539c:	e0621005 	rsb	r1, r2, r5
    53a0:	e3a00000 	mov	r0, #0	; 0x0
    53a4:	ebfffd01 	bl	47b0 <tt_rnd_get_int>
    53a8:	e59d300c 	ldr	r3, [sp, #12]
    53ac:	e0240495 	mla	r4, r5, r4, r0
    53b0:	e3530000 	cmp	r3, #0	; 0x0
    53b4:	05963014 	ldreq	r3, [r6, #20]
    53b8:	15993014 	ldrne	r3, [r9, #20]
    53bc:	e1a05fc4 	asr	r5, r4, #31
    53c0:	e0876394 	umull	r6, r7, r4, r3
    53c4:	e3a01000 	mov	r1, #0	; 0x0
    53c8:	e3a02010 	mov	r2, #16	; 0x10
    53cc:	e28d001c 	add	r0, sp, #28	; 0x1c
    53d0:	e0277593 	mla	r7, r3, r5, r7
    53d4:	ebfff058 	bl	153c <memset>
    53d8:	e59f32d0 	ldr	r3, [pc, #720]	; 56b0 <tt_nand_trolling+0x3bc>
    53dc:	e59d200c 	ldr	r2, [sp, #12]
    53e0:	e58d301c 	str	r3, [sp, #28]
    53e4:	e59f32c8 	ldr	r3, [pc, #712]	; 56b4 <tt_nand_trolling+0x3c0>
    53e8:	e28d1030 	add	r1, sp, #48	; 0x30
    53ec:	e58d3020 	str	r3, [sp, #32]
    53f0:	e59f32c0 	ldr	r3, [pc, #704]	; 56b8 <tt_nand_trolling+0x3c4>
    53f4:	e59f82b0 	ldr	r8, [pc, #688]	; 56ac <tt_nand_trolling+0x3b8>
    53f8:	e58d3024 	str	r3, [sp, #36]
    53fc:	e0813102 	add	r3, r1, r2, lsl #2
    5400:	e513b014 	ldr	fp, [r3, #-20]
    5404:	e3a01001 	mov	r1, #1	; 0x1
    5408:	e1a0000b 	mov	r0, fp
    540c:	eb000452 	bl	655c <vs_assign_by_name>
    5410:	e3500000 	cmp	r0, #0	; 0x0
    5414:	0a000005 	beq	5430 <tt_nand_trolling+0x13c>
    5418:	e1a0100b 	mov	r1, fp
    541c:	e59f0298 	ldr	r0, [pc, #664]	; 56bc <tt_nand_trolling+0x3c8>
    5420:	e1a0e00f 	mov	lr, pc
    5424:	e598f000 	ldr	pc, [r8]
    5428:	e3e00000 	mvn	r0, #0	; 0x0
    542c:	ea00009b 	b	56a0 <tt_nand_trolling+0x3ac>
    5430:	e59d2014 	ldr	r2, [sp, #20]
    5434:	e59d302c 	ldr	r3, [sp, #44]
    5438:	e59f0280 	ldr	r0, [pc, #640]	; 56c0 <tt_nand_trolling+0x3cc>
    543c:	e1a0100b 	mov	r1, fp
    5440:	e88d00c0 	stm	sp, {r6, r7}
    5444:	e1a0e00f 	mov	lr, pc
    5448:	e598f000 	ldr	pc, [r8]
    544c:	e1a00006 	mov	r0, r6
    5450:	e1a01007 	mov	r1, r7
    5454:	eb0002a1 	bl	5ee0 <vs_isbad>
    5458:	e250a000 	subs	sl, r0, #0	; 0x0
    545c:	0a000004 	beq	5474 <tt_nand_trolling+0x180>
    5460:	e59f025c 	ldr	r0, [pc, #604]	; 56c4 <tt_nand_trolling+0x3d0>
    5464:	e1a0e00f 	mov	lr, pc
    5468:	e598f000 	ldr	pc, [r8]
    546c:	e3e0000a 	mvn	r0, #10	; 0xa
    5470:	ea00008a 	b	56a0 <tt_nand_trolling+0x3ac>
    5474:	e1a01007 	mov	r1, r7
    5478:	e59d202c 	ldr	r2, [sp, #44]
    547c:	e1a00006 	mov	r0, r6
    5480:	eb000287 	bl	5ea4 <vs_erase>
    5484:	e1a0000a 	mov	r0, sl
    5488:	eb002179 	bl	da74 <get_timer>
    548c:	e59d102c 	ldr	r1, [sp, #44]
    5490:	e1a02006 	mov	r2, r6
    5494:	e1a03007 	mov	r3, r7
    5498:	e1a04000 	mov	r4, r0
    549c:	e59d0014 	ldr	r0, [sp, #20]
    54a0:	e88d0402 	stm	sp, {r1, sl}
    54a4:	eb000268 	bl	5e4c <vs_write>
    54a8:	e1a00004 	mov	r0, r4
    54ac:	eb002170 	bl	da74 <get_timer>
    54b0:	e1a01000 	mov	r1, r0
    54b4:	e59d002c 	ldr	r0, [sp, #44]
    54b8:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    54bc:	e1a00540 	asr	r0, r0, #10
    54c0:	e0000095 	mul	r0, r5, r0
    54c4:	eb003cb2 	bl	14794 <__aeabi_uidiv>
    54c8:	e58d0008 	str	r0, [sp, #8]
    54cc:	e1a0000a 	mov	r0, sl
    54d0:	eb002167 	bl	da74 <get_timer>
    54d4:	e59d102c 	ldr	r1, [sp, #44]
    54d8:	e1a02006 	mov	r2, r6
    54dc:	e1a03007 	mov	r3, r7
    54e0:	e1a04000 	mov	r4, r0
    54e4:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    54e8:	e88d0402 	stm	sp, {r1, sl}
    54ec:	eb000356 	bl	624c <vs_read>
    54f0:	e1a00004 	mov	r0, r4
    54f4:	eb00215e 	bl	da74 <get_timer>
    54f8:	e59d402c 	ldr	r4, [sp, #44]
    54fc:	e1a01000 	mov	r1, r0
    5500:	e1a00544 	asr	r0, r4, #10
    5504:	e0000095 	mul	r0, r5, r0
    5508:	eb003ca1 	bl	14794 <__aeabi_uidiv>
    550c:	e1a02004 	mov	r2, r4
    5510:	e1a05000 	mov	r5, r0
    5514:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    5518:	e59d0014 	ldr	r0, [sp, #20]
    551c:	e3a0300c 	mov	r3, #12	; 0xc
    5520:	ebfffc4e 	bl	4660 <tt_cmp_buffer>
    5524:	e3500000 	cmp	r0, #0	; 0x0
    5528:	0a000007 	beq	554c <tt_nand_trolling+0x258>
    552c:	e58d0000 	str	r0, [sp]
    5530:	e1a0100b 	mov	r1, fp
    5534:	e59d2008 	ldr	r2, [sp, #8]
    5538:	e1a03005 	mov	r3, r5
    553c:	e59f0184 	ldr	r0, [pc, #388]	; 56c8 <tt_nand_trolling+0x3d4>
    5540:	e1a0e00f 	mov	lr, pc
    5544:	e598f000 	ldr	pc, [r8]
    5548:	ea000005 	b	5564 <tt_nand_trolling+0x270>
    554c:	e1a0100b 	mov	r1, fp
    5550:	e59d2008 	ldr	r2, [sp, #8]
    5554:	e1a03005 	mov	r3, r5
    5558:	e59f016c 	ldr	r0, [pc, #364]	; 56cc <tt_nand_trolling+0x3d8>
    555c:	e1a0e00f 	mov	lr, pc
    5560:	e598f000 	ldr	pc, [r8]
    5564:	e59d102c 	ldr	r1, [sp, #44]
    5568:	e59f0160 	ldr	r0, [pc, #352]	; 56d0 <tt_nand_trolling+0x3dc>
    556c:	e890000c 	ldm	r0, {r2, r3}
    5570:	e0922001 	adds	r2, r2, r1
    5574:	e0a33fc1 	adc	r3, r3, r1, asr #31
    5578:	e59d100c 	ldr	r1, [sp, #12]
    557c:	e880000c 	stm	r0, {r2, r3}
    5580:	e3510000 	cmp	r1, #0	; 0x0
    5584:	0a000040 	beq	568c <tt_nand_trolling+0x398>
    5588:	e2618003 	rsb	r8, r1, #3	; 0x3
    558c:	e28d2030 	add	r2, sp, #48	; 0x30
    5590:	e0823108 	add	r3, r2, r8, lsl #2
    5594:	e513a014 	ldr	sl, [r3, #-20]
    5598:	e3a01001 	mov	r1, #1	; 0x1
    559c:	e1a0000a 	mov	r0, sl
    55a0:	eb0003ed 	bl	655c <vs_assign_by_name>
    55a4:	e3500000 	cmp	r0, #0	; 0x0
    55a8:	0a000005 	beq	55c4 <tt_nand_trolling+0x2d0>
    55ac:	e1a0100a 	mov	r1, sl
    55b0:	e59f0104 	ldr	r0, [pc, #260]	; 56bc <tt_nand_trolling+0x3c8>
    55b4:	e59f30f0 	ldr	r3, [pc, #240]	; 56ac <tt_nand_trolling+0x3b8>
    55b8:	e1a0e00f 	mov	lr, pc
    55bc:	e593f000 	ldr	pc, [r3]
    55c0:	eaffff98 	b	5428 <tt_nand_trolling+0x134>
    55c4:	e3580001 	cmp	r8, #1	; 0x1
    55c8:	15990010 	ldrne	r0, [r9, #16]
    55cc:	e5993014 	ldr	r3, [r9, #20]
    55d0:	e59d1010 	ldr	r1, [sp, #16]
    55d4:	e0803003 	add	r3, r0, r3
    55d8:	e0030391 	mul	r3, r1, r3
    55dc:	e3a00000 	mov	r0, #0	; 0x0
    55e0:	e58d302c 	str	r3, [sp, #44]
    55e4:	eb002122 	bl	da74 <get_timer>
    55e8:	e59d102c 	ldr	r1, [sp, #44]
    55ec:	e1a02006 	mov	r2, r6
    55f0:	e1a04000 	mov	r4, r0
    55f4:	e1a03007 	mov	r3, r7
    55f8:	e58d1000 	str	r1, [sp]
    55fc:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5600:	e3a01000 	mov	r1, #0	; 0x0
    5604:	e58d1004 	str	r1, [sp, #4]
    5608:	eb00030f 	bl	624c <vs_read>
    560c:	e1a00004 	mov	r0, r4
    5610:	eb002117 	bl	da74 <get_timer>
    5614:	e59d402c 	ldr	r4, [sp, #44]
    5618:	e1a01000 	mov	r1, r0
    561c:	e1a03544 	asr	r3, r4, #10
    5620:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    5624:	e0000390 	mul	r0, r0, r3
    5628:	eb003c59 	bl	14794 <__aeabi_uidiv>
    562c:	e3580002 	cmp	r8, #2	; 0x2
    5630:	13a03000 	movne	r3, #0	; 0x0
    5634:	03a03001 	moveq	r3, #1	; 0x1
    5638:	e1a05000 	mov	r5, r0
    563c:	e1a02004 	mov	r2, r4
    5640:	e59d0014 	ldr	r0, [sp, #20]
    5644:	e3a0148e 	mov	r1, #-1912602624	; 0x8e000000
    5648:	e58d9000 	str	r9, [sp]
    564c:	ebfffdfa 	bl	4e3c <tt_nand_cross_cmp>
    5650:	e3500000 	cmp	r0, #0	; 0x0
    5654:	e59f8050 	ldr	r8, [pc, #80]	; 56ac <tt_nand_trolling+0x3b8>
    5658:	0a000006 	beq	5678 <tt_nand_trolling+0x384>
    565c:	e1a03000 	mov	r3, r0
    5660:	e1a0100a 	mov	r1, sl
    5664:	e1a02005 	mov	r2, r5
    5668:	e59f0064 	ldr	r0, [pc, #100]	; 56d4 <tt_nand_trolling+0x3e0>
    566c:	e1a0e00f 	mov	lr, pc
    5670:	e598f000 	ldr	pc, [r8]
    5674:	ea000004 	b	568c <tt_nand_trolling+0x398>
    5678:	e1a0100a 	mov	r1, sl
    567c:	e1a02005 	mov	r2, r5
    5680:	e59f0050 	ldr	r0, [pc, #80]	; 56d8 <tt_nand_trolling+0x3e4>
    5684:	e1a0e00f 	mov	lr, pc
    5688:	e598f000 	ldr	pc, [r8]
    568c:	e59f0048 	ldr	r0, [pc, #72]	; 56dc <tt_nand_trolling+0x3e8>
    5690:	e59f3014 	ldr	r3, [pc, #20]	; 56ac <tt_nand_trolling+0x3b8>
    5694:	e1a0e00f 	mov	lr, pc
    5698:	e593f000 	ldr	pc, [r3]
    569c:	e3a00000 	mov	r0, #0	; 0x0
    56a0:	e28dd034 	add	sp, sp, #52	; 0x34
    56a4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    56a8:	000176fb 	.word	0x000176fb
    56ac:	3c00d6cc 	.word	0x3c00d6cc
    56b0:	0001610f 	.word	0x0001610f
    56b4:	00016267 	.word	0x00016267
    56b8:	0001626b 	.word	0x0001626b
    56bc:	0001771a 	.word	0x0001771a
    56c0:	00017737 	.word	0x00017737
    56c4:	00017767 	.word	0x00017767
    56c8:	0001778f 	.word	0x0001778f
    56cc:	000177c8 	.word	0x000177c8
    56d0:	3c00d158 	.word	0x3c00d158
    56d4:	000177f4 	.word	0x000177f4
    56d8:	00017821 	.word	0x00017821
    56dc:	00016bd7 	.word	0x00016bd7

000056e0 <do_tt_ndloop>:
    56e0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    56e4:	e3a00001 	mov	r0, #1	; 0x1
    56e8:	e24dd004 	sub	sp, sp, #4	; 0x4
    56ec:	eb001212 	bl	9f3c <nand_get_config>
    56f0:	e2505000 	subs	r5, r0, #0	; 0x0
    56f4:	1a000005 	bne	5710 <do_tt_ndloop+0x30>
    56f8:	e59f00d0 	ldr	r0, [pc, #208]	; 57d0 <do_tt_ndloop+0xf0>
    56fc:	e59f30d0 	ldr	r3, [pc, #208]	; 57d4 <do_tt_ndloop+0xf4>
    5700:	e1a0e00f 	mov	lr, pc
    5704:	e593f000 	ldr	pc, [r3]
    5708:	e3e00001 	mvn	r0, #1	; 0x1
    570c:	ea00002d 	b	57c8 <do_tt_ndloop+0xe8>
    5710:	e3a00000 	mov	r0, #0	; 0x0
    5714:	eb0020d6 	bl	da74 <get_timer>
    5718:	e59f10b8 	ldr	r1, [pc, #184]	; 57d8 <do_tt_ndloop+0xf8>
    571c:	e3a02000 	mov	r2, #0	; 0x0
    5720:	e3a03000 	mov	r3, #0	; 0x0
    5724:	e881000c 	stm	r1, {r2, r3}
    5728:	e59f70a4 	ldr	r7, [pc, #164]	; 57d4 <do_tt_ndloop+0xf4>
    572c:	e1a06000 	mov	r6, r0
    5730:	e3a04000 	mov	r4, #0	; 0x0
    5734:	e1a01004 	mov	r1, r4
    5738:	e59f009c 	ldr	r0, [pc, #156]	; 57dc <do_tt_ndloop+0xfc>
    573c:	e1a0e00f 	mov	lr, pc
    5740:	e597f000 	ldr	pc, [r7]
    5744:	e3a01000 	mov	r1, #0	; 0x0
    5748:	e1a02005 	mov	r2, r5
    574c:	e3a00001 	mov	r0, #1	; 0x1
    5750:	ebfffee7 	bl	52f4 <tt_nand_trolling>
    5754:	e3500000 	cmp	r0, #0	; 0x0
    5758:	e2844001 	add	r4, r4, #1	; 0x1
    575c:	1a000003 	bne	5770 <do_tt_ndloop+0x90>
    5760:	e1a01000 	mov	r1, r0
    5764:	e1a02005 	mov	r2, r5
    5768:	e3a00002 	mov	r0, #2	; 0x2
    576c:	ebfffee0 	bl	52f4 <tt_nand_trolling>
    5770:	eb00341a 	bl	127e0 <tstc>
    5774:	e3500000 	cmp	r0, #0	; 0x0
    5778:	0affffed 	beq	5734 <do_tt_ndloop+0x54>
    577c:	eb003418 	bl	127e4 <getc>
    5780:	e3500003 	cmp	r0, #3	; 0x3
    5784:	1affffea 	bne	5734 <do_tt_ndloop+0x54>
    5788:	e1a00006 	mov	r0, r6
    578c:	eb0020b8 	bl	da74 <get_timer>
    5790:	e59f3040 	ldr	r3, [pc, #64]	; 57d8 <do_tt_ndloop+0xf8>
    5794:	e30e1a60 	movw	r1, #60000	; 0xea60
    5798:	e5934000 	ldr	r4, [r3]
    579c:	e5933004 	ldr	r3, [r3, #4]
    57a0:	e1a04a24 	lsr	r4, r4, #20
    57a4:	e1844603 	orr	r4, r4, r3, lsl #12
    57a8:	eb003c7b 	bl	1499c <__aeabi_idiv>
    57ac:	e1a01004 	mov	r1, r4
    57b0:	e1a02000 	mov	r2, r0
    57b4:	e59f3018 	ldr	r3, [pc, #24]	; 57d4 <do_tt_ndloop+0xf4>
    57b8:	e59f0020 	ldr	r0, [pc, #32]	; 57e0 <do_tt_ndloop+0x100>
    57bc:	e1a0e00f 	mov	lr, pc
    57c0:	e593f000 	ldr	pc, [r3]
    57c4:	e3a00000 	mov	r0, #0	; 0x0
    57c8:	e28dd004 	add	sp, sp, #4	; 0x4
    57cc:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    57d0:	00017841 	.word	0x00017841
    57d4:	3c00d6cc 	.word	0x3c00d6cc
    57d8:	3c00d158 	.word	0x3c00d158
    57dc:	00016f4a 	.word	0x00016f4a
    57e0:	00016f54 	.word	0x00016f54

000057e4 <do_tt_nand>:
    57e4:	e3520001 	cmp	r2, #1	; 0x1
    57e8:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    57ec:	e1a08002 	mov	r8, r2
    57f0:	e1a0a003 	mov	sl, r3
    57f4:	c59f90dc 	ldrgt	r9, [pc, #220]	; 58d8 <do_tt_nand+0xf4>
    57f8:	c3a06001 	movgt	r6, #1	; 0x1
    57fc:	ca000005 	bgt	5818 <do_tt_nand+0x34>
    5800:	e59f00d4 	ldr	r0, [pc, #212]	; 58dc <do_tt_nand+0xf8>
    5804:	e59f30cc 	ldr	r3, [pc, #204]	; 58d8 <do_tt_nand+0xf4>
    5808:	e1a0e00f 	mov	lr, pc
    580c:	e593f000 	ldr	pc, [r3]
    5810:	e3e00000 	mvn	r0, #0	; 0x0
    5814:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    5818:	e3a01000 	mov	r1, #0	; 0x0
    581c:	e3a0200a 	mov	r2, #10	; 0xa
    5820:	e79a0106 	ldr	r0, [sl, r6, lsl #2]
    5824:	eb002fd6 	bl	11784 <simple_strtoul>
    5828:	e1a07000 	mov	r7, r0
    582c:	ebffeb6b 	bl	5e0 <set_xom>
    5830:	e3a00000 	mov	r0, #0	; 0x0
    5834:	eb0017c3 	bl	b748 <nand_rescan_config>
    5838:	e3a00000 	mov	r0, #0	; 0x0
    583c:	eb0011be 	bl	9f3c <nand_get_config>
    5840:	e1a05000 	mov	r5, r0
    5844:	e3a00001 	mov	r0, #1	; 0x1
    5848:	eb0011bb 	bl	9f3c <nand_get_config>
    584c:	e3500000 	cmp	r0, #0	; 0x0
    5850:	13550000 	cmpne	r5, #0	; 0x0
    5854:	e1a04000 	mov	r4, r0
    5858:	1a000005 	bne	5874 <do_tt_nand+0x90>
    585c:	e59f007c 	ldr	r0, [pc, #124]	; 58e0 <do_tt_nand+0xfc>
    5860:	e59f3070 	ldr	r3, [pc, #112]	; 58d8 <do_tt_nand+0xf4>
    5864:	e1a0e00f 	mov	lr, pc
    5868:	e593f000 	ldr	pc, [r3]
    586c:	e3e00001 	mvn	r0, #1	; 0x1
    5870:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    5874:	e1a01007 	mov	r1, r7
    5878:	e59f0064 	ldr	r0, [pc, #100]	; 58e4 <do_tt_nand+0x100>
    587c:	e1a0e00f 	mov	lr, pc
    5880:	e599f000 	ldr	pc, [r9]
    5884:	e3a00000 	mov	r0, #0	; 0x0
    5888:	e1a01005 	mov	r1, r5
    588c:	e1a02004 	mov	r2, r4
    5890:	ebfffe97 	bl	52f4 <tt_nand_trolling>
    5894:	e370000b 	cmn	r0, #11	; 0xb
    5898:	0afffff9 	beq	5884 <do_tt_nand+0xa0>
    589c:	e3a00001 	mov	r0, #1	; 0x1
    58a0:	e1a01005 	mov	r1, r5
    58a4:	e1a02004 	mov	r2, r4
    58a8:	ebfffe91 	bl	52f4 <tt_nand_trolling>
    58ac:	e370000b 	cmn	r0, #11	; 0xb
    58b0:	0afffff9 	beq	589c <do_tt_nand+0xb8>
    58b4:	e1a01005 	mov	r1, r5
    58b8:	e1a02004 	mov	r2, r4
    58bc:	e2866001 	add	r6, r6, #1	; 0x1
    58c0:	e3a00002 	mov	r0, #2	; 0x2
    58c4:	ebfffe8a 	bl	52f4 <tt_nand_trolling>
    58c8:	e1560008 	cmp	r6, r8
    58cc:	baffffd1 	blt	5818 <do_tt_nand+0x34>
    58d0:	e3a00000 	mov	r0, #0	; 0x0
    58d4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    58d8:	3c00d6cc 	.word	0x3c00d6cc
    58dc:	00017873 	.word	0x00017873
    58e0:	00017841 	.word	0x00017841
    58e4:	000178a9 	.word	0x000178a9

000058e8 <tt_wire_get_opt>:
    58e8:	e5900008 	ldr	r0, [r0, #8]
    58ec:	e12fff1e 	bx	lr

000058f0 <tt_wire_get_len>:
    58f0:	e590000c 	ldr	r0, [r0, #12]
    58f4:	e12fff1e 	bx	lr

000058f8 <tt_wire_make_data>:
    58f8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    58fc:	e24dd004 	sub	sp, sp, #4	; 0x4
    5900:	e1a04000 	mov	r4, r0
    5904:	e1a07002 	mov	r7, r2
    5908:	e1a06003 	mov	r6, r3
    590c:	ebffef12 	bl	155c <memcpy>
    5910:	e59f3040 	ldr	r3, [pc, #64]	; 5958 <tt_wire_make_data+0x60>
    5914:	e5846008 	str	r6, [r4, #8]
    5918:	e5843000 	str	r3, [r4]
    591c:	e59d3018 	ldr	r3, [sp, #24]
    5920:	e3a05000 	mov	r5, #0	; 0x0
    5924:	e584300c 	str	r3, [r4, #12]
    5928:	e1a01007 	mov	r1, r7
    592c:	e5845004 	str	r5, [r4, #4]
    5930:	e1a00005 	mov	r0, r5
    5934:	eb000469 	bl	6ae0 <hash_init>
    5938:	e1a01007 	mov	r1, r7
    593c:	e1a00004 	mov	r0, r4
    5940:	eb000441 	bl	6a4c <hash_data>
    5944:	e2840004 	add	r0, r4, #4	; 0x4
    5948:	eb000423 	bl	69dc <hash_value>
    594c:	e1a00005 	mov	r0, r5
    5950:	e28dd004 	add	sp, sp, #4	; 0x4
    5954:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    5958:	7766dd80 	.word	0x7766dd80

0000595c <tt_wire_check_valid>:
    595c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    5960:	e5903000 	ldr	r3, [r0]
    5964:	e59f2084 	ldr	r2, [pc, #132]	; 59f0 <tt_wire_check_valid+0x94>
    5968:	e24dd00c 	sub	sp, sp, #12	; 0xc
    596c:	e1530002 	cmp	r3, r2
    5970:	e1a04000 	mov	r4, r0
    5974:	e1a06001 	mov	r6, r1
    5978:	e5907004 	ldr	r7, [r0, #4]
    597c:	0a000006 	beq	599c <tt_wire_check_valid+0x40>
    5980:	e1a01003 	mov	r1, r3
    5984:	e59f0068 	ldr	r0, [pc, #104]	; 59f4 <tt_wire_check_valid+0x98>
    5988:	e59f3068 	ldr	r3, [pc, #104]	; 59f8 <tt_wire_check_valid+0x9c>
    598c:	e1a0e00f 	mov	lr, pc
    5990:	e593f000 	ldr	pc, [r3]
    5994:	e3a00000 	mov	r0, #0	; 0x0
    5998:	ea000012 	b	59e8 <tt_wire_check_valid+0x8c>
    599c:	e3a05000 	mov	r5, #0	; 0x0
    59a0:	e5805004 	str	r5, [r0, #4]
    59a4:	e1a00005 	mov	r0, r5
    59a8:	eb00044c 	bl	6ae0 <hash_init>
    59ac:	e1a01006 	mov	r1, r6
    59b0:	e1a00004 	mov	r0, r4
    59b4:	eb000424 	bl	6a4c <hash_data>
    59b8:	e28d0004 	add	r0, sp, #4	; 0x4
    59bc:	eb000406 	bl	69dc <hash_value>
    59c0:	e59d1004 	ldr	r1, [sp, #4]
    59c4:	e1510007 	cmp	r1, r7
    59c8:	03a00001 	moveq	r0, #1	; 0x1
    59cc:	0a000005 	beq	59e8 <tt_wire_check_valid+0x8c>
    59d0:	e59f0024 	ldr	r0, [pc, #36]	; 59fc <tt_wire_check_valid+0xa0>
    59d4:	e1a02007 	mov	r2, r7
    59d8:	e59f3018 	ldr	r3, [pc, #24]	; 59f8 <tt_wire_check_valid+0x9c>
    59dc:	e1a0e00f 	mov	lr, pc
    59e0:	e593f000 	ldr	pc, [r3]
    59e4:	e1a00005 	mov	r0, r5
    59e8:	e28dd00c 	add	sp, sp, #12	; 0xc
    59ec:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    59f0:	7766dd80 	.word	0x7766dd80
    59f4:	00017923 	.word	0x00017923
    59f8:	3c00d6cc 	.word	0x3c00d6cc
    59fc:	0001794c 	.word	0x0001794c

00005a00 <do_tt_wire>:
    5a00:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    5a04:	e3520001 	cmp	r2, #1	; 0x1
    5a08:	e24dd01c 	sub	sp, sp, #28	; 0x1c
    5a0c:	e1a04003 	mov	r4, r3
    5a10:	d59f032c 	ldrle	r0, [pc, #812]	; 5d44 <do_tt_wire+0x344>
    5a14:	da00002b 	ble	5ac8 <do_tt_wire+0xc8>
    5a18:	e3a02003 	mov	r2, #3	; 0x3
    5a1c:	e5930004 	ldr	r0, [r3, #4]
    5a20:	e59f1320 	ldr	r1, [pc, #800]	; 5d48 <do_tt_wire+0x348>
    5a24:	ebffee93 	bl	1478 <strncmp>
    5a28:	e3500000 	cmp	r0, #0	; 0x0
    5a2c:	059f2318 	ldreq	r2, [pc, #792]	; 5d4c <do_tt_wire+0x34c>
    5a30:	0a000006 	beq	5a50 <do_tt_wire+0x50>
    5a34:	e5940004 	ldr	r0, [r4, #4]
    5a38:	e59f1310 	ldr	r1, [pc, #784]	; 5d50 <do_tt_wire+0x350>
    5a3c:	e3a02003 	mov	r2, #3	; 0x3
    5a40:	ebffee8c 	bl	1478 <strncmp>
    5a44:	e3500000 	cmp	r0, #0	; 0x0
    5a48:	1a000009 	bne	5a74 <do_tt_wire+0x74>
    5a4c:	e59f2300 	ldr	r2, [pc, #768]	; 5d54 <do_tt_wire+0x354>
    5a50:	e59f3300 	ldr	r3, [pc, #768]	; 5d58 <do_tt_wire+0x358>
    5a54:	e3a00000 	mov	r0, #0	; 0x0
    5a58:	e5832000 	str	r2, [r3]
    5a5c:	eb002004 	bl	da74 <get_timer>
    5a60:	e59f82f4 	ldr	r8, [pc, #756]	; 5d5c <do_tt_wire+0x35c>
    5a64:	e59f92ec 	ldr	r9, [pc, #748]	; 5d58 <do_tt_wire+0x358>
    5a68:	e1a0a008 	mov	sl, r8
    5a6c:	e58d000c 	str	r0, [sp, #12]
    5a70:	ea000006 	b	5a90 <do_tt_wire+0x90>
    5a74:	e5941004 	ldr	r1, [r4, #4]
    5a78:	e59f02e0 	ldr	r0, [pc, #736]	; 5d60 <do_tt_wire+0x360>
    5a7c:	e59f32d8 	ldr	r3, [pc, #728]	; 5d5c <do_tt_wire+0x35c>
    5a80:	e1a0e00f 	mov	lr, pc
    5a84:	e593f000 	ldr	pc, [r3]
    5a88:	e3e00000 	mvn	r0, #0	; 0x0
    5a8c:	ea0000aa 	b	5d3c <do_tt_wire+0x33c>
    5a90:	e59f02cc 	ldr	r0, [pc, #716]	; 5d64 <do_tt_wire+0x364>
    5a94:	eb001fd2 	bl	d9e4 <udelay>
    5a98:	e59f02c8 	ldr	r0, [pc, #712]	; 5d68 <do_tt_wire+0x368>
    5a9c:	e1a0e00f 	mov	lr, pc
    5aa0:	e598f000 	ldr	pc, [r8]
    5aa4:	e5993000 	ldr	r3, [r9]
    5aa8:	e1a0e00f 	mov	lr, pc
    5aac:	e593f000 	ldr	pc, [r3]
    5ab0:	e3500000 	cmp	r0, #0	; 0x0
    5ab4:	a3a07000 	movge	r7, #0	; 0x0
    5ab8:	a3a06020 	movge	r6, #32	; 0x20
    5abc:	a59fb2a8 	ldrge	fp, [pc, #680]	; 5d6c <do_tt_wire+0x36c>
    5ac0:	aa000004 	bge	5ad8 <do_tt_wire+0xd8>
    5ac4:	e59f02a4 	ldr	r0, [pc, #676]	; 5d70 <do_tt_wire+0x370>
    5ac8:	e59f328c 	ldr	r3, [pc, #652]	; 5d5c <do_tt_wire+0x35c>
    5acc:	e1a0e00f 	mov	lr, pc
    5ad0:	e593f000 	ldr	pc, [r3]
    5ad4:	eaffffeb 	b	5a88 <do_tt_wire+0x88>
    5ad8:	e3570000 	cmp	r7, #0	; 0x0
    5adc:	e1a01006 	mov	r1, r6
    5ae0:	e59f028c 	ldr	r0, [pc, #652]	; 5d74 <do_tt_wire+0x374>
    5ae4:	1a00002f 	bne	5ba8 <do_tt_wire+0x1a8>
    5ae8:	e59f0288 	ldr	r0, [pc, #648]	; 5d78 <do_tt_wire+0x378>
    5aec:	e1a0e00f 	mov	lr, pc
    5af0:	e598f000 	ldr	pc, [r8]
    5af4:	e1a00007 	mov	r0, r7
    5af8:	eb001fdd 	bl	da74 <get_timer>
    5afc:	e1a01006 	mov	r1, r6
    5b00:	e1a04000 	mov	r4, r0
    5b04:	e5993000 	ldr	r3, [r9]
    5b08:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5b0c:	e1a0e00f 	mov	lr, pc
    5b10:	e593f004 	ldr	pc, [r3, #4]
    5b14:	e1a05000 	mov	r5, r0
    5b18:	e1a00004 	mov	r0, r4
    5b1c:	eb001fd4 	bl	da74 <get_timer>
    5b20:	e3550000 	cmp	r5, #0	; 0x0
    5b24:	e1a04000 	mov	r4, r0
    5b28:	e1a01006 	mov	r1, r6
    5b2c:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5b30:	ba00003d 	blt	5c2c <do_tt_wire+0x22c>
    5b34:	ebffff88 	bl	595c <tt_wire_check_valid>
    5b38:	e3500000 	cmp	r0, #0	; 0x0
    5b3c:	e1a01004 	mov	r1, r4
    5b40:	e1a03546 	asr	r3, r6, #10
    5b44:	1a000007 	bne	5b68 <do_tt_wire+0x168>
    5b48:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    5b4c:	e0000390 	mul	r0, r0, r3
    5b50:	eb003b91 	bl	1499c <__aeabi_idiv>
    5b54:	e1a01000 	mov	r1, r0
    5b58:	e59f021c 	ldr	r0, [pc, #540]	; 5d7c <do_tt_wire+0x37c>
    5b5c:	e1a0e00f 	mov	lr, pc
    5b60:	e59af000 	ldr	pc, [sl]
    5b64:	eaffffc9 	b	5a90 <do_tt_wire+0x90>
    5b68:	e59fc210 	ldr	ip, [pc, #528]	; 5d80 <do_tt_wire+0x380>
    5b6c:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    5b70:	e0000392 	mul	r0, r2, r3
    5b74:	e89c000c 	ldm	ip, {r2, r3}
    5b78:	e0922006 	adds	r2, r2, r6
    5b7c:	e0a33fc6 	adc	r3, r3, r6, asr #31
    5b80:	e88c000c 	stm	ip, {r2, r3}
    5b84:	eb003b84 	bl	1499c <__aeabi_idiv>
    5b88:	e1a01000 	mov	r1, r0
    5b8c:	e59f01f0 	ldr	r0, [pc, #496]	; 5d84 <do_tt_wire+0x384>
    5b90:	e1a0e00f 	mov	lr, pc
    5b94:	e598f000 	ldr	pc, [r8]
    5b98:	e3a0348e 	mov	r3, #-1912602624	; 0x8e000000
    5b9c:	e5937008 	ldr	r7, [r3, #8]
    5ba0:	e593600c 	ldr	r6, [r3, #12]
    5ba4:	ea000044 	b	5cbc <do_tt_wire+0x2bc>
    5ba8:	e3570001 	cmp	r7, #1	; 0x1
    5bac:	1a000030 	bne	5c74 <do_tt_wire+0x274>
    5bb0:	e1a0e00f 	mov	lr, pc
    5bb4:	e598f000 	ldr	pc, [r8]
    5bb8:	e1a01007 	mov	r1, r7
    5bbc:	e3a00000 	mov	r0, #0	; 0x0
    5bc0:	ebfffafa 	bl	47b0 <tt_rnd_get_int>
    5bc4:	e3a01020 	mov	r1, #32	; 0x20
    5bc8:	e1a07000 	mov	r7, r0
    5bcc:	e3a02502 	mov	r2, #8388608	; 0x800000
    5bd0:	e28d0014 	add	r0, sp, #20	; 0x14
    5bd4:	ebfffb1e 	bl	4854 <tt_rnd_get_data>
    5bd8:	e59dc014 	ldr	ip, [sp, #20]
    5bdc:	e1a01000 	mov	r1, r0
    5be0:	e1a02006 	mov	r2, r6
    5be4:	e1a03007 	mov	r3, r7
    5be8:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5bec:	e58dc000 	str	ip, [sp]
    5bf0:	ebffff40 	bl	58f8 <tt_wire_make_data>
    5bf4:	e3a00000 	mov	r0, #0	; 0x0
    5bf8:	eb001f9d 	bl	da74 <get_timer>
    5bfc:	e1a01006 	mov	r1, r6
    5c00:	e1a05000 	mov	r5, r0
    5c04:	e5993000 	ldr	r3, [r9]
    5c08:	e3a0048e 	mov	r0, #-1912602624	; 0x8e000000
    5c0c:	e1a0e00f 	mov	lr, pc
    5c10:	e593f008 	ldr	pc, [r3, #8]
    5c14:	e1a04000 	mov	r4, r0
    5c18:	e1a00005 	mov	r0, r5
    5c1c:	eb001f94 	bl	da74 <get_timer>
    5c20:	e3540000 	cmp	r4, #0	; 0x0
    5c24:	e1a01000 	mov	r1, r0
    5c28:	aa000003 	bge	5c3c <do_tt_wire+0x23c>
    5c2c:	e59f0154 	ldr	r0, [pc, #340]	; 5d88 <do_tt_wire+0x388>
    5c30:	e1a0e00f 	mov	lr, pc
    5c34:	e59af000 	ldr	pc, [sl]
    5c38:	eaffff94 	b	5a90 <do_tt_wire+0x90>
    5c3c:	e89b000c 	ldm	fp, {r2, r3}
    5c40:	e0922006 	adds	r2, r2, r6
    5c44:	e0a33fc6 	adc	r3, r3, r6, asr #31
    5c48:	e59d6014 	ldr	r6, [sp, #20]
    5c4c:	e88b000c 	stm	fp, {r2, r3}
    5c50:	e1a00546 	asr	r0, r6, #10
    5c54:	e3a02ffa 	mov	r2, #1000	; 0x3e8
    5c58:	e0000092 	mul	r0, r2, r0
    5c5c:	eb003b4e 	bl	1499c <__aeabi_idiv>
    5c60:	e1a01000 	mov	r1, r0
    5c64:	e59f0120 	ldr	r0, [pc, #288]	; 5d8c <do_tt_wire+0x38c>
    5c68:	e1a0e00f 	mov	lr, pc
    5c6c:	e598f000 	ldr	pc, [r8]
    5c70:	ea000011 	b	5cbc <do_tt_wire+0x2bc>
    5c74:	e3570003 	cmp	r7, #3	; 0x3
    5c78:	1a000004 	bne	5c90 <do_tt_wire+0x290>
    5c7c:	e59f010c 	ldr	r0, [pc, #268]	; 5d90 <do_tt_wire+0x390>
    5c80:	e59f30d4 	ldr	r3, [pc, #212]	; 5d5c <do_tt_wire+0x35c>
    5c84:	e1a0e00f 	mov	lr, pc
    5c88:	e593f000 	ldr	pc, [r3]
    5c8c:	ea000010 	b	5cd4 <do_tt_wire+0x2d4>
    5c90:	e3570002 	cmp	r7, #2	; 0x2
    5c94:	1a000003 	bne	5ca8 <do_tt_wire+0x2a8>
    5c98:	e59f00f4 	ldr	r0, [pc, #244]	; 5d94 <do_tt_wire+0x394>
    5c9c:	e1a0e00f 	mov	lr, pc
    5ca0:	e598f000 	ldr	pc, [r8]
    5ca4:	eaffff79 	b	5a90 <do_tt_wire+0x90>
    5ca8:	e59f00e8 	ldr	r0, [pc, #232]	; 5d98 <do_tt_wire+0x398>
    5cac:	e59f30a8 	ldr	r3, [pc, #168]	; 5d5c <do_tt_wire+0x35c>
    5cb0:	e1a0e00f 	mov	lr, pc
    5cb4:	e593f000 	ldr	pc, [r3]
    5cb8:	eafffffe 	b	5cb8 <do_tt_wire+0x2b8>
    5cbc:	eb0032c7 	bl	127e0 <tstc>
    5cc0:	e3500000 	cmp	r0, #0	; 0x0
    5cc4:	0affff83 	beq	5ad8 <do_tt_wire+0xd8>
    5cc8:	eb0032c5 	bl	127e4 <getc>
    5ccc:	e3500003 	cmp	r0, #3	; 0x3
    5cd0:	1affff80 	bne	5ad8 <do_tt_wire+0xd8>
    5cd4:	e59d000c 	ldr	r0, [sp, #12]
    5cd8:	eb001f65 	bl	da74 <get_timer>
    5cdc:	e59f3088 	ldr	r3, [pc, #136]	; 5d6c <do_tt_wire+0x36c>
    5ce0:	e59f4074 	ldr	r4, [pc, #116]	; 5d5c <do_tt_wire+0x35c>
    5ce4:	e893000a 	ldm	r3, {r1, r3}
    5ce8:	e1a01a21 	lsr	r1, r1, #20
    5cec:	e1811603 	orr	r1, r1, r3, lsl #12
    5cf0:	e1a05000 	mov	r5, r0
    5cf4:	e59f00a0 	ldr	r0, [pc, #160]	; 5d9c <do_tt_wire+0x39c>
    5cf8:	e1a0e00f 	mov	lr, pc
    5cfc:	e594f000 	ldr	pc, [r4]
    5d00:	e59f3078 	ldr	r3, [pc, #120]	; 5d80 <do_tt_wire+0x380>
    5d04:	e59f0094 	ldr	r0, [pc, #148]	; 5da0 <do_tt_wire+0x3a0>
    5d08:	e893000a 	ldm	r3, {r1, r3}
    5d0c:	e1a01a21 	lsr	r1, r1, #20
    5d10:	e1811603 	orr	r1, r1, r3, lsl #12
    5d14:	e1a0e00f 	mov	lr, pc
    5d18:	e594f000 	ldr	pc, [r4]
    5d1c:	e30e1a60 	movw	r1, #60000	; 0xea60
    5d20:	e1a00005 	mov	r0, r5
    5d24:	eb003b1c 	bl	1499c <__aeabi_idiv>
    5d28:	e1a01000 	mov	r1, r0
    5d2c:	e59f0070 	ldr	r0, [pc, #112]	; 5da4 <do_tt_wire+0x3a4>
    5d30:	e1a0e00f 	mov	lr, pc
    5d34:	e594f000 	ldr	pc, [r4]
    5d38:	e3a00000 	mov	r0, #0	; 0x0
    5d3c:	e28dd01c 	add	sp, sp, #28	; 0x1c
    5d40:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    5d44:	00017975 	.word	0x00017975
    5d48:	000179be 	.word	0x000179be
    5d4c:	0001560c 	.word	0x0001560c
    5d50:	000179c2 	.word	0x000179c2
    5d54:	00015624 	.word	0x00015624
    5d58:	3c00d170 	.word	0x3c00d170
    5d5c:	3c00d6cc 	.word	0x3c00d6cc
    5d60:	000179c6 	.word	0x000179c6
    5d64:	001e8480 	.word	0x001e8480
    5d68:	000179e6 	.word	0x000179e6
    5d6c:	3c00d168 	.word	0x3c00d168
    5d70:	00017a08 	.word	0x00017a08
    5d74:	00017a75 	.word	0x00017a75
    5d78:	00017a2b 	.word	0x00017a2b
    5d7c:	00017a57 	.word	0x00017a57
    5d80:	3c00d160 	.word	0x3c00d160
    5d84:	00017a66 	.word	0x00017a66
    5d88:	00017a47 	.word	0x00017a47
    5d8c:	00017a91 	.word	0x00017a91
    5d90:	00017aa0 	.word	0x00017aa0
    5d94:	00017ab2 	.word	0x00017ab2
    5d98:	00017acb 	.word	0x00017acb
    5d9c:	00017af8 	.word	0x00017af8
    5da0:	00017b12 	.word	0x00017b12
    5da4:	00017b30 	.word	0x00017b30

00005da8 <vs_is_assigned>:
    5da8:	e59f300c 	ldr	r3, [pc, #12]	; 5dbc <vs_is_assigned+0x14>
    5dac:	e5930000 	ldr	r0, [r3]
    5db0:	e2500000 	subs	r0, r0, #0	; 0x0
    5db4:	13a00001 	movne	r0, #1	; 0x1
    5db8:	e12fff1e 	bx	lr
    5dbc:	3c00d178 	.word	0x3c00d178

00005dc0 <vs_is_opt_available>:
    5dc0:	e350000d 	cmp	r0, #13	; 0xd
    5dc4:	959f2078 	ldrls	r2, [pc, #120]	; 5e44 <vs_is_opt_available+0x84>
    5dc8:	93a0302c 	movls	r3, #44	; 0x2c
    5dcc:	859f3074 	ldrhi	r3, [pc, #116]	; 5e48 <vs_is_opt_available+0x88>
    5dd0:	90202093 	mlals	r0, r3, r0, r2
    5dd4:	85930000 	ldrhi	r0, [r3]
    5dd8:	e3500000 	cmp	r0, #0	; 0x0
    5ddc:	0a000016 	beq	5e3c <vs_is_opt_available+0x7c>
    5de0:	e3510005 	cmp	r1, #5	; 0x5
    5de4:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
    5de8:	ea000013 	b	5e3c <vs_is_opt_available+0x7c>
    5dec:	00005e04 	.word	0x00005e04
    5df0:	00005e0c 	.word	0x00005e0c
    5df4:	00005e14 	.word	0x00005e14
    5df8:	00005e1c 	.word	0x00005e1c
    5dfc:	00005e24 	.word	0x00005e24
    5e00:	00005e2c 	.word	0x00005e2c
    5e04:	e590300c 	ldr	r3, [r0, #12]
    5e08:	ea000008 	b	5e30 <vs_is_opt_available+0x70>
    5e0c:	e5903010 	ldr	r3, [r0, #16]
    5e10:	ea000006 	b	5e30 <vs_is_opt_available+0x70>
    5e14:	e5903014 	ldr	r3, [r0, #20]
    5e18:	ea000004 	b	5e30 <vs_is_opt_available+0x70>
    5e1c:	e5903018 	ldr	r3, [r0, #24]
    5e20:	ea000002 	b	5e30 <vs_is_opt_available+0x70>
    5e24:	e590301c 	ldr	r3, [r0, #28]
    5e28:	ea000000 	b	5e30 <vs_is_opt_available+0x70>
    5e2c:	e5903020 	ldr	r3, [r0, #32]
    5e30:	e2530000 	subs	r0, r3, #0	; 0x0
    5e34:	13a00001 	movne	r0, #1	; 0x1
    5e38:	e12fff1e 	bx	lr
    5e3c:	e3a00000 	mov	r0, #0	; 0x0
    5e40:	e12fff1e 	bx	lr
    5e44:	00015624 	.word	0x00015624
    5e48:	3c00d178 	.word	0x3c00d178

00005e4c <vs_write>:
    5e4c:	e92d4010 	push	{r4, lr}
    5e50:	e24dd008 	sub	sp, sp, #8	; 0x8
    5e54:	e59d4010 	ldr	r4, [sp, #16]
    5e58:	e3540000 	cmp	r4, #0	; 0x0
    5e5c:	01a00004 	moveq	r0, r4
    5e60:	0a00000c 	beq	5e98 <vs_write+0x4c>
    5e64:	e59f1034 	ldr	r1, [pc, #52]	; 5ea0 <vs_write+0x54>
    5e68:	e5911000 	ldr	r1, [r1]
    5e6c:	e3510000 	cmp	r1, #0	; 0x0
    5e70:	03e00001 	mvneq	r0, #1	; 0x1
    5e74:	0a000007 	beq	5e98 <vs_write+0x4c>
    5e78:	e591c014 	ldr	ip, [r1, #20]
    5e7c:	e35c0000 	cmp	ip, #0	; 0x0
    5e80:	03e00002 	mvneq	r0, #2	; 0x2
    5e84:	0a000003 	beq	5e98 <vs_write+0x4c>
    5e88:	e59d1014 	ldr	r1, [sp, #20]
    5e8c:	e58d4000 	str	r4, [sp]
    5e90:	e58d1004 	str	r1, [sp, #4]
    5e94:	e12fff3c 	blx	ip
    5e98:	e28dd008 	add	sp, sp, #8	; 0x8
    5e9c:	e8bd8010 	pop	{r4, pc}
    5ea0:	3c00d178 	.word	0x3c00d178

00005ea4 <vs_erase>:
    5ea4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5ea8:	e59f302c 	ldr	r3, [pc, #44]	; 5edc <vs_erase+0x38>
    5eac:	e24dd004 	sub	sp, sp, #4	; 0x4
    5eb0:	e5933000 	ldr	r3, [r3]
    5eb4:	e3530000 	cmp	r3, #0	; 0x0
    5eb8:	03e00001 	mvneq	r0, #1	; 0x1
    5ebc:	0a000004 	beq	5ed4 <vs_erase+0x30>
    5ec0:	e593301c 	ldr	r3, [r3, #28]
    5ec4:	e3530000 	cmp	r3, #0	; 0x0
    5ec8:	03e00002 	mvneq	r0, #2	; 0x2
    5ecc:	0a000000 	beq	5ed4 <vs_erase+0x30>
    5ed0:	e12fff33 	blx	r3
    5ed4:	e28dd004 	add	sp, sp, #4	; 0x4
    5ed8:	e8bd8000 	pop	{pc}
    5edc:	3c00d178 	.word	0x3c00d178

00005ee0 <vs_isbad>:
    5ee0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5ee4:	e59f302c 	ldr	r3, [pc, #44]	; 5f18 <vs_isbad+0x38>
    5ee8:	e24dd004 	sub	sp, sp, #4	; 0x4
    5eec:	e5933000 	ldr	r3, [r3]
    5ef0:	e3530000 	cmp	r3, #0	; 0x0
    5ef4:	03e00001 	mvneq	r0, #1	; 0x1
    5ef8:	0a000004 	beq	5f10 <vs_isbad+0x30>
    5efc:	e5933024 	ldr	r3, [r3, #36]
    5f00:	e3530000 	cmp	r3, #0	; 0x0
    5f04:	03e00002 	mvneq	r0, #2	; 0x2
    5f08:	0a000000 	beq	5f10 <vs_isbad+0x30>
    5f0c:	e12fff33 	blx	r3
    5f10:	e28dd004 	add	sp, sp, #4	; 0x4
    5f14:	e8bd8000 	pop	{pc}
    5f18:	3c00d178 	.word	0x3c00d178

00005f1c <vs_scrub>:
    5f1c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5f20:	e59f302c 	ldr	r3, [pc, #44]	; 5f54 <vs_scrub+0x38>
    5f24:	e24dd004 	sub	sp, sp, #4	; 0x4
    5f28:	e5933000 	ldr	r3, [r3]
    5f2c:	e3530000 	cmp	r3, #0	; 0x0
    5f30:	03e00001 	mvneq	r0, #1	; 0x1
    5f34:	0a000004 	beq	5f4c <vs_scrub+0x30>
    5f38:	e5933020 	ldr	r3, [r3, #32]
    5f3c:	e3530000 	cmp	r3, #0	; 0x0
    5f40:	03e00002 	mvneq	r0, #2	; 0x2
    5f44:	0a000000 	beq	5f4c <vs_scrub+0x30>
    5f48:	e12fff33 	blx	r3
    5f4c:	e28dd004 	add	sp, sp, #4	; 0x4
    5f50:	e8bd8000 	pop	{pc}
    5f54:	3c00d178 	.word	0x3c00d178

00005f58 <vs_special>:
    5f58:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5f5c:	e59f302c 	ldr	r3, [pc, #44]	; 5f90 <vs_special+0x38>
    5f60:	e24dd004 	sub	sp, sp, #4	; 0x4
    5f64:	e5933000 	ldr	r3, [r3]
    5f68:	e3530000 	cmp	r3, #0	; 0x0
    5f6c:	03e00001 	mvneq	r0, #1	; 0x1
    5f70:	0a000004 	beq	5f88 <vs_special+0x30>
    5f74:	e5933028 	ldr	r3, [r3, #40]
    5f78:	e3530000 	cmp	r3, #0	; 0x0
    5f7c:	03e00002 	mvneq	r0, #2	; 0x2
    5f80:	0a000000 	beq	5f88 <vs_special+0x30>
    5f84:	e12fff33 	blx	r3
    5f88:	e28dd004 	add	sp, sp, #4	; 0x4
    5f8c:	e8bd8000 	pop	{pc}
    5f90:	3c00d178 	.word	0x3c00d178

00005f94 <vs_align>:
    5f94:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    5f98:	e350000e 	cmp	r0, #14	; 0xe
    5f9c:	059f3044 	ldreq	r3, [pc, #68]	; 5fe8 <vs_align+0x54>
    5fa0:	e24dd004 	sub	sp, sp, #4	; 0x4
    5fa4:	05930000 	ldreq	r0, [r3]
    5fa8:	0a000004 	beq	5fc0 <vs_align+0x2c>
    5fac:	e350000d 	cmp	r0, #13	; 0xd
    5fb0:	8a000009 	bhi	5fdc <vs_align+0x48>
    5fb4:	e59f2030 	ldr	r2, [pc, #48]	; 5fec <vs_align+0x58>
    5fb8:	e3a0302c 	mov	r3, #44	; 0x2c
    5fbc:	e0202093 	mla	r0, r3, r0, r2
    5fc0:	e3500000 	cmp	r0, #0	; 0x0
    5fc4:	0a000004 	beq	5fdc <vs_align+0x48>
    5fc8:	e590000c 	ldr	r0, [r0, #12]
    5fcc:	e3500000 	cmp	r0, #0	; 0x0
    5fd0:	0a000001 	beq	5fdc <vs_align+0x48>
    5fd4:	e12fff30 	blx	r0
    5fd8:	ea000000 	b	5fe0 <vs_align+0x4c>
    5fdc:	e3a00000 	mov	r0, #0	; 0x0
    5fe0:	e28dd004 	add	sp, sp, #4	; 0x4
    5fe4:	e8bd8000 	pop	{pc}
    5fe8:	3c00d178 	.word	0x3c00d178
    5fec:	00015624 	.word	0x00015624

00005ff0 <vs_device_bootable>:
    5ff0:	e350000e 	cmp	r0, #14	; 0xe
    5ff4:	059f3030 	ldreq	r3, [pc, #48]	; 602c <vs_device_bootable+0x3c>
    5ff8:	05930000 	ldreq	r0, [r3]
    5ffc:	0a000004 	beq	6014 <vs_device_bootable+0x24>
    6000:	e350000d 	cmp	r0, #13	; 0xd
    6004:	8a000006 	bhi	6024 <vs_device_bootable+0x34>
    6008:	e59f2020 	ldr	r2, [pc, #32]	; 6030 <vs_device_bootable+0x40>
    600c:	e3a0302c 	mov	r3, #44	; 0x2c
    6010:	e0202093 	mla	r0, r3, r0, r2
    6014:	e3500000 	cmp	r0, #0	; 0x0
    6018:	15903008 	ldrne	r3, [r0, #8]
    601c:	12030001 	andne	r0, r3, #1	; 0x1
    6020:	112fff1e 	bxne	lr
    6024:	e3a00000 	mov	r0, #0	; 0x0
    6028:	e12fff1e 	bx	lr
    602c:	3c00d178 	.word	0x3c00d178
    6030:	00015624 	.word	0x00015624

00006034 <vs_device_burnsrc>:
    6034:	e350000e 	cmp	r0, #14	; 0xe
    6038:	059f3034 	ldreq	r3, [pc, #52]	; 6074 <vs_device_burnsrc+0x40>
    603c:	05930000 	ldreq	r0, [r3]
    6040:	0a000004 	beq	6058 <vs_device_burnsrc+0x24>
    6044:	e350000d 	cmp	r0, #13	; 0xd
    6048:	8a000007 	bhi	606c <vs_device_burnsrc+0x38>
    604c:	e59f2024 	ldr	r2, [pc, #36]	; 6078 <vs_device_burnsrc+0x44>
    6050:	e3a0302c 	mov	r3, #44	; 0x2c
    6054:	e0202093 	mla	r0, r3, r0, r2
    6058:	e3500000 	cmp	r0, #0	; 0x0
    605c:	15903008 	ldrne	r3, [r0, #8]
    6060:	11a030a3 	lsrne	r3, r3, #1
    6064:	12030001 	andne	r0, r3, #1	; 0x1
    6068:	112fff1e 	bxne	lr
    606c:	e3a00000 	mov	r0, #0	; 0x0
    6070:	e12fff1e 	bx	lr
    6074:	3c00d178 	.word	0x3c00d178
    6078:	00015624 	.word	0x00015624

0000607c <vs_device_burndst>:
    607c:	e350000e 	cmp	r0, #14	; 0xe
    6080:	059f3034 	ldreq	r3, [pc, #52]	; 60bc <vs_device_burndst+0x40>
    6084:	05930000 	ldreq	r0, [r3]
    6088:	0a000004 	beq	60a0 <vs_device_burndst+0x24>
    608c:	e350000d 	cmp	r0, #13	; 0xd
    6090:	8a000007 	bhi	60b4 <vs_device_burndst+0x38>
    6094:	e59f2024 	ldr	r2, [pc, #36]	; 60c0 <vs_device_burndst+0x44>
    6098:	e3a0302c 	mov	r3, #44	; 0x2c
    609c:	e0202093 	mla	r0, r3, r0, r2
    60a0:	e3500000 	cmp	r0, #0	; 0x0
    60a4:	15903008 	ldrne	r3, [r0, #8]
    60a8:	11a03123 	lsrne	r3, r3, #2
    60ac:	12030001 	andne	r0, r3, #1	; 0x1
    60b0:	112fff1e 	bxne	lr
    60b4:	e3a00000 	mov	r0, #0	; 0x0
    60b8:	e12fff1e 	bx	lr
    60bc:	3c00d178 	.word	0x3c00d178
    60c0:	00015624 	.word	0x00015624

000060c4 <vs_device_boot0>:
    60c4:	e350000e 	cmp	r0, #14	; 0xe
    60c8:	059f3034 	ldreq	r3, [pc, #52]	; 6104 <vs_device_boot0+0x40>
    60cc:	05930000 	ldreq	r0, [r3]
    60d0:	0a000004 	beq	60e8 <vs_device_boot0+0x24>
    60d4:	e350000d 	cmp	r0, #13	; 0xd
    60d8:	8a000007 	bhi	60fc <vs_device_boot0+0x38>
    60dc:	e59f2024 	ldr	r2, [pc, #36]	; 6108 <vs_device_boot0+0x44>
    60e0:	e3a0302c 	mov	r3, #44	; 0x2c
    60e4:	e0202093 	mla	r0, r3, r0, r2
    60e8:	e3500000 	cmp	r0, #0	; 0x0
    60ec:	15903008 	ldrne	r3, [r0, #8]
    60f0:	11a031a3 	lsrne	r3, r3, #3
    60f4:	12030001 	andne	r0, r3, #1	; 0x1
    60f8:	112fff1e 	bxne	lr
    60fc:	e3a00000 	mov	r0, #0	; 0x0
    6100:	e12fff1e 	bx	lr
    6104:	3c00d178 	.word	0x3c00d178
    6108:	00015624 	.word	0x00015624

0000610c <vs_device_name>:
    610c:	e350000e 	cmp	r0, #14	; 0xe
    6110:	059f3028 	ldreq	r3, [pc, #40]	; 6140 <vs_device_name+0x34>
    6114:	05930000 	ldreq	r0, [r3]
    6118:	0a000004 	beq	6130 <vs_device_name+0x24>
    611c:	e350000d 	cmp	r0, #13	; 0xd
    6120:	8a000004 	bhi	6138 <vs_device_name+0x2c>
    6124:	e59f2018 	ldr	r2, [pc, #24]	; 6144 <vs_device_name+0x38>
    6128:	e3a0302c 	mov	r3, #44	; 0x2c
    612c:	e0202093 	mla	r0, r3, r0, r2
    6130:	e3500000 	cmp	r0, #0	; 0x0
    6134:	112fff1e 	bxne	lr
    6138:	e3a00000 	mov	r0, #0	; 0x0
    613c:	e12fff1e 	bx	lr
    6140:	3c00d178 	.word	0x3c00d178
    6144:	00015624 	.word	0x00015624

00006148 <vs_device_erasable>:
    6148:	e350000e 	cmp	r0, #14	; 0xe
    614c:	059f3034 	ldreq	r3, [pc, #52]	; 6188 <vs_device_erasable+0x40>
    6150:	05930000 	ldreq	r0, [r3]
    6154:	0a000004 	beq	616c <vs_device_erasable+0x24>
    6158:	e350000d 	cmp	r0, #13	; 0xd
    615c:	8a000007 	bhi	6180 <vs_device_erasable+0x38>
    6160:	e59f2024 	ldr	r2, [pc, #36]	; 618c <vs_device_erasable+0x44>
    6164:	e3a0302c 	mov	r3, #44	; 0x2c
    6168:	e0202093 	mla	r0, r3, r0, r2
    616c:	e3500000 	cmp	r0, #0	; 0x0
    6170:	15903008 	ldrne	r3, [r0, #8]
    6174:	11a03223 	lsrne	r3, r3, #4
    6178:	12030001 	andne	r0, r3, #1	; 0x1
    617c:	112fff1e 	bxne	lr
    6180:	e3a00000 	mov	r0, #0	; 0x0
    6184:	e12fff1e 	bx	lr
    6188:	3c00d178 	.word	0x3c00d178
    618c:	00015624 	.word	0x00015624

00006190 <vs_device_count>:
    6190:	e3a0000e 	mov	r0, #14	; 0xe
    6194:	e12fff1e 	bx	lr

00006198 <vs_device_id>:
    6198:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    619c:	e59f7040 	ldr	r7, [pc, #64]	; 61e4 <vs_device_id+0x4c>
    61a0:	e24dd004 	sub	sp, sp, #4	; 0x4
    61a4:	e1a05000 	mov	r5, r0
    61a8:	e3a04000 	mov	r4, #0	; 0x0
    61ac:	e3a0602c 	mov	r6, #44	; 0x2c
    61b0:	e0217496 	mla	r1, r6, r4, r7
    61b4:	e1a00005 	mov	r0, r5
    61b8:	e3a02010 	mov	r2, #16	; 0x10
    61bc:	ebffecad 	bl	1478 <strncmp>
    61c0:	e3500000 	cmp	r0, #0	; 0x0
    61c4:	0a000003 	beq	61d8 <vs_device_id+0x40>
    61c8:	e2844001 	add	r4, r4, #1	; 0x1
    61cc:	e354000e 	cmp	r4, #14	; 0xe
    61d0:	1afffff6 	bne	61b0 <vs_device_id+0x18>
    61d4:	e2844001 	add	r4, r4, #1	; 0x1
    61d8:	e1a00004 	mov	r0, r4
    61dc:	e28dd004 	add	sp, sp, #4	; 0x4
    61e0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    61e4:	00015624 	.word	0x00015624

000061e8 <vs_is_device>:
    61e8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    61ec:	e350000e 	cmp	r0, #14	; 0xe
    61f0:	059f304c 	ldreq	r3, [pc, #76]	; 6244 <vs_is_device+0x5c>
    61f4:	e1a0c001 	mov	ip, r1
    61f8:	e24dd004 	sub	sp, sp, #4	; 0x4
    61fc:	05931000 	ldreq	r1, [r3]
    6200:	0a000004 	beq	6218 <vs_is_device+0x30>
    6204:	e350000d 	cmp	r0, #13	; 0xd
    6208:	8a00000a 	bhi	6238 <vs_is_device+0x50>
    620c:	e59f2034 	ldr	r2, [pc, #52]	; 6248 <vs_is_device+0x60>
    6210:	e3a0302c 	mov	r3, #44	; 0x2c
    6214:	e0212093 	mla	r1, r3, r0, r2
    6218:	e3510000 	cmp	r1, #0	; 0x0
    621c:	0a000005 	beq	6238 <vs_is_device+0x50>
    6220:	e1a0000c 	mov	r0, ip
    6224:	e3a02010 	mov	r2, #16	; 0x10
    6228:	ebffec92 	bl	1478 <strncmp>
    622c:	e2700001 	rsbs	r0, r0, #1	; 0x1
    6230:	33a00000 	movcc	r0, #0	; 0x0
    6234:	ea000000 	b	623c <vs_is_device+0x54>
    6238:	e3a00000 	mov	r0, #0	; 0x0
    623c:	e28dd004 	add	sp, sp, #4	; 0x4
    6240:	e8bd8000 	pop	{pc}
    6244:	3c00d178 	.word	0x3c00d178
    6248:	00015624 	.word	0x00015624

0000624c <vs_read>:
    624c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    6250:	e59f4234 	ldr	r4, [pc, #564]	; 648c <vs_read+0x240>
    6254:	e1a09000 	mov	r9, r0
    6258:	e5940000 	ldr	r0, [r4]
    625c:	e24dd024 	sub	sp, sp, #36	; 0x24
    6260:	e3500000 	cmp	r0, #0	; 0x0
    6264:	e1a06002 	mov	r6, r2
    6268:	e1a07003 	mov	r7, r3
    626c:	e59d5048 	ldr	r5, [sp, #72]
    6270:	03e04001 	mvneq	r4, #1	; 0x1
    6274:	0a000081 	beq	6480 <vs_read+0x234>
    6278:	e5903010 	ldr	r3, [r0, #16]
    627c:	e3530000 	cmp	r3, #0	; 0x0
    6280:	03e04002 	mvneq	r4, #2	; 0x2
    6284:	0a00007d 	beq	6480 <vs_read+0x234>
    6288:	e3550000 	cmp	r5, #0	; 0x0
    628c:	01a04005 	moveq	r4, r5
    6290:	0a00007a 	beq	6480 <vs_read+0x234>
    6294:	e59f11f4 	ldr	r1, [pc, #500]	; 6490 <vs_read+0x244>
    6298:	e3a02003 	mov	r2, #3	; 0x3
    629c:	ebffec75 	bl	1478 <strncmp>
    62a0:	e3500000 	cmp	r0, #0	; 0x0
    62a4:	1a00000a 	bne	62d4 <vs_read+0x88>
    62a8:	e59d204c 	ldr	r2, [sp, #76]
    62ac:	e58d5000 	str	r5, [sp]
    62b0:	e58d2004 	str	r2, [sp, #4]
    62b4:	e5941000 	ldr	r1, [r4]
    62b8:	e1a00009 	mov	r0, r9
    62bc:	e1a02006 	mov	r2, r6
    62c0:	e1a03007 	mov	r3, r7
    62c4:	e1a0e00f 	mov	lr, pc
    62c8:	e591f010 	ldr	pc, [r1, #16]
    62cc:	e1a04000 	mov	r4, r0
    62d0:	ea00006a 	b	6480 <vs_read+0x234>
    62d4:	e3a0000e 	mov	r0, #14	; 0xe
    62d8:	ebffff2d 	bl	5f94 <vs_align>
    62dc:	e2402001 	sub	r2, r0, #1	; 0x1
    62e0:	e0124006 	ands	r4, r2, r6
    62e4:	10644000 	rsbne	r4, r4, r0
    62e8:	e0643005 	rsb	r3, r4, r5
    62ec:	e0032002 	and	r2, r3, r2
    62f0:	e192a004 	orrs	sl, r2, r4
    62f4:	e1a08000 	mov	r8, r0
    62f8:	e58d3014 	str	r3, [sp, #20]
    62fc:	e58d200c 	str	r2, [sp, #12]
    6300:	e58da018 	str	sl, [sp, #24]
    6304:	01a0400a 	moveq	r4, sl
    6308:	058da01c 	streq	sl, [sp, #28]
    630c:	0a00002c 	beq	63c4 <vs_read+0x178>
    6310:	ebffecf6 	bl	16f0 <malloc>
    6314:	e250b000 	subs	fp, r0, #0	; 0x0
    6318:	e58db01c 	str	fp, [sp, #28]
    631c:	1a00000c 	bne	6354 <vs_read+0x108>
    6320:	e59f416c 	ldr	r4, [pc, #364]	; 6494 <vs_read+0x248>
    6324:	e59f016c 	ldr	r0, [pc, #364]	; 6498 <vs_read+0x24c>
    6328:	e3001115 	movw	r1, #277	; 0x115
    632c:	e1a0e00f 	mov	lr, pc
    6330:	e594f000 	ldr	pc, [r4]
    6334:	e59f0160 	ldr	r0, [pc, #352]	; 649c <vs_read+0x250>
    6338:	e59f1160 	ldr	r1, [pc, #352]	; 64a0 <vs_read+0x254>
    633c:	e1a0e00f 	mov	lr, pc
    6340:	e594f000 	ldr	pc, [r4]
    6344:	e59f0158 	ldr	r0, [pc, #344]	; 64a4 <vs_read+0x258>
    6348:	e1a0e00f 	mov	lr, pc
    634c:	e594f000 	ldr	pc, [r4]
    6350:	eafffffe 	b	6350 <vs_read+0x104>
    6354:	e3540000 	cmp	r4, #0	; 0x0
    6358:	0a000019 	beq	63c4 <vs_read+0x178>
    635c:	e59f2128 	ldr	r2, [pc, #296]	; 648c <vs_read+0x240>
    6360:	e2683000 	rsb	r3, r8, #0	; 0x0
    6364:	e5921000 	ldr	r1, [r2]
    6368:	e59d204c 	ldr	r2, [sp, #76]
    636c:	e006a003 	and	sl, r6, r3
    6370:	e3a0b000 	mov	fp, #0	; 0x0
    6374:	e58d2004 	str	r2, [sp, #4]
    6378:	e1a0300b 	mov	r3, fp
    637c:	e58d8000 	str	r8, [sp]
    6380:	e1a0200a 	mov	r2, sl
    6384:	e1a0e00f 	mov	lr, pc
    6388:	e591f010 	ldr	pc, [r1, #16]
    638c:	e250b000 	subs	fp, r0, #0	; 0x0
    6390:	b1a0400b 	movlt	r4, fp
    6394:	ba000037 	blt	6478 <vs_read+0x22c>
    6398:	e59d201c 	ldr	r2, [sp, #28]
    639c:	e0641008 	rsb	r1, r4, r8
    63a0:	e0821001 	add	r1, r2, r1
    63a4:	e1a00009 	mov	r0, r9
    63a8:	e1a02004 	mov	r2, r4
    63ac:	ebffec6a 	bl	155c <memcpy>
    63b0:	e0966004 	adds	r6, r6, r4
    63b4:	e59d5014 	ldr	r5, [sp, #20]
    63b8:	e0899004 	add	r9, r9, r4
    63bc:	e2a77000 	adc	r7, r7, #0	; 0x0
    63c0:	e1a0400b 	mov	r4, fp
    63c4:	e59d300c 	ldr	r3, [sp, #12]
    63c8:	e0555003 	subs	r5, r5, r3
    63cc:	0a00000f 	beq	6410 <vs_read+0x1c4>
    63d0:	e59f30b4 	ldr	r3, [pc, #180]	; 648c <vs_read+0x240>
    63d4:	e59da04c 	ldr	sl, [sp, #76]
    63d8:	e58d5000 	str	r5, [sp]
    63dc:	e5931000 	ldr	r1, [r3]
    63e0:	e1a00009 	mov	r0, r9
    63e4:	e58da004 	str	sl, [sp, #4]
    63e8:	e1a02006 	mov	r2, r6
    63ec:	e1a03007 	mov	r3, r7
    63f0:	e1a0e00f 	mov	lr, pc
    63f4:	e591f010 	ldr	pc, [r1, #16]
    63f8:	e3500000 	cmp	r0, #0	; 0x0
    63fc:	ba000012 	blt	644c <vs_read+0x200>
    6400:	e0966005 	adds	r6, r6, r5
    6404:	e0a77fc5 	adc	r7, r7, r5, asr #31
    6408:	e0844000 	add	r4, r4, r0
    640c:	e0899005 	add	r9, r9, r5
    6410:	e59db00c 	ldr	fp, [sp, #12]
    6414:	e35b0000 	cmp	fp, #0	; 0x0
    6418:	0a000013 	beq	646c <vs_read+0x220>
    641c:	e59d204c 	ldr	r2, [sp, #76]
    6420:	e59f3064 	ldr	r3, [pc, #100]	; 648c <vs_read+0x240>
    6424:	e58d2004 	str	r2, [sp, #4]
    6428:	e58d8000 	str	r8, [sp]
    642c:	e5931000 	ldr	r1, [r3]
    6430:	e1a02006 	mov	r2, r6
    6434:	e1a03007 	mov	r3, r7
    6438:	e59d001c 	ldr	r0, [sp, #28]
    643c:	e1a0e00f 	mov	lr, pc
    6440:	e591f010 	ldr	pc, [r1, #16]
    6444:	e3500000 	cmp	r0, #0	; 0x0
    6448:	aa000001 	bge	6454 <vs_read+0x208>
    644c:	e1a04000 	mov	r4, r0
    6450:	ea000005 	b	646c <vs_read+0x220>
    6454:	e1a00009 	mov	r0, r9
    6458:	e59d101c 	ldr	r1, [sp, #28]
    645c:	e59d200c 	ldr	r2, [sp, #12]
    6460:	ebffec3d 	bl	155c <memcpy>
    6464:	e59d300c 	ldr	r3, [sp, #12]
    6468:	e0844003 	add	r4, r4, r3
    646c:	e59da018 	ldr	sl, [sp, #24]
    6470:	e35a0000 	cmp	sl, #0	; 0x0
    6474:	0a000001 	beq	6480 <vs_read+0x234>
    6478:	e59d001c 	ldr	r0, [sp, #28]
    647c:	ebffed03 	bl	1890 <free>
    6480:	e1a00004 	mov	r0, r4
    6484:	e28dd024 	add	sp, sp, #36	; 0x24
    6488:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    648c:	3c00d178 	.word	0x3c00d178
    6490:	0001626b 	.word	0x0001626b
    6494:	3c00d6cc 	.word	0x3c00d6cc
    6498:	00017b59 	.word	0x00017b59
    649c:	0001641f 	.word	0x0001641f
    64a0:	00017b6b 	.word	0x00017b6b
    64a4:	00017b76 	.word	0x00017b76

000064a8 <vs_reset>:
    64a8:	e92d4010 	push	{r4, lr}
    64ac:	e59f402c 	ldr	r4, [pc, #44]	; 64e0 <vs_reset+0x38>
    64b0:	e5940000 	ldr	r0, [r4]
    64b4:	ebffeab5 	bl	f90 <cpu_recase>
    64b8:	e5944000 	ldr	r4, [r4]
    64bc:	e3540000 	cmp	r4, #0	; 0x0
    64c0:	03e00001 	mvneq	r0, #1	; 0x1
    64c4:	08bd8010 	popeq	{r4, pc}
    64c8:	e5944018 	ldr	r4, [r4, #24]
    64cc:	e3540000 	cmp	r4, #0	; 0x0
    64d0:	03e00002 	mvneq	r0, #2	; 0x2
    64d4:	08bd8010 	popeq	{r4, pc}
    64d8:	e12fff34 	blx	r4
    64dc:	e8bd8010 	pop	{r4, pc}
    64e0:	3c00d178 	.word	0x3c00d178

000064e4 <vs_assign_by_id>:
    64e4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    64e8:	e59fc05c 	ldr	ip, [pc, #92]	; 654c <vs_assign_by_id+0x68>
    64ec:	e3a03000 	mov	r3, #0	; 0x0
    64f0:	e350000d 	cmp	r0, #13	; 0xd
    64f4:	e24dd004 	sub	sp, sp, #4	; 0x4
    64f8:	e58c3000 	str	r3, [ip]
    64fc:	8a00000b 	bhi	6530 <vs_assign_by_id+0x4c>
    6500:	e59f2048 	ldr	r2, [pc, #72]	; 6550 <vs_assign_by_id+0x6c>
    6504:	e283302c 	add	r3, r3, #44	; 0x2c
    6508:	e0232093 	mla	r3, r3, r0, r2
    650c:	e3510000 	cmp	r1, #0	; 0x0
    6510:	e58c3000 	str	r3, [ip]
    6514:	01a00001 	moveq	r0, r1
    6518:	0a000002 	beq	6528 <vs_assign_by_id+0x44>
    651c:	e28dd004 	add	sp, sp, #4	; 0x4
    6520:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    6524:	eaffffdf 	b	64a8 <vs_reset>
    6528:	e28dd004 	add	sp, sp, #4	; 0x4
    652c:	e8bd8000 	pop	{pc}
    6530:	e1a01000 	mov	r1, r0
    6534:	e59f3018 	ldr	r3, [pc, #24]	; 6554 <vs_assign_by_id+0x70>
    6538:	e59f0018 	ldr	r0, [pc, #24]	; 6558 <vs_assign_by_id+0x74>
    653c:	e1a0e00f 	mov	lr, pc
    6540:	e593f000 	ldr	pc, [r3]
    6544:	e3e00000 	mvn	r0, #0	; 0x0
    6548:	eafffff6 	b	6528 <vs_assign_by_id+0x44>
    654c:	3c00d178 	.word	0x3c00d178
    6550:	00015624 	.word	0x00015624
    6554:	3c00d6cc 	.word	0x3c00d6cc
    6558:	00017ba7 	.word	0x00017ba7

0000655c <vs_assign_by_name>:
    655c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    6560:	e59f3084 	ldr	r3, [pc, #132]	; 65ec <vs_assign_by_name+0x90>
    6564:	e3a02000 	mov	r2, #0	; 0x0
    6568:	e24dd004 	sub	sp, sp, #4	; 0x4
    656c:	e5832000 	str	r2, [r3]
    6570:	e1a06000 	mov	r6, r0
    6574:	e1a07001 	mov	r7, r1
    6578:	e1a04002 	mov	r4, r2
    657c:	e3a0a02c 	mov	sl, #44	; 0x2c
    6580:	e59f8068 	ldr	r8, [pc, #104]	; 65f0 <vs_assign_by_name+0x94>
    6584:	ea00000d 	b	65c0 <vs_assign_by_name+0x64>
    6588:	e025849a 	mla	r5, sl, r4, r8
    658c:	e1a00005 	mov	r0, r5
    6590:	ebffebb8 	bl	1478 <strncmp>
    6594:	e3500000 	cmp	r0, #0	; 0x0
    6598:	1a000007 	bne	65bc <vs_assign_by_name+0x60>
    659c:	e59f3048 	ldr	r3, [pc, #72]	; 65ec <vs_assign_by_name+0x90>
    65a0:	e3570000 	cmp	r7, #0	; 0x0
    65a4:	e5835000 	str	r5, [r3]
    65a8:	01a00007 	moveq	r0, r7
    65ac:	0a00000c 	beq	65e4 <vs_assign_by_name+0x88>
    65b0:	e28dd004 	add	sp, sp, #4	; 0x4
    65b4:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    65b8:	eaffffba 	b	64a8 <vs_reset>
    65bc:	e2844001 	add	r4, r4, #1	; 0x1
    65c0:	e354000e 	cmp	r4, #14	; 0xe
    65c4:	e1a01006 	mov	r1, r6
    65c8:	e3a02010 	mov	r2, #16	; 0x10
    65cc:	1affffed 	bne	6588 <vs_assign_by_name+0x2c>
    65d0:	e59f001c 	ldr	r0, [pc, #28]	; 65f4 <vs_assign_by_name+0x98>
    65d4:	e59f301c 	ldr	r3, [pc, #28]	; 65f8 <vs_assign_by_name+0x9c>
    65d8:	e1a0e00f 	mov	lr, pc
    65dc:	e593f000 	ldr	pc, [r3]
    65e0:	e3e00000 	mvn	r0, #0	; 0x0
    65e4:	e28dd004 	add	sp, sp, #4	; 0x4
    65e8:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    65ec:	3c00d178 	.word	0x3c00d178
    65f0:	00015624 	.word	0x00015624
    65f4:	00017bc0 	.word	0x00017bc0
    65f8:	3c00d6cc 	.word	0x3c00d6cc

000065fc <ss_lock_isk>:
    65fc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6600:	e24dd004 	sub	sp, sp, #4	; 0x4
    6604:	eb0001d5 	bl	6d60 <security_enabled>
    6608:	e3500000 	cmp	r0, #0	; 0x0
    660c:	0a000002 	beq	661c <ss_lock_isk+0x20>
    6610:	e28dd004 	add	sp, sp, #4	; 0x4
    6614:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    6618:	ea00002e 	b	66d8 <block_cipher_lock_isk>
    661c:	e28dd004 	add	sp, sp, #4	; 0x4
    6620:	e8bd8000 	pop	{pc}

00006624 <ss_aes_data>:
    6624:	e92d4070 	push	{r4, r5, r6, lr}
    6628:	e1a05000 	mov	r5, r0
    662c:	e1a06001 	mov	r6, r1
    6630:	e1a04002 	mov	r4, r2
    6634:	eb0001c9 	bl	6d60 <security_enabled>
    6638:	e3500000 	cmp	r0, #0	; 0x0
    663c:	0a000007 	beq	6660 <ss_aes_data+0x3c>
    6640:	e284200f 	add	r2, r4, #15	; 0xf
    6644:	e1a00005 	mov	r0, r5
    6648:	e1a01006 	mov	r1, r6
    664c:	e3c2200f 	bic	r2, r2, #15	; 0xf
    6650:	eb0000a0 	bl	68d8 <block_cipher_operation>
    6654:	e3500000 	cmp	r0, #0	; 0x0
    6658:	13e00000 	mvnne	r0, #0	; 0x0
    665c:	18bd8070 	popne	{r4, r5, r6, pc}
    6660:	e3a00000 	mov	r0, #0	; 0x0
    6664:	e8bd8070 	pop	{r4, r5, r6, pc}

00006668 <ss_aes_init>:
    6668:	e92d4070 	push	{r4, r5, r6, lr}
    666c:	e1a05000 	mov	r5, r0
    6670:	e1a04001 	mov	r4, r1
    6674:	e1a06002 	mov	r6, r2
    6678:	eb0001b8 	bl	6d60 <security_enabled>
    667c:	e3500000 	cmp	r0, #0	; 0x0
    6680:	0a00000b 	beq	66b4 <ss_aes_init+0x4c>
    6684:	e3540000 	cmp	r4, #0	; 0x0
    6688:	01a00005 	moveq	r0, r5
    668c:	01a02004 	moveq	r2, r4
    6690:	01a03006 	moveq	r3, r6
    6694:	03a01001 	moveq	r1, #1	; 0x1
    6698:	11a00005 	movne	r0, r5
    669c:	11a02004 	movne	r2, r4
    66a0:	11a03006 	movne	r3, r6
    66a4:	13a01000 	movne	r1, #0	; 0x0
    66a8:	eb00002c 	bl	6760 <block_cipher_aes_init>
    66ac:	e3500000 	cmp	r0, #0	; 0x0
    66b0:	1a000001 	bne	66bc <ss_aes_init+0x54>
    66b4:	e3a00000 	mov	r0, #0	; 0x0
    66b8:	e8bd8070 	pop	{r4, r5, r6, pc}
    66bc:	e3e00000 	mvn	r0, #0	; 0x0
    66c0:	e8bd8070 	pop	{r4, r5, r6, pc}

000066c4 <check_register_bit>:
    66c4:	e3a03001 	mov	r3, #1	; 0x1
    66c8:	e0103113 	ands	r3, r0, r3, lsl r1
    66cc:	03a00000 	moveq	r0, #0	; 0x0
    66d0:	13a00001 	movne	r0, #1	; 0x1
    66d4:	e12fff1e 	bx	lr

000066d8 <block_cipher_lock_isk>:
    66d8:	e59f200c 	ldr	r2, [pc, #12]	; 66ec <block_cipher_lock_isk+0x14>
    66dc:	e5923400 	ldr	r3, [r2, #1024]
    66e0:	e3833706 	orr	r3, r3, #1572864	; 0x180000
    66e4:	e5823400 	str	r3, [r2, #1024]
    66e8:	e12fff1e 	bx	lr
    66ec:	23200000 	.word	0x23200000

000066f0 <block_cipher_reset>:
    66f0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    66f4:	e59f4060 	ldr	r4, [pc, #96]	; 675c <block_cipher_reset+0x6c>
    66f8:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    66fc:	e5843400 	str	r3, [r4, #1024]
    6700:	e24dd004 	sub	sp, sp, #4	; 0x4
    6704:	eb001ca5 	bl	d9a0 <get_ticks>
    6708:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    670c:	e3a07000 	mov	r7, #0	; 0x0
    6710:	e0966000 	adds	r6, r6, r0
    6714:	e1a05004 	mov	r5, r4
    6718:	e0a77001 	adc	r7, r7, r1
    671c:	e5944400 	ldr	r4, [r4, #1024]
    6720:	ea000008 	b	6748 <block_cipher_reset+0x58>
    6724:	e5954400 	ldr	r4, [r5, #1024]
    6728:	eb001c9c 	bl	d9a0 <get_ticks>
    672c:	e1510007 	cmp	r1, r7
    6730:	8a000002 	bhi	6740 <block_cipher_reset+0x50>
    6734:	1a000003 	bne	6748 <block_cipher_reset+0x58>
    6738:	e1500006 	cmp	r0, r6
    673c:	9a000001 	bls	6748 <block_cipher_reset+0x58>
    6740:	e3a00001 	mov	r0, #1	; 0x1
    6744:	ea000002 	b	6754 <block_cipher_reset+0x64>
    6748:	e3540000 	cmp	r4, #0	; 0x0
    674c:	bafffff4 	blt	6724 <block_cipher_reset+0x34>
    6750:	e3a00000 	mov	r0, #0	; 0x0
    6754:	e28dd004 	add	sp, sp, #4	; 0x4
    6758:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    675c:	23200000 	.word	0x23200000

00006760 <block_cipher_aes_init>:
    6760:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6764:	e24dd004 	sub	sp, sp, #4	; 0x4
    6768:	e1a05000 	mov	r5, r0
    676c:	e1a04001 	mov	r4, r1
    6770:	e1a06002 	mov	r6, r2
    6774:	e1a07003 	mov	r7, r3
    6778:	ebffffdc 	bl	66f0 <block_cipher_reset>
    677c:	e3500000 	cmp	r0, #0	; 0x0
    6780:	1a00003d 	bne	687c <block_cipher_aes_init+0x11c>
    6784:	e59f3134 	ldr	r3, [pc, #308]	; 68c0 <block_cipher_aes_init+0x160>
    6788:	e3a02001 	mov	r2, #1	; 0x1
    678c:	e3550001 	cmp	r5, #1	; 0x1
    6790:	e5832404 	str	r2, [r3, #1028]
    6794:	8a000038 	bhi	687c <block_cipher_aes_init+0x11c>
    6798:	e3540003 	cmp	r4, #3	; 0x3
    679c:	13540000 	cmpne	r4, #0	; 0x0
    67a0:	0a000001 	beq	67ac <block_cipher_aes_init+0x4c>
    67a4:	e1540002 	cmp	r4, r2
    67a8:	1a000033 	bne	687c <block_cipher_aes_init+0x11c>
    67ac:	e1a03305 	lsl	r3, r5, #6
    67b0:	e3833901 	orr	r3, r3, #16384	; 0x4000
    67b4:	e3833004 	orr	r3, r3, #4	; 0x4
    67b8:	e59f2100 	ldr	r2, [pc, #256]	; 68c0 <block_cipher_aes_init+0x160>
    67bc:	e1833984 	orr	r3, r3, r4, lsl #19
    67c0:	e3540000 	cmp	r4, #0	; 0x0
    67c4:	e5823400 	str	r3, [r2, #1024]
    67c8:	1a000008 	bne	67f0 <block_cipher_aes_init+0x90>
    67cc:	e59f10f0 	ldr	r1, [pc, #240]	; 68c4 <block_cipher_aes_init+0x164>
    67d0:	e59f00f0 	ldr	r0, [pc, #240]	; 68c8 <block_cipher_aes_init+0x168>
    67d4:	e59f30f0 	ldr	r3, [pc, #240]	; 68cc <block_cipher_aes_init+0x16c>
    67d8:	e0862001 	add	r2, r6, r1
    67dc:	e0823003 	add	r3, r2, r3
    67e0:	e5933000 	ldr	r3, [r3]
    67e4:	e4813004 	str	r3, [r1], #4
    67e8:	e1510000 	cmp	r1, r0
    67ec:	1afffff8 	bne	67d4 <block_cipher_aes_init+0x74>
    67f0:	e59f10d0 	ldr	r1, [pc, #208]	; 68c8 <block_cipher_aes_init+0x168>
    67f4:	e59f00d4 	ldr	r0, [pc, #212]	; 68d0 <block_cipher_aes_init+0x170>
    67f8:	e59f30d4 	ldr	r3, [pc, #212]	; 68d4 <block_cipher_aes_init+0x174>
    67fc:	e0872001 	add	r2, r7, r1
    6800:	e0823003 	add	r3, r2, r3
    6804:	e5933000 	ldr	r3, [r3]
    6808:	e4813004 	str	r3, [r1], #4
    680c:	e1510000 	cmp	r1, r0
    6810:	1afffff8 	bne	67f8 <block_cipher_aes_init+0x98>
    6814:	e3540000 	cmp	r4, #0	; 0x0
    6818:	1a000008 	bne	6840 <block_cipher_aes_init+0xe0>
    681c:	eb001c5f 	bl	d9a0 <get_ticks>
    6820:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    6824:	e3a05000 	mov	r5, #0	; 0x0
    6828:	e0944000 	adds	r4, r4, r0
    682c:	e59f608c 	ldr	r6, [pc, #140]	; 68c0 <block_cipher_aes_init+0x160>
    6830:	e0a55001 	adc	r5, r5, r1
    6834:	e596340c 	ldr	r3, [r6, #1036]
    6838:	e3130008 	tst	r3, #8	; 0x8
    683c:	0a000011 	beq	6888 <block_cipher_aes_init+0x128>
    6840:	eb001c56 	bl	d9a0 <get_ticks>
    6844:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    6848:	e3a05000 	mov	r5, #0	; 0x0
    684c:	e0944000 	adds	r4, r4, r0
    6850:	e59f6068 	ldr	r6, [pc, #104]	; 68c0 <block_cipher_aes_init+0x160>
    6854:	e0a55001 	adc	r5, r5, r1
    6858:	e596340c 	ldr	r3, [r6, #1036]
    685c:	e59f205c 	ldr	r2, [pc, #92]	; 68c0 <block_cipher_aes_init+0x160>
    6860:	e3130010 	tst	r3, #16	; 0x10
    6864:	0a00000e 	beq	68a4 <block_cipher_aes_init+0x144>
    6868:	e5923400 	ldr	r3, [r2, #1024]
    686c:	e3a00000 	mov	r0, #0	; 0x0
    6870:	e3833001 	orr	r3, r3, #1	; 0x1
    6874:	e5823400 	str	r3, [r2, #1024]
    6878:	ea000000 	b	6880 <block_cipher_aes_init+0x120>
    687c:	e3a00001 	mov	r0, #1	; 0x1
    6880:	e28dd004 	add	sp, sp, #4	; 0x4
    6884:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6888:	eb001c44 	bl	d9a0 <get_ticks>
    688c:	e1510005 	cmp	r1, r5
    6890:	8afffff9 	bhi	687c <block_cipher_aes_init+0x11c>
    6894:	1affffe6 	bne	6834 <block_cipher_aes_init+0xd4>
    6898:	e1500004 	cmp	r0, r4
    689c:	8afffff6 	bhi	687c <block_cipher_aes_init+0x11c>
    68a0:	eaffffe3 	b	6834 <block_cipher_aes_init+0xd4>
    68a4:	eb001c3d 	bl	d9a0 <get_ticks>
    68a8:	e1510005 	cmp	r1, r5
    68ac:	8afffff2 	bhi	687c <block_cipher_aes_init+0x11c>
    68b0:	1affffe8 	bne	6858 <block_cipher_aes_init+0xf8>
    68b4:	e1500004 	cmp	r0, r4
    68b8:	8affffef 	bhi	687c <block_cipher_aes_init+0x11c>
    68bc:	eaffffe5 	b	6858 <block_cipher_aes_init+0xf8>
    68c0:	23200000 	.word	0x23200000
    68c4:	23200420 	.word	0x23200420
    68c8:	23200440 	.word	0x23200440
    68cc:	dcdffbe0 	.word	0xdcdffbe0
    68d0:	23200450 	.word	0x23200450
    68d4:	dcdffbc0 	.word	0xdcdffbc0

000068d8 <block_cipher_operation>:
    68d8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    68dc:	e24dd00c 	sub	sp, sp, #12	; 0xc
    68e0:	e1a08002 	mov	r8, r2
    68e4:	e58d0004 	str	r0, [sp, #4]
    68e8:	e1a09001 	mov	r9, r1
    68ec:	e1a06002 	mov	r6, r2
    68f0:	e59fa0c4 	ldr	sl, [pc, #196]	; 69bc <block_cipher_operation+0xe4>
    68f4:	e3a0b000 	mov	fp, #0	; 0x0
    68f8:	ea000028 	b	69a0 <block_cipher_operation+0xc8>
    68fc:	e59d3004 	ldr	r3, [sp, #4]
    6900:	e3560902 	cmp	r6, #32768	; 0x8000
    6904:	b1a07006 	movlt	r7, r6
    6908:	a3a07902 	movge	r7, #32768	; 0x8000
    690c:	e0664008 	rsb	r4, r6, r8
    6910:	e3a00000 	mov	r0, #0	; 0x0
    6914:	e0831004 	add	r1, r3, r4
    6918:	e1a02007 	mov	r2, r7
    691c:	eb0000ed 	bl	6cd8 <dma_sdma_init>
    6920:	e3500000 	cmp	r0, #0	; 0x0
    6924:	1a000021 	bne	69b0 <block_cipher_operation+0xd8>
    6928:	e0891004 	add	r1, r9, r4
    692c:	e2800001 	add	r0, r0, #1	; 0x1
    6930:	e1a02007 	mov	r2, r7
    6934:	eb0000e7 	bl	6cd8 <dma_sdma_init>
    6938:	e3500000 	cmp	r0, #0	; 0x0
    693c:	1a00001b 	bne	69b0 <block_cipher_operation+0xd8>
    6940:	eb0000d4 	bl	6c98 <dma_start>
    6944:	e3a00001 	mov	r0, #1	; 0x1
    6948:	eb0000d2 	bl	6c98 <dma_start>
    694c:	eb001c13 	bl	d9a0 <get_ticks>
    6950:	e090400a 	adds	r4, r0, sl
    6954:	e0a1500b 	adc	r5, r1, fp
    6958:	ea000005 	b	6974 <block_cipher_operation+0x9c>
    695c:	eb001c0f 	bl	d9a0 <get_ticks>
    6960:	e1510005 	cmp	r1, r5
    6964:	8a000011 	bhi	69b0 <block_cipher_operation+0xd8>
    6968:	1a000001 	bne	6974 <block_cipher_operation+0x9c>
    696c:	e1500004 	cmp	r0, r4
    6970:	8a00000e 	bhi	69b0 <block_cipher_operation+0xd8>
    6974:	e3a01003 	mov	r1, #3	; 0x3
    6978:	e3a00001 	mov	r0, #1	; 0x1
    697c:	eb0000cb 	bl	6cb0 <dma_get_status>
    6980:	e3500000 	cmp	r0, #0	; 0x0
    6984:	0afffff4 	beq	695c <block_cipher_operation+0x84>
    6988:	e3a00000 	mov	r0, #0	; 0x0
    698c:	e3a01003 	mov	r1, #3	; 0x3
    6990:	eb0000c6 	bl	6cb0 <dma_get_status>
    6994:	e3500000 	cmp	r0, #0	; 0x0
    6998:	0a000004 	beq	69b0 <block_cipher_operation+0xd8>
    699c:	e0676006 	rsb	r6, r7, r6
    69a0:	e3560000 	cmp	r6, #0	; 0x0
    69a4:	1affffd4 	bne	68fc <block_cipher_operation+0x24>
    69a8:	e1a00006 	mov	r0, r6
    69ac:	ea000000 	b	69b4 <block_cipher_operation+0xdc>
    69b0:	e3a00001 	mov	r0, #1	; 0x1
    69b4:	e28dd00c 	add	sp, sp, #12	; 0xc
    69b8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    69bc:	000f4240 	.word	0x000f4240

000069c0 <hash_len>:
    69c0:	e59f300c 	ldr	r3, [pc, #12]	; 69d4 <hash_len+0x14>
    69c4:	e5932000 	ldr	r2, [r3]
    69c8:	e59f3008 	ldr	r3, [pc, #8]	; 69d8 <hash_len+0x18>
    69cc:	e7930202 	ldr	r0, [r3, r2, lsl #4]
    69d0:	e12fff1e 	bx	lr
    69d4:	3c00d17c 	.word	0x3c00d17c
    69d8:	0001588c 	.word	0x0001588c

000069dc <hash_value>:
    69dc:	e92d4070 	push	{r4, r5, r6, lr}
    69e0:	e59f6054 	ldr	r6, [pc, #84]	; 6a3c <hash_value+0x60>
    69e4:	e1a05000 	mov	r5, r0
    69e8:	e5963000 	ldr	r3, [r6]
    69ec:	e3530000 	cmp	r3, #0	; 0x0
    69f0:	0a000009 	beq	6a1c <hash_value+0x40>
    69f4:	eb0000d9 	bl	6d60 <security_enabled>
    69f8:	e2504000 	subs	r4, r0, #0	; 0x0
    69fc:	1a000006 	bne	6a1c <hash_value+0x40>
    6a00:	e59f0038 	ldr	r0, [pc, #56]	; 6a40 <hash_value+0x64>
    6a04:	e5961000 	ldr	r1, [r6]
    6a08:	e59f3034 	ldr	r3, [pc, #52]	; 6a44 <hash_value+0x68>
    6a0c:	e1a0e00f 	mov	lr, pc
    6a10:	e593f000 	ldr	pc, [r3]
    6a14:	e1a00004 	mov	r0, r4
    6a18:	e8bd8070 	pop	{r4, r5, r6, pc}
    6a1c:	e59f3018 	ldr	r3, [pc, #24]	; 6a3c <hash_value+0x60>
    6a20:	e1a00005 	mov	r0, r5
    6a24:	e5932000 	ldr	r2, [r3]
    6a28:	e59f3018 	ldr	r3, [pc, #24]	; 6a48 <hash_value+0x6c>
    6a2c:	e0833202 	add	r3, r3, r2, lsl #4
    6a30:	e1a0e00f 	mov	lr, pc
    6a34:	e593f00c 	ldr	pc, [r3, #12]
    6a38:	e8bd8070 	pop	{r4, r5, r6, pc}
    6a3c:	3c00d17c 	.word	0x3c00d17c
    6a40:	00017bdb 	.word	0x00017bdb
    6a44:	3c00d6cc 	.word	0x3c00d6cc
    6a48:	0001588c 	.word	0x0001588c

00006a4c <hash_data>:
    6a4c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6a50:	e59f5074 	ldr	r5, [pc, #116]	; 6acc <hash_data+0x80>
    6a54:	e24dd004 	sub	sp, sp, #4	; 0x4
    6a58:	e5953000 	ldr	r3, [r5]
    6a5c:	e1a06000 	mov	r6, r0
    6a60:	e3530000 	cmp	r3, #0	; 0x0
    6a64:	e1a07001 	mov	r7, r1
    6a68:	0a000009 	beq	6a94 <hash_data+0x48>
    6a6c:	eb0000bb 	bl	6d60 <security_enabled>
    6a70:	e2504000 	subs	r4, r0, #0	; 0x0
    6a74:	1a000006 	bne	6a94 <hash_data+0x48>
    6a78:	e59f0050 	ldr	r0, [pc, #80]	; 6ad0 <hash_data+0x84>
    6a7c:	e5951000 	ldr	r1, [r5]
    6a80:	e59f304c 	ldr	r3, [pc, #76]	; 6ad4 <hash_data+0x88>
    6a84:	e1a0e00f 	mov	lr, pc
    6a88:	e593f000 	ldr	pc, [r3]
    6a8c:	e1a00004 	mov	r0, r4
    6a90:	ea00000b 	b	6ac4 <hash_data+0x78>
    6a94:	e59f003c 	ldr	r0, [pc, #60]	; 6ad8 <hash_data+0x8c>
    6a98:	e59f3034 	ldr	r3, [pc, #52]	; 6ad4 <hash_data+0x88>
    6a9c:	e1a0e00f 	mov	lr, pc
    6aa0:	e593f000 	ldr	pc, [r3]
    6aa4:	e59f3020 	ldr	r3, [pc, #32]	; 6acc <hash_data+0x80>
    6aa8:	e1a00006 	mov	r0, r6
    6aac:	e5932000 	ldr	r2, [r3]
    6ab0:	e59f3024 	ldr	r3, [pc, #36]	; 6adc <hash_data+0x90>
    6ab4:	e1a01007 	mov	r1, r7
    6ab8:	e0833202 	add	r3, r3, r2, lsl #4
    6abc:	e1a0e00f 	mov	lr, pc
    6ac0:	e593f008 	ldr	pc, [r3, #8]
    6ac4:	e28dd004 	add	sp, sp, #4	; 0x4
    6ac8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6acc:	3c00d17c 	.word	0x3c00d17c
    6ad0:	00017bdb 	.word	0x00017bdb
    6ad4:	3c00d6cc 	.word	0x3c00d6cc
    6ad8:	00017bfc 	.word	0x00017bfc
    6adc:	0001588c 	.word	0x0001588c

00006ae0 <hash_init>:
    6ae0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6ae4:	e3500003 	cmp	r0, #3	; 0x3
    6ae8:	e24dd004 	sub	sp, sp, #4	; 0x4
    6aec:	e1a04000 	mov	r4, r0
    6af0:	e1a07001 	mov	r7, r1
    6af4:	9a000005 	bls	6b10 <hash_init+0x30>
    6af8:	e59f0064 	ldr	r0, [pc, #100]	; 6b64 <hash_init+0x84>
    6afc:	e59f3064 	ldr	r3, [pc, #100]	; 6b68 <hash_init+0x88>
    6b00:	e1a0e00f 	mov	lr, pc
    6b04:	e593f000 	ldr	pc, [r3]
    6b08:	e3e00000 	mvn	r0, #0	; 0x0
    6b0c:	ea000012 	b	6b5c <hash_init+0x7c>
    6b10:	e59f6054 	ldr	r6, [pc, #84]	; 6b6c <hash_init+0x8c>
    6b14:	e3500000 	cmp	r0, #0	; 0x0
    6b18:	e5860000 	str	r0, [r6]
    6b1c:	0a000009 	beq	6b48 <hash_init+0x68>
    6b20:	eb00008e 	bl	6d60 <security_enabled>
    6b24:	e2505000 	subs	r5, r0, #0	; 0x0
    6b28:	1a000006 	bne	6b48 <hash_init+0x68>
    6b2c:	e59f003c 	ldr	r0, [pc, #60]	; 6b70 <hash_init+0x90>
    6b30:	e5961000 	ldr	r1, [r6]
    6b34:	e59f302c 	ldr	r3, [pc, #44]	; 6b68 <hash_init+0x88>
    6b38:	e1a0e00f 	mov	lr, pc
    6b3c:	e593f000 	ldr	pc, [r3]
    6b40:	e1a00005 	mov	r0, r5
    6b44:	ea000004 	b	6b5c <hash_init+0x7c>
    6b48:	e59f3024 	ldr	r3, [pc, #36]	; 6b74 <hash_init+0x94>
    6b4c:	e1a00007 	mov	r0, r7
    6b50:	e0833204 	add	r3, r3, r4, lsl #4
    6b54:	e1a0e00f 	mov	lr, pc
    6b58:	e593f004 	ldr	pc, [r3, #4]
    6b5c:	e28dd004 	add	sp, sp, #4	; 0x4
    6b60:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    6b64:	00017c07 	.word	0x00017c07
    6b68:	3c00d6cc 	.word	0x3c00d6cc
    6b6c:	3c00d17c 	.word	0x3c00d17c
    6b70:	00017bdb 	.word	0x00017bdb
    6b74:	0001588c 	.word	0x0001588c

00006b78 <md5_value>:
    6b78:	e92d4030 	push	{r4, r5, lr}
    6b7c:	e59f3054 	ldr	r3, [pc, #84]	; 6bd8 <md5_value+0x60>
    6b80:	e1a05000 	mov	r5, r0
    6b84:	e893000c 	ldm	r3, {r2, r3}
    6b88:	e1823003 	orr	r3, r2, r3
    6b8c:	e24dd024 	sub	sp, sp, #36	; 0x24
    6b90:	e3530000 	cmp	r3, #0	; 0x0
    6b94:	13e00000 	mvnne	r0, #0	; 0x0
    6b98:	1a00000c 	bne	6bd0 <md5_value+0x58>
    6b9c:	e1a0000d 	mov	r0, sp
    6ba0:	eb00016d 	bl	715c <hash_module_get_result>
    6ba4:	e3500000 	cmp	r0, #0	; 0x0
    6ba8:	e1a0400d 	mov	r4, sp
    6bac:	13e00001 	mvnne	r0, #1	; 0x1
    6bb0:	1a000006 	bne	6bd0 <md5_value+0x58>
    6bb4:	e1a0200d 	mov	r2, sp
    6bb8:	e7d23000 	ldrb	r3, [r2, r0]
    6bbc:	e7c53000 	strb	r3, [r5, r0]
    6bc0:	e2800001 	add	r0, r0, #1	; 0x1
    6bc4:	e3500010 	cmp	r0, #16	; 0x10
    6bc8:	1afffffa 	bne	6bb8 <md5_value+0x40>
    6bcc:	e3a00000 	mov	r0, #0	; 0x0
    6bd0:	e28dd024 	add	sp, sp, #36	; 0x24
    6bd4:	e8bd8030 	pop	{r4, r5, pc}
    6bd8:	3c00d180 	.word	0x3c00d180

00006bdc <md5_data>:
    6bdc:	e59f305c 	ldr	r3, [pc, #92]	; 6c40 <md5_data+0x64>
    6be0:	e92d4070 	push	{r4, r5, r6, lr}
    6be4:	e1a06001 	mov	r6, r1
    6be8:	e5931004 	ldr	r1, [r3, #4]
    6bec:	e1a04006 	mov	r4, r6
    6bf0:	e3a05000 	mov	r5, #0	; 0x0
    6bf4:	e1550001 	cmp	r5, r1
    6bf8:	e593c000 	ldr	ip, [r3]
    6bfc:	8a000002 	bhi	6c0c <md5_data+0x30>
    6c00:	1a000003 	bne	6c14 <md5_data+0x38>
    6c04:	e154000c 	cmp	r4, ip
    6c08:	9a000001 	bls	6c14 <md5_data+0x38>
    6c0c:	e3e00000 	mvn	r0, #0	; 0x0
    6c10:	e8bd8070 	pop	{r4, r5, r6, pc}
    6c14:	e1a0200c 	mov	r2, ip
    6c18:	e1a03001 	mov	r3, r1
    6c1c:	e59fc01c 	ldr	ip, [pc, #28]	; 6c40 <md5_data+0x64>
    6c20:	e0522004 	subs	r2, r2, r4
    6c24:	e0c33005 	sbc	r3, r3, r5
    6c28:	e1a01006 	mov	r1, r6
    6c2c:	e88c000c 	stm	ip, {r2, r3}
    6c30:	eb0001ac 	bl	72e8 <hash_dma_transfer>
    6c34:	e3500000 	cmp	r0, #0	; 0x0
    6c38:	13e00001 	mvnne	r0, #1	; 0x1
    6c3c:	e8bd8070 	pop	{r4, r5, r6, pc}
    6c40:	3c00d180 	.word	0x3c00d180

00006c44 <md5_init>:
    6c44:	e1a03ea0 	lsr	r3, r0, #29
    6c48:	e1a0c180 	lsl	ip, r0, #3
    6c4c:	e92d4010 	push	{r4, lr}
    6c50:	e59f403c 	ldr	r4, [pc, #60]	; 6c94 <md5_init+0x50>
    6c54:	e1a0200c 	mov	r2, ip
    6c58:	e1a01003 	mov	r1, r3
    6c5c:	e3a00000 	mov	r0, #0	; 0x0
    6c60:	e5843004 	str	r3, [r4, #4]
    6c64:	e584c000 	str	ip, [r4]
    6c68:	eb000182 	bl	7278 <hash_module_init>
    6c6c:	e3500000 	cmp	r0, #0	; 0x0
    6c70:	05942000 	ldreq	r2, [r4]
    6c74:	05943004 	ldreq	r3, [r4, #4]
    6c78:	01a021a2 	lsreq	r2, r2, #3
    6c7c:	01822e83 	orreq	r2, r2, r3, lsl #29
    6c80:	01a031a3 	lsreq	r3, r3, #3
    6c84:	13e00000 	mvnne	r0, #0	; 0x0
    6c88:	05843004 	streq	r3, [r4, #4]
    6c8c:	05842000 	streq	r2, [r4]
    6c90:	e8bd8010 	pop	{r4, pc}
    6c94:	3c00d180 	.word	0x3c00d180

00006c98 <dma_start>:
    6c98:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6c9c:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6ca0:	e1a00400 	lsl	r0, r0, #8
    6ca4:	e3a03001 	mov	r3, #1	; 0x1
    6ca8:	e5803000 	str	r3, [r0]
    6cac:	e12fff1e 	bx	lr

00006cb0 <dma_get_status>:
    6cb0:	e3a0c001 	mov	ip, #1	; 0x1
    6cb4:	e1a0211c 	lsl	r2, ip, r1
    6cb8:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6cbc:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6cc0:	e1a01400 	lsl	r1, r0, #8
    6cc4:	e5913024 	ldr	r3, [r1, #36]
    6cc8:	e0130002 	ands	r0, r3, r2
    6ccc:	15812024 	strne	r2, [r1, #36]
    6cd0:	11a0000c 	movne	r0, ip
    6cd4:	e12fff1e 	bx	lr

00006cd8 <dma_sdma_init>:
    6cd8:	e280c823 	add	ip, r0, #2293760	; 0x230000
    6cdc:	e28cca02 	add	ip, ip, #8192	; 0x2000
    6ce0:	e1a0040c 	lsl	r0, ip, #8
    6ce4:	e2800004 	add	r0, r0, #4	; 0x4
    6ce8:	e5903000 	ldr	r3, [r0]
    6cec:	e3520801 	cmp	r2, #65536	; 0x10000
    6cf0:	e3833001 	orr	r3, r3, #1	; 0x1
    6cf4:	e5803000 	str	r3, [r0]
    6cf8:	e92d4010 	push	{r4, lr}
    6cfc:	e3a03341 	mov	r3, #67108865	; 0x4000001
    6d00:	e1a0440c 	lsl	r4, ip, #8
    6d04:	e584300c 	str	r3, [r4, #12]
    6d08:	31a02802 	lslcc	r2, r2, #16
    6d0c:	31a0340c 	lslcc	r3, ip, #8
    6d10:	31a02822 	lsrcc	r2, r2, #16
    6d14:	32833010 	addcc	r3, r3, #16	; 0x10
    6d18:	35832000 	strcc	r2, [r3]
    6d1c:	23a00001 	movcs	r0, #1	; 0x1
    6d20:	35841014 	strcc	r1, [r4, #20]
    6d24:	33a00000 	movcc	r0, #0	; 0x0
    6d28:	e8bd8010 	pop	{r4, pc}

00006d2c <security_cke_control>:
    6d2c:	e59fc014 	ldr	ip, [pc, #20]	; 6d48 <security_cke_control+0x1c>
    6d30:	e3a02001 	mov	r2, #1	; 0x1
    6d34:	e59c3000 	ldr	r3, [ip]
    6d38:	e1c33012 	bic	r3, r3, r2, lsl r0
    6d3c:	e1833011 	orr	r3, r3, r1, lsl r0
    6d40:	e58c3000 	str	r3, [ip]
    6d44:	e12fff1e 	bx	lr
    6d48:	23202000 	.word	0x23202000

00006d4c <security_enable>:
    6d4c:	e59f3008 	ldr	r3, [pc, #8]	; 6d5c <security_enable+0x10>
    6d50:	e3a02001 	mov	r2, #1	; 0x1
    6d54:	e5832000 	str	r2, [r3]
    6d58:	e12fff1e 	bx	lr
    6d5c:	3c00d188 	.word	0x3c00d188

00006d60 <security_enabled>:
    6d60:	e59f300c 	ldr	r3, [pc, #12]	; 6d74 <security_enabled+0x14>
    6d64:	e5930000 	ldr	r0, [r3]
    6d68:	e2500000 	subs	r0, r0, #0	; 0x0
    6d6c:	13a00001 	movne	r0, #1	; 0x1
    6d70:	e12fff1e 	bx	lr
    6d74:	3c00d188 	.word	0x3c00d188

00006d78 <dma_reset>:
    6d78:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    6d7c:	e2800823 	add	r0, r0, #2293760	; 0x230000
    6d80:	e2800a02 	add	r0, r0, #8192	; 0x2000
    6d84:	e1a07400 	lsl	r7, r0, #8
    6d88:	e3a03002 	mov	r3, #2	; 0x2
    6d8c:	e24dd004 	sub	sp, sp, #4	; 0x4
    6d90:	e5873000 	str	r3, [r7]
    6d94:	eb001b01 	bl	d9a0 <get_ticks>
    6d98:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    6d9c:	e3a05000 	mov	r5, #0	; 0x0
    6da0:	e0944000 	adds	r4, r4, r0
    6da4:	e0a55001 	adc	r5, r5, r1
    6da8:	e5976000 	ldr	r6, [r7]
    6dac:	ea000008 	b	6dd4 <dma_reset+0x5c>
    6db0:	e5976000 	ldr	r6, [r7]
    6db4:	eb001af9 	bl	d9a0 <get_ticks>
    6db8:	e1510005 	cmp	r1, r5
    6dbc:	8a000002 	bhi	6dcc <dma_reset+0x54>
    6dc0:	1a000003 	bne	6dd4 <dma_reset+0x5c>
    6dc4:	e1500004 	cmp	r0, r4
    6dc8:	9a000001 	bls	6dd4 <dma_reset+0x5c>
    6dcc:	e3a00001 	mov	r0, #1	; 0x1
    6dd0:	ea000002 	b	6de0 <dma_reset+0x68>
    6dd4:	e3560000 	cmp	r6, #0	; 0x0
    6dd8:	1afffff4 	bne	6db0 <dma_reset+0x38>
    6ddc:	e1a00006 	mov	r0, r6
    6de0:	e28dd004 	add	sp, sp, #4	; 0x4
    6de4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

00006de8 <sha1_value>:
    6de8:	e92d4030 	push	{r4, r5, lr}
    6dec:	e59f3054 	ldr	r3, [pc, #84]	; 6e48 <sha1_value+0x60>
    6df0:	e1a05000 	mov	r5, r0
    6df4:	e893000c 	ldm	r3, {r2, r3}
    6df8:	e1823003 	orr	r3, r2, r3
    6dfc:	e24dd024 	sub	sp, sp, #36	; 0x24
    6e00:	e3530000 	cmp	r3, #0	; 0x0
    6e04:	13e00000 	mvnne	r0, #0	; 0x0
    6e08:	1a00000c 	bne	6e40 <sha1_value+0x58>
    6e0c:	e1a0000d 	mov	r0, sp
    6e10:	eb0000d1 	bl	715c <hash_module_get_result>
    6e14:	e3500000 	cmp	r0, #0	; 0x0
    6e18:	e1a0400d 	mov	r4, sp
    6e1c:	13e00001 	mvnne	r0, #1	; 0x1
    6e20:	1a000006 	bne	6e40 <sha1_value+0x58>
    6e24:	e1a0200d 	mov	r2, sp
    6e28:	e7d23000 	ldrb	r3, [r2, r0]
    6e2c:	e7c53000 	strb	r3, [r5, r0]
    6e30:	e2800001 	add	r0, r0, #1	; 0x1
    6e34:	e3500014 	cmp	r0, #20	; 0x14
    6e38:	1afffffa 	bne	6e28 <sha1_value+0x40>
    6e3c:	e3a00000 	mov	r0, #0	; 0x0
    6e40:	e28dd024 	add	sp, sp, #36	; 0x24
    6e44:	e8bd8030 	pop	{r4, r5, pc}
    6e48:	3c00d190 	.word	0x3c00d190

00006e4c <sha1_data>:
    6e4c:	e59f305c 	ldr	r3, [pc, #92]	; 6eb0 <sha1_data+0x64>
    6e50:	e92d4070 	push	{r4, r5, r6, lr}
    6e54:	e1a06001 	mov	r6, r1
    6e58:	e5931004 	ldr	r1, [r3, #4]
    6e5c:	e1a04006 	mov	r4, r6
    6e60:	e3a05000 	mov	r5, #0	; 0x0
    6e64:	e1550001 	cmp	r5, r1
    6e68:	e593c000 	ldr	ip, [r3]
    6e6c:	8a000002 	bhi	6e7c <sha1_data+0x30>
    6e70:	1a000003 	bne	6e84 <sha1_data+0x38>
    6e74:	e154000c 	cmp	r4, ip
    6e78:	9a000001 	bls	6e84 <sha1_data+0x38>
    6e7c:	e3e00000 	mvn	r0, #0	; 0x0
    6e80:	e8bd8070 	pop	{r4, r5, r6, pc}
    6e84:	e1a0200c 	mov	r2, ip
    6e88:	e1a03001 	mov	r3, r1
    6e8c:	e59fc01c 	ldr	ip, [pc, #28]	; 6eb0 <sha1_data+0x64>
    6e90:	e0522004 	subs	r2, r2, r4
    6e94:	e0c33005 	sbc	r3, r3, r5
    6e98:	e1a01006 	mov	r1, r6
    6e9c:	e88c000c 	stm	ip, {r2, r3}
    6ea0:	eb000110 	bl	72e8 <hash_dma_transfer>
    6ea4:	e3500000 	cmp	r0, #0	; 0x0
    6ea8:	13e00001 	mvnne	r0, #1	; 0x1
    6eac:	e8bd8070 	pop	{r4, r5, r6, pc}
    6eb0:	3c00d190 	.word	0x3c00d190

00006eb4 <sha1_init>:
    6eb4:	e1a03ea0 	lsr	r3, r0, #29
    6eb8:	e1a0c180 	lsl	ip, r0, #3
    6ebc:	e92d4010 	push	{r4, lr}
    6ec0:	e59f403c 	ldr	r4, [pc, #60]	; 6f04 <sha1_init+0x50>
    6ec4:	e1a0200c 	mov	r2, ip
    6ec8:	e1a01003 	mov	r1, r3
    6ecc:	e3a00001 	mov	r0, #1	; 0x1
    6ed0:	e5843004 	str	r3, [r4, #4]
    6ed4:	e584c000 	str	ip, [r4]
    6ed8:	eb0000e6 	bl	7278 <hash_module_init>
    6edc:	e3500000 	cmp	r0, #0	; 0x0
    6ee0:	05942000 	ldreq	r2, [r4]
    6ee4:	05943004 	ldreq	r3, [r4, #4]
    6ee8:	01a021a2 	lsreq	r2, r2, #3
    6eec:	01822e83 	orreq	r2, r2, r3, lsl #29
    6ef0:	01a031a3 	lsreq	r3, r3, #3
    6ef4:	13e00000 	mvnne	r0, #0	; 0x0
    6ef8:	05843004 	streq	r3, [r4, #4]
    6efc:	05842000 	streq	r2, [r4]
    6f00:	e8bd8010 	pop	{r4, pc}
    6f04:	3c00d190 	.word	0x3c00d190

00006f08 <sha256_value>:
    6f08:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    6f0c:	e59f3028 	ldr	r3, [pc, #40]	; 6f3c <sha256_value+0x34>
    6f10:	e24dd004 	sub	sp, sp, #4	; 0x4
    6f14:	e893000c 	ldm	r3, {r2, r3}
    6f18:	e1823003 	orr	r3, r2, r3
    6f1c:	e3530000 	cmp	r3, #0	; 0x0
    6f20:	13e00000 	mvnne	r0, #0	; 0x0
    6f24:	1a000002 	bne	6f34 <sha256_value+0x2c>
    6f28:	eb00008b 	bl	715c <hash_module_get_result>
    6f2c:	e3500000 	cmp	r0, #0	; 0x0
    6f30:	13e00001 	mvnne	r0, #1	; 0x1
    6f34:	e28dd004 	add	sp, sp, #4	; 0x4
    6f38:	e8bd8000 	pop	{pc}
    6f3c:	3c00d198 	.word	0x3c00d198

00006f40 <sha256_data>:
    6f40:	e59f305c 	ldr	r3, [pc, #92]	; 6fa4 <sha256_data+0x64>
    6f44:	e92d4070 	push	{r4, r5, r6, lr}
    6f48:	e1a06001 	mov	r6, r1
    6f4c:	e5931004 	ldr	r1, [r3, #4]
    6f50:	e1a04006 	mov	r4, r6
    6f54:	e3a05000 	mov	r5, #0	; 0x0
    6f58:	e1550001 	cmp	r5, r1
    6f5c:	e593c000 	ldr	ip, [r3]
    6f60:	8a000002 	bhi	6f70 <sha256_data+0x30>
    6f64:	1a000003 	bne	6f78 <sha256_data+0x38>
    6f68:	e154000c 	cmp	r4, ip
    6f6c:	9a000001 	bls	6f78 <sha256_data+0x38>
    6f70:	e3e00000 	mvn	r0, #0	; 0x0
    6f74:	e8bd8070 	pop	{r4, r5, r6, pc}
    6f78:	e1a0200c 	mov	r2, ip
    6f7c:	e1a03001 	mov	r3, r1
    6f80:	e59fc01c 	ldr	ip, [pc, #28]	; 6fa4 <sha256_data+0x64>
    6f84:	e0522004 	subs	r2, r2, r4
    6f88:	e0c33005 	sbc	r3, r3, r5
    6f8c:	e1a01006 	mov	r1, r6
    6f90:	e88c000c 	stm	ip, {r2, r3}
    6f94:	eb0000d3 	bl	72e8 <hash_dma_transfer>
    6f98:	e3500000 	cmp	r0, #0	; 0x0
    6f9c:	13e00001 	mvnne	r0, #1	; 0x1
    6fa0:	e8bd8070 	pop	{r4, r5, r6, pc}
    6fa4:	3c00d198 	.word	0x3c00d198

00006fa8 <sha256_init>:
    6fa8:	e1a03ea0 	lsr	r3, r0, #29
    6fac:	e1a0c180 	lsl	ip, r0, #3
    6fb0:	e92d4010 	push	{r4, lr}
    6fb4:	e59f403c 	ldr	r4, [pc, #60]	; 6ff8 <sha256_init+0x50>
    6fb8:	e1a0200c 	mov	r2, ip
    6fbc:	e1a01003 	mov	r1, r3
    6fc0:	e3a00003 	mov	r0, #3	; 0x3
    6fc4:	e5843004 	str	r3, [r4, #4]
    6fc8:	e584c000 	str	ip, [r4]
    6fcc:	eb0000a9 	bl	7278 <hash_module_init>
    6fd0:	e3500000 	cmp	r0, #0	; 0x0
    6fd4:	05942000 	ldreq	r2, [r4]
    6fd8:	05943004 	ldreq	r3, [r4, #4]
    6fdc:	01a021a2 	lsreq	r2, r2, #3
    6fe0:	01822e83 	orreq	r2, r2, r3, lsl #29
    6fe4:	01a031a3 	lsreq	r3, r3, #3
    6fe8:	13e00000 	mvnne	r0, #0	; 0x0
    6fec:	05843004 	streq	r3, [r4, #4]
    6ff0:	05842000 	streq	r2, [r4]
    6ff4:	e8bd8010 	pop	{r4, pc}
    6ff8:	3c00d198 	.word	0x3c00d198

00006ffc <crc32_init>:
    6ffc:	e59f300c 	ldr	r3, [pc, #12]	; 7010 <crc32_init+0x14>
    7000:	e3e02000 	mvn	r2, #0	; 0x0
    7004:	e5832000 	str	r2, [r3]
    7008:	e3a00000 	mov	r0, #0	; 0x0
    700c:	e12fff1e 	bx	lr
    7010:	3c00d1a0 	.word	0x3c00d1a0

00007014 <crc32_data>:
    7014:	e59f311c 	ldr	r3, [pc, #284]	; 7138 <crc32_data+0x124>
    7018:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    701c:	e593c000 	ldr	ip, [r3]
    7020:	e1a08000 	mov	r8, r0
    7024:	e1a06001 	mov	r6, r1
    7028:	e1a04000 	mov	r4, r0
    702c:	e1a07001 	mov	r7, r1
    7030:	e59f5104 	ldr	r5, [pc, #260]	; 713c <crc32_data+0x128>
    7034:	ea000027 	b	70d8 <crc32_data+0xc4>
    7038:	e5543008 	ldrb	r3, [r4, #-8]
    703c:	e5542007 	ldrb	r2, [r4, #-7]
    7040:	e02c3003 	eor	r3, ip, r3
    7044:	e20330ff 	and	r3, r3, #255	; 0xff
    7048:	e7953103 	ldr	r3, [r5, r3, lsl #2]
    704c:	e5541006 	ldrb	r1, [r4, #-6]
    7050:	e023342c 	eor	r3, r3, ip, lsr #8
    7054:	e0232002 	eor	r2, r3, r2
    7058:	e20220ff 	and	r2, r2, #255	; 0xff
    705c:	e7952102 	ldr	r2, [r5, r2, lsl #2]
    7060:	e5540005 	ldrb	r0, [r4, #-5]
    7064:	e0222423 	eor	r2, r2, r3, lsr #8
    7068:	e0221001 	eor	r1, r2, r1
    706c:	e20110ff 	and	r1, r1, #255	; 0xff
    7070:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    7074:	e5541004 	ldrb	r1, [r4, #-4]
    7078:	e0233422 	eor	r3, r3, r2, lsr #8
    707c:	e0230000 	eor	r0, r3, r0
    7080:	e20000ff 	and	r0, r0, #255	; 0xff
    7084:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    7088:	e5540003 	ldrb	r0, [r4, #-3]
    708c:	e0222423 	eor	r2, r2, r3, lsr #8
    7090:	e0221001 	eor	r1, r2, r1
    7094:	e20110ff 	and	r1, r1, #255	; 0xff
    7098:	e7953101 	ldr	r3, [r5, r1, lsl #2]
    709c:	e554c002 	ldrb	ip, [r4, #-2]
    70a0:	e0233422 	eor	r3, r3, r2, lsr #8
    70a4:	e0230000 	eor	r0, r3, r0
    70a8:	e20000ff 	and	r0, r0, #255	; 0xff
    70ac:	e7952100 	ldr	r2, [r5, r0, lsl #2]
    70b0:	e5541001 	ldrb	r1, [r4, #-1]
    70b4:	e0222423 	eor	r2, r2, r3, lsr #8
    70b8:	e022c00c 	eor	ip, r2, ip
    70bc:	e20cc0ff 	and	ip, ip, #255	; 0xff
    70c0:	e795310c 	ldr	r3, [r5, ip, lsl #2]
    70c4:	e0233422 	eor	r3, r3, r2, lsr #8
    70c8:	e0231001 	eor	r1, r3, r1
    70cc:	e20110ff 	and	r1, r1, #255	; 0xff
    70d0:	e7952101 	ldr	r2, [r5, r1, lsl #2]
    70d4:	e022c423 	eor	ip, r2, r3, lsr #8
    70d8:	e3570007 	cmp	r7, #7	; 0x7
    70dc:	e2844008 	add	r4, r4, #8	; 0x8
    70e0:	e2477008 	sub	r7, r7, #8	; 0x8
    70e4:	8affffd3 	bhi	7038 <crc32_data+0x24>
    70e8:	e3c63007 	bic	r3, r6, #7	; 0x7
    70ec:	e0880003 	add	r0, r8, r3
    70f0:	e59f3040 	ldr	r3, [pc, #64]	; 7138 <crc32_data+0x124>
    70f4:	e2161007 	ands	r1, r6, #7	; 0x7
    70f8:	e583c000 	str	ip, [r3]
    70fc:	0a00000b 	beq	7130 <crc32_data+0x11c>
    7100:	e59f4034 	ldr	r4, [pc, #52]	; 713c <crc32_data+0x128>
    7104:	e3a02000 	mov	r2, #0	; 0x0
    7108:	e7d03002 	ldrb	r3, [r0, r2]
    710c:	e2822001 	add	r2, r2, #1	; 0x1
    7110:	e02c3003 	eor	r3, ip, r3
    7114:	e20330ff 	and	r3, r3, #255	; 0xff
    7118:	e7943103 	ldr	r3, [r4, r3, lsl #2]
    711c:	e1510002 	cmp	r1, r2
    7120:	e023c42c 	eor	ip, r3, ip, lsr #8
    7124:	1afffff7 	bne	7108 <crc32_data+0xf4>
    7128:	e59f3008 	ldr	r3, [pc, #8]	; 7138 <crc32_data+0x124>
    712c:	e583c000 	str	ip, [r3]
    7130:	e3a00000 	mov	r0, #0	; 0x0
    7134:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    7138:	3c00d1a0 	.word	0x3c00d1a0
    713c:	000158cc 	.word	0x000158cc

00007140 <crc32_value>:
    7140:	e59f3010 	ldr	r3, [pc, #16]	; 7158 <crc32_value+0x18>
    7144:	e5933000 	ldr	r3, [r3]
    7148:	e1e03003 	mvn	r3, r3
    714c:	e5803000 	str	r3, [r0]
    7150:	e3a00000 	mov	r0, #0	; 0x0
    7154:	e12fff1e 	bx	lr
    7158:	3c00d1a0 	.word	0x3c00d1a0

0000715c <hash_module_get_result>:
    715c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    7160:	e1a08000 	mov	r8, r0
    7164:	eb001a0d 	bl	d9a0 <get_ticks>
    7168:	e3a04e7d 	mov	r4, #2000	; 0x7d0
    716c:	e59f3088 	ldr	r3, [pc, #136]	; 71fc <hash_module_get_result+0xa0>
    7170:	e3a05000 	mov	r5, #0	; 0x0
    7174:	e0944000 	adds	r4, r4, r0
    7178:	e0a55001 	adc	r5, r5, r1
    717c:	e1a07003 	mov	r7, r3
    7180:	e593680c 	ldr	r6, [r3, #2060]
    7184:	ea000008 	b	71ac <hash_module_get_result+0x50>
    7188:	e597680c 	ldr	r6, [r7, #2060]
    718c:	eb001a03 	bl	d9a0 <get_ticks>
    7190:	e1510005 	cmp	r1, r5
    7194:	8a000002 	bhi	71a4 <hash_module_get_result+0x48>
    7198:	1a000003 	bne	71ac <hash_module_get_result+0x50>
    719c:	e1500004 	cmp	r0, r4
    71a0:	9a000001 	bls	71ac <hash_module_get_result+0x50>
    71a4:	e3a00001 	mov	r0, #1	; 0x1
    71a8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    71ac:	e3160001 	tst	r6, #1	; 0x1
    71b0:	0afffff4 	beq	7188 <hash_module_get_result+0x2c>
    71b4:	e59f0044 	ldr	r0, [pc, #68]	; 7200 <hash_module_get_result+0xa4>
    71b8:	e59fc044 	ldr	ip, [pc, #68]	; 7204 <hash_module_get_result+0xa8>
    71bc:	e1a03000 	mov	r3, r0
    71c0:	e6931008 	ldr	r1, [r3], r8
    71c4:	e59f203c 	ldr	r2, [pc, #60]	; 7208 <hash_module_get_result+0xac>
    71c8:	e2800004 	add	r0, r0, #4	; 0x4
    71cc:	e0832002 	add	r2, r3, r2
    71d0:	e150000c 	cmp	r0, ip
    71d4:	e5821000 	str	r1, [r2]
    71d8:	1afffff7 	bne	71bc <hash_module_get_result+0x60>
    71dc:	e59f2018 	ldr	r2, [pc, #24]	; 71fc <hash_module_get_result+0xa0>
    71e0:	e3a03001 	mov	r3, #1	; 0x1
    71e4:	e582380c 	str	r3, [r2, #2060]
    71e8:	e5923800 	ldr	r3, [r2, #2048]
    71ec:	e3a00000 	mov	r0, #0	; 0x0
    71f0:	e3c33080 	bic	r3, r3, #128	; 0x80
    71f4:	e5823800 	str	r3, [r2, #2048]
    71f8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    71fc:	23200000 	.word	0x23200000
    7200:	23200858 	.word	0x23200858
    7204:	23200878 	.word	0x23200878
    7208:	dcdff7a8 	.word	0xdcdff7a8

0000720c <hash_module_clear>:
    720c:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    7210:	e59f405c 	ldr	r4, [pc, #92]	; 7274 <hash_module_clear+0x68>
    7214:	e3a03002 	mov	r3, #2	; 0x2
    7218:	e5843800 	str	r3, [r4, #2048]
    721c:	e24dd004 	sub	sp, sp, #4	; 0x4
    7220:	eb0019de 	bl	d9a0 <get_ticks>
    7224:	e3a06e7d 	mov	r6, #2000	; 0x7d0
    7228:	e3a07000 	mov	r7, #0	; 0x0
    722c:	e0966000 	adds	r6, r6, r0
    7230:	e1a05004 	mov	r5, r4
    7234:	e0a77001 	adc	r7, r7, r1
    7238:	e5944800 	ldr	r4, [r4, #2048]
    723c:	ea000008 	b	7264 <hash_module_clear+0x58>
    7240:	e5954800 	ldr	r4, [r5, #2048]
    7244:	eb0019d5 	bl	d9a0 <get_ticks>
    7248:	e1510007 	cmp	r1, r7
    724c:	8a000002 	bhi	725c <hash_module_clear+0x50>
    7250:	1a000003 	bne	7264 <hash_module_clear+0x58>
    7254:	e1500006 	cmp	r0, r6
    7258:	9a000001 	bls	7264 <hash_module_clear+0x58>
    725c:	e3a00001 	mov	r0, #1	; 0x1
    7260:	ea000001 	b	726c <hash_module_clear+0x60>
    7264:	e2140002 	ands	r0, r4, #2	; 0x2
    7268:	1afffff4 	bne	7240 <hash_module_clear+0x34>
    726c:	e28dd004 	add	sp, sp, #4	; 0x4
    7270:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    7274:	23200000 	.word	0x23200000

00007278 <hash_module_init>:
    7278:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    727c:	e59f4060 	ldr	r4, [pc, #96]	; 72e4 <hash_module_init+0x6c>
    7280:	e24dd004 	sub	sp, sp, #4	; 0x4
    7284:	e5943800 	ldr	r3, [r4, #2048]
    7288:	e1a05000 	mov	r5, r0
    728c:	e3c33080 	bic	r3, r3, #128	; 0x80
    7290:	e5843800 	str	r3, [r4, #2048]
    7294:	e1a07001 	mov	r7, r1
    7298:	e1a06002 	mov	r6, r2
    729c:	ebffffda 	bl	720c <hash_module_clear>
    72a0:	e3500000 	cmp	r0, #0	; 0x0
    72a4:	13a00001 	movne	r0, #1	; 0x1
    72a8:	1a00000b 	bne	72dc <hash_module_init+0x64>
    72ac:	e3a03001 	mov	r3, #1	; 0x1
    72b0:	e1a02185 	lsl	r2, r5, #3
    72b4:	e5843804 	str	r3, [r4, #2052]
    72b8:	e5842800 	str	r2, [r4, #2048]
    72bc:	e5847814 	str	r7, [r4, #2068]
    72c0:	e5846810 	str	r6, [r4, #2064]
    72c4:	e5943800 	ldr	r3, [r4, #2048]
    72c8:	e3833080 	orr	r3, r3, #128	; 0x80
    72cc:	e5843800 	str	r3, [r4, #2048]
    72d0:	e5943800 	ldr	r3, [r4, #2048]
    72d4:	e3833004 	orr	r3, r3, #4	; 0x4
    72d8:	e5843800 	str	r3, [r4, #2048]
    72dc:	e28dd004 	add	sp, sp, #4	; 0x4
    72e0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    72e4:	23200000 	.word	0x23200000

000072e8 <hash_dma_transfer>:
    72e8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    72ec:	e1a08001 	mov	r8, r1
    72f0:	e24dd004 	sub	sp, sp, #4	; 0x4
    72f4:	e1a09000 	mov	r9, r0
    72f8:	e1a06001 	mov	r6, r1
    72fc:	e59fa090 	ldr	sl, [pc, #144]	; 7394 <hash_dma_transfer+0xac>
    7300:	e3a0b000 	mov	fp, #0	; 0x0
    7304:	ea00001b 	b	7378 <hash_dma_transfer+0x90>
    7308:	e3560902 	cmp	r6, #32768	; 0x8000
    730c:	b1a07006 	movlt	r7, r6
    7310:	a3a07902 	movge	r7, #32768	; 0x8000
    7314:	e0661008 	rsb	r1, r6, r8
    7318:	e0891001 	add	r1, r9, r1
    731c:	e3a00002 	mov	r0, #2	; 0x2
    7320:	e1a02007 	mov	r2, r7
    7324:	ebfffe6b 	bl	6cd8 <dma_sdma_init>
    7328:	e3500000 	cmp	r0, #0	; 0x0
    732c:	1a000015 	bne	7388 <hash_dma_transfer+0xa0>
    7330:	e2800002 	add	r0, r0, #2	; 0x2
    7334:	ebfffe57 	bl	6c98 <dma_start>
    7338:	eb001998 	bl	d9a0 <get_ticks>
    733c:	e090400a 	adds	r4, r0, sl
    7340:	e0a1500b 	adc	r5, r1, fp
    7344:	ea000005 	b	7360 <hash_dma_transfer+0x78>
    7348:	eb001994 	bl	d9a0 <get_ticks>
    734c:	e1510005 	cmp	r1, r5
    7350:	8a00000c 	bhi	7388 <hash_dma_transfer+0xa0>
    7354:	1a000001 	bne	7360 <hash_dma_transfer+0x78>
    7358:	e1500004 	cmp	r0, r4
    735c:	8a000009 	bhi	7388 <hash_dma_transfer+0xa0>
    7360:	e3a01003 	mov	r1, #3	; 0x3
    7364:	e3a00002 	mov	r0, #2	; 0x2
    7368:	ebfffe50 	bl	6cb0 <dma_get_status>
    736c:	e3500000 	cmp	r0, #0	; 0x0
    7370:	0afffff4 	beq	7348 <hash_dma_transfer+0x60>
    7374:	e0676006 	rsb	r6, r7, r6
    7378:	e3560000 	cmp	r6, #0	; 0x0
    737c:	1affffe1 	bne	7308 <hash_dma_transfer+0x20>
    7380:	e1a00006 	mov	r0, r6
    7384:	ea000000 	b	738c <hash_dma_transfer+0xa4>
    7388:	e3a00001 	mov	r0, #1	; 0x1
    738c:	e28dd004 	add	sp, sp, #4	; 0x4
    7390:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7394:	000f4240 	.word	0x000f4240

00007398 <cdbg_log_toggle>:
    7398:	e59f3014 	ldr	r3, [pc, #20]	; 73b4 <cdbg_log_toggle+0x1c>
    739c:	e3500000 	cmp	r0, #0	; 0x0
    73a0:	e59f2010 	ldr	r2, [pc, #16]	; 73b8 <cdbg_log_toggle+0x20>
    73a4:	01a02003 	moveq	r2, r3
    73a8:	e59f300c 	ldr	r3, [pc, #12]	; 73bc <cdbg_log_toggle+0x24>
    73ac:	e5832000 	str	r2, [r3]
    73b0:	e12fff1e 	bx	lr
    73b4:	000126e8 	.word	0x000126e8
    73b8:	0001276c 	.word	0x0001276c
    73bc:	3c00d6cc 	.word	0x3c00d6cc

000073c0 <cdbg_wipe_bootarea>:
    73c0:	e59f3018 	ldr	r3, [pc, #24]	; 73e0 <cdbg_wipe_bootarea+0x20>
    73c4:	e59f1018 	ldr	r1, [pc, #24]	; 73e4 <cdbg_wipe_bootarea+0x24>
    73c8:	e59f2018 	ldr	r2, [pc, #24]	; 73e8 <cdbg_wipe_bootarea+0x28>
    73cc:	e5031004 	str	r1, [r3, #-4]
    73d0:	e2833004 	add	r3, r3, #4	; 0x4
    73d4:	e1530002 	cmp	r3, r2
    73d8:	1afffffb 	bne	73cc <cdbg_wipe_bootarea+0xc>
    73dc:	e12fff1e 	bx	lr
    73e0:	3c000004 	.word	0x3c000004
    73e4:	eafffffe 	.word	0xeafffffe
    73e8:	3c008004 	.word	0x3c008004

000073ec <cdbg_verify_burn>:
    73ec:	e59f3004 	ldr	r3, [pc, #4]	; 73f8 <cdbg_verify_burn+0xc>
    73f0:	e5930000 	ldr	r0, [r3]
    73f4:	e12fff1e 	bx	lr
    73f8:	3c00d1a4 	.word	0x3c00d1a4

000073fc <cdbg_verify_burn_key>:
    73fc:	e59f0000 	ldr	r0, [pc, #0]	; 7404 <cdbg_verify_burn_key+0x8>
    7400:	e12fff1e 	bx	lr
    7404:	3c00d1a8 	.word	0x3c00d1a8

00007408 <cdbg_verify_burn_enable>:
    7408:	e3500000 	cmp	r0, #0	; 0x0
    740c:	e59f3018 	ldr	r3, [pc, #24]	; 742c <cdbg_verify_burn_enable+0x24>
    7410:	03a02000 	moveq	r2, #0	; 0x0
    7414:	13a02001 	movne	r2, #1	; 0x1
    7418:	e5832000 	str	r2, [r3]
    741c:	012fff1e 	bxeq	lr
    7420:	e59f0008 	ldr	r0, [pc, #8]	; 7430 <cdbg_verify_burn_enable+0x28>
    7424:	e3a02020 	mov	r2, #32	; 0x20
    7428:	eaffe84b 	b	155c <memcpy>
    742c:	3c00d1a4 	.word	0x3c00d1a4
    7430:	3c00d1a8 	.word	0x3c00d1a8

00007434 <cdbg_jump>:
    7434:	e92d4030 	push	{r4, r5, lr}
    7438:	e1a04000 	mov	r4, r0
    743c:	e24dd004 	sub	sp, sp, #4	; 0x4
    7440:	e1a05001 	mov	r5, r1
    7444:	ebfffc6c 	bl	65fc <ss_lock_isk>
    7448:	e59f001c 	ldr	r0, [pc, #28]	; 746c <cdbg_jump+0x38>
    744c:	e1a01004 	mov	r1, r4
    7450:	e59f3018 	ldr	r3, [pc, #24]	; 7470 <cdbg_jump+0x3c>
    7454:	e1a0e00f 	mov	lr, pc
    7458:	e593f000 	ldr	pc, [r3]
    745c:	e3000bb8 	movw	r0, #3000	; 0xbb8
    7460:	eb00195f 	bl	d9e4 <udelay>
    7464:	e1a00005 	mov	r0, r5
    7468:	e12fff34 	blx	r4
    746c:	00017c48 	.word	0x00017c48
    7470:	3c00d6cc 	.word	0x3c00d6cc

00007474 <cdbg_restore_os>:
    7474:	e92d4010 	push	{r4, lr}
    7478:	ebffe4eb 	bl	82c <get_wakeup_entry>
    747c:	e59f301c 	ldr	r3, [pc, #28]	; 74a0 <cdbg_restore_os+0x2c>
    7480:	e1a04000 	mov	r4, r0
    7484:	e59f0018 	ldr	r0, [pc, #24]	; 74a4 <cdbg_restore_os+0x30>
    7488:	e1a0e00f 	mov	lr, pc
    748c:	e593f000 	ldr	pc, [r3]
    7490:	e1a00004 	mov	r0, r4
    7494:	e3a01000 	mov	r1, #0	; 0x0
    7498:	e8bd4010 	pop	{r4, lr}
    749c:	eaffffe4 	b	7434 <cdbg_jump>
    74a0:	3c00d6cc 	.word	0x3c00d6cc
    74a4:	00017c58 	.word	0x00017c58

000074a8 <cdbg_get_pp>:
    74a8:	e92d4070 	push	{r4, r5, r6, lr}
    74ac:	e24dd008 	sub	sp, sp, #8	; 0x8
    74b0:	e1a0000d 	mov	r0, sp
    74b4:	e59f103c 	ldr	r1, [pc, #60]	; 74f8 <cdbg_get_pp+0x50>
    74b8:	e3a02008 	mov	r2, #8	; 0x8
    74bc:	ebffe826 	bl	155c <memcpy>
    74c0:	e1a0400d 	mov	r4, sp
    74c4:	e1a0600d 	mov	r6, sp
    74c8:	ea000004 	b	74e0 <cdbg_get_pp+0x38>
    74cc:	eb002cc4 	bl	127e4 <getc>
    74d0:	e1500004 	cmp	r0, r4
    74d4:	e2854001 	add	r4, r5, #1	; 0x1
    74d8:	e1a05006 	mov	r5, r6
    74dc:	1a000000 	bne	74e4 <cdbg_get_pp+0x3c>
    74e0:	e1a05004 	mov	r5, r4
    74e4:	e5d54000 	ldrb	r4, [r5]
    74e8:	e3540000 	cmp	r4, #0	; 0x0
    74ec:	1afffff6 	bne	74cc <cdbg_get_pp+0x24>
    74f0:	e28dd008 	add	sp, sp, #8	; 0x8
    74f4:	e8bd8070 	pop	{r4, r5, r6, pc}
    74f8:	00015f66 	.word	0x00015f66

000074fc <cdbg_shell>:
    74fc:	e92d4010 	push	{r4, lr}
    7500:	ebfffc3d 	bl	65fc <ss_lock_isk>
    7504:	e59f3048 	ldr	r3, [pc, #72]	; 7554 <cdbg_shell+0x58>
    7508:	e59f1048 	ldr	r1, [pc, #72]	; 7558 <cdbg_shell+0x5c>
    750c:	e59f2048 	ldr	r2, [pc, #72]	; 755c <cdbg_shell+0x60>
    7510:	e5031004 	str	r1, [r3, #-4]
    7514:	e2833004 	add	r3, r3, #4	; 0x4
    7518:	e1530002 	cmp	r3, r2
    751c:	1afffffb 	bne	7510 <cdbg_shell+0x14>
    7520:	e3a00001 	mov	r0, #1	; 0x1
    7524:	eb003488 	bl	1474c <ss_jtag_en>
    7528:	e59f4030 	ldr	r4, [pc, #48]	; 7560 <cdbg_shell+0x64>
    752c:	e59f3030 	ldr	r3, [pc, #48]	; 7564 <cdbg_shell+0x68>
    7530:	e59f0030 	ldr	r0, [pc, #48]	; 7568 <cdbg_shell+0x6c>
    7534:	e5843000 	str	r3, [r4]
    7538:	eb002c8b 	bl	1276c <printf_on>
    753c:	ebffffd9 	bl	74a8 <cdbg_get_pp>
    7540:	e59f0024 	ldr	r0, [pc, #36]	; 756c <cdbg_shell+0x70>
    7544:	e1a0e00f 	mov	lr, pc
    7548:	e594f000 	ldr	pc, [r4]
    754c:	eb002f8f 	bl	13390 <main_loop>
    7550:	eafffffd 	b	754c <cdbg_shell+0x50>
    7554:	3c000004 	.word	0x3c000004
    7558:	eafffffe 	.word	0xeafffffe
    755c:	3c008004 	.word	0x3c008004
    7560:	3c00d6cc 	.word	0x3c00d6cc
    7564:	0001276c 	.word	0x0001276c
    7568:	00017c67 	.word	0x00017c67
    756c:	00017c6c 	.word	0x00017c6c

00007570 <cdbg_do_config>:
    7570:	e92d4070 	push	{r4, r5, r6, lr}
    7574:	e59f30ec 	ldr	r3, [pc, #236]	; 7668 <cdbg_do_config+0xf8>
    7578:	e1a04000 	mov	r4, r0
    757c:	e1a06001 	mov	r6, r1
    7580:	e59f00e4 	ldr	r0, [pc, #228]	; 766c <cdbg_do_config+0xfc>
    7584:	e59f10e4 	ldr	r1, [pc, #228]	; 7670 <cdbg_do_config+0x100>
    7588:	e1a0e00f 	mov	lr, pc
    758c:	e593f000 	ldr	pc, [r3]
    7590:	e3a02000 	mov	r2, #0	; 0x0
    7594:	e59f00d4 	ldr	r0, [pc, #212]	; 7670 <cdbg_do_config+0x100>
    7598:	ea000006 	b	75b8 <cdbg_do_config+0x48>
    759c:	e7943002 	ldr	r3, [r4, r2]
    75a0:	e2822004 	add	r2, r2, #4	; 0x4
    75a4:	e1530000 	cmp	r3, r0
    75a8:	1a000002 	bne	75b8 <cdbg_do_config+0x48>
    75ac:	e3510000 	cmp	r1, #0	; 0x0
    75b0:	1a000004 	bne	75c8 <cdbg_do_config+0x58>
    75b4:	ea000007 	b	75d8 <cdbg_do_config+0x68>
    75b8:	e1520006 	cmp	r2, r6
    75bc:	e0841002 	add	r1, r4, r2
    75c0:	bafffff5 	blt	759c <cdbg_do_config+0x2c>
    75c4:	ea000003 	b	75d8 <cdbg_do_config+0x68>
    75c8:	e5910004 	ldr	r0, [r1, #4]
    75cc:	eb001b74 	bl	e3a4 <spi_set_bytes>
    75d0:	e3a05000 	mov	r5, #0	; 0x0
    75d4:	ea000004 	b	75ec <cdbg_do_config+0x7c>
    75d8:	e59f0094 	ldr	r0, [pc, #148]	; 7674 <cdbg_do_config+0x104>
    75dc:	e59f3084 	ldr	r3, [pc, #132]	; 7668 <cdbg_do_config+0xf8>
    75e0:	e1a0e00f 	mov	lr, pc
    75e4:	e593f000 	ldr	pc, [r3]
    75e8:	e3e05000 	mvn	r5, #0	; 0x0
    75ec:	e59f1084 	ldr	r1, [pc, #132]	; 7678 <cdbg_do_config+0x108>
    75f0:	e59f0074 	ldr	r0, [pc, #116]	; 766c <cdbg_do_config+0xfc>
    75f4:	e59f306c 	ldr	r3, [pc, #108]	; 7668 <cdbg_do_config+0xf8>
    75f8:	e1a0e00f 	mov	lr, pc
    75fc:	e593f000 	ldr	pc, [r3]
    7600:	e3a02000 	mov	r2, #0	; 0x0
    7604:	e59f106c 	ldr	r1, [pc, #108]	; 7678 <cdbg_do_config+0x108>
    7608:	ea000006 	b	7628 <cdbg_do_config+0xb8>
    760c:	e7943002 	ldr	r3, [r4, r2]
    7610:	e2822004 	add	r2, r2, #4	; 0x4
    7614:	e1530001 	cmp	r3, r1
    7618:	1a000002 	bne	7628 <cdbg_do_config+0xb8>
    761c:	e3500000 	cmp	r0, #0	; 0x0
    7620:	1a000004 	bne	7638 <cdbg_do_config+0xc8>
    7624:	ea000008 	b	764c <cdbg_do_config+0xdc>
    7628:	e1520006 	cmp	r2, r6
    762c:	e0840002 	add	r0, r4, r2
    7630:	bafffff5 	blt	760c <cdbg_do_config+0x9c>
    7634:	ea000004 	b	764c <cdbg_do_config+0xdc>
    7638:	eb001042 	bl	b748 <nand_rescan_config>
    763c:	e3500000 	cmp	r0, #0	; 0x0
    7640:	01a03000 	moveq	r3, r0
    7644:	13e03000 	mvnne	r3, #0	; 0x0
    7648:	ea000004 	b	7660 <cdbg_do_config+0xf0>
    764c:	e59f3014 	ldr	r3, [pc, #20]	; 7668 <cdbg_do_config+0xf8>
    7650:	e59f0024 	ldr	r0, [pc, #36]	; 767c <cdbg_do_config+0x10c>
    7654:	e1a0e00f 	mov	lr, pc
    7658:	e593f000 	ldr	pc, [r3]
    765c:	e3e03000 	mvn	r3, #0	; 0x0
    7660:	e0000395 	mul	r0, r5, r3
    7664:	e8bd8070 	pop	{r4, r5, r6, pc}
    7668:	3c00d6cc 	.word	0x3c00d6cc
    766c:	00017c7d 	.word	0x00017c7d
    7670:	4a82467c 	.word	0x4a82467c
    7674:	00017c9a 	.word	0x00017c9a
    7678:	aca4570e 	.word	0xaca4570e
    767c:	00017cb9 	.word	0x00017cb9

00007680 <cdbg_config_isi>:
    7680:	e92d4030 	push	{r4, r5, lr}
    7684:	e24dd004 	sub	sp, sp, #4	; 0x4
    7688:	e1a04000 	mov	r4, r0
    768c:	ebffe975 	bl	1c68 <isi_check>
    7690:	e2505000 	subs	r5, r0, #0	; 0x0
    7694:	0a000007 	beq	76b8 <cdbg_config_isi+0x38>
    7698:	e59f002c 	ldr	r0, [pc, #44]	; 76cc <cdbg_config_isi+0x4c>
    769c:	e1a01005 	mov	r1, r5
    76a0:	e59f3028 	ldr	r3, [pc, #40]	; 76d0 <cdbg_config_isi+0x50>
    76a4:	e1a0e00f 	mov	lr, pc
    76a8:	e593f000 	ldr	pc, [r3]
    76ac:	e1a00005 	mov	r0, r5
    76b0:	e28dd004 	add	sp, sp, #4	; 0x4
    76b4:	e8bd8030 	pop	{r4, r5, pc}
    76b8:	e594100c 	ldr	r1, [r4, #12]
    76bc:	e2840c02 	add	r0, r4, #512	; 0x200
    76c0:	e28dd004 	add	sp, sp, #4	; 0x4
    76c4:	e8bd4030 	pop	{r4, r5, lr}
    76c8:	eaffffa8 	b	7570 <cdbg_do_config>
    76cc:	00017cd9 	.word	0x00017cd9
    76d0:	3c00d6cc 	.word	0x3c00d6cc

000076d4 <burn_image_loop>:
    76d4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    76d8:	e24dd04c 	sub	sp, sp, #76	; 0x4c
    76dc:	e59db074 	ldr	fp, [sp, #116]
    76e0:	e1a06000 	mov	r6, r0
    76e4:	e35b0000 	cmp	fp, #0	; 0x0
    76e8:	e58d102c 	str	r1, [sp, #44]
    76ec:	e58d2028 	str	r2, [sp, #40]
    76f0:	e58d3024 	str	r3, [sp, #36]
    76f4:	e59da070 	ldr	sl, [sp, #112]
    76f8:	1a00000c 	bne	7730 <burn_image_loop+0x5c>
    76fc:	e59f449c 	ldr	r4, [pc, #1180]	; 7ba0 <burn_image_loop+0x4cc>
    7700:	e59f049c 	ldr	r0, [pc, #1180]	; 7ba4 <burn_image_loop+0x4d0>
    7704:	e3a0102e 	mov	r1, #46	; 0x2e
    7708:	e1a0e00f 	mov	lr, pc
    770c:	e594f000 	ldr	pc, [r4]
    7710:	e59f0490 	ldr	r0, [pc, #1168]	; 7ba8 <burn_image_loop+0x4d4>
    7714:	e59f1490 	ldr	r1, [pc, #1168]	; 7bac <burn_image_loop+0x4d8>
    7718:	e1a0e00f 	mov	lr, pc
    771c:	e594f000 	ldr	pc, [r4]
    7720:	e59f0488 	ldr	r0, [pc, #1160]	; 7bb0 <burn_image_loop+0x4dc>
    7724:	e1a0e00f 	mov	lr, pc
    7728:	e594f000 	ldr	pc, [r4]
    772c:	eafffffe 	b	772c <burn_image_loop+0x58>
    7730:	e59d002c 	ldr	r0, [sp, #44]
    7734:	e59d1024 	ldr	r1, [sp, #36]
    7738:	e1500001 	cmp	r0, r1
    773c:	1a000005 	bne	7758 <burn_image_loop+0x84>
    7740:	e59f046c 	ldr	r0, [pc, #1132]	; 7bb4 <burn_image_loop+0x4e0>
    7744:	e59f3454 	ldr	r3, [pc, #1108]	; 7ba0 <burn_image_loop+0x4cc>
    7748:	e1a0e00f 	mov	lr, pc
    774c:	e593f000 	ldr	pc, [r3]
    7750:	e3e00001 	mvn	r0, #1	; 0x1
    7754:	ea00010f 	b	7b98 <burn_image_loop+0x4c4>
    7758:	e596c000 	ldr	ip, [r6]
    775c:	e3a03000 	mov	r3, #0	; 0x0
    7760:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    7764:	e002200c 	and	r2, r2, ip
    7768:	e1a01483 	lsl	r1, r3, #9
    776c:	e1811ba2 	orr	r1, r1, r2, lsr #23
    7770:	e1a0e482 	lsl	lr, r2, #9
    7774:	e1a0200e 	mov	r2, lr
    7778:	e1a03001 	mov	r3, r1
    777c:	e1cd23f0 	strd	r2, [sp, #48]
    7780:	e1cd83d0 	ldrd	r8, [sp, #48]
    7784:	e1d600b8 	ldrh	r0, [r6, #8]
    7788:	e3a02401 	mov	r2, #16777216	; 0x1000000
    778c:	e0988002 	adds	r8, r8, r2
    7790:	e5961004 	ldr	r1, [r6, #4]
    7794:	e3a03000 	mov	r3, #0	; 0x0
    7798:	e1a0cc2c 	lsr	ip, ip, #24
    779c:	e0a99003 	adc	r9, r9, r3
    77a0:	e1a05000 	mov	r5, r0
    77a4:	e3a04000 	mov	r4, #0	; 0x0
    77a8:	e20cc03f 	and	ip, ip, #63	; 0x3f
    77ac:	e59d0024 	ldr	r0, [sp, #36]
    77b0:	e1cd83f0 	strd	r8, [sp, #48]
    77b4:	e58dc03c 	str	ip, [sp, #60]
    77b8:	e1848001 	orr	r8, r4, r1
    77bc:	e1a09005 	mov	r9, r5
    77c0:	e596500c 	ldr	r5, [r6, #12]
    77c4:	ebfffa5f 	bl	6148 <vs_device_erasable>
    77c8:	e1500004 	cmp	r0, r4
    77cc:	0a000006 	beq	77ec <burn_image_loop+0x118>
    77d0:	e1a01004 	mov	r1, r4
    77d4:	e59d0024 	ldr	r0, [sp, #36]
    77d8:	ebfffb41 	bl	64e4 <vs_assign_by_id>
    77dc:	e1a00008 	mov	r0, r8
    77e0:	e1a01009 	mov	r1, r9
    77e4:	e1a0200a 	mov	r2, sl
    77e8:	ebfff9ad 	bl	5ea4 <vs_erase>
    77ec:	e59d002c 	ldr	r0, [sp, #44]
    77f0:	e3a01001 	mov	r1, #1	; 0x1
    77f4:	ebfffb3a 	bl	64e4 <vs_assign_by_id>
    77f8:	e59d003c 	ldr	r0, [sp, #60]
    77fc:	e2403002 	sub	r3, r0, #2	; 0x2
    7800:	e3530001 	cmp	r3, #1	; 0x1
    7804:	8a000018 	bhi	786c <burn_image_loop+0x198>
    7808:	e3a01c02 	mov	r1, #512	; 0x200
    780c:	e58d1000 	str	r1, [sp]
    7810:	e59d1028 	ldr	r1, [sp, #40]
    7814:	e1cd23d0 	ldrd	r2, [sp, #48]
    7818:	e1a0000b 	mov	r0, fp
    781c:	e58d1004 	str	r1, [sp, #4]
    7820:	ebfffa89 	bl	624c <vs_read>
    7824:	e1a0000b 	mov	r0, fp
    7828:	ebffe8b3 	bl	1afc <isi_check_header>
    782c:	e2504000 	subs	r4, r0, #0	; 0x0
    7830:	1a00000d 	bne	786c <burn_image_loop+0x198>
    7834:	e59b3004 	ldr	r3, [fp, #4]
    7838:	e1b03e23 	lsrs	r3, r3, #28
    783c:	0a00000a 	beq	786c <burn_image_loop+0x198>
    7840:	e59f0370 	ldr	r0, [pc, #880]	; 7bb8 <burn_image_loop+0x4e4>
    7844:	e59f3354 	ldr	r3, [pc, #852]	; 7ba0 <burn_image_loop+0x4cc>
    7848:	e1a0e00f 	mov	lr, pc
    784c:	e593f000 	ldr	pc, [r3]
    7850:	e59f2364 	ldr	r2, [pc, #868]	; 7bbc <burn_image_loop+0x4e8>
    7854:	e1a01004 	mov	r1, r4
    7858:	e3a00001 	mov	r0, #1	; 0x1
    785c:	ebfffb81 	bl	6668 <ss_aes_init>
    7860:	e3a02001 	mov	r2, #1	; 0x1
    7864:	e58d2044 	str	r2, [sp, #68]
    7868:	ea000001 	b	7874 <burn_image_loop+0x1a0>
    786c:	e3a03000 	mov	r3, #0	; 0x0
    7870:	e58d3044 	str	r3, [sp, #68]
    7874:	e59d003c 	ldr	r0, [sp, #60]
    7878:	e3500005 	cmp	r0, #5	; 0x5
    787c:	11a06005 	movne	r6, r5
    7880:	13a07901 	movne	r7, #16384	; 0x4000
    7884:	1a000010 	bne	78cc <burn_image_loop+0x1f8>
    7888:	e59d0024 	ldr	r0, [sp, #36]
    788c:	ebfff9c0 	bl	5f94 <vs_align>
    7890:	e1a04000 	mov	r4, r0
    7894:	e1a00005 	mov	r0, r5
    7898:	eb0009ed 	bl	a054 <fnd_size_shrink>
    789c:	e1a06000 	mov	r6, r0
    78a0:	e1a00005 	mov	r0, r5
    78a4:	eb0009dc 	bl	a01c <fnd_size_match>
    78a8:	e3500000 	cmp	r0, #0	; 0x0
    78ac:	11a07004 	movne	r7, r4
    78b0:	1a000005 	bne	78cc <burn_image_loop+0x1f8>
    78b4:	e59f0304 	ldr	r0, [pc, #772]	; 7bc0 <burn_image_loop+0x4ec>
    78b8:	e59f32e0 	ldr	r3, [pc, #736]	; 7ba0 <burn_image_loop+0x4cc>
    78bc:	e1a0e00f 	mov	lr, pc
    78c0:	e593f000 	ldr	pc, [r3]
    78c4:	e3e00000 	mvn	r0, #0	; 0x0
    78c8:	ea0000b2 	b	7b98 <burn_image_loop+0x4c4>
    78cc:	e59d0024 	ldr	r0, [sp, #36]
    78d0:	e59f12ec 	ldr	r1, [pc, #748]	; 7bc4 <burn_image_loop+0x4f0>
    78d4:	ebfffa43 	bl	61e8 <vs_is_device>
    78d8:	e3500000 	cmp	r0, #0	; 0x0
    78dc:	1a00000a 	bne	790c <burn_image_loop+0x238>
    78e0:	e59d0024 	ldr	r0, [sp, #36]
    78e4:	e59f12dc 	ldr	r1, [pc, #732]	; 7bc8 <burn_image_loop+0x4f4>
    78e8:	ebfffa3e 	bl	61e8 <vs_is_device>
    78ec:	e3500000 	cmp	r0, #0	; 0x0
    78f0:	1a000005 	bne	790c <burn_image_loop+0x238>
    78f4:	e59d0024 	ldr	r0, [sp, #36]
    78f8:	e59f12cc 	ldr	r1, [pc, #716]	; 7bcc <burn_image_loop+0x4f8>
    78fc:	ebfffa39 	bl	61e8 <vs_is_device>
    7900:	e3500000 	cmp	r0, #0	; 0x0
    7904:	01a05000 	moveq	r5, r0
    7908:	0a00001a 	beq	7978 <burn_image_loop+0x2a4>
    790c:	e59d0024 	ldr	r0, [sp, #36]
    7910:	e59f12b4 	ldr	r1, [pc, #692]	; 7bcc <burn_image_loop+0x4f8>
    7914:	ebfffa33 	bl	61e8 <vs_is_device>
    7918:	e3500000 	cmp	r0, #0	; 0x0
    791c:	0a00000a 	beq	794c <burn_image_loop+0x278>
    7920:	e3a00000 	mov	r0, #0	; 0x0
    7924:	eb000984 	bl	9f3c <nand_get_config>
    7928:	e3500000 	cmp	r0, #0	; 0x0
    792c:	0a00000b 	beq	7960 <burn_image_loop+0x28c>
    7930:	e590301c 	ldr	r3, [r0, #28]
    7934:	e3550902 	cmp	r5, #32768	; 0x8000
    7938:	33a06902 	movcc	r6, #32768	; 0x8000
    793c:	e3530802 	cmp	r3, #131072	; 0x20000
    7940:	21a05003 	movcs	r5, r3
    7944:	33a05802 	movcc	r5, #131072	; 0x20000
    7948:	ea00000a 	b	7978 <burn_image_loop+0x2a4>
    794c:	e3a00001 	mov	r0, #1	; 0x1
    7950:	eb000979 	bl	9f3c <nand_get_config>
    7954:	e3500000 	cmp	r0, #0	; 0x0
    7958:	1590501c 	ldrne	r5, [r0, #28]
    795c:	1a000005 	bne	7978 <burn_image_loop+0x2a4>
    7960:	e59f0268 	ldr	r0, [pc, #616]	; 7bd0 <burn_image_loop+0x4fc>
    7964:	e59f3234 	ldr	r3, [pc, #564]	; 7ba0 <burn_image_loop+0x4cc>
    7968:	e1a0e00f 	mov	lr, pc
    796c:	e593f000 	ldr	pc, [r3]
    7970:	e3e00033 	mvn	r0, #51	; 0x33
    7974:	ea000087 	b	7b98 <burn_image_loop+0x4c4>
    7978:	e1a0100a 	mov	r1, sl
    797c:	e59f0250 	ldr	r0, [pc, #592]	; 7bd4 <burn_image_loop+0x500>
    7980:	e1a02007 	mov	r2, r7
    7984:	e59f4214 	ldr	r4, [pc, #532]	; 7ba0 <burn_image_loop+0x4cc>
    7988:	e1a0e00f 	mov	lr, pc
    798c:	e594f000 	ldr	pc, [r4]
    7990:	e2453001 	sub	r3, r5, #1	; 0x1
    7994:	e1a00003 	mov	r0, r3
    7998:	e1a01fc0 	asr	r1, r0, #31
    799c:	e066a00a 	rsb	sl, r6, sl
    79a0:	e1cd01f8 	strd	r0, [sp, #24]
    79a4:	e1a02005 	mov	r2, r5
    79a8:	e1a03fc2 	asr	r3, r2, #31
    79ac:	e1a00007 	mov	r0, r7
    79b0:	e3a01000 	mov	r1, #0	; 0x0
    79b4:	e58da040 	str	sl, [sp, #64]
    79b8:	e58d6038 	str	r6, [sp, #56]
    79bc:	e3a0a000 	mov	sl, #0	; 0x0
    79c0:	e1cd21f0 	strd	r2, [sp, #16]
    79c4:	e1cd00f8 	strd	r0, [sp, #8]
    79c8:	ea00006f 	b	7b8c <burn_image_loop+0x4b8>
    79cc:	e59d1038 	ldr	r1, [sp, #56]
    79d0:	e0663001 	rsb	r3, r6, r1
    79d4:	e1b03a23 	lsrs	r3, r3, #20
    79d8:	0a000005 	beq	79f4 <burn_image_loop+0x320>
    79dc:	e59f01f4 	ldr	r0, [pc, #500]	; 7bd8 <burn_image_loop+0x504>
    79e0:	e1a01a26 	lsr	r1, r6, #20
    79e4:	e59f21b4 	ldr	r2, [pc, #436]	; 7ba0 <burn_image_loop+0x4cc>
    79e8:	e1a0e00f 	mov	lr, pc
    79ec:	e592f000 	ldr	pc, [r2]
    79f0:	e58d6038 	str	r6, [sp, #56]
    79f4:	e35a0000 	cmp	sl, #0	; 0x0
    79f8:	d3a00000 	movle	r0, #0	; 0x0
    79fc:	c3a00001 	movgt	r0, #1	; 0x1
    7a00:	e28aa001 	add	sl, sl, #1	; 0x1
    7a04:	eb0021c4 	bl	1011c <led_ctrl>
    7a08:	e35a0032 	cmp	sl, #50	; 0x32
    7a0c:	e3a01000 	mov	r1, #0	; 0x0
    7a10:	e59d002c 	ldr	r0, [sp, #44]
    7a14:	c24aa064 	subgt	sl, sl, #100	; 0x64
    7a18:	ebfffab1 	bl	64e4 <vs_assign_by_id>
    7a1c:	e59d303c 	ldr	r3, [sp, #60]
    7a20:	e3530005 	cmp	r3, #5	; 0x5
    7a24:	11a01007 	movne	r1, r7
    7a28:	1a000001 	bne	7a34 <burn_image_loop+0x360>
    7a2c:	eb000970 	bl	9ff4 <fnd_align>
    7a30:	e1a01000 	mov	r1, r0
    7a34:	e58d1000 	str	r1, [sp]
    7a38:	e59d1028 	ldr	r1, [sp, #40]
    7a3c:	e1cd23d0 	ldrd	r2, [sp, #48]
    7a40:	e1a0000b 	mov	r0, fp
    7a44:	e58d1004 	str	r1, [sp, #4]
    7a48:	ebfff9ff 	bl	624c <vs_read>
    7a4c:	e59d2044 	ldr	r2, [sp, #68]
    7a50:	e3520000 	cmp	r2, #0	; 0x0
    7a54:	0a000003 	beq	7a68 <burn_image_loop+0x394>
    7a58:	e1a0000b 	mov	r0, fp
    7a5c:	e1a0100b 	mov	r1, fp
    7a60:	e1a02007 	mov	r2, r7
    7a64:	ebfffaee 	bl	6624 <ss_aes_data>
    7a68:	e59d0024 	ldr	r0, [sp, #36]
    7a6c:	e3a01000 	mov	r1, #0	; 0x0
    7a70:	ebfffa9b 	bl	64e4 <vs_assign_by_id>
    7a74:	e3500000 	cmp	r0, #0	; 0x0
    7a78:	1a00002e 	bne	7b38 <burn_image_loop+0x464>
    7a7c:	e3550000 	cmp	r5, #0	; 0x0
    7a80:	0a00001f 	beq	7b04 <burn_image_loop+0x430>
    7a84:	e1cd01d8 	ldrd	r0, [sp, #24]
    7a88:	e0082000 	and	r2, r8, r0
    7a8c:	e0093001 	and	r3, r9, r1
    7a90:	e1921003 	orrs	r1, r2, r3
    7a94:	0a000012 	beq	7ae4 <burn_image_loop+0x410>
    7a98:	ea000019 	b	7b04 <burn_image_loop+0x430>
    7a9c:	e59d1040 	ldr	r1, [sp, #64]
    7aa0:	e1510005 	cmp	r1, r5
    7aa4:	e0651001 	rsb	r1, r5, r1
    7aa8:	e58d1040 	str	r1, [sp, #64]
    7aac:	da000006 	ble	7acc <burn_image_loop+0x3f8>
    7ab0:	e59f10e8 	ldr	r1, [pc, #232]	; 7ba0 <burn_image_loop+0x4cc>
    7ab4:	e1a0e00f 	mov	lr, pc
    7ab8:	e591f000 	ldr	pc, [r1]
    7abc:	e1cd21d0 	ldrd	r2, [sp, #16]
    7ac0:	e0988002 	adds	r8, r8, r2
    7ac4:	e0a99003 	adc	r9, r9, r3
    7ac8:	ea000005 	b	7ae4 <burn_image_loop+0x410>
    7acc:	e59f0108 	ldr	r0, [pc, #264]	; 7bdc <burn_image_loop+0x508>
    7ad0:	e59f30c8 	ldr	r3, [pc, #200]	; 7ba0 <burn_image_loop+0x4cc>
    7ad4:	e1a0e00f 	mov	lr, pc
    7ad8:	e593f000 	ldr	pc, [r3]
    7adc:	e3e00034 	mvn	r0, #52	; 0x34
    7ae0:	ea00002c 	b	7b98 <burn_image_loop+0x4c4>
    7ae4:	e1a00008 	mov	r0, r8
    7ae8:	e1a01009 	mov	r1, r9
    7aec:	ebfff8fb 	bl	5ee0 <vs_isbad>
    7af0:	e3500000 	cmp	r0, #0	; 0x0
    7af4:	e1a02008 	mov	r2, r8
    7af8:	e1a03009 	mov	r3, r9
    7afc:	e59f00dc 	ldr	r0, [pc, #220]	; 7be0 <burn_image_loop+0x50c>
    7b00:	1affffe5 	bne	7a9c <burn_image_loop+0x3c8>
    7b04:	e59d303c 	ldr	r3, [sp, #60]
    7b08:	e3530005 	cmp	r3, #5	; 0x5
    7b0c:	11a01007 	movne	r1, r7
    7b10:	1a000001 	bne	7b1c <burn_image_loop+0x448>
    7b14:	eb000936 	bl	9ff4 <fnd_align>
    7b18:	e1a01000 	mov	r1, r0
    7b1c:	e58d1000 	str	r1, [sp]
    7b20:	e1a0000b 	mov	r0, fp
    7b24:	e3a01000 	mov	r1, #0	; 0x0
    7b28:	e1a02008 	mov	r2, r8
    7b2c:	e1a03009 	mov	r3, r9
    7b30:	e58d1004 	str	r1, [sp, #4]
    7b34:	ebfff8c4 	bl	5e4c <vs_write>
    7b38:	e59d003c 	ldr	r0, [sp, #60]
    7b3c:	e3500001 	cmp	r0, #1	; 0x1
    7b40:	91a04007 	movls	r4, r7
    7b44:	83a04000 	movhi	r4, #0	; 0x0
    7b48:	e3500005 	cmp	r0, #5	; 0x5
    7b4c:	11a0c007 	movne	ip, r7
    7b50:	1a000001 	bne	7b5c <burn_image_loop+0x488>
    7b54:	eb000926 	bl	9ff4 <fnd_align>
    7b58:	e1a0c000 	mov	ip, r0
    7b5c:	e1cd03d0 	ldrd	r0, [sp, #48]
    7b60:	e1560007 	cmp	r6, r7
    7b64:	31a0e006 	movcc	lr, r6
    7b68:	21a0e007 	movcs	lr, r7
    7b6c:	e090200c 	adds	r2, r0, ip
    7b70:	e2a13000 	adc	r3, r1, #0	; 0x0
    7b74:	e1cd23f0 	strd	r2, [sp, #48]
    7b78:	e1cd20d8 	ldrd	r2, [sp, #8]
    7b7c:	e0988002 	adds	r8, r8, r2
    7b80:	e08bb004 	add	fp, fp, r4
    7b84:	e0a99003 	adc	r9, r9, r3
    7b88:	e06e6006 	rsb	r6, lr, r6
    7b8c:	e3560000 	cmp	r6, #0	; 0x0
    7b90:	1affff8d 	bne	79cc <burn_image_loop+0x2f8>
    7b94:	e1a00006 	mov	r0, r6
    7b98:	e28dd04c 	add	sp, sp, #76	; 0x4c
    7b9c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7ba0:	3c00d6cc 	.word	0x3c00d6cc
    7ba4:	00017b59 	.word	0x00017b59
    7ba8:	0001641f 	.word	0x0001641f
    7bac:	00017cf4 	.word	0x00017cf4
    7bb0:	00017cfb 	.word	0x00017cfb
    7bb4:	00017d11 	.word	0x00017d11
    7bb8:	00017d3e 	.word	0x00017d3e
    7bbc:	3c00d088 	.word	0x3c00d088
    7bc0:	00017d57 	.word	0x00017d57
    7bc4:	00016267 	.word	0x00016267
    7bc8:	0001626b 	.word	0x0001626b
    7bcc:	0001610f 	.word	0x0001610f
    7bd0:	00017d74 	.word	0x00017d74
    7bd4:	00017d8c 	.word	0x00017d8c
    7bd8:	00017d9b 	.word	0x00017d9b
    7bdc:	00017dc7 	.word	0x00017dc7
    7be0:	00017da8 	.word	0x00017da8

00007be4 <burn_images>:
    7be4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7be8:	e590b01c 	ldr	fp, [r0, #28]
    7bec:	e24dd014 	sub	sp, sp, #20	; 0x14
    7bf0:	e59f41dc 	ldr	r4, [pc, #476]	; 7dd4 <burn_images+0x1f0>
    7bf4:	e1a05000 	mov	r5, r0
    7bf8:	e58d100c 	str	r1, [sp, #12]
    7bfc:	e59f01d4 	ldr	r0, [pc, #468]	; 7dd8 <burn_images+0x1f4>
    7c00:	e1a0100b 	mov	r1, fp
    7c04:	e3a07000 	mov	r7, #0	; 0x0
    7c08:	e58d2008 	str	r2, [sp, #8]
    7c0c:	e1a0e00f 	mov	lr, pc
    7c10:	e594f000 	ldr	pc, [r4]
    7c14:	e2856020 	add	r6, r5, #32	; 0x20
    7c18:	e1a09004 	mov	r9, r4
    7c1c:	e1a0a007 	mov	sl, r7
    7c20:	ea000061 	b	7dac <burn_images+0x1c8>
    7c24:	e59f01b0 	ldr	r0, [pc, #432]	; 7ddc <burn_images+0x1f8>
    7c28:	e1a01007 	mov	r1, r7
    7c2c:	e1a0e00f 	mov	lr, pc
    7c30:	e599f000 	ldr	pc, [r9]
    7c34:	e5d63003 	ldrb	r3, [r6, #3]
    7c38:	e5968008 	ldr	r8, [r6, #8]
    7c3c:	e203503f 	and	r5, r3, #63	; 0x3f
    7c40:	e3550003 	cmp	r5, #3	; 0x3
    7c44:	0a000010 	beq	7c8c <burn_images+0xa8>
    7c48:	ca000006 	bgt	7c68 <burn_images+0x84>
    7c4c:	e3550001 	cmp	r5, #1	; 0x1
    7c50:	0a000013 	beq	7ca4 <burn_images+0xc0>
    7c54:	c1a04828 	lsrgt	r4, r8, #16
    7c58:	ca000026 	bgt	7cf8 <burn_images+0x114>
    7c5c:	e3550000 	cmp	r5, #0	; 0x0
    7c60:	0a000059 	beq	7dcc <burn_images+0x1e8>
    7c64:	ea00001f 	b	7ce8 <burn_images+0x104>
    7c68:	e3550005 	cmp	r5, #5	; 0x5
    7c6c:	0a000008 	beq	7c94 <burn_images+0xb0>
    7c70:	b59f0168 	ldrlt	r0, [pc, #360]	; 7de0 <burn_images+0x1fc>
    7c74:	ba000007 	blt	7c98 <burn_images+0xb4>
    7c78:	e3550030 	cmp	r5, #48	; 0x30
    7c7c:	0a00000e 	beq	7cbc <burn_images+0xd8>
    7c80:	e3550031 	cmp	r5, #49	; 0x31
    7c84:	1a000017 	bne	7ce8 <burn_images+0x104>
    7c88:	ea000010 	b	7cd0 <burn_images+0xec>
    7c8c:	e59f0150 	ldr	r0, [pc, #336]	; 7de4 <burn_images+0x200>
    7c90:	ea000000 	b	7c98 <burn_images+0xb4>
    7c94:	e59f014c 	ldr	r0, [pc, #332]	; 7de8 <burn_images+0x204>
    7c98:	ebfff93e 	bl	6198 <vs_device_id>
    7c9c:	e1a04000 	mov	r4, r0
    7ca0:	ea000014 	b	7cf8 <burn_images+0x114>
    7ca4:	e59f0140 	ldr	r0, [pc, #320]	; 7dec <burn_images+0x208>
    7ca8:	e1a01008 	mov	r1, r8
    7cac:	e1a0a008 	mov	sl, r8
    7cb0:	e1a0e00f 	mov	lr, pc
    7cb4:	e599f000 	ldr	pc, [r9]
    7cb8:	ea000043 	b	7dcc <burn_images+0x1e8>
    7cbc:	e59f012c 	ldr	r0, [pc, #300]	; 7df0 <burn_images+0x20c>
    7cc0:	e1a0e00f 	mov	lr, pc
    7cc4:	e599f000 	ldr	pc, [r9]
    7cc8:	ebffe2e1 	bl	854 <system_reset>
    7ccc:	ea00003e 	b	7dcc <burn_images+0x1e8>
    7cd0:	e59f011c 	ldr	r0, [pc, #284]	; 7df4 <burn_images+0x210>
    7cd4:	e1a0e00f 	mov	lr, pc
    7cd8:	e599f000 	ldr	pc, [r9]
    7cdc:	e3a00001 	mov	r0, #1	; 0x1
    7ce0:	ebffe1d8 	bl	448 <try_boot>
    7ce4:	ea000038 	b	7dcc <burn_images+0x1e8>
    7ce8:	e59f0108 	ldr	r0, [pc, #264]	; 7df8 <burn_images+0x214>
    7cec:	e1a0e00f 	mov	lr, pc
    7cf0:	e599f000 	ldr	pc, [r9]
    7cf4:	ea00002a 	b	7da4 <burn_images+0x1c0>
    7cf8:	e354000f 	cmp	r4, #15	; 0xf
    7cfc:	0a00000a 	beq	7d2c <burn_images+0x148>
    7d00:	e1a00004 	mov	r0, r4
    7d04:	e3a01001 	mov	r1, #1	; 0x1
    7d08:	ebfff9f5 	bl	64e4 <vs_assign_by_id>
    7d0c:	e3500000 	cmp	r0, #0	; 0x0
    7d10:	0a000005 	beq	7d2c <burn_images+0x148>
    7d14:	e59f00e0 	ldr	r0, [pc, #224]	; 7dfc <burn_images+0x218>
    7d18:	e59f30b4 	ldr	r3, [pc, #180]	; 7dd4 <burn_images+0x1f0>
    7d1c:	e1a0e00f 	mov	lr, pc
    7d20:	e593f000 	ldr	pc, [r3]
    7d24:	e3e04002 	mvn	r4, #2	; 0x2
    7d28:	ea000022 	b	7db8 <burn_images+0x1d4>
    7d2c:	e3550001 	cmp	r5, #1	; 0x1
    7d30:	e1a0c828 	lsr	ip, r8, #16
    7d34:	01a0e00a 	moveq	lr, sl
    7d38:	13a0e30f 	movne	lr, #1006632960	; 0x3c000000
    7d3c:	e1a0c80c 	lsl	ip, ip, #16
    7d40:	e1a03004 	mov	r3, r4
    7d44:	e1a00006 	mov	r0, r6
    7d48:	e59d100c 	ldr	r1, [sp, #12]
    7d4c:	e59d2008 	ldr	r2, [sp, #8]
    7d50:	e88d5000 	stm	sp, {ip, lr}
    7d54:	ebfffe5e 	bl	76d4 <burn_image_loop>
    7d58:	e3500000 	cmp	r0, #0	; 0x0
    7d5c:	aa000006 	bge	7d7c <burn_images+0x198>
    7d60:	e1a01007 	mov	r1, r7
    7d64:	e59f0094 	ldr	r0, [pc, #148]	; 7e00 <burn_images+0x21c>
    7d68:	e59f3064 	ldr	r3, [pc, #100]	; 7dd4 <burn_images+0x1f0>
    7d6c:	e1a0e00f 	mov	lr, pc
    7d70:	e593f000 	ldr	pc, [r3]
    7d74:	e3e04000 	mvn	r4, #0	; 0x0
    7d78:	ea00000e 	b	7db8 <burn_images+0x1d4>
    7d7c:	e3550001 	cmp	r5, #1	; 0x1
    7d80:	1a000003 	bne	7d94 <burn_images+0x1b0>
    7d84:	e5960004 	ldr	r0, [r6, #4]
    7d88:	e3a01000 	mov	r1, #0	; 0x0
    7d8c:	ebfffda8 	bl	7434 <cdbg_jump>
    7d90:	ea000003 	b	7da4 <burn_images+0x1c0>
    7d94:	e3550000 	cmp	r5, #0	; 0x0
    7d98:	1a000001 	bne	7da4 <burn_images+0x1c0>
    7d9c:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
    7da0:	ebfffe36 	bl	7680 <cdbg_config_isi>
    7da4:	e2877001 	add	r7, r7, #1	; 0x1
    7da8:	e2866010 	add	r6, r6, #16	; 0x10
    7dac:	e157000b 	cmp	r7, fp
    7db0:	baffff9b 	blt	7c24 <burn_images+0x40>
    7db4:	e3a04000 	mov	r4, #0	; 0x0
    7db8:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
    7dbc:	ebffe6b3 	bl	1890 <free>
    7dc0:	e1a00004 	mov	r0, r4
    7dc4:	e28dd014 	add	sp, sp, #20	; 0x14
    7dc8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7dcc:	e3a0400f 	mov	r4, #15	; 0xf
    7dd0:	eaffffd5 	b	7d2c <burn_images+0x148>
    7dd4:	3c00d6cc 	.word	0x3c00d6cc
    7dd8:	00017de5 	.word	0x00017de5
    7ddc:	00017e02 	.word	0x00017e02
    7de0:	00016267 	.word	0x00016267
    7de4:	0001610f 	.word	0x0001610f
    7de8:	0001626b 	.word	0x0001626b
    7dec:	00017e18 	.word	0x00017e18
    7df0:	00017e35 	.word	0x00017e35
    7df4:	00017e4a 	.word	0x00017e4a
    7df8:	00017e5d 	.word	0x00017e5d
    7dfc:	00017e71 	.word	0x00017e71
    7e00:	00017e8e 	.word	0x00017e8e

00007e04 <burn>:
    7e04:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    7e08:	e1a09000 	mov	r9, r0
    7e0c:	e24dd00c 	sub	sp, sp, #12	; 0xc
    7e10:	e3a00c02 	mov	r0, #512	; 0x200
    7e14:	ebffe635 	bl	16f0 <malloc>
    7e18:	e2507000 	subs	r7, r0, #0	; 0x0
    7e1c:	1a000004 	bne	7e34 <burn+0x30>
    7e20:	e59f019c 	ldr	r0, [pc, #412]	; 7fc4 <burn+0x1c0>
    7e24:	e59f319c 	ldr	r3, [pc, #412]	; 7fc8 <burn+0x1c4>
    7e28:	e1a0e00f 	mov	lr, pc
    7e2c:	e593f000 	ldr	pc, [r3]
    7e30:	ea000060 	b	7fb8 <burn+0x1b4>
    7e34:	ebffe202 	bl	644 <boot_device>
    7e38:	e3a05000 	mov	r5, #0	; 0x0
    7e3c:	e59f8184 	ldr	r8, [pc, #388]	; 7fc8 <burn+0x1c4>
    7e40:	e3a0bc02 	mov	fp, #512	; 0x200
    7e44:	e1a0a000 	mov	sl, r0
    7e48:	ea00004c 	b	7f80 <burn+0x17c>
    7e4c:	e1a00005 	mov	r0, r5
    7e50:	ebfff877 	bl	6034 <vs_device_burnsrc>
    7e54:	e3500000 	cmp	r0, #0	; 0x0
    7e58:	0a000047 	beq	7f7c <burn+0x178>
    7e5c:	e155000a 	cmp	r5, sl
    7e60:	0a000045 	beq	7f7c <burn+0x178>
    7e64:	e3a03001 	mov	r3, #1	; 0x1
    7e68:	e0193513 	ands	r3, r9, r3, lsl r5
    7e6c:	1a000042 	bne	7f7c <burn+0x178>
    7e70:	ebffe2d9 	bl	9dc <gtc_time>
    7e74:	e1a00005 	mov	r0, r5
    7e78:	e59f114c 	ldr	r1, [pc, #332]	; 7fcc <burn+0x1c8>
    7e7c:	ebfff8d9 	bl	61e8 <vs_is_device>
    7e80:	e3500000 	cmp	r0, #0	; 0x0
    7e84:	1a000004 	bne	7e9c <burn+0x98>
    7e88:	e1a00005 	mov	r0, r5
    7e8c:	e59f113c 	ldr	r1, [pc, #316]	; 7fd0 <burn+0x1cc>
    7e90:	ebfff8d4 	bl	61e8 <vs_is_device>
    7e94:	e3500000 	cmp	r0, #0	; 0x0
    7e98:	0a00000c 	beq	7ed0 <burn+0xcc>
    7e9c:	ebffe360 	bl	c24 <assert_linkpc>
    7ea0:	e3500000 	cmp	r0, #0	; 0x0
    7ea4:	0a000009 	beq	7ed0 <burn+0xcc>
    7ea8:	eb0023a9 	bl	10d54 <linkpc_loop>
    7eac:	e2506000 	subs	r6, r0, #0	; 0x0
    7eb0:	0a000038 	beq	7f98 <burn+0x194>
    7eb4:	e1a00005 	mov	r0, r5
    7eb8:	e5984000 	ldr	r4, [r8]
    7ebc:	ebfff892 	bl	610c <vs_device_name>
    7ec0:	e1a02006 	mov	r2, r6
    7ec4:	e1a01000 	mov	r1, r0
    7ec8:	e59f0104 	ldr	r0, [pc, #260]	; 7fd4 <burn+0x1d0>
    7ecc:	ea00000a 	b	7efc <burn+0xf8>
    7ed0:	e1a00005 	mov	r0, r5
    7ed4:	e3a01001 	mov	r1, #1	; 0x1
    7ed8:	ebfff981 	bl	64e4 <vs_assign_by_id>
    7edc:	e2506000 	subs	r6, r0, #0	; 0x0
    7ee0:	0a000007 	beq	7f04 <burn+0x100>
    7ee4:	e1a00005 	mov	r0, r5
    7ee8:	e5984000 	ldr	r4, [r8]
    7eec:	ebfff886 	bl	610c <vs_device_name>
    7ef0:	e1a01000 	mov	r1, r0
    7ef4:	e59f00dc 	ldr	r0, [pc, #220]	; 7fd8 <burn+0x1d4>
    7ef8:	e1a02006 	mov	r2, r6
    7efc:	e12fff34 	blx	r4
    7f00:	ea00001d 	b	7f7c <burn+0x178>
    7f04:	e1a01007 	mov	r1, r7
    7f08:	e59f00cc 	ldr	r0, [pc, #204]	; 7fdc <burn+0x1d8>
    7f0c:	e3a02401 	mov	r2, #16777216	; 0x1000000
    7f10:	e3a03c02 	mov	r3, #512	; 0x200
    7f14:	e58d6000 	str	r6, [sp]
    7f18:	e1a0e00f 	mov	lr, pc
    7f1c:	e598f000 	ldr	pc, [r8]
    7f20:	e3a02401 	mov	r2, #16777216	; 0x1000000
    7f24:	e3a03000 	mov	r3, #0	; 0x0
    7f28:	e1a00007 	mov	r0, r7
    7f2c:	e58d6004 	str	r6, [sp, #4]
    7f30:	e58db000 	str	fp, [sp]
    7f34:	ebfff8c4 	bl	624c <vs_read>
    7f38:	e1a00007 	mov	r0, r7
    7f3c:	eb0029a4 	bl	125d4 <ius_check_header>
    7f40:	e2504000 	subs	r4, r0, #0	; 0x0
    7f44:	1a00000c 	bne	7f7c <burn+0x178>
    7f48:	e1a01005 	mov	r1, r5
    7f4c:	e59f3074 	ldr	r3, [pc, #116]	; 7fc8 <burn+0x1c4>
    7f50:	e59f0088 	ldr	r0, [pc, #136]	; 7fe0 <burn+0x1dc>
    7f54:	e1a0e00f 	mov	lr, pc
    7f58:	e593f000 	ldr	pc, [r3]
    7f5c:	e1a00004 	mov	r0, r4
    7f60:	e1a01004 	mov	r1, r4
    7f64:	ebfffd27 	bl	7408 <cdbg_verify_burn_enable>
    7f68:	e1a00007 	mov	r0, r7
    7f6c:	e1a01005 	mov	r1, r5
    7f70:	e1a02004 	mov	r2, r4
    7f74:	ebffff1a 	bl	7be4 <burn_images>
    7f78:	ea000006 	b	7f98 <burn+0x194>
    7f7c:	e2855001 	add	r5, r5, #1	; 0x1
    7f80:	ebfff882 	bl	6190 <vs_device_count>
    7f84:	e1550000 	cmp	r5, r0
    7f88:	baffffaf 	blt	7e4c <burn+0x48>
    7f8c:	e1a00007 	mov	r0, r7
    7f90:	ebffe63e 	bl	1890 <free>
    7f94:	ea000007 	b	7fb8 <burn+0x1b4>
    7f98:	e59f0044 	ldr	r0, [pc, #68]	; 7fe4 <burn+0x1e0>
    7f9c:	e59f3024 	ldr	r3, [pc, #36]	; 7fc8 <burn+0x1c4>
    7fa0:	e1a0e00f 	mov	lr, pc
    7fa4:	e593f000 	ldr	pc, [r3]
    7fa8:	e1a00007 	mov	r0, r7
    7fac:	ebffe637 	bl	1890 <free>
    7fb0:	ebfffd51 	bl	74fc <cdbg_shell>
    7fb4:	eafffffe 	b	7fb4 <burn+0x1b0>
    7fb8:	e3e00000 	mvn	r0, #0	; 0x0
    7fbc:	e28dd00c 	add	sp, sp, #12	; 0xc
    7fc0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    7fc4:	00017ea6 	.word	0x00017ea6
    7fc8:	3c00d6cc 	.word	0x3c00d6cc
    7fcc:	000179c2 	.word	0x000179c2
    7fd0:	000179be 	.word	0x000179be
    7fd4:	00017ec6 	.word	0x00017ec6
    7fd8:	00017ee8 	.word	0x00017ee8
    7fdc:	00017f07 	.word	0x00017f07
    7fe0:	00017f37 	.word	0x00017f37
    7fe4:	00017f4f 	.word	0x00017f4f

00007fe8 <mmc_send_cmd>:
    7fe8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    7fec:	e24dd004 	sub	sp, sp, #4	; 0x4
    7ff0:	e1a0e00f 	mov	lr, pc
    7ff4:	e590f104 	ldr	pc, [r0, #260]
    7ff8:	e28dd004 	add	sp, sp, #4	; 0x4
    7ffc:	e8bd8000 	pop	{pc}

00008000 <mmc_set_blocklen>:
    8000:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8004:	e3a03015 	mov	r3, #21	; 0x15
    8008:	e24dd024 	sub	sp, sp, #36	; 0x24
    800c:	e3a02000 	mov	r2, #0	; 0x0
    8010:	e58d3004 	str	r3, [sp, #4]
    8014:	e3a03010 	mov	r3, #16	; 0x10
    8018:	e58d1008 	str	r1, [sp, #8]
    801c:	e1cd30b0 	strh	r3, [sp]
    8020:	e58d201c 	str	r2, [sp, #28]
    8024:	e1a0100d 	mov	r1, sp
    8028:	e1a0e00f 	mov	lr, pc
    802c:	e590f104 	ldr	pc, [r0, #260]
    8030:	e28dd024 	add	sp, sp, #36	; 0x24
    8034:	e8bd8000 	pop	{pc}

00008038 <find_mmc_device>:
    8038:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    803c:	e59f3048 	ldr	r3, [pc, #72]	; 808c <find_mmc_device+0x54>
    8040:	e24dd004 	sub	sp, sp, #4	; 0x4
    8044:	e1a01000 	mov	r1, r0
    8048:	e1a0c003 	mov	ip, r3
    804c:	e5932000 	ldr	r2, [r3]
    8050:	ea000004 	b	8068 <find_mmc_device+0x30>
    8054:	e592309c 	ldr	r3, [r2, #156]
    8058:	e1a00002 	mov	r0, r2
    805c:	e1530001 	cmp	r3, r1
    8060:	0a000007 	beq	8084 <find_mmc_device+0x4c>
    8064:	e5922000 	ldr	r2, [r2]
    8068:	e152000c 	cmp	r2, ip
    806c:	1afffff8 	bne	8054 <find_mmc_device+0x1c>
    8070:	e59f0018 	ldr	r0, [pc, #24]	; 8090 <find_mmc_device+0x58>
    8074:	e59f3018 	ldr	r3, [pc, #24]	; 8094 <find_mmc_device+0x5c>
    8078:	e1a0e00f 	mov	lr, pc
    807c:	e593f000 	ldr	pc, [r3]
    8080:	e3a00000 	mov	r0, #0	; 0x0
    8084:	e28dd004 	add	sp, sp, #4	; 0x4
    8088:	e8bd8000 	pop	{pc}
    808c:	3c00d1c8 	.word	0x3c00d1c8
    8090:	00017f72 	.word	0x00017f72
    8094:	3c00d6cc 	.word	0x3c00d6cc

00008098 <mmc_send_ext_csd>:
    8098:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    809c:	e3a03015 	mov	r3, #21	; 0x15
    80a0:	e24dd034 	sub	sp, sp, #52	; 0x34
    80a4:	e58d3004 	str	r3, [sp, #4]
    80a8:	e3a03c02 	mov	r3, #512	; 0x200
    80ac:	e3a02000 	mov	r2, #0	; 0x0
    80b0:	e3a0c001 	mov	ip, #1	; 0x1
    80b4:	e58d302c 	str	r3, [sp, #44]
    80b8:	e3a03008 	mov	r3, #8	; 0x8
    80bc:	e58d1020 	str	r1, [sp, #32]
    80c0:	e58d201c 	str	r2, [sp, #28]
    80c4:	e58d2008 	str	r2, [sp, #8]
    80c8:	e58dc024 	str	ip, [sp, #36]
    80cc:	e1cd30b0 	strh	r3, [sp]
    80d0:	e58dc028 	str	ip, [sp, #40]
    80d4:	e1a0100d 	mov	r1, sp
    80d8:	e28d2020 	add	r2, sp, #32	; 0x20
    80dc:	e1a0e00f 	mov	lr, pc
    80e0:	e590f104 	ldr	pc, [r0, #260]
    80e4:	e28dd034 	add	sp, sp, #52	; 0x34
    80e8:	e8bd8000 	pop	{pc}

000080ec <sd_switch>:
    80ec:	e3e0c4ff 	mvn	ip, #-16777216	; 0xff000000
    80f0:	e1a02102 	lsl	r2, r2, #2
    80f4:	e18ccf81 	orr	ip, ip, r1, lsl #31
    80f8:	e3a0100f 	mov	r1, #15	; 0xf
    80fc:	e1ccc211 	bic	ip, ip, r1, lsl r2
    8100:	e18cc213 	orr	ip, ip, r3, lsl r2
    8104:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8108:	e3a03015 	mov	r3, #21	; 0x15
    810c:	e24dd034 	sub	sp, sp, #52	; 0x34
    8110:	e58d3004 	str	r3, [sp, #4]
    8114:	e3a03000 	mov	r3, #0	; 0x0
    8118:	e58d301c 	str	r3, [sp, #28]
    811c:	e59d3038 	ldr	r3, [sp, #56]
    8120:	e3a02001 	mov	r2, #1	; 0x1
    8124:	e58d3020 	str	r3, [sp, #32]
    8128:	e3a03040 	mov	r3, #64	; 0x40
    812c:	e58d302c 	str	r3, [sp, #44]
    8130:	e3a03006 	mov	r3, #6	; 0x6
    8134:	e58d2024 	str	r2, [sp, #36]
    8138:	e58d2028 	str	r2, [sp, #40]
    813c:	e58dc008 	str	ip, [sp, #8]
    8140:	e1cd30b0 	strh	r3, [sp]
    8144:	e1a0100d 	mov	r1, sp
    8148:	e28d2020 	add	r2, sp, #32	; 0x20
    814c:	e1a0e00f 	mov	lr, pc
    8150:	e590f104 	ldr	pc, [r0, #260]
    8154:	e28dd034 	add	sp, sp, #52	; 0x34
    8158:	e8bd8000 	pop	{pc}

0000815c <mmc_set_ios>:
    815c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8160:	e24dd004 	sub	sp, sp, #4	; 0x4
    8164:	e1a0e00f 	mov	lr, pc
    8168:	e590f108 	ldr	pc, [r0, #264]
    816c:	e28dd004 	add	sp, sp, #4	; 0x4
    8170:	e8bd8000 	pop	{pc}

00008174 <mmc_set_clock>:
    8174:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    8178:	e590203c 	ldr	r2, [r0, #60]
    817c:	e590c038 	ldr	ip, [r0, #56]
    8180:	e1510002 	cmp	r1, r2
    8184:	21a01002 	movcs	r1, r2
    8188:	e24dd004 	sub	sp, sp, #4	; 0x4
    818c:	e151000c 	cmp	r1, ip
    8190:	25801048 	strcs	r1, [r0, #72]
    8194:	3580c048 	strcc	ip, [r0, #72]
    8198:	e1a0e00f 	mov	lr, pc
    819c:	e590f108 	ldr	pc, [r0, #264]
    81a0:	e28dd004 	add	sp, sp, #4	; 0x4
    81a4:	e8bd8000 	pop	{pc}

000081a8 <mmc_set_bus_width>:
    81a8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    81ac:	e5801044 	str	r1, [r0, #68]
    81b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    81b4:	e1a0e00f 	mov	lr, pc
    81b8:	e590f108 	ldr	pc, [r0, #264]
    81bc:	e28dd004 	add	sp, sp, #4	; 0x4
    81c0:	e8bd8000 	pop	{pc}

000081c4 <mmc_send_if_cond>:
    81c4:	e92d4010 	push	{r4, lr}
    81c8:	e590202c 	ldr	r2, [r0, #44]
    81cc:	e59f3060 	ldr	r3, [pc, #96]	; 8234 <mmc_send_if_cond+0x70>
    81d0:	e24dd020 	sub	sp, sp, #32	; 0x20
    81d4:	e0023003 	and	r3, r2, r3
    81d8:	e3530000 	cmp	r3, #0	; 0x0
    81dc:	e28d1020 	add	r1, sp, #32	; 0x20
    81e0:	e3a03008 	mov	r3, #8	; 0x8
    81e4:	e300c1aa 	movw	ip, #426	; 0x1aa
    81e8:	e16132b0 	strh	r3, [r1, #-32]!
    81ec:	03a0c0aa 	moveq	ip, #170	; 0xaa
    81f0:	e3a02000 	mov	r2, #0	; 0x0
    81f4:	e3a03015 	mov	r3, #21	; 0x15
    81f8:	e98d1008 	stmib	sp, {r3, ip}
    81fc:	e58d201c 	str	r2, [sp, #28]
    8200:	e1a0100d 	mov	r1, sp
    8204:	e5903104 	ldr	r3, [r0, #260]
    8208:	e1a04000 	mov	r4, r0
    820c:	e12fff33 	blx	r3
    8210:	e3500000 	cmp	r0, #0	; 0x0
    8214:	1a000004 	bne	822c <mmc_send_if_cond+0x68>
    8218:	e5dd300c 	ldrb	r3, [sp, #12]
    821c:	e35300aa 	cmp	r3, #170	; 0xaa
    8220:	059f3010 	ldreq	r3, [pc, #16]	; 8238 <mmc_send_if_cond+0x74>
    8224:	13e00010 	mvnne	r0, #16	; 0x10
    8228:	05843030 	streq	r3, [r4, #48]
    822c:	e28dd020 	add	sp, sp, #32	; 0x20
    8230:	e8bd8010 	pop	{r4, pc}
    8234:	00ff8000 	.word	0x00ff8000
    8238:	00020020 	.word	0x00020020

0000823c <mmc_register>:
    823c:	e5903110 	ldr	r3, [r0, #272]
    8240:	e59fc05c 	ldr	ip, [pc, #92]	; 82a4 <mmc_register+0x68>
    8244:	e3530000 	cmp	r3, #0	; 0x0
    8248:	e3a03006 	mov	r3, #6	; 0x6
    824c:	e59c2000 	ldr	r2, [ip]
    8250:	e5803098 	str	r3, [r0, #152]
    8254:	e59f304c 	ldr	r3, [pc, #76]	; 82a8 <mmc_register+0x6c>
    8258:	e2821001 	add	r1, r2, #1	; 0x1
    825c:	e58030f8 	str	r3, [r0, #248]
    8260:	e59f3044 	ldr	r3, [pc, #68]	; 82ac <mmc_register+0x70>
    8264:	e580209c 	str	r2, [r0, #156]
    8268:	e58030fc 	str	r3, [r0, #252]
    826c:	e3a03001 	mov	r3, #1	; 0x1
    8270:	e58c1000 	str	r1, [ip]
    8274:	e5c030a4 	strb	r3, [r0, #164]
    8278:	03a03c02 	moveq	r3, #512	; 0x200
    827c:	05803110 	streq	r3, [r0, #272]
    8280:	e59f3028 	ldr	r3, [pc, #40]	; 82b0 <mmc_register+0x74>
    8284:	e5800004 	str	r0, [r0, #4]
    8288:	e5932004 	ldr	r2, [r3, #4]
    828c:	e5803000 	str	r3, [r0]
    8290:	e5830004 	str	r0, [r3, #4]
    8294:	e5820000 	str	r0, [r2]
    8298:	e5802004 	str	r2, [r0, #4]
    829c:	e3a00000 	mov	r0, #0	; 0x0
    82a0:	e12fff1e 	bx	lr
    82a4:	3c00d0a8 	.word	0x3c00d0a8
    82a8:	00009478 	.word	0x00009478
    82ac:	00008994 	.word	0x00008994
    82b0:	3c00d1c8 	.word	0x3c00d1c8

000082b4 <mmc_get_dev>:
    82b4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    82b8:	e24dd004 	sub	sp, sp, #4	; 0x4
    82bc:	ebffff5d 	bl	8038 <find_mmc_device>
    82c0:	e3500000 	cmp	r0, #0	; 0x0
    82c4:	12800098 	addne	r0, r0, #152	; 0x98
    82c8:	e28dd004 	add	sp, sp, #4	; 0x4
    82cc:	e8bd8000 	pop	{pc}

000082d0 <print_mmc_devices>:
    82d0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    82d4:	e59f3068 	ldr	r3, [pc, #104]	; 8344 <print_mmc_devices+0x74>
    82d8:	e24dd004 	sub	sp, sp, #4	; 0x4
    82dc:	e1a05000 	mov	r5, r0
    82e0:	e1a07003 	mov	r7, r3
    82e4:	e5934000 	ldr	r4, [r3]
    82e8:	e59f6058 	ldr	r6, [pc, #88]	; 8348 <print_mmc_devices+0x78>
    82ec:	ea00000a 	b	831c <print_mmc_devices+0x4c>
    82f0:	e594209c 	ldr	r2, [r4, #156]
    82f4:	e1a0e00f 	mov	lr, pc
    82f8:	e596f000 	ldr	pc, [r6]
    82fc:	e5943000 	ldr	r3, [r4]
    8300:	e59f0044 	ldr	r0, [pc, #68]	; 834c <print_mmc_devices+0x7c>
    8304:	e1530007 	cmp	r3, r7
    8308:	e1a01005 	mov	r1, r5
    830c:	0a000001 	beq	8318 <print_mmc_devices+0x48>
    8310:	e1a0e00f 	mov	lr, pc
    8314:	e596f000 	ldr	pc, [r6]
    8318:	e5944000 	ldr	r4, [r4]
    831c:	e1540007 	cmp	r4, r7
    8320:	e2841008 	add	r1, r4, #8	; 0x8
    8324:	e59f0024 	ldr	r0, [pc, #36]	; 8350 <print_mmc_devices+0x80>
    8328:	1afffff0 	bne	82f0 <print_mmc_devices+0x20>
    832c:	e59f0020 	ldr	r0, [pc, #32]	; 8354 <print_mmc_devices+0x84>
    8330:	e59f3010 	ldr	r3, [pc, #16]	; 8348 <print_mmc_devices+0x78>
    8334:	e1a0e00f 	mov	lr, pc
    8338:	e593f000 	ldr	pc, [r3]
    833c:	e28dd004 	add	sp, sp, #4	; 0x4
    8340:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8344:	3c00d1c8 	.word	0x3c00d1c8
    8348:	3c00d6cc 	.word	0x3c00d6cc
    834c:	00017f92 	.word	0x00017f92
    8350:	00017f8b 	.word	0x00017f8b
    8354:	00016bd7 	.word	0x00016bd7

00008358 <get_mmc_num>:
    8358:	e59f3004 	ldr	r3, [pc, #4]	; 8364 <get_mmc_num+0xc>
    835c:	e5930000 	ldr	r0, [r3]
    8360:	e12fff1e 	bx	lr
    8364:	3c00d0a8 	.word	0x3c00d0a8

00008368 <mmc_vs_align>:
    8368:	e3a00c02 	mov	r0, #512	; 0x200
    836c:	e12fff1e 	bx	lr

00008370 <init_mmc>:
    8370:	e59f3060 	ldr	r3, [pc, #96]	; 83d8 <init_mmc+0x68>
    8374:	e59f2060 	ldr	r2, [pc, #96]	; 83dc <init_mmc+0x6c>
    8378:	e92d4070 	push	{r4, r5, r6, lr}
    837c:	e3a04000 	mov	r4, #0	; 0x0
    8380:	e3a00b02 	mov	r0, #2048	; 0x800
    8384:	e5833004 	str	r3, [r3, #4]
    8388:	e5824000 	str	r4, [r2]
    838c:	e5833000 	str	r3, [r3]
    8390:	ebffe4d6 	bl	16f0 <malloc>
    8394:	e1a05000 	mov	r5, r0
    8398:	e2806b01 	add	r6, r0, #1024	; 0x400
    839c:	e1a00005 	mov	r0, r5
    83a0:	e3a01000 	mov	r1, #0	; 0x0
    83a4:	e3a02f4e 	mov	r2, #312	; 0x138
    83a8:	ebffe463 	bl	153c <memset>
    83ac:	e5856130 	str	r6, [r5, #304]
    83b0:	e1a00004 	mov	r0, r4
    83b4:	e1a01005 	mov	r1, r5
    83b8:	eb000490 	bl	9600 <imap_sdhc_initialize>
    83bc:	e3500000 	cmp	r0, #0	; 0x0
    83c0:	e2844001 	add	r4, r4, #1	; 0x1
    83c4:	e2855f4e 	add	r5, r5, #312	; 0x138
    83c8:	18bd8070 	popne	{r4, r5, r6, pc}
    83cc:	e3540003 	cmp	r4, #3	; 0x3
    83d0:	1afffff1 	bne	839c <init_mmc+0x2c>
    83d4:	e8bd8070 	pop	{r4, r5, r6, pc}
    83d8:	3c00d1c8 	.word	0x3c00d1c8
    83dc:	3c00d0a8 	.word	0x3c00d0a8

000083e0 <sd_change_freq>:
    83e0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    83e4:	e5903050 	ldr	r3, [r0, #80]
    83e8:	e3a01000 	mov	r1, #0	; 0x0
    83ec:	e2132b01 	ands	r2, r3, #1024	; 0x400
    83f0:	e1a04000 	mov	r4, r0
    83f4:	e580104c 	str	r1, [r0, #76]
    83f8:	e24dd084 	sub	sp, sp, #132	; 0x84
    83fc:	11a00001 	movne	r0, r1
    8400:	1a000070 	bne	85c8 <sd_change_freq+0x1e8>
    8404:	e1d438b0 	ldrh	r3, [r4, #128]
    8408:	e3a05015 	mov	r5, #21	; 0x15
    840c:	e1a03803 	lsl	r3, r3, #16
    8410:	e58d3050 	str	r3, [sp, #80]
    8414:	e28d6048 	add	r6, sp, #72	; 0x48
    8418:	e3a03037 	mov	r3, #55	; 0x37
    841c:	e1cd34b8 	strh	r3, [sp, #72]
    8420:	e58d504c 	str	r5, [sp, #76]
    8424:	e58d2064 	str	r2, [sp, #100]
    8428:	e1a01006 	mov	r1, r6
    842c:	e1a0e00f 	mov	lr, pc
    8430:	e594f104 	ldr	pc, [r4, #260]
    8434:	e3500000 	cmp	r0, #0	; 0x0
    8438:	1a000062 	bne	85c8 <sd_change_freq+0x1e8>
    843c:	e3a03033 	mov	r3, #51	; 0x33
    8440:	e58d504c 	str	r5, [sp, #76]
    8444:	e1a09006 	mov	r9, r6
    8448:	e58d0064 	str	r0, [sp, #100]
    844c:	e1cd34b8 	strh	r3, [sp, #72]
    8450:	e58d0050 	str	r0, [sp, #80]
    8454:	e3a05003 	mov	r5, #3	; 0x3
    8458:	e28da078 	add	sl, sp, #120	; 0x78
    845c:	e3a08008 	mov	r8, #8	; 0x8
    8460:	e3a06001 	mov	r6, #1	; 0x1
    8464:	e28d7068 	add	r7, sp, #104	; 0x68
    8468:	e58da068 	str	sl, [sp, #104]
    846c:	e58d8074 	str	r8, [sp, #116]
    8470:	e58d6070 	str	r6, [sp, #112]
    8474:	e58d606c 	str	r6, [sp, #108]
    8478:	e1a00004 	mov	r0, r4
    847c:	e1a01009 	mov	r1, r9
    8480:	e1a02007 	mov	r2, r7
    8484:	e1a0e00f 	mov	lr, pc
    8488:	e594f104 	ldr	pc, [r4, #260]
    848c:	e250b000 	subs	fp, r0, #0	; 0x0
    8490:	0a000003 	beq	84a4 <sd_change_freq+0xc4>
    8494:	e3550000 	cmp	r5, #0	; 0x0
    8498:	e2455001 	sub	r5, r5, #1	; 0x1
    849c:	1afffff1 	bne	8468 <sd_change_freq+0x88>
    84a0:	ea000047 	b	85c4 <sd_change_freq+0x1e4>
    84a4:	e59d3078 	ldr	r3, [sp, #120]
    84a8:	e59d007c 	ldr	r0, [sp, #124]
    84ac:	e1a01c03 	lsl	r1, r3, #24
    84b0:	e2032cff 	and	r2, r3, #65280	; 0xff00
    84b4:	e1811c23 	orr	r1, r1, r3, lsr #24
    84b8:	e1811402 	orr	r1, r1, r2, lsl #8
    84bc:	e20338ff 	and	r3, r3, #16711680	; 0xff0000
    84c0:	e1811423 	orr	r1, r1, r3, lsr #8
    84c4:	e1a03c00 	lsl	r3, r0, #24
    84c8:	e1833c20 	orr	r3, r3, r0, lsr #24
    84cc:	e2002cff 	and	r2, r0, #65280	; 0xff00
    84d0:	e1a0cc21 	lsr	ip, r1, #24
    84d4:	e1833402 	orr	r3, r3, r2, lsl #8
    84d8:	e20008ff 	and	r0, r0, #16711680	; 0xff0000
    84dc:	e20cc00f 	and	ip, ip, #15	; 0xf
    84e0:	e1833420 	orr	r3, r3, r0, lsr #8
    84e4:	e35c0001 	cmp	ip, #1	; 0x1
    84e8:	e584305c 	str	r3, [r4, #92]
    84ec:	e5841058 	str	r1, [r4, #88]
    84f0:	059f30d8 	ldreq	r3, [pc, #216]	; 85d0 <sd_change_freq+0x1f0>
    84f4:	0a000004 	beq	850c <sd_change_freq+0x12c>
    84f8:	3a000002 	bcc	8508 <sd_change_freq+0x128>
    84fc:	e35c0002 	cmp	ip, #2	; 0x2
    8500:	059f30cc 	ldreq	r3, [pc, #204]	; 85d4 <sd_change_freq+0x1f4>
    8504:	0a000000 	beq	850c <sd_change_freq+0x12c>
    8508:	e59f30c8 	ldr	r3, [pc, #200]	; 85d8 <sd_change_freq+0x1f8>
    850c:	e5843030 	str	r3, [r4, #48]
    8510:	e5943058 	ldr	r3, [r4, #88]
    8514:	e5942030 	ldr	r2, [r4, #48]
    8518:	e3130701 	tst	r3, #262144	; 0x40000
    851c:	1594304c 	ldrne	r3, [r4, #76]
    8520:	13833c01 	orrne	r3, r3, #256	; 0x100
    8524:	1584304c 	strne	r3, [r4, #76]
    8528:	e59f30a8 	ldr	r3, [pc, #168]	; 85d8 <sd_change_freq+0x1f8>
    852c:	e1520003 	cmp	r2, r3
    8530:	13a05004 	movne	r5, #4	; 0x4
    8534:	128d6008 	addne	r6, sp, #8	; 0x8
    8538:	1a000007 	bne	855c <sd_change_freq+0x17c>
    853c:	ea000020 	b	85c4 <sd_change_freq+0x1e4>
    8540:	e58d6000 	str	r6, [sp]
    8544:	ebfffee8 	bl	80ec <sd_switch>
    8548:	e3500000 	cmp	r0, #0	; 0x0
    854c:	1a00001d 	bne	85c8 <sd_change_freq+0x1e8>
    8550:	e59d3024 	ldr	r3, [sp, #36]
    8554:	e3130c02 	tst	r3, #512	; 0x200
    8558:	0a000005 	beq	8574 <sd_change_freq+0x194>
    855c:	e3a01000 	mov	r1, #0	; 0x0
    8560:	e2555001 	subs	r5, r5, #1	; 0x1
    8564:	e1a00004 	mov	r0, r4
    8568:	e1a02001 	mov	r2, r1
    856c:	e3a03001 	mov	r3, #1	; 0x1
    8570:	2afffff2 	bcs	8540 <sd_change_freq+0x160>
    8574:	e59d3014 	ldr	r3, [sp, #20]
    8578:	e3130c02 	tst	r3, #512	; 0x200
    857c:	0a000010 	beq	85c4 <sd_change_freq+0x1e4>
    8580:	e3a01001 	mov	r1, #1	; 0x1
    8584:	e28dc008 	add	ip, sp, #8	; 0x8
    8588:	e1a00004 	mov	r0, r4
    858c:	e3a02000 	mov	r2, #0	; 0x0
    8590:	e1a03001 	mov	r3, r1
    8594:	e58dc000 	str	ip, [sp]
    8598:	ebfffed3 	bl	80ec <sd_switch>
    859c:	e3500000 	cmp	r0, #0	; 0x0
    85a0:	1a000008 	bne	85c8 <sd_change_freq+0x1e8>
    85a4:	e59d3018 	ldr	r3, [sp, #24]
    85a8:	e1a03c03 	lsl	r3, r3, #24
    85ac:	e203340f 	and	r3, r3, #251658240	; 0xf000000
    85b0:	e3530401 	cmp	r3, #16777216	; 0x1000000
    85b4:	0594304c 	ldreq	r3, [r4, #76]
    85b8:	03833001 	orreq	r3, r3, #1	; 0x1
    85bc:	0584304c 	streq	r3, [r4, #76]
    85c0:	ea000000 	b	85c8 <sd_change_freq+0x1e8>
    85c4:	e1a0000b 	mov	r0, fp
    85c8:	e28dd084 	add	sp, sp, #132	; 0x84
    85cc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    85d0:	0002001a 	.word	0x0002001a
    85d4:	00020020 	.word	0x00020020
    85d8:	00020010 	.word	0x00020010

000085dc <sd_send_op_cond>:
    85dc:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    85e0:	e59f9140 	ldr	r9, [pc, #320]	; 8728 <sd_send_op_cond+0x14c>
    85e4:	e24dd020 	sub	sp, sp, #32	; 0x20
    85e8:	e1a04000 	mov	r4, r0
    85ec:	e3a05ffa 	mov	r5, #1000	; 0x3e8
    85f0:	e3a0a015 	mov	sl, #21	; 0x15
    85f4:	e3a06000 	mov	r6, #0	; 0x0
    85f8:	e1a0700d 	mov	r7, sp
    85fc:	e3a08001 	mov	r8, #1	; 0x1
    8600:	e3a03037 	mov	r3, #55	; 0x37
    8604:	e1cd30b0 	strh	r3, [sp]
    8608:	e1a0100d 	mov	r1, sp
    860c:	e1a02006 	mov	r2, r6
    8610:	e58da004 	str	sl, [sp, #4]
    8614:	e58d6008 	str	r6, [sp, #8]
    8618:	e58d601c 	str	r6, [sp, #28]
    861c:	e1a00004 	mov	r0, r4
    8620:	e1a0e00f 	mov	lr, pc
    8624:	e594f104 	ldr	pc, [r4, #260]
    8628:	e2503000 	subs	r3, r0, #0	; 0x0
    862c:	e1a0100d 	mov	r1, sp
    8630:	e3a02000 	mov	r2, #0	; 0x0
    8634:	e59fe0f0 	ldr	lr, [pc, #240]	; 872c <sd_send_op_cond+0x150>
    8638:	e1a00004 	mov	r0, r4
    863c:	1a000036 	bne	871c <sd_send_op_cond+0x140>
    8640:	e594302c 	ldr	r3, [r4, #44]
    8644:	e594c030 	ldr	ip, [r4, #48]
    8648:	e003e00e 	and	lr, r3, lr
    864c:	e15c0009 	cmp	ip, r9
    8650:	038e3101 	orreq	r3, lr, #1073741824	; 0x40000000
    8654:	e58de008 	str	lr, [sp, #8]
    8658:	058d3008 	streq	r3, [sp, #8]
    865c:	e3a03029 	mov	r3, #41	; 0x29
    8660:	e1cd30b0 	strh	r3, [sp]
    8664:	e58d8004 	str	r8, [sp, #4]
    8668:	e5943104 	ldr	r3, [r4, #260]
    866c:	e12fff33 	blx	r3
    8670:	e2503000 	subs	r3, r0, #0	; 0x0
    8674:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    8678:	1a000027 	bne	871c <sd_send_op_cond+0x140>
    867c:	eb0014d8 	bl	d9e4 <udelay>
    8680:	e59d300c 	ldr	r3, [sp, #12]
    8684:	e3530000 	cmp	r3, #0	; 0x0
    8688:	ba000002 	blt	8698 <sd_send_op_cond+0xbc>
    868c:	e2555001 	subs	r5, r5, #1	; 0x1
    8690:	2affffda 	bcs	8600 <sd_send_op_cond+0x24>
    8694:	ea00001f 	b	8718 <sd_send_op_cond+0x13c>
    8698:	e3550000 	cmp	r5, #0	; 0x0
    869c:	da00001d 	ble	8718 <sd_send_op_cond+0x13c>
    86a0:	e59f3080 	ldr	r3, [pc, #128]	; 8728 <sd_send_op_cond+0x14c>
    86a4:	e5942030 	ldr	r2, [r4, #48]
    86a8:	e1520003 	cmp	r2, r3
    86ac:	12433010 	subne	r3, r3, #16	; 0x10
    86b0:	15843030 	strne	r3, [r4, #48]
    86b4:	e5943050 	ldr	r3, [r4, #80]
    86b8:	e3130b01 	tst	r3, #1024	; 0x400
    86bc:	0a00000c 	beq	86f4 <sd_send_op_cond+0x118>
    86c0:	e3a03001 	mov	r3, #1	; 0x1
    86c4:	e3a02000 	mov	r2, #0	; 0x0
    86c8:	e58d3004 	str	r3, [sp, #4]
    86cc:	e3a0303a 	mov	r3, #58	; 0x3a
    86d0:	e1cd30b0 	strh	r3, [sp]
    86d4:	e58d2008 	str	r2, [sp, #8]
    86d8:	e58d201c 	str	r2, [sp, #28]
    86dc:	e1a00004 	mov	r0, r4
    86e0:	e1a0100d 	mov	r1, sp
    86e4:	e1a0e00f 	mov	lr, pc
    86e8:	e594f104 	ldr	pc, [r4, #260]
    86ec:	e2503000 	subs	r3, r0, #0	; 0x0
    86f0:	1a000009 	bne	871c <sd_send_op_cond+0x140>
    86f4:	e59d200c 	ldr	r2, [sp, #12]
    86f8:	e3a03000 	mov	r3, #0	; 0x0
    86fc:	e1c438b0 	strh	r3, [r4, #128]
    8700:	e1a03f22 	lsr	r3, r2, #30
    8704:	e2033001 	and	r3, r3, #1	; 0x1
    8708:	e5843040 	str	r3, [r4, #64]
    870c:	e5842054 	str	r2, [r4, #84]
    8710:	e3a03000 	mov	r3, #0	; 0x0
    8714:	ea000000 	b	871c <sd_send_op_cond+0x140>
    8718:	e3e03010 	mvn	r3, #16	; 0x10
    871c:	e1a00003 	mov	r0, r3
    8720:	e28dd020 	add	sp, sp, #32	; 0x20
    8724:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    8728:	00020020 	.word	0x00020020
    872c:	00ff8000 	.word	0x00ff8000

00008730 <mmc_go_idle>:
    8730:	e92d4010 	push	{r4, lr}
    8734:	e1a04000 	mov	r4, r0
    8738:	e24dd020 	sub	sp, sp, #32	; 0x20
    873c:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    8740:	eb0014a7 	bl	d9e4 <udelay>
    8744:	e3a02000 	mov	r2, #0	; 0x0
    8748:	e1a00004 	mov	r0, r4
    874c:	e1cd20b0 	strh	r2, [sp]
    8750:	e58d2008 	str	r2, [sp, #8]
    8754:	e58d2004 	str	r2, [sp, #4]
    8758:	e58d201c 	str	r2, [sp, #28]
    875c:	e1a0100d 	mov	r1, sp
    8760:	e1a0e00f 	mov	lr, pc
    8764:	e594f104 	ldr	pc, [r4, #260]
    8768:	e2504000 	subs	r4, r0, #0	; 0x0
    876c:	1a000001 	bne	8778 <mmc_go_idle+0x48>
    8770:	e2800e7d 	add	r0, r0, #2000	; 0x7d0
    8774:	eb00149a 	bl	d9e4 <udelay>
    8778:	e1a00004 	mov	r0, r4
    877c:	e28dd020 	add	sp, sp, #32	; 0x20
    8780:	e8bd8010 	pop	{r4, pc}

00008784 <mmc_send_op_cond>:
    8784:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    8788:	e24dd020 	sub	sp, sp, #32	; 0x20
    878c:	e1a05000 	mov	r5, r0
    8790:	e3a06001 	mov	r6, #1	; 0x1
    8794:	ebffffe5 	bl	8730 <mmc_go_idle>
    8798:	e3a02000 	mov	r2, #0	; 0x0
    879c:	e1cd60b0 	strh	r6, [sp]
    87a0:	e58d6004 	str	r6, [sp, #4]
    87a4:	e58d2008 	str	r2, [sp, #8]
    87a8:	e58d201c 	str	r2, [sp, #28]
    87ac:	e1a00005 	mov	r0, r5
    87b0:	e1a0100d 	mov	r1, sp
    87b4:	e1a0e00f 	mov	lr, pc
    87b8:	e595f104 	ldr	pc, [r5, #260]
    87bc:	e2504000 	subs	r4, r0, #0	; 0x0
    87c0:	e1a0700d 	mov	r7, sp
    87c4:	1a000039 	bne	88b0 <mmc_send_op_cond+0x12c>
    87c8:	e2800ffa 	add	r0, r0, #1000	; 0x3e8
    87cc:	eb001484 	bl	d9e4 <udelay>
    87d0:	e1a08006 	mov	r8, r6
    87d4:	e1a0700d 	mov	r7, sp
    87d8:	e1a06004 	mov	r6, r4
    87dc:	e3024710 	movw	r4, #10000	; 0x2710
    87e0:	e595302c 	ldr	r3, [r5, #44]
    87e4:	e3a02001 	mov	r2, #1	; 0x1
    87e8:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
    87ec:	e1cd20b0 	strh	r2, [sp]
    87f0:	e58d3008 	str	r3, [sp, #8]
    87f4:	e1a02006 	mov	r2, r6
    87f8:	e58d8004 	str	r8, [sp, #4]
    87fc:	e58d601c 	str	r6, [sp, #28]
    8800:	e1a0100d 	mov	r1, sp
    8804:	e1a00005 	mov	r0, r5
    8808:	e1a0e00f 	mov	lr, pc
    880c:	e595f104 	ldr	pc, [r5, #260]
    8810:	e2502000 	subs	r2, r0, #0	; 0x0
    8814:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    8818:	11a04002 	movne	r4, r2
    881c:	1a000023 	bne	88b0 <mmc_send_op_cond+0x12c>
    8820:	eb00146f 	bl	d9e4 <udelay>
    8824:	e59d300c 	ldr	r3, [sp, #12]
    8828:	e3530000 	cmp	r3, #0	; 0x0
    882c:	ba000002 	blt	883c <mmc_send_op_cond+0xb8>
    8830:	e2544001 	subs	r4, r4, #1	; 0x1
    8834:	2affffe9 	bcs	87e0 <mmc_send_op_cond+0x5c>
    8838:	ea00001b 	b	88ac <mmc_send_op_cond+0x128>
    883c:	e3540000 	cmp	r4, #0	; 0x0
    8840:	da000019 	ble	88ac <mmc_send_op_cond+0x128>
    8844:	e5953050 	ldr	r3, [r5, #80]
    8848:	e3130b01 	tst	r3, #1024	; 0x400
    884c:	0a00000b 	beq	8880 <mmc_send_op_cond+0xfc>
    8850:	e3a0303a 	mov	r3, #58	; 0x3a
    8854:	e58d8004 	str	r8, [sp, #4]
    8858:	e1cd30b0 	strh	r3, [sp]
    885c:	e58d6008 	str	r6, [sp, #8]
    8860:	e58d601c 	str	r6, [sp, #28]
    8864:	e1a0100d 	mov	r1, sp
    8868:	e1a02006 	mov	r2, r6
    886c:	e1a00005 	mov	r0, r5
    8870:	e1a0e00f 	mov	lr, pc
    8874:	e595f104 	ldr	pc, [r5, #260]
    8878:	e2504000 	subs	r4, r0, #0	; 0x0
    887c:	1a00000b 	bne	88b0 <mmc_send_op_cond+0x12c>
    8880:	e59d300c 	ldr	r3, [sp, #12]
    8884:	e3a01801 	mov	r1, #65536	; 0x10000
    8888:	e1a02f23 	lsr	r2, r3, #30
    888c:	e2022001 	and	r2, r2, #1	; 0x1
    8890:	e5852040 	str	r2, [r5, #64]
    8894:	e3a02000 	mov	r2, #0	; 0x0
    8898:	e5851030 	str	r1, [r5, #48]
    889c:	e1c528b0 	strh	r2, [r5, #128]
    88a0:	e5853054 	str	r3, [r5, #84]
    88a4:	e3a04000 	mov	r4, #0	; 0x0
    88a8:	ea000000 	b	88b0 <mmc_send_op_cond+0x12c>
    88ac:	e3e04010 	mvn	r4, #16	; 0x10
    88b0:	e1a00004 	mov	r0, r4
    88b4:	e28dd020 	add	sp, sp, #32	; 0x20
    88b8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

000088bc <mmc_send_status>:
    88bc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    88c0:	e1d038b0 	ldrh	r3, [r0, #128]
    88c4:	e24dd024 	sub	sp, sp, #36	; 0x24
    88c8:	e1a03803 	lsl	r3, r3, #16
    88cc:	e58d3008 	str	r3, [sp, #8]
    88d0:	e3a03000 	mov	r3, #0	; 0x0
    88d4:	e3a02015 	mov	r2, #21	; 0x15
    88d8:	e58d301c 	str	r3, [sp, #28]
    88dc:	e3a0300d 	mov	r3, #13	; 0xd
    88e0:	e1a06000 	mov	r6, r0
    88e4:	e58d2004 	str	r2, [sp, #4]
    88e8:	e1a04001 	mov	r4, r1
    88ec:	e1cd30b0 	strh	r3, [sp]
    88f0:	e1a0700d 	mov	r7, sp
    88f4:	e1a0100d 	mov	r1, sp
    88f8:	e3a02000 	mov	r2, #0	; 0x0
    88fc:	e1a00006 	mov	r0, r6
    8900:	e1a0e00f 	mov	lr, pc
    8904:	e596f104 	ldr	pc, [r6, #260]
    8908:	e2505000 	subs	r5, r0, #0	; 0x0
    890c:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    8910:	1a000018 	bne	8978 <mmc_send_status+0xbc>
    8914:	e59d300c 	ldr	r3, [sp, #12]
    8918:	e3130c01 	tst	r3, #256	; 0x100
    891c:	1a00000e 	bne	895c <mmc_send_status+0xa0>
    8920:	eb00142f 	bl	d9e4 <udelay>
    8924:	e59d100c 	ldr	r1, [sp, #12]
    8928:	e59f3054 	ldr	r3, [pc, #84]	; 8984 <mmc_send_status+0xc8>
    892c:	e2444001 	sub	r4, r4, #1	; 0x1
    8930:	e0013003 	and	r3, r1, r3
    8934:	e3530000 	cmp	r3, #0	; 0x0
    8938:	0a000005 	beq	8954 <mmc_send_status+0x98>
    893c:	e59f0044 	ldr	r0, [pc, #68]	; 8988 <mmc_send_status+0xcc>
    8940:	e59f3044 	ldr	r3, [pc, #68]	; 898c <mmc_send_status+0xd0>
    8944:	e1a0e00f 	mov	lr, pc
    8948:	e593f000 	ldr	pc, [r3]
    894c:	e3e05011 	mvn	r5, #17	; 0x11
    8950:	ea000008 	b	8978 <mmc_send_status+0xbc>
    8954:	e3740001 	cmn	r4, #1	; 0x1
    8958:	1affffe5 	bne	88f4 <mmc_send_status+0x38>
    895c:	e3540000 	cmp	r4, #0	; 0x0
    8960:	1a000004 	bne	8978 <mmc_send_status+0xbc>
    8964:	e59f0024 	ldr	r0, [pc, #36]	; 8990 <mmc_send_status+0xd4>
    8968:	e59f301c 	ldr	r3, [pc, #28]	; 898c <mmc_send_status+0xd0>
    896c:	e1a0e00f 	mov	lr, pc
    8970:	e593f000 	ldr	pc, [r3]
    8974:	e3e05012 	mvn	r5, #18	; 0x12
    8978:	e1a00005 	mov	r0, r5
    897c:	e28dd024 	add	sp, sp, #36	; 0x24
    8980:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    8984:	fdf94080 	.word	0xfdf94080
    8988:	00017f96 	.word	0x00017f96
    898c:	3c00d6cc 	.word	0x3c00d6cc
    8990:	00017fac 	.word	0x00017fac

00008994 <mmc_bwrite>:
    8994:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8998:	e24dd034 	sub	sp, sp, #52	; 0x34
    899c:	e1a06001 	mov	r6, r1
    89a0:	e1a0b002 	mov	fp, r2
    89a4:	e1a0a003 	mov	sl, r3
    89a8:	ebfffda2 	bl	8038 <find_mmc_device>
    89ac:	e2504000 	subs	r4, r0, #0	; 0x0
    89b0:	0a000060 	beq	8b38 <mmc_bwrite+0x1a4>
    89b4:	e594108c 	ldr	r1, [r4, #140]
    89b8:	ebfffd90 	bl	8000 <mmc_set_blocklen>
    89bc:	e3500000 	cmp	r0, #0	; 0x0
    89c0:	01a09000 	moveq	r9, r0
    89c4:	01a0700b 	moveq	r7, fp
    89c8:	0a000005 	beq	89e4 <mmc_bwrite+0x50>
    89cc:	e594108c 	ldr	r1, [r4, #140]
    89d0:	e59f016c 	ldr	r0, [pc, #364]	; 8b44 <mmc_bwrite+0x1b0>
    89d4:	e59f316c 	ldr	r3, [pc, #364]	; 8b48 <mmc_bwrite+0x1b4>
    89d8:	e1a0e00f 	mov	lr, pc
    89dc:	e593f000 	ldr	pc, [r3]
    89e0:	ea000054 	b	8b38 <mmc_bwrite+0x1a4>
    89e4:	e5943110 	ldr	r3, [r4, #272]
    89e8:	e59420a8 	ldr	r2, [r4, #168]
    89ec:	e1570003 	cmp	r7, r3
    89f0:	31a05007 	movcc	r5, r7
    89f4:	21a05003 	movcs	r5, r3
    89f8:	e0858006 	add	r8, r5, r6
    89fc:	e1580002 	cmp	r8, r2
    8a00:	9a000005 	bls	8a1c <mmc_bwrite+0x88>
    8a04:	e59f0140 	ldr	r0, [pc, #320]	; 8b4c <mmc_bwrite+0x1b8>
    8a08:	e1a01008 	mov	r1, r8
    8a0c:	e59f3134 	ldr	r3, [pc, #308]	; 8b48 <mmc_bwrite+0x1b4>
    8a10:	e1a0e00f 	mov	lr, pc
    8a14:	e593f000 	ldr	pc, [r3]
    8a18:	ea00003d 	b	8b14 <mmc_bwrite+0x180>
    8a1c:	e3550001 	cmp	r5, #1	; 0x1
    8a20:	93a03018 	movls	r3, #24	; 0x18
    8a24:	91cd30b0 	strhls	r3, [sp]
    8a28:	e5943040 	ldr	r3, [r4, #64]
    8a2c:	83a02019 	movhi	r2, #25	; 0x19
    8a30:	81cd20b0 	strhhi	r2, [sp]
    8a34:	e3530000 	cmp	r3, #0	; 0x0
    8a38:	0594308c 	ldreq	r3, [r4, #140]
    8a3c:	e3a02015 	mov	r2, #21	; 0x15
    8a40:	00030693 	muleq	r3, r3, r6
    8a44:	158d6008 	strne	r6, [sp, #8]
    8a48:	058d3008 	streq	r3, [sp, #8]
    8a4c:	e58d2004 	str	r2, [sp, #4]
    8a50:	e58d901c 	str	r9, [sp, #28]
    8a54:	e58da020 	str	sl, [sp, #32]
    8a58:	e58d5028 	str	r5, [sp, #40]
    8a5c:	e594308c 	ldr	r3, [r4, #140]
    8a60:	e3a02002 	mov	r2, #2	; 0x2
    8a64:	e58d2024 	str	r2, [sp, #36]
    8a68:	e58d302c 	str	r3, [sp, #44]
    8a6c:	e1a00004 	mov	r0, r4
    8a70:	e1a0100d 	mov	r1, sp
    8a74:	e28d2020 	add	r2, sp, #32	; 0x20
    8a78:	e1a0e00f 	mov	lr, pc
    8a7c:	e594f104 	ldr	pc, [r4, #260]
    8a80:	e3500000 	cmp	r0, #0	; 0x0
    8a84:	0a000004 	beq	8a9c <mmc_bwrite+0x108>
    8a88:	e59f00c0 	ldr	r0, [pc, #192]	; 8b50 <mmc_bwrite+0x1bc>
    8a8c:	e59f30b4 	ldr	r3, [pc, #180]	; 8b48 <mmc_bwrite+0x1b4>
    8a90:	e1a0e00f 	mov	lr, pc
    8a94:	e593f000 	ldr	pc, [r3]
    8a98:	ea00001d 	b	8b14 <mmc_bwrite+0x180>
    8a9c:	e5943050 	ldr	r3, [r4, #80]
    8aa0:	e1a03523 	lsr	r3, r3, #10
    8aa4:	e2233001 	eor	r3, r3, #1	; 0x1
    8aa8:	e3550001 	cmp	r5, #1	; 0x1
    8aac:	93a03000 	movls	r3, #0	; 0x0
    8ab0:	82033001 	andhi	r3, r3, #1	; 0x1
    8ab4:	e3530000 	cmp	r3, #0	; 0x0
    8ab8:	0a000011 	beq	8b04 <mmc_bwrite+0x170>
    8abc:	e3a0200c 	mov	r2, #12	; 0xc
    8ac0:	e3a0301d 	mov	r3, #29	; 0x1d
    8ac4:	e1cd20b0 	strh	r2, [sp]
    8ac8:	e98d0208 	stmib	sp, {r3, r9}
    8acc:	e58d901c 	str	r9, [sp, #28]
    8ad0:	e1a00004 	mov	r0, r4
    8ad4:	e1a0100d 	mov	r1, sp
    8ad8:	e1a02009 	mov	r2, r9
    8adc:	e1a0e00f 	mov	lr, pc
    8ae0:	e594f104 	ldr	pc, [r4, #260]
    8ae4:	e3500000 	cmp	r0, #0	; 0x0
    8ae8:	0a000005 	beq	8b04 <mmc_bwrite+0x170>
    8aec:	e59f0060 	ldr	r0, [pc, #96]	; 8b54 <mmc_bwrite+0x1c0>
    8af0:	e59f3050 	ldr	r3, [pc, #80]	; 8b48 <mmc_bwrite+0x1b4>
    8af4:	e1a0e00f 	mov	lr, pc
    8af8:	e593f000 	ldr	pc, [r3]
    8afc:	e1a00009 	mov	r0, r9
    8b00:	ea00000d 	b	8b3c <mmc_bwrite+0x1a8>
    8b04:	e1a00004 	mov	r0, r4
    8b08:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8b0c:	ebffff6a 	bl	88bc <mmc_send_status>
    8b10:	ea000001 	b	8b1c <mmc_bwrite+0x188>
    8b14:	e3550000 	cmp	r5, #0	; 0x0
    8b18:	1a000006 	bne	8b38 <mmc_bwrite+0x1a4>
    8b1c:	e0577005 	subs	r7, r7, r5
    8b20:	e594008c 	ldr	r0, [r4, #140]
    8b24:	102aa590 	mlane	sl, r0, r5, sl
    8b28:	11a06008 	movne	r6, r8
    8b2c:	1affffac 	bne	89e4 <mmc_bwrite+0x50>
    8b30:	e000009b 	mul	r0, fp, r0
    8b34:	ea000000 	b	8b3c <mmc_bwrite+0x1a8>
    8b38:	e3a00000 	mov	r0, #0	; 0x0
    8b3c:	e28dd034 	add	sp, sp, #52	; 0x34
    8b40:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    8b44:	00017fc8 	.word	0x00017fc8
    8b48:	3c00d6cc 	.word	0x3c00d6cc
    8b4c:	00017fd2 	.word	0x00017fd2
    8b50:	00017ffe 	.word	0x00017ffe
    8b54:	00018010 	.word	0x00018010

00008b58 <mmc2_vs_write>:
    8b58:	e1a024a2 	lsr	r2, r2, #9
    8b5c:	e1821b83 	orr	r1, r2, r3, lsl #23
    8b60:	e59d2000 	ldr	r2, [sp]
    8b64:	e1a03000 	mov	r3, r0
    8b68:	e1a024c2 	asr	r2, r2, #9
    8b6c:	e3a00002 	mov	r0, #2	; 0x2
    8b70:	eaffff87 	b	8994 <mmc_bwrite>

00008b74 <mmc1_vs_write>:
    8b74:	e1a024a2 	lsr	r2, r2, #9
    8b78:	e1821b83 	orr	r1, r2, r3, lsl #23
    8b7c:	e59d2000 	ldr	r2, [sp]
    8b80:	e1a03000 	mov	r3, r0
    8b84:	e1a024c2 	asr	r2, r2, #9
    8b88:	e3a00001 	mov	r0, #1	; 0x1
    8b8c:	eaffff80 	b	8994 <mmc_bwrite>

00008b90 <mmc0_vs_write>:
    8b90:	e1a024a2 	lsr	r2, r2, #9
    8b94:	e1821b83 	orr	r1, r2, r3, lsl #23
    8b98:	e59d2000 	ldr	r2, [sp]
    8b9c:	e1a03000 	mov	r3, r0
    8ba0:	e1a024c2 	asr	r2, r2, #9
    8ba4:	e3a00000 	mov	r0, #0	; 0x0
    8ba8:	eaffff79 	b	8994 <mmc_bwrite>

00008bac <mmc_switch>:
    8bac:	e92d4030 	push	{r4, r5, lr}
    8bb0:	e1a02802 	lsl	r2, r2, #16
    8bb4:	e3822403 	orr	r2, r2, #50331648	; 0x3000000
    8bb8:	e24dd024 	sub	sp, sp, #36	; 0x24
    8bbc:	e1822403 	orr	r2, r2, r3, lsl #8
    8bc0:	e3a0301d 	mov	r3, #29	; 0x1d
    8bc4:	e3a01000 	mov	r1, #0	; 0x0
    8bc8:	e58d3004 	str	r3, [sp, #4]
    8bcc:	e3a03006 	mov	r3, #6	; 0x6
    8bd0:	e58d2008 	str	r2, [sp, #8]
    8bd4:	e1cd30b0 	strh	r3, [sp]
    8bd8:	e1a02001 	mov	r2, r1
    8bdc:	e58d101c 	str	r1, [sp, #28]
    8be0:	e1a0100d 	mov	r1, sp
    8be4:	e1a04000 	mov	r4, r0
    8be8:	e1a0e00f 	mov	lr, pc
    8bec:	e590f104 	ldr	pc, [r0, #260]
    8bf0:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8bf4:	e1a05000 	mov	r5, r0
    8bf8:	e1a00004 	mov	r0, r4
    8bfc:	ebffff2e 	bl	88bc <mmc_send_status>
    8c00:	e1a00005 	mov	r0, r5
    8c04:	e28dd024 	add	sp, sp, #36	; 0x24
    8c08:	e8bd8030 	pop	{r4, r5, pc}

00008c0c <mmc_change_freq>:
    8c0c:	e92d4070 	push	{r4, r5, r6, lr}
    8c10:	e5903050 	ldr	r3, [r0, #80]
    8c14:	e24ddc02 	sub	sp, sp, #512	; 0x200
    8c18:	e3130b01 	tst	r3, #1024	; 0x400
    8c1c:	e3a03000 	mov	r3, #0	; 0x0
    8c20:	e1a04000 	mov	r4, r0
    8c24:	e580304c 	str	r3, [r0, #76]
    8c28:	1a000020 	bne	8cb0 <mmc_change_freq+0xa4>
    8c2c:	e5902030 	ldr	r2, [r0, #48]
    8c30:	e59f3084 	ldr	r3, [pc, #132]	; 8cbc <mmc_change_freq+0xb0>
    8c34:	e1520003 	cmp	r2, r3
    8c38:	9a00001c 	bls	8cb0 <mmc_change_freq+0xa4>
    8c3c:	e3a03c01 	mov	r3, #256	; 0x100
    8c40:	e580304c 	str	r3, [r0, #76]
    8c44:	e1a0100d 	mov	r1, sp
    8c48:	ebfffd12 	bl	8098 <mmc_send_ext_csd>
    8c4c:	e3500000 	cmp	r0, #0	; 0x0
    8c50:	e1a0600d 	mov	r6, sp
    8c54:	1a000016 	bne	8cb4 <mmc_change_freq+0xa8>
    8c58:	e3a01001 	mov	r1, #1	; 0x1
    8c5c:	e1a00004 	mov	r0, r4
    8c60:	e3a020b9 	mov	r2, #185	; 0xb9
    8c64:	e1a03001 	mov	r3, r1
    8c68:	e5dd50c4 	ldrb	r5, [sp, #196]
    8c6c:	ebffffce 	bl	8bac <mmc_switch>
    8c70:	e3500000 	cmp	r0, #0	; 0x0
    8c74:	1a00000e 	bne	8cb4 <mmc_change_freq+0xa8>
    8c78:	e1a0100d 	mov	r1, sp
    8c7c:	e1a00004 	mov	r0, r4
    8c80:	ebfffd04 	bl	8098 <mmc_send_ext_csd>
    8c84:	e3500000 	cmp	r0, #0	; 0x0
    8c88:	1a000009 	bne	8cb4 <mmc_change_freq+0xa8>
    8c8c:	e5dd30b9 	ldrb	r3, [sp, #185]
    8c90:	e3530000 	cmp	r3, #0	; 0x0
    8c94:	0a000006 	beq	8cb4 <mmc_change_freq+0xa8>
    8c98:	e594304c 	ldr	r3, [r4, #76]
    8c9c:	e3150002 	tst	r5, #2	; 0x2
    8ca0:	13833011 	orrne	r3, r3, #17	; 0x11
    8ca4:	03833001 	orreq	r3, r3, #1	; 0x1
    8ca8:	e584304c 	str	r3, [r4, #76]
    8cac:	ea000000 	b	8cb4 <mmc_change_freq+0xa8>
    8cb0:	e3a00000 	mov	r0, #0	; 0x0
    8cb4:	e28ddc02 	add	sp, sp, #512	; 0x200
    8cb8:	e8bd8070 	pop	{r4, r5, r6, pc}
    8cbc:	0001003f 	.word	0x0001003f

00008cc0 <mmc_startup>:
    8cc0:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    8cc4:	e5903050 	ldr	r3, [r0, #80]
    8cc8:	e24ddf8e 	sub	sp, sp, #568	; 0x238
    8ccc:	e3130b01 	tst	r3, #1024	; 0x400
    8cd0:	13a0300a 	movne	r3, #10	; 0xa
    8cd4:	03a03002 	moveq	r3, #2	; 0x2
    8cd8:	e28d1c02 	add	r1, sp, #512	; 0x200
    8cdc:	e3a02000 	mov	r2, #0	; 0x0
    8ce0:	e1c131b0 	strh	r3, [r1, #16]
    8ce4:	e28d5e21 	add	r5, sp, #528	; 0x210
    8ce8:	e3a03007 	mov	r3, #7	; 0x7
    8cec:	e58d3214 	str	r3, [sp, #532]
    8cf0:	e58d2218 	str	r2, [sp, #536]
    8cf4:	e58d222c 	str	r2, [sp, #556]
    8cf8:	e1a01005 	mov	r1, r5
    8cfc:	e1a06000 	mov	r6, r0
    8d00:	e1a0e00f 	mov	lr, pc
    8d04:	e590f104 	ldr	pc, [r0, #260]
    8d08:	e2504000 	subs	r4, r0, #0	; 0x0
    8d0c:	1a000159 	bne	9278 <mmc_startup+0x5b8>
    8d10:	e3a02010 	mov	r2, #16	; 0x10
    8d14:	e2860070 	add	r0, r6, #112	; 0x70
    8d18:	e285100c 	add	r1, r5, #12	; 0xc
    8d1c:	ebffe20e 	bl	155c <memcpy>
    8d20:	e5963050 	ldr	r3, [r6, #80]
    8d24:	e2132b01 	ands	r2, r3, #1024	; 0x400
    8d28:	1a000013 	bne	8d7c <mmc_startup+0xbc>
    8d2c:	e1d638b0 	ldrh	r3, [r6, #128]
    8d30:	e3a00003 	mov	r0, #3	; 0x3
    8d34:	e1a03803 	lsl	r3, r3, #16
    8d38:	e58d3218 	str	r3, [sp, #536]
    8d3c:	e3a03015 	mov	r3, #21	; 0x15
    8d40:	e58d3214 	str	r3, [sp, #532]
    8d44:	e28d3c02 	add	r3, sp, #512	; 0x200
    8d48:	e1c301b0 	strh	r0, [r3, #16]
    8d4c:	e58d222c 	str	r2, [sp, #556]
    8d50:	e1a01005 	mov	r1, r5
    8d54:	e1a00006 	mov	r0, r6
    8d58:	e1a0e00f 	mov	lr, pc
    8d5c:	e596f104 	ldr	pc, [r6, #260]
    8d60:	e2504000 	subs	r4, r0, #0	; 0x0
    8d64:	1a000143 	bne	9278 <mmc_startup+0x5b8>
    8d68:	e5963030 	ldr	r3, [r6, #48]
    8d6c:	e3130802 	tst	r3, #131072	; 0x20000
    8d70:	128d1c02 	addne	r1, sp, #512	; 0x200
    8d74:	11d111be 	ldrhne	r1, [r1, #30]
    8d78:	11c618b0 	strhne	r1, [r6, #128]
    8d7c:	e1d638b0 	ldrh	r3, [r6, #128]
    8d80:	e3a01000 	mov	r1, #0	; 0x0
    8d84:	e1a03803 	lsl	r3, r3, #16
    8d88:	e3a02007 	mov	r2, #7	; 0x7
    8d8c:	e58d3218 	str	r3, [sp, #536]
    8d90:	e3a00009 	mov	r0, #9	; 0x9
    8d94:	e28d3c02 	add	r3, sp, #512	; 0x200
    8d98:	e1c301b0 	strh	r0, [r3, #16]
    8d9c:	e58d2214 	str	r2, [sp, #532]
    8da0:	e58d122c 	str	r1, [sp, #556]
    8da4:	e1a02001 	mov	r2, r1
    8da8:	e1a00006 	mov	r0, r6
    8dac:	e28d1e21 	add	r1, sp, #528	; 0x210
    8db0:	e1a0e00f 	mov	lr, pc
    8db4:	e596f104 	ldr	pc, [r6, #260]
    8db8:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    8dbc:	e1a04000 	mov	r4, r0
    8dc0:	e1a00006 	mov	r0, r6
    8dc4:	ebfffebc 	bl	88bc <mmc_send_status>
    8dc8:	e3540000 	cmp	r4, #0	; 0x0
    8dcc:	1a000129 	bne	9278 <mmc_startup+0x5b8>
    8dd0:	e5963030 	ldr	r3, [r6, #48]
    8dd4:	e59d021c 	ldr	r0, [sp, #540]
    8dd8:	e59d2224 	ldr	r2, [sp, #548]
    8ddc:	e59d1228 	ldr	r1, [sp, #552]
    8de0:	e3530801 	cmp	r3, #65536	; 0x10000
    8de4:	e59d3220 	ldr	r3, [sp, #544]
    8de8:	e5862068 	str	r2, [r6, #104]
    8dec:	e5863064 	str	r3, [r6, #100]
    8df0:	e586106c 	str	r1, [r6, #108]
    8df4:	e5860060 	str	r0, [r6, #96]
    8df8:	1a000013 	bne	8e4c <mmc_startup+0x18c>
    8dfc:	e1a03d20 	lsr	r3, r0, #26
    8e00:	e203300f 	and	r3, r3, #15	; 0xf
    8e04:	e3530004 	cmp	r3, #4	; 0x4
    8e08:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
    8e0c:	ea00000c 	b	8e44 <mmc_startup+0x184>
    8e10:	00008e44 	.word	0x00008e44
    8e14:	00008e24 	.word	0x00008e24
    8e18:	00008e2c 	.word	0x00008e2c
    8e1c:	00008e34 	.word	0x00008e34
    8e20:	00008e3c 	.word	0x00008e3c
    8e24:	e59f3458 	ldr	r3, [pc, #1112]	; 9284 <mmc_startup+0x5c4>
    8e28:	ea000006 	b	8e48 <mmc_startup+0x188>
    8e2c:	e59f3454 	ldr	r3, [pc, #1108]	; 9288 <mmc_startup+0x5c8>
    8e30:	ea000004 	b	8e48 <mmc_startup+0x188>
    8e34:	e59f3450 	ldr	r3, [pc, #1104]	; 928c <mmc_startup+0x5cc>
    8e38:	ea000002 	b	8e48 <mmc_startup+0x188>
    8e3c:	e59f344c 	ldr	r3, [pc, #1100]	; 9290 <mmc_startup+0x5d0>
    8e40:	ea000000 	b	8e48 <mmc_startup+0x188>
    8e44:	e59f3448 	ldr	r3, [pc, #1096]	; 9294 <mmc_startup+0x5d4>
    8e48:	e5863030 	str	r3, [r6, #48]
    8e4c:	e28d1c02 	add	r1, sp, #512	; 0x200
    8e50:	e1d132b2 	ldrh	r3, [r1, #34]
    8e54:	e3a0e001 	mov	lr, #1	; 0x1
    8e58:	e203300f 	and	r3, r3, #15	; 0xf
    8e5c:	e1a0c31e 	lsl	ip, lr, r3
    8e60:	e59d121c 	ldr	r1, [sp, #540]
    8e64:	e59f242c 	ldr	r2, [pc, #1068]	; 9298 <mmc_startup+0x5d8>
    8e68:	e1a031a1 	lsr	r3, r1, #3
    8e6c:	e203300f 	and	r3, r3, #15	; 0xf
    8e70:	e7920103 	ldr	r0, [r2, r3, lsl #2]
    8e74:	e59f3420 	ldr	r3, [pc, #1056]	; 929c <mmc_startup+0x5dc>
    8e78:	e2011007 	and	r1, r1, #7	; 0x7
    8e7c:	e7933101 	ldr	r3, [r3, r1, lsl #2]
    8e80:	e5962030 	ldr	r2, [r6, #48]
    8e84:	e0030093 	mul	r3, r3, r0
    8e88:	e3120802 	tst	r2, #131072	; 0x20000
    8e8c:	e5863084 	str	r3, [r6, #132]
    8e90:	e586c088 	str	ip, [r6, #136]
    8e94:	059d3228 	ldreq	r3, [sp, #552]
    8e98:	1586c08c 	strne	ip, [r6, #140]
    8e9c:	01a03b23 	lsreq	r3, r3, #22
    8ea0:	0203300f 	andeq	r3, r3, #15	; 0xf
    8ea4:	01a0331e 	lsleq	r3, lr, r3
    8ea8:	0586308c 	streq	r3, [r6, #140]
    8eac:	e5963040 	ldr	r3, [r6, #64]
    8eb0:	e5962064 	ldr	r2, [r6, #100]
    8eb4:	e3530000 	cmp	r3, #0	; 0x0
    8eb8:	e5960068 	ldr	r0, [r6, #104]
    8ebc:	0a000007 	beq	8ee0 <mmc_startup+0x220>
    8ec0:	e202203f 	and	r2, r2, #63	; 0x3f
    8ec4:	e1a03820 	lsr	r3, r0, #16
    8ec8:	e1833802 	orr	r3, r3, r2, lsl #16
    8ecc:	e1a08003 	mov	r8, r3
    8ed0:	e3a09000 	mov	r9, #0	; 0x0
    8ed4:	e3a04008 	mov	r4, #8	; 0x8
    8ed8:	e3a05000 	mov	r5, #0	; 0x0
    8edc:	ea000009 	b	8f08 <mmc_startup+0x248>
    8ee0:	e1a03b02 	lsl	r3, r2, #22
    8ee4:	e1a03b23 	lsr	r3, r3, #22
    8ee8:	e200190e 	and	r1, r0, #229376	; 0x38000
    8eec:	e1a02f20 	lsr	r2, r0, #30
    8ef0:	e1822103 	orr	r2, r2, r3, lsl #2
    8ef4:	e1a017a1 	lsr	r1, r1, #15
    8ef8:	e1a08002 	mov	r8, r2
    8efc:	e3a09000 	mov	r9, #0	; 0x0
    8f00:	e1a04001 	mov	r4, r1
    8f04:	e3a05000 	mov	r5, #0	; 0x0
    8f08:	e3a02001 	mov	r2, #1	; 0x1
    8f0c:	e0980002 	adds	r0, r8, r2
    8f10:	e3a03000 	mov	r3, #0	; 0x0
    8f14:	e0a91003 	adc	r1, r9, r3
    8f18:	e2842002 	add	r2, r4, #2	; 0x2
    8f1c:	eb002f3a 	bl	14c0c <__aeabi_llsl>
    8f20:	e596c088 	ldr	ip, [r6, #136]
    8f24:	e0832c90 	umull	r2, r3, r0, ip
    8f28:	e023319c 	mla	r3, ip, r1, r3
    8f2c:	e35c0c02 	cmp	ip, #512	; 0x200
    8f30:	e1c629f0 	strd	r2, [r6, #144]
    8f34:	83a03c02 	movhi	r3, #512	; 0x200
    8f38:	85863088 	strhi	r3, [r6, #136]
    8f3c:	e596308c 	ldr	r3, [r6, #140]
    8f40:	e3530c02 	cmp	r3, #512	; 0x200
    8f44:	83a03c02 	movhi	r3, #512	; 0x200
    8f48:	8586308c 	strhi	r3, [r6, #140]
    8f4c:	e5963050 	ldr	r3, [r6, #80]
    8f50:	e2131b01 	ands	r1, r3, #1024	; 0x400
    8f54:	1a00000f 	bne	8f98 <mmc_startup+0x2d8>
    8f58:	e1d638b0 	ldrh	r3, [r6, #128]
    8f5c:	e3a0201d 	mov	r2, #29	; 0x1d
    8f60:	e1a03803 	lsl	r3, r3, #16
    8f64:	e58d2214 	str	r2, [sp, #532]
    8f68:	e58d3218 	str	r3, [sp, #536]
    8f6c:	e28d2c02 	add	r2, sp, #512	; 0x200
    8f70:	e3a03007 	mov	r3, #7	; 0x7
    8f74:	e1c231b0 	strh	r3, [r2, #16]
    8f78:	e58d122c 	str	r1, [sp, #556]
    8f7c:	e1a02001 	mov	r2, r1
    8f80:	e1a00006 	mov	r0, r6
    8f84:	e28d1e21 	add	r1, sp, #528	; 0x210
    8f88:	e1a0e00f 	mov	lr, pc
    8f8c:	e596f104 	ldr	pc, [r6, #260]
    8f90:	e2504000 	subs	r4, r0, #0	; 0x0
    8f94:	1a0000b7 	bne	9278 <mmc_startup+0x5b8>
    8f98:	e5962030 	ldr	r2, [r6, #48]
    8f9c:	e3e03000 	mvn	r3, #0	; 0x0
    8fa0:	e3120802 	tst	r2, #131072	; 0x20000
    8fa4:	e5c63082 	strb	r3, [r6, #130]
    8fa8:	1a00001e 	bne	9028 <mmc_startup+0x368>
    8fac:	e59f32ec 	ldr	r3, [pc, #748]	; 92a0 <mmc_startup+0x5e0>
    8fb0:	e1520003 	cmp	r2, r3
    8fb4:	9a00001b 	bls	9028 <mmc_startup+0x368>
    8fb8:	e28d1010 	add	r1, sp, #16	; 0x10
    8fbc:	e1a00006 	mov	r0, r6
    8fc0:	ebfffc34 	bl	8098 <mmc_send_ext_csd>
    8fc4:	e5dd30d0 	ldrb	r3, [sp, #208]
    8fc8:	e2701001 	rsbs	r1, r0, #1	; 0x1
    8fcc:	33a01000 	movcc	r1, #0	; 0x0
    8fd0:	e3530001 	cmp	r3, #1	; 0x1
    8fd4:	93a01000 	movls	r1, #0	; 0x0
    8fd8:	e3510000 	cmp	r1, #0	; 0x0
    8fdc:	0a00000d 	beq	9018 <mmc_startup+0x358>
    8fe0:	e5dd20e6 	ldrb	r2, [sp, #230]
    8fe4:	e5dd10e5 	ldrb	r1, [sp, #229]
    8fe8:	e5dd00e4 	ldrb	r0, [sp, #228]
    8fec:	e1a02802 	lsl	r2, r2, #16
    8ff0:	e1822401 	orr	r2, r2, r1, lsl #8
    8ff4:	e5dd10e7 	ldrb	r1, [sp, #231]
    8ff8:	e1822000 	orr	r2, r2, r0
    8ffc:	e1822c01 	orr	r2, r2, r1, lsl #24
    9000:	e1a03fc2 	asr	r3, r2, #31
    9004:	e1a01483 	lsl	r1, r3, #9
    9008:	e1811ba2 	orr	r1, r1, r2, lsr #23
    900c:	e1a02482 	lsl	r2, r2, #9
    9010:	e5861094 	str	r1, [r6, #148]
    9014:	e5862090 	str	r2, [r6, #144]
    9018:	e5dd30b0 	ldrb	r3, [sp, #176]
    901c:	e3130001 	tst	r3, #1	; 0x1
    9020:	15dd30c3 	ldrbne	r3, [sp, #195]
    9024:	15c63082 	strbne	r3, [r6, #130]
    9028:	e5963030 	ldr	r3, [r6, #48]
    902c:	e3130802 	tst	r3, #131072	; 0x20000
    9030:	0a000002 	beq	9040 <mmc_startup+0x380>
    9034:	e1a00006 	mov	r0, r6
    9038:	ebfffce8 	bl	83e0 <sd_change_freq>
    903c:	ea000001 	b	9048 <mmc_startup+0x388>
    9040:	e1a00006 	mov	r0, r6
    9044:	ebfffef0 	bl	8c0c <mmc_change_freq>
    9048:	e3500000 	cmp	r0, #0	; 0x0
    904c:	e1a05000 	mov	r5, r0
    9050:	11a04000 	movne	r4, r0
    9054:	1a000087 	bne	9278 <mmc_startup+0x5b8>
    9058:	e596304c 	ldr	r3, [r6, #76]
    905c:	e5962050 	ldr	r2, [r6, #80]
    9060:	e5961030 	ldr	r1, [r6, #48]
    9064:	e0022003 	and	r2, r2, r3
    9068:	e3110802 	tst	r1, #131072	; 0x20000
    906c:	e586204c 	str	r2, [r6, #76]
    9070:	e2023c01 	and	r3, r2, #256	; 0x100
    9074:	0a00002c 	beq	912c <mmc_startup+0x46c>
    9078:	e3530000 	cmp	r3, #0	; 0x0
    907c:	0a000023 	beq	9110 <mmc_startup+0x450>
    9080:	e1d638b0 	ldrh	r3, [r6, #128]
    9084:	e28d2c02 	add	r2, sp, #512	; 0x200
    9088:	e1a03803 	lsl	r3, r3, #16
    908c:	e58d3218 	str	r3, [sp, #536]
    9090:	e3a08015 	mov	r8, #21	; 0x15
    9094:	e3a03037 	mov	r3, #55	; 0x37
    9098:	e28d9e21 	add	r9, sp, #528	; 0x210
    909c:	e1c231b0 	strh	r3, [r2, #16]
    90a0:	e58d022c 	str	r0, [sp, #556]
    90a4:	e58d8214 	str	r8, [sp, #532]
    90a8:	e1a00006 	mov	r0, r6
    90ac:	e1a01009 	mov	r1, r9
    90b0:	e1a02005 	mov	r2, r5
    90b4:	e1a0e00f 	mov	lr, pc
    90b8:	e596f104 	ldr	pc, [r6, #260]
    90bc:	e2504000 	subs	r4, r0, #0	; 0x0
    90c0:	1a00006c 	bne	9278 <mmc_startup+0x5b8>
    90c4:	e3a03002 	mov	r3, #2	; 0x2
    90c8:	e3a01006 	mov	r1, #6	; 0x6
    90cc:	e28d0c02 	add	r0, sp, #512	; 0x200
    90d0:	e1c011b0 	strh	r1, [r0, #16]
    90d4:	e58d8214 	str	r8, [sp, #532]
    90d8:	e58d3218 	str	r3, [sp, #536]
    90dc:	e58d522c 	str	r5, [sp, #556]
    90e0:	e1a01009 	mov	r1, r9
    90e4:	e1a02005 	mov	r2, r5
    90e8:	e1a00006 	mov	r0, r6
    90ec:	e1a0e00f 	mov	lr, pc
    90f0:	e596f104 	ldr	pc, [r6, #260]
    90f4:	e2504000 	subs	r4, r0, #0	; 0x0
    90f8:	1a00005e 	bne	9278 <mmc_startup+0x5b8>
    90fc:	e3a03004 	mov	r3, #4	; 0x4
    9100:	e5863044 	str	r3, [r6, #68]
    9104:	e1a00006 	mov	r0, r6
    9108:	e1a0e00f 	mov	lr, pc
    910c:	e596f108 	ldr	pc, [r6, #264]
    9110:	e596304c 	ldr	r3, [r6, #76]
    9114:	e3130001 	tst	r3, #1	; 0x1
    9118:	11a00006 	movne	r0, r6
    911c:	159f1180 	ldrne	r1, [pc, #384]	; 92a4 <mmc_startup+0x5e4>
    9120:	01a00006 	moveq	r0, r6
    9124:	059f117c 	ldreq	r1, [pc, #380]	; 92a8 <mmc_startup+0x5e8>
    9128:	ea000022 	b	91b8 <mmc_startup+0x4f8>
    912c:	e3530000 	cmp	r3, #0	; 0x0
    9130:	0a000008 	beq	9158 <mmc_startup+0x498>
    9134:	e3a01001 	mov	r1, #1	; 0x1
    9138:	e1a03001 	mov	r3, r1
    913c:	e1a00006 	mov	r0, r6
    9140:	e3a020b7 	mov	r2, #183	; 0xb7
    9144:	ebfffe98 	bl	8bac <mmc_switch>
    9148:	e2504000 	subs	r4, r0, #0	; 0x0
    914c:	03a03004 	moveq	r3, #4	; 0x4
    9150:	1a000048 	bne	9278 <mmc_startup+0x5b8>
    9154:	ea000009 	b	9180 <mmc_startup+0x4c0>
    9158:	e3120c02 	tst	r2, #512	; 0x200
    915c:	0a00000b 	beq	9190 <mmc_startup+0x4d0>
    9160:	e1a00006 	mov	r0, r6
    9164:	e3a01001 	mov	r1, #1	; 0x1
    9168:	e3a020b7 	mov	r2, #183	; 0xb7
    916c:	e3a03002 	mov	r3, #2	; 0x2
    9170:	ebfffe8d 	bl	8bac <mmc_switch>
    9174:	e2504000 	subs	r4, r0, #0	; 0x0
    9178:	1a00003e 	bne	9278 <mmc_startup+0x5b8>
    917c:	e3a03008 	mov	r3, #8	; 0x8
    9180:	e5863044 	str	r3, [r6, #68]
    9184:	e1a00006 	mov	r0, r6
    9188:	e1a0e00f 	mov	lr, pc
    918c:	e596f108 	ldr	pc, [r6, #264]
    9190:	e596304c 	ldr	r3, [r6, #76]
    9194:	e3130001 	tst	r3, #1	; 0x1
    9198:	01a00006 	moveq	r0, r6
    919c:	059f1108 	ldreq	r1, [pc, #264]	; 92ac <mmc_startup+0x5ec>
    91a0:	0a000004 	beq	91b8 <mmc_startup+0x4f8>
    91a4:	e3130010 	tst	r3, #16	; 0x10
    91a8:	159f1100 	ldrne	r1, [pc, #256]	; 92b0 <mmc_startup+0x5f0>
    91ac:	059f1100 	ldreq	r1, [pc, #256]	; 92b4 <mmc_startup+0x5f4>
    91b0:	11a00006 	movne	r0, r6
    91b4:	01a00006 	moveq	r0, r6
    91b8:	ebfffbed 	bl	8174 <mmc_set_clock>
    91bc:	e5964094 	ldr	r4, [r6, #148]
    91c0:	e5960090 	ldr	r0, [r6, #144]
    91c4:	e5961088 	ldr	r1, [r6, #136]
    91c8:	e3a03000 	mov	r3, #0	; 0x0
    91cc:	e3540000 	cmp	r4, #0	; 0x0
    91d0:	e5c630a3 	strb	r3, [r6, #163]
    91d4:	e5c630a2 	strb	r3, [r6, #162]
    91d8:	e58610ac 	str	r1, [r6, #172]
    91dc:	e58d0230 	str	r0, [sp, #560]
    91e0:	e58d4234 	str	r4, [sp, #564]
    91e4:	1a000003 	bne	91f8 <mmc_startup+0x538>
    91e8:	eb002d69 	bl	14794 <__aeabi_uidiv>
    91ec:	e58d4234 	str	r4, [sp, #564]
    91f0:	e58d0230 	str	r0, [sp, #560]
    91f4:	ea000001 	b	9200 <mmc_startup+0x540>
    91f8:	e28d0e23 	add	r0, sp, #560	; 0x230
    91fc:	eb002123 	bl	11690 <__div64_32>
    9200:	e5d6007f 	ldrb	r0, [r6, #127]
    9204:	e59d1230 	ldr	r1, [sp, #560]
    9208:	e5962070 	ldr	r2, [r6, #112]
    920c:	e5963078 	ldr	r3, [r6, #120]
    9210:	e58610a8 	str	r1, [r6, #168]
    9214:	e1803403 	orr	r3, r0, r3, lsl #8
    9218:	e1a02422 	lsr	r2, r2, #8
    921c:	e59f1094 	ldr	r1, [pc, #148]	; 92b8 <mmc_startup+0x5f8>
    9220:	e28600b0 	add	r0, r6, #176	; 0xb0
    9224:	eb0024e2 	bl	125b4 <sprintf>
    9228:	e596c074 	ldr	ip, [r6, #116]
    922c:	e5d62070 	ldrb	r2, [r6, #112]
    9230:	e1a0482c 	lsr	r4, ip, #16
    9234:	e1a0542c 	lsr	r5, ip, #8
    9238:	e1a03c2c 	lsr	r3, ip, #24
    923c:	e20440ff 	and	r4, r4, #255	; 0xff
    9240:	e20cc0ff 	and	ip, ip, #255	; 0xff
    9244:	e59f1070 	ldr	r1, [pc, #112]	; 92bc <mmc_startup+0x5fc>
    9248:	e20550ff 	and	r5, r5, #255	; 0xff
    924c:	e28600d9 	add	r0, r6, #217	; 0xd9
    9250:	e88d1030 	stm	sp, {r4, r5, ip}
    9254:	eb0024d6 	bl	125b4 <sprintf>
    9258:	e5962078 	ldr	r2, [r6, #120]
    925c:	e28600ee 	add	r0, r6, #238	; 0xee
    9260:	e1a03c22 	lsr	r3, r2, #24
    9264:	e203300f 	and	r3, r3, #15	; 0xf
    9268:	e1a02e22 	lsr	r2, r2, #28
    926c:	e59f104c 	ldr	r1, [pc, #76]	; 92c0 <mmc_startup+0x600>
    9270:	eb0024cf 	bl	125b4 <sprintf>
    9274:	e3a04000 	mov	r4, #0	; 0x0
    9278:	e1a00004 	mov	r0, r4
    927c:	e28ddf8e 	add	sp, sp, #568	; 0x238
    9280:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    9284:	00010014 	.word	0x00010014
    9288:	00010022 	.word	0x00010022
    928c:	00010030 	.word	0x00010030
    9290:	00010040 	.word	0x00010040
    9294:	00010012 	.word	0x00010012
    9298:	00015cdc 	.word	0x00015cdc
    929c:	00015ccc 	.word	0x00015ccc
    92a0:	0001003f 	.word	0x0001003f
    92a4:	02faf080 	.word	0x02faf080
    92a8:	017d7840 	.word	0x017d7840
    92ac:	01312d00 	.word	0x01312d00
    92b0:	03197500 	.word	0x03197500
    92b4:	018cba80 	.word	0x018cba80
    92b8:	0001802b 	.word	0x0001802b
    92bc:	0001803d 	.word	0x0001803d
    92c0:	00018048 	.word	0x00018048

000092c4 <mmc_init>:
    92c4:	e92d4010 	push	{r4, lr}
    92c8:	ebfffb5a 	bl	8038 <find_mmc_device>
    92cc:	e1a04000 	mov	r4, r0
    92d0:	e1a0e00f 	mov	lr, pc
    92d4:	e590f10c 	ldr	pc, [r0, #268]
    92d8:	e3500000 	cmp	r0, #0	; 0x0
    92dc:	18bd8010 	popne	{r4, pc}
    92e0:	e3a03001 	mov	r3, #1	; 0x1
    92e4:	e5843044 	str	r3, [r4, #68]
    92e8:	e1a00004 	mov	r0, r4
    92ec:	e1a0e00f 	mov	lr, pc
    92f0:	e594f108 	ldr	pc, [r4, #264]
    92f4:	e1a00004 	mov	r0, r4
    92f8:	e59f1060 	ldr	r1, [pc, #96]	; 9360 <mmc_init+0x9c>
    92fc:	ebfffb9c 	bl	8174 <mmc_set_clock>
    9300:	e1a00004 	mov	r0, r4
    9304:	ebfffd09 	bl	8730 <mmc_go_idle>
    9308:	e3500000 	cmp	r0, #0	; 0x0
    930c:	18bd8010 	popne	{r4, pc}
    9310:	e5c40083 	strb	r0, [r4, #131]
    9314:	e1a00004 	mov	r0, r4
    9318:	ebfffba9 	bl	81c4 <mmc_send_if_cond>
    931c:	e1a00004 	mov	r0, r4
    9320:	ebfffcad 	bl	85dc <sd_send_op_cond>
    9324:	e3700013 	cmn	r0, #19	; 0x13
    9328:	1a000009 	bne	9354 <mmc_init+0x90>
    932c:	e1a00004 	mov	r0, r4
    9330:	ebfffd13 	bl	8784 <mmc_send_op_cond>
    9334:	e3500000 	cmp	r0, #0	; 0x0
    9338:	0a000005 	beq	9354 <mmc_init+0x90>
    933c:	e59f0020 	ldr	r0, [pc, #32]	; 9364 <mmc_init+0xa0>
    9340:	e59f3020 	ldr	r3, [pc, #32]	; 9368 <mmc_init+0xa4>
    9344:	e1a0e00f 	mov	lr, pc
    9348:	e593f000 	ldr	pc, [r3]
    934c:	e3e00010 	mvn	r0, #16	; 0x10
    9350:	e8bd8010 	pop	{r4, pc}
    9354:	e1a00004 	mov	r0, r4
    9358:	e8bd4010 	pop	{r4, lr}
    935c:	eafffe57 	b	8cc0 <mmc_startup>
    9360:	00061a80 	.word	0x00061a80
    9364:	0001804e 	.word	0x0001804e
    9368:	3c00d6cc 	.word	0x3c00d6cc

0000936c <mmc2_vs_reset>:
    936c:	e3a00002 	mov	r0, #2	; 0x2
    9370:	eaffffd3 	b	92c4 <mmc_init>

00009374 <mmc1_vs_reset>:
    9374:	e3a00001 	mov	r0, #1	; 0x1
    9378:	eaffffd1 	b	92c4 <mmc_init>

0000937c <mmc0_vs_reset>:
    937c:	e3a00000 	mov	r0, #0	; 0x0
    9380:	eaffffcf 	b	92c4 <mmc_init>

00009384 <mmc_read_blocks>:
    9384:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    9388:	e3530001 	cmp	r3, #1	; 0x1
    938c:	e24dd030 	sub	sp, sp, #48	; 0x30
    9390:	e1a06003 	mov	r6, r3
    9394:	83a03012 	movhi	r3, #18	; 0x12
    9398:	93a03011 	movls	r3, #17	; 0x11
    939c:	e1cd30b0 	strh	r3, [sp]
    93a0:	e5903040 	ldr	r3, [r0, #64]
    93a4:	e3a07000 	mov	r7, #0	; 0x0
    93a8:	e3530000 	cmp	r3, #0	; 0x0
    93ac:	05903088 	ldreq	r3, [r0, #136]
    93b0:	158d2008 	strne	r2, [sp, #8]
    93b4:	00030293 	muleq	r3, r3, r2
    93b8:	058d3008 	streq	r3, [sp, #8]
    93bc:	e5903088 	ldr	r3, [r0, #136]
    93c0:	e3a02015 	mov	r2, #21	; 0x15
    93c4:	e58d302c 	str	r3, [sp, #44]
    93c8:	e3a03001 	mov	r3, #1	; 0x1
    93cc:	e58d2004 	str	r2, [sp, #4]
    93d0:	e58d1020 	str	r1, [sp, #32]
    93d4:	e1a04000 	mov	r4, r0
    93d8:	e58d3024 	str	r3, [sp, #36]
    93dc:	e58d701c 	str	r7, [sp, #28]
    93e0:	e58d6028 	str	r6, [sp, #40]
    93e4:	e1a0100d 	mov	r1, sp
    93e8:	e28d2020 	add	r2, sp, #32	; 0x20
    93ec:	e1a0e00f 	mov	lr, pc
    93f0:	e594f104 	ldr	pc, [r4, #260]
    93f4:	e2505000 	subs	r5, r0, #0	; 0x0
    93f8:	e1a0800d 	mov	r8, sp
    93fc:	11a00007 	movne	r0, r7
    9400:	1a000018 	bne	9468 <mmc_read_blocks+0xe4>
    9404:	e3560001 	cmp	r6, #1	; 0x1
    9408:	9a000012 	bls	9458 <mmc_read_blocks+0xd4>
    940c:	e3a0301d 	mov	r3, #29	; 0x1d
    9410:	e58d3004 	str	r3, [sp, #4]
    9414:	e3a0300c 	mov	r3, #12	; 0xc
    9418:	e1cd30b0 	strh	r3, [sp]
    941c:	e58d5008 	str	r5, [sp, #8]
    9420:	e58d501c 	str	r5, [sp, #28]
    9424:	e1a0100d 	mov	r1, sp
    9428:	e1a00004 	mov	r0, r4
    942c:	e1a02005 	mov	r2, r5
    9430:	e1a0e00f 	mov	lr, pc
    9434:	e594f104 	ldr	pc, [r4, #260]
    9438:	e3500000 	cmp	r0, #0	; 0x0
    943c:	0a000005 	beq	9458 <mmc_read_blocks+0xd4>
    9440:	e59f0028 	ldr	r0, [pc, #40]	; 9470 <mmc_read_blocks+0xec>
    9444:	e59f3028 	ldr	r3, [pc, #40]	; 9474 <mmc_read_blocks+0xf0>
    9448:	e1a0e00f 	mov	lr, pc
    944c:	e593f000 	ldr	pc, [r3]
    9450:	e1a00005 	mov	r0, r5
    9454:	ea000003 	b	9468 <mmc_read_blocks+0xe4>
    9458:	e1a00004 	mov	r0, r4
    945c:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    9460:	ebfffd15 	bl	88bc <mmc_send_status>
    9464:	e1a00006 	mov	r0, r6
    9468:	e28dd030 	add	sp, sp, #48	; 0x30
    946c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    9470:	00018010 	.word	0x00018010
    9474:	3c00d6cc 	.word	0x3c00d6cc

00009478 <mmc_bread>:
    9478:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    947c:	e252a000 	subs	sl, r2, #0	; 0x0
    9480:	e24dd004 	sub	sp, sp, #4	; 0x4
    9484:	e1a07001 	mov	r7, r1
    9488:	e1a08003 	mov	r8, r3
    948c:	0a000027 	beq	9530 <mmc_bread+0xb8>
    9490:	ebfffae8 	bl	8038 <find_mmc_device>
    9494:	e2506000 	subs	r6, r0, #0	; 0x0
    9498:	0a000024 	beq	9530 <mmc_bread+0xb8>
    949c:	e59620a8 	ldr	r2, [r6, #168]
    94a0:	e08a1007 	add	r1, sl, r7
    94a4:	e1510002 	cmp	r1, r2
    94a8:	9a000004 	bls	94c0 <mmc_bread+0x48>
    94ac:	e59f0088 	ldr	r0, [pc, #136]	; 953c <mmc_bread+0xc4>
    94b0:	e59f3088 	ldr	r3, [pc, #136]	; 9540 <mmc_bread+0xc8>
    94b4:	e1a0e00f 	mov	lr, pc
    94b8:	e593f000 	ldr	pc, [r3]
    94bc:	ea00001b 	b	9530 <mmc_bread+0xb8>
    94c0:	e5961088 	ldr	r1, [r6, #136]
    94c4:	ebfffacd 	bl	8000 <mmc_set_blocklen>
    94c8:	e3500000 	cmp	r0, #0	; 0x0
    94cc:	01a0500a 	moveq	r5, sl
    94d0:	0a000005 	beq	94ec <mmc_bread+0x74>
    94d4:	e5961088 	ldr	r1, [r6, #136]
    94d8:	e59f0064 	ldr	r0, [pc, #100]	; 9544 <mmc_bread+0xcc>
    94dc:	e59f305c 	ldr	r3, [pc, #92]	; 9540 <mmc_bread+0xc8>
    94e0:	e1a0e00f 	mov	lr, pc
    94e4:	e593f000 	ldr	pc, [r3]
    94e8:	ea000010 	b	9530 <mmc_bread+0xb8>
    94ec:	e5964110 	ldr	r4, [r6, #272]
    94f0:	e1a02007 	mov	r2, r7
    94f4:	e1550004 	cmp	r5, r4
    94f8:	31a04005 	movcc	r4, r5
    94fc:	e1a01008 	mov	r1, r8
    9500:	e1a00006 	mov	r0, r6
    9504:	e1a03004 	mov	r3, r4
    9508:	ebffff9d 	bl	9384 <mmc_read_blocks>
    950c:	e1500004 	cmp	r0, r4
    9510:	e0877000 	add	r7, r7, r0
    9514:	1a000005 	bne	9530 <mmc_bread+0xb8>
    9518:	e0555000 	subs	r5, r5, r0
    951c:	e5962088 	ldr	r2, [r6, #136]
    9520:	10288092 	mlane	r8, r2, r0, r8
    9524:	1afffff0 	bne	94ec <mmc_bread+0x74>
    9528:	e000029a 	mul	r0, sl, r2
    952c:	ea000000 	b	9534 <mmc_bread+0xbc>
    9530:	e3a00000 	mov	r0, #0	; 0x0
    9534:	e28dd004 	add	sp, sp, #4	; 0x4
    9538:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    953c:	00017fd2 	.word	0x00017fd2
    9540:	3c00d6cc 	.word	0x3c00d6cc
    9544:	00018077 	.word	0x00018077

00009548 <mmc2_vs_read>:
    9548:	e1a024a2 	lsr	r2, r2, #9
    954c:	e1821b83 	orr	r1, r2, r3, lsl #23
    9550:	e59d2000 	ldr	r2, [sp]
    9554:	e1a03000 	mov	r3, r0
    9558:	e1a024c2 	asr	r2, r2, #9
    955c:	e3a00002 	mov	r0, #2	; 0x2
    9560:	eaffffc4 	b	9478 <mmc_bread>

00009564 <mmc1_vs_read>:
    9564:	e1a024a2 	lsr	r2, r2, #9
    9568:	e1821b83 	orr	r1, r2, r3, lsl #23
    956c:	e59d2000 	ldr	r2, [sp]
    9570:	e1a03000 	mov	r3, r0
    9574:	e1a024c2 	asr	r2, r2, #9
    9578:	e3a00001 	mov	r0, #1	; 0x1
    957c:	eaffffbd 	b	9478 <mmc_bread>

00009580 <mmc0_vs_read>:
    9580:	e1a024a2 	lsr	r2, r2, #9
    9584:	e1821b83 	orr	r1, r2, r3, lsl #23
    9588:	e59d2000 	ldr	r2, [sp]
    958c:	e1a03000 	mov	r3, r0
    9590:	e1a024c2 	asr	r2, r2, #9
    9594:	e3a00000 	mov	r0, #0	; 0x0
    9598:	eaffffb6 	b	9478 <mmc_bread>

0000959c <mci_pull_data32>:
    959c:	e1a02142 	asr	r2, r2, #2
    95a0:	e3a0c000 	mov	ip, #0	; 0x0
    95a4:	ea000003 	b	95b8 <mci_pull_data32+0x1c>
    95a8:	e5903118 	ldr	r3, [r0, #280]
    95ac:	e5933200 	ldr	r3, [r3, #512]
    95b0:	e781300c 	str	r3, [r1, ip]
    95b4:	e28cc004 	add	ip, ip, #4	; 0x4
    95b8:	e3520000 	cmp	r2, #0	; 0x0
    95bc:	e2422001 	sub	r2, r2, #1	; 0x1
    95c0:	cafffff8 	bgt	95a8 <mci_pull_data32+0xc>
    95c4:	e12fff1e 	bx	lr

000095c8 <mci_push_data32>:
    95c8:	e92d4010 	push	{r4, lr}
    95cc:	e3a0c000 	mov	ip, #0	; 0x0
    95d0:	e1a04000 	mov	r4, r0
    95d4:	e1a00001 	mov	r0, r1
    95d8:	e1a01142 	asr	r1, r2, #2
    95dc:	ea000003 	b	95f0 <mci_push_data32+0x28>
    95e0:	e790200c 	ldr	r2, [r0, ip]
    95e4:	e5943118 	ldr	r3, [r4, #280]
    95e8:	e28cc004 	add	ip, ip, #4	; 0x4
    95ec:	e5832200 	str	r2, [r3, #512]
    95f0:	e3510000 	cmp	r1, #0	; 0x0
    95f4:	e2411001 	sub	r1, r1, #1	; 0x1
    95f8:	cafffff8 	bgt	95e0 <mci_push_data32+0x18>
    95fc:	e8bd8010 	pop	{r4, pc}

00009600 <imap_sdhc_initialize>:
    9600:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9604:	e3510000 	cmp	r1, #0	; 0x0
    9608:	e24dd004 	sub	sp, sp, #4	; 0x4
    960c:	1a000005 	bne	9628 <imap_sdhc_initialize+0x28>
    9610:	e59f00ec 	ldr	r0, [pc, #236]	; 9704 <imap_sdhc_initialize+0x104>
    9614:	e59f30ec 	ldr	r3, [pc, #236]	; 9708 <imap_sdhc_initialize+0x108>
    9618:	e1a0e00f 	mov	lr, pc
    961c:	e593f000 	ldr	pc, [r3]
    9620:	e3e00004 	mvn	r0, #4	; 0x4
    9624:	ea000034 	b	96fc <imap_sdhc_initialize+0xfc>
    9628:	e3500000 	cmp	r0, #0	; 0x0
    962c:	03a0359d 	moveq	r3, #658505728	; 0x27400000
    9630:	0a000005 	beq	964c <imap_sdhc_initialize+0x4c>
    9634:	e3500001 	cmp	r0, #1	; 0x1
    9638:	059f30cc 	ldreq	r3, [pc, #204]	; 970c <imap_sdhc_initialize+0x10c>
    963c:	0a000002 	beq	964c <imap_sdhc_initialize+0x4c>
    9640:	e3500002 	cmp	r0, #2	; 0x2
    9644:	1a000018 	bne	96ac <imap_sdhc_initialize+0xac>
    9648:	e59f30c0 	ldr	r3, [pc, #192]	; 9710 <imap_sdhc_initialize+0x110>
    964c:	e5813118 	str	r3, [r1, #280]
    9650:	e59f30bc 	ldr	r3, [pc, #188]	; 9714 <imap_sdhc_initialize+0x114>
    9654:	e591c130 	ldr	ip, [r1, #304]
    9658:	e5813104 	str	r3, [r1, #260]
    965c:	e59f30b4 	ldr	r3, [pc, #180]	; 9718 <imap_sdhc_initialize+0x118>
    9660:	e3a02000 	mov	r2, #0	; 0x0
    9664:	e5813108 	str	r3, [r1, #264]
    9668:	e59f30ac 	ldr	r3, [pc, #172]	; 971c <imap_sdhc_initialize+0x11c>
    966c:	e5810114 	str	r0, [r1, #276]
    9670:	e581310c 	str	r3, [r1, #268]
    9674:	e59f30a4 	ldr	r3, [pc, #164]	; 9720 <imap_sdhc_initialize+0x120>
    9678:	e5812034 	str	r2, [r1, #52]
    967c:	e581302c 	str	r3, [r1, #44]
    9680:	e3003101 	movw	r3, #257	; 0x101
    9684:	e5813050 	str	r3, [r1, #80]
    9688:	e59f3094 	ldr	r3, [pc, #148]	; 9724 <imap_sdhc_initialize+0x124>
    968c:	e5812028 	str	r2, [r1, #40]
    9690:	e5813038 	str	r3, [r1, #56]
    9694:	e59f308c 	ldr	r3, [pc, #140]	; 9728 <imap_sdhc_initialize+0x128>
    9698:	e5812110 	str	r2, [r1, #272]
    969c:	e581303c 	str	r3, [r1, #60]
    96a0:	e1a0000c 	mov	r0, ip
    96a4:	e2822010 	add	r2, r2, #16	; 0x10
    96a8:	ea000005 	b	96c4 <imap_sdhc_initialize+0xc4>
    96ac:	e59f0078 	ldr	r0, [pc, #120]	; 972c <imap_sdhc_initialize+0x12c>
    96b0:	e59f3050 	ldr	r3, [pc, #80]	; 9708 <imap_sdhc_initialize+0x108>
    96b4:	e1a0e00f 	mov	lr, pc
    96b8:	e593f000 	ldr	pc, [r3]
    96bc:	e3e00000 	mvn	r0, #0	; 0x0
    96c0:	ea00000d 	b	96fc <imap_sdhc_initialize+0xfc>
    96c4:	e5913130 	ldr	r3, [r1, #304]
    96c8:	e0823003 	add	r3, r2, r3
    96cc:	e2822010 	add	r2, r2, #16	; 0x10
    96d0:	e3520b01 	cmp	r2, #1024	; 0x400
    96d4:	e580300c 	str	r3, [r0, #12]
    96d8:	e2800010 	add	r0, r0, #16	; 0x10
    96dc:	1afffff8 	bne	96c4 <imap_sdhc_initialize+0xc4>
    96e0:	e5912130 	ldr	r2, [r1, #304]
    96e4:	e3a03020 	mov	r3, #32	; 0x20
    96e8:	e58c33f0 	str	r3, [ip, #1008]
    96ec:	e58c23fc 	str	r2, [ip, #1020]
    96f0:	e1a00001 	mov	r0, r1
    96f4:	ebfffad0 	bl	823c <mmc_register>
    96f8:	e3a00000 	mov	r0, #0	; 0x0
    96fc:	e28dd004 	add	sp, sp, #4	; 0x4
    9700:	e8bd8000 	pop	{pc}
    9704:	00018081 	.word	0x00018081
    9708:	3c00d6cc 	.word	0x3c00d6cc
    970c:	27200000 	.word	0x27200000
    9710:	27300000 	.word	0x27300000
    9714:	000097a0 	.word	0x000097a0
    9718:	00009c78 	.word	0x00009c78
    971c:	00009ac0 	.word	0x00009ac0
    9720:	00ff8000 	.word	0x00ff8000
    9724:	00061a80 	.word	0x00061a80
    9728:	018cba80 	.word	0x018cba80
    972c:	00018097 	.word	0x00018097

00009730 <mci_send_cmd>:
    9730:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    9734:	e5903118 	ldr	r3, [r0, #280]
    9738:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    973c:	e5832028 	str	r2, [r3, #40]
    9740:	e5903118 	ldr	r3, [r0, #280]
    9744:	e24dd004 	sub	sp, sp, #4	; 0x4
    9748:	e583102c 	str	r1, [r3, #44]
    974c:	e1a04000 	mov	r4, r0
    9750:	eb001092 	bl	d9a0 <get_ticks>
    9754:	e59f5040 	ldr	r5, [pc, #64]	; 979c <mci_send_cmd+0x6c>
    9758:	e1a06000 	mov	r6, r0
    975c:	e1a07001 	mov	r7, r1
    9760:	e5943118 	ldr	r3, [r4, #280]
    9764:	e593302c 	ldr	r3, [r3, #44]
    9768:	e3530000 	cmp	r3, #0	; 0x0
    976c:	aa000008 	bge	9794 <mci_send_cmd+0x64>
    9770:	eb00108a 	bl	d9a0 <get_ticks>
    9774:	e1a02000 	mov	r2, r0
    9778:	e1a03001 	mov	r3, r1
    977c:	e0522006 	subs	r2, r2, r6
    9780:	e0c33007 	sbc	r3, r3, r7
    9784:	e3530000 	cmp	r3, #0	; 0x0
    9788:	1a000001 	bne	9794 <mci_send_cmd+0x64>
    978c:	e1520005 	cmp	r2, r5
    9790:	9afffff2 	bls	9760 <mci_send_cmd+0x30>
    9794:	e28dd004 	add	sp, sp, #4	; 0x4
    9798:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    979c:	0007a11f 	.word	0x0007a11f

000097a0 <imap_sdhc_send_cmd>:
    97a0:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    97a4:	e252a000 	subs	sl, r2, #0	; 0x0
    97a8:	e1a04000 	mov	r4, r0
    97ac:	e1a08001 	mov	r8, r1
    97b0:	0a000039 	beq	989c <imap_sdhc_send_cmd+0xfc>
    97b4:	e99a000c 	ldmib	sl, {r2, r3}
    97b8:	e59a100c 	ldr	r1, [sl, #12]
    97bc:	e3120001 	tst	r2, #1	; 0x1
    97c0:	e0000193 	mul	r0, r3, r1
    97c4:	15942118 	ldrne	r2, [r4, #280]
    97c8:	11a03801 	lslne	r3, r1, #16
    97cc:	13833001 	orrne	r3, r3, #1	; 0x1
    97d0:	15823100 	strne	r3, [r2, #256]
    97d4:	e59a200c 	ldr	r2, [sl, #12]
    97d8:	e5943118 	ldr	r3, [r4, #280]
    97dc:	e1a0e000 	mov	lr, r0
    97e0:	e583201c 	str	r2, [r3, #28]
    97e4:	e5943118 	ldr	r3, [r4, #280]
    97e8:	e3a05000 	mov	r5, #0	; 0x0
    97ec:	e5830020 	str	r0, [r3, #32]
    97f0:	e5942118 	ldr	r2, [r4, #280]
    97f4:	e3019ff0 	movw	r9, #8176	; 0x1ff0
    97f8:	e5923000 	ldr	r3, [r2]
    97fc:	e3a0714a 	mov	r7, #-2147483630	; 0x80000012
    9800:	e3833020 	orr	r3, r3, #32	; 0x20
    9804:	e5823000 	str	r3, [r2]
    9808:	e59a6000 	ldr	r6, [sl]
    980c:	e594c130 	ldr	ip, [r4, #304]
    9810:	e15e0009 	cmp	lr, r9
    9814:	31a0100e 	movcc	r1, lr
    9818:	21a01009 	movcs	r1, r9
    981c:	e59c0004 	ldr	r0, [ip, #4]
    9820:	e59f3288 	ldr	r3, [pc, #648]	; 9ab0 <imap_sdhc_send_cmd+0x310>
    9824:	e1a02981 	lsl	r2, r1, #19
    9828:	e0003003 	and	r3, r0, r3
    982c:	e1a029a2 	lsr	r2, r2, #19
    9830:	e1822003 	orr	r2, r2, r3
    9834:	e05ee001 	subs	lr, lr, r1
    9838:	e0853006 	add	r3, r5, r6
    983c:	e98c000c 	stmib	ip, {r2, r3}
    9840:	e58c7000 	str	r7, [ip]
    9844:	e1a0200c 	mov	r2, ip
    9848:	e0855001 	add	r5, r5, r1
    984c:	e28cc010 	add	ip, ip, #16	; 0x10
    9850:	1affffee 	bne	9810 <imap_sdhc_send_cmd+0x70>
    9854:	e3c73002 	bic	r3, r7, #2	; 0x2
    9858:	e3833004 	orr	r3, r3, #4	; 0x4
    985c:	e5823000 	str	r3, [r2]
    9860:	e5942130 	ldr	r2, [r4, #304]
    9864:	e5923000 	ldr	r3, [r2]
    9868:	e3833008 	orr	r3, r3, #8	; 0x8
    986c:	e5823000 	str	r3, [r2]
    9870:	e5942118 	ldr	r2, [r4, #280]
    9874:	e5923000 	ldr	r3, [r2]
    9878:	e3833402 	orr	r3, r3, #33554432	; 0x2000000
    987c:	e5823000 	str	r3, [r2]
    9880:	e5942118 	ldr	r2, [r4, #280]
    9884:	e5923080 	ldr	r3, [r2, #128]
    9888:	e3833080 	orr	r3, r3, #128	; 0x80
    988c:	e5823080 	str	r3, [r2, #128]
    9890:	e5942118 	ldr	r2, [r4, #280]
    9894:	e3a03001 	mov	r3, #1	; 0x1
    9898:	e5823084 	str	r3, [r2, #132]
    989c:	e1d800b0 	ldrh	r0, [r8]
    98a0:	e5982004 	ldr	r2, [r8, #4]
    98a4:	e350000c 	cmp	r0, #12	; 0xc
    98a8:	03a03901 	moveq	r3, #16384	; 0x4000
    98ac:	13a03a02 	movne	r3, #8192	; 0x2000
    98b0:	e3120004 	tst	r2, #4	; 0x4
    98b4:	13833c01 	orrne	r3, r3, #256	; 0x100
    98b8:	e3120001 	tst	r2, #1	; 0x1
    98bc:	e3831202 	orr	r1, r3, #536870912	; 0x20000000
    98c0:	0a000002 	beq	98d0 <imap_sdhc_send_cmd+0x130>
    98c4:	e3120002 	tst	r2, #2	; 0x2
    98c8:	e3811040 	orr	r1, r1, #64	; 0x40
    98cc:	13811080 	orrne	r1, r1, #128	; 0x80
    98d0:	e35a0000 	cmp	sl, #0	; 0x0
    98d4:	0a000003 	beq	98e8 <imap_sdhc_send_cmd+0x148>
    98d8:	e59a3004 	ldr	r3, [sl, #4]
    98dc:	e3811c02 	orr	r1, r1, #512	; 0x200
    98e0:	e3130002 	tst	r3, #2	; 0x2
    98e4:	13811b01 	orrne	r1, r1, #1024	; 0x400
    98e8:	e594312c 	ldr	r3, [r4, #300]
    98ec:	e1811000 	orr	r1, r1, r0
    98f0:	e3530000 	cmp	r3, #0	; 0x0
    98f4:	13a03000 	movne	r3, #0	; 0x0
    98f8:	1584312c 	strne	r3, [r4, #300]
    98fc:	e5982008 	ldr	r2, [r8, #8]
    9900:	e5943118 	ldr	r3, [r4, #280]
    9904:	13811902 	orrne	r1, r1, #32768	; 0x8000
    9908:	e5832028 	str	r2, [r3, #40]
    990c:	e5943118 	ldr	r3, [r4, #280]
    9910:	e3811102 	orr	r1, r1, #-2147483648	; 0x80000000
    9914:	e583102c 	str	r1, [r3, #44]
    9918:	eb001020 	bl	d9a0 <get_ticks>
    991c:	e3005bb8 	movw	r5, #3000	; 0xbb8
    9920:	e1a06000 	mov	r6, r0
    9924:	e1a07001 	mov	r7, r1
    9928:	eb00101c 	bl	d9a0 <get_ticks>
    992c:	e1a02000 	mov	r2, r0
    9930:	e1a03001 	mov	r3, r1
    9934:	e0522006 	subs	r2, r2, r6
    9938:	e0c33007 	sbc	r3, r3, r7
    993c:	e3530000 	cmp	r3, #0	; 0x0
    9940:	1a000007 	bne	9964 <imap_sdhc_send_cmd+0x1c4>
    9944:	e1520005 	cmp	r2, r5
    9948:	8a000005 	bhi	9964 <imap_sdhc_send_cmd+0x1c4>
    994c:	e5943118 	ldr	r3, [r4, #280]
    9950:	e5932044 	ldr	r2, [r3, #68]
    9954:	e2831044 	add	r1, r3, #68	; 0x44
    9958:	e3120004 	tst	r2, #4	; 0x4
    995c:	0afffff1 	beq	9928 <imap_sdhc_send_cmd+0x188>
    9960:	ea000005 	b	997c <imap_sdhc_send_cmd+0x1dc>
    9964:	e59f0148 	ldr	r0, [pc, #328]	; 9ab4 <imap_sdhc_send_cmd+0x314>
    9968:	e59f3148 	ldr	r3, [pc, #328]	; 9ab8 <imap_sdhc_send_cmd+0x318>
    996c:	e1a0e00f 	mov	lr, pc
    9970:	e593f000 	ldr	pc, [r3]
    9974:	e3e00011 	mvn	r0, #17	; 0x11
    9978:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    997c:	e5915000 	ldr	r5, [r1]
    9980:	e3003142 	movw	r3, #322	; 0x142
    9984:	e0053003 	and	r3, r5, r3
    9988:	e3530000 	cmp	r3, #0	; 0x0
    998c:	e3a03004 	mov	r3, #4	; 0x4
    9990:	e5813000 	str	r3, [r1]
    9994:	0a00000e 	beq	99d4 <imap_sdhc_send_cmd+0x234>
    9998:	e1d810b0 	ldrh	r1, [r8]
    999c:	e5943118 	ldr	r3, [r4, #280]
    99a0:	e3510037 	cmp	r1, #55	; 0x37
    99a4:	13510008 	cmpne	r1, #8	; 0x8
    99a8:	e5835044 	str	r5, [r3, #68]
    99ac:	0a000004 	beq	99c4 <imap_sdhc_send_cmd+0x224>
    99b0:	e59f0104 	ldr	r0, [pc, #260]	; 9abc <imap_sdhc_send_cmd+0x31c>
    99b4:	e1a02005 	mov	r2, r5
    99b8:	e59f30f8 	ldr	r3, [pc, #248]	; 9ab8 <imap_sdhc_send_cmd+0x318>
    99bc:	e1a0e00f 	mov	lr, pc
    99c0:	e593f000 	ldr	pc, [r3]
    99c4:	e3150c01 	tst	r5, #256	; 0x100
    99c8:	13e00012 	mvnne	r0, #18	; 0x12
    99cc:	03e00011 	mvneq	r0, #17	; 0x11
    99d0:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    99d4:	e5983004 	ldr	r3, [r8, #4]
    99d8:	e5942118 	ldr	r2, [r4, #280]
    99dc:	e3130002 	tst	r3, #2	; 0x2
    99e0:	05923030 	ldreq	r3, [r2, #48]
    99e4:	0588300c 	streq	r3, [r8, #12]
    99e8:	0a00000a 	beq	9a18 <imap_sdhc_send_cmd+0x278>
    99ec:	e592303c 	ldr	r3, [r2, #60]
    99f0:	e588300c 	str	r3, [r8, #12]
    99f4:	e5943118 	ldr	r3, [r4, #280]
    99f8:	e5933038 	ldr	r3, [r3, #56]
    99fc:	e5883010 	str	r3, [r8, #16]
    9a00:	e5943118 	ldr	r3, [r4, #280]
    9a04:	e5933034 	ldr	r3, [r3, #52]
    9a08:	e5883014 	str	r3, [r8, #20]
    9a0c:	e5943118 	ldr	r3, [r4, #280]
    9a10:	e5933030 	ldr	r3, [r3, #48]
    9a14:	e5883018 	str	r3, [r8, #24]
    9a18:	e35a0000 	cmp	sl, #0	; 0x0
    9a1c:	01a0000a 	moveq	r0, sl
    9a20:	08bd87f0 	popeq	{r4, r5, r6, r7, r8, r9, sl, pc}
    9a24:	e594c118 	ldr	ip, [r4, #280]
    9a28:	e59a3004 	ldr	r3, [sl, #4]
    9a2c:	e28c7044 	add	r7, ip, #68	; 0x44
    9a30:	e2030001 	and	r0, r3, #1	; 0x1
    9a34:	e28c608c 	add	r6, ip, #140	; 0x8c
    9a38:	e5975000 	ldr	r5, [r7]
    9a3c:	e3500000 	cmp	r0, #0	; 0x0
    9a40:	e596e000 	ldr	lr, [r6]
    9a44:	e2051008 	and	r1, r5, #8	; 0x8
    9a48:	0a000005 	beq	9a64 <imap_sdhc_send_cmd+0x2c4>
    9a4c:	e3510000 	cmp	r1, #0	; 0x0
    9a50:	0afffff8 	beq	9a38 <imap_sdhc_send_cmd+0x298>
    9a54:	e31e0002 	tst	lr, #2	; 0x2
    9a58:	0afffff6 	beq	9a38 <imap_sdhc_send_cmd+0x298>
    9a5c:	e3a03002 	mov	r3, #2	; 0x2
    9a60:	ea000004 	b	9a78 <imap_sdhc_send_cmd+0x2d8>
    9a64:	e3510000 	cmp	r1, #0	; 0x0
    9a68:	0afffff2 	beq	9a38 <imap_sdhc_send_cmd+0x298>
    9a6c:	e31e0001 	tst	lr, #1	; 0x1
    9a70:	0afffff0 	beq	9a38 <imap_sdhc_send_cmd+0x298>
    9a74:	e3a03001 	mov	r3, #1	; 0x1
    9a78:	e58c308c 	str	r3, [ip, #140]
    9a7c:	e5943118 	ldr	r3, [r4, #280]
    9a80:	e3a00000 	mov	r0, #0	; 0x0
    9a84:	e5835044 	str	r5, [r3, #68]
    9a88:	e5942118 	ldr	r2, [r4, #280]
    9a8c:	e5923000 	ldr	r3, [r2]
    9a90:	e3c33402 	bic	r3, r3, #33554432	; 0x2000000
    9a94:	e3833004 	orr	r3, r3, #4	; 0x4
    9a98:	e5823000 	str	r3, [r2]
    9a9c:	e5942118 	ldr	r2, [r4, #280]
    9aa0:	e5923080 	ldr	r3, [r2, #128]
    9aa4:	e3c33080 	bic	r3, r3, #128	; 0x80
    9aa8:	e5823080 	str	r3, [r2, #128]
    9aac:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    9ab0:	03ffe000 	.word	0x03ffe000
    9ab4:	000180a9 	.word	0x000180a9
    9ab8:	3c00d6cc 	.word	0x3c00d6cc
    9abc:	000180b7 	.word	0x000180b7

00009ac0 <imap_sdhc_init>:
    9ac0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    9ac4:	e5901114 	ldr	r1, [r0, #276]
    9ac8:	e3a02000 	mov	r2, #0	; 0x0
    9acc:	e3a03001 	mov	r3, #1	; 0x1
    9ad0:	e3510001 	cmp	r1, #1	; 0x1
    9ad4:	e24dd004 	sub	sp, sp, #4	; 0x4
    9ad8:	e1a04000 	mov	r4, r0
    9adc:	e5802048 	str	r2, [r0, #72]
    9ae0:	e580312c 	str	r3, [r0, #300]
    9ae4:	e580211c 	str	r2, [r0, #284]
    9ae8:	0a000007 	beq	9b0c <imap_sdhc_init+0x4c>
    9aec:	3a000002 	bcc	9afc <imap_sdhc_init+0x3c>
    9af0:	e3510002 	cmp	r1, #2	; 0x2
    9af4:	1a00000c 	bne	9b2c <imap_sdhc_init+0x6c>
    9af8:	ea000007 	b	9b1c <imap_sdhc_init+0x5c>
    9afc:	e3a0000d 	mov	r0, #13	; 0xd
    9b00:	ebffdb5a 	bl	870 <pads_init_module>
    9b04:	e59f0148 	ldr	r0, [pc, #328]	; 9c54 <imap_sdhc_init+0x194>
    9b08:	ea000006 	b	9b28 <imap_sdhc_init+0x68>
    9b0c:	e3a0000e 	mov	r0, #14	; 0xe
    9b10:	ebffdb56 	bl	870 <pads_init_module>
    9b14:	e59f013c 	ldr	r0, [pc, #316]	; 9c58 <imap_sdhc_init+0x198>
    9b18:	ea000002 	b	9b28 <imap_sdhc_init+0x68>
    9b1c:	e3a0000f 	mov	r0, #15	; 0xf
    9b20:	ebffdb52 	bl	870 <pads_init_module>
    9b24:	e59f0130 	ldr	r0, [pc, #304]	; 9c5c <imap_sdhc_init+0x19c>
    9b28:	ebffdb15 	bl	784 <module_enable>
    9b2c:	e5942118 	ldr	r2, [r4, #280]
    9b30:	e59f5128 	ldr	r5, [pc, #296]	; 9c60 <imap_sdhc_init+0x1a0>
    9b34:	e5923070 	ldr	r3, [r2, #112]
    9b38:	e1a03943 	asr	r3, r3, #18
    9b3c:	e2033007 	and	r3, r3, #7	; 0x7
    9b40:	e3530002 	cmp	r3, #2	; 0x2
    9b44:	03a03003 	moveq	r3, #3	; 0x3
    9b48:	13a03002 	movne	r3, #2	; 0x2
    9b4c:	e5843120 	str	r3, [r4, #288]
    9b50:	e3a03007 	mov	r3, #7	; 0x7
    9b54:	e5823000 	str	r3, [r2]
    9b58:	e59f3104 	ldr	r3, [pc, #260]	; 9c64 <imap_sdhc_init+0x1a4>
    9b5c:	e5843124 	str	r3, [r4, #292]
    9b60:	e59f3100 	ldr	r3, [pc, #256]	; 9c68 <imap_sdhc_init+0x1a8>
    9b64:	e5843128 	str	r3, [r4, #296]
    9b68:	eb000f8c 	bl	d9a0 <get_ticks>
    9b6c:	e1a06000 	mov	r6, r0
    9b70:	e1a07001 	mov	r7, r1
    9b74:	e5943118 	ldr	r3, [r4, #280]
    9b78:	e3a00001 	mov	r0, #1	; 0x1
    9b7c:	e5933000 	ldr	r3, [r3]
    9b80:	e3130007 	tst	r3, #7	; 0x7
    9b84:	0a00000d 	beq	9bc0 <imap_sdhc_init+0x100>
    9b88:	eb000f95 	bl	d9e4 <udelay>
    9b8c:	eb000f83 	bl	d9a0 <get_ticks>
    9b90:	e1a02000 	mov	r2, r0
    9b94:	e1a03001 	mov	r3, r1
    9b98:	e0522006 	subs	r2, r2, r6
    9b9c:	e0c33007 	sbc	r3, r3, r7
    9ba0:	e3530000 	cmp	r3, #0	; 0x0
    9ba4:	1a000001 	bne	9bb0 <imap_sdhc_init+0xf0>
    9ba8:	e1520005 	cmp	r2, r5
    9bac:	9afffff0 	bls	9b74 <imap_sdhc_init+0xb4>
    9bb0:	e59f00b4 	ldr	r0, [pc, #180]	; 9c6c <imap_sdhc_init+0x1ac>
    9bb4:	e59f30b4 	ldr	r3, [pc, #180]	; 9c70 <imap_sdhc_init+0x1b0>
    9bb8:	e1a0e00f 	mov	lr, pc
    9bbc:	e593f000 	ldr	pc, [r3]
    9bc0:	e5943118 	ldr	r3, [r4, #280]
    9bc4:	e3a01000 	mov	r1, #0	; 0x0
    9bc8:	e5831024 	str	r1, [r3, #36]
    9bcc:	e5943118 	ldr	r3, [r4, #280]
    9bd0:	e3e02000 	mvn	r2, #0	; 0x0
    9bd4:	e5832044 	str	r2, [r3, #68]
    9bd8:	e5943118 	ldr	r3, [r4, #280]
    9bdc:	e1a00001 	mov	r0, r1
    9be0:	e5832014 	str	r2, [r3, #20]
    9be4:	e5942118 	ldr	r2, [r4, #280]
    9be8:	e592304c 	ldr	r3, [r2, #76]
    9bec:	e1a03283 	lsl	r3, r3, #5
    9bf0:	e1a03b23 	lsr	r3, r3, #22
    9bf4:	e1833803 	orr	r3, r3, r3, lsl #16
    9bf8:	e582304c 	str	r3, [r2, #76]
    9bfc:	e5943118 	ldr	r3, [r4, #280]
    9c00:	e5831010 	str	r1, [r3, #16]
    9c04:	e5942118 	ldr	r2, [r4, #280]
    9c08:	e5923004 	ldr	r3, [r2, #4]
    9c0c:	e3833001 	orr	r3, r3, #1	; 0x1
    9c10:	e5823004 	str	r3, [r2, #4]
    9c14:	e5942118 	ldr	r2, [r4, #280]
    9c18:	e59f3054 	ldr	r3, [pc, #84]	; 9c74 <imap_sdhc_init+0x1b4>
    9c1c:	e5823058 	str	r3, [r2, #88]
    9c20:	e5942118 	ldr	r2, [r4, #280]
    9c24:	e3003103 	movw	r3, #259	; 0x103
    9c28:	e5823090 	str	r3, [r2, #144]
    9c2c:	e5942130 	ldr	r2, [r4, #304]
    9c30:	e5943118 	ldr	r3, [r4, #280]
    9c34:	e5832088 	str	r2, [r3, #136]
    9c38:	e5942118 	ldr	r2, [r4, #280]
    9c3c:	e5923080 	ldr	r3, [r2, #128]
    9c40:	e3c3307c 	bic	r3, r3, #124	; 0x7c
    9c44:	e3833008 	orr	r3, r3, #8	; 0x8
    9c48:	e5823080 	str	r3, [r2, #128]
    9c4c:	e28dd004 	add	sp, sp, #4	; 0x4
    9c50:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    9c54:	21e39000 	.word	0x21e39000
    9c58:	21e38800 	.word	0x21e38800
    9c5c:	21e38c00 	.word	0x21e38c00
    9c60:	0007a11f 	.word	0x0007a11f
    9c64:	0000959c 	.word	0x0000959c
    9c68:	000095c8 	.word	0x000095c8
    9c6c:	000180cf 	.word	0x000180cf
    9c70:	3c00d6cc 	.word	0x3c00d6cc
    9c74:	008c2800 	.word	0x008c2800

00009c78 <imap_sdhc_set_ios>:
    9c78:	e5902048 	ldr	r2, [r0, #72]
    9c7c:	e590311c 	ldr	r3, [r0, #284]
    9c80:	e92d4070 	push	{r4, r5, r6, lr}
    9c84:	e1520003 	cmp	r2, r3
    9c88:	e1a05000 	mov	r5, r0
    9c8c:	0a00002a 	beq	9d3c <imap_sdhc_set_ios+0xc4>
    9c90:	e5903114 	ldr	r3, [r0, #276]
    9c94:	e59f00d0 	ldr	r0, [pc, #208]	; 9d6c <imap_sdhc_set_ios+0xf4>
    9c98:	e0830000 	add	r0, r3, r0
    9c9c:	e1a00200 	lsl	r0, r0, #4
    9ca0:	ebffdd57 	bl	1204 <module_get_clock>
    9ca4:	e5954048 	ldr	r4, [r5, #72]
    9ca8:	e1a06000 	mov	r6, r0
    9cac:	e1a01004 	mov	r1, r4
    9cb0:	eb002b33 	bl	14984 <__aeabi_uidivmod>
    9cb4:	e3510000 	cmp	r1, #0	; 0x0
    9cb8:	0a000005 	beq	9cd4 <imap_sdhc_set_ios+0x5c>
    9cbc:	e1a00006 	mov	r0, r6
    9cc0:	e1a01004 	mov	r1, r4
    9cc4:	eb002ab2 	bl	14794 <__aeabi_uidiv>
    9cc8:	e1a000a0 	lsr	r0, r0, #1
    9ccc:	e2806001 	add	r6, r0, #1	; 0x1
    9cd0:	ea000003 	b	9ce4 <imap_sdhc_set_ios+0x6c>
    9cd4:	e1a00006 	mov	r0, r6
    9cd8:	e1a01004 	mov	r1, r4
    9cdc:	eb002aac 	bl	14794 <__aeabi_uidiv>
    9ce0:	e1a060a0 	lsr	r6, r0, #1
    9ce4:	e5953118 	ldr	r3, [r5, #280]
    9ce8:	e3a04000 	mov	r4, #0	; 0x0
    9cec:	e5834010 	str	r4, [r3, #16]
    9cf0:	e5953118 	ldr	r3, [r5, #280]
    9cf4:	e1a00005 	mov	r0, r5
    9cf8:	e583400c 	str	r4, [r3, #12]
    9cfc:	e59f106c 	ldr	r1, [pc, #108]	; 9d70 <imap_sdhc_set_ios+0xf8>
    9d00:	e1a02004 	mov	r2, r4
    9d04:	ebfffe89 	bl	9730 <mci_send_cmd>
    9d08:	e5953118 	ldr	r3, [r5, #280]
    9d0c:	e1a00005 	mov	r0, r5
    9d10:	e5836008 	str	r6, [r3, #8]
    9d14:	e59f1054 	ldr	r1, [pc, #84]	; 9d70 <imap_sdhc_set_ios+0xf8>
    9d18:	e1a02004 	mov	r2, r4
    9d1c:	ebfffe83 	bl	9730 <mci_send_cmd>
    9d20:	e5952118 	ldr	r2, [r5, #280]
    9d24:	e3a03001 	mov	r3, #1	; 0x1
    9d28:	e5823010 	str	r3, [r2, #16]
    9d2c:	e1a00005 	mov	r0, r5
    9d30:	e1a02004 	mov	r2, r4
    9d34:	e59f1034 	ldr	r1, [pc, #52]	; 9d70 <imap_sdhc_set_ios+0xf8>
    9d38:	ebfffe7c 	bl	9730 <mci_send_cmd>
    9d3c:	e5953044 	ldr	r3, [r5, #68]
    9d40:	e3530004 	cmp	r3, #4	; 0x4
    9d44:	05952118 	ldreq	r2, [r5, #280]
    9d48:	03a03001 	moveq	r3, #1	; 0x1
    9d4c:	05823018 	streq	r3, [r2, #24]
    9d50:	08bd8070 	popeq	{r4, r5, r6, pc}
    9d54:	e3530008 	cmp	r3, #8	; 0x8
    9d58:	e5950118 	ldr	r0, [r5, #280]
    9d5c:	059f3010 	ldreq	r3, [pc, #16]	; 9d74 <imap_sdhc_set_ios+0xfc>
    9d60:	13a03000 	movne	r3, #0	; 0x0
    9d64:	e5803018 	str	r3, [r0, #24]
    9d68:	e8bd8070 	pop	{r4, r5, r6, pc}
    9d6c:	021e0a24 	.word	0x021e0a24
    9d70:	00202000 	.word	0x00202000
    9d74:	00010001 	.word	0x00010001

00009d78 <generic_ffs>:
    9d78:	e3500000 	cmp	r0, #0	; 0x0
    9d7c:	01a03000 	moveq	r3, r0
    9d80:	0a000010 	beq	9dc8 <generic_ffs+0x50>
    9d84:	e1a03800 	lsl	r3, r0, #16
    9d88:	e1a03823 	lsr	r3, r3, #16
    9d8c:	e3530000 	cmp	r3, #0	; 0x0
    9d90:	01a00840 	asreq	r0, r0, #16
    9d94:	13a03001 	movne	r3, #1	; 0x1
    9d98:	03a03011 	moveq	r3, #17	; 0x11
    9d9c:	e31000ff 	tst	r0, #255	; 0xff
    9da0:	01a00440 	asreq	r0, r0, #8
    9da4:	02833008 	addeq	r3, r3, #8	; 0x8
    9da8:	e310000f 	tst	r0, #15	; 0xf
    9dac:	01a00240 	asreq	r0, r0, #4
    9db0:	02833004 	addeq	r3, r3, #4	; 0x4
    9db4:	e3100003 	tst	r0, #3	; 0x3
    9db8:	01a00140 	asreq	r0, r0, #2
    9dbc:	02833002 	addeq	r3, r3, #2	; 0x2
    9dc0:	e3100001 	tst	r0, #1	; 0x1
    9dc4:	02833001 	addeq	r3, r3, #1	; 0x1
    9dc8:	e1a00003 	mov	r0, r3
    9dcc:	e12fff1e 	bx	lr

00009dd0 <print_nand_cfg>:
    9dd0:	e92d4030 	push	{r4, r5, lr}
    9dd4:	e1a05000 	mov	r5, r0
    9dd8:	e24dd004 	sub	sp, sp, #4	; 0x4
    9ddc:	e59f4110 	ldr	r4, [pc, #272]	; 9ef4 <print_nand_cfg+0x124>
    9de0:	e59f0110 	ldr	r0, [pc, #272]	; 9ef8 <print_nand_cfg+0x128>
    9de4:	e1a0e00f 	mov	lr, pc
    9de8:	e594f000 	ldr	pc, [r4]
    9dec:	e5d51000 	ldrb	r1, [r5]
    9df0:	e59f0104 	ldr	r0, [pc, #260]	; 9efc <print_nand_cfg+0x12c>
    9df4:	e1a0e00f 	mov	lr, pc
    9df8:	e594f000 	ldr	pc, [r4]
    9dfc:	e5d51001 	ldrb	r1, [r5, #1]
    9e00:	e59f00f8 	ldr	r0, [pc, #248]	; 9f00 <print_nand_cfg+0x130>
    9e04:	e1a0e00f 	mov	lr, pc
    9e08:	e594f000 	ldr	pc, [r4]
    9e0c:	e5d51002 	ldrb	r1, [r5, #2]
    9e10:	e59f00ec 	ldr	r0, [pc, #236]	; 9f04 <print_nand_cfg+0x134>
    9e14:	e1a0e00f 	mov	lr, pc
    9e18:	e594f000 	ldr	pc, [r4]
    9e1c:	e5d51003 	ldrb	r1, [r5, #3]
    9e20:	e59f00e0 	ldr	r0, [pc, #224]	; 9f08 <print_nand_cfg+0x138>
    9e24:	e1a0e00f 	mov	lr, pc
    9e28:	e594f000 	ldr	pc, [r4]
    9e2c:	e5d51004 	ldrb	r1, [r5, #4]
    9e30:	e59f00d4 	ldr	r0, [pc, #212]	; 9f0c <print_nand_cfg+0x13c>
    9e34:	e1a0e00f 	mov	lr, pc
    9e38:	e594f000 	ldr	pc, [r4]
    9e3c:	e5d51005 	ldrb	r1, [r5, #5]
    9e40:	e59f00c8 	ldr	r0, [pc, #200]	; 9f10 <print_nand_cfg+0x140>
    9e44:	e1a0e00f 	mov	lr, pc
    9e48:	e594f000 	ldr	pc, [r4]
    9e4c:	e5951008 	ldr	r1, [r5, #8]
    9e50:	e59f00bc 	ldr	r0, [pc, #188]	; 9f14 <print_nand_cfg+0x144>
    9e54:	e1a0e00f 	mov	lr, pc
    9e58:	e594f000 	ldr	pc, [r4]
    9e5c:	e595100c 	ldr	r1, [r5, #12]
    9e60:	e59f00b0 	ldr	r0, [pc, #176]	; 9f18 <print_nand_cfg+0x148>
    9e64:	e1a0e00f 	mov	lr, pc
    9e68:	e594f000 	ldr	pc, [r4]
    9e6c:	e5951010 	ldr	r1, [r5, #16]
    9e70:	e59f00a4 	ldr	r0, [pc, #164]	; 9f1c <print_nand_cfg+0x14c>
    9e74:	e1a0e00f 	mov	lr, pc
    9e78:	e594f000 	ldr	pc, [r4]
    9e7c:	e5951014 	ldr	r1, [r5, #20]
    9e80:	e59f0098 	ldr	r0, [pc, #152]	; 9f20 <print_nand_cfg+0x150>
    9e84:	e1a0e00f 	mov	lr, pc
    9e88:	e594f000 	ldr	pc, [r4]
    9e8c:	e5951018 	ldr	r1, [r5, #24]
    9e90:	e59f008c 	ldr	r0, [pc, #140]	; 9f24 <print_nand_cfg+0x154>
    9e94:	e1a0e00f 	mov	lr, pc
    9e98:	e594f000 	ldr	pc, [r4]
    9e9c:	e595101c 	ldr	r1, [r5, #28]
    9ea0:	e59f0080 	ldr	r0, [pc, #128]	; 9f28 <print_nand_cfg+0x158>
    9ea4:	e1a0e00f 	mov	lr, pc
    9ea8:	e594f000 	ldr	pc, [r4]
    9eac:	e5951028 	ldr	r1, [r5, #40]
    9eb0:	e59f0074 	ldr	r0, [pc, #116]	; 9f2c <print_nand_cfg+0x15c>
    9eb4:	e1a0e00f 	mov	lr, pc
    9eb8:	e594f000 	ldr	pc, [r4]
    9ebc:	e5951030 	ldr	r1, [r5, #48]
    9ec0:	e59f0068 	ldr	r0, [pc, #104]	; 9f30 <print_nand_cfg+0x160>
    9ec4:	e1a0e00f 	mov	lr, pc
    9ec8:	e594f000 	ldr	pc, [r4]
    9ecc:	e5951034 	ldr	r1, [r5, #52]
    9ed0:	e59f005c 	ldr	r0, [pc, #92]	; 9f34 <print_nand_cfg+0x164>
    9ed4:	e1a0e00f 	mov	lr, pc
    9ed8:	e594f000 	ldr	pc, [r4]
    9edc:	e5d51006 	ldrb	r1, [r5, #6]
    9ee0:	e59f0050 	ldr	r0, [pc, #80]	; 9f38 <print_nand_cfg+0x168>
    9ee4:	e1a0e00f 	mov	lr, pc
    9ee8:	e594f000 	ldr	pc, [r4]
    9eec:	e28dd004 	add	sp, sp, #4	; 0x4
    9ef0:	e8bd8030 	pop	{r4, r5, pc}
    9ef4:	3c00d6cc 	.word	0x3c00d6cc
    9ef8:	000180e2 	.word	0x000180e2
    9efc:	000180f4 	.word	0x000180f4
    9f00:	000180fc 	.word	0x000180fc
    9f04:	0001810c 	.word	0x0001810c
    9f08:	00018117 	.word	0x00018117
    9f0c:	00018127 	.word	0x00018127
    9f10:	00018137 	.word	0x00018137
    9f14:	00018147 	.word	0x00018147
    9f18:	00018151 	.word	0x00018151
    9f1c:	0001815b 	.word	0x0001815b
    9f20:	00018168 	.word	0x00018168
    9f24:	00018176 	.word	0x00018176
    9f28:	00018185 	.word	0x00018185
    9f2c:	00018194 	.word	0x00018194
    9f30:	000181a0 	.word	0x000181a0
    9f34:	000181ad 	.word	0x000181ad
    9f38:	000181bb 	.word	0x000181bb

00009f3c <nand_get_config>:
    9f3c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9f40:	e3500000 	cmp	r0, #0	; 0x0
    9f44:	e24dd004 	sub	sp, sp, #4	; 0x4
    9f48:	0a000007 	beq	9f6c <nand_get_config+0x30>
    9f4c:	e3500001 	cmp	r0, #1	; 0x1
    9f50:	1a00000b 	bne	9f84 <nand_get_config+0x48>
    9f54:	e59f0048 	ldr	r0, [pc, #72]	; 9fa4 <nand_get_config+0x68>
    9f58:	e5903000 	ldr	r3, [r0]
    9f5c:	e3530000 	cmp	r3, #0	; 0x0
    9f60:	128000b0 	addne	r0, r0, #176	; 0xb0
    9f64:	0a00000b 	beq	9f98 <nand_get_config+0x5c>
    9f68:	ea00000b 	b	9f9c <nand_get_config+0x60>
    9f6c:	e59f0030 	ldr	r0, [pc, #48]	; 9fa4 <nand_get_config+0x68>
    9f70:	e5903004 	ldr	r3, [r0, #4]
    9f74:	e3530000 	cmp	r3, #0	; 0x0
    9f78:	12800014 	addne	r0, r0, #20	; 0x14
    9f7c:	0a000005 	beq	9f98 <nand_get_config+0x5c>
    9f80:	ea000005 	b	9f9c <nand_get_config+0x60>
    9f84:	e1a01000 	mov	r1, r0
    9f88:	e59f3018 	ldr	r3, [pc, #24]	; 9fa8 <nand_get_config+0x6c>
    9f8c:	e59f0018 	ldr	r0, [pc, #24]	; 9fac <nand_get_config+0x70>
    9f90:	e1a0e00f 	mov	lr, pc
    9f94:	e593f000 	ldr	pc, [r3]
    9f98:	e3a00000 	mov	r0, #0	; 0x0
    9f9c:	e28dd004 	add	sp, sp, #4	; 0x4
    9fa0:	e8bd8000 	pop	{pc}
    9fa4:	3c00d734 	.word	0x3c00d734
    9fa8:	3c00d6cc 	.word	0x3c00d6cc
    9fac:	000181c5 	.word	0x000181c5

00009fb0 <bnd_vs_align>:
    9fb0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9fb4:	e3a00000 	mov	r0, #0	; 0x0
    9fb8:	e24dd004 	sub	sp, sp, #4	; 0x4
    9fbc:	ebffffde 	bl	9f3c <nand_get_config>
    9fc0:	e3500000 	cmp	r0, #0	; 0x0
    9fc4:	15900014 	ldrne	r0, [r0, #20]
    9fc8:	e28dd004 	add	sp, sp, #4	; 0x4
    9fcc:	e8bd8000 	pop	{pc}

00009fd0 <nnd_vs_align>:
    9fd0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9fd4:	e3a00001 	mov	r0, #1	; 0x1
    9fd8:	e24dd004 	sub	sp, sp, #4	; 0x4
    9fdc:	ebffffd6 	bl	9f3c <nand_get_config>
    9fe0:	e3500000 	cmp	r0, #0	; 0x0
    9fe4:	15900014 	ldrne	r0, [r0, #20]
    9fe8:	e28dd004 	add	sp, sp, #4	; 0x4
    9fec:	e8bd8000 	pop	{pc}

00009ff0 <fnd_vs_align>:
    9ff0:	eafffff6 	b	9fd0 <nnd_vs_align>

00009ff4 <fnd_align>:
    9ff4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    9ff8:	e3a00001 	mov	r0, #1	; 0x1
    9ffc:	e24dd004 	sub	sp, sp, #4	; 0x4
    a000:	ebffffcd 	bl	9f3c <nand_get_config>
    a004:	e3500000 	cmp	r0, #0	; 0x0
    a008:	15902014 	ldrne	r2, [r0, #20]
    a00c:	15903010 	ldrne	r3, [r0, #16]
    a010:	10830002 	addne	r0, r3, r2
    a014:	e28dd004 	add	sp, sp, #4	; 0x4
    a018:	e8bd8000 	pop	{pc}

0000a01c <fnd_size_match>:
    a01c:	e92d4010 	push	{r4, lr}
    a020:	e1a04000 	mov	r4, r0
    a024:	e3a00001 	mov	r0, #1	; 0x1
    a028:	ebffffc3 	bl	9f3c <nand_get_config>
    a02c:	e3500000 	cmp	r0, #0	; 0x0
    a030:	08bd8010 	popeq	{r4, pc}
    a034:	e5903014 	ldr	r3, [r0, #20]
    a038:	e5901010 	ldr	r1, [r0, #16]
    a03c:	e1a00004 	mov	r0, r4
    a040:	e0811003 	add	r1, r1, r3
    a044:	eb002a4e 	bl	14984 <__aeabi_uidivmod>
    a048:	e2710001 	rsbs	r0, r1, #1	; 0x1
    a04c:	33a00000 	movcc	r0, #0	; 0x0
    a050:	e8bd8010 	pop	{r4, pc}

0000a054 <fnd_size_shrink>:
    a054:	e92d4030 	push	{r4, r5, lr}
    a058:	e1a05000 	mov	r5, r0
    a05c:	e24dd004 	sub	sp, sp, #4	; 0x4
    a060:	e3a00001 	mov	r0, #1	; 0x1
    a064:	ebffffb4 	bl	9f3c <nand_get_config>
    a068:	e3500000 	cmp	r0, #0	; 0x0
    a06c:	0a000005 	beq	a088 <fnd_size_shrink+0x34>
    a070:	e5904014 	ldr	r4, [r0, #20]
    a074:	e5901010 	ldr	r1, [r0, #16]
    a078:	e1a00005 	mov	r0, r5
    a07c:	e0841001 	add	r1, r4, r1
    a080:	eb0029c3 	bl	14794 <__aeabi_uidiv>
    a084:	e0000094 	mul	r0, r4, r0
    a088:	e28dd004 	add	sp, sp, #4	; 0x4
    a08c:	e8bd8030 	pop	{r4, r5, pc}

0000a090 <init_nand>:
    a090:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    a094:	e3a01000 	mov	r1, #0	; 0x0
    a098:	e24dd004 	sub	sp, sp, #4	; 0x4
    a09c:	e3a02f53 	mov	r2, #332	; 0x14c
    a0a0:	e59f000c 	ldr	r0, [pc, #12]	; a0b4 <init_nand+0x24>
    a0a4:	ebffdd24 	bl	153c <memset>
    a0a8:	e3a00000 	mov	r0, #0	; 0x0
    a0ac:	e28dd004 	add	sp, sp, #4	; 0x4
    a0b0:	e8bd8000 	pop	{pc}
    a0b4:	3c00d734 	.word	0x3c00d734

0000a0b8 <__base_block_markbad>:
    a0b8:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    a0bc:	e24dd07c 	sub	sp, sp, #124	; 0x7c
    a0c0:	e1cd06f0 	strd	r0, [sp, #96]
    a0c4:	e3a00001 	mov	r0, #1	; 0x1
    a0c8:	ebffff9b 	bl	9f3c <nand_get_config>
    a0cc:	e250a000 	subs	sl, r0, #0	; 0x0
    a0d0:	03e00000 	mvneq	r0, #0	; 0x0
    a0d4:	0a000035 	beq	a1b0 <__base_block_markbad+0xf8>
    a0d8:	e3a04000 	mov	r4, #0	; 0x0
    a0dc:	e58d4074 	str	r4, [sp, #116]
    a0e0:	e59a5014 	ldr	r5, [sl, #20]
    a0e4:	e5da7005 	ldrb	r7, [sl, #5]
    a0e8:	e5da8003 	ldrb	r8, [sl, #3]
    a0ec:	e59f30c4 	ldr	r3, [pc, #196]	; a1b8 <__base_block_markbad+0x100>
    a0f0:	e58d5000 	str	r5, [sp]
    a0f4:	e59ac010 	ldr	ip, [sl, #16]
    a0f8:	e5932010 	ldr	r2, [r3, #16]
    a0fc:	e3a06001 	mov	r6, #1	; 0x1
    a100:	e1cd06d0 	ldrd	r0, [sp, #96]
    a104:	e58dc004 	str	ip, [sp, #4]
    a108:	e58d6008 	str	r6, [sp, #8]
    a10c:	eb002ab7 	bl	14bf0 <__aeabi_lasr>
    a110:	e58d5014 	str	r5, [sp, #20]
    a114:	e58d000c 	str	r0, [sp, #12]
    a118:	e58d4010 	str	r4, [sp, #16]
    a11c:	e58d7018 	str	r7, [sp, #24]
    a120:	e5da3004 	ldrb	r3, [sl, #4]
    a124:	e58d4024 	str	r4, [sp, #36]
    a128:	e58d4020 	str	r4, [sp, #32]
    a12c:	e58d301c 	str	r3, [sp, #28]
    a130:	e5da3002 	ldrb	r3, [sl, #2]
    a134:	e28a204c 	add	r2, sl, #76	; 0x4c
    a138:	e58d3028 	str	r3, [sp, #40]
    a13c:	e5da3001 	ldrb	r3, [sl, #1]
    a140:	e28a105c 	add	r1, sl, #92	; 0x5c
    a144:	e58d302c 	str	r3, [sp, #44]
    a148:	e28d3070 	add	r3, sp, #112	; 0x70
    a14c:	e58d3048 	str	r3, [sp, #72]
    a150:	e28d306c 	add	r3, sp, #108	; 0x6c
    a154:	e58d304c 	str	r3, [sp, #76]
    a158:	e28d3074 	add	r3, sp, #116	; 0x74
    a15c:	e28a006c 	add	r0, sl, #108	; 0x6c
    a160:	e28ac07c 	add	ip, sl, #124	; 0x7c
    a164:	e58d3050 	str	r3, [sp, #80]
    a168:	e28a5038 	add	r5, sl, #56	; 0x38
    a16c:	e3a03004 	mov	r3, #4	; 0x4
    a170:	e28a408c 	add	r4, sl, #140	; 0x8c
    a174:	e58d2034 	str	r2, [sp, #52]
    a178:	e58d1038 	str	r1, [sp, #56]
    a17c:	e58d003c 	str	r0, [sp, #60]
    a180:	e58d3054 	str	r3, [sp, #84]
    a184:	e58d5030 	str	r5, [sp, #48]
    a188:	e58dc040 	str	ip, [sp, #64]
    a18c:	e58d4044 	str	r4, [sp, #68]
    a190:	e58d6058 	str	r6, [sp, #88]
    a194:	e5dac006 	ldrb	ip, [sl, #6]
    a198:	e1a01006 	mov	r1, r6
    a19c:	e1a00007 	mov	r0, r7
    a1a0:	e1a02008 	mov	r2, r8
    a1a4:	e1a03006 	mov	r3, r6
    a1a8:	e58dc05c 	str	ip, [sp, #92]
    a1ac:	eb000a35 	bl	ca88 <nf2_page_op>
    a1b0:	e28dd07c 	add	sp, sp, #124	; 0x7c
    a1b4:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    a1b8:	3c00d734 	.word	0x3c00d734

0000a1bc <bnd_vs_write>:
    a1bc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a1c0:	e3a05000 	mov	r5, #0	; 0x0
    a1c4:	e24dd08c 	sub	sp, sp, #140	; 0x8c
    a1c8:	e1a08000 	mov	r8, r0
    a1cc:	e1a00005 	mov	r0, r5
    a1d0:	e1a06002 	mov	r6, r2
    a1d4:	e1a07003 	mov	r7, r3
    a1d8:	e58d5084 	str	r5, [sp, #132]
    a1dc:	e58d5080 	str	r5, [sp, #128]
    a1e0:	e58d507c 	str	r5, [sp, #124]
    a1e4:	ebffff54 	bl	9f3c <nand_get_config>
    a1e8:	e1a04000 	mov	r4, r0
    a1ec:	e3a00001 	mov	r0, #1	; 0x1
    a1f0:	ebffff51 	bl	9f3c <nand_get_config>
    a1f4:	e1540005 	cmp	r4, r5
    a1f8:	11500005 	cmpne	r0, r5
    a1fc:	0a00004f 	beq	a340 <bnd_vs_write+0x184>
    a200:	e59f3148 	ldr	r3, [pc, #328]	; a350 <bnd_vs_write+0x194>
    a204:	e1a00006 	mov	r0, r6
    a208:	e593200c 	ldr	r2, [r3, #12]
    a20c:	e1a01007 	mov	r1, r7
    a210:	eb002a76 	bl	14bf0 <__aeabi_lasr>
    a214:	e5941014 	ldr	r1, [r4, #20]
    a218:	e58d0060 	str	r0, [sp, #96]
    a21c:	e59d00b0 	ldr	r0, [sp, #176]
    a220:	eb00295b 	bl	14794 <__aeabi_uidiv>
    a224:	e2842038 	add	r2, r4, #56	; 0x38
    a228:	e284304c 	add	r3, r4, #76	; 0x4c
    a22c:	e284c05c 	add	ip, r4, #92	; 0x5c
    a230:	e284e06c 	add	lr, r4, #108	; 0x6c
    a234:	e1a07008 	mov	r7, r8
    a238:	e58d0074 	str	r0, [sp, #116]
    a23c:	e58d2064 	str	r2, [sp, #100]
    a240:	e58d3068 	str	r3, [sp, #104]
    a244:	e58dc06c 	str	ip, [sp, #108]
    a248:	e58de070 	str	lr, [sp, #112]
    a24c:	e284b07c 	add	fp, r4, #124	; 0x7c
    a250:	e284908c 	add	r9, r4, #140	; 0x8c
    a254:	e1a06005 	mov	r6, r5
    a258:	e1a0a005 	mov	sl, r5
    a25c:	e3a08001 	mov	r8, #1	; 0x1
    a260:	ea00002d 	b	a31c <bnd_vs_write+0x160>
    a264:	e59f20e8 	ldr	r2, [pc, #232]	; a354 <bnd_vs_write+0x198>
    a268:	e594c014 	ldr	ip, [r4, #20]
    a26c:	e58d207c 	str	r2, [sp, #124]
    a270:	e5d40005 	ldrb	r0, [r4, #5]
    a274:	e5d42003 	ldrb	r2, [r4, #3]
    a278:	e58de00c 	str	lr, [sp, #12]
    a27c:	e58dc014 	str	ip, [sp, #20]
    a280:	e58d7010 	str	r7, [sp, #16]
    a284:	e58dc000 	str	ip, [sp]
    a288:	e58da004 	str	sl, [sp, #4]
    a28c:	e58d8008 	str	r8, [sp, #8]
    a290:	e58da018 	str	sl, [sp, #24]
    a294:	e58da01c 	str	sl, [sp, #28]
    a298:	e58d8020 	str	r8, [sp, #32]
    a29c:	e58d8024 	str	r8, [sp, #36]
    a2a0:	e5d4c002 	ldrb	ip, [r4, #2]
    a2a4:	e59de064 	ldr	lr, [sp, #100]
    a2a8:	e58dc028 	str	ip, [sp, #40]
    a2ac:	e5d4c001 	ldrb	ip, [r4, #1]
    a2b0:	e58de030 	str	lr, [sp, #48]
    a2b4:	e58dc02c 	str	ip, [sp, #44]
    a2b8:	e59dc068 	ldr	ip, [sp, #104]
    a2bc:	e59de06c 	ldr	lr, [sp, #108]
    a2c0:	e58dc034 	str	ip, [sp, #52]
    a2c4:	e59dc070 	ldr	ip, [sp, #112]
    a2c8:	e58de038 	str	lr, [sp, #56]
    a2cc:	e58dc03c 	str	ip, [sp, #60]
    a2d0:	e28de084 	add	lr, sp, #132	; 0x84
    a2d4:	e28dc080 	add	ip, sp, #128	; 0x80
    a2d8:	e58de048 	str	lr, [sp, #72]
    a2dc:	e58dc04c 	str	ip, [sp, #76]
    a2e0:	e28de07c 	add	lr, sp, #124	; 0x7c
    a2e4:	e3a0c004 	mov	ip, #4	; 0x4
    a2e8:	e58de050 	str	lr, [sp, #80]
    a2ec:	e58db040 	str	fp, [sp, #64]
    a2f0:	e58d9044 	str	r9, [sp, #68]
    a2f4:	e58dc054 	str	ip, [sp, #84]
    a2f8:	e58d8058 	str	r8, [sp, #88]
    a2fc:	e5d4c006 	ldrb	ip, [r4, #6]
    a300:	e58dc05c 	str	ip, [sp, #92]
    a304:	eb0009df 	bl	ca88 <nf2_page_op>
    a308:	e3500000 	cmp	r0, #0	; 0x0
    a30c:	1a00000b 	bne	a340 <bnd_vs_write+0x184>
    a310:	e5943014 	ldr	r3, [r4, #20]
    a314:	e0866003 	add	r6, r6, r3
    a318:	e0877003 	add	r7, r7, r3
    a31c:	e59de074 	ldr	lr, [sp, #116]
    a320:	e59d2060 	ldr	r2, [sp, #96]
    a324:	e155000e 	cmp	r5, lr
    a328:	e1a0100a 	mov	r1, sl
    a32c:	e085e002 	add	lr, r5, r2
    a330:	e1a03008 	mov	r3, r8
    a334:	e2855001 	add	r5, r5, #1	; 0x1
    a338:	baffffc9 	blt	a264 <bnd_vs_write+0xa8>
    a33c:	ea000000 	b	a344 <bnd_vs_write+0x188>
    a340:	e3e06000 	mvn	r6, #0	; 0x0
    a344:	e1a00006 	mov	r0, r6
    a348:	e28dd08c 	add	sp, sp, #140	; 0x8c
    a34c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a350:	3c00d734 	.word	0x3c00d734
    a354:	031800ff 	.word	0x031800ff

0000a358 <bnd_vs_read>:
    a358:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a35c:	e3a05000 	mov	r5, #0	; 0x0
    a360:	e24dd08c 	sub	sp, sp, #140	; 0x8c
    a364:	e1a08000 	mov	r8, r0
    a368:	e1a00005 	mov	r0, r5
    a36c:	e1a06002 	mov	r6, r2
    a370:	e1a07003 	mov	r7, r3
    a374:	e58d5084 	str	r5, [sp, #132]
    a378:	e58d5080 	str	r5, [sp, #128]
    a37c:	e58d507c 	str	r5, [sp, #124]
    a380:	ebfffeed 	bl	9f3c <nand_get_config>
    a384:	e2504000 	subs	r4, r0, #0	; 0x0
    a388:	0a000056 	beq	a4e8 <bnd_vs_read+0x190>
    a38c:	e59f3164 	ldr	r3, [pc, #356]	; a4f8 <bnd_vs_read+0x1a0>
    a390:	e1a00006 	mov	r0, r6
    a394:	e593200c 	ldr	r2, [r3, #12]
    a398:	e1a01007 	mov	r1, r7
    a39c:	eb002a13 	bl	14bf0 <__aeabi_lasr>
    a3a0:	e5941014 	ldr	r1, [r4, #20]
    a3a4:	e58d0060 	str	r0, [sp, #96]
    a3a8:	e59d00b0 	ldr	r0, [sp, #176]
    a3ac:	eb0028f8 	bl	14794 <__aeabi_uidiv>
    a3b0:	e2842038 	add	r2, r4, #56	; 0x38
    a3b4:	e284304c 	add	r3, r4, #76	; 0x4c
    a3b8:	e284c05c 	add	ip, r4, #92	; 0x5c
    a3bc:	e284e06c 	add	lr, r4, #108	; 0x6c
    a3c0:	e1a07008 	mov	r7, r8
    a3c4:	e58d0074 	str	r0, [sp, #116]
    a3c8:	e58d2064 	str	r2, [sp, #100]
    a3cc:	e58d3068 	str	r3, [sp, #104]
    a3d0:	e58dc06c 	str	ip, [sp, #108]
    a3d4:	e58de070 	str	lr, [sp, #112]
    a3d8:	e284b07c 	add	fp, r4, #124	; 0x7c
    a3dc:	e284908c 	add	r9, r4, #140	; 0x8c
    a3e0:	e1a06005 	mov	r6, r5
    a3e4:	e1a08005 	mov	r8, r5
    a3e8:	e3a0a001 	mov	sl, #1	; 0x1
    a3ec:	ea000034 	b	a4c4 <bnd_vs_read+0x16c>
    a3f0:	e594c014 	ldr	ip, [r4, #20]
    a3f4:	e5d40005 	ldrb	r0, [r4, #5]
    a3f8:	e5d42003 	ldrb	r2, [r4, #3]
    a3fc:	e58de00c 	str	lr, [sp, #12]
    a400:	e58dc014 	str	ip, [sp, #20]
    a404:	e58d7010 	str	r7, [sp, #16]
    a408:	e58dc000 	str	ip, [sp]
    a40c:	e58d8004 	str	r8, [sp, #4]
    a410:	e58d8008 	str	r8, [sp, #8]
    a414:	e58d8018 	str	r8, [sp, #24]
    a418:	e58d801c 	str	r8, [sp, #28]
    a41c:	e58da020 	str	sl, [sp, #32]
    a420:	e58da024 	str	sl, [sp, #36]
    a424:	e5d4c002 	ldrb	ip, [r4, #2]
    a428:	e59de064 	ldr	lr, [sp, #100]
    a42c:	e58dc028 	str	ip, [sp, #40]
    a430:	e5d4c001 	ldrb	ip, [r4, #1]
    a434:	e58de030 	str	lr, [sp, #48]
    a438:	e58dc02c 	str	ip, [sp, #44]
    a43c:	e59dc068 	ldr	ip, [sp, #104]
    a440:	e59de06c 	ldr	lr, [sp, #108]
    a444:	e58dc034 	str	ip, [sp, #52]
    a448:	e59dc070 	ldr	ip, [sp, #112]
    a44c:	e58de038 	str	lr, [sp, #56]
    a450:	e58dc03c 	str	ip, [sp, #60]
    a454:	e28de084 	add	lr, sp, #132	; 0x84
    a458:	e28dc080 	add	ip, sp, #128	; 0x80
    a45c:	e58de048 	str	lr, [sp, #72]
    a460:	e58dc04c 	str	ip, [sp, #76]
    a464:	e28de07c 	add	lr, sp, #124	; 0x7c
    a468:	e3a0c004 	mov	ip, #4	; 0x4
    a46c:	e58de050 	str	lr, [sp, #80]
    a470:	e58db040 	str	fp, [sp, #64]
    a474:	e58d9044 	str	r9, [sp, #68]
    a478:	e58dc054 	str	ip, [sp, #84]
    a47c:	e58da058 	str	sl, [sp, #88]
    a480:	e5d4c006 	ldrb	ip, [r4, #6]
    a484:	e58dc05c 	str	ip, [sp, #92]
    a488:	eb00097e 	bl	ca88 <nf2_page_op>
    a48c:	e3500000 	cmp	r0, #0	; 0x0
    a490:	1a000014 	bne	a4e8 <bnd_vs_read+0x190>
    a494:	e59d3084 	ldr	r3, [sp, #132]
    a498:	e1dd27be 	ldrh	r2, [sp, #126]
    a49c:	e3530000 	cmp	r3, #0	; 0x0
    a4a0:	1a000006 	bne	a4c0 <bnd_vs_read+0x168>
    a4a4:	e59d3080 	ldr	r3, [sp, #128]
    a4a8:	e3530000 	cmp	r3, #0	; 0x0
    a4ac:	1a000003 	bne	a4c0 <bnd_vs_read+0x168>
    a4b0:	e3520fc6 	cmp	r2, #792	; 0x318
    a4b4:	05943014 	ldreq	r3, [r4, #20]
    a4b8:	00877003 	addeq	r7, r7, r3
    a4bc:	00866003 	addeq	r6, r6, r3
    a4c0:	e58d207c 	str	r2, [sp, #124]
    a4c4:	e59de074 	ldr	lr, [sp, #116]
    a4c8:	e59d2060 	ldr	r2, [sp, #96]
    a4cc:	e155000e 	cmp	r5, lr
    a4d0:	e1a01008 	mov	r1, r8
    a4d4:	e085e002 	add	lr, r5, r2
    a4d8:	e1a0300a 	mov	r3, sl
    a4dc:	e2855001 	add	r5, r5, #1	; 0x1
    a4e0:	baffffc2 	blt	a3f0 <bnd_vs_read+0x98>
    a4e4:	ea000000 	b	a4ec <bnd_vs_read+0x194>
    a4e8:	e3e06000 	mvn	r6, #0	; 0x0
    a4ec:	e1a00006 	mov	r0, r6
    a4f0:	e28dd08c 	add	sp, sp, #140	; 0x8c
    a4f4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a4f8:	3c00d734 	.word	0x3c00d734

0000a4fc <base_vs_write>:
    a4fc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a500:	e3a01000 	mov	r1, #0	; 0x0
    a504:	e24dd094 	sub	sp, sp, #148	; 0x94
    a508:	e1a0a000 	mov	sl, r0
    a50c:	e3a00001 	mov	r0, #1	; 0x1
    a510:	e58d1088 	str	r1, [sp, #136]
    a514:	e1a08002 	mov	r8, r2
    a518:	e1a09003 	mov	r9, r3
    a51c:	e58d108c 	str	r1, [sp, #140]
    a520:	e59db0bc 	ldr	fp, [sp, #188]
    a524:	ebfffe84 	bl	9f3c <nand_get_config>
    a528:	e1a04000 	mov	r4, r0
    a52c:	e5900010 	ldr	r0, [r0, #16]
    a530:	e2800004 	add	r0, r0, #4	; 0x4
    a534:	ebffdc6d 	bl	16f0 <malloc>
    a538:	e2507000 	subs	r7, r0, #0	; 0x0
    a53c:	1a000007 	bne	a560 <base_vs_write+0x64>
    a540:	e5941010 	ldr	r1, [r4, #16]
    a544:	e59f0240 	ldr	r0, [pc, #576]	; a78c <base_vs_write+0x290>
    a548:	e2811004 	add	r1, r1, #4	; 0x4
    a54c:	e59f323c 	ldr	r3, [pc, #572]	; a790 <base_vs_write+0x294>
    a550:	e1a0e00f 	mov	lr, pc
    a554:	e593f000 	ldr	pc, [r3]
    a558:	e3e05000 	mvn	r5, #0	; 0x0
    a55c:	ea000087 	b	a780 <base_vs_write+0x284>
    a560:	e35b0000 	cmp	fp, #0	; 0x0
    a564:	01a0200b 	moveq	r2, fp
    a568:	03e01000 	mvneq	r1, #0	; 0x0
    a56c:	0a000002 	beq	a57c <base_vs_write+0x80>
    a570:	ea000007 	b	a594 <base_vs_write+0x98>
    a574:	e7c71002 	strb	r1, [r7, r2]
    a578:	e2822001 	add	r2, r2, #1	; 0x1
    a57c:	e5943010 	ldr	r3, [r4, #16]
    a580:	e2833004 	add	r3, r3, #4	; 0x4
    a584:	e1520003 	cmp	r2, r3
    a588:	3afffff9 	bcc	a574 <base_vs_write+0x78>
    a58c:	e3a03000 	mov	r3, #0	; 0x0
    a590:	e5c73001 	strb	r3, [r7, #1]
    a594:	e59f31f8 	ldr	r3, [pc, #504]	; a794 <base_vs_write+0x298>
    a598:	e35b0000 	cmp	fp, #0	; 0x0
    a59c:	e5932010 	ldr	r2, [r3, #16]
    a5a0:	e1a00008 	mov	r0, r8
    a5a4:	e1a01009 	mov	r1, r9
    a5a8:	01a0500b 	moveq	r5, fp
    a5ac:	15945010 	ldrne	r5, [r4, #16]
    a5b0:	eb00298e 	bl	14bf0 <__aeabi_lasr>
    a5b4:	e5946014 	ldr	r6, [r4, #20]
    a5b8:	e58d0060 	str	r0, [sp, #96]
    a5bc:	e0851006 	add	r1, r5, r6
    a5c0:	e59d00b8 	ldr	r0, [sp, #184]
    a5c4:	eb002872 	bl	14794 <__aeabi_uidiv>
    a5c8:	e58d007c 	str	r0, [sp, #124]
    a5cc:	e5d43005 	ldrb	r3, [r4, #5]
    a5d0:	e3a06000 	mov	r6, #0	; 0x0
    a5d4:	e58d3080 	str	r3, [sp, #128]
    a5d8:	e5d43004 	ldrb	r3, [r4, #4]
    a5dc:	e1a0800a 	mov	r8, sl
    a5e0:	e58d3084 	str	r3, [sp, #132]
    a5e4:	e2843038 	add	r3, r4, #56	; 0x38
    a5e8:	e58d3064 	str	r3, [sp, #100]
    a5ec:	e284304c 	add	r3, r4, #76	; 0x4c
    a5f0:	e58d3068 	str	r3, [sp, #104]
    a5f4:	e284305c 	add	r3, r4, #92	; 0x5c
    a5f8:	e58d306c 	str	r3, [sp, #108]
    a5fc:	e284306c 	add	r3, r4, #108	; 0x6c
    a600:	e58d3070 	str	r3, [sp, #112]
    a604:	e284307c 	add	r3, r4, #124	; 0x7c
    a608:	e58d3074 	str	r3, [sp, #116]
    a60c:	e284308c 	add	r3, r4, #140	; 0x8c
    a610:	e58d3078 	str	r3, [sp, #120]
    a614:	e1a05006 	mov	r5, r6
    a618:	e1a0a006 	mov	sl, r6
    a61c:	e3a09001 	mov	r9, #1	; 0x1
    a620:	ea00004f 	b	a764 <base_vs_write+0x268>
    a624:	e35b0001 	cmp	fp, #1	; 0x1
    a628:	1a000012 	bne	a678 <base_vs_write+0x17c>
    a62c:	e3e03000 	mvn	r3, #0	; 0x0
    a630:	e5c73000 	strb	r3, [r7]
    a634:	e3a03000 	mov	r3, #0	; 0x0
    a638:	e5c73001 	strb	r3, [r7, #1]
    a63c:	e3e03000 	mvn	r3, #0	; 0x0
    a640:	e5c73002 	strb	r3, [r7, #2]
    a644:	e5c73003 	strb	r3, [r7, #3]
    a648:	e1a01007 	mov	r1, r7
    a64c:	e3a02000 	mov	r2, #0	; 0x0
    a650:	ea000004 	b	a668 <base_vs_write+0x16c>
    a654:	e5943014 	ldr	r3, [r4, #20]
    a658:	e0883003 	add	r3, r8, r3
    a65c:	e7d33002 	ldrb	r3, [r3, r2]
    a660:	e2822001 	add	r2, r2, #1	; 0x1
    a664:	e5c13003 	strb	r3, [r1, #3]
    a668:	e5943010 	ldr	r3, [r4, #16]
    a66c:	e2811001 	add	r1, r1, #1	; 0x1
    a670:	e1520003 	cmp	r2, r3
    a674:	3afffff6 	bcc	a654 <base_vs_write+0x158>
    a678:	e5943014 	ldr	r3, [r4, #20]
    a67c:	e5941010 	ldr	r1, [r4, #16]
    a680:	e5d40005 	ldrb	r0, [r4, #5]
    a684:	e5d42003 	ldrb	r2, [r4, #3]
    a688:	e58d3014 	str	r3, [sp, #20]
    a68c:	e58d3000 	str	r3, [sp]
    a690:	e59d3080 	ldr	r3, [sp, #128]
    a694:	e58dc00c 	str	ip, [sp, #12]
    a698:	e58d3018 	str	r3, [sp, #24]
    a69c:	e59d3084 	ldr	r3, [sp, #132]
    a6a0:	e58d1004 	str	r1, [sp, #4]
    a6a4:	e58d301c 	str	r3, [sp, #28]
    a6a8:	e3a03000 	mov	r3, #0	; 0x0
    a6ac:	e58d9008 	str	r9, [sp, #8]
    a6b0:	e58d8010 	str	r8, [sp, #16]
    a6b4:	e58d3020 	str	r3, [sp, #32]
    a6b8:	e58d9024 	str	r9, [sp, #36]
    a6bc:	e5d43002 	ldrb	r3, [r4, #2]
    a6c0:	e2811004 	add	r1, r1, #4	; 0x4
    a6c4:	e58d3028 	str	r3, [sp, #40]
    a6c8:	e5d43001 	ldrb	r3, [r4, #1]
    a6cc:	e58d1054 	str	r1, [sp, #84]
    a6d0:	e58d302c 	str	r3, [sp, #44]
    a6d4:	e59d3064 	ldr	r3, [sp, #100]
    a6d8:	e3a01000 	mov	r1, #0	; 0x0
    a6dc:	e58d3030 	str	r3, [sp, #48]
    a6e0:	e59d3068 	ldr	r3, [sp, #104]
    a6e4:	e58d3034 	str	r3, [sp, #52]
    a6e8:	e59d306c 	ldr	r3, [sp, #108]
    a6ec:	e58d3038 	str	r3, [sp, #56]
    a6f0:	e59d3070 	ldr	r3, [sp, #112]
    a6f4:	e58d303c 	str	r3, [sp, #60]
    a6f8:	e59d3074 	ldr	r3, [sp, #116]
    a6fc:	e58d3040 	str	r3, [sp, #64]
    a700:	e59d3078 	ldr	r3, [sp, #120]
    a704:	e58d3044 	str	r3, [sp, #68]
    a708:	e28d308c 	add	r3, sp, #140	; 0x8c
    a70c:	e58d3048 	str	r3, [sp, #72]
    a710:	e28d3088 	add	r3, sp, #136	; 0x88
    a714:	e58d304c 	str	r3, [sp, #76]
    a718:	e58d7050 	str	r7, [sp, #80]
    a71c:	e58d9058 	str	r9, [sp, #88]
    a720:	e5d4c006 	ldrb	ip, [r4, #6]
    a724:	e1a03009 	mov	r3, r9
    a728:	e58dc05c 	str	ip, [sp, #92]
    a72c:	eb0008d5 	bl	ca88 <nf2_page_op>
    a730:	e3500000 	cmp	r0, #0	; 0x0
    a734:	13e05000 	mvnne	r5, #0	; 0x0
    a738:	1a00000e 	bne	a778 <base_vs_write+0x27c>
    a73c:	e35b0000 	cmp	fp, #0	; 0x0
    a740:	e5942014 	ldr	r2, [r4, #20]
    a744:	15943010 	ldrne	r3, [r4, #16]
    a748:	008aa002 	addeq	sl, sl, r2
    a74c:	10833002 	addne	r3, r3, r2
    a750:	108aa003 	addne	sl, sl, r3
    a754:	00888002 	addeq	r8, r8, r2
    a758:	10888003 	addne	r8, r8, r3
    a75c:	e2866001 	add	r6, r6, #1	; 0x1
    a760:	e1a0500a 	mov	r5, sl
    a764:	e59d307c 	ldr	r3, [sp, #124]
    a768:	e1560003 	cmp	r6, r3
    a76c:	e59d3060 	ldr	r3, [sp, #96]
    a770:	e086c003 	add	ip, r6, r3
    a774:	baffffaa 	blt	a624 <base_vs_write+0x128>
    a778:	e1a00007 	mov	r0, r7
    a77c:	ebffdc43 	bl	1890 <free>
    a780:	e1a00005 	mov	r0, r5
    a784:	e28dd094 	add	sp, sp, #148	; 0x94
    a788:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    a78c:	000181e3 	.word	0x000181e3
    a790:	3c00d6cc 	.word	0x3c00d6cc
    a794:	3c00d734 	.word	0x3c00d734

0000a798 <fnd_vs_write>:
    a798:	e3a01001 	mov	r1, #1	; 0x1
    a79c:	e58d1004 	str	r1, [sp, #4]
    a7a0:	eaffff55 	b	a4fc <base_vs_write>

0000a7a4 <nnd_vs_write>:
    a7a4:	e3a01000 	mov	r1, #0	; 0x0
    a7a8:	e58d1004 	str	r1, [sp, #4]
    a7ac:	eaffff52 	b	a4fc <base_vs_write>

0000a7b0 <base_vs_read>:
    a7b0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    a7b4:	e3a01000 	mov	r1, #0	; 0x0
    a7b8:	e24dd094 	sub	sp, sp, #148	; 0x94
    a7bc:	e1a09000 	mov	r9, r0
    a7c0:	e3a00001 	mov	r0, #1	; 0x1
    a7c4:	e58d1088 	str	r1, [sp, #136]
    a7c8:	e1a06002 	mov	r6, r2
    a7cc:	e1a07003 	mov	r7, r3
    a7d0:	e58d108c 	str	r1, [sp, #140]
    a7d4:	e59db0bc 	ldr	fp, [sp, #188]
    a7d8:	ebfffdd7 	bl	9f3c <nand_get_config>
    a7dc:	e1a04000 	mov	r4, r0
    a7e0:	e5900010 	ldr	r0, [r0, #16]
    a7e4:	e2800004 	add	r0, r0, #4	; 0x4
    a7e8:	ebffdbc0 	bl	16f0 <malloc>
    a7ec:	e250a000 	subs	sl, r0, #0	; 0x0
    a7f0:	1a000007 	bne	a814 <base_vs_read+0x64>
    a7f4:	e5941010 	ldr	r1, [r4, #16]
    a7f8:	e59f0278 	ldr	r0, [pc, #632]	; aa78 <base_vs_read+0x2c8>
    a7fc:	e2811004 	add	r1, r1, #4	; 0x4
    a800:	e59f3274 	ldr	r3, [pc, #628]	; aa7c <base_vs_read+0x2cc>
    a804:	e1a0e00f 	mov	lr, pc
    a808:	e593f000 	ldr	pc, [r3]
    a80c:	e3e05000 	mvn	r5, #0	; 0x0
    a810:	ea000095 	b	aa6c <base_vs_read+0x2bc>
    a814:	e59f3264 	ldr	r3, [pc, #612]	; aa80 <base_vs_read+0x2d0>
    a818:	e35b0000 	cmp	fp, #0	; 0x0
    a81c:	e5932010 	ldr	r2, [r3, #16]
    a820:	e1a00006 	mov	r0, r6
    a824:	e1a01007 	mov	r1, r7
    a828:	01a0500b 	moveq	r5, fp
    a82c:	15945010 	ldrne	r5, [r4, #16]
    a830:	eb0028ee 	bl	14bf0 <__aeabi_lasr>
    a834:	e5948014 	ldr	r8, [r4, #20]
    a838:	e58d0060 	str	r0, [sp, #96]
    a83c:	e0851008 	add	r1, r5, r8
    a840:	e59d00b8 	ldr	r0, [sp, #184]
    a844:	eb0027d2 	bl	14794 <__aeabi_uidiv>
    a848:	e58d007c 	str	r0, [sp, #124]
    a84c:	e5d43005 	ldrb	r3, [r4, #5]
    a850:	e3a07000 	mov	r7, #0	; 0x0
    a854:	e58d3080 	str	r3, [sp, #128]
    a858:	e5d43004 	ldrb	r3, [r4, #4]
    a85c:	e1a06009 	mov	r6, r9
    a860:	e58d3084 	str	r3, [sp, #132]
    a864:	e2843038 	add	r3, r4, #56	; 0x38
    a868:	e58d3064 	str	r3, [sp, #100]
    a86c:	e284304c 	add	r3, r4, #76	; 0x4c
    a870:	e58d3068 	str	r3, [sp, #104]
    a874:	e284305c 	add	r3, r4, #92	; 0x5c
    a878:	e58d306c 	str	r3, [sp, #108]
    a87c:	e284306c 	add	r3, r4, #108	; 0x6c
    a880:	e58d3070 	str	r3, [sp, #112]
    a884:	e284307c 	add	r3, r4, #124	; 0x7c
    a888:	e58d3074 	str	r3, [sp, #116]
    a88c:	e284308c 	add	r3, r4, #140	; 0x8c
    a890:	e58d3078 	str	r3, [sp, #120]
    a894:	e1a05007 	mov	r5, r7
    a898:	e1a08007 	mov	r8, r7
    a89c:	e3a09001 	mov	r9, #1	; 0x1
    a8a0:	ea00006a 	b	aa50 <base_vs_read+0x2a0>
    a8a4:	e5943014 	ldr	r3, [r4, #20]
    a8a8:	e5941010 	ldr	r1, [r4, #16]
    a8ac:	e5d40005 	ldrb	r0, [r4, #5]
    a8b0:	e5d42003 	ldrb	r2, [r4, #3]
    a8b4:	e58d3014 	str	r3, [sp, #20]
    a8b8:	e58d3000 	str	r3, [sp]
    a8bc:	e3a03000 	mov	r3, #0	; 0x0
    a8c0:	e58d3008 	str	r3, [sp, #8]
    a8c4:	e59d3080 	ldr	r3, [sp, #128]
    a8c8:	e58dc00c 	str	ip, [sp, #12]
    a8cc:	e58d3018 	str	r3, [sp, #24]
    a8d0:	e59d3084 	ldr	r3, [sp, #132]
    a8d4:	e58d1004 	str	r1, [sp, #4]
    a8d8:	e58d301c 	str	r3, [sp, #28]
    a8dc:	e3a03000 	mov	r3, #0	; 0x0
    a8e0:	e58d6010 	str	r6, [sp, #16]
    a8e4:	e58d3020 	str	r3, [sp, #32]
    a8e8:	e58d9024 	str	r9, [sp, #36]
    a8ec:	e5d43002 	ldrb	r3, [r4, #2]
    a8f0:	e2811004 	add	r1, r1, #4	; 0x4
    a8f4:	e58d3028 	str	r3, [sp, #40]
    a8f8:	e5d43001 	ldrb	r3, [r4, #1]
    a8fc:	e58d1054 	str	r1, [sp, #84]
    a900:	e58d302c 	str	r3, [sp, #44]
    a904:	e59d3064 	ldr	r3, [sp, #100]
    a908:	e3a01000 	mov	r1, #0	; 0x0
    a90c:	e58d3030 	str	r3, [sp, #48]
    a910:	e59d3068 	ldr	r3, [sp, #104]
    a914:	e58d3034 	str	r3, [sp, #52]
    a918:	e59d306c 	ldr	r3, [sp, #108]
    a91c:	e58d3038 	str	r3, [sp, #56]
    a920:	e59d3070 	ldr	r3, [sp, #112]
    a924:	e58d303c 	str	r3, [sp, #60]
    a928:	e59d3074 	ldr	r3, [sp, #116]
    a92c:	e58d3040 	str	r3, [sp, #64]
    a930:	e59d3078 	ldr	r3, [sp, #120]
    a934:	e58d3044 	str	r3, [sp, #68]
    a938:	e28d308c 	add	r3, sp, #140	; 0x8c
    a93c:	e58d3048 	str	r3, [sp, #72]
    a940:	e28d3088 	add	r3, sp, #136	; 0x88
    a944:	e58d304c 	str	r3, [sp, #76]
    a948:	e58da050 	str	sl, [sp, #80]
    a94c:	e58d9058 	str	r9, [sp, #88]
    a950:	e5d4c006 	ldrb	ip, [r4, #6]
    a954:	e1a03009 	mov	r3, r9
    a958:	e58dc05c 	str	ip, [sp, #92]
    a95c:	eb000849 	bl	ca88 <nf2_page_op>
    a960:	e3500000 	cmp	r0, #0	; 0x0
    a964:	1a00001d 	bne	a9e0 <base_vs_read+0x230>
    a968:	e5da1001 	ldrb	r1, [sl, #1]
    a96c:	e35100ff 	cmp	r1, #255	; 0xff
    a970:	1a00000b 	bne	a9a4 <base_vs_read+0x1f4>
    a974:	e35b0000 	cmp	fp, #0	; 0x0
    a978:	1a000003 	bne	a98c <base_vs_read+0x1dc>
    a97c:	e1a00006 	mov	r0, r6
    a980:	e5942014 	ldr	r2, [r4, #20]
    a984:	ebffdaec 	bl	153c <memset>
    a988:	ea000027 	b	aa2c <base_vs_read+0x27c>
    a98c:	e5943010 	ldr	r3, [r4, #16]
    a990:	e5942014 	ldr	r2, [r4, #20]
    a994:	e1a00006 	mov	r0, r6
    a998:	e0832002 	add	r2, r3, r2
    a99c:	ebffdae6 	bl	153c <memset>
    a9a0:	ea000010 	b	a9e8 <base_vs_read+0x238>
    a9a4:	e59d108c 	ldr	r1, [sp, #140]
    a9a8:	e3510000 	cmp	r1, #0	; 0x0
    a9ac:	1a000002 	bne	a9bc <base_vs_read+0x20c>
    a9b0:	e59d3088 	ldr	r3, [sp, #136]
    a9b4:	e3530000 	cmp	r3, #0	; 0x0
    a9b8:	0a00000a 	beq	a9e8 <base_vs_read+0x238>
    a9bc:	e59d2088 	ldr	r2, [sp, #136]
    a9c0:	e2511000 	subs	r1, r1, #0	; 0x0
    a9c4:	13a01001 	movne	r1, #1	; 0x1
    a9c8:	e2522000 	subs	r2, r2, #0	; 0x0
    a9cc:	13a02001 	movne	r2, #1	; 0x1
    a9d0:	e59f00ac 	ldr	r0, [pc, #172]	; aa84 <base_vs_read+0x2d4>
    a9d4:	e59f30a0 	ldr	r3, [pc, #160]	; aa7c <base_vs_read+0x2cc>
    a9d8:	e1a0e00f 	mov	lr, pc
    a9dc:	e593f000 	ldr	pc, [r3]
    a9e0:	e3e05000 	mvn	r5, #0	; 0x0
    a9e4:	ea00001e 	b	aa64 <base_vs_read+0x2b4>
    a9e8:	e35b0001 	cmp	fp, #1	; 0x1
    a9ec:	01a0000a 	moveq	r0, sl
    a9f0:	03a01000 	moveq	r1, #0	; 0x0
    a9f4:	1a00000a 	bne	aa24 <base_vs_read+0x274>
    a9f8:	ea000004 	b	aa10 <base_vs_read+0x260>
    a9fc:	e5943014 	ldr	r3, [r4, #20]
    aa00:	e5d02003 	ldrb	r2, [r0, #3]
    aa04:	e0863003 	add	r3, r6, r3
    aa08:	e7c32001 	strb	r2, [r3, r1]
    aa0c:	e2811001 	add	r1, r1, #1	; 0x1
    aa10:	e5943010 	ldr	r3, [r4, #16]
    aa14:	e2800001 	add	r0, r0, #1	; 0x1
    aa18:	e1510003 	cmp	r1, r3
    aa1c:	3afffff6 	bcc	a9fc <base_vs_read+0x24c>
    aa20:	ea000003 	b	aa34 <base_vs_read+0x284>
    aa24:	e35b0000 	cmp	fp, #0	; 0x0
    aa28:	1a000001 	bne	aa34 <base_vs_read+0x284>
    aa2c:	e5943014 	ldr	r3, [r4, #20]
    aa30:	ea000002 	b	aa40 <base_vs_read+0x290>
    aa34:	e5943010 	ldr	r3, [r4, #16]
    aa38:	e5942014 	ldr	r2, [r4, #20]
    aa3c:	e0833002 	add	r3, r3, r2
    aa40:	e0888003 	add	r8, r8, r3
    aa44:	e0866003 	add	r6, r6, r3
    aa48:	e2877001 	add	r7, r7, #1	; 0x1
    aa4c:	e1a05008 	mov	r5, r8
    aa50:	e59d307c 	ldr	r3, [sp, #124]
    aa54:	e1570003 	cmp	r7, r3
    aa58:	e59d3060 	ldr	r3, [sp, #96]
    aa5c:	e087c003 	add	ip, r7, r3
    aa60:	baffff8f 	blt	a8a4 <base_vs_read+0xf4>
    aa64:	e1a0000a 	mov	r0, sl
    aa68:	ebffdb88 	bl	1890 <free>
    aa6c:	e1a00005 	mov	r0, r5
    aa70:	e28dd094 	add	sp, sp, #148	; 0x94
    aa74:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    aa78:	00018201 	.word	0x00018201
    aa7c:	3c00d6cc 	.word	0x3c00d6cc
    aa80:	3c00d734 	.word	0x3c00d734
    aa84:	0001821e 	.word	0x0001821e

0000aa88 <fnd_vs_read>:
    aa88:	e3a01001 	mov	r1, #1	; 0x1
    aa8c:	e58d1004 	str	r1, [sp, #4]
    aa90:	eaffff46 	b	a7b0 <base_vs_read>

0000aa94 <nnd_vs_read>:
    aa94:	e3a01000 	mov	r1, #0	; 0x0
    aa98:	e58d1004 	str	r1, [sp, #4]
    aa9c:	eaffff43 	b	a7b0 <base_vs_read>

0000aaa0 <base_get_badmark>:
    aaa0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    aaa4:	e24dd07c 	sub	sp, sp, #124	; 0x7c
    aaa8:	e58d0064 	str	r0, [sp, #100]
    aaac:	e3a05000 	mov	r5, #0	; 0x0
    aab0:	e3a00001 	mov	r0, #1	; 0x1
    aab4:	e58d5074 	str	r5, [sp, #116]
    aab8:	e58d5070 	str	r5, [sp, #112]
    aabc:	ebfffd1e 	bl	9f3c <nand_get_config>
    aac0:	e2504000 	subs	r4, r0, #0	; 0x0
    aac4:	0a000008 	beq	aaec <base_get_badmark+0x4c>
    aac8:	e5940010 	ldr	r0, [r4, #16]
    aacc:	e2800004 	add	r0, r0, #4	; 0x4
    aad0:	ebffdb06 	bl	16f0 <malloc>
    aad4:	e2506000 	subs	r6, r0, #0	; 0x0
    aad8:	1a000005 	bne	aaf4 <base_get_badmark+0x54>
    aadc:	e59f01c4 	ldr	r0, [pc, #452]	; aca8 <base_get_badmark+0x208>
    aae0:	e59f31c4 	ldr	r3, [pc, #452]	; acac <base_get_badmark+0x20c>
    aae4:	e1a0e00f 	mov	lr, pc
    aae8:	e593f000 	ldr	pc, [r3]
    aaec:	e3e05000 	mvn	r5, #0	; 0x0
    aaf0:	ea000069 	b	ac9c <base_get_badmark+0x1fc>
    aaf4:	e5941010 	ldr	r1, [r4, #16]
    aaf8:	e5d42003 	ldrb	r2, [r4, #3]
    aafc:	e5943014 	ldr	r3, [r4, #20]
    ab00:	e58d1014 	str	r1, [sp, #20]
    ab04:	e58d1004 	str	r1, [sp, #4]
    ab08:	e59d1064 	ldr	r1, [sp, #100]
    ab0c:	e3a07001 	mov	r7, #1	; 0x1
    ab10:	e58d5008 	str	r5, [sp, #8]
    ab14:	e58d100c 	str	r1, [sp, #12]
    ab18:	e58d5018 	str	r5, [sp, #24]
    ab1c:	e58d5020 	str	r5, [sp, #32]
    ab20:	e58d5024 	str	r5, [sp, #36]
    ab24:	e58d3000 	str	r3, [sp]
    ab28:	e58d6010 	str	r6, [sp, #16]
    ab2c:	e58d701c 	str	r7, [sp, #28]
    ab30:	e5d43002 	ldrb	r3, [r4, #2]
    ab34:	e2841038 	add	r1, r4, #56	; 0x38
    ab38:	e58d1068 	str	r1, [sp, #104]
    ab3c:	e58d3028 	str	r3, [sp, #40]
    ab40:	e59d3068 	ldr	r3, [sp, #104]
    ab44:	e284104c 	add	r1, r4, #76	; 0x4c
    ab48:	e58d106c 	str	r1, [sp, #108]
    ab4c:	e58d3030 	str	r3, [sp, #48]
    ab50:	e58d1034 	str	r1, [sp, #52]
    ab54:	e28d3070 	add	r3, sp, #112	; 0x70
    ab58:	e28d1074 	add	r1, sp, #116	; 0x74
    ab5c:	e284b05c 	add	fp, r4, #92	; 0x5c
    ab60:	e284906c 	add	r9, r4, #108	; 0x6c
    ab64:	e284a07c 	add	sl, r4, #124	; 0x7c
    ab68:	e284808c 	add	r8, r4, #140	; 0x8c
    ab6c:	e58d502c 	str	r5, [sp, #44]
    ab70:	e58d1048 	str	r1, [sp, #72]
    ab74:	e58d304c 	str	r3, [sp, #76]
    ab78:	e58d5050 	str	r5, [sp, #80]
    ab7c:	e58d5054 	str	r5, [sp, #84]
    ab80:	e58d5058 	str	r5, [sp, #88]
    ab84:	e58db038 	str	fp, [sp, #56]
    ab88:	e58d903c 	str	r9, [sp, #60]
    ab8c:	e58da040 	str	sl, [sp, #64]
    ab90:	e58d8044 	str	r8, [sp, #68]
    ab94:	e5d4c006 	ldrb	ip, [r4, #6]
    ab98:	e1a00005 	mov	r0, r5
    ab9c:	e1a01007 	mov	r1, r7
    aba0:	e1a03007 	mov	r3, r7
    aba4:	e58dc05c 	str	ip, [sp, #92]
    aba8:	eb0007b6 	bl	ca88 <nf2_page_op>
    abac:	e2505000 	subs	r5, r0, #0	; 0x0
    abb0:	0a000004 	beq	abc8 <base_get_badmark+0x128>
    abb4:	e59f00f4 	ldr	r0, [pc, #244]	; acb0 <base_get_badmark+0x210>
    abb8:	e59f30ec 	ldr	r3, [pc, #236]	; acac <base_get_badmark+0x20c>
    abbc:	e1a0e00f 	mov	lr, pc
    abc0:	e593f000 	ldr	pc, [r3]
    abc4:	ea000032 	b	ac94 <base_get_badmark+0x1f4>
    abc8:	e5d63000 	ldrb	r3, [r6]
    abcc:	e35300ff 	cmp	r3, #255	; 0xff
    abd0:	0a00002f 	beq	ac94 <base_get_badmark+0x1f4>
    abd4:	e5943014 	ldr	r3, [r4, #20]
    abd8:	e5d40005 	ldrb	r0, [r4, #5]
    abdc:	e5941010 	ldr	r1, [r4, #16]
    abe0:	e5d42003 	ldrb	r2, [r4, #3]
    abe4:	e58d3000 	str	r3, [sp]
    abe8:	e59d3064 	ldr	r3, [sp, #100]
    abec:	e58d1014 	str	r1, [sp, #20]
    abf0:	e98d0022 	stmib	sp, {r1, r5}
    abf4:	e58d0018 	str	r0, [sp, #24]
    abf8:	e58d300c 	str	r3, [sp, #12]
    abfc:	e58d6010 	str	r6, [sp, #16]
    ac00:	e5d43004 	ldrb	r3, [r4, #4]
    ac04:	e58d5020 	str	r5, [sp, #32]
    ac08:	e58d5024 	str	r5, [sp, #36]
    ac0c:	e58d301c 	str	r3, [sp, #28]
    ac10:	e5d43002 	ldrb	r3, [r4, #2]
    ac14:	e1a01007 	mov	r1, r7
    ac18:	e58d3028 	str	r3, [sp, #40]
    ac1c:	e5d43001 	ldrb	r3, [r4, #1]
    ac20:	e58d5058 	str	r5, [sp, #88]
    ac24:	e58d302c 	str	r3, [sp, #44]
    ac28:	e59d3068 	ldr	r3, [sp, #104]
    ac2c:	e58d5050 	str	r5, [sp, #80]
    ac30:	e58d3030 	str	r3, [sp, #48]
    ac34:	e59d306c 	ldr	r3, [sp, #108]
    ac38:	e58d5054 	str	r5, [sp, #84]
    ac3c:	e58d3034 	str	r3, [sp, #52]
    ac40:	e28d3074 	add	r3, sp, #116	; 0x74
    ac44:	e58d3048 	str	r3, [sp, #72]
    ac48:	e28d3070 	add	r3, sp, #112	; 0x70
    ac4c:	e58d304c 	str	r3, [sp, #76]
    ac50:	e58db038 	str	fp, [sp, #56]
    ac54:	e58d903c 	str	r9, [sp, #60]
    ac58:	e58da040 	str	sl, [sp, #64]
    ac5c:	e58d8044 	str	r8, [sp, #68]
    ac60:	e5d4c006 	ldrb	ip, [r4, #6]
    ac64:	e1a03007 	mov	r3, r7
    ac68:	e58dc05c 	str	ip, [sp, #92]
    ac6c:	eb000785 	bl	ca88 <nf2_page_op>
    ac70:	e2505000 	subs	r5, r0, #0	; 0x0
    ac74:	1a000006 	bne	ac94 <base_get_badmark+0x1f4>
    ac78:	e59d3070 	ldr	r3, [sp, #112]
    ac7c:	e3530000 	cmp	r3, #0	; 0x0
    ac80:	1a000002 	bne	ac90 <base_get_badmark+0x1f0>
    ac84:	e5d63000 	ldrb	r3, [r6]
    ac88:	e35300ff 	cmp	r3, #255	; 0xff
    ac8c:	0a000000 	beq	ac94 <base_get_badmark+0x1f4>
    ac90:	e3a05001 	mov	r5, #1	; 0x1
    ac94:	e1a00006 	mov	r0, r6
    ac98:	ebffdafc 	bl	1890 <free>
    ac9c:	e1a00005 	mov	r0, r5
    aca0:	e28dd07c 	add	sp, sp, #124	; 0x7c
    aca4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    aca8:	00018232 	.word	0x00018232
    acac:	3c00d6cc 	.word	0x3c00d6cc
    acb0:	00018259 	.word	0x00018259

0000acb4 <nnd_vs_isbad>:
    acb4:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
    acb8:	e1a08000 	mov	r8, r0
    acbc:	e3a00001 	mov	r0, #1	; 0x1
    acc0:	e1a09001 	mov	r9, r1
    acc4:	ebfffc9c 	bl	9f3c <nand_get_config>
    acc8:	e2506000 	subs	r6, r0, #0	; 0x0
    accc:	0a00001f 	beq	ad50 <nnd_vs_isbad+0x9c>
    acd0:	e596301c 	ldr	r3, [r6, #28]
    acd4:	e1a01009 	mov	r1, r9
    acd8:	e2433001 	sub	r3, r3, #1	; 0x1
    acdc:	e1c84003 	bic	r4, r8, r3
    ace0:	e59f307c 	ldr	r3, [pc, #124]	; ad64 <nnd_vs_isbad+0xb0>
    ace4:	e1a00004 	mov	r0, r4
    ace8:	e5932010 	ldr	r2, [r3, #16]
    acec:	eb0027bf 	bl	14bf0 <__aeabi_lasr>
    acf0:	e1a08000 	mov	r8, r0
    acf4:	e5960008 	ldr	r0, [r6, #8]
    acf8:	e1a05009 	mov	r5, r9
    acfc:	e0880000 	add	r0, r8, r0
    ad00:	ebffff66 	bl	aaa0 <base_get_badmark>
    ad04:	e2502000 	subs	r2, r0, #0	; 0x0
    ad08:	ba00000a 	blt	ad38 <nnd_vs_isbad+0x84>
    ad0c:	13a00001 	movne	r0, #1	; 0x1
    ad10:	18bd8370 	popne	{r4, r5, r6, r8, r9, pc}
    ad14:	e596000c 	ldr	r0, [r6, #12]
    ad18:	e5963008 	ldr	r3, [r6, #8]
    ad1c:	e1500003 	cmp	r0, r3
    ad20:	01a00002 	moveq	r0, r2
    ad24:	08bd8370 	popeq	{r4, r5, r6, r8, r9, pc}
    ad28:	e0880000 	add	r0, r8, r0
    ad2c:	ebffff5b 	bl	aaa0 <base_get_badmark>
    ad30:	e3500000 	cmp	r0, #0	; 0x0
    ad34:	aa000007 	bge	ad58 <nnd_vs_isbad+0xa4>
    ad38:	e1a02004 	mov	r2, r4
    ad3c:	e1a03005 	mov	r3, r5
    ad40:	e59f0020 	ldr	r0, [pc, #32]	; ad68 <nnd_vs_isbad+0xb4>
    ad44:	e59f1020 	ldr	r1, [pc, #32]	; ad6c <nnd_vs_isbad+0xb8>
    ad48:	e1a0e00f 	mov	lr, pc
    ad4c:	e591f000 	ldr	pc, [r1]
    ad50:	e3e00000 	mvn	r0, #0	; 0x0
    ad54:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    ad58:	03a00000 	moveq	r0, #0	; 0x0
    ad5c:	13a00001 	movne	r0, #1	; 0x1
    ad60:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
    ad64:	3c00d734 	.word	0x3c00d734
    ad68:	00018272 	.word	0x00018272
    ad6c:	3c00d6cc 	.word	0x3c00d6cc

0000ad70 <base_scan_badblocks>:
    ad70:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ad74:	e1a0b000 	mov	fp, r0
    ad78:	e24dd004 	sub	sp, sp, #4	; 0x4
    ad7c:	e3a00001 	mov	r0, #1	; 0x1
    ad80:	ebfffc6d 	bl	9f3c <nand_get_config>
    ad84:	e250a000 	subs	sl, r0, #0	; 0x0
    ad88:	0a000005 	beq	ada4 <base_scan_badblocks+0x34>
    ad8c:	e3a07000 	mov	r7, #0	; 0x0
    ad90:	e1a08007 	mov	r8, r7
    ad94:	e3a04000 	mov	r4, #0	; 0x0
    ad98:	e3a05000 	mov	r5, #0	; 0x0
    ad9c:	e59f90b8 	ldr	r9, [pc, #184]	; ae5c <base_scan_badblocks+0xec>
    ada0:	ea00001a 	b	ae10 <base_scan_badblocks+0xa0>
    ada4:	e59f00b4 	ldr	r0, [pc, #180]	; ae60 <base_scan_badblocks+0xf0>
    ada8:	e59f30ac 	ldr	r3, [pc, #172]	; ae5c <base_scan_badblocks+0xec>
    adac:	e1a0e00f 	mov	lr, pc
    adb0:	e593f000 	ldr	pc, [r3]
    adb4:	e3e00000 	mvn	r0, #0	; 0x0
    adb8:	ea000025 	b	ae54 <base_scan_badblocks+0xe4>
    adbc:	ebffffbc 	bl	acb4 <nnd_vs_isbad>
    adc0:	e3500000 	cmp	r0, #0	; 0x0
    adc4:	0a00000d 	beq	ae00 <base_scan_badblocks+0x90>
    adc8:	aa000004 	bge	ade0 <base_scan_badblocks+0x70>
    adcc:	e59f0090 	ldr	r0, [pc, #144]	; ae64 <base_scan_badblocks+0xf4>
    add0:	e1a01007 	mov	r1, r7
    add4:	e1a0e00f 	mov	lr, pc
    add8:	e599f000 	ldr	pc, [r9]
    addc:	ea000006 	b	adfc <base_scan_badblocks+0x8c>
    ade0:	e35b0000 	cmp	fp, #0	; 0x0
    ade4:	0a000004 	beq	adfc <base_scan_badblocks+0x8c>
    ade8:	e1a02004 	mov	r2, r4
    adec:	e1a03005 	mov	r3, r5
    adf0:	e59f0070 	ldr	r0, [pc, #112]	; ae68 <base_scan_badblocks+0xf8>
    adf4:	e1a0e00f 	mov	lr, pc
    adf8:	e599f000 	ldr	pc, [r9]
    adfc:	e2888001 	add	r8, r8, #1	; 0x1
    ae00:	e59a301c 	ldr	r3, [sl, #28]
    ae04:	e2877001 	add	r7, r7, #1	; 0x1
    ae08:	e0944003 	adds	r4, r4, r3
    ae0c:	e2a55000 	adc	r5, r5, #0	; 0x0
    ae10:	e59a6024 	ldr	r6, [sl, #36]
    ae14:	e1a00004 	mov	r0, r4
    ae18:	e1570006 	cmp	r7, r6
    ae1c:	e1a01005 	mov	r1, r5
    ae20:	3affffe5 	bcc	adbc <base_scan_badblocks+0x4c>
    ae24:	e3a00064 	mov	r0, #100	; 0x64
    ae28:	e1a01006 	mov	r1, r6
    ae2c:	e0000890 	mul	r0, r0, r8
    ae30:	eb002657 	bl	14794 <__aeabi_uidiv>
    ae34:	e1a01008 	mov	r1, r8
    ae38:	e1a03000 	mov	r3, r0
    ae3c:	e1a02006 	mov	r2, r6
    ae40:	e59f0024 	ldr	r0, [pc, #36]	; ae6c <base_scan_badblocks+0xfc>
    ae44:	e59fc010 	ldr	ip, [pc, #16]	; ae5c <base_scan_badblocks+0xec>
    ae48:	e1a0e00f 	mov	lr, pc
    ae4c:	e59cf000 	ldr	pc, [ip]
    ae50:	e3a00000 	mov	r0, #0	; 0x0
    ae54:	e28dd004 	add	sp, sp, #4	; 0x4
    ae58:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    ae5c:	3c00d6cc 	.word	0x3c00d6cc
    ae60:	00018290 	.word	0x00018290
    ae64:	000182ac 	.word	0x000182ac
    ae68:	00016797 	.word	0x00016797
    ae6c:	000182c6 	.word	0x000182c6

0000ae70 <fnd_vs_isbad>:
    ae70:	eaffff8f 	b	acb4 <nnd_vs_isbad>

0000ae74 <bnd_vs_isbad>:
    ae74:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ae78:	e59f3014 	ldr	r3, [pc, #20]	; ae94 <bnd_vs_isbad+0x20>
    ae7c:	e24dd004 	sub	sp, sp, #4	; 0x4
    ae80:	e5932008 	ldr	r2, [r3, #8]
    ae84:	eb002760 	bl	14c0c <__aeabi_llsl>
    ae88:	e28dd004 	add	sp, sp, #4	; 0x4
    ae8c:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
    ae90:	eaffff87 	b	acb4 <nnd_vs_isbad>
    ae94:	3c00d734 	.word	0x3c00d734

0000ae98 <nnd_vs_reset>:
    ae98:	e92d4030 	push	{r4, r5, lr}
    ae9c:	e3a00001 	mov	r0, #1	; 0x1
    aea0:	e24dd004 	sub	sp, sp, #4	; 0x4
    aea4:	ebfffc24 	bl	9f3c <nand_get_config>
    aea8:	e2504000 	subs	r4, r0, #0	; 0x0
    aeac:	0a000014 	beq	af04 <nnd_vs_reset+0x6c>
    aeb0:	e3a01001 	mov	r1, #1	; 0x1
    aeb4:	eb000a2b 	bl	d768 <nf2_hardware_reset>
    aeb8:	e3500000 	cmp	r0, #0	; 0x0
    aebc:	0a000011 	beq	af08 <nnd_vs_reset+0x70>
    aec0:	e5d41000 	ldrb	r1, [r4]
    aec4:	e59f5044 	ldr	r5, [pc, #68]	; af10 <nnd_vs_reset+0x78>
    aec8:	e3510001 	cmp	r1, #1	; 0x1
    aecc:	1a000009 	bne	aef8 <nnd_vs_reset+0x60>
    aed0:	e59f003c 	ldr	r0, [pc, #60]	; af14 <nnd_vs_reset+0x7c>
    aed4:	e1a0e00f 	mov	lr, pc
    aed8:	e595f000 	ldr	pc, [r5]
    aedc:	e3a01000 	mov	r1, #0	; 0x0
    aee0:	e5c41000 	strb	r1, [r4]
    aee4:	e1a00004 	mov	r0, r4
    aee8:	eb000a1e 	bl	d768 <nf2_hardware_reset>
    aeec:	e3500000 	cmp	r0, #0	; 0x0
    aef0:	0a000004 	beq	af08 <nnd_vs_reset+0x70>
    aef4:	e5d41000 	ldrb	r1, [r4]
    aef8:	e59f0018 	ldr	r0, [pc, #24]	; af18 <nnd_vs_reset+0x80>
    aefc:	e1a0e00f 	mov	lr, pc
    af00:	e595f000 	ldr	pc, [r5]
    af04:	e3e00003 	mvn	r0, #3	; 0x3
    af08:	e28dd004 	add	sp, sp, #4	; 0x4
    af0c:	e8bd8030 	pop	{r4, r5, pc}
    af10:	3c00d6cc 	.word	0x3c00d6cc
    af14:	00018307 	.word	0x00018307
    af18:	000182e5 	.word	0x000182e5

0000af1c <fnd_vs_reset>:
    af1c:	eaffffdd 	b	ae98 <nnd_vs_reset>

0000af20 <base_vs_erase>:
    af20:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    af24:	e1a04000 	mov	r4, r0
    af28:	e24dd00c 	sub	sp, sp, #12	; 0xc
    af2c:	e3a00001 	mov	r0, #1	; 0x1
    af30:	e1a05001 	mov	r5, r1
    af34:	e1a06002 	mov	r6, r2
    af38:	e58d3004 	str	r3, [sp, #4]
    af3c:	ebfffbfe 	bl	9f3c <nand_get_config>
    af40:	e2507000 	subs	r7, r0, #0	; 0x0
    af44:	0a000047 	beq	b068 <base_vs_erase+0x148>
    af48:	e597201c 	ldr	r2, [r7, #28]
    af4c:	e242c001 	sub	ip, r2, #1	; 0x1
    af50:	e00c3004 	and	r3, ip, r4
    af54:	e3530000 	cmp	r3, #0	; 0x0
    af58:	e0836006 	add	r6, r3, r6
    af5c:	1a000001 	bne	af68 <base_vs_erase+0x48>
    af60:	e116000c 	tst	r6, ip
    af64:	0a00000a 	beq	af94 <base_vs_erase+0x74>
    af68:	e08c3006 	add	r3, ip, r6
    af6c:	e2622000 	rsb	r2, r2, #0	; 0x0
    af70:	e0036002 	and	r6, r3, r2
    af74:	e1c4400c 	bic	r4, r4, ip
    af78:	e59f0108 	ldr	r0, [pc, #264]	; b088 <base_vs_erase+0x168>
    af7c:	e1a01006 	mov	r1, r6
    af80:	e1a02004 	mov	r2, r4
    af84:	e1a03005 	mov	r3, r5
    af88:	e59fc0fc 	ldr	ip, [pc, #252]	; b08c <base_vs_erase+0x16c>
    af8c:	e1a0e00f 	mov	lr, pc
    af90:	e59cf000 	ldr	pc, [ip]
    af94:	e59f30f4 	ldr	r3, [pc, #244]	; b090 <base_vs_erase+0x170>
    af98:	e1a00004 	mov	r0, r4
    af9c:	e5932010 	ldr	r2, [r3, #16]
    afa0:	e1a01005 	mov	r1, r5
    afa4:	eb002711 	bl	14bf0 <__aeabi_lasr>
    afa8:	e597101c 	ldr	r1, [r7, #28]
    afac:	e1a08000 	mov	r8, r0
    afb0:	e1a00006 	mov	r0, r6
    afb4:	eb0025f6 	bl	14794 <__aeabi_uidiv>
    afb8:	e3a09000 	mov	r9, #0	; 0x0
    afbc:	e1a0b000 	mov	fp, r0
    afc0:	e1a06009 	mov	r6, r9
    afc4:	ea000022 	b	b054 <base_vs_erase+0x134>
    afc8:	e59d3004 	ldr	r3, [sp, #4]
    afcc:	e3530000 	cmp	r3, #0	; 0x0
    afd0:	1a000002 	bne	afe0 <base_vs_erase+0xc0>
    afd4:	ebffff36 	bl	acb4 <nnd_vs_isbad>
    afd8:	e3500000 	cmp	r0, #0	; 0x0
    afdc:	1a000010 	bne	b024 <base_vs_erase+0x104>
    afe0:	e1a01008 	mov	r1, r8
    afe4:	e5d72002 	ldrb	r2, [r7, #2]
    afe8:	e3a00000 	mov	r0, #0	; 0x0
    afec:	eb000662 	bl	c97c <nf2_erase>
    aff0:	e250a000 	subs	sl, r0, #0	; 0x0
    aff4:	0a000010 	beq	b03c <base_vs_erase+0x11c>
    aff8:	e5971020 	ldr	r1, [r7, #32]
    affc:	e1a00008 	mov	r0, r8
    b000:	eb0025e3 	bl	14794 <__aeabi_uidiv>
    b004:	e1a0200a 	mov	r2, sl
    b008:	e1a01000 	mov	r1, r0
    b00c:	e59f3078 	ldr	r3, [pc, #120]	; b08c <base_vs_erase+0x16c>
    b010:	e59f007c 	ldr	r0, [pc, #124]	; b094 <base_vs_erase+0x174>
    b014:	e1a0e00f 	mov	lr, pc
    b018:	e593f000 	ldr	pc, [r3]
    b01c:	e3e06000 	mvn	r6, #0	; 0x0
    b020:	ea000005 	b	b03c <base_vs_erase+0x11c>
    b024:	e5971014 	ldr	r1, [r7, #20]
    b028:	e59f0068 	ldr	r0, [pc, #104]	; b098 <base_vs_erase+0x178>
    b02c:	e0010891 	mul	r1, r1, r8
    b030:	e59f3054 	ldr	r3, [pc, #84]	; b08c <base_vs_erase+0x16c>
    b034:	e1a0e00f 	mov	lr, pc
    b038:	e593f000 	ldr	pc, [r3]
    b03c:	e597201c 	ldr	r2, [r7, #28]
    b040:	e5973020 	ldr	r3, [r7, #32]
    b044:	e0944002 	adds	r4, r4, r2
    b048:	e0888003 	add	r8, r8, r3
    b04c:	e2a55000 	adc	r5, r5, #0	; 0x0
    b050:	e2899001 	add	r9, r9, #1	; 0x1
    b054:	e159000b 	cmp	r9, fp
    b058:	e1a00004 	mov	r0, r4
    b05c:	e1a01005 	mov	r1, r5
    b060:	baffffd8 	blt	afc8 <base_vs_erase+0xa8>
    b064:	ea000004 	b	b07c <base_vs_erase+0x15c>
    b068:	e59f002c 	ldr	r0, [pc, #44]	; b09c <base_vs_erase+0x17c>
    b06c:	e59f3018 	ldr	r3, [pc, #24]	; b08c <base_vs_erase+0x16c>
    b070:	e1a0e00f 	mov	lr, pc
    b074:	e593f000 	ldr	pc, [r3]
    b078:	e3e06000 	mvn	r6, #0	; 0x0
    b07c:	e1a00006 	mov	r0, r6
    b080:	e28dd00c 	add	sp, sp, #12	; 0xc
    b084:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b088:	00018327 	.word	0x00018327
    b08c:	3c00d6cc 	.word	0x3c00d6cc
    b090:	3c00d734 	.word	0x3c00d734
    b094:	0001834e 	.word	0x0001834e
    b098:	0001836e 	.word	0x0001836e
    b09c:	00018384 	.word	0x00018384

0000b0a0 <nnd_vs_scrub>:
    b0a0:	e3a03001 	mov	r3, #1	; 0x1
    b0a4:	eaffff9d 	b	af20 <base_vs_erase>

0000b0a8 <bnd_vs_erase>:
    b0a8:	e92d4030 	push	{r4, r5, lr}
    b0ac:	e59f3020 	ldr	r3, [pc, #32]	; b0d4 <bnd_vs_erase+0x2c>
    b0b0:	e1a04002 	mov	r4, r2
    b0b4:	e5935008 	ldr	r5, [r3, #8]
    b0b8:	e24dd004 	sub	sp, sp, #4	; 0x4
    b0bc:	e1a02005 	mov	r2, r5
    b0c0:	eb0026d1 	bl	14c0c <__aeabi_llsl>
    b0c4:	e1a02514 	lsl	r2, r4, r5
    b0c8:	e28dd004 	add	sp, sp, #4	; 0x4
    b0cc:	e8bd4030 	pop	{r4, r5, lr}
    b0d0:	eafffff2 	b	b0a0 <nnd_vs_scrub>
    b0d4:	3c00d734 	.word	0x3c00d734

0000b0d8 <nnd_vs_erase>:
    b0d8:	e3a03000 	mov	r3, #0	; 0x0
    b0dc:	eaffff8f 	b	af20 <base_vs_erase>

0000b0e0 <base_block_markbad>:
    b0e0:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
    b0e4:	e1a08000 	mov	r8, r0
    b0e8:	e24dd004 	sub	sp, sp, #4	; 0x4
    b0ec:	e3a00001 	mov	r0, #1	; 0x1
    b0f0:	e1a09001 	mov	r9, r1
    b0f4:	ebfffb90 	bl	9f3c <nand_get_config>
    b0f8:	e2504000 	subs	r4, r0, #0	; 0x0
    b0fc:	03e05000 	mvneq	r5, #0	; 0x0
    b100:	0a000029 	beq	b1ac <base_block_markbad+0xcc>
    b104:	e594301c 	ldr	r3, [r4, #28]
    b108:	e59f00a8 	ldr	r0, [pc, #168]	; b1b8 <base_block_markbad+0xd8>
    b10c:	e2433001 	sub	r3, r3, #1	; 0x1
    b110:	e1c86003 	bic	r6, r8, r3
    b114:	e1a02006 	mov	r2, r6
    b118:	e1a03009 	mov	r3, r9
    b11c:	e59f5098 	ldr	r5, [pc, #152]	; b1bc <base_block_markbad+0xdc>
    b120:	e1a0e00f 	mov	lr, pc
    b124:	e595f000 	ldr	pc, [r5]
    b128:	e1a00006 	mov	r0, r6
    b12c:	e1a01009 	mov	r1, r9
    b130:	e594201c 	ldr	r2, [r4, #28]
    b134:	ebffffe7 	bl	b0d8 <nnd_vs_erase>
    b138:	e3500000 	cmp	r0, #0	; 0x0
    b13c:	e1a07009 	mov	r7, r9
    b140:	0a000002 	beq	b150 <base_block_markbad+0x70>
    b144:	e59f0074 	ldr	r0, [pc, #116]	; b1c0 <base_block_markbad+0xe0>
    b148:	e1a0e00f 	mov	lr, pc
    b14c:	e595f000 	ldr	pc, [r5]
    b150:	e5943014 	ldr	r3, [r4, #20]
    b154:	e5942008 	ldr	r2, [r4, #8]
    b158:	e0020392 	mul	r2, r2, r3
    b15c:	e0960002 	adds	r0, r6, r2
    b160:	e2a71000 	adc	r1, r7, #0	; 0x0
    b164:	ebfffbd3 	bl	a0b8 <__base_block_markbad>
    b168:	e5943008 	ldr	r3, [r4, #8]
    b16c:	e1a05000 	mov	r5, r0
    b170:	e594000c 	ldr	r0, [r4, #12]
    b174:	e1500003 	cmp	r0, r3
    b178:	0a000005 	beq	b194 <base_block_markbad+0xb4>
    b17c:	e5943014 	ldr	r3, [r4, #20]
    b180:	e0030093 	mul	r3, r3, r0
    b184:	e0960003 	adds	r0, r6, r3
    b188:	e2a71000 	adc	r1, r7, #0	; 0x0
    b18c:	ebfffbc9 	bl	a0b8 <__base_block_markbad>
    b190:	e1855000 	orr	r5, r5, r0
    b194:	e3550000 	cmp	r5, #0	; 0x0
    b198:	0a000003 	beq	b1ac <base_block_markbad+0xcc>
    b19c:	e59f0020 	ldr	r0, [pc, #32]	; b1c4 <base_block_markbad+0xe4>
    b1a0:	e59f3014 	ldr	r3, [pc, #20]	; b1bc <base_block_markbad+0xdc>
    b1a4:	e1a0e00f 	mov	lr, pc
    b1a8:	e593f000 	ldr	pc, [r3]
    b1ac:	e1a00005 	mov	r0, r5
    b1b0:	e28dd004 	add	sp, sp, #4	; 0x4
    b1b4:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
    b1b8:	00018398 	.word	0x00018398
    b1bc:	3c00d6cc 	.word	0x3c00d6cc
    b1c0:	000183b8 	.word	0x000183b8
    b1c4:	000183d0 	.word	0x000183d0

0000b1c8 <fnd_vs_erase>:
    b1c8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    b1cc:	e1a04000 	mov	r4, r0
    b1d0:	e24dd004 	sub	sp, sp, #4	; 0x4
    b1d4:	e3a00001 	mov	r0, #1	; 0x1
    b1d8:	e1a06002 	mov	r6, r2
    b1dc:	e1a05001 	mov	r5, r1
    b1e0:	ebfffb55 	bl	9f3c <nand_get_config>
    b1e4:	e1a07000 	mov	r7, r0
    b1e8:	ebfffb81 	bl	9ff4 <fnd_align>
    b1ec:	e1a01000 	mov	r1, r0
    b1f0:	e1a00006 	mov	r0, r6
    b1f4:	eb002566 	bl	14794 <__aeabi_uidiv>
    b1f8:	e5972014 	ldr	r2, [r7, #20]
    b1fc:	e1a01005 	mov	r1, r5
    b200:	e0020092 	mul	r2, r2, r0
    b204:	e1a00004 	mov	r0, r4
    b208:	e28dd004 	add	sp, sp, #4	; 0x4
    b20c:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
    b210:	eaffffb0 	b	b0d8 <nnd_vs_erase>

0000b214 <nand_rescan_bootcfg>:
    b214:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    b218:	ebffd60e 	bl	a58 <ecfg_paras>
    b21c:	e59f31ac 	ldr	r3, [pc, #428]	; b3d0 <nand_rescan_bootcfg+0x1bc>
    b220:	e1a08000 	mov	r8, r0
    b224:	e1d360b0 	ldrh	r6, [r3]
    b228:	e3560000 	cmp	r6, #0	; 0x0
    b22c:	1a00000b 	bne	b260 <nand_rescan_bootcfg+0x4c>
    b230:	ebffd503 	bl	644 <boot_device>
    b234:	e3500000 	cmp	r0, #0	; 0x0
    b238:	1a000002 	bne	b248 <nand_rescan_bootcfg+0x34>
    b23c:	ebffd4ed 	bl	5f8 <boot_info>
    b240:	e1a06000 	mov	r6, r0
    b244:	ea000005 	b	b260 <nand_rescan_bootcfg+0x4c>
    b248:	e59f0184 	ldr	r0, [pc, #388]	; b3d4 <nand_rescan_bootcfg+0x1c0>
    b24c:	e59f3184 	ldr	r3, [pc, #388]	; b3d8 <nand_rescan_bootcfg+0x1c4>
    b250:	e1a0e00f 	mov	lr, pc
    b254:	e593f000 	ldr	pc, [r3]
    b258:	e3e00000 	mvn	r0, #0	; 0x0
    b25c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    b260:	e59f7174 	ldr	r7, [pc, #372]	; b3dc <nand_rescan_bootcfg+0x1c8>
    b264:	e1a00426 	lsr	r0, r6, #8
    b268:	e1a023a6 	lsr	r2, r6, #7
    b26c:	e200000f 	and	r0, r0, #15	; 0xf
    b270:	e2022001 	and	r2, r2, #1	; 0x1
    b274:	e3a03001 	mov	r3, #1	; 0x1
    b278:	e1a01000 	mov	r1, r0
    b27c:	e5873004 	str	r3, [r7, #4]
    b280:	e5c72019 	strb	r2, [r7, #25]
    b284:	e5c70017 	strb	r0, [r7, #23]
    b288:	e59f5148 	ldr	r5, [pc, #328]	; b3d8 <nand_rescan_bootcfg+0x1c4>
    b28c:	e59f014c 	ldr	r0, [pc, #332]	; b3e0 <nand_rescan_bootcfg+0x1cc>
    b290:	e1a0e00f 	mov	lr, pc
    b294:	e595f000 	ldr	pc, [r5]
    b298:	e3160070 	tst	r6, #112	; 0x70
    b29c:	e3a0cb01 	mov	ip, #1024	; 0x400
    b2a0:	13a04010 	movne	r4, #16	; 0x10
    b2a4:	03a04008 	moveq	r4, #8	; 0x8
    b2a8:	e1a03126 	lsr	r3, r6, #2
    b2ac:	e3160002 	tst	r6, #2	; 0x2
    b2b0:	13a01005 	movne	r1, #5	; 0x5
    b2b4:	03a01004 	moveq	r1, #4	; 0x4
    b2b8:	e1a0000c 	mov	r0, ip
    b2bc:	e2033003 	and	r3, r3, #3	; 0x3
    b2c0:	e2062001 	and	r2, r6, #1	; 0x1
    b2c4:	e587c028 	str	ip, [r7, #40]
    b2c8:	e5c71016 	strb	r1, [r7, #22]
    b2cc:	e5c73014 	strb	r3, [r7, #20]
    b2d0:	e5c72015 	strb	r2, [r7, #21]
    b2d4:	e5c7401a 	strb	r4, [r7, #26]
    b2d8:	ebfffaa6 	bl	9d78 <generic_ffs>
    b2dc:	e59730c4 	ldr	r3, [r7, #196]
    b2e0:	e2402001 	sub	r2, r0, #1	; 0x1
    b2e4:	e1a00523 	lsr	r0, r3, #10
    b2e8:	e587200c 	str	r2, [r7, #12]
    b2ec:	ebfffaa1 	bl	9d78 <generic_ffs>
    b2f0:	e2400001 	sub	r0, r0, #1	; 0x1
    b2f4:	e5870008 	str	r0, [r7, #8]
    b2f8:	e1a01004 	mov	r1, r4
    b2fc:	e59f00e0 	ldr	r0, [pc, #224]	; b3e4 <nand_rescan_bootcfg+0x1d0>
    b300:	e1a0e00f 	mov	lr, pc
    b304:	e595f000 	ldr	pc, [r5]
    b308:	e3a00c02 	mov	r0, #512	; 0x200
    b30c:	ebffd5c8 	bl	a34 <ecfg_check_flag>
    b310:	e3500000 	cmp	r0, #0	; 0x0
    b314:	0a000008 	beq	b33c <nand_rescan_bootcfg+0x128>
    b318:	e30c30c0 	movw	r3, #49344	; 0xc0c0
    b31c:	e5873048 	str	r3, [r7, #72]
    b320:	e3e034ae 	mvn	r3, #-1375731712	; 0xae000000
    b324:	e587303c 	str	r3, [r7, #60]
    b328:	e59f30b8 	ldr	r3, [pc, #184]	; b3e8 <nand_rescan_bootcfg+0x1d4>
    b32c:	e5873040 	str	r3, [r7, #64]
    b330:	e3043023 	movw	r3, #16419	; 0x4023
    b334:	e5873044 	str	r3, [r7, #68]
    b338:	ea000008 	b	b360 <nand_rescan_bootcfg+0x14c>
    b33c:	e5d83007 	ldrb	r3, [r8, #7]
    b340:	e1d810ba 	ldrh	r1, [r8, #10]
    b344:	e1a03c03 	lsl	r3, r3, #24
    b348:	e1e03c23 	mvn	r3, r3, lsr #24
    b34c:	e1d820bc 	ldrh	r2, [r8, #12]
    b350:	e1e03c03 	mvn	r3, r3, lsl #24
    b354:	e5872048 	str	r2, [r7, #72]
    b358:	e587303c 	str	r3, [r7, #60]
    b35c:	e5871044 	str	r1, [r7, #68]
    b360:	e3a00c01 	mov	r0, #256	; 0x100
    b364:	ebffd5b2 	bl	a34 <ecfg_check_flag>
    b368:	e3500000 	cmp	r0, #0	; 0x0
    b36c:	e59f1068 	ldr	r1, [pc, #104]	; b3dc <nand_rescan_bootcfg+0x1c8>
    b370:	0a00000b 	beq	b3a4 <nand_rescan_bootcfg+0x190>
    b374:	e3003994 	movw	r3, #2452	; 0x994
    b378:	e581305c 	str	r3, [r1, #92]
    b37c:	e59f3068 	ldr	r3, [pc, #104]	; b3ec <nand_rescan_bootcfg+0x1d8>
    b380:	e3a00000 	mov	r0, #0	; 0x0
    b384:	e581304c 	str	r3, [r1, #76]
    b388:	e59f3060 	ldr	r3, [pc, #96]	; b3f0 <nand_rescan_bootcfg+0x1dc>
    b38c:	e5813050 	str	r3, [r1, #80]
    b390:	e59f305c 	ldr	r3, [pc, #92]	; b3f4 <nand_rescan_bootcfg+0x1e0>
    b394:	e5813054 	str	r3, [r1, #84]
    b398:	e59f3058 	ldr	r3, [pc, #88]	; b3f8 <nand_rescan_bootcfg+0x1e4>
    b39c:	e5813058 	str	r3, [r1, #88]
    b3a0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    b3a4:	e5983010 	ldr	r3, [r8, #16]
    b3a8:	e1d820be 	ldrh	r2, [r8, #14]
    b3ac:	e581304c 	str	r3, [r1, #76]
    b3b0:	e5983014 	ldr	r3, [r8, #20]
    b3b4:	e5813050 	str	r3, [r1, #80]
    b3b8:	e5983018 	ldr	r3, [r8, #24]
    b3bc:	e5813054 	str	r3, [r1, #84]
    b3c0:	e598301c 	ldr	r3, [r8, #28]
    b3c4:	e581205c 	str	r2, [r1, #92]
    b3c8:	e5813058 	str	r3, [r1, #88]
    b3cc:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    b3d0:	3c00d1d0 	.word	0x3c00d1d0
    b3d4:	000183f7 	.word	0x000183f7
    b3d8:	3c00d6cc 	.word	0x3c00d6cc
    b3dc:	3c00d734 	.word	0x3c00d734
    b3e0:	00018415 	.word	0x00018415
    b3e4:	0001842f 	.word	0x0001842f
    b3e8:	00fffaf0 	.word	0x00fffaf0
    b3ec:	1c723550 	.word	0x1c723550
    b3f0:	20fe074e 	.word	0x20fe074e
    b3f4:	1698347e 	.word	0x1698347e
    b3f8:	3fff2c80 	.word	0x3fff2c80

0000b3fc <nand_init_randomizer>:
    b3fc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b400:	e5d03001 	ldrb	r3, [r0, #1]
    b404:	e24dd004 	sub	sp, sp, #4	; 0x4
    b408:	e3530000 	cmp	r3, #0	; 0x0
    b40c:	e1a05000 	mov	r5, r0
    b410:	0a000036 	beq	b4f0 <nand_init_randomizer+0xf4>
    b414:	e5901014 	ldr	r1, [r0, #20]
    b418:	e3a02000 	mov	r2, #0	; 0x0
    b41c:	e5d00003 	ldrb	r0, [r0, #3]
    b420:	eb0003e8 	bl	c3c8 <nf2_ecc_num>
    b424:	e1a04000 	mov	r4, r0
    b428:	e1a01004 	mov	r1, r4
    b42c:	e59f00c8 	ldr	r0, [pc, #200]	; b4fc <nand_init_randomizer+0x100>
    b430:	e59f30c8 	ldr	r3, [pc, #200]	; b500 <nand_init_randomizer+0x104>
    b434:	e1a0e00f 	mov	lr, pc
    b438:	e593f000 	ldr	pc, [r3]
    b43c:	e5d53006 	ldrb	r3, [r5, #6]
    b440:	e5952014 	ldr	r2, [r5, #20]
    b444:	e3530010 	cmp	r3, #16	; 0x10
    b448:	e5953010 	ldr	r3, [r5, #16]
    b44c:	e59f00b0 	ldr	r0, [pc, #176]	; b504 <nand_init_randomizer+0x108>
    b450:	11a07002 	movne	r7, r2
    b454:	11a06003 	movne	r6, r3
    b458:	01a070a2 	lsreq	r7, r2, #1
    b45c:	01a060a3 	lsreq	r6, r3, #1
    b460:	01a040c4 	asreq	r4, r4, #1
    b464:	ebffd4c6 	bl	784 <module_enable>
    b468:	e5950048 	ldr	r0, [r5, #72]
    b46c:	eb000485 	bl	c688 <nf2_set_polynomial>
    b470:	e247b001 	sub	fp, r7, #1	; 0x1
    b474:	e2869001 	add	r9, r6, #1	; 0x1
    b478:	e244a001 	sub	sl, r4, #1	; 0x1
    b47c:	e2858038 	add	r8, r5, #56	; 0x38
    b480:	e1a04005 	mov	r4, r5
    b484:	e3a07000 	mov	r7, #0	; 0x0
    b488:	e2866003 	add	r6, r6, #3	; 0x3
    b48c:	e19800b7 	ldrh	r0, [r8, r7]
    b490:	e1a0100b 	mov	r1, fp
    b494:	e1a00880 	lsl	r0, r0, #17
    b498:	e5952048 	ldr	r2, [r5, #72]
    b49c:	e1a008a0 	lsr	r0, r0, #17
    b4a0:	eb0007a2 	bl	d330 <nf2_randc_seed_hw>
    b4a4:	e5d53006 	ldrb	r3, [r5, #6]
    b4a8:	e6ff2070 	uxth	r2, r0
    b4ac:	e3530010 	cmp	r3, #16	; 0x10
    b4b0:	e1a01006 	mov	r1, r6
    b4b4:	e1c424bc 	strh	r2, [r4, #76]
    b4b8:	01a01009 	moveq	r1, r9
    b4bc:	e1a00002 	mov	r0, r2
    b4c0:	e5952048 	ldr	r2, [r5, #72]
    b4c4:	eb000799 	bl	d330 <nf2_randc_seed_hw>
    b4c8:	e1c405bc 	strh	r0, [r4, #92]
    b4cc:	e1d405bc 	ldrh	r0, [r4, #92]
    b4d0:	e1a0100a 	mov	r1, sl
    b4d4:	e5952048 	ldr	r2, [r5, #72]
    b4d8:	eb000794 	bl	d330 <nf2_randc_seed_hw>
    b4dc:	e2877002 	add	r7, r7, #2	; 0x2
    b4e0:	e3570010 	cmp	r7, #16	; 0x10
    b4e4:	e1c406bc 	strh	r0, [r4, #108]
    b4e8:	e2844002 	add	r4, r4, #2	; 0x2
    b4ec:	1affffe6 	bne	b48c <nand_init_randomizer+0x90>
    b4f0:	e3a00000 	mov	r0, #0	; 0x0
    b4f4:	e28dd004 	add	sp, sp, #4	; 0x4
    b4f8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    b4fc:	0001843d 	.word	0x0001843d
    b500:	3c00d6cc 	.word	0x3c00d6cc
    b504:	21e3c000 	.word	0x21e3c000

0000b508 <nand_calculate_rnbtimeout>:
    b508:	e92d4030 	push	{r4, r5, lr}
    b50c:	e59f4040 	ldr	r4, [pc, #64]	; b554 <nand_calculate_rnbtimeout+0x4c>
    b510:	e24dd004 	sub	sp, sp, #4	; 0x4
    b514:	e1d053bc 	ldrh	r5, [r0, #60]
    b518:	e59f0038 	ldr	r0, [pc, #56]	; b558 <nand_calculate_rnbtimeout+0x50>
    b51c:	ebffd738 	bl	1204 <module_get_clock>
    b520:	e1a01004 	mov	r1, r4
    b524:	eb00251c 	bl	1499c <__aeabi_idiv>
    b528:	e1a01000 	mov	r1, r0
    b52c:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    b530:	eb002519 	bl	1499c <__aeabi_idiv>
    b534:	e0050594 	mul	r5, r4, r5
    b538:	e1a01000 	mov	r1, r0
    b53c:	e1a00005 	mov	r0, r5
    b540:	eb002515 	bl	1499c <__aeabi_idiv>
    b544:	e3c004ff 	bic	r0, r0, #-16777216	; 0xff000000
    b548:	e3c000ff 	bic	r0, r0, #255	; 0xff
    b54c:	e28dd004 	add	sp, sp, #4	; 0x4
    b550:	e8bd8030 	pop	{r4, r5, pc}
    b554:	000f4240 	.word	0x000f4240
    b558:	21e0a270 	.word	0x21e0a270

0000b55c <nand_calculate_synctiming>:
    b55c:	e92d4030 	push	{r4, r5, lr}
    b560:	e1a05000 	mov	r5, r0
    b564:	e24dd004 	sub	sp, sp, #4	; 0x4
    b568:	e59f0044 	ldr	r0, [pc, #68]	; b5b4 <nand_calculate_synctiming+0x58>
    b56c:	ebffd724 	bl	1204 <module_get_clock>
    b570:	e59f1040 	ldr	r1, [pc, #64]	; b5b8 <nand_calculate_synctiming+0x5c>
    b574:	eb002508 	bl	1499c <__aeabi_idiv>
    b578:	e1a01000 	mov	r1, r0
    b57c:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    b580:	eb002505 	bl	1499c <__aeabi_idiv>
    b584:	e1a04000 	mov	r4, r0
    b588:	e1a01000 	mov	r1, r0
    b58c:	e5d5002c 	ldrb	r0, [r5, #44]
    b590:	eb002501 	bl	1499c <__aeabi_idiv>
    b594:	e1a01004 	mov	r1, r4
    b598:	e2804001 	add	r4, r0, #1	; 0x1
    b59c:	e5d5002d 	ldrb	r0, [r5, #45]
    b5a0:	eb0024fd 	bl	1499c <__aeabi_idiv>
    b5a4:	e2800002 	add	r0, r0, #2	; 0x2
    b5a8:	e1840200 	orr	r0, r4, r0, lsl #4
    b5ac:	e28dd004 	add	sp, sp, #4	; 0x4
    b5b0:	e8bd8030 	pop	{r4, r5, pc}
    b5b4:	21e0a270 	.word	0x21e0a270
    b5b8:	007a1200 	.word	0x007a1200

0000b5bc <nand_calculate_phydelay>:
    b5bc:	e92d4030 	push	{r4, r5, lr}
    b5c0:	e1d043be 	ldrh	r4, [r0, #62]
    b5c4:	e24dd004 	sub	sp, sp, #4	; 0x4
    b5c8:	e59f0050 	ldr	r0, [pc, #80]	; b620 <nand_calculate_phydelay+0x64>
    b5cc:	ebffd70c 	bl	1204 <module_get_clock>
    b5d0:	e59f104c 	ldr	r1, [pc, #76]	; b624 <nand_calculate_phydelay+0x68>
    b5d4:	eb0024f0 	bl	1499c <__aeabi_idiv>
    b5d8:	e1a05424 	lsr	r5, r4, #8
    b5dc:	e20440ff 	and	r4, r4, #255	; 0xff
    b5e0:	e3540000 	cmp	r4, #0	; 0x0
    b5e4:	03a04001 	moveq	r4, #1	; 0x1
    b5e8:	e0010490 	mul	r1, r0, r4
    b5ec:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    b5f0:	eb0024e9 	bl	1499c <__aeabi_idiv>
    b5f4:	e3550000 	cmp	r5, #0	; 0x0
    b5f8:	03a05001 	moveq	r5, #1	; 0x1
    b5fc:	e3a03ffa 	mov	r3, #1000	; 0x3e8
    b600:	e0000093 	mul	r0, r3, r0
    b604:	e1a01005 	mov	r1, r5
    b608:	eb0024e3 	bl	1499c <__aeabi_idiv>
    b60c:	e35000ff 	cmp	r0, #255	; 0xff
    b610:	c30f0efe 	movwgt	r0, #65278	; 0xfefe
    b614:	d1800400 	orrle	r0, r0, r0, lsl #8
    b618:	e28dd004 	add	sp, sp, #4	; 0x4
    b61c:	e8bd8030 	pop	{r4, r5, pc}
    b620:	21e0a270 	.word	0x21e0a270
    b624:	007a1200 	.word	0x007a1200

0000b628 <nand_calculate_timing>:
    b628:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    b62c:	e1a04000 	mov	r4, r0
    b630:	e59f0108 	ldr	r0, [pc, #264]	; b740 <nand_calculate_timing+0x118>
    b634:	ebffd6f2 	bl	1204 <module_get_clock>
    b638:	e59f1104 	ldr	r1, [pc, #260]	; b744 <nand_calculate_timing+0x11c>
    b63c:	eb0024d6 	bl	1499c <__aeabi_idiv>
    b640:	e1a01000 	mov	r1, r0
    b644:	e3a00ffa 	mov	r0, #1000	; 0x3e8
    b648:	eb0024d3 	bl	1499c <__aeabi_idiv>
    b64c:	e5d45029 	ldrb	r5, [r4, #41]
    b650:	e5d43026 	ldrb	r3, [r4, #38]
    b654:	e1a07000 	mov	r7, r0
    b658:	e5d46025 	ldrb	r6, [r4, #37]
    b65c:	e0653003 	rsb	r3, r5, r3
    b660:	e5d42024 	ldrb	r2, [r4, #36]
    b664:	e5d4102a 	ldrb	r1, [r4, #42]
    b668:	e5d4002b 	ldrb	r0, [r4, #43]
    b66c:	e5d4c028 	ldrb	ip, [r4, #40]
    b670:	e1c33fc3 	bic	r3, r3, r3, asr #31
    b674:	e1520001 	cmp	r2, r1
    b678:	31a02001 	movcc	r2, r1
    b67c:	e1500005 	cmp	r0, r5
    b680:	21a08000 	movcs	r8, r0
    b684:	31a08005 	movcc	r8, r5
    b688:	e15c0006 	cmp	ip, r6
    b68c:	21a0900c 	movcs	r9, ip
    b690:	31a09006 	movcc	r9, r6
    b694:	e1570003 	cmp	r7, r3
    b698:	e2825005 	add	r5, r2, #5	; 0x5
    b69c:	e5d44027 	ldrb	r4, [r4, #39]
    b6a0:	c3a0a001 	movgt	sl, #1	; 0x1
    b6a4:	ca000003 	bgt	b6b8 <nand_calculate_timing+0x90>
    b6a8:	e1a00003 	mov	r0, r3
    b6ac:	e1a01007 	mov	r1, r7
    b6b0:	eb0024b9 	bl	1499c <__aeabi_idiv>
    b6b4:	e280a001 	add	sl, r0, #1	; 0x1
    b6b8:	e1570005 	cmp	r7, r5
    b6bc:	c3a06001 	movgt	r6, #1	; 0x1
    b6c0:	ca000003 	bgt	b6d4 <nand_calculate_timing+0xac>
    b6c4:	e1a00005 	mov	r0, r5
    b6c8:	e1a01007 	mov	r1, r7
    b6cc:	eb0024b2 	bl	1499c <__aeabi_idiv>
    b6d0:	e2806001 	add	r6, r0, #1	; 0x1
    b6d4:	e1570008 	cmp	r7, r8
    b6d8:	c3a05001 	movgt	r5, #1	; 0x1
    b6dc:	ca000003 	bgt	b6f0 <nand_calculate_timing+0xc8>
    b6e0:	e1a00008 	mov	r0, r8
    b6e4:	e1a01007 	mov	r1, r7
    b6e8:	eb0024ab 	bl	1499c <__aeabi_idiv>
    b6ec:	e2805001 	add	r5, r0, #1	; 0x1
    b6f0:	e1570004 	cmp	r7, r4
    b6f4:	c3a04001 	movgt	r4, #1	; 0x1
    b6f8:	ca000003 	bgt	b70c <nand_calculate_timing+0xe4>
    b6fc:	e1a00004 	mov	r0, r4
    b700:	e1a01007 	mov	r1, r7
    b704:	eb0024a4 	bl	1499c <__aeabi_idiv>
    b708:	e2804001 	add	r4, r0, #1	; 0x1
    b70c:	e1570009 	cmp	r7, r9
    b710:	c3a03001 	movgt	r3, #1	; 0x1
    b714:	ca000003 	bgt	b728 <nand_calculate_timing+0x100>
    b718:	e1a00009 	mov	r0, r9
    b71c:	e1a01007 	mov	r1, r7
    b720:	eb00249d 	bl	1499c <__aeabi_idiv>
    b724:	e2803001 	add	r3, r0, #1	; 0x1
    b728:	e1a0080a 	lsl	r0, sl, #16
    b72c:	e1800606 	orr	r0, r0, r6, lsl #12
    b730:	e1800405 	orr	r0, r0, r5, lsl #8
    b734:	e1800204 	orr	r0, r0, r4, lsl #4
    b738:	e1800003 	orr	r0, r0, r3
    b73c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    b740:	21e0a270 	.word	0x21e0a270
    b744:	000f4240 	.word	0x000f4240

0000b748 <nand_rescan_config>:
    b748:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    b74c:	e59f343c 	ldr	r3, [pc, #1084]	; bb90 <nand_rescan_config+0x448>
    b750:	e24dd024 	sub	sp, sp, #36	; 0x24
    b754:	e1a04000 	mov	r4, r0
    b758:	e59f0434 	ldr	r0, [pc, #1076]	; bb94 <nand_rescan_config+0x44c>
    b75c:	e1a0e00f 	mov	lr, pc
    b760:	e593f000 	ldr	pc, [r3]
    b764:	e59f042c 	ldr	r0, [pc, #1068]	; bb98 <nand_rescan_config+0x450>
    b768:	ebffd608 	bl	f90 <cpu_recase>
    b76c:	ebfffea8 	bl	b214 <nand_rescan_bootcfg>
    b770:	e3500000 	cmp	r0, #0	; 0x0
    b774:	1a000001 	bne	b780 <nand_rescan_config+0x38>
    b778:	e59f041c 	ldr	r0, [pc, #1052]	; bb9c <nand_rescan_config+0x454>
    b77c:	ebffff1e 	bl	b3fc <nand_init_randomizer>
    b780:	e3540000 	cmp	r4, #0	; 0x0
    b784:	11a07004 	movne	r7, r4
    b788:	1a0000ee 	bne	bb48 <nand_rescan_config+0x400>
    b78c:	ea0000fc 	b	bb84 <nand_rescan_config+0x43c>
    b790:	e1a00007 	mov	r0, r7
    b794:	ebffffa3 	bl	b628 <nand_calculate_timing>
    b798:	e1a04000 	mov	r4, r0
    b79c:	e1a00007 	mov	r0, r7
    b7a0:	ebffff6d 	bl	b55c <nand_calculate_synctiming>
    b7a4:	e184bc00 	orr	fp, r4, r0, lsl #24
    b7a8:	e1a00007 	mov	r0, r7
    b7ac:	ebffff82 	bl	b5bc <nand_calculate_phydelay>
    b7b0:	e1a09000 	mov	r9, r0
    b7b4:	e1a00007 	mov	r0, r7
    b7b8:	ebffff52 	bl	b508 <nand_calculate_rnbtimeout>
    b7bc:	e1a0a000 	mov	sl, r0
    b7c0:	e1a0100b 	mov	r1, fp
    b7c4:	e59f03d4 	ldr	r0, [pc, #980]	; bba0 <nand_rescan_config+0x458>
    b7c8:	e1a02009 	mov	r2, r9
    b7cc:	e1a0300a 	mov	r3, sl
    b7d0:	e59fc3b8 	ldr	ip, [pc, #952]	; bb90 <nand_rescan_config+0x448>
    b7d4:	e1a0e00f 	mov	lr, pc
    b7d8:	e59cf000 	ldr	pc, [ip]
    b7dc:	e5971054 	ldr	r1, [r7, #84]
    b7e0:	e1d744b0 	ldrh	r4, [r7, #64]
    b7e4:	e1a0c121 	lsr	ip, r1, #2
    b7e8:	e20cc003 	and	ip, ip, #3	; 0x3
    b7ec:	e2011003 	and	r1, r1, #3	; 0x3
    b7f0:	e1a0200b 	mov	r2, fp
    b7f4:	e1a0300a 	mov	r3, sl
    b7f8:	e28d0018 	add	r0, sp, #24	; 0x18
    b7fc:	e88d1210 	stm	sp, {r4, r9, ip}
    b800:	eb000782 	bl	d610 <nand_readid>
    b804:	e5dd001b 	ldrb	r0, [sp, #27]
    b808:	e5ddc01c 	ldrb	ip, [sp, #28]
    b80c:	e5dd401d 	ldrb	r4, [sp, #29]
    b810:	e5dd501e 	ldrb	r5, [sp, #30]
    b814:	e5dd601f 	ldrb	r6, [sp, #31]
    b818:	e5dd1018 	ldrb	r1, [sp, #24]
    b81c:	e88d1001 	stm	sp, {r0, ip}
    b820:	e58d4008 	str	r4, [sp, #8]
    b824:	e5dd2019 	ldrb	r2, [sp, #25]
    b828:	e5dd301a 	ldrb	r3, [sp, #26]
    b82c:	e58d500c 	str	r5, [sp, #12]
    b830:	e58d6010 	str	r6, [sp, #16]
    b834:	e59f0368 	ldr	r0, [pc, #872]	; bba4 <nand_rescan_config+0x45c>
    b838:	e59fc350 	ldr	ip, [pc, #848]	; bb90 <nand_rescan_config+0x448>
    b83c:	e1a0e00f 	mov	lr, pc
    b840:	e59cf000 	ldr	pc, [ip]
    b844:	e3a04000 	mov	r4, #0	; 0x0
    b848:	e1a01007 	mov	r1, r7
    b84c:	e5d1301c 	ldrb	r3, [r1, #28]
    b850:	e2811001 	add	r1, r1, #1	; 0x1
    b854:	e3530000 	cmp	r3, #0	; 0x0
    b858:	0a000009 	beq	b884 <nand_rescan_config+0x13c>
    b85c:	e28d0018 	add	r0, sp, #24	; 0x18
    b860:	e7d02004 	ldrb	r2, [r0, r4]
    b864:	e1520003 	cmp	r2, r3
    b868:	0a000005 	beq	b884 <nand_rescan_config+0x13c>
    b86c:	e59f0334 	ldr	r0, [pc, #820]	; bba8 <nand_rescan_config+0x460>
    b870:	e1a01004 	mov	r1, r4
    b874:	e59fc314 	ldr	ip, [pc, #788]	; bb90 <nand_rescan_config+0x448>
    b878:	e1a0e00f 	mov	lr, pc
    b87c:	e59cf000 	ldr	pc, [ip]
    b880:	ea000002 	b	b890 <nand_rescan_config+0x148>
    b884:	e2844001 	add	r4, r4, #1	; 0x1
    b888:	e3540008 	cmp	r4, #8	; 0x8
    b88c:	1affffee 	bne	b84c <nand_rescan_config+0x104>
    b890:	e3540008 	cmp	r4, #8	; 0x8
    b894:	e2877064 	add	r7, r7, #100	; 0x64
    b898:	1a0000aa 	bne	bb48 <nand_rescan_config+0x400>
    b89c:	e59f32ec 	ldr	r3, [pc, #748]	; bb90 <nand_rescan_config+0x448>
    b8a0:	e59f0304 	ldr	r0, [pc, #772]	; bbac <nand_rescan_config+0x464>
    b8a4:	e1a0e00f 	mov	lr, pc
    b8a8:	e593f000 	ldr	pc, [r3]
    b8ac:	e5983054 	ldr	r3, [r8, #84]
    b8b0:	e59f22f8 	ldr	r2, [pc, #760]	; bbb0 <nand_rescan_config+0x468>
    b8b4:	e2033003 	and	r3, r3, #3	; 0x3
    b8b8:	e5c230b0 	strb	r3, [r2, #176]
    b8bc:	e5983054 	ldr	r3, [r8, #84]
    b8c0:	e1a033a3 	lsr	r3, r3, #7
    b8c4:	e2033001 	and	r3, r3, #1	; 0x1
    b8c8:	e5c230b1 	strb	r3, [r2, #177]
    b8cc:	e5983054 	ldr	r3, [r8, #84]
    b8d0:	e1a03223 	lsr	r3, r3, #4
    b8d4:	e2033007 	and	r3, r3, #7	; 0x7
    b8d8:	e5c230b2 	strb	r3, [r2, #178]
    b8dc:	e5d81055 	ldrb	r1, [r8, #85]
    b8e0:	e351004f 	cmp	r1, #79	; 0x4f
    b8e4:	c3a0100c 	movgt	r1, #12	; 0xc
    b8e8:	ca000020 	bgt	b970 <nand_rescan_config+0x228>
    b8ec:	e3510047 	cmp	r1, #71	; 0x47
    b8f0:	c3a0100b 	movgt	r1, #11	; 0xb
    b8f4:	ca00001d 	bgt	b970 <nand_rescan_config+0x228>
    b8f8:	e351003f 	cmp	r1, #63	; 0x3f
    b8fc:	c3a0100a 	movgt	r1, #10	; 0xa
    b900:	ca00001a 	bgt	b970 <nand_rescan_config+0x228>
    b904:	e351003b 	cmp	r1, #59	; 0x3b
    b908:	c3a01009 	movgt	r1, #9	; 0x9
    b90c:	ca000017 	bgt	b970 <nand_rescan_config+0x228>
    b910:	e3510037 	cmp	r1, #55	; 0x37
    b914:	c1a01004 	movgt	r1, r4
    b918:	ca000014 	bgt	b970 <nand_rescan_config+0x228>
    b91c:	e351002f 	cmp	r1, #47	; 0x2f
    b920:	c3a01007 	movgt	r1, #7	; 0x7
    b924:	ca000011 	bgt	b970 <nand_rescan_config+0x228>
    b928:	e3510027 	cmp	r1, #39	; 0x27
    b92c:	c3a01006 	movgt	r1, #6	; 0x6
    b930:	ca00000e 	bgt	b970 <nand_rescan_config+0x228>
    b934:	e351001f 	cmp	r1, #31	; 0x1f
    b938:	c3a01005 	movgt	r1, #5	; 0x5
    b93c:	ca00000b 	bgt	b970 <nand_rescan_config+0x228>
    b940:	e3510017 	cmp	r1, #23	; 0x17
    b944:	c3a01004 	movgt	r1, #4	; 0x4
    b948:	ca000008 	bgt	b970 <nand_rescan_config+0x228>
    b94c:	e351000f 	cmp	r1, #15	; 0xf
    b950:	c3a01003 	movgt	r1, #3	; 0x3
    b954:	ca000005 	bgt	b970 <nand_rescan_config+0x228>
    b958:	e3510007 	cmp	r1, #7	; 0x7
    b95c:	c3a01002 	movgt	r1, #2	; 0x2
    b960:	ca000002 	bgt	b970 <nand_rescan_config+0x228>
    b964:	e3510003 	cmp	r1, #3	; 0x3
    b968:	d3a01000 	movle	r1, #0	; 0x0
    b96c:	c3a01001 	movgt	r1, #1	; 0x1
    b970:	e59f2238 	ldr	r2, [pc, #568]	; bbb0 <nand_rescan_config+0x468>
    b974:	e5c210b3 	strb	r1, [r2, #179]
    b978:	e5d83056 	ldrb	r3, [r8, #86]
    b97c:	e3530007 	cmp	r3, #7	; 0x7
    b980:	93a03000 	movls	r3, #0	; 0x0
    b984:	83a03001 	movhi	r3, #1	; 0x1
    b988:	e5c230b4 	strb	r3, [r2, #180]
    b98c:	e5983054 	ldr	r3, [r8, #84]
    b990:	e1a03423 	lsr	r3, r3, #8
    b994:	e1a03803 	lsl	r3, r3, #16
    b998:	e1a03823 	lsr	r3, r3, #16
    b99c:	e2533000 	subs	r3, r3, #0	; 0x0
    b9a0:	13a03001 	movne	r3, #1	; 0x1
    b9a4:	e5c230b5 	strb	r3, [r2, #181]
    b9a8:	e1d8e3b0 	ldrh	lr, [r8, #48]
    b9ac:	e1d803b8 	ldrh	r0, [r8, #56]
    b9b0:	e1d813b6 	ldrh	r1, [r8, #54]
    b9b4:	e1d833ba 	ldrh	r3, [r8, #58]
    b9b8:	e35e0000 	cmp	lr, #0	; 0x0
    b9bc:	e58200bc 	str	r0, [r2, #188]
    b9c0:	e58230c0 	str	r3, [r2, #192]
    b9c4:	e58210b8 	str	r1, [r2, #184]
    b9c8:	01a0000e 	moveq	r0, lr
    b9cc:	0a00000c 	beq	ba04 <nand_rescan_config+0x2bc>
    b9d0:	e1a0380e 	lsl	r3, lr, #16
    b9d4:	e31304ff 	tst	r3, #-16777216	; 0xff000000
    b9d8:	01a03403 	lsleq	r3, r3, #8
    b9dc:	e3a00010 	mov	r0, #16	; 0x10
    b9e0:	02400008 	subeq	r0, r0, #8	; 0x8
    b9e4:	e313020f 	tst	r3, #-268435456	; 0xf0000000
    b9e8:	01a03203 	lsleq	r3, r3, #4
    b9ec:	02400004 	subeq	r0, r0, #4	; 0x4
    b9f0:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
    b9f4:	01a03103 	lsleq	r3, r3, #2
    b9f8:	02400002 	subeq	r0, r0, #2	; 0x2
    b9fc:	e3530000 	cmp	r3, #0	; 0x0
    ba00:	a2400001 	subge	r0, r0, #1	; 0x1
    ba04:	e3a04001 	mov	r4, #1	; 0x1
    ba08:	e2400001 	sub	r0, r0, #1	; 0x1
    ba0c:	e1a00014 	lsl	r0, r4, r0
    ba10:	e1d823b2 	ldrh	r2, [r8, #50]
    ba14:	e59f5194 	ldr	r5, [pc, #404]	; bbb0 <nand_rescan_config+0x468>
    ba18:	e0020092 	mul	r2, r2, r0
    ba1c:	e1d8c4b0 	ldrh	ip, [r8, #64]
    ba20:	e1d813b4 	ldrh	r1, [r8, #52]
    ba24:	e2403001 	sub	r3, r0, #1	; 0x1
    ba28:	e003300e 	and	r3, r3, lr
    ba2c:	e58530c8 	str	r3, [r5, #200]
    ba30:	e58520cc 	str	r2, [r5, #204]
    ba34:	e58510d4 	str	r1, [r5, #212]
    ba38:	e1a02009 	mov	r2, r9
    ba3c:	e58dc000 	str	ip, [sp]
    ba40:	e1a0100b 	mov	r1, fp
    ba44:	e1a0300a 	mov	r3, sl
    ba48:	e58500c4 	str	r0, [r5, #196]
    ba4c:	e585c0e0 	str	ip, [r5, #224]
    ba50:	e59f015c 	ldr	r0, [pc, #348]	; bbb4 <nand_rescan_config+0x46c>
    ba54:	e59fc134 	ldr	ip, [pc, #308]	; bb90 <nand_rescan_config+0x448>
    ba58:	e585b0d8 	str	fp, [r5, #216]
    ba5c:	e58590e4 	str	r9, [r5, #228]
    ba60:	e585a0dc 	str	sl, [r5, #220]
    ba64:	e1a0e00f 	mov	lr, pc
    ba68:	e59cf000 	ldr	pc, [ip]
    ba6c:	e5983044 	ldr	r3, [r8, #68]
    ba70:	e1d824b2 	ldrh	r2, [r8, #66]
    ba74:	e58530e8 	str	r3, [r5, #232]
    ba78:	e5983048 	ldr	r3, [r8, #72]
    ba7c:	e1a00005 	mov	r0, r5
    ba80:	e58530ec 	str	r3, [r5, #236]
    ba84:	e598304c 	ldr	r3, [r8, #76]
    ba88:	e58530f0 	str	r3, [r5, #240]
    ba8c:	e5983050 	ldr	r3, [r8, #80]
    ba90:	e58520f8 	str	r2, [r5, #248]
    ba94:	e58530f4 	str	r3, [r5, #244]
    ba98:	e5983054 	ldr	r3, [r8, #84]
    ba9c:	e48040b0 	str	r4, [r0], #176
    baa0:	e313000c 	tst	r3, #12	; 0xc
    baa4:	13a03010 	movne	r3, #16	; 0x10
    baa8:	03a03008 	moveq	r3, #8	; 0x8
    baac:	e5c530b6 	strb	r3, [r5, #182]
    bab0:	e1d833b2 	ldrh	r3, [r8, #50]
    bab4:	e58530d0 	str	r3, [r5, #208]
    bab8:	ebfff8c4 	bl	9dd0 <print_nand_cfg>
    babc:	e59500c4 	ldr	r0, [r5, #196]
    bac0:	ebfff8ac 	bl	9d78 <generic_ffs>
    bac4:	e2400001 	sub	r0, r0, #1	; 0x1
    bac8:	e5850010 	str	r0, [r5, #16]
    bacc:	ebffd2dc 	bl	644 <boot_device>
    bad0:	e3500000 	cmp	r0, #0	; 0x0
    bad4:	1a000005 	bne	baf0 <nand_rescan_config+0x3a8>
    bad8:	e5951028 	ldr	r1, [r5, #40]
    badc:	e59500c4 	ldr	r0, [r5, #196]
    bae0:	eb00232b 	bl	14794 <__aeabi_uidiv>
    bae4:	ebfff8a3 	bl	9d78 <generic_ffs>
    bae8:	e2400001 	sub	r0, r0, #1	; 0x1
    baec:	e5850008 	str	r0, [r5, #8]
    baf0:	e59f20b8 	ldr	r2, [pc, #184]	; bbb0 <nand_rescan_config+0x468>
    baf4:	e5d230b6 	ldrb	r3, [r2, #182]
    baf8:	e3530008 	cmp	r3, #8	; 0x8
    bafc:	1a000006 	bne	bb1c <nand_rescan_config+0x3d4>
    bb00:	e5d230b0 	ldrb	r3, [r2, #176]
    bb04:	e59f20ac 	ldr	r2, [pc, #172]	; bbb8 <nand_rescan_config+0x470>
    bb08:	e3530000 	cmp	r3, #0	; 0x0
    bb0c:	03003c83 	movweq	r3, #3203	; 0xc83
    bb10:	13003c8b 	movwne	r3, #3211	; 0xc8b
    bb14:	e1c230b0 	strh	r3, [r2]
    bb18:	ea000002 	b	bb28 <nand_rescan_config+0x3e0>
    bb1c:	e59f3094 	ldr	r3, [pc, #148]	; bbb8 <nand_rescan_config+0x470>
    bb20:	e3002c93 	movw	r2, #3219	; 0xc93
    bb24:	e1c320b0 	strh	r2, [r3]
    bb28:	e59f3080 	ldr	r3, [pc, #128]	; bbb0 <nand_rescan_config+0x468>
    bb2c:	e59320cc 	ldr	r2, [r3, #204]
    bb30:	e5931008 	ldr	r1, [r3, #8]
    bb34:	e28300b0 	add	r0, r3, #176	; 0xb0
    bb38:	e1a02132 	lsr	r2, r2, r1
    bb3c:	e5832030 	str	r2, [r3, #48]
    bb40:	ebfffe2d 	bl	b3fc <nand_init_randomizer>
    bb44:	ea000004 	b	bb5c <nand_rescan_config+0x414>
    bb48:	e5973000 	ldr	r3, [r7]
    bb4c:	e59f0068 	ldr	r0, [pc, #104]	; bbbc <nand_rescan_config+0x474>
    bb50:	e1a08007 	mov	r8, r7
    bb54:	e1530000 	cmp	r3, r0
    bb58:	0affff0c 	beq	b790 <nand_rescan_config+0x48>
    bb5c:	e5982000 	ldr	r2, [r8]
    bb60:	e59f3054 	ldr	r3, [pc, #84]	; bbbc <nand_rescan_config+0x474>
    bb64:	e1520003 	cmp	r2, r3
    bb68:	0a000005 	beq	bb84 <nand_rescan_config+0x43c>
    bb6c:	e59f004c 	ldr	r0, [pc, #76]	; bbc0 <nand_rescan_config+0x478>
    bb70:	e59f3018 	ldr	r3, [pc, #24]	; bb90 <nand_rescan_config+0x448>
    bb74:	e1a0e00f 	mov	lr, pc
    bb78:	e593f000 	ldr	pc, [r3]
    bb7c:	e3e00000 	mvn	r0, #0	; 0x0
    bb80:	ea000000 	b	bb88 <nand_rescan_config+0x440>
    bb84:	e3a00000 	mov	r0, #0	; 0x0
    bb88:	e28dd024 	add	sp, sp, #36	; 0x24
    bb8c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    bb90:	3c00d6cc 	.word	0x3c00d6cc
    bb94:	00018451 	.word	0x00018451
    bb98:	00016267 	.word	0x00016267
    bb9c:	3c00d748 	.word	0x3c00d748
    bba0:	000184da 	.word	0x000184da
    bba4:	00018465 	.word	0x00018465
    bba8:	00018496 	.word	0x00018496
    bbac:	000184b2 	.word	0x000184b2
    bbb0:	3c00d734 	.word	0x3c00d734
    bbb4:	000184cf 	.word	0x000184cf
    bbb8:	3c00d1d0 	.word	0x3c00d1d0
    bbbc:	aca4570e 	.word	0xaca4570e
    bbc0:	000184e6 	.word	0x000184e6

0000bbc4 <bnd_vs_reset>:
    bbc4:	e92d4010 	push	{r4, lr}
    bbc8:	e3a00000 	mov	r0, #0	; 0x0
    bbcc:	ebfffedd 	bl	b748 <nand_rescan_config>
    bbd0:	e3a00000 	mov	r0, #0	; 0x0
    bbd4:	ebfff8d8 	bl	9f3c <nand_get_config>
    bbd8:	e1a04000 	mov	r4, r0
    bbdc:	e3a00001 	mov	r0, #1	; 0x1
    bbe0:	ebfff8d5 	bl	9f3c <nand_get_config>
    bbe4:	e3540000 	cmp	r4, #0	; 0x0
    bbe8:	03e04000 	mvneq	r4, #0	; 0x0
    bbec:	0a00000a 	beq	bc1c <bnd_vs_reset+0x58>
    bbf0:	e3500000 	cmp	r0, #0	; 0x0
    bbf4:	01a00004 	moveq	r0, r4
    bbf8:	e3a01000 	mov	r1, #0	; 0x0
    bbfc:	eb0006d9 	bl	d768 <nf2_hardware_reset>
    bc00:	e2504000 	subs	r4, r0, #0	; 0x0
    bc04:	0a000004 	beq	bc1c <bnd_vs_reset+0x58>
    bc08:	e59f0014 	ldr	r0, [pc, #20]	; bc24 <bnd_vs_reset+0x60>
    bc0c:	e1a01004 	mov	r1, r4
    bc10:	e59f3010 	ldr	r3, [pc, #16]	; bc28 <bnd_vs_reset+0x64>
    bc14:	e1a0e00f 	mov	lr, pc
    bc18:	e593f000 	ldr	pc, [r3]
    bc1c:	e1a00004 	mov	r0, r4
    bc20:	e8bd8010 	pop	{r4, pc}
    bc24:	00018501 	.word	0x00018501
    bc28:	3c00d6cc 	.word	0x3c00d6cc

0000bc2c <ecc_add_stat>:
    bc2c:	e92d4070 	push	{r4, r5, r6, lr}
    bc30:	e59f3134 	ldr	r3, [pc, #308]	; bd6c <ecc_add_stat+0x140>
    bc34:	e24dd030 	sub	sp, sp, #48	; 0x30
    bc38:	e28d4004 	add	r4, sp, #4	; 0x4
    bc3c:	e1a0c003 	mov	ip, r3
    bc40:	e1a06000 	mov	r6, r0
    bc44:	e1a05001 	mov	r5, r1
    bc48:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    bc4c:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    bc50:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
    bc54:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
    bc58:	e59c1000 	ldr	r1, [ip]
    bc5c:	e59f210c 	ldr	r2, [pc, #268]	; bd70 <ecc_add_stat+0x144>
    bc60:	e28d3028 	add	r3, sp, #40	; 0x28
    bc64:	e3560000 	cmp	r6, #0	; 0x0
    bc68:	e5841000 	str	r1, [r4]
    bc6c:	e8920003 	ldm	r2, {r0, r1}
    bc70:	e8830003 	stm	r3, {r0, r1}
    bc74:	0a00001b 	beq	bce8 <ecc_add_stat+0xbc>
    bc78:	e3550000 	cmp	r5, #0	; 0x0
    bc7c:	b59f20f0 	ldrlt	r2, [pc, #240]	; bd74 <ecc_add_stat+0x148>
    bc80:	b5923008 	ldrlt	r3, [r2, #8]
    bc84:	b2833001 	addlt	r3, r3, #1	; 0x1
    bc88:	b5823008 	strlt	r3, [r2, #8]
    bc8c:	ba000034 	blt	bd64 <ecc_add_stat+0x138>
    bc90:	059f20dc 	ldreq	r2, [pc, #220]	; bd74 <ecc_add_stat+0x148>
    bc94:	0592300c 	ldreq	r3, [r2, #12]
    bc98:	02833001 	addeq	r3, r3, #1	; 0x1
    bc9c:	0582300c 	streq	r3, [r2, #12]
    bca0:	0a00002f 	beq	bd64 <ecc_add_stat+0x138>
    bca4:	e3550008 	cmp	r5, #8	; 0x8
    bca8:	c1a01005 	movgt	r1, r5
    bcac:	c59f00c4 	ldrgt	r0, [pc, #196]	; bd78 <ecc_add_stat+0x14c>
    bcb0:	ca00001d 	bgt	bd2c <ecc_add_stat+0x100>
    bcb4:	e59d3028 	ldr	r3, [sp, #40]
    bcb8:	e1550003 	cmp	r5, r3
    bcbc:	d3a01000 	movle	r1, #0	; 0x0
    bcc0:	da000003 	ble	bcd4 <ecc_add_stat+0xa8>
    bcc4:	e59d302c 	ldr	r3, [sp, #44]
    bcc8:	e1550003 	cmp	r5, r3
    bccc:	d3a01001 	movle	r1, #1	; 0x1
    bcd0:	c3a01002 	movgt	r1, #2	; 0x2
    bcd4:	e59f2098 	ldr	r2, [pc, #152]	; bd74 <ecc_add_stat+0x148>
    bcd8:	e7923101 	ldr	r3, [r2, r1, lsl #2]
    bcdc:	e2833001 	add	r3, r3, #1	; 0x1
    bce0:	e7823101 	str	r3, [r2, r1, lsl #2]
    bce4:	ea00001e 	b	bd64 <ecc_add_stat+0x138>
    bce8:	e3550000 	cmp	r5, #0	; 0x0
    bcec:	b59f2088 	ldrlt	r2, [pc, #136]	; bd7c <ecc_add_stat+0x150>
    bcf0:	b5923024 	ldrlt	r3, [r2, #36]
    bcf4:	b2833001 	addlt	r3, r3, #1	; 0x1
    bcf8:	b5823024 	strlt	r3, [r2, #36]
    bcfc:	ba000018 	blt	bd64 <ecc_add_stat+0x138>
    bd00:	059f2074 	ldreq	r2, [pc, #116]	; bd7c <ecc_add_stat+0x150>
    bd04:	05923028 	ldreq	r3, [r2, #40]
    bd08:	02833001 	addeq	r3, r3, #1	; 0x1
    bd0c:	05823028 	streq	r3, [r2, #40]
    bd10:	0a000013 	beq	bd64 <ecc_add_stat+0x138>
    bd14:	e355007f 	cmp	r5, #127	; 0x7f
    bd18:	d1a00006 	movle	r0, r6
    bd1c:	d28d2004 	addle	r2, sp, #4	; 0x4
    bd20:	da000005 	ble	bd3c <ecc_add_stat+0x110>
    bd24:	e59f0054 	ldr	r0, [pc, #84]	; bd80 <ecc_add_stat+0x154>
    bd28:	e1a01005 	mov	r1, r5
    bd2c:	e59f3050 	ldr	r3, [pc, #80]	; bd84 <ecc_add_stat+0x158>
    bd30:	e1a0e00f 	mov	lr, pc
    bd34:	e593f000 	ldr	pc, [r3]
    bd38:	ea000009 	b	bd64 <ecc_add_stat+0x138>
    bd3c:	e7923100 	ldr	r3, [r2, r0, lsl #2]
    bd40:	e1550003 	cmp	r5, r3
    bd44:	da000002 	ble	bd54 <ecc_add_stat+0x128>
    bd48:	e2800001 	add	r0, r0, #1	; 0x1
    bd4c:	e3500009 	cmp	r0, #9	; 0x9
    bd50:	1afffff9 	bne	bd3c <ecc_add_stat+0x110>
    bd54:	e59f2020 	ldr	r2, [pc, #32]	; bd7c <ecc_add_stat+0x150>
    bd58:	e7923100 	ldr	r3, [r2, r0, lsl #2]
    bd5c:	e2833001 	add	r3, r3, #1	; 0x1
    bd60:	e7823100 	str	r3, [r2, r0, lsl #2]
    bd64:	e28dd030 	add	sp, sp, #48	; 0x30
    bd68:	e8bd8070 	pop	{r4, r5, r6, pc}
    bd6c:	00015d24 	.word	0x00015d24
    bd70:	00015d1c 	.word	0x00015d1c
    bd74:	3c00d204 	.word	0x3c00d204
    bd78:	00018521 	.word	0x00018521
    bd7c:	3c00d1d4 	.word	0x3c00d1d4
    bd80:	0001854c 	.word	0x0001854c
    bd84:	3c00d6cc 	.word	0x3c00d6cc

0000bd88 <ecc_print_stat>:
    bd88:	e92d4070 	push	{r4, r5, r6, lr}
    bd8c:	e59f1204 	ldr	r1, [pc, #516]	; bf98 <ecc_print_stat+0x210>
    bd90:	e3a06000 	mov	r6, #0	; 0x0
    bd94:	e1a02006 	mov	r2, r6
    bd98:	e7913002 	ldr	r3, [r1, r2]
    bd9c:	e2822004 	add	r2, r2, #4	; 0x4
    bda0:	e352002c 	cmp	r2, #44	; 0x2c
    bda4:	e0866003 	add	r6, r6, r3
    bda8:	e59f41e8 	ldr	r4, [pc, #488]	; bf98 <ecc_print_stat+0x210>
    bdac:	1afffff9 	bne	bd98 <ecc_print_stat+0x10>
    bdb0:	e3560063 	cmp	r6, #99	; 0x63
    bdb4:	e59f51e0 	ldr	r5, [pc, #480]	; bf9c <ecc_print_stat+0x214>
    bdb8:	ca000003 	bgt	bdcc <ecc_print_stat+0x44>
    bdbc:	e59f01dc 	ldr	r0, [pc, #476]	; bfa0 <ecc_print_stat+0x218>
    bdc0:	e1a0e00f 	mov	lr, pc
    bdc4:	e595f000 	ldr	pc, [r5]
    bdc8:	e8bd8070 	pop	{r4, r5, r6, pc}
    bdcc:	e59f01d0 	ldr	r0, [pc, #464]	; bfa4 <ecc_print_stat+0x21c>
    bdd0:	e1a0e00f 	mov	lr, pc
    bdd4:	e595f000 	ldr	pc, [r5]
    bdd8:	e59f01c8 	ldr	r0, [pc, #456]	; bfa8 <ecc_print_stat+0x220>
    bddc:	e1a0e00f 	mov	lr, pc
    bde0:	e595f000 	ldr	pc, [r5]
    bde4:	e5941028 	ldr	r1, [r4, #40]
    bde8:	e59f01bc 	ldr	r0, [pc, #444]	; bfac <ecc_print_stat+0x224>
    bdec:	e1a0e00f 	mov	lr, pc
    bdf0:	e595f000 	ldr	pc, [r5]
    bdf4:	e5941000 	ldr	r1, [r4]
    bdf8:	e59f01b0 	ldr	r0, [pc, #432]	; bfb0 <ecc_print_stat+0x228>
    bdfc:	e1a0e00f 	mov	lr, pc
    be00:	e595f000 	ldr	pc, [r5]
    be04:	e5941004 	ldr	r1, [r4, #4]
    be08:	e59f01a4 	ldr	r0, [pc, #420]	; bfb4 <ecc_print_stat+0x22c>
    be0c:	e1a0e00f 	mov	lr, pc
    be10:	e595f000 	ldr	pc, [r5]
    be14:	e5941008 	ldr	r1, [r4, #8]
    be18:	e59f0198 	ldr	r0, [pc, #408]	; bfb8 <ecc_print_stat+0x230>
    be1c:	e1a0e00f 	mov	lr, pc
    be20:	e595f000 	ldr	pc, [r5]
    be24:	e594100c 	ldr	r1, [r4, #12]
    be28:	e59f018c 	ldr	r0, [pc, #396]	; bfbc <ecc_print_stat+0x234>
    be2c:	e1a0e00f 	mov	lr, pc
    be30:	e595f000 	ldr	pc, [r5]
    be34:	e5941010 	ldr	r1, [r4, #16]
    be38:	e59f0180 	ldr	r0, [pc, #384]	; bfc0 <ecc_print_stat+0x238>
    be3c:	e1a0e00f 	mov	lr, pc
    be40:	e595f000 	ldr	pc, [r5]
    be44:	e5941014 	ldr	r1, [r4, #20]
    be48:	e59f0174 	ldr	r0, [pc, #372]	; bfc4 <ecc_print_stat+0x23c>
    be4c:	e1a0e00f 	mov	lr, pc
    be50:	e595f000 	ldr	pc, [r5]
    be54:	e5941018 	ldr	r1, [r4, #24]
    be58:	e59f0168 	ldr	r0, [pc, #360]	; bfc8 <ecc_print_stat+0x240>
    be5c:	e1a0e00f 	mov	lr, pc
    be60:	e595f000 	ldr	pc, [r5]
    be64:	e594101c 	ldr	r1, [r4, #28]
    be68:	e59f015c 	ldr	r0, [pc, #348]	; bfcc <ecc_print_stat+0x244>
    be6c:	e1a0e00f 	mov	lr, pc
    be70:	e595f000 	ldr	pc, [r5]
    be74:	e5941020 	ldr	r1, [r4, #32]
    be78:	e59f0150 	ldr	r0, [pc, #336]	; bfd0 <ecc_print_stat+0x248>
    be7c:	e1a0e00f 	mov	lr, pc
    be80:	e595f000 	ldr	pc, [r5]
    be84:	e5941024 	ldr	r1, [r4, #36]
    be88:	e59f0144 	ldr	r0, [pc, #324]	; bfd4 <ecc_print_stat+0x24c>
    be8c:	e1a0e00f 	mov	lr, pc
    be90:	e595f000 	ldr	pc, [r5]
    be94:	e59f010c 	ldr	r0, [pc, #268]	; bfa8 <ecc_print_stat+0x220>
    be98:	e1a0e00f 	mov	lr, pc
    be9c:	e595f000 	ldr	pc, [r5]
    bea0:	e1a00006 	mov	r0, r6
    bea4:	e3a01064 	mov	r1, #100	; 0x64
    bea8:	eb0022bb 	bl	1499c <__aeabi_idiv>
    beac:	e5944028 	ldr	r4, [r4, #40]
    beb0:	e1a01000 	mov	r1, r0
    beb4:	e0644006 	rsb	r4, r4, r6
    beb8:	e1a00004 	mov	r0, r4
    bebc:	eb0022b6 	bl	1499c <__aeabi_idiv>
    bec0:	e59f4110 	ldr	r4, [pc, #272]	; bfd8 <ecc_print_stat+0x250>
    bec4:	e1a02546 	asr	r2, r6, #10
    bec8:	e1a03000 	mov	r3, r0
    becc:	e1a01006 	mov	r1, r6
    bed0:	e59f0104 	ldr	r0, [pc, #260]	; bfdc <ecc_print_stat+0x254>
    bed4:	e1a0e00f 	mov	lr, pc
    bed8:	e595f000 	ldr	pc, [r5]
    bedc:	e894000c 	ldm	r4, {r2, r3}
    bee0:	e0833002 	add	r3, r3, r2
    bee4:	e5942008 	ldr	r2, [r4, #8]
    bee8:	e594100c 	ldr	r1, [r4, #12]
    beec:	e0833002 	add	r3, r3, r2
    bef0:	e0836001 	add	r6, r3, r1
    bef4:	e3560063 	cmp	r6, #99	; 0x63
    bef8:	d8bd8070 	pople	{r4, r5, r6, pc}
    befc:	e59f00dc 	ldr	r0, [pc, #220]	; bfe0 <ecc_print_stat+0x258>
    bf00:	e1a0e00f 	mov	lr, pc
    bf04:	e595f000 	ldr	pc, [r5]
    bf08:	e59f0098 	ldr	r0, [pc, #152]	; bfa8 <ecc_print_stat+0x220>
    bf0c:	e1a0e00f 	mov	lr, pc
    bf10:	e595f000 	ldr	pc, [r5]
    bf14:	e594100c 	ldr	r1, [r4, #12]
    bf18:	e59f008c 	ldr	r0, [pc, #140]	; bfac <ecc_print_stat+0x224>
    bf1c:	e1a0e00f 	mov	lr, pc
    bf20:	e595f000 	ldr	pc, [r5]
    bf24:	e5941000 	ldr	r1, [r4]
    bf28:	e59f00b4 	ldr	r0, [pc, #180]	; bfe4 <ecc_print_stat+0x25c>
    bf2c:	e1a0e00f 	mov	lr, pc
    bf30:	e595f000 	ldr	pc, [r5]
    bf34:	e5941004 	ldr	r1, [r4, #4]
    bf38:	e59f0078 	ldr	r0, [pc, #120]	; bfb8 <ecc_print_stat+0x230>
    bf3c:	e1a0e00f 	mov	lr, pc
    bf40:	e595f000 	ldr	pc, [r5]
    bf44:	e5941008 	ldr	r1, [r4, #8]
    bf48:	e59f0084 	ldr	r0, [pc, #132]	; bfd4 <ecc_print_stat+0x24c>
    bf4c:	e1a0e00f 	mov	lr, pc
    bf50:	e595f000 	ldr	pc, [r5]
    bf54:	e59f004c 	ldr	r0, [pc, #76]	; bfa8 <ecc_print_stat+0x220>
    bf58:	e1a0e00f 	mov	lr, pc
    bf5c:	e595f000 	ldr	pc, [r5]
    bf60:	e1a00006 	mov	r0, r6
    bf64:	e3a01064 	mov	r1, #100	; 0x64
    bf68:	eb00228b 	bl	1499c <__aeabi_idiv>
    bf6c:	e594400c 	ldr	r4, [r4, #12]
    bf70:	e1a01000 	mov	r1, r0
    bf74:	e0644006 	rsb	r4, r4, r6
    bf78:	e1a00004 	mov	r0, r4
    bf7c:	eb002286 	bl	1499c <__aeabi_idiv>
    bf80:	e1a01006 	mov	r1, r6
    bf84:	e1a02000 	mov	r2, r0
    bf88:	e59f0058 	ldr	r0, [pc, #88]	; bfe8 <ecc_print_stat+0x260>
    bf8c:	e1a0e00f 	mov	lr, pc
    bf90:	e595f000 	ldr	pc, [r5]
    bf94:	e8bd8070 	pop	{r4, r5, r6, pc}
    bf98:	3c00d1d4 	.word	0x3c00d1d4
    bf9c:	3c00d6cc 	.word	0x3c00d6cc
    bfa0:	00018577 	.word	0x00018577
    bfa4:	0001858b 	.word	0x0001858b
    bfa8:	000185a4 	.word	0x000185a4
    bfac:	000185c2 	.word	0x000185c2
    bfb0:	000185d1 	.word	0x000185d1
    bfb4:	000185e0 	.word	0x000185e0
    bfb8:	000185ef 	.word	0x000185ef
    bfbc:	000185fe 	.word	0x000185fe
    bfc0:	0001860d 	.word	0x0001860d
    bfc4:	0001861c 	.word	0x0001861c
    bfc8:	0001862b 	.word	0x0001862b
    bfcc:	0001863a 	.word	0x0001863a
    bfd0:	00018649 	.word	0x00018649
    bfd4:	00018658 	.word	0x00018658
    bfd8:	3c00d204 	.word	0x3c00d204
    bfdc:	00018667 	.word	0x00018667
    bfe0:	0001869b 	.word	0x0001869b
    bfe4:	000186b5 	.word	0x000186b5
    bfe8:	000186c4 	.word	0x000186c4

0000bfec <nf2_set_busw>:
    bfec:	e3500010 	cmp	r0, #16	; 0x10
    bff0:	03a02001 	moveq	r2, #1	; 0x1
    bff4:	13a02000 	movne	r2, #0	; 0x0
    bff8:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    bffc:	e58321b8 	str	r2, [r3, #440]
    c000:	e3a00000 	mov	r0, #0	; 0x0
    c004:	e12fff1e 	bx	lr

0000c008 <nf2_ecc_cap>:
    c008:	e350000f 	cmp	r0, #15	; 0xf
    c00c:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    c010:	ea000011 	b	c05c <nf2_ecc_cap+0x54>
    c014:	0000c054 	.word	0x0000c054
    c018:	0000c064 	.word	0x0000c064
    c01c:	0000c06c 	.word	0x0000c06c
    c020:	0000c074 	.word	0x0000c074
    c024:	0000c07c 	.word	0x0000c07c
    c028:	0000c084 	.word	0x0000c084
    c02c:	0000c08c 	.word	0x0000c08c
    c030:	0000c094 	.word	0x0000c094
    c034:	0000c09c 	.word	0x0000c09c
    c038:	0000c0a4 	.word	0x0000c0a4
    c03c:	0000c0ac 	.word	0x0000c0ac
    c040:	0000c0b4 	.word	0x0000c0b4
    c044:	0000c0bc 	.word	0x0000c0bc
    c048:	0000c0c4 	.word	0x0000c0c4
    c04c:	0000c0cc 	.word	0x0000c0cc
    c050:	0000c0d4 	.word	0x0000c0d4
    c054:	e3a00002 	mov	r0, #2	; 0x2
    c058:	e12fff1e 	bx	lr
    c05c:	e3a00000 	mov	r0, #0	; 0x0
    c060:	e12fff1e 	bx	lr
    c064:	e3a00004 	mov	r0, #4	; 0x4
    c068:	e12fff1e 	bx	lr
    c06c:	e3a00008 	mov	r0, #8	; 0x8
    c070:	e12fff1e 	bx	lr
    c074:	e3a00010 	mov	r0, #16	; 0x10
    c078:	e12fff1e 	bx	lr
    c07c:	e3a00018 	mov	r0, #24	; 0x18
    c080:	e12fff1e 	bx	lr
    c084:	e3a00020 	mov	r0, #32	; 0x20
    c088:	e12fff1e 	bx	lr
    c08c:	e3a00028 	mov	r0, #40	; 0x28
    c090:	e12fff1e 	bx	lr
    c094:	e3a00030 	mov	r0, #48	; 0x30
    c098:	e12fff1e 	bx	lr
    c09c:	e3a00038 	mov	r0, #56	; 0x38
    c0a0:	e12fff1e 	bx	lr
    c0a4:	e3a0003c 	mov	r0, #60	; 0x3c
    c0a8:	e12fff1e 	bx	lr
    c0ac:	e3a00040 	mov	r0, #64	; 0x40
    c0b0:	e12fff1e 	bx	lr
    c0b4:	e3a00048 	mov	r0, #72	; 0x48
    c0b8:	e12fff1e 	bx	lr
    c0bc:	e3a00050 	mov	r0, #80	; 0x50
    c0c0:	e12fff1e 	bx	lr
    c0c4:	e3a00060 	mov	r0, #96	; 0x60
    c0c8:	e12fff1e 	bx	lr
    c0cc:	e3a00070 	mov	r0, #112	; 0x70
    c0d0:	e12fff1e 	bx	lr
    c0d4:	e3a0007f 	mov	r0, #127	; 0x7f
    c0d8:	e12fff1e 	bx	lr

0000c0dc <trx_afifo_reg>:
    c0dc:	e2503000 	subs	r3, r0, #0	; 0x0
    c0e0:	e1a00002 	mov	r0, r2
    c0e4:	1a000008 	bne	c10c <trx_afifo_reg+0x30>
    c0e8:	e1a02802 	lsl	r2, r2, #16
    c0ec:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c0f0:	e3811a0f 	orr	r1, r1, #61440	; 0xf000
    c0f4:	e1a02822 	lsr	r2, r2, #16
    c0f8:	e1a00820 	lsr	r0, r0, #16
    c0fc:	e5831060 	str	r1, [r3, #96]
    c100:	e5832060 	str	r2, [r3, #96]
    c104:	e5830060 	str	r0, [r3, #96]
    c108:	e12fff1e 	bx	lr
    c10c:	e3530001 	cmp	r3, #1	; 0x1
    c110:	01a03802 	lsleq	r3, r2, #16
    c114:	11813403 	orrne	r3, r1, r3, lsl #8
    c118:	03a0230a 	moveq	r2, #671088640	; 0x28000000
    c11c:	03811cf1 	orreq	r1, r1, #61696	; 0xf100
    c120:	01a03823 	lsreq	r3, r3, #16
    c124:	13833a0f 	orrne	r3, r3, #61440	; 0xf000
    c128:	13a0230a 	movne	r2, #671088640	; 0x28000000
    c12c:	05821060 	streq	r1, [r2, #96]
    c130:	e5823060 	str	r3, [r2, #96]
    c134:	e12fff1e 	bx	lr

0000c138 <trx_afifo_enable>:
    c138:	e24dd008 	sub	sp, sp, #8	; 0x8
    c13c:	e3a03000 	mov	r3, #0	; 0x0
    c140:	e58d3004 	str	r3, [sp, #4]
    c144:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c148:	e5923054 	ldr	r3, [r2, #84]
    c14c:	e58d3004 	str	r3, [sp, #4]
    c150:	e59d3004 	ldr	r3, [sp, #4]
    c154:	e3833001 	orr	r3, r3, #1	; 0x1
    c158:	e58d3004 	str	r3, [sp, #4]
    c15c:	e59d3004 	ldr	r3, [sp, #4]
    c160:	e5823054 	str	r3, [r2, #84]
    c164:	e28dd008 	add	sp, sp, #8	; 0x8
    c168:	e12fff1e 	bx	lr

0000c16c <trx_afifo_disable>:
    c16c:	e3a02000 	mov	r2, #0	; 0x0
    c170:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c174:	e5832054 	str	r2, [r3, #84]
    c178:	e12fff1e 	bx	lr

0000c17c <trx_nand_cs>:
    c17c:	e3500001 	cmp	r0, #1	; 0x1
    c180:	03e02001 	mvneq	r2, #1	; 0x1
    c184:	13a0200f 	movne	r2, #15	; 0xf
    c188:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c18c:	e583204c 	str	r2, [r3, #76]
    c190:	e12fff1e 	bx	lr

0000c194 <trx_afifo_start>:
    c194:	e3a02001 	mov	r2, #1	; 0x1
    c198:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c19c:	e5832084 	str	r2, [r3, #132]
    c1a0:	e12fff1e 	bx	lr

0000c1a4 <trx_afifo_is_finish>:
    c1a4:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c1a8:	e593308c 	ldr	r3, [r3, #140]
    c1ac:	e0000003 	and	r0, r0, r3
    c1b0:	e12fff1e 	bx	lr

0000c1b4 <trx_afifo_intr_clear>:
    c1b4:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    c1b8:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c1bc:	e583208c 	str	r2, [r3, #140]
    c1c0:	e12fff1e 	bx	lr

0000c1c4 <trx_afifo_read_state>:
    c1c4:	e3500003 	cmp	r0, #3	; 0x3
    c1c8:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    c1cc:	ea000003 	b	c1e0 <trx_afifo_read_state+0x1c>
    c1d0:	0000c1e8 	.word	0x0000c1e8
    c1d4:	0000c1f4 	.word	0x0000c1f4
    c1d8:	0000c200 	.word	0x0000c200
    c1dc:	0000c20c 	.word	0x0000c20c
    c1e0:	e3e00000 	mvn	r0, #0	; 0x0
    c1e4:	e12fff1e 	bx	lr
    c1e8:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c1ec:	e5930010 	ldr	r0, [r3, #16]
    c1f0:	e12fff1e 	bx	lr
    c1f4:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c1f8:	e5930014 	ldr	r0, [r3, #20]
    c1fc:	e12fff1e 	bx	lr
    c200:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c204:	e59300e0 	ldr	r0, [r3, #224]
    c208:	e12fff1e 	bx	lr
    c20c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c210:	e59300e4 	ldr	r0, [r3, #228]
    c214:	e12fff1e 	bx	lr

0000c218 <trx_afifo_etd>:
    c218:	e1811080 	orr	r1, r1, r0, lsl #1
    c21c:	e3811902 	orr	r1, r1, #32768	; 0x8000
    c220:	e1811102 	orr	r1, r1, r2, lsl #2
    c224:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c228:	e5831060 	str	r1, [r3, #96]
    c22c:	e12fff1e 	bx	lr

0000c230 <cpu_afifo_disable>:
    c230:	e24dd008 	sub	sp, sp, #8	; 0x8
    c234:	e3a03000 	mov	r3, #0	; 0x0
    c238:	e58d3004 	str	r3, [sp, #4]
    c23c:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c240:	e5923054 	ldr	r3, [r2, #84]
    c244:	e58d3004 	str	r3, [sp, #4]
    c248:	e59d3004 	ldr	r3, [sp, #4]
    c24c:	e3c33002 	bic	r3, r3, #2	; 0x2
    c250:	e58d3004 	str	r3, [sp, #4]
    c254:	e59d3004 	ldr	r3, [sp, #4]
    c258:	e5823054 	str	r3, [r2, #84]
    c25c:	e28dd008 	add	sp, sp, #8	; 0x8
    c260:	e12fff1e 	bx	lr

0000c264 <cpu_afifo_enable>:
    c264:	e24dd008 	sub	sp, sp, #8	; 0x8
    c268:	e3a03000 	mov	r3, #0	; 0x0
    c26c:	e58d3004 	str	r3, [sp, #4]
    c270:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c274:	e5923054 	ldr	r3, [r2, #84]
    c278:	e58d3004 	str	r3, [sp, #4]
    c27c:	e59d3004 	ldr	r3, [sp, #4]
    c280:	e3833002 	orr	r3, r3, #2	; 0x2
    c284:	e58d3004 	str	r3, [sp, #4]
    c288:	e59d3004 	ldr	r3, [sp, #4]
    c28c:	e5823054 	str	r3, [r2, #84]
    c290:	e28dd008 	add	sp, sp, #8	; 0x8
    c294:	e12fff1e 	bx	lr

0000c298 <trx_afifo_nop>:
    c298:	e1a01b01 	lsl	r1, r1, #22
    c29c:	e1a01b21 	lsr	r1, r1, #22
    c2a0:	e200000f 	and	r0, r0, #15	; 0xf
    c2a4:	e1811500 	orr	r1, r1, r0, lsl #10
    c2a8:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c2ac:	e5831060 	str	r1, [r3, #96]
    c2b0:	e12fff1e 	bx	lr

0000c2b4 <trx_afifo_ncmd>:
    c2b4:	e200000f 	and	r0, r0, #15	; 0xf
    c2b8:	e1a01401 	lsl	r1, r1, #8
    c2bc:	e1811500 	orr	r1, r1, r0, lsl #10
    c2c0:	e3811901 	orr	r1, r1, #16384	; 0x4000
    c2c4:	e1811002 	orr	r1, r1, r2
    c2c8:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c2cc:	e5831060 	str	r1, [r3, #96]
    c2d0:	e12fff1e 	bx	lr

0000c2d4 <nf2_secc_cap>:
    c2d4:	e3500000 	cmp	r0, #0	; 0x0
    c2d8:	0a000003 	beq	c2ec <nf2_secc_cap+0x18>
    c2dc:	e3500001 	cmp	r0, #1	; 0x1
    c2e0:	03a00008 	moveq	r0, #8	; 0x8
    c2e4:	13a00000 	movne	r0, #0	; 0x0
    c2e8:	e12fff1e 	bx	lr
    c2ec:	e3a00004 	mov	r0, #4	; 0x4
    c2f0:	e12fff1e 	bx	lr

0000c2f4 <nf2_secc_cfg>:
    c2f4:	e3a0c30a 	mov	ip, #671088640	; 0x28000000
    c2f8:	e59c3018 	ldr	r3, [ip, #24]
    c2fc:	e24dd008 	sub	sp, sp, #8	; 0x8
    c300:	e58d3004 	str	r3, [sp, #4]
    c304:	e1a00800 	lsl	r0, r0, #16
    c308:	e59d3004 	ldr	r3, [sp, #4]
    c30c:	e1800881 	orr	r0, r0, r1, lsl #17
    c310:	e1800182 	orr	r0, r0, r2, lsl #3
    c314:	e1800003 	orr	r0, r0, r3
    c318:	e58d0004 	str	r0, [sp, #4]
    c31c:	e59d3004 	ldr	r3, [sp, #4]
    c320:	e3510000 	cmp	r1, #0	; 0x0
    c324:	e58c3018 	str	r3, [ip, #24]
    c328:	03a01004 	moveq	r1, #4	; 0x4
    c32c:	0a000002 	beq	c33c <nf2_secc_cfg+0x48>
    c330:	e3510001 	cmp	r1, #1	; 0x1
    c334:	03a01008 	moveq	r1, #8	; 0x8
    c338:	13a01000 	movne	r1, #0	; 0x0
    c33c:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c340:	e592311c 	ldr	r3, [r2, #284]
    c344:	e58d3004 	str	r3, [sp, #4]
    c348:	e59d3004 	ldr	r3, [sp, #4]
    c34c:	e3c3380f 	bic	r3, r3, #983040	; 0xf0000
    c350:	e58d3004 	str	r3, [sp, #4]
    c354:	e59d3004 	ldr	r3, [sp, #4]
    c358:	e1833801 	orr	r3, r3, r1, lsl #16
    c35c:	e58d3004 	str	r3, [sp, #4]
    c360:	e59d3004 	ldr	r3, [sp, #4]
    c364:	e582311c 	str	r3, [r2, #284]
    c368:	e28dd008 	add	sp, sp, #8	; 0x8
    c36c:	e12fff1e 	bx	lr

0000c370 <nf2_ecc_cfg>:
    c370:	e380cc03 	orr	ip, r0, #768	; 0x300
    c374:	e18cc581 	orr	ip, ip, r1, lsl #11
    c378:	e92d4010 	push	{r4, lr}
    c37c:	e18cc202 	orr	ip, ip, r2, lsl #4
    c380:	e18cc603 	orr	ip, ip, r3, lsl #12
    c384:	e59d3010 	ldr	r3, [sp, #16]
    c388:	e59d100c 	ldr	r1, [sp, #12]
    c38c:	e18cc083 	orr	ip, ip, r3, lsl #1
    c390:	e59d3014 	ldr	r3, [sp, #20]
    c394:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    c398:	e18cc903 	orr	ip, ip, r3, lsl #18
    c39c:	e59d3008 	ldr	r3, [sp, #8]
    c3a0:	e1a00002 	mov	r0, r2
    c3a4:	e1811803 	orr	r1, r1, r3, lsl #16
    c3a8:	e584c018 	str	ip, [r4, #24]
    c3ac:	e584101c 	str	r1, [r4, #28]
    c3b0:	ebffff14 	bl	c008 <nf2_ecc_cap>
    c3b4:	e594311c 	ldr	r3, [r4, #284]
    c3b8:	e3c33c7f 	bic	r3, r3, #32512	; 0x7f00
    c3bc:	e1833400 	orr	r3, r3, r0, lsl #8
    c3c0:	e584311c 	str	r3, [r4, #284]
    c3c4:	e8bd8010 	pop	{r4, pc}

0000c3c8 <nf2_ecc_num>:
    c3c8:	e1a01541 	asr	r1, r1, #10
    c3cc:	e350000f 	cmp	r0, #15	; 0xf
    c3d0:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
    c3d4:	ea00000f 	b	c418 <nf2_ecc_num+0x50>
    c3d8:	0000c420 	.word	0x0000c420
    c3dc:	0000c428 	.word	0x0000c428
    c3e0:	0000c430 	.word	0x0000c430
    c3e4:	0000c438 	.word	0x0000c438
    c3e8:	0000c440 	.word	0x0000c440
    c3ec:	0000c448 	.word	0x0000c448
    c3f0:	0000c450 	.word	0x0000c450
    c3f4:	0000c458 	.word	0x0000c458
    c3f8:	0000c460 	.word	0x0000c460
    c3fc:	0000c468 	.word	0x0000c468
    c400:	0000c470 	.word	0x0000c470
    c404:	0000c478 	.word	0x0000c478
    c408:	0000c480 	.word	0x0000c480
    c40c:	0000c488 	.word	0x0000c488
    c410:	0000c490 	.word	0x0000c490
    c414:	0000c498 	.word	0x0000c498
    c418:	e3a00000 	mov	r0, #0	; 0x0
    c41c:	ea00001f 	b	c4a0 <nf2_ecc_num+0xd8>
    c420:	e1a00101 	lsl	r0, r1, #2
    c424:	ea00001d 	b	c4a0 <nf2_ecc_num+0xd8>
    c428:	e1a00181 	lsl	r0, r1, #3
    c42c:	ea00001b 	b	c4a0 <nf2_ecc_num+0xd8>
    c430:	e1a00201 	lsl	r0, r1, #4
    c434:	ea000019 	b	c4a0 <nf2_ecc_num+0xd8>
    c438:	e3a0301c 	mov	r3, #28	; 0x1c
    c43c:	ea000016 	b	c49c <nf2_ecc_num+0xd4>
    c440:	e3a0302c 	mov	r3, #44	; 0x2c
    c444:	ea000014 	b	c49c <nf2_ecc_num+0xd4>
    c448:	e3a03038 	mov	r3, #56	; 0x38
    c44c:	ea000012 	b	c49c <nf2_ecc_num+0xd4>
    c450:	e3a03048 	mov	r3, #72	; 0x48
    c454:	ea000010 	b	c49c <nf2_ecc_num+0xd4>
    c458:	e3a03054 	mov	r3, #84	; 0x54
    c45c:	ea00000e 	b	c49c <nf2_ecc_num+0xd4>
    c460:	e3a03064 	mov	r3, #100	; 0x64
    c464:	ea00000c 	b	c49c <nf2_ecc_num+0xd4>
    c468:	e3a0306c 	mov	r3, #108	; 0x6c
    c46c:	ea00000a 	b	c49c <nf2_ecc_num+0xd4>
    c470:	e3a03070 	mov	r3, #112	; 0x70
    c474:	ea000008 	b	c49c <nf2_ecc_num+0xd4>
    c478:	e1a00381 	lsl	r0, r1, #7
    c47c:	ea000007 	b	c4a0 <nf2_ecc_num+0xd8>
    c480:	e3a0308c 	mov	r3, #140	; 0x8c
    c484:	ea000004 	b	c49c <nf2_ecc_num+0xd4>
    c488:	e3a030a8 	mov	r3, #168	; 0xa8
    c48c:	ea000002 	b	c49c <nf2_ecc_num+0xd4>
    c490:	e3a030c4 	mov	r3, #196	; 0xc4
    c494:	ea000000 	b	c49c <nf2_ecc_num+0xd4>
    c498:	e3a030e0 	mov	r3, #224	; 0xe0
    c49c:	e0000193 	mul	r0, r3, r1
    c4a0:	e3520001 	cmp	r2, #1	; 0x1
    c4a4:	01a00080 	lsleq	r0, r0, #1
    c4a8:	e12fff1e 	bx	lr

0000c4ac <nf2_addr_cfg>:
    c4ac:	e3530010 	cmp	r3, #16	; 0x10
    c4b0:	01a010c1 	asreq	r1, r1, #1
    c4b4:	01a020c2 	asreq	r2, r2, #1
    c4b8:	e1813802 	orr	r3, r1, r2, lsl #16
    c4bc:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c4c0:	e3a01000 	mov	r1, #0	; 0x0
    c4c4:	e5820020 	str	r0, [r2, #32]
    c4c8:	e5821024 	str	r1, [r2, #36]
    c4cc:	e5821028 	str	r1, [r2, #40]
    c4d0:	e582102c 	str	r1, [r2, #44]
    c4d4:	e5823030 	str	r3, [r2, #48]
    c4d8:	e12fff1e 	bx	lr

0000c4dc <nf2_sdma_cfg>:
    c4dc:	e59dc000 	ldr	ip, [sp]
    c4e0:	e1813803 	orr	r3, r1, r3, lsl #16
    c4e4:	e1a0c10c 	lsl	ip, ip, #2
    c4e8:	e3a0130a 	mov	r1, #671088640	; 0x28000000
    c4ec:	e5810038 	str	r0, [r1, #56]
    c4f0:	e38cc001 	orr	ip, ip, #1	; 0x1
    c4f4:	e581203c 	str	r2, [r1, #60]
    c4f8:	e5813040 	str	r3, [r1, #64]
    c4fc:	e3a03341 	mov	r3, #67108865	; 0x4000001
    c500:	e5813044 	str	r3, [r1, #68]
    c504:	e581c048 	str	ip, [r1, #72]
    c508:	e12fff1e 	bx	lr

0000c50c <nf2_check_ecc>:
    c50c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    c510:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c514:	e59230e8 	ldr	r3, [r2, #232]
    c518:	e24dd028 	sub	sp, sp, #40	; 0x28
    c51c:	e58d3000 	str	r3, [sp]
    c520:	e59230ec 	ldr	r3, [r2, #236]
    c524:	e3a0c000 	mov	ip, #0	; 0x0
    c528:	e58d3004 	str	r3, [sp, #4]
    c52c:	e59230f0 	ldr	r3, [r2, #240]
    c530:	e1a06001 	mov	r6, r1
    c534:	e58d3008 	str	r3, [sp, #8]
    c538:	e59230f4 	ldr	r3, [r2, #244]
    c53c:	e1a0400c 	mov	r4, ip
    c540:	e58d300c 	str	r3, [sp, #12]
    c544:	e59230f8 	ldr	r3, [r2, #248]
    c548:	e1a08540 	asr	r8, r0, #10
    c54c:	e58d3010 	str	r3, [sp, #16]
    c550:	e59230fc 	ldr	r3, [r2, #252]
    c554:	e1a0500c 	mov	r5, ip
    c558:	e58d3014 	str	r3, [sp, #20]
    c55c:	e5923100 	ldr	r3, [r2, #256]
    c560:	e58d3018 	str	r3, [sp, #24]
    c564:	e5923104 	ldr	r3, [r2, #260]
    c568:	e58d301c 	str	r3, [sp, #28]
    c56c:	e5927108 	ldr	r7, [r2, #264]
    c570:	e581c000 	str	ip, [r1]
    c574:	e58d7020 	str	r7, [sp, #32]
    c578:	ea000028 	b	c620 <nf2_check_ecc+0x114>
    c57c:	e1a03457 	asr	r3, r7, r4
    c580:	e3130001 	tst	r3, #1	; 0x1
    c584:	0a000004 	beq	c59c <nf2_check_ecc+0x90>
    c588:	e3a00000 	mov	r0, #0	; 0x0
    c58c:	e3e01000 	mvn	r1, #0	; 0x0
    c590:	ebfffda5 	bl	bc2c <ecc_add_stat>
    c594:	e3e00000 	mvn	r0, #0	; 0x0
    c598:	ea000024 	b	c630 <nf2_check_ecc+0x124>
    c59c:	e1a03144 	asr	r3, r4, #2
    c5a0:	e3520003 	cmp	r2, #3	; 0x3
    c5a4:	979ff102 	ldrls	pc, [pc, r2, lsl #2]
    c5a8:	ea000015 	b	c604 <nf2_check_ecc+0xf8>
    c5ac:	0000c5bc 	.word	0x0000c5bc
    c5b0:	0000c5cc 	.word	0x0000c5cc
    c5b4:	0000c5e4 	.word	0x0000c5e4
    c5b8:	0000c5f4 	.word	0x0000c5f4
    c5bc:	e28d2028 	add	r2, sp, #40	; 0x28
    c5c0:	e0823103 	add	r3, r2, r3, lsl #2
    c5c4:	e5133028 	ldr	r3, [r3, #-40]
    c5c8:	ea00000c 	b	c600 <nf2_check_ecc+0xf4>
    c5cc:	e28d2028 	add	r2, sp, #40	; 0x28
    c5d0:	e0823103 	add	r3, r2, r3, lsl #2
    c5d4:	e5133028 	ldr	r3, [r3, #-40]
    c5d8:	e2033c7f 	and	r3, r3, #32512	; 0x7f00
    c5dc:	e1a05443 	asr	r5, r3, #8
    c5e0:	ea000007 	b	c604 <nf2_check_ecc+0xf8>
    c5e4:	e28d2028 	add	r2, sp, #40	; 0x28
    c5e8:	e0823103 	add	r3, r2, r3, lsl #2
    c5ec:	e15332b6 	ldrh	r3, [r3, #-38]
    c5f0:	ea000002 	b	c600 <nf2_check_ecc+0xf4>
    c5f4:	e28d2028 	add	r2, sp, #40	; 0x28
    c5f8:	e0823103 	add	r3, r2, r3, lsl #2
    c5fc:	e5533025 	ldrb	r3, [r3, #-37]
    c600:	e203507f 	and	r5, r3, #127	; 0x7f
    c604:	e3a00000 	mov	r0, #0	; 0x0
    c608:	e1a01005 	mov	r1, r5
    c60c:	ebfffd86 	bl	bc2c <ecc_add_stat>
    c610:	e5963000 	ldr	r3, [r6]
    c614:	e2844001 	add	r4, r4, #1	; 0x1
    c618:	e0833005 	add	r3, r3, r5
    c61c:	e5863000 	str	r3, [r6]
    c620:	e1540008 	cmp	r4, r8
    c624:	e2042003 	and	r2, r4, #3	; 0x3
    c628:	baffffd3 	blt	c57c <nf2_check_ecc+0x70>
    c62c:	e3a00000 	mov	r0, #0	; 0x0
    c630:	e28dd028 	add	sp, sp, #40	; 0x28
    c634:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000c638 <nf2_check_secc>:
    c638:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    c63c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c640:	e593310c 	ldr	r3, [r3, #268]
    c644:	e24dd004 	sub	sp, sp, #4	; 0x4
    c648:	e3130040 	tst	r3, #64	; 0x40
    c64c:	0a000004 	beq	c664 <nf2_check_secc+0x2c>
    c650:	e3a00001 	mov	r0, #1	; 0x1
    c654:	e3e01000 	mvn	r1, #0	; 0x0
    c658:	ebfffd73 	bl	bc2c <ecc_add_stat>
    c65c:	e3e00000 	mvn	r0, #0	; 0x0
    c660:	ea000006 	b	c680 <nf2_check_secc+0x48>
    c664:	e3130080 	tst	r3, #128	; 0x80
    c668:	1203300f 	andne	r3, r3, #15	; 0xf
    c66c:	15803000 	strne	r3, [r0]
    c670:	e5901000 	ldr	r1, [r0]
    c674:	e3a00001 	mov	r0, #1	; 0x1
    c678:	ebfffd6b 	bl	bc2c <ecc_add_stat>
    c67c:	e3a00000 	mov	r0, #0	; 0x0
    c680:	e28dd004 	add	sp, sp, #4	; 0x4
    c684:	e8bd8000 	pop	{pc}

0000c688 <nf2_set_polynomial>:
    c688:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c68c:	e5830160 	str	r0, [r3, #352]
    c690:	e3a00001 	mov	r0, #1	; 0x1
    c694:	e12fff1e 	bx	lr

0000c698 <nf2_afifo_reset>:
    c698:	e92d4070 	push	{r4, r5, r6, lr}
    c69c:	e3a02000 	mov	r2, #0	; 0x0
    c6a0:	e24dd008 	sub	sp, sp, #8	; 0x8
    c6a4:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c6a8:	e58d2004 	str	r2, [sp, #4]
    c6ac:	e2822010 	add	r2, r2, #16	; 0x10
    c6b0:	e5832080 	str	r2, [r3, #128]
    c6b4:	e1a05003 	mov	r5, r3
    c6b8:	eb0004b8 	bl	d9a0 <get_ticks>
    c6bc:	e59f6034 	ldr	r6, [pc, #52]	; c6f8 <nf2_afifo_reset+0x60>
    c6c0:	e1a04000 	mov	r4, r0
    c6c4:	eb0004b5 	bl	d9a0 <get_ticks>
    c6c8:	e0640000 	rsb	r0, r4, r0
    c6cc:	e1500006 	cmp	r0, r6
    c6d0:	c3e00000 	mvngt	r0, #0	; 0x0
    c6d4:	ca000005 	bgt	c6f0 <nf2_afifo_reset+0x58>
    c6d8:	e5953080 	ldr	r3, [r5, #128]
    c6dc:	e203301f 	and	r3, r3, #31	; 0x1f
    c6e0:	e58d3004 	str	r3, [sp, #4]
    c6e4:	e59d0004 	ldr	r0, [sp, #4]
    c6e8:	e3500000 	cmp	r0, #0	; 0x0
    c6ec:	1afffff4 	bne	c6c4 <nf2_afifo_reset+0x2c>
    c6f0:	e28dd008 	add	sp, sp, #8	; 0x8
    c6f4:	e8bd8070 	pop	{r4, r5, r6, pc}
    c6f8:	0007a120 	.word	0x0007a120

0000c6fc <nf2_soft_reset>:
    c6fc:	e92d4070 	push	{r4, r5, r6, lr}
    c700:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c704:	e24dd008 	sub	sp, sp, #8	; 0x8
    c708:	e3a03000 	mov	r3, #0	; 0x0
    c70c:	e58d3004 	str	r3, [sp, #4]
    c710:	e5820080 	str	r0, [r2, #128]
    c714:	e1a05002 	mov	r5, r2
    c718:	eb0004a0 	bl	d9a0 <get_ticks>
    c71c:	e59f6034 	ldr	r6, [pc, #52]	; c758 <nf2_soft_reset+0x5c>
    c720:	e1a04000 	mov	r4, r0
    c724:	eb00049d 	bl	d9a0 <get_ticks>
    c728:	e0640000 	rsb	r0, r4, r0
    c72c:	e1500006 	cmp	r0, r6
    c730:	c3e00000 	mvngt	r0, #0	; 0x0
    c734:	ca000005 	bgt	c750 <nf2_soft_reset+0x54>
    c738:	e5953080 	ldr	r3, [r5, #128]
    c73c:	e203301f 	and	r3, r3, #31	; 0x1f
    c740:	e58d3004 	str	r3, [sp, #4]
    c744:	e59d0004 	ldr	r0, [sp, #4]
    c748:	e3500000 	cmp	r0, #0	; 0x0
    c74c:	1afffff4 	bne	c724 <nf2_soft_reset+0x28>
    c750:	e28dd008 	add	sp, sp, #8	; 0x8
    c754:	e8bd8070 	pop	{r4, r5, r6, pc}
    c758:	0007a120 	.word	0x0007a120

0000c75c <nf2_check_irq>:
    c75c:	e92d4030 	push	{r4, r5, lr}
    c760:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    c764:	e594208c 	ldr	r2, [r4, #140]
    c768:	e5943090 	ldr	r3, [r4, #144]
    c76c:	e24dd004 	sub	sp, sp, #4	; 0x4
    c770:	e0035002 	and	r5, r3, r2
    c774:	e3150701 	tst	r5, #262144	; 0x40000
    c778:	0a000006 	beq	c798 <nf2_check_irq+0x3c>
    c77c:	e5943090 	ldr	r3, [r4, #144]
    c780:	e3c55701 	bic	r5, r5, #262144	; 0x40000
    c784:	e3c33701 	bic	r3, r3, #262144	; 0x40000
    c788:	e5843090 	str	r3, [r4, #144]
    c78c:	e3a03701 	mov	r3, #262144	; 0x40000
    c790:	e584308c 	str	r3, [r4, #140]
    c794:	ea000072 	b	c964 <nf2_check_irq+0x208>
    c798:	e3150802 	tst	r5, #131072	; 0x20000
    c79c:	0a000006 	beq	c7bc <nf2_check_irq+0x60>
    c7a0:	e5943090 	ldr	r3, [r4, #144]
    c7a4:	e3c55802 	bic	r5, r5, #131072	; 0x20000
    c7a8:	e3c33802 	bic	r3, r3, #131072	; 0x20000
    c7ac:	e5843090 	str	r3, [r4, #144]
    c7b0:	e3a03802 	mov	r3, #131072	; 0x20000
    c7b4:	e584308c 	str	r3, [r4, #140]
    c7b8:	ea000069 	b	c964 <nf2_check_irq+0x208>
    c7bc:	e59f21ac 	ldr	r2, [pc, #428]	; c970 <nf2_check_irq+0x214>
    c7c0:	e59f31a8 	ldr	r3, [pc, #424]	; c970 <nf2_check_irq+0x214>
    c7c4:	e0052002 	and	r2, r5, r2
    c7c8:	e1520003 	cmp	r2, r3
    c7cc:	1a000006 	bne	c7ec <nf2_check_irq+0x90>
    c7d0:	e5943090 	ldr	r3, [r4, #144]
    c7d4:	e3c55702 	bic	r5, r5, #524288	; 0x80000
    c7d8:	e3c33702 	bic	r3, r3, #524288	; 0x80000
    c7dc:	e3c33b01 	bic	r3, r3, #1024	; 0x400
    c7e0:	e5843090 	str	r3, [r4, #144]
    c7e4:	e584208c 	str	r2, [r4, #140]
    c7e8:	ea000006 	b	c808 <nf2_check_irq+0xac>
    c7ec:	e3150b01 	tst	r5, #1024	; 0x400
    c7f0:	0a000006 	beq	c810 <nf2_check_irq+0xb4>
    c7f4:	e5943090 	ldr	r3, [r4, #144]
    c7f8:	e3c33b01 	bic	r3, r3, #1024	; 0x400
    c7fc:	e5843090 	str	r3, [r4, #144]
    c800:	e3a03b01 	mov	r3, #1024	; 0x400
    c804:	e584308c 	str	r3, [r4, #140]
    c808:	e3c55b01 	bic	r5, r5, #1024	; 0x400
    c80c:	ea000054 	b	c964 <nf2_check_irq+0x208>
    c810:	e3150a01 	tst	r5, #4096	; 0x1000
    c814:	0a000006 	beq	c834 <nf2_check_irq+0xd8>
    c818:	e59f3154 	ldr	r3, [pc, #340]	; c974 <nf2_check_irq+0x218>
    c81c:	e59f0154 	ldr	r0, [pc, #340]	; c978 <nf2_check_irq+0x21c>
    c820:	e1a0e00f 	mov	lr, pc
    c824:	e593f000 	ldr	pc, [r3]
    c828:	e3a03a01 	mov	r3, #4096	; 0x1000
    c82c:	e5843090 	str	r3, [r4, #144]
    c830:	ea00004b 	b	c964 <nf2_check_irq+0x208>
    c834:	e3150001 	tst	r5, #1	; 0x1
    c838:	13a0300f 	movne	r3, #15	; 0xf
    c83c:	1584508c 	strne	r5, [r4, #140]
    c840:	1584304c 	strne	r3, [r4, #76]
    c844:	1a000046 	bne	c964 <nf2_check_irq+0x208>
    c848:	e3150002 	tst	r5, #2	; 0x2
    c84c:	13a03002 	movne	r3, #2	; 0x2
    c850:	1584308c 	strne	r3, [r4, #140]
    c854:	13a03001 	movne	r3, #1	; 0x1
    c858:	15843088 	strne	r3, [r4, #136]
    c85c:	1a000040 	bne	c964 <nf2_check_irq+0x208>
    c860:	e3150902 	tst	r5, #32768	; 0x8000
    c864:	13a03902 	movne	r3, #32768	; 0x8000
    c868:	1584308c 	strne	r3, [r4, #140]
    c86c:	13c55902 	bicne	r5, r5, #32768	; 0x8000
    c870:	1a00003b 	bne	c964 <nf2_check_irq+0x208>
    c874:	e3150c01 	tst	r5, #256	; 0x100
    c878:	13a03c01 	movne	r3, #256	; 0x100
    c87c:	1584308c 	strne	r3, [r4, #140]
    c880:	13c55c01 	bicne	r5, r5, #256	; 0x100
    c884:	1a000036 	bne	c964 <nf2_check_irq+0x208>
    c888:	e3150c02 	tst	r5, #512	; 0x200
    c88c:	0a000007 	beq	c8b0 <nf2_check_irq+0x154>
    c890:	e3a02c02 	mov	r2, #512	; 0x200
    c894:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c898:	e583208c 	str	r2, [r3, #140]
    c89c:	e3a02039 	mov	r2, #57	; 0x39
    c8a0:	e3e0332f 	mvn	r3, #-1140850688	; 0xbc000000
    c8a4:	e5032f77 	str	r2, [r3, #-3959]
    c8a8:	e3c55c02 	bic	r5, r5, #512	; 0x200
    c8ac:	ea00002c 	b	c964 <nf2_check_irq+0x208>
    c8b0:	e3150010 	tst	r5, #16	; 0x10
    c8b4:	0a000008 	beq	c8dc <nf2_check_irq+0x180>
    c8b8:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c8bc:	e3a03010 	mov	r3, #16	; 0x10
    c8c0:	e582308c 	str	r3, [r2, #140]
    c8c4:	e592309c 	ldr	r3, [r2, #156]
    c8c8:	e3c55010 	bic	r5, r5, #16	; 0x10
    c8cc:	e5823034 	str	r3, [r2, #52]
    c8d0:	e3a03002 	mov	r3, #2	; 0x2
    c8d4:	e5823088 	str	r3, [r2, #136]
    c8d8:	ea000021 	b	c964 <nf2_check_irq+0x208>
    c8dc:	e3150080 	tst	r5, #128	; 0x80
    c8e0:	0a00000b 	beq	c914 <nf2_check_irq+0x1b8>
    c8e4:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    c8e8:	e3a03080 	mov	r3, #128	; 0x80
    c8ec:	e584308c 	str	r3, [r4, #140]
    c8f0:	e594309c 	ldr	r3, [r4, #156]
    c8f4:	e3a00008 	mov	r0, #8	; 0x8
    c8f8:	e0833000 	add	r3, r3, r0
    c8fc:	e5843034 	str	r3, [r4, #52]
    c900:	ebffff7d 	bl	c6fc <nf2_soft_reset>
    c904:	e3a03002 	mov	r3, #2	; 0x2
    c908:	e5843088 	str	r3, [r4, #136]
    c90c:	e3c55080 	bic	r5, r5, #128	; 0x80
    c910:	ea000013 	b	c964 <nf2_check_irq+0x208>
    c914:	e3150040 	tst	r5, #64	; 0x40
    c918:	0a000009 	beq	c944 <nf2_check_irq+0x1e8>
    c91c:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c920:	e3a03040 	mov	r3, #64	; 0x40
    c924:	e582308c 	str	r3, [r2, #140]
    c928:	e3a03004 	mov	r3, #4	; 0x4
    c92c:	e5823088 	str	r3, [r2, #136]
    c930:	e3a02038 	mov	r2, #56	; 0x38
    c934:	e3e0332f 	mvn	r3, #-1140850688	; 0xbc000000
    c938:	e5032f77 	str	r2, [r3, #-3959]
    c93c:	e3c55040 	bic	r5, r5, #64	; 0x40
    c940:	ea000007 	b	c964 <nf2_check_irq+0x208>
    c944:	e3150801 	tst	r5, #65536	; 0x10000
    c948:	0a000005 	beq	c964 <nf2_check_irq+0x208>
    c94c:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    c950:	e3a02801 	mov	r2, #65536	; 0x10000
    c954:	e583208c 	str	r2, [r3, #140]
    c958:	e3a02008 	mov	r2, #8	; 0x8
    c95c:	e5832088 	str	r2, [r3, #136]
    c960:	e3c55801 	bic	r5, r5, #65536	; 0x10000
    c964:	e1a00005 	mov	r0, r5
    c968:	e28dd004 	add	sp, sp, #4	; 0x4
    c96c:	e8bd8030 	pop	{r4, r5, pc}
    c970:	00080400 	.word	0x00080400
    c974:	3c00d6cc 	.word	0x3c00d6cc
    c978:	000186ec 	.word	0x000186ec

0000c97c <nf2_erase>:
    c97c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    c980:	e3a00001 	mov	r0, #1	; 0x1
    c984:	e24dd008 	sub	sp, sp, #8	; 0x8
    c988:	e1a06001 	mov	r6, r1
    c98c:	e1a05002 	mov	r5, r2
    c990:	ebffff59 	bl	c6fc <nf2_soft_reset>
    c994:	e2504000 	subs	r4, r0, #0	; 0x0
    c998:	13e04000 	mvnne	r4, #0	; 0x0
    c99c:	1a000035 	bne	ca78 <nf2_erase+0xfc>
    c9a0:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    c9a4:	e5826020 	str	r6, [r2, #32]
    c9a8:	e58d4004 	str	r4, [sp, #4]
    c9ac:	e5923054 	ldr	r3, [r2, #84]
    c9b0:	e1a0e002 	mov	lr, r2
    c9b4:	e58d3004 	str	r3, [sp, #4]
    c9b8:	e59d3004 	ldr	r3, [sp, #4]
    c9bc:	e2450002 	sub	r0, r5, #2	; 0x2
    c9c0:	e3833001 	orr	r3, r3, #1	; 0x1
    c9c4:	e58d3004 	str	r3, [sp, #4]
    c9c8:	e59d3004 	ldr	r3, [sp, #4]
    c9cc:	e3a0c0ff 	mov	ip, #255	; 0xff
    c9d0:	e5823054 	str	r3, [r2, #84]
    c9d4:	e3043060 	movw	r3, #16480	; 0x4060
    c9d8:	e5823060 	str	r3, [r2, #96]
    c9dc:	e1a02004 	mov	r2, r4
    c9e0:	ea000003 	b	c9f4 <nf2_erase+0x78>
    c9e4:	e006311c 	and	r3, r6, ip, lsl r1
    c9e8:	e1a03153 	asr	r3, r3, r1
    c9ec:	e3833c41 	orr	r3, r3, #16640	; 0x4100
    c9f0:	e58e3060 	str	r3, [lr, #96]
    c9f4:	e1520000 	cmp	r2, r0
    c9f8:	e1a01182 	lsl	r1, r2, #3
    c9fc:	e2822001 	add	r2, r2, #1	; 0x1
    ca00:	bafffff7 	blt	c9e4 <nf2_erase+0x68>
    ca04:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    ca08:	e30438d0 	movw	r3, #18640	; 0x48d0
    ca0c:	e5823060 	str	r3, [r2, #96]
    ca10:	e3a03000 	mov	r3, #0	; 0x0
    ca14:	e5823054 	str	r3, [r2, #84]
    ca18:	e3e03001 	mvn	r3, #1	; 0x1
    ca1c:	e582304c 	str	r3, [r2, #76]
    ca20:	e2833003 	add	r3, r3, #3	; 0x3
    ca24:	e5823084 	str	r3, [r2, #132]
    ca28:	e1a07002 	mov	r7, r2
    ca2c:	eb0003db 	bl	d9a0 <get_ticks>
    ca30:	e59f804c 	ldr	r8, [pc, #76]	; ca84 <nf2_erase+0x108>
    ca34:	e1a05000 	mov	r5, r0
    ca38:	e3006801 	movw	r6, #2049	; 0x801
    ca3c:	eb0003d7 	bl	d9a0 <get_ticks>
    ca40:	e0650000 	rsb	r0, r5, r0
    ca44:	e1500008 	cmp	r0, r8
    ca48:	e3002801 	movw	r2, #2049	; 0x801
    ca4c:	c3e04000 	mvngt	r4, #0	; 0x0
    ca50:	ca000003 	bgt	ca64 <nf2_erase+0xe8>
    ca54:	e597308c 	ldr	r3, [r7, #140]
    ca58:	e0032002 	and	r2, r3, r2
    ca5c:	e1520006 	cmp	r2, r6
    ca60:	1afffff5 	bne	ca3c <nf2_erase+0xc0>
    ca64:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    ca68:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    ca6c:	e583208c 	str	r2, [r3, #140]
    ca70:	e3a0200f 	mov	r2, #15	; 0xf
    ca74:	e583204c 	str	r2, [r3, #76]
    ca78:	e1a00004 	mov	r0, r4
    ca7c:	e28dd008 	add	sp, sp, #8	; 0x8
    ca80:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    ca84:	0007a120 	.word	0x0007a120

0000ca88 <nf2_page_op>:
    ca88:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    ca8c:	e24dd0b4 	sub	sp, sp, #180	; 0xb4
    ca90:	e59d9120 	ldr	r9, [sp, #288]
    ca94:	e59da124 	ldr	sl, [sp, #292]
    ca98:	e3a0c000 	mov	ip, #0	; 0x0
    ca9c:	e589c000 	str	ip, [r9]
    caa0:	e58d0074 	str	r0, [sp, #116]
    caa4:	e58ac000 	str	ip, [sl]
    caa8:	e3a00001 	mov	r0, #1	; 0x1
    caac:	e58dc0ac 	str	ip, [sp, #172]
    cab0:	e58d1070 	str	r1, [sp, #112]
    cab4:	e58d206c 	str	r2, [sp, #108]
    cab8:	e58d3068 	str	r3, [sp, #104]
    cabc:	e59db0f0 	ldr	fp, [sp, #240]
    cac0:	e59d5104 	ldr	r5, [sp, #260]
    cac4:	e59d8108 	ldr	r8, [sp, #264]
    cac8:	e59d710c 	ldr	r7, [sp, #268]
    cacc:	e59d4110 	ldr	r4, [sp, #272]
    cad0:	e59da118 	ldr	sl, [sp, #280]
    cad4:	e59d911c 	ldr	r9, [sp, #284]
    cad8:	ebffff07 	bl	c6fc <nf2_soft_reset>
    cadc:	e3500000 	cmp	r0, #0	; 0x0
    cae0:	13e0c000 	mvnne	ip, #0	; 0x0
    cae4:	e58d009c 	str	r0, [sp, #156]
    cae8:	158dc09c 	strne	ip, [sp, #156]
    caec:	1a000208 	bne	d314 <nf2_page_op+0x88c>
    caf0:	e59d30dc 	ldr	r3, [sp, #220]
    caf4:	e3550001 	cmp	r5, #1	; 0x1
    caf8:	e2833004 	add	r3, r3, #4	; 0x4
    cafc:	159de09c 	ldrne	lr, [sp, #156]
    cb00:	e59d1070 	ldr	r1, [sp, #112]
    cb04:	e58d3098 	str	r3, [sp, #152]
    cb08:	03a0330a 	moveq	r3, #671088640	; 0x28000000
    cb0c:	13a0330a 	movne	r3, #671088640	; 0x28000000
    cb10:	05835164 	streq	r5, [r3, #356]
    cb14:	1583e164 	strne	lr, [r3, #356]
    cb18:	e3510000 	cmp	r1, #0	; 0x0
    cb1c:	1a000042 	bne	cc2c <nf2_page_op+0x1a4>
    cb20:	e59d2074 	ldr	r2, [sp, #116]
    cb24:	e3520001 	cmp	r2, #1	; 0x1
    cb28:	1a00002f 	bne	cbec <nf2_page_op+0x164>
    cb2c:	e3550001 	cmp	r5, #1	; 0x1
    cb30:	1a000018 	bne	cb98 <nf2_page_op+0x110>
    cb34:	e5982000 	ldr	r2, [r8]
    cb38:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cb3c:	e5832140 	str	r2, [r3, #320]
    cb40:	e5982004 	ldr	r2, [r8, #4]
    cb44:	e1d4c0bc 	ldrh	ip, [r4, #12]
    cb48:	e5832144 	str	r2, [r3, #324]
    cb4c:	e5982008 	ldr	r2, [r8, #8]
    cb50:	e1d470b2 	ldrh	r7, [r4, #2]
    cb54:	e5832148 	str	r2, [r3, #328]
    cb58:	e1d400b0 	ldrh	r0, [r4]
    cb5c:	e1d460b6 	ldrh	r6, [r4, #6]
    cb60:	e1d410b4 	ldrh	r1, [r4, #4]
    cb64:	e1d450ba 	ldrh	r5, [r4, #10]
    cb68:	e1d420b8 	ldrh	r2, [r4, #8]
    cb6c:	e1d4e0be 	ldrh	lr, [r4, #14]
    cb70:	e598400c 	ldr	r4, [r8, #12]
    cb74:	e1800807 	orr	r0, r0, r7, lsl #16
    cb78:	e1811806 	orr	r1, r1, r6, lsl #16
    cb7c:	e1822805 	orr	r2, r2, r5, lsl #16
    cb80:	e18cc80e 	orr	ip, ip, lr, lsl #16
    cb84:	e583414c 	str	r4, [r3, #332]
    cb88:	e5830150 	str	r0, [r3, #336]
    cb8c:	e5831154 	str	r1, [r3, #340]
    cb90:	e5832158 	str	r2, [r3, #344]
    cb94:	e583c15c 	str	ip, [r3, #348]
    cb98:	e59d006c 	ldr	r0, [sp, #108]
    cb9c:	e59d10d8 	ldr	r1, [sp, #216]
    cba0:	e59d20f8 	ldr	r2, [sp, #248]
    cba4:	ebfffe07 	bl	c3c8 <nf2_ecc_num>
    cba8:	e59d9098 	ldr	r9, [sp, #152]
    cbac:	e59da0d8 	ldr	sl, [sp, #216]
    cbb0:	e35b0001 	cmp	fp, #1	; 0x1
    cbb4:	e089300a 	add	r3, r9, sl
    cbb8:	11a04003 	movne	r4, r3
    cbbc:	11a0c000 	movne	ip, r0
    cbc0:	1a000005 	bne	cbdc <nf2_page_op+0x154>
    cbc4:	e59dc0f4 	ldr	ip, [sp, #244]
    cbc8:	e1a04003 	mov	r4, r3
    cbcc:	e35c0000 	cmp	ip, #0	; 0x0
    cbd0:	03a03008 	moveq	r3, #8	; 0x8
    cbd4:	13a0300c 	movne	r3, #12	; 0xc
    cbd8:	e083c000 	add	ip, r3, r0
    cbdc:	e1a0e004 	mov	lr, r4
    cbe0:	e1a06004 	mov	r6, r4
    cbe4:	e3a05000 	mov	r5, #0	; 0x0
    cbe8:	ea0000ec 	b	cfa0 <nf2_page_op+0x518>
    cbec:	e3550001 	cmp	r5, #1	; 0x1
    cbf0:	1a000008 	bne	cc18 <nf2_page_op+0x190>
    cbf4:	e5983000 	ldr	r3, [r8]
    cbf8:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    cbfc:	e5823140 	str	r3, [r2, #320]
    cc00:	e5983004 	ldr	r3, [r8, #4]
    cc04:	e5823144 	str	r3, [r2, #324]
    cc08:	e5983008 	ldr	r3, [r8, #8]
    cc0c:	e5823148 	str	r3, [r2, #328]
    cc10:	e598300c 	ldr	r3, [r8, #12]
    cc14:	e582314c 	str	r3, [r2, #332]
    cc18:	e59de098 	ldr	lr, [sp, #152]
    cc1c:	e59d10d8 	ldr	r1, [sp, #216]
    cc20:	e3a0b000 	mov	fp, #0	; 0x0
    cc24:	e08e4001 	add	r4, lr, r1
    cc28:	ea000030 	b	ccf0 <nf2_page_op+0x268>
    cc2c:	e59d30fc 	ldr	r3, [sp, #252]
    cc30:	e3530001 	cmp	r3, #1	; 0x1
    cc34:	1a00003f 	bne	cd38 <nf2_page_op+0x2b0>
    cc38:	e59d2074 	ldr	r2, [sp, #116]
    cc3c:	e3520001 	cmp	r2, #1	; 0x1
    cc40:	1a000026 	bne	cce0 <nf2_page_op+0x258>
    cc44:	e3550001 	cmp	r5, #1	; 0x1
    cc48:	1a000018 	bne	ccb0 <nf2_page_op+0x228>
    cc4c:	e5982000 	ldr	r2, [r8]
    cc50:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cc54:	e5832140 	str	r2, [r3, #320]
    cc58:	e5982004 	ldr	r2, [r8, #4]
    cc5c:	e1d4c0bc 	ldrh	ip, [r4, #12]
    cc60:	e5832144 	str	r2, [r3, #324]
    cc64:	e5982008 	ldr	r2, [r8, #8]
    cc68:	e1d470b2 	ldrh	r7, [r4, #2]
    cc6c:	e5832148 	str	r2, [r3, #328]
    cc70:	e1d400b0 	ldrh	r0, [r4]
    cc74:	e1d460b6 	ldrh	r6, [r4, #6]
    cc78:	e1d410b4 	ldrh	r1, [r4, #4]
    cc7c:	e1d450ba 	ldrh	r5, [r4, #10]
    cc80:	e1d420b8 	ldrh	r2, [r4, #8]
    cc84:	e1d4e0be 	ldrh	lr, [r4, #14]
    cc88:	e598400c 	ldr	r4, [r8, #12]
    cc8c:	e1800807 	orr	r0, r0, r7, lsl #16
    cc90:	e1811806 	orr	r1, r1, r6, lsl #16
    cc94:	e1822805 	orr	r2, r2, r5, lsl #16
    cc98:	e18cc80e 	orr	ip, ip, lr, lsl #16
    cc9c:	e583414c 	str	r4, [r3, #332]
    cca0:	e5830150 	str	r0, [r3, #336]
    cca4:	e5831154 	str	r1, [r3, #340]
    cca8:	e5832158 	str	r2, [r3, #344]
    ccac:	e583c15c 	str	ip, [r3, #348]
    ccb0:	e59d006c 	ldr	r0, [sp, #108]
    ccb4:	e59d10d8 	ldr	r1, [sp, #216]
    ccb8:	e59d20f8 	ldr	r2, [sp, #248]
    ccbc:	ebfffdc1 	bl	c3c8 <nf2_ecc_num>
    ccc0:	e59d90d8 	ldr	r9, [sp, #216]
    ccc4:	e59d3098 	ldr	r3, [sp, #152]
    ccc8:	e3a0b000 	mov	fp, #0	; 0x0
    cccc:	e1a0c000 	mov	ip, r0
    ccd0:	e0836009 	add	r6, r3, r9
    ccd4:	e1a04009 	mov	r4, r9
    ccd8:	e1a0e009 	mov	lr, r9
    ccdc:	ea000013 	b	cd30 <nf2_page_op+0x2a8>
    cce0:	e3550001 	cmp	r5, #1	; 0x1
    cce4:	0a000003 	beq	ccf8 <nf2_page_op+0x270>
    cce8:	e59d40d8 	ldr	r4, [sp, #216]
    ccec:	e3a0b000 	mov	fp, #0	; 0x0
    ccf0:	e1a0e004 	mov	lr, r4
    ccf4:	ea00000b 	b	cd28 <nf2_page_op+0x2a0>
    ccf8:	e5983000 	ldr	r3, [r8]
    ccfc:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    cd00:	e5823140 	str	r3, [r2, #320]
    cd04:	e5983004 	ldr	r3, [r8, #4]
    cd08:	e3a0b000 	mov	fp, #0	; 0x0
    cd0c:	e5823144 	str	r3, [r2, #324]
    cd10:	e5983008 	ldr	r3, [r8, #8]
    cd14:	e59d40d8 	ldr	r4, [sp, #216]
    cd18:	e5823148 	str	r3, [r2, #328]
    cd1c:	e598300c 	ldr	r3, [r8, #12]
    cd20:	e59de0d8 	ldr	lr, [sp, #216]
    cd24:	e582314c 	str	r3, [r2, #332]
    cd28:	e1a0c00b 	mov	ip, fp
    cd2c:	e1a0600b 	mov	r6, fp
    cd30:	e1a0500b 	mov	r5, fp
    cd34:	ea000099 	b	cfa0 <nf2_page_op+0x518>
    cd38:	e59dc074 	ldr	ip, [sp, #116]
    cd3c:	e35c0001 	cmp	ip, #1	; 0x1
    cd40:	1a00007b 	bne	cf34 <nf2_page_op+0x4ac>
    cd44:	e59d10d8 	ldr	r1, [sp, #216]
    cd48:	e59d006c 	ldr	r0, [sp, #108]
    cd4c:	e59d20f8 	ldr	r2, [sp, #248]
    cd50:	ebfffd9c 	bl	c3c8 <nf2_ecc_num>
    cd54:	e59de098 	ldr	lr, [sp, #152]
    cd58:	e59d10d8 	ldr	r1, [sp, #216]
    cd5c:	e35b0001 	cmp	fp, #1	; 0x1
    cd60:	e08ee001 	add	lr, lr, r1
    cd64:	e58d00a0 	str	r0, [sp, #160]
    cd68:	e58de0a4 	str	lr, [sp, #164]
    cd6c:	1a000037 	bne	ce50 <nf2_page_op+0x3c8>
    cd70:	e3550001 	cmp	r5, #1	; 0x1
    cd74:	1a00002a 	bne	ce24 <nf2_page_op+0x39c>
    cd78:	e1d790b6 	ldrh	r9, [r7, #6]
    cd7c:	e1d730b2 	ldrh	r3, [r7, #2]
    cd80:	e58d9060 	str	r9, [sp, #96]
    cd84:	e59d9114 	ldr	r9, [sp, #276]
    cd88:	e59d2114 	ldr	r2, [sp, #276]
    cd8c:	e58d3064 	str	r3, [sp, #100]
    cd90:	e1d7a0ba 	ldrh	sl, [r7, #10]
    cd94:	e1d930be 	ldrh	r3, [r9, #14]
    cd98:	e1d740b0 	ldrh	r4, [r7]
    cd9c:	e59d9064 	ldr	r9, [sp, #100]
    cda0:	e1d210b2 	ldrh	r1, [r2, #2]
    cda4:	e1d750bc 	ldrh	r5, [r7, #12]
    cda8:	e1d7e0b4 	ldrh	lr, [r7, #4]
    cdac:	e58da05c 	str	sl, [sp, #92]
    cdb0:	e1844809 	orr	r4, r4, r9, lsl #16
    cdb4:	e1d7a0be 	ldrh	sl, [r7, #14]
    cdb8:	e59d9060 	ldr	r9, [sp, #96]
    cdbc:	e58d1058 	str	r1, [sp, #88]
    cdc0:	e1d260bc 	ldrh	r6, [r2, #12]
    cdc4:	e1d7c0b8 	ldrh	ip, [r7, #8]
    cdc8:	e1d200b0 	ldrh	r0, [r2]
    cdcc:	e1d280b6 	ldrh	r8, [r2, #6]
    cdd0:	e1d210b4 	ldrh	r1, [r2, #4]
    cdd4:	e1d270ba 	ldrh	r7, [r2, #10]
    cdd8:	e18ee809 	orr	lr, lr, r9, lsl #16
    cddc:	e1d220b8 	ldrh	r2, [r2, #8]
    cde0:	e59d905c 	ldr	r9, [sp, #92]
    cde4:	e185580a 	orr	r5, r5, sl, lsl #16
    cde8:	e59da058 	ldr	sl, [sp, #88]
    cdec:	e1866803 	orr	r6, r6, r3, lsl #16
    cdf0:	e18cc809 	orr	ip, ip, r9, lsl #16
    cdf4:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cdf8:	e180080a 	orr	r0, r0, sl, lsl #16
    cdfc:	e1811808 	orr	r1, r1, r8, lsl #16
    ce00:	e1822807 	orr	r2, r2, r7, lsl #16
    ce04:	e5834140 	str	r4, [r3, #320]
    ce08:	e583e144 	str	lr, [r3, #324]
    ce0c:	e583c148 	str	ip, [r3, #328]
    ce10:	e583514c 	str	r5, [r3, #332]
    ce14:	e5830150 	str	r0, [r3, #336]
    ce18:	e5831154 	str	r1, [r3, #340]
    ce1c:	e5832158 	str	r2, [r3, #344]
    ce20:	e583615c 	str	r6, [r3, #348]
    ce24:	e59d10f4 	ldr	r1, [sp, #244]
    ce28:	e59dc0a4 	ldr	ip, [sp, #164]
    ce2c:	e59de0a0 	ldr	lr, [sp, #160]
    ce30:	e59d4098 	ldr	r4, [sp, #152]
    ce34:	e3510000 	cmp	r1, #0	; 0x0
    ce38:	e08c600e 	add	r6, ip, lr
    ce3c:	e59d50d8 	ldr	r5, [sp, #216]
    ce40:	03a0c008 	moveq	ip, #8	; 0x8
    ce44:	13a0c00c 	movne	ip, #12	; 0xc
    ce48:	e1a0e004 	mov	lr, r4
    ce4c:	ea000053 	b	cfa0 <nf2_page_op+0x518>
    ce50:	e3550001 	cmp	r5, #1	; 0x1
    ce54:	1a000028 	bne	cefc <nf2_page_op+0x474>
    ce58:	e1da20b2 	ldrh	r2, [sl, #2]
    ce5c:	e1da30b6 	ldrh	r3, [sl, #6]
    ce60:	e1d910b2 	ldrh	r1, [r9, #2]
    ce64:	e58d2054 	str	r2, [sp, #84]
    ce68:	e1dac0ba 	ldrh	ip, [sl, #10]
    ce6c:	e1d960bc 	ldrh	r6, [r9, #12]
    ce70:	e1da40b0 	ldrh	r4, [sl]
    ce74:	e58d3050 	str	r3, [sp, #80]
    ce78:	e58d1048 	str	r1, [sp, #72]
    ce7c:	e1d930be 	ldrh	r3, [r9, #14]
    ce80:	e1d900b0 	ldrh	r0, [r9]
    ce84:	e1d980b6 	ldrh	r8, [r9, #6]
    ce88:	e1d910b4 	ldrh	r1, [r9, #4]
    ce8c:	e1d970ba 	ldrh	r7, [r9, #10]
    ce90:	e1d920b8 	ldrh	r2, [r9, #8]
    ce94:	e59d9054 	ldr	r9, [sp, #84]
    ce98:	e1da50bc 	ldrh	r5, [sl, #12]
    ce9c:	e1dae0b4 	ldrh	lr, [sl, #4]
    cea0:	e58dc04c 	str	ip, [sp, #76]
    cea4:	e1844809 	orr	r4, r4, r9, lsl #16
    cea8:	e1dac0b8 	ldrh	ip, [sl, #8]
    ceac:	e59d9050 	ldr	r9, [sp, #80]
    ceb0:	e1daa0be 	ldrh	sl, [sl, #14]
    ceb4:	e18ee809 	orr	lr, lr, r9, lsl #16
    ceb8:	e185580a 	orr	r5, r5, sl, lsl #16
    cebc:	e59d904c 	ldr	r9, [sp, #76]
    cec0:	e59da048 	ldr	sl, [sp, #72]
    cec4:	e1866803 	orr	r6, r6, r3, lsl #16
    cec8:	e18cc809 	orr	ip, ip, r9, lsl #16
    cecc:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    ced0:	e180080a 	orr	r0, r0, sl, lsl #16
    ced4:	e1811808 	orr	r1, r1, r8, lsl #16
    ced8:	e1822807 	orr	r2, r2, r7, lsl #16
    cedc:	e5834140 	str	r4, [r3, #320]
    cee0:	e583e144 	str	lr, [r3, #324]
    cee4:	e583c148 	str	ip, [r3, #328]
    cee8:	e583514c 	str	r5, [r3, #332]
    ceec:	e5830150 	str	r0, [r3, #336]
    cef0:	e5831154 	str	r1, [r3, #340]
    cef4:	e5832158 	str	r2, [r3, #344]
    cef8:	e583615c 	str	r6, [r3, #348]
    cefc:	e59dc0d8 	ldr	ip, [sp, #216]
    cf00:	e59d00a0 	ldr	r0, [sp, #160]
    cf04:	e1a0454c 	asr	r4, ip, #10
    cf08:	e1a01004 	mov	r1, r4
    cf0c:	eb001ea2 	bl	1499c <__aeabi_idiv>
    cf10:	e59de0a4 	ldr	lr, [sp, #164]
    cf14:	e2444001 	sub	r4, r4, #1	; 0x1
    cf18:	e026e490 	mla	r6, r0, r4, lr
    cf1c:	e59d10d8 	ldr	r1, [sp, #216]
    cf20:	e59d4098 	ldr	r4, [sp, #152]
    cf24:	e1a0c000 	mov	ip, r0
    cf28:	e284eb01 	add	lr, r4, #1024	; 0x400
    cf2c:	e2415b01 	sub	r5, r1, #1024	; 0x400
    cf30:	ea00001a 	b	cfa0 <nf2_page_op+0x518>
    cf34:	e3550001 	cmp	r5, #1	; 0x1
    cf38:	159d4098 	ldrne	r4, [sp, #152]
    cf3c:	13a0b000 	movne	fp, #0	; 0x0
    cf40:	1a000012 	bne	cf90 <nf2_page_op+0x508>
    cf44:	e1d740b6 	ldrh	r4, [r7, #6]
    cf48:	e1d730be 	ldrh	r3, [r7, #14]
    cf4c:	e1d7c0bc 	ldrh	ip, [r7, #12]
    cf50:	e1d750b2 	ldrh	r5, [r7, #2]
    cf54:	e1d700b0 	ldrh	r0, [r7]
    cf58:	e1d710b4 	ldrh	r1, [r7, #4]
    cf5c:	e1d7e0ba 	ldrh	lr, [r7, #10]
    cf60:	e1d720b8 	ldrh	r2, [r7, #8]
    cf64:	e1811804 	orr	r1, r1, r4, lsl #16
    cf68:	e18cc803 	orr	ip, ip, r3, lsl #16
    cf6c:	e1800805 	orr	r0, r0, r5, lsl #16
    cf70:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    cf74:	e182280e 	orr	r2, r2, lr, lsl #16
    cf78:	e59d4098 	ldr	r4, [sp, #152]
    cf7c:	e5830140 	str	r0, [r3, #320]
    cf80:	e3a0b000 	mov	fp, #0	; 0x0
    cf84:	e5831144 	str	r1, [r3, #324]
    cf88:	e5832148 	str	r2, [r3, #328]
    cf8c:	e583c14c 	str	ip, [r3, #332]
    cf90:	e59d50d8 	ldr	r5, [sp, #216]
    cf94:	e1a0e004 	mov	lr, r4
    cf98:	e1a0c00b 	mov	ip, fp
    cf9c:	e1a0600b 	mov	r6, fp
    cfa0:	e59d2130 	ldr	r2, [sp, #304]
    cfa4:	e59d90f8 	ldr	r9, [sp, #248]
    cfa8:	e3520001 	cmp	r2, #1	; 0x1
    cfac:	059d312c 	ldreq	r3, [sp, #300]
    cfb0:	e59da0e0 	ldr	sl, [sp, #224]
    cfb4:	00634004 	rsbeq	r4, r3, r4
    cfb8:	e59d1070 	ldr	r1, [sp, #112]
    cfbc:	e3a03000 	mov	r3, #0	; 0x0
    cfc0:	e59d206c 	ldr	r2, [sp, #108]
    cfc4:	e59d0074 	ldr	r0, [sp, #116]
    cfc8:	e88d5000 	stm	sp, {ip, lr}
    cfcc:	e58d900c 	str	r9, [sp, #12]
    cfd0:	e58da008 	str	sl, [sp, #8]
    cfd4:	ebfffce5 	bl	c370 <nf2_ecc_cfg>
    cfd8:	e1a0000b 	mov	r0, fp
    cfdc:	e59d10f4 	ldr	r1, [sp, #244]
    cfe0:	e59d2068 	ldr	r2, [sp, #104]
    cfe4:	ebfffcc2 	bl	c2f4 <nf2_secc_cfg>
    cfe8:	e59d3134 	ldr	r3, [sp, #308]
    cfec:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    cff0:	e3530010 	cmp	r3, #16	; 0x10
    cff4:	e59d30e8 	ldr	r3, [sp, #232]
    cff8:	e59dc130 	ldr	ip, [sp, #304]
    cffc:	e59de12c 	ldr	lr, [sp, #300]
    d000:	e5823038 	str	r3, [r2, #56]
    d004:	e59d3128 	ldr	r3, [sp, #296]
    d008:	e1a0110c 	lsl	r1, ip, #2
    d00c:	e582303c 	str	r3, [r2, #60]
    d010:	e184080e 	orr	r0, r4, lr, lsl #16
    d014:	e3811001 	orr	r1, r1, #1	; 0x1
    d018:	e3a03341 	mov	r3, #67108865	; 0x4000001
    d01c:	e5820040 	str	r0, [r2, #64]
    d020:	e5823044 	str	r3, [r2, #68]
    d024:	e5821048 	str	r1, [r2, #72]
    d028:	e59d20e4 	ldr	r2, [sp, #228]
    d02c:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    d030:	e59d10e0 	ldr	r1, [sp, #224]
    d034:	e5842020 	str	r2, [r4, #32]
    d038:	e59d2100 	ldr	r2, [sp, #256]
    d03c:	01a050c5 	asreq	r5, r5, #1
    d040:	01a060c6 	asreq	r6, r6, #1
    d044:	e1853806 	orr	r3, r5, r6, lsl #16
    d048:	e3510000 	cmp	r1, #0	; 0x0
    d04c:	e3a05000 	mov	r5, #0	; 0x0
    d050:	e5845024 	str	r5, [r4, #36]
    d054:	e2422001 	sub	r2, r2, #1	; 0x1
    d058:	e5845028 	str	r5, [r4, #40]
    d05c:	e584502c 	str	r5, [r4, #44]
    d060:	e5843030 	str	r3, [r4, #48]
    d064:	1a000041 	bne	d170 <nf2_page_op+0x6e8>
    d068:	e382c008 	orr	ip, r2, #8	; 0x8
    d06c:	e3a03020 	mov	r3, #32	; 0x20
    d070:	e20cc00f 	and	ip, ip, #15	; 0xf
    d074:	e59d20e0 	ldr	r2, [sp, #224]
    d078:	e3a0e001 	mov	lr, #1	; 0x1
    d07c:	e5843090 	str	r3, [r4, #144]
    d080:	e1a0c50c 	lsl	ip, ip, #10
    d084:	e3a03901 	mov	r3, #16384	; 0x4000
    d088:	e584e054 	str	lr, [r4, #84]
    d08c:	e38ccc41 	orr	ip, ip, #16640	; 0x4100
    d090:	e5843060 	str	r3, [r4, #96]
    d094:	e2833e43 	add	r3, r3, #1072	; 0x430
    d098:	e584c060 	str	ip, [r4, #96]
    d09c:	e1a0100e 	mov	r1, lr
    d0a0:	e5843060 	str	r3, [r4, #96]
    d0a4:	e3a00008 	mov	r0, #8	; 0x8
    d0a8:	ebfffc0b 	bl	c0dc <trx_afifo_reg>
    d0ac:	e59d30e0 	ldr	r3, [sp, #224]
    d0b0:	e59d2074 	ldr	r2, [sp, #116]
    d0b4:	e5843054 	str	r3, [r4, #84]
    d0b8:	e1520005 	cmp	r2, r5
    d0bc:	e3a0300e 	mov	r3, #14	; 0xe
    d0c0:	e584304c 	str	r3, [r4, #76]
    d0c4:	13a03007 	movne	r3, #7	; 0x7
    d0c8:	03a03005 	moveq	r3, #5	; 0x5
    d0cc:	e5843084 	str	r3, [r4, #132]
    d0d0:	eb000232 	bl	d9a0 <get_ticks>
    d0d4:	e59f5244 	ldr	r5, [pc, #580]	; d320 <nf2_page_op+0x898>
    d0d8:	e1a04000 	mov	r4, r0
    d0dc:	eb00022f 	bl	d9a0 <get_ticks>
    d0e0:	e0640000 	rsb	r0, r4, r0
    d0e4:	e1500005 	cmp	r0, r5
    d0e8:	da000006 	ble	d108 <nf2_page_op+0x680>
    d0ec:	e59f0230 	ldr	r0, [pc, #560]	; d324 <nf2_page_op+0x89c>
    d0f0:	e59f3230 	ldr	r3, [pc, #560]	; d328 <nf2_page_op+0x8a0>
    d0f4:	e3e09000 	mvn	r9, #0	; 0x0
    d0f8:	e1a0e00f 	mov	lr, pc
    d0fc:	e593f000 	ldr	pc, [r3]
    d100:	e58d909c 	str	r9, [sp, #156]
    d104:	ea000002 	b	d114 <nf2_page_op+0x68c>
    d108:	ebfffd93 	bl	c75c <nf2_check_irq>
    d10c:	e3500020 	cmp	r0, #32	; 0x20
    d110:	1afffff1 	bne	d0dc <nf2_page_op+0x654>
    d114:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d118:	e593208c 	ldr	r2, [r3, #140]
    d11c:	e59d00d8 	ldr	r0, [sp, #216]
    d120:	e583208c 	str	r2, [r3, #140]
    d124:	e3a0200f 	mov	r2, #15	; 0xf
    d128:	e583204c 	str	r2, [r3, #76]
    d12c:	e28d10ac 	add	r1, sp, #172	; 0xac
    d130:	ebfffcf5 	bl	c50c <nf2_check_ecc>
    d134:	e3500000 	cmp	r0, #0	; 0x0
    d138:	059da120 	ldreq	sl, [sp, #288]
    d13c:	e59dc120 	ldr	ip, [sp, #288]
    d140:	059a3000 	ldreq	r3, [sl]
    d144:	13a030ff 	movne	r3, #255	; 0xff
    d148:	e35b0001 	cmp	fp, #1	; 0x1
    d14c:	e58c3000 	str	r3, [ip]
    d150:	1a00006f 	bne	d314 <nf2_page_op+0x88c>
    d154:	e28d00ac 	add	r0, sp, #172	; 0xac
    d158:	ebfffd36 	bl	c638 <nf2_check_secc>
    d15c:	e3500000 	cmp	r0, #0	; 0x0
    d160:	159de124 	ldrne	lr, [sp, #292]
    d164:	13a030ff 	movne	r3, #255	; 0xff
    d168:	158e3000 	strne	r3, [lr]
    d16c:	ea000068 	b	d314 <nf2_page_op+0x88c>
    d170:	e382c008 	orr	ip, r2, #8	; 0x8
    d174:	e3013001 	movw	r3, #4097	; 0x1001
    d178:	e20cc00f 	and	ip, ip, #15	; 0xf
    d17c:	e5843090 	str	r3, [r4, #144]
    d180:	e3a0e001 	mov	lr, #1	; 0x1
    d184:	e3043080 	movw	r3, #16512	; 0x4080
    d188:	e1a0c50c 	lsl	ip, ip, #10
    d18c:	e584e054 	str	lr, [r4, #84]
    d190:	e38ccc41 	orr	ip, ip, #16640	; 0x4100
    d194:	e5843060 	str	r3, [r4, #96]
    d198:	e2433dd2 	sub	r3, r3, #13440	; 0x3480
    d19c:	e584c060 	str	ip, [r4, #96]
    d1a0:	e5843060 	str	r3, [r4, #96]
    d1a4:	e3043810 	movw	r3, #18448	; 0x4810
    d1a8:	e1a0100e 	mov	r1, lr
    d1ac:	e1a02005 	mov	r2, r5
    d1b0:	e5843060 	str	r3, [r4, #96]
    d1b4:	e3a00008 	mov	r0, #8	; 0x8
    d1b8:	ebfffbc7 	bl	c0dc <trx_afifo_reg>
    d1bc:	e59d1074 	ldr	r1, [sp, #116]
    d1c0:	e3a0300e 	mov	r3, #14	; 0xe
    d1c4:	e3510000 	cmp	r1, #0	; 0x0
    d1c8:	e5845054 	str	r5, [r4, #84]
    d1cc:	e584304c 	str	r3, [r4, #76]
    d1d0:	13a03007 	movne	r3, #7	; 0x7
    d1d4:	03a03005 	moveq	r3, #5	; 0x5
    d1d8:	e5843084 	str	r3, [r4, #132]
    d1dc:	eb0001ef 	bl	d9a0 <get_ticks>
    d1e0:	e59f5138 	ldr	r5, [pc, #312]	; d320 <nf2_page_op+0x898>
    d1e4:	e1a04000 	mov	r4, r0
    d1e8:	eb0001ec 	bl	d9a0 <get_ticks>
    d1ec:	e0640000 	rsb	r0, r4, r0
    d1f0:	e1500005 	cmp	r0, r5
    d1f4:	da00003d 	ble	d2f0 <nf2_page_op+0x868>
    d1f8:	e3a0030a 	mov	r0, #671088640	; 0x28000000
    d1fc:	e590108c 	ldr	r1, [r0, #140]
    d200:	e5902094 	ldr	r2, [r0, #148]
    d204:	e59030a0 	ldr	r3, [r0, #160]
    d208:	e590c0a4 	ldr	ip, [r0, #164]
    d20c:	e590e0a8 	ldr	lr, [r0, #168]
    d210:	e590405c 	ldr	r4, [r0, #92]
    d214:	e5905098 	ldr	r5, [r0, #152]
    d218:	e5906038 	ldr	r6, [r0, #56]
    d21c:	e590703c 	ldr	r7, [r0, #60]
    d220:	e5908168 	ldr	r8, [r0, #360]
    d224:	e590a16c 	ldr	sl, [r0, #364]
    d228:	e5909170 	ldr	r9, [r0, #368]
    d22c:	e58d9078 	str	r9, [sp, #120]
    d230:	e5909174 	ldr	r9, [r0, #372]
    d234:	e58d907c 	str	r9, [sp, #124]
    d238:	e5909178 	ldr	r9, [r0, #376]
    d23c:	e58d9080 	str	r9, [sp, #128]
    d240:	e590917c 	ldr	r9, [r0, #380]
    d244:	e58d9084 	str	r9, [sp, #132]
    d248:	e5909180 	ldr	r9, [r0, #384]
    d24c:	e58d9088 	str	r9, [sp, #136]
    d250:	e5909184 	ldr	r9, [r0, #388]
    d254:	e58d908c 	str	r9, [sp, #140]
    d258:	e5909188 	ldr	r9, [r0, #392]
    d25c:	e58d9090 	str	r9, [sp, #144]
    d260:	e590918c 	ldr	r9, [r0, #396]
    d264:	e590b190 	ldr	fp, [r0, #400]
    d268:	e58d9094 	str	r9, [sp, #148]
    d26c:	e5900194 	ldr	r0, [r0, #404]
    d270:	e88d5000 	stm	sp, {ip, lr}
    d274:	e58d4008 	str	r4, [sp, #8]
    d278:	e58d500c 	str	r5, [sp, #12]
    d27c:	e58d6010 	str	r6, [sp, #16]
    d280:	e59dc07c 	ldr	ip, [sp, #124]
    d284:	e59de080 	ldr	lr, [sp, #128]
    d288:	e59d9084 	ldr	r9, [sp, #132]
    d28c:	e58da01c 	str	sl, [sp, #28]
    d290:	e59da078 	ldr	sl, [sp, #120]
    d294:	e58dc024 	str	ip, [sp, #36]
    d298:	e58da020 	str	sl, [sp, #32]
    d29c:	e59dc08c 	ldr	ip, [sp, #140]
    d2a0:	e59da088 	ldr	sl, [sp, #136]
    d2a4:	e58de028 	str	lr, [sp, #40]
    d2a8:	e58d902c 	str	r9, [sp, #44]
    d2ac:	e59de090 	ldr	lr, [sp, #144]
    d2b0:	e59d9094 	ldr	r9, [sp, #148]
    d2b4:	e58da030 	str	sl, [sp, #48]
    d2b8:	e58dc034 	str	ip, [sp, #52]
    d2bc:	e58d0044 	str	r0, [sp, #68]
    d2c0:	e58d7014 	str	r7, [sp, #20]
    d2c4:	e58d8018 	str	r8, [sp, #24]
    d2c8:	e58de038 	str	lr, [sp, #56]
    d2cc:	e58d903c 	str	r9, [sp, #60]
    d2d0:	e58db040 	str	fp, [sp, #64]
    d2d4:	e59f0050 	ldr	r0, [pc, #80]	; d32c <nf2_page_op+0x8a4>
    d2d8:	e59fc048 	ldr	ip, [pc, #72]	; d328 <nf2_page_op+0x8a0>
    d2dc:	e3e0a000 	mvn	sl, #0	; 0x0
    d2e0:	e1a0e00f 	mov	lr, pc
    d2e4:	e59cf000 	ldr	pc, [ip]
    d2e8:	e58da09c 	str	sl, [sp, #156]
    d2ec:	ea000002 	b	d2fc <nf2_page_op+0x874>
    d2f0:	ebfffd19 	bl	c75c <nf2_check_irq>
    d2f4:	e3500001 	cmp	r0, #1	; 0x1
    d2f8:	1affffba 	bne	d1e8 <nf2_page_op+0x760>
    d2fc:	ebfffd16 	bl	c75c <nf2_check_irq>
    d300:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d304:	e593208c 	ldr	r2, [r3, #140]
    d308:	e583208c 	str	r2, [r3, #140]
    d30c:	e3a0200f 	mov	r2, #15	; 0xf
    d310:	e583204c 	str	r2, [r3, #76]
    d314:	e59d009c 	ldr	r0, [sp, #156]
    d318:	e28dd0b4 	add	sp, sp, #180	; 0xb4
    d31c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    d320:	0007a120 	.word	0x0007a120
    d324:	000186ff 	.word	0x000186ff
    d328:	3c00d6cc 	.word	0x3c00d6cc
    d32c:	0001870d 	.word	0x0001870d

0000d330 <nf2_randc_seed_hw>:
    d330:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d334:	e1801801 	orr	r1, r0, r1, lsl #16
    d338:	e3a03001 	mov	r3, #1	; 0x1
    d33c:	e58211c4 	str	r1, [r2, #452]
    d340:	e58231c8 	str	r3, [r2, #456]
    d344:	e3a03000 	mov	r3, #0	; 0x0
    d348:	e92d4070 	push	{r4, r5, r6, lr}
    d34c:	e58231c8 	str	r3, [r2, #456]
    d350:	e1a05002 	mov	r5, r2
    d354:	eb000191 	bl	d9a0 <get_ticks>
    d358:	e59f6048 	ldr	r6, [pc, #72]	; d3a8 <nf2_randc_seed_hw+0x78>
    d35c:	e1a04000 	mov	r4, r0
    d360:	eb00018e 	bl	d9a0 <get_ticks>
    d364:	e0640000 	rsb	r0, r4, r0
    d368:	e1500006 	cmp	r0, r6
    d36c:	da000005 	ble	d388 <nf2_randc_seed_hw+0x58>
    d370:	e59f0034 	ldr	r0, [pc, #52]	; d3ac <nf2_randc_seed_hw+0x7c>
    d374:	e59f3034 	ldr	r3, [pc, #52]	; d3b0 <nf2_randc_seed_hw+0x80>
    d378:	e1a0e00f 	mov	lr, pc
    d37c:	e593f000 	ldr	pc, [r3]
    d380:	e3e00000 	mvn	r0, #0	; 0x0
    d384:	e8bd8070 	pop	{r4, r5, r6, pc}
    d388:	e59531cc 	ldr	r3, [r5, #460]
    d38c:	e3130801 	tst	r3, #65536	; 0x10000
    d390:	1afffff2 	bne	d360 <nf2_randc_seed_hw+0x30>
    d394:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d398:	e59331cc 	ldr	r3, [r3, #460]
    d39c:	e1a00803 	lsl	r0, r3, #16
    d3a0:	e1a00820 	lsr	r0, r0, #16
    d3a4:	e8bd8070 	pop	{r4, r5, r6, pc}
    d3a8:	0007a120 	.word	0x0007a120
    d3ac:	000187d3 	.word	0x000187d3
    d3b0:	3c00d6cc 	.word	0x3c00d6cc

0000d3b4 <nf2_asyn_reset>:
    d3b4:	e92d4070 	push	{r4, r5, r6, lr}
    d3b8:	e1a04000 	mov	r4, r0
    d3bc:	e3a00001 	mov	r0, #1	; 0x1
    d3c0:	ebfffccd 	bl	c6fc <nf2_soft_reset>
    d3c4:	e3a01001 	mov	r1, #1	; 0x1
    d3c8:	e1e04411 	mvn	r4, r1, lsl r4
    d3cc:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d3d0:	e30520ff 	movw	r2, #20735	; 0x50ff
    d3d4:	e5831054 	str	r1, [r3, #84]
    d3d8:	e5832060 	str	r2, [r3, #96]
    d3dc:	e3a02000 	mov	r2, #0	; 0x0
    d3e0:	e5832054 	str	r2, [r3, #84]
    d3e4:	e593204c 	ldr	r2, [r3, #76]
    d3e8:	e583404c 	str	r4, [r3, #76]
    d3ec:	e5831084 	str	r1, [r3, #132]
    d3f0:	e1a06003 	mov	r6, r3
    d3f4:	eb000169 	bl	d9a0 <get_ticks>
    d3f8:	e3005bb8 	movw	r5, #3000	; 0xbb8
    d3fc:	e1a04000 	mov	r4, r0
    d400:	eb000166 	bl	d9a0 <get_ticks>
    d404:	e0640000 	rsb	r0, r4, r0
    d408:	e1500005 	cmp	r0, r5
    d40c:	da000005 	ble	d428 <nf2_asyn_reset+0x74>
    d410:	e59f0038 	ldr	r0, [pc, #56]	; d450 <nf2_asyn_reset+0x9c>
    d414:	e59f3038 	ldr	r3, [pc, #56]	; d454 <nf2_asyn_reset+0xa0>
    d418:	e1a0e00f 	mov	lr, pc
    d41c:	e593f000 	ldr	pc, [r3]
    d420:	e3e00000 	mvn	r0, #0	; 0x0
    d424:	ea000003 	b	d438 <nf2_asyn_reset+0x84>
    d428:	e596308c 	ldr	r3, [r6, #140]
    d42c:	e3130001 	tst	r3, #1	; 0x1
    d430:	0afffff2 	beq	d400 <nf2_asyn_reset+0x4c>
    d434:	e3a00000 	mov	r0, #0	; 0x0
    d438:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d43c:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    d440:	e583208c 	str	r2, [r3, #140]
    d444:	e3a0200f 	mov	r2, #15	; 0xf
    d448:	e583204c 	str	r2, [r3, #76]
    d44c:	e8bd8070 	pop	{r4, r5, r6, pc}
    d450:	000187ee 	.word	0x000187ee
    d454:	3c00d6cc 	.word	0x3c00d6cc

0000d458 <nf2_active_sync>:
    d458:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    d45c:	e1a05000 	mov	r5, r0
    d460:	e3a00001 	mov	r0, #1	; 0x1
    d464:	ebfffca4 	bl	c6fc <nf2_soft_reset>
    d468:	e3a0430a 	mov	r4, #671088640	; 0x28000000
    d46c:	e3a06000 	mov	r6, #0	; 0x0
    d470:	e3a07001 	mov	r7, #1	; 0x1
    d474:	e30430ef 	movw	r3, #16623	; 0x40ef
    d478:	e58460c4 	str	r6, [r4, #196]
    d47c:	e5847054 	str	r7, [r4, #84]
    d480:	e5843060 	str	r3, [r4, #96]
    d484:	e2833012 	add	r3, r3, #18	; 0x12
    d488:	e5843060 	str	r3, [r4, #96]
    d48c:	e3a0cc42 	mov	ip, #16896	; 0x4200
    d490:	e3a03c52 	mov	r3, #20992	; 0x5200
    d494:	e3855c42 	orr	r5, r5, #16896	; 0x4200
    d498:	e5845060 	str	r5, [r4, #96]
    d49c:	e3a00002 	mov	r0, #2	; 0x2
    d4a0:	e584c060 	str	ip, [r4, #96]
    d4a4:	e3a0100f 	mov	r1, #15	; 0xf
    d4a8:	e584c060 	str	ip, [r4, #96]
    d4ac:	e1a02006 	mov	r2, r6
    d4b0:	e5843060 	str	r3, [r4, #96]
    d4b4:	ebfffb08 	bl	c0dc <trx_afifo_reg>
    d4b8:	e3a03c01 	mov	r3, #256	; 0x100
    d4bc:	e3a00003 	mov	r0, #3	; 0x3
    d4c0:	e1a01007 	mov	r1, r7
    d4c4:	e1a02006 	mov	r2, r6
    d4c8:	e5843060 	str	r3, [r4, #96]
    d4cc:	ebfffb02 	bl	c0dc <trx_afifo_reg>
    d4d0:	e3a0300e 	mov	r3, #14	; 0xe
    d4d4:	e5846054 	str	r6, [r4, #84]
    d4d8:	e584304c 	str	r3, [r4, #76]
    d4dc:	e5847084 	str	r7, [r4, #132]
    d4e0:	eb00012e 	bl	d9a0 <get_ticks>
    d4e4:	e59f8048 	ldr	r8, [pc, #72]	; d534 <nf2_active_sync+0xdc>
    d4e8:	e1a05000 	mov	r5, r0
    d4ec:	eb00012b 	bl	d9a0 <get_ticks>
    d4f0:	e0650000 	rsb	r0, r5, r0
    d4f4:	e1500008 	cmp	r0, r8
    d4f8:	da000005 	ble	d514 <nf2_active_sync+0xbc>
    d4fc:	e59f0034 	ldr	r0, [pc, #52]	; d538 <nf2_active_sync+0xe0>
    d500:	e59f3034 	ldr	r3, [pc, #52]	; d53c <nf2_active_sync+0xe4>
    d504:	e1a0e00f 	mov	lr, pc
    d508:	e593f000 	ldr	pc, [r3]
    d50c:	e3e00000 	mvn	r0, #0	; 0x0
    d510:	ea000003 	b	d524 <nf2_active_sync+0xcc>
    d514:	e594308c 	ldr	r3, [r4, #140]
    d518:	e3130001 	tst	r3, #1	; 0x1
    d51c:	0afffff2 	beq	d4ec <nf2_active_sync+0x94>
    d520:	e3a00000 	mov	r0, #0	; 0x0
    d524:	e3e024ff 	mvn	r2, #-16777216	; 0xff000000
    d528:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d52c:	e583208c 	str	r2, [r3, #140]
    d530:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    d534:	0007a120 	.word	0x0007a120
    d538:	00018806 	.word	0x00018806
    d53c:	3c00d6cc 	.word	0x3c00d6cc

0000d540 <nf2_timing_init>:
    d540:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    d544:	e1a04000 	mov	r4, r0
    d548:	e24dd004 	sub	sp, sp, #4	; 0x4
    d54c:	e59f00b8 	ldr	r0, [pc, #184]	; d60c <nf2_timing_init+0xcc>
    d550:	e1a07002 	mov	r7, r2
    d554:	e1a06003 	mov	r6, r3
    d558:	e1a05001 	mov	r5, r1
    d55c:	ebffcc88 	bl	784 <module_enable>
    d560:	e3a00010 	mov	r0, #16	; 0x10
    d564:	ebffccc1 	bl	870 <pads_init_module>
    d568:	e59d301c 	ldr	r3, [sp, #28]
    d56c:	e3530010 	cmp	r3, #16	; 0x10
    d570:	13a02000 	movne	r2, #0	; 0x0
    d574:	1a000002 	bne	d584 <nf2_timing_init+0x44>
    d578:	e3a00011 	mov	r0, #17	; 0x11
    d57c:	ebffccbb 	bl	870 <pads_init_module>
    d580:	e3a02001 	mov	r2, #1	; 0x1
    d584:	e3a0330a 	mov	r3, #671088640	; 0x28000000
    d588:	e58321b8 	str	r2, [r3, #440]
    d58c:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d590:	e59230c8 	ldr	r3, [r2, #200]
    d594:	e3540000 	cmp	r4, #0	; 0x0
    d598:	e3c33070 	bic	r3, r3, #112	; 0x70
    d59c:	e3833040 	orr	r3, r3, #64	; 0x40
    d5a0:	e58230c8 	str	r3, [r2, #200]
    d5a4:	058240c4 	streq	r4, [r2, #196]
    d5a8:	059230c8 	ldreq	r3, [r2, #200]
    d5ac:	03833001 	orreq	r3, r3, #1	; 0x1
    d5b0:	0a000005 	beq	d5cc <nf2_timing_init+0x8c>
    d5b4:	e3540001 	cmp	r4, #1	; 0x1
    d5b8:	13a03002 	movne	r3, #2	; 0x2
    d5bc:	058240c4 	streq	r4, [r2, #196]
    d5c0:	158230c4 	strne	r3, [r2, #196]
    d5c4:	e59230c8 	ldr	r3, [r2, #200]
    d5c8:	e3c33001 	bic	r3, r3, #1	; 0x1
    d5cc:	e58230c8 	str	r3, [r2, #200]
    d5d0:	e3c534ff 	bic	r3, r5, #-16777216	; 0xff000000
    d5d4:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d5d8:	e3c3360e 	bic	r3, r3, #14680064	; 0xe00000
    d5dc:	e5823000 	str	r3, [r2]
    d5e0:	e1a01c25 	lsr	r1, r5, #24
    d5e4:	e30e3001 	movw	r3, #57345	; 0xe001
    d5e8:	e5821004 	str	r1, [r2, #4]
    d5ec:	e582300c 	str	r3, [r2, #12]
    d5f0:	e58260d0 	str	r6, [r2, #208]
    d5f4:	e5827008 	str	r7, [r2, #8]
    d5f8:	e59d3018 	ldr	r3, [sp, #24]
    d5fc:	e3a00001 	mov	r0, #1	; 0x1
    d600:	e58230d4 	str	r3, [r2, #212]
    d604:	e28dd004 	add	sp, sp, #4	; 0x4
    d608:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    d60c:	21e3c000 	.word	0x21e3c000

0000d610 <nand_readid>:
    d610:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    d614:	e1a06000 	mov	r6, r0
    d618:	e24dd010 	sub	sp, sp, #16	; 0x10
    d61c:	e59f013c 	ldr	r0, [pc, #316]	; d760 <nand_readid+0x150>
    d620:	e1a04002 	mov	r4, r2
    d624:	e1a05003 	mov	r5, r3
    d628:	ebffcce2 	bl	9b8 <module_reset>
    d62c:	e59dc02c 	ldr	ip, [sp, #44]
    d630:	e1a01004 	mov	r1, r4
    d634:	e58dc000 	str	ip, [sp]
    d638:	e59dc030 	ldr	ip, [sp, #48]
    d63c:	e1a02005 	mov	r2, r5
    d640:	e59d3028 	ldr	r3, [sp, #40]
    d644:	e3a00000 	mov	r0, #0	; 0x0
    d648:	e58dc004 	str	ip, [sp, #4]
    d64c:	ebffffbb 	bl	d540 <nf2_timing_init>
    d650:	e3a00000 	mov	r0, #0	; 0x0
    d654:	ebffff56 	bl	d3b4 <nf2_asyn_reset>
    d658:	e3500000 	cmp	r0, #0	; 0x0
    d65c:	1a00003b 	bne	d750 <nand_readid+0x140>
    d660:	e2800001 	add	r0, r0, #1	; 0x1
    d664:	ebfffc24 	bl	c6fc <nf2_soft_reset>
    d668:	e2505000 	subs	r5, r0, #0	; 0x0
    d66c:	1a000037 	bne	d750 <nand_readid+0x140>
    d670:	e58d500c 	str	r5, [sp, #12]
    d674:	e3a0230a 	mov	r2, #671088640	; 0x28000000
    d678:	e5923054 	ldr	r3, [r2, #84]
    d67c:	e1a07002 	mov	r7, r2
    d680:	e58d300c 	str	r3, [sp, #12]
    d684:	e59d300c 	ldr	r3, [sp, #12]
    d688:	e59f80d4 	ldr	r8, [pc, #212]	; d764 <nand_readid+0x154>
    d68c:	e3833001 	orr	r3, r3, #1	; 0x1
    d690:	e58d300c 	str	r3, [sp, #12]
    d694:	e59d300c 	ldr	r3, [sp, #12]
    d698:	e5823054 	str	r3, [r2, #84]
    d69c:	e3043090 	movw	r3, #16528	; 0x4090
    d6a0:	e5823060 	str	r3, [r2, #96]
    d6a4:	e2833070 	add	r3, r3, #112	; 0x70
    d6a8:	e5823060 	str	r3, [r2, #96]
    d6ac:	e3a03008 	mov	r3, #8	; 0x8
    d6b0:	e5823060 	str	r3, [r2, #96]
    d6b4:	e3a03c7f 	mov	r3, #32512	; 0x7f00
    d6b8:	e5823060 	str	r3, [r2, #96]
    d6bc:	e3e03001 	mvn	r3, #1	; 0x1
    d6c0:	e5825054 	str	r5, [r2, #84]
    d6c4:	e582304c 	str	r3, [r2, #76]
    d6c8:	e2833003 	add	r3, r3, #3	; 0x3
    d6cc:	e5823084 	str	r3, [r2, #132]
    d6d0:	eb0000b2 	bl	d9a0 <get_ticks>
    d6d4:	e1a04000 	mov	r4, r0
    d6d8:	eb0000b0 	bl	d9a0 <get_ticks>
    d6dc:	e0640000 	rsb	r0, r4, r0
    d6e0:	e1500008 	cmp	r0, r8
    d6e4:	c3e05000 	mvngt	r5, #0	; 0x0
    d6e8:	ca000002 	bgt	d6f8 <nand_readid+0xe8>
    d6ec:	e597308c 	ldr	r3, [r7, #140]
    d6f0:	e3130001 	tst	r3, #1	; 0x1
    d6f4:	0afffff7 	beq	d6d8 <nand_readid+0xc8>
    d6f8:	e3a0030a 	mov	r0, #671088640	; 0x28000000
    d6fc:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    d700:	e580308c 	str	r3, [r0, #140]
    d704:	e3a0300f 	mov	r3, #15	; 0xf
    d708:	e580304c 	str	r3, [r0, #76]
    d70c:	e5903010 	ldr	r3, [r0, #16]
    d710:	e1a02423 	lsr	r2, r3, #8
    d714:	e1a01823 	lsr	r1, r3, #16
    d718:	e1a0cc23 	lsr	ip, r3, #24
    d71c:	e5c62006 	strb	r2, [r6, #6]
    d720:	e5c61005 	strb	r1, [r6, #5]
    d724:	e5c6c004 	strb	ip, [r6, #4]
    d728:	e5c63007 	strb	r3, [r6, #7]
    d72c:	e5903014 	ldr	r3, [r0, #20]
    d730:	e1a02c23 	lsr	r2, r3, #24
    d734:	e1a01423 	lsr	r1, r3, #8
    d738:	e1a00823 	lsr	r0, r3, #16
    d73c:	e5c62000 	strb	r2, [r6]
    d740:	e5c61002 	strb	r1, [r6, #2]
    d744:	e5c60001 	strb	r0, [r6, #1]
    d748:	e5c63003 	strb	r3, [r6, #3]
    d74c:	ea000000 	b	d754 <nand_readid+0x144>
    d750:	e3e05000 	mvn	r5, #0	; 0x0
    d754:	e1a00005 	mov	r0, r5
    d758:	e28dd010 	add	sp, sp, #16	; 0x10
    d75c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    d760:	21e3c000 	.word	0x21e3c000
    d764:	0007a120 	.word	0x0007a120

0000d768 <nf2_hardware_reset>:
    d768:	e92d4030 	push	{r4, r5, lr}
    d76c:	e1a04000 	mov	r4, r0
    d770:	e5d00000 	ldrb	r0, [r0]
    d774:	e594c034 	ldr	ip, [r4, #52]
    d778:	e3500002 	cmp	r0, #2	; 0x2
    d77c:	e5d4e006 	ldrb	lr, [r4, #6]
    d780:	e5943030 	ldr	r3, [r4, #48]
    d784:	e24dd00c 	sub	sp, sp, #12	; 0xc
    d788:	e594202c 	ldr	r2, [r4, #44]
    d78c:	03a00002 	moveq	r0, #2	; 0x2
    d790:	13a00000 	movne	r0, #0	; 0x0
    d794:	e1a05001 	mov	r5, r1
    d798:	e5941028 	ldr	r1, [r4, #40]
    d79c:	e88d5000 	stm	sp, {ip, lr}
    d7a0:	ebffff66 	bl	d540 <nf2_timing_init>
    d7a4:	e3a00000 	mov	r0, #0	; 0x0
    d7a8:	ebffff01 	bl	d3b4 <nf2_asyn_reset>
    d7ac:	e2503000 	subs	r3, r0, #0	; 0x0
    d7b0:	1a00000e 	bne	d7f0 <nf2_hardware_reset+0x88>
    d7b4:	e3550000 	cmp	r5, #0	; 0x0
    d7b8:	0a00000c 	beq	d7f0 <nf2_hardware_reset+0x88>
    d7bc:	e5d40000 	ldrb	r0, [r4]
    d7c0:	e3500001 	cmp	r0, #1	; 0x1
    d7c4:	1a000009 	bne	d7f0 <nf2_hardware_reset+0x88>
    d7c8:	e594c034 	ldr	ip, [r4, #52]
    d7cc:	e5d4e006 	ldrb	lr, [r4, #6]
    d7d0:	e2841028 	add	r1, r4, #40	; 0x28
    d7d4:	e891000e 	ldm	r1, {r1, r2, r3}
    d7d8:	e88d5000 	stm	sp, {ip, lr}
    d7dc:	ebffff57 	bl	d540 <nf2_timing_init>
    d7e0:	e3a00010 	mov	r0, #16	; 0x10
    d7e4:	e28dd00c 	add	sp, sp, #12	; 0xc
    d7e8:	e8bd4030 	pop	{r4, r5, lr}
    d7ec:	eaffff19 	b	d458 <nf2_active_sync>
    d7f0:	e1a00003 	mov	r0, r3
    d7f4:	e28dd00c 	add	sp, sp, #12	; 0xc
    d7f8:	e8bd8030 	pop	{r4, r5, pc}

0000d7fc <serial_tstc>:
    d7fc:	e59f3010 	ldr	r3, [pc, #16]	; d814 <serial_tstc+0x18>
    d800:	e5930018 	ldr	r0, [r3, #24]
    d804:	e1a00220 	lsr	r0, r0, #4
    d808:	e2200001 	eor	r0, r0, #1	; 0x1
    d80c:	e2000001 	and	r0, r0, #1	; 0x1
    d810:	e12fff1e 	bx	lr
    d814:	20803000 	.word	0x20803000

0000d818 <serial_putc>:
    d818:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    d81c:	e24dd004 	sub	sp, sp, #4	; 0x4
    d820:	e1a06000 	mov	r6, r0
    d824:	eb00005d 	bl	d9a0 <get_ticks>
    d828:	e3a04ffa 	mov	r4, #1000	; 0x3e8
    d82c:	e3a05000 	mov	r5, #0	; 0x0
    d830:	e0944000 	adds	r4, r4, r0
    d834:	e59f7034 	ldr	r7, [pc, #52]	; d870 <serial_putc+0x58>
    d838:	e0a55001 	adc	r5, r5, r1
    d83c:	e5973018 	ldr	r3, [r7, #24]
    d840:	e3130020 	tst	r3, #32	; 0x20
    d844:	0a000005 	beq	d860 <serial_putc+0x48>
    d848:	eb000054 	bl	d9a0 <get_ticks>
    d84c:	e1510005 	cmp	r1, r5
    d850:	8a000002 	bhi	d860 <serial_putc+0x48>
    d854:	1afffff8 	bne	d83c <serial_putc+0x24>
    d858:	e1500004 	cmp	r0, r4
    d85c:	9afffff6 	bls	d83c <serial_putc+0x24>
    d860:	e59f3008 	ldr	r3, [pc, #8]	; d870 <serial_putc+0x58>
    d864:	e5836000 	str	r6, [r3]
    d868:	e28dd004 	add	sp, sp, #4	; 0x4
    d86c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    d870:	20803000 	.word	0x20803000

0000d874 <serial_puts>:
    d874:	e92d4030 	push	{r4, r5, lr}
    d878:	e1a05000 	mov	r5, r0
    d87c:	e24dd004 	sub	sp, sp, #4	; 0x4
    d880:	e3a04000 	mov	r4, #0	; 0x0
    d884:	ea000006 	b	d8a4 <serial_puts+0x30>
    d888:	e353000a 	cmp	r3, #10	; 0xa
    d88c:	1a000001 	bne	d898 <serial_puts+0x24>
    d890:	e3a0000d 	mov	r0, #13	; 0xd
    d894:	ebffffdf 	bl	d818 <serial_putc>
    d898:	e7d50004 	ldrb	r0, [r5, r4]
    d89c:	ebffffdd 	bl	d818 <serial_putc>
    d8a0:	e2844001 	add	r4, r4, #1	; 0x1
    d8a4:	e7d53004 	ldrb	r3, [r5, r4]
    d8a8:	e3530000 	cmp	r3, #0	; 0x0
    d8ac:	1afffff5 	bne	d888 <serial_puts+0x14>
    d8b0:	e28dd004 	add	sp, sp, #4	; 0x4
    d8b4:	e8bd8030 	pop	{r4, r5, pc}

0000d8b8 <serial_getc>:
    d8b8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    d8bc:	e59f2024 	ldr	r2, [pc, #36]	; d8e8 <serial_getc+0x30>
    d8c0:	e24dd004 	sub	sp, sp, #4	; 0x4
    d8c4:	e5923018 	ldr	r3, [r2, #24]
    d8c8:	e3130010 	tst	r3, #16	; 0x10
    d8cc:	1afffffc 	bne	d8c4 <serial_getc+0xc>
    d8d0:	ebffdba9 	bl	477c <tt_rnd_seed>
    d8d4:	e59f300c 	ldr	r3, [pc, #12]	; d8e8 <serial_getc+0x30>
    d8d8:	e5930000 	ldr	r0, [r3]
    d8dc:	e6ef0070 	uxtb	r0, r0
    d8e0:	e28dd004 	add	sp, sp, #4	; 0x4
    d8e4:	e8bd8000 	pop	{pc}
    d8e8:	20803000 	.word	0x20803000

0000d8ec <serial_setbrg>:
    d8ec:	e92d4010 	push	{r4, lr}
    d8f0:	e59f0038 	ldr	r0, [pc, #56]	; d930 <serial_setbrg+0x44>
    d8f4:	ebffce42 	bl	1204 <module_get_clock>
    d8f8:	e59f1034 	ldr	r1, [pc, #52]	; d934 <serial_setbrg+0x48>
    d8fc:	e1a04000 	mov	r4, r0
    d900:	eb001c25 	bl	1499c <__aeabi_idiv>
    d904:	e6bf3070 	sxth	r3, r0
    d908:	e59f0028 	ldr	r0, [pc, #40]	; d938 <serial_setbrg+0x4c>
    d90c:	e59f1020 	ldr	r1, [pc, #32]	; d934 <serial_setbrg+0x48>
    d910:	e0204390 	mla	r0, r0, r3, r4
    d914:	e59f4020 	ldr	r4, [pc, #32]	; d93c <serial_setbrg+0x50>
    d918:	e1a00300 	lsl	r0, r0, #6
    d91c:	e5843024 	str	r3, [r4, #36]
    d920:	eb001c1d 	bl	1499c <__aeabi_idiv>
    d924:	e200003f 	and	r0, r0, #63	; 0x3f
    d928:	e5840028 	str	r0, [r4, #40]
    d92c:	e8bd8010 	pop	{r4, pc}
    d930:	21e0a290 	.word	0x21e0a290
    d934:	001c2000 	.word	0x001c2000
    d938:	ffe3e000 	.word	0xffe3e000
    d93c:	20803000 	.word	0x20803000

0000d940 <init_serial>:
    d940:	e92d4010 	push	{r4, lr}
    d944:	e59f004c 	ldr	r0, [pc, #76]	; d998 <init_serial+0x58>
    d948:	e59f404c 	ldr	r4, [pc, #76]	; d99c <init_serial+0x5c>
    d94c:	ebffcb8c 	bl	784 <module_enable>
    d950:	e3a00013 	mov	r0, #19	; 0x13
    d954:	ebffcbc5 	bl	870 <pads_init_module>
    d958:	e594302c 	ldr	r3, [r4, #44]
    d95c:	e3c330ff 	bic	r3, r3, #255	; 0xff
    d960:	e3833060 	orr	r3, r3, #96	; 0x60
    d964:	e584302c 	str	r3, [r4, #44]
    d968:	ebffffdf 	bl	d8ec <serial_setbrg>
    d96c:	e5943030 	ldr	r3, [r4, #48]
    d970:	e3a00000 	mov	r0, #0	; 0x0
    d974:	e3c33cff 	bic	r3, r3, #65280	; 0xff00
    d978:	e3c33087 	bic	r3, r3, #135	; 0x87
    d97c:	e3833c03 	orr	r3, r3, #768	; 0x300
    d980:	e3833001 	orr	r3, r3, #1	; 0x1
    d984:	e5843030 	str	r3, [r4, #48]
    d988:	e594302c 	ldr	r3, [r4, #44]
    d98c:	e3833010 	orr	r3, r3, #16	; 0x10
    d990:	e584302c 	str	r3, [r4, #44]
    d994:	e8bd8010 	pop	{r4, pc}
    d998:	21e12000 	.word	0x21e12000
    d99c:	20803000 	.word	0x20803000

0000d9a0 <get_ticks>:
    d9a0:	e59f1038 	ldr	r1, [pc, #56]	; d9e0 <get_ticks+0x40>
    d9a4:	e5913204 	ldr	r3, [r1, #516]
    d9a8:	e5912200 	ldr	r2, [r1, #512]
    d9ac:	e591c204 	ldr	ip, [r1, #516]
    d9b0:	e15c0003 	cmp	ip, r3
    d9b4:	15913200 	ldrne	r3, [r1, #512]
    d9b8:	01a00002 	moveq	r0, r2
    d9bc:	03a01000 	moveq	r1, #0	; 0x0
    d9c0:	11a00003 	movne	r0, r3
    d9c4:	13a01000 	movne	r1, #0	; 0x0
    d9c8:	e3a02000 	mov	r2, #0	; 0x0
    d9cc:	e1822000 	orr	r2, r2, r0
    d9d0:	e18c3001 	orr	r3, ip, r1
    d9d4:	e1a01003 	mov	r1, r3
    d9d8:	e1a00002 	mov	r0, r2
    d9dc:	e12fff1e 	bx	lr
    d9e0:	21b00000 	.word	0x21b00000

0000d9e4 <udelay>:
    d9e4:	e59f1084 	ldr	r1, [pc, #132]	; da70 <udelay+0x8c>
    d9e8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    d9ec:	e5913204 	ldr	r3, [r1, #516]
    d9f0:	e5912200 	ldr	r2, [r1, #512]
    d9f4:	e591c204 	ldr	ip, [r1, #516]
    d9f8:	e15c0003 	cmp	ip, r3
    d9fc:	15913200 	ldrne	r3, [r1, #512]
    da00:	01a04002 	moveq	r4, r2
    da04:	03a05000 	moveq	r5, #0	; 0x0
    da08:	11a04003 	movne	r4, r3
    da0c:	13a05000 	movne	r5, #0	; 0x0
    da10:	e3a02000 	mov	r2, #0	; 0x0
    da14:	e1822004 	orr	r2, r2, r4
    da18:	e18c3005 	orr	r3, ip, r5
    da1c:	e0926000 	adds	r6, r2, r0
    da20:	e59fc048 	ldr	ip, [pc, #72]	; da70 <udelay+0x8c>
    da24:	e2a37000 	adc	r7, r3, #0	; 0x0
    da28:	e59c3204 	ldr	r3, [ip, #516]
    da2c:	e59c2200 	ldr	r2, [ip, #512]
    da30:	e59c1204 	ldr	r1, [ip, #516]
    da34:	e1510003 	cmp	r1, r3
    da38:	159c3200 	ldrne	r3, [ip, #512]
    da3c:	01a04002 	moveq	r4, r2
    da40:	03a05000 	moveq	r5, #0	; 0x0
    da44:	11a04003 	movne	r4, r3
    da48:	13a05000 	movne	r5, #0	; 0x0
    da4c:	e1851001 	orr	r1, r5, r1
    da50:	e3a02000 	mov	r2, #0	; 0x0
    da54:	e1570001 	cmp	r7, r1
    da58:	e1840002 	orr	r0, r4, r2
    da5c:	8afffff1 	bhi	da28 <udelay+0x44>
    da60:	18bd80f0 	popne	{r4, r5, r6, r7, pc}
    da64:	e1560000 	cmp	r6, r0
    da68:	8affffee 	bhi	da28 <udelay+0x44>
    da6c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    da70:	21b00000 	.word	0x21b00000

0000da74 <get_timer>:
    da74:	e59f2074 	ldr	r2, [pc, #116]	; daf0 <get_timer+0x7c>
    da78:	e92d4070 	push	{r4, r5, r6, lr}
    da7c:	e5923204 	ldr	r3, [r2, #516]
    da80:	e1a06000 	mov	r6, r0
    da84:	e5921200 	ldr	r1, [r2, #512]
    da88:	e5920204 	ldr	r0, [r2, #516]
    da8c:	e24dd008 	sub	sp, sp, #8	; 0x8
    da90:	e1500003 	cmp	r0, r3
    da94:	15923200 	ldrne	r3, [r2, #512]
    da98:	01a04001 	moveq	r4, r1
    da9c:	03a05000 	moveq	r5, #0	; 0x0
    daa0:	11a04003 	movne	r4, r3
    daa4:	13a05000 	movne	r5, #0	; 0x0
    daa8:	e1a03000 	mov	r3, r0
    daac:	e3a02000 	mov	r2, #0	; 0x0
    dab0:	e1851003 	orr	r1, r5, r3
    dab4:	e1840002 	orr	r0, r4, r2
    dab8:	e1510002 	cmp	r1, r2
    dabc:	e88d0003 	stm	sp, {r0, r1}
    dac0:	1a000003 	bne	dad4 <get_timer+0x60>
    dac4:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    dac8:	eb001b31 	bl	14794 <__aeabi_uidiv>
    dacc:	e58d0000 	str	r0, [sp]
    dad0:	ea000002 	b	dae0 <get_timer+0x6c>
    dad4:	e1a0000d 	mov	r0, sp
    dad8:	e3a01ffa 	mov	r1, #1000	; 0x3e8
    dadc:	eb000eeb 	bl	11690 <__div64_32>
    dae0:	e59d0000 	ldr	r0, [sp]
    dae4:	e0660000 	rsb	r0, r6, r0
    dae8:	e28dd008 	add	sp, sp, #8	; 0x8
    daec:	e8bd8070 	pop	{r4, r5, r6, pc}
    daf0:	21b00000 	.word	0x21b00000

0000daf4 <init_timer>:
    daf4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    daf8:	e3a00003 	mov	r0, #3	; 0x3
    dafc:	e24dd004 	sub	sp, sp, #4	; 0x4
    db00:	ebffcdbf 	bl	1204 <module_get_clock>
    db04:	e59f1030 	ldr	r1, [pc, #48]	; db3c <init_timer+0x48>
    db08:	eb001ba3 	bl	1499c <__aeabi_idiv>
    db0c:	e2401001 	sub	r1, r0, #1	; 0x1
    db10:	e59f2028 	ldr	r2, [pc, #40]	; db40 <init_timer+0x4c>
    db14:	e1a01401 	lsl	r1, r1, #8
    db18:	e3a03000 	mov	r3, #0	; 0x0
    db1c:	e3811001 	orr	r1, r1, #1	; 0x1
    db20:	e5823208 	str	r3, [r2, #520]
    db24:	e1a00003 	mov	r0, r3
    db28:	e5823200 	str	r3, [r2, #512]
    db2c:	e5823204 	str	r3, [r2, #516]
    db30:	e5821208 	str	r1, [r2, #520]
    db34:	e28dd004 	add	sp, sp, #4	; 0x4
    db38:	e8bd8000 	pop	{pc}
    db3c:	000f4240 	.word	0x000f4240
    db40:	21b00000 	.word	0x21b00000

0000db44 <e2p_vs_align>:
    db44:	e3a00010 	mov	r0, #16	; 0x10
    db48:	e12fff1e 	bx	lr

0000db4c <e2p_vs_reset>:
    db4c:	e3a00000 	mov	r0, #0	; 0x0
    db50:	e12fff1e 	bx	lr

0000db54 <iic_address_num>:
    db54:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    db58:	e24dd004 	sub	sp, sp, #4	; 0x4
    db5c:	ebffcaa5 	bl	5f8 <boot_info>
    db60:	e59f3014 	ldr	r3, [pc, #20]	; db7c <iic_address_num+0x28>
    db64:	e1a00080 	lsl	r0, r0, #1
    db68:	e20000ff 	and	r0, r0, #255	; 0xff
    db6c:	e5830000 	str	r0, [r3]
    db70:	e3a00001 	mov	r0, #1	; 0x1
    db74:	e28dd004 	add	sp, sp, #4	; 0x4
    db78:	e8bd8000 	pop	{pc}
    db7c:	3c00d214 	.word	0x3c00d214

0000db80 <iic_read_get_address>:
    db80:	e1d020b2 	ldrh	r2, [r0, #2]
    db84:	e590300c 	ldr	r3, [r0, #12]
    db88:	e92d4010 	push	{r4, lr}
    db8c:	e0833002 	add	r3, r3, r2
    db90:	e1c030b2 	strh	r3, [r0, #2]
    db94:	e1a04000 	mov	r4, r0
    db98:	ebffffed 	bl	db54 <iic_address_num>
    db9c:	e3500000 	cmp	r0, #0	; 0x0
    dba0:	1a000008 	bne	dbc8 <iic_read_get_address+0x48>
    dba4:	e1d430b2 	ldrh	r3, [r4, #2]
    dba8:	e3530c01 	cmp	r3, #256	; 0x100
    dbac:	1a000005 	bne	dbc8 <iic_read_get_address+0x48>
    dbb0:	e1d430b0 	ldrh	r3, [r4]
    dbb4:	e1c400b2 	strh	r0, [r4, #2]
    dbb8:	e2833002 	add	r3, r3, #2	; 0x2
    dbbc:	e1c430b0 	strh	r3, [r4]
    dbc0:	e3a03001 	mov	r3, #1	; 0x1
    dbc4:	e1c431b0 	strh	r3, [r4, #16]
    dbc8:	ebffffe1 	bl	db54 <iic_address_num>
    dbcc:	e3500000 	cmp	r0, #0	; 0x0
    dbd0:	e5941008 	ldr	r1, [r4, #8]
    dbd4:	0a000004 	beq	dbec <iic_read_get_address+0x6c>
    dbd8:	e3510000 	cmp	r1, #0	; 0x0
    dbdc:	13a03000 	movne	r3, #0	; 0x0
    dbe0:	15843008 	strne	r3, [r4, #8]
    dbe4:	1a00000e 	bne	dc24 <iic_read_get_address+0xa4>
    dbe8:	ea000010 	b	dc30 <iic_read_get_address+0xb0>
    dbec:	e1d430b2 	ldrh	r3, [r4, #2]
    dbf0:	e2632c01 	rsb	r2, r3, #256	; 0x100
    dbf4:	e1520001 	cmp	r2, r1
    dbf8:	e5842004 	str	r2, [r4, #4]
    dbfc:	2a000005 	bcs	dc18 <iic_read_get_address+0x98>
    dc00:	e0623001 	rsb	r3, r2, r1
    dc04:	e5843008 	str	r3, [r4, #8]
    dc08:	e3a03001 	mov	r3, #1	; 0x1
    dc0c:	e1c431b0 	strh	r3, [r4, #16]
    dc10:	e584200c 	str	r2, [r4, #12]
    dc14:	ea000003 	b	dc28 <iic_read_get_address+0xa8>
    dc18:	e3510000 	cmp	r1, #0	; 0x0
    dc1c:	0a000003 	beq	dc30 <iic_read_get_address+0xb0>
    dc20:	e5840008 	str	r0, [r4, #8]
    dc24:	e584100c 	str	r1, [r4, #12]
    dc28:	e3a00001 	mov	r0, #1	; 0x1
    dc2c:	e8bd8010 	pop	{r4, pc}
    dc30:	e3a00000 	mov	r0, #0	; 0x0
    dc34:	e8bd8010 	pop	{r4, pc}

0000dc38 <iic_write_get_address>:
    dc38:	e1d020b2 	ldrh	r2, [r0, #2]
    dc3c:	e590300c 	ldr	r3, [r0, #12]
    dc40:	e92d4010 	push	{r4, lr}
    dc44:	e0833002 	add	r3, r3, r2
    dc48:	e1c030b2 	strh	r3, [r0, #2]
    dc4c:	e1a04000 	mov	r4, r0
    dc50:	ebffffbf 	bl	db54 <iic_address_num>
    dc54:	e3500000 	cmp	r0, #0	; 0x0
    dc58:	1a000008 	bne	dc80 <iic_write_get_address+0x48>
    dc5c:	e1d430b2 	ldrh	r3, [r4, #2]
    dc60:	e3530c01 	cmp	r3, #256	; 0x100
    dc64:	1a000005 	bne	dc80 <iic_write_get_address+0x48>
    dc68:	e1d430b0 	ldrh	r3, [r4]
    dc6c:	e1c400b2 	strh	r0, [r4, #2]
    dc70:	e2833002 	add	r3, r3, #2	; 0x2
    dc74:	e1c430b0 	strh	r3, [r4]
    dc78:	e3a03001 	mov	r3, #1	; 0x1
    dc7c:	e1c431b0 	strh	r3, [r4, #16]
    dc80:	e1d430b2 	ldrh	r3, [r4, #2]
    dc84:	e5940008 	ldr	r0, [r4, #8]
    dc88:	e203300f 	and	r3, r3, #15	; 0xf
    dc8c:	e2632010 	rsb	r2, r3, #16	; 0x10
    dc90:	e1520000 	cmp	r2, r0
    dc94:	e5842004 	str	r2, [r4, #4]
    dc98:	2a000004 	bcs	dcb0 <iic_write_get_address+0x78>
    dc9c:	e0623000 	rsb	r3, r2, r0
    dca0:	e5843008 	str	r3, [r4, #8]
    dca4:	e584200c 	str	r2, [r4, #12]
    dca8:	e3a00001 	mov	r0, #1	; 0x1
    dcac:	e8bd8010 	pop	{r4, pc}
    dcb0:	e3500000 	cmp	r0, #0	; 0x0
    dcb4:	13a03000 	movne	r3, #0	; 0x0
    dcb8:	1584000c 	strne	r0, [r4, #12]
    dcbc:	15843008 	strne	r3, [r4, #8]
    dcc0:	13a00001 	movne	r0, #1	; 0x1
    dcc4:	e8bd8010 	pop	{r4, pc}

0000dcc8 <iic_word_addr_func>:
    dcc8:	e92d4030 	push	{r4, r5, lr}
    dccc:	e24dd004 	sub	sp, sp, #4	; 0x4
    dcd0:	e1a04000 	mov	r4, r0
    dcd4:	e1a05001 	mov	r5, r1
    dcd8:	ebffff9d 	bl	db54 <iic_address_num>
    dcdc:	e3500000 	cmp	r0, #0	; 0x0
    dce0:	16ff0074 	uxthne	r0, r4
    dce4:	1a00000c 	bne	dd1c <iic_word_addr_func+0x54>
    dce8:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
    dcec:	e0051003 	and	r1, r5, r3
    dcf0:	e3a020ff 	mov	r2, #255	; 0xff
    dcf4:	e3510000 	cmp	r1, #0	; 0x0
    dcf8:	e0040002 	and	r0, r4, r2
    dcfc:	aa000005 	bge	dd18 <iic_word_addr_func+0x50>
    dd00:	e3e02000 	mvn	r2, #0	; 0x0
    dd04:	e0900002 	adds	r0, r0, r2
    dd08:	e3e020ff 	mvn	r2, #255	; 0xff
    dd0c:	e1800002 	orr	r0, r0, r2
    dd10:	e3a02001 	mov	r2, #1	; 0x1
    dd14:	e0900002 	adds	r0, r0, r2
    dd18:	e6ff0070 	uxth	r0, r0
    dd1c:	e28dd004 	add	sp, sp, #4	; 0x4
    dd20:	e8bd8030 	pop	{r4, r5, pc}

0000dd24 <iic_addr_func>:
    dd24:	e92d4030 	push	{r4, r5, lr}
    dd28:	e24dd004 	sub	sp, sp, #4	; 0x4
    dd2c:	e1a04000 	mov	r4, r0
    dd30:	e1a05001 	mov	r5, r1
    dd34:	ebffff86 	bl	db54 <iic_address_num>
    dd38:	e3500000 	cmp	r0, #0	; 0x0
    dd3c:	e59f003c 	ldr	r0, [pc, #60]	; dd80 <iic_addr_func+0x5c>
    dd40:	11d000b0 	ldrhne	r0, [r0]
    dd44:	1a00000b 	bne	dd78 <iic_addr_func+0x54>
    dd48:	e1a02005 	mov	r2, r5
    dd4c:	e1a03fc2 	asr	r3, r2, #31
    dd50:	e1a02fc3 	asr	r2, r3, #31
    dd54:	e20220ff 	and	r2, r2, #255	; 0xff
    dd58:	e3a03000 	mov	r3, #0	; 0x0
    dd5c:	e0922004 	adds	r2, r2, r4
    dd60:	e0a33005 	adc	r3, r3, r5
    dd64:	e5900000 	ldr	r0, [r0]
    dd68:	e1a01422 	lsr	r1, r2, #8
    dd6c:	e1811c03 	orr	r1, r1, r3, lsl #24
    dd70:	e1800081 	orr	r0, r0, r1, lsl #1
    dd74:	e6ff0070 	uxth	r0, r0
    dd78:	e28dd004 	add	sp, sp, #4	; 0x4
    dd7c:	e8bd8030 	pop	{r4, r5, pc}
    dd80:	3c00d214 	.word	0x3c00d214

0000dd84 <e2p_vs_write>:
    dd84:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    dd88:	e1a01003 	mov	r1, r3
    dd8c:	e24dd024 	sub	sp, sp, #36	; 0x24
    dd90:	e1a06000 	mov	r6, r0
    dd94:	e1a00002 	mov	r0, r2
    dd98:	e1a05003 	mov	r5, r3
    dd9c:	e1a04002 	mov	r4, r2
    dda0:	ebffffdf 	bl	dd24 <iic_addr_func>
    dda4:	e1a01005 	mov	r1, r5
    dda8:	e1cd00bc 	strh	r0, [sp, #12]
    ddac:	e1a00004 	mov	r0, r4
    ddb0:	ebffffc4 	bl	dcc8 <iic_word_addr_func>
    ddb4:	e3a03000 	mov	r3, #0	; 0x0
    ddb8:	e59d2048 	ldr	r2, [sp, #72]
    ddbc:	e1a05003 	mov	r5, r3
    ddc0:	e58d3018 	str	r3, [sp, #24]
    ddc4:	e28dc00c 	add	ip, sp, #12	; 0xc
    ddc8:	e3a03001 	mov	r3, #1	; 0x1
    ddcc:	e1cd00be 	strh	r0, [sp, #14]
    ddd0:	e58d2014 	str	r2, [sp, #20]
    ddd4:	e1cd31bc 	strh	r3, [sp, #28]
    ddd8:	e59fb10c 	ldr	fp, [pc, #268]	; deec <e2p_vs_write+0x168>
    dddc:	e3a0a001 	mov	sl, #1	; 0x1
    dde0:	e1a08005 	mov	r8, r5
    dde4:	e28c9012 	add	r9, ip, #18	; 0x12
    dde8:	ea000037 	b	decc <e2p_vs_write+0x148>
    ddec:	ebffff58 	bl	db54 <iic_address_num>
    ddf0:	e3500000 	cmp	r0, #0	; 0x0
    ddf4:	e1dd10be 	ldrh	r1, [sp, #14]
    ddf8:	11a03421 	lsrne	r3, r1, #8
    ddfc:	15cd301e 	strbne	r3, [sp, #30]
    de00:	15cd101f 	strbne	r1, [sp, #31]
    de04:	13a07002 	movne	r7, #2	; 0x2
    de08:	1a000005 	bne	de24 <e2p_vs_write+0xa0>
    de0c:	e6ef1071 	uxtb	r1, r1
    de10:	e5cd101e 	strb	r1, [sp, #30]
    de14:	e59f00d4 	ldr	r0, [pc, #212]	; def0 <e2p_vs_write+0x16c>
    de18:	e1a0e00f 	mov	lr, pc
    de1c:	e59bf000 	ldr	pc, [fp]
    de20:	e3a07001 	mov	r7, #1	; 0x1
    de24:	ebfffedd 	bl	d9a0 <get_ticks>
    de28:	e1dd31bc 	ldrh	r3, [sp, #28]
    de2c:	e1a04000 	mov	r4, r0
    de30:	e3530000 	cmp	r3, #0	; 0x0
    de34:	0a000016 	beq	de94 <e2p_vs_write+0x110>
    de38:	e3a0c000 	mov	ip, #0	; 0x0
    de3c:	e5dd200c 	ldrb	r2, [sp, #12]
    de40:	e1a00008 	mov	r0, r8
    de44:	e1a0100a 	mov	r1, sl
    de48:	e1a03008 	mov	r3, r8
    de4c:	e1cdc1bc 	strh	ip, [sp, #28]
    de50:	e58da000 	str	sl, [sp]
    de54:	e58d8004 	str	r8, [sp, #4]
    de58:	eb000137 	bl	e33c <iic_init>
    de5c:	e3500000 	cmp	r0, #0	; 0x0
    de60:	1a00000b 	bne	de94 <e2p_vs_write+0x110>
    de64:	e1a00008 	mov	r0, r8
    de68:	ea00001d 	b	dee4 <e2p_vs_write+0x160>
    de6c:	ebfffecb 	bl	d9a0 <get_ticks>
    de70:	e1540000 	cmp	r4, r0
    de74:	2a000008 	bcs	de9c <e2p_vs_write+0x118>
    de78:	e59f0074 	ldr	r0, [pc, #116]	; def4 <e2p_vs_write+0x170>
    de7c:	e1a0e00f 	mov	lr, pc
    de80:	e59bf000 	ldr	pc, [fp]
    de84:	e59d3018 	ldr	r3, [sp, #24]
    de88:	e0853003 	add	r3, r5, r3
    de8c:	e6ff5073 	uxth	r5, r3
    de90:	ea00000b 	b	dec4 <e2p_vs_write+0x140>
    de94:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    de98:	e2844e12 	add	r4, r4, #288	; 0x120
    de9c:	e59dc018 	ldr	ip, [sp, #24]
    dea0:	e1a01009 	mov	r1, r9
    dea4:	e1a02007 	mov	r2, r7
    dea8:	e1a03006 	mov	r3, r6
    deac:	e3a00000 	mov	r0, #0	; 0x0
    deb0:	e58dc000 	str	ip, [sp]
    deb4:	e58da004 	str	sl, [sp, #4]
    deb8:	eb000078 	bl	e0a0 <iicreg_write>
    debc:	e3500000 	cmp	r0, #0	; 0x0
    dec0:	0affffe9 	beq	de6c <e2p_vs_write+0xe8>
    dec4:	e59d3018 	ldr	r3, [sp, #24]
    dec8:	e0866003 	add	r6, r6, r3
    decc:	e28d000c 	add	r0, sp, #12	; 0xc
    ded0:	ebffff58 	bl	dc38 <iic_write_get_address>
    ded4:	e3500000 	cmp	r0, #0	; 0x0
    ded8:	1affffc3 	bne	ddec <e2p_vs_write+0x68>
    dedc:	e59d2048 	ldr	r2, [sp, #72]
    dee0:	e0650002 	rsb	r0, r5, r2
    dee4:	e28dd024 	add	sp, sp, #36	; 0x24
    dee8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    deec:	3c00d6cc 	.word	0x3c00d6cc
    def0:	0001881f 	.word	0x0001881f
    def4:	00018829 	.word	0x00018829

0000def8 <e2p_vs_read>:
    def8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    defc:	e1a01003 	mov	r1, r3
    df00:	e24dd024 	sub	sp, sp, #36	; 0x24
    df04:	e1a06000 	mov	r6, r0
    df08:	e1a00002 	mov	r0, r2
    df0c:	e1a05003 	mov	r5, r3
    df10:	e1a04002 	mov	r4, r2
    df14:	e59d7048 	ldr	r7, [sp, #72]
    df18:	ebffff81 	bl	dd24 <iic_addr_func>
    df1c:	e1a01005 	mov	r1, r5
    df20:	e1cd00bc 	strh	r0, [sp, #12]
    df24:	e1a00004 	mov	r0, r4
    df28:	ebffff66 	bl	dcc8 <iic_word_addr_func>
    df2c:	e28db00c 	add	fp, sp, #12	; 0xc
    df30:	e3a03000 	mov	r3, #0	; 0x0
    df34:	e3a02001 	mov	r2, #1	; 0x1
    df38:	e1cd00be 	strh	r0, [sp, #14]
    df3c:	e1cd21bc 	strh	r2, [sp, #28]
    df40:	e58d7014 	str	r7, [sp, #20]
    df44:	e58d3018 	str	r3, [sp, #24]
    df48:	e1a05003 	mov	r5, r3
    df4c:	e3a09001 	mov	r9, #1	; 0x1
    df50:	e28ba012 	add	sl, fp, #18	; 0x12
    df54:	ea00002b 	b	e008 <e2p_vs_read+0x110>
    df58:	ebfffefd 	bl	db54 <iic_address_num>
    df5c:	e1dd20be 	ldrh	r2, [sp, #14]
    df60:	e3500000 	cmp	r0, #0	; 0x0
    df64:	11a03422 	lsrne	r3, r2, #8
    df68:	15cd301e 	strbne	r3, [sp, #30]
    df6c:	15cd201f 	strbne	r2, [sp, #31]
    df70:	13a08002 	movne	r8, #2	; 0x2
    df74:	05cd201e 	strbeq	r2, [sp, #30]
    df78:	03a08001 	moveq	r8, #1	; 0x1
    df7c:	ebfffe87 	bl	d9a0 <get_ticks>
    df80:	e1dd31bc 	ldrh	r3, [sp, #28]
    df84:	e1a04000 	mov	r4, r0
    df88:	e3530000 	cmp	r3, #0	; 0x0
    df8c:	0a000010 	beq	dfd4 <e2p_vs_read+0xdc>
    df90:	e3a0c000 	mov	ip, #0	; 0x0
    df94:	e5dd200c 	ldrb	r2, [sp, #12]
    df98:	e1a00005 	mov	r0, r5
    df9c:	e1a01009 	mov	r1, r9
    dfa0:	e1a03005 	mov	r3, r5
    dfa4:	e1cdc1bc 	strh	ip, [sp, #28]
    dfa8:	e58d9000 	str	r9, [sp]
    dfac:	e58d5004 	str	r5, [sp, #4]
    dfb0:	eb0000e1 	bl	e33c <iic_init>
    dfb4:	e3500000 	cmp	r0, #0	; 0x0
    dfb8:	1a000005 	bne	dfd4 <e2p_vs_read+0xdc>
    dfbc:	e1a07005 	mov	r7, r5
    dfc0:	ea000014 	b	e018 <e2p_vs_read+0x120>
    dfc4:	ebfffe75 	bl	d9a0 <get_ticks>
    dfc8:	e1540000 	cmp	r4, r0
    dfcc:	2a000002 	bcs	dfdc <e2p_vs_read+0xe4>
    dfd0:	ea00000a 	b	e000 <e2p_vs_read+0x108>
    dfd4:	e2844a7a 	add	r4, r4, #499712	; 0x7a000
    dfd8:	e2844e12 	add	r4, r4, #288	; 0x120
    dfdc:	e59dc018 	ldr	ip, [sp, #24]
    dfe0:	e1a0100a 	mov	r1, sl
    dfe4:	e1a02008 	mov	r2, r8
    dfe8:	e1a03006 	mov	r3, r6
    dfec:	e3a00000 	mov	r0, #0	; 0x0
    dff0:	e58dc000 	str	ip, [sp]
    dff4:	eb000061 	bl	e180 <iic_read_func>
    dff8:	e3500000 	cmp	r0, #0	; 0x0
    dffc:	0afffff0 	beq	dfc4 <e2p_vs_read+0xcc>
    e000:	e59d3018 	ldr	r3, [sp, #24]
    e004:	e0866003 	add	r6, r6, r3
    e008:	e1a0000b 	mov	r0, fp
    e00c:	ebfffedb 	bl	db80 <iic_read_get_address>
    e010:	e3500000 	cmp	r0, #0	; 0x0
    e014:	1affffcf 	bne	df58 <e2p_vs_read+0x60>
    e018:	e1a00007 	mov	r0, r7
    e01c:	e28dd024 	add	sp, sp, #36	; 0x24
    e020:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

0000e024 <iicreg_read>:
    e024:	e3500001 	cmp	r0, #1	; 0x1
    e028:	e92d4070 	push	{r4, r5, r6, lr}
    e02c:	83a00000 	movhi	r0, #0	; 0x0
    e030:	e1a05001 	mov	r5, r1
    e034:	e1a04002 	mov	r4, r2
    e038:	88bd8070 	pophi	{r4, r5, r6, pc}
    e03c:	e59f3058 	ldr	r3, [pc, #88]	; e09c <iicreg_read+0x78>
    e040:	e5932070 	ldr	r2, [r3, #112]
    e044:	e2022008 	and	r2, r2, #8	; 0x8
    e048:	ea000000 	b	e050 <iicreg_read+0x2c>
    e04c:	e5931010 	ldr	r1, [r3, #16]
    e050:	e3520000 	cmp	r2, #0	; 0x0
    e054:	1afffffc 	bne	e04c <iicreg_read+0x28>
    e058:	e59f103c 	ldr	r1, [pc, #60]	; e09c <iicreg_read+0x78>
    e05c:	e1a00004 	mov	r0, r4
    e060:	e3a06c01 	mov	r6, #256	; 0x100
    e064:	e591c070 	ldr	ip, [r1, #112]
    e068:	e2503000 	subs	r3, r0, #0	; 0x0
    e06c:	13a03001 	movne	r3, #1	; 0x1
    e070:	e01330ac 	ands	r3, r3, ip, lsr #1
    e074:	15816010 	strne	r6, [r1, #16]
    e078:	12400001 	subne	r0, r0, #1	; 0x1
    e07c:	e31c0008 	tst	ip, #8	; 0x8
    e080:	15913010 	ldrne	r3, [r1, #16]
    e084:	12822001 	addne	r2, r2, #1	; 0x1
    e088:	14c53001 	strbne	r3, [r5], #1
    e08c:	e1520004 	cmp	r2, r4
    e090:	1afffff3 	bne	e064 <iicreg_read+0x40>
    e094:	e3a00001 	mov	r0, #1	; 0x1
    e098:	e8bd8070 	pop	{r4, r5, r6, pc}
    e09c:	20300000 	.word	0x20300000

0000e0a0 <iicreg_write>:
    e0a0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    e0a4:	e59d4018 	ldr	r4, [sp, #24]
    e0a8:	e3500001 	cmp	r0, #1	; 0x1
    e0ac:	e1a05001 	mov	r5, r1
    e0b0:	e5dd801c 	ldrb	r8, [sp, #28]
    e0b4:	e1a01003 	mov	r1, r3
    e0b8:	e0847002 	add	r7, r4, r2
    e0bc:	8a00002c 	bhi	e174 <iicreg_write+0xd4>
    e0c0:	e59f00b4 	ldr	r0, [pc, #180]	; e17c <iicreg_write+0xdc>
    e0c4:	e5903034 	ldr	r3, [r0, #52]
    e0c8:	e213c040 	ands	ip, r3, #64	; 0x40
    e0cc:	15903080 	ldrne	r3, [r0, #128]
    e0d0:	15903054 	ldrne	r3, [r0, #84]
    e0d4:	13a00000 	movne	r0, #0	; 0x0
    e0d8:	18bd81f0 	popne	{r4, r5, r6, r7, r8, pc}
    e0dc:	e5903070 	ldr	r3, [r0, #112]
    e0e0:	e3130004 	tst	r3, #4	; 0x4
    e0e4:	11a06000 	movne	r6, r0
    e0e8:	11a0000c 	movne	r0, ip
    e0ec:	11a0c006 	movne	ip, r6
    e0f0:	1a00000d 	bne	e12c <iicreg_write+0x8c>
    e0f4:	ea00001e 	b	e174 <iicreg_write+0xd4>
    e0f8:	e5963070 	ldr	r3, [r6, #112]
    e0fc:	e3130002 	tst	r3, #2	; 0x2
    e100:	0afffffc 	beq	e0f8 <iicreg_write+0x58>
    e104:	e3520000 	cmp	r2, #0	; 0x0
    e108:	14d53001 	ldrbne	r3, [r5], #1
    e10c:	12422001 	subne	r2, r2, #1	; 0x1
    e110:	1a000003 	bne	e124 <iicreg_write+0x84>
    e114:	e3540000 	cmp	r4, #0	; 0x0
    e118:	0a000002 	beq	e128 <iicreg_write+0x88>
    e11c:	e4d13001 	ldrb	r3, [r1], #1
    e120:	e2444001 	sub	r4, r4, #1	; 0x1
    e124:	e58c3010 	str	r3, [ip, #16]
    e128:	e2800001 	add	r0, r0, #1	; 0x1
    e12c:	e1500007 	cmp	r0, r7
    e130:	3afffff0 	bcc	e0f8 <iicreg_write+0x58>
    e134:	e59f2040 	ldr	r2, [pc, #64]	; e17c <iicreg_write+0xdc>
    e138:	e5923070 	ldr	r3, [r2, #112]
    e13c:	e3130004 	tst	r3, #4	; 0x4
    e140:	0afffffc 	beq	e138 <iicreg_write+0x98>
    e144:	e3580000 	cmp	r8, #0	; 0x0
    e148:	159f202c 	ldrne	r2, [pc, #44]	; e17c <iicreg_write+0xdc>
    e14c:	0a000006 	beq	e16c <iicreg_write+0xcc>
    e150:	e5923034 	ldr	r3, [r2, #52]
    e154:	e3130c02 	tst	r3, #512	; 0x200
    e158:	0afffffc 	beq	e150 <iicreg_write+0xb0>
    e15c:	e59f3018 	ldr	r3, [pc, #24]	; e17c <iicreg_write+0xdc>
    e160:	e3010964 	movw	r0, #6500	; 0x1964
    e164:	e5933060 	ldr	r3, [r3, #96]
    e168:	ebfffe1d 	bl	d9e4 <udelay>
    e16c:	e3a00001 	mov	r0, #1	; 0x1
    e170:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    e174:	e3a00000 	mov	r0, #0	; 0x0
    e178:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    e17c:	20300000 	.word	0x20300000

0000e180 <iic_read_func>:
    e180:	e92d4070 	push	{r4, r5, r6, lr}
    e184:	e59f4080 	ldr	r4, [pc, #128]	; e20c <iic_read_func+0x8c>
    e188:	e1a06003 	mov	r6, r3
    e18c:	e5d43000 	ldrb	r3, [r4]
    e190:	e24dd008 	sub	sp, sp, #8	; 0x8
    e194:	e243300a 	sub	r3, r3, #10	; 0xa
    e198:	e6ef3073 	uxtb	r3, r3
    e19c:	e3530001 	cmp	r3, #1	; 0x1
    e1a0:	e1a05000 	mov	r5, r0
    e1a4:	9a00000b 	bls	e1d8 <iic_read_func+0x58>
    e1a8:	e3a0e000 	mov	lr, #0	; 0x0
    e1ac:	e1a0300e 	mov	r3, lr
    e1b0:	e3a0c001 	mov	ip, #1	; 0x1
    e1b4:	e58dc004 	str	ip, [sp, #4]
    e1b8:	e58de000 	str	lr, [sp]
    e1bc:	ebffffb7 	bl	e0a0 <iicreg_write>
    e1c0:	e3500000 	cmp	r0, #0	; 0x0
    e1c4:	03a0300c 	moveq	r3, #12	; 0xc
    e1c8:	05c43000 	strbeq	r3, [r4]
    e1cc:	0a00000c 	beq	e204 <iic_read_func+0x84>
    e1d0:	e3a0300a 	mov	r3, #10	; 0xa
    e1d4:	e5c43000 	strb	r3, [r4]
    e1d8:	e59d2018 	ldr	r2, [sp, #24]
    e1dc:	e1a00005 	mov	r0, r5
    e1e0:	e1a01006 	mov	r1, r6
    e1e4:	ebffff8e 	bl	e024 <iicreg_read>
    e1e8:	e59f201c 	ldr	r2, [pc, #28]	; e20c <iic_read_func+0x8c>
    e1ec:	e3500000 	cmp	r0, #0	; 0x0
    e1f0:	13a0300d 	movne	r3, #13	; 0xd
    e1f4:	03a0300b 	moveq	r3, #11	; 0xb
    e1f8:	15c23000 	strbne	r3, [r2]
    e1fc:	13a00001 	movne	r0, #1	; 0x1
    e200:	05c23000 	strbeq	r3, [r2]
    e204:	e28dd008 	add	sp, sp, #8	; 0x8
    e208:	e8bd8070 	pop	{r4, r5, r6, pc}
    e20c:	3c00d218 	.word	0x3c00d218

0000e210 <iic_writeread>:
    e210:	e92d4070 	push	{r4, r5, r6, lr}
    e214:	e59f4088 	ldr	r4, [pc, #136]	; e2a4 <iic_writeread+0x94>
    e218:	e24dd010 	sub	sp, sp, #16	; 0x10
    e21c:	e5d43000 	ldrb	r3, [r4]
    e220:	e5cd100f 	strb	r1, [sp, #15]
    e224:	e243300a 	sub	r3, r3, #10	; 0xa
    e228:	e6ef3073 	uxtb	r3, r3
    e22c:	e3530001 	cmp	r3, #1	; 0x1
    e230:	e1a06002 	mov	r6, r2
    e234:	e1a05000 	mov	r5, r0
    e238:	9a00000c 	bls	e270 <iic_writeread+0x60>
    e23c:	e3a0c000 	mov	ip, #0	; 0x0
    e240:	e1a0300c 	mov	r3, ip
    e244:	e28d100f 	add	r1, sp, #15	; 0xf
    e248:	e3a02001 	mov	r2, #1	; 0x1
    e24c:	e58dc000 	str	ip, [sp]
    e250:	e58dc004 	str	ip, [sp, #4]
    e254:	ebffff91 	bl	e0a0 <iicreg_write>
    e258:	e3500000 	cmp	r0, #0	; 0x0
    e25c:	03a0300c 	moveq	r3, #12	; 0xc
    e260:	05c43000 	strbeq	r3, [r4]
    e264:	0a00000c 	beq	e29c <iic_writeread+0x8c>
    e268:	e3a0300a 	mov	r3, #10	; 0xa
    e26c:	e5c43000 	strb	r3, [r4]
    e270:	e3a02001 	mov	r2, #1	; 0x1
    e274:	e1a00005 	mov	r0, r5
    e278:	e1a01006 	mov	r1, r6
    e27c:	ebffff68 	bl	e024 <iicreg_read>
    e280:	e59f201c 	ldr	r2, [pc, #28]	; e2a4 <iic_writeread+0x94>
    e284:	e3500000 	cmp	r0, #0	; 0x0
    e288:	13a0300d 	movne	r3, #13	; 0xd
    e28c:	03a0300b 	moveq	r3, #11	; 0xb
    e290:	15c23000 	strbne	r3, [r2]
    e294:	13a00001 	movne	r0, #1	; 0x1
    e298:	05c23000 	strbeq	r3, [r2]
    e29c:	e28dd010 	add	sp, sp, #16	; 0x10
    e2a0:	e8bd8070 	pop	{r4, r5, r6, pc}
    e2a4:	3c00d218 	.word	0x3c00d218

0000e2a8 <iicreg_init>:
    e2a8:	e92d4030 	push	{r4, r5, lr}
    e2ac:	e1a04000 	mov	r4, r0
    e2b0:	e24dd004 	sub	sp, sp, #4	; 0x4
    e2b4:	e59f0074 	ldr	r0, [pc, #116]	; e330 <iicreg_init+0x88>
    e2b8:	e1a05002 	mov	r5, r2
    e2bc:	ebffcbd0 	bl	1204 <module_get_clock>
    e2c0:	e3540001 	cmp	r4, #1	; 0x1
    e2c4:	83a00000 	movhi	r0, #0	; 0x0
    e2c8:	8a000016 	bhi	e328 <iicreg_init+0x80>
    e2cc:	e59f1060 	ldr	r1, [pc, #96]	; e334 <iicreg_init+0x8c>
    e2d0:	eb00192f 	bl	14794 <__aeabi_uidiv>
    e2d4:	e59f205c 	ldr	r2, [pc, #92]	; e338 <iicreg_init+0x90>
    e2d8:	e1a010a0 	lsr	r1, r0, #1
    e2dc:	e3a0c000 	mov	ip, #0	; 0x0
    e2e0:	e3a03043 	mov	r3, #67	; 0x43
    e2e4:	e582c06c 	str	ip, [r2, #108]
    e2e8:	e5823000 	str	r3, [r2]
    e2ec:	e5820014 	str	r0, [r2, #20]
    e2f0:	e5820018 	str	r0, [r2, #24]
    e2f4:	e58210a0 	str	r1, [r2, #160]
    e2f8:	e59230a0 	ldr	r3, [r2, #160]
    e2fc:	e1a000a5 	lsr	r0, r5, #1
    e300:	e3c33c01 	bic	r3, r3, #256	; 0x100
    e304:	e58230a0 	str	r3, [r2, #160]
    e308:	e5923000 	ldr	r3, [r2]
    e30c:	e3a01001 	mov	r1, #1	; 0x1
    e310:	e3833020 	orr	r3, r3, #32	; 0x20
    e314:	e5823000 	str	r3, [r2]
    e318:	e582c030 	str	ip, [r2, #48]
    e31c:	e5820004 	str	r0, [r2, #4]
    e320:	e582106c 	str	r1, [r2, #108]
    e324:	e1a00001 	mov	r0, r1
    e328:	e28dd004 	add	sp, sp, #4	; 0x4
    e32c:	e8bd8030 	pop	{r4, r5, pc}
    e330:	21e0a360 	.word	0x21e0a360
    e334:	0009c400 	.word	0x0009c400
    e338:	20300000 	.word	0x20300000

0000e33c <iic_init>:
    e33c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    e340:	e59fc054 	ldr	ip, [pc, #84]	; e39c <iic_init+0x60>
    e344:	e24dd004 	sub	sp, sp, #4	; 0x4
    e348:	e3a0e00d 	mov	lr, #13	; 0xd
    e34c:	e1a0a000 	mov	sl, r0
    e350:	e59f0048 	ldr	r0, [pc, #72]	; e3a0 <iic_init+0x64>
    e354:	e1a04001 	mov	r4, r1
    e358:	e1a05002 	mov	r5, r2
    e35c:	e1a06003 	mov	r6, r3
    e360:	e5cce000 	strb	lr, [ip]
    e364:	e5dd7020 	ldrb	r7, [sp, #32]
    e368:	e5dd8024 	ldrb	r8, [sp, #36]
    e36c:	ebffc904 	bl	784 <module_enable>
    e370:	e3a0000a 	mov	r0, #10	; 0xa
    e374:	ebffc93d 	bl	870 <pads_init_module>
    e378:	e1a0000a 	mov	r0, sl
    e37c:	e1a01004 	mov	r1, r4
    e380:	e1a02005 	mov	r2, r5
    e384:	e1a03006 	mov	r3, r6
    e388:	e58d7020 	str	r7, [sp, #32]
    e38c:	e58d8024 	str	r8, [sp, #36]
    e390:	e28dd004 	add	sp, sp, #4	; 0x4
    e394:	e8bd45f0 	pop	{r4, r5, r6, r7, r8, sl, lr}
    e398:	eaffffc2 	b	e2a8 <iicreg_init>
    e39c:	3c00d218 	.word	0x3c00d218
    e3a0:	21e10c00 	.word	0x21e10c00

0000e3a4 <spi_set_bytes>:
    e3a4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    e3a8:	e59f3020 	ldr	r3, [pc, #32]	; e3d0 <spi_set_bytes+0x2c>
    e3ac:	e1a01000 	mov	r1, r0
    e3b0:	e5830000 	str	r0, [r3]
    e3b4:	e24dd004 	sub	sp, sp, #4	; 0x4
    e3b8:	e59f0014 	ldr	r0, [pc, #20]	; e3d4 <spi_set_bytes+0x30>
    e3bc:	e59f3014 	ldr	r3, [pc, #20]	; e3d8 <spi_set_bytes+0x34>
    e3c0:	e1a0e00f 	mov	lr, pc
    e3c4:	e593f000 	ldr	pc, [r3]
    e3c8:	e28dd004 	add	sp, sp, #4	; 0x4
    e3cc:	e8bd8000 	pop	{pc}
    e3d0:	3c00d0ac 	.word	0x3c00d0ac
    e3d4:	00018830 	.word	0x00018830
    e3d8:	3c00d6cc 	.word	0x3c00d6cc

0000e3dc <spi_fifo_clear>:
    e3dc:	e59f2028 	ldr	r2, [pc, #40]	; e40c <spi_fifo_clear+0x30>
    e3e0:	e592300c 	ldr	r3, [r2, #12]
    e3e4:	e3130010 	tst	r3, #16	; 0x10
    e3e8:	1afffffc 	bne	e3e0 <spi_fifo_clear+0x4>
    e3ec:	ea000001 	b	e3f8 <spi_fifo_clear+0x1c>
    e3f0:	e5923008 	ldr	r3, [r2, #8]
    e3f4:	ea000000 	b	e3fc <spi_fifo_clear+0x20>
    e3f8:	e59f200c 	ldr	r2, [pc, #12]	; e40c <spi_fifo_clear+0x30>
    e3fc:	e592300c 	ldr	r3, [r2, #12]
    e400:	e3130004 	tst	r3, #4	; 0x4
    e404:	1afffff9 	bne	e3f0 <spi_fifo_clear+0x14>
    e408:	e12fff1e 	bx	lr
    e40c:	20500000 	.word	0x20500000

0000e410 <flash_vs_align>:
    e410:	e3a00c01 	mov	r0, #256	; 0x100
    e414:	e12fff1e 	bx	lr

0000e418 <spi_exchange>:
    e418:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    e41c:	e5905008 	ldr	r5, [r0, #8]
    e420:	e3510001 	cmp	r1, #1	; 0x1
    e424:	e24dd00c 	sub	sp, sp, #12	; 0xc
    e428:	e1a0a000 	mov	sl, r0
    e42c:	e5908004 	ldr	r8, [r0, #4]
    e430:	d1a04005 	movle	r4, r5
    e434:	d3a07000 	movle	r7, #0	; 0x0
    e438:	da000020 	ble	e4c0 <spi_exchange+0xa8>
    e43c:	e280a00c 	add	sl, r0, #12	; 0xc
    e440:	e1a01008 	mov	r1, r8
    e444:	e1a0000d 	mov	r0, sp
    e448:	e1a02005 	mov	r2, r5
    e44c:	ebffcc42 	bl	155c <memcpy>
    e450:	e99a0012 	ldmib	sl, {r1, r4}
    e454:	e0843005 	add	r3, r4, r5
    e458:	e3530008 	cmp	r3, #8	; 0x8
    e45c:	a3a03008 	movge	r3, #8	; 0x8
    e460:	e6ef6073 	uxtb	r6, r3
    e464:	e3510000 	cmp	r1, #0	; 0x0
    e468:	e1a0700d 	mov	r7, sp
    e46c:	059a8000 	ldreq	r8, [sl]
    e470:	00667084 	rsbeq	r7, r6, r4, lsl #1
    e474:	0a000008 	beq	e49c <spi_exchange+0x84>
    e478:	e0654006 	rsb	r4, r5, r6
    e47c:	e0870005 	add	r0, r7, r5
    e480:	e1a02004 	mov	r2, r4
    e484:	ebffcc34 	bl	155c <memcpy>
    e488:	e99a000c 	ldmib	sl, {r2, r3}
    e48c:	e0855003 	add	r5, r5, r3
    e490:	e0828004 	add	r8, r2, r4
    e494:	e3a07000 	mov	r7, #0	; 0x0
    e498:	e0664005 	rsb	r4, r6, r5
    e49c:	e3a02000 	mov	r2, #0	; 0x0
    e4a0:	e1a0000d 	mov	r0, sp
    e4a4:	e59f126c 	ldr	r1, [pc, #620]	; e718 <spi_exchange+0x300>
    e4a8:	ea000002 	b	e4b8 <spi_exchange+0xa0>
    e4ac:	e7d03002 	ldrb	r3, [r0, r2]
    e4b0:	e2822001 	add	r2, r2, #1	; 0x1
    e4b4:	e5813008 	str	r3, [r1, #8]
    e4b8:	e1520006 	cmp	r2, r6
    e4bc:	bafffffa 	blt	e4ac <spi_exchange+0x94>
    e4c0:	e59f6250 	ldr	r6, [pc, #592]	; e718 <spi_exchange+0x300>
    e4c4:	e3a0b005 	mov	fp, #5	; 0x5
    e4c8:	e5969018 	ldr	r9, [r6, #24]
    e4cc:	e3190004 	tst	r9, #4	; 0x4
    e4d0:	0a000030 	beq	e598 <spi_exchange+0x180>
    e4d4:	e3550003 	cmp	r5, #3	; 0x3
    e4d8:	da000005 	ble	e4f4 <spi_exchange+0xdc>
    e4dc:	e5963008 	ldr	r3, [r6, #8]
    e4e0:	e2455004 	sub	r5, r5, #4	; 0x4
    e4e4:	e5963008 	ldr	r3, [r6, #8]
    e4e8:	e5963008 	ldr	r3, [r6, #8]
    e4ec:	e5963008 	ldr	r3, [r6, #8]
    e4f0:	ea000028 	b	e598 <spi_exchange+0x180>
    e4f4:	e3550000 	cmp	r5, #0	; 0x0
    e4f8:	0a000003 	beq	e50c <spi_exchange+0xf4>
    e4fc:	e2555001 	subs	r5, r5, #1	; 0x1
    e500:	e5963008 	ldr	r3, [r6, #8]
    e504:	1afffffc 	bne	e4fc <spi_exchange+0xe4>
    e508:	ea000022 	b	e598 <spi_exchange+0x180>
    e50c:	e59a3000 	ldr	r3, [sl]
    e510:	e3530000 	cmp	r3, #0	; 0x0
    e514:	1a000004 	bne	e52c <spi_exchange+0x114>
    e518:	e59f01fc 	ldr	r0, [pc, #508]	; e71c <spi_exchange+0x304>
    e51c:	e59f31fc 	ldr	r3, [pc, #508]	; e720 <spi_exchange+0x308>
    e520:	e1a0e00f 	mov	lr, pc
    e524:	e593f000 	ldr	pc, [r3]
    e528:	ea00001a 	b	e598 <spi_exchange+0x180>
    e52c:	e3540003 	cmp	r4, #3	; 0x3
    e530:	da00000e 	ble	e570 <spi_exchange+0x158>
    e534:	e5962008 	ldr	r2, [r6, #8]
    e538:	e1a03008 	mov	r3, r8
    e53c:	e4c32001 	strb	r2, [r3], #1
    e540:	e5962008 	ldr	r2, [r6, #8]
    e544:	e2831001 	add	r1, r3, #1	; 0x1
    e548:	e5c82001 	strb	r2, [r8, #1]
    e54c:	e5962008 	ldr	r2, [r6, #8]
    e550:	e2544004 	subs	r4, r4, #4	; 0x4
    e554:	e5c32001 	strb	r2, [r3, #1]
    e558:	e5963008 	ldr	r3, [r6, #8]
    e55c:	e2812001 	add	r2, r1, #1	; 0x1
    e560:	e5c13001 	strb	r3, [r1, #1]
    e564:	0a00005e 	beq	e6e4 <spi_exchange+0x2cc>
    e568:	e2828001 	add	r8, r2, #1	; 0x1
    e56c:	ea000009 	b	e598 <spi_exchange+0x180>
    e570:	e3540000 	cmp	r4, #0	; 0x0
    e574:	0a00005a 	beq	e6e4 <spi_exchange+0x2cc>
    e578:	e59f2198 	ldr	r2, [pc, #408]	; e718 <spi_exchange+0x300>
    e57c:	e1a01005 	mov	r1, r5
    e580:	e5923008 	ldr	r3, [r2, #8]
    e584:	e7c83001 	strb	r3, [r8, r1]
    e588:	e2811001 	add	r1, r1, #1	; 0x1
    e58c:	e1510004 	cmp	r1, r4
    e590:	1afffffa 	bne	e580 <spi_exchange+0x168>
    e594:	ea000052 	b	e6e4 <spi_exchange+0x2cc>
    e598:	e3190008 	tst	r9, #8	; 0x8
    e59c:	0a00002a 	beq	e64c <spi_exchange+0x234>
    e5a0:	e59a3004 	ldr	r3, [sl, #4]
    e5a4:	e3530000 	cmp	r3, #0	; 0x0
    e5a8:	0a000019 	beq	e614 <spi_exchange+0x1fc>
    e5ac:	e3540003 	cmp	r4, #3	; 0x3
    e5b0:	da00000d 	ble	e5ec <spi_exchange+0x1d4>
    e5b4:	e1a03008 	mov	r3, r8
    e5b8:	e4d32001 	ldrb	r2, [r3], #1
    e5bc:	e2544004 	subs	r4, r4, #4	; 0x4
    e5c0:	e5862008 	str	r2, [r6, #8]
    e5c4:	e5d82001 	ldrb	r2, [r8, #1]
    e5c8:	e2831001 	add	r1, r3, #1	; 0x1
    e5cc:	e5862008 	str	r2, [r6, #8]
    e5d0:	e5d33001 	ldrb	r3, [r3, #1]
    e5d4:	e2818002 	add	r8, r1, #2	; 0x2
    e5d8:	e5863008 	str	r3, [r6, #8]
    e5dc:	e5d13001 	ldrb	r3, [r1, #1]
    e5e0:	e5863008 	str	r3, [r6, #8]
    e5e4:	1a000018 	bne	e64c <spi_exchange+0x234>
    e5e8:	ea00003d 	b	e6e4 <spi_exchange+0x2cc>
    e5ec:	e3540000 	cmp	r4, #0	; 0x0
    e5f0:	0a00003b 	beq	e6e4 <spi_exchange+0x2cc>
    e5f4:	e59f111c 	ldr	r1, [pc, #284]	; e718 <spi_exchange+0x300>
    e5f8:	e3a02000 	mov	r2, #0	; 0x0
    e5fc:	e7d83002 	ldrb	r3, [r8, r2]
    e600:	e2822001 	add	r2, r2, #1	; 0x1
    e604:	e1520004 	cmp	r2, r4
    e608:	e5813008 	str	r3, [r1, #8]
    e60c:	1afffffa 	bne	e5fc <spi_exchange+0x1e4>
    e610:	ea000033 	b	e6e4 <spi_exchange+0x2cc>
    e614:	e3570003 	cmp	r7, #3	; 0x3
    e618:	da000005 	ble	e634 <spi_exchange+0x21c>
    e61c:	e586b008 	str	fp, [r6, #8]
    e620:	e2477004 	sub	r7, r7, #4	; 0x4
    e624:	e586b008 	str	fp, [r6, #8]
    e628:	e586b008 	str	fp, [r6, #8]
    e62c:	e586b008 	str	fp, [r6, #8]
    e630:	ea000005 	b	e64c <spi_exchange+0x234>
    e634:	e3570000 	cmp	r7, #0	; 0x0
    e638:	0a000003 	beq	e64c <spi_exchange+0x234>
    e63c:	e3a03005 	mov	r3, #5	; 0x5
    e640:	e2577001 	subs	r7, r7, #1	; 0x1
    e644:	e5863008 	str	r3, [r6, #8]
    e648:	1afffffb 	bne	e63c <spi_exchange+0x224>
    e64c:	e3190002 	tst	r9, #2	; 0x2
    e650:	0affff9c 	beq	e4c8 <spi_exchange+0xb0>
    e654:	ea000009 	b	e680 <spi_exchange+0x268>
    e658:	e596300c 	ldr	r3, [r6, #12]
    e65c:	e2455001 	sub	r5, r5, #1	; 0x1
    e660:	e3130004 	tst	r3, #4	; 0x4
    e664:	15963008 	ldrne	r3, [r6, #8]
    e668:	1a000006 	bne	e688 <spi_exchange+0x270>
    e66c:	e59f00b0 	ldr	r0, [pc, #176]	; e724 <spi_exchange+0x30c>
    e670:	e1a01005 	mov	r1, r5
    e674:	e1a0e00f 	mov	lr, pc
    e678:	e597f000 	ldr	pc, [r7]
    e67c:	ea000001 	b	e688 <spi_exchange+0x270>
    e680:	e59f6090 	ldr	r6, [pc, #144]	; e718 <spi_exchange+0x300>
    e684:	e59f7094 	ldr	r7, [pc, #148]	; e720 <spi_exchange+0x308>
    e688:	e3550000 	cmp	r5, #0	; 0x0
    e68c:	1afffff1 	bne	e658 <spi_exchange+0x240>
    e690:	ea00000a 	b	e6c0 <spi_exchange+0x2a8>
    e694:	e595300c 	ldr	r3, [r5, #12]
    e698:	e2444001 	sub	r4, r4, #1	; 0x1
    e69c:	e3130004 	tst	r3, #4	; 0x4
    e6a0:	15953008 	ldrne	r3, [r5, #8]
    e6a4:	14c83001 	strbne	r3, [r8], #1
    e6a8:	1a000006 	bne	e6c8 <spi_exchange+0x2b0>
    e6ac:	e59f0074 	ldr	r0, [pc, #116]	; e728 <spi_exchange+0x310>
    e6b0:	e1a01004 	mov	r1, r4
    e6b4:	e1a0e00f 	mov	lr, pc
    e6b8:	e596f000 	ldr	pc, [r6]
    e6bc:	ea000001 	b	e6c8 <spi_exchange+0x2b0>
    e6c0:	e59f5050 	ldr	r5, [pc, #80]	; e718 <spi_exchange+0x300>
    e6c4:	e59f6054 	ldr	r6, [pc, #84]	; e720 <spi_exchange+0x308>
    e6c8:	e59a3000 	ldr	r3, [sl]
    e6cc:	e3530000 	cmp	r3, #0	; 0x0
    e6d0:	13540000 	cmpne	r4, #0	; 0x0
    e6d4:	1affffee 	bne	e694 <spi_exchange+0x27c>
    e6d8:	e59f3038 	ldr	r3, [pc, #56]	; e718 <spi_exchange+0x300>
    e6dc:	e3a02003 	mov	r2, #3	; 0x3
    e6e0:	e5832020 	str	r2, [r3, #32]
    e6e4:	e59f202c 	ldr	r2, [pc, #44]	; e718 <spi_exchange+0x300>
    e6e8:	e592300c 	ldr	r3, [r2, #12]
    e6ec:	e3130010 	tst	r3, #16	; 0x10
    e6f0:	1afffffc 	bne	e6e8 <spi_exchange+0x2d0>
    e6f4:	ea000001 	b	e700 <spi_exchange+0x2e8>
    e6f8:	e5923008 	ldr	r3, [r2, #8]
    e6fc:	ea000000 	b	e704 <spi_exchange+0x2ec>
    e700:	e59f2010 	ldr	r2, [pc, #16]	; e718 <spi_exchange+0x300>
    e704:	e592300c 	ldr	r3, [r2, #12]
    e708:	e3130004 	tst	r3, #4	; 0x4
    e70c:	1afffff9 	bne	e6f8 <spi_exchange+0x2e0>
    e710:	e28dd00c 	add	sp, sp, #12	; 0xc
    e714:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    e718:	20500000 	.word	0x20500000
    e71c:	00018847 	.word	0x00018847
    e720:	3c00d6cc 	.word	0x3c00d6cc
    e724:	0001885e 	.word	0x0001885e
    e728:	00018876 	.word	0x00018876

0000e72c <flash_vs_read>:
    e72c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    e730:	e24dd024 	sub	sp, sp, #36	; 0x24
    e734:	e28d4018 	add	r4, sp, #24	; 0x18
    e738:	e2841003 	add	r1, r4, #3	; 0x3
    e73c:	e59da040 	ldr	sl, [sp, #64]
    e740:	e58d1004 	str	r1, [sp, #4]
    e744:	e3a01005 	mov	r1, #5	; 0x5
    e748:	e3a0c000 	mov	ip, #0	; 0x0
    e74c:	e58d1008 	str	r1, [sp, #8]
    e750:	e2811006 	add	r1, r1, #6	; 0x6
    e754:	e58dc018 	str	ip, [sp, #24]
    e758:	e2845004 	add	r5, r4, #4	; 0x4
    e75c:	e58d000c 	str	r0, [sp, #12]
    e760:	e58dc010 	str	ip, [sp, #16]
    e764:	e5cd101b 	strb	r1, [sp, #27]
    e768:	e1a06002 	mov	r6, r2
    e76c:	e1a07003 	mov	r7, r3
    e770:	e58dc000 	str	ip, [sp]
    e774:	e1a0400a 	mov	r4, sl
    e778:	e1a0800d 	mov	r8, sp
    e77c:	ea00000c 	b	e7b4 <flash_vs_read+0x88>
    e780:	e3540a01 	cmp	r4, #4096	; 0x1000
    e784:	b1a03004 	movlt	r3, r4
    e788:	a3a03a01 	movge	r3, #4096	; 0x1000
    e78c:	e5852000 	str	r2, [r5]
    e790:	e58d3014 	str	r3, [sp, #20]
    e794:	ebffff1f 	bl	e418 <spi_exchange>
    e798:	e59d2014 	ldr	r2, [sp, #20]
    e79c:	e59d300c 	ldr	r3, [sp, #12]
    e7a0:	e0966002 	adds	r6, r6, r2
    e7a4:	e0833002 	add	r3, r3, r2
    e7a8:	e0a77fc2 	adc	r7, r7, r2, asr #31
    e7ac:	e58d300c 	str	r3, [sp, #12]
    e7b0:	e0624004 	rsb	r4, r2, r4
    e7b4:	e1a02c06 	lsl	r2, r6, #24
    e7b8:	e2061cff 	and	r1, r6, #65280	; 0xff00
    e7bc:	e1822c26 	orr	r2, r2, r6, lsr #24
    e7c0:	e1822401 	orr	r2, r2, r1, lsl #8
    e7c4:	e20638ff 	and	r3, r6, #16711680	; 0xff0000
    e7c8:	e1822423 	orr	r2, r2, r3, lsr #8
    e7cc:	e3540000 	cmp	r4, #0	; 0x0
    e7d0:	e1a02442 	asr	r2, r2, #8
    e7d4:	e1a0000d 	mov	r0, sp
    e7d8:	e3a01002 	mov	r1, #2	; 0x2
    e7dc:	1affffe7 	bne	e780 <flash_vs_read+0x54>
    e7e0:	e1a0000a 	mov	r0, sl
    e7e4:	e28dd024 	add	sp, sp, #36	; 0x24
    e7e8:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}

0000e7ec <flash_poll>:
    e7ec:	e92d4030 	push	{r4, r5, lr}
    e7f0:	e24dd024 	sub	sp, sp, #36	; 0x24
    e7f4:	e28dc018 	add	ip, sp, #24	; 0x18
    e7f8:	e28c3001 	add	r3, ip, #1	; 0x1
    e7fc:	e3a02000 	mov	r2, #0	; 0x0
    e800:	e3a01001 	mov	r1, #1	; 0x1
    e804:	e58d300c 	str	r3, [sp, #12]
    e808:	e3a03005 	mov	r3, #5	; 0x5
    e80c:	e58d2018 	str	r2, [sp, #24]
    e810:	e58d2010 	str	r2, [sp, #16]
    e814:	e58d1014 	str	r1, [sp, #20]
    e818:	e5cd3018 	strb	r3, [sp, #24]
    e81c:	e1a04000 	mov	r4, r0
    e820:	e58d201c 	str	r2, [sp, #28]
    e824:	e88d1004 	stm	sp, {r2, ip}
    e828:	e58d1008 	str	r1, [sp, #8]
    e82c:	e1a0500d 	mov	r5, sp
    e830:	e1a0000d 	mov	r0, sp
    e834:	e3a01002 	mov	r1, #2	; 0x2
    e838:	ebfffef6 	bl	e418 <spi_exchange>
    e83c:	e5dd3019 	ldrb	r3, [sp, #25]
    e840:	e3540000 	cmp	r4, #0	; 0x0
    e844:	11a030a3 	lsrne	r3, r3, #1
    e848:	12233001 	eorne	r3, r3, #1	; 0x1
    e84c:	e2033001 	and	r3, r3, #1	; 0x1
    e850:	e3530000 	cmp	r3, #0	; 0x0
    e854:	1afffff5 	bne	e830 <flash_poll+0x44>
    e858:	e28dd024 	add	sp, sp, #36	; 0x24
    e85c:	e8bd8030 	pop	{r4, r5, pc}

0000e860 <flash_wren>:
    e860:	e92d4010 	push	{r4, lr}
    e864:	e24dd020 	sub	sp, sp, #32	; 0x20
    e868:	e3a04001 	mov	r4, #1	; 0x1
    e86c:	e28dc018 	add	ip, sp, #24	; 0x18
    e870:	e08c3004 	add	r3, ip, r4
    e874:	e3a02000 	mov	r2, #0	; 0x0
    e878:	e58d3010 	str	r3, [sp, #16]
    e87c:	e3a03006 	mov	r3, #6	; 0x6
    e880:	e58d2018 	str	r2, [sp, #24]
    e884:	e1a0000d 	mov	r0, sp
    e888:	e5cd3018 	strb	r3, [sp, #24]
    e88c:	e1a01004 	mov	r1, r4
    e890:	e3a03004 	mov	r3, #4	; 0x4
    e894:	e58d200c 	str	r2, [sp, #12]
    e898:	e5cd3019 	strb	r3, [sp, #25]
    e89c:	e58d201c 	str	r2, [sp, #28]
    e8a0:	e88d1004 	stm	sp, {r2, ip}
    e8a4:	e58d4008 	str	r4, [sp, #8]
    e8a8:	e58d4014 	str	r4, [sp, #20]
    e8ac:	ebfffed9 	bl	e418 <spi_exchange>
    e8b0:	e1a00004 	mov	r0, r4
    e8b4:	ebffffcc 	bl	e7ec <flash_poll>
    e8b8:	e28dd020 	add	sp, sp, #32	; 0x20
    e8bc:	e8bd8010 	pop	{r4, pc}

0000e8c0 <flash_vs_erase>:
    e8c0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
    e8c4:	e24dd034 	sub	sp, sp, #52	; 0x34
    e8c8:	e28d4028 	add	r4, sp, #40	; 0x28
    e8cc:	e3a06001 	mov	r6, #1	; 0x1
    e8d0:	e59f109c 	ldr	r1, [pc, #156]	; e974 <flash_vs_erase+0xb4>
    e8d4:	e3a02008 	mov	r2, #8	; 0x8
    e8d8:	e1a00004 	mov	r0, r4
    e8dc:	ebffcb1e 	bl	155c <memcpy>
    e8e0:	e0843006 	add	r3, r4, r6
    e8e4:	e2842003 	add	r2, r4, #3	; 0x3
    e8e8:	e3a05000 	mov	r5, #0	; 0x0
    e8ec:	e28d7004 	add	r7, sp, #4	; 0x4
    e8f0:	e58d3008 	str	r3, [sp, #8]
    e8f4:	e3a03002 	mov	r3, #2	; 0x2
    e8f8:	e58d300c 	str	r3, [sp, #12]
    e8fc:	e58d2014 	str	r2, [sp, #20]
    e900:	e58d4020 	str	r4, [sp, #32]
    e904:	e58d5004 	str	r5, [sp, #4]
    e908:	e58d5010 	str	r5, [sp, #16]
    e90c:	e58d6018 	str	r6, [sp, #24]
    e910:	e58d501c 	str	r5, [sp, #28]
    e914:	e58d6024 	str	r6, [sp, #36]
    e918:	ebffffd0 	bl	e860 <flash_wren>
    e91c:	e1a01006 	mov	r1, r6
    e920:	e1a00007 	mov	r0, r7
    e924:	ebfffebb 	bl	e418 <spi_exchange>
    e928:	e1a00005 	mov	r0, r5
    e92c:	ebffffae 	bl	e7ec <flash_poll>
    e930:	e2870018 	add	r0, r7, #24	; 0x18
    e934:	e1a01006 	mov	r1, r6
    e938:	ebfffeb6 	bl	e418 <spi_exchange>
    e93c:	e1a01006 	mov	r1, r6
    e940:	e1a00007 	mov	r0, r7
    e944:	ebfffeb3 	bl	e418 <spi_exchange>
    e948:	e1a00005 	mov	r0, r5
    e94c:	ebffffa6 	bl	e7ec <flash_poll>
    e950:	ebffffc2 	bl	e860 <flash_wren>
    e954:	e1a01006 	mov	r1, r6
    e958:	e287000c 	add	r0, r7, #12	; 0xc
    e95c:	ebfffead 	bl	e418 <spi_exchange>
    e960:	e1a00005 	mov	r0, r5
    e964:	ebffffa0 	bl	e7ec <flash_poll>
    e968:	e1a00005 	mov	r0, r5
    e96c:	e28dd034 	add	sp, sp, #52	; 0x34
    e970:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
    e974:	00015f6e 	.word	0x00015f6e

0000e978 <flash_vs_write>:
    e978:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
    e97c:	e3a01004 	mov	r1, #4	; 0x4
    e980:	e24dd020 	sub	sp, sp, #32	; 0x20
    e984:	e3a0c000 	mov	ip, #0	; 0x0
    e988:	e59d9040 	ldr	r9, [sp, #64]
    e98c:	e58d1008 	str	r1, [sp, #8]
    e990:	e2811002 	add	r1, r1, #2	; 0x2
    e994:	e58dc018 	str	ip, [sp, #24]
    e998:	e28d701c 	add	r7, sp, #28	; 0x1c
    e99c:	e5cd1018 	strb	r1, [sp, #24]
    e9a0:	e3a01002 	mov	r1, #2	; 0x2
    e9a4:	e58dc00c 	str	ip, [sp, #12]
    e9a8:	e58d0010 	str	r0, [sp, #16]
    e9ac:	e5cd101b 	strb	r1, [sp, #27]
    e9b0:	e1a04002 	mov	r4, r2
    e9b4:	e1a05003 	mov	r5, r3
    e9b8:	e58dc000 	str	ip, [sp]
    e9bc:	e58d7004 	str	r7, [sp, #4]
    e9c0:	e1a06009 	mov	r6, r9
    e9c4:	e59fa080 	ldr	sl, [pc, #128]	; ea4c <flash_vs_write+0xd4>
    e9c8:	e1a0800d 	mov	r8, sp
    e9cc:	ea000011 	b	ea18 <flash_vs_write+0xa0>
    e9d0:	e59a3000 	ldr	r3, [sl]
    e9d4:	e5872000 	str	r2, [r7]
    e9d8:	e1560003 	cmp	r6, r3
    e9dc:	d58d6014 	strle	r6, [sp, #20]
    e9e0:	c58d3014 	strgt	r3, [sp, #20]
    e9e4:	ebffff9d 	bl	e860 <flash_wren>
    e9e8:	e1a0000d 	mov	r0, sp
    e9ec:	e3a01002 	mov	r1, #2	; 0x2
    e9f0:	ebfffe88 	bl	e418 <spi_exchange>
    e9f4:	e3a00000 	mov	r0, #0	; 0x0
    e9f8:	ebffff7b 	bl	e7ec <flash_poll>
    e9fc:	e59d2014 	ldr	r2, [sp, #20]
    ea00:	e59d3010 	ldr	r3, [sp, #16]
    ea04:	e0944002 	adds	r4, r4, r2
    ea08:	e0833002 	add	r3, r3, r2
    ea0c:	e0a55fc2 	adc	r5, r5, r2, asr #31
    ea10:	e58d3010 	str	r3, [sp, #16]
    ea14:	e0626006 	rsb	r6, r2, r6
    ea18:	e1a02c04 	lsl	r2, r4, #24
    ea1c:	e1822c24 	orr	r2, r2, r4, lsr #24
    ea20:	e2041cff 	and	r1, r4, #65280	; 0xff00
    ea24:	e1822401 	orr	r2, r2, r1, lsl #8
    ea28:	e20438ff 	and	r3, r4, #16711680	; 0xff0000
    ea2c:	e1822423 	orr	r2, r2, r3, lsr #8
    ea30:	e3c220ff 	bic	r2, r2, #255	; 0xff
    ea34:	e3560000 	cmp	r6, #0	; 0x0
    ea38:	e3822002 	orr	r2, r2, #2	; 0x2
    ea3c:	1affffe3 	bne	e9d0 <flash_vs_write+0x58>
    ea40:	e1a00009 	mov	r0, r9
    ea44:	e28dd020 	add	sp, sp, #32	; 0x20
    ea48:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
    ea4c:	3c00d0ac 	.word	0x3c00d0ac

0000ea50 <spi_init_pads>:
    ea50:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    ea54:	e3a0000b 	mov	r0, #11	; 0xb
    ea58:	e24dd004 	sub	sp, sp, #4	; 0x4
    ea5c:	ebffc783 	bl	870 <pads_init_module>
    ea60:	e3a00000 	mov	r0, #0	; 0x0
    ea64:	e28dd004 	add	sp, sp, #4	; 0x4
    ea68:	e8bd8000 	pop	{pc}

0000ea6c <spi_init_clk>:
    ea6c:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
    ea70:	e59f00e8 	ldr	r0, [pc, #232]	; eb60 <spi_init_clk+0xf4>
    ea74:	e24dd00c 	sub	sp, sp, #12	; 0xc
    ea78:	ebffc9e1 	bl	1204 <module_get_clock>
    ea7c:	e3a05002 	mov	r5, #2	; 0x2
    ea80:	e59fa0dc 	ldr	sl, [pc, #220]	; eb64 <spi_init_clk+0xf8>
    ea84:	e1a08000 	mov	r8, r0
    ea88:	ea00000c 	b	eac0 <spi_init_clk+0x54>
    ea8c:	e1a06004 	mov	r6, r4
    ea90:	e1a01007 	mov	r1, r7
    ea94:	e1a00008 	mov	r0, r8
    ea98:	eb0017bf 	bl	1499c <__aeabi_idiv>
    ea9c:	e150000a 	cmp	r0, sl
    eaa0:	e0877005 	add	r7, r7, r5
    eaa4:	e2864001 	add	r4, r6, #1	; 0x1
    eaa8:	da000007 	ble	eacc <spi_init_clk+0x60>
    eaac:	e3540c01 	cmp	r4, #256	; 0x100
    eab0:	1afffff5 	bne	ea8c <spi_init_clk+0x20>
    eab4:	e2855002 	add	r5, r5, #2	; 0x2
    eab8:	e3550c01 	cmp	r5, #256	; 0x100
    eabc:	0a000003 	beq	ead0 <spi_init_clk+0x64>
    eac0:	e1a07005 	mov	r7, r5
    eac4:	e3a06000 	mov	r6, #0	; 0x0
    eac8:	eafffff0 	b	ea90 <spi_init_clk+0x24>
    eacc:	e1a04006 	mov	r4, r6
    ead0:	e35400ff 	cmp	r4, #255	; 0xff
    ead4:	d35500fe 	cmple	r5, #254	; 0xfe
    ead8:	da000007 	ble	eafc <spi_init_clk+0x90>
    eadc:	e1a01005 	mov	r1, r5
    eae0:	e1a02004 	mov	r2, r4
    eae4:	e59f007c 	ldr	r0, [pc, #124]	; eb68 <spi_init_clk+0xfc>
    eae8:	e59f307c 	ldr	r3, [pc, #124]	; eb6c <spi_init_clk+0x100>
    eaec:	e1a0e00f 	mov	lr, pc
    eaf0:	e593f000 	ldr	pc, [r3]
    eaf4:	e3a05002 	mov	r5, #2	; 0x2
    eaf8:	e3a04027 	mov	r4, #39	; 0x27
    eafc:	e2841001 	add	r1, r4, #1	; 0x1
    eb00:	e0010195 	mul	r1, r5, r1
    eb04:	e1a00008 	mov	r0, r8
    eb08:	eb0017a3 	bl	1499c <__aeabi_idiv>
    eb0c:	e1a01008 	mov	r1, r8
    eb10:	e58d0000 	str	r0, [sp]
    eb14:	e1a02005 	mov	r2, r5
    eb18:	e1a03004 	mov	r3, r4
    eb1c:	e59f004c 	ldr	r0, [pc, #76]	; eb70 <spi_init_clk+0x104>
    eb20:	e59fc044 	ldr	ip, [pc, #68]	; eb6c <spi_init_clk+0x100>
    eb24:	e1a0e00f 	mov	lr, pc
    eb28:	e59cf000 	ldr	pc, [ip]
    eb2c:	e59f2040 	ldr	r2, [pc, #64]	; eb74 <spi_init_clk+0x108>
    eb30:	e3a03000 	mov	r3, #0	; 0x0
    eb34:	e1a01404 	lsl	r1, r4, #8
    eb38:	e5823004 	str	r3, [r2, #4]
    eb3c:	e38110c7 	orr	r1, r1, #199	; 0xc7
    eb40:	e5823014 	str	r3, [r2, #20]
    eb44:	e1a00003 	mov	r0, r3
    eb48:	e283300a 	add	r3, r3, #10	; 0xa
    eb4c:	e5825010 	str	r5, [r2, #16]
    eb50:	e5821000 	str	r1, [r2]
    eb54:	e5823004 	str	r3, [r2, #4]
    eb58:	e28dd00c 	add	sp, sp, #12	; 0xc
    eb5c:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
    eb60:	21e0a360 	.word	0x21e0a360
    eb64:	00551928 	.word	0x00551928
    eb68:	0001888e 	.word	0x0001888e
    eb6c:	3c00d6cc 	.word	0x3c00d6cc
    eb70:	000188bc 	.word	0x000188bc
    eb74:	20500000 	.word	0x20500000

0000eb78 <flash_vs_reset>:
    eb78:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    eb7c:	e59f0044 	ldr	r0, [pc, #68]	; ebc8 <flash_vs_reset+0x50>
    eb80:	e24dd004 	sub	sp, sp, #4	; 0x4
    eb84:	ebffc6fe 	bl	784 <module_enable>
    eb88:	ebffffb7 	bl	ea6c <spi_init_clk>
    eb8c:	e3a0000b 	mov	r0, #11	; 0xb
    eb90:	ebffc736 	bl	870 <pads_init_module>
    eb94:	e59f2030 	ldr	r2, [pc, #48]	; ebcc <flash_vs_reset+0x54>
    eb98:	e592300c 	ldr	r3, [r2, #12]
    eb9c:	e3130010 	tst	r3, #16	; 0x10
    eba0:	1afffffc 	bne	eb98 <flash_vs_reset+0x20>
    eba4:	ea000001 	b	ebb0 <flash_vs_reset+0x38>
    eba8:	e5923008 	ldr	r3, [r2, #8]
    ebac:	ea000000 	b	ebb4 <flash_vs_reset+0x3c>
    ebb0:	e59f2014 	ldr	r2, [pc, #20]	; ebcc <flash_vs_reset+0x54>
    ebb4:	e592300c 	ldr	r3, [r2, #12]
    ebb8:	e2130004 	ands	r0, r3, #4	; 0x4
    ebbc:	1afffff9 	bne	eba8 <flash_vs_reset+0x30>
    ebc0:	e28dd004 	add	sp, sp, #4	; 0x4
    ebc4:	e8bd8000 	pop	{pc}
    ebc8:	21e11400 	.word	0x21e11400
    ebcc:	20500000 	.word	0x20500000

0000ebd0 <ram_addr2offs>:
    ebd0:	e3500000 	cmp	r0, #0	; 0x0
    ebd4:	e3a01000 	mov	r1, #0	; 0x0
    ebd8:	a3a02102 	movge	r2, #-2147483648	; 0x80000000
    ebdc:	b3a02102 	movlt	r2, #-2147483648	; 0x80000000
    ebe0:	a3a03000 	movge	r3, #0	; 0x0
    ebe4:	b3e03000 	mvnlt	r3, #0	; 0x0
    ebe8:	e0922000 	adds	r2, r2, r0
    ebec:	e0a33001 	adc	r3, r3, r1
    ebf0:	e1a01003 	mov	r1, r3
    ebf4:	e1a00002 	mov	r0, r2
    ebf8:	e12fff1e 	bx	lr

0000ebfc <ram_offs2addr>:
    ebfc:	e2800102 	add	r0, r0, #-2147483648	; 0x80000000
    ec00:	e12fff1e 	bx	lr

0000ec04 <ram_vs_align>:
    ec04:	e3a00c02 	mov	r0, #512	; 0x200
    ec08:	e12fff1e 	bx	lr

0000ec0c <ram_vs_reset>:
    ec0c:	e3a00000 	mov	r0, #0	; 0x0
    ec10:	e12fff1e 	bx	lr

0000ec14 <ram_vs_write>:
    ec14:	e92d4010 	push	{r4, lr}
    ec18:	e59d4008 	ldr	r4, [sp, #8]
    ec1c:	e1a01000 	mov	r1, r0
    ec20:	e2820102 	add	r0, r2, #-2147483648	; 0x80000000
    ec24:	e1a02004 	mov	r2, r4
    ec28:	ebffca4b 	bl	155c <memcpy>
    ec2c:	e1a00004 	mov	r0, r4
    ec30:	e8bd8010 	pop	{r4, pc}

0000ec34 <ram_vs_read>:
    ec34:	e92d4010 	push	{r4, lr}
    ec38:	e59d4008 	ldr	r4, [sp, #8]
    ec3c:	e2821102 	add	r1, r2, #-2147483648	; 0x80000000
    ec40:	e1a02004 	mov	r2, r4
    ec44:	ebffca44 	bl	155c <memcpy>
    ec48:	e1a00004 	mov	r0, r4
    ec4c:	e8bd8010 	pop	{r4, pc}

0000ec50 <CreateAtaCFIS>:
    ec50:	e3a0c000 	mov	ip, #0	; 0x0
    ec54:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    ec58:	e1a05003 	mov	r5, r3
    ec5c:	e1a04000 	mov	r4, r0
    ec60:	e1a06001 	mov	r6, r1
    ec64:	e1a08002 	mov	r8, r2
    ec68:	e1a0300c 	mov	r3, ip
    ec6c:	e7c4300c 	strb	r3, [r4, ip]
    ec70:	e28cc001 	add	ip, ip, #1	; 0x1
    ec74:	e35c0014 	cmp	ip, #20	; 0x14
    ec78:	e3a07000 	mov	r7, #0	; 0x0
    ec7c:	1afffffa 	bne	ec6c <CreateAtaCFIS+0x1c>
    ec80:	e5d43001 	ldrb	r3, [r4, #1]
    ec84:	e59d2018 	ldr	r2, [sp, #24]
    ec88:	e3c3307f 	bic	r3, r3, #127	; 0x7f
    ec8c:	e1520007 	cmp	r2, r7
    ec90:	e6ef3073 	uxtb	r3, r3
    ec94:	03a02080 	moveq	r2, #128	; 0x80
    ec98:	11a02007 	movne	r2, r7
    ec9c:	e3c33080 	bic	r3, r3, #128	; 0x80
    eca0:	e35600ca 	cmp	r6, #202	; 0xca
    eca4:	135600c8 	cmpne	r6, #200	; 0xc8
    eca8:	e1822003 	orr	r2, r2, r3
    ecac:	e3a03027 	mov	r3, #39	; 0x27
    ecb0:	e5c43000 	strb	r3, [r4]
    ecb4:	01a03c25 	lsreq	r3, r5, #24
    ecb8:	e1a0c428 	lsr	ip, r8, #8
    ecbc:	e1a01425 	lsr	r1, r5, #8
    ecc0:	e1a00825 	lsr	r0, r5, #16
    ecc4:	0203300f 	andeq	r3, r3, #15	; 0xf
    ecc8:	e5c47007 	strb	r7, [r4, #7]
    eccc:	e5c42001 	strb	r2, [r4, #1]
    ecd0:	e5c4700b 	strb	r7, [r4, #11]
    ecd4:	e5c4c00d 	strb	ip, [r4, #13]
    ecd8:	e5c41005 	strb	r1, [r4, #5]
    ecdc:	e5c40006 	strb	r0, [r4, #6]
    ece0:	e5c46002 	strb	r6, [r4, #2]
    ece4:	e5c47003 	strb	r7, [r4, #3]
    ece8:	e5c4800c 	strb	r8, [r4, #12]
    ecec:	e5c45004 	strb	r5, [r4, #4]
    ecf0:	05c43007 	strbeq	r3, [r4, #7]
    ecf4:	0a000003 	beq	ed08 <CreateAtaCFIS+0xb8>
    ecf8:	e3560035 	cmp	r6, #53	; 0x35
    ecfc:	13560025 	cmpne	r6, #37	; 0x25
    ed00:	01a03c25 	lsreq	r3, r5, #24
    ed04:	05c43008 	strbeq	r3, [r4, #8]
    ed08:	e59d201c 	ldr	r2, [sp, #28]
    ed0c:	e5d43007 	ldrb	r3, [r4, #7]
    ed10:	e3520000 	cmp	r2, #0	; 0x0
    ed14:	e3a01000 	mov	r1, #0	; 0x0
    ed18:	13a02004 	movne	r2, #4	; 0x4
    ed1c:	03a02000 	moveq	r2, #0	; 0x0
    ed20:	e3833040 	orr	r3, r3, #64	; 0x40
    ed24:	e35600ec 	cmp	r6, #236	; 0xec
    ed28:	e5c43007 	strb	r3, [r4, #7]
    ed2c:	e5c4200f 	strb	r2, [r4, #15]
    ed30:	e5c41009 	strb	r1, [r4, #9]
    ed34:	e5c4100a 	strb	r1, [r4, #10]
    ed38:	e5c4100e 	strb	r1, [r4, #14]
    ed3c:	e5841010 	str	r1, [r4, #16]
    ed40:	05c41007 	strbeq	r1, [r4, #7]
    ed44:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

0000ed48 <CreateCommandHeader>:
    ed48:	e92d4070 	push	{r4, r5, r6, lr}
    ed4c:	e59d6018 	ldr	r6, [sp, #24]
    ed50:	e1a04000 	mov	r4, r0
    ed54:	e1a00001 	mov	r0, r1
    ed58:	e3a01000 	mov	r1, #0	; 0x0
    ed5c:	e1a05003 	mov	r5, r3
    ed60:	e084c280 	add	ip, r4, r0, lsl #5
    ed64:	e1a03001 	mov	r3, r1
    ed68:	e7c1300c 	strb	r3, [r1, ip]
    ed6c:	e2811001 	add	r1, r1, #1	; 0x1
    ed70:	e3510020 	cmp	r1, #32	; 0x20
    ed74:	1afffffb 	bne	ed68 <CreateCommandHeader+0x20>
    ed78:	e3a03000 	mov	r3, #0	; 0x0
    ed7c:	e58c2008 	str	r2, [ip, #8]
    ed80:	e58c300c 	str	r3, [ip, #12]
    ed84:	e1cc50b2 	strh	r5, [ip, #2]
    ed88:	e1560003 	cmp	r6, r3
    ed8c:	e7d43280 	ldrb	r3, [r4, r0, lsl #5]
    ed90:	13c3301c 	bicne	r3, r3, #28	; 0x1c
    ed94:	03c3301a 	biceq	r3, r3, #26	; 0x1a
    ed98:	13833003 	orrne	r3, r3, #3	; 0x3
    ed9c:	03833005 	orreq	r3, r3, #5	; 0x5
    eda0:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    eda4:	e59d3010 	ldr	r3, [sp, #16]
    eda8:	e7d42280 	ldrb	r2, [r4, r0, lsl #5]
    edac:	e3530000 	cmp	r3, #0	; 0x0
    edb0:	13a03020 	movne	r3, #32	; 0x20
    edb4:	03a03000 	moveq	r3, #0	; 0x0
    edb8:	e3c22020 	bic	r2, r2, #32	; 0x20
    edbc:	e1833002 	orr	r3, r3, r2
    edc0:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    edc4:	e6ef2073 	uxtb	r2, r3
    edc8:	e59d3014 	ldr	r3, [sp, #20]
    edcc:	e3c22040 	bic	r2, r2, #64	; 0x40
    edd0:	e3530000 	cmp	r3, #0	; 0x0
    edd4:	13a03040 	movne	r3, #64	; 0x40
    edd8:	03a03000 	moveq	r3, #0	; 0x0
    eddc:	e1833002 	orr	r3, r3, r2
    ede0:	e6ef3073 	uxtb	r3, r3
    ede4:	e3c33080 	bic	r3, r3, #128	; 0x80
    ede8:	e7c43280 	strb	r3, [r4, r0, lsl #5]
    edec:	e5dc3001 	ldrb	r3, [ip, #1]
    edf0:	e3560000 	cmp	r6, #0	; 0x0
    edf4:	e3c330f0 	bic	r3, r3, #240	; 0xf0
    edf8:	e6ef3073 	uxtb	r3, r3
    edfc:	13a01002 	movne	r1, #2	; 0x2
    ee00:	03a01000 	moveq	r1, #0	; 0x0
    ee04:	e3c33002 	bic	r3, r3, #2	; 0x2
    ee08:	e59d001c 	ldr	r0, [sp, #28]
    ee0c:	e1811003 	orr	r1, r1, r3
    ee10:	e6ef3071 	uxtb	r3, r1
    ee14:	e2500000 	subs	r0, r0, #0	; 0x0
    ee18:	13a00001 	movne	r0, #1	; 0x1
    ee1c:	e3c33001 	bic	r3, r3, #1	; 0x1
    ee20:	e1803003 	orr	r3, r0, r3
    ee24:	e6ef3073 	uxtb	r3, r3
    ee28:	e3c33004 	bic	r3, r3, #4	; 0x4
    ee2c:	e3a02000 	mov	r2, #0	; 0x0
    ee30:	e1833100 	orr	r3, r3, r0, lsl #2
    ee34:	e58c201c 	str	r2, [ip, #28]
    ee38:	e58c2004 	str	r2, [ip, #4]
    ee3c:	e5cc3001 	strb	r3, [ip, #1]
    ee40:	e58c2010 	str	r2, [ip, #16]
    ee44:	e58c2014 	str	r2, [ip, #20]
    ee48:	e58c2018 	str	r2, [ip, #24]
    ee4c:	e8bd8070 	pop	{r4, r5, r6, pc}

0000ee50 <get_max_lba>:
    ee50:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
    ee54:	e3a04000 	mov	r4, #0	; 0x0
    ee58:	e2807c05 	add	r7, r0, #1280	; 0x500
    ee5c:	e1a06000 	mov	r6, r0
    ee60:	e24dd010 	sub	sp, sp, #16	; 0x10
    ee64:	e3a010ec 	mov	r1, #236	; 0xec
    ee68:	e3a02001 	mov	r2, #1	; 0x1
    ee6c:	e1a03004 	mov	r3, r4
    ee70:	e1a00007 	mov	r0, r7
    ee74:	e58d4000 	str	r4, [sp]
    ee78:	e58d4004 	str	r4, [sp, #4]
    ee7c:	e2868b01 	add	r8, r6, #1024	; 0x400
    ee80:	ebffff72 	bl	ec50 <CreateAtaCFIS>
    ee84:	e2862b02 	add	r2, r6, #2048	; 0x800
    ee88:	e2863d16 	add	r3, r6, #1408	; 0x580
    ee8c:	e1a01004 	mov	r1, r4
    ee90:	e7c31004 	strb	r1, [r3, r4]
    ee94:	e2844001 	add	r4, r4, #1	; 0x1
    ee98:	e3540010 	cmp	r4, #16	; 0x10
    ee9c:	e3a05000 	mov	r5, #0	; 0x0
    eea0:	1afffffa 	bne	ee90 <get_max_lba+0x40>
    eea4:	e5872080 	str	r2, [r7, #128]
    eea8:	e596258c 	ldr	r2, [r6, #1420]
    eeac:	e5865584 	str	r5, [r6, #1412]
    eeb0:	e1a03b22 	lsr	r3, r2, #22
    eeb4:	e1a03b03 	lsl	r3, r3, #22
    eeb8:	e1e024a3 	mvn	r2, r3, lsr #9
    eebc:	e1e02482 	mvn	r2, r2, lsl #9
    eec0:	e1a03c22 	lsr	r3, r2, #24
    eec4:	e3c33080 	bic	r3, r3, #128	; 0x80
    eec8:	e586258c 	str	r2, [r6, #1420]
    eecc:	e1a01005 	mov	r1, r5
    eed0:	e5c6358f 	strb	r3, [r6, #1423]
    eed4:	e1a02007 	mov	r2, r7
    eed8:	e3a03001 	mov	r3, #1	; 0x1
    eedc:	e1a00006 	mov	r0, r6
    eee0:	e58d5000 	str	r5, [sp]
    eee4:	e58d5004 	str	r5, [sp, #4]
    eee8:	e58d5008 	str	r5, [sp, #8]
    eeec:	e58d500c 	str	r5, [sp, #12]
    eef0:	ebffff94 	bl	ed48 <CreateCommandHeader>
    eef4:	e1a01005 	mov	r1, r5
    eef8:	e1a03005 	mov	r3, r5
    eefc:	e7c83001 	strb	r3, [r8, r1]
    ef00:	e2811001 	add	r1, r1, #1	; 0x1
    ef04:	e3510c01 	cmp	r1, #256	; 0x100
    ef08:	1afffffb 	bne	eefc <get_max_lba+0xac>
    ef0c:	e59f20c8 	ldr	r2, [pc, #200]	; efdc <get_max_lba+0x18c>
    ef10:	e3e03000 	mvn	r3, #0	; 0x0
    ef14:	e5826100 	str	r6, [r2, #256]
    ef18:	e5828108 	str	r8, [r2, #264]
    ef1c:	e5823110 	str	r3, [r2, #272]
    ef20:	e5823130 	str	r3, [r2, #304]
    ef24:	e5923118 	ldr	r3, [r2, #280]
    ef28:	e1a00002 	mov	r0, r2
    ef2c:	e3833010 	orr	r3, r3, #16	; 0x10
    ef30:	e5823118 	str	r3, [r2, #280]
    ef34:	e5923118 	ldr	r3, [r2, #280]
    ef38:	e3833001 	orr	r3, r3, #1	; 0x1
    ef3c:	e5823118 	str	r3, [r2, #280]
    ef40:	e3a03001 	mov	r3, #1	; 0x1
    ef44:	e5823138 	str	r3, [r2, #312]
    ef48:	e5903110 	ldr	r3, [r0, #272]
    ef4c:	e59f1088 	ldr	r1, [pc, #136]	; efdc <get_max_lba+0x18c>
    ef50:	e3130002 	tst	r3, #2	; 0x2
    ef54:	0afffffb 	beq	ef48 <get_max_lba+0xf8>
    ef58:	e3e03000 	mvn	r3, #0	; 0x0
    ef5c:	e5912008 	ldr	r2, [r1, #8]
    ef60:	e5813110 	str	r3, [r1, #272]
    ef64:	e5813008 	str	r3, [r1, #8]
    ef68:	e5912118 	ldr	r2, [r1, #280]
    ef6c:	e30c3011 	movw	r3, #49169	; 0xc011
    ef70:	e0023003 	and	r3, r2, r3
    ef74:	e3530000 	cmp	r3, #0	; 0x0
    ef78:	e596e8c8 	ldr	lr, [r6, #2248]
    ef7c:	e59648cc 	ldr	r4, [r6, #2252]
    ef80:	0a00000e 	beq	efc0 <get_max_lba+0x170>
    ef84:	e5913118 	ldr	r3, [r1, #280]
    ef88:	e1a0c001 	mov	ip, r1
    ef8c:	e3c33001 	bic	r3, r3, #1	; 0x1
    ef90:	e5813118 	str	r3, [r1, #280]
    ef94:	e59c3118 	ldr	r3, [ip, #280]
    ef98:	e59f203c 	ldr	r2, [pc, #60]	; efdc <get_max_lba+0x18c>
    ef9c:	e3130902 	tst	r3, #32768	; 0x8000
    efa0:	1afffffb 	bne	ef94 <get_max_lba+0x144>
    efa4:	e5923118 	ldr	r3, [r2, #280]
    efa8:	e1a01002 	mov	r1, r2
    efac:	e3c33010 	bic	r3, r3, #16	; 0x10
    efb0:	e5823118 	str	r3, [r2, #280]
    efb4:	e5913118 	ldr	r3, [r1, #280]
    efb8:	e3130901 	tst	r3, #16384	; 0x4000
    efbc:	1afffffc 	bne	efb4 <get_max_lba+0x164>
    efc0:	e59f2014 	ldr	r2, [pc, #20]	; efdc <get_max_lba+0x18c>
    efc4:	e5923120 	ldr	r3, [r2, #288]
    efc8:	e2130088 	ands	r0, r3, #136	; 0x88
    efcc:	1afffffc 	bne	efc4 <get_max_lba+0x174>
    efd0:	e08e0000 	add	r0, lr, r0
    efd4:	e28dd010 	add	sp, sp, #16	; 0x10
    efd8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
    efdc:	28200000 	.word	0x28200000

0000efe0 <sata_vs_align>:
    efe0:	e3a00c02 	mov	r0, #512	; 0x200
    efe4:	e12fff1e 	bx	lr

0000efe8 <init_sata>:
    efe8:	e12fff1e 	bx	lr

0000efec <sata_vs_write>:
    efec:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    eff0:	e1a08000 	mov	r8, r0
    eff4:	e24dd034 	sub	sp, sp, #52	; 0x34
    eff8:	e3a00c0e 	mov	r0, #3584	; 0xe00
    effc:	e1a06002 	mov	r6, r2
    f000:	e1a07003 	mov	r7, r3
    f004:	ebffc9b9 	bl	16f0 <malloc>
    f008:	e3500000 	cmp	r0, #0	; 0x0
    f00c:	e58d0014 	str	r0, [sp, #20]
    f010:	1a000004 	bne	f028 <sata_vs_write+0x3c>
    f014:	e59f0680 	ldr	r0, [pc, #1664]	; f69c <sata_vs_write+0x6b0>
    f018:	e59f3680 	ldr	r3, [pc, #1664]	; f6a0 <sata_vs_write+0x6b4>
    f01c:	e1a0e00f 	mov	lr, pc
    f020:	e593f000 	ldr	pc, [r3]
    f024:	ea0000ea 	b	f3d4 <sata_vs_write+0x3e8>
    f028:	e59d0014 	ldr	r0, [sp, #20]
    f02c:	e2803fff 	add	r3, r0, #1020	; 0x3fc
    f030:	e2833003 	add	r3, r3, #3	; 0x3
    f034:	e3c33fff 	bic	r3, r3, #1020	; 0x3fc
    f038:	e3c33003 	bic	r3, r3, #3	; 0x3
    f03c:	e1530000 	cmp	r3, r0
    f040:	e58d3018 	str	r3, [sp, #24]
    f044:	02833a01 	addeq	r3, r3, #4096	; 0x1000
    f048:	058d3018 	streq	r3, [sp, #24]
    f04c:	e59d0018 	ldr	r0, [sp, #24]
    f050:	ebffff7e 	bl	ee50 <get_max_lba>
    f054:	e59d1058 	ldr	r1, [sp, #88]
    f058:	e1a0c000 	mov	ip, r0
    f05c:	e1a03b81 	lsl	r3, r1, #23
    f060:	e1a03ba3 	lsr	r3, r3, #23
    f064:	e3530000 	cmp	r3, #0	; 0x0
    f068:	1a0000d3 	bne	f3bc <sata_vs_write+0x3d0>
    f06c:	e30041ff 	movw	r4, #511	; 0x1ff
    f070:	e3a05000 	mov	r5, #0	; 0x0
    f074:	e0062004 	and	r2, r6, r4
    f078:	e0073005 	and	r3, r7, r5
    f07c:	e1920003 	orrs	r0, r2, r3
    f080:	1a0000cd 	bne	f3bc <sata_vs_write+0x3d0>
    f084:	e3180001 	tst	r8, #1	; 0x1
    f088:	1a0000cb 	bne	f3bc <sata_vs_write+0x3d0>
    f08c:	e1a00007 	mov	r0, r7
    f090:	e1a01fc0 	asr	r1, r0, #31
    f094:	e1a02fc1 	asr	r2, r1, #31
    f098:	e1a03002 	mov	r3, r2
    f09c:	e0022004 	and	r2, r2, r4
    f0a0:	e0922006 	adds	r2, r2, r6
    f0a4:	e0033005 	and	r3, r3, r5
    f0a8:	e0a33007 	adc	r3, r3, r7
    f0ac:	e1a014a2 	lsr	r1, r2, #9
    f0b0:	e1a0600c 	mov	r6, ip
    f0b4:	e1a07fc6 	asr	r7, r6, #31
    f0b8:	e1811b83 	orr	r1, r1, r3, lsl #23
    f0bc:	e3570000 	cmp	r7, #0	; 0x0
    f0c0:	e58d1028 	str	r1, [sp, #40]
    f0c4:	e1a00001 	mov	r0, r1
    f0c8:	e3a01000 	mov	r1, #0	; 0x0
    f0cc:	8a000002 	bhi	f0dc <sata_vs_write+0xf0>
    f0d0:	1a0000b9 	bne	f3bc <sata_vs_write+0x3d0>
    f0d4:	e1560000 	cmp	r6, r0
    f0d8:	9a0000b7 	bls	f3bc <sata_vs_write+0x3d0>
    f0dc:	e59d2058 	ldr	r2, [sp, #88]
    f0e0:	e3520402 	cmp	r2, #33554432	; 0x2000000
    f0e4:	ba000007 	blt	f108 <sata_vs_write+0x11c>
    f0e8:	e1a02006 	mov	r2, r6
    f0ec:	e1a03007 	mov	r3, r7
    f0f0:	e0522000 	subs	r2, r2, r0
    f0f4:	e0c33001 	sbc	r3, r3, r1
    f0f8:	e3530000 	cmp	r3, #0	; 0x0
    f0fc:	1a000001 	bne	f108 <sata_vs_write+0x11c>
    f100:	e3520801 	cmp	r2, #65536	; 0x10000
    f104:	3a0000ac 	bcc	f3bc <sata_vs_write+0x3d0>
    f108:	e59d3058 	ldr	r3, [sp, #88]
    f10c:	e3530401 	cmp	r3, #16777216	; 0x1000000
    f110:	ba000007 	blt	f134 <sata_vs_write+0x148>
    f114:	e1a02006 	mov	r2, r6
    f118:	e1a03007 	mov	r3, r7
    f11c:	e0522000 	subs	r2, r2, r0
    f120:	e0c33001 	sbc	r3, r3, r1
    f124:	e3530000 	cmp	r3, #0	; 0x0
    f128:	1a000001 	bne	f134 <sata_vs_write+0x148>
    f12c:	e3520902 	cmp	r2, #32768	; 0x8000
    f130:	3a0000a1 	bcc	f3bc <sata_vs_write+0x3d0>
    f134:	e59dc058 	ldr	ip, [sp, #88]
    f138:	e35c0502 	cmp	ip, #8388608	; 0x800000
    f13c:	ba000007 	blt	f160 <sata_vs_write+0x174>
    f140:	e1a02006 	mov	r2, r6
    f144:	e1a03007 	mov	r3, r7
    f148:	e0522000 	subs	r2, r2, r0
    f14c:	e0c33001 	sbc	r3, r3, r1
    f150:	e3530000 	cmp	r3, #0	; 0x0
    f154:	1a000001 	bne	f160 <sata_vs_write+0x174>
    f158:	e3520901 	cmp	r2, #16384	; 0x4000
    f15c:	3a000096 	bcc	f3bc <sata_vs_write+0x3d0>
    f160:	e59d2058 	ldr	r2, [sp, #88]
    f164:	e3520501 	cmp	r2, #4194304	; 0x400000
    f168:	ba000007 	blt	f18c <sata_vs_write+0x1a0>
    f16c:	e1a02006 	mov	r2, r6
    f170:	e1a03007 	mov	r3, r7
    f174:	e0522000 	subs	r2, r2, r0
    f178:	e0c33001 	sbc	r3, r3, r1
    f17c:	e3530000 	cmp	r3, #0	; 0x0
    f180:	1a000001 	bne	f18c <sata_vs_write+0x1a0>
    f184:	e3520a02 	cmp	r2, #8192	; 0x2000
    f188:	3a00008b 	bcc	f3bc <sata_vs_write+0x3d0>
    f18c:	e59d3058 	ldr	r3, [sp, #88]
    f190:	e3530602 	cmp	r3, #2097152	; 0x200000
    f194:	ba000007 	blt	f1b8 <sata_vs_write+0x1cc>
    f198:	e1a02006 	mov	r2, r6
    f19c:	e1a03007 	mov	r3, r7
    f1a0:	e0522000 	subs	r2, r2, r0
    f1a4:	e0c33001 	sbc	r3, r3, r1
    f1a8:	e3530000 	cmp	r3, #0	; 0x0
    f1ac:	1a000001 	bne	f1b8 <sata_vs_write+0x1cc>
    f1b0:	e3520a01 	cmp	r2, #4096	; 0x1000
    f1b4:	3a000080 	bcc	f3bc <sata_vs_write+0x3d0>
    f1b8:	e59dc058 	ldr	ip, [sp, #88]
    f1bc:	e35c0601 	cmp	ip, #1048576	; 0x100000
    f1c0:	ba000007 	blt	f1e4 <sata_vs_write+0x1f8>
    f1c4:	e1a02006 	mov	r2, r6
    f1c8:	e1a03007 	mov	r3, r7
    f1cc:	e0522000 	subs	r2, r2, r0
    f1d0:	e0c33001 	sbc	r3, r3, r1
    f1d4:	e3530000 	cmp	r3, #0	; 0x0
    f1d8:	1a000001 	bne	f1e4 <sata_vs_write+0x1f8>
    f1dc:	e3520b02 	cmp	r2, #2048	; 0x800
    f1e0:	3a000075 	bcc	f3bc <sata_vs_write+0x3d0>
    f1e4:	e59d2058 	ldr	r2, [sp, #88]
    f1e8:	e3520702 	cmp	r2, #524288	; 0x80000
    f1ec:	ba000007 	blt	f210 <sata_vs_write+0x224>
    f1f0:	e1a02006 	mov	r2, r6
    f1f4:	e1a03007 	mov	r3, r7
    f1f8:	e0522000 	subs	r2, r2, r0
    f1fc:	e0c33001 	sbc	r3, r3, r1
    f200:	e3530000 	cmp	r3, #0	; 0x0
    f204:	1a000001 	bne	f210 <sata_vs_write+0x224>
    f208:	e3520b01 	cmp	r2, #1024	; 0x400
    f20c:	3a00006a 	bcc	f3bc <sata_vs_write+0x3d0>
    f210:	e59d3058 	ldr	r3, [sp, #88]
    f214:	e3530701 	cmp	r3, #262144	; 0x40000
    f218:	ba000007 	blt	f23c <sata_vs_write+0x250>
    f21c:	e1a02006 	mov	r2, r6
    f220:	e1a03007 	mov	r3, r7
    f224:	e0522000 	subs	r2, r2, r0
    f228:	e0c33001 	sbc	r3, r3, r1
    f22c:	e3530000 	cmp	r3, #0	; 0x0
    f230:	1a000001 	bne	f23c <sata_vs_write+0x250>
    f234:	e3520c02 	cmp	r2, #512	; 0x200
    f238:	3a00005f 	bcc	f3bc <sata_vs_write+0x3d0>
    f23c:	e59dc058 	ldr	ip, [sp, #88]
    f240:	e35c0802 	cmp	ip, #131072	; 0x20000
    f244:	ba000008 	blt	f26c <sata_vs_write+0x280>
    f248:	e3e020ff 	mvn	r2, #255	; 0xff
    f24c:	e0922006 	adds	r2, r2, r6
    f250:	e3e03000 	mvn	r3, #0	; 0x0
    f254:	e0a33007 	adc	r3, r3, r7
    f258:	e1510003 	cmp	r1, r3
    f25c:	8a000056 	bhi	f3bc <sata_vs_write+0x3d0>
    f260:	1a000001 	bne	f26c <sata_vs_write+0x280>
    f264:	e1500002 	cmp	r0, r2
    f268:	8a000053 	bhi	f3bc <sata_vs_write+0x3d0>
    f26c:	e59d2058 	ldr	r2, [sp, #88]
    f270:	e3520801 	cmp	r2, #65536	; 0x10000
    f274:	ba000007 	blt	f298 <sata_vs_write+0x2ac>
    f278:	e1a02006 	mov	r2, r6
    f27c:	e1a03007 	mov	r3, r7
    f280:	e0522000 	subs	r2, r2, r0
    f284:	e0c33001 	sbc	r3, r3, r1
    f288:	e3530000 	cmp	r3, #0	; 0x0
    f28c:	1a000001 	bne	f298 <sata_vs_write+0x2ac>
    f290:	e352007f 	cmp	r2, #127	; 0x7f
    f294:	9a000048 	bls	f3bc <sata_vs_write+0x3d0>
    f298:	e59d3058 	ldr	r3, [sp, #88]
    f29c:	e3530902 	cmp	r3, #32768	; 0x8000
    f2a0:	ba000007 	blt	f2c4 <sata_vs_write+0x2d8>
    f2a4:	e1a02006 	mov	r2, r6
    f2a8:	e1a03007 	mov	r3, r7
    f2ac:	e0522000 	subs	r2, r2, r0
    f2b0:	e0c33001 	sbc	r3, r3, r1
    f2b4:	e3530000 	cmp	r3, #0	; 0x0
    f2b8:	1a000001 	bne	f2c4 <sata_vs_write+0x2d8>
    f2bc:	e352003f 	cmp	r2, #63	; 0x3f
    f2c0:	9a00003d 	bls	f3bc <sata_vs_write+0x3d0>
    f2c4:	e59dc058 	ldr	ip, [sp, #88]
    f2c8:	e35c0901 	cmp	ip, #16384	; 0x4000
    f2cc:	ba000007 	blt	f2f0 <sata_vs_write+0x304>
    f2d0:	e1a02006 	mov	r2, r6
    f2d4:	e1a03007 	mov	r3, r7
    f2d8:	e0522000 	subs	r2, r2, r0
    f2dc:	e0c33001 	sbc	r3, r3, r1
    f2e0:	e3530000 	cmp	r3, #0	; 0x0
    f2e4:	1a000001 	bne	f2f0 <sata_vs_write+0x304>
    f2e8:	e352001f 	cmp	r2, #31	; 0x1f
    f2ec:	9a000032 	bls	f3bc <sata_vs_write+0x3d0>
    f2f0:	e59d2058 	ldr	r2, [sp, #88]
    f2f4:	e3520a02 	cmp	r2, #8192	; 0x2000
    f2f8:	ba000007 	blt	f31c <sata_vs_write+0x330>
    f2fc:	e1a02006 	mov	r2, r6
    f300:	e1a03007 	mov	r3, r7
    f304:	e0522000 	subs	r2, r2, r0
    f308:	e0c33001 	sbc	r3, r3, r1
    f30c:	e3530000 	cmp	r3, #0	; 0x0
    f310:	1a000001 	bne	f31c <sata_vs_write+0x330>
    f314:	e352000f 	cmp	r2, #15	; 0xf
    f318:	9a000027 	bls	f3bc <sata_vs_write+0x3d0>
    f31c:	e59d3058 	ldr	r3, [sp, #88]
    f320:	e3530a01 	cmp	r3, #4096	; 0x1000
    f324:	ba000007 	blt	f348 <sata_vs_write+0x35c>
    f328:	e1a02006 	mov	r2, r6
    f32c:	e1a03007 	mov	r3, r7
    f330:	e0522000 	subs	r2, r2, r0
    f334:	e0c33001 	sbc	r3, r3, r1
    f338:	e3530000 	cmp	r3, #0	; 0x0
    f33c:	1a000001 	bne	f348 <sata_vs_write+0x35c>
    f340:	e3520007 	cmp	r2, #7	; 0x7
    f344:	9a00001c 	bls	f3bc <sata_vs_write+0x3d0>
    f348:	e59dc058 	ldr	ip, [sp, #88]
    f34c:	e35c0b02 	cmp	ip, #2048	; 0x800
    f350:	ba000007 	blt	f374 <sata_vs_write+0x388>
    f354:	e1a02006 	mov	r2, r6
    f358:	e1a03007 	mov	r3, r7
    f35c:	e0522000 	subs	r2, r2, r0
    f360:	e0c33001 	sbc	r3, r3, r1
    f364:	e3530000 	cmp	r3, #0	; 0x0
    f368:	1a000001 	bne	f374 <sata_vs_write+0x388>
    f36c:	e3520003 	cmp	r2, #3	; 0x3
    f370:	9a000011 	bls	f3bc <sata_vs_write+0x3d0>
    f374:	e59d2058 	ldr	r2, [sp, #88]
    f378:	e3520b01 	cmp	r2, #1024	; 0x400
    f37c:	ba000007 	blt	f3a0 <sata_vs_write+0x3b4>
    f380:	e1a02006 	mov	r2, r6
    f384:	e1a03007 	mov	r3, r7
    f388:	e0522000 	subs	r2, r2, r0
    f38c:	e0c33001 	sbc	r3, r3, r1
    f390:	e3530000 	cmp	r3, #0	; 0x0
    f394:	1a000001 	bne	f3a0 <sata_vs_write+0x3b4>
    f398:	e3520001 	cmp	r2, #1	; 0x1
    f39c:	9a000006 	bls	f3bc <sata_vs_write+0x3d0>
    f3a0:	e59d3058 	ldr	r3, [sp, #88]
    f3a4:	e3530c02 	cmp	r3, #512	; 0x200
    f3a8:	ba00000b 	blt	f3dc <sata_vs_write+0x3f0>
    f3ac:	e1560000 	cmp	r6, r0
    f3b0:	1a000009 	bne	f3dc <sata_vs_write+0x3f0>
    f3b4:	e1570001 	cmp	r7, r1
    f3b8:	1a000007 	bne	f3dc <sata_vs_write+0x3f0>
    f3bc:	e59f02e0 	ldr	r0, [pc, #736]	; f6a4 <sata_vs_write+0x6b8>
    f3c0:	e59f32d8 	ldr	r3, [pc, #728]	; f6a0 <sata_vs_write+0x6b4>
    f3c4:	e1a0e00f 	mov	lr, pc
    f3c8:	e593f000 	ldr	pc, [r3]
    f3cc:	e59d0014 	ldr	r0, [sp, #20]
    f3d0:	ebffc92e 	bl	1890 <free>
    f3d4:	e3e04000 	mvn	r4, #0	; 0x0
    f3d8:	ea0000ac 	b	f690 <sata_vs_write+0x6a4>
    f3dc:	e59dc018 	ldr	ip, [sp, #24]
    f3e0:	e59d0018 	ldr	r0, [sp, #24]
    f3e4:	e59d1018 	ldr	r1, [sp, #24]
    f3e8:	e28ccb01 	add	ip, ip, #1024	; 0x400
    f3ec:	e2800c05 	add	r0, r0, #1280	; 0x500
    f3f0:	e2811d16 	add	r1, r1, #1408	; 0x580
    f3f4:	e59fb2ac 	ldr	fp, [pc, #684]	; f6a8 <sata_vs_write+0x6bc>
    f3f8:	e59d5058 	ldr	r5, [sp, #88]
    f3fc:	e58d8024 	str	r8, [sp, #36]
    f400:	e58dc01c 	str	ip, [sp, #28]
    f404:	e58d0020 	str	r0, [sp, #32]
    f408:	e58d102c 	str	r1, [sp, #44]
    f40c:	e3e024fe 	mvn	r2, #-33554432	; 0xfe000000
    f410:	e1550002 	cmp	r5, r2
    f414:	c3a09501 	movgt	r9, #4194304	; 0x400000
    f418:	c3a08008 	movgt	r8, #8	; 0x8
    f41c:	c3a0a801 	movgt	sl, #65536	; 0x10000
    f420:	ca00001c 	bgt	f498 <sata_vs_write+0x4ac>
    f424:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    f428:	e1550003 	cmp	r5, r3
    f42c:	c3a09501 	movgt	r9, #4194304	; 0x400000
    f430:	c3a08004 	movgt	r8, #4	; 0x4
    f434:	c3a0a902 	movgt	sl, #32768	; 0x8000
    f438:	ca000016 	bgt	f498 <sata_vs_write+0x4ac>
    f43c:	e59fc268 	ldr	ip, [pc, #616]	; f6ac <sata_vs_write+0x6c0>
    f440:	e155000c 	cmp	r5, ip
    f444:	c3a09501 	movgt	r9, #4194304	; 0x400000
    f448:	c3a08002 	movgt	r8, #2	; 0x2
    f44c:	c3a0a901 	movgt	sl, #16384	; 0x4000
    f450:	ca000010 	bgt	f498 <sata_vs_write+0x4ac>
    f454:	e59f0254 	ldr	r0, [pc, #596]	; f6b0 <sata_vs_write+0x6c4>
    f458:	e1550000 	cmp	r5, r0
    f45c:	c3a09501 	movgt	r9, #4194304	; 0x400000
    f460:	c3a08001 	movgt	r8, #1	; 0x1
    f464:	c3a0aa02 	movgt	sl, #8192	; 0x2000
    f468:	ca00000a 	bgt	f498 <sata_vs_write+0x4ac>
    f46c:	e59f1240 	ldr	r1, [pc, #576]	; f6b4 <sata_vs_write+0x6c8>
    f470:	e2453c02 	sub	r3, r5, #512	; 0x200
    f474:	e1530001 	cmp	r3, r1
    f478:	8a000080 	bhi	f680 <sata_vs_write+0x694>
    f47c:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    f480:	e3550000 	cmp	r5, #0	; 0x0
    f484:	e2833003 	add	r3, r3, #3	; 0x3
    f488:	a1a03005 	movge	r3, r5
    f48c:	e1a0a4c3 	asr	sl, r3, #9
    f490:	e1a0948a 	lsl	r9, sl, #9
    f494:	e3a08001 	mov	r8, #1	; 0x1
    f498:	e3a04000 	mov	r4, #0	; 0x0
    f49c:	e1a0200a 	mov	r2, sl
    f4a0:	e59d3028 	ldr	r3, [sp, #40]
    f4a4:	e59d0020 	ldr	r0, [sp, #32]
    f4a8:	e3a01035 	mov	r1, #53	; 0x35
    f4ac:	e58d4000 	str	r4, [sp]
    f4b0:	e58d4004 	str	r4, [sp, #4]
    f4b4:	ebfffde5 	bl	ec50 <CreateAtaCFIS>
    f4b8:	e1a03208 	lsl	r3, r8, #4
    f4bc:	e1a02004 	mov	r2, r4
    f4c0:	e59dc02c 	ldr	ip, [sp, #44]
    f4c4:	e7cc2004 	strb	r2, [ip, r4]
    f4c8:	e2844001 	add	r4, r4, #1	; 0x1
    f4cc:	e1540003 	cmp	r4, r3
    f4d0:	3afffffa 	bcc	f4c0 <sata_vs_write+0x4d4>
    f4d4:	e2493001 	sub	r3, r9, #1	; 0x1
    f4d8:	e3a01000 	mov	r1, #0	; 0x0
    f4dc:	e1a04503 	lsl	r4, r3, #10
    f4e0:	e59dc024 	ldr	ip, [sp, #36]
    f4e4:	ea00000b 	b	f518 <sata_vs_write+0x52c>
    f4e8:	e3a03000 	mov	r3, #0	; 0x0
    f4ec:	e58e3004 	str	r3, [lr, #4]
    f4f0:	e59d3020 	ldr	r3, [sp, #32]
    f4f4:	e783c202 	str	ip, [r3, r2, lsl #4]
    f4f8:	e5902004 	ldr	r2, [r0, #4]
    f4fc:	e08cc009 	add	ip, ip, r9
    f500:	e1843b22 	orr	r3, r4, r2, lsr #22
    f504:	e1a02563 	ror	r2, r3, #10
    f508:	e1a03c22 	lsr	r3, r2, #24
    f50c:	e3c33080 	bic	r3, r3, #128	; 0x80
    f510:	e5802004 	str	r2, [r0, #4]
    f514:	e5c03007 	strb	r3, [r0, #7]
    f518:	e59d0020 	ldr	r0, [sp, #32]
    f51c:	e2812008 	add	r2, r1, #8	; 0x8
    f520:	e0803201 	add	r3, r0, r1, lsl #4
    f524:	e080e202 	add	lr, r0, r2, lsl #4
    f528:	e1510008 	cmp	r1, r8
    f52c:	e2830088 	add	r0, r3, #136	; 0x88
    f530:	e2813001 	add	r3, r1, #1	; 0x1
    f534:	e6ff1073 	uxth	r1, r3
    f538:	3affffea 	bcc	f4e8 <sata_vs_write+0x4fc>
    f53c:	e3a04000 	mov	r4, #0	; 0x0
    f540:	e1a03008 	mov	r3, r8
    f544:	e3a0c001 	mov	ip, #1	; 0x1
    f548:	e59d0018 	ldr	r0, [sp, #24]
    f54c:	e1a01004 	mov	r1, r4
    f550:	e59d2020 	ldr	r2, [sp, #32]
    f554:	e88d1010 	stm	sp, {r4, ip}
    f558:	e58d4008 	str	r4, [sp, #8]
    f55c:	e58d400c 	str	r4, [sp, #12]
    f560:	ebfffdf8 	bl	ed48 <CreateCommandHeader>
    f564:	e1a03004 	mov	r3, r4
    f568:	e59d001c 	ldr	r0, [sp, #28]
    f56c:	e7c03004 	strb	r3, [r0, r4]
    f570:	e2844001 	add	r4, r4, #1	; 0x1
    f574:	e3540c01 	cmp	r4, #256	; 0x100
    f578:	1afffffa 	bne	f568 <sata_vs_write+0x57c>
    f57c:	e59d1018 	ldr	r1, [sp, #24]
    f580:	e3e03000 	mvn	r3, #0	; 0x0
    f584:	e58b1100 	str	r1, [fp, #256]
    f588:	e58b0108 	str	r0, [fp, #264]
    f58c:	e58b3110 	str	r3, [fp, #272]
    f590:	e58b3130 	str	r3, [fp, #304]
    f594:	e59b3118 	ldr	r3, [fp, #280]
    f598:	e3a02001 	mov	r2, #1	; 0x1
    f59c:	e3833010 	orr	r3, r3, #16	; 0x10
    f5a0:	e58b3118 	str	r3, [fp, #280]
    f5a4:	e59b3118 	ldr	r3, [fp, #280]
    f5a8:	e3833001 	orr	r3, r3, #1	; 0x1
    f5ac:	e58b3118 	str	r3, [fp, #280]
    f5b0:	e58b2138 	str	r2, [fp, #312]
    f5b4:	e59b3110 	ldr	r3, [fp, #272]
    f5b8:	e3130001 	tst	r3, #1	; 0x1
    f5bc:	0afffffc 	beq	f5b4 <sata_vs_write+0x5c8>
    f5c0:	e59fc0e0 	ldr	ip, [pc, #224]	; f6a8 <sata_vs_write+0x6bc>
    f5c4:	e3e03000 	mvn	r3, #0	; 0x0
    f5c8:	e58c3110 	str	r3, [ip, #272]
    f5cc:	e58c3008 	str	r3, [ip, #8]
    f5d0:	e59c2118 	ldr	r2, [ip, #280]
    f5d4:	e30c3011 	movw	r3, #49169	; 0xc011
    f5d8:	e0023003 	and	r3, r2, r3
    f5dc:	e3530000 	cmp	r3, #0	; 0x0
    f5e0:	0a00000c 	beq	f618 <sata_vs_write+0x62c>
    f5e4:	e59c3118 	ldr	r3, [ip, #280]
    f5e8:	e3c33001 	bic	r3, r3, #1	; 0x1
    f5ec:	e58c3118 	str	r3, [ip, #280]
    f5f0:	e59b3118 	ldr	r3, [fp, #280]
    f5f4:	e3130902 	tst	r3, #32768	; 0x8000
    f5f8:	1afffffc 	bne	f5f0 <sata_vs_write+0x604>
    f5fc:	e59f00a4 	ldr	r0, [pc, #164]	; f6a8 <sata_vs_write+0x6bc>
    f600:	e5903118 	ldr	r3, [r0, #280]
    f604:	e3c33010 	bic	r3, r3, #16	; 0x10
    f608:	e5803118 	str	r3, [r0, #280]
    f60c:	e59b3118 	ldr	r3, [fp, #280]
    f610:	e3130901 	tst	r3, #16384	; 0x4000
    f614:	1afffffc 	bne	f60c <sata_vs_write+0x620>
    f618:	e59b3120 	ldr	r3, [fp, #288]
    f61c:	e3130088 	tst	r3, #136	; 0x88
    f620:	1afffffc 	bne	f618 <sata_vs_write+0x62c>
    f624:	e0010899 	mul	r1, r9, r8
    f628:	e0555001 	subs	r5, r5, r1
    f62c:	0a000013 	beq	f680 <sata_vs_write+0x694>
    f630:	e59dc028 	ldr	ip, [sp, #40]
    f634:	e1b02b85 	lsls	r2, r5, #23
    f638:	13a02001 	movne	r2, #1	; 0x1
    f63c:	e08cc00a 	add	ip, ip, sl
    f640:	e3550000 	cmp	r5, #0	; 0x0
    f644:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    f648:	e2833003 	add	r3, r3, #3	; 0x3
    f64c:	e082200c 	add	r2, r2, ip
    f650:	a1a03005 	movge	r3, r5
    f654:	e3570000 	cmp	r7, #0	; 0x0
    f658:	e58dc028 	str	ip, [sp, #40]
    f65c:	e08234c3 	add	r3, r2, r3, asr #9
    f660:	8a000002 	bhi	f670 <sata_vs_write+0x684>
    f664:	1a000005 	bne	f680 <sata_vs_write+0x694>
    f668:	e1560003 	cmp	r6, r3
    f66c:	9a000003 	bls	f680 <sata_vs_write+0x694>
    f670:	e59d0024 	ldr	r0, [sp, #36]
    f674:	e0800001 	add	r0, r0, r1
    f678:	e58d0024 	str	r0, [sp, #36]
    f67c:	eaffff62 	b	f40c <sata_vs_write+0x420>
    f680:	e59d1058 	ldr	r1, [sp, #88]
    f684:	e59d0014 	ldr	r0, [sp, #20]
    f688:	e0654001 	rsb	r4, r5, r1
    f68c:	ebffc87f 	bl	1890 <free>
    f690:	e1a00004 	mov	r0, r4
    f694:	e28dd034 	add	sp, sp, #52	; 0x34
    f698:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    f69c:	000188e1 	.word	0x000188e1
    f6a0:	3c00d6cc 	.word	0x3c00d6cc
    f6a4:	00018904 	.word	0x00018904
    f6a8:	28200000 	.word	0x28200000
    f6ac:	007fffff 	.word	0x007fffff
    f6b0:	003fffff 	.word	0x003fffff
    f6b4:	003ffdff 	.word	0x003ffdff

0000f6b8 <sata_vs_read>:
    f6b8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
    f6bc:	e1a08000 	mov	r8, r0
    f6c0:	e24dd034 	sub	sp, sp, #52	; 0x34
    f6c4:	e3a00c0e 	mov	r0, #3584	; 0xe00
    f6c8:	e1a06002 	mov	r6, r2
    f6cc:	e1a07003 	mov	r7, r3
    f6d0:	ebffc806 	bl	16f0 <malloc>
    f6d4:	e3500000 	cmp	r0, #0	; 0x0
    f6d8:	e58d0014 	str	r0, [sp, #20]
    f6dc:	1a000004 	bne	f6f4 <sata_vs_read+0x3c>
    f6e0:	e59f0680 	ldr	r0, [pc, #1664]	; fd68 <sata_vs_read+0x6b0>
    f6e4:	e59f3680 	ldr	r3, [pc, #1664]	; fd6c <sata_vs_read+0x6b4>
    f6e8:	e1a0e00f 	mov	lr, pc
    f6ec:	e593f000 	ldr	pc, [r3]
    f6f0:	ea0000ea 	b	faa0 <sata_vs_read+0x3e8>
    f6f4:	e59d0014 	ldr	r0, [sp, #20]
    f6f8:	e2803fff 	add	r3, r0, #1020	; 0x3fc
    f6fc:	e2833003 	add	r3, r3, #3	; 0x3
    f700:	e3c33fff 	bic	r3, r3, #1020	; 0x3fc
    f704:	e3c33003 	bic	r3, r3, #3	; 0x3
    f708:	e1530000 	cmp	r3, r0
    f70c:	e58d3018 	str	r3, [sp, #24]
    f710:	02833a01 	addeq	r3, r3, #4096	; 0x1000
    f714:	058d3018 	streq	r3, [sp, #24]
    f718:	e59d0018 	ldr	r0, [sp, #24]
    f71c:	ebfffdcb 	bl	ee50 <get_max_lba>
    f720:	e59d1058 	ldr	r1, [sp, #88]
    f724:	e1a0c000 	mov	ip, r0
    f728:	e1a03b81 	lsl	r3, r1, #23
    f72c:	e1a03ba3 	lsr	r3, r3, #23
    f730:	e3530000 	cmp	r3, #0	; 0x0
    f734:	1a0000d3 	bne	fa88 <sata_vs_read+0x3d0>
    f738:	e30041ff 	movw	r4, #511	; 0x1ff
    f73c:	e3a05000 	mov	r5, #0	; 0x0
    f740:	e0062004 	and	r2, r6, r4
    f744:	e0073005 	and	r3, r7, r5
    f748:	e1920003 	orrs	r0, r2, r3
    f74c:	1a0000cd 	bne	fa88 <sata_vs_read+0x3d0>
    f750:	e3180001 	tst	r8, #1	; 0x1
    f754:	1a0000cb 	bne	fa88 <sata_vs_read+0x3d0>
    f758:	e1a00007 	mov	r0, r7
    f75c:	e1a01fc0 	asr	r1, r0, #31
    f760:	e1a02fc1 	asr	r2, r1, #31
    f764:	e1a03002 	mov	r3, r2
    f768:	e0022004 	and	r2, r2, r4
    f76c:	e0922006 	adds	r2, r2, r6
    f770:	e0033005 	and	r3, r3, r5
    f774:	e0a33007 	adc	r3, r3, r7
    f778:	e1a014a2 	lsr	r1, r2, #9
    f77c:	e1a0600c 	mov	r6, ip
    f780:	e1a07fc6 	asr	r7, r6, #31
    f784:	e1811b83 	orr	r1, r1, r3, lsl #23
    f788:	e3570000 	cmp	r7, #0	; 0x0
    f78c:	e58d1028 	str	r1, [sp, #40]
    f790:	e1a00001 	mov	r0, r1
    f794:	e3a01000 	mov	r1, #0	; 0x0
    f798:	8a000002 	bhi	f7a8 <sata_vs_read+0xf0>
    f79c:	1a0000b9 	bne	fa88 <sata_vs_read+0x3d0>
    f7a0:	e1560000 	cmp	r6, r0
    f7a4:	9a0000b7 	bls	fa88 <sata_vs_read+0x3d0>
    f7a8:	e59d2058 	ldr	r2, [sp, #88]
    f7ac:	e3520402 	cmp	r2, #33554432	; 0x2000000
    f7b0:	ba000007 	blt	f7d4 <sata_vs_read+0x11c>
    f7b4:	e1a02006 	mov	r2, r6
    f7b8:	e1a03007 	mov	r3, r7
    f7bc:	e0522000 	subs	r2, r2, r0
    f7c0:	e0c33001 	sbc	r3, r3, r1
    f7c4:	e3530000 	cmp	r3, #0	; 0x0
    f7c8:	1a000001 	bne	f7d4 <sata_vs_read+0x11c>
    f7cc:	e3520801 	cmp	r2, #65536	; 0x10000
    f7d0:	3a0000ac 	bcc	fa88 <sata_vs_read+0x3d0>
    f7d4:	e59d3058 	ldr	r3, [sp, #88]
    f7d8:	e3530401 	cmp	r3, #16777216	; 0x1000000
    f7dc:	ba000007 	blt	f800 <sata_vs_read+0x148>
    f7e0:	e1a02006 	mov	r2, r6
    f7e4:	e1a03007 	mov	r3, r7
    f7e8:	e0522000 	subs	r2, r2, r0
    f7ec:	e0c33001 	sbc	r3, r3, r1
    f7f0:	e3530000 	cmp	r3, #0	; 0x0
    f7f4:	1a000001 	bne	f800 <sata_vs_read+0x148>
    f7f8:	e3520902 	cmp	r2, #32768	; 0x8000
    f7fc:	3a0000a1 	bcc	fa88 <sata_vs_read+0x3d0>
    f800:	e59d2058 	ldr	r2, [sp, #88]
    f804:	e3520502 	cmp	r2, #8388608	; 0x800000
    f808:	ba000007 	blt	f82c <sata_vs_read+0x174>
    f80c:	e1a02006 	mov	r2, r6
    f810:	e1a03007 	mov	r3, r7
    f814:	e0522000 	subs	r2, r2, r0
    f818:	e0c33001 	sbc	r3, r3, r1
    f81c:	e3530000 	cmp	r3, #0	; 0x0
    f820:	1a000001 	bne	f82c <sata_vs_read+0x174>
    f824:	e3520901 	cmp	r2, #16384	; 0x4000
    f828:	3a000096 	bcc	fa88 <sata_vs_read+0x3d0>
    f82c:	e59d3058 	ldr	r3, [sp, #88]
    f830:	e3530501 	cmp	r3, #4194304	; 0x400000
    f834:	ba000007 	blt	f858 <sata_vs_read+0x1a0>
    f838:	e1a02006 	mov	r2, r6
    f83c:	e1a03007 	mov	r3, r7
    f840:	e0522000 	subs	r2, r2, r0
    f844:	e0c33001 	sbc	r3, r3, r1
    f848:	e3530000 	cmp	r3, #0	; 0x0
    f84c:	1a000001 	bne	f858 <sata_vs_read+0x1a0>
    f850:	e3520a02 	cmp	r2, #8192	; 0x2000
    f854:	3a00008b 	bcc	fa88 <sata_vs_read+0x3d0>
    f858:	e59d2058 	ldr	r2, [sp, #88]
    f85c:	e3520602 	cmp	r2, #2097152	; 0x200000
    f860:	ba000007 	blt	f884 <sata_vs_read+0x1cc>
    f864:	e1a02006 	mov	r2, r6
    f868:	e1a03007 	mov	r3, r7
    f86c:	e0522000 	subs	r2, r2, r0
    f870:	e0c33001 	sbc	r3, r3, r1
    f874:	e3530000 	cmp	r3, #0	; 0x0
    f878:	1a000001 	bne	f884 <sata_vs_read+0x1cc>
    f87c:	e3520a01 	cmp	r2, #4096	; 0x1000
    f880:	3a000080 	bcc	fa88 <sata_vs_read+0x3d0>
    f884:	e59d3058 	ldr	r3, [sp, #88]
    f888:	e3530601 	cmp	r3, #1048576	; 0x100000
    f88c:	ba000007 	blt	f8b0 <sata_vs_read+0x1f8>
    f890:	e1a02006 	mov	r2, r6
    f894:	e1a03007 	mov	r3, r7
    f898:	e0522000 	subs	r2, r2, r0
    f89c:	e0c33001 	sbc	r3, r3, r1
    f8a0:	e3530000 	cmp	r3, #0	; 0x0
    f8a4:	1a000001 	bne	f8b0 <sata_vs_read+0x1f8>
    f8a8:	e3520b02 	cmp	r2, #2048	; 0x800
    f8ac:	3a000075 	bcc	fa88 <sata_vs_read+0x3d0>
    f8b0:	e59d2058 	ldr	r2, [sp, #88]
    f8b4:	e3520702 	cmp	r2, #524288	; 0x80000
    f8b8:	ba000007 	blt	f8dc <sata_vs_read+0x224>
    f8bc:	e1a02006 	mov	r2, r6
    f8c0:	e1a03007 	mov	r3, r7
    f8c4:	e0522000 	subs	r2, r2, r0
    f8c8:	e0c33001 	sbc	r3, r3, r1
    f8cc:	e3530000 	cmp	r3, #0	; 0x0
    f8d0:	1a000001 	bne	f8dc <sata_vs_read+0x224>
    f8d4:	e3520b01 	cmp	r2, #1024	; 0x400
    f8d8:	3a00006a 	bcc	fa88 <sata_vs_read+0x3d0>
    f8dc:	e59d3058 	ldr	r3, [sp, #88]
    f8e0:	e3530701 	cmp	r3, #262144	; 0x40000
    f8e4:	ba000007 	blt	f908 <sata_vs_read+0x250>
    f8e8:	e1a02006 	mov	r2, r6
    f8ec:	e1a03007 	mov	r3, r7
    f8f0:	e0522000 	subs	r2, r2, r0
    f8f4:	e0c33001 	sbc	r3, r3, r1
    f8f8:	e3530000 	cmp	r3, #0	; 0x0
    f8fc:	1a000001 	bne	f908 <sata_vs_read+0x250>
    f900:	e3520c02 	cmp	r2, #512	; 0x200
    f904:	3a00005f 	bcc	fa88 <sata_vs_read+0x3d0>
    f908:	e59d2058 	ldr	r2, [sp, #88]
    f90c:	e3520802 	cmp	r2, #131072	; 0x20000
    f910:	ba000008 	blt	f938 <sata_vs_read+0x280>
    f914:	e3e020ff 	mvn	r2, #255	; 0xff
    f918:	e0922006 	adds	r2, r2, r6
    f91c:	e3e03000 	mvn	r3, #0	; 0x0
    f920:	e0a33007 	adc	r3, r3, r7
    f924:	e1510003 	cmp	r1, r3
    f928:	8a000056 	bhi	fa88 <sata_vs_read+0x3d0>
    f92c:	1a000001 	bne	f938 <sata_vs_read+0x280>
    f930:	e1500002 	cmp	r0, r2
    f934:	8a000053 	bhi	fa88 <sata_vs_read+0x3d0>
    f938:	e59d3058 	ldr	r3, [sp, #88]
    f93c:	e3530801 	cmp	r3, #65536	; 0x10000
    f940:	ba000007 	blt	f964 <sata_vs_read+0x2ac>
    f944:	e1a02006 	mov	r2, r6
    f948:	e1a03007 	mov	r3, r7
    f94c:	e0522000 	subs	r2, r2, r0
    f950:	e0c33001 	sbc	r3, r3, r1
    f954:	e3530000 	cmp	r3, #0	; 0x0
    f958:	1a000001 	bne	f964 <sata_vs_read+0x2ac>
    f95c:	e352007f 	cmp	r2, #127	; 0x7f
    f960:	9a000048 	bls	fa88 <sata_vs_read+0x3d0>
    f964:	e59d2058 	ldr	r2, [sp, #88]
    f968:	e3520902 	cmp	r2, #32768	; 0x8000
    f96c:	ba000007 	blt	f990 <sata_vs_read+0x2d8>
    f970:	e1a02006 	mov	r2, r6
    f974:	e1a03007 	mov	r3, r7
    f978:	e0522000 	subs	r2, r2, r0
    f97c:	e0c33001 	sbc	r3, r3, r1
    f980:	e3530000 	cmp	r3, #0	; 0x0
    f984:	1a000001 	bne	f990 <sata_vs_read+0x2d8>
    f988:	e352003f 	cmp	r2, #63	; 0x3f
    f98c:	9a00003d 	bls	fa88 <sata_vs_read+0x3d0>
    f990:	e59d3058 	ldr	r3, [sp, #88]
    f994:	e3530901 	cmp	r3, #16384	; 0x4000
    f998:	ba000007 	blt	f9bc <sata_vs_read+0x304>
    f99c:	e1a02006 	mov	r2, r6
    f9a0:	e1a03007 	mov	r3, r7
    f9a4:	e0522000 	subs	r2, r2, r0
    f9a8:	e0c33001 	sbc	r3, r3, r1
    f9ac:	e3530000 	cmp	r3, #0	; 0x0
    f9b0:	1a000001 	bne	f9bc <sata_vs_read+0x304>
    f9b4:	e352001f 	cmp	r2, #31	; 0x1f
    f9b8:	9a000032 	bls	fa88 <sata_vs_read+0x3d0>
    f9bc:	e59d2058 	ldr	r2, [sp, #88]
    f9c0:	e3520a02 	cmp	r2, #8192	; 0x2000
    f9c4:	ba000007 	blt	f9e8 <sata_vs_read+0x330>
    f9c8:	e1a02006 	mov	r2, r6
    f9cc:	e1a03007 	mov	r3, r7
    f9d0:	e0522000 	subs	r2, r2, r0
    f9d4:	e0c33001 	sbc	r3, r3, r1
    f9d8:	e3530000 	cmp	r3, #0	; 0x0
    f9dc:	1a000001 	bne	f9e8 <sata_vs_read+0x330>
    f9e0:	e352000f 	cmp	r2, #15	; 0xf
    f9e4:	9a000027 	bls	fa88 <sata_vs_read+0x3d0>
    f9e8:	e59d3058 	ldr	r3, [sp, #88]
    f9ec:	e3530a01 	cmp	r3, #4096	; 0x1000
    f9f0:	ba000007 	blt	fa14 <sata_vs_read+0x35c>
    f9f4:	e1a02006 	mov	r2, r6
    f9f8:	e1a03007 	mov	r3, r7
    f9fc:	e0522000 	subs	r2, r2, r0
    fa00:	e0c33001 	sbc	r3, r3, r1
    fa04:	e3530000 	cmp	r3, #0	; 0x0
    fa08:	1a000001 	bne	fa14 <sata_vs_read+0x35c>
    fa0c:	e3520007 	cmp	r2, #7	; 0x7
    fa10:	9a00001c 	bls	fa88 <sata_vs_read+0x3d0>
    fa14:	e59d2058 	ldr	r2, [sp, #88]
    fa18:	e3520b02 	cmp	r2, #2048	; 0x800
    fa1c:	ba000007 	blt	fa40 <sata_vs_read+0x388>
    fa20:	e1a02006 	mov	r2, r6
    fa24:	e1a03007 	mov	r3, r7
    fa28:	e0522000 	subs	r2, r2, r0
    fa2c:	e0c33001 	sbc	r3, r3, r1
    fa30:	e3530000 	cmp	r3, #0	; 0x0
    fa34:	1a000001 	bne	fa40 <sata_vs_read+0x388>
    fa38:	e3520003 	cmp	r2, #3	; 0x3
    fa3c:	9a000011 	bls	fa88 <sata_vs_read+0x3d0>
    fa40:	e59d3058 	ldr	r3, [sp, #88]
    fa44:	e3530b01 	cmp	r3, #1024	; 0x400
    fa48:	ba000007 	blt	fa6c <sata_vs_read+0x3b4>
    fa4c:	e1a02006 	mov	r2, r6
    fa50:	e1a03007 	mov	r3, r7
    fa54:	e0522000 	subs	r2, r2, r0
    fa58:	e0c33001 	sbc	r3, r3, r1
    fa5c:	e3530000 	cmp	r3, #0	; 0x0
    fa60:	1a000001 	bne	fa6c <sata_vs_read+0x3b4>
    fa64:	e3520001 	cmp	r2, #1	; 0x1
    fa68:	9a000006 	bls	fa88 <sata_vs_read+0x3d0>
    fa6c:	e59d2058 	ldr	r2, [sp, #88]
    fa70:	e3520c02 	cmp	r2, #512	; 0x200
    fa74:	ba00000b 	blt	faa8 <sata_vs_read+0x3f0>
    fa78:	e1560000 	cmp	r6, r0
    fa7c:	1a000009 	bne	faa8 <sata_vs_read+0x3f0>
    fa80:	e1570001 	cmp	r7, r1
    fa84:	1a000007 	bne	faa8 <sata_vs_read+0x3f0>
    fa88:	e59f02e0 	ldr	r0, [pc, #736]	; fd70 <sata_vs_read+0x6b8>
    fa8c:	e59f32d8 	ldr	r3, [pc, #728]	; fd6c <sata_vs_read+0x6b4>
    fa90:	e1a0e00f 	mov	lr, pc
    fa94:	e593f000 	ldr	pc, [r3]
    fa98:	e59d0014 	ldr	r0, [sp, #20]
    fa9c:	ebffc77b 	bl	1890 <free>
    faa0:	e3e04000 	mvn	r4, #0	; 0x0
    faa4:	ea0000ac 	b	fd5c <sata_vs_read+0x6a4>
    faa8:	e59d3018 	ldr	r3, [sp, #24]
    faac:	e59d0018 	ldr	r0, [sp, #24]
    fab0:	e59d1018 	ldr	r1, [sp, #24]
    fab4:	e2833b01 	add	r3, r3, #1024	; 0x400
    fab8:	e2800c05 	add	r0, r0, #1280	; 0x500
    fabc:	e2811d16 	add	r1, r1, #1408	; 0x580
    fac0:	e59fb2ac 	ldr	fp, [pc, #684]	; fd74 <sata_vs_read+0x6bc>
    fac4:	e59d5058 	ldr	r5, [sp, #88]
    fac8:	e58d8024 	str	r8, [sp, #36]
    facc:	e58d301c 	str	r3, [sp, #28]
    fad0:	e58d0020 	str	r0, [sp, #32]
    fad4:	e58d102c 	str	r1, [sp, #44]
    fad8:	e3e024fe 	mvn	r2, #-33554432	; 0xfe000000
    fadc:	e1550002 	cmp	r5, r2
    fae0:	c3a09501 	movgt	r9, #4194304	; 0x400000
    fae4:	c3a08008 	movgt	r8, #8	; 0x8
    fae8:	c3a0a801 	movgt	sl, #65536	; 0x10000
    faec:	ca00001c 	bgt	fb64 <sata_vs_read+0x4ac>
    faf0:	e3e034ff 	mvn	r3, #-16777216	; 0xff000000
    faf4:	e1550003 	cmp	r5, r3
    faf8:	c3a09501 	movgt	r9, #4194304	; 0x400000
    fafc:	c3a08004 	movgt	r8, #4	; 0x4
    fb00:	c3a0a902 	movgt	sl, #32768	; 0x8000
    fb04:	ca000016 	bgt	fb64 <sata_vs_read+0x4ac>
    fb08:	e59f0268 	ldr	r0, [pc, #616]	; fd78 <sata_vs_read+0x6c0>
    fb0c:	e1550000 	cmp	r5, r0
    fb10:	c3a09501 	movgt	r9, #4194304	; 0x400000
    fb14:	c3a08002 	movgt	r8, #2	; 0x2
    fb18:	c3a0a901 	movgt	sl, #16384	; 0x4000
    fb1c:	ca000010 	bgt	fb64 <sata_vs_read+0x4ac>
    fb20:	e59f1254 	ldr	r1, [pc, #596]	; fd7c <sata_vs_read+0x6c4>
    fb24:	e1550001 	cmp	r5, r1
    fb28:	c3a09501 	movgt	r9, #4194304	; 0x400000
    fb2c:	c3a08001 	movgt	r8, #1	; 0x1
    fb30:	c3a0aa02 	movgt	sl, #8192	; 0x2000
    fb34:	ca00000a 	bgt	fb64 <sata_vs_read+0x4ac>
    fb38:	e59f2240 	ldr	r2, [pc, #576]	; fd80 <sata_vs_read+0x6c8>
    fb3c:	e2453c02 	sub	r3, r5, #512	; 0x200
    fb40:	e1530002 	cmp	r3, r2
    fb44:	8a000080 	bhi	fd4c <sata_vs_read+0x694>
    fb48:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    fb4c:	e3550000 	cmp	r5, #0	; 0x0
    fb50:	e2833003 	add	r3, r3, #3	; 0x3
    fb54:	a1a03005 	movge	r3, r5
    fb58:	e1a0a4c3 	asr	sl, r3, #9
    fb5c:	e1a0948a 	lsl	r9, sl, #9
    fb60:	e3a08001 	mov	r8, #1	; 0x1
    fb64:	e3a04000 	mov	r4, #0	; 0x0
    fb68:	e1a0200a 	mov	r2, sl
    fb6c:	e59d3028 	ldr	r3, [sp, #40]
    fb70:	e59d0020 	ldr	r0, [sp, #32]
    fb74:	e3a01025 	mov	r1, #37	; 0x25
    fb78:	e58d4000 	str	r4, [sp]
    fb7c:	e58d4004 	str	r4, [sp, #4]
    fb80:	ebfffc32 	bl	ec50 <CreateAtaCFIS>
    fb84:	e1a03208 	lsl	r3, r8, #4
    fb88:	e1a02004 	mov	r2, r4
    fb8c:	e59d002c 	ldr	r0, [sp, #44]
    fb90:	e7c02004 	strb	r2, [r0, r4]
    fb94:	e2844001 	add	r4, r4, #1	; 0x1
    fb98:	e1540003 	cmp	r4, r3
    fb9c:	3afffffa 	bcc	fb8c <sata_vs_read+0x4d4>
    fba0:	e2493001 	sub	r3, r9, #1	; 0x1
    fba4:	e3a01000 	mov	r1, #0	; 0x0
    fba8:	e1a04503 	lsl	r4, r3, #10
    fbac:	e59dc024 	ldr	ip, [sp, #36]
    fbb0:	ea00000b 	b	fbe4 <sata_vs_read+0x52c>
    fbb4:	e3a03000 	mov	r3, #0	; 0x0
    fbb8:	e58e3004 	str	r3, [lr, #4]
    fbbc:	e59d3020 	ldr	r3, [sp, #32]
    fbc0:	e783c202 	str	ip, [r3, r2, lsl #4]
    fbc4:	e5902004 	ldr	r2, [r0, #4]
    fbc8:	e08cc009 	add	ip, ip, r9
    fbcc:	e1843b22 	orr	r3, r4, r2, lsr #22
    fbd0:	e1a02563 	ror	r2, r3, #10
    fbd4:	e1a03c22 	lsr	r3, r2, #24
    fbd8:	e3c33080 	bic	r3, r3, #128	; 0x80
    fbdc:	e5802004 	str	r2, [r0, #4]
    fbe0:	e5c03007 	strb	r3, [r0, #7]
    fbe4:	e59d0020 	ldr	r0, [sp, #32]
    fbe8:	e2812008 	add	r2, r1, #8	; 0x8
    fbec:	e0803201 	add	r3, r0, r1, lsl #4
    fbf0:	e080e202 	add	lr, r0, r2, lsl #4
    fbf4:	e1510008 	cmp	r1, r8
    fbf8:	e2830088 	add	r0, r3, #136	; 0x88
    fbfc:	e2813001 	add	r3, r1, #1	; 0x1
    fc00:	e6ff1073 	uxth	r1, r3
    fc04:	3affffea 	bcc	fbb4 <sata_vs_read+0x4fc>
    fc08:	e3a04000 	mov	r4, #0	; 0x0
    fc0c:	e1a03008 	mov	r3, r8
    fc10:	e59d0018 	ldr	r0, [sp, #24]
    fc14:	e1a01004 	mov	r1, r4
    fc18:	e59d2020 	ldr	r2, [sp, #32]
    fc1c:	e58d4000 	str	r4, [sp]
    fc20:	e58d4004 	str	r4, [sp, #4]
    fc24:	e58d4008 	str	r4, [sp, #8]
    fc28:	e58d400c 	str	r4, [sp, #12]
    fc2c:	ebfffc45 	bl	ed48 <CreateCommandHeader>
    fc30:	e1a03004 	mov	r3, r4
    fc34:	e59d101c 	ldr	r1, [sp, #28]
    fc38:	e7c13004 	strb	r3, [r1, r4]
    fc3c:	e2844001 	add	r4, r4, #1	; 0x1
    fc40:	e3540c01 	cmp	r4, #256	; 0x100
    fc44:	1afffffa 	bne	fc34 <sata_vs_read+0x57c>
    fc48:	e59d2018 	ldr	r2, [sp, #24]
    fc4c:	e3e03000 	mvn	r3, #0	; 0x0
    fc50:	e58b2100 	str	r2, [fp, #256]
    fc54:	e58b1108 	str	r1, [fp, #264]
    fc58:	e58b3110 	str	r3, [fp, #272]
    fc5c:	e58b3130 	str	r3, [fp, #304]
    fc60:	e59b3118 	ldr	r3, [fp, #280]
    fc64:	e3833010 	orr	r3, r3, #16	; 0x10
    fc68:	e58b3118 	str	r3, [fp, #280]
    fc6c:	e59b3118 	ldr	r3, [fp, #280]
    fc70:	e3833001 	orr	r3, r3, #1	; 0x1
    fc74:	e58b3118 	str	r3, [fp, #280]
    fc78:	e3a03001 	mov	r3, #1	; 0x1
    fc7c:	e58b3138 	str	r3, [fp, #312]
    fc80:	e59b3110 	ldr	r3, [fp, #272]
    fc84:	e3130001 	tst	r3, #1	; 0x1
    fc88:	0afffffc 	beq	fc80 <sata_vs_read+0x5c8>
    fc8c:	e59f00e0 	ldr	r0, [pc, #224]	; fd74 <sata_vs_read+0x6bc>
    fc90:	e3e03000 	mvn	r3, #0	; 0x0
    fc94:	e5803110 	str	r3, [r0, #272]
    fc98:	e5803008 	str	r3, [r0, #8]
    fc9c:	e5902118 	ldr	r2, [r0, #280]
    fca0:	e30c3011 	movw	r3, #49169	; 0xc011
    fca4:	e0023003 	and	r3, r2, r3
    fca8:	e3530000 	cmp	r3, #0	; 0x0
    fcac:	0a00000c 	beq	fce4 <sata_vs_read+0x62c>
    fcb0:	e5903118 	ldr	r3, [r0, #280]
    fcb4:	e3c33001 	bic	r3, r3, #1	; 0x1
    fcb8:	e5803118 	str	r3, [r0, #280]
    fcbc:	e59b3118 	ldr	r3, [fp, #280]
    fcc0:	e3130902 	tst	r3, #32768	; 0x8000
    fcc4:	1afffffc 	bne	fcbc <sata_vs_read+0x604>
    fcc8:	e59f10a4 	ldr	r1, [pc, #164]	; fd74 <sata_vs_read+0x6bc>
    fccc:	e5913118 	ldr	r3, [r1, #280]
    fcd0:	e3c33010 	bic	r3, r3, #16	; 0x10
    fcd4:	e5813118 	str	r3, [r1, #280]
    fcd8:	e59b3118 	ldr	r3, [fp, #280]
    fcdc:	e3130901 	tst	r3, #16384	; 0x4000
    fce0:	1afffffc 	bne	fcd8 <sata_vs_read+0x620>
    fce4:	e59b3120 	ldr	r3, [fp, #288]
    fce8:	e3130088 	tst	r3, #136	; 0x88
    fcec:	1afffffc 	bne	fce4 <sata_vs_read+0x62c>
    fcf0:	e0010899 	mul	r1, r9, r8
    fcf4:	e0555001 	subs	r5, r5, r1
    fcf8:	0a000013 	beq	fd4c <sata_vs_read+0x694>
    fcfc:	e59d0028 	ldr	r0, [sp, #40]
    fd00:	e1b02b85 	lsls	r2, r5, #23
    fd04:	13a02001 	movne	r2, #1	; 0x1
    fd08:	e080000a 	add	r0, r0, sl
    fd0c:	e3550000 	cmp	r5, #0	; 0x0
    fd10:	e2853f7f 	add	r3, r5, #508	; 0x1fc
    fd14:	e2833003 	add	r3, r3, #3	; 0x3
    fd18:	e0822000 	add	r2, r2, r0
    fd1c:	a1a03005 	movge	r3, r5
    fd20:	e3570000 	cmp	r7, #0	; 0x0
    fd24:	e58d0028 	str	r0, [sp, #40]
    fd28:	e08234c3 	add	r3, r2, r3, asr #9
    fd2c:	8a000002 	bhi	fd3c <sata_vs_read+0x684>
    fd30:	1a000005 	bne	fd4c <sata_vs_read+0x694>
    fd34:	e1560003 	cmp	r6, r3
    fd38:	9a000003 	bls	fd4c <sata_vs_read+0x694>
    fd3c:	e59d2024 	ldr	r2, [sp, #36]
    fd40:	e0822001 	add	r2, r2, r1
    fd44:	e58d2024 	str	r2, [sp, #36]
    fd48:	eaffff62 	b	fad8 <sata_vs_read+0x420>
    fd4c:	e59d3058 	ldr	r3, [sp, #88]
    fd50:	e59d0014 	ldr	r0, [sp, #20]
    fd54:	e0654003 	rsb	r4, r5, r3
    fd58:	ebffc6cc 	bl	1890 <free>
    fd5c:	e1a00004 	mov	r0, r4
    fd60:	e28dd034 	add	sp, sp, #52	; 0x34
    fd64:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
    fd68:	00018927 	.word	0x00018927
    fd6c:	3c00d6cc 	.word	0x3c00d6cc
    fd70:	00018904 	.word	0x00018904
    fd74:	28200000 	.word	0x28200000
    fd78:	007fffff 	.word	0x007fffff
    fd7c:	003fffff 	.word	0x003fffff
    fd80:	003ffdff 	.word	0x003ffdff

0000fd84 <sata_vs_reset>:
    fd84:	e92d4070 	push	{r4, r5, r6, lr}
    fd88:	e59f02d4 	ldr	r0, [pc, #724]	; 10064 <sata_vs_reset+0x2e0>
    fd8c:	ebffc27c 	bl	784 <module_enable>
    fd90:	e59f32d0 	ldr	r3, [pc, #720]	; 10068 <sata_vs_reset+0x2e4>
    fd94:	e3a01000 	mov	r1, #0	; 0x0
    fd98:	e3a0c001 	mov	ip, #1	; 0x1
    fd9c:	e3a06010 	mov	r6, #16	; 0x10
    fda0:	e3a02006 	mov	r2, #6	; 0x6
    fda4:	e583c830 	str	ip, [r3, #2096]
    fda8:	e3a04002 	mov	r4, #2	; 0x2
    fdac:	e5836820 	str	r6, [r3, #2080]
    fdb0:	e5831824 	str	r1, [r3, #2084]
    fdb4:	e5832828 	str	r2, [r3, #2088]
    fdb8:	e282200c 	add	r2, r2, #12	; 0xc
    fdbc:	e583482c 	str	r4, [r3, #2092]
    fdc0:	e5834834 	str	r4, [r3, #2100]
    fdc4:	e5831838 	str	r1, [r3, #2104]
    fdc8:	e583183c 	str	r1, [r3, #2108]
    fdcc:	e5831840 	str	r1, [r3, #2112]
    fdd0:	e5832844 	str	r2, [r3, #2116]
    fdd4:	e3a02009 	mov	r2, #9	; 0x9
    fdd8:	e583c848 	str	ip, [r3, #2120]
    fddc:	e3a05003 	mov	r5, #3	; 0x3
    fde0:	e583184c 	str	r1, [r3, #2124]
    fde4:	e5832850 	str	r2, [r3, #2128]
    fde8:	e082200c 	add	r2, r2, ip
    fdec:	e5831854 	str	r1, [r3, #2132]
    fdf0:	e3040e20 	movw	r0, #20000	; 0x4e20
    fdf4:	e5832858 	str	r2, [r3, #2136]
    fdf8:	e583585c 	str	r5, [r3, #2140]
    fdfc:	e583c860 	str	ip, [r3, #2144]
    fe00:	e5831864 	str	r1, [r3, #2148]
    fe04:	e5831868 	str	r1, [r3, #2152]
    fe08:	e583486c 	str	r4, [r3, #2156]
    fe0c:	e583c870 	str	ip, [r3, #2160]
    fe10:	e5835874 	str	r5, [r3, #2164]
    fe14:	e5836878 	str	r6, [r3, #2168]
    fe18:	e5831830 	str	r1, [r3, #2096]
    fe1c:	ebfff6f0 	bl	d9e4 <udelay>
    fe20:	e59f2244 	ldr	r2, [pc, #580]	; 1006c <sata_vs_reset+0x2e8>
    fe24:	e5923004 	ldr	r3, [r2, #4]
    fe28:	e1a01002 	mov	r1, r2
    fe2c:	e3833001 	orr	r3, r3, #1	; 0x1
    fe30:	e5823004 	str	r3, [r2, #4]
    fe34:	e5913004 	ldr	r3, [r1, #4]
    fe38:	e59f222c 	ldr	r2, [pc, #556]	; 1006c <sata_vs_reset+0x2e8>
    fe3c:	e3130001 	tst	r3, #1	; 0x1
    fe40:	1afffffb 	bne	fe34 <sata_vs_reset+0xb0>
    fe44:	e5923000 	ldr	r3, [r2]
    fe48:	e3a00b03 	mov	r0, #3072	; 0xc00
    fe4c:	e3833306 	orr	r3, r3, #402653184	; 0x18000000
    fe50:	e5823000 	str	r3, [r2]
    fe54:	e3a03001 	mov	r3, #1	; 0x1
    fe58:	e582300c 	str	r3, [r2, #12]
    fe5c:	e5923118 	ldr	r3, [r2, #280]
    fe60:	e3833707 	orr	r3, r3, #1835008	; 0x1c0000
    fe64:	e5823118 	str	r3, [r2, #280]
    fe68:	ebffc620 	bl	16f0 <malloc>
    fe6c:	e2505000 	subs	r5, r0, #0	; 0x0
    fe70:	1a000004 	bne	fe88 <sata_vs_reset+0x104>
    fe74:	e59f01f4 	ldr	r0, [pc, #500]	; 10070 <sata_vs_reset+0x2ec>
    fe78:	e59f31f4 	ldr	r3, [pc, #500]	; 10074 <sata_vs_reset+0x2f0>
    fe7c:	e1a0e00f 	mov	lr, pc
    fe80:	e593f000 	ldr	pc, [r3]
    fe84:	ea000041 	b	ff90 <sata_vs_reset+0x20c>
    fe88:	e2853fff 	add	r3, r5, #1020	; 0x3fc
    fe8c:	e2833003 	add	r3, r3, #3	; 0x3
    fe90:	e59f11d4 	ldr	r1, [pc, #468]	; 1006c <sata_vs_reset+0x2e8>
    fe94:	e3c30fff 	bic	r0, r3, #1020	; 0x3fc
    fe98:	e3c00003 	bic	r0, r0, #3	; 0x3
    fe9c:	e5913000 	ldr	r3, [r1]
    fea0:	e1500005 	cmp	r0, r5
    fea4:	02800a01 	addeq	r0, r0, #4096	; 0x1000
    fea8:	e3130302 	tst	r3, #134217728	; 0x8000000
    feac:	0a00002e 	beq	ff6c <sata_vs_reset+0x1e8>
    feb0:	e5912118 	ldr	r2, [r1, #280]
    feb4:	e30c3011 	movw	r3, #49169	; 0xc011
    feb8:	e0023003 	and	r3, r2, r3
    febc:	e3530000 	cmp	r3, #0	; 0x0
    fec0:	0a00000e 	beq	ff00 <sata_vs_reset+0x17c>
    fec4:	e5913118 	ldr	r3, [r1, #280]
    fec8:	e1a0c001 	mov	ip, r1
    fecc:	e3c33001 	bic	r3, r3, #1	; 0x1
    fed0:	e5813118 	str	r3, [r1, #280]
    fed4:	e59c3118 	ldr	r3, [ip, #280]
    fed8:	e59f218c 	ldr	r2, [pc, #396]	; 1006c <sata_vs_reset+0x2e8>
    fedc:	e3130902 	tst	r3, #32768	; 0x8000
    fee0:	1afffffb 	bne	fed4 <sata_vs_reset+0x150>
    fee4:	e5923118 	ldr	r3, [r2, #280]
    fee8:	e1a01002 	mov	r1, r2
    feec:	e3c33010 	bic	r3, r3, #16	; 0x10
    fef0:	e5823118 	str	r3, [r2, #280]
    fef4:	e5913118 	ldr	r3, [r1, #280]
    fef8:	e3130901 	tst	r3, #16384	; 0x4000
    fefc:	1afffffc 	bne	fef4 <sata_vs_reset+0x170>
    ff00:	e59f2164 	ldr	r2, [pc, #356]	; 1006c <sata_vs_reset+0x2e8>
    ff04:	e280eb01 	add	lr, r0, #1024	; 0x400
    ff08:	e592312c 	ldr	r3, [r2, #300]
    ff0c:	e1a01002 	mov	r1, r2
    ff10:	e3c3300f 	bic	r3, r3, #15	; 0xf
    ff14:	e582312c 	str	r3, [r2, #300]
    ff18:	e5820100 	str	r0, [r2, #256]
    ff1c:	e582e108 	str	lr, [r2, #264]
    ff20:	e5923118 	ldr	r3, [r2, #280]
    ff24:	e3833010 	orr	r3, r3, #16	; 0x10
    ff28:	e5823118 	str	r3, [r2, #280]
    ff2c:	e5923118 	ldr	r3, [r2, #280]
    ff30:	e3833002 	orr	r3, r3, #2	; 0x2
    ff34:	e5823118 	str	r3, [r2, #280]
    ff38:	e5913128 	ldr	r3, [r1, #296]
    ff3c:	e59f2128 	ldr	r2, [pc, #296]	; 1006c <sata_vs_reset+0x2e8>
    ff40:	e203300f 	and	r3, r3, #15	; 0xf
    ff44:	e3530003 	cmp	r3, #3	; 0x3
    ff48:	13530001 	cmpne	r3, #1	; 0x1
    ff4c:	1afffff9 	bne	ff38 <sata_vs_reset+0x1b4>
    ff50:	e3e03000 	mvn	r3, #0	; 0x0
    ff54:	e5823130 	str	r3, [r2, #304]
    ff58:	e5923120 	ldr	r3, [r2, #288]
    ff5c:	e59f1108 	ldr	r1, [pc, #264]	; 1006c <sata_vs_reset+0x2e8>
    ff60:	e3130089 	tst	r3, #137	; 0x89
    ff64:	1afffffb 	bne	ff58 <sata_vs_reset+0x1d4>
    ff68:	ea00000a 	b	ff98 <sata_vs_reset+0x214>
    ff6c:	e59f4100 	ldr	r4, [pc, #256]	; 10074 <sata_vs_reset+0x2f0>
    ff70:	e59f0100 	ldr	r0, [pc, #256]	; 10078 <sata_vs_reset+0x2f4>
    ff74:	e1a0e00f 	mov	lr, pc
    ff78:	e594f000 	ldr	pc, [r4]
    ff7c:	e59f00f8 	ldr	r0, [pc, #248]	; 1007c <sata_vs_reset+0x2f8>
    ff80:	e1a0e00f 	mov	lr, pc
    ff84:	e594f000 	ldr	pc, [r4]
    ff88:	e1a00005 	mov	r0, r5
    ff8c:	ebffc63f 	bl	1890 <free>
    ff90:	e3e00000 	mvn	r0, #0	; 0x0
    ff94:	e8bd8070 	pop	{r4, r5, r6, pc}
    ff98:	e5912118 	ldr	r2, [r1, #280]
    ff9c:	e30c3011 	movw	r3, #49169	; 0xc011
    ffa0:	e0023003 	and	r3, r2, r3
    ffa4:	e3530000 	cmp	r3, #0	; 0x0
    ffa8:	0a00000e 	beq	ffe8 <sata_vs_reset+0x264>
    ffac:	e5913118 	ldr	r3, [r1, #280]
    ffb0:	e1a0c001 	mov	ip, r1
    ffb4:	e3c33001 	bic	r3, r3, #1	; 0x1
    ffb8:	e5813118 	str	r3, [r1, #280]
    ffbc:	e59c3118 	ldr	r3, [ip, #280]
    ffc0:	e59f20a4 	ldr	r2, [pc, #164]	; 1006c <sata_vs_reset+0x2e8>
    ffc4:	e3130902 	tst	r3, #32768	; 0x8000
    ffc8:	1afffffb 	bne	ffbc <sata_vs_reset+0x238>
    ffcc:	e5923118 	ldr	r3, [r2, #280]
    ffd0:	e1a01002 	mov	r1, r2
    ffd4:	e3c33010 	bic	r3, r3, #16	; 0x10
    ffd8:	e5823118 	str	r3, [r2, #280]
    ffdc:	e5913118 	ldr	r3, [r1, #280]
    ffe0:	e3130901 	tst	r3, #16384	; 0x4000
    ffe4:	1afffffc 	bne	ffdc <sata_vs_reset+0x258>
    ffe8:	e59f207c 	ldr	r2, [pc, #124]	; 1006c <sata_vs_reset+0x2e8>
    ffec:	e3e01000 	mvn	r1, #0	; 0x0
    fff0:	e5820100 	str	r0, [r2, #256]
    fff4:	e582e108 	str	lr, [r2, #264]
    fff8:	e5923118 	ldr	r3, [r2, #280]
    fffc:	e3833010 	orr	r3, r3, #16	; 0x10
   10000:	e5823118 	str	r3, [r2, #280]
   10004:	e3e03502 	mvn	r3, #8388608	; 0x800000
   10008:	e5821130 	str	r1, [r2, #304]
   1000c:	e5821110 	str	r1, [r2, #272]
   10010:	e5821008 	str	r1, [r2, #8]
   10014:	e5823114 	str	r3, [r2, #276]
   10018:	e5923004 	ldr	r3, [r2, #4]
   1001c:	e1a01002 	mov	r1, r2
   10020:	e3833002 	orr	r3, r3, #2	; 0x2
   10024:	e5823004 	str	r3, [r2, #4]
   10028:	e5913128 	ldr	r3, [r1, #296]
   1002c:	e5912120 	ldr	r2, [r1, #288]
   10030:	e203300f 	and	r3, r3, #15	; 0xf
   10034:	e3530003 	cmp	r3, #3	; 0x3
   10038:	1afffffa 	bne	10028 <sata_vs_reset+0x2a4>
   1003c:	e2124089 	ands	r4, r2, #137	; 0x89
   10040:	1afffff8 	bne	10028 <sata_vs_reset+0x2a4>
   10044:	e1a00005 	mov	r0, r5
   10048:	ebffc610 	bl	1890 <free>
   1004c:	e59f002c 	ldr	r0, [pc, #44]	; 10080 <sata_vs_reset+0x2fc>
   10050:	e59f301c 	ldr	r3, [pc, #28]	; 10074 <sata_vs_reset+0x2f0>
   10054:	e1a0e00f 	mov	lr, pc
   10058:	e593f000 	ldr	pc, [r3]
   1005c:	e1a00004 	mov	r0, r4
   10060:	e8bd8070 	pop	{r4, r5, r6, pc}
   10064:	21e3c800 	.word	0x21e3c800
   10068:	21e3c000 	.word	0x21e3c000
   1006c:	28200000 	.word	0x28200000
   10070:	00018949 	.word	0x00018949
   10074:	3c00d6cc 	.word	0x3c00d6cc
   10078:	0001896a 	.word	0x0001896a
   1007c:	0001898d 	.word	0x0001898d
   10080:	000189a3 	.word	0x000189a3

00010084 <init_led>:
   10084:	e3a0c000 	mov	ip, #0	; 0x0
   10088:	e59f3010 	ldr	r3, [pc, #16]	; 100a0 <init_led+0x1c>
   1008c:	e59f0010 	ldr	r0, [pc, #16]	; 100a4 <init_led+0x20>
   10090:	e1a0200c 	mov	r2, ip
   10094:	e3a01002 	mov	r1, #2	; 0x2
   10098:	e583c000 	str	ip, [r3]
   1009c:	eaffc2fc 	b	c94 <pads_chmod>
   100a0:	3c00d880 	.word	0x3c00d880
   100a4:	002e002d 	.word	0x002e002d

000100a8 <led_off>:
   100a8:	e3a02000 	mov	r2, #0	; 0x0
   100ac:	e59f300c 	ldr	r3, [pc, #12]	; 100c0 <led_off+0x18>
   100b0:	e1a01002 	mov	r1, r2
   100b4:	e3a0002e 	mov	r0, #46	; 0x2e
   100b8:	e5832000 	str	r2, [r3]
   100bc:	eaffc384 	b	ed4 <pads_setpin>
   100c0:	3c00d880 	.word	0x3c00d880

000100c4 <led_on>:
   100c4:	e3a02001 	mov	r2, #1	; 0x1
   100c8:	e59f300c 	ldr	r3, [pc, #12]	; 100dc <led_on+0x18>
   100cc:	e1a01002 	mov	r1, r2
   100d0:	e3a0002e 	mov	r0, #46	; 0x2e
   100d4:	e5832000 	str	r2, [r3]
   100d8:	eaffc37d 	b	ed4 <pads_setpin>
   100dc:	3c00d880 	.word	0x3c00d880

000100e0 <led_pulse>:
   100e0:	e92d4010 	push	{r4, lr}
   100e4:	e1a04000 	mov	r4, r0
   100e8:	ebfffff5 	bl	100c4 <led_on>
   100ec:	e3a00005 	mov	r0, #5	; 0x5
   100f0:	ea000001 	b	100fc <led_pulse+0x1c>
   100f4:	ebfffff2 	bl	100c4 <led_on>
   100f8:	e3a00002 	mov	r0, #2	; 0x2
   100fc:	ebfff638 	bl	d9e4 <udelay>
   10100:	ebffffe8 	bl	100a8 <led_off>
   10104:	e3a00002 	mov	r0, #2	; 0x2
   10108:	ebfff635 	bl	d9e4 <udelay>
   1010c:	e3540000 	cmp	r4, #0	; 0x0
   10110:	e2444001 	sub	r4, r4, #1	; 0x1
   10114:	cafffff6 	bgt	100f4 <led_pulse+0x14>
   10118:	e8bd8010 	pop	{r4, pc}

0001011c <led_ctrl>:
   1011c:	e3500000 	cmp	r0, #0	; 0x0
   10120:	0a000000 	beq	10128 <led_ctrl+0xc>
   10124:	eaffffe6 	b	100c4 <led_on>
   10128:	eaffffde 	b	100a8 <led_off>

0001012c <led_toggle>:
   1012c:	e59f300c 	ldr	r3, [pc, #12]	; 10140 <led_toggle+0x14>
   10130:	e5930000 	ldr	r0, [r3]
   10134:	e2700001 	rsbs	r0, r0, #1	; 0x1
   10138:	33a00000 	movcc	r0, #0	; 0x0
   1013c:	eafffff6 	b	1011c <led_ctrl>
   10140:	3c00d880 	.word	0x3c00d880

00010144 <udc_get_cable_state>:
   10144:	e3a00001 	mov	r0, #1	; 0x1
   10148:	e12fff1e 	bx	lr

0001014c <udc_disconnect>:
   1014c:	e59f103c 	ldr	r1, [pc, #60]	; 10190 <udc_disconnect+0x44>
   10150:	e59f303c 	ldr	r3, [pc, #60]	; 10194 <udc_disconnect+0x48>
   10154:	e5912010 	ldr	r2, [r1, #16]
   10158:	e3a00000 	mov	r0, #0	; 0x0
   1015c:	e3822001 	orr	r2, r2, #1	; 0x1
   10160:	e92d4010 	push	{r4, lr}
   10164:	e5c30000 	strb	r0, [r3]
   10168:	e1a04001 	mov	r4, r1
   1016c:	e5812010 	str	r2, [r1, #16]
   10170:	ea000000 	b	10178 <udc_disconnect+0x2c>
   10174:	ebfff61a 	bl	d9e4 <udelay>
   10178:	e5943010 	ldr	r3, [r4, #16]
   1017c:	e3a0000a 	mov	r0, #10	; 0xa
   10180:	e2133001 	ands	r3, r3, #1	; 0x1
   10184:	1afffffa 	bne	10174 <udc_disconnect+0x28>
   10188:	e1a00003 	mov	r0, r3
   1018c:	e8bd8010 	pop	{r4, pc}
   10190:	27100000 	.word	0x27100000
   10194:	3c00d224 	.word	0x3c00d224

00010198 <udc_event_handle>:
   10198:	e59f2768 	ldr	r2, [pc, #1896]	; 10908 <udc_event_handle+0x770>
   1019c:	e59f3768 	ldr	r3, [pc, #1896]	; 1090c <udc_event_handle+0x774>
   101a0:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
   101a4:	e5934014 	ldr	r4, [r3, #20]
   101a8:	e5923000 	ldr	r3, [r2]
   101ac:	e59f0758 	ldr	r0, [pc, #1880]	; 1090c <udc_event_handle+0x774>
   101b0:	e1530004 	cmp	r3, r4
   101b4:	e204ca03 	and	ip, r4, #12288	; 0x3000
   101b8:	15824000 	strne	r4, [r2]
   101bc:	e2043b03 	and	r3, r4, #3072	; 0xc00
   101c0:	e35c0a01 	cmp	ip, #4096	; 0x1000
   101c4:	e24dd008 	sub	sp, sp, #8	; 0x8
   101c8:	e5803014 	str	r3, [r0, #20]
   101cc:	0a000002 	beq	101dc <udc_event_handle+0x44>
   101d0:	e35c0a02 	cmp	ip, #8192	; 0x2000
   101d4:	1a000034 	bne	102ac <udc_event_handle+0x114>
   101d8:	ea000025 	b	10274 <udc_event_handle+0xdc>
   101dc:	e3a03302 	mov	r3, #134217728	; 0x8000000
   101e0:	e5803b00 	str	r3, [r0, #2816]
   101e4:	e5803b40 	str	r3, [r0, #2880]
   101e8:	e59f3720 	ldr	r3, [pc, #1824]	; 10910 <udc_event_handle+0x778>
   101ec:	e3a02009 	mov	r2, #9	; 0x9
   101f0:	e580381c 	str	r3, [r0, #2076]
   101f4:	e3a03b02 	mov	r3, #2048	; 0x800
   101f8:	e5802814 	str	r2, [r0, #2068]
   101fc:	e5802810 	str	r2, [r0, #2064]
   10200:	e5803024 	str	r3, [r0, #36]
   10204:	e2833301 	add	r3, r3, #67108864	; 0x4000000
   10208:	e5803028 	str	r3, [r0, #40]
   1020c:	e2833b01 	add	r3, r3, #1024	; 0x400
   10210:	e5803104 	str	r3, [r0, #260]
   10214:	e59f36f8 	ldr	r3, [pc, #1784]	; 10914 <udc_event_handle+0x77c>
   10218:	e1a05000 	mov	r5, r0
   1021c:	e580305c 	str	r3, [r0, #92]
   10220:	e5903010 	ldr	r3, [r0, #16]
   10224:	e3c33d1f 	bic	r3, r3, #1984	; 0x7c0
   10228:	e3833e43 	orr	r3, r3, #1072	; 0x430
   1022c:	e5803010 	str	r3, [r0, #16]
   10230:	ea000000 	b	10238 <udc_event_handle+0xa0>
   10234:	ebfff5ea 	bl	d9e4 <udelay>
   10238:	e5953010 	ldr	r3, [r5, #16]
   1023c:	e3a00001 	mov	r0, #1	; 0x1
   10240:	e3130030 	tst	r3, #48	; 0x30
   10244:	e59f16c0 	ldr	r1, [pc, #1728]	; 1090c <udc_event_handle+0x774>
   10248:	1afffff9 	bne	10234 <udc_event_handle+0x9c>
   1024c:	e59f36c4 	ldr	r3, [pc, #1732]	; 10918 <udc_event_handle+0x780>
   10250:	e2800009 	add	r0, r0, #9	; 0x9
   10254:	e5932024 	ldr	r2, [r3, #36]
   10258:	e59f36bc 	ldr	r3, [pc, #1724]	; 1091c <udc_event_handle+0x784>
   1025c:	e5813b10 	str	r3, [r1, #2832]
   10260:	e3a03a01 	mov	r3, #4096	; 0x1000
   10264:	e5812b14 	str	r2, [r1, #2836]
   10268:	e5813014 	str	r3, [r1, #20]
   1026c:	ebfff5dc 	bl	d9e4 <udelay>
   10270:	ea00000d 	b	102ac <udc_event_handle+0x114>
   10274:	e5902808 	ldr	r2, [r0, #2056]
   10278:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   1027c:	e5803b00 	str	r3, [r0, #2816]
   10280:	e1a020a2 	lsr	r2, r2, #1
   10284:	e59f368c 	ldr	r3, [pc, #1676]	; 10918 <udc_event_handle+0x780>
   10288:	e5901018 	ldr	r1, [r0, #24]
   1028c:	e2022003 	and	r2, r2, #3	; 0x3
   10290:	e5c3201c 	strb	r2, [r3, #28]
   10294:	e59f3684 	ldr	r3, [pc, #1668]	; 10920 <udc_event_handle+0x788>
   10298:	e3811008 	orr	r1, r1, #8	; 0x8
   1029c:	e3a02001 	mov	r2, #1	; 0x1
   102a0:	e5801018 	str	r1, [r0, #24]
   102a4:	e5832000 	str	r2, [r3]
   102a8:	e580c014 	str	ip, [r0, #20]
   102ac:	e59f3658 	ldr	r3, [pc, #1624]	; 1090c <udc_event_handle+0x774>
   102b0:	e3140701 	tst	r4, #262144	; 0x40000
   102b4:	e5939818 	ldr	r9, [r3, #2072]
   102b8:	0a000051 	beq	10404 <udc_event_handle+0x26c>
   102bc:	e3190001 	tst	r9, #1	; 0x1
   102c0:	0a000042 	beq	103d0 <udc_event_handle+0x238>
   102c4:	e5935908 	ldr	r5, [r3, #2312]
   102c8:	e3150001 	tst	r5, #1	; 0x1
   102cc:	0a00003b 	beq	103c0 <udc_event_handle+0x228>
   102d0:	e59f3640 	ldr	r3, [pc, #1600]	; 10918 <udc_event_handle+0x780>
   102d4:	e5933020 	ldr	r3, [r3, #32]
   102d8:	e2433002 	sub	r3, r3, #2	; 0x2
   102dc:	e3530003 	cmp	r3, #3	; 0x3
   102e0:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   102e4:	ea000035 	b	103c0 <udc_event_handle+0x228>
   102e8:	00010328 	.word	0x00010328
   102ec:	000102f8 	.word	0x000102f8
   102f0:	0001035c 	.word	0x0001035c
   102f4:	0001035c 	.word	0x0001035c
   102f8:	e59f160c 	ldr	r1, [pc, #1548]	; 1090c <udc_event_handle+0x774>
   102fc:	e59f0614 	ldr	r0, [pc, #1556]	; 10918 <udc_event_handle+0x780>
   10300:	e5912910 	ldr	r2, [r1, #2320]
   10304:	e590c00c 	ldr	ip, [r0, #12]
   10308:	e3a03004 	mov	r3, #4	; 0x4
   1030c:	e3822702 	orr	r2, r2, #524288	; 0x80000
   10310:	e5803020 	str	r3, [r0, #32]
   10314:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   10318:	e5812910 	str	r2, [r1, #2320]
   1031c:	e581c914 	str	ip, [r1, #2324]
   10320:	e5813900 	str	r3, [r1, #2304]
   10324:	ea000025 	b	103c0 <udc_event_handle+0x228>
   10328:	e59f15dc 	ldr	r1, [pc, #1500]	; 1090c <udc_event_handle+0x774>
   1032c:	e59f05e4 	ldr	r0, [pc, #1508]	; 10918 <udc_event_handle+0x780>
   10330:	e5913b10 	ldr	r3, [r1, #2832]
   10334:	e590c024 	ldr	ip, [r0, #36]
   10338:	e3c3307f 	bic	r3, r3, #127	; 0x7f
   1033c:	e3833702 	orr	r3, r3, #524288	; 0x80000
   10340:	e5813b10 	str	r3, [r1, #2832]
   10344:	e3a02005 	mov	r2, #5	; 0x5
   10348:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   1034c:	e581cb14 	str	ip, [r1, #2836]
   10350:	e5802020 	str	r2, [r0, #32]
   10354:	e5813b00 	str	r3, [r1, #2816]
   10358:	ea000018 	b	103c0 <udc_event_handle+0x228>
   1035c:	e59f05b4 	ldr	r0, [pc, #1460]	; 10918 <udc_event_handle+0x780>
   10360:	e5d03015 	ldrb	r3, [r0, #21]
   10364:	e3530009 	cmp	r3, #9	; 0x9
   10368:	1a00000a 	bne	10398 <udc_event_handle+0x200>
   1036c:	e59f1598 	ldr	r1, [pc, #1432]	; 1090c <udc_event_handle+0x774>
   10370:	e59f35ac 	ldr	r3, [pc, #1452]	; 10924 <udc_event_handle+0x78c>
   10374:	e3a02001 	mov	r2, #1	; 0x1
   10378:	e5813920 	str	r3, [r1, #2336]
   1037c:	e2433d07 	sub	r3, r3, #448	; 0x1c0
   10380:	e5813b40 	str	r3, [r1, #2880]
   10384:	e591381c 	ldr	r3, [r1, #2076]
   10388:	e5c02000 	strb	r2, [r0]
   1038c:	e3833701 	orr	r3, r3, #262144	; 0x40000
   10390:	e3833002 	orr	r3, r3, #2	; 0x2
   10394:	e581381c 	str	r3, [r1, #2076]
   10398:	e59f256c 	ldr	r2, [pc, #1388]	; 1090c <udc_event_handle+0x774>
   1039c:	e59f1574 	ldr	r1, [pc, #1396]	; 10918 <udc_event_handle+0x780>
   103a0:	e59f3574 	ldr	r3, [pc, #1396]	; 1091c <udc_event_handle+0x784>
   103a4:	e5910024 	ldr	r0, [r1, #36]
   103a8:	e5823b10 	str	r3, [r2, #2832]
   103ac:	e3a03001 	mov	r3, #1	; 0x1
   103b0:	e5813020 	str	r3, [r1, #32]
   103b4:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   103b8:	e5820b14 	str	r0, [r2, #2836]
   103bc:	e5823b00 	str	r3, [r2, #2816]
   103c0:	e300320d 	movw	r3, #525	; 0x20d
   103c4:	e59f2540 	ldr	r2, [pc, #1344]	; 1090c <udc_event_handle+0x774>
   103c8:	e0053003 	and	r3, r5, r3
   103cc:	e5823908 	str	r3, [r2, #2312]
   103d0:	e3190002 	tst	r9, #2	; 0x2
   103d4:	0a00000a 	beq	10404 <udc_event_handle+0x26c>
   103d8:	e59f352c 	ldr	r3, [pc, #1324]	; 1090c <udc_event_handle+0x774>
   103dc:	e5932928 	ldr	r2, [r3, #2344]
   103e0:	e3120001 	tst	r2, #1	; 0x1
   103e4:	0a000003 	beq	103f8 <udc_event_handle+0x260>
   103e8:	e5933930 	ldr	r3, [r3, #2352]
   103ec:	e3d3120f 	bics	r1, r3, #-268435456	; 0xf0000000
   103f0:	059f3520 	ldreq	r3, [pc, #1312]	; 10918 <udc_event_handle+0x780>
   103f4:	05c31002 	strbeq	r1, [r3, #2]
   103f8:	e59f350c 	ldr	r3, [pc, #1292]	; 1090c <udc_event_handle+0x774>
   103fc:	e2022005 	and	r2, r2, #5	; 0x5
   10400:	e5832928 	str	r2, [r3, #2344]
   10404:	e3140702 	tst	r4, #524288	; 0x80000
   10408:	0a00013c 	beq	10900 <udc_event_handle+0x768>
   1040c:	e3190801 	tst	r9, #65536	; 0x10000
   10410:	0a000124 	beq	108a8 <udc_event_handle+0x710>
   10414:	e59f34f0 	ldr	r3, [pc, #1264]	; 1090c <udc_event_handle+0x774>
   10418:	e593ab08 	ldr	sl, [r3, #2824]
   1041c:	e31a0001 	tst	sl, #1	; 0x1
   10420:	0a00002b 	beq	104d4 <udc_event_handle+0x33c>
   10424:	e59f34ec 	ldr	r3, [pc, #1260]	; 10918 <udc_event_handle+0x780>
   10428:	e5933020 	ldr	r3, [r3, #32]
   1042c:	e2433002 	sub	r3, r3, #2	; 0x2
   10430:	e3530003 	cmp	r3, #3	; 0x3
   10434:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   10438:	ea000025 	b	104d4 <udc_event_handle+0x33c>
   1043c:	00010478 	.word	0x00010478
   10440:	0001044c 	.word	0x0001044c
   10444:	000104ac 	.word	0x000104ac
   10448:	000104ac 	.word	0x000104ac
   1044c:	e59f14c4 	ldr	r1, [pc, #1220]	; 10918 <udc_event_handle+0x780>
   10450:	e59f24b4 	ldr	r2, [pc, #1204]	; 1090c <udc_event_handle+0x774>
   10454:	e3a03004 	mov	r3, #4	; 0x4
   10458:	e591000c 	ldr	r0, [r1, #12]
   1045c:	e5813020 	str	r3, [r1, #32]
   10460:	e3a03702 	mov	r3, #524288	; 0x80000
   10464:	e5823910 	str	r3, [r2, #2320]
   10468:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   1046c:	e5820914 	str	r0, [r2, #2324]
   10470:	e5823900 	str	r3, [r2, #2304]
   10474:	ea000016 	b	104d4 <udc_event_handle+0x33c>
   10478:	e59f148c 	ldr	r1, [pc, #1164]	; 1090c <udc_event_handle+0x774>
   1047c:	e59f0494 	ldr	r0, [pc, #1172]	; 10918 <udc_event_handle+0x780>
   10480:	e5913b10 	ldr	r3, [r1, #2832]
   10484:	e590c00c 	ldr	ip, [r0, #12]
   10488:	e3c3307f 	bic	r3, r3, #127	; 0x7f
   1048c:	e3833702 	orr	r3, r3, #524288	; 0x80000
   10490:	e5813b10 	str	r3, [r1, #2832]
   10494:	e3a02005 	mov	r2, #5	; 0x5
   10498:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   1049c:	e581cb14 	str	ip, [r1, #2836]
   104a0:	e5802020 	str	r2, [r0, #32]
   104a4:	e5813b00 	str	r3, [r1, #2816]
   104a8:	ea000009 	b	104d4 <udc_event_handle+0x33c>
   104ac:	e59f2458 	ldr	r2, [pc, #1112]	; 1090c <udc_event_handle+0x774>
   104b0:	e59f1460 	ldr	r1, [pc, #1120]	; 10918 <udc_event_handle+0x780>
   104b4:	e59f3460 	ldr	r3, [pc, #1120]	; 1091c <udc_event_handle+0x784>
   104b8:	e5910024 	ldr	r0, [r1, #36]
   104bc:	e5823b10 	str	r3, [r2, #2832]
   104c0:	e3a03001 	mov	r3, #1	; 0x1
   104c4:	e5813020 	str	r3, [r1, #32]
   104c8:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   104cc:	e5820b14 	str	r0, [r2, #2836]
   104d0:	e5823b00 	str	r3, [r2, #2816]
   104d4:	e31a0004 	tst	sl, #4	; 0x4
   104d8:	159f342c 	ldrne	r3, [pc, #1068]	; 1090c <udc_event_handle+0x774>
   104dc:	13a02004 	movne	r2, #4	; 0x4
   104e0:	15832b08 	strne	r2, [r3, #2824]
   104e4:	e31a0008 	tst	sl, #8	; 0x8
   104e8:	0a0000ea 	beq	10898 <udc_event_handle+0x700>
   104ec:	e31a0040 	tst	sl, #64	; 0x40
   104f0:	e59f1430 	ldr	r1, [pc, #1072]	; 10928 <udc_event_handle+0x790>
   104f4:	0a000006 	beq	10514 <udc_event_handle+0x37c>
   104f8:	e59f340c 	ldr	r3, [pc, #1036]	; 1090c <udc_event_handle+0x774>
   104fc:	e5933b14 	ldr	r3, [r3, #2836]
   10500:	e2432004 	sub	r2, r3, #4	; 0x4
   10504:	e2433008 	sub	r3, r3, #8	; 0x8
   10508:	e5812004 	str	r2, [r1, #4]
   1050c:	e5813000 	str	r3, [r1]
   10510:	ea000005 	b	1052c <udc_event_handle+0x394>
   10514:	e59f33fc 	ldr	r3, [pc, #1020]	; 10918 <udc_event_handle+0x780>
   10518:	e5932024 	ldr	r2, [r3, #36]
   1051c:	e5923000 	ldr	r3, [r2]
   10520:	e5813000 	str	r3, [r1]
   10524:	e5923004 	ldr	r3, [r2, #4]
   10528:	e5813004 	str	r3, [r1, #4]
   1052c:	e59f23e4 	ldr	r2, [pc, #996]	; 10918 <udc_event_handle+0x780>
   10530:	e5d20014 	ldrb	r0, [r2, #20]
   10534:	e3100080 	tst	r0, #128	; 0x80
   10538:	13a03002 	movne	r3, #2	; 0x2
   1053c:	03a03003 	moveq	r3, #3	; 0x3
   10540:	e5823020 	str	r3, [r2, #32]
   10544:	e59f23cc 	ldr	r2, [pc, #972]	; 10918 <udc_event_handle+0x780>
   10548:	e1d211ba 	ldrh	r1, [r2, #26]
   1054c:	e3510000 	cmp	r1, #0	; 0x0
   10550:	03a03004 	moveq	r3, #4	; 0x4
   10554:	05823020 	streq	r3, [r2, #32]
   10558:	e59f33b8 	ldr	r3, [pc, #952]	; 10918 <udc_event_handle+0x780>
   1055c:	e5d33015 	ldrb	r3, [r3, #21]
   10560:	e3530009 	cmp	r3, #9	; 0x9
   10564:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   10568:	ea0000ca 	b	10898 <udc_event_handle+0x700>
   1056c:	00010830 	.word	0x00010830
   10570:	00010898 	.word	0x00010898
   10574:	00010898 	.word	0x00010898
   10578:	00010898 	.word	0x00010898
   1057c:	00010898 	.word	0x00010898
   10580:	0001077c 	.word	0x0001077c
   10584:	00010594 	.word	0x00010594
   10588:	00010898 	.word	0x00010898
   1058c:	00010898 	.word	0x00010898
   10590:	000107cc 	.word	0x000107cc
   10594:	e3a00040 	mov	r0, #64	; 0x40
   10598:	ebffc454 	bl	16f0 <malloc>
   1059c:	e2506000 	subs	r6, r0, #0	; 0x0
   105a0:	0a0000bc 	beq	10898 <udc_event_handle+0x700>
   105a4:	e59f536c 	ldr	r5, [pc, #876]	; 10918 <udc_event_handle+0x780>
   105a8:	e1d521b6 	ldrh	r2, [r5, #22]
   105ac:	e1a03422 	lsr	r3, r2, #8
   105b0:	e2433001 	sub	r3, r3, #1	; 0x1
   105b4:	e3530005 	cmp	r3, #5	; 0x5
   105b8:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   105bc:	ea000054 	b	10714 <udc_event_handle+0x57c>
   105c0:	000105d8 	.word	0x000105d8
   105c4:	000105f0 	.word	0x000105f0
   105c8:	00010648 	.word	0x00010648
   105cc:	00010714 	.word	0x00010714
   105d0:	00010714 	.word	0x00010714
   105d4:	0001061c 	.word	0x0001061c
   105d8:	e1a00006 	mov	r0, r6
   105dc:	e59f1348 	ldr	r1, [pc, #840]	; 1092c <udc_event_handle+0x794>
   105e0:	e3a02012 	mov	r2, #18	; 0x12
   105e4:	ebffc3dc 	bl	155c <memcpy>
   105e8:	e3a03012 	mov	r3, #18	; 0x12
   105ec:	ea000028 	b	10694 <udc_event_handle+0x4fc>
   105f0:	e59f3338 	ldr	r3, [pc, #824]	; 10930 <udc_event_handle+0x798>
   105f4:	e3a04020 	mov	r4, #32	; 0x20
   105f8:	e3a0c000 	mov	ip, #0	; 0x0
   105fc:	e1a01003 	mov	r1, r3
   10600:	e1a00006 	mov	r0, r6
   10604:	e1a02004 	mov	r2, r4
   10608:	e5c3c003 	strb	ip, [r3, #3]
   1060c:	e5c34002 	strb	r4, [r3, #2]
   10610:	ebffc3d1 	bl	155c <memcpy>
   10614:	e5854004 	str	r4, [r5, #4]
   10618:	ea000045 	b	10734 <udc_event_handle+0x59c>
   1061c:	e1d531ba 	ldrh	r3, [r5, #26]
   10620:	e59fc30c 	ldr	ip, [pc, #780]	; 10934 <udc_event_handle+0x79c>
   10624:	e353000a 	cmp	r3, #10	; 0xa
   10628:	23a0300a 	movcs	r3, #10	; 0xa
   1062c:	e5cc3000 	strb	r3, [ip]
   10630:	e1a0100c 	mov	r1, ip
   10634:	e1a00006 	mov	r0, r6
   10638:	e3a0200a 	mov	r2, #10	; 0xa
   1063c:	ebffc3c6 	bl	155c <memcpy>
   10640:	e3a0300a 	mov	r3, #10	; 0xa
   10644:	ea000012 	b	10694 <udc_event_handle+0x4fc>
   10648:	e21270ff 	ands	r7, r2, #255	; 0xff
   1064c:	13a03000 	movne	r3, #0	; 0x0
   10650:	03a03001 	moveq	r3, #1	; 0x1
   10654:	e3570005 	cmp	r7, #5	; 0x5
   10658:	d1a08003 	movle	r8, r3
   1065c:	c3838001 	orrgt	r8, r3, #1	; 0x1
   10660:	e3580000 	cmp	r8, #0	; 0x0
   10664:	e59f52ac 	ldr	r5, [pc, #684]	; 10918 <udc_event_handle+0x780>
   10668:	0a00000b 	beq	1069c <udc_event_handle+0x504>
   1066c:	e1d531ba 	ldrh	r3, [r5, #26]
   10670:	e59fc2c0 	ldr	ip, [pc, #704]	; 10938 <udc_event_handle+0x7a0>
   10674:	e3530004 	cmp	r3, #4	; 0x4
   10678:	23a03004 	movcs	r3, #4	; 0x4
   1067c:	e5cc3000 	strb	r3, [ip]
   10680:	e1a0100c 	mov	r1, ip
   10684:	e1a00006 	mov	r0, r6
   10688:	e3a02004 	mov	r2, #4	; 0x4
   1068c:	ebffc3b2 	bl	155c <memcpy>
   10690:	e3a03004 	mov	r3, #4	; 0x4
   10694:	e5853004 	str	r3, [r5, #4]
   10698:	ea000025 	b	10734 <udc_event_handle+0x59c>
   1069c:	e59f4298 	ldr	r4, [pc, #664]	; 1093c <udc_event_handle+0x7a4>
   106a0:	e7940107 	ldr	r0, [r4, r7, lsl #2]
   106a4:	ebffc392 	bl	14f4 <strlen>
   106a8:	e6ef0070 	uxtb	r0, r0
   106ac:	e2803001 	add	r3, r0, #1	; 0x1
   106b0:	e5d5201a 	ldrb	r2, [r5, #26]
   106b4:	e1a03083 	lsl	r3, r3, #1
   106b8:	e6ef3073 	uxtb	r3, r3
   106bc:	e1530002 	cmp	r3, r2
   106c0:	31a0e003 	movcc	lr, r3
   106c4:	21a0e002 	movcs	lr, r2
   106c8:	e3a03003 	mov	r3, #3	; 0x3
   106cc:	e5c63001 	strb	r3, [r6, #1]
   106d0:	e5c6e000 	strb	lr, [r6]
   106d4:	e1a05008 	mov	r5, r8
   106d8:	e3a02001 	mov	r2, #1	; 0x1
   106dc:	ea000005 	b	106f8 <udc_event_handle+0x560>
   106e0:	e7943107 	ldr	r3, [r4, r7, lsl #2]
   106e4:	e0833002 	add	r3, r3, r2
   106e8:	e5533001 	ldrb	r3, [r3, #-1]
   106ec:	e7c63082 	strb	r3, [r6, r2, lsl #1]
   106f0:	e5c15001 	strb	r5, [r1, #1]
   106f4:	e6ef207c 	uxtb	r2, ip
   106f8:	e1500002 	cmp	r0, r2
   106fc:	e282c001 	add	ip, r2, #1	; 0x1
   10700:	e0861082 	add	r1, r6, r2, lsl #1
   10704:	aafffff5 	bge	106e0 <udc_event_handle+0x548>
   10708:	e59f3208 	ldr	r3, [pc, #520]	; 10918 <udc_event_handle+0x780>
   1070c:	e583e004 	str	lr, [r3, #4]
   10710:	ea000007 	b	10734 <udc_event_handle+0x59c>
   10714:	e59f21f0 	ldr	r2, [pc, #496]	; 1090c <udc_event_handle+0x774>
   10718:	e3a03702 	mov	r3, #524288	; 0x80000
   1071c:	e5951024 	ldr	r1, [r5, #36]
   10720:	e5823910 	str	r3, [r2, #2320]
   10724:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   10728:	e5821914 	str	r1, [r2, #2324]
   1072c:	e5823900 	str	r3, [r2, #2304]
   10730:	ea00000e 	b	10770 <udc_event_handle+0x5d8>
   10734:	e59f11dc 	ldr	r1, [pc, #476]	; 10918 <udc_event_handle+0x780>
   10738:	e59f01cc 	ldr	r0, [pc, #460]	; 1090c <udc_event_handle+0x774>
   1073c:	e1d120b4 	ldrh	r2, [r1, #4]
   10740:	e1d131ba 	ldrh	r3, [r1, #26]
   10744:	e1520003 	cmp	r2, r3
   10748:	21a02003 	movcs	r2, r3
   1074c:	e3823702 	orr	r3, r2, #524288	; 0x80000
   10750:	e5803910 	str	r3, [r0, #2320]
   10754:	e3a03000 	mov	r3, #0	; 0x0
   10758:	e5c13001 	strb	r3, [r1, #1]
   1075c:	e2833321 	add	r3, r3, #-2080374784	; 0x84000000
   10760:	e5812004 	str	r2, [r1, #4]
   10764:	e5806914 	str	r6, [r0, #2324]
   10768:	e5803900 	str	r3, [r0, #2304]
   1076c:	e581600c 	str	r6, [r1, #12]
   10770:	e1a00006 	mov	r0, r6
   10774:	ebffc445 	bl	1890 <free>
   10778:	ea000046 	b	10898 <udc_event_handle+0x700>
   1077c:	e59fc1bc 	ldr	ip, [pc, #444]	; 10940 <udc_event_handle+0x7a8>
   10780:	e5dc3000 	ldrb	r3, [ip]
   10784:	e35300ff 	cmp	r3, #255	; 0xff
   10788:	1a000042 	bne	10898 <udc_event_handle+0x700>
   1078c:	e59f1178 	ldr	r1, [pc, #376]	; 1090c <udc_event_handle+0x774>
   10790:	e59f0180 	ldr	r0, [pc, #384]	; 10918 <udc_event_handle+0x780>
   10794:	e5913800 	ldr	r3, [r1, #2048]
   10798:	e1d021b6 	ldrh	r2, [r0, #22]
   1079c:	e3c33e7f 	bic	r3, r3, #2032	; 0x7f0
   107a0:	e202207f 	and	r2, r2, #127	; 0x7f
   107a4:	e1833202 	orr	r3, r3, r2, lsl #4
   107a8:	e5813800 	str	r3, [r1, #2048]
   107ac:	e5900024 	ldr	r0, [r0, #36]
   107b0:	e3a03702 	mov	r3, #524288	; 0x80000
   107b4:	e5cc2000 	strb	r2, [ip]
   107b8:	e5813910 	str	r3, [r1, #2320]
   107bc:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   107c0:	e5810914 	str	r0, [r1, #2324]
   107c4:	e5813900 	str	r3, [r1, #2304]
   107c8:	ea000032 	b	10898 <udc_event_handle+0x700>
   107cc:	e59f016c 	ldr	r0, [pc, #364]	; 10940 <udc_event_handle+0x7a8>
   107d0:	e5d03000 	ldrb	r3, [r0]
   107d4:	e35300ff 	cmp	r3, #255	; 0xff
   107d8:	0a00002e 	beq	10898 <udc_event_handle+0x700>
   107dc:	e59f1134 	ldr	r1, [pc, #308]	; 10918 <udc_event_handle+0x780>
   107e0:	e1d131b6 	ldrh	r3, [r1, #22]
   107e4:	e3530000 	cmp	r3, #0	; 0x0
   107e8:	03e03000 	mvneq	r3, #0	; 0x0
   107ec:	05911024 	ldreq	r1, [r1, #36]
   107f0:	059f2114 	ldreq	r2, [pc, #276]	; 1090c <udc_event_handle+0x774>
   107f4:	05c03000 	strbeq	r3, [r0]
   107f8:	0a000013 	beq	1084c <udc_event_handle+0x6b4>
   107fc:	e3530001 	cmp	r3, #1	; 0x1
   10800:	1a000024 	bne	10898 <udc_event_handle+0x700>
   10804:	e59f2100 	ldr	r2, [pc, #256]	; 1090c <udc_event_handle+0x774>
   10808:	e59f3134 	ldr	r3, [pc, #308]	; 10944 <udc_event_handle+0x7ac>
   1080c:	e5911024 	ldr	r1, [r1, #36]
   10810:	e5823920 	str	r3, [r2, #2336]
   10814:	e2833d07 	add	r3, r3, #448	; 0x1c0
   10818:	e5823b40 	str	r3, [r2, #2880]
   1081c:	e592381c 	ldr	r3, [r2, #2076]
   10820:	e3833701 	orr	r3, r3, #262144	; 0x40000
   10824:	e3833002 	orr	r3, r3, #2	; 0x2
   10828:	e582381c 	str	r3, [r2, #2076]
   1082c:	ea000006 	b	1084c <udc_event_handle+0x6b4>
   10830:	e210c01f 	ands	ip, r0, #31	; 0x1f
   10834:	1a000017 	bne	10898 <udc_event_handle+0x700>
   10838:	e3510000 	cmp	r1, #0	; 0x0
   1083c:	e59f50d4 	ldr	r5, [pc, #212]	; 10918 <udc_event_handle+0x780>
   10840:	1a000004 	bne	10858 <udc_event_handle+0x6c0>
   10844:	e5951024 	ldr	r1, [r5, #36]
   10848:	e59f20bc 	ldr	r2, [pc, #188]	; 1090c <udc_event_handle+0x774>
   1084c:	e3a03702 	mov	r3, #524288	; 0x80000
   10850:	e5823910 	str	r3, [r2, #2320]
   10854:	ea00000c 	b	1088c <udc_event_handle+0x6f4>
   10858:	e1d531b6 	ldrh	r3, [r5, #22]
   1085c:	e59f20a8 	ldr	r2, [pc, #168]	; 1090c <udc_event_handle+0x774>
   10860:	e3530002 	cmp	r3, #2	; 0x2
   10864:	23a03002 	movcs	r3, #2	; 0x2
   10868:	e6ef3073 	uxtb	r3, r3
   1086c:	e1530001 	cmp	r3, r1
   10870:	21a03001 	movcs	r3, r1
   10874:	e3830702 	orr	r0, r3, #524288	; 0x80000
   10878:	e28d1006 	add	r1, sp, #6	; 0x6
   1087c:	e5c5c001 	strb	ip, [r5, #1]
   10880:	e5853004 	str	r3, [r5, #4]
   10884:	e585100c 	str	r1, [r5, #12]
   10888:	e5820910 	str	r0, [r2, #2320]
   1088c:	e3a03321 	mov	r3, #-2080374784	; 0x84000000
   10890:	e5821914 	str	r1, [r2, #2324]
   10894:	e5823900 	str	r3, [r2, #2304]
   10898:	e3003209 	movw	r3, #521	; 0x209
   1089c:	e59f2068 	ldr	r2, [pc, #104]	; 1090c <udc_event_handle+0x774>
   108a0:	e00a3003 	and	r3, sl, r3
   108a4:	e5823b08 	str	r3, [r2, #2824]
   108a8:	e3190701 	tst	r9, #262144	; 0x40000
   108ac:	0a000013 	beq	10900 <udc_event_handle+0x768>
   108b0:	e59f1054 	ldr	r1, [pc, #84]	; 1090c <udc_event_handle+0x774>
   108b4:	e5912b48 	ldr	r2, [r1, #2888]
   108b8:	e3120001 	tst	r2, #1	; 0x1
   108bc:	0a00000c 	beq	108f4 <udc_event_handle+0x75c>
   108c0:	e5913b50 	ldr	r3, [r1, #2896]
   108c4:	e59f504c 	ldr	r5, [pc, #76]	; 10918 <udc_event_handle+0x780>
   108c8:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
   108cc:	e3c3373e 	bic	r3, r3, #16252928	; 0xf80000
   108d0:	e3530c02 	cmp	r3, #512	; 0x200
   108d4:	05953028 	ldreq	r3, [r5, #40]
   108d8:	13a03000 	movne	r3, #0	; 0x0
   108dc:	03833702 	orreq	r3, r3, #524288	; 0x80000
   108e0:	05813b50 	streq	r3, [r1, #2896]
   108e4:	05913b40 	ldreq	r3, [r1, #2880]
   108e8:	15c53003 	strbne	r3, [r5, #3]
   108ec:	03833321 	orreq	r3, r3, #-2080374784	; 0x84000000
   108f0:	05813b40 	streq	r3, [r1, #2880]
   108f4:	e59f3010 	ldr	r3, [pc, #16]	; 1090c <udc_event_handle+0x774>
   108f8:	e2022005 	and	r2, r2, #5	; 0x5
   108fc:	e5832b48 	str	r2, [r3, #2888]
   10900:	e28dd008 	add	sp, sp, #8	; 0x8
   10904:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
   10908:	3c00d220 	.word	0x3c00d220
   1090c:	27100000 	.word	0x27100000
   10910:	00010001 	.word	0x00010001
   10914:	0c001780 	.word	0x0c001780
   10918:	3c00d224 	.word	0x3c00d224
   1091c:	60080040 	.word	0x60080040
   10920:	3c00d21c 	.word	0x3c00d21c
   10924:	18088200 	.word	0x18088200
   10928:	3c00d238 	.word	0x3c00d238
   1092c:	00015f76 	.word	0x00015f76
   10930:	3c00d0d9 	.word	0x3c00d0d9
   10934:	3c00d0f9 	.word	0x3c00d0f9
   10938:	3c00d0b0 	.word	0x3c00d0b0
   1093c:	3c00d884 	.word	0x3c00d884
   10940:	3c00d0d8 	.word	0x3c00d0d8
   10944:	18088040 	.word	0x18088040

00010948 <udc_write>:
   10948:	e59f30a4 	ldr	r3, [pc, #164]	; 109f4 <udc_write+0xac>
   1094c:	e3a02001 	mov	r2, #1	; 0x1
   10950:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   10954:	e583000c 	str	r0, [r3, #12]
   10958:	e1a05001 	mov	r5, r1
   1095c:	e5c32001 	strb	r2, [r3, #1]
   10960:	e1a04003 	mov	r4, r3
   10964:	e1a08002 	mov	r8, r2
   10968:	e59f6088 	ldr	r6, [pc, #136]	; 109f8 <udc_write+0xb0>
   1096c:	e59f7088 	ldr	r7, [pc, #136]	; 109fc <udc_write+0xb4>
   10970:	ea00001b 	b	109e4 <udc_write+0x9c>
   10974:	e3550701 	cmp	r5, #262144	; 0x40000
   10978:	b1a02005 	movlt	r2, r5
   1097c:	a3a02701 	movge	r2, #262144	; 0x40000
   10980:	e292303f 	adds	r3, r2, #63	; 0x3f
   10984:	4282307e 	addmi	r3, r2, #126	; 0x7e
   10988:	e594100c 	ldr	r1, [r4, #12]
   1098c:	e1a03343 	asr	r3, r3, #6
   10990:	e1823983 	orr	r3, r2, r3, lsl #19
   10994:	e5863930 	str	r3, [r6, #2352]
   10998:	e5861934 	str	r1, [r6, #2356]
   1099c:	e5963920 	ldr	r3, [r6, #2336]
   109a0:	e5842004 	str	r2, [r4, #4]
   109a4:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
   109a8:	e5863920 	str	r3, [r6, #2336]
   109ac:	e5c48002 	strb	r8, [r4, #2]
   109b0:	ea000000 	b	109b8 <udc_write+0x70>
   109b4:	ebfffdf7 	bl	10198 <udc_event_handle>
   109b8:	e5d43002 	ldrb	r3, [r4, #2]
   109bc:	e3530000 	cmp	r3, #0	; 0x0
   109c0:	0a000002 	beq	109d0 <udc_write+0x88>
   109c4:	e5973000 	ldr	r3, [r7]
   109c8:	e3530000 	cmp	r3, #0	; 0x0
   109cc:	1afffff8 	bne	109b4 <udc_write+0x6c>
   109d0:	e5943004 	ldr	r3, [r4, #4]
   109d4:	e594200c 	ldr	r2, [r4, #12]
   109d8:	e0635005 	rsb	r5, r3, r5
   109dc:	e0833002 	add	r3, r3, r2
   109e0:	e584300c 	str	r3, [r4, #12]
   109e4:	e3550000 	cmp	r5, #0	; 0x0
   109e8:	1affffe1 	bne	10974 <udc_write+0x2c>
   109ec:	e1a00005 	mov	r0, r5
   109f0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   109f4:	3c00d224 	.word	0x3c00d224
   109f8:	27100000 	.word	0x27100000
   109fc:	3c00d0d4 	.word	0x3c00d0d4

00010a00 <udc_read>:
   10a00:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
   10a04:	e59f30b4 	ldr	r3, [pc, #180]	; 10ac0 <udc_read+0xc0>
   10a08:	e24dd004 	sub	sp, sp, #4	; 0x4
   10a0c:	e1a04001 	mov	r4, r1
   10a10:	e1a05003 	mov	r5, r3
   10a14:	e5830010 	str	r0, [r3, #16]
   10a18:	e3a0a001 	mov	sl, #1	; 0x1
   10a1c:	e59f60a0 	ldr	r6, [pc, #160]	; 10ac4 <udc_read+0xc4>
   10a20:	e59f80a0 	ldr	r8, [pc, #160]	; 10ac8 <udc_read+0xc8>
   10a24:	e1a07003 	mov	r7, r3
   10a28:	ea00001f 	b	10aac <udc_read+0xac>
   10a2c:	e3540701 	cmp	r4, #262144	; 0x40000
   10a30:	b1a02004 	movlt	r2, r4
   10a34:	a3a02701 	movge	r2, #262144	; 0x40000
   10a38:	e2823f7f 	add	r3, r2, #508	; 0x1fc
   10a3c:	e2833003 	add	r3, r3, #3	; 0x3
   10a40:	e5951010 	ldr	r1, [r5, #16]
   10a44:	e1a034c3 	asr	r3, r3, #9
   10a48:	e1823983 	orr	r3, r2, r3, lsl #19
   10a4c:	e5863b50 	str	r3, [r6, #2896]
   10a50:	e5861b54 	str	r1, [r6, #2900]
   10a54:	e5963b40 	ldr	r3, [r6, #2880]
   10a58:	e5852008 	str	r2, [r5, #8]
   10a5c:	e3833321 	orr	r3, r3, #-2080374784	; 0x84000000
   10a60:	e5863b40 	str	r3, [r6, #2880]
   10a64:	e5c5a003 	strb	sl, [r5, #3]
   10a68:	ea000002 	b	10a78 <udc_read+0x78>
   10a6c:	ebfffdc9 	bl	10198 <udc_event_handle>
   10a70:	e3a00001 	mov	r0, #1	; 0x1
   10a74:	ebfff3da 	bl	d9e4 <udelay>
   10a78:	e5d53003 	ldrb	r3, [r5, #3]
   10a7c:	e3530000 	cmp	r3, #0	; 0x0
   10a80:	0a000004 	beq	10a98 <udc_read+0x98>
   10a84:	e5983000 	ldr	r3, [r8]
   10a88:	e3530000 	cmp	r3, #0	; 0x0
   10a8c:	1afffff6 	bne	10a6c <udc_read+0x6c>
   10a90:	e3e04000 	mvn	r4, #0	; 0x0
   10a94:	ea000006 	b	10ab4 <udc_read+0xb4>
   10a98:	e5973008 	ldr	r3, [r7, #8]
   10a9c:	e5972010 	ldr	r2, [r7, #16]
   10aa0:	e0634004 	rsb	r4, r3, r4
   10aa4:	e0833002 	add	r3, r3, r2
   10aa8:	e5873010 	str	r3, [r7, #16]
   10aac:	e3540000 	cmp	r4, #0	; 0x0
   10ab0:	1affffdd 	bne	10a2c <udc_read+0x2c>
   10ab4:	e1a00004 	mov	r0, r4
   10ab8:	e28dd004 	add	sp, sp, #4	; 0x4
   10abc:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
   10ac0:	3c00d224 	.word	0x3c00d224
   10ac4:	27100000 	.word	0x27100000
   10ac8:	3c00d0d4 	.word	0x3c00d0d4

00010acc <udc_connect>:
   10acc:	e92d4070 	push	{r4, r5, r6, lr}
   10ad0:	e59f41e4 	ldr	r4, [pc, #484]	; 10cbc <udc_connect+0x1f0>
   10ad4:	e3a02000 	mov	r2, #0	; 0x0
   10ad8:	e5943024 	ldr	r3, [r4, #36]
   10adc:	e3e01000 	mvn	r1, #0	; 0x0
   10ae0:	e1530002 	cmp	r3, r2
   10ae4:	e59f31d4 	ldr	r3, [pc, #468]	; 10cc0 <udc_connect+0x1f4>
   10ae8:	e5c42003 	strb	r2, [r4, #3]
   10aec:	e5c31000 	strb	r1, [r3]
   10af0:	e5c42000 	strb	r2, [r4]
   10af4:	e5c42002 	strb	r2, [r4, #2]
   10af8:	1a000002 	bne	10b08 <udc_connect+0x3c>
   10afc:	e3a00028 	mov	r0, #40	; 0x28
   10b00:	ebffc2fa 	bl	16f0 <malloc>
   10b04:	e5840024 	str	r0, [r4, #36]
   10b08:	e59f31ac 	ldr	r3, [pc, #428]	; 10cbc <udc_connect+0x1f0>
   10b0c:	e3a06001 	mov	r6, #1	; 0x1
   10b10:	e59f51ac 	ldr	r5, [pc, #428]	; 10cc4 <udc_connect+0x1f8>
   10b14:	e59f01ac 	ldr	r0, [pc, #428]	; 10cc8 <udc_connect+0x1fc>
   10b18:	e5836020 	str	r6, [r3, #32]
   10b1c:	ebffbf18 	bl	784 <module_enable>
   10b20:	e3a02004 	mov	r2, #4	; 0x4
   10b24:	e3a03003 	mov	r3, #3	; 0x3
   10b28:	e3a01000 	mov	r1, #0	; 0x0
   10b2c:	e585243c 	str	r2, [r5, #1084]
   10b30:	e5853440 	str	r3, [r5, #1088]
   10b34:	e5852444 	str	r2, [r5, #1092]
   10b38:	e5853448 	str	r3, [r5, #1096]
   10b3c:	e585344c 	str	r3, [r5, #1100]
   10b40:	e5853450 	str	r3, [r5, #1104]
   10b44:	e5856454 	str	r6, [r5, #1108]
   10b48:	e5856458 	str	r6, [r5, #1112]
   10b4c:	e585145c 	str	r1, [r5, #1116]
   10b50:	e5851460 	str	r1, [r5, #1120]
   10b54:	e5953420 	ldr	r3, [r5, #1056]
   10b58:	e1a00006 	mov	r0, r6
   10b5c:	e3c33007 	bic	r3, r3, #7	; 0x7
   10b60:	e3833005 	orr	r3, r3, #5	; 0x5
   10b64:	e5853420 	str	r3, [r5, #1056]
   10b68:	e5953424 	ldr	r3, [r5, #1060]
   10b6c:	e3833003 	orr	r3, r3, #3	; 0x3
   10b70:	e5853424 	str	r3, [r5, #1060]
   10b74:	e595342c 	ldr	r3, [r5, #1068]
   10b78:	e1833006 	orr	r3, r3, r6
   10b7c:	e585342c 	str	r3, [r5, #1068]
   10b80:	e5954400 	ldr	r4, [r5, #1024]
   10b84:	e384400f 	orr	r4, r4, #15	; 0xf
   10b88:	e5854400 	str	r4, [r5, #1024]
   10b8c:	ebfff394 	bl	d9e4 <udelay>
   10b90:	e3c4300e 	bic	r3, r4, #14	; 0xe
   10b94:	e5853400 	str	r3, [r5, #1024]
   10b98:	e1a00006 	mov	r0, r6
   10b9c:	e3c4400f 	bic	r4, r4, #15	; 0xf
   10ba0:	ebfff38f 	bl	d9e4 <udelay>
   10ba4:	e5854400 	str	r4, [r5, #1024]
   10ba8:	e1a00006 	mov	r0, r6
   10bac:	ebfff38c 	bl	d9e4 <udelay>
   10bb0:	e59f2114 	ldr	r2, [pc, #276]	; 10ccc <udc_connect+0x200>
   10bb4:	e59f3114 	ldr	r3, [pc, #276]	; 10cd0 <udc_connect+0x204>
   10bb8:	e1a05002 	mov	r5, r2
   10bbc:	e582300c 	str	r3, [r2, #12]
   10bc0:	e5954010 	ldr	r4, [r5, #16]
   10bc4:	e3a00028 	mov	r0, #40	; 0x28
   10bc8:	ebfff385 	bl	d9e4 <udelay>
   10bcc:	e3540000 	cmp	r4, #0	; 0x0
   10bd0:	e59f40f4 	ldr	r4, [pc, #244]	; 10ccc <udc_connect+0x200>
   10bd4:	aafffff9 	bge	10bc0 <udc_connect+0xf4>
   10bd8:	e5943010 	ldr	r3, [r4, #16]
   10bdc:	e3a0000a 	mov	r0, #10	; 0xa
   10be0:	e3833001 	orr	r3, r3, #1	; 0x1
   10be4:	e5843010 	str	r3, [r4, #16]
   10be8:	ebfff37d 	bl	d9e4 <udelay>
   10bec:	e1a05004 	mov	r5, r4
   10bf0:	e5954010 	ldr	r4, [r5, #16]
   10bf4:	e3a0000a 	mov	r0, #10	; 0xa
   10bf8:	ebfff379 	bl	d9e4 <udelay>
   10bfc:	e3140001 	tst	r4, #1	; 0x1
   10c00:	e59fc0c4 	ldr	ip, [pc, #196]	; 10ccc <udc_connect+0x200>
   10c04:	1afffff9 	bne	10bf0 <udc_connect+0x124>
   10c08:	e3a03027 	mov	r3, #39	; 0x27
   10c0c:	e58c3008 	str	r3, [ip, #8]
   10c10:	e59c303c 	ldr	r3, [ip, #60]
   10c14:	e59c3014 	ldr	r3, [ip, #20]
   10c18:	e2130001 	ands	r0, r3, #1	; 0x1
   10c1c:	1a000024 	bne	10cb4 <udc_connect+0x1e8>
   10c20:	e3a03301 	mov	r3, #67108864	; 0x4000000
   10c24:	e58c3800 	str	r3, [ip, #2048]
   10c28:	e59f30a4 	ldr	r3, [pc, #164]	; 10cd4 <udc_connect+0x208>
   10c2c:	e59f20a4 	ldr	r2, [pc, #164]	; 10cd8 <udc_connect+0x20c>
   10c30:	e58c3018 	str	r3, [ip, #24]
   10c34:	e59f30a0 	ldr	r3, [pc, #160]	; 10cdc <udc_connect+0x210>
   10c38:	e59f10a0 	ldr	r1, [pc, #160]	; 10ce0 <udc_connect+0x214>
   10c3c:	e5823014 	str	r3, [r2, #20]
   10c40:	e59f309c 	ldr	r3, [pc, #156]	; 10ce4 <udc_connect+0x218>
   10c44:	e5821010 	str	r1, [r2, #16]
   10c48:	e5823004 	str	r3, [r2, #4]
   10c4c:	e59f3094 	ldr	r3, [pc, #148]	; 10ce8 <udc_connect+0x21c>
   10c50:	e582100c 	str	r1, [r2, #12]
   10c54:	e5823008 	str	r3, [r2, #8]
   10c58:	ebfff385 	bl	da74 <get_timer>
   10c5c:	e59f5058 	ldr	r5, [pc, #88]	; 10cbc <udc_connect+0x1f0>
   10c60:	e1a04000 	mov	r4, r0
   10c64:	ebfffd4b 	bl	10198 <udc_event_handle>
   10c68:	e5d53000 	ldrb	r3, [r5]
   10c6c:	e1a00004 	mov	r0, r4
   10c70:	e3530000 	cmp	r3, #0	; 0x0
   10c74:	1a000003 	bne	10c88 <udc_connect+0x1bc>
   10c78:	ebfff37d 	bl	da74 <get_timer>
   10c7c:	e3500ffa 	cmp	r0, #1000	; 0x3e8
   10c80:	9afffff7 	bls	10c64 <udc_connect+0x198>
   10c84:	ea00000a 	b	10cb4 <udc_connect+0x1e8>
   10c88:	e59f203c 	ldr	r2, [pc, #60]	; 10ccc <udc_connect+0x200>
   10c8c:	e59f3058 	ldr	r3, [pc, #88]	; 10cec <udc_connect+0x220>
   10c90:	e3a00000 	mov	r0, #0	; 0x0
   10c94:	e5823920 	str	r3, [r2, #2336]
   10c98:	e2833d07 	add	r3, r3, #448	; 0x1c0
   10c9c:	e5823b40 	str	r3, [r2, #2880]
   10ca0:	e592381c 	ldr	r3, [r2, #2076]
   10ca4:	e3833701 	orr	r3, r3, #262144	; 0x40000
   10ca8:	e3833002 	orr	r3, r3, #2	; 0x2
   10cac:	e582381c 	str	r3, [r2, #2076]
   10cb0:	e8bd8070 	pop	{r4, r5, r6, pc}
   10cb4:	e3e00000 	mvn	r0, #0	; 0x0
   10cb8:	e8bd8070 	pop	{r4, r5, r6, pc}
   10cbc:	3c00d224 	.word	0x3c00d224
   10cc0:	3c00d0d8 	.word	0x3c00d0d8
   10cc4:	21e38000 	.word	0x21e38000
   10cc8:	21e38400 	.word	0x21e38400
   10ccc:	27100000 	.word	0x27100000
   10cd0:	4c001408 	.word	0x4c001408
   10cd4:	000c3c00 	.word	0x000c3c00
   10cd8:	3c00d884 	.word	0x3c00d884
   10cdc:	000189b3 	.word	0x000189b3
   10ce0:	000189c8 	.word	0x000189c8
   10ce4:	000189bc 	.word	0x000189bc
   10ce8:	3c00d0b4 	.word	0x3c00d0b4
   10cec:	18088040 	.word	0x18088040

00010cf0 <linkpc_set_stage>:
   10cf0:	e1a01000 	mov	r1, r0
   10cf4:	e59f0004 	ldr	r0, [pc, #4]	; 10d00 <linkpc_set_stage+0x10>
   10cf8:	e3a02010 	mov	r2, #16	; 0x10
   10cfc:	eaffc1ac 	b	13b4 <strncpy>
   10d00:	3c00d104 	.word	0x3c00d104

00010d04 <__response_ok>:
   10d04:	e92d4010 	push	{r4, lr}
   10d08:	e24dd020 	sub	sp, sp, #32	; 0x20
   10d0c:	e59f1038 	ldr	r1, [pc, #56]	; 10d4c <__response_ok+0x48>
   10d10:	e3a02005 	mov	r2, #5	; 0x5
   10d14:	e1a0000d 	mov	r0, sp
   10d18:	ebffc20f 	bl	155c <memcpy>
   10d1c:	e3a0201b 	mov	r2, #27	; 0x1b
   10d20:	e3a01000 	mov	r1, #0	; 0x0
   10d24:	e28d0005 	add	r0, sp, #5	; 0x5
   10d28:	ebffc203 	bl	153c <memset>
   10d2c:	e1a0000d 	mov	r0, sp
   10d30:	e3a01004 	mov	r1, #4	; 0x4
   10d34:	ebffff03 	bl	10948 <udc_write>
   10d38:	e59f0010 	ldr	r0, [pc, #16]	; 10d50 <__response_ok+0x4c>
   10d3c:	e1a0400d 	mov	r4, sp
   10d40:	ebfff327 	bl	d9e4 <udelay>
   10d44:	e28dd020 	add	sp, sp, #32	; 0x20
   10d48:	e8bd8010 	pop	{r4, pc}
   10d4c:	00015f88 	.word	0x00015f88
   10d50:	0007a120 	.word	0x0007a120

00010d54 <linkpc_loop>:
   10d54:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   10d58:	e59f05bc 	ldr	r0, [pc, #1468]	; 1131c <linkpc_loop+0x5c8>
   10d5c:	e24ddf5b 	sub	sp, sp, #364	; 0x16c
   10d60:	e59f45b8 	ldr	r4, [pc, #1464]	; 11320 <linkpc_loop+0x5cc>
   10d64:	e1a0e00f 	mov	lr, pc
   10d68:	e594f000 	ldr	pc, [r4]
   10d6c:	ebffff56 	bl	10acc <udc_connect>
   10d70:	e3500000 	cmp	r0, #0	; 0x0
   10d74:	13a04001 	movne	r4, #1	; 0x1
   10d78:	1a000160 	bne	11300 <linkpc_loop+0x5ac>
   10d7c:	e59f05a0 	ldr	r0, [pc, #1440]	; 11324 <linkpc_loop+0x5d0>
   10d80:	e1a0e00f 	mov	lr, pc
   10d84:	e594f000 	ldr	pc, [r4]
   10d88:	e28d8094 	add	r8, sp, #148	; 0x94
   10d8c:	e59f0594 	ldr	r0, [pc, #1428]	; 11328 <linkpc_loop+0x5d4>
   10d90:	e1a0e00f 	mov	lr, pc
   10d94:	e594f000 	ldr	pc, [r4]
   10d98:	e59fb58c 	ldr	fp, [pc, #1420]	; 1132c <linkpc_loop+0x5d8>
   10d9c:	e2882004 	add	r2, r8, #4	; 0x4
   10da0:	e1a09004 	mov	r9, r4
   10da4:	e58d200c 	str	r2, [sp, #12]
   10da8:	e59b1000 	ldr	r1, [fp]
   10dac:	e3510000 	cmp	r1, #0	; 0x0
   10db0:	159fc578 	ldrne	ip, [pc, #1400]	; 11330 <linkpc_loop+0x5dc>
   10db4:	028d4f45 	addeq	r4, sp, #276	; 0x114
   10db8:	159c3000 	ldrne	r3, [ip]
   10dbc:	03a05040 	moveq	r5, #64	; 0x40
   10dc0:	1283430f 	addne	r4, r3, #1006632960	; 0x3c000000
   10dc4:	11a05001 	movne	r5, r1
   10dc8:	e1a01005 	mov	r1, r5
   10dcc:	e1a00004 	mov	r0, r4
   10dd0:	ebffff0a 	bl	10a00 <udc_read>
   10dd4:	e1a00008 	mov	r0, r8
   10dd8:	e3a01000 	mov	r1, #0	; 0x0
   10ddc:	e3a02080 	mov	r2, #128	; 0x80
   10de0:	ebffc1d5 	bl	153c <memset>
   10de4:	e59b3000 	ldr	r3, [fp]
   10de8:	e3530000 	cmp	r3, #0	; 0x0
   10dec:	0a00002b 	beq	10ea0 <linkpc_loop+0x14c>
   10df0:	e28d4014 	add	r4, sp, #20	; 0x14
   10df4:	e3a02080 	mov	r2, #128	; 0x80
   10df8:	e3a01000 	mov	r1, #0	; 0x0
   10dfc:	e1a00004 	mov	r0, r4
   10e00:	ebffc1cd 	bl	153c <memset>
   10e04:	e59f0528 	ldr	r0, [pc, #1320]	; 11334 <linkpc_loop+0x5e0>
   10e08:	e59f1528 	ldr	r1, [pc, #1320]	; 11338 <linkpc_loop+0x5e4>
   10e0c:	e1a0e00f 	mov	lr, pc
   10e10:	e599f000 	ldr	pc, [r9]
   10e14:	e59f2514 	ldr	r2, [pc, #1300]	; 11330 <linkpc_loop+0x5dc>
   10e18:	e59b0000 	ldr	r0, [fp]
   10e1c:	e5923000 	ldr	r3, [r2]
   10e20:	e59fc508 	ldr	ip, [pc, #1288]	; 11330 <linkpc_loop+0x5dc>
   10e24:	e0632000 	rsb	r2, r3, r0
   10e28:	e1550002 	cmp	r5, r2
   10e2c:	90833005 	addls	r3, r3, r5
   10e30:	80833002 	addhi	r3, r3, r2
   10e34:	e1530000 	cmp	r3, r0
   10e38:	e58c3000 	str	r3, [ip]
   10e3c:	3affffd9 	bcc	10da8 <linkpc_loop+0x54>
   10e40:	e59f34f4 	ldr	r3, [pc, #1268]	; 1133c <linkpc_loop+0x5e8>
   10e44:	e5933000 	ldr	r3, [r3]
   10e48:	e3530000 	cmp	r3, #0	; 0x0
   10e4c:	e59f34ec 	ldr	r3, [pc, #1260]	; 11340 <linkpc_loop+0x5ec>
   10e50:	159f14ec 	ldrne	r1, [pc, #1260]	; 11344 <linkpc_loop+0x5f0>
   10e54:	e5830000 	str	r0, [r3]
   10e58:	e3a03000 	mov	r3, #0	; 0x0
   10e5c:	11a00004 	movne	r0, r4
   10e60:	01a00004 	moveq	r0, r4
   10e64:	059f14dc 	ldreq	r1, [pc, #1244]	; 11348 <linkpc_loop+0x5f4>
   10e68:	e58b3000 	str	r3, [fp]
   10e6c:	e28d4014 	add	r4, sp, #20	; 0x14
   10e70:	eb0005cf 	bl	125b4 <sprintf>
   10e74:	e1a00004 	mov	r0, r4
   10e78:	ebffc19d 	bl	14f4 <strlen>
   10e7c:	e1a01000 	mov	r1, r0
   10e80:	e1a00004 	mov	r0, r4
   10e84:	ebfffeaf 	bl	10948 <udc_write>
   10e88:	e59f24a0 	ldr	r2, [pc, #1184]	; 11330 <linkpc_loop+0x5dc>
   10e8c:	e59f04b8 	ldr	r0, [pc, #1208]	; 1134c <linkpc_loop+0x5f8>
   10e90:	e5921000 	ldr	r1, [r2]
   10e94:	e1a0e00f 	mov	lr, pc
   10e98:	e599f000 	ldr	pc, [r9]
   10e9c:	eaffffc1 	b	10da8 <linkpc_loop+0x54>
   10ea0:	e1a01004 	mov	r1, r4
   10ea4:	e59f0488 	ldr	r0, [pc, #1160]	; 11334 <linkpc_loop+0x5e0>
   10ea8:	e1a0e00f 	mov	lr, pc
   10eac:	e599f000 	ldr	pc, [r9]
   10eb0:	e59f1498 	ldr	r1, [pc, #1176]	; 11350 <linkpc_loop+0x5fc>
   10eb4:	e1a00008 	mov	r0, r8
   10eb8:	eb0005bd 	bl	125b4 <sprintf>
   10ebc:	e1a00004 	mov	r0, r4
   10ec0:	e59f148c 	ldr	r1, [pc, #1164]	; 11354 <linkpc_loop+0x600>
   10ec4:	e3a02006 	mov	r2, #6	; 0x6
   10ec8:	ebffc16a 	bl	1478 <strncmp>
   10ecc:	e3500000 	cmp	r0, #0	; 0x0
   10ed0:	1a000002 	bne	10ee0 <linkpc_loop+0x18c>
   10ed4:	ebffff8a 	bl	10d04 <__response_ok>
   10ed8:	ebffbe5d 	bl	854 <system_reset>
   10edc:	ea0000fa 	b	112cc <linkpc_loop+0x578>
   10ee0:	e1a00004 	mov	r0, r4
   10ee4:	e59f146c 	ldr	r1, [pc, #1132]	; 11358 <linkpc_loop+0x604>
   10ee8:	e3a02007 	mov	r2, #7	; 0x7
   10eec:	ebffc161 	bl	1478 <strncmp>
   10ef0:	e3500000 	cmp	r0, #0	; 0x0
   10ef4:	1a00002d 	bne	10fb0 <linkpc_loop+0x25c>
   10ef8:	e2844007 	add	r4, r4, #7	; 0x7
   10efc:	e1a02004 	mov	r2, r4
   10f00:	e59f1454 	ldr	r1, [pc, #1108]	; 1135c <linkpc_loop+0x608>
   10f04:	e59f0454 	ldr	r0, [pc, #1108]	; 11360 <linkpc_loop+0x60c>
   10f08:	e1a0e00f 	mov	lr, pc
   10f0c:	e599f000 	ldr	pc, [r9]
   10f10:	e59f1430 	ldr	r1, [pc, #1072]	; 11348 <linkpc_loop+0x5f4>
   10f14:	e1a00008 	mov	r0, r8
   10f18:	ebffc11e 	bl	1398 <strcpy>
   10f1c:	e59f1440 	ldr	r1, [pc, #1088]	; 11364 <linkpc_loop+0x610>
   10f20:	e1a00004 	mov	r0, r4
   10f24:	e3a02007 	mov	r2, #7	; 0x7
   10f28:	ebffc152 	bl	1478 <strncmp>
   10f2c:	e3500000 	cmp	r0, #0	; 0x0
   10f30:	059d000c 	ldreq	r0, [sp, #12]
   10f34:	059f142c 	ldreq	r1, [pc, #1068]	; 11368 <linkpc_loop+0x614>
   10f38:	0a000010 	beq	10f80 <linkpc_loop+0x22c>
   10f3c:	e59f1428 	ldr	r1, [pc, #1064]	; 1136c <linkpc_loop+0x618>
   10f40:	e1a00004 	mov	r0, r4
   10f44:	e3a02005 	mov	r2, #5	; 0x5
   10f48:	ebffc14a 	bl	1478 <strncmp>
   10f4c:	e3500000 	cmp	r0, #0	; 0x0
   10f50:	059d000c 	ldreq	r0, [sp, #12]
   10f54:	059f1414 	ldreq	r1, [pc, #1044]	; 11370 <linkpc_loop+0x61c>
   10f58:	0a000008 	beq	10f80 <linkpc_loop+0x22c>
   10f5c:	e1a00004 	mov	r0, r4
   10f60:	e59f140c 	ldr	r1, [pc, #1036]	; 11374 <linkpc_loop+0x620>
   10f64:	e3a02008 	mov	r2, #8	; 0x8
   10f68:	ebffc142 	bl	1478 <strncmp>
   10f6c:	e3500000 	cmp	r0, #0	; 0x0
   10f70:	1a000004 	bne	10f88 <linkpc_loop+0x234>
   10f74:	e59f33fc 	ldr	r3, [pc, #1020]	; 11378 <linkpc_loop+0x624>
   10f78:	e59d000c 	ldr	r0, [sp, #12]
   10f7c:	e5931008 	ldr	r1, [r3, #8]
   10f80:	ebffc104 	bl	1398 <strcpy>
   10f84:	ea0000d0 	b	112cc <linkpc_loop+0x578>
   10f88:	e1a00004 	mov	r0, r4
   10f8c:	e59f13e8 	ldr	r1, [pc, #1000]	; 1137c <linkpc_loop+0x628>
   10f90:	e3a0200c 	mov	r2, #12	; 0xc
   10f94:	ebffc137 	bl	1478 <strncmp>
   10f98:	e3500000 	cmp	r0, #0	; 0x0
   10f9c:	1a0000ca 	bne	112cc <linkpc_loop+0x578>
   10fa0:	e59d000c 	ldr	r0, [sp, #12]
   10fa4:	e59f13d4 	ldr	r1, [pc, #980]	; 11380 <linkpc_loop+0x62c>
   10fa8:	e3a02903 	mov	r2, #49152	; 0xc000
   10fac:	ea000020 	b	11034 <linkpc_loop+0x2e0>
   10fb0:	e1a00004 	mov	r0, r4
   10fb4:	e59f13c8 	ldr	r1, [pc, #968]	; 11384 <linkpc_loop+0x630>
   10fb8:	e3a02009 	mov	r2, #9	; 0x9
   10fbc:	ebffc12d 	bl	1478 <strncmp>
   10fc0:	e2505000 	subs	r5, r0, #0	; 0x0
   10fc4:	1a00001c 	bne	1103c <linkpc_loop+0x2e8>
   10fc8:	e59f336c 	ldr	r3, [pc, #876]	; 1133c <linkpc_loop+0x5e8>
   10fcc:	e59fc35c 	ldr	ip, [pc, #860]	; 11330 <linkpc_loop+0x5dc>
   10fd0:	e3a02010 	mov	r2, #16	; 0x10
   10fd4:	e1a01005 	mov	r1, r5
   10fd8:	e5835000 	str	r5, [r3]
   10fdc:	e58c5000 	str	r5, [ip]
   10fe0:	e2840009 	add	r0, r4, #9	; 0x9
   10fe4:	eb0001e6 	bl	11784 <simple_strtoul>
   10fe8:	e1a01000 	mov	r1, r0
   10fec:	e58b0000 	str	r0, [fp]
   10ff0:	e59f0390 	ldr	r0, [pc, #912]	; 11388 <linkpc_loop+0x634>
   10ff4:	e1a0e00f 	mov	lr, pc
   10ff8:	e599f000 	ldr	pc, [r9]
   10ffc:	e59b2000 	ldr	r2, [fp]
   11000:	e3520000 	cmp	r2, #0	; 0x0
   11004:	01a00008 	moveq	r0, r8
   11008:	059f137c 	ldreq	r1, [pc, #892]	; 1138c <linkpc_loop+0x638>
   1100c:	0a0000ad 	beq	112c8 <linkpc_loop+0x574>
   11010:	e3520903 	cmp	r2, #49152	; 0xc000
   11014:	91a00008 	movls	r0, r8
   11018:	959f1370 	ldrls	r1, [pc, #880]	; 11390 <linkpc_loop+0x63c>
   1101c:	9a000004 	bls	11034 <linkpc_loop+0x2e0>
   11020:	e1a00008 	mov	r0, r8
   11024:	e59f1368 	ldr	r1, [pc, #872]	; 11394 <linkpc_loop+0x640>
   11028:	eb000561 	bl	125b4 <sprintf>
   1102c:	e58b5000 	str	r5, [fp]
   11030:	ea0000a5 	b	112cc <linkpc_loop+0x578>
   11034:	eb00055e 	bl	125b4 <sprintf>
   11038:	ea0000a3 	b	112cc <linkpc_loop+0x578>
   1103c:	e1a00004 	mov	r0, r4
   11040:	e59f1350 	ldr	r1, [pc, #848]	; 11398 <linkpc_loop+0x644>
   11044:	e3a02006 	mov	r2, #6	; 0x6
   11048:	ebffc10a 	bl	1478 <strncmp>
   1104c:	e3500000 	cmp	r0, #0	; 0x0
   11050:	1a00005a 	bne	111c0 <linkpc_loop+0x46c>
   11054:	e2844006 	add	r4, r4, #6	; 0x6
   11058:	e1a00004 	mov	r0, r4
   1105c:	e59f1338 	ldr	r1, [pc, #824]	; 1139c <linkpc_loop+0x648>
   11060:	e3a02003 	mov	r2, #3	; 0x3
   11064:	ebffc103 	bl	1478 <strncmp>
   11068:	e3500000 	cmp	r0, #0	; 0x0
   1106c:	1a000007 	bne	11090 <linkpc_loop+0x33c>
   11070:	e280030f 	add	r0, r0, #1006632960	; 0x3c000000
   11074:	ebffd981 	bl	7680 <cdbg_config_isi>
   11078:	e3500000 	cmp	r0, #0	; 0x0
   1107c:	11a00008 	movne	r0, r8
   11080:	159f1318 	ldrne	r1, [pc, #792]	; 113a0 <linkpc_loop+0x64c>
   11084:	01a00008 	moveq	r0, r8
   11088:	059f12b8 	ldreq	r1, [pc, #696]	; 11348 <linkpc_loop+0x5f4>
   1108c:	ea00008d 	b	112c8 <linkpc_loop+0x574>
   11090:	e1a00004 	mov	r0, r4
   11094:	ebffd43f 	bl	6198 <vs_device_id>
   11098:	e59f5304 	ldr	r5, [pc, #772]	; 113a4 <linkpc_loop+0x650>
   1109c:	e59fc304 	ldr	ip, [pc, #772]	; 113a8 <linkpc_loop+0x654>
   110a0:	e5953000 	ldr	r3, [r5]
   110a4:	e1a07000 	mov	r7, r0
   110a8:	e58d3000 	str	r3, [sp]
   110ac:	e59f02f8 	ldr	r0, [pc, #760]	; 113ac <linkpc_loop+0x658>
   110b0:	e59f12f8 	ldr	r1, [pc, #760]	; 113b0 <linkpc_loop+0x65c>
   110b4:	e1a02007 	mov	r2, r7
   110b8:	e59c3000 	ldr	r3, [ip]
   110bc:	e1a0e00f 	mov	lr, pc
   110c0:	e599f000 	ldr	pc, [r9]
   110c4:	e357000e 	cmp	r7, #14	; 0xe
   110c8:	ca000006 	bgt	110e8 <linkpc_loop+0x394>
   110cc:	e1a00007 	mov	r0, r7
   110d0:	ebffd41c 	bl	6148 <vs_device_erasable>
   110d4:	e3500000 	cmp	r0, #0	; 0x0
   110d8:	0a000005 	beq	110f4 <linkpc_loop+0x3a0>
   110dc:	e5953000 	ldr	r3, [r5]
   110e0:	e3530000 	cmp	r3, #0	; 0x0
   110e4:	1a000002 	bne	110f4 <linkpc_loop+0x3a0>
   110e8:	e1a00008 	mov	r0, r8
   110ec:	e59f12c0 	ldr	r1, [pc, #704]	; 113b4 <linkpc_loop+0x660>
   110f0:	ea000074 	b	112c8 <linkpc_loop+0x574>
   110f4:	e3a00901 	mov	r0, #16384	; 0x4000
   110f8:	ebffc17c 	bl	16f0 <malloc>
   110fc:	e250a000 	subs	sl, r0, #0	; 0x0
   11100:	01a00008 	moveq	r0, r8
   11104:	059f12ac 	ldreq	r1, [pc, #684]	; 113b8 <linkpc_loop+0x664>
   11108:	0a00006e 	beq	112c8 <linkpc_loop+0x574>
   1110c:	e3a01001 	mov	r1, #1	; 0x1
   11110:	e1a00007 	mov	r0, r7
   11114:	ebffd4f2 	bl	64e4 <vs_assign_by_id>
   11118:	e3500000 	cmp	r0, #0	; 0x0
   1111c:	b1a00008 	movlt	r0, r8
   11120:	b59f1294 	ldrlt	r1, [pc, #660]	; 113bc <linkpc_loop+0x668>
   11124:	ba000021 	blt	111b0 <linkpc_loop+0x45c>
   11128:	e1a04c07 	lsl	r4, r7, #24
   1112c:	e3a0030f 	mov	r0, #1006632960	; 0x3c000000
   11130:	e58d4154 	str	r4, [sp, #340]
   11134:	ebfff6a5 	bl	ebd0 <ram_addr2offs>
   11138:	e3a024ff 	mov	r2, #-16777216	; 0xff000000
   1113c:	e0922000 	adds	r2, r2, r0
   11140:	e3e03000 	mvn	r3, #0	; 0x0
   11144:	e0a33001 	adc	r3, r3, r1
   11148:	e59f1254 	ldr	r1, [pc, #596]	; 113a4 <linkpc_loop+0x650>
   1114c:	e28d0f55 	add	r0, sp, #340	; 0x154
   11150:	e1d1c0b2 	ldrh	ip, [r1, #2]
   11154:	e1a014a2 	lsr	r1, r2, #9
   11158:	e1811b83 	orr	r1, r1, r3, lsl #23
   1115c:	e59f2244 	ldr	r2, [pc, #580]	; 113a8 <linkpc_loop+0x654>
   11160:	e59f31d8 	ldr	r3, [pc, #472]	; 11340 <linkpc_loop+0x5ec>
   11164:	e5925000 	ldr	r5, [r2]
   11168:	e5936000 	ldr	r6, [r3]
   1116c:	e1a0c80c 	lsl	ip, ip, #16
   11170:	e1844001 	orr	r4, r4, r1
   11174:	e1a03007 	mov	r3, r7
   11178:	e3a0100d 	mov	r1, #13	; 0xd
   1117c:	e3a02000 	mov	r2, #0	; 0x0
   11180:	e58d4154 	str	r4, [sp, #340]
   11184:	e58d5158 	str	r5, [sp, #344]
   11188:	e58d6160 	str	r6, [sp, #352]
   1118c:	e58dc000 	str	ip, [sp]
   11190:	e58dc15c 	str	ip, [sp, #348]
   11194:	e58da004 	str	sl, [sp, #4]
   11198:	ebffd94d 	bl	76d4 <burn_image_loop>
   1119c:	e3500000 	cmp	r0, #0	; 0x0
   111a0:	159f1218 	ldrne	r1, [pc, #536]	; 113c0 <linkpc_loop+0x66c>
   111a4:	059f119c 	ldreq	r1, [pc, #412]	; 11348 <linkpc_loop+0x5f4>
   111a8:	11a00008 	movne	r0, r8
   111ac:	01a00008 	moveq	r0, r8
   111b0:	eb0004ff 	bl	125b4 <sprintf>
   111b4:	e1a0000a 	mov	r0, sl
   111b8:	ebffc1b4 	bl	1890 <free>
   111bc:	ea000042 	b	112cc <linkpc_loop+0x578>
   111c0:	e1a00004 	mov	r0, r4
   111c4:	e59f11f8 	ldr	r1, [pc, #504]	; 113c4 <linkpc_loop+0x670>
   111c8:	e3a02008 	mov	r2, #8	; 0x8
   111cc:	ebffc0a9 	bl	1478 <strncmp>
   111d0:	e3500000 	cmp	r0, #0	; 0x0
   111d4:	1a000003 	bne	111e8 <linkpc_loop+0x494>
   111d8:	ebfffec9 	bl	10d04 <__response_ok>
   111dc:	e3a00001 	mov	r0, #1	; 0x1
   111e0:	ebffbc98 	bl	448 <try_boot>
   111e4:	ea000038 	b	112cc <linkpc_loop+0x578>
   111e8:	e1a00004 	mov	r0, r4
   111ec:	e59f11d4 	ldr	r1, [pc, #468]	; 113c8 <linkpc_loop+0x674>
   111f0:	e3a0200a 	mov	r2, #10	; 0xa
   111f4:	ebffc09f 	bl	1478 <strncmp>
   111f8:	e2505000 	subs	r5, r0, #0	; 0x0
   111fc:	1a000019 	bne	11268 <linkpc_loop+0x514>
   11200:	e28d1f59 	add	r1, sp, #356	; 0x164
   11204:	e284000b 	add	r0, r4, #11	; 0xb
   11208:	e3a02010 	mov	r2, #16	; 0x10
   1120c:	eb00015c 	bl	11784 <simple_strtoul>
   11210:	e59f3190 	ldr	r3, [pc, #400]	; 113a8 <linkpc_loop+0x654>
   11214:	e1a01005 	mov	r1, r5
   11218:	e5830000 	str	r0, [r3]
   1121c:	e59d0164 	ldr	r0, [sp, #356]
   11220:	e3a02010 	mov	r2, #16	; 0x10
   11224:	e2800001 	add	r0, r0, #1	; 0x1
   11228:	eb000155 	bl	11784 <simple_strtoul>
   1122c:	e59f4170 	ldr	r4, [pc, #368]	; 113a4 <linkpc_loop+0x650>
   11230:	e59fc170 	ldr	ip, [pc, #368]	; 113a8 <linkpc_loop+0x654>
   11234:	e1a02000 	mov	r2, r0
   11238:	e5840000 	str	r0, [r4]
   1123c:	e59c1000 	ldr	r1, [ip]
   11240:	e59f0184 	ldr	r0, [pc, #388]	; 113cc <linkpc_loop+0x678>
   11244:	e1a0e00f 	mov	lr, pc
   11248:	e599f000 	ldr	pc, [r9]
   1124c:	e59fc154 	ldr	ip, [pc, #340]	; 113a8 <linkpc_loop+0x654>
   11250:	e1a00008 	mov	r0, r8
   11254:	e5943000 	ldr	r3, [r4]
   11258:	e59f1170 	ldr	r1, [pc, #368]	; 113d0 <linkpc_loop+0x67c>
   1125c:	e59c2000 	ldr	r2, [ip]
   11260:	eb0004d3 	bl	125b4 <sprintf>
   11264:	ea000018 	b	112cc <linkpc_loop+0x578>
   11268:	e1a00004 	mov	r0, r4
   1126c:	e59f1160 	ldr	r1, [pc, #352]	; 113d4 <linkpc_loop+0x680>
   11270:	e3a0200d 	mov	r2, #13	; 0xd
   11274:	ebffc07f 	bl	1478 <strncmp>
   11278:	e3500000 	cmp	r0, #0	; 0x0
   1127c:	1a000006 	bne	1129c <linkpc_loop+0x548>
   11280:	e59f20f0 	ldr	r2, [pc, #240]	; 11378 <linkpc_loop+0x624>
   11284:	e284100e 	add	r1, r4, #14	; 0xe
   11288:	e5920008 	ldr	r0, [r2, #8]
   1128c:	ebffc041 	bl	1398 <strcpy>
   11290:	ebfffe9b 	bl	10d04 <__response_ok>
   11294:	ebfffeae 	bl	10d54 <linkpc_loop>
   11298:	ea00000b 	b	112cc <linkpc_loop+0x578>
   1129c:	e1a00004 	mov	r0, r4
   112a0:	e59f1130 	ldr	r1, [pc, #304]	; 113d8 <linkpc_loop+0x684>
   112a4:	e3a02007 	mov	r2, #7	; 0x7
   112a8:	ebffc072 	bl	1478 <strncmp>
   112ac:	e3500000 	cmp	r0, #0	; 0x0
   112b0:	1a000002 	bne	112c0 <linkpc_loop+0x56c>
   112b4:	ebfffe92 	bl	10d04 <__response_ok>
   112b8:	ebffd88f 	bl	74fc <cdbg_shell>
   112bc:	eafffffe 	b	112bc <linkpc_loop+0x568>
   112c0:	e59f1114 	ldr	r1, [pc, #276]	; 113dc <linkpc_loop+0x688>
   112c4:	e1a00008 	mov	r0, r8
   112c8:	eb0004b9 	bl	125b4 <sprintf>
   112cc:	e3a01c01 	mov	r1, #256	; 0x100
   112d0:	e1a00008 	mov	r0, r8
   112d4:	ebffc08e 	bl	1514 <strnlen>
   112d8:	e1a01000 	mov	r1, r0
   112dc:	e1a00008 	mov	r0, r8
   112e0:	ebfffd98 	bl	10948 <udc_write>
   112e4:	e3500000 	cmp	r0, #0	; 0x0
   112e8:	0afffeae 	beq	10da8 <linkpc_loop+0x54>
   112ec:	e59f00ec 	ldr	r0, [pc, #236]	; 113e0 <linkpc_loop+0x68c>
   112f0:	e59f3028 	ldr	r3, [pc, #40]	; 11320 <linkpc_loop+0x5cc>
   112f4:	e1a0e00f 	mov	lr, pc
   112f8:	e593f000 	ldr	pc, [r3]
   112fc:	e3a04000 	mov	r4, #0	; 0x0
   11300:	e59f00dc 	ldr	r0, [pc, #220]	; 113e4 <linkpc_loop+0x690>
   11304:	e59f3014 	ldr	r3, [pc, #20]	; 11320 <linkpc_loop+0x5cc>
   11308:	e1a0e00f 	mov	lr, pc
   1130c:	e593f000 	ldr	pc, [r3]
   11310:	e1a00004 	mov	r0, r4
   11314:	e28ddf5b 	add	sp, sp, #364	; 0x16c
   11318:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   1131c:	000189d9 	.word	0x000189d9
   11320:	3c00d6cc 	.word	0x3c00d6cc
   11324:	00016a13 	.word	0x00016a13
   11328:	000189e8 	.word	0x000189e8
   1132c:	3c00d250 	.word	0x3c00d250
   11330:	3c00d254 	.word	0x3c00d254
   11334:	0001641f 	.word	0x0001641f
   11338:	00015fa8 	.word	0x00015fa8
   1133c:	3c00d258 	.word	0x3c00d258
   11340:	3c00d25c 	.word	0x3c00d25c
   11344:	000189fb 	.word	0x000189fb
   11348:	00015f88 	.word	0x00015f88
   1134c:	00018a01 	.word	0x00018a01
   11350:	00018a24 	.word	0x00018a24
   11354:	00018a29 	.word	0x00018a29
   11358:	00018a30 	.word	0x00018a30
   1135c:	00015fb4 	.word	0x00015fb4
   11360:	00018a38 	.word	0x00018a38
   11364:	00018a40 	.word	0x00018a40
   11368:	3c00d088 	.word	0x3c00d088
   1136c:	00018a48 	.word	0x00018a48
   11370:	3c00d104 	.word	0x3c00d104
   11374:	00018a4e 	.word	0x00018a4e
   11378:	3c00d884 	.word	0x3c00d884
   1137c:	00018a57 	.word	0x00018a57
   11380:	00018aba 	.word	0x00018aba
   11384:	00018a64 	.word	0x00018a64
   11388:	00018a6e 	.word	0x00018a6e
   1138c:	00018a8d 	.word	0x00018a8d
   11390:	00018abf 	.word	0x00018abf
   11394:	00018aa6 	.word	0x00018aa6
   11398:	00018ac8 	.word	0x00018ac8
   1139c:	00016898 	.word	0x00016898
   113a0:	00018acf 	.word	0x00018acf
   113a4:	3c00d264 	.word	0x3c00d264
   113a8:	3c00d260 	.word	0x3c00d260
   113ac:	00018ae7 	.word	0x00018ae7
   113b0:	00015fc1 	.word	0x00015fc1
   113b4:	00018af7 	.word	0x00018af7
   113b8:	00018b0e 	.word	0x00018b0e
   113bc:	00018b1c 	.word	0x00018b1c
   113c0:	00018b35 	.word	0x00018b35
   113c4:	00018b4a 	.word	0x00018b4a
   113c8:	00018b53 	.word	0x00018b53
   113cc:	00018b5e 	.word	0x00018b5e
   113d0:	00018b76 	.word	0x00018b76
   113d4:	00018b91 	.word	0x00018b91
   113d8:	00018b9f 	.word	0x00018b9f
   113dc:	00018ba7 	.word	0x00018ba7
   113e0:	00018bbb 	.word	0x00018bbb
   113e4:	00018bde 	.word	0x00018bde

000113e8 <pp_list>:
   113e8:	00000994 	.word	0x00000994
   113ec:	00003771 	.word	0x00003771
   113f0:	00003739 	.word	0x00003739
   113f4:	00003cb9 	.word	0x00003cb9
   113f8:	000029a0 	.word	0x000029a0
   113fc:	00003f91 	.word	0x00003f91
   11400:	000031e1 	.word	0x000031e1
   11404:	00002e6b 	.word	0x00002e6b
   11408:	0000275d 	.word	0x0000275d
   1140c:	00002297 	.word	0x00002297
   11410:	00002378 	.word	0x00002378
   11414:	000034db 	.word	0x000034db
   11418:	000029fc 	.word	0x000029fc
   1141c:	0000271c 	.word	0x0000271c
   11420:	000036ec 	.word	0x000036ec
   11424:	00003ab9 	.word	0x00003ab9
   11428:	00003218 	.word	0x00003218
   1142c:	0000307f 	.word	0x0000307f
   11430:	0000267b 	.word	0x0000267b
   11434:	00003d95 	.word	0x00003d95
   11438:	00003e0f 	.word	0x00003e0f
   1143c:	00002d75 	.word	0x00002d75
   11440:	000031a3 	.word	0x000031a3

00011444 <print_size>:
   11444:	e3700107 	cmn	r0, #-1073741823	; 0xc0000001
   11448:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   1144c:	e1a05000 	mov	r5, r0
   11450:	e1a08001 	mov	r8, r1
   11454:	83a04101 	movhi	r4, #1073741824	; 0x40000000
   11458:	83a07047 	movhi	r7, #71	; 0x47
   1145c:	8a000005 	bhi	11478 <print_size+0x34>
   11460:	e59f30a8 	ldr	r3, [pc, #168]	; 11510 <print_size+0xcc>
   11464:	e1500003 	cmp	r0, r3
   11468:	83a0704d 	movhi	r7, #77	; 0x4d
   1146c:	93a0706b 	movls	r7, #107	; 0x6b
   11470:	83a04601 	movhi	r4, #1048576	; 0x100000
   11474:	93a04b01 	movls	r4, #1024	; 0x400
   11478:	e1a01004 	mov	r1, r4
   1147c:	e1a00005 	mov	r0, r5
   11480:	eb000cc3 	bl	14794 <__aeabi_uidiv>
   11484:	e1a01004 	mov	r1, r4
   11488:	e1a06000 	mov	r6, r0
   1148c:	e1a00005 	mov	r0, r5
   11490:	eb000d3b 	bl	14984 <__aeabi_uidivmod>
   11494:	e2510000 	subs	r0, r1, #0	; 0x0
   11498:	01a04000 	moveq	r4, r0
   1149c:	0a000009 	beq	114c8 <print_size+0x84>
   114a0:	e0625694 	mls	r2, r4, r6, r5
   114a4:	e1a030a4 	lsr	r3, r4, #1
   114a8:	e3a0000a 	mov	r0, #10	; 0xa
   114ac:	e1a01004 	mov	r1, r4
   114b0:	e0203290 	mla	r0, r0, r2, r3
   114b4:	eb000cb6 	bl	14794 <__aeabi_uidiv>
   114b8:	e3500009 	cmp	r0, #9	; 0x9
   114bc:	e1a04000 	mov	r4, r0
   114c0:	82866001 	addhi	r6, r6, #1	; 0x1
   114c4:	8240400a 	subhi	r4, r0, #10	; 0xa
   114c8:	e1a01006 	mov	r1, r6
   114cc:	e59f5040 	ldr	r5, [pc, #64]	; 11514 <print_size+0xd0>
   114d0:	e59f0040 	ldr	r0, [pc, #64]	; 11518 <print_size+0xd4>
   114d4:	e1a0e00f 	mov	lr, pc
   114d8:	e595f000 	ldr	pc, [r5]
   114dc:	e3540000 	cmp	r4, #0	; 0x0
   114e0:	0a000003 	beq	114f4 <print_size+0xb0>
   114e4:	e1a01004 	mov	r1, r4
   114e8:	e59f002c 	ldr	r0, [pc, #44]	; 1151c <print_size+0xd8>
   114ec:	e1a0e00f 	mov	lr, pc
   114f0:	e595f000 	ldr	pc, [r5]
   114f4:	e1a01007 	mov	r1, r7
   114f8:	e1a02008 	mov	r2, r8
   114fc:	e59f001c 	ldr	r0, [pc, #28]	; 11520 <print_size+0xdc>
   11500:	e59f300c 	ldr	r3, [pc, #12]	; 11514 <print_size+0xd0>
   11504:	e1a0e00f 	mov	lr, pc
   11508:	e593f000 	ldr	pc, [r3]
   1150c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   11510:	000fffff 	.word	0x000fffff
   11514:	3c00d6cc 	.word	0x3c00d6cc
   11518:	00018be6 	.word	0x00018be6
   1151c:	00018beb 	.word	0x00018beb
   11520:	00018bf0 	.word	0x00018bf0

00011524 <print_buffer>:
   11524:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   11528:	e24dd044 	sub	sp, sp, #68	; 0x44
   1152c:	e59d6068 	ldr	r6, [sp, #104]
   11530:	e1a07002 	mov	r7, r2
   11534:	e0020692 	mul	r2, r2, r6
   11538:	e3520040 	cmp	r2, #64	; 0x40
   1153c:	e1a09000 	mov	r9, r0
   11540:	e1a0a001 	mov	sl, r1
   11544:	e1a08003 	mov	r8, r3
   11548:	9a000003 	bls	1155c <print_buffer+0x38>
   1154c:	e3a00040 	mov	r0, #64	; 0x40
   11550:	e1a01007 	mov	r1, r7
   11554:	eb000c8e 	bl	14794 <__aeabi_uidiv>
   11558:	e1a06000 	mov	r6, r0
   1155c:	e3560000 	cmp	r6, #0	; 0x0
   11560:	1a00003c 	bne	11658 <print_buffer+0x134>
   11564:	e3a00010 	mov	r0, #16	; 0x10
   11568:	e1a01007 	mov	r1, r7
   1156c:	eb000c88 	bl	14794 <__aeabi_uidiv>
   11570:	e1a06000 	mov	r6, r0
   11574:	ea000037 	b	11658 <print_buffer+0x134>
   11578:	e59f00f0 	ldr	r0, [pc, #240]	; 11670 <print_buffer+0x14c>
   1157c:	e1a01009 	mov	r1, r9
   11580:	e1a0e00f 	mov	lr, pc
   11584:	e59bf000 	ldr	pc, [fp]
   11588:	e1560008 	cmp	r6, r8
   1158c:	21a06008 	movcs	r6, r8
   11590:	e1a0500a 	mov	r5, sl
   11594:	e3a04000 	mov	r4, #0	; 0x0
   11598:	ea00000f 	b	115dc <print_buffer+0xb8>
   1159c:	e3570004 	cmp	r7, #4	; 0x4
   115a0:	05951000 	ldreq	r1, [r5]
   115a4:	059f00c8 	ldreq	r0, [pc, #200]	; 11674 <print_buffer+0x150>
   115a8:	078d1104 	streq	r1, [sp, r4, lsl #2]
   115ac:	0a000006 	beq	115cc <print_buffer+0xa8>
   115b0:	e3570002 	cmp	r7, #2	; 0x2
   115b4:	01d510b0 	ldrheq	r1, [r5]
   115b8:	15d51000 	ldrbne	r1, [r5]
   115bc:	01a03084 	lsleq	r3, r4, #1
   115c0:	059f00b0 	ldreq	r0, [pc, #176]	; 11678 <print_buffer+0x154>
   115c4:	018d10b3 	strheq	r1, [sp, r3]
   115c8:	17cd1004 	strbne	r1, [sp, r4]
   115cc:	e1a0e00f 	mov	lr, pc
   115d0:	e59bf000 	ldr	pc, [fp]
   115d4:	e0855007 	add	r5, r5, r7
   115d8:	e2844001 	add	r4, r4, #1	; 0x1
   115dc:	e1540006 	cmp	r4, r6
   115e0:	e59f0094 	ldr	r0, [pc, #148]	; 1167c <print_buffer+0x158>
   115e4:	3affffec 	bcc	1159c <print_buffer+0x78>
   115e8:	e0050697 	mul	r5, r7, r6
   115ec:	e59f008c 	ldr	r0, [pc, #140]	; 11680 <print_buffer+0x15c>
   115f0:	e08aa005 	add	sl, sl, r5
   115f4:	eb000450 	bl	1273c <puts>
   115f8:	e3a04000 	mov	r4, #0	; 0x0
   115fc:	ea000009 	b	11628 <print_buffer+0x104>
   11600:	e7dd0004 	ldrb	r0, [sp, r4]
   11604:	e59f2078 	ldr	r2, [pc, #120]	; 11684 <print_buffer+0x160>
   11608:	e2844001 	add	r4, r4, #1	; 0x1
   1160c:	e7d23000 	ldrb	r3, [r2, r0]
   11610:	e3130097 	tst	r3, #151	; 0x97
   11614:	0a000001 	beq	11620 <print_buffer+0xfc>
   11618:	e3100080 	tst	r0, #128	; 0x80
   1161c:	0a000000 	beq	11624 <print_buffer+0x100>
   11620:	e3a0002e 	mov	r0, #46	; 0x2e
   11624:	eb00046c 	bl	127dc <putc>
   11628:	e1540005 	cmp	r4, r5
   1162c:	3afffff3 	bcc	11600 <print_buffer+0xdc>
   11630:	e59f0050 	ldr	r0, [pc, #80]	; 11688 <print_buffer+0x164>
   11634:	e1a0e00f 	mov	lr, pc
   11638:	e59bf000 	ldr	pc, [fp]
   1163c:	eb000469 	bl	127e8 <ctrlc>
   11640:	e3500000 	cmp	r0, #0	; 0x0
   11644:	13e00000 	mvnne	r0, #0	; 0x0
   11648:	1a000006 	bne	11668 <print_buffer+0x144>
   1164c:	e0899005 	add	r9, r9, r5
   11650:	e0668008 	rsb	r8, r6, r8
   11654:	ea000000 	b	1165c <print_buffer+0x138>
   11658:	e59fb02c 	ldr	fp, [pc, #44]	; 1168c <print_buffer+0x168>
   1165c:	e3580000 	cmp	r8, #0	; 0x0
   11660:	1affffc4 	bne	11578 <print_buffer+0x54>
   11664:	e1a00008 	mov	r0, r8
   11668:	e28dd044 	add	sp, sp, #68	; 0x44
   1166c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   11670:	00018bf7 	.word	0x00018bf7
   11674:	00018037 	.word	0x00018037
   11678:	00018bfe 	.word	0x00018bfe
   1167c:	00018c04 	.word	0x00018c04
   11680:	00018c0a 	.word	0x00018c0a
   11684:	00015feb 	.word	0x00015feb
   11688:	00016bd7 	.word	0x00016bd7
   1168c:	3c00d6cc 	.word	0x3c00d6cc

00011690 <__div64_32>:
   11690:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
   11694:	e8900300 	ldm	r0, {r8, r9}
   11698:	e1510009 	cmp	r1, r9
   1169c:	e1a0a000 	mov	sl, r0
   116a0:	e1a04001 	mov	r4, r1
   116a4:	83a06000 	movhi	r6, #0	; 0x0
   116a8:	83a07000 	movhi	r7, #0	; 0x0
   116ac:	8a000007 	bhi	116d0 <__div64_32+0x40>
   116b0:	e1a00009 	mov	r0, r9
   116b4:	eb000c36 	bl	14794 <__aeabi_uidiv>
   116b8:	e0030094 	mul	r3, r4, r0
   116bc:	e3a06000 	mov	r6, #0	; 0x0
   116c0:	e1a02006 	mov	r2, r6
   116c4:	e1a07000 	mov	r7, r0
   116c8:	e0588002 	subs	r8, r8, r2
   116cc:	e0c99003 	sbc	r9, r9, r3
   116d0:	e1a00004 	mov	r0, r4
   116d4:	e3a01000 	mov	r1, #0	; 0x0
   116d8:	e3a05000 	mov	r5, #0	; 0x0
   116dc:	e3a04001 	mov	r4, #1	; 0x1
   116e0:	ea000003 	b	116f4 <__div64_32+0x64>
   116e4:	e0900000 	adds	r0, r0, r0
   116e8:	e0a11001 	adc	r1, r1, r1
   116ec:	e0944004 	adds	r4, r4, r4
   116f0:	e0a55005 	adc	r5, r5, r5
   116f4:	e3510000 	cmp	r1, #0	; 0x0
   116f8:	e3a03000 	mov	r3, #0	; 0x0
   116fc:	ca000002 	bgt	1170c <__div64_32+0x7c>
   11700:	1a000002 	bne	11710 <__div64_32+0x80>
   11704:	e3500000 	cmp	r0, #0	; 0x0
   11708:	0a000000 	beq	11710 <__div64_32+0x80>
   1170c:	e3a03001 	mov	r3, #1	; 0x1
   11710:	e1590001 	cmp	r9, r1
   11714:	e3a02000 	mov	r2, #0	; 0x0
   11718:	8a000002 	bhi	11728 <__div64_32+0x98>
   1171c:	1a000002 	bne	1172c <__div64_32+0x9c>
   11720:	e1580000 	cmp	r8, r0
   11724:	9a000000 	bls	1172c <__div64_32+0x9c>
   11728:	e3a02001 	mov	r2, #1	; 0x1
   1172c:	e0033002 	and	r3, r3, r2
   11730:	e31300ff 	tst	r3, #255	; 0xff
   11734:	1affffea 	bne	116e4 <__div64_32+0x54>
   11738:	e1510009 	cmp	r1, r9
   1173c:	8a000006 	bhi	1175c <__div64_32+0xcc>
   11740:	1a000001 	bne	1174c <__div64_32+0xbc>
   11744:	e1500008 	cmp	r0, r8
   11748:	8a000003 	bhi	1175c <__div64_32+0xcc>
   1174c:	e0588000 	subs	r8, r8, r0
   11750:	e0c99001 	sbc	r9, r9, r1
   11754:	e0966004 	adds	r6, r6, r4
   11758:	e0a77005 	adc	r7, r7, r5
   1175c:	e1b050a5 	lsrs	r5, r5, #1
   11760:	e1a04064 	rrx	r4, r4
   11764:	e1943005 	orrs	r3, r4, r5
   11768:	0a000002 	beq	11778 <__div64_32+0xe8>
   1176c:	e1b010a1 	lsrs	r1, r1, #1
   11770:	e1a00060 	rrx	r0, r0
   11774:	eaffffef 	b	11738 <__div64_32+0xa8>
   11778:	e88a00c0 	stm	sl, {r6, r7}
   1177c:	e1a00008 	mov	r0, r8
   11780:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

00011784 <simple_strtoul>:
   11784:	e92d4070 	push	{r4, r5, r6, lr}
   11788:	e5d03000 	ldrb	r3, [r0]
   1178c:	e1a05001 	mov	r5, r1
   11790:	e3530030 	cmp	r3, #48	; 0x30
   11794:	e1a01002 	mov	r1, r2
   11798:	1a00000d 	bne	117d4 <simple_strtoul+0x50>
   1179c:	e5f03001 	ldrb	r3, [r0, #1]!
   117a0:	e3530078 	cmp	r3, #120	; 0x78
   117a4:	1a000007 	bne	117c8 <simple_strtoul+0x44>
   117a8:	e5d03001 	ldrb	r3, [r0, #1]
   117ac:	e59f2084 	ldr	r2, [pc, #132]	; 11838 <simple_strtoul+0xb4>
   117b0:	e280c001 	add	ip, r0, #1	; 0x1
   117b4:	e7d23003 	ldrb	r3, [r2, r3]
   117b8:	e3130044 	tst	r3, #68	; 0x44
   117bc:	11a0000c 	movne	r0, ip
   117c0:	13a01010 	movne	r1, #16	; 0x10
   117c4:	1a000004 	bne	117dc <simple_strtoul+0x58>
   117c8:	e3510000 	cmp	r1, #0	; 0x0
   117cc:	03a01008 	moveq	r1, #8	; 0x8
   117d0:	ea000001 	b	117dc <simple_strtoul+0x58>
   117d4:	e3520000 	cmp	r2, #0	; 0x0
   117d8:	03a0100a 	moveq	r1, #10	; 0xa
   117dc:	e3a04000 	mov	r4, #0	; 0x0
   117e0:	e59f6050 	ldr	r6, [pc, #80]	; 11838 <simple_strtoul+0xb4>
   117e4:	ea000001 	b	117f0 <simple_strtoul+0x6c>
   117e8:	e0243491 	mla	r4, r1, r4, r3
   117ec:	e2800001 	add	r0, r0, #1	; 0x1
   117f0:	e5d0c000 	ldrb	ip, [r0]
   117f4:	e7d6200c 	ldrb	r2, [r6, ip]
   117f8:	e24c3030 	sub	r3, ip, #48	; 0x30
   117fc:	e3120044 	tst	r2, #68	; 0x44
   11800:	0a000008 	beq	11828 <simple_strtoul+0xa4>
   11804:	e3120004 	tst	r2, #4	; 0x4
   11808:	1a000004 	bne	11820 <simple_strtoul+0x9c>
   1180c:	e24c3020 	sub	r3, ip, #32	; 0x20
   11810:	e3120002 	tst	r2, #2	; 0x2
   11814:	e6ef3073 	uxtb	r3, r3
   11818:	12433037 	subne	r3, r3, #55	; 0x37
   1181c:	024c3037 	subeq	r3, ip, #55	; 0x37
   11820:	e1530001 	cmp	r3, r1
   11824:	3affffef 	bcc	117e8 <simple_strtoul+0x64>
   11828:	e3550000 	cmp	r5, #0	; 0x0
   1182c:	15850000 	strne	r0, [r5]
   11830:	e1a00004 	mov	r0, r4
   11834:	e8bd8070 	pop	{r4, r5, r6, pc}
   11838:	00015feb 	.word	0x00015feb

0001183c <simple_strtol>:
   1183c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   11840:	e5d03000 	ldrb	r3, [r0]
   11844:	e24dd004 	sub	sp, sp, #4	; 0x4
   11848:	e353002d 	cmp	r3, #45	; 0x2d
   1184c:	1a000004 	bne	11864 <simple_strtol+0x28>
   11850:	e2800001 	add	r0, r0, #1	; 0x1
   11854:	ebffffca 	bl	11784 <simple_strtoul>
   11858:	e2600000 	rsb	r0, r0, #0	; 0x0
   1185c:	e28dd004 	add	sp, sp, #4	; 0x4
   11860:	e8bd8000 	pop	{pc}
   11864:	e28dd004 	add	sp, sp, #4	; 0x4
   11868:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
   1186c:	eaffffc4 	b	11784 <simple_strtoul>

00011870 <ustrtoul>:
   11870:	e92d4010 	push	{r4, lr}
   11874:	e1a04001 	mov	r4, r1
   11878:	ebffffc1 	bl	11784 <simple_strtoul>
   1187c:	e5941000 	ldr	r1, [r4]
   11880:	e5d13000 	ldrb	r3, [r1]
   11884:	e353004b 	cmp	r3, #75	; 0x4b
   11888:	0a00000a 	beq	118b8 <ustrtoul+0x48>
   1188c:	8a000002 	bhi	1189c <ustrtoul+0x2c>
   11890:	e3530047 	cmp	r3, #71	; 0x47
   11894:	18bd8010 	popne	{r4, pc}
   11898:	ea000004 	b	118b0 <ustrtoul+0x40>
   1189c:	e353004d 	cmp	r3, #77	; 0x4d
   118a0:	0a000003 	beq	118b4 <ustrtoul+0x44>
   118a4:	e353006b 	cmp	r3, #107	; 0x6b
   118a8:	18bd8010 	popne	{r4, pc}
   118ac:	ea000001 	b	118b8 <ustrtoul+0x48>
   118b0:	e1a00500 	lsl	r0, r0, #10
   118b4:	e1a00500 	lsl	r0, r0, #10
   118b8:	e5d13001 	ldrb	r3, [r1, #1]
   118bc:	e1a00500 	lsl	r0, r0, #10
   118c0:	e3530069 	cmp	r3, #105	; 0x69
   118c4:	18bd8010 	popne	{r4, pc}
   118c8:	e5d13002 	ldrb	r3, [r1, #2]
   118cc:	e2812002 	add	r2, r1, #2	; 0x2
   118d0:	e3530042 	cmp	r3, #66	; 0x42
   118d4:	02813003 	addeq	r3, r1, #3	; 0x3
   118d8:	05843000 	streq	r3, [r4]
   118dc:	15842000 	strne	r2, [r4]
   118e0:	e8bd8010 	pop	{r4, pc}

000118e4 <simple_strtoull>:
   118e4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
   118e8:	e5d03000 	ldrb	r3, [r0]
   118ec:	e1a07000 	mov	r7, r0
   118f0:	e3530030 	cmp	r3, #48	; 0x30
   118f4:	e1a08001 	mov	r8, r1
   118f8:	e1a06002 	mov	r6, r2
   118fc:	1a00000d 	bne	11938 <simple_strtoull+0x54>
   11900:	e5f73001 	ldrb	r3, [r7, #1]!
   11904:	e3530078 	cmp	r3, #120	; 0x78
   11908:	1a000007 	bne	1192c <simple_strtoull+0x48>
   1190c:	e5d73001 	ldrb	r3, [r7, #1]
   11910:	e59f20ac 	ldr	r2, [pc, #172]	; 119c4 <simple_strtoull+0xe0>
   11914:	e2870001 	add	r0, r7, #1	; 0x1
   11918:	e7d23003 	ldrb	r3, [r2, r3]
   1191c:	e3130044 	tst	r3, #68	; 0x44
   11920:	11a07000 	movne	r7, r0
   11924:	13a06010 	movne	r6, #16	; 0x10
   11928:	1a000004 	bne	11940 <simple_strtoull+0x5c>
   1192c:	e3560000 	cmp	r6, #0	; 0x0
   11930:	03a06008 	moveq	r6, #8	; 0x8
   11934:	ea000001 	b	11940 <simple_strtoull+0x5c>
   11938:	e3520000 	cmp	r2, #0	; 0x0
   1193c:	03a0600a 	moveq	r6, #10	; 0xa
   11940:	e3a04000 	mov	r4, #0	; 0x0
   11944:	e3a05000 	mov	r5, #0	; 0x0
   11948:	e59f9074 	ldr	r9, [pc, #116]	; 119c4 <simple_strtoull+0xe0>
   1194c:	e3a0a000 	mov	sl, #0	; 0x0
   11950:	ea000004 	b	11968 <simple_strtoull+0x84>
   11954:	e0832694 	umull	r2, r3, r4, r6
   11958:	e0233596 	mla	r3, r6, r5, r3
   1195c:	e0904002 	adds	r4, r0, r2
   11960:	e0a15003 	adc	r5, r1, r3
   11964:	e2877001 	add	r7, r7, #1	; 0x1
   11968:	e5d7c000 	ldrb	ip, [r7]
   1196c:	e7d9200c 	ldrb	r2, [r9, ip]
   11970:	e24c3030 	sub	r3, ip, #48	; 0x30
   11974:	e3120044 	tst	r2, #68	; 0x44
   11978:	0a00000c 	beq	119b0 <simple_strtoull+0xcc>
   1197c:	e3120004 	tst	r2, #4	; 0x4
   11980:	1a000004 	bne	11998 <simple_strtoull+0xb4>
   11984:	e24c3020 	sub	r3, ip, #32	; 0x20
   11988:	e3120002 	tst	r2, #2	; 0x2
   1198c:	e6ef3073 	uxtb	r3, r3
   11990:	12433037 	subne	r3, r3, #55	; 0x37
   11994:	024c3037 	subeq	r3, ip, #55	; 0x37
   11998:	e1a00003 	mov	r0, r3
   1199c:	e1a01fc0 	asr	r1, r0, #31
   119a0:	e15a0001 	cmp	sl, r1
   119a4:	1a000001 	bne	119b0 <simple_strtoull+0xcc>
   119a8:	e1560000 	cmp	r6, r0
   119ac:	8affffe8 	bhi	11954 <simple_strtoull+0x70>
   119b0:	e3580000 	cmp	r8, #0	; 0x0
   119b4:	15887000 	strne	r7, [r8]
   119b8:	e1a01005 	mov	r1, r5
   119bc:	e1a00004 	mov	r0, r4
   119c0:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
   119c4:	00015feb 	.word	0x00015feb

000119c8 <put_dec_trunc>:
   119c8:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   119cc:	e1a03421 	lsr	r3, r1, #8
   119d0:	e1a04221 	lsr	r4, r1, #4
   119d4:	e204400f 	and	r4, r4, #15	; 0xf
   119d8:	e203600f 	and	r6, r3, #15	; 0xf
   119dc:	e1a05621 	lsr	r5, r1, #12
   119e0:	e0842006 	add	r2, r4, r6
   119e4:	e201100f 	and	r1, r1, #15	; 0xf
   119e8:	e3a03006 	mov	r3, #6	; 0x6
   119ec:	e0822005 	add	r2, r2, r5
   119f0:	e0221293 	mla	r2, r3, r2, r1
   119f4:	e3a080cd 	mov	r8, #205	; 0xcd
   119f8:	e00c0298 	mul	ip, r8, r2
   119fc:	e3a01009 	mov	r1, #9	; 0x9
   11a00:	e0214591 	mla	r1, r1, r5, r4
   11a04:	e1a0c5ac 	lsr	ip, ip, #11
   11a08:	e3a0700a 	mov	r7, #10	; 0xa
   11a0c:	e3a03005 	mov	r3, #5	; 0x5
   11a10:	e0231693 	mla	r3, r3, r6, r1
   11a14:	e0622c97 	mls	r2, r7, ip, r2
   11a18:	e093100c 	adds	r1, r3, ip
   11a1c:	e2822030 	add	r2, r2, #48	; 0x30
   11a20:	e1a04000 	mov	r4, r0
   11a24:	e4c02001 	strb	r2, [r0], #1
   11a28:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
   11a2c:	e0030198 	mul	r3, r8, r1
   11a30:	e1a035a3 	lsr	r3, r3, #11
   11a34:	e0621397 	mls	r2, r7, r3, r1
   11a38:	e0831086 	add	r1, r3, r6, lsl #1
   11a3c:	e2822030 	add	r2, r2, #48	; 0x30
   11a40:	e1913005 	orrs	r3, r1, r5
   11a44:	e5c42001 	strb	r2, [r4, #1]
   11a48:	e2800001 	add	r0, r0, #1	; 0x1
   11a4c:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
   11a50:	e3a0300d 	mov	r3, #13	; 0xd
   11a54:	e0030193 	mul	r3, r3, r1
   11a58:	e1a033a3 	lsr	r3, r3, #7
   11a5c:	e0621397 	mls	r2, r7, r3, r1
   11a60:	e0931105 	adds	r1, r3, r5, lsl #2
   11a64:	e2822030 	add	r2, r2, #48	; 0x30
   11a68:	e4c02001 	strb	r2, [r0], #1
   11a6c:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
   11a70:	e0030198 	mul	r3, r8, r1
   11a74:	e1a0c5a3 	lsr	ip, r3, #11
   11a78:	e0631c97 	mls	r3, r7, ip, r1
   11a7c:	e35c0000 	cmp	ip, #0	; 0x0
   11a80:	e2833030 	add	r3, r3, #48	; 0x30
   11a84:	e4c03001 	strb	r3, [r0], #1
   11a88:	128c3030 	addne	r3, ip, #48	; 0x30
   11a8c:	14c03001 	strbne	r3, [r0], #1
   11a90:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

00011a94 <string>:
   11a94:	e59fc0a4 	ldr	ip, [pc, #164]	; 11b40 <string+0xac>
   11a98:	e3510000 	cmp	r1, #0	; 0x0
   11a9c:	e92d4070 	push	{r4, r5, r6, lr}
   11aa0:	11a06001 	movne	r6, r1
   11aa4:	01a0600c 	moveq	r6, ip
   11aa8:	e1a01003 	mov	r1, r3
   11aac:	e1a04000 	mov	r4, r0
   11ab0:	e1a00006 	mov	r0, r6
   11ab4:	e1a05002 	mov	r5, r2
   11ab8:	ebffbe95 	bl	1514 <strnlen>
   11abc:	e59d3010 	ldr	r3, [sp, #16]
   11ac0:	e1a0c000 	mov	ip, r0
   11ac4:	e3130010 	tst	r3, #16	; 0x10
   11ac8:	01a02004 	moveq	r2, r4
   11acc:	03a01020 	moveq	r1, #32	; 0x20
   11ad0:	0a000001 	beq	11adc <string+0x48>
   11ad4:	ea000006 	b	11af4 <string+0x60>
   11ad8:	e5421001 	strb	r1, [r2, #-1]
   11adc:	e2455001 	sub	r5, r5, #1	; 0x1
   11ae0:	e2853001 	add	r3, r5, #1	; 0x1
   11ae4:	e15c0003 	cmp	ip, r3
   11ae8:	e1a04002 	mov	r4, r2
   11aec:	e2822001 	add	r2, r2, #1	; 0x1
   11af0:	bafffff8 	blt	11ad8 <string+0x44>
   11af4:	e3a02000 	mov	r2, #0	; 0x0
   11af8:	ea000002 	b	11b08 <string+0x74>
   11afc:	e7d63002 	ldrb	r3, [r6, r2]
   11b00:	e7c43002 	strb	r3, [r4, r2]
   11b04:	e2822001 	add	r2, r2, #1	; 0x1
   11b08:	e152000c 	cmp	r2, ip
   11b0c:	e0843002 	add	r3, r4, r2
   11b10:	bafffff9 	blt	11afc <string+0x68>
   11b14:	e1a01003 	mov	r1, r3
   11b18:	e1a02005 	mov	r2, r5
   11b1c:	e3a03020 	mov	r3, #32	; 0x20
   11b20:	ea000000 	b	11b28 <string+0x94>
   11b24:	e5413001 	strb	r3, [r1, #-1]
   11b28:	e15c0002 	cmp	ip, r2
   11b2c:	e1a00001 	mov	r0, r1
   11b30:	e2422001 	sub	r2, r2, #1	; 0x1
   11b34:	e2811001 	add	r1, r1, #1	; 0x1
   11b38:	bafffff9 	blt	11b24 <string+0x90>
   11b3c:	e8bd8070 	pop	{r4, r5, r6, pc}
   11b40:	00018c0f 	.word	0x00018c0f

00011b44 <put_dec>:
   11b44:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   11b48:	e24dd00c 	sub	sp, sp, #12	; 0xc
   11b4c:	e28d8008 	add	r8, sp, #8	; 0x8
   11b50:	e16820f8 	strd	r2, [r8, #-8]!
   11b54:	e1a07000 	mov	r7, r0
   11b58:	e3a090cd 	mov	r9, #205	; 0xcd
   11b5c:	e3a0a00a 	mov	sl, #10	; 0xa
   11b60:	e3a0b00d 	mov	fp, #13	; 0xd
   11b64:	e59d5004 	ldr	r5, [sp, #4]
   11b68:	e1a0000d 	mov	r0, sp
   11b6c:	e3550000 	cmp	r5, #0	; 0x0
   11b70:	e59f10f0 	ldr	r1, [pc, #240]	; 11c68 <put_dec+0x124>
   11b74:	e59d4000 	ldr	r4, [sp]
   11b78:	1a00000f 	bne	11bbc <put_dec+0x78>
   11b7c:	e59f30e8 	ldr	r3, [pc, #232]	; 11c6c <put_dec+0x128>
   11b80:	e1a00004 	mov	r0, r4
   11b84:	e1540003 	cmp	r4, r3
   11b88:	8a000004 	bhi	11ba0 <put_dec+0x5c>
   11b8c:	e1a00007 	mov	r0, r7
   11b90:	e1a01004 	mov	r1, r4
   11b94:	ebffff8b 	bl	119c8 <put_dec_trunc>
   11b98:	e28dd00c 	add	sp, sp, #12	; 0xc
   11b9c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   11ba0:	eb000b77 	bl	14984 <__aeabi_uidivmod>
   11ba4:	e1a00004 	mov	r0, r4
   11ba8:	e1a06001 	mov	r6, r1
   11bac:	e59f10b4 	ldr	r1, [pc, #180]	; 11c68 <put_dec+0x124>
   11bb0:	eb000af7 	bl	14794 <__aeabi_uidiv>
   11bb4:	e88d0021 	stm	sp, {r0, r5}
   11bb8:	ea000001 	b	11bc4 <put_dec+0x80>
   11bbc:	ebfffeb3 	bl	11690 <__div64_32>
   11bc0:	e1a06000 	mov	r6, r0
   11bc4:	e1a02226 	lsr	r2, r6, #4
   11bc8:	e1a0c426 	lsr	ip, r6, #8
   11bcc:	e20cc00f 	and	ip, ip, #15	; 0xf
   11bd0:	e202200f 	and	r2, r2, #15	; 0xf
   11bd4:	e1a0e626 	lsr	lr, r6, #12
   11bd8:	e082500c 	add	r5, r2, ip
   11bdc:	e085500e 	add	r5, r5, lr
   11be0:	e206300f 	and	r3, r6, #15	; 0xf
   11be4:	e3a01006 	mov	r1, #6	; 0x6
   11be8:	e0253591 	mla	r5, r1, r5, r3
   11bec:	e3a03009 	mov	r3, #9	; 0x9
   11bf0:	e0222e93 	mla	r2, r3, lr, r2
   11bf4:	e3a01005 	mov	r1, #5	; 0x5
   11bf8:	e0000599 	mul	r0, r9, r5
   11bfc:	e0222c91 	mla	r2, r1, ip, r2
   11c00:	e1a005a0 	lsr	r0, r0, #11
   11c04:	e0822000 	add	r2, r2, r0
   11c08:	e0010299 	mul	r1, r9, r2
   11c0c:	e1a015a1 	lsr	r1, r1, #11
   11c10:	e081c08c 	add	ip, r1, ip, lsl #1
   11c14:	e0030c9b 	mul	r3, fp, ip
   11c18:	e1a033a3 	lsr	r3, r3, #7
   11c1c:	e083e10e 	add	lr, r3, lr, lsl #2
   11c20:	e0040e99 	mul	r4, r9, lr
   11c24:	e1a045a4 	lsr	r4, r4, #11
   11c28:	e06ee49a 	mls	lr, sl, r4, lr
   11c2c:	e060509a 	mls	r0, sl, r0, r5
   11c30:	e061219a 	mls	r1, sl, r1, r2
   11c34:	e063c39a 	mls	r3, sl, r3, ip
   11c38:	e2800030 	add	r0, r0, #48	; 0x30
   11c3c:	e2811030 	add	r1, r1, #48	; 0x30
   11c40:	e2833030 	add	r3, r3, #48	; 0x30
   11c44:	e28ee030 	add	lr, lr, #48	; 0x30
   11c48:	e2844030 	add	r4, r4, #48	; 0x30
   11c4c:	e5c70000 	strb	r0, [r7]
   11c50:	e5c71001 	strb	r1, [r7, #1]
   11c54:	e5c73002 	strb	r3, [r7, #2]
   11c58:	e5c7e003 	strb	lr, [r7, #3]
   11c5c:	e5c74004 	strb	r4, [r7, #4]
   11c60:	e2877005 	add	r7, r7, #5	; 0x5
   11c64:	eaffffbe 	b	11b64 <put_dec+0x20>
   11c68:	000186a0 	.word	0x000186a0
   11c6c:	0001869f 	.word	0x0001869f

00011c70 <number>:
   11c70:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   11c74:	e24dd05c 	sub	sp, sp, #92	; 0x5c
   11c78:	e1a0a000 	mov	sl, r0
   11c7c:	e59d0080 	ldr	r0, [sp, #128]
   11c80:	e59d808c 	ldr	r8, [sp, #140]
   11c84:	e250100a 	subs	r1, r0, #10	; 0xa
   11c88:	13a01001 	movne	r1, #1	; 0x1
   11c8c:	e0011328 	and	r1, r1, r8, lsr #6
   11c90:	e3180010 	tst	r8, #16	; 0x10
   11c94:	e58d1008 	str	r1, [sp, #8]
   11c98:	e2081020 	and	r1, r8, #32	; 0x20
   11c9c:	13c88001 	bicne	r8, r8, #1	; 0x1
   11ca0:	e3180002 	tst	r8, #2	; 0x2
   11ca4:	e1a06002 	mov	r6, r2
   11ca8:	e1a07003 	mov	r7, r3
   11cac:	e59d5084 	ldr	r5, [sp, #132]
   11cb0:	e58d1004 	str	r1, [sp, #4]
   11cb4:	0a000010 	beq	11cfc <number+0x8c>
   11cb8:	e3530000 	cmp	r3, #0	; 0x0
   11cbc:	ba000002 	blt	11ccc <number+0x5c>
   11cc0:	e3180004 	tst	r8, #4	; 0x4
   11cc4:	0a000008 	beq	11cec <number+0x7c>
   11cc8:	ea000004 	b	11ce0 <number+0x70>
   11ccc:	e2766000 	rsbs	r6, r6, #0	; 0x0
   11cd0:	e2e77000 	rsc	r7, r7, #0	; 0x0
   11cd4:	e2455001 	sub	r5, r5, #1	; 0x1
   11cd8:	e3a0902d 	mov	r9, #45	; 0x2d
   11cdc:	ea000007 	b	11d00 <number+0x90>
   11ce0:	e2455001 	sub	r5, r5, #1	; 0x1
   11ce4:	e3a0902b 	mov	r9, #43	; 0x2b
   11ce8:	ea000004 	b	11d00 <number+0x90>
   11cec:	e3180008 	tst	r8, #8	; 0x8
   11cf0:	12455001 	subne	r5, r5, #1	; 0x1
   11cf4:	13a09020 	movne	r9, #32	; 0x20
   11cf8:	1a000000 	bne	11d00 <number+0x90>
   11cfc:	e3a09000 	mov	r9, #0	; 0x0
   11d00:	e59d2008 	ldr	r2, [sp, #8]
   11d04:	e3520000 	cmp	r2, #0	; 0x0
   11d08:	0a000003 	beq	11d1c <number+0xac>
   11d0c:	e59d3080 	ldr	r3, [sp, #128]
   11d10:	e2455001 	sub	r5, r5, #1	; 0x1
   11d14:	e3530010 	cmp	r3, #16	; 0x10
   11d18:	02455001 	subeq	r5, r5, #1	; 0x1
   11d1c:	e196c007 	orrs	ip, r6, r7
   11d20:	03a03030 	moveq	r3, #48	; 0x30
   11d24:	05cd3016 	strbeq	r3, [sp, #22]
   11d28:	03a04001 	moveq	r4, #1	; 0x1
   11d2c:	0a000020 	beq	11db4 <number+0x144>
   11d30:	e59d0080 	ldr	r0, [sp, #128]
   11d34:	e350000a 	cmp	r0, #10	; 0xa
   11d38:	0a000017 	beq	11d9c <number+0x12c>
   11d3c:	e3500010 	cmp	r0, #16	; 0x10
   11d40:	03a01004 	moveq	r1, #4	; 0x4
   11d44:	13a01003 	movne	r1, #3	; 0x3
   11d48:	e58d100c 	str	r1, [sp, #12]
   11d4c:	e240b001 	sub	fp, r0, #1	; 0x1
   11d50:	e3a04000 	mov	r4, #0	; 0x0
   11d54:	e6ef3076 	uxtb	r3, r6
   11d58:	e59f2164 	ldr	r2, [pc, #356]	; 11ec4 <number+0x254>
   11d5c:	e003300b 	and	r3, r3, fp
   11d60:	e59dc004 	ldr	ip, [sp, #4]
   11d64:	e7d23003 	ldrb	r3, [r2, r3]
   11d68:	e1a00006 	mov	r0, r6
   11d6c:	e18c3003 	orr	r3, ip, r3
   11d70:	e28dc016 	add	ip, sp, #22	; 0x16
   11d74:	e1a01007 	mov	r1, r7
   11d78:	e59d200c 	ldr	r2, [sp, #12]
   11d7c:	e7cc3004 	strb	r3, [ip, r4]
   11d80:	eb000b93 	bl	14bd4 <__aeabi_llsr>
   11d84:	e1a06000 	mov	r6, r0
   11d88:	e1960001 	orrs	r0, r6, r1
   11d8c:	e1a07001 	mov	r7, r1
   11d90:	e2844001 	add	r4, r4, #1	; 0x1
   11d94:	1affffee 	bne	11d54 <number+0xe4>
   11d98:	ea000005 	b	11db4 <number+0x144>
   11d9c:	e28d4016 	add	r4, sp, #22	; 0x16
   11da0:	e1a00004 	mov	r0, r4
   11da4:	e1a02006 	mov	r2, r6
   11da8:	e1a03007 	mov	r3, r7
   11dac:	ebffff64 	bl	11b44 <put_dec>
   11db0:	e0644000 	rsb	r4, r4, r0
   11db4:	e59d3088 	ldr	r3, [sp, #136]
   11db8:	e1540003 	cmp	r4, r3
   11dbc:	a1a01004 	movge	r1, r4
   11dc0:	b1a01003 	movlt	r1, r3
   11dc4:	e3180011 	tst	r8, #17	; 0x11
   11dc8:	e061c005 	rsb	ip, r1, r5
   11dcc:	01a0300a 	moveq	r3, sl
   11dd0:	03a02020 	moveq	r2, #32	; 0x20
   11dd4:	0a000001 	beq	11de0 <number+0x170>
   11dd8:	ea000004 	b	11df0 <number+0x180>
   11ddc:	e5432001 	strb	r2, [r3, #-1]
   11de0:	e25cc001 	subs	ip, ip, #1	; 0x1
   11de4:	e1a0a003 	mov	sl, r3
   11de8:	e2833001 	add	r3, r3, #1	; 0x1
   11dec:	5afffffa 	bpl	11ddc <number+0x16c>
   11df0:	e3590000 	cmp	r9, #0	; 0x0
   11df4:	14ca9001 	strbne	r9, [sl], #1
   11df8:	e59d2008 	ldr	r2, [sp, #8]
   11dfc:	e3520000 	cmp	r2, #0	; 0x0
   11e00:	0a000006 	beq	11e20 <number+0x1b0>
   11e04:	e59d3080 	ldr	r3, [sp, #128]
   11e08:	e3530010 	cmp	r3, #16	; 0x10
   11e0c:	e3a03030 	mov	r3, #48	; 0x30
   11e10:	e4ca3001 	strb	r3, [sl], #1
   11e14:	059d0004 	ldreq	r0, [sp, #4]
   11e18:	03803058 	orreq	r3, r0, #88	; 0x58
   11e1c:	04ca3001 	strbeq	r3, [sl], #1
   11e20:	e3180010 	tst	r8, #16	; 0x10
   11e24:	1a000009 	bne	11e50 <number+0x1e0>
   11e28:	e3180001 	tst	r8, #1	; 0x1
   11e2c:	13a02030 	movne	r2, #48	; 0x30
   11e30:	03a02020 	moveq	r2, #32	; 0x20
   11e34:	e1a0300a 	mov	r3, sl
   11e38:	ea000000 	b	11e40 <number+0x1d0>
   11e3c:	e5432001 	strb	r2, [r3, #-1]
   11e40:	e25cc001 	subs	ip, ip, #1	; 0x1
   11e44:	e1a0a003 	mov	sl, r3
   11e48:	e2833001 	add	r3, r3, #1	; 0x1
   11e4c:	5afffffa 	bpl	11e3c <number+0x1cc>
   11e50:	e1a0000a 	mov	r0, sl
   11e54:	e3a03030 	mov	r3, #48	; 0x30
   11e58:	ea000000 	b	11e60 <number+0x1f0>
   11e5c:	e5403001 	strb	r3, [r0, #-1]
   11e60:	e2411001 	sub	r1, r1, #1	; 0x1
   11e64:	e1540001 	cmp	r4, r1
   11e68:	e1a02000 	mov	r2, r0
   11e6c:	e2800001 	add	r0, r0, #1	; 0x1
   11e70:	dafffff9 	ble	11e5c <number+0x1ec>
   11e74:	e28d1058 	add	r1, sp, #88	; 0x58
   11e78:	e0813004 	add	r3, r1, r4
   11e7c:	e2431043 	sub	r1, r3, #67	; 0x43
   11e80:	ea000001 	b	11e8c <number+0x21c>
   11e84:	e5d13001 	ldrb	r3, [r1, #1]
   11e88:	e5423001 	strb	r3, [r2, #-1]
   11e8c:	e2544001 	subs	r4, r4, #1	; 0x1
   11e90:	e1a03002 	mov	r3, r2
   11e94:	e2411001 	sub	r1, r1, #1	; 0x1
   11e98:	e2822001 	add	r2, r2, #1	; 0x1
   11e9c:	5afffff8 	bpl	11e84 <number+0x214>
   11ea0:	e3a02020 	mov	r2, #32	; 0x20
   11ea4:	ea000000 	b	11eac <number+0x23c>
   11ea8:	e5432001 	strb	r2, [r3, #-1]
   11eac:	e25cc001 	subs	ip, ip, #1	; 0x1
   11eb0:	e1a00003 	mov	r0, r3
   11eb4:	e2833001 	add	r3, r3, #1	; 0x1
   11eb8:	5afffffa 	bpl	11ea8 <number+0x238>
   11ebc:	e28dd05c 	add	sp, sp, #92	; 0x5c
   11ec0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   11ec4:	00015fdb 	.word	0x00015fdb

00011ec8 <vsprintf>:
   11ec8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   11ecc:	e24dd084 	sub	sp, sp, #132	; 0x84
   11ed0:	e58d0028 	str	r0, [sp, #40]
   11ed4:	e1a08001 	mov	r8, r1
   11ed8:	e1a0a000 	mov	sl, r0
   11edc:	e28d107b 	add	r1, sp, #123	; 0x7b
   11ee0:	e28d007d 	add	r0, sp, #125	; 0x7d
   11ee4:	e58d2024 	str	r2, [sp, #36]
   11ee8:	e1a06002 	mov	r6, r2
   11eec:	e58d001c 	str	r0, [sp, #28]
   11ef0:	e58d1018 	str	r1, [sp, #24]
   11ef4:	ea0001a3 	b	12588 <vsprintf+0x6c0>
   11ef8:	e3530025 	cmp	r3, #37	; 0x25
   11efc:	1a00015f 	bne	12480 <vsprintf+0x5b8>
   11f00:	e3a07000 	mov	r7, #0	; 0x0
   11f04:	e5f82001 	ldrb	r2, [r8, #1]!
   11f08:	e352002b 	cmp	r2, #43	; 0x2b
   11f0c:	0a00000c 	beq	11f44 <vsprintf+0x7c>
   11f10:	8a000004 	bhi	11f28 <vsprintf+0x60>
   11f14:	e3520020 	cmp	r2, #32	; 0x20
   11f18:	0a00000b 	beq	11f4c <vsprintf+0x84>
   11f1c:	e3520023 	cmp	r2, #35	; 0x23
   11f20:	1a00000f 	bne	11f64 <vsprintf+0x9c>
   11f24:	ea00000a 	b	11f54 <vsprintf+0x8c>
   11f28:	e352002d 	cmp	r2, #45	; 0x2d
   11f2c:	0a000002 	beq	11f3c <vsprintf+0x74>
   11f30:	e3520030 	cmp	r2, #48	; 0x30
   11f34:	1a00000a 	bne	11f64 <vsprintf+0x9c>
   11f38:	ea000007 	b	11f5c <vsprintf+0x94>
   11f3c:	e3877010 	orr	r7, r7, #16	; 0x10
   11f40:	eaffffef 	b	11f04 <vsprintf+0x3c>
   11f44:	e3877004 	orr	r7, r7, #4	; 0x4
   11f48:	eaffffed 	b	11f04 <vsprintf+0x3c>
   11f4c:	e3877008 	orr	r7, r7, #8	; 0x8
   11f50:	eaffffeb 	b	11f04 <vsprintf+0x3c>
   11f54:	e3877040 	orr	r7, r7, #64	; 0x40
   11f58:	eaffffe9 	b	11f04 <vsprintf+0x3c>
   11f5c:	e3877001 	orr	r7, r7, #1	; 0x1
   11f60:	eaffffe7 	b	11f04 <vsprintf+0x3c>
   11f64:	e2423030 	sub	r3, r2, #48	; 0x30
   11f68:	e6ef3073 	uxtb	r3, r3
   11f6c:	e3530009 	cmp	r3, #9	; 0x9
   11f70:	93a09000 	movls	r9, #0	; 0x0
   11f74:	93a0100a 	movls	r1, #10	; 0xa
   11f78:	9a000002 	bls	11f88 <vsprintf+0xc0>
   11f7c:	ea000007 	b	11fa0 <vsprintf+0xd8>
   11f80:	e0292991 	mla	r9, r1, r9, r2
   11f84:	e2888001 	add	r8, r8, #1	; 0x1
   11f88:	e5d83000 	ldrb	r3, [r8]
   11f8c:	e2432030 	sub	r2, r3, #48	; 0x30
   11f90:	e6ef3072 	uxtb	r3, r2
   11f94:	e3530009 	cmp	r3, #9	; 0x9
   11f98:	9afffff8 	bls	11f80 <vsprintf+0xb8>
   11f9c:	ea000008 	b	11fc4 <vsprintf+0xfc>
   11fa0:	e352002a 	cmp	r2, #42	; 0x2a
   11fa4:	13e09000 	mvnne	r9, #0	; 0x0
   11fa8:	1a000005 	bne	11fc4 <vsprintf+0xfc>
   11fac:	e5969000 	ldr	r9, [r6]
   11fb0:	e2888001 	add	r8, r8, #1	; 0x1
   11fb4:	e3590000 	cmp	r9, #0	; 0x0
   11fb8:	e2866004 	add	r6, r6, #4	; 0x4
   11fbc:	b2699000 	rsblt	r9, r9, #0	; 0x0
   11fc0:	b3877010 	orrlt	r7, r7, #16	; 0x10
   11fc4:	e5d83000 	ldrb	r3, [r8]
   11fc8:	e353002e 	cmp	r3, #46	; 0x2e
   11fcc:	13e02000 	mvnne	r2, #0	; 0x0
   11fd0:	158d202c 	strne	r2, [sp, #44]
   11fd4:	1a00001d 	bne	12050 <vsprintf+0x188>
   11fd8:	e5f82001 	ldrb	r2, [r8, #1]!
   11fdc:	e2423030 	sub	r3, r2, #48	; 0x30
   11fe0:	e6ef3073 	uxtb	r3, r3
   11fe4:	e3530009 	cmp	r3, #9	; 0x9
   11fe8:	93a03000 	movls	r3, #0	; 0x0
   11fec:	958d302c 	strls	r3, [sp, #44]
   11ff0:	93a0100a 	movls	r1, #10	; 0xa
   11ff4:	9a000004 	bls	1200c <vsprintf+0x144>
   11ff8:	ea000009 	b	12024 <vsprintf+0x15c>
   11ffc:	e59d002c 	ldr	r0, [sp, #44]
   12000:	e2888001 	add	r8, r8, #1	; 0x1
   12004:	e0202091 	mla	r0, r1, r0, r2
   12008:	e58d002c 	str	r0, [sp, #44]
   1200c:	e5d83000 	ldrb	r3, [r8]
   12010:	e2432030 	sub	r2, r3, #48	; 0x30
   12014:	e6ef3072 	uxtb	r3, r2
   12018:	e3530009 	cmp	r3, #9	; 0x9
   1201c:	9afffff6 	bls	11ffc <vsprintf+0x134>
   12020:	ea000005 	b	1203c <vsprintf+0x174>
   12024:	e352002a 	cmp	r2, #42	; 0x2a
   12028:	1a000006 	bne	12048 <vsprintf+0x180>
   1202c:	e5961000 	ldr	r1, [r6]
   12030:	e2888001 	add	r8, r8, #1	; 0x1
   12034:	e58d102c 	str	r1, [sp, #44]
   12038:	e2866004 	add	r6, r6, #4	; 0x4
   1203c:	e59d202c 	ldr	r2, [sp, #44]
   12040:	e3520000 	cmp	r2, #0	; 0x0
   12044:	aa000001 	bge	12050 <vsprintf+0x188>
   12048:	e3a03000 	mov	r3, #0	; 0x0
   1204c:	e58d302c 	str	r3, [sp, #44]
   12050:	e5d83000 	ldrb	r3, [r8]
   12054:	e353006c 	cmp	r3, #108	; 0x6c
   12058:	13530068 	cmpne	r3, #104	; 0x68
   1205c:	0a000008 	beq	12084 <vsprintf+0x1bc>
   12060:	e353004c 	cmp	r3, #76	; 0x4c
   12064:	0a000006 	beq	12084 <vsprintf+0x1bc>
   12068:	e353005a 	cmp	r3, #90	; 0x5a
   1206c:	0a000004 	beq	12084 <vsprintf+0x1bc>
   12070:	e353007a 	cmp	r3, #122	; 0x7a
   12074:	0a000002 	beq	12084 <vsprintf+0x1bc>
   12078:	e3530074 	cmp	r3, #116	; 0x74
   1207c:	13e02000 	mvnne	r2, #0	; 0x0
   12080:	1a000007 	bne	120a4 <vsprintf+0x1dc>
   12084:	e353006c 	cmp	r3, #108	; 0x6c
   12088:	e1a02003 	mov	r2, r3
   1208c:	e2888001 	add	r8, r8, #1	; 0x1
   12090:	1a000003 	bne	120a4 <vsprintf+0x1dc>
   12094:	e5d83000 	ldrb	r3, [r8]
   12098:	e353006c 	cmp	r3, #108	; 0x6c
   1209c:	02888001 	addeq	r8, r8, #1	; 0x1
   120a0:	03a0204c 	moveq	r2, #76	; 0x4c
   120a4:	e5d83000 	ldrb	r3, [r8]
   120a8:	e353006e 	cmp	r3, #110	; 0x6e
   120ac:	0a0000df 	beq	12430 <vsprintf+0x568>
   120b0:	8a00000c 	bhi	120e8 <vsprintf+0x220>
   120b4:	e3530063 	cmp	r3, #99	; 0x63
   120b8:	0a000017 	beq	1211c <vsprintf+0x254>
   120bc:	8a000004 	bhi	120d4 <vsprintf+0x20c>
   120c0:	e3530025 	cmp	r3, #37	; 0x25
   120c4:	0a0000ed 	beq	12480 <vsprintf+0x5b8>
   120c8:	e3530058 	cmp	r3, #88	; 0x58
   120cc:	1a0000e5 	bne	12468 <vsprintf+0x5a0>
   120d0:	ea0000e0 	b	12458 <vsprintf+0x590>
   120d4:	e3530064 	cmp	r3, #100	; 0x64
   120d8:	0a0000e0 	beq	12460 <vsprintf+0x598>
   120dc:	e3530069 	cmp	r3, #105	; 0x69
   120e0:	1a0000e0 	bne	12468 <vsprintf+0x5a0>
   120e4:	ea0000dd 	b	12460 <vsprintf+0x598>
   120e8:	e3530073 	cmp	r3, #115	; 0x73
   120ec:	0a000021 	beq	12178 <vsprintf+0x2b0>
   120f0:	8a000004 	bhi	12108 <vsprintf+0x240>
   120f4:	e353006f 	cmp	r3, #111	; 0x6f
   120f8:	0a0000d3 	beq	1244c <vsprintf+0x584>
   120fc:	e3530070 	cmp	r3, #112	; 0x70
   12100:	1a0000d8 	bne	12468 <vsprintf+0x5a0>
   12104:	ea000023 	b	12198 <vsprintf+0x2d0>
   12108:	e3530075 	cmp	r3, #117	; 0x75
   1210c:	0a0000dd 	beq	12488 <vsprintf+0x5c0>
   12110:	e3530078 	cmp	r3, #120	; 0x78
   12114:	1a0000d3 	bne	12468 <vsprintf+0x5a0>
   12118:	ea0000cd 	b	12454 <vsprintf+0x58c>
   1211c:	e3170010 	tst	r7, #16	; 0x10
   12120:	01a0300a 	moveq	r3, sl
   12124:	03a02020 	moveq	r2, #32	; 0x20
   12128:	0a000001 	beq	12134 <vsprintf+0x26c>
   1212c:	ea000005 	b	12148 <vsprintf+0x280>
   12130:	e5432001 	strb	r2, [r3, #-1]
   12134:	e2499001 	sub	r9, r9, #1	; 0x1
   12138:	e3590000 	cmp	r9, #0	; 0x0
   1213c:	e1a0a003 	mov	sl, r3
   12140:	e2833001 	add	r3, r3, #1	; 0x1
   12144:	cafffff9 	bgt	12130 <vsprintf+0x268>
   12148:	e5963000 	ldr	r3, [r6]
   1214c:	e1a0200a 	mov	r2, sl
   12150:	e4c23001 	strb	r3, [r2], #1
   12154:	e3a03020 	mov	r3, #32	; 0x20
   12158:	ea000000 	b	12160 <vsprintf+0x298>
   1215c:	e5423001 	strb	r3, [r2, #-1]
   12160:	e2499001 	sub	r9, r9, #1	; 0x1
   12164:	e3590000 	cmp	r9, #0	; 0x0
   12168:	e1a0a002 	mov	sl, r2
   1216c:	e2822001 	add	r2, r2, #1	; 0x1
   12170:	cafffff9 	bgt	1215c <vsprintf+0x294>
   12174:	ea0000ab 	b	12428 <vsprintf+0x560>
   12178:	e58d7000 	str	r7, [sp]
   1217c:	e1a0000a 	mov	r0, sl
   12180:	e5961000 	ldr	r1, [r6]
   12184:	e1a02009 	mov	r2, r9
   12188:	e59d302c 	ldr	r3, [sp, #44]
   1218c:	ebfffe40 	bl	11a94 <string>
   12190:	e1a0a000 	mov	sl, r0
   12194:	ea0000a3 	b	12428 <vsprintf+0x560>
   12198:	e596b000 	ldr	fp, [r6]
   1219c:	e35b0000 	cmp	fp, #0	; 0x0
   121a0:	1a000005 	bne	121bc <vsprintf+0x2f4>
   121a4:	e1a0000a 	mov	r0, sl
   121a8:	e1a02009 	mov	r2, r9
   121ac:	e59d302c 	ldr	r3, [sp, #44]
   121b0:	e59f13f0 	ldr	r1, [pc, #1008]	; 125a8 <vsprintf+0x6e0>
   121b4:	e58d7000 	str	r7, [sp]
   121b8:	ea000081 	b	123c4 <vsprintf+0x4fc>
   121bc:	e5d83001 	ldrb	r3, [r8, #1]
   121c0:	e2882001 	add	r2, r8, #1	; 0x1
   121c4:	e353004d 	cmp	r3, #77	; 0x4d
   121c8:	0a000008 	beq	121f0 <vsprintf+0x328>
   121cc:	8a000002 	bhi	121dc <vsprintf+0x314>
   121d0:	e3530049 	cmp	r3, #73	; 0x49
   121d4:	1a00007c 	bne	123cc <vsprintf+0x504>
   121d8:	ea000025 	b	12274 <vsprintf+0x3ac>
   121dc:	e3530069 	cmp	r3, #105	; 0x69
   121e0:	0a000022 	beq	12270 <vsprintf+0x3a8>
   121e4:	e353006d 	cmp	r3, #109	; 0x6d
   121e8:	1a000077 	bne	123cc <vsprintf+0x504>
   121ec:	e3877040 	orr	r7, r7, #64	; 0x40
   121f0:	e1a03327 	lsr	r3, r7, #6
   121f4:	e2233001 	eor	r3, r3, #1	; 0x1
   121f8:	e2034001 	and	r4, r3, #1	; 0x1
   121fc:	e3a0e000 	mov	lr, #0	; 0x0
   12200:	e28dc05b 	add	ip, sp, #91	; 0x5b
   12204:	e3a0503a 	mov	r5, #58	; 0x3a
   12208:	e7db200e 	ldrb	r2, [fp, lr]
   1220c:	e59f0398 	ldr	r0, [pc, #920]	; 125ac <vsprintf+0x6e4>
   12210:	e202300f 	and	r3, r2, #15	; 0xf
   12214:	e7d02222 	ldrb	r2, [r0, r2, lsr #4]
   12218:	e7d03003 	ldrb	r3, [r0, r3]
   1221c:	e1a0000c 	mov	r0, ip
   12220:	e4c02001 	strb	r2, [r0], #1
   12224:	e35e0005 	cmp	lr, #5	; 0x5
   12228:	03a01000 	moveq	r1, #0	; 0x0
   1222c:	12041001 	andne	r1, r4, #1	; 0x1
   12230:	e3510000 	cmp	r1, #0	; 0x0
   12234:	e5cc3001 	strb	r3, [ip, #1]
   12238:	e28ee001 	add	lr, lr, #1	; 0x1
   1223c:	e280c001 	add	ip, r0, #1	; 0x1
   12240:	128cc001 	addne	ip, ip, #1	; 0x1
   12244:	15c05001 	strbne	r5, [r0, #1]
   12248:	e35e0006 	cmp	lr, #6	; 0x6
   1224c:	1affffed 	bne	12208 <vsprintf+0x340>
   12250:	e3a03000 	mov	r3, #0	; 0x0
   12254:	e5cc3000 	strb	r3, [ip]
   12258:	e1a0000a 	mov	r0, sl
   1225c:	e3c7c040 	bic	ip, r7, #64	; 0x40
   12260:	e1a02009 	mov	r2, r9
   12264:	e59d302c 	ldr	r3, [sp, #44]
   12268:	e28d105b 	add	r1, sp, #91	; 0x5b
   1226c:	ea000053 	b	123c0 <vsprintf+0x4f8>
   12270:	e3877040 	orr	r7, r7, #64	; 0x40
   12274:	e5d23001 	ldrb	r3, [r2, #1]
   12278:	e3530036 	cmp	r3, #54	; 0x36
   1227c:	1a00002d 	bne	12338 <vsprintf+0x470>
   12280:	e1a03327 	lsr	r3, r7, #6
   12284:	e2233001 	eor	r3, r3, #1	; 0x1
   12288:	e1a0500b 	mov	r5, fp
   1228c:	e3a04000 	mov	r4, #0	; 0x0
   12290:	e203b001 	and	fp, r3, #1	; 0x1
   12294:	e28dc033 	add	ip, sp, #51	; 0x33
   12298:	e5d52000 	ldrb	r2, [r5]
   1229c:	e59fe308 	ldr	lr, [pc, #776]	; 125ac <vsprintf+0x6e4>
   122a0:	e202300f 	and	r3, r2, #15	; 0xf
   122a4:	e7de0003 	ldrb	r0, [lr, r3]
   122a8:	e7de3222 	ldrb	r3, [lr, r2, lsr #4]
   122ac:	e1a0100c 	mov	r1, ip
   122b0:	e4c13001 	strb	r3, [r1], #1
   122b4:	e5cc0001 	strb	r0, [ip, #1]
   122b8:	e5d52001 	ldrb	r2, [r5, #1]
   122bc:	e59f02e8 	ldr	r0, [pc, #744]	; 125ac <vsprintf+0x6e4>
   122c0:	e202300f 	and	r3, r2, #15	; 0xf
   122c4:	e7d0c003 	ldrb	ip, [r0, r3]
   122c8:	e59f32dc 	ldr	r3, [pc, #732]	; 125ac <vsprintf+0x6e4>
   122cc:	e3540007 	cmp	r4, #7	; 0x7
   122d0:	03a0e000 	moveq	lr, #0	; 0x0
   122d4:	120be001 	andne	lr, fp, #1	; 0x1
   122d8:	e7d32222 	ldrb	r2, [r3, r2, lsr #4]
   122dc:	e35e0000 	cmp	lr, #0	; 0x0
   122e0:	e58d2014 	str	r2, [sp, #20]
   122e4:	e2810001 	add	r0, r1, #1	; 0x1
   122e8:	e59de014 	ldr	lr, [sp, #20]
   122ec:	e2802001 	add	r2, r0, #1	; 0x1
   122f0:	e5c1e001 	strb	lr, [r1, #1]
   122f4:	e2844001 	add	r4, r4, #1	; 0x1
   122f8:	e5c0c001 	strb	ip, [r0, #1]
   122fc:	13a0003a 	movne	r0, #58	; 0x3a
   12300:	e282c001 	add	ip, r2, #1	; 0x1
   12304:	128cc001 	addne	ip, ip, #1	; 0x1
   12308:	15c20001 	strbne	r0, [r2, #1]
   1230c:	e3540008 	cmp	r4, #8	; 0x8
   12310:	e2855002 	add	r5, r5, #2	; 0x2
   12314:	1affffdf 	bne	12298 <vsprintf+0x3d0>
   12318:	e3a03000 	mov	r3, #0	; 0x0
   1231c:	e5cc3000 	strb	r3, [ip]
   12320:	e1a0000a 	mov	r0, sl
   12324:	e3c7c040 	bic	ip, r7, #64	; 0x40
   12328:	e1a02009 	mov	r2, r9
   1232c:	e59d302c 	ldr	r3, [sp, #44]
   12330:	e28d1033 	add	r1, sp, #51	; 0x33
   12334:	ea000021 	b	123c0 <vsprintf+0x4f8>
   12338:	e3530034 	cmp	r3, #52	; 0x34
   1233c:	13c77040 	bicne	r7, r7, #64	; 0x40
   12340:	1a000021 	bne	123cc <vsprintf+0x504>
   12344:	e3a05000 	mov	r5, #0	; 0x0
   12348:	e28d406d 	add	r4, sp, #109	; 0x6d
   1234c:	e7db1005 	ldrb	r1, [fp, r5]
   12350:	e28d007d 	add	r0, sp, #125	; 0x7d
   12354:	ebfffd9b 	bl	119c8 <put_dec_trunc>
   12358:	e59d101c 	ldr	r1, [sp, #28]
   1235c:	e2402001 	sub	r2, r0, #1	; 0x1
   12360:	e0610000 	rsb	r0, r1, r0
   12364:	e3a01000 	mov	r1, #0	; 0x0
   12368:	ea000002 	b	12378 <vsprintf+0x4b0>
   1236c:	e5d23001 	ldrb	r3, [r2, #1]
   12370:	e7c43001 	strb	r3, [r4, r1]
   12374:	e2811001 	add	r1, r1, #1	; 0x1
   12378:	e59d3018 	ldr	r3, [sp, #24]
   1237c:	e2422001 	sub	r2, r2, #1	; 0x1
   12380:	e1520003 	cmp	r2, r3
   12384:	1afffff8 	bne	1236c <vsprintf+0x4a4>
   12388:	e3550003 	cmp	r5, #3	; 0x3
   1238c:	13a0e02e 	movne	lr, #46	; 0x2e
   12390:	e0844000 	add	r4, r4, r0
   12394:	e2855001 	add	r5, r5, #1	; 0x1
   12398:	14c4e001 	strbne	lr, [r4], #1
   1239c:	e3550004 	cmp	r5, #4	; 0x4
   123a0:	1affffe9 	bne	1234c <vsprintf+0x484>
   123a4:	e3a03000 	mov	r3, #0	; 0x0
   123a8:	e5c43000 	strb	r3, [r4]
   123ac:	e59d302c 	ldr	r3, [sp, #44]
   123b0:	e3c7c040 	bic	ip, r7, #64	; 0x40
   123b4:	e1a0000a 	mov	r0, sl
   123b8:	e1a02009 	mov	r2, r9
   123bc:	e28d106d 	add	r1, sp, #109	; 0x6d
   123c0:	e58dc000 	str	ip, [sp]
   123c4:	ebfffdb2 	bl	11a94 <string>
   123c8:	ea00000d 	b	12404 <vsprintf+0x53c>
   123cc:	e3a01010 	mov	r1, #16	; 0x10
   123d0:	e3790001 	cmn	r9, #1	; 0x1
   123d4:	e58d1000 	str	r1, [sp]
   123d8:	e59d102c 	ldr	r1, [sp, #44]
   123dc:	e387c020 	orr	ip, r7, #32	; 0x20
   123e0:	02899009 	addeq	r9, r9, #9	; 0x9
   123e4:	0387c021 	orreq	ip, r7, #33	; 0x21
   123e8:	e1a0000a 	mov	r0, sl
   123ec:	e1a0200b 	mov	r2, fp
   123f0:	e3a03000 	mov	r3, #0	; 0x0
   123f4:	e58d9004 	str	r9, [sp, #4]
   123f8:	e58d1008 	str	r1, [sp, #8]
   123fc:	e58dc00c 	str	ip, [sp, #12]
   12400:	ebfffe1a 	bl	11c70 <number>
   12404:	e1a0a000 	mov	sl, r0
   12408:	e1a02008 	mov	r2, r8
   1240c:	e5d23001 	ldrb	r3, [r2, #1]
   12410:	e59fe198 	ldr	lr, [pc, #408]	; 125b0 <vsprintf+0x6e8>
   12414:	e1a08002 	mov	r8, r2
   12418:	e7de3003 	ldrb	r3, [lr, r3]
   1241c:	e2822001 	add	r2, r2, #1	; 0x1
   12420:	e3130007 	tst	r3, #7	; 0x7
   12424:	1afffff8 	bne	1240c <vsprintf+0x544>
   12428:	e2866004 	add	r6, r6, #4	; 0x4
   1242c:	ea000054 	b	12584 <vsprintf+0x6bc>
   12430:	e59d0028 	ldr	r0, [sp, #40]
   12434:	e5963000 	ldr	r3, [r6]
   12438:	e2861004 	add	r1, r6, #4	; 0x4
   1243c:	e060200a 	rsb	r2, r0, sl
   12440:	e1a06001 	mov	r6, r1
   12444:	e5832000 	str	r2, [r3]
   12448:	ea00004d 	b	12584 <vsprintf+0x6bc>
   1244c:	e3a0c008 	mov	ip, #8	; 0x8
   12450:	ea00000d 	b	1248c <vsprintf+0x5c4>
   12454:	e3877020 	orr	r7, r7, #32	; 0x20
   12458:	e3a0c010 	mov	ip, #16	; 0x10
   1245c:	ea00000a 	b	1248c <vsprintf+0x5c4>
   12460:	e3877002 	orr	r7, r7, #2	; 0x2
   12464:	ea000007 	b	12488 <vsprintf+0x5c0>
   12468:	e3a03025 	mov	r3, #37	; 0x25
   1246c:	e4ca3001 	strb	r3, [sl], #1
   12470:	e5d83000 	ldrb	r3, [r8]
   12474:	e3530000 	cmp	r3, #0	; 0x0
   12478:	02488001 	subeq	r8, r8, #1	; 0x1
   1247c:	0a000040 	beq	12584 <vsprintf+0x6bc>
   12480:	e4ca3001 	strb	r3, [sl], #1
   12484:	ea00003e 	b	12584 <vsprintf+0x6bc>
   12488:	e3a0c00a 	mov	ip, #10	; 0xa
   1248c:	e352004c 	cmp	r2, #76	; 0x4c
   12490:	1a000009 	bne	124bc <vsprintf+0x5f4>
   12494:	e59d1024 	ldr	r1, [sp, #36]
   12498:	e3a02000 	mov	r2, #0	; 0x0
   1249c:	e0613006 	rsb	r3, r1, r6
   124a0:	e3130007 	tst	r3, #7	; 0x7
   124a4:	02866004 	addeq	r6, r6, #4	; 0x4
   124a8:	e896000a 	ldm	r6, {r1, r3}
   124ac:	e1a05003 	mov	r5, r3
   124b0:	e1824001 	orr	r4, r2, r1
   124b4:	e2866008 	add	r6, r6, #8	; 0x8
   124b8:	ea000028 	b	12560 <vsprintf+0x698>
   124bc:	e352006c 	cmp	r2, #108	; 0x6c
   124c0:	1a000006 	bne	124e0 <vsprintf+0x618>
   124c4:	e5963000 	ldr	r3, [r6]
   124c8:	e3170002 	tst	r7, #2	; 0x2
   124cc:	e1a04003 	mov	r4, r3
   124d0:	e3a05000 	mov	r5, #0	; 0x0
   124d4:	e2866004 	add	r6, r6, #4	; 0x4
   124d8:	0a000020 	beq	12560 <vsprintf+0x698>
   124dc:	ea00001e 	b	1255c <vsprintf+0x694>
   124e0:	e352007a 	cmp	r2, #122	; 0x7a
   124e4:	1352005a 	cmpne	r2, #90	; 0x5a
   124e8:	05963000 	ldreq	r3, [r6]
   124ec:	02866004 	addeq	r6, r6, #4	; 0x4
   124f0:	01a04003 	moveq	r4, r3
   124f4:	03a05000 	moveq	r5, #0	; 0x0
   124f8:	0a000018 	beq	12560 <vsprintf+0x698>
   124fc:	e3520074 	cmp	r2, #116	; 0x74
   12500:	05963000 	ldreq	r3, [r6]
   12504:	02866004 	addeq	r6, r6, #4	; 0x4
   12508:	0a00000b 	beq	1253c <vsprintf+0x674>
   1250c:	e3520068 	cmp	r2, #104	; 0x68
   12510:	e2861004 	add	r1, r6, #4	; 0x4
   12514:	e5962000 	ldr	r2, [r6]
   12518:	e2070002 	and	r0, r7, #2	; 0x2
   1251c:	1a000009 	bne	12548 <vsprintf+0x680>
   12520:	e6ff3072 	uxth	r3, r2
   12524:	e3500000 	cmp	r0, #0	; 0x0
   12528:	e1a06001 	mov	r6, r1
   1252c:	e1a04003 	mov	r4, r3
   12530:	e3a05000 	mov	r5, #0	; 0x0
   12534:	0a000009 	beq	12560 <vsprintf+0x698>
   12538:	e6bf3074 	sxth	r3, r4
   1253c:	e1a04003 	mov	r4, r3
   12540:	e1a05fc4 	asr	r5, r4, #31
   12544:	ea000005 	b	12560 <vsprintf+0x698>
   12548:	e3500000 	cmp	r0, #0	; 0x0
   1254c:	e1a06001 	mov	r6, r1
   12550:	e1a04002 	mov	r4, r2
   12554:	e3a05000 	mov	r5, #0	; 0x0
   12558:	0a000000 	beq	12560 <vsprintf+0x698>
   1255c:	e1a05fc4 	asr	r5, r4, #31
   12560:	e59de02c 	ldr	lr, [sp, #44]
   12564:	e1a0000a 	mov	r0, sl
   12568:	e1a02004 	mov	r2, r4
   1256c:	e1a03005 	mov	r3, r5
   12570:	e58dc000 	str	ip, [sp]
   12574:	e98d4200 	stmib	sp, {r9, lr}
   12578:	e58d700c 	str	r7, [sp, #12]
   1257c:	ebfffdbb 	bl	11c70 <number>
   12580:	e1a0a000 	mov	sl, r0
   12584:	e2888001 	add	r8, r8, #1	; 0x1
   12588:	e5d83000 	ldrb	r3, [r8]
   1258c:	e3530000 	cmp	r3, #0	; 0x0
   12590:	1afffe58 	bne	11ef8 <vsprintf+0x30>
   12594:	e5ca3000 	strb	r3, [sl]
   12598:	e59d1028 	ldr	r1, [sp, #40]
   1259c:	e061000a 	rsb	r0, r1, sl
   125a0:	e28dd084 	add	sp, sp, #132	; 0x84
   125a4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   125a8:	00018c16 	.word	0x00018c16
   125ac:	00015fca 	.word	0x00015fca
   125b0:	00015feb 	.word	0x00015feb

000125b4 <sprintf>:
   125b4:	e92d000e 	push	{r1, r2, r3}
   125b8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   125bc:	e59d1004 	ldr	r1, [sp, #4]
   125c0:	e28d2008 	add	r2, sp, #8	; 0x8
   125c4:	ebfffe3f 	bl	11ec8 <vsprintf>
   125c8:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
   125cc:	e28dd00c 	add	sp, sp, #12	; 0xc
   125d0:	e12fff1e 	bx	lr

000125d4 <ius_check_header>:
   125d4:	e92d4070 	push	{r4, r5, r6, lr}
   125d8:	e5901000 	ldr	r1, [r0]
   125dc:	e59f30e8 	ldr	r3, [pc, #232]	; 126cc <ius_check_header+0xf8>
   125e0:	e24dd008 	sub	sp, sp, #8	; 0x8
   125e4:	e1510003 	cmp	r1, r3
   125e8:	e1a04000 	mov	r4, r0
   125ec:	0a000005 	beq	12608 <ius_check_header+0x34>
   125f0:	e59f00d8 	ldr	r0, [pc, #216]	; 126d0 <ius_check_header+0xfc>
   125f4:	e59f30d8 	ldr	r3, [pc, #216]	; 126d4 <ius_check_header+0x100>
   125f8:	e1a0e00f 	mov	lr, pc
   125fc:	e593f000 	ldr	pc, [r3]
   12600:	e3e00000 	mvn	r0, #0	; 0x0
   12604:	ea00002e 	b	126c4 <ius_check_header+0xf0>
   12608:	ebffd377 	bl	73ec <cdbg_verify_burn>
   1260c:	e3500000 	cmp	r0, #0	; 0x0
   12610:	0a000011 	beq	1265c <ius_check_header+0x88>
   12614:	e5943004 	ldr	r3, [r4, #4]
   12618:	e3130002 	tst	r3, #2	; 0x2
   1261c:	1a000005 	bne	12638 <ius_check_header+0x64>
   12620:	e59f00b0 	ldr	r0, [pc, #176]	; 126d8 <ius_check_header+0x104>
   12624:	e59f30a8 	ldr	r3, [pc, #168]	; 126d4 <ius_check_header+0x100>
   12628:	e1a0e00f 	mov	lr, pc
   1262c:	e593f000 	ldr	pc, [r3]
   12630:	e3e00021 	mvn	r0, #33	; 0x21
   12634:	ea000022 	b	126c4 <ius_check_header+0xf0>
   12638:	ebffd36f 	bl	73fc <cdbg_verify_burn_key>
   1263c:	e59f2098 	ldr	r2, [pc, #152]	; 126dc <ius_check_header+0x108>
   12640:	e1a01000 	mov	r1, r0
   12644:	e3a00000 	mov	r0, #0	; 0x0
   12648:	ebffd006 	bl	6668 <ss_aes_init>
   1264c:	e2840010 	add	r0, r4, #16	; 0x10
   12650:	e1a01000 	mov	r1, r0
   12654:	e3a02e1f 	mov	r2, #496	; 0x1f0
   12658:	ebffcff1 	bl	6624 <ss_aes_data>
   1265c:	e3a06000 	mov	r6, #0	; 0x0
   12660:	e5945018 	ldr	r5, [r4, #24]
   12664:	e3a01c02 	mov	r1, #512	; 0x200
   12668:	e5846018 	str	r6, [r4, #24]
   1266c:	e1a00006 	mov	r0, r6
   12670:	ebffd11a 	bl	6ae0 <hash_init>
   12674:	e3a01c02 	mov	r1, #512	; 0x200
   12678:	e1a00004 	mov	r0, r4
   1267c:	ebffd0f2 	bl	6a4c <hash_data>
   12680:	e28d0004 	add	r0, sp, #4	; 0x4
   12684:	ebffd0d4 	bl	69dc <hash_value>
   12688:	e59d2004 	ldr	r2, [sp, #4]
   1268c:	e59f3040 	ldr	r3, [pc, #64]	; 126d4 <ius_check_header+0x100>
   12690:	e1550002 	cmp	r5, r2
   12694:	e5845018 	str	r5, [r4, #24]
   12698:	0a000005 	beq	126b4 <ius_check_header+0xe0>
   1269c:	e59f003c 	ldr	r0, [pc, #60]	; 126e0 <ius_check_header+0x10c>
   126a0:	e1a01005 	mov	r1, r5
   126a4:	e1a0e00f 	mov	lr, pc
   126a8:	e593f000 	ldr	pc, [r3]
   126ac:	e3e0001f 	mvn	r0, #31	; 0x1f
   126b0:	ea000003 	b	126c4 <ius_check_header+0xf0>
   126b4:	e59f0028 	ldr	r0, [pc, #40]	; 126e4 <ius_check_header+0x110>
   126b8:	e1a0e00f 	mov	lr, pc
   126bc:	e593f000 	ldr	pc, [r3]
   126c0:	e1a00006 	mov	r0, r6
   126c4:	e28dd008 	add	sp, sp, #8	; 0x8
   126c8:	e8bd8070 	pop	{r4, r5, r6, pc}
   126cc:	7375692b 	.word	0x7375692b
   126d0:	00018c1d 	.word	0x00018c1d
   126d4:	3c00d6cc 	.word	0x3c00d6cc
   126d8:	00018c37 	.word	0x00018c37
   126dc:	3c00d088 	.word	0x3c00d088
   126e0:	00018c46 	.word	0x00018c46
   126e4:	00018c6e 	.word	0x00018c6e

000126e8 <printf_off>:
   126e8:	e92d000f 	push	{r0, r1, r2, r3}
   126ec:	e28dd010 	add	sp, sp, #16	; 0x10
   126f0:	e12fff1e 	bx	lr

000126f4 <disable_ctrlc>:
   126f4:	e59f300c 	ldr	r3, [pc, #12]	; 12708 <disable_ctrlc+0x14>
   126f8:	e5932000 	ldr	r2, [r3]
   126fc:	e5830000 	str	r0, [r3]
   12700:	e1a00002 	mov	r0, r2
   12704:	e12fff1e 	bx	lr
   12708:	3c00d26c 	.word	0x3c00d26c

0001270c <had_ctrlc>:
   1270c:	e59f3004 	ldr	r3, [pc, #4]	; 12718 <had_ctrlc+0xc>
   12710:	e5930000 	ldr	r0, [r3]
   12714:	e12fff1e 	bx	lr
   12718:	3c00d268 	.word	0x3c00d268

0001271c <clear_ctrlc>:
   1271c:	e59f3008 	ldr	r3, [pc, #8]	; 1272c <clear_ctrlc+0x10>
   12720:	e3a02000 	mov	r2, #0	; 0x0
   12724:	e5832000 	str	r2, [r3]
   12728:	e12fff1e 	bx	lr
   1272c:	3c00d268 	.word	0x3c00d268

00012730 <dbg>:
   12730:	e92d000f 	push	{r0, r1, r2, r3}
   12734:	e28dd010 	add	sp, sp, #16	; 0x10
   12738:	e12fff1e 	bx	lr

0001273c <puts>:
   1273c:	eaffec4c 	b	d874 <serial_puts>

00012740 <vprintf>:
   12740:	e92d4010 	push	{r4, lr}
   12744:	e24ddd06 	sub	sp, sp, #384	; 0x180
   12748:	e1a02001 	mov	r2, r1
   1274c:	e1a01000 	mov	r1, r0
   12750:	e1a0000d 	mov	r0, sp
   12754:	ebfffddb 	bl	11ec8 <vsprintf>
   12758:	e1a0000d 	mov	r0, sp
   1275c:	e1a0400d 	mov	r4, sp
   12760:	ebffec43 	bl	d874 <serial_puts>
   12764:	e28ddd06 	add	sp, sp, #384	; 0x180
   12768:	e8bd8010 	pop	{r4, pc}

0001276c <printf_on>:
   1276c:	e92d000f 	push	{r0, r1, r2, r3}
   12770:	e92d4010 	push	{r4, lr}
   12774:	e24ddd06 	sub	sp, sp, #384	; 0x180
   12778:	e1a0000d 	mov	r0, sp
   1277c:	e59d1188 	ldr	r1, [sp, #392]
   12780:	e28d2f63 	add	r2, sp, #396	; 0x18c
   12784:	ebfffdcf 	bl	11ec8 <vsprintf>
   12788:	e1a0000d 	mov	r0, sp
   1278c:	e1a0400d 	mov	r4, sp
   12790:	ebffec37 	bl	d874 <serial_puts>
   12794:	e28ddd06 	add	sp, sp, #384	; 0x180
   12798:	e8bd4010 	pop	{r4, lr}
   1279c:	e28dd010 	add	sp, sp, #16	; 0x10
   127a0:	e12fff1e 	bx	lr

000127a4 <serial_printf>:
   127a4:	e92d000f 	push	{r0, r1, r2, r3}
   127a8:	e92d4010 	push	{r4, lr}
   127ac:	e24ddd06 	sub	sp, sp, #384	; 0x180
   127b0:	e1a0000d 	mov	r0, sp
   127b4:	e59d1188 	ldr	r1, [sp, #392]
   127b8:	e28d2f63 	add	r2, sp, #396	; 0x18c
   127bc:	ebfffdc1 	bl	11ec8 <vsprintf>
   127c0:	e1a0000d 	mov	r0, sp
   127c4:	e1a0400d 	mov	r4, sp
   127c8:	ebffec29 	bl	d874 <serial_puts>
   127cc:	e28ddd06 	add	sp, sp, #384	; 0x180
   127d0:	e8bd4010 	pop	{r4, lr}
   127d4:	e28dd010 	add	sp, sp, #16	; 0x10
   127d8:	e12fff1e 	bx	lr

000127dc <putc>:
   127dc:	eaffec0d 	b	d818 <serial_putc>

000127e0 <tstc>:
   127e0:	eaffec05 	b	d7fc <serial_tstc>

000127e4 <getc>:
   127e4:	eaffec33 	b	d8b8 <serial_getc>

000127e8 <ctrlc>:
   127e8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   127ec:	e59f3040 	ldr	r3, [pc, #64]	; 12834 <ctrlc+0x4c>
   127f0:	e24dd004 	sub	sp, sp, #4	; 0x4
   127f4:	e5933000 	ldr	r3, [r3]
   127f8:	e3530000 	cmp	r3, #0	; 0x0
   127fc:	1a000009 	bne	12828 <ctrlc+0x40>
   12800:	ebffebfd 	bl	d7fc <serial_tstc>
   12804:	e3500000 	cmp	r0, #0	; 0x0
   12808:	0a000006 	beq	12828 <ctrlc+0x40>
   1280c:	ebffec29 	bl	d8b8 <serial_getc>
   12810:	e3500003 	cmp	r0, #3	; 0x3
   12814:	059f301c 	ldreq	r3, [pc, #28]	; 12838 <ctrlc+0x50>
   12818:	03a02001 	moveq	r2, #1	; 0x1
   1281c:	05832000 	streq	r2, [r3]
   12820:	01a00002 	moveq	r0, r2
   12824:	0a000000 	beq	1282c <ctrlc+0x44>
   12828:	e3a00000 	mov	r0, #0	; 0x0
   1282c:	e28dd004 	add	sp, sp, #4	; 0x4
   12830:	e8bd8000 	pop	{pc}
   12834:	3c00d26c 	.word	0x3c00d26c
   12838:	3c00d268 	.word	0x3c00d268

0001283c <parse_line>:
   1283c:	e92d4010 	push	{r4, lr}
   12840:	e3a04000 	mov	r4, #0	; 0x0
   12844:	e1a02004 	mov	r2, r4
   12848:	ea000000 	b	12850 <parse_line+0x14>
   1284c:	e2800001 	add	r0, r0, #1	; 0x1
   12850:	e5d03000 	ldrb	r3, [r0]
   12854:	e3530009 	cmp	r3, #9	; 0x9
   12858:	13530020 	cmpne	r3, #32	; 0x20
   1285c:	0afffffa 	beq	1284c <parse_line+0x10>
   12860:	e3530000 	cmp	r3, #0	; 0x0
   12864:	0a00000c 	beq	1289c <parse_line+0x60>
   12868:	e7810104 	str	r0, [r1, r4, lsl #2]
   1286c:	e2844001 	add	r4, r4, #1	; 0x1
   12870:	ea000000 	b	12878 <parse_line+0x3c>
   12874:	e2800001 	add	r0, r0, #1	; 0x1
   12878:	e5d03000 	ldrb	r3, [r0]
   1287c:	e3530000 	cmp	r3, #0	; 0x0
   12880:	13530020 	cmpne	r3, #32	; 0x20
   12884:	0a000002 	beq	12894 <parse_line+0x58>
   12888:	e3530009 	cmp	r3, #9	; 0x9
   1288c:	1afffff8 	bne	12874 <parse_line+0x38>
   12890:	ea000003 	b	128a4 <parse_line+0x68>
   12894:	e3530000 	cmp	r3, #0	; 0x0
   12898:	1a000001 	bne	128a4 <parse_line+0x68>
   1289c:	e7813104 	str	r3, [r1, r4, lsl #2]
   128a0:	ea000007 	b	128c4 <parse_line+0x88>
   128a4:	e3540010 	cmp	r4, #16	; 0x10
   128a8:	e5c02000 	strb	r2, [r0]
   128ac:	1affffe6 	bne	1284c <parse_line+0x10>
   128b0:	e59f0014 	ldr	r0, [pc, #20]	; 128cc <parse_line+0x90>
   128b4:	e1a01004 	mov	r1, r4
   128b8:	e59f3010 	ldr	r3, [pc, #16]	; 128d0 <parse_line+0x94>
   128bc:	e1a0e00f 	mov	lr, pc
   128c0:	e593f000 	ldr	pc, [r3]
   128c4:	e1a00004 	mov	r0, r4
   128c8:	e8bd8010 	pop	{r4, pc}
   128cc:	00018c84 	.word	0x00018c84
   128d0:	3c00d6cc 	.word	0x3c00d6cc

000128d4 <run_command>:
   128d4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   128d8:	e1a05000 	mov	r5, r0
   128dc:	e24ddf93 	sub	sp, sp, #588	; 0x24c
   128e0:	e1a0a001 	mov	sl, r1
   128e4:	ebffff8c 	bl	1271c <clear_ctrlc>
   128e8:	e3550000 	cmp	r5, #0	; 0x0
   128ec:	0a0000ac 	beq	12ba4 <run_command+0x2d0>
   128f0:	e5d53000 	ldrb	r3, [r5]
   128f4:	e3530000 	cmp	r3, #0	; 0x0
   128f8:	0a0000a9 	beq	12ba4 <run_command+0x2d0>
   128fc:	e1a00005 	mov	r0, r5
   12900:	ebffbafb 	bl	14f4 <strlen>
   12904:	e35000ff 	cmp	r0, #255	; 0xff
   12908:	9a000002 	bls	12918 <run_command+0x44>
   1290c:	e59f02a0 	ldr	r0, [pc, #672]	; 12bb4 <run_command+0x2e0>
   12910:	ebffff89 	bl	1273c <puts>
   12914:	ea0000a2 	b	12ba4 <run_command+0x2d0>
   12918:	e28d4f41 	add	r4, sp, #260	; 0x104
   1291c:	e1a01005 	mov	r1, r5
   12920:	e1a00004 	mov	r0, r4
   12924:	e3a06000 	mov	r6, #0	; 0x0
   12928:	ebffba9a 	bl	1398 <strcpy>
   1292c:	e3a07001 	mov	r7, #1	; 0x1
   12930:	e1a0b006 	mov	fp, r6
   12934:	e28d9f81 	add	r9, sp, #516	; 0x204
   12938:	ea000091 	b	12b84 <run_command+0x2b0>
   1293c:	e3510027 	cmp	r1, #39	; 0x27
   12940:	1a000002 	bne	12950 <run_command+0x7c>
   12944:	e5523001 	ldrb	r3, [r2, #-1]
   12948:	e353005c 	cmp	r3, #92	; 0x5c
   1294c:	12200001 	eorne	r0, r0, #1	; 0x1
   12950:	e3500000 	cmp	r0, #0	; 0x0
   12954:	1a000009 	bne	12980 <run_command+0xac>
   12958:	e351003b 	cmp	r1, #59	; 0x3b
   1295c:	13a03000 	movne	r3, #0	; 0x0
   12960:	03a03001 	moveq	r3, #1	; 0x1
   12964:	e1520004 	cmp	r2, r4
   12968:	03a03000 	moveq	r3, #0	; 0x0
   1296c:	e3530000 	cmp	r3, #0	; 0x0
   12970:	0a000002 	beq	12980 <run_command+0xac>
   12974:	e5523001 	ldrb	r3, [r2, #-1]
   12978:	e353005c 	cmp	r3, #92	; 0x5c
   1297c:	1a000005 	bne	12998 <run_command+0xc4>
   12980:	e2822001 	add	r2, r2, #1	; 0x1
   12984:	e5d21000 	ldrb	r1, [r2]
   12988:	e3510000 	cmp	r1, #0	; 0x0
   1298c:	1affffea 	bne	1293c <run_command+0x68>
   12990:	e1a08002 	mov	r8, r2
   12994:	ea000001 	b	129a0 <run_command+0xcc>
   12998:	e5c20000 	strb	r0, [r2]
   1299c:	e2828001 	add	r8, r2, #1	; 0x1
   129a0:	e1a00004 	mov	r0, r4
   129a4:	ebffbad2 	bl	14f4 <strlen>
   129a8:	e3a0c000 	mov	ip, #0	; 0x0
   129ac:	e3a0ec01 	mov	lr, #256	; 0x100
   129b0:	e1a0100c 	mov	r1, ip
   129b4:	e28d5004 	add	r5, sp, #4	; 0x4
   129b8:	ea000049 	b	12ae4 <run_command+0x210>
   129bc:	e35c0003 	cmp	ip, #3	; 0x3
   129c0:	e2400001 	sub	r0, r0, #1	; 0x1
   129c4:	e4d42001 	ldrb	r2, [r4], #1
   129c8:	0a000035 	beq	12aa4 <run_command+0x1d0>
   129cc:	e352005c 	cmp	r2, #92	; 0x5c
   129d0:	13a03000 	movne	r3, #0	; 0x0
   129d4:	03a03001 	moveq	r3, #1	; 0x1
   129d8:	e351005c 	cmp	r1, #92	; 0x5c
   129dc:	03a03000 	moveq	r3, #0	; 0x0
   129e0:	e3530000 	cmp	r3, #0	; 0x0
   129e4:	0a000004 	beq	129fc <run_command+0x128>
   129e8:	e3500000 	cmp	r0, #0	; 0x0
   129ec:	0a00003f 	beq	12af0 <run_command+0x21c>
   129f0:	e4d42001 	ldrb	r2, [r4], #1
   129f4:	e2400001 	sub	r0, r0, #1	; 0x1
   129f8:	e3a0105c 	mov	r1, #92	; 0x5c
   129fc:	e35c0003 	cmp	ip, #3	; 0x3
   12a00:	979ff10c 	ldrls	pc, [pc, ip, lsl #2]
   12a04:	ea000035 	b	12ae0 <run_command+0x20c>
   12a08:	00012a18 	.word	0x00012a18
   12a0c:	00012a5c 	.word	0x00012a5c
   12a10:	00012a94 	.word	0x00012a94
   12a14:	00012aa4 	.word	0x00012aa4
   12a18:	e251105c 	subs	r1, r1, #92	; 0x5c
   12a1c:	13a01001 	movne	r1, #1	; 0x1
   12a20:	e3520027 	cmp	r2, #39	; 0x27
   12a24:	13a03000 	movne	r3, #0	; 0x0
   12a28:	02013001 	andeq	r3, r1, #1	; 0x1
   12a2c:	e3530000 	cmp	r3, #0	; 0x0
   12a30:	13a0c003 	movne	ip, #3	; 0x3
   12a34:	1a000022 	bne	12ac4 <run_command+0x1f0>
   12a38:	e3520024 	cmp	r2, #36	; 0x24
   12a3c:	13a03000 	movne	r3, #0	; 0x0
   12a40:	02013001 	andeq	r3, r1, #1	; 0x1
   12a44:	e3530000 	cmp	r3, #0	; 0x0
   12a48:	13a0c001 	movne	ip, #1	; 0x1
   12a4c:	13a02024 	movne	r2, #36	; 0x24
   12a50:	04c52001 	strbeq	r2, [r5], #1
   12a54:	024ee001 	subeq	lr, lr, #1	; 0x1
   12a58:	ea000020 	b	12ae0 <run_command+0x20c>
   12a5c:	e352007b 	cmp	r2, #123	; 0x7b
   12a60:	13520028 	cmpne	r2, #40	; 0x28
   12a64:	13a03000 	movne	r3, #0	; 0x0
   12a68:	03a03001 	moveq	r3, #1	; 0x1
   12a6c:	03a0c002 	moveq	ip, #2	; 0x2
   12a70:	0a00001a 	beq	12ae0 <run_command+0x20c>
   12a74:	e3a01024 	mov	r1, #36	; 0x24
   12a78:	e25ee001 	subs	lr, lr, #1	; 0x1
   12a7c:	e4c51001 	strb	r1, [r5], #1
   12a80:	0a000015 	beq	12adc <run_command+0x208>
   12a84:	e4c52001 	strb	r2, [r5], #1
   12a88:	e24ee001 	sub	lr, lr, #1	; 0x1
   12a8c:	e1a0c003 	mov	ip, r3
   12a90:	ea000012 	b	12ae0 <run_command+0x20c>
   12a94:	e352007d 	cmp	r2, #125	; 0x7d
   12a98:	13520029 	cmpne	r2, #41	; 0x29
   12a9c:	0a00000e 	beq	12adc <run_command+0x208>
   12aa0:	ea00000e 	b	12ae0 <run_command+0x20c>
   12aa4:	e3520027 	cmp	r2, #39	; 0x27
   12aa8:	13a03000 	movne	r3, #0	; 0x0
   12aac:	03a03001 	moveq	r3, #1	; 0x1
   12ab0:	e351005c 	cmp	r1, #92	; 0x5c
   12ab4:	03a03000 	moveq	r3, #0	; 0x0
   12ab8:	e3530000 	cmp	r3, #0	; 0x0
   12abc:	0a000002 	beq	12acc <run_command+0x1f8>
   12ac0:	e3a0c000 	mov	ip, #0	; 0x0
   12ac4:	e3a02027 	mov	r2, #39	; 0x27
   12ac8:	ea000004 	b	12ae0 <run_command+0x20c>
   12acc:	e4c52001 	strb	r2, [r5], #1
   12ad0:	e24ee001 	sub	lr, lr, #1	; 0x1
   12ad4:	e3a0c003 	mov	ip, #3	; 0x3
   12ad8:	ea000000 	b	12ae0 <run_command+0x20c>
   12adc:	e3a0c000 	mov	ip, #0	; 0x0
   12ae0:	e1a01002 	mov	r1, r2
   12ae4:	e3500000 	cmp	r0, #0	; 0x0
   12ae8:	135e0000 	cmpne	lr, #0	; 0x0
   12aec:	1affffb2 	bne	129bc <run_command+0xe8>
   12af0:	e35e0000 	cmp	lr, #0	; 0x0
   12af4:	15c5b000 	strbne	fp, [r5]
   12af8:	0545e001 	strbeq	lr, [r5, #-1]
   12afc:	e28d0004 	add	r0, sp, #4	; 0x4
   12b00:	e1a01009 	mov	r1, r9
   12b04:	ebffff4c 	bl	1283c <parse_line>
   12b08:	e2505000 	subs	r5, r0, #0	; 0x0
   12b0c:	0a00000d 	beq	12b48 <run_command+0x274>
   12b10:	e59d0204 	ldr	r0, [sp, #516]
   12b14:	ebffc129 	bl	2fc0 <find_cmd>
   12b18:	e2504000 	subs	r4, r0, #0	; 0x0
   12b1c:	1a000005 	bne	12b38 <run_command+0x264>
   12b20:	e59f0090 	ldr	r0, [pc, #144]	; 12bb8 <run_command+0x2e4>
   12b24:	e59d1204 	ldr	r1, [sp, #516]
   12b28:	e59f308c 	ldr	r3, [pc, #140]	; 12bbc <run_command+0x2e8>
   12b2c:	e1a0e00f 	mov	lr, pc
   12b30:	e593f000 	ldr	pc, [r3]
   12b34:	ea000003 	b	12b48 <run_command+0x274>
   12b38:	e5943004 	ldr	r3, [r4, #4]
   12b3c:	e1550003 	cmp	r5, r3
   12b40:	da000002 	ble	12b50 <run_command+0x27c>
   12b44:	ebffc0d7 	bl	2ea8 <cmd_usage>
   12b48:	e3e06000 	mvn	r6, #0	; 0x0
   12b4c:	ea00000b 	b	12b80 <run_command+0x2ac>
   12b50:	e1a02005 	mov	r2, r5
   12b54:	e1a0100a 	mov	r1, sl
   12b58:	e1a03009 	mov	r3, r9
   12b5c:	e1a0e00f 	mov	lr, pc
   12b60:	e594f00c 	ldr	pc, [r4, #12]
   12b64:	e3500000 	cmp	r0, #0	; 0x0
   12b68:	13e06000 	mvnne	r6, #0	; 0x0
   12b6c:	e5944008 	ldr	r4, [r4, #8]
   12b70:	ebfffee5 	bl	1270c <had_ctrlc>
   12b74:	e3500000 	cmp	r0, #0	; 0x0
   12b78:	1a000009 	bne	12ba4 <run_command+0x2d0>
   12b7c:	e0077004 	and	r7, r7, r4
   12b80:	e1a04008 	mov	r4, r8
   12b84:	e5d43000 	ldrb	r3, [r4]
   12b88:	e3530000 	cmp	r3, #0	; 0x0
   12b8c:	11a02004 	movne	r2, r4
   12b90:	13a00000 	movne	r0, #0	; 0x0
   12b94:	1affff7a 	bne	12984 <run_command+0xb0>
   12b98:	e3560000 	cmp	r6, #0	; 0x0
   12b9c:	01a06007 	moveq	r6, r7
   12ba0:	ea000000 	b	12ba8 <run_command+0x2d4>
   12ba4:	e3e06000 	mvn	r6, #0	; 0x0
   12ba8:	e1a00006 	mov	r0, r6
   12bac:	e28ddf93 	add	sp, sp, #588	; 0x24c
   12bb0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   12bb4:	00018ca3 	.word	0x00018ca3
   12bb8:	00018cb9 	.word	0x00018cb9
   12bbc:	3c00d6cc 	.word	0x3c00d6cc

00012bc0 <cread_add_char>:
   12bc0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   12bc4:	e3510000 	cmp	r1, #0	; 0x0
   12bc8:	e24dd004 	sub	sp, sp, #4	; 0x4
   12bcc:	e1a07000 	mov	r7, r0
   12bd0:	e1a04002 	mov	r4, r2
   12bd4:	e1a00003 	mov	r0, r3
   12bd8:	e59d6018 	ldr	r6, [sp, #24]
   12bdc:	e59dc01c 	ldr	ip, [sp, #28]
   12be0:	1a000003 	bne	12bf4 <cread_add_char+0x34>
   12be4:	e5922000 	ldr	r2, [r2]
   12be8:	e5933000 	ldr	r3, [r3]
   12bec:	e1520003 	cmp	r2, r3
   12bf0:	1a000026 	bne	12c90 <cread_add_char+0xd0>
   12bf4:	e5902000 	ldr	r2, [r0]
   12bf8:	e24c3001 	sub	r3, ip, #1	; 0x1
   12bfc:	e1520003 	cmp	r2, r3
   12c00:	9a000003 	bls	12c14 <cread_add_char+0x54>
   12c04:	e3a00007 	mov	r0, #7	; 0x7
   12c08:	e28dd004 	add	sp, sp, #4	; 0x4
   12c0c:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
   12c10:	eafffef1 	b	127dc <putc>
   12c14:	e3510000 	cmp	r1, #0	; 0x0
   12c18:	e2821001 	add	r1, r2, #1	; 0x1
   12c1c:	e5801000 	str	r1, [r0]
   12c20:	0a00001a 	beq	12c90 <cread_add_char+0xd0>
   12c24:	e5943000 	ldr	r3, [r4]
   12c28:	e0635001 	rsb	r5, r3, r1
   12c2c:	e3550001 	cmp	r5, #1	; 0x1
   12c30:	9a000004 	bls	12c48 <cread_add_char+0x88>
   12c34:	e2830001 	add	r0, r3, #1	; 0x1
   12c38:	e0860000 	add	r0, r6, r0
   12c3c:	e0861003 	add	r1, r6, r3
   12c40:	e2452001 	sub	r2, r5, #1	; 0x1
   12c44:	ebffba4d 	bl	1580 <memmove>
   12c48:	e5943000 	ldr	r3, [r4]
   12c4c:	e59f0074 	ldr	r0, [pc, #116]	; 12cc8 <cread_add_char+0x108>
   12c50:	e7c67003 	strb	r7, [r6, r3]
   12c54:	e5942000 	ldr	r2, [r4]
   12c58:	e59f306c 	ldr	r3, [pc, #108]	; 12ccc <cread_add_char+0x10c>
   12c5c:	e0862002 	add	r2, r6, r2
   12c60:	e1a01005 	mov	r1, r5
   12c64:	e1a0e00f 	mov	lr, pc
   12c68:	e593f000 	ldr	pc, [r3]
   12c6c:	e5943000 	ldr	r3, [r4]
   12c70:	e2833001 	add	r3, r3, #1	; 0x1
   12c74:	e5843000 	str	r3, [r4]
   12c78:	ea000000 	b	12c80 <cread_add_char+0xc0>
   12c7c:	ebfffed6 	bl	127dc <putc>
   12c80:	e2555001 	subs	r5, r5, #1	; 0x1
   12c84:	e3a00008 	mov	r0, #8	; 0x8
   12c88:	1afffffb 	bne	12c7c <cread_add_char+0xbc>
   12c8c:	ea00000b 	b	12cc0 <cread_add_char+0x100>
   12c90:	e5943000 	ldr	r3, [r4]
   12c94:	e59f002c 	ldr	r0, [pc, #44]	; 12cc8 <cread_add_char+0x108>
   12c98:	e7c67003 	strb	r7, [r6, r3]
   12c9c:	e5942000 	ldr	r2, [r4]
   12ca0:	e59f3024 	ldr	r3, [pc, #36]	; 12ccc <cread_add_char+0x10c>
   12ca4:	e0862002 	add	r2, r6, r2
   12ca8:	e3a01001 	mov	r1, #1	; 0x1
   12cac:	e1a0e00f 	mov	lr, pc
   12cb0:	e593f000 	ldr	pc, [r3]
   12cb4:	e5943000 	ldr	r3, [r4]
   12cb8:	e2833001 	add	r3, r3, #1	; 0x1
   12cbc:	e5843000 	str	r3, [r4]
   12cc0:	e28dd004 	add	sp, sp, #4	; 0x4
   12cc4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   12cc8:	00018cdc 	.word	0x00018cdc
   12ccc:	3c00d6cc 	.word	0x3c00d6cc

00012cd0 <readline_into_buffer>:
   12cd0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   12cd4:	e59f367c 	ldr	r3, [pc, #1660]	; 13358 <readline_into_buffer+0x688>
   12cd8:	e24dd01c 	sub	sp, sp, #28	; 0x1c
   12cdc:	e593e000 	ldr	lr, [r3]
   12ce0:	e1a06001 	mov	r6, r1
   12ce4:	e35e0000 	cmp	lr, #0	; 0x0
   12ce8:	1a000014 	bne	12d40 <readline_into_buffer+0x70>
   12cec:	e59f3668 	ldr	r3, [pc, #1640]	; 1335c <readline_into_buffer+0x68c>
   12cf0:	e3e02000 	mvn	r2, #0	; 0x0
   12cf4:	e5832000 	str	r2, [r3]
   12cf8:	e59f3660 	ldr	r3, [pc, #1632]	; 13360 <readline_into_buffer+0x690>
   12cfc:	e59fc660 	ldr	ip, [pc, #1632]	; 13364 <readline_into_buffer+0x694>
   12d00:	e583e000 	str	lr, [r3]
   12d04:	e59f365c 	ldr	r3, [pc, #1628]	; 13368 <readline_into_buffer+0x698>
   12d08:	e59f465c 	ldr	r4, [pc, #1628]	; 1336c <readline_into_buffer+0x69c>
   12d0c:	e583e000 	str	lr, [r3]
   12d10:	e59f3658 	ldr	r3, [pc, #1624]	; 13370 <readline_into_buffer+0x6a0>
   12d14:	e1a0100e 	mov	r1, lr
   12d18:	e583e000 	str	lr, [r3]
   12d1c:	e1a0300e 	mov	r3, lr
   12d20:	e784c001 	str	ip, [r4, r1]
   12d24:	e2811004 	add	r1, r1, #4	; 0x4
   12d28:	e3510010 	cmp	r1, #16	; 0x10
   12d2c:	e4cc3100 	strb	r3, [ip], #256
   12d30:	1afffffa 	bne	12d20 <readline_into_buffer+0x50>
   12d34:	e59f361c 	ldr	r3, [pc, #1564]	; 13358 <readline_into_buffer+0x688>
   12d38:	e3a02001 	mov	r2, #1	; 0x1
   12d3c:	e5832000 	str	r2, [r3]
   12d40:	ebfffe7d 	bl	1273c <puts>
   12d44:	e3a03000 	mov	r3, #0	; 0x0
   12d48:	e58d3014 	str	r3, [sp, #20]
   12d4c:	e58d3010 	str	r3, [sp, #16]
   12d50:	e1a05003 	mov	r5, r3
   12d54:	e3a09001 	mov	r9, #1	; 0x1
   12d58:	e59fb614 	ldr	fp, [pc, #1556]	; 13374 <readline_into_buffer+0x6a4>
   12d5c:	e59fa5f8 	ldr	sl, [pc, #1528]	; 1335c <readline_into_buffer+0x68c>
   12d60:	ea000000 	b	12d68 <readline_into_buffer+0x98>
   12d64:	e3a05000 	mov	r5, #0	; 0x0
   12d68:	ebfffe9d 	bl	127e4 <getc>
   12d6c:	e6ef0070 	uxtb	r0, r0
   12d70:	e350000d 	cmp	r0, #13	; 0xd
   12d74:	1350000a 	cmpne	r0, #10	; 0xa
   12d78:	13a04000 	movne	r4, #0	; 0x0
   12d7c:	03a04001 	moveq	r4, #1	; 0x1
   12d80:	1a00000b 	bne	12db4 <readline_into_buffer+0xe4>
   12d84:	e59f35e8 	ldr	r3, [pc, #1512]	; 13374 <readline_into_buffer+0x6a4>
   12d88:	e59f05e8 	ldr	r0, [pc, #1512]	; 13378 <readline_into_buffer+0x6a8>
   12d8c:	e1a0e00f 	mov	lr, pc
   12d90:	e593f000 	ldr	pc, [r3]
   12d94:	e59d5010 	ldr	r5, [sp, #16]
   12d98:	e3a03000 	mov	r3, #0	; 0x0
   12d9c:	e7c63005 	strb	r3, [r6, r5]
   12da0:	e5d63000 	ldrb	r3, [r6]
   12da4:	e3530000 	cmp	r3, #0	; 0x0
   12da8:	13530021 	cmpne	r3, #33	; 0x21
   12dac:	0a000162 	beq	1333c <readline_into_buffer+0x66c>
   12db0:	ea00014e 	b	132f0 <readline_into_buffer+0x620>
   12db4:	e3550000 	cmp	r5, #0	; 0x0
   12db8:	0a000033 	beq	12e8c <readline_into_buffer+0x1bc>
   12dbc:	e3550001 	cmp	r5, #1	; 0x1
   12dc0:	1a00000c 	bne	12df8 <readline_into_buffer+0x128>
   12dc4:	e350005b 	cmp	r0, #91	; 0x5b
   12dc8:	05cd0009 	strbeq	r0, [sp, #9]
   12dcc:	03a05002 	moveq	r5, #2	; 0x2
   12dd0:	0affffe4 	beq	12d68 <readline_into_buffer+0x98>
   12dd4:	e3a0cc01 	mov	ip, #256	; 0x100
   12dd8:	e5dd0008 	ldrb	r0, [sp, #8]
   12ddc:	e1a01009 	mov	r1, r9
   12de0:	e28d2014 	add	r2, sp, #20	; 0x14
   12de4:	e28d3010 	add	r3, sp, #16	; 0x10
   12de8:	e88d1040 	stm	sp, {r6, ip}
   12dec:	e1a05004 	mov	r5, r4
   12df0:	ebffff72 	bl	12bc0 <cread_add_char>
   12df4:	eaffffdb 	b	12d68 <readline_into_buffer+0x98>
   12df8:	e2403041 	sub	r3, r0, #65	; 0x41
   12dfc:	e3530007 	cmp	r3, #7	; 0x7
   12e00:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
   12e04:	ea000009 	b	12e30 <readline_into_buffer+0x160>
   12e08:	00013174 	.word	0x00013174
   12e0c:	000131b4 	.word	0x000131b4
   12e10:	00012e7c 	.word	0x00012e7c
   12e14:	00012f88 	.word	0x00012f88
   12e18:	00012e30 	.word	0x00012e30
   12e1c:	00012e30 	.word	0x00012e30
   12e20:	00012e30 	.word	0x00012e30
   12e24:	00012e28 	.word	0x00012e28
   12e28:	e3a05000 	mov	r5, #0	; 0x0
   12e2c:	ea000043 	b	12f40 <readline_into_buffer+0x270>
   12e30:	e28d1018 	add	r1, sp, #24	; 0x18
   12e34:	e0813005 	add	r3, r1, r5
   12e38:	e5430010 	strb	r0, [r3, #-16]
   12e3c:	e2855001 	add	r5, r5, #1	; 0x1
   12e40:	e3a04000 	mov	r4, #0	; 0x0
   12e44:	e28d8010 	add	r8, sp, #16	; 0x10
   12e48:	e3a07c01 	mov	r7, #256	; 0x100
   12e4c:	ea000004 	b	12e64 <readline_into_buffer+0x194>
   12e50:	e28dc008 	add	ip, sp, #8	; 0x8
   12e54:	e7dc0004 	ldrb	r0, [ip, r4]
   12e58:	e88d00c0 	stm	sp, {r6, r7}
   12e5c:	ebffff57 	bl	12bc0 <cread_add_char>
   12e60:	e2844001 	add	r4, r4, #1	; 0x1
   12e64:	e1540005 	cmp	r4, r5
   12e68:	e1a01009 	mov	r1, r9
   12e6c:	e28d2014 	add	r2, sp, #20	; 0x14
   12e70:	e1a03008 	mov	r3, r8
   12e74:	1afffff5 	bne	12e50 <readline_into_buffer+0x180>
   12e78:	eaffffb9 	b	12d64 <readline_into_buffer+0x94>
   12e7c:	e3a00006 	mov	r0, #6	; 0x6
   12e80:	e350000b 	cmp	r0, #11	; 0xb
   12e84:	e3a05000 	mov	r5, #0	; 0x0
   12e88:	ea000001 	b	12e94 <readline_into_buffer+0x1c4>
   12e8c:	e350000b 	cmp	r0, #11	; 0xb
   12e90:	0a00005f 	beq	13014 <readline_into_buffer+0x344>
   12e94:	8a00000d 	bhi	12ed0 <readline_into_buffer+0x200>
   12e98:	e3500004 	cmp	r0, #4	; 0x4
   12e9c:	0a000042 	beq	12fac <readline_into_buffer+0x2dc>
   12ea0:	8a000005 	bhi	12ebc <readline_into_buffer+0x1ec>
   12ea4:	e3500002 	cmp	r0, #2	; 0x2
   12ea8:	0a000037 	beq	12f8c <readline_into_buffer+0x2bc>
   12eac:	8a000028 	bhi	12f54 <readline_into_buffer+0x284>
   12eb0:	e3500001 	cmp	r0, #1	; 0x1
   12eb4:	1a000106 	bne	132d4 <readline_into_buffer+0x604>
   12eb8:	ea000020 	b	12f40 <readline_into_buffer+0x270>
   12ebc:	e3500006 	cmp	r0, #6	; 0x6
   12ec0:	0a000027 	beq	12f64 <readline_into_buffer+0x294>
   12ec4:	3a000066 	bcc	13064 <readline_into_buffer+0x394>
   12ec8:	e3500008 	cmp	r0, #8	; 0x8
   12ecc:	ea000015 	b	12f28 <readline_into_buffer+0x258>
   12ed0:	e3500015 	cmp	r0, #21	; 0x15
   12ed4:	0a000070 	beq	1309c <readline_into_buffer+0x3cc>
   12ed8:	8a000007 	bhi	12efc <readline_into_buffer+0x22c>
   12edc:	e350000f 	cmp	r0, #15	; 0xf
   12ee0:	02299001 	eoreq	r9, r9, #1	; 0x1
   12ee4:	0affff9f 	beq	12d68 <readline_into_buffer+0x98>
   12ee8:	e3500010 	cmp	r0, #16	; 0x10
   12eec:	0a0000a0 	beq	13174 <readline_into_buffer+0x4a4>
   12ef0:	e350000e 	cmp	r0, #14	; 0xe
   12ef4:	1a0000f6 	bne	132d4 <readline_into_buffer+0x604>
   12ef8:	ea0000ad 	b	131b4 <readline_into_buffer+0x4e4>
   12efc:	e350001b 	cmp	r0, #27	; 0x1b
   12f00:	05cd0008 	strbeq	r0, [sp, #8]
   12f04:	03a05001 	moveq	r5, #1	; 0x1
   12f08:	0affff96 	beq	12d68 <readline_into_buffer+0x98>
   12f0c:	8a000002 	bhi	12f1c <readline_into_buffer+0x24c>
   12f10:	e3500018 	cmp	r0, #24	; 0x18
   12f14:	1a0000ee 	bne	132d4 <readline_into_buffer+0x604>
   12f18:	ea00005f 	b	1309c <readline_into_buffer+0x3cc>
   12f1c:	e350007f 	cmp	r0, #127	; 0x7f
   12f20:	0a000075 	beq	130fc <readline_into_buffer+0x42c>
   12f24:	e35000ff 	cmp	r0, #255	; 0xff
   12f28:	1a0000e9 	bne	132d4 <readline_into_buffer+0x604>
   12f2c:	ea000072 	b	130fc <readline_into_buffer+0x42c>
   12f30:	ebfffe29 	bl	127dc <putc>
   12f34:	e59d3014 	ldr	r3, [sp, #20]
   12f38:	e2433001 	sub	r3, r3, #1	; 0x1
   12f3c:	e58d3014 	str	r3, [sp, #20]
   12f40:	e59d3014 	ldr	r3, [sp, #20]
   12f44:	e3a00008 	mov	r0, #8	; 0x8
   12f48:	e3530000 	cmp	r3, #0	; 0x0
   12f4c:	1afffff7 	bne	12f30 <readline_into_buffer+0x260>
   12f50:	eaffff84 	b	12d68 <readline_into_buffer+0x98>
   12f54:	e3a03000 	mov	r3, #0	; 0x0
   12f58:	e5c63000 	strb	r3, [r6]
   12f5c:	e3e00000 	mvn	r0, #0	; 0x0
   12f60:	ea0000fa 	b	13350 <readline_into_buffer+0x680>
   12f64:	e59d2014 	ldr	r2, [sp, #20]
   12f68:	e59d3010 	ldr	r3, [sp, #16]
   12f6c:	e1520003 	cmp	r2, r3
   12f70:	2affff7c 	bcs	12d68 <readline_into_buffer+0x98>
   12f74:	e7d60002 	ldrb	r0, [r6, r2]
   12f78:	ebfffe17 	bl	127dc <putc>
   12f7c:	e59d3014 	ldr	r3, [sp, #20]
   12f80:	e2833001 	add	r3, r3, #1	; 0x1
   12f84:	ea0000d0 	b	132cc <readline_into_buffer+0x5fc>
   12f88:	e3a05000 	mov	r5, #0	; 0x0
   12f8c:	e59d3014 	ldr	r3, [sp, #20]
   12f90:	e3530000 	cmp	r3, #0	; 0x0
   12f94:	0affff73 	beq	12d68 <readline_into_buffer+0x98>
   12f98:	e3a00008 	mov	r0, #8	; 0x8
   12f9c:	ebfffe0e 	bl	127dc <putc>
   12fa0:	e59d3014 	ldr	r3, [sp, #20]
   12fa4:	e2433001 	sub	r3, r3, #1	; 0x1
   12fa8:	ea0000c7 	b	132cc <readline_into_buffer+0x5fc>
   12fac:	e59d2014 	ldr	r2, [sp, #20]
   12fb0:	e59d3010 	ldr	r3, [sp, #16]
   12fb4:	e1520003 	cmp	r2, r3
   12fb8:	2affff6a 	bcs	12d68 <readline_into_buffer+0x98>
   12fbc:	e2433001 	sub	r3, r3, #1	; 0x1
   12fc0:	e0534002 	subs	r4, r3, r2
   12fc4:	0a00000a 	beq	12ff4 <readline_into_buffer+0x324>
   12fc8:	e2821001 	add	r1, r2, #1	; 0x1
   12fcc:	e0861001 	add	r1, r6, r1
   12fd0:	e0860002 	add	r0, r6, r2
   12fd4:	e1a02004 	mov	r2, r4
   12fd8:	ebffb968 	bl	1580 <memmove>
   12fdc:	e59d2014 	ldr	r2, [sp, #20]
   12fe0:	e59f0394 	ldr	r0, [pc, #916]	; 1337c <readline_into_buffer+0x6ac>
   12fe4:	e0862002 	add	r2, r6, r2
   12fe8:	e1a01004 	mov	r1, r4
   12fec:	e1a0e00f 	mov	lr, pc
   12ff0:	e59bf000 	ldr	pc, [fp]
   12ff4:	e3a00020 	mov	r0, #32	; 0x20
   12ff8:	ebfffdf7 	bl	127dc <putc>
   12ffc:	e2444001 	sub	r4, r4, #1	; 0x1
   13000:	e3a00008 	mov	r0, #8	; 0x8
   13004:	ebfffdf4 	bl	127dc <putc>
   13008:	e3740001 	cmn	r4, #1	; 0x1
   1300c:	1afffffa 	bne	12ffc <readline_into_buffer+0x32c>
   13010:	ea000053 	b	13164 <readline_into_buffer+0x494>
   13014:	e59d2014 	ldr	r2, [sp, #20]
   13018:	e59d3010 	ldr	r3, [sp, #16]
   1301c:	e1520003 	cmp	r2, r3
   13020:	31a04002 	movcc	r4, r2
   13024:	3a000002 	bcc	13034 <readline_into_buffer+0x364>
   13028:	eaffff4e 	b	12d68 <readline_into_buffer+0x98>
   1302c:	ebfffdea 	bl	127dc <putc>
   13030:	e2844001 	add	r4, r4, #1	; 0x1
   13034:	e59d3010 	ldr	r3, [sp, #16]
   13038:	e3a00020 	mov	r0, #32	; 0x20
   1303c:	e1540003 	cmp	r4, r3
   13040:	3afffff9 	bcc	1302c <readline_into_buffer+0x35c>
   13044:	ea000000 	b	1304c <readline_into_buffer+0x37c>
   13048:	ebfffde3 	bl	127dc <putc>
   1304c:	e59d2014 	ldr	r2, [sp, #20]
   13050:	e3a00008 	mov	r0, #8	; 0x8
   13054:	e1540002 	cmp	r4, r2
   13058:	e2444001 	sub	r4, r4, #1	; 0x1
   1305c:	8afffff9 	bhi	13048 <readline_into_buffer+0x378>
   13060:	ea000023 	b	130f4 <readline_into_buffer+0x424>
   13064:	e59d3014 	ldr	r3, [sp, #20]
   13068:	e59d1010 	ldr	r1, [sp, #16]
   1306c:	e1530001 	cmp	r3, r1
   13070:	2affff3c 	bcs	12d68 <readline_into_buffer+0x98>
   13074:	e0631001 	rsb	r1, r3, r1
   13078:	e0862003 	add	r2, r6, r3
   1307c:	e59f02f8 	ldr	r0, [pc, #760]	; 1337c <readline_into_buffer+0x6ac>
   13080:	e1a0e00f 	mov	lr, pc
   13084:	e59bf000 	ldr	pc, [fp]
   13088:	ea00008e 	b	132c8 <readline_into_buffer+0x5f8>
   1308c:	ebfffdd2 	bl	127dc <putc>
   13090:	e59d3014 	ldr	r3, [sp, #20]
   13094:	e2433001 	sub	r3, r3, #1	; 0x1
   13098:	e58d3014 	str	r3, [sp, #20]
   1309c:	e59d2014 	ldr	r2, [sp, #20]
   130a0:	e3a00008 	mov	r0, #8	; 0x8
   130a4:	e3520000 	cmp	r2, #0	; 0x0
   130a8:	1afffff7 	bne	1308c <readline_into_buffer+0x3bc>
   130ac:	e59d3010 	ldr	r3, [sp, #16]
   130b0:	e3530000 	cmp	r3, #0	; 0x0
   130b4:	11a04002 	movne	r4, r2
   130b8:	1a000002 	bne	130c8 <readline_into_buffer+0x3f8>
   130bc:	eaffff29 	b	12d68 <readline_into_buffer+0x98>
   130c0:	ebfffdc5 	bl	127dc <putc>
   130c4:	e2844001 	add	r4, r4, #1	; 0x1
   130c8:	e59d3010 	ldr	r3, [sp, #16]
   130cc:	e3a00020 	mov	r0, #32	; 0x20
   130d0:	e1540003 	cmp	r4, r3
   130d4:	3afffff9 	bcc	130c0 <readline_into_buffer+0x3f0>
   130d8:	ea000000 	b	130e0 <readline_into_buffer+0x410>
   130dc:	ebfffdbe 	bl	127dc <putc>
   130e0:	e59d2014 	ldr	r2, [sp, #20]
   130e4:	e3a00008 	mov	r0, #8	; 0x8
   130e8:	e1540002 	cmp	r4, r2
   130ec:	e2444001 	sub	r4, r4, #1	; 0x1
   130f0:	8afffff9 	bhi	130dc <readline_into_buffer+0x40c>
   130f4:	e58d2010 	str	r2, [sp, #16]
   130f8:	eaffff1a 	b	12d68 <readline_into_buffer+0x98>
   130fc:	e59d0014 	ldr	r0, [sp, #20]
   13100:	e3500000 	cmp	r0, #0	; 0x0
   13104:	0affff17 	beq	12d68 <readline_into_buffer+0x98>
   13108:	e59d2010 	ldr	r2, [sp, #16]
   1310c:	e2403001 	sub	r3, r0, #1	; 0x1
   13110:	e0604002 	rsb	r4, r0, r2
   13114:	e0861000 	add	r1, r6, r0
   13118:	e1a02004 	mov	r2, r4
   1311c:	e0860003 	add	r0, r6, r3
   13120:	e58d3014 	str	r3, [sp, #20]
   13124:	ebffb915 	bl	1580 <memmove>
   13128:	e3a00008 	mov	r0, #8	; 0x8
   1312c:	ebfffdaa 	bl	127dc <putc>
   13130:	e59d2014 	ldr	r2, [sp, #20]
   13134:	e59f0240 	ldr	r0, [pc, #576]	; 1337c <readline_into_buffer+0x6ac>
   13138:	e0862002 	add	r2, r6, r2
   1313c:	e1a01004 	mov	r1, r4
   13140:	e1a0e00f 	mov	lr, pc
   13144:	e59bf000 	ldr	pc, [fp]
   13148:	e3a00020 	mov	r0, #32	; 0x20
   1314c:	ebfffda2 	bl	127dc <putc>
   13150:	e2444001 	sub	r4, r4, #1	; 0x1
   13154:	e3a00008 	mov	r0, #8	; 0x8
   13158:	ebfffd9f 	bl	127dc <putc>
   1315c:	e3740001 	cmn	r4, #1	; 0x1
   13160:	1afffffa 	bne	13150 <readline_into_buffer+0x480>
   13164:	e59d3010 	ldr	r3, [sp, #16]
   13168:	e2433001 	sub	r3, r3, #1	; 0x1
   1316c:	e58d3010 	str	r3, [sp, #16]
   13170:	eafffefc 	b	12d68 <readline_into_buffer+0x98>
   13174:	e59a2000 	ldr	r2, [sl]
   13178:	e3520000 	cmp	r2, #0	; 0x0
   1317c:	ba000024 	blt	13214 <readline_into_buffer+0x544>
   13180:	e2423001 	sub	r3, r2, #1	; 0x1
   13184:	e3730001 	cmn	r3, #1	; 0x1
   13188:	059f01d0 	ldreq	r0, [pc, #464]	; 13360 <readline_into_buffer+0x690>
   1318c:	e58a3000 	str	r3, [sl]
   13190:	05903000 	ldreq	r3, [r0]
   13194:	e59fc1cc 	ldr	ip, [pc, #460]	; 13368 <readline_into_buffer+0x698>
   13198:	058a3000 	streq	r3, [sl]
   1319c:	e59a1000 	ldr	r1, [sl]
   131a0:	e59c3000 	ldr	r3, [ip]
   131a4:	e1510003 	cmp	r1, r3
   131a8:	058a2000 	streq	r2, [sl]
   131ac:	1a000014 	bne	13204 <readline_into_buffer+0x534>
   131b0:	ea000017 	b	13214 <readline_into_buffer+0x544>
   131b4:	e59a1000 	ldr	r1, [sl]
   131b8:	e3510000 	cmp	r1, #0	; 0x0
   131bc:	ba000014 	blt	13214 <readline_into_buffer+0x544>
   131c0:	e59f21a0 	ldr	r2, [pc, #416]	; 13368 <readline_into_buffer+0x698>
   131c4:	e5923000 	ldr	r3, [r2]
   131c8:	e1510003 	cmp	r1, r3
   131cc:	0a000010 	beq	13214 <readline_into_buffer+0x544>
   131d0:	e59f3188 	ldr	r3, [pc, #392]	; 13360 <readline_into_buffer+0x690>
   131d4:	e59fc18c 	ldr	ip, [pc, #396]	; 13368 <readline_into_buffer+0x698>
   131d8:	e5932000 	ldr	r2, [r3]
   131dc:	e2813001 	add	r3, r1, #1	; 0x1
   131e0:	e1530002 	cmp	r3, r2
   131e4:	e58a3000 	str	r3, [sl]
   131e8:	c3a03000 	movgt	r3, #0	; 0x0
   131ec:	c58a3000 	strgt	r3, [sl]
   131f0:	e59a1000 	ldr	r1, [sl]
   131f4:	e59c3000 	ldr	r3, [ip]
   131f8:	e1510003 	cmp	r1, r3
   131fc:	059f517c 	ldreq	r5, [pc, #380]	; 13380 <readline_into_buffer+0x6b0>
   13200:	0a00000a 	beq	13230 <readline_into_buffer+0x560>
   13204:	e59f0160 	ldr	r0, [pc, #352]	; 1336c <readline_into_buffer+0x69c>
   13208:	e7905101 	ldr	r5, [r0, r1, lsl #2]
   1320c:	e3550000 	cmp	r5, #0	; 0x0
   13210:	1a000006 	bne	13230 <readline_into_buffer+0x560>
   13214:	e3a00007 	mov	r0, #7	; 0x7
   13218:	ebfffd6f 	bl	127dc <putc>
   1321c:	eafffed0 	b	12d64 <readline_into_buffer+0x94>
   13220:	ebfffd6d 	bl	127dc <putc>
   13224:	e59d3014 	ldr	r3, [sp, #20]
   13228:	e2433001 	sub	r3, r3, #1	; 0x1
   1322c:	e58d3014 	str	r3, [sp, #20]
   13230:	e59d2014 	ldr	r2, [sp, #20]
   13234:	e3a00008 	mov	r0, #8	; 0x8
   13238:	e3520000 	cmp	r2, #0	; 0x0
   1323c:	1afffff7 	bne	13220 <readline_into_buffer+0x550>
   13240:	e59d3010 	ldr	r3, [sp, #16]
   13244:	e3530000 	cmp	r3, #0	; 0x0
   13248:	11a04002 	movne	r4, r2
   1324c:	1a000002 	bne	1325c <readline_into_buffer+0x58c>
   13250:	ea00000d 	b	1328c <readline_into_buffer+0x5bc>
   13254:	ebfffd60 	bl	127dc <putc>
   13258:	e2844001 	add	r4, r4, #1	; 0x1
   1325c:	e59d3010 	ldr	r3, [sp, #16]
   13260:	e3a00020 	mov	r0, #32	; 0x20
   13264:	e1540003 	cmp	r4, r3
   13268:	3afffff9 	bcc	13254 <readline_into_buffer+0x584>
   1326c:	ea000000 	b	13274 <readline_into_buffer+0x5a4>
   13270:	ebfffd59 	bl	127dc <putc>
   13274:	e59d2014 	ldr	r2, [sp, #20]
   13278:	e3a00008 	mov	r0, #8	; 0x8
   1327c:	e1540002 	cmp	r4, r2
   13280:	e2444001 	sub	r4, r4, #1	; 0x1
   13284:	8afffff9 	bhi	13270 <readline_into_buffer+0x5a0>
   13288:	e58d2010 	str	r2, [sp, #16]
   1328c:	e1a01005 	mov	r1, r5
   13290:	e1a00006 	mov	r0, r6
   13294:	ebffb83f 	bl	1398 <strcpy>
   13298:	e1a00006 	mov	r0, r6
   1329c:	ebffb894 	bl	14f4 <strlen>
   132a0:	e59d3014 	ldr	r3, [sp, #20]
   132a4:	e58d0010 	str	r0, [sp, #16]
   132a8:	e1530000 	cmp	r3, r0
   132ac:	2afffeac 	bcs	12d64 <readline_into_buffer+0x94>
   132b0:	e0631000 	rsb	r1, r3, r0
   132b4:	e0862003 	add	r2, r6, r3
   132b8:	e59f00bc 	ldr	r0, [pc, #188]	; 1337c <readline_into_buffer+0x6ac>
   132bc:	e1a0e00f 	mov	lr, pc
   132c0:	e59bf000 	ldr	pc, [fp]
   132c4:	e3a05000 	mov	r5, #0	; 0x0
   132c8:	e59d3010 	ldr	r3, [sp, #16]
   132cc:	e58d3014 	str	r3, [sp, #20]
   132d0:	eafffea4 	b	12d68 <readline_into_buffer+0x98>
   132d4:	e3a0cc01 	mov	ip, #256	; 0x100
   132d8:	e1a01009 	mov	r1, r9
   132dc:	e28d2014 	add	r2, sp, #20	; 0x14
   132e0:	e28d3010 	add	r3, sp, #16	; 0x10
   132e4:	e88d1040 	stm	sp, {r6, ip}
   132e8:	ebfffe34 	bl	12bc0 <cread_add_char>
   132ec:	eafffe9d 	b	12d68 <readline_into_buffer+0x98>
   132f0:	e59f4070 	ldr	r4, [pc, #112]	; 13368 <readline_into_buffer+0x698>
   132f4:	e59f3070 	ldr	r3, [pc, #112]	; 1336c <readline_into_buffer+0x69c>
   132f8:	e5942000 	ldr	r2, [r4]
   132fc:	e1a01006 	mov	r1, r6
   13300:	e7930102 	ldr	r0, [r3, r2, lsl #2]
   13304:	ebffb823 	bl	1398 <strcpy>
   13308:	e5943000 	ldr	r3, [r4]
   1330c:	e59f104c 	ldr	r1, [pc, #76]	; 13360 <readline_into_buffer+0x690>
   13310:	e2833001 	add	r3, r3, #1	; 0x1
   13314:	e5912000 	ldr	r2, [r1]
   13318:	e3530003 	cmp	r3, #3	; 0x3
   1331c:	c3a03000 	movgt	r3, #0	; 0x0
   13320:	e1530002 	cmp	r3, r2
   13324:	e59f2044 	ldr	r2, [pc, #68]	; 13370 <readline_into_buffer+0x6a0>
   13328:	e5843000 	str	r3, [r4]
   1332c:	c5813000 	strgt	r3, [r1]
   13330:	e5923000 	ldr	r3, [r2]
   13334:	e2833001 	add	r3, r3, #1	; 0x1
   13338:	e5823000 	str	r3, [r2]
   1333c:	e59f3024 	ldr	r3, [pc, #36]	; 13368 <readline_into_buffer+0x698>
   13340:	e1a00005 	mov	r0, r5
   13344:	e5932000 	ldr	r2, [r3]
   13348:	e59f300c 	ldr	r3, [pc, #12]	; 1335c <readline_into_buffer+0x68c>
   1334c:	e5832000 	str	r2, [r3]
   13350:	e28dd01c 	add	sp, sp, #28	; 0x1c
   13354:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   13358:	3c00d274 	.word	0x3c00d274
   1335c:	3c00d114 	.word	0x3c00d114
   13360:	3c00d278 	.word	0x3c00d278
   13364:	3c00d9ac 	.word	0x3c00d9ac
   13368:	3c00d27c 	.word	0x3c00d27c
   1336c:	3c00d99c 	.word	0x3c00d99c
   13370:	3c00d270 	.word	0x3c00d270
   13374:	3c00d6cc 	.word	0x3c00d6cc
   13378:	00016bd7 	.word	0x00016bd7
   1337c:	00018cdc 	.word	0x00018cdc
   13380:	00016bd8 	.word	0x00016bd8

00013384 <readline>:
   13384:	e59f1000 	ldr	r1, [pc, #0]	; 1338c <readline+0x8>
   13388:	eafffe50 	b	12cd0 <readline_into_buffer>
   1338c:	3c00d89c 	.word	0x3c00d89c

00013390 <main_loop>:
   13390:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   13394:	e24dd004 	sub	sp, sp, #4	; 0x4
   13398:	e59f0030 	ldr	r0, [pc, #48]	; 133d0 <main_loop+0x40>
   1339c:	ebfffff8 	bl	13384 <readline>
   133a0:	e1a03000 	mov	r3, r0
   133a4:	e3730001 	cmn	r3, #1	; 0x1
   133a8:	e3a01000 	mov	r1, #0	; 0x0
   133ac:	e59f0020 	ldr	r0, [pc, #32]	; 133d4 <main_loop+0x44>
   133b0:	1a000002 	bne	133c0 <main_loop+0x30>
   133b4:	e59f001c 	ldr	r0, [pc, #28]	; 133d8 <main_loop+0x48>
   133b8:	ebfffcdf 	bl	1273c <puts>
   133bc:	eafffff5 	b	13398 <main_loop+0x8>
   133c0:	e3530000 	cmp	r3, #0	; 0x0
   133c4:	dafffff3 	ble	13398 <main_loop+0x8>
   133c8:	ebfffd41 	bl	128d4 <run_command>
   133cc:	eafffff1 	b	13398 <main_loop+0x8>
   133d0:	00018cfc 	.word	0x00018cfc
   133d4:	3c00d89c 	.word	0x3c00d89c
   133d8:	00016bd7 	.word	0x00016bd7

000133dc <parse_num>:
   133dc:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   133e0:	e1a08001 	mov	r8, r1
   133e4:	e1a07002 	mov	r7, r2
   133e8:	e1a06003 	mov	r6, r3
   133ec:	ea000000 	b	133f4 <parse_num+0x18>
   133f0:	e2800001 	add	r0, r0, #1	; 0x1
   133f4:	e5d03000 	ldrb	r3, [r0]
   133f8:	e3530020 	cmp	r3, #32	; 0x20
   133fc:	0afffffb 	beq	133f0 <parse_num+0x14>
   13400:	ea000037 	b	134e4 <parse_num+0x108>
   13404:	e3520000 	cmp	r2, #0	; 0x0
   13408:	0a00000a 	beq	13438 <parse_num+0x5c>
   1340c:	e3530030 	cmp	r3, #48	; 0x30
   13410:	1a000008 	bne	13438 <parse_num+0x5c>
   13414:	e5d02001 	ldrb	r2, [r0, #1]
   13418:	e2423041 	sub	r3, r2, #65	; 0x41
   1341c:	e6ef3073 	uxtb	r3, r3
   13420:	e3530019 	cmp	r3, #25	; 0x19
   13424:	92823020 	addls	r3, r2, #32	; 0x20
   13428:	96ef2073 	uxtbls	r2, r3
   1342c:	e3520078 	cmp	r2, #120	; 0x78
   13430:	02800002 	addeq	r0, r0, #2	; 0x2
   13434:	03a04010 	moveq	r4, #16	; 0x10
   13438:	e4d0c001 	ldrb	ip, [r0], #1
   1343c:	e24c3030 	sub	r3, ip, #48	; 0x30
   13440:	e24c2041 	sub	r2, ip, #65	; 0x41
   13444:	e6ef3073 	uxtb	r3, r3
   13448:	e6ef2072 	uxtb	r2, r2
   1344c:	e3530009 	cmp	r3, #9	; 0x9
   13450:	83a01000 	movhi	r1, #0	; 0x0
   13454:	93a01001 	movls	r1, #1	; 0x1
   13458:	e3520005 	cmp	r2, #5	; 0x5
   1345c:	83a02000 	movhi	r2, #0	; 0x0
   13460:	93a02001 	movls	r2, #1	; 0x1
   13464:	e1913002 	orrs	r3, r1, r2
   13468:	1a000003 	bne	1347c <parse_num+0xa0>
   1346c:	e24c3061 	sub	r3, ip, #97	; 0x61
   13470:	e6ef3073 	uxtb	r3, r3
   13474:	e3530005 	cmp	r3, #5	; 0x5
   13478:	8a00000f 	bhi	134bc <parse_num+0xe0>
   1347c:	e3510000 	cmp	r1, #0	; 0x0
   13480:	124c2030 	subne	r2, ip, #48	; 0x30
   13484:	1a000007 	bne	134a8 <parse_num+0xcc>
   13488:	e24c3061 	sub	r3, ip, #97	; 0x61
   1348c:	e6ef3073 	uxtb	r3, r3
   13490:	e3530005 	cmp	r3, #5	; 0x5
   13494:	924c2057 	subls	r2, ip, #87	; 0x57
   13498:	9a000002 	bls	134a8 <parse_num+0xcc>
   1349c:	e3520000 	cmp	r2, #0	; 0x0
   134a0:	0a000002 	beq	134b0 <parse_num+0xd4>
   134a4:	e24c2037 	sub	r2, ip, #55	; 0x37
   134a8:	e1520004 	cmp	r2, r4
   134ac:	aa000002 	bge	134bc <parse_num+0xe0>
   134b0:	e0252495 	mla	r5, r5, r4, r2
   134b4:	e3a02000 	mov	r2, #0	; 0x0
   134b8:	ea00000c 	b	134f0 <parse_num+0x114>
   134bc:	e3560000 	cmp	r6, #0	; 0x0
   134c0:	0a000012 	beq	13510 <parse_num+0x134>
   134c4:	e3a03000 	mov	r3, #0	; 0x0
   134c8:	e7d62003 	ldrb	r2, [r6, r3]
   134cc:	e2833001 	add	r3, r3, #1	; 0x1
   134d0:	e3520000 	cmp	r2, #0	; 0x0
   134d4:	0a00000d 	beq	13510 <parse_num+0x134>
   134d8:	e15c0002 	cmp	ip, r2
   134dc:	1afffff9 	bne	134c8 <parse_num+0xec>
   134e0:	ea000005 	b	134fc <parse_num+0x120>
   134e4:	e3a02001 	mov	r2, #1	; 0x1
   134e8:	e3a0400a 	mov	r4, #10	; 0xa
   134ec:	e3a05000 	mov	r5, #0	; 0x0
   134f0:	e5d03000 	ldrb	r3, [r0]
   134f4:	e3530000 	cmp	r3, #0	; 0x0
   134f8:	1affffc1 	bne	13404 <parse_num+0x28>
   134fc:	e3570000 	cmp	r7, #0	; 0x0
   13500:	15870000 	strne	r0, [r7]
   13504:	e5885000 	str	r5, [r8]
   13508:	e3a00001 	mov	r0, #1	; 0x1
   1350c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   13510:	e3a00000 	mov	r0, #0	; 0x0
   13514:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

00013518 <xyzModem_stream_close>:
   13518:	e59f2044 	ldr	r2, [pc, #68]	; 13564 <xyzModem_stream_close+0x4c>
   1351c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   13520:	e5920428 	ldr	r0, [r2, #1064]
   13524:	e59f303c 	ldr	r3, [pc, #60]	; 13568 <xyzModem_stream_close+0x50>
   13528:	e592c424 	ldr	ip, [r2, #1060]
   1352c:	e592e418 	ldr	lr, [r2, #1048]
   13530:	e3500000 	cmp	r0, #0	; 0x0
   13534:	e24dd00c 	sub	sp, sp, #12	; 0xc
   13538:	e59f102c 	ldr	r1, [pc, #44]	; 1356c <xyzModem_stream_close+0x54>
   1353c:	01a01003 	moveq	r1, r3
   13540:	e59f3028 	ldr	r3, [pc, #40]	; 13570 <xyzModem_stream_close+0x58>
   13544:	e88d5000 	stm	sp, {ip, lr}
   13548:	e593c000 	ldr	ip, [r3]
   1354c:	e59f0020 	ldr	r0, [pc, #32]	; 13574 <xyzModem_stream_close+0x5c>
   13550:	e5923420 	ldr	r3, [r2, #1056]
   13554:	e592241c 	ldr	r2, [r2, #1052]
   13558:	e12fff3c 	blx	ip
   1355c:	e28dd00c 	add	sp, sp, #12	; 0xc
   13560:	e8bd8000 	pop	{pc}
   13564:	3c00d290 	.word	0x3c00d290
   13568:	00018d00 	.word	0x00018d00
   1356c:	00018d06 	.word	0x00018d06
   13570:	3c00d6cc 	.word	0x3c00d6cc
   13574:	00018d0a 	.word	0x00018d0a

00013578 <xyzModem_error>:
   13578:	e2800008 	add	r0, r0, #8	; 0x8
   1357c:	e3500007 	cmp	r0, #7	; 0x7
   13580:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
   13584:	ea000017 	b	135e8 <xyzModem_error+0x70>
   13588:	000135e0 	.word	0x000135e0
   1358c:	000135d8 	.word	0x000135d8
   13590:	000135d0 	.word	0x000135d0
   13594:	000135c8 	.word	0x000135c8
   13598:	000135c0 	.word	0x000135c0
   1359c:	000135b8 	.word	0x000135b8
   135a0:	000135a8 	.word	0x000135a8
   135a4:	000135b0 	.word	0x000135b0
   135a8:	e59f0040 	ldr	r0, [pc, #64]	; 135f0 <xyzModem_error+0x78>
   135ac:	e12fff1e 	bx	lr
   135b0:	e59f003c 	ldr	r0, [pc, #60]	; 135f4 <xyzModem_error+0x7c>
   135b4:	e12fff1e 	bx	lr
   135b8:	e59f0038 	ldr	r0, [pc, #56]	; 135f8 <xyzModem_error+0x80>
   135bc:	e12fff1e 	bx	lr
   135c0:	e59f0034 	ldr	r0, [pc, #52]	; 135fc <xyzModem_error+0x84>
   135c4:	e12fff1e 	bx	lr
   135c8:	e59f0030 	ldr	r0, [pc, #48]	; 13600 <xyzModem_error+0x88>
   135cc:	e12fff1e 	bx	lr
   135d0:	e59f002c 	ldr	r0, [pc, #44]	; 13604 <xyzModem_error+0x8c>
   135d4:	e12fff1e 	bx	lr
   135d8:	e59f0028 	ldr	r0, [pc, #40]	; 13608 <xyzModem_error+0x90>
   135dc:	e12fff1e 	bx	lr
   135e0:	e59f0024 	ldr	r0, [pc, #36]	; 1360c <xyzModem_error+0x94>
   135e4:	e12fff1e 	bx	lr
   135e8:	e59f0020 	ldr	r0, [pc, #32]	; 13610 <xyzModem_error+0x98>
   135ec:	e12fff1e 	bx	lr
   135f0:	00018d4b 	.word	0x00018d4b
   135f4:	00018d6b 	.word	0x00018d6b
   135f8:	00018d7d 	.word	0x00018d7d
   135fc:	00018d87 	.word	0x00018d87
   13600:	00018d93 	.word	0x00018d93
   13604:	00018d9d 	.word	0x00018d9d
   13608:	00018dad 	.word	0x00018dad
   1360c:	00018dc0 	.word	0x00018dc0
   13610:	00018dd5 	.word	0x00018dd5

00013614 <delay>:
   13614:	e12fff1e 	bx	lr

00013618 <init_num>:
   13618:	e59f305c 	ldr	r3, [pc, #92]	; 1367c <init_num+0x64>
   1361c:	e3a01001 	mov	r1, #1	; 0x1
   13620:	e5c31000 	strb	r1, [r3]
   13624:	e59f3054 	ldr	r3, [pc, #84]	; 13680 <init_num+0x68>
   13628:	e3a02000 	mov	r2, #0	; 0x0
   1362c:	e5c32000 	strb	r2, [r3]
   13630:	e59f304c 	ldr	r3, [pc, #76]	; 13684 <init_num+0x6c>
   13634:	e5c32000 	strb	r2, [r3]
   13638:	e59f3048 	ldr	r3, [pc, #72]	; 13688 <init_num+0x70>
   1363c:	e5c32000 	strb	r2, [r3]
   13640:	e59f3044 	ldr	r3, [pc, #68]	; 1368c <init_num+0x74>
   13644:	e5c32000 	strb	r2, [r3]
   13648:	e59f3040 	ldr	r3, [pc, #64]	; 13690 <init_num+0x78>
   1364c:	e5832000 	str	r2, [r3]
   13650:	e59f303c 	ldr	r3, [pc, #60]	; 13694 <init_num+0x7c>
   13654:	e5c32000 	strb	r2, [r3]
   13658:	e59f3038 	ldr	r3, [pc, #56]	; 13698 <init_num+0x80>
   1365c:	e5c32000 	strb	r2, [r3]
   13660:	e59f3034 	ldr	r3, [pc, #52]	; 1369c <init_num+0x84>
   13664:	e5c32000 	strb	r2, [r3]
   13668:	e59f3030 	ldr	r3, [pc, #48]	; 136a0 <init_num+0x88>
   1366c:	e5c32000 	strb	r2, [r3]
   13670:	e59f302c 	ldr	r3, [pc, #44]	; 136a4 <init_num+0x8c>
   13674:	e5c32000 	strb	r2, [r3]
   13678:	e12fff1e 	bx	lr
   1367c:	3c00d118 	.word	0x3c00d118
   13680:	3c00d28d 	.word	0x3c00d28d
   13684:	3c00d280 	.word	0x3c00d280
   13688:	3c00d281 	.word	0x3c00d281
   1368c:	3c00d282 	.word	0x3c00d282
   13690:	3c00d284 	.word	0x3c00d284
   13694:	3c00d288 	.word	0x3c00d288
   13698:	3c00d289 	.word	0x3c00d289
   1369c:	3c00d28a 	.word	0x3c00d28a
   136a0:	3c00d28b 	.word	0x3c00d28b
   136a4:	3c00d28c 	.word	0x3c00d28c

000136a8 <out_message>:
   136a8:	e92d4010 	push	{r4, lr}
   136ac:	e59f30c8 	ldr	r3, [pc, #200]	; 1377c <out_message+0xd4>
   136b0:	e59f40c8 	ldr	r4, [pc, #200]	; 13780 <out_message+0xd8>
   136b4:	e5d31000 	ldrb	r1, [r3]
   136b8:	e59f00c4 	ldr	r0, [pc, #196]	; 13784 <out_message+0xdc>
   136bc:	e1a0e00f 	mov	lr, pc
   136c0:	e594f000 	ldr	pc, [r4]
   136c4:	e59f30bc 	ldr	r3, [pc, #188]	; 13788 <out_message+0xe0>
   136c8:	e59f00bc 	ldr	r0, [pc, #188]	; 1378c <out_message+0xe4>
   136cc:	e5d31000 	ldrb	r1, [r3]
   136d0:	e1a0e00f 	mov	lr, pc
   136d4:	e594f000 	ldr	pc, [r4]
   136d8:	e59f30b0 	ldr	r3, [pc, #176]	; 13790 <out_message+0xe8>
   136dc:	e59f00b0 	ldr	r0, [pc, #176]	; 13794 <out_message+0xec>
   136e0:	e5931000 	ldr	r1, [r3]
   136e4:	e1a0e00f 	mov	lr, pc
   136e8:	e594f000 	ldr	pc, [r4]
   136ec:	e59f30a4 	ldr	r3, [pc, #164]	; 13798 <out_message+0xf0>
   136f0:	e59f00a4 	ldr	r0, [pc, #164]	; 1379c <out_message+0xf4>
   136f4:	e5d31000 	ldrb	r1, [r3]
   136f8:	e1a0e00f 	mov	lr, pc
   136fc:	e594f000 	ldr	pc, [r4]
   13700:	e59f3098 	ldr	r3, [pc, #152]	; 137a0 <out_message+0xf8>
   13704:	e59f0098 	ldr	r0, [pc, #152]	; 137a4 <out_message+0xfc>
   13708:	e5d31000 	ldrb	r1, [r3]
   1370c:	e1a0e00f 	mov	lr, pc
   13710:	e594f000 	ldr	pc, [r4]
   13714:	e59f308c 	ldr	r3, [pc, #140]	; 137a8 <out_message+0x100>
   13718:	e59f008c 	ldr	r0, [pc, #140]	; 137ac <out_message+0x104>
   1371c:	e5d31000 	ldrb	r1, [r3]
   13720:	e1a0e00f 	mov	lr, pc
   13724:	e594f000 	ldr	pc, [r4]
   13728:	e59f3080 	ldr	r3, [pc, #128]	; 137b0 <out_message+0x108>
   1372c:	e59f0080 	ldr	r0, [pc, #128]	; 137b4 <out_message+0x10c>
   13730:	e5d31000 	ldrb	r1, [r3]
   13734:	e1a0e00f 	mov	lr, pc
   13738:	e594f000 	ldr	pc, [r4]
   1373c:	e59f3074 	ldr	r3, [pc, #116]	; 137b8 <out_message+0x110>
   13740:	e59f0074 	ldr	r0, [pc, #116]	; 137bc <out_message+0x114>
   13744:	e5d31000 	ldrb	r1, [r3]
   13748:	e1a0e00f 	mov	lr, pc
   1374c:	e594f000 	ldr	pc, [r4]
   13750:	e59f3068 	ldr	r3, [pc, #104]	; 137c0 <out_message+0x118>
   13754:	e59f0068 	ldr	r0, [pc, #104]	; 137c4 <out_message+0x11c>
   13758:	e5d31000 	ldrb	r1, [r3]
   1375c:	e1a0e00f 	mov	lr, pc
   13760:	e594f000 	ldr	pc, [r4]
   13764:	e59f305c 	ldr	r3, [pc, #92]	; 137c8 <out_message+0x120>
   13768:	e59f005c 	ldr	r0, [pc, #92]	; 137cc <out_message+0x124>
   1376c:	e5d31000 	ldrb	r1, [r3]
   13770:	e1a0e00f 	mov	lr, pc
   13774:	e594f000 	ldr	pc, [r4]
   13778:	e8bd8010 	pop	{r4, pc}
   1377c:	3c00d280 	.word	0x3c00d280
   13780:	3c00d6cc 	.word	0x3c00d6cc
   13784:	00018de3 	.word	0x00018de3
   13788:	3c00d281 	.word	0x3c00d281
   1378c:	00018df1 	.word	0x00018df1
   13790:	3c00d284 	.word	0x3c00d284
   13794:	00018e04 	.word	0x00018e04
   13798:	3c00d288 	.word	0x3c00d288
   1379c:	00018e16 	.word	0x00018e16
   137a0:	3c00d289 	.word	0x3c00d289
   137a4:	00018e29 	.word	0x00018e29
   137a8:	3c00d28a 	.word	0x3c00d28a
   137ac:	00018e40 	.word	0x00018e40
   137b0:	3c00d118 	.word	0x3c00d118
   137b4:	00018e58 	.word	0x00018e58
   137b8:	3c00d28b 	.word	0x3c00d28b
   137bc:	00018e67 	.word	0x00018e67
   137c0:	3c00d28c 	.word	0x3c00d28c
   137c4:	00018e80 	.word	0x00018e80
   137c8:	3c00d28d 	.word	0x3c00d28d
   137cc:	00018e92 	.word	0x00018e92

000137d0 <Procpacket>:
   137d0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   137d4:	e59fc1ac 	ldr	ip, [pc, #428]	; 13988 <Procpacket+0x1b8>
   137d8:	e59f21ac 	ldr	r2, [pc, #428]	; 1398c <Procpacket+0x1bc>
   137dc:	e5dc3000 	ldrb	r3, [ip]
   137e0:	e1a06000 	mov	r6, r0
   137e4:	e5c03000 	strb	r3, [r0]
   137e8:	e5923000 	ldr	r3, [r2]
   137ec:	e1a05001 	mov	r5, r1
   137f0:	e5c03001 	strb	r3, [r0, #1]
   137f4:	e5923000 	ldr	r3, [r2]
   137f8:	e1e03003 	mvn	r3, r3
   137fc:	e5c03002 	strb	r3, [r0, #2]
   13800:	e59f3188 	ldr	r3, [pc, #392]	; 13990 <Procpacket+0x1c0>
   13804:	e5922000 	ldr	r2, [r2]
   13808:	e5933000 	ldr	r3, [r3]
   1380c:	e1520003 	cmp	r2, r3
   13810:	1a000008 	bne	13838 <Procpacket+0x68>
   13814:	e59f3178 	ldr	r3, [pc, #376]	; 13994 <Procpacket+0x1c4>
   13818:	e5d32000 	ldrb	r2, [r3]
   1381c:	e59f3174 	ldr	r3, [pc, #372]	; 13998 <Procpacket+0x1c8>
   13820:	e3520001 	cmp	r2, #1	; 0x1
   13824:	e5934000 	ldr	r4, [r3]
   13828:	e5cc2000 	strb	r2, [ip]
   1382c:	03a07080 	moveq	r7, #128	; 0x80
   13830:	13a07b01 	movne	r7, #1024	; 0x400
   13834:	ea000004 	b	1384c <Procpacket+0x7c>
   13838:	e5dc3000 	ldrb	r3, [ip]
   1383c:	e3530001 	cmp	r3, #1	; 0x1
   13840:	13a04b01 	movne	r4, #1024	; 0x400
   13844:	03a04080 	moveq	r4, #128	; 0x80
   13848:	e1a07004 	mov	r7, r4
   1384c:	e59f3148 	ldr	r3, [pc, #328]	; 1399c <Procpacket+0x1cc>
   13850:	e5d33000 	ldrb	r3, [r3]
   13854:	e3530000 	cmp	r3, #0	; 0x0
   13858:	1a000008 	bne	13880 <Procpacket+0xb0>
   1385c:	e59f313c 	ldr	r3, [pc, #316]	; 139a0 <Procpacket+0x1d0>
   13860:	e2860003 	add	r0, r6, #3	; 0x3
   13864:	e5931000 	ldr	r1, [r3]
   13868:	e59f3134 	ldr	r3, [pc, #308]	; 139a4 <Procpacket+0x1d4>
   1386c:	e1a02004 	mov	r2, r4
   13870:	e5933000 	ldr	r3, [r3]
   13874:	e0631001 	rsb	r1, r3, r1
   13878:	e0851001 	add	r1, r5, r1
   1387c:	ebffb736 	bl	155c <memcpy>
   13880:	e59f111c 	ldr	r1, [pc, #284]	; 139a4 <Procpacket+0x1d4>
   13884:	e59f311c 	ldr	r3, [pc, #284]	; 139a8 <Procpacket+0x1d8>
   13888:	e5912000 	ldr	r2, [r1]
   1388c:	e5d33000 	ldrb	r3, [r3]
   13890:	e0642002 	rsb	r2, r4, r2
   13894:	e3530000 	cmp	r3, #0	; 0x0
   13898:	e5812000 	str	r2, [r1]
   1389c:	0a000006 	beq	138bc <Procpacket+0xec>
   138a0:	e1a02007 	mov	r2, r7
   138a4:	e2860003 	add	r0, r6, #3	; 0x3
   138a8:	e3a01000 	mov	r1, #0	; 0x0
   138ac:	ebffb722 	bl	153c <memset>
   138b0:	e59f30f4 	ldr	r3, [pc, #244]	; 139ac <Procpacket+0x1dc>
   138b4:	e3a02001 	mov	r2, #1	; 0x1
   138b8:	e5c32000 	strb	r2, [r3]
   138bc:	e59f30c8 	ldr	r3, [pc, #200]	; 1398c <Procpacket+0x1bc>
   138c0:	e5932000 	ldr	r2, [r3]
   138c4:	e59f30c4 	ldr	r3, [pc, #196]	; 13990 <Procpacket+0x1c0>
   138c8:	e5933000 	ldr	r3, [r3]
   138cc:	e1520003 	cmp	r2, r3
   138d0:	1a000004 	bne	138e8 <Procpacket+0x118>
   138d4:	e2840003 	add	r0, r4, #3	; 0x3
   138d8:	e0860000 	add	r0, r6, r0
   138dc:	e0642007 	rsb	r2, r4, r7
   138e0:	e3a0101a 	mov	r1, #26	; 0x1a
   138e4:	ebffb714 	bl	153c <memset>
   138e8:	e3a00000 	mov	r0, #0	; 0x0
   138ec:	e59f80bc 	ldr	r8, [pc, #188]	; 139b0 <Procpacket+0x1e0>
   138f0:	e1a04006 	mov	r4, r6
   138f4:	e1a0c000 	mov	ip, r0
   138f8:	e1a05000 	mov	r5, r0
   138fc:	e1a0342c 	lsr	r3, ip, #8
   13900:	e5d42003 	ldrb	r2, [r4, #3]
   13904:	e1a03083 	lsl	r3, r3, #1
   13908:	e19810b3 	ldrh	r1, [r8, r3]
   1390c:	e2800001 	add	r0, r0, #1	; 0x1
   13910:	e182340c 	orr	r3, r2, ip, lsl #8
   13914:	e0233001 	eor	r3, r3, r1
   13918:	e0852002 	add	r2, r5, r2
   1391c:	e1500007 	cmp	r0, r7
   13920:	e6ffc073 	uxth	ip, r3
   13924:	e6ef5072 	uxtb	r5, r2
   13928:	e2844001 	add	r4, r4, #1	; 0x1
   1392c:	e59f107c 	ldr	r1, [pc, #124]	; 139b0 <Procpacket+0x1e0>
   13930:	3afffff1 	bcc	138fc <Procpacket+0x12c>
   13934:	e59f3078 	ldr	r3, [pc, #120]	; 139b4 <Procpacket+0x1e4>
   13938:	e0860007 	add	r0, r6, r7
   1393c:	e5d33000 	ldrb	r3, [r3]
   13940:	e3530000 	cmp	r3, #0	; 0x0
   13944:	05c05003 	strbeq	r5, [r0, #3]
   13948:	0a00000c 	beq	13980 <Procpacket+0x1b0>
   1394c:	e1a0342c 	lsr	r3, ip, #8
   13950:	e1a03083 	lsl	r3, r3, #1
   13954:	e19120b3 	ldrh	r2, [r1, r3]
   13958:	e022240c 	eor	r2, r2, ip, lsl #8
   1395c:	e1a03442 	asr	r3, r2, #8
   13960:	e20330ff 	and	r3, r3, #255	; 0xff
   13964:	e1a03083 	lsl	r3, r3, #1
   13968:	e19130b3 	ldrh	r3, [r1, r3]
   1396c:	e0233402 	eor	r3, r3, r2, lsl #8
   13970:	e6ff3073 	uxth	r3, r3
   13974:	e1a02423 	lsr	r2, r3, #8
   13978:	e5c03004 	strb	r3, [r0, #4]
   1397c:	e5c02003 	strb	r2, [r0, #3]
   13980:	e3a00000 	mov	r0, #0	; 0x0
   13984:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
   13988:	3c00d282 	.word	0x3c00d282
   1398c:	3c00d284 	.word	0x3c00d284
   13990:	3c00ddac 	.word	0x3c00ddac
   13994:	3c00ddb4 	.word	0x3c00ddb4
   13998:	3c00ddb0 	.word	0x3c00ddb0
   1399c:	3c00d288 	.word	0x3c00d288
   139a0:	3c00d72c 	.word	0x3c00d72c
   139a4:	3c00d730 	.word	0x3c00d730
   139a8:	3c00d28d 	.word	0x3c00d28d
   139ac:	3c00d28c 	.word	0x3c00d28c
   139b0:	00015d54 	.word	0x00015d54
   139b4:	3c00d728 	.word	0x3c00d728

000139b8 <procsend>:
   139b8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   139bc:	e59f31f8 	ldr	r3, [pc, #504]	; 13bbc <procsend+0x204>
   139c0:	e1a06000 	mov	r6, r0
   139c4:	e5d30000 	ldrb	r0, [r3]
   139c8:	e24dd004 	sub	sp, sp, #4	; 0x4
   139cc:	e3500000 	cmp	r0, #0	; 0x0
   139d0:	e1a0c002 	mov	ip, r2
   139d4:	e59f41e4 	ldr	r4, [pc, #484]	; 13bc0 <procsend+0x208>
   139d8:	0a000065 	beq	13b74 <procsend+0x1bc>
   139dc:	e59f31e0 	ldr	r3, [pc, #480]	; 13bc4 <procsend+0x20c>
   139e0:	e1a02b02 	lsl	r2, r2, #22
   139e4:	e1a02b22 	lsr	r2, r2, #22
   139e8:	e583c000 	str	ip, [r3]
   139ec:	e59f31d4 	ldr	r3, [pc, #468]	; 13bc8 <procsend+0x210>
   139f0:	e59f01d4 	ldr	r0, [pc, #468]	; 13bcc <procsend+0x214>
   139f4:	e3520000 	cmp	r2, #0	; 0x0
   139f8:	e1a0152c 	lsr	r1, ip, #10
   139fc:	e583c000 	str	ip, [r3]
   13a00:	03a03b01 	moveq	r3, #1024	; 0x400
   13a04:	e5802000 	str	r2, [r0]
   13a08:	e5841000 	str	r1, [r4]
   13a0c:	05803000 	streq	r3, [r0]
   13a10:	0a000006 	beq	13a30 <procsend+0x78>
   13a14:	e2813001 	add	r3, r1, #1	; 0x1
   13a18:	e352007f 	cmp	r2, #127	; 0x7f
   13a1c:	e59f21ac 	ldr	r2, [pc, #428]	; 13bd0 <procsend+0x218>
   13a20:	e5843000 	str	r3, [r4]
   13a24:	93a03001 	movls	r3, #1	; 0x1
   13a28:	83a03002 	movhi	r3, #2	; 0x2
   13a2c:	e5c23000 	strb	r3, [r2]
   13a30:	e59f019c 	ldr	r0, [pc, #412]	; 13bd4 <procsend+0x21c>
   13a34:	ebffb6ae 	bl	14f4 <strlen>
   13a38:	e59f2198 	ldr	r2, [pc, #408]	; 13bd8 <procsend+0x220>
   13a3c:	e3500080 	cmp	r0, #128	; 0x80
   13a40:	83a03002 	movhi	r3, #2	; 0x2
   13a44:	93a03001 	movls	r3, #1	; 0x1
   13a48:	85c23000 	strbhi	r3, [r2]
   13a4c:	95c23000 	strbls	r3, [r2]
   13a50:	e59f3184 	ldr	r3, [pc, #388]	; 13bdc <procsend+0x224>
   13a54:	83a07b01 	movhi	r7, #1024	; 0x400
   13a58:	e5d31000 	ldrb	r1, [r3]
   13a5c:	93a07080 	movls	r7, #128	; 0x80
   13a60:	e3510000 	cmp	r1, #0	; 0x0
   13a64:	13a01000 	movne	r1, #0	; 0x0
   13a68:	12872005 	addne	r2, r7, #5	; 0x5
   13a6c:	02872004 	addeq	r2, r7, #4	; 0x4
   13a70:	11a00006 	movne	r0, r6
   13a74:	01a00006 	moveq	r0, r6
   13a78:	ebffb6af 	bl	153c <memset>
   13a7c:	e59f3154 	ldr	r3, [pc, #340]	; 13bd8 <procsend+0x220>
   13a80:	e3a05000 	mov	r5, #0	; 0x0
   13a84:	e5d33000 	ldrb	r3, [r3]
   13a88:	e59f0144 	ldr	r0, [pc, #324]	; 13bd4 <procsend+0x21c>
   13a8c:	e5c63000 	strb	r3, [r6]
   13a90:	e3e03000 	mvn	r3, #0	; 0x0
   13a94:	e5c63002 	strb	r3, [r6, #2]
   13a98:	e5c65001 	strb	r5, [r6, #1]
   13a9c:	ebffb694 	bl	14f4 <strlen>
   13aa0:	e59f112c 	ldr	r1, [pc, #300]	; 13bd4 <procsend+0x21c>
   13aa4:	e1a02000 	mov	r2, r0
   13aa8:	e1a04000 	mov	r4, r0
   13aac:	e2860003 	add	r0, r6, #3	; 0x3
   13ab0:	ebffb63f 	bl	13b4 <strncpy>
   13ab4:	e59f3108 	ldr	r3, [pc, #264]	; 13bc4 <procsend+0x20c>
   13ab8:	e2844004 	add	r4, r4, #4	; 0x4
   13abc:	e0860004 	add	r0, r6, r4
   13ac0:	e5932000 	ldr	r2, [r3]
   13ac4:	e59f1114 	ldr	r1, [pc, #276]	; 13be0 <procsend+0x228>
   13ac8:	e59f4114 	ldr	r4, [pc, #276]	; 13be4 <procsend+0x22c>
   13acc:	ebfffab8 	bl	125b4 <sprintf>
   13ad0:	e1a00006 	mov	r0, r6
   13ad4:	e1a0c005 	mov	ip, r5
   13ad8:	e1a0e005 	mov	lr, r5
   13adc:	e1a0342c 	lsr	r3, ip, #8
   13ae0:	e5d02003 	ldrb	r2, [r0, #3]
   13ae4:	e1a03083 	lsl	r3, r3, #1
   13ae8:	e19410b3 	ldrh	r1, [r4, r3]
   13aec:	e2855001 	add	r5, r5, #1	; 0x1
   13af0:	e182340c 	orr	r3, r2, ip, lsl #8
   13af4:	e0233001 	eor	r3, r3, r1
   13af8:	e08e2002 	add	r2, lr, r2
   13afc:	e1550007 	cmp	r5, r7
   13b00:	e6ffc073 	uxth	ip, r3
   13b04:	e6efe072 	uxtb	lr, r2
   13b08:	e2800001 	add	r0, r0, #1	; 0x1
   13b0c:	e59f10d0 	ldr	r1, [pc, #208]	; 13be4 <procsend+0x22c>
   13b10:	3afffff1 	bcc	13adc <procsend+0x124>
   13b14:	e59f30c0 	ldr	r3, [pc, #192]	; 13bdc <procsend+0x224>
   13b18:	e0860007 	add	r0, r6, r7
   13b1c:	e5d33000 	ldrb	r3, [r3]
   13b20:	e3530000 	cmp	r3, #0	; 0x0
   13b24:	05c0e003 	strbeq	lr, [r0, #3]
   13b28:	0a00000c 	beq	13b60 <procsend+0x1a8>
   13b2c:	e1a0342c 	lsr	r3, ip, #8
   13b30:	e1a03083 	lsl	r3, r3, #1
   13b34:	e19120b3 	ldrh	r2, [r1, r3]
   13b38:	e022240c 	eor	r2, r2, ip, lsl #8
   13b3c:	e1a03442 	asr	r3, r2, #8
   13b40:	e20330ff 	and	r3, r3, #255	; 0xff
   13b44:	e1a03083 	lsl	r3, r3, #1
   13b48:	e19130b3 	ldrh	r3, [r1, r3]
   13b4c:	e0233402 	eor	r3, r3, r2, lsl #8
   13b50:	e6ff3073 	uxth	r3, r3
   13b54:	e1a02423 	lsr	r2, r3, #8
   13b58:	e5c03004 	strb	r3, [r0, #4]
   13b5c:	e5c02003 	strb	r2, [r0, #3]
   13b60:	e59f3080 	ldr	r3, [pc, #128]	; 13be8 <procsend+0x230>
   13b64:	e3a02001 	mov	r2, #1	; 0x1
   13b68:	e5c32000 	strb	r2, [r3]
   13b6c:	e3a00000 	mov	r0, #0	; 0x0
   13b70:	ea00000f 	b	13bb4 <procsend+0x1fc>
   13b74:	e59f2070 	ldr	r2, [pc, #112]	; 13bec <procsend+0x234>
   13b78:	e5943000 	ldr	r3, [r4]
   13b7c:	e5922000 	ldr	r2, [r2]
   13b80:	e1520003 	cmp	r2, r3
   13b84:	8a00000a 	bhi	13bb4 <procsend+0x1fc>
   13b88:	e59f3060 	ldr	r3, [pc, #96]	; 13bf0 <procsend+0x238>
   13b8c:	e59f2044 	ldr	r2, [pc, #68]	; 13bd8 <procsend+0x220>
   13b90:	e5d33000 	ldrb	r3, [r3]
   13b94:	e1a00006 	mov	r0, r6
   13b98:	e3530000 	cmp	r3, #0	; 0x0
   13b9c:	13a03001 	movne	r3, #1	; 0x1
   13ba0:	03a03002 	moveq	r3, #2	; 0x2
   13ba4:	e5c23000 	strb	r3, [r2]
   13ba8:	e1a0200c 	mov	r2, ip
   13bac:	ebffff07 	bl	137d0 <Procpacket>
   13bb0:	e6ef0070 	uxtb	r0, r0
   13bb4:	e28dd004 	add	sp, sp, #4	; 0x4
   13bb8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   13bbc:	3c00d118 	.word	0x3c00d118
   13bc0:	3c00ddac 	.word	0x3c00ddac
   13bc4:	3c00d730 	.word	0x3c00d730
   13bc8:	3c00d72c 	.word	0x3c00d72c
   13bcc:	3c00ddb0 	.word	0x3c00ddb0
   13bd0:	3c00ddb4 	.word	0x3c00ddb4
   13bd4:	00018ea1 	.word	0x00018ea1
   13bd8:	3c00d282 	.word	0x3c00d282
   13bdc:	3c00d728 	.word	0x3c00d728
   13be0:	00017f8f 	.word	0x00017f8f
   13be4:	00015d54 	.word	0x00015d54
   13be8:	3c00d281 	.word	0x3c00d281
   13bec:	3c00d284 	.word	0x3c00d284
   13bf0:	3c00d28d 	.word	0x3c00d28d

00013bf4 <CYGACC_COMM_IF_PUTC>:
   13bf4:	e1a00001 	mov	r0, r1
   13bf8:	eafffaf7 	b	127dc <putc>

00013bfc <real_send>:
   13bfc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   13c00:	e59f30ec 	ldr	r3, [pc, #236]	; 13cf4 <real_send+0xf8>
   13c04:	e24ddb01 	sub	sp, sp, #1024	; 0x400
   13c08:	e5d33000 	ldrb	r3, [r3]
   13c0c:	e24dd00c 	sub	sp, sp, #12	; 0xc
   13c10:	e3530000 	cmp	r3, #0	; 0x0
   13c14:	e1a0e000 	mov	lr, r0
   13c18:	e1a0c001 	mov	ip, r1
   13c1c:	159f30d4 	ldrne	r3, [pc, #212]	; 13cf8 <real_send+0xfc>
   13c20:	13a02000 	movne	r2, #0	; 0x0
   13c24:	15832000 	strne	r2, [r3]
   13c28:	1a000006 	bne	13c48 <real_send+0x4c>
   13c2c:	e59f30c8 	ldr	r3, [pc, #200]	; 13cfc <real_send+0x100>
   13c30:	e5d33000 	ldrb	r3, [r3]
   13c34:	e3530000 	cmp	r3, #0	; 0x0
   13c38:	059f20b8 	ldreq	r2, [pc, #184]	; 13cf8 <real_send+0xfc>
   13c3c:	05923000 	ldreq	r3, [r2]
   13c40:	02833001 	addeq	r3, r3, #1	; 0x1
   13c44:	05823000 	streq	r3, [r2]
   13c48:	e28d0008 	add	r0, sp, #8	; 0x8
   13c4c:	e2400005 	sub	r0, r0, #5	; 0x5
   13c50:	e1a0100e 	mov	r1, lr
   13c54:	e1a0200c 	mov	r2, ip
   13c58:	ebffff56 	bl	139b8 <procsend>
   13c5c:	e2507000 	subs	r7, r0, #0	; 0x0
   13c60:	1a00001f 	bne	13ce4 <real_send+0xe8>
   13c64:	e59f3094 	ldr	r3, [pc, #148]	; 13d00 <real_send+0x104>
   13c68:	e28d5008 	add	r5, sp, #8	; 0x8
   13c6c:	e5d31000 	ldrb	r1, [r3]
   13c70:	e59f308c 	ldr	r3, [pc, #140]	; 13d04 <real_send+0x108>
   13c74:	e3510001 	cmp	r1, #1	; 0x1
   13c78:	e5d32000 	ldrb	r2, [r3]
   13c7c:	e3003404 	movw	r3, #1028	; 0x404
   13c80:	11a06003 	movne	r6, r3
   13c84:	03a06084 	moveq	r6, #132	; 0x84
   13c88:	e3520000 	cmp	r2, #0	; 0x0
   13c8c:	12866001 	addne	r6, r6, #1	; 0x1
   13c90:	e2455005 	sub	r5, r5, #5	; 0x5
   13c94:	e3a04000 	mov	r4, #0	; 0x0
   13c98:	e7d50004 	ldrb	r0, [r5, r4]
   13c9c:	e2844001 	add	r4, r4, #1	; 0x1
   13ca0:	ebfffacd 	bl	127dc <putc>
   13ca4:	e1540006 	cmp	r4, r6
   13ca8:	3afffffa 	bcc	13c98 <real_send+0x9c>
   13cac:	e59f3054 	ldr	r3, [pc, #84]	; 13d08 <real_send+0x10c>
   13cb0:	e5d33000 	ldrb	r3, [r3]
   13cb4:	e3530000 	cmp	r3, #0	; 0x0
   13cb8:	13e07000 	mvnne	r7, #0	; 0x0
   13cbc:	1a000008 	bne	13ce4 <real_send+0xe8>
   13cc0:	e59f3030 	ldr	r3, [pc, #48]	; 13cf8 <real_send+0xfc>
   13cc4:	e5932000 	ldr	r2, [r3]
   13cc8:	e59f303c 	ldr	r3, [pc, #60]	; 13d0c <real_send+0x110>
   13ccc:	e5933000 	ldr	r3, [r3]
   13cd0:	e1520003 	cmp	r2, r3
   13cd4:	059f3034 	ldreq	r3, [pc, #52]	; 13d10 <real_send+0x114>
   13cd8:	03a02001 	moveq	r2, #1	; 0x1
   13cdc:	05c32000 	strbeq	r2, [r3]
   13ce0:	03a07003 	moveq	r7, #3	; 0x3
   13ce4:	e1a00007 	mov	r0, r7
   13ce8:	e28dd00c 	add	sp, sp, #12	; 0xc
   13cec:	e28ddb01 	add	sp, sp, #1024	; 0x400
   13cf0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   13cf4:	3c00d28d 	.word	0x3c00d28d
   13cf8:	3c00d284 	.word	0x3c00d284
   13cfc:	3c00d118 	.word	0x3c00d118
   13d00:	3c00d282 	.word	0x3c00d282
   13d04:	3c00d728 	.word	0x3c00d728
   13d08:	3c00d28c 	.word	0x3c00d28c
   13d0c:	3c00ddac 	.word	0x3c00ddac
   13d10:	3c00d288 	.word	0x3c00d288

00013d14 <CYGACC_COMM_IF_GETC_TIMEOUT>:
   13d14:	e92d4070 	push	{r4, r5, r6, lr}
   13d18:	e1a05001 	mov	r5, r1
   13d1c:	e3a04000 	mov	r4, #0	; 0x0
   13d20:	e59f603c 	ldr	r6, [pc, #60]	; 13d64 <CYGACC_COMM_IF_GETC_TIMEOUT+0x50>
   13d24:	ea000000 	b	13d2c <CYGACC_COMM_IF_GETC_TIMEOUT+0x18>
   13d28:	ebffe72d 	bl	d9e4 <udelay>
   13d2c:	ebfffaab 	bl	127e0 <tstc>
   13d30:	e3500000 	cmp	r0, #0	; 0x0
   13d34:	e3a00014 	mov	r0, #20	; 0x14
   13d38:	1a000002 	bne	13d48 <CYGACC_COMM_IF_GETC_TIMEOUT+0x34>
   13d3c:	e1540006 	cmp	r4, r6
   13d40:	e2844001 	add	r4, r4, #1	; 0x1
   13d44:	1afffff7 	bne	13d28 <CYGACC_COMM_IF_GETC_TIMEOUT+0x14>
   13d48:	ebfffaa4 	bl	127e0 <tstc>
   13d4c:	e3500000 	cmp	r0, #0	; 0x0
   13d50:	08bd8070 	popeq	{r4, r5, r6, pc}
   13d54:	ebfffaa2 	bl	127e4 <getc>
   13d58:	e5c50000 	strb	r0, [r5]
   13d5c:	e3a00001 	mov	r0, #1	; 0x1
   13d60:	e8bd8070 	pop	{r4, r5, r6, pc}
   13d64:	000186a0 	.word	0x000186a0

00013d68 <control_send>:
   13d68:	e59f323c 	ldr	r3, [pc, #572]	; 13fac <control_send+0x244>
   13d6c:	e3a02000 	mov	r2, #0	; 0x0
   13d70:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   13d74:	e5c32000 	strb	r2, [r3]
   13d78:	e59f3230 	ldr	r3, [pc, #560]	; 13fb0 <control_send+0x248>
   13d7c:	e59f4230 	ldr	r4, [pc, #560]	; 13fb4 <control_send+0x24c>
   13d80:	e5c32000 	strb	r2, [r3]
   13d84:	e59f322c 	ldr	r3, [pc, #556]	; 13fb8 <control_send+0x250>
   13d88:	e59f522c 	ldr	r5, [pc, #556]	; 13fbc <control_send+0x254>
   13d8c:	e5832000 	str	r2, [r3]
   13d90:	e3a03001 	mov	r3, #1	; 0x1
   13d94:	e59fe224 	ldr	lr, [pc, #548]	; 13fc0 <control_send+0x258>
   13d98:	e59f8224 	ldr	r8, [pc, #548]	; 13fc4 <control_send+0x25c>
   13d9c:	e59f6224 	ldr	r6, [pc, #548]	; 13fc8 <control_send+0x260>
   13da0:	e59f7224 	ldr	r7, [pc, #548]	; 13fcc <control_send+0x264>
   13da4:	e59fc224 	ldr	ip, [pc, #548]	; 13fd0 <control_send+0x268>
   13da8:	e5c43000 	strb	r3, [r4]
   13dac:	e59f3220 	ldr	r3, [pc, #544]	; 13fd4 <control_send+0x26c>
   13db0:	e24dd014 	sub	sp, sp, #20	; 0x14
   13db4:	e5c52000 	strb	r2, [r5]
   13db8:	e5c32000 	strb	r2, [r3]
   13dbc:	e58d0004 	str	r0, [sp, #4]
   13dc0:	e1a0b001 	mov	fp, r1
   13dc4:	e5c82000 	strb	r2, [r8]
   13dc8:	e5c62000 	strb	r2, [r6]
   13dcc:	e5c72000 	strb	r2, [r7]
   13dd0:	e5ce2000 	strb	r2, [lr]
   13dd4:	e5cc2000 	strb	r2, [ip]
   13dd8:	e1a0a002 	mov	sl, r2
   13ddc:	e1a0900e 	mov	r9, lr
   13de0:	e59f51f0 	ldr	r5, [pc, #496]	; 13fd8 <control_send+0x270>
   13de4:	ea000000 	b	13dec <control_send+0x84>
   13de8:	e3a0a000 	mov	sl, #0	; 0x0
   13dec:	e3a00000 	mov	r0, #0	; 0x0
   13df0:	e28d100f 	add	r1, sp, #15	; 0xf
   13df4:	ebffffc6 	bl	13d14 <CYGACC_COMM_IF_GETC_TIMEOUT>
   13df8:	e3500000 	cmp	r0, #0	; 0x0
   13dfc:	0afffffa 	beq	13dec <control_send+0x84>
   13e00:	e5dd300f 	ldrb	r3, [sp, #15]
   13e04:	e3530018 	cmp	r3, #24	; 0x18
   13e08:	0a000042 	beq	13f18 <control_send+0x1b0>
   13e0c:	8a000004 	bhi	13e24 <control_send+0xbc>
   13e10:	e3530006 	cmp	r3, #6	; 0x6
   13e14:	0a00002c 	beq	13ecc <control_send+0x164>
   13e18:	e3530015 	cmp	r3, #21	; 0x15
   13e1c:	1afffff1 	bne	13de8 <control_send+0x80>
   13e20:	ea000010 	b	13e68 <control_send+0x100>
   13e24:	e3530043 	cmp	r3, #67	; 0x43
   13e28:	0a000001 	beq	13e34 <control_send+0xcc>
   13e2c:	e3530063 	cmp	r3, #99	; 0x63
   13e30:	1affffec 	bne	13de8 <control_send+0x80>
   13e34:	e5d73000 	ldrb	r3, [r7]
   13e38:	e5d42000 	ldrb	r2, [r4]
   13e3c:	e5d61000 	ldrb	r1, [r6]
   13e40:	e1833002 	orr	r3, r3, r2
   13e44:	e1913003 	orrs	r3, r1, r3
   13e48:	0affffe6 	beq	13de8 <control_send+0x80>
   13e4c:	e3510000 	cmp	r1, #0	; 0x0
   13e50:	159f2178 	ldrne	r2, [pc, #376]	; 13fd0 <control_send+0x268>
   13e54:	13a03001 	movne	r3, #1	; 0x1
   13e58:	15c23000 	strbne	r3, [r2]
   13e5c:	e3a03001 	mov	r3, #1	; 0x1
   13e60:	e5c53000 	strb	r3, [r5]
   13e64:	ea00002f 	b	13f28 <control_send+0x1c0>
   13e68:	e59f313c 	ldr	r3, [pc, #316]	; 13fac <control_send+0x244>
   13e6c:	e5d33000 	ldrb	r3, [r3]
   13e70:	e3530000 	cmp	r3, #0	; 0x0
   13e74:	0a000008 	beq	13e9c <control_send+0x134>
   13e78:	e59f2138 	ldr	r2, [pc, #312]	; 13fb8 <control_send+0x250>
   13e7c:	e5923000 	ldr	r3, [r2]
   13e80:	e2433001 	sub	r3, r3, #1	; 0x1
   13e84:	e5823000 	str	r3, [r2]
   13e88:	e3a03001 	mov	r3, #1	; 0x1
   13e8c:	e5c43000 	strb	r3, [r4]
   13e90:	e3a03000 	mov	r3, #0	; 0x0
   13e94:	e5c93000 	strb	r3, [r9]
   13e98:	ea000022 	b	13f28 <control_send+0x1c0>
   13e9c:	e5d42000 	ldrb	r2, [r4]
   13ea0:	e3520000 	cmp	r2, #0	; 0x0
   13ea4:	15c53000 	strbne	r3, [r5]
   13ea8:	1a000007 	bne	13ecc <control_send+0x164>
   13eac:	e5d93000 	ldrb	r3, [r9]
   13eb0:	e3530000 	cmp	r3, #0	; 0x0
   13eb4:	15c92000 	strbne	r2, [r9]
   13eb8:	e59f20f8 	ldr	r2, [pc, #248]	; 13fb8 <control_send+0x250>
   13ebc:	e5923000 	ldr	r3, [r2]
   13ec0:	e2433001 	sub	r3, r3, #1	; 0x1
   13ec4:	e5823000 	str	r3, [r2]
   13ec8:	ea000016 	b	13f28 <control_send+0x1c0>
   13ecc:	e5d42000 	ldrb	r2, [r4]
   13ed0:	e3520000 	cmp	r2, #0	; 0x0
   13ed4:	1affffc3 	bne	13de8 <control_send+0x80>
   13ed8:	e5d91000 	ldrb	r1, [r9]
   13edc:	e3510000 	cmp	r1, #0	; 0x0
   13ee0:	0a000005 	beq	13efc <control_send+0x194>
   13ee4:	e59f30c0 	ldr	r3, [pc, #192]	; 13fac <control_send+0x244>
   13ee8:	e1a0a002 	mov	sl, r2
   13eec:	e5c32000 	strb	r2, [r3]
   13ef0:	e3a03001 	mov	r3, #1	; 0x1
   13ef4:	e5c73000 	strb	r3, [r7]
   13ef8:	eaffffbb 	b	13dec <control_send+0x84>
   13efc:	e5d83000 	ldrb	r3, [r8]
   13f00:	e3530000 	cmp	r3, #0	; 0x0
   13f04:	13a03001 	movne	r3, #1	; 0x1
   13f08:	15c63000 	strbne	r3, [r6]
   13f0c:	11a0a001 	movne	sl, r1
   13f10:	1affffb5 	bne	13dec <control_send+0x84>
   13f14:	ea000003 	b	13f28 <control_send+0x1c0>
   13f18:	e28aa001 	add	sl, sl, #1	; 0x1
   13f1c:	e35a0002 	cmp	sl, #2	; 0x2
   13f20:	1affffb1 	bne	13dec <control_send+0x84>
   13f24:	ea00001d 	b	13fa0 <control_send+0x238>
   13f28:	e59f20a0 	ldr	r2, [pc, #160]	; 13fd0 <control_send+0x268>
   13f2c:	e5d23000 	ldrb	r3, [r2]
   13f30:	e3530000 	cmp	r3, #0	; 0x0
   13f34:	1a000008 	bne	13f5c <control_send+0x1f4>
   13f38:	e59f207c 	ldr	r2, [pc, #124]	; 13fbc <control_send+0x254>
   13f3c:	e5d23000 	ldrb	r3, [r2]
   13f40:	e3530000 	cmp	r3, #0	; 0x0
   13f44:	0a000004 	beq	13f5c <control_send+0x1f4>
   13f48:	e3a00004 	mov	r0, #4	; 0x4
   13f4c:	ebfffa22 	bl	127dc <putc>
   13f50:	e3a03001 	mov	r3, #1	; 0x1
   13f54:	e5c83000 	strb	r3, [r8]
   13f58:	eaffffa2 	b	13de8 <control_send+0x80>
   13f5c:	e59d0004 	ldr	r0, [sp, #4]
   13f60:	e1a0100b 	mov	r1, fp
   13f64:	ebffff24 	bl	13bfc <real_send>
   13f68:	e3500003 	cmp	r0, #3	; 0x3
   13f6c:	0affff9d 	beq	13de8 <control_send+0x80>
   13f70:	e3500000 	cmp	r0, #0	; 0x0
   13f74:	1a00000a 	bne	13fa4 <control_send+0x23c>
   13f78:	e5d93000 	ldrb	r3, [r9]
   13f7c:	e3530000 	cmp	r3, #0	; 0x0
   13f80:	e5d43000 	ldrb	r3, [r4]
   13f84:	15c90000 	strbne	r0, [r9]
   13f88:	e3530000 	cmp	r3, #0	; 0x0
   13f8c:	13a03000 	movne	r3, #0	; 0x0
   13f90:	15c43000 	strbne	r3, [r4]
   13f94:	12833001 	addne	r3, r3, #1	; 0x1
   13f98:	15c93000 	strbne	r3, [r9]
   13f9c:	eaffff91 	b	13de8 <control_send+0x80>
   13fa0:	e3a00000 	mov	r0, #0	; 0x0
   13fa4:	e28dd014 	add	sp, sp, #20	; 0x14
   13fa8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   13fac:	3c00d281 	.word	0x3c00d281
   13fb0:	3c00d282 	.word	0x3c00d282
   13fb4:	3c00d118 	.word	0x3c00d118
   13fb8:	3c00d284 	.word	0x3c00d284
   13fbc:	3c00d288 	.word	0x3c00d288
   13fc0:	3c00d28b 	.word	0x3c00d28b
   13fc4:	3c00d280 	.word	0x3c00d280
   13fc8:	3c00d289 	.word	0x3c00d289
   13fcc:	3c00d28a 	.word	0x3c00d28a
   13fd0:	3c00d28d 	.word	0x3c00d28d
   13fd4:	3c00d28c 	.word	0x3c00d28c
   13fd8:	3c00d728 	.word	0x3c00d728

00013fdc <xyzModem_stream_terminate>:
   13fdc:	e92d4030 	push	{r4, r5, lr}
   13fe0:	e3500000 	cmp	r0, #0	; 0x0
   13fe4:	e24dd00c 	sub	sp, sp, #12	; 0xc
   13fe8:	e1a04001 	mov	r4, r1
   13fec:	0a000020 	beq	14074 <xyzModem_stream_terminate+0x98>
   13ff0:	e59f4098 	ldr	r4, [pc, #152]	; 14090 <xyzModem_stream_terminate+0xb4>
   13ff4:	e5943414 	ldr	r3, [r4, #1044]
   13ff8:	e2433001 	sub	r3, r3, #1	; 0x1
   13ffc:	e3530001 	cmp	r3, #1	; 0x1
   14000:	8a000020 	bhi	14088 <xyzModem_stream_terminate+0xac>
   14004:	e3a00018 	mov	r0, #24	; 0x18
   14008:	ebfff9f3 	bl	127dc <putc>
   1400c:	e3a00018 	mov	r0, #24	; 0x18
   14010:	ebfff9f1 	bl	127dc <putc>
   14014:	e3a00018 	mov	r0, #24	; 0x18
   14018:	ebfff9ef 	bl	127dc <putc>
   1401c:	e3a00018 	mov	r0, #24	; 0x18
   14020:	ebfff9ed 	bl	127dc <putc>
   14024:	e3a00008 	mov	r0, #8	; 0x8
   14028:	ebfff9eb 	bl	127dc <putc>
   1402c:	e3a00008 	mov	r0, #8	; 0x8
   14030:	ebfff9e9 	bl	127dc <putc>
   14034:	e3a00008 	mov	r0, #8	; 0x8
   14038:	ebfff9e7 	bl	127dc <putc>
   1403c:	e3a00008 	mov	r0, #8	; 0x8
   14040:	ebfff9e5 	bl	127dc <putc>
   14044:	e1a05004 	mov	r5, r4
   14048:	e28d4007 	add	r4, sp, #7	; 0x7
   1404c:	e5953000 	ldr	r3, [r5]
   14050:	e1a01004 	mov	r1, r4
   14054:	e5d30000 	ldrb	r0, [r3]
   14058:	ebffff2d 	bl	13d14 <CYGACC_COMM_IF_GETC_TIMEOUT>
   1405c:	e3500000 	cmp	r0, #0	; 0x0
   14060:	1afffff9 	bne	1404c <xyzModem_stream_terminate+0x70>
   14064:	e59f3024 	ldr	r3, [pc, #36]	; 14090 <xyzModem_stream_terminate+0xb4>
   14068:	e3a02001 	mov	r2, #1	; 0x1
   1406c:	e583242c 	str	r2, [r3, #1068]
   14070:	ea000004 	b	14088 <xyzModem_stream_terminate+0xac>
   14074:	e12fff34 	blx	r4
   14078:	e3500000 	cmp	r0, #0	; 0x0
   1407c:	aafffffc 	bge	14074 <xyzModem_stream_terminate+0x98>
   14080:	e59f000c 	ldr	r0, [pc, #12]	; 14094 <xyzModem_stream_terminate+0xb8>
   14084:	ebffe656 	bl	d9e4 <udelay>
   14088:	e28dd00c 	add	sp, sp, #12	; 0xc
   1408c:	e8bd8030 	pop	{r4, r5, pc}
   14090:	3c00d290 	.word	0x3c00d290
   14094:	0003d090 	.word	0x0003d090

00014098 <xyzModem_get_hdr>:
   14098:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   1409c:	e59f42a0 	ldr	r4, [pc, #672]	; 14344 <xyzModem_get_hdr+0x2ac>
   140a0:	e24dd00c 	sub	sp, sp, #12	; 0xc
   140a4:	e5943430 	ldr	r3, [r4, #1072]
   140a8:	e3530000 	cmp	r3, #0	; 0x0
   140ac:	0a000003 	beq	140c0 <xyzModem_get_hdr+0x28>
   140b0:	e3a00006 	mov	r0, #6	; 0x6
   140b4:	ebfff9c8 	bl	127dc <putc>
   140b8:	e3a03000 	mov	r3, #0	; 0x0
   140bc:	e5843430 	str	r3, [r4, #1072]
   140c0:	e3a04000 	mov	r4, #0	; 0x0
   140c4:	e59f6278 	ldr	r6, [pc, #632]	; 14344 <xyzModem_get_hdr+0x2ac>
   140c8:	e1a05004 	mov	r5, r4
   140cc:	e28d7007 	add	r7, sp, #7	; 0x7
   140d0:	e5963000 	ldr	r3, [r6]
   140d4:	e1a01007 	mov	r1, r7
   140d8:	e5d30000 	ldrb	r0, [r3]
   140dc:	ebffff0c 	bl	13d14 <CYGACC_COMM_IF_GETC_TIMEOUT>
   140e0:	e3500000 	cmp	r0, #0	; 0x0
   140e4:	e2855001 	add	r5, r5, #1	; 0x1
   140e8:	e59f2254 	ldr	r2, [pc, #596]	; 14344 <xyzModem_get_hdr+0x2ac>
   140ec:	0a000021 	beq	14178 <xyzModem_get_hdr+0xe0>
   140f0:	e5dd3007 	ldrb	r3, [sp, #7]
   140f4:	e3530002 	cmp	r3, #2	; 0x2
   140f8:	0a00000c 	beq	14130 <xyzModem_get_hdr+0x98>
   140fc:	8a000002 	bhi	1410c <xyzModem_get_hdr+0x74>
   14100:	e3530001 	cmp	r3, #1	; 0x1
   14104:	1afffff1 	bne	140d0 <xyzModem_get_hdr+0x38>
   14108:	ea000004 	b	14120 <xyzModem_get_hdr+0x88>
   1410c:	e3530004 	cmp	r3, #4	; 0x4
   14110:	0a000012 	beq	14160 <xyzModem_get_hdr+0xc8>
   14114:	e3530018 	cmp	r3, #24	; 0x18
   14118:	1affffec 	bne	140d0 <xyzModem_get_hdr+0x38>
   1411c:	ea000007 	b	14140 <xyzModem_get_hdr+0xa8>
   14120:	e592341c 	ldr	r3, [r2, #1052]
   14124:	e2833001 	add	r3, r3, #1	; 0x1
   14128:	e582341c 	str	r3, [r2, #1052]
   1412c:	ea00007c 	b	14324 <xyzModem_get_hdr+0x28c>
   14130:	e5923420 	ldr	r3, [r2, #1056]
   14134:	e2833001 	add	r3, r3, #1	; 0x1
   14138:	e5823420 	str	r3, [r2, #1056]
   1413c:	ea000078 	b	14324 <xyzModem_get_hdr+0x28c>
   14140:	e5963424 	ldr	r3, [r6, #1060]
   14144:	e2844001 	add	r4, r4, #1	; 0x1
   14148:	e2833001 	add	r3, r3, #1	; 0x1
   1414c:	e3540003 	cmp	r4, #3	; 0x3
   14150:	e5863424 	str	r3, [r6, #1060]
   14154:	1affffdd 	bne	140d0 <xyzModem_get_hdr+0x38>
   14158:	e3e00004 	mvn	r0, #4	; 0x4
   1415c:	ea00006e 	b	1431c <xyzModem_get_hdr+0x284>
   14160:	e3550001 	cmp	r5, #1	; 0x1
   14164:	1affffd9 	bne	140d0 <xyzModem_get_hdr+0x38>
   14168:	e3a00006 	mov	r0, #6	; 0x6
   1416c:	ebfff99a 	bl	127dc <putc>
   14170:	e3e00003 	mvn	r0, #3	; 0x3
   14174:	ea000068 	b	1431c <xyzModem_get_hdr+0x284>
   14178:	e1a05002 	mov	r5, r2
   1417c:	e28d4006 	add	r4, sp, #6	; 0x6
   14180:	e5953000 	ldr	r3, [r5]
   14184:	e1a01004 	mov	r1, r4
   14188:	e5d30000 	ldrb	r0, [r3]
   1418c:	ebfffee0 	bl	13d14 <CYGACC_COMM_IF_GETC_TIMEOUT>
   14190:	e3500000 	cmp	r0, #0	; 0x0
   14194:	1afffff9 	bne	14180 <xyzModem_get_hdr+0xe8>
   14198:	e59f01a8 	ldr	r0, [pc, #424]	; 14348 <xyzModem_get_hdr+0x2b0>
   1419c:	ebffe610 	bl	d9e4 <udelay>
   141a0:	ea00005a 	b	14310 <xyzModem_get_hdr+0x278>
   141a4:	e5943000 	ldr	r3, [r4]
   141a8:	e59f119c 	ldr	r1, [pc, #412]	; 1434c <xyzModem_get_hdr+0x2b4>
   141ac:	e5d30000 	ldrb	r0, [r3]
   141b0:	ebfffed7 	bl	13d14 <CYGACC_COMM_IF_GETC_TIMEOUT>
   141b4:	e3500000 	cmp	r0, #0	; 0x0
   141b8:	0a000054 	beq	14310 <xyzModem_get_hdr+0x278>
   141bc:	e28d5008 	add	r5, sp, #8	; 0x8
   141c0:	e5753001 	ldrb	r3, [r5, #-1]!
   141c4:	e2842004 	add	r2, r4, #4	; 0x4
   141c8:	e3530001 	cmp	r3, #1	; 0x1
   141cc:	03a03080 	moveq	r3, #128	; 0x80
   141d0:	13a03b01 	movne	r3, #1024	; 0x400
   141d4:	e5843410 	str	r3, [r4, #1040]
   141d8:	e5842404 	str	r2, [r4, #1028]
   141dc:	e1a07004 	mov	r7, r4
   141e0:	e3a04000 	mov	r4, #0	; 0x0
   141e4:	ea000008 	b	1420c <xyzModem_get_hdr+0x174>
   141e8:	e5973000 	ldr	r3, [r7]
   141ec:	e5d30000 	ldrb	r0, [r3]
   141f0:	ebfffec7 	bl	13d14 <CYGACC_COMM_IF_GETC_TIMEOUT>
   141f4:	e3500000 	cmp	r0, #0	; 0x0
   141f8:	e0872004 	add	r2, r7, r4
   141fc:	e2844001 	add	r4, r4, #1	; 0x1
   14200:	0a000042 	beq	14310 <xyzModem_get_hdr+0x278>
   14204:	e5dd3007 	ldrb	r3, [sp, #7]
   14208:	e5c23004 	strb	r3, [r2, #4]
   1420c:	e5973410 	ldr	r3, [r7, #1040]
   14210:	e1a01005 	mov	r1, r5
   14214:	e1540003 	cmp	r4, r3
   14218:	e59f6124 	ldr	r6, [pc, #292]	; 14344 <xyzModem_get_hdr+0x2ac>
   1421c:	bafffff1 	blt	141e8 <xyzModem_get_hdr+0x150>
   14220:	e5963000 	ldr	r3, [r6]
   14224:	e59f1124 	ldr	r1, [pc, #292]	; 14350 <xyzModem_get_hdr+0x2b8>
   14228:	e5d30000 	ldrb	r0, [r3]
   1422c:	ebfffeb8 	bl	13d14 <CYGACC_COMM_IF_GETC_TIMEOUT>
   14230:	e3500000 	cmp	r0, #0	; 0x0
   14234:	0a000035 	beq	14310 <xyzModem_get_hdr+0x278>
   14238:	e5963428 	ldr	r3, [r6, #1064]
   1423c:	e3530000 	cmp	r3, #0	; 0x0
   14240:	0a000005 	beq	1425c <xyzModem_get_hdr+0x1c4>
   14244:	e5963000 	ldr	r3, [r6]
   14248:	e59f1104 	ldr	r1, [pc, #260]	; 14354 <xyzModem_get_hdr+0x2bc>
   1424c:	e5d30000 	ldrb	r0, [r3]
   14250:	ebfffeaf 	bl	13d14 <CYGACC_COMM_IF_GETC_TIMEOUT>
   14254:	e3500000 	cmp	r0, #0	; 0x0
   14258:	0a00002c 	beq	14310 <xyzModem_get_hdr+0x278>
   1425c:	e59f40e0 	ldr	r4, [pc, #224]	; 14344 <xyzModem_get_hdr+0x2ac>
   14260:	e5d43409 	ldrb	r3, [r4, #1033]
   14264:	e5d42408 	ldrb	r2, [r4, #1032]
   14268:	e0233002 	eor	r3, r3, r2
   1426c:	e35300ff 	cmp	r3, #255	; 0xff
   14270:	0a000009 	beq	1429c <xyzModem_get_hdr+0x204>
   14274:	e1a05004 	mov	r5, r4
   14278:	e28d4006 	add	r4, sp, #6	; 0x6
   1427c:	e5953000 	ldr	r3, [r5]
   14280:	e1a01004 	mov	r1, r4
   14284:	e5d30000 	ldrb	r0, [r3]
   14288:	ebfffea1 	bl	13d14 <CYGACC_COMM_IF_GETC_TIMEOUT>
   1428c:	e3500000 	cmp	r0, #0	; 0x0
   14290:	1afffff9 	bne	1427c <xyzModem_get_hdr+0x1e4>
   14294:	e3e00005 	mvn	r0, #5	; 0x5
   14298:	ea00001f 	b	1431c <xyzModem_get_hdr+0x284>
   1429c:	e5943428 	ldr	r3, [r4, #1064]
   142a0:	e3530000 	cmp	r3, #0	; 0x0
   142a4:	01a02003 	moveq	r2, r3
   142a8:	01a0c004 	moveq	ip, r4
   142ac:	05940410 	ldreq	r0, [r4, #1040]
   142b0:	01a01003 	moveq	r1, r3
   142b4:	0a00000a 	beq	142e4 <xyzModem_get_hdr+0x24c>
   142b8:	e2840004 	add	r0, r4, #4	; 0x4
   142bc:	e5941410 	ldr	r1, [r4, #1040]
   142c0:	eb000110 	bl	14708 <cyg_crc16>
   142c4:	e5d4340b 	ldrb	r3, [r4, #1035]
   142c8:	e5d4240a 	ldrb	r2, [r4, #1034]
   142cc:	e1833402 	orr	r3, r3, r2, lsl #8
   142d0:	e1500003 	cmp	r0, r3
   142d4:	ea00000a 	b	14304 <xyzModem_get_hdr+0x26c>
   142d8:	e5d33004 	ldrb	r3, [r3, #4]
   142dc:	e0813003 	add	r3, r1, r3
   142e0:	e6ff1073 	uxth	r1, r3
   142e4:	e1520000 	cmp	r2, r0
   142e8:	e08c3002 	add	r3, ip, r2
   142ec:	e2822001 	add	r2, r2, #1	; 0x1
   142f0:	bafffff8 	blt	142d8 <xyzModem_get_hdr+0x240>
   142f4:	e59f3048 	ldr	r3, [pc, #72]	; 14344 <xyzModem_get_hdr+0x2ac>
   142f8:	e5d3240a 	ldrb	r2, [r3, #1034]
   142fc:	e20130ff 	and	r3, r1, #255	; 0xff
   14300:	e1520003 	cmp	r2, r3
   14304:	13e00006 	mvnne	r0, #6	; 0x6
   14308:	1a000003 	bne	1431c <xyzModem_get_hdr+0x284>
   1430c:	ea000001 	b	14318 <xyzModem_get_hdr+0x280>
   14310:	e3e00002 	mvn	r0, #2	; 0x2
   14314:	ea000000 	b	1431c <xyzModem_get_hdr+0x284>
   14318:	e3a00000 	mov	r0, #0	; 0x0
   1431c:	e28dd00c 	add	sp, sp, #12	; 0xc
   14320:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
   14324:	e59f4018 	ldr	r4, [pc, #24]	; 14344 <xyzModem_get_hdr+0x2ac>
   14328:	e59f1028 	ldr	r1, [pc, #40]	; 14358 <xyzModem_get_hdr+0x2c0>
   1432c:	e5943000 	ldr	r3, [r4]
   14330:	e5d30000 	ldrb	r0, [r3]
   14334:	ebfffe76 	bl	13d14 <CYGACC_COMM_IF_GETC_TIMEOUT>
   14338:	e3500000 	cmp	r0, #0	; 0x0
   1433c:	0afffff3 	beq	14310 <xyzModem_get_hdr+0x278>
   14340:	eaffff97 	b	141a4 <xyzModem_get_hdr+0x10c>
   14344:	3c00d290 	.word	0x3c00d290
   14348:	0003d090 	.word	0x0003d090
   1434c:	3c00d699 	.word	0x3c00d699
   14350:	3c00d69a 	.word	0x3c00d69a
   14354:	3c00d69b 	.word	0x3c00d69b
   14358:	3c00d698 	.word	0x3c00d698

0001435c <xyzModem_stream_read>:
   1435c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   14360:	e59f5240 	ldr	r5, [pc, #576]	; 145a8 <xyzModem_stream_read+0x24c>
   14364:	e24dd004 	sub	sp, sp, #4	; 0x4
   14368:	e1a08000 	mov	r8, r0
   1436c:	e1a07001 	mov	r7, r1
   14370:	e1a0b002 	mov	fp, r2
   14374:	e3e06004 	mvn	r6, #4	; 0x4
   14378:	e3a0a000 	mov	sl, #0	; 0x0
   1437c:	e1a09005 	mov	r9, r5
   14380:	ea00007e 	b	14580 <xyzModem_stream_read+0x224>
   14384:	e5953410 	ldr	r3, [r5, #1040]
   14388:	e3530000 	cmp	r3, #0	; 0x0
   1438c:	03a04014 	moveq	r4, #20	; 0x14
   14390:	0a00005d 	beq	1450c <xyzModem_stream_read+0x1b0>
   14394:	ea000065 	b	14530 <xyzModem_stream_read+0x1d4>
   14398:	ebffff3e 	bl	14098 <xyzModem_get_hdr>
   1439c:	e2506000 	subs	r6, r0, #0	; 0x0
   143a0:	1a000039 	bne	1448c <xyzModem_stream_read+0x130>
   143a4:	e5d5240c 	ldrb	r2, [r5, #1036]
   143a8:	e5d51408 	ldrb	r1, [r5, #1032]
   143ac:	e2423001 	sub	r3, r2, #1	; 0x1
   143b0:	e1510002 	cmp	r1, r2
   143b4:	e20330ff 	and	r3, r3, #255	; 0xff
   143b8:	1a00002d 	bne	14474 <xyzModem_stream_read+0x118>
   143bc:	e599c414 	ldr	ip, [r9, #1044]
   143c0:	e2813001 	add	r3, r1, #1	; 0x1
   143c4:	e5c9340c 	strb	r3, [r9, #1036]
   143c8:	e35c0001 	cmp	ip, #1	; 0x1
   143cc:	e3a03001 	mov	r3, #1	; 0x1
   143d0:	e5893430 	str	r3, [r9, #1072]
   143d4:	0a000002 	beq	143e4 <xyzModem_stream_read+0x88>
   143d8:	e5993434 	ldr	r3, [r9, #1076]
   143dc:	e3530000 	cmp	r3, #0	; 0x0
   143e0:	1a000017 	bne	14444 <xyzModem_stream_read+0xe8>
   143e4:	e5953410 	ldr	r3, [r5, #1040]
   143e8:	e5950404 	ldr	r0, [r5, #1028]
   143ec:	e0802003 	add	r2, r0, r3
   143f0:	e5523001 	ldrb	r3, [r2, #-1]
   143f4:	e353001a 	cmp	r3, #26	; 0x1a
   143f8:	1a00000f 	bne	1443c <xyzModem_stream_read+0xe0>
   143fc:	e5523002 	ldrb	r3, [r2, #-2]
   14400:	e353001a 	cmp	r3, #26	; 0x1a
   14404:	1a00000c 	bne	1443c <xyzModem_stream_read+0xe0>
   14408:	e5523003 	ldrb	r3, [r2, #-3]
   1440c:	e353001a 	cmp	r3, #26	; 0x1a
   14410:	1a000009 	bne	1443c <xyzModem_stream_read+0xe0>
   14414:	ea000000 	b	1441c <xyzModem_stream_read+0xc0>
   14418:	e5851410 	str	r1, [r5, #1040]
   1441c:	e5953410 	ldr	r3, [r5, #1040]
   14420:	e3530000 	cmp	r3, #0	; 0x0
   14424:	e0802003 	add	r2, r0, r3
   14428:	e2431001 	sub	r1, r3, #1	; 0x1
   1442c:	0a000002 	beq	1443c <xyzModem_stream_read+0xe0>
   14430:	e5523001 	ldrb	r3, [r2, #-1]
   14434:	e353001a 	cmp	r3, #26	; 0x1a
   14438:	0afffff6 	beq	14418 <xyzModem_stream_read+0xbc>
   1443c:	e35c0001 	cmp	ip, #1	; 0x1
   14440:	0a00003a 	beq	14530 <xyzModem_stream_read+0x1d4>
   14444:	e5951434 	ldr	r1, [r5, #1076]
   14448:	e3510000 	cmp	r1, #0	; 0x0
   1444c:	0a000037 	beq	14530 <xyzModem_stream_read+0x1d4>
   14450:	e5953438 	ldr	r3, [r5, #1080]
   14454:	e5950410 	ldr	r0, [r5, #1040]
   14458:	e0802003 	add	r2, r0, r3
   1445c:	e1520001 	cmp	r2, r1
   14460:	80803001 	addhi	r3, r0, r1
   14464:	80623003 	rsbhi	r3, r2, r3
   14468:	e5852438 	str	r2, [r5, #1080]
   1446c:	85853410 	strhi	r3, [r5, #1040]
   14470:	ea00002e 	b	14530 <xyzModem_stream_read+0x1d4>
   14474:	e1510003 	cmp	r1, r3
   14478:	13e06007 	mvnne	r6, #7	; 0x7
   1447c:	1a00001a 	bne	144ec <xyzModem_stream_read+0x190>
   14480:	e3a00006 	mov	r0, #6	; 0x6
   14484:	ebfff8d4 	bl	127dc <putc>
   14488:	ea00001f 	b	1450c <xyzModem_stream_read+0x1b0>
   1448c:	e3760005 	cmn	r6, #5	; 0x5
   14490:	0a000021 	beq	1451c <xyzModem_stream_read+0x1c0>
   14494:	e3760004 	cmn	r6, #4	; 0x4
   14498:	1a000013 	bne	144ec <xyzModem_stream_read+0x190>
   1449c:	e3a00006 	mov	r0, #6	; 0x6
   144a0:	ebfff8cd 	bl	127dc <putc>
   144a4:	e5953414 	ldr	r3, [r5, #1044]
   144a8:	e3530002 	cmp	r3, #2	; 0x2
   144ac:	1a00000b 	bne	144e0 <xyzModem_stream_read+0x184>
   144b0:	e5950428 	ldr	r0, [r5, #1064]
   144b4:	e3500000 	cmp	r0, #0	; 0x0
   144b8:	13a00043 	movne	r0, #67	; 0x43
   144bc:	03a00015 	moveq	r0, #21	; 0x15
   144c0:	ebfff8c5 	bl	127dc <putc>
   144c4:	e5953418 	ldr	r3, [r5, #1048]
   144c8:	e2833001 	add	r3, r3, #1	; 0x1
   144cc:	e5853418 	str	r3, [r5, #1048]
   144d0:	ebfffef0 	bl	14098 <xyzModem_get_hdr>
   144d4:	e1a06000 	mov	r6, r0
   144d8:	e3a00006 	mov	r0, #6	; 0x6
   144dc:	ebfff8be 	bl	127dc <putc>
   144e0:	e3a03001 	mov	r3, #1	; 0x1
   144e4:	e585342c 	str	r3, [r5, #1068]
   144e8:	ea000009 	b	14514 <xyzModem_stream_read+0x1b8>
   144ec:	e5950428 	ldr	r0, [r5, #1064]
   144f0:	e3500000 	cmp	r0, #0	; 0x0
   144f4:	13a00043 	movne	r0, #67	; 0x43
   144f8:	03a00015 	moveq	r0, #21	; 0x15
   144fc:	ebfff8b6 	bl	127dc <putc>
   14500:	e5953418 	ldr	r3, [r5, #1048]
   14504:	e2833001 	add	r3, r3, #1	; 0x1
   14508:	e5853418 	str	r3, [r5, #1048]
   1450c:	e2544001 	subs	r4, r4, #1	; 0x1
   14510:	2affffa0 	bcs	14398 <xyzModem_stream_read+0x3c>
   14514:	e3560000 	cmp	r6, #0	; 0x0
   14518:	aa000004 	bge	14530 <xyzModem_stream_read+0x1d4>
   1451c:	e59f3084 	ldr	r3, [pc, #132]	; 145a8 <xyzModem_stream_read+0x24c>
   14520:	e3e02000 	mvn	r2, #0	; 0x0
   14524:	e58b6000 	str	r6, [fp]
   14528:	e5832410 	str	r2, [r3, #1040]
   1452c:	ea00001a 	b	1459c <xyzModem_stream_read+0x240>
   14530:	e595342c 	ldr	r3, [r5, #1068]
   14534:	e3530000 	cmp	r3, #0	; 0x0
   14538:	1a000010 	bne	14580 <xyzModem_stream_read+0x224>
   1453c:	e5954410 	ldr	r4, [r5, #1040]
   14540:	e1a00008 	mov	r0, r8
   14544:	e1570004 	cmp	r7, r4
   14548:	b1a04007 	movlt	r4, r7
   1454c:	a1a04004 	movge	r4, r4
   14550:	e1a02004 	mov	r2, r4
   14554:	e5951404 	ldr	r1, [r5, #1028]
   14558:	ebffb3ff 	bl	155c <memcpy>
   1455c:	e5953410 	ldr	r3, [r5, #1040]
   14560:	e5952404 	ldr	r2, [r5, #1028]
   14564:	e0643003 	rsb	r3, r4, r3
   14568:	e0822004 	add	r2, r2, r4
   1456c:	e5853410 	str	r3, [r5, #1040]
   14570:	e5852404 	str	r2, [r5, #1028]
   14574:	e0647007 	rsb	r7, r4, r7
   14578:	e0888004 	add	r8, r8, r4
   1457c:	e08aa004 	add	sl, sl, r4
   14580:	e595342c 	ldr	r3, [r5, #1068]
   14584:	e2733001 	rsbs	r3, r3, #1	; 0x1
   14588:	33a03000 	movcc	r3, #0	; 0x0
   1458c:	e3570000 	cmp	r7, #0	; 0x0
   14590:	d3a03000 	movle	r3, #0	; 0x0
   14594:	e3530000 	cmp	r3, #0	; 0x0
   14598:	1affff79 	bne	14384 <xyzModem_stream_read+0x28>
   1459c:	e1a0000a 	mov	r0, sl
   145a0:	e28dd004 	add	sp, sp, #4	; 0x4
   145a4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
   145a8:	3c00d290 	.word	0x3c00d290

000145ac <xyzModem_stream_open>:
   145ac:	e92d45f0 	push	{r4, r5, r6, r7, r8, sl, lr}
   145b0:	e59f4140 	ldr	r4, [pc, #320]	; 146f8 <xyzModem_stream_open+0x14c>
   145b4:	e24dd00c 	sub	sp, sp, #12	; 0xc
   145b8:	e3a05000 	mov	r5, #0	; 0x0
   145bc:	e28d3008 	add	r3, sp, #8	; 0x8
   145c0:	e3a02001 	mov	r2, #1	; 0x1
   145c4:	e5235004 	str	r5, [r3, #-4]!
   145c8:	e5842428 	str	r2, [r4, #1064]
   145cc:	e5845410 	str	r5, [r4, #1040]
   145d0:	e584542c 	str	r5, [r4, #1068]
   145d4:	e5845430 	str	r5, [r4, #1072]
   145d8:	e5902004 	ldr	r2, [r0, #4]
   145dc:	e5843000 	str	r3, [r4]
   145e0:	e3a00043 	mov	r0, #67	; 0x43
   145e4:	e5842414 	str	r2, [r4, #1044]
   145e8:	e5845418 	str	r5, [r4, #1048]
   145ec:	e584541c 	str	r5, [r4, #1052]
   145f0:	e5845420 	str	r5, [r4, #1056]
   145f4:	e5845424 	str	r5, [r4, #1060]
   145f8:	e5845438 	str	r5, [r4, #1080]
   145fc:	e5845434 	str	r5, [r4, #1076]
   14600:	e1a08001 	mov	r8, r1
   14604:	ebfff874 	bl	127dc <putc>
   14608:	e5943414 	ldr	r3, [r4, #1044]
   1460c:	e3530001 	cmp	r3, #1	; 0x1
   14610:	05c4340c 	strbeq	r3, [r4, #1036]
   14614:	01a00005 	moveq	r0, r5
   14618:	0a000034 	beq	146f0 <xyzModem_stream_open+0x144>
   1461c:	e1a07004 	mov	r7, r4
   14620:	e3a06014 	mov	r6, #20	; 0x14
   14624:	e3a0400a 	mov	r4, #10	; 0xa
   14628:	e1a0a005 	mov	sl, r5
   1462c:	ea00002b 	b	146e0 <xyzModem_stream_open+0x134>
   14630:	ebfffe98 	bl	14098 <xyzModem_get_hdr>
   14634:	e2505000 	subs	r5, r0, #0	; 0x0
   14638:	1a000016 	bne	14698 <xyzModem_stream_open+0xec>
   1463c:	e59f20b4 	ldr	r2, [pc, #180]	; 146f8 <xyzModem_stream_open+0x14c>
   14640:	e5d23408 	ldrb	r3, [r2, #1032]
   14644:	e3530000 	cmp	r3, #0	; 0x0
   14648:	1a00000b 	bne	1467c <xyzModem_stream_open+0xd0>
   1464c:	e1a03002 	mov	r3, r2
   14650:	e5930404 	ldr	r0, [r3, #1028]
   14654:	e4d02001 	ldrb	r2, [r0], #1
   14658:	e3520000 	cmp	r2, #0	; 0x0
   1465c:	e5830404 	str	r0, [r3, #1028]
   14660:	1afffffa 	bne	14650 <xyzModem_stream_open+0xa4>
   14664:	e59f3090 	ldr	r3, [pc, #144]	; 146fc <xyzModem_stream_open+0x150>
   14668:	e59f1090 	ldr	r1, [pc, #144]	; 14700 <xyzModem_stream_open+0x154>
   1466c:	ebfffb5a 	bl	133dc <parse_num>
   14670:	e59f3080 	ldr	r3, [pc, #128]	; 146f8 <xyzModem_stream_open+0x14c>
   14674:	e3a02001 	mov	r2, #1	; 0x1
   14678:	e5832430 	str	r2, [r3, #1072]
   1467c:	e59f3074 	ldr	r3, [pc, #116]	; 146f8 <xyzModem_stream_open+0x14c>
   14680:	e3a01000 	mov	r1, #0	; 0x0
   14684:	e3a02001 	mov	r2, #1	; 0x1
   14688:	e5c3240c 	strb	r2, [r3, #1036]
   1468c:	e5831410 	str	r1, [r3, #1040]
   14690:	e1a00001 	mov	r0, r1
   14694:	ea000015 	b	146f0 <xyzModem_stream_open+0x144>
   14698:	e3750003 	cmn	r5, #3	; 0x3
   1469c:	1a00000d 	bne	146d8 <xyzModem_stream_open+0x12c>
   146a0:	e2444001 	sub	r4, r4, #1	; 0x1
   146a4:	e3540000 	cmp	r4, #0	; 0x0
   146a8:	e59f0054 	ldr	r0, [pc, #84]	; 14704 <xyzModem_stream_open+0x158>
   146ac:	d587a428 	strle	sl, [r7, #1064]
   146b0:	ebffe4cb 	bl	d9e4 <udelay>
   146b4:	e5970428 	ldr	r0, [r7, #1064]
   146b8:	e3500000 	cmp	r0, #0	; 0x0
   146bc:	13a00043 	movne	r0, #67	; 0x43
   146c0:	03a00015 	moveq	r0, #21	; 0x15
   146c4:	ebfff844 	bl	127dc <putc>
   146c8:	e5973418 	ldr	r3, [r7, #1048]
   146cc:	e2833001 	add	r3, r3, #1	; 0x1
   146d0:	e5873418 	str	r3, [r7, #1048]
   146d4:	ea000001 	b	146e0 <xyzModem_stream_open+0x134>
   146d8:	e3750005 	cmn	r5, #5	; 0x5
   146dc:	0a000001 	beq	146e8 <xyzModem_stream_open+0x13c>
   146e0:	e2566001 	subs	r6, r6, #1	; 0x1
   146e4:	2affffd1 	bcs	14630 <xyzModem_stream_open+0x84>
   146e8:	e5885000 	str	r5, [r8]
   146ec:	e3e00000 	mvn	r0, #0	; 0x0
   146f0:	e28dd00c 	add	sp, sp, #12	; 0xc
   146f4:	e8bd85f0 	pop	{r4, r5, r6, r7, r8, sl, pc}
   146f8:	3c00d290 	.word	0x3c00d290
   146fc:	00018c0d 	.word	0x00018c0d
   14700:	3c00d6c4 	.word	0x3c00d6c4
   14704:	0007a120 	.word	0x0007a120

00014708 <cyg_crc16>:
   14708:	e3a02000 	mov	r2, #0	; 0x0
   1470c:	e92d4010 	push	{r4, lr}
   14710:	e1a0c000 	mov	ip, r0
   14714:	e59f402c 	ldr	r4, [pc, #44]	; 14748 <cyg_crc16+0x40>
   14718:	e1a00002 	mov	r0, r2
   1471c:	ea000006 	b	1473c <cyg_crc16+0x34>
   14720:	e7dc3002 	ldrb	r3, [ip, r2]
   14724:	e2822001 	add	r2, r2, #1	; 0x1
   14728:	e0233420 	eor	r3, r3, r0, lsr #8
   1472c:	e1a03083 	lsl	r3, r3, #1
   14730:	e19430b3 	ldrh	r3, [r4, r3]
   14734:	e0233400 	eor	r3, r3, r0, lsl #8
   14738:	e6ff0073 	uxth	r0, r3
   1473c:	e1520001 	cmp	r2, r1
   14740:	bafffff6 	blt	14720 <cyg_crc16+0x18>
   14744:	e8bd8010 	pop	{r4, pc}
   14748:	00015d54 	.word	0x00015d54

0001474c <ss_jtag_en>:
   1474c:	e59f300c 	ldr	r3, [pc, #12]	; 14760 <ss_jtag_en+0x14>
   14750:	e2500000 	subs	r0, r0, #0	; 0x0
   14754:	13a00001 	movne	r0, #1	; 0x1
   14758:	e58308b8 	str	r0, [r3, #2232]
   1475c:	e12fff1e 	bx	lr
   14760:	21e08000 	.word	0x21e08000

00014764 <raise>:
   14764:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   14768:	e1a01000 	mov	r1, r0
   1476c:	e24dd004 	sub	sp, sp, #4	; 0x4
   14770:	e59f0014 	ldr	r0, [pc, #20]	; 1478c <raise+0x28>
   14774:	e59f3014 	ldr	r3, [pc, #20]	; 14790 <raise+0x2c>
   14778:	e1a0e00f 	mov	lr, pc
   1477c:	e593f000 	ldr	pc, [r3]
   14780:	e3a00000 	mov	r0, #0	; 0x0
   14784:	e28dd004 	add	sp, sp, #4	; 0x4
   14788:	e8bd8000 	pop	{pc}
   1478c:	00018ea9 	.word	0x00018ea9
   14790:	3c00d6cc 	.word	0x3c00d6cc

00014794 <__aeabi_uidiv>:
   14794:	e2512001 	subs	r2, r1, #1	; 0x1
   14798:	012fff1e 	bxeq	lr
   1479c:	3a000074 	bcc	14974 <__aeabi_uidiv+0x1e0>
   147a0:	e1500001 	cmp	r0, r1
   147a4:	9a00006b 	bls	14958 <__aeabi_uidiv+0x1c4>
   147a8:	e1110002 	tst	r1, r2
   147ac:	0a00006c 	beq	14964 <__aeabi_uidiv+0x1d0>
   147b0:	e16f3f10 	clz	r3, r0
   147b4:	e16f2f11 	clz	r2, r1
   147b8:	e0423003 	sub	r3, r2, r3
   147bc:	e273301f 	rsbs	r3, r3, #31	; 0x1f
   147c0:	10833083 	addne	r3, r3, r3, lsl #1
   147c4:	e3a02000 	mov	r2, #0	; 0x0
   147c8:	108ff103 	addne	pc, pc, r3, lsl #2
   147cc:	e1a00000 	nop			(mov r0,r0)
   147d0:	e1500f81 	cmp	r0, r1, lsl #31
   147d4:	e0a22002 	adc	r2, r2, r2
   147d8:	20400f81 	subcs	r0, r0, r1, lsl #31
   147dc:	e1500f01 	cmp	r0, r1, lsl #30
   147e0:	e0a22002 	adc	r2, r2, r2
   147e4:	20400f01 	subcs	r0, r0, r1, lsl #30
   147e8:	e1500e81 	cmp	r0, r1, lsl #29
   147ec:	e0a22002 	adc	r2, r2, r2
   147f0:	20400e81 	subcs	r0, r0, r1, lsl #29
   147f4:	e1500e01 	cmp	r0, r1, lsl #28
   147f8:	e0a22002 	adc	r2, r2, r2
   147fc:	20400e01 	subcs	r0, r0, r1, lsl #28
   14800:	e1500d81 	cmp	r0, r1, lsl #27
   14804:	e0a22002 	adc	r2, r2, r2
   14808:	20400d81 	subcs	r0, r0, r1, lsl #27
   1480c:	e1500d01 	cmp	r0, r1, lsl #26
   14810:	e0a22002 	adc	r2, r2, r2
   14814:	20400d01 	subcs	r0, r0, r1, lsl #26
   14818:	e1500c81 	cmp	r0, r1, lsl #25
   1481c:	e0a22002 	adc	r2, r2, r2
   14820:	20400c81 	subcs	r0, r0, r1, lsl #25
   14824:	e1500c01 	cmp	r0, r1, lsl #24
   14828:	e0a22002 	adc	r2, r2, r2
   1482c:	20400c01 	subcs	r0, r0, r1, lsl #24
   14830:	e1500b81 	cmp	r0, r1, lsl #23
   14834:	e0a22002 	adc	r2, r2, r2
   14838:	20400b81 	subcs	r0, r0, r1, lsl #23
   1483c:	e1500b01 	cmp	r0, r1, lsl #22
   14840:	e0a22002 	adc	r2, r2, r2
   14844:	20400b01 	subcs	r0, r0, r1, lsl #22
   14848:	e1500a81 	cmp	r0, r1, lsl #21
   1484c:	e0a22002 	adc	r2, r2, r2
   14850:	20400a81 	subcs	r0, r0, r1, lsl #21
   14854:	e1500a01 	cmp	r0, r1, lsl #20
   14858:	e0a22002 	adc	r2, r2, r2
   1485c:	20400a01 	subcs	r0, r0, r1, lsl #20
   14860:	e1500981 	cmp	r0, r1, lsl #19
   14864:	e0a22002 	adc	r2, r2, r2
   14868:	20400981 	subcs	r0, r0, r1, lsl #19
   1486c:	e1500901 	cmp	r0, r1, lsl #18
   14870:	e0a22002 	adc	r2, r2, r2
   14874:	20400901 	subcs	r0, r0, r1, lsl #18
   14878:	e1500881 	cmp	r0, r1, lsl #17
   1487c:	e0a22002 	adc	r2, r2, r2
   14880:	20400881 	subcs	r0, r0, r1, lsl #17
   14884:	e1500801 	cmp	r0, r1, lsl #16
   14888:	e0a22002 	adc	r2, r2, r2
   1488c:	20400801 	subcs	r0, r0, r1, lsl #16
   14890:	e1500781 	cmp	r0, r1, lsl #15
   14894:	e0a22002 	adc	r2, r2, r2
   14898:	20400781 	subcs	r0, r0, r1, lsl #15
   1489c:	e1500701 	cmp	r0, r1, lsl #14
   148a0:	e0a22002 	adc	r2, r2, r2
   148a4:	20400701 	subcs	r0, r0, r1, lsl #14
   148a8:	e1500681 	cmp	r0, r1, lsl #13
   148ac:	e0a22002 	adc	r2, r2, r2
   148b0:	20400681 	subcs	r0, r0, r1, lsl #13
   148b4:	e1500601 	cmp	r0, r1, lsl #12
   148b8:	e0a22002 	adc	r2, r2, r2
   148bc:	20400601 	subcs	r0, r0, r1, lsl #12
   148c0:	e1500581 	cmp	r0, r1, lsl #11
   148c4:	e0a22002 	adc	r2, r2, r2
   148c8:	20400581 	subcs	r0, r0, r1, lsl #11
   148cc:	e1500501 	cmp	r0, r1, lsl #10
   148d0:	e0a22002 	adc	r2, r2, r2
   148d4:	20400501 	subcs	r0, r0, r1, lsl #10
   148d8:	e1500481 	cmp	r0, r1, lsl #9
   148dc:	e0a22002 	adc	r2, r2, r2
   148e0:	20400481 	subcs	r0, r0, r1, lsl #9
   148e4:	e1500401 	cmp	r0, r1, lsl #8
   148e8:	e0a22002 	adc	r2, r2, r2
   148ec:	20400401 	subcs	r0, r0, r1, lsl #8
   148f0:	e1500381 	cmp	r0, r1, lsl #7
   148f4:	e0a22002 	adc	r2, r2, r2
   148f8:	20400381 	subcs	r0, r0, r1, lsl #7
   148fc:	e1500301 	cmp	r0, r1, lsl #6
   14900:	e0a22002 	adc	r2, r2, r2
   14904:	20400301 	subcs	r0, r0, r1, lsl #6
   14908:	e1500281 	cmp	r0, r1, lsl #5
   1490c:	e0a22002 	adc	r2, r2, r2
   14910:	20400281 	subcs	r0, r0, r1, lsl #5
   14914:	e1500201 	cmp	r0, r1, lsl #4
   14918:	e0a22002 	adc	r2, r2, r2
   1491c:	20400201 	subcs	r0, r0, r1, lsl #4
   14920:	e1500181 	cmp	r0, r1, lsl #3
   14924:	e0a22002 	adc	r2, r2, r2
   14928:	20400181 	subcs	r0, r0, r1, lsl #3
   1492c:	e1500101 	cmp	r0, r1, lsl #2
   14930:	e0a22002 	adc	r2, r2, r2
   14934:	20400101 	subcs	r0, r0, r1, lsl #2
   14938:	e1500081 	cmp	r0, r1, lsl #1
   1493c:	e0a22002 	adc	r2, r2, r2
   14940:	20400081 	subcs	r0, r0, r1, lsl #1
   14944:	e1500001 	cmp	r0, r1
   14948:	e0a22002 	adc	r2, r2, r2
   1494c:	20400001 	subcs	r0, r0, r1
   14950:	e1a00002 	mov	r0, r2
   14954:	e12fff1e 	bx	lr
   14958:	03a00001 	moveq	r0, #1	; 0x1
   1495c:	13a00000 	movne	r0, #0	; 0x0
   14960:	e12fff1e 	bx	lr
   14964:	e16f2f11 	clz	r2, r1
   14968:	e262201f 	rsb	r2, r2, #31	; 0x1f
   1496c:	e1a00230 	lsr	r0, r0, r2
   14970:	e12fff1e 	bx	lr
   14974:	e52de008 	str	lr, [sp, #-8]!
   14978:	eb0002cb 	bl	154ac <__div0>
   1497c:	e3a00000 	mov	r0, #0	; 0x0
   14980:	e49df008 	ldr	pc, [sp], #8

00014984 <__aeabi_uidivmod>:
   14984:	e92d4003 	push	{r0, r1, lr}
   14988:	ebffff81 	bl	14794 <__aeabi_uidiv>
   1498c:	e8bd4006 	pop	{r1, r2, lr}
   14990:	e0030092 	mul	r3, r2, r0
   14994:	e0411003 	sub	r1, r1, r3
   14998:	e12fff1e 	bx	lr

0001499c <__aeabi_idiv>:
   1499c:	e3510000 	cmp	r1, #0	; 0x0
   149a0:	e020c001 	eor	ip, r0, r1
   149a4:	0a000080 	beq	14bac <__aeabi_idiv+0x210>
   149a8:	42611000 	rsbmi	r1, r1, #0	; 0x0
   149ac:	e2512001 	subs	r2, r1, #1	; 0x1
   149b0:	0a000070 	beq	14b78 <__aeabi_idiv+0x1dc>
   149b4:	e1b03000 	movs	r3, r0
   149b8:	42603000 	rsbmi	r3, r0, #0	; 0x0
   149bc:	e1530001 	cmp	r3, r1
   149c0:	9a00006f 	bls	14b84 <__aeabi_idiv+0x1e8>
   149c4:	e1110002 	tst	r1, r2
   149c8:	0a000071 	beq	14b94 <__aeabi_idiv+0x1f8>
   149cc:	e16f2f13 	clz	r2, r3
   149d0:	e16f0f11 	clz	r0, r1
   149d4:	e0402002 	sub	r2, r0, r2
   149d8:	e272201f 	rsbs	r2, r2, #31	; 0x1f
   149dc:	10822082 	addne	r2, r2, r2, lsl #1
   149e0:	e3a00000 	mov	r0, #0	; 0x0
   149e4:	108ff102 	addne	pc, pc, r2, lsl #2
   149e8:	e1a00000 	nop			(mov r0,r0)
   149ec:	e1530f81 	cmp	r3, r1, lsl #31
   149f0:	e0a00000 	adc	r0, r0, r0
   149f4:	20433f81 	subcs	r3, r3, r1, lsl #31
   149f8:	e1530f01 	cmp	r3, r1, lsl #30
   149fc:	e0a00000 	adc	r0, r0, r0
   14a00:	20433f01 	subcs	r3, r3, r1, lsl #30
   14a04:	e1530e81 	cmp	r3, r1, lsl #29
   14a08:	e0a00000 	adc	r0, r0, r0
   14a0c:	20433e81 	subcs	r3, r3, r1, lsl #29
   14a10:	e1530e01 	cmp	r3, r1, lsl #28
   14a14:	e0a00000 	adc	r0, r0, r0
   14a18:	20433e01 	subcs	r3, r3, r1, lsl #28
   14a1c:	e1530d81 	cmp	r3, r1, lsl #27
   14a20:	e0a00000 	adc	r0, r0, r0
   14a24:	20433d81 	subcs	r3, r3, r1, lsl #27
   14a28:	e1530d01 	cmp	r3, r1, lsl #26
   14a2c:	e0a00000 	adc	r0, r0, r0
   14a30:	20433d01 	subcs	r3, r3, r1, lsl #26
   14a34:	e1530c81 	cmp	r3, r1, lsl #25
   14a38:	e0a00000 	adc	r0, r0, r0
   14a3c:	20433c81 	subcs	r3, r3, r1, lsl #25
   14a40:	e1530c01 	cmp	r3, r1, lsl #24
   14a44:	e0a00000 	adc	r0, r0, r0
   14a48:	20433c01 	subcs	r3, r3, r1, lsl #24
   14a4c:	e1530b81 	cmp	r3, r1, lsl #23
   14a50:	e0a00000 	adc	r0, r0, r0
   14a54:	20433b81 	subcs	r3, r3, r1, lsl #23
   14a58:	e1530b01 	cmp	r3, r1, lsl #22
   14a5c:	e0a00000 	adc	r0, r0, r0
   14a60:	20433b01 	subcs	r3, r3, r1, lsl #22
   14a64:	e1530a81 	cmp	r3, r1, lsl #21
   14a68:	e0a00000 	adc	r0, r0, r0
   14a6c:	20433a81 	subcs	r3, r3, r1, lsl #21
   14a70:	e1530a01 	cmp	r3, r1, lsl #20
   14a74:	e0a00000 	adc	r0, r0, r0
   14a78:	20433a01 	subcs	r3, r3, r1, lsl #20
   14a7c:	e1530981 	cmp	r3, r1, lsl #19
   14a80:	e0a00000 	adc	r0, r0, r0
   14a84:	20433981 	subcs	r3, r3, r1, lsl #19
   14a88:	e1530901 	cmp	r3, r1, lsl #18
   14a8c:	e0a00000 	adc	r0, r0, r0
   14a90:	20433901 	subcs	r3, r3, r1, lsl #18
   14a94:	e1530881 	cmp	r3, r1, lsl #17
   14a98:	e0a00000 	adc	r0, r0, r0
   14a9c:	20433881 	subcs	r3, r3, r1, lsl #17
   14aa0:	e1530801 	cmp	r3, r1, lsl #16
   14aa4:	e0a00000 	adc	r0, r0, r0
   14aa8:	20433801 	subcs	r3, r3, r1, lsl #16
   14aac:	e1530781 	cmp	r3, r1, lsl #15
   14ab0:	e0a00000 	adc	r0, r0, r0
   14ab4:	20433781 	subcs	r3, r3, r1, lsl #15
   14ab8:	e1530701 	cmp	r3, r1, lsl #14
   14abc:	e0a00000 	adc	r0, r0, r0
   14ac0:	20433701 	subcs	r3, r3, r1, lsl #14
   14ac4:	e1530681 	cmp	r3, r1, lsl #13
   14ac8:	e0a00000 	adc	r0, r0, r0
   14acc:	20433681 	subcs	r3, r3, r1, lsl #13
   14ad0:	e1530601 	cmp	r3, r1, lsl #12
   14ad4:	e0a00000 	adc	r0, r0, r0
   14ad8:	20433601 	subcs	r3, r3, r1, lsl #12
   14adc:	e1530581 	cmp	r3, r1, lsl #11
   14ae0:	e0a00000 	adc	r0, r0, r0
   14ae4:	20433581 	subcs	r3, r3, r1, lsl #11
   14ae8:	e1530501 	cmp	r3, r1, lsl #10
   14aec:	e0a00000 	adc	r0, r0, r0
   14af0:	20433501 	subcs	r3, r3, r1, lsl #10
   14af4:	e1530481 	cmp	r3, r1, lsl #9
   14af8:	e0a00000 	adc	r0, r0, r0
   14afc:	20433481 	subcs	r3, r3, r1, lsl #9
   14b00:	e1530401 	cmp	r3, r1, lsl #8
   14b04:	e0a00000 	adc	r0, r0, r0
   14b08:	20433401 	subcs	r3, r3, r1, lsl #8
   14b0c:	e1530381 	cmp	r3, r1, lsl #7
   14b10:	e0a00000 	adc	r0, r0, r0
   14b14:	20433381 	subcs	r3, r3, r1, lsl #7
   14b18:	e1530301 	cmp	r3, r1, lsl #6
   14b1c:	e0a00000 	adc	r0, r0, r0
   14b20:	20433301 	subcs	r3, r3, r1, lsl #6
   14b24:	e1530281 	cmp	r3, r1, lsl #5
   14b28:	e0a00000 	adc	r0, r0, r0
   14b2c:	20433281 	subcs	r3, r3, r1, lsl #5
   14b30:	e1530201 	cmp	r3, r1, lsl #4
   14b34:	e0a00000 	adc	r0, r0, r0
   14b38:	20433201 	subcs	r3, r3, r1, lsl #4
   14b3c:	e1530181 	cmp	r3, r1, lsl #3
   14b40:	e0a00000 	adc	r0, r0, r0
   14b44:	20433181 	subcs	r3, r3, r1, lsl #3
   14b48:	e1530101 	cmp	r3, r1, lsl #2
   14b4c:	e0a00000 	adc	r0, r0, r0
   14b50:	20433101 	subcs	r3, r3, r1, lsl #2
   14b54:	e1530081 	cmp	r3, r1, lsl #1
   14b58:	e0a00000 	adc	r0, r0, r0
   14b5c:	20433081 	subcs	r3, r3, r1, lsl #1
   14b60:	e1530001 	cmp	r3, r1
   14b64:	e0a00000 	adc	r0, r0, r0
   14b68:	20433001 	subcs	r3, r3, r1
   14b6c:	e35c0000 	cmp	ip, #0	; 0x0
   14b70:	42600000 	rsbmi	r0, r0, #0	; 0x0
   14b74:	e12fff1e 	bx	lr
   14b78:	e13c0000 	teq	ip, r0
   14b7c:	42600000 	rsbmi	r0, r0, #0	; 0x0
   14b80:	e12fff1e 	bx	lr
   14b84:	33a00000 	movcc	r0, #0	; 0x0
   14b88:	01a00fcc 	asreq	r0, ip, #31
   14b8c:	03800001 	orreq	r0, r0, #1	; 0x1
   14b90:	e12fff1e 	bx	lr
   14b94:	e16f2f11 	clz	r2, r1
   14b98:	e262201f 	rsb	r2, r2, #31	; 0x1f
   14b9c:	e35c0000 	cmp	ip, #0	; 0x0
   14ba0:	e1a00233 	lsr	r0, r3, r2
   14ba4:	42600000 	rsbmi	r0, r0, #0	; 0x0
   14ba8:	e12fff1e 	bx	lr
   14bac:	e52de008 	str	lr, [sp, #-8]!
   14bb0:	eb00023d 	bl	154ac <__div0>
   14bb4:	e3a00000 	mov	r0, #0	; 0x0
   14bb8:	e49df008 	ldr	pc, [sp], #8

00014bbc <__aeabi_idivmod>:
   14bbc:	e92d4003 	push	{r0, r1, lr}
   14bc0:	ebffff75 	bl	1499c <__aeabi_idiv>
   14bc4:	e8bd4006 	pop	{r1, r2, lr}
   14bc8:	e0030092 	mul	r3, r2, r0
   14bcc:	e0411003 	sub	r1, r1, r3
   14bd0:	e12fff1e 	bx	lr

00014bd4 <__aeabi_llsr>:
   14bd4:	e2523020 	subs	r3, r2, #32	; 0x20
   14bd8:	e262c020 	rsb	ip, r2, #32	; 0x20
   14bdc:	41a00230 	lsrmi	r0, r0, r2
   14be0:	51a00331 	lsrpl	r0, r1, r3
   14be4:	41800c11 	orrmi	r0, r0, r1, lsl ip
   14be8:	e1a01231 	lsr	r1, r1, r2
   14bec:	e12fff1e 	bx	lr

00014bf0 <__aeabi_lasr>:
   14bf0:	e2523020 	subs	r3, r2, #32	; 0x20
   14bf4:	e262c020 	rsb	ip, r2, #32	; 0x20
   14bf8:	41a00230 	lsrmi	r0, r0, r2
   14bfc:	51a00351 	asrpl	r0, r1, r3
   14c00:	41800c11 	orrmi	r0, r0, r1, lsl ip
   14c04:	e1a01251 	asr	r1, r1, r2
   14c08:	e12fff1e 	bx	lr

00014c0c <__aeabi_llsl>:
   14c0c:	e2523020 	subs	r3, r2, #32	; 0x20
   14c10:	e262c020 	rsb	ip, r2, #32	; 0x20
   14c14:	41a01211 	lslmi	r1, r1, r2
   14c18:	51a01310 	lslpl	r1, r0, r3
   14c1c:	41811c30 	orrmi	r1, r1, r0, lsr ip
   14c20:	e1a00210 	lsl	r0, r0, r2
   14c24:	e12fff1e 	bx	lr

00014c28 <__aeabi_drsub>:
   14c28:	e2211102 	eor	r1, r1, #-2147483648	; 0x80000000
   14c2c:	ea000000 	b	14c34 <__adddf3>

00014c30 <__aeabi_dsub>:
   14c30:	e2233102 	eor	r3, r3, #-2147483648	; 0x80000000

00014c34 <__adddf3>:
   14c34:	e92d4030 	push	{r4, r5, lr}
   14c38:	e1a04081 	lsl	r4, r1, #1
   14c3c:	e1a05083 	lsl	r5, r3, #1
   14c40:	e1340005 	teq	r4, r5
   14c44:	01300002 	teqeq	r0, r2
   14c48:	1194c000 	orrsne	ip, r4, r0
   14c4c:	1195c002 	orrsne	ip, r5, r2
   14c50:	11f0cac4 	mvnsne	ip, r4, asr #21
   14c54:	11f0cac5 	mvnsne	ip, r5, asr #21
   14c58:	0a000079 	beq	14e44 <__adddf3+0x210>
   14c5c:	e1a04aa4 	lsr	r4, r4, #21
   14c60:	e0745aa5 	rsbs	r5, r4, r5, lsr #21
   14c64:	b2655000 	rsblt	r5, r5, #0	; 0x0
   14c68:	da000006 	ble	14c88 <__adddf3+0x54>
   14c6c:	e0844005 	add	r4, r4, r5
   14c70:	e0202002 	eor	r2, r0, r2
   14c74:	e0213003 	eor	r3, r1, r3
   14c78:	e0220000 	eor	r0, r2, r0
   14c7c:	e0231001 	eor	r1, r3, r1
   14c80:	e0202002 	eor	r2, r0, r2
   14c84:	e0213003 	eor	r3, r1, r3
   14c88:	e3550036 	cmp	r5, #54	; 0x36
   14c8c:	88bd8030 	pophi	{r4, r5, pc}
   14c90:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   14c94:	e1a01601 	lsl	r1, r1, #12
   14c98:	e3a0c601 	mov	ip, #1048576	; 0x100000
   14c9c:	e18c1621 	orr	r1, ip, r1, lsr #12
   14ca0:	0a000001 	beq	14cac <__adddf3+0x78>
   14ca4:	e2700000 	rsbs	r0, r0, #0	; 0x0
   14ca8:	e2e11000 	rsc	r1, r1, #0	; 0x0
   14cac:	e3130102 	tst	r3, #-2147483648	; 0x80000000
   14cb0:	e1a03603 	lsl	r3, r3, #12
   14cb4:	e18c3623 	orr	r3, ip, r3, lsr #12
   14cb8:	0a000001 	beq	14cc4 <__adddf3+0x90>
   14cbc:	e2722000 	rsbs	r2, r2, #0	; 0x0
   14cc0:	e2e33000 	rsc	r3, r3, #0	; 0x0
   14cc4:	e1340005 	teq	r4, r5
   14cc8:	0a000057 	beq	14e2c <__adddf3+0x1f8>
   14ccc:	e2444001 	sub	r4, r4, #1	; 0x1
   14cd0:	e275e020 	rsbs	lr, r5, #32	; 0x20
   14cd4:	ba000005 	blt	14cf0 <__adddf3+0xbc>
   14cd8:	e1a0ce12 	lsl	ip, r2, lr
   14cdc:	e0900532 	adds	r0, r0, r2, lsr r5
   14ce0:	e2a11000 	adc	r1, r1, #0	; 0x0
   14ce4:	e0900e13 	adds	r0, r0, r3, lsl lr
   14ce8:	e0b11553 	adcs	r1, r1, r3, asr r5
   14cec:	ea000006 	b	14d0c <__adddf3+0xd8>
   14cf0:	e2455020 	sub	r5, r5, #32	; 0x20
   14cf4:	e28ee020 	add	lr, lr, #32	; 0x20
   14cf8:	e3520001 	cmp	r2, #1	; 0x1
   14cfc:	e1a0ce13 	lsl	ip, r3, lr
   14d00:	238cc002 	orrcs	ip, ip, #2	; 0x2
   14d04:	e0900553 	adds	r0, r0, r3, asr r5
   14d08:	e0b11fc3 	adcs	r1, r1, r3, asr #31
   14d0c:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   14d10:	5a000002 	bpl	14d20 <__adddf3+0xec>
   14d14:	e27cc000 	rsbs	ip, ip, #0	; 0x0
   14d18:	e2f00000 	rscs	r0, r0, #0	; 0x0
   14d1c:	e2e11000 	rsc	r1, r1, #0	; 0x0
   14d20:	e3510601 	cmp	r1, #1048576	; 0x100000
   14d24:	3a00000e 	bcc	14d64 <__adddf3+0x130>
   14d28:	e3510602 	cmp	r1, #2097152	; 0x200000
   14d2c:	3a000006 	bcc	14d4c <__adddf3+0x118>
   14d30:	e1b010a1 	lsrs	r1, r1, #1
   14d34:	e1b00060 	rrxs	r0, r0
   14d38:	e1a0c06c 	rrx	ip, ip
   14d3c:	e2844001 	add	r4, r4, #1	; 0x1
   14d40:	e1a02a84 	lsl	r2, r4, #21
   14d44:	e3720501 	cmn	r2, #4194304	; 0x400000
   14d48:	2a000055 	bcs	14ea4 <__adddf3+0x270>
   14d4c:	e35c0102 	cmp	ip, #-2147483648	; 0x80000000
   14d50:	01b0c0a0 	lsrseq	ip, r0, #1
   14d54:	e2b00000 	adcs	r0, r0, #0	; 0x0
   14d58:	e0a11a04 	adc	r1, r1, r4, lsl #20
   14d5c:	e1811005 	orr	r1, r1, r5
   14d60:	e8bd8030 	pop	{r4, r5, pc}
   14d64:	e1b0c08c 	lsls	ip, ip, #1
   14d68:	e0b00000 	adcs	r0, r0, r0
   14d6c:	e0a11001 	adc	r1, r1, r1
   14d70:	e3110601 	tst	r1, #1048576	; 0x100000
   14d74:	e2444001 	sub	r4, r4, #1	; 0x1
   14d78:	1afffff3 	bne	14d4c <__adddf3+0x118>
   14d7c:	e3310000 	teq	r1, #0	; 0x0
   14d80:	01a01000 	moveq	r1, r0
   14d84:	03a00000 	moveq	r0, #0	; 0x0
   14d88:	e16f3f11 	clz	r3, r1
   14d8c:	02833020 	addeq	r3, r3, #32	; 0x20
   14d90:	e243300b 	sub	r3, r3, #11	; 0xb
   14d94:	e2532020 	subs	r2, r3, #32	; 0x20
   14d98:	aa000007 	bge	14dbc <__adddf3+0x188>
   14d9c:	e292200c 	adds	r2, r2, #12	; 0xc
   14da0:	da000004 	ble	14db8 <__adddf3+0x184>
   14da4:	e282c014 	add	ip, r2, #20	; 0x14
   14da8:	e262200c 	rsb	r2, r2, #12	; 0xc
   14dac:	e1a00c11 	lsl	r0, r1, ip
   14db0:	e1a01231 	lsr	r1, r1, r2
   14db4:	ea000004 	b	14dcc <__adddf3+0x198>
   14db8:	e2822014 	add	r2, r2, #20	; 0x14
   14dbc:	d262c020 	rsble	ip, r2, #32	; 0x20
   14dc0:	e1a01211 	lsl	r1, r1, r2
   14dc4:	d1811c30 	orrle	r1, r1, r0, lsr ip
   14dc8:	d1a00210 	lslle	r0, r0, r2
   14dcc:	e0544003 	subs	r4, r4, r3
   14dd0:	a0811a04 	addge	r1, r1, r4, lsl #20
   14dd4:	a1811005 	orrge	r1, r1, r5
   14dd8:	a8bd8030 	popge	{r4, r5, pc}
   14ddc:	e1e04004 	mvn	r4, r4
   14de0:	e254401f 	subs	r4, r4, #31	; 0x1f
   14de4:	aa00000d 	bge	14e20 <__adddf3+0x1ec>
   14de8:	e294400c 	adds	r4, r4, #12	; 0xc
   14dec:	ca000005 	bgt	14e08 <__adddf3+0x1d4>
   14df0:	e2844014 	add	r4, r4, #20	; 0x14
   14df4:	e2642020 	rsb	r2, r4, #32	; 0x20
   14df8:	e1a00430 	lsr	r0, r0, r4
   14dfc:	e1800211 	orr	r0, r0, r1, lsl r2
   14e00:	e1851431 	orr	r1, r5, r1, lsr r4
   14e04:	e8bd8030 	pop	{r4, r5, pc}
   14e08:	e264400c 	rsb	r4, r4, #12	; 0xc
   14e0c:	e2642020 	rsb	r2, r4, #32	; 0x20
   14e10:	e1a00230 	lsr	r0, r0, r2
   14e14:	e1800411 	orr	r0, r0, r1, lsl r4
   14e18:	e1a01005 	mov	r1, r5
   14e1c:	e8bd8030 	pop	{r4, r5, pc}
   14e20:	e1a00431 	lsr	r0, r1, r4
   14e24:	e1a01005 	mov	r1, r5
   14e28:	e8bd8030 	pop	{r4, r5, pc}
   14e2c:	e3340000 	teq	r4, #0	; 0x0
   14e30:	e2233601 	eor	r3, r3, #1048576	; 0x100000
   14e34:	02211601 	eoreq	r1, r1, #1048576	; 0x100000
   14e38:	02844001 	addeq	r4, r4, #1	; 0x1
   14e3c:	12455001 	subne	r5, r5, #1	; 0x1
   14e40:	eaffffa1 	b	14ccc <__adddf3+0x98>
   14e44:	e1f0cac4 	mvns	ip, r4, asr #21
   14e48:	11f0cac5 	mvnsne	ip, r5, asr #21
   14e4c:	0a000018 	beq	14eb4 <__adddf3+0x280>
   14e50:	e1340005 	teq	r4, r5
   14e54:	01300002 	teqeq	r0, r2
   14e58:	0a000003 	beq	14e6c <__adddf3+0x238>
   14e5c:	e194c000 	orrs	ip, r4, r0
   14e60:	01a01003 	moveq	r1, r3
   14e64:	01a00002 	moveq	r0, r2
   14e68:	e8bd8030 	pop	{r4, r5, pc}
   14e6c:	e1310003 	teq	r1, r3
   14e70:	13a01000 	movne	r1, #0	; 0x0
   14e74:	13a00000 	movne	r0, #0	; 0x0
   14e78:	18bd8030 	popne	{r4, r5, pc}
   14e7c:	e1b0caa4 	lsrs	ip, r4, #21
   14e80:	1a000003 	bne	14e94 <__adddf3+0x260>
   14e84:	e1b00080 	lsls	r0, r0, #1
   14e88:	e0b11001 	adcs	r1, r1, r1
   14e8c:	23811102 	orrcs	r1, r1, #-2147483648	; 0x80000000
   14e90:	e8bd8030 	pop	{r4, r5, pc}
   14e94:	e2944501 	adds	r4, r4, #4194304	; 0x400000
   14e98:	32811601 	addcc	r1, r1, #1048576	; 0x100000
   14e9c:	38bd8030 	popcc	{r4, r5, pc}
   14ea0:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   14ea4:	e385147f 	orr	r1, r5, #2130706432	; 0x7f000000
   14ea8:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   14eac:	e3a00000 	mov	r0, #0	; 0x0
   14eb0:	e8bd8030 	pop	{r4, r5, pc}
   14eb4:	e1f0cac4 	mvns	ip, r4, asr #21
   14eb8:	11a01003 	movne	r1, r3
   14ebc:	11a00002 	movne	r0, r2
   14ec0:	01f0cac5 	mvnseq	ip, r5, asr #21
   14ec4:	11a03001 	movne	r3, r1
   14ec8:	11a02000 	movne	r2, r0
   14ecc:	e1904601 	orrs	r4, r0, r1, lsl #12
   14ed0:	01925603 	orrseq	r5, r2, r3, lsl #12
   14ed4:	01310003 	teqeq	r1, r3
   14ed8:	13811702 	orrne	r1, r1, #524288	; 0x80000
   14edc:	e8bd8030 	pop	{r4, r5, pc}

00014ee0 <__aeabi_ui2d>:
   14ee0:	e3300000 	teq	r0, #0	; 0x0
   14ee4:	03a01000 	moveq	r1, #0	; 0x0
   14ee8:	012fff1e 	bxeq	lr
   14eec:	e92d4030 	push	{r4, r5, lr}
   14ef0:	e3a04b01 	mov	r4, #1024	; 0x400
   14ef4:	e2844032 	add	r4, r4, #50	; 0x32
   14ef8:	e3a05000 	mov	r5, #0	; 0x0
   14efc:	e3a01000 	mov	r1, #0	; 0x0
   14f00:	eaffff9d 	b	14d7c <__adddf3+0x148>

00014f04 <__aeabi_i2d>:
   14f04:	e3300000 	teq	r0, #0	; 0x0
   14f08:	03a01000 	moveq	r1, #0	; 0x0
   14f0c:	012fff1e 	bxeq	lr
   14f10:	e92d4030 	push	{r4, r5, lr}
   14f14:	e3a04b01 	mov	r4, #1024	; 0x400
   14f18:	e2844032 	add	r4, r4, #50	; 0x32
   14f1c:	e2105102 	ands	r5, r0, #-2147483648	; 0x80000000
   14f20:	42600000 	rsbmi	r0, r0, #0	; 0x0
   14f24:	e3a01000 	mov	r1, #0	; 0x0
   14f28:	eaffff93 	b	14d7c <__adddf3+0x148>

00014f2c <__aeabi_f2d>:
   14f2c:	e1b02080 	lsls	r2, r0, #1
   14f30:	e1a011c2 	asr	r1, r2, #3
   14f34:	e1a01061 	rrx	r1, r1
   14f38:	e1a00e02 	lsl	r0, r2, #28
   14f3c:	121234ff 	andsne	r3, r2, #-16777216	; 0xff000000
   14f40:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   14f44:	1221130e 	eorne	r1, r1, #939524096	; 0x38000000
   14f48:	112fff1e 	bxne	lr
   14f4c:	e3320000 	teq	r2, #0	; 0x0
   14f50:	133304ff 	teqne	r3, #-16777216	; 0xff000000
   14f54:	012fff1e 	bxeq	lr
   14f58:	e92d4030 	push	{r4, r5, lr}
   14f5c:	e3a04d0e 	mov	r4, #896	; 0x380
   14f60:	e2015102 	and	r5, r1, #-2147483648	; 0x80000000
   14f64:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   14f68:	eaffff83 	b	14d7c <__adddf3+0x148>

00014f6c <__aeabi_ul2d>:
   14f6c:	e1902001 	orrs	r2, r0, r1
   14f70:	012fff1e 	bxeq	lr
   14f74:	e92d4030 	push	{r4, r5, lr}
   14f78:	e3a05000 	mov	r5, #0	; 0x0
   14f7c:	ea000006 	b	14f9c <__aeabi_l2d+0x1c>

00014f80 <__aeabi_l2d>:
   14f80:	e1902001 	orrs	r2, r0, r1
   14f84:	012fff1e 	bxeq	lr
   14f88:	e92d4030 	push	{r4, r5, lr}
   14f8c:	e2115102 	ands	r5, r1, #-2147483648	; 0x80000000
   14f90:	5a000001 	bpl	14f9c <__aeabi_l2d+0x1c>
   14f94:	e2700000 	rsbs	r0, r0, #0	; 0x0
   14f98:	e2e11000 	rsc	r1, r1, #0	; 0x0
   14f9c:	e3a04b01 	mov	r4, #1024	; 0x400
   14fa0:	e2844032 	add	r4, r4, #50	; 0x32
   14fa4:	e1b0cb21 	lsrs	ip, r1, #22
   14fa8:	0affff5c 	beq	14d20 <__adddf3+0xec>
   14fac:	e3a02003 	mov	r2, #3	; 0x3
   14fb0:	e1b0c1ac 	lsrs	ip, ip, #3
   14fb4:	12822003 	addne	r2, r2, #3	; 0x3
   14fb8:	e1b0c1ac 	lsrs	ip, ip, #3
   14fbc:	12822003 	addne	r2, r2, #3	; 0x3
   14fc0:	e08221ac 	add	r2, r2, ip, lsr #3
   14fc4:	e2623020 	rsb	r3, r2, #32	; 0x20
   14fc8:	e1a0c310 	lsl	ip, r0, r3
   14fcc:	e1a00230 	lsr	r0, r0, r2
   14fd0:	e1800311 	orr	r0, r0, r1, lsl r3
   14fd4:	e1a01231 	lsr	r1, r1, r2
   14fd8:	e0844002 	add	r4, r4, r2
   14fdc:	eaffff4f 	b	14d20 <__adddf3+0xec>

00014fe0 <__aeabi_dmul>:
   14fe0:	e92d4070 	push	{r4, r5, r6, lr}
   14fe4:	e3a0c0ff 	mov	ip, #255	; 0xff
   14fe8:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   14fec:	e01c4a21 	ands	r4, ip, r1, lsr #20
   14ff0:	101c5a23 	andsne	r5, ip, r3, lsr #20
   14ff4:	1134000c 	teqne	r4, ip
   14ff8:	1135000c 	teqne	r5, ip
   14ffc:	0b00006f 	bleq	151c0 <__aeabi_dmul+0x1e0>
   15000:	e0844005 	add	r4, r4, r5
   15004:	e0216003 	eor	r6, r1, r3
   15008:	e1c11a8c 	bic	r1, r1, ip, lsl #21
   1500c:	e1c33a8c 	bic	r3, r3, ip, lsl #21
   15010:	e1905601 	orrs	r5, r0, r1, lsl #12
   15014:	11925603 	orrsne	r5, r2, r3, lsl #12
   15018:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   1501c:	e3833601 	orr	r3, r3, #1048576	; 0x100000
   15020:	0a00001c 	beq	15098 <__aeabi_dmul+0xb8>
   15024:	e08ec290 	umull	ip, lr, r0, r2
   15028:	e3a05000 	mov	r5, #0	; 0x0
   1502c:	e0a5e291 	umlal	lr, r5, r1, r2
   15030:	e2062102 	and	r2, r6, #-2147483648	; 0x80000000
   15034:	e0a5e390 	umlal	lr, r5, r0, r3
   15038:	e3a06000 	mov	r6, #0	; 0x0
   1503c:	e0a65391 	umlal	r5, r6, r1, r3
   15040:	e33c0000 	teq	ip, #0	; 0x0
   15044:	138ee001 	orrne	lr, lr, #1	; 0x1
   15048:	e24440ff 	sub	r4, r4, #255	; 0xff
   1504c:	e3560c02 	cmp	r6, #512	; 0x200
   15050:	e2c44c03 	sbc	r4, r4, #768	; 0x300
   15054:	2a000002 	bcs	15064 <__aeabi_dmul+0x84>
   15058:	e1b0e08e 	lsls	lr, lr, #1
   1505c:	e0b55005 	adcs	r5, r5, r5
   15060:	e0a66006 	adc	r6, r6, r6
   15064:	e1821586 	orr	r1, r2, r6, lsl #11
   15068:	e1811aa5 	orr	r1, r1, r5, lsr #21
   1506c:	e1a00585 	lsl	r0, r5, #11
   15070:	e1800aae 	orr	r0, r0, lr, lsr #21
   15074:	e1a0e58e 	lsl	lr, lr, #11
   15078:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   1507c:	835c0c07 	cmphi	ip, #1792	; 0x700
   15080:	8a00000f 	bhi	150c4 <__aeabi_dmul+0xe4>
   15084:	e35e0102 	cmp	lr, #-2147483648	; 0x80000000
   15088:	01b0e0a0 	lsrseq	lr, r0, #1
   1508c:	e2b00000 	adcs	r0, r0, #0	; 0x0
   15090:	e0a11a04 	adc	r1, r1, r4, lsl #20
   15094:	e8bd8070 	pop	{r4, r5, r6, pc}
   15098:	e2066102 	and	r6, r6, #-2147483648	; 0x80000000
   1509c:	e1861001 	orr	r1, r6, r1
   150a0:	e1800002 	orr	r0, r0, r2
   150a4:	e0211003 	eor	r1, r1, r3
   150a8:	e05440ac 	subs	r4, r4, ip, lsr #1
   150ac:	c074500c 	rsbsgt	r5, r4, ip
   150b0:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   150b4:	c8bd8070 	popgt	{r4, r5, r6, pc}
   150b8:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   150bc:	e3a0e000 	mov	lr, #0	; 0x0
   150c0:	e2544001 	subs	r4, r4, #1	; 0x1
   150c4:	ca000058 	bgt	1522c <__aeabi_dmul+0x24c>
   150c8:	e3740036 	cmn	r4, #54	; 0x36
   150cc:	d3a00000 	movle	r0, #0	; 0x0
   150d0:	d2011102 	andle	r1, r1, #-2147483648	; 0x80000000
   150d4:	d8bd8070 	pople	{r4, r5, r6, pc}
   150d8:	e2644000 	rsb	r4, r4, #0	; 0x0
   150dc:	e2544020 	subs	r4, r4, #32	; 0x20
   150e0:	aa000018 	bge	15148 <__aeabi_dmul+0x168>
   150e4:	e294400c 	adds	r4, r4, #12	; 0xc
   150e8:	ca00000b 	bgt	1511c <__aeabi_dmul+0x13c>
   150ec:	e2844014 	add	r4, r4, #20	; 0x14
   150f0:	e2645020 	rsb	r5, r4, #32	; 0x20
   150f4:	e1a03510 	lsl	r3, r0, r5
   150f8:	e1a00430 	lsr	r0, r0, r4
   150fc:	e1800511 	orr	r0, r0, r1, lsl r5
   15100:	e2012102 	and	r2, r1, #-2147483648	; 0x80000000
   15104:	e3c11102 	bic	r1, r1, #-2147483648	; 0x80000000
   15108:	e0900fa3 	adds	r0, r0, r3, lsr #31
   1510c:	e0a21431 	adc	r1, r2, r1, lsr r4
   15110:	e19ee083 	orrs	lr, lr, r3, lsl #1
   15114:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   15118:	e8bd8070 	pop	{r4, r5, r6, pc}
   1511c:	e264400c 	rsb	r4, r4, #12	; 0xc
   15120:	e2645020 	rsb	r5, r4, #32	; 0x20
   15124:	e1a03410 	lsl	r3, r0, r4
   15128:	e1a00530 	lsr	r0, r0, r5
   1512c:	e1800411 	orr	r0, r0, r1, lsl r4
   15130:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   15134:	e0900fa3 	adds	r0, r0, r3, lsr #31
   15138:	e2a11000 	adc	r1, r1, #0	; 0x0
   1513c:	e19ee083 	orrs	lr, lr, r3, lsl #1
   15140:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   15144:	e8bd8070 	pop	{r4, r5, r6, pc}
   15148:	e2645020 	rsb	r5, r4, #32	; 0x20
   1514c:	e18ee510 	orr	lr, lr, r0, lsl r5
   15150:	e1a03430 	lsr	r3, r0, r4
   15154:	e1833511 	orr	r3, r3, r1, lsl r5
   15158:	e1a00431 	lsr	r0, r1, r4
   1515c:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   15160:	e1c00431 	bic	r0, r0, r1, lsr r4
   15164:	e0800fa3 	add	r0, r0, r3, lsr #31
   15168:	e19ee083 	orrs	lr, lr, r3, lsl #1
   1516c:	01c00fa3 	biceq	r0, r0, r3, lsr #31
   15170:	e8bd8070 	pop	{r4, r5, r6, pc}
   15174:	e3340000 	teq	r4, #0	; 0x0
   15178:	1a000008 	bne	151a0 <__aeabi_dmul+0x1c0>
   1517c:	e2016102 	and	r6, r1, #-2147483648	; 0x80000000
   15180:	e1b00080 	lsls	r0, r0, #1
   15184:	e0a11001 	adc	r1, r1, r1
   15188:	e3110601 	tst	r1, #1048576	; 0x100000
   1518c:	02444001 	subeq	r4, r4, #1	; 0x1
   15190:	0afffffa 	beq	15180 <__aeabi_dmul+0x1a0>
   15194:	e1811006 	orr	r1, r1, r6
   15198:	e3350000 	teq	r5, #0	; 0x0
   1519c:	11a0f00e 	movne	pc, lr
   151a0:	e2036102 	and	r6, r3, #-2147483648	; 0x80000000
   151a4:	e1b02082 	lsls	r2, r2, #1
   151a8:	e0a33003 	adc	r3, r3, r3
   151ac:	e3130601 	tst	r3, #1048576	; 0x100000
   151b0:	02455001 	subeq	r5, r5, #1	; 0x1
   151b4:	0afffffa 	beq	151a4 <__aeabi_dmul+0x1c4>
   151b8:	e1833006 	orr	r3, r3, r6
   151bc:	e1a0f00e 	mov	pc, lr
   151c0:	e134000c 	teq	r4, ip
   151c4:	e00c5a23 	and	r5, ip, r3, lsr #20
   151c8:	1135000c 	teqne	r5, ip
   151cc:	0a000006 	beq	151ec <__aeabi_dmul+0x20c>
   151d0:	e1906081 	orrs	r6, r0, r1, lsl #1
   151d4:	11926083 	orrsne	r6, r2, r3, lsl #1
   151d8:	1affffe5 	bne	15174 <__aeabi_dmul+0x194>
   151dc:	e0211003 	eor	r1, r1, r3
   151e0:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   151e4:	e3a00000 	mov	r0, #0	; 0x0
   151e8:	e8bd8070 	pop	{r4, r5, r6, pc}
   151ec:	e1906081 	orrs	r6, r0, r1, lsl #1
   151f0:	01a00002 	moveq	r0, r2
   151f4:	01a01003 	moveq	r1, r3
   151f8:	11926083 	orrsne	r6, r2, r3, lsl #1
   151fc:	0a00000f 	beq	15240 <__aeabi_dmul+0x260>
   15200:	e134000c 	teq	r4, ip
   15204:	1a000001 	bne	15210 <__aeabi_dmul+0x230>
   15208:	e1906601 	orrs	r6, r0, r1, lsl #12
   1520c:	1a00000b 	bne	15240 <__aeabi_dmul+0x260>
   15210:	e135000c 	teq	r5, ip
   15214:	1a000003 	bne	15228 <__aeabi_dmul+0x248>
   15218:	e1926603 	orrs	r6, r2, r3, lsl #12
   1521c:	11a00002 	movne	r0, r2
   15220:	11a01003 	movne	r1, r3
   15224:	1a000005 	bne	15240 <__aeabi_dmul+0x260>
   15228:	e0211003 	eor	r1, r1, r3
   1522c:	e2011102 	and	r1, r1, #-2147483648	; 0x80000000
   15230:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   15234:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
   15238:	e3a00000 	mov	r0, #0	; 0x0
   1523c:	e8bd8070 	pop	{r4, r5, r6, pc}
   15240:	e381147f 	orr	r1, r1, #2130706432	; 0x7f000000
   15244:	e381173e 	orr	r1, r1, #16252928	; 0xf80000
   15248:	e8bd8070 	pop	{r4, r5, r6, pc}

0001524c <__aeabi_ddiv>:
   1524c:	e92d4070 	push	{r4, r5, r6, lr}
   15250:	e3a0c0ff 	mov	ip, #255	; 0xff
   15254:	e38ccc07 	orr	ip, ip, #1792	; 0x700
   15258:	e01c4a21 	ands	r4, ip, r1, lsr #20
   1525c:	101c5a23 	andsne	r5, ip, r3, lsr #20
   15260:	1134000c 	teqne	r4, ip
   15264:	1135000c 	teqne	r5, ip
   15268:	0b00005c 	bleq	153e0 <__aeabi_ddiv+0x194>
   1526c:	e0444005 	sub	r4, r4, r5
   15270:	e021e003 	eor	lr, r1, r3
   15274:	e1925603 	orrs	r5, r2, r3, lsl #12
   15278:	e1a01601 	lsl	r1, r1, #12
   1527c:	0a00004b 	beq	153b0 <__aeabi_ddiv+0x164>
   15280:	e1a03603 	lsl	r3, r3, #12
   15284:	e3a05201 	mov	r5, #268435456	; 0x10000000
   15288:	e1853223 	orr	r3, r5, r3, lsr #4
   1528c:	e1833c22 	orr	r3, r3, r2, lsr #24
   15290:	e1a02402 	lsl	r2, r2, #8
   15294:	e1855221 	orr	r5, r5, r1, lsr #4
   15298:	e1855c20 	orr	r5, r5, r0, lsr #24
   1529c:	e1a06400 	lsl	r6, r0, #8
   152a0:	e20e1102 	and	r1, lr, #-2147483648	; 0x80000000
   152a4:	e1550003 	cmp	r5, r3
   152a8:	01560002 	cmpeq	r6, r2
   152ac:	e2a440fd 	adc	r4, r4, #253	; 0xfd
   152b0:	e2844c03 	add	r4, r4, #768	; 0x300
   152b4:	2a000001 	bcs	152c0 <__aeabi_ddiv+0x74>
   152b8:	e1b030a3 	lsrs	r3, r3, #1
   152bc:	e1a02062 	rrx	r2, r2
   152c0:	e0566002 	subs	r6, r6, r2
   152c4:	e0c55003 	sbc	r5, r5, r3
   152c8:	e1b030a3 	lsrs	r3, r3, #1
   152cc:	e1a02062 	rrx	r2, r2
   152d0:	e3a00601 	mov	r0, #1048576	; 0x100000
   152d4:	e3a0c702 	mov	ip, #524288	; 0x80000
   152d8:	e056e002 	subs	lr, r6, r2
   152dc:	e0d5e003 	sbcs	lr, r5, r3
   152e0:	20466002 	subcs	r6, r6, r2
   152e4:	21a0500e 	movcs	r5, lr
   152e8:	2180000c 	orrcs	r0, r0, ip
   152ec:	e1b030a3 	lsrs	r3, r3, #1
   152f0:	e1a02062 	rrx	r2, r2
   152f4:	e056e002 	subs	lr, r6, r2
   152f8:	e0d5e003 	sbcs	lr, r5, r3
   152fc:	20466002 	subcs	r6, r6, r2
   15300:	21a0500e 	movcs	r5, lr
   15304:	218000ac 	orrcs	r0, r0, ip, lsr #1
   15308:	e1b030a3 	lsrs	r3, r3, #1
   1530c:	e1a02062 	rrx	r2, r2
   15310:	e056e002 	subs	lr, r6, r2
   15314:	e0d5e003 	sbcs	lr, r5, r3
   15318:	20466002 	subcs	r6, r6, r2
   1531c:	21a0500e 	movcs	r5, lr
   15320:	2180012c 	orrcs	r0, r0, ip, lsr #2
   15324:	e1b030a3 	lsrs	r3, r3, #1
   15328:	e1a02062 	rrx	r2, r2
   1532c:	e056e002 	subs	lr, r6, r2
   15330:	e0d5e003 	sbcs	lr, r5, r3
   15334:	20466002 	subcs	r6, r6, r2
   15338:	21a0500e 	movcs	r5, lr
   1533c:	218001ac 	orrcs	r0, r0, ip, lsr #3
   15340:	e195e006 	orrs	lr, r5, r6
   15344:	0a00000d 	beq	15380 <__aeabi_ddiv+0x134>
   15348:	e1a05205 	lsl	r5, r5, #4
   1534c:	e1855e26 	orr	r5, r5, r6, lsr #28
   15350:	e1a06206 	lsl	r6, r6, #4
   15354:	e1a03183 	lsl	r3, r3, #3
   15358:	e1833ea2 	orr	r3, r3, r2, lsr #29
   1535c:	e1a02182 	lsl	r2, r2, #3
   15360:	e1b0c22c 	lsrs	ip, ip, #4
   15364:	1affffdb 	bne	152d8 <__aeabi_ddiv+0x8c>
   15368:	e3110601 	tst	r1, #1048576	; 0x100000
   1536c:	1a000006 	bne	1538c <__aeabi_ddiv+0x140>
   15370:	e1811000 	orr	r1, r1, r0
   15374:	e3a00000 	mov	r0, #0	; 0x0
   15378:	e3a0c102 	mov	ip, #-2147483648	; 0x80000000
   1537c:	eaffffd5 	b	152d8 <__aeabi_ddiv+0x8c>
   15380:	e3110601 	tst	r1, #1048576	; 0x100000
   15384:	01811000 	orreq	r1, r1, r0
   15388:	03a00000 	moveq	r0, #0	; 0x0
   1538c:	e254c0fd 	subs	ip, r4, #253	; 0xfd
   15390:	835c0c07 	cmphi	ip, #1792	; 0x700
   15394:	8affff4a 	bhi	150c4 <__aeabi_dmul+0xe4>
   15398:	e055c003 	subs	ip, r5, r3
   1539c:	0056c002 	subseq	ip, r6, r2
   153a0:	01b0c0a0 	lsrseq	ip, r0, #1
   153a4:	e2b00000 	adcs	r0, r0, #0	; 0x0
   153a8:	e0a11a04 	adc	r1, r1, r4, lsl #20
   153ac:	e8bd8070 	pop	{r4, r5, r6, pc}
   153b0:	e20ee102 	and	lr, lr, #-2147483648	; 0x80000000
   153b4:	e18e1621 	orr	r1, lr, r1, lsr #12
   153b8:	e09440ac 	adds	r4, r4, ip, lsr #1
   153bc:	c074500c 	rsbsgt	r5, r4, ip
   153c0:	c1811a04 	orrgt	r1, r1, r4, lsl #20
   153c4:	c8bd8070 	popgt	{r4, r5, r6, pc}
   153c8:	e3811601 	orr	r1, r1, #1048576	; 0x100000
   153cc:	e3a0e000 	mov	lr, #0	; 0x0
   153d0:	e2544001 	subs	r4, r4, #1	; 0x1
   153d4:	eaffff3a 	b	150c4 <__aeabi_dmul+0xe4>
   153d8:	e185e006 	orr	lr, r5, r6
   153dc:	eaffff38 	b	150c4 <__aeabi_dmul+0xe4>
   153e0:	e00c5a23 	and	r5, ip, r3, lsr #20
   153e4:	e134000c 	teq	r4, ip
   153e8:	0135000c 	teqeq	r5, ip
   153ec:	0affff93 	beq	15240 <__aeabi_dmul+0x260>
   153f0:	e134000c 	teq	r4, ip
   153f4:	1a000006 	bne	15414 <__aeabi_ddiv+0x1c8>
   153f8:	e1904601 	orrs	r4, r0, r1, lsl #12
   153fc:	1affff8f 	bne	15240 <__aeabi_dmul+0x260>
   15400:	e135000c 	teq	r5, ip
   15404:	1affff87 	bne	15228 <__aeabi_dmul+0x248>
   15408:	e1a00002 	mov	r0, r2
   1540c:	e1a01003 	mov	r1, r3
   15410:	eaffff8a 	b	15240 <__aeabi_dmul+0x260>
   15414:	e135000c 	teq	r5, ip
   15418:	1a000004 	bne	15430 <__aeabi_ddiv+0x1e4>
   1541c:	e1925603 	orrs	r5, r2, r3, lsl #12
   15420:	0affff6d 	beq	151dc <__aeabi_dmul+0x1fc>
   15424:	e1a00002 	mov	r0, r2
   15428:	e1a01003 	mov	r1, r3
   1542c:	eaffff83 	b	15240 <__aeabi_dmul+0x260>
   15430:	e1906081 	orrs	r6, r0, r1, lsl #1
   15434:	11926083 	orrsne	r6, r2, r3, lsl #1
   15438:	1affff4d 	bne	15174 <__aeabi_dmul+0x194>
   1543c:	e1904081 	orrs	r4, r0, r1, lsl #1
   15440:	1affff78 	bne	15228 <__aeabi_dmul+0x248>
   15444:	e1925083 	orrs	r5, r2, r3, lsl #1
   15448:	1affff63 	bne	151dc <__aeabi_dmul+0x1fc>
   1544c:	eaffff7b 	b	15240 <__aeabi_dmul+0x260>

00015450 <__aeabi_d2iz>:
   15450:	e1a02081 	lsl	r2, r1, #1
   15454:	e2922602 	adds	r2, r2, #2097152	; 0x200000
   15458:	2a00000c 	bcs	15490 <__aeabi_d2iz+0x40>
   1545c:	5a000009 	bpl	15488 <__aeabi_d2iz+0x38>
   15460:	e3e03e3e 	mvn	r3, #992	; 0x3e0
   15464:	e0532ac2 	subs	r2, r3, r2, asr #21
   15468:	9a00000a 	bls	15498 <__aeabi_d2iz+0x48>
   1546c:	e1a03581 	lsl	r3, r1, #11
   15470:	e3833102 	orr	r3, r3, #-2147483648	; 0x80000000
   15474:	e1833aa0 	orr	r3, r3, r0, lsr #21
   15478:	e3110102 	tst	r1, #-2147483648	; 0x80000000
   1547c:	e1a00233 	lsr	r0, r3, r2
   15480:	12600000 	rsbne	r0, r0, #0	; 0x0
   15484:	e12fff1e 	bx	lr
   15488:	e3a00000 	mov	r0, #0	; 0x0
   1548c:	e12fff1e 	bx	lr
   15490:	e1900601 	orrs	r0, r0, r1, lsl #12
   15494:	1a000002 	bne	154a4 <__aeabi_d2iz+0x54>
   15498:	e2110102 	ands	r0, r1, #-2147483648	; 0x80000000
   1549c:	03e00102 	mvneq	r0, #-2147483648	; 0x80000000
   154a0:	e12fff1e 	bx	lr
   154a4:	e3a00000 	mov	r0, #0	; 0x0
   154a8:	e12fff1e 	bx	lr

000154ac <__div0>:
   154ac:	e92d4002 	push	{r1, lr}
   154b0:	e3a00008 	mov	r0, #8	; 0x8
   154b4:	ebfffcaa 	bl	14764 <raise>
   154b8:	e8bd8002 	pop	{r1, pc}
